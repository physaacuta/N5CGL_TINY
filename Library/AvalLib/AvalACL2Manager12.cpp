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
		LOG(em_INF), "[%s] ガンマ値設定 : %g", my_sThreadName, gamma);
		SetGamma( gamma );
	}

}

void AvalACL2Manager12::OnFrameGrabbed(void** pImgs, DWORD nFrameNum, int* nSeqNum, __int64 nFastTime)
{
	//// 空き器を取得
	FRAME_DATA* p = myque_pFrame->GetFree(0);				// セマフォー待ちバージョン(すぐに結果を返したい)
	if( NULL == p ) {
		// 器空きなし (回復可能なエラー)
		LOG(em_ERR), "[%s] GRABバッファオーバラン(SNo=%d, FNo=%d):スレッドループキュー空き無し!", my_sThreadName, nSeqNum[0], nFrameNum);
		OnAddInfo(IportLib::DIV_ID_ERROR_NO_IMG_QUEFULL, nFrameNum, nSeqNum[0], 0, 0 );
		return;
	}


	//// 画像を格納
	p->cls_Time.PutTimestamp(0, nFastTime);			// ＃０：最初のカメラ角度を取得した時間


	// データコピー
	const long nImageSize = this->GetFrameWidth()*this->GetFrameHeight();
	p->nCamset	= 0;
	p->nLen		= nImageSize;
	p->nNo		= nFrameNum;


	for(int ii=0; ii<my_nGrabber; ii++) {
		p->nSeqNum[ii] = (WORD)nSeqNum[ii];		//iportSQNOとの互換性をとるため、16bitにする
		p->hInfo[ii]   = my_hAddInfo[ii];

		m_gamma->Exec((WORD*)pImgs[ii], &p->pImgs[0], my_nFrameW, my_nFrameH);
	}

	p->cls_Time.PutTimestamp(1);					// ＃１：全部のカメラ角度が揃った時間 (=今)


	// 登録
	myque_pFrame->AddPool(p); 
}