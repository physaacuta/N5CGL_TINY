//
// エリア総和テーブルを作る関数
//

#include <crtdbg.h>
#include "MakeSatasm.h"

#if !defined(_M_AMD64) || defined(__ICL)
// X86 or X64(icl)

void __stdcall SwdetectMMX::VertAdd(
		const unsigned char* data,
		int w,
		unsigned long * acc)
{
#ifndef _M_AMD64
    __asm{
		// 引数
        mov			esi, data;
		mov			edi, acc;
		pxor		xmm7, xmm7;		// xmm7: zero
		mov			ebx, w;
		shr			ebx, 5;			// w/32

		// loop
L1:
		// 0-15
		movdqa		xmm0, [esi];		// 0-15
		movdqa		xmm2, xmm0;		// コピー
		punpcklbw	xmm0, xmm7;		// byte -> word
		punpckhbw	xmm2, xmm7;		// byte -> word
		movdqa		xmm1, xmm0;
		movdqa		xmm3, xmm2;
		punpcklwd	xmm0, xmm7;		// word -> dword
		punpckhwd	xmm1, xmm7;
		punpcklwd	xmm2, xmm7;
		punpckhwd	xmm3, xmm7;

		paddd		xmm0, [edi];		// 0-3 
		movdqa		[edi], xmm0;
		paddd		xmm1, [edi+16];		// 4-7
		movdqa		[edi+16], xmm1;
		paddd		xmm2, [edi+32];		// 8-11
		movdqa		[edi+32], xmm2;
		paddd		xmm3, [edi+48];		// 12-15
		movdqa		[edi+48], xmm3;

		// 16-31
		movdqa		xmm0, [esi+16];		// 16-31
		movdqa		xmm2, xmm0;			// コピー
		punpcklbw	xmm0, xmm7;		// byte -> word
		punpckhbw	xmm2, xmm7;		// byte -> word
		movdqa		xmm1, xmm0;
		movdqa		xmm3, xmm2;
		punpcklwd	xmm0, xmm7;		// word -> dword
		punpckhwd	xmm1, xmm7;
		punpcklwd	xmm2, xmm7;
		punpckhwd	xmm3, xmm7;

		paddd		xmm0, [edi+64];		// 16-19 
		movdqa		[edi+64], xmm0;
		paddd		xmm1, [edi+80];		// 20-23
		movdqa		[edi+80], xmm1;
		paddd		xmm2, [edi+96];		// 24-27
		movdqa		[edi+96], xmm2;
		paddd		xmm3, [edi+112];	// 28-31
		movdqa		[edi+112], xmm3;

		add			esi, 32;			// 次の32個
		add			edi, 128;

		dec			ebx;
		jnz			L1;
    }
#else
    __asm{
                // 引数
        mov             rsi, data;
        mov             rdi, acc;
        pxor            xmm7, xmm7;             // xmm7: zero
        mov             ebx, w;
        shr             ebx, 5;                 // w/32

        // loop
    L1:
        // 0-15
        movdqa          xmm0, [rsi];            // 0-15
        movdqa          xmm2, xmm0;             // コピー
        punpcklbw       xmm0, xmm7;             // byte -> word
        punpckhbw       xmm2, xmm7;             // byte -> word
        movdqa          xmm1, xmm0;
        movdqa          xmm3, xmm2;
        punpcklwd       xmm0, xmm7;             // word -> dword
        punpckhwd       xmm1, xmm7;
        punpcklwd       xmm2, xmm7;
        punpckhwd       xmm3, xmm7;

        paddd           xmm0, [rdi];            // 0-3 
        movdqa          [rdi], xmm0;
        paddd           xmm1, [rdi+16];         // 4-7
        movdqa          [rdi+16], xmm1;
        paddd           xmm2, [rdi+32];         // 8-11
        movdqa          [rdi+32], xmm2;
        paddd           xmm3, [rdi+48];         // 12-15
        movdqa          [rdi+48], xmm3;

        // 16-31
        movdqa          xmm0, [rsi+16];         // 16-31
        movdqa          xmm2, xmm0;                     // コピー
        punpcklbw       xmm0, xmm7;             // byte -> word
        punpckhbw       xmm2, xmm7;             // byte -> word
        movdqa          xmm1, xmm0;
        movdqa          xmm3, xmm2;
        punpcklwd       xmm0, xmm7;             // word -> dword
        punpckhwd       xmm1, xmm7;
        punpcklwd       xmm2, xmm7;
        punpckhwd       xmm3, xmm7;

        paddd           xmm0, [rdi+64];         // 16-19 
        movdqa          [rdi+64], xmm0;
        paddd           xmm1, [rdi+80];         // 20-23
        movdqa          [rdi+80], xmm1;
        paddd           xmm2, [rdi+96];         // 24-27
        movdqa          [rdi+96], xmm2;
        paddd           xmm3, [rdi+112];        // 28-31
        movdqa          [rdi+112], xmm3;

        add             rsi, 32;                        // 次の32個
        add             rdi, 128;

        dec             ebx;
        jnz             L1;
    }
#endif
}

