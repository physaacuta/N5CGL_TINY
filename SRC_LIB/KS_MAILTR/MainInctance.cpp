#include "StdAfx.h"
#include "MainInctance.h"

#include "winsock2.h"

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
	gcls_TrUdpMgr.SetLogMgr(mcls_pLog);

	////////////////////////////////////
	//// ワーカースレッド 起動
	////////////////////////////////////

	gcls_TrUdpMgr.Start();



	return TRUE;
}

//------------------------------------------
// 終了処理 (一度のみ起動)
//------------------------------------------
void MainInctance::Exit()
{

	// ワーカースレッド 停止
	gcls_TrUdpMgr.Stop();

	// ワーカースレッド 解放
	//////delete gcls_TrUdpMgr;
	
}


//------------------------------------------
// スレッド開始前処理 (スレッド実行直後(シグナルセット前)にコールバック)
//------------------------------------------
int MainInctance::ThreadFirst()
{
	//=======================================================
	// UDP生成
	while(true){
		if( SetUdp() )	break;
		LOG(em_ERR), "[%s] UDP 準備中・・・ ", my_sThreadName);
		Sleep(2000);
	}

	////// 他ワークスレッドの準備が整うまで待機 (MainInctanceのみ)
	LOG(em_MSG), "[%s] 他ワークスレッドの準備が整うまで待機中・・・ [0x%X]", my_sThreadName, GetTid() );
	WaitForSingleObject(m_evThStart, INFINITE);
	
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
		case E_DEF_ML_TRANS:			// メールスロット転送要求
				if(true) {
					struct in_addr		addr;
					memcpy(&addr, &pMail->que.mailtr_que.nUdpIpAddr, sizeof(in_addr));

					LOG(em_INF),  "[%s] メールスロット転送 [Ip:%s][Pc:%s][Task:%s]", my_sThreadName, inet_ntoa(addr), pMail->que.mailtr_que.cPcName, pMail->que.mailtr_que.cTaskName);

					UdpMail udpmail = {};
					memcpy(udpmail.hostname, pMail->que.mailtr_que.cPcName, sizeof(udpmail.hostname));
					memcpy(udpmail.mailname, pMail->que.mailtr_que.cTaskName, sizeof(udpmail.mailname));
					udpmail.datalen = sizeof(pMail->que.mailtr_que.cMsg);
					memcpy(udpmail.data, pMail->que.mailtr_que.cMsg, udpmail.datalen);


					sockaddr_in sin;
					sin.sin_family = AF_INET;
					sin.sin_port = htons( KS_MAILTR_PORT );
					sin.sin_addr.s_addr = pMail->que.mailtr_que.nUdpIpAddr;

					sendto(m_sock_udp, (const char*)&udpmail, sizeof(UdpMail), 0, (sockaddr*)&sin, sizeof(sin));
				}
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

//////////////////////////////////////////////////////////////////////
// ソケット

//------------------------------------------
// UDP SOCKET生成
// 戻り値 復帰情報
//------------------------------------------
BOOL MainInctance::SetUdp()
{
	WSADATA wsaData;
	WORD wVersionRequired = MAKEWORD(2, 2);
	WSAStartup(wVersionRequired, &wsaData);
	m_sock_udp = (int)socket(AF_INET, SOCK_DGRAM, 0);
	if (INVALID_SOCKET == m_sock_udp) return FALSE;
	return TRUE;
}


