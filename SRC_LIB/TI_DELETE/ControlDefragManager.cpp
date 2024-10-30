#include "StdAfx.h"
#include "ControlDefragManager.h"

//------------------------------------------
// コンストラクタ
//------------------------------------------
ControlDefragManager::ControlDefragManager(void) :
ThreadManager("CntDefragMgr"),
m_nClassCnt(0),
m_bExecDefrag(false)
{
	// 初期化
	for(int ii=0; ii<CONTROL_MAXCNT; ii++) mcls_pDefrag[ii] = NULL;
	//// ハンドル生成
	m_evTimerExec  = CreateWaitableTimer(NULL, FALSE, NULL);

	m_nDefragIntervalDay = GetPrivateProfileInt(TI_DELETE, "DEFRAG_INTERVAL_DAY", 0, TASKINI_NAME);
}

//------------------------------------------
// デストラクタ
//------------------------------------------
ControlDefragManager::~ControlDefragManager(void)
{
	for(int ii=0; ii<CONTROL_MAXCNT; ii++) {
		SAFE_DELETE( mcls_pDefrag[ii] );
	}
	//// ハンドル解放
	CloseHandle(m_evTimerExec);
}


//------------------------------------------
// 管理スレッド生成
// int id 自分のID (1オリジン)
// CString cSession セッション
//------------------------------------------
void ControlDefragManager::WorkerNew(int id, char* cSession)
{
	if( ! IsDefragEnable() ) return;

	//// ワーカースレッド 初期化
	if( m_nClassCnt < id ) m_nClassCnt = id;
	mcls_pDefrag[id-1] = new DefragManager(id, cSession);
	mcls_pDefrag[id-1]->SetLogMgr( mcls_pLog );
	mcls_pDefrag[id-1]->SetExecDefragPnt(&m_bExecDefrag);
	mcls_pDefrag[id-1]->SetRunCnacel(false);	// 実行可
}

//------------------------------------------
// 管理スレッド ストップ通知
//------------------------------------------
void ControlDefragManager::WorkerStop()
{
	if( ! IsDefragEnable() ) return;

	for(int ii=0; ii<CONTROL_MAXCNT; ii++) {
		if( NULL == mcls_pDefrag[ii]) continue;
		
		mcls_pDefrag[ii]->SetRunCnacel(true);	// 実行不可
	}
}

//------------------------------------------
// スレッド開始前処理 (スレッド実行直後(シグナルセット前)にコールバック)
//------------------------------------------
int ControlDefragManager::ThreadFirst()
{

	//// 子供を開始
	for(int ii=0; ii<CONTROL_MAXCNT; ii++) {
		if( NULL == mcls_pDefrag[ii]) continue;
		
		mcls_pDefrag[ii]->Start();
	}


	//// 事前準備
	const int nCycle = 2 * 60 * 1000;
//	const int nCycle = 3 * 1000;

	__int64 ts = nCycle * -10000; 
	SetWaitableTimer(m_evTimerExec, (LARGE_INTEGER *)&ts, nCycle, NULL, NULL, FALSE);

	//// イベント設定
	HANDLE hArray[] = { m_evTimerExec
						};
	SetEventNum(sizeof(hArray)/sizeof(HANDLE), hArray);
	return 0;
}

//------------------------------------------
// スレッド終了前処理 (終了シグナル後にコールバック)
//------------------------------------------
int ControlDefragManager::ThreadLast()
{
	// 定周期停止
	CancelWaitableTimer(m_evTimerExec);


	// 子供も停止
	for(int ii=0; ii<CONTROL_MAXCNT; ii++) {
		if( NULL == mcls_pDefrag[ii]) continue;
		
		mcls_pDefrag[ii]->Stop();
	}
	return 0;
}

//------------------------------------------
// スレッドイベント発生
// int nEventNo イベントNo (0オリジン)
//------------------------------------------
void ControlDefragManager::ThreadEvent(int nEventNo)
{
	////// シグナルの条件
	enum {	EV_TIMER_EXEC = 0,				// 処理実行 定周期タイマ
	};

	////// シグナル条件分け
	switch (nEventNo) {

//-----------------------------------------------------------------------------------------------
	case EV_TIMER_EXEC: 					// 処理実行 定周期タイマ
		Exec();
		return;
	
//-----------------------------------------------------------------------------------------------
	default :
		// ありえない！！
		LOG(em_ERR), "[%s] ThreadEvent EventNo NG=%d", my_sThreadName, nEventNo );
		return;
	}
}

//------------------------------------------
// インデックス デフラグ実行可否判定
//------------------------------------------
void ControlDefragManager::Exec()
{
	char	cWk[2048];
	GetPrivateProfileString(TI_DELETE, "SQL_DEFRAG_RUN", "", cWk, sizeof(cWk), TASKINI_NAME);


	// 実行
	int		nExec;		// デフラグ実行可=1, 不可=0
	OdbcBase clsDB;
	clsDB.GetSelectKey_Static(cWk, &nExec, TASKINI_NAME);

	// セット
	m_bExecDefrag = nExec;
}