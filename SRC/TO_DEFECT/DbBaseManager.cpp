#include "StdAfx.h"
#include "DbBaseManager.h"

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
DbBaseManager::DbBaseManager(const char *cThreadName) :
ThreadManager(cThreadName),
m_nJpgQuality(80),
m_bQueState(false),
m_bBcpState(false),
m_bDeleteDbMode(true),
mque_AddDB(MAX_ADDDBCOUNT),
gque_AddSQLData(MAX_SQLCOUNT)
{
	// 初期化
	for(int ii=0; ii<THREAD_DBSAVE; ii++) mcls_pDbs[ii] = NULL;

	// シグナル初期化
	m_evTimeBcpAutoCommit  = CreateWaitableTimer(NULL, FALSE, NULL);
	m_evTimeBcpStatusCheck = CreateWaitableTimer(NULL, FALSE, NULL);

	// 実績保存スキップ
	for(int ii=0; ii<NUM_MEN; ii++)	{
		m_nSaveSkipId[ii]		= 0;
		m_bSaveSkipState[ii]	= false;
		m_evNowSaveSkip[ii]		= CreateEvent(NULL, FALSE, FALSE, NULL);
		m_evTimeSaveSkip[ii]	= CreateWaitableTimer(NULL, FALSE, NULL);
	}
}

//------------------------------------------
// デストラクタ
//------------------------------------------
DbBaseManager::~DbBaseManager(void)
{
	// 念のため
	Free();

	// 開放
	CloseHandle(m_evTimeBcpAutoCommit);
	CloseHandle(m_evTimeBcpStatusCheck);
	for(int ii=0; ii<NUM_MEN; ii++)	{
		CloseHandle(m_evNowSaveSkip[ii]);
		CloseHandle(m_evTimeSaveSkip[ii]);
	}
}

// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 基本

//------------------------------------------
// 関数初期化
//------------------------------------------
void DbBaseManager::Alloc()
{
	//// DB登録クラス生成
	for(int ii=0; ii<THREAD_DBSAVE; ii++) {
		mcls_pDbs[ii] = new DBSaveManager( ii, &mque_AddDB );
		mcls_pDbs[ii]->SetLogMgr( mcls_pLog );
		mcls_pDbs[ii]->SetReTry(true);			// SQL実行失敗時リトライする 
		mcls_pDbs[ii]->Start();
	}


	// これは、継承先で定義すること
	//// 先頭のインスタンスのみ DB削除管理に通知ありとする
	//mcls_pDbs[0]->SetSendDbManager(true);
		
	//// バッファ初期化
	memset(m_SqlInf, 0x00, sizeof(m_SqlInf));

	//// 定周期タイマー生成
	__int64 ts = (__int64)TIME_BCP_STATUS_CHECK * -10000;
	SetWaitableTimer(m_evTimeBcpStatusCheck, (LARGE_INTEGER*)&ts, TIME_BCP_STATUS_CHECK, NULL, NULL, FALSE);


	//// 継承先に通知
	OnAlloc();
}

//------------------------------------------
// 関数開放
//------------------------------------------
void DbBaseManager::Free()
{
	//// DB登録クラス生成
	for(int ii=0; ii<THREAD_DBSAVE; ii++) {
		if(NULL == mcls_pDbs[ii]) continue;

		mcls_pDbs[ii]->Stop(30000);
		SAFE_DELETE( mcls_pDbs[ii] );
	}

	//// 定周期タイマー停止
	CancelWaitableTimer(m_evTimeBcpStatusCheck);

	//// 継承先に通知
	OnFree();
}

//------------------------------------------
// リセット
//------------------------------------------
void DbBaseManager::ReSetMaxCount()
{
	gque_AddSQLData.ReSetMaxCount();
	mque_AddDB.ReSetMaxCount();

	//// 時間計測準備
	mcls_Time.InitAve();
	for(int ii=0; ii<THREAD_DBSAVE; ii++) {
		if(NULL == mcls_pDbs[ii]) continue;

		mcls_pDbs[ii]->InitTime();
	}
	OnReSetMaxCount();
}

