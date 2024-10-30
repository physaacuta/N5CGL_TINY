#include "StdAfx.h"
#include "IportManager1pColorCorr.h"

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
IportManager1pColorCorr::IportManager1pColorCorr(CString sMyManagerName, CString sMyIport) :
// 1.���O
IportManager1p(sMyManagerName, sMyIport)
{
	// �J���[�Ȃ̂ŁA�`�����l������3�ɕύX
	my_nChannel = 3;
}

//------------------------------------------
// �f�X�g���N�^
//------------------------------------------
IportManager1pColorCorr::~IportManager1pColorCorr(void)
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
bool IportManager1pColorCorr::Init(DWORD nFrameW, DWORD nFrameH, DWORD camset, DWORD angle, BYTE nDummyVal)
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
void IportManager1pColorCorr::Dispose()
{
	//// �g����
    if( NULL != my_mat_corr )
    {
        for(int i=0; i<2; i++)
        {
            for(int j=0; j<my_nCamsetNum; j++)
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
// long nX �摜�T�C�Y (1�R����)
// long nY �摜�T�C�Y (1�R����)
// DWORD nFrameNum �t���[��No (1�R����)
// DWORD nCamset �J�����Z�b�g�P�ʂ̉���(0�I���W��)
// WORD* nSeqNum iPort�V�[�P���X�ԍ�
// __int64 nFastTime �ŏ��̃J�����p�x���擾��������
//------------------------------------------
void IportManager1pColorCorr::OnFrameGrabbed(void** pImgs, long nX, long nY, DWORD nFrameNum, DWORD nCamset, WORD* nSeqNum, __int64 nFastTime)
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
	p->nLen		= nX * nY;
	p->nNo		= nFrameNum;

	for(DWORD ii=0; ii<my_nAngleNum; ii++) {
		p->nSeqNum[ii] = nSeqNum[ii];
		p->hInfo[ii]   = my_hAddInfo[GetCamIndex(nCamset, ii)];

		if( ! my_bImgTurn_LR ) {
			// �J���[�摜��RGB�摜�{���F����
#ifndef IPORTLIB_IMG_CORR_MMX
			IportLib::Copy_PackedColor_Corr_1p(&p->pImgs[ii*my_nImgSize*my_nChannel], &pImgs[ii], 
				my_nFrameW, my_nFrameW*my_nChannel, my_nFrameW, my_nFrameH,
				my_mat_corr[my_mat_fore][nCamset] );
#else
			IportLib::Copy_PackedColor_Corr_1p_MMX(&p->pImgs[ii*my_nImgSize*my_nChannel], &pImgs[ii], 
				my_nFrameW, my_nFrameW*my_nChannel, my_nFrameW, my_nFrameH,
				my_mat_corr[my_mat_fore][nCamset] );
#endif

		} else {
			_ASSERT( false );	// ���Ή�
		}
	}

	p->cls_Time.PutTimestamp(1);					// ���P�F�S���̃J�����p�x������������ (=��)

	// �o�^
	myque_pFrame->AddPool(p); 
//	myque_pFrame->AddFree(p);  // �e�X�g
}

//------------------------------------------
// ���F�s��l�Z�b�g
// DWORD nCamset �J�����Z�b�g�P�ʂ̉���(0�I���W��)
// const float* mat ���F�s��l
//------------------------------------------
void IportManager1pColorCorr::SetCorrMat(int camset, const float* mat)
{
    const int mat_back = 1 - my_mat_fore;
    memcpy(my_mat_corr[mat_back][camset], mat, sizeof(float) * IportLib::NCOEF);
    my_mat_fore = mat_back;
}
//------------------------------------------
// ���F�s��l���Z�b�g
//------------------------------------------
void IportManager1pColorCorr::ResetCorrMat()
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
