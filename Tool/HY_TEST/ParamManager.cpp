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
	if( ! Read_TypeName() )		return false;
	if( ! Read_GradeName() )	return false;
	if( ! Read_PcodeOutput() )	return false;
	if( ! Read_PallOutput() )	return false;

	// 前工程傷関連
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
		return false;
	}

	//// SQL文作成
	sql =  "SELECT";
	sql += " 探傷画面表示区間小, 探傷画面表示区間大";
	sql += " ,設備長_検査基点_上流カメラ";
	sql += " ,設備長_検査基点_SPM";
	sql += " ,設備長_表示基点_裏検査台";
	sql += " ,設備長_表示基点_表検査台";
	sql += " ,設備長_表示基点_シャー";
	sql += " FROM ";
	sql += cDbName;

	//// 実行
	if( !clsDB.ExecuteDirect(sql)) {
		LOG(em_ERR), "[%s] SQL実行エラー %s", my_sThreadName, sql);
		return false;
	}

	//// 取得
	while (TRUE) {
		int sqlrc = clsDB.Fetch();
		if (sqlrc == KIZUODBC_FETCH_OK) {					// データ有り
	
			// データ部
			buf.nMapDsp[0] = MAP_REPRES_MIN;					// システム最小は、インクルードより取得
			clsDB.GetData(1, &buf.nMapDsp[1] );
			clsDB.GetData(2, &buf.nMapDsp[2]);
			clsDB.GetData(3, &buf.nBaseToCamera);
			clsDB.GetData(4, &buf.nBaseToSpm);
			clsDB.GetData(5, &buf.nDspToUraCamera);
			clsDB.GetData(6, &buf.nDspToOmoteCamera);
			clsDB.GetData(7, &buf.nDspToCut);

		} else if( sqlrc == KIZUODBC_FETCH_NODATE ) {		// データ無し
			break;	
		} else {								// エラー
			LOG(em_ERR), "[%s] %s", my_sThreadName, sql);
			return false;
		}
	}
	
	// 5CGL
	buf.nSpmToUraCamera  = 82296;		// SPM ～ 裏検査台[mm]
	buf.nCameraToDsp = 276639;			// カメラ位置 ～ 表示基点[mm]
	buf.nCameraToCamera = 23594;		// 裏検査位置 ～ 表検査位置[mm]
	buf.nDeflistFilter = 0;				// 重欠陥一覧画面_フィルタ

	//// 取得データをメンバー変数にセット
	memcpy( &p, &buf, sizeof(buf));
	LOG(em_MSG), "[%s] %s 取得完了", my_sThreadName, cDbName);
	return true;
}

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
		return false;
	}

	//// SQL文作成
	sql =  "SELECT";
	sql += " ID, EdasysID, 優先度";
	sql += ", ISNULL(切出画像種類1,1), ISNULL(切出画像種類2,2)";
	sql += " FROM ";
	sql += cDbName;
	sql += " ORDER BY 優先度 ASC ";


	//// 実行
	if( !clsDB.ExecuteDirect(sql)) {
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
				continue;
			}
			
			// データ部
			clsDB.GetData(2, &buf.type_id[buf.num].edasysID );
			clsDB.GetData(3, &buf.type_id[buf.num].priority );

			clsDB.GetData(4, &imgidx );
			if(0<imgidx || imgidx<=MAX_IMAGE_CNT) buf.type_id[buf.num].bEnable[ imgidx-1 ] = true; 

			clsDB.GetData(5, &imgidx );
			if(0<imgidx || imgidx<=MAX_IMAGE_CNT) buf.type_id[buf.num].bEnable[ imgidx-1 ] = true; 

			buf.num++;

		} else if( sqlrc == KIZUODBC_FETCH_NODATE ) {		// データ無し
			break;	
		} else {								// エラー
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
		return false;
	}

	//// SQL文作成
	sql =  "SELECT";
	sql += " ID, EdasysID, 優先度, 欠陥グレード";
	sql += " FROM ";
	sql += cDbName;
	sql += " ORDER BY 優先度 ASC ";

	//// 実行
	if( !clsDB.ExecuteDirect(sql)) {
		//syslog(SYSNO_DB_EXECUTEDIRECT_ERR, "[%s]", cDbName);
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
				//syslog(SYSNO_DB_DATA_ERR, "[%s id=%d]", cDbName, id);					
				continue;
			}
			
			// データ部
			clsDB.GetData(2, &buf.grade_id[buf.num].edasysID );
			clsDB.GetData(3, &buf.grade_id[buf.num].priority );
			clsDB.GetData(4, &buf.grade_id[buf.num].nDefDiv );
			buf.num++;

		} else if( sqlrc == KIZUODBC_FETCH_NODATE ) {		// データ無し
			break;	
		} else {								// エラー
			//syslog(SYSNO_DB_FETCH_ERR, "[%s]", cDbName);
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
		//syslog(SYSNO_DB_CONNECT_ERR, "");
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
		//syslog(SYSNO_DB_EXECUTEDIRECT_ERR, "[%s]", cDbName);
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
				//syslog(SYSNO_DB_DATA_ERR, "[%s id=%d]", cDbName, id);					
				continue;
			}
			id = id -1;		// 使いやすいように0オリジンに変更


			// -----------------
			// データ部
			clsDB.GetData(2, &buf.rec[id].umu );

		} else if( sqlrc == KIZUODBC_FETCH_NODATE ) {		// データ無し
			break;	
		} else {								// エラー
			//syslog(SYSNO_DB_FETCH_ERR, "[%s]", cDbName);
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
	sql += " , 疵種EdasysID , 探傷画面表示_表, 探傷画面表示_裏 ";
	sql += " , 入側単位疵点マップ画面表示 ";
	sql += " FROM ";
	sql += cDbName;

	//// 実行
	if( !clsDB.ExecuteDirect(sql)) {
		//syslog(SYSNO_DB_EXECUTEDIRECT_ERR, "[%s]", cDbName);
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
				//syslog(SYSNO_DB_DATA_ERR, "[%s id=%d]", cDbName, id);					
				continue;
			}
			id = id -1;		// 使いやすいように0オリジンに変更

			// -----------------
			clsDB.GetData(2, &typid);
			if(-1 == typid) {								// デフォルト
				buf.rec[id].defput.edasysID = typid;

				clsDB.GetData(3, (int*)&buf.rec[id].defput.kind[OUTPUTFLG_DSP_O]);
				clsDB.GetData(4, (int*)&buf.rec[id].defput.kind[OUTPUTFLG_DSP_U]);
				clsDB.GetData(5, (int*)&buf.rec[id].defput.kind[OUTPUTFLG_MAP]);

			} else {										// 通常
				no = buf.rec[id].nPutNum;
				buf.rec[id].output[no].edasysID = typid;

				clsDB.GetData(3, (int*)&buf.rec[id].output[no].kind[OUTPUTFLG_DSP_O]);
				clsDB.GetData(4, (int*)&buf.rec[id].output[no].kind[OUTPUTFLG_DSP_U]);
				clsDB.GetData(5, (int*)&buf.rec[id].output[no].kind[OUTPUTFLG_MAP]);

				buf.rec[id].nPutNum = no + 1;
			}

		} else if( sqlrc == KIZUODBC_FETCH_NODATE ) {		// データ無し
			break;	
		} else {								// エラー
			//syslog(SYSNO_DB_FETCH_ERR, "[%s]", cDbName);
			return false;
		}
	}

	//// 取得データをメンバー変数にセット
	memcpy( &p, &buf, sizeof(buf));
	LOG(em_MSG), "[%s] %s 取得完了", my_sThreadName, cDbName);

	return true;
}

