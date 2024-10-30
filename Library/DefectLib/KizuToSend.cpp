#include "StdAfx.h"
#include "KizuToSend.h"


//------------------------------------------
// コンストラクタ
//------------------------------------------
KizuToSend::KizuToSend() :
my_cSession(""),
my_bRetryMode(false),
my_emSockState(STATE_NONE),
my_pHead(NULL),
my_nSendingDataSize(0),
my_nSendNum(1),
mcls_pSock(NULL),
mcls_pLog(NULL)
{
	// シグナル生成
	my_evReTryConnect	= CreateWaitableTimer(NULL, FALSE, NULL);
	my_evConnect		= CreateEvent(NULL, FALSE, FALSE, NULL);
	my_evDisConnect		= CreateEvent(NULL, FALSE, FALSE, NULL);
	my_evSendTimeout	= CreateEvent(NULL, FALSE, FALSE, NULL);
	my_evSendEnd		= CreateEvent(NULL, FALSE, FALSE, NULL);
	my_evAnserClose		= CreateEvent(NULL, FALSE, FALSE, NULL);

//	// PC名称取得
//	memcpy(my_cPCname, getmypcname(), SIZE_NAME_PC);
	memset(my_cPCname, 0x00, sizeof(my_cPCname));
}

//------------------------------------------
// デストラクタ
//------------------------------------------
KizuToSend::~KizuToSend(void)
{
	// ソケット破棄
	SAFE_DELETE(mcls_pSock);

	// シグナル開放
	CloseHandle(my_evReTryConnect);
	CloseHandle(my_evConnect);
	CloseHandle(my_evDisConnect);
	CloseHandle(my_evSendTimeout);
	CloseHandle(my_evSendEnd);
	CloseHandle(my_evAnserClose);
}

//------------------------------------------
// 現在のステータス名称
//------------------------------------------
CString KizuToSend::GetSockStateName() const
{
	CString wk;
	if		(my_emSockState == STATE_NONE)		{ wk = "初期状態 (切断中)"; }
	else if (my_emSockState == STATE_IDLE)		{ wk = "準備完了 (接続中)"; }
	else if (my_emSockState == STATE_HEAD)		{ wk = "ヘッダ部 送信中";}
	else if (my_emSockState == STATE_DATA)		{ wk = "データ部 送信中";}
	else										{ wk = "ステータス異常";}
	return wk;
}


//------------------------------------------
// 現バッファを返却する
//------------------------------------------
void KizuToSend::QueBufSendBack()
{
	if(NULL == my_pHead) return;
	gque_SendBack.AddToTail(my_pHead);
	my_pHead = NULL;
}
//------------------------------------------
// すべてのキューを返却する
//------------------------------------------
void KizuToSend::QueAllSendBack()
{
	SOCK_F_HEAD*		deli;

	// デバック用ログ
	if(0 !=gque_Deli.GetMaxCount() ){		// 1回も動いていない時は出さない
		LOG(em_MSG), "[KizuToSend] 未送信キュー開放! Que=%d, MAX=%d", gque_Deli.GetCount(), gque_Deli.GetMaxCount());
	}

	// キューに溜まっているインスタンスを全部開放
	while(true) {
		deli = gque_Deli.GetFromHead(0);
		if (deli == NULL) break;

		// 返却
		gque_SendBack.AddToTail(deli);
		deli = NULL;
	}
	// 最大件数リセット
	gque_Deli.ReSetMaxCount(); 
	// 取り出し済みのバッファもあれば一緒に開放する
	QueBufSendBack();
}


