#include <Windows.h>
#include <crtdbg.h>
#include "MemCopy.h"

#ifndef _M_X64		// x86 コンパイル

// 8バイト単位のコピー
void __stdcall KizuMMX::mem_copy_8(
    const unsigned char* src,
    unsigned long src_pitch,
    unsigned char* dst,
    unsigned long dst_pitch,
    unsigned long width,
    unsigned long height)
{
	_ASSERT( 0 == width % 8 );

	__asm{
		// 引数
		mov			esi, src;
		//
		mov			ebx, src_pitch;
		mov			eax, width;
		sub			ebx, eax;
		//
		mov			edi, dst;
		//
		mov			edx, dst_pitch;
		sub			edx, eax;
		//
		mov			ecx, height;
		//
		shr			eax, 3;         // eax: width / 8
		movd		mm7, eax;

	NEXT:
		movq		mm0, [esi];
		movq		[edi], mm0;

		add			esi, 8;
		add			edi, 8;
		dec			eax;
		jnz			NEXT;

		add			esi, ebx;
		add			edi, edx;
		movd		eax, mm7;
		dec			ecx;
		jnz			NEXT;

		emms;
	}
}

// 32バイト単位のコピー
void __stdcall KizuMMX::mem_copy_32(
    const unsigned char* src,
    unsigned long src_pitch,
    unsigned char* dst,
    unsigned long dst_pitch,
    unsigned long width,
    unsigned long height)
{
	_ASSERT( 0 == width % 32 );

	__asm{
		// 引数
		mov			esi, src;
		//
		mov			ebx, src_pitch;
		mov			eax, width;
		sub			ebx, eax;
		//
		mov			edi, dst;
		//
		mov			edx, dst_pitch;
		sub			edx, eax;
		//
		mov			ecx, height;
		//
		shr			eax, 5;         // eax: width / 32
		movd		mm7, eax;

	NEXT:
		movq		mm0, [esi];
		movq		mm1, [esi + 8];
		movq		mm2, [esi + 16];
		movq		mm3, [esi + 24];
		movq		[edi], mm0;
		movq		[edi + 8], mm1;
		movq		[edi + 16], mm2;
		movq		[edi + 24], mm3;

		add			esi, 32;
		add			edi, 32;
		dec			eax;
		jnz			NEXT;

		add			esi, ebx;
		add			edi, edx;
		movd		eax, mm7;
		dec			ecx;
		jnz			NEXT;

		emms;
	}
}

// 64バイト単位のコピー
void __stdcall KizuMMX::mem_copy_64(
    const unsigned char* src,
    unsigned long src_pitch,
    unsigned char* dst,
    unsigned long dst_pitch,
    unsigned long width,
    unsigned long height)
{
	_ASSERT( 0 == width % 64 );

	__asm{
		// 引数
		mov			esi, src;
		//
		mov			ebx, src_pitch;
		mov			eax, width;
		sub			ebx, eax;
		//
		mov			edi, dst;
		//
		mov			edx, dst_pitch;
		sub			edx, eax;
		//
		mov			ecx, height;
		//
		shr			eax, 6;         // eax: width / 64
		mov			width, eax;

	NEXT:
		movq		mm0, [esi];
		movq		mm1, [esi + 8];
		movq		mm2, [esi + 16];
		movq		mm3, [esi + 24];
		movq		mm4, [esi + 32];
		movq		mm5, [esi + 40];
		movq		mm6, [esi + 48];
		movq		mm7, [esi + 56];
		movq		[edi], mm0;
		movq		[edi + 8], mm1;
		movq		[edi + 16], mm2;
		movq		[edi + 24], mm3;
		movq		[edi + 32], mm4;
		movq		[edi + 40], mm5;
		movq		[edi + 48], mm6;
		movq		[edi + 56], mm7;

		add			esi, 64;
		add			edi, 64;
		dec			eax;
		jnz			NEXT;

		add			esi, ebx;
		add			edi, edx;
		mov			eax, width;
		dec			ecx;
		jnz			NEXT;

		emms;
	}
}

