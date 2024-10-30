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
	// PLG
	gcls_pPlg = new PlgControlManager();
	gcls_pPlg->SetLogMgr(mcls_pLog);
	// PIO
	gcls_pPio = new PioControlManager();
	gcls_pPio->SetLogMgr(mcls_pLog);
	gcls_pPio->SetPlgCls(gcls_pPlg);

	// シミュレータ管理
	for(int ii=0; ii<SMYU_STORY_MAX_FILE; ii++) {
		gcls_pSmyuPlg[ii] = new SmyuPlgManager(ii);
		gcls_pSmyuPlg[ii]->SetLogMgr(mcls_pLog);
		gcls_pSmyuPlg[ii]->SetPlgMgr(gcls_pPlg);
		gcls_pSmyuPlg[ii]->SetPioMgr(gcls_pPio);
	}
	for(int ii=0; ii<SMYU_STORY_MAX_FILE; ii++) {
		gcls_pSmyuTim[ii] = new SmyuTimManager(ii);
		gcls_pSmyuTim[ii]->SetLogMgr(mcls_pLog);
		gcls_pSmyuTim[ii]->SetPlgMgr(gcls_pPlg);
		gcls_pSmyuTim[ii]->SetPioMgr(gcls_pPio);
	}
	gcls_pSmyuEvt = new SmyuEvtManager();
	gcls_pSmyuEvt->SetLogMgr(mcls_pLog);
	gcls_pSmyuEvt->SetPlgMgr(gcls_pPlg);
	gcls_pSmyuEvt->SetPioMgr(gcls_pPio);


	////////////////////////////////////
	//// ワーカースレッド 起動
	////////////////////////////////////
	gcls_pPlg->Start(THREAD_PRIORITY_TIME_CRITICAL);
	gcls_pPio->Start();

	//// シナリオ設定 ＆ スレッドスタート
	ExecInit();

	return TRUE;
}

//------------------------------------------
// 終了処理 (一度のみ起動)
//------------------------------------------
void MainInctance::Exit()
{

	//// ワーカースレッド 停止
	gcls_pPlg->Stop();
	gcls_pPio->Stop();
	
	for(int ii=0; ii<SMYU_STORY_MAX_FILE; ii++) gcls_pSmyuPlg[ii]->Stop();
	for(int ii=0; ii<SMYU_STORY_MAX_FILE; ii++) gcls_pSmyuTim[ii]->Stop();
	gcls_pSmyuEvt->Stop();

	//// ワーカースレッド 開放
	delete gcls_pPlg;
	delete gcls_pPio;

	for(int ii=0; ii<SMYU_STORY_MAX_FILE; ii++) SAFE_DELETE( gcls_pSmyuPlg[ii] );
	for(int ii=0; ii<SMYU_STORY_MAX_FILE; ii++) SAFE_DELETE( gcls_pSmyuTim[ii] );
	SAFE_DELETE( gcls_pSmyuEvt );

}

//------------------------------------------
// スレッド開始前処理 (スレッド実行直後(シグナルセット前)にコールバック)
//------------------------------------------
int MainInctance::ThreadFirst()
{
	////// 他ワークスレッドの準備が整うまで待機 (MainInctanceのみ)
	LOG(em_MSG), "[%s] 他ワークスレッドの準備が整うまで待機中・・・ [0x%X]", my_sThreadName, GetTid() );
	WaitForSingleObject(m_evThStart, INFINITE);

	//// イベント設定
	HANDLE hArray[] = { mque_DeliMail.g_evSem
						};
	SetEventNum(sizeof(hArray)/sizeof(HANDLE), hArray);
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
		case E_DEF_ML_SMYU_START:
			ExecStart();
			break;

//-----------------------------------------------------------
		case E_DEF_ML_SMYU_STOP:
			ExecStop();
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

// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 処理
//------------------------------------------
// シナリオ設定
//------------------------------------------
void MainInctance::ExecInit()
{
	char	cWk[2048];
	CString cKey;

		
	//// 距離駆動
	for(int ii=0; ii<SMYU_STORY_MAX_FILE; ii++) {
		cKey.Format("CSV_PATH_PLG_%d", ii);

		GetPrivateProfileString(GOT_CONTROL, cKey, "", cWk, sizeof(cWk), TASKINI_NAME);
		if( 0 == strlen(cWk)) break;
		
		gcls_pSmyuPlg[ii]->SetCsvFilePath(cWk);
		gcls_pSmyuPlg[ii]->Start();
	}

	//// 時間駆動
	for(int ii=0; ii<SMYU_STORY_MAX_FILE; ii++) {
		cKey.Format("CSV_PATH_TIM_%d", ii);

		GetPrivateProfileString(GOT_CONTROL, cKey, "", cWk, sizeof(cWk), TASKINI_NAME);
		if( 0 == strlen(cWk)) break;
		
		gcls_pSmyuTim[ii]->SetCsvFilePath(cWk);
		gcls_pSmyuTim[ii]->Start();
	}

	//// イベント駆動
	cKey.Format("CSV_PATH_EVT_%d", 0);

	GetPrivateProfileString(GOT_CONTROL, cKey, "", cWk, sizeof(cWk), TASKINI_NAME);
	if( 0 != strlen(cWk)) {
		gcls_pSmyuEvt->SetCsvFilePath(cWk);
		gcls_pSmyuEvt->Start();
	}
}

//------------------------------------------
// シナリオ開始
//------------------------------------------
void MainInctance::ExecStart()
{
	//// 距離駆動
	for(int ii=0; ii<SMYU_STORY_MAX_FILE; ii++) {
		if( gcls_pSmyuPlg[ii]->IsThreadRun() ) gcls_pSmyuPlg[ii]->SmyuStart();
	}

	//// 時間駆動
	for(int ii=0; ii<SMYU_STORY_MAX_FILE; ii++) {
		if( gcls_pSmyuTim[ii]->IsThreadRun() ) gcls_pSmyuTim[ii]->SmyuStart();
	}

	//// イベント駆動
	if( gcls_pSmyuEvt->IsThreadRun() ) gcls_pSmyuEvt->SmyuStart();


	//// PLG距離管理
	gcls_pPlg->SmyuStart();
	for(int ii=0; ii<SMYU_PLG_MAX_CHANNEL; ii++) {
		gcls_pPlg->SetTimer_SpeedCycle(ii, true);
	}
}

//------------------------------------------
// シナリオ停止
//------------------------------------------
void MainInctance::ExecStop()
{
	//// PLG停止
	for(int ii=0; ii<SMYU_PLG_MAX_CHANNEL; ii++) {
		gcls_pPlg->SetTimer_SpeedCycle(ii, false);
	}

	//// 距離駆動
	for(int ii=0; ii<SMYU_STORY_MAX_FILE; ii++) {
		if( gcls_pSmyuPlg[ii]->IsThreadRun() ) gcls_pSmyuPlg[ii]->SmyuStop();
	}

	//// 時間駆動
	for(int ii=0; ii<SMYU_STORY_MAX_FILE; ii++) {
		if( gcls_pSmyuTim[ii]->IsThreadRun() ) gcls_pSmyuTim[ii]->SmyuStop();
	}

	//// イベント駆動
	if( gcls_pSmyuEvt->IsThreadRun() ) gcls_pSmyuEvt->SmyuStop();
}