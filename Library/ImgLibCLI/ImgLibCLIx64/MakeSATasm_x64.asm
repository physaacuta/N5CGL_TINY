.code


;;
;; �G���A���a�e�[�u�������֐�
;;

VertAdd PROC

;; �����̃X�^�b�N�߂�
mov			[rsp + 8], rcx				;; const unsigned char* data
mov			[rsp + 16], rdx				;; int w
mov			[rsp + 24], r8				;; unsigned long * acc
;	mov			[rsp + 32], r9				;;

		;; ���W�X�^�Ҕ�
		mov			r8, rbx
		mov			r9, rsi
		mov			r10, rdi 

		;; ����
        mov			rsi, [rsp + 8]												;; esi, data��rsi, [rsp + 8]�ɕύX
		mov			rdi, [rsp + 24]												;; edi, acc��rdi, [rsp + 24]�ɕύX
		pxor		xmm4, xmm4			;; xmm4: zero
		mov			ebx, [rsp + 16]												;; w��[rsp + 16]�ɕύX
		shr			ebx, 5				;; w/32

		;; loop
L1:
		;; 0-15
		movdqa		xmm0, [rsi]			;; 0-15									;; [esi]��[rsi]�ɕύX
		movdqa		xmm2, xmm0			;; �R�s�[
		punpcklbw	xmm0, xmm4			;; byte -> word
		punpckhbw	xmm2, xmm4			;; byte -> word
		movdqa		xmm1, xmm0
		movdqa		xmm3, xmm2
		punpcklwd	xmm0, xmm4			;; word -> dword
		punpckhwd	xmm1, xmm4
		punpcklwd	xmm2, xmm4
		punpckhwd	xmm3, xmm4

		paddd		xmm0, [rdi]			;; 0-3									;; [edi]��[rdi]�ɕύX
		movdqa		[rdi], xmm0													;; [edi]��[rdi]�ɕύX
		paddd		xmm1, [rdi+16]		;; 4-7									;; [edi+16]��[rdi+16]�ɕύX
		movdqa		[rdi+16], xmm1												;; [edi+16]��[rdi+16]�ɕύX
		paddd		xmm2, [rdi+32]		;; 8-11									;; [edi+32]��[rdi+32]�ɕύX
		movdqa		[rdi+32], xmm2												;; [edi+32]��[rdi+32]�ɕύX
		paddd		xmm3, [rdi+48]		;; 12-15								;; [edi+48]��[rdi+48]�ɕύX
		movdqa		[rdi+48], xmm3												;; [edi+48]��[rdi+48]�ɕύX

		;; 16-31
		movdqa		xmm0, [rsi+16]		;; 16-31								;; [esi+16]��[rsi+16]�ɕύX
		movdqa		xmm2, xmm0			;; �R�s�[
		punpcklbw	xmm0, xmm4			;; byte -> word
		punpckhbw	xmm2, xmm4			;; byte -> word
		movdqa		xmm1, xmm0
		movdqa		xmm3, xmm2
		punpcklwd	xmm0, xmm4			;; word -> dword
		punpckhwd	xmm1, xmm4
		punpcklwd	xmm2, xmm4
		punpckhwd	xmm3, xmm4

		paddd		xmm0, [rdi+64]		;; 16-19								;; [edi+64]��[rdi+64]�ɕύX
		movdqa		[rdi+64], xmm0												;; [edi+64]��[rdi+64]�ɕύX
		paddd		xmm1, [rdi+80]		;; 20-23								;; [edi+80]��[rdi+80]�ɕύX
		movdqa		[rdi+80], xmm1												;; [edi+80]��[rdi+80]�ɕύX
		paddd		xmm2, [rdi+96]		;; 24-27								;; [edi+96]��[rdi+96]�ɕύX
		movdqa		[rdi+96], xmm2												;; [edi+96]��[rdi+96]�ɕύX
		paddd		xmm3, [rdi+112]		;; 28-31								;; [edi+112]��[rdi+112]�ɕύX
		movdqa		[rdi+112], xmm3												;; [edi+112]��[rdi+112]�ɕύX

		add			rsi, 32				;; ����32��								;; esi��rsi�ɕύX
		add			rdi, 128													;; edi��rdi�ɕύX

		dec			ebx
		jnz			L1
		
		;; ���W�X�^���A
		mov			rbx, r8
		mov			rsi, r9
		mov			rdi, r10
ret

VertAdd ENDP


VertAddLimit PROC

;; �����̃X�^�b�N�߂�
mov			[rsp + 8], rcx				;; const unsigned char* data
mov			[rsp + 16], rdx				;; int w
mov			[rsp + 24], r8				;; unsigned long * acc
mov			[rsp + 32], r9				;; unsigned char lower
										;; unsigned char upper

push		rbp
mov			rbp, rsp		;; �ȍ~�A�������Q�Ƃ���Ƃ���rbp���g���A�A�h���X��+8����Bleave���g���̂�rbp��ύX���Ă̓_���B

	sub			rsp, 16
	movdqu		[rbp - 16], xmm7
	
		;; ���W�X�^�Ҕ�
		mov			r8, rbx
		mov			r9, rsi
		mov			r10, rdi

		;; ����
        mov			rsi, [rbp + 8 + 8]												;; esi, data��rsi, [rsp + 8]�ɕύX
		mov			rdi, [rbp + 8 + 24]												;; edi, acc��rdi, [rsp + 24]�ɕύX
		pxor		xmm7, xmm7			;; xmm7: zero
		mov			ebx, [rbp + 8 + 16]												;; w��[rsp + 16]�ɕύX
		shr			ebx, 5				;; w/32
		movzx		eax, BYTE PTR [rbp + 8 + 32]									;; BYTE PTR lower��BYTE PTR [rsp + 32]�ɕύX
		movd		xmm4, eax
		punpcklbw	xmm4, xmm4
		punpcklwd	xmm4, xmm4
		punpckldq	xmm4, xmm4
		punpcklqdq	xmm4, xmm4
		movzx		eax, BYTE PTR [rbp + 8 + 40]									;; BYTE PTR upper��BYTE PTR [rsp + 40]�ɕύX
		movd		xmm5, eax
		punpcklbw	xmm5, xmm5
		punpcklwd	xmm5, xmm5
		punpckldq	xmm5, xmm5
		punpcklqdq	xmm5, xmm5

		;; loop
L1:
		;; 0-15
		movdqa		xmm0, [rsi]			;; 0-15									;; [esi]��[rsi]�ɕύX
		pmaxub		xmm0, xmm4
		pminub		xmm0, xmm5
		movdqa		xmm2, xmm0			;; �R�s�[
		punpcklbw	xmm0, xmm7			;; byte -> word
		punpckhbw	xmm2, xmm7			;; byte -> word
		movdqa		xmm1, xmm0
		movdqa		xmm3, xmm2
		punpcklwd	xmm0, xmm7			;; word -> dword
		punpckhwd	xmm1, xmm7
		punpcklwd	xmm2, xmm7
		punpckhwd	xmm3, xmm7

		paddd		xmm0, [rdi]			;; 0-3									;; [edi]��[rdi]�ɕύX
		movdqa		[rdi], xmm0													;; [edi]��[rdi]�ɕύX
		paddd		xmm1, [rdi+16]		;; 4-7									;; [edi+16]��[rdi+16]�ɕύX
		movdqa		[rdi+16], xmm1												;; [edi+16]��[rdi+16]�ɕύX
		paddd		xmm2, [rdi+32]		;; 8-11									;; [edi+32]��[rdi+32]�ɕύX
		movdqa		[rdi+32], xmm2												;; [edi+32]��[rdi+32]�ɕύX
		paddd		xmm3, [rdi+48]		;; 12-15								;; [edi+48]��[rdi+48]�ɕύX
		movdqa		[rdi+48], xmm3												;; [edi+48]��[rdi+48]�ɕύX

		;; 16-31
		movdqa		xmm0, [rsi+16]		;; 16-31								;; [esi+16]��[rsi+16]�ɕύX
		pmaxub		xmm0, xmm4
		pminub		xmm0, xmm5
		movdqa		xmm2, xmm0			;; �R�s�[
		punpcklbw	xmm0, xmm7			;; byte -> word
		punpckhbw	xmm2, xmm7			;; byte -> word
		movdqa		xmm1, xmm0
		movdqa		xmm3, xmm2
		punpcklwd	xmm0, xmm7			;; word -> dword
		punpckhwd	xmm1, xmm7
		punpcklwd	xmm2, xmm7
		punpckhwd	xmm3, xmm7

		paddd		xmm0, [rdi+64]		;; 16-19								;; [edi+64]��[rdi+64]�ɕύX
		movdqa		[rdi+64], xmm0												;; [edi+64]��[rdi+64]�ɕύX
		paddd		xmm1, [rdi+80]		;; 20-23								;; [edi+80]��[rdi+80]�ɕύX
		movdqa		[rdi+80], xmm1												;; [edi+80]��[rdi+80]�ɕύX
		paddd		xmm2, [rdi+96]		;; 24-27								;; [edi+96]��[rdi+96]�ɕύX
		movdqa		[rdi+96], xmm2												;; [edi+96]��[rdi+96]�ɕύX
		paddd		xmm3, [rdi+112]		;; 28-31								;; [edi+112]��[rdi+112]�ɕύX
		movdqa		[rdi+112], xmm3												;; [edi+112]��[rdi+112]�ɕύX

		add			rsi, 32				;; ����32��								;; esi��rsi�ɕύX
		add			rdi, 128													;; edi��rdi�ɕύX

		dec			ebx
		jnz			L1
		
		;; ���W�X�^���A
		mov			rbx, r8
		mov			rsi, r9
		mov			rdi, r10
	;;
	movdqu		xmm7, [rbp - 16]

leave
ret

VertAddLimit ENDP


HorzAccum PROC

;; �����̃X�^�b�N�߂�
mov			[rsp + 8], rcx				;; unsigned long * va
mov			[rsp + 16], rdx				;; int w
mov			[rsp + 24], r8				;; unsigned long * sat

		;; ���W�X�^�Ҕ�
		mov			r8, rbx
		mov			r9, rsi
		mov			r10, rdi

		mov			rsi, [rsp + 8]
		mov			rdi, [rsp + 24]
		mov			ebx, [rsp + 16]
		xor			rcx, rcx
		xor			eax, eax			;; accumulator
L1:
		add			eax, [rsi+4*rcx]
		movnti		[rdi+4*rcx], eax
		add			eax, [rsi+4*rcx+4]
		movnti		[rdi+4*rcx+4], eax
		add			eax, [rsi+4*rcx+8]
		movnti		[rdi+4*rcx+8], eax
		add			eax, [rsi+4*rcx+12]
		movnti		[rdi+4*rcx+12], eax
		add			eax, [rsi+4*rcx+16]
		movnti		[rdi+4*rcx+16], eax
		add			eax, [rsi+4*rcx+20]
		movnti		[rdi+4*rcx+20], eax
		add			eax, [rsi+4*rcx+24]
		movnti		[rdi+4*rcx+24], eax
		add			eax, [rsi+4*rcx+28]
		movnti		[rdi+4*rcx+28], eax

		add			ecx, 8
		cmp			ecx, ebx
		jnz			L1

		;; ���W�X�^���A
		mov			rbx, r8
		mov			rsi, r9
		mov			rdi, r10

ret
HorzAccum ENDP


HorzAccumStep8 PROC

;; �����̃X�^�b�N�߂�
mov			[rsp + 8], rcx				;; unsigned long * va
mov			[rsp + 16], rdx				;; int w
mov			[rsp + 24], r8				;; int step
mov			[rsp + 32], r9				;; unsigned long * sat

		;; ���W�X�^�Ҕ�
		mov			r8, rbx
		mov			r9, rsi
		mov			r10, rdi

		mov			rsi, [rsp + 8]
		mov			rdi, [rsp + 32]
		mov			ebx, [rsp + 16]
		xor			rcx, rcx
		xor			eax, eax		;; accumulator
L1:
		mov			edx, [rsp + 24]
		shr			edx, 3			;; step / 8
L2:
		add			eax, [rsi+4*rcx]
		add			eax, [rsi+4*rcx+4]
		add			eax, [rsi+4*rcx+8]
		add			eax, [rsi+4*rcx+12]
		add			eax, [rsi+4*rcx+16]
		add			eax, [rsi+4*rcx+20]
		add			eax, [rsi+4*rcx+24]
		add			eax, [rsi+4*rcx+28]
		add			ecx, 8
		dec			edx
		jnz			L2

		mov			[rdi], eax
		add			rdi, 4

		cmp			ecx, ebx
		jnz			L1

		;; ���W�X�^���A
		mov			rbx, r8
		mov			rsi, r9
		mov			rdi, r10

ret

HorzAccumStep8 ENDP


HorzAccumStep PROC

;; �����̃X�^�b�N�߂�
mov			[rsp + 8], rcx				;; unsigned long * va
mov			[rsp + 16], rdx				;; int w
mov			[rsp + 24], r8				;; int step
mov			[rsp + 32], r9				;; unsigned long * sat

		;; ���W�X�^�Ҕ�
		mov			r8, rbx
		mov			r9, rsi
		mov			r10, rdi

		mov			rsi, [rsp + 8]												;; esi, va��rsi, [rsp + 8]�ɕύX
		mov			rdi, [rsp + 32]												;; edi, sat��rdi, [rsp + 32]�ɕύX
		mov			ebx, [rsp + 16]												;; w��[rsp + 16]�ɕύX
		xor			rcx, rcx
		xor			eax, eax			;; accumulator
L1:
		mov			edx, [rsp + 24]												;; step��[rsp + 24]�ɕύX
L2:
		add			eax, [rsi+4*rcx]											;; [esi+4*ecx]��[rsi+4*rcx]�ɕύX
		inc			ecx
		dec			edx
		jnz			L2

		mov			[rdi], eax													;; [edi]��[rdi]�ɕύX
		add			rdi, 4

		cmp			ecx, ebx
		jnae		L1
		
		;; ���W�X�^���A
		mov			rbx, r8
		mov			rsi, r9
		mov			rdi, r10

ret

HorzAccumStep ENDP


MakeSAT PROC

;; �����̃X�^�b�N�߂�
mov			[rsp + 8], rcx				;; const unsigned char* data
mov			[rsp + 16], rdx				;; int w
mov DWORD PTR [rsp + 20], 0
mov			[rsp + 24], r8				;; int h
mov			[rsp + 32], r9				;; unsigned long * sat

		;; ���W�X�^�Ҕ�
		mov			r8, rbx
		mov			r9, rsi
		mov			r10, rdi

		mov			rsi, [rsp + 8]
		mov			rdi, [rsp + 32]

		mov			ebx, [rsp + 16]
		xor			rcx, rcx
		xor			rax, rax		;; accumulator
L1:
		movzx		edx, BYTE PTR [rsi+rcx]
		add			eax, edx
		mov			[rdi+4*rcx], eax
		movzx		edx, BYTE PTR [rsi+rcx+1]
		add			eax, edx
		mov			[rdi+4*rcx+4], eax
		movzx		edx, BYTE PTR [rsi+rcx+2]
		add			eax, edx
		mov			[rdi+4*rcx+8], eax
		movzx		edx, BYTE PTR [rsi+rcx+3]
		add			eax, edx
		mov			[rdi+4*rcx+12], eax

		add			ecx, 4
		cmp			ecx, ebx
		jnz			L1

		add			rsi, [rsp + 16]
		mov			eax, [rsp + 16]
		add			eax, eax
		add			eax, eax	;; w*4
		add			rdi, rax
		mov			ecx, 1		;; pushed ecx : vertical counter
L2:
		push		rcx;
		xor			eax, eax
		xor			ecx, ecx		;; ecx:horizontal counter
		xor			ebx, ebx
		sub			ebx, [rsp + 16]			;; ebx: (-w)
L3:
		movzx		edx, BYTE PTR [rsi+rcx]
		add			eax, edx
		mov			edx, eax
		add			edx, [rdi+4*rbx]
		mov			[rdi+4*rcx], edx
		movzx		edx, BYTE PTR [rsi+rcx+1]
		add			eax, edx
		mov			edx, eax
		add			edx, [rdi+4*rbx+4]
		mov			[rdi+4*rcx+4], edx
		movzx		edx, BYTE PTR [rsi+rcx+2]
		add			eax, edx
		mov			edx, eax
		add			edx, [rdi+4*rbx+8]
		mov			[rdi+4*rcx+8], edx
		movzx		edx, BYTE PTR [rsi+rcx+3]
		add			eax, edx
		mov			edx, eax
		add			edx, [rdi+4*rbx+12]
		mov			[rdi+4*rcx+12], edx

		mov			edx, ecx
		sub			edx, ebx	;; edx: w
		add			ebx, 4
		add			ecx, 4
		cmp			ecx, edx
		jnz			L3

		add			rsi, [rsp + 16]
		mov			eax, [rsp + 16]
		add			eax, eax
		add			eax, eax		;; w*4
		add			rdi, rax

		pop			rcx
		inc			ecx
		cmp			ecx, [rsp + 24]
		jnz			L2

		;; ���W�X�^���A
		mov			rbx, r8
		mov			rsi, r9
		mov			rdi, r10

ret

MakeSAT ENDP


MakeSATline PROC

;; �����̃X�^�b�N�߂�
mov			[rsp + 8], rcx				;; const unsigned char* data
mov			[rsp + 16], rdx				;; int w
mov			[rsp + 24], r8				;; unsigned long * sat
mov			[rsp + 32], r9				;; unsigned long * upper

		;; ���W�X�^�Ҕ�
		mov			r8, rbx
		mov			r9, rsi
		mov			r10, rdi

		mov			rsi, [rsp + 8]
		mov			rdi, [rsp + 24]
		mov			rbx, [rsp + 32]

		xor			rcx, rcx
		xor			eax, eax		;; accumulator
L1:
		movzx		edx, BYTE PTR [rsi+rcx]
		add			eax, edx
		mov			edx, eax
		add			edx, [rbx+4*rcx]
		mov			[rdi+4*rcx], edx
		movzx		edx, BYTE PTR [rsi+rcx+1]
		add			eax, edx
		mov			edx, eax
		add			edx, [rbx+4*rcx+4]
		mov			[rdi+4*rcx+4], edx
		movzx		edx, BYTE PTR [rsi+rcx+2]
		add			eax, edx
		mov			edx, eax
		add			edx, [rbx+4*rcx+8]
		mov			[rdi+4*rcx+8], edx
		movzx		edx, BYTE PTR [rsi+rcx+3]
		add			eax, edx;
		mov			edx, eax;
		add			edx, [rbx+4*rcx+12];
		mov			[rdi+4*rcx+12], edx;

		add			ecx, 4;
		cmp			ecx, [rsp + 16];
		jnz			L1;

		;; ���W�X�^���A
		mov			rbx, r8
		mov			rsi, r9
		mov			rdi, r10

ret

MakeSATline ENDP


LUTline PROC

;; �����̃X�^�b�N�߂�
mov			[rsp + 8], rcx				;; const unsigned char* data
mov			[rsp + 16], rdx				;; int w
mov			[rsp + 24], r8				;; unsigned char * outdata
mov			[rsp + 32], r9				;; unsigned char * lut

		;; ���W�X�^�Ҕ�
		mov			r8, rbx
		mov			r9, rsi
		mov			r10, rdi

		mov			rbx, [rsp + 32]
		mov			edx, [rsp + 16]
		mov			rsi, [rsp + 8]
		mov			rdi, [rsp + 24]
		xor			rcx, rcx
L1:
		movzx		rax, BYTE PTR [rsi+rcx]
		movzx		eax, BYTE PTR [rbx+rax]
		mov			[rdi+rcx], al
		movzx		rax, BYTE PTR [rsi+rcx+1]
		movzx		eax, BYTE PTR [rbx+rax]
		mov			[rdi+rcx+1], al
		movzx		rax, BYTE PTR [rsi+rcx+2]
		movzx		eax, BYTE PTR [rbx+rax]
		mov			[rdi+rcx+2], al
		movzx		rax, BYTE PTR [rsi+rcx+3]
		movzx		eax, BYTE PTR [rbx+rax]
		mov			[rdi+rcx+3], al

		add			ecx, 4
		cmp			ecx, edx
		jnz			L1

		;; ���W�X�^���A
		mov			rbx, r8
		mov			rsi, r9
		mov			rdi, r10

ret

LUTline ENDP


end
