#include "StdAfx.h"
#include "DataSender.h"

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
DataSender::DataSender(void) :
mcls_pStatus(NULL),
mcls_pData(NULL),
mcls_pSock(NULL),
mcls_pCull(NULL),
m_bSendHls(false),
m_nJpegQuality(80),
m_bFrameSend(true),
ThreadManager("DataSender")
{
#ifdef SOT_TEST
	m_emKindHt = DIV_HANTEI_HT;
	m_nCamPair = 1;
#endif

	// 定周期タイマー
	m_evTimeHls = CreateWaitableTimer(NULL, FALSE, NULL);

	ClearFrameNo();

	// ログ
	mcls_pLogSock = new LogFileManager("SOCK", NULL, 50, 2);
	LOGSOCK(em_MSG), "[%s] 起動", my_sThreadName);
}

//------------------------------------------
// デストラクタ
//------------------------------------------
DataSender::~DataSender(void)
{
	// 定周期タイマー
	CancelWaitableTimer(m_evTimeHls);
	CloseHandle(m_evTimeHls);

	// ログ
	LOGSOCK(em_MSG), "[%s] 終了", my_sThreadName);
	if (NULL != mcls_pLogSock) {
		delete mcls_pLogSock;
		mcls_pLogSock = NULL;
	}
}

//------------------------------------------
// スレッド開始前処理 (スレッド実行直後(シグナルセット前)にコールバック)
//------------------------------------------
int DataSender::ThreadFirst()
{
	//// イベント設定
	for(int ii=0; ii<HT_CONNECT_NUM; ii++) {
		AddEventNum(1, &mcls_pSock[ii]->gque_SendBack.g_evSem );
	}
	AddEventNum(1, &mcls_pData->gque_Deli.g_evSem );
	AddEventNum(1, &m_evTimeHls);
	return 0;
}

//------------------------------------------
// スレッド終了前処理 (終了シグナル後にコールバック)
//------------------------------------------
int DataSender::ThreadLast()
{
	CancelWaitableTimer(m_evTimeHls);
	return 0;
}

//------------------------------------------
// スレッドイベント発生
// int nEventNo イベントNo (0オリジン)
//------------------------------------------
void DataSender::ThreadEvent(int nEventNo)
{
	////// シグナルの条件
	enum {	EV_NEW_DATA_SEND_BACK = 0,								// 上記のデータが統括に送信完了したよ通知
			EV_NEW_DATA = (EV_NEW_DATA_SEND_BACK + HT_CONNECT_NUM),	// フレーム情報が共有メモリにセットされたよ通知
			EV_HLS = EV_NEW_DATA + 1								// 定周期通知

	};

	//-----------------------------------------------------------------------------------------------
	if( EV_NEW_DATA_SEND_BACK <= nEventNo && EV_NEW_DATA > nEventNo) {		// 上記のデータが統括に送信完了したよ通知

		NewDataSendBack(nEventNo-EV_NEW_DATA_SEND_BACK); 

	//-----------------------------------------------------------------------------------------------
	} else if (EV_NEW_DATA == nEventNo) {									// フレーム情報が共有メモリにセットされたよ通知
		if(true) {
			BYTE * pNewData = mcls_pData->gque_Deli.GetFromHead();
			NewDataExec(pNewData);						// データパイプより 1フレーム情報を処理
			SAFE_DELETE_ARRAY(pNewData);
		}

	//-----------------------------------------------------------------------------------------------
	} else if (EV_HLS == nEventNo) {										// 定周期通知
		SendHls();

	} else {
		// ありえない！！
		LOG(em_ERR), "[%s] ThreadEvent EventNo NG=%d", my_sThreadName, nEventNo );
		return;
	}

}

//------------------------------------------
// FrameNo関係初期化
//------------------------------------------
void DataSender::ClearFrameNo()
{
	for(int ii=0; ii<MAX_HT1_CAMSET; ii++) {
		m_nFrameNo[ii]		= -1;
		m_nRecvFrameNo[ii]	= -1;
	}
	m_nDspFrameNo			= -1;
	m_nDspRecvFrameNo		= -1;

	// 時間
	mcls_Time.InitAve();
	mcls_TimeCycle.InitCycle();

	memset(g_nTime_All, 0x00, sizeof(g_nTime_All));
	memset(g_nTime_Rpi, 0x00, sizeof(g_nTime_Rpi));
	memset(g_nTime_To, 0x00, sizeof(g_nTime_To));
	memset(g_nTime_ToFrame, 0x00, sizeof(g_nTime_ToFrame));
}

//------------------------------------------
// ヘルシー
//------------------------------------------
void DataSender::StartHls()
{
	__int64 ts = 1000 * -10000;			// 1ミリ秒→100ナノ秒 (マイナス指定で、指定時間経過でシグナル) (第3引数は2回目以降の起動間隔)
	SetWaitableTimer(m_evTimeHls, (LARGE_INTEGER *)&ts, ST_SEND_INTERVAL, NULL, NULL, FALSE);

}
void DataSender::StopHls()
{
	CancelWaitableTimer(m_evTimeHls);
}

// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// メイン処理


