#pragma once


#include <Windows.h>

#include "IportCommon.h"													// iPortpè


// ¬Fª£
namespace IportLib
{
	static const int NCOEF = 3*4;				// ¬Fsñ [RGB][R,G,B,offsetl]


	// J[æðRGBæÉÏ·
	void Copy_PackedColor8_1p(BYTE* dstImg, void** pImgsrc, long pitch_dst, long pitch_src, long width, long height);
	void Copy_PackedColor8_2p(BYTE* dstImg, void** pImgsrc, long pitch_dst, long pitch_src, long width, long height);


	// J[æðRGB{¬Fª£ãæÉÏ·
	void Copy_PackedColor_Corr_1p(BYTE* dstImg, void** pImgsrc, long pitch_dst, long pitch_src, long width, long height, const float*const c);
	void Copy_PackedColor_Corr_2p(BYTE* dstImg, void** pImgsrc, long pitch_dst, long pitch_src, long width, long height, const float*const c);

	// J[æðRGB{¬Fª£ãæÉÏ·( ASM Å)
	void Copy_PackedColor_Corr_1p_MMX(BYTE* dstImg, void** pImgsrc, long pitch_dst, long pitch_src, long width, long height, const float*const c);
	void Copy_PackedColor_Corr_2p_MMX(BYTE* dstImg, void** pImgsrc, long pitch_dst, long pitch_src, long width, long height, const float*const c);

/*
	// CÍÀÛÌWð4096{µ½®l
	void make_rgbplanar_color_corr_C(BYTE*const* pdst, long pitch_dst,
									 const BYTE*const* psrc,
									 long width, long height, const short*const c);
*/
};