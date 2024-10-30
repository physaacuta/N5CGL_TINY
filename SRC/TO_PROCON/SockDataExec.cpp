#include "StdAfx.h"
#include "SockDataExec.h"

//#define tDEBUG

// メモリーリーク検出 ----->>>
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif
// <<<------------------------

//------------------------------------------
// コンストラクタ
//------------------------------------------
SockDataExec::SockDataExec(void) :
mque_AddDB(64)
{

///*/----> ◆オンライン時は必ずコメント化すること
	m_hCoil = CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, sizeof(TO_COIL_TBL), TBL_TO_COIL_NAME);
	mtbl_pCoil =(TO_COIL_TBL*) MapViewOfFile(m_hCoil, FILE_MAP_ALL_ACCESS, 0, 0, 0);
//*///<<---

#ifndef tDEBUG			// デバック時は、無処理
	// コイル情報テーブル
	mem_connect(&m_hCoil, (LPVOID *)&mtbl_pCoil, TBL_TO_COIL_NAME);
	if(m_hCoil == NULL) {
		syslog(SYSNO_MEM_OPEN, "コイル情報テーブル アクセス失敗");
		_ASSERT(FALSE);
	}
	memset(mtbl_pCoil, 0x00, sizeof(TO_COIL_TBL));

#endif


}

//------------------------------------------
// デストラクタ
//------------------------------------------
SockDataExec::~SockDataExec(void)
{
#ifndef tDEBUG			// デバック時は、無処理
	//// 共有メモリ開放
	mem_close(&m_hCoil, (LPVOID *)&mtbl_pCoil);							// コイル情報
#endif


}

//------------------------------------------
// 関数初期化
//------------------------------------------
void SockDataExec::Alloc()
{
#ifndef tDEBUG			// デバック時は、無処理
	//// DB登録クラス生成
	mcls_pDbs = new DBSaveManager(0, &mque_AddDB);
	mcls_pDbs->SetLogMgr(mcls_pLog);
	mcls_pDbs->SetSendDbManager(false);
	mcls_pDbs->Start();
#endif
}

//------------------------------------------
// 関数開放
//------------------------------------------
void SockDataExec::Free()
{
#ifndef tDEBUG			// デバック時は、無処理
	//// DB登録クラス解放
	mcls_pDbs->Stop(3000);
	delete mcls_pDbs;
#endif
}

//------------------------------------------
// 送信データ取得 (この中で、バイト反転しちゃうよ)
// Delivery* deli 要求情報
// int* nTc トランザクションコード
// int* nSize 送信データ長
// BYTE* data 送信データ
//------------------------------------------
bool SockDataExec::AddQue_Ziseki(COMMON_QUE const* pQue)
{
	// 送信要求 キューに登録
	Delivery* deli = new Delivery;
	deli->pData = (BYTE*) new PRO_TYPE_ZISEKI;
	deli->eStat = QUE_ZISEKI_SEND;
	strcpy(deli->cKey, pQue->fc.data);

	CString		cSql;

	//// 実績伝文生成
	PRO_TYPE_ZISEKI* buf = (PRO_TYPE_ZISEKI*)deli->pData;

	//// DBから実績データを取得
#ifndef tDEBUG											//// 本番データ
	// SQL文作成
	cSql =  "SELECT 生伝文 ";
	cSql += " FROM ";
	cSql += DB_BUF_PRO_S_ZISEKI;
	cSql += " WHERE ID=";
	CreateSQL::sqlColumnInsert(&cSql, deli->cKey, sizeof(deli->cKey));		// ID
	CreateSQL::sqlColumnTrim(&cSql);
	// 取得
	if( ! OdbcBase::GetSelectImage_Static(cSql, sizeof(PRO_TYPE_ZISEKI), (char*)buf, TASKINI_NAME, INI_DB ) ) {
		// 取得失敗
		LOG(em_ERR), "[%s] 検査実績伝文 DB取得失敗 [ID=%s]", my_sThreadName, deli->cKey);
		LOG(em_NON), "%s", cSql);
		SAFE_DELETE(deli->pData);
		SAFE_DELETE(deli);
		return false;
	}