//------------------------------------------
// データパイプより 1フレーム情報を処理 (これが判定のメイン機能)
// BYTE* pNewData パイプ送信データ
//------------------------------------------
void DataSender::NewDataExec(BYTE* pNewData)
{
	ParamManager::P_COMMON_MAST const& prm = mcls_pPrmMgr->GetCommonMast();

	//nc_smem_pipe_data* pPipe = (nc_smem_pipe_data*) pNewData;
	NCL_SMEM_PIPE_DATA* pPipe = (NCL_SMEM_PIPE_DATA*) pNewData;
	int nFrameSkip = 0;			// 0:以外  NG


	//// 時間計測
	mcls_Time.CounterStart();

	mcls_Time.PutTimestamp(MC_TIME_ALL_S);

	// 複数カメラセットの場合は、最初のカメラセットのときに時間計測
	int camset = mcls_pStatus->GetCamPairItemCamset( m_nCamPair, 0, m_emKindHt);
	if( pPipe->camid == camset ) {
		mcls_TimeCycle.CounterCycle();
	}

	// 全長インスタンスの何番目か特定
	int nRpiIdx = 0;
	for (int ii=0; ii<FRAME_HT_SEND_PORT; ii++) {
		int camset = mcls_pStatus->GetCamPairItemCamset( m_nCamPair, ii, m_emKindHt);
		if( pPipe->camid == camset ) {
			nRpiIdx = ii;
			break;
		}
	}


	////////////////////////////////////
	// 検査中以外は 処理しないので、オフセットの返却のみ
#ifndef SOT_TEST
	if( ! mcls_pStatus->IsRunning() ) {
		// オフセットを返すべきもののみ
		if (pPipe->smem_size > 0) {
			mcls_pData->SendBack(pPipe->smem_offset);
		}
		return;
	}
#endif	

	// ロジックパララン時は処理しないので、オフセットの返却のみ
	if (DIV_HANTEI_PR == m_emKindHt)
	{
		// オフセットを返すべきもののみ
		if (pPipe->smem_size > 0) {
			mcls_pData->SendBack(pPipe->smem_offset);
		}
		return;
	}

	////////////////////////////////////
	// 整合性チェク
	DWORD	frame_num = pPipe->frame_num;

	// オフセット値 異常
	if( 0 > pPipe->smem_size ) {
		LOG(em_ERR), "[%s] 共有メモリオフセット値エラー [fNo=%d camid=%d offset=%d size=%d]", my_sThreadName, frame_num, pPipe->camid, pPipe->smem_offset, pPipe->smem_size );
		syslog(109, "[fNo=%d camid=%d offset=%d size=%d]", frame_num, pPipe->camid, pPipe->smem_offset, pPipe->smem_size);
		nFrameSkip = 9;

	// 共有メモリ空き無し
	} else if( 0 == pPipe->smem_size && NCL_SMEMPIPE_SMEMFULL == pPipe->smem_offset  ) {
		LOG(em_ERR), "[%s] [NCL_SMEMPIPE_SMEMFULL] 共有メモリ空き無し [fNo=%d camid=%d offset=%d size=%d]", my_sThreadName, frame_num, pPipe->camid, pPipe->smem_offset, pPipe->smem_size );
		syslog(112, "[NCL_SMEMPIPE_SMEMFULL発生(共有メモリ空き無し):fNo=%d camid=%d offset=%d size=%d]", frame_num, pPipe->camid, pPipe->smem_offset, pPipe->smem_size );
		nFrameSkip = NCL_SMEMPIPE_SMEMFULL;


	// 不明
	} else if( 0 == pPipe->smem_size && 0 > pPipe->smem_offset) {
		LOG(em_ERR), "[%s] [NP_SMEMPIPE_???] パイプデータ不明 [fNo=%d camid=%d offset=%d size=%d]", my_sThreadName, frame_num, pPipe->camid, pPipe->smem_offset, pPipe->smem_size );
		syslog(110, "[パイプデータ不明:fNo=%d camid=%d offset=%d size=%d]", frame_num, pPipe->camid, pPipe->smem_offset, pPipe->smem_size );
		nFrameSkip = 1;
	}


	//// フレームNo整合性チェック
	int nIndex = pPipe->camid - mcls_pStatus->GetCamPairItemCamset(m_nCamPair, 0, m_emKindHt);
	if(m_nRecvFrameNo[nIndex] +1 != pPipe->frame_num) {
		LOG(em_ERR), "[%s] 画像処理タスク→判定 フレームNo受信抜け [camset=%d now=%d pre=%d]", my_sThreadName, pPipe->camid, pPipe->frame_num, m_nRecvFrameNo[nIndex]);
		syslog(113, "[画像処理タスク→判定 フレームNo受信抜け camset=%d now=%d pre=%d]", pPipe->camid, pPipe->frame_num, m_nRecvFrameNo[nIndex]);
	}
	m_nRecvFrameNo[nIndex] = pPipe->frame_num;
	m_nDspRecvFrameNo = pPipe->frame_num;

	



	////////////////////////////////////
	// 良く使うものを準備
	//nc_smem_frame_header* pSmemH = (nc_smem_frame_header*)(mcls_pData->GetMapPointer() + pPipe->offset);	// 共有メモリの先頭アドレス + オフセット値 = 本フレームでの共有メモリの位置
    NCL_SMEM_FRAME_HEADER* pSmemH = (NCL_SMEM_FRAME_HEADER*)(mcls_pData->GetMapPointer() + pPipe->smem_offset);	// 共有メモリの先頭アドレス + オフセット値 = 本フレームでの共有メモリの位置
	BYTE* pSmemAry = (BYTE*)pSmemH;					// 使いやすいように。このフレームの先頭ポインタ
	if( 0 == nFrameSkip ) {
		if( pSmemH->camid != pPipe->camid || pSmemH->frame_num != pPipe->frame_num ){
			LOG(em_ERR), "<SMEM-PIPE Mismatch>, cam %d:%d, frame %d:%d",
				pSmemH->camid , pPipe->camid , pSmemH->frame_num ,pPipe->frame_num);
		}
	}


	////////////////////////////////////
	// 代表エッジ位置 算出
	float	dEdgeAvePos[2];									// 平均エッジ位置。不明の場合 0。（カメラ座標系)mm単位
	float	dEdgeMaxPos[2];									// 最大エッジ位置。不明の場合 0。（カメラ座標系)mm単位
	float	dEdgeMinPos[2];									// 最小エッジ位置。不明の場合 0。（カメラ座標系)mm単位
	memset(dEdgeAvePos, 0x00, sizeof(dEdgeAvePos));
	if( 0 == nFrameSkip ) {
		float* pEdge = (float*)&pSmemAry[ pSmemH->edge_offset ];

		// 平均算出
		for(int jj=0; jj<2; jj++) {
			double dSum = 0;
			long nCnt = 0;
			for(int kk=0; kk<SIZE_FRAME_IMG_Y; kk++) {
				//int idx = (2*kk) + jj;
				int idx = (SIZE_FRAME_IMG_Y*jj) + kk;
//LOG(em_MSG), "[%d] v=%.1f", idx, pEdge[idx]);
				//if(2>=kk) LOG(em_MSG), "[%d] v=%.1f", idx, pEdge[idx]);
				dSum += pEdge[idx]; nCnt++;
			}

			// 平均算出
			if(0 == nCnt)	dEdgeAvePos[jj] = 0;
			else			dEdgeAvePos[jj] = (float)(dSum/nCnt);
//LOG(em_MSG), "EdgeAvePos[%d]=%.1f (%.1f/%d)", jj, dEdgeAvePos[jj], dSum, nCnt);
		}

		// 最大,最小算出
		if( true ) {
			long nCnt = 0;
			double dMax[2] = { 0, 0 };
			double dMin[2] = {COIL_WID_MAX, COIL_WID_MAX};
			for(int kk=0; kk<SIZE_FRAME_IMG_Y; kk++) {
				int idx1 = (SIZE_FRAME_IMG_Y*0) + kk;
				int idx2 = (SIZE_FRAME_IMG_Y*1) + kk;
				if (abs(dMax[0])+abs(dMax[1]) < abs(pEdge[idx1])+abs(pEdge[idx2])) {
					dMax[0] = pEdge[idx1];
					dMax[1] = pEdge[idx2];
				}
				if (abs(dMin[0])+ abs(dMin[1]) > abs(pEdge[idx1])+abs(pEdge[idx2])) {
					dMin[0] = pEdge[idx1];
					dMin[1] = pEdge[idx2];
				}
				nCnt++;
			}

			// 最大, 最小算出
			if(0 == nCnt) {
				for(int jj=0; jj<2; jj++) {
					dEdgeMaxPos[jj] = 0;
					dEdgeMinPos[jj] = 0;
				}
			}
			else {
				for(int jj=0; jj<2; jj++) {
					dEdgeMaxPos[jj] = (float)dMax[jj];
					dEdgeMinPos[jj] = (float)dMin[jj];
				}
			}
		}

	}


	////////////////////////////////////
	// 全長画像処理へ送信

