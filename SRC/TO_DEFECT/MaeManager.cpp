#include "StdAfx.h"
#include "MaeManager.h"

// メモリーリーク検出 ----->>>
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif
// <<<------------------------

#include "MainInstance.h"

//------------------------------------------
// コンストラクタ
//------------------------------------------
MaeManager::MaeManager(void) :
ThreadManager("MaeMgr")
{
	m_pCoilInf = NULL;
}

//------------------------------------------
// デストラクタ
//------------------------------------------
MaeManager::~MaeManager(void)
{
	delete m_pCoilInf;
}


//------------------------------------------
// スレッド開始前処理 (スレッド実行直後(シグナルセット前)にコールバック)
//------------------------------------------
int MaeManager::ThreadFirst()
{
	//// 事前準備
	m_pCoilInf = new COIL_INF;

	//// イベント設定
	HANDLE hArray[] = { mque_MaeRequest.g_evSem 
						};
	SetEventNum(sizeof(hArray)/sizeof(HANDLE), hArray);
	return 0;
}

//------------------------------------------
// スレッド終了前処理 (終了シグナル後にコールバック)
//------------------------------------------
int MaeManager::ThreadLast()
{
	return 0;
}

//------------------------------------------
// スレッドイベント発生
// int nEventNo イベントNo (0オリジン)
//------------------------------------------
void MaeManager::ThreadEvent(int nEventNo)
{
	////// シグナルの条件
	enum {	EV_QUR = 0,						// 処理依頼
	};

	////// シグナル条件分け
	switch (nEventNo) {

//-----------------------------------------------------------------------------------------------
	case EV_QUR: 					// 処理依頼
		if(true) {
			TYP_MAE_REQUEST* pDeli = mque_MaeRequest.GetFromHead(); 
			em_EXEC_MODE emMode = pDeli->emMode;
			switch (emMode) {
			//------------------------------------------------------------------------------------
			// 全前工程情報更新
			case UPDATE_MAE_REQUEST:
				Exec_MaeRequest(pDeli);

				// 表示PCに通知(前工程情報の有無に関わらず通知)
				MainInstance::Send_HyDefect_MaeRead(pDeli->cKizukenNo);
				break;
			//------------------------------------------------------------------------------------
			// フラグのみ更新
			case UPDATE_FRAG_ONLY:
				Exec_FragOnly(pDeli);
				break;
			default:
				break;
			}

			delete pDeli;
		}
		return;

	
//-----------------------------------------------------------------------------------------------
	default :
		// ありえない！！
		LOG(em_ERR), "[%s] ThreadEvent EventNo NG=%d", my_sThreadName, nEventNo );
		return;
	}
}


// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 外部アクセス
//------------------------------------------
// コイル情報受信
// COIL_INF const* pCoil	更新対象のコイル情報
//------------------------------------------
void MaeManager::RecvCoilInf(COIL_INF const* pCoil)
{
	memcpy(m_pCoilInf, pCoil, sizeof(COIL_INF));
	//// ODBC接続クラス インスタンス生成
	OdbcBase clsDB;
	if (!clsDB.Connect(TASKINI_NAME)) {
		// データベース接続エラー
		LOG(em_ERR), "[%s] DB接続エラー", my_sThreadName);
		syslog(SYSNO_DB_CONNECT_ERR, "");
		return;
	}

	//================================================
	//// FF前工程疵情報 受信 / 未受信チェック
	SYSTEMTIME STime;
	GetLocalTime(&STime);
	CString sWk;
	int nFfRecvFrag = -1;
	sWk.Format( "SELECT TOP(1) FF前工程疵情報受信フラグ FROM %s WHERE 命令No='%4s' AND 仮管理Noフラグ=-1 AND 登録日付+30 > '%d/%d/%d'",
		DB_MAE_COIL_INF, pCoil->data.cMeiNo, STime.wYear, STime.wMonth, STime.wDay);

	bool bRetc = clsDB.GetSelectKey( sWk, &nFfRecvFrag);
	if( ! bRetc ) {
		// データ無しもありえる
		LOG(em_WAR), "[%s] 対象の前工程コイル情報無し [命令No%4s:%s]", my_sThreadName, pCoil->data.cMeiNo, pCoil->cKizukenNo);
		//LOG(em_NON), "%s", sWk);
		return;
	}
	else if(0 != nFfRecvFrag) {
		// FFから前工程疵情報が未受信
		LOG(em_WAR), "[%s] FF前工程疵情報未受信 [命令No%4s:%s]", my_sThreadName, pCoil->data.cMeiNo, pCoil->cKizukenNo);
		return;
	}

	// ここまで来たら通常通り通知
	AddQue_MaeRequest(pCoil, UPDATE_MAE_REQUEST);
}

