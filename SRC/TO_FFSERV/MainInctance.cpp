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
// QUE_SEND_STAT emNo メッセージNo (イベントNoそのままにしようかと思ったが、FACT??_01とかだと分かりにくいからやめた)
// COMMON_QUE* que メールスロット構造体そのもの
//------------------------------------------
void MainInctance::SetDeliveryMail(QUE_SEND_STAT emNo, COMMON_QUE* que)
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
	//// 送受信データ受け渡しクラス
	gcls_pData = new SockDataExec();
	gcls_pData->SetLogMgr(mcls_pLog);

	gcls_pSock = new SockManager();
	gcls_pSock->SetLogMgr(mcls_pLog);
//	gcls_pSock->SetMainPostTid(GetTid());
	gcls_pSock->SetSockDataExec(gcls_pData);


	////////////////////////////////////
	//// ワーカースレッド 起動
	////////////////////////////////////
	gcls_pSock->Start();


	return TRUE;
}

//------------------------------------------
// 終了処理 (一度のみ起動)
//------------------------------------------
void MainInctance::Exit()
{
	//// ワーカースレッド 停止
	gcls_pSock->Stop();

	//// ワーカースレッド 開放
	delete gcls_pData;
	delete gcls_pSock;

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
	case EV_MAIL: 					// メールスロット 通知
		pMail = mque_DeliMail.GetFromHead();
		switch(pMail->emNo) {

//-----------------------------------------------------------
		case QUE_SEND_ZISEKI:		// 疵実績送信要求
			{
				Delivery* deli = new Delivery;
				memset(deli, 0x00, sizeof(Delivery));
				deli->eStat = QUE_SEND_ZISEKI;											// 要求モード
				// デバック時 実績表ファイル名称
				memcpy( deli->cOmtFileName, pMail->que.mix.cdata[0], sizeof(pMail->que.mix.cdata[0]) );
				// デバック時 実績裏ファイル名称
				memcpy( deli->cUraFileName, pMail->que.mix.cdata[1], sizeof(pMail->que.mix.cdata[1]) );
				// デバック時 状態表ファイル名称
				memcpy(deli->cStOmtFileName, pMail->que.mix.cdata[2], sizeof(pMail->que.mix.cdata[2]));
				// デバック時 状態裏ファイル名称
				memcpy(deli->cStUraFileName, pMail->que.mix.cdata[3], sizeof(pMail->que.mix.cdata[3]));
				gcls_pSock->gque_Deli.AddToTail(deli);
			}
			break;

//-----------------------------------------------------------
		case QUE_SEND_MAE:			// 前工程実績要求
			{
				Delivery* deli = new Delivery;
				memset(deli, 0x00, sizeof(Delivery));

				deli->eStat = QUE_SEND_MAE;												// 要求モード
				memcpy( &deli->keyinf, pMail->que.mix.cdata[0], SIZE_COIL_KEY);			// コイル特定するためのキー情報
				memcpy( deli->cLine_Code, pMail->que.mix.cdata[1], SIZE_KOUTEI_CODE);	// 工程コード
				memcpy( deli->cMeiNo, pMail->que.mix.cdata[2], SIZE_MEI_NO);			// 命令No
				memcpy( deli->cCoilNo, pMail->que.mix.cdata[3], SIZE_COIL_NO);			// コイルNo
				gcls_pSock->gque_Deli.AddToTail(deli); 
			}
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
