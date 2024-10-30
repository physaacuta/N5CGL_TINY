// *********************************************************************************
//	SOCKET通信基底クラス
//	[Ver]
//		Ver.01    2007/01/30  vs2005 対応
//
//	[メモ]
//		使用するタスクで 『ws2_32.lib』 をリンクする必要無くしました。
//	[追加ライブラリファイル]
//		ws2_32.lib
// *********************************************************************************

#pragma once

#include <windows.h>
#include <winsock2.h>

#pragma warning( disable : 4312 )        // 警告メッセージ 4312 を 1 回だけ出力

namespace KizuSock3
{
	//------------------------------------------
	// 定数
	//------------------------------------------
	#define	KS_MAXCLIENTS	6 //4				// 一つのソケットの同時最大接続数 (判定PCが半分になったからこれでOK)
	#define	KS_SNDBUF		512*1024		// ソケットのデフォルトの送信バッファサイズ
	#define	KS_RCVBUF		512*1024		// ソケットのデフォルトの受信バッファサイズ

	// スレッドのリターンコード
	#define	THR_NORMAL_END	0
	#define	THR_RECV_ERR	1
	#define	THR_SEND_ERR	2
	#define	THR_CLOSE_END	3

	// メッセージ
	// 実際にはmsgbaseが加算されて送られる
	#define	WM_KS_ACCEPT		1					// アクセプト完了
	#define	WM_KS_RECV			2					// 受信完了
	#define	WM_KS_SEND			3					// 送信完了
	#define	WM_KS_DISCONNECT	4					// 切断された
	#define	WM_KS_RECV_TIMEOUT	5					// 受信タイムアウト
	#define	WM_KS_SEND_TIMEOUT	6					// 送信タイムアウト

	// メッセージのwParamにはソケットのIDが入る
	// lParamにはwindowsのエラーコードまたは０が入る

	// 関数エラーコード
	#define	KIZUSOCK_INVALID_HND		(-8)
	#define KIZUSOCK_INVALID_ID			(-9)
	#define	KIZUSOCK_ERROR				(-3)

	// ソケットの終了した区分
	#define KIZUSOCK_CLOSEKIND_NON		0			// 終了した事が無いから不明
	#define KIZUSOCK_CLOSEKIND_SOCK		1			// ソケットが切断された (= 相手からの切断)
	#define KIZUSOCK_CLOSEKIND_PROG		2			// 異常検出した為、プログラムで切断
	#define KIZUSOCK_CLOSEKIND_THEAD	3			// スレッド終了
	#define KIZUSOCK_CLOSEKIND_CANCEL	4			// キャンセルで切断

	// ステータスビット
	#define KIZUSOCK_STAT_ACTIVE		0x01L		// スレッドが生きている状態
	#define KIZUSOCK_STAT_WAIT_RECV		0x02L		// データ読み込み中
	#define KIZUSOCK_STAT_WAIT_SEND		0x04L		// データ送信中
	#define KIZUSOCK_STAT_THREAD_ERR	0x80000000L	// 何らかのエラーでスレッドが終了
	#define KIZUSOCK_STAT_RECV_TIMEOUT	0x100L		// 受信がタイムアウトしている
	#define	KIZUSOCK_STAT_SEND_TIMEOUT	0x200L		// 送信がタイムアウトしている

	// ステータス操作
	#define	STAT_ACTIVE(x)	((x) & KIZUSOCK_STAT_ACTIVE)
	#define	SET_ACTIVE(x) ((x) |= KIZUSOCK_STAT_ACTIVE)
	#define	RST_ACTIVE(x) ((x)&= ~KIZUSOCK_STAT_ACTIVE)

	#define	STAT_WAIT_RECV(x)	((x) & KIZUSOCK_STAT_WAIT_RECV)
	#define	SET_WAIT_RECV(x) ((x)|= KIZUSOCK_STAT_WAIT_RECV)
	#define	RST_WAIT_RECV(x) ((x) &= ~KIZUSOCK_STAT_WAIT_RECV)

	#define	STAT_WAIT_SEND(x)	((x) & KIZUSOCK_STAT_WAIT_SEND)
	#define	SET_WAIT_SEND(x) ((x) |= KIZUSOCK_STAT_WAIT_SEND)
	#define	RST_WAIT_SEND(x) ((x) &= ~KIZUSOCK_STAT_WAIT_SEND)

	#define	STAT_THREAD_ERR(x)	((x) & KIZUSOCK_STAT_THREAD_ERR)
	#define	SET_THREAD_ERR(x) ((x) |= KIZUSOCK_STAT_THREAD_ERR)
	#define	RST_THREAD_ERR(x) ((x) &= ~KIZUSOCK_STAT_THREAD_ERR)

