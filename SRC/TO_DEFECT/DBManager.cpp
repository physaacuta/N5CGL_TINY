#include "StdAfx.h"
#include "DBManager.h"
#include "ToLinkswUtil.h"
#include <float.h>
#include <limits.h>
// メモリーリーク検出 ----->>>
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif
// <<<------------------------

#include "MainInstance.h"

#define DBMGR_INSERT_S(x)	CreateSQL::sqlColumnInsert(&sql, x, sizeof(x))		// 文字列用
#define DBMGR_INSERT_V(x)	CreateSQL::sqlColumnInsert(&sql, x)					// 文字列以外用
#define DBMGR_INSERT_N(x,y) CreateSQL::sqlColumnInsert(&sql, x, y)				// その他用

// 不正な値だとBCPが失敗するので、フィルタする
//　NaN -> 0
//  +∞ -> +1.79e308
//  -∞ -> -1.79e308
//  正常な値 -> そのまま.
#ifndef NOT_FILTER_NAN_INF
inline static double filter_nan_inf( double v )
{
	if( _isnan(v) ){	//Not A Number
		return 0;
	}
	if( !_finite(v) ){ //∞
		if( v<0 ) return -DBL_MAX;
		if( v>0 ) return +DBL_MAX;
	}
	return v;
}
#else
inline static double filter_nan_inf( double v )
{
	return v;
}
#endif

#ifndef NOT_FILTER_SHRT
inline static short filter_short( int nVal, int nMin, int nMax )
{
	if( nVal < nMin ) return nMin;
	if( nVal > nMax ) return nMax;
	return nVal;
}
#else
inline static short filter_short( int nVal, int nMin, int nMax )
{
	return nVal;
}
#endif

//------------------------------------------
// コンストラクタ
//------------------------------------------
DBManager::DBManager(void) :
DbBaseManager("DBMgr")
{

	// 機器状態戻し
	KizuFunction::SetStatus("BCP_ERR_ID",			true, false);	// 画像書込み失敗
	KizuFunction::SetStatus("DB_SAVE_MAX_1_ID",		true, false);	// 検査実績保存上限
	KizuFunction::SetStatus("DB_SAVE_MAX_2_ID",		true, false);	// 検査実績保存上限
	KizuFunction::SetStatus("DB_SAVE_SKIP_1_ID",	true, false);	// 検査実績保存スキップ
	KizuFunction::SetStatus("DB_SAVE_SKIP_2_ID",	true, false);	// 検査実績保存スキップ
	this->SetSaveSkipId(0, GetPrivateProfileInt(INI_STATUS, "DB_SAVE_SKIP_1_ID", 0, TASKINI_NAME) );
	this->SetSaveSkipId(1, GetPrivateProfileInt(INI_STATUS, "DB_SAVE_SKIP_2_ID", 0, TASKINI_NAME) );
}

//------------------------------------------
// デストラクタ
//------------------------------------------
DBManager::~DBManager(void)
{
}


// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 継承元からコールバック

//==========================================
// 関数初期化
//------------------------------------------
// bool		bMode	: BCP状態変更
//==========================================
void DBManager::OnAlloc(bool bMode)
{
	mcls_Bcps.SetLogMgr( mcls_pLog );
	mcls_Bcps.SetStatusMgr(mcls_pStatus);
	mcls_Bcps.Alloc();
	mcls_Bcps.Start();

	//// 先頭のインスタンスのみ DB削除管理に通知ありとする
	mcls_pDbs[0]->SetSendDbManager(false);
	this->Send_To_DbManage(true);			// 書込み優先を送信
}

//==========================================
// 関数開放
//==========================================
void DBManager::OnFree()
{
	mcls_Bcps.Stop();
	mcls_Bcps.Free();
}


//==========================================
// DB登録依頼
//------------------------------------------
// int		emNo	: ENUM_KUBUNの番号
// BYTE*	pData	: データ部 (継承元で開放処理を行うのでここでは、開放する必要なし)
//==========================================
void DBManager::OnQueDb(int emNo, BYTE* pData)
{
	//// 区分により処理分け
	switch(emNo) {
		//-----------------------------------------------------------
		case em_COILINF:												// コイル情報
			DbBaseManager::ChangeSQLInf(((DBDATA_COIL_INF*)pData)->cKizukenNo);		// バッファ切替
			CreateSQL_CoilInf( (const DBDATA_COIL_INF*)pData);
			break;

		//-----------------------------------------------------------
		case em_COILRESULT:												// コイル実績
			CreateSQL_CoilResult( (const DBDATA_COIL_RESULT*)pData );
			break;

		//-----------------------------------------------------------
		case em_COILRESULT_UPDATE:										// コイル実績アップデート
			CreateSQL_CoilResultUpdate( (const DBDATA_COIL_RESULT_UPDATE*)pData );
			break;

		//-----------------------------------------------------------
		case em_COILPOSITION:											// コイル位置
			CreateSQL_CoilPosition( (const DBDATA_COIL_POSITION*)pData);
			break;

		//-----------------------------------------------------------
		case em_COILTRAND:												// コイルトレンド
			CreateSQL_CoilTrand( (const DBDATA_COIL_TRAND*)pData );
			break;

		//-----------------------------------------------------------
		case em_COILCUTPOS:												// コイルカット位置
			CreateSQL_CoilCutPos( (const DBDATA_COIL_CUT_POS*)pData );
			break;

		//-----------------------------------------------------------
		case em_DEFECTINF:												// 疵情報
#ifndef NON_DB_SAVE
			if (!CreateSQL_DefectInf( (const DBDATA_DEFECT_INF*)pData ))
			{
				// 疵保存上限に達していた場合は画像をクリアする
				// ※以前はこのクリアがないため、画像領域分メモリリークしていた
				for (int ii = 0; ii<MAX_CAMANGLE; ii++) { SAFE_DELETE_ARRAY(((DBDATA_DEFECT_INF*)pData)->pImg[ii]); }
			}
#endif
			break;

		//-----------------------------------------------------------
		case em_EDGE_COIL_INF:											// エッジ検出実績テーブル
			CreateSQL_EdgeCoilInf( (const DBDATA_EDGE_COIL_INF*)pData );
			break;

		//-----------------------------------------------------------
		case em_EDGE_CHANGE_INF:										// エッジ検出モード変化管理テーブル
			CreateSQL_EdgeChangeInf( (const DBDATA_EDGE_CHANGE_INF*)pData );
			break;
			
		//-----------------------------------------------------------
		case em_CAMLINKSW:												// カメラリンクスイッチ設定
			CreateSQL_CamLinkSW( (const DBDATA_CAMERA_LINKSWITCH*)pData );
			break;

		//-----------------------------------------------------------
		case em_PULSE_RATE_UPDATE:										// シーケンサからパルスレート変更
			CreateSQL_PulseRateUpdate( (DBDATA_PULSE_RATE_UPDATE*)pData);
			break;

		//-----------------------------------------------------------
		case em_DB_ADD_SYNC:											// 登録状況問い合わせ
			Add_Synclock( (const DBDATA_ADD_SYNC*)pData );
			break;

// ------ MainConst用 ------------>>>
		case ENUM_DB_KUBUN::em_STATUS_LOG :								// ステータスログ
			CreateSQL_StatusLog( (const DBDATA_STATUS_LOG*)pData );
			break;
// <<<------------------------------

	}
}

//==========================================
// リセット依頼
//==========================================
void DBManager::OnReSetMaxCount()
{
	mcls_Bcps.ReSetMaxCount();
}

// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// SQL生成関連