//------------------------------------------
// 検査パターン共通パラメータ読込
//------------------------------------------
bool ParamManager::Read_PallOutput()
{
	CString sql;					// SQL文
	int no;

	//// パラメータ初期化
	CString cDbName;
	P_PALL_OUTPUT&	p = m_PallOutput;
	P_PALL_OUTPUT		buf;
	memset(&buf, 0x00, sizeof(buf));


	//// ODBC接続クラス インスタンス生成
	OdbcBase clsDB;
	if (!clsDB.Connect(TASKINI_NAME)) {
		// データベース接続エラー
		LOG(em_ERR), "[%s] DB接続エラー", my_sThreadName);
		//syslog(SYSNO_DB_CONNECT_ERR, "");
		return false;
	}


	////////////////////////////////////
	// 出力条件
	////////////////////////////////////
	int typid;
	no = 0;
	cDbName = DB_PALL_OUTPUT;
	//// SQL文作成
	sql = "SELECT";
	sql += " 疵種EdasysID , 重欠陥一覧画面表示_詳細条件, 重欠陥一覧画面表示_簡易条件 ";
	sql += " FROM ";
	sql += cDbName;

	//// 実行
	if (!clsDB.ExecuteDirect(sql)) {
		//syslog(SYSNO_DB_EXECUTEDIRECT_ERR, "[%s]", cDbName);
		return false;
	}

	//// 取得
	while (TRUE) {
		int sqlrc = clsDB.Fetch();
		if (sqlrc == KIZUODBC_FETCH_OK) {					// データ有り

								// 主キー
								// -----------------
			clsDB.GetData(1, &typid);
			if (-1 == typid) {								// デフォルト
				buf.inf.defput.edasysID = typid;

				clsDB.GetData(2, (int*)&buf.inf.defput.kind[PALLOUT_DETAIL]);
				clsDB.GetData(3, (int*)&buf.inf.defput.kind[PALLOUT_SIMPLE]);

			}
			else {										// 通常
				no = buf.inf.nPutNum;
				buf.inf.output[no].edasysID = typid;

				clsDB.GetData(2, (int*)&buf.inf.output[no].kind[OUTPUTFLG_DSP_O]);
				clsDB.GetData(3, (int*)&buf.inf.output[no].kind[OUTPUTFLG_DSP_U]);

				buf.inf.nPutNum = no + 1;
			}

		}
		else if (sqlrc == KIZUODBC_FETCH_NODATE) {		// データ無し
			break;
		}
		else {								// エラー
											//syslog(SYSNO_DB_FETCH_ERR, "[%s]", cDbName);
			return false;
		}
	}

	//// 取得データをメンバー変数にセット
	memcpy(&p, &buf, sizeof(buf));
	LOG(em_MSG), "[%s] %s 取得完了", my_sThreadName, cDbName);

	return true;
}

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
// 戻り値 優先度 (未検出:0)
//------------------------------------------
int ParamManager::GetYusenT(long tid) const 
{
	for(int ii=0; ii<m_TypeName.num; ii++) {
		if( m_TypeName.type_id[ii].edasysID == tid ) return m_TypeName.type_id[ii].priority;
	}
	return 0;
}

