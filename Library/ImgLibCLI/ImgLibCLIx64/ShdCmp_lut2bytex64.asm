.code



lut2byte PROC

;; �����̃X�^�b�N�߂�
mov		[rsp + 8], rcx				;; ubyte_t* quotient
mov		[rsp + 16], rdx				;; const ubyte_t* numerator
mov		[rsp + 24], r8				;; const ubyte_t* denominator
mov		[rsp + 32], r9				;; std::size_t size_x
									;; std::size_t size_y
									;; const ubyte_t* look_up_table

	;; ���W�X�^�Ҕ�
	mov			r8, rbx
	mov			r9, rsi
	mov			r10, rdi
	
	mov			rdi, [rsp + 8]
	mov			rcx, [rsp + 16]
	mov			rdx, [rsp + 24]
	mov			rsi, [rsp + 32]
	imul		esi, [rsp + 40]
	mov			rbx, [rsp + 48]
	sub			esi, 8
	jb			L1

L0:
	movq		mm1, [rcx]					;; ���q
	add			rcx, 8
	movq		mm2, [rdx]					;; ����
	add			rdx, 8
	movq		mm3, mm2
	punpcklbw	mm2, mm1					;; mm2 = ���q3����3 ���q2����2 ���q1����1 ���q0����0 (����4byte�̃A���p�b�N)
	punpckhbw	mm3, mm1					;; mm3 = ���q7����7 ���q6����6 ���q5����5 ���q4����4 (���4byte�̃A���p�b�N)

	movd		rax, mm2					;; rax = �@�@ �@�@  �@�@ �@�@  ���q1����1 ���q0����0
	psrlq		mm2, 16						;; mm2 = �@�@ �@�@  ���q3����3 ���q2����2 ���q1����1
	and			rax, 0FFFFh					;; rax = �@�@ �@�@  �@�@ �@�@  �@�@ �@�@  ���q0����0
	movzx		rax, BYTE PTR [rbx + rax]
	pxor		mm4, mm4
	movd		mm4, rax					;; mm4 = look_up_table [���q0����0]

	movd		rax, mm2					;; rax = �@�@ �@�@  �@�@ �@�@  ���q2����2 ���q1����1
	psrlq		mm2, 16						;; mm2 = �@�@ �@�@  �@�@ �@�@  ���q3����3 ���q2����2
	and			rax, 0FFFFh					;; rax = �@�@ �@�@  �@�@ �@�@  �@�@ �@�@  ���q1����1
	movzx		rax, BYTE PTR [rbx + rax]
	pxor		mm1, mm1
	movd		mm1, rax					;; mm1 = look_up_table [���q1����1]
	psllq		mm1, 8
	por			mm4, mm1					;; mm4 = look_up_table [���q1����1 ���q0����0]

	movd		rax, mm2					;; rax = �@�@ �@�@  �@�@ �@�@  ���q3����3 ���q2����2
	psrlq		mm2, 16						;; mm2 = �@�@ �@�@  �@�@ �@�@  �@�@ �@�@  ���q3����3
	and			rax, 0FFFFh					;; rax = �@�@ �@�@  �@�@ �@�@  �@�@ �@�@  ���q2����2
	movzx		rax, BYTE PTR [rbx + rax]
	pxor		mm1, mm1
	movd		mm1, rax					;; mm1 = look_up_table [���q2����2]
	psllq		mm1, 16
	por			mm4, mm1					;; mm4 = look_up_table [���q2����2 ���q1����1 ���q0����0]

	movd		rax, mm2					;; rax = �@�@ �@�@  �@�@ �@�@  �@�@ �@�@  ���q3����3
	and			rax, 0FFFFh					;; rax = �@�@ �@�@  �@�@ �@�@  �@�@ �@�@  ���q3����3
	movzx		rax, BYTE PTR [rbx + rax]
	pxor		mm1, mm1
	movd		mm1, rax					;; mm1 = look_up_table [���q3����3]
	psllq		mm1, 24
	por			mm4, mm1					;; mm4 = look_up_table [���q3����3 ���q2����2 ���q1����1 ���q0����0]

	movd		rax, mm3					;; rax = �@�@ �@�@  �@�@ �@�@  ���q5����5 ���q4����4
	psrlq		mm3, 16						;; mm3 = �@�@ �@�@  ���q7����7 ���q6����6 ���q5����5
	and			rax, 0FFFFh					;; rax = �@�@ �@�@  �@�@ �@�@  �@�@ �@�@  ���q4����4
	movzx		rax, BYTE PTR [rbx + rax]
	pxor		mm1, mm1
	movd		mm1, rax					;; mm1 = look_up_table [���q4����4]
	psllq		mm1, 32
	por			mm4, mm1					;; mm4 = look_up_table [���q4����4 ���q3����3 ���q2����2 ���q1����1 ���q0����0]

	movd		rax, mm3					;; rax = �@�@ �@�@  �@�@ �@�@  ���q6����6 ���q5����5
	psrlq		mm3, 16						;; mm3 = �@�@ �@�@  �@�@ �@�@  ���q7����7 ���q6����6
	and			rax, 0FFFFh					;; rax = �@�@ �@�@  �@�@ �@�@  �@�@ �@�@  ���q5����5
	movzx		rax, BYTE PTR [rbx + rax]
	pxor		mm1, mm1
	movd		mm1, rax					;; mm1 = look_up_table [���q5����5]
	psllq		mm1, 40
	por			mm4, mm1					;; mm4 = look_up_table [���q5����5 ���q4����4 ���q3����3 ���q2����2 ���q1����1 ���q0����0]

	movd		rax, mm3					;; rax = �@�@ �@�@  �@�@ �@�@  ���q7����7 ���q6����6
	psrlq		mm3, 16						;; mm4 = �@�@ �@�@  �@�@ �@�@  �@�@ �@�@  ���q7����7
	and			rax, 0FFFFh					;; rax = �@�@ �@�@  �@�@ �@�@  �@�@ �@�@  ���q6����6
	movzx		rax, BYTE PTR [rbx + rax]
	pxor		mm1, mm1
	movd		mm1, rax					;; mm1 = look_up_table [���q6����6]
	psllq		mm1, 48
	por			mm4, mm1					;; mm4 = look_up_table [���q6����6 ���q5����5 ���q4����4 ���q3����3 ���q2����2 ���q1����1 ���q0����0]

	movd		rax, mm3					;; rax = �@�@ �@�@  �@�@ �@�@  �@�@ �@�@  ���q7����7
	and			rax, 0FFFFh					;; rax = �@�@ �@�@  �@�@ �@�@  �@�@ �@�@  ���q7����7
	movzx		rax, BYTE PTR [rbx + rax]
	pxor		mm1, mm1
	movd		mm1, rax					;; mm1 = look_up_table [���q7����7]
	psllq		mm1, 56
	por			mm4, mm1					;; mm4 = look_up_table [���q7����7 ���q6����6 ���q5����5 ���q4����4 ���q3����3 ���q2����2 ���q1����1 ���q0����0]

	movq		[rdi], mm4
	add			rdi, 8
	sub			esi, 8
	jae			L0

