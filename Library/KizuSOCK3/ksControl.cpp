// ksControl.cpp: ksControl クラスのインプリメンテーション
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include <process.h>
#include <crtdbg.h>
#include <winsock2.h>
#include "ksControl.h"

using namespace KizuSock3;

//////////////////////////////////////////////////////////////////////
// 構築/消滅
//////////////////////////////////////////////////////////////////////
//------------------------------------------
// コンストラクタ
// DWORD tid PostThreadMessageの送り先 (ThreadIDを渡せば良い)
// DWORD msgbase メッセージベース番号
//------------------------------------------
ksControl::ksControl(DWORD tid, DWORD msgbase) : 
m_nTid(tid),
m_nMsgBase(msgbase),
m_nInstCount(0),
m_sock(INVALID_SOCKET),
m_hThread(NULL),
m_nTimeout(INFINITE),
m_evRecvTimeout(NULL),
m_evSendTimeout(NULL),
m_evDisConnect(NULL),
m_evAccept(NULL),
m_nKsThPriority(THREAD_PRIORITY_NORMAL)
{
	// 初期化
	for (int ii = 0; ii < KS_MAXCLIENTS; ii++) mcls_pInst[ii] = NULL;
	// イベント生成
	m_evStopEvent		  = CreateEvent(NULL, FALSE, FALSE, NULL);
	m_evAccepting		  = CreateEvent(NULL, TRUE, FALSE, NULL);	// WSAEventSelectで使うのでmanual resetとしておく
	m_evAcceptEndCollBack = CreateEvent(NULL, FALSE, FALSE, NULL);	// 使う側でシグナルとする。 (Acceptシグナル通知時しか使用しない)

	// クリティカルセクション
	InitializeCriticalSection(&m_cs);
}
//------------------------------------------
// デストラクタ
//------------------------------------------
ksControl::~ksControl()
{
	// 子スレッドを消す
	for (int ii = 0; ii < KS_MAXCLIENTS; ii++) {
		if (NULL != mcls_pInst[ii]) {
			delete mcls_pInst[ii];
			mcls_pInst[ii] = NULL;
		}
	} 
	// Listen用メインスレッドを消す
	if (NULL != m_hThread) {
		DWORD exitCode;
		GetExitCodeThread(m_hThread, &exitCode);
		if (STILL_ACTIVE == exitCode) {
			SetEvent(m_evStopEvent);
			DWORD dwRet = WaitForSingleObject(m_hThread, 5000);
			_ASSERT(dwRet == WAIT_OBJECT_0);
		}
		CloseHandle(m_hThread);
	}
	// 開放
	if( INVALID_SOCKET != m_sock ) closesocket(m_sock);	// サーバソケットの消去
	CloseHandle(m_evStopEvent);
	CloseHandle(m_evAccepting);
	CloseHandle(m_evAcceptEndCollBack);
	DeleteCriticalSection(&m_cs);
}

//------------------------------------------
// ソケットを開く
// DWORD rcvBufSz 受信バッファサイズ
// DWORD sndBufSz 送信バッファサイズ
//------------------------------------------
int ksControl::SockOpen(DWORD rcvBufSz, DWORD sndBufSz)
{
	WSADATA wsaData;
	WORD wVersionRequired = MAKEWORD(2, 2);		// WinSock 2.0 Windows NT

	if( WSAStartup(wVersionRequired, &wsaData) == 0 ){
		// WinSockﾊﾞｰｼﾞｮﾝﾁｪｯｸ
		if( wVersionRequired != wsaData.wVersion ){
			WSACleanup();
			return -1;
		}
	} else {
		return -1;
	}

	// ソケットの生成
	m_sock = WSASocket( AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED );
	if( m_sock == INVALID_SOCKET ){
		return -2;
	}
	// バッファサイズセット
	int opt = rcvBufSz;
	setsockopt( m_sock, SOL_SOCKET, SO_RCVBUF, (const char *)&opt, sizeof(opt) );
	opt = sndBufSz;
	setsockopt( m_sock, SOL_SOCKET, SO_SNDBUF, (const char *)&opt, sizeof(opt) );
// バッファに貯まったパケットを即、送信するときに以下のコメントを使用
//	opt = 1;
//	setsockopt( m_sock, IPPROTO_TCP, TCP_NODELAY, (const char *)&opt, sizeof(opt) );
	return 0;
}

