#ifdef MACRO_REDUCE

	// const ubyte_t *srcptr
	// T			 *dstptr (T == uword_t or T == ulong_t)
	// T			 *dstend
	// ulong_t		  size_x
	// ulong_t		  bsiz_r == bsiz_y * size_x
	// ulong_t		  blks_x

	__asm
	  {
		pxor		mm0, mm0;			// MM0 は常に 0 を保持する.
		mov			esi, srcptr;
		mov			edi, dstptr;
		mov			ecx, size_x;
		// while (dstptr < dstend)
		R0:	mov			edx, esi;		// const ubyte_t* blkptr = srcptr;
			add			esi, bsiz_r;	// srcptr += bsiz_r;
			mov			eax, blks_x;
			#if SCAL_X < 3				// T* rowend = dstptr + blks_x;
				lea			ebx, [edi + eax * 2 + 1 - (16 >> SCAL_X)];
				// このときは, R1 からのループで複数ブロックを処理することに
				// なるので, 右端がはみ出さないようにチェックしておく.
				// LEA 命令の -7 も同じ理由による.
				cmp			edi, ebx;
				jae			R4;
			#elif SCAL_X == 3
				lea			ebx, [edi + eax * 2];
			#else
				lea			ebx, [edi + eax * 4];
			#endif
			// while (dstptr < rowend)
			R1:	mov			eax, edx;	// const ubyte_t* rasptr = blkptr;
				#if SCAL_X <= 3			// blkptr += bsiz_x <= 8 ? 8 : bsiz_x;
					add			edx, 8;
				#else
					add			edx, 1 << SCAL_X;
				#endif
				movq		mm4, mm0;
				movq		mm5, mm0;
				// while (rasptr < srcptr)
				R2:	movq		mm2, [eax+0];
					#if SCAL_X <= 3
						add			eax, ecx;
					#endif
					movq		mm3, mm2;
					punpcklbw	mm2, mm0;
					punpckhbw	mm3, mm0;
					#if SCAL_X <= 3
						cmp			eax, esi;
					#endif
					paddw		mm4, mm2;
					paddw		mm5, mm3;
					#if SCAL_X > 3
						movq		mm2, [eax+8];
						#if SCAL_X == 4
							add			eax, ecx;
						#endif
						movq		mm3, mm2;
						punpcklbw	mm2, mm0;
						punpckhbw	mm3, mm0;
						#if SCAL_X == 4
							cmp			eax, esi;
						#endif
						paddw		mm4, mm2;
						paddw		mm5, mm3;
						#if SCAL_X > 4
							movq		mm2, [eax+16];
							movq		mm3, mm2;
							punpcklbw	mm2, mm0;
							punpckhbw	mm3, mm0;
							paddw		mm4, mm2;
							paddw		mm5, mm3;
							movq		mm2, [eax+24];
							add			eax, ecx;
							movq		mm3, mm2;
							punpcklbw	mm2, mm0;
							punpckhbw	mm3, mm0;
							cmp			eax, esi;
							paddw		mm4, mm2;
							paddw		mm5, mm3;
						#endif
					#endif
				jb			R2;

				#if SCAL_X == 0
					movq		[edi+0], mm4;
					movq		[edi+8], mm5;
					add			edi, 16;
				#elif SCAL_X == 1
					movq		mm2, mm4;		// 4:aabbccdd 5:eeffgghh
					punpcklwd	mm4, mm5;		// 4:aaeebbff
					punpckhwd	mm2, mm5;		//            2:ccggddhh
					movq		mm5, mm4;		//            5:aaeebbff
					punpcklwd	mm4, mm2;		// 4:aacceegg
					punpckhwd	mm5, mm2;		//            5:bbddffhh
					paddw		mm4, mm5;		// 4:abcdefgh
					movq		[edi], mm4;
					add			edi, 8;
				#elif SCAL_X == 2
					movq		mm2, mm4;		// 4:aabbccdd 5:eeffgghh
					punpcklwd	mm4, mm5;		// 4:aaeebbff
					punpckhwd	mm2, mm5;		//            2:ccggddhh
					paddw		mm4, mm2;		// 4:acegbdfh
					movq		mm5, mm4;		//            5:acegbdfh
					punpckhdq	mm5, mm0;		//            5:bdfh0000
					paddw		mm4, mm5;		// 4:adehxxxx
					movd		[edi], mm4;
					add			edi, 4;
				#else
					// bsiz_x >= 16 のとき, ピクセル値の和は最大 17 ビット以上となるが,
					// mm4:mm5 の各ワードに 8 分割されているので, まだまだ余裕がある.
					// つまり, いきなり mm4:mm5 を加算しても, オーバフローの心配はない.
					paddw		mm4, mm5;
					movq		mm5, mm4;		// 4:aabbccdd 5:aabbccdd
					punpcklwd	mm4, mm0;		// 4:aa00bb00
					punpckhwd	mm5, mm0;		//            5:cc00dd00
					paddd		mm4, mm5;		// 4:ac00bd00
					movq		mm5, mm4;		//            5:ac00bd00
					punpckhdq	mm5, mm0;		//            5:bd000000
					paddd		mm4, mm5;		// 4:ad000000
					#if SCAL_X == 3
						movd		eax, mm4;
						mov			[edi], ax;
						add			edi, 2;
					#else
						// このときは, ulong_t の配列である.
						movd		[edi], mm4;
						add			edi, 4;
					#endif
				#endif
				cmp			edi, ebx;
			jb			R1;

			#if SCAL_X < 3
			R4:	add			ebx, (16 >> SCAL_X) - 1;
				cmp			edi, ebx;
				jae			R7;
				#if SCAL_X < 2
					// 汎用レジスタが足りない!
					push		ebx;
				#endif
				R5:	mov			eax, edx;
					#if SCAL_X == 0
						inc			edx;
					#else
						add			edx, 1 << SCAL_X;
					#endif
					pxor		mm4, mm4;
					#if SCAL_X == 0
						R6:	movzx		ebx, BYTE PTR [eax];
							add			eax, ecx;
							movd		mm2, ebx;
							paddw		mm4, mm2;
							cmp			eax, esi;
						jb			R6;
						movd		eax, mm4;
						mov			[edi], ax;
						add			edi, 2;
						cmp			edi, [esp];
					#elif SCAL_X == 1
						R6:	movzx		ebx, WORD PTR [eax];
							add			eax, ecx;
							movd		mm2, ebx;
							punpcklbw	mm2, mm0;
							paddw		mm4, mm2;
							cmp			eax, esi;
						jb			R6;
						movq		mm5, mm4;
						punpckhwd	mm4, mm0;
						paddw		mm4, mm5;
						movd		eax, mm4;
						mov			[edi], ax;
						add			edi, 2;
						cmp			edi, [esp];
					#else
						R6:	movd		mm2, [eax];
							add			eax, ecx;
							punpcklbw	mm2, mm0;
							paddw		mm4, mm2;
							cmp			eax, esi;
						jb			R6;
						movq		mm5, mm4;	// 4:aabbccdd 5:aabbccdd
						punpcklwd	mm4, mm0;	// 4:aa00bb00
						punpckhwd	mm5, mm0;	//            5:cc00dd00
						paddd		mm4, mm5;	// 4:ac00bd00
						movq		mm5, mm4;
						punpckhdq	mm4, mm0;	// 4:bd000000
						paddd		mm4, mm5;
						movd		eax, mm4;
						mov			[edi], ax;
						add			edi, 2;
						cmp			edi, ebx;
					#endif
				jb			R5;
				#if SCAL_X < 2
					pop			ebx;			// 別に EBX でなくてもよい.
				#endif
			R7:
			#endif

			cmp			edi, dstend;
		jb			R0;
	  }

