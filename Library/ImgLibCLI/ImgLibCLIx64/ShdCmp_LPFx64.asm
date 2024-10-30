.code



reduce_0 PROC

;; 引数のスタック戻し
mov		[rsp + 8], rcx				;; const ubyte_t *srcptr
mov		[rsp + 16], rdx				;; uword_t *dstptr
mov		[rsp + 24], r8				;; uword_t *dstend
mov		[rsp + 32], r9				;; ulong_t size_x
									;; ulong_t bsiz_r
									;; ulong_t blks_x
									
	;; レジスタ待避
	mov			r8, rbx
	mov			r9, rsi
	mov			r10, rdi

	pxor		mm0, mm0			;; MM0 は常に 0 を保持する.
	mov			rsi, [rsp + 8]
	mov			rdi, [rsp + 16]
	xor			rcx, rcx
	mov			ecx, [rsp + 32]
	;; while (dstptr < dstend)
R0:
	mov			rdx, rsi			;; const ubyte_t* blkptr = srcptr
	xor			rax, rax
	mov			eax, [rsp + 40]
	add			rsi, rax			;; srcptr += bsiz_r
	xor			rax, rax
	mov			eax, [rsp + 48]
	lea			rbx, [rdi + rax * 2 + 1 - 16]
	;; このときは, R1 からのループで複数ブロックを処理することに
	;; なるので, 右端がはみ出さないようにチェックしておく.
	;; LEA 命令の -7 も同じ理由による.
	cmp			rdi, rbx
	jae			R4
	;; while (dstptr < rowend)
R1:
	mov			rax, rdx			;; const ubyte_t* rasptr = blkptr
	add			rdx, 8
	movq		mm4, mm0
	movq		mm5, mm0
	;; while (rasptr < srcptr)
R2:
	movq		mm2, [rax+0]
	add			rax, rcx
	movq		mm3, mm2
	punpcklbw	mm2, mm0
	punpckhbw	mm3, mm0
	cmp			rax, rsi
	paddw		mm4, mm2
	paddw		mm5, mm3
	jb			R2;

	movq		[rdi+0], mm4
	movq		[rdi+8], mm5
	add			rdi, 16
	cmp			rdi, rbx
	jb			R1
	
R4:
	add			rbx, 16 - 1
	cmp			rdi, rbx
	jae			R7
R5:
	mov			rax, rdx
	inc			rdx
	pxor		mm4, mm4
R6:
	movzx		r11, BYTE PTR [rax]
	add			rax, rcx
	movd		mm2, r11
	paddw		mm4, mm2
	cmp			rax, rsi
	jb			R6
	movd		rax, mm4
	mov			[rdi], ax
	add			rdi, 2
	cmp			rdi, [rsp]
	jb			R5
R7:
	cmp			rdi, [rsp + 24]
	jb			R0
	
	;; レジスタ復帰
	mov			rbx, r8
	mov			rsi, r9
	mov			rdi, r10

ret

reduce_0 ENDP



reduce_1 PROC

;; 引数のスタック戻し
mov		[rsp + 8], rcx				;; const ubyte_t *srcptr
mov		[rsp + 16], rdx				;; uword_t *dstptr
mov		[rsp + 24], r8				;; uword_t *dstend
mov		[rsp + 32], r9				;; ulong_t size_x
									;; ulong_t bsiz_r
									;; ulong_t blks_x
									
	;; レジスタ待避
	mov			r8, rbx
	mov			r9, rsi
	mov			r10, rdi

	pxor		mm0, mm0			;; MM0 は常に 0 を保持する.
	mov			rsi, [rsp + 8]
	mov			rdi, [rsp + 16]
	xor			rcx, rcx
	mov			ecx, [rsp + 32]
	;; while (dstptr < dstend)
R0:
	mov			rdx, rsi			;; const ubyte_t* blkptr = srcptr
	xor			rax, rax
	mov			eax, [rsp + 40]
	add			rsi, rax			;; srcptr += bsiz_r
	xor			rax, rax
	mov			eax, [rsp + 48]
	lea			rbx, [rdi + rax * 2 + 1 - 8]
	;; このときは, R1 からのループで複数ブロックを処理することに
	;; なるので, 右端がはみ出さないようにチェックしておく.
	;; LEA 命令の -7 も同じ理由による.
	cmp			rdi, rbx
	jae			R4
	;; while (dstptr < rowend)
R1:
	mov			rax, rdx			;; const ubyte_t* rasptr = blkptr
	add			rdx, 8
	movq		mm4, mm0
	movq		mm5, mm0
	;; while (rasptr < srcptr)
R2:
	movq		mm2, [rax+0]
	add			rax, rcx
	movq		mm3, mm2
	punpcklbw	mm2, mm0
	punpckhbw	mm3, mm0
	cmp			rax, rsi
	paddw		mm4, mm2
	paddw		mm5, mm3
	jb			R2

	movq		mm2, mm4			;; 4:aabbccdd 5:eeffgghh
	punpcklwd	mm4, mm5			;; 4:aaeebbff
	punpckhwd	mm2, mm5			;;            2:ccggddhh
	movq		mm5, mm4			;;            5:aaeebbff
	punpcklwd	mm4, mm2			;; 4:aacceegg
	punpckhwd	mm5, mm2			;;            5:bbddffhh
	paddw		mm4, mm5			;; 4:abcdefgh
	movq		[rdi], mm4
	add			rdi, 8
	cmp			rdi, rbx
	jb			R1

R4:
	add			rbx, 8 - 1
	cmp			rdi, rbx
	jae			R7
R5:
	mov			rax, rdx
	add			edx, 2
	pxor		mm4, mm4
R6:
	movzx		r11, WORD PTR [rax]
	add			rax, rcx
	movd		mm2, r11
	punpcklbw	mm2, mm0
	paddw		mm4, mm2
	cmp			rax, rsi
	jb			R6
	movq		mm5, mm4
	punpckhwd	mm4, mm0
	paddw		mm4, mm5
	movd		rax, mm4
	mov			[rdi], ax
	add			rdi, 2
	cmp			rdi, [rsp]
	jb			R5
R7:
	cmp			rdi, [rsp + 24]
	jb			R0

	;; レジスタ復帰
	mov			rbx, r8
	mov			rsi, r9
	mov			rdi, r10

