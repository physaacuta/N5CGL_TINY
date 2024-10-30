#include "StdAfx.h"
#include "ParamManager.h"

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
ParamManager::ParamManager(void) :
ThreadManager("ParamManager")
{
	// シグナル生成
	m_evReadParam = CreateEvent( NULL, FALSE, FALSE, NULL );
}

//------------------------------------------
// デストラクタ
//------------------------------------------
ParamManager::~ParamManager(void)
{
	// イベント開放
	CloseHandle( m_evReadParam );
}

//------------------------------------------
// スレッド開始前処理 (スレッド実行直後(シグナルセット前)にコールバック)
//------------------------------------------
int ParamManager::ThreadFirst()
{
	//// イベント設定
	HANDLE	hArray[] = {m_evReadParam
						};
	SetEventNum( sizeof(hArray)/sizeof(HANDLE), hArray );
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
void ParamManager::ThreadEvent( int nEventNo )
{
	//// シグナルの条件
	enum {	EV_READ = 0,
			};						// パラメータ読み込み

	////// シグナル条件分け
	switch ( nEventNo ) {

//-----------------------------------------------------------------------------------------------
	case EV_READ:					// パラメータ読み込み
		ReadParam_All();			
		break;

//-----------------------------------------------------------------------------------------------
	default :
		// ありえない！！
		break;
	}
}

//------------------------------------------
// 全パラメータ読み込み
//------------------------------------------
bool ParamManager::ReadParam_All()
{
	if( ! ReadParam_Type() )		return false;
	if( ! ReadParam_Grade() )		return false;
	if( ! ReadParam_Master() )		return false;
	return true;
}

//------------------------------------------
// 疵種パラメータ読込
//------------------------------------------
bool ParamManager::ReadParam_Type()
{
	CString sql;					// SQL文
	int id;

	//// パラメータ初期化
	CString cDbName;
	cDbName = DB_MAE_TYPE_NAME;
	PARAM_TYPE	buf;
	memset( &buf, 0x00, sizeof(buf) );

	//// ODBC接続クラス インスタンス生成
	OdbcBase clsDB;
	if( ! clsDB.Connect( TASKINI_NAME ) ) {
		// データベース接続エラー
		LOG(em_ERR), "[%s] DB接続エラー", my_sThreadName );
		syslog(SYSNO_DB_CONNECT_ERR, "");
		return false;
	}

	//// SQL文作成
	sql =  "SELECT";
	sql += " 前工程コード, FFName, ID ";
	sql += " FROM ";
	sql += cDbName;

	//// 実行
	if( ! clsDB.ExecuteDirect( sql ) ) {
		LOG(em_NON), "%s", sql );
		LOG(em_ERR), "[%s] 前工程疵種マスター実行エラー [%s]", my_sThreadName, cDbName );
		syslog(SYSNO_DB_EXECUTEDIRECT_ERR, "[%s]", cDbName);
		return false;
	}
	
	id = 0;
	//// 取得
	while (true) {
		int sqlrc = clsDB.Fetch();
		if( sqlrc == KIZUODBC_FETCH_OK ) {					// データ有り

			// データ部
			clsDB.GetDataImage( 1, &buf.type_id[id].cProc, sizeof(buf.type_id[0].cProc) );
			KizuFunction::SpaceToNull( buf.type_id[id].cProc, sizeof(buf.type_id[0].cProc) );
			clsDB.GetDataImage( 2, &buf.type_id[id].cFFName, sizeof(buf.type_id[0].cFFName) );
			KizuFunction::SpaceToNull( buf.type_id[id].cFFName, sizeof(buf.type_id[0].cFFName) );
			clsDB.GetData( 3, &buf.type_id[id].edasysID );
			id++;
			buf.num++;

		} else if( sqlrc == KIZUODBC_FETCH_NODATE ) {		// データ無し
			break;	
		} else {								// エラー
			LOG(em_NON), "%s", sql );
			LOG(em_ERR), "[%s] 前工程疵種マスター取得エラー [%s]", my_sThreadName, cDbName );
			syslog(SYSNO_DB_FETCH_ERR, "[%s]", cDbName);
			return false;
		}
	}

	//// 取得データをメンバー変数にセット
	memcpy( &m_Param_Type, &buf, sizeof(buf) );
	LOG(em_MSG), "[%s] %s 取得完了", my_sThreadName, cDbName );
	return true;
}

