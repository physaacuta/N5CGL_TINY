#include "StdAfx.h"
#include "BCPManager.h"

using namespace KizuODBC;

//////////////////////////////////////////////////////////////////////
// 構築/消滅
//////////////////////////////////////////////////////////////////////
//------------------------------------------
// コンストラクタ
//------------------------------------------
BCPManager::BCPManager(void) : OdbcManager() 
{
}
//------------------------------------------
// デストラクタ
//------------------------------------------
BCPManager::~BCPManager(void)
{
}
//------------------------------------------
// デフォルトセッションでの接続
// LPCSTR cIniPath iniファイルパス
// 戻り値 -:異常 1:接続済み 0:正常接続
//------------------------------------------
int BCPManager::Connect(LPCSTR cIniPath)
{
	return Connect(cIniPath, KIZUODBC_ODBCMANAGER_SESSION);
}
//------------------------------------------
// セッション指定での接続
// LPCSTR cIniPath iniファイルパス
// LPCSTR cSession セッション
// 戻り値 -:異常 1:接続済み 0:正常接続
//------------------------------------------
int BCPManager::Connect(LPCSTR cIniPath, LPCSTR cSession)
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
// セッション指定での接続
// LPCSTR cIniPath iniファイルパス
// LPCSTR cSession セッション
// 戻り値 -:異常 1:接続済み 0:正常接続
//------------------------------------------
int BCPManager::Connect(char const* dsn, char const* uid, char const* pwd)
{
	SQLRETURN sqlrc;

	// 既に接続済み？
	if( m_bIsConnect ) return 1;

	OdbcLockedEnv env;						// クリティカルセクション (ODBCはどうやらマルチスレッドでハンドルを生成したらハンドルリークしてそう)

	// BCP接続有効化 これがBCP系 -------------------->>>>>>>>>>
	sqlrc = SQLSetConnectAttr(m_hdbc, SQL_COPT_SS_BCP, (SQLPOINTER)SQL_BCP_ON, SQL_IS_INTEGER);
	if (sqlrc == SQL_SUCCESS_WITH_INFO) {m_nLastErrNo = -106;	GetErrMsg(SQL_HANDLE_DBC);goto NG;}
	if (sqlrc != SQL_SUCCESS)			{m_nLastErrNo = -6;		GetErrMsg(SQL_HANDLE_DBC);goto NG;}
	// <<<<<< -------------------------------------------------
	
	// 接続
	sqlrc = SQLConnect(m_hdbc, (SQLCHAR*)dsn, SQL_NTS, (SQLCHAR*)uid, SQL_NTS, (SQLCHAR*)pwd, SQL_NTS);
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