void __stdcall SwdetectMMX::VertAddLimit(
		const unsigned char* data,
		int w,
		unsigned long * acc,
		unsigned char lower,
		unsigned char upper)
{
#ifndef _M_AMD64
    __asm{
		// 引数
        mov			esi, data;
		mov			edi, acc;
		pxor		xmm7, xmm7;		// xmm7: zero
		mov			ebx, w;
		shr			ebx, 5;			// w/32
		movzx		eax, BYTE PTR lower;
		movd		xmm4, eax;
		punpcklbw	xmm4, xmm4;
		punpcklwd	xmm4, xmm4;
		punpckldq	xmm4, xmm4;
		punpcklqdq	xmm4, xmm4;
		movzx		eax, BYTE PTR upper;
		movd		xmm5, eax;
		punpcklbw	xmm5, xmm5;
		punpcklwd	xmm5, xmm5;
		punpckldq	xmm5, xmm5;
		punpcklqdq	xmm5, xmm5;

		// loop
L1:
		// 0-15
		movdqa		xmm0, [esi];		// 0-15
		pmaxub		xmm0, xmm4;
		pminub		xmm0, xmm5;
		movdqa		xmm2, xmm0;		// コピー
		punpcklbw	xmm0, xmm7;		// byte -> word
		punpckhbw	xmm2, xmm7;		// byte -> word
		movdqa		xmm1, xmm0;
		movdqa		xmm3, xmm2;
		punpcklwd	xmm0, xmm7;		// word -> dword
		punpckhwd	xmm1, xmm7;
		punpcklwd	xmm2, xmm7;
		punpckhwd	xmm3, xmm7;

		paddd		xmm0, [edi];		// 0-3 
		movdqa		[edi], xmm0;
		paddd		xmm1, [edi+16];		// 4-7
		movdqa		[edi+16], xmm1;
		paddd		xmm2, [edi+32];		// 8-11
		movdqa		[edi+32], xmm2;
		paddd		xmm3, [edi+48];		// 12-15
		movdqa		[edi+48], xmm3;

		// 16-31
		movdqa		xmm0, [esi+16];		// 16-31
		pmaxub		xmm0, xmm4;
		pminub		xmm0, xmm5;
		movdqa		xmm2, xmm0;			// コピー
		punpcklbw	xmm0, xmm7;		// byte -> word
		punpckhbw	xmm2, xmm7;		// byte -> word
		movdqa		xmm1, xmm0;
		movdqa		xmm3, xmm2;
		punpcklwd	xmm0, xmm7;		// word -> dword
		punpckhwd	xmm1, xmm7;
		punpcklwd	xmm2, xmm7;
		punpckhwd	xmm3, xmm7;

		paddd		xmm0, [edi+64];		// 16-19 
		movdqa		[edi+64], xmm0;
		paddd		xmm1, [edi+80];		// 20-23
		movdqa		[edi+80], xmm1;
		paddd		xmm2, [edi+96];		// 24-27
		movdqa		[edi+96], xmm2;
		paddd		xmm3, [edi+112];	// 28-31
		movdqa		[edi+112], xmm3;

		add			esi, 32;			// 次の32個
		add			edi, 128;

		dec			ebx;
		jnz			L1;
    }
#else
    __asm{
        // 引数
        mov             rsi, data;
        mov             rdi, acc;
        pxor            xmm7, xmm7;             // xmm7: zero
        mov             ebx, w;
        shr             ebx, 5;                 // w/32
        movzx           eax, BYTE PTR lower;
        movd            xmm4, eax;
        punpcklbw       xmm4, xmm4;
        punpcklwd       xmm4, xmm4;
        punpckldq       xmm4, xmm4;
        punpcklqdq      xmm4, xmm4;
        movzx           eax, BYTE PTR upper;
        movd            xmm5, eax;
        punpcklbw       xmm5, xmm5;
        punpcklwd       xmm5, xmm5;
        punpckldq       xmm5, xmm5;
        punpcklqdq      xmm5, xmm5;

        // loop
    L1:
        // 0-15
        movdqa          xmm0, [rsi];            // 0-15
        pmaxub          xmm0, xmm4;
        pminub          xmm0, xmm5;
        movdqa          xmm2, xmm0;             // コピー
        punpcklbw       xmm0, xmm7;             // byte -> word
        punpckhbw       xmm2, xmm7;             // byte -> word
        movdqa          xmm1, xmm0;
        movdqa          xmm3, xmm2;
        punpcklwd       xmm0, xmm7;             // word -> dword
        punpckhwd       xmm1, xmm7;
        punpcklwd       xmm2, xmm7;
        punpckhwd       xmm3, xmm7;

        paddd           xmm0, [rdi];            // 0-3 
        movdqa          [rdi], xmm0;
        paddd           xmm1, [rdi+16];         // 4-7
        movdqa          [rdi+16], xmm1;
        paddd           xmm2, [rdi+32];         // 8-11
        movdqa          [rdi+32], xmm2;
        paddd           xmm3, [rdi+48];         // 12-15
        movdqa          [rdi+48], xmm3;

        // 16-31
        movdqa          xmm0, [rsi+16];         // 16-31
        pmaxub          xmm0, xmm4;
        pminub          xmm0, xmm5;
        movdqa          xmm2, xmm0;                     // コピー
        punpcklbw       xmm0, xmm7;             // byte -> word
        punpckhbw       xmm2, xmm7;             // byte -> word
        movdqa          xmm1, xmm0;
        movdqa          xmm3, xmm2;
        punpcklwd       xmm0, xmm7;             // word -> dword
        punpckhwd       xmm1, xmm7;
        punpcklwd       xmm2, xmm7;
        punpckhwd       xmm3, xmm7;

        paddd           xmm0, [rdi+64];         // 16-19 
        movdqa          [rdi+64], xmm0;
        paddd           xmm1, [rdi+80];         // 20-23
        movdqa          [rdi+80], xmm1;
        paddd           xmm2, [rdi+96];         // 24-27
        movdqa          [rdi+96], xmm2;
        paddd           xmm3, [rdi+112];        // 28-31
        movdqa          [rdi+112], xmm3;

        add             rsi, 32;                        // 次の32個
        add             rdi, 128;

        dec             ebx;
        jnz             L1;
    }
#endif
}
void __stdcall SwdetectMMX::HorzAccum(
		unsigned long * va,
		int w,
		unsigned long * sat)
{
#ifndef _M_AMD64
	__asm {
		// top line
		mov			esi, va;
		mov			edi, sat;
		mov			ebx, w;
		xor			ecx, ecx;
		xor			eax, eax;		// accumulator
L1:
		add			eax, [esi+4*ecx];
		movnti		[edi+4*ecx], eax;
		add			eax, [esi+4*ecx+4];
		movnti		[edi+4*ecx+4], eax;
		add			eax, [esi+4*ecx+8];
		movnti		[edi+4*ecx+8], eax;
		add			eax, [esi+4*ecx+12];
		movnti		[edi+4*ecx+12], eax;
		add			eax, [esi+4*ecx+16];
		movnti		[edi+4*ecx+16], eax;
		add			eax, [esi+4*ecx+20];
		movnti		[edi+4*ecx+20], eax;
		add			eax, [esi+4*ecx+24];
		movnti		[edi+4*ecx+24], eax;
		add			eax, [esi+4*ecx+28];
		movnti		[edi+4*ecx+28], eax;

		add			ecx, 8;
		cmp			ecx, ebx;
		jnz			L1;
	}
#else
        __asm {
                // top line
                mov     rsi, va;
                mov     rdi, sat;
                mov     ebx, w;
                xor     rcx, rcx;
                xor     eax, eax;               // accumulator
        L1:
                add     eax, [rsi+4*rcx];
                movnti  [rdi+4*rcx], eax;
                add     eax, [rsi+4*rcx+4];
                movnti  [rdi+4*rcx+4], eax;
                add     eax, [rsi+4*rcx+8];
                movnti  [rdi+4*rcx+8], eax;
                add     eax, [rsi+4*rcx+12];
                movnti  [rdi+4*rcx+12], eax;
                add     eax, [rsi+4*rcx+16];
                movnti  [rdi+4*rcx+16], eax;
                add     eax, [rsi+4*rcx+20];
                movnti  [rdi+4*rcx+20], eax;
                add     eax, [rsi+4*rcx+24];
                movnti  [rdi+4*rcx+24], eax;
                add     eax, [rsi+4*rcx+28];
                movnti  [rdi+4*rcx+28], eax;

                add     ecx, 8;
                cmp     ecx, ebx;
                jnz     L1;
        }
#endif
}

