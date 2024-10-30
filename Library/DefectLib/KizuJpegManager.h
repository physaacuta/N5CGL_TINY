// *********************************************************************************
//	Jpeg�摜�����N���X
//	[Ver]
//		Ver.01    2007/01/30  vs2005 �Ή�
//		Ver.02	  2010/11/30  ijl20.dll �� bin�̒��ɓ����K�v�Ȃ��ƂȂ���
//
//	[����]
//		�J�����ɂ́Aipp���C���X�g�[�����Ă����K�v�L��
//
////// �g�p�ɂ�����
// �E�v���W�F�N�g/�v���p�e�B��I��
// �E�ǉ��̃C���N���[�h�f�B���N�g���ŁwC:\Program Files\Intel\IPP\5.0\ia32\include�x��ǉ�
//     (ipp5.0�̃C���N���[�h�t�H���_)
// �E�ǉ��̃��C�u�����f�B���N�g���Łw..\..\include\Other\Ipp�x��ǉ�
//
// ���\�z
//	[�ǉ��C���N���[�h�p�X]
//		C:\Program Files\Intel\IPP\5.0\ia32\include
//	[�ǉ����C�u�����p�X]
//		..\Include\Other\Ipp
//	[�ǉ����C�u�����t�@�C��]
/*
ijl20l.lib
ippimerged.lib
ippsmerged.lib
ippjmerged.lib
ippcvmerged.lib
ippsemerged.lib
ippiemerged.lib
ippjemerged.lib
ippcvemerged.lib
ippcorel.lib
*/
/*
ijl20l.lib;ippimergedem64t.lib;ippsmergedem64t.lib;ippjmergedem64t.lib;ippcvmergedem64t.lib;ippsemergedem64t.lib;ippiemergedem64t.lib;ippjemergedem64t.lib;ippcvemergedem64t.lib;ippcoreem64tl.lib
*/
// *********************************************************************************



#pragma once

#include <windows.h>

#include "..\..\Include\LibSelect.h"											// ���C�u�����ؑփw�b�_�[

#ifdef DEFECTLIB_KIZUJPEGMANAGER
	// �s�̃C���N���[�h
	#include "..\..\include\Other\Ipp\ijl.h"									// JPEG�ϊ��̃C���N���[�h
#endif


namespace KizuLib
{
	class KizuJpegManager
	{
	public:
		KizuJpegManager(void);
		virtual ~KizuJpegManager(void);

		static int BmpToJpeg(int nW, int nH, const BYTE* bP, int jquality, int* jSize, BYTE* jP, bool nTB = false);		// BMP��JPEG �ϊ��i�O���[�j
		static int BmpToJpegColor(int nW, int nH, const BYTE* bP, int jquality, int* jSize, BYTE* jP, bool nTB = false);// BMP��JPEG �ϊ��i�J���[�j
		static int BmpToJPEGfile(int nW, int nH, const BYTE* bP, int jquality, const char* Path, bool nTB = false);	// BMP��JPEG �ϊ���t�@�C���������݁i�O���[�j


		static int JpegToBmp(const BYTE* jP, int jSize, int nW, int nH, BYTE* bP, bool nTB = false );					// JPEG��BMP �ϊ��i�O���[�j
		static int JpegToBmpColor(const BYTE* jP, int jSize, int nW, int nH, BYTE* bP, bool nTB = false );				// JPEG��BMP �ϊ��i�J���[�j

	};
};

