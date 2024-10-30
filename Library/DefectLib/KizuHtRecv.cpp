// KizuHtRecv.cpp: KizuHtRecv クラスのインプリメンテーション
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "KizuHtRecv.h"

#define LOGKIZUHTRECV(t) if(mcls_pLogD!=NULL)     mcls_pLogD->Write(KizuLib::LogFileManager::t		// 調査用ログ出力

//////////////////////////////////////////////////////////////////////
// 構築/消滅
//////////////////////////////////////////////////////////////////////
//------------------------------------------
// コンストラクタ
// const char *cSession セッション文字列
// int torb 表示識別フラグ(0:表, 1:裏)
// int accept_num クライアント数 (物理回線数)
// int recv_num フレーム単位での受信数 (論理回線数) (通常accept_numと同じ。複数の幅方向カメラを1台の判定で処理する場合に変わってくる)
// int recv_max_size 受信バッファの最大値 (デフォルト32M)
//------------------------------------------
KizuHtRecv::KizuHtRecv(const char *cSession, int torb, int accept_num, int recv_num, int recv_max_size) :
my_nThSock(0),
my_cSession(cSession), 
my_nTorB(torb),
my_nAcceptNum(accept_num),
my_nRecvNum(recv_num),
my_bRun(FALSE),
PARAM_SOCK_RECV_DATASIZE(recv_max_size),	// Ver.1.3.0.0
mcls_pSock(NULL),
mcls_pLog(NULL),
mcls_pLogD(NULL)
{
	//// 初期化
	for(int ii=0; ii<PARAM_SOCK_MAX; ii++) {
		memset(my_cRecvWkHead, 0x00, sizeof(my_cRecvWkHead));
		memset(my_cRecvHead, 0x00, sizeof(my_cRecvHead));
	}

	// 物理回線数でループ
//	for(int ii=0; ii<my_nAcceptNum; ii++) {
	for(int ii=0; ii<PARAM_SOCK_MAX; ii++) {
		// イベント
		my_evDisConnect[ii]	 = CreateEvent(NULL, FALSE, FALSE, NULL);	// 切断イベント認識(ID)
		my_evRecvTimeOut[ii] = CreateEvent(NULL, FALSE, FALSE, NULL);	// 受信タイムアウトイベント認識(ID)
		my_evRecv[ii]		 = CreateEvent(NULL, FALSE, FALSE, NULL);	// 受信イベント認識(ID)
	}

	// 論理回線数でループ
	for(int ii=0; ii<my_nRecvNum; ii++) {
		// 受信データ
		my_cRecvData[ii]	= new BYTE[PARAM_SOCK_RECV_DATASIZE];		// 受信データ(RECVID)
		memset(my_cRecvData[ii], 0x00, PARAM_SOCK_RECV_DATASIZE);
	}


	// イベント
	my_evConnectAll 		= CreateEvent(NULL, FALSE, FALSE, NULL);	// 全クライアント接続完了時
	my_evErrToClose 		= CreateEvent(NULL, FALSE, FALSE, NULL);	// 異常発生により クライアント切断時
	my_evDetectWaitStart	= CreateEvent(NULL, FALSE, FALSE, NULL);	// 検査開始待ち要求シグナル
	my_evDetectRun			= CreateEvent(NULL, FALSE, FALSE, NULL);	// 検査開始要求シグナル
	my_evDetectStop			= CreateEvent(NULL, FALSE, FALSE, NULL);	// 検査停止要求シグナル
	my_evAccept				= CreateEvent(NULL, FALSE, FALSE, NULL);	// 各判定PCよりAccept時

	//// リセット(初期化)
	Reset();
}

