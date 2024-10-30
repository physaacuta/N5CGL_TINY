.code

mem_copy_8 PROC

;;�����̃X�^�b�N�߂�
mov			[rsp + 8], rcx				;; const unsigned char* src
mov			[rsp + 16], rdx				;; unsigned long src_pitch
mov			[rsp + 24], r8				;; unsigned char* dst
mov			[rsp + 32], r9				;; unsigned long dst_pitch
										;; unsigned long width
										;; unsigned long height
										
	;; ���W�X�^�Ҕ�
	mov			r8, rbx
	mov			r9, rsi
	mov			r10, rdi

	;; ����
	mov			rsi, [rsp + 8]												;; esi, src��rsi, [rsp + 8]�ɕύX
	;;
	mov			ebx, [rsp + 16]												;; ebx, src_pitch��rbx, [rsp + 16]�ɕύX
	mov			eax, [rsp + 40]												;; width��[rsp + 40]�ɕύX
	sub			rbx, rax
	;;
	mov			rdi, [rsp + 24]												;; edi, dst��rdi, [rsp + 24]�ɕύX
	;;
	mov			edx, [rsp + 32]												;; edx, dst_pitch��rdx, [rsp + 32]�ɕύX
	sub			rdx, rax
	;;
	mov			ecx, [rsp + 48]												;; height��[rsp + 48]�ɕύX
	;;
	shr			eax, 3					;; eax: width / 8
	movd		mm7, eax

NEXT:
	movq		mm0, [rsi]													;; [esi]��[rsi]�ɕύX
	movq		[rdi], mm0													;; [edi]��[rdi]�ɕύX

	add			rsi, 8														;; esi��rsi�ɕύX
	add			rdi, 8														;; edi��rdi�ɕύX
	dec			eax
	jnz			NEXT

	add			rsi, rbx													;; esi,ebx��esi,rbx�ɕύX
	add			rdi, rdx													;; edi,edx��rdi,rdx�ɕύX
	movd		eax, mm7
	dec			ecx
	jnz			NEXT

	emms
	
	;; ���W�X�^���A
	mov			rbx, r8
	mov			rsi, r9
	mov			rdi, r10
        
ret

mem_copy_8 ENDP


mem_copy_32 PROC

;;�����̃X�^�b�N�߂�
mov			[rsp + 8], rcx				;; const unsigned char* src
mov			[rsp + 16], rdx				;; unsigned long src_pitch
mov			[rsp + 24], r8				;; unsigned char* dst
mov			[rsp + 32], r9				;; unsigned long dst_pitch
									    ;; unsigned long width
										;; unsigned long height

	;; ���W�X�^�Ҕ�
	mov			r8, rbx
	mov			r9, rsi
	mov			r10, rdi

	;; ����
	mov			rsi, [rsp + 8]												;; esi, src��rsi, [rsp + 8]�ɕύX
	;;
	mov			ebx, [rsp + 16]												;; ebx, src_pitch��rbx, [rsp + 16]�ɕύX
	mov			eax, [rsp + 40]												;; width��[rsp + 40]�ɕύX
	sub			rbx, rax
	;;
	mov			rdi, [rsp + 24]												;; edi, dst��rdi, [rsp + 24]�ɕύX
	;;
	mov			edx, [rsp + 32]												;; edx, dst_pitch��rdx, [rsp + 32]�ɕύX
	sub			rdx, rax
	;;
	mov			ecx, [rsp + 48]												;; height��[rsp + 48]�ɕύX
	;;
	shr			eax, 5					;; eax: width / 32
	movd		mm7, eax

