// *********************************************************************************
//	TCP/IP 通信 の基底クラス
//	[Ver]
//		Ver.01    2007/01/30  vs2005 対応
//
//	[メモ]
//		使用するタスクで 『ws2_32.lib』 をリンクする必要無くしました。
//	[追加ライブラリファイル]
//		ws2_32.lib
// *********************************************************************************

#pragma once

#include "ksControl.h"

namespace KizuSock3
{
	class SockBase  
	{
	public:
		SockBase(const char *cSession, const char *cIniPath, DWORD tid, DWORD timeout, DWORD client_num = 1,
				 DWORD r_size = 0, DWORD s_size = 0, DWORD msgbase = WM_USER);						// コンストラクタ

		virtual ~SockBase();																		// デストラクタ

		void SetOpt_NewAcceptNgMode(int nMode)		{my_nNewAcceptNgMode=nMode;}

		//// メッセージ通知からイベント通知に変更
		void SetEvAccept(HANDLE evAccept)			{mycls_pKc->SetEvAccept(evAccept);};			// アクセプト通知 イベント設定 (ksThreadには関係なし)
		// 全部のソケットに対して同じシグナル
		void SetEvRecvTimeout(HANDLE evRecvTimeout) {mycls_pKc->SetEvRecvTimeout(evRecvTimeout);};	// 受信タイムアウト イベント設定
		void SetEvSendTimeout(HANDLE evSendTimeout) {mycls_pKc->SetEvSendTimeout(evSendTimeout);};	// 送信タイムアウト イベント設定
		void SetEvDisConnect(HANDLE evDisConnect)	{mycls_pKc->SetEvDisConnect(evDisConnect);};	// 切断通知 イベント設定

		// 指定のソケットのみ
		void SetEvRecvTimeout(int id, HANDLE evRecvTimeout) { if(NULL != mycls_pKt[id]) mycls_pKt[id]->SetEvRecvTimeout(evRecvTimeout); }	// 受信タイムアウト イベント設定
		void SetEvSendTimeout(int id, HANDLE evSendTimeout) { if(NULL != mycls_pKt[id]) mycls_pKt[id]->SetEvSendTimeout(evSendTimeout); }	// 送信タイムアウト イベント設定
		void SetEvDisConnect(int id, HANDLE evDisConnect)   { if(NULL != mycls_pKt[id]) mycls_pKt[id]->SetEvDisConnect(evDisConnect); }	// 切断通知 イベント設定


		// 操作
		int Connect();									// コネクト (接続されるまでブロックされる)
		int Listen();									// リスニング (ノンブロック)
		int Accept();									// アクセプト (シグナル用)
		int Accept(MSG *msg);							// アクセプト (メッセージ用)
		void Close(WPARAM ksT); 							// 特定のクライアントを切り離す (ポインタ指定)
		void Close(ksThread* ksT);
		void CloseID(int id);							// 特定のクライアントを切り離す (配列位置指定)
		void CloseAll();								// 全てのクライアントを切り離す
		void Cancel();									// 現在の読み込みキャンセルする関数

		int Recv(LPVOID pData, DWORD len, UINT ksT, HANDLE evRecvEnd = NULL);				// 受信開始依頼 (ポインタ指定)
		int Recv(LPVOID pData, DWORD len, ksThread* ksT, HANDLE evRecvEnd = NULL);		
		int Recv(int id, LPVOID pData, DWORD len, HANDLE evRecvEnd = NULL);					// 受信開始依頼 (配列位置指定)

		int Send(LPCVOID pData, DWORD len, UINT ksT, HANDLE evSendEnd = NULL);				// 送信開始依頼 (ポインタ指定)
		int Send(LPCVOID pData, DWORD len, ksThread* ksT, HANDLE evSendEnd = NULL);		
		int Send(int id, LPCVOID pData, DWORD len, HANDLE evSendEnd = NULL);				// 送信開始依頼 (配列位置指定)

		void SetKsThPriority(int nPriority) { if(NULL!=mycls_pKc) mycls_pKc->SetKsThPriority(nPriority); }	// ksThreadの優先度を設定

		// GET
		int GetSockID(WPARAM ksT) const;				// 指定されたポインタと一致する配列位置を返す
		int GetSockID(ksThread* ksT) const;
		int GetFreeID() const;							// 未割り当ての配列位置を返す
		int GetActive() const;							// 今生きている子スレッドの数を返す
		int GetCloseKindID(int id) const;				// ソケットの終了区分を返す

	protected:
		ksControl *mycls_pKc;							// ソケット操作クラス
		ksThread  *mycls_pKt[KS_MAXCLIENTS];			// 各クライアント用ソケットポインタ (id)

		long	  my_nClientAddr[KS_MAXCLIENTS];		// クライアントのIPアドレス(id)
		DWORD	  my_nClientNum;						// クライアントMAX数 (サーバーなら接続相手数分。クライアントなら通常1)

		int		  my_nNewAcceptNgMode;					// 新規Accept受付異常時の挙動を操作
														//   ハーフコネクション中の保持IPと新規コネクトIPが同じときの挙動
															// 0:従来どおり。ライブラリ内で新旧セッションをすげかえる
															// 1:昔のセッションを内部的に切断。で、今回のを新規で受け付ける。
	};
};