#include "StdAfx.h"
#include "PreDBManager.h"

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
PreDBManager::PreDBManager():
ThreadManager("PreDBManager")
{
#ifdef LOCAL	// デバック時は、無処理
	//*/----> ◆テスト用コード
	CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, 2048 * 5, TBL_TO_COIL_NAME);
	//*///<<--- ◆テスト用コード
#endif			// デバック時は、無処理

	// 統合サーバー参照
	mcls_pSelect2TCM	= new PreDBSelectManager(TO_MAEDAT,"TO_MAEDAT_RETRY_2TCM","LINE_2TCM_SQL");
	mcls_pSelect3TCM    = new PreDBSelectManager(TO_MAEDAT,"TO_MAEDAT_RETRY_3TCM","LINE_3TCM_SQL");
	// 20200830 冷延　検査装置試験機　ハード・ソフト（2TCM穴検更新） 溝口 ----->>>
	mcls_pSelectN2TCM_ANA = new PreDBSelectManager(TO_MAEDAT, "TO_MAEDAT_RETRY_2TCM", "LINE_N2TCM_ANA_SQL");
	// 20200830 冷延　検査装置試験機　ハード・ソフト（2TCM穴検更新） 溝口 <<<-----
	// 20221021 2TCM実機化に伴う下工程改造 小島 --->>>
	mcls_pSelectN2TCM_KIZU = new PreDBSelectManager(TO_MAEDAT, "TO_MAEDAT_RETRY_2TCM", "LINE_N2TCM_KIZU_SQL");
	// 20221021 2TCM実機化に伴う下工程改造 小島 ---<<<

	m_emLastResult_2TCM	= PreDBSelectManager::INIT_DB;
	m_emLastResult_3TCM = PreDBSelectManager::INIT_DB;
	// 20200830 冷延　検査装置試験機　ハード・ソフト（2TCM穴検更新） 溝口 ----->>>
	m_emLastResult_N2TCM_ANA = PreDBSelectManager::NON_DB;
	// 20200830 冷延　検査装置試験機　ハード・ソフト（2TCM穴検更新） 溝口 <<<-----
	// 20221021 2TCM実機化に伴う下工程改造 小島 --->>>
	m_emLastResult_N2TCM_KIZU = PreDBSelectManager::NON_DB;
	// 20221021 2TCM実機化に伴う下工程改造 小島 ---<<<

	//// 共有メモリ
	// コイル情報テーブル
	mem_connect(&m_hCoil, (LPVOID *)&mtbl_pCoil, TBL_TO_COIL_NAME);
	if (m_hCoil == NULL) {
		syslog(SYSNO_MEM_OPEN, "コイル情報テーブル[%s] アクセス失敗", TBL_TO_COIL_NAME);
		_ASSERT(FALSE);
	}
}

//------------------------------------------
// デストラクタ
//------------------------------------------
PreDBManager::~PreDBManager(void)
{
	//// 共有メモリ開放
	mem_close(&m_hCoil, (LPVOID *)&mtbl_pCoil);							// コイル情報

	delete mcls_pSelect2TCM;
	delete mcls_pSelect3TCM;
	// 20200830 冷延　検査装置試験機　ハード・ソフト（2TCM穴検更新） 溝口 ----->>>
	delete mcls_pSelectN2TCM_ANA;
	// 20200830 冷延　検査装置試験機　ハード・ソフト（2TCM穴検更新） 溝口 <<<-----
	// 20221021 2TCM実機化に伴う下工程改造 小島 --->>>
	delete mcls_pSelectN2TCM_KIZU;
	// 20221021 2TCM実機化に伴う下工程改造 小島 ---<<<

}

//------------------------------------------
// スレッド開始前処理 (スレッド実行直後(シグナルセット前)にコールバック)
//------------------------------------------
int PreDBManager::ThreadFirst()
{
	//// イベント設定
	HANDLE	hArray[] = {gque_AddSQLData.g_evSem
						};
	SetEventNum( sizeof(hArray)/sizeof(HANDLE), hArray );
	return 0;
}

//------------------------------------------
// スレッド終了前処理 (終了シグナル後にコールバック)
//------------------------------------------
int PreDBManager::ThreadLast()
{
	//int		nKizuKenNoCnt;	// 2014.05.09 コメント化
	//bool	bDbMode;			// 2014.05.09 コメント化
	DeliveryPreDB* pDeli;

	// 受け渡しキューに格納されている項目を全て開放
	while( TRUE ) {
		pDeli = gque_AddSQLData.GetFromHead( 0 );
		if( pDeli == NULL ) break;

		// 開放
		SAFE_DELETE_ARRAY( pDeli )
	}

	return 0;
}

//------------------------------------------
// スレッドイベント発生
// int nEventNo イベントNo (0オリジン)
//------------------------------------------
void PreDBManager::ThreadEvent( int nEventNo )
{
	DeliveryPreDB* pDeli;				// スレッドキュー受け渡し

	// シグナルの条件
	enum {	EV_DB = 0					// 前工程実績情報取得
			};

	////// シグナル条件分け
	switch ( nEventNo ) {

//-----------------------------------------------------------------------------------------------
		case EV_DB:

			LOG(em_MSG), "[%s] 前工程実績取得要求", my_sThreadName );

			pDeli = gque_AddSQLData.GetFromHead();
			
			// DB登録処理
			DBInsMain( pDeli );

			// 解放
			SAFE_DELETE_ARRAY( pDeli )

			break;

//-----------------------------------------------------------------------------------------------
	default :
		// ありえない！！
		LOG(em_ERR), "[%s] ThreadEvent EventNo NG=%d", my_sThreadName, nEventNo );
		break;
	}
	return;
}

