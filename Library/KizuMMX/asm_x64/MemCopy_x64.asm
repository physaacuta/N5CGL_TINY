.code

mem_copy_8 PROC

;;引数のスタック戻し
mov			[rsp + 8], rcx				;; const unsigned char* src
mov			[rsp + 16], rdx				;; unsigned long src_pitch
mov			[rsp + 24], r8				;; unsigned char* dst
mov			[rsp + 32], r9				;; unsigned long dst_pitch
										;; unsigned long width
										;; unsigned long height
										
	;; レジスタ待避
	mov			r8, rbx
	mov			r9, rsi
	mov			r10, rdi

	;; 引数
	mov			rsi, [rsp + 8]												;; esi, srcをrsi, [rsp + 8]に変更
	;;
	mov			ebx, [rsp + 16]												;; ebx, src_pitchをrbx, [rsp + 16]に変更
	mov			eax, [rsp + 40]												;; widthを[rsp + 40]に変更
	sub			rbx, rax
	;;
	mov			rdi, [rsp + 24]												;; edi, dstをrdi, [rsp + 24]に変更
	;;
	mov			edx, [rsp + 32]												;; edx, dst_pitchをrdx, [rsp + 32]に変更
	sub			rdx, rax
	;;
	mov			ecx, [rsp + 48]												;; heightを[rsp + 48]に変更
	;;
	shr			eax, 3					;; eax: width / 8
	movd		mm7, eax

NEXT:
	movq		mm0, [rsi]													;; [esi]を[rsi]に変更
	movq		[rdi], mm0													;; [edi]を[rdi]に変更

	add			rsi, 8														;; esiをrsiに変更
	add			rdi, 8														;; ediをrdiに変更
	dec			eax
	jnz			NEXT

	add			rsi, rbx													;; esi,ebxをesi,rbxに変更
	add			rdi, rdx													;; edi,edxをrdi,rdxに変更
	movd		eax, mm7
	dec			ecx
	jnz			NEXT

	emms
	
	;; レジスタ復帰
	mov			rbx, r8
	mov			rsi, r9
	mov			rdi, r10
        
ret

mem_copy_8 ENDP


mem_copy_32 PROC

;;引数のスタック戻し
mov			[rsp + 8], rcx				;; const unsigned char* src
mov			[rsp + 16], rdx				;; unsigned long src_pitch
mov			[rsp + 24], r8				;; unsigned char* dst
mov			[rsp + 32], r9				;; unsigned long dst_pitch
									    ;; unsigned long width
										;; unsigned long height

	;; レジスタ待避
	mov			r8, rbx
	mov			r9, rsi
	mov			r10, rdi

	;; 引数
	mov			rsi, [rsp + 8]												;; esi, srcをrsi, [rsp + 8]に変更
	;;
	mov			ebx, [rsp + 16]												;; ebx, src_pitchをrbx, [rsp + 16]に変更
	mov			eax, [rsp + 40]												;; widthを[rsp + 40]に変更
	sub			rbx, rax
	;;
	mov			rdi, [rsp + 24]												;; edi, dstをrdi, [rsp + 24]に変更
	;;
	mov			edx, [rsp + 32]												;; edx, dst_pitchをrdx, [rsp + 32]に変更
	sub			rdx, rax
	;;
	mov			ecx, [rsp + 48]												;; heightを[rsp + 48]に変更
	;;
	shr			eax, 5					;; eax: width / 32
	movd		mm7, eax

NEXT:
	movq		mm0, [rsi]													;; [esi]を[rsi]に変更
	movq		mm1, [rsi + 8]												;; [esi + 8]を[rsi + 8]に変更
	movq		mm2, [rsi + 16]												;; [esi + 16]を[rsi + 16]に変更
	movq		mm3, [rsi + 24]												;; [esi + 24]を[rsi + 24]に変更
	movq		[rdi], mm0													;; [edi]を[rdi]に変更
	movq		[rdi + 8], mm1												;; [edi + 8]を[rdi + 8]に変更
	movq		[rdi + 16], mm2												;; [edi + 16]を[rdi + 16]に変更
	movq		[rdi + 24], mm3												;; [edi + 24]を[rdi + 24]に変更

	add			rsi, 32														;; esiをrsiに変更	
	add			rdi, 32														;; ediをrdiに変更	
	dec			eax
	jnz			NEXT

	add			rsi, rbx													;; esi, ebxをesi, rbxに変更	
	add			rdi, rdx													;; edi, edxをrdi, rdxに変更	
	movd		eax, mm7
	dec			ecx
	jnz			NEXT

	emms
	
	;; レジスタ復帰
	mov			rbx, r8
	mov			rsi, r9
	mov			rdi, r10
	
ret

