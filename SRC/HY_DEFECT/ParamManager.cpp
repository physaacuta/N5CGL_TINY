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
ThreadManager("ParamMgr")
{
	//// ハンドル生成
}

//------------------------------------------
// デストラクタ
//------------------------------------------
ParamManager::~ParamManager(void)
{
	//// ハンドル解放
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
// スレッドイベント発生
// int nEventNo イベントNo (0オリジン)
//------------------------------------------
void ParamManager::ThreadEvent(int nEventNo)
{
	////// シグナルの条件
	enum {	EV_QUE = 0,						// スレッドキュー通知
	};

	////// シグナル条件分け
	switch (nEventNo) {

//-----------------------------------------------------------------------------------------------
	case EV_QUE: 							// スレッドキュー通知
		COMMON_QUE* pDeli;				// スレッドキュー受け渡し
		pDeli = gque_Deli.GetFromHead();
		ExecParam(pDeli);
		delete pDeli;
		break;
	
//-----------------------------------------------------------------------------------------------
	default :
		// ありえない！！
		LOG(em_ERR), "[%s] ThreadEvent EventNo NG=%d", my_sThreadName, nEventNo );
		return;
	}
}

//------------------------------------------
// パラメータ読込制御
// COMMON_QUE* pQue キュー受け渡し
//------------------------------------------
void ParamManager::ExecParam(COMMON_QUE* pQue)
{
	// 面倒なので全部読むに変更
	ReadParam_All(false);
}

// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// パラメータ読込


//------------------------------------------
// 全パラメータ読み込み
// bool init true 初期起動時のみ
//------------------------------------------
bool ParamManager::ReadParam_All(bool init)
{
	// 初期起動時のみ
	if(init) {
		if( ! Read_CommonMast() )	return false;
	}
	// それ以外
// 2024.07.26 Kouno 2CGL検査装置φω精度向上機能導入(長周期) ---------------->>>>>
	if( ! Read_CommonMast_LongPd() )   return false;
// 2024.07.26 Kouno 2CGL検査装置φω精度向上機能導入(長周期) <<<<<----------------
	if( ! Read_TypeName() )		return false;
	if( ! Read_GradeName() )	return false;
	if( ! Read_PcodeOutput() )	return false;

	// 前工程関連
	//if( ! Read_MaeMaster() )	return false;
	//if( ! Read_MaeTypeName() )	return false;
	//if( ! Read_MaeGradeName() )	return false;

	return true;
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
	sql += " 探傷画面表示区間小, 探傷画面表示区間大";
	sql += " FROM ";
	sql += cDbName;

	//// 実行
	if( !clsDB.ExecuteDirect(sql)) {
		LOG(em_ERR), "[%s] SQL実行エラー", my_sThreadName);
		LOG(em_NON), "%s", sql);
		syslog(SYSNO_DB_EXECUTEDIRECT_ERR, "[%s]", cDbName);
		return false;
	}

	//// 取得
	while (TRUE) {
		int sqlrc = clsDB.Fetch();
		if (sqlrc == KIZUODBC_FETCH_OK) {					// データ有り
	
			// データ部
			buf.nMapDsp[0] = MAP_REPRES_MIN;					// システム最小は、インクルードより取得
			clsDB.GetData(1, &buf.nMapDsp[1]);
			clsDB.GetData(2, &buf.nMapDsp[2]);

		} else if( sqlrc == KIZUODBC_FETCH_NODATE ) {		// データ無し
			break;	
		} else {								// エラー
			syslog(SYSNO_DB_FETCH_ERR, "[%s]", cDbName);
			LOG(em_ERR), "[%s] FETCH ERR SQL", my_sThreadName);
			LOG(em_NON), "%s", sql);
			return false;
		}
	}

	//// 取得データをメンバー変数にセット
	memcpy( &p, &buf, sizeof(buf));
	LOG(em_MSG), "[%s] %s 取得完了", my_sThreadName, cDbName);
	return true;
}