//------------------------------------------
// DB削除管理に通知
// bool bWrite	true:書き込み優先
//------------------------------------------
void DbBaseManager::Send_To_DbManage(bool bWrite)
{
	if(  m_bDeleteDbMode == bWrite) return ;

	m_bDeleteDbMode = bWrite;

	// 送信
	COMMON_QUE que;
	que.nEventNo = FACT_TI_DELETE_01;
	que.nLineNo = LINE_ID;				
	que.fl.data[0] = (bWrite ? 1 : 0 );	// 0:削除優先 1:書込み優先
	send_mail(TI_DELETE, ".", &que);
}


// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 欠陥個数紐付け用

//------------------------------------------
// バッファ切り替え
// const char* cKizukenNo 管理No
//------------------------------------------
void DbBaseManager::ChangeSQLInf(const char* cKizukenNo)
{
	int ii;

	//// ホントに同じものがないか。複数回実行されるため対応
	if( -1 != GetRecSQLInf(cKizukenNo) ) return;

	//// バッファレコード移動
	for(ii=SIZE_SQLINF_NUM-1; ii>0; ii--) {
		memcpy(&m_SqlInf[ii], &m_SqlInf[ii-1], sizeof(TYP_SQL_INF));
	}
	//// 先頭初期化
	memset(&m_SqlInf[0], 0x00, sizeof(TYP_SQL_INF));
	
	//// 管理Noセット
	memcpy(m_SqlInf[0].cKizukenNo, cKizukenNo, SIZE_KIZUKEN_NO );
}

//------------------------------------------
// バッファレコード特定
// const char* cKizukenNo 管理No
//------------------------------------------
int DbBaseManager::GetRecSQLInf(const char* cKizukenNo)
{
	int ii;
	// 検索
	for(ii=0; ii<SIZE_SQLINF_NUM; ii++) {
		if(0 == strcmp(m_SqlInf[ii].cKizukenNo, cKizukenNo) ) {
			return ii;
		}
	}
	// 未検出
	return -1;
}


// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// BCP関連

//------------------------------------------
// BCPクラス コネクト
// BCPBase* cls_bcp BCPクラス
// char const* pTableName 接続するテーブル名称
//------------------------------------------
bool DbBaseManager::BCP_Connect(BCPBase* cls_bcp, char const* pTableName)
{
	//// 既にコネクト状態？
	if( cls_bcp->IsConnect() ) return true;

	//// DB接続
	if( ! cls_bcp->Connect(TASKINI_NAME) ) {
		LOG(em_ERR), "[%s] DB コネクト失敗", my_sThreadName);
		return false;
	}

	//// BCPイニシャル
	if( !cls_bcp->BCP_Init(pTableName) ) {
		LOG(em_ERR), "[%s] BCP Init失敗 [%s]", my_sThreadName, cls_bcp->GetTableName());
		cls_bcp->DisConnect();
		return false;
	}

	LOG(em_MSG), "[%s] BCP コネクト完了 [%s]", my_sThreadName, cls_bcp->GetTableName());
	return true;
}

//------------------------------------------
// BCPクラス 開放
// BCPBase* cls_bcp BCPクラス
//------------------------------------------
void DbBaseManager::BCP_Dispose(BCPBase* cls_bcp)
{
	//// コネクトしてない？
	if( ! cls_bcp->IsConnect() ) return;
	
	//// もしまだ未登録の奴があれば
	cls_bcp->BCP_Batch(); 

	//// BCPの開放
	if( cls_bcp->BCP_Down() ) {
		LOG(em_MSG), "[%s] BCP 開放完了 [%s]", my_sThreadName, cls_bcp->GetTableName());
	} else {
		LOG(em_ERR), "[%s] BCP 開放失敗 [%s]", my_sThreadName, cls_bcp->GetTableName());
	}
}

