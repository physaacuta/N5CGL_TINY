#include "StdAfx.h"
#include "ThreadManager.h"


//////////////////////////////////////////////////////////////////////
// 構築/消滅
//////////////////////////////////////////////////////////////////////
//------------------------------------------
// コンストラクタ
// const char* pThreadName 自スレッド名
// int nMaxWaitNum WaitForMultipleObjectsの最大待ち行列
//------------------------------------------
ThreadManager::ThreadManager(const char* pThreadName, int nMaxWaitNum) :
my_sThreadName(pThreadName),
PARAM_MAX_WAIT_NUM(nMaxWaitNum),
my_pEvWait(NULL),
my_nWaitNum(0),
my_nWaitNumNow(0),
mcls_pLog(NULL)
{
	// 領域確保
	my_pEvWait = new HANDLE[PARAM_MAX_WAIT_NUM];
	for(int ii=0; ii<PARAM_MAX_WAIT_NUM; ii++) my_pEvWait[ii] = INVALID_HANDLE_VALUE; 
}

//------------------------------------------
// デストラクタ
//------------------------------------------
ThreadManager::~ThreadManager(void)
{
	SAFE_DELETE_ARRAY(my_pEvWait);
	my_pEvWait = NULL;
}

//------------------------------------------
// イベントセット
// int num セットするシグナル数
// LPHANDLE pWait シグナルの配列
//------------------------------------------
bool ThreadManager::SetEventNum(int num, LPHANDLE pWait)
{ 
	if(PARAM_MAX_WAIT_NUM <= num) return false;
	memcpy(&my_pEvWait[1], pWait, sizeof(HANDLE)*num);						// 終了シグナルを考慮
	my_nWaitNum = num;	
	my_nWaitNumNow = my_nWaitNum;
	return true;
}

//------------------------------------------
// イベントの追加 (1コだけ)
// HANDLE pWait シグナル  (関数の戻り値に対してのとき)
//------------------------------------------
bool ThreadManager::AddEventNum(HANDLE pWait)
{ 
	if(PARAM_MAX_WAIT_NUM <= (my_nWaitNum+1) ) return false;

	memcpy(&my_pEvWait[1+my_nWaitNum], &pWait, sizeof(HANDLE));			// 終了シグナルを考慮
	my_nWaitNum += 1;	
	my_nWaitNumNow = my_nWaitNum;
	return true;
}

//------------------------------------------
// イベントの追加 (配列)
// int num セットするシグナル数
// LPHANDLE pWait シグナルの配列
//------------------------------------------
bool ThreadManager::AddEventNum(int num, LPHANDLE pWait)
{ 
	if(PARAM_MAX_WAIT_NUM <= (my_nWaitNum+num) ) return false;
	memcpy(&my_pEvWait[1+my_nWaitNum], pWait, sizeof(HANDLE)*num);			// 終了シグナルを考慮
	my_nWaitNum += num;	
	my_nWaitNumNow = my_nWaitNum;
	return true;
}
//------------------------------------------
// WaitForの待ち数を変更 (通信タスク等で使用する為)
// int num セットするシグナル数
//------------------------------------------
bool ThreadManager::ChangeWaitNum(int num)
{ 
	if(PARAM_MAX_WAIT_NUM <= num) return false;
	my_nWaitNumNow = num;	
	return true;
}

//------------------------------------------
// スレッド
//------------------------------------------
UINT ThreadManager::MainThread(LPVOID param)
{

/*
	int nEventNo;
	int nRetc;

	////// メインインスタンス取り出し
	ThreadManager *p = (ThreadManager *)param;
	p->SetTid();									// 自スレッドIDセット
	nRetc = p->ThreadFirst();						// スレッド開始をコールバック

	//// WaitForMultipleObjectsイベント
	p->my_pEvWait[0] = p->my_evStop;
	

	pLOG(em_MSG), "[%s] スレッド開始=%d [0x%X]", p->my_sThreadName, nRetc, p->GetTid());
	while( 1 ) {
		DWORD ret = WaitForMultipleObjects((p->my_nWaitNum+1), p->my_pEvWait, FALSE, INFINITE);
		nEventNo = ret - WAIT_OBJECT_0; 

//-----------------------------------------------------------------------------------------------
		// スレッド終了
		if( 0 == nEventNo ) {
			nRetc = p->ThreadLast();				// スレッド終了をコールバック
			pLOG(em_MSG), "[%s] スレッド終了=%d [0x%X]", p->my_sThreadName, nRetc, p->GetTid());
			return nRetc;
		}

//-----------------------------------------------------------------------------------------------
		// 受信
		if( 1 <= nEventNo && nEventNo <= p->my_nWaitNum ) {
			p->ThreadEvent(nEventNo-1);				// イベントをコールバック
			continue;
		}
	}

	// ありえない
	pLOG(em_ERR), "[%s] メインループから抜けた！！=%d [0x%X]", p->my_sThreadName, nRetc, p->GetTid());
	return -1;
*/

	////// メインインスタンス取り出し
	ThreadManager *p = (ThreadManager *)param;
	p->SetTid();									// 自スレッドIDセット
	p->SetThreadName( p->GetThreadName());			// 自スレッド名の設定


	//// WaitForMultipleObjectsイベント
	p->my_pEvWait[0] = p->my_evStop;
	
	//// メインループでブロック
	return p->ThreadMainLoop();
}

//------------------------------------------
// スレッドメインループ
//------------------------------------------
int	ThreadManager::ThreadMainLoop()
{
	int nEventNo;
	int nRetc;

	nRetc = ThreadFirst();						// スレッド開始をコールバック
	if (INVALID_HANDLE_VALUE != my_hThreadReady) { SetEvent(my_hThreadReady); }		// 実行待機版

	LOG(em_MSG), "[%s] スレッド開始=%d [0x%X]", my_sThreadName, nRetc, GetTid());
	while( 1 ) {
		//// シグナル待ち数変更
		ThreadWaitFor(my_nWaitNum, &my_nWaitNumNow);

		//// シグナル待ち
		DWORD ret = WaitForMultipleObjects((my_nWaitNumNow+1), my_pEvWait, FALSE, INFINITE);
		nEventNo = ret - WAIT_OBJECT_0; 

//-----------------------------------------------------------------------------------------------
		// スレッド終了
		if( 0 == nEventNo ) {
			nRetc = ThreadLast();				// スレッド終了をコールバック
			LOG(em_MSG), "[%s] スレッド終了=%d [0x%X]", my_sThreadName, nRetc, GetTid());
			return nRetc;
		}

//-----------------------------------------------------------------------------------------------
		// 受信
		if( 1 <= nEventNo && nEventNo <= my_nWaitNum ) {
			ThreadEvent(nEventNo-1);				// イベントをコールバック
			continue;
		}
	}

	// ありえない
	LOG(em_ERR), "[%s] メインループから抜けた！！=%d [0x%X]", my_sThreadName, nRetc, GetTid());
	return -1;
}