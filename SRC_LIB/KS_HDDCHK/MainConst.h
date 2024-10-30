// *********************************************************************************
//	当タスクの基本定数
//	[Ver]
//		Ver.01    2007/08/01  vs2005 対応
//
//	[メモ]
//		
// *********************************************************************************

#pragma once

#include "..\..\Include\LineCommon.h"									// 疵検共通ヘッダー


//=======================================
// メッセージ通知
//=======================================
//// 全てMainInctanceで受ける
//       場合によっては、MainInctanceが各ワーカースレッドにたれ流しする
enum ENUM_MAININCTANCE {
	E_DEF_ML_PARAM = WM_USER + 100,									// パラメータ変更通知


	E_DEF_END														// 終了
};

// 20181220 Smart Storage Administrator対応 --->>>
enum EM_HDDCHK_MODE {
	EM_HDDCHK_MODE_URU = 0,											// Universal Raid Utility
	EM_HDDCHK_MODE_SSA												// Smart Storage Administrator
};
// 20181220 Smart Storage Administrator対応 ---<<<

