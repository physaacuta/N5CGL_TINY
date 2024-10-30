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
#include "..\..\Include\HyTbl.h"										// 
#include "..\..\Include\KizuCommonSelect.h"								// 
#include "..\..\Include\KizuListID.h"									// 
#include "..\..\Include\KizuSmyu.h"										// 
#include "..\..\Include\LibSelect.h"									// 
#include "..\..\Include\LineCommon.h"									// 
#include "..\..\Include\OutIF_HT_RAW.h"									// 
#include "..\..\Include\OutInterface.h"									// 
#include "..\..\Include\ToHtCommon.h"									// 
#include "..\..\Include\ToTbl.h"										// 


//=======================================
// メッセージ通知
//=======================================
//// 全てMainInctanceで受ける
//       場合によっては、MainInctanceが各ワーカースレッドにたれ流しする
enum ENUM_MAININCTANCE {
	E_DEF_ML_PARAM = WM_USER + 100,									// パラメータ変更通知


	E_DEF_END														// 終了
};

