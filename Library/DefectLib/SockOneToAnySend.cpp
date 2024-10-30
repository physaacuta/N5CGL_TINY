#include "StdAfx.h"
#include "SockOneToAnySend.h"

//------------------------------------------
// コンストラクタ
//------------------------------------------
SockOneToAnySend::SockOneToAnySend() :
ThreadManager("SockSend"),
my_cSession(""),
my_bRetryMode(false),

my_emSockState(STATE_NONE),
my_pHead(NULL),
my_nSendingDataSize(0),
my_emSendAddId(STATE_SUB_NONE),
my_bHelcyCancel(false),

mcls_pSock(NULL),
mque_pDeli(NULL)
{
	// シグナル生成
	my_tmReTryConnect	= CreateWaitableTimer(NULL, FALSE, NULL);
	my_tmHelcy			= CreateWaitableTimer(NULL, FALSE, NULL);

	my_evConnect		= CreateEvent(NULL, FALSE, FALSE, NULL);
	my_evDisConnect		= CreateEvent(NULL, FALSE, FALSE, NULL);
	my_evSendTimeout	= CreateEvent(NULL, FALSE, FALSE, NULL);
	my_evSendEnd		= CreateEvent(NULL, FALSE, FALSE, NULL);
	my_evAnserClose		= CreateEvent(NULL, FALSE, FALSE, NULL);
	my_evAnserConnect	= CreateEvent(NULL, FALSE, FALSE, NULL);

	InitializeCriticalSection(&m_csLock);
}

//------------------------------------------
// デストラクタ
//------------------------------------------
SockOneToAnySend::~SockOneToAnySend(void)
{
	// ソケット破棄
	SAFE_DELETE(mcls_pSock);

	// シグナル開放
	CloseHandle(my_tmReTryConnect);
	CloseHandle(my_tmHelcy);

	CloseHandle(my_evConnect);
	CloseHandle(my_evDisConnect);
	CloseHandle(my_evSendTimeout);
	CloseHandle(my_evSendEnd);
	CloseHandle(my_evAnserClose);
	CloseHandle(my_evAnserConnect);

	DeleteCriticalSection(&m_csLock);

}

//------------------------------------------
// 指定のステータス名称
// SOCK_STATE st ステータス
//------------------------------------------
CString SockOneToAnySend::GetSockStateName(SOCK_STATE st) const
{
	CString wk;
	if		(st == STATE_NONE)		{ wk = "初期状態 (切断中)"; }
	else if (st == STATE_IDLE)		{ wk = "準備完了 (接続中)"; }
	else if (st == STATE_HEAD)		{ wk = "ヘッダ部 送信中";}
	else if (st == STATE_DATA)		{ wk = "データ部 送信中";}
	else							{ wk = "ステータス異常";}
	return wk;
}

//------------------------------------------
// 現バッファを具無しキューに返却する
//------------------------------------------
void SockOneToAnySend::QueBufSendBack()
{
	if(NULL == my_pHead) return;
	mque_pDeli->AddFree(my_pHead);
	my_pHead = NULL;
}

//------------------------------------------
// すべてのキューを返却する
//------------------------------------------
void SockOneToAnySend::QueAllSendBack()
{
	SOCK_F_HEAD2*		deli = NULL;

	// デバック用ログ
	if(0 !=mque_pDeli->GetMaxCountPool() ){		// 1回も動いていない時は出さない
		LOG(em_MSG), "[%s] 未送信キュー開放! Que=%d, MAX=%d", my_sThreadName, mque_pDeli->GetCountPool(), mque_pDeli->GetMaxCountPool());
	}

	// キューに溜まっているインスタンスを全部開放
	while(true) {
		DWORD ret = WaitForMultipleObjects(1, &mque_pDeli->g_evSemPool, FALSE, 0);
		if( WAIT_TIMEOUT==ret ) break;
		deli = mque_pDeli->GetPool();
		if (deli == NULL) break;


		// 返却
		mque_pDeli->AddFree(deli);
		deli = NULL;
	}
	// 最大件数リセット
	mque_pDeli->ReSetMaxCountPool(); 
	// 取り出し済みのバッファもあれば一緒に開放する
	QueBufSendBack();
}

//------------------------------------------
// スレッド開始前処理 (スレッド実行直後(シグナルセット前)にコールバック)
//------------------------------------------
int SockOneToAnySend::ThreadFirst()
{
	mcls_Time.InitAve();

	//// イベント設定
	HANDLE hArray[] = { my_tmReTryConnect,
						my_evConnect,
						my_evDisConnect, 
						my_evSendTimeout,
						my_evSendEnd,
						my_tmHelcy,
						mque_pDeli->g_evSemPool
						};
	SetEventNum(sizeof(hArray)/sizeof(HANDLE), hArray);
	return 0;
}

