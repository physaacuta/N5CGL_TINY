#include "StdAfx.h"
#include "BaseTcpIp.h"

//////////////////////////////////////////////////////////////////////
// 構築/消滅
//////////////////////////////////////////////////////////////////////
//------------------------------------------
// コンストラクタ
// DELIVERY_COMMON* del 受け渡し構造体
//------------------------------------------
BaseTcpIp::BaseTcpIp(DELIVERY_COMMON* del) :
my_emSockState(STATE_NONE),
my_pNowDataR(NULL),
my_pNowDataS(NULL),
my_nCloseKind(-1),
my_bIsErrConnectQueing(false),
my_tId_ReTry(0),
mque_Running(16),
gque_Anser(16),
mcls_pSock(NULL),
mcls_pLog(NULL)
{
	// 引数セット
	memcpy(&my_Del_Common, del, sizeof(my_Del_Common) );

	// シグナル生成
	my_evUserClose   = CreateEvent(NULL, FALSE, FALSE, NULL);		// ユーザーからの強制切断通知
	my_evRunningEnd  = CreateEvent(NULL, FALSE, FALSE, NULL);		// ユーザーからの操作状態完了したよ通知

	// 同期シグナル (ソケット関連)
	my_evReTryConnect = CreateWaitableTimer(NULL, FALSE, NULL);		// 再接続要求タイマイベント
	my_evAccept		  = CreateEvent(NULL, FALSE, FALSE, NULL);		// Accept イベント
	my_evDisConnect	  = CreateEvent(NULL, FALSE, FALSE, NULL);		// 切断 イベント
	my_evSendTimeout  = CreateEvent(NULL, FALSE, FALSE, NULL);		// 送信タイムアウト イベント
	my_evRecvEnd	  = CreateEvent(NULL, FALSE, FALSE, NULL);		// 受信完了 イベント
	my_evSendEnd	  = CreateEvent(NULL, FALSE, FALSE, NULL);		// 送信完了 イベント

	my_evLockRecvEnd = CreateEvent(NULL, FALSE, FALSE, NULL);		// ブロッキング受信完了イベント
	my_evLockSendEnd = CreateEvent(NULL, FALSE, FALSE, NULL);		// ブロッキング送信完了イベント
}

//------------------------------------------
// デストラクタ
//------------------------------------------
BaseTcpIp::~BaseTcpIp(void)
{
	// シグナル開放
	CloseHandle(my_evUserClose);
	CloseHandle(my_evRunningEnd);

	// 同期シグナル (ソケット関連)
	CloseHandle(my_evReTryConnect);
	CloseHandle(my_evAccept);
	CloseHandle(my_evDisConnect);
	CloseHandle(my_evSendTimeout);
	CloseHandle(my_evRecvEnd);
	CloseHandle(my_evSendEnd);

	CloseHandle(my_evLockRecvEnd);
	CloseHandle(my_evLockSendEnd);
}

//------------------------------------------
// 現在のステータス名称
// SOCK_STATE st ステータス
//------------------------------------------
CString BaseTcpIp::GetSockStateName(SOCK_STATE st) const
{
	CString wk;
	if		(st == STATE_NONE)		{ wk = "初期状態 (切断中)"; }
	else if (st == STATE_IDLE)		{ wk = "準備完了 (接続中)"; }
	else if (st == STATE_RSING)		{ wk = "送受信中";}
	else if (st == STATE_RECVING)	{ wk = "受信中";}
	else if (st == STATE_SENDING)	{ wk = "送信中";}
	else							{ wk = "ステータス異常";}
	return wk;
}

