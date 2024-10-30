#include "StdAfx.h"
#include "BCPManager.h"

using namespace KizuODBC;

//////////////////////////////////////////////////////////////////////
// �\�z/����
//////////////////////////////////////////////////////////////////////
//------------------------------------------
// �R���X�g���N�^
//------------------------------------------
BCPManager::BCPManager(void) : OdbcManager() 
{
}
//------------------------------------------
// �f�X�g���N�^
//------------------------------------------
BCPManager::~BCPManager(void)
{
}
//------------------------------------------
// �f�t�H���g�Z�b�V�����ł̐ڑ�
// LPCSTR cIniPath ini�t�@�C���p�X
// �߂�l -:�ُ� 1:�ڑ��ς� 0:����ڑ�
//------------------------------------------
int BCPManager::Connect(LPCSTR cIniPath)
{
	return Connect(cIniPath, KIZUODBC_ODBCMANAGER_SESSION);
}
//------------------------------------------
// �Z�b�V�����w��ł̐ڑ�
// LPCSTR cIniPath ini�t�@�C���p�X
// LPCSTR cSession �Z�b�V����
// �߂�l -:�ُ� 1:�ڑ��ς� 0:����ڑ�
//------------------------------------------
int BCPManager::Connect(LPCSTR cIniPath, LPCSTR cSession)
{
	char dsn[128];			// �f�[�^�x�[�X��
	char uid[128];			// ���[�U�[��
	char pwd[128];			// �p�X���[�h

	// ini�t�@�C���擾
	GetPrivateProfileString(cSession, "ODBCNAME", "", dsn, sizeof(dsn), cIniPath);
	GetPrivateProfileString(cSession, "USERID",	  "", uid, sizeof(uid), cIniPath);
	GetPrivateProfileString(cSession, "PASSWORD", "", pwd, sizeof(pwd), cIniPath);

	// �C�ӎw��Őڑ�
	return Connect( dsn, uid , pwd);
}

//------------------------------------------
// �Z�b�V�����w��ł̐ڑ�
// LPCSTR cIniPath ini�t�@�C���p�X
// LPCSTR cSession �Z�b�V����
// �߂�l -:�ُ� 1:�ڑ��ς� 0:����ڑ�
//------------------------------------------
int BCPManager::Connect(char const* dsn, char const* uid, char const* pwd)
{
	SQLRETURN sqlrc;

	// ���ɐڑ��ς݁H
	if( m_bIsConnect ) return 1;

	OdbcLockedEnv env;						// �N���e�B�J���Z�N�V���� (ODBC�͂ǂ����}���`�X���b�h�Ńn���h���𐶐�������n���h�����[�N���Ă���)

	// BCP�ڑ��L���� ���ꂪBCP�n -------------------->>>>>>>>>>
	sqlrc = SQLSetConnectAttr(m_hdbc, SQL_COPT_SS_BCP, (SQLPOINTER)SQL_BCP_ON, SQL_IS_INTEGER);
	if (sqlrc == SQL_SUCCESS_WITH_INFO) {m_nLastErrNo = -106;	GetErrMsg(SQL_HANDLE_DBC);goto NG;}
	if (sqlrc != SQL_SUCCESS)			{m_nLastErrNo = -6;		GetErrMsg(SQL_HANDLE_DBC);goto NG;}
	// <<<<<< -------------------------------------------------
	
	// �ڑ�
	sqlrc = SQLConnect(m_hdbc, (SQLCHAR*)dsn, SQL_NTS, (SQLCHAR*)uid, SQL_NTS, (SQLCHAR*)pwd, SQL_NTS);
	if (sqlrc != SQL_SUCCESS && sqlrc != SQL_SUCCESS_WITH_INFO)	{m_nLastErrNo = -4;	GetErrMsg(SQL_HANDLE_DBC); goto NG;}


	// �I�v�V�����ݒ�
	sqlrc = AutoCommitEnable(true);												// �I�[�g�R�~�b�g�֎~�ɂ�����A�o�N�����������Ȃ̂ŁA�I�[�g�R�~�b�g���[�h�ɂ��Ă�����
	if (sqlrc == SQL_SUCCESS_WITH_INFO) {m_nLastErrNo = -105;	GetErrMsg(SQL_HANDLE_DBC);goto NG;}
	if (sqlrc != SQL_SUCCESS)			{m_nLastErrNo = -5;		GetErrMsg(SQL_HANDLE_DBC);goto NG;}


	// �G���h����
	m_bIsConnect = true;
	memcpy( m_dsn, dsn, sizeof(m_dsn));

	return 0;
NG:
	// �ؒf
	DisConnect();
	return m_nLastErrNo;
}