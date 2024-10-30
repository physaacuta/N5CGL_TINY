#include "StdAfx.h"
#include "PreProcManager.h"

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
PreProcManager::PreProcManager(void) :
ThreadManager("PreProcManager")
{

	//// 共有メモリ
	// コイル情報テーブル
	mem_connect( &m_hCoil, (LPVOID *)&mtbl_pCoil, TBL_TO_COIL_NAME );
	if( m_hCoil == NULL ) {
		syslog(SYSNO_MEM_OPEN, "コイル情報テーブル[%s] アクセス失敗", TBL_TO_COIL_NAME);
		_ASSERT( FALSE );
	}

}

//------------------------------------------
// デストラクタ
//------------------------------------------
PreProcManager::~PreProcManager(void)
{

	//// 共有メモリ開放
	mem_close( &m_hCoil, (LPVOID *)&mtbl_pCoil );							// コイル情報
}

//------------------------------------------
// 関数初期化
//------------------------------------------
void PreProcManager::Alloc()
{
	while( true ) {
		if( mcls_pParam->ReadParam_All() ) break;

		LOG( em_MSG), "[%s] パラメータ読込失敗(DB接続異常？！)。リトライ中・・・", my_sThreadName );
		Sleep( 10000 );			
	}
}

//------------------------------------------
// スレッド開始前処理 (スレッド実行直後(シグナルセット前)にコールバック)
//------------------------------------------
int PreProcManager::ThreadFirst()
{

	//// イベント設定
	HANDLE	hArray[] = {gque_Deli.g_evSem
						};
	SetEventNum( sizeof(hArray)/sizeof(HANDLE), hArray );

	return 0;
}

//------------------------------------------
// スレッドイベント発生
// int nEventNo イベントNo (0オリジン)
//------------------------------------------
void PreProcManager::ThreadEvent( int nEventNo )
{
	COMMON_QUE* pDeli;				// スレッドキュー受け渡し

	////// シグナルの条件
	enum {	EV_QUE = 0,						// メール要求(コイル情報受信通知,FF前工程疵情報受信通知)
			};

	////// シグナル条件分け
	switch ( nEventNo ) {
//-----------------------------------------------------------------------------------------------
	case EV_QUE: 						// メールスロット 通知
		pDeli = gque_Deli.GetFromHead();

		switch( pDeli->nEventNo ) {
//-----------------------------------------------------------
		case FACT_TO_MAEDAT_02:			// コイル情報受信通知

			// コイル通板順位テーブル登録
			// 前工程コイル情報登録
			if( ! MakeMaeCoilInf( pDeli ) ) {
				LOG(em_ERR), "[%s] コイル情報登録失敗 [管理No=%s]", my_sThreadName, pDeli->mix.cdata[0] );
				syslog(PARAM_SYSLOG_BASENO + 1, "[管理No=%s]", pDeli->mix.cdata[0] );
			}
			
			// FF送信要求
			mcls_pFFSend->SetFFSend();
			break;

//-----------------------------------------------------------
		case FACT_TO_MAEDAT_03:			// FF前工程疵情報受信通知

			// パラメータ読み込み
			mcls_pParam->ReadParam_All();

			// FF前工程情報登録
			MakeMaeDefectInf( pDeli );

			// 前工程編集管理へ通知
			mcls_pPreDefect->SetGetData();
			break;

//-----------------------------------------------------------
		case E_PD_END:
			break;

//-----------------------------------------------------------
		}
		// 開放
		SAFE_DELETE(pDeli);

		return;

//-----------------------------------------------------------------------------------------------
	default :
		// ありえない！！
		LOG(em_ERR), "[%s] ThreadEvent EventNo NG=%d", my_sThreadName, nEventNo );
		return;
	}
}

//------------------------------------------
// スレッド終了前処理 (終了シグナル後にコールバック)
//------------------------------------------
int PreProcManager::ThreadLast()
{
	COMMON_QUE* pDeli;

	// 受け渡しキューに格納されている項目を全て開放
	while( true ) {
		pDeli = gque_Deli.GetFromHead( 0 );
		if( pDeli == NULL ) break;

		// 開放
		SAFE_DELETE_ARRAY( pDeli )
	}

	return 0;
}

//------------------------------------------
// 前工程コイル情報登録メイン処理
// COMMON_QUE* pQue キュー受け渡し
//-----------------------------------------
bool PreProcManager::MakeMaeCoilInf( COMMON_QUE* pQue )
{
	PRO_TYPE_DATA_COILINF& coil = ((PRO_TYPE_COILINF*)mtbl_pCoil->rec[pQue->mix.idata[0]].val)->data;	// コイル情報
	int nDataCnt;								// データ件数
	char cMeiNo[ SIZE_MEI_NO+1 ];				// 命令No
	char cKizukenNo[ SIZE_KIZUKEN_NO+1 ];		// 管理No
	PRO_TYPE_DATA_COILINF::NEXT_COIL* pnext_coil;	// 次コイル情報

	//// 初期化
	nDataCnt = 0;
	memset( cKizukenNo, 0, sizeof(cKizukenNo) );
	memcpy( cKizukenNo, pQue->mix.cdata[0], SIZE_KIZUKEN_NO );



	//// ODBC接続クラス インスタンス生成
	OdbcBase clsDB;
	if( ! clsDB.Connect( TASKINI_NAME ) ) {
		// データベース接続エラー
		LOG(em_ERR), "[%s] DB接続エラー", my_sThreadName );
		syslog(SYSNO_DB_CONNECT_ERR, "");
		return false;
	}

	// 予定コイル分ループ
	for( int ii = 0; ii < PRO_SIZE_NEXTINF_CNT; ii++ ) {

		// 次コイル情報
		pnext_coil = &coil.next_coil[ii];

		// キー情報 命令No
		memset( cMeiNo, 0x00, sizeof(cMeiNo) );
		memcpy( cMeiNo, pnext_coil->KeyInf.cMeiNo, SIZE_MEI_NO );

		//命令Noが空白の時は、登録しない
		if( ' ' == cMeiNo[0] || 0x00 == cMeiNo[0] ) continue;

		// ウォーマー材は要求しない
		//if('1' == pnext_coil->cWalmKubun[0]) continue;

		//// コイル通板順位テーブル登録
		if( ! InsertCoilOrder( &clsDB, cKizukenNo, (ii + 1), pnext_coil ) ) {
			return false;
		}

		//// 前工程コイル情報有無チェック
		if( ! SelectCountMaeCoilInf( &clsDB, &pnext_coil->KeyInf, &nDataCnt ) ) {
			return false;
		}

		if( 0 == nDataCnt ) {
			// 未登録の場合は、登録する

			//// 前工程コイル情報登録
			if( ! InsertMaeCoilInf( &clsDB, cKizukenNo, (ii + 1), pnext_coil ) ) {
				return false;
			}
		}
	}

	//// 入側フロント端板長さ 更新
	if( ! UpdateMaeCoilInf( &clsDB, mtbl_pCoil->rec[pQue->mix.idata[0]].basedata.cCoilKey, mtbl_pCoil->rec[pQue->mix.idata[0]].basedata.nCutLenF) ) {
		return false;
	}

	return true;
}