//------------------------------------------
// 接続して子スレッドを作成する（メインスレッドはなし）
// ksThread **newInst 未割り当てのポインタに子スレッドのポインタ値をセットして返す
//------------------------------------------
int ksControl::Connect(ksThread **newInst)
{
	int ii;
	SOCKADDR_IN 	addr_in;

	// ｻｰﾊﾞのｱﾄﾞﾚｽｾｯﾄ
	addr_in.sin_family = AF_INET;
	addr_in.sin_port   = htons( m_nPort );
	addr_in.sin_addr.s_addr = inet_addr( m_Ip );

	// 接続
	// クライアントの場合は、WSAEventSelectは使わないのでブロッキングモードのまま
	if (connect( m_sock, (LPSOCKADDR)&addr_in, sizeof(SOCKADDR_IN))) {
		int a = GetLastError();
		return -4;
	}

	// 新しく子スレッドのインスタンスを作る
	for (ii = 0; ii < KS_MAXCLIENTS; ii++) {		// 空きの子スレッドを探す
		if (NULL == mcls_pInst[ii]) break;
	}
	if (KS_MAXCLIENTS == ii) return -1;	// 上限に達した
	mcls_pInst[ii] = new ksThread(m_sock, m_nTimeout, m_nTid, m_nMsgBase, 
								  m_evRecvTimeout, m_evSendTimeout, m_evDisConnect);

	// 子スレッド起動
	if (NULL == mcls_pInst[ii]) return -2;
	if (mcls_pInst[ii]->Start(m_nKsThPriority) ) {
		delete mcls_pInst[ii];
		mcls_pInst[ii] = NULL;
		return -3;
	}
	// 生成した子スレッド値をセット
	*newInst = mcls_pInst[ii];
	m_nInstCount++;
	return 0;			
}

//------------------------------------------
// 非同期ですぐに戻る。実際につながるとメッセージが送られる
// int backlog 保留中の接続のキューの最大長
//------------------------------------------
int ksControl::Listen(int backlog)
{
	SOCKADDR_IN 	addr_in;
	
	// サーバなのでポート番号を再利用できるようにする
	int opt = 1;
	setsockopt( m_sock, SOL_SOCKET, SO_REUSEADDR, (const char *)&opt, sizeof(opt) );

	// ｻｰﾊﾞのｱﾄﾞﾚｽｾｯﾄ
	addr_in.sin_family = AF_INET;
	addr_in.sin_port   = htons( m_nPort );
	addr_in.sin_addr.s_addr = htonl(INADDR_ANY);
	
	// ソケットに名前をつける
	if (bind( m_sock, (LPSOCKADDR)&addr_in, sizeof(SOCKADDR_IN))) return -1;

	// イベントが出るようにする。これ以降ノンブロッキングになることに注意
	if (SOCKET_ERROR == WSAEventSelect(m_sock, m_evAccepting, FD_ACCEPT)) return -3;

	// listen
	if (listen( m_sock, backlog )) return -2;

	// サーバスレッドスタート
	unsigned int tid;
	m_hThread = (HANDLE)_beginthreadex(NULL, 0, MainThread, (LPVOID)this, 0, &tid);
	if (NULL == m_hThread) return -1;
	return 0;
}

//------------------------------------------
// 切断して解放
// ksThread *pChild 切断した子スレッドのインスタンス
//------------------------------------------
int ksControl::CloseChild(ksThread *pChild)	
{
	int ii;
	// 指定された子スレッドを探す
	for (ii = 0; ii < KS_MAXCLIENTS; ii++) {
		if (pChild == mcls_pInst[ii]) break;
	}
	if (ii == KS_MAXCLIENTS) return -1;

	// 子スレッドを削除
	delete mcls_pInst[ii];	// ソケットの消去
	mcls_pInst[ii] = NULL;

	EnterCriticalSection(&m_cs);
	m_nInstCount--;
	LeaveCriticalSection(&m_cs);
	return 0;
}