#else													//// テスト用ダミーデータ
	memset(buf, 0x20, sizeof(PRO_TYPE_ZISEKI));
	memset(buf->data.grade_inf, 0x00, sizeof(buf->data.grade_inf) );

	buf->head.bHead[0] = gcPRO_HEAD_H_GYOUMU;
	buf->head.bHead[1] = gcPRO_HEAD_L_OK;
	buf->head.nLen = PRO_TYPE_LENGTH(sizeof(PRO_TYPE_ZISEKI));

	buf->head.nTC = PRO_TC_ZISEKI;
	buf->head.nKubun = PRO_KUBUN_NOMAL;
	buf->head.nDate[0] = 2012;
	buf->head.nDate[1] = 3;
	buf->head.nDate[2] = 23;
	buf->head.nDate[3] = 9;
	buf->head.nDate[4] = 30;
	buf->head.nDate[5] = 59;
	buf->head.nGrandFlg = 0;
	buf->data.nZiMode = 0;
	buf->data.nKikiMode = 0;
	//memcpy(buf->data.cMeiNo, "0123", 4);
	//memcpy(buf->data.cCoilNo, "1-2345      ", 12);
	//buf->data.nKoban = 1;
		
	memcpy(buf->data.cMeiNo, &mtyp_Cut.data.cMeiNo[0], sizeof(buf->data.cMeiNo));		// 命令No
	memcpy(buf->data.cCoilNo, &mtyp_Cut.data.cCoilNo[0], sizeof(buf->data.cCoilNo));	// コイルNo
	buf->data.nKoban = mtyp_Cut.data.nKoban;									// 子番

	for(int ii=0; ii<6; ii++ ) {
		buf->data.grade_inf[ii].nMixedRate = (ii+1)*100; 
		buf->data.grade_inf[ii].nTypeCnt[0] = 1;
		buf->data.grade_inf[ii].nTypeCnt[1] = 2; 
		buf->data.grade_inf[ii].nTypeCnt[2] = 3; 
		buf->data.grade_inf[ii].nTypeCnt[3] = 4; 
		buf->data.grade_inf[ii].nTypeCnt[4] = 5; 
		buf->data.grade_inf[ii].nTypeCnt[5] = 6; 
		buf->data.grade_inf[ii].nTypeCnt[6] = 7; 
		buf->data.grade_inf[ii].nTypeCnt[7] = 8; 
		buf->data.grade_inf[ii].nTypeCnt[8] = 9; 
		buf->data.grade_inf[ii].nTypeCnt[9] = 10;
	}
#endif

	//// 送信データ 編集
	//*size = buf->head.nLen;
	deli->nSize = sizeof(PRO_TYPE_ZISEKI);
	deli->nTc	= buf->head.nTC;

	CString		cDumpName;
#ifdef BYTE_HANTEN
	// 生データダンプ
	cDumpName.Format("%s 送信(生)ダンプ %s(%s, EvtNo=%d).log", TO_PROCON, "[送信ポート]", "業務伝文", deli->nTc);
	KizuFunction::DumpData( cDumpName, deli->nSize, (BYTE *)buf, m_bDumpMode);

	// ------------------------------------
	// プロコン送信伝文 バイト反転
	// ------------------------------------
	ByteHantenProDataSend(deli->nTc, (BYTE *)buf);
#endif

	// 変データダンプ
	cDumpName.Format("%s 送信(変)ダンプ %s(%s, EvtNo=%d).log", TO_PROCON, "[送信ポート]", "業務伝文", deli->nTc);
	KizuFunction::DumpData(cDumpName, deli->nSize, (BYTE *)buf, m_bDumpMode);

	//================================================
	// 送信
	//	LOG(em_MSG), "[%s] 検査実績伝文送信依頼", my_sThreadName);
	//// 送信依頼
	if (!mque_pDeli->AddToTailFreeCheck(deli, 0)) {
		LOG(em_ERR), "[%s] 送信依頼異常キューフル", my_sThreadName);
		syslog(SYSNO_QUEFULL_WAR, "[送信依頼]");
		SAFE_DELETE(deli->pData);
		SAFE_DELETE(deli);
		return false;
	}
	return true;
}
//------------------------------------------
// 実績送信結果
// Delivery* deli 要求情報
// bool bAns 送信結果
//------------------------------------------
bool SockDataExec::SetSendDataAnser(Delivery* deli, bool bAns)
{
#ifdef tDEBUG			// デバック時は、無処理
	return true;
#endif

	int imode = bAns == true ? 0 : -1;


	// SQL文生成
	CString		sql;
	sql = "UPDATE ";
	sql += DB_BUF_PRO_S_ZISEKI;
	sql += " SET ";
	CreateSQL::sqlColumnUpdate(&sql, "伝文区分",imode); 
	sql += "送信日付=";	CreateSQL::sqlColumnAddTime(&sql); 
	sql += " WHERE ID=";
	CreateSQL::sqlColumnInsert(&sql, deli->cKey, sizeof(deli->cKey));		// ID
	CreateSQL::sqlColumnTrim(&sql);

	//// 生成したSQL文を DB登録スレッドに登録依頼する
	DBSaveManager::DeliverySQL*  pDeli;
	pDeli = new DBSaveManager::DeliverySQL();
	pDeli->addmode	= true;
	pDeli->strSql	= sql;
	pDeli->cnt		= 0;
	if( ! mque_AddDB.AddToTail(pDeli, 20, 20) ) {		// 絶対登録
		// 絶対登録も無理・・・
		LOG(em_ERR), "[%s] DB登録依頼キューフル [%s]", my_sThreadName, DB_BUF_PRO_S_ZISEKI);
		syslog(SYSNO_QUEFULL_ERR, "[%s]", DB_BUF_PRO_S_ZISEKI);
		SAFE_DELETE( pDeli );
	}
	else
	{
		LOG(em_MSG), "[%s] 実績送信結果 DB登録完了", my_sThreadName);
	}
	return true;
}	