//	if( NULL != mcls_pCull) {
	if(DIV_HANTEI_HT == m_emKindHt) {
		mcls_Time.PutTimestamp(MC_TIME_RPI_S);

		NCL_FRAME_IMAGE*	pF;	// 共有メモリの画像情報先頭位置
		CullManager::DELI* pDeli;

		if (0 == nFrameSkip && pSmemH->frame_offset >0) {
			pF = (NCL_FRAME_IMAGE*)&pSmemAry[pSmemH->frame_offset];	// 共有メモリの画像情報先頭位置

			// 生成サイズ決定
			int nDeliSize = sizeof(CullManager::DELI) - sizeof(BYTE);
			for (int ii = 0; ii<MAX_IMAGE_CNT; ii++) {
				if (TRUE == prm.nImgSave[ii]) {
					//nDeliSize += SIZE_FRAME_IMG_X*SIZE_FRAME_IMG_Y;
					nDeliSize += pF->stride * pF->height;
				}
			}

			// 生成
			BYTE* pWk = (BYTE*)_aligned_malloc(nDeliSize, 16);//new BYTE[nDeliSize];
			pDeli = (CullManager::DELI*)pWk;

			// セット
			pDeli->nFrameSkip = nFrameSkip;
			pDeli->nCamId = (pPipe->camid - 1) % NUM_CAM_POS;
			pDeli->nFrameNo = pPipe->frame_num;
			for (int ii = 0; ii<2; ii++) {
				pDeli->dEdgePos[ii] = dEdgeAvePos[ii];
			}

			pDeli->nWidth = pF->width;
			pDeli->nHeight = pF->height;
			pDeli->nStride = pF->stride;
			pDeli->nChannel = pF->channel;

			int nOffset = 0;
			for (int ii = 0; ii<MAX_IMAGE_CNT; ii++) {
				if (TRUE == prm.nImgSave[ii]) {
#if 0	// フレーム画像をエンジン間引き対応前
					pDeli->nImgSize[ii] = SIZE_FRAME_IMG_X*SIZE_FRAME_IMG_Y;
					//memcpy( &pDeli->pImg[nOffset], 
					//	&pSmemAry[(pSmemH->frame_offset + (SIZE_FRAME_IMG_X*SIZE_FRAME_IMG_Y*ii))],
					//	SIZE_FRAME_IMG_X*SIZE_FRAME_IMG_Y);
					KizuMMX::mem_copy_64(&pSmemAry[(pSmemH->frame_offset + (SIZE_FRAME_IMG_X*SIZE_FRAME_IMG_Y*ii))], SIZE_FRAME_IMG_X,
						&pDeli->pImg[nOffset], SIZE_FRAME_IMG_X,
						SIZE_FRAME_IMG_X, SIZE_FRAME_IMG_Y);
					//KizuMMX::mem_copy_64xmm(&pSmemAry[(pSmemH->frame_offset + (SIZE_FRAME_IMG_X*SIZE_FRAME_IMG_Y*ii))], SIZE_FRAME_IMG_X, 
					//	&pDeli->pImg[nOffset], SIZE_FRAME_IMG_X,
					//	SIZE_FRAME_IMG_X, SIZE_FRAME_IMG_Y);
					//KizuMMX::mem_copy_64xmm_su(&pDeli->pImg[nOffset], &pSmemAry[(pSmemH->frame_offset + (SIZE_FRAME_IMG_X*SIZE_FRAME_IMG_Y*ii))],
					//	SIZE_FRAME_IMG_X * SIZE_FRAME_IMG_Y);
					nOffset += SIZE_FRAME_IMG_X*SIZE_FRAME_IMG_Y;
#else
					pDeli->nImgSize[ii] = pF->stride * pF->height;
					//KizuMMX::mem_copy_64(&pF->image[pF->stride*pF->height*ii], pF->stride, &pDeli->pImg[nOffset], pF->stride, pF->stride, pF->height);
					KizuMMX::mem_copy_64xmm(&pF->image[pF->stride*pF->height*ii], pF->stride, &pDeli->pImg[nOffset], pF->stride, pF->stride, pF->height);
					nOffset += pF->stride * pF->height;
#endif
				}
				else {
					pDeli->nImgSize[ii] = 0;
				}
			}
		}
		else {																	// 生成サイズ決定
			int nDeliSize = sizeof(CullManager::DELI) - sizeof(BYTE);
			for (int ii = 0; ii<MAX_IMAGE_CNT; ii++) {
				if (TRUE == prm.nImgSave[ii]) {
					nDeliSize += (SIZE_FRAME_IMG_X / prm.nImgCullX)*(SIZE_FRAME_IMG_Y / prm.nImgCullY);
				}
			}

			// 生成
			BYTE* pWk = (BYTE*)_aligned_malloc(nDeliSize, 16);//new BYTE[nDeliSize];
			pDeli = (CullManager::DELI*)pWk;

			// セット
			pDeli->nFrameSkip = nFrameSkip;
			pDeli->nCamId = (pPipe->camid - 1) % NUM_CAM_POS;
			pDeli->nFrameNo = pPipe->frame_num;
			for (int ii = 0; ii<2; ii++) {
				pDeli->dEdgePos[ii] = dEdgeAvePos[ii];
			}

			pDeli->nWidth = SIZE_FRAME_IMG_X / prm.nImgCullX;
			pDeli->nHeight = SIZE_FRAME_IMG_Y / prm.nImgCullY;
			pDeli->nStride = SIZE_FRAME_IMG_X / prm.nImgCullX;
			pDeli->nChannel = MAX_IMAGE_CNT;

			int nOffset = 0;
			for (int ii = 0; ii<MAX_IMAGE_CNT; ii++) {
				if (TRUE == prm.nImgSave[ii]) {
					pDeli->nImgSize[ii] = pDeli->nStride * pDeli->nHeight;
					//KizuMMX::mem_copy_64xmm(&pF->image[pF->stride*pF->height*ii], pF->stride, &pDeli->pImg[nOffset], pF->stride, pF->stride, pF->height);
					nOffset += pDeli->nStride * pDeli->nHeight;
				}
				else {
					pDeli->nImgSize[ii] = 0;
				}
			}
		}


		// 依頼
		if( ! mcls_pCull[nRpiIdx]->gque_Deli.AddToTailFreeCheck(pDeli) ) {
			delete pDeli;
			LOG(em_ERR), "[%s] RAW間引き依頼 キューフル!!", my_sThreadName);
		}
		mcls_Time.PutTimestamp(MC_TIME_RPI_E);		
	}


