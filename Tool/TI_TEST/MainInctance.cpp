#include "StdAfx.h"
#include "MainInctance.h"

#include "TI_TEST.h"			// 画面の部品に直接アクセスするため


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
MainInctance::MainInctance(void) :
m_nCamPair(0),
m_nMen(0),
m_emKindHt(DIV_HANTEI_HT),
m_bNgCam(false),
m_nAppParam(0),
m_nSysMode(0),
ThreadManager("MI")
{
	// iniファイルからカメラ台数、カメラ開始位置を取得
	// 以降の全長保存との接続数などになる値
	g_nNumCam = GetPrivateProfileInt(TI_TEST, "NUM_CAM", NUM_CAM_POS, TASKINI_NAME);
	g_nCamStart = GetPrivateProfileInt(TI_TEST, "CAM_START", 1, TASKINI_NAME);

	for (int ii=0; ii<NUM_CAM_POS; ii++) {
	//for (int ii=0; ii<FRAME_HT_SEND_PORT; ii++) {
		gcls_pSockRpi[ii] = NULL;
		gque_pSockRpi[ii] = NULL;
		gcls_pCull[ii] = NULL;
	}
	
	for (int ii=0; ii<NUM_MEN; ii++) {
		m_nNowScode[ii] = MAX_SCODE;
	}

	// シグナル生成
	m_evThStart = CreateEvent(NULL, FALSE, FALSE, NULL);				// メインスレッド同期用
	m_evKenStart = CreateEvent(NULL, FALSE, FALSE, NULL);				// メインスレッド同期用
	m_evKenStop = CreateEvent(NULL, FALSE, FALSE, NULL);				// メインスレッド同期用

	m_evTimerExec = CreateWaitableTimer(NULL, FALSE, NULL);				// 自動検査開始タイマーイベント
}

//------------------------------------------
// デストラクタ
//------------------------------------------
MainInctance::~MainInctance(void)
{
	// シグナル開放
	CloseHandle(m_evThStart);
	CloseHandle(m_evKenStart);
	CloseHandle(m_evKenStop);

	// 定周期停止
	CancelWaitableTimer(m_evTimerExec);
	SAFE_DELETE_HANDLE(m_evTimerExec);
}

//------------------------------------------
// スレッドキューにメールデータ登録
// ENUM_MAININCTANCE emNo メッセージNo (イベントNoそのままにしようかと思ったが、FACT??_01とかだと分かりにくいからやめた)
// COMMON_QUE* que メールスロット構造体そのもの
//------------------------------------------
void MainInctance::SetDeliveryMail(ENUM_MAININCTANCE emNo, COMMON_QUE* que)
{
	DELIVERY_MAIL* pMail = new DELIVERY_MAIL;
	pMail->emNo	= emNo;
	memcpy( &pMail->que, que, sizeof(COMMON_QUE));

	// スレッドキューに登録
	if( ! mque_DeliMail.AddToTailFreeCheck(pMail) ) {
		delete pMail;
		LOG(em_ERR), "[%s] mque_DeliMail キューフル!!", my_sThreadName);
	}
}

//------------------------------------------
// 初期化処理 (一度のみ起動)
//------------------------------------------
BOOL MainInctance::Init()
{
	//// メインインスタンスのみ、スレッドだけは作っておく (すぐに非シグナルとする)
	//// 継承元クラス実行 (メイン制御スレッドの為、ちょっと優先度高め)
	ThreadManager::Start(THREAD_PRIORITY_ABOVE_NORMAL );

	// スレッドIDが割りつくまでちょっと休憩
	while( 0==GetTid() ) {
		LOG(em_MSG), "[%s] スレッドID割り当て中・・・", my_sThreadName);
		Sleep(1);			// スレッド起動までちょっと待ち
	}


	////////////////////////////////////
	//// パラメータ読込待ち

	// パラメータクラス
	gcls_PrmMgr.SetLogMgr(mcls_pLog); 
	while( true ) {
		if( gcls_PrmMgr.ReadParam_All() ) break;
		LOG(em_MSG), "[%s] パラメータクラス パラメータ読込失敗(DB接続異常？！)。リトライ中・・・", my_sThreadName);
		Sleep(5000);
	}

	////////////////////////////////////
	//// 全長画像関係
