
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
FuncBase("ParamMgr")
{
}
//------------------------------------------
// デストラクタ
//------------------------------------------
ParamManager::~ParamManager(void)
{
}

// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// パラメータ読込

//------------------------------------------
// 全パラメータ読み込み
//------------------------------------------
bool ParamManager::ReadParam_All()
{
	bool retc = true;
	if( ! Read_CommonMast() )	retc = false;
	if( ! Read_ScodeOutput() )	retc = false;
	if( ! Read_PcodeOutput() )	retc = false;
	return retc;
}

//------------------------------------------
// 疵検定数パラメータ読込
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
		LOG(em_ERR), "[%s] DB接続エラー", my_sThreadName);
		syslog(SYSNO_DB_CONNECT_ERR, "");
		return false;
	}

	//// SQL文作成
	sql =  "SELECT";
	sql += " [ウォーマ材指定位置] ";
	sql += " FROM ";
	sql += cDbName;

	//// 実行
	if( !clsDB.ExecuteDirect(sql)) {
		LOG(em_ERR), "[%s] DB_EXECUTEDIRECT [%s]", my_sThreadName, cDbName);
		LOG(em_NON), "%s", sql);
		syslog(SYSNO_DB_EXECUTEDIRECT_ERR, "[%s]", cDbName);
		return false;
	}

	//// 取得
	while (TRUE) {
		int sqlrc = clsDB.Fetch();
		if (sqlrc == KIZUODBC_FETCH_OK) {					// データ有り
	
			// データ部
			clsDB.GetData(1, &buf.nWalm );


		} else if( sqlrc == KIZUODBC_FETCH_NODATE ) {		// データ無し
			break;	
		} else {								// エラー
			LOG(em_ERR), "[%s] SYSNO_DB_FETCH_ERR [%s]", my_sThreadName, cDbName);
			LOG(em_NON), "%s", sql);
			syslog(SYSNO_DB_FETCH_ERR, "[%s]", cDbName);
			return false;
		}
	}

	//// 取得データをメンバー変数にセット
	memcpy( &p, &buf, sizeof(buf));
	LOG(em_MSG), "[%s] %s 取得完了", my_sThreadName, cDbName);
	return true;
}

