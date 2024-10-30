#pragma once


#include <Windows.h>

#include "IportCommon.h"													// iPort用定数


// 混色分離
namespace IportLib
{
	static const int NCOEF = 3*4;				// 混色行列数 [RGB][R,G,B,offset値]


	// カラー画像をRGB画像に変換
	void Copy_PackedColor8_1p(BYTE* dstImg, void** pImgsrc, long pitch_dst, long pitch_src, long width, long height);
	void Copy_PackedColor8_2p(BYTE* dstImg, void** pImgsrc, long pitch_dst, long pitch_src, long width, long height);


	// カラー画像をRGB＋混色分離後画像に変換
	void Copy_PackedColor_Corr_1p(BYTE* dstImg, void** pImgsrc, long pitch_dst, long pitch_src, long width, long height, const float*const c);
	void Copy_PackedColor_Corr_2p(BYTE* dstImg, void** pImgsrc, long pitch_dst, long pitch_src, long width, long height, const float*const c);

	// カラー画像をRGB＋混色分離後画像に変換( ASM 版)
	void Copy_PackedColor_Corr_1p_MMX(BYTE* dstImg, void** pImgsrc, long pitch_dst, long pitch_src, long width, long height, const float*const c);
	void Copy_PackedColor_Corr_2p_MMX(BYTE* dstImg, void** pImgsrc, long pitch_dst, long pitch_src, long width, long height, const float*const c);

/*
	// Cは実際の係数を4096倍した整数値
	void make_rgbplanar_color_corr_C(BYTE*const* pdst, long pitch_dst,
									 const BYTE*const* psrc,
									 long width, long height, const short*const c);
*/
};