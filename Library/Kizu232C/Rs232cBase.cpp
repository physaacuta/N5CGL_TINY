#include "StdAfx.h"
#include "Rs232cBase.h"
#include <crtdbg.h>

#include <process.h>
#include <stdio.h>


using namespace Kizu232C;

//------------------------------------------
// コンストラクタ
// const char *cSession iniファイル セッション
// const char *cIniPath iniファイル パス
// DWORD tid PostThreadMessageの送り先 (ThreadIDを渡せば良い)
// DWORD r_size 232Cの受信バッファサイズ
// DWORD s_size 232Cの送信バッファサイズ
// DWORD msgbase メッセージベース番号
// bool bSoftFolwOff ソフトフロー制御を無効時true = 制御線が接続されていないRS-232Cケーブルを使用時に有効とする
//------------------------------------------
Rs232cBase::Rs232cBase(const char *cSession, const char *cIniPath, DWORD tid,
					   DWORD r_size, DWORD s_size, DWORD msgbase, bool bSoftFolwOff) :
m_nTid(tid),
m_nMsgBase(msgbase),
m_hCom(INVALID_HANDLE_VALUE),
m_hThread(NULL),
m_nTimeout(INFINITE),
m_evStopEvent(NULL),
m_evSetTimeout(NULL),
m_evCancelEvent(NULL),
m_evRecvStart(NULL),
m_evSendStart(NULL),
m_evRecvAck(NULL),
m_evSendAck(NULL),
m_evRecvEnd(NULL),
m_evSendEnd(NULL),
m_cRecvCR(NULL),
m_stat(0),
m_apierr(0)
{
	DCB	dcb;					// DCB構造体
								// 通信パラメータ
	char cComName[12], cBaud[12], cParity[12], cData[12], cStop[12], cTo[12], cDtr[12], cRts[12], cIdsr[12], cOcts[12], cParam[128];
	COMMTIMEOUTS CommTimeOuts;	// COMMTIMEOUTS構造体
	
	// COMポート取得
	GetPrivateProfileString(cSession, "com", "",	cComName, sizeof(cComName), cIniPath);
	if (strlen(cComName) == 0) {
		throw -8;				// COMポート指定がない
	}
	// 通信ポートをオープンする
	m_hCom = CreateFile(cComName, GENERIC_READ|GENERIC_WRITE , NULL,
						NULL, OPEN_EXISTING,
						FILE_ATTRIBUTE_NORMAL | FILE_FLAG_OVERLAPPED,
						NULL);
	if(INVALID_HANDLE_VALUE == m_hCom) throw -1;

	// バッファサイズを指定 
	if( !SetupComm(m_hCom, r_size, s_size) ) {
		CloseHandle(m_hCom);
		m_hCom = INVALID_HANDLE_VALUE;
		throw -2;
	}

	// 通信パラメータを設定する
	GetCommState(m_hCom, &dcb);		// 現在の制御設定をデバイス制御ブロック（DCB 構造体）に格納
	
	GetPrivateProfileString(cSession, "baud",	"", cBaud,	 sizeof(cBaud),	  cIniPath);		// ボーレート取得
	GetPrivateProfileString(cSession, "parity", "", cParity, sizeof(cParity), cIniPath);		// パリティ設定取得
	GetPrivateProfileString(cSession, "data",	"", cData,	 sizeof(cData),   cIniPath);		// データビット取得
	GetPrivateProfileString(cSession, "stop",	"", cStop,   sizeof(cStop),   cIniPath);		// ストップビット取得
	GetPrivateProfileString(cSession, "to",		"", cTo,	 sizeof(cTo),	  cIniPath);		// 無限タイムアウト処理取得
	GetPrivateProfileString(cSession, "dtr",	"", cDtr,	 sizeof(cDtr),	  cIniPath);		// データ ターミナル レディ (DTR) 取得
	GetPrivateProfileString(cSession, "rts",	"", cRts,	 sizeof(cRts),	  cIniPath);		// 送信要求 (RTS) 取得
	GetPrivateProfileString(cSession, "idsr",	"", cIdsr,	 sizeof(cIdsr),   cIniPath);		// DSR 回線取得
	GetPrivateProfileString(cSession, "octs",	"", cOcts,	 sizeof(cOcts),   cIniPath);		// 送信可 (CTS) 回線取得

	if( strlen(cBaud) == 0 || strlen(cParity) == 0 ||  strlen(cData) == 0 || strlen(cStop) == 0
		|| strlen(cTo) == 0 || strlen(cDtr) == 0 ||  strlen(cRts) == 0 || strlen(cIdsr) == 0 || strlen(cOcts) == 0) {
		CloseHandle(m_hCom);
		m_hCom = INVALID_HANDLE_VALUE;
		throw -7;	// INIファイルが設定されていない
	}

	// 設定文字列作成
	sprintf(cParam, "baud=%s parity=%s data=%s stop=%s to=%s octs=%s dtr=%s rts=%s idsr=%s",
		cBaud, cParity, cData, cStop, cTo, cOcts, cDtr, cRts, cIdsr);
									
	if( !BuildCommDCBAndTimeouts(cParam, &dcb, &CommTimeOuts) ) {	// デバイス制御ブロックに格納
		CloseHandle(m_hCom);
		m_hCom = INVALID_HANDLE_VALUE;
		throw -3;	
	}
	CommTimeOuts.ReadIntervalTimeout = 0;
	CommTimeOuts.ReadTotalTimeoutMultiplier = 0;
	CommTimeOuts.ReadTotalTimeoutConstant = 0;
	CommTimeOuts.WriteTotalTimeoutConstant = 0;
	CommTimeOuts.WriteTotalTimeoutMultiplier = 0;

	// タイムアウトパラメータを設定
	if( !SetCommTimeouts(m_hCom, &CommTimeOuts) ) {	
		CloseHandle(m_hCom);
		m_hCom = INVALID_HANDLE_VALUE;
		throw -4;	
	}	

	// ソフトフロー無視対応 (カメラコントローラー 制御線未配線用対策版)
	if(bSoftFolwOff) {
		dcb.fOutxCtsFlow = 0;
		dcb.fOutxDsrFlow  = 0;
		dcb.fInX = 0;
		dcb.fOutX = 0;
	} else {
		// デフォルトでは、ソフトフロー制御はすべて有効状態の為、何もしない
	}

	// デバイス制御ブロック（DCB 構造体）の指定に従って通信デバイスを構成
	dcb.fAbortOnError = 0;
	if( !SetCommState(m_hCom, &dcb) ) {	
		CloseHandle(m_hCom);
		m_hCom = INVALID_HANDLE_VALUE;
		throw -5;	
	}

	// バッファをクリア
	PurgeComm(m_hCom, PURGE_TXABORT | PURGE_RXABORT | PURGE_TXCLEAR | PURGE_RXCLEAR);

	// スレッドイベント生成
	m_evStopEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	m_evSetTimeout = CreateEvent(NULL, FALSE, FALSE, NULL);
	m_evCancelEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	m_evRecvStart = CreateEvent(NULL, FALSE, FALSE, NULL);
	m_evSendStart = CreateEvent(NULL, FALSE, FALSE, NULL);
	m_evRecvAck = CreateEvent(NULL, FALSE, FALSE, NULL);
	m_evSendAck = CreateEvent(NULL, FALSE, FALSE, NULL);

	// メインスレッドを開始
	UINT uiTid;					// スレッドID
	m_hThread = (HANDLE)_beginthreadex(NULL, 0, MainThread, (LPVOID)this, 0, &uiTid);
	if (NULL == m_hThread) {
		CloseHandle(m_hCom);
		m_hCom = INVALID_HANDLE_VALUE;
		throw -6;
	}

	// ステータスイニシャル
	m_stat = KIZU232C_STAT_ACTIVE;
}