//==========================================
// コイル情報
//------------------------------------------
// const DBDATA_COIL_INF* pWk : 生成情報	
//==========================================
void DBManager::CreateSQL_CoilInf(const DBDATA_COIL_INF* pWk)
{
	CString sql;

	//=================================
	////// コイル情報
	// ヘッダー部
	sql  = "INSERT INTO ";
	sql += DB_COIL_INF;
	sql += " VALUES(";

	// データ部
	//------------------------------------------------------------------>>>
	DBMGR_INSERT_S(pWk->cKizukenNo);				// 管理No
	CreateSQL::sqlColumnAddTime(&sql, true);		// 登録日付
	DBMGR_INSERT_S(pWk->cCoilKey);					// キー情報
	DBMGR_INSERT_S(pWk->cMeiNo);					// 命令No
	DBMGR_INSERT_S(pWk->cCoilNo);					// 生産No
	DBMGR_INSERT_V(pWk->nSendPoint);				// 送信位置
	DBMGR_INSERT_V(pWk->nLenIn);					// コイル長
	DBMGR_INSERT_V(pWk->nAtuZai);					// 材料板厚
	DBMGR_INSERT_V(pWk->nWidZai);					// 材料板幅
	DBMGR_INSERT_V(pWk->nAtuSei);					// 注文板厚
	DBMGR_INSERT_V(pWk->nWidSei);					// 注文板幅
	DBMGR_INSERT_V(pWk->nWidTrim);					// 現工程トリム幅
	DBMGR_INSERT_V(pWk->nNobiSPM);					// SPM伸び率
	DBMGR_INSERT_V(pWk->nNobiTLV);					// TLV伸び率
	DBMGR_INSERT_V(pWk->nCutF);						// 入側におけるフロント端板カットの実績長
	DBMGR_INSERT_V(pWk->nCutT);						// 入側におけるテール端板カットの実績長
	DBMGR_INSERT_V(pWk->nSPMRoolKei_Up);			// SPMワークロール径 (上)
	DBMGR_INSERT_V(pWk->nSPMRoolKei_Down);			// SPMワークロール径 (下)
	DBMGR_INSERT_N(NULL, 0);						// 予備_int_10
	DBMGR_INSERT_N(NULL, 0);						// 予備_int_9
	DBMGR_INSERT_N(NULL, 0);						// 予備_int_8
	DBMGR_INSERT_N(NULL, 0);						// 予備_int_7
	DBMGR_INSERT_N(NULL, 0);						// 予備_int_6
	DBMGR_INSERT_N(NULL, 0);						// 予備_int_5
	DBMGR_INSERT_N(NULL, 0);						// 予備_int_4
	DBMGR_INSERT_N(NULL, 0);						// 予備_int_3
	DBMGR_INSERT_N(NULL, 0);						// 予備_int_2
	DBMGR_INSERT_N(NULL, 0);						// 予備_int_1
	DBMGR_INSERT_S(pWk->cKenKijun_O);				// 検査基準 表
	DBMGR_INSERT_S(pWk->cKenKijun_U);				// 検査基準 裏
	DBMGR_INSERT_S(pWk->cKurometo_Kind);			// クロメート 実績種類
	DBMGR_INSERT_S(pWk->cKurometo_Umu);				// クロメート 実績有無
	DBMGR_INSERT_S(pWk->cHinCode);					// 品種
	DBMGR_INSERT_S(pWk->cMekkiKind);				// メッキ種類
	DBMGR_INSERT_S(pWk->cSyuZai);					// 主材質
	DBMGR_INSERT_S(pWk->cDifMaker);					// ディファレンシャルマーカー
	DBMGR_INSERT_S(pWk->cKenban);					// 原板区分
	DBMGR_INSERT_S(pWk->cMekkiCycle);				// メッキサイクル
	DBMGR_INSERT_S(pWk->cInLineSPM);				// レベラー インラインSPM
	DBMGR_INSERT_S(pWk->cKagakuToyu);				// 化学処理塗油
	DBMGR_INSERT_S(pWk->cAtukkaSPM);				// オフラインSPM圧下率
	DBMGR_INSERT_S(pWk->cMekkiKubun);				// メッキ区分
	DBMGR_INSERT_S(pWk->cOmoteSyori1);				// 表面処理1桁目
	DBMGR_INSERT_S(pWk->cOmoteSyori2);				// 表面処理2桁目
	DBMGR_INSERT_S(pWk->cToyu);						// 塗油
	DBMGR_INSERT_S(pWk->cKouteiYobi);				// 工程記号予備
	DBMGR_INSERT_S(pWk->cBaund);					// バウンダリ合わせ
	DBMGR_INSERT_S(pWk->cCaplSyuZai);				// CAPL 主材質
	DBMGR_INSERT_S(pWk->cMetuke);					// 目付規格
	DBMGR_INSERT_S(pWk->cNi);						// Ni付着量
	DBMGR_INSERT_S(pWk->cSodoTCM);					// TCMロール粗度
	DBMGR_INSERT_S(pWk->cSpmUmu);					// SPM圧下有無
	DBMGR_INSERT_S(pWk->cNextLine);					// 計画次工程コード
	DBMGR_INSERT_S(pWk->cMuke);						// 向先
	DBMGR_INSERT_S(pWk->cYouto);					// 用途
	DBMGR_INSERT_S(pWk->cCaplSiage);				// CAPL表面仕上げ
	DBMGR_INSERT_S(pWk->cCaplSanUmu);				// CAPL酸洗有無
	DBMGR_INSERT_S(pWk->cHinMode);					// 品種区分
	DBMGR_INSERT_S(pWk->cGaCrKubun);				// GA/CR区分
	DBMGR_INSERT_S(pWk->cSpmJisekiUmu);				// SPM圧下実績有無
	DBMGR_INSERT_S(pWk->cKouteiCode);				// 工程コード
	DBMGR_INSERT_N(NULL, 0);						// 予備_char_10
	DBMGR_INSERT_N(NULL, 0);						// 予備_char_9
	DBMGR_INSERT_N(NULL, 0);						// 予備_char_8
	DBMGR_INSERT_N(NULL, 0);						// 予備_char_7
	DBMGR_INSERT_N(NULL, 0);						// 予備_char_6
	DBMGR_INSERT_N(NULL, 0);						// 予備_char_5
	DBMGR_INSERT_N(NULL, 0);						// 予備_char_4
	DBMGR_INSERT_N(NULL, 0);						// 予備_char_3
	DBMGR_INSERT_N(NULL, 0);						// 予備_char_2
	DBMGR_INSERT_N(NULL, 0);						// 予備_char_1
	//<<<------------------------------------------------------------------

	CreateSQL::sqlColumnTrim(&sql);					// 最後のカンマを取り除く

	sql += ")";

	//==================================================
	//// 生成したSQL文を DB登録スレッドに登録依頼
	DBSaveManager::DeliverySQL* pDeli = new DBSaveManager::DeliverySQL();
	pDeli->strSql	= sql;
	pDeli->addmode	= true;
	pDeli->bCalcTime= false;
	pDeli->cnt		= 0;


	// 依頼
	if( ! mque_AddDB.AddToTail(pDeli, 20, 20) ){		// 絶対登録
		// 絶対登録も無理…
		LOG(em_ERR), "[%s] DB登録依頼キューフル [DBDATA_COIL_INF]", my_sThreadName);
		syslog(SYSNO_QUEFULL_ERR, "[DBDATA_COIL_INF]");
		SAFE_DELETE(pDeli->SendMailItem);
		delete pDeli;
		// DBが異常(通常ありえない)
		mcls_pStatus->AddQueMyErrWar(true, ON_SAVE_COIL_INF);
	}
}

