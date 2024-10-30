// *********************************************************************************
//	ODBC �N���X
//	[Ver]
//		Ver.01    2007/01/30  vs2005 �Ή�
//
//	[����]
//		�g�p����^�X�N�� �wodbc32.lib�x �������N����K�v�������܂����B
//		OdbcManager ���g���₷���悤�ɔ�킹
//	[�ǉ����C�u�����t�@�C��]
//		Odbc32.lib
// *********************************************************************************

#pragma once

#include <stdio.h>
#include "OdbcManager.h"														// ODBC�ڑ����N���X

namespace KizuODBC
{
	class OdbcBase
	{
	private:
		static const __int64 BinalyFileOneWriteSize=2*1024*1024;				// �o�C�i���t�@�C��1��̍ő发�����݃T�C�Y


	public:

		OdbcBase(void);
		virtual ~OdbcBase(void);
		
	//------------------------------------------
	// OdbcManager �̔�킹�֘A
	//------------------------------------------
		static bool OpenCheck(LPCSTR cIniPath, LPCSTR cSession = NULL);			// �ڑ��`�F�b�N
		static bool OpenCheck(char const* dsn, char const* uid, char const* pwd); // �C�ӎw��Őڑ��`�F�b�N
		static bool OpenCheck(char const* addr, char const* dsn, char const* uid, char const* pwd, bool bNative=false); // ODBC�𖢒�`�Őڑ��`�F�b�N


		// �ڑ�
		bool Connect(LPCSTR cIniPath, LPCSTR cSession = NULL);					// �Z�b�V�����w��ł̐ڑ�
		bool Connect(char const* dsn, char const* uid, char const* pwd);		// �C�ӎw��ł̐ڑ�
		bool ConnectDriver(char const* addr, char const* dsn, char const* uid, char const* pwd, bool bNative=false); // ODBC�𖢒�`�Őڑ�


//		bool Connect();															// �f�t�H���g�Z�b�V�����Őڑ�
		void DisConnect();														// �ؒf
		bool IsConnect() const {return mycls_pDB->IsConnect();}					// �ڑ������m�F
		bool AutoCommitEnable(bool flg);										// �I�[�g�R�~�b�g ON/OFF

		// ���s
		bool ExecuteDirect(LPCSTR cSql, bool bErrAutoClose = true);				// SQL�����s
		bool TranCommit();														// �R�~�b�g
		bool TranRollBack();													// ���[���o�b�N
		int  Fetch();															// �t�F�b�`

		// �o�C�i���f�[�^�o�^�֘A
		bool SqlBinalyWrite(LPCSTR cSql, int paramCnt, DWORD *size, LPVOID *pVal, bool bErrAutoClose = true);				// �������o�C�i���o�^
		bool SqlBinalyWrite(LPCSTR cSql, DWORD size, LPVOID pVal, bool bErrAutoClose = true);								// 1���o�C�i���o�^ (�ȈՔ�)
		bool SqlBinalyWrite(LPCSTR cSql, DWORD size1, DWORD size2, LPVOID pVal1, LPVOID pVal2, bool bErrAutoClose = true);	// 2���o�C�i���o�^ (�ȈՔ�)

		// ������ȃo�C�i���t�@�C����o�^ (�擾�������ӂ��K�v)
		bool SqlBinalyFileWrite(LPCSTR cSql, int paramCnt, int pathsize, const char* cFPath, bool bErrAutoClose = true);	// �������o�C�i���o�^

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
		void GetNumCols(WORD *clos) {if( ! mycls_pDB->IsConnect()) return; mycls_pDB->GetNumCols(clos); }	// �񐔎擾
		void GetNumRows(DWORD *rows){if( ! mycls_pDB->IsConnect()) return; mycls_pDB->GetNumRows(rows); }	// �s���擾 (DELETE�̏ꍇ���ŁASQL�𔭍s������Ŏg�p)
	
		const SQLCHAR *GetErrorDetail() const  { return mycls_pDB->g_cLastErrMsgMain; }

	//------------------------------------------
	// SQL�� ��������֘A
	//------------------------------------------
		// insert
		static int sqlColumnInsert(LPSTR pSql, const char *val, int size);
		static int sqlColumnInsert(LPSTR pSql, short val);
		static int sqlColumnInsert(LPSTR pSql, int val);
		static int sqlColumnInsert(LPSTR pSql, long val);
		static int sqlColumnInsert(LPSTR pSql, DWORD val);
		static int sqlColumnInsert(LPSTR pSql, float val);
		static int sqlColumnInsert(LPSTR pSql, double val);