//------------------------------------------
// 前工程コイル情報 件数取得処理
// OdbcBase* clspDB		DBインスタンス
// PRO_TYPE_KEY* keyinf	キー情報
// int* nDataCnt		データ件数
//------------------------------------------
bool PreProcManager::SelectCountMaeCoilInf( OdbcBase* clspDB, PRO_TYPE_KEY* keyinf, int* nDataCnt )
{
	char cMeiNo[SIZE_MEI_NO + 1];			// 命令No(文字列)
	CString sql;							// SQL文
	CString cDbName;
	cDbName = DB_MAE_COIL_INF;
	CString wk;
	CTime t = CTime::GetCurrentTime() - CTimeSpan( 1,0,0,0 );		// 1日前
	CString strTime = t.Format("'%Y/%m/%d %H:%M:%S'");

	// FFへの要求は命令Noのみで実施。
	// FFからのコイルNoは、"-"が削除されるためキー情報の一致確認はしない
	memset(cMeiNo, 0x00, sizeof(cMeiNo));
	memcpy(cMeiNo, keyinf->cMeiNo, sizeof(SIZE_MEI_NO));

	sql =  "SELECT COUNT(*)";
	sql += " FROM ";
	sql += cDbName;
	wk.Format( " WHERE キー情報 LIKE '%s%%' AND 登録日付 > %s", cMeiNo, strTime );
	//wk.Format( " WHERE キー情報 = '%s' AND 登録日付 > %s", cKeyinf, strTime );
	sql += wk;
	
	//// 実行
	// 件数取得
	if( ! clspDB->GetSelectKey( sql, nDataCnt ) ) {
		LOG(em_NON), "%s", sql );
		LOG(em_ERR), "[%s] 前工程コイル情報実行エラー [%s]", my_sThreadName, cDbName );
		syslog(SYSNO_DB_FETCH_ERR, "[%s]", cDbName);
		return false;
	}

	return true;
}

//------------------------------------------
// 前工程コイル情報 取得処理
// OdbcBase* clspDB		DBインスタンス
// PRO_TYPE_KEY* keyinf	キー情報
// char* cKizuKenNo		仮管理NO
// char* nFFFlg			FF前工程疵情報受信フラグ
// char* nKariFlg		仮管理Noフラグ
// 戻り値　ture:データ有り false:データ無し
//-----------------------------------------
bool PreProcManager::SelectMaeCoilInf( OdbcBase* clspDB, PRO_TYPE_KEY* keyinf, char* cKizuKenNo, int* nFFFlg, int* nKariFlg )
{
	
	char cMeiNo[SIZE_MEI_NO + 1];			// 命令No(文字列)
	CString sql;							// SQL文
	CString cDbName;
	cDbName = DB_MAE_COIL_INF;
	CString wk;
	CTime t = CTime::GetCurrentTime() - CTimeSpan( 1, 0, 0, 0 );	// 1日前
	CString strTime = t.Format("'%Y/%m/%d %H:%M:%S'");

	// FFへの要求は命令Noのみで実施。
	// FFからのコイルNoは、"-"が削除されるためキー情報の一致確認はしない
	memset(cMeiNo, 0x00, sizeof(cMeiNo));
	memcpy(cMeiNo, keyinf->cMeiNo, sizeof(SIZE_MEI_NO));

	//// SQL文作成
	sql =  "SELECT";
	sql += "  管理No";
	sql += " ,FF前工程疵情報受信フラグ";
	sql += " ,仮管理Noフラグ ";
	sql += " FROM ";
	sql += cDbName;
	wk.Format(" WHERE キー情報 LIKE '%s%%' AND 登録日付 > %s", cMeiNo, strTime);
	//wk.Format(" WHERE キー情報 = '%s' AND 登録日付 > %s", cKeyinf, strTime);
	sql += wk;
	
	//// 実行
	if( ! clspDB->ExecuteDirect( sql ) ) {
		LOG(em_NON), "%s", sql );
		LOG(em_ERR), "[%s] 前工程コイル情報実行エラー [%s]", my_sThreadName, cDbName );
		syslog(SYSNO_DB_EXECUTEDIRECT_ERR, "[%s]", cDbName);
		return false;
	}
	
	//// 取得
	int sqlrc = clspDB->Fetch();
	if( sqlrc == KIZUODBC_FETCH_OK ) {					// データ有り
		
		clspDB->GetData( 1, cKizuKenNo, SIZE_KIZUKEN_NO );		// 管理No
		clspDB->GetData( 2, nFFFlg );							// FF前工程疵情報受信フラグ
		clspDB->GetData( 3, nKariFlg );							// 仮管理Noフラグ


	} else if( sqlrc == KIZUODBC_FETCH_NODATE ) {		// データ無し
		return false;

	} else {								// エラー
		LOG(em_NON), "%s", sql );
		LOG(em_ERR), "[%s] 前工程コイル情報取得エラー [%s]", my_sThreadName, cDbName );
		syslog(SYSNO_DB_FETCH_ERR, "[%s]", cDbName);
		return false;
	}

	return true;
}

