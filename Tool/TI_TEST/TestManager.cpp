#include "StdAfx.h"
#include "TestManager.h"

#include <io.h>
#include <fcntl.h>

// メモリーリーク検出 ----->>>
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//#define DB_WRITE											// 本番時コメント  (テスト時コメントアウト)

//------------------------------------------
// コンストラクタ
//------------------------------------------
TestManager::TestManager(int nNumCam, int nCamStart) :
ThreadManager("TestManager"),
mcls_pCull(NULL),
m_nNumCam(nNumCam),
m_nCamStart(nCamStart)
{

	//// ハンドル生成
	m_evTimerExec  = CreateWaitableTimer(NULL, FALSE, NULL);
	my_nSelectMode = GRAB_MODE_INIT;
	my_nSmpNowNo = 0;

}
//------------------------------------------
// デストラクタ
//------------------------------------------
TestManager::~TestManager(void)
{

}

////////////////////////////////////////////////////////////////////////////////
// スレッド
////////////////////////////////////////////////////////////////////////////////
//------------------------------------------
// スレッド開始前処理 (スレッド実行直後(シグナルセット前)にコールバック)
//------------------------------------------
int TestManager::ThreadFirst()
{
//// 事前準備
												
	//// イベント設定
	HANDLE hArray[] = { gque_Deli.g_evSem,
						m_evTimerExec
						};
	SetEventNum(sizeof(hArray)/sizeof(HANDLE), hArray);


	return 0;
}

//------------------------------------------
// スレッド終了前処理 (終了シグナル後にコールバック)
//------------------------------------------
int TestManager::ThreadLast()
{
	DELIVERY_KEY *pDeli;				// キューデータ

	// 未処理のキューを破棄
	while(true) {
		pDeli = gque_Deli.GetFromHead(0);
		if( pDeli == NULL ) break;
		
		// 削除
		LOG(em_WAR), "[%s] 未処理キュー削除", my_sThreadName);
		delete pDeli;
	}

	// 定周期停止
	CancelWaitableTimer(m_evTimerExec);

	SAFE_DELETE_HANDLE(m_evTimerExec);

	return 0;
}

//------------------------------------------
// スレッドイベント発生
// int nEventNo イベントNo (0オリジン)
//------------------------------------------
void TestManager::ThreadEvent(int nEventNo)
{
	DELIVERY_KEY*	pDeli;			// 受け渡し依頼データ

	////// シグナルの条件
	enum {	EV_QUE = 0,						// キュー依頼
			EV_TIMER_EXEC					// 処理実行 定周期タイマー
	};

	////// シグナル条件分け
	switch (nEventNo) {

//-----------------------------------------------------------------------------------------------
	case EV_QUE:
		pDeli = gque_Deli.GetFromHead(); 
		Exec(pDeli);
		SAFE_DELETE(pDeli);
		break;
	
	case EV_TIMER_EXEC: 				// 処理実行 定周期タイマー
		if(true) {
			LOG(em_MSG), "[%s] 定周期タイマー", my_sThreadName);
			ExecCycle();
		}
		break;

//-----------------------------------------------------------------------------------------------
	default :
		// ありえない！！
		LOG(em_ERR), "[%s] ThreadEvent EventNo NG=%d", my_sThreadName, nEventNo );
		return;
	}
}

//========================
// 確保時
void TestManager::Alloc()
{
	//// メンバー変数に登録
	ParamManager::P_COMMON_MAST const& prm = mcls_pParam->GetCommonMast();

	my_nFrameW = SIZE_FRAME_IMG_X / prm.nImgCullX;
	my_nFrameH = SIZE_FRAME_IMG_Y / prm.nImgCullY;
	my_nImgSize = my_nFrameW * my_nFrameH;				// 使う機会が多いから定義しておく
	my_nCamsetNum = NUM_CAM_POS;
	my_nAngleNum = MAX_PHYSICAL_CAMANGLE;
	my_nChannel = MAX_CAMANGLE;
	my_nFrameNum = 0;

	// 画像領域確保
	pImgs = (BYTE*)_aligned_malloc(my_nImgSize * my_nChannel, 32);


	// 初期化
	memset(pImgs, 0x00, my_nImgSize);
};

//========================
// 解放時
void TestManager::Free()
{
	if (NULL != pImgs) {
		_aligned_free(pImgs);
		pImgs = NULL;
	}
};