#ifdef FRAME_RAW_SEND
	for (int ii=0; ii<NUM_CAM_POS; ii++) {				// 試験ツールのため、幅方向全カメラ
	//for (int ii=0; ii<FRAME_HT_SEND_PORT; ii++) {
		// バッファ確保
		// サイズが大きすぎる？？　MAX_BUF_QUE_RPISOCKを仮に２に変更
		//gque_pSockRpi[ii] = new ThreadLoopQueue<HT_RPI_FDATA>(MAX_BUF_QUE_RPISOCK);
		//int n[1] = { SIZE_FRAME_IMG_X * SIZE_FRAME_IMG_Y * MAX_IMAGE_CNT };
		//QueueAlloc(ii, MAX_BUF_QUE_RPISOCK, n);
		int nMaxBufQueRpisock = 4;
		gque_pSockRpi[ii] = new ThreadLoopQueue<HT_RPI_FDATA>(nMaxBufQueRpisock);
		int n[1] = { (SIZE_FRAME_IMG_X * SIZE_FRAME_IMG_Y * MAX_IMAGE_CNT) / 4 };
		//QueueAlloc(ii, MAX_BUF_QUE_RPISOCK, n);
		QueueAlloc(ii, nMaxBufQueRpisock, n);
		// 送受信スレッド起動
		gcls_pSockRpi[ii] = new SockOneToAnySend();
		gcls_pSockRpi[ii]->SetLogMgr(mcls_pLog);
		gcls_pSockRpi[ii]->SetTLQue((ThreadLoopQueue<SOCK_F_HEAD2>*) gque_pSockRpi[ii]);
		////gcls_pSockRpi[ii]->StartLock(THREAD_PRIORITY_TIME_CRITICAL);

		// 間引きクラス
		gcls_pCull[ii] = new CullManager();
		gcls_pCull[ii]->SetLogMgr(mcls_pLog);
		gcls_pCull[ii]->SetLogMgrCull(mcls_pLogCull);
		gcls_pCull[ii]->SetParamMgr(&gcls_PrmMgr);
		gcls_pCull[ii]->SetRpiToSend(gcls_pSockRpi[ii]);
		gcls_pCull[ii]->SetQueSockRpi(gque_pSockRpi[ii]);
		////gcls_pCull[ii]->StartLock();

	}
#endif





	////////////////////////////////////
	//// ワーカースレッド 初期化
	// テスト管理
	gcls_pTest = new TestManager(g_nNumCam, g_nCamStart);
	gcls_pTest->SetLogMgr(mcls_pLog);
	gcls_pTest->SetParamMgr(&gcls_PrmMgr);
	gcls_pTest->SetCullMgr(gcls_pCull);
	gcls_pTest->Alloc();
	// 試験用初期設定
	gcls_pTest->SetChangeFrm(2000);


	////////////////////////////////////
	//// ワーカースレッド 起動
	gcls_PrmMgr.StartLock();
	gcls_pTest->Start();

	////////////////////////////////////
	//// 初期状態

	int pcid = getpcid();
	return TRUE;
}

//------------------------------------------
// 終了処理 (一度のみ起動)
//------------------------------------------
void MainInctance::Exit()
{
	//// ワーカースレッド 停止
	gcls_PrmMgr.Stop();
	gcls_pTest->Free();
	gcls_pTest->Stop();

	//// ワーカースレッド 開放
	delete gcls_pTest;

	////////////////////////////////////
	// 全長画像関係
#ifdef FRAME_RAW_SEND
	QueueFree();

	for (int ii=0; ii<NUM_CAM_POS; ii++) {
	//for (int ii=0; ii<FRAME_HT_SEND_PORT; ii++) {
		if( gcls_pSockRpi[ii]->IsThreadEvOn() ) gcls_pSockRpi[ii]->Stop();
		if( gcls_pCull[ii]->IsThreadEvOn()) gcls_pCull[ii]->Stop();

		delete gque_pSockRpi[ii];
		delete gcls_pSockRpi[ii];
		delete gcls_pCull[ii];
	}
#endif
}