void __stdcall SwdetectMMX::HorzAccumStep8(
		unsigned long * va,
		int w,
		int step,
		unsigned long * sat)
{
#ifndef _M_AMD64
	__asm {
		mov			esi, va;
		mov			edi, sat;
		mov			ebx, w;
		xor			ecx, ecx;
		xor			eax, eax;		// accumulator
L1:
		mov			edx, step;
		shr			edx, 3;			// step / 8
L2:
		add			eax, [esi+4*ecx];
		add			eax, [esi+4*ecx+4];
		add			eax, [esi+4*ecx+8];
		add			eax, [esi+4*ecx+12];
		add			eax, [esi+4*ecx+16];
		add			eax, [esi+4*ecx+20];
		add			eax, [esi+4*ecx+24];
		add			eax, [esi+4*ecx+28];
		add			ecx, 8;
		dec			edx;
		jnz			L2;

		mov			[edi], eax;
		add			edi, 4;

		cmp			ecx, ebx;
		jnz			L1;
	}
#else
        __asm {
                mov     rsi, va;
                mov     rdi, sat;
                mov     ebx, w;
                xor     rcx, rcx;
                xor     eax, eax;               // accumulator
        L1:
                mov     edx, step;
                shr     edx, 3;                 // step / 8
        L2:
                add     eax, [rsi+4*rcx];
                add     eax, [rsi+4*rcx+4];
                add     eax, [rsi+4*rcx+8];
                add     eax, [rsi+4*rcx+12];
                add     eax, [rsi+4*rcx+16];
                add     eax, [rsi+4*rcx+20];
                add     eax, [rsi+4*rcx+24];
                add     eax, [rsi+4*rcx+28];
                add     ecx, 8;
                dec     edx;
                jnz     L2;

                mov     [rdi], eax;
                add     rdi, 4;

                cmp     ecx, ebx;
                jnz     L1;
        }
#endif
}