//------------------------------------------
// デストラクタ
//------------------------------------------
KizuHtRecv::~KizuHtRecv()
{
	//// ソケット解放
	if( NULL != mcls_pSock ) {
		delete mcls_pSock;
		mcls_pSock = NULL;
	}
	//// 資源開放
	// 物理的な数でループ
	for(int ii=0; ii<PARAM_SOCK_MAX; ii++) {
		// イベント
		CloseHandle(my_evDisConnect[ii]);
		CloseHandle(my_evRecvTimeOut[ii]);
		CloseHandle(my_evRecv[ii]);
	}
	// 論理的な数でループ
	for(int ii=0; ii<my_nRecvNum; ii++) {
		// 受信データ
		delete [] my_cRecvData[ii];
		my_cRecvData[ii] = NULL;
	}

	//// イベント
	CloseHandle(my_evConnectAll);	// 全クライアント接続完了時
	CloseHandle(my_evErrToClose);	// 異常発生により クライアント切断時
	CloseHandle(my_evDetectWaitStart);
	CloseHandle(my_evDetectRun);
	CloseHandle(my_evDetectStop);
	CloseHandle(my_evAccept);	// 各判定PCよりAccept時
}

//------------------------------------------
// 現在のステータス名称
// int id 物理回線ID
//------------------------------------------
CString KizuHtRecv::GetSockStateName(int id) const
{
	CString wk;
	if		(my_emSockState[id] == STATE_NONE)	{ wk = "初期状態 (切断中)"; }
	else if (my_emSockState[id] == STATE_IDLE)	{ wk = "準備完了 (接続中)"; }
	else if (my_emSockState[id] == STATE_HEAD)	{ wk = "ヘッダ部 受信中";}
	else if (my_emSockState[id] == STATE_DATA)	{ wk = "データ部 受信中";}
	else										{ wk = "ステータス異常";}
	return wk;
}

// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ステータス制御
//------------------------------------------
// クライアント名称をセット (これで判定PCからのイニシャル伝文を判断してどの判定機能と繋がったか判断する)
// int id 0オリジン (DS,DQ…の順)
// char const* cVal クライアント名
//------------------------------------------
void KizuHtRecv::SetCliantName(int id, char const* cVal)
{
	sprintf(my_cClientName[id], cVal);
}


// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// シーケンス制御

//------------------------------------------
// すべてのヘッダー受信を開始
//------------------------------------------
BOOL KizuHtRecv::RecvAllHeader()
{
	int ii;
	//// 開始可能かチェック
	for(ii=0; ii<my_nAcceptNum; ii++) {
		if( my_bRun && my_PC[ii] < 0 ) {					// PCID未割り当て
			LOG(em_ERR), "[KizuHtRecv] <%s> 全クライントが繋がっていないためヘッダ受信中止 [接続数=%d]", my_cSession, ii);
			//syslog(206, "[接続数=%d]", ii);
			// 全てのクライアントを切断
			CloseAll();
			return FALSE;

		} else if( STATE_IDLE != my_emSockState[ii] ) {		// ヘッダー受信可能状態でない
			LOG(em_ERR), "[KizuHtRecv] <%s> ヘッダー受信可能状態でない [PCID=%d status=%d]", my_cSession, my_PC[ii], my_emSockState[ii]);
			//syslog(207, "[PCID=%d status=%d]", my_PC[ii], my_emSockState[ii]);
			// 全てのクライアントを切断
			CloseAll();
			return FALSE;
		}
	}

	//// データ受信開始準備
	for(ii=0; ii<my_nRecvNum; ii++) {			// 論理回線単位
		my_bDataReady[ii] = false;						// データ受信待ち
	}
	for(ii=0; ii<my_nAcceptNum; ii++) {			// 物理回線単位
		my_nRecvingDataSize[ii] = 0;
		my_nRecvNowCnt[ii] = 0;
	}

	//// データ受信開始
	for(ii=0; ii<my_nAcceptNum; ii++) {
		// ヘッダー受信開始
		if( ! RecvHeaderDefect(ii) ) {
			return FALSE;
		}
	}
	return TRUE;
}

