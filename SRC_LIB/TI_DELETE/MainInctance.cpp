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
g_nClassCnt(0),
mcls_pLog(NULL)
{
	//// シグナル生成
	m_evThStart  = CreateEvent(NULL, FALSE, FALSE, NULL);				// メインスレッド同期用


	//// 各クラス関連初期化
	for(int ii=0; ii<CONTROL_MAXCNT; ii++) {
		gcls_pContMgr[ii] = NULL;
	}
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
// 初期化処理 (一度のみ起動)
//------------------------------------------
BOOL MainInctance::Init()
{
	//// メインインスタンスのみ、スレッドだけは作っておく (すぐに非シグナルとする)
	//// 継承元クラス実行 (メイン制御スレッドの為、ちょっと優先度高め)
	WorkThread::Start(MainThread, THREAD_PRIORITY_ABOVE_NORMAL );

	// スレッドIDが割りつくまでちょっと休憩
	while( 0==GetTid() ) {
		LOG(em_MSG), "[MainInctance] スレッドID割り当て中・・・");
		Sleep(1);			// スレッド起動までちょっと待ち
	}

	//// パフォーマンス管理スレッド
	gcls_ContDefrag.SetLogMgr(mcls_pLog);


	//// iniファイル読み込み 
	char		cWk[1024];
	CString		strVal;
	g_nClassCnt = 0;
	for(int ii=0; ii<CONTROL_MAXCNT; ii++) {
		strVal.Format("SESSION_%d", ii+1);
		GetPrivateProfileString(TI_DELETE, strVal, "", cWk, sizeof(cWk), TASKINI_NAME);		
		if( 0 >= strlen(cWk) ) break;

		// ワーカースレッド 初期化
		gcls_pContMgr[g_nClassCnt] = new ControlManager(cWk, mcls_pLog);
		g_nClassCnt++;

		// パフォーマンス管理 子スレッド
		gcls_ContDefrag.WorkerNew(ii+1, cWk); 
	}


	//// ワーカースレッド 起動
	for(int ii=0; ii<g_nClassCnt; ii++) {
		gcls_pContMgr[ii]->WorkerInit();
		gcls_pContMgr[ii]->Start();
	}

	//// 無害Gr削除
	gcls_pContLowGr = new ControlLowGr();
	gcls_pContLowGr->SetLogMgr(mcls_pLog); 
	if( gcls_pContLowGr->IsEnable() ) {
		// 有効な場合のみ
		gcls_pContLowGr->WorkerInit();
		gcls_pContLowGr->Start();
	}

	//// パフォーマンス管理
	gcls_ContDefrag.Start();

	return TRUE;
}

//------------------------------------------
// 終了処理 (一度のみ起動)
//------------------------------------------
void MainInctance::Exit()
{
	//// コントロールスレッド 停止
	for(int ii=0; ii<g_nClassCnt; ii++) {
		gcls_pContMgr[ii]->Stop();
	}

	//// ワーカースレッド 停止
	// まず全体に停止指示を通知する
	for(int ii=0; ii<g_nClassCnt; ii++) {
		gcls_pContMgr[ii]->WorkerStop();
	}
	//// 無害Gr削除
	if( gcls_pContLowGr->IsEnable() ) {
		gcls_pContLowGr->Stop();
		gcls_pContLowGr->WorkerStop();
	}
	//// パフォーマンス管理
	gcls_ContDefrag.WorkerStop(); 

	Sleep(1000);

	//// ワーカースレッド 開放
	for(int ii=0; ii<g_nClassCnt; ii++) {
		gcls_pContMgr[ii]->WorkerExit();
	}

	//// コントロールスレッド 開放
	for(int ii=0; ii<g_nClassCnt; ii++) {
		SAFE_DELETE(gcls_pContMgr[ii]);
	}

	//// 無害Gr削除 (無条件に実行でOK)
	gcls_pContLowGr->WorkerExit();
	SAFE_DELETE(gcls_pContLowGr);

	//// パフォーマンス管理
	gcls_ContDefrag.Stop();
}


//------------------------------------------
// 削除/書込 優先モード (true:削除優先)
// bool mode True：削除優先 False:書込優先
//------------------------------------------
void MainInctance::SetWaitMode(bool mode)
{
	for(int ii=0; ii<g_nClassCnt; ii++) {
		gcls_pContMgr[ii]->SetWaitMode(mode);
	}
}


//------------------------------------------
// スレッド
//------------------------------------------
UINT WINAPI MainInctance::MainThread(LPVOID param)
{
	MSG	msg;								// MSG構造体
	
	////// メインインスタンス取り出し
	MainInctance *p = (MainInctance *)param;						// 自クラスのインスタンスを取得
	PeekMessage(&msg, NULL, WM_USER, WM_USER, PM_NOREMOVE);			// メッセージ作成
	p->SetTid();													// スレッドIDセット


	////// 他ワークスレッドの準備が整うまで待機
	pLOG(em_MSG), "[MainInctance] 他ワークスレッドの準備が整うまで待機中・・・ [0x%X]", p->GetTid() );
	WaitForSingleObject(p->m_evThStart, INFINITE);


	////// メッセージキュー読み込み
	pLOG(em_MSG), "[MainInctance] スレッド開始 [0x%X]", p->GetTid());
	while( int ret = GetMessage(&msg, NULL, 0, 0) ){
		_ASSERT( -1 != ret );
		switch (msg.message) {

//-----------------------------------------------------------------------------------------------
		case WM_USER_TH_END: 						// スレッド終了要求
			pLOG(em_MSG), "[MainInctance] スレッド終了 [0x%X]", p->GetTid());

			return 0;

		default :
			// ありえない！！
			break;
		}
	}

	// ありえない
	pLOG(em_ERR), "[MainInctance] メインループから抜けた！！[0x%X]", p->GetTid());
	return 0;
}