#endif // MACRO_REDUCE

#ifdef MACRO_EXPAND_X

	// const T*		  srcptr,
	// const T*		  srcend,
	// ulong_t*		  dstptr,
	// ulong_t*		  dstend,
	// const ulong_t* basptr,
	// ulong_t		  bsiz_x,
	// ulong_t		  bsiz_y,
	// bool			  diff)

	// src には blks_x = size_x / bsiz_x 個分のデータが含まれる.
	// size_x % bsiz_x != 0 のとき, 右側の半端なピクセルには, すべて最
	// 後のデータを埋めることにした.

	#if SCAL_X == 0

		__asm
		  {
			mov			esi, srcptr;
			mov			edx, srcend;
			mov			edi, dstptr;
			pxor		mm0, mm0;
			#if DIFF != 0
				mov			ebx, basptr;
			#endif
			bsf			ecx, bsiz_y;
			movd		mm7, ecx;

			sub			edx, 7;
			cmp			esi, edx;
			jae			X1;

			X0:	movq		mm2, [esi];
				add			esi, 8;
				movq		mm3, mm2;
				punpcklwd	mm2, mm0;
				punpckhwd	mm3, mm0;
				#if DIFF == 0
					pslld		mm2, mm7;
					pslld		mm3, mm7;
				#else
					movq		mm4, [ebx + 0];
					movq		mm5, [ebx + 8];
					psrld		mm4, mm7;			// *basptr++ >> scal_y.
					add			ebx, 16;
					psrld		mm5, mm7;
					psubd		mm2, mm4;
					psubd		mm3, mm5;
				#endif
				movq		[edi + 0], mm2;
				movq		[edi + 8], mm3;
				add			edi, 16;
				cmp			esi, edx;
			jb			X0;

		X1:	add			edx, 7;
			cmp			esi, srcend;
			jae			X3;

			X2:	movzx		eax, WORD PTR [esi];
				add			esi, 2;
				#if DIFF == 0
					shl			eax, cl;
					mov			[edi], eax;
				#else
					movd		mm2, eax;
					movd		mm4, [ebx];
					psrld		mm4, mm7;
					add			ebx, 4;
					psubd		mm2, mm4;
					movd		[edi], mm2;
				#endif
				add			edi, 4;
				cmp			esi, edx;
			jb			X2;

			// BSIZ_X == 1 (SCAL_X == 0) のとき, 右端に半端なピクセルはあり得ない.
		X3:	mov			srcptr, esi;
		  }

	#elif SCAL_X <= 3

	  {
		std::size_t scal_y = iscalb(bsiz_y);
		ulong_t s0 = *srcptr++ << (!DIFF ? scal_y : 0), ss = s0 << SCAL_X;

		// 左端.
		for (ulong_t x = 0; x < 1 << (SCAL_X - 1); ++x)
			#if DIFF == 0
				*dstptr++ = ss;
			#else
				*dstptr++ = ss - (*basptr++ >> scal_y);
			#endif

		// 本体 (blks_x - 1) * bsiz_x ピクセル.
		while (srcptr < srcend)
		  {
			ulong_t s1 = *srcptr++ << (!DIFF ? scal_y : 0);
			ulong_t ds = s1 - s0;
			s0 = s1;
			for (ulong_t x = 0; x < 1 << SCAL_X; ++x)
				#if DIFF == 0
					*dstptr++ = ss, ss += ds;
				#else
					*dstptr++ = ss - (*basptr++ >> scal_y), ss += ds;
				#endif
		  }

		// 右端.
		while (dstptr < dstend)
			#if DIFF == 0
				*dstptr++ = ss;
			#else
				*dstptr++ = ss - (*basptr++ >> scal_y);
			#endif
	  }

	#else

		__asm
		  {
			mov			esi, srcptr;
			mov			edx, srcend;
			mov			edi, dstptr;
			#if DIFF != 0
				mov			ebx, basptr;
			#endif
			bsf			ecx, bsiz_y;
			#if DIFF == 0
				// DIFF == 0 のときは, MM7 に Y 方向のシフトカウントを入れておく.
				movd		mm7, ecx;
			#endif

			movd		mm4, [esi];
			add			esi, 4;
			#if DIFF == 0
				// MM0 に常にひとつ前のデータを入れておき,
				// 次のデータとの差を計算するのに使用する.
				// DIFF != 0 のときは, MMX レジスタに空きがないため,
				// その都度読み直す必要がある. (多分キャッシュに残っているハズ.)
				pslld		mm4, mm7;
				movq		mm0, mm4;
			#endif
			punpckldq	mm4, mm4;
			pslld		mm4, SCAL_X;

			// 左端.
			#if DIFF == 0
				movq		[edi +  0], mm4;
				movq		[edi +  8], mm4;
				movq		[edi + 16], mm4;
				movq		[edi + 24], mm4;
				#if SCAL_X >= 5
					movq		[edi + 32], mm4;
					movq		[edi + 40], mm4;
					movq		[edi + 48], mm4;
					movq		[edi + 56], mm4;
				#endif
			#else
				movd		mm0, ecx;
				movq		mm6, [ebx + 0];
				add			ebx, 4 << (SCAL_X - 1);
				movq		mm2, mm4;
				psrld		mm6, mm0;
				psubd		mm2, mm6;
				movq		[edi +  0], mm2;
				movq		[edi +  8], mm2;
				movq		[edi + 16], mm2;
				movq		[edi + 24], mm2;
				#if SCAL_X >= 5
					movq		[edi + 32], mm2;
					movq		[edi + 40], mm2;
					movq		[edi + 48], mm2;
					movq		[edi + 56], mm2;
				#endif
			#endif
			add			edi, 4 << (SCAL_X - 1);

			// 本体 (blks_x - 1) * bsiz_x ピクセル.
			cmp			esi, edx;
			jae			X1;
			X0:
				#if DIFF != 0
					movd	mm0, [esi - 4];
				#endif
				movd		mm1, [esi];
				#if DIFF == 0
					pslld		mm1, mm7;
				#endif
				add			esi, 4;
				movq		mm3, mm1;
				punpckldq	mm4, mm4;			// s0 * bsiz_x : s0 * bsiz_x
				psubd		mm1, mm0;			// ds (= s1 - s0)
				pxor		mm0, mm0;
				punpckldq	mm0, mm1;			// ds : 0
				punpckldq	mm1, mm1;			// ds : ds
				paddd		mm4, mm0;			// s0 * bsiz_x + 1ds : + 0ds
				paddd		mm1, mm1;			// 2ds : 2ds
				movq		mm5, mm4;
				paddd		mm5, mm1;			// s0 * bsiz_x + 3ds : + 2ds
				paddd		mm1, mm1;			// 4ds : 4ds
				#if DIFF == 0
					movq		mm0, mm3;
				#else
					movd		mm0, ecx;
				#endif

				#if DIFF == 0
					movq		[edi +  0], mm4;
					paddd		mm4, mm1;
					movq		[edi +  8], mm5;
					paddd		mm5, mm1;
					movq		[edi + 16], mm4;
					paddd		mm4, mm1;
					movq		[edi + 24], mm5;
					paddd		mm5, mm1;
					movq		[edi + 32], mm4;
					paddd		mm4, mm1;
					movq		[edi + 40], mm5;
					paddd		mm5, mm1;
					movq		[edi + 48], mm4;
					paddd		mm4, mm1;
					movq		[edi + 56], mm5;
					#if SCAL_X >= 5
						paddd		mm5, mm1;
						movq		[edi + 64], mm4;
						paddd		mm4, mm1;
						movq		[edi + 72], mm5;
						paddd		mm5, mm1;
						movq		[edi + 80], mm4;
						paddd		mm4, mm1;
						movq		[edi + 88], mm5;
						paddd		mm5, mm1;
						movq		[edi + 96], mm4;
						paddd		mm4, mm1;
						movq		[edi + 104], mm5;
						paddd		mm5, mm1;
						movq		[edi + 112], mm4;
						paddd		mm4, mm1;
						movq		[edi + 120], mm5;
					#endif
				#else
					movq		mm6, [ebx +  0];
					movq		mm2, mm4;
					psrld		mm6, mm0;
					movq		mm7, [ebx +  8];
					movq		mm3, mm5;
					psrld		mm7, mm0;
					psubd		mm2, mm6;
					psubd		mm3, mm7;
					movq		[edi +  0], mm2;
					paddd		mm4, mm1;
					movq		[edi +  8], mm3;
					paddd		mm5, mm1;

					movq		mm6, [ebx + 16];
					movq		mm2, mm4;
					psrld		mm6, mm0;
					movq		mm7, [ebx + 24];
					movq		mm3, mm5;
					psrld		mm7, mm0;
					psubd		mm2, mm6;
					psubd		mm3, mm7;
					movq		[edi + 16], mm2;
					paddd		mm4, mm1;
					movq		[edi + 24], mm3;
					paddd		mm5, mm1;

					movq		mm6, [ebx + 32];
					movq		mm2, mm4;
					psrld		mm6, mm0;
					movq		mm7, [ebx + 40];
					movq		mm3, mm5;
					psrld		mm7, mm0;
					psubd		mm2, mm6;
					psubd		mm3, mm7;
					movq		[edi + 32], mm2;
					paddd		mm4, mm1;
					movq		[edi + 40], mm3;
					paddd		mm5, mm1;

					movq		mm6, [ebx + 48];
					movq		mm2, mm4;
					psrld		mm6, mm0;
					movq		mm7, [ebx + 56];
					movq		mm3, mm5;
					psrld		mm7, mm0;
					psubd		mm2, mm6;
					psubd		mm3, mm7;
					movq		[edi + 48], mm2;
					paddd		mm4, mm1;
					movq		[edi + 56], mm3;

					#if SCAL_X >= 5
						paddd		mm5, mm1;

						movq		mm6, [ebx + 64];
						movq		mm2, mm4;
						psrld		mm6, mm0;
						movq		mm7, [ebx + 72];
						movq		mm3, mm5;
						psrld		mm7, mm0;
						psubd		mm2, mm6;
						psubd		mm3, mm7;
						movq		[edi + 64], mm2;
						paddd		mm4, mm1;
						movq		[edi + 72], mm3;
						paddd		mm5, mm1;

						movq		mm6, [ebx + 80];
						movq		mm2, mm4;
						psrld		mm6, mm0;
						movq		mm7, [ebx + 88];
						movq		mm3, mm5;
						psrld		mm7, mm0;
						psubd		mm2, mm6;
						psubd		mm3, mm7;
						movq		[edi + 80], mm2;
						paddd		mm4, mm1;
						movq		[edi + 88], mm3;
						paddd		mm5, mm1;

						movq		mm6, [ebx + 96];
						movq		mm2, mm4;
						psrld		mm6, mm0;
						movq		mm7, [ebx + 104];
						movq		mm3, mm5;
						psrld		mm7, mm0;
						psubd		mm2, mm6;
						psubd		mm3, mm7;
						movq		[edi + 96], mm2;
						paddd		mm4, mm1;
						movq		[edi + 104], mm3;
						paddd		mm5, mm1;

						movq		mm6, [ebx + 112];
						movq		mm2, mm4;
						psrld		mm6, mm0;
						movq		mm7, [ebx + 120];
						movq		mm3, mm5;
						psrld		mm7, mm0;
						psubd		mm2, mm6;
						psubd		mm3, mm7;
						movq		[edi + 112], mm2;
						paddd		mm4, mm1;
						movq		[edi + 120], mm3;
					#endif
					add			ebx, 4 << SCAL_X;
				#endif
				add			edi, 4 << SCAL_X;
				cmp			esi, edx;
				punpckldq	mm4, mm4;
			jb			X0;

			// 右端. 少なくとも bsiz_x / 2 (>= 8) 個以上は残っていることに注意.
		X1:	mov			edx, dstend;
			sub			edx, 15;
			cmp			edi, edx;
			jae			X3;
			#if DIFF != 0
				movq		mm6, [ebx];
				psrld		mm6, mm0;		// MM0 には scal_y が入っている.
				psubd		mm4, mm6;
			#endif
			X2:	movq		[edi + 0], mm4;
				movq		[edi + 8], mm4;
				add			edi, 16;
				cmp			edi, edx;
			jb			X2;
		X3:	add			edx, 15;
			cmp			edi, edx;
			jae			X5;
			X4:	movd		[edi], mm4;
				add			edi, 4;
				cmp			edi, edx;
			jb			X4;
		X5:	mov			srcptr, esi;
		  }

	#endif
	return srcptr;

