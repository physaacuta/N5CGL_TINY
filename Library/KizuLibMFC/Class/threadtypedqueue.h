// *********************************************************************************
//	��Œ�̃X���b�h�L���[ �N���X
//	[Ver]
//		Ver.01    2007/01/30  vs2005 �Ή� (�R���X�g���N�^�ŃT�C�Y�w��ɕύX)
//
//	[����]
//
// *********************************************************************************

#if !defined(AFX_THREADTYPEDQUEUE_H__991FE91A_A838_40BC_B0FA_629EB6E83831__INCLUDED_)
#define AFX_THREADTYPEDQUEUE_H__991FE91A_A838_40BC_B0FA_629EB6E83831__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

namespace KizuLib
{
	template <class data_t> 
	class ThreadTypedQueue : public CTypedPtrList< CPtrList, data_t *>
	{
	public:
		ThreadTypedQueue(int SemCnt=256);									// �R���X�g���N�^
		virtual ~ThreadTypedQueue();										// �f�X�g���N�^ (�v�f�͉������Ȃ�)
		
		
		// �v�f����
		BOOL Alloc(int n);													// �G���A�m�� (�L���[�̃N���X��Alloc���\�b�h���������Ă�����)
		void Free();														// �G���A�J�� (�L���[�̃N���X��Free���\�b�h���������Ă�����)
		BOOL AddToTail( data_t * p);										// �v�f�𖖔��Ɉ�ǉ�
		data_t * GetFromHead(DWORD waitTime);								// �擪�v�f���o��(�Z�}�t�H�҂��o�[�W����)
		data_t * GetFromHead();												// �擪�v�f���o��(�҂��Ȃ��o�[�W����) (�Z�}�t�H������Ȃ����璍�ӂ��鎖)
		

		// �v���p�e�B
		BOOL IsEmpty()		const {return CPtrList::IsEmpty();};			// �󂩂ǂ���
		int GetCount()		const {return (int)CPtrList::GetCount();};		// ���݂̓o�^����
		int GetSemCount()	const {return my_nSemCount;};					// �o�^�o����ő匏��

		// ���J�ϐ�
		HANDLE					g_evSem;									// �Z�}�t�H �n���h��

	protected:
		CRITICAL_SECTION		my_cs;										// �N���e�B�J���Z�N�V����
		int						my_nSemCount;								// �Z�}�t�H�T�C�Y

	};

	// //////////////////////////////////////////////////////////////////////////////
	// �e���v���[�g�Ȃ̂Ńw�b�_�t�@�C���ɓ���Ă����K�v����
	// //////////////////////////////////////////////////////////////////////////////

	//------------------------------------------
	// �R���X�g���N�^
	// int SemCnt �Z�}�t�H�̃T�C�Y (�f�t�H���g256)
	//------------------------------------------
	template <class data_t>
	ThreadTypedQueue<data_t>::ThreadTypedQueue(int SemCnt) :
	my_nSemCount(SemCnt)
	{
		// �C���X�^���X����
		g_evSem = CreateSemaphore(NULL, 0, my_nSemCount, NULL);
		InitializeCriticalSection(&my_cs);							// �N���e�B�J���Z�N�V�����I�u�W�F�N�g��������

	}
	//------------------------------------------
	// �f�X�g���N�^
	//------------------------------------------
	template <class data_t>
	ThreadTypedQueue<data_t>::~ThreadTypedQueue()
	{
		CloseHandle(g_evSem);
		DeleteCriticalSection(&my_cs);								// �N���e�B�J���Z�N�V�����I�u�W�F�N�g���J��
	}

	//------------------------------------------
	// �G���A�m��
	// int n �m�ۂ���z��
	//------------------------------------------
	template <class data_t>
	BOOL ThreadTypedQueue<data_t>::Alloc(int n)
	{
		//// �����Z�b�g & ������
		data_t *p;

		// ��̊�p��
		for (int ii = 0; ii < n; ii++) {
			p = new data_t;
			p->Alloc();							// ���������N���X�̃��b�\�h�����s (�x���o�C���f�B���O)
			AddToTail(p);		// ���ǉ�
		}
		return TRUE;
	}
	//------------------------------------------
	// �G���A�J��
	//------------------------------------------
	template <class data_t>
	void ThreadTypedQueue<data_t>::Free()
	{
		// �L���[�� �擾
		int n = GetCount();

		for (int ii = 0; ii < n; ii++) {
			data_t *p = GetFromHead(0);			// �Z�}�t�H���܂߂ăN���A
			ASSERT(p);
			p->Free();							// ���o�����N���X�̃��b�\�h�����s (�x���o�C���f�B���O)
			delete p;
		}
	}

	//------------------------------------------
	// �v�f�𖖔��Ɉ�ǉ�
	// data_t * p �o�^����L���[
	// �߂�l ���A���
	//------------------------------------------
	template <class data_t>
	BOOL ThreadTypedQueue<data_t>::AddToTail(data_t * p)
	{
		BOOL ret;
		EnterCriticalSection(&my_cs);
		ret = ReleaseSemaphore(g_evSem, 1, NULL);
		AddTail(p);
		LeaveCriticalSection(&my_cs);
		ASSERT(ret);
		return ret;
	}

	//------------------------------------------
	// �擪�v�f���o��(�Z�}�t�H�҂��o�[�W����)
	// DWORD waitTime �҂�����
	// �߂�l ���o�����C���X�^���X
	//------------------------------------------
	template <class data_t>
	data_t * ThreadTypedQueue<data_t>::GetFromHead(DWORD waitTime)
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
	data_t * ThreadTypedQueue<data_t>::GetFromHead()
	{
		data_t *p = NULL;

		EnterCriticalSection(&my_cs);
		if (!IsEmpty()) {
			p = RemoveHead();
		}
		LeaveCriticalSection(&my_cs);

		return p;
	}
};
#endif // !defined(AFX_THREADTYPEDQUEUE_H__991FE91A_A838_40BC_B0FA_629EB6E83831__INCLUDED_)
