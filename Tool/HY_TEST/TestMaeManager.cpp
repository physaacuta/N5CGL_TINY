#include "StdAfx.h"
#include "TestMaeManager.h"

// メモリーリーク検出 ----->>>
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif
//------------------------------------------
// コンストラクタ
//------------------------------------------
TestMaeManager::TestMaeManager(void) :
ThreadManager("MaeTest")
{
}

TestMaeManager::~TestMaeManager(void)
{
}

////////////////////////////////////////////////////////////////////////////////
// スレッド
////////////////////////////////////////////////////////////////////////////////
//------------------------------------------
// スレッド開始前処理 (スレッド実行直後(シグナルセット前)にコールバック)
//------------------------------------------
int TestMaeManager::ThreadFirst()
{
//// 事前準備
		//// イベント設定
	HANDLE hArray[] = { gque_Deli.g_evSem,
						};
	SetEventNum(sizeof(hArray)/sizeof(HANDLE), hArray);

	return 0;
}

//------------------------------------------
// スレッド終了前処理 (終了シグナル後にコールバック)
//------------------------------------------
int TestMaeManager::ThreadLast()
{
	DELIVERY_KEY *pDeli;				// キューデータ

	// 未処理のキューを破棄
	while(true) {
		pDeli = gque_Deli.GetFromHead(0);
		if( pDeli == NULL ) break;
		
		// 削除
		LOG(em_WAR), "[%s] 未処理キュー削除 [%s]", my_sThreadName, pDeli->cKizukenNo);
		delete pDeli;
	}

	return 0;
}


//------------------------------------------
// スレッドイベント発生
// int nEventNo イベントNo (0オリジン)
//------------------------------------------
void TestMaeManager::ThreadEvent(int nEventNo)
{
	DELIVERY_KEY*	pDeli;			// 受け渡し依頼データ

	////// シグナルの条件
	enum {	EV_QUE = 0,						// キュー依頼
	};

	////// シグナル条件分け
	switch (nEventNo) {

//-----------------------------------------------------------------------------------------------
	case EV_QUE:
		pDeli = gque_Deli.GetFromHead(); 
		Exec( pDeli );
		SAFE_DELETE(pDeli);
		break;
		
//-----------------------------------------------------------------------------------------------
	default :
		// ありえない！！
		LOG(em_ERR), "[%s] ThreadEvent EventNo NG=%d", my_sThreadName, nEventNo );
		return;
	}
}

// 前工程ライン別コイル情報登録
bool TestMaeManager::SetMaeCoilLine(char const* cKizukenNo,int ou, int nKouteiID)
{
	CString sql;				// SQL文
	CString cDbName;
	cDbName = DB_MAE_COIL_LINE ;

	//// ODBC接続クラス インスタンス生成
	OdbcBase clsDB;
	if( ! clsDB.Connect(TASKINI_NAME)) {
		// データベース接続エラー
		LOG(em_ERR), "[%s] DB接続エラー", my_sThreadName);
		return false;
	}

	////// 前工程疵情報
	// ヘッダー部
	sql = "INSERT INTO ";
	sql += cDbName;
	sql += " VALUES(";	

	// データ部
	CreateSQL::sqlColumnInsert( &sql, cKizukenNo, SIZE_KIZUKEN_NO );	// 疵検管理No
	CreateSQL::sqlColumnInsert( &sql, nKouteiID);						// 工程種別
	CreateSQL::sqlColumnInsert( &sql, ou );								// 表裏区分
	CreateSQL::sqlColumnInsert( &sql, 0 );								// 処理ステータス
	CreateSQL::sqlColumnInsert( &sql, "22", 2 );						// 疵検出工程
	CreateSQL::sqlColumnInsert( &sql,  cKizukenNo, SIZE_KIZUKEN_NO );	// 前工程疵検管理No
	CreateSQL::sqlColumnInsert( &sql, 0 );								// 疵検処理モード
	CreateSQL::sqlColumnInsert( &sql, 1 );								// 疵設定個数
	CreateSQL::sqlColumnInsert( &sql, 1 );								// 疵個数OV
	CreateSQL::sqlColumnTrim( &sql );									// 最後のカンマを取り除く
	sql += ")";

	// 実行
	if( ! clsDB.ExecuteDirect( sql ) ) {
		LOG(em_ERR), "[%s] 前工程ライン別コイル情報登録エラー [%s]", my_sThreadName, cDbName );
		LOG(em_ERR), "[%s] SQL [%s]", my_sThreadName, sql );
		syslog(SYSNO_DB_EXECUTEDIRECT_ERR, "[%s]", cDbName);
		return false;
	} else {
		LOG(em_MSG), "[%s] 前工程ライン別コイル情報登録 疵検管理No[%s]", my_sThreadName, cKizukenNo );
		return true;
	}


}