//------------------------------------------
// BCPクラス データ転送
// BCPBase* cls_bcp BCPクラス
//------------------------------------------
bool DbBaseManager::BCP_SendRow(BCPBase* cls_bcp)
{
	//// データ 転送
	if( ! cls_bcp->BCP_SendRow() ) {
		LOG(em_ERR), "[%s] BCP sendrow失敗 [%s]", my_sThreadName, cls_bcp->GetTableName());
		return false;
	}
	return true;
}

//------------------------------------------
// BCPクラス データ書込み
// BCPBase* cls_bcp BCPクラス
//------------------------------------------
bool DbBaseManager::BCP_Batch(BCPBase* cls_bcp)
{
	int retc = cls_bcp->BCP_Batch();
	if( -1 == retc ) {
		LOG(em_ERR), "[%s] BCP batch失敗 [%s]", my_sThreadName, cls_bcp->GetTableName());
		return false;
	} else {
		//LOG(em_MSG), "[%s] BCP batch [%s] %d件", my_sThreadName, cls_bcp->GetTableName(), retc);
	}
	return true;
}
//------------------------------------------
// BCPクラス バインドデータサイズ指定 (varbinary登録少ない現象の、暫定対策用)
// BCPBase* cls_bcp BCPクラス
// int coulmn テーブルのカラム位置 (1オリジン)
// long nSize サイズ
//------------------------------------------
bool DbBaseManager::BCP_ColLen(BCPBase* cls_bcp, int coulmn, long nSize)
{
	//// バインドデータサイズ指定
	if( ! cls_bcp->BCP_ColLen(coulmn, nSize) ) {
		LOG(em_ERR), "[%s] BCP collen失敗 [%s]", my_sThreadName, cls_bcp->GetTableName());
		return false;
	}
	return true;
}

//------------------------------------------
// BCP自動コミットタイマー開始
//------------------------------------------
void DbBaseManager::BCP_AutoCommitTimerOn()
{
	// 自動コミット用 (１回のみ)
	__int64 ts = (__int64)TIME_BCP_AUTO_COMMIT * -10000;
	SetWaitableTimer(m_evTimeBcpAutoCommit, (LARGE_INTEGER*)&ts, 0, NULL, NULL, FALSE);
}

// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// スレッド操作


//------------------------------------------
// スレッド開始前処理 (スレッド実行直後(シグナルセット前)にコールバック)
//------------------------------------------
int DbBaseManager::ThreadFirst()
{
	//// 事前準備

	//// イベント設定
	AddEventNum(2, m_evNowSaveSkip );
	AddEventNum(2, m_evTimeSaveSkip );				// 大量登録中でも、即時に反映させたいため、キューよりも前

	AddEventNum(1, &gque_AddSQLData.g_evSem);
	AddEventNum(1, &m_evTimeBcpAutoCommit);
	AddEventNum(1, &m_evTimeBcpStatusCheck);

	//// イベント追加
	ThreadFirst_AddHandle();

	return 0;
}

//------------------------------------------
// スレッド終了前処理 (終了シグナル後にコールバック)
//------------------------------------------
int DbBaseManager::ThreadLast()
{
	return 0;
}

