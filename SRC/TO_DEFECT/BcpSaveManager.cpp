#include "StdAfx.h"
#include "BcpSaveManager.h"
#include <float.h>
#include <limits.h>
// メモリーリーク検出 ----->>>
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
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
BCPSaveManager::BCPSaveManager(void) :
DbBaseManager("BCPMgr")
{
	//// BCP系 初期化
	for(int ii=0; ii<MAX_BCP_INS; ii++){
		// 疵画像
		mcls_pBcpDefImg[ii] = NULL;
		m_pBcpDefImgBuf[ii] = NULL;
		m_nBcpDefSendRowCnt[ii] = 0;

		//// フレーム画像
		//mcls_pBcpFrmImg[ii] = NULL;
		//m_pBcpFrmImgBuf[ii] = NULL;
		//m_nBcpFrmSendRowCnt[ii] = 0;

		// 特徴量
		m_nBcpAttrSendRowCnt[ii] = 0;
		for(int jj=0; jj<MAX_BCP_INS_ATTR; jj++){
			mcls_pBcpAttr[ii][jj] = NULL;
			m_pBcpAttrBuf[ii][jj] = NULL;
		}
	}
	//////// コイル位置情報データ
	//////for (int ii=0; ii<EM_DIV_SYSTEM::DIV_SYSTEM_END; ii++) {
	//////	mcls_pBcpCoilPos[ii] = NULL;
	//////	m_pBcpCoilPosBuf[ii] = NULL;
	//////}
	//////m_nBcpCoilPosSendRowCnt = 0;


	//// 機器状態戻し
	//KizuFunction::SetStatus("BCP_ERR_ID",			true, false);	// 画像書込み失敗
	//KizuFunction::SetStatus("DB_SAVE_MAX_1_ID",		true, false);	// 検査実績保存上限
	//KizuFunction::SetStatus("DB_SAVE_MAX_2_ID",		true, false);	// 検査実績保存上限
	//KizuFunction::SetStatus("DB_SAVE_SKIP_1_ID",	true, false);	// 検査実績保存スキップ
	//KizuFunction::SetStatus("DB_SAVE_SKIP_2_ID",	true, false);	// 検査実績保存スキップ
	//this->SetSaveSkipId(0, GetPrivateProfileInt(INI_STATUS, "DB_SAVE_SKIP_1_ID", 0, TASKINI_NAME) );
	//this->SetSaveSkipId(1, GetPrivateProfileInt(INI_STATUS, "DB_SAVE_SKIP_2_ID", 0, TASKINI_NAME) );

}

//------------------------------------------
// デストラクタ
//------------------------------------------
BCPSaveManager::~BCPSaveManager(void)
{
}



// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// BCP関連

//==========================================
// BCPクラス テーブル紐付け (疵画像)
//==========================================
void BCPSaveManager::BCP_Bind_Defect_Img()
{
	//// 表/裏同一構成なので、同じ領域を使用する。

	// メモリ領域の固定 & 可変長項目のサイズ定義
	for(int ii=0; ii<MAX_BCP_INS; ii++){

		// 領域確保
		m_pBcpDefImgBuf[ii] = new TYP_BCP_DEFECT_IMG;
		for(int jj=0; jj<MAX_IMAGE_CNT; jj++)					m_pBcpDefImgBuf[ii]->img[jj].Alloc(KIZUBCP_MAX_IMGSIZE);	// 可変長対応版 画像領域確保
		for(int jj=MAX_IMAGE_CNT; jj<MAX_HT1_CAMANGLE; jj++)	m_pBcpDefImgBuf[ii]->img[jj].Alloc(0);						// NULL領域


		// DBのテーブルに同期する
		mcls_pBcpDefImg[ii]->BCP_Bind_String	(1, m_pBcpDefImgBuf[ii]->cKizukenNo);
		mcls_pBcpDefImg[ii]->BCP_Bind_Value		(2, (BYTE*)&m_pBcpDefImgBuf[ii]->nKizuNo, BCPBase::em_Int);
		for(int jj=0; jj<MAX_HT1_CAMANGLE; jj++)	mcls_pBcpDefImg[ii]->BCP_Bind_Binary(3+jj, &m_pBcpDefImgBuf[ii]->img[jj], BCPBase::em_VerBinary);

	}
}

