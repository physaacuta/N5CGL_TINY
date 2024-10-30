#include "StdAfx.h"
#include "MainInctance.h"

// メモリーリーク検出 ----->>>
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
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
	pMail->emNo = emNo;
	memcpy(&pMail->que, que, sizeof(COMMON_QUE));

	// スレッドキューに登録
	if (!mque_DeliMail.AddToTailFreeCheck(pMail)) {
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
	ThreadManager::Start(THREAD_PRIORITY_ABOVE_NORMAL);

	// スレッドIDが割りつくまでちょっと休憩
	while( 0==GetTid() ) {
		LOG(em_MSG), "[%s] スレッドID割り当て中・・・", my_sThreadName);
		Sleep(1);			// スレッド起動までちょっと待ち
	}



	////////////////////////////////////
	//// ワーカースレッド 初期化
	////////////////////////////////////
	//// 送受信クラス
	gcls_pSock = new SockDataExec();
	gcls_pSock->SetLogMgr(mcls_pLog);
	gcls_pSock->SetQueDeli(&gque_DeliSend);
	gcls_pSock->Alloc();

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
	gcls_pSock->Free();

	//// ワーカースレッド 開放
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
	HANDLE hArray[] = { 
		mque_DeliMail.g_evSem
	};
	SetEventNum(sizeof(hArray) / sizeof(HANDLE), hArray);

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
	enum {
		EV_MAIL	= 0			// メールスロット 通知 (メールは一番最後にしようかな)
	};

	////// シグナル条件分け
	switch (nEventNo) {

//-----------------------------------------------------------------------------------------------
	case EV_MAIL: 						// メールスロット 通知
		pMail = mque_DeliMail.GetFromHead();
		switch (pMail->emNo) {

//-----------------------------------------------------------
		case E_DEF_DEBUG_RECV:			// デバック用伝文受信通知

			if (true) {
				BYTE bindata[2048];
				memset(bindata, 0x00, sizeof(bindata));
				int nFileSize = KizuFunction::GetFileSize(pMail->que.mix.cdata[0]);
				if (0 >= nFileSize) {
					LOG(em_ERR), "[%s] ファイルが見つからないか、ゼロバイトです。[%s] Size=%d", my_sThreadName, pMail->que.mix.cdata[0], nFileSize );
					break;
				}
				if (nFileSize > sizeof(bindata)) {
					nFileSize = sizeof(bindata);
				}

				int nTc = pMail->que.mix.idata[0];
				int nMode = 0;			// 読み込みモード(0:バイト反転なし,1:バイト反転あり) BUF_PRO_R_COILINFのバイナリデータを読み込む場合は、1:バイト反転ありで実行
#ifdef BYTE_HANTEN
				nMode = 1;
#endif
				if (!gcls_pSock->ReadBufFile(nTc, bindata, pMail->que.mix.cdata[0], nMode)) {
					LOG(em_ERR), "[%s] ファイル読込みで失敗です。TC=%d [%s] Size=%d", my_sThreadName, nTc, pMail->que.mix.cdata[0], nFileSize );
					break;
				}
				gcls_pSock->SetRecvData(nTc, nFileSize, bindata);
			}
			break;

//-----------------------------------------------------------
		}
		// 開放
		delete pMail;
		return;

		//-----------------------------------------------------------------------------------------------
	default:
		// ありえない！！
		LOG(em_ERR), "[%s] ThreadEvent EventNo NG=%d", my_sThreadName, nEventNo );
		return;
	}
}