/*
	//   生画像を全判定から送るため、従来方式では間に合わない。画像処理で実施しているような回転寿司形式とする
	if ( NULL != mcls_pSockRpi[nRpiIdx] && mcls_pSockRpi[nRpiIdx]->IsConnect()) {
		mcls_Time.PutTimestamp(MC_TIME_RPI_S);


		// キュー取り出し
		HT_RPI_FDATA* pRpiH = NULL;
		int nCnt = 3;												// リトライ回数。あまり多いと弊害が多いので、１フレーム間隔内位で
		do {
			pRpiH = (HT_RPI_FDATA*)mque_pSockRpi[nRpiIdx]->GetFree(0);		// セマフォー待ちバージョン(すぐに結果を返したい)
			if (NULL != pRpiH) break;

			LOG(em_WAR), "[%s] フレーム送信バッファキューフル [%d]", my_sThreadName, m_nRecvFrameNo[nIndex]);
			Sleep(30);
			nCnt--;
		} while (nCnt);

		// データセット
		if (NULL == pRpiH) {
			LOG(em_ERR), "[%s] フレーム送信バッファキューフル [%d]", my_sThreadName, m_nRecvFrameNo[nIndex] );
			syslog(SYSNO_QUEFULL_WAR, "フレーム送信 [%d]", m_nRecvFrameNo[nIndex]);
		} else {
		
			//------------------------------------
			//// ベースヘッダー部生成
			//		使いまわしキューなので、使わなくても必ずデータセットすること
			memcpy( pRpiH->base_head.code, T_CODE_DEFECT, sizeof(pRpiH->base_head.code) );
			memcpy( pRpiH->base_head.pname, getmypcname(), sizeof(pRpiH->base_head.pname) );
			pRpiH->base_head.subinf = 0;			
	
			//// データセット
			pRpiH->nCamId = (pPipe->camid-1) % NUM_CAM_POS;
			pRpiH->nFrameNo = pPipe->frame_num;
			// エッジ
			for(int ii=0; ii<2; ii++) {
				pRpiH->dEdgePos[ii] = dEdgeAvePos[ii];
			}			
			pRpiH->nWidth = SIZE_FRAME_IMG_X / prm.nImgCullX;
			pRpiH->nHeight= SIZE_FRAME_IMG_Y / prm.nImgCullY;
			pRpiH->nImgCnt = 0;
	

			//------------------------------------
			// 共有メモリ部データセット
			BYTE* pImg = (BYTE*)pRpiH->base_head.addr[0];		// 画像ポインタ
			int nSize = pRpiH->nWidth * pRpiH->nHeight;
			int nImgSetSize = 0;								// フレーム画像セットサイズ


			for(int ii=0; ii<MAX_IMAGE_CNT; ii++) {
				if( FALSE == prm.nImgSave[ii] ) {
					pRpiH->nImgSize[ii] = 0;
					pRpiH->nImgOffset[ii] = nImgSetSize;
					continue;
				}

				//----------------------
				// 画像生成
				if( 0 == nFrameSkip ) {
					if( 1 == prm.nImgCullX && 1 == prm.nImgCullY ) {
						// 間引き無し
						memcpy(&pImg[nImgSetSize],
							&pSmemAry[(pSmemH->frame_offset + (SIZE_FRAME_IMG_X*SIZE_FRAME_IMG_Y*ii))],
							nSize);
					} else {
						// 間引きあり
						ResizeRaw(&pSmemAry[(pSmemH->frame_offset + (SIZE_FRAME_IMG_X*SIZE_FRAME_IMG_Y*ii))],
							SIZE_FRAME_IMG_X, SIZE_FRAME_IMG_Y,
							prm.nImgCullX, prm.nImgCullY,
							&pImg[nImgSetSize],
							1);
					}
				} else {
					// ダミー画像
					memset( &pImg[ nImgSetSize ], 0x00, nSize );
					LOG(em_WAR), "[%s] [%d] Rpi生画像 ダミーフレーム送信", my_sThreadName, pRpiH->nFrameNo);
				}

				// 後始末
				pRpiH->nImgSize[ii] = nSize;
				pRpiH->nImgOffset[ii] = nImgSetSize;
				nImgSetSize += nSize;
				pRpiH->nImgCnt++;



				//// 検証
				//for(int jj=0; jj<200; jj++) {
				//	memset( &pImg[ pImg[ pRpiH->nImgOffset[ii] ]  + 2048*jj], 0xff, 2048);
				//}

#ifdef SOT_IMG_OUTPUT
//// BMP
if(true) {
	CString s;
	//s.Format("to_raw_%d_%d.bmp", pRpiH->nFCNo, ii);
	s.Format("to_raw_%d.bmp", ii);
	KizuBitmapManager kb;
	kb.DataToBMPfile_Gray(pRpiH->nWidth, pRpiH->nHeight, &pImg[ pRpiH->nImgOffset[ii] ], s);
}
#endif
			}		


			//------------------------------------
			// 伝文送信

			//// 伝文長 (データ部もある場合は、共有メモリにセットされた分だけ加算する)
			pRpiH->base_head.len = sizeof(HT_RPI_FDATA) + nImgSetSize;
			pRpiH->base_head_addr.len_head = sizeof(HT_RPI_FDATA);
			pRpiH->base_head_addr.len_addr[0] = nImgSetSize;
			mque_pSockRpi[nRpiIdx]->AddPool(pRpiH);								// 送信
		}

		mcls_Time.PutTimestamp(MC_TIME_RPI_E);		
	}
*/


	////////////////////////////////////
	// 統括へ送信
	HT_TO_HEAD* pToH = new HT_TO_HEAD;
	memset(pToH, 0x00, sizeof(HT_TO_HEAD));
//LOGSOCK(em_MSG), "[%d] ヘッダ部：HT_TO_HEAD=%d", pPipe->frame_num, sizeof(HT_TO_HEAD));

	// 末尾が可変だけでなく、
	// 中間項目も可変対応とするので、一旦最大領域を毎回 New する。
	int nToSendSize = 0 ;				// 実際の送信サイズ
	int nToSetMaxSize = sizeof(HT_TO_DATA) + pPipe->smem_size;		// New最大サイズ

	//// Jpegフレーム画像 生成するフレーム？
	bool bIsDebug_JpegFrame = false;			// △△△△△△△△△△△△△△△△△△△ 完全テスト用。実際にはメールスロット等で判断？。以降の処理はチャントできている

	//if(bIsDebug_JpegFrame && pSmemH->frame_offset > 0) {
	//	//nToSetMaxSize += SIZE_FRAME_IMG_X*SIZE_FRAME_IMG_Y*MAX_IMAGE_CNT;
	//	NCL_FRAME_IMAGE*	pF = (NCL_FRAME_IMAGE*)&pSmemAry[pSmemH->frame_offset];	// 共有メモリの画像情報先頭位置
	//	nToSetMaxSize += pF->stride * pF->height * MAX_IMAGE_CNT;
	//}

//	LOG(em_INF), "frame%d : ofs %d, smemsz %d, to %d", pPipe->frame_num, pPipe->smem_offset, pPipe->smem_size, nToSetMaxSize);

	if(0 != nFrameSkip) nToSetMaxSize = sizeof(HT_TO_DATA) + sizeof(NCL_SMEM_FRAME_HEADER);

//LOG(em_INF), "[%s] [%d] Jpegフレーム画像 Size=%d Skip=%d smem_size=%d", my_sThreadName, pToH->nFrameNo, nToSetMaxSize, nFrameSkip, pPipe->smem_size);


LOGSOCK(em_MSG), "[%d] データ部：データ領域=%d", pToH->nFrameNo, nToSetMaxSize);
	BYTE* pToAry = new BYTE[nToSetMaxSize];
	HT_TO_DATA* pToD = (HT_TO_DATA*)&pToAry[0];
	memset(pToAry, 0x00, sizeof(HT_TO_DATA));		// ヘッダー部のみ。後は全部コピーするので良いかな

	int idx = 0;									// 片面動作時は 0固定


	//------------------------------------
	//// ベースヘッダー部生成
	// トランザクションコード
	memcpy(pToH->base_head.code, T_CODE_DEFECT, sizeof(pToH->base_head.code) );
	memcpy(pToH->base_head.pname, getmypcname(), sizeof(pToH->base_head.pname) );

	// 業務伝文の場合は、論理回線IDをセット (0始まり)
	pToH->base_head.subinf = (short)(pPipe->camid-1) % NUM_CAM_POS;
	pToH->base_head.addr[0] = (long*)&pToAry[0];	// 今回 Newしたポインタをセット
	pToH->base_head.addr[1] = NULL;
	pToH->base_head.len = sizeof(HT_TO_HEAD);	// 伝文長 は一番最後
	//if(pPipe->offset > 0) pToH->base_head.len += pPipe->size;


	//------------------------------------
	//// ヘッダー部生成
	pToH->nFrameNo			= pPipe->frame_num;
	pToH->nFrameSkip		= nFrameSkip;
	pToH->nJpegCmp			= 0;				// 現時点では未確定

	pToH->camset			= pPipe->camid;
	pToH->offset			= pPipe->smem_offset;
	pToH->size				= pPipe->smem_size;

	if(0 == nFrameSkip) {
		pToH->nEdgeNg			= STATUS_EDGELOST == pSmemH->status ? 1 : 0;
		pToH->nFrameSt			= pSmemH->status;

		pToH->frame_pos			= (long)pSmemH->front_pos;
		memcpy( pToH->br,		pSmemH->frame_brightness, sizeof(pToH->br) );
	}

	memcpy( pToH->edge_pos,	dEdgeAvePos, sizeof(pToH->edge_pos) );
	memcpy( pToH->edge_pos_max,	dEdgeMaxPos, sizeof(pToH->edge_pos_max) );
	memcpy( pToH->edge_pos_min,	dEdgeMinPos, sizeof(pToH->edge_pos_min) );


	//------------------------------------
	//// データ部生成
	// 2023.08.29 諸藤 SMEMFULL時の共有メモリアクセス違反対応 --------------------------->>>>>>
	// pToD->defect_num = pSmemH->defect_num;
	// pToD->image_num = pSmemH->image_num;
	if(0 == nFrameSkip)
	{
		pToD->defect_num = pSmemH->defect_num;
		pToD->image_num = pSmemH->image_num;
	}
	else
	{
		pToD->defect_num = 0;
		pToD->image_num = 0;
	}
	// 2023.08.29 諸藤 SMEMFULL時の共有メモリアクセス違反対応 <<<<<<---------------------------




	//------------------------------------
	// 共有メモリ部
	// 基本的に8の倍数とする
	nToSendSize += GET_JUST_SIZE(sizeof(HT_TO_DATA), 8);	//8の倍数
	BYTE*	pSemF = NULL;		// 共有メモリ読込先
	int		nWriteSize = 0;		// 書き込みサイズ
