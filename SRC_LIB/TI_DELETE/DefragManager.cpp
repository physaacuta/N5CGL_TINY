#include "StdAfx.h"
#include "DefragManager.h"

// メモリーリーク検出 ----->>>
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif
// <<<------------------------

//------------------------------------------
// コンストラクタ
// int id 自分のID (1オリジン)
// CString cSession セッション
//------------------------------------------
DefragManager::DefragManager(int id, char* cSession) :
ThreadManager( GetThreadNameArray("DefragMgr", id) ),
m_cSession(cSession),
m_pExecDefrag(NULL),
m_nTblNum(0),
m_pTblDef(NULL),
m_nTblExecNum(0),
m_nIdxExecNum(0),
m_bRunCancel(false)
{
	//// ハンドル生成
	m_evTimerExec  = CreateWaitableTimer(NULL, FALSE, NULL);

	m_nDefragIntervalDay = GetPrivateProfileInt(TI_DELETE, "DEFRAG_INTERVAL_DAY", 0, TASKINI_NAME);
	m_LastDefragTime = CTime::GetCurrentTime();


	//----------------------------------------
	////// テーブル情報取得
	int		nTblCnt;				// 件数
	char    cWk[1024];
	CString strVal;
	GetPrivateProfileString(m_cSession, "DB_ODBC", "", cWk, sizeof(cWk), TASKINI_NAME);
	m_cOdbcSession = cWk;


	//// 件数を取得
	nTblCnt = 0;
	while(true) {
		strVal.Format("ZI_%d", nTblCnt+1);
		GetPrivateProfileString(m_cSession, strVal, "", cWk, sizeof(cWk), TASKINI_NAME);		
		if(0 >= strlen(cWk) ) break;
		nTblCnt++;
	}
	m_nTblNum = nTblCnt;
	//// 0件以外の場合 テーブル生成
	if(0 != m_nTblNum) {
		nTblCnt = 0;
		m_pTblDef = new DEF_TBL[m_nTblNum];
		for(int ii=0; ii<m_nTblNum; ii++) {
			strVal.Format("ZI_%d", ii+1);
			GetPrivateProfileString(m_cSession, strVal, "", cWk, sizeof(cWk), TASKINI_NAME);		
		
			if( 0 < strlen(cWk) ) {
				strVal = cWk;
				
				// トークン分割
				int		rec = 0;
				int		curPos = 0;
				CString strToken = strVal.Tokenize(",", curPos);
				while ( strToken != "" ) {
					strToken.Trim(_T(" 	"));		// iniファイル設定文字の前後のスペース、タブを取り除く

					switch( rec ) {
						case 0:						// テーブル名
							m_pTblDef[ii].cDefTable = strToken; 
							break;
						default:
							break;
					}

					// 次へ
					strToken = strVal.Tokenize(",", curPos);
					rec ++;
				}
			}
		}
	}
}

//------------------------------------------
// デストラクタ
//------------------------------------------
DefragManager::~DefragManager(void)
{
	//// ハンドル解放
	CloseHandle(m_evTimerExec);
	// 開放
	SAFE_DELETE_ARRAY(m_pTblDef); 
}

//------------------------------------------
// スレッド開始前処理 (スレッド実行直後(シグナルセット前)にコールバック)
//------------------------------------------
int DefragManager::ThreadFirst()
{
	//// 事前準備
	//const int nCycle = (2 * 60 * 1000) ;
	const int nCycle = (3 * 1000) ;	
	__int64 ts = nCycle * -10000;			// 2分周期
	SetWaitableTimer(m_evTimerExec, (LARGE_INTEGER *)&ts, nCycle, NULL, NULL, FALSE);

	//// イベント設定
	HANDLE hArray[] = { m_evTimerExec
						};
	SetEventNum(sizeof(hArray)/sizeof(HANDLE), hArray);
	return 0;
}

//------------------------------------------
// スレッド終了前処理 (終了シグナル後にコールバック)
//------------------------------------------
int DefragManager::ThreadLast()
{
	// 定周期停止
	CancelWaitableTimer(m_evTimerExec);
	return 0;
}

//------------------------------------------
// スレッドイベント発生
// int nEventNo イベントNo (0オリジン)
//------------------------------------------
void DefragManager::ThreadEvent(int nEventNo)
{
	////// シグナルの条件
	enum {	EV_TIMER_EXEC = 0,				// 処理実行 定周期タイマ
	};

	////// シグナル条件分け
	switch (nEventNo) {

//-----------------------------------------------------------------------------------------------
	case EV_TIMER_EXEC: 					// 処理実行 定周期タイマ
		Exec();
		return;

	
//-----------------------------------------------------------------------------------------------
	default :
		// ありえない！！
		LOG(em_ERR), "[%s] ThreadEvent EventNo NG=%d", my_sThreadName, nEventNo );
		return;
	}
}