mem_copy_32 ENDP


mem_copy_64 PROC

;;引数のスタック戻し
mov			[rsp + 8], rcx				;; const unsigned char* src
mov			[rsp + 16], rdx				;; unsigned long src_pitch
mov			[rsp + 24], r8				;; unsigned char* dst
mov			[rsp + 32], r9				;; unsigned long dst_pitch
										;; unsigned long width
										;; unsigned long height
										
	;; レジスタ待避
	mov			r8, rbx
	mov			r9, rsi
	mov			r10, rdi
   
	;; 引数
	mov			rsi, [rsp + 8]												;; esi, srcをrsi, [rsp + 8]に変更
	;;
	mov			ebx, [rsp + 16]												;; ebx, src_pitchをrbx, [rsp + 16]に変更
	mov			eax, [rsp + 40]												;; widthを[rsp + 40]に変更
	sub			rbx, rax
	;;
	mov			rdi, [rsp + 24]												;; edi, dstをrdi, [rsp + 24]に変更
	;;
	mov			edx, [rsp + 32]												;; edx, dst_pitchをrdx, [rsp + 32]に変更
	sub			rdx, rax
	;;
	mov			ecx, [rsp + 48]												;; heightを[rsp + 48]に変更
	;;
	shr			eax, 6					;; eax: width / 64
	mov			[rsp + 40], eax												;; widthを[rsp + 40]に変更

NEXT:
	movq		mm0, [rsi]													;; [esi]を[rsi]に変更
	movq		mm1, [rsi + 8]												;; [esi + 8]を[rsi + 8]に変更
	movq		mm2, [rsi + 16]												;; [esi + 16]を[rsi + 16]に変更
	movq		mm3, [rsi + 24]												;; [esi + 24]を[rsi + 24]に変更
	movq		mm4, [rsi + 32]												;; [esi + 32]を[rsi + 32]に変更
	movq		mm5, [rsi + 40]												;; [esi + 40]を[rsi + 40]に変更
	movq		mm6, [rsi + 48]												;; [esi + 48]を[rsi + 48]に変更
	movq		mm7, [rsi + 56]												;; [esi + 56]を[rsi + 56]に変更
	movq		[rdi], mm0													;; [edi]を[rdi]に変更
	movq		[rdi + 8], mm1												;; [edi + 8]を[rdi + 8]に変更
	movq		[rdi + 16], mm2												;; [edi + 16]を[rdi + 16]に変更
	movq		[rdi + 24], mm3												;; [edi + 24]を[rdi + 24]に変更
	movq		[rdi + 32], mm4												;; [edi + 32]を[rdi + 32]に変更
	movq		[rdi + 40], mm5												;; [edi + 40]を[rdi + 40]に変更
	movq		[rdi + 48], mm6												;; [edi + 48]を[rdi + 48]に変更
	movq		[rdi + 56], mm7												;; [edi + 56]を[rdi + 56]に変更

	add			rsi, 64														;; esiをrsiに変更
	add			rdi, 64														;; ediをrdiに変更
	dec			eax
	jnz			NEXT

	add			rsi, rbx													;; esi, ebxをrsi, rbxに変更
	add			rdi, rdx													;; edi, edxをrdi, rdxに変更
	mov			eax, [rsp + 40]												;; widthを[rsp + 40]に変更
	dec			ecx
	jnz			NEXT

	emms

	;; レジスタ復帰
	mov			rbx, r8
	mov			rsi, r9
	mov			rdi, r10

ret

mem_copy_64 ENDP


mem_copy_16xmm PROC

;;引数のスタック戻し
mov			[rsp + 8], rcx				;; const unsigned char* src
mov			[rsp + 16], rdx				;; unsigned long src_pitch
mov			[rsp + 24], r8				;; unsigned char* dst
mov			[rsp + 32], r9				;; unsigned long dst_pitch
										;; unsigned long width,
										;; unsigned long height
										
	;; レジスタ待避
	mov			r8, rbx
	mov			r9, rsi
	mov			r10, rdi

	;; 引数
	mov			rsi, [rsp + 8]												;; esi, srcをrsi, [rsp + 8]に変更
	;;
	mov			ebx, [rsp + 16]												;; ebx, src_pitchをrbx, [rsp + 16]に変更
	mov			eax, [rsp + 40]												;; widthを[rsp + 40]に変更
	sub			rbx, rax
	;;
	mov			rdi, [rsp + 24]												;; edi, dstをrdi, [rsp + 24]に変更
	;;
	mov			edx, [rsp + 32]												;; edx, dst_pitchをrdx, [rsp + 32]に変更
	sub			rdx, rax
	;;
	mov			ecx, [rsp + 48]												;; heightを[rsp + 48]に変更
	;;
	shr			eax, 4					;; eax: width / 16
	mov			[rsp + 40], eax												;; widthを[rsp + 40]に変更