//------------------------------------------
// 受信データ 事後処理
// int nTc トランザクションコード
// int nSize 受信データ長
// BYTE* data 受信データ
//------------------------------------------
bool SockDataExec::SetRecvData(int nTc, int nSize, BYTE * data)
{
	COMMON_QUE		que;
	bool bRetc = true;


	memset(&que, 0x00, sizeof(que));
	que.nLineNo = getlineid();
	
	CString			sql;
	char			cKey[SIZE_SERIAL_FF_NO];		// ミリ秒単位の主キー
	KizuFunction::GetSerialNoFF(cKey); 

	// ------------------------------------
	// プロコン受信伝文 バイト反転
	// ------------------------------------
#ifdef BYTE_HANTEN
	ByteHantenProDataRecv(nTc, data);
#endif

	//// 各伝文生成
	switch(nTc) {
	//---------------------------------------------------------
	case PRO_TC_INIT:								// イニシャル伝文
		if(true) {
			//// 伝文生成
			PRO_TYPE_INIT* buf = (PRO_TYPE_INIT*)data;

			syslog(329, "");

			// 統括管理へ通知
			que.nEventNo = FACT_TO_DEFECT_15;
			send_mail(TO_DEFECT, ".", &que);
		}
		break;

	//---------------------------------------------------------
	case PRO_TC_COIL:								// コイル情報
		if(true) {
			//// 伝文生成
			PRO_TYPE_COILINF* buf = (PRO_TYPE_COILINF*)data;

			int nAlmNo = (1 == buf->data.CoilInf.nSendPoint) ? 325 : 326;
			syslog(nAlmNo, "[%s] [%.12s] 送信位置=%d", cKey, buf->data.CoilInf.KeyInf.cCoilNo, buf->data.CoilInf.nSendPoint);
			LOG(em_MSG), "[%s] [%s] [%.12s] 送信位置=%d", my_sThreadName, cKey, buf->data.CoilInf.KeyInf.cCoilNo, buf->data.CoilInf.nSendPoint);

#ifndef tDEBUG			// デバック時は、無処理
			//// ------------------------------
			//// 送信位置         TBL_TO_COIL
			////  1 : 検査位置 →  Record : 0
			////  2 : 表示位置 →  Record : 1
			//// ------------------------------
			//// 共有メモリに書き込み
			int nRec = buf->data.CoilInf.nSendPoint - 1;
			if(nRec < 0 || nRec >= 2 ) {
				LOG(em_ERR), "[%s] コイル情報伝文 送信位置異常 [%d]", my_sThreadName, nRec);
				return false;
			}
			memset( &mtbl_pCoil->rec[nRec], 0x00, sizeof(mtbl_pCoil->rec[nRec]));
			memcpy( mtbl_pCoil->rec[nRec].val, buf, nSize);
#endif

			//// 統括管理へ通知
			que.nEventNo = FACT_TO_GETDAT_02;
			que.fl.data[0] = nRec;				// 受信位置(共有メモリ格納位置)
			send_mail(TO_GETDAT, ".", &que);

			// BUFテーブル登録
			InsertBufProRData(nTc, data);

		}
		break;


	//---------------------------------------------------------
	case PRO_TC_SAISUN:								// 採寸実績
		if(true) {
			//// 伝文生成
			PRO_TYPE_SAISUN* buf = (PRO_TYPE_SAISUN*)data;

			syslog(327, "[%s] [%.12s]", cKey, buf->data.KeyInf.cCoilNo);

			// BUFテーブル登録
			InsertBufProRData(nTc, data);
		}
		break;

	//---------------------------------------------------------
	case PRO_TC_CUT:								// カット実績伝文
		if(true) {
			//// 伝文生成
			PRO_TYPE_CUT* buf = (PRO_TYPE_CUT*)data;

			syslog(328, "[%s] [%.12s]", cKey, buf->data.KeyInf.cCoilNo);

			// BUFテーブル登録
			InsertBufProRData(nTc, data);

#ifdef tDEBUG			// デバック時のみ
			
			// カット情報保存
			memset(&mtyp_Cut, 0x00, sizeof(PRO_TYPE_CUT));
			memcpy(&mtyp_Cut, buf, sizeof(mtyp_Cut));

			Sleep(10000);
			memset(&que, 0x00, sizeof(que));
			que.nEventNo = FACT_TO_PROCON_01;
			que.nLineNo = getlineid();
			send_mail(TO_PROCON, ".", &que);
			
#endif

		}
		break;
	//---------------------------------------------------------
	case PRO_TC_HELS:								// ヘルシー伝文
		if (true) {
			// ヘルシー伝文監視 開始
			SetHelcy();
		}
		break;

	//---------------------------------------------------------
	default:
		LOG(em_ERR), "[%s] TC異常 [%d]", my_sThreadName, nTc);
		syslog(324, "[受信伝文:TC異常=%d]", nTc);
		bRetc = false;
		break;
	}


	// 変データダンプ
	CString		cDumpName;
	cDumpName.Format("%s 受信(変)ダンプ %s(%s, EvtNo=%d).log", TO_PROCON, "[受信ポート]", "業務伝文", nTc );
	KizuFunction::DumpData(cDumpName, nSize, data, m_bDumpMode);

	return bRetc;
}

