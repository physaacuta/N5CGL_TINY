#include "StdAfx.h"
#include "PreDBSelectManager.h"

// メモリーリーク検出 ----->>>
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif
// <<<------------------------

//----------------------------------------------------------
// コンストラクタ
//----------------------------------------------------------
//	[IN]	CString	strTaskSection	自タスクセクション
//	[IN]	CString	strRetrySection	リトライセクション
//	[IN]	CString	strSqlKey		実績検索SQLキー
//----------------------------------------------------------
PreDBSelectManager::PreDBSelectManager(CString strTaskSection, CString strRetrySection, CString strSqlKey)
{
	char	cTmp[128];

	// 初期化
	m_strTaskSection	= strTaskSection;					// 自タスク	セクション[KizuTask.ini]
	m_strRetrySection	= strRetrySection;					// リトライセクション[KizuTask.ini]
	m_strSqlKey			= strSqlKey;						// 実績検索SQLキー

	mcls_pLog		= NULL;									// ログ管理クラス
	m_nSelectMode	= NON_DB;								// 接続サーバー選択モード

	// 前工程蓄積サーバー選択状態
	m_typStatusPre.nPing		= SELECT_NON;
	m_typStatusPre.nConnect		= SELECT_NON;
	m_typStatusPre.nSearchCoil	= SELECT_NON;

	// 統合サーバー選択状態
	m_typStatusShare.nPing			= SELECT_NON;
	m_typStatusShare.nConnect		= SELECT_NON;
	m_typStatusShare.nSearchCoil	= SELECT_NON;

	//----------------------------------
	// 前工程蓄積サーバー
	//----------------------------------
	// PINGリトライ回数
	m_typPingRetryPre.nCount	= GetPrivateProfileInt(m_strRetrySection, "PING_RETRY_COUNT_PRE", 3, TASKINI_NAME);
	// PINGリトライ待ち時間[ms]
	m_typPingRetryPre.nTime		= GetPrivateProfileInt(m_strRetrySection, "PING_RETRY_TIME_PRE", 1000, TASKINI_NAME);
	// DB接続リトライ回数
	m_typConnectRetryPre.nCount	= GetPrivateProfileInt(m_strRetrySection, "CONNECT_RETRY_COUNT_PRE", 1, TASKINI_NAME);
	// DB接続リトライ待ち時間[ms]
	m_typConnectRetryPre.nTime	= GetPrivateProfileInt(m_strRetrySection, "CONNECT_RETRY_TIME_PRE", 1000, TASKINI_NAME);

	//----------------------------------
	// 統合サーバー
	//----------------------------------
	// PINGリトライ回数
	m_typPingRetryShare.nCount		= GetPrivateProfileInt(m_strRetrySection, "PING_RETRY_COUNT_SHARE", 3, TASKINI_NAME);
	// PINGリトライ待ち時間[ms]
	m_typPingRetryShare.nTime		= GetPrivateProfileInt(m_strRetrySection, "PING_RETRY_TIME_SHARE", 1000, TASKINI_NAME);
	// DB接続リトライ回数
	m_typConnectRetryShare.nCount	= GetPrivateProfileInt(m_strRetrySection, "CONNECT_RETRY_COUNT_SHARE", 1, TASKINI_NAME);
	// DB接続リトライ待ち時間[ms]
	m_typConnectRetryShare.nTime	= GetPrivateProfileInt(m_strRetrySection, "CONNECT_RETRY_TIME_SHARE", 1000, TASKINI_NAME);

	// 実績検索SQLフォーマット
	m_strSqlFmt.Empty();
	GetPrivateProfileString(m_strTaskSection, m_strSqlKey, "", cTmp, sizeof(cTmp), TASKINI_NAME);
	if( strlen(cTmp) > 0 ){				// SQL
		m_strSqlFmt	= cTmp;
	}
}

//----------------------------------------------------------
// デストラクタ
//----------------------------------------------------------
PreDBSelectManager::~PreDBSelectManager(void)
{
}

//----------------------------------------------------------
// 接続サーバー選択状態取得
//----------------------------------------------------------
//	[IN]	EM_SELECT_DB		nSelectMode	DB選択モード
//	[OUT]	TYP_SELECT_STATUS*	typStatus	選択状態
//----------------------------------------------------------
void PreDBSelectManager::GetSelectStatus(EM_SELECT_DB nSelectMode, TYP_SELECT_STATUS* typStatus)
{
	// 初期化
	typStatus->nPing		= SELECT_NON;
	typStatus->nConnect		= SELECT_NON;
	typStatus->nSearchCoil	= SELECT_NON;

	switch( nSelectMode ){
		case PRE_DB:
			typStatus->nPing		= m_typStatusPre.nPing;
			typStatus->nConnect		= m_typStatusPre.nConnect;
			typStatus->nSearchCoil	= m_typStatusPre.nSearchCoil;
			break;
		case SHARE_DB:
			typStatus->nPing		= m_typStatusShare.nPing;
			typStatus->nConnect		= m_typStatusShare.nConnect;
			typStatus->nSearchCoil	= m_typStatusShare.nSearchCoil;
			break;
	}
}

