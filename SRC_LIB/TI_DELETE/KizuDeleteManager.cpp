#include "StdAfx.h"
#include "KizuDeleteManager.h"

// メモリーリーク検出 ----->>>
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif
// <<<------------------------


// //////////////////////////////////////////////////
// 普通のメソッド
// //////////////////////////////////////////////////

//------------------------------------------
// コンストラクタ
// int id 自分のID (1オリジン)
// int* waittime 待ち時間 [ms]
// HANDLE evEndCycle 終了シグナル
// HANDLE evEndSizeMin 終了シグナル
// CString cSession 自分の設定が定義されているiniファイルのセッション
// CString cOdbcSession ODBC設定セッション
//------------------------------------------
KizuDeleteManager::KizuDeleteManager(int id, int* nWaitTime, HANDLE evEndCycle, HANDLE evEndSizeMin, CString cSession, CString cOdbcSession) :
my_nID(id),
my_pnWaitTime(nWaitTime),
my_evEndCycle(evEndCycle),
my_evEndSizeMin(evEndSizeMin),
my_cSession(cSession),
my_cOdbcSession(cOdbcSession),
my_bIsExec_Cycle(false),
my_bIsExec_SizeMin(false),
my_bStopFlg(false),
my_nTranSizeMin(0),
my_nTranSizeNow(0),
mcls_pLock(NULL),
mcls_pLog(NULL)
{
	// イベント生成
	my_evCycle		= CreateEvent(NULL, FALSE, FALSE, NULL);
	my_evSizeMin	= CreateEvent(NULL, FALSE, FALSE, NULL);
	my_evExecFast	= CreateEvent(NULL, FALSE, FALSE, NULL);
}

//------------------------------------------
// デストラクタ
//------------------------------------------
KizuDeleteManager::~KizuDeleteManager(void)
{
	// イベント破棄
	CloseHandle(my_evCycle);
	CloseHandle(my_evSizeMin);
	CloseHandle(my_evExecFast);
}

//------------------------------------------
// スレッド
//------------------------------------------
UINT WINAPI KizuDeleteManager::MainThread(LPVOID param)
{
	////// メインインスタンス取り出し
	KizuDeleteManager *p = (KizuDeleteManager *)param;
	p->SetTid();								// 自スレッドIDセット

	// WaitForMultipleObjectsイベント
	HANDLE	hArray[] = {p->my_evStop,
						p->my_evCycle,
						p->my_evSizeMin, 
						p->my_evExecFast
						};
	enum {	EV_STOP = WAIT_OBJECT_0,		// スレッド終了
			EV_CYCLE,						// 定周期実行イベント
			EV_SIZEMIN,						// DB容量低下実行イベント
			EV_EXEC_FAST,					// 初回実行
			EV_END	};
	
	// メッセージキュー読み込み
	pLOG(em_MSG), "[KD] [%s] スレッド<%d>開始 [0x%X]", p->my_cSession, p->my_nID, p->GetTid());
	while (1) {
		DWORD ret = WaitForMultipleObjects(EV_END, hArray, FALSE, INFINITE);
		switch (ret) {

//-----------------------------------------------------------------------------------------------
		case EV_STOP:												// スレッド終了
			pLOG(em_MSG), "[KD] [%s] スレッド<%d>終了 [0x%X]", p->my_cSession, p->my_nID, p->GetTid());
			return 0;

//-----------------------------------------------------------------------------------------------
		case EV_CYCLE:												// 定周期実行イベント
			pLOG(em_INF), "[KD] [%s] スレッド<%d> 定周期実行開始", p->my_cSession, p->my_nID);
			p->my_bIsExec_Cycle = true;

			// --------------------->>>
			// システム毎の独自処理実行
			p->ExecCycle();															// 任意の処理
			// <<<---------------------
			
			if(0 < p->my_Param.nLogSaveDay) {
				p->Delete_Log(p->my_Param.nLogSaveDay);								// ログ削除処理
			}
			p->Exec_Shrink();														// トランザクション圧縮処理
			
			p->my_bIsExec_Cycle = false;
			pLOG(em_INF), "[KD] [%s] スレッド<%d> 定周期実行終了", p->my_cSession, p->my_nID);

			// 定周期実行が終了した為、通知
			SetEvent(p->my_evEndCycle);
			break;

