#include "StdAfx.h"
#include "MainInctance.h"

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
ThreadManager("MI")
{
	// シグナル生成
	m_evThStart  = CreateEvent(NULL, FALSE, FALSE, NULL);				// メインスレッド同期用
	// 初期化
	for(int ii=0; ii<MAX_THREAD; ii++) gcls_pDmLedMgr[ii] = NULL;
}
//------------------------------------------
// デストラクタ
//------------------------------------------
MainInctance::~MainInctance(void)
{
	// シグナル開放
	CloseHandle(m_evThStart);
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
	//// ワーカースレッド 初期化
	////////////////////////////////////
	int nBlock		= GetPrivateProfileInt(KT_LEDMGR, "BLOCK", 28, TASKINI_NAME);
	int nTempUnit	= GetPrivateProfileInt(KT_LEDMGR, "TEMP_UNIT", 3, TASKINI_NAME);
	//int nTempThr	= GetPrivateProfileInt(KT_LEDMGR, "TEMP_THR", 60, TASKINI_NAME);
	//int nTempThrDown= GetPrivateProfileInt(KT_LEDMGR, "TEMP_THR_DOWN", 100, TASKINI_NAME);
	//int nTimerCycle = GetPrivateProfileInt(KT_LEDMGR, "TIMER_CYCLE", 180, TASKINI_NAME);

	CString sWk;
	char	cKey[64];
	for(int ii=0; ii<MAX_THREAD; ii++) {
		// キーチェック
		sWk.Format("NAME_%d", ii+1);
		GetPrivateProfileString(KT_LEDMGR, sWk, "", cKey, sizeof(cKey), TASKINI_NAME);
		if(0 == strlen(cKey) ) break;

		// インスタンス生成
		gcls_pDmLedMgr[ii] = new DummyLedManager(cKey, ii, nBlock, nTempUnit);
		gcls_pDmLedMgr[ii]->SetLogMgr(mcls_pLog);
	}

	////////////////////////////////////
	//// ワーカースレッド 起動
	////////////////////////////////////

	for(int ii=0; ii<MAX_THREAD; ii++) {
		if(NULL == gcls_pDmLedMgr[ii]) break;
		gcls_pDmLedMgr[ii]->Start();
	}


	return TRUE;
}

//------------------------------------------
// 終了処理 (一度のみ起動)
//------------------------------------------
void MainInctance::Exit()
{
	//// ワーカースレッド停止
	for(int ii=0; ii<MAX_THREAD; ii++) {
		if(NULL == gcls_pDmLedMgr[ii]) continue;
		gcls_pDmLedMgr[ii]->Stop(500);
	}

	//// ワーカースレッド解放
	for(int ii=0; ii<MAX_THREAD; ii++) {
		if(NULL == gcls_pDmLedMgr[ii]) continue;
		SAFE_DELETE( gcls_pDmLedMgr[ii] );
	}
}

//------------------------------------------
// スレッド開始前処理 (スレッド実行直後(シグナルセット前)にコールバック)
//------------------------------------------
int MainInctance::ThreadFirst()
{
	////// 他ワークスレッドの準備が整うまで待機 (MainInctanceのみ)
	LOG(em_MSG), "[%s] 他ワークスレッドの準備が整うまで待機中・・・ [0x%X]", my_sThreadName, GetTid() );
	WaitForSingleObject(m_evThStart, INFINITE);

//*/
	//// イベント設定
	HANDLE hArray[] = { mque_DeliMail.g_evSem
						};
	SetEventNum(sizeof(hArray)/sizeof(HANDLE), hArray);
//*/
	return 0;
}

//------------------------------------------
// スレッドイベント発生
// int nEventNo イベントNo (0オリジン)
//------------------------------------------
void MainInctance::ThreadEvent(int nEventNo)
{
	DELIVERY_MAIL* pMail;

	////// シグナルの条件
	enum {	EV_MAIL = 0,				// メールスロット 通知 (メールは一番最後にしようかな)
	};

	////// シグナル条件分け
	switch (nEventNo) {

//-----------------------------------------------------------------------------------------------
	case EV_MAIL: 						// メールスロット 通知
		pMail = mque_DeliMail.GetFromHead();
		switch(pMail->emNo) {

//-----------------------------------------------------------
		case 1:
			break;


//-----------------------------------------------------------
		}
		// 開放
		delete pMail;
		return;

//-----------------------------------------------------------------------------------------------
	default :
		// ありえない！！
		LOG(em_ERR), "[%s] ThreadEvent EventNo NG=%d", my_sThreadName, nEventNo );
		return;
	}
}

