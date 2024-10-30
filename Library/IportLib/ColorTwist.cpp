#include "stdafx.h"
#include <crtdbg.h>
#include <Windows.h>
#include <intrin.h>
#include "ColorTwist.h"
#ifndef _M_X64
// src 1CH
static void rgbplanar_colortwist_asm_word(BYTE** pdst, size_t width, size_t dst_pitch,
									  const BYTE* psrc,  size_t src_pitch, size_t height,
									  const short* c_r, const short* c_g, const short* c_b)
{
	__asm {
		mov			eax, 0x800B0A09;
		movd		xmm7, eax;
		pslldq		xmm7, 4;
		mov			eax, 0x80080706;
		movd		xmm6, eax;
		por			xmm7, xmm6;
		pslldq		xmm7, 4;
		mov			eax, 0x80050403;
		movd		xmm6, eax;
		por			xmm7, xmm6;
		pslldq		xmm7, 4;
		mov			eax, 0x80020100;
		movd		xmm6, eax;
		por			xmm7, xmm6;

		mov			eax, c_r;
		movq		xmm4, QWORD PTR [eax];
		punpcklqdq	xmm4, xmm4;

		mov			eax, c_g;
		movq		xmm5, QWORD PTR [eax];
		punpcklqdq	xmm5, xmm5;

		mov			eax, c_b;
		movq		xmm6, QWORD PTR [eax];
		punpcklqdq	xmm6, xmm6;

		mov			esi, psrc;
		mov			ecx, height;
		xor			edi, edi;				// dst offset
L0:
		xor			ebx, ebx;				// x axis counter
L1:
		mov			eax, ebx;
		add			eax, eax;
		add			eax, ebx;				// x * 3
		
		pxor		xmm2, xmm2;
		movq		xmm0, QWORD PTR [esi+eax];		// G2 R2 B1 G1 R1 B0 G0 R0
		movd		xmm1, DWORD PTR [esi+eax+8];	//             B3 G3 R3 B2
		pslldq		xmm1, 8;
		por			xmm0, xmm1;
		pshufb		xmm0, xmm7;						// 0BGR 0BGR 0BGR 0BGR
		movdqa		xmm1, xmm0;
		punpcklbw	xmm0, xmm2;						// 0BGR:1 0BGR:0
		punpckhbw	xmm1, xmm2;						// 0BGR:3 0BGR:2

		// R
		movdqa		xmm2, xmm0;
		pmaddwd		xmm2, xmm4;
		movdqa		xmm3, xmm1;
		pmaddwd		xmm3, xmm4;
		phaddd		xmm2, xmm3;
		psrad		xmm2, 12;
		packssdw	xmm2, xmm2;
		packuswb	xmm2, xmm2;

		mov			eax, pdst;
		mov			eax, [eax];
		add			eax, edi;
		movd		[eax + ebx], xmm2;
		
		// G
		movdqa		xmm2, xmm0;
		pmaddwd		xmm2, xmm5;
		movdqa		xmm3, xmm1;
		pmaddwd		xmm3, xmm5;
		phaddd		xmm2, xmm3;
		psrad		xmm2, 12;
		packssdw	xmm2, xmm2;
		packuswb	xmm2, xmm2;

		mov			eax, pdst;
		mov			eax, [eax+4];
		add			eax, edi;
		movd		[eax + ebx], xmm2;

		// B
		movdqa		xmm2, xmm0;
		pmaddwd		xmm2, xmm6;
		movdqa		xmm3, xmm1;
		pmaddwd		xmm3, xmm6;
		phaddd		xmm2, xmm3;
		psrad		xmm2, 12;
		packssdw	xmm2, xmm2;
		packuswb	xmm2, xmm2;

		mov			eax, pdst;
		mov			eax, [eax+8];
		add			eax, edi;
		movd		[eax + ebx], xmm2;

		add			ebx, 4;
		cmp			ebx, width;
		jnz			L1;

		add			esi, src_pitch;
		add			edi, dst_pitch;
		dec			ecx;
		jnz			L0;
	}
}

