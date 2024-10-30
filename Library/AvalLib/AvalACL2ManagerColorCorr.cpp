#include "StdAfx.h"
#include "AvalACL2ManagerColorCorr.h"

#include "..\IportLib\make_planar_color_corr.h"										// 画像の変換

using namespace AvalLib;


AvalACL2ManagerColorCorr::AvalACL2ManagerColorCorr(CString sMyManagerName, CString sMyIport)
:AvalACL2Manager(sMyManagerName,sMyIport)
,CGrabberCorrMatImpl( MAX_CAMNUM )
{
		my_nChannel = 3;
}

AvalACL2ManagerColorCorr::~AvalACL2ManagerColorCorr(void)
{
}

void AvalACL2ManagerColorCorr::OnFrameGrabbed(void** pImgs, DWORD nFrameNum, int* nSeqNum, __int64 nFastTime)
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
	p->cls_Time.PutTimestamp(0);			// ＃０：最初のカメラ角度を取得した時間


	// データコピー
	const long nImageSize = this->GetFrameWidth()*this->GetFrameHeight();
	p->nCamset	= 0;
	p->nLen		= nImageSize;		// = m_nImageSize と同義のはず
	p->nNo		= nFrameNum;


	for(int ii=0; ii<my_nGrabber; ii++) {
		p->nSeqNum[ii] = (WORD)nSeqNum[ii];		//iportSQNOとの互換性をとるため、16bitにする
		p->hInfo[ii]   = my_hAddInfo[ii];
	
		if( ! my_bImgTurn_LR ) {
/*
*			IportLib::Copy_PackedColor_Corr_1p(&p->pImgs[ii*my_nImgSize*my_nChannel], &pImgs[ii], 
*					my_nFrameW, my_nFrameW*my_nChannel, my_nFrameW, my_nFrameH,
*					GetCorrCoeff(ii) );
*			IportLib::Copy_PackedColor8_1p(&p->pImgs[ii*my_nImgSize*my_nChannel], &pImgs[ii], 
*				my_nFrameW, my_nFrameW*my_nChannel, my_nFrameW, my_nFrameH);
*/

#ifndef IPORTLIB_IMG_CORR_MMX
			IportLib::Copy_PackedColor_Corr_1p(&p->pImgs[ii*my_nImgSize*my_nChannel], &pImgs[ii], 
				my_nFrameW, my_nFrameW*my_nChannel, my_nFrameW, my_nFrameH,
				GetCorrCoeff(ii));
#else
			IportLib::Copy_PackedColor_Corr_1p_MMX(&p->pImgs[ii*my_nImgSize*my_nChannel], &pImgs[ii], 
				my_nFrameW, my_nFrameW*my_nChannel, my_nFrameW, my_nFrameH,
				GetCorrCoeff(ii));
#endif
            
		} else {
			_ASSERT( false );	// 未対応
		}
	}

	p->cls_Time.PutTimestamp(1);					// ＃１：全部のカメラ角度が揃った時間 (=今)

	double ms = p->cls_Time.GetTimestamp(1,  0 );
	if( ms > 150 ){
		static int wk[8];
		static int i;
		wk[i] = (int)ms;
		i = (i+1)%8;
	}
	// 登録
	myque_pFrame->AddPool(p); 
}