L1:
	add			esi, 8
	jz			L3
	dec			rdi

L2:
	movzx		rax, BYTE PTR [rcx]			;; rax = ���q
	inc			rcx
	movzx		r11, BYTE PTR [rdx]			;; r11 = ����
	shl			rax, 8
	inc			rdi
	or			rax, r11					;; rax = ���q����
	inc			rdx
	mov			al, BYTE PTR [rbx + rax]	;; al = look_up_table [���q����]
	dec			esi
	mov			[rdi], al
	jnz			L2

L3:
	;; ���W�X�^���A
	mov			rbx, r8
	mov			rsi, r9
	mov			rdi, r10

	emms

ret

lut2byte ENDP



lut2bytent PROC

;; �����̃X�^�b�N�߂�
mov		[rsp + 8], rcx				;; ubyte_t* quotient
mov		[rsp + 16], rdx				;; const ubyte_t* numerator
mov		[rsp + 24], r8				;; const ubyte_t* denominator
mov		[rsp + 32], r9				;; std::size_t size_x
									;; std::size_t size_y
									;; const ubyte_t* look_up_table

	;; ���W�X�^�Ҕ�
	mov			r8, rbx
	mov			r9, rsi
	mov			r10, rdi

	mov			rdi, [rsp + 8]
	mov			rcx, [rsp + 16]
	mov			rdx, [rsp + 24]
	mov			esi, [rsp + 32]
	imul		esi, [rsp + 40]
	mov			rbx, [rsp + 48]
	sub			esi, 8
	jb			L1