//------------------------------------------
// コイル通板順位テーブル登録
// OdbcBase* clspDB		DBインスタンス
// char* cKizukenNo		疵検管理NO
// int nTujun			通順
// PRO_TYPE_KEY* keyinf	キー情報
// PRO_TYPE_DATA_COILINF::NEXT_COIL* pnext_coil 次コイル情報
//------------------------------------------
bool PreProcManager::InsertCoilOrder( OdbcBase* clspDB, char* cKizukenNo, int nTujun, PRO_TYPE_DATA_COILINF::NEXT_COIL* pnext_coil )
{
	CString sql;					// SQL文
	CString cDbName;
	cDbName = DB_COIL_ORDER;

	//// コイル位置情報
	// ヘッダー部
	sql = "INSERT INTO ";
	sql += cDbName;
	sql += " VALUES(";	

	// データ部
	CreateSQL::sqlColumnInsert( &sql, cKizukenNo, SIZE_KIZUKEN_NO );						// 管理No
	CreateSQL::sqlColumnInsert( &sql, nTujun );												// 通順
	CreateSQL::sqlColumnInsert( &sql, (char*)&pnext_coil->KeyInf, sizeof(PRO_TYPE_KEY) );	// キー情報
	CreateSQL::sqlColumnInsert(&sql, pnext_coil->nFrontLen);								// 入側フロント端板長さ
	CreateSQL::sqlColumnInsert( &sql, pnext_coil->cKouteiCode, SIZE_KOUTEI_CODE );			// 前工程コード
	CreateSQL::sqlColumnTrim( &sql );														// 最後のカンマを取り除く
	sql += ")";

	// 実行
	if( ! clspDB->ExecuteDirect( sql ) ) {
		LOG(em_NON), "%s", sql );
		LOG(em_ERR), "[%s] コイル通板順位テーブル登録エラー [%s]", my_sThreadName, cDbName );
		syslog(SYSNO_DB_EXECUTEDIRECT_ERR, "[%s]", cDbName);
		return false;
	}

	LOG(em_MSG), "[%s] コイル通板順位テーブル登録 [管理No=%s][通順=%.4d]", my_sThreadName, cKizukenNo, nTujun );
	return true;
}

//------------------------------------------
// 前工程コイル情報登録
// OdbcBase* clspDB			 DBインスタンス
// char* cKizukenNo          管理NO
// int   nTujun		         通順
// PRO_TYPE_DATA_COILINF::NEXT_COIL* pnext_coil 次コイル情報
//-----------------------------------------
bool PreProcManager::InsertMaeCoilInf( OdbcBase* clspDB, char* cKizukenNo, int nTujun,
									   PRO_TYPE_DATA_COILINF::NEXT_COIL* pnext_coil )
{
	CString sql;					// SQL文
	CString cDbName;
	cDbName = DB_MAE_COIL_INF;
	char cWkKizukenNo[SIZE_KIZUKEN_NO];
	char cKeyinf[SIZE_COIL_KEY+1];

	sprintf( cWkKizukenNo, "%.17s%02d",cKizukenNo, nTujun );

	memset(cKeyinf, 0x00, sizeof(cKeyinf));
	memcpy(cKeyinf, &pnext_coil->KeyInf, sizeof(PRO_TYPE_KEY));

	////// コイル位置情報
	// ヘッダー部
	sql = "INSERT INTO ";
	sql += cDbName;
	sql += " VALUES(";	

	// データ部
	CreateSQL::sqlColumnInsert(&sql, cWkKizukenNo, SIZE_KIZUKEN_NO);				// 仮管理No
	CreateSQL::sqlColumnAddTime(&sql, true);										// 登録日付
	CreateSQL::sqlColumnInsert(&sql, (char*)&pnext_coil->KeyInf, SIZE_COIL_KEY);	// キー情報
	CreateSQL::sqlColumnInsert(&sql, NULL, 0);										// ID
	CreateSQL::sqlColumnInsert(&sql, -1);											// 仮管理Noフラグ(-1:仮管理No)
	CreateSQL::sqlColumnInsert(&sql, -1);											// FF前工程疵情報受信フラグ(-1:前工程疵情報未受信)

	CreateSQL::sqlColumnInsert(&sql, pnext_coil->cKouteiCode, SIZE_KOUTEI_CODE);	// 工程コード

	CreateSQL::sqlColumnInsert(&sql, pnext_coil->KeyInf.cMeiNo,  SIZE_MEI_NO);		// 命令No
	CreateSQL::sqlColumnInsert(&sql, pnext_coil->KeyInf.cCoilNo, SIZE_COIL_NO);		// コイルNo
	CreateSQL::sqlColumnInsert(&sql, 0);											// 入側フロント端板長さ
	CreateSQL::sqlColumnInsert(&sql, NULL, 0);										// 冷延コイル番号
	CreateSQL::sqlColumnInsert(&sql, NULL, 0);										// 冷延コイル重量
	CreateSQL::sqlColumnInsert(&sql, NULL, 0);										// 冷延コイル長さ
	CreateSQL::sqlColumnInsert(&sql, NULL, 0);										// 冷延厚
	CreateSQL::sqlColumnInsert(&sql, NULL, 0);										// 板厚公差MIN
	CreateSQL::sqlColumnInsert(&sql, NULL, 0);										// 板厚公差MAX
	CreateSQL::sqlColumnInsert(&sql, NULL, 0);										// 板巾_向先1
	CreateSQL::sqlColumnInsert(&sql, NULL, 0);										// 検査基準
	CreateSQL::sqlColumnInsert(&sql, NULL, 0);										// 捲き方向
	CreateSQL::sqlColumnInsert(&sql, NULL, 0);										// 材料巾
	CreateSQL::sqlColumnInsert(&sql, NULL, 0);										// 板幅公差MIN_向先1
	CreateSQL::sqlColumnInsert(&sql, NULL, 0);										// 板幅公差MAX_向先1
	CreateSQL::sqlColumnInsert(&sql, NULL, 0);										// 長さ_ロット枚数_向先1
	CreateSQL::sqlColumnInsert(&sql, NULL, 0);										// 内径_向先1
	CreateSQL::sqlColumnInsert(&sql, NULL, 0);										// 板厚
	CreateSQL::sqlColumnInsert(&sql, NULL, 0);										// 入側内径
	CreateSQL::sqlColumnInsert(&sql, NULL, 0);										// 剪断長_向先1
	CreateSQL::sqlColumnInsert(&sql, NULL, 0);										// 前工程コード
	CreateSQL::sqlColumnInsert(&sql, NULL, 0);										// 仮想RCL疵検管理No
	CreateSQL::sqlColumnInsert(&sql, NULL, 0);										// 母体となる冷延コイルNo
	CreateSQL::sqlColumnInsert(&sql, NULL, 0);										// 採取開始位置長さ
	CreateSQL::sqlColumnInsert(&sql, NULL, 0);										// 採取終了位置長さ
	CreateSQL::sqlColumnInsert(&sql, NULL, 0);										// 採取子番
	CreateSQL::sqlColumnTrim(&sql);													// 最後のカンマを取り除く
	sql += ")";

	// 実行
	if( ! clspDB->ExecuteDirect( sql ) ) {
		LOG(em_NON), "%s", sql );
		LOG(em_ERR), "[%s] 前工程コイル情報登録エラー [%s]", my_sThreadName, cDbName );
		syslog(SYSNO_DB_EXECUTEDIRECT_ERR, "[%s]", cDbName);
		return false;
	}
	LOG(em_MSG), "[%s] 前工程コイル情報登録 [仮管理No=%s][キー情報=%s]",
		my_sThreadName, cWkKizukenNo, cKeyinf);
	return true;
}