#endif // MACRO_EXPAND_X

#ifdef MACRO_EXPAND

	//	const T*	   srcptr,
	//	const T*	   srcend,
	//	ubyte_t*	   dstptr,
	//	ubyte_t*	   dstend,
	//	ulong_t*	   tm0ptr,
	//	ulong_t		   size_x,
	//	ulong_t		   bsiz_x,
	//	ulong_t		   bsiz_y,
	//	ulong_t		   blks_x)
	  {
	ulong_t* tm1ptr = tm0ptr + size_x;
	std::size_t scale_y = iscalb(bsiz_y);

	// bsiz_x, bsiz_y <= 2^5 のとき, *srcptr の最大値は 2^8 * 2^5 *
	// 2^5 = 2^18 未満であるから, 18 ビットに収まっている.
	//
	// x, y 方向への線形補間の際, それぞれ最大 bsiz_x, bsiz_y 倍するた
	// め, 途中の結果は 2^18 * 2^5 * 2^5 = 2^28, つまり 28 ビットまで使
	// 用する可能性がある. 32-bit 符号付きデータとしては, あと 3 ビッ
	// トしか余裕がないので, 注意すること.

	// 上端.
	srcptr = CAT(expand_x_, SCAL_X)(
		srcptr, srcptr + blks_x,
		tm0ptr, tm0ptr + size_x, bsiz_y);
	for (ulong_t y = 0; y < bsiz_y / 2; ++y)
		for (ulong_t x = 0; x < size_x; ++x)
			*dstptr++ = ubyte_t(tm0ptr[x] >> 2 * (SCAL_X + scale_y));

	// 本体.
	while (srcptr < srcend)
	  {
		srcptr = CAT(expand_xdiff_, SCAL_X)(
			srcptr, srcptr + blks_x,
			tm1ptr, tm1ptr + size_x, tm0ptr, bsiz_y);
		dstptr = expand_xy(dstptr, tm0ptr, tm1ptr, size_x, bsiz_y, 2 * (SCAL_X + scale_y));
	  }

	// 下端.
	while (dstptr < dstend)
		for (ulong_t x = 0; x < size_x; ++x)
			*dstptr++ = ubyte_t(tm0ptr[x] >> 2 * (SCAL_X + scale_y));

	#if TEST > 1
		std::cout << std::endl;
	#endif
	  }