//------------------------------------------
// Grパラメータ読込
//------------------------------------------
bool ParamManager::ReadParam_Grade()
{
	CString sql;					// SQL文
	int id;

	//// パラメータ初期化
	CString cDbName;
	cDbName = DB_MAE_GRADE_NAME;
	PARAM_GRADE	buf;
	memset( &buf, 0x00, sizeof(buf) );

	//// ODBC接続クラス インスタンス生成
	OdbcBase clsDB;
	if( ! clsDB.Connect( TASKINI_NAME ) ) {
		// データベース接続エラー
		LOG(em_ERR), "[%s] DB接続エラー", my_sThreadName );
		syslog(SYSNO_DB_CONNECT_ERR, "");
		return false;
	}

	//// SQL文作成
	sql =  "SELECT";
	sql += " 前工程コード, FFName, グレードID ";
	sql += " FROM ";
	sql += cDbName;

	//// 実行
	if( ! clsDB.ExecuteDirect( sql ) ) {
		LOG(em_NON), "%s", sql );
		LOG(em_ERR), "[%s] 前工程グレードマスター実行エラー [%s]", my_sThreadName, cDbName );
		syslog(SYSNO_DB_EXECUTEDIRECT_ERR, "[%s]", cDbName);
		return false;
	}
	
	id = 0;

	//// 取得
	while (true) {
		int sqlrc = clsDB.Fetch();
		if( sqlrc == KIZUODBC_FETCH_OK ) {					// データ有り

			// データ部
			clsDB.GetDataImage( 1, &buf.grade_id[id].cProc, sizeof(buf.grade_id[0].cProc) );
			KizuFunction::SpaceToNull( buf.grade_id[id].cProc, sizeof(buf.grade_id[0].cProc) );
			clsDB.GetDataImage( 2, &buf.grade_id[id].cFFName, sizeof(buf.grade_id[0].cFFName) );
			KizuFunction::SpaceToNull( buf.grade_id[id].cFFName, sizeof(buf.grade_id[0].cFFName) );
			clsDB.GetData( 3, &buf.grade_id[id].edasysID );
			id++;
			buf.num++;

		} else if( sqlrc == KIZUODBC_FETCH_NODATE ) {		// データ無し
			break;	
		} else {								// エラー
			LOG(em_NON), "%s", sql );
			LOG(em_ERR), "[%s] 前工程グレードマスター取得エラー [%s]", my_sThreadName, cDbName );
			syslog(SYSNO_DB_FETCH_ERR, "[%s]", cDbName);
			return false;
		}
	}

	//// 取得データをメンバー変数にセット
	memcpy( &m_Param_Grade, &buf, sizeof(buf) );
	LOG(em_MSG), "[%s] %s 取得完了", my_sThreadName, cDbName );
	return true;
}

//------------------------------------------
// 前工程マスターテーブル読込
//------------------------------------------
bool ParamManager::ReadParam_Master()
{
	CString sql;					// SQL文
	int id;

	//// パラメータ初期化
	CString cDbName;
	cDbName = DB_MAE_MASTER;
	PARAM_MASTER	buf;
	memset( &buf, 0x00, sizeof(buf) );

	//// ODBC接続クラス インスタンス生成
	OdbcBase clsDB;
	if( ! clsDB.Connect( TASKINI_NAME ) ) {
		// データベース接続エラー
		LOG(em_ERR), "[%s] DB接続エラー", my_sThreadName );
		syslog(SYSNO_DB_CONNECT_ERR, "");
		return false;
	}

	//// SQL文作成
	sql =  "SELECT";
	sql += " 前工程コード";
	sql += ",有効判定";
    sql += ",前工程表示モード";
	sql += ",画像形式";
    // 2014.07.25 川原  所間連携    >>>>>>>>
    sql += ",他所材判定";
    // 2014.07.25 川原  所間連携    <<<<<<<<
	sql += " FROM ";
	sql += cDbName;
	sql += " WHERE 有効判定 <> 0";				// 0：無効でない
	sql += "   AND 前工程表示モード <> 0";		// 0：本システムで未管理でない

    //// 実行
	if( ! clsDB.ExecuteDirect( sql ) ) {
		LOG(em_NON), "%s", sql );
		LOG(em_ERR), "[%s] 前工程マスター実行エラー [%s]", my_sThreadName, cDbName );
		syslog(SYSNO_DB_EXECUTEDIRECT_ERR, "[%s]", cDbName);
		return false;
	}
	
	id = 0;

	//// 取得
	while (true) {
		int sqlrc = clsDB.Fetch();
		if( sqlrc == KIZUODBC_FETCH_OK ) {					// データ有り

			// データ部

			clsDB.GetDataImage( 1, &buf.master_id[id].cProc, sizeof(buf.master_id[id].cProc) );
			KizuFunction::SpaceToNull( buf.master_id[id].cProc, sizeof(buf.master_id[id].cProc) );
			clsDB.GetData( 2, &buf.master_id[id].nEnable );
			clsDB.GetData( 3, &buf.master_id[id].nMode );
			clsDB.GetData( 4, &buf.master_id[id].nImgFmt );
			
            // 2014.07.25 川原  所間連携    >>>>>>>>
			clsDB.GetData( 5, &buf.master_id[id].nTasyoZai );
            // 2014.07.25 川原  所間連携    <<<<<<<<

            id++;
			buf.num++;

		} else if( sqlrc == KIZUODBC_FETCH_NODATE ) {		// データ無し
			break;	
		} else {								// エラー
			LOG(em_NON), "%s", sql );
			LOG(em_ERR), "[%s] 前工程マスター取得エラー [%s]", my_sThreadName, cDbName );
			syslog(SYSNO_DB_FETCH_ERR, "[%s]", cDbName);
			return false;
		}
	}

	//// 取得データをメンバー変数にセット
	memcpy( &m_Param_Master, &buf, sizeof(buf) );
	LOG(em_MSG), "[%s] %s 取得完了", my_sThreadName, cDbName );
	return true;
}

