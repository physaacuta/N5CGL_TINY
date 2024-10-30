#include "StdAfx.h"
#include "AvalACL2Manager12.h"

#include "Gamma12_8.h"

using namespace AvalLib;


AvalACL2Manager12::AvalACL2Manager12(CString sMyManagerName, CString sMyIport)
:AvalACL2Manager(sMyManagerName,sMyIport){
	m_gamma = new Gamma_12_8();
}

AvalACL2Manager12::~AvalACL2Manager12(void)
{
}
void AvalACL2Manager12::SetGamma(double v)
{
	m_gamma->Set(v);
}

void		AvalACL2Manager12::SetParam( const char *id, const void *param ) 
{
	if( strcmp(id, "GAMMA") == 0 ){
		double gamma = *static_cast<const double*>(param);
		LOG(em_INF), "[%s] �K���}�l�ݒ� : %g", my_sThreadName, gamma);
		SetGamma( gamma );
	}

}

void AvalACL2Manager12::OnFrameGrabbed(void** pImgs, DWORD nFrameNum, int* nSeqNum, __int64 nFastTime)
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

		m_gamma->Exec((WORD*)pImgs[ii], &p->pImgs[0], my_nFrameW, my_nFrameH);
	}

	p->cls_Time.PutTimestamp(1);					// ���P�F�S���̃J�����p�x������������ (=��)


	// �o�^
	myque_pFrame->AddPool(p); 
}