// イベント受信処理
void TestManager::Exec(DELIVERY_KEY* pDeli)
{
	if (GRAB_MODE_MAN == pDeli->nMode) {
		// 手動
		LOG(em_MSG), "[%s] 手動実行 [Mode:%d]", my_sThreadName, pDeli->nMode );

		// 手動ファイルオ－プン
		OpenManual();
		// 手動ファイルクローズ
		CloseManual();

		//// BMPのデータ部のみ取得
		KizuBitmapManager	kb;
		int	nWidth;
		int nHeght;
		kb.BMPfileToData_Gray(pDeli->cPath, &nWidth, &nHeght);

		int nBytes = nWidth * nHeght;
		BYTE* img = new BYTE[nBytes];
		int rec = kb.BMPfileToData_Gray(pDeli->cPath, &nWidth, &nHeght, img);
		if (0 < rec) {
			OnManualFrameGrabbed(img, nBytes);
		}
		delete img;

	}
	else if (GRAB_MODE_BMP == pDeli->nMode) {
		// BMP
		if (0 == pDeli->nCycle) {
			// BMPファイルクローズ
			CloseBmp();

			// タイマーキャンセル
			LOG(em_MSG), "[%s] タイマーキャンセル [Mode:%d]", my_sThreadName, pDeli->nMode );
			CancelWaitableTimer(m_evTimerExec);
		}
		else {
			// BMPファイルオープン
			bool bRtn = OpenBmp(pDeli->cPath, pDeli->nStart, pDeli->nEnd, pDeli->nCycle);
			if (!bRtn) {
				LOG(em_ERR), "[%s] BMPファイルオープンエラーのため、タイマー未セット [Mode:%d][Cycle:%d]", my_sThreadName, pDeli->nMode, pDeli->nCycle );
				return;
			}

			// タイマーセット
			LOG(em_MSG), "[%s] タイマーセット [Mode:%d][Cycle:%d]", my_sThreadName, pDeli->nMode, pDeli->nCycle );
			__int64 ts = (pDeli->nCycle) * -1000;		// 1ミリ秒→100ナノ秒 (マイナス指定で、指定時間経過でシグナル) (第3引数は2回目以降の起動間隔)
			int nCycleTime = pDeli->nCycle;				// 定周期間隔
			SetWaitableTimer(m_evTimerExec, (LARGE_INTEGER *)&ts, nCycleTime, NULL, NULL, FALSE);
		}
	}
	else if (GRAB_MODE_AVI == pDeli->nMode) {
		// AVI
		if (0 == pDeli->nCycle) {
			// AVIファイルクローズ
			CloseAvi();

			// タイマーキャンセル
			LOG(em_MSG), "[%s] タイマーキャンセル [Mode:%d]", my_sThreadName, pDeli->nMode );
			CancelWaitableTimer(m_evTimerExec);
		}
		else {
			// AVIファイルオープン
			bool bRtn = OpenAvi(pDeli->cPath, pDeli->nCycle);
			if (!bRtn) {
				LOG(em_ERR), "[%s] AVIファイルオープンエラーのため、タイマー未セット [Mode:%d][Cycle:%d]", my_sThreadName, pDeli->nMode, pDeli->nCycle );
				return;
			}

			// タイマーセット
			LOG(em_MSG), "[%s] タイマーセット [Mode:%d][Cycle:%d]", my_sThreadName, pDeli->nMode, pDeli->nCycle );
			__int64 ts = (pDeli->nCycle) * -1000;		// 1ミリ秒→100ナノ秒 (マイナス指定で、指定時間経過でシグナル) (第3引数は2回目以降の起動間隔)
			int nCycleTime = pDeli->nCycle;				// 定周期間隔
			SetWaitableTimer(m_evTimerExec, (LARGE_INTEGER *)&ts, nCycleTime, NULL, NULL, FALSE);
		}
	}

	my_nSelectMode = pDeli->nMode;			// 選択モード保持

}

// 定周期処理
void TestManager::ExecCycle()
{
	if (GRAB_MODE_MAN == my_nSelectMode) {
		// 手動
		LOG(em_MSG), "[%s] 手動実行無し [Mode:%d]", my_sThreadName, my_nSelectMode );
	}
	else if (GRAB_MODE_BMP == my_nSelectMode) {
		// BMP
		LOG(em_MSG), "[%s] BMP実行 [Mode:%d]", my_sThreadName, my_nSelectMode );
		OnBmpFrameGrabbed();

	}
	else if (GRAB_MODE_AVI == my_nSelectMode) {
		// AVI
		LOG(em_MSG), "[%s] AVI実行 [Mode:%d]", my_sThreadName, my_nSelectMode );
		OnAviFrameGrabbed();
	}

}


// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// サンプル画像操作系

//------------------------------------------
// 連続ビットマップ定義
// char const* cPath ビットマップファイルのフォーマット(形式は、カメラセット(0オリジン),カメラ角度(0オリジン),連番) (例:test_Set0_Ang0_No0.bmp)
// int nNoFrom 連番の開始位置
// int nNoTo 連番の終了位置
// int nCycleTimer 画像取込周期 [ms]
//------------------------------------------
bool TestManager::OpenBmp(char const* cPathFormat, int nNoFrom, int nNoTo, int nCycleTimer)
{
	//// データセット
	my_sSmpPathFormat = cPathFormat;		// ファイルパス
	my_nSmpIntervalNo[0] = nNoFrom;			// 開始No
	my_nSmpIntervalNo[1] = nNoTo;			// 終了No
	my_nSmpCycleTimer = nCycleTimer;		// 周期


	//// オープンチェック
	// ビットマップの場合は、実際のオープンは毎回画像取込時に行う。
	// ここでは、ファイルがあるか程度のチェックを行う
	CString				sPath;
	for (DWORD ii = 0; ii<my_nCamsetNum; ii++) {
		for (DWORD jj = 0; jj<my_nAngleNum; jj++) {
			for (int kk = my_nSmpIntervalNo[0]; kk <= my_nSmpIntervalNo[1]; kk++) {
				// ファイルパスの生成
				sPath.Format(my_sSmpPathFormat, ii, jj, kk);
				// オープンチェック
				int fh = _open(sPath, _O_RDONLY);
				if (-1 == fh) {
					LOG(em_ERR), "[%s] BitMapファイルがありません。(%s)", my_sThreadName, sPath);
					return false;
				}
				_close(fh);
			}
		}
	}

	//// その他必要なデータ
	my_nSmpNowNo = nNoFrom;
	return true;
}

//------------------------------------------
// 連続ビットマップ解放
//------------------------------------------
void TestManager::CloseBmp()
{
	//// 準備はしたけど、今のところ何もしないかな
}

//------------------------------------------
// AVI定義
// char const* cPath AVIファイルのフォーマット(形式は、カメラセット(0オリジン),カメラ角度(0オリジン)) (例:test_Set0_Ang0.avi)
// int nCycleTimer 画像取込周期 [ms]
//------------------------------------------
bool TestManager::OpenAvi(char const* cPathFormat, int nCycleTimer)
{
	//// データセット
	my_sSmpPathFormat = cPathFormat;
	my_nSmpCycleTimer = nCycleTimer;

	//// AVIファイルの準備
	CString				sPath;
	DWORD				ch;
	bool				ret;

	for (DWORD ii = 0; ii<my_nCamsetNum; ii++) {
		for (DWORD jj = 0; jj<my_nAngleNum; jj++) {
			ch = GetCamIndex(ii, jj);

			// 生成		
			mycls_Aiv[ch] = new KizuAviManager();
			// ファイルパスの生成
			sPath.Format(my_sSmpPathFormat, ii, jj);
			// AVIファイルオープン
			ret = mycls_Aiv[ch]->OpenAVIFile(sPath);
			if (!ret) {
				LOG(em_ERR), "[%s] AVIファイルがありません。(%s)", my_sThreadName, sPath);
				return false;
			}
			// 読み込み
			my_nSmpIntervalNo[0] = 0;
			my_nSmpIntervalNo[1] = mycls_Aiv[ch]->GetFrameCnt();
		}
	}

	//// その他必要なデータ
	my_nSmpNowNo = 0;
	return true;
}

//------------------------------------------
// AVI解放
//------------------------------------------
void TestManager::CloseAvi()
{
	for (int ii = 0; ii<NUM_CAMERA; ii++) {
		if (NULL == mycls_Aiv[ii]) continue;

		mycls_Aiv[ii]->CloseAVIFile();
		SAFE_DELETE(mycls_Aiv[ii]);
	}
}

//------------------------------------------
// 画像データを手動で追加
//------------------------------------------
bool TestManager::OpenManual()
{
	my_nSmpNowNo = 0;
	return true;
}
//------------------------------------------
// 画像データを手動で追加
//------------------------------------------
void TestManager::CloseManual()
{
	//// 準備はしたけど、今のところ何もしないかな
}