//==========================================
// コイル実績
//------------------------------------------
// const DBDATA_COIL_RESULT* pWk : 生成情報
//==========================================
void DBManager::CreateSQL_CoilResult(const DBDATA_COIL_RESULT* pWk)
{
	CString sql;

	//======================================
	//// 事前準備
	int infRec = GetRecSQLInf(pWk->cKizukenNo);

	//// バッファレコード特定
	if( -1 == infRec ){
		// 特定失敗
		LOG(em_ERR), "[%s] 保存中コイルバッファに無し [%s]", my_sThreadName, pWk->cKizukenNo);
		syslog(280, "[%s:DBDATA_COIL_RESULT]", pWk->cKizukenNo);
		return;
	}

	//// 書き漏らし疵有り？
	if( 0 != m_SqlInf[infRec].nDeleCnt[0] ||
		0 != m_SqlInf[infRec].nDeleCnt[1] ||
		0 != m_SqlInf[infRec].nLowGrDeleCnt[0] ||
		0 != m_SqlInf[infRec].nLowGrDeleCnt[1] ) {

		int nMsgNo = (0<(m_SqlInf[infRec].nDeleCnt[0]+m_SqlInf[infRec].nDeleCnt[1])) ? 282 : 281;
		LOG(em_WAR), "[%s] DB書き漏らし発生 [%s][有害=%d:%d / 無害=%d:%d]", my_sThreadName, pWk->cKizukenNo, 
			m_SqlInf[infRec].nDeleCnt[0], m_SqlInf[infRec].nDeleCnt[1], m_SqlInf[infRec].nLowGrDeleCnt[0], m_SqlInf[infRec].nLowGrDeleCnt[1]);
		syslog(nMsgNo, "[%s; 有害=%d:%d / 無害=%d:%d]", pWk->cKizukenNo, 
			m_SqlInf[infRec].nDeleCnt[0], m_SqlInf[infRec].nDeleCnt[1], m_SqlInf[infRec].nLowGrDeleCnt[0], m_SqlInf[infRec].nLowGrDeleCnt[1]);

	}

	//// 上限到達解除
	DbBaseManager::SaveMaxReset(infRec);

	// カメラ実績登録
	CreateSQL_CameraResult(pWk);

	//======================================
	//// BCP (コイル単位で残っている画像情報も締め)
	//OnBcpAutoCommit(false);
	
	BCPSaveManager::DeliveryDB *deliDB = new BCPSaveManager::DeliveryDB;
	deliDB->kubun = BCPSaveManager::em_BCP_BATCH;
	deliDB->data = (BYTE*)new DBDATA_BCP_BATCH_EXEC;
	DBDATA_BCP_BATCH_EXEC* pData = (DBDATA_BCP_BATCH_EXEC*)deliDB->data;
	
	pData->bMode = false;


	//================================================
	//// 登録依頼
	if (!mcls_Bcps.gque_AddSQLData.AddToTail(deliDB, 20, 20)) {
		LOG(em_ERR), "[%s] DB登録依頼キューフル [BCP_BATCH_EXEC]", my_sThreadName);
		syslog(SYSNO_QUEFULL_ERR, "[BCP_BATCH_EXEC]");

		delete deliDB->data;
		delete deliDB;
	}

	//======================================
	//// コイル実績
	// ヘッダー部
	sql  = "INSERT INTO ";
	sql += DB_COIL_RESULT;
	sql += " VALUES(";

	// データ部
	//------------------------------------------------------------------>>>
	DBMGR_INSERT_S(pWk->cKizukenNo);											// 管理No
	CreateSQL::sqlColumnAddTime(&sql, true);									// 登録日付
	DBMGR_INSERT_V(pWk->nEndMode);												// 実績格納区分
	for(int ii=0; ii<NUM_MEN; ii++)	DBMGR_INSERT_V(pWk->nSt_Hantei[ii]);		// 判定状態
	for(int ii=0; ii<NUM_MEN; ii++) DBMGR_INSERT_V(pWk->nSt_Light[ii]);			// 照明装置状態
	DBMGR_INSERT_V(pWk->nSt_CamControl);										// カメラコントロール回路状態
	DBMGR_INSERT_V(pWk->nSt_CamSwitch);											// カメラリンクスイッチ回路状態
	DBMGR_INSERT_V(pWk->nSt_Procon);											// プロコン状態
	DBMGR_INSERT_V(pWk->nSt_FF);												// FF状態
	DBMGR_INSERT_V(pWk->nSt_Sqence);											// シーケンサ状態
	for(int ii=0; ii<NUM_MEN; ii++)	DBMGR_INSERT_V(pWk->nSt_Cam_Inline[ii]);	// カメラ台車状態[表裏]
	for(int ii=0; ii<NUM_MEN; ii++) DBMGR_INSERT_V(pWk->nSt_DbSkip[ii]);		// DB未保存[表裏]
	for(int ii=0; ii<NUM_MEN; ii++) DBMGR_INSERT_V(pWk->nSt_FrameSkip[ii]);		// 未検査[表裏]

	// 使用判定
	CString sWkO;	CString sWkU;	CString sWk;
	sWkO = "";		sWkU = "";
	for(int ii=0; ii<NUM_HANTEI; ii++){
		sWk.Format("%02d", pWk->nHtID[0][ii]);	sWkO += sWk;
		sWk.Format("%02d", pWk->nHtID[1][ii]);	sWkU += sWk;
	}
	DBMGR_INSERT_N(sWkO, NUM_HANTEI*2);											// 使用判定_表
	DBMGR_INSERT_N(sWkU, NUM_HANTEI*2);											// 使用判定_裏
	
	// 全長使用判定
	sWkO = "";		sWkU = "";
	for(int ii=0; ii<NUM_RPIEXEC; ii++){
		sWk.Format("%02d", pWk->nRpiID[0][ii]);	sWkO += sWk;
		sWk.Format("%02d", pWk->nRpiID[1][ii]);	sWkU += sWk;
	}
	DBMGR_INSERT_N(sWkO, NUM_RPIEXEC *2);										// 使用全長_表
	DBMGR_INSERT_N(sWkU, NUM_RPIEXEC *2);										// 使用全長_裏

	DBMGR_INSERT_V( (pWk->bStartWpdOk?0:1) );									// 検査基点信号検出可否
	DBMGR_INSERT_V(0);															// 無害Gr削除区分
	DBMGR_INSERT_V(0);															// 統合サーバー取得区分
	DBMGR_INSERT_V( (pWk->setting.bWalm?1:0) );									// ウォーマー材区分
	DBMGR_INSERT_V( pWk->nCoilCnt );											// 検査本数
	DBMGR_INSERT_N(NULL, 0);													// 機器状態予備_10
	DBMGR_INSERT_N(NULL, 0);													// 機器状態予備_9
	DBMGR_INSERT_N(NULL, 0);													// 機器状態予備_8
	DBMGR_INSERT_N(NULL, 0);													// 機器状態予備_7
	DBMGR_INSERT_N(NULL, 0);													// 機器状態予備_6
	DBMGR_INSERT_N(NULL, 0);													// 機器状態予備_5
	DBMGR_INSERT_N(NULL, 0);													// 機器状態予備_4
	DBMGR_INSERT_N(NULL, 0);													// 機器状態予備_3
	DBMGR_INSERT_N(NULL, 0);													// 機器状態予備_2
	DBMGR_INSERT_N(NULL, 0);													// 機器状態予備_1
	DBMGR_INSERT_V( (pWk->setting.bDefScode[0]?1:0) );							// 検査表面状態No_Def区分_表
	DBMGR_INSERT_V( (pWk->setting.bDefScode[1]?1:0) );							// 検査表面状態No_Def区分_裏
	DBMGR_INSERT_V( (pWk->setting.bDefPcode[0]?1:0) );							// 検査パターンコード_Def区分_表
	DBMGR_INSERT_V( (pWk->setting.bDefPcode[1]?1:0) );							// 検査パターンコード_Def区分_裏
	DBMGR_INSERT_V( pWk->setting.scode[0] );									// 検査表面状態No_表
	DBMGR_INSERT_V( pWk->setting.scode[1] );									// 検査表面状態No_裏
	DBMGR_INSERT_V( pWk->setting.pcode[0] );									// 検査パターンコード_表
	DBMGR_INSERT_V( pWk->setting.pcode[1] );									// 検査パターンコード_裏

	// φωで管理するデータだが、後にUpdate
	DBMGR_INSERT_V(pWk->nSpmStat);												// SPM圧下有無
	DBMGR_INSERT_V(pWk->nLength);												// 検査装置実測長
	DBMGR_INSERT_V(pWk->nLengthSub);											// 検査比較実測長
	DBMGR_INSERT_V(pWk->nLengthDsp);											// 表示実測長
	DBMGR_INSERT_V(pWk->nOutCoilPos);											// 出側単位先端位置

	DBMGR_INSERT_V( m_SqlInf[infRec].nTotalCnt[0] );							// 表保存対象疵個数
	DBMGR_INSERT_V( m_SqlInf[infRec].nTotalCnt[1] );							// 裏保存対象疵個数
	DBMGR_INSERT_V( m_SqlInf[infRec].nDeleCnt[0] );								// 表削除有害疵個数
	DBMGR_INSERT_V( m_SqlInf[infRec].nDeleCnt[1] );								// 裏削除有害疵個数
	DBMGR_INSERT_V( m_SqlInf[infRec].nLowGrDeleCnt[0] );						// 表削除無害疵個数
	DBMGR_INSERT_V( m_SqlInf[infRec].nLowGrDeleCnt[1] );						// 裏削除無害疵個数
	DBMGR_INSERT_V( m_SqlInf[infRec].nImgDeleCnt[0] );							// 表画像削除有害疵個数
	DBMGR_INSERT_V( m_SqlInf[infRec].nImgDeleCnt[1] );							// 裏画像削除有害疵個数
	DBMGR_INSERT_V( m_SqlInf[infRec].nImgLowGrDeleCnt[0] );						// 表画像削除無害疵個数
	DBMGR_INSERT_V( m_SqlInf[infRec].nImgLowGrDeleCnt[1] );						// 裏画像削除無害疵個数
	DBMGR_INSERT_V( m_SqlInf[infRec].nSaveCnt[0] );								// 表保存疵個数
	DBMGR_INSERT_V( m_SqlInf[infRec].nSaveCnt[1] );								// 裏保存疵個数
	DBMGR_INSERT_V( m_SqlInf[infRec].nSaveImgAllCnt[0] );						// 表画像保存疵個数
	DBMGR_INSERT_V( m_SqlInf[infRec].nSaveImgAllCnt[1] );						// 裏画像保存疵個数
	DBMGR_INSERT_V( pWk->emUnten );												// 運転状態
	DBMGR_INSERT_V( pWk->emOneKensa[0] );										// 検査状態_表
	DBMGR_INSERT_V( pWk->emOneKensa[1] );										// 検査状態_裏
	DBMGR_INSERT_V( pWk->emOneKiki[0] );										// 機器状態_表
	DBMGR_INSERT_V( pWk->emOneKiki[1] );										// 機器状態_裏

	DBMGR_INSERT_V( pWk->emPlg );												// PLG選択区分
	DBMGR_INSERT_V( pWk->nResultSendOk );										// 外部実績送信許可
	
	// 別タスクでUpdate
	DBMGR_INSERT_V(1.0);														// 補正率
	DBMGR_INSERT_V( 1);															// プロコン実績反映
	DBMGR_INSERT_V( 0);															// プロコン実測長
	DBMGR_INSERT_V( 0);															// 成品数

	DBMGR_INSERT_V( pWk->nToyuSply );											// 塗油スプレー装置状態
	//<<<------------------------------------------------------------------

	CreateSQL::sqlColumnTrim(&sql);					// 最後のカンマを取り除く

	sql += ")";


	//======================================================	
	//// 生成したSQL文を DB登録スレッドに登録依頼する
	DBSaveManager::DeliverySQL* pDeli = new DBSaveManager::DeliverySQL();
	pDeli->strSql	= sql;
	pDeli->addmode	= true;
	pDeli->bCalcTime= false;
	pDeli->cnt		= 0;

	//======================================================	
	// コイル実績登録完了時に 疵トレンド画面 に 画面更新通知を送る
	pDeli->SendMailItem = NULL;
	pDeli->SendMailItem = new MAIL_ITEM;
	memset(pDeli->SendMailItem, 0x00, sizeof(MAIL_ITEM));
	strcpy(pDeli->SendMailItem->cPc, "*");
	strcpy(pDeli->SendMailItem->cTask, SO_TRAND);
	pDeli->SendMailItem->Que.nEventNo = FACT_SO_TRAND_01;

	// 依頼
	if( ! mque_AddDB.AddToTail(pDeli, 20, 20) ) {		// 絶対登録
		// 絶対登録も無理・・・
		LOG(em_ERR), "[%s] DB登録依頼キューフル [DBDATA_COIL_RESULT]", my_sThreadName);
		syslog(SYSNO_QUEFULL_ERR, "[DBDATA_COIL_RESULT]");
		SAFE_DELETE(pDeli->SendMailItem);
		delete pDeli;
		// DBが異常 (通常ありえない)
		mcls_pStatus->AddQueMyErrWar(true, ON_SAVE_COIL_RESULT);
	} else {
		syslog(283, "[%s] コイル実績", pWk->cKizukenNo);
	}
}

