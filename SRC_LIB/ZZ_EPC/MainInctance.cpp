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
	// データ送信用クラス
	gcls_pData = new DataSender();
	gcls_pData->SetLogMgr(mcls_pLog);
	
	// コマンド受信用クラス
	gcls_pCommand = new CommandRecvre();
	gcls_pCommand->SetLogMgr(mcls_pLog);
	gcls_pCommand->SetMainPostTid(GetTid()); 
	gcls_pCommand->SetDataSender(gcls_pData); 
	// 速度UDP受信用クラス
	gcls_pPlg = new PLGReceive();
	gcls_pPlg->SetLogMgr(mcls_pLog);
	gcls_pPlg->SetDataSender(gcls_pData); 
	// エッジ位置情報UDP受信用クラス
	gcls_pEdge = new EdgeReceive();
	gcls_pEdge->SetLogMgr(mcls_pLog);
	gcls_pEdge->SetDataSender(gcls_pData); 

	//// ワーカースレッド 起動
	gcls_pCommand->Start();
	gcls_pData->Start();
	gcls_pPlg->Start();
	gcls_pEdge->Start();
	return TRUE;
}

//------------------------------------------
// 終了処理 (一度のみ起動)
//------------------------------------------
void MainInctance::Exit()
{

	//// ワーカースレッド 停止
	gcls_pCommand->Stop();
	gcls_pData->Stop();
	gcls_pPlg->Stop();
	gcls_pEdge->Stop();
	//// ワーカースレッド 開放
	delete gcls_pCommand;
	delete gcls_pData;
	delete gcls_pPlg;
	delete gcls_pEdge;
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