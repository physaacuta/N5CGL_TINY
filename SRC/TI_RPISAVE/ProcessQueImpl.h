// *********************************************************************************
//	各種キューを、IQueue<T>インターフェースで統一的にとり扱うためのラッパークラス群
//	[Ver]
//		Ver.01    2013/07/09	初版
//
//	[メモ]
// *********************************************************************************

#pragma once

#include "..\..\Library\KizuLib\CLASS\ThreadQueue.h"					// キュー
#include "..\..\Library\KizuLibMFC\CLASS\ThreadLoopQueue.h"				// 回転寿司形式のスレッドキュー
#include "..\..\Library\KizuLibMFC\Class\ThreadSyncManager.h"			// 並列スレッド間順序保障制御クラス


/*/ メモリーリーク検出 ----->>>
#ifdef _DEBUG
#undef THIS_FILE
#define new DEBUG_NEW
#endif
//*/// <<<------------------------

using namespace KizuLib;


//======================================================================
// Queueインターフェース
// CProcessQueueThreadが使う
template<class Tque>			// 操作対象Que
interface IQueue
{
	virtual ~IQueue() {};
	virtual HANDLE GetEvQue() = 0 ;										// シグナル
	virtual int GetCount() = 0 ;										// Que現在件数
	virtual int GetMaxCount() = 0 ;										// Que最大件数

	virtual Tque* GetFromHead( void* arg=NULL ) = 0;					// キュー取得 ※ argは現在 ThreadSyncManagerの evEnd しか無し
	virtual BOOL AddToTail( Tque* pQueData, void *arg=NULL ) = 0;		// 返却 ※ argは現在 ThreadSyncManagerの evEnd しか無し
};

//横流しキュー。基本的に継承して、フックに使う。
template<class T> 
class CInterceptQueue : implements IQueue<T>
{
protected:
	IQueue<T>*		mque_pQue;								// Que本体

public:
	CInterceptQueue(IQueue<T> *pQue ) :			// コンストラクタ
	mque_pQue(pQue)
	{}

	virtual HANDLE GetEvQue() { return mque_pQue->GetEvQue(); }
	virtual int GetCount() { return mque_pQue->GetCount(); }
	virtual int GetMaxCount() { return mque_pQue->GetMaxCount(); }

	virtual T* GetFromHead(void* arg=NULL) {  
		return mque_pQue->GetFromHead(arg);
	}
	virtual BOOL AddToTail( T* pQueData, void *arg=NULL) {
		return mque_pQue->AddToTail(pQueData, arg);
	}
};

// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 継承 版 (IQueueサポート版)

//======================================================================
// ThreadQue
template<class T> 
class ThreadQueueImpl : public ThreadQueue<T>,
						implements IQueue<T>
{
public:

	virtual HANDLE GetEvQue() { return ThreadQueue::g_evSem; }
	virtual int GetCount() { return ThreadQueue::GetCount(); }
	virtual int GetMaxCount() { return ThreadQueue::GetMaxCount(); }

	virtual T* GetFromHead(void* arg=NULL) {  
		return ThreadQueue::GetFromHead();
	}
	virtual BOOL AddToTail( T* pQueData, void *arg=NULL) {
		return ThreadQueue::AddToTail(pQueData);
	}
};

//======================================================================
// ThreadLoopQueue 
template<class T> 
class ThreadLoopQueueImpl : public ThreadLoopQueue<T>
{
public:
	IQueue<T>*		gIQue_pFree;							// 空きキュー側
	IQueue<T>*		gIQue_pPool;							// 空きキュー側

public:
	ThreadLoopQueueImpl( int SemCnt=256 ) :					// コンストラクタ
	ThreadLoopQueue(SemCnt)
	{
		gIQue_pFree = new CThreadLoopQueue_FreeQue<T>(this);
		gIQue_pPool = new CThreadLoopQueue_PoolQue<T>(this);
	}

	virtual ~ThreadLoopQueueImpl()								// デストラクタ
	{
		// 今回作ったやつを解放
		delete gIQue_pFree;
		delete gIQue_pPool;
	}
};

//======================================================================
// ThreadSyncManager 
template<class T> 
class ThreadSyncManagerImpl : public ThreadSyncManager<T>
{
public:
	IQueue<T>*		gIQue_pExec;							// 実行キュー側
	IQueue<T>*		gIQue_pEnd;								// 順序キュー側

public:
	ThreadSyncManagerImpl(const char* pThreadName="", int InSemCnt = 256, int OutSemCnt = 512, int nMaxWaitNum = 50) :	// コンストラクタ
	ThreadSyncManager(pThreadName,InSemCnt,OutSemCnt,nMaxWaitNum)
	{
		gIQue_pExec = new CThreadSyncManager_ExecQue<T>(this);
		gIQue_pEnd = new CThreadSyncManager_EndQue<T>(this);
	}

