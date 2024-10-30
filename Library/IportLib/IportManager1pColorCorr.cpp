#include "StdAfx.h"
#include "IportManager1pColorCorr.h"

// メモリーリーク検出 ----->>>
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif
// <<<------------------------

using namespace IportLib;


////===================================================================
//// マクロ
////===================================================================
//#define LOM(t) mcls_LogMe.Write(KizuLib::LogFileManager::t			// ローカル専用ログ



//------------------------------------------
// コンストラクタ
// CString sMyManagerName 自クラスの名称
// CString sMyIport iPortクラスの名称
//------------------------------------------
IportManager1pColorCorr::IportManager1pColorCorr(CString sMyManagerName, CString sMyIport) :
// 1.名前
IportManager1p(sMyManagerName, sMyIport)
{
	// カラーなので、チャンネル数を3に変更
	my_nChannel = 3;
}

//------------------------------------------
// デストラクタ
//------------------------------------------
IportManager1pColorCorr::~IportManager1pColorCorr(void)
{
}


//------------------------------------------
// 初期化 ※ 自分で画像をキューイングしたい場合に使うかな
// DWORD nFrameW フレームサイズ (2口を足し合わせたサイズ)
// DWORD nFrameH フレームサイズ
// DWORD camset 幅方向数
// DWORD angle 角度数
// BYTE nDummyVal ダミー画像の色
//------------------------------------------
bool IportManager1pColorCorr::Init(DWORD nFrameW, DWORD nFrameH, DWORD camset, DWORD angle, BYTE nDummyVal)
{
	//// 拡張部
	my_mat_corr = new float**[2];		// double bufferedの考え方
    for(int ii=0; ii<2; ii++)
    {
        my_mat_corr[ii] = new float*[ MAX_IPORT_CAMSET ];
        for(int jj=0; jj<MAX_IPORT_CAMSET; jj++)
        {
			my_mat_corr[ii][jj] = new float[ IportLib::NCOEF ];
        }
    }
	ResetCorrMat();


	//// 基本クラスを実行
	return __super::Init(nFrameW, nFrameH, camset, angle, nDummyVal);
}

//------------------------------------------
// 解放
//------------------------------------------
void IportManager1pColorCorr::Dispose()
{
	//// 拡張部
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

	//// 基本クラスを実行
	return __super::Dispose();
}

//------------------------------------------
// 1カメラセット分の画像を取得して、スレッドループキューにキューイング
// void** pImgs 画像(カメラ角度分)
// long nX 画像サイズ (1コだけ)
// long nY 画像サイズ (1コだけ)
// DWORD nFrameNum フレームNo (1コだけ)
// DWORD nCamset カメラセット単位の何個目(0オリジン)
// WORD* nSeqNum iPortシーケンス番号
// __int64 nFastTime 最初のカメラ角度を取得した時間
//------------------------------------------
void IportManager1pColorCorr::OnFrameGrabbed(void** pImgs, long nX, long nY, DWORD nFrameNum, DWORD nCamset, WORD* nSeqNum, __int64 nFastTime)
{
	//// 空き器を取得
	FRAME_DATA* p = myque_pFrame->GetFree(0);				// セマフォー待ちバージョン(すぐに結果を返したい)
	if( NULL == p ) {
		// 器空きなし (回復可能なエラー)
		LOG(em_ERR), "[%s] GRABバッファオーバラン(set=%d, No=%d):スレッドループキュー空き無し!", my_sThreadName, nCamset, nFrameNum);
		OnAddInfo(IportLib::DIV_ID_ERROR_NO_IMG_QUEFULL, nFrameNum, nCamset, 0, 0 );
		return;
	}


	//// 画像を格納
	// 時間をセット
	p->cls_Time.PutTimestamp(0, nFastTime);			// ＃０：最初のカメラ角度を取得した時間

	// データコピー
	p->nCamset	= nCamset;
	p->nLen		= nX * nY;
	p->nNo		= nFrameNum;

	for(DWORD ii=0; ii<my_nAngleNum; ii++) {
		p->nSeqNum[ii] = nSeqNum[ii];
		p->hInfo[ii]   = my_hAddInfo[GetCamIndex(nCamset, ii)];

		if( ! my_bImgTurn_LR ) {
			// カラー画像をRGB画像＋混色分離
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
			_ASSERT( false );	// 未対応
		}
	}

	p->cls_Time.PutTimestamp(1);					// ＃１：全部のカメラ角度が揃った時間 (=今)

	// 登録
	myque_pFrame->AddPool(p); 
//	myque_pFrame->AddFree(p);  // テスト
}

//------------------------------------------
// 混色行列値セット
// DWORD nCamset カメラセット単位の何個目(0オリジン)
// const float* mat 混色行列値
//------------------------------------------
void IportManager1pColorCorr::SetCorrMat(int camset, const float* mat)
{
    const int mat_back = 1 - my_mat_fore;
    memcpy(my_mat_corr[mat_back][camset], mat, sizeof(float) * IportLib::NCOEF);
    my_mat_fore = mat_back;
}
//------------------------------------------
// 混色行列値リセット
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