//------------------------------------------
// FF前工程疵情報取得処理
// OdbcBase* clspDB		DBインスタンス
// char* cId			ID
// PRO_TYPE_KEY* keyinf	キー情報
// FF_MAE_INF* typData	FF前工程実績
// 戻り値　ture:正常 false:異常
//------------------------------------------
bool PreProcManager::SelectBufFfRMaeinf( OdbcBase* clspDB, char* cId, PRO_TYPE_KEY* pkeyinf, FF_MAE_INF* typData ) 
{
	CString sql;					// SQL文
	CString cDbName;
	cDbName = DB_BUF_FF_R_MAEINF;
	CString wk;

	//// SQL文作成(受信伝文取得)
	sql =  "SELECT";
	sql += " 生伝文1, 生伝文2, キー情報";
	sql += " FROM ";
	sql += cDbName;
	wk.Format(" WHERE ID = '%s'",cId);
	sql += wk;

	//// 実行
	if( ! clspDB->ExecuteDirect( sql ) ) {
		LOG(em_NON), "%s", sql );
		LOG(em_ERR), "[%s] FF前工程疵情報受信バッファ実行エラー [%s]", my_sThreadName, cDbName );
		syslog(SYSNO_DB_EXECUTEDIRECT_ERR, "[%s]", cDbName);
		return false;
	}

	//// 取得
	int sqlrc = clspDB->Fetch();
	if( sqlrc == KIZUODBC_FETCH_OK ) {					// データ有り


		clspDB->GetDataImage( 1, &typData->torb[0], sizeof(FF_MAE_JISSEKI) );		// 生伝文1
		clspDB->GetDataImage( 2, &typData->torb[1], sizeof(FF_MAE_JISSEKI) );		// 生伝文2
		clspDB->GetDataImage( 3, pkeyinf, SIZE_COIL_KEY );							// キー情報

	} else if( sqlrc == KIZUODBC_FETCH_NODATE ) {		// データ無し
		return false;	
	} else {								// エラー
		LOG(em_NON), "%s", sql );
		LOG(em_ERR), "[%s]  FF前工程疵情報受信バッファ取得エラー [%s]", my_sThreadName, cDbName );
		syslog(SYSNO_DB_FETCH_ERR, "[%s]", cDbName);
		return false;
	}

	return true;

}

//------------------------------------------
// FF前工程情報登録メイン処理
// COMMON_QUE* pQue キュー受け渡し
//-----------------------------------------
void PreProcManager::MakeMaeDefectInf( COMMON_QUE* pQue )
{
	PRO_TYPE_KEY keyinf;				// キー情報
	FF_MAE_INF typMaejis;				// FF前工程実績
	int nFFFlg = 0;						// FF前工程疵情報展開フラグ
	int nKariFlg = 0;					// 仮管理Noフラグ
	char cMeiNo[SIZE_MEI_NO+1];			// 命令No
	char cKeyinf[SIZE_COIL_KEY+1];		// キー情報(文字列)
	char cKizuKenNO[SIZE_KIZUKEN_NO];	// 管理No
	

	// 初期化
	memset( &keyinf, 0x00, sizeof(keyinf) );
	memset( &typMaejis, 0x00, sizeof(typMaejis) );
	memset( cKizuKenNO, 0x00, sizeof(cKizuKenNO) );


	//// ODBC接続クラス インスタンス生成
	OdbcBase clsDB;
	if( ! clsDB.Connect( TASKINI_NAME ) ) {
		// データベース接続エラー
		LOG(em_ERR), "[%s] DB接続エラー", my_sThreadName );
		syslog(SYSNO_DB_CONNECT_ERR, "");
		return;
	}

	//// FF前工程疵情報 取得
	if( ! SelectBufFfRMaeinf( &clsDB, pQue->fc.data, &keyinf, &typMaejis ) ) {
		LOG(em_ERR), "[%s] FF前工程実績情報取得失敗 [ID=%s]", my_sThreadName, &pQue->fc.data[0] );
		return;
	}

	// 命令No
	memset( cMeiNo, 0x00, sizeof(cMeiNo) );
	memcpy( cMeiNo, keyinf.cMeiNo, SIZE_MEI_NO );
	memset( cKeyinf, 0x00, sizeof(cKeyinf) );
	memcpy( cKeyinf, &keyinf, sizeof(PRO_TYPE_KEY) );
	
	// FFに当該コイル番号のデータが存在したか
	if( 0 == memcmp( typMaejis.torb[0].typFFData.Hd.ReiCoilNo, "XXXXXXXXXX", sizeof(typMaejis.torb[0].typFFData.Hd.ReiCoilNo) ) ) {
		LOG(em_ERR), "[%s] FFサーバーに該当命令Noコイルなし [命令No=%s][キー情報=%s]", my_sThreadName, cMeiNo, cKeyinf );
		syslog(PARAM_SYSLOG_BASENO + 0 ,"[命令No=%s][キー情報=%s]", cMeiNo, cKeyinf); // // 0 : FFサーバーに該当命令Noコイル無し
		return;
	}

	//// 仮管理Noフラグ、FF前工程疵情報受信フラグ取得
	if( ! SelectMaeCoilInf( &clsDB, &keyinf, cKizuKenNO, &nFFFlg, &nKariFlg ) ) {
		LOG(em_ERR), "[%s] 前工程コイル情報なし [キー情報=%s]", my_sThreadName, cKeyinf );
		return;
	}
	// FF前工程疵情報受信チェック
	if( -1 != nFFFlg ) {
		LOG(em_WAR), "[%s] 前工程疵情報正常受信済み [キー情報=%s]", my_sThreadName, cKeyinf );
		return;
	}
	// 仮管理Noチェック
	if( -1 != nKariFlg ) {
		LOG(em_WAR), "[%s] 管理No展開済み [キー情報=%s]", my_sThreadName, cKeyinf );
		return;
	}

	//// 前工程疵情報登録
	if( ! InsertDefectInfMain( &clsDB, cKizuKenNO, &typMaejis ) ) {
		LOG(em_ERR), "[%s] 前工程疵情報更新失敗 [命令No=%s][仮管理No=%s]", my_sThreadName, cMeiNo, cKizuKenNO );
		return;
	}

	//// 前工程コイル情報更新
	if( ! UpdateCoilInf( &clsDB, cKizuKenNO, pQue->fc.data, &typMaejis ) ) {
		LOG(em_ERR), "[%s] 前工程コイル情報更新失敗 [命令No=%s] [仮管理No=%s]", my_sThreadName, cMeiNo, cKizuKenNO );
		return;
	}


	LOG(em_MSG), "[%s] 前工程コイル情報更新 [命令No=%s] [仮管理No=%s]", my_sThreadName, cMeiNo, cKizuKenNO );
}