//-----------------------------------------------------------------------------------------------
		case EV_SIZEMIN:											// DB容量低下実行イベント
			pLOG(em_INF), "[KD] [%s] スレッド<%d> DB容量低下実行開始", p->my_cSession, p->my_nID);
			p->my_bIsExec_SizeMin = true;
				
			// --------------------->>>
			// システム毎の独自処理実行
			p->ExecSizeMin(); 														// 任意の処理
			// <<<---------------------

			p->Exec_Shrink();														// トランザクション圧縮処理

			p->my_bIsExec_SizeMin = false;
			pLOG(em_INF), "[KD] [%s] スレッド<%d> DB容量低下実行終了", p->my_cSession, p->my_nID);

			// 容量低下実行が終了した為、再度 容量チェック通知
			SetEvent(p->my_evEndSizeMin);
			break;

//-----------------------------------------------------------------------------------------------
		case EV_EXEC_FAST:											// 初回実行
			pLOG(em_INF), "[KD] [%s] スレッド<%d> 初回実行開始", p->my_cSession, p->my_nID);
			p->my_bIsExec_SizeMin = true;
				
			// --------------------->>>
			// システム毎の独自処理実行
			p->ExecFast(); 															// 任意の処理
			// <<<---------------------

			p->Exec_Shrink();														// トランザクション圧縮処理

			p->my_bIsExec_SizeMin = false;
			pLOG(em_INF), "[KD] [%s] スレッド<%d> 初回実行終了", p->my_cSession, p->my_nID);

			// 容量低下実行が終了した為、再度 容量チェック通知
			SetEvent(p->my_evEndSizeMin);
			break;

//-----------------------------------------------------------------------------------------------
		default :
			// ありえない！！
			break;
		}
	}
	return 0;
}


// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// よく使用そうなメソッド定義 (共通的に使えるように作っておく事)
// 取得関連

//------------------------------------------
// 指定テーブルの最古のキーを取得
// char const *tbl 削除テーブル
// char const *keyName 主キー項目 (項目の型は、文字列型)
// int size 取得する文字数
// char *keyVal 取得した結果
//------------------------------------------
bool KizuDeleteManager::Select_OldKey(char const *tbl, char const *keyName, int size, char *keyVal)
{
	CString sql;
	memset(keyVal, 0x00, size);
	//// ODBC接続クラス インスタンス生成
	OdbcBase clsDB;
	if( ! clsDB.Connect(TASKINI_NAME, my_cOdbcSession)) {
		// データベース接続エラー
		LOG(em_ERR), "[KD] [%s] DB接続エラー", my_cSession);
		syslog(SYSNO_DB_CONNECT_ERR, "");
		return false;
	}
	//// 最古のキー取得
	sql.Format("SELECT ISNULL( Min(%s), '') FROM %s ", keyName, tbl);
	if( !clsDB.ExecuteDirect(sql)) {
		LOG(em_ERR), "[KD] [%s] スレッド<%d> キー取得 [%s]", my_cSession, my_nID, sql);
		syslog(SYSNO_DB_EXECUTEDIRECT_ERR, "[キー取得]");
		return false;
	}
	int sqlrc = clsDB.Fetch();
	if (sqlrc == KIZUODBC_FETCH_OK) {						// データ有り
		clsDB.GetData(1, keyVal, size);
		if( 1 >= strlen(keyVal) ) return false;				// データ無し 
	} else if( sqlrc == KIZUODBC_FETCH_NODATE ) {			// データ無し
		return false;	// ありえない
	} else {								// エラー
		syslog(SYSNO_DB_FETCH_ERR, "[主キー取得処理]");
		return false;
	}
	return true;
}
bool KizuDeleteManager::Select_OldKey(char const *tbl, char const *keyName, char const *keySort, int size, char *keyVal)
{
	CString sql;
	memset(keyVal, 0x00, size);
	//// ODBC接続クラス インスタンス生成
	OdbcBase clsDB;
	if( ! clsDB.Connect(TASKINI_NAME, my_cOdbcSession)) {
		// データベース接続エラー
		LOG(em_ERR), "[KD] [%s] DB接続エラー", my_cSession);
		syslog(SYSNO_DB_CONNECT_ERR, "");
		return false;
	}
	//// 最古のキー取得
	sql.Format("SELECT TOP 1 %s FROM %s ORDER BY %s ASC", keyName, tbl, keySort);
	if( !clsDB.ExecuteDirect(sql)) {
		LOG(em_ERR), "[KD] [%s] スレッド<%d> キー取得 [%s]", my_cSession, my_nID, sql);
		syslog(SYSNO_DB_EXECUTEDIRECT_ERR, "[キー取得]");
		return false;
	}
	int sqlrc = clsDB.Fetch();
	if (sqlrc == KIZUODBC_FETCH_OK) {						// データ有り
		clsDB.GetData(1, keyVal, size);
		if( 1 >= strlen(keyVal) ) return false;				// データ無し 
	} else if( sqlrc == KIZUODBC_FETCH_NODATE ) {			// データ無し
		return false;	// ありえない
	} else {								// エラー
		syslog(SYSNO_DB_FETCH_ERR, "[主キー取得処理]");
		return false;
	}
	return true;
}