NEXT:
	movq		mm0, [rsi]													;; [esi]��[rsi]�ɕύX
	movq		mm1, [rsi + 8]												;; [esi + 8]��[rsi + 8]�ɕύX
	movq		mm2, [rsi + 16]												;; [esi + 16]��[rsi + 16]�ɕύX
	movq		mm3, [rsi + 24]												;; [esi + 24]��[rsi + 24]�ɕύX
	movq		[rdi], mm0													;; [edi]��[rdi]�ɕύX
	movq		[rdi + 8], mm1												;; [edi + 8]��[rdi + 8]�ɕύX
	movq		[rdi + 16], mm2												;; [edi + 16]��[rdi + 16]�ɕύX
	movq		[rdi + 24], mm3												;; [edi + 24]��[rdi + 24]�ɕύX

	add			rsi, 32														;; esi��rsi�ɕύX	
	add			rdi, 32														;; edi��rdi�ɕύX	
	dec			eax
	jnz			NEXT

	add			rsi, rbx													;; esi, ebx��esi, rbx�ɕύX	
	add			rdi, rdx													;; edi, edx��rdi, rdx�ɕύX	
	movd		eax, mm7
	dec			ecx
	jnz			NEXT

	emms
	
	;; ���W�X�^���A
	mov			rbx, r8
	mov			rsi, r9
	mov			rdi, r10
	
ret

mem_copy_32 ENDP


mem_copy_64 PROC

;;�����̃X�^�b�N�߂�
mov			[rsp + 8], rcx				;; const unsigned char* src
mov			[rsp + 16], rdx				;; unsigned long src_pitch
mov			[rsp + 24], r8				;; unsigned char* dst
mov			[rsp + 32], r9				;; unsigned long dst_pitch
										;; unsigned long width
										;; unsigned long height
										
	;; ���W�X�^�Ҕ�
	mov			r8, rbx
	mov			r9, rsi
	mov			r10, rdi
   
	;; ����
	mov			rsi, [rsp + 8]												;; esi, src��rsi, [rsp + 8]�ɕύX
	;;
	mov			ebx, [rsp + 16]												;; ebx, src_pitch��rbx, [rsp + 16]�ɕύX
	mov			eax, [rsp + 40]												;; width��[rsp + 40]�ɕύX
	sub			rbx, rax
	;;
	mov			rdi, [rsp + 24]												;; edi, dst��rdi, [rsp + 24]�ɕύX
	;;
	mov			edx, [rsp + 32]												;; edx, dst_pitch��rdx, [rsp + 32]�ɕύX
	sub			rdx, rax
	;;
	mov			ecx, [rsp + 48]												;; height��[rsp + 48]�ɕύX
	;;
	shr			eax, 6					;; eax: width / 64
	mov			[rsp + 40], eax												;; width��[rsp + 40]�ɕύX

NEXT:
	movq		mm0, [rsi]													;; [esi]��[rsi]�ɕύX
	movq		mm1, [rsi + 8]												;; [esi + 8]��[rsi + 8]�ɕύX
	movq		mm2, [rsi + 16]												;; [esi + 16]��[rsi + 16]�ɕύX
	movq		mm3, [rsi + 24]												;; [esi + 24]��[rsi + 24]�ɕύX
	movq		mm4, [rsi + 32]												;; [esi + 32]��[rsi + 32]�ɕύX
	movq		mm5, [rsi + 40]												;; [esi + 40]��[rsi + 40]�ɕύX
	movq		mm6, [rsi + 48]												;; [esi + 48]��[rsi + 48]�ɕύX
	movq		mm7, [rsi + 56]												;; [esi + 56]��[rsi + 56]�ɕύX
	movq		[rdi], mm0													;; [edi]��[rdi]�ɕύX
	movq		[rdi + 8], mm1												;; [edi + 8]��[rdi + 8]�ɕύX
	movq		[rdi + 16], mm2												;; [edi + 16]��[rdi + 16]�ɕύX
	movq		[rdi + 24], mm3												;; [edi + 24]��[rdi + 24]�ɕύX
	movq		[rdi + 32], mm4												;; [edi + 32]��[rdi + 32]�ɕύX
	movq		[rdi + 40], mm5												;; [edi + 40]��[rdi + 40]�ɕύX
	movq		[rdi + 48], mm6												;; [edi + 48]��[rdi + 48]�ɕύX
	movq		[rdi + 56], mm7												;; [edi + 56]��[rdi + 56]�ɕύX

	add			rsi, 64														;; esi��rsi�ɕύX
	add			rdi, 64														;; edi��rdi�ɕύX
	dec			eax
	jnz			NEXT

	add			rsi, rbx													;; esi, ebx��rsi, rbx�ɕύX
	add			rdi, rdx													;; edi, edx��rdi, rdx�ɕύX
	mov			eax, [rsp + 40]												;; width��[rsp + 40]�ɕύX
	dec			ecx
	jnz			NEXT

	emms

	;; ���W�X�^���A
	mov			rbx, r8
	mov			rsi, r9
	mov			rdi, r10

