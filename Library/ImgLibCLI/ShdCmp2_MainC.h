// *********************************************************************************
//	�V�F�[�f�B���O�␳�̂Q �N���X (C++�p)
//	[Ver]
//		Ver.01    2013/6/22  ����
//
//	[����]
//		�ECLI�ォ�� �C�����C���A�Z���u�����́A�g�p�ł��Ȃ��ׁA�ȉ��̐ݒ���s������
//			�EMakeSATasm.cpp, LPFSAT.cpp
//				�v���p�e�B / �\���v���p�e�B / C/C++ / �S�� / �u���ʌ��ꃉ���^�C�� �T�|�[�g�ŃR���p�C���v���u�T�|�[�g���Ȃ��v�ɕύX
// *********************************************************************************
#pragma once

#include <windows.h>


namespace ImgLibCLI
{

	class ShdCmp2_MainC
	{
	public:
		//=======================================
		// �萔��`
		//=======================================
		static const int SHRINK_WIDTH	= 16;								// LPF�p�̏����P�ʕ� 8�̔{��
		static const int SHRINK_HEIGHT	= 16;								// LPF�p�̏����P�ʍ� 8�̔{��

		static const int DEF_LPF_WIDTH	= 64;								// �f�t�H���gLPF�u���b�N��  �i0�`�摜�̕��j  SHINK_WIDTH�̔{��
		static const int DEF_LPF_HEIGHT	= 64;								// �f�t�H���gLPF�u���b�N�����i0�`�摜�̍����jSHINK_HEIGHT�̔{��

		static const int DEF_LPF_TARGET	= 128;								// �f�t�H���g�V�F�[�����O�␳�̖ڕW�P�x�l �i0�`255�j


		ShdCmp2_MainC(int imgW, int imgH, int lpfW, int lpfH, BYTE target);		// �R���X�g���N�^
		ShdCmp2_MainC();
		virtual ~ShdCmp2_MainC();												// �f�X�g���N�^. IDispose::Dispose
		//=======================================
		// ���J���\�b�h
		//=======================================

		BOOL					Alloc();										// ALLOC(�_�~�[�j
		void					Free();											// �J��(�_�~�[�j

		BOOL	CreateLPF(int imgW, int imgH, int lpfW, int lpfH, BYTE taget);	// �摜�T�C�Y�ς�LPF����

		
		// �V�F�[�f�B���O�␳
		bool				Exec(const BYTE * pSrc, BYTE * pDst);				
		
		// �V�F�[�f�B���O�␳(�͈͌���.�G�b�W�O��LPF�T�C�Y���������Ȃ�)
		bool				ExecEdgeCopy(int lEdge, int rEdge, const BYTE * pSrc, BYTE * pDst );

		// �V�F�[�f�B���O�␳(�͈͌���.�G�b�W�O�͍��̈���)
		bool				ExecEdgeMask(int lEdge, int rEdge, const BYTE * pSrc, BYTE * pDst);


		//=======================================
		// Static �֐�
		//=======================================

		//IPP�������B���s���ɍœK��CPU�����[�`�����I�������B
		//���s��main�̓�������ŁA�ꔭ�ĂԂׂ��B
		static int  ippStaticInit();

		//�V�F�[�f�B���O�␳�iLPF�ETARGET�̓f�t�H���g�j
		static bool ExecStatic(int imgW, int imgH, const BYTE * pSrc, BYTE * pDst);
		//�V�F�[�f�B���O�␳�iLPF�ETARGET�w��j
		static bool	ExecStatic(int imgW, int imgH, int lpfW, int lpfH, BYTE taget, const BYTE * pSrc, BYTE * pDst);

	private:
		// �����o�[�N���X
		class CLPFSAT*				mcls_pLPFSAT;								// LPF�摜�ϊ��i�쐬�j�N���X
		class CDevideMMX*			mcls_pDivide;								// �ڕW�s�N�Z���P�x�ւ̕ϊ��N���X
		BYTE *						m_pLPFPic;									//�@LPF�������摜�B pSrc == pDst�̂Ƃ��p�e���|����
		BYTE						m_target;									// �ڕW�s�N�Z���P�x
	};
}
