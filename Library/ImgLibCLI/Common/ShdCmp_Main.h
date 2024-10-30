// *********************************************************************************
//	�V�F�[�f�B���O�␳ �N���X (CLI�p)
//	[Ver]
//		Ver.01    2008/10/28  ����
//
//	[����]
//		�ECLI�ォ�� �C�����C���A�Z���u�����́A�g�p�ł��Ȃ��ׁA�ȉ��̐ݒ���s������
//			�EShdCmp_Main.cpp�ȊO�̃V�F�[�����O�␳�p��*.cpp���ΏۂƂ��āA
//				�v���p�e�B / �\���v���p�e�B / C/C++ / �S�� / �u���ʌ��ꃉ���^�C�� �T�|�[�g�ŃR���p�C���v���u�T�|�[�g���Ȃ��v�ɕύX
// *********************************************************************************
#pragma once

#include <windows.h>
#include "ShdCmp_DeShade.h"

using namespace System;
using namespace System::Runtime::InteropServices;

using namespace ImgLibCLI;

namespace ImgLibCLI
{

	public ref class ShadingComp
	{
	public:
		//=======================================
		// �萔��`
		//=======================================
		static const long DEF_LPF_WIDTH		= 32;								// LPF�u���b�N��  �i0�`�摜�̕��j  8�̔{��
		static const long DEF_LPF_HEIGHT	= 32;								// LPF�u���b�N�����i0�`�摜�̍����j8�̔{��
		static const long DEF_LPF_TARGET	= 128;								// �V�F�[�����O�␳�̖ڕW�P�x�l �i0�`255�j


		//=======================================
		// ���J���\�b�h
		//=======================================
		ShadingComp();															// �R���X�g���N�^
		virtual ~ShadingComp();													// �f�X�g���N�^
		
		BOOL					Alloc(long imgW, long imgH);					// ���b�N
		void					Free();											// �J��
		BOOL					CreateLPF();									// LPF����
		BOOL					CreateLPF(long lpfW, long lpfH, BYTE taget);	// LPF����
		BOOL					Exec(array<BYTE>^ pSrc, array<BYTE>^ pDst);		// �V�F�[�f�B���O�␳
		BOOL					ExecEdgeMask(array<BYTE>^ pSrc, array<BYTE>^ pDst, long lEdge, long rEdge);// �V�F�[�f�B���O�␳(�G�b�W�O������)
		BOOL					ExecEdgeCopy(array<BYTE>^ pSrc, array<BYTE>^ pDst, long lEdge, long rEdge);// �V�F�[�f�B���O�␳(�G�b�W�O���摜)

		//=======================================
		// Static �֐�
		//=======================================
		static BOOL	ExecStatic(long imgW, long imgH, array<BYTE>^ pSrc, array<BYTE>^ pDst);
		static BOOL	ExecStatic(long imgW, long imgH, long lpfW, long lpfH, BYTE target, array<BYTE>^ pSrc, array<BYTE>^ pDst);


	private:
		// �����o�[�N���X
		CDeShadeMMX*			mcls_pDeshade;									// ���ۂ̃V�F�[�����O�␳���{�p�N���X
		CDeShadeMMX::Params*	mcls_pParams;									// �V�F�[�����O�␳�p�����[�^�N���X
		//BYTE*					m_pImgBuf;										// �V�F�[�����O�␳�ϊ��摜�i�[�̈�
		IntPtr					m_pImgBuf;										// �V�F�[�����O�␳�ϊ��摜�i�[�̈�
		

		// �����o�[�ϐ�
		long					m_nImgW;										// �Ώۉ摜�̕�
		long					m_nImgH;										// �Ώۉ摜�̍���
		bool					m_bIsLpf;										// LPF�����ςݎ� true
	
	};
}