ret

mem_copy_64 ENDP


mem_copy_16xmm PROC

;;�����̃X�^�b�N�߂�
mov			[rsp + 8], rcx				;; const unsigned char* src
mov			[rsp + 16], rdx				;; unsigned long src_pitch
mov			[rsp + 24], r8				;; unsigned char* dst
mov			[rsp + 32], r9				;; unsigned long dst_pitch
										;; unsigned long width,
										;; unsigned long height
										
	;; ���W�X�^�Ҕ�
	mov			r8, rbx
	mov			r9, rsi
	mov			r10, rdi

	;; ����
	mov			rsi, [rsp + 8]												;; esi, src��rsi, [rsp + 8]�ɕύX
	;;
	mov			ebx, [rsp + 16]												;; ebx, src_pitch��rbx, [rsp + 16]�ɕύX
	mov			eax, [rsp + 40]												;; width��[rsp + 40]�ɕύX
	sub			rbx, rax
	;;
	mov			rdi, [rsp + 24]												;; edi, dst��rdi, [rsp + 24]�ɕύX
	;;
	mov			edx, [rsp + 32]												;; edx, dst_pitch��rdx, [rsp + 32]�ɕύX
	sub			rdx, rax
	;;
	mov			ecx, [rsp + 48]												;; height��[rsp + 48]�ɕύX
	;;
	shr			eax, 4					;; eax: width / 16
	mov			[rsp + 40], eax												;; width��[rsp + 40]�ɕύX

NEXT:
	;; PentiumIII�ł��g����悤��movaps���g��
	movaps		xmm0, [rsi]													;; [esi]��[rsi]�ɕύX

	;; PentiumIII�ł��g����悤��movntps���g��
	movntps		[rdi], xmm0													;; [edi]��[rdi]�ɕύX

	add			rsi, 16														;; esi��rsi�ɕύX
	add			rdi, 16														;; edi��rdi�ɕύX
	dec			eax
	jnz			NEXT

	add			rsi, rbx													;; esi,ebx��rsi,rbx�ɕύX
	add			rdi, rdx													;; edi,edx��rdi,rdx�ɕύX
	mov			eax, [rsp + 40]												;; width��[rsp + 40]�ɕύX
	dec			ecx
	jnz			NEXT
	
	;; ���W�X�^���A
	mov			rbx, r8
	mov			rsi, r9
	mov			rdi, r10
	
ret

mem_copy_16xmm ENDP


mem_copy_x64_pitch PROC

;;�����̃X�^�b�N�߂�
mov			[rsp + 8], rcx				;; const unsigned char* src
mov			[rsp + 16], rdx				;; unsigned long src_pitch
mov			[rsp + 24], r8				;; unsigned char* dst
mov			[rsp + 32], r9				;; unsigned long dst_pitch
									    ;; unsigned long width
									    ;; unsigned long height
									    
	;; ���W�X�^�Ҕ�
	mov			r8, rbx
	mov			r9, rsi
	mov			r10, rdi

	;; ����
	mov			rsi, [rsp + 8]												;; esi, src��rsi, [rsp + 8]�ɕύX
	;;
	mov			ebx, [rsp + 16]												;; src_pitch��[rsp + 16]�ɕύX
	mov			eax, [rsp + 40]												;; width��[rsp + 40]�ɕύX
	sub			rbx, rax
	;;
	mov			rdi, [rsp + 24]												;; edi, dst��rdi, [rsp + 24]�ɕύX
	;;
	mov			edx, [rsp + 32]												;; edx, dst_pitch��rdx, [rsp + 32]�ɕύX
	sub			rdx, rax
	;;
	mov			ecx, [rsp + 48]												;; height��[rsp + 48]�ɕύX
	;;
	shr			eax, 6					;; eax: width / 64
	mov			[rsp + 40], eax												;; width��[rsp + 40]�ɕύX