//------------------------------------------
// DB登録メイン処理
// DeliveryPreDB* pDeli  キューデータ
//------------------------------------------
void  PreDBManager::DBInsMain( DeliveryPreDB* pDeli )
{
	//int   nKizuKenNoCnt;  // 2014.05.09 コメント化
	//bool  bDbMode;		// 2014.05.09 コメント化

    CString     cMyDbSection;   // DBセクション
    OdbcBase    clsPreDB;       // 前工程DBインスタンス
    EM_PRE_LINE PreLine;        // 前工程識別

	//// ODBC接続クラス インスタンス生成
	OdbcBase clsDB;
	if( ! clsDB.Connect( TASKINI_NAME ) ) {
		// データベース接続エラー
		LOG(em_ERR), "[%s] DB接続エラー", my_sThreadName );
		syslog(SYSNO_DB_CONNECT_ERR, "");
		return;
	}

    LOG(em_MSG), "[%s] 前工程登録処理開始 [管理No = %s] [前工程管理No = %s] [表裏区分 = %d]",
		my_sThreadName, pDeli->cKizuKenNo, pDeli->cPreKizuKenNo, pDeli->nOuDiv );

    //// 一応対象かチェック
	if( ! CheckInsert( &clsDB, pDeli ) ) {
		LOG(em_WAR), "[%s] 登録対象外 [管理No = %s] [前工程管理No = %s] [表裏区分 = %d]",
			my_sThreadName, pDeli->cKizuKenNo, pDeli->cPreKizuKenNo, pDeli->nOuDiv );
		return;
	}

	// 疵検出工程から前工程を決定
	if (0 == strcmp(KOUTEI_CODE_2TCM, pDeli->cLineCode)){
		// 20200830 冷延　検査装置試験機　ハード・ソフト（2TCM穴検更新） 溝口 ----->>>
		//PreLine = PRE_LINE_2TCM;		// 2TCM	
		//cMyDbSection = "DB_2TCM";
		// 疵検IDにて新旧の判定を行う
		if (0 == strncmp(pDeli->cPreKizuKenNo, KIZUKEN_CODE_2TCM, SIZE_KIZUKEN_CODE)) {
			PreLine = PRE_LINE_2TCM;		// 2TCM	
			cMyDbSection = "DB_2TCM";
		}
		else {
			// 20221021 2TCM実機化に伴う下工程改造 小島 --->>>
			//PreLine = PRE_LINE_N2TCM_ANA;	// N2TCM_ANA	
			//cMyDbSection = "DB_N2TCM_ANA";
			if (0 == strncmp(pDeli->cPreKizuKenNo, KIZUKEN_CODE_N2TCM_VZ, SIZE_KIZUKEN_CODE)) {
				PreLine = PRE_LINE_N2TCM_KIZU;	// N2TCM_KIZU	
				cMyDbSection = "DB_N2TCM_KIZU";
			}
			else {
				PreLine = PRE_LINE_N2TCM_ANA;	// N2TCM_ANA	
				cMyDbSection = "DB_N2TCM_ANA";
			}
			// 20221021 2TCM実機化に伴う下工程改造 小島 ---<<<
		}
		// 20200830 冷延　検査装置試験機　ハード・ソフト（2TCM穴検更新） 溝口 <<<-----
	} else if(0 == strcmp(KOUTEI_CODE_3TCM, pDeli->cLineCode)){
		PreLine = PRE_LINE_3TCM;		// 3TCM	
		cMyDbSection = "DB_3TCM";
	} else {
		// 対象外の前工程コード
		return;
	}

	// 接続先DBを決定(前工程蓄積 or 統合)
	bool bResult;
	if (PRE_LINE_2TCM == PreLine)	bResult = mcls_pSelect2TCM->SelectConnectDB("DB_2TCM", "DB_2TCM_SHARE", pDeli->cPreKizuKenNo, &clsPreDB);
	if (PRE_LINE_3TCM == PreLine)	bResult = mcls_pSelect3TCM->SelectConnectDB("DB_3TCM", "DB_3TCM_SHARE", pDeli->cPreKizuKenNo, &clsPreDB);
	// 20200830 冷延　検査装置試験機　ハード・ソフト（2TCM穴検更新） 溝口 ----->>>
	if (PRE_LINE_N2TCM_ANA == PreLine)	bResult = mcls_pSelectN2TCM_ANA->SelectConnectDB("DB_N2TCM_ANA", "DB_N2TCM_ANA_SHARE", pDeli->cPreKizuKenNo, &clsPreDB);
	// 20200830 冷延　検査装置試験機　ハード・ソフト（2TCM穴検更新） 溝口 <<<-----
	// 20221021 2TCM実機化に伴う下工程改造 小島 --->>>
	if (PRE_LINE_N2TCM_KIZU == PreLine)	bResult = mcls_pSelectN2TCM_KIZU->SelectConnectDB("DB_N2TCM_KIZU", "DB_N2TCM_KIZU_SHARE", pDeli->cPreKizuKenNo, &clsPreDB);
	// 20221021 2TCM実機化に伴う下工程改造 小島 ---<<<

	// 接続結果によりイベントログ・シスログを出力
	if (bResult){
		//--------------------
		// 接続結果OK
		//--------------------
		PreDBSelectManager::EM_SELECT_DB	nSelectDB;
		//int								AlertNo = GetAlertNo(PreDBSelectManager::PRE_DB,PreLine);

		// 接続先DBを取得
		if (PRE_LINE_2TCM == PreLine) nSelectDB = mcls_pSelect2TCM->GetSelectMode();
		if (PRE_LINE_3TCM == PreLine) nSelectDB = mcls_pSelect3TCM->GetSelectMode();
		// 20200830 冷延　検査装置試験機　ハード・ソフト（2TCM穴検更新） 溝口 ----->>>
		if (PRE_LINE_N2TCM_ANA == PreLine) nSelectDB = mcls_pSelectN2TCM_ANA->GetSelectMode();
		// 20200830 冷延　検査装置試験機　ハード・ソフト（2TCM穴検更新） 溝口 <<<-----
		// 20221021 2TCM実機化に伴う下工程改造 小島 --->>>
		if (PRE_LINE_N2TCM_KIZU == PreLine) nSelectDB = mcls_pSelectN2TCM_KIZU->GetSelectMode();
		// 20221021 2TCM実機化に伴う下工程改造 小島 ---<<<

		int AlertNo = GetAlertNo(nSelectDB, PreLine);

		// 前回と接続先DBが同じ場合は、シスログ出力無し
		if (PRE_LINE_2TCM == PreLine){
			if (m_emLastResult_2TCM == nSelectDB)	AlertNo = NON_ALERT;
		}
		if (PRE_LINE_3TCM == PreLine){
			if (m_emLastResult_3TCM == nSelectDB)	AlertNo = NON_ALERT;
		}
		// 20200830 冷延　検査装置試験機　ハード・ソフト（2TCM穴検更新） 溝口 ----->>>
		if (PRE_LINE_N2TCM_ANA == PreLine) {
			if (m_emLastResult_N2TCM_ANA == nSelectDB)	AlertNo = NON_ALERT;
		}
		// 20200830 冷延　検査装置試験機　ハード・ソフト（2TCM穴検更新） 溝口 <<<-----
		// 20221021 2TCM実機化に伴う下工程改造 小島 --->>>
		if (PRE_LINE_N2TCM_KIZU == PreLine) {
			if (m_emLastResult_N2TCM_KIZU == nSelectDB)	AlertNo = NON_ALERT;
		}
		// 20221021 2TCM実機化に伴う下工程改造 小島 ---<<<

		switch (nSelectDB){
		case PreDBSelectManager::PRE_DB:
			if (PRE_LINE_2TCM == PreLine) m_emLastResult_2TCM = PreDBSelectManager::PRE_DB;
			if (PRE_LINE_3TCM == PreLine) m_emLastResult_3TCM = PreDBSelectManager::PRE_DB;
			// 20200830 冷延　検査装置試験機　ハード・ソフト（2TCM穴検更新） 溝口 ----->>>
			if (PRE_LINE_N2TCM_ANA == PreLine) m_emLastResult_N2TCM_ANA = PreDBSelectManager::PRE_DB;
			// 20200830 冷延　検査装置試験機　ハード・ソフト（2TCM穴検更新） 溝口 <<<-----
			// 20221021 2TCM実機化に伴う下工程改造 小島 --->>>
			if (PRE_LINE_N2TCM_KIZU == PreLine) m_emLastResult_N2TCM_KIZU = PreDBSelectManager::PRE_DB;
			// 20221021 2TCM実機化に伴う下工程改造 小島 ---<<<

			if (NON_ALERT != AlertNo) syslog(AlertNo, "[蓄積][工程 = %s]", pDeli->cLineCode);
			LOG(em_MSG), "[%s] 前工程DB接続結果[TCM:蓄積]", my_sThreadName);
			break;
		case PreDBSelectManager::SHARE_DB:
			if (PRE_LINE_2TCM == PreLine) m_emLastResult_2TCM = PreDBSelectManager::SHARE_DB;
			if (PRE_LINE_3TCM == PreLine) m_emLastResult_3TCM = PreDBSelectManager::SHARE_DB;
			// 20200830 冷延　検査装置試験機　ハード・ソフト（2TCM穴検更新） 溝口 ----->>>
			if (PRE_LINE_N2TCM_ANA == PreLine) m_emLastResult_N2TCM_ANA = PreDBSelectManager::SHARE_DB;
			// 20200830 冷延　検査装置試験機　ハード・ソフト（2TCM穴検更新） 溝口 <<<-----
			// 20221021 2TCM実機化に伴う下工程改造 小島 --->>>
			if (PRE_LINE_N2TCM_KIZU == PreLine) m_emLastResult_N2TCM_KIZU = PreDBSelectManager::SHARE_DB;
			// 20221021 2TCM実機化に伴う下工程改造 小島 ---<<<

			if (NON_ALERT != AlertNo) syslog(AlertNo, "[統合][工程 = %s]", pDeli->cLineCode);
			LOG(em_MSG), "[%s] 前工程DB接続結果[統合][工程 = %s]", my_sThreadName, pDeli->cLineCode);
			break;
		default:
			// ありえない
			break;
		}
	} else{
		//--------------------
		// 接続結果NG
		//--------------------
		// 現地DBのシスログ出力
		int AlertNo = GetAlertNo(PreDBSelectManager::PRE_DB, PreLine);

		// 前回と接続先DBが同じ場合は、シスログ出力無し
		if (PRE_LINE_2TCM == PreLine){
			if (m_emLastResult_2TCM == PreDBSelectManager::NON_DB)	AlertNo = NON_ALERT;
		}
		if (PRE_LINE_3TCM == PreLine){
			if (m_emLastResult_3TCM == PreDBSelectManager::NON_DB)	AlertNo = NON_ALERT;
		}
		// 20200830 冷延　検査装置試験機　ハード・ソフト（2TCM穴検更新） 溝口 ----->>>
		if (PRE_LINE_N2TCM_ANA == PreLine) {
			if (m_emLastResult_N2TCM_ANA == PreDBSelectManager::NON_DB)	AlertNo = NON_ALERT;
		}
		// 20200830 冷延　検査装置試験機　ハード・ソフト（2TCM穴検更新） 溝口 <<<-----
		// 20221021 2TCM実機化に伴う下工程改造 小島 --->>>
		if (PRE_LINE_N2TCM_KIZU == PreLine) {
			if (m_emLastResult_N2TCM_KIZU == PreDBSelectManager::NON_DB)	AlertNo = NON_ALERT;
		}
		// 20221021 2TCM実機化に伴う下工程改造 小島 ---<<<
		if (NON_ALERT != AlertNo) syslog(AlertNo, "[蓄積][工程 = %s]", pDeli->cLineCode);		// シスログ出力(前工程蓄積)


		// 統合のシスログ出力
		AlertNo = GetAlertNo(PreDBSelectManager::SHARE_DB, PreLine);

		// 前回と接続先DBが同じ場合は、シスログ出力無し
		if (PRE_LINE_2TCM == PreLine){
			if (m_emLastResult_2TCM == PreDBSelectManager::NON_DB)	AlertNo = NON_ALERT;
		}
		if (PRE_LINE_3TCM == PreLine){
			if (m_emLastResult_3TCM == PreDBSelectManager::NON_DB)	AlertNo = NON_ALERT;
		}
		// 20200830 冷延　検査装置試験機　ハード・ソフト（2TCM穴検更新） 溝口 ----->>>
		if (PRE_LINE_N2TCM_ANA == PreLine) {
			if (m_emLastResult_N2TCM_ANA == PreDBSelectManager::NON_DB)	AlertNo = NON_ALERT;
		}
		// 20200830 冷延　検査装置試験機　ハード・ソフト（2TCM穴検更新） 溝口 <<<-----
		// 20221021 2TCM実機化に伴う下工程改造 小島 --->>>
		if (PRE_LINE_N2TCM_KIZU == PreLine) {
			if (m_emLastResult_N2TCM_KIZU == PreDBSelectManager::NON_DB)	AlertNo = NON_ALERT;
		}
		// 20221021 2TCM実機化に伴う下工程改造 小島 ---<<<

		if (NON_ALERT != AlertNo) syslog(AlertNo, "[統合][工程 = %s]", pDeli->cLineCode);		// シスログ出力(統合)
		LOG(em_ERR), "[%s] 前工程DB接続エラー [%s]", my_sThreadName, cMyDbSection );

		if (PRE_LINE_2TCM == PreLine) m_emLastResult_2TCM = PreDBSelectManager::NON_DB;
		if (PRE_LINE_3TCM == PreLine) m_emLastResult_3TCM = PreDBSelectManager::NON_DB;
		// 20200830 冷延　検査装置試験機　ハード・ソフト（2TCM穴検更新） 溝口 ----->>>
		if (PRE_LINE_N2TCM_ANA == PreLine) m_emLastResult_N2TCM_ANA = PreDBSelectManager::NON_DB;
		// 20200830 冷延　検査装置試験機　ハード・ソフト（2TCM穴検更新） 溝口 <<<-----
		// 20221021 2TCM実機化に伴う下工程改造 小島 --->>>
		if (PRE_LINE_N2TCM_KIZU == PreLine) m_emLastResult_N2TCM_KIZU = PreDBSelectManager::NON_DB;
		// 20221021 2TCM実機化に伴う下工程改造 小島 ---<<<

		return;
	}

	// 前工程疵情報、画像情報登録
	if (!DBIns(PreLine, pDeli, &clsDB, &clsPreDB)) {
		LOG(em_ERR), "[%s] 前工程疵情報、疵画像登録失敗 [疵検管理No = %s] [前工程疵検管理No = %s] [表裏区分 = %d]",
			my_sThreadName, pDeli->cKizuKenNo, pDeli->cPreKizuKenNo, pDeli->nOuDiv );
			syslog(PARAM_SYSLOG_BASENO + 1, "[管理No = %s] [前工程管理No = %s] [表裏区分 = %d]", pDeli->cKizuKenNo, pDeli->cPreKizuKenNo, pDeli->nOuDiv); // 前工程疵情報、疵画像登録失敗
			return;
	}

	// 前工程ライン別コイル情報更新　前工程データ取得済み認識セット
	UpdateCoilLine(&clsDB, pDeli);

	LOG(em_MSG), "[%s] 前工程登録処理終了 [管理No = %s] [前工程管理No = %s] [表裏区分 = %d]",
		my_sThreadName, pDeli->cKizuKenNo, pDeli->cPreKizuKenNo, pDeli->nOuDiv );


	PRO_TYPE_COILINF& denbun = (PRO_TYPE_COILINF&)mtbl_pCoil->rec[0].val;	// コイル情報伝文
	// 現コイル＆裏面の場合は統括に通知する
	if (0 == memcmp(pDeli->cMeiNo, denbun.data.CoilInf.KeyInf.cMeiNo, SIZE_MEI_NO) &&
		1 == pDeli->nOuDiv) {
		SendMailDefect(pDeli->cMeiNo, pDeli->cCoilNo);
	}

	// 2014.05.09 諸藤 統合サーバー参照 <<<-----
}