NEXT:
	;; PentiumIIIでも使えるようにmovapsを使う
	movaps		xmm0, [rsi]													;; [esi]を[rsi]に変更

	;; PentiumIIIでも使えるようにmovntpsを使う
	movntps		[rdi], xmm0													;; [edi]を[rdi]に変更

	add			rsi, 16														;; esiをrsiに変更
	add			rdi, 16														;; ediをrdiに変更
	dec			eax
	jnz			NEXT

	add			rsi, rbx													;; esi,ebxをrsi,rbxに変更
	add			rdi, rdx													;; edi,edxをrdi,rdxに変更
	mov			eax, [rsp + 40]												;; widthを[rsp + 40]に変更
	dec			ecx
	jnz			NEXT
	
	;; レジスタ復帰
	mov			rbx, r8
	mov			rsi, r9
	mov			rdi, r10
	
ret

mem_copy_16xmm ENDP


mem_copy_x64_pitch PROC

;;引数のスタック戻し
mov			[rsp + 8], rcx				;; const unsigned char* src
mov			[rsp + 16], rdx				;; unsigned long src_pitch
mov			[rsp + 24], r8				;; unsigned char* dst
mov			[rsp + 32], r9				;; unsigned long dst_pitch
									    ;; unsigned long width
									    ;; unsigned long height
									    
	;; レジスタ待避
	mov			r8, rbx
	mov			r9, rsi
	mov			r10, rdi

	;; 引数
	mov			rsi, [rsp + 8]												;; esi, srcをrsi, [rsp + 8]に変更
	;;
	mov			ebx, [rsp + 16]												;; src_pitchを[rsp + 16]に変更
	mov			eax, [rsp + 40]												;; widthを[rsp + 40]に変更
	sub			rbx, rax
	;;
	mov			rdi, [rsp + 24]												;; edi, dstをrdi, [rsp + 24]に変更
	;;
	mov			edx, [rsp + 32]												;; edx, dst_pitchをrdx, [rsp + 32]に変更
	sub			rdx, rax
	;;
	mov			ecx, [rsp + 48]												;; heightを[rsp + 48]に変更
	;;
	shr			eax, 6					;; eax: width / 64
	mov			[rsp + 40], eax												;; widthを[rsp + 40]に変更

NEXT:
	;; PentiumIIIでも使えるようにmovapsを使う
	movaps		xmm0, [rsi]													;; [esi]を[rsi]に変更
	movaps		xmm1, [rsi + 16]											;; [esi + 16]を[rsi + 16]に変更
	movaps		xmm2, [rsi + 32]											;; [esi + 32]を[rsi + 32]に変更
	movaps		xmm3, [rsi + 48]											;; [esi + 48]を[rsi + 48]に変更

	;; PentiumIIIでも使えるようにmovntpsを使う
	movntps		[rdi],      xmm0											;; [edi]を[rdi]に変更
	movntps		[rdi + 16], xmm1											;; [edi + 16]を[rdi + 16]に変更
	movntps		[rdi + 32], xmm2											;; [edi + 32]を[rdi + 32]に変更
	movntps		[rdi + 48], xmm3											;; [edi + 48]を[rdi + 48]に変更

	add			rsi, 64														;; esiをrsiに変更
	add			rdi, 64														;; ediをrdiに変更
	dec			eax
	jnz			NEXT

	add			rsi, rbx													;; esi, ebxをrsi, rbxに変更
	add			rdi, rdx													;; edi, edxをrdi, rdxに変更
	mov			eax, [rsp + 40]												;; widthを[rsp + 40]に変更
	dec			ecx
	jnz			NEXT
    
   	;; レジスタ復帰
	mov			rbx, r8
	mov			rsi, r9
	mov			rdi, r10
        
ret

mem_copy_x64_pitch ENDP


mem_clear_64xmm PROC

;;引数のスタック戻し
mov			[rsp + 8], rcx				;; void* dst
mov			[rsp + 16], rdx				;; unsigned long sz
mov			[rsp + 24], r8	
mov			[rsp + 32], r9

	;; レジスタ待避
	mov			r8, rbx
	mov			r9, rsi
	mov			r10, rdi

	;; 引数
	mov			rdi, [rsp + 8]												;; edi, dstをrdi, [rsp + 8]に変更
	mov			ecx, [rsp + 16]												;; szを[rsp + 16]に変更
	shr			ecx, 6					;; eax: width / 64
	pxor		xmm7, xmm7