//------------------------------------------
// 検査表面状態パラメータ読込
//------------------------------------------
bool ParamManager::Read_ScodeOutput()
{
	CString sql;					// SQL文
	int id;							// scode,pcodeのID (0オリジン)
	int no;
		
	//// パラメータ初期化
	CString cDbName;
	P_SCODE_OUTPUT&	buf = (P_SCODE_OUTPUT&)m_ScodeOutput;
	memset(&buf, 0x00, sizeof(P_SCODE_OUTPUT));


	//// ODBC接続クラス インスタンス生成
	OdbcBase clsDB;
	if( ! clsDB.Connect(TASKINI_NAME)) {
		// データベース接続エラー
		LOG(em_ERR), "[%s] DB接続エラー", my_sThreadName);
		syslog(SYSNO_DB_CONNECT_ERR, "");
		return false;
	}

	////////////////////////////////////
	// マスター
	////////////////////////////////////
	cDbName = DB_SCODE_MASTER;
	//// SQL文作成
	sql =  "SELECT";
	sql += " scode, 有効判定";
	sql += " FROM ";
	sql += cDbName;
	sql += " ORDER BY 優先度 ASC ";

	//// 実行
	if( !clsDB.ExecuteDirect(sql)) {
		LOG(em_ERR), "[%s] DB_EXECUTEDIRECT [%s]",my_sThreadName,cDbName);
		LOG(em_NON), "%s", sql);
		syslog(SYSNO_DB_EXECUTEDIRECT_ERR, "[%s]", cDbName);
		return false;
	}

	//// 取得
	while (TRUE) {
		int sqlrc = clsDB.Fetch();
		if (sqlrc == KIZUODBC_FETCH_OK) {					// データ有り

			// 主キー
			clsDB.GetData(1, &id);
			if( MAX_SCODE < id || 0 >= id ) {
				// 主キー不正
				LOG(em_ERR), "[%s] [%s 主キー不正 id=%d]", my_sThreadName, cDbName, id);
				syslog(SYSNO_DB_DATA_ERR, "[%s id=%d]", cDbName, id);					
				continue;
			}
			id -= 1;										// 0オリジンに変更する


			// データ部
			clsDB.GetData(2, &buf.rec[id].bUmu );


			//// 優先度順に格納
			buf.nPriority[ buf.nMasterNum ] = id;
			buf.nMasterNum++;

		} else if( sqlrc == KIZUODBC_FETCH_NODATE ) {		// データ無し
			break;	
		} else {								// エラー
			LOG(em_ERR), "[%s] DB FETCH_ERR [%s]", my_sThreadName, cDbName);
			LOG(em_NON), "%s", sql);
			syslog(SYSNO_DB_FETCH_ERR, "[%s]", cDbName);
			return false;
		}
	}

	////////////////////////////////////
	// 条件設定
	////////////////////////////////////
	cDbName = DB_SCODE_JUDGE;
	//// SQL文作成
	sql =  "SELECT";
	sql += " scode, 条件No, 有効判定";

	sql += " ,ISNULL(SPM圧下有無, -1)";							// 4.SPM圧下有無
	sql += " ,命令No";											// 5.命令No
	sql += " ,生産No";											// 6.命令No
	sql += " ,ISNULL(注文板厚下限, 0), ISNULL(注文板厚上限, 0)";// 7.注文板厚下限[μm] 8.注文板厚上限[μm]
	sql += " ,検査基準";										// 9.検査基準
	sql += " ,向先";											// 10.向先
	sql += " ,用途";											// 11.用途
	sql += " ,主材質";											// 12.主材質
	sql += " ,表面処理1桁目";									// 13.表面処理1桁目
	sql += " ,品種";											// 14.品種

	sql += " FROM ";
	sql += cDbName;
	sql += " WHERE 有効判定=1";		// 転送量削減対策
	sql += " ORDER BY scode, 条件No";

	//// 実行
	if( !clsDB.ExecuteDirect(sql)) {
		LOG(em_ERR), "[%s] DB_EXECUTEDIRECT [%s]",my_sThreadName,cDbName);
		LOG(em_NON), "%s", sql);
		syslog(SYSNO_DB_EXECUTEDIRECT_ERR, "[%s]", cDbName);
		return false;
	}

	//// 取得
	while (TRUE) {
		int sqlrc = clsDB.Fetch();
		if (sqlrc == KIZUODBC_FETCH_OK) {					// データ有り

			// 主キー
			clsDB.GetData(1, &id);
			clsDB.GetData(2, &no);
			if( (MAX_SCODE < id || 0 >= id) || (MAX_CODE_JUDGE < no || 0 >= no) ) {
				// 主キー不正
				LOG(em_ERR), "[%s] [%s 主キー不正 id=%d no=%d]", my_sThreadName, cDbName, id, no);
				syslog(SYSNO_DB_DATA_ERR, "[%s id=%d no=%d]", cDbName, id, no);					
				continue;
			}
			id -= 1;										// 0オリジンに変更する
			no -= 1;										// 0オリジンに変更する
			clsDB.GetData(3, &buf.rec[id].typJudge[no].bUmu );

			// データ部
			clsDB.GetData(4, &buf.rec[id].typJudge[no].nSpm);				// SPM圧下有無
			clsDB.GetData(5, &buf.rec[id].typJudge[no].cMeiNo, 32);			// 命令No
			clsDB.GetData(6, &buf.rec[id].typJudge[no].cCoilNo, 32);		// 生産No
			clsDB.GetData(7, &buf.rec[id].typJudge[no].nAtuSei[0]);			// 注文板厚下限
			clsDB.GetData(8, &buf.rec[id].typJudge[no].nAtuSei[1]);			// 注文板厚上限
			clsDB.GetData(9, &buf.rec[id].typJudge[no].cKenKijun, 32);		// 検査基準
			clsDB.GetData(10, &buf.rec[id].typJudge[no].cMuke, 32);			// 向先
			clsDB.GetData(11, &buf.rec[id].typJudge[no].cYouto, 32);		// 用途
			clsDB.GetData(12, &buf.rec[id].typJudge[no].cSyuZai, 32);		// 主材質
			clsDB.GetData(13, &buf.rec[id].typJudge[no].cOmoteSyori1, 32);	// 表面処理1桁目
			clsDB.GetData(14, &buf.rec[id].typJudge[no].cHinCode, 32);		// 品種

		} else if( sqlrc == KIZUODBC_FETCH_NODATE ) {		// データ無し
			break;	
		} else {								// エラー
			LOG(em_ERR), "[%s] DB FETCH_ERR [%s]", my_sThreadName, cDbName);
			LOG(em_NON), "%s", sql);
			syslog(SYSNO_DB_FETCH_ERR, "[%s]", cDbName);
			return false;
		}
	}

	LOG(em_MSG), "[%s] %s 取得完了", my_sThreadName, cDbName);

	return true;
}


