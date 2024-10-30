#include "StdAfx.h"
#include "ControlManager.h"

// メモリーリーク検出 ----->>>
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif
// <<<------------------------

//------------------------------------------
// コンストラクタ
// CString cSession セッション
// LogFileManager* pLog ログファイルマネージャーポインタ
//------------------------------------------
ControlManager::ControlManager(char* cSession, LogFileManager* pLog) :
m_cSession(cSession),
m_bTranStat(DIV_BOOL_NON),
m_bExecFast(false),
mcls_pLog(pLog)
{
	int ii;
	CString strVal;
	char	cWk[1024];

	//// シグナル生成
	m_evEndCycle	 = CreateEvent(NULL, FALSE, FALSE, NULL);	
	m_evEndSizeCheck = CreateEvent(NULL, FALSE, FALSE, NULL);		

	//// iniファイル読み込み
	// ODBCのセッション
	GetPrivateProfileString(m_cSession, "DB_ODBC", "", cWk, sizeof(cWk), TASKINI_NAME);
	m_cOdbcSession = cWk;
	// トランザクションログサイズチェック
	m_nIniTranSizeErrID = GetPrivateProfileInt(m_cSession, "DB_TRAN_SIZE_ERR_ID", 0, TASKINI_NAME);	
	// トランザクションログサイズ
	m_nIniTranErrSize	= GetPrivateProfileInt(m_cSession, "DB_TRAN_ERR_SIZE", 10000, TASKINI_NAME);	

	// 定周期実行周期
	m_nIniTimeCycle = GetPrivateProfileInt(m_cSession, "TIME_CYCLE", 0, TASKINI_NAME);		
	m_nIniTimeCycle = m_nIniTimeCycle * 60 * 1000;// *1000;//* 60 * 1000;	// 分→ms 
	// DB容量チェック周期
	m_nIniTimeSizeCheck = GetPrivateProfileInt(m_cSession, "TIME_SIZECHECK", 0, TASKINI_NAME);		
	m_nIniTimeSizeCheck = m_nIniTimeSizeCheck * 60 * 1000;// *1000;//* 60 * 1000;	// 分→ms 
	// DB容量低下時の削除スレッド数
	m_nIniThreadCount = GetPrivateProfileInt(m_cSession, "THREAD_COUNT", 0, TASKINI_NAME);		
	if(m_nIniThreadCount < 0 || m_nIniThreadCount > THREAD_MAXCNT) _ASSERT(FALSE);
	// 空き容量率
	for(ii=0; ii<THREAD_MAXCNT; ii++) m_dIniDeleteRitu[ii] = 0.0;
	for(ii=0; ii<m_nIniThreadCount; ii++ ) {
		strVal.Format("DEL_RITU_%d", ii+1);
		m_dIniDeleteRitu[ii] = (double)GetPrivateProfileInt(m_cSession, strVal, 0, TASKINI_NAME);		
		if(m_dIniDeleteRitu[ii] == 0.0) {		// 終わり
			break;
		}
		m_dIniDeleteRitu[ii] = m_dIniDeleteRitu[ii] / 10000;	// [%-2]→[1.0]
	}
	if(0 != m_nIniThreadCount && 0 == ii) _ASSERT(FALSE);

	// 書込優先時の待ち時間 [ms]
	m_nIniTimeMaxWait = GetPrivateProfileInt(m_cSession, "TIME_MAX_WAIT", 0, TASKINI_NAME);		
	// 削除優先時の待ち時間 [ms]
	m_nIniTimeMinWait = GetPrivateProfileInt(m_cSession, "TIME_MIN_WAIT", 0, TASKINI_NAME);	

	//// パラメータ条件
	memset(&m_Param, 0x00, sizeof(m_Param));
	// ログ保存日数
	m_Param.nLogSaveDay = GetPrivateProfileInt(m_cSession, "LOG_SAVEDAY", 0, TASKINI_NAME);
	if(m_Param.nLogSaveDay < 0) _ASSERT(FALSE);
	// 削除コイル件数
	m_Param.nDelCoilCnt = GetPrivateProfileInt(m_cSession, "COIL_DEL_COUNT", 0, TASKINI_NAME);	
	// コイル実績テーブル
	GetPrivateProfileString(m_cSession, "COILRESULT_TBL", "", m_Param.sCoilResultTbl, sizeof(m_Param.sCoilResultTbl), TASKINI_NAME);
	// コイル実績 主キー
	GetPrivateProfileString(m_cSession, "COILRESULT_KEY", "", m_Param.sCoilResultKey, sizeof(m_Param.sCoilResultKey), TASKINI_NAME);
	// コイル実績 ソートキー
	GetPrivateProfileString(m_cSession, "COILRESULT_SORT", "", m_Param.sCoilResultSort, sizeof(m_Param.sCoilResultSort), TASKINI_NAME);
	if( 0 == strlen(m_Param.sCoilResultSort) ) memcpy(m_Param.sCoilResultSort, m_Param.sCoilResultKey, sizeof(m_Param.sCoilResultSort));


	//// 初期化
	memset(&m_DiskSpace, 0x00, sizeof(m_DiskSpace));
	for(ii=0; ii<THREAD_MAXCNT; ii++) {
		mcls_pDele[ii] = NULL;
	}
	mcls_pCycle = NULL;
	m_bWaitMode = true;
	m_nWaitTime = m_nIniTimeMinWait;	
}