//------------------------------------------
// スレッドイベント発生
// int nEventNo イベントNo (0オリジン)
//------------------------------------------
void DbBaseManager::ThreadEvent(int nEventNo)
{
	DbBaseManager::DeliveryDB* pDeli;		// DB登録依頼データ

	////// シグナルの条件
	enum {	EV_NOW_SAVE_SKIP_O = 0,			// 実績保存遅れ発生 表
			EV_NOW_SAVE_SKIP_U,				// 実績保存遅れ発生 裏
			EV_TIME_SAVE_SKIP_O,			// 実績保存遅れ終了 表
			EV_TIME_SAVE_SKIP_U,			// 実績保存遅れ終了 裏

			EV_QUE,							// DB登録依頼
			EV_TIME_BCP_AUTO_COMMIT,		// BCP自動コミット用
			EV_TIME_BCP_STATUS_CHECK,		// BCP状態チェック
			
			EV_LAST
	};


	////// シグナル条件分け
	//======================================================
	// DB登録依頼
	if(nEventNo == EV_QUE) {
		//// 取り出し	
		pDeli = gque_AddSQLData.GetFromHead();

		//// 継承先に通知
		OnQueDb(pDeli->kubun, pDeli->data );

		//// 開放
		SAFE_DELETE(pDeli->data);
		SAFE_DELETE(pDeli);


	//======================================================
	// BCP自動コミット用
	} else if(nEventNo == EV_TIME_BCP_AUTO_COMMIT)	{

		//// 継承先に通知
		OnBcpAutoCommit(true);

	//======================================================
	// BCP状態チェック用
	} else if(nEventNo == EV_TIME_BCP_STATUS_CHECK)	{

		//// 継承先に通知
		OnBcpStatusCheck();

	//======================================================
	// 実績保存遅れ終了
	} else if(nEventNo == EV_TIME_SAVE_SKIP_O || nEventNo == EV_TIME_SAVE_SKIP_U)	{
		
		SaveSkipEnd(nEventNo-EV_TIME_SAVE_SKIP_O);

	//======================================================
	// 実績保存遅れ発生
	} else if(nEventNo == EV_NOW_SAVE_SKIP_O || nEventNo == EV_NOW_SAVE_SKIP_U)	{
		SaveSkipStart(nEventNo-EV_NOW_SAVE_SKIP_O);

	//======================================================
	// それ以外。継承先で追加したシグナル
	} else {
		//// 継承先に通知
		OnAddSignal( nEventNo - EV_LAST );
	}
}


// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 実績保存スキップ
//------------------------------------------
// 実績保存スキップ 開始
// int nMen 表裏
//------------------------------------------
void DbBaseManager::SaveSkipStart(int nMen)
{
	// 既にスキップ中であれば 機器状態に登録はしない
	if( ! m_bSaveSkipState[nMen] ) {	
		// 機器状態セット
		m_bSaveSkipState[nMen] = true;
		mcls_pStatus->SetStatusQue(m_nSaveSkipId[nMen], false, false);		// 有害Grスキップ
		mcls_pStatus->SetStatusRead();										// 非同期強制読み込みシグナルセット (すぐに状態を通知したいため)
	}

	// 外部公開シグナル
	__int64 ts = TIME_SAVE_SKIP_WAIT * -10000;
	SetWaitableTimer(m_evTimeSaveSkip[nMen], (LARGE_INTEGER*)&ts, 0, NULL, NULL, FALSE);
}

//------------------------------------------
// 実績保存スキップ 完了
// int nMen 表裏
//------------------------------------------
void DbBaseManager::SaveSkipEnd(int nMen)
{
	m_bSaveSkipState[nMen] = false;
	mcls_pStatus->SetStatusQue(m_nSaveSkipId[nMen], true);	// 有害Grスキップ
	mcls_pStatus->SetStatusRead();							// 非同期強制読み込みシグナルセット (すぐに状態を通知したいため)
}

//------------------------------------------
// 外部実績保存スキップ時 の 削除個数加算
// char const* cKizukenNo
// int nMen 表裏
// int nLowGrFlg 無害Grフラグ (1:無害疵Gr  0:有害疵Gr)
//------------------------------------------
void DbBaseManager::SetSaveSkipAdd(char const* cKizukenNo, int nMen, int nLowGrFlg)
{
	int infRec  = GetRecSQLInf(cKizukenNo);
	if( -1 == infRec ) return;

	// 加算
	if( 1 == nLowGrFlg ) m_SqlInf[infRec].nLowGrDeleCnt[nMen] ++;
	else				 m_SqlInf[infRec].nDeleCnt[nMen] ++;
	m_SqlInf[infRec].nTotalCnt[nMen] ++;
}