//------------------------------------------
// スレッド開始前処理 (スレッド実行直後(シグナルセット前)にコールバック)
//------------------------------------------
int MainInctance::ThreadFirst()
{
	////// 他ワークスレッドの準備が整うまで待機 (MainInctanceのみ)
	LOG(em_MSG), "[%s] 他ワークスレッドの準備が整うまで待機中・・・ [0x%X]", my_sThreadName, GetTid() );
	WaitForSingleObject(m_evThStart, INFINITE);


	HANDLE h[NUM_CAM_POS];
	//// イベント設定
	AddEventNum(1, &mque_DeliMail.g_evSem);
	AddEventNum(1, &m_evKenStart);
	AddEventNum(1, &m_evKenStop);
	AddEventNum(1, &m_evTimerExec);

	// 全長
#ifdef FRAME_RAW_SEND
	for (int ii=0; ii<NUM_CAM_POS; ii++) h[ii] = gcls_pSockRpi[ii]->GetEvConnect();
	AddEventNum(NUM_CAM_POS, h);		// 全長接続
	//for (int ii=0; ii<FRAME_HT_SEND_PORT; ii++) h[ii] = gcls_pSockRpi[ii]->GetEvConnect();	
	//AddEventNum(FRAME_HT_SEND_PORT, h);		// 全長接続
#endif

	// タイマーセット
	LOG(em_MSG), "[%s] 自動検査開始タイマーセット [%d秒後]", my_sThreadName, (PARAM_TIMEOUT_AUTO_START/1000) );
	__int64 ts = PARAM_TIMEOUT_AUTO_START * -10000;		// 1ミリ秒→100ナノ秒 (マイナス指定で、指定時間経過でシグナル) (第3引数は2回目以降の起動間隔)
	int nCycleTime = 0;							// 定周期間隔
	SetWaitableTimer(m_evTimerExec, (LARGE_INTEGER *)&ts, nCycleTime, NULL, NULL, FALSE);

	return 0;
}

//------------------------------------------
// スレッド終了前処理 (終了シグナル後にコールバック)
//------------------------------------------
int MainInctance::ThreadLast()
{
	return 0;
}

