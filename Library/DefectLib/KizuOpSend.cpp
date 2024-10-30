#include "stdafx.h"
#include "KizuOpSend.h"

//////////////////////////////////////////////////////////////////////
// 構築/消滅
//////////////////////////////////////////////////////////////////////
//------------------------------------------
// コンストラクタ
// const char *cSession iniファイルセッション
//------------------------------------------
KizuOpSend::KizuOpSend(const char *cSession) :
my_cSession(cSession),
my_bIsNowSend(FALSE),
my_pHead(NULL),
my_nSendingDataSize(0),
mcls_pSock(NULL),
mcls_pLog(NULL)
{
	// 全クライアント切断中認識
	for(int ii=0; ii<PARAM_SOCK_MAX; ii++) my_bIsConnect[ii] = FALSE;


	// シグナル生成
	my_evConnect	= CreateEvent(NULL, FALSE, FALSE, NULL);
	my_evClose		= CreateEvent(NULL, FALSE, FALSE, NULL);
}

//------------------------------------------
// デストラクタ
//------------------------------------------
KizuOpSend::~KizuOpSend()
{
	// 全キュー開放
	QueAllFree();

	// ソケット開放
	if(NULL != mcls_pSock) delete mcls_pSock;

	// シグナル開放
	CloseHandle(my_evConnect);
	CloseHandle(my_evClose);
}


//------------------------------------------
// 現バッファを開放する
//------------------------------------------
void KizuOpSend::QueBufFree()
{
	if(my_pHead==NULL) return;
	
	// 開放
	if ( my_pHead->addr[0] != NULL ) delete my_pHead->addr[0];		// データ部開放
	if ( my_pHead->addr[1] != NULL ) delete my_pHead->addr[1];
	delete my_pHead;

	my_pHead = NULL;
}

//------------------------------------------
// すべてのキュー開放
//------------------------------------------
void KizuOpSend::QueAllFree()
{
	SOCK_BASE_HEAD*		deli = NULL;

	// デバック用ログ
	if(0 != gque_Deli.GetMaxCount() ){		// 1回も動いていない時は出さない
		LOG(em_MSG), "[KizuOpSend] <%s> 未送信キュー開放! Que=%d, MAX=%d", my_cSession, gque_Deli.GetCount(), gque_Deli.GetMaxCount());
	}

	// キューに溜まっているインスタンスを全部開放
	while(true) {
		deli = gque_Deli.GetFromHead(0);
		if (deli == NULL) break;
		// 開放
		if ( deli->addr[0] != NULL ) delete deli->addr[0];		// データ部開放
		if ( deli->addr[1] != NULL ) delete deli->addr[1];
		delete deli;
		deli = NULL;
	}
	// 最大件数リセット
	gque_Deli.ReSetMaxCount(); 
	// 取り出し済みのバッファもあれば一緒に開放する
	QueBufFree();
}