//==========================================
// BCPクラス テーブル紐付け (特徴量)
//==========================================
void BCPSaveManager::BCP_Bind_Defect_Attr()
{
	//// 表/裏同一構成なので、同じ領域を使用する。

	// メモリ領域の固定 & 可変長項目のサイズ定義
	for(int ii=0; ii<MAX_BCP_INS; ii++) {
		for(int jj=0; jj<MAX_BCP_INS_ATTR; jj++ ) {
			// 領域確保
			m_pBcpAttrBuf[ii][jj] = new TYP_BCP_DEFECT_ATTR;

			// DBのテーブルに同期する
			mcls_pBcpAttr[ii][jj]->BCP_Bind_String	(1, m_pBcpAttrBuf[ii][jj]->cKizukenNo);
			mcls_pBcpAttr[ii][jj]->BCP_Bind_Value 	(2, (BYTE*)&m_pBcpAttrBuf[ii][jj]->nKizuNo,	BCPBase::em_Int);


			if( 0==jj ) {	// 基本
				for(int kk=0; kk<MAX_ATTR_BASE; kk++)  mcls_pBcpAttr[ii][jj]->BCP_Bind_Value(3+kk, (BYTE*)&m_pBcpAttrBuf[ii][jj]->attr[kk], BCPBase::em_Double);
			} else {		// 角度
				for(int kk=0; kk<MAX_ATTR_ANGLE; kk++) mcls_pBcpAttr[ii][jj]->BCP_Bind_Value(3+kk, (BYTE*)&m_pBcpAttrBuf[ii][jj]->attr[kk], BCPBase::em_Double);
			}
		}
	}
}

////////==========================================
//////// BCPクラス テーブル紐付け (コイル位置情報)
////////==========================================
//////void BCPSaveManager::BCP_Bind_Coil_Position()
//////{
//////	for (int ii=0; ii<EM_DIV_SYSTEM::DIV_SYSTEM_END; ii++) {
//////		// メモリ領域の固定 & 可変長項目のサイズ定義
//////		m_pBcpCoilPosBuf[ii] = new TYP_BCP_COIL_POSITION;
//////
//////		// DBのテーブルに同期する
//////		mcls_pBcpCoilPos[ii]->BCP_Bind_String(1, m_pBcpCoilPosBuf[ii]->CoilPos.cKizukenNo);
//////		mcls_pBcpCoilPos[ii]->BCP_Bind_Value(2, (BYTE*)&m_pBcpCoilPosBuf[ii]->CoilPos.nSpmUmu, BCPBase::em_Int);
//////		mcls_pBcpCoilPos[ii]->BCP_Bind_Value(3, (BYTE*)&m_pBcpCoilPosBuf[ii]->CoilPos.nID, BCPBase::em_Int);
//////		mcls_pBcpCoilPos[ii]->BCP_Bind_Value(4, (BYTE*)&m_pBcpCoilPosBuf[ii]->CoilPos.emKensa[0], BCPBase::em_Int);
//////		mcls_pBcpCoilPos[ii]->BCP_Bind_Value(5, (BYTE*)&m_pBcpCoilPosBuf[ii]->CoilPos.emKensa[1], BCPBase::em_Int);
//////		mcls_pBcpCoilPos[ii]->BCP_Bind_Value(6, (BYTE*)&m_pBcpCoilPosBuf[ii]->CoilPos.emKiki[0], BCPBase::em_Int);
//////		mcls_pBcpCoilPos[ii]->BCP_Bind_Value(7, (BYTE*)&m_pBcpCoilPosBuf[ii]->CoilPos.emKiki[1], BCPBase::em_Int);
//////		mcls_pBcpCoilPos[ii]->BCP_Bind_Value(8, (BYTE*)&m_pBcpCoilPosBuf[ii]->CoilPos.dEdge[0], BCPBase::em_Float);
//////		mcls_pBcpCoilPos[ii]->BCP_Bind_Value(9, (BYTE*)&m_pBcpCoilPosBuf[ii]->CoilPos.dEdge[1], BCPBase::em_Float);
//////		mcls_pBcpCoilPos[ii]->BCP_Bind_Value(10, (BYTE*)&m_pBcpCoilPosBuf[ii]->CoilPos.dWalk, BCPBase::em_Float);
//////	}
//////}