//----------------------------------------------------------
// 前工程接続DB選択
//----------------------------------------------------------
//	[IN]	char*		cPreKizukenNo	前工程疵検管理№
//	[OUT]	OdbcBase*	pConnectDB		接続DB
//----------------------------------------------------------
//	戻り値	bool
//			true	正常
//			false	異常
//----------------------------------------------------------
bool PreDBSelectManager::SelectConnectDB(char* cPreKizukenNo, OdbcBase* pConnectDB)
{
	bool	bRet;

	// 前工程接続DB選択
	bRet	= SelectConnectDB(m_strDBSectionPre, m_strDBSectionShare, cPreKizukenNo, pConnectDB);

	return bRet;
}

//----------------------------------------------------------
// 前工程接続DB選択
//----------------------------------------------------------
//	[IN]	CString		strPreDB,		前工程蓄積サーバーDBセクション
//	[IN]	CString		strShareDB,		統合サーバーDBセクション
//	[IN]	char*		cPreKizukenNo	前工程疵検管理№
//	[OUT]	OdbcBase*	pConnectDB		接続DB
//----------------------------------------------------------
//	戻り値	bool
//			true	正常
//			false	異常
//----------------------------------------------------------
bool PreDBSelectManager::SelectConnectDB(CString strPreDB, CString strShareDB, char* cPreKizukenNo, OdbcBase* pConnectDB)
{
	bool	bRet;

    // 2014.07.25 川原  所間連携    >>>>>>>>
	//CString	strPreKizukenNo(cPreKizukenNo, SIZE_KIZUKEN_NO);

    // 初期化
	m_nSelectMode	= NON_DB;			// 未選択

	// 前工程蓄積サーバー接続
	bRet	= ConnectPreServer(strPreDB, cPreKizukenNo, pConnectDB);
	if( ! bRet ){
		// 統合サーバー接続
		bRet	= ConnectShareServer(strShareDB, pConnectDB);
	}
    // 2014.07.25 川原  所間連携    <<<<<<<<

	return bRet;
}

//----------------------------------------------------------
// 選択サーバーDB接続
//----------------------------------------------------------
//	[OUT]	OdbcBase*	pConnectDB	接続DB
//----------------------------------------------------------
//	戻り値	bool
//			true	正常
//			false	異常
//----------------------------------------------------------
bool PreDBSelectManager::ConnectSelectDB( OdbcBase* pConnectDB)
{
	bool	bRet;

	// 選択サーバー判定
	switch( m_nSelectMode ){
		case PRE_DB:	// 前工程蓄積DB
			bRet	= ConnectPreDB(pConnectDB);
			break;
		case SHARE_DB:	// 統合DB
			bRet	= ConnectShareDB(pConnectDB);
			break;
		default:		// 未選択
			bRet	= false;
			break;
	}

	return bRet;
}

//----------------------------------------------------------
// 前工程蓄積サーバーDB接続
//----------------------------------------------------------
//	[OUT]	OdbcBase*	pConnectDB	接続DB
//----------------------------------------------------------
//	戻り値	bool
//			true	正常
//			false	異常
//----------------------------------------------------------
bool PreDBSelectManager::ConnectPreDB(OdbcBase* pConnectDB)
{
	bool	bRet;

	// DB接続
	bRet	= ConnectDB(m_strDBSectionPre, &m_typConnectRetryPre, pConnectDB);

	return bRet;
}

//----------------------------------------------------------
// 統合サーバーDB接続
//----------------------------------------------------------
//	[OUT]	OdbcBase*	pConnectDB	接続DB
//----------------------------------------------------------
//	戻り値	bool
//			true	正常
//			false	異常
//----------------------------------------------------------
bool PreDBSelectManager::ConnectShareDB(OdbcBase* pConnectDB)
{
	bool	bRet;

	// DB接続
	bRet	= ConnectDB(m_strDBSectionShare, &m_typConnectRetryShare, pConnectDB);

	return bRet;
}

//----------------------------------------------------------
// 前工程蓄積サーバーDB接続チェック
//----------------------------------------------------------
//	戻り値	bool
//			true	正常
//			false	異常
//----------------------------------------------------------
bool PreDBSelectManager::CheckPreDB(void)
{
	bool	bRet;

	// サーバーDB接続チェック
	bRet	= CheckConnectDB(m_strDBSectionPre, PRE_DB);

	return bRet;
}