//------------------------------------------
// スレッド
//  MsgWaitForMultipleObjects を使わないとダメ。シグナルだけでは対応不可。送受信でmsgのIDが必要
//------------------------------------------
UINT WINAPI KizuOpSend::MainThread(LPVOID param)
{
	MSG	msg;								// MSG構造体
	int id;

	////// メインインスタンス取り出し
	KizuOpSend *p = (KizuOpSend *)param;
	p->SetTid();								// 自スレッドIDセット
	PeekMessage(&msg, NULL, WM_USER, WM_USER, PM_NOREMOVE);			// メッセージ作成


	//// ソケット生成 + Listen (成功するまでブロック)
	bool disp = true;
	while( TRUE ) {
		if( p->Listen(disp) ) break;		// 最初の一回のみログ出力 (通常Listenで失敗する事無し)
		Sleep(2000);
		disp = false;
	}


	//// ヘルシー送信タイマー開始
	__int64 ts = PARAM_TIME_HELCY_SEND * -10000;	// 1μ秒→100ナノ秒 (マイナス指定で、指定時間経過でシグナル) (第3引数は2回目以降の起動間隔)
	HANDLE	evHelcy;								// ヘルシー伝文送信タイマー
	evHelcy	= CreateWaitableTimer(NULL, FALSE, NULL);			
	SetWaitableTimer(evHelcy, (LARGE_INTEGER *)&ts, PARAM_TIME_HELCY_SEND, NULL, NULL, FALSE);


	//// WaitForMultipleObjectsイベント
	HANDLE	hArray[] = {p->my_evStop,
						evHelcy,
						p->gque_Deli.g_evSem,
						};
	enum {	EV_STOP = WAIT_OBJECT_0,		// スレッド終了
			EV_HELCY,						// ヘルシー伝文送信
			EV_QUE,							// キューイベント
			EV_END	};


	// メッセージキュー読み込み
	pLOG(em_MSG), "[KizuOpSend] <%s> スレッド開始 [0x%X]", p->my_cSession, p->GetTid());
	while (1) {
		int wait_num = ( p->my_bIsNowSend ) ? EV_QUE : EV_END;
//		DWORD ret = WaitForMultipleObjects(wait_num, hArray, FALSE, INFINITE);
		int ret = MsgWaitForMultipleObjects(wait_num, hArray, FALSE, INFINITE, QS_POSTMESSAGE | QS_TIMER);
		switch (ret) {

//-----------------------------------------------------------------------------------------------
		case EV_STOP:												// スレッド終了
			pLOG(em_MSG), "[KizuOpSend] <%s> スレッド終了 [0x%X]", p->my_cSession, p->GetTid());

			// ヘルシー送信タイマー停止
			CancelWaitableTimer(evHelcy);
			CloseHandle(evHelcy);
			return 0;


//-----------------------------------------------------------------------------------------------
		case EV_HELCY:												// ヘルシー伝文送信
			//pLOG(em_MSG), "[KizuOpSend] <%s> ヘルシー伝文送信", p->my_cSession);
			p->SendHelcy();
			break;


//-----------------------------------------------------------------------------------------------
		case EV_QUE:												// 送信依頼 (キューセマフォー)
			if ( EV_QUE == wait_num-1 ) {
				p->SendStart();
			}
			break;
		}


		// ここに来たときは、メッセージが複数個あるかもしれない
		// 従ってループでメッセージを全部吸い出す (したがって PeekMessage でないとダメ)
		int rc;
		while (rc = PeekMessage(&msg, NULL, 0, 0, PM_REMOVE) ) {
			switch( msg.message ) {

//-----------------------------------------------------------------------------------------------
			case WM_USER+WM_KS_ACCEPT:								// 接続要求
				//// アクセプト
				id = p->mcls_pSock->Accept(&msg);
				if( id < 0 ) {
					pLOG(em_ERR), "[KizuOpSend] <%s> クライアント数オーバー", p->my_cSession);
					syslog(SYSNO_SOCK_CLIENT_MAX, "[KizuOpSend]");
					break;
				}

				//// 正常
				pLOG(em_MSG), "[KizuOpSend] <%s> ソケット接続 [client_no=%d]", p->my_cSession, id);
				syslog(SYSNO_SOCK_CONNECT, "[(KizuOpSend) %s: client_no=%d]", p->my_cSession, id);
				p->my_bIsConnect[id] = TRUE;			// クライアント接続認識
				SetEvent(p->my_evConnect);
				break;

//-----------------------------------------------------------------------------------------------
			case WM_USER+WM_KS_DISCONNECT:							// 切断通知
				//// 送信中に切断有り
				if( p->my_bIsNowSend ) {
					pLOG(em_ERR), "[KizuOpSend] <%s> 送信中に切断！送信途中だけど中断", p->my_cSession);
					// 送信強制終了
					p->SendEnd();
				} else {
					pLOG(em_ERR), "[KizuOpSend] <%s> 切断通知！", p->my_cSession);
				}
				//id = p->mcls_pSock->GetSockID((UINT)msg.wParam);		// UINTは64bitAPの場合NG
				id = p->mcls_pSock->GetSockID(msg.wParam);	
				if(-1==id) break;		// 既にどっかで切断済み
				p->Close(id); 
				SetEvent(p->my_evClose);
				break;