// 2024.07.26 Kouno 2CGL検査装置φω精度向上機能導入(長周期) ---------------->>>>>
//------------------------------------------
// 疵検定数パラメータ（長周期関係）読込
//------------------------------------------
bool ParamManager::Read_CommonMast_LongPd()
{
	CString sql;					// SQL文


	//// パラメータ初期化
	CString cDbName;
	cDbName = DB_COMMON_MASTER;
	P_COMMON_MAST_LONGPD&	p = m_CommonMast_LongPd;
	P_COMMON_MAST_LONGPD	buf;
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
	sql += " LongPd_GuidanceAvailable ";
	sql += " FROM ";
	sql += cDbName;

	//// 実行
	if( !clsDB.ExecuteDirect(sql)) {
		LOG(em_ERR), "[%s] SQL実行エラー", my_sThreadName);
		LOG(em_NON), "%s", sql);
		syslog(SYSNO_DB_EXECUTEDIRECT_ERR, "[%s]", cDbName);
		return false;
	}

	//// 取得
	while (TRUE) {
		int sqlrc = clsDB.Fetch();
		if (sqlrc == KIZUODBC_FETCH_OK) {					// データ有り
			clsDB.GetData(1, &buf.nLongPd_GuidanceAvailable);
		} else if( sqlrc == KIZUODBC_FETCH_NODATE ) {		// データ無し
			break;	
		} else {								// エラー
			syslog(SYSNO_DB_FETCH_ERR, "[%s]", cDbName);
			LOG(em_ERR), "[%s] FETCH ERR SQL", my_sThreadName);
			LOG(em_NON), "%s", sql);
			return false;
		}
	}

	//// 取得データをメンバー変数にセット
	memcpy( &p, &buf, sizeof(buf));
	LOG(em_MSG), "[%s] %s 長周期関係 取得完了", my_sThreadName, cDbName);
	return true;
}
// 2024.07.26 Kouno 2CGL検査装置φω精度向上機能導入(長周期) <<<<<----------------