NEXT:
	movntdq		[rdi], xmm7													;; [edi]を[rdi]に変更
	movntdq		[rdi+16], xmm7												;; [edi+16]を[rdi+16]に変更
	movntdq		[rdi+32], xmm7												;; [edi+32]を[rdi+32]に変更
	movntdq		[rdi+48], xmm7												;; [edi+48]を[rdi+48]に変更

	add			rdi, 64														;; ediをrdiに変更
	dec			ecx
	jnz			NEXT
	
	;; レジスタ復帰
	mov			rbx, r8
	mov			rsi, r9
	mov			rdi, r10
	
ret

mem_clear_64xmm ENDP


mem_copy_x64_block PROC

;;引数のスタック戻し
mov			[rsp + 8], rcx				;; void *dst
mov			[rsp + 16], rdx				;; const void *src
mov			[rsp + 24], r8				;; size_t bksize
mov			[rsp + 32], r9

	;; レジスタ待避
	mov			r8, rbx
	mov			r9, rsi
	mov			r10, rdi

	mov			rsi, [rsp + 16]												;; esi, srcをrsi, [rsp + 16]に変更
	mov			rdi, [rsp + 8]												;; edi, dstをrdi, [rsp + 8]に変更
	mov			ecx, [rsp + 24]												;; bksizeを[rsp + 24]に変更
	dec			ecx
	sar			ecx, 6
	inc			ecx
L1:
	movdqa		xmm0, [rsi]				;; load 0-15						;; [esi]を[rsi]に変更
	add			rdi, 64														;; ediをrdiに変更
	movdqa		xmm1, [rsi+16]			;; load 16-31						;; [esi+16]を[rsi+16]に変更
	add			rsi, 64														;; esiをrsiに変更
	movdqa		xmm2, [rsi-32]			;; load 32-47						;; [esi-32]を[rsi-32]に変更
	movntdq		[rdi-64], xmm0			;; store 0-15						;; [edi-64]を[rdi-64]に変更
	movdqa		xmm3, [rsi-16]			;; load 48-63						;; [esi-16]を[rsi-16]に変更
	movntdq		[rdi-48], xmm1			;; store 16-31						;; [edi-48]を[rdi-48]に変更
	movntdq		[rdi-32], xmm2			;; store 32-47						;; [edi-32]を[rdi-32]に変更
	dec			ecx
	movntdq		[rdi-16], xmm3			;; store 48-63						;; [edi-16]を[rdi-16]に変更
	jnz			L1
	
	;; レジスタ復帰
	mov			rbx, r8
	mov			rsi, r9
	mov			rdi, r10
	
ret

mem_copy_x64_block ENDP


mem_copy_nt PROC

;;引数のスタック戻し
mov			[rsp + 8], rcx				;; void *dst
mov			[rsp + 16], rdx				;; const void *src
mov			[rsp + 24], r8				;; size_t bksize
mov			[rsp + 32], r9

	;; レジスタ待避
	mov			r8, rbx
	mov			r9, rsi
	mov			r10, rdi

	mov			rsi, [rsp + 16]												;; esi, srcをrsi, [rsp + 16]に変更		
	mov			rdi, [rsp + 8]												;; edi, dstをrdi, [rsp + 8]に変更
	mov			rcx, [rsp + 24]												;; bksizeを[rsp + 24]に変更
	and			rcx, 31
	rep			movsb

	mov			rcx, [rsp + 24]												;; bksizeを[rsp + 24]に変更
	shr			rcx, 5
	test		rcx, rcx
	jz			LEND

L1:
	movq		mm0, [rsi]													;; [esi]を[rsi]に変更		
	movq		mm1, [rsi+8]												;; [esi+8]を[rsi+8]に変更
	movq		mm2, [rsi+16]												;; [esi+16]を[rsi+16]に変更
	movq		mm3, [rsi+24]												;; [esi+24]を[rsi+24]に変更
	add			rsi, 32														;; esiをrsiに変更
	movntq		[rdi], mm0													;; [edi]を[rdi]に変更
	movntq		[rdi+8], mm1												;; [edi+8]を[rdi+8]に変更
	movntq		[rdi+16], mm2												;; [edi+16]を[rdi+16]に変更
	movntq		[rdi+24], mm3												;; [edi+24]を[rdi+24]に変更
	add			rdi, 32														;; ediをrdiに変更

	dec			rcx
	jnz			L1
LEND:
	emms
	
	;; レジスタ復帰
	mov			rbx, r8
	mov			rsi, r9
	mov			rdi, r10
			
ret

mem_copy_nt ENDP


mem_set_64xmm_short PROC
;	mov			[rsp + 8], rcx		; unsigned short* dst,
;	mov			[rsp +16], rdx		; unsigned short val,
;	mov			[rsp +24], r8		; unsigned long sz

	;	edi 退避
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