//------------------------------------------
// 業務伝文 ヘッダー部受信完了
// int id ID
// 戻り値： -1:異常 0～:RECVID=CAMPOS 
//------------------------------------------
int KizuHtRecv::AnsHeader(int id)
{
	_ASSERT(NULL != mcls_pSock);

	//// ホントに業務伝文か確認
	if(0 != memcmp( my_cRecvWkHead[id].base_head.code, T_CODE_DEFECT, SIZE_SOCK_TC) ) {
		LOG(em_ERR), "[KizuHtRecv] <%s> 業務伝文伝文 トランザクションコード異常[name=%s code=%4.4s]", my_cSession, my_cClientName[my_PC[id]-1], my_cRecvWkHead[id].base_head.code);
		syslog(SYSNO_SOCK_TC_NG, "[TC異常 name=%s code=%4.4s]", my_cClientName[my_PC[id]-1], my_cRecvWkHead[id].base_head.code);
		// 全てのクライアントを切断
		CloseAll();
		return -1;
	}

	//// 論理回線IDは正常か
	if( 0 > my_cRecvWkHead[id].base_head.subinf || my_nRecvNum <= my_cRecvWkHead[id].base_head.subinf ) {
		LOG(em_ERR), "[KizuHtRecv] <%s> 業務伝文伝文 論理回線ID異常[name=%s id=%d]", my_cSession, my_cClientName[my_PC[id]-1], my_cRecvWkHead[id].base_head.subinf);
		syslog(SYSNO_SOCK_TC_NG, "[論理回線ID異常 name=%s id=%d]", my_cClientName[my_PC[id]-1], my_cRecvWkHead[id].base_head.subinf);
		// 全てのクライアントを切断
		CloseAll();
		return -1;
	}

	//// データ部受信開始の準備
	int recvid = my_cRecvWkHead[id].base_head.subinf;								// 論理回線ID
	DWORD sz   = my_cRecvWkHead[id].base_head.len - SIZE_SOCK_HEAD;					// 受信サイズ確認
	memcpy( &my_cRecvHead[recvid], &my_cRecvWkHead[id], sizeof(SOCK_F_HEAD));		// ワークヘッダーの情報を 論理回線順に並び替え
	my_nRecvingDataSize[id] = 0;													// データ部受信サイズ初期化


	//// 業務伝文受信
	//▲	LOG(em_INF), "[KizuHtRecv] データ部受信 [%s, %d]", my_cClientName[my_PC[id]-1], sz);//▲
//LOGKIZUHTRECV(em_INF), "[KizuHtRecv] <%s> データ部受信 [%s, %d]", my_cSession, my_cClientName[my_PC[id]-1], sz);

	_ASSERT(sz >= 0);
	if( 0 == sz ) {					// ヘッダー部のみ
		// 業務伝文 受信完了チェック
		if( AnsData(id) ) {
			return  recvid;
		} else {
			return -1;
		}
	}

	//// データ部がある場合は、続けてデータ部を受信
	if( RecvDataDefect(id, recvid) ) {
		return recvid;
	} else {
		return -1;
	}
}

//------------------------------------------
// 業務伝文 受信完了チェック
// int id ID
//------------------------------------------
BOOL KizuHtRecv::AnsData(int id)
{
	int recvid = my_cRecvWkHead[id].base_head.subinf;		// 論理回線ID

	//// 論理回線単位でホントにデータを全部受信できた？ (分割送信対応)
	if(my_cRecvWkHead[id].base_head.len > my_nRecvingDataSize[id]+SIZE_SOCK_HEAD) {

//LOG(em_INF), "[KizuHtRecv] <%s> 伝文分割受信 id=%d reccvid=%d size=%d total=%d", my_cSession, id, recvid, my_nRecvingDataSize[id], my_cRecvWkHead[id].base_head.len);
//LOGKIZUHTRECV(em_INF), "[KizuHtRecv] <%s> 伝文分割受信 id=%d reccvid=%d size=%d total=%d", my_cSession, id, recvid, my_nRecvingDataSize[id], my_cRecvWkHead[id].base_head.len);

		// まだデータ部の受信途中
		return RecvDataDefect(id, recvid);
	} 


	//// 論理回線単位で受信完了
	my_bDataReady[recvid] = true;	
	my_nRecvNowCnt[id] += 1;

//	LOG(em_INF), "[KizuHtRecv] <%s> 論理回線受信完了 id=%d reccvid=%d", my_cSession, id, recvid);
	//// 物理回線単位でデータ受信完了？
	if( my_nRecvNowCnt[id] < my_nRecvMaxCnt[id] ) {
//LOG(em_INF), "[KizuHtRecv] <%s> 複数論理回線受信開始 id=%d reccvid=%d", my_cSession, id, recvid);

		// 次論理回線のヘッダー部受信開始
		return RecvHeaderDefect(id); 
	}

//▲	LOG(em_INF), "[KizuHtRecv] <%s> 物理回線受信完了 id=%d reccvid=%d", my_cSession, id, recvid);
LOGKIZUHTRECV(em_INF), "[KizuHtRecv] <%s> 物理回線受信完了 id=%d reccvid=%d", my_cSession, id, recvid);

	//// ここまで来たら、この物理回線での１フレーム分の情報は全て受信完了
	my_emSockState[id] = STATE_IDLE;				// ステータス変更
	return TRUE;
}

