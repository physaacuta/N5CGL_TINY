// *********************************************************************************
//	libJpeg-turbo�摜�����N���X
//	[Ver]
//		Ver.01    2018/09/25  libjpeg-turbo ( dll ) �Ή�
//
//	[����]
//		LibJpeg-turbo�g�p���́ALoadLibrary��DLL��������̂ŁA�R���p�C�����͓��Ƀ��C�u�����Ƃ��̎Q�Ƃ͕s�v
//
////// �g�p�ɂ�����
// �E���s����DLL������K�v����
//
/*
turbojpeg_x64.dll
turbojpeg_x86.dll
*/
// *********************************************************************************



#pragma once

#include <windows.h>

#include "..\..\Include\LibSelect.h"											// ���C�u�����ؑփw�b�_�[

namespace KizuLib
{
	class KizuLjtJpegManager
	{
	private:
		void *tjEncode;

	public:
		KizuLjtJpegManager(void);
		~KizuLjtJpegManager(void);

		int BmpToJpeg(int nW, int nH, const unsigned char* bP, int jquality, int* jSize, unsigned char* jP, bool nTB = false);		// BMP��JPEG �ϊ��i�O���[�j
		int BmpToJpegColor(int nW, int nH, const unsigned char* bP, int jquality, int* jSize, unsigned char* jP, bool nTB = false);	// BMP��JPEG �ϊ��i�J���[�j
		int BmpToJPEGfile(int nW, int nH, const BYTE* bP, int jquality, const char* Path, bool nTB = false);						// BMP��JPEG �ϊ���t�@�C���������݁i�O���[�j


		int JpegToBmp(const unsigned char* jP, int jSize, int nW, int nH, unsigned char* bP, bool nTB = false);						// JPEG��BMP �ϊ��i�O���[�j
		int JpegToBmpColor(const unsigned char* jP, int jSize, int nW, int nH, unsigned char* bP, bool nTB = false);				// JPEG��BMP �ϊ��i�J���[�j
		int JpegToHeader(const unsigned char* jP, int jSize, int *nW, int *nH, bool *bColored);
	};
};
