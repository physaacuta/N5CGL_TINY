#include "StdAfx.h"
#include "SockOneToAnyRecv.h"

//------------------------------------------
// コンストラクタ
//------------------------------------------
SockOneToAnyRecv::SockOneToAnyRecv(void) :
ThreadManager("SockRecv"),
my_cSession(""),
mcls_pSock(NULL),
mque_pDeli(NULL)
{
	//// 初期化
	for(int ii=0; ii<PARAM_SOCK_MAX; ii++) {
		my_emSockState[ii]	 = STATE_NONE;
		my_pHead[ii]		 = NULL; 
		my_nRecvingDataSize[ii] = 0;
		my_emRecvAddId[ii]	 = STATE_SUB_NONE;
		my_bHelcyOk[ii]		 = false;

		// イベント
		my_evDisConnect[ii]	 = CreateEvent(NULL, FALSE, FALSE, NULL);	// 切断イベント認識(ID)
		my_evRecvTimeOut[ii] = CreateEvent(NULL, FALSE, FALSE, NULL);	// 受信タイムアウトイベント認識(ID)
		my_evRecv[ii]		 = CreateEvent(NULL, FALSE, FALSE, NULL);	// 受信イベント認識(ID)

	}

	// イベント
	my_evCloseReq			= CreateEvent(NULL, FALSE, FALSE, NULL);	// 回線切断要求
	my_evAccept				= CreateEvent(NULL, FALSE, FALSE, NULL);
	my_tmHelcy				= CreateWaitableTimer(NULL, FALSE, NULL);

	//// リセット(初期化)
	Reset();
}

//------------------------------------------
// デストラクタ
//------------------------------------------
SockOneToAnyRecv::~SockOneToAnyRecv(void)
{
	//// ソケット解放
	if( NULL != mcls_pSock ) {
		delete mcls_pSock;
		mcls_pSock = NULL;
	}

	//// イベント
	CloseHandle(my_evCloseReq);
	CloseHandle(my_evAccept);	// 各判定PCよりAccept時
	CloseHandle(my_tmHelcy);
}

// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// TPC/IP 通信制御

//------------------------------------------
// リセット
//------------------------------------------
void SockOneToAnyRecv::Reset()
{
	//// 現在の読込キャンセル
	if(mcls_pSock) mcls_pSock->Cancel();

	//// 初期化
	memset( my_RecvWkHead, 0x00, sizeof(my_RecvWkHead));

	//// すでにキューから器を取っていたら返却
	for(int ii=0; ii<PARAM_SOCK_MAX; ii++) {
		if( NULL != my_pHead[ii] ) {
			mque_pDeli->AddFree(my_pHead[ii]);
			LOG(em_INF), "[%s] <%s> キュー強制返却", my_sThreadName, my_cSession);
			my_pHead[ii] = NULL;
		}
	}

}

//------------------------------------------
// Listen
// bool bLogDsp ログ出力有無 (false:失敗ログ出さない)
//------------------------------------------
BOOL SockOneToAnyRecv::Listen(bool bLogDsp)
{
	int retc;
	// ソケットインスタンス生成
	try {
		mcls_pSock = new SockBase(my_cSession, TASKINI_NAME, GetTid(), PARAM_SOCK_TIMEOUT, PARAM_SOCK_MAX, PARAM_SOCK_RECV_BUFSIZE);
		//mcls_pSock->SetOpt_NewAcceptNgMode(1);		// アクセプト異常時の挙動を設定。ハーフコネクション中の保持IPと新規IPが同一時には、新規セッションとして受け付ける
		mcls_pSock->SetOpt_NewAcceptNgMode(0);
		mcls_pSock->SetKsThPriority( GetThreadPriority(my_hThread) );	// 自分自身のスレッド優先度を引き継ぐ

	} catch(int code) {
		mcls_pSock = NULL;
		if(bLogDsp) {
			LOG(em_ERR), "[%s] <%s> ソケット New失敗 err_code=%d", my_sThreadName, my_cSession, code);
			syslog(SYSNO_SOCK_NEW_ERR, "[%s: err_code=%d]", my_cSession, code);
		}
		return FALSE;
	}

	// Listen
	if( NULL != mcls_pSock ) {
		mcls_pSock->SetEvAccept(my_evAccept);			// Accept認識をイベントにする (サーバーのみ)

		retc = mcls_pSock->Listen();
		if( 0 != retc ) {
			delete mcls_pSock;
			mcls_pSock = NULL;
			if(bLogDsp) {
				LOG(em_ERR), "[%s] <%s> ソケット Listen失敗 err_code=%d", my_sThreadName, my_cSession, retc);
				syslog(SYSNO_SOCK_LISTEN_ERR, "[%s: err_code=%d]", my_cSession, retc);
			}
			return FALSE;
		}
	}

	LOG(em_MSG), "[%s] <%s> ソケット Listen開始", my_sThreadName, my_cSession);
	return TRUE;
}