ret

reduce_1 ENDP


reduce_2 PROC

;; 引数のスタック戻し
mov		[rsp + 8], rcx				;; const ubyte_t *srcptr
mov		[rsp + 16], rdx				;; uword_t *dstptr
mov		[rsp + 24], r8				;; uword_t *dstend
mov		[rsp + 32], r9				;; ulong_t size_x
									;; ulong_t bsiz_r
									;; ulong_t blks_x
									
	;; レジスタ待避
	mov			r8, rbx
	mov			r9, rsi
	mov			r10, rdi

	pxor		mm0, mm0			;; MM0 は常に 0 を保持する.
	mov			rsi, [rsp + 8]
	mov			rdi, [rsp + 16]
	xor			rcx, rcx
	mov			ecx, [rsp + 32]
	;; while (dstptr < dstend)
R0:
	mov			rdx, rsi			;; const ubyte_t* blkptr = srcptr
	xor			rax, rax
	mov			eax, [rsp + 40]
	add			rsi, rax			;; srcptr += bsiz_r
	xor			rax, rax
	mov			eax, [rsp + 48]
	lea			rbx, [rdi + rax * 2 + 1 - 4]
	;; このときは, R1 からのループで複数ブロックを処理することに
	;; なるので, 右端がはみ出さないようにチェックしておく.
	;; LEA 命令の -7 も同じ理由による.
	cmp			rdi, rbx
	jae			R4
	;; while (dstptr < rowend)
R1:
	mov			rax, rdx			;; const ubyte_t* rasptr = blkptr
	add			rdx, 8
	movq		mm4, mm0
	movq		mm5, mm0
	;; while (rasptr < srcptr)
R2:
	movq		mm2, [rax+0]
	add			rax, rcx
	movq		mm3, mm2
	punpcklbw	mm2, mm0
	punpckhbw	mm3, mm0
	cmp			rax, rsi
	paddw		mm4, mm2
	paddw		mm5, mm3
	jb			R2

	movq		mm2, mm4			;; 4:aabbccdd 5:eeffgghh
	punpcklwd	mm4, mm5			;; 4:aaeebbff
	punpckhwd	mm2, mm5			;;            2:ccggddhh
	paddw		mm4, mm2			;; 4:acegbdfh
	movq		mm5, mm4			;;            5:acegbdfh
	punpckhdq	mm5, mm0			;;            5:bdfh0000
	paddw		mm4, mm5			;; 4:adehxxxx
	movd		DWORD PTR [rdi], mm4
	add			rdi, 4
	cmp			rdi, rbx
	jb			R1

R4:
	add			rbx, 4 - 1
	cmp			rdi, rbx
	jae			R7
R5:
	mov			rax, rdx
	add			rdx, 4
	pxor		mm4, mm4
R6:
	movd		mm2, DWORD PTR [rax]
	add			rax, rcx
	punpcklbw	mm2, mm0
	paddw		mm4, mm2
	cmp			rax, rsi
	jb			R6
	movq		mm5, mm4			;; 4:aabbccdd 5:aabbccdd
	punpcklwd	mm4, mm0			;; 4:aa00bb00
	punpckhwd	mm5, mm0			;;            5:cc00dd00
	paddd		mm4, mm5			;; 4:ac00bd00
	movq		mm5, mm4
	punpckhdq	mm4, mm0			;; 4:bd000000
	paddd		mm4, mm5
	movd		rax, mm4
	mov			[rdi], ax
	add			rdi, 2
	cmp			rdi, rbx
	jb			R5
R7:
	cmp			rdi, [rsp + 24]
	jb			R0
	
	;; レジスタ復帰
	mov			rbx, r8
	mov			rsi, r9
	mov			rdi, r10

ret

reduce_2 ENDP


reduce_3 PROC

;; 引数のスタック戻し
mov		[rsp + 8], rcx				;; const ubyte_t *srcptr
mov		[rsp + 16], rdx				;; uword_t *dstptr
mov		[rsp + 24], r8				;; uword_t *dstend
mov		[rsp + 32], r9				;; ulong_t size_x
									;; ulong_t bsiz_r
									;; ulong_t blks_x
									
	;; レジスタ待避
	mov			r8, rbx
	mov			r9, rsi
	mov			r10, rdi

	pxor		mm0, mm0			;; MM0 は常に 0 を保持する.
	mov			rsi, [rsp + 8]
	mov			rdi, [rsp + 16]
	xor			rcx, rcx
	mov			ecx, [rsp + 32]
	;; while (dstptr < dstend)
R0:
	mov			rdx, rsi			;; const ubyte_t* blkptr = srcptr
	xor			rax, rax
	mov			eax, [rsp + 40]
	add			rsi, rax			;; srcptr += bsiz_r
	xor			rax, rax
	mov			eax, [rsp + 48]
	lea			rbx, [rdi + rax * 2]
	;; while (dstptr < rowend)
R1:
	mov			rax, rdx			;; const ubyte_t* rasptr = blkptr
	add			rdx, 8
	movq		mm4, mm0
	movq		mm5, mm0
	;; while (rasptr < srcptr)
R2:
	movq		mm2, [rax+0]
	add			rax, rcx
	movq		mm3, mm2
	punpcklbw	mm2, mm0
	punpckhbw	mm3, mm0
	cmp			rax, rsi
	paddw		mm4, mm2
	paddw		mm5, mm3
	jb			R2

	;; bsiz_x >= 16 のとき, ピクセル値の和は最大 17 ビット以上となるが,
	;; mm4:mm5 の各ワードに 8 分割されているので, まだまだ余裕がある.
	;; つまり, いきなり mm4:mm5 を加算しても, オーバフローの心配はない.
	paddw		mm4, mm5
	movq		mm5, mm4			;; 4:aabbccdd 5:aabbccdd
	punpcklwd	mm4, mm0			;; 4:aa00bb00
	punpckhwd	mm5, mm0			;;            5:cc00dd00
	paddd		mm4, mm5			;; 4:ac00bd00
	movq		mm5, mm4			;;            5:ac00bd00
	punpckhdq	mm5, mm0			;;            5:bd000000
	paddd		mm4, mm5			;; 4:ad000000
	movd		rax, mm4
	mov			[rdi], ax
	add			rdi, 2
	cmp			rdi, rbx
	jb			R1

	cmp			rdi, [rsp + 24]
	jb			R0

	;; レジスタ復帰
	mov			rbx, r8
	mov			rsi, r9
	mov			rdi, r10

