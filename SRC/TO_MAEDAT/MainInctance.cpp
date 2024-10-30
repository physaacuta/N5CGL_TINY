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
	m_evThStart  = CreateEvent( NULL, FALSE, FALSE, NULL );				// メインスレッド同期用
}
//------------------------------------------
// デストラクタ
//------------------------------------------
MainInctance::~MainInctance(void)
{
	// シグナル開放
	CloseHandle( m_evThStart );
}

//------------------------------------------
// スレッドキューにメールデータ登録
// ENUM_MAEDATOPERATION emNo メッセージNo (イベントNoそのままにしようかと思ったが、FACT??_01とかだと分かりにくいからやめた)
// COMMON_QUE* que メールスロット構造体そのもの
//------------------------------------------
void MainInctance::SetDeliveryMail( ENUM_MAEDATOPERATION emNo, COMMON_QUE* que )
{
	DELIVERY_MAIL* pMail = new DELIVERY_MAIL;
	pMail->emNo	= emNo;
	memcpy( &pMail->que, que, sizeof(COMMON_QUE) );

	// スレッドキューに登録
	if( ! mque_DeliMail.AddToTailFreeCheck( pMail ) ) {
		delete pMail;
		LOG(em_ERR), "[%s] mque_DeliMail キューフル!!", my_sThreadName );
	}
}

//------------------------------------------
// 初期化処理 (一度のみ起動)
//------------------------------------------
BOOL MainInctance::Init()
{
	//// メインインスタンスのみ、スレッドだけは作っておく (すぐに非シグナルとする)
	//// 継承元クラス実行 (メイン制御スレッドの為、ちょっと優先度高め)
	ThreadManager::Start( THREAD_PRIORITY_ABOVE_NORMAL );

	// スレッドIDが割りつくまでちょっと休憩
	while( 0==GetTid() ) {
		LOG(em_MSG), "[%s] スレッドID割り当て中・・・", my_sThreadName );
		Sleep(1);			// スレッド起動までちょっと待ち
	}


	////////////////////////////////////
	//// ワーカースレッド 初期化
	////////////////////////////////////
	// FF送信管理クラス
	gcls_FFSend.SetLogMgr( mcls_pLog );

	// パラメータ管理クラス
	gcls_Param.SetLogMgr( mcls_pLog );

	// 前工程編集クラス
	gcls_pPreDefect = new PreDefectManager();
	gcls_pPreDefect->SetLogMgr( mcls_pLog );

	// 前工程実績情報DB登録管理クラス
	gcls_pPreDB = new PreDBManager();
	gcls_pPreDB->SetLogMgr( mcls_pLog );
    // 2014.07.25 川原  所間連携    >>>>>>>>
    gcls_pPreDB->SetParamManager( &gcls_Param );
    // 2014.07.25 川原  所間連携    <<<<<<<<

    // 前工程メイン管理クラス
	gcls_PreProcManager = new PreProcManager();
	gcls_PreProcManager->SetLogMgr( mcls_pLog );
	gcls_PreProcManager->SetMainPostTid( GetTid() );
	gcls_PreProcManager->SetFFSendManager( &gcls_FFSend );
	gcls_PreProcManager->SetParamManager( &gcls_Param );
	gcls_PreProcManager->SetPreDefectManager( gcls_pPreDefect );
	gcls_PreProcManager->SetPreDBManager( gcls_pPreDB );
	gcls_PreProcManager->Alloc();

	////////////////////////////////////
	//// ワーカースレッド 起動
	////////////////////////////////////
	gcls_PreProcManager->Start();
	gcls_FFSend.Start();
	gcls_Param.Start();
	gcls_pPreDefect->Start();
	gcls_pPreDB->Start();

	return TRUE;
}

//------------------------------------------
// 終了処理 (一度のみ起動)
//------------------------------------------
void MainInctance::Exit()
{

	//// ワーカースレッド 停止
	gcls_PreProcManager->Stop();
	gcls_FFSend.Stop();
	gcls_Param .Stop();
	gcls_pPreDefect->Stop();
	gcls_pPreDB->Stop();

	//// ワーカースレッド 開放
	delete gcls_PreProcManager;
	delete gcls_pPreDefect;
	delete gcls_pPreDB;

}

//------------------------------------------
// スレッド開始前処理 (スレッド実行直後(シグナルセット前)にコールバック)
//------------------------------------------
int MainInctance::ThreadFirst()
{
	////// 他ワークスレッドの準備が整うまで待機 (MainInctanceのみ)
	LOG(em_MSG), "[%s] 他ワークスレッドの準備が整うまで待機中・・・ [0x%X]", my_sThreadName, GetTid() );
	WaitForSingleObject( m_evThStart, INFINITE );

	//// イベント設定
	HANDLE hArray[] = { mque_DeliMail.g_evSem
						};
	SetEventNum( sizeof(hArray)/sizeof(HANDLE), hArray );

	return 0;
}

//------------------------------------------
// スレッドイベント発生
// int nEventNo イベントNo (0オリジン)
//------------------------------------------
void MainInctance::ThreadEvent( int nEventNo )
{
	DELIVERY_MAIL* pMail;

	////// シグナルの条件
	enum {	EV_MAIL = 0,				// メールスロット 通知 (メールは一番最後にしようかな)
		};

	////// シグナル条件分け
	switch ( nEventNo ) {

//-----------------------------------------------------------------------------------------------
	case EV_MAIL: 						// メールスロット 通知
		pMail = mque_DeliMail.GetFromHead();
		switch( pMail->emNo ) {

//-----------------------------------------------------------
		case E_PD_ML_COIL:		// コイル情報受信通知
			{
				COMMON_QUE* pQue = new COMMON_QUE;
				memcpy( pQue, &pMail->que, sizeof(COMMON_QUE) );

				// ＦＦ前工程疵情報管理へ
				gcls_PreProcManager->gque_Deli.AddToTail( pQue ); 
			}
			break;

//-----------------------------------------------------------
		case E_PD_ML_FF:		// FF前工程疵情報受信通知
			{
				COMMON_QUE* pQue = new COMMON_QUE;
				memcpy( pQue, &pMail->que, sizeof(COMMON_QUE) );
				
				// ＦＦ前工程疵情報管理へ
				gcls_PreProcManager->gque_Deli.AddToTail( pQue );
			}
			break;

//-----------------------------------------------------------
		case E_PD_END:
			break;


//-----------------------------------------------------------
		}
		// 開放
		SAFE_DELETE(pMail);
		return;

//-----------------------------------------------------------------------------------------------
	default :
		// ありえない！！
		LOG(em_ERR), "[%s] ThreadEvent EventNo NG=%d", my_sThreadName, nEventNo );
		return;
	}
}