#endif // MACRO_EXPAND

#ifndef MACRO
#define MACRO "ShdCmp_LPF.cpp"

#include <memory>
#ifdef TEST
	#include <iomanip>
	#include <iostream>
	#include <cstdlib>
	#ifdef _MSC_VER
		namespace std { using ::rand; }
	#endif
#endif
#include "ShdCmp_NSimg.h"

#define CAT2(x, y)	x##y
#define CAT(x, y)	CAT2(x, y)

#pragma warning(disable: 4799) // "No EMMS instruction."

#define MACRO_REDUCE

static void __stdcall reduce_0(
	const ubyte_t *srcptr,
	uword_t		  *dstptr,
	uword_t		  *dstend,
	ulong_t		   size_x,
	ulong_t		   bsiz_r,
	ulong_t		   blks_x) throw()
{
	#define SCAL_X 0
	#include MACRO
	#undef SCAL_X
}

static void __stdcall reduce_1(
	const ubyte_t* srcptr,
	uword_t*	   dstptr,
	uword_t*	   dstend,
	ulong_t		   size_x,
	ulong_t		   bsiz_r,
	ulong_t		   blks_x) throw()
{
	#define SCAL_X 1
	#include MACRO
	#undef SCAL_X
}

static void __stdcall reduce_2(
	const ubyte_t* srcptr,
	uword_t*	   dstptr,
	uword_t*	   dstend,
	ulong_t		   size_x,
	ulong_t		   bsiz_r,
	ulong_t		   blks_x) throw()
{
	#define SCAL_X 2
	#include MACRO
	#undef SCAL_X
}