//------------------------------------------
// 前工程疵情報書込み完了通知受信
// char const* cMeiNo		命令No
// char const* cCoilNo		コイルNo
//------------------------------------------
void MaeManager::RecvMaeWrite(char const* cMeiNo, char const* cCoilNo)
{
	// 取得した前工程疵情報書込み完了通知が現コイルだった場合の救済処置
	if(	0 == memcmp(m_pCoilInf->data.cMeiNo,	  cMeiNo,  sizeof(SIZE_MEI_NO)) &&
		0 == memcmp(m_pCoilInf->data.cCoilNoIn, cCoilNo, sizeof(SIZE_COIL_NO))) {
		// ここまで来たら通常通り通知
		AddQue_MaeRequest((const COIL_INF*)m_pCoilInf, UPDATE_MAE_REQUEST);
	}
}

//------------------------------------------
// コイル実績書込み完了通知受信
// char const* cMeiNo		命令No
// char const* cCoilNo		コイルNo
//------------------------------------------
void MaeManager::RecvCoilResultWrite(COIL_INF const* pCoil)
{
	AddQue_MaeRequest((const COIL_INF*)pCoil, UPDATE_FRAG_ONLY);
}


// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// メンバー変数
//------------------------------------------
// 仮管理No更新 依頼
// COIL_INF const* pCoil	更新対象のコイル情報
//------------------------------------------
void MaeManager::AddQue_MaeRequest(COIL_INF const* pCoil, em_EXEC_MODE emMode)
{
	//================================================
	//// 事前準備
	TYP_MAE_REQUEST *pDeli = new TYP_MAE_REQUEST;

	memcpy( pDeli->cKizukenNo, pCoil->cKizukenNo, SIZE_KIZUKEN_NO);
	memcpy( pDeli->cMeiNo, pCoil->data.cMeiNo, sizeof(pDeli->cMeiNo));
	memcpy( pDeli->cCoilNo, pCoil->data.cCoilNoIn, sizeof(pDeli->cCoilNo));
	pDeli->nNobiSPM = pCoil->data_u.CoilInf.nNobiSPM;	// SPM伸び率[10^2%]
	pDeli->nCutF	= pCoil->data_u.CoilInf.nCutF;		// 入側におけるフロント端板カットの実績長[mm]
	pDeli->emMode	= emMode;							// 全前工程情報更新モード


	//================================================
	//// 登録依頼
	if( ! mque_MaeRequest.AddToTailFreeCheck(pDeli) ) {	// 登録
		LOG(em_ERR), "[%s] 前工程情報 仮疵検管理No更新依頼キューフル", my_sThreadName);
		syslog(SYSNO_QUEFULL_WAR, "[前工程情報 仮疵検管理No更新]");
		delete pDeli;
	}
}

// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// メイン処理