//------------------------------------------
// 登録対象かチェックする
// OdbcBase* clspDB DB登録インスタンス
// DeliveryPreDB* pDeli キューデータ
// 戻り値 復帰情報 true:データあり false:データなし
//------------------------------------------
bool PreDBManager::CheckInsert( OdbcBase* clspDB, DeliveryPreDB* pDeli )
{
	CString sql;					// SQL文
	CString wk;
	int		nCnt = 0;
	CString cDbName;
	CString cDbName2;
	cDbName = DB_MAE_COIL_INF;
	cDbName2 = DB_MAE_COIL_LINE;

	//// SQL文作成(管理No取得)
	sql =  "SELECT";
	sql += " INF.管理No ";
	sql += " FROM ";
	sql += cDbName;
	sql += " AS INF, ";
	sql += cDbName2;
	sql += " AS LINE ";
	sql += " WHERE ";
	sql += " INF.管理No = LINE.管理No ";
	sql += " AND INF.FF前工程疵情報受信フラグ = 0 ";
	sql += " AND INF.仮管理Noフラグ = -1 ";
	sql += " AND LINE.処理ステータス = -1 ";
	wk.Format( " AND INF.管理No = '%s' AND LINE.表裏区分 = %d ", pDeli->cKizuKenNo, pDeli->nOuDiv );
	sql += wk;
	
	return clspDB->GetSelectKey( sql, &nCnt );
}

