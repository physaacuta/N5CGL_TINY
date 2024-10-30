#include "StdAfx.h"
#include "IportManager2pColorCorr.h"

// �������[���[�N���o ----->>>
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif
// <<<------------------------

using namespace IportLib;


////===================================================================
//// �}�N��
////===================================================================
//#define LOM(t) mcls_LogMe.Write(KizuLib::LogFileManager::t			// ���[�J����p���O


//------------------------------------------
// �R���X�g���N�^
// CString sMyManagerName ���N���X�̖���
// CString sMyIport iPort�N���X�̖���
//------------------------------------------
IportManager2pColorCorr::IportManager2pColorCorr(CString sMyManagerName, CString sMyIport) :
// 1.���O
IportManager2p(sMyManagerName, sMyIport),
my_mat_corr(NULL),
my_mat_fore(0)
{
	// �J���[�Ȃ̂ŁA�`�����l������3�ɕύX
	my_nChannel = 3;

}

//------------------------------------------
// �f�X�g���N�^
//------------------------------------------
IportManager2pColorCorr::~IportManager2pColorCorr(void)
{
}


//------------------------------------------
// ������ �� �����ŉ摜���L���[�C���O�������ꍇ�Ɏg������
// DWORD nFrameW �t���[���T�C�Y (2���𑫂����킹���T�C�Y)
// DWORD nFrameH �t���[���T�C�Y
// DWORD camset ��������
// DWORD angle �p�x��
// BYTE nDummyVal �_�~�[�摜�̐F
//------------------------------------------
bool IportManager2pColorCorr::Init(DWORD nFrameW, DWORD nFrameH, DWORD camset, DWORD angle, BYTE nDummyVal)
{
	//// �g����
	my_mat_corr = new float**[2];		// double buffered�̍l����
    for(int ii=0; ii<2; ii++)
    {
        my_mat_corr[ii] = new float*[ MAX_IPORT_CAMSET ];
        for(int jj=0; jj<MAX_IPORT_CAMSET; jj++)
        {
			my_mat_corr[ii][jj] = new float[ IportLib::NCOEF ];
        }
    }
	ResetCorrMat();


	//// ��{�N���X�����s
	return __super::Init(nFrameW, nFrameH, camset, angle, nDummyVal);
}

//------------------------------------------
// ���
//------------------------------------------
void IportManager2pColorCorr::Dispose()
{
	//// �g����
    if( NULL != my_mat_corr )
    {
        for(int i=0; i<2; i++)
        {
            for(int j=0; j<MAX_IPORT_CAMSET; j++)
            {
                delete[] my_mat_corr[i][j];
            }
            delete[] my_mat_corr[i];
        }
        delete[] my_mat_corr;
        my_mat_corr = NULL;
    }

	//// ��{�N���X�����s
	return __super::Dispose();
}