// 前工程FF疵情報テーブル登録
bool TestMaeManager::SetMaeFFInf(char const* cKizukenNo,int nKizuNo, int ou, double dNobiSPM, int nKouteiID)
{

	CString sql;				// SQL文
	CString cDbName;
	cDbName = DB_MAE_FF_INF;

	//// ODBC接続クラス インスタンス生成
	OdbcBase clsDB;
	if( ! clsDB.Connect(TASKINI_NAME)) {
		// データベース接続エラー
		LOG(em_ERR), "[%s] DB接続エラー", my_sThreadName);
		return false;
	}

	////// 前工程疵情報
	// ヘッダー部
	sql = "INSERT INTO ";
	sql += cDbName;
	sql += " VALUES(";	

	// FF疵グレードを決定する "1 "～"7 "
	char cFFgrade[2];
	cFFgrade[0] = '1' + rand() % 7;
	cFFgrade[1] = ' ';

	// FF疵種を決定する
	//char* FF_Tname[] = { "B1","B4","B0","71","61","10","77","B9","C0" };
	//int FF_Tid[] = { 2,17,20,6,7,5,13,19,18 };
	char* FF_Tname[] = { "2M","2N" };
	int FF_Tid[] = { 1,2 };
	int nTidx = rand()%(sizeof(FF_Tid)/sizeof(int));

	int nPitch; // [mm]
	nPitch = 5000;

	// データ部
	CreateSQL::sqlColumnInsert( &sql, cKizukenNo, SIZE_KIZUKEN_NO );										// 疵検管理No
	CreateSQL::sqlColumnInsert( &sql, nKizuNo + 1);															// 疵No
	CreateSQL::sqlColumnInsert( &sql, nKouteiID);																	// 工程種別
	CreateSQL::sqlColumnInsert( &sql, ou );																	// 表裏区分
	CreateSQL::sqlColumnInsert( &sql, 0 );																	// 事前編集フラグ
	CreateSQL::sqlColumnInsert( &sql, cKizukenNo, SIZE_KIZUKEN_NO );										// 前工程疵検管理No
	CreateSQL::sqlColumnInsert( &sql, nKizuNo + 1);															// 前工程疵No
	if (6 == nKouteiID) {
		CreateSQL::sqlColumnInsert(&sql,"TT", 2);												// FF疵種
	}
	else {
		CreateSQL::sqlColumnInsert(&sql, FF_Tname[nTidx], 2);												// FF疵種
	}
	
	CreateSQL::sqlColumnInsert( &sql, cFFgrade , 2 );														// FFグレード
	CreateSQL::sqlColumnInsert( &sql, "22", 2);																// FF疵検出工程
	CreateSQL::sqlColumnInsert( &sql, 1 );																	// FF疵区分
	CreateSQL::sqlColumnInsert( &sql, 30000 + (2 * (nKizuNo+1) * nPitch) );											// FF疵長さ方向位置
	CreateSQL::sqlColumnInsert( &sql, 150 );																// FF疵巾方向位置
	CreateSQL::sqlColumnInsert( &sql, 30 );																	// FF疵長さ
	CreateSQL::sqlColumnInsert( &sql, FF_Tid[nTidx] );														// 判定疵種
	CreateSQL::sqlColumnInsert( &sql, cFFgrade[0] - '0' );													// 判定グレード
	if (0!=dNobiSPM) {
		CreateSQL::sqlColumnInsert(&sql, (3000 + (2 * (nKizuNo+1) * nPitch))* dNobiSPM);								// 最終長手位置
	}
	else {
		CreateSQL::sqlColumnInsert(&sql, 3000 + (2 * (nKizuNo+1) * nPitch));											// 最終長手位置
	}

	CreateSQL::sqlColumnTrim( &sql );					// 最後のカンマを取り除く
	sql += ")";

	// 実行
	if( ! clsDB.ExecuteDirect( sql ) ) {
		LOG(em_ERR), "[%s] FF疵情報登録エラー [%s]", my_sThreadName, cDbName );
		LOG(em_ERR), "[%s] SQL [%s]", my_sThreadName, sql );
		syslog(SYSNO_DB_EXECUTEDIRECT_ERR, "[%s]", cDbName);
		return false;
	} else {
		LOG(em_MSG), "[%s] FF疵情報登録 [疵検査管理No = %s] 疵No=%d",
			my_sThreadName, cKizukenNo, nKizuNo+1 );
		return true;
	}
	

}