//==========================================
// カメラ実績
//------------------------------------------
// const DBDATA_COIL_RESULT* pWk : 生成情報
//==========================================
void DBManager::CreateSQL_CameraResult(const DBDATA_COIL_RESULT* pWk)
{
	CString sql;

	//======================================
	//// コイル実績
	// ヘッダー部
	sql = "INSERT INTO ";
	sql += DB_COIL_CAMERA;
	sql += " VALUES(";

	// データ部
	//------------------------------------------------------------------>>>
	DBMGR_INSERT_S(pWk->cKizukenNo);									// 管理No
	// 輝度
	for (int ii = 0; ii < MAX_CAMSET; ii++)
	{
		for (int jj = 0; jj < (MAX_CAMANGLE - 1); jj++)
		{
			DBMGR_INSERT_V(pWk->nBr[ii][jj][0]);						// 輝度(C)
		}
	}
	// 露光時間
	for (int ii = 0; ii < MAX_CAMSET; ii++)
	{
		for (int jj = 0; jj < (MAX_CAMANGLE - 1); jj++)
		{
			DBMGR_INSERT_V(pWk->nExp[ii][jj]);							// 露光時間[clk]
		}
	}
	// ゲイン
	for (int ii = 0; ii < MAX_CAMSET; ii++)
	{
		for (int jj = 0; jj < (MAX_CAMANGLE - 1); jj++)
		{
			DBMGR_INSERT_V(pWk->nGain[ii][jj]);							// ゲイン[倍]
		}
	}
	//<<<------------------------------------------------------------------

	CreateSQL::sqlColumnTrim(&sql);					// 最後のカンマを取り除く

	sql += ")";

	//======================================================	
	//// 生成したSQL文を DB登録スレッドに登録依頼する
	DBSaveManager::DeliverySQL* pDeli = new DBSaveManager::DeliverySQL();
	pDeli->strSql = sql;
	pDeli->addmode = true;
	pDeli->bCalcTime = false;
	pDeli->cnt = 0;

	// 依頼
	if (!mque_AddDB.AddToTail(pDeli, 20, 20)) {		// 絶対登録
													// 絶対登録も無理・・・
		LOG(em_ERR), "[%s] DB登録依頼キューフル [DBDATA_COIL_CAMERA]", my_sThreadName);
		syslog(SYSNO_QUEFULL_ERR, "[DBDATA_COIL_CAMERA]");
		SAFE_DELETE(pDeli->SendMailItem);
		delete pDeli;
		// DBが異常 (通常ありえない)
		mcls_pStatus->AddQueMyErrWar(true, ON_SAVE_COIL_RESULT);
	}
	else {
		syslog(283, "[%s] カメラ実績", pWk->cKizukenNo);
	}
}

//==========================================
// コイル実績アップデート （実績確定後のUpDate用）
//------------------------------------------
// const DBDATA_COIL_RESULT_UPDATE* pWk	: 生成情報
//==========================================
void DBManager::CreateSQL_CoilResultUpdate(const DBDATA_COIL_RESULT_UPDATE* pWk)
{
	CString sql;	

	// /////////////////////////////////
	// レコード特定
	int infRec = GetRecSQLInf(pWk->cKizukenNo);
	if( -1 == infRec ) {
		LOG(em_ERR), "[%s] 保存中コイルバッファに無し [%s]", my_sThreadName, pWk->cKizukenNo );
		syslog(280, "[%s]", pWk->cKizukenNo);
		return;
	}

	//======================================================	
	////// コイル実績
	// ヘッダー部
	sql = "UPDATE ";
	sql += DB_COIL_RESULT;
	sql += " SET ";	

	// -------------------------------------------------------------------------------------------->>>
	// データ部

	// シャーカット信号受信
	if(MODE_RECV_SHRCUT == pWk->emMode) {
		CreateSQL::sqlColumnUpdate(&sql, "出側単位先端位置",	pWk->nOutCoilPos);					// 出側単位先端位置 [mm]
	}
	// 表示基点WPD ON
	else if(MODE_DSP_WPD_ON == pWk->emMode) {
		CreateSQL::sqlColumnUpdate(&sql, "表示実測長",			pWk->nDspLength);					// 表示実測長 [mm]
		CreateSQL::sqlColumnUpdate(&sql, "SPM圧下有無",			pWk->nSpmUmu);
	}
	// <<<--------------------------------------------------------------------------------------------

	CreateSQL::sqlColumnTrim(&sql);		// 最後のカンマを取り除く

	// Where
	sql += " WHERE ";
	CreateSQL::sqlColumnUpdate(&sql, "管理No", pWk->cKizukenNo, SIZE_KIZUKEN_NO);					// 管理No 
	CreateSQL::sqlColumnTrim(&sql);		// 最後のカンマを取り除く


	//======================================================	
	//// 生成したSQL文を DB登録スレッドに登録依頼する
	DBSaveManager::DeliverySQL*  pDeli = new DBSaveManager::DeliverySQL();
	pDeli->strSql	= sql;
	pDeli->addmode	= true;
	pDeli->bCalcTime= false;
	pDeli->cnt		= 0;

	//======================================================	
	// コイル実績更新完了時に 疵トレンド画面 に 画面更新通知を送る
	pDeli->SendMailItem = NULL;
	pDeli->SendMailItem = new MAIL_ITEM;
	memset(pDeli->SendMailItem, 0x00, sizeof(MAIL_ITEM));
	strcpy(pDeli->SendMailItem->cPc, "*");
	strcpy(pDeli->SendMailItem->cTask, SO_TRAND);
	pDeli->SendMailItem->Que.nEventNo = FACT_SO_TRAND_01;

	// 依頼
	if( ! mque_AddDB.AddToTail(pDeli, 20, 20) ) {		// 絶対登録
		// 絶対登録も無理・・・
		LOG(em_ERR), "[%s] DB登録依頼キューフル [DBDATA_RESULT_UPDATE]", my_sThreadName);
		syslog(SYSNO_QUEFULL_ERR, "[DBDATA_RESULT_UPDATE]");
		delete pDeli;
		// DBが異常 (通常ありえない)
		mcls_pStatus->AddQueMyErrWar(true, ON_SAVE_COIL_RESULT);
	}
}

