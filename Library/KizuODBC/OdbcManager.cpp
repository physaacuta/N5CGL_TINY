#include "StdAfx.h"
#include "OdbcManager.h"

#include <stdio.h>
#include <crtdbg.h> // �f�o�b�N�p

using namespace KizuODBC;

//////////////////////////////////////////////////////////////////////
// �\�z/����
//////////////////////////////////////////////////////////////////////
//------------------------------------------
// �R���X�g���N�^
//------------------------------------------
OdbcManager::OdbcManager(void) :
m_bIsConnect(false),
m_bIsCreateHandle(false),
m_nLastErrNo(0),
m_nColumnIndexCheck(0),
m_hdbc(NULL),
m_hstmt(NULL)
{
	// ������
	memset(g_cLastErrMsgMain, 0x00, sizeof(g_cLastErrMsgMain));
	memset(g_cLastErrMsgSub, 0x00, sizeof(g_cLastErrMsgSub));
	g_nLastErrMsgNo = 0;

	// �ڑ��n���h������
	Alloc();
}

//------------------------------------------
// �f�X�g���N�^
//------------------------------------------
OdbcManager::~OdbcManager(void)
{
	// �ؒf
	DisConnect();
	// ���
	Free();
}

//------------------------------------------
// �ڑ��n���h������
//	���n���h���́Astatic�Ő�������Ă���
//------------------------------------------
void OdbcManager::Alloc()
{
	SQLRETURN sqlrc;

	// ������
	m_bIsConnect = false;
	m_bIsCreateHandle = false;
	m_nLastErrNo = 0;


	// ���n���h��������ɑ��݁H
	OdbcLockedEnv env;						// �N���e�B�J���Z�N�V���� (ODBC�͂ǂ����}���`�X���b�h�Ńn���h���𐶐�������n���h�����[�N���Ă���)
	if( NULL == env.GetHandle() ){ //OdbcEnv::GetHandle() ) {
		m_nLastErrNo = -1;
		return;
	}

	// �ڑ��n���h���̎擾
	sqlrc = SQLAllocHandle(SQL_HANDLE_DBC, env.GetHandle(), &m_hdbc);
	if (sqlrc == SQL_SUCCESS_WITH_INFO) {m_nLastErrNo = -103;	GetErrMsg(SQL_HANDLE_DBC); goto NG;}
	if (sqlrc != SQL_SUCCESS)			{m_nLastErrNo = -3;		GetErrMsg(SQL_HANDLE_DBC); goto NG;}

	// �R�l�N�g�ڑ��^�C���A�E�g�ݒ� (���O�C���^�C���A�E�g���� 5�b) (=�ǂ����native client�̎��ɂ͖��������炵��)
	sqlrc = SQLSetConnectAttr(m_hdbc, SQL_ATTR_LOGIN_TIMEOUT, (SQLPOINTER)5, SQL_IS_INTEGER);
///	if (sqlrc == SQL_SUCCESS_WITH_INFO) {m_nLastErrNo = -106;	GetErrMsg(SQL_HANDLE_DBC); goto NG;}	// �G���[�`�F�b�N���� (�G���[�ɂȂ��Ă�����Ȃ�����)(Access�ɐڑ�����ꍇ�ȂǂŃG���[�ɂȂ��)
///	if (sqlrc != SQL_SUCCESS)			{m_nLastErrNo = -6;		GetErrMsg(SQL_HANDLE_DBC); goto NG;}	


	// �����܂ŗ�����n���h������
	m_bIsCreateHandle = true;
	return;
NG:
	// ���
	Free();
}

//------------------------------------------
// �ڑ��n���h�����
//------------------------------------------
void OdbcManager::Free()
{
	OdbcLockedEnv env;						// �N���e�B�J���Z�N�V���� (ODBC�͂ǂ����}���`�X���b�h�Ńn���h���𐶐�������n���h�����[�N���Ă���)

	// �n���h�����
	m_bIsCreateHandle = false;

	if(m_hdbc) {
		SQLFreeHandle(SQL_HANDLE_DBC, m_hdbc);
		m_hdbc = NULL;
	}
}