//------------------------------------------
// Accept (シグナル通知用)
// 戻り値 -1:NG  0～：正常。物理回線ID
//------------------------------------------
int SockOneToAnyRecv::Accept()
{
	_ASSERT(NULL != mcls_pSock);

	int id = mcls_pSock->Accept();
	if( id < 0 ) {
		LOG(em_ERR), "[%s] <%s> クライアント数オーバー", my_sThreadName, my_cSession);
		syslog(SYSNO_SOCK_CLIENT_MAX, "[%s]", my_cSession);
		return -1;
	}
	// 今繋がったソケットの状態を変更
	mcls_pSock->SetEvDisConnect(id, my_evDisConnect[id]);					// 切断認識をイベントにする
	mcls_pSock->SetEvRecvTimeout(id, my_evRecvTimeOut[id]);					// 受信タイムアウト認識をイベントにする

	// ステータス変更
	my_emSockState[id] = STATE_IDLE;
	my_bHelcyOk[id]	= true;					// まずは１回目として接続されたので、正常認識にしておく
	return id;
}

//------------------------------------------
// クライアント切断
// int id 物理回線ID
//------------------------------------------
void SockOneToAnyRecv::Close(int id)
{
	if(STATE_NONE == my_emSockState[id]) {
		LOG(em_MSG), "[%s] <%s> (%d) 切断", my_sThreadName, my_cSession, id);
	}
	mcls_pSock->CloseID(id);
	my_emSockState[id] = STATE_NONE;
}

//------------------------------------------
// 全クライアント切断 & 異常通知
// bool bErr 異常通知 (false:異常切断 true:正常切断)
//------------------------------------------
void SockOneToAnyRecv::CloseAll(bool bErr)
{
	if(NULL == mcls_pSock) return;
	// 切断
	mcls_pSock->CloseAll();
	// リセット
	Reset();
	// 異常通知
	if(! bErr) ErrCall();
}

//------------------------------------------
// 異常通知のみ (回線切断はしない)
// この関数がコールされるのは、異常の時だけのはず
//------------------------------------------
void SockOneToAnyRecv::ErrCall()
{
	if(NULL == mcls_pSock) return;

	// リセット
	Reset();
}

//------------------------------------------
// 業務伝文のヘッダー受信を開始
// int id 物理回線ID
//------------------------------------------
BOOL SockOneToAnyRecv::RecvHead(int id)
{
	my_nRecvingDataSize[id] = 0;
	my_emRecvAddId[id] = STATE_SUB_NONE;

	// ヘッダー受信開始
	int iRet = mcls_pSock->Recv(id, &my_RecvWkHead[id], SIZE_SOCK_HEAD, my_evRecv[id]);
	if( 0 != iRet ) {
		LOG(em_ERR), "[%s] <%s> (%d)ヘッダー受信エラー [err_code=%d]", my_sThreadName, my_cSession, id, iRet);
		syslog(SYSNO_SOCK_RECV_ERR, "[<%s> (HEAD) id=%d err_code=%d]", my_cSession, id, iRet);
		return FALSE;
	} else {
		// ステータス変更
		my_emSockState[id] = STATE_HEAD;
		my_nRecvingDataSize[id] = SIZE_SOCK_HEAD;
		return TRUE;
	}
}

