.code

memcpy_2d_hflip_sse PROC
        mov     [rsp + 8], rcx  ; dst
        mov     [rsp + 16], rdx ; dst_pitch
        mov     [rsp + 24], r8  ; src
        mov     [rsp + 32], r9  ; width
        mov     r10, [rsp + 40] ; height

        mov     rax, 0FFFFFFFFh
        and     rdx, rax
        and     r9, rax
        and     r10, rax

        mov     rax, r9
        shr     rax, 4          ; rax: backup of width

        add     rcx, r9         ; move dst to one byte behind of last byte in first line

L1:
        mov     r9, rax         ; restore x-counter
        movd    xmm5, rcx       ; backup of dst

L0:
        sub     rcx, 16
        movdqu  xmm0, [r8]
        add     r8, 16

        movd    r11, xmm0
        bswap   r11
        movd    xmm1, r11

        pslldq  xmm1, 8
        psrldq  xmm0, 8
        movd    r11, xmm0
        bswap   r11
        movd    xmm2, r11
        por     xmm1, xmm2

        movdqa  [rcx], xmm1

        dec     r9
        jnz     L0

        movd    rcx, xmm5       ; restore dst
        add     rcx, rdx        ; move dst to next line
        dec     r10
        jnz     L1

        ret
memcpy_2d_hflip_sse ENDP

end