//------------------------------------------
// 指定テーブルの最古のキーを取得
// char const *tbl 削除テーブル
// char const *keyName 主キー項目 (項目の型は、数値型)
// int *keyVal 取得した結果
//------------------------------------------
bool KizuDeleteManager::Select_OldKey(char const *tbl, char const *keyName, int *keyVal)
{
	CString sql;
	*keyVal = 0;
	//// ODBC接続クラス インスタンス生成
	OdbcBase clsDB;
	if( ! clsDB.Connect(TASKINI_NAME, my_cOdbcSession)) {
		// データベース接続エラー
		LOG(em_ERR), "[KD] [%s] DB接続エラー", my_cSession);
		syslog(SYSNO_DB_CONNECT_ERR, "");
		return false;
	}
	//// 最古のキー取得
	sql.Format("SELECT ISNULL( Min(%s), '') FROM %s ", keyName, tbl);
	if( !clsDB.ExecuteDirect(sql)) {
		LOG(em_ERR), "[KD] [%s] スレッド<%d> キー取得 [%s]", my_cSession, my_nID, sql);
		syslog(SYSNO_DB_EXECUTEDIRECT_ERR, "[キー取得]");
		return false;
	}
	int sqlrc = clsDB.Fetch();
	if (sqlrc == KIZUODBC_FETCH_OK) {						// データ有り
		clsDB.GetData(1, keyVal);
	} else if( sqlrc == KIZUODBC_FETCH_NODATE ) {			// データ無し
		return false;	// ありえない
	} else {								// エラー
		syslog(SYSNO_DB_FETCH_ERR, "[主キー取得処理]");
		return false;
	}
	return true;
}
bool KizuDeleteManager::Select_OldKey(char const *tbl, char const *keyName, char const *keySort, int *keyVal)
{
	CString sql;
	*keyVal = 0;
	//// ODBC接続クラス インスタンス生成
	OdbcBase clsDB;
	if( ! clsDB.Connect(TASKINI_NAME, my_cOdbcSession)) {
		// データベース接続エラー
		LOG(em_ERR), "[KD] [%s] DB接続エラー", my_cSession);
		syslog(SYSNO_DB_CONNECT_ERR, "");
		return false;
	}
	//// 最古のキー取得
	sql.Format("SELECT TOP 1 %s FROM %s ORDER BY %s ASC", keyName, tbl, keySort);
	if( !clsDB.ExecuteDirect(sql)) {
		LOG(em_ERR), "[KD] [%s] スレッド<%d> キー取得 [%s]", my_cSession, my_nID, sql);
		syslog(SYSNO_DB_EXECUTEDIRECT_ERR, "[キー取得]");
		return false;
	}
	int sqlrc = clsDB.Fetch();
	if (sqlrc == KIZUODBC_FETCH_OK) {						// データ有り
		clsDB.GetData(1, keyVal);
	} else if( sqlrc == KIZUODBC_FETCH_NODATE ) {			// データ無し
		return false;	// ありえない
	} else {								// エラー
		syslog(SYSNO_DB_FETCH_ERR, "[主キー取得処理]");
		return false;
	}
	return true;
}
//------------------------------------------
// 指定テーブルのサブキーを取得
// char const *tbl 削除テーブル
// char const *keyName 主キー項目 (項目の型は、文字列型)
// char const *subName サブ主キー項目 (項目の型は、何でもOK)
// char const *keyVal 主キー
// int size 取得する文字数
// char *subVal 取得した結果
//------------------------------------------
bool KizuDeleteManager::Select_SubKey(char const *tbl, char const *keyName, char const *subName, char const *keyVal, int size, char *subVal)
{
	CString sql;
	memset(subVal, 0x00, size);

	//// ODBC接続クラス インスタンス生成
	OdbcBase clsDB;
	if( ! clsDB.Connect(TASKINI_NAME, my_cOdbcSession)) {
		// データベース接続エラー
		LOG(em_ERR), "[KD] [%s] DB接続エラー", my_cSession);
		syslog(SYSNO_DB_CONNECT_ERR, "");
		return false;
	}

	//// 最古のキー取得
	sql.Format("SELECT ISNULL( %s, '') FROM %s WHERE %s='%s'", subName, tbl, keyName, keyVal);
	if( !clsDB.ExecuteDirect(sql)) {
		LOG(em_ERR), "[KD] [%s] スレッド<%d> キー取得 [%s]", my_cSession, my_nID, sql);
		syslog(SYSNO_DB_EXECUTEDIRECT_ERR, "[キー取得]");
		return false;
	}
	int sqlrc = clsDB.Fetch();
	if (sqlrc == KIZUODBC_FETCH_OK) {						// データ有り
		clsDB.GetData(1, subVal, size);
		if( 1 >= strlen(keyVal) ) return false;				// データ無し 
	} else if( sqlrc == KIZUODBC_FETCH_NODATE ) {			// データ無し
		return false;	// ありえない
	} else {								// エラー
		syslog(SYSNO_DB_FETCH_ERR, "[サブキー取得処理]");
		return false;
	}
	return true;
}
bool KizuDeleteManager::Select_SubKey(char const *tbl, char const *keyName, char const *subName, char const *keyVal, int *subVal)
{
	CString sql;
	*subVal = 0;

	//// ODBC接続クラス インスタンス生成
	OdbcBase clsDB;
	if( ! clsDB.Connect(TASKINI_NAME, my_cOdbcSession)) {
		// データベース接続エラー
		LOG(em_ERR), "[KD] [%s] DB接続エラー", my_cSession);
		syslog(SYSNO_DB_CONNECT_ERR, "");
		return false;
	}

	//// 最古のキー取得
	sql.Format("SELECT ISNULL( %s, '') FROM %s WHERE %s='%s'", subName, tbl, keyName, keyVal);
	if( !clsDB.ExecuteDirect(sql)) {
		LOG(em_ERR), "[KD] [%s] スレッド<%d> キー取得 [%s]", my_cSession, my_nID, sql);
		syslog(SYSNO_DB_EXECUTEDIRECT_ERR, "[キー取得]");
		return false;
	}
	int sqlrc = clsDB.Fetch();
	if (sqlrc == KIZUODBC_FETCH_OK) {						// データ有り
		clsDB.GetData(1, subVal);
	} else if( sqlrc == KIZUODBC_FETCH_NODATE ) {			// データ無し
		return false;	// ありえない
	} else {								// エラー
		syslog(SYSNO_DB_FETCH_ERR, "[サブキー取得処理]");
		return false;
	}
	return true;
}


// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// データ容量管理

//------------------------------------------
// トランザクションログ 圧縮
//    (実績削除するとトランザクションログがすぐ巨大化する為の処置)
//------------------------------------------
bool KizuDeleteManager::Exec_Shrink()
{
	CString sql;					// SQL文

	//// ODBC接続クラス インスタンス生成
	OdbcBase clsDB;
	if( ! clsDB.Connect(TASKINI_NAME, my_cOdbcSession)) {
		// データベース接続エラー
		LOG(em_ERR), "[KD] [%s] DB接続エラー", my_cSession);
		syslog(SYSNO_DB_CONNECT_ERR, "");
		return false;
	}

	//// SQL文作成
	if( ! clsDB.ExecuteDirect_ShrinkFile() ) {
		LOG(em_ERR), "[KD] [%s] トランザクション圧縮処理失敗", my_cSession);
		syslog(SYSNO_DB_EXECUTEDIRECT_ERR, "[トランザクション圧縮処理]");
		return false;
	}


	//// フェッチ
	int now;							// 現在値
	int min;							// 最小値
	int sqlrc = clsDB.Fetch();
	if (sqlrc == KIZUODBC_FETCH_OK) {						// データ有り
		clsDB.GetData(3, &now);
		clsDB.GetData(4, &min);
		// 8K単位なので、M単位に変換
		now = now * 8 /1024;
		min = min * 8 /1024;
	} else if( sqlrc == KIZUODBC_FETCH_NODATE ) {			// データ無し
		//ありえない
	} else {												// エラー
		LOG(em_ERR), "[KD] [%s] トランザクション圧縮処理失敗(フェッチエラー)", my_cSession);
		syslog(SYSNO_DB_FETCH_ERR, "[トランザクション圧縮処理]");
		return false;
	}