ret

reduce_3 ENDP


reduce_4 PROC

;; 引数のスタック戻し
mov		[rsp + 8], rcx				;; const ubyte_t *srcptr
mov		[rsp + 16], rdx				;; ulong_t *dstptr
mov		[rsp + 24], r8				;; ulong_t *dstend
mov		[rsp + 32], r9				;; ulong_t size_x
									;; ulong_t bsiz_r
									;; ulong_t blks_x
									
	;; レジスタ待避
	mov			r8, rbx
	mov			r9, rsi
	mov			r10, rdi

	pxor		mm0, mm0			;; MM0 は常に 0 を保持する.
	mov			rsi, [rsp + 8]
	mov			rdi, [rsp + 16]
	xor			rcx, rcx
	mov			ecx, [rsp + 32]
	;; while (dstptr < dstend)
R0:
	mov			rdx, rsi			;; const ubyte_t* blkptr = srcptr;
	xor			rax, rax
	mov			eax, [rsp + 40]
	add			rsi, rax			;; srcptr += bsiz_r;
	xor			rax, rax
	mov			eax, [rsp + 48]
	lea			rbx, [rdi + rax * 4]
	;; while (dstptr < rowend)
R1:
	mov			rax, rdx			;; const ubyte_t* rasptr = blkptr;
	add			rdx, 16
	movq		mm4, mm0
	movq		mm5, mm0
	;; while (rasptr < srcptr)
R2:
	movq		mm2, [rax+0]
	movq		mm3, mm2
	punpcklbw	mm2, mm0
	punpckhbw	mm3, mm0
	paddw		mm4, mm2
	paddw		mm5, mm3
	movq		mm2, [rax+8]
	add			rax, rcx
	movq		mm3, mm2
	punpcklbw	mm2, mm0
	punpckhbw	mm3, mm0
	cmp			rax, rsi
	paddw		mm4, mm2
	paddw		mm5, mm3
	jb			R2

	;; bsiz_x >= 16 のとき, ピクセル値の和は最大 17 ビット以上となるが,
	;; mm4:mm5 の各ワードに 8 分割されているので, まだまだ余裕がある.
	;; つまり, いきなり mm4:mm5 を加算しても, オーバフローの心配はない.
	paddw		mm4, mm5
	movq		mm5, mm4			;; 4:aabbccdd 5:aabbccdd
	punpcklwd	mm4, mm0			;; 4:aa00bb00
	punpckhwd	mm5, mm0			;;            5:cc00dd00
	paddd		mm4, mm5			;; 4:ac00bd00
	movq		mm5, mm4			;;            5:ac00bd00
	punpckhdq	mm5, mm0			;;            5:bd000000
	paddd		mm4, mm5			;; 4:ad000000
	;; このときは, ulong_t の配列である.
	movd		DWORD PTR [rdi], mm4
	add			rdi, 4
	cmp			rdi, rbx
	jb			R1

	cmp			rdi, [rsp + 24]
	jb			R0

	;; レジスタ復帰
	mov			rbx, r8
	mov			rsi, r9
	mov			rdi, r10

ret

reduce_4 ENDP


reduce_5 PROC

;; 引数のスタック戻し
mov		[rsp + 8], rcx				;; const ubyte_t *srcptr
mov		[rsp + 16], rdx				;; ulong_t *dstptr
mov		[rsp + 24], r8				;; ulong_t *dstend
mov		[rsp + 32], r9				;; ulong_t size_x
									;; ulong_t bsiz_r
									;; ulong_t blks_x
									
	;; レジスタ待避
	mov			r8, rbx
	mov			r9, rsi
	mov			r10, rdi

	pxor		mm0, mm0			;; MM0 は常に 0 を保持する.
	mov			rsi, [rsp + 8]
	mov			rdi, [rsp + 16]
	xor			rcx, rcx
	mov			ecx, [rsp + 32]
	;; while (dstptr < dstend)
R0:
	mov			rdx, rsi			;; const ubyte_t* blkptr = srcptr;
	xor			rax, rax
	mov			eax, [rsp + 40]
	add			rsi, rax			;; srcptr += bsiz_r;
	xor			rax, rax
	mov			eax, [rsp + 48]
	lea			rbx, [rdi + rax * 4]
	;; while (dstptr < rowend)
R1:
	mov			rax, rdx			;; const ubyte_t* rasptr = blkptr;
	add			rdx, 32
	movq		mm4, mm0
	movq		mm5, mm0
	;; while (rasptr < srcptr)
R2:
	movq		mm2, [rax+0]
	movq		mm3, mm2
	punpcklbw	mm2, mm0
	punpckhbw	mm3, mm0
	paddw		mm4, mm2
	paddw		mm5, mm3
	movq		mm2, [rax+8]
	movq		mm3, mm2
	punpcklbw	mm2, mm0
	punpckhbw	mm3, mm0
	paddw		mm4, mm2
	paddw		mm5, mm3
	movq		mm2, [rax+16]
	movq		mm3, mm2
	punpcklbw	mm2, mm0
	punpckhbw	mm3, mm0
	paddw		mm4, mm2
	paddw		mm5, mm3
	movq		mm2, [rax+24]
	add			rax, rcx
	movq		mm3, mm2
	punpcklbw	mm2, mm0
	punpckhbw	mm3, mm0
	cmp			rax, rsi
	paddw		mm4, mm2
	paddw		mm5, mm3
	jb			R2

	;; bsiz_x >= 16 のとき, ピクセル値の和は最大 17 ビット以上となるが,
	;; mm4:mm5 の各ワードに 8 分割されているので, まだまだ余裕がある.
	;; つまり, いきなり mm4:mm5 を加算しても, オーバフローの心配はない.
	paddw		mm4, mm5
	movq		mm5, mm4			;; 4:aabbccdd 5:aabbccdd
	punpcklwd	mm4, mm0			;; 4:aa00bb00
	punpckhwd	mm5, mm0			;;            5:cc00dd00
	paddd		mm4, mm5			;; 4:ac00bd00
	movq		mm5, mm4			;;            5:ac00bd00
	punpckhdq	mm5, mm0			;;            5:bd000000
	paddd		mm4, mm5			;; 4:ad000000
	;; このときは, ulong_t の配列である.
	movd		DWORD PTR [rdi], mm4
	add			rdi, 4
	cmp			rdi, rbx
	jb			R1

	cmp			rdi, [rsp + 24]
	jb			R0

	;; レジスタ復帰
	mov			rbx, r8
	mov			rsi, r9
	mov			rdi, r10

