.code



reduce_0 PROC

;; �����̃X�^�b�N�߂�
mov		[rsp + 8], rcx				;; const ubyte_t *srcptr
mov		[rsp + 16], rdx				;; uword_t *dstptr
mov		[rsp + 24], r8				;; uword_t *dstend
mov		[rsp + 32], r9				;; ulong_t size_x
									;; ulong_t bsiz_r
									;; ulong_t blks_x
									
	;; ���W�X�^�Ҕ�
	mov			r8, rbx
	mov			r9, rsi
	mov			r10, rdi

	pxor		mm0, mm0			;; MM0 �͏�� 0 ��ێ�����.
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
	;; ���̂Ƃ���, R1 ����̃��[�v�ŕ����u���b�N���������邱�Ƃ�
	;; �Ȃ�̂�, �E�[���͂ݏo���Ȃ��悤�Ƀ`�F�b�N���Ă���.
	;; LEA ���߂� -7 ���������R�ɂ��.
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
	
	;; ���W�X�^���A
	mov			rbx, r8
	mov			rsi, r9
	mov			rdi, r10

ret

reduce_0 ENDP



reduce_1 PROC

;; �����̃X�^�b�N�߂�
mov		[rsp + 8], rcx				;; const ubyte_t *srcptr
mov		[rsp + 16], rdx				;; uword_t *dstptr
mov		[rsp + 24], r8				;; uword_t *dstend
mov		[rsp + 32], r9				;; ulong_t size_x
									;; ulong_t bsiz_r
									;; ulong_t blks_x
									
	;; ���W�X�^�Ҕ�
	mov			r8, rbx
	mov			r9, rsi
	mov			r10, rdi

	pxor		mm0, mm0			;; MM0 �͏�� 0 ��ێ�����.
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
	;; ���̂Ƃ���, R1 ����̃��[�v�ŕ����u���b�N���������邱�Ƃ�
	;; �Ȃ�̂�, �E�[���͂ݏo���Ȃ��悤�Ƀ`�F�b�N���Ă���.
	;; LEA ���߂� -7 ���������R�ɂ��.
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

	;; ���W�X�^���A
	mov			rbx, r8
	mov			rsi, r9
	mov			rdi, r10

ret

reduce_1 ENDP


reduce_2 PROC

;; �����̃X�^�b�N�߂�
mov		[rsp + 8], rcx				;; const ubyte_t *srcptr
mov		[rsp + 16], rdx				;; uword_t *dstptr
mov		[rsp + 24], r8				;; uword_t *dstend
mov		[rsp + 32], r9				;; ulong_t size_x
									;; ulong_t bsiz_r
									;; ulong_t blks_x
									
	;; ���W�X�^�Ҕ�
	mov			r8, rbx
	mov			r9, rsi
	mov			r10, rdi

	pxor		mm0, mm0			;; MM0 �͏�� 0 ��ێ�����.
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
	;; ���̂Ƃ���, R1 ����̃��[�v�ŕ����u���b�N���������邱�Ƃ�
	;; �Ȃ�̂�, �E�[���͂ݏo���Ȃ��悤�Ƀ`�F�b�N���Ă���.
	;; LEA ���߂� -7 ���������R�ɂ��.
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
	
	;; ���W�X�^���A
	mov			rbx, r8
	mov			rsi, r9
	mov			rdi, r10

ret

reduce_2 ENDP


reduce_3 PROC

;; �����̃X�^�b�N�߂�
mov		[rsp + 8], rcx				;; const ubyte_t *srcptr
mov		[rsp + 16], rdx				;; uword_t *dstptr
mov		[rsp + 24], r8				;; uword_t *dstend
mov		[rsp + 32], r9				;; ulong_t size_x
									;; ulong_t bsiz_r
									;; ulong_t blks_x
									
	;; ���W�X�^�Ҕ�
	mov			r8, rbx
	mov			r9, rsi
	mov			r10, rdi

	pxor		mm0, mm0			;; MM0 �͏�� 0 ��ێ�����.
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

	;; bsiz_x >= 16 �̂Ƃ�, �s�N�Z���l�̘a�͍ő� 17 �r�b�g�ȏ�ƂȂ邪,
	;; mm4:mm5 �̊e���[�h�� 8 ��������Ă���̂�, �܂��܂��]�T������.
	;; �܂�, �����Ȃ� mm4:mm5 �����Z���Ă�, �I�[�o�t���[�̐S�z�͂Ȃ�.
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

	;; ���W�X�^���A
	mov			rbx, r8
	mov			rsi, r9
	mov			rdi, r10

ret

reduce_3 ENDP


reduce_4 PROC

;; �����̃X�^�b�N�߂�
mov		[rsp + 8], rcx				;; const ubyte_t *srcptr
mov		[rsp + 16], rdx				;; ulong_t *dstptr
mov		[rsp + 24], r8				;; ulong_t *dstend
mov		[rsp + 32], r9				;; ulong_t size_x
									;; ulong_t bsiz_r
									;; ulong_t blks_x
									
	;; ���W�X�^�Ҕ�
	mov			r8, rbx
	mov			r9, rsi
	mov			r10, rdi

	pxor		mm0, mm0			;; MM0 �͏�� 0 ��ێ�����.
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

	;; bsiz_x >= 16 �̂Ƃ�, �s�N�Z���l�̘a�͍ő� 17 �r�b�g�ȏ�ƂȂ邪,
	;; mm4:mm5 �̊e���[�h�� 8 ��������Ă���̂�, �܂��܂��]�T������.
	;; �܂�, �����Ȃ� mm4:mm5 �����Z���Ă�, �I�[�o�t���[�̐S�z�͂Ȃ�.
	paddw		mm4, mm5
	movq		mm5, mm4			;; 4:aabbccdd 5:aabbccdd
	punpcklwd	mm4, mm0			;; 4:aa00bb00
	punpckhwd	mm5, mm0			;;            5:cc00dd00
	paddd		mm4, mm5			;; 4:ac00bd00
	movq		mm5, mm4			;;            5:ac00bd00
	punpckhdq	mm5, mm0			;;            5:bd000000
	paddd		mm4, mm5			;; 4:ad000000
	;; ���̂Ƃ���, ulong_t �̔z��ł���.
	movd		DWORD PTR [rdi], mm4
	add			rdi, 4
	cmp			rdi, rbx
	jb			R1

	cmp			rdi, [rsp + 24]
	jb			R0

	;; ���W�X�^���A
	mov			rbx, r8
	mov			rsi, r9
	mov			rdi, r10

ret

reduce_4 ENDP


reduce_5 PROC

;; �����̃X�^�b�N�߂�
mov		[rsp + 8], rcx				;; const ubyte_t *srcptr
mov		[rsp + 16], rdx				;; ulong_t *dstptr
mov		[rsp + 24], r8				;; ulong_t *dstend
mov		[rsp + 32], r9				;; ulong_t size_x
									;; ulong_t bsiz_r
									;; ulong_t blks_x
									
	;; ���W�X�^�Ҕ�
	mov			r8, rbx
	mov			r9, rsi
	mov			r10, rdi

	pxor		mm0, mm0			;; MM0 �͏�� 0 ��ێ�����.
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

	;; bsiz_x >= 16 �̂Ƃ�, �s�N�Z���l�̘a�͍ő� 17 �r�b�g�ȏ�ƂȂ邪,
	;; mm4:mm5 �̊e���[�h�� 8 ��������Ă���̂�, �܂��܂��]�T������.
	;; �܂�, �����Ȃ� mm4:mm5 �����Z���Ă�, �I�[�o�t���[�̐S�z�͂Ȃ�.
	paddw		mm4, mm5
	movq		mm5, mm4			;; 4:aabbccdd 5:aabbccdd
	punpcklwd	mm4, mm0			;; 4:aa00bb00
	punpckhwd	mm5, mm0			;;            5:cc00dd00
	paddd		mm4, mm5			;; 4:ac00bd00
	movq		mm5, mm4			;;            5:ac00bd00
	punpckhdq	mm5, mm0			;;            5:bd000000
	paddd		mm4, mm5			;; 4:ad000000
	;; ���̂Ƃ���, ulong_t �̔z��ł���.
	movd		DWORD PTR [rdi], mm4
	add			rdi, 4
	cmp			rdi, rbx
	jb			R1

	cmp			rdi, [rsp + 24]
	jb			R0

	;; ���W�X�^���A
	mov			rbx, r8
	mov			rsi, r9
	mov			rdi, r10

ret

reduce_5 ENDP


expand_xy PROC

;; �����̃X�^�b�N�߂�
mov		[rsp + 8], rcx				;; ubyte_t* dstptr
mov		[rsp + 16], rdx				;; ulong_t* srcptr
mov		[rsp + 24], r8				;; const ulong_t* difptr
mov		[rsp + 32], r9				;; ulong_t size_x
									;; ulong_t bsiz_y
									;; ulong_t scale

	;; ���W�X�^�Ҕ�
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

	;; �ȉ��̃��W�X�^�̎g�p�|���V�[��, ���W�X�^������Ȃ����ߏ��X�킩��ɂ���.
	;; �ŏ��̑g�� MM4:MM5, ���̑g�� MM5:MM6 ���y�A�Ƃ��Ďg�p���Ă���.
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

	;; �߂�l dstptr
	mov			rax, rdi

	;; ���W�X�^���A
	mov			rbx, r8
	mov			rsi, r9
	mov			rdi, r10

ret

expand_xy ENDP


iscalb PROC

;; �����̃X�^�b�N�߂�
mov		[rsp + 8], rcx				;; ulong_t
mov		[rsp + 16], rdx				;; 
mov		[rsp + 24], r8				;; 
mov		[rsp + 32], r9				;; 
									
	;; ���W�X�^�Ҕ�
	mov			r8, rbx
	mov			r9, rsi
	mov			r10, rdi

	;; rcx�̍ŉ��ʃZ�b�g�r�b�g���������āA���̃r�b�g�E�C���f�b�N�X��rax�ɃX�g�A
	bsf			rax, [rsp + 8]

	;; ���W�X�^���A
	mov			rbx, r8
	mov			rsi, r9
	mov			rdi, r10

ret

iscalb ENDP



expand_x_0 PROC

;; �����̃X�^�b�N�߂�
mov		[rsp + 8], rcx				;; const uword_t* srcptr
mov		[rsp + 16], rdx				;; const uword_t* srcend
mov		[rsp + 24], r8				;; ulong_t* dstptr
mov		[rsp + 32], r9				;; ulong_t* dstend
									;; ulong_t bsiz_y

	;; ���W�X�^�Ҕ�
	mov			r8, rbx
	mov			r9, rsi
	mov			r10, rdi

	;; src �ɂ� blks_x = size_x / bsiz_x ���̃f�[�^���܂܂��.
	;; size_x % bsiz_x != 0 �̂Ƃ�, �E���̔��[�ȃs�N�Z���ɂ�, ���ׂč�
	;; ��̃f�[�^�𖄂߂邱�Ƃɂ���.

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

	;; BSIZ_X == 1 (SCAL_X == 0) �̂Ƃ�, �E�[�ɔ��[�ȃs�N�Z���͂��蓾�Ȃ�.
X3:
	mov			[rsp + 8], rsi

	;; �߂�l srcptr
	mov			rax, rsi
	
	;; ���W�X�^���A
	mov			rbx, r8
	mov			rsi, r9
	mov			rdi, r10

ret

expand_x_0 ENDP


expand_xdiff_0 PROC

;; �����̃X�^�b�N�߂�
mov		[rsp + 8], rcx				;; const uword_t* srcptr
mov		[rsp + 16], rdx				;; const uword_t* srcend
mov		[rsp + 24], r8				;; ulong_t*	dstptr
mov		[rsp + 32], r9				;; ulong_t* dstend
									;; const ulong_t* basptr
									;; ulong_t bsiz_y

	;; ���W�X�^�Ҕ�
	mov			r8, rbx
	mov			r9, rsi
	mov			r10, rdi
	
	;; src �ɂ� blks_x = size_x / bsiz_x ���̃f�[�^���܂܂��.
	;; size_x % bsiz_x != 0 �̂Ƃ�, �E���̔��[�ȃs�N�Z���ɂ�, ���ׂč�
	;; ��̃f�[�^�𖄂߂邱�Ƃɂ���.

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

	;; BSIZ_X == 1 (SCAL_X == 0) �̂Ƃ�, �E�[�ɔ��[�ȃs�N�Z���͂��蓾�Ȃ�.
X3:
	mov			[rsp + 8], rsi
	
	;; �߂�l srcptr
	mov			rax, rsi
	
	;; ���W�X�^���A
	mov			rbx, r8
	mov			rsi, r9
	mov			rdi, r10

ret

expand_xdiff_0 ENDP


expand_x_4 PROC

;; �����̃X�^�b�N�߂�
mov		[rsp + 8], rcx				;; const uword_t* srcptr
mov		[rsp + 16], rdx				;; const uword_t* srcend
mov		[rsp + 24], r8				;; ulong_t* dstptr
mov		[rsp + 32], r9				;; ulong_t* dstend
									;; ulong_t bsiz_y

	;; ���W�X�^�Ҕ�
	mov			r8, rbx
	mov			r9, rsi
	mov			r10, rdi
	
	;; src �ɂ� blks_x = size_x / bsiz_x ���̃f�[�^���܂܂��.
	;; size_x % bsiz_x != 0 �̂Ƃ�, �E���̔��[�ȃs�N�Z���ɂ�, ���ׂč�
	;; ��̃f�[�^�𖄂߂邱�Ƃɂ���.

	mov			rsi, [rsp + 8]
	mov			rdx, [rsp + 16]
	mov			rdi, [rsp + 24]
	bsf			ecx, [rsp + 40]
	;; DIFF == 0 �̂Ƃ���, MM7 �� Y �����̃V�t�g�J�E���g�����Ă���.
	pxor		mm7, mm7
	movd		mm7, ecx

	pxor		mm4, mm4
	movd		mm4, DWORD PTR [rsi]
	add			rsi, 4
	;; MM0 �ɏ�ɂЂƂO�̃f�[�^�����Ă���,
	;; ���̃f�[�^�Ƃ̍����v�Z����̂Ɏg�p����.
	;; DIFF != 0 �̂Ƃ���, MMX ���W�X�^�ɋ󂫂��Ȃ�����,
	;; ���̓s�x�ǂݒ����K�v������. (�����L���b�V���Ɏc���Ă���n�Y.)
	pslld		mm4, mm7
	movq		mm0, mm4
	punpckldq	mm4, mm4
	pslld		mm4, 4

	;; ���[.
	movq		[rdi +  0], mm4
	movq		[rdi +  8], mm4
	movq		[rdi + 16], mm4
	movq		[rdi + 24], mm4
	add			rdi, 32

	;; �{�� (blks_x - 1) * bsiz_x �s�N�Z��.
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

	;; �E�[. ���Ȃ��Ƃ� bsiz_x / 2 (>= 8) �ȏ�͎c���Ă��邱�Ƃɒ���.
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

	;; �߂�l srcptr
	mov			rax, rsi
	
	;; ���W�X�^���A
	mov			rbx, r8
	mov			rsi, r9
	mov			rdi, r10

ret

expand_x_4 ENDP


expand_xdiff_4 PROC

;; �����̃X�^�b�N�߂�
;; �����̃X�^�b�N�߂�
mov		[rsp + 8], rcx				;; const uword_t* srcptr
mov		[rsp + 16], rdx				;; const uword_t* srcend
mov		[rsp + 24], r8				;; ulong_t*	dstptr
mov		[rsp + 32], r9				;; ulong_t* dstend
									;; const ulong_t* basptr
									;; ulong_t bsiz_y

	;; ���W�X�^�Ҕ�
	mov			r8, rbx
	mov			r9, rsi
	mov			r10, rdi
	
	;; src �ɂ� blks_x = size_x / bsiz_x ���̃f�[�^���܂܂��.
	;; size_x % bsiz_x != 0 �̂Ƃ�, �E���̔��[�ȃs�N�Z���ɂ�, ���ׂč�
	;; ��̃f�[�^�𖄂߂邱�Ƃɂ���.

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

	;; �{�� (blks_x - 1) * bsiz_x �s�N�Z��.
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

	;; �E�[. ���Ȃ��Ƃ� bsiz_x / 2 (>= 8) �ȏ�͎c���Ă��邱�Ƃɒ���.
X1:
	mov			rdx, [rsp + 32]
	sub			rdx, 15
	cmp			rdi, rdx
	jae			X3
	movq		mm6, [rbx]
	psrld		mm6, mm0			;; MM0 �ɂ� scal_y �������Ă���.
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

	;; �߂�l srcptr
	mov			rax, rsi
	
	;; ���W�X�^���A
	mov			rbx, r8
	mov			rsi, r9
	mov			rdi, r10

ret

expand_xdiff_4 ENDP


expand_x_5 PROC

;; �����̃X�^�b�N�߂�
mov		[rsp + 8], rcx				;; const ulong_t* srcptr
mov		[rsp + 16], rdx				;; const ulong_t* srcend
mov		[rsp + 24], r8				;; ulong_t* dstptr
mov		[rsp + 32], r9				;; ulong_t* dstend
									;; ulong_t bsiz_y

	;; ���W�X�^�Ҕ�
	mov			r8, rbx
	mov			r9, rsi
	mov			r10, rdi
	
	;; src �ɂ� blks_x = size_x / bsiz_x ���̃f�[�^���܂܂��.
	;; size_x % bsiz_x != 0 �̂Ƃ�, �E���̔��[�ȃs�N�Z���ɂ�, ���ׂč�
	;; ��̃f�[�^�𖄂߂邱�Ƃɂ���.

	mov			rsi, [rsp + 8]
	mov			rdx, [rsp + 16]
	mov			rdi, [rsp + 24]
	bsf			ecx, [rsp + 40]
	;; DIFF == 0 �̂Ƃ���, MM7 �� Y �����̃V�t�g�J�E���g�����Ă���.
	pxor		mm7, mm7
	movd		mm7, ecx

	pxor		mm4, mm4
	movd		mm4, DWORD PTR [rsi]
	add			rsi, 4
	;; MM0 �ɏ�ɂЂƂO�̃f�[�^�����Ă���,
	;; ���̃f�[�^�Ƃ̍����v�Z����̂Ɏg�p����.
	;; DIFF != 0 �̂Ƃ���, MMX ���W�X�^�ɋ󂫂��Ȃ�����,
	;; ���̓s�x�ǂݒ����K�v������. (�����L���b�V���Ɏc���Ă���n�Y.)
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

	;; �{�� (blks_x - 1) * bsiz_x �s�N�Z��.
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

	;; �E�[. ���Ȃ��Ƃ� bsiz_x / 2 (>= 8) �ȏ�͎c���Ă��邱�Ƃɒ���.
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

	;; �߂�l srcptr
	mov			rax, rsi

	;; ���W�X�^���A
	mov			rbx, r8
	mov			rsi, r9
	mov			rdi, r10

ret

expand_x_5 ENDP


expand_xdiff_5 PROC

;; �����̃X�^�b�N�߂�
mov		[rsp + 8], rcx				;; const ulong_t* srcptr
mov		[rsp + 16], rdx				;; const ulong_t* srcend
mov		[rsp + 24], r8				;; ulong_t*	dstptr
mov		[rsp + 32], r9				;; ulong_t* dstend
									;; const ulong_t* basptr
									;; ulong_t bsiz_y

	;; ���W�X�^�Ҕ�
	mov			r8, rbx
	mov			r9, rsi
	mov			r10, rdi
	
	;; src �ɂ� blks_x = size_x / bsiz_x ���̃f�[�^���܂܂��.
	;; size_x % bsiz_x != 0 �̂Ƃ�, �E���̔��[�ȃs�N�Z���ɂ�, ���ׂč�
	;; ��̃f�[�^�𖄂߂邱�Ƃɂ���.

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

	;; ���[.
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

	;; �{�� (blks_x - 1) * bsiz_x �s�N�Z��.
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

	;; �E�[. ���Ȃ��Ƃ� bsiz_x / 2 (>= 8) �ȏ�͎c���Ă��邱�Ƃɒ���.
X1:
	mov			rdx, [rsp + 32]
	sub			rdx, 15
	cmp			rdi, rdx
	jae			X3
	movq		mm6, [rbx]
	psrld		mm6, mm0			;; MM0 �ɂ� scal_y �������Ă���.
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

	;; �߂�l srcptr
	mov			rax, rsi

	;; ���W�X�^���A
	mov			rbx, r8
	mov			rsi, r9
	mov			rdi, r10

ret

expand_xdiff_5 ENDP


lpf_emms PROC

;; �����̃X�^�b�N�߂�
mov		[rsp + 8], rcx				;; 
mov		[rsp + 16], rdx				;; 
mov		[rsp + 24], r8				;; 
mov		[rsp + 32], r9				;; 

	;; ���W�X�^�Ҕ�
	mov			r8, rbx
	mov			r9, rsi
	mov			r10, rdi

	;; ���W�X�^���A
	mov			rbx, r8
	mov			rsi, r9
	mov			rdi, r10

	emms

ret

lpf_emms ENDP



end
