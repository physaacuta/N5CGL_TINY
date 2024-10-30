// *********************************************************************************
//	�V�F�[�f�B���O�␳�̂Q �N���X (CLI�p)
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



#include "../../../include/LibSelect.h"									// ���C�u�����ؑփw�b�_�[
//LPFSAT�̎�����IPP���g���Ă���̂ŁAIPP���ł́A�N���X��������B
#ifdef DEFECTLIB_KIZUJPEGMANAGER										

using namespace System;
using namespace System::Runtime::InteropServices;


namespace ImgLibCLI
{
	public ref class ShadingComp2
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


		//=======================================
		// ���J���\�b�h
		//=======================================
		ShadingComp2(int imgW, int imgH, int lpfW, int lpfH, BYTE target);		// �R���X�g���N�^
		virtual ~ShadingComp2();												// �f�X�g���N�^. IDispose::Dispose

		//�݊��C���^�[�t�F�[�X�Q
		ShadingComp2();		// �R���X�g���N�^
		bool					Alloc(long imgW, long imgH);					// ���b�N
		void					Free();											// �J��
		bool					CreateLPF();									// LPF����
		bool					CreateLPF(long lpfW, long lpfH, BYTE taget);	// LPF����
		
		// �V�F�[�f�B���O�␳
		bool				Exec(array<BYTE>^ pSrc, array<BYTE>^ pDst);	
		bool				Exec(IntPtr pSrc, IntPtr pDst);				
		
		// �V�F�[�f�B���O�␳(�͈͌���.�G�b�W�O�͌��摜�̂܂�)
		bool				ExecEdgeCopy(array<BYTE>^ pSrc, array<BYTE>^ pDst, int lEdge, int rEdge);
		bool				ExecEdgeCopy(IntPtr pSrc, IntPtr pDst, int lEdge, int rEdge);			 

		// �V�F�[�f�B���O�␳(�͈͌���.�G�b�W�O�͍��̈���)
		bool				ExecEdgeMask(array<BYTE>^ pSrc, array<BYTE>^ pDst, int lEdge, int rEdge);
		bool				ExecEdgeMask(IntPtr pSrc, IntPtr pDst, int lEdge, int rEdge);			   


		//=======================================
		// Static �֐�
		//=======================================
		//�V�F�[�f�B���O�␳�iLPF�ETARGET�̓f�t�H���g�j
		static bool ExecStatic(long imgW, long imgH, array<BYTE>^ pSrc, array<BYTE>^ pDst);
		static bool ExecStatic(long imgW, long imgH, IntPtr pSrc, IntPtr pDst);
		//�V�F�[�f�B���O�␳�iLPF�ETARGET�w��j
		static bool	ExecStatic(long imgW, long imgH, long lpfW, long lpfH, BYTE taget, array<BYTE>^ pSrc, array<BYTE>^ pDst);
		static bool	ExecStatic(long imgW, long imgH, long lpfW, long lpfH, BYTE taget, IntPtr pSrc,IntPtr pDst);

	private:
		// �����o�[�N���X
		class CLPFSAT*				mcls_pLPFSAT;								// LPF�摜�ϊ��i�쐬�j�N���X
		class CDevideMMX*			mcls_pDivide;								// �ڕW�s�N�Z���P�x�ւ̕ϊ��N���X
		IntPtr						m_pLPFPic;									//�@LPF�������摜�B pSrc == pDst�̂Ƃ��p�e���|����
		BYTE						m_target;
		
		int							m_nImgWidth;
		int							m_nImgHeight;
	};
}
#endif