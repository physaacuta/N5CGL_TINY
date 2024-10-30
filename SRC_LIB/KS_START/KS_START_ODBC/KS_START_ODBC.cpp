// KS_START_ODBC.cpp : コンソール アプリケーションのエントリ ポイントを定義します。
//

#include "stdafx.h"
#include "KS_START_ODBC.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 唯一のアプリケーション オブジェクトです。

CWinApp theApp;

using namespace std;

int _tmain(int argc, TCHAR* argv[], TCHAR* envp[])
{
	int nRetCode = 0;

	// MFC を初期化して、エラーの場合は結果を印刷します。
	if (!AfxWinInit(::GetModuleHandle(NULL), NULL, ::GetCommandLine(), 0))
	{
		// TODO: 必要に応じてエラー コードを変更してください。
		_tprintf(_T("致命的なエラー: MFC の初期化ができませんでした。\n"));
		nRetCode = 1;
	}
	else
	{
		// TODO: アプリケーションの動作を記述するコードをここに挿入してください。

		//// 第一引数に入ったセッションを使用
		char const * Session = argv[1];
//MessageBox(NULL, Session, "", 0);

		// ===>> 坂巻 2018.10.21 ODBCドライバ区分個別指定機能追加
		//if( 2 != argc ) return -9;
		if( 3 != argc ) return -9;
		// <<===
		if( NULL == Session ) return -8;

		// 今iniファイルに設定されているDBセッションでODBCを設定する
		char wk_addr[128];
		char wk_odbc[128];
		char wk_dsna[128];
		GetPrivateProfileString(Session, "DBADDLESS", "", (char *)wk_addr, sizeof(wk_addr), TASKINI_NAME);
		GetPrivateProfileString(Session, "ODBCNAME",  "", (char *)wk_odbc, sizeof(wk_odbc), TASKINI_NAME);
		GetPrivateProfileString(Session, "DSNAME",    "", (char *)wk_dsna, sizeof(wk_dsna), TASKINI_NAME);
		
		// データが全部あるかチェック
		if( 0 == strlen(wk_addr) ||
			0 == strlen(wk_odbc) ||
			0 == strlen(wk_dsna) ) {
			//LOG(em_MSG), "[CheckDB] DB Session情報無し");
			return -1;
		}
		
		// ドライバ
		int driverid = GetPrivateProfileInt("DB", "DRIVERID", -1, PCINI_NAME);
		// ===>> 坂巻 2018.10.21 ODBCドライバ区分個別指定機能追加
		int const nId = atoi(argv[2]);
		CString sWk;
		sWk.Format("DRIVERID_%d", nId);
		driverid = GetPrivateProfileInt("DB", sWk, driverid, PCINI_NAME);
		// <<===
		if(0 > driverid ) {
			//LOG(em_ERR), "ODBCのドライバ区分異常 [%d]", driverid);
			//_ASSERT(0);
			return -2;
		}

		// 書き換え
		if( ODBC_Setting(driverid, wk_odbc, wk_addr, wk_dsna) ) {
			return 0;
		} else {
			return -3;	
		}
	}

	return nRetCode;
}
