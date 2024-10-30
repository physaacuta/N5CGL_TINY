#include "StdAfx.h"
#include "OdbcManager.h"

#include <stdio.h>
#include <crtdbg.h> // デバック用

using namespace KizuODBC;

//////////////////////////////////////////////////////////////////////
// 構築/消滅
//////////////////////////////////////////////////////////////////////
//------------------------------------------
// コンストラクタ
//------------------------------------------
OdbcManager::OdbcManager(void) :
m_bIsConnect(false),
m_bIsCreateHandle(false),
m_nLastErrNo(0),
m_nColumnIndexCheck(0),
m_hdbc(NULL),
m_hstmt(NULL)
{
	// 初期化
	memset(g_cLastErrMsgMain, 0x00, sizeof(g_cLastErrMsgMain));
	memset(g_cLastErrMsgSub, 0x00, sizeof(g_cLastErrMsgSub));
	g_nLastErrMsgNo = 0;

	// 接続ハンドル生成
	Alloc();
}

//------------------------------------------
// デストラクタ
//------------------------------------------
OdbcManager::~OdbcManager(void)
{
	// 切断
	DisConnect();
	// 解放
	Free();
}

//------------------------------------------
// 接続ハンドル生成
//	環境ハンドルは、staticで生成されている
//------------------------------------------
void OdbcManager::Alloc()
{
	SQLRETURN sqlrc;

	// 初期化
	m_bIsConnect = false;
	m_bIsCreateHandle = false;
	m_nLastErrNo = 0;


	// 環境ハンドルが正常に存在？
	OdbcLockedEnv env;						// クリティカルセクション (ODBCはどうやらマルチスレッドでハンドルを生成したらハンドルリークしてそう)
	if( NULL == env.GetHandle() ){ //OdbcEnv::GetHandle() ) {
		m_nLastErrNo = -1;
		return;
	}

	// 接続ハンドルの取得
	sqlrc = SQLAllocHandle(SQL_HANDLE_DBC, env.GetHandle(), &m_hdbc);
	if (sqlrc == SQL_SUCCESS_WITH_INFO) {m_nLastErrNo = -103;	GetErrMsg(SQL_HANDLE_DBC); goto NG;}
	if (sqlrc != SQL_SUCCESS)			{m_nLastErrNo = -3;		GetErrMsg(SQL_HANDLE_DBC); goto NG;}

	// コネクト接続タイムアウト設定 (ログインタイムアウト時間 5秒) (=どうやらnative clientの時には無視されるらしい)
	sqlrc = SQLSetConnectAttr(m_hdbc, SQL_ATTR_LOGIN_TIMEOUT, (SQLPOINTER)5, SQL_IS_INTEGER);
///	if (sqlrc == SQL_SUCCESS_WITH_INFO) {m_nLastErrNo = -106;	GetErrMsg(SQL_HANDLE_DBC); goto NG;}	// エラーチェック無し (エラーになっても困らないから)(Accessに接続する場合などでエラーになる為)
///	if (sqlrc != SQL_SUCCESS)			{m_nLastErrNo = -6;		GetErrMsg(SQL_HANDLE_DBC); goto NG;}	


	// ここまで来たらハンドル正常
	m_bIsCreateHandle = true;
	return;
NG:
	// 解放
	Free();
}

//------------------------------------------
// 接続ハンドル解放
//------------------------------------------
void OdbcManager::Free()
{
	OdbcLockedEnv env;						// クリティカルセクション (ODBCはどうやらマルチスレッドでハンドルを生成したらハンドルリークしてそう)

	// ハンドル解放
	m_bIsCreateHandle = false;

	if(m_hdbc) {
		SQLFreeHandle(SQL_HANDLE_DBC, m_hdbc);
		m_hdbc = NULL;
	}
}

//------------------------------------------
// デフォルトセッションでの接続
// LPCSTR cIniPath iniファイルパス
// 戻り値 -:異常 1:接続済み 0:正常接続
//------------------------------------------
int OdbcManager::Connect(LPCSTR cIniPath)
{
	return Connect(cIniPath, KIZUODBC_ODBCMANAGER_SESSION);
}