//LOGSOCK(em_MSG), "[%d] データ部：HT_TO_DATA=%d defect_num=%d image_num=%d", pToH->nFrameNo, GET_JUST_SIZE(sizeof(HT_TO_DATA), 8), pToD->defect_num, pToD->image_num);



	//----------
	// NCL_SMEM_FRAME_HEADER
	if( 0 == nFrameSkip ) {
		pSemF = &pSmemAry[0];		// = pSmemH = NCL_SMEM_FRAME_HEADER
		nWriteSize = sizeof(NCL_SMEM_FRAME_HEADER);
		pToD->header_offset = nToSendSize;
		memcpy(&pToAry[nToSendSize], pSemF, nWriteSize);
		nToSendSize += GET_JUST_SIZE(nWriteSize, 8);
//LOGSOCK(em_MSG), "[%d] データ部：NCL_SMEM_FRAME_HEADER=%d", pToH->nFrameNo, GET_JUST_SIZE(nWriteSize, 8));
	}

	//----------
	// 疵情報関連
	if (0 != nFrameSkip || 0 == pSmemH->defect_num) {
		// 疵無しの場合、処理しないようにする？？
		pToD->result_offset = 0;
		pToD->attr_offset = 0;
	} else {
		//----------
		//nc_defect_result
		pSemF = &pSmemAry[pSmemH->result_offset];
		nWriteSize = sizeof(NCL_DEFECT_RESULT) * pSmemH->defect_num;
		pToD->result_offset = nToSendSize;
		memcpy(&pToAry[nToSendSize], pSemF, nWriteSize);
		nToSendSize += GET_JUST_SIZE(nWriteSize, 8);
//LOGSOCK(em_MSG), "[%d] データ部：nc_defect_result=%d", pToH->nFrameNo, GET_JUST_SIZE(nWriteSize, 8));

		//----------
		//attr
		pSemF = &pSmemAry[pSmemH->attr_offset];
		nWriteSize = sizeof(double)*(NCL_nattr_common+(NCL_nattr_angle*MAX_IMAGE_CNT))*pSmemH->defect_num;
		pToD->attr_offset = nToSendSize;
		memcpy(&pToAry[nToSendSize], pSemF, nWriteSize);
		nToSendSize += GET_JUST_SIZE(nWriteSize, 8);
//LOGSOCK(em_MSG), "[%d] データ部：attr=%d", pToH->nFrameNo, GET_JUST_SIZE(nWriteSize, 8));
	}


	//----------
	// 疵画像関連
	mcls_Time.PutTimestamp(MC_TIME_JPEG_S);
//	if ( true) {
	if ( 0 != nFrameSkip ||0 == pSmemH->image_num) {
		pToD->image_offset = 0;
	} else {
		pToD->image_offset = nToSendSize;
		{
			int nJpegSize = CreateJpegData(pSmemH, &pToAry[nToSendSize], nToSetMaxSize -nToSendSize );
			nToSendSize += nJpegSize;
			//LOG(em_MSG), "画像(%d)=%d(%d)", nJpegSize, nToSendSize);
LOGSOCK(em_MSG), "[%d] データ部：疵画像サイズ=%d", pToH->nFrameNo, nJpegSize);
		}
	}
	mcls_Time.PutTimestamp(MC_TIME_JPEG_E);


	//----------
	// Jpegフレーム画像関連
	mcls_Time.PutTimestamp(MC_TIME_JF_S);
	if (bIsDebug_JpegFrame && pSmemH->frame_offset > 0) {
		pToD->frame_jpeg_offset = nToSendSize;
		int nJpegSize = CreateJpegFrame(pSmemH, &pToAry[nToSendSize]);
		nToSendSize += nJpegSize;
LOGSOCK(em_MSG), "[%d] データ部：Jpegフレーム画像サイズ=%d", pToH->nFrameNo, nJpegSize);
	} else {
		pToD->frame_jpeg_offset = 0;
	}
	mcls_Time.PutTimestamp(MC_TIME_JF_E);

LOGSOCK(em_MSG), "[%d] データ部：HT_TO_DATA=%d defect_num=%d image_num=%d header_offset=%d result_offset=%d attr_offset=%d image_offset=%d frame_jpeg_offset=%d",
pToH->nFrameNo,
GET_JUST_SIZE(sizeof(HT_TO_DATA), 8),
pToD->defect_num,
pToD->image_num,
pToD->header_offset,
pToD->result_offset,
pToD->attr_offset,
pToD->image_offset,
pToD->frame_jpeg_offset
);
// ダミーデータ
//if (0 == pSmemH->defect_num)
//{
//	LOGSOCK(em_MSG), "[%d] %d から %d byte分 0xFFでパディング", pToH->nFrameNo, nToSendSize, sizeof(HT_TO_HEAD) - SIZE_SOCK_HEAD);
//	memset(&pToAry[nToSendSize], 0xFF, sizeof(HT_TO_HEAD) - SIZE_SOCK_HEAD);
//	pToAry[nToSendSize + (sizeof(HT_TO_HEAD) - SIZE_SOCK_HEAD - 1)] = 0xfe;
//}

	// 判定PCのパフォーマンスカウンタの周波数
	QueryPerformanceFrequency((LARGE_INTEGER*)&pToH->ht_freq[0]);

	mcls_Time.PutTimestamp(MC_TIME_ALL_E);

	__int64 nWk;
	nWk = mcls_Time.GetRawTimestamp(MC_TIME_ALL_S);
	memcpy(&pToH->ht_timestamp_start, (void*)&nWk, sizeof(__int64));	// 画像処理エンジンからフレーム受信時のタイムスタンプ
	nWk = mcls_Time.GetRawTimestamp(MC_TIME_ALL_E);
	memcpy(&pToH->ht_timestamp_end, (void*)&nWk, sizeof(__int64));		// HT→TO送信時のタイムスタンプ


	////◆	
	//LOG(em_MSG), "[%s] [%d] 送信開始 <cam=%d> 疵=%d 画像=%d DataSize=%dbyte 切出Jpeg圧縮=%dsm F/Cut距離=%d/%dmm Edge=%d/%d", my_sThreadName,
	//	pToH->nFrameNo, pToH->camset, pSmemH->defect_num, pSmemH->image_num, nToSendSize,
	//	(int)mcls_Time.GetTimestamp(MC_TIME_JPEG_E, MC_TIME_JPEG_S), 
	//	(int)pSmemH->front_pos, (int)pSmemH->shear_cut_pos, 
	//	(int)dEdgeAvePos[0], (int)dEdgeAvePos[1]
	//	);

	LOGSOCK(em_MSG), "[%d] 送信開始 <cam=%d> Size=%.1fKb (Que=%d) All:%dms Rpi:%dms Jpeg:%dms", pToH->nFrameNo, pToH->camset, (double)nToSendSize / 1024.0, mcls_pSock[idx]->gque_Deli.GetCount(), (int)mcls_Time.GetTimestamp(MC_TIME_ALL_E, MC_TIME_ALL_S), (int)mcls_Time.GetTimestamp(MC_TIME_RPI_E, MC_TIME_RPI_S), (int)mcls_Time.GetTimestamp(MC_TIME_JPEG_E, MC_TIME_JPEG_S));



	//------------------------------------
	//// サイズ反映
	pToH->base_head.len = sizeof(HT_TO_HEAD) + nToSendSize;		// ヘッダー＋データ部