//==========================================
// BCP状態を変更
//------------------------------------------
// bool		bMode		: BCP状態変更
//==========================================
void BCPSaveManager::CheckBcpStatus(bool bMode)
{
	if( ! bMode ) {
		m_bBcpState = false;

		LOG(em_MSG), "[%s] BCP 状態に異常発生", my_sThreadName);
		mcls_pStatus->SetStatusQue("BCP_ERR_ID", false);


		//// BCP再接続
		OnFree();			// 解放
		OnAlloc(false);		// 接続


		// ちょっとだけ次のキューを処理されるのを待つ
		Sleep(50);


	////////////////////////////////
	}else if( !m_bBcpState && bMode ) {
		m_bBcpState = true;
		LOG(em_INF), "[%s] BCP 状態が正常復旧", my_sThreadName);
		mcls_pStatus->SetStatusQue("BCP_ERR_ID", true);
	}
}



// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 継承元からコールバック

//==========================================
// 関数初期化
//------------------------------------------
// bool		bMode	: BCP状態変更
//==========================================
void BCPSaveManager::OnAlloc(bool bMode)
{
	//// BCPクラス生成
	if(bMode) m_bBcpState = true;

	//------------------------------------
	//// 疵画像
	for(int ii=0; ii<MAX_BCP_INS; ii++){
		mcls_pBcpDefImg[ii] = new BCPBase();
		CString strTable = (0==ii ? DB_DEFECT_IMG_T : DB_DEFECT_IMG_B);
		BCP_Connect(mcls_pBcpDefImg[ii], strTable);
	}
	BCP_Bind_Defect_Img();	// バッファを紐つけ

	
	//// 特徴量
	char Tbl[MAX_BCP_INS][MAX_BCP_INS_ATTR][32] = 
#ifdef HT_IS_COLOR
		{	{DB_DEFECT_ATTR0_T, DB_DEFECT_ATTR1_T, DB_DEFECT_ATTR2_T, DB_DEFECT_ATTR3_T, DB_DEFECT_ATTR4_T},
			{DB_DEFECT_ATTR0_B, DB_DEFECT_ATTR1_B, DB_DEFECT_ATTR2_B, DB_DEFECT_ATTR3_B, DB_DEFECT_ATTR4_B} };
#else
		{	{DB_DEFECT_ATTR0_T, DB_DEFECT_ATTR1_T, DB_DEFECT_ATTR2_T},
			{DB_DEFECT_ATTR0_B, DB_DEFECT_ATTR1_B, DB_DEFECT_ATTR2_B} };
#endif
	for(int ii=0; ii<MAX_BCP_INS; ii++){
		for(int jj=0; jj<MAX_BCP_INS_ATTR; jj++){
			mcls_pBcpAttr[ii][jj] = new BCPBase();
			CString strTable = Tbl[ii][jj];
			BCP_Connect(mcls_pBcpAttr[ii][jj], strTable);
		}
	}
	BCP_Bind_Defect_Attr();	// バッファ紐つけ

	////////// コイル位置情報
	//////CString strTable[EM_DIV_SYSTEM::DIV_SYSTEM_END] = { DB_COIL_POSITION, DB_COIL_POSITION_OUT, DB_COIL_POSITION_ANA };
	//////for (int ii = 0; ii<EM_DIV_SYSTEM::DIV_SYSTEM_END; ii++) {
	//////	mcls_pBcpCoilPos[ii] = new BCPBase();
	//////	BCP_Connect(mcls_pBcpCoilPos[ii], strTable[ii]);
	//////}
	//////BCP_Bind_Coil_Position();

}