//------------------------------------------
// アンサー名称
// DELI_ANS_STATE st アンサーステータス
//------------------------------------------
CString BaseTcpIp::GetAnsStateName(DELI_ANS_STATE st) const
{
	CString wk;
	if		(st == ANS_CONNECT)		{ wk = "接続完了通知"; }
	else if (st == ANS_CLOSE)		{ wk = "切断完了通知"; }
	else if (st == ANS_RECVEND)		{ wk = "非同期受信完了通知";}
	else if (st == ANS_SENDEND)		{ wk = "非同期送信完了通知";}
	else if (st == ANS_ERR_CONNECT)	{ wk = "コネクト失敗通知";}
	else							{ wk = "ステータス異常";}
	return wk;
}

//------------------------------------------
// スレッド
//------------------------------------------
UINT WINAPI BaseTcpIp::MainThread(LPVOID param)
{
	////// メインインスタンス取り出し
	BaseTcpIp *p = (BaseTcpIp *)param;
	p->SetTid();								// 自スレッドIDセット

	//// WaitForMultipleObjectsイベント
	HANDLE	hArray[] = {p->my_evStop,
						p->my_evUserClose,
						p->my_evReTryConnect,
						p->my_evAccept,
						p->my_evDisConnect,
						p->my_evSendTimeout,
						p->my_evRecvEnd,
						p->my_evSendEnd,
						p->mque_Running.g_evSem
						};
	enum {	EV_STOP = WAIT_OBJECT_0,		// スレッド終了
			EV_USERCLOSE,					// ユーザーからの強制切断通知
			EV_RETRY,						// 再接続要求タイマイベント
			EV_ACCEPT,						// Accept イベント
			EV_DISCONNECT,					// 切断イベント
			EV_SENDTIMEOUT,					// 送信タイムアウト イベント
			EV_RECVEND,						// 受信完了イベント
			EV_SENDEND,						// 送信完了イベント
			EV_QUE,							// キューイベント
			EV_END	};


	//// ソケット インスタンスの生成
	p->SockNew(true);

	//// Listen (サーバー時のみ) (成功するまでブロック)
	if( p->my_Del_Common.bSorC ) {
		bool disp = true;
		while( TRUE ) {
			if( p->Listen(disp) ) break;		// 最初の一回のみログ出力 (通常Listenで失敗する事無し)
			Sleep(2000);
			disp = false;
		}
	}
	//// Connect (クライアント時のみ)
	p->my_tId_ReTry = 0;
	if( ! p->my_Del_Common.bSorC ) {
		if( ! p->Connect(true) ) {
			p->Close(true);				// リトライタイマの為
		}
	}

	// メッセージキュー読み込み
	pLOG(em_INF), "[BaseTcpIp] <%d:%s> スレッド開始 [0x%X]", p->my_Del_Common.nMyID, p->my_Del_Common.cMyName, p->GetTid());
	while (1) {
		//int ret = MsgWaitForMultipleObjects(EV_END, hArray, FALSE, INFINITE, QS_POSTMESSAGE | QS_TIMER);
		DWORD ret = WaitForMultipleObjects(EV_END, hArray, FALSE, INFINITE);
		switch (ret) {

//-----------------------------------------------------------------------------------------------
		case EV_STOP:												// スレッド終了
			// 切断 (通知等したく無い為、手動で操作)
			if( NULL != p->mcls_pSock ) {
				if( p->my_Del_Common.bSorC ) p->mcls_pSock->CloseAll();		// つながっている全てのクライアントを切断する	
				else						 p->SockDelete(false);			// インスタンス開放
			}
			// ソケット インスタンスの開放
			p->SockDelete(true);

			// 非同期用のインスタンスが残っていたら 仕方が無いのでこのクラスで消せる奴だけ消す  // ▲ ホントにいい？？
			SAFE_DELETE(p->my_pNowDataS);
			SAFE_DELETE(p->my_pNowDataR);

			pLOG(em_INF), "[BaseTcpIp] <%d:%s> スレッド終了 [0x%X]", p->my_Del_Common.nMyID, p->my_Del_Common.cMyName, p->GetTid());
			return 0;

//-----------------------------------------------------------------------------------------------
		case EV_USERCLOSE:											// ユーザーからの強制切断通知
			// ソケット切断
			p->Close(true);
			break;

//-----------------------------------------------------------------------------------------------
		case EV_RETRY:												// 再接続要求タイマイベント
			if( ! p->Connect(false) ) {
				p->Close(false);											// リトライタイマの為
			}
			break;

//-----------------------------------------------------------------------------------------------
		case EV_ACCEPT:												// Accept イベント
			if( ! p->Accept() ) {
				p->Close(true);												// リトライタイマの為
			}
			break;

//-----------------------------------------------------------------------------------------------
		case EV_DISCONNECT:											// 切断イベント
			if(NULL != p->mcls_pSock) p->my_nCloseKind = p->mcls_pSock->GetCloseKindID(0);			// 切断種別取得
			p->Close(true);
			break;

//-----------------------------------------------------------------------------------------------
		case EV_SENDTIMEOUT:										// 送信タイムアウト イベント
			pLOG(em_WAR), "[BaseTcpIp] <%d:%s> ソケット 送信タイムアウト", p->my_Del_Common.nMyID, p->my_Del_Common.cMyName);
			if(NULL != p->my_pNowDataS ) {
				p->my_pNowDataS->nAns = 1; // タイムアウト
				p->CheckSendEnd();
			}
			break;