//------------------------------------------
// プロコン受信伝文 バイト反転
// int nTc トランザクションコード
// BYTE* data 受信データ
//------------------------------------------
void SockDataExec::ByteHantenProDataRecv(int nTc, BYTE * data)
{
#ifdef BYTE_HANTEN

	// ヘッダー部
	PRO_TYPE_HEAD *pHead = (PRO_TYPE_HEAD *)data;
	KizuFunction::ByteHanten2(2 * 1, (BYTE*)&pHead->nLen);
	KizuFunction::ByteHanten4(4 * 1, (BYTE*)&pHead->nTC);

	switch (nTc)
	{
		//-----------------------------------------------------------
	case PRO_TC_COIL:				// コイル情報伝文
		if (true)
		{
			// データ部
			PRO_TYPE_COILINF *pBuf = (PRO_TYPE_COILINF *)data;
			PRO_TYPE_DATA_COILINF *pData = (PRO_TYPE_DATA_COILINF *)&pBuf->data;
			KizuFunction::ByteHanten2(2 * 1, (BYTE*)&pData->Common.nKubun);
			KizuFunction::ByteHanten2(2 * 6, (BYTE*)&pData->Common.nDate);
			KizuFunction::ByteHanten2(2 * 1, (BYTE*)&pData->Common.nGrandFlg);

			KizuFunction::ByteHanten4(4 * 1, (BYTE*)&pData->CoilInf.nSendPoint);
			KizuFunction::ByteHanten4(4 * 1, (BYTE*)&pData->CoilInf.nLenIn);
			KizuFunction::ByteHanten4(4 * 1, (BYTE*)&pData->CoilInf.nAtuZai);
			KizuFunction::ByteHanten4(4 * 1, (BYTE*)&pData->CoilInf.nWidZai);
			KizuFunction::ByteHanten4(4 * 1, (BYTE*)&pData->CoilInf.nAtuSei);
			KizuFunction::ByteHanten4(4 * 1, (BYTE*)&pData->CoilInf.nWidSei);
			KizuFunction::ByteHanten4(4 * 1, (BYTE*)&pData->CoilInf.nWidTrim);
			KizuFunction::ByteHanten4(4 * 1, (BYTE*)&pData->CoilInf.nNobiSPM);
			KizuFunction::ByteHanten4(4 * 1, (BYTE*)&pData->CoilInf.nNobiTLV);
			KizuFunction::ByteHanten4(4 * 1, (BYTE*)&pData->CoilInf.nCutF);
			KizuFunction::ByteHanten4(4 * 1, (BYTE*)&pData->CoilInf.nCutT);
			KizuFunction::ByteHanten4(4 * 1, (BYTE*)&pData->CoilInf.nSPMRoolKei_Up);
			KizuFunction::ByteHanten4(4 * 1, (BYTE*)&pData->CoilInf.nSPMRoolKei_Down);
			KizuFunction::ByteHanten4(4 * 31, (BYTE*)&pData->CoilInf.nYobi);
			for (int ii = 0; ii < PRO_SIZE_NEXTINF_CNT; ii++)
			{
				KizuFunction::ByteHanten4(4 * 1, (BYTE*)&pData->next_coil[ii].nFrontLen);
			}
		}
		break;

		//-----------------------------------------------------------
	case PRO_TC_SAISUN:				// 採寸実績伝文
		if (true)
		{
			// データ部
			PRO_TYPE_SAISUN *pBuf = (PRO_TYPE_SAISUN *)data;
			PRO_TYPE_DATA_SAISUN *pData = (PRO_TYPE_DATA_SAISUN *)&pBuf->data;
			KizuFunction::ByteHanten2(2 * 1, (BYTE*)&pData->Common.nKubun);
			KizuFunction::ByteHanten2(2 * 6, (BYTE*)&pData->Common.nDate);
			KizuFunction::ByteHanten2(2 * 1, (BYTE*)&pData->Common.nGrandFlg);

			KizuFunction::ByteHanten4(4 * 1, (BYTE*)&pData->nLen);
			KizuFunction::ByteHanten4(4 * 1, (BYTE*)&pData->nMakiLenF);
			KizuFunction::ByteHanten4(4 * 1, (BYTE*)&pData->nSetCnt);
			KizuFunction::ByteHanten4(4 * 9, (BYTE*)&pData->nYobi);
			for (int ii = 0; ii < PRO_MAX_CUT_COUNT; ii++)
			{
				KizuFunction::ByteHanten4(4 * 1, (BYTE*)&pData->saisun[ii].nStart);
				KizuFunction::ByteHanten4(4 * 1, (BYTE*)&pData->saisun[ii].nEnd);
			}
		}
		break;

		//-----------------------------------------------------------
	case PRO_TC_CUT:				// カット実績伝文
		if (true)
		{
			// データ部
			PRO_TYPE_CUT *pBuf = (PRO_TYPE_CUT *)data;
			PRO_TYPE_DATA_CUT *pData = (PRO_TYPE_DATA_CUT *)&pBuf->data;
			KizuFunction::ByteHanten2(2 * 1, (BYTE*)&pData->Common.nKubun);
			KizuFunction::ByteHanten2(2 * 6, (BYTE*)&pData->Common.nDate);
			KizuFunction::ByteHanten2(2 * 1, (BYTE*)&pData->Common.nGrandFlg);

			KizuFunction::ByteHanten4(4 * 1, (BYTE*)&pData->nMode);
			KizuFunction::ByteHanten4(4 * 1, (BYTE*)&pData->nStart);
			KizuFunction::ByteHanten4(4 * 1, (BYTE*)&pData->nEnd);
			KizuFunction::ByteHanten4(4 * 1, (BYTE*)&pData->nKoban);
		}
		break;

		//-----------------------------------------------------------
	case PRO_TC_INIT:				// イニシャル伝文
		// データ部
		PRO_TYPE_INIT *pBuf = (PRO_TYPE_INIT *)data;
		PRO_TYPE_DATA_INIT *pData = (PRO_TYPE_DATA_INIT *)&pBuf->data;
		KizuFunction::ByteHanten2(2 * 1, (BYTE*)&pData->Common.nKubun);
		KizuFunction::ByteHanten2(2 * 6, (BYTE*)&pData->Common.nDate);
		KizuFunction::ByteHanten2(2 * 1, (BYTE*)&pData->Common.nGrandFlg);
		break;

		//-----------------------------------------------------------
	default:
		// ありえない！！
		break;
	}
#endif
}