//------------------------------------------
// セッション指定での接続
// LPCSTR cIniPath iniファイルパス
// LPCSTR cSession セッション
// 戻り値 -:異常 1:接続済み 0:正常接続
//------------------------------------------
int OdbcManager::Connect(LPCSTR cIniPath, LPCSTR cSession)
{
	char dsn[128];			// データベース名
	char uid[128];			// ユーザー名
	char pwd[128];			// パスワード

	// iniファイル取得
	GetPrivateProfileString(cSession, "ODBCNAME", "", dsn, sizeof(dsn), cIniPath);
	GetPrivateProfileString(cSession, "USERID",	  "", uid, sizeof(uid), cIniPath);
	GetPrivateProfileString(cSession, "PASSWORD", "", pwd, sizeof(pwd), cIniPath);

	// 任意指定で接続
	return Connect( dsn, uid , pwd);
}

//------------------------------------------
// 任意指定での接続
// char const* dsn  データベース名
// char const* uid  ユーザー名
// char const* pwd  パスワード
// 戻り値 -:異常 1:接続済み 0:正常接続
//------------------------------------------
int OdbcManager::Connect(char const* dsn, char const* uid, char const* pwd)
{
	SQLRETURN sqlrc;

	// 既に接続済み？
	if( m_bIsConnect ) return 1;

	OdbcLockedEnv env;						// クリティカルセクション (ODBCはどうやらマルチスレッドでハンドルを生成したらハンドルリークしてそう)

//*/
	// テストタイムアウトチェック
	//sqlrc = SQLSetConnectAttr(m_hdbc, SQL_ATTR_LOGIN_TIMEOUT, (SQLPOINTER)10, SQL_IS_INTEGER);
	//sqlrc = SQLSetConnectAttr(m_hdbc, SQL_ATTR_CONNECTION_TIMEOUT, (SQLPOINTER)1, SQL_IS_INTEGER);
//*/

	// 接続
	sqlrc = SQLConnect(m_hdbc, (SQLCHAR*)dsn, SQL_NTS, (SQLCHAR*)uid, SQL_NTS, (SQLCHAR*)pwd, SQL_NTS);
///	if (sqlrc == SQL_SUCCESS_WITH_INFO) {retc = -104; goto NG;}
	if (sqlrc != SQL_SUCCESS && sqlrc != SQL_SUCCESS_WITH_INFO)	{m_nLastErrNo = -4;	GetErrMsg(SQL_HANDLE_DBC); goto NG;}


	// オプション設定
	sqlrc = AutoCommitEnable(true);												// オートコミット禁止にしたら、バク続発しそうなので、オートコミットモードにしておこう
	///if (sqlrc != SQL_SUCCESS && sqlrc != SQL_SUCCESS_WITH_INFO) {retc = -5; goto NG;}
	if (sqlrc == SQL_SUCCESS_WITH_INFO) {m_nLastErrNo = -105;	GetErrMsg(SQL_HANDLE_DBC);goto NG;}
	if (sqlrc != SQL_SUCCESS)			{m_nLastErrNo = -5;		GetErrMsg(SQL_HANDLE_DBC);goto NG;}


	// エンド処理
	m_bIsConnect = true;
	memcpy( m_dsn, dsn, sizeof(m_dsn));

	return 0;
NG:
	// 切断
	DisConnect();
	return m_nLastErrNo;
}

