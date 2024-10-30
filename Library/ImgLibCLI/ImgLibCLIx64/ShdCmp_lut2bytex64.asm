.code



lut2byte PROC

;; 引数のスタック戻し
mov		[rsp + 8], rcx				;; ubyte_t* quotient
mov		[rsp + 16], rdx				;; const ubyte_t* numerator
mov		[rsp + 24], r8				;; const ubyte_t* denominator
mov		[rsp + 32], r9				;; std::size_t size_x
									;; std::size_t size_y
									;; const ubyte_t* look_up_table

	;; レジスタ待避
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
	movq		mm1, [rcx]					;; 分子
	add			rcx, 8
	movq		mm2, [rdx]					;; 分母
	add			rdx, 8
	movq		mm3, mm2
	punpcklbw	mm2, mm1					;; mm2 = 分子3分母3 分子2分母2 分子1分母1 分子0分母0 (下位4byteのアンパック)
	punpckhbw	mm3, mm1					;; mm3 = 分子7分母7 分子6分母6 分子5分母5 分子4分母4 (上位4byteのアンパック)

	movd		rax, mm2					;; rax = 　　 　　  　　 　　  分子1分母1 分子0分母0
	psrlq		mm2, 16						;; mm2 = 　　 　　  分子3分母3 分子2分母2 分子1分母1
	and			rax, 0FFFFh					;; rax = 　　 　　  　　 　　  　　 　　  分子0分母0
	movzx		rax, BYTE PTR [rbx + rax]
	pxor		mm4, mm4
	movd		mm4, rax					;; mm4 = look_up_table [分子0分母0]

	movd		rax, mm2					;; rax = 　　 　　  　　 　　  分子2分母2 分子1分母1
	psrlq		mm2, 16						;; mm2 = 　　 　　  　　 　　  分子3分母3 分子2分母2
	and			rax, 0FFFFh					;; rax = 　　 　　  　　 　　  　　 　　  分子1分母1
	movzx		rax, BYTE PTR [rbx + rax]
	pxor		mm1, mm1
	movd		mm1, rax					;; mm1 = look_up_table [分子1分母1]
	psllq		mm1, 8
	por			mm4, mm1					;; mm4 = look_up_table [分子1分母1 分子0分母0]

	movd		rax, mm2					;; rax = 　　 　　  　　 　　  分子3分母3 分子2分母2
	psrlq		mm2, 16						;; mm2 = 　　 　　  　　 　　  　　 　　  分子3分母3
	and			rax, 0FFFFh					;; rax = 　　 　　  　　 　　  　　 　　  分子2分母2
	movzx		rax, BYTE PTR [rbx + rax]
	pxor		mm1, mm1
	movd		mm1, rax					;; mm1 = look_up_table [分子2分母2]
	psllq		mm1, 16
	por			mm4, mm1					;; mm4 = look_up_table [分子2分母2 分子1分母1 分子0分母0]

	movd		rax, mm2					;; rax = 　　 　　  　　 　　  　　 　　  分子3分母3
	and			rax, 0FFFFh					;; rax = 　　 　　  　　 　　  　　 　　  分子3分母3
	movzx		rax, BYTE PTR [rbx + rax]
	pxor		mm1, mm1
	movd		mm1, rax					;; mm1 = look_up_table [分子3分母3]
	psllq		mm1, 24
	por			mm4, mm1					;; mm4 = look_up_table [分子3分母3 分子2分母2 分子1分母1 分子0分母0]

	movd		rax, mm3					;; rax = 　　 　　  　　 　　  分子5分母5 分子4分母4
	psrlq		mm3, 16						;; mm3 = 　　 　　  分子7分母7 分子6分母6 分子5分母5
	and			rax, 0FFFFh					;; rax = 　　 　　  　　 　　  　　 　　  分子4分母4
	movzx		rax, BYTE PTR [rbx + rax]
	pxor		mm1, mm1
	movd		mm1, rax					;; mm1 = look_up_table [分子4分母4]
	psllq		mm1, 32
	por			mm4, mm1					;; mm4 = look_up_table [分子4分母4 分子3分母3 分子2分母2 分子1分母1 分子0分母0]

	movd		rax, mm3					;; rax = 　　 　　  　　 　　  分子6分母6 分子5分母5
	psrlq		mm3, 16						;; mm3 = 　　 　　  　　 　　  分子7分母7 分子6分母6
	and			rax, 0FFFFh					;; rax = 　　 　　  　　 　　  　　 　　  分子5分母5
	movzx		rax, BYTE PTR [rbx + rax]
	pxor		mm1, mm1
	movd		mm1, rax					;; mm1 = look_up_table [分子5分母5]
	psllq		mm1, 40
	por			mm4, mm1					;; mm4 = look_up_table [分子5分母5 分子4分母4 分子3分母3 分子2分母2 分子1分母1 分子0分母0]

	movd		rax, mm3					;; rax = 　　 　　  　　 　　  分子7分母7 分子6分母6
	psrlq		mm3, 16						;; mm4 = 　　 　　  　　 　　  　　 　　  分子7分母7
	and			rax, 0FFFFh					;; rax = 　　 　　  　　 　　  　　 　　  分子6分母6
	movzx		rax, BYTE PTR [rbx + rax]
	pxor		mm1, mm1
	movd		mm1, rax					;; mm1 = look_up_table [分子6分母6]
	psllq		mm1, 48
	por			mm4, mm1					;; mm4 = look_up_table [分子6分母6 分子5分母5 分子4分母4 分子3分母3 分子2分母2 分子1分母1 分子0分母0]

	movd		rax, mm3					;; rax = 　　 　　  　　 　　  　　 　　  分子7分母7
	and			rax, 0FFFFh					;; rax = 　　 　　  　　 　　  　　 　　  分子7分母7
	movzx		rax, BYTE PTR [rbx + rax]
	pxor		mm1, mm1
	movd		mm1, rax					;; mm1 = look_up_table [分子7分母7]
	psllq		mm1, 56
	por			mm4, mm1					;; mm4 = look_up_table [分子7分母7 分子6分母6 分子5分母5 分子4分母4 分子3分母3 分子2分母2 分子1分母1 分子0分母0]

	movq		[rdi], mm4
	add			rdi, 8
	sub			esi, 8
	jae			L0

