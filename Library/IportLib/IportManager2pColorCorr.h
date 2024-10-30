#pragma once

// ���[�J��
#include "IportManager2p.h"													// iPort2p���N���X
#include "make_planar_color_corr.h"

// �C���|�[�g
using namespace KizuLib;
using namespace ImgLib;

namespace IportLib
{

	//===================================================================
	// �J�����捞�{��
	//===================================================================
	class IportManager2pColorCorr :	public IportManager2p
								  , implements IGrabberCorrMat
	{
	//// ���J�\����
	public:


	//// ���J�֐�
	public:
		IportManager2pColorCorr(CString sMyManagerName="iPortMgr2pClr", CString sMyIport="iPortBase");
		virtual ~IportManager2pColorCorr(void);

		virtual bool		Init(DWORD nFrameW, DWORD nFrameH, DWORD camset=1, DWORD angle=1, BYTE nDummyVal=0x80);	// ������
		virtual void		Dispose();															// ���

		virtual void		SetCorrMat(int camset, const float* mat);							// ���F�s��l�Z�b�g
		virtual void		ResetCorrMat();														// ���F�s��l���Z�b�g


	//// �����o�[�֐�
	protected:
		virtual void		OnFrameGrabbed(void** pImgs, long nX, long nY, DWORD nFrameNum, DWORD nCamset, WORD* nSeqNum, __int64 nFastTime);	// 1�J�����Z�b�g���̉摜���擾���āA�X���b�h���[�v�L���[�ɃL���[�C���O


	//// �����o�[�ϐ�
    private:
		float***			my_mat_corr;					// ���F�s��l [0or1][�J�����Z�b�g][���F�s��l (r,g,b,offset)]
        int					my_mat_fore;					// �_�u���o�b�t�@�[�̍l�����Bmy_mat_corr�̂ǂ����̔z����g����

	};
};

