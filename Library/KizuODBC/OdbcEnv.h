// *********************************************************************************
//	ODBC�ڑ��p���n���h�������N���X
//	[Ver]
//		Ver.01    2008/01/13  vs2005 �Ή�
//
//	[����]
//		singleton�f�U�C���p�^�[���Ő݌v (= private�R���X�g���N�^+static�C���X�^���X)
//		exe�ŗB��̃C���X�^���X
//		static�R���X�g���N�^�Ő�������āAdelete�ŉ�������B
//	[�ǉ����C�u�����t�@�C��]
//		Odbc32.lib
// *********************************************************************************
#pragma once

#include <windows.h>
#include <sql.h>
#include <sqlext.h>

#pragma warning(disable : 4996)							// ***���Â��`���Ƃ��Đ錾����܂����B���o���Ȃ��΍�


namespace KizuODBC
{
	//// ���n���h��(singleton)
	class OdbcEnv
	{
	private:
		SQLHENV					m_henv;											// ���n���h��
		CRITICAL_SECTION		m_cs;											// �X���b�h�Ԃ̔r������

		static OdbcEnv			m_inst;											// exe�ŗB��̃C���X�^���X�B ����������new����āA�I������delete�����.

		OdbcEnv();																// private�R���X�g���N�^

	public:
		virtual ~OdbcEnv();														// �f�X�g���N�^

		// �C�����C��
		static inline void Lock()			{ EnterCriticalSection(&m_inst.m_cs); }
		static inline void Unlock()			{ LeaveCriticalSection(&m_inst.m_cs); }
//		static inline BOOL TryLock()		{ return TryEnterCriticalSection(&m_inst.m_cs); }	// �N���e�B�J���Z�N�V���� NG�̏ꍇ�͑������ʂ��Ԃ�

		static inline SQLHENV GetHandle()	{ return m_inst.m_henv; }


		// �f�o�b�N�����p
	public:	
		void GetErrMsg();														// �ُ픭�����̏ڍ׃��b�Z�[�W�擾
		SQLCHAR					g_cLastErrMsgMain[256];
		SQLCHAR					g_cLastErrMsgSub[256];
		SQLINTEGER				g_nLastErrMsgNo;
	};



	//// ���n���h������ & �r���t������N���X
	class OdbcLockedEnv
	{
	private:
		BOOL					m_bCsLocked;									// �N���e�B�J���Z�N�V�������b�N���
			
	public:
	    
		//OdbcLockedEnv(BOOL bWait = TRUE);										// �R���X�g���N�^��lock �r���ł͕K�v�Ȃ��ׁA����
		OdbcLockedEnv();														// �R���X�g���N�^��lock
		virtual ~OdbcLockedEnv();												// �f�X�g���N�^��unlock

		// �C�����C��
		inline BOOL IsCsLocked() const		{ return m_bCsLocked; }
		//inline operator SQLHENV() const		{ return OdbcEnv::GetHandle(); }	// �L���X�g���Z�q���I�[�o���[�h���Ă��̂܂܎g����悤�ɂ���
		inline SQLHENV GetHandle() const	{ return OdbcEnv::GetHandle(); }	// ���n���h���擾
	};
};


