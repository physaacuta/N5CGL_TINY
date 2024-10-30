// *********************************************************************************
//	Bitmap�摜�����N���X
//	[Ver]
//		Ver.01    2007/01/30  vs2005 �Ή�
//
//	[����]
//		�E�ʏ�́ABMP�t�@�C���̐擪�̕��Ƀf�[�^���Z�b�g�����
//			�摜�Ƃ��ẮA���̂ق��ɕ\�������
//		�E�r����FC�ō������́A�w�b�_�ŏ㉺���΂Ƃ��Ă���B�f�[�^�́A�z��̏オ�A�摜����ƂȂ�
// *********************************************************************************

#pragma once

#include <windows.h>

namespace KizuLib
{
	class KizuBitmapManager
	{
	public:
		KizuBitmapManager(void);												// �R���X�g���N�^
		virtual ~KizuBitmapManager(void);										// �f�X�g���N�^
		
		//�J���[�摜�p
		int GetBitmapHeaderSize_Color();														// Bitmap�w�b�_�T�C�Y�擾
		int CreateBitmapHeader_Color(int nW, int nH, BYTE* BmpHead, bool nTB = false);		// Bitmap�w�b�_����
		bool DrawText_Color(int nW, int nH, BYTE* BmpData, int nLeft, int nTop, char const* szText, int nFontSize, DWORD color);// ������`��


		// �O���[�摜�p
		int GetBitmapHeaderSize_Gray();														// Bitmap�w�b�_�T�C�Y�擾
		int CreateBitmapHeader_Gray(int nW, int nH, BYTE* BmpHead, bool nTB = false);		// Bitmap�w�b�_����
		void DataToBMPfile_Gray(int nW, int nH, const BYTE* BmpData, const BYTE* BmpHead, const char* Path);// Bitmap�t�@�C���֏o��
		void DataToBMPfile_Gray(int nW, int nH, const BYTE* BmpData, const char* Path, bool nTB = false);	// Bitmap�t�@�C���֏o�� (Bitmap�w�b�_�������Ő���)
		int BMPfileToData_Gray(const char* Path, int* nW, int* nH, BYTE *BmpData);			// Bitmap�t�@�C���ǂݍ���
		int BMPfileToData_Gray(const char* Path, int* nW, int* nH);							// Bitmap�t�@�C���̃w�b�_�[���̂ݓǍ���

		void DrawLine_Gray(int nW, int nH, BYTE *BmpData, int nX1, int nY1, int nX2, int nY2, BYTE color=255);	// ����`��
		bool DrawText_Gray(int nW, int nH, BYTE* BmpData, int nLeft, int nTop, char const* szText, int nFontSize=16, BYTE color=255);	// ������`��


		// 2�l�摜�p
		int GetBitmapHeaderSize_Bin();														// Bitmap�w�b�_�T�C�Y�擾
		int CreateBitmapHeader_Bin(int nW, int nH, BYTE* BmpHead, bool nTB = false);		// Bitmap�w�b�_����
		void DataToBMPfile_Bin(int nW, int nH, const BYTE* BmpData, const BYTE* BmpHead, const char* Path);	// Bitmap�t�@�C���֏o��
		void DataToBMPfile_Bin(int nW, int nH, const BYTE* BmpData, const char* Path, bool nTB = false);	// Bitmap�t�@�C���֏o�� (Bitmap�w�b�_�������Ő���)
//		int BMPfileToData(const char* Path, int* nW, int* nH, BYTE *BmpData);				// Bitmap�t�@�C���ǂݍ���

/*
	int GetBitmapHeaderSize2();
	int CreateBitmapHeader2(int bW, int bH, BYTE *BmpHead);
	void DataToBMPfile2(int bW, int bH, const BYTE *BmpData, const char *Path);
	void DataToBMPfile2(int bW, int bH, const BYTE *BmpData, BYTE *BmpHead, const char *Path);
*/

	};
};