//------------------------------------------
// デストラクタ
//------------------------------------------
Rs232cBase::~Rs232cBase(void)
{
	// スレッドを終了
	if (NULL != m_hThread) {
		DWORD exitCode;
		GetExitCodeThread(m_hThread, &exitCode);
		if (STILL_ACTIVE == exitCode) {
			SetEvent(m_evStopEvent);
			DWORD dwRet = WaitForSingleObject(m_hThread, 5000);
			//_ASSERT(dwRet == WAIT_OBJECT_0);
			if (dwRet == WAIT_TIMEOUT) {
				TerminateThread(m_hThread, 0);
			}
		}
		CloseHandle(m_hThread);
		m_hThread = NULL;
	}
	// スレッドイベント解放
	CloseHandle(m_evStopEvent);
	CloseHandle(m_evSetTimeout);
	CloseHandle(m_evCancelEvent);
	CloseHandle(m_evRecvStart);
	CloseHandle(m_evSendStart);
	CloseHandle(m_evRecvAck);
	CloseHandle(m_evSendAck);

	// 通信ポートをクローズする
	if (m_hCom != INVALID_HANDLE_VALUE) {
		CloseHandle(m_hCom);
		m_hCom = INVALID_HANDLE_VALUE;
	}
}

//------------------------------------------
// 通信エラーの情報確認
// 戻り値 入力バッファに格納されているデータのバイト数 (ようは 0なら正常)
//------------------------------------------
DWORD Rs232cBase::GetQueueLen() const
{
	COMSTAT lpCstat;	// COMSTAT構造体
	DWORD dwErrflag;	// エラーコードを受け取る変数

	// 通信エラーの情報を取得
	ClearCommError(m_hCom, &dwErrflag, &lpCstat);
	return(lpCstat.cbInQue);
}

