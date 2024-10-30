#include "StdAfx.h"
#include "SmyuPlgManager.h"

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
SmyuPlgManager::SmyuPlgManager(int nMyID) :
SmyuBaseManager("SmyuPlgMgr", nMyID)
{	
	//// ハンドル生成
	m_evTimerExec  = CreateWaitableTimer(NULL, FALSE, NULL);

	for(int ii=0; ii<SMYU_PLG_MAX_CHANNEL; ii++) m_nStoryIndex[ii] = 0;
}

//------------------------------------------
// デストラクタ
//------------------------------------------
SmyuPlgManager::~SmyuPlgManager(void)
{
	SmyuStop();

	//// ハンドル解放
	CloseHandle(m_evTimerExec);
}


//------------------------------------------
// スレッド開始前処理 (スレッド実行直後(シグナルセット前)にコールバック)
//------------------------------------------
int SmyuPlgManager::ThreadFirst()
{
	//// イベント設定
	HANDLE hArray[] = { m_evTimerExec
						};
	SetEventNum(sizeof(hArray)/sizeof(HANDLE), hArray);
	return 0;
}

//------------------------------------------
// スレッド終了前処理 (終了シグナル後にコールバック)
//------------------------------------------
int SmyuPlgManager::ThreadLast()
{
	// 定周期停止
	CancelWaitableTimer(m_evTimerExec);
	return 0;
}

//------------------------------------------
// スレッドイベント発生
// int nEventNo イベントNo (0オリジン)
//------------------------------------------
void SmyuPlgManager::ThreadEvent(int nEventNo)
{
	////// シグナルの条件
	enum {	EV_TIMER_EXEC = 0,				// 処理実行 定周期タイマ
	};

	////// シグナル条件分け
	switch (nEventNo) {

//-----------------------------------------------------------------------------------------------
	case EV_TIMER_EXEC: 					// 処理実行 定周期タイマ
		SmyuExec();
		return;

	
//-----------------------------------------------------------------------------------------------
	default :
		// ありえない！！
		LOG(em_ERR), "[%s] ThreadEvent EventNo NG=%d", my_sThreadName, nEventNo );
		return;
	}
}


// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 処理

//------------------------------------------
// シミュレータ スタート
//------------------------------------------
void SmyuPlgManager::SmyuStart()
{
	for(int ii=0; ii<SMYU_PLG_MAX_CHANNEL; ii++) m_nStoryIndex[ii] = 0;

	//// 事前準備
	__int64 ts = TIME_EXEC_INTERVAL * -10000; 
	SetWaitableTimer(m_evTimerExec, (LARGE_INTEGER *)&ts, TIME_EXEC_INTERVAL, NULL, NULL, FALSE);

}
//------------------------------------------
// シミュレータ ストップ
//------------------------------------------
void SmyuPlgManager::SmyuStop()
{
	CancelWaitableTimer(m_evTimerExec);
}

//------------------------------------------
// シミュレータ 実行
//------------------------------------------
void SmyuPlgManager::SmyuExec()
{
	double	dLen[SMYU_PLG_MAX_CHANNEL];
	int		nCnt = (int)m_lstStory.size();

	//================================================
	// 通板距離を取得
	for(int ii=0; ii<SMYU_PLG_MAX_CHANNEL; ii++) {
		dLen[ii] = mcls_pPlg->GetLength(ii); 
	}


	//================================================
	// シナリオ実行
	for(int ii=0; ii<SMYU_PLG_MAX_CHANNEL; ii++) {

		int nIndex = m_nStoryIndex[ii];						// 現在実行位置を取得
		for(int jj=nIndex; jj<nCnt; jj++) {
			if(DIV_KIND_PLG != m_lstStory[jj]->emRaise) goto jjcontnue;	// そもそも PLG でない。 通常ありえない

			//// 発生条件部 取得
			TYP_RAISE_PLG_POS* pD = (TYP_RAISE_PLG_POS*)m_lstStory[jj]->pRaise;
			if(ii != pD->nCh ) goto jjcontnue;							// 今回のChとは違う
			if( dLen[ii] < pD->nLen ) break;							// まだ、対象位置に到達していない


			//// イベント部チェック
			if( DIV_KIND_ZZ == m_lstStory[jj]->emEvent && 0 == m_lstStory[jj]->nEventSub ) {
				// リセット処理
				TYP_EVENT_ZZ_RESET* pE = (TYP_EVENT_ZZ_RESET*)m_lstStory[jj]->pEvent;
				
				// 通板距離を初期化
				dLen[ii] = 0.0;
				mcls_pPlg->ResetLength(pE->nCh);
				LOG(em_MSG), "[%s] CH=%d 通板距離リセット", my_sThreadName, pE->nCh);

				// シナリオも最初からとする
				if( 1 == pE->nMode ) {
					m_nStoryIndex[pE->nCh] = 0;	
					LOG(em_MSG), "[%s] CH=%d シナリオ先頭から実行", my_sThreadName, pE->nCh);
					break;
				}

			} else {

				// シナリオ 発生
				this->RaiseEvents(m_lstStory[jj]->emEvent, m_lstStory[jj]->nEventSub, (BYTE*)m_lstStory[jj]->pEvent);
			}
			
jjcontnue:
			// シナリオ実行位置を変更
			m_nStoryIndex[ii] = jj+1;
		}
	}
}