//------------------------------------------
// スレッド
//------------------------------------------
UINT WINAPI KizuToSend::MainThread(LPVOID param)
{
	////// メインインスタンス取り出し
	KizuToSend *p = (KizuToSend *)param;
	p->SetTid();								// 自スレッドIDセット

	// WaitForMultipleObjectsイベント
	HANDLE	hArray[] = {p->my_evStop,
						p->my_evReTryConnect,
						p->my_evConnect,
						p->my_evDisConnect, 
						p->my_evSendTimeout,
						p->my_evSendEnd,
						p->gque_Deli.g_evSem
						};
	enum {	EV_STOP = WAIT_OBJECT_0,		// スレッド終了
			EV_RETRY,						// 再接続要求タイマイベント
			EV_CONNECT,						// 接続イベント
			EV_DISCONNECT,					// 切断イベント
			EV_SENDTIMEOUT,					// 送信タイムアウト イベント
			EV_SENDEND,						// 送信完了イベント
			EV_QUE,							// 送信依頼
			EV_END	};

	// メッセージキュー読み込み
	pLOG(em_MSG), "[KizuToSend] <%s> スレッド開始 [0x%X]", p->my_cSession, p->GetTid());
	while (1) {
		int wait_num = (p->my_emSockState != STATE_NONE && p->my_emSockState != STATE_IDLE ) ? EV_END-1 : EV_END;
		DWORD ret = WaitForMultipleObjects(wait_num, hArray, FALSE, INFINITE);

		switch (ret) {

//-----------------------------------------------------------------------------------------------
		case EV_STOP:												// スレッド終了
			pLOG(em_MSG), "[KizuToSend] <%s> スレッド終了 [0x%X]", p->my_cSession, p->GetTid());
			// ソケット破棄
			SAFE_DELETE(p->mcls_pSock);
			return 0;

//-----------------------------------------------------------------------------------------------
		case EV_RETRY:												// 再接続要求タイマイベント
			p->Connect(false);											// リトライ接続
			break;

//-----------------------------------------------------------------------------------------------
		case EV_CONNECT:											// 接続要求イベント
			pLOG(em_MSG), "[KizuToSend] <%s> 回線接続要求", p->my_cSession);
			p->Connect(true);											// 初期接続
			break;

//-----------------------------------------------------------------------------------------------
		case EV_DISCONNECT:											// 切断イベント
			pLOG(em_WAR), "[KizuToSend] 回線切断通知");
			p->Close();													// 切断
			break;

//-----------------------------------------------------------------------------------------------
		case EV_SENDTIMEOUT:										// 送信タイムアウト イベント
			pLOG(em_WAR), "[KizuToSend] 送信タイムアウト");
			syslog(SYSNO_SOCK_SEND_TIMEOUT,"", p->my_cSession);
			p->Close();
			break;

//-----------------------------------------------------------------------------------------------
		case EV_SENDEND:											// 送信完了イベント
			if(NULL == p->mcls_pSock) {										// 切断済み
				pLOG(em_WAR), "[KizuToSend] 切断済み状態で、送信完了イベント");
				break;	
			}

			switch( p->my_emSockState ) {
			case KizuToSend::STATE_NONE:									// イニシャル伝文送信完了
				pLOG(em_INF), "[KizuToSend] イニシャル伝文送信完了");

				//バク修正 20080514  (これだと、自分で作ったデータなのに、送信完了データ受け渡しキューにキューイングされてしまう) --->>>
				//p->SendEnd(); 
				SAFE_DELETE(p->my_pHead);
				p->my_emSockState = STATE_IDLE;		// ステータス変更
				// <<<----
				break;

			case KizuToSend::STATE_HEAD:									// ヘッダー部送信完了
				// この伝文は、ヘルシー？
				if( 0 == memcmp( p->my_pHead->base_head.code, T_CODE_HELCY, SIZE_SOCK_TC) ) {
					delete p->my_pHead;
					p->my_pHead = NULL;
					p->my_emSockState = STATE_IDLE;		// ステータス変更
					break;
				}

				// このフレームはヘッダー部のみ？
				if( sizeof(SOCK_F_HEAD) == p->my_pHead->base_head.len ) {
					p->SendEnd();
				} else {
					// 続いてデータ部送信
					p->SendData();
				}
				break;

			case KizuToSend::STATE_DATA:									// データ部送信完了
				// まだ分割送信中？
				if(p->my_pHead->base_head.len > p->my_nSendingDataSize+SIZE_SOCK_HEAD) {
					
					// 残りの部分を送信
					p->SendData();
				} else {
					// 受信完了
					p->SendEnd();
				}
				break;
			}
			break;