//------------------------------------------
// FF前工程情報登録メイン処理
// OdbcBase* clsInsDB	登録DBインスタンス
// char* cKizuKenNO　疵検管理No
// FF_MAE_INF* typData FF前工程実績
// 戻り値　ture:正常 false:異常
//-----------------------------------------
bool PreProcManager::InsertDefectInfMain( OdbcBase* clsInsDB, char* cKizuKenNO, FF_MAE_INF* typData )
{
	int ii;							// カウンタ
	int jj;							// カウンタ
	int kk;							// カウンタ
	int nKizuNo;					// 疵No
	FF_MAE_JISSEKI*		pFfMae;		// 変数簡略化用 前工程疵情報伝文本体
	FF_MAE_TYPE_INF*	pKizuInf;	// 変数簡略化用 疵検出情報
	FF_MAE_TYPE_DATA*	pKizuData;	// 変数簡略化用 疵情報
	int					nDspMode;	// 前工程表示モード

	//初期化
	nKizuNo = 1;

	// 表と裏のループ処理
	for( ii = 0; ii < NUM_MEN; ii++ ) {
		// 前工程疵情報伝文本体 変数簡略化
		pFfMae = &typData->torb[ii].typFFData;


		// 疵検出情報ループ
		for(jj = 0; jj < MAX_FF_MAE_DINF_CNT; jj++) {


			// 疵検出情報 変数簡略化
			pKizuInf = &pFfMae->Inf[jj];

			//疵検出工程が空白の時は、登録しない
			if( 0 == memcmp( pKizuInf->Kizukbn, "  ", sizeof(pKizuInf->Kizukbn) ) ||
				0x00 == pKizuInf->Kizukbn[0] ) {
				continue;
			}

			//// 前工程表示モード取得
			nDspMode = mcls_pParam->GetDspMode( pKizuInf->Kizukbn );

			if( -1 == nDspMode ) {
				// 前工程表示モード取得エラー(DB登録無し)
				LOG(em_INF), "[%s] 前工程マスター未登録 [工程コード=%.2s]", my_sThreadName, pKizuInf->Kizukbn );
#if 0	// 前工程マスター未登録はアラーム出力不要(表示対象外の工程は登録しないため)
				syslog(373, "[工程コード=%.2s]", pKizuInf->Kizukbn );
#endif
			}

			//// 前工程ライン別コイル情報テーブル登録
			if( ! InsertMaeCoilLine( clsInsDB, cKizuKenNO, ii, (jj+1), nDspMode, pKizuInf ) ) {
				return false;
			}

			// 疵実績伝文 (全部)のループ
			for( kk = 0; kk < MAX_FF_MAE_DEF_CNT; kk++ ) {
				pKizuData = &pFfMae->def[kk];

				// 疵検出工程が一致
				if( 0 == memcmp( pKizuInf->Kizukbn, pKizuData->Kizukbn, sizeof(pKizuInf->Kizukbn) ) ) {
					// 20221021 2TCM実機化に伴う下工程改造 小島 --->>>
					if (DIV_PROC_KIND_INDEX_TCM_ANA == jj && MAX_FF_MAE_DEF_CNT - MAX_FF_MAE_N2TCM_KIZU_CNT <= kk) { continue; }		// N2TCM穴とN2TCMの区別を入っている場所で認識
					if (DIV_PROC_KIND_INDEX_TCM_KIZU == jj && MAX_FF_MAE_DEF_CNT - MAX_FF_MAE_N2TCM_KIZU_CNT > kk) { continue; }
					// 20221021 2TCM実機化に伴う下工程改造 小島 ---<<<

					//// 前工程FF疵情報テーブル登録
					if( ! InsertMaeFfInf( clsInsDB, cKizuKenNO, nKizuNo, (jj+1), ii, nDspMode, pKizuInf, pKizuData ) ) {
						return false;
					}

					// 疵No カウントアップ
					nKizuNo += 1;

				}
			}
		}
	}

	return true;
}

