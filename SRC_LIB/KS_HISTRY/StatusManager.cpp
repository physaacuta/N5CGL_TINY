#include "StdAfx.h"
#include "StatusManager.h"


// メモリーリーク検出 ----->>>
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif
// <<<------------------------

//------------------------------------------
// コンストラクタ
//------------------------------------------
StatusManager::StatusManager(void) :
ThreadManager("StMgr"),
m_vItem(NULL)
{
	m_nDbColumnKiki		= GetPrivateProfileInt(KS_HISTRY, "MAX_DB_COLUMN_KIKI",	1, TASKINI_NAME);
	m_nDbColumnTemp		= GetPrivateProfileInt(KS_HISTRY, "MAX_DB_COLUMN_TEMP",	0, TASKINI_NAME);

	//// ハンドル生成
	m_evTimerCheck	  = CreateWaitableTimer(NULL, FALSE, NULL);
}
//------------------------------------------
// デストラクタ
//------------------------------------------
StatusManager::~StatusManager(void)
{
	//// ハンドル解放
	CloseHandle(m_evTimerCheck);
}

//------------------------------------------
// スレッド開始前処理 (スレッド実行直後(シグナルセット前)にコールバック)
//------------------------------------------
int StatusManager::ThreadFirst()
{
	//// マスター情報を取得
	while(true) {
		if( ! Init() ) {
			Sleep(10000);
			continue;
		}
		break;
	}

	//// 10秒固定タイマー 起動
	__int64 ts = DB_CHECK_TIME * -10000;		// 1ミリ秒→100ナノ秒 (マイナス指定で、指定時間経過でシグナル) (第3引数は2回目以降の起動間隔)
	SetWaitableTimer(m_evTimerCheck, (LARGE_INTEGER *)&ts, DB_CHECK_TIME, NULL, NULL, FALSE);


	//// イベント設定
	HANDLE hArray[] = { m_evTimerCheck
						};
	SetEventNum(sizeof(hArray)/sizeof(HANDLE), hArray);
	return 0;
}
//------------------------------------------
// スレッド終了前処理 (終了シグナル後にコールバック)
//------------------------------------------
int StatusManager::ThreadLast()
{
	//// タイマー停止
	CancelWaitableTimer(m_evTimerCheck);

	return 0;
}

//------------------------------------------
// スレッドイベント発生
// int nEventNo イベントNo (0オリジン)
//------------------------------------------
void StatusManager::ThreadEvent(int nEventNo)
{
	////// シグナルの条件
	enum {	EV_TIEM_CHECK = 0,				// DBアクセスチェック
	};

	////// シグナル条件分け
	switch (nEventNo) {

//-----------------------------------------------------------------------------------------------
	case EV_TIEM_CHECK: 					// 保存フォルダアクセスチェック
		CheckItem();
		return;

//-----------------------------------------------------------------------------------------------
	default :
		// ありえない！！
		LOG(em_ERR), "[%s] ThreadEvent EventNo NG=%d", my_sThreadName, nEventNo );
		return;
	}
}

// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// メイン処理

//------------------------------------------
// DBから設定情報を取得
//------------------------------------------
bool StatusManager::Init()
{
	CString sql;					// SQL文
	CString	sWk;
	char cWk[128];

	//// パラメータ初期化
	CString cDbName;
	cDbName = DB_ST_HISTORY_MASTER;

	//// ODBC接続クラス インスタンス生成
	OdbcBase clsDB;
	if( ! clsDB.Connect(TASKINI_NAME)) {
		// データベース接続エラー
		LOG(em_ERR), "[%s] DB接続エラー", my_sThreadName);
		syslog(SYSNO_DB_CONNECT_ERR, "");
		return false;
	}

	//// SQL文作成
	sql =  "SELECT";
	sql += " 区分, 収集周期";
	for(int ii=0; ii<m_nDbColumnKiki; ii++) {
		sWk.Format(", ISNULL(名称%d, '')", ii+1);
		sql += sWk;
	}
	for(int ii=0; ii<m_nDbColumnKiki; ii++) {
		sWk.Format(", ISNULL(機器ID_%d, 0)", ii+1);
		sql += sWk;
	}
	sql += " FROM ";
	sql += cDbName;
	sql += " ORDER BY 区分 ASC";



	//// 実行
	if( !clsDB.ExecuteDirect(sql)) {
		syslog(SYSNO_DB_EXECUTEDIRECT_ERR, "[%s]", cDbName);
		return false;
	}

	//// 取得
	while (TRUE) {
		int sqlrc = clsDB.Fetch();
		if (sqlrc == KIZUODBC_FETCH_OK) {					// データ有り
		
			TYP_LIST t;

			// DBから取得
			clsDB.GetData(1, &t.nKey);
			clsDB.GetData(2, &t.nCycle);

			for(int ii=0; ii<m_nDbColumnKiki; ii++) {
				//memset(cWk, 0x00, sizeof(cWk));
				clsDB.GetData(3 + ii, cWk, sizeof(cWk));
				t.sName[ii] = cWk;
			}
			for(int ii=0; ii<m_nDbColumnKiki; ii++) {
				clsDB.GetData(3 + m_nDbColumnKiki + ii, &t.nId[ii]);
			}


			// その他
			if(0 < t.nCycle) {
				t.nNextCnt = 1;						// 初回は、1分後に全部一度読む
			} else {
				t.nNextCnt = -1;					// 対象外
			}
			for(int ii=0; ii<MAX_COLUMN; ii++) {
				t.nNewN[ii] = 0;
				t.sNewS[ii] = ""; 
			}

			// リストに追加
			m_vItem.push_back(t);
	

		} else if( sqlrc == KIZUODBC_FETCH_NODATE ) {		// データ無し
			break;	
		} else {								// エラー
			syslog(SYSNO_DB_FETCH_ERR, "[%s]", cDbName);
			return false;
		}
	}
	return true;
}

