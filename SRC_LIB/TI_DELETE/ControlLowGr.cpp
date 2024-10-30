#include "StdAfx.h"
#include "ControlLowGr.h"


//------------------------------------------
// コンストラクタ
//------------------------------------------
ControlLowGr::ControlLowGr() :
ThreadManager("LowGrCont")
{
	char		cWk[1024];
	CString		sWk;


	//// iniファイル読込
	m_nIniThreadCount = GetPrivateProfileInt(TI_DELETE, "LOWGR_THREAD_COUNT", 0, TASKINI_NAME);
	m_nIniCycle = GetPrivateProfileInt(TI_DELETE, "LOWGR_CYCLE", 60, TASKINI_NAME);		
	m_nIniCycle = m_nIniCycle * 60 * 1000;					// 分→ms 
	m_nIniDeleDay = GetPrivateProfileInt(TI_DELETE, "LOWGR_DELEDAY", 0, TASKINI_NAME);

	GetPrivateProfileString(TI_DELETE, "SQL_SELECT_KEY", "", cWk, sizeof(cWk), TASKINI_NAME );
	m_cIniSqlSelectKey = cWk;


	// テーブル取得
	GetPrivateProfileString(TI_DELETE, "COILRESULT_TBL", "", cWk, sizeof(cWk), TASKINI_NAME );
	m_typSet.cTblResult = cWk;
	GetPrivateProfileString(TI_DELETE, "COILRESULT_TIME", "", cWk, sizeof(cWk), TASKINI_NAME );
	m_typSet.cKeyResultTime = cWk;
	GetPrivateProfileString(TI_DELETE, "MAINKEY_NAME", "", cWk, sizeof(cWk), TASKINI_NAME );
	m_typSet.cKeyMain = cWk;
	GetPrivateProfileString(TI_DELETE, "SUBKEY_NAME", "", cWk, sizeof(cWk), TASKINI_NAME );
	m_typSet.cKeySub = cWk;

	m_typSet.nMainCnt = 0;
	for(int ii=0; ii<MAX_TBL_MAIN; ii++) {
		CString		sKey;
		int			curPos	= 0;				// 区切り位置

		// メインデータ取得
		sKey.Format("TBL_KEY_%d", ii+1); 
		GetPrivateProfileString(TI_DELETE, sKey, "", cWk, sizeof(cWk), TASKINI_NAME );
		if(0==strlen(cWk)) continue;
		sWk = cWk;

		// メインテーブル設定取得
		SETTING_TBL::MAIN& typ = m_typSet.main[m_typSet.nMainCnt];
		typ.nSubCnt	= 0;
		GetColumnVal(sWk, curPos, typ.cTblMain );
		GetColumnVal(sWk, curPos, typ.cSqlWhere);

		// サブテーブル取得
		for(int jj=0; jj<MAX_TBL_SUB; jj++) {
			sKey.Format("TBL_SUB_%d_%d", ii+1, jj+1); 
			GetPrivateProfileString(TI_DELETE, sKey, "", cWk, sizeof(cWk), TASKINI_NAME );
			if(0==strlen(cWk)) continue;

			typ.cTblSub[ typ.nSubCnt ] = cWk;
			typ.nSubCnt ++;
		}
		m_typSet.nMainCnt ++;
	}


	//// その他もろもろ
	m_bEnable = __BOOL m_nIniThreadCount;

	//// シグナル生成
	m_evTimeCycle	= CreateWaitableTimer(NULL, FALSE, NULL);	
	m_evEndDelete	= CreateEvent(NULL, FALSE, FALSE, NULL);	
	for(int ii=0; ii<THREAD_MAXCNT; ii++) {
		mcls_pLowGr[ii] = NULL;
	}
}

//------------------------------------------
// デストラクタ
//------------------------------------------
ControlLowGr::~ControlLowGr(void)
{
	// 開放
	CancelWaitableTimer(m_evTimeCycle);
	CloseHandle(m_evTimeCycle);
	CloseHandle(m_evEndDelete);
}


//------------------------------------------
// 削除スレッド初期化処理 (一度のみ起動)
//------------------------------------------
void ControlLowGr::WorkerInit()
{
	//// ワーカースレッド 初期化
	for(int ii=0; ii<m_nIniThreadCount; ii++) {
		mcls_pLowGr[ii] = new DeleLowGr(ii+1, m_evEndDelete, &m_typSet);
		mcls_pLowGr[ii]->SetLogMgr(mcls_pLog); 
	}	

	//// ワーカースレッド 起動
	for(int ii=0; ii<m_nIniThreadCount; ii++) {
		mcls_pLowGr[ii]->Start();
	}
}


