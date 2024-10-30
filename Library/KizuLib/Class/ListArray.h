// *********************************************************************************
//	�}���\�ȃX���b�h�L���[ �N���X (ThreadQueue�̍��@�\��)
//	[Ver]
//		Ver.01    2007/03/08  vs2005 �Ή�
//
//	[����]
//		�X���b�h�L���[�� �}���@�\���
// *********************************************************************************


#pragma once

#include <windows.h>

namespace KizuLib
{
	template <class data_t>	class ListArray : public ThreadQueue<data_t>
	{	
	public:
		ListArray(int SemCnt = 256);										// �R���X�g���N�^
		virtual ~ListArray();												// �f�X�g���N�^ (�v�f�͉������Ȃ�)

		
		//// �}���ɕK�v�ȋ@�\���
		// �v�f�ɃA�N�Z�X���Ă���Œ��ɁA�ʃX���b�h��Que�̃A�N�Z�X������̂��֎~�����
		void LockQue()			{ EnterCriticalSection(&my_cs); }			// �N���e�B�J���Z�N�V����
		void UnlockQue()		{ LeaveCriticalSection(&my_cs); }			// 

		data_t * Array(int index);											// �v�f�ւ̃A�N�Z�X

		BOOL AddToInsert(data_t * newData, int index);									// �v�f��}������
		BOOL AddToInsert(data_t * newData, int index, int retryCnt, int waitTime=1);	// �v�f��}������ (�ǉ������܂ő҂�)

	protected:

	};

	// //////////////////////////////////////////////////////////////////////////////
	// �e���v���[�g�Ȃ̂Ńw�b�_�t�@�C���ɓ���Ă����K�v����
	// //////////////////////////////////////////////////////////////////////////////

	//------------------------------------------
	// �R���X�g���N�^
	// int SemCnt �Z�}�t�H�̃T�C�Y (�f�t�H���g256)
	//------------------------------------------
	template <class data_t>
	ListArray<data_t>::ListArray(int SemCnt) : ThreadQueue(SemCnt)
	{
	}

	//------------------------------------------
	// �f�X�g���N�^
	//------------------------------------------
	template <class data_t>
	ListArray<data_t>::~ListArray()
	{
	}

	//------------------------------------------
	// �v�f�ɃA�N�Z�X (�K��Lock���Ă���)
	// int index �擪����̔z��ʒu (0�I���W��)
	// �߂�l�F�v�f
	//------------------------------------------
	template <class data_t>
	data_t * ListArray<data_t>::Array(int index)
	{
		// �����`�F�b�N
		if( 0 > index )					return NULL;
		if( this->GetCount() <= index )	return NULL;
 
		// �v�f��Ԃ�
		return my_deque[index];
	}

	//------------------------------------------
	// �v�f��}������
	// data_t * newData �}������C���X�^���X
	// int index �擪����̑}������z��ʒu (0�I���W��) �� 1���w�肵����[1]�ɑ}���B[1]�ɓ����Ă����̂�[2]�Ɉړ�
	// �߂�l�F���A���
	//------------------------------------------
	template <class data_t>
	BOOL ListArray<data_t>::AddToInsert(data_t * newData, int index)
	{
		// �����`�F�b�N
		if( 0 > index )					return FALSE;
		if( this->GetCount() < index )	return FALSE;

		// �}������C���e�[�^�����o��
		std::deque<data_t*>::iterator it = my_deque.begin(); 
		it += index;

		// �v�f��}��
		BOOL ret;

		EnterCriticalSection(&my_cs);
		my_deque.insert(it, newData);
		if (GetCount() > my_nMaxCount) my_nMaxCount = GetCount();
		ret = ReleaseSemaphore(g_evSem, 1, NULL);
		LeaveCriticalSection(&my_cs);
		ASSERT(ret);
		return ret;
	}

	//------------------------------------------
	// �v�f��}������ (�ǉ������܂ő҂�)
	// data_t * p �}������C���X�^���X
	// int index �擪����̑}������z��ʒu (0�I���W��) �� 1���w�肵����[1]�ɑ}���B[1]�ɓ����Ă����̂�[2]�Ɉړ�
	// int retryCnt ���g���C��
	// int waitTime Sleep�Ԋu [ms]
	// �߂�l ���A���
	//------------------------------------------
	template <class data_t>
	BOOL ListArray<data_t>::AddToInsert(data_t * newData, int index, int retryCnt, int waitTime)
	{
		BOOL ret;
		
		EnterCriticalSection(&my_csSeq);

		for ( int ii=0; ii<retryCnt; ii++ ) {
			// �ǉ��\���`�F�b�N
			if (GetCount() < my_nSemCount ) {	// �ǉ��\
				ret = AddToInsert(newData, index);
				LeaveCriticalSection(&my_csSeq);	
				return ret;
			} else {						// �҂�
				Sleep(waitTime);
			}
		}
		LeaveCriticalSection(&my_csSeq);
		return FALSE;
	}
};