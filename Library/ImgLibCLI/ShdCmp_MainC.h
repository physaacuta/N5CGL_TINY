// *********************************************************************************
//	�V�F�[�f�B���O�␳ �N���X (C++�p)
//	[Ver]
//		Ver.01    2012/10/29  ����
//
//	[����]
//		�EC++�p�BCLI�p�Ƃ��Ă��g�����߁A�v���O�����{�̂́AImgLibCLI�ɂ����Ă���
// *********************************************************************************
#pragma once

#include "Common\ShdCmp_DeShade.h"

using namespace ImgLibCLI;

namespace ImgLibCLI
{
	class ShdCmp_MainC
	{
	public:
		//=======================================
		// �萔��`
		//=======================================
		static const long DEF_LPF_WIDTH		= 32;								// LPF�u���b�N��  �i0�`�摜�̕��j  8�̔{��
		static const long DEF_LPF_HEIGHT	= 32;								// LPF�u���b�N�����i0�`�摜�̍����j8�̔{��
		static const long DEF_LPF_TARGET	= 128;								// �V�F�[�����O�␳�̖ڕW�P�x�l �i0�`255�j


	public:
		ShdCmp_MainC(void);
		~ShdCmp_MainC(void);

		//=======================================
		// ���J���\�b�h
		//=======================================
		
		BOOL					Alloc();										// ���b�N
		void					Free();											// �J��

		BOOL	CreateLPF(long imgW, long imgH, long lpfW, long lpfH, BYTE taget);	// �摜�T�C�Y�ς�LPF����
		BOOL	Exec(BYTE const* pSrc, BYTE* pDst);									// �V�F�[�f�B���O�␳
		BOOL	ExecEdgeMask(long nPosL, long nPosR, BYTE const* pSrc, BYTE* pDst);	// �V�F�[�f�B���O�␳(�G�b�W�O������)
		BOOL	ExecEdgeCopy(long nPosL, long nPosR, BYTE const* pSrc, BYTE* pDst);	// �V�F�[�f�B���O�␳(�G�b�W�O�͌��摜)


		//=======================================
		// Static �֐�
		//=======================================
		static BOOL	ExecStatic(long imgW, long imgH, BYTE const* pSrc, BYTE* pDst);
		static BOOL	ExecStatic(long imgW, long imgH, long lpfW, long lpfH, BYTE taget, BYTE const* pSrc, BYTE* pDst);


	private:
		// �����o�[�N���X
		CDeShadeMMX*			mcls_pDeshade;									// ���ۂ̃V�F�[�����O�␳���{�p�N���X
		CDeShadeMMX::Params*	mcls_pParams;									// �V�F�[�����O�␳�p�����[�^�N���X
		

		// �����o�[�ϐ�
		long					m_nImgW;										// �Ώۉ摜�̕�
		long					m_nImgH;										// �Ώۉ摜�̍���
		bool					m_bIsLpf;										// LPF�����ςݎ� true
	
	};
};