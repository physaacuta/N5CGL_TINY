#include "StdAfx.h"
#include "ParamManager.h"

// メモリーリーク検出 ----->>>
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif
// <<<------------------------

#include "MainInctance.h"

//------------------------------------------
// コンストラクタ
//------------------------------------------
ParamManager::ParamManager(void) :
ThreadManager("ParamMgr")
{
}

//------------------------------------------
// デストラクタ
//------------------------------------------
ParamManager::~ParamManager(void)
{
}


//------------------------------------------
// スレッド開始前処理 (スレッド実行直後(シグナルセット前)にコールバック)
//------------------------------------------
int ParamManager::ThreadFirst()
{

	//// イベント設定
	HANDLE hArray[] = { gque_Deli.g_evSem
						};
	SetEventNum(sizeof(hArray)/sizeof(HANDLE), hArray);
	return 0;
}

//------------------------------------------
// スレッド終了前処理 (終了シグナル後にコールバック)
//------------------------------------------
int ParamManager::ThreadLast()
{
	return 0;
}

//------------------------------------------
// スレッドイベント発生
// int nEventNo イベントNo (0オリジン)
//------------------------------------------
void ParamManager::ThreadEvent(int nEventNo)
{
	////// シグナルの条件
	enum {	EV_QUE = 0,
	};

	////// シグナル条件分け
	switch (nEventNo) {

//-----------------------------------------------------------------------------------------------
	case EV_QUE: 
		return;

//-----------------------------------------------------------------------------------------------
	default :
		// ありえない！！
		return;
	}
}

// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// パラメータ読込

//------------------------------------------
// 全パラメータ読み込み
//------------------------------------------
bool ParamManager::ReadParam_All()
{
	if( ! Read_CommonMast() )	return false;

	return true;
}
//------------------------------------------
// φω定数パラメータ読込
//------------------------------------------
bool ParamManager::Read_CommonMast()
{
	CString sql;					// SQL文

	//// パラメータ初期化
	CString cDbName;
	cDbName = DB_COMMON_MASTER;
	P_COMMON_MAST&	p = m_CommonMast;
	P_COMMON_MAST	buf;
	memset(&buf, 0x00, sizeof(buf));


	//// ODBC接続クラス インスタンス生成
	OdbcBase clsDB;
	if( ! clsDB.Connect(TASKINI_NAME)) {
		// データベース接続エラー
		LOG(em_ERR), "[%s] DB接続エラー[%s]", my_sThreadName, clsDB.GetLastErrMsg());
		syslog(SYSNO_DB_CONNECT_ERR, "");
		return false;
	}

	//// SQL文作成
	sql  = "SELECT ";
	sql += " LongPd_GuidanceAvailable ";					// [長周期欠陥ガイダンス有無]
	sql += ",LongPd_GuidanceCheckInterval ";				// [長周期欠陥ガイダンス確認実施間隔]
	sql += ",LongPd_NearFurnaceRangeFrom ";					// [長周期欠陥炉チカ判定範囲From]
	sql += ",LongPd_NearFurnaceRangeTo ";					// [長周期欠陥炉チカ判定範囲To]
	sql += "FROM ";
	sql += cDbName;

	//// 実行
	if( !clsDB.ExecuteDirect(sql)) {
		syslog(SYSNO_DB_EXECUTEDIRECT_ERR, "[%s][%s]", cDbName, clsDB.GetLastErrMsg());
		return false;
	}

	//// 取得
	int nRecord = 0;
	while (TRUE) {
		int sqlrc = clsDB.Fetch();
		if (sqlrc == KIZUODBC_FETCH_OK) {					// データ有り
	
			// データ部
			clsDB.GetData(1, &buf.nLongPd_GuidanceAvailable);		// [長周期欠陥ガイダンス有無]
			clsDB.GetData(2, &buf.nLongPd_GuidanceCheckInterval);	// [長周期欠陥ガイダンス確認実施間隔]
			clsDB.GetData(3, &buf.nLongPd_NearFurnaceRangeFrom);	// [長周期欠陥炉チカ判定範囲From]
			clsDB.GetData(4, &buf.nLongPd_NearFurnaceRangeTo);		// [長周期欠陥炉チカ判定範囲To]

			nRecord++;

		} else if( sqlrc == KIZUODBC_FETCH_NODATE ) {		// データ無し
			break;	
		} else {								// エラー
			syslog(SYSNO_DB_FETCH_ERR, "[%s][%s]", cDbName, clsDB.GetLastErrMsg());
			return false;
		}
	}

	//// 取ってきたレコード数は正常？？
	if( 1 != nRecord ){
		LOG(em_ERR), "[%s] %s レコード数異常[%d]", my_sThreadName, cDbName, nRecord);
		return false;
	}

	//// 取得データをメンバー変数にセット
	memcpy( &p, &buf, sizeof(buf));
	LOG(em_MSG), "[%s] %s 取得完了", my_sThreadName, cDbName);
	return true;
}