//------------------------------------------
// 業務伝文のデータ部受信を開始
// int id 物理回線ID
//------------------------------------------
BOOL SockOneToAnyRecv::RecvData(int id)
{
	// 受信サイズ決定
	DWORD size = 0;
	//long* p = NULL;
	BYTE* p = NULL;
	while(1) {
		// ヘッダー部の続きでデータ部有り
		if( STATE_SUB_NONE >= my_emRecvAddId[id] && SIZE_SOCK_HEAD < my_pHead[id]->base_head_addr.len_head ) {
			my_emRecvAddId[id] = STATE_SUB_HEAD_EX;
			size = my_pHead[id]->base_head_addr.len_head - SIZE_SOCK_HEAD;
			//p = &((long*)my_pHead[id])[ SIZE_SOCK_HEAD ];
			p = &((BYTE*)my_pHead[id])[ SIZE_SOCK_HEAD ];
			break;
		}
		// 
		if( STATE_SUB_HEAD_EX >= my_emRecvAddId[id] && 0 != my_pHead[id]->base_head_addr.len_addr[0]  ) {
			my_emRecvAddId[id] = STATE_SUB_ADDR0;
			size = my_pHead[id]->base_head_addr.len_addr[0];
			p = (BYTE*)my_pHead[id]->base_head.addr[0]; 
			break;
		}
		//
		if( STATE_SUB_ADDR0 >= my_emRecvAddId[id] && 0 != my_pHead[id]->base_head_addr.len_addr[1]  ) {
			my_emRecvAddId[id] = STATE_SUB_ADDR1;
			size = my_pHead[id]->base_head_addr.len_addr[1];
			p = (BYTE*)my_pHead[id]->base_head.addr[1]; 
			break;
		}
		break;
	}


	//▲ LOG(em_MSG), "[%s] <%s> (%d) データ部受信開始 [size=%d, sub=%d]", my_sThreadName, my_cSession, id, size, my_emRecvAddId[id]);


	// データ部受信開始
	int iRet = mcls_pSock->Recv(id, p, size, my_evRecv[id]);
	if( 0 != iRet ) {
		LOG(em_ERR), "[%s] <%s> (%d) データ部受信エラー [size=%d, err_code=%d]", my_sThreadName, my_cSession, id, size, iRet);
		syslog(SYSNO_SOCK_RECV_ERR, "[(DATA) <%s> id=%d size=%d err_code=%d]", my_cSession, id, size, iRet);
		return FALSE;
	} else {
		// ステータス変更
		my_emSockState[id] = STATE_DATA;
		my_nRecvingDataSize[id]	+= size;		// 受信サイズ

		return TRUE;
	}
}

//------------------------------------------
// 受信データのチェック
// int id 物理回線ID
// 戻り値 -1:切断  0:何もしない
//------------------------------------------
BOOL SockOneToAnyRecv::RecvCheck(int id)
{
	if( my_emSockState[id] != STATE_HEAD && STATE_DATA != my_emSockState[id]) {
		LOG(em_MSG), "[%s] <%s> (%d) 受信完了イベント時ソケットモード異常 [%d]", my_sThreadName, my_cSession, id, my_emSockState[id]);
		return FALSE;
	}
	my_bHelcyOk[id] = true;		// 何かしら受信できた。


	//----------------
	// ヘルシー？
	if( my_emSockState[id] == STATE_HEAD ) {

		if( 0 == memcmp( my_RecvWkHead[id].base_head.code, T_CODE_HELCY, SIZE_SOCK_TC ) ) {	
			// ヘルシー伝文受信
			Recv_Helcy();

			// 次伝文受信
			if( ! RecvHead(id) ) return FALSE;
			return TRUE;
		}

		// ヘッダー部のみの伝文
		if( STATE_HEAD == my_emSockState[id] && SIZE_SOCK_HEAD == my_RecvWkHead[id].base_head.len ) {
			if( ! GetFreeQue(id) ) return FALSE;
			// キュー登録
			AddPoolQue(id);


			// 次伝文受信
			if( ! RecvHead(id) ) return FALSE;
			return TRUE;
		}
	}


	//----------------
	// 1伝文終わり
		// データ部取込完了
	if( STATE_DATA == my_emSockState[id] && my_nRecvingDataSize[id]  == my_RecvWkHead[id].base_head.len ) {

//▲		LOG(em_MSG), "[%s] <%s> (%d) データ部受信完了 [size=%d, sub=%d]", my_sThreadName, my_cSession, id, my_RecvWkHead[id].base_head.len, my_emRecvAddId[id]);
		mcls_Time.PutTimestamp(1);
		//LOG(em_INF), "[%s] <%s> (%d) 受信時間 %.3f", my_sThreadName, my_cSession, id, mcls_Time.GetTimestamp(1, 0));

		// キュー登録
		AddPoolQue(id);

		// 次伝文受信
		if( ! RecvHead(id) ) return FALSE;
		return TRUE;
	}

	//----------------
	// データ部読込中
	if( STATE_DATA == my_emSockState[id] ) {
		mcls_Time.PutTimestamp(0);
		if( ! RecvData(id)) return FALSE;
		return TRUE;
	}


	//----------------
	// ここまで来たらデータ部読込開始
	if( ! GetFreeQue(id) ) return FALSE;
	if( ! RecvData(id)) return FALSE;
	return TRUE;
}