//-----------------------------------------------------------------------------------------------
			case WM_USER+WM_KS_SEND:								// 送信完了
				// 全クライアントに送信完了
				if(p->my_bIsAllSend) {		
					p->SendEnd();
					break;
				}
				
				// 次を送信
				p->Send();
				break;

//-----------------------------------------------------------------------------------------------
			case WM_USER+WM_KS_SEND_TIMEOUT:
				pLOG(em_ERR), "[KizuOpSend] <%s> 送信タイムアウト", p->my_cSession);
				p->SendEnd();
				//id = p->mcls_pSock->GetSockID((UINT)msg.wParam);		// UINTは64bitAPの場合NG
				id = p->mcls_pSock->GetSockID(msg.wParam);
				if(-1==id) break;		// 既にどっかで切断済み
				p->Close(id); 
				break;
			}
		}
	}
	return 0;
}

// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// シーケンス制御

//------------------------------------------
// 疵情報伝文送信 開始処理
//------------------------------------------
void KizuOpSend::SendStart()
{
	// バッファにキューがすでにある？
	if(my_pHead != NULL) {
		LOG(em_ERR), "[KizuOpSend] <%s> バッファにキューがすでに有り", my_cSession);
		QueBufFree();
	}

	// キュー取り出し
	my_pHead = gque_Deli.GetFromHead();

	// 送信開始準備
	my_bIsAllSend = FALSE;
	my_nNowSendCount = 0;
	my_nSendingDataSize =0;

	// ヘッダー送信開始
	if( Send() ) {						// 送信開始
		my_bIsNowSend = TRUE;
	} else {							// 1台も送れなかった
		// 特になにもしない
	}
}

//------------------------------------------
// 疵情報伝文送信 完了処理
//------------------------------------------
void KizuOpSend::SendEnd()
{
	//// 全部送信完了
	my_bIsNowSend = FALSE;
	my_bIsAllSend = TRUE;			// 念の為に・・・
	QueBufFree();
}

//------------------------------------------
// ヘルシー伝文送信 処理
//------------------------------------------
void KizuOpSend::SendHelcy()
{
	SOCK_BASE_HEAD*		pSendBuf = new SOCK_BASE_HEAD;

	//// データセット
	memset(pSendBuf, 0x00, sizeof(SOCK_BASE_HEAD));
	memcpy(pSendBuf->code, T_CODE_HELCY, SIZE_SOCK_TC);
	strcpy(pSendBuf->pname, TOKATU);
	pSendBuf->len		= sizeof(SOCK_BASE_HEAD);
	pSendBuf->addr[0]	= NULL;
	pSendBuf->addr[1]	= NULL;

	//// 送信依頼
	if( ! gque_Deli.AddToTailFreeCheck(pSendBuf) ) {		// 登録
		LOG(em_ERR), "[KizuOpSend] 統括⇒表示間 ヘルシー伝文送信依頼キューフル");
		syslog(SYSNO_QUEFULL_WAR, "[統括⇒表示間 ヘルシー伝文]");
		delete pSendBuf;
	}
}





// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// TPC/IP 通信制御

//------------------------------------------
// Listen
// bool bLogDsp ログ出力有無 (false:失敗ログ出さない)
//------------------------------------------
BOOL KizuOpSend::Listen(bool bLogDsp)
{
	int retc;
	// ソケットインスタンス生成
	try {
		mcls_pSock = new SockBase(my_cSession, TASKINI_NAME, GetTid(), PARAM_SOCK_TIMEOUT, 
										 PARAM_SOCK_MAX, 0, PARAM_SOCK_SEND_BUFSIZE);
	} catch(int code) {
		mcls_pSock = NULL;
		if(bLogDsp) {
			LOG(em_ERR), "[KizuOpSend] <%s> ソケット New失敗 err_code=%d", my_cSession, code);
			syslog(SYSNO_SOCK_NEW_ERR, "[%s: err_code=%d]", my_cSession, code);
		}
		return FALSE;
	}

	// Listen
	if( NULL != mcls_pSock ) {
		retc = mcls_pSock->Listen();
		if( 0 != retc ) {
			delete mcls_pSock;
			mcls_pSock = NULL;
			if(bLogDsp) {
				LOG(em_ERR), "[KizuOpSend] <%s> ソケット Listen失敗 err_code=%d", my_cSession, retc);
				syslog(SYSNO_SOCK_LISTEN_ERR, "[%s: err_code=%d]", my_cSession, retc);
			}
			return FALSE;
		}
	}

	LOG(em_MSG), "[KizuOpSend] <%s> ソケット Listen開始", my_cSession);
	return TRUE;
}