//------------------------------------------
// 仮管理No更新
// TYP_MAE_REQUEST const* p	依頼
//------------------------------------------
void MaeManager::Exec_MaeRequest(TYP_MAE_REQUEST const* p)
{
	char	cKariKizukenNo[SIZE_KIZUKEN_NO];				// 仮管理No
	double	dNobi = 1.0 + ((double)p->nNobiSPM / 10000.0);	// [10-2%→率]
	int		nCutF = p->nCutF;								// プロコンからもらう入側端板長さは[mm]をそのまま使用

	//// ODBC接続クラス インスタンス生成
	OdbcBase clsDB;
	if( ! clsDB.Connect(TASKINI_NAME)) {
		// データベース接続エラー
		LOG(em_ERR), "[%s] DB接続エラー", my_sThreadName);
		syslog(SYSNO_DB_CONNECT_ERR, "");
		return;
	}


	//================================================
	//// 仮管理Noの特定
	SYSTEMTIME STime;
	GetLocalTime(&STime);
	CString sWk;
	sWk.Format( "SELECT TOP(1) 管理No FROM %s WHERE コイルNo='%12s' AND 命令No='%4s' AND 仮管理Noフラグ=-1 AND 登録日付+30 > '%d/%d/%d'",
		DB_MAE_COIL_INF, p->cCoilNo, p->cMeiNo, STime.wYear, STime.wMonth, STime.wDay);

	bool bRetc = clsDB.GetSelectKey( sWk, sizeof(cKariKizukenNo), cKariKizukenNo);
	if( ! bRetc || 0x00 == cKariKizukenNo[0] ) {
		// データ無しもありえる
		LOG(em_WAR), "[%s] 対象の前工程コイル情報無し [コイルNo%12s:命令No%4s:%s]", my_sThreadName, p->cCoilNo, p->cMeiNo, p->cKizukenNo);
		return;
	}
	
	//================================================
	// 前工程情報編集　5CGL極微小φω独自（横展不可）
	// ・仮管理No → 管理Noへ
	// ・入側端板長さ減算
	// ・伸び率 反映

	// SQL文準備
	static const int nSqlCnt =5;
	CString sql[nSqlCnt];
	CString table[nSqlCnt] = {DB_MAE_DEFECT_IMG, DB_MAE_DEFECT_INF, DB_MAE_FF_INF, DB_MAE_COIL_LINE, DB_MAE_COIL_INF };
	// 前工程疵画像テーブル
	sql[0].Format( "UPDATE %s SET 管理No='%s' WHERE 管理No='%s'", DB_MAE_DEFECT_IMG, p->cKizukenNo, cKariKizukenNo);
	// 前工程疵情報テーブル
	sql[1].Format( "UPDATE %s SET 管理No='%s' WHERE 管理No='%s'", DB_MAE_DEFECT_INF, p->cKizukenNo, cKariKizukenNo);
	// FF前工程疵情報テーブル
	sql[2].Format( "UPDATE %s SET 管理No='%s', "
		//"FF疵長さ方向位置=(CASE WHEN FF疵長さ方向位置 - %d <= 0 THEN 0 ELSE FF疵長さ方向位置 - %d END) * %f,"
		//"最終長手位置= 最終長手位置 * %f "
		//DB_MAE_FF_INF, p->cKizukenNo, nCutF, nCutF, dNobi, dNobi, cKariKizukenNo);
		"最終長手位置= (CASE WHEN FF疵長さ方向位置 - %d <= 0 THEN 0 ELSE FF疵長さ方向位置 - %d END) * %f "
		"WHERE 管理No='%s'",
		DB_MAE_FF_INF, p->cKizukenNo, nCutF, nCutF, dNobi, cKariKizukenNo);
	// 前工程ライン別コイル情報テーブル
	sql[3].Format( "UPDATE %s SET 管理No='%s' WHERE 管理No='%s'",
		DB_MAE_COIL_LINE, p->cKizukenNo, cKariKizukenNo);
	// 前工程コイル情報テーブル
	sql[4].Format("UPDATE %s SET 管理No='%s', 仮管理Noフラグ=0, "
		"入側フロント端板長さ=入側フロント端板長さ * %f, "
		"冷延コイル長さ=冷延コイル長さ * %f, "
		"採取開始位置長さ=採取開始位置長さ * %f, "
		"採取終了位置長さ=採取終了位置長さ * %f "
		"WHERE 管理No='%s'",
		DB_MAE_COIL_INF, p->cKizukenNo, dNobi, dNobi, dNobi, dNobi, cKariKizukenNo);
	LOG(em_MSG), "[%s] 仮管理No付け替え開始 [%s]→[%s] 伸び率:%.04f フロント端板カット長:%d", my_sThreadName, cKariKizukenNo, p->cKizukenNo, dNobi, nCutF);

	//for (int ii = 0; ii < nSqlCnt; ii++)
	//{
	//	LOG(em_MSG), "[%s] 仮管理No付け替え SQL%d [%s] [%s]", my_sThreadName, ii + 1, cKariKizukenNo, sql[ii]);
	//}

	//// 管理No 付け替え
	DWORD nUpDateRow;				// 変更行数
	for(int ii=0; ii<nSqlCnt; ii++) {
		// UpDate
		if( ! clsDB.ExecuteDirect(sql[ii]) ) {
			LOG(em_ERR), "[%s] 仮管理No付け替え異常 [%s] [%s]", my_sThreadName, cKariKizukenNo, sql[ii]);
			syslog(SYSNO_DB_EXECUTEDIRECT_ERR, "[%s][管理No:%s]", table[ii], cKariKizukenNo);
		} else {
			clsDB.GetNumRows(&nUpDateRow); // 変更行数取得

			LOG(em_MSG), "[%s] 仮管理No付け替え SQL%d個目完了 (変更対象行:%d)", my_sThreadName, ii+1, nUpDateRow);
		}
	}
}


