// ksThread.cpp: ksThread クラスのインプリメンテーション
//
//////////////////////////////////////////////////////////////////////


#include "stdafx.h"
#include <process.h>
#include <crtdbg.h>

#include <winsock2.h>
#include "ksThread.h"

using namespace KizuSock3;

//////////////////////////////////////////////////////////////////////
// 構築/消滅
//////////////////////////////////////////////////////////////////////
//------------------------------------------
// コンストラクタ
// SOCKET sock 通信するソケットハンドル
// DWORD timeout タイムアウト間隔 [ms]
// DWORD tid PostThreadMessageの送り先 (ThreadIDを渡せば良い)
// DWORD msgbase メッセージベース番号
// HANDLE evRecvTimeout 受信タイムアウトハンドル
// HANDLE evSendTimeout 受信タイムアウトハンドル
// HANDLE evDisconnect 切断通知ハンドル
//------------------------------------------
ksThread::ksThread(SOCKET sock, DWORD timeout, DWORD tid, DWORD msgbase,
				   HANDLE evRecvTimeout, HANDLE evSendTimeout, HANDLE evDisconnect) :
m_sock((HANDLE)sock),
m_nTimeout(timeout),
m_nTid(tid),
m_nMsgBase(msgbase),
m_evRecvTimeout(evRecvTimeout),
m_evSendTimeout(evSendTimeout),
m_evDisConnect(evDisconnect),
m_hThread(NULL),
m_stat(0),
m_apierr(0),
m_nCloseKind(KIZUSOCK_CLOSEKIND_NON)
{

	// シグナル生成
	m_evStopEvent	= CreateEvent(NULL, FALSE, FALSE, NULL);
	m_evSetTimeout	= CreateEvent(NULL, FALSE, FALSE, NULL);
	m_evCancelEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	m_evRecvStart	= CreateEvent(NULL, FALSE, FALSE, NULL);
	m_evSendStart	= CreateEvent(NULL, FALSE, FALSE, NULL);
	m_evRecvAck		= CreateEvent(NULL, FALSE, FALSE, NULL);
	m_evSendAck		= CreateEvent(NULL, FALSE, FALSE, NULL);
}
//------------------------------------------
// デストラクタ
//------------------------------------------
ksThread::~ksThread()
{
	Stop();
	closesocket((SOCKET)m_sock);
	// シグナル開放
	CloseHandle(m_evStopEvent);
	CloseHandle(m_evSetTimeout);
	CloseHandle(m_evCancelEvent);
	CloseHandle(m_evRecvStart);
	CloseHandle(m_evSendStart);
	CloseHandle(m_evRecvAck);
	CloseHandle(m_evSendAck);
}

//------------------------------------------
// スレッド開始
// int nPriority 優先度
//------------------------------------------
int ksThread::Start(int nPriority)
{
	unsigned int tid;
	m_hThread = (HANDLE)_beginthreadex(NULL, 0, MainThread, (LPVOID)this, 0, &tid);
	if (0 == m_hThread) {
		m_stat = KIZUSOCK_STAT_THREAD_ERR;
		return -1;
	}

	// スレッド優先度指定
	if(THREAD_PRIORITY_NORMAL != nPriority) {
		SetThreadPriority(m_hThread, nPriority);
	}

	m_stat = KIZUSOCK_STAT_ACTIVE;
	return 0;
}

//------------------------------------------
// スレッド停止
//------------------------------------------
void ksThread::Stop()
{
	if (NULL != m_hThread) {
		DWORD exitCode;
		GetExitCodeThread(m_hThread, &exitCode);
		if (STILL_ACTIVE == exitCode) {
			SetEvent(m_evStopEvent);
			DWORD dwRet = WaitForSingleObject(m_hThread, 5000);
			_ASSERT(dwRet == WAIT_OBJECT_0);
			if (dwRet == WAIT_TIMEOUT) {
				TerminateThread(m_hThread, 0);
			}
		}
		CloseHandle(m_hThread);
		m_hThread = NULL;
	}
}