ret

reduce_5 ENDP


expand_xy PROC

;; 引数のスタック戻し
mov		[rsp + 8], rcx				;; ubyte_t* dstptr
mov		[rsp + 16], rdx				;; ulong_t* srcptr
mov		[rsp + 24], r8				;; const ulong_t* difptr
mov		[rsp + 32], r9				;; ulong_t size_x
									;; ulong_t bsiz_y
									;; ulong_t scale

	;; レジスタ待避
	mov			r8, rbx
	mov			r9, rsi
	mov			r10, rdi

	mov			rdi, [rsp + 8]
	mov			ecx, [rsp + 48]
	pxor		mm7, mm7
	movd		mm7, ecx

L0:
	mov			rbx, [rsp + 16]
	mov			rsi, [rsp + 24]
	mov			eax, [rsp + 32]
	sub			eax, 8
	jb			L2

	;; 以下のレジスタの使用ポリシーは, レジスタが足りないため少々わかりにくい.
	;; 最初の組は MM4:MM5, 次の組は MM5:MM6 をペアとして使用している.
L1:
	movq		mm4, [rbx + 0]
	movq		mm5, [rbx + 8]
	movq		mm0, mm4
	psrld		mm4, mm7
	movq		mm2, [rsi + 0]
	movq		mm1, mm5
	psrld		mm5, mm7
	movq		mm3, [rsi + 8]
	paddd		mm0, mm2
	paddd		mm1, mm3
	packssdw	mm4, mm5
	movq		[rbx + 0], mm0
	movq		[rbx + 8], mm1
	movq		mm5, [rbx + 16]
	movq		mm6, [rbx + 24]
	movq		mm0, mm5
	psrld		mm5, mm7
	movq		mm2, [rsi + 16]
	movq		mm1, mm6
	psrld		mm6, mm7
	movq		mm3, [rsi + 24]
	paddd		mm0, mm2
	paddd		mm1, mm3
	packssdw	mm5, mm6
	movq		[rbx + 16], mm0
	movq		[rbx + 24], mm1
	packuswb	mm4, mm5
	add			rbx, 32
	add			rsi, 32
	movq		[rdi], mm4
	add			rdi, 8
	sub			eax, 8
	jae			L1

L2:
	add			eax, 8
	jz			L4
	mov			ch, al
L3:
	mov			edx, [rbx]
	mov			eax, edx
	shr			edx, cl
	add			eax, [rsi]
	add			rsi, 4
	mov			[rdi], dl
	inc			rdi
	mov			[rbx], eax
	add			rbx, 4
	dec			ch
	jnz			L3
L4:
	dec			DWORD PTR [rsp + 40]

	jnz			L0
	mov			[rsp + 8], rdi

	;; 戻り値 dstptr
	mov			rax, rdi

	;; レジスタ復帰
	mov			rbx, r8
	mov			rsi, r9
	mov			rdi, r10

ret

expand_xy ENDP


iscalb PROC

;; 引数のスタック戻し
mov		[rsp + 8], rcx				;; ulong_t
mov		[rsp + 16], rdx				;; 
mov		[rsp + 24], r8				;; 
mov		[rsp + 32], r9				;; 
									
	;; レジスタ待避
	mov			r8, rbx
	mov			r9, rsi
	mov			r10, rdi

	;; rcxの最下位セットビットを検索して、そのビット・インデックスをraxにストア
	bsf			rax, [rsp + 8]

	;; レジスタ復帰
	mov			rbx, r8
	mov			rsi, r9
	mov			rdi, r10

ret

iscalb ENDP



expand_x_0 PROC

;; 引数のスタック戻し
mov		[rsp + 8], rcx				;; const uword_t* srcptr
mov		[rsp + 16], rdx				;; const uword_t* srcend
mov		[rsp + 24], r8				;; ulong_t* dstptr
mov		[rsp + 32], r9				;; ulong_t* dstend
									;; ulong_t bsiz_y

	;; レジスタ待避
	mov			r8, rbx
	mov			r9, rsi
	mov			r10, rdi

	;; src には blks_x = size_x / bsiz_x 個分のデータが含まれる.
	;; size_x % bsiz_x != 0 のとき, 右側の半端なピクセルには, すべて最
	;; 後のデータを埋めることにした.

	mov			rsi, [rsp + 8]
	mov			rdx, [rsp + 16]
	mov			rdi, [rsp + 24]
	pxor		mm0, mm0
	bsf			ecx, [rsp + 40]
	movd		mm7, ecx

	sub			rdx, 7
	cmp			rsi, rdx
	jae			X1

X0:
	movq		mm2, [rsi]
	add			rsi, 8
	movq		mm3, mm2
	punpcklwd	mm2, mm0
	punpckhwd	mm3, mm0
	pslld		mm2, mm7
	pslld		mm3, mm7
	
	movq		[rdi + 0], mm2
	movq		[rdi + 8], mm3
	add			rdi, 16
	cmp			rsi, rdx
	jb			X0

X1:
	add			rdx, 7
	cmp			rsi, [rsp + 16]
	jae			X3

X2:
	movzx		rax, WORD PTR [rsi]
	add			rsi, 2
	shl			rax, cl
	mov			[rdi], rax
	add			rdi, 4
	cmp			rsi, rdx
	jb			X2

	;; BSIZ_X == 1 (SCAL_X == 0) のとき, 右端に半端なピクセルはあり得ない.
X3:
	mov			[rsp + 8], rsi

	;; 戻り値 srcptr
	mov			rax, rsi
	
	;; レジスタ復帰
	mov			rbx, r8
	mov			rsi, r9
	mov			rdi, r10

ret

expand_x_0 ENDP


expand_xdiff_0 PROC