//------------------------------------------
// イニシャル伝文 受信完了
// int id ID
//------------------------------------------
BOOL KizuHtRecv::AnsInitializeSocket(int id)
{
	// ホントにイニシャル伝文か確認
	if(0 != memcmp( my_cRecvWkHead[id].base_head.code, T_CODE_NAME, SIZE_SOCK_TC) ) {
		LOG(em_ERR), "[KizuHtRecv] <%s> イニシャル伝文 トランザクションコード異常[code = %4.4s]", my_cSession, my_cRecvWkHead[id].base_head.code);
		syslog(SYSNO_SOCK_TC_NG, "[section=%s code=%4.4s]", my_cSession, my_cRecvWkHead[id].base_head.code);
		// 異常通知
		CloseAll();
		return FALSE;
	}
	//LOG(em_WAR), "[%d] <%s> イニシャル伝文", my_nTorB, my_cRecvWkHead[id].base_head.pname);
LOGKIZUHTRECV(em_WAR), "[KizuHtRecv] [%d] <%s> イニシャル伝文受信", my_nTorB, my_cRecvWkHead[id].base_head.pname);

	// PCID を決定
	for(int ii=0; ii<my_nAcceptNum; ii++) {
		if(0 != strcmp( my_cRecvWkHead[id].base_head.pname, my_cClientName[ii] ) ) continue;

		// 論理回線数
		int recvcnt = my_cRecvWkHead[id].base_head.subinf;
		if( 0 >= recvcnt || my_nRecvNum < recvcnt ) {
			LOG(em_ERR), "[KizuHtRecv] <%s> イニシャル伝文 論理回線数異常[name = %s, recvcnt=%d]", my_cSession, my_cRecvWkHead[id].base_head.pname, recvcnt);
			syslog(SYSNO_SOCK_TC_NG, "[論理回線数異常 name=%s recvcnt=%d]", my_cRecvWkHead[id].base_head.pname, recvcnt);
			return FALSE;
		}
		my_nRecvMaxCnt[id] = recvcnt;
		my_nRecvNowCnt[id] = 0;

		// PCID 見つけた
		my_PC[id] = ii+1;
		// ステータス変更
		my_emSockState[id] = STATE_IDLE;

		LOG(em_MSG), "[KizuHtRecv] <%s> イニシャル伝文受信 [%s] ID=%d PCID=%d", my_cSession, my_cRecvWkHead[id].base_head.pname, id, my_PC[id]);
LOGKIZUHTRECV(em_MSG), "[KizuHtRecv] <%s> イニシャル伝文受信 [%s] ID=%d PCID=%d", my_cSession, my_cRecvWkHead[id].base_head.pname, id, my_PC[id]);

		syslog(SYSNO_SOCK_INITIALIZ, "[name=%s]", my_cRecvWkHead[id].base_head.pname);
		return TRUE;
	}

	// PCID未割り当て
	LOG(em_ERR), "[KizuHtRecv] <%s> イニシャル伝文 PCID未割り当て[name = %s]", my_cSession, my_cRecvWkHead[id].base_head.pname);
LOGKIZUHTRECV(em_ERR), "[KizuHtRecv] <%s> イニシャル伝文 PCID未割り当て[name = %s]", my_cSession, my_cRecvWkHead[id].base_head.pname);
	syslog(SYSNO_SOCK_TC_NG, "[PCID未割り当て name=%s]", my_cRecvWkHead[id].base_head.pname);
	return FALSE;
}

