#include "StdAfx.h"
#include "FFSendManager.h"

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
FFSendManager::FFSendManager(void) :
ThreadManager("FFSendManager")
{
	// シグナル生成
	m_evFFSend		= CreateEvent(NULL, FALSE, FALSE, NULL);
	m_evTimeCycle	= CreateEvent(NULL, FALSE, FALSE, NULL);
}

//------------------------------------------
// デストラクタ
//------------------------------------------
FFSendManager::~FFSendManager(void)
{
	// イベント開放
	CloseHandle( m_evFFSend );
}

//------------------------------------------
// スレッド開始前処理 (スレッド実行直後(シグナルセット前)にコールバック)
//------------------------------------------
int FFSendManager::ThreadFirst()
{
	////////////////////////////////////
	// 事前準備
	////////////////////////////////////
	//// PIO状態確認 定周期タイマーセット
	__int64 ts = (__int64)FFSEND_TIMER * -10000;	// 1μ秒→100ナノ秒 (マイナス指定で、指定時間経過でシグナル) (第3引数は2回目以降の起動間隔)

	m_evTimeCycle	= CreateWaitableTimer( NULL, FALSE, NULL );			
	SetWaitableTimer( m_evTimeCycle,   (LARGE_INTEGER *)&ts, 0, NULL, NULL, FALSE );

	//// イベント設定
	HANDLE	hArray[] = {m_evFFSend,
						m_evTimeCycle};

	SetEventNum( sizeof(hArray)/sizeof(HANDLE), hArray );
	return 0;
}

//------------------------------------------
// スレッド終了前処理 (終了シグナル後にコールバック)
//------------------------------------------
int FFSendManager::ThreadLast()
{
	CancelWaitableTimer( m_evTimeCycle );
	CloseHandle( m_evTimeCycle );

	return 0;
}

//------------------------------------------
// スレッドイベント発生
// int nEventNo イベントNo (0オリジン)
//------------------------------------------
void FFSendManager::ThreadEvent( int nEventNo )
{
	//// PIO状態確認 定周期タイマーセット
	__int64 ts = (__int64)FFSEND_TIMER * -10000;	// 1μ秒→100ナノ秒 (マイナス指定で、指定時間経過でシグナル) (第3引数は2回目以降の起動間隔)

	////// シグナルの条件
	enum {	EV_FFSEND,						// FF送信要求
			EV_TIMECYCLE,					// 定周期実行
			};

	////// シグナル条件分け
	switch ( nEventNo ) {

//-----------------------------------------------------------------------------------------------
	case EV_FFSEND:
		LOG(em_MSG), "[%s] FF送信要求チェック処理", my_sThreadName );
		FFSendMain();
		
		// 次のタイマーを発行
		CancelWaitableTimer( m_evTimeCycle );
		SetWaitableTimer( m_evTimeCycle,   (LARGE_INTEGER *)&ts, 0, NULL, NULL, FALSE );
		break;

//-----------------------------------------------------------
	case EV_TIMECYCLE:
		LOG(em_MSG), "[%s] 定周期処理 [0x%X]", my_sThreadName, GetTid() );
		FFSendMain();

		// 次のタイマーを発行
		CancelWaitableTimer( m_evTimeCycle );
		SetWaitableTimer( m_evTimeCycle,   (LARGE_INTEGER *)&ts, 0, NULL, NULL, FALSE );
		break;

//-----------------------------------------------------------
	default :
		// ありえない！！
		break;
	}
}