void __stdcall SwdetectMMX::HorzAccumStep(
		unsigned long * va,
		int w,
		int step,
		unsigned long * sat)
{
#ifndef _M_AMD64
	__asm {
		mov			esi, va;
		mov			edi, sat;
		mov			ebx, w;
		xor			ecx, ecx;
		xor			eax, eax;		// accumulator
L1:
		mov			edx, step;
L2:
		add			eax, [esi+4*ecx];
		inc			ecx;
		dec			edx;
		jnz			L2;

		mov			[edi], eax;
		add			edi, 4;

		cmp			ecx, ebx;
		jnae		L1;
	}
#else
        __asm {
                mov     rsi, va;
                mov     rdi, sat;
                mov     ebx, w;
                xor     rcx, rcx;
                xor     eax, eax;               // accumulator
        L1:
                mov     edx, step;
        L2:
                add     eax, [rsi+4*rcx];
                inc     ecx;
                dec     edx;
                jnz     L2;

                mov     [rdi], eax;
                add     rdi, 4;

                cmp     ecx, ebx;
                jnae    L1;
        }
#endif
}
void __stdcall SwdetectMMX::MakeSAT(
        const unsigned char* data,        // 対象画像データ 16 byte boundary
        int w,           // 幅
		int h,			// 高さ
        unsigned long * sat)		// SAT
{
#ifndef _M_AMD64
	__asm {
		// top line
		mov			esi, data;
		mov			edi, sat;

		mov			ebx, w;
		xor			ecx, ecx;
		xor			eax, eax;		// accumulator
L1:
		movzx		edx, BYTE PTR [esi+ecx];
		add			eax, edx;
		mov			[edi+4*ecx], eax;
		movzx		edx, BYTE PTR [esi+ecx+1];
		add			eax, edx;
		mov			[edi+4*ecx+4], eax;
		movzx		edx, BYTE PTR [esi+ecx+2];
		add			eax, edx;
		mov			[edi+4*ecx+8], eax;
		movzx		edx, BYTE PTR [esi+ecx+3];
		add			eax, edx;
		mov			[edi+4*ecx+12], eax;

		add			ecx, 4;
		cmp			ecx, ebx;
		jnz			L1;

		add			esi, w;
		mov			eax, w;
		add			eax, eax;
		add			eax, eax;		// w*4
		add			edi, eax;
		mov			ecx, 1;			// pushed ecx : vertical counter
L2:
		push		ecx;
		xor			eax, eax;
		xor			ecx, ecx;		// ecx:horizontal counter
		xor			ebx, ebx;
		sub			ebx, w;			// ebx: (-w)
L3:
		movzx		edx, BYTE PTR [esi+ecx];
		add			eax, edx;
		mov			edx, eax;
		add			edx, [edi+4*ebx];
		mov			[edi+4*ecx], edx;
		movzx		edx, BYTE PTR [esi+ecx+1];
		add			eax, edx;
		mov			edx, eax;
		add			edx, [edi+4*ebx+4];
		mov			[edi+4*ecx+4], edx;
		movzx		edx, BYTE PTR [esi+ecx+2];
		add			eax, edx;
		mov			edx, eax;
		add			edx, [edi+4*ebx+8];
		mov			[edi+4*ecx+8], edx;
		movzx		edx, BYTE PTR [esi+ecx+3];
		add			eax, edx;
		mov			edx, eax;
		add			edx, [edi+4*ebx+12];
		mov			[edi+4*ecx+12], edx;

		mov			edx, ecx;
		sub			edx, ebx;	// edx: w
		add			ebx, 4;
		add			ecx, 4;
		cmp			ecx, edx;
		jnz			L3;

		add			esi, w;
		mov			eax, w;
		add			eax, eax;
		add			eax, eax;		// w*4
		add			edi, eax;

		pop			ecx;
		inc			ecx;
		cmp			ecx, h;
		jnz			L2;
	}
#else
        __asm {
                // top line
                mov     rsi, data;
                mov     rdi, sat;

                mov     ebx, w;
                xor     rcx, rcx;
                xor     eax, eax;               // accumulator
        L1:
                movzx   edx, BYTE PTR [rsi+rcx];
                add     eax, edx;
                mov     [rdi+4*rcx], eax;
                movzx   dx, BYTE PTR [rsi+rcx+1];
                add     eax, edx;
                mov     [rdi+4*rcx+4], eax;
                movzx   dx, BYTE PTR [rsi+rcx+2];
                add     eax, edx;
                mov     [rdi+4*rcx+8], eax;
                movzx   edx, BYTE PTR [rsi+rcx+3];
                add     eax, edx;
                mov     [rdi+4*rcx+12], eax;

                add     ecx, 4;
                cmp     ecx, ebx;
                jnz     L1;

                xor     rax, rax;
                mov     eax, w;
                add     rsi, rax;
                add     eax, eax;
                add     eax, eax;               // w*4
                add     rdi, rax;
                mov     ecx, 1;                 // pushed ecx : vertical counter
        L2:
                push    rcx;
                xor     eax, eax;
                xor     rcx, rcx;               // ecx:horizontal counter
                xor     ebx, ebx;
                sub     ebx, w;                 // ebx: (-w)
        L3:
                movzx   edx, BYTE PTR [rsi+rcx];
                add     eax, edx;
                mov     edx, eax;
                add     edx, [rdi+4*rbx];
                mov     [rdi+4*rcx], edx;
                movzx   edx, BYTE PTR [rsi+rcx+1];
                add     eax, edx;
                mov     edx, eax;
                add     edx, [rdi+4*rbx+4];
                mov     [rdi+4*rcx+4], edx;
                movzx   edx, BYTE PTR [rsi+rcx+2];
                add     eax, edx;
                mov     edx, eax;
                add     edx, [rdi+4*rbx+8];
                mov     [rdi+4*rcx+8], edx;
                movzx   edx, BYTE PTR [rsi+rcx+3];
                add     eax, edx;
                mov     edx, eax;
                add     edx, [rdi+4*rbx+12];
                mov     [rdi+4*rcx+12], edx;

                mov     edx, ecx;
                sub     edx, ebx;       // edx: w
                add     ebx, 4;
                add     ecx, 4;
                cmp     ecx, edx;
                jnz     L3;

                xor     rax, rax;
                mov     eax, w;
                add     rsi, rax;
                add     eax, eax;
                add     eax, eax;               // w*4
                add     rdi, rax;

                pop     rcx;
                inc     ecx;
                cmp     ecx, h;
                jnz     L2;
        }
#endif
}