NEXT:
	;; PentiumIII�ł��g����悤��movaps���g��
	movaps		xmm0, [rsi]													;; [esi]��[rsi]�ɕύX
	movaps		xmm1, [rsi + 16]											;; [esi + 16]��[rsi + 16]�ɕύX
	movaps		xmm2, [rsi + 32]											;; [esi + 32]��[rsi + 32]�ɕύX
	movaps		xmm3, [rsi + 48]											;; [esi + 48]��[rsi + 48]�ɕύX

	;; PentiumIII�ł��g����悤��movntps���g��
	movntps		[rdi],      xmm0											;; [edi]��[rdi]�ɕύX
	movntps		[rdi + 16], xmm1											;; [edi + 16]��[rdi + 16]�ɕύX
	movntps		[rdi + 32], xmm2											;; [edi + 32]��[rdi + 32]�ɕύX
	movntps		[rdi + 48], xmm3											;; [edi + 48]��[rdi + 48]�ɕύX

	add			rsi, 64														;; esi��rsi�ɕύX
	add			rdi, 64														;; edi��rdi�ɕύX
	dec			eax
	jnz			NEXT

	add			rsi, rbx													;; esi, ebx��rsi, rbx�ɕύX
	add			rdi, rdx													;; edi, edx��rdi, rdx�ɕύX
	mov			eax, [rsp + 40]												;; width��[rsp + 40]�ɕύX
	dec			ecx
	jnz			NEXT
    
   	;; ���W�X�^���A
	mov			rbx, r8
	mov			rsi, r9
	mov			rdi, r10
        
ret

mem_copy_x64_pitch ENDP


mem_clear_64xmm PROC

;;�����̃X�^�b�N�߂�
mov			[rsp + 8], rcx				;; void* dst
mov			[rsp + 16], rdx				;; unsigned long sz
mov			[rsp + 24], r8	
mov			[rsp + 32], r9

	;; ���W�X�^�Ҕ�
	mov			r8, rbx
	mov			r9, rsi
	mov			r10, rdi

	;; ����
	mov			rdi, [rsp + 8]												;; edi, dst��rdi, [rsp + 8]�ɕύX
	mov			ecx, [rsp + 16]												;; sz��[rsp + 16]�ɕύX
	shr			ecx, 6					;; eax: width / 64
	pxor		xmm7, xmm7

NEXT:
	movntdq		[rdi], xmm7													;; [edi]��[rdi]�ɕύX
	movntdq		[rdi+16], xmm7												;; [edi+16]��[rdi+16]�ɕύX
	movntdq		[rdi+32], xmm7												;; [edi+32]��[rdi+32]�ɕύX
	movntdq		[rdi+48], xmm7												;; [edi+48]��[rdi+48]�ɕύX

	add			rdi, 64														;; edi��rdi�ɕύX
	dec			ecx
	jnz			NEXT
	
	;; ���W�X�^���A
	mov			rbx, r8
	mov			rsi, r9
	mov			rdi, r10
	
ret

mem_clear_64xmm ENDP


mem_copy_x64_block PROC

;;�����̃X�^�b�N�߂�
mov			[rsp + 8], rcx				;; void *dst
mov			[rsp + 16], rdx				;; const void *src
mov			[rsp + 24], r8				;; size_t bksize
mov			[rsp + 32], r9

	;; ���W�X�^�Ҕ�
	mov			r8, rbx
	mov			r9, rsi
	mov			r10, rdi

	mov			rsi, [rsp + 16]												;; esi, src��rsi, [rsp + 16]�ɕύX
	mov			rdi, [rsp + 8]												;; edi, dst��rdi, [rsp + 8]�ɕύX
	mov			ecx, [rsp + 24]												;; bksize��[rsp + 24]�ɕύX
	dec			ecx
	sar			ecx, 6
	inc			ecx