//------------------------------------------
// インデックス デフラグ実行
//------------------------------------------
void DefragManager::Exec()
{
	if( ! IsExec() ) return;
	
	//------------------------------------
	// 事前準備
	//// 経過時間で、再実行可能かチェック
	CTimeSpan	ts(m_nDefragIntervalDay, 0, 0, 0);	// 日
//▲	CTimeSpan	ts(0,0,0,20);	// 日
	CTime		ans = m_LastDefragTime + ts;
	if( ans >= CTime::GetCurrentTime() ) return;


	if( 0 != m_nTblExecNum || 0 != m_nIdxExecNum) {
		LOG(em_INF), "[%s] デフラグ再開 (%d:%d)", my_sThreadName, m_nTblExecNum, m_nIdxExecNum);
	}



	//------------------------------------
	// 実行
	int nRetc;			// 終了状態 (0:正常 1:キャンセル  -1:異常)
	for(int ii=m_nTblExecNum; ii<m_nTblNum; ii++) {
		std::vector<CString>			vIdxName;			// インデックス一覧

		//// インデックス一覧取得
		if( ! GetIndexTables(m_pTblDef[ii].cDefTable, &vIdxName) ) continue;
		if( ! IsExec() ) break; 
	
		//// デフラグ実行
		LOG(em_MSG), "[%s] Indexテーブル取得 %s=%d", my_sThreadName, m_pTblDef[ii].cDefTable, vIdxName.size());
		for(int jj=m_nIdxExecNum; jj<vIdxName.size(); jj++) {
			if(ii >= m_nTblNum) continue;
			if(jj >= vIdxName.size()) continue;

			// 実行
			nRetc = ExecDefrag(m_pTblDef[ii].cDefTable, vIdxName[jj]);
			if( ! IsExec() ) break; 
			
			// index単位 でちゃんと終わった
			if( IsExec() && 0 == nRetc ) {
				m_nIdxExecNum = jj+1;
			}
		}

		// table単位 でちゃんと終わった
		if( IsExec() && 0 == nRetc ) {
			m_nTblExecNum = ii+1;
		}
		m_nIdxExecNum = 0;
	}

	//------------------------------------
	// エンド
	// 全部 ちゃんと終わった場合、実行途中フラグは初期化
	if( IsExec() && 0 == nRetc) {
		LOG(em_INF), "[%s] デフラグ完了", my_sThreadName);

		m_nTblExecNum = 0;
		m_nIdxExecNum = 0;

		m_LastDefragTime = CTime::GetCurrentTime();// = CTime.GetCurrentTime();
	}
}

//------------------------------------------
// インデックス一覧取得
// char const* pName		テーブル名
// std::vector<CString>* v	テーブル名に紐付くインデックス一覧
//------------------------------------------
bool DefragManager::GetIndexTables(char const* pName, std::vector<CString>* v)
{
	char	cWk[256];

	//----------------------------------------
	// 一覧取得
	CString		sql;
	sql = "SELECT i.name FROM sys.indexes AS i INNER JOIN sysobjects AS o ON i.object_id = o.id WHERE o.xtype='U'";
	sql.AppendFormat(" AND o.name='%s'", pName); 


	//// ODBC接続クラス インスタンス生成
	OdbcBase clsDB;
	if( ! clsDB.Connect(TASKINI_NAME, m_cOdbcSession)) {
		// データベース接続エラー
		LOG(em_ERR), "[%s] DB接続エラー", my_sThreadName);
		syslog(SYSNO_DB_CONNECT_ERR, "");
		return false;
	}
	if( !clsDB.ExecuteDirect(sql)) {
		LOG(em_ERR), "[%s] SQL実行エラー", my_sThreadName);
		syslog(SYSNO_DB_EXECUTEDIRECT_ERR, "管理テーブル" );
		return false;
	}
	//// 取得
	while (TRUE) {
		int sqlrc = clsDB.Fetch();
		if (sqlrc == KIZUODBC_FETCH_OK) {					// データ有り
		
			clsDB.GetData(1, cWk, sizeof(cWk));
			v->push_back( cWk ); 

		} else if( sqlrc == KIZUODBC_FETCH_NODATE ) {		// データ無し
			break;	
		} else {								// エラー
			syslog(SYSNO_DB_FETCH_ERR, "管理テーブル");
			return false;
		}
	}
	return true;
}

//------------------------------------------
// インデックス デフラグ実行
// 復帰情報		終了状態 (0:正常 1:キャンセル  -1:異常)
//------------------------------------------
int DefragManager::ExecDefrag(char const* pTblName, char const* pIdxName)
{
	//----------------------------------------
	// 再構成(デフラグ)：REORGANIZE : オンライン中可
	// 再構築：REBUILD : オンライン中不可
	CString		sql;
	sql.Format("ALTER INDEX %s on %s REORGANIZE", pIdxName, pTblName);



	//// ODBC接続クラス インスタンス生成
	OdbcBase clsDB;
	if( ! clsDB.Connect(TASKINI_NAME, m_cOdbcSession)) {
		// データベース接続エラー
		LOG(em_ERR), "[%s] DB接続エラー", my_sThreadName);
		syslog(SYSNO_DB_CONNECT_ERR, "");
		return -1;
	}

	//// 時間計測
	KizuPerformanceCounter clsT;
	clsT.InitAve(); 
	clsT.CounterStart();


	//// 実行
	int  nRetc = 0;
	bool bFast = true;
	while(true) {
		nRetc = clsDB.ExecuteDirectAsync(sql, bFast);
		if( 0 == nRetc || -1 == nRetc) break;

		// 中断チェック
		if( ! IsExec() ) {
			LOG(em_MSG), "[%s] Indexデフラグ中断認識 [%s:%s]", my_sThreadName, pTblName, pIdxName);
			if( ! clsDB.ExecuteDirectAsyncCancel() ) {
				LOG(em_MSG), "[%s] キャンセル失敗 [%s]", my_sThreadName, clsDB.GetLastErrMsg());
			} else {
				LOG(em_MSG), "[%s] キャンセル成功", my_sThreadName, pTblName, pIdxName);
			}
			return 1;
		}

//		LOG(em_MSG), "[%s] Indexデフラグ中… [%s:%s]", my_sThreadName, pTblName, pIdxName);
		bFast=false;
		Sleep(1000);
	}


	//// 終わり
	clsT.CounterEnd();
	LOG(em_MSG), "[%s] Indexデフラグ完了 [%s:%s] (%ds)", my_sThreadName, pTblName, pIdxName, (int)(clsT.GetCycleTime()/1000.0) );
	return nRetc;
}