		// update
		static int sqlColumnUpdate(LPSTR pSql, const char *name, const char *val, int size);
		static int sqlColumnUpdate(LPSTR pSql, const char *name, short val);
		static int sqlColumnUpdate(LPSTR pSql, const char *name, int val);
		static int sqlColumnUpdate(LPSTR pSql, const char *name, long val);
		static int sqlColumnUpdate(LPSTR pSql, const char *name, DWORD val);
		static int sqlColumnUpdate(LPSTR pSql, const char *name, float val);
		static int sqlColumnUpdate(LPSTR pSql, const char *name, double val);
		
		// ����ȊO
		static int sqlColumnTrim(LPSTR pSql);									// sqlColumnUpdate�ōŌ�ɂ��� ","���������
		static int sqlColumnAddTime(LPSTR pSql);								// ������t�^


	//------------------------------------------
	// �W���I��Select���̎��s�擾
	//------------------------------------------
		// �P��s1���ڂ�int�f�[�^�̂ݎ擾 (������count(*)��SQL�����s�Ɏg�p)
		bool GetSelectKey(char const *cSql, int size, char *pVal);	
		bool GetSelectKey(char const *cSql, int *pVal);
		bool GetSelectKey(char const *cSql, float *pVal);
		bool GetSelectKey(char const *cSql, double *pVal);
		bool GetSelectDateTime(char const *cSql, int *pVal);
		bool GetSelectImage(char const *cSql, int size, char *pVal);
		static bool GetSelectKey_Static(char const *cSql, int size, char *pVal, LPCSTR cIniPath, LPCSTR cSession=NULL);		// �X�^�e�B�b�N (mycls_pDB���g�p���Ȃ�)
		static bool GetSelectKey_Static(char const *cSql, int *pVal, LPCSTR cIniPath, LPCSTR cSession=NULL);				// �X�^�e�B�b�N (mycls_pDB���g�p���Ȃ�)
		static bool GetSelectKey_Static(char const *cSql, float *pVal, LPCSTR cIniPath, LPCSTR cSession=NULL);				// �X�^�e�B�b�N (mycls_pDB���g�p���Ȃ�)
		static bool GetSelectKey_Static(char const *cSql, double *pVal, LPCSTR cIniPath, LPCSTR cSession=NULL);				// �X�^�e�B�b�N (mycls_pDB���g�p���Ȃ�)
		static bool GetSelectDateTime_Static(char const *cSql, int *pVal, LPCSTR cIniPath, LPCSTR cSession=NULL);			// �X�^�e�B�b�N (mycls_pDB���g�p���Ȃ�)
		static bool GetSelectImage_Static(char const *cSql, int size, char *pVal, LPCSTR cIniPath, LPCSTR cSession=NULL);	// �X�^�e�B�b�N (mycls_pDB���g�p���Ȃ�)


	//------------------------------------------
	// �֗����sSQL (�R�l�N�g���Ă����ԂŎ��s���Ă�)
	//------------------------------------------
		
		bool ExecuteDirect_ShrinkFile() { return ExecuteDirect("DBCC SHRINKFILE(2, 0)"); };			// DB���k (8K�P�ʂȂ̂Œ���)
		
		bool ExecuteDirect_AddLinkServer															// �����N�T�[�o�[�̒ǉ�
			(char const *cLinkName, char const* addr, char const* dsn, char const* uid, char const* pwd, bool bDelete=true, char const* srvproduct="mySQLServer", char const* provider="SQLNCLI");

	//------------------------------------------
	// ���S�ȕ⏕�@�\�֘A
	//------------------------------------------
		static bool CheckIniSession(LPCSTR cIniPath1, LPCSTR cSession1, LPCSTR cIniPath2, LPCSTR cSession2);		// ini�t�@�C���̃Z�b�V���������ꂩ����
		char* GetLastErrMsg() { return (char*)mycls_pDB->g_cLastErrMsgMain; }
		
	//------------------------------------------
	// �񓯊����s
	//------------------------------------------
		int	 ExecuteDirectAsync(LPCSTR cSql, bool bFast, bool bErrAutoClose=true);	// �񓯊����s
		bool ExecuteDirectAsyncCancel();											// �񓯊����s�L�����Z��

	protected:
		OdbcManager*	mycls_pDB;												// ODBC�ڑ����N���X (�N���X�𒼂Ŏg��)
	};
};