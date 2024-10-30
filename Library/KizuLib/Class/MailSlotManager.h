// *********************************************************************************
//	MailSlot クラス
//	[Ver]
//		Ver.01    2007/01/30  vs2005 対応
//
//	[メモ]
//
// *********************************************************************************

#pragma once
#pragma warning(disable : 4996)							// ***が古い形式として宣言されました。を出さない対策

#include "ICommunity.h"										// 通信関連の送受信メッソドのインターフェース



namespace KizuLib
{
	class MailSlotManager : implements ICommunity,
							implements ICommunityAsync
	{
	public:
		MailSlotManager();
		virtual ~MailSlotManager();

		// syslog等々でNewしてない時に使用するよう
		static int SendMail(LPCSTR SendSlotName, LPCSTR SendPcName, LPCVOID sendbuff, DWORD len);	// 送信
		static int Connect(LPCSTR SendSlotName, LPCSTR SendPcName, HANDLE* hSend);					// 送信スロット接続
		static void Close(HANDLE* hSend);															// スロット切断


		bool Create(LPCSTR SlotName, DWORD lReadTimeout = MAILSLOT_WAIT_FOREVER);					// スロット構築


		// ICommunityの実装
		virtual int Recv(LPVOID pDate, DWORD len);													// 受信
		virtual int Send(LPCVOID pDate, DWORD len);													// 送信

		// ICommunityAsyncの実装
		virtual bool Recv(LPVOID pDate, DWORD len, OVERLAPPED *ovl);								// 受信
		virtual bool Send(LPCVOID pDate, DWORD len, OVERLAPPED *ovl);								// 送信
		virtual bool GetResult(OVERLAPPED *ovl);													// 非同期結果問い合わせ
		virtual int GetError() { return m_nLastError; }												// 最終エラー取得 (抽象クラスを含む為constつけじゃだめ)

	private:
		HANDLE					m_hSlot;									// スロット本体
		int						m_nLastError;								// 最終エラー
		static CRITICAL_SECTION	m_csLock;									// クリティカルセクション (SendMailのセーフスレット化)
	};
};