void __stdcall SwdetectMMX::MakeSATline(
        const unsigned char* data,        // 対象画像データ 16 byte boundary
        int w,           // 幅
        unsigned long * sat,
		unsigned long * upper)	
{
#ifndef _M_AMD64
	__asm {
		// top line
		mov			esi, data;
		mov			edi, sat;
		mov			ebx, upper;

		xor			ecx, ecx;
		xor			eax, eax;		// accumulator
L1:
		movzx		edx, BYTE PTR [esi+ecx];
		add			eax, edx;
		mov			edx, eax;
		add			edx, [ebx+4*ecx];
		mov			[edi+4*ecx], edx;
		movzx		edx, BYTE PTR [esi+ecx+1];
		add			eax, edx;
		mov			edx, eax;
		add			edx, [ebx+4*ecx+4];
		mov			[edi+4*ecx+4], edx;
		movzx		edx, BYTE PTR [esi+ecx+2];
		add			eax, edx;
		mov			edx, eax;
		add			edx, [ebx+4*ecx+8];
		mov			[edi+4*ecx+8], edx;
		movzx		edx, BYTE PTR [esi+ecx+3];
		add			eax, edx;
		mov			edx, eax;
		add			edx, [ebx+4*ecx+12];
		mov			[edi+4*ecx+12], edx;

		add			ecx, 4;
		cmp			ecx, w;
		jnz			L1;

	}
#else
        __asm {
                // top line
                mov     rsi, data;
                mov     rdi, sat;
                mov     rbx, upper;

                xor     rcx, rcx;
                xor     eax, eax;               // accumulator
        L1:
                movzx   edx, BYTE PTR [rsi+rcx];
                add     eax, edx;
                mov     edx, eax;
                add     edx, [rbx+4*rcx];
                mov     [rdi+4*rcx], edx;
                movzx   edx, BYTE PTR [rsi+rcx+1];
                add     eax, edx;
                mov     edx, eax;
                add     edx, [rbx+4*rcx+4];
                mov     [rdi+4*rcx+4], edx;
                movzx   edx, BYTE PTR [rsi+rcx+2];
                add     eax, edx;
                mov     edx, eax;
                add     edx, [rbx+4*rcx+8];
                mov     [rdi+4*rcx+8], edx;
                movzx   edx, BYTE PTR [rsi+rcx+3];
                add     eax, edx;
                mov     edx, eax;
                add     edx, [rbx+4*rcx+12];
                mov     [rdi+4*rcx+12], edx;

                add     ecx, 4;
                cmp     ecx, w;
                jnz     L1;
        }
#endif
}