//-----------------------------------------------------------------------------------------------
		case EV_QUE:												// 送信依頼
			p->SendStart();
			break;

//-----------------------------------------------------------------------------------------------
		default :
			// ありえない！！
			break;
		}
	}
	return 0;
}


// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// シーケンス制御


//------------------------------------------
// ヘッダー部送信開始
//------------------------------------------
void KizuToSend::SendHeader()
{
	// ソケット状態変更
	my_emSockState = STATE_HEAD;
	
	// 送信
	int iRet = mcls_pSock->Send(0, my_pHead, sizeof(SOCK_F_HEAD), my_evSendEnd);
	if( iRet ) {					// ソケット送信エラー
		LOG(em_ERR), "[KizuToSend] 疵情報伝文 ヘッダ部 送信失敗");
		syslog(SYSNO_SOCK_SEND_ERR, "[KizuToSend err_code = %d] (疵情報伝文 ヘッダー部)", iRet);

		// ソケット切断
		Close();
	}
}

//------------------------------------------
// データ部送信開始
//------------------------------------------
void KizuToSend::SendData()
{
	// データ部がホントにあるか確認
	if(my_pHead->base_head.addr[0] == NULL) {
		//LOG(em_ERR), "[KizuToSend] 疵情報伝文 データ部 アドレス無し");
		SendEnd();
		return;
	}

	// 送信サイズ指定
	DWORD size = my_pHead->base_head.len - SIZE_SOCK_HEAD - my_nSendingDataSize;		// 送信サイズ
	if(size > SIZE_SOCK_DATA_MAX) size = SIZE_SOCK_DATA_MAX;							// 1回の送信サイズオーバー

//LOG(em_ERR), "[KizuToSend] データ部送信 [%dbyte目, %X]", my_nSendingDataSize, ((BYTE*)my_pHead->base_head.addr[0])[my_nSendingDataSize]);

	// 送信
	int iRet = mcls_pSock->Send(0, &((BYTE*)my_pHead->base_head.addr[0])[my_nSendingDataSize], size, my_evSendEnd);
	if( iRet ) {					// ソケット送信エラー
		LOG(em_ERR), "[KizuToSend] 疵情報伝文 データ部 送信失敗 [size=%d, total=%d]", size, my_nSendingDataSize);
		syslog(SYSNO_SOCK_SEND_ERR, "[KizuToSend err_code = %d] (疵情報伝文 データ部 size=%d total=%d)", iRet, size, my_nSendingDataSize);

		// ソケット切断
		Close();
	} else {
		// ソケット状態変更
		my_emSockState = STATE_DATA;
		my_nSendingDataSize += size;
	}
}

//------------------------------------------
// 疵情報伝文送信 開始処理
//------------------------------------------
void KizuToSend::SendStart()
{
	// バッファにキューがすでにある？
	if(my_pHead != NULL) {
		LOG(em_ERR), "[KizuToSend] バッファにキューがすでに有り");
	}

	// キュー取り出し
	my_pHead = gque_Deli.GetFromHead();
	my_nSendingDataSize = 0;

	// ソケット未接続時は、インスタンス開放
	if( STATE_NONE == my_emSockState ) {
		// へルシー？
		if( 0 == memcmp( my_pHead->base_head.code, T_CODE_HELCY, SIZE_SOCK_TC) ) {
			delete my_pHead;
			my_pHead = NULL;
			return;
		}
		
		QueBufSendBack();
		return;
	}

	// ヘッダー送信開始
	SendHeader();
}

//------------------------------------------
// 疵情報伝文送信 完了処理
//------------------------------------------
void KizuToSend::SendEnd()
{
	if( my_pHead == NULL ) return;
	// 現バッファのインスタンスを返却
	QueBufSendBack();						
	my_emSockState = STATE_IDLE;		// ステータス変更
}


// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// TPC/IP 通信制御