//------------------------------------------
// 疵種パラメータ読込
//------------------------------------------
bool ParamManager::Read_TypeName()
{
	CString sql;					// SQL文
	int id;
	int imgidx;

	//// パラメータ初期化
	CString cDbName;
	cDbName = DB_TYPE_NAME;
	P_TYPE_NAME&	p = m_TypeName;
	P_TYPE_NAME		buf;
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
    // 切り出し画像はNULLが設定されていたら、切り出し画像種類1=1 / 切り出し画像種類2=2
    // として設定する
	sql =  "SELECT";
	sql += " ID, EdasysID, 優先度";
	sql += ", ISNULL(切出画像種類1,1), ISNULL(切出画像種類2,2)";
	sql += " FROM ";
	sql += cDbName;
	sql += " ORDER BY 優先度 ASC ";

	//// 実行
	if( !clsDB.ExecuteDirect(sql)) {
		LOG(em_ERR), "[%s] SQL実行エラー", my_sThreadName);
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
			if( MAX_TYPE <= id || 0 > id ) {
				// 主キー不正
				LOG(em_ERR), "[%s] [%s 主キー不正 id=%d]", my_sThreadName, cDbName, id);
				syslog(SYSNO_DB_DATA_ERR, "[%s id=%d]", cDbName, id);					
				continue;
			}
			
			// データ部
			clsDB.GetData(2, &buf.type_id[buf.num].edasysID);
			clsDB.GetData(3, &buf.type_id[buf.num].priority);

			clsDB.GetData(4, &imgidx );
			if(0<imgidx || imgidx<=MAX_IMAGE_CNT) buf.type_id[buf.num].bEnable[ imgidx-1 ] = true; 

			clsDB.GetData(5, &imgidx );
			if(0<imgidx || imgidx<=MAX_IMAGE_CNT) buf.type_id[buf.num].bEnable[ imgidx-1 ] = true; 

			buf.num++;

		} else if( sqlrc == KIZUODBC_FETCH_NODATE ) {		// データ無し
			break;	
		} else {								// エラー
			LOG(em_ERR), "[%s] FETCH ERR SQL", my_sThreadName);
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
// Grパラメータ読込
//------------------------------------------
bool ParamManager::Read_GradeName()
{
	CString sql;					// SQL文
	int id;

	//// パラメータ初期化
	CString cDbName;
	cDbName = DB_GRADE_NAME;
	P_GRADE_NAME&	p = m_GradeName;
	P_GRADE_NAME	buf;
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
	sql += " ID, EdasysID, 優先度";
	sql += " FROM ";
	sql += cDbName;
	sql += " ORDER BY 優先度 ASC ";

	//// 実行
	if( !clsDB.ExecuteDirect(sql)) {
		LOG(em_ERR), "[%s] SQL実行エラー", my_sThreadName);
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
			if( MAX_GRADE <= id || 0 > id ) {
				// 主キー不正
				LOG(em_ERR), "[%s] [%s 主キー不正 id=%d]", my_sThreadName, cDbName, id);
				syslog(SYSNO_DB_DATA_ERR, "[%s id=%d]", cDbName, id);					
				continue;
			}
			
			// データ部
			clsDB.GetData(2, &buf.grade_id[buf.num].edasysID);
			clsDB.GetData(3, &buf.grade_id[buf.num].priority);
			buf.num++;

		} else if( sqlrc == KIZUODBC_FETCH_NODATE ) {		// データ無し
			break;	
		} else {								// エラー
			LOG(em_ERR), "[%s] FETCH ERR SQL", my_sThreadName);
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
// 検査パターン別パラメータ読込
//------------------------------------------
bool ParamManager::Read_PcodeOutput()
{
	CString sql;					// SQL文
	int id;
	int no;

	//// パラメータ初期化
	CString cDbName;
	P_PCODE_OUTPUT&	p = m_PcodeOutput;
	P_PCODE_OUTPUT		buf;
	memset(&buf, 0x00, sizeof(buf));


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

	//// 実行
	if( !clsDB.ExecuteDirect(sql)) {
		LOG(em_ERR), "[%s] SQL実行エラー", my_sThreadName);
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
			id = id -1;		// 使いやすいように0オリジンに変更


			// -----------------
			// データ部
			clsDB.GetData(2, &buf.rec[id].umu);

		} else if( sqlrc == KIZUODBC_FETCH_NODATE ) {		// データ無し
			break;	
		} else {								// エラー
			LOG(em_ERR), "[%s] FETCH ERR SQL", my_sThreadName);
			LOG(em_NON), "%s", sql);
			syslog(SYSNO_DB_FETCH_ERR, "[%s]", cDbName);
			return false;
		}
	}

	////////////////////////////////////
	// 出力条件
	////////////////////////////////////
	int typid;	
	no = 0;
	cDbName = DB_PCODE_OUTPUT;
	//// SQL文作成
	sql =  "SELECT";
	sql += " pcode";
	sql += ", 疵種EdasysID, 探傷画面表示_表, 探傷画面表示_裏 ";
	sql += " FROM ";
	sql += cDbName;

	//// 実行
	if( !clsDB.ExecuteDirect(sql)) {
		LOG(em_ERR), "[%s] SQL実行エラー", my_sThreadName);
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
			id = id -1;		// 使いやすいように0オリジンに変更

			// -----------------
			clsDB.GetData(2, &typid);
			if(-1 == typid) {								// デフォルト
				buf.rec[id].defput.edasysID = typid;

				clsDB.GetData(3, (int*)&buf.rec[id].defput.kind[OUTPUTFLG_DSP_O]);
				clsDB.GetData(4, (int*)&buf.rec[id].defput.kind[OUTPUTFLG_DSP_U]);
			} else {										// 通常
				no = buf.rec[id].nPutNum;
				buf.rec[id].output[no].edasysID = typid;

				clsDB.GetData(3, (int*)&buf.rec[id].output[no].kind[OUTPUTFLG_DSP_O]);
				clsDB.GetData(4, (int*)&buf.rec[id].output[no].kind[OUTPUTFLG_DSP_U]);

				buf.rec[id].nPutNum = no + 1;
			}

		} else if( sqlrc == KIZUODBC_FETCH_NODATE ) {		// データ無し
			break;	
		} else {								// エラー
			LOG(em_ERR), "[%s] FETCH ERR SQL", my_sThreadName);
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