//------------------------------------------
// 探傷開始処理
//------------------------------------------
void KizuHtRecv::DefectStart()
{
	_ASSERT(NULL != mcls_pSock);

	mcls_pSock->Cancel();	// 読込中があればキャンセル
	// 初期化
	for(int ii=0; ii<my_nAcceptNum; ii++) {
		my_emSockState[ii] = STATE_IDLE;
	}
	for(int ii=0; ii<my_nRecvNum; ii++) {
		my_nFrameNo[ii] = -1;
		my_bDataReady[ii] = false;
	}

	my_bRun = TRUE;
}
//------------------------------------------
// 探傷停止処理
//------------------------------------------
void KizuHtRecv::DefectStop()
{
	my_bRun = FALSE;
}

//------------------------------------------
// フレームNoの連番チェック
// DWORD framenum 今回受信したフレームNo (0オリジン)
// int id 物理回線ID
// int recvid 論理回線ID
//------------------------------------------
BOOL KizuHtRecv::CheckFrameNo(DWORD framenum, int id, int recvid)
{
	// 探傷中以外は正常
	if( !my_bRun ) return TRUE;

	// フレームNoチェック
	if( my_nFrameNo[recvid] + 1 == framenum ) {
		my_nFrameNo[recvid] = framenum;
		return TRUE;
	}
	
	// フレームNo食い違いエラー
	LOG(em_ERR), "[KizuHtRecv] <%s> フレームNo食い違い発生 [%s, recvid=%d pre=%d, now=%d]", my_cSession, my_cClientName[my_PC[id]-1], recvid, my_nFrameNo[recvid], framenum);
LOGKIZUHTRECV(em_ERR), "[KizuHtRecv] <%s> フレームNo食い違い発生 [%s, recvid=%d pre=%d, now=%d]", my_cSession, my_cClientName[my_PC[id]-1], recvid, my_nFrameNo[recvid], framenum);
	//syslog(250, "[%s: recvid=%d pre=%d now=%d]", my_cClientName[my_PC[id]-1], recvid, my_nFrameNo[recvid], framenum);

	// 全てのクライアントを切断
	CloseAll();
	return FALSE;
}

//------------------------------------------
// 全判定PCから受信完了
// 戻り値 TRUE:全判定PC読込完了
//------------------------------------------
BOOL KizuHtRecv::CheckAllReadEnd()
{
	// 探傷停止中はダメ
	if( !my_bRun ) return FALSE;

	// 業務伝文受信済み？
	for(int ii=0; ii<my_nRecvNum; ii++) {
		if( !my_bDataReady[ii] ) return FALSE;
	}
	return TRUE;
}

// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// TPC/IP 通信制御
//------------------------------------------
// リセット
//------------------------------------------
void KizuHtRecv::Reset()
{
	//// 現在の読込キャンセル
//	my_bRun = FALSE;
	if(mcls_pSock) mcls_pSock->Cancel();

	//// 初期化
	memset( my_cRecvWkHead, 0x00, sizeof(my_cRecvWkHead));
	memset( my_cRecvHead, 0x00, sizeof(my_cRecvHead));

	// 論理回線数でループ
	for(int ii=0; ii<my_nRecvNum; ii++) {
		my_nFrameNo[ii] = -1;
		my_bDataReady[ii] = false;
	}
	// 物理回線数でループ
	for(int ii=0; ii<my_nAcceptNum; ii++) {
		my_emSockState[ii] = STATE_NONE;
		memset( my_cClientName[ii], 0x00, SIZE_NAME_PC);
		my_PC[ii] = -1;
		my_nRecvingDataSize[ii]	= 0;
		my_nRecvMaxCnt[ii]		= 0;
		my_nRecvNowCnt[ii]		= 0;
		ResetEvent(my_evRecv[ii]);
	}
}

