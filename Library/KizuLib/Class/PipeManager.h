// *********************************************************************************
//	Pipe クラス (クライアント側のみ)
//	[Ver]
//		Ver.01    2007/01/30  vs2005 対応
//		Ver.02	  2010/03/02  サーバーモードに対応
//
//	[メモ]
//		こちら側がサーバーとなって、CreateNamedPipeを実行する処理は実装してない為注意
// *********************************************************************************

#pragma once

#include "ICommunity.h"										// 通信関連の送受信メッソドのインターフェース

namespace KizuLib
{
	class PipeManager : implements ICommunity, 
						implements ICommunityAsync
	{
	public:
		PipeManager(LPCSTR PipeName, bool bIsServer=false);
		virtual ~PipeManager();

		void Cancel() const;												// 未処理のI/Oを取り消す
		DWORD Listen(OVERLAPPED *ovl);										// クライアントからの接続を受け付ける。(非ブロック)


		// ICommunityの実装
		virtual int Recv(LPVOID pDate, DWORD len);							// 受信
		virtual int Send(LPCVOID pDate, DWORD len);							// 送信

		// ICommunityAsyncの実装
		virtual bool Recv(LPVOID pDate, DWORD len, OVERLAPPED *ovl);		// 受信
		virtual bool Send(LPCVOID pDate, DWORD len, OVERLAPPED *ovl);		// 送信
		virtual bool GetResult(OVERLAPPED *ovl);							// 非同期結果問い合わせ
		virtual int GetError()	{ return m_nLastError; }					// 最終エラー取得 (抽象クラスを含む為constつけじゃだめ)

		// プロパティ
		HANDLE		GetPipeHandle()	{ return m_hPipe; }

	private:
		HANDLE					m_hPipe;									// パイプ本体
		bool					m_bIsServer;								// Server時true
		int						m_nLastError;								// 最終エラー
	};
};
