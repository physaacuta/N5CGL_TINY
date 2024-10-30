#include "StdAfx.h"
#include "SampManager.h"

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
SampManager::SampManager(void) :
ThreadManager("SampManager")
{
	//// ハンドル生成
	m_evTimerExec  = CreateWaitableTimer(NULL, FALSE, NULL);
}

//------------------------------------------
// デストラクタ
//------------------------------------------
SampManager::~SampManager(void)
{
	//// ハンドル解放
	CloseHandle(m_evTimerExec);
}


//------------------------------------------
// スレッド開始前処理 (スレッド実行直後(シグナルセット前)にコールバック)
//------------------------------------------
int SampManager::ThreadFirst()
{
	//// 事前準備
	__int64 ts = 3000 * -10000; 
	SetWaitableTimer(m_evTimerExec, (LARGE_INTEGER *)&ts, 300000, NULL, NULL, FALSE);

	//// イベント設定
	HANDLE hArray[] = { m_evTimerExec
						};
	SetEventNum(sizeof(hArray)/sizeof(HANDLE), hArray);
	return 0;
}

//------------------------------------------
// スレッド終了前処理 (終了シグナル後にコールバック)
//------------------------------------------
int SampManager::ThreadLast()
{
	// 定周期停止
	CancelWaitableTimer(m_evTimerExec);
	return 0;
}

//------------------------------------------
// スレッドイベント発生
// int nEventNo イベントNo (0オリジン)
//------------------------------------------
void SampManager::ThreadEvent(int nEventNo)
{
	////// シグナルの条件
	enum {	EV_TIMER_EXEC = 0,				// 処理実行 定周期タイマ
	};

	////// シグナル条件分け
	switch (nEventNo) {

//-----------------------------------------------------------------------------------------------
	case EV_TIMER_EXEC: 					// 処理実行 定周期タイマ
		LOG(em_MSG), "[%s] Thread Exe=%d", my_sThreadName, nEventNo );
		SampExec();
		return;

	
//-----------------------------------------------------------------------------------------------
	default :
		// ありえない！！
		LOG(em_ERR), "[%s] ThreadEvent EventNo NG=%d", my_sThreadName, nEventNo );
		return;
	}
}

//------------------------------------------
// 試験処理
//------------------------------------------
int SampManager::SampExec()
{
	return 0;
}