//-----------------------------------------------------------------------------------------------
		case EV_RECVEND:											// 受信完了イベント
		//	pLOG(em_INF), "[BaseTcpIp] <%d:%s> ソケット 受信完了", p->my_Del_Common.nMyID, p->my_Del_Common.cMyName);
			// ステータス変更
			if( STATE_RSING == p->my_emSockState ) p->my_emSockState = STATE_SENDING;
			else								   p->my_emSockState = STATE_IDLE;
			// 通知 又は 開放
			p->CheckRecvEnd();
			break;

//-----------------------------------------------------------------------------------------------
		case EV_SENDEND:											// 送信完了イベント
		//	pLOG(em_INF), "[BaseTcpIp] <%d:%s> ソケット 送信完了", p->my_Del_Common.nMyID, p->my_Del_Common.cMyName);
			// ステータス変更
			if( STATE_RSING == p->my_emSockState ) p->my_emSockState = STATE_RECVING;
			else								   p->my_emSockState = STATE_IDLE;
			// 通知 又は 開放
			p->CheckSendEnd();
			break;

//-----------------------------------------------------------------------------------------------
		case EV_QUE:												// 送受信操作依頼
			p->RunningStart();
			break;
		}
	}
	return 0;
}

// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// シーケンス制御

//------------------------------------------
// 非同期 送受信操作依頼
// 戻り値 false:キューに溜まり過ぎ？
//------------------------------------------
bool BaseTcpIp::SetRunningData(DELIVERY_DATA* data)
{
	// 前の非同期操作継続中？
	if( data->bRorS ) {
		if(NULL != my_pNowDataR) {
			return false;}
	} else {
		if(NULL != my_pNowDataS) return false;
	}
	
	// 登録
	if( 0 != mque_Running.GetFreeCount() ) {		// 登録
		mque_Running.AddToTail(data);
		return true;
	} else {									// 空き無し
		return false;
	}
}