// src 2CH interleaved
static void rgbplanar_colortwist_asm_word_2tap(BYTE** pdst, size_t half_width, size_t dst_pitch,
									  const BYTE*const* psrc,  size_t src_pitch, size_t height,
									  const short* c_r, const short* c_g, const short* c_b)
{
	__declspec(align(16)) __m128i	tmp_r;
	__declspec(align(16)) __m128i	tmp_g;
	__declspec(align(16)) __m128i	tmp_b;

	__asm {
		mov			eax, 0x800B0A09;
		movd		xmm7, eax;
		pslldq		xmm7, 4;
		mov			eax, 0x80080706;
		movd		xmm6, eax;
		por			xmm7, xmm6;
		pslldq		xmm7, 4;
		mov			eax, 0x80050403;
		movd		xmm6, eax;
		por			xmm7, xmm6;
		pslldq		xmm7, 4;
		mov			eax, 0x80020100;
		movd		xmm6, eax;
		por			xmm7, xmm6;

		mov			eax, c_r;
		movq		xmm4, QWORD PTR [eax];
		punpcklqdq	xmm4, xmm4;

		mov			eax, c_g;
		movq		xmm5, QWORD PTR [eax];
		punpcklqdq	xmm5, xmm5;

		mov			eax, c_b;
		movq		xmm6, QWORD PTR [eax];
		punpcklqdq	xmm6, xmm6;

		xor			esi, esi;				// src offset
		mov			ecx, height;
		xor			edi, edi;				// dst offset
L0:
		xor			edx, edx;				// x/2 counter
L1:
		// psrc[0]
		pxor		xmm2, xmm2;
		mov			eax, psrc;
		mov			eax, [eax];				// psrc[0]
		add			eax, edx;
		add			eax, edx;
		add			eax, edx;				// + x*3
		movq		xmm0, QWORD PTR [esi+eax];		// G2 R2 B1 G1 R1 B0 G0 R0
		movd		xmm1, DWORD PTR [esi+eax+8];	//             B3 G3 R3 B2
		pslldq		xmm1, 8;
		por			xmm0, xmm1;
		pshufb		xmm0, xmm7;						// 0BGR 0BGR 0BGR 0BGR
		movdqa		xmm1, xmm0;
		punpcklbw	xmm0, xmm2;						// 0BGR:1 0BGR:0
		punpckhbw	xmm1, xmm2;						// 0BGR:3 0BGR:2

		// R0
		movdqa		xmm2, xmm0;
		pmaddwd		xmm2, xmm4;
		movdqa		xmm3, xmm1;
		pmaddwd		xmm3, xmm4;
		phaddd		xmm2, xmm3;
		psrad		xmm2, 12;
		packssdw	xmm2, xmm2;
		packuswb	xmm2, xmm2;

//		mov			eax, pdst;
//		mov			eax, [eax];
//		add			eax, edi;
		movdqa		tmp_r, xmm2;
		
		// G0
		movdqa		xmm2, xmm0;
		pmaddwd		xmm2, xmm5;
		movdqa		xmm3, xmm1;
		pmaddwd		xmm3, xmm5;
		phaddd		xmm2, xmm3;
		psrad		xmm2, 12;
		packssdw	xmm2, xmm2;
		packuswb	xmm2, xmm2;

//		mov			eax, pdst;
//		mov			eax, [eax+4];
//		add			eax, edi;
		movdqa		tmp_g, xmm2;

		// B0
		movdqa		xmm2, xmm0;
		pmaddwd		xmm2, xmm6;
		movdqa		xmm3, xmm1;
		pmaddwd		xmm3, xmm6;
		phaddd		xmm2, xmm3;
		psrad		xmm2, 12;
		packssdw	xmm2, xmm2;
		packuswb	xmm2, xmm2;

//		mov			eax, pdst;
//		mov			eax, [eax+8];
//		add			eax, edi;
		movdqa		tmp_b, xmm2;

		// psrc[1]
		pxor		xmm2, xmm2;
		mov			eax, psrc;
		mov			eax, [eax+4];				// psrc[1]
		add			eax, edx;
		add			eax, edx;
		add			eax, edx;				// + x*3
		movq		xmm0, QWORD PTR [esi+eax];		// G2 R2 B1 G1 R1 B0 G0 R0
		movd		xmm1, DWORD PTR [esi+eax+8];	//             B3 G3 R3 B2
		pslldq		xmm1, 8;
		por			xmm0, xmm1;
		pshufb		xmm0, xmm7;						// 0BGR 0BGR 0BGR 0BGR
		movdqa		xmm1, xmm0;
		punpcklbw	xmm0, xmm2;						// 0BGR:1 0BGR:0
		punpckhbw	xmm1, xmm2;						// 0BGR:3 0BGR:2

		// R1
		movdqa		xmm2, xmm0;
		pmaddwd		xmm2, xmm4;
		movdqa		xmm3, xmm1;
		pmaddwd		xmm3, xmm4;
		phaddd		xmm2, xmm3;
		psrad		xmm2, 12;
		packssdw	xmm2, xmm2;
		packuswb	xmm2, xmm2;

		movdqa		xmm3, tmp_r;
		punpcklbw	xmm3, xmm2;
		mov			eax, pdst;
		mov			eax, [eax];
		add			eax, edi;
		movq		QWORD PTR [eax + 2*edx], xmm3;
		
		// G1
		movdqa		xmm2, xmm0;
		pmaddwd		xmm2, xmm5;
		movdqa		xmm3, xmm1;
		pmaddwd		xmm3, xmm5;
		phaddd		xmm2, xmm3;
		psrad		xmm2, 12;
		packssdw	xmm2, xmm2;
		packuswb	xmm2, xmm2;

		movdqa		xmm3, tmp_g;
		punpcklbw	xmm3, xmm2;
		mov			eax, pdst;
		mov			eax, [eax+4];
		add			eax, edi;
		movq		QWORD PTR [eax + 2*edx], xmm3;

		// B1
		movdqa		xmm2, xmm0;
		pmaddwd		xmm2, xmm6;
		movdqa		xmm3, xmm1;
		pmaddwd		xmm3, xmm6;
		phaddd		xmm2, xmm3;
		psrad		xmm2, 12;
		packssdw	xmm2, xmm2;
		packuswb	xmm2, xmm2;

		movdqa		xmm3, tmp_b;
		punpcklbw	xmm3, xmm2;
		mov			eax, pdst;
		mov			eax, [eax+8];
		add			eax, edi;
		movq		QWORD PTR [eax + 2*edx], xmm3;

		add			edx, 4;
		cmp			edx, half_width;
		jnz			L1;

		add			esi, src_pitch;
		add			edi, dst_pitch;
		dec			ecx;
		jnz			L0;
	}
}
#endif 

