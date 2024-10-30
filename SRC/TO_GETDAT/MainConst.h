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
	E_DEF_ML_COIL = WM_USER + 100,									// コイル情報受信通知
	E_DEF_ML_COIL_REV,												// 表裏反転してコイル情報再受信

	E_DEF_END														// 終了
};

//=======================================
// 小物メソッド
//=======================================


//------------------------------------------
// 文字列を入れ替え
// char*     str1 文字列その1
// char*	 str2 文字列その2
// const int size 文字列のサイズ(256まで)デフォルトで、文字列の短い方の文字数
//------------------------------------------
static void ChangeString(char* str1, char* str2, const int size=-1)
{
	char cWk[256];
	memset(cWk, 0x00, 256);

	int  nSize = size;
	if( -1 == nSize )	nSize = (int)min(strlen(str1), strlen(str2));


	memcpy(cWk,  str1, nSize);	// str1 を wk   へ
	memcpy(str1, str2, nSize);	// str2 を str1 へ
	memcpy(str2, cWk,  nSize);	// wk   を str2 へ

}