// SSE2命令を用いた16バイト単位のコピー
void __stdcall KizuMMX::mem_copy_16xmm(
    const unsigned char* src,					// must be aligned on 16byte boundary
    unsigned long src_pitch,
    unsigned char* dst,
    unsigned long dst_pitch,
    unsigned long width,
    unsigned long height)
{
	_ASSERT( 0 == width % 16 );

	__asm{
		// 引数
		mov			esi, src;
		//
		mov			ebx, src_pitch;
		mov			eax, width;
		sub			ebx, eax;
		//
		mov			edi, dst;
		//
		mov			edx, dst_pitch;
		sub			edx, eax;
		//
		mov			ecx, height;
		//
		shr			eax, 4;         // eax: width / 16
		mov			width, eax;

	NEXT:
		// PentiumIIIでも使えるようにmovapsを使う
		movaps		xmm0, [esi];

		// PentiumIIIでも使えるようにmovntpsを使う
		movntps		[edi], xmm0;

		add			esi, 16;
		add			edi, 16;
		dec			eax;
		jnz			NEXT;

		add			esi, ebx;
		add			edi, edx;
		mov			eax, width;
		dec			ecx;
		jnz			NEXT;
	}
}

// SSE2命令を用いた64バイト単位のコピー
void __stdcall KizuMMX::mem_copy_64xmm(
    const unsigned char* src,					// must be aligned on 16byte boundary
    unsigned long src_pitch,
    unsigned char* dst,
    unsigned long dst_pitch,
    unsigned long width,
    unsigned long height)
{
	_ASSERT( 0 == width % 64 );

	__asm{
		// 引数
		mov			esi, src;
		//
		mov			ebx, src_pitch;
		mov			eax, width;
		sub			ebx, eax;
		//
		mov			edi, dst;
		//
		mov			edx, dst_pitch;
		sub			edx, eax;
		//
		mov			ecx, height;
		//
		shr			eax, 6;         // eax: width / 64
		mov			width, eax;

	NEXT:
		// PentiumIIIでも使えるようにmovapsを使う
		movaps		xmm0, [esi];
		movaps		xmm1, [esi + 16];
		movaps		xmm2, [esi + 32];
		movaps		xmm3, [esi + 48];

		// PentiumIIIでも使えるようにmovntpsを使う
		movntps		[edi],      xmm0;
		movntps		[edi + 16], xmm1;
		movntps		[edi + 32], xmm2;
		movntps		[edi + 48], xmm3;

		add			esi, 64;
		add			edi, 64;
		dec			eax;
		jnz			NEXT;

		add			esi, ebx;
		add			edi, edx;
		mov			eax, width;
		dec			ecx;
		jnz			NEXT;
	}
}

// SSE2命令を用いた64バイト単位のメモリクリア
void __stdcall KizuMMX::mem_clear_64xmm(
    void* dst,									// must be aligned on 16byte boundary
    unsigned long sz)
{
	_ASSERT( 0 == sz % 64 );

	__asm{
		// 引数
		mov			edi, dst;
		mov			ecx, sz;
		shr			ecx, 6;         // eax: width / 64
		pxor		xmm7, xmm7;

	NEXT:
		movntdq		[edi], xmm7;
		movntdq		[edi+16], xmm7;
		movntdq		[edi+32], xmm7;
		movntdq		[edi+48], xmm7;

		add			edi, 64;
		dec			ecx;
		jnz			NEXT;
	}
}

// SSE2命令を用いた64バイト単位のメモリセット
void __stdcall KizuMMX::mem_set_64xmm_short(
    unsigned short* dst,						// must be aligned on 16byte boundary
	unsigned short val,
    unsigned long sz)
{
	_ASSERT( 0 == sz % 32 );

	__asm{
		// 引数
		mov			edi, dst;
		mov			ecx, sz;
		shr			ecx, 5;         // eax: sz / 32
		movzx		eax, WORD PTR val;
		movd		xmm7, eax;
		punpcklwd	xmm7, xmm7;
		punpckldq	xmm7, xmm7;
		punpcklqdq	xmm7, xmm7;

	NEXT:
		movntdq		[edi], xmm7;
		movntdq		[edi+16], xmm7;
		movntdq		[edi+32], xmm7;
		movntdq		[edi+48], xmm7;

		add			edi, 64;
		dec			ecx;
		jnz			NEXT;
	}
}

// ブロック単位のSSE2命令を用いた64バイト単位のコピー
void __stdcall KizuMMX::mem_copy_64xmm(
	void *dst,
	const void *src,
	size_t bksize)
{
	_ASSERT( 0 == bksize % 64 );

	__asm {
		mov			esi, src;
		mov			edi, dst;
		mov			ecx, bksize;
		dec			ecx;
		sar			ecx, 6;
		inc			ecx;
	L1:
		movdqa		xmm0, [esi];		// load 0-15
		add			edi, 64;
		movdqa		xmm1, [esi+16];		// load 16-31
		add			esi, 64;
		movdqa		xmm2, [esi-32];		// load 32-47
		movntdq		[edi-64], xmm0;		// store 0-15
		movdqa		xmm3, [esi-16];		// load 48-63
		movntdq		[edi-48], xmm1;		// store 16-31
		movntdq		[edi-32], xmm2;		// store 32-47
		dec			ecx;
		movntdq		[edi-16], xmm3;		// store 48-63
		jnz			L1;
	}
}