//------------------------------------------
// プロコン送信伝文(シーケンサデータ)バイト反転
// int nTc トランザクションコード
// BYTE* data 受信データ
//------------------------------------------
void SockDataExec::ByteHantenProDataSend(int nTc, BYTE * data)
{
#ifdef BYTE_HANTEN


	// ヘッダー部
	PRO_TYPE_HEAD *pHead = (PRO_TYPE_HEAD *)data;
	KizuFunction::ByteHanten2(2 * 1, (BYTE*)&pHead->nLen);
	KizuFunction::ByteHanten4(4 * 1, (BYTE*)&pHead->nTC);

	switch (nTc)
	{
		//-----------------------------------------------------------
	case PRO_TC_ZISEKI:								// 疵検査実績伝文
		if (true)
		{
			// データ部
			PRO_TYPE_ZISEKI *pBuf = (PRO_TYPE_ZISEKI *)data;
			PRO_TYPE_DATA_ZISEKI *pData = (PRO_TYPE_DATA_ZISEKI *)&pBuf->data;
			KizuFunction::ByteHanten2(2 * 1, (BYTE*)&pData->Common.nKubun);
			KizuFunction::ByteHanten2(2 * 6, (BYTE*)&pData->Common.nDate);
			KizuFunction::ByteHanten2(2 * 1, (BYTE*)&pData->Common.nGrandFlg);

			KizuFunction::ByteHanten4(4 * 1, (BYTE*)&pData->nZiMode);
			KizuFunction::ByteHanten4(4 * 1, (BYTE*)&pData->nKikiMode);
			KizuFunction::ByteHanten4(4 * 1, (BYTE*)&pData->nKoban);
			for (int ii = 0; ii < 6; ii++)
			{
				KizuFunction::ByteHanten2(2 * 1, (BYTE*)&pData->grade_inf[ii].nMixedRate);
				KizuFunction::ByteHanten2(2 * 1, (BYTE*)&pData->grade_inf[ii].nYobi1);
				KizuFunction::ByteHanten2(2 * 10, (BYTE*)&pData->grade_inf[ii].nTypeCnt);
				KizuFunction::ByteHanten2(2 * 2, (BYTE*)&pData->grade_inf[ii].nYobi2);
			}
		}
		break;

		//-----------------------------------------------------------
	default:
		// ありえない！！
		break;
	}
#endif
}

// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// その他


