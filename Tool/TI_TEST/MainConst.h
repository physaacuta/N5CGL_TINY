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

#define FRAME_HT_SEND_PORT 2
#define FRAME_RAW_SEND

//=======================================
// デバック

//#define SOT_TEST								// 単体テスト時 コメントアウト
//#define SOT_IMG_OUTPUT						// 単体テスト用中間画像出力時 コメントアウト 

// テスト用
#define		TI_TEST		"TI_TEST"


//=======================================
// 切り替えスイッチ
//   ※これは使える
//=======================================

	
//=======================================
// 固定サイズ
//=======================================



//=======================================
// ログマクロ
#define LOGSOCK(t)  if(mcls_pLogSock!=NULL)       mcls_pLogSock->Write(KizuLib::LogFileManager::t		// ソケット用ログ出力
#define LOGCULL(t)  if(mcls_pLogCull!=NULL)       mcls_pLogCull->Write(KizuLib::LogFileManager::t		// 全長処理用ログ出力




//=======================================
// メッセージ通知
//=======================================
//// 全てMainInctanceで受ける
//       場合によっては、MainInctanceが各ワーカースレッドにたれ流しする
enum ENUM_MAININCTANCE {
	E_DEF_ML_PARAM = WM_USER + 100,				// パラメータ変更通知

	E_DEF_END									// 終了
};


#define FACT_TI_TEST_01					9801					// ツール間連動 起動要因
#define FACT_TI_TEST_02					9802					// ツール間連動 起動要因