static void __stdcall reduce_3(
	const ubyte_t* srcptr,
	uword_t*	   dstptr,
	uword_t*	   dstend,
	ulong_t		   size_x,
	ulong_t		   bsiz_r,
	ulong_t		   blks_x) throw()
{
	#define SCAL_X 3
	#include MACRO
	#undef SCAL_X
}

static void __stdcall reduce_4(
	const ubyte_t* srcptr,
	ulong_t*	   dstptr,
	ulong_t*	   dstend,
	ulong_t		   size_x,
	ulong_t		   bsiz_r,
	ulong_t		   blks_x) throw()
{
	#define SCAL_X 4
	#include MACRO
	#undef SCAL_X
}

static void __stdcall reduce_5(
	const ubyte_t* srcptr,
	ulong_t*	   dstptr,
	ulong_t*	   dstend,
	ulong_t		   size_x,
	ulong_t		   bsiz_r,
	ulong_t		   blks_x) throw()
{
	#define SCAL_X 5
	#include MACRO
	#undef SCAL_X
}

#undef MACRO_REDUCE

static ubyte_t* __stdcall expand_xy(
	ubyte_t*	   dstptr,
	ulong_t*	   srcptr,
	const ulong_t* difptr,
	ulong_t		   size_x,
	ulong_t		   bsiz_y,
	ulong_t		   scale) throw()
{
	__asm
	  {
		mov			edi, dstptr;
		mov			ecx, scale;
		movd		mm7, ecx;

		L0:	mov			ebx, srcptr;
			mov			esi, difptr;
			mov			eax, size_x;
			sub			eax, 8;
			jb			L2;

			// 以下のレジスタの使用ポリシーは, レジスタが足りないため少々わかりにくい.
			// 最初の組は MM4:MM5, 次の組は MM5:MM6 をペアとして使用している.
			L1:	movq		mm4, [ebx + 0];
				movq		mm5, [ebx + 8];
				movq		mm0, mm4;
				psrld		mm4, mm7;
				movq		mm2, [esi + 0];
				movq		mm1, mm5;
				psrld		mm5, mm7;
				movq		mm3, [esi + 8];
				paddd		mm0, mm2;
				paddd		mm1, mm3;
				packssdw	mm4, mm5;
				movq		[ebx + 0], mm0;
				movq		[ebx + 8], mm1;
				movq		mm5, [ebx + 16];
				movq		mm6, [ebx + 24];
				movq		mm0, mm5;
				psrld		mm5, mm7;
				movq		mm2, [esi + 16];
				movq		mm1, mm6;
				psrld		mm6, mm7;
				movq		mm3, [esi + 24];
				paddd		mm0, mm2;
				paddd		mm1, mm3;
				packssdw	mm5, mm6;
				movq		[ebx + 16], mm0;
				movq		[ebx + 24], mm1;
				packuswb	mm4, mm5;
				add			ebx, 32;
				add			esi, 32;
				movq		[edi], mm4;
				add			edi, 8;
				sub			eax, 8;
			jae			L1;

		L2:	add			eax, 8;
			jz			L4;
			mov			ch, al;
			L3:	mov			edx, [ebx];
				mov			eax, edx;
				shr			edx, cl;
				add			eax, [esi];
				add			esi, 4;
				mov			[edi], dl;
				inc			edi;
				mov			[ebx], eax;
				add			ebx, 4;
				dec			ch;
			jnz			L3;
		L4:	dec			bsiz_y;

		jnz			L0;
		mov			dstptr, edi;
	  }
	return dstptr;
}