//------------------------------------------
// 前工程ライン別コイル情報登録
// OdbcBase* clsInsDB	DB登録インスタンス
// char cKizuKenNO		疵検管理No
// int nOuDiv			表裏区分
// int nProcKind		工程種別(1～7)
// int nDspMode			前工程表示モード
// FF_MAE_TYPE_INF* pKizuInf	
// 戻り値　ture:正常 false:異常
//------------------------------------------
bool PreProcManager::InsertMaeCoilLine( OdbcBase* clsInsDB, char* cKizuKenNO, int nOuDiv, int nProcKind,
									    int nDspMode, FF_MAE_TYPE_INF* pKizuInf )
{
	CString sql;					// SQL文
	CString cDbName;
	cDbName = DB_MAE_COIL_LINE;
	char cId[SIZE_SERIAL_FF_NO+1];

	////// 前工程疵情報
	// ヘッダー部
	sql = "INSERT INTO ";
	sql += cDbName;
	sql += " VALUES(";	

	// データ部
	CreateSQL::sqlColumnInsert( &sql, cKizuKenNO, SIZE_KIZUKEN_NO );								// 管理No
	CreateSQL::sqlColumnInsert( &sql, nProcKind );													// 工程種別
	CreateSQL::sqlColumnInsert( &sql, nOuDiv );														// 表裏区分
	// 処理ステータス
	if ( 1 == nDspMode ) {								// 前工程φω情報取得＆疵画像表示
		CreateSQL::sqlColumnInsert( &sql, -1 );			// -1：未取得
	} else if ( 2 == nDspMode ) {						// ＦＦ情報のみ表示
		CreateSQL::sqlColumnInsert( &sql,  -9 );		//  -9 : 取得取得対象外の前工程
	} else {											// 表示対象外
		CreateSQL::sqlColumnInsert( &sql, 0 );			// 0：該当する前工程φω情報を取得済み
	}
	CreateSQL::sqlColumnInsert( &sql, pKizuInf->Kizukbn, sizeof(pKizuInf->Kizukbn) );				// 疵検出工程
	memset( cId, 0x00, sizeof(cId) );
	memcpy( cId, pKizuInf->Id, sizeof(pKizuInf->Id) );
	KizuFunction::Trim( cId );
	CreateSQL::sqlColumnInsert( &sql, cId, sizeof(pKizuInf->Id) );															// 前工程管理No
	CreateSQL::sqlColumnInsert( &sql, KizuFunction::AtoIex( pKizuInf->Mode, sizeof(pKizuInf->Mode) ) );						// 検査装置処理モード
	CreateSQL::sqlColumnInsert( &sql, KizuFunction::AtoIex( pKizuInf->DataDefault, sizeof(pKizuInf->DataDefault) ) );		// 疵設定個数
	CreateSQL::sqlColumnInsert( &sql, KizuFunction::AtoIex( pKizuInf->OverStatus, sizeof(pKizuInf->OverStatus) ) );			// 疵個数OV
	CreateSQL::sqlColumnTrim( &sql );					// 最後のカンマを取り除く
	sql += ")";

	// 実行
	if( ! clsInsDB->ExecuteDirect( sql ) ) {
		LOG(em_NON), "%s", sql );
		LOG(em_ERR), "[%s] 前工程ライン別コイル情報登録エラー [%s]", my_sThreadName, cDbName );
		syslog(SYSNO_DB_EXECUTEDIRECT_ERR, "[%s]", cDbName);
		return false;
	}

	return true;
}

//------------------------------------------
// 前工程FF疵情報テーブル登録
// OdbcBase* clsInsDB	DB登録インスタンス
// char* cKizuKenNO		疵検管理No
// int nKizuNo			疵No
// int nProcKind		工程種別(1～7)
// int nOuDiv			表裏区分
// int nDspMode			前工程表示モード
// FF_MAE_TYPE_INF* pKizuInf	疵検出情報
// FF_MAE_TYPE_DATA* pKizuData	疵情報
// 戻り値　ture:正常 false:異常
//------------------------------------------
bool PreProcManager::InsertMaeFfInf( OdbcBase* clsInsDB, char* cKizuKenNO, int nKizuNo, int nProcKind, int nOuDiv,
	                                 int nDspMode, FF_MAE_TYPE_INF* pKizuInf, FF_MAE_TYPE_DATA* pKizuData )
{
	int nTypeEdasysID;			// 疵種EdasysID
	int nGradeEdasysID;			// グレードEdasysID
	CString sql;				// SQL文
	CString cDbName;
	cDbName = DB_MAE_FF_INF;
	char cKind[2];				// 疵種
	char cGrade[2];				// 疵グレード
	char cMaeNo[5];
	int nDefectPos;				// FF疵長さ方向位置
	int nWidPos;				// FF疵巾方向位置
	int nLen;					// FF疵長さ

	memset( cKind, 0x00, sizeof(cKind) );
	memcpy( cKind, pKizuData->Kind, sizeof(cKind) );
	memset( cGrade, 0x00, sizeof(cGrade) );
	memcpy( cGrade, pKizuData->Grade, sizeof(cGrade) );
	memset( cMaeNo, 0x00, sizeof(cMaeNo) );
	memcpy( cMaeNo, pKizuData->No, sizeof(pKizuData->No) );

	////// 前工程疵情報
	// ヘッダー部
	sql = "INSERT INTO ";
	sql += cDbName;
	sql += " VALUES(";	

	// データ部
	CreateSQL::sqlColumnInsert( &sql, cKizuKenNO, SIZE_KIZUKEN_NO );										// 管理No
	CreateSQL::sqlColumnInsert( &sql, nKizuNo );															// 疵No
	CreateSQL::sqlColumnInsert( &sql, nProcKind );															// 工程種別
	CreateSQL::sqlColumnInsert( &sql, nOuDiv );																// 表裏区分
	CreateSQL::sqlColumnInsert( &sql, 0 );																	// 事前編集フラグ
	CreateSQL::sqlColumnInsert( &sql, pKizuInf->Id, sizeof(pKizuInf->Id) );									// 前工程疵検管理No
	// 20221213 2TCM実機化に伴う下工程改造(不具合対応) 溝口 --->>>
	//CreateSQL::sqlColumnInsert( &sql, strtol(cMaeNo, NULL, 16));											// 前工程疵No
	if (DIV_PROC_KIND_INDEX_TCM_ANA  == nProcKind -1 ) {
		CreateSQL::sqlColumnInsert(&sql, strtol(cMaeNo, NULL, 10));											// 前工程疵No
	}
	else {
		CreateSQL::sqlColumnInsert( &sql, strtol(cMaeNo, NULL, 16));										// 前工程疵No
	}
	// 20221213 2TCM実機化に伴う下工程改造(不具合対応) 溝口 --->>>
	CreateSQL::sqlColumnInsert( &sql, pKizuData->Kind, sizeof(pKizuData->Kind) );							// FF疵種
	CreateSQL::sqlColumnInsert( &sql, pKizuData->Grade, sizeof(pKizuData->Grade) );							// FFグレード
	CreateSQL::sqlColumnInsert( &sql, pKizuInf->Kizukbn, sizeof(pKizuInf->Kizukbn) );						// FF疵検出工程
	CreateSQL::sqlColumnInsert( &sql, KizuFunction::AtoIex( pKizuData->kbn, sizeof(pKizuData->kbn) ) );		// FF疵区分

	// FF疵長さ方向位置
	nDefectPos = KizuFunction::AtoIex( pKizuData->DefectPos, sizeof(pKizuData->DefectPos) );
	nDefectPos *= 100;		// 単位 [0.1m]から[mm]へ変換
	CreateSQL::sqlColumnInsert( &sql, nDefectPos );
	// FF疵巾方向位置
	nWidPos = KizuFunction::AtoIex( pKizuData->WidPos, sizeof(pKizuData->WidPos) );
	CreateSQL::sqlColumnInsert( &sql, nWidPos );
	// FF疵長さ
	nLen = KizuFunction::AtoIex( pKizuData->Len, sizeof(pKizuData->Len) );
	nLen *= 100;			// 単位 [0.1m]から[mm]へ変換
	CreateSQL::sqlColumnInsert( &sql, nLen );

	// 判定疵種、判定グレード取得
	KizuFunction::SpaceToNull( cKind, sizeof(cKind) );
	KizuFunction::SpaceToNull( cGrade, sizeof(cGrade) );
	nTypeEdasysID = mcls_pParam->GetTyppeEdasysID( pKizuData->Kizukbn, cKind );
	if( 0 == nTypeEdasysID && 0 < nDspMode ) {
		// システム上で表示対象でかつ疵種未登録
		LOG(em_INF), "[%s] 前工程疵種マスター未登録 [工程コード=%.2s , 疵種=%.2s]", my_sThreadName, pKizuData->Kizukbn , cKind );
		//syslog(374, "[工程コード=%.2s, 疵種=%.2s]", pKizuData->Kizukbn , cKind );
	}
	nGradeEdasysID = mcls_pParam->GetGradeEdasysID( pKizuData->Kizukbn, cGrade );
	if( 0 == nGradeEdasysID && 0 < nDspMode ) {
		// システム上で表示対象でかつグレード未登録
		LOG(em_INF), "[%s] 前工程グレードマスター未登録 [工程コード=%.2s, 疵グレード=%.2s]", my_sThreadName, pKizuData->Kizukbn , cGrade );
		//syslog(375, "[工程コード=%.2s, 疵グレード=%.2s]", pKizuData->Kizukbn , cGrade );
	}
	CreateSQL::sqlColumnInsert( &sql, nTypeEdasysID );														// 判定疵種
	CreateSQL::sqlColumnInsert( &sql, nGradeEdasysID );														// 判定グレード
	CreateSQL::sqlColumnInsert( &sql, nDefectPos );															// 最終長手位置(登録時はFF疵長さ方向位置と同じ)
	CreateSQL::sqlColumnTrim( &sql );					// 最後のカンマを取り除く
	sql += ")";

	// 実行
	if( ! clsInsDB->ExecuteDirect( sql ) ) {
		LOG(em_NON), "%s", sql );
		LOG(em_ERR), "[%s] 疵情報登録エラー [%s]", my_sThreadName, cDbName );
		syslog(SYSNO_DB_EXECUTEDIRECT_ERR, "[%s]", cDbName);
		return false;
	}

	return true;
}