//==========================================
// コイル位置
//------------------------------------------
// const DBDATA_COIL_POSITION* pWk	: 生成情報
//==========================================
void DBManager::CreateSQL_CoilPosition(const DBDATA_COIL_POSITION* pWk)
{
	CString sql;

	//========================================================
	////// コイル位置情報
	// ヘッダー部
	sql  = "INSERT INTO ";
	sql += DB_COIL_POSITION;
	sql += " VALUES(";

	//------------------------------------------------------->>>
	// データ部
	DBMGR_INSERT_S(pWk->cKizukenNo);											// 管理No
	DBMGR_INSERT_V(pWk->nID);													// ID
	for (int ii = 0; ii < NUM_MEN; ii++)DBMGR_INSERT_V(pWk->emKensa[ii]);		// 検査状態
	for (int ii = 0; ii < NUM_MEN; ii++)DBMGR_INSERT_V(pWk->emKiki[ii]);		// 機器状態
	for (int ii = 0; ii < 2; ii++)		DBMGR_INSERT_V(pWk->dEdge[ii]);			// エッジ位置生[0:左/1:右]
	DBMGR_INSERT_V(pWk->dWalk);													// ウォーク量
	//<<<-------------------------------------------------------

	CreateSQL::sqlColumnTrim(&sql);												// 最後のカンマを取り除く

	sql += ")";

	//======================================================	
	//// 生成したSQL文を DB登録スレッドに登録依頼する
	DBSaveManager::DeliverySQL* pDeli = new DBSaveManager::DeliverySQL();
	pDeli->strSql	= sql;
	pDeli->addmode	= true;
	pDeli->bCalcTime= false;
	pDeli->cnt		= 0;


	// 依頼
	if( ! mque_AddDB.AddToTailFreeCheck(pDeli, 10) ) {
		// キューフル
		delete pDeli;
		LOG(em_ERR), "[%s] DB登録依頼キューフル [DBDATA_COIL_POSITION]", my_sThreadName);
	}

}

//==========================================
// コイルトレンド
//------------------------------------------
// const DBDATA_COIL_TRAND* pWk	: 生成情報
//==========================================
void DBManager::CreateSQL_CoilTrand(const DBDATA_COIL_TRAND* pWk)
{
	CString sql;

	//===========================================
	//// コイルトレンド情報
	// ヘッダー部
	sql  = "INSERT INTO ";
	sql += DB_COIL_TRAND;
	sql += " VALUES(";

	//----------------------------------------------------------------------->>>
	// データ部
	DBMGR_INSERT_S(pWk->cKizukenNo);							// 管理No
	DBMGR_INSERT_V(pWk->nMen);									// 表裏区分
	double dLen = (double)pWk->nLen * pWk->dSpmHosei;
	DBMGR_INSERT_V((int)dLen);										// 取得位置
	CreateSQL::sqlColumnAddTime(&sql, true);					// 登録日付
	DBMGR_INSERT_V(pWk->nSpeed);								// ライン速度
	DBMGR_INSERT_V(pWk->dCoilWidMax);							// 板幅Min
	DBMGR_INSERT_V(pWk->dCoilWidMin);							// 板幅Max
	DBMGR_INSERT_V(pWk->dWalk);									// ウォーク量
	// 照明装置_調光値[0:R 1:G1 2:G2 3:B1 4:B2]
	for (int ii = 0; ii < DIV_LED_COLOR_END; ii++)		 DBMGR_INSERT_V(pWk->nLightValue[ii]);
	// 輝度[0:DS～9:WS][0:R 1:G 2:B 3:G-B][0:C]
	for (int ii = 0; ii < NUM_CAM_POS; ii++)
	{
		for (int jj = 0; jj < (NUM_CAM_ANGLE - 1); jj++) DBMGR_INSERT_V(pWk->nBr[ii][jj][0]);
	}
	// 露光時間[0:DS～9:WS][0:R 1:G 2:B 3:G-B]
	for (int ii = 0; ii < NUM_CAM_POS; ii++)
	{
		for (int jj = 0; jj < (NUM_CAM_ANGLE - 1); jj++) DBMGR_INSERT_V(filter_short(pWk->nExp[ii][jj], SHRT_MIN, SHRT_MAX));
	}
	// カメラゲイン値[0:DS～9:WS][0:R 1:G 2:B 3:G-B]
	for (int ii = 0; ii < NUM_CAM_POS; ii++)
	{
		for (int jj = 0; jj < (NUM_CAM_ANGLE - 1); jj++) DBMGR_INSERT_V(pWk->dGain[ii][jj]);
	}
	//<<<-----------------------------------------------------------------------
	CreateSQL::sqlColumnTrim(&sql);								// 最後のカンマを取り除く
	sql += ")";

	//// 依頼
	DBSaveManager::DeliverySQL*  pDeli = new DBSaveManager::DeliverySQL();
	pDeli->addmode	= true;
	pDeli->cnt		= 0;
	pDeli->strSql = sql;
	if( ! mque_AddDB.AddToTailFreeCheck(pDeli, 30) ) {
		LOG(em_ERR), "[%s] DB登録依頼キューフル [DB_COIL_TRAND]", my_sThreadName);
		delete pDeli;
	}
}

//==========================================
// コイルカット位置
//------------------------------------------
// const DBDATA_COIL_CUT_POS* pWk : 生成情報
//==========================================
void DBManager::CreateSQL_CoilCutPos(const DBDATA_COIL_CUT_POS* pWk)
{
	CString sql;

	////// コイルカット位置情報
	for (int ii = 0; ii<pWk->data.nCnt; ii++)
	{
		// ヘッダー部
		sql  = "INSERT INTO ";
		sql += DB_COIL_CUT_POS;
		sql += " VALUES(";

		//---------------------------------------------------------------->>>
		// データ部
		DBMGR_INSERT_S(pWk->data.incoil[ii].cKizukenNo);	// 管理No
		DBMGR_INSERT_V(pWk->data.incoil[ii].nInCutNum);		// 入側子番
		CreateSQL::sqlColumnAddTime(&sql, true);			// 登録日付
		DBMGR_INSERT_S(pWk->data.cID);						// ID
		DBMGR_INSERT_V(ii+1);								// No(出側分割No)
		DBMGR_INSERT_V(pWk->nMode);							// 区分
		DBMGR_INSERT_V(pWk->data.incoil[ii].nFront);		// 先頭位置
		DBMGR_INSERT_V(pWk->data.incoil[ii].nLength);		// 保有長さ
		DBMGR_INSERT_V(pWk->data.incoil[ii].nOutPos);		// 出側距離位置
		//<<<----------------------------------------------------------------

		CreateSQL::sqlColumnTrim(&sql);						// 最後のカンマを取り除く

		sql += ")";

		//// 依頼
		DBSaveManager::DeliverySQL*  pDeli = new DBSaveManager::DeliverySQL();
		pDeli->addmode	= true;
		pDeli->cnt		= 0;
		pDeli->strSql = sql;
		if( ! mque_AddDB.AddToTail(pDeli, 20, 20) ) {		// 絶対登録
			// 絶対登録も無理・・・
			LOG(em_ERR), "[%s] DB登録依頼キューフル [DB_COIL_CUT_POS]", my_sThreadName);
			syslog(SYSNO_QUEFULL_ERR, "[DB_COIL_CUT_POS]");
			delete pDeli;
		}
	}
}