//------------------------------------------
// デストラクタ
//------------------------------------------
ControlManager::~ControlManager(void)
{
	// 開放
	CloseHandle(m_evEndCycle);
	CloseHandle(m_evEndSizeCheck);
}

//------------------------------------------
// 削除スレッド初期化処理 (一度のみ起動)
//------------------------------------------
void ControlManager::WorkerInit()
{
	//// ワーカースレッド 初期化
	// 定周期実行
	mcls_pCycle = new DeleTable(-1, &m_nWaitTime, m_evEndCycle, NULL, m_cSession, m_cOdbcSession);
	mcls_pCycle->SetLogMgr(mcls_pLog); 
	mcls_pCycle->SetParamInf(&m_Param);
	// 容量実行
	for(int ii=0; ii<m_nIniThreadCount; ii++) {
		mcls_pDele[ii] = new DeleTable(ii+1, &m_nWaitTime, NULL, m_evEndSizeCheck, m_cSession, m_cOdbcSession);
		mcls_pDele[ii]->SetLogMgr(mcls_pLog); 
		mcls_pDele[ii]->SetLockMgr(&mcls_Lock); 
		mcls_pDele[ii]->SetParamInf(&m_Param);
	}	

	//// ワーカースレッド 起動
	mcls_pCycle->Start();
	for(int ii=0; ii<m_nIniThreadCount; ii++) {
		mcls_pDele[ii]->Start();
	}
}


//------------------------------------------
// 削除スレッド停止処理 (一度のみ起動)
//------------------------------------------
void ControlManager::WorkerStop()
{
	//// ワーカースレッド 停止
	mcls_pCycle->SetStop();
	// まず全体に停止指示を通知する
	for(int ii=0; ii<m_nIniThreadCount; ii++) {
		mcls_pDele[ii]->SetStop();
	}
	LOG(em_MSG), "[Cont] [%s] 各子スレッド停止要求完了", m_cSession);
}

//------------------------------------------
// 削除スレッド終了処理 (一度のみ起動)
//------------------------------------------
void ControlManager::WorkerExit()
{
	//// ワーカースレッド 開放
	mcls_pCycle->Stop();
	SAFE_DELETE(mcls_pCycle);
	for(int ii=0; ii<m_nIniThreadCount; ii++) {
		mcls_pDele[ii]->Stop();
		SAFE_DELETE( mcls_pDele[ii] );
	}
}


