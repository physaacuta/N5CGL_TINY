#include "StdAfx.h"
#include "OdbcEnv.h"

#include <crtdbg.h> // �f�o�b�N�p

using namespace KizuODBC;

//////////////////////////////////////////////////////////////////////
// OdbcEnv �̃X�^�e�B�b�N�ϐ�������
//////////////////////////////////////////////////////////////////////
OdbcEnv			OdbcEnv::m_inst;					// �X�^�e�B�b�N�ϐ�������


//////////////////////////////////////////////////////////////////////
// OdbcEnv
//////////////////////////////////////////////////////////////////////
//------------------------------------------
// �R���X�g���N�^
//------------------------------------------
OdbcEnv::OdbcEnv(void) :
m_henv(NULL)
{
	// ������
	memset(g_cLastErrMsgMain, 0x00, sizeof(g_cLastErrMsgMain));
	memset(g_cLastErrMsgSub, 0x00, sizeof(g_cLastErrMsgSub));
	g_nLastErrMsgNo = 0;

	// �N���e�B�J���Z�N�V���� ������
	InitializeCriticalSection(&m_cs);
	
	// ���n���h�� ����
    SQLRETURN sqlrc = SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &m_henv);
    if( SQL_SUCCESS != sqlrc )
    {
		_RPT0(_CRT_WARN, "SQL_HANDLE_ENV �n���h�������G���[\n");
        m_henv = NULL;
		return;
    }

	// �������̐ݒ� (ODBC�h���C�o�o�[�W�����w�� 3.x)
	sqlrc = SQLSetEnvAttr(m_henv, SQL_ATTR_ODBC_VERSION, (SQLPOINTER)SQL_OV_ODBC3, SQL_IS_INTEGER);
	if (sqlrc != SQL_SUCCESS && sqlrc != SQL_SUCCESS_WITH_INFO) { 
		GetErrMsg();
		SQLFreeHandle(SQL_HANDLE_ENV, m_henv);
		m_henv = NULL;
		return;
	}
}

//------------------------------------------
// �f�X�g���N�^
//------------------------------------------
OdbcEnv::~OdbcEnv(void)
{
	// ���
	if( NULL != m_henv ) {
		SQLFreeHandle(SQL_HANDLE_ENV, m_henv);
		m_henv = NULL;
	}
	DeleteCriticalSection(&m_cs);
}


//------------------------------------------
// �ُ픭�����̏ڍ׃��b�Z�[�W�擾
//------------------------------------------
void OdbcEnv::GetErrMsg()
{
	memset(g_cLastErrMsgMain, 0x00, sizeof(g_cLastErrMsgMain));
	memset(g_cLastErrMsgSub,  0x00, sizeof(g_cLastErrMsgSub));
	g_nLastErrMsgNo = 0;
	SQLSMALLINT retc;

	SQLGetDiagRec(SQL_HANDLE_ENV, m_henv, 1, g_cLastErrMsgSub, &g_nLastErrMsgNo, g_cLastErrMsgMain, (SQLSMALLINT)sizeof(g_cLastErrMsgMain), &retc );
	
	_RPT0(_CRT_WARN, g_cLastErrMsgSub);
	_RPT0(_CRT_WARN, g_cLastErrMsgMain);
	_RPT0(_CRT_WARN, "\n");
}



//////////////////////////////////////////////////////////////////////
// OdbcLockedEnv
//////////////////////////////////////////////////////////////////////
//------------------------------------------
// �R���X�g���N�^��lock
// BOOL bWait true:�N���e�B�J���Z�N�V�����҂�����
//			  false:�N���e�B�J���Z�N�V�����҂������B(�r���ł͂قڎg�p���Ȃ�)
//------------------------------------------
//OdbcLockedEnv::OdbcLockedEnv(BOOL bWait)
OdbcLockedEnv::OdbcLockedEnv()
{
//	if(bWait) {
//		// ���b�N (�N���e�B�J���Z�N�V�����˓�)
//		OdbcEnv::Lock();
//		m_bCsLocked = TRUE;
//
//	} else {
//		// NG���́A�����߂�
//		m_bCsLocked = OdbcEnv::TryLock();
//	}

	// ���b�N (�N���e�B�J���Z�N�V�����˓�)
	OdbcEnv::Lock();
	m_bCsLocked = TRUE;
}

//------------------------------------------
// �f�X�g���N�^��unlock
//------------------------------------------
OdbcLockedEnv::~OdbcLockedEnv()
{
	if( m_bCsLocked ) OdbcEnv::Unlock();
}


