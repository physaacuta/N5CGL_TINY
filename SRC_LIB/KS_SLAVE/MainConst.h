// *********************************************************************************
//	当タスクの基本定数
//	[Ver]
//		Ver.01    2007/04/30  vs2005 対応
//
//	[メモ]
//		
// *********************************************************************************

#pragma once

//=======================================
// メッセージ通知
//=======================================
//// 全てMainInctanceで受ける
//       場合によっては、MainInctanceが各ワーカースレッドにたれ流しする
enum ENUM_MAININCTANCE {
	// メールスロット受信時
	E_DEF_ML_MASTEROK = WM_USER + 100,			// PCマスター管理立ち上げ完了通知

	E_DEF_END									// 終了
};