//------------------------------------------
// 削除スレッド停止処理 (一度のみ起動)
//------------------------------------------
void ControlLowGr::WorkerStop()
{
	//// ワーカースレッド 停止
	// まず全体に停止指示を通知する
	for(int ii=0; ii<THREAD_MAXCNT; ii++) {
		if(NULL == mcls_pLowGr[ii]) continue;
		mcls_pLowGr[ii]->SetStop();
	}
	LOG(em_MSG), "[%s] 各子スレッド停止要求完了", my_sThreadName );
}

//------------------------------------------
// 削除スレッド終了処理 (一度のみ起動)
//------------------------------------------
void ControlLowGr::WorkerExit()
{
	//// ワーカースレッド 開放
	for(int ii=0; ii<THREAD_MAXCNT; ii++) {
		if(NULL == mcls_pLowGr[ii]) continue;
		mcls_pLowGr[ii]->Stop(20000);
		SAFE_DELETE( mcls_pLowGr[ii] );
	}
}


// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// スレッド操作

//------------------------------------------
// スレッド開始前処理 (スレッド実行直後(シグナルセット前)にコールバック)
//------------------------------------------
int ControlLowGr::ThreadFirst()
{
	//// 定周期タイマーセット
	__int64 ts = (__int64)m_nIniCycle * -10000;	// 1μ秒→100ナノ秒 (マイナス指定で、指定時間経過でシグナル) (第3引数は2回目以降の起動間隔)
	SetWaitableTimer(m_evTimeCycle, (LARGE_INTEGER *)&ts, m_nIniCycle, NULL, NULL, FALSE);


	//// イベント設定
	HANDLE hArray[] = { m_evTimeCycle,			// 定周期タイマー
						m_evEndDelete			// DB削除完了
						};
	SetEventNum(sizeof(hArray)/sizeof(HANDLE), hArray);
	return 0;
}

//------------------------------------------
// スレッド終了前処理 (終了シグナル後にコールバック)
//------------------------------------------
int ControlLowGr::ThreadLast()
{
	return 0;
}

//------------------------------------------
// スレッドイベント発生
// int nEventNo イベントNo (0オリジン)
//------------------------------------------
void ControlLowGr::ThreadEvent(int nEventNo)
{
	////// シグナルの条件
	enum {	EV_TIMECYCLE = 0,				// 定周期タイマー
			EV_END_DELETE					// DB削除完了
	};

	////// シグナル条件分け
	switch (nEventNo) {

//-----------------------------------------------------------------------------------------------
	case EV_TIMECYCLE:											// 定周期タイマー
		if( ! CheckDeleListItem()) {
			LOG(em_WAR), "[%s] 定周期実行失敗", my_sThreadName );
			break;
		}
		if( 0 != m_nIniDeleDay ) {
			if( ! BackUpDeleListItem()) {
				LOG(em_WAR), "[%s] バックアップ実行失敗", my_sThreadName );
				break;
			}
		}
		break;
//-----------------------------------------------------------------------------------------------
	case EV_END_DELETE:											// DB削除完了
		if( ! CheckDeleListItem()) {
			LOG(em_WAR), "[%s] 定周期実行失敗", my_sThreadName );
			break;
		}

		if( 0 != m_nIniDeleDay ) {
			if( ! BackUpDeleListItem()) {
				LOG(em_WAR), "[%s] バックアップ実行失敗", my_sThreadName );
				break;
			}
		}
	
		break;

//-----------------------------------------------------------------------------------------------
	default :
		// ありえない！！
		LOG(em_ERR), "[%s] ThreadEvent EventNo NG=%d", my_sThreadName, nEventNo );
		return;
	}
}


// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// メイン

//------------------------------------------
// 削除対象の疵検管理Noかチェック
// char const* cKey チェックする主キー
// 戻り値： 0～:削除対象インデックス  -1:空き無し  -9:既に実行中
//------------------------------------------
int ControlLowGr::CheckExecItem(char const* cKey)
{
	int nIsWait = -1;
	for(int ii=0; ii<m_nIniThreadCount; ii++) {
		// 空きはある
		if(! mcls_pLowGr[ii]->IsExec() ) {		
			nIsWait = ii;
			continue;
		}

		// 同一の疵検管理Noで既に実行しているものがある。
		if( 0 == strcmp(cKey, mcls_pLowGr[ii]->ExecKey()) ) {
			return -9;
		}
	}
	return nIsWait;
}