//------------------------------------------
// �f�t�H���g�Z�b�V�����ł̐ڑ�
// LPCSTR cIniPath ini�t�@�C���p�X
// �߂�l -:�ُ� 1:�ڑ��ς� 0:����ڑ�
//------------------------------------------
int OdbcManager::Connect(LPCSTR cIniPath)
{
	return Connect(cIniPath, KIZUODBC_ODBCMANAGER_SESSION);
}

//------------------------------------------
// �Z�b�V�����w��ł̐ڑ�
// LPCSTR cIniPath ini�t�@�C���p�X
// LPCSTR cSession �Z�b�V����
// �߂�l -:�ُ� 1:�ڑ��ς� 0:����ڑ�
//------------------------------------------
int OdbcManager::Connect(LPCSTR cIniPath, LPCSTR cSession)
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
// �C�ӎw��ł̐ڑ�
// char const* dsn  �f�[�^�x�[�X��
// char const* uid  ���[�U�[��
// char const* pwd  �p�X���[�h
// �߂�l -:�ُ� 1:�ڑ��ς� 0:����ڑ�
//------------------------------------------
int OdbcManager::Connect(char const* dsn, char const* uid, char const* pwd)
{
	SQLRETURN sqlrc;

	// ���ɐڑ��ς݁H
	if( m_bIsConnect ) return 1;

	OdbcLockedEnv env;						// �N���e�B�J���Z�N�V���� (ODBC�͂ǂ����}���`�X���b�h�Ńn���h���𐶐�������n���h�����[�N���Ă���)

//*/
	// �e�X�g�^�C���A�E�g�`�F�b�N
	//sqlrc = SQLSetConnectAttr(m_hdbc, SQL_ATTR_LOGIN_TIMEOUT, (SQLPOINTER)10, SQL_IS_INTEGER);
	//sqlrc = SQLSetConnectAttr(m_hdbc, SQL_ATTR_CONNECTION_TIMEOUT, (SQLPOINTER)1, SQL_IS_INTEGER);
//*/

	// �ڑ�
	sqlrc = SQLConnect(m_hdbc, (SQLCHAR*)dsn, SQL_NTS, (SQLCHAR*)uid, SQL_NTS, (SQLCHAR*)pwd, SQL_NTS);
///	if (sqlrc == SQL_SUCCESS_WITH_INFO) {retc = -104; goto NG;}
	if (sqlrc != SQL_SUCCESS && sqlrc != SQL_SUCCESS_WITH_INFO)	{m_nLastErrNo = -4;	GetErrMsg(SQL_HANDLE_DBC); goto NG;}


	// �I�v�V�����ݒ�
	sqlrc = AutoCommitEnable(true);												// �I�[�g�R�~�b�g�֎~�ɂ�����A�o�N�����������Ȃ̂ŁA�I�[�g�R�~�b�g���[�h�ɂ��Ă�����
	///if (sqlrc != SQL_SUCCESS && sqlrc != SQL_SUCCESS_WITH_INFO) {retc = -5; goto NG;}
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

//------------------------------------------
// ODBC�𖢒�`�Őڑ�
// char const* addr �f�[�^�x�[�X�A�h���X
// char const* dsn  �f�[�^�x�[�X��
// char const* uid  ���[�U�[��
// char const* pwd  �p�X���[�h
// bool bNative SQL Native Client �Őڑ��� true�B�ʏ��false
// �߂�l -:�ُ� 1:�ڑ��ς� 0:����ڑ�
//------------------------------------------
int OdbcManager::ConnectDriver(char const* addr, char const* dsn, char const* uid, char const* pwd, bool bNative)
{
	SQLRETURN sqlrc;

	// ���ɐڑ��ς݁H
	if( m_bIsConnect ) return 1;

	OdbcLockedEnv env;						// �N���e�B�J���Z�N�V���� (ODBC�͂ǂ����}���`�X���b�h�Ńn���h���𐶐�������n���h�����[�N���Ă���)

	// �ڑ������񐶐�
	SQLCHAR     szOutConn[255];
	SQLSMALLINT cbOutConn;
	char cConnect[1024];
	//if(bNative) sprintf(cConnect, "DRIVER={SQL Native Client};SERVER=%s;DSN=%s;UID=%s;PWD=%s;", addr, dsn, uid, pwd);
	//else		sprintf(cConnect, "DRIVER={SQL Server};SERVER=%s;DSN=%s;UID=%s;PWD=%s;", addr, dsn, uid, pwd);
	if(bNative) sprintf(cConnect, "DRIVER={SQL Native Client};SERVER=%s;DATABASE=%s;UID=%s;PWD=%s;", addr, dsn, uid, pwd);	// SQLServer2005�p
	else		sprintf(cConnect, "DRIVER={SQL Server};SERVER=%s;DATABASE=%s;UID=%s;PWD=%s;", addr, dsn, uid, pwd);

	// �ڑ�
	sqlrc = SQLDriverConnect(m_hdbc, NULL, (SQLTCHAR*)cConnect , SQL_NTS, szOutConn, sizeof(szOutConn), &cbOutConn, SQL_DRIVER_NOPROMPT);
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

//------------------------------------------
// �ؒf
//------------------------------------------
void OdbcManager::DisConnect()
{
	OdbcLockedEnv env;						// �N���e�B�J���Z�N�V���� (ODBC�͂ǂ����}���`�X���b�h�Ńn���h���𐶐�������n���h�����[�N���Ă���)

	// �ؒf
	if(m_hstmt)	{
		SQLFreeHandle(SQL_HANDLE_STMT, m_hstmt);
		m_hstmt = NULL;
	}
	if(m_hdbc) {
		SQLDisconnect(m_hdbc);
		m_bIsConnect = false;
	}
}

//------------------------------------------
// �I�[�g�R�~�b�g ON/OFF
// bool flg �g�����U�N�V�����Ǝg�������ꍇ�́Afalse�ɂ��鎖
//------------------------------------------
SQLRETURN OdbcManager::AutoCommitEnable(bool flg)
{
	SQLRETURN retc;
	if(flg) {
///		retc = SQLSetConnectOption(m_hdbc, SQL_AUTOCOMMIT, SQL_AUTOCOMMIT_ON);
		retc = SQLSetConnectAttr(m_hdbc, SQL_ATTR_AUTOCOMMIT, (SQLPOINTER)SQL_AUTOCOMMIT_ON, SQL_IS_INTEGER);
		if( SQL_SUCCESS != retc ) GetErrMsg();
		return retc;
	} else {
///		retc = SQLSetConnectOption(m_hdbc, SQL_AUTOCOMMIT, SQL_AUTOCOMMIT_OFF);	
		retc = SQLSetConnectAttr(m_hdbc, SQL_ATTR_AUTOCOMMIT, (SQLPOINTER)SQL_AUTOCOMMIT_OFF, SQL_IS_INTEGER);
		if( SQL_SUCCESS != retc ) GetErrMsg();
		return retc;
	}
}

//------------------------------------------
// SQL�����s
// LPCSTR cSql SQL��
// �߂�l SQLRETURN (SQL_ERROR,SQL_INVALID_HANDLE)�̏ꍇ�ُ͈�?
//------------------------------------------
SQLRETURN OdbcManager::ExecuteDirect(LPCSTR cSql)
{
	if(m_hstmt) { SQLFreeHandle(SQL_HANDLE_STMT, m_hstmt); m_hstmt = NULL; }

	SQLRETURN sqlrc = SQLAllocHandle(SQL_HANDLE_STMT, m_hdbc, &m_hstmt);
	if (sqlrc != SQL_SUCCESS) GetErrMsg(SQL_HANDLE_STMT);
	m_nColumnIndexCheck = 0;		// GetData��index���N���A	

	return SQLExecDirect(m_hstmt, (SQLCHAR *)cSql, SQL_NTS);
}

//------------------------------------------
// �񓯊�SQL�����s
// LPCSTR cSql SQL��
// bool bFlg	true ����		false�Q��ڈȍ~
// �߂�l SQLRETURN (SQL_ERROR,SQL_INVALID_HANDLE)�̏ꍇ�ُ͈�?
//------------------------------------------
SQLRETURN OdbcManager::ExecuteDirectAsync(LPCSTR cSql, bool bFlg)
{
	if(bFlg) {

		if(m_hstmt) { SQLFreeHandle(SQL_HANDLE_STMT, m_hstmt); m_hstmt = NULL; }

		SQLRETURN sqlrc = SQLAllocHandle(SQL_HANDLE_STMT, m_hdbc, &m_hstmt);
		if (sqlrc != SQL_SUCCESS) GetErrMsg(SQL_HANDLE_STMT);
		m_nColumnIndexCheck = 0;		// GetData��index���N���A	

		// �񓯊��L��
		sqlrc = SQLSetStmtAttr( m_hstmt, SQL_ATTR_ASYNC_ENABLE, (SQLPOINTER) SQL_ASYNC_ENABLE_ON, 0);
		if (sqlrc != SQL_SUCCESS) GetErrMsg(SQL_HANDLE_STMT);
	}

	return SQLExecDirect(m_hstmt, (SQLCHAR *)cSql, SQL_NTS);
}

//------------------------------------------
// �񓯊�SQL�����s�̖����I�ȃL�����Z��
// �߂�l SQLRETURN (SQL_ERROR,SQL_INVALID_HANDLE)�̏ꍇ�ُ͈�?
//------------------------------------------
SQLRETURN OdbcManager::ExecuteDirectAsyncCancel()
{
	return SQLCancel(m_hstmt);
}

//------------------------------------------
// �t�F�b�`
//------------------------------------------
SQLRETURN OdbcManager::Fetch()
{
	m_nColumnIndexCheck = 0;		// GetData��index���N���A	
	return SQLFetch(m_hstmt); 
}

//------------------------------------------
// �p�����[�^�t��SQL���Z�b�g
// LPCSTR cSql ? �t����SQL��
// �߂�l SQLRETURN (SQL_ERROR,SQL_INVALID_HANDLE)�̏ꍇ�ُ͈�?
//------------------------------------------
SQLRETURN OdbcManager::Prepare(LPCSTR cSql)
{
	if(m_hstmt) { SQLFreeHandle(SQL_HANDLE_STMT, m_hstmt); m_hstmt = NULL; }

	SQLAllocHandle(SQL_HANDLE_STMT, m_hdbc, &m_hstmt);

	return SQLPrepare(m_hstmt, (SQLCHAR *)cSql, SQL_NTS);
}

//------------------------------------------
// �o�C���h
// WORD ipar �o�C���hNo (1�I���W��)
// DWORD cbColDef �o�C���h�f�[�^�T�C�Y
// LPVOID rgbValue �f�[�^ (out)
// �߂�l SQLRETURN (SQL_ERROR,SQL_INVALID_HANDLE)�̏ꍇ�ُ͈�?
//------------------------------------------
//SQLRETURN OdbcManager::BindParameter(WORD ipar, DWORD cbColDef, LPVOID rgbValue)
SQLRETURN OdbcManager::BindParameter(SQLSMALLINT ipar, SQLUINTEGER cbColDef, SQLLEN* rgbValue)
{
	//return SQLBindParameter(m_hstmt, ipar, SQL_PARAM_INPUT, SQL_C_BINARY, SQL_LONGVARBINARY, cbColDef, 0, (PTR)ipar, 0, (long*)rgbValue);
	return SQLBindParameter(m_hstmt, ipar, SQL_PARAM_INPUT, SQL_C_BINARY, SQL_LONGVARBINARY, cbColDef, 0, (SQLPOINTER)ipar, 0, rgbValue);
}
//------------------------------------------
// �p�����[�^�ԍ��擾
// PTR* pVal �g�[�N��
// �߂�l SQLRETURN SQLNEEDDATA(�o�^�O), SQLSUCCESS(�o�^��) �� ����
//------------------------------------------
//SQLRETURN OdbcManager::ParamData(PTR* pVal)
SQLRETURN OdbcManager::ParamData(SQLPOINTER* pVal)
{
#ifndef true
	return SQLParamData(m_hstmt, pVal);
#else
	SQLRETURN a =SQLParamData(m_hstmt, pVal);
		UCHAR errmsg[256];
		SQLError(0, m_hdbc, m_hstmt, NULL, NULL, errmsg, sizeof(errmsg), NULL );
		_RPT0(_CRT_WARN, errmsg);
		_RPT0(_CRT_WARN, "\n");
	return a;
#endif
}
//------------------------------------------
// �p�����[�^�f�[�^��DB�֑��M
// LPVOID pVal �o�C�i���f�[�^�|�C���^
// DWORD size �f�[�^�T�C�Y
// �߂�l SQLRETURN (SQL_ERROR,SQL_INVALID_HANDLE)�̏ꍇ�ُ͈�?
//------------------------------------------
SQLRETURN OdbcManager::PutData(SQLPOINTER pVal, SQLINTEGER size)
{
#ifndef true
	return SQLPutData(m_hstmt, pVal, size);
#else
	SQLRETURN a =SQLPutData(m_hstmt, pVal, size);
	if(a == SQL_ERROR) {
		UCHAR errmsg[256];
		UCHAR errmsg1[256];
		SQLINTEGER    i;
		SQLError(0, m_hdbc, m_hstmt, errmsg1, &i, errmsg, sizeof(errmsg), NULL );
		_RPT0(_CRT_WARN, errmsg);
		_RPT0(_CRT_WARN, "\n");
	}
	return a;
#endif
}

//------------------------------------------
// �f�[�^�擾
// WORD n �J�����ʒu (1�I���W��)
// LPVOID pVal �o�b�t�@�|�C���^
// DWORD size �o�b�t�@�T�C�Y
// �߂�l �擾������
//------------------------------------------
// ����
long OdbcManager::GetData(WORD n, LPVOID pVal, DWORD size )
{
	_ASSERT( m_nColumnIndexCheck < n );
	m_nColumnIndexCheck = n;

	SQLLEN inum;
	SQLRETURN sqlrc = SQLGetData(m_hstmt, n, SQL_C_CHAR, pVal, size, &inum);
	if(sqlrc != SQL_SUCCESS) GetErrMsg(SQL_HANDLE_STMT);
	return (long)inum;
}
// ����
long OdbcManager::GetData(WORD n, short *pVal )
{
	_ASSERT( m_nColumnIndexCheck < n );
	m_nColumnIndexCheck = n;

	SQLLEN inum;
	SQLRETURN sqlrc = SQLGetData(m_hstmt, n, SQL_C_SHORT, pVal, 2, &inum);
	if(sqlrc != SQL_SUCCESS) GetErrMsg(SQL_HANDLE_STMT);
	return (long)inum;
}
long OdbcManager::GetData(WORD n, int *pVal )
{
	_ASSERT( m_nColumnIndexCheck < n );
	m_nColumnIndexCheck = n;

	SQLLEN inum;
	SQLRETURN sqlrc = SQLGetData(m_hstmt, n, SQL_C_LONG, pVal, 4, &inum);
	if(sqlrc != SQL_SUCCESS) GetErrMsg(SQL_HANDLE_STMT);
	return (long)inum;
}
long OdbcManager::GetData(WORD n, long *pVal )
{
	_ASSERT( m_nColumnIndexCheck < n );
	m_nColumnIndexCheck = n;

	SQLLEN inum;
	SQLRETURN sqlrc = SQLGetData(m_hstmt, n, SQL_C_LONG, pVal, 4, &inum);
	if(sqlrc != SQL_SUCCESS) GetErrMsg(SQL_HANDLE_STMT);
	return (long)inum;
}
// ����
long OdbcManager::GetData(WORD n, float *pVal)
{
	_ASSERT( m_nColumnIndexCheck < n );
	m_nColumnIndexCheck = n;

	SQLLEN inum;
	SQLRETURN sqlrc = SQLGetData(m_hstmt, n, SQL_C_FLOAT, pVal, 4, &inum);
	if(sqlrc != SQL_SUCCESS) GetErrMsg(SQL_HANDLE_STMT);
	return (long)inum;
}
long OdbcManager::GetData(WORD n, double *pVal)
{
	_ASSERT( m_nColumnIndexCheck < n );
	m_nColumnIndexCheck = n;

	SQLLEN inum;
	SQLRETURN sqlrc = SQLGetData(m_hstmt, n, SQL_C_DOUBLE, pVal, 8, &inum);
	if(sqlrc != SQL_SUCCESS) GetErrMsg(SQL_HANDLE_STMT);
	return (long)inum;
}
//------------------------------------------
// ���t�^
// WORD n �J�����ʒu (0�I���W��)
//------------------------------------------
// int[3] �N����
long OdbcManager::GetDataDate(WORD n, int *pVal)
{
	_ASSERT( m_nColumnIndexCheck < n );
	m_nColumnIndexCheck = n;

	SQLLEN inum;
	SQL_DATE_STRUCT  d;
	SQLRETURN sqlrc = SQLGetData(m_hstmt, n, SQL_C_DATE, &d, 0, &inum);
	if(sqlrc != SQL_SUCCESS) GetErrMsg(SQL_HANDLE_STMT);
	pVal[0] = d.year;
	pVal[1] = d.month;
	pVal[2] = d.day;
	return (long)inum;
}
// int[6] �N���������b
long OdbcManager::GetDataDateTime(WORD n, int *pVal)
{
	_ASSERT( m_nColumnIndexCheck < n );
	m_nColumnIndexCheck = n;

	SQLLEN inum;
	SQL_TIMESTAMP_STRUCT  d;
	SQLRETURN sqlrc = SQLGetData(m_hstmt, n, SQL_C_TIMESTAMP, &d, 0, &inum);
	if(sqlrc != SQL_SUCCESS) GetErrMsg(SQL_HANDLE_STMT);
	pVal[0] = d.year;
	pVal[1] = d.month;
	pVal[2] = d.day;
	pVal[3] = d.hour;
	pVal[4] = d.minute;
	pVal[5] = d.second;
	return (long)inum;
}

//------------------------------------------
// �o�C�i���f�[�^�擾
// WORD n �J�����ʒu (0�I���W��)
// LPVOID pVal �o�b�t�@�|�C���^
// DWORD size �o�b�t�@�T�C�Y
// �߂�l �擾�o�C�g��
//------------------------------------------
long OdbcManager::GetDataImage(WORD n, LPVOID pVal, DWORD size )
{
	_ASSERT( m_nColumnIndexCheck < n );
	m_nColumnIndexCheck = n;

	SQLLEN inum;
	SQLRETURN sqlrc = SQLGetData(m_hstmt, n, SQL_C_BINARY, pVal, size, &inum);
	if(sqlrc != SQL_SUCCESS) GetErrMsg(SQL_HANDLE_STMT);
	return (long)inum;
}


//------------------------------------------
// �ُ픭�����̏ڍ׃��b�Z�[�W�擾
//------------------------------------------
void OdbcManager::GetErrMsg()
{
	memset(g_cLastErrMsgMain, 0x00, sizeof(g_cLastErrMsgMain));
	memset(g_cLastErrMsgSub, 0x00, sizeof(g_cLastErrMsgSub));
	g_nLastErrMsgNo = 0;
	SQLSMALLINT retc;

	if (m_hstmt && m_hdbc) {
		// SQL2005�ɂȂ��Ă���p�ɂɐڑ��G���[�ɂȂ����ׂ̒���
		//SQLError(0, m_hdbc, m_hstmt, NULL, NULL, errmsg, sizeof(errmsg), NULL );
		SQLError(0, m_hdbc, m_hstmt, g_cLastErrMsgSub, &g_nLastErrMsgNo, g_cLastErrMsgMain, (SQLSMALLINT)sizeof(g_cLastErrMsgMain), &retc );

		_RPT0(_CRT_WARN, g_cLastErrMsgMain);
		_RPT0(_CRT_WARN, "\n");
	}
}

void OdbcManager::GetErrMsg(SQLSMALLINT iType)
{
	memset(g_cLastErrMsgMain, 0x00, sizeof(g_cLastErrMsgMain));
	memset(g_cLastErrMsgSub,  0x00, sizeof(g_cLastErrMsgSub));
	g_nLastErrMsgNo = 0;
	SQLSMALLINT retc;

	if( SQL_HANDLE_ENV == iType ) {
		SQLGetDiagRec(iType, OdbcEnv::GetHandle(), 1, g_cLastErrMsgSub, &g_nLastErrMsgNo, g_cLastErrMsgMain, (SQLSMALLINT)sizeof(g_cLastErrMsgMain), &retc );
	} else if ( SQL_HANDLE_DBC == iType ) {
		SQLGetDiagRec(iType, m_hdbc, 1, g_cLastErrMsgSub, &g_nLastErrMsgNo, g_cLastErrMsgMain, (SQLSMALLINT)sizeof(g_cLastErrMsgMain), &retc );
	} else {
		SQLGetDiagRec(iType, m_hstmt, 1, g_cLastErrMsgSub, &g_nLastErrMsgNo, g_cLastErrMsgMain, (SQLSMALLINT)sizeof(g_cLastErrMsgMain), &retc );
	}
	
	_RPT0(_CRT_WARN, g_cLastErrMsgSub);
	_RPT0(_CRT_WARN, g_cLastErrMsgMain);
	_RPT0(_CRT_WARN, "\n");

}