//------------------------------------------
// 新規キュー取得
// int id 物理回線ID
//------------------------------------------
bool SockOneToAnyRecv::GetFreeQue(int id)
{
	// 空きキュー取り出し
	my_pHead[id] = mque_pDeli->GetFree(0);
	if( NULL == my_pHead[id] ) {
		LOG(em_ERR), "[%s] <%s> (%d) 空きキュー無し！伝文消失", my_sThreadName, my_cSession, id);
		return FALSE;
	}

	// ポインタ部分は元を保持(重要)
	long*			addr[2] ={ my_pHead[id]->base_head.addr[0], my_pHead[id]->base_head.addr[1] };

	// ヘッダー部はコピー
	memcpy( my_pHead[id], &my_RecvWkHead[id], SIZE_SOCK_HEAD);

	// ポインタの戻し
	my_pHead[id]->base_head.addr[0] = addr[0];
	my_pHead[id]->base_head.addr[1] = addr[1];
	return TRUE;
}

//------------------------------------------
// 受信キューの登録
// int id 物理回線ID
//------------------------------------------
void SockOneToAnyRecv::AddPoolQue(int id)
{
	// キュー登録
	mque_pDeli->AddPool(my_pHead[id]);
//▲	mque_pDeli->AddFree(my_pHead[id]);

	my_pHead[id] = NULL;
	return;
}

//------------------------------------------
// ヘルシー伝文受信完了 (ヘッダー部のみ)
//------------------------------------------
void SockOneToAnyRecv::Recv_Helcy()
{
	// やり方を変えたから不要
	//// 一度キャンセル
	//CancelWaitableTimer(my_evHelcy);

	//// ヘルシー伝文タイムー監視開始
	//__int64 ts = (__int64)PARAM_TIMEOUT_HELCY_RECV * -10000;	// 1μ秒→100ナノ秒 (マイナス指定で、指定時間経過でシグナル) (第3引数は2回目以降の起動間隔)
	//SetWaitableTimer(my_evHelcy, (LARGE_INTEGER *)&ts, 0, NULL, NULL, FALSE);
}


// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// スレッド制御

//------------------------------------------
// スレッド開始前処理 (スレッド実行直後(シグナルセット前)にコールバック)
//------------------------------------------
int SockOneToAnyRecv::ThreadFirst()
{
	mcls_Time.InitAve();

	// ヘルシー起動
	__int64 ts = (__int64)PARAM_TIMEOUT_HELCY_RECV * -10000;	// 1μ秒→100ナノ秒 (マイナス指定で、指定時間経過でシグナル) (第3引数は2回目以降の起動間隔)
	SetWaitableTimer(my_tmHelcy, (LARGE_INTEGER *)&ts, PARAM_TIMEOUT_HELCY_RECV, NULL, NULL, FALSE);


	//// イベント設定
	AddEventNum(PARAM_SOCK_MAX, my_evRecv);
	AddEventNum(PARAM_SOCK_MAX, my_evDisConnect);
	AddEventNum(PARAM_SOCK_MAX, my_evRecvTimeOut);
	AddEventNum(1, &my_evCloseReq);
	AddEventNum(1, &my_evAccept);
	AddEventNum(1, &my_tmHelcy);

	while( TRUE ) {
		if( Listen(true) ) break;
		Sleep(2000);
	}

	return 0;
}