;; 引数のスタック戻し
mov		[rsp + 8], rcx				;; const uword_t* srcptr
mov		[rsp + 16], rdx				;; const uword_t* srcend
mov		[rsp + 24], r8				;; ulong_t*	dstptr
mov		[rsp + 32], r9				;; ulong_t* dstend
									;; const ulong_t* basptr
									;; ulong_t bsiz_y

	;; レジスタ待避
	mov			r8, rbx
	mov			r9, rsi
	mov			r10, rdi
	
	;; src には blks_x = size_x / bsiz_x 個分のデータが含まれる.
	;; size_x % bsiz_x != 0 のとき, 右側の半端なピクセルには, すべて最
	;; 後のデータを埋めることにした.

	mov			rsi, [rsp + 8]
	mov			rdx, [rsp + 16]
	mov			rdi, [rsp + 24]
	pxor		mm0, mm0
	mov			rbx, [rsp + 40]
	bsf			ecx, [rsp + 48]
	pxor		mm7, mm7
	movd		mm7, ecx

	sub			rdx, 7
	cmp			rsi, rdx
	jae			X1

X0:
	movq		mm2, [rsi]
	add			rsi, 8
	movq		mm3, mm2
	punpcklwd	mm2, mm0
	punpckhwd	mm3, mm0
	movq		mm4, [rbx + 0]
	movq		mm5, [rbx + 8]
	psrld		mm4, mm7			;; *basptr++ >> scal_y.
	add			rbx, 16
	psrld		mm5, mm7
	psubd		mm2, mm4
	psubd		mm3, mm5
	movq		[rdi + 0], mm2
	movq		[rdi + 8], mm3
	add			rdi, 16
	cmp			rsi, rdx
	jb			X0

X1:
	add			rdx, 7
	cmp			rsi, [rsp + 16]
	jae			X3

X2:
	movzx		rax, WORD PTR [rsi]
	add			rsi, 2
	movd		mm2, rax
	movd		mm4, DWORD PTR [rbx]
	psrld		mm4, mm7
	add			rbx, 4
	psubd		mm2, mm4
	movd		DWORD PTR [rdi], mm2
	add			rdi, 4
	cmp			rsi, rdx
	jb			X2

	;; BSIZ_X == 1 (SCAL_X == 0) のとき, 右端に半端なピクセルはあり得ない.
X3:
	mov			[rsp + 8], rsi
	
	;; 戻り値 srcptr
	mov			rax, rsi
	
	;; レジスタ復帰
	mov			rbx, r8
	mov			rsi, r9
	mov			rdi, r10

ret

expand_xdiff_0 ENDP


expand_x_4 PROC

;; 引数のスタック戻し
mov		[rsp + 8], rcx				;; const uword_t* srcptr
mov		[rsp + 16], rdx				;; const uword_t* srcend
mov		[rsp + 24], r8				;; ulong_t* dstptr
mov		[rsp + 32], r9				;; ulong_t* dstend
									;; ulong_t bsiz_y

	;; レジスタ待避
	mov			r8, rbx
	mov			r9, rsi
	mov			r10, rdi
	
	;; src には blks_x = size_x / bsiz_x 個分のデータが含まれる.
	;; size_x % bsiz_x != 0 のとき, 右側の半端なピクセルには, すべて最
	;; 後のデータを埋めることにした.

	mov			rsi, [rsp + 8]
	mov			rdx, [rsp + 16]
	mov			rdi, [rsp + 24]
	bsf			ecx, [rsp + 40]
	;; DIFF == 0 のときは, MM7 に Y 方向のシフトカウントを入れておく.
	pxor		mm7, mm7
	movd		mm7, ecx

	pxor		mm4, mm4
	movd		mm4, DWORD PTR [rsi]
	add			rsi, 4
	;; MM0 に常にひとつ前のデータを入れておき,
	;; 次のデータとの差を計算するのに使用する.
	;; DIFF != 0 のときは, MMX レジスタに空きがないため,
	;; その都度読み直す必要がある. (多分キャッシュに残っているハズ.)
	pslld		mm4, mm7
	movq		mm0, mm4
	punpckldq	mm4, mm4
	pslld		mm4, 4

	;; 左端.
	movq		[rdi +  0], mm4
	movq		[rdi +  8], mm4
	movq		[rdi + 16], mm4
	movq		[rdi + 24], mm4
	add			rdi, 32

	;; 本体 (blks_x - 1) * bsiz_x ピクセル.
	cmp			rsi, rdx
	jae			X1
X0:
	pxor		mm1, mm1
	movd		mm1, DWORD PTR [rsi]
	pslld		mm1, mm7
	add			rsi, 4
	movq		mm3, mm1
	punpckldq	mm4, mm4			;; s0 * bsiz_x : s0 * bsiz_x
	psubd		mm1, mm0			;; ds (= s1 - s0)
	pxor		mm0, mm0
	punpckldq	mm0, mm1			;; ds : 0
	punpckldq	mm1, mm1			;; ds : ds
	paddd		mm4, mm0			;; s0 * bsiz_x + 1ds : + 0ds
	paddd		mm1, mm1			;; 2ds : 2ds
	movq		mm5, mm4
	paddd		mm5, mm1			;; s0 * bsiz_x + 3ds : + 2ds
	paddd		mm1, mm1			;; 4ds : 4ds
	movq		mm0, mm3

	movq		[rdi +  0], mm4
	paddd		mm4, mm1
	movq		[rdi +  8], mm5
	paddd		mm5, mm1
	movq		[rdi + 16], mm4
	paddd		mm4, mm1
	movq		[rdi + 24], mm5
	paddd		mm5, mm1
	movq		[rdi + 32], mm4
	paddd		mm4, mm1
	movq		[rdi + 40], mm5
	paddd		mm5, mm1
	movq		[rdi + 48], mm4
	paddd		mm4, mm1
	movq		[rdi + 56], mm5

	add			rdi, 64
	cmp			rsi, rdx
	punpckldq	mm4, mm4
	jb			X0

	;; 右端. 少なくとも bsiz_x / 2 (>= 8) 個以上は残っていることに注意.
X1:
	mov			rdx, [rsp + 32]
	sub			rdx, 15
	cmp			rdi, rdx
	jae			X3
X2:
	movq		[rdi + 0], mm4
	movq		[rdi + 8], mm4
	add			rdi, 16
	cmp			rdi, rdx
	jb			X2