//------------------------------------------
// スレッド
//------------------------------------------
UINT WINAPI ControlManager::MainThread(LPVOID param)
{
//	int ii;

	////// メインインスタンス取り出し
	ControlManager *p = (ControlManager *)param;				// 自クラスのインスタンスを取得
	p->SetTid();										// 自スレッドIDセット


	////////////////////////////////////
	// 事前準備
	////////////////////////////////////

	//// 定周期タイマーセット
	HANDLE		evTimeCycle;										// 定周期実行周期
	HANDLE		evTimeSizeCheck;									// DB容量チェック周期
	evTimeCycle		= CreateWaitableTimer(NULL, FALSE, NULL);	
	evTimeSizeCheck	= CreateWaitableTimer(NULL, FALSE, NULL);	
	if(0 != p->m_nIniTimeCycle) {
		__int64 ts = 10000 * -10000;	// 1μ秒→100ナノ秒 (マイナス指定で、指定時間経過でシグナル) (第3引数は2回目以降の起動間隔)
		SetWaitableTimer(evTimeCycle,		(LARGE_INTEGER *)&ts, p->m_nIniTimeCycle, NULL, NULL, FALSE);
	}
	if(0 != p->m_nIniTimeSizeCheck) {
		__int64 ts = 10000 * -10000;	// 1μ秒→100ナノ秒 (マイナス指定で、指定時間経過でシグナル) (第3引数は2回目以降の起動間隔)
		SetWaitableTimer(evTimeSizeCheck,	(LARGE_INTEGER *)&ts, p->m_nIniTimeSizeCheck, NULL, NULL, FALSE);
	}


	////////////////////////////////////
	// 通常のメインスレッド処理
	////////////////////////////////////

	// WaitForMultipleObjectsイベント
	HANDLE	hArray[] = {p->my_evStop,
						evTimeCycle,
						p->m_evEndCycle,
						evTimeSizeCheck,
						p->m_evEndSizeCheck
						};
	enum {	EV_STOP = WAIT_OBJECT_0,		// スレッド終了
			EV_TIMECYCLE,					// 定周期タイマー
			EV_END_TIMECYCLE,				// 定周期タイマー完了
			EV_SIZECHECK,					// DB容量チェック
			EV_END_SIZECHECK,				// DB容量チェック完了
			EV_END	};

	// メッセージキュー読み込み
	pLOG(em_MSG), "[Cont] [%s] スレッド開始 [0x%X]", p->m_cSession, p->GetTid());
	while (1) {
		DWORD ret = WaitForMultipleObjects(EV_END, hArray, FALSE, INFINITE);

		switch (ret) {

//-----------------------------------------------------------------------------------------------
		case EV_STOP:												// スレッド終了
			pLOG(em_MSG), "[Cont] [%s] スレッド終了 [0x%X]", p->m_cSession, p->GetTid());
			CancelWaitableTimer(evTimeCycle);
			CancelWaitableTimer(evTimeSizeCheck);
			CloseHandle(evTimeCycle);
			CloseHandle(evTimeSizeCheck);
			return 0;


//-----------------------------------------------------------------------------------------------
		case EV_TIMECYCLE:											// 定周期タイマー
			if( ! OdbcBase::OpenCheck( TASKINI_NAME, p->m_cOdbcSession) ) {
				pLOG(em_ERR), "[Cont] [%s] DB接続失敗 ", p->m_cSession);
				break;
			}

			if( p->mcls_pCycle->IsExec_Cycle()) {
				pLOG(em_WAR), "[Cont] [%s] 既に、定周期実行 ", p->m_cSession);
				break;
			}

			// 実行依頼
			pLOG(em_MSG), "[Cont] [%s] 定周期実行依頼", p->m_cSession);
			p->mcls_pCycle->SetEvCycle();
			break;

//-----------------------------------------------------------------------------------------------
		case EV_END_TIMECYCLE:										// 定周期タイマー完了
			// トランザクションログサイズチェック
			p->Check_TranSize();
			break;

//-----------------------------------------------------------------------------------------------
		case EV_SIZECHECK:											// DB容量チェック
			if( ! OdbcBase::OpenCheck( TASKINI_NAME, p->m_cOdbcSession) ) {
				pLOG(em_ERR), "[Cont] [%s] DB接続失敗 ", p->m_cSession);
				break;
			}

			// 初回実行？
			if( ! p->m_bExecFast) {
				if( 1 <= p->m_nIniThreadCount ) p->mcls_pDele[0]->SetEvExecFast();
				p->m_bExecFast = true;
				break;
			}

			// DBの使用容量チェック
			p->CheckDB_Size();			
			break;


//-----------------------------------------------------------------------------------------------
		case EV_END_SIZECHECK:										// DB容量チェック完了
			// DB容量チェックが完了した為、再度容量チェック
			p->CheckDB_Size();
			break;

//-----------------------------------------------------------------------------------------------
		default :
			// ありえない！！
			break;
		}
	}
	return 0;
}


//------------------------------------------
// 削除/書込 優先モード (true:削除優先)
// bool mode True：削除優先 False:書込優先
//------------------------------------------
void ControlManager::SetWaitMode(bool mode)
{
	// 書込優先で最終スレッドが実行中の場合は 削除優先とする
	if ( ! mode ) {
		if( 0 == m_nIniThreadCount) {
			// チェックなし
		} else if ( mcls_pDele[m_nIniThreadCount-1]->IsExec() ) {
			LOG(em_ERR), "[Cont] [%s] 最終スレッド実行中により、強制的に削除優先モードへ行こう", m_cSession);
			mode = true;	
		}
	}

	// 状態変更
	if( mode != m_bWaitMode ) {
		if (mode) {			// 削除優先
			m_nWaitTime = m_nIniTimeMinWait;
			m_bWaitMode = true;
		} else {			// 書き込み優先
			m_nWaitTime = m_nIniTimeMaxWait;
			m_bWaitMode = false;
		}
	}
}


