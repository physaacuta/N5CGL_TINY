#include "StdAfx.h"
#include "MainInctance.h"

//------------------------------------------
// コンストラクタ
//------------------------------------------
MainInctance::MainInctance(void) :
mcls_pLog(NULL)
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

	//// ワーカースレッド 初期化
	gcls_pEvt = new EvtlogManager();
	gcls_pEvt->SetLogMgr(mcls_pLog);
	gcls_pEvt->SetMainPostTid(GetTid());


	//// ワーカースレッド 起動
	gcls_pEvt->Start();

	return TRUE;
}

//------------------------------------------
// 終了処理 (一度のみ起動)
//------------------------------------------
void MainInctance::Exit()
{
	//// ワーカースレッド 停止
	gcls_pEvt->Stop();

	//// ワーカースレッド 開放
	delete gcls_pEvt;
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
/*
//-----------------------------------------------------------------------------------------------
		case E_DEF_PCNAMESET: 					// PC名称変更通知
			pLOG(em_MSG), "[MainInctance] PC名称変更通知 [0x%X]", p->GetTid());
			// PC名称変更通知を受け取った時のメール通知処理を入れること 
			SetEvent(p->gcls_pEvtManager->m_evChangePcName);
			break;
//-----------------------------------------------------------------------------------------------
		case E_DEF_DATAREAD: 					// ユーザーによる強制チェック開始ボタン
			pLOG(em_MSG), "[MainInctance] ユーザーによる強制チェック開始ボタン [0x%X]", p->GetTid());
			SetEvent(p->gcls_pEvtManager->m_evhDataRead);
			break;
//-----------------------------------------------------------------------------------------------
*/
		case WM_USER_TH_END: 					// スレッド終了要求
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