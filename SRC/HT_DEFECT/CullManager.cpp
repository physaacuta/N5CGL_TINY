#include "StdAfx.h"
#include "CullManager.h"

#include "..\..\Library\KizuLib\Class\KizuBitmapManager.h"	// デバック用

// メモリーリーク検出 ----->>>
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif
// <<<------------------------

//------------------------------------------
// コンストラクタ
//------------------------------------------
CullManager::CullManager(void) :
ThreadManager("CullMgr")
{
	memset(g_nTime_Rpi, 0x00, sizeof(g_nTime_Rpi));

	// 時間
	mcls_Time.InitAve();

}

//------------------------------------------
// デストラクタ
//------------------------------------------
CullManager::~CullManager(void)
{
}


//------------------------------------------
// スレッド開始前処理 (スレッド実行直後(シグナルセット前)にコールバック)
//------------------------------------------
int CullManager::ThreadFirst()
{
	//// 事前準備

	//// イベント設定
	HANDLE hArray[] = { gque_Deli.g_evSem 
						};
	SetEventNum(sizeof(hArray)/sizeof(HANDLE), hArray);
	return 0;
}

//------------------------------------------
// スレッド終了前処理 (終了シグナル後にコールバック)
//------------------------------------------
int CullManager::ThreadLast()
{
	// 定周期停止
	return 0;
}

//------------------------------------------
// スレッドイベント発生
// int nEventNo イベントNo (0オリジン)
//------------------------------------------
void CullManager::ThreadEvent(int nEventNo)
{
	////// シグナルの条件
	enum {	EV_QUE = 0,				// QUE依頼
	};

	////// シグナル条件分け
	switch (nEventNo) {

//-----------------------------------------------------------------------------------------------
	case EV_QUE: 					// QUE依頼
		{
			DELI* pDeli = gque_Deli.GetFromHead();
			Exec(pDeli);
			_aligned_free(pDeli);
			//delete pDeli;
		}
		return;

	
//-----------------------------------------------------------------------------------------------
	default :
		// ありえない！！
		LOG(em_ERR), "[%s] ThreadEvent EventNo NG=%d", my_sThreadName, nEventNo );
		return;
	}
}