//------------------------------------------
// 情報編集処理
// EM_PRE_LINE PreLine 前工程ライン
// DeliveryPreDB* pDeli キューデータ
// OdbcBase* clsInsDB	登録DBインスタンス
// OdbcBase* clsPreDB   前工程DBインスタンス
// 戻り値 復帰情報 true:正常 false:異常
//------------------------------------------
bool PreDBManager::DBIns( EM_PRE_LINE PreLine, DeliveryPreDB* pDeli, OdbcBase* clsInsDB, OdbcBase* clsPreDB )
{

	CString sql;					// SQL文
	CString wk;
	int ii;
	MAE_DEFECT_IMAGE_INF  typData;	// 前工程情報
	CString cDbName;
	cDbName = DB_MAE_FF_INF;
	
	// ODBC接続クラス インスタンス生成(自DB)
	OdbcBase clsDB;
	if( ! clsDB.Connect( TASKINI_NAME ) ) {
		// データベース接続エラー
		LOG(em_ERR), "[%s] DB接続エラー", my_sThreadName );
		syslog(SYSNO_DB_CONNECT_ERR, "");
		return false;
	}

	sql = "SELECT";
	sql += " 疵No, 工程種別, 表裏区分, 前工程疵No ";
	sql += " FROM ";
	sql += cDbName;
	// ＦＦサーバ上で疵Noが０のレコードは前工程データ取得対象外
	wk.Format( " WHERE 管理No = '%s' AND 前工程管理No = '%s' AND 表裏区分 = %d",
		pDeli->cKizuKenNo , pDeli->cPreKizuKenNo, pDeli->nOuDiv );
	sql += wk;
	sql += " AND 前工程疵No >= 1 ";

	LOG(em_INF), "[%s] 前工程FF疵情報取得対象検索 [%s]", my_sThreadName, sql );
	//// 実行
	if( ! clsDB.ExecuteDirect( sql ) ) {
		LOG(em_NON), "%s", sql );
		LOG(em_ERR), "[%s] 前工程FF疵情報実行エラー [%s]", my_sThreadName, cDbName );
		syslog(SYSNO_DB_EXECUTEDIRECT_ERR, "[%s]", cDbName);
		return false;
	}
	
	//// 取得
	while (true) {

		int sqlrc = clsDB.Fetch();
		if( sqlrc == KIZUODBC_FETCH_OK ) {					// データ有り
			
			// 初期化
			memset( &typData, 0x00, sizeof(typData) );

			memcpy( typData.cKizuKenNo, pDeli->cKizuKenNo, sizeof(typData.cKizuKenNo) );			// 管理No
			memcpy( typData.cPreKizuKenNo, pDeli->cPreKizuKenNo, sizeof(typData.cPreKizuKenNo) );	// 前工程管理NO

			clsDB.GetData( 1, &typData.nKizuNo );													// 疵No
			clsDB.GetData( 2, &typData.nKouteiSyubetu );											// 工程種別
			clsDB.GetData( 3, &typData.nOuDiv );													// 表裏区分
			clsDB.GetData( 4, &typData.nMaeKizuNo );												// 前工程疵No

			// 2014.05.09 諸藤 統合サーバー参照 ------------>>>
			switch(PreLine){
				case PRE_LINE_2TCM:						//// 前工程情報取得
					if( ! GetNew2TCMInf( clsPreDB, &typData ) )	goto Ending;
					break;
				case PRE_LINE_3TCM:						//// 前工程情報取得
					if (!GetNew3TCMInf(clsPreDB, &typData))	goto Ending;
					break;
					// 20200830 冷延　検査装置試験機　ハード・ソフト（2TCM穴検更新） 溝口 ----->>>
				case PRE_LINE_N2TCM_ANA:				//// 前工程情報取得
					if (!GetNewN2TCM_ANAInf(clsPreDB, &typData))	goto Ending;
					break;
					// 20200830 冷延　検査装置試験機　ハード・ソフト（2TCM穴検更新） 溝口 <<<-----
				// 20221021 2TCM実機化に伴う下工程改造 小島 --->>>
				case PRE_LINE_N2TCM_KIZU:				//// 前工程情報取得
					if (!GetNewN2TCM_KIZUInf(clsPreDB, &typData))	goto Ending;
					break;
				// 20221021 2TCM実機化に伴う下工程改造 小島 ---<<<
				default:
					// ありえない
					break;
			}
			// 2014.05.09 諸藤 統合サーバー参照 <<<------------

            //// 前工程疵情報テーブル更新
			if( ! InsertDefectInf( clsInsDB, &typData ) ) {
				goto Ending;
			}

            //// 前工程疵画像テーブル登録 ※疵画像有りかチェック
			if( 0 != typData.nCnt ) {
				if( ! InsertDefectImg( clsInsDB, &typData ) ) {
					goto Ending;
				}
			}

			// 開放
			for( ii = 0; ii< MAX_MAE_IMAGE; ii++ ) {
				SAFE_DELETE_ARRAY( typData.pImg[ii] )
			}
			
		} else if( sqlrc == KIZUODBC_FETCH_NODATE ) {		// データ無し
			break;

		} else {								// エラー
			LOG(em_NON), "%s", sql );
			LOG(em_ERR), "[%s] 前工程FF疵情報取得エラー [%s]", my_sThreadName, cDbName );
			syslog(SYSNO_DB_FETCH_ERR, "[%s]", cDbName);
			return false;
		}
	}

	return true;

Ending: 
	
	// 開放
	for( ii = 0; ii < MAX_MAE_IMAGE; ii++ ) {
		SAFE_DELETE_ARRAY( typData.pImg[ii] )
	}
	return false;
}