//------------------------------------------
// グレード優先度取得
// long gid グレードEdasysID
// 戻り値 優先度 (未検出:0)
//------------------------------------------
int ParamManager::GetYusenG(long gid) const
{
	for(int ii=0; ii<m_GradeName.num; ii++) {
		if( m_GradeName.grade_id[ii].edasysID == gid ) return m_GradeName.grade_id[ii].priority;
	}
	return 0;
}

//------------------------------------------
// 欠陥区分取得
// long gid グレードEdasysID
// 戻り値 優先度 (未検出:0)
//------------------------------------------
int ParamManager::GetDefDivG(long gid) const
{
	for(int ii=0; ii<m_GradeName.num; ii++) {
		if( m_GradeName.grade_id[ii].edasysID == gid ) return m_GradeName.grade_id[ii].nDefDiv;
	}
	return 0;
}



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
// 出力対象の欠陥か判定
// EM_PALLOUT output どの出力条件をチェックするの？
// long tid		疵種EdasysID
// long gid		グレードEdasysID
// 戻り値 true:対象
//------------------------------------------
bool ParamManager::Check_PallOutput(EM_PALLOUT output, long tid, long gid) const
{
	//// 疵種EdasysID 指定有り？
	for (int ii = 0; ii<m_PallOutput.inf.nPutNum; ii++) {
		if (tid == m_PallOutput.inf.output[ii].edasysID) {
			// Gr設定無しの疵種の為 全部対象外
			if (0 == m_PallOutput.inf.output[ii].kind[output]) return false;

			// Gr設定有りの疵種の為 優先度比較
			if (GetYusenG(gid) <= GetYusenG(m_PallOutput.inf.output[ii].kind[output]))	return true;
			else																		return false;
		}
	}

	//// 無いときは デフォルト指定 (ココまで来た場合は、疵種設定無しの時)
	// Gr設定無しの疵種の為 全部対象外
	if (0 == m_PallOutput.inf.defput.kind[output]) return false;

	// Gr設定有りの疵種の為 優先度比較
	if (GetYusenG(gid) <= GetYusenG(m_PallOutput.inf.defput.kind[output]))	return true;
	else																	return false;
}
//------------------------------------------
// 疵種に対して各角度が表示かチェック
// long tid		疵種EdasysID
// int index	角度 (0オリジン)
// 戻り値 true:対象
//------------------------------------------
bool ParamManager::Check_ImgDsp(long tid, int index) const
{
	// 通常
	return true;

	// 色差
	if(0==tid) return false;
	for(int ii=0; ii<m_TypeName.num; ii++) {
		if( m_TypeName.type_id[ii].edasysID == tid ) {
			return m_TypeName.type_id[ii].bEnable[ index ];
		}
	}

	return false;
}


// ランダムでTidを取得する
int ParamManager::GetRandTid(int pcode)
{
	int nTid;

	// 指定されたpcode から 使用できる疵種IDの数を求める

	// 使用できる疵種の数内で乱数を発生させる

	// TIDを求める
	nTid = rand() % 3 + 1;

	return nTid;
}