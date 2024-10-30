#pragma once

// *********************************************************************************
//	ODBC登録用
//	[Ver]
//		Ver.01    2012/03/23  vs2005 対応
//
//	[メモ]
//		x86,x64に対応させるため、ファンクション化
// *********************************************************************************

// #include <odbcinst.h>	// が必要
// ODBCCP32.lib が必要

#include <windows.h>
#include <odbcinst.h>													// ODBC自動設定用



// DBのIniファイル操作関連
BOOL ODBC_Setting(int nDriverID, LPCSTR cDSN, LPCSTR cDataSource, LPCSTR cDataBase);	// ODBC自動設定 (同一DNSの場合は、変更となる)