//------------------------------------------
// フラグのみ更新
// TYP_MAE_REQUEST const* p	依頼
//------------------------------------------
void MaeManager::Exec_FragOnly(TYP_MAE_REQUEST const* p)
{
	char	cKariKizukenNo[SIZE_KIZUKEN_NO];		// 仮管理No


	//// ODBC接続クラス インスタンス生成
	OdbcBase clsDB;
	if( ! clsDB.Connect(TASKINI_NAME)) {
		// データベース接続エラー
		LOG(em_ERR), "[%s] DB接続エラー", my_sThreadName);
		syslog(SYSNO_DB_CONNECT_ERR, "");
		return;
	}


	//================================================
	//// 仮管理Noの特定
	SYSTEMTIME STime;
	GetLocalTime(&STime);
	CString sWk;
	sWk.Format( "SELECT TOP(1) 管理No FROM %s WHERE コイルNo='%12s' AND 命令No='%4s' AND 仮管理Noフラグ=-1 AND 登録日付+30 > '%d/%d/%d'",
		DB_MAE_COIL_INF, p->cCoilNo, p->cMeiNo, STime.wYear, STime.wMonth, STime.wDay);

	bool bRetc = clsDB.GetSelectKey( sWk, sizeof(cKariKizukenNo), cKariKizukenNo);
	if( ! bRetc || 0x00 == cKariKizukenNo[0] ) {
		// データ無しもありえる
		LOG(em_INF), "[%s] 対象の前工程コイル情報無し（既に更新済み？） [コイルNo%12s:命令No%4s:%s]", my_sThreadName, p->cCoilNo, p->cMeiNo, p->cKizukenNo);
		return;
	}
	
	//================================================
	// 前工程情報編集　1CAPL中央φω独自（横展不可）
	// ・仮管理Noフラグ → -9(通販済み)へ

	// SQL文準備
	static const int nSqlCnt =1;
	CString sql[nSqlCnt];
	CString table[nSqlCnt] = { DB_MAE_COIL_LINE };
	// 前工程コイル情報テーブル
	sql[0].Format("UPDATE %s SET 仮管理Noフラグ=-9 "
		"WHERE 管理No='%s'",
		DB_MAE_COIL_INF, cKariKizukenNo);
	LOG(em_MSG), "[%s] 仮管理Noフラグのみ更新開始 [%s]→[%s]", my_sThreadName, cKariKizukenNo, p->cKizukenNo);

	//// 管理No 付け替え
	DWORD nUpDateRow;				// 変更行数
	for(int ii=0; ii<nSqlCnt; ii++) {
		// UpDate
		if( ! clsDB.ExecuteDirect(sql[ii]) ) {
			LOG(em_ERR), "[%s] 仮管理Noフラグのみ更新異常 [%s] [%s]", my_sThreadName, cKariKizukenNo, sql[ii]);
			syslog(SYSNO_DB_EXECUTEDIRECT_ERR, "[%s][管理No:%s]", table[ii], cKariKizukenNo);
		} else {
			clsDB.GetNumRows(&nUpDateRow); // 変更行数取得

			LOG(em_MSG), "[%s] 仮管理Noフラグのみ更新 SQL%d個目完了 (変更対象行:%d)", my_sThreadName, ii+1, nUpDateRow);
		}
	}
}