L1:
	add			esi, 8
	jz			L3
	dec			rdi

L2:
	movzx		rax, BYTE PTR [rcx]			;; rax = 分子
	inc			rcx
	movzx		r11, BYTE PTR [rdx]			;; r11 = 分母
	shl			rax, 8
	inc			rdi
	or			rax, r11					;; rax = 分子分母
	inc			rdx
	mov			al, BYTE PTR [rbx + rax]	;; al = look_up_table [分子分母]
	dec			esi
	mov			[rdi], al
	jnz			L2

L3:
	;; レジスタ復帰
	mov			rbx, r8
	mov			rsi, r9
	mov			rdi, r10

	emms

ret

lut2byte ENDP



lut2bytent PROC

;; 引数のスタック戻し
mov		[rsp + 8], rcx				;; ubyte_t* quotient
mov		[rsp + 16], rdx				;; const ubyte_t* numerator
mov		[rsp + 24], r8				;; const ubyte_t* denominator
mov		[rsp + 32], r9				;; std::size_t size_x
									;; std::size_t size_y
									;; const ubyte_t* look_up_table

	;; レジスタ待避
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
	movq		mm1, [rcx]					;; 分子
	add			rcx, 8
	movq		mm2, [rdx]					;; 分母
	add			rdx, 8
	movq		mm3, mm2
	punpcklbw	mm2, mm1					;; mm2 = 分子3分母3 分子2分母2 分子1分母1 分子0分母0 (下位4byteのアンパック)
	punpckhbw	mm3, mm1					;; mm3 = 分子7分母7 分子6分母6 分子5分母5 分子4分母4 (上位4byteのアンパック)

	movd		rax, mm2					;; rax = 　　 　　  　　 　　  分子1分母1 分子0分母0
	psrlq		mm2, 16						;; mm2 = 　　 　　  分子3分母3 分子2分母2 分子1分母1
	and			rax, 0FFFFh					;; rax = 　　 　　  　　 　　  　　 　　  分子0分母0
	movzx		rax, BYTE PTR [rbx + rax]
	movd		mm4, rax					;; mm4 = look_up_table [分子0分母0]

	movd		rax, mm2					;; rax = 　　 　　  　　 　　  分子2分母2 分子1分母1
	psrlq		mm2, 16						;; mm2 = 　　 　　  　　 　　  分子3分母3 分子2分母2
	and			rax, 0FFFFh					;; rax = 　　 　　  　　 　　  　　 　　  分子1分母1
	movzx		rax, BYTE PTR [rbx + rax]
	movd		mm1, rax					;; mm1 = look_up_table [分子1分母1]
	psllq		mm1, 8
	por			mm4, mm1					;; mm4 = look_up_table [分子1分母1 分子0分母0]

	movd		rax, mm2					;; rax = 　　 　　  　　 　　  分子3分母3 分子2分母2
	psrlq		mm2, 16						;; mm2 = 　　 　　  　　 　　  　　 　　  分子3分母3
	and			rax, 0FFFFh					;; rax = 　　 　　  　　 　　  　　 　　  分子2分母2
	movzx		rax, BYTE PTR [rbx + rax]
	movd		mm1, rax					;; mm1 = look_up_table [分子2分母2]
	psllq		mm1, 16
	por			mm4, mm1					;; mm4 = look_up_table [分子2分母2 分子1分母1 分子0分母0]

	movd		rax, mm2					;; rax = 　　 　　  　　 　　  　　 　　  分子3分母3
	and			rax, 0FFFFh					;; rax = 　　 　　  　　 　　  　　 　　  分子3分母3
	movzx		rax, BYTE PTR [rbx + rax]
	movd		mm1, rax					;; mm1 = look_up_table [分子3分母3]
	psllq		mm1, 24
	por			mm4, mm1					;; mm4 = look_up_table [分子3分母3 分子2分母2 分子1分母1 分子0分母0]

	movd		rax, mm3					;; rax = 　　 　　  　　 　　  分子5分母5 分子4分母4
	psrlq		mm3, 16						;; mm3 = 　　 　　  分子7分母7 分子6分母6 分子5分母5
	and			rax, 0FFFFh					;; rax = 　　 　　  　　 　　  　　 　　  分子4分母4
	movzx		rax, BYTE PTR [rbx + rax]
	movd		mm1, eax					;; mm1 = look_up_table [分子4分母4]
	psllq		mm1, 32
	por			mm4, mm1					;; mm4 = look_up_table [分子4分母4 分子3分母3 分子2分母2 分子1分母1 分子0分母0]

	movd		rax, mm3					;; rax = 　　 　　  　　 　　  分子6分母6 分子5分母5
	psrlq		mm3, 16						;; mm3 = 　　 　　  　　 　　  分子7分母7 分子6分母6
	and			rax, 0FFFFh					;; rax = 　　 　　  　　 　　  　　 　　  分子5分母5
	movzx		rax, BYTE PTR [rbx + rax]
	movd		mm1, rax					;; mm1 = look_up_table [分子5分母5]
	psllq		mm1, 40
	por			mm4, mm1					;; mm4 = look_up_table [分子5分母5 分子4分母4 分子3分母3 分子2分母2 分子1分母1 分子0分母0]

	movd		rax, mm3					;; rax = 　　 　　  　　 　　  分子7分母7 分子6分母6
	psrlq		mm3, 16						;; mm4 = 　　 　　  　　 　　  　　 　　  分子7分母7
	and			rax, 0FFFFh					;; rax = 　　 　　  　　 　　  　　 　　  分子6分母6
	movzx		rax, BYTE PTR [rbx + rax]
	movd		mm1, rax					;; mm1 = look_up_table [分子6分母6]
	psllq		mm1, 48
	por			mm4, mm1					;; mm4 = look_up_table [分子6分母6 分子5分母5 分子4分母4 分子3分母3 分子2分母2 分子1分母1 分子0分母0]

	movd		rax, mm3					;; rax = 　　 　　  　　 　　  　　 　　  分子7分母7
	and			rax, 0FFFFh					;; rax = 　　 　　  　　 　　  　　 　　  分子7分母7
	movzx		rax, BYTE PTR [rbx + rax]
	movd		mm1, rax					;; mm1 = look_up_table [分子7分母7]
	psllq		mm1, 56
	por			mm4, mm1					;; mm4 = look_up_table [分子7分母7 分子6分母6 分子5分母5 分子4分母4 分子3分母3 分子2分母2 分子1分母1 分子0分母0]

	movntq		[rdi], mm4
	add			rdi, 8
	sub			esi, 8
	jae			L0

L1:
	add			esi, 8
	jz			L3
	dec			rdi

L2:
	movzx		rax, BYTE PTR [rcx]		;; rax = 分子
	inc			rcx
	movzx		r11, BYTE PTR [rdx]		;; r11 = 分母
	shl			rax, 8
	inc			rdi
	or			rax, r11				;; rax = 分子分母
	inc			rdx
	mov			al, BYTE PTR [rbx + rax]	;; al = look_up_table [分子分母]
	dec			esi
	mov			[rdi], al
	jnz			L2

L3:
	;; レジスタ復帰
	mov			rbx, r8
	mov			rsi, r9
	mov			rdi, r10

	emms

ret

lut2bytent ENDP



end