//------------------------------------------
// 無害Gr削除対象チェック
//   統合サーバーが取得完了した場合のみ
//------------------------------------------
bool ControlLowGr::CheckDeleListItem()
{
	char	cKey[64];
	int		nRetc;

	//// 空き有り？
	if( 0 > CheckExecItem("") ) return true;

	//// ODBC接続クラス インスタンス生成
	OdbcBase clsDB;
	if( ! clsDB.Connect(TASKINI_NAME, INI_DB)) {
		// データベース接続エラー
		LOG(em_ERR), "[%s] [%s] DB接続エラー", my_sThreadName, INI_DB);
		syslog(SYSNO_DB_CONNECT_ERR, "");
		return false;
	}

	//// 無害Gr削除対象の疵検管理Noを取得
	if( !clsDB.ExecuteDirect(m_cIniSqlSelectKey)) {
		LOG(em_ERR), "[%s] キー取得 [%s]", my_sThreadName, m_cIniSqlSelectKey);
		syslog(SYSNO_DB_EXECUTEDIRECT_ERR, "[%s:キー取得]", my_sThreadName);
		return false;
	}

	while(true) {
		int sqlrc = clsDB.Fetch();
		if (sqlrc == KIZUODBC_FETCH_OK) {						// データ有り
			
			clsDB.GetData(1, cKey, sizeof(cKey));

			//// 空きチェック
			nRetc = CheckExecItem(cKey);
			if( 0 <= nRetc ) {
				// 待ち状態のスレッドに実行依頼を行う
				mcls_pLowGr[nRetc]->SetExec(cKey);
			} else if( -1 == nRetc) {
				// 空き無し
				return true;
			}
	
		} else if( sqlrc == KIZUODBC_FETCH_NODATE ) {			// データ無し
			break;
		} else {								// エラー
			syslog(SYSNO_DB_FETCH_ERR, "[%s:主キー取得処理]", my_sThreadName);
			return false;
		}
	}
	return true;
}

//------------------------------------------
// 無害Gr削除対象チェック
//   3日以上経過した無害Gr (残骸のバックアップ削除処理)
//------------------------------------------
bool ControlLowGr::BackUpDeleListItem()
{
	CString sql;
	char	cKey[64];
	int		nRetc;

	//// 空き有り？
	if( 0 > CheckExecItem("") ) return true;

	//// ODBC接続クラス インスタンス生成
	OdbcBase clsDB;
	if( ! clsDB.Connect(TASKINI_NAME, INI_DB)) {
		// データベース接続エラー
		LOG(em_ERR), "[%s] [%s] DB接続エラー", my_sThreadName, INI_DB);
		syslog(SYSNO_DB_CONNECT_ERR, "");
		return false;
	}


	//// 3日前の境界の疵検管理No
	char cOldKey[64];
	CString wk;
	SYSTEMTIME STime;
	GetLocalTime(&STime);
	sql.Format("SELECT %s FROM %s WHERE %s+%d<'%d/%d/%d'",
		m_typSet.cKeyMain, m_typSet.cTblResult, m_typSet.cKeyResultTime, m_nIniDeleDay, STime.wYear, STime.wMonth, STime.wDay);
	if( ! clsDB.GetSelectKey(sql, sizeof(cOldKey), cOldKey) ) {
		LOG(em_INF), "[%s] 3日前の主キー無し。無害Gr削除やめ [%s]", my_sThreadName, sql);
		//LOG(em_ERR), "[%s] 主キー取得失敗 [%s]", my_sThreadName, sql);
		//syslog(SYSNO_DB_EXECUTEDIRECT_ERR, "[%s:主キー取得]", m_typSet.cTblResult);
		//return false;
	}


	//// 3日前で 無害Grが存在する 疵検管理Noを取得
	sql = "";
	for(int ii=0; ii<m_typSet.nMainCnt; ii++) {
		wk.Format("SELECT DISTINCT %s FROM %s WHERE %s<'%s' %s",
			m_typSet.cKeyMain, m_typSet.main[ii].cTblMain, m_typSet.cKeyMain, cOldKey, m_typSet.main[ii].cSqlWhere );
		sql += wk;

		if(ii != m_typSet.nMainCnt-1) sql += " UNION ALL ";
	}


	if( !clsDB.ExecuteDirect(sql)) {
		LOG(em_ERR), "[%s] キー取得 [%s]", my_sThreadName, sql);
		syslog(SYSNO_DB_EXECUTEDIRECT_ERR, "[%s:キー取得]", m_typSet.main[0].cTblMain);
		return false;
	}


	while(true) {
		int sqlrc = clsDB.Fetch();
		if (sqlrc == KIZUODBC_FETCH_OK) {						// データ有り
			
			clsDB.GetData(1, cKey, sizeof(cKey));

			//// 空きチェック
			nRetc = CheckExecItem(cKey);
			if( 0 <= nRetc ) {
				LOG(em_WAR), "[%s] バックアップで強制無害Gr削除実施！ [%s] <%d>", my_sThreadName, cKey, nRetc+1);
				syslog(500, "[%s]", cKey);

				// 待ち状態のスレッドに実行依頼を行う
				mcls_pLowGr[nRetc]->SetExec(cKey);
			} else if( -1 == nRetc) {
				// 空き無し
				return true;
			}
	
		} else if( sqlrc == KIZUODBC_FETCH_NODATE ) {			// データ無し
			break;
		} else {								// エラー
			syslog(SYSNO_DB_FETCH_ERR, "[%s:主キー取得処理]", m_typSet.main[0].cTblMain);
			return false;
		}
	}
	return true;
}