//==========================================
// 疵実績保存 フロー制御
//------------------------------------------
// char const* cKizukenNo      : 管理No
// int         nRec	           : 配列位置 0オリジン
// int         nMen            : 表裏
// int         nLowGrFlg       : 無害Grフラグ (1:無害疵Gr  0:有害疵Gr)
// int         nFrameDefectMax : 1フレーム内で最大疵 (0:最大疵じゃない 1:最大疵)
// 戻値        int             : -1:保存NG   0:保存OK   9:画像保存無しバージョン
//==========================================
int DbBaseManager::SaveDefectCheck(char const* cKizukenNo, int nMen, int nLowGrFlg, int nFrameDefectMax)
{
	int nRec = GetRecSQLInf(cKizukenNo);
	if( -1 == nRec ) return -1;

	return SaveDefectCheck(nRec, nMen, nLowGrFlg, nFrameDefectMax);
}
int DbBaseManager::SaveDefectCheck(int nRec, int nMen, int nLowGrFlg, int nFrameDefectMax)
{
	if( 0 > nRec || SIZE_SQLINF_NUM <= nRec ) return -1;

	//======================================================
	// 登録キューの空き数により 登録加減
	int nQueFreeCntSQL = gque_AddSQLData.GetFreeCount();		// SQL登録キュー
	int nQueFreeCntDB  = mque_AddDB.GetFreeCount();				// DB登録キュー
	double dRateSQL = ((double)nQueFreeCntSQL / MAX_SQLCOUNT) * 100;
	double dRateDB  = ((double)nQueFreeCntDB / MAX_ADDDBCOUNT) * 100;

	//// もう間に合わない為、有害Grでも破棄
	// スキップ終了
	if( m_bQueState && (RATE_SKIP_END_DB <= dRateDB && RATE_SKIP_END_SQL <= dRateSQL) ){
		m_bQueState = false;
		LOG(em_WAR), "[%s] 疵実績破棄終わり。有害破棄=%d:%d QUE空[SQL生成=%d, DB登録=%d]", my_sThreadName, m_SqlInf[nRec].nDeleCnt[0], m_SqlInf[nRec].nDeleCnt[1], nQueFreeCntSQL, nQueFreeCntDB );

		gque_AddSQLData.ReSetMaxCount();
		mque_AddDB.ReSetMaxCount(); 
	}
	// スキップ開始
	if(m_bQueState || RATE_SKIP_START_DB > dRateDB || RATE_SKIP_START_SQL > dRateSQL) {
		// もう間に合わない
		if( ! m_bQueState ) {
			m_bQueState = true;
			LOG(em_WAR), "[%s] 登録間に合わない為、疵実績破棄 開始！！ QUE空[SQL生成=%d, DB登録=%d]", my_sThreadName, nQueFreeCntSQL, nQueFreeCntDB );
		}

		// スキップ中でも有害は極力残したい対策
		if( RATE_SKIP_START_DB < dRateDB || RATE_SKIP_START_SQL < dRateSQL ) {	// スキップ中の若干余裕がある場合、有害な最大疵のみは保存
			if( (0 == nLowGrFlg && 1 == nFrameDefectMax) ){
#ifdef DEBUG_PRINT_SAVEDEFCHK
				LOG(em_FIL), "[%s] スキップ中だが、有害疵なので残す [LowFlg=%d]QUE空[SQL生成=%.2f％, DB登録=%.2f％]", my_sThreadName, nLowGrFlg, dRateSQL, dRateDB );
#endif
				return 0;
			}
		}

		// スキップ
		if( 1 == nLowGrFlg ) {
			m_SqlInf[nRec].nLowGrDeleCnt[nMen] ++;
#ifdef DEBUG_PRINT_SAVEDEFCHK
			LOG(em_FIL), "[%s] 無害疵なのでスキップ [LorFlg=%d][削除数:%d]QUE空[SQL生成=%.2f％, DB登録=%.2f％]", my_sThreadName, nLowGrFlg, m_SqlInf[nRec].nLowGrDeleCnt[nMen], dRateSQL, dRateDB );
#endif
		} else {
			// 有害グレード
			m_SqlInf[nRec].nDeleCnt[nMen] ++;
			this->SetEvSaveSkip(nMen);		// 有害Grスキップの機器状態管理開始 
#ifdef DEBUG_PRINT_SAVEDEFCHK
			LOG(em_FIL), "[%s] 有害疵をスキップ [LorFlg=%d][削除数:%d]QUE空[SQL生成=%.2f％, DB登録=%.2f％]", my_sThreadName, nLowGrFlg, m_SqlInf[nRec].nDeleCnt[nMen], dRateSQL, dRateDB );
