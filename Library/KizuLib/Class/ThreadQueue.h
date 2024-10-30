// *********************************************************************************
//	�X���b�h�L���[ �N���X
//	[Ver]
//		Ver.01    2007/01/30  vs2005 �Ή� (�R���X�g���N�^�ŃT�C�Y�w��ɕύX)
//
//	[����]
//		�킴�ƃ|�C���^�����g���Ȃ��悤�ɂ��Ă��܂��B�K���C���X�^���X��n���܂��傤�B
// *********************************************************************************

#pragma once

#include <deque>
#include <windows.h>
//using namespace std;

namespace KizuLib
{
	template <class data_t> class ThreadQueue
	{
	public:
		ThreadQueue(int SemCnt = 256);										// �R���X�g���N�^
		virtual ~ThreadQueue();												// �f�X�g���N�^ (�v�f�͉������Ȃ�)

		// �v�f����
		void Free();														// �S�v�f������idata_t�̃f�X�g���N�^���Ă�)
		BOOL AddToTail(data_t * newData);									// �v�f�𖖔��Ɉ�ǉ�
		BOOL AddToTail(data_t * newData, int retryCnt, int waitTime=1);		// �v�f�𖖔��Ɉ�ǉ� (�ǉ������܂ő҂�)
		BOOL AddToTailFreeCheck( data_t * newData, int FreeCount=0);		// �c�o�^�������`�F�b�N�� �v�f�𖖔��Ɉ�ǉ�
		BOOL AddToHead(data_t * newData);									// �v�f��擪�Ɉ�ǉ�
																			// �g���I���������̂��̂��g�����Ƃ�
																			// L2�L���b�V���������オ�邱�Ƃ�����
		data_t * GetFromHead(DWORD waitTime);								// �擪�v�f���o��(�Z�}�t�H�҂��o�[�W����)
		data_t * GetFromHead();												// �擪�v�f���o��(�҂��Ȃ��o�[�W����) (�Z�}�t�H������Ȃ����璍�ӂ��鎖)

		// �v���p�e�B
		BOOL IsEmpty()		const {return my_deque.empty();};				// �󂩂ǂ���
		int  GetCount()		const {return (int)my_deque.size();};			// ���݂̓o�^�����擾
		int  GetSemCount()	const {return my_nSemCount;};					// �o�^�o����ő匏��
		int  GetMaxCount()	const {return my_nMaxCount;};					// �u�ԍő�o�^�����擾
		void ReSetMaxCount()	  { my_nMaxCount = 0; };					// �u�ԍő�o�^����������
		int  GetFreeCount()	const {return GetSemCount() - GetCount();}		// �c�o�^����
		

		// ���J�ϐ�
		HANDLE					g_evSem;									// �Z�}�t�H �n���h��
		

	protected:
		CRITICAL_SECTION		my_cs;										// �N���e�B�J���Z�N�V����
		CRITICAL_SECTION		my_csSeq;									// �N���e�B�J���Z�N�V���� (��A�̗���p)
		std::deque<data_t*>		my_deque;									// �L���[
		int						my_nSemCount;								// �Z�}�t�H�T�C�Y
		int						my_nMaxCount;								// �u�ԍő�o�^����
	};


	// //////////////////////////////////////////////////////////////////////////////
	// �e���v���[�g�Ȃ̂Ńw�b�_�t�@�C���ɓ���Ă����K�v����
	// //////////////////////////////////////////////////////////////////////////////

	//------------------------------------------
	// �R���X�g���N�^
	// int SemCnt �Z�}�t�H�̃T�C�Y (�f�t�H���g256)
	//------------------------------------------
	template <class data_t>
	ThreadQueue<data_t>::ThreadQueue(int SemCnt) :
	my_nMaxCount(0),
	my_nSemCount(SemCnt)
	{
		g_evSem = CreateSemaphore(NULL, 0, my_nSemCount, NULL);
		InitializeCriticalSection(&my_cs);							// �N���e�B�J���Z�N�V�����I�u�W�F�N�g��������
		InitializeCriticalSection(&my_csSeq);
	}

	//------------------------------------------
	// �f�X�g���N�^
	//------------------------------------------
	template <class data_t>
	ThreadQueue<data_t>::~ThreadQueue()
	{
		Free();
		CloseHandle(g_evSem);
		DeleteCriticalSection(&my_cs);								// �N���e�B�J���Z�N�V�����I�u�W�F�N�g���J��
		DeleteCriticalSection(&my_csSeq);
	}