//------------------------------------------
// スレッド終了前処理 (終了シグナル後にコールバック)
//------------------------------------------
int SockOneToAnySend::ThreadLast()
{
	return 0;
}

//------------------------------------------
// スレッドのWaitFor待ちの直前処理  
// int nDefWaitNum 元の数
// int* nNowWaitNum 変更後の数
//------------------------------------------
void SockOneToAnySend::ThreadWaitFor(int nDefWaitNum, int* nNowWaitNum)
{
	// 実行中か確認
	bool bExec = (my_emSockState != STATE_NONE && my_emSockState != STATE_IDLE ) ? true : false;

	// 待ち行列数変更
	if( bExec )	*nNowWaitNum = nDefWaitNum-1;
	else		*nNowWaitNum = nDefWaitNum;
}

//------------------------------------------
// スレッドイベント発生
// int nEventNo イベントNo (0オリジン)
//------------------------------------------
void SockOneToAnySend::ThreadEvent(int nEventNo)
{	
	////// シグナルの条件
	enum {	EV_RETRY,						// 再接続要求タイマイベント
			EV_CONNECT,						// 接続イベント
			EV_DISCONNECT,					// 切断イベント
			EV_SENDTIMEOUT,					// 送信タイムアウト イベント
			EV_SENDEND,						// 送信完了イベント
			EV_HELCY,						// ヘルシータイマー
			EV_QUE							// 送信依頼
	};

	
// ◆	LOG(em_MSG), "[%s] <%s> イベント受信=%d", my_sThreadName, my_cSession, nEventNo);


	////// シグナル条件分け
	switch (nEventNo) {
//-----------------------------------------------------------------------------------------------
	case EV_RETRY:												// 再接続要求タイマイベント
		Connect(false);											// リトライ接続
		return;

//-----------------------------------------------------------------------------------------------
	case EV_CONNECT:											// 接続要求イベント
		LOG(em_MSG), "[%s] <%s> 回線接続要求", my_sThreadName, my_cSession);
		Connect(true);											// 初期接続
		return;

//-----------------------------------------------------------------------------------------------
	case EV_DISCONNECT:											// 切断イベント
		LOG(em_WAR), "[%s] <%s> 回線切断通知", my_sThreadName, my_cSession);
		Close();													// 切断
		return;

//-----------------------------------------------------------------------------------------------
	case EV_SENDTIMEOUT:										// 送信タイムアウト イベント
		LOG(em_WAR), "[%s] <%s> 送信タイムアウト", my_sThreadName, my_cSession);
		syslog(SYSNO_SOCK_SEND_TIMEOUT,"", my_cSession);
		Close();
		return;

//-----------------------------------------------------------------------------------------------
	case EV_SENDEND:											// 送信完了イベント
		SendData();
		return;

//-----------------------------------------------------------------------------------------------
	case EV_HELCY:											// ヘルシー送信イベント
		SendHelcy();
		return;

//-----------------------------------------------------------------------------------------------
	case EV_QUE:												// 送信依頼
		SendStart();
		return;

//-----------------------------------------------------------------------------------------------
	default :
		// ありえない！！
		LOG(em_ERR), "[%s] ThreadEvent EventNo NG=%d", my_sThreadName, nEventNo );
		return;
	}
}

// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// シーケンス制御

//------------------------------------------
// ヘッダー部送信開始
//------------------------------------------
void SockOneToAnySend::SendHeader()
{
	if(NULL == my_pHead) {
		LOG(em_WAR), "[%s] <%s> ヘッダ部 送信バッファ無し", my_sThreadName, my_cSession);
		return ;
	}

	_ASSERT( my_pHead->base_head_addr.len_head <= SIZE_SOCK_DATA_MAX);
//LOG(em_INF), "[%s] Start size=%d sub=%d", my_sThreadName, my_pHead->base_head.len, my_pHead->base_head_addr.len_head );	// 上でチェックしているはずなので

	// ソケット状態変更
	my_emSockState = STATE_HEAD;
	my_emSendAddId = STATE_SUB_NONE;
	my_nSendingDataSize = 0;

	// 送信
	int iRet = mcls_pSock->Send(0, my_pHead, my_pHead->base_head_addr.len_head, my_evSendEnd);
	if( iRet ) {					// ソケット送信エラー
		LOG(em_ERR), "[%s] <%s> ヘッダ部 送信失敗", my_sThreadName, my_cSession);
		syslog(SYSNO_SOCK_SEND_ERR, "[SockOneToAnySend err_code = %d] (疵情報伝文 ヘッダー部)", iRet);

		// ソケット切断
		Close();
	}
}