//------------------------------------------
// 収集タイミングを監視
//------------------------------------------
void StatusManager::CheckItem()
{
	for(int ii=0; ii<(int)m_vItem.size(); ii++) {
		// そもそも対象外は 無視
		if( 0  >= m_vItem[ii].nCycle )   continue;
		if( -1 == m_vItem[ii].nNextCnt ) continue;

		// タイマーチェック
		m_vItem[ii].nNextCnt -= 1;
		if( 0 < m_vItem[ii].nNextCnt) continue;

		// 実行
		GetStatusInf(&m_vItem[ii]);

		// 次回 セット
		m_vItem[ii].nNextCnt = m_vItem[ii].nCycle;
	}
}

//------------------------------------------
// DBから設定情報を取得
// TYP_LIST* t 対象情報
//------------------------------------------
bool StatusManager::GetStatusInf(TYP_LIST* t)
{
	CString sql;					// SQL文
	CString	sWk;
	int nSetCnt = 0;				// 機器ID件数


	//// ODBC接続クラス インスタンス生成
	OdbcBase clsDB;
	if( ! clsDB.Connect(TASKINI_NAME)) {
		// データベース接続エラー
		LOG(em_ERR), "[%s] DB接続エラー", my_sThreadName);
		syslog(SYSNO_DB_CONNECT_ERR, "");
		return false;
	}


	// ================================
	//// 現在の機器状態と取得
	sql =  "SELECT 機器ID, 機器状態 ";
	sql += "FROM ";
	sql += DB_STATUS_INF;
	sql += " WHERE ";

	for(int ii=0; ii<m_nDbColumnKiki; ii++) {
		if( 0 >= t->nId[ii] ) continue;
		if( 0 != nSetCnt) sql += "OR ";

		sWk.Format("機器ID=%d ", t->nId[ii]);
		sql += sWk;

		nSetCnt ++;
	}

	//// 実行
	if( !clsDB.ExecuteDirect(sql)) {
		syslog(SYSNO_DB_EXECUTEDIRECT_ERR, "[%s]", DB_STATUS_INF);
		return false;
	}


	// ================================
	// バッファに保存
	memset(t->bSetN, 0x00, sizeof(t->bSetN));
	memset(t->bSetS, 0x00, sizeof(t->bSetS));

	//// 取得
	int nId = 0;
	int nWk = 0;
	while (TRUE) {
		int sqlrc = clsDB.Fetch();
		if (sqlrc == KIZUODBC_FETCH_OK) {					// データ有り

			// 同一IDにセットする
			clsDB.GetData(1, &nId);
			clsDB.GetData(2, &nWk);
			for(int ii=0; ii<m_nDbColumnKiki; ii++) {
				if( nId != t->nId[ii] ) continue;
				t->bSetN[ii] = true;
				t->nNewN[ii] = nWk;
				break;
			}

		} else if( sqlrc == KIZUODBC_FETCH_NODATE ) {		// データ無し
			break;	
		} else {								// エラー
			syslog(SYSNO_DB_FETCH_ERR, "[%s]", DB_STATUS_INF);
			return false;
		}
	}
	
	//// 現在時刻をセット
	CTime tWk = CTime::GetCurrentTime();
	t->sNewTime = tWk.Format("%Y/%m/%d %H:%M:%S");

	//// 継承先で付加情報 を 付与
	SetStringData(t);						


	// ================================
	// 履歴に登録
	sql = "INSERT INTO ";
	sql += DB_ST_HISTORY_INF;
	sql += " VALUES(";

	// データ部
	CreateSQL::sqlColumnInsert(&sql, t->sNewTime);
	CreateSQL::sqlColumnInsert(&sql, t->nKey);

	for(int ii=0; ii<m_nDbColumnKiki; ii++) {
		if( t->bSetN[ii] )	CreateSQL::sqlColumnInsert(&sql, t->nNewN[ii]);
		else				CreateSQL::sqlColumnInsert(&sql, NULL, 0);
	}
	for(int ii=0; ii<m_nDbColumnTemp; ii++) {
		if( t->bSetS[ii] )	CreateSQL::sqlColumnInsert(&sql, t->sNewS[ii]);
		else				CreateSQL::sqlColumnInsert(&sql, NULL, 0);
	}
	CreateSQL::sqlColumnTrim(&sql);		// 最後のカンマを取り除く 
	sql += ")";

	//// 実行
	if( ! clsDB.ExecuteDirect(sql) ) {
		LOG(em_ERR), "[%s] 登録失敗 [%s] [%s]", my_sThreadName, DB_ST_HISTORY_INF, sql);
		syslog(SYSNO_DB_EXECUTEDIRECT_ERR, "[%s]", DB_ST_HISTORY_INF);
		return false;
	}
	return true;
}



