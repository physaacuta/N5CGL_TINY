// *********************************************************************************
//	ThreadLoopQueue���g�p���� ��]�����`���̃X���b�h�L���[ �N���X
//	[Ver]
//		Ver.01    2007/01/30  vs2005 �Ή� (�R���X�g���N�^�ŃT�C�Y�w��ɕύX)
//		Ver.02	  2010/01/22  ThreadTypedQueue.h���g��Ȃ��悤�ɂ���
//
//	[����]
//
// *********************************************************************************

#pragma once

#include "..\..\KizuLib\Class\ThreadQueue.h"								// �X���b�h�L���[ �N���X

namespace KizuLib
{
	//------------------------------------------
	// ��m�ۗp�̃C���^�[�t�F�[�X
	//------------------------------------------
	interface IThreadLoopItem
	{
	public:
		virtual	void Alloc(int const* p=0) = 0;								// ��̐���
		virtual	void Free() = 0;											// ��̉��
	};


	//------------------------------------------
	// ��]�����`���̃X���b�h�L���[ �N���X
	//------------------------------------------
	template <class data_t> 
	class ThreadLoopQueue
	{
	public:
		ThreadLoopQueue(int SemCnt=256);									// �R���X�g���N�^
		virtual ~ThreadLoopQueue();											// �f�X�g���N�^ (�v�f�͉������Ȃ�)

		// �� ����
		BOOL QueueAlloc(int n, int const* p=0);								// ��̊퐶�� (�L���[�̃N���X��Alloc���\�b�h���������Ă�����)
		void QueueFree();													// ����J��   (�L���[�̃N���X��Free���\�b�h���������Ă�����)
		
		// �v�f����
		BOOL AddFree( data_t * p) {return mque_Free.AddToTail(p);};						// �H�׏I�������̊��ԋp
//		BOOL AddFree( data_t * p) {return mque_Free.AddToHead(p);};						// �H�׏I�������̊��ԋp
		BOOL AddPool( data_t * p) {return mque_Pool.AddToTail(p);};						// ����Z�b�g������L���o�^	
		data_t * GetFree(DWORD waitTime) {return mque_Free.GetFromHead(waitTime);};		// ��̊���擾 (�Z�}�t�H�҂��o�[�W����)
		data_t * GetFree() {return mque_Free.GetFromHead();};							// ��̊���擾 (�҂��Ȃ��o�[�W����)
		data_t * GetPool() {return mque_Pool.GetFromHead();};							// ��L����擾 (�C�x���g�҂������o�[�W�����Ȃ̂� WaitForMultipleObjects �ŃZ�}�t�H�����Z�����Ă����K�v����)

		// �v���p�e�B
		int GetSemCount()  const {return mque_Free.GetSemCount();}			// �o�^�o����ő匏��

		int GetCountFree() const {return (int)mque_Free.GetCount();};		// ���݂̋�̊퐔
		int GetCountPool() const {return (int)mque_Pool.GetCount();};		// ���݂̋�L�퐔
		int GetRunningCount() const { return mque_Free.GetSemCount() - (mque_Free.GetCount() + mque_Pool.GetCount()); }	// ���� �ǂ����Ŏ��o���Ďg���Ă����̐�
		int GetMaxCountPool()	const {return mque_Pool.GetMaxCount();};	// �u�ԍő�o�^�����擾
		void ReSetMaxCountPool()	  { mque_Pool.ReSetMaxCount(); };		// �u�ԍő�o�^����������


		// �r������p ���O���Ő��䂷�邽�߁A�����͏\�����ӂ��K�v
		void EnterCS() { EnterCriticalSection(&m_csSeq); }
		void LeaveCS() { LeaveCriticalSection(&m_csSeq); }


		// ���J�ϐ�
		HANDLE					g_evSemFree;								// ��̊�L���[�̃Z�}�t�H �n���h��
		HANDLE					g_evSemPool;								// ��L��L���[�̃Z�}�t�H �n���h��


	private:
		// �e�C���X�^���X
		ThreadQueue<data_t>		mque_Free;									// ��̊�
		ThreadQueue<data_t>		mque_Pool;									// ��L��

		CRITICAL_SECTION		m_csSeq;									// �N���e�B�J���Z�N�V���� (��A�̗���p)
	};


	// //////////////////////////////////////////////////////////////////////////////
	// �e���v���[�g�Ȃ̂Ńw�b�_�t�@�C���ɓ���Ă����K�v����
	// //////////////////////////////////////////////////////////////////////////////

	//------------------------------------------
	// �R���X�g���N�^
	// int SemCnt �Z�}�t�H�̃T�C�Y (�f�t�H���g256)
	//------------------------------------------
	template <class data_t>
	ThreadLoopQueue<data_t>::ThreadLoopQueue(int SemCnt) :
	mque_Free(SemCnt),
	mque_Pool(SemCnt)
	{
		g_evSemFree = mque_Free.g_evSem;
		g_evSemPool = mque_Pool.g_evSem;

		InitializeCriticalSection(&m_csSeq);
	}

	//------------------------------------------
	// �f�X�g���N�^
	//------------------------------------------
	template <class data_t>
	ThreadLoopQueue<data_t>::~ThreadLoopQueue()
	{
		DeleteCriticalSection(&m_csSeq);
	}

	//------------------------------------------
	// ��̊퐶�� (�L���[�̃N���X��Alloc���\�b�h���������Ă�����)
	// int n �m�ۂ���z��
	// int const* p �t�т�����
	// �߂�l ���A���
	//------------------------------------------
	template <class data_t>
	BOOL ThreadLoopQueue<data_t>::QueueAlloc(int n, int const* p)
	{	
		data_t *pNewData;

		if(n > mque_Free.GetSemCount() ) return FALSE;

		//// ��̊���쐬
		for (int ii = 0; ii < n; ii++) {
			pNewData = new data_t;
			((IThreadLoopItem*)pNewData)->Alloc(p);	// ���������N���X�̃��b�\�h�����s (�x���o�C���f�B���O)
			mque_Free.AddToTail(pNewData);			// ���ǉ�
		}
		return TRUE;
	}

	//------------------------------------------
	// ����J�� (�L���[�̃N���X��Free���\�b�h���������Ă�����)
	//------------------------------------------
	template <class data_t>
	void ThreadLoopQueue<data_t>::QueueFree()
	{
		data_t *p = NULL;
		int		n = 0;

		//// ���S�ĊJ��
		n = mque_Pool.GetCount();
		for (int ii = 0; ii < n; ii++) {
			p = mque_Pool.GetFromHead(0);
			ASSERT(p);
			((IThreadLoopItem*)p)->Free();	// ���������N���X�̃��b�\�h�����s (�x���o�C���f�B���O)
			delete p;
			p = NULL;
		}

		n = mque_Free.GetCount();
		for (int ii = 0; ii < n; ii++) {
			p = mque_Free.GetFromHead(0);
			ASSERT(p);
			((IThreadLoopItem*)p)->Free();	// ���������N���X�̃��b�\�h�����s (�x���o�C���f�B���O)
			delete p;
			p = NULL;
		}
	}
};