//------------------------------------------
// 2TCM情報取得
// OdbcBase* clsPreDB				前工程DB
// MAE_DEFECT_IMAGE_INF* typData	前工程情報
// 戻り値 復帰情報 true:正常 false:異常
//------------------------------------------
bool PreDBManager::GetNew2TCMInf( OdbcBase* clsPreDB, MAE_DEFECT_IMAGE_INF* typData )
{
	
	CString sql;					// SQL文
	CString wk;
	int sqlrc;
	CString cDbName;
	cDbName  = DB_2TCM_DEFECT_INF_IN;

	LOG(em_MSG), "[%s] 2TCM情報取得 [管理No:%s] [疵No=%d]", my_sThreadName, typData->cPreKizuKenNo, typData->nMaeKizuNo );

	// 前工程疵情報取得
	sql = "SELECT ";
	sql += "DSWS区分, 長手位置, DS位置, WS位置,";
	sql += "ライン速度,縦分解能,横分解能,外接Xmin,外接Xmax,外接Ymin,外接Ymax,";
	sql += "疵中心位置X,疵中心位置Y,縦画素数,横画素数,画像サイズ";
	sql += " FROM ";
	sql += cDbName;
	wk.Format( " WHERE 疵検管理No= '%.20s' AND 疵No = %d ", typData->cPreKizuKenNo, typData->nMaeKizuNo );
	sql += wk;

	//// 実行
	if( ! clsPreDB->ExecuteDirect( sql ) ) {
		LOG(em_NON), "%s", sql );
		LOG(em_ERR), "[%s] 2TCM疵情報実行エラー [%s]", my_sThreadName, cDbName);
		syslog(SYSNO_DB_EXECUTEDIRECT_ERR, "[%s]", cDbName);
		return false;
	}

	sqlrc = clsPreDB->Fetch();
	if( sqlrc == KIZUODBC_FETCH_OK ) {					// データ有り

		clsPreDB->GetData(1, (int*)&typData->typDefectData.dsws);			// DSWS区分		
		clsPreDB->GetData(2, &typData->typDefectData.y);					// 長手位置
		clsPreDB->GetData(3, &typData->typDefectData.ds);					// DS位置
		clsPreDB->GetData(4, &typData->typDefectData.ws);					// WS位置
		clsPreDB->GetData(5, &typData->typDefectData.speed);				// ライン速度
		clsPreDB->GetData(6, &typData->typDefectData.h_res);				// 縦分解能
		clsPreDB->GetData(7, &typData->typDefectData.w_res);				// 横分解能
		clsPreDB->GetData(8, (int*)&typData->typDefectData.box_x_min);		// 外接Xmin
		clsPreDB->GetData(9, (int*)&typData->typDefectData.box_x_max);		// 外接Xmax
		clsPreDB->GetData(10, (int*)&typData->typDefectData.box_y_min);		// 外接Ymin
		clsPreDB->GetData(11, (int*)&typData->typDefectData.box_y_max);		// 外接Ymax
		clsPreDB->GetData(12, (int*)&typData->typDefectData.center_x);		// 疵中心位置X
		clsPreDB->GetData(13, (int*)&typData->typDefectData.center_y);		// 疵中心位置Y
		clsPreDB->GetData(14, (int*)&typData->typDefectData.h_size);		// 縦画素数
		clsPreDB->GetData(15, (int*)&typData->typDefectData.w_size);		// 横画素数
		clsPreDB->GetData(16, (int*)&typData->nSize[0]);					// 画像サイズ

	} else if( sqlrc == KIZUODBC_FETCH_NODATE ) {		// データ無し

	} else {								// エラー
		LOG(em_NON), "%s", sql );
		LOG(em_ERR), "[%s] 2TCM疵情報取得エラー [%s]", my_sThreadName, cDbName);
		syslog(SYSNO_DB_FETCH_ERR, "[%s]", cDbName);
		return false;
	}


	cDbName  = DB_2TCM_DEFECT_IMG_IN;


	// 前工程疵画像取得	
	sql = "SELECT 疵画像 ";
	sql += " FROM ";
	sql += cDbName;
	wk.Format(" WHERE 疵検管理No= '%s' AND 疵No = %d ", typData->cPreKizuKenNo, typData->nMaeKizuNo );
	sql += wk;

	//// 実行
	if( ! clsPreDB->ExecuteDirect( sql ) ) {
		LOG(em_NON), "%s", sql );
		LOG(em_ERR), "[%s] 2TCM疵画像実行エラー [%s]", my_sThreadName, cDbName);
		syslog(SYSNO_DB_EXECUTEDIRECT_ERR, "[%s]", cDbName);
		return false;
	}

	sqlrc = clsPreDB->Fetch();
	if( sqlrc == KIZUODBC_FETCH_OK ) {					// データ有り
		
		// 画像情報作成 2014.05.27 J.Wakazono 巻き取り前データ取得失敗対応
		for(int ii=0; ii<MAX_MAE_IMAGE; ii++) {
			if(0 < typData->nSize[ii]) {
				typData->pImg[ii] = new BYTE[ typData->nSize[ii] ];
				typData->nCnt++;
			} else {
				break;
			}
		}

		clsPreDB->GetDataImage( 1, typData->pImg[0], typData->nSize[0] );	// 疵画像
		
	} else if( sqlrc == KIZUODBC_FETCH_NODATE ) {		// データ無し
		
		// 画像情報作成
		LOG(em_INF), "[%s] 画像無し %s", my_sThreadName, sql );
		typData->nCnt = 0;	// 画像データ数 0

	} else {								// エラー
		LOG(em_NON), "%s", sql );
		LOG(em_ERR), "[%s] 2TCM疵画像取得エラー [%s]", my_sThreadName, cDbName);
		syslog(SYSNO_DB_FETCH_ERR, "[%s]", cDbName);
		return false;
	}

	return true;
}

//------------------------------------------
// 3TCM情報取得
// OdbcBase* clsPreDB				前工程DB
// MAE_DEFECT_IMAGE_INF* typData	前工程情報
// 戻り値 復帰情報 true:正常 false:異常
//------------------------------------------
bool PreDBManager::GetNew3TCMInf(OdbcBase* clsPreDB, MAE_DEFECT_IMAGE_INF* typData)
{

	CString sql;					// SQL文
	CString wk;
	int sqlrc;
	CString cDbName;
	cDbName = DB_3TCM_DEFECT_INF_IN;

	LOG(em_MSG), "[%s] 3TCM情報取得 [管理No:%s] [疵No=%d]", my_sThreadName, typData->cPreKizuKenNo, typData->nMaeKizuNo );

	// 前工程疵情報取得
	sql = "SELECT ";
	sql += "DSWS区分, 長手位置, DS位置, WS位置,";
	sql += "ライン速度,縦分解能,横分解能,外接Xmin,外接Xmax,外接Ymin,外接Ymax,";
	sql += "疵中心位置X,疵中心位置Y,縦画素数,横画素数,画像サイズ";
	sql += " FROM ";
	sql += cDbName;
	wk.Format(" WHERE 疵検管理No= '%.20s' AND 疵No = %d ", typData->cPreKizuKenNo, typData->nMaeKizuNo);
	sql += wk;

	//// 実行
	if (!clsPreDB->ExecuteDirect(sql)) {
		LOG(em_NON), "%s", sql );
		LOG(em_ERR), "[%s] 3TCM疵情報実行エラー [%s]", my_sThreadName, cDbName);
		syslog(SYSNO_DB_EXECUTEDIRECT_ERR, "[%s]", cDbName);
		return false;
	}

	sqlrc = clsPreDB->Fetch();
	if (sqlrc == KIZUODBC_FETCH_OK) {					// データ有り

		clsPreDB->GetData(1, (int*)&typData->typDefectData.dsws);			// DSWS区分		
		clsPreDB->GetData(2, &typData->typDefectData.y);					// 長手位置
		clsPreDB->GetData(3, &typData->typDefectData.ds);					// DS位置
		clsPreDB->GetData(4, &typData->typDefectData.ws);					// WS位置
		clsPreDB->GetData(5, &typData->typDefectData.speed);				// ライン速度
		clsPreDB->GetData(6, &typData->typDefectData.h_res);				// 縦分解能
		clsPreDB->GetData(7, &typData->typDefectData.w_res);				// 横分解能
		clsPreDB->GetData(8, (int*)&typData->typDefectData.box_x_min);		// 外接Xmin
		clsPreDB->GetData(9, (int*)&typData->typDefectData.box_x_max);		// 外接Xmax
		clsPreDB->GetData(10, (int*)&typData->typDefectData.box_y_min);		// 外接Ymin
		clsPreDB->GetData(11, (int*)&typData->typDefectData.box_y_max);		// 外接Ymax
		clsPreDB->GetData(12, (int*)&typData->typDefectData.center_x);		// 疵中心位置X
		clsPreDB->GetData(13, (int*)&typData->typDefectData.center_y);		// 疵中心位置Y
		clsPreDB->GetData(14, (int*)&typData->typDefectData.h_size);		// 縦画素数
		clsPreDB->GetData(15, (int*)&typData->typDefectData.w_size);		// 横画素数
		clsPreDB->GetData(16, (int*)&typData->nSize[0]);					// 画像サイズ

	} else if (sqlrc == KIZUODBC_FETCH_NODATE) {		// データ無し

	} else {								// エラー
		LOG(em_NON), "%s", sql );
		LOG(em_ERR), "[%s] 3TCM疵情報取得エラー [%s]", my_sThreadName, cDbName);
		syslog(SYSNO_DB_FETCH_ERR, "[%s]", cDbName);
		return false;
	}


	cDbName = DB_3TCM_DEFECT_IMG_IN;


	// 前工程疵画像取得	
	sql = "SELECT";
	sql += " 疵画像 ";
	sql += " FROM ";
	sql += cDbName;
	wk.Format(" WHERE 疵検管理No= '%s' AND 疵No = %d ", typData->cPreKizuKenNo, typData->nMaeKizuNo);
	sql += wk;

	//// 実行
	if (!clsPreDB->ExecuteDirect(sql)) {
		LOG(em_NON), "%s", sql );
		LOG(em_ERR), "[%s] 3TCM疵画像実行エラー [%s]", my_sThreadName, cDbName);
		syslog(SYSNO_DB_EXECUTEDIRECT_ERR, "[%s]", cDbName);
		return false;
	}

	sqlrc = clsPreDB->Fetch();
	if (sqlrc == KIZUODBC_FETCH_OK) {					// データ有り

		// 画像情報作成 2014.05.27 J.Wakazono 巻き取り前データ取得失敗対応
		for (int ii = 0; ii<MAX_MAE_IMAGE; ii++) {
			if (0 < typData->nSize[ii]) {
				typData->pImg[ii] = new BYTE[typData->nSize[ii]];
				typData->nCnt++;
			} else {
				break;
			}
		}

		clsPreDB->GetDataImage(1, typData->pImg[0], typData->nSize[0]);	// 疵画像

	} else if (sqlrc == KIZUODBC_FETCH_NODATE) {		// データ無し

														// 画像情報作成
		typData->nCnt = 0;	// 画像データ数 0

	} else {								// エラー
		LOG(em_NON), "%s", sql );
		LOG(em_ERR), "[%s] 3TCM疵画像取得エラー [%s]", my_sThreadName, cDbName);
		syslog(SYSNO_DB_FETCH_ERR, "[%s]", cDbName);
		return false;
	}

	return true;
}

