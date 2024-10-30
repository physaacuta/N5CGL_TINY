#include "StdAfx.h"
#include "OdbcEnv.h"

#include <crtdbg.h> // デバック用

using namespace KizuODBC;

//////////////////////////////////////////////////////////////////////
// OdbcEnv のスタティック変数初期化
//////////////////////////////////////////////////////////////////////
OdbcEnv			OdbcEnv::m_inst;					// スタティック変数初期化


//////////////////////////////////////////////////////////////////////
// OdbcEnv
//////////////////////////////////////////////////////////////////////
//------------------------------------------
// コンストラクタ
//------------------------------------------
OdbcEnv::OdbcEnv(void) :
m_henv(NULL)
{
	// 初期化
	memset(g_cLastErrMsgMain, 0x00, sizeof(g_cLastErrMsgMain));
	memset(g_cLastErrMsgSub, 0x00, sizeof(g_cLastErrMsgSub));
	g_nLastErrMsgNo = 0;

	// クリティカルセクション 初期化
	InitializeCriticalSection(&m_cs);
	
	// 環境ハンドル 生成
    SQLRETURN sqlrc = SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &m_henv);
    if( SQL_SUCCESS != sqlrc )
    {
		_RPT0(_CRT_WARN, "SQL_HANDLE_ENV ハンドル生成エラー\n");
        m_henv = NULL;
		return;
    }

	// 環境属性の設定 (ODBCドライババージョン指定 3.x)
	sqlrc = SQLSetEnvAttr(m_henv, SQL_ATTR_ODBC_VERSION, (SQLPOINTER)SQL_OV_ODBC3, SQL_IS_INTEGER);
	if (sqlrc != SQL_SUCCESS && sqlrc != SQL_SUCCESS_WITH_INFO) { 
		GetErrMsg();
		SQLFreeHandle(SQL_HANDLE_ENV, m_henv);
		m_henv = NULL;
		return;
	}
}

//------------------------------------------
// デストラクタ
//------------------------------------------
OdbcEnv::~OdbcEnv(void)
{
	// 解放
	if( NULL != m_henv ) {
		SQLFreeHandle(SQL_HANDLE_ENV, m_henv);
		m_henv = NULL;
	}
	DeleteCriticalSection(&m_cs);
}


//------------------------------------------
// 異常発生時の詳細メッセージ取得
//------------------------------------------
void OdbcEnv::GetErrMsg()
{
	memset(g_cLastErrMsgMain, 0x00, sizeof(g_cLastErrMsgMain));
	memset(g_cLastErrMsgSub,  0x00, sizeof(g_cLastErrMsgSub));
	g_nLastErrMsgNo = 0;
	SQLSMALLINT retc;

	SQLGetDiagRec(SQL_HANDLE_ENV, m_henv, 1, g_cLastErrMsgSub, &g_nLastErrMsgNo, g_cLastErrMsgMain, (SQLSMALLINT)sizeof(g_cLastErrMsgMain), &retc );
	
	_RPT0(_CRT_WARN, g_cLastErrMsgSub);
	_RPT0(_CRT_WARN, g_cLastErrMsgMain);
	_RPT0(_CRT_WARN, "\n");
}



//////////////////////////////////////////////////////////////////////
// OdbcLockedEnv
//////////////////////////////////////////////////////////////////////
//------------------------------------------
// コンストラクタでlock
// BOOL bWait true:クリティカルセクション待ちあり
//			  false:クリティカルセクション待ち無し。(疵検ではほぼ使用しない)
//------------------------------------------
//OdbcLockedEnv::OdbcLockedEnv(BOOL bWait)
OdbcLockedEnv::OdbcLockedEnv()
{
//	if(bWait) {
//		// ロック (クリティカルセクション突入)
//		OdbcEnv::Lock();
//		m_bCsLocked = TRUE;
//
//	} else {
//		// NG時は、即時戻る
//		m_bCsLocked = OdbcEnv::TryLock();
//	}

	// ロック (クリティカルセクション突入)
	OdbcEnv::Lock();
	m_bCsLocked = TRUE;
}

//------------------------------------------
// デストラクタでunlock
//------------------------------------------
OdbcLockedEnv::~OdbcLockedEnv()
{
	if( m_bCsLocked ) OdbcEnv::Unlock();
}