__declspec(naked) static ulong_t __fastcall iscalb(ulong_t) throw()
{
	__asm
	  {
		bsf		eax, ecx;
		ret;
	  }
}

#define MACRO_EXPAND_X

static const uword_t* __stdcall expand_x_0(
	const uword_t* srcptr,
	const uword_t* srcend,
	ulong_t*	   dstptr,
	ulong_t*	   dstend,
	ulong_t		   bsiz_y) throw()
{
	#define SCAL_X	0
	#define DIFF	0
	#include MACRO
	#undef SCAL_X
	#undef DIFF
}

static const uword_t* __stdcall expand_xdiff_0(
	const uword_t* srcptr,
	const uword_t* srcend,
	ulong_t*	   dstptr,
	ulong_t*	   dstend,
	const ulong_t* basptr,
	ulong_t		   bsiz_y) throw()
{
	#define SCAL_X	0
	#define DIFF	1
	#include MACRO
	#undef SCAL_X
	#undef DIFF
}

static const uword_t* __stdcall expand_x_1(
	const uword_t* srcptr,
	const uword_t* srcend,
	ulong_t*	   dstptr,
	ulong_t*	   dstend,
	ulong_t		   bsiz_y) throw()
{
	#define SCAL_X	1
	#define DIFF	0
	#include MACRO
	#undef SCAL_X
	#undef DIFF
}