L1:
	movdqa		xmm0, [rsi]				;; load 0-15						;; [esi]��[rsi]�ɕύX
	add			rdi, 64														;; edi��rdi�ɕύX
	movdqa		xmm1, [rsi+16]			;; load 16-31						;; [esi+16]��[rsi+16]�ɕύX
	add			rsi, 64														;; esi��rsi�ɕύX
	movdqa		xmm2, [rsi-32]			;; load 32-47						;; [esi-32]��[rsi-32]�ɕύX
	movntdq		[rdi-64], xmm0			;; store 0-15						;; [edi-64]��[rdi-64]�ɕύX
	movdqa		xmm3, [rsi-16]			;; load 48-63						;; [esi-16]��[rsi-16]�ɕύX
	movntdq		[rdi-48], xmm1			;; store 16-31						;; [edi-48]��[rdi-48]�ɕύX
	movntdq		[rdi-32], xmm2			;; store 32-47						;; [edi-32]��[rdi-32]�ɕύX
	dec			ecx
	movntdq		[rdi-16], xmm3			;; store 48-63						;; [edi-16]��[rdi-16]�ɕύX
	jnz			L1
	
	;; ���W�X�^���A
	mov			rbx, r8
	mov			rsi, r9
	mov			rdi, r10
	
ret

mem_copy_x64_block ENDP


mem_copy_nt PROC

;;�����̃X�^�b�N�߂�
mov			[rsp + 8], rcx				;; void *dst
mov			[rsp + 16], rdx				;; const void *src
mov			[rsp + 24], r8				;; size_t bksize
mov			[rsp + 32], r9

	;; ���W�X�^�Ҕ�
	mov			r8, rbx
	mov			r9, rsi
	mov			r10, rdi

	mov			rsi, [rsp + 16]												;; esi, src��rsi, [rsp + 16]�ɕύX		
	mov			rdi, [rsp + 8]												;; edi, dst��rdi, [rsp + 8]�ɕύX
	mov			rcx, [rsp + 24]												;; bksize��[rsp + 24]�ɕύX
	and			rcx, 31
	rep			movsb

	mov			rcx, [rsp + 24]												;; bksize��[rsp + 24]�ɕύX
	shr			rcx, 5
	test		rcx, rcx
	jz			LEND

L1:
	movq		mm0, [rsi]													;; [esi]��[rsi]�ɕύX		
	movq		mm1, [rsi+8]												;; [esi+8]��[rsi+8]�ɕύX
	movq		mm2, [rsi+16]												;; [esi+16]��[rsi+16]�ɕύX
	movq		mm3, [rsi+24]												;; [esi+24]��[rsi+24]�ɕύX
	add			rsi, 32														;; esi��rsi�ɕύX
	movntq		[rdi], mm0													;; [edi]��[rdi]�ɕύX
	movntq		[rdi+8], mm1												;; [edi+8]��[rdi+8]�ɕύX
	movntq		[rdi+16], mm2												;; [edi+16]��[rdi+16]�ɕύX
	movntq		[rdi+24], mm3												;; [edi+24]��[rdi+24]�ɕύX
	add			rdi, 32														;; edi��rdi�ɕύX

	dec			rcx
	jnz			L1
LEND:
	emms
	
	;; ���W�X�^���A
	mov			rbx, r8
	mov			rsi, r9
	mov			rdi, r10
			
ret

mem_copy_nt ENDP


mem_set_64xmm_short PROC
;	mov			[rsp + 8], rcx		; unsigned short* dst,
;	mov			[rsp +16], rdx		; unsigned short val,
;	mov			[rsp +24], r8		; unsigned long sz

	;	edi �ޔ�
	mov			r9, rdi;

    mov			rdi, rcx;
	mov			rcx, r8;
    shr			rcx, 5;         // eax: sz / 32
	movd		xmm0, rdx;
	punpcklwd	xmm0, xmm0;
	punpckldq	xmm0, xmm0;
	punpcklqdq	xmm0, xmm0;

NEXT:
	movntdq		[rdi], xmm0;
	movntdq		[rdi+16], xmm0;
	movntdq		[rdi+32], xmm0;
	movntdq		[rdi+48], xmm0;

    add			rdi, 64;
    dec			rcx;
    jnz			NEXT;
    
	mov			rdi, r9;

	ret

mem_set_64xmm_short ENDP

end