//------------------------------------------
// 受信伝文のBUFテーブル登録
// int nTc トランザクションコード
// BYTE* data 受信データ
// 戻り値 登録成否
//------------------------------------------
bool SockDataExec::InsertBufProRData(int nTc, BYTE * data)
{
	CString		sql;						// SQL文
	char		cKey[SIZE_SERIAL_FF_NO];	// ID
	KizuFunction::GetSerialNoFF(cKey);

	switch (nTc)
	{
		//-----------------------------------------------------------
	case PRO_TC_COIL:											// コイル情報伝文
		if (true)
		{
			PRO_TYPE_COILINF *pCoil = (PRO_TYPE_COILINF *)data;

			// SQL作成
			sql = "INSERT INTO ";
			sql += DB_BUF_PRO_R_COILINF;
			sql += " VALUES (";
			CreateSQL::sqlColumnInsert(&sql, cKey, sizeof(cKey));													// ID
			CreateSQL::sqlColumnAddTime(&sql, true);																// 登録日付
			CreateSQL::sqlColumnInsert(&sql, (const char *)&pCoil->data.CoilInf.KeyInf, sizeof(pCoil->data.CoilInf.KeyInf));	// キー情報
			CreateSQL::sqlColumnInsert(&sql, pCoil->data.CoilInf.nSendPoint);										// 送信位置
			sql += "?";																								// 生伝文
			sql += ")";

			// 生成したSQL文を DB登録スレッド に登録依頼
			DBSaveManager::DeliverySQL *pDeli = new DBSaveManager::DeliverySQL();
			pDeli->addmode = true;
			pDeli->strSql = sql;
			pDeli->cnt = 1;
			pDeli->size[0] = sizeof(PRO_TYPE_COILINF);
			pDeli->pImg[0] = new BYTE[pDeli->size[0]];
			memcpy(pDeli->pImg[0], pCoil, pDeli->size[0]);

			// DB更新完了時に コイル情報受信通知
			// ※ 共有メモリ格納時点で送信済み

			// DB登録依頼
			if (!mque_AddDB.AddToTail(pDeli, 20, 20))
			{
				// 絶対登録も無理・・・
				LOG(em_ERR), "[%s] DB登録依頼キューフル [%s]", my_sThreadName, DB_BUF_PRO_R_COILINF);
				syslog(SYSNO_QUEFULL_ERR, "[%s]", DB_BUF_PRO_R_COILINF);
				SAFE_DELETE(pDeli->pImg[0]);
				SAFE_DELETE(pDeli->SendMailItem);
				SAFE_DELETE(pDeli);
			}
			else
			{
				LOG(em_MSG), "[%s] コイル情報伝文 DB登録依頼完了", my_sThreadName);
				Sleep(100);		// 検査用と表示用のコイル情報を同時受信した場合、IDが被ってBUFテーブル登録エラーとなるための対策
			}
		}
		break;

		//-----------------------------------------------------------
	case PRO_TC_SAISUN:									// 採寸実績伝文
		if (true)
		{
			PRO_TYPE_SAISUN *pSaisun = (PRO_TYPE_SAISUN *)data;

			// SQL作成
			sql = "INSERT INTO ";
			sql += DB_BUF_PRO_R_SAISUN;
			sql += " VALUES (";
			CreateSQL::sqlColumnInsert(&sql, cKey, sizeof(cKey));													// ID
			CreateSQL::sqlColumnAddTime(&sql, true);																// 登録日付
			CreateSQL::sqlColumnInsert(&sql, (const char *)&pSaisun->data.KeyInf, sizeof(pSaisun->data.KeyInf));	// キー情報
			sql += "?,";																							// 生伝文
			sql += "NULL,";																							// 管理No
			sql += "NULL";																							// 編集日付
			sql += ")";

			// 生成したSQL文を DB登録スレッド に登録依頼
			DBSaveManager::DeliverySQL *pDeli = new DBSaveManager::DeliverySQL();
			pDeli->addmode = true;
			pDeli->strSql = sql;
			pDeli->cnt = 1;
			pDeli->size[0] = sizeof(PRO_TYPE_SAISUN);
			pDeli->pImg[0] = new BYTE[pDeli->size[0]];
			memcpy(pDeli->pImg[0], pSaisun, pDeli->size[0]);

			// DB更新完了時に プロコン採寸実績受信通知
			pDeli->SendMailItem = new MAIL_ITEM;
			memset(pDeli->SendMailItem, 0x00, sizeof(MAIL_ITEM));
			strcpy(pDeli->SendMailItem->cPc, ".");
			strcpy(pDeli->SendMailItem->cTask, TO_PUTDAT);
			pDeli->SendMailItem->Que.nEventNo = FACT_TO_PUTDAT_02;
			pDeli->SendMailItem->Que.nLineNo = getlineid();
			memcpy(pDeli->SendMailItem->Que.fc.data, cKey, sizeof(cKey));

			// DB登録依頼
			if (!mque_AddDB.AddToTail(pDeli, 20, 20))
			{
				// 絶対登録も無理・・・
				LOG(em_ERR), "[%s] DB登録依頼キューフル [%s]", my_sThreadName, DB_BUF_PRO_R_SAISUN);
				syslog(SYSNO_QUEFULL_ERR, "[%s]", DB_BUF_PRO_R_SAISUN);
				SAFE_DELETE(pDeli->pImg[0]);
				SAFE_DELETE(pDeli->SendMailItem);
				SAFE_DELETE(pDeli);
			}
			else
			{
				LOG(em_MSG), "[%s] 採寸実績伝文 DB登録依頼完了", my_sThreadName);
			}
		}
		break;

		//-----------------------------------------------------------
	case PRO_TC_CUT:									// カット実績伝文
		if (true)
		{
			PRO_TYPE_CUT *pCut = (PRO_TYPE_CUT *)data;

			// SQL作成
			sql = "INSERT INTO ";
			sql += DB_BUF_PRO_R_CUT;
			sql += " VALUES (";
			CreateSQL::sqlColumnInsert(&sql, cKey, sizeof(cKey));													// ID
			CreateSQL::sqlColumnAddTime(&sql, true);																// 登録日付
			CreateSQL::sqlColumnInsert(&sql, (const char *)&pCut->data.KeyInf, sizeof(pCut->data.KeyInf));			// キー情報
			CreateSQL::sqlColumnInsert(&sql, pCut->data.nStart);													// 開始位置
			CreateSQL::sqlColumnInsert(&sql, pCut->data.nEnd);														// 終了位置
			sql += "?,";																							// 生伝文
			sql += "NULL,";																							// 管理No
			sql += "NULL";																							// 編集日付
			sql += ")";

			// 生成したSQL文を DB登録スレッド に登録依頼
			DBSaveManager::DeliverySQL *pDeli = new DBSaveManager::DeliverySQL();
			pDeli->addmode = true;
			pDeli->strSql = sql;
			pDeli->cnt = 1;
			pDeli->size[0] = sizeof(PRO_TYPE_CUT);
			pDeli->pImg[0] = new BYTE[pDeli->size[0]];
			memcpy(pDeli->pImg[0], pCut, pDeli->size[0]);

			// DB更新完了時に プロコンカット実績受信通知
			pDeli->SendMailItem = new MAIL_ITEM;
			memset(pDeli->SendMailItem, 0x00, sizeof(MAIL_ITEM));
			strcpy(pDeli->SendMailItem->cPc, ".");
			strcpy(pDeli->SendMailItem->cTask, TO_PUTDAT);
			pDeli->SendMailItem->Que.nEventNo = FACT_TO_PUTDAT_03;
			pDeli->SendMailItem->Que.nLineNo = getlineid();
			memcpy(pDeli->SendMailItem->Que.fc.data, cKey, sizeof(cKey));

			// DB登録依頼
			if (!mque_AddDB.AddToTail(pDeli, 20, 20))
			{
				// 絶対登録も無理・・・
				LOG(em_ERR), "[%s] DB登録依頼キューフル [%s]", my_sThreadName, DB_BUF_PRO_R_CUT);
				syslog(SYSNO_QUEFULL_ERR, "[%s]", DB_BUF_PRO_R_CUT);
				SAFE_DELETE(pDeli->pImg[0]);
				SAFE_DELETE(pDeli->SendMailItem);
				SAFE_DELETE(pDeli);
			}
			else
			{
				LOG(em_MSG), "[%s] カット実績伝文 DB登録依頼完了", my_sThreadName);
			}
		}
		break;

		//-----------------------------------------------------------
	case PRO_TC_INIT:									// イニシャル伝文
																// 特に無し
		break;

		//-----------------------------------------------------------
	default:
		// ありえないはず！！
		LOG(em_ERR), "[%s] InsertBufProRData 受信伝文種類 NG=%d", my_sThreadName, nTc);
		break;
	}

	return true;
}