//------------------------------------------
// 全カメラセット、カメラ角度に対してビットマップ画像をセット
//------------------------------------------
void TestManager::OnBmpFrameGrabbed()
{
	KizuBitmapManager	cls_bmp;
	CString				sPath;
	int					nW;
	int					nH;

	for (DWORD ii = 0; ii<my_nCamsetNum; ii++) {

		if (ii < (m_nCamStart - 1) || ii >= ((m_nCamStart - 1) + m_nNumCam)) {
			continue;		// 対象外カメラ
		}

		//// Bitmapファイルを取得
		//LOG(em_MSG), "lock (set=%d)", ii);
		for (DWORD jj = 0; jj<my_nAngleNum; jj++) {

			sPath.Format(my_sSmpPathFormat, ii, jj, my_nSmpNowNo);
			cls_bmp.BMPfileToData_Gray(sPath, &nW, &nH, &pImgs[my_nImgSize * jj]);

			// 画像上下反転
			KizuAviManager::HantenTB(nW, nH, &pImgs[my_nImgSize * jj]);
			_ASSERT(nW == my_nFrameW && nH == my_nFrameH);

			// チャンネル数分ループ。色差対応
			for (int kk = 1; kk<my_nChannel; kk++) {
				//memcpy(&pImgs[kk*my_nImgSize], pImgs, my_nImgSize);
				KizuMMX::mem_copy_64xmm(pImgs, my_nFrameW, &pImgs[kk*my_nImgSize], my_nFrameW, my_nFrameW, my_nFrameH);
			}
		}

		LOG(em_MSG), "->Fno=%d, path=%s", my_nFrameNum, sPath);

		//// 全長処理に送信要求を登録
		int nCamId = ii + 1;
		OnAddCull(nCamId);


	}

	//// 次のため
	my_nFrameNum++;
	my_nSmpNowNo++;
	if (my_nSmpNowNo > my_nSmpIntervalNo[1]) my_nSmpNowNo = my_nSmpIntervalNo[0];		// 一周まわったから最初の画像を取得
}

//------------------------------------------
// 全カメラセット、カメラ角度に対してAVI画像をセット
//------------------------------------------
void TestManager::OnAviFrameGrabbed()
{
	bool ret;

	for (DWORD ii = 0; ii<my_nCamsetNum; ii++) {

		if (ii < (m_nCamStart - 1) || ii >= ((m_nCamStart - 1) + m_nNumCam)) {
			continue;		// 対象外カメラ
		}

		//LOG(em_MSG), "lock (set=%d)", ii);
		for (DWORD jj = 0; jj<my_nAngleNum; jj++) {

			ret = mycls_Aiv[ii]->LoadFromAVI(my_nSmpNowNo, &pImgs[my_nImgSize * jj], true);
			if (!ret) {
				LOG(em_ERR), "[%s] AVIファイル読み込み失敗 (set=%d, ang=%d, no=%d)", my_nFrameNum, ii, jj, my_nSmpNowNo);
			}

			// チャンネル数分ループ。色差対応
			for (int kk = 1; kk<my_nChannel; kk++) {
				//memcpy(&pImgs[kk*my_nImgSize], pImgs, my_nImgSize);
				KizuMMX::mem_copy_64xmm(pImgs, my_nFrameW, &pImgs[kk*my_nImgSize], my_nFrameW, my_nFrameW, my_nFrameH);
			}

		}

		LOG(em_MSG), "->Fno=%d", my_nFrameNum);


		//// 全長処理に送信要求を登録
		int nCamId = ii + 1;
		OnAddCull(nCamId);


	}

	//// 次のため
	my_nFrameNum++;
	my_nSmpNowNo++;
	if (my_nSmpNowNo >= my_nSmpIntervalNo[1]) my_nSmpNowNo = my_nSmpIntervalNo[0];		// 一周まわったから最初の画像を取得
}
//------------------------------------------
// 1カメラセット分の画像を取得して、スレッドループキューにキューイング
// void** pImgs 画像(カメラ角度分)
// DWORD nBytes 画像サイズ (1コだけ)
//------------------------------------------
bool TestManager::OnManualFrameGrabbed(BYTE* pImgsBuf, DWORD nBytes)
{
	// データコピー
	for (int ii = 0; ii<my_nCamsetNum; ii++) {
		int nCamId = ii + 1;
		for (DWORD jj = 0; jj < my_nAngleNum; jj++) {
			//memcpy(&pImgs[0], &pImgsBuf[jj*nBytes], nBytes);
			KizuMMX::mem_copy_64xmm(&pImgsBuf[jj*nBytes], nBytes, &pImgs[0], nBytes, nBytes, 1);

			// チャンネル数分ループ。色差対応
			for (int kk = 1; kk < my_nChannel; kk++) {
				//memcpy(&pImgs[kk*nBytes], pImgs, nBytes);
				KizuMMX::mem_copy_64xmm(pImgs, nBytes, &pImgs[kk*nBytes], nBytes, nBytes, 1);
			}
															
			OnAddCull(nCamId);				//// 全長処理に送信要求を登録
		}
	}

	//// 次のため
	my_nFrameNum++;
	my_nSmpNowNo++;
	return true;
}