//------------------------------------------
// 疵種EdasysID取得
// char* cProc		前工程コード
// char* cFFName　　FF疵種
// 戻り値 EdasysID (未検出 0)
//------------------------------------------
int ParamManager::GetTyppeEdasysID( char* cProc, char* cFFName ) const 
{
	for( int ii=0; ii<m_Param_Type.num; ii++ ) {
		
		// 前工程とFF疵種が一致
		if( 0 == memcmp( m_Param_Type.type_id[ii].cProc, cProc, sizeof(m_Param_Type.type_id[ii].cProc) )
			&& 0 == memcmp( m_Param_Type.type_id[ii].cFFName, cFFName, sizeof(m_Param_Type.type_id[ii].cFFName) ) ) {

			return m_Param_Type.type_id[ii].edasysID;
		}

	}
	return 0;
}

//------------------------------------------
// グレードEdasysID取得
// char* cProc		前工程コード
// char* cFFName　　FFグレード
// 戻り値 EdasysID (未検出 0)
//------------------------------------------
int ParamManager::GetGradeEdasysID(char* cProc, char* cFFName) const 
{
	for( int ii=0; ii<m_Param_Grade.num; ii++ ) {
		
		// 前工程とFFグレードが一致
		if( 0 == memcmp( m_Param_Grade.grade_id[ii].cProc, cProc, sizeof(m_Param_Grade.grade_id[ii].cProc) )
			&& 0 == memcmp( m_Param_Grade.grade_id[ii].cFFName, cFFName, sizeof(m_Param_Grade.grade_id[ii].cFFName) ) ) {
			return m_Param_Grade.grade_id[ii].edasysID;
		}
	}
	return 0;
}

//------------------------------------------
// 前工程マスター前工程表示モード取得
// char* cProc		前工程コード
// 戻り値 前工程表示モード (未検出 -1)
//------------------------------------------
int ParamManager::GetDspMode(char* cProc) const 
{
	for( int ii=0; ii<m_Param_Master.num; ii++ ) {
		
		// 前工程コードが一致
		if( 0 == memcmp( m_Param_Master.master_id[ii].cProc, cProc, sizeof(m_Param_Master.master_id[ii].cProc) ) ) {
			return m_Param_Master.master_id[ii].nMode;
		}
	}
	return -1;
}

// 2014.07.25 川原  所間連携    >>>>>>>>
//------------------------------------------
// 他所材判定取得
//------------------------------------------
// char*	cProc	前工程コード
//------------------------------------------
// 戻り値	他所材判定[0:自所材、1:他所材]
//------------------------------------------
int ParamManager::GetTasyoZai(CString cProc)
{
	int	ii;					// カウンター
	int nTasyoZai   = 0;    // 他所材判定[0:自所材]

    // 前工程コード検索
	for(ii=0; ii<m_Param_Master.num; ii++){ // 登録数分
		// 前工程一致
		if( 0 == memcmp(cProc, m_Param_Master.master_id[ii].cProc, 2) ){
			// 他所材判定
			nTasyoZai	= m_Param_Master.master_id[ii].nTasyoZai;
			break;
		}
	}

	return	nTasyoZai;
}
// 2014.07.25 川原  所間連携    <<<<<<<<