static const uword_t* __stdcall expand_xdiff_1(
	const uword_t* srcptr,
	const uword_t* srcend,
	ulong_t*	   dstptr,
	ulong_t*	   dstend,
	const ulong_t* basptr,
	ulong_t		   bsiz_y) throw()
{
	#define SCAL_X	1
	#define DIFF	1
	#include MACRO
	#undef SCAL_X
	#undef DIFF
}

static const uword_t* __stdcall expand_x_2(
	const uword_t* srcptr,
	const uword_t* srcend,
	ulong_t*	   dstptr,
	ulong_t*	   dstend,
	ulong_t		   bsiz_y) throw()
{
	#define SCAL_X	2
	#define DIFF	0
	#include MACRO
	#undef SCAL_X
	#undef DIFF
}

static const uword_t* __stdcall expand_xdiff_2(
	const uword_t* srcptr,
	const uword_t* srcend,
	ulong_t*	   dstptr,
	ulong_t*	   dstend,
	const ulong_t* basptr,
	ulong_t		   bsiz_y) throw()
{
	#define SCAL_X	2
	#define DIFF	1
	#include MACRO
	#undef SCAL_X
	#undef DIFF
}

static const uword_t* __stdcall expand_x_3(
	const uword_t* srcptr,
	const uword_t* srcend,
	ulong_t*	   dstptr,
	ulong_t*	   dstend,
	ulong_t		   bsiz_y) throw()
{
	#define SCAL_X	3
	#define DIFF	0
	#include MACRO
	#undef SCAL_X
	#undef DIFF
}

static const uword_t* __stdcall expand_xdiff_3(
	const uword_t* srcptr,
	const uword_t* srcend,
	ulong_t*	   dstptr,
	ulong_t*	   dstend,
	const ulong_t* basptr,
	ulong_t		   bsiz_y) throw()
{
	#define SCAL_X	3
	#define DIFF	1
	#include MACRO
	#undef SCAL_X
	#undef DIFF
}

static const ulong_t* __stdcall expand_x_4(
	const ulong_t* srcptr,
	const ulong_t* srcend,
	ulong_t*	   dstptr,
	ulong_t*	   dstend,
	ulong_t		   bsiz_y) throw()
{
	#define SCAL_X	4
	#define DIFF	0
	#include MACRO
	#undef SCAL_X
	#undef DIFF
}

static const ulong_t* __stdcall expand_xdiff_4(
	const ulong_t* srcptr,
	const ulong_t* srcend,
	ulong_t*	   dstptr,
	ulong_t*	   dstend,
	const ulong_t* basptr,
	ulong_t		   bsiz_y) throw()
{
	#define SCAL_X	4
	#define DIFF	1
	#include MACRO
	#undef SCAL_X
	#undef DIFF
}

static const ulong_t* __stdcall expand_x_5(
	const ulong_t* srcptr,
	const ulong_t* srcend,
	ulong_t*	   dstptr,
	ulong_t*	   dstend,
	ulong_t		   bsiz_y) throw()
{
	#define SCAL_X	5
	#define DIFF	0
	#include MACRO
	#undef SCAL_X
	#undef DIFF
}

static const ulong_t* __stdcall expand_xdiff_5(
	const ulong_t* srcptr,
	const ulong_t* srcend,
	ulong_t*	   dstptr,
	ulong_t*	   dstend,
	const ulong_t* basptr,
	ulong_t		   bsiz_y) throw()
{
	#define SCAL_X	5
	#define DIFF	1
	#include MACRO
	#undef SCAL_X
	#undef DIFF
}

#undef MACRO_EXPAND_X

#define MACRO_EXPAND

static void __stdcall expand_0(
	const uword_t* srcptr,
	const uword_t* srcend,
	ubyte_t*	   dstptr,
	ubyte_t*	   dstend,
	ulong_t*	   tm0ptr,
	ulong_t		   size_x,
	ulong_t		   bsiz_y,
	ulong_t		   blks_x) throw()
{
	#define SCAL_X	0
	#include MACRO
	#undef SCAL_X
}

static void __stdcall expand_1(
	const uword_t* srcptr,
	const uword_t* srcend,
	ubyte_t*	   dstptr,
	ubyte_t*	   dstend,
	ulong_t*	   tm0ptr,
	ulong_t		   size_x,
	ulong_t		   bsiz_y,
	ulong_t		   blks_x) throw()
{
	#define SCAL_X	1
	#include MACRO
	#undef SCAL_X
}

static void __stdcall expand_2(
	const uword_t* srcptr,
	const uword_t* srcend,
	ubyte_t*	   dstptr,
	ubyte_t*	   dstend,
	ulong_t*	   tm0ptr,
	ulong_t		   size_x,
	ulong_t		   bsiz_y,
	ulong_t		   blks_x) throw()
{
	#define SCAL_X	2
	#include MACRO
	#undef SCAL_X
}