void __stdcall rgbplanar_colortwist_asm(BYTE** pdst, size_t width, size_t dst_pitch,
		const BYTE* psrc,  size_t src_pitch, size_t height, const float mat[12])
{
	short c_r[4];
	short c_g[4];
	short c_b[4];
	// å≈íËè¨êîì_ââéZ
	for (int i = 0; i < 4; i++) {
		const int val_r = (i < 3) ? (int)(mat[0*4+i]*4096 + 0.5) : 0;
		const int val_g = (i < 3) ? (int)(mat[1*4+i]*4096 + 0.5) : 0;
		const int val_b = (i < 3) ? (int)(mat[2*4+i]*4096 + 0.5) : 0;
		_ASSERT(val_r < 32768 || val_r >= -32768);
		_ASSERT(val_g < 32768 || val_g >= -32768);
		_ASSERT(val_b < 32768 || val_b >= -32768);

		c_r[i] = (short)val_r;
		c_g[i] = (short)val_g;
		c_b[i] = (short)val_b;
	}

#ifdef _M_X64
	int info[4];
	__cpuid(info, 1);
	if (info[2] & (1 << 28)) {
		::rgbplanar_colortwist_asm_avx(pdst, width, dst_pitch, psrc, src_pitch, height, c_r, c_g, c_b);
	} else {
		::rgbplanar_colortwist_asm_sse(pdst, width, dst_pitch, psrc, src_pitch, height, c_r, c_g, c_b);
	}
#else
	::rgbplanar_colortwist_asm_word(pdst, width, dst_pitch, psrc, src_pitch, height, c_r, c_g, c_b);
#endif 
}

void __stdcall rgbplanar_colortwist_asm(BYTE** pdst, size_t width, size_t dst_pitch,
		const BYTE*const* psrc,  size_t src_pitch, size_t height, const float mat[12])
{
	short c_r[4];
	short c_g[4];
	short c_b[4];
	// å≈íËè¨êîì_ââéZ
	for (int i = 0; i < 4; i++) {
		const int val_r = (i < 3) ? (int)(mat[0*4+i]*4096 + 0.5) : 0;
		const int val_g = (i < 3) ? (int)(mat[1*4+i]*4096 + 0.5) : 0;
		const int val_b = (i < 3) ? (int)(mat[2*4+i]*4096 + 0.5) : 0;
		_ASSERT(val_r < 32768 || val_r >= -32768);
		_ASSERT(val_g < 32768 || val_g >= -32768);
		_ASSERT(val_b < 32768 || val_b >= -32768);

		c_r[i] = (short)val_r;
		c_g[i] = (short)val_g;
		c_b[i] = (short)val_b;
	}

#ifdef _M_X64
	int info[4];
	__cpuid(info, 1);
	if (info[2] & (1 << 28)) {
		::rgbplanar_colortwist_asm_avx_2tap(pdst, width/2, dst_pitch, psrc, src_pitch, height, c_r, c_g, c_b);
	} else {
		::rgbplanar_colortwist_asm_sse_2tap(pdst, width/2, dst_pitch, psrc, src_pitch, height, c_r, c_g, c_b);
	}
#else
		::rgbplanar_colortwist_asm_word_2tap(pdst, width/2, dst_pitch, psrc, src_pitch, height, c_r, c_g, c_b);
#endif 
}