//------------------------------------------
// 非同期 送受信操作依頼
//------------------------------------------
void BaseTcpIp::RunningStart()
{
	//// 操作要求取り出し
	DELIVERY_DATA* wk;
	wk = mque_Running.GetFromHead();
	wk->nAns = -9;								// ステータス初期化

	//// ソケット有効？
	if( ! IsConnect() ) {
		if( wk->bRorS ){
			SetAnserQue(ANS_RECVEND, wk);
		} else{
			SetAnserQue(ANS_SENDEND, wk);
		}
		return;
	}

	//// 前の非同期操作継続中
	if( wk->bRorS ) {
		if(NULL != my_pNowDataR) {
			wk->nAns = -7;
			SetAnserQue(ANS_RECVEND, wk);
			return;
		}
	} else {
		if(NULL != my_pNowDataS) {
			wk->nAns = -7;
			SetAnserQue(ANS_SENDEND, wk);
			return;
		}
	}

	//// 操作実行
	if( wk->bRorS ) {								// 受信
	//	LOG(em_INF), "[BaseTcpIp] <%d:%s> ソケット 受信開始", my_Del_Common.nMyID, my_Del_Common.cMyName);

		wk->nAns = mcls_pSock->Recv(0, &wk->Data[wk->nStartPoint], wk->nSize, my_evRecvEnd );

		// 状態変更
		if(0 == wk->nAns) {					// 受信操作成功
			if( STATE_SENDING == my_emSockState ) my_emSockState = STATE_RSING;
			else								  my_emSockState = STATE_RECVING;
			my_pNowDataR = wk;							// 操作データセット
		} else {										// 受信操作失敗
			SetAnserQue(ANS_RECVEND, wk);					// 受信完了通知
		}

	} else {										// 送信
	//	LOG(em_INF), "[BaseTcpIp] <%d:%s> ソケット 送信開始", my_Del_Common.nMyID, my_Del_Common.cMyName);

		wk->nAns = mcls_pSock->Send(0, &wk->Data[wk->nStartPoint], wk->nSize, my_evSendEnd );

		// 状態変更
		if(0 == wk->nAns) {					// 送信操作成功
			if( STATE_RECVING == my_emSockState ) my_emSockState = STATE_RSING;
			else								  my_emSockState = STATE_SENDING;
			my_pNowDataS = wk;							// 操作データセット
		} else {										// 送信操作失敗
			SetAnserQue(ANS_SENDEND, wk);					// 送信完了通知
		}
	}
}

//------------------------------------------
// 外部通知用にキューイング
// DELI_ANS_STATE ans ステータス
// DELIVERY_DATA* data データ (送受信以外はNULL) デフォルトNULL指定
//------------------------------------------
bool BaseTcpIp::SetAnserQue(DELI_ANS_STATE state, DELIVERY_DATA* data)
{
	// 登録
	if( 0 != gque_Anser.GetFreeCount() ) {		// 登録
		// 受け渡しデータ生成
		DELIVERY_ANS * ans = new DELIVERY_ANS;
		ans->nMyID = my_Del_Common.nMyID;
		ans->state = state;
		ans->Data = data;
		
		// 登録
		gque_Anser.AddToTail(ans);
		return true;
	} else {									// 空き無し
		LOG(em_WAR), "[BaseTcpIp] <%d:%s> アンサーキュー空き無し", my_Del_Common.nMyID, my_Del_Common.cMyName);
		return false;
	}
}

//------------------------------------------
// 同期 送信
// BYTE* data 送信データ
// int size 送信サイズ
// int timeout タイムアウト時間
// 戻り値 復帰情報
//			0 :正常処理完了
//			-1:このインスタンスは前のSENDを実行中
//			-2:子スレッドがとまっている
//			-9:ソケットが無効
//------------------------------------------
int BaseTcpIp::LockSend(BYTE* data, int size, int timeout)
{
	//// ソケット有効？
	if( ! IsConnect() ) return -9;

	// 同期 送信
	int retc;
	retc = mcls_pSock->Send(0, data, size, my_evLockSendEnd );
	if(0 != retc) return retc;			// 即時失敗

	// 処理完了 又は タイムアウト までブロック
	DWORD ret = WaitForSingleObject(my_evLockSendEnd, timeout);
	if (WAIT_TIMEOUT == ret) return -1;	// コマンド応答せず
	return 0;
}