// 20200830 冷延　検査装置試験機　ハード・ソフト（2TCM穴検更新） 溝口 ----->>>
//------------------------------------------
// N2TCM_ANA情報取得
// OdbcBase* clsPreDB				前工程DB
// MAE_DEFECT_IMAGE_INF* typData	前工程情報
// 戻り値 復帰情報 true:正常 false:異常
//------------------------------------------
bool PreDBManager::GetNewN2TCM_ANAInf(OdbcBase* clsPreDB, MAE_DEFECT_IMAGE_INF* typData)
{

	CString sql;					// SQL文
	CString wk;
	int sqlrc;
	CString cDbName;
	cDbName = DB_N2TCM_ANA_DEFECT_INF;

	LOG(em_MSG), "[%s] N2TCM_ANA情報取得 [管理No:%s] [疵No=%d]", my_sThreadName, typData->cPreKizuKenNo, typData->nMaeKizuNo );

	// 前工程疵情報取得
	sql = "SELECT ";
	sql += "DSWS区分, 長手位置, DS位置, WS位置,";
	sql += "ライン速度,縦分解能,横分解能,外接Xmin,外接Xmax,外接Ymin,外接Ymax,";
	sql += "疵中心位置X,疵中心位置Y,縦画素数,横画素数,画像サイズ正";
	sql += " FROM ";
	sql += cDbName;
	wk.Format(" WHERE 管理No= '%.20s' AND 疵No = %d ", typData->cPreKizuKenNo, typData->nMaeKizuNo);
	sql += wk;

	//// 実行
	if (!clsPreDB->ExecuteDirect(sql)) {
		LOG(em_NON), "%s", sql );
		syslog(SYSNO_DB_EXECUTEDIRECT_ERR, "[%s]", cDbName);
		return false;
	}

	sqlrc = clsPreDB->Fetch();
	if (sqlrc == KIZUODBC_FETCH_OK) {					// データ有り

		clsPreDB->GetData(1, (int*)&typData->typDefectData.dsws);			// DSWS区分		
		clsPreDB->GetData(2, &typData->typDefectData.y);					// 長手位置
		clsPreDB->GetData(3, &typData->typDefectData.ds);					// DS位置
		clsPreDB->GetData(4, &typData->typDefectData.ws);					// WS位置
		clsPreDB->GetData(5, &typData->typDefectData.speed);				// ライン速度
		clsPreDB->GetData(6, &typData->typDefectData.h_res);				// 縦分解能
		clsPreDB->GetData(7, &typData->typDefectData.w_res);				// 横分解能
		clsPreDB->GetData(8, (int*)&typData->typDefectData.box_x_min);		// 外接Xmin
		clsPreDB->GetData(9, (int*)&typData->typDefectData.box_x_max);		// 外接Xmax
		clsPreDB->GetData(10, (int*)&typData->typDefectData.box_y_min);		// 外接Ymin
		clsPreDB->GetData(11, (int*)&typData->typDefectData.box_y_max);		// 外接Ymax
		clsPreDB->GetData(12, (int*)&typData->typDefectData.center_x);		// 疵中心位置X
		clsPreDB->GetData(13, (int*)&typData->typDefectData.center_y);		// 疵中心位置Y
		clsPreDB->GetData(14, (int*)&typData->typDefectData.h_size);		// 縦画素数
		clsPreDB->GetData(15, (int*)&typData->typDefectData.w_size);		// 横画素数
		clsPreDB->GetData(16, (int*)&typData->nSize[0]);					// 画像サイズ

	}
	else if (sqlrc == KIZUODBC_FETCH_NODATE) {		// データ無し

	}
	else {								// エラー
		syslog(SYSNO_DB_FETCH_ERR, "[%s]", cDbName);
		return false;
	}


	cDbName = DB_N2TCM_ANA_DEFECT_IMG;


	// 前工程疵画像取得	
	sql = "SELECT 正画像 ";
	sql += " FROM ";
	sql += cDbName;
	wk.Format(" WHERE 管理No= '%s' AND 疵No = %d ", typData->cPreKizuKenNo, typData->nMaeKizuNo);
	sql += wk;

	//// 実行
	if (!clsPreDB->ExecuteDirect(sql)) {
		LOG(em_NON), "%s", sql );
		syslog(SYSNO_DB_EXECUTEDIRECT_ERR, "[%s]", cDbName);
		return false;
	}

	sqlrc = clsPreDB->Fetch();
	if (sqlrc == KIZUODBC_FETCH_OK) {					// データ有り

														// 画像情報作成 2014.05.27 J.Wakazono 巻き取り前データ取得失敗対応
		for (int ii = 0; ii<MAX_MAE_IMAGE; ii++) {
			if (0 < typData->nSize[ii]) {
				typData->pImg[ii] = new BYTE[typData->nSize[ii]];
				typData->nCnt++;
			}
			else {
				break;
			}
		}

		clsPreDB->GetDataImage(1, typData->pImg[0], typData->nSize[0]);	// 疵画像

	}
	else if (sqlrc == KIZUODBC_FETCH_NODATE) {		// データ無し

													// 画像情報作成
		typData->nCnt = 0;	// 画像データ数 0

	}
	else {								// エラー
		syslog(SYSNO_DB_FETCH_ERR, "[%s]", cDbName);
		return false;
	}

	return true;
}
// 20200830 冷延　検査装置試験機　ハード・ソフト（2TCM穴検更新） 溝口 <<<-----