//------------------------------------------
// 接続
// bool bLogDsp ログ出力有無 (false:失敗ログ出さない)
//------------------------------------------
void KizuToSend::Connect(bool bLogDsp)
{
	int retc;

	//// 未送信バッファを返却 (念の為)
	QueAllSendBack();

	// ソケットインスタンス生成
	try {
		mcls_pSock = new SockBase( my_cSession, TASKINI_NAME, GetTid(), PARAM_SOCK_TIMEOUT, 1, 0, PARAM_SOCK_SEND_BUFSIZE);
	} catch(int code) {
		mcls_pSock = NULL;
		if(bLogDsp) {
			LOG(em_ERR), "[KizuToSend] <%s> ソケット New失敗 err_code=%d", my_cSession, code);
			syslog(SYSNO_SOCK_NEW_ERR, "[%s: err_code=%d]", my_cSession, code);
		}
	}

	// コネクト
	if( NULL != mcls_pSock ) {
		retc = mcls_pSock->Connect();
		if( 0 != retc ) {
			delete mcls_pSock;
			mcls_pSock = NULL;
			if(bLogDsp) {
				LOG(em_ERR), "[KizuToSend] <%s> ソケット Connect失敗 err_code=%d", my_cSession, retc);
				syslog(SYSNO_SOCK_CONNECT_ERR, "[%s: err_code=%d]", my_cSession, retc);
			}
		}
	}

	// 後処理
	if( NULL != mcls_pSock ) {								// 成功
		mcls_pSock->SetEvSendTimeout(my_evSendTimeout);			// 送信タイムアウトをイベントにする
		mcls_pSock->SetEvDisConnect(my_evDisConnect);			// 切断認識をイベントにする

		// イニシャル伝文生成
		my_pHead = new SOCK_F_HEAD;
		memset(my_pHead, 0x00, sizeof(SOCK_F_HEAD));
		memcpy(my_pHead->base_head.code, T_CODE_NAME, sizeof(my_pHead->base_head.code));	// トランザクションコード
		memcpy(my_pHead->base_head.pname, my_cPCname, sizeof(my_cPCname));					// PC名称
		my_pHead->base_head.subinf = my_nSendNum;											// 論理回線数
		my_pHead->base_head.len = sizeof(SOCK_F_HEAD);										// 伝文長
		my_pHead->base_head.addr[0] = NULL;													// データ部のアドレス
		my_pHead->base_head.addr[1] = NULL;													// 未使用


		// 送信
		int iRet = mcls_pSock->Send(0, my_pHead, my_pHead->base_head.len, my_evSendEnd );
		if(iRet) {				// 送信異常
			LOG(em_ERR), "[KizuToSend] イニシャル伝文送信失敗, err_code=%d", iRet);
			syslog(SYSNO_SOCK_SEND_ERR, "[err_code = %d] (イニシャル伝文)", iRet);
			
			// ソケットインスタンス破棄
			SAFE_DELETE(mcls_pSock);

			// 切断
			Close();												// 切断 & リトライ設定
		} else {				// 送信成功
			// アプリレベル ソケット接続完了
			LOG(em_MSG), "[KizuToSend] 統括→判定間 コネクション確立");
		}

	} else {												// ソケット生成失敗
		Close();												// 切断 & リトライ設定
	}
}

//------------------------------------------
// 切断 & リトライ設定
//------------------------------------------
void KizuToSend::Close()
{
	// ソケット切断
	if(NULL != mcls_pSock) {
		SAFE_DELETE(mcls_pSock); 
		LOG(em_WAR), "[KizuToSend] <%s> ソケット 切断", my_cSession);
		syslog(SYSNO_SOCK_CLOSE, "[%s]", my_cSession);
	}

	// 初期化
	my_emSockState = STATE_NONE;					// ソケット状態 初期化
	QueAllSendBack();								// 未送信バッファを返却
	
	// 外部へ 切断を通知
	SetEvent(my_evAnserClose); 

	// リトライ (有効時のみ)
	if(my_bRetryMode) {
		LARGE_INTEGER ts;
		ts.QuadPart = (__int64)PARAM_SOCK_RETRY_TIME * -10000;	// 1[ms]→100[ns] (マイナス指定で、指定時間経過でシグナル) (第3引数は2回目以降の起動間隔)
		SetWaitableTimer(my_evReTryConnect, &ts, 0, NULL, NULL, FALSE);
	}
}