////------------------------------------------
//// 前工程マスターテーブル読込
////------------------------------------------
//bool ParamManager::Read_MaeMaster()
//{
//	int	nRet;							// リターンコード
//
//	CString	sSql;						// SQL文
//	CString	sTbl;						// テーブル名
//
//	P_MAE_MASTER	typTmp;				// 前工程マスター
//
//	OdbcBase	clsDB;					// ODBCクラス
//
//	// 初期化
//	memset( &typTmp, 0x00, sizeof(typTmp) );
//
//	// データベース接続
//	if( false == clsDB.Connect(TASKINI_NAME) ){				// データベース接続エラー
//		LOG(em_ERR), "[%s] DB接続エラー", my_sThreadName);
//		syslog(SYSNO_DB_CONNECT_ERR, "");
//		return false;
//	}
//
//	// テーブル名
//	sTbl	= DB_MAE_MASTER;								// 前工程マスター
//
//	// SQL文作成
//	sSql.Format("SELECT 前工程コード, 前工程表示モード, 画像形式 FROM %s WHERE 有効判定=1", sTbl);
//
//	// SQL実行
//	if( false == clsDB.ExecuteDirect(sSql) ){
//		LOG(em_ERR), "[%s] SQL実行エラー %s", my_sThreadName, sSql);
//		syslog(SYSNO_DB_EXECUTEDIRECT_ERR, "[%s]", sTbl);
//		return false;
//	}
//
//	// テーブルデータ取得
//	while( TRUE ){											// 有効レコード分
//		nRet	= clsDB.Fetch();							// レコード参照
//		if( KIZUODBC_FETCH_OK == nRet ){					// レコード有り
//			if( typTmp.nRec < MAX_MAE_LINE ){				// 有効登録数
//				// レコードデータ取得
//															// 前工程コード
//				clsDB.GetData(1, typTmp.typRec[typTmp.nRec].cMaeCode, sizeof(typTmp.typRec[typTmp.nRec].cMaeCode));
//															// 前工程表示モード
//				clsDB.GetData(2, &typTmp.typRec[typTmp.nRec].nMaeDsp);
//															// 画像形式
//				clsDB.GetData(3, &typTmp.typRec[typTmp.nRec].nImgFmt);
//				typTmp.nRec++;								// レコード数更新
//			}
//		} else if( KIZUODBC_FETCH_NODATE == nRet ){			// レコード無し
//			break;
//		} else {											// 参照エラー
//			LOG(em_ERR), "[%s] FETCH ERR SQL[%s]", my_sThreadName, sSql);
//			syslog(SYSNO_DB_FETCH_ERR, "[%s]", sTbl);
//			return false;
//		}
//	}
//
//	// 読み込みデータ格納
//	memcpy(&m_MaeMaster, &typTmp, sizeof(typTmp));
//	LOG(em_MSG), "[%s] %s 取得完了 件数[%d]", my_sThreadName, sTbl, typTmp.nRec);
//
//	return true;
//}
//
////------------------------------------------
//// 前工程疵種マスターテーブル読込
////------------------------------------------
//bool ParamManager::Read_MaeTypeName()
//{
//	int	nRet;							// リターンコード
//
//	CString	sSql;						// SQL文
//	CString	sTbl;						// テーブル名
//
//	P_MAE_TYPE_NAME	typTmp;				// 前工程疵種マスターテーブル
//
//	OdbcBase	clsDB;					// ODBCクラス
//
//	// 初期化
//	memset( &typTmp, 0x00, sizeof(typTmp) );
//
//	// データベース接続
//	if( false == clsDB.Connect(TASKINI_NAME) ){				// データベース接続エラー
//		LOG(em_ERR), "[%s] DB接続エラー", my_sThreadName);
//		syslog(SYSNO_DB_CONNECT_ERR, "");
//		return false;
//	}
//
//	// テーブル名
//	sTbl	= DB_MAE_TYPE_NAME;								// 前工程疵種マスター
//
//	// SQL文作成
//	sSql.Format("SELECT 前工程コード, FFName, ID, 優先度, 表示有無 FROM %s", sTbl);
//
//	// SQL実行
//	if( false == clsDB.ExecuteDirect(sSql) ){
//		LOG(em_ERR), "[%s] SQL実行エラー %s", my_sThreadName, sSql);
//		syslog(SYSNO_DB_EXECUTEDIRECT_ERR, "[%s]", sTbl);
//		return false;
//	}
//
//	// テーブルデータ取得
//	while( TRUE ){											// 有効レコード分
//		nRet	= clsDB.Fetch();							// レコード参照
//		if( KIZUODBC_FETCH_OK == nRet ){					// レコード有り
//			if( typTmp.nRec < MAX_MAE_TYPE ){				// 有効レコード数
//				// レコードデータ取得
//															// 前工程コード
//				clsDB.GetData(1, typTmp.typRec[typTmp.nRec].cMaeCode, sizeof(typTmp.typRec[typTmp.nRec].cMaeCode));
//															// FFName
//				clsDB.GetData(2, typTmp.typRec[typTmp.nRec].cFFName, sizeof(typTmp.typRec[typTmp.nRec].cFFName));
//				KizuFunction::Trim(typTmp.typRec[typTmp.nRec].cFFName);
//															// ID
//				clsDB.GetData(3, &typTmp.typRec[typTmp.nRec].nTypeID);
//															// 優先度
//				clsDB.GetData(4, &typTmp.typRec[typTmp.nRec].nPriority);
//															// 表示有無
//				clsDB.GetData(5, &typTmp.typRec[typTmp.nRec].nDispID);
//
//				typTmp.nRec++;								// レコード数更新
//			}
//		} else if( KIZUODBC_FETCH_NODATE == nRet ){			// レコード無し
//			break;
//		} else {											// 参照エラー
//			LOG(em_ERR), "[%s] FETCH ERR SQL[%s]", my_sThreadName, sSql);
//			syslog(SYSNO_DB_FETCH_ERR, "[%s]", sTbl);
//			return false;
//		}
//	}
//
//	// 読み込みデータ格納
//	memcpy(&m_MaeTypeName, &typTmp, sizeof(typTmp));
//	LOG(em_MSG), "[%s] %s 取得完了 件数[%d]", my_sThreadName, sTbl, typTmp.nRec);
//
//	return true;
//}
//
////------------------------------------------
//// 前工程疵グレードマスターテーブル読込
////------------------------------------------
//bool ParamManager::Read_MaeGradeName()
//{
//	int	nRet;							// リターンコード
//
//	CString	sSql;						// SQL文
//	CString	sTbl;						// テーブル名
//
//	P_MAE_GRADE_NAME	typTmp;			// 前工程疵グレードマスターテーブル
//
//	OdbcBase	clsDB;					// ODBCクラス
//
//	// 初期化
//	memset( &typTmp, 0x00, sizeof(typTmp) );
//
//	// データベース接続
//	if( false == clsDB.Connect(TASKINI_NAME) ){				// データベース接続エラー
//		LOG(em_ERR), "[%s] DB接続エラー", my_sThreadName);
//		syslog(SYSNO_DB_CONNECT_ERR, "");
//		return false;
//	}
//
//	// テーブル名
//	sTbl	= DB_MAE_GRADE_NAME;							// 前工程疵グレードマスター
//
//	// SQL文作成
//	sSql.Format("SELECT 前工程コード, FFName, グレードID, 優先度 FROM %s", sTbl);
//
//	// SQL実行
//	if( false == clsDB.ExecuteDirect(sSql) ){
//		LOG(em_ERR), "[%s] SQL実行エラー %s", my_sThreadName, sSql);
//		syslog(SYSNO_DB_EXECUTEDIRECT_ERR, "[%s]", sTbl);
//		return false;
//	}
//
//	// テーブルデータ取得
//	while( TRUE ){											// 有効レコード分
//		nRet	= clsDB.Fetch();							// レコード参照
//		if( KIZUODBC_FETCH_OK == nRet ){					// レコード有り
//			if( typTmp.nRec < MAX_MAE_GRADE ){				// 有効レコード数
//				// レコードデータ取得
//															// 前工程コード
//				clsDB.GetData(1, typTmp.typRec[typTmp.nRec].cMaeCode, sizeof(typTmp.typRec[typTmp.nRec].cMaeCode));
//															// FFName
//				clsDB.GetData(2, typTmp.typRec[typTmp.nRec].cFFName, sizeof(typTmp.typRec[typTmp.nRec].cFFName));
//				KizuFunction::Trim(typTmp.typRec[typTmp.nRec].cFFName);
//															// ID
//				clsDB.GetData(3, &typTmp.typRec[typTmp.nRec].nGradeID);
//															// 優先度
//				clsDB.GetData(4, &typTmp.typRec[typTmp.nRec].nPriority);
//				typTmp.nRec++;								// レコード数更新
//			}
//		} else if( KIZUODBC_FETCH_NODATE == nRet ){			// レコード無し
//			break;
//		} else {											// 参照エラー
//			LOG(em_ERR), "[%s] FETCH ERR SQL[%s]", my_sThreadName, sSql);
//			syslog(SYSNO_DB_FETCH_ERR, "[%s]", sTbl);
//			return false;
//		}
//	}
//
//	// 読み込みデータ格納
//	memcpy(&m_MaeGradeName, &typTmp, sizeof(typTmp));
//	LOG(em_MSG), "[%s] %s 取得完了 件数[%d]", my_sThreadName, sTbl, typTmp.nRec);
//
//	return true;
//}

// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 出力条件チェック
//------------------------------------------
// 疵種EdasysID の有無
// long tid 疵種EdasysID
// 戻り値 復帰情報
//------------------------------------------
bool ParamManager::CheckEdasysID_T(long tid) const 
{
	if(0==tid) return false;
	for(int ii=0; ii<m_TypeName.num; ii++) {
		if( m_TypeName.type_id[ii].edasysID == tid ) return true;
	}
	return false;
}
//------------------------------------------
// グレードEdasysID の有無
// long tid グレードEdasysID
// 戻り値 復帰情報
//------------------------------------------
bool ParamManager::CheckEdasysID_G(long gid) const 
{
	if(0==gid) return false;
	for(int ii=0; ii<m_GradeName.num; ii++) {
		if( m_GradeName.grade_id[ii].edasysID == gid ) return true;
	}
	return false;
}

//------------------------------------------
// 疵種優先度取得
// long tid 疵種EdasysID
// 戻り値 優先度 (未検出-1)
//------------------------------------------
int ParamManager::GetYusenT(long tid) const 
{
	for(int ii=0; ii<m_TypeName.num; ii++) {
		if( m_TypeName.type_id[ii].edasysID == tid ) return m_TypeName.type_id[ii].priority;
	}
	return -1;
}

//------------------------------------------
// グレード優先度取得
// long gid グレードEdasysID
// 戻り値 優先度 (未検出-1)
//------------------------------------------
int ParamManager::GetYusenG(long gid) const
{
	for(int ii=0; ii<m_GradeName.num; ii++) {
		if( m_GradeName.grade_id[ii].edasysID == gid ) return m_GradeName.grade_id[ii].priority;
	}
	return -1;
}

