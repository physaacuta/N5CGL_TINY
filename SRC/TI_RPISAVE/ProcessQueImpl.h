// *********************************************************************************
//	�e��L���[���AIQueue<T>�C���^�[�t�F�[�X�œ���I�ɂƂ舵�����߂̃��b�p�[�N���X�Q
//	[Ver]
//		Ver.01    2013/07/09	����
//
//	[����]
// *********************************************************************************

#pragma once

#include "..\..\Library\KizuLib\CLASS\ThreadQueue.h"					// �L���[
#include "..\..\Library\KizuLibMFC\CLASS\ThreadLoopQueue.h"				// ��]���i�`���̃X���b�h�L���[
#include "..\..\Library\KizuLibMFC\Class\ThreadSyncManager.h"			// ����X���b�h�ԏ����ۏᐧ��N���X


/*/ �������[���[�N���o ----->>>
#ifdef _DEBUG
#undef THIS_FILE
#define new DEBUG_NEW
#endif
//*/// <<<------------------------

using namespace KizuLib;


//======================================================================
// Queue�C���^�[�t�F�[�X
// CProcessQueueThread���g��
template<class Tque>			// ����Ώ�Que
interface IQueue
{
	virtual ~IQueue() {};
	virtual HANDLE GetEvQue() = 0 ;										// �V�O�i��
	virtual int GetCount() = 0 ;										// Que���݌���
	virtual int GetMaxCount() = 0 ;										// Que�ő匏��

	virtual Tque* GetFromHead( void* arg=NULL ) = 0;					// �L���[�擾 �� arg�͌��� ThreadSyncManager�� evEnd ��������
	virtual BOOL AddToTail( Tque* pQueData, void *arg=NULL ) = 0;		// �ԋp �� arg�͌��� ThreadSyncManager�� evEnd ��������
};

//�������L���[�B��{�I�Ɍp�����āA�t�b�N�Ɏg���B
template<class T> 
class CInterceptQueue : implements IQueue<T>
{
protected:
	IQueue<T>*		mque_pQue;								// Que�{��

public:
	CInterceptQueue(IQueue<T> *pQue ) :			// �R���X�g���N�^
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
// �p�� �� (IQueue�T�|�[�g��)

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
	IQueue<T>*		gIQue_pFree;							// �󂫃L���[��
	IQueue<T>*		gIQue_pPool;							// �󂫃L���[��

public:
	ThreadLoopQueueImpl( int SemCnt=256 ) :					// �R���X�g���N�^
	ThreadLoopQueue(SemCnt)
	{
		gIQue_pFree = new CThreadLoopQueue_FreeQue<T>(this);
		gIQue_pPool = new CThreadLoopQueue_PoolQue<T>(this);
	}

	virtual ~ThreadLoopQueueImpl()								// �f�X�g���N�^
	{
		// ��������������
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
	IQueue<T>*		gIQue_pExec;							// ���s�L���[��
	IQueue<T>*		gIQue_pEnd;								// �����L���[��

public:
	ThreadSyncManagerImpl(const char* pThreadName="", int InSemCnt = 256, int OutSemCnt = 512, int nMaxWaitNum = 50) :	// �R���X�g���N�^
	ThreadSyncManager(pThreadName,InSemCnt,OutSemCnt,nMaxWaitNum)
	{
		gIQue_pExec = new CThreadSyncManager_ExecQue<T>(this);
		gIQue_pEnd = new CThreadSyncManager_EndQue<T>(this);
	}

	virtual ~ThreadSyncManagerImpl()						// �f�X�g���N�^
	{
		// ��������������
		delete gIQue_pExec;
		delete gIQue_pEnd;
	}
};



// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ���b�p�[ ��

//======================================================================
//ThreadQue
template<class T> 
class CThreadQueue : implements IQueue<T>
{
protected:
	ThreadQueue<T>*		mque_pQue;								// Que�{��

public:
	CThreadQueue(ThreadLoopQueue<T> *pQue ) :			// �R���X�g���N�^
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
//ThreadLoopQueue �� �󂫃L���[��
template<class T> 
class CThreadLoopQueue_FreeQue : implements IQueue<T>
{
protected:
	ThreadLoopQueue<T>*		mque_pLoop;								// Que�{��

public:
	CThreadLoopQueue_FreeQue(ThreadLoopQueue<T> *pQue ) :			// �R���X�g���N�^
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
//ThreadLoopQueue �� ��L��L���[��
template<class T> 
class CThreadLoopQueue_PoolQue : implements IQueue<T>
{
protected:
	ThreadLoopQueue<T>*		mque_pLoop;								// Que�{��

public:
	CThreadLoopQueue_PoolQue(ThreadLoopQueue<T> *pQue ) :			// �R���X�g���N�^
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
//ThreadSyncManager �� ���s�L���[�� (In��)
template<class T> 
class CThreadSyncManager_ExecQue : implements IQueue<T>
{
protected:
	ThreadSyncManager<T>*		mque_pSync;								// Que�{��

public:
	CThreadSyncManager_ExecQue(ThreadSyncManager<T> *pQue ) :			// �R���X�g���N�^
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
//ThreadSyncManager �� �����L���[�� (Out��)
template<class T> 
class CThreadSyncManager_EndQue : implements IQueue<T>
{
protected:
	ThreadSyncManager<T>*		mque_pSync;								// Que�{��

public:
	CThreadSyncManager_EndQue(ThreadSyncManager<T> *pQue ) :			// �R���X�g���N�^
	mque_pSync(pQue)
	{}

	virtual HANDLE GetEvQue() { return mque_pSync->GetEndQueEvSem(); }
	virtual int GetCount() { return mque_pSync->GetEndQueCount(); }
	virtual int GetMaxCount() { return mque_pSync->GetEndQueMaxCount(); }

	virtual T* GetFromHead(void* arg=NULL) {  
		return mque_pSync->GetEndQueFromHead();
	}
	virtual BOOL AddToTail( T* pQueData, void *evEnd) {
		// pQueData �͎g��Ȃ�
		mque_pSync->SetExecQueEnd((HANDLE)evEnd);
		return TRUE;
	}
};