//------------------------------------------
// DBの使用容量チェック
//------------------------------------------
void ControlManager::CheckDB_Size() 
{
	int retc;				// スレッド実行個数
	retc = GetDB_Size();
	LOG(em_MSG), "[Cont] [%s] DB容量 [%.2f%%]", m_cSession, m_DiskSpace.dFreeRitu  * 100);

	if(retc > 0) {
		LOG(em_WAR), "[Cont] [%s] DB容量低下 [%.2f%%] スレッド%dまで実行", m_cSession, m_DiskSpace.dFreeRitu  * 100, retc);
		// DB容量低下実行
		for(int ii=0; ii<retc; ii++) {
			if( ! mcls_pDele[ii]->IsExec()) mcls_pDele[ii]->SetEvSizeMin();
		}
	}
}

//------------------------------------------
// DBの使用容量取得
// 戻り値 int -1:失敗  0:容量有り 1～容量無しによりこの数までのスレッドを実行
//------------------------------------------
int ControlManager::GetDB_Size() 
{
	CString sql;					// SQL文
	char buf[256];

	//// ODBC接続クラス インスタンス生成
	OdbcBase clsDB;
	if( ! clsDB.Connect(TASKINI_NAME, m_cOdbcSession )) {
		// データベース接続エラー
		LOG(em_ERR), "[Cont] [%s] DB接続エラー[%s]", m_cSession, m_cOdbcSession);
		syslog(SYSNO_DB_CONNECT_ERR, "%s:%s", m_cSession, m_cOdbcSession);
		return -1;
	}


	////// ストアドプロシージャーをコール
	sql = "sp_spaceused";
	if( !clsDB.ExecuteDirect(sql)) {
		LOG(em_ERR), "[Cont] [%s] DBSizeCheckエラー [%s]", m_cSession, m_cOdbcSession);
		syslog(SYSNO_DB_EXECUTEDIRECT_ERR, "[DBSizeCheck = %s:%s]", m_cSession, m_cOdbcSession);
		return -1;
	}

	//// 取得
	while (TRUE) {
		int sqlrc = clsDB.Fetch();
		if (sqlrc == KIZUODBC_FETCH_OK) {						// データ有り
			//clsDB.GetData(1, buf, sizeof(buf));										// DB名称
			clsDB.GetData(2, buf, sizeof(buf));		m_DiskSpace.dTotalSize = atof(buf);	// データベースのサイズ (%f MB)となっているから注意
			clsDB.GetData(3, buf, sizeof(buf));		m_DiskSpace.dFreeSize  = atof(buf);	// 未割り当て領域 (%f MB)となっているから注意																
			m_DiskSpace.dFreeRitu = m_DiskSpace.dFreeSize / m_DiskSpace.dTotalSize;		// 空き率算出												
			
		} else if( sqlrc == KIZUODBC_FETCH_NODATE ) {			// データ無し
			break;	
		} else {												// エラー
			syslog(SYSNO_DB_FETCH_ERR, "[DBSizeCheck = %s:%s]", m_cSession, m_cOdbcSession);
			return -1;
		}
	}

	//// 容量チェック (下から順にチェック)
	for(int ii=THREAD_MAXCNT-1; ii>=0; ii--) {
		if(0.0 == m_dIniDeleteRitu[ii]) continue;

		// 指定より下回っている？
		if( m_dIniDeleteRitu[ii] >= m_DiskSpace.dFreeRitu ) {
			return ii+1;
		}
	}
	return 0;
}

//------------------------------------------
// トランザクションログサイズチェック
//------------------------------------------
void ControlManager::Check_TranSize() 
{
	// チェック
	int min = mcls_pCycle->GetTranSizeMin();
	int now = mcls_pCycle->GetTranSizeNow();

	if( now == min ) {
		// 異常 → 正常
		if( DIV_BOOL_TRUE != m_bTranStat ) {
			LOG(em_MSG) , "[Cont] [%s] トランザクションログサイズ 正常復帰", m_cSession);
			if( 0 != m_nIniTranSizeErrID ) KizuFunction::SetStatus(m_nIniTranSizeErrID, true);
			m_bTranStat = DIV_BOOL_TRUE;
		}

	} else {
		// 正常 → 異常
		//  通常10Mが100倍になったらやっぱりおかしい
		//  10倍くらいならでっかいバイナリデータ登録した時など、一瞬増える可能性がある。
		if( now >= m_nIniTranErrSize ) {
			LOG(em_ERR) , "[Cont] [%s] トランザクションログサイズ 異常 [%d/%d]", m_cSession, now, min);
			if( DIV_BOOL_FALSE != m_bTranStat ) {
				if( 0 != m_nIniTranSizeErrID ) KizuFunction::SetStatus(m_nIniTranSizeErrID, false);

				m_bTranStat = DIV_BOOL_FALSE;
			}
		} else {
			LOG(em_WAR) , "[Cont] [%s] トランザクションログサイズ 警告 [%d/%d]", m_cSession, now, min);
		}
	}
} 