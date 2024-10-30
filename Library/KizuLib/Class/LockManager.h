// *********************************************************************************
//	�X���b�h�ԓ�������N���X
//	[Ver]
//		Ver.01    2007/10/11  vs2005 �Ή�
//
//	[����]
//
// *********************************************************************************

#ifndef KIZULIB_LOCKMANAGER_H
#define KIZULIB_LOCKMANAGER_H

#include <Windows.h>

namespace KizuLib
{
	class LockManager
	{
	public:
		// //////////////////////////////////////////////////////////////////////////////
		// �������Ⴂ����w�b�_�[�ɑS������Ă���
		// //////////////////////////////////////////////////////////////////////////////
		// �R���X�g���N�^
		LockManager()														
		{
			// ������
			InitializeCriticalSection(&my_CsLock);								// �N���e�B�J���Z�N�V�����I�u�W�F�N�g��������
			my_EvLock  = CreateEvent(NULL, TRUE, TRUE, NULL);					// �}�j���A�����Z�b�g�A�ʏ킸���ƃV�O�i�����
		}

		// �f�X�g���N�^
		virtual ~LockManager() {											
			// �J��
			DeleteCriticalSection(&my_CsLock);									// �N���e�B�J���Z�N�V�����I�u�W�F�N�g���J��
			CloseHandle(my_EvLock);
		}

		// ���b�N
		void LockTh() {
			EnterCriticalSection(&my_CsLock);
			ResetEvent(my_EvLock); 
		}
		// �A�����b�N
		void UnlockTh() {
			LeaveCriticalSection(&my_CsLock);
			SetEvent(my_EvLock); 
		}
		// �u���b�N
		void BlockTh() {
			WaitForSingleObject(my_EvLock, INFINITE);
		}
		
		//// �O���A�N�Z�X
		HANDLE	GetEvLock()   const {return my_EvLock; };	

	protected:
		CRITICAL_SECTION		my_CsLock;									// �N���e�B�J���Z�N�V����
		HANDLE					my_EvLock;									// ���b�N�V�O�i�� (���b�N���Ă��鎞�̂݁A��V�O�i���ɂ���)
	};
};

#endif