#endif
		}

		m_SqlInf[nRec].nTotalCnt[nMen] ++;
		return -1;
	}

	//// 厚手系は画像スキップ処理がないためコメント
//	//// 無害Grの歯抜け画像スキップ機能
//	//// 5疵に1疵だけ画像を保存
//	if( 1 == nLowGrFlg && 1 != nFrameDefectMax && 
//		( RATE_LOWGR_IMGSKIP_DB > dRateDB || RATE_LOWGR_IMGSKIP_SQL > dRateSQL ) &&
//		( 0 != m_SqlInf[nRec].nTotalCnt[nMen] % 5) ) {
//#ifdef DEBUG_PRINT_SAVEDEFCHK
//		LOG(em_FIL), "[%s] 無害疵歯抜け画像スキップ[LorFlg=%d]QUE空[SQL生成=%.2f％, DB登録=%.2f％]", my_sThreadName, nLowGrFlg, dRateSQL, dRateDB );
//#endif
//		
//		if( 1 == nLowGrFlg )	m_SqlInf[nRec].nImgLowGrDeleCnt[nMen]++;
//		else					m_SqlInf[nRec].nImgDeleCnt[nMen]++;
//		
//		return 9;		// 画像無し
//	}


	//// バッファフルしそうな為、無害Grは破棄
	// 有害Grは 極力破棄したくない為、余裕を持たせて無害Grを破棄する。
	if( 1 == nLowGrFlg && 1 != nFrameDefectMax &&
		(RATE_DEST_LOWGR_DB > dRateDB || RATE_DEST_LOWGR_SQL > dRateSQL)){

		m_SqlInf[nRec].nLowGrDeleCnt[nMen] ++;
		m_SqlInf[nRec].nTotalCnt[nMen] ++;
#ifdef DEBUG_PRINT_SAVEDEFCHK
		LOG(em_FIL), "[%s] 無害Grを破棄[LorFlg=%d][削除数:%d]QUE空[SQL生成=%.2f％, DB登録=%.2f％]", my_sThreadName, nLowGrFlg, m_SqlInf[nRec].nLowGrDeleCnt[nMen], dRateSQL, dRateDB );
#endif
		return -1;
	}


	//// バッファフルしそうな為、１フレーム１疵だけ保存にする
	if( 1 != nFrameDefectMax &&
		( RATE_ONLY_MAXDEF_DB > dRateDB || RATE_ONLY_MAXDEF_SQL > dRateSQL ) ) {

		m_SqlInf[nRec].nLowGrDeleCnt[nMen] ++;		// 有害Grの保存可能性は有るが、１フレームに１コは有害疵を保存しているため、無害カウントの方を加算する
		m_SqlInf[nRec].nTotalCnt[nMen] ++;
#ifdef DEBUG_PRINT_SAVEDEFCHK
		LOG(em_FIL), "[%s] 1フレーム1疵保存の為、スキップ[LorFlg=%d][削除数:%d]QUE空[SQL生成=%.2f％, DB登録=%.2f％]", my_sThreadName, nLowGrFlg, m_SqlInf[nRec].nLowGrDeleCnt[nMen], dRateSQL, dRateDB );
#endif
		return -1;
	}

	//// バッファフルしそうな為、有害疵な１フレーム１疵だけ保存にする
	if( 1 == nLowGrFlg && 1 != nFrameDefectMax &&
		( RATE_ONLY_MAXHIGH_DB > dRateDB || RATE_ONLY_MAXHIGH_SQL > dRateSQL ) ) {

		m_SqlInf[nRec].nLowGrDeleCnt[nMen] ++;		// 有害Grの保存可能性は有るが、１フレームに１コは有害疵を保存しているため、無害カウントの方を加算する
		m_SqlInf[nRec].nTotalCnt[nMen] ++;
#ifdef DEBUG_PRINT_SAVEDEFCHK
		LOG(em_FIL), "[%s] 1フレーム1有害疵保存の為、スキップ[LorFlg=%d][削除数:%d]QUE空[SQL生成=%.2f％, DB登録=%.2f％]", my_sThreadName, nLowGrFlg, m_SqlInf[nRec].nLowGrDeleCnt[nMen], dRateSQL, dRateDB );
