#include <Windows.h>
#include <crtdbg.h>

#include "TapReconst.h"
#include "asm_x64\TapReconst_x64.h"

// �O���錾
void memcpy_2d(void* dst, int dst_pitch,
			   const void* src, int src_pitch,
			   int width, int height);




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