////------------------------------------------
//// 前工程疵種優先度取得
//// char*	cProc	前工程コード
//// int		nTid	疵種EdasysID
//// 戻り値	優先度 (未検出-1)
////------------------------------------------
//int ParamManager::GetMaeYusenT(char* cProc, int nTid) const
//{
//	int	nPriority	= -1;				// 優先度
//
//	// 前工程疵種検索
//	for(int ii=0; ii<m_MaeTypeName.nRec; ii++){
//		if( 0 == memcmp(cProc, m_MaeTypeName.typRec[ii].cMaeCode, SIZE_KOUTEI_CODE) ){
//			if( nTid == m_MaeTypeName.typRec[ii].nTypeID ){
//				nPriority	= m_MaeTypeName.typRec[ii].nPriority;
//				break;
//			}
//		}
//	}
//
//	return nPriority;
//}
//
////------------------------------------------
//// 前工程グレード優先度取得
//// char*	cProc	前工程コード
//// int		nGid	グレードEdasysID
//// 戻り値	優先度 (未検出-1)
////------------------------------------------
//int ParamManager::GetMaeYusenG(char* cProc, int nGid) const
//{
//	int	nPriority	= -1;				// 優先度
//
//	// 前工程グレード検索
//	for(int ii=0; ii<m_MaeGradeName.nRec; ii++){
//		if( 0 == memcmp(cProc, m_MaeGradeName.typRec[ii].cMaeCode, SIZE_KOUTEI_CODE) ){
//			if( nGid == m_MaeGradeName.typRec[ii].nGradeID ){
//				nPriority	= m_MaeGradeName.typRec[ii].nPriority;
//				break;
//			}
//		}
//	}
//
//	return nPriority;
//}