//==========================================
// 関数開放
//==========================================
void BCPSaveManager::OnFree()
{
	//// BCPクラス開放
	// 疵画像
	for(int ii=0; ii<MAX_BCP_INS; ii++){
		if(NULL == mcls_pBcpDefImg[ii]) continue;
		BCP_Dispose( mcls_pBcpDefImg[ii] );
		SAFE_DELETE( mcls_pBcpDefImg[ii] );
		SAFE_DELETE( m_pBcpDefImgBuf[ii] );
		m_nBcpDefSendRowCnt[ii] = 0;
	}

	// 特徴量
	for(int ii=0; ii<MAX_BCP_INS; ii++){
		for(int jj=0; jj<MAX_BCP_INS_ATTR; jj++){
			if(NULL == mcls_pBcpAttr[ii][jj]) continue;
			BCP_Dispose( mcls_pBcpAttr[ii][jj] );
			SAFE_DELETE( mcls_pBcpAttr[ii][jj] );
			SAFE_DELETE( m_pBcpAttrBuf[ii][jj] );
		}
		m_nBcpAttrSendRowCnt[ii] = 0;
	}

	//////// コイル位置情報
	//////for (int ii = 0; ii < EM_DIV_SYSTEM::DIV_SYSTEM_END; ii++) {
	//////	if (NULL != mcls_pBcpCoilPos[ii]) {
	//////		BCP_Dispose(mcls_pBcpCoilPos[ii]);
	//////		SAFE_DELETE(mcls_pBcpCoilPos[ii]);
	//////		SAFE_DELETE(m_pBcpCoilPosBuf[ii]);
	//////		m_nBcpCoilPosSendRowCnt = 0;
	//////	}
	//////}

}


//==========================================
// BCP自動コミット用
//------------------------------------------
// bool		bAuto		: 自動コミット時true	
//==========================================
void BCPSaveManager::OnBcpAutoCommit(bool bAuto)
{
	// ずっとコミットされずにいるBCP_SendRowしたデータを確定させる (定修時に効果的)

	for(int ii=0; ii<MAX_BCP_INS; ii++){

		// 疵画像
		if( 0 != m_nBcpDefSendRowCnt[ii] ){
			if(bAuto) { LOG(em_MSG), "[BcpSaveManager] BCP 自動コミット <%s>",  (0 ==ii?DB_DEFECT_IMG_T:DB_DEFECT_IMG_B)); }

			BCP_Batch(mcls_pBcpDefImg[ii]);

			m_nBcpDefSendRowCnt[ii] = 0;

		}

		// 特徴量
		if( 0 != m_nBcpAttrSendRowCnt[ii] ){
			for(int jj=0; jj<MAX_BCP_INS_ATTR; jj++){
				BCP_Batch(mcls_pBcpAttr[ii][jj]);
			}

			m_nBcpAttrSendRowCnt[ii] = 0;
		}
	}

	//////// コイル位置情報
	//////if (0 != m_nBcpCoilPosSendRowCnt) {
	//////	if (bAuto) { LOG(em_MSG), "[%s] BCP 自動コミット <%s>", my_sThreadName, DB_COIL_POSITION); }

	//////	for (int ii = 0; ii < EM_DIV_SYSTEM::DIV_SYSTEM_END; ii++)
	//////		BCP_Batch(mcls_pBcpCoilPos[ii]);

	//////	m_nBcpCoilPosSendRowCnt = 0;
	//////}

}

//==========================================
// BCP状態チェック(定周期タイマーイベント)
// ※ DB異常でBCPが切れた場合、従来だと次疵発生まで復旧しなかった対応
//==========================================
void BCPSaveManager::OnBcpStatusCheck()
{
	//// 再復旧を試みる
	if( ! m_bBcpState ) {
		bool bMode = OdbcBase::OpenCheck(TASKINI_NAME, INI_DB);
		// 正常復帰した場合に再接続させる
		CheckBcpStatus(bMode);
	}
}

//==========================================
// DB登録依頼
//------------------------------------------
// int		emNo	: ENUM_KUBUNの番号
// BYTE*	pData	: データ部 (継承元で開放処理を行うのでここでは、開放する必要なし)
//==========================================
void BCPSaveManager::OnQueDb(int emNo, BYTE* pData)
{
	//// 区分により処理分け
	switch(emNo) {
		//-----------------------------------------------------------
		case em_DEFECTINF:												// 疵情報
#ifndef NON_DB_SAVE
			CreateSQL_DefectInf( (const DBDATA_DEFECT_INF*)pData );
#endif
			// この為だけに渡された画像領域を開放
			for(int ii=0; ii<MAX_HT1_CAMANGLE; ii++) { SAFE_DELETE_ARRAY( ((DBDATA_DEFECT_INF*)pData)->pImg[ii] ); }
			break;

		//-----------------------------------------------------------
		case em_BCP_BATCH:												// BCP batch強制実行
			BcpBatchExec((const DBDATA_BCP_BATCH_EXEC*)pData);
			break;

		// <<<------------------------------

	}
}