void __stdcall SwdetectMMX::LUTline(
		const unsigned char* data,
		int w,
		unsigned char * outdata, 
		unsigned char * lut)
{
#ifndef _M_AMD64
	__asm {
		mov		ebx, lut;
		mov		edx, w;
		mov		esi, data;
		mov		edi, outdata;
		xor		ecx, ecx;
L1:
		movzx	eax, [esi+ecx];
		movzx	eax, [ebx+eax];
		mov		[edi+ecx], al;
		movzx	eax, [esi+ecx+1];
		movzx	eax, [ebx+eax];
		mov		[edi+ecx+1], al;
		movzx	eax, [esi+ecx+2];
		movzx	eax, [ebx+eax];
		mov		[edi+ecx+2], al;
		movzx	eax, [esi+ecx+3];
		movzx	eax, [ebx+eax];
		mov		[edi+ecx+3], al;

		add		ecx, 4;
		cmp		ecx, edx;
		jnz		L1;
	}
#else
        __asm {
                mov     rbx, lut;
                mov     edx, w;
                mov     rsi, data;
                mov     rdi, outdata;
                xor     rcx, rcx;
        L1:
                movzx   rax, BYTE PTR [rsi+rcx];
                movzx   eax, BYTE PTR [rbx+rax];
                mov     [rdi+rcx], al;
                movzx   rax, BYTE PTR [rsi+rcx+1];
                movzx   eax, BYTE PTR [rbx+rax];
                mov     [rdi+rcx+1], al;
                movzx   rax, BYTE PTR [rsi+rcx+2];
                movzx   eax, BYTE PTR [rbx+rax];
                mov     [rdi+rcx+2], al;
                movzx   rax, BYTE PTR [rsi+rcx+3];
                movzx   eax, BYTE PTR [rbx+rax];
                mov     [rdi+rcx+3], al;

                add     ecx, 4;
                cmp     ecx, edx;
                jnz     L1;
        }
#endif
}