// 20221021 2TCM実機化に伴う下工程改造 小島 --->>>
//------------------------------------------
// N2TCM_KIZU情報取得
// OdbcBase* clsPreDB				前工程DB
// MAE_DEFECT_IMAGE_INF* typData	前工程情報
// 戻り値 復帰情報 true:正常 false:異常
//------------------------------------------
bool PreDBManager::GetNewN2TCM_KIZUInf(OdbcBase* clsPreDB, MAE_DEFECT_IMAGE_INF* typData)
{

	CString sql;					// SQL文
	CString wk;
	int sqlrc;
	CString cDbName;
	cDbName = DB_N2TCM_KIZU_DEFECT_INF;

	LOG(em_MSG), "[%s] N2TCM_KIZU情報取得 [管理No:%s] [疵No=%d]", my_sThreadName, typData->cPreKizuKenNo, typData->nMaeKizuNo );

	// 前工程疵情報取得
	sql = "SELECT ";
	sql += "DSWS区分, 長手位置, DS位置, WS位置,";
	sql += "ライン速度,縦分解能,横分解能,外接Xmin,外接Xmax,外接Ymin,外接Ymax,";
	sql += "疵中心位置X,疵中心位置Y,縦画素数,横画素数,画像サイズ正";
	sql += " FROM ";
	sql += cDbName;
	wk.Format(" WHERE 管理No= '%.20s' AND 疵No = %d ", typData->cPreKizuKenNo, typData->nMaeKizuNo);
	sql += wk;

	//// 実行
	if (!clsPreDB->ExecuteDirect(sql)) {
		LOG(em_NON), "%s", sql );
		syslog(SYSNO_DB_EXECUTEDIRECT_ERR, "[%s]", cDbName);
		return false;
	}

	sqlrc = clsPreDB->Fetch();
	if (sqlrc == KIZUODBC_FETCH_OK) {					// データ有り

		clsPreDB->GetData(1, (int*)&typData->typDefectData.dsws);			// DSWS区分		
		clsPreDB->GetData(2, &typData->typDefectData.y);					// 長手位置
		clsPreDB->GetData(3, &typData->typDefectData.ds);					// DS位置
		clsPreDB->GetData(4, &typData->typDefectData.ws);					// WS位置
		clsPreDB->GetData(5, &typData->typDefectData.speed);				// ライン速度
		clsPreDB->GetData(6, &typData->typDefectData.h_res);				// 縦分解能
		clsPreDB->GetData(7, &typData->typDefectData.w_res);				// 横分解能
		clsPreDB->GetData(8, (int*)&typData->typDefectData.box_x_min);		// 外接Xmin
		clsPreDB->GetData(9, (int*)&typData->typDefectData.box_x_max);		// 外接Xmax
		clsPreDB->GetData(10, (int*)&typData->typDefectData.box_y_min);		// 外接Ymin
		clsPreDB->GetData(11, (int*)&typData->typDefectData.box_y_max);		// 外接Ymax
		clsPreDB->GetData(12, (int*)&typData->typDefectData.center_x);		// 疵中心位置X
		clsPreDB->GetData(13, (int*)&typData->typDefectData.center_y);		// 疵中心位置Y
		clsPreDB->GetData(14, (int*)&typData->typDefectData.h_size);		// 縦画素数
		clsPreDB->GetData(15, (int*)&typData->typDefectData.w_size);		// 横画素数
		clsPreDB->GetData(16, (int*)&typData->nSize[0]);					// 画像サイズ

	}
	else if (sqlrc == KIZUODBC_FETCH_NODATE) {		// データ無し

	}
	else {								// エラー
		syslog(SYSNO_DB_FETCH_ERR, "[%s]", cDbName);
		return false;
	}


	cDbName = DB_N2TCM_KIZU_DEFECT_IMG;


	// 前工程疵画像取得	
	sql = "SELECT 正画像 ";
	sql += " FROM ";
	sql += cDbName;
	wk.Format(" WHERE 管理No= '%s' AND 疵No = %d ", typData->cPreKizuKenNo, typData->nMaeKizuNo);
	sql += wk;

	//// 実行
	if (!clsPreDB->ExecuteDirect(sql)) {
		LOG(em_NON), "%s", sql );
		syslog(SYSNO_DB_EXECUTEDIRECT_ERR, "[%s]", cDbName);
		return false;
	}

	sqlrc = clsPreDB->Fetch();
	if (sqlrc == KIZUODBC_FETCH_OK) {					// データ有り

														// 画像情報作成 2014.05.27 J.Wakazono 巻き取り前データ取得失敗対応
		for (int ii = 0; ii<MAX_MAE_IMAGE; ii++) {
			if (0 < typData->nSize[ii]) {
				typData->pImg[ii] = new BYTE[typData->nSize[ii]];
				typData->nCnt++;
			}
			else {
				break;
			}
		}

		clsPreDB->GetDataImage(1, typData->pImg[0], typData->nSize[0]);	// 疵画像

	}
	else if (sqlrc == KIZUODBC_FETCH_NODATE) {		// データ無し

													// 画像情報作成
		typData->nCnt = 0;	// 画像データ数 0

	}
	else {								// エラー
		syslog(SYSNO_DB_FETCH_ERR, "[%s]", cDbName);
		return false;
	}

	return true;
}
// 20221021 2TCM実機化に伴う下工程改造 小島 ---<<<

//------------------------------------------
// 前工程疵情報登録
// OdbcBase* 　　　　clsDB   DBインスタンス
// MAE_DEFECT_INF* 　typData 前工程情報
//------------------------------------------
bool PreDBManager::InsertDefectInf( OdbcBase* clsDB, MAE_DEFECT_IMAGE_INF* typData )
{
	CString sql;					// SQL文
	CString cDbName;
	cDbName = DB_MAE_DEFECT_INF;


	////// 前工程疵情報
	// ヘッダー部
	sql = "INSERT INTO ";
	sql += cDbName;
	sql += " VALUES(";	

	// データ部
	CreateSQL::sqlColumnInsert( &sql, typData->cKizuKenNo, SIZE_KIZUKEN_NO );	// 管理No
	CreateSQL::sqlColumnInsert( &sql, typData->nKizuNo );						// 疵No
	CreateSQL::sqlColumnInsert( &sql, typData->nKouteiSyubetu );				// 工程種別
	CreateSQL::sqlColumnInsert( &sql, typData->nOuDiv );						// 表裏区分
	CreateSQL::sqlColumnInsert( &sql, typData->typDefectData.cycle_div );		// 周期フラグ
	CreateSQL::sqlColumnInsert( &sql, typData->typDefectData.y );				// 長手位置
	CreateSQL::sqlColumnInsert( &sql, typData->typDefectData.dsws );			// DSWS区分
	CreateSQL::sqlColumnInsert( &sql, typData->typDefectData.ds );				// DS位置
	CreateSQL::sqlColumnInsert( &sql, typData->typDefectData.ws );				// WS位置
	CreateSQL::sqlColumnInsert( &sql, typData->typDefectData.speed );			// ライン速度
	CreateSQL::sqlColumnInsert( &sql, typData->typDefectData.pitch );			// ピッチ
	CreateSQL::sqlColumnInsert( &sql, typData->typDefectData.roolkei );			// 径
	CreateSQL::sqlColumnInsert( &sql, typData->typDefectData.kizu_wid );		// 疵幅
	CreateSQL::sqlColumnInsert( &sql, typData->typDefectData.kizu_len );		// 疵長さ
	CreateSQL::sqlColumnInsert( &sql, typData->typDefectData.h_res );			// 縦分解能
	CreateSQL::sqlColumnInsert( &sql, typData->typDefectData.w_res );			// 横分解能
	CreateSQL::sqlColumnInsert( &sql, typData->typDefectData.box_x_min );		// 外接Xmin
	CreateSQL::sqlColumnInsert( &sql, typData->typDefectData.box_x_max );		// 外接Xmax
	CreateSQL::sqlColumnInsert( &sql, typData->typDefectData.box_y_min );		// 外接Ymin
	CreateSQL::sqlColumnInsert( &sql, typData->typDefectData.box_y_max );		// 外接Ymax
	CreateSQL::sqlColumnInsert( &sql, typData->typDefectData.center_x );		// 疵中心位置X
	CreateSQL::sqlColumnInsert( &sql, typData->typDefectData.center_y );		// 疵中心位置Y
	CreateSQL::sqlColumnInsert( &sql, typData->typDefectData.h_size );			// 縦画素数
	CreateSQL::sqlColumnInsert( &sql, typData->typDefectData.w_size );			// 横画素数
	CreateSQL::sqlColumnInsert( &sql, (int)typData->nSize[0] );					// 画像サイズ1
	CreateSQL::sqlColumnInsert( &sql, (int)typData->nSize[1] );					// 画像サイズ2
	CreateSQL::sqlColumnInsert( &sql, (int)typData->nSize[2] );					// 画像サイズ3
	CreateSQL::sqlColumnInsert( &sql, (int)typData->nSize[3] );					// 画像サイズ4
	CreateSQL::sqlColumnTrim( &sql );									// 最後のカンマを取り除く
	sql += ")";

	// 実行
	if( ! clsDB->ExecuteDirect( sql ) ) {
		LOG(em_NON), "%s", sql );
		LOG(em_ERR), "[%s] 疵情報登録エラー [%s]", my_sThreadName, cDbName );
		syslog(SYSNO_DB_EXECUTEDIRECT_ERR, "[%s]", cDbName);
		return false;
	}
	
	return true;

}

