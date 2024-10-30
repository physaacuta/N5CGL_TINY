// *********************************************************************************
//	RS-232C通信 クラス
//	[Ver]
//		Ver.01    2007/01/30  vs2005 対応
//
//	[メモ]
//		
// *********************************************************************************

#pragma once

#include <windows.h>

#pragma warning(disable : 4996)							// ***が古い形式として宣言されました。を出さない対策

namespace Kizu232C
{
	//------------------------------------------
	// 定数
	//------------------------------------------
	// メッセージ
	// 実際にはmsgbaseが加算されて送られる
	#define	WM_KS232C_RECV			1
	#define	WM_KS232C_SEND			2
	#define	WM_KS232C_ERR			3
	#define	WM_KS232C_RECV_TIMEOUT	4
	#define	WM_KS232C_SEND_TIMEOUT	5

	// ステータスビット
	#define KIZU232C_STAT_ACTIVE		0x01L			// スレッドが生きている状態
	#define KIZU232C_STAT_WAIT_RECV		0x02L			// データ読み込み中
	#define KIZU232C_STAT_WAIT_SEND		0x04L			// データ送信中
	#define KIZU232C_STAT_THREAD_ERR	0x80000000L		// 何らかのエラーでスレッドが終了
	#define KIZU232C_STAT_RECV_TIMEOUT	0x100L			// 受信がタイムアウトしている
	#define	KIZU232C_STAT_SEND_TIMEOUT	0x200L			// 送信がタイムアウトしている

	// ステータス操作
	#define	STAT_ACTIVE(x)	((x) & KIZU232C_STAT_ACTIVE)
	#define	SET_ACTIVE(x) ((x) |= KIZU232C_STAT_ACTIVE)
	#define	RST_ACTIVE(x) ((x)&= ~KIZU232C_STAT_ACTIVE)

	#define	STAT_WAIT_RECV(x)	((x) & KIZU232C_STAT_WAIT_RECV)
	#define	SET_WAIT_RECV(x) ((x)|= KIZU232C_STAT_WAIT_RECV)
	#define	RST_WAIT_RECV(x) ((x) &= ~KIZU232C_STAT_WAIT_RECV)

	#define	STAT_WAIT_SEND(x)	((x) & KIZU232C_STAT_WAIT_SEND)
	#define	SET_WAIT_SEND(x) ((x) |= KIZU232C_STAT_WAIT_SEND)
	#define	RST_WAIT_SEND(x) ((x) &= ~KIZU232C_STAT_WAIT_SEND)

	#define	STAT_THREAD_ERR(x)	((x) & KIZU232C_STAT_THREAD_ERR)
	#define	SET_THREAD_ERR(x) ((x) |= KIZU232C_STAT_THREAD_ERR)
	#define	RST_THREAD_ERR(x) ((x) &= ~KIZU232C_STAT_THREAD_ERR)

	#define	STAT_RECV_TIMEOUT(x)	((x) & KIZU232C_STAT_RECV_TIMEOUT)
	#define	SET_RECV_TIMEOUT(x) ((x) |= KIZU232C_STAT_RECV_TIMEOUT)
	#define	RST_RECV_TIMEOUT(x) ((x) &= ~KIZU232C_STAT_RECV_TIMEOUT)

	#define	STAT_SEND_TIMEOUT(x)	(x & KIZU232C_STAT_SEND_TIMEOUT)
	#define	SET_SEND_TIMEOUT(x) (x |= KIZU232C_STAT_SEND_TIMEOUT)
	#define	RST_SEND_TIMEOUT(x) (x &= ~KIZU232C_STAT_SEND_TIMEOUT)


	//------------------------------------------
	// クラス
	//------------------------------------------
	class Rs232cBase
	{
	public:
		
		Rs232cBase(const char *cSession, const char *cIniPath, DWORD tid,
				   DWORD r_size = 0, DWORD s_size = 0, DWORD msgbase = WM_USER, bool bSoftFolwOff = false);		// コンストラクタ
		virtual ~Rs232cBase(void);														// デストラクタ

		// 動作
		int Recv(LPVOID pData, DWORD len);												// 受信開始処理 (メインスレッドの受信開始処理が完了するまでブロック)
		int Recv(LPVOID pData, DWORD len, HANDLE evEvent);
		int Send(LPCVOID pData, DWORD len);												// 送信開始処理 (メインスレッドの送信開始処理が完了するまでブロック)
		int Send(LPCVOID pData, DWORD len, HANDLE evEvent);	
		void Cancel();																	// 送受信待ちを取りやめ

		// SET
		void SetTimeout(DWORD timeout);													// タイムアウト再設定
		void SetRecvEndCR(char cr) { m_cRecvCR = cr; }									// 受信完了判断CR (1バイトづつ読み込み、CRと同じだったら 受信完了とする)

		// GET
		DWORD GetQueueLen() const;														// 通信エラーの情報確認

	private:
		static unsigned int __stdcall MainThread(LPVOID arg);							// メインスレッド
		void	sendMsg(UINT msg, WPARAM wParam, LPARAM lParam);						// メッセージ・イベント 通知


		DWORD	m_nTid;						// 通知先 (PostThreadMessageの送り先)
		DWORD	m_nMsgBase;					// メッセージベース番号

		HANDLE	m_hCom;						// RS-232C 通信回線ハンドル
		HANDLE	m_hThread;					// メインスレッドハンドル
		DWORD	m_nTimeout;					// タイムアウトの値
		DWORD	m_stat;						// ステータス
		DWORD	m_apierr;					// GetLastErrorの値
		char	m_cRecvCR;					// 受信完了判断CR

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
		HANDLE	m_evRecvEnd;				// 受信完了通知 イベント				
		HANDLE	m_evSendEnd;				// 送信完了通知 イベント
	};
};