//------------------------------------------
// ODBCを未定義で接続
// char const* addr データベースアドレス
// char const* dsn  データベース名
// char const* uid  ユーザー名
// char const* pwd  パスワード
// bool bNative SQL Native Client で接続時 true。通常はfalse
// 戻り値 -:異常 1:接続済み 0:正常接続
//------------------------------------------
int OdbcManager::ConnectDriver(char const* addr, char const* dsn, char const* uid, char const* pwd, bool bNative)
{
	SQLRETURN sqlrc;

	// 既に接続済み？
	if( m_bIsConnect ) return 1;

	OdbcLockedEnv env;						// クリティカルセクション (ODBCはどうやらマルチスレッドでハンドルを生成したらハンドルリークしてそう)

	// 接続文字列生成
	SQLCHAR     szOutConn[255];
	SQLSMALLINT cbOutConn;
	char cConnect[1024];
	//if(bNative) sprintf(cConnect, "DRIVER={SQL Native Client};SERVER=%s;DSN=%s;UID=%s;PWD=%s;", addr, dsn, uid, pwd);
	//else		sprintf(cConnect, "DRIVER={SQL Server};SERVER=%s;DSN=%s;UID=%s;PWD=%s;", addr, dsn, uid, pwd);
	if(bNative) sprintf(cConnect, "DRIVER={SQL Native Client};SERVER=%s;DATABASE=%s;UID=%s;PWD=%s;", addr, dsn, uid, pwd);	// SQLServer2005用
	else		sprintf(cConnect, "DRIVER={SQL Server};SERVER=%s;DATABASE=%s;UID=%s;PWD=%s;", addr, dsn, uid, pwd);

	// 接続
	sqlrc = SQLDriverConnect(m_hdbc, NULL, (SQLTCHAR*)cConnect , SQL_NTS, szOutConn, sizeof(szOutConn), &cbOutConn, SQL_DRIVER_NOPROMPT);
	if (sqlrc != SQL_SUCCESS && sqlrc != SQL_SUCCESS_WITH_INFO)	{m_nLastErrNo = -4;	GetErrMsg(SQL_HANDLE_DBC); goto NG;}

	// オプション設定
	sqlrc = AutoCommitEnable(true);												// オートコミット禁止にしたら、バク続発しそうなので、オートコミットモードにしておこう
	if (sqlrc == SQL_SUCCESS_WITH_INFO) {m_nLastErrNo = -105;	GetErrMsg(SQL_HANDLE_DBC);goto NG;}
	if (sqlrc != SQL_SUCCESS)			{m_nLastErrNo = -5;		GetErrMsg(SQL_HANDLE_DBC);goto NG;}


	// エンド処理
	m_bIsConnect = true;
	memcpy( m_dsn, dsn, sizeof(m_dsn));

	return 0;
NG:
	// 切断
	DisConnect();
	
	return m_nLastErrNo;
}

//------------------------------------------
// 切断
//------------------------------------------
void OdbcManager::DisConnect()
{
	OdbcLockedEnv env;						// クリティカルセクション (ODBCはどうやらマルチスレッドでハンドルを生成したらハンドルリークしてそう)

	// 切断
	if(m_hstmt)	{
		SQLFreeHandle(SQL_HANDLE_STMT, m_hstmt);
		m_hstmt = NULL;
	}
	if(m_hdbc) {
		SQLDisconnect(m_hdbc);
		m_bIsConnect = false;
	}
}

//------------------------------------------
// オートコミット ON/OFF
// bool flg トランザクションと使いたい場合は、falseにする事
//------------------------------------------
SQLRETURN OdbcManager::AutoCommitEnable(bool flg)
{
	SQLRETURN retc;
	if(flg) {
///		retc = SQLSetConnectOption(m_hdbc, SQL_AUTOCOMMIT, SQL_AUTOCOMMIT_ON);
		retc = SQLSetConnectAttr(m_hdbc, SQL_ATTR_AUTOCOMMIT, (SQLPOINTER)SQL_AUTOCOMMIT_ON, SQL_IS_INTEGER);
		if( SQL_SUCCESS != retc ) GetErrMsg();
		return retc;
	} else {
///		retc = SQLSetConnectOption(m_hdbc, SQL_AUTOCOMMIT, SQL_AUTOCOMMIT_OFF);	
		retc = SQLSetConnectAttr(m_hdbc, SQL_ATTR_AUTOCOMMIT, (SQLPOINTER)SQL_AUTOCOMMIT_OFF, SQL_IS_INTEGER);
		if( SQL_SUCCESS != retc ) GetErrMsg();
		return retc;
	}
}

//------------------------------------------
// SQL文実行
// LPCSTR cSql SQL文
// 戻り値 SQLRETURN (SQL_ERROR,SQL_INVALID_HANDLE)の場合は異常?
//------------------------------------------
SQLRETURN OdbcManager::ExecuteDirect(LPCSTR cSql)
{
	if(m_hstmt) { SQLFreeHandle(SQL_HANDLE_STMT, m_hstmt); m_hstmt = NULL; }

	SQLRETURN sqlrc = SQLAllocHandle(SQL_HANDLE_STMT, m_hdbc, &m_hstmt);
	if (sqlrc != SQL_SUCCESS) GetErrMsg(SQL_HANDLE_STMT);
	m_nColumnIndexCheck = 0;		// GetDataのindexをクリア	

	return SQLExecDirect(m_hstmt, (SQLCHAR *)cSql, SQL_NTS);
}