//------------------------------------------
// 同期 受信
// BYTE* data 受信データ
// int size 受信サイズ
// int timeout タイムアウト時間
// 戻り値 復帰情報
//			0 :正常処理完了
//			-1:このインスタンスは前のSENDを実行中
//			-2:子スレッドがとまっている
//			-9:ソケットが無効
//------------------------------------------
int BaseTcpIp::LockRecv(BYTE* data, int size, int timeout)
{
	//// ソケット有効？
	if( ! IsConnect() ) return -9;

	// 同期 受信
	int retc;
	retc = mcls_pSock->Recv(0, data, size, my_evLockRecvEnd );
	if(0 != retc) return retc;			// 即時失敗

	// 処理完了 又は タイムアウト までブロック
	DWORD ret = WaitForSingleObject(my_evLockRecvEnd, timeout);
	if (WAIT_TIMEOUT == ret) return -1;	// コマンド応答せず
	return 0;
}



//------------------------------------------
// 非同期 受信完了
//------------------------------------------
void BaseTcpIp::CheckRecvEnd()
{
	if(NULL==my_pNowDataR) return;
	// シグナル受け取ると直ぐ処理されるので、まずNULLにしておく
	DELIVERY_DATA* wk = my_pNowDataR;
	my_pNowDataR = NULL;
	// 通知 or 開放 ?
	if( wk->bAnsQueing ) {
		SetAnserQue(ANS_RECVEND, wk);
	} else {
		DeleteEndData(wk);
	}
	my_pNowDataR = NULL;
}

//------------------------------------------
// 非同期 送信完了
//------------------------------------------
void BaseTcpIp::CheckSendEnd()
{
	if(NULL==my_pNowDataS) return;
	// シグナル受け取ると直ぐ処理されるので、まずNULLにしておく
	DELIVERY_DATA* wk = my_pNowDataS;
	my_pNowDataS = NULL;
	// 通知 or 開放 ?
	if( wk->bAnsQueing ) {
		SetAnserQue(ANS_SENDEND, wk);
	} else {
		DeleteEndData(wk);
	}
	my_pNowDataS = NULL;
}

//------------------------------------------
// 非同期 送受信データの完了したのを自動開放 (送受信の結果情報を戻り値として返す)
//------------------------------------------
int BaseTcpIp::DeleteEndData(DELIVERY_DATA* pData )	
{	
	if(NULL==pData) return false;
	int wk = pData->nAns;
	SAFE_DELETE_ARRAY(pData->Data);
	SAFE_DELETE(pData); 
	return wk;
};

// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// TPC/IP 通信制御

//------------------------------------------
// インスタンス生成
// bool bLogDsp ログ出力有無 (false:失敗ログ出さない)
//------------------------------------------
BOOL BaseTcpIp::SockNew(bool bLogDsp)
{
	// ソケットインスタンス生成
	try {
		mcls_pSock = new SockBase( my_Del_Common.cSession, TASKINI_NAME, GetTid(), PARAM_SOCK_TIMEOUT, 1, 
									my_Del_Common.nBufferSize, my_Del_Common.nBufferSize);
		mcls_pSock->SetOpt_NewAcceptNgMode(1);		// アクセプト異常時の挙動を設定。ハーフコネクション中の保持IPと新規IPが同一時には、新規セッションとして受け付ける

	} catch(int code) {
		mcls_pSock = NULL;
		if(bLogDsp) {
			LOG(em_WAR), "[BaseTcpIp] <%d:%s> ソケット New失敗 err_code=%d", my_Del_Common.nMyID, my_Del_Common.cMyName, code);
			syslog(SYSNO_SOCK_NEW_ERR, "[%d:%s err_code=%d]", my_Del_Common.nMyID, my_Del_Common.cMyName, code);
		}
		return FALSE;
	}

	//// 後処理
	if( my_Del_Common.bSorC ) mcls_pSock->SetEvAccept(my_evAccept);	// Accept認識をイベントにする (サーバーのみ)
	mcls_pSock->SetEvDisConnect(my_evDisConnect);					// 切断認識をイベントにする
	mcls_pSock->SetEvSendTimeout(my_evSendTimeout);					// 送信タイムアウト認識をイベントにする
	return TRUE;
}
//------------------------------------------
// インスタンス開放
// bool bLogDsp ログ出力有無 (false:失敗ログ出さない)
//------------------------------------------
BOOL BaseTcpIp::SockDelete(bool bLogDsp)
{
	// インスタンス開放
	SAFE_DELETE( mcls_pSock);
	return TRUE;
}