//------------------------------------------
// 全長保存へ転送通知
// int nCamId カメラNo
//------------------------------------------
void TestManager::OnAddCull(int nCamId)
{
	ParamManager::P_COMMON_MAST const& prm = mcls_pParam->GetCommonMast();

	CullManager::DELI* pDeli;

	// 生成サイズ決定
	int nDeliSize = sizeof(CullManager::DELI) - sizeof(BYTE);
	for (int ii = 0; ii < MAX_IMAGE_CNT; ii++) {
		if (TRUE == prm.nImgSave[ii]) {
			nDeliSize += my_nImgSize;
		}
	}

	// 生成
	BYTE* pWk = (BYTE*)_aligned_malloc(nDeliSize, 16);//new BYTE[nDeliSize];
	pDeli = (CullManager::DELI*)pWk;

	// セット
	pDeli->nFrameSkip = 0;
	pDeli->nCamId = (nCamId - 1) % NUM_CAM_POS;
	pDeli->nFrameNo = my_nFrameNum;
	pDeli->dEdgePos[0] = -500;
	pDeli->dEdgePos[1] = 500;

	pDeli->nWidth = my_nFrameW;
	pDeli->nHeight = my_nFrameH;
	pDeli->nStride = my_nFrameW;
	pDeli->nChannel = 0;

	int nOffset = 0;
	for (int ii = 0; ii < MAX_IMAGE_CNT; ii++) {
		if (TRUE == prm.nImgSave[ii]) {
			pDeli->nImgSize[ii] = pDeli->nStride * pDeli->nHeight;
			//memcpy( &pDeli->pImg[nOffset], &pImgs[(my_nImgSize*ii)], my_nImgSize);
			KizuMMX::mem_copy_64xmm(&pImgs[(my_nImgSize*ii)], pDeli->nStride, &pDeli->pImg[nOffset], pDeli->nStride, pDeli->nStride, pDeli->nHeight);
			nOffset += pDeli->nStride * pDeli->nHeight;
			pDeli->nChannel++;
		}
		else {
			pDeli->nImgSize[ii] = 0;
		}
	}


	// 依頼
	if (!mcls_pCull[nCamId-1]->gque_Deli.AddToTailFreeCheck(pDeli)) {
		delete pDeli;
		LOG(em_ERR), "[%s] RAW間引き依頼 キューフル!!", my_sThreadName);
	}


	// 連続稼動時は、次コイル情報事前通知を送信
	if(0 < my_nChangeFrm && 1 == nCamId) {
		if ((my_nChangeFrm - 1) == (my_nFrameNum % my_nChangeFrm)) {
			Send_TiRpisave_NextCoil(my_nFrameNum + 1);
		}
	}

}

//------------------------------------------
// 全長保存へ次コイル情報事前通知
// int nFrmNo フレームNo
//------------------------------------------
void TestManager::Send_TiRpisave_NextCoil(int nFrmNo)
{

	COMMON_QUE que;
	que.nEventNo = FACT_TI_RPISAVE_04;		// 次コイル情報事前通知 
	que.nLineNo = LINE_ID;
	que.mix.idata[0] = -1;					// 対象インスタンス
	que.mix.idata[1] = 1;					// 検査パターン(表)
	que.mix.idata[2] = 1;					// 検査パターン(裏)
	que.mix.idata[3] = 1;					// 検査表面状態(表)
	que.mix.idata[4] = 1;					// 検査表面状態(裏)
	que.mix.idata[5] = nFrmNo;				// 実行フレームNo
	que.mix.idata[6] = 10;					// 先頭長手位置[mm]

	KizuFunction::GetKizukenNo(que.mix.cdata[0]);			// 管理No
	sprintf(que.mix.cdata[1], "COILNO%03d", nFrmNo % 100);	// コイルNo

	send_mail(TI_RPISAVE, "*", &que);
}