//------------------------------------------
// 出力対象の欠陥か判定
// EM_OUTPUTFLG output どの出力条件をチェックするの？
// long pcode	検査パターン (0オリジン)
// long tid		疵種EdasysID
// long gid		グレードEdasysID
// 戻り値 true:対象
//------------------------------------------
bool ParamManager::Check_Output(EM_OUTPUTFLG output, long pcode, long tid, long gid) const
{
	// 引数チェック
	if( 0 > pcode || MAX_PCODE <= pcode ) {
		LOG(em_ERR), "[%s] output=%d pcodeNG=%d", my_sThreadName, output, pcode);	
		return false;
	}

	//// 疵種EdasysID 指定有り？
	for(int ii=0; ii<m_PcodeOutput.rec[pcode].nPutNum; ii++) {
		if( tid == m_PcodeOutput.rec[pcode].output[ii].edasysID ) {
			// Gr設定無しの疵種の為 全部対象外
			if( 0 == m_PcodeOutput.rec[pcode].output[ii].kind[output] ) return false;

			// Gr設定有りの疵種の為 優先度比較
			if( GetYusenG(gid) <= GetYusenG(m_PcodeOutput.rec[pcode].output[ii].kind[output]) )	return true;
			else																				return false;
		}
	}

	//// 無いときは デフォルト指定 (ココまで来た場合は、疵種設定無しの時)
	// Gr設定無しの疵種の為 全部対象外
	if( 0 == m_PcodeOutput.rec[pcode].defput.kind[output] ) return false;

	// Gr設定有りの疵種の為 優先度比較
	if( GetYusenG(gid) <= GetYusenG(m_PcodeOutput.rec[pcode].defput.kind[output]) )	return true;
	else																			return false;
}
//------------------------------------------
// 疵種に対して色差が表示かチェック
// long tid		疵種EdasysID
// int index	色差 (0オリジン)
// 戻り値 true:対象
//------------------------------------------
bool ParamManager::Check_ImgDsp(long tid, int index) const
{

	//// 通常
	//return true;

	// 色差
	if(0==tid) return false;
	if(index>=MAX_IMAGE_CNT) return false;
	for(int ii=0; ii<m_TypeName.num; ii++) {
		if( m_TypeName.type_id[ii].edasysID == tid ) {
			return m_TypeName.type_id[ii].bEnable[ index ];
		}
	}

	return false;
}