LOGSOCK(em_MSG), "[%d] 　　　計=%d(%d)", pToH->nFrameNo, pToH->base_head.len, nToSendSize);

	// フレーム情報を統括管理に送信する
	// ※ロジックパララン時は送信抑制
	if (m_bFrameSend)
	{
		//// 統括へ送信
		// スレッドキューに登録
		if (mcls_pSock[idx]->IsConnect() && mcls_pSock[idx]->gque_Deli.GetFreeCount() > 0) {
			mcls_pSock[idx]->gque_Deli.AddToTail((SOCK_F_HEAD*)pToH);
		}
		else {
			// 空き無し
			//mcls_pData->SendBack(pToH->offset);		// 下で無条件に返すのでここでの返却無し
			delete[] pToAry;
			delete pToH;

			if (mcls_pSock[idx]->IsConnect()) {
				LOG(em_ERR), "[%s] 統括送信キュー キューフル!!", my_sThreadName);
				syslog(SYSNO_QUEFULL_WAR, "[統括送信]");
			}
		}
	}
	else
	{
		// 送信せずに破棄
		delete[] pToAry;
		delete pToH;
	}

	//// オフセット値の返却対象  
	// ※今回は共有メモリから直接送信じゃないので、ここでもう返却
	if(NCL_SMEMPIPE_SMEMFULL != pPipe->smem_offset ) {
		mcls_pData->SendBack(pPipe->smem_offset);
	}


	//// 時間
	mcls_Time.CounterEnd();

	g_nTime_All[0]		= (int)mcls_Time.GetTimestamp(MC_TIME_ALL_E, MC_TIME_ALL_S);
	g_nTime_Rpi[0]		= (int)mcls_Time.GetTimestamp(MC_TIME_RPI_E, MC_TIME_RPI_S);
	g_nTime_To[0]		= (int)mcls_Time.GetTimestamp(MC_TIME_JPEG_E, MC_TIME_JPEG_S);
	g_nTime_ToFrame[0]	= (int)mcls_Time.GetTimestamp(MC_TIME_JF_E, MC_TIME_JF_S); 

	g_nTime_All[1]		= max(g_nTime_All[0], g_nTime_All[1]);
	g_nTime_Rpi[1]		= max(g_nTime_Rpi[0], g_nTime_Rpi[1]);
	g_nTime_To[1]		= max(g_nTime_To[0], g_nTime_To[1]);
	g_nTime_ToFrame[1]	= max(g_nTime_ToFrame[0], g_nTime_ToFrame[1]);
}

//------------------------------------------
// 定周期
//------------------------------------------
void DataSender::SendHls()
{
	HT_TO_HEAD_HLS* pToH = new HT_TO_HEAD_HLS;
	memset(pToH, 0x00, sizeof(HT_TO_HEAD_HLS));

	//------------------------------------
	// ヘッダ
	memcpy( pToH->base_head.code, T_CODE_HELCY, sizeof(pToH->base_head.code) );
	memcpy( pToH->base_head.pname, getmypcname(), sizeof(pToH->base_head.pname) );
	pToH->base_head.subinf = 0;
	pToH->base_head.addr[0] = NULL;
	pToH->base_head.addr[1] = NULL;


	//------------------------------------
	// データ
	pToH->nEngStatSet		= mcls_pStatus->IsEngStatNew();
	ENG_STAT				EngStatWk;
	mcls_pStatus->GetEngStat(&EngStatWk);
	memcpy( &pToH->EngStat, &EngStatWk, sizeof(ENG_STAT) );

#ifdef DEBUG_LOG_HT_HEALTHY
	for (int ii = 0; ii < 2; ii++)
	{
		LOG(em_FIL), "[%s] エンジン状態 判定→統括ヘルシー [ID:%d][温度:%.3f][露光:%.3f %.3f %.3f][ゲイン:%.3f %.3f %.3f][電圧:%.3f]",
			my_sThreadName,
			pToH->EngStat.camid[ii],
			pToH->EngStat.temp[ii],
			pToH->EngStat.exptime[0][ii],
			pToH->EngStat.exptime[1][ii],
			pToH->EngStat.exptime[2][ii],
			pToH->EngStat.gain[0][ii],
			pToH->EngStat.gain[1][ii],
			pToH->EngStat.gain[2][ii],
			pToH->EngStat.volt[ii]);
	}
#endif

	int idx = 0;
	if( ! mcls_pSock[idx]->gque_Deli.AddToTailFreeCheck( (SOCK_F_HEAD*)pToH, 0) ) {
		LOG(em_ERR), "[%s] 判定→統括間 定周期送信キューフル", my_sThreadName);
		delete pToH;
	}
}

//------------------------------------------
// NewDataExec で Newした送信データの開放 および 共有メモリのオフセット値開放
// int idx どっちのソケット (0オリジン)
//------------------------------------------
void DataSender::NewDataSendBack(int idx)
{
	HT_TO_HEAD* pData = (HT_TO_HEAD*)mcls_pSock[idx]->gque_SendBack.GetFromHead();
		
//◆	LOG(em_MSG), "[%s] 完了[%d]", my_sThreadName, pData->nFrameNo);
// 調査
	if (pData->camset <= 0 || pData->camset > MAX_CAMSET) {
		LOG(em_ERR), "[%s] 判定→統括間 返却用NewDataSendBack 異常 [camset=%d fno=%d]", my_sThreadName, pData->camset, pData->nFrameNo);
		return;
	}

	//// フレームNo連続性チェック
	int nIndex = pData->camset - mcls_pStatus->GetCamPairItemCamset(m_nCamPair, 0, m_emKindHt);
	if(m_nFrameNo[nIndex] +1 != pData->nFrameNo) {
		LOG(em_ERR), "[%s] 判定→統括間 フレームNo送信抜け [camset=%d now=%d pre=%d]", my_sThreadName, pData->camset, pData->nFrameNo, m_nFrameNo[nIndex]);
		syslog(113, "[判定→統括間 フレームNo送信抜け camset=%d now=%d pre=%d]", pData->camset, pData->nFrameNo, m_nFrameNo[nIndex]);
	}
	LOGSOCK(em_MSG), "[%d] 送信完了 <cam=%d> (Que=%d) Jpeg圧縮時間:%dms", pData->nFrameNo, pData->camset, mcls_pSock[idx]->gque_Deli.GetCount(), g_nTime_To[0]);


	//// 送信完了したフレームNoをセット
	m_nFrameNo[nIndex] = pData->nFrameNo;
	m_nDspFrameNo = pData->nFrameNo;


	//// 必ず返却、オフセット値の返却対象
	////mcls_pData->SendBack( pData->offset );

	
	//// 統括へ送信データを開放
	SAFE_DELETE( pData->base_head.addr[0] );
	SAFE_DELETE( pData ); 
}




// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Jpeg変換
//------------------------------------------
// 判定管理でJPEG変換 (切り出し画像)
// NCL_SMEM_FRAME_HEADER* pSmem 本フレームの共有メモリ先頭アドレス
// BYTE* pImgAddr 統括送信用構造体 切り出し画像セット先頭アドレス
// 戻り値：切り出し画像関連 トータルサイズ
//------------------------------------------
int DataSender::CreateJpegData(NCL_SMEM_FRAME_HEADER* pSmem, BYTE* pImgAddr, int freesize)
{
	BYTE* pSmemAry = (BYTE*)pSmem;						// 使いやすいように。この共有メモリ先頭ポインタ


//	mcls_pLog->WriteLock( LogFileManager::em_MSG, "[%s] 圧縮開始 疵=%d, サイズ=%d, off_img=%d off_次=%d,%d,%d", 
//		my_sThreadName, pH->image_num, pSmem->size, pH->image_offset, pH->frame_jpg_offset, pH->profile_offset, pH->rawframe_jpg_offset);


	//// 実行有無チェック
	int nImgCnt = pSmem->image_num;
	if (0 == nImgCnt) return 0;


	//// BMP→JPEG
	int nTotalBmpSize = 0;						// 送信対象のビットマップ画像領域サイズ
	int nTotalJpegSize = 0;
	int nFreeSize = freesize;
	bool bJpegError = false;

	for(int ii=0; ii<nImgCnt; ii++) {
		DEFECT_HEAD_IMAGE*	pT = (DEFECT_HEAD_IMAGE*)&pImgAddr[nTotalJpegSize];		// 自分の書き込み領域
		NCL_IMAGE_HEADER*	pF = (NCL_IMAGE_HEADER*)&pSmemAry[pSmem->image_offset + nTotalBmpSize];	// 共有メモリの画像情報先頭位置

		// データセット
		pT->nWidth    = pF->width;
		pT->nHeight   = pF->height;
		pT->fShrinkRatio = pF->shrinkratio;

		// Jpeg画像に変換
		int nOffset   = sizeof(DEFECT_HEAD_IMAGE);		// 自構造体内のJpeg画像オフセット
		int nJpegSize = 0;
		for(int jj=0; jj<MAX_IMAGE_CNT; jj++) {

#ifdef SOT_IMG_OUTPUT
//// BMP
if(true) {
	CString s;
	//s.Format("ht%d.bmp", jj);
	//s.Format("to_kiri%d_%d_%d_%d.bmp", pSmem->frame_num, pSmem->camset, ii, jj);
	s.Format("to_kiri%d_%d_%d.bmp", pSmem->camid, ii, jj);
	KizuBitmapManager kb;
	kb.DataToBMPfile_Gray(pT->nWidth, pT->nHeight, &pF->image[ pT->nWidth*pT->nHeight*jj ], s);
}
#endif

			if( nFreeSize - (nTotalJpegSize + nOffset) < (int)(pT->nWidth * pT->nHeight / 2) )
				DebugBreak();

#ifdef JPEGLIB_IPP
			KizuJpegManager::BmpToJpeg(pT->nWidth, pT->nHeight, &pF->image[pT->nWidth*pT->nHeight*jj],
				m_nJpegQuality, &nJpegSize, &pImgAddr[nTotalJpegSize + nOffset]);
#else
			int resp = mcls_KizuJpegManager.BmpToJpeg(pT->nWidth, pT->nHeight, &pF->image[pT->nWidth*pT->nHeight*jj],
				m_nJpegQuality, &nJpegSize, &pImgAddr[nTotalJpegSize + nOffset]);
			// Jpeg変換エラー
			if (0 != resp && !bJpegError)
			{
				// 1フレームの処理につき、初回のみログ出力
				LOG(em_ERR), "[%s] Jpeg画像変換エラー [frameno=%d camid=%d defno=%d imgno=%d defnum=%d, imgnum=%d][v:%d h:%d] result=%d", my_sThreadName, pSmem->frame_num, pSmem->camid, ii + 1, jj + 1, pSmem->defect_num, pSmem->image_num, pT->nWidth, pT->nHeight, resp);
				bJpegError = true;
			}
#endif

#ifdef SOT_IMG_OUTPUT
//// JPEG
if(true) {
	CString s;
	//s.Format("ht%d.jpg", jj);
	//s.Format("to_kiri%d_%d_%d_%d.jpg", pSmem->frame_num, pSmem->camset, ii, jj);
	s.Format("to_kiri%d_%d_%d.jpg", pSmem->camid, ii, jj);
	std::ofstream   os;
	os.open(s, std::ios::binary);
	os.write((const char*)&pImgAddr[nTotalJpegSize + nOffset], nJpegSize );
	os.close();
}
#endif

			pT->nSize[jj]		= nJpegSize;
			pT->nImgOffset[jj]	= nOffset;
			
			// 次のため
			nOffset				+= GET_JUST_SIZE(nJpegSize, 8);		// 8の倍数で、今回の配列サイズ (=次の開始位置)
		}

		// 次のため
		pT->nNextImgOffset  =  nOffset;  //(((sizeof(DEFECT_HEAD_IMAGE) + nJpegNext)/8)+1)*8;	
		nTotalJpegSize		+= nOffset;
		//nTotalBmpSize		+= (pF->width*pF->height*MAX_IMAGE_CNT) + sizeof(DWORD)*4;
		nTotalBmpSize		+= (pF->width*pF->height*MAX_IMAGE_CNT) + (sizeof(NCL_IMAGE_HEADER) - 1);
	}


	//// 小さくなった差分を算出
	int nSa = nTotalBmpSize - nTotalJpegSize;
	if(0 > nSa) {
		// 領域破壊！！
		LOG(em_ERR), "[%s]    BMP画像よりもJPEG画像の方がサイズが大きい！[差=%d] Bmp:%d Jpeg:%d", my_sThreadName, nSa, nTotalBmpSize, nTotalJpegSize);
		LOGSOCK(em_ERR), "[%s]    BMP画像よりもJPEG画像の方がサイズが大きい！[差=%d] Bmp:%d Jpeg:%d", my_sThreadName, nSa, nTotalBmpSize, nTotalJpegSize);
	} else {
		LOGSOCK(em_MSG), "[%s]    BMP画像→JPEG画像圧縮 [差=%dbyte] %d⇒%d", my_sThreadName, nSa, nTotalBmpSize, nTotalJpegSize);
	}
	return nTotalJpegSize;
}

//------------------------------------------
// 判定管理でJPEG変換 (フレーム画像)
// NCL_SMEM_FRAME_HEADER* pSmem 本フレームの共有メモリ先頭アドレス
// BYTE* pImgAddr 統括送信用構造体 切り出し画像セット先頭アドレス
// 戻り値：切り出し画像関連 トータルサイズ
//------------------------------------------
int DataSender::CreateJpegFrame(NCL_SMEM_FRAME_HEADER* pSmem, BYTE* pImgAddr)
{
	BYTE* pSmemAry = (BYTE*)pSmem;						// 使いやすいように。この共有メモリ先頭ポインタ


	//// BMP→JPEG


	DEFECT_HEAD_IMAGE*	pT = (DEFECT_HEAD_IMAGE*)&pImgAddr[0];		// 自分の書き込み領域
	//BYTE*				pF = (BYTE*)&pSmemAry[pSmem->frame_offset];	// 共有メモリの画像情報先頭位置
	NCL_FRAME_IMAGE*	pF = (NCL_FRAME_IMAGE*)&pSmemAry[pSmem->frame_offset];	// 共有メモリの画像情報先頭位置

	// データセット
	//pT->nWidth    = SIZE_FRAME_IMG_X;
	//pT->nHeight   = SIZE_FRAME_IMG_Y;
	pT->nWidth    = pF->width;
	pT->nHeight   = pF->height;


	// Jpeg画像に変換
	int nOffset   = sizeof(DEFECT_HEAD_IMAGE);		// 自構造体内のJpeg画像オフセット
	int nJpegSize = 0;
	for(int jj=0; jj<(int)pF->channel; jj++) {

#ifdef JPEGLIB_IPP
		//KizuJpegManager::BmpToJpeg(pT->nWidth, pT->nHeight, &pF[ pT->nWidth*pT->nHeight*jj ], m_nJpegQuality, &nJpegSize, &pImgAddr[0 + nOffset]);
		KizuJpegManager::BmpToJpeg(pT->nWidth, pT->nHeight, &pF->image[pF->stride*pF->height*jj], m_nJpegQuality, &nJpegSize, &pImgAddr[0 + nOffset]);
#else
		mcls_KizuJpegManager.BmpToJpeg(pT->nWidth, pT->nHeight, &pF->image[pF->stride*pF->height*jj], m_nJpegQuality, &nJpegSize, &pImgAddr[0 + nOffset]);
#endif

		pT->nSize[jj]		= nJpegSize;
		pT->nImgOffset[jj]	= nOffset;
		
		// 次のため
		nOffset				+= GET_JUST_SIZE(nJpegSize, 8);		// 8の倍数で、今回の配列サイズ (=次の開始位置)
	}


	int nTotalJpegSize		= nOffset;
	//int nTotalBmpSize		= (SIZE_FRAME_IMG_X*SIZE_FRAME_IMG_Y*MAX_IMAGE_CNT);
	int nTotalBmpSize		= (pF->stride*pF->height*pF->channel);




	//// 小さくなった差分を算出
	int nSa = nTotalBmpSize - nTotalJpegSize;
	if(0 > nSa) {
		// 領域破壊！！
		LOG(em_ERR), "[%s]    (フレーム)BMP画像よりもJPEG画像の方がサイズが大きい！[差=%d]", my_sThreadName, nSa);
		LOGSOCK(em_ERR), "[%s]    (フレーム)BMP画像よりもJPEG画像の方がサイズが大きい！[差=%d]", my_sThreadName, nSa);
	} else {
		LOGSOCK(em_MSG), "[%s]   (フレーム) BMP画像→JPEG画像圧縮 [差=%dbyte] %d⇒%d", my_sThreadName, nSa, nTotalBmpSize, nTotalJpegSize);
	}
	return nTotalJpegSize;
}













