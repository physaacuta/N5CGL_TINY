#include "StdAfx.h"
#include "BasePipe.h"

//------------------------------------------
// コンストラクタ
//------------------------------------------
BasePipe::BasePipe(void) :
my_sThreadName("BasePipe"),
my_nConnect(0),
my_bIsServer(false),
my_sPipeName(""),
mcls_pPipe(NULL),
mcls_pLog(NULL)
{
	memset(&my_ovlSend,	0, sizeof(OVERLAPPED));
	memset(&my_ovlRecv,	0, sizeof(OVERLAPPED));
	memset(&my_ovlAccept,0, sizeof(OVERLAPPED));
	my_ovlSend.hEvent	= CreateEvent(NULL, TRUE, FALSE, NULL); // manual, reset
	my_ovlRecv.hEvent	= CreateEvent(NULL, TRUE, FALSE, NULL); // manual, reset
	my_ovlAccept.hEvent	= CreateEvent(NULL, FALSE, FALSE, NULL); // auto, reset

	InitializeCriticalSection(&my_cs);
}

//------------------------------------------
// デストラクタ
//------------------------------------------
BasePipe::~BasePipe(void)
{
	CloseHandle(my_ovlSend.hEvent);
	CloseHandle(my_ovlRecv.hEvent);
	CloseHandle(my_ovlAccept.hEvent);

	DeleteCriticalSection(&my_cs);
}

// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// パイプ操作

//------------------------------------------
// コネクト (接続されるまでブロックされる)
//------------------------------------------
bool BasePipe::Connect()
{
	if(my_bIsServer) return false;
	if(mcls_pPipe) return true;		// 既に接続済み

	//// 接続処理
	try {
		mcls_pPipe = new PipeManager(my_sPipeName, my_bIsServer);
		my_nConnect = 1;
		return true;
	} catch(int code) {
		LOG(em_WAR), "[%s] PIPE インスタンス生成失敗 [%d]", my_sThreadName, code);
		mcls_pPipe = NULL;
		return false;
	}
}

//------------------------------------------
// リスニング (ノンブロック)
//  サーバー時の接続関数
//------------------------------------------
bool BasePipe::Listen()
{
	if( ! my_bIsServer) return false;

	// インスタンスがまだなければNew
	if( NULL == mcls_pPipe) {
		try {
			mcls_pPipe = new PipeManager(my_sPipeName, my_bIsServer);
		} catch(int code) {
			LOG(em_WAR), "[%s] PIPE インスタンス生成失敗 [%d]", my_sThreadName, code);
			mcls_pPipe = NULL;
			return false;
		}
	}

	// リスニング
	DWORD ret = mcls_pPipe->Listen(&my_ovlAccept);
	if(0 != ret) {
		LOG(em_WAR), "[%s] PIPE リスニング失敗 [%d]", my_sThreadName, ret);
		if (ERROR_PIPE_CONNECTED == ret) {
			// 接続済み(通常ありえない)
			return false;
		}
		return false;
	}

	return true;
}

//------------------------------------------
// アクセプト完了
//------------------------------------------
void BasePipe::Accepting()
{
	EnterCriticalSection(&my_cs);

	// 状態変更
	my_nConnect += 1;

	LeaveCriticalSection(&my_cs);
}

//------------------------------------------
// クローズ
//------------------------------------------
void BasePipe::Close()
{
	EnterCriticalSection(&my_cs);

	// 接続を全部解除する
	if( NULL != mcls_pPipe) {
		mcls_pPipe->Cancel();		// 未処理のI/Oを取り消す
		delete mcls_pPipe;			// インスタンス開放
		mcls_pPipe = NULL;
	}

	// ハンドルも一旦シグナルを落とす
	ResetEvent(my_ovlSend.hEvent);
	ResetEvent(my_ovlRecv.hEvent);
	ResetEvent(my_ovlAccept.hEvent);

	// 状態変更
	my_nConnect = 0;

	LeaveCriticalSection(&my_cs);
}

//------------------------------------------
// ヘッダー部受信開始 (非同期)
//------------------------------------------
bool BasePipe::StartRecvHeader()
{
	bool ret;
	if( 0 == my_nConnect ) return false;

	EnterCriticalSection(&my_cs);

	memset(&my_cRecvHeadBuf, 0x00, sizeof(my_cRecvHeadBuf));
	my_bRecvHead = true;
	ret = mcls_pPipe->Recv(&my_cRecvHeadBuf, sizeof(my_cRecvHeadBuf), &my_ovlRecv);
	if(!ret) {
		if( ERROR_MORE_DATA == mcls_pPipe->GetError() ) {	// オープン後にRecv前にデータが既に送信されている場合、引き続きデータ部にもデータがあるというワーニングが出る対策
			ret = true;
			goto Ending;
		}
		LOG(em_ERR), "[%s] PIPE ヘッダー部読み込み失敗", my_sThreadName);
	}
Ending:
	LeaveCriticalSection(&my_cs);
	return ret;
}

//------------------------------------------
// ヘッダー部受信完了時
//------------------------------------------
DWORD BasePipe::EndRecvHeader()
{
	DWORD	ret;
	if( 0 == my_nConnect ) return ERROR_BROKEN_PIPE;	// 切断
	
	if( mcls_pPipe == NULL ) {
		ret = ERROR_BROKEN_PIPE;	// 切断
	} else {
		if( mcls_pPipe->GetResult(&my_ovlRecv) ) {
			ret = ERROR_SUCCESS;
		} else {
			ret = mcls_pPipe->GetError();
		}
	}

	ResetEvent(my_ovlRecv.hEvent);					// シグナルマニュアルリセット
	return ret;
}


//------------------------------------------
// データ部受信 (同期)
// BYTE* buf 受信バッファのポインタ
// DWORD len 残りの長さ
//------------------------------------------
DWORD BasePipe::GetRecvBody(BYTE* buf, DWORD len)
{
	if( 0 == my_nConnect ) return ERROR_BROKEN_PIPE;	// 切断

	// 同期で残りのデータを取得
	return mcls_pPipe->Recv(buf, len);
}

//------------------------------------------
// データ送信 (同期)
// BYTE* buf 送信バッファのポインタ
// DWORD len 送信データ
// int nMaxWait 最大待ち時間 [ms]
//------------------------------------------
bool BasePipe::SendLock(BYTE* buf, DWORD len, int nMaxWait)
{
	bool ret;
	if( 0 == my_nConnect ) return false;

	EnterCriticalSection(&my_cs);

	ret = mcls_pPipe->Send(buf, len, &my_ovlSend);
	if(!ret) {
		LOG(em_ERR), "[%s] PIPE 同期送信失敗", my_sThreadName);
	} else {
		// 送信完了まで待ち
		DWORD dwTransByte;
		DWORD dwWaitRet = WaitForSingleObject(my_ovlSend.hEvent, nMaxWait);
		ret = !! GetOverlappedResult(mcls_pPipe->GetPipeHandle(), &my_ovlSend, &dwTransByte, FALSE);
		if (WAIT_OBJECT_0 == dwWaitRet) ResetEvent(my_ovlSend.hEvent);			// 正常に送信できたからシグナルリセット
	}

	LeaveCriticalSection(&my_cs);

	return ret;
}

//------------------------------------------
// データ送信 (非同期)
// BYTE* buf 送信バッファのポインタ
// DWORD len 送信データ
// int nMaxWait 最大待ち時間 [ms]
//------------------------------------------
bool BasePipe::SendUnLock(BYTE* buf, DWORD len)
{
	return mcls_pPipe->Send(buf, len, &my_ovlSend);;
}