	virtual ~ThreadSyncManagerImpl()						// デストラクタ
	{
		// 今回作ったやつを解放
		delete gIQue_pExec;
		delete gIQue_pEnd;
	}
};



// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ラッパー 版

//======================================================================
//ThreadQue
template<class T> 
class CThreadQueue : implements IQueue<T>
{
protected:
	ThreadQueue<T>*		mque_pQue;								// Que本体

public:
	CThreadQueue(ThreadLoopQueue<T> *pQue ) :			// コンストラクタ
	mque_pQue(pQue)
	{}

	virtual HANDLE GetEvQue() { return mque_pQue->g_evSem; }
	virtual int GetCount() { return mque_pQue->GetCount(); }
	virtual int GetMaxCount() { return mque_pQue->GetMaxCount(); }

	virtual T* GetFromHead(void* arg=NULL) {  
		return mque_pQue->GetHead();
	}
	virtual BOOL AddToTail( T* pQueData, void *arg=NULL) {
		return mque_pQue->AddToTail(pQueData);
	}
};

//======================================================================
//ThreadLoopQueue の 空きキュー側
template<class T> 
class CThreadLoopQueue_FreeQue : implements IQueue<T>
{
protected:
	ThreadLoopQueue<T>*		mque_pLoop;								// Que本体

public:
	CThreadLoopQueue_FreeQue(ThreadLoopQueue<T> *pQue ) :			// コンストラクタ
	mque_pLoop(pQue)
	{}

	virtual HANDLE GetEvQue() { return mque_pLoop->g_evSemFree; }
	virtual int GetCount() { return mque_pLoop->GetCountFree(); }
	virtual int GetMaxCount() { return 0; }

	virtual T* GetFromHead(void* arg=NULL) {  
		return mque_pLoop->GetFree();
	}
	virtual BOOL AddToTail( T* pQueData, void *arg=NULL) {
		return mque_pLoop->AddFree(pQueData);
	}
};

//======================================================================
//ThreadLoopQueue の 具有りキュー側
template<class T> 
class CThreadLoopQueue_PoolQue : implements IQueue<T>
{
protected:
	ThreadLoopQueue<T>*		mque_pLoop;								// Que本体

public:
	CThreadLoopQueue_PoolQue(ThreadLoopQueue<T> *pQue ) :			// コンストラクタ
	mque_pLoop(pQue)
	{}

	virtual HANDLE GetEvQue() { return mque_pLoop->g_evSemPool; }
	virtual int GetCount() { return mque_pLoop->GetCountPool(); }
	virtual int GetMaxCount() { return mque_pLoop->GetMaxCountPool(); }

	virtual T* GetFromHead(void* arg=NULL) {  
		return mque_pLoop->GetPool();
	}
	virtual BOOL AddToTail( T* pQueData, void *arg=NULL) {
		return mque_pLoop->AddPool(pQueData);
	}
};

//======================================================================
//ThreadSyncManager の 実行キュー側 (In側)
template<class T> 
class CThreadSyncManager_ExecQue : implements IQueue<T>
{
protected:
	ThreadSyncManager<T>*		mque_pSync;								// Que本体

public:
	CThreadSyncManager_ExecQue(ThreadSyncManager<T> *pQue ) :			// コンストラクタ
	mque_pSync(pQue)
	{}

	virtual HANDLE GetEvQue() { return mque_pSync->GetExecQueEvSem(); }
	virtual int GetCount() { return mque_pSync->GetSyncQueCount(); }
	virtual int GetMaxCount() { return mque_pSync->GetSyncQueMaxCount(); }

	virtual T* GetFromHead(void* evEnd) {  
		return mque_pSync->GetExecQueFromHead((HANDLE*)evEnd);
	}
	virtual BOOL AddToTail( T* pQueData, void *arg=NULL) {
		return mque_pSync->AddToTail(pQueData);
	}
};

//======================================================================
//ThreadSyncManager の 順序キュー側 (Out側)
template<class T> 
class CThreadSyncManager_EndQue : implements IQueue<T>
{
protected:
	ThreadSyncManager<T>*		mque_pSync;								// Que本体

public:
	CThreadSyncManager_EndQue(ThreadSyncManager<T> *pQue ) :			// コンストラクタ
	mque_pSync(pQue)
	{}

	virtual HANDLE GetEvQue() { return mque_pSync->GetEndQueEvSem(); }
	virtual int GetCount() { return mque_pSync->GetEndQueCount(); }
	virtual int GetMaxCount() { return mque_pSync->GetEndQueMaxCount(); }

	virtual T* GetFromHead(void* arg=NULL) {  
		return mque_pSync->GetEndQueFromHead();
	}
	virtual BOOL AddToTail( T* pQueData, void *evEnd) {
		// pQueData は使わない
		mque_pSync->SetExecQueEnd((HANDLE)evEnd);
		return TRUE;
	}
};