#endif
		return -1;
	}

#ifdef DEBUG_PRINT_SAVEDEFCHK
		LOG(em_FIL), "[%s] 保存対象[LorFlg=%d]QUE空[SQL生成=%.2f％, DB登録=%.2f％]", my_sThreadName, nLowGrFlg, dRateSQL, dRateDB );
#endif
	return 0;
}

//==========================================
// 疵実績保存 上限確認
//------------------------------------------
// char const* cKizukenNo : 管理No
// int         nRec	      : 配列位置 0オリジン
// int         nMen       : 表or裏 (0:表 1:裏)
// int         nLowGrFlg  : 無害Grフラグ (1:無害疵Gr  0:有害疵Gr)
// 戻値        int        : 確認結果 (-1:保存NG 0:保存OK 9:画像のみ保存)
//------------------------------------------
int DbBaseManager::SaveMaxCheck(char const* cKizukenNo, int nMen, int nLowGrFlg)
{
	int nRec = GetRecSQLInf(cKizukenNo);
	if( -1 == nRec ) return -1;

	return SaveMaxCheck(nRec, nMen, nLowGrFlg);
}
int DbBaseManager::SaveMaxCheck(int nRec, int nMen, int nLowGrFlg)
{
	if( 0 > nRec || SIZE_SQLINF_NUM <= nRec ) return -1;

	//======================================================
	// 保存疵個数チェック

	// 最大保存件数に到達
	if( MAX_DETECT_SAVE <= (m_SqlInf[nRec].nSaveCnt[nMen]) ){
		if( !m_SqlInf[nRec].bMaxSave[nMen] ){
			m_SqlInf[nRec].bMaxSave[nMen] = true;

			if( ! m_SqlInf[nRec].bResult ) {			// 遅延で入ってきた場合の対策
				LOG(em_WAR), "[%s] <%s> %s 保存疵個数超え", my_sThreadName, DivNameManager::GetTorB(nMen), m_SqlInf[nRec].cKizukenNo );
				syslog(284, "[<%s> %s]", DivNameManager::GetTorB(nMen), m_SqlInf[nRec].cKizukenNo);
				mcls_pStatus->SetStatusQue("DB_SAVE_MAX_%d_ID", nMen+1, 1, false);	// 検査実績保存上限
			}
		}
		if( 1 == nLowGrFlg ) m_SqlInf[nRec].nLowGrDeleCnt[nMen] ++;
		else				 m_SqlInf[nRec].nDeleCnt[nMen] ++;
		m_SqlInf[nRec].nTotalCnt[nMen] ++;
#ifdef DEBUG_PRINT_SAVEMAXCHK
		LOG(em_FIL), "[保存フロー] 疵保存なし 最大保存上限到達[LowFlg:%d][保存疵個数:%d][保存画像個数:%d/%d]", nLowGrFlg, m_SqlInf[nRec].nSaveCnt[nMen], m_SqlInf[nRec].nSaveImgCnt[nMen], m_SqlInf[nRec].nSaveImgAllCnt[nMen]);
#endif
		return -1;

	// 無害Grの最大保存件数に到達
	}
	else if (MAX_DETECT_LOWGR_SAVE <= m_SqlInf[nRec].nSaveCnt[nMen] && 1 == nLowGrFlg) {
		// 無害Grの場合は、破棄
		m_SqlInf[nRec].nLowGrDeleCnt[nMen] ++;
		m_SqlInf[nRec].nTotalCnt[nMen] ++;
#ifdef DEBUG_PRINT_SAVEMAXCHK
		LOG(em_FIL), "[保存フロー] 疵保存なし 無害保存上限到達[LowFlg:%d][保存疵個数:%d][保存画像個数:%d/%d]", nLowGrFlg, m_SqlInf[nRec].nSaveCnt[nMen], m_SqlInf[nRec].nSaveImgCnt[nMen], m_SqlInf[nRec].nSaveImgAllCnt[nMen]);