//------------------------------------------
// 受信開始処理 (メインスレッドの受信開始処理が完了するまでブロック)
// LPVOID pData 受信領域
// DWORD len 受信するサイズ
// HANDLE evEvent 受信完了時に通知するシグナル
//------------------------------------------
int ksThread::Recv(LPVOID pData, DWORD len)
{
	return Recv(pData, len, NULL);
}
int ksThread::Recv(LPVOID pData, DWORD len, HANDLE evEvent)
{
	if (STAT_WAIT_RECV(m_stat)) return -1;					// このインスタンスは前のRECVを実行中
	if (!STAT_ACTIVE(m_stat)) return -2;					// 子スレッドがとまっている
	// セット
	m_pRecvData = (char*)pData;
	m_nRecvSz = len;
	m_evRecvEnd = evEvent;									// 受信完了時に通知するシグナル (NULL時はメッセージ通知となる)
	// 受信開始依頼
//	_RPT1(_CRT_WARN, "OVL Recv event %p\n", this);	
	SetEvent(m_evRecvStart);								// メインスレッドに受信開始を通知
	DWORD ret = WaitForSingleObject(m_evRecvAck, 3000);		// メインスレッドが受信開始処理完了するまでブロック
	if (WAIT_TIMEOUT == ret) return -3;
	return 0;
}

//------------------------------------------
// 送信開始処理 (メインスレッドの送信開始処理が完了するまでブロック)
// LPVOID pData 送信領域
// DWORD len 送信するサイズ
// HANDLE evEvent 送信完了時に通知するシグナル
//------------------------------------------
int ksThread::Send(LPCVOID pData, DWORD len)
{
	return Send(pData, len, NULL);
}
int ksThread::Send(LPCVOID pData, DWORD len, HANDLE evEvent)
{
	if (STAT_WAIT_SEND(m_stat)) return -1;	// このインスタンスは前のSENDを実行中
	if (!STAT_ACTIVE(m_stat)) return -2; // 子スレッドがとまっている
	// セット
	m_pSendData = (char const*)pData;
	m_nSendSz = len;
	m_evSendEnd = evEvent;									// 送信完了時に通知するシグナル (NULL時はメッセージ通知となる)
	// 送信開始依頼
	SetEvent(m_evSendStart);								// メインスレッドに送信開始を通知
	DWORD ret = WaitForSingleObject(m_evSendAck, 3000);		// メインスレッドが送信開始処理完了するまでブロック
	if (WAIT_TIMEOUT == ret) return -3;
	return 0;
}

//------------------------------------------
// 送受信待ちを取りやめ
//------------------------------------------
void ksThread::Cancel()
{
	SetEvent(m_evCancelEvent);
	// スレッド異常終了している場合は すぐに終わる (送信中、受信中に クローズ通知が来た場合、下で無限ループとなる為の処置)
	if(STAT_THREAD_ERR(m_stat)) return;
	// 送受信とも落ちるまで待つ
	while (STAT_WAIT_SEND(m_stat) || STAT_WAIT_RECV(m_stat)) {
		Sleep(50);
	}
}
//------------------------------------------
// タイムアウト再設定
// DWORD timeout タイムアウト間隔 [ms]
//------------------------------------------
void ksThread::SetTimeout(DWORD timeout)
{
	m_nTimeout = timeout;
	SetEvent(m_evSetTimeout);			// メインスレッドに通知して、一度実行させて、新規時間でまた待ちにする。
}