//------------------------------------------
// 前工程疵情報登録
//------------------------------------------
bool TestMaeManager::SetMaeDefectInf(MAE_DEFECT_IMAGE_INF* typData)
{
	//// ODBC接続クラス インスタンス生成
	OdbcBase clsDB;
	if( ! clsDB.Connect(TASKINI_NAME)) {
		// データベース接続エラー
		LOG(em_ERR), "[%s] DB接続エラー", my_sThreadName);
		return false;
	}
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
	CreateSQL::sqlColumnTrim( &sql );											// 最後のカンマを取り除く
	sql += ")";

	// 実行
	if( ! clsDB.ExecuteDirect( sql ) ) {
		LOG(em_ERR), "[%s] 疵情報登録エラー [%s]", my_sThreadName, cDbName );
		LOG(em_ERR), "[%s] SQL [%s]", my_sThreadName, sql );
		syslog(SYSNO_DB_EXECUTEDIRECT_ERR, "[%s]", cDbName);
		return false;
	}
	
	return true;
}

//------------------------------------------
// 前疵画像をDBに登録
//------------------------------------------
bool TestMaeManager::InsertDefectImg(MAE_DEFECT_IMAGE_INF* typData)
{
	//// ODBC接続クラス インスタンス生成
	OdbcBase clsDB;
	if( ! clsDB.Connect(TASKINI_NAME)) {
		// データベース接続エラー
		LOG(em_ERR), "[%s] DB接続エラー", my_sThreadName);
		return false;
	}
	CString sql;					// SQL文
	CString cDbName;
	cDbName = DB_MAE_DEFECT_IMG;


	////// 前工程疵情報
	// ヘッダー部
	sql = "INSERT INTO ";
	sql += cDbName;
	sql += " VALUES(";	

	// データ部
	CreateSQL::sqlColumnInsert( &sql, typData->cKizuKenNo, SIZE_KIZUKEN_NO );	// 疵検管理No 
	CreateSQL::sqlColumnInsert( &sql, typData->nKizuNo );						// 疵No
	for( int ii = 0; ii< MAX_MAE_IMAGE; ii++ ) {
		if( 0 < typData->nSize[ii] ) {
			sql += " ?,";
		} else {
			sql += " NULL,";
		}
	}
	CreateSQL::sqlColumnTrim( &sql );									// 最後のカンマを取り除く
	sql += ")";

	// 実行
	if( ! clsDB.SqlBinalyWrite( sql, typData->nCnt, typData->nSize, (LPVOID*)typData->pImg) ) {
		LOG(em_ERR), "[%s] 画像登録失敗", my_sThreadName );
		LOG(em_ERR), "[%s] SQL [%s]", my_sThreadName, sql );
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
// 前疵情報をDBに登録
//------------------------------------------
bool TestMaeManager::CreateDefectInf(char const* cKizukenNo, int nKizuNo, int ou, int nKouteiID )
{
	MAE_DEFECT_IMAGE_INF typData;
	int nGazoCnt = 1;
	int nPitch = 5000;

	memcpy(typData.cKizuKenNo , cKizukenNo , SIZE_KIZUKEN_NO);
	memcpy(typData.cPreKizuKenNo , cKizukenNo , SIZE_KIZUKEN_NO);
	typData.nKizuNo = nKizuNo + 1;
	typData.nKouteiSyubetu = nKouteiID;
	typData.nOuDiv = ou ;
	typData.typDefectData.cycle_div = 0;
	typData.typDefectData.y = (float)(3000 + (5 * (nKizuNo+1) * nPitch));
	typData.typDefectData.dsws = 1;
	typData.typDefectData.ds = 300;
	typData.typDefectData.ws = 200;
	typData.typDefectData.speed = 300;
	typData.typDefectData.pitch = 0;
	typData.typDefectData.roolkei = 0;
	typData.typDefectData.kizu_wid = 30;
	typData.typDefectData.kizu_len = 150;
	typData.typDefectData.w_res = 1.0;
	typData.typDefectData.h_res = 1.0;
	typData.typDefectData.box_x_min = 30;
	typData.typDefectData.box_x_max = 130;
	typData.typDefectData.box_y_min = 50;
	typData.typDefectData.box_y_max = 150;
	typData.typDefectData.center_x = 80;
	typData.typDefectData.center_y = 100;
	typData.typDefectData.w_size = 256;
	typData.typDefectData.h_size = 256;
	typData.nCnt = nGazoCnt;

	CString cfileName[MAX_CAMANGLE];		// ファイルパス
	CFile cfile[MAX_CAMANGLE];				// ファイルオブジェクト
	long nFileLen[MAX_CAMANGLE];			// ファイルサイズ

	nFileLen[0] = typData.nSize[0] = 0;		
	nFileLen[1] = typData.nSize[1] = 0;
	nFileLen[2] = typData.nSize[2] = 0;
	nFileLen[3] = typData.nSize[3] = 0;

	for (int jj = 0; jj< nGazoCnt; jj++) {
		if (6 == nKouteiID) {
			cfileName[jj] = ".\\TCM疵.jpg";
		}
		else {
			cfileName[jj] = ".\\TCM穴.jpg";
		}

		//cfileName[jj] = ".\\疵01_1.jpg";
		//cfileName[jj] = ".\\1.bmp";
		// 対象画像のファイルをオープンする
		if(cfile[jj].Open(cfileName[jj], 0, 0)) {
			nFileLen[jj] = cfile[jj].GetLength();				// 対象画像ファイルのサイズを取得する
			typData.nSize[jj] = nFileLen[jj];					// 画像ファイルのサイズをセットする
			typData.pImg[jj] = new BYTE[typData.nSize[jj]];		// 画像ファイル読み込み用バッファを確保する
			memset(typData.pImg[jj], 0x00, nFileLen[jj]);		//
			cfile[jj].Read(typData.pImg[jj], nFileLen[jj]);		// 画像ファイルを読み込む
			cfile[jj].Close();									// 画像ファイルをクローズする
		}
	}

	// MAE_DEFECT_IMGに画像登録
	InsertDefectImg(&typData);

	SetMaeDefectInf(&typData);

	// 開放
	for( int ii = 0; ii < 1; ii++ ) {
		SAFE_DELETE_ARRAY( typData.pImg[ii] )
	}

	return true;
}