//------------------------------------------
// 受信開始処理 (メインスレッドの受信開始処理が完了するまでブロック)
// LPVOID pData 受信領域
// DWORD len 受信するサイズ
// HANDLE evEvent 受信完了時に通知するシグナル
//------------------------------------------
int Rs232cBase::Recv(LPVOID pData, DWORD len)
{
	return Recv(pData, len, NULL);
}
int Rs232cBase::Recv(LPVOID pData, DWORD len, HANDLE evEvent)
{
	if (m_hCom == INVALID_HANDLE_VALUE) return -1;	// 通信ポートがクローズされている
	if (STAT_WAIT_RECV(m_stat)) return -2;	// 前のリードを実行中
	if (!STAT_ACTIVE(m_stat)) return -3;	// 子スレッドがとまっている

	// セット
	m_pRecvData = (char*)pData;
	m_nRecvSz = len;
	m_evRecvEnd  = evEvent;									// 受信完了時に通知するシグナル (NULL時はメッセージ通知となる)
	// 受信開始依頼
	SetEvent(m_evRecvStart);								// メインスレッドに受信開始を通知
	DWORD ret = WaitForSingleObject(m_evRecvAck, 1000);		// メインスレッドが受信開始処理完了するまでブロック
	if (WAIT_TIMEOUT == ret) return -4;
	return 0;
}

//------------------------------------------
// 送信開始処理 (メインスレッドの送信開始処理が完了するまでブロック)
// LPVOID pData 送信領域
// DWORD len 送信するサイズ
// HANDLE evEvent 送信完了時に通知するシグナル
//------------------------------------------
int Rs232cBase::Send(LPCVOID pData, DWORD len)
{
	return Send(pData, len, NULL);
}
int Rs232cBase::Send(LPCVOID pData, DWORD len, HANDLE evEvent)
{
	if (m_hCom == INVALID_HANDLE_VALUE) return -1;	// 通信ポートがクローズされている
	if (STAT_WAIT_SEND(m_stat)) return -2;	// 前のライトを実行中
	if (!STAT_ACTIVE(m_stat)) return -3;	// 子スレッドがとまっている

	// セット
	m_pSendData = (char const*)pData;
	m_nSendSz = len;
	m_evSendEnd = evEvent;									// 送信完了時に通知するシグナル (NULL時はメッセージ通知となる)
	// 送信開始依頼
	SetEvent(m_evSendStart);								// メインスレッドに送信開始を通知
	DWORD ret = WaitForSingleObject(m_evSendAck, 3000);		// メインスレッドが送信開始処理完了するまでブロック
	if (WAIT_TIMEOUT == ret) return -4;
	return 0;
}