//------------------------------------------
// 切断処理
// int id クライアントID
//------------------------------------------
void KizuOpSend::Close(int id)
{
	if(-1==id) return;		// 既にどっかで切断済み

	// 切断処理
	mcls_pSock->CloseID(id);

	my_bIsConnect[id] = FALSE;
	LOG(em_ERR), "[KizuOpSend] <%s> クライアント切断 [client_no=%d]", my_cSession, id);
	syslog(SYSNO_SOCK_CLOSE, "[KizuOpSend(%s): client_no=%d]", my_cSession, id);
}

//------------------------------------------
// 疵情報伝文送信
// 戻り値 FALSE:1台も送れずに終了した場合
//------------------------------------------
BOOL KizuOpSend::Send()
{
/*
	// 送信可能なクライアントを探す
	int ii;
	for(ii=my_nNowSendCount; ii<PARAM_SOCK_MAX; ii++) {
		// つながってないものは論外
		if( ! my_bIsConnect[ii] ) continue;

		// 未送信のクライアント発見
		int iRet = mcls_pSock->Send(ii, my_pHead, my_pHead->len );
		if( iRet ) {				// 送信異常
			LOG(em_ERR), "[KizuOpSend] <%s> 送信エラー[client_no=%d, err_code=%d]", my_cSession, ii, iRet);
			syslog(SYSNO_SOCK_SEND_ERR, "[KizuOpSend(%s): client_no = %d: err_code = %d]", my_cSession, ii, iRet);
			Close(ii);
			continue;

		} else {
			// 送信完了
			my_nNowSendCount++;
			// 全部送信できた？
			if(ii == (PARAM_SOCK_MAX-1) ) my_bIsAllSend = TRUE;
			break;		// 1回づつしか送らないため
		}
	}

	//// 送信可能なクライアントが無い場合は送信完了がこないから ここで送信完了とする
	if(PARAM_SOCK_MAX == ii) {
		SendEnd();
		return FALSE;
	} else {
		return TRUE;
	}
*/

	// 送信位置のポインタ
	BYTE* pSendData = (BYTE*)my_pHead;	
	// 送信サイズ指定
	DWORD size;

	// 送信可能なクライアントを探す
	int ii;
	for(ii=my_nNowSendCount; ii<PARAM_SOCK_MAX; ii++) {
		// つながってないものは論外
		if( my_bIsConnect[ii] ) break;
	}

	//// 送信可能なクライアントが無い場合は送信完了がこないから ここで送信完了とする
	if(PARAM_SOCK_MAX == ii ) {
		SendEnd();
		return FALSE;
	}

	// まだ分割送信中？
	size = my_pHead->len - my_nSendingDataSize;					// 送信サイズ
	if(size > SIZE_SOCK_DATA_MAX) size = SIZE_SOCK_DATA_MAX;	// 1回の送信サイズオーバー
	pSendData += (my_nSendingDataSize);

	// 未送信のクライアント発見
	int iRet = mcls_pSock->Send(ii, pSendData, size );
	if( iRet ) {				// 送信異常
		LOG(em_ERR), "[KizuOpSend] <%s> 送信エラー[client_no=%d, err_code=%d]", my_cSession, ii, iRet);
		syslog(SYSNO_SOCK_SEND_ERR, "[KizuOpSend(%s), client_no = %d, err_code = %d]", my_cSession, ii, iRet);
		Close(ii);
		my_nSendingDataSize = 0;
		Send();

	} else {
		my_nSendingDataSize += size;
		if( my_nSendingDataSize >= my_pHead->len  ) {
			// 送信完了
			my_nNowSendCount++;
			my_nSendingDataSize = 0;
			// 全部送信できた？
			if(ii == (PARAM_SOCK_MAX-1) ) my_bIsAllSend = TRUE;
		}
	}
	return TRUE;
}
