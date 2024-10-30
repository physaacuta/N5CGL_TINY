// *********************************************************************************
//	通信関連の送受信メッソドのインターフェース
//	[Ver]
//		Ver.01    2007/01/30  vs2005 対応
//
//	[メモ]
//
// *********************************************************************************
#pragma once

#include <windows.h>
#include "KizuMacro.h"												// マクロ集

namespace KizuLib
{
	// 同期
	interface ICommunity
	//class ICommunity
	{
		virtual int Recv(LPVOID, DWORD) = 0;						// 受信
		virtual int Send(LPCVOID, DWORD) = 0;						// 送信
	};
	// 非同期
	interface ICommunityAsync
	//class ICommunityAsync
	{
		virtual bool Recv(LPVOID, DWORD, OVERLAPPED*) = 0;			// 受信
		virtual bool Send(LPCVOID, DWORD, OVERLAPPED*) = 0;			// 送信
		virtual bool GetResult(OVERLAPPED*) = 0;					// 非同期結果問い合わせ
		virtual int GetError() = 0;									// 最終異常
	};

};