X3:
	add			rdx, 15
	cmp			rdi, rdx
	jae			X5
X4:
	movd		DWORD PTR [rdi], mm4
	add			rdi, 4
	cmp			rdi, rdx
	jb			X4
X5:
	mov			[rsp + 8], rsi

	;; 戻り値 srcptr
	mov			rax, rsi
	
	;; レジスタ復帰
	mov			rbx, r8
	mov			rsi, r9
	mov			rdi, r10

ret

expand_x_4 ENDP


expand_xdiff_4 PROC

;; 引数のスタック戻し
;; 引数のスタック戻し
mov		[rsp + 8], rcx				;; const uword_t* srcptr
mov		[rsp + 16], rdx				;; const uword_t* srcend
mov		[rsp + 24], r8				;; ulong_t*	dstptr
mov		[rsp + 32], r9				;; ulong_t* dstend
									;; const ulong_t* basptr
									;; ulong_t bsiz_y

	;; レジスタ待避
	mov			r8, rbx
	mov			r9, rsi
	mov			r10, rdi
	
	;; src には blks_x = size_x / bsiz_x 個分のデータが含まれる.
	;; size_x % bsiz_x != 0 のとき, 右側の半端なピクセルには, すべて最
	;; 後のデータを埋めることにした.

	mov			rsi, [rsp + 8]
	mov			rdx, [rsp + 16]
	mov			rdi, [rsp + 24]
	mov			rbx, [rsp + 40]
	bsf			ecx, [rsp + 48]

	pxor		mm4, mm4
	movd		mm4, DWORD PTR [rsi]
	add			rsi, 4
	punpckldq	mm4, mm4
	pslld		mm4, 4

	pxor		mm0, mm0
	movd		mm0, ecx
	movq		mm6, [rbx + 0]
	add			rbx, 32
	movq		mm2, mm4
	psrld		mm6, mm0
	psubd		mm2, mm6
	movq		[rdi +  0], mm2
	movq		[rdi +  8], mm2
	movq		[rdi + 16], mm2
	movq		[rdi + 24], mm2
	add			rdi, 32

	;; 本体 (blks_x - 1) * bsiz_x ピクセル.
	cmp			rsi, rdx
	jae			X1
X0:
	pxor		mm0, mm0
	movd		mm0, DWORD PTR [rsi - 4]
	pxor		mm1, mm1
	movd		mm1, DWORD PTR [rsi]
	add			rsi, 4
	movq		mm3, mm1
	punpckldq	mm4, mm4			;; s0 * bsiz_x : s0 * bsiz_x
	psubd		mm1, mm0			;; ds (= s1 - s0)
	pxor		mm0, mm0
	punpckldq	mm0, mm1			;; ds : 0
	punpckldq	mm1, mm1			;; ds : ds
	paddd		mm4, mm0			;; s0 * bsiz_x + 1ds : + 0ds
	paddd		mm1, mm1			;; 2ds : 2ds
	movq		mm5, mm4
	paddd		mm5, mm1			;; s0 * bsiz_x + 3ds : + 2ds
	paddd		mm1, mm1			;; 4ds : 4ds
	movd		mm0, ecx

	movq		mm6, [rbx +  0]
	movq		mm2, mm4
	psrld		mm6, mm0
	movq		mm7, [rbx +  8]
	movq		mm3, mm5
	psrld		mm7, mm0
	psubd		mm2, mm6
	psubd		mm3, mm7
	movq		[rdi +  0], mm2
	paddd		mm4, mm1
	movq		[rdi +  8], mm3
	paddd		mm5, mm1
	
	movq		mm6, [rbx + 16]
	movq		mm2, mm4
	psrld		mm6, mm0
	movq		mm7, [rbx + 24]
	movq		mm3, mm5
	psrld		mm7, mm0
	psubd		mm2, mm6
	psubd		mm3, mm7
	movq		[rdi + 16], mm2
	paddd		mm4, mm1
	movq		[rdi + 24], mm3
	paddd		mm5, mm1

	movq		mm6, [rbx + 32]
	movq		mm2, mm4
	psrld		mm6, mm0
	movq		mm7, [rbx + 40]
	movq		mm3, mm5
	psrld		mm7, mm0
	psubd		mm2, mm6
	psubd		mm3, mm7
	movq		[rdi + 32], mm2
	paddd		mm4, mm1
	movq		[rdi + 40], mm3
	paddd		mm5, mm1

	movq		mm6, [rbx + 48]
	movq		mm2, mm4
	psrld		mm6, mm0
	movq		mm7, [rbx + 56]
	movq		mm3, mm5
	psrld		mm7, mm0
	psubd		mm2, mm6
	psubd		mm3, mm7
	movq		[rdi + 48], mm2
	paddd		mm4, mm1
	movq		[rdi + 56], mm3

	add			rbx, 64
	add			rdi, 64
	cmp			rsi, rdx
	punpckldq	mm4, mm4
	jb			X0

	;; 右端. 少なくとも bsiz_x / 2 (>= 8) 個以上は残っていることに注意.
X1:
	mov			rdx, [rsp + 32]
	sub			rdx, 15
	cmp			rdi, rdx
	jae			X3
	movq		mm6, [rbx]
	psrld		mm6, mm0			;; MM0 には scal_y が入っている.
	psubd		mm4, mm6
X2:
	movq		[rdi + 0], mm4
	movq		[rdi + 8], mm4
	add			rdi, 16
	cmp			rdi, rdx
	jb			X2
X3:
	add			rdx, 15
	cmp			rdi, rdx
	jae			X5
X4:
	movd		DWORD PTR [rdi], mm4
	add			rdi, 4
	cmp			rdi, rdx
	jb			X4
X5:
	mov			[rsp + 8], rsi

	;; 戻り値 srcptr
	mov			rax, rsi
	
	;; レジスタ復帰
	mov			rbx, r8
	mov			rsi, r9
	mov			rdi, r10

ret

expand_xdiff_4 ENDP


expand_x_5 PROC