//------------------------------------------
// 処理
//------------------------------------------
void CullManager::Exec(DELI* pDeli)
{
	ParamManager::P_COMMON_MAST const& prm = mcls_pPrmMgr->GetCommonMast();

	// 生画像を全判定から送るため、従来方式では間に合わない。画像処理で実施しているような回転寿司形式とする
	if ( NULL == mcls_pSockRpi || ! mcls_pSockRpi->IsConnect()) return;


	//// 時間計測
	mcls_Time.CounterStart();

	mcls_Time.PutTimestamp(MC_TIME_RPI_S);


	// キュー取り出し
	HT_RPI_FDATA* pRpiH = NULL;
	int nCnt = 3;												// リトライ回数。あまり多いと弊害が多いので、１フレーム間隔内位で
	do {
		pRpiH = (HT_RPI_FDATA*)mque_pSockRpi->GetFree(0);		// セマフォー待ちバージョン(すぐに結果を返したい)
		if (NULL != pRpiH) break;

		LOG(em_WAR), "[%s] フレーム送信バッファキューフル <%d> [%d]", my_sThreadName, pDeli->nCamId, pDeli->nFrameNo);
		Sleep(30);
		nCnt--;
	} while (nCnt);


	// データセット
	if (NULL == pRpiH) {
		LOG(em_ERR), "[%s] フレーム送信バッファキューフル <%d> [%d]", my_sThreadName, pDeli->nCamId, pDeli->nFrameNo );
		syslog(SYSNO_QUEFULL_WAR, "フレーム送信 <%d> [%d]", pDeli->nCamId, pDeli->nFrameNo);
	} else {
	
		//------------------------------------
		//// ベースヘッダー部生成
		//		使いまわしキューなので、使わなくても必ずデータセットすること
		memcpy( pRpiH->base_head.code, T_CODE_DEFECT, sizeof(pRpiH->base_head.code) );
		memcpy( pRpiH->base_head.pname, getmypcname(), sizeof(pRpiH->base_head.pname) );
		pRpiH->base_head.subinf = (short)(pDeli->nCamId) % NUM_CAM_POS;;

		//// データセット
		pRpiH->nCamId = pDeli->nCamId;
		pRpiH->nFrameNo = pDeli->nFrameNo;
		// エッジ
		for(int ii=0; ii<2; ii++) {
			pRpiH->dEdgePos[ii] = pDeli->dEdgePos[ii];
		}			
		// 画像処理エンジンからの画像出力サイズのまま
		//pRpiH->nWidth = SIZE_FRAME_IMG_X / prm.nImgCullX;
		//pRpiH->nHeight= SIZE_FRAME_IMG_Y / prm.nImgCullY;
		//pRpiH->nWidth = SIZE_FRAME_IMG_X / prm.nImgCullX;		// 変換後サイズ
		//pRpiH->nHeight= SIZE_GRAB_IMG_Y / prm.nImgCullY;
		pRpiH->nWidth = pDeli->nStride;							// 画像処理エンジンからの出力サイズ
		pRpiH->nHeight = pDeli->nHeight;
		pRpiH->nImgCnt = 0;


		//------------------------------------
		// 共有メモリ部データセット
		BYTE* pImg = (BYTE*)pRpiH->base_head.addr[0];		// 画像ポインタ
		int nSize = pRpiH->nWidth * pRpiH->nHeight;
		int nImgSetSize = 0;								// フレーム画像セットサイズ
		int nImgFromOffset = 0;								// 元バッファオフセット

		// 画像処理エンジン側にて間引きを実施するため、そのまま画像データコピー
		//double dXF = pRpiH->nWidth / (double)SIZE_GRAB_IMG_X;			// 縮尺率
		//double dYF = 1.0 / (double)prm.nImgCullY;


		for(int ii=0; ii<MAX_IMAGE_CNT; ii++) {
			if( FALSE == prm.nImgSave[ii] ) {
				pRpiH->nImgSize[ii] = 0;
				pRpiH->nImgOffset[ii] = nImgSetSize;
				continue;
			}

			//----------------------
			// 画像生成
			if( 0 == pDeli->nFrameSkip ) {
		
				// 画像処理エンジン側にて間引きを実施するため、そのまま画像データコピー
				//////ResizeRaw( &pDeli->pImg[nImgFromOffset],
				//////	SIZE_FRAME_IMG_X, SIZE_FRAME_IMG_Y,			// 元画像サイズ
				//////	0, 0, SIZE_GRAB_IMG_X, SIZE_GRAB_IMG_Y,		// 元画像のROI
				//////	pRpiH->nWidth, pRpiH->nHeight,				// 変換後画像サイズ
				//////	dXF, dYF,									// 縮尺率
				//////	&pImg[nImgSetSize],
				//////	0);											// モード
				memcpy( &pImg[ nImgSetSize ], &pDeli->pImg[nImgFromOffset], nSize );
			} else {
				// ダミー画像
				memset( &pImg[ nImgSetSize ], 0x00, nSize );
				LOG(em_WAR), "[%s] <%d>[%d] Rpi生画像 ダミーフレーム送信", my_sThreadName, pRpiH->nCamId, pRpiH->nFrameNo);
			}

			// 後始末
			pRpiH->nImgSize[ii] = nSize;
			pRpiH->nImgOffset[ii] = nImgSetSize;
			nImgSetSize += nSize;
			nImgFromOffset += pDeli->nImgSize[ii];
			pRpiH->nImgCnt++;



			//// 検証
			//for(int jj=0; jj<200; jj++) {
			//	memset( &pImg[ pImg[ pRpiH->nImgOffset[ii] ]  + 2048*jj], 0xff, 2048);
			//}

//#ifdef SOT_IMG_OUTPUT
//// BMP
if(true) {
CString s;
//s.Format("to_raw_%d_%d.bmp", pRpiH->nFCNo, ii);
s.Format("to_raw_%d.bmp", ii);
KizuBitmapManager kb;
kb.DataToBMPfile_Gray(pRpiH->nWidth, pRpiH->nHeight, &pImg[ pRpiH->nImgOffset[ii] ], s);
}
//#endif
		}		

		////LOG(em_MSG), "[%s] 全長送信依頼 <cam=%d> [%d]", my_sThreadName,	pRpiH->nCamId, pRpiH->nFrameNo );
		LOGCULL(em_MSG), "[%s] 全長送信依頼 <cam=%d> [%d]", my_sThreadName,	pRpiH->nCamId, pRpiH->nFrameNo );



		//------------------------------------
		// 伝文送信

		//// 伝文長 (データ部もある場合は、共有メモリにセットされた分だけ加算する)
		pRpiH->base_head.len = sizeof(HT_RPI_FDATA) + nImgSetSize;
		pRpiH->base_head_addr.len_head = sizeof(HT_RPI_FDATA);
		pRpiH->base_head_addr.len_addr[0] = nImgSetSize;
		mque_pSockRpi->AddPool(pRpiH);								// 送信

		
	}

	mcls_Time.PutTimestamp(MC_TIME_RPI_E);		

	//// 時間
	mcls_Time.CounterEnd();

	// 時間関係
	g_nTime_Rpi[0]		= (int)mcls_Time.GetTimestamp(MC_TIME_RPI_E, MC_TIME_RPI_S); 
	g_nTime_Rpi[1]		= max(g_nTime_Rpi[0], g_nTime_Rpi[1]); 

}


// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 完全付帯機能

// IPP関数を使うため明示的にインクルード

#ifndef __IPPDEFS_H__
#include "ippdefs.h"
#endif
#ifndef __IPPCORE_H__
#include "ippcore.h"
#endif
#ifndef __IPPS_H__
#include "ipps.h"
#endif
#ifndef __IPPI_H__
#include "ippi.h"
#endif

//------------------------------------------
// RAW画像 間引き
// BYTE* const pSrcRaw	入力画像	
// long nSrcWidth		入力幅pixel
// long nSrcHeight		入力高pixel
// long nCullX			間引き率 X  2を指定したら、1024→512  になる
// long nCullY			
// BYTE* pDstRaw		出力画像
// int mode				画質モード 0:高速  1:高画質
//------------------------------------------
bool CullManager::ResizeRaw(BYTE* const pSrcRaw, long nSrcWidth, long nSrcHeight,
	long nCullX, long nCullY, BYTE* pDstRaw, int mode)
{
	long nDstWidth = nSrcWidth / nCullX;
	long nDstHeight = nSrcHeight / nCullY;

	return ResizeRaw(pSrcRaw, nSrcWidth, nSrcHeight, pDstRaw, nDstWidth, nDstHeight, mode);
}

bool CullManager::ResizeRaw(BYTE* const pSrcRaw, long nSrcWidth, long nSrcHeight,
	BYTE* pDstRaw, long nDstWidth, long nDstHeight, int mode)
{
	const IppiSize	src_size = { nSrcWidth, nSrcHeight };
	const IppiSize	dst_size = { nDstWidth, nDstHeight };
	const IppiRect	src_roi = { 0, 0, nSrcWidth, nSrcHeight };		// ROI=範囲
//	const IppiRect	dst_roi	 = { 0, 0, nDstWidth, nDstHeight};		// ROI=範囲

	const double xf = nDstWidth / (double)nSrcWidth;
	const double yf = nDstHeight / (double)nSrcHeight;

	// モード
	int interp;
	if (0 == mode) {
		// 高速
		interp = IPPI_INTER_LINEAR;
	}
	else {
		// 高画質
		interp = IPPI_INTER_SUPER;
	}
	IppStatus s = ippiResize_8u_C1R(pSrcRaw, src_size, nSrcWidth, src_roi,
		pDstRaw, nDstWidth, dst_size, xf, yf, interp);

	return (ippStsNoErr == s ? true : false);
}

//
// dXF 1/2 であれば 0.5 をセット
bool CullManager::ResizeRaw(BYTE* const pSrcRaw, 
	int nSrcWidth, int nSrcHeight,
	int nSrcRoiX, int nSrcRoiY, int nSrcRoiW, int nSrcRoiH,
	int nDstWidth, int nDstHeight,
	double dXF, double dYF,
	BYTE* pDstRaw, int mode)
{
	const IppiSize	src_size = { nSrcWidth, nSrcHeight };
	const IppiSize	dst_size = { nDstWidth, nDstHeight };
	const IppiRect	src_roi = { nSrcRoiX, nSrcRoiY, nSrcRoiW, nSrcRoiH };		// ROI=範囲

	const double xf = dXF;
	const double yf = dYF;

	// モード
	int interp;
	if (0 == mode) {
		// 高速
		interp = IPPI_INTER_LINEAR;
	}
	else {
		// 高画質
		interp = IPPI_INTER_SUPER;
	}
	IppStatus s = ippiResize_8u_C1R(pSrcRaw, src_size, nSrcWidth, src_roi,
		pDstRaw, nDstWidth, dst_size, xf, yf, interp);

	return (ippStsNoErr == s ? true : false);

}