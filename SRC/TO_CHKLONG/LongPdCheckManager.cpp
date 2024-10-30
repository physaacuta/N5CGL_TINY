#include "StdAfx.h"
#include "LongPdCheckManager.h"

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
LongPdCheckManager::LongPdCheckManager(void) :
ThreadManager("LPdMgr"),
m_nParamCycleTime(-1)
{
	//// ハンドル生成
	m_evTimerExec  = CreateWaitableTimer(NULL, FALSE, NULL);

	//// 初期化
	memset(m_CheckInf, 0x00, sizeof(m_CheckInf));
}

//------------------------------------------
// デストラクタ
//------------------------------------------
LongPdCheckManager::~LongPdCheckManager(void)
{
	//// ハンドル解放
	CloseHandle(m_evTimerExec);

	//// 開放
}

//------------------------------------------
// スレッド開始前処理 (スレッド実行直後(シグナルセット前)にコールバック)
//------------------------------------------
int LongPdCheckManager::ThreadFirst()
{
	//// 事前準備
	m_nParamCycleTime = mcls_pParam->GetCommonMast().nLongPd_GuidanceCheckInterval * 1000;

	__int64 ts = m_nParamCycleTime * -10000; 
	SetWaitableTimer(m_evTimerExec, (LARGE_INTEGER *)&ts, m_nParamCycleTime, NULL, NULL, FALSE);

	//// イベント設定
	HANDLE hArray[] = { m_evTimerExec
						};
	SetEventNum(sizeof(hArray)/sizeof(HANDLE), hArray);
	return 0;
}

//------------------------------------------
// スレッド終了前処理 (終了シグナル後にコールバック)
//------------------------------------------
int LongPdCheckManager::ThreadLast()
{
	// 定周期停止
	CancelWaitableTimer(m_evTimerExec);
	return 0;
}

//------------------------------------------
// スレッドイベント発生
// int nEventNo イベントNo (0オリジン)
//------------------------------------------
void LongPdCheckManager::ThreadEvent(int nEventNo)
{
	////// シグナルの条件
	enum {	EV_TIMER_EXEC = 0,				// 処理実行 定周期タイマ
	};

	////// シグナル条件分け
	switch (nEventNo) {

//-----------------------------------------------------------------------------------------------
	case EV_TIMER_EXEC: 					// 処理実行 定周期タイマ
		{
			// パラメータ再読み込み
			bool bRead = mcls_pParam->ReadParam_All();
			if ( true == bRead )
			{
				// 長周期欠陥チェック
				CheckLongPd();

				// タイマーの時間が変更されていたら反映
				int nTime = mcls_pParam->GetCommonMast().nLongPd_GuidanceCheckInterval * 1000;
				if( m_nParamCycleTime != nTime ) 
				{
					LOG(em_MSG), "[%s] 長周期欠陥チェック 監視周期変更 [%d]->[%d]sec", my_sThreadName, m_nParamCycleTime / 1000, mcls_pParam->GetCommonMast().nLongPd_GuidanceCheckInterval );

					m_nParamCycleTime = nTime;

					// 変更された値でタイマー起動
					CancelWaitableTimer(m_evTimerExec);
					__int64 ts = m_nParamCycleTime * -10000; 
					SetWaitableTimer(m_evTimerExec, (LARGE_INTEGER *)&ts, m_nParamCycleTime, NULL, NULL, FALSE);
				}
			}
		}
		
		break;
	
//-----------------------------------------------------------------------------------------------
	default :
		// ありえない！！
		LOG(em_ERR), "[%s] ThreadEvent EventNo NG=%d", my_sThreadName, nEventNo );
		break;

	}
}

// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 出力