//------------------------------------------
// Listen
// bool bLogDsp ログ出力有無 (false:失敗ログ出さない)
//------------------------------------------
BOOL KizuHtRecv::Listen(bool bLogDsp)
{
	int retc;
	// ソケットインスタンス生成
	try {
		mcls_pSock = new SockBase(my_cSession, TASKINI_NAME, my_nThSock, PARAM_SOCK_TIMEOUT,
										 my_nAcceptNum, PARAM_SOCK_RECV_BUFSIZE);
	} catch(int code) {
		mcls_pSock = NULL;
		if(bLogDsp) {
			LOG(em_ERR), "[KizuHtRecv] <%s> ソケット New失敗 err_code=%d", my_cSession, code);
			syslog(SYSNO_SOCK_NEW_ERR, "[%s: err_code=%d]", my_cSession, code);
		}
		return FALSE;
	}

	// Listen
	if( NULL != mcls_pSock ) {
		mcls_pSock->SetEvAccept(my_evAccept);							// Accept認識をイベントにする (サーバーのみ)

		retc = mcls_pSock->Listen();
		if( 0 != retc ) {
			delete mcls_pSock;
			mcls_pSock = NULL;
			if(bLogDsp) {
				LOG(em_ERR), "[KizuHtRecv] <%s> ソケット Listen失敗 err_code=%d", my_cSession, retc);
				syslog(SYSNO_SOCK_LISTEN_ERR, "[%s: err_code=%d]", my_cSession, retc);
			}
			return FALSE;
		}
	}

	LOG(em_MSG), "[KizuHtRecv] <%s> ソケット Listen開始", my_cSession);
	return TRUE;
}

//------------------------------------------
// 全判定正常?
//------------------------------------------
bool KizuHtRecv::AllConnect()
{
	int ii;
	// 全接続？
	for(ii=0; ii<my_nAcceptNum; ii++) {
		if(my_emSockState[ii] == STATE_NONE) break;
	}
	if(ii==my_nAcceptNum) return true;
	else				  return false;
}
/*
//------------------------------------------
// Accept
// MSG* msg WM_KS_ACCEPT時のメッセージ
//------------------------------------------
int KizuHtRecv::Accept(MSG* msg)
{
	_ASSERT(NULL != mcls_pSock);

	// ID取得
	int id = mcls_pSock->Accept(msg);
	if( id < 0 ) {
		LOG(em_ERR), "[KizuHtRecv] <%s> クライアント数オーバー", my_cSession);
		syslog(SYSNO_SOCK_CLIENT_MAX, "[%s]", my_cSession);
		return FALSE;
	}

	// ステータス変更
	my_emSockState[id] = STATE_IDLE;

	// 全接続？
	int ii;
	for(ii=0; ii<my_nAcceptNum; ii++) {
		if(my_emSockState[ii] == STATE_NONE) break;
	}
	if(ii==my_nAcceptNum) SetEvent(my_evConnect);
	return id;
}
*/

//------------------------------------------
// Accept (シグナル通知用)
//------------------------------------------
int KizuHtRecv::Accept()
{
	_ASSERT(NULL != mcls_pSock);

	// ID取得
	int id = mcls_pSock->Accept();
	if( id < 0 ) {
		LOG(em_ERR), "[KizuHtRecv] <%s> クライアント数オーバー", my_cSession);
		syslog(SYSNO_SOCK_CLIENT_MAX, "[%s]", my_cSession);
		return FALSE;
	}

	// 今繋がったソケットの状態を変更
	mcls_pSock->SetEvDisConnect(id, my_evDisConnect[id]);					// 切断認識をイベントにする
	mcls_pSock->SetEvRecvTimeout(id, my_evRecvTimeOut[id]);					// 受信タイムアウト認識をイベントにする

	// ステータス変更
	my_emSockState[id] = STATE_IDLE;

	// 全接続？
/*
	int ii;
	for(ii=0; ii<my_nAcceptNum; ii++) {
		if(my_emSockState[ii] == STATE_NONE) break;
	}
	if(ii==my_nAcceptNum) SetEvent(my_evConnectAll);
*/
	if( AllConnect() ) SetEvent(my_evConnectAll);
	return id;
}

//------------------------------------------
// 全クライアント切断 & 異常通知
// bool bErr 異常通知 (false:異常切断 true:正常切断)
//------------------------------------------
void KizuHtRecv::CloseAll(bool bErr)
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
void KizuHtRecv::ErrCall()
{
	if(NULL == mcls_pSock) return;

	// リセット
	Reset();
	SetEvent(my_evErrToClose);
}