//------------------------------------------
// メッセージ・イベント 通知
// UINT msg 処理区分 (WM_KS_なんちゃら)
// WPARAM wParam (メッセージ通知時のみ)
// LPARAM lParam (メッセージ通知時のみ)
//------------------------------------------
void ksThread::sendMsg(UINT msg, WPARAM wParam, LPARAM lParam)
{
	if( msg == WM_KS_RECV && m_evRecvEnd != NULL ) {						// 受信完了 (シグナル通知)
		SetEvent(m_evRecvEnd);


	} else if( msg == WM_KS_SEND && m_evSendEnd != NULL ) {					// 送信完了 (シグナル通知)
		SetEvent(m_evSendEnd);

	} else if( msg == WM_KS_RECV_TIMEOUT && m_evRecvTimeout != NULL ) {		// 受信タイムアウト (シグナル通知)
		SetEvent(m_evRecvTimeout);

	} else if( msg == WM_KS_SEND_TIMEOUT && m_evSendTimeout != NULL ) {		// 送信タイムアウト (シグナル通知)
		SetEvent(m_evSendTimeout);

	} else if( msg == WM_KS_DISCONNECT && m_evDisConnect != NULL ) {		// 切断 (シグナル通知)
		SetEvent(m_evDisConnect);

	} else if (0 != m_nTid) {												// メッセージ通知
		// WM_KS_RECV,			自インスタンス値,		0固定
		// WM_KS_RECV_TIMEOUT,	自インスタンス値,		0固定
		// WM_KS_SEND_TIMEOUT,	自インスタンス値,		0固定
		// WM_KS_DISCONNECT,	自インスタンス値,		(0:相手から切断 それ以外:自分で強制切断)
		PostThreadMessage(m_nTid, m_nMsgBase+msg, wParam, lParam);			
	}
}

//
// スレッド本体
//