//------------------------------------------
// データ部送信開始
//------------------------------------------
void SockOneToAnySend::SendData()
{
	//----------------
	// 次どれ？
	AutoLock lock(&m_csLock);
	if(NULL == my_pHead) {
		LOG(em_WAR), "[%s] <%s> データ部 送信バッファ無し", my_sThreadName, my_cSession);
		return ;
	}

	DWORD size = 0;
	long* p = NULL;
	while(1) {
		// 
		if( STATE_SUB_NONE >= my_emSendAddId && 0 != my_pHead->base_head_addr.len_addr[0]  ) {
			my_emSendAddId = STATE_SUB_ADDR0;
			size = my_pHead->base_head_addr.len_addr[0];
			p = my_pHead->base_head.addr[0]; 
			break;
		}
		//
		if( STATE_SUB_ADDR0 >= my_emSendAddId && 0 != my_pHead->base_head_addr.len_addr[1]  ) {
			my_emSendAddId = STATE_SUB_ADDR1;
			size = my_pHead->base_head_addr.len_addr[1];
			p = my_pHead->base_head.addr[1]; 
			break;
		}
		break;
	}

	// 次はある？
	if( NULL == p ) {
		SendEnd();
		return ;
	}

	//----------------
	// 送信
//LOG(em_INF), "[%s] size=%d sub=%d QueFree:%d", my_sThreadName, size, my_emSendAddId, mque_pDeli->GetCountFree());	// 上でチェックしているはずなので

	// 送信
	int iRet = mcls_pSock->Send(0, p, size, my_evSendEnd);
	if( iRet ) {					// ソケット送信エラー
		LOG(em_ERR), "[%s] <%s> データ部 送信失敗 [size=%d, total=%d]", my_sThreadName, my_cSession, size, my_nSendingDataSize);
		syslog(SYSNO_SOCK_SEND_ERR, "[SockOneToAnySend err_code = %d] (データ部 size=%d total=%d)", iRet, size, my_nSendingDataSize);

		// ソケット切断
		Close();
	} else {
		// ソケット状態変更
		my_emSockState = STATE_DATA;
		my_nSendingDataSize += size;
	}
}

//------------------------------------------
// 伝文送信 開始処理
//------------------------------------------
void SockOneToAnySend::SendStart()
{
	mcls_Time.PutTimestamp(0);

	AutoLock lock(&m_csLock);
	// バッファにキューがすでにある？
	if(my_pHead != NULL) {
		LOG(em_ERR), "[%s] <%s> バッファにキューがすでに有り", my_sThreadName, my_cSession);
		QueBufSendBack();
	}

	// キュー取り出し
	my_pHead = mque_pDeli->GetPool();

	// ソケット未接続時は、インスタンス開放
	if( STATE_NONE == my_emSockState ) {
		// ◆		LOG(em_WAR), "[%s] <%s> 切断時の送信依頼", my_sThreadName, my_cSession);

		QueBufSendBack();
		return;
	}

	// ヘッダー送信開始
	SendHeader();
}

//------------------------------------------
// 疵情報伝文送信 完了処理
//------------------------------------------
void SockOneToAnySend::SendEnd()
{
	AutoLock lock(&m_csLock);

	if( my_pHead == NULL ) return;

	// ヘルシー以外？
	if( memcmp(my_pHead->base_head.code, &T_CODE_HELCY, sizeof(my_pHead->base_head.code))) {
		my_bHelcyCancel = true;
	}

	// 現バッファのインスタンスを返却
	QueBufSendBack();						
	my_emSockState = STATE_IDLE;		// ステータス変更
	mcls_Time.PutTimestamp(1);

//LOG(em_INF), "[%s] 送信時間 %.3f QueFree:%d", my_sThreadName, mcls_Time.GetTimestamp(1,0), mque_pDeli->GetCountFree());	// 上でチェックしているはずなので

}


//------------------------------------------
// ヘルシー伝文送信 処理
//------------------------------------------
void SockOneToAnySend::SendHelcy()
{
	//// 前回ヘルシーからの間で、実データを送っているので、ヘルシーは必要なし
	if(my_bHelcyCancel) {
		my_bHelcyCancel = false;
		return;
	}

	//// 送信
	SOCK_F_HEAD2* p = mque_pDeli->GetFree(0);	// セマフォー待ちバージョン(すぐに結果を返したい)
	if( NULL == p ) {
		LOG(em_ERR), "[%s] <%s> キューなし", my_sThreadName, my_cSession );
		return;
	}

	strcpy(p->base_head.pname, getmypcname());
	memcpy(p->base_head.code, &T_CODE_HELCY, sizeof(p->base_head.code));
	p->base_head.len		= sizeof(SOCK_F_HEAD);

	p->base_head_addr.len_head	= p->base_head.len;
	p->base_head_addr.len_addr[0]= 0;
	p->base_head_addr.len_addr[1]= 0;
	
	if( ! mque_pDeli->AddPool(p) ) {		// 登録
		LOG(em_ERR), "[%s] <%s> ヘルシー伝文送信依頼キューフル", my_sThreadName, my_cSession);
		syslog(SYSNO_QUEFULL_WAR, "[PC間 ヘルシー伝文]");
	}
}

// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// TPC/IP 通信制御


//------------------------------------------
// 接続
// bool bLogDsp ログ出力有無 (false:失敗ログ出さない)
//------------------------------------------
void SockOneToAnySend::Connect(bool bLogDsp)
{
	int retc;

	//// 未送信バッファを返却 (念の為)
	QueAllSendBack();

	// ソケットインスタンス生成
	try {
		mcls_pSock = new SockBase( my_cSession, TASKINI_NAME, GetTid(), PARAM_SOCK_TIMEOUT, PARAM_SOCK_MAX, 0, PARAM_SOCK_SEND_BUFSIZE);
		mcls_pSock->SetKsThPriority( GetThreadPriority(my_hThread) );	// 自分自身のスレッド優先度を引き継ぐ
		//mcls_pSock = new SockBase( my_cSession, TASKINI_NAME, 0, PARAM_SOCK_TIMEOUT, PARAM_SOCK_MAX, 0, PARAM_SOCK_SEND_BUFSIZE);
	} catch(int code) {
		mcls_pSock = NULL;
		if(bLogDsp) {
			LOG(em_ERR), "[%s] <%s> ソケット New失敗 err_code=%d", my_sThreadName, my_cSession, code);
			syslog(SYSNO_SOCK_NEW_ERR, "[%s: err_code=%d]", my_cSession, code);
		}
		return ;
	}

	// コネクト
	if( NULL != mcls_pSock ) {
		retc = mcls_pSock->Connect();
		if( 0 != retc ) {
			delete mcls_pSock;
			mcls_pSock = NULL;
			if(bLogDsp) {
				LOG(em_ERR), "[%s] <%s> ソケット Connect失敗 err_code=%d", my_sThreadName, my_cSession, retc);
				syslog(SYSNO_SOCK_CONNECT_ERR, "[%s: err_code=%d]", my_cSession, retc);
			}
		}
	}

	// 後処理
	if( NULL != mcls_pSock ) {								// 成功
		mcls_pSock->SetEvSendTimeout(my_evSendTimeout);			// 送信タイムアウトをイベントにする
		mcls_pSock->SetEvDisConnect(my_evDisConnect);			// 切断認識をイベントにする

		LOG(em_MSG), "[%s] <%s> コネクション確立", my_sThreadName, my_cSession);
		my_emSockState = STATE_IDLE;							// ステータス変更
		SetEvent(my_evAnserConnect);

		// ヘルシー起動
		__int64 ts = PARAM_TIME_HELCY_SEND * -10000;	// 1μ秒→100ナノ秒 (マイナス指定で、指定時間経過でシグナル) (第3引数は2回目以降の起動間隔)
		SetWaitableTimer(my_tmHelcy, (LARGE_INTEGER *)&ts, PARAM_TIME_HELCY_SEND, NULL, NULL, FALSE);
		my_bHelcyCancel = false;

	} else {												// ソケット生成失敗
		Close();												// 切断 & リトライ設定
	}
}

//------------------------------------------
// 切断 & リトライ設定
//------------------------------------------
void SockOneToAnySend::Close()
{
	// ソケット切断
	if(NULL != mcls_pSock) {
		SAFE_DELETE(mcls_pSock); 
		LOG(em_WAR), "[%s] <%s> ソケット 切断", my_sThreadName, my_cSession);
		syslog(SYSNO_SOCK_CLOSE, "[%s]", my_cSession);
	}

	// 初期化
	AutoLock lock(&m_csLock);
	QueAllSendBack();								// 未送信バッファを返却
	my_emSockState = STATE_NONE;					// ソケット状態 初期化
	SetEvent(my_evAnserClose);						// 外部へ 切断を通知

	// ヘルシー伝文タイムー監視終了
	CancelWaitableTimer(my_tmHelcy);

	// リトライ (有効時のみ)
	if(my_bRetryMode) {
		LARGE_INTEGER ts;
		ts.QuadPart = (__int64)PARAM_SOCK_RETRY_TIME * -10000;	// 1[ms]→100[ns] (マイナス指定で、指定時間経過でシグナル) (第3引数は2回目以降の起動間隔)
		SetWaitableTimer(my_tmReTryConnect, &ts, 0, NULL, NULL, FALSE);
	}
}
