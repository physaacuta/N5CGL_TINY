// *********************************************************************************
//	ODBC�ڑ����N���X
//	[Ver]
//		Ver.01    2007/01/30  vs2005 �Ή�
//
//	[����]
//		�g�p����^�X�N�� �wodbc32.lib�x �������N����K�v�������܂����B
//	[�ǉ����C�u�����t�@�C��]
//		Odbc32.lib
// *********************************************************************************
#pragma once

#include <windows.h>
#include <sql.h>
#include <sqlext.h>

#include "OdbcEnv.h"

#pragma warning(disable : 4996)							// ***���Â��`���Ƃ��Đ錾����܂����B���o���Ȃ��΍�

namespace KizuODBC
{
	class OdbcManager
	{
	public:
		// Fetch�G���[�R�[�h
		#define KIZUODBC_FETCH_ERR							-1					// �ُ�
		#define KIZUODBC_FETCH_OK							0					// ����
		#define KIZUODBC_FETCH_NODATE						1					// �f�[�^����

	protected:
		#define KIZUODBC_ODBCMANAGER_SESSION				"DB"				// ini�t�@�C���Z�b�V����

	public:
		OdbcManager(void);
		virtual ~OdbcManager(void);

		// ���
		bool IsConnect()	  const {return m_bIsConnect;}						// �ڑ����
		bool IsCreateHandle() const {return m_bIsCreateHandle;}					// �n���h���������


		// �ڑ�
		int Connect(LPCSTR cIniPath);											// �f�t�H���g�Z�b�V�����ł̐ڑ�
		int Connect(LPCSTR cIniPath, LPCSTR cSession);							// �Z�b�V�����w��ł̐ڑ�
		int Connect(char const* dsn, char const* uid, char const* pwd);			// �C�ӎw��ł̐ڑ�
		int ConnectDriver(char const* addr, char const* dsn, char const* uid, char const* pwd, bool bNative=false);	// ODBC����`�Őڑ�
		void DisConnect();														// �ؒf
		SQLRETURN AutoCommitEnable(bool flg);									// �I�[�g�R�~�b�g ON/OFF


		// ���s
		SQLRETURN ExecuteDirect(LPCSTR cSql);									// SQL�����s
		SQLRETURN ExecuteDirectAsync(LPCSTR cSql, bool bFlg);					// �񓯊�SQL�����s
		SQLRETURN ExecuteDirectAsyncCancel();									// �񓯊�SQL�����s�̖����I�ȃL�����Z��

		void TranCommit()	{ SQLTransact(OdbcEnv::GetHandle(), m_hdbc, SQL_COMMIT);}			// �R�~�b�g
		void TranRollBack() { SQLTransact(OdbcEnv::GetHandle(), m_hdbc, SQL_ROLLBACK);}		// ���[���o�b�N
///		void TranCommit()	{ SQLEndTran(SQL_HANDLE_ENV, m_henv, SQL_COMMIT);}		// �R�~�b�g
///		void TranRollBack() { SQLEndTran(SQL_HANDLE_ENV, m_henv, SQL_ROLLBACK);}	// ���[���o�b�N
		//SQLRETURN Fetch()	{ return SQLFetch(m_hstmt); }						// �t�F�b�`
		SQLRETURN Fetch();														// �t�F�b�`

		// �o�C���h���s
		SQLRETURN Prepare(LPCSTR cSql);											// �p�����[�^�t��SQL���Z�b�g
		SQLRETURN BindParameter(SQLSMALLINT ipar, SQLUINTEGER cbColDef, SQLLEN* rgbValue);	// �o�C���h
		SQLRETURN Execute() { return SQLExecute(m_hstmt); }						// �p�����[�^�t��SQL�����s
		SQLRETURN ParamData(SQLPOINTER* pVal);									// �p�����[�^�ԍ��擾
		SQLRETURN PutData(SQLPOINTER pVal, SQLINTEGER size);					// �p�����[�^�f�[�^��DB�֑��M

		// �f�[�^�擾
		long GetData(WORD n, LPVOID pVal, DWORD size );							// ���� �f�[�^�擾
		long GetData(WORD n, short *pVal );										// ���� �f�[�^�擾
		long GetData(WORD n, int *pVal );										// ���� �f�[�^�擾
		long GetData(WORD n, long *pVal );										// ���� �f�[�^�擾
		long GetData(WORD n, float *pVal );										// ���� �f�[�^�擾
		long GetData(WORD n, double *pVal );									// ���� �f�[�^�擾

		long GetDataDate(WORD n, int *pVal);									// ���t (�N����) �f�[�^�擾
		long GetDataDateTime(WORD n, int *pVal);								// ���t (�N���������b) �f�[�^�擾
		
		long GetDataImage(WORD n, LPVOID pVal, DWORD size );					// �o�C�i�� �f�[�^�擾

		// ���̑����擾
		void GetNumCols(SQLSMALLINT *clos) {SQLNumResultCols(m_hstmt, clos); }			// �񐔎擾
		void GetNumCols(WORD *clos) {SQLNumResultCols(m_hstmt, (SQLSMALLINT*)clos); }	// �񐔎擾 (�݊��ׁ̈A�I�[�o�[���[�h���c���Ă���)
		void GetNumRows(SQLLEN *rows) {SQLRowCount(m_hstmt, rows);}						// �s���擾 (DELETE�̏ꍇ��)
		void GetNumRows(DWORD *rows) {SQLLEN wk; GetNumRows(&wk); *rows=(DWORD)wk; }	// �s���擾 (�݊��ׁ̈A�I�[�o�[���[�h���c���Ă���)
		

		// �f�o�b�N�����p
		void GetErrMsg();				
		void GetErrMsg(SQLSMALLINT iType);										// �ُ픭�����̏ڍ׃��b�Z�[�W�擾
		SQLCHAR					g_cLastErrMsgMain[256];
		SQLCHAR					g_cLastErrMsgSub[256];
		SQLINTEGER				g_nLastErrMsgNo;
		

	protected:
		void Alloc();															// �ڑ��n���h������
		void Free();															// �ڑ��n���h�����

		SQLHDBC					m_hdbc;											// �ڑ��n���h��
		SQLHSTMT				m_hstmt;										// �X�e�[�g�����g�n���h��
		SQLCHAR					m_dsn[128];										// �f�[�^�x�[�X��

		bool					m_bIsConnect;									// DB�ɐڑ����H
		bool					m_bIsCreateHandle;								// ���E�ڑ��n���h���̐��������튮���H
		int						m_nLastErrNo;									// Odbc�G���[No
		int						m_nColumnIndexCheck;							// �J�����A�N�Z�X(Set:GetData Reset:ExecuteDirect or Fetch) (GetData �ŃJ�������e���R�Ŏ擾����� ODBC�̎d�l�㐳��Ɏ�荞�߂Ȃ����߃`�F�b�N)

	};
};