// non-temporal(キャッシュを消費しない）書き込みを用いたメモリコピー
void __stdcall KizuMMX::mem_copy_nt(
	void *dst,
	const void *src,
	size_t bksize)
{
	__asm {
		mov			esi, src;
		mov			edi, dst;
		mov			ecx, bksize;
		and			ecx, 31;
		rep			movsb;	

		mov			ecx, bksize;
		shr			ecx, 5;
		test		ecx, ecx;
		jz			LEND;

	L1:
		movq		mm0, [esi];
		movq		mm1, [esi+8];
		movq		mm2, [esi+16];
		movq		mm3, [esi+24];
		add			esi, 32;
		movntq		[edi], mm0;
		movntq		[edi+8], mm1;
		movntq		[edi+16], mm2;
		movntq		[edi+24], mm3;
		add			edi, 32;

		dec			ecx;
		jnz			L1;

	LEND:
		emms;
	}
}
        
#else		// x64 コンパイル

#include "asm_x64\MemCopy_x64.h"

// 8バイト単位のコピー
void __stdcall KizuMMX::mem_copy_8(
    const unsigned char* src,
    unsigned long src_pitch,
    unsigned char* dst,
    unsigned long dst_pitch,
    unsigned long width,
    unsigned long height)
{
    _ASSERT( 0 == width % 8 );

    ::mem_copy_8(src, src_pitch, dst, dst_pitch, width, height);
}

// 32バイト単位のコピー
void __stdcall KizuMMX::mem_copy_32(
    const unsigned char* src,
    unsigned long src_pitch,
    unsigned char* dst,
    unsigned long dst_pitch,
    unsigned long width,
    unsigned long height)
{
    _ASSERT( 0 == width % 32 );

    ::mem_copy_32(src, src_pitch, dst, dst_pitch, width, height);
}

// 64バイト単位のコピー
void __stdcall KizuMMX::mem_copy_64(
    const unsigned char* src,
    unsigned long src_pitch,
    unsigned char* dst,
    unsigned long dst_pitch,
    unsigned long width,
    unsigned long height)
{
    _ASSERT( 0 == width % 64 );

    ::mem_copy_64(src, src_pitch, dst, dst_pitch, width, height);
}

// SSE2命令を用いた64バイト単位のコピー
void __stdcall KizuMMX::mem_copy_64xmm(
    const unsigned char* src,					// must be aligned on 16byte boundary
    unsigned long src_pitch,
    unsigned char* dst,
    unsigned long dst_pitch,
    unsigned long width,
    unsigned long height)
{
    _ASSERT( 0 == width % 64 );

    ::mem_copy_x64_pitch(src, src_pitch, dst, dst_pitch, width, height);
}

// SSE2命令を用いた64バイト単位のコピー
void __stdcall KizuMMX::mem_copy_16xmm(
    const unsigned char* src,					// must be aligned on 16byte boundary
    unsigned long src_pitch,
    unsigned char* dst,
    unsigned long dst_pitch,
    unsigned long width,
    unsigned long height)
{
    _ASSERT( 0 == width % 16 );

    ::mem_copy_16xmm(src, src_pitch, dst, dst_pitch, width, height);
}

// SSE2命令を用いた64バイト単位のメモリクリア
void __stdcall KizuMMX::mem_clear_64xmm(
    void* dst,									// must be aligned on 16byte boundary
    unsigned long sz)
{
    _ASSERT( 0 == sz % 64 );

    ::mem_clear_64xmm(dst, sz);
}

// SSE2命令を用いた64バイト単位のメモリセット
void __stdcall KizuMMX::mem_set_64xmm_short(
    unsigned short* dst,						// must be aligned on 16byte boundary
	unsigned short val,
    unsigned long sz)
{
    _ASSERT( 0 == sz % 32 );

	::mem_set_64xmm_short(dst, val, sz);
}

// ブロック単位のSSE2命令を用いた64バイト単位のコピー
void __stdcall KizuMMX::mem_copy_64xmm(
	void *dst,
	const void *src,
	size_t bksize)
{
    _ASSERT( 0 == bksize % 64 );

    ::mem_copy_x64_block(dst, src, bksize);
}

// non-temporal(キャッシュを消費しない）書き込みを用いたメモリコピー
void __stdcall KizuMMX::mem_copy_nt(
	void *dst,
	const void *src,
	size_t bksize)
{
    ::mem_copy_nt(dst, src, bksize);
}

#endif