////------------------------------------------
//// 疵種ID取得
//// char*	cProc	前工程コード
//// char*	cType	前工程疵種
//// 戻り値	疵種ID
////------------------------------------------
//int ParamManager::GetMaeTID(char* cProc, char* cType)
//{
//	int	nTypeID	= 0;
//	char	cName[SIZE_FFNAME_TYPE+1];
//
//	memset(cName, 0x00, sizeof(cName));
//	memcpy(cName, cType, SIZE_FFNAME_TYPE);
//	KizuFunction::Trim(cName);
//
//	// 前工程疵種検索
//	for(int ii=0; ii<m_MaeTypeName.nRec; ii++){
//		// 疵種一致
//		if( 0 == memcmp(cProc, m_MaeTypeName.typRec[ii].cMaeCode, SIZE_KOUTEI_CODE)
//		&&  0 == memcmp(cName, m_MaeTypeName.typRec[ii].cFFName,  SIZE_FFNAME_TYPE) ){
//			nTypeID	= m_MaeTypeName.typRec[ii].nTypeID;
//			break;
//		}
//	}
//
//	return	nTypeID;
//}
//
////------------------------------------------
//// グレードID取得
//// char*	cProc	前工程コード
//// char*	cGrade	前工程グレード
//// 戻り値	グレードID
////------------------------------------------
//int ParamManager::GetMaeGID(char* cProc, char* cGrade)
//{
//	int	nGradeID	= 0;
//	char	cName[SIZE_FFNAME_GRADE+1];
//
//	memset(cName, 0x00, sizeof(cName));
//	memcpy(cName, cGrade, SIZE_FFNAME_GRADE);
//	KizuFunction::Trim(cName);
//
//	// 前工程グレード検索
//	for(int ii=0; ii<m_MaeGradeName.nRec; ii++){
//		// グレード一致
//		if( 0 == memcmp(cProc, m_MaeGradeName.typRec[ii].cMaeCode, SIZE_KOUTEI_CODE)
//		&&  0 == memcmp(cName, m_MaeGradeName.typRec[ii].cFFName,  SIZE_FFNAME_GRADE) ){
//			nGradeID	= m_MaeGradeName.typRec[ii].nGradeID;
//			break;
//		}
//	}
//
//	return	nGradeID;
//}
//
////------------------------------------------
//// 表示対象疵判定
//// char*	cProc	前工程コード
//// char*	cType	前工程疵種
//// char*	cGrade	前工程グレード
//// 戻り値	判定結果(true=表示対象、false=表示対象外)
////------------------------------------------
//bool ParamManager::CheckDispDef(char* cProc, char* cType, char* cGrade)
//{
//	bool	bDisp		= false;
//	int		nDispID		= 0;
//	int		nGradeID	= 0;
//	char	cName[SIZE_FFNAME_TYPE+1];
//
//	memset(cName, 0x00, sizeof(cName));
//	memcpy(cName, cType, SIZE_FFNAME_TYPE );
//	KizuFunction::Trim(cName);
//
//	// グレードID取得
//	nGradeID	= GetMaeGID(cProc, cGrade);
//
//	// 前工程疵種検索
//	for(int ii=0; ii<m_MaeTypeName.nRec; ii++){
//		// 疵種一致
//		if( 0 == memcmp(cProc, m_MaeTypeName.typRec[ii].cMaeCode, SIZE_KOUTEI_CODE)
//		&&  0 == memcmp(cName, m_MaeTypeName.typRec[ii].cFFName,  SIZE_FFNAME_TYPE) ){
//			// 表示有無取得
//			nDispID	= m_MaeTypeName.typRec[ii].nDispID;
//			// 表示有
//			if( 0 < nDispID ){
//				// グレード優先度判定
//				if( GetMaeYusenG(cProc, nGradeID) <= GetMaeYusenG(cProc, nDispID) ){
//					// 表示対象疵
//					bDisp	= true;
//				}
//			}
//			break;
//		}
//	}
//
//	return	bDisp;
//}