static void __stdcall expand_3(
	const uword_t* srcptr,
	const uword_t* srcend,
	ubyte_t*	   dstptr,
	ubyte_t*	   dstend,
	ulong_t*	   tm0ptr,
	ulong_t		   size_x,
	ulong_t		   bsiz_y,
	ulong_t		   blks_x) throw()
{
	#define SCAL_X	3
	#include MACRO
	#undef SCAL_X
}

static void __stdcall expand_4(
	const ulong_t* srcptr,
	const ulong_t* srcend,
	ubyte_t*	   dstptr,
	ubyte_t*	   dstend,
	ulong_t*	   tm0ptr,
	ulong_t		   size_x,
	ulong_t		   bsiz_y,
	ulong_t		   blks_x) throw()
{
	#define SCAL_X	4
	#include MACRO
	#undef SCAL_X
}

static void __stdcall expand_5(
	const ulong_t* srcptr,
	const ulong_t* srcend,
	ubyte_t*	   dstptr,
	ubyte_t*	   dstend,
	ulong_t*	   tm0ptr,
	ulong_t		   size_x,
	ulong_t		   bsiz_y,
	ulong_t		   blks_x) throw()
{
	#define SCAL_X	5
	#include MACRO
	#undef SCAL_X
}

#undef MACRO_EXPAND

#pragma warning(default: 4799)

void __stdcall LPF(
	const ubyte_t* srcptr,
	ubyte_t*	   dstptr,
	std::size_t	   size_x,
	std::size_t	   size_y,
	std::size_t	   bsiz_x,
	std::size_t	   bsiz_y)
{
	ulong_t scal_x = iscalb(bsiz_x);
	ulong_t scal_y = iscalb(bsiz_y);
	ulong_t blks_x = size_x >> scal_x;
	ulong_t blks_y = size_y >> scal_y;
	ulong_t blks_t = blks_x * blks_y;
	if (scal_x <= 3)
	  {
		static void (__stdcall* const reduce[])(
			const ubyte_t*, uword_t*, uword_t*,
			ulong_t, ulong_t, ulong_t) throw() = { reduce_0, reduce_1, reduce_2, reduce_3 };
		static void (__stdcall* const expand[])(
			const uword_t*, const uword_t*, ubyte_t*, ubyte_t*, ulong_t*,
			ulong_t, ulong_t, ulong_t) throw() = { expand_0, expand_1, expand_2, expand_3 };
		std::auto_ptr<uword_t> redbuf(new uword_t[blks_t]);
		std::auto_ptr<ulong_t> tmpbuf(new ulong_t[2 * size_x]);
		reduce[scal_x](srcptr,
			redbuf.get(), redbuf.get() + blks_t,
			size_x, bsiz_y * size_x, blks_x);
		expand[scal_x](redbuf.get(), redbuf.get() + blks_t,
			dstptr, dstptr + size_x * size_y,
			tmpbuf.get(),
			size_x, bsiz_y, blks_x);
		__asm	emms;
	  }
	else
	  {
		ulong_t* tmpbuf = new ulong_t[blks_t + 2 * size_x];
		(scal_x == 4 ? reduce_4 : reduce_5)(srcptr,
			tmpbuf, tmpbuf + blks_t,
			size_x, bsiz_y * size_x, blks_x);
		(scal_x == 4 ? expand_4 : expand_5)(tmpbuf, tmpbuf + blks_t,
			dstptr, dstptr + size_x * size_y,
			tmpbuf + blks_t,
			size_x, bsiz_y, blks_x);
		__asm	emms;
		delete [] tmpbuf;
	  }
}

#ifdef TEST

static unsigned random(unsigned n)
{
	return unsigned(n * std::rand() / (RAND_MAX + 1.0));
}

int main()
{
	std::size_t sx = 1024, sy = 512;
	ubyte_t* image = new ubyte_t[sx * sy];
	for (std::size_t iter = 0; iter < 10; ++iter)
	  {
		for (std::size_t n = 0; n < sx * sy; ++n)
		  {
			image[n] = ubyte_t(random(256));
		  }
		__int64 clock;
		__asm
		  {
			rdtsc;
			mov		DWORD PTR clock[0], eax;
			mov		DWORD PTR clock[4], edx;
		  }
		LPF(image, image, sx, sy, 16, 16);
		__asm
		  {
			rdtsc;
			sub		eax, DWORD PTR clock[0];
			sbb		edx, DWORD PTR clock[4];
			mov		DWORD PTR clock[0], eax;
			mov		DWORD PTR clock[4], edx;
		  }
		std::cout
			<< "Iter #" << iter << ": "
			<< std::fixed << std::setprecision(2) << std::setw(6) << double(clock) / (1E6 / 3)
			<< "msec." << std::endl;
	  }
	delete[] image;
	return 0;
}

#endif

// local variables:
// tab-width: 4
// end:
// char-encoding: Ｓｈｉｆｔ－ＪＩＳ.
#endif
