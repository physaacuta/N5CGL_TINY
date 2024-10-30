// *********************************************************************************
//	当タスクの基本定数
//	[Ver]
//		Ver.01    2007/08/01  vs2005 対応
//
//	[メモ]
//		
// *********************************************************************************

#pragma once

#include "..\..\Include\KizuCommon.h"									// 検査共通ヘッダー

#pragma once

// Mail転送するためのUDPのパケット構造。固定長。
struct UdpMail {
	char		hostname[16];		// UDPを受信した人が、メールスロットを投げる先のPC
	char		mailname[16];		// メールスロット名
	int			datalen;			// データ長さ。（下のdataのうち、どれだけ有効か）。
	unsigned char	data[512];		// メールスロットのデータ。(UDPとしては固定）
};

// メールスロットUdpProxyサーバの、UDPポート。固定。
static const WORD KS_MAILTR_PORT = 10445;


//=======================================
// メッセージ通知
//=======================================
//// 全てMainInctanceで受ける
//       場合によっては、MainInctanceが各ワーカースレッドにたれ流しする
enum ENUM_MAININCTANCE {
	E_DEF_ML_TRANS = WM_USER + 100,									// メールスロット転送要求

	E_DEF_END														// 終了
};



