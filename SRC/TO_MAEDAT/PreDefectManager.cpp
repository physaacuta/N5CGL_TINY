#include "StdAfx.h"
#include "PreDefectManager.h"

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
PreDefectManager::PreDefectManager():
ThreadManager("PreDefectManager")
{
	// シグナル生成
	m_evGetData = CreateEvent(NULL, FALSE, FALSE, NULL);
	m_evTimeCycle = CreateWaitableTimer(NULL, FALSE, NULL);

}

//------------------------------------------
// デストラクタ
//------------------------------------------
PreDefectManager::~PreDefectManager(void)
{
	// イベント開放
	CloseHandle( m_evGetData );
	CloseHandle( m_evTimeCycle );
}

//------------------------------------------
// スレッド開始前処理 (スレッド実行直後(シグナルセット前)にコールバック)
//------------------------------------------
int PreDefectManager::ThreadFirst()
{
	////////////////////////////////////
	// 事前準備
	////////////////////////////////////
	//// PIO状態確認 定周期タイマーセット
	__int64 ts = (__int64)PREPROC_TIMER * -10000;	// 1μ秒→100ナノ秒 (マイナス指定で、指定時間経過でシグナル) (第3引数は2回目以降の起動間隔)

	SetWaitableTimer( m_evTimeCycle,   (LARGE_INTEGER *)&ts, 0, NULL, NULL, FALSE );

	//// イベント設定
	HANDLE hArray[] = { m_evGetData,
						m_evTimeCycle
						};

	SetEventNum( sizeof(hArray)/sizeof(HANDLE), hArray );

	return 0;
}

//------------------------------------------
// スレッドイベント発生
// int nEventNo イベントNo (0オリジン)
//------------------------------------------
void PreDefectManager::ThreadEvent( int nEventNo )
{
	//// PIO状態確認 定周期タイマーセット
	__int64 ts = (__int64)PREPROC_TIMER * -10000;	// 1μ秒→100ナノ秒 (マイナス指定で、指定時間経過でシグナル) (第3引数は2回目以降の起動間隔)

	////// シグナルの条件
	enum {	EV_GETDATA = 0,					// 前工程実績情報取得
			EV_TIMECYCLE,					// 定周期実行
		};

	////// シグナル条件分け
	switch ( nEventNo ) {
//-----------------------------------------------------------
		case EV_GETDATA:
			LOG(em_MSG), "[%s] 前工程実績取得要求 [0x%X]", my_sThreadName, GetTid() );
			PreDefectMain();

			// 次のタイマーを発行
			CancelWaitableTimer( m_evTimeCycle );
			SetWaitableTimer( m_evTimeCycle,   (LARGE_INTEGER *)&ts, 0, NULL, NULL, FALSE );
			break;

//-----------------------------------------------------------
		case EV_TIMECYCLE:											// 定周期
			LOG(em_MSG), "[%s] 定周期処理 [0x%X]", my_sThreadName, GetTid() );
			PreDefectMain();

			// 次のタイマーを発行
			CancelWaitableTimer( m_evTimeCycle );
			SetWaitableTimer( m_evTimeCycle,   (LARGE_INTEGER *)&ts, 0, NULL, NULL, FALSE );
			break;

//-----------------------------------------------------------
		default :
			// ありえない！！
			LOG(em_ERR), "[%s] ThreadEvent EventNo NG=%d", my_sThreadName, nEventNo );
			break;

//-----------------------------------------------------------
	}
}

//------------------------------------------
// スレッド終了前処理 (終了シグナル後にコールバック)
//------------------------------------------
int PreDefectManager::ThreadLast()
{
	return 0;
}

//------------------------------------------
// 前工程実績編集メイン処理
//------------------------------------------
void PreDefectManager::PreDefectMain()
{
	
	CString sql;					// SQL文
	CString wk;
	CTime t = CTime::GetCurrentTime() - CTimeSpan( 1, 0, 0, 0 );			// 1日前
	CString strTime = t.Format("'%Y/%m/%d %H:%M:%S'");
	CString cDbName;
	CString cDbName2;
	cDbName = DB_MAE_COIL_INF;
	cDbName2 = DB_MAE_COIL_LINE;

	//// ODBC接続クラス インスタンス生成
	OdbcBase clsDB;
	if( ! clsDB.Connect( TASKINI_NAME ) ) {
		// データベース接続エラー
		LOG(em_ERR), "[%s] DB接続エラー", my_sThreadName );
		syslog(SYSNO_DB_CONNECT_ERR, "");
		return;
	}

	//// SQL文作成(管理No取得)
	sql =  "SELECT";
	sql += " INF.管理No, LINE.前工程管理No, LINE.疵検出工程, LINE.表裏区分 , INF.命令No, INF.コイルNo ";	// 2014.05.09 諸藤 広畑材の表示
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
	wk.Format( " AND 登録日付 > %s", strTime );
	sql += wk;
	sql += " ORDER BY 登録日付 ASC ";

	//// 実行
	if( ! clsDB.ExecuteDirect( sql ) ) {
		LOG(em_NON), "%s", sql );
		syslog(SYSNO_DB_EXECUTEDIRECT_ERR, "[%s]", cDbName);
		return;
	}
	
	//// 取得
	while (true) {

		int sqlrc = clsDB.Fetch();
		if( sqlrc == KIZUODBC_FETCH_OK ) {					// データ有り
			
			PreDBManager::DeliveryPreDB*  pDeli = new PreDBManager::DeliveryPreDB;

			clsDB.GetData( 1, pDeli->cKizuKenNo, sizeof(pDeli->cKizuKenNo) );				// 管理No
			clsDB.GetData( 2, pDeli->cPreKizuKenNo, sizeof(pDeli->cPreKizuKenNo) );			// 前工程管理No
			clsDB.GetData( 3, pDeli->cLineCode, sizeof(pDeli->cLineCode));					// 工程コード(疵検出工程)	2014.05.09 諸藤 広畑材の表示
			clsDB.GetData( 4, &pDeli->nOuDiv );												// 表裏区分
			clsDB.GetData( 5, &pDeli->cMeiNo, sizeof(pDeli->cMeiNo) );						// 命令No
			clsDB.GetData( 6, &pDeli->cCoilNo, sizeof(pDeli->cCoilNo) );					// コイルNo

			// DB登録処理へ送信
			mcls_pPreDB->gque_AddSQLData.AddToTail( pDeli );

			LOG(em_MSG), "[%s]　DB登録処理要求 [管理No=%s][前工程管理No=%s][表裏区分=%d]",
				my_sThreadName, pDeli->cKizuKenNo, pDeli->cPreKizuKenNo, pDeli->nOuDiv );
	
		} else if( sqlrc == KIZUODBC_FETCH_NODATE ) {		// データ無し
			return;

		} else {								// エラー
			LOG(em_NON), "%s", sql );
			LOG(em_ERR), "[%s] 前工程コイル情報取得エラー [%s][%s]", my_sThreadName, cDbName, cDbName2 );
			syslog(SYSNO_DB_FETCH_ERR, "[%s]", cDbName);
			return;
		}
	}

	return;

}