#endif
		return -1;
	}
//
//	// 画像の最大保存件数に到達
//	} else if(MAX_DETECT_IMAGE_SAVE <= m_SqlInf[nRec].nSaveImgCnt[nMen] ) {
//		// 疵情報のみ登録
//#ifdef DEBUG_PRINT_SAVEMAXCHK
//		LOG(em_FIL), "[保存フロー] 疵情報のみ 画像保存上限到達[LowFlg:%d / MarkFlg:%d][保存疵個数:%d][保存画像個数:%d/%d]", nLowGrFlg, nMarkFlg, m_SqlInf[nRec].nSaveCnt[nMen], m_SqlInf[nRec].nSaveImgCnt[nMen], m_SqlInf[nRec].nSaveImgAllCnt[nMen]);
//#endif
//
//		if( 1 == nLowGrFlg )	m_SqlInf[nRec].nImgLowGrDeleCnt[nMen]++;
//		else					m_SqlInf[nRec].nImgDeleCnt[nMen]++;
//
//		return 9;
//
//	// 保存済疵個数が一定件数に到達
//	} else if(MAX_DEFECT_LOWER_IMG_SAVE <= m_SqlInf[nRec].nSaveCnt[nMen] && 1 == nLowGrFlg ){
//		// 無害Grは疵情報のみ登録
//#ifdef DEBUG_PRINT_SAVEMAXCHK
//		LOG(em_FIL), "[保存フロー] 疵情報のみ 無害画像保存上限到達[LowFlg:%d][保存疵個数:%d][保存画像個数:%d/%d]", nLowGrFlg, m_SqlInf[nRec].nSaveCnt[nMen], m_SqlInf[nRec].nSaveImgCnt[nMen], m_SqlInf[nRec].nSaveImgAllCnt[nMen]);
//#endif
//
//		m_SqlInf[nRec].nImgLowGrDeleCnt[nMen]++;
//		return 9;
//	}
#ifdef DEBUG_PRINT_SAVEMAXCHK
	LOG(em_FIL), "[保存フロー] 疵画像保存[LowFlg:%d][保存疵個数:%d][保存画像個数:%d/%d]", nLowGrFlg, m_SqlInf[nRec].nSaveCnt[nMen], m_SqlInf[nRec].nSaveImgCnt[nMen], m_SqlInf[nRec].nSaveImgAllCnt[nMen]);
#endif
	return 0;
}
//------------------------------------------
// 疵実績保存 上限リセット
// int nRec	配列位置 0オリジン
//------------------------------------------
void DbBaseManager::SaveMaxReset(int nRec)
{
	//// 上限到達してた？
	for(int ii=0; ii<NUM_MEN; ii++) {
		if( true == m_SqlInf[nRec].bMaxSave[ii] ) {
			mcls_pStatus->SetStatusQue("DB_SAVE_MAX_%d_ID", ii+1, 0, false);	// 検査実績保存上限 解除
		}
	}
}

//------------------------------------------
// 疵画像保存数リセット
// char const*	cKizukenNo	: 管理No
// int			nTorB		: 表裏区分(0:表 1:裏 2:両面)
// 戻値			int			: リセット結果 (-1:リセットNG 0:リセットOK)
//------------------------------------------
int DbBaseManager::ResetInf_SaveImgCnt(char const* cKizukenNo, int nTorB)
{
	int nRec = GetRecSQLInf(cKizukenNo);
	if( -1 == nRec ) return -1;

	if(0 == nTorB)						m_SqlInf[nRec].nSaveImgCnt[0] = 0;
	else if(1 == nTorB)					m_SqlInf[nRec].nSaveImgCnt[1] = 0;
	else {
		for(int ii=0; ii<NUM_MEN; ii++) m_SqlInf[nRec].nSaveImgCnt[ii] = 0;
	}
	return 0;
}