//------------------------------------------
// 子スレッドが有るか
// UINT id 子スレッドのポインタ値
// 戻り値 TRUE:有り FALSE:無し
//------------------------------------------
int ksControl::IsValidChild(UINT id) const
{
	if (0 == id) return FALSE;
	for (int ii = 0; ii < KS_MAXCLIENTS; ii++) {
		if (id == (UINT)mcls_pInst[ii]) return TRUE;
	}
	return FALSE;
}
//------------------------------------------
// 今生きているスレッドの数を返す
//------------------------------------------
int ksControl::GetActive() const
{
	int num = 0;
	for (int ii = 0; ii < KS_MAXCLIENTS; ii++) {
		if (NULL != mcls_pInst[ii]) {
			if (STAT_ACTIVE(mcls_pInst[ii]->GetStat()))		num++;
		}
	}
	return num;
}

//------------------------------------------
// IP、ポート設定
// const char *ip IPアドレス (xxx.xxx.xxx.xxxの形式)
// short port ポート
//------------------------------------------
void ksControl::SetAddr(const char *ip, short port)
{
	_ASSERT(NULL != m_Ip && NULL != ip && 20>strlen(ip));
	strcpy(m_Ip, ip);
	m_Ip[sizeof(m_Ip)-1] = '\0';
	m_nPort = port;
}

//------------------------------------------
// タイムアウト再設定
// DWORD timeout タイムアウト時間
//------------------------------------------
void ksControl::SetTimeout(DWORD timeout)
{
	m_nTimeout = timeout;
	for (int ii = 0; ii < KS_MAXCLIENTS; ii++) {
		if (NULL != mcls_pInst[ii]) {
			if (STAT_ACTIVE(mcls_pInst[ii]->GetStat()) )	mcls_pInst[ii]->SetTimeout(timeout);
		}
	}
}
//------------------------------------------
// 受信タイムアウト イベント設定
// HANDLE evRecvTimeout 受信タイムアウトイベント用ハンドル
//------------------------------------------
void ksControl::SetEvRecvTimeout(HANDLE evRecvTimeout)
{
	m_evRecvTimeout = evRecvTimeout;
	for (int ii = 0; ii < KS_MAXCLIENTS; ii++) {
		if (NULL != mcls_pInst[ii]) { 
			if (STAT_ACTIVE(mcls_pInst[ii]->GetStat()) )	mcls_pInst[ii]->SetEvRecvTimeout(evRecvTimeout);
		}
	}
}
//------------------------------------------
// 送信タイムアウト イベント設定
// HANDLE evSendTimeout 送信タイムアウトイベント用ハンドル
//------------------------------------------
void ksControl::SetEvSendTimeout(HANDLE evSendTimeout)
{
	m_evSendTimeout = evSendTimeout;
	for (int ii = 0; ii < KS_MAXCLIENTS; ii++) {
		if (NULL != mcls_pInst[ii]) { 
			if (STAT_ACTIVE(mcls_pInst[ii]->GetStat()) )	mcls_pInst[ii]->SetEvSendTimeout(evSendTimeout);
		}
	}
}
//------------------------------------------
// 切断通知 イベント設定
// HANDLE evDisConnect 切断通知イベント用ハンドル
//------------------------------------------
void ksControl::SetEvDisConnect(HANDLE evDisConnect)
{
	m_evDisConnect = evDisConnect;
	for (int ii = 0; ii < KS_MAXCLIENTS; ii++) {
		if (NULL != mcls_pInst[ii]) { 
			if (STAT_ACTIVE(mcls_pInst[ii]->GetStat()) )	mcls_pInst[ii]->SetEvDisConnect(evDisConnect);
		}
	}
}
//------------------------------------------
// アクセプト通知 イベント設定
// HANDLE evAccept アクセプト通知イベント用ハンドル
//------------------------------------------
void ksControl::SetEvAccept(HANDLE evAccept)
{
	m_evAccept = evAccept;
}

//------------------------------------------
// メッセージ・イベント 通知
// UINT msg 処理区分 (WM_KS_なんちゃら)
// WPARAM wParam (メッセージ通知時のみ)
// LPARAM lParam (メッセージ通知時のみ)
//------------------------------------------
void ksControl::sendMsg(UINT msg, WPARAM wParam, LPARAM lParam)
{
	if( msg == WM_KS_ACCEPT && m_evAccept != NULL ) {						// アクセプト要求 (シグナル通知)
		// Accept メッセージを生成
		memset(&m_LastAcceptMsg, 0x00, sizeof(m_LastAcceptMsg));
		m_LastAcceptMsg.message = m_nMsgBase+msg;
		m_LastAcceptMsg.wParam = wParam;
		m_LastAcceptMsg.lParam = lParam;

		// 外部に通知
		SetEvent(m_evAccept);

		// 通知先でAcceptメッセージを取得するまでブロック！！
		DWORD ret = WaitForSingleObject(m_evAcceptEndCollBack, 3000);	
		if (WAIT_TIMEOUT == ret) {
			// コーディングがおかしい。
			_ASSERT(0);
		}
		

	} else if (0 != m_nTid) {
		// WM_KS_ACCEPT,		子スレッドインスタンス値,		接続元のIPアドレス
		PostThreadMessage(m_nTid, m_nMsgBase+msg, wParam, lParam);
	}
}