//==========================================
// リセット依頼
//==========================================
void BCPSaveManager::OnReSetMaxCount()
{
	//// 時間計測準備
	mcls_AttrSendRowTime.InitAve();
	mcls_AttrBcpBatchTime.InitAve();
	mcls_ImgSendRowTime.InitAve();
	mcls_ImgBcpBatchTime.InitAve();
}

// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// SQL生成関連

//==========================================
// 疵情報
//------------------------------------------
// const DBDATA_DEFECT_INF* pWk	: 生成情報
//==========================================
void BCPSaveManager::CreateSQL_DefectInf(const DBDATA_DEFECT_INF* pWk)
{
	int  ou;							// 表裏区分[0:表 / 1:裏]
	int	 nJpegSize[MAX_HT1_CAMANGLE];	// Bitmap→Jpegに変換した画像サイズ

	//==================================================
	// 事前準備
	ou		= pWk->data.TorB;


	this->mcls_Time.CounterStart();		// 処理時間計測開始
	
	//==================================================
	// 基本 特徴量 (BCP書込み)
	{
		// ヘッダー
		for(int ii=0; ii<MAX_BCP_INS_ATTR; ii++){
			memcpy(m_pBcpAttrBuf[ou][ii]->cKizukenNo, pWk->cKizukenNo, sizeof(pWk->cKizukenNo));	// 管理No
			m_pBcpAttrBuf[ou][ii]->nKizuNo = pWk->nKizuNo;												// 疵No
		}
		// 基本
		for(int ii=0; ii<MAX_ATTR_BASE; ii++){
			m_pBcpAttrBuf[ou][0]->attr[ii] = filter_nan_inf(pWk->attr_base[ii]);
		}
		// 角度
		for(int jj=0; jj<MAX_BCP_INS_ATTR-1; jj++) {
			for(int ii=0; ii<MAX_ATTR_ANGLE; ii++) { 
				m_pBcpAttrBuf[ou][jj+1]->attr[ii] = filter_nan_inf(pWk->attr_angle[jj][ii]);
			}
		}
		// 転送
		mcls_AttrSendRowTime.CounterStart();		// 処理時間計測開始
		for(int jj=0; jj<MAX_BCP_INS_ATTR; jj++) {
			bool bWk = BCP_SendRow( mcls_pBcpAttr[ou][jj] );
			CheckBcpStatus(bWk);										// BCPの結果を処理させる
			if(!bWk) OnBcpStatusCheck();								// 再復旧を試みる
#ifdef DEBUG_HYPER_V_TEST
			if (0 == jj) break;					// ★開発環境ではDBロックされるため
#endif
		}
		mcls_AttrSendRowTime.CounterEnd();		// 処理時間計測終了

		//// ある程度まとまって書き込み (これがホントの書込み)
		m_nBcpAttrSendRowCnt[ou] += 1;
		mcls_AttrBcpBatchTime.CounterStart();		// 処理時間計測開始
		if( m_nBcpAttrSendRowCnt[ou] >= MAX_BCP_BATCH_COUNT_ATTR ) {	
			for(int jj=0; jj<MAX_BCP_INS_ATTR; jj++) {
				BCP_Batch(mcls_pBcpAttr[ou][jj]);
			}
			m_nBcpAttrSendRowCnt[ou] = 0;
		}
		mcls_AttrBcpBatchTime.CounterEnd();		// 処理時間計測終了
	}


	//======================================================
	// 疵画像 (BCP書込み)

	//// 情報セット
	m_pBcpDefImgBuf[ou]->cKizukenNo[0] = 0x00;
	m_pBcpDefImgBuf[ou]->nKizuNo = 0;
	for(int ii=0; ii<MAX_HT1_CAMANGLE; ii++) m_pBcpDefImgBuf[ou]->img[ii].SetNullData();		// 未セットはNULL画像を登録するように
	memset(nJpegSize, 0x00, sizeof(nJpegSize));

	if( ! pWk->bImgSkip) {
		if( pWk->isBmp ) {
			// Jpegに変換と共に、BCPエリアに正・乱画像データセット
			//    今回セットした奴以外は、定義ですべてNULLとなるので気をつける事（ホントはここでNULLを登録したかったが・・・やり方が不明）
			for(int ii=0; ii<MAX_IMAGE_CNT; ii++) {
				// 画像なし疵対応
				if (0 == pWk->nImgsize[ii]) continue;	// 画像サイズが0の場合はバイナリセットしない
#ifndef JPEGLIB_LJT
				KizuJpegManager::BmpToJpeg( pWk->data.w_size, pWk->data.h_size, pWk->pImg[ii], m_nJpgQuality, &nJpegSize[ii], m_pBcpDefImgBuf[ou]->img[ii].pData);
#else
				mcls_KizuJpegManager.BmpToJpeg(pWk->data.w_size, pWk->data.h_size, pWk->pImg[ii], m_nJpgQuality, &nJpegSize[ii], m_pBcpDefImgBuf[ou]->img[ii].pData);
#endif
				m_pBcpDefImgBuf[ou]->img[ii].SetSize( nJpegSize[ii] );
			}
		} else {
			// すでにJpeg画像の為 BCPエリアに画像をコピーするだけ
			for(int ii=0; ii<MAX_IMAGE_CNT; ii++) {
				// 画像なし疵対応
				if (0 == pWk->nImgsize[ii]) continue;	// 画像サイズが0の場合はバイナリセットしない
				memcpy(m_pBcpDefImgBuf[ou]->img[ii].pData, pWk->pImg[ii], pWk->nImgsize[ii]);
				nJpegSize[ii] = pWk->nImgsize[ii];
				m_pBcpDefImgBuf[ou]->img[ii].SetSize( nJpegSize[ii] );
				BCP_ColLen( mcls_pBcpDefImg[ou], 3+ii, nJpegSize[ii] );	// varbinary(MAX) に定義変更してから、0xFFFFまでしか登録できない暫定対策
			}
		}


		// その他 画像情報 項目セット
		memcpy(m_pBcpDefImgBuf[ou]->cKizukenNo, pWk->cKizukenNo, sizeof(pWk->cKizukenNo));		// 管理No
		//m_pBcpDefImgBuf[ou]->nKizuNo = pWk->data.nKizuNo;										// 疵No
		m_pBcpDefImgBuf[ou]->nKizuNo = pWk->nKizuNo;											// 疵No
		//KizuODBC::BCPBase::Get_BPC_Time( m_pBcpDefImgBuf[nBcpInc]->cAddTime );						// 登録日付の付与

		
		//// BCP 登録
		// データ転送 (これだけでは、書き込まれないので注意)
		mcls_ImgSendRowTime.CounterStart();		// 処理時間計測開始
		bool bWk = BCP_SendRow( mcls_pBcpDefImg[ou] );
		mcls_ImgSendRowTime.CounterEnd();		// 処理時間計測終了
		CheckBcpStatus(bWk);										// BCPの結果を処理させる
		if(!bWk) OnBcpStatusCheck();								// 再復旧を試みる

				
		//// ある程度まとまって書き込み (これがホントの書込み)
		m_nBcpDefSendRowCnt[ou] += 1;
		if( m_nBcpDefSendRowCnt[ou] >= MAX_BCP_BATCH_COUNT_DEF ) {
			mcls_ImgBcpBatchTime.CounterStart();		// 処理時間計測開始
			BCP_Batch(mcls_pBcpDefImg[ou]);
			mcls_ImgSendRowTime.CounterEnd();		// 処理時間計測終了
			m_nBcpDefSendRowCnt[ou] = 0;
		}
	}

	//// 自動コミット用
	this->BCP_AutoCommitTimerOn();

	// 処理時間計測終了
	mcls_Time.CounterEnd();
}

//==========================================
// BCP batch 強制実行
//==========================================
void BCPSaveManager::BcpBatchExec(const DBDATA_BCP_BATCH_EXEC* pWk)
{
	OnBcpAutoCommit(pWk->bMode);
}