//------------------------------------------
// Listen
// bool bLogDsp ログ出力有無 (false:失敗ログ出さない)
//------------------------------------------
BOOL BaseTcpIp::Listen(bool bLogDsp)
{
	int retc;

	// Listen
	if( NULL != mcls_pSock ) {
		retc = mcls_pSock->Listen();
		if( 0 != retc ) {
			if(bLogDsp) {
				LOG(em_WAR), "[BaseTcpIp] <%d:%s> ソケット Listen失敗 err_code=%d", my_Del_Common.nMyID, my_Del_Common.cMyName, retc);
				syslog(SYSNO_SOCK_LISTEN_ERR, "[%d:%s err_code=%d]", my_Del_Common.nMyID, my_Del_Common.cMyName, retc);
			}
			return FALSE;
		}
	}

	LOG(em_INF), "[BaseTcpIp] <%d:%s> ソケット Listen開始", my_Del_Common.nMyID, my_Del_Common.cMyName);
	return TRUE;
}

//------------------------------------------
// 接続(ブロッキング)
// bool bLogDsp ログ出力有無 (false:失敗ログ出さない)
//------------------------------------------
BOOL BaseTcpIp::Connect(bool bLogDsp)
{
	int retc;

	// インスタンスが出来て無かったら生成
	if( NULL == mcls_pSock ) {
		SockNew(false);
	}

	// コネクト
	if( NULL != mcls_pSock ) {
		retc = mcls_pSock->Connect();
		if( 0 != retc ) {
			//delete mcls_pSock;
			//mcls_pSock = NULL;
			if(bLogDsp) {
				LOG(em_WAR), "[BaseTcpIp] <%d:%s> ソケット Connect失敗 err_code=%d", my_Del_Common.nMyID, my_Del_Common.cMyName, retc);
				syslog(SYSNO_SOCK_CONNECT_ERR, "[%d:%s err_code=%d]", my_Del_Common.nMyID, my_Del_Common.cMyName, retc);
			}

			// コネクト失敗時 キューイング
			if(my_bIsErrConnectQueing) SetAnserQue(ANS_ERR_CONNECT);

			return FALSE;
		}
	}

	//// バッファ確認
	if(NULL!=my_pNowDataR) {
		SAFE_DELETE(my_pNowDataR);
		LOG(em_WAR), "[BaseTcpIp] <%d:%s> ソケット Connect時 非同期受信バッファデータ有り", my_Del_Common.nMyID, my_Del_Common.cMyName);
	}
	if(NULL!=my_pNowDataS) {
		SAFE_DELETE(my_pNowDataS);
		LOG(em_WAR), "[BaseTcpIp] <%d:%s> ソケット Connect時 非同期送信バッファデータ有り", my_Del_Common.nMyID, my_Del_Common.cMyName);
	}

	//// 状態変更
	my_nCloseKind = -1;
	my_emSockState = STATE_IDLE;
	SetAnserQue(ANS_CONNECT);				// 接続完了通知

	LOG(em_INF), "[BaseTcpIp] <%d:%s> ソケット Connect完了", my_Del_Common.nMyID, my_Del_Common.cMyName);
	syslog(SYSNO_SOCK_CONNECT, "[%d:%s]", my_Del_Common.nMyID, my_Del_Common.cMyName);
	return TRUE;
}