//------------------------------------------
// メインスレッド
//------------------------------------------
unsigned int __stdcall ksThread::MainThread(LPVOID arg)
{
	// メインスレッド準備完了
	ksThread * p = (ksThread *)arg;
	SET_ACTIVE(p->m_stat);

	// 同期制御用イベント
	OVERLAPPED ovlSend, ovlRecv;
	memset(&ovlSend, 0, sizeof(OVERLAPPED));
	memset(&ovlRecv, 0, sizeof(OVERLAPPED));
	ovlSend.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	ovlRecv.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	HANDLE evCloseEvent = CreateEvent(NULL, TRUE, FALSE, NULL);		// ソケットのクローズシグナル
	WSAEventSelect((SOCKET)p->m_sock, evCloseEvent, FD_CLOSE);		// CLOSEを通知可能にする
	p->m_nCloseKind = KIZUSOCK_CLOSEKIND_NON;

	DWORD recvSize, sendSize, recvTotal, sendTotal, restRecv, restSend;
	DWORD recvStartTick, sendStartTick;
	
	// イベント作成
	HANDLE evArray[8] = {p->m_evStopEvent, p->m_evSetTimeout, p->m_evCancelEvent,
						 p->m_evRecvStart, p->m_evSendStart,
						 ovlRecv.hEvent, ovlSend.hEvent, evCloseEvent};
	enum { EV_STOP = WAIT_OBJECT_0,			// 終了通知
		   EV_SETTIMEOUT,					// タイムアウト時間再設定
		   EV_CANCEL,						// 送受信キャンセル
		   EV_RECV_START,					// 受信開始依頼
		   EV_SEND_START,					// 送信開始依頼
		   EV_OVL_RECV,						// 非同期受信完了
		   EV_OVL_SEND,						// 非同期送信完了
		   EV_CLOSE,						// 切断通知
		   EV_END};

	while (true) {
		DWORD dwWait = WaitForMultipleObjects(EV_END, evArray, FALSE, p->m_nTimeout);
		DWORD evNum = dwWait - WAIT_OBJECT_0;

#ifdef _DEBUG
		char msg[256];
		if (WAIT_TIMEOUT == dwWait) wsprintf(msg, "event timeout\n");
		if (EV_STOP == evNum) wsprintf(msg, "event stop\n");
		if (EV_SETTIMEOUT == evNum) wsprintf(msg, "event settimeout\n");
		if (EV_CANCEL == evNum) wsprintf(msg, "event cancel\n");
		if (EV_RECV_START == evNum) wsprintf(msg, "event recv\n");
		if (EV_SEND_START == evNum) wsprintf(msg, "event send\n");
		if (EV_OVL_RECV == evNum) wsprintf(msg, "event ovl recv\n");
		if (EV_OVL_SEND == evNum) wsprintf(msg, "event ovl send\n");
		if (EV_CLOSE == evNum) wsprintf(msg, "event close\n");
		//		_RPT0(_CRT_WARN, msg);
#endif

		// 停止は最優先事項
		if (evNum == EV_STOP) {	
			CancelIo(p->m_sock);
			RST_WAIT_RECV(p->m_stat);
			RST_WAIT_SEND(p->m_stat);
			shutdown((SOCKET)p->m_sock, SD_BOTH);
			p->m_nCloseKind = KIZUSOCK_CLOSEKIND_THEAD;
			CloseHandle(ovlSend.hEvent);
			CloseHandle(ovlRecv.hEvent);
			CloseHandle(evCloseEvent);
			RST_ACTIVE(p->m_stat);				
			return THR_NORMAL_END;					// 正常停止
		}

		// 受信タイムアウトチェック
		if (STAT_WAIT_RECV(p->m_stat) && WAIT_TIMEOUT == dwWait) {	// 読み待ちのとき
			if (GetTickCount() < 0x80000000 && recvStartTick > 0x80000000) { // ラップアラウンドではもう一度待つ
				recvStartTick = GetTickCount();
			} else if ((GetTickCount() - recvStartTick) >= p->m_nTimeout) {
				SET_RECV_TIMEOUT(p->m_stat);
				p->sendMsg(WM_KS_RECV_TIMEOUT, (WPARAM)p, 0);	// 自インスタンスを返す
			}
		}
		// 送信タイムアウトチェック
		if (STAT_WAIT_SEND(p->m_stat) && WAIT_TIMEOUT == dwWait) {	// 書き待ちのとき
			if (GetTickCount() < 0x80000000 && sendStartTick > 0x80000000) { // ラップアラウンドではもう一度待つ
				sendStartTick = GetTickCount();
			} else if ((GetTickCount() - sendStartTick) >= p->m_nTimeout) {
				SET_SEND_TIMEOUT(p->m_stat);
				p->sendMsg(WM_KS_SEND_TIMEOUT, (WPARAM)p, 0);	// 自インスタンスを返す
			}
		}

		// タイムアウト 又は、 タイムアウト時間再設定 の場合は、ここで終わり
		if (WAIT_TIMEOUT == dwWait || evNum == EV_SETTIMEOUT) continue;


		switch (evNum) {
		case EV_CANCEL:							// CancelIo
			CancelIo(p->m_sock);
			RST_WAIT_RECV(p->m_stat);
			RST_WAIT_SEND(p->m_stat);
//_RPT1(_CRT_WARN, "Cancel %p\n", p);
			break;
			
		case EV_RECV_START:						// 非同期読み取りを開始する
			SET_WAIT_RECV(p->m_stat);
			if (!ReadFile(p->m_sock, p->m_pRecvData, p->m_nRecvSz, NULL, &ovlRecv)) {
				p->m_apierr = GetLastError();			// 即失敗の場合
				if (p->m_apierr != ERROR_IO_PENDING) goto read_abort;
			}
			recvStartTick = GetTickCount();				// 処理開始時間セット (タイムアウトチェックに使用)
			recvTotal = 0;
			SetEvent(p->m_evRecvAck);					// 受信開始処理完了
//_RPT2(_CRT_WARN, "RecvStart %p size=%d\n", p, p->m_nRecvSz);
			break;
			
		case EV_SEND_START:						// 非同期送信を開始する
			SET_WAIT_SEND(p->m_stat);
			if (!WriteFile(p->m_sock, p->m_pSendData, p->m_nSendSz, NULL, &ovlSend)) {
				p->m_apierr = GetLastError();			// 即失敗の場合
				if (p->m_apierr != ERROR_IO_PENDING) goto send_abort;
			}
			sendStartTick = GetTickCount();				// 処理開始時間セット (タイムアウトチェックに使用)
			sendTotal = 0;
			SetEvent(p->m_evSendAck);					// 送信開始処理完了
//_RPT2(_CRT_WARN, "SendStart %p size=%d\n", p, p->m_nSendSz);
			break;
			
		case EV_OVL_RECV:						// 非同期読み取りの完了
			if (GetOverlappedResult(p->m_sock, &ovlRecv, &recvSize, TRUE)) {	// なぜかTRUEでないといけない
				if (0 == recvSize) { // ソケットが閉じた場合
//_RPT0(_CRT_WARN, "socket closed gracefully\n");
					p->m_apierr = 0;
					goto read_abort;
				}
				recvStartTick = GetTickCount();			// 一回取得できたので再度新しい時間をセット (タイムアウトチェックに使用)
				// 正常にリード
				recvTotal += recvSize;
				if (recvTotal == p->m_nRecvSz) {
//_RPT1(_CRT_WARN, "RecvEnd %d\n", recvTotal);
					// データ読み込み完了
					ResetEvent(ovlRecv.hEvent);
					RST_RECV_TIMEOUT(p->m_stat);
					RST_WAIT_RECV(p->m_stat);	// ステータス降ろす

					// 通知
					p->sendMsg(WM_KS_RECV, (WPARAM)p, 0);

					break;
				}
				// 次のリード発行 (一回で取り込めない場合。指定サイズ取り込むまで繰り返す)
				restRecv = p->m_nRecvSz - recvTotal;
//_RPT2(_CRT_WARN, "Recving･･･ now=%d(zan=%d)\n", recvTotal, restRecv);
				if (!ReadFile(p->m_sock, p->m_pRecvData+recvTotal, restRecv, NULL, &ovlRecv)) {
					p->m_apierr = GetLastError();	// 即失敗の場合
					if (p->m_apierr != ERROR_IO_PENDING) goto read_abort;
				}
				break;

			} else {									// リードエラー発生
				p->m_apierr = GetLastError();
read_abort :
				if (ERROR_OPERATION_ABORTED == p->m_apierr) {
					ResetEvent(ovlRecv.hEvent);
					_RPT0(_CRT_WARN, "read abort\n");
					p->m_nCloseKind = KIZUSOCK_CLOSEKIND_CANCEL;
					break;			// CancelIoを発行したとき
				}
				_RPT0(_CRT_WARN, "socket read error\n");
#ifdef _DEBUG
				LPVOID lpMsgBuf;
				FormatMessage( FORMAT_MESSAGE_ALLOCATE_BUFFER |
					FORMAT_MESSAGE_FROM_SYSTEM |
					FORMAT_MESSAGE_IGNORE_INSERTS,
					NULL,
					p->m_apierr,
					MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // 既定の言語
					(LPTSTR) &lpMsgBuf,	0, NULL);
				_RPT0(_CRT_WARN, (const char *)lpMsgBuf);
				LocalFree(lpMsgBuf);
#endif
				CancelIo(p->m_sock);
				RST_WAIT_RECV(p->m_stat);
				RST_WAIT_SEND(p->m_stat);
				shutdown((SOCKET)p->m_sock, SD_BOTH);
				p->m_nCloseKind = KIZUSOCK_CLOSEKIND_SOCK;	// 受信待ちは相手からの切断
				CloseHandle(ovlSend.hEvent);
				CloseHandle(ovlRecv.hEvent);
				CloseHandle(evCloseEvent);
				SET_THREAD_ERR(p->m_stat);
				RST_ACTIVE(p->m_stat);
				p->sendMsg(WM_KS_DISCONNECT, (WPARAM)p, p->m_apierr);			// 異常切断通知
				return THR_RECV_ERR;											// メインスレッド 受信異常により終了
			}
			
		case EV_OVL_SEND:						// 非同期書き込みの完了
//				_RPT0(_CRT_WARN, "event ovl send\n");
			if (GetOverlappedResult(p->m_sock, &ovlSend, &sendSize, TRUE)) {
				// 正常に送信
				sendStartTick = GetTickCount();			// 一回取得できたので再度新しい時間をセット (タイムアウトチェックに使用)
				sendTotal += sendSize;
				if (sendTotal == p->m_nSendSz) {
//_RPT1(_CRT_WARN, "SendEnd %d\n", sendTotal);
					// データ書き込み完了
					ResetEvent(ovlSend.hEvent);
					RST_SEND_TIMEOUT(p->m_stat);
					RST_WAIT_SEND(p->m_stat); // ステータス降ろす
					// 通知
					p->sendMsg(WM_KS_SEND, (WPARAM)p, 0);
					break;
				}
				// 次の送信発行 (一回で送れない場合。指定サイズ送れるまで繰り返す)
				restSend = p->m_nSendSz	- sendTotal;
//_RPT2(_CRT_WARN, "Sending･･･ now=%d(zan=%d)\n", sendTotal, restSend);
				if (!WriteFile(p->m_sock, p->m_pSendData+sendTotal, restSend, NULL, &ovlSend)) {
					p->m_apierr = GetLastError();	// 即失敗の場合
					if (p->m_apierr != ERROR_IO_PENDING) goto send_abort;
				}
				break;
			} else {  // 送信エラー発生
				p->m_apierr = GetLastError();
send_abort:
				if (ERROR_OPERATION_ABORTED == p->m_apierr) {
					ResetEvent(ovlSend.hEvent);
					_RPT0(_CRT_WARN, "send abort\n");
					p->m_nCloseKind = KIZUSOCK_CLOSEKIND_CANCEL;
					break; // CancelIoを発行したとき
				}
				_RPT0(_CRT_WARN, "socket send error\n");
#ifdef _DEBUG
				LPVOID lpMsgBuf;
				FormatMessage( FORMAT_MESSAGE_ALLOCATE_BUFFER |
					FORMAT_MESSAGE_FROM_SYSTEM |
					FORMAT_MESSAGE_IGNORE_INSERTS,
					NULL,
					p->m_apierr,
					MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // 既定の言語
					(LPTSTR) &lpMsgBuf,	0, NULL);
				_RPT0(_CRT_WARN, (const char *)lpMsgBuf);
				LocalFree(lpMsgBuf);
#endif
				CancelIo(p->m_sock);
				RST_WAIT_RECV(p->m_stat);
				RST_WAIT_SEND(p->m_stat);
				shutdown((SOCKET)p->m_sock, SD_BOTH);
				p->m_nCloseKind = KIZUSOCK_CLOSEKIND_PROG;
				CloseHandle(ovlSend.hEvent);
				CloseHandle(ovlRecv.hEvent);
				CloseHandle(evCloseEvent);
				SET_THREAD_ERR(p->m_stat);
				RST_ACTIVE(p->m_stat);
				p->sendMsg(WM_KS_DISCONNECT, (WPARAM)p, p->m_apierr);			// 異常切断通知
				return THR_SEND_ERR;											// メインスレッド 送信異常により終了
			}

		case EV_CLOSE :							// 相手より閉じられた場合
//			_RPT0(_CRT_WARN, "socket close\n");
			CancelIo(p->m_sock);
			RST_WAIT_RECV(p->m_stat);
			RST_WAIT_SEND(p->m_stat);
			shutdown((SOCKET)p->m_sock, SD_BOTH);
			p->m_nCloseKind = KIZUSOCK_CLOSEKIND_SOCK;
			CloseHandle(ovlSend.hEvent);
			CloseHandle(ovlRecv.hEvent);
			CloseHandle(evCloseEvent);
			SET_THREAD_ERR(p->m_stat);
			RST_ACTIVE(p->m_stat);
			p->sendMsg(WM_KS_DISCONNECT, (WPARAM)p, 0);							// 正常切断通知

			return THR_CLOSE_END;												// メインスレッド 相手からの切断により終了

		default:	// ありえない
			_ASSERT(0);
		}
	}
}
