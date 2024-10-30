// *********************************************************************************
//	当タスクの基本定数
//	[Ver]
//		Ver.01    2007/08/01  vs2005 対応
//
//	[メモ]
//		
// *********************************************************************************

#pragma once

#include "..\..\Include\KizuCommon.h"									// 疵検共通ヘッダー

//=======================================
// 管理定数
//=======================================
#define MAX_THREAD			8										// 最大スレッド管理数
#define MAX_LED_BLOCK		256										// 照明装置 最大配光ブロック数
#define MAX_LED_CH			4										// 照明装置 最大チャンネル数
#define MAX_TEMP_UNIT		9										// 照明装置 温度ユニット最大数

#define DEF_ALL_INDEX		0										// 全Index指定
#define DEF_INDEX_END		4										// Index終端

//=======================================
// メッセージ通知
//=======================================
//// 全てMainInctanceで受ける
//       場合によっては、MainInctanceが各ワーカースレッドにたれ流しする
enum ENUM_MAININCTANCE {
	E_DEF_ML_PARAM = WM_USER + 100,									// パラメータ変更通知


	E_DEF_END														// 終了
};

#define KT_LEDMGR "KT_LEDMGR"

#define DummyLED "DummyLED"