#else
// X64 with msc
#include "../ImgLibCLIx64/MakeSATasm_x64.h"

void __stdcall SwdetectMMX::VertAdd(
		const unsigned char* data,
		int w,
		unsigned long * acc)
{
	::VertAdd(data, w, acc);
}

void __stdcall SwdetectMMX::VertAddLimit(
		const unsigned char* data,
		int w,
		unsigned long * acc,
		unsigned char lower,
		unsigned char upper)
{
    ::VertAddLimit(data, w, acc, lower, upper);
}

void __stdcall SwdetectMMX::HorzAccum(
		unsigned long * va,
		int w,
		unsigned long * sat)
{
	::HorzAccum(va, w, sat);
}

void __stdcall SwdetectMMX::HorzAccumStep8(
		unsigned long * va,
		int w,
		int step,
		unsigned long * sat)
{
	::HorzAccumStep8(va, w, step, sat);
}

void __stdcall SwdetectMMX::HorzAccumStep(
		unsigned long * va,
		int w,
		int step,
		unsigned long * sat)
{
	::HorzAccumStep(va, w, step, sat);
}

void __stdcall SwdetectMMX::MakeSAT(
        const unsigned char* data,        // 対象画像データ 16 byte boundary
        int w,           // 幅
		int h,			// 高さ
        unsigned long * sat)		// SAT
{
	::MakeSAT(data, w, h, sat);
}

void __stdcall SwdetectMMX::MakeSATline(
        const unsigned char* data,        // 対象画像データ 16 byte boundary
        int w,           // 幅
        unsigned long * sat,
		unsigned long * upper)	
{
	::MakeSATline(data, w, sat, upper);
}

void __stdcall SwdetectMMX::LUTline(
		const unsigned char* data,
		int w,
		unsigned char * outdata, 
		unsigned char * lut)
{
	::LUTline(data, w, outdata, lut);
}

#endif