//------------------------------------------
// 送受信待ちを取りやめ
//------------------------------------------
void Rs232cBase::Cancel()
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
void Rs232cBase::SetTimeout(DWORD timeout)
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
void Rs232cBase::sendMsg(UINT msg, WPARAM wParam, LPARAM lParam)
{
	if( msg == WM_KS232C_RECV && m_evRecvEnd != NULL ) {					// 受信完了 (シグナル通知)
		SetEvent(m_evRecvEnd);

	} else if( msg == WM_KS232C_SEND && m_evSendEnd != NULL ) {				// 送信完了 (シグナル通知)
		SetEvent(m_evSendEnd);

	} else if (0 != m_nTid) {												// メッセージ通知
		// WM_KS232C_RECV,		自インスタンス値,		0固定
		// WM_KS232C_SEND,		自インスタンス値,		0固定

		// WM_KS232C_ERR,		自インスタンス値,		異常コード (m_apierr)
		PostThreadMessage(m_nTid, m_nMsgBase+msg, wParam, lParam);			
	}
}



//
// スレッド本体
//
//------------------------------------------
// メインスレッド
//------------------------------------------
unsigned int __stdcall Rs232cBase::MainThread(LPVOID arg)
{
	// メインスレッド準備完了
	Rs232cBase * p = (Rs232cBase *)arg;
	SET_ACTIVE(p->m_stat);

	// 同期制御用イベント
	OVERLAPPED ovlSend, ovlRecv;
	memset(&ovlSend, 0, sizeof(OVERLAPPED));
	memset(&ovlRecv, 0, sizeof(OVERLAPPED));
	ovlSend.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	ovlRecv.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);

	DWORD recvSize, sendSize, recvTotal, sendTotal, restRecv, restSend;
	DWORD recvStartTick, sendStartTick;
	
	// イベント作成
	HANDLE evArray[7] = {p->m_evStopEvent, p->m_evSetTimeout, p->m_evCancelEvent,
						 p->m_evRecvStart, p->m_evSendStart,
						 ovlRecv.hEvent, ovlSend.hEvent};
	enum { EV_STOP = WAIT_OBJECT_0,			// 終了通知
		   EV_SETTIMEOUT,					// タイムアウト時間再設定
		   EV_CANCEL,						// 送受信キャンセル
		   EV_RECV_START,					// 受信開始依頼
		   EV_SEND_START,					// 送信開始依頼
		   EV_OVL_RECV,						// 非同期受信完了
		   EV_OVL_SEND,						// 非同期送信完了
		   EV_END};

	while (true) {
		DWORD dwWait = WaitForMultipleObjects(EV_END, evArray, FALSE, p->m_nTimeout);
		DWORD evNum = dwWait - WAIT_OBJECT_0;

		// 停止は最優先事項
		if (evNum == EV_STOP) {	
			CancelIo(p->m_hCom);
			CloseHandle(ovlSend.hEvent);
			CloseHandle(ovlRecv.hEvent);
			RST_ACTIVE(p->m_stat);				
			return 0;							// 正常停止
		}

		// 受信タイムアウトチェック
		if (STAT_WAIT_RECV(p->m_stat) && WAIT_TIMEOUT == dwWait) {	// 読み待ちのとき
			if (GetTickCount() < 0x80000000 && recvStartTick > 0x80000000) { // ラップアラウンドではもう一度待つ
				recvStartTick = GetTickCount();
			} else if ((GetTickCount() - recvStartTick) >= p->m_nTimeout) {
				SET_RECV_TIMEOUT(p->m_stat);
				p->sendMsg(WM_KS232C_RECV_TIMEOUT, (WPARAM)p, 0);	// 自インスタンスを返す
			}
		}
		// 送信タイムアウトチェック
		if (STAT_WAIT_SEND(p->m_stat) && WAIT_TIMEOUT == dwWait) {	// 書き待ちのとき
			if (GetTickCount() < 0x80000000 && sendStartTick > 0x80000000) { // ラップアラウンドではもう一度待つ
				sendStartTick = GetTickCount();
			} else if ((GetTickCount() - sendStartTick) >= p->m_nTimeout) {
				SET_SEND_TIMEOUT(p->m_stat);
				p->sendMsg(WM_KS232C_SEND_TIMEOUT, (WPARAM)p, 0);	// 自インスタンスを返す
			}
		}

		// タイムアウト 又は、 タイムアウト時間再設定 の場合は、ここで終わり
		if (WAIT_TIMEOUT == dwWait || evNum == EV_SETTIMEOUT) continue;


		switch (evNum) {
		case EV_CANCEL:							// CancelIo
			RST_WAIT_RECV(p->m_stat);
			RST_WAIT_SEND(p->m_stat);
			break;
			
		case EV_RECV_START:						// 非同期読み取りを開始する
			SET_WAIT_RECV(p->m_stat);
			if (!ReadFile(p->m_hCom, p->m_pRecvData, p->m_nRecvSz, NULL, &ovlRecv)) {
				p->m_apierr = GetLastError();			// 即失敗の場合
				if (p->m_apierr != ERROR_IO_PENDING) goto read_abort;
			}
			recvStartTick = GetTickCount();				// 処理開始時間セット (タイムアウトチェックに使用)
			recvTotal = 0;
			SetEvent(p->m_evRecvAck);					// 受信開始処理完了
//_RPT1(_CRT_WARN, "RecvStart %p   ------>>>\n", p);
			break;
			
		case EV_SEND_START:						// 非同期送信を開始する
			SET_WAIT_SEND(p->m_stat);
			if (!WriteFile(p->m_hCom, p->m_pSendData, p->m_nSendSz, NULL, &ovlSend)) {
				p->m_apierr = GetLastError();			// 即失敗の場合
				if (p->m_apierr != ERROR_IO_PENDING) goto send_abort;
			}
			sendStartTick = GetTickCount();				// 処理開始時間セット (タイムアウトチェックに使用)
			sendTotal = 0;
			SetEvent(p->m_evSendAck);					// 送信開始処理完了
//_RPT1(_CRT_WARN, "SendStart %p\n", p);
			break;
			
		case EV_OVL_RECV:						// 非同期読み取りの完了
			if (GetOverlappedResult(p->m_hCom, &ovlRecv, &recvSize, TRUE)) {	// なぜかTRUEでないといけない
				if (0 == recvSize) { // ポートが閉じた場合
					p->m_apierr = 0;
					goto read_abort;
				}
				recvStartTick = GetTickCount();			// 一回取得できたので再度新しい時間をセット (タイムアウトチェックに使用)
				
				// 受信形式により処理分け
				if(NULL == p->m_cRecvCR) {				//// 固定長読み込み
					// 正常にリード
					recvTotal += recvSize;
					if (recvTotal == p->m_nRecvSz) {
						// データ読み込み完了
						ResetEvent(ovlRecv.hEvent);
						RST_RECV_TIMEOUT(p->m_stat);
						RST_WAIT_RECV(p->m_stat);	// ステータス降ろす
						// 通知
						p->sendMsg(WM_KS232C_RECV, (WPARAM)p, 0);
						break;
					}
					// 次のリード発行 (一回で取り込めない場合。指定サイズ取り込むまで繰り返す)
					restRecv = p->m_nRecvSz - recvTotal;
					if (!ReadFile(p->m_hCom, p->m_pRecvData+recvTotal, restRecv, NULL, &ovlRecv)) {
						p->m_apierr = GetLastError();	// 即失敗の場合
						if (p->m_apierr != ERROR_IO_PENDING) goto read_abort;
					}

				} else {								//// 終了文字列チェック
//_RPT2(_CRT_WARN, "Recving [%c] %p\n", p->m_pRecvData[recvTotal], p);					
					// 終端かチェック
					if( p->m_cRecvCR == p->m_pRecvData[recvTotal] ) {
						// データ読み込み完了
						ResetEvent(ovlRecv.hEvent);
						RST_RECV_TIMEOUT(p->m_stat);
						RST_WAIT_RECV(p->m_stat);	// ステータス降ろす
						// 通知
						p->sendMsg(WM_KS232C_RECV, (WPARAM)p, 0);
//_RPT1(_CRT_WARN, "RecvEnd %p   <<<-----------\n", p);
						break;
					}
					// 次のリード発行 (一回で取り込めない場合。指定サイズ取り込むまで繰り返す)
					recvTotal += recvSize;		// 可変長の場合は、receSizeは1固定かな
					if (!ReadFile(p->m_hCom, &p->m_pRecvData[recvTotal], recvSize, NULL, &ovlRecv)) {
						p->m_apierr = GetLastError();	// 即失敗の場合
						if (p->m_apierr != ERROR_IO_PENDING) goto read_abort;
					}
				}

				break;
			} else {									// リードエラー発生
				p->m_apierr = GetLastError();
read_abort :

				CloseHandle(ovlSend.hEvent);
				CloseHandle(ovlRecv.hEvent);
				SET_THREAD_ERR(p->m_stat);
				RST_ACTIVE(p->m_stat);
				p->sendMsg(WM_KS232C_ERR, (WPARAM)p, p->m_apierr);			// 異常切断通知
				return 0;											// メインスレッド 受信異常により終了
			}
			
		case EV_OVL_SEND:						// 非同期書き込みの完了
			if (GetOverlappedResult(p->m_hCom, &ovlSend, &sendSize, TRUE)) {
				// 正常に送信
				sendStartTick = GetTickCount();			// 一回取得できたので再度新しい時間をセット (タイムアウトチェックに使用)
				sendTotal += sendSize;
				if (sendTotal == p->m_nSendSz) {
					// データ書き込み完了
					ResetEvent(ovlSend.hEvent);
					RST_SEND_TIMEOUT(p->m_stat);
					RST_WAIT_SEND(p->m_stat); // ステータス降ろす
					// 通知
					p->sendMsg(WM_KS232C_SEND, (WPARAM)p, 0);
//_RPT1(_CRT_WARN, "SendEnd %p\n", p);
					break;
				}
				// 次の送信発行 (一回で送れない場合。指定サイズ送れるまで繰り返す)
				restSend = p->m_nSendSz	- sendTotal;
				if (!WriteFile(p->m_hCom, p->m_pSendData+sendTotal, restSend, NULL, &ovlSend)) {
					p->m_apierr = GetLastError();	// 即失敗の場合
					if (p->m_apierr != ERROR_IO_PENDING) goto send_abort;
				}
				break;
			} else {  // 送信エラー発生
				p->m_apierr = GetLastError();
send_abort:
				CloseHandle(ovlSend.hEvent);
				CloseHandle(ovlRecv.hEvent);
				SET_THREAD_ERR(p->m_stat);
				RST_ACTIVE(p->m_stat);
				p->sendMsg(WM_KS232C_ERR, (WPARAM)p, p->m_apierr);			// 異常切断通知
				return 0;											// メインスレッド 送信異常により終了
			}

		default:	// ありえない
			continue;
		}
	}
}
