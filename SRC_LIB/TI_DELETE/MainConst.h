// *********************************************************************************
//	当タスクの基本定数
//	[Ver]
//		Ver.01    2007/08/01  vs2005 対応
//
//	[メモ]
//		
// *********************************************************************************

#pragma once

// 基本的にどこでも使用する可能性があるインクルード
#include "..\..\Include\KizuCommon.h"								// 疵検共通ヘッダー



//=======================================
// 簡単な共通関数
//=======================================
static CString GetThreadNameArray(char* c, int nNo) {
	CString s;
	s.Format("%s_%d", c, nNo);
	return s;
}
//------------------------------------------
// １行のデータから , 単位の文字列を取得
// CString& sVal 1行単位の情報
// int& curPos 区切り位置
// CString& sToken 取得した列の文字列
// 戻り値 復帰情報
//------------------------------------------
static bool GetColumnVal(CString& sVal, int& curPos, CString& sToken)
{
	CString sWk;
	sWk = sVal.Tokenize(",", curPos);
	sWk.Trim(_T(" 	"));					// 前後のスペース、タブを取り除く
	if( 0 == sWk.GetLength() ) return false;

	sToken = sWk;
	return true;
}


//=======================================
// 定数
//=======================================
static const int CONTROL_MAXCNT				= 10;					// コントロールスレッド最大数


//------------------------------------------
// 無害グレード削除定義
//------------------------------------------
static const int MAX_TBL_MAIN	= 4;
static const int MAX_TBL_SUB	= 16;
struct SETTING_TBL {
	CString		cTblResult;											// コイル実績テーブル
	CString		cKeyResultTime;										// コイル実績テーブルの日付列
	CString		cKeyMain;											// 主キー名
	CString		cKeySub;											// サブキー名

	int			nMainCnt;											// メインセット数
	struct MAIN {
		CString		cTblMain;										// メインテーブル名
		CString		cSqlWhere;										// 抽出SQL

		int			nSubCnt;										// Subセット数
		CString		cTblSub[MAX_TBL_SUB];							// Subテーブル名
	} main[MAX_TBL_MAIN];
};