/*** マーク監視装置 の技術を応用
//------------------------------------------
// 判定管理でJPEG変換
// HT_TO_HEAD* pData 統括送信用 構造体
// nc_smem_pipe_data* pSmem 共有メモリ構造体
// int* pSender 新しい送信ポインタ
// 戻り値：小さくなったサイズ
//------------------------------------------
int DataSender::CreateJpegData(HT_TO_HEAD* pData, nc_smem_pipe_data* pSmem, HANDLE* pSendAdder)
{
	int nIndex  = 0;						// pDの先頭からのアクセス位置
	int nWkSize = 0;

	nc_smem_frame_header* pH = (nc_smem_frame_header*)(mcls_pData->GetMapPointer() + pSmem->offset);	// 共有メモリの先頭アドレス + オフセット値 = 本フレームでの共有メモリの位置
	BYTE* pHAry = (BYTE*)pH;					// 使いやすいように。このフレームの先頭ポインタ


//	mcls_pLog->WriteLock( LogFileManager::em_MSG, "[%s] 圧縮開始 疵=%d, サイズ=%d, off_img=%d off_次=%d,%d,%d", 
//		my_sThreadName, pH->image_num, pSmem->size, pH->image_offset, pH->frame_jpg_offset, pH->profile_offset, pH->rawframe_jpg_offset);


	//// 実行有無チェック
	int nImgCnt = pH->image_num;
	if( 0 == nImgCnt ) return 0;
	if( 0 >= pSmem->size ) return 0;				// 特別な意味


	//// 領域確保
	// ※ この時点では、送信サイズではない
	BYTE* pD = new BYTE[pSmem->size];			// 新しい統括に送るデータエリア

	//// 画像領域前のエリアをコピー
	nWkSize = pH->image_offset;				// BMP画像の前までは変更無し 
	memcpy( &pD[nIndex], &pHAry[0], nWkSize);
	nIndex += nWkSize;


	//// BMP→JPEG
	int nTotalBmpSize = 0;						// 送信対象のビットマップ画像領域サイズ
	int nTotalJpegSize = 0;
	//int nMemOffset = pH->image_offset;			// 共有メモリの画像情報先頭位置
	for(int ii=0; ii<nImgCnt; ii++) {

		DEFECT_HEAD_IMAGE* pT = (DEFECT_HEAD_IMAGE*)&pD[nIndex];						// 自分の領域
		nc_image_hdr* pF = (nc_image_hdr*)&pHAry[pH->image_offset + nTotalBmpSize];		// 共有メモリの画像情報先頭位置

		// データセット
		pT->nWidth    = pF->width;
		pT->nHeight   = pF->height;
		nTotalBmpSize += (pF->width*pF->height*MAX_IMAGE_CNT) + sizeof(DWORD)*2;

		// Jpeg画像に変換
		int nOffset   = sizeof(DEFECT_HEAD_IMAGE);
		int nJpegSize = 0;
		for(int jj=0; jj<MAX_IMAGE_CNT; jj++) {


#ifdef SOT_IMG_OUTPUT
//// BMP
if(true) {
	CString s;
	//s.Format("ht%d.bmp", jj);
	//s.Format("to_kiri%d_%d_%d_%d.bmp", pSmem->frame_num, pSmem->camset, ii, jj);
	s.Format("to_kiri%d_%d_%d.bmp", pSmem->camset, ii, jj);
	KizuBitmapManager kb;
	kb.DataToBMPfile_Gray(pT->nWidth, pT->nHeight, &pF->image[ pT->nWidth*pT->nHeight*jj ], s);
}
#endif


			KizuJpegManager::BmpToJpeg(pT->nWidth, pT->nHeight, &pF->image[ pT->nWidth*pT->nHeight*jj ], m_nJpegQuality, &nJpegSize, &pD[nIndex + nOffset]);

#ifdef SOT_IMG_OUTPUT
//// JPEG
if(true) {
	CString s;
	//s.Format("ht%d.jpg", jj);
	//s.Format("to_kiri%d_%d_%d_%d.jpg", pSmem->frame_num, pSmem->camset, ii, jj);
	s.Format("to_kiri%d_%d_%d.jpg", pSmem->camset, ii, jj);
	std::ofstream   os;
	os.open(s, std::ios::binary);
	os.write((const char*)&pD[nIndex + nOffset], nJpegSize );
	os.close();
}
#endif

			pT->nImgOffset[jj]	= nOffset;
			pT->nSize[jj]		= nJpegSize;
			
			// 次のため
			nOffset				+= ((nJpegSize/8)+1)*8;
		}

		// 次のため
		pT->nNextImgOffset  =  nOffset;  //(((sizeof(DEFECT_HEAD_IMAGE) + nJpegNext)/8)+1)*8;	// 8の倍数で、今回の配列サイズ (=次の開始位置)
		nTotalJpegSize		+= nOffset;
		nIndex				+= nOffset;
	}


	// 判定が落ちた調査用
	int nSa = nTotalBmpSize - nTotalJpegSize;
	int nImgNextPoint = pH->image_offset+nTotalBmpSize;
	int nAfter = pSmem->size - nImgNextPoint;
	if(0 > nSa) {
		// 領域破壊！！
		LOG(em_ERR), "[%s] BMP画像よりもJPEG画像の方がサイズが大きい！[差=%d]", my_sThreadName, nSa);

	} else {
		//LOG(em_MSG), "[%s] BMP画像→JPEG画像圧縮 [差=%dbyte]", my_sThreadName, nSa);

//		mcls_pLog->WriteLock( LogFileManager::em_ERR, "[%s] 圧縮後 BMP画像よりもJPEG画像の方がサイズが大きい！[差=%d]", my_sThreadName, nSa);
	}

//	mcls_pLog->WriteLock( LogFileManager::em_MSG, "[%s] 圧縮後 [差=%d] next=%d, after=%d, 誤差=%d", my_sThreadName,
//		nSa, nImgNextPoint, nAfter, pSmem->size - nImgNextPoint - nAfter);

	memcpy( &pD[nIndex], &pHAry[nImgNextPoint], pSmem->size - nImgNextPoint);

	//// 送信するポインタをセット
	*pSendAdder = pD;
	return nSa;
}
***/

