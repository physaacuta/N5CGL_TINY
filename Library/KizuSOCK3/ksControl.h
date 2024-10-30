// *********************************************************************************
//	SOCKET操作・接続クラス
//	[Ver]
//		Ver.01    2007/01/30  vs2005 対応
//
//	[メモ]
//		使用するタスクで 『ws2_32.lib』 をリンクする必要無くしました。
//	[追加ライブラリファイル]
//		ws2_32.lib
// *********************************************************************************

#pragma once

#pragma warning(disable : 4311)							// 警告メッセージ 4311 を 1 回だけ出力
#pragma warning(disable : 4996)							// ***が古い形式として宣言されました。を出さない対策

#include "ksThread.h"

namespace KizuSock3
{
	class ksControl  
	{
	public:

		ksControl(DWORD tid, DWORD msgbase);								// コンストラクタ
		virtual ~ksControl();												// デストラクタ

		// 動作
		int SockOpen(DWORD rcvBufSz, DWORD sndBufSz);						// ソケットを開く
		int	Connect(ksThread **newInst);									// 接続して子スレッドを作成する（メインスレッドはなし）
		int Listen(int backlog);											// 非同期ですぐに戻る。実際につながるとメッセージが送られる
		int CloseChild(ksThread *pChild);									// 切断して解放

		// GET
		int IsValidChild(UINT id) const;									// 子スレッドが有るか
		int	GetActive() const;												// 今生きている子スレッドの数を返す
		int GetInstCount() const {return m_nInstCount;};					// 生成した子スレッドの数
		void GetAcceptEventMsg(MSG* msg);									// アクセプトシグナル通知時にMSG情報を取得 (アクセプトシグナル通知時のみ使用)

		// SET
		void SetAddr(const char *ip, short port);							// IP、ポート設定
		void SetTimeout(DWORD timeout);										// タイムアウト再設定
		void SetEvRecvTimeout(HANDLE evRecvTimeout);						// 受信タイムアウト イベント設定
		void SetEvSendTimeout(HANDLE evSendTimeout);						// 送信タイムアウト イベント設定
		void SetEvDisConnect(HANDLE evDisConnect);							// 切断通知 イベント設定
		void SetEvAccept(HANDLE evAccept);									// アクセプト通知 イベント設定 (KizuSock3で使えるようにした。シグナル通知後必ずGetAcceptEventMsgコールする事)
		void SetKsThPriority(int nPriority) { m_nKsThPriority=nPriority; }	// ksThreadの優先度を設定

	private:
		static unsigned int __stdcall MainThread(LPVOID arg);				// Listen用メインスレッド
		void	sendMsg(UINT msg, WPARAM wParam, LPARAM lParam);			// メッセージ・イベント 通知
		//friend static void sendMsg(ksControl *p, UINT msg, WPARAM wParam, LPARAM lParam);


		DWORD	m_nTid;						// 通知先 (PostThreadMessageの送り先)
		DWORD	m_nMsgBase;					// メッセージベース番号
		SOCKET	m_sock;						// 最初に作ったソケット

		HANDLE	m_hThread;					// メインスレッドハンドル
		DWORD	m_nTimeout;					// タイムアウトの値
		CRITICAL_SECTION	m_cs;			// m_nInstCountのためのクリティカルセクション

		// ソケット操作クラス
		ksThread*	mcls_pInst[KS_MAXCLIENTS];		// 子スレッドをを収容する配列
		DWORD		m_nInstCount;					// 生成された子スレッドの数
		char		m_Ip[20];				// IPアドレス
		short		m_nPort;				// ポート番号
		MSG			m_LastAcceptMsg;		// 最後にアクセプトされた情報 (アクセプトシグナル通知時のみ使用)
		int			m_nKsThPriority;		// 子スレッド(実際の送受信スレッド)の優先度

		// 自スレッド操作用シグナル
		HANDLE	m_evStopEvent;				// 停止イベント
		HANDLE	m_evAccepting;				// acceptで子スレッドが追加されたときのイベント
		HANDLE  m_evAcceptEndCollBack;		// アクセプト通信した後、処理完了時にシグナル (アクセプトシグナル通知時のみ使用)

		// イベント通知用シグナル
		HANDLE m_evRecvTimeout;				// 受信タイムアウト イベント
		HANDLE m_evSendTimeout;				// 送信タイムアウト イベント
		HANDLE m_evDisConnect;				// 切断通知 イベント
		HANDLE m_evAccept;					// アクセプト通知 イベント
	};
};