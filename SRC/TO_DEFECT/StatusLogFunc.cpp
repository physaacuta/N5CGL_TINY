#include "StdAfx.h"
#include "StatusLogFunc.h"

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
StatusLogFunc::StatusLogFunc(void) :
FuncBase("StatFunc")
{
	// 初期化
	memset( &m_oldStatusLog, 0x00, sizeof(m_oldStatusLog));
}

//------------------------------------------
// デストラクタ
//------------------------------------------
StatusLogFunc::~StatusLogFunc(void)
{

}

// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 処理

//------------------------------------------
// 初期設定
//------------------------------------------
void StatusLogFunc::Alloc()
{
	// AutoNoに必要なデータを初期セットする
	InitAutoNo();
}

//------------------------------------------
// AutoNoに必要なデータを初期セットする
//------------------------------------------
void StatusLogFunc::InitAutoNo()
{
	//// 現在日をセット
	m_nDay = CTime::GetCurrentTime().GetDay(); 

	//// ODBCの設定位置を取得する
	// 現在のセッションを取得する
	char Session[128];
	GetPrivateProfileString("DB", "SESSION", "", (char *)Session, sizeof(Session), PCINI_NAME);
	// 位置特定
	char SessionWK[128];
	CString		cKey;
	for(int ii=1; ii<10; ii++) {
		cKey.Format("ODBCSET_%d", ii); 
		GetPrivateProfileString("DB", cKey, "", SessionWK, sizeof(SessionWK), PCINI_NAME);
		if(0 == strcmp(Session, SessionWK)) {
			m_nOdbcID = ii;
			break;
		}
	}


	//// 最新日付のAutoNoを取得する
	CString cSql;					// SQL文

	// SQL文
	cSql.Format("SELECT TOP(1) AutoNo FROM %s ORDER BY 登録日付 desc, AutoNo desc", DB_STATUS_LOG);

	// 実行
	int nWk;
	if( ! KizuODBC::OdbcBase::GetSelectKey_Static(cSql, &nWk, TASKINI_NAME, INI_DB) ) {
		LOG(em_ERR), "[%s] オートNo 最新値 取得失敗！！", my_sThreadName );
		m_nAutoNo = 0;
	} else {
		m_nAutoNo = nWk / 10;
		LOG(em_INF), "[%s] オートNo 最新値 DBのAutoNo=%d, 現在日付=%d (連番=%d, ID=%d)", my_sThreadName, nWk, m_nDay, m_nAutoNo, m_nOdbcID);
	}
}

//------------------------------------------
// 日付をチェックして、日が変われば、オートNoをリセットする。
// int nDay 登録日
//------------------------------------------
void StatusLogFunc::RefrashAutoNo(int nDay)
{
	// 日が変更された？
	if(m_nDay == nDay) {
		// 同じ日なら AutoNoを更新するだけ
		m_nAutoNo ++;
		return;
	}

	// セット
	LOG(em_MSG), "[%s] オートNoリセット 変更日[%d→%d]", my_sThreadName, m_nDay, nDay);
	m_nDay	  = nDay;
	m_nAutoNo = 1;
}

//------------------------------------------
// ステータス情報チェック
// TYP_STATUS_LOG* pTyp 今回の情報
//------------------------------------------
void StatusLogFunc::CheckStatusLog(TYP_STATUS_LOG* pTyp)
{
	bool bAdd = false;

	//--------------------------------
	// 登録チェック

	// 前回から変化有り？
	if( 0 != memcmp( &m_oldStatusLog, pTyp, sizeof(TYP_STATUS_LOG::CHECK_DATA)) ) {
		memcpy( &m_oldStatusLog, pTyp, sizeof(m_oldStatusLog));
		bAdd = true;
	}
	// 稼動状態が、初期化中のときは、登録しない。鬱陶しい
	if( DIV_KADOU_INIT == pTyp->Data.emKadou ) {
		bAdd = false;
	}


	//---------------
	// DB登録
	if(bAdd) {
		//// 事前準備
		DELIVERY_DB* pDeli = new DELIVERY_DB;
		pDeli->kubun = ENUM_DB_KUBUN::em_STATUS_LOG;
		pDeli->data = (BYTE*)new DBDATA_STATUS_LOG;
		DBDATA_STATUS_LOG* pWk = (DBDATA_STATUS_LOG*)pDeli->data;

		// データ
		pWk->emKadou	= pTyp->Data.emKadou;
		for(int ii=0; ii<NUM_MEN; ii++) {
			pWk->emKensa[ii]	= pTyp->Data.emKensa[ii];
			pWk->emKiki[ii]		= pTyp->Data.emKiki[ii];
		}
		memcpy(pWk->cMsg, pTyp->Data.cMsg, sizeof(pTyp->Data.cMsg));
		memcpy(pWk->cKizukenNo, pTyp->cKizukenNo, sizeof(pTyp->cKizukenNo));
		memcpy(pWk->cCoilNo, pTyp->cCoilNo, sizeof(pTyp->cCoilNo));

		// 主キーセット
		pWk->time = CTime::GetCurrentTime();
		RefrashAutoNo(pWk->time.GetDay());			// 日付によるオートNoリセット
		pWk->nAutoNo = (m_nAutoNo*10 + m_nOdbcID); 

		LOG(em_MSG), "[%s] 変化[%s][%s][%s/%s][%s/%s]", my_sThreadName, pWk->cCoilNo, pWk->cKizukenNo,
			DivNameManager::GetDivKensa(pWk->emKensa[0]), DivNameManager::GetDivKensa(pWk->emKensa[1]),
			DivNameManager::GetDivKiki(pWk->emKiki[0]), DivNameManager::GetDivKiki(pWk->emKiki[1]) );


		//// 登録依頼
		if( ! mque_pAddSQLData->AddToTailFreeCheck(pDeli, 10) ) { 
			// キューフル
			delete pDeli->data;
			delete pDeli;

			// DBが異常 (通常ありえない)
			LOG(em_ERR), "[%s] DB登録依頼キューフル [DBDATA_STATUS_LOG]", my_sThreadName);
			syslog(SYSNO_QUEFULL_WAR, "[DBDATA_STATUS_LOG]");
		}
	}
}

