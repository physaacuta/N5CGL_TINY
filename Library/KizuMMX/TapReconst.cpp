#include <Windows.h>
#include <crtdbg.h>

#include "TapReconst.h"

// �O���錾
void memcpy_2d(void* dst, int dst_pitch,
			   const void* src, int src_pitch,
			   int width, int height);


#ifndef _M_X64		// x86 �R���p�C��
void memcpy_2d_hflip_sse(void* dst, int dst_pitch,
							 const void* src,
							 int width, int height);

#endif

#ifndef _M_X64		// x86 �R���p�C��
// dst��aligned(16)�Adst_pitch��16�̔{���Awidth��32�̔{��(width/2��16�̔{��)
void __stdcall KizuMMX::hflip_righthalf(const unsigned char* src_lefthalf,
										const unsigned char* src_righthalf,
										unsigned char* dst, long dst_pitch,
										long width, long height)
{
    const long hwidth = width / 2;
    _ASSERT( 0 == (hwidth % 16) );
    _ASSERT( 0 == (dst_pitch % 16) );
    // ������
    memcpy_2d(dst, dst_pitch,
              src_lefthalf, hwidth,
              hwidth, height);

    // �E����
    memcpy_2d_hflip_sse(dst + hwidth, dst_pitch,
                        src_righthalf,
                        hwidth, height);
}
#endif

#ifndef _M_X64		// x86 �R���p�C��

static void memcpy_2d(void* dst, int dst_pitch,
                      const void* src, int src_pitch,
                      int width, int height)
{
    for(int i=0; i<height; i++)
    {
        memcpy(dst, src, width);
        dst = ((BYTE*)dst) + dst_pitch;
        src = ((BYTE*)src) + src_pitch;
    }
}

// src_pitch����o�[�W����
static void memcpy_2d_hflip_sse(void* dst, int dst_pitch,
                                const void* src, int src_pitch,
                                int width, int height)
{
	__asm
	{
		mov			esi, src;
		mov			ebx, src_pitch;

		mov			edi, dst;
		mov			edx, dst_pitch;

		add			edi, width;  // �ŏ��̍s�̍ŏI�o�C�g��1�o�C�g���Ɉړ�

	L1:
		mov			ecx, width;
		shr			ecx, 4;
		movd		xmm5, esi;   // xmm5: backup of esi
		movd		xmm6, edi;   // xmm6: backup of edi
	L0:
		sub			edi, 16;     // store�̑O�Ɉ���
		movdqu		xmm0, [esi];
		add			esi, 16;
		//
		movd		eax, xmm0;
		bswap		eax;
		movd		xmm1, eax;
		//
		pslldq		xmm1, 4;
		psrldq		xmm0, 4;
		movd		eax, xmm0;
		bswap		eax;
		movd		xmm2, eax;
		por			xmm1, xmm2;
		//
		pslldq		xmm1, 4;
		psrldq		xmm0, 4;
		movd		eax, xmm0;
		bswap		eax;
		movd		xmm3, eax;
		por			xmm1, xmm3;
		//
		pslldq		xmm1, 4;
		psrldq		xmm0, 4;
		movd		eax, xmm0;
		bswap		eax;
		movd		xmm4, eax;
		por			xmm1, xmm4;
		//
		movdqa		[edi], xmm1;
		//
		dec			ecx;
		jnz			L0;
		//
		movd		esi, xmm5;
		add			esi, ebx;
		movd		edi, xmm6;
		add			edi, edx;
		dec			height;
		jnz			L1;
	}
}