//------------------------------------------
// 非同期SQL文実行
// LPCSTR cSql SQL文
// bool bFlg	true 初回		false２回目以降
// 戻り値 SQLRETURN (SQL_ERROR,SQL_INVALID_HANDLE)の場合は異常?
//------------------------------------------
SQLRETURN OdbcManager::ExecuteDirectAsync(LPCSTR cSql, bool bFlg)
{
	if(bFlg) {

		if(m_hstmt) { SQLFreeHandle(SQL_HANDLE_STMT, m_hstmt); m_hstmt = NULL; }

		SQLRETURN sqlrc = SQLAllocHandle(SQL_HANDLE_STMT, m_hdbc, &m_hstmt);
		if (sqlrc != SQL_SUCCESS) GetErrMsg(SQL_HANDLE_STMT);
		m_nColumnIndexCheck = 0;		// GetDataのindexをクリア	

		// 非同期有効
		sqlrc = SQLSetStmtAttr( m_hstmt, SQL_ATTR_ASYNC_ENABLE, (SQLPOINTER) SQL_ASYNC_ENABLE_ON, 0);
		if (sqlrc != SQL_SUCCESS) GetErrMsg(SQL_HANDLE_STMT);
	}

	return SQLExecDirect(m_hstmt, (SQLCHAR *)cSql, SQL_NTS);
}

//------------------------------------------
// 非同期SQL文実行の明示的なキャンセル
// 戻り値 SQLRETURN (SQL_ERROR,SQL_INVALID_HANDLE)の場合は異常?
//------------------------------------------
SQLRETURN OdbcManager::ExecuteDirectAsyncCancel()
{
	return SQLCancel(m_hstmt);
}

//------------------------------------------
// フェッチ
//------------------------------------------
SQLRETURN OdbcManager::Fetch()
{
	m_nColumnIndexCheck = 0;		// GetDataのindexをクリア	
	return SQLFetch(m_hstmt); 
}

//------------------------------------------
// パラメータ付きSQL文セット
// LPCSTR cSql ? 付きのSQL文
// 戻り値 SQLRETURN (SQL_ERROR,SQL_INVALID_HANDLE)の場合は異常?
//------------------------------------------
SQLRETURN OdbcManager::Prepare(LPCSTR cSql)
{
	if(m_hstmt) { SQLFreeHandle(SQL_HANDLE_STMT, m_hstmt); m_hstmt = NULL; }

	SQLAllocHandle(SQL_HANDLE_STMT, m_hdbc, &m_hstmt);

	return SQLPrepare(m_hstmt, (SQLCHAR *)cSql, SQL_NTS);
}

//------------------------------------------
// バインド
// WORD ipar バインドNo (1オリジン)
// DWORD cbColDef バインドデータサイズ
// LPVOID rgbValue データ (out)
// 戻り値 SQLRETURN (SQL_ERROR,SQL_INVALID_HANDLE)の場合は異常?
//------------------------------------------
//SQLRETURN OdbcManager::BindParameter(WORD ipar, DWORD cbColDef, LPVOID rgbValue)
SQLRETURN OdbcManager::BindParameter(SQLSMALLINT ipar, SQLUINTEGER cbColDef, SQLLEN* rgbValue)
{
	//return SQLBindParameter(m_hstmt, ipar, SQL_PARAM_INPUT, SQL_C_BINARY, SQL_LONGVARBINARY, cbColDef, 0, (PTR)ipar, 0, (long*)rgbValue);
	return SQLBindParameter(m_hstmt, ipar, SQL_PARAM_INPUT, SQL_C_BINARY, SQL_LONGVARBINARY, cbColDef, 0, (SQLPOINTER)ipar, 0, rgbValue);
}
//------------------------------------------
// パラメータ番号取得
// PTR* pVal トークン
// 戻り値 SQLRETURN SQLNEEDDATA(登録前), SQLSUCCESS(登録後) 時 成功
//------------------------------------------
//SQLRETURN OdbcManager::ParamData(PTR* pVal)
SQLRETURN OdbcManager::ParamData(SQLPOINTER* pVal)
{
#ifndef true
	return SQLParamData(m_hstmt, pVal);
#else
	SQLRETURN a =SQLParamData(m_hstmt, pVal);
		UCHAR errmsg[256];
		SQLError(0, m_hdbc, m_hstmt, NULL, NULL, errmsg, sizeof(errmsg), NULL );
		_RPT0(_CRT_WARN, errmsg);
		_RPT0(_CRT_WARN, "\n");
	return a;
#endif
}
//------------------------------------------
// パラメータデータをDBへ送信
// LPVOID pVal バイナリデータポインタ
// DWORD size データサイズ
// 戻り値 SQLRETURN (SQL_ERROR,SQL_INVALID_HANDLE)の場合は異常?
//------------------------------------------
SQLRETURN OdbcManager::PutData(SQLPOINTER pVal, SQLINTEGER size)
{
#ifndef true
	return SQLPutData(m_hstmt, pVal, size);
#else
	SQLRETURN a =SQLPutData(m_hstmt, pVal, size);
	if(a == SQL_ERROR) {
		UCHAR errmsg[256];
		UCHAR errmsg1[256];
		SQLINTEGER    i;
		SQLError(0, m_hdbc, m_hstmt, errmsg1, &i, errmsg, sizeof(errmsg), NULL );
		_RPT0(_CRT_WARN, errmsg);
		_RPT0(_CRT_WARN, "\n");
	}
	return a;
#endif
}