//------------------------------------------
// 前工程疵画像登録
// OdbcBase* 　　　　clsDB   DBインスタンス
// MAE_DEFECT_INF* 　typData 前工程情報
//------------------------------------------
bool PreDBManager::InsertDefectImg( OdbcBase* clsDB, MAE_DEFECT_IMAGE_INF* typData )	
{
	
	CString sql;					// SQL文
	CString cDbName;
	cDbName = DB_MAE_DEFECT_IMG;

	////// 前工程疵情報
	// ヘッダー部
	sql = "INSERT INTO ";
	sql += cDbName;
	sql += " VALUES(";	

	// データ部
	CreateSQL::sqlColumnInsert( &sql, typData->cKizuKenNo, SIZE_KIZUKEN_NO );	// 管理No 
	CreateSQL::sqlColumnInsert( &sql, typData->nKizuNo );						// 疵No
	for( int ii = 0; ii< MAX_MAE_IMAGE; ii++ ) {
		// 2014.05.27 J.Wakazono 巻き取り前データ取得失敗対応
		if( 0 < typData->nSize[ii] ) {
			sql += " ?,";
		} else {
			sql += " NULL,";
		}
	}
	CreateSQL::sqlColumnTrim( &sql );									// 最後のカンマを取り除く
	sql += ")";

	// 実行
	if( ! clsDB->SqlBinalyWrite( sql, typData->nCnt, typData->nSize, (LPVOID*)typData->pImg ) ) {
		LOG(em_NON), "%s", sql );
		LOG(em_ERR), "[%s] 画像登録失敗", my_sThreadName );
		LOG(em_ERR), "[%s] 画像1 サイズ [%d]", my_sThreadName, typData->nSize[0] );
		LOG(em_ERR), "[%s] 画像2 サイズ [%d]", my_sThreadName, typData->nSize[1] );
		LOG(em_ERR), "[%s] 画像3 サイズ [%d]", my_sThreadName, typData->nSize[2] );
		LOG(em_ERR), "[%s] 画像4 サイズ [%d]", my_sThreadName, typData->nSize[3] );
		syslog(SYSNO_DB_EXECUTEDIRECT_ERR, "[%s]", cDbName);
		return false;
	}
	
	return true;

}

//------------------------------------------
// 前工程疵情報更新
// OdbcBase* clspDB		DBインスタンス
// DeliveryPreDB* pDeli	キューデータ
//------------------------------------------
void PreDBManager::UpdateCoilLine( OdbcBase* clspDB, DeliveryPreDB* pDeli )
{
	CString sql;					// SQL文
	CString wk;
	CString cDbName;
	cDbName = DB_MAE_COIL_LINE;

	////// 前工程ライン別コイル情報の処理ステータスを0に更新
	//// SQL文作成
	sql = "UPDATE ";
	sql += cDbName;
	sql += " SET ";
	sql += " 処理ステータス = 0 ";
	wk.Format(" WHERE 管理No = '%s' ", pDeli->cKizuKenNo );
	sql += wk;
	wk.Format(" AND 前工程管理No = '%s' ", pDeli->cPreKizuKenNo );
	sql += wk;
	wk.Format(" AND 表裏区分 = %d ", pDeli->nOuDiv );
	sql += wk;
		
	// 実行
	if( ! clspDB->ExecuteDirect( sql ) ) {
		LOG(em_NON), "%s", sql );
		LOG(em_ERR), "[%s] 前工程ライン別コイル情報UPDATEエラー [%s]", my_sThreadName, cDbName );
		syslog(SYSNO_DB_EXECUTEDIRECT_ERR, "[%s]", cDbName);
	}

}

// 2014.07.25 川原  所間連携    >>>>>>>>
//------------------------------------------
// 自所材、他所材の判定
// Char* LineCode		前工程コード
// 戻り値:
//------------------------------------------
int PreDBManager::CheckAnotherMill(char* LineCode)
{
    int nRet;

    // 他所材判定取得
    nRet    = mcls_pParam->GetTasyoZai(LineCode);

    return nRet;
}
// 2014.07.25 川原  所間連携    >>>>>>>>

//--------------------------------------------
// 接続結果からアラームNoを決定
// EM_SELECT_DB nSelect		前工程蓄積 or 統合
// 戻り値: アラームNo
//--------------------------------------------
int PreDBManager::GetAlertNo(PreDBSelectManager::EM_SELECT_DB nSelect,EM_PRE_LINE nPreLine)
{
	PreDBSelectManager::TYP_SELECT_STATUS typStatus;

    CString	strWk;		// イベントログ出力用
	switch(nSelect){
		case PreDBSelectManager::PRE_DB:	strWk = "蓄積";	break;
		case PreDBSelectManager::SHARE_DB:	strWk = "統合";	break;
	}

    // 選択サーバーの接続チェック結果を取得
	switch(nPreLine){
		case PRE_LINE_2TCM:
			mcls_pSelect2TCM->GetSelectStatus(nSelect,&typStatus);
			break;
		case PRE_LINE_3TCM:
			mcls_pSelect3TCM->GetSelectStatus(nSelect, &typStatus);
			break;
			// 20200830 冷延　検査装置試験機　ハード・ソフト（2TCM穴検更新） 溝口 ----->>>
		case PRE_LINE_N2TCM_ANA:
			mcls_pSelectN2TCM_ANA->GetSelectStatus(nSelect, &typStatus);
			break;
			// 20200830 冷延　検査装置試験機　ハード・ソフト（2TCM穴検更新） 溝口 <<<-----
		// 20221021 2TCM実機化に伴う下工程改造 小島 --->>>
		case PRE_LINE_N2TCM_KIZU:
			mcls_pSelectN2TCM_KIZU->GetSelectStatus(nSelect, &typStatus);
			break;
		// 20221021 2TCM実機化に伴う下工程改造 小島 ---<<<

	}

    // チェック結果からアラームNoを決定
	if(PreDBSelectManager::SELECT_NG == typStatus.nPing){
		LOG(em_ERR), "[%s] Ping応答エラー [%s]", my_sThreadName, strWk);	
		return ALERT_NG_PING;
	}else if(PreDBSelectManager::SELECT_NG == typStatus.nConnect){
		LOG(em_ERR), "[%s] ODBC接続エラー [%s]", my_sThreadName, strWk);		
		return ALERT_NG_CONNECT;
	}else if(PreDBSelectManager::SELECT_NG == typStatus.nSearchCoil){
		LOG(em_ERR), "[%s] コイル実績なし [%s]", my_sThreadName, strWk);		
		return ALERT_NG_SEARCH;
	}
	LOG(em_MSG), "[%s] 接続成功 [%s]", my_sThreadName, strWk);	
	return ALERT_OK;
}

//------------------------------------------
// 前工程疵情報書込み完了通知送信
// char* cMeiNo		命令No
// char* cCoilNo	コイルNo
//------------------------------------------
void PreDBManager::SendMailDefect(char* cMeiNo, char* cCoilNo)
{
	char cMeiNoBuff[SIZE_MEI_NO + 1];
	char cCoilNoBuff[SIZE_COIL_NO + 1];
	COMMON_QUE		que;
	memset(&que, 0x00, sizeof(que));

	memset(&cMeiNoBuff, 0x00, sizeof(cMeiNoBuff));
	strcpy(cMeiNoBuff, cMeiNo);
	KizuFunction::Trim(cMeiNoBuff);

	memset(&cCoilNoBuff, 0x00, sizeof(cCoilNoBuff));
	strcpy(cCoilNoBuff, cCoilNo);
	KizuFunction::Trim(cCoilNoBuff);

	que.nLineNo = getlineid();

	//// 統括へ通知
	que.nEventNo = FACT_TO_DEFECT_12;						// 要求モード
	sprintf(que.mix.cdata[0], "%04s", cMeiNoBuff);			// 命令No
	sprintf(que.mix.cdata[1], "%.12s", cCoilNoBuff);		// コイルNo


	send_mail(TO_DEFECT, ".", &que);

	LOG(em_MSG), "[%s] 前工程疵情報書込み完了通知送信 [命令No = %s,コイルNo = %s]", my_sThreadName, cMeiNoBuff, cCoilNoBuff );

}