//------------------------------------------
// スレッドイベント発生
// int nEventNo イベントNo (0オリジン)
//------------------------------------------
void MainInctance::ThreadEvent(int nEventNo)
{
	DELIVERY_MAIL* pMail;
	//int retc;

	////// シグナルの条件
	enum {	EV_MAIL = 0,					// メールスロット 通知 (メールは一番最後にしようかな)

			EV_KEN_START,					// 検査開始
		    EV_KEN_STOP,					// 検査停止
			EV_TIMER_START,					// 自動検査開始

#ifdef FRAME_RAW_SEND
			EV_RPI_SOCK_CONECT,				// 対全長 接続 (FRAME_HT_SEND_PORT分)
#endif
	};

	////// シグナル条件分け
	switch (nEventNo) {

//-----------------------------------------------------------------------------------------------
	case EV_MAIL: 						// メールスロット 通知
	{
		pMail = mque_DeliMail.GetFromHead();
		switch(pMail->emNo) {

//-----------------------------------------------------------
		case E_DEF_ML_PARAM :									// パラメータ変更要求 
			break;	

//-----------------------------------------------------------
		}
		// 開放
		delete pMail;
		break;
	}

//-----------------------------------------------------------
	case EV_KEN_START: 					// 検査開始
	//---------------------------------------
	//// 全長処理と接続
		if (true) {
#ifdef FRAME_RAW_SEND
			if (1 == m_nSysMode) {
				LOG(em_INF), "[%s] 検査開始中に開始要求のため、破棄", my_sThreadName);
				break;
			}

			CancelWaitableTimer(m_evTimerExec);			// 自動開始要求のタイマー停止

			CString				sWk;

			for (int ii = 0; ii < NUM_CAM_POS; ii++) {
				//for (int ii=0; ii<FRAME_HT_SEND_PORT; ii++) {
				gcls_pSockRpi[ii]->StartLock(THREAD_PRIORITY_TIME_CRITICAL);
				gcls_pCull[ii]->StartLock();
			}
			Sleep(100);

			for (int ii = 0; ii < NUM_CAM_POS; ii++) {
				//for (int ii=0; ii<FRAME_HT_SEND_PORT; ii++) {

				if (ii == (g_nCamStart - 1)) {
					for (int jj = g_nCamStart-1; jj < ((g_nCamStart-1)+g_nNumCam); jj++) {

						int nId = (jj) % NUM_CAM_POS;
						sWk.Format("%s_RPI_TO_%d_%d", HT_DEFECT, m_nMen, nId);
						gcls_pSockRpi[jj]->SetSession(sWk);
						gcls_pSockRpi[jj]->SetRetryMode(true);
						gcls_pSockRpi[jj]->SetConnect();
					}
					break;
				}
			}
			m_nSysMode = 1;	// 開始
#endif
		}

		break;

//-----------------------------------------------------------
	case EV_KEN_STOP: 					// 検査停止
		//---------------------------------------
		// 全長処理 切断
		if (true) {

#ifdef FRAME_RAW_SEND
			if (0 == m_nSysMode) {
				LOG(em_INF), "[%s] 検査停止中に停止要求のため、破棄", my_sThreadName);
				break;
			}

			for (int ii = 0; ii < NUM_CAM_POS; ii++) {
				//for (int ii=0; ii<FRAME_HT_SEND_PORT; ii++) {

				if (gcls_pSockRpi[ii]->IsThreadEvOn()) {
					gcls_pSockRpi[ii]->SetRetryMode(false);		// リトライはしたくない
					gcls_pSockRpi[ii]->SetDisConnect();
				}
			}
			Sleep(1000);		// 非同期で全長処理切断しているため、切断される前にスレッド停止されてしまうと異常終了となる。ちょっと長めに待つ
			//Sleep(300);
			for (int ii = 0; ii < NUM_CAM_POS; ii++) {
				//for (int ii=0; ii<FRAME_HT_SEND_PORT; ii++) {		
				if (gcls_pSockRpi[ii]->IsThreadEvOn()) gcls_pSockRpi[ii]->Stop();
				if (gcls_pCull[ii]->IsThreadEvOn()) gcls_pCull[ii]->Stop();
			}
			m_nSysMode = 0;	// 停止
#endif
		}
		break;

//-----------------------------------------------------------
	case EV_TIMER_START: 				// 自動検査開始
		if (true) {
			// 開始イベント
			SetEvKenStart();
		}
		break;

//-----------------------------------------------------------
#ifdef FRAME_RAW_SEND
	case EV_RPI_SOCK_CONECT + 0:									// 対全長 接続
		LOG(em_MSG), "[%s] 対全長1  接続完了", my_sThreadName);
		break;
	case EV_RPI_SOCK_CONECT + 1:									// 対全長 接続
		LOG(em_MSG), "[%s] 対全長2  接続完了", my_sThreadName);
		break;
	case EV_RPI_SOCK_CONECT + 2:									// 対全長 接続
		LOG(em_MSG), "[%s] 対全長3  接続完了", my_sThreadName);
		break;
	case EV_RPI_SOCK_CONECT + 3:									// 対全長 接続
		LOG(em_MSG), "[%s] 対全長4  接続完了", my_sThreadName);
		break;
	case EV_RPI_SOCK_CONECT + 4:									// 対全長 接続
		LOG(em_MSG), "[%s] 対全長5  接続完了", my_sThreadName);
		break;
	case EV_RPI_SOCK_CONECT + 5:									// 対全長 接続
		LOG(em_MSG), "[%s] 対全長6  接続完了", my_sThreadName);
		break;
	case EV_RPI_SOCK_CONECT + 6:									// 対全長 接続
		LOG(em_MSG), "[%s] 対全長7  接続完了", my_sThreadName);
		break;
	case EV_RPI_SOCK_CONECT + 7:									// 対全長 接続
		LOG(em_MSG), "[%s] 対全長8  接続完了", my_sThreadName);
		break;
	case EV_RPI_SOCK_CONECT + 8:									// 対全長 接続
		LOG(em_MSG), "[%s] 対全長9  接続完了", my_sThreadName);
		break;
	case EV_RPI_SOCK_CONECT + 9:									// 対全長 接続
		LOG(em_MSG), "[%s] 対全長10 接続完了", my_sThreadName);
		break;