//------------------------------------------
// 1�J�����Z�b�g���̉摜���擾���āA�X���b�h���[�v�L���[�ɃL���[�C���O
// void** pImgs �摜(�J�����p�x��)
// long* nX �摜�T�C�Y (1�R����)
// long* nY �摜�T�C�Y (1�R����)
// DWORD nFrameNum �t���[��No (1�R����)
// DWORD nCamset �J�����Z�b�g�P�ʂ̉���(0�I���W��)
// WORD* nSeqNum iPort�V�[�P���X�ԍ�
// __int64 nFastTime �ŏ��̃J�����p�x���擾��������
//------------------------------------------
void IportManager2pColorCorr::OnFrameGrabbed(void** pImgs, long nX, long nY, DWORD nFrameNum, DWORD nCamset, WORD* nSeqNum, __int64 nFastTime)
{
	//// �󂫊���擾
	FRAME_DATA* p = myque_pFrame->GetFree(0);				// �Z�}�t�H�[�҂��o�[�W����(�����Ɍ��ʂ�Ԃ�����)
	if( NULL == p ) {
		// ��󂫂Ȃ� (�񕜉\�ȃG���[)
		LOG(em_ERR), "[%s] GRAB�o�b�t�@�I�[�o����(set=%d, No=%d):�X���b�h���[�v�L���[�󂫖���!", my_sThreadName, nCamset, nFrameNum);
		OnAddInfo(IportLib::DIV_ID_ERROR_NO_IMG_QUEFULL, nFrameNum, nCamset, 0, 0 );
		return;
	}

	//// �摜���i�[
	// ���Ԃ��Z�b�g
	p->cls_Time.PutTimestamp(0, nFastTime);			// ���O�F�ŏ��̃J�����p�x���擾��������

	// �f�[�^�R�s�[
	p->nCamset	= nCamset;
	p->nLen		= (nX*2) * nY;	// 1�R����
	p->nNo		= nFrameNum;

	for(DWORD ii=0; ii<my_nAngleNum; ii++) {
		//p->nSeqNum[ii] = nSeqNum[ii];
		p->nSeqNum[ii*2+0] = nSeqNum[ii];
		p->nSeqNum[ii*2+1] = nSeqNum[ii+1];	// �����Ȃ�b��
		p->hInfo[ii]   = my_hAddInfo[GetCamIndex(nCamset, ii)];


		if( ! my_bImgTurn_LR ) {
			// �J���[�摜��RGB�摜�{���F����
#ifndef IPORTLIB_IMG_CORR_MMX
			IportLib::Copy_PackedColor_Corr_2p(&p->pImgs[ii*my_nImgSize*my_nChannel], &pImgs[2*ii], 
				my_nFrameW, nX*my_nChannel, my_nFrameW, my_nFrameH,
				my_mat_corr[my_mat_fore][nCamset] );
#else
			IportLib::Copy_PackedColor_Corr_2p_MMX(&p->pImgs[ii*my_nImgSize*my_nChannel], &pImgs[2*ii], 
				my_nFrameW, nX*my_nChannel, my_nFrameW, my_nFrameH,
				my_mat_corr[my_mat_fore][nCamset] );
#endif
		} else {
			_ASSERT( false );	// ���Ή�
		}
	}

	p->cls_Time.PutTimestamp(1);					// ���P�F�S���̃J�����p�x������������ (=��)

	// �o�^
	myque_pFrame->AddPool(p); 
}

//------------------------------------------
// ���F�s��l�Z�b�g
// DWORD nCamset �J�����Z�b�g�P�ʂ̉���(0�I���W��)
// const float* mat ���F�s��l
//------------------------------------------
void IportManager2pColorCorr::SetCorrMat(int camset, const float* mat)
{
    const int mat_back = 1 - my_mat_fore;
    memcpy(my_mat_corr[mat_back][camset], mat, sizeof(float) * IportLib::NCOEF);
    my_mat_fore = mat_back;

/*//	
LOG(em_INF), "[%s] cam=%d,%d (R:%.2f,%.2f,%.2f,%.2f) (G:%.2f,%.2f,%.2f,%.2f) (B:%.2f,%.2f,%.2f,%.2f)", my_sThreadName, camset, mat_back,
my_mat_corr[mat_back][camset][0],my_mat_corr[mat_back][camset][1],my_mat_corr[mat_back][camset][2],my_mat_corr[mat_back][camset][3],
my_mat_corr[mat_back][camset][4],my_mat_corr[mat_back][camset][5],my_mat_corr[mat_back][camset][6],my_mat_corr[mat_back][camset][7],
my_mat_corr[mat_back][camset][8],my_mat_corr[mat_back][camset][9],my_mat_corr[mat_back][camset][10],my_mat_corr[mat_back][camset][11]);
//*/
}

//------------------------------------------
// ���F�s��l���Z�b�g
//------------------------------------------
void IportManager2pColorCorr::ResetCorrMat()
{
    for(int ii=0; ii<2; ii++)
    {
        for(int jj=0; jj<MAX_IPORT_CAMSET; jj++)
        {
            my_mat_corr[ii][jj][0]  = 1;
            my_mat_corr[ii][jj][1]  = 0;
            my_mat_corr[ii][jj][2]  = 0;
            my_mat_corr[ii][jj][3]  = 0;
            //
            my_mat_corr[ii][jj][4]  = 0;
            my_mat_corr[ii][jj][5]  = 1;
            my_mat_corr[ii][jj][6]  = 0;
            my_mat_corr[ii][jj][7]  = 0;
            //
            my_mat_corr[ii][jj][8]  = 0;
            my_mat_corr[ii][jj][9]  = 0;
            my_mat_corr[ii][jj][10] = 1;
            my_mat_corr[ii][jj][11] = 0;
        }
    }

}