//	clsDB.TranCommit(); 20081208

	//// トランザクションログサイズセット
	my_nTranSizeMin = min;
	my_nTranSizeNow = now;
	
	if(now != min) {
		LOG(em_MSG), "[KD] [%s] スレッド<%d> トランザクション圧縮完了 [%d]/[%d]", my_cSession, my_nID, now, min);
	}
	return true;
}

//------------------------------------------
// Log削除処理
// int nDelLogDay ログの保存日数
//------------------------------------------
bool KizuDeleteManager::Delete_Log(int nDelLogDay)
{
	// ログ削除
	return Delete_Day(DB_LOG_INF, "登録日付", nDelLogDay);
}


//------------------------------------------
// 指定日数以前データを一括削除
// char const *tbl 削除テーブル
// char const *keyDayName キー列名 (日付型：通常 登録日付)
// int day 削除日数 (1だと今日基準の1日前のデータを削除)
//------------------------------------------
bool KizuDeleteManager::Delete_Day(char const *tbl, char const *keyDayName, int day)
{
	CString sql;					// SQL文

	//// ODBC接続クラス インスタンス生成
	OdbcBase clsDB;
	if( ! clsDB.Connect(TASKINI_NAME, my_cOdbcSession)) {
		// データベース接続エラー
		LOG(em_ERR), "[KD] [%s] DB接続エラー", my_cSession);
		syslog(SYSNO_DB_CONNECT_ERR, "");
		return false;
	}

	//// SQL文作成(ログの一定期間超過分を削除する時等に使用する)
	SYSTEMTIME STime;
	GetLocalTime(&STime);
	sql.Format("DELETE FROM %s WHERE %s + %d < '%d/%d/%d'", tbl, keyDayName, day, STime.wYear, STime.wMonth, STime.wDay);
	if( !clsDB.ExecuteDirect(sql) ) {
		LOG(em_ERR), "[KD] [%s] %s削除失敗", my_cSession, tbl);
		syslog(SYSNO_DB_EXECUTEDIRECT_ERR, "[%s]", tbl);
		return false;
	}
//	clsDB.TranCommit(); 20081208
	LOG(em_MSG), "[KD] [%s] スレッド<%d> %s削除完了", my_cSession, my_nID, tbl );
	return true;
}

//------------------------------------------
// 指定日数以前データを１日単位で削除
// char const *tbl 削除テーブル
// char const *keyDayName キー列名 (日付型：通常 登録日付)
// int day 削除日数 (1だと今日基準の1日前のデータを削除)
//------------------------------------------
bool KizuDeleteManager::Delete_Day2(char const *tbl, char const *keyDayName, int day)
{
	CString sql;					// SQL文
	char selectKeyVal[128];		// 現在のキー
	int nExecCnt = 0;			// 実行回数

	//// ODBC接続クラス インスタンス生成
	OdbcBase clsDB;
	if( ! clsDB.Connect(TASKINI_NAME, my_cOdbcSession)) {
		// データベース接続エラー
		LOG(em_ERR), "[KD] [%s] DB接続エラー", my_cSession);
		syslog(SYSNO_DB_CONNECT_ERR, "");
		return false;
	}

	//// SQL文作成(日付の１日単位での削除)。当日を除いて 削除日数より古いものを削除
	SYSTEMTIME STime;
	GetLocalTime(&STime);
	while(true) {
		sql.Format("SELECT TOP 1 CONVERT(varchar, %s,111) FROM %s WHERE %s + %d < '%d/%d/%d' ORDER BY %s ASC", 
			keyDayName, tbl, keyDayName, day, STime.wYear, STime.wMonth, STime.wDay, keyDayName);
		if( !clsDB.ExecuteDirect(sql) ) {
			LOG(em_ERR), "[KD] [%s] %s削除失敗", my_cSession, tbl);
			syslog(SYSNO_DB_EXECUTEDIRECT_ERR, "[%s]", tbl);
			return false;
		}
		int sqlrc = clsDB.Fetch();
		if (sqlrc == KIZUODBC_FETCH_OK) {						// データ有り
			clsDB.GetData(1, selectKeyVal, sizeof(selectKeyVal));
			if( 1 > strlen(selectKeyVal) ) break;				// データ無し 
		} else if( sqlrc == KIZUODBC_FETCH_NODATE ) {			// データ無し
			break;	// ありえない
		} else {								// エラー
			syslog(SYSNO_DB_FETCH_ERR, "[古い日付削除処理]");
			return false;
		}
	
		// 削除
		LOG(em_MSG), "[KD] [%s] スレッド<%d> 実績削除 [%s] [%s] [%sまで]", my_cSession, my_nID, tbl, keyDayName, selectKeyVal);
		sql.Format("DELETE FROM %s WHERE %s<='%s 23:59:59'", tbl, keyDayName, selectKeyVal);
		if( !clsDB.ExecuteDirect(sql) ) {
			LOG(em_ERR), "[KD] [%s] %s日付少量削除失敗", my_cSession, tbl);
			syslog(SYSNO_DB_EXECUTEDIRECT_ERR, "[DB日付少量削除処理]");
			return false;
		}

		//// 一時停止
		Sleep(*my_pnWaitTime);
		if(my_bStopFlg) return true; // 終了？

		nExecCnt ++;
		if(10 <= nExecCnt) return true;	// 10日分を上限として打ち切り。また次回に削除する。消し忘れで大量に削除する場合の対策
	}

	LOG(em_MSG), "[KD] [%s] スレッド<%d> %s削除完了", my_cSession, my_nID, tbl );
	return true;
}

