// *********************************************************************************
//	スタティック関数集
//	[Ver]
//		Ver.01    2007/01/30  vs2005 対応
//
//	[メモ]
//
// *********************************************************************************

#pragma once

#include <windows.h>

//#include "..\..\KizuODBC\OdbcBase.h"							// ODBC操作クラス

namespace KizuLib
{
	class KizuFunction
	{
	public:
		// 機器状態関係
		static bool SetStatus(int id, int state, bool bLogDsp =true, char const * cMsg = "" );			// DB_SYSTEM_STATEに機器状態をセットする (0:正常)
		static bool SetStatus(int id, bool state, bool bLogDsp =true, char const * cMsg  = "" );		//										 (true:正常)
			// iniファイルから都度読込版  セクションは基本
		static bool SetStatus(char const * cKey, bool state, bool bLogDsp =true, char const * cMsg  = "" );// iniファイルから機器IDを取得する (true:正常)
		static bool SetStatus(char const * cKey, int state, bool bLogDsp =true, char const * cMsg  = "" );// iniファイルから機器IDを取得する (true:正常)
		static bool SetStatus(char const * cKeyFormat, int id, int state, bool bLogDsp =true, char const * cMsg = "" );

			// セクションも指定版
		static bool SetStatus(char const* cSec, char const * cKey, bool state, bool bLogDsp =true, char const * cMsg  = "" ); // iniファイルから機器IDを取得する (true:正常)
		static bool SetStatus(char const* cSec, char const * cKey, int state, bool bLogDsp =true, char const * cMsg  = "" );  // iniファイルから機器IDを取得する (true:正常)
		static bool SetStatus(char const* cSec, char const * cKeyFormat, int id, int state, bool bLogDsp =true, char const * cMsg = "" );



		// 外部I/F関係
		static void DumpData(LPCSTR filename, long len, const BYTE* data, bool addTime = false);	// ダンプ
		static BOOL ByteHanten4(long len, BYTE* data);						// 4バイト反転 (元データ書き換えの為 注意)
		static BOOL ByteHanten2(long len, BYTE* data);						// 2バイト反転 (元データ書き換えの為 注意)
		static long ByteHanten4(BYTE* data);								// 4バイト反転後のデータ取得 (元データは変わらない)
		static short ByteHanten2(BYTE* data);								// 2バイト反転後のデータ取得 (元データは変わらない)
		static void SpaceToNull(char* cVal, int size);						// スペースをNULLに変更する
		static void Trim(char* cVal,int nMode=0);							// スペースをトリムする
		static void TrimSpaceNull(char* cT, int nTSize, char const* cF, int nFSize, int nMode=0);	// 固定長文字からStringに変換。

		static int AtoIex(char const* c, int size);							// 指定数分の文字を使用してIntに変換
		static float AtoFex(char const* c, int size);						// 指定数分の文字を使用してFloatに変換
		static double AtoDex(char const* c, int size);						// 指定数分の文字を使用してDoubleに変換

		// 主キー生成
		static void GetKizukenNo(char* cNo);								// 基本となる疵検管理Noを生成 (ID+yyyymmdd hhnnss\0)
		static void GetSerialNo(char* cNo);									// 基本となるシリアルNoを生成 (yyyymmdd hhnnss\0)
		static void GetSerialNoFF(char* cNo);								// 基本となるIDNoを生成       (yyyymmdd hhnnssff\0)

		// 内部処理関係
		static bool SetNetBiosComputerName(char const* cNewNetBiosComputerName); // NetBIOSのコンピューター名前
		static long GetFileSize(char const* cFPath);						// ファイルサイズを取得する

		// 疵検独自で良く使用する関数
		static bool SelectPriorityDetect(double nowLen, int nowYusenT, int nowYusenG, double preLen, int preYusenT, int preYusenG ); // 欠陥優先度比較
		static bool SelectPriorityDetect(int nCnt, double const* pNow, double const* pPre, bool const* bMode);
		static int  SelectGridCol(int width, int interval, int maxCol);		// グリッドの列数を算出する
		static int  SelectColPosition(int DorW, double edge_D, double edge_W, long width, long col_width, long col_max, long col_interval);		// グリッド位置判定関数 (センター基準)
			// (奇数対応版)
		static int  SelectGridCol2( int width, int interval, int maxCol);	// グリッドの列数を算出する
		static int	SelectColPosition2(int DorW, double edge_D, double edge_W, long width, long col_width, long col_max, long col_interval);
			// (固定列版)
		static int SelectColPositionFix(int DorW, double edge_D, double edge_W, long width, long col_max); // グリッド位置判定関数 (列数固定バージョン)

	};
};