//------------------------------------------
// イニシャル伝文受信用のヘッダー受信 (ようは PCID 未割り当て状態時)
// int id 物理回線ID (Acceptの戻り値を渡す)
//------------------------------------------
BOOL KizuHtRecv::RecvHeaderInitialize(int id)
{
	// ヘッダー受信開始
	int iRet = mcls_pSock->Recv(id, &my_cRecvWkHead[id], SIZE_SOCK_HEAD, my_evRecv[id]);
	if( 0 != iRet ) {
		if( -1 == my_PC[id] ) {
			LOG(em_ERR), "[KizuHtRecv] <%s> ヘッダー受信エラー [err_code=%d]", my_cSession, iRet);
			syslog(SYSNO_SOCK_RECV_ERR, "[(HEAD) section=%s err_code=%d]", my_cSession, iRet);
		} else {
			LOG(em_ERR), "[KizuHtRecv] <%s> ヘッダー受信エラー [err_code=%d]", my_cClientName[my_PC[id]-1],iRet);
			syslog(SYSNO_SOCK_RECV_ERR, "[(HEAD) name=%s err_code=%d]", my_cClientName[my_PC[id]-1], iRet);
		}
		// 異常通知
		CloseAll();
		return FALSE;
	} else {
		// ステータス変更
		my_emSockState[id] = STATE_HEAD;
	}
	return TRUE;
}

//------------------------------------------
// 業務伝文のヘッダー受信を開始
// int id 物理回線ID
//------------------------------------------
BOOL KizuHtRecv::RecvHeaderDefect(int id)
{
	// ヘッダー受信開始
	int iRet = mcls_pSock->Recv(id, &my_cRecvWkHead[id], SIZE_SOCK_HEAD, my_evRecv[id]);
	if( 0 != iRet ) {
		LOG(em_ERR), "[KizuHtRecv] <%s> ヘッダー受信エラー [err_code=%d]", my_cClientName[my_PC[id]-1],iRet);
		syslog(SYSNO_SOCK_RECV_ERR, "[(HEAD) name=%s err_code=%d]", my_cClientName[my_PC[id]-1], iRet);
		// 異常通知
		CloseAll();
		return FALSE;
	} else {
		// ステータス変更
		my_emSockState[id] = STATE_HEAD;
		return TRUE;
	}
}

//------------------------------------------
// 業務伝文のデータ部受信を開始
// int id 物理回線ID
// int recvid 論理回線ID
//------------------------------------------
BOOL KizuHtRecv::RecvDataDefect(int id, int recvid)
{
	// 受信サイズ決定
	DWORD size = my_cRecvWkHead[id].base_head.len - SIZE_SOCK_HEAD - my_nRecvingDataSize[id];		// 受信サイズ
	if(size > SIZE_SOCK_DATA_MAX) size = SIZE_SOCK_DATA_MAX;										// 1回の受信サイズオーバー

	// データ部受信開始
	int iRet = mcls_pSock->Recv(id, &my_cRecvData[recvid][my_nRecvingDataSize[id]], size, my_evRecv[id]);
	if( 0 != iRet ) {
		LOG(em_ERR), "[KizuHtRecv] <%s> データ部受信エラー [%s, recvid=%d, size=%d, now=%d, total=%d, err_code=%d]", my_cSession, my_cClientName[my_PC[id]-1], recvid, size, my_nRecvingDataSize[id], my_cRecvWkHead[id].base_head.len, iRet);
		syslog(SYSNO_SOCK_RECV_ERR, "[(DATA) name=%s recvid=%d size=%d now=%d total=%d err_code=%d]", my_cClientName[my_PC[id]-1], recvid, size, my_nRecvingDataSize[id], my_cRecvWkHead[id].base_head.len, iRet);
		// 全てのクライアントを切断
		CloseAll();
		return FALSE;
	} else {
		// ステータス変更
		my_emSockState[id] = STATE_DATA;
		my_nRecvingDataSize[id] += size;
		return TRUE;
	}
}