//==========================================
// 疵情報
//------------------------------------------
// const DBDATA_DEFECT_INF* pWk	: 生成情報
//==========================================
BOOL DBManager::CreateSQL_DefectInf(const DBDATA_DEFECT_INF* pWk)
{
	int  ou;							// 表裏区分[0:表 / 1:裏]
	int  infRec;						// バッファレコード
	int  nKizuNo;						// 今回の疵No
	bool bImgSkip = false;				// true時画像のみスキップ
	CString sql;
	DBSaveManager::DeliverySQL* pDeli;

	//==================================================
	// 事前準備
	ou		= pWk->data.TorB;
	infRec	= GetRecSQLInf(pWk->cKizukenNo);

	//// バッファレコード特定
	if( -1 == infRec ){
		// 特定失敗
		LOG(em_ERR), "[%s] 保存中コイルバッファに無し [%s]", my_sThreadName, pWk->cKizukenNo);
		syslog(280, "[%s:DBDATA_DEFECT_INF]", pWk->cKizukenNo);
		return false;
	}

	//==================================================
	// フロー制御
	int CheckResult = this->SaveMaxCheck(infRec, ou, pWk->nLowGrFlg);

	//// 最大保存件数に到達
	if( -1 == CheckResult ) return false;

	//// 画像のみスキップか判定
	if( pWk->bImgSkip || 9 == CheckResult ) bImgSkip = true;

	//// 削除優先？
	if( true){
		int nQueCntSQL = gque_AddSQLData.GetCount();		// SQL登録キュー
		int nQueCntDB  = mque_AddDB.GetCount();				// DB登録キュー
		if( 200<=nQueCntSQL || 200<=nQueCntDB ) this->Send_To_DbManage(true);		// 書込優先を送信
		else									this->Send_To_DbManage(false);		// 削除優先を送信
	}

	this->mcls_Time.CounterStart();		// 処理時間計測開始

	//==================================================
	// 疵個数加算
	m_SqlInf[infRec].nSaveCnt[ou]++;
	m_SqlInf[infRec].nTotalCnt[ou]++;
	nKizuNo = m_SqlInf[infRec].nSaveCnt[0] + m_SqlInf[infRec].nSaveCnt[1];
	// 20200520 疵保存個数上限拡張 --->>>
	int nOffsetVrcl = VRCL_DEFECT_NO_END - VRCL_DEFECT_NO_START + 1;
	if (VRCL_DEFECT_NO_START <= nKizuNo) {		// VRCL新規疵入力範囲を除外する
		nKizuNo = nKizuNo + nOffsetVrcl;
	}
	// 20200520 疵保存個数上限拡張 ---<<<

	if( !bImgSkip ) {
		m_SqlInf[infRec].nSaveImgAllCnt[ou]++;
		m_SqlInf[infRec].nSaveImgCnt[ou]++;
	}

	//======================================================
	// 疵情報
	pDeli = new DBSaveManager::DeliverySQL();
	pDeli->addmode	= true;
	pDeli->bCalcTime= true;
	pDeli->cnt		= 0;
 
	////// 疵情報
	// ヘッダー部
	sql = "INSERT INTO ";
	if(0 == ou) sql += DB_DEFECT_INF_T;
	else		sql += DB_DEFECT_INF_B;
	sql += " VALUES(";

	//---------------------------------------------------------------->>>
	// データ部
	DBMGR_INSERT_S(pWk->cKizukenNo);								// 管理No 
	DBMGR_INSERT_V(nKizuNo);										// 疵No

	DBMGR_INSERT_V( pWk->data.TorB);								// 表裏
	DBMGR_INSERT_V( (int)pWk->attr_base[EPC_ATTR_BASE_MERGE]);		// 重複結果 (1:重複) 
	DBMGR_INSERT_V( pWk->data.nTid);								// 疵種EdasysID
	DBMGR_INSERT_V( pWk->data.nGid);								// グレードEdasysID
	DBMGR_INSERT_V( pWk->data.cycle_div);							// 周期疵区分(0:一般疵, 1:簡易周期疵,連続疵 2:周期疵)
	DBMGR_INSERT_V( pWk->data.dsws);								// DS,WS区分 (0,1)
	DBMGR_INSERT_V( pWk->data.camset);								// カメラセットID (1～20)

	DBMGR_INSERT_V( pWk->dLen);										// Fからの位置[mm]
	DBMGR_INSERT_V( pWk->data.ds);									// DS側エッジからの位置[mm]
	DBMGR_INSERT_V( pWk->data.ws);									// WS側エッジからの位置[mm]

	DBMGR_INSERT_V( pWk->data.speed);								// 速度[mpm]
	DBMGR_INSERT_V( pWk->data.pitch);								// ピッチ[mm]
	DBMGR_INSERT_V( pWk->data.roolkei);								// ロール径[mm]
	
	DBMGR_INSERT_V( pWk->data.aria);								// 面積 [mm^2]
	DBMGR_INSERT_V( (int)pWk->data.kizu_wid);						// 疵の幅 [mm]
	DBMGR_INSERT_V( (int)pWk->data.kizu_len);						// 疵の長さ [mm]
	DBMGR_INSERT_V( pWk->nLowGrFlg);								// 無害Gr / 有害Gr

	DBMGR_INSERT_V( pWk->data.nWidDiv);								// 幅分割区分
	DBMGR_INSERT_V( pWk->data.nWidAbsPos);							// 幅方向絶対位置
	DBMGR_INSERT_V( pWk->nIsDspFlg);								// 表示対象区分_探傷画面

	DBMGR_INSERT_N(NULL, 0);										// 10
	DBMGR_INSERT_N(NULL, 0);										// 9
	DBMGR_INSERT_N(NULL, 0);										// 8
	DBMGR_INSERT_N(NULL, 0);										// 7
	DBMGR_INSERT_N(NULL, 0);										// 6
	DBMGR_INSERT_N(NULL, 0);										// 5
	DBMGR_INSERT_N(NULL, 0);										// 4
	DBMGR_INSERT_N(NULL, 0);										// 3
	DBMGR_INSERT_N(NULL, 0);										// 2
	DBMGR_INSERT_N(NULL, 0);										// 1

	DBMGR_INSERT_V(pWk->data.nKizuNo);								// 疵連番
	DBMGR_INSERT_V( pWk->data.h_res);								// 縦分解能[mm/pixel]
	DBMGR_INSERT_V( pWk->data.w_res);								// 横分解能[mm/pixel]
	DBMGR_INSERT_V( (int)pWk->data.nFrameNo);						// フレームNo

	DBMGR_INSERT_V( pWk->data.frm_x_min);							// フレーム外接Xmin[pixel] (特徴量の外接長方形そのもの)
	DBMGR_INSERT_V( pWk->data.frm_x_max);							// フレーム外接Xmax[pixel] 
	DBMGR_INSERT_V( pWk->data.frm_y_min);							// フレーム外接Ymin[pixel]
	DBMGR_INSERT_V( pWk->data.frm_y_max);							// フレーム外接Ymax[pixel]

	DBMGR_INSERT_V( pWk->data.cull_frm_x_min);						// 間引きフレーム外接Xmin[pixel] (特徴量の外接長方形そのもの)
	DBMGR_INSERT_V( pWk->data.cull_frm_x_max);						// 間引きフレーム外接Xmax[pixel] 
	DBMGR_INSERT_V( pWk->data.cull_frm_y_min);						// 間引きフレーム外接Ymin[pixel]
	DBMGR_INSERT_V( pWk->data.cull_frm_y_max);						// 間引きフレーム外接Ymax[pixel]

	DBMGR_INSERT_V( pWk->data.box_x_min);							// 外接Xmin[pixel]		(マイナス値ありえる)
	DBMGR_INSERT_V( pWk->data.box_x_max);							// 外接Xmax[pixel]	
	DBMGR_INSERT_V( pWk->data.box_y_min);							// 外接Ymin[pixel]
	DBMGR_INSERT_V( pWk->data.box_y_max);							// 外接Ymax[pixel]
	DBMGR_INSERT_V( pWk->data.center_x);							// 疵中心位置X [pixel] 
	DBMGR_INSERT_V( pWk->data.center_y);							// 疵中心位置Y [pixel]
	DBMGR_INSERT_V( pWk->data.h_size);								// 縦画素数[pixel]
	DBMGR_INSERT_V( pWk->data.w_size);								// 横画素数[pixel]
	
	for(int ii=0; ii<MAX_HT1_CAMANGLE; ii++) {
		DBMGR_INSERT_V( pWk->nImgsize[ii]);							// 画像サイズ（正／乱）
	}
	
	DBMGR_INSERT_V(pWk->data.base_box_x_min);						// 元画像外接Xmin[pixel]	(マイナス値ありえる)
	DBMGR_INSERT_V(pWk->data.base_box_x_max);						// 元画像外接Xmax[pixel]	
	DBMGR_INSERT_V(pWk->data.base_box_y_min);						// 元画像外接Ymin[pixel]
	DBMGR_INSERT_V(pWk->data.base_box_y_max);						// 元画像外接Ymax[pixel]
	DBMGR_INSERT_V(pWk->data.base_center_x);						// 元画像疵中心位置X [pixel] 
	DBMGR_INSERT_V(pWk->data.base_center_y);						// 元画像疵中心位置Y [pixel]
	DBMGR_INSERT_V(pWk->data.base_h_size);							// 元画像縦画素数[pixel]
	DBMGR_INSERT_V(pWk->data.base_w_size);							// 元画像横画素数[pixel]

	DBMGR_INSERT_V(pWk->data.base_h_res);							// 元縦分解能[mm/pixel]
	DBMGR_INSERT_V(pWk->data.base_w_res);							// 元横分解能[mm/pixel]
	DBMGR_INSERT_V(pWk->data.nShrinkRatio);							// 縮小率
	//<<<----------------------------------------------------------------

	CreateSQL::sqlColumnTrim(&sql);		// 最後のカンマを取り除く

	sql += ")";

	//// 依頼
	pDeli->strSql = sql;
	if( ! mque_AddDB.AddToTail(pDeli, 20, 20) ) {		// 絶対登録
		// 絶対登録も無理・・・
		LOG(em_ERR), "[%s] DB登録依頼キューフル [DB_DEFECT_INF]", my_sThreadName);
		syslog(SYSNO_QUEFULL_ERR, "[DB_DEFECT_INF]");
		delete pDeli;
		// DBが異常 (上で最低減チェックしている為、通常ありえない)
		mcls_pStatus->AddQueMyErrWar(false, ON_SAVE_DEFECT_INF);
	}

	// BCP書込みクラスに書込み依頼
	BCPSaveManager::DeliveryDB *deliDB = new BCPSaveManager::DeliveryDB;
	deliDB->kubun = BCPSaveManager::em_DEFECTINF;
	deliDB->data = (BYTE*)new DBDATA_DEFECT_INF;
	DBDATA_DEFECT_INF* pData = (DBDATA_DEFECT_INF*)deliDB->data;
	memcpy(pData, pWk, sizeof(DBDATA_DEFECT_INF));
	
	
	// 画像データセット
	for(int ii=0; ii<MAX_CAMANGLE; ii++) {
		pData->pImg[ii]	  = pWk->pImg[ii];
	}

	// BCP書込みクラス受渡し用変数
	pData->nKizuNo = nKizuNo;
	pData->bImgSkip = bImgSkip;

	//================================================
	//// 登録依頼
	if (!mcls_Bcps.gque_AddSQLData.AddToTail(deliDB, 20, 20)) {
		LOG(em_ERR), "[%s] DB登録依頼キューフル [DBDATA_DEFECT_INF]", my_sThreadName);
		syslog(SYSNO_QUEFULL_WAR, "[DBDATA_DEFECT_INF]");

		// キューフル
		for(int ii=0; ii<MAX_CAMANGLE; ii++) { SAFE_DELETE_ARRAY(pData->pImg[ii]); }
		delete deliDB->data;
		delete deliDB;
	}

	// 処理時間計測終了
	mcls_Time.CounterEnd();

	return true;
}