//------------------------------------------
// データ取得
// WORD n カラム位置 (1オリジン)
// LPVOID pVal バッファポインタ
// DWORD size バッファサイズ
// 戻り値 取得文字数
//------------------------------------------
// 文字
long OdbcManager::GetData(WORD n, LPVOID pVal, DWORD size )
{
	_ASSERT( m_nColumnIndexCheck < n );
	m_nColumnIndexCheck = n;

	SQLLEN inum;
	SQLRETURN sqlrc = SQLGetData(m_hstmt, n, SQL_C_CHAR, pVal, size, &inum);
	if(sqlrc != SQL_SUCCESS) GetErrMsg(SQL_HANDLE_STMT);
	return (long)inum;
}
// 整数
long OdbcManager::GetData(WORD n, short *pVal )
{
	_ASSERT( m_nColumnIndexCheck < n );
	m_nColumnIndexCheck = n;

	SQLLEN inum;
	SQLRETURN sqlrc = SQLGetData(m_hstmt, n, SQL_C_SHORT, pVal, 2, &inum);
	if(sqlrc != SQL_SUCCESS) GetErrMsg(SQL_HANDLE_STMT);
	return (long)inum;
}
long OdbcManager::GetData(WORD n, int *pVal )
{
	_ASSERT( m_nColumnIndexCheck < n );
	m_nColumnIndexCheck = n;

	SQLLEN inum;
	SQLRETURN sqlrc = SQLGetData(m_hstmt, n, SQL_C_LONG, pVal, 4, &inum);
	if(sqlrc != SQL_SUCCESS) GetErrMsg(SQL_HANDLE_STMT);
	return (long)inum;
}
long OdbcManager::GetData(WORD n, long *pVal )
{
	_ASSERT( m_nColumnIndexCheck < n );
	m_nColumnIndexCheck = n;

	SQLLEN inum;
	SQLRETURN sqlrc = SQLGetData(m_hstmt, n, SQL_C_LONG, pVal, 4, &inum);
	if(sqlrc != SQL_SUCCESS) GetErrMsg(SQL_HANDLE_STMT);
	return (long)inum;
}
// 実数
long OdbcManager::GetData(WORD n, float *pVal)
{
	_ASSERT( m_nColumnIndexCheck < n );
	m_nColumnIndexCheck = n;

	SQLLEN inum;
	SQLRETURN sqlrc = SQLGetData(m_hstmt, n, SQL_C_FLOAT, pVal, 4, &inum);
	if(sqlrc != SQL_SUCCESS) GetErrMsg(SQL_HANDLE_STMT);
	return (long)inum;
}
long OdbcManager::GetData(WORD n, double *pVal)
{
	_ASSERT( m_nColumnIndexCheck < n );
	m_nColumnIndexCheck = n;

	SQLLEN inum;
	SQLRETURN sqlrc = SQLGetData(m_hstmt, n, SQL_C_DOUBLE, pVal, 8, &inum);
	if(sqlrc != SQL_SUCCESS) GetErrMsg(SQL_HANDLE_STMT);
	return (long)inum;
}
//------------------------------------------
// 日付型
// WORD n カラム位置 (0オリジン)
//------------------------------------------
// int[3] 年月日
long OdbcManager::GetDataDate(WORD n, int *pVal)
{
	_ASSERT( m_nColumnIndexCheck < n );
	m_nColumnIndexCheck = n;

	SQLLEN inum;
	SQL_DATE_STRUCT  d;
	SQLRETURN sqlrc = SQLGetData(m_hstmt, n, SQL_C_DATE, &d, 0, &inum);
	if(sqlrc != SQL_SUCCESS) GetErrMsg(SQL_HANDLE_STMT);
	pVal[0] = d.year;
	pVal[1] = d.month;
	pVal[2] = d.day;
	return (long)inum;
}
// int[6] 年月日時分秒
long OdbcManager::GetDataDateTime(WORD n, int *pVal)
{
	_ASSERT( m_nColumnIndexCheck < n );
	m_nColumnIndexCheck = n;

	SQLLEN inum;
	SQL_TIMESTAMP_STRUCT  d;
	SQLRETURN sqlrc = SQLGetData(m_hstmt, n, SQL_C_TIMESTAMP, &d, 0, &inum);
	if(sqlrc != SQL_SUCCESS) GetErrMsg(SQL_HANDLE_STMT);
	pVal[0] = d.year;
	pVal[1] = d.month;
	pVal[2] = d.day;
	pVal[3] = d.hour;
	pVal[4] = d.minute;
	pVal[5] = d.second;
	return (long)inum;
}