#endif

//-----------------------------------------------------------------------------------------------
	default :
		// ありえない！！
		LOG(em_ERR), "[%s] ThreadEvent EventNo NG=%d", my_sThreadName, nEventNo );
		return;
	}
}

// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
#pragma region // 全長

//------------------------------------------
// 全長処理送信用の回転寿司キュー確保
// int n		キュー数
// int const* p	受渡情報
//					[0] 受渡画像最大サイズ
//------------------------------------------
void MainInctance::QueueAlloc(int idx, int n, int const* p)
{	
	HT_RPI_FDATA *pD = NULL;

	//// 空の器を作成
	for (int ii = 0; ii < n; ii++) {
		pD = new HT_RPI_FDATA;
		pD->Alloc(p);
		gque_pSockRpi[idx]->AddFree(pD);			// 器を追加
	}
	
	LOG(em_MSG),"[%s] 全長処理<%d> 送信キュー%d件確保 [%dMB]", my_sThreadName, idx, n, (__int64)p[0] * n / 1024 / 1024);
	return ;
}

//------------------------------------------
// 全長処理送信用の回転寿司キュー解放
//------------------------------------------
void MainInctance::QueueFree()
{	
	HT_RPI_FDATA *pD = NULL;
	int nPoolNum = 0;
	int nFreeNum = 0;

	for (int ii=0; ii<NUM_CAM_POS; ii++) {
	//for (int ii=0; ii<FRAME_HT_SEND_PORT; ii++) {
		// 具有キュー
		while(true) {
			pD = gque_pSockRpi[ii]->GetPool();
			if( NULL == pD) break;
			pD->Free();
			delete pD;
			pD = NULL;
			nPoolNum ++;
		}
		// 空きキュー
		while(true) {
			pD = gque_pSockRpi[ii]->GetFree();
			if( NULL == pD) break;
			pD->Free();
			delete pD;
			pD = NULL;
			nFreeNum ++;
		}
	}
	LOG(em_MSG),"[%s] 全長処理 送信キュー解放 [PoolQue=%d; FreeQue=%d]", my_sThreadName, nPoolNum, nFreeNum );
}

#pragma endregion


//------------------------------------------
// 全長処理試験用ツール連動起動
//------------------------------------------
void MainInctance::Send_TiTest_Rendo(int nCycle)
{
	// 試験用 周期開始/停止
	COMMON_QUE que;
	que.nEventNo = FACT_TI_TEST_01;
	que.nLineNo = LINE_ID;
	que.fl.data[0] = nCycle;									// 周期

	for (int ii = 0; ii < NUM_CAM_POS; ii++)
	{
		CString sTaskName;
		sTaskName.Format("%s%02d", TI_TEST, (ii + 1));
		if (0 == strcmp(sTaskName, getmytaskname())) continue;
		send_mail(sTaskName, "*", &que);

		LOG(em_MSG), "[%s] 全長処理 連動起動 tn:%s Fact:%d %d", my_sThreadName, sTaskName, que.nEventNo, que.fl.data[0] );
	}

	return;
}

//------------------------------------------
// 全長処理試験用ツール連動起動
//------------------------------------------
void MainInctance::Send_TiTest_FrameNo(int nFrameNo, int nCycleTime)
{
	// 試験用 周期開始/停止
	COMMON_QUE que;
	que.nEventNo = FACT_TI_TEST_02;
	que.nLineNo = LINE_ID;
	que.fl.data[0] = nFrameNo;									// フレームNo
	que.fl.data[1] = nCycleTime;								// 周期時間[ms]

	for (int ii = 0; ii < NUM_CAM_POS; ii++)
	{
		CString sTaskName;
		sTaskName.Format("%s%02d", TI_TEST, (ii + 1));
		if (0 == strcmp(sTaskName, getmytaskname())) continue;
		send_mail(sTaskName, "*", &que);

		LOG(em_MSG), "[%s] 全長処理 連動起動 tn:%s Fact:%d fl[%d][%d]", my_sThreadName, sTaskName, que.nEventNo, que.fl.data[0], que.fl.data[1] );
	}

	return;
}