//==========================================
// エッジ検出実績
//------------------------------------------
// const DBDATA_EDGE_COIL_INF* pWk : 生成情報
//==========================================
void DBManager::CreateSQL_EdgeCoilInf(const DBDATA_EDGE_COIL_INF* pWk)
{
	CString sql;	

	////// 情報
	// ヘッダー部
	sql = "INSERT INTO ";
	sql += DB_EDGE_COIL_INF;
	sql += " VALUES(";

	// データ部
	DBMGR_INSERT_S( pWk->cKizukenNo);								// 管理No 
	DBMGR_INSERT_V( pWk->nMen);										// 表裏
	DBMGR_INSERT_V( pWk->dCoilWidMin);								// 板幅最小
	DBMGR_INSERT_V( pWk->dCoilWidMax);								// 板幅最大

	DBMGR_INSERT_V( pWk->nLimitEdgeErrNum);							// 板幅許容誤差範囲外数
	DBMGR_INSERT_V( pWk->nLostEdgeNum[0]);							// エッジ未検出数 (左エッジ)
	DBMGR_INSERT_V( pWk->nLostEdgeNum[1]);							// エッジ未検出数 (右エッジ)
	DBMGR_INSERT_V( pWk->nFrameNumCoil);							// コイル単位フレーム数
	for(int ii=0; ii<NUM_CAM_POS; ii++)	{
		for(int jj=0; jj<5; jj++) {
			DBMGR_INSERT_V( pWk->nFrameNumSkip[ii][jj]); 
		}
	}

	CreateSQL::sqlColumnTrim(&sql);		// 最後のカンマを取り除く
	sql += ")";
	
	//// 依頼
	DBSaveManager::DeliverySQL*  pDeli = new DBSaveManager::DeliverySQL();
	pDeli->addmode	= true;
	pDeli->cnt		= 0;
	pDeli->strSql = sql;
	if( ! mque_AddDB.AddToTail(pDeli, 20, 20) ) {		// 絶対登録
		// 絶対登録も無理・・・
		LOG(em_ERR), "[%s] DB登録依頼キューフル [DB_EDGE_COIL_INF]", my_sThreadName);
		syslog(SYSNO_QUEFULL_ERR, "[DB_EDGE_COIL_INF]");
		delete pDeli;
	}
}

//==========================================
// エッジ検出モード変化管理
//------------------------------------------
// const DBDATA_EDGE_CHANGE_INF* pWk	
//==========================================
void DBManager::CreateSQL_EdgeChangeInf(const DBDATA_EDGE_CHANGE_INF* pWk)
{
	CString sql;	

	//======================================
	// EDGE_CHANGE_INF
	////// 情報
	// ヘッダー部
	sql = "INSERT INTO ";
	sql += DB_EDGE_CHANGE_INF;
	sql += " VALUES(";

	//---------------------------------------------------------------->>>
	// データ部
	DBMGR_INSERT_S( pWk->cKizukenNo);			// 管理No 
	DBMGR_INSERT_V((int)(pWk->nPos));			// 長手位置 [mm]
	DBMGR_INSERT_V( pWk->nMen);					// 表裏区分 (0:表 1:裏)
	DBMGR_INSERT_V( pWk->nFrameNo);				// フレームNo
	DBMGR_INSERT_V( pWk->nEdgeMode);			// 検出区分 (0:正常  1:片ｴｯｼﾞ優先  2:ｾﾝﾀｰﾗｲﾝ振り分け  -1:スキップ   -9:対象外   -99:初期状態）
	DBMGR_INSERT_V( pWk->dEdgeRawPos[0]);		// 生エッジ位置 [mm] (左エッジ)
	DBMGR_INSERT_V( pWk->dEdgeRawPos[1]);		// 生エッジ位置 [mm] (右エッジ)
	DBMGR_INSERT_V( pWk->dEdgeAnsPos[0]);		// 最終エッジ位置 [mm] (左エッジ)
	DBMGR_INSERT_V( pWk->dEdgeAnsPos[1]);		// 最終エッジ位置 [mm] (右エッジ)
	DBMGR_INSERT_V( pWk->nEdgeErrReason);		// エッジ不正理由の論理和
	//<<<----------------------------------------------------------------

	CreateSQL::sqlColumnTrim(&sql);				// 最後のカンマを取り除く

	sql += ")";
	
	//// 依頼
	DBSaveManager::DeliverySQL*  pDeli = new DBSaveManager::DeliverySQL();
	pDeli->addmode	= true;
	pDeli->cnt		= 0;
	pDeli->strSql = sql;
	if( ! mque_AddDB.AddToTail(pDeli, 20, 20) ) {		// 絶対登録
		// 絶対登録も無理・・・
		LOG(em_ERR), "[%s] DB登録依頼キューフル [DB_EDGE_CHANGE_INF]", my_sThreadName);
		syslog(SYSNO_QUEFULL_ERR, "[DB_EDGE_CHANGE_INF]");
		delete pDeli;
	}
}


//==========================================
// カメラリンクスイッチ設定
//------------------------------------------
// const DBDATA_CAMERA_LINKSWITCH* pWk : 生成情報	
//==========================================
void DBManager::CreateSQL_CamLinkSW(const DBDATA_CAMERA_LINKSWITCH* pWk)
{
	CString sql;

	// カメラリンクスイッチ接続設定取得
	ToLinkswUtil::TO_LINKSW_OUTPUT info;
	memset(&info, 0x00, sizeof(info));
	ToLinkswUtil::GetDeterminedHanteiPC(mcls_pStatus, &info);

	// ----------------------
	// DBに登録
	OdbcBase clsDB;
	if( ! clsDB.Connect(TASKINI_NAME)) {
		LOG(em_ERR), "[%s] DB接続エラー", my_sThreadName);
		syslog(SYSNO_DB_CONNECT_ERR, "");
		return;
	}

	// 既存定義クリア
	sql.Format("UPDATE %s SET ", DB_CAMERA_LINKSWITCH);
	for (int ii = 0; ii < NUM_CAMLINKSW_CH_MAX; ii++)
	{
		if (0 != ii) sql.AppendFormat(", ");
		sql.AppendFormat("[ボード%d_カメラ選択]=0", ii + 1);
	}
	if (!clsDB.ExecuteDirect(sql)) {
		LOG(em_ERR), "[%s] %s<%s>", my_sThreadName, clsDB.GetLastErrMsg(), sql);
		syslog(SYSNO_DB_EXECUTEDIRECT_ERR, "[%s]", sql);
		return;
	}

	Sleep(100);

	// 判定台数分カメラセット登録
	sql = "";
	for(int ii=0; ii<NUM_ALL_HT; ii++ ) {
		for(int jj=0; jj<NUM_CAMLINK_BOARD; jj++) {
			if (0 == info.output[ii][jj].nFPGACh) continue;				// ボード未割当
			sql.Format("UPDATE %s SET [ボード%d_カメラ選択]=%d WHERE ボードID=%d", DB_CAMERA_LINKSWITCH,
				info.output[ii][jj].nFPGACh,
				info.output[ii][jj].nCamPort,
				info.output[ii][jj].nFPGABoardNo );

			if( !clsDB.ExecuteDirect(sql)) {
				LOG(em_ERR), "[%s] %s<%s>", my_sThreadName, clsDB.GetLastErrMsg(), sql);
				syslog(SYSNO_DB_EXECUTEDIRECT_ERR, "[%s]", sql);
				return;
			}
		}
	}

	LOG(em_MSG), "[%s] CamLinkSw DB定義変更", my_sThreadName);
	CString sTarget;
	int nPCID = 0;
	for (int ii = 0; ii < NUM_ALL_HT; ii++)
	{
		sTarget.Empty();
		if (0 == info.output[ii][0].nFPGACh)
		{
			sTarget = "-----------------";
		}
		else
		{
			sTarget.AppendFormat("PCID:%02d(%s)", nPCID + 1, HANTEI(nPCID));
			nPCID++;
		}

		for (int jj = 0; jj < NUM_CAMLINK_BOARD; jj++)
		{
			LOG(em_MSG), "[%s]   %s LinkswNo:%02d FPGA:%d Ch:%02d camNo:%02d camset:%02d",
				my_sThreadName,
				sTarget,
				info.output[ii][jj].nLinkswNo,
				info.output[ii][jj].nFPGABoardNo,
				info.output[ii][jj].nFPGACh,
				info.output[ii][jj].nCamPort,
				info.output[ii][jj].nCamset);
		}
	}

	LOG(em_MSG), "[%s] 検査開始設定コマンド送信(FACT_TO_LINKSW_02)", my_sThreadName);
	ToOutMailSender::Send_ToLinksw_Start();
	return;
}