;; 引数のスタック戻し
mov		[rsp + 8], rcx				;; const ulong_t* srcptr
mov		[rsp + 16], rdx				;; const ulong_t* srcend
mov		[rsp + 24], r8				;; ulong_t* dstptr
mov		[rsp + 32], r9				;; ulong_t* dstend
									;; ulong_t bsiz_y

	;; レジスタ待避
	mov			r8, rbx
	mov			r9, rsi
	mov			r10, rdi
	
	;; src には blks_x = size_x / bsiz_x 個分のデータが含まれる.
	;; size_x % bsiz_x != 0 のとき, 右側の半端なピクセルには, すべて最
	;; 後のデータを埋めることにした.

	mov			rsi, [rsp + 8]
	mov			rdx, [rsp + 16]
	mov			rdi, [rsp + 24]
	bsf			ecx, [rsp + 40]
	;; DIFF == 0 のときは, MM7 に Y 方向のシフトカウントを入れておく.
	pxor		mm7, mm7
	movd		mm7, ecx

	pxor		mm4, mm4
	movd		mm4, DWORD PTR [rsi]
	add			rsi, 4
	;; MM0 に常にひとつ前のデータを入れておき,
	;; 次のデータとの差を計算するのに使用する.
	;; DIFF != 0 のときは, MMX レジスタに空きがないため,
	;; その都度読み直す必要がある. (多分キャッシュに残っているハズ.)
	pslld		mm4, mm7
	movq		mm0, mm4
	punpckldq	mm4, mm4
	pslld		mm4, 5

	movq		[rdi +  0], mm4
	movq		[rdi +  8], mm4
	movq		[rdi + 16], mm4
	movq		[rdi + 24], mm4
	movq		[rdi + 32], mm4
	movq		[rdi + 40], mm4
	movq		[rdi + 48], mm4
	movq		[rdi + 56], mm4
	add			rdi, 64

	;; 本体 (blks_x - 1) * bsiz_x ピクセル.
	cmp			rsi, rdx
	jae			X1
X0:
	pxor		mm1, mm1
	movd		mm1, DWORD PTR [rsi]
	pslld		mm1, mm7
	add			rsi, 4
	movq		mm3, mm1
	punpckldq	mm4, mm4			;; s0 * bsiz_x : s0 * bsiz_x
	psubd		mm1, mm0			;; ds (= s1 - s0)
	pxor		mm0, mm0
	punpckldq	mm0, mm1			;; ds : 0
	punpckldq	mm1, mm1			;; ds : ds
	paddd		mm4, mm0			;; s0 * bsiz_x + 1ds : + 0ds
	paddd		mm1, mm1			;; 2ds : 2ds
	movq		mm5, mm4
	paddd		mm5, mm1			;; s0 * bsiz_x + 3ds : + 2ds
	paddd		mm1, mm1			;; 4ds : 4ds
	movq		mm0, mm3

	movq		[rdi +  0], mm4
	paddd		mm4, mm1
	movq		[rdi +  8], mm5
	paddd		mm5, mm1
	movq		[rdi + 16], mm4
	paddd		mm4, mm1
	movq		[rdi + 24], mm5
	paddd		mm5, mm1
	movq		[rdi + 32], mm4
	paddd		mm4, mm1
	movq		[rdi + 40], mm5
	paddd		mm5, mm1
	movq		[rdi + 48], mm4
	paddd		mm4, mm1
	movq		[rdi + 56], mm5
	paddd		mm5, mm1
	movq		[rdi + 64], mm4
	paddd		mm4, mm1
	movq		[rdi + 72], mm5
	paddd		mm5, mm1
	movq		[rdi + 80], mm4
	paddd		mm4, mm1
	movq		[rdi + 88], mm5
	paddd		mm5, mm1
	movq		[rdi + 96], mm4
	paddd		mm4, mm1
	movq		[rdi + 104], mm5
	paddd		mm5, mm1
	movq		[rdi + 112], mm4
	paddd		mm4, mm1
	movq		[rdi + 120], mm5
	
	add			rdi, 128
	cmp			rsi, rdx
	punpckldq	mm4, mm4
	jb			X0

	;; 右端. 少なくとも bsiz_x / 2 (>= 8) 個以上は残っていることに注意.
X1:
	mov			rdx, [rsp + 32]
	sub			rdx, 15
	cmp			rdi, rdx
	jae			X3
X2:
	movq		[rdi + 0], mm4
	movq		[rdi + 8], mm4
	add			rdi, 16
	cmp			rdi, rdx
	jb			X2
X3:
	add			rdx, 15
	cmp			rdi, rdx
	jae			X5
X4:
	movd		DWORD PTR [rdi], mm4
	add			rdi, 4
	cmp			rdi, rdx
	jb			X4
X5:
	mov			[rsp + 8], rsi

	;; 戻り値 srcptr
	mov			rax, rsi

	;; レジスタ復帰
	mov			rbx, r8
	mov			rsi, r9
	mov			rdi, r10

ret

expand_x_5 ENDP


expand_xdiff_5 PROC

;; 引数のスタック戻し
mov		[rsp + 8], rcx				;; const ulong_t* srcptr
mov		[rsp + 16], rdx				;; const ulong_t* srcend
mov		[rsp + 24], r8				;; ulong_t*	dstptr
mov		[rsp + 32], r9				;; ulong_t* dstend
									;; const ulong_t* basptr
									;; ulong_t bsiz_y

	;; レジスタ待避
	mov			r8, rbx
	mov			r9, rsi
	mov			r10, rdi
	
	;; src には blks_x = size_x / bsiz_x 個分のデータが含まれる.
	;; size_x % bsiz_x != 0 のとき, 右側の半端なピクセルには, すべて最
	;; 後のデータを埋めることにした.

	mov			rsi, [rsp + 8]
	mov			rdx, [rsp + 16]
	mov			rdi, [rsp + 24]
	mov			rbx, [rsp + 40]
	bsf			ecx, [rsp + 48]

	pxor		mm4, mm4
	movd		mm4, DWORD PTR [rsi]
	add			rsi, 4
	punpckldq	mm4, mm4
	pslld		mm4, 5

	;; 左端.
	pxor		mm0, mm0
	movd		mm0, ecx
	movq		mm6, [rbx + 0]
	add			rbx, 64
	movq		mm2, mm4
	psrld		mm6, mm0
	psubd		mm2, mm6
	movq		[rdi +  0], mm2
	movq		[rdi +  8], mm2
	movq		[rdi + 16], mm2
	movq		[rdi + 24], mm2
	movq		[rdi + 32], mm2
	movq		[rdi + 40], mm2
	movq		[rdi + 48], mm2
	movq		[rdi + 56], mm2
	add			rdi, 64

	;; 本体 (blks_x - 1) * bsiz_x ピクセル.
	cmp			rsi, rdx
	jae			X1