//------------------------------------------
// バッファファイルを読込
// int nTc				トランザクションコード
// BYTE* data			受信データ
// char* cFileName		ファイル名称
// int nMode			読み込みモード(0:バイト反転なし,1:バイト反転あり)
// 戻り値				true:正常 false:読込み失敗
//------------------------------------------
bool SockDataExec::ReadBufFile(int nTc, BYTE * data, char* cFileName, int nMode)
{
	BYTE nRecvBuf[1024];		// 伝文バッファ
	unsigned int nSize;			// 伝文サイズ
	CString sFileName;			// ファイル名称
	CFile file;

	sFileName.Format("%s", cFileName);

	switch (nTc) {
	case PRO_TC_COIL:
		nSize = sizeof(PRO_TYPE_COILINF);
		break;
	case PRO_TC_SAISUN:
		nSize = sizeof(PRO_TYPE_SAISUN);
		break;
	case PRO_TC_CUT:
		nSize = sizeof(PRO_TYPE_CUT);
		break;
	case PRO_TC_INIT:
		nSize = sizeof(PRO_TYPE_INIT);
		break;
	default:
		LOG(em_ERR), "[%s] TC対象外[%d]", my_sThreadName, nTc);
		return false;
	}

	// ファイル読み込み
	if (!file.Open(sFileName, CFile::typeBinary | CFile::modeRead | CFile::modeNoTruncate)) {
		LOG(em_ERR), "[%s] %s読み込み失敗", my_sThreadName, sFileName);
		return false;
	}
	else if (nSize != (unsigned int)file.GetLength()) {
		LOG(em_ERR), "[%s] %sサイズ異常", my_sThreadName, sFileName);
		file.Close();
		return false;
	}
	else {
		nSize = (unsigned int)file.GetLength();
		file.Read(nRecvBuf, nSize);
		LOG(em_MSG), "[%s] %s読み込み成功 TC:%d Size:%d Mode:%d", my_sThreadName, sFileName, nTc, nSize, nMode);
	}

	// バイト反転あり
	if (1 == nMode) {
		PRO_TYPE_HEAD* pHead = (PRO_TYPE_HEAD*)nRecvBuf;
		KizuFunction::ByteHanten2(2 * 1, (BYTE*)&pHead->nLen);
		KizuFunction::ByteHanten4(4 * 1, (BYTE*)&pHead->nTC);
		switch (nTc) {
		case PRO_TC_COIL:
			if (true) {
				PRO_TYPE_COILINF* pCoil = (PRO_TYPE_COILINF*)nRecvBuf;
				KizuFunction::ByteHanten2(2 * 1, (BYTE*)&pCoil->data.Common.nKubun);
				KizuFunction::ByteHanten2(2 * 6, (BYTE*)&pCoil->data.Common.nDate);
				KizuFunction::ByteHanten2(2 * 1, (BYTE*)&pCoil->data.Common.nGrandFlg);

				KizuFunction::ByteHanten4(4 * 1, (BYTE*)&pCoil->data.CoilInf.nSendPoint);
				KizuFunction::ByteHanten4(4 * 1, (BYTE*)&pCoil->data.CoilInf.nLenIn);
				KizuFunction::ByteHanten4(4 * 1, (BYTE*)&pCoil->data.CoilInf.nAtuZai);
				KizuFunction::ByteHanten4(4 * 1, (BYTE*)&pCoil->data.CoilInf.nWidZai);
				KizuFunction::ByteHanten4(4 * 1, (BYTE*)&pCoil->data.CoilInf.nAtuSei);
				KizuFunction::ByteHanten4(4 * 1, (BYTE*)&pCoil->data.CoilInf.nWidSei);
				KizuFunction::ByteHanten4(4 * 1, (BYTE*)&pCoil->data.CoilInf.nWidTrim);
				KizuFunction::ByteHanten4(4 * 1, (BYTE*)&pCoil->data.CoilInf.nNobiSPM);
				KizuFunction::ByteHanten4(4 * 1, (BYTE*)&pCoil->data.CoilInf.nNobiTLV);
				KizuFunction::ByteHanten4(4 * 1, (BYTE*)&pCoil->data.CoilInf.nCutF);
				KizuFunction::ByteHanten4(4 * 1, (BYTE*)&pCoil->data.CoilInf.nCutT);
				KizuFunction::ByteHanten4(4 * 1, (BYTE*)&pCoil->data.CoilInf.nSPMRoolKei_Up);
				KizuFunction::ByteHanten4(4 * 1, (BYTE*)&pCoil->data.CoilInf.nSPMRoolKei_Down);
				KizuFunction::ByteHanten4(4 * 31, (BYTE*)&pCoil->data.CoilInf.nYobi);
				for (int ii = 0; ii < PRO_SIZE_NEXTINF_CNT; ii++)
				{
					KizuFunction::ByteHanten4(4 * 1, (BYTE*)&pCoil->data.next_coil[ii].nFrontLen);
				}
			}
			break;
		case PRO_TC_SAISUN:
			if (true) {
				PRO_TYPE_SAISUN* pSaisun = (PRO_TYPE_SAISUN*)nRecvBuf;
				KizuFunction::ByteHanten2(2 * 1, (BYTE*)&pSaisun->data.Common.nKubun);
				KizuFunction::ByteHanten2(2 * 6, (BYTE*)&pSaisun->data.Common.nDate);
				KizuFunction::ByteHanten2(2 * 1, (BYTE*)&pSaisun->data.Common.nGrandFlg);

				KizuFunction::ByteHanten4(4 * 1, (BYTE*)&pSaisun->data.nLen);
				KizuFunction::ByteHanten4(4 * 1, (BYTE*)&pSaisun->data.nMakiLenF);
				KizuFunction::ByteHanten4(4 * 1, (BYTE*)&pSaisun->data.nSetCnt);
				KizuFunction::ByteHanten4(4 * 9, (BYTE*)&pSaisun->data.nYobi);
				for (int ii = 0; ii < PRO_MAX_CUT_COUNT; ii++) {
					KizuFunction::ByteHanten4(4 * 1, (BYTE*)&pSaisun->data.saisun[ii].nStart);
					KizuFunction::ByteHanten4(4 * 1, (BYTE*)&pSaisun->data.saisun[ii].nEnd);
				}
			}
			break;
		case PRO_TC_CUT:
			if (true) {
				PRO_TYPE_CUT* pCut = (PRO_TYPE_CUT*)nRecvBuf;
				KizuFunction::ByteHanten2(2 * 1, (BYTE*)&pCut->data.Common.nKubun);
				KizuFunction::ByteHanten2(2 * 6, (BYTE*)&pCut->data.Common.nDate);
				KizuFunction::ByteHanten2(2 * 1, (BYTE*)&pCut->data.Common.nGrandFlg);

				KizuFunction::ByteHanten4(4 * 1, (BYTE*)&pCut->data.nMode);
				KizuFunction::ByteHanten4(4 * 1, (BYTE*)&pCut->data.nStart);
				KizuFunction::ByteHanten4(4 * 1, (BYTE*)&pCut->data.nEnd);
				KizuFunction::ByteHanten4(4 * 1, (BYTE*)&pCut->data.nKoban);
			}
			break;
		case PRO_TC_INIT:
				PRO_TYPE_INIT* pInit = (PRO_TYPE_INIT*)nRecvBuf;

				KizuFunction::ByteHanten2(2 * 1, (BYTE*)&pInit->data.Common.nKubun);
				KizuFunction::ByteHanten2(2 * 6, (BYTE*)&pInit->data.Common.nDate);
				KizuFunction::ByteHanten2(2 * 1, (BYTE*)&pInit->data.Common.nGrandFlg);
			break;
		}
	}

	memcpy(data, nRecvBuf, nSize);

	// ファイルクローズ
	file.Close();

	return true;
}