//------------------------------------------
// バイナリデータ取得
// WORD n カラム位置 (0オリジン)
// LPVOID pVal バッファポインタ
// DWORD size バッファサイズ
// 戻り値 取得バイト数
//------------------------------------------
long OdbcManager::GetDataImage(WORD n, LPVOID pVal, DWORD size )
{
	_ASSERT( m_nColumnIndexCheck < n );
	m_nColumnIndexCheck = n;

	SQLLEN inum;
	SQLRETURN sqlrc = SQLGetData(m_hstmt, n, SQL_C_BINARY, pVal, size, &inum);
	if(sqlrc != SQL_SUCCESS) GetErrMsg(SQL_HANDLE_STMT);
	return (long)inum;
}


//------------------------------------------
// 異常発生時の詳細メッセージ取得
//------------------------------------------
void OdbcManager::GetErrMsg()
{
	memset(g_cLastErrMsgMain, 0x00, sizeof(g_cLastErrMsgMain));
	memset(g_cLastErrMsgSub, 0x00, sizeof(g_cLastErrMsgSub));
	g_nLastErrMsgNo = 0;
	SQLSMALLINT retc;

	if (m_hstmt && m_hdbc) {
		// SQL2005になってから頻繁に接続エラーになった為の調査
		//SQLError(0, m_hdbc, m_hstmt, NULL, NULL, errmsg, sizeof(errmsg), NULL );
		SQLError(0, m_hdbc, m_hstmt, g_cLastErrMsgSub, &g_nLastErrMsgNo, g_cLastErrMsgMain, (SQLSMALLINT)sizeof(g_cLastErrMsgMain), &retc );

		_RPT0(_CRT_WARN, g_cLastErrMsgMain);
		_RPT0(_CRT_WARN, "\n");
	}
}

void OdbcManager::GetErrMsg(SQLSMALLINT iType)
{
	memset(g_cLastErrMsgMain, 0x00, sizeof(g_cLastErrMsgMain));
	memset(g_cLastErrMsgSub,  0x00, sizeof(g_cLastErrMsgSub));
	g_nLastErrMsgNo = 0;
	SQLSMALLINT retc;

	if( SQL_HANDLE_ENV == iType ) {
		SQLGetDiagRec(iType, OdbcEnv::GetHandle(), 1, g_cLastErrMsgSub, &g_nLastErrMsgNo, g_cLastErrMsgMain, (SQLSMALLINT)sizeof(g_cLastErrMsgMain), &retc );
	} else if ( SQL_HANDLE_DBC == iType ) {
		SQLGetDiagRec(iType, m_hdbc, 1, g_cLastErrMsgSub, &g_nLastErrMsgNo, g_cLastErrMsgMain, (SQLSMALLINT)sizeof(g_cLastErrMsgMain), &retc );
	} else {
		SQLGetDiagRec(iType, m_hstmt, 1, g_cLastErrMsgSub, &g_nLastErrMsgNo, g_cLastErrMsgMain, (SQLSMALLINT)sizeof(g_cLastErrMsgMain), &retc );
	}
	
	_RPT0(_CRT_WARN, g_cLastErrMsgSub);
	_RPT0(_CRT_WARN, g_cLastErrMsgMain);
	_RPT0(_CRT_WARN, "\n");

}