//前工程コイル情報登録
// FF前工程疵情報受信フラグに0をセットする
bool TestMaeManager::SetMaeCoilInf(char const* cKizukenNo , char const* cKeyInf,char const* cMeiNo , char const* cCoilNo)
{
	CString sql;					// SQL文
	CString cDbName;
	cDbName = DB_MAE_COIL_INF;

	char cKey[SIZE_SERIAL_FF_NO];

	//// ODBC接続クラス インスタンス生成
	OdbcBase clsDB;
	if( ! clsDB.Connect(TASKINI_NAME)) {
		// データベース接続エラー
		LOG(em_ERR), "[%s] DB接続エラー", my_sThreadName);
		return false;
	}

	// ID作成
	KizuFunction::GetSerialNoFF(cKey);									

	////// コイル位置情報
	// ヘッダー部
	sql = "INSERT INTO ";
	sql += cDbName;
	sql += " VALUES(";	

	// データ部
	CreateSQL::sqlColumnInsert( &sql, cKizukenNo, SIZE_KIZUKEN_NO );				// 管理No
	CreateSQL::sqlColumnAddTime(&sql, true);										// 登録日付
	CreateSQL::sqlColumnInsert( &sql, cKeyInf, SIZE_COIL_KEY);						// キー情報
	CreateSQL::sqlColumnInsert( &sql, cKey, SIZE_SERIAL_FF_NO );					// ID
	CreateSQL::sqlColumnInsert( &sql, 0 );											// 仮管理Noフラグ 0:正式管理Noをセット
	CreateSQL::sqlColumnInsert( &sql, 0 );											// FF前工程疵情報受信フラグ
	CreateSQL::sqlColumnInsert( &sql, KOUTEI_CODE_2TCM, SIZE_KOUTEI_CODE );			// 工程コード
	CreateSQL::sqlColumnInsert( &sql, cMeiNo,  SIZE_MEI_NO );						// 命令No
	CreateSQL::sqlColumnInsert( &sql, cCoilNo, SIZE_COIL_NO );						// コイルNo
	CreateSQL::sqlColumnInsert( &sql, 0 );											// 入側フロント端板長さ
	CreateSQL::sqlColumnInsert( &sql, NULL, 0 );									// 冷延コイル番号
	CreateSQL::sqlColumnInsert( &sql, NULL, 0 );									// 冷延コイル重量
	CreateSQL::sqlColumnInsert( &sql, NULL, 0 );									// 冷延コイル長さ
	CreateSQL::sqlColumnInsert( &sql, NULL, 0 );									// 冷延厚
	CreateSQL::sqlColumnInsert( &sql, NULL, 0 );									// 板厚公差MIN
	CreateSQL::sqlColumnInsert( &sql, NULL, 0 );									// 板厚公差MAX
	CreateSQL::sqlColumnInsert( &sql, NULL, 0 );									// 板巾_向先1
	CreateSQL::sqlColumnInsert( &sql, NULL, 0 );									// 検査基準
	CreateSQL::sqlColumnInsert( &sql, NULL, 0 );									// 捲き方向
	CreateSQL::sqlColumnInsert( &sql, NULL, 0 );									// 材料巾
	CreateSQL::sqlColumnInsert( &sql, NULL, 0 );									// 板幅公差MIN_向先1
	CreateSQL::sqlColumnInsert( &sql, NULL, 0 );									// 板幅公差MAX_向先1
	CreateSQL::sqlColumnInsert( &sql, NULL, 0 );									// 長さ_ロット枚数_向先1
	CreateSQL::sqlColumnInsert( &sql, NULL, 0 );									// 内径_向先1
	CreateSQL::sqlColumnInsert( &sql, NULL, 0 );									// 板厚
	CreateSQL::sqlColumnInsert( &sql, NULL, 0 );									// 入側内径
	CreateSQL::sqlColumnInsert( &sql, NULL, 0 );									// 剪断長_向先1
	CreateSQL::sqlColumnInsert( &sql, NULL, 0 );									// 前工程コード
	CreateSQL::sqlColumnInsert( &sql, NULL, 0 );									// 仮想RCL管理No
	CreateSQL::sqlColumnInsert( &sql, NULL, 0 );									// 母体となる冷延コイルNo
	CreateSQL::sqlColumnInsert( &sql, NULL, 0 );									// 採取開始位置長さ
	CreateSQL::sqlColumnInsert( &sql, NULL, 0 );									// 採取終了位置長さ
	CreateSQL::sqlColumnInsert( &sql, NULL, 0 );									// 採取子番
	CreateSQL::sqlColumnTrim(&sql);													// 最後のカンマを取り除く
	sql += ")";


	if( ! clsDB.ExecuteDirect( sql ) ) {
		LOG(em_ERR), "[%s] 前工程コイル情報登録エラー [%s]", my_sThreadName, cDbName );
		LOG(em_ERR), "[%s] SQL [%s]", my_sThreadName, sql );
		return false;
	} else {
		LOG(em_MSG), "[%s] 前工程コイル情報登録 [疵検査管理No = %s] [キー情報 = %s]",
			my_sThreadName, cKizukenNo, cKeyInf );
		return true;
	}
}


