#include "StdAfx.h"
#include "OdbcSetFunc.h"


//===========================================================================
// DBの操作関連
//===========================================================================


//------------------------------------------
// ODBC自動設定 (同一DNSの場合は、変更となる)
// int nDriverID ドライバ区分 (0:SQL Server, 1:SQL Native Client, 2:SQL Native Client 10.0, 3:SQL Native Client 11.0)  
// LPCSTR cDNS 勝手に付けれる名前(ODBCの名前)
// LPCSTR cDataSource データベース接続アドレス
// LPCSTR cDatabase データベース名 (NULL許可)
// 実行するタスクに ODBCCP32.lib を追加する事
//------------------------------------------
BOOL ODBC_Setting(int nDriverID, LPCSTR cDSN, LPCSTR cDataSource, LPCSTR cDataBase)
{
	// ODBCクラスへの追加は、難しい ( = KizuLibCLIでエラーが発生する)

// #include <odbcinst.h>	// が必要
// ODBCCP32.lib が必要
	// VS2015 からは、legacy_stdio_definitions.lib も必要。


//DSN = 勝手に付けれる名前
//DESCRIPTION = 説明書き
//TRUSTED_CONNECTION=No これでSQL認証に出来るはず

// 各項目の間にNULLが必要
// ラストはNULL2つ以上必要


	//// 接続文字列
	int len;
	char wk[256];
	memset(wk, 0x00, sizeof(wk));


	//// 追加するドライバを指定
	char deriver[256];
	memset(deriver, 0x00, sizeof(deriver));
	if(0 == nDriverID) {
		strcpy(deriver, "SQL Server");
	} else if(1 == nDriverID) {
		strcpy(deriver, "SQL Native Client");
	} else if(2 == nDriverID) {
		strcpy(deriver, "SQL Server Native Client 10.0");
	} else if(3 == nDriverID) {
		strcpy(deriver, "SQL Server Native Client 11.0");
	} else {
		return false;
	}

	// 削除用文字列
	len  = sprintf(wk		, "Trusted_Connection=no");		len += 1;
	len += sprintf(wk + len , "DSN=%s", cDSN);				
	
	//// ODBC自動削除 (ドライバ名は何でもよい。でも空白はNGだった)
	//  ※ 統括には、SQL Native Client を使用する為、ドライバが異なる。その為、一度消してから、再度作成する
	SQLConfigDataSource( NULL, ODBC_REMOVE_SYS_DSN, "SQL Server", wk );

	// 追加用文字列
	len  = sprintf(wk		, "Trusted_Connection=no");		len += 1;
	len += sprintf(wk + len , "DSN=%s", cDSN);				len += 1;
	len += sprintf(wk + len	, "SERVER=%s", cDataSource);	len += 1;
	len += sprintf(wk + len	, "DataBase=%s", cDataBase);

	//// ODBC自動登録 (ユーザー・パスは設定不可能。どっちにしてもプログラムで使うときに指定する為必要ない？)
	return SQLConfigDataSource( NULL, ODBC_ADD_SYS_DSN, deriver, wk );
}