//----------------------------------------------------------
// 統合サーバーDB接続チェック
//----------------------------------------------------------
//	戻り値	bool
//			true	正常
//			false	異常
//----------------------------------------------------------
bool PreDBSelectManager::CheckShareDB(void)
{
	bool	bRet;

	// サーバーDB接続チェック
	bRet	= CheckConnectDB(m_strDBSectionShare, SHARE_DB);

	return bRet;
}

//----------------------------------------------------------
// DB接続チェック
//----------------------------------------------------------
//	[IN]	CString			strDBSection	DBセクション
//	[IN]	EM_SELECT_DB	nSelect			選択モード
//----------------------------------------------------------
//	戻り値	bool
//			true	正常
//			false	異常
//----------------------------------------------------------
bool PreDBSelectManager::CheckConnectDB(CString strDBSection, EM_SELECT_DB nSelect)
{
	bool		bRet;

	TYP_RETRY_INF	typPingRetry;
	TYP_RETRY_INF	typConnectRetry;

	OdbcBase	clsDB;

	// 初期化
	typPingRetry.nCount		= 1;
	typPingRetry.nTime		= 1000;
	typConnectRetry.nCount	= 1;
	typConnectRetry.nTime	= 1000;

	// リトライ設定
	switch( nSelect ){
		case PRE_DB:
			typPingRetry	= m_typPingRetryPre;
			typConnectRetry	= m_typConnectRetryPre;
			break;
		case SHARE_DB:
			typPingRetry	= m_typPingRetryShare;
			typConnectRetry	= m_typConnectRetryShare;
			break;
	}

	// Pingサーバー
	bRet	= PingServer(strDBSection, &typPingRetry);
	if( bRet ){
		// リトライ分
		for(int ii = 0; ii < typConnectRetry.nCount; ii++){
			// DB接続チェック
			bRet	= clsDB.OpenCheck(TASKINI_NAME,strDBSection);
			if( bRet ){						// 接続OK
				break;
			} else{							// 接続NG
				Sleep(typConnectRetry.nTime);
			}
		}
	}

	return bRet;
}

//----------------------------------------------------------
// Pingサーバー
//----------------------------------------------------------
//	[IN]	CString			strDBSection	DBセクション
//	[IN]	TYP_RETRY_INF*	typRetry		リトライ情報
//----------------------------------------------------------
//	戻り値	bool
//			true	正常
//			false	異常
//----------------------------------------------------------
bool PreDBSelectManager::PingServer(CString strDBSection, TYP_RETRY_INF* typRetry)
{
	bool	bRet;

	char	cTmp[128];

	CString	strIP;

	// 初期化
	bRet	= false;

	// IPアドレス取得
	GetPrivateProfileString(strDBSection, "IPADDLESS", "", cTmp, sizeof(cTmp), TASKINI_NAME);
	if( strlen(cTmp) > 0 ){				// IPアドレス有り
		strIP	= cTmp;
		// リトライ分
		for(int ii = 0; ii < typRetry->nCount; ii++){
			// Ping発行
			bRet	= mcls_Ping.Ping(strIP, typRetry->nTime);
			if( bRet ){					// Ping到達
				break;
			}
		}
	}

	if( ! bRet ){
		LOG(em_ERR), "[PreDBSelectManager] Pingエラー [%s]", strDBSection);
	}

	return bRet;
}

//----------------------------------------------------------
// DB接続
//----------------------------------------------------------
//	[IN]	CString			strDBSection	DBセクション
//	[IN]	TYP_RETRY_INF*	typRetry		リトライ情報
//	[OUT]	OdbcBase*		pConnectDB		接続DB
//----------------------------------------------------------
//	戻り値	bool
//			true	正常
//			false	異常
//----------------------------------------------------------
bool PreDBSelectManager::ConnectDB(CString strDBSection, TYP_RETRY_INF*	typRetry, OdbcBase* pConnectDB)
{
	bool	bRet;

	// 初期化
	bRet	= false;

	// リトライ分
	for(int ii = 0; ii < typRetry->nCount; ii++){
		// DB接続
		bRet	= pConnectDB->Connect(TASKINI_NAME, strDBSection);
		if( bRet ){						// 接続OK
			break;
		} else{							// 接続NG
			Sleep(typRetry->nTime);
		}
	}

	if( ! bRet ){
		LOG(em_ERR), "[PreDBSelectManager] DB接続エラー [%s]", strDBSection);
	}

	return bRet;
}

