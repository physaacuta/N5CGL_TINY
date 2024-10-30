#ifndef KIZU_MMX_TAP_RECONST_H
#define KIZU_MMX_TAP_RECONST_H

namespace KizuMMX
{

    // dst: width x heightの画像の左上、ピッチはdst_pitchバイト。
    // src_lefthalf : 左半分の画像(width/2 x height)の左上、ピッチはwidth/2
    // src_righthalf: 右半分の画像(width/2 x height)の左上、ピッチはwidth/2
    void __stdcall hflip_righthalf(
		const unsigned char* src_lefthalf,
		const unsigned char* src_righthalf,
		unsigned char* dst,
		long dst_pitch,
		long width, long height);

};

#endif