//------------------------------------------
// 指定件数以外のデータ削除
// char const *tbl 削除テーブル
// char const *keyName 主キー項目 (項目の型は、文字列型)
// char const *subName サブ主キー項目 (項目の型は、何でもOK)
// int delCnt 一度に削除する数
// int nMaxNum 保存上限数
//------------------------------------------
bool KizuDeleteManager::Delete_Cnt(char const *tbl, char const *keyName, char const *subName, int delCnt, int nMaxNum)
{
	CString sql;						// SQL文
	char cKey[256];		// キー
	memset( cKey, 0x00, sizeof(cKey));

	//// ODBC接続クラス インスタンス生成
	OdbcBase clsDB;
	if( ! clsDB.Connect(TASKINI_NAME, my_cOdbcSession)) {
		// データベース接続エラー
		LOG(em_ERR), "[KD] [%s] DB接続エラー", my_cSession);
		syslog(SYSNO_DB_CONNECT_ERR, "");
		return false;
	}

	sql.Format("SELECT COUNT(*), Min(a.%s) FROM (SELECT Top %d %s FROM %s GROUP BY %s ORDER BY %s DESC) a",
		keyName, (nMaxNum+1), keyName, tbl, keyName, keyName, keyName);
	if( !clsDB.ExecuteDirect(sql)) {
		syslog(SYSNO_DB_EXECUTEDIRECT_ERR, "[KD] [%s] [指定件数保持カウント失敗 %s]", my_cSession, tbl);
		return false;
	}

	int sqlrc = clsDB.Fetch();
	int cnt = 0;
	if (sqlrc == KIZUODBC_FETCH_OK) {				// データ有り
		clsDB.GetData(1, &cnt);
		clsDB.GetData(2, cKey, sizeof(cKey));
	} else if( sqlrc == KIZUODBC_FETCH_NODATE ) {	// データ無し
	} else {										// エラー
		syslog(SYSNO_DB_FETCH_ERR, "[KD] [%s] [削除対象コイル読込み失敗 %s]", my_cSession, tbl);
		return false;
	}

	// 指定件数件以上？
	if ( cnt >= (nMaxNum+1)) {
		// 指定キーより古い実績を少量削除処理
		if( ! Delete_OldInf(tbl, keyName, subName, delCnt, cKey) ) {
			LOG(em_ERR), "[KD] [%s] %s削除失敗 (%s:%s)", my_cSession, tbl, keyName, cKey);
			return false;	// 失敗
		} else {
			LOG(em_MSG), "[KD] [%s] 指定キーより古い実績を少量削除処理[%s] (%s:%s)", my_cSession, tbl, keyName, cKey);
		}
	}

	LOG(em_MSG), "[KD] [%s] スレッド<%d> %削除完了 (%s:%s)", my_cSession, my_nID, tbl, keyName, cKey );
	return true;
}

// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// INF

//------------------------------------------
// 指定主キーの 一括削除/少量削除 エントリー
// DEL_TBL const *tbl 削除テーブル定義
// char *keyVal 主キーの値 (通常 疵検管理No) 
//------------------------------------------
bool KizuDeleteManager::Delete_Inf(DEL_TBL const *dtbl, char const *keyVal)		
{
	if( 0 == dtbl->nDelRow ) {			// 一括削除開始
		return Delete_Inf(dtbl->cDelTable, dtbl->cKeyName, keyVal);
	} else {							// 少量削除開始
		return Delete_Inf(dtbl->cDelTable, dtbl->cKeyName, dtbl->cSubName, dtbl->nDelRow, keyVal);
	}
}

