#include "StdAfx.h"
#include "SmyuTimManager.h"

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
SmyuTimManager::SmyuTimManager(int nMyID) :
SmyuBaseManager("SmyuTimMgr", nMyID)
{
	//// ハンドル生成
	for(int ii=0; ii<TIME_MAX_NUM; ii++) {
		m_evTimerExec[ii] = CreateWaitableTimer(NULL, FALSE, NULL);
	}
}

//------------------------------------------
// デストラクタ
//------------------------------------------
SmyuTimManager::~SmyuTimManager(void)
{
	SmyuStop();

	//// ハンドル解放
	for(int ii=0; ii<TIME_MAX_NUM; ii++) {
		CloseHandle(m_evTimerExec[ii]);
	}
}

//------------------------------------------
// スレッド開始前処理 (スレッド実行直後(シグナルセット前)にコールバック)
//------------------------------------------
int SmyuTimManager::ThreadFirst()
{
	//// イベント設定
	this->AddEventNum(TIME_MAX_NUM, m_evTimerExec);
	return 0;
}

//------------------------------------------
// スレッド終了前処理 (終了シグナル後にコールバック)
//------------------------------------------
int SmyuTimManager::ThreadLast()
{
	return 0;
}

//------------------------------------------
// スレッドイベント発生
// int nEventNo イベントNo (0オリジン)
//------------------------------------------
void SmyuTimManager::ThreadEvent(int nEventNo)
{
	////// シグナルの条件
	enum {	EV_TIMER_EXEC = 0,				// 処理実行 定周期タイマ
	};
 

	if( nEventNo >= EV_TIMER_EXEC && nEventNo < EV_TIMER_EXEC+TIME_MAX_NUM) {
		int nIndex = nEventNo - EV_TIMER_EXEC;

		SmyuExec(nIndex);
	}
}



// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 処理

//------------------------------------------
// シミュレータ スタート
//------------------------------------------
void SmyuTimManager::SmyuStart()
{
	//// 対象の発生条件部を特定
	for(int ii=0; ii<(int)m_lstStory.size(); ii++ ) {
		if(DIV_KIND_TIME != m_lstStory[ii]->emRaise ) continue;

		TYP_RAISE_TIM_CYCLE* pD = (TYP_RAISE_TIM_CYCLE*)m_lstStory[ii]->pRaise;
 		
		TimerExec(pD->nID, pD->nFirst, pD->nCycle );
	}
}

//------------------------------------------
// シミュレータ ストップ
//------------------------------------------
void SmyuTimManager::SmyuStop()
{
	for(int ii=0; ii<TIME_MAX_NUM; ii++) {
		CancelWaitableTimer(m_evTimerExec[ii]);
	}
}

//------------------------------------------
// シミュレータ 実行
// int nIndex 配列位置 
//------------------------------------------
void SmyuTimManager::SmyuExec(int nIndex)
{
	//// 同一タイマーIDのものを実行させる
	for(int ii=0; ii<(int)m_lstStory.size(); ii++ ) {
		if(DIV_KIND_TIME != m_lstStory[ii]->emRaise ) continue;

		TYP_RAISE_TIM_CYCLE* pD = (TYP_RAISE_TIM_CYCLE*)m_lstStory[ii]->pRaise;
		if( nIndex != pD->nID ) continue;		

		// シナリオ 発生
		this->RaiseEvents(m_lstStory[ii]->emEvent, m_lstStory[ii]->nEventSub, (BYTE*)m_lstStory[ii]->pEvent);
	}
}

//------------------------------------------
// タイマー操作
// int nId 配列位置
// int nFirst １回目時間
// int nCycle ２回目以降の繰り返し
//------------------------------------------
void SmyuTimManager::TimerExec(int nId, int nFirst, int nCycle)
{
	if(0 > nId || TIME_MAX_NUM <= nId ) return;

	if(0 < nFirst) {
		__int64 ts = nFirst * -10000; 
		SetWaitableTimer(m_evTimerExec[nId], (LARGE_INTEGER *)&ts, nCycle, NULL, NULL, FALSE);
	} else {
		CancelWaitableTimer(m_evTimerExec[nId]);
	}
}