	#define	STAT_RECV_TIMEOUT(x)	((x) & KIZUSOCK_STAT_RECV_TIMEOUT)
	#define	SET_RECV_TIMEOUT(x) ((x) |= KIZUSOCK_STAT_RECV_TIMEOUT)
	#define	RST_RECV_TIMEOUT(x) ((x) &= ~KIZUSOCK_STAT_RECV_TIMEOUT)

	#define	STAT_SEND_TIMEOUT(x)	(x & KIZUSOCK_STAT_SEND_TIMEOUT)
	#define	SET_SEND_TIMEOUT(x) (x |= KIZUSOCK_STAT_SEND_TIMEOUT)
	#define	RST_SEND_TIMEOUT(x) (x &= ~KIZUSOCK_STAT_SEND_TIMEOUT)

	//------------------------------------------
	// クラス
	//------------------------------------------
	class ksThread  
	{
	public:
		ksThread(SOCKET sock, DWORD timeout, DWORD tid, DWORD msgbase, 
				HANDLE evRecvTimeout, HANDLE evSendTimeout, HANDLE evDisconnect);			// コンストラクタ
		virtual ~ksThread();																// デストラクタ
		int Start(int nPriority = THREAD_PRIORITY_NORMAL);									// スレッド開始
		void Stop();																		// スレッド停止

		// 動作
		int Recv(LPVOID pData, DWORD len);													// 受信開始処理 (メインスレッドの受信開始処理が完了するまでブロック)			
		int Recv(LPVOID pData, DWORD len, HANDLE evEvent);									// 受信開始処理 (メインスレッドの受信開始処理が完了するまでブロック)
		int Send(LPCVOID pData, DWORD len);													// 送信開始処理 (メインスレッドの送信開始処理が完了するまでブロック)
		int Send(LPCVOID pData, DWORD len, HANDLE evEvent);									// 送信開始処理 (メインスレッドの送信開始処理が完了するまでブロック)
		void Cancel();																		// 送受信待ちを取りやめ

		// SET
		void SetTimeout(DWORD timeout);														// タイムアウト再設定
		void SetEvRecvTimeout(HANDLE evRecvTimeout) {m_evRecvTimeout = evRecvTimeout; };	// 受信タイムアウト イベント設定
		void SetEvSendTimeout(HANDLE evSendTimeout) {m_evSendTimeout = evSendTimeout; };	// 送信タイムアウト イベント設定
		void SetEvDisConnect(HANDLE evDisConnect)   {m_evDisConnect  = evDisConnect; };		// 切断通知 イベント設定

		// GET
		HANDLE GetThreadHandle() const {return m_hThread;};									// スレッドハンドル
		DWORD  GetStat()		 const {return m_stat;};									// ステータス
		DWORD  GetCloseKind()	 const {return m_nCloseKind;};								// ソケットの終了区分

	private:
		static unsigned int __stdcall MainThread(LPVOID arg); 								// メインスレッド
		void	sendMsg(UINT msg, WPARAM wParam, LPARAM lParam);							// メッセージ・イベント 通知


		DWORD	m_nTid;						// 通知先 (PostThreadMessageの送り先)
		DWORD	m_nMsgBase;					// メッセージベース番号
		HANDLE	m_sock;						// acceptで得られたソケットまたは主ソケット

		HANDLE	m_hThread;					// メインスレッドハンドル
		DWORD	m_nTimeout;					// タイムアウトの値
		DWORD	m_stat;						// ステータス
		DWORD	m_apierr;					// GetLastErrorの値
		DWORD	m_nCloseKind;				// ソケットの終了区分

		// 送受信用 バッファ
		char	*m_pRecvData;				// IN:受信ヘッダのポインタ
		DWORD	m_nRecvSz;					// IN:受信データのサイズ
		const char	*m_pSendData;			// IN:送信ヘッダのポインタ
		DWORD	m_nSendSz;					// IN:送信データのサイズ

		// 自スレッド操作用シグナル
		HANDLE	m_evStopEvent;				// IN:停止イベント
		HANDLE	m_evSetTimeout;				// IN:タイムアウト再設定
		HANDLE	m_evCancelEvent;			// IN:CancelIo発行						(送受信待ちを取りやめ)
		HANDLE	m_evRecvStart;				// IN:リード発行許可					(受信開始処理してね要求)
		HANDLE	m_evSendStart;				// IN:データ送信						(送信開始処理してね要求)
		HANDLE	m_evRecvAck;				// OUT:受信がスレッドに受け付けられた	(受信開始処理したよ通知)
		HANDLE	m_evSendAck;				// OUT:送信がスレッドに受け付けられた	(送信開始処理したよ通知)
		
		// イベント通知用シグナル
		HANDLE m_evRecvEnd;					// 受信完了通知 イベント				
		HANDLE m_evSendEnd;					// 送信完了通知 イベント
		HANDLE m_evRecvTimeout;				// 受信タイムアウト イベント
		HANDLE m_evSendTimeout;				// 送信タイムアウト イベント
		HANDLE m_evDisConnect;				// 切断通知 イベント
	};
};
