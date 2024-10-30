#pragma once


#include <Windows.h>

#include "IportCommon.h"													// iPort�p�萔


// ���F����
namespace IportLib
{
	static const int NCOEF = 3*4;				// ���F�s�� [RGB][R,G,B,offset�l]


	// �J���[�摜��RGB�摜�ɕϊ�
	void Copy_PackedColor8_1p(BYTE* dstImg, void** pImgsrc, long pitch_dst, long pitch_src, long width, long height);
	void Copy_PackedColor8_2p(BYTE* dstImg, void** pImgsrc, long pitch_dst, long pitch_src, long width, long height);


	// �J���[�摜��RGB�{���F������摜�ɕϊ�
	void Copy_PackedColor_Corr_1p(BYTE* dstImg, void** pImgsrc, long pitch_dst, long pitch_src, long width, long height, const float*const c);
	void Copy_PackedColor_Corr_2p(BYTE* dstImg, void** pImgsrc, long pitch_dst, long pitch_src, long width, long height, const float*const c);

	// �J���[�摜��RGB�{���F������摜�ɕϊ�( ASM ��)
	void Copy_PackedColor_Corr_1p_MMX(BYTE* dstImg, void** pImgsrc, long pitch_dst, long pitch_src, long width, long height, const float*const c);
	void Copy_PackedColor_Corr_2p_MMX(BYTE* dstImg, void** pImgsrc, long pitch_dst, long pitch_src, long width, long height, const float*const c);

/*
	// C�͎��ۂ̌W����4096�{���������l
	void make_rgbplanar_color_corr_C(BYTE*const* pdst, long pitch_dst,
									 const BYTE*const* psrc,
									 long width, long height, const short*const c);
*/
};