//------------------------------------------
// 長周期欠陥チェック　※定周期
//------------------------------------------
void LongPdCheckManager::CheckLongPd()
{
	const ParamManager::P_COMMON_MAST& prm = mcls_pParam->GetCommonMast();
	CString csSQL = "";
	CString csTemp = "";
	CString csLog = "";
	char cSQLBuf[256];

	LOG(em_MSG), "[%s] ----- 長周期欠陥　定周期チェック ---------------------------", my_sThreadName);

	//---------------------------------
	// ガイダンス機能が無効ならチェック無し
	//---------------------------------
	if( 0 == prm.nLongPd_GuidanceAvailable ) 		
	{
		LOG(em_MSG), "[%s] ガイダンス機能 無効中のためチェック無し", my_sThreadName);
		return;
	}

	// 初期化
	CEHCK_INF	wkInf[CHECK_COIL_NUM];
	memset(wkInf, 0x00, sizeof(wkInf));

	// DB
	OdbcBase clsDB;
	if( ! clsDB.Connect(TASKINI_NAME)) {
		LOG(em_ERR), "[%s] DB接続エラー", my_sThreadName);
		syslog(SYSNO_DB_CONNECT_ERR, "");
		return;
	}

	//---------------------------------
	// 対象コイルを取得
	//---------------------------------
	csSQL = " SELECT ";
	csTemp.Format(" TOP %d ", CHECK_COIL_NUM);
	csSQL += csTemp;
	csSQL += "  管理No ";
	csSQL += " ,ISNULL(生産No, '') As コイルNo ";
	csTemp.Format(" FROM %s ", DB_COIL_INF);
	csSQL += csTemp;
	csTemp.Format(" ORDER BY 登録日付 DESC ");
	csSQL += csTemp;

	if (!clsDB.ExecuteDirect(csSQL)) {
		LOG(em_ERR), "[%s] %s<%s>", my_sThreadName, clsDB.GetLastErrMsg(), csSQL);
		syslog(SYSNO_DB_EXECUTEDIRECT_ERR, "[%s]", DB_COIL_INF);
		return;
	}

	//// 取得
	int nGetCount = 0;
	while (TRUE) {
		int sqlrc = clsDB.Fetch();
		if (sqlrc == KIZUODBC_FETCH_OK) {					// データ有り
			// 管理No
			memset(cSQLBuf, 0x00, sizeof(cSQLBuf));
			clsDB.GetData(1, &cSQLBuf, sizeof(cSQLBuf) );
			memcpy(wkInf[nGetCount].cManagementNo, cSQLBuf, sizeof(wkInf[nGetCount].cManagementNo));

			// コイルNo
			memset(cSQLBuf, 0x00, sizeof(cSQLBuf));
			clsDB.GetData(2, &cSQLBuf, sizeof(cSQLBuf) );
			memcpy(wkInf[nGetCount].cCoilNoIn, cSQLBuf, sizeof(wkInf[nGetCount].cCoilNoIn));
		
			csTemp.Format(" 対象%d:[%.*s]", nGetCount + 1, sizeof(wkInf[nGetCount].cManagementNo), wkInf[nGetCount].cManagementNo);
			csLog += csTemp;

			// カウントアップ
			nGetCount ++;
		} else if( sqlrc == KIZUODBC_FETCH_NODATE ) {		// データ無し
			break;	
		} else {								// エラー
			syslog(SYSNO_DB_FETCH_ERR, "[%s][%s]", DB_COIL_INF, clsDB.GetLastErrMsg());
			return;
		}
	}

	if ( 0 < nGetCount )
	{
		LOG(em_MSG), "[%s] チェック対象 %s", my_sThreadName, csLog);
	}

	// 対象コイルに長周期欠陥が存在するかチェック
	// 長周期欠陥は炉チカ（定数マスタの設定範囲内）のものを対象とする。
	for ( int nCoilIndex = 0; nCoilIndex < nGetCount; nCoilIndex++ ) 		
	{
		// 長周期欠陥の存在チェック
		int nLongPdCnt[NUM_MEN];
		memset(nLongPdCnt, 0x00, sizeof(nLongPdCnt));

		for( int nMen = 0; nMen < NUM_MEN; nMen++ ) 			
		{
			// SQL作成
			CString csTable = (0 == nMen ? DB_LONGPD_EXEC_RESULT_DEFECT_T : DB_LONGPD_EXEC_RESULT_DEFECT_B);

			csSQL = " SELECT ";
			csSQL += " COUNT(DefectNo) ";
			csSQL += " FROM ";
			csTemp.Format(" %s ", csTable);
			csSQL += csTemp;
			csSQL += " WHERE ";
			csTemp.Format(" ManagementNo = '%.20s' ", wkInf[nCoilIndex].cManagementNo);
			csSQL += csTemp;
			csSQL += " AND ( ";
			for( int ii = 1; ii <= MAX_LONGPD_FILTER; ii++ ) 				
			{
				if( ii > 1 ) 					
				{
					csSQL += " OR ";
				}
				csTemp.Format(" Cycle_Filter%d BETWEEN %d AND %d ", ii, prm.nLongPd_NearFurnaceRangeFrom, prm.nLongPd_NearFurnaceRangeTo);
				csSQL += csTemp;
			}
			csSQL += " ) ";

			// 実行して取得
			if (!clsDB.ExecuteDirect(csSQL)) {
				LOG(em_ERR), "[%s] %s<%s>", my_sThreadName, clsDB.GetLastErrMsg(), csSQL);
				syslog(SYSNO_DB_EXECUTEDIRECT_ERR, "[%s]", csTable);
				return;
			}

			//// 取得
			while (TRUE) {
				int sqlrc = clsDB.Fetch();
				if (sqlrc == KIZUODBC_FETCH_OK) {					// データ有り
					clsDB.GetData(1, &wkInf[nCoilIndex].nCount[nMen] );
					if( 0 < wkInf[nCoilIndex].nCount[nMen] ) 						
					{
						// 長周期欠陥あり
						wkInf[nCoilIndex].bLongPdExist = TRUE;
					}
				} else if( sqlrc == KIZUODBC_FETCH_NODATE ) {		// データ無し
					break;	
				} else {								// エラー
					syslog(SYSNO_DB_FETCH_ERR, "[%s][%s]", csTable, clsDB.GetLastErrMsg());
					return;
				}
			}
		}

		// 前回の結果が有れば取得
		BOOL bPrevCheck = false;
		for( int ii = 0; ii < CHECK_COIL_NUM; ii++ )
		{
			if( 0 == memcmp(wkInf[nCoilIndex].cManagementNo, m_CheckInf[ii].cManagementNo, SIZE_KIZUKEN_NO) ) 					
			{
				bPrevCheck = m_CheckInf[ii].bLongPdExist;
			}
		}

		// 前回チェック時に検出されておらず、今回検出されたなら表示管理へ通知する。
		if( 0 < wkInf[nCoilIndex].nCount[0] || 0 < wkInf[nCoilIndex].nCount[1] )
		{
			if( TRUE == wkInf[nCoilIndex].bLongPdExist && FALSE == bPrevCheck ) 						
			{
				LOG(em_MSG), "[%s] 長周期欠陥発生 管理No[%.20s] 表[%d] 裏[%d]", my_sThreadName, wkInf[nCoilIndex].cManagementNo, wkInf[nCoilIndex].nCount[0], wkInf[nCoilIndex].nCount[1] );

				// 表示管理へ通知
				COMMON_QUE que;
				memset(&que, 0x00, sizeof(que));

				que.nEventNo = FACT_HY_DEFECT_05;
				que.nLineNo = getlineid();
				memcpy(que.mix.cdata[0], wkInf[nCoilIndex].cManagementNo, SIZE_KIZUKEN_NO);

				// 送信
				int iRet = send_mail(HY_DEFECT, "*", &que);
				if( 0 != iRet ) 
				{
					syslog(SYSNO_MAIL_SNED_ERR, "[表示管理へ長周期欠陥発生通知 err_code=%d]", iRet);
				}
				else 							
				{
					LOG(em_MSG), "[%s] 長周期欠陥発生通知(FACT_HY_DEFECT_05) 送信 管理No[%.20s]", my_sThreadName, wkInf[nCoilIndex].cManagementNo);
				}
			}
		}
	}

	// バッファにコピー
	memcpy(m_CheckInf, wkInf, sizeof(m_CheckInf));
}