//------------------------------------------
// アクセプトシグナル通知時にMSG情報を取得 (アクセプトシグナル通知時のみ使用)
// MSG* msg アクセプトシグナル通知された時のアクセプト情報
//
// 注意：アクセプトシグナル通知後、本メソッドがコールされるまで、本メインスレッドはブロックされる。
//------------------------------------------
void ksControl::GetAcceptEventMsg(MSG* msg)
{
	// データを渡す
	memcpy(msg, &m_LastAcceptMsg, sizeof(m_LastAcceptMsg));

	// メインスレッド実行開始通知
	SetEvent( m_evAcceptEndCollBack); 
}


//
// スレッド本体
//

//------------------------------------------
// Listen用メインスレッド
//------------------------------------------
//static UINT __stdcall ksControlThread(LPVOID arg)
unsigned int __stdcall ksControl::MainThread(LPVOID arg)
{
	int ii;

	// メインスレッド準備完了
	ksControl * p = (ksControl *)arg;

	// イベント作成
	HANDLE evArray[2] = {p->m_evStopEvent, p->m_evAccepting};
	enum { EV_STOP = WAIT_OBJECT_0,			// 終了通知
		   EV_ACCEPT,						// アクセプト要求
		   EV_END};

	while (true) {
		DWORD dwWait = WaitForMultipleObjects(EV_END, evArray, FALSE, INFINITE);

		// 終了要求を最優先
		if (EV_STOP == dwWait) {	// 終了
			return 0;
		}

		// ACCEPTによる新規インスタンス追加
		if (EV_ACCEPT == dwWait) {	
			ResetEvent(p->m_evAccepting);			// シグナルを手動で落とす

			// acceptをする
			int			iaddrlen;
			SOCKADDR_IN	addr;			// 接続元の情報
			DWORD		ulongIP;		// 接続元のIP
			SOCKET		newSock;		// ACCEPTしたソケットハンドル
			
			iaddrlen = sizeof(addr);
			newSock = accept( p->m_sock, (LPSOCKADDR)&addr, &iaddrlen );

			// 接続要求元（クライアント）のＩＰアドレス取り出し
			memcpy((char *)&ulongIP, &addr.sin_addr.S_un.S_un_b.s_b1, 4);
			
			// 子スレッドの作成と起動
			for (ii = 0; ii < KS_MAXCLIENTS; ii++) {		// 空き子スレッドを探す
				if (NULL == p->mcls_pInst[ii]) break;
			}
			if (KS_MAXCLIENTS == ii) {
				// 一杯なので切断
				shutdown(newSock, SD_SEND);
				closesocket(newSock);
				continue;
			}
			// 子スレッドの生成
			p->mcls_pInst[ii] = new ksThread(newSock, p->m_nTimeout, p->m_nTid, p->m_nMsgBase, 
										     p->m_evRecvTimeout, p->m_evSendTimeout, p->m_evDisConnect);
			
			// 子スレッド起動
			_ASSERT(p->mcls_pInst[ii]);
			p->mcls_pInst[ii]->Start(p->m_nKsThPriority);
			// カウンタ
			EnterCriticalSection(&p->m_cs);
			p->m_nInstCount++;
			LeaveCriticalSection(&p->m_cs);
//char tmsg[256];
//wsprintf(tmsg, "★[%d] new %d \n",ii,(WPARAM)p->mcls_pInst[ii]);
//_RPT0(_CRT_WARN, tmsg);
			// 通知
			p->sendMsg(WM_KS_ACCEPT, (WPARAM)p->mcls_pInst[ii], ulongIP);
			continue;
		}
		_ASSERT(0);
	}
}