L0:	
	movq		mm1, [rcx]					;; ���q
	add			rcx, 8
	movq		mm2, [rdx]					;; ����
	add			rdx, 8
	movq		mm3, mm2
	punpcklbw	mm2, mm1					;; mm2 = ���q3����3 ���q2����2 ���q1����1 ���q0����0 (����4byte�̃A���p�b�N)
	punpckhbw	mm3, mm1					;; mm3 = ���q7����7 ���q6����6 ���q5����5 ���q4����4 (���4byte�̃A���p�b�N)

	movd		rax, mm2					;; rax = �@�@ �@�@  �@�@ �@�@  ���q1����1 ���q0����0
	psrlq		mm2, 16						;; mm2 = �@�@ �@�@  ���q3����3 ���q2����2 ���q1����1
	and			rax, 0FFFFh					;; rax = �@�@ �@�@  �@�@ �@�@  �@�@ �@�@  ���q0����0
	movzx		rax, BYTE PTR [rbx + rax]
	movd		mm4, rax					;; mm4 = look_up_table [���q0����0]

	movd		rax, mm2					;; rax = �@�@ �@�@  �@�@ �@�@  ���q2����2 ���q1����1
	psrlq		mm2, 16						;; mm2 = �@�@ �@�@  �@�@ �@�@  ���q3����3 ���q2����2
	and			rax, 0FFFFh					;; rax = �@�@ �@�@  �@�@ �@�@  �@�@ �@�@  ���q1����1
	movzx		rax, BYTE PTR [rbx + rax]
	movd		mm1, rax					;; mm1 = look_up_table [���q1����1]
	psllq		mm1, 8
	por			mm4, mm1					;; mm4 = look_up_table [���q1����1 ���q0����0]

	movd		rax, mm2					;; rax = �@�@ �@�@  �@�@ �@�@  ���q3����3 ���q2����2
	psrlq		mm2, 16						;; mm2 = �@�@ �@�@  �@�@ �@�@  �@�@ �@�@  ���q3����3
	and			rax, 0FFFFh					;; rax = �@�@ �@�@  �@�@ �@�@  �@�@ �@�@  ���q2����2
	movzx		rax, BYTE PTR [rbx + rax]
	movd		mm1, rax					;; mm1 = look_up_table [���q2����2]
	psllq		mm1, 16
	por			mm4, mm1					;; mm4 = look_up_table [���q2����2 ���q1����1 ���q0����0]

	movd		rax, mm2					;; rax = �@�@ �@�@  �@�@ �@�@  �@�@ �@�@  ���q3����3
	and			rax, 0FFFFh					;; rax = �@�@ �@�@  �@�@ �@�@  �@�@ �@�@  ���q3����3
	movzx		rax, BYTE PTR [rbx + rax]
	movd		mm1, rax					;; mm1 = look_up_table [���q3����3]
	psllq		mm1, 24
	por			mm4, mm1					;; mm4 = look_up_table [���q3����3 ���q2����2 ���q1����1 ���q0����0]

	movd		rax, mm3					;; rax = �@�@ �@�@  �@�@ �@�@  ���q5����5 ���q4����4
	psrlq		mm3, 16						;; mm3 = �@�@ �@�@  ���q7����7 ���q6����6 ���q5����5
	and			rax, 0FFFFh					;; rax = �@�@ �@�@  �@�@ �@�@  �@�@ �@�@  ���q4����4
	movzx		rax, BYTE PTR [rbx + rax]
	movd		mm1, eax					;; mm1 = look_up_table [���q4����4]
	psllq		mm1, 32
	por			mm4, mm1					;; mm4 = look_up_table [���q4����4 ���q3����3 ���q2����2 ���q1����1 ���q0����0]

	movd		rax, mm3					;; rax = �@�@ �@�@  �@�@ �@�@  ���q6����6 ���q5����5
	psrlq		mm3, 16						;; mm3 = �@�@ �@�@  �@�@ �@�@  ���q7����7 ���q6����6
	and			rax, 0FFFFh					;; rax = �@�@ �@�@  �@�@ �@�@  �@�@ �@�@  ���q5����5
	movzx		rax, BYTE PTR [rbx + rax]
	movd		mm1, rax					;; mm1 = look_up_table [���q5����5]
	psllq		mm1, 40
	por			mm4, mm1					;; mm4 = look_up_table [���q5����5 ���q4����4 ���q3����3 ���q2����2 ���q1����1 ���q0����0]

	movd		rax, mm3					;; rax = �@�@ �@�@  �@�@ �@�@  ���q7����7 ���q6����6
	psrlq		mm3, 16						;; mm4 = �@�@ �@�@  �@�@ �@�@  �@�@ �@�@  ���q7����7
	and			rax, 0FFFFh					;; rax = �@�@ �@�@  �@�@ �@�@  �@�@ �@�@  ���q6����6
	movzx		rax, BYTE PTR [rbx + rax]
	movd		mm1, rax					;; mm1 = look_up_table [���q6����6]
	psllq		mm1, 48
	por			mm4, mm1					;; mm4 = look_up_table [���q6����6 ���q5����5 ���q4����4 ���q3����3 ���q2����2 ���q1����1 ���q0����0]

	movd		rax, mm3					;; rax = �@�@ �@�@  �@�@ �@�@  �@�@ �@�@  ���q7����7
	and			rax, 0FFFFh					;; rax = �@�@ �@�@  �@�@ �@�@  �@�@ �@�@  ���q7����7
	movzx		rax, BYTE PTR [rbx + rax]
	movd		mm1, rax					;; mm1 = look_up_table [���q7����7]
	psllq		mm1, 56
	por			mm4, mm1					;; mm4 = look_up_table [���q7����7 ���q6����6 ���q5����5 ���q4����4 ���q3����3 ���q2����2 ���q1����1 ���q0����0]

	movntq		[rdi], mm4
	add			rdi, 8
	sub			esi, 8
	jae			L0

L1:
	add			esi, 8
	jz			L3
	dec			rdi

L2:
	movzx		rax, BYTE PTR [rcx]		;; rax = ���q
	inc			rcx
	movzx		r11, BYTE PTR [rdx]		;; r11 = ����
	shl			rax, 8
	inc			rdi
	or			rax, r11				;; rax = ���q����
	inc			rdx
	mov			al, BYTE PTR [rbx + rax]	;; al = look_up_table [���q����]
	dec			esi
	mov			[rdi], al
	jnz			L2

L3:
	;; ���W�X�^���A
	mov			rbx, r8
	mov			rsi, r9
	mov			rdi, r10

	emms

ret

lut2bytent ENDP



end