//------------------------------------------
// 前工程コイル情報更新
// char* cKizukenNo 疵検管理NO
// char* cId        ID
// FF_MAE_JISSEKI* typMaeJis
// 戻り値　ture:正常 false:異常
//-----------------------------------------
bool PreProcManager::UpdateCoilInf( OdbcBase* clsInsDB, char* cKizukenNo, char* cId, FF_MAE_INF* typData )
{
	CString sql;					// SQL文
	CString wk;
	CString cDbName;
	cDbName = DB_MAE_COIL_INF;
	FF_MAE_JISSEKI* pFfMaeO;		// 変数簡略化用
	int nMukesaki1i;				// 板巾_向先1
	int nMukesaki1KL;				// 剪断長_向先1

	// 変数の簡略化
	pFfMaeO = &typData->torb[0].typFFData;

	////// 前工程コイル情報
	//// SQL文作成
	sql = "UPDATE ";
	sql += cDbName;
	sql += " SET ";

	// データ部
	// 表裏共通はずなので表のデータをセット
	CreateSQL::sqlColumnUpdate( &sql, "ID", cId, SIZE_SERIAL_FF_NO );																		// ID
	CreateSQL::sqlColumnUpdate( &sql, "FF前工程疵情報受信フラグ", 0 );																		// FF前工程疵情報受信フラグ
	CreateSQL::sqlColumnUpdate( &sql, "冷延コイル番号", pFfMaeO->Hd.ReiCoilNo, sizeof(pFfMaeO->Hd.ReiCoilNo) );								// 冷延コイル番号
	CreateSQL::sqlColumnUpdate( &sql, "冷延コイル重量", KizuFunction::AtoIex( pFfMaeO->Hd.ReiCoilG, sizeof(pFfMaeO->Hd.ReiCoilG) ) );		// 冷延コイル重量
	CreateSQL::sqlColumnUpdate( &sql, "冷延コイル長さ", KizuFunction::AtoIex( pFfMaeO->Hd.ReiCoilL, sizeof(pFfMaeO->Hd.ReiCoilL) ) );		// 冷延コイル長さ
	CreateSQL::sqlColumnUpdate( &sql, "冷延厚", KizuFunction::AtoIex( pFfMaeO->Hd.ReiCoilAtu, sizeof(pFfMaeO->Hd.ReiCoilAtu) ) );			// 冷延厚
	CreateSQL::sqlColumnUpdate( &sql, "板厚公差MIN", KizuFunction::AtoIex( pFfMaeO->Hd.ItaatuMin, sizeof(pFfMaeO->Hd.ItaatuMin) ) );		// 板厚公差MIN
	CreateSQL::sqlColumnUpdate( &sql, "板厚公差MAX", KizuFunction::AtoIex( pFfMaeO->Hd.ItaatuMax, sizeof(pFfMaeO->Hd.ItaatuMax) ) );		// 板厚公差MAX
	nMukesaki1i = KizuFunction::AtoIex( pFfMaeO->Hd.Mukesaki1i, sizeof(pFfMaeO->Hd.Mukesaki1i) );
	nMukesaki1i /= 10;				// 単位 [0.1mm]から[mm]へ変換
	CreateSQL::sqlColumnUpdate( &sql, "板巾_向先1", nMukesaki1i );																			// 板巾_向先1
	CreateSQL::sqlColumnUpdate( &sql, "検査基準", pFfMaeO->Hd.Kensa, sizeof(pFfMaeO->Hd.Kensa) );											// 検査基準
	CreateSQL::sqlColumnUpdate( &sql, "捲き方向", KizuFunction::AtoIex( pFfMaeO->Hd.Maki, sizeof(pFfMaeO->Hd.Maki) ) );						// 捲き方向
	CreateSQL::sqlColumnUpdate( &sql, "材料巾", KizuFunction::AtoIex( pFfMaeO->Hd.Zairyou, sizeof(pFfMaeO->Hd.Zairyou) ) );					// 材料巾
	CreateSQL::sqlColumnUpdate( &sql, "板幅公差MIN_向先1", KizuFunction::AtoIex( pFfMaeO->Hd.Mukesaki1kMin, sizeof(pFfMaeO->Hd.Mukesaki1kMin) ) );	// 板幅公差MIN_向先1
	CreateSQL::sqlColumnUpdate( &sql, "板幅公差MAX_向先1", KizuFunction::AtoIex( pFfMaeO->Hd.Mukesaki1kMax, sizeof(pFfMaeO->Hd.Mukesaki1kMax) ) );	// 板幅公差MAX_向先1
	CreateSQL::sqlColumnUpdate( &sql, "長さ_ロット枚数_向先1", KizuFunction::AtoIex( pFfMaeO->Hd.Mukesaki1L, sizeof(pFfMaeO->Hd.Mukesaki1L) ) );	// 長さ_ロット枚数_向先1
	CreateSQL::sqlColumnUpdate( &sql, "内径_向先1", KizuFunction::AtoIex( pFfMaeO->Hd.Mukesaki1N, sizeof(pFfMaeO->Hd.Mukesaki1N) ) );				// 内径_向先1
	CreateSQL::sqlColumnUpdate( &sql, "板厚", KizuFunction::AtoIex( pFfMaeO->Hd.Itaatu, sizeof(pFfMaeO->Hd.Itaatu) ) );						// 板厚
	CreateSQL::sqlColumnUpdate( &sql, "入側内径", KizuFunction::AtoIex( pFfMaeO->Hd.Irigawa, sizeof(pFfMaeO->Hd.Irigawa) ) );				// 入側内径
	nMukesaki1KL = KizuFunction::AtoIex( pFfMaeO->Hd.Mukesaki1KL, sizeof(pFfMaeO->Hd.Mukesaki1KL) );
	nMukesaki1KL /= 1000;			// 単位 [0.1mm]から[0.1m]へ変換
	CreateSQL::sqlColumnUpdate( &sql, "剪断長_向先1", nMukesaki1KL );																		// 剪断長_向先1
	CreateSQL::sqlColumnUpdate( &sql, "前工程コード", pFfMaeO->Hd.Makoutei, sizeof(pFfMaeO->Hd.Makoutei) );									// 前工程コード
	CreateSQL::sqlColumnUpdate( &sql, "仮想RCL管理No", pFfMaeO->VRcl.KizukenNo, sizeof(pFfMaeO->VRcl.KizukenNo) );							// 仮想RCL疵検管理No
	CreateSQL::sqlColumnUpdate( &sql, "母体となる冷延コイルNo", pFfMaeO->FromTo.CoilNo, sizeof(pFfMaeO->FromTo.CoilNo) );					// 母体となるHOTコイルNo
	CreateSQL::sqlColumnUpdate( &sql, "採取開始位置長さ", KizuFunction::AtoIex( pFfMaeO->FromTo.From, sizeof(pFfMaeO->FromTo.From) ) );		// 採取開始位置長さ
	CreateSQL::sqlColumnUpdate( &sql, "採取終了位置長さ", KizuFunction::AtoIex( pFfMaeO->FromTo.To, sizeof(pFfMaeO->FromTo.To) ) );			// 採取終了位置長さ
	CreateSQL::sqlColumnUpdate( &sql, "採取子番", KizuFunction::AtoIex( pFfMaeO->FromTo.Koban, sizeof(pFfMaeO->FromTo.Koban) ) );			// 採取子番
	CreateSQL::sqlColumnTrim( &sql );										// 最後のカンマを取り除く

	wk.Format( " WHERE 管理No = '%s'", cKizukenNo );
	sql += wk;
	
	// 実行
	if( ! clsInsDB->ExecuteDirect( sql ) ) {
		LOG(em_NON), "%s", my_sThreadName, sql );
		LOG(em_ERR), "[%s] 前工程コイル情報更新エラー [%s]", my_sThreadName, cDbName );
		syslog(SYSNO_DB_EXECUTEDIRECT_ERR, "[%s]", cDbName);
		return false;
	}

	return true;
}

