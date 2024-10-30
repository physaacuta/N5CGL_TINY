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
m_emMainState(STATE_NON),
m_emUnten(DIV_UNTEN_STOP),
ThreadManager("StatusMgr")
{
	// 初期化
	InitializeCriticalSection(&m_csLock);						// クリティカルセクションオブジェクトを初期化
	// もろもろ処理化
	memset(&m_CamPairInf, 0x00, sizeof(m_CamPairInf) );

	m_bGetEngStatNew = false;
	memset(&m_EngStat, 0x00, sizeof(m_EngStat));

}

//------------------------------------------
// デストラクタ
//------------------------------------------
StatusManager::~StatusManager(void)
{
	//// 開放
	DeleteCriticalSection(&m_csLock);							// クリティカルセクションオブジェクトを開放
}

// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ステータス制御

//------------------------------------------
// 指定のステータス名称取得
// MAIN_STATE st ステータス
//------------------------------------------
CString StatusManager::GetMainStateName(MAIN_STATE st) const
{
	CString wk;
	if		(st == STATE_NON)		{ wk = "初期化中"; }
	else if (st == STATE_INIT)		{ wk = "初期状態 (資源開放状態)"; }
	else if (st == STATE_WAIT)		{ wk = "準備完了 (資源占有状態)"; }
	else if (st == STATE_RUN)		{ wk = "検査中";}
	else if (st == STATE_CANNOT)	{ wk = "続行不可能";}
	else							{ wk.Format("ステータス異常 <%d>", st); }
	return wk;
}


//------------------------------------------
// ステータス変更
// MAIN_STATE st ステータス
//------------------------------------------
bool StatusManager::SetMainState(MAIN_STATE st)
{
	////// メインインスタンス取り出し
	MAIN_STATE	old;
	old = GetMainState();
	if( old == st ) {
		LOG(em_ERR), "ステータス変更NG [%s] → [%s]", GetMainStateName(old), GetMainStateName(st)); 
		return false;
	}

	//// クリティカルセクション突入
	AutoLock lock(&m_csLock);

	//// 変更ステータスにより処理分け
	m_emMainState = st;
	syslog(102, "[%s] ⇒ [%s]", GetMainStateName(old), GetMainStateName(st)); 

	//// 画面状態変更
	LOG(em_MSG), "============= <<ステータス変更>> [%s] ⇒ [%s] =============", GetMainStateName(old), GetMainStateName(st)); 
	return true;

}


//------------------------------------------
// 運転モード変更
// M_DIV_UNTEN st 運転モード
//------------------------------------------
bool StatusManager::SetUnten(EM_DIV_UNTEN st)
{
	////// メインインスタンス取り出し
	EM_DIV_UNTEN	old;
	old = GetUnten();
	if( old == st ) {
		//LOG(em_ERR), "運転状態変更NG [%s] → [%s]", GetUntenName(old), GetUntenName(st)); 
		return false;
	}

	//// クリティカルセクション突入
	AutoLock lock(&m_csLock);

	//// 変更ステータスにより処理分け
	m_emUnten = st;
	syslog(103, "[%s] ⇒ [%s]", GetUntenName(old), GetUntenName(st)); 
	LOG(em_MSG), "<<運転状態変更>> [%s] ⇒ [%s]", GetUntenName(old), GetUntenName(st)); 
	return true;

}





// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// カメラペア関係

//------------------------------------------
// パラメータ取得
//------------------------------------------
bool StatusManager::GetParam_CamPairInf()
{
	CString sql;					// SQL文
	int camset;
	int campair;
	int campair_cy;

	//// 領域初期化
	memset(&m_CamPairInf, 0x00, sizeof(m_CamPairInf) );

	//// ODBC接続クラス インスタンス生成
	OdbcBase clsDB;
	if( ! clsDB.Connect(TASKINI_NAME)) {
		// データベース接続エラー
		LOG(em_ERR), "[%s] DB接続エラー", my_sThreadName);
		syslog(SYSNO_DB_CONNECT_ERR, "");
		return false;
	}


	////// カメラセット別
	//// SQL文作成
	sql =  "SELECT";
	sql += " camset, CamPair, ISNULL(CamPairCycle,1)";
	sql += " FROM ";
	sql += DB_PARAM_CAMSET;
	sql += " ORDER BY camset ASC";
	//// 実行
	if( !clsDB.ExecuteDirect(sql)) {
		syslog(SYSNO_DB_EXECUTEDIRECT_ERR, "[%s]", DB_PARAM_CAMSET);
		return false;
	}
	//// 取得
	while (TRUE) {
		int sqlrc = clsDB.Fetch();
		if (sqlrc == KIZUODBC_FETCH_OK) {					// データ有り
			// 主キー
			clsDB.GetData(1, &camset);
			clsDB.GetData(2, &campair);
			clsDB.GetData(3, &campair_cy);

			if( camset > MAX_CAMSET || camset <= 0 || campair > NUM_CAMPAIR || campair <= 0 ||
				campair_cy > NUM_CAMPAIR_CYCLE || campair_cy <= 0 ) {

				// 主キー不正
				LOG(em_ERR), "[%s] [%s 主キー不正 camset=%d campair=%d campair_cy=%d]", my_sThreadName, DB_PARAM_CAMSET, camset, campair, campair_cy);
				syslog(SYSNO_DB_DATA_ERR, "[%s camset=%d campair=%d campair_cy=%d]", DB_PARAM_CAMSET, camset, campair, campair_cy);	
				continue;
			}

			// データセット
			m_CamPairInf.item[campair-1].camset[ m_CamPairInf.item[campair-1].num ] = camset;
			m_CamPairInf.item[campair-1].num ++;
			
			m_CamPairInf.item_cycle[campair_cy-1].camset[ m_CamPairInf.item_cycle[campair_cy-1].num ] = camset;
			m_CamPairInf.item_cycle[campair_cy-1].num ++;

		} else if( sqlrc == KIZUODBC_FETCH_NODATE ) {		// データ無し
			break;
		} else {								// エラー
			syslog(SYSNO_DB_FETCH_ERR, "[%s]", DB_PARAM_CAMSET);
			return false;
		}
	}

	LOG(em_MSG), "[%s] パラメータ取得完了", my_sThreadName);
	return true;
}