//------------------------------------------
// スレッド終了前処理 (終了シグナル後にコールバック)
//------------------------------------------
int SockOneToAnyRecv::ThreadLast()
{
	// ヘルシー伝文タイムー監視終了
	CancelWaitableTimer(my_tmHelcy);
	return 0;
}

//------------------------------------------
// スレッドイベント発生
// int nEventNo イベントNo (0オリジン)
//------------------------------------------
void SockOneToAnyRecv::ThreadEvent(int nEventNo)
{
	int id = 0;										// 物理回線ID (0オリジン)

	////// シグナルの条件
	enum {	EV_RECV				= 0,								// 受信完了通知			(PARAM_SOCK_MAX分)
			EV_DISCONNECT		= EV_RECV+PARAM_SOCK_MAX,			// 切断通知				(PARAM_SOCK_MAX分)
			EV_RECVTIMEOUT		= EV_DISCONNECT+PARAM_SOCK_MAX,		// 受信タイムアウト		(PARAM_SOCK_MAX分)
			EV_CLOSEREQ			= EV_RECVTIMEOUT+PARAM_SOCK_MAX,	// 回線切断要求
			EV_ACCEPT,												// アクセプト通知
			EV_HELCY,												// ヘルシータイマー
			EV_LAST
	};
	

	////// シグナル条件分け
	//======================================================
	// 受信完了通知 (PARAM_SOCK_MAX分)
	if(nEventNo >= EV_RECV && nEventNo < EV_RECV+PARAM_SOCK_MAX) {
		id = nEventNo - EV_RECV;
	
		if( ! RecvCheck(id) ) {	
			Close(id);
		}

	//======================================================
	// 切断通知 (PARAM_SOCK_MAX分)
	} else if(nEventNo >= EV_DISCONNECT && nEventNo < EV_DISCONNECT+PARAM_SOCK_MAX) {
		id = nEventNo - EV_DISCONNECT;
		Close(id);
		LOG(em_MSG), "[%s] <%s> (%d) 回線切断", my_sThreadName, my_cSession, id );

	//======================================================
	// 受信タイムアウト (PARAM_SOCK_MAX分)
	} else if(nEventNo >= EV_RECVTIMEOUT && nEventNo < EV_RECVTIMEOUT+PARAM_SOCK_MAX) {

	//======================================================
	// 回線切断要求 (PARAM_SOCK_MAX分)
	} else if(nEventNo == EV_CLOSEREQ) {
		CloseAll(true);
		LOG(em_MSG), "[%s] <%s> (%d) 全回線切断", my_sThreadName, my_cSession, id );

	//======================================================
	// アクセプト通知
	} else if(nEventNo == EV_ACCEPT)	{
		int id = Accept();
		if( 0 > id ) {
			LOG(em_ERR), "[%s] <%s> (%d) クライアント数オーバー", my_sThreadName, my_cSession, id);
			return;
		}
		LOG(em_MSG), "[%s] <%s> (%d) アクセプト シグナル", my_sThreadName, my_cSession, id);

		
		// ヘッダー読込開始
		if( ! RecvHead(id) ) {
			LOG(em_ERR), "[%s] <%s> (%d) ACCEPT後の初回読込失敗", my_sThreadName, my_cSession, id);
			Close(id);
		}
	
	//======================================================
	// ヘルシー通知
	} else if(nEventNo == EV_HELCY)	{
		for(int ii=0; ii<PARAM_SOCK_MAX; ii++) {
			if( ! my_bHelcyOk[ii] && IsConnect(ii) ) {
				LOG(em_ERR), "[%s] <%s> (%d) ヘルシー伝文タイムアウト", my_sThreadName, my_cSession, ii);
				Close(ii);
			}
			my_bHelcyOk[ii] = false;
		}
	}

}