//------------------------------------------
// Accept (シグナル通知用)
//------------------------------------------
BOOL BaseTcpIp::Accept()
{
	// Accept
	if( NULL != mcls_pSock ) {
		if( mcls_pSock->Accept() < 0 ) {
			LOG(em_WAR), "[BaseTcpIp] <%d:%s> ソケット Accept失敗 クライアント数オーバー", my_Del_Common.nMyID, my_Del_Common.cMyName);
			syslog(SYSNO_SOCK_CLIENT_MAX, "[%d:%s]", my_Del_Common.nMyID, my_Del_Common.cMyName);
			return FALSE;
		}
	}

	//// バッファ確認
	if(NULL!=my_pNowDataR) {
		SAFE_DELETE(my_pNowDataR);
		LOG(em_WAR), "[BaseTcpIp] <%d:%s> ソケット Accept時 非同期受信バッファデータ有り", my_Del_Common.nMyID, my_Del_Common.cMyName);
	}
	if(NULL!=my_pNowDataS) {
		SAFE_DELETE(my_pNowDataS);
		LOG(em_WAR), "[BaseTcpIp] <%d:%s> ソケット Accept時 非同期送信バッファデータ有り", my_Del_Common.nMyID, my_Del_Common.cMyName);
	}

	//// 状態変更
	my_nCloseKind = -1;
	my_emSockState = STATE_IDLE;
	SetAnserQue(ANS_CONNECT);				// 接続完了通知

	LOG(em_INF), "[BaseTcpIp] <%d:%s> ソケット Accept完了", my_Del_Common.nMyID, my_Del_Common.cMyName);
	syslog(SYSNO_SOCK_CONNECT, "[%d:%s]", my_Del_Common.nMyID, my_Del_Common.cMyName);
	return TRUE;
}

//------------------------------------------
// 切断
// bool bLogDsp ログ出力有無 (false:失敗ログ出さない)
//------------------------------------------
BOOL BaseTcpIp::Close(bool bLogDsp)
{
	int connectNum = 0;							// つながっていた数

	//// 切断
	if( NULL != mcls_pSock ) {
		// 送受信中の場合があるので、キャンセルしておく
		mcls_pSock->Cancel();
		if(NULL != mcls_pSock) my_nCloseKind = mcls_pSock->GetCloseKindID(0);
		// サーバーなら切断するだけ。クライアントならインスタンスを削除
		connectNum = mcls_pSock->GetActive();	
		if( my_Del_Common.bSorC ) mcls_pSock->CloseAll();					// つながっている全てのクライアントを切断する 
		else					  SockDelete(false);						// インスタンス開放
	}

	//// リトライタイマーセット (クライアント時のみ)
	if( ! my_Del_Common.bSorC ) {
		LARGE_INTEGER ts;
		ts.QuadPart = (__int64)my_Del_Common.nRetryTimer * -10000;	// 1[ms]→100[ns] (マイナス指定で、指定時間経過でシグナル) (第3引数は2回目以降の起動間隔)
		SetWaitableTimer(my_evReTryConnect, &ts, 0, NULL, NULL, FALSE);
	}

	//// 非同期操作中だったら、そのデータも返しておく
	if( STATE_RECVING == my_emSockState ) {
		if(NULL != my_pNowDataR) {
			my_pNowDataR->nAns = -8;	// 非同期操作中に切断
			CheckRecvEnd();
		}
	} else if( STATE_SENDING == my_emSockState ) {
		if(NULL != my_pNowDataS) {
			my_pNowDataS->nAns = -8;	// 非同期操作中に切断
			CheckSendEnd();
		}
	}

	//// 状態変更
	my_emSockState = STATE_NONE;
	if( 0 != connectNum) {
		SetAnserQue(ANS_CLOSE);				// 切断完了通知

		LOG(em_INF), "[BaseTcpIp] <%d:%s> ソケット 切断", my_Del_Common.nMyID, my_Del_Common.cMyName);
		syslog(SYSNO_SOCK_CLOSE, "[%d:%s]", my_Del_Common.nMyID, my_Del_Common.cMyName);
	}
	return TRUE;
}