//----------------------------------------------------------
// コイル実績検索
//----------------------------------------------------------
//	[IN]	CString		strPreKizukenNo	前工程疵検管理№
//	[IN]	OdbcBase*	pConnectDB		接続DB
//----------------------------------------------------------
//	戻り値	bool
//			true	正常
//			false	異常
//----------------------------------------------------------
bool PreDBSelectManager::SearchCoilResult(CString strPreKizukenNo, OdbcBase* pConnectDB)
{
	bool	bRet;

	int		nCnt;

	CString	strTmp;
	CString	strSQL;

	// 初期化
	bRet	= false;
	nCnt	= 0;

	// SQL作成
	if( ! m_strSqlFmt.IsEmpty() ){
		strSQL.Format(m_strSqlFmt,strPreKizukenNo);
		// SQL実行
		bRet	= pConnectDB->GetSelectKey(strSQL, &nCnt);
		if( bRet ){
			if( nCnt > 0 ){
				bRet	= true;
			} else{
				bRet	= false;
			}
		}
	}

	if( ! bRet ){
		LOG(em_ERR), "[PreDBSelectManager] 実績検索エラー [%s]", strSQL);
	}

	return bRet;
}

//----------------------------------------------------------
// 前工程蓄積サーバー接続
//----------------------------------------------------------
//	[IN]	CString		strDBSection,	DBセクション
//	[IN]	char*		cPreKizukenNo   前工程疵検管理№
//	[OUT]	OdbcBase*	pConnectDB		接続DB
//----------------------------------------------------------
//	戻り値	bool
//			true	正常
//			false	異常
//----------------------------------------------------------
bool PreDBSelectManager::ConnectPreServer(CString strDBSection, char* cPreKizukenNo, OdbcBase* pConnectDB)
{
	bool	bRet;

    // 2014.07.25 川原  所間連携    >>>>>>>>
	CString	strPreKizukenNo(cPreKizukenNo, SIZE_KIZUKEN_NO);
    // 2014.07.25 川原  所間連携    <<<<<<<<

    // 初期化
	m_typStatusPre.nPing		= SELECT_NON;
	m_typStatusPre.nConnect		= SELECT_NON;
	m_typStatusPre.nSearchCoil	= SELECT_NON;

    // Pingサーバー
	bRet	= PingServer(strDBSection, &m_typPingRetryPre);
	if( bRet ){
		m_typStatusPre.nPing	= SELECT_OK;
		// DB接続
		bRet	= ConnectDB(strDBSection, &m_typConnectRetryPre, pConnectDB);
		if( bRet ){
			m_typStatusPre.nConnect	= SELECT_OK;
			// コイル実績チェック
			bRet	= SearchCoilResult(strPreKizukenNo, pConnectDB);
			if( bRet ){
				m_typStatusPre.nSearchCoil	= SELECT_OK;
			} else{
				m_typStatusPre.nSearchCoil	= SELECT_NG;
				pConnectDB->DisConnect();					// DB切断
			}
		} else{
			m_typStatusPre.nConnect	= SELECT_NG;
		}
	} else{
		m_typStatusPre.nPing	= SELECT_NG;
	}

    // 2014.07.25 川原  所間連携    >>>>>>>>
    // 接続モード
    if( bRet ){
        m_nSelectMode	= PRE_DB;		// 前工程蓄積DB
    }else{
        m_nSelectMode	= NON_DB;		// 未選択
    }
    // 2014.07.25 川原  所間連携    <<<<<<<<

	return bRet;
}

//----------------------------------------------------------
// 統合サーバー接続
//----------------------------------------------------------
//	[IN]	CString		strDBSection	DBセクション
//	[OUT]	OdbcBase*	pConnectDB		接続DB
//----------------------------------------------------------
//	戻り値	bool
//			true	正常
//			false	異常
//----------------------------------------------------------
bool PreDBSelectManager::ConnectShareServer(CString strDBSection, OdbcBase* pConnectDB)
{
	bool	bRet;

	// 初期化
	m_typStatusShare.nPing			= SELECT_NON;
	m_typStatusShare.nConnect		= SELECT_NON;
	m_typStatusShare.nSearchCoil	= SELECT_NON;

	// Pingサーバー
	bRet	= PingServer(strDBSection, &m_typPingRetryShare);
	if( bRet ){
		m_typStatusShare.nPing	= SELECT_OK;
		// DB接続
		bRet	= ConnectDB(strDBSection, &m_typConnectRetryShare, pConnectDB);
		if( bRet ){
			m_typStatusShare.nConnect	= SELECT_OK;
		} else{
			m_typStatusShare.nConnect	= SELECT_NG;
		}
	} else{
		m_typStatusShare.nPing	= SELECT_NG;
	}

    // 2014.07.25 川原  所間連携    >>>>>>>>
    // 接続モード
    if( bRet ){
        m_nSelectMode	= SHARE_DB;	// 統合DB
    }else{
        m_nSelectMode	= NON_DB;	    // 未選択
    }
    // 2014.07.25 川原  所間連携    <<<<<<<<

	return bRet;
}
