#include "StdAfx.h"
#include "IportManager2pColor.h"

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
IportManager2pColor::IportManager2pColor(CString sMyManagerName, CString sMyIport) :
// 1.���O
IportManager2p(sMyManagerName, sMyIport)
{
	// �J���[�Ȃ̂ŁA�`�����l������3�ɕύX
	my_nChannel = 3;
}

//------------------------------------------
// �f�X�g���N�^
//------------------------------------------
IportManager2pColor::~IportManager2pColor(void)
{
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
void IportManager2pColor::OnFrameGrabbed(void** pImgs, long nX, long nY, DWORD nFrameNum, DWORD nCamset, WORD* nSeqNum, __int64 nFastTime)
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
			// �J���[�摜��RGB�摜�ɕ���
			//Copy_PackedColor8(&p->pImgs[ii*my_nImgSize*my_nChannel], &pImgs[2*ii], nX*my_nChannel);
			IportLib::Copy_PackedColor8_2p(&p->pImgs[ii*my_nImgSize*my_nChannel], &pImgs[2*ii], 
				my_nFrameW, nX*my_nChannel, my_nFrameW, my_nFrameH);
		} else {
			_ASSERT( false );	// ���Ή�
		}
	}

	p->cls_Time.PutTimestamp(1);					// ���P�F�S���̃J�����p�x������������ (=��)

	// �o�^
	myque_pFrame->AddPool(p); 
}

/*
//------------------------------------------
// �J���[�摜��RGB�摜�ɕ���
// BYTE* dstImg		RGB�����摜 (��)
// void** pImgsrc	�J���[�摜	(��)
// int pitch_src	iPort������̕�(��f���l��)
//------------------------------------------
void IportManager2pColor::Copy_PackedColor8(BYTE* dstImg, void** pImgsrc, int pitch_src)
{
    const long width  = my_nFrameW;
    const long height = my_nFrameH;
    const long pitch  = my_nFrameW;

    const BYTE*const psrc0 = (BYTE*)pImgsrc[0];
    const BYTE*const psrc1 = (BYTE*)pImgsrc[1];

    BYTE*const pdstR = &dstImg[my_nImgSize*IPORT_COLOR_R];
    BYTE*const pdstG = &dstImg[my_nImgSize*IPORT_COLOR_G];
    BYTE*const pdstB = &dstImg[my_nImgSize*IPORT_COLOR_B];

    const long sz_pack = 3; // RGB

    for(int y=0; y<height; y++)
    {
        const long src_yoffset = y * pitch_src;
        const long dst_yoffset = y * pitch;
        for(int x=0; x<width; x+=2)
        {
			int srcoffset = src_yoffset + sz_pack * x/2;

			// ������f						// ������
            pdstG[dst_yoffset + x]     = psrc0[srcoffset + 1];	//G
            pdstB[dst_yoffset + x]     = psrc0[srcoffset    ];	//B
            pdstR[dst_yoffset + x]     = psrc0[srcoffset + 2];	//R
			// ���f						// �E����
            pdstG[dst_yoffset + x + 1] = psrc1[srcoffset + 1];	//G
            pdstB[dst_yoffset + x + 1] = psrc1[srcoffset    ];	//B
            pdstR[dst_yoffset + x + 1] = psrc1[srcoffset + 2];	//R
        }
    }
}
*/


