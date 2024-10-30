#ifndef KIZU_MMX_TAP_RECONST_X64_H
#define KIZU_MMX_TAP_RECONST_X64_H

//namespace KizuMMX
//{
    // dst: width x heightの画像の左上、ピッチはdst_pitchバイト。
    // src_lefthalf : 左半分の画像(width/2 x height)の左上、ピッチはwidth/2
    // src_righthalf: 右半分の画像(width/2 x height)の左上、ピッチはwidth/2
	extern "C" void __cdecl memcpy_2d_hflip_sse(void* dst, int dst_pitch, const void* src,
			                                    int width, int height);
//};

#endif