X0:
	pxor		mm0, mm0
	movd		mm0, DWORD PTR [rsi - 4]
	pxor		mm1, mm1
	movd		mm1, DWORD PTR [rsi]
	add			rsi, 4
	movq		mm3, mm1
	punpckldq	mm4, mm4			;; s0 * bsiz_x : s0 * bsiz_x
	psubd		mm1, mm0			;; ds (= s1 - s0)
	pxor		mm0, mm0
	punpckldq	mm0, mm1			;; ds : 0
	punpckldq	mm1, mm1			;; ds : ds
	paddd		mm4, mm0			;; s0 * bsiz_x + 1ds : + 0ds
	paddd		mm1, mm1			;; 2ds : 2ds
	movq		mm5, mm4
	paddd		mm5, mm1			;; s0 * bsiz_x + 3ds : + 2ds
	paddd		mm1, mm1			;; 4ds : 4ds
	pxor		mm0, mm0
	movd		mm0, ecx

	movq		mm6, [rbx +  0]
	movq		mm2, mm4
	psrld		mm6, mm0
	movq		mm7, [rbx +  8]
	movq		mm3, mm5
	psrld		mm7, mm0
	psubd		mm2, mm6
	psubd		mm3, mm7
	movq		[rdi +  0], mm2
	paddd		mm4, mm1
	movq		[rdi +  8], mm3
	paddd		mm5, mm1

	movq		mm6, [rbx + 16]
	movq		mm2, mm4
	psrld		mm6, mm0
	movq		mm7, [rbx + 24]
	movq		mm3, mm5
	psrld		mm7, mm0
	psubd		mm2, mm6
	psubd		mm3, mm7
	movq		[rdi + 16], mm2
	paddd		mm4, mm1
	movq		[rdi + 24], mm3
	paddd		mm5, mm1

	movq		mm6, [rbx + 32]
	movq		mm2, mm4
	psrld		mm6, mm0
	movq		mm7, [rbx + 40]
	movq		mm3, mm5
	psrld		mm7, mm0
	psubd		mm2, mm6
	psubd		mm3, mm7
	movq		[rdi + 32], mm2
	paddd		mm4, mm1
	movq		[rdi + 40], mm3
	paddd		mm5, mm1

	movq		mm6, [rbx + 48]
	movq		mm2, mm4
	psrld		mm6, mm0
	movq		mm7, [rbx + 56]
	movq		mm3, mm5
	psrld		mm7, mm0
	psubd		mm2, mm6
	psubd		mm3, mm7
	movq		[rdi + 48], mm2
	paddd		mm4, mm1
	movq		[rdi + 56], mm3
	paddd		mm5, mm1

	movq		mm6, [rbx + 64]
	movq		mm2, mm4
	psrld		mm6, mm0
	movq		mm7, [rbx + 72]
	movq		mm3, mm5
	psrld		mm7, mm0
	psubd		mm2, mm6
	psubd		mm3, mm7
	movq		[rdi + 64], mm2
	paddd		mm4, mm1
	movq		[rdi + 72], mm3
	paddd		mm5, mm1

	movq		mm6, [rbx + 80]
	movq		mm2, mm4
	psrld		mm6, mm0
	movq		mm7, [rbx + 88]
	movq		mm3, mm5
	psrld		mm7, mm0
	psubd		mm2, mm6
	psubd		mm3, mm7
	movq		[rdi + 80], mm2
	paddd		mm4, mm1
	movq		[rdi + 88], mm3
	paddd		mm5, mm1

	movq		mm6, [rbx + 96]
	movq		mm2, mm4
	psrld		mm6, mm0
	movq		mm7, [rbx + 104]
	movq		mm3, mm5
	psrld		mm7, mm0
	psubd		mm2, mm6
	psubd		mm3, mm7
	movq		[rdi + 96], mm2
	paddd		mm4, mm1
	movq		[rdi + 104], mm3
	paddd		mm5, mm1

	movq		mm6, [rbx + 112]
	movq		mm2, mm4
	psrld		mm6, mm0
	movq		mm7, [rbx + 120]
	movq		mm3, mm5
	psrld		mm7, mm0
	psubd		mm2, mm6
	psubd		mm3, mm7
	movq		[rdi + 112], mm2
	paddd		mm4, mm1
	movq		[rdi + 120], mm3
	
	add			rbx, 128
	add			rdi, 128
	cmp			rsi, rdx
	punpckldq	mm4, mm4
	jb			X0

	;; 右端. 少なくとも bsiz_x / 2 (>= 8) 個以上は残っていることに注意.
X1:
	mov			rdx, [rsp + 32]
	sub			rdx, 15
	cmp			rdi, rdx
	jae			X3
	movq		mm6, [rbx]
	psrld		mm6, mm0			;; MM0 には scal_y が入っている.
	psubd		mm4, mm6
X2:
	movq		[rdi + 0], mm4
	movq		[rdi + 8], mm4
	add			rdi, 16
	cmp			rdi, rdx
	jb			X2
X3:
	add			rdx, 15
	cmp			rdi, rdx
	jae			X5
X4:
	movd		DWORD PTR [rdi], mm4
	add			rdi, 4
	cmp			rdi, rdx
	jb			X4
X5:
	mov			[rsp + 8], rsi

	;; 戻り値 srcptr
	mov			rax, rsi

	;; レジスタ復帰
	mov			rbx, r8
	mov			rsi, r9
	mov			rdi, r10

ret

expand_xdiff_5 ENDP


lpf_emms PROC

;; 引数のスタック戻し
mov		[rsp + 8], rcx				;; 
mov		[rsp + 16], rdx				;; 
mov		[rsp + 24], r8				;; 
mov		[rsp + 32], r9				;; 

	;; レジスタ待避
	mov			r8, rbx
	mov			r9, rsi
	mov			r10, rdi

	;; レジスタ復帰
	mov			rbx, r8
	mov			rsi, r9
	mov			rdi, r10

	emms

ret

lpf_emms ENDP



end