//------------------------------------------
// 前工程実績要求メイン処理
//------------------------------------------
void FFSendManager::FFSendMain()
{
	CString sql;					// SQL文
	CString wk;
	CString cDbName;
	cDbName = DB_MAE_COIL_INF;

	CTime t = CTime::GetCurrentTime() - CTimeSpan( 1, 0, 0, 0 );	// 1日前
	CString strTime = t.Format("'%Y/%m/%d %H:%M:%S'");
	char	cKeyinf[SIZE_COIL_KEY + 1];						// キー情報
	char	cLine_Code[SIZE_KOUTEI_CODE + 1];				// 工程コード
	char	cMeiNo[SIZE_MEI_NO + 1];						// 命令No
	char	cCoilNo[SIZE_COIL_NO + 1];						// コイルNo

	memset( &cKeyinf, 0x00, sizeof(cKeyinf) );

	//// ODBC接続クラス インスタンス生成
	OdbcBase clsDB;
	if( ! clsDB.Connect( TASKINI_NAME ) ) {
		// データベース接続エラー
		LOG(em_ERR), "[%s] DB接続エラー", my_sThreadName );
		syslog(SYSNO_DB_CONNECT_ERR, "");
		return;
	}

	//// SQL文作成(命令No取得)
	sql =  "SELECT";
	sql += " キー情報, 命令No, 工程コード, コイルNo ";
	sql += " FROM ";
	sql += cDbName;
	sql += " WHERE FF前工程疵情報受信フラグ = -1";
	sql += " AND 仮管理Noフラグ = -1 ";
	wk.Format( " AND 登録日付 > %s", strTime );
	sql += wk;
	sql += " ORDER BY 管理No ASC ";		// 過去から順番に送信(登録日付では、複数レコード登録時に通順の最後から要求されてしまうため)
	//sql += " ORDER BY 登録日付 ASC ";
	
	//// 実行
	if( ! clsDB.ExecuteDirect( sql ) ) {
		LOG(em_NON), "%s", sql );
		LOG(em_ERR), "[%s] 前工程コイル情報実行エラー [%s]", my_sThreadName, cDbName );
		syslog(SYSNO_DB_EXECUTEDIRECT_ERR, "[%s]", cDbName);
		return;
	}
	
	//// 取得
	while (true) {

		int sqlrc = clsDB.Fetch();
		if( sqlrc == KIZUODBC_FETCH_OK ) {					// データ有り
			
			clsDB.GetData(1, cKeyinf, sizeof(cKeyinf));			// キー情報
			clsDB.GetData(2, cMeiNo, sizeof(cMeiNo));			// 命令No
			clsDB.GetData(3, cLine_Code, sizeof(cLine_Code));	// 工程コード
			clsDB.GetData(4, cCoilNo, sizeof(cCoilNo));			// コイルNo

			// 前工程実績要求
			SendMailFFReq(cKeyinf, cMeiNo, cLine_Code, cCoilNo);

		} else if( sqlrc == KIZUODBC_FETCH_NODATE ) {		// データ無し
			return;

		} else {								// エラー
			LOG(em_NON), "%s", sql );
			LOG(em_ERR), "[%s] 前工程コイル情報取得エラー [%s]", my_sThreadName, cDbName );
			syslog(SYSNO_DB_FETCH_ERR, "[%s]", cDbName);
			return;
		}
	}

	return;

}

//------------------------------------------
// 前工程実績要求
// char* cKeyinf	コイル特定するためのキー情報
// char* cMeiNo		命令No
// char* cLine_Code	工程コード
// char* cCoilNo	コイルNo
//------------------------------------------
void FFSendManager::SendMailFFReq(char* cKeyinf, char* cMeiNo, char* cLine_Code, char* cCoilNo)
{
	char cMeiNoBuff[SIZE_MEI_NO + 1];
	COMMON_QUE		que;
	memset(&que, 0x00, sizeof(que));

	memset(&cMeiNoBuff, 0x00, sizeof(cMeiNoBuff));
	strcpy(cMeiNoBuff, cMeiNo);
	KizuFunction::Trim(cMeiNoBuff);

	que.nLineNo = getlineid();

	//// 対FFサーバ通信へ通知
	que.nEventNo = FACT_TO_FFSERV_02;							// 要求モード
	memcpy(&que.mix.cdata[0], cKeyinf, SIZE_COIL_KEY);			// コイル特定するためのキー情報
	//memcpy(&que.mix.cdata[1], cLine_Code, SIZE_KOUTEI_CODE);	// 工程コード
	memcpy(&que.mix.cdata[1], KOUTEI_CODE, SIZE_KOUTEI_CODE);	// 工程コード(プロコンからのコイル情報には工程コードが入っていない)
	sprintf(que.mix.cdata[2], "%04s", cMeiNoBuff);				// 命令No
	memcpy(&que.mix.cdata[3], cCoilNo, SIZE_COIL_NO);			// コイルNo


	send_mail(TO_FFSERV, ".", &que);

	LOG(em_MSG), "[%s] 前工程実績要求送信 [キー情報=%16s]", my_sThreadName, cKeyinf );

}