	//------------------------------------------
	// �S�v�f������idata_t�̃f�X�g���N�^���Ă�)
	//------------------------------------------
	template <class data_t>
	void ThreadQueue<data_t>::Free()
	{
		int n = GetCount();
		data_t * p;
		for (int i = 0; i < n; i++) {
			p = GetFromHead(0);
			ASSERT(p);
			delete p;
		}
		my_nMaxCount = 0;
	}

	//------------------------------------------
	// �v�f�𖖔��Ɉ�ǉ�
	// data_t * p �o�^����C���X�^���X
	// �߂�l ���A���
	//------------------------------------------
	template <class data_t>
	BOOL ThreadQueue<data_t>::AddToTail(data_t * newData)
	{
		BOOL ret;
		
		EnterCriticalSection(&my_cs);
		my_deque.push_back(newData);
		if (GetCount() > my_nMaxCount) my_nMaxCount = GetCount();
		ret = ReleaseSemaphore(g_evSem, 1, NULL);
		LeaveCriticalSection(&my_cs);
		ASSERT(ret);
		return ret;
	}

	//------------------------------------------
	// �v�f�𖖔��Ɉ�ǉ� (�ǉ������܂ő҂�)
	// data_t * p �o�^����C���X�^���X
	// int retryCnt ���g���C��
	// int waitTime Sleep�Ԋu [ms]
	// �߂�l ���A���
	//------------------------------------------
	template <class data_t>
	BOOL ThreadQueue<data_t>::AddToTail(data_t * newData, int retryCnt, int waitTime)
	{
		BOOL ret;
		
		EnterCriticalSection(&my_csSeq);

		for ( int ii=0; ii<retryCnt; ii++ ) {
			// �ǉ��\���`�F�b�N
			if (GetCount() < my_nSemCount ) {	// �ǉ��\
				ret = AddToTail(newData);
				LeaveCriticalSection(&my_csSeq);	
				return ret;
			} else {						// �҂�
				Sleep(waitTime);
			}
		}
		LeaveCriticalSection(&my_csSeq);
		return FALSE;
	}
	//------------------------------------------
	// �c�o�^�������`�F�b�N�� �v�f�𖖔��Ɉ�ǉ�
	// data_t * p �o�^����C���X�^���X
	// int FreeCount �󂫗e�ʁB���̐��l���傫���ꍇ�̂ݓo�^����B���l�ȉ��̏ꍇ�́A�o�^���s�Ƃ���
	// �߂�l ���A���
	//------------------------------------------
	template <class data_t>
	BOOL ThreadQueue<data_t>::AddToTailFreeCheck(data_t * newData, int FreeCount)
	{
		BOOL ret;
		
		EnterCriticalSection(&my_csSeq);
		if( FreeCount < GetFreeCount() ) {	
			ret = AddToTail(newData);
		} else {
			ret = FALSE;
		}
		LeaveCriticalSection(&my_csSeq);
		return ret;
	}


	//------------------------------------------
	// �v�f��擪�Ɉ�ǉ�
	// data_t * p �o�^����C���X�^���X
	// �߂�l ���A���
	//------------------------------------------
	template <class data_t>
	BOOL ThreadQueue<data_t>::AddToHead(data_t * newData)
	{
		BOOL ret;
		EnterCriticalSection(&my_cs);
		my_deque.push_front(newData);
		if (GetCount() > my_nMaxCount) my_nMaxCount = GetCount();	// �ő匏���X�V
		ret = ReleaseSemaphore(g_evSem, 1, NULL);
		LeaveCriticalSection(&my_cs);
		ASSERT(ret);
		return ret;
	}

	//------------------------------------------
	// �擪�v�f���o��(�Z�}�t�H�҂��o�[�W����)
	// DWORD waitTime �҂����� [ms]
	// �߂�l ���o�����C���X�^���X
	//------------------------------------------
	template <class data_t>
	data_t * ThreadQueue<data_t>::GetFromHead(DWORD waitTime)
	{
		if (WaitForSingleObject(g_evSem, waitTime) == WAIT_TIMEOUT) {
			return NULL;
		}
		return (GetFromHead());
	}

	//------------------------------------------
	// �擪�v�f���o��(�҂��Ȃ��o�[�W����) (�Z�}�t�H������Ȃ����璍�ӂ��鎖)
	// �߂�l ���o�����C���X�^���X
	//------------------------------------------
	template <class data_t>
	data_t * ThreadQueue<data_t>::GetFromHead()
	{
		data_t * p = NULL;
		EnterCriticalSection(&my_cs);
		if (!IsEmpty()) {
			p = (data_t *)my_deque.front();
			my_deque.pop_front();
		}
		LeaveCriticalSection(&my_cs);
		return p;
	}
};