//------------------------------------------
// 検査パターン別パラメータ読込
//------------------------------------------
bool ParamManager::Read_PcodeOutput()
{
	CString sql;					// SQL文
	int id;							// scode,pcodeのID (0オリジン)
	int no;

	//// パラメータ初期化
	CString cDbName;
	P_PCODE_OUTPUT&	buf = (P_PCODE_OUTPUT&)m_PcodeOutput;
	memset(&buf, 0x00, sizeof(P_PCODE_OUTPUT));


	//// ODBC接続クラス インスタンス生成
	OdbcBase clsDB;
	if( ! clsDB.Connect(TASKINI_NAME)) {
		// データベース接続エラー
		LOG(em_ERR), "[%s] DB接続エラー", my_sThreadName);
		syslog(SYSNO_DB_CONNECT_ERR, "");
		return false;
	}

	////////////////////////////////////
	// マスター
	////////////////////////////////////
	cDbName = DB_PCODE_MASTER;
	//// SQL文作成
	sql =  "SELECT";
	sql += " pcode, 有効判定";
	sql += " FROM ";
	sql += cDbName;
	sql += " ORDER BY 優先度 ASC ";

	//// 実行
	if( !clsDB.ExecuteDirect(sql)) {
		LOG(em_ERR), "[%s] DB_EXECUTEDIRECT [%s]",my_sThreadName,cDbName);
		LOG(em_NON), "%s", sql);
		syslog(SYSNO_DB_EXECUTEDIRECT_ERR, "[%s]", cDbName);
		return false;
	}

	//// 取得
	while (TRUE) {
		int sqlrc = clsDB.Fetch();
		if (sqlrc == KIZUODBC_FETCH_OK) {					// データ有り

			// 主キー
			clsDB.GetData(1, &id);
			if( MAX_PCODE < id || 0 >= id ) {
				// 主キー不正
				LOG(em_ERR), "[%s] [%s 主キー不正 id=%d]", my_sThreadName, cDbName, id);
				syslog(SYSNO_DB_DATA_ERR, "[%s id=%d]", cDbName, id);					
				continue;
			}
			id -= 1;										// 0オリジンに変更する


			// データ部
			clsDB.GetData(2, &buf.rec[id].bUmu );


			//// 優先度順に格納
			buf.nPriority[ buf.nMasterNum ] = id;
			buf.nMasterNum++;

		} else if( sqlrc == KIZUODBC_FETCH_NODATE ) {		// データ無し
			break;	
		} else {								// エラー
			LOG(em_ERR), "[%s] DB FETCH_ERR [%s]", my_sThreadName, cDbName);
			LOG(em_NON), "%s", sql);
			syslog(SYSNO_DB_FETCH_ERR, "[%s]", cDbName);
			return false;
		}
	}

	////////////////////////////////////
	// 条件設定
	////////////////////////////////////
	cDbName = DB_PCODE_JUDGE;
	//// SQL文作成
	sql = "SELECT";
	sql += " pcode, 条件No, 有効判定";

	sql += " ,命令No";											// 4.命令No
	sql += " ,生産No";											// 5.命令No
	sql += " ,ISNULL(注文板厚下限, 0), ISNULL(注文板厚上限, 0)";// 6.注文板厚下限[μm] 7.注文板厚上限[μm]
	sql += " ,検査基準";										// 8.検査基準
	sql += " ,向先";											// 9.向先
	sql += " ,用途";											// 10.用途
	sql += " ,主材質";											// 11.主材質
	sql += " ,表面処理1桁目";									// 12.表面処理1桁目
	sql += " ,品種";											// 13.品種

	sql += " FROM ";
	sql += cDbName;
	sql += " WHERE 有効判定=1";		// 転送量削減対策
	sql += " ORDER BY pcode, 条件No";

	//// 実行
	if( !clsDB.ExecuteDirect(sql)) {
		LOG(em_ERR), "[%s] DB_EXECUTEDIRECT [%s]", my_sThreadName, cDbName);
		LOG(em_NON), "%s", sql);
		syslog(SYSNO_DB_EXECUTEDIRECT_ERR, "[%s]", cDbName);
		return false;
	}

	//// 取得
	while (TRUE) {
		int sqlrc = clsDB.Fetch();
		if (sqlrc == KIZUODBC_FETCH_OK) {					// データ有り

			// 主キー
			clsDB.GetData(1, &id);
			clsDB.GetData(2, &no);
			if( (MAX_PCODE < id || 0 >= id) || (MAX_CODE_JUDGE < no || 0 >= no) ) {
				// 主キー不正
				LOG(em_ERR), "[%s] [%s 主キー不正 id=%d no=%d]", my_sThreadName, cDbName, id, no);
				syslog(SYSNO_DB_DATA_ERR, "[%s id=%d no=%d]", cDbName, id, no);
				continue;
			}
			id -= 1;										// 0オリジンに変更する
			no -= 1;										// 0オリジンに変更する
			clsDB.GetData(3, &buf.rec[id].typJudge[no].bUmu );

			// データ部
			clsDB.GetData(4, &buf.rec[id].typJudge[no].cMeiNo, 32);			// 命令No
			clsDB.GetData(5, &buf.rec[id].typJudge[no].cCoilNo, 32);		// 生産No
			clsDB.GetData(6, &buf.rec[id].typJudge[no].nAtuSei[0]);			// 注文板厚下限
			clsDB.GetData(7, &buf.rec[id].typJudge[no].nAtuSei[1]);			// 注文板厚上限
			clsDB.GetData(8, &buf.rec[id].typJudge[no].cKenKijun, 32);		// 検査基準
			clsDB.GetData(9, &buf.rec[id].typJudge[no].cMuke, 32);			// 向先
			clsDB.GetData(10, &buf.rec[id].typJudge[no].cYouto, 32);		// 用途
			clsDB.GetData(11, &buf.rec[id].typJudge[no].cSyuZai, 32);		// 主材質
			clsDB.GetData(12, &buf.rec[id].typJudge[no].cOmoteSyori1, 32);	// 表面処理1桁目
			clsDB.GetData(13, &buf.rec[id].typJudge[no].cHinCode,32);		// 品種

		} else if( sqlrc == KIZUODBC_FETCH_NODATE ) {		// データ無し
			break;	
		} else {								// エラー
			LOG(em_ERR), "[%s] DB FETCH_ERR [%s]", my_sThreadName, cDbName);
			LOG(em_NON), "%s", sql);
			syslog(SYSNO_DB_FETCH_ERR, "[%s]", cDbName);
			return false;
		}
	}

	LOG(em_MSG), "[%s] %s 取得完了", my_sThreadName, cDbName);

	return true;
}