// HY_DEFECTに前工程疵情報読込通知を送る
void TestMaeManager::Send_Hy_MaeRead(char const* cKizukenNo)
{
	COMMON_QUE  que;
	memset(&que, 0x00, sizeof(que));
	que.nLineNo = getlineid();

	// HY_DEFECT
	que.nEventNo = FACT_HY_DEFECT_03;
	memcpy( que.fc.data , cKizukenNo, SIZE_KIZUKEN_NO); 
	send_mail(HY_DEFECT,".", &que);

}

//イベント受信処理
void TestMaeManager::Exec( DELIVERY_KEY* pDeli )
{
	int nDefInfoCnt = 5;
	// 前コイル情報作成
	SetMaeCoilInf(pDeli->cKizukenNo, pDeli->cKeyInf, pDeli->cMeiNo, pDeli->cCoilNo);

	for(int ou = 0 ; ou < 1 ; ou++ ){
		// 前ライン情報作成
		SetMaeCoilLine(pDeli->cKizukenNo, ou, 4);

		for (int ii = 0; ii < nDefInfoCnt; ii++) {
			// 前FF情報作成
			SetMaeFFInf(pDeli->cKizukenNo, ii + ou * nDefInfoCnt, ou, pDeli->dNobiSPM, 4);
			// 前疵情報作成
			CreateDefectInf(pDeli->cKizukenNo, ii + ou*nDefInfoCnt, ou, 4);
		}
		// 前ライン情報作成
		SetMaeCoilLine(pDeli->cKizukenNo, ou, 6);

		for (int ii = 0; ii < nDefInfoCnt; ii++) {
			// 前FF情報作成
			SetMaeFFInf(pDeli->cKizukenNo, ii + ou * nDefInfoCnt + nDefInfoCnt, ou, pDeli->dNobiSPM, 6);
			// 前疵情報作成
			CreateDefectInf(pDeli->cKizukenNo, ii + ou*nDefInfoCnt + nDefInfoCnt, ou, 6);
		}
	}

	Send_Hy_MaeRead(pDeli->cKizukenNo);
}