//------------------------------------------
// 指定主キーの 一括削除 (1コイル分)
// char const *tbl 削除テーブル
// char *keyName 主キー項目 (項目の型は、文字列型)
// char *keyVal 主キーの値 (通常 疵検管理No) 
//------------------------------------------
bool KizuDeleteManager::Delete_Inf(char const *tbl, char const *keyName, char const *keyVal)
{
	CString sql;

	//// ODBC接続クラス インスタンス生成
	OdbcBase clsDB;
	if( ! clsDB.Connect(TASKINI_NAME, my_cOdbcSession)) {
		// データベース接続エラー
		LOG(em_ERR), "[KD] [%s] DB接続エラー", my_cSession);
		syslog(SYSNO_DB_CONNECT_ERR, "");
		return false;
	}

	////// 実績削除
	LOG(em_MSG), "[KD] [%s] スレッド<%d> 実績削除 [%s] [%s]", my_cSession, my_nID, tbl, keyVal);
	sql.Format("DELETE FROM %s WHERE %s='%s'", tbl, keyName, keyVal);
	if( !clsDB.ExecuteDirect(sql) ) {
		LOG(em_ERR), "[KD] [%s] スレッド<%d> 実績削除異常 [%s] [%s]", my_cSession, my_nID, tbl, sql);
		syslog(SYSNO_DB_EXECUTEDIRECT_ERR, "[DB実績削除処理]");
		return false;
	}
//	clsDB.TranCommit(); 20081208
	return true;
}

//------------------------------------------
// 指定主キーの 少量削除
// char const *tbl 削除テーブル
// char const *keyName 主キー項目 (項目の型は、文字列型)
// char const *subName サブ主キー項目 (項目の型は、何でもOK)
// int delCnt １回の削除件数
// char const *keyVal 主キーの値 (通常 疵検管理No) 
//------------------------------------------
bool KizuDeleteManager::Delete_Inf(char const *tbl, char const *keyName, char const *subName, int delCnt, char const *keyVal)
{
	CString sql;
	int nowCnt;				// 現在の削除位置

	//// ODBC接続クラス インスタンス生成
	OdbcBase clsDB;
	if( ! clsDB.Connect(TASKINI_NAME, my_cOdbcSession)) {
		// データベース接続エラー
		LOG(em_ERR), "[KD] [%s] DB接続エラー", my_cSession);
		syslog(SYSNO_DB_CONNECT_ERR, "");
		return false;
	}

	////// 実績削除 (ちょっとづつ削除)
	while(true) {
		// 削除位置決定 (必ず TOP を使用する場合は、Order句をつける事。順番が不定となる)
		sql.Format("SELECT ISNULL( Max(a.%s), -1) FROM (SELECT TOP %d %s FROM %s WHERE %s = '%s' ORDER BY %s ASC) a",
			subName, delCnt, subName, tbl, keyName, keyVal, subName);
		if( !clsDB.ExecuteDirect(sql)) {
			LOG(em_ERR), "[KD] [%s] スレッド<%d> DB実績少量削除処理 [%s]", my_cSession, my_nID, sql);
			syslog(SYSNO_DB_EXECUTEDIRECT_ERR, "[DB実績少量削除処理]");
			return false;
		}
		int sqlrc = clsDB.Fetch();
		if (sqlrc == KIZUODBC_FETCH_OK) {						// データ有り
			clsDB.GetData(1, &nowCnt);
			if( -1 == nowCnt ) break;					// データ無し 
		} else if( sqlrc == KIZUODBC_FETCH_NODATE ) {			// データ無し
			break;	// ありえない	
		} else {								// エラー
			syslog(SYSNO_DB_FETCH_ERR, "[DB実績少量削除処理]");
			return false;
		}
		
		// 削除
		LOG(em_MSG), "[KD] [%s] スレッド<%d> 実績削除 [%s] [%s] [%dまで]", my_cSession, my_nID, tbl, keyVal, nowCnt);
		sql.Format("DELETE FROM %s WHERE %s='%s' AND %s<=%d", tbl, keyName, keyVal, subName, nowCnt);
		if( !clsDB.ExecuteDirect(sql) ) {
			LOG(em_ERR), "[KD] [%s] スレッド<%d> 実績削除異常 [%s] [%s]", my_cSession, my_nID, tbl, sql);
			syslog(SYSNO_DB_EXECUTEDIRECT_ERR, "[DB実績少量削除処理]");
			return false;
		}
		//clsDB.TranCommit(); 20081208


		//// 一時停止
		Sleep(*my_pnWaitTime);
		if(my_bStopFlg) return true; // 終了？
	}
	return true;
}

// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// OLD

//------------------------------------------
// 一括削除/少量削除 エントリー (指定キーより古い実績を削除処理)
// DEL_TBL const *tbl 削除テーブル定義
// char *keyVal 主キーの値 (通常 疵検管理No) 
//------------------------------------------
bool KizuDeleteManager::Delete_OldInf(DEL_TBL const *dtbl, char const *keyVal)		
{
	if( 0 == dtbl->nDelRow ) {			// 一括削除開始
		return Delete_OldInf(dtbl->cDelTable, dtbl->cKeyName, NULL, 0, keyVal);
	} else {							// 少量削除開始
		return Delete_OldInf(dtbl->cDelTable, dtbl->cKeyName, dtbl->cSubName, dtbl->nDelRow, keyVal);
	}
}

//------------------------------------------
// 指定キーより古い実績を一括削除
// char const *tbl 削除テーブル
// char const *keyName 主キー項目 (項目の型は、文字列型)
// char const *keyVal 主キーの値 (通常 疵検管理No) 
// int delCnt １回の削除件数
//------------------------------------------
bool KizuDeleteManager::Delete_OldInf(char const *tbl, char const *keyName, char const *keyVal)
{
	return Delete_OldInf(tbl, keyName, NULL, 0, keyVal);
}

//------------------------------------------
// 指定キーより古い実績を少量削除
//	※ サブ主キー or 1回の削除件数が無ければ、主キー単位で古い実績を削除していく
// char const *tbl 削除テーブル
// char const *keyName 主キー項目 (項目の型は、文字列型)
// char const *subName サブ主キー項目 (項目の型は、何でもOK)
// int delCnt １回の削除件数
// char const *keyVal 主キーの値 (通常 疵検管理No) 
//------------------------------------------
bool KizuDeleteManager::Delete_OldInf(char const *tbl, char const *keyName, char const *subName, int delCnt, char const *keyVal)
{
	CString sql;
	char selectKeyVal[128];		// 現在のキー
	int nExecCnt = 0;			// 実行回数

	//// ODBC接続クラス インスタンス生成
	OdbcBase clsDB;
	if( ! clsDB.Connect(TASKINI_NAME, my_cOdbcSession)) {
		// データベース接続エラー
		LOG(em_ERR), "[KD] [%s] DB接続エラー", my_cSession);
		syslog(SYSNO_DB_CONNECT_ERR, "");
		return false;
	}

	////// 実績削除 (少量削除)
	while(true) {
		//// 削除対象コイル決定
		sql.Format("SELECT ISNULL( Min(%s), '') FROM %s WHERE %s<'%s'", keyName, tbl, keyName, keyVal);
		if( !clsDB.ExecuteDirect(sql)) {
			LOG(em_ERR), "[KD] [%s] スレッド<%d> 古い実績削除処理 [%s]", my_cSession, my_nID, sql);
			syslog(SYSNO_DB_EXECUTEDIRECT_ERR, "[古い実績削除処理]");
			return false;
		}
		int sqlrc = clsDB.Fetch();
		if (sqlrc == KIZUODBC_FETCH_OK) {						// データ有り
			clsDB.GetData(1, selectKeyVal, sizeof(selectKeyVal));
			if( 1 > strlen(selectKeyVal) ) break;				// データ無し 
		} else if( sqlrc == KIZUODBC_FETCH_NODATE ) {			// データ無し
			break;	// ありえない
		} else {								// エラー
			syslog(SYSNO_DB_FETCH_ERR, "[古い実績削除処理]");
			return false;
		}

		//// 対象疵検管理Noの実績を削除
		if(0 == delCnt || 1>strlen(subName) ) {
			// 指定主キーのみ一括削除
			if( ! Delete_Inf(tbl, keyName, selectKeyVal ) ) {
				return false;
			}
		} else {
			// 少量削除
			if( ! Delete_Inf(tbl, keyName, subName, delCnt, selectKeyVal ) ) {
				return false;
			}
		}


		//// 一時停止
		Sleep(*my_pnWaitTime);
		if(my_bStopFlg) return true;	// 終了？
		nExecCnt ++;
		if(10 <= nExecCnt) return true;	// 10回位を上限として打ち切り。また次回に削除する。消し忘れで大量に削除する場合の対策
	}
	return true;
}
