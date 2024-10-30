#include "StdAfx.h"
#include "AvalACL2ManagerColor.h"

#include "..\IportLib\make_planar_color_corr.h"										// �摜�̕ϊ�


using namespace AvalLib;


AvalACL2ManagerColor::AvalACL2ManagerColor(CString sMyManagerName, CString sMyIport)
:AvalACL2Manager(sMyManagerName,sMyIport){
		my_nChannel = 3;
}

AvalACL2ManagerColor::~AvalACL2ManagerColor(void)
{
}

void AvalACL2ManagerColor::OnFrameGrabbed(void** pImgs, DWORD nFrameNum, int* nSeqNum, __int64 nFastTime)
{
	//// �󂫊���擾
	FRAME_DATA* p = myque_pFrame->GetFree(0);				// �Z�}�t�H�[�҂��o�[�W����(�����Ɍ��ʂ�Ԃ�����)
	if( NULL == p ) {
		// ��󂫂Ȃ� (�񕜉\�ȃG���[)
		LOG(em_ERR), "[%s] GRAB�o�b�t�@�I�[�o����(SNo=%d, FNo=%d):�X���b�h���[�v�L���[�󂫖���!", my_sThreadName, nSeqNum[0], nFrameNum);
		OnAddInfo(IportLib::DIV_ID_ERROR_NO_IMG_QUEFULL, nFrameNum, nSeqNum[0], 0, 0 );
		return;
	}


	//// �摜���i�[
	p->cls_Time.PutTimestamp(0, nFastTime);			// ���O�F�ŏ��̃J�����p�x���擾��������


	// �f�[�^�R�s�[
	const long nImageSize = this->GetFrameWidth()*this->GetFrameHeight();
	p->nCamset	= 0;
	p->nLen		= nImageSize;
	p->nNo		= nFrameNum;


	for(int ii=0; ii<my_nGrabber; ii++) {
		p->nSeqNum[ii] = (WORD)nSeqNum[ii];		//iportSQNO�Ƃ̌݊������Ƃ邽�߁A16bit�ɂ���
		p->hInfo[ii]   = my_hAddInfo[ii];

		if( ! my_bImgTurn_LR ) {
			IportLib::Copy_PackedColor8_1p(&p->pImgs[ii*my_nImgSize*my_nChannel], &pImgs[ii], 
				my_nFrameW, my_nFrameW*my_nChannel, my_nFrameW, my_nFrameH);
		} else {
			_ASSERT( false );	// ���Ή�
		}
	}

	p->cls_Time.PutTimestamp(1);					// ���P�F�S���̃J�����p�x������������ (=��)


	// �o�^
	myque_pFrame->AddPool(p); 
}