//------------------------------------------
// 前工程コイル情報更新(入側フロント端板長さ)
// OdbcBase* clspDB		DBインスタンス
// char* cCoilKey		キー情報
// int	nFrontLen		入側フロント端板長さ
//------------------------------------------
bool PreProcManager::UpdateMaeCoilInf( OdbcBase* clspDB, char* cCoilKey, int nFrontLen )
{
	CString sql;					// SQL文
	CString cDbName;
	cDbName = DB_MAE_COIL_INF;
	CString wk;
	CTime t = CTime::GetCurrentTime() - CTimeSpan(1, 0, 0, 0);		// 1日前
	CString strTime = t.Format("'%Y/%m/%d %H:%M:%S'");
	char cKeyinf[SIZE_COIL_KEY + 1];

	// コイル切替タイミングで、仮管理Noから管理Noの振りなおしをするため、
	// コイル情報受信タイミングは、キー情報から対象コイルを特定する
	memset(cKeyinf, 0x00, sizeof(cKeyinf));
	memcpy(cKeyinf, cCoilKey, SIZE_COIL_KEY);

	////// 前工程コイル情報
	//// SQL文作成
	sql = "UPDATE ";
	sql += cDbName;
	sql += " SET ";

	// データ部
	CreateSQL::sqlColumnUpdate( &sql, "入側フロント端板長さ", nFrontLen );
	CreateSQL::sqlColumnTrim( &sql );
	wk.Format( " WHERE キー情報 = '%s' AND 登録日付 > %s", cKeyinf, strTime);
	sql += wk;
	
	// 実行
	if( ! clspDB->ExecuteDirect( sql ) ) {
		LOG(em_NON), "%s", sql );
		LOG(em_ERR), "[%s] 前工程コイル情報更新エラー [%s]", my_sThreadName, cDbName );
		syslog(SYSNO_DB_EXECUTEDIRECT_ERR, "[%s]", cDbName);
		return false;
	}

	return true;
}