// src_pitch���Ȃ����ă������Q�Ƃ����炵���o�[�W����
// src_pitch����o�[�W���������{�� --> ������̗p
void memcpy_2d_hflip_sse(void* dst, int dst_pitch,
                         const void* src,
                         int width, int height)
{
	__asm
	{
		mov			esi, src;

		mov			edi, dst;
		mov			edx, dst_pitch;

		mov			ebx, width;
		movd		xmm5, ebx; // xmm5: backup of width

		add			edi, ebx;  // �ŏ��̍s�̍ŏI�o�C�g��1�o�C�g���Ɉړ�

		mov			ebx, height; // ebx: y-counter

	L1:
		movd		ecx, xmm5;   // ecx: x-counter
		shr			ecx, 4;
		movd		xmm6, edi;   // xmm6: backup of edi
	L0:
		sub			edi, 16;     // store�̑O�Ɉ���
		movdqu		xmm0, [esi];
		add			esi, 16;
		//
		movd		eax, xmm0;
		bswap		eax;
		movd		xmm1, eax;
		//
		pslldq		xmm1, 4;
		psrldq		xmm0, 4;
		movd		eax, xmm0;
		bswap		eax;
		movd		xmm2, eax;
		por			xmm1, xmm2;
		//
		pslldq		xmm1, 4;
		psrldq		xmm0, 4;
		movd		eax, xmm0;
		bswap		eax;
		movd		xmm3, eax;
		por			xmm1, xmm3;
		//
		pslldq		xmm1, 4;
		psrldq		xmm0, 4;
		movd		eax, xmm0;
		bswap		eax;
		movd		xmm4, eax;
		por			xmm1, xmm4;
		//
		movdqa		[edi], xmm1;
		//
		dec			ecx;
		jnz			L0;
		//
		movd		edi, xmm6;
		add			edi, edx;
		dec			ebx;
		jnz			L1;
	}
}

// ssse3���߂��g�p
static void memcpy_2d_hflip_ssse3(void* dst, int dst_pitch,
                                  const void* src, int src_pitch,
                                  int width, int height)
{
	__asm
	{
		mov			esi, src;
		mov			ebx, src_pitch;

		mov			edi, dst;
		mov			edx, dst_pitch;

		mov			ecx, height;

		mov			eax, width;
		movd		xmm1, eax;   // xmm1: backup of width

		add			edi, eax;  // �ŏ��̍s�̍ŏI�o�C�g��1�o�C�g���Ɉړ�

		mov			eax, 0x00010203;
		movd		xmm7, eax;
		pslldq		xmm7, 4;
		mov			eax, 0x04050607;
		movd		xmm6, eax;
		por			xmm7, xmm6;
		pslldq		xmm7, 4;
		mov			eax, 0x08090a0b;
		movd		xmm6, eax;
		por			xmm7, xmm6;
		pslldq		xmm7, 4;
		mov			eax, 0x0c0d0e0f;
		movd		xmm6, eax;
		por			xmm7, xmm6;  // xmm7: mask for shuffle

	L1:
		movd		eax, xmm1;
		shr			eax, 4;
		movd		xmm5, esi;   // xmm5: backup of esi
		movd		xmm6, edi;   // xmm6: backup of edi
	L0:
		sub			edi, 16;     // store�̑O�Ɉ���
		movdqu		xmm0, [esi];
		add			esi, 16;
		//
		pshufb		xmm0, xmm7; // only available on SSSE3
		//
		movdqa		[edi], xmm0;
		//
		dec			eax;
		jnz			L0;
		//
		movd		esi, xmm5;
		add			esi, ebx;
		movd		edi, xmm6;
		add			edi, edx;
		dec			ecx;
		jnz			L1;
	}
}

// ssse3���߂��g�p
// src_pitch�Ȃ�
// mask�̓��������烍�[�h
static void memcpy_2d_hflip_ssse3(void* dst, int dst_pitch,
                                  const void* src,
                                  int width, int height)
{
	static __declspec(align(16)) const BYTE mask[16] =
		{
			0x0f, 0x0e, 0x0d, 0x0c,
			0x0b, 0x0a, 0x09, 0x08,
			0x07, 0x06, 0x05, 0x04,
			0x03, 0x02, 0x01, 0x00
		};

	__asm
	{
		mov			esi, src;

		mov			edi, dst;
		mov			edx, dst_pitch;

		mov			ecx, height;

		mov			eax, width;
		mov			ebx, eax;    // ebx: backup of width

		add			edi, eax;  // �ŏ��̍s�̍ŏI�o�C�g��1�o�C�g���Ɉړ�

		movdqa		xmm7, mask; // xmm7: mask for shuffle(0x000102030405060708090a0b0c0d0e0f)

	L1:
		mov			eax, ebx;
		shr			eax, 4;
		movd		xmm6, edi;   // xmm6: backup of edi
	L0:
		sub			edi, 16;     // store�̑O�Ɉ���
		movdqu		xmm0, [esi];
		add			esi, 16;
		//
		pshufb		xmm0, xmm7; // only available on SSSE3
		//
		movdqa		[edi], xmm0;
		//
		dec			eax;
		jnz			L0;
		//
		movd		edi, xmm6;
		add			edi, edx;
		dec			ecx;
		jnz			L1;
	}
}
#endif