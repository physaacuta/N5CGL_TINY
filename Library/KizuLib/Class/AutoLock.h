// *********************************************************************************
//	CRITICAL_SECTION �̃��b�N�E�A�����b�N �N���X
//	[Ver]
//		Ver.01    2007/01/30  vs2005 �Ή�
//		Ver.02	  2013/12/26  �����L�����Z���@�\�ǉ�
//
//	[����]
//
// *********************************************************************************

#ifndef KIZULIB_AUTOLOCK_H
#define KIZULIB_AUTOLOCK_H

#include <Windows.h>

namespace KizuLib
{
	////////////////////////////////////////////////////////////////////////
	// �C�Ӕr������
	//------------
	// �g����
	//  AutoLock autolock(&m_csLock);	// �����ꂾ����Ok
	class AutoLock  
	{
	// Ver.01
	//public:
	//	AutoLock(CRITICAL_SECTION * pCS)									// �R���X�g���N�^
	//	{
	//		my_pCS = pCS;
	//		EnterCriticalSection(my_pCS);
	//	}
	//		
	//	virtual ~AutoLock() {												// �f�X�g���N�^
	//		LeaveCriticalSection(my_pCS);
	//	}

	//protected:
	//	CRITICAL_SECTION*		my_pCS;										// �N���e�B�J���Z�N�V����

	// Ver.02
	public:
		AutoLock(CRITICAL_SECTION * pCS)									// �R���X�g���N�^
		{
			my_pCS = pCS;
			EnterCriticalSection(my_pCS);
		}
			
		void UnLock() {														// �����L�����Z��
			if( NULL == my_pCS ) return;
			LeaveCriticalSection(my_pCS);
			my_pCS = NULL;
		}

		virtual ~AutoLock() {												// �f�X�g���N�^
			//if( NULL == my_pCS ) return;
			//LeaveCriticalSection(my_pCS);
			//my_pCS = NULL;
			UnLock();
		}

	protected:
		CRITICAL_SECTION*		my_pCS;										// �N���e�B�J���Z�N�V����

	};


	////////////////////////////////////////////////////////////////////////
	// �v���Z�X�� �r������
/**
	//--------------------------------------------
	// �v���Z�X���B��̔r��
	// singleton
	class LockOnlyCs
	{
	//// �d�v�Bexe�ŗB��̃C���X�^���X��N���X��
	private:
		static LockOnlyCs				m_inst;											// exe�ŗB��̃C���X�^���X�B ����������new����āA�I������delete�����.
		mutable CRITICAL_SECTION		m_csLock;									// ���N���X�̑��C���X�^���X�Ԃ̔r������

	//// ���J�͂��ׂ�static
	public:
		static LockOnlyCs* GetIns() { 
			static LockOnlyCs ins; 
			return &ins;
		}
		static inline CRITICAL_SECTION* GetCS() { 
			return &GetIns()->m_csLock;
		}

	//// ����
	private:

		//// �R���X�g���N�^(singleton)
		LockOnlyCs(void)																	// private�R���X�g���N�^
		{
			InitializeCriticalSection(&m_csLock);
		}

		// �R�s�[�R���X�g���N�^ �֎~
		LockOnlyCs(const LockOnlyCs &ob) {}
		// ����֎~
		LockOnlyCs& operator=(const LockOnlyCs& ob) {}

	public:
		virtual ~LockOnlyCs(void)
		{ 
			DeleteCriticalSection(&m_csLock); 
		}
	};

**/

	//--------------------------------------------
	// �v���Z�X���B��
	//------------
	// �g����
	//  �E���C�u�������Ŏg���̐�΂ɋ֎~
	//  �EAutoLockOnly	autolock;	// �����ꂾ����Ok
	class AutoLockOnly
	{

		//--------------------------------------------
		// �v���Z�X���B��̔r�� (����q�N���X)
		// singleton
		class LockOnlyCs
		{
		//// �d�v�Bexe�ŗB��̃C���X�^���X��N���X��
		private:
			static LockOnlyCs				m_inst;											// exe�ŗB��̃C���X�^���X�B ����������new����āA�I������delete�����.
			mutable CRITICAL_SECTION		m_csLock;									// ���N���X�̑��C���X�^���X�Ԃ̔r������

		//// ���J�͂��ׂ�static
		public:
			static LockOnlyCs* GetIns() { 
				static LockOnlyCs ins; 
				return &ins;
			}
			static inline CRITICAL_SECTION* GetCS() { 
				return &GetIns()->m_csLock;
			}

		//// ����
		private:

			//// �R���X�g���N�^(singleton)
			LockOnlyCs(void)																	// private�R���X�g���N�^
			{
				InitializeCriticalSection(&m_csLock);
			}

			// �R�s�[�R���X�g���N�^ �֎~
			LockOnlyCs(const LockOnlyCs &ob) {}
			// ����֎~
			LockOnlyCs& operator=(const LockOnlyCs& ob) {}

		public:
			virtual ~LockOnlyCs(void)
			{ 
				DeleteCriticalSection(&m_csLock); 
			}
		};


		
	//--------------------------------------------
	// ����
	public:
		AutoLockOnly()										// �R���X�g���N�^
		{
			//CRITICAL_SECTION* cs = OnlyCs::GetCS();
			EnterCriticalSection(LockOnlyCs::GetCS());
		}
			
		void UnLock() {										// �����L�����Z��
			LeaveCriticalSection(LockOnlyCs::GetCS());
		}

		virtual ~AutoLockOnly() {							// �f�X�g���N�^
			UnLock();
		}
	};

};

#endif