//==========================================
// シーケンサからパルスレート変更
//------------------------------------------
// const DBDATA_PULSE_RATE_UPDATE* pWk : 生成情報	
//==========================================
void DBManager::CreateSQL_PulseRateUpdate(const DBDATA_PULSE_RATE_UPDATE* pWk)
{
	CString sql;

	////// 情報
	sql =  "UPDATE ";
	sql += DB_PARAM_COMMON;
	sql += " SET ";
	CreateSQL::sqlColumnUpdate(&sql, "PLGパルスレート", pWk->dPls[0]);
	CreateSQL::sqlColumnUpdate(&sql, "PLGパルスレート比較", pWk->dPls[1]);
	CreateSQL::sqlColumnUpdate(&sql, "PLGパルスレート表示", pWk->dPls[2]);
	CreateSQL::sqlColumnUpdate(&sql, "Vres", pWk->dVres[0]);

	CreateSQL::sqlColumnTrim(&sql);		// 最後のカンマを取り除く

	// Where
	//sql += " WHERE ";
	//CreateSQL::sqlColumnUpdate(&sql, "Key", 0);		// Key(1レコードなので、本来Where句無くても良いはずだが
	//CreateSQL::sqlColumnTrim(&sql);		// 最後のカンマを取り除く

	//// 依頼
	DBSaveManager::DeliverySQL*  pDeli = new DBSaveManager::DeliverySQL();
	pDeli->addmode	= true;
	pDeli->cnt		= 0;
	pDeli->strSql   = sql;

	//// 登録完了後、自タスクにパラメータ変更通知を送信
	//// ホントはこんな手段でやりたくはないが…
	pDeli->SendMailItem = NULL;
	pDeli->SendMailItem = new MAIL_ITEM;
	memset(pDeli->SendMailItem, 0x00, sizeof(MAIL_ITEM));
	strcpy(pDeli->SendMailItem->cTask, TO_DEFECT);
	strcpy(pDeli->SendMailItem->cPc, ".");
	pDeli->SendMailItem->Que.nEventNo = FACT_TO_DEFECT_01;

	if( ! mque_AddDB.AddToTail(pDeli, 20, 20) ) {		// 絶対登録
		// 絶対登録も無理・・・
		LOG(em_ERR), "[%s] DB登録依頼キューフル [DB_PARAM_COMMON]", my_sThreadName);
		syslog(SYSNO_QUEFULL_ERR, "[DB_PARAM_COMMON]");
		delete pDeli->SendMailItem;
		delete pDeli;
	}
}

//==========================================
// 登録状況問い合わせ
//------------------------------------------
// const DBDATA_ADD_SYNC* pWk : 生成情報
//==========================================
void DBManager::Add_Synclock(const DBDATA_ADD_SYNC* pWk)
{
	//// 問い合わせ用の特殊データ
	DBSaveManager::DeliverySQL* pDeli = new DBSaveManager::DeliverySQL();
	pDeli->strSql	= "";
	pDeli->addmode	= true;
	pDeli->bCalcTime= false;
	pDeli->cnt		= 0;

	// メールスロット受信タイミングで、DBキューが一巡したら、返却する
	pDeli->SendMailItem = NULL;
	pDeli->SendMailItem = new MAIL_ITEM;
	memcpy( pDeli->SendMailItem, &pWk->sendItem, sizeof(MAIL_ITEM));	 

	// 依頼
	if( ! mque_AddDB.AddToTail(pDeli, 20, 20) ) {		// 絶対登録
		// 絶対登録も無理・・・
		LOG(em_ERR), "[%s] 登録状況問い合わせ。キューフルの為、問い合わせ取りやめ", my_sThreadName);
		delete pDeli->SendMailItem;
		delete pDeli;
	}
	LOG(em_MSG), "[%s] 登録状況問い合わせ開始 要求元(%s)", my_sThreadName, pWk->sendItem.cTask);
}

//==========================================
// 疵画像登録数リセット
//------------------------------------------
// const DBDATA_COUNT_RESET* pWk : 生成情報
//==========================================
void DBManager::Reset_ImgCount(const DBDATA_COUNT_RESET* pWk)
{
	ResetInf_SaveImgCnt(pWk->cKizukenNo, pWk->nTorB);

	int nRec = GetRecSQLInf(pWk->cKizukenNo);
	if(0 > nRec) nRec = 0;

	LOG(em_MSG), "[%s] 疵画像登録数リセット(%s) <保存> 全数(%d/%d) 出側(%d/%d) <削除> 無害(%d/%d) 有害(%d/%d)", 
		my_sThreadName, pWk->cKizukenNo,
		GetInf_SaveImgAllCnt(nRec, 0), GetInf_SaveImgAllCnt(nRec, 1),
		GetInf_SaveImgCnt(nRec, 0), GetInf_SaveImgCnt(nRec, 1),
		GetInf_ImgLowGrDeleCnt(nRec, 0), GetInf_ImgLowGrDeleCnt(nRec, 1),
		GetInf_ImgDeleCnt(nRec, 0), GetInf_ImgDeleCnt(nRec, 1));
}

//==========================================
// ステータスログ
//------------------------------------------
// const DBDATA_STATUS_LOG* pWk : 生成情報
//==========================================
void DBManager::CreateSQL_StatusLog(const DBDATA_STATUS_LOG* pWk)
{
	CString sql;	
	int ii;

	////// 情報
	// ヘッダー部
	sql = "INSERT INTO ";
	sql += DB_STATUS_LOG;
	sql += " VALUES(";

	// データ部
	CreateSQL::sqlColumnAddTime(&sql, pWk->time, true);		// 登録日付 
	DBMGR_INSERT_V(pWk->nAutoNo);			// 連番

	DBMGR_INSERT_V( (int)pWk->emKadou);
	for(ii=0; ii<NUM_MEN; ii++) DBMGR_INSERT_V( (int)pWk->emKensa[ii]);
	for(ii=0; ii<NUM_MEN; ii++) DBMGR_INSERT_V( (int)pWk->emKiki[ii]);
	
	DBMGR_INSERT_S( pWk->cMsg );
	DBMGR_INSERT_S( pWk->cKizukenNo);
	DBMGR_INSERT_S( pWk->cCoilNo);


	CreateSQL::sqlColumnTrim(&sql);		// 最後のカンマを取り除く
	sql += ")";
	
	//// 依頼
	DBSaveManager::DeliverySQL*  pDeli = new DBSaveManager::DeliverySQL();
	pDeli->addmode	= true;
	pDeli->cnt		= 0;
	pDeli->strSql = sql;
	if( ! mque_AddDB.AddToTail(pDeli, 20, 20) ) {		// 絶対登録
		// 絶対登録も無理・・・
		LOG(em_ERR), "[DBManager] DB登録依頼キューフル [DB_STATUS_LOG]");
		syslog(SYSNO_QUEFULL_ERR, "[DB_STATUS_LOG]");
		delete pDeli;
	}
}
