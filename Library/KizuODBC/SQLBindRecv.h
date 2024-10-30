#pragma once

// *********************************************************************************
//	VB.NET����SQLCommand�݂����ȃN���X��ODBC�ŁB
//	[Ver]
//		Ver.01    2015/09/07  
//
//	[����]
//		Execute�܂���BindResult...�ŁA��ƌ��ʊi�[�ϐ���R�Â��Ă����B
//		Execute���āAFetch������A���̕ϐ�����������邵���݁B
//		BindResult�̓|�C���^�ɂ���̂ŁAFetch�����܂Ŏ��̂����݂��邱�ƁB�i���Ɏ����ϐ��̎����ɒ��Ӂj�B

//	[SQL]
//		"SELECT FROM Table WHERE..." �ȂǂƁA�񖼂��L�ڂ��Ȃ��`���ŏ���
//		�񖼂́AExecute���O�ɖ��܂�B
//		WHERE�呤��Bind�͗񖼂�NULL�ł��邱�ƁB
// *********************************************************************************


#include "SQLBindExec.h"
namespace KizuODBC {

	class SQLBindRecvColumn : public SQLBindExec
	{
	protected:
		friend class SqlAccessor;
		std::deque<SQLLEN>		m_OutHolder;		//Bind�ϐ��̃T�C�Y�z���_�[.[1]������B[0]�͖��g�p�B
		std::deque<const char *>m_sOutColNames;


	public:
        //sQuery; "SELECT FROM Table" �ȂǂƁA�񖼂��L�ڂ��Ȃ��`���ŏ���
        //  �񖼂́AExecute���O�ő}�������
		SQLBindRecvColumn(OdbcManager &ncli, const char *sQuery);
		~SQLBindRecvColumn();

		virtual void CreateSQL() override;
		
		
        SQLRETURN  Fetch();

		//���ʂ�Binding�B(Fetch����Bind���ꂽ�ϐ��̒��g���ς��j

		//NULL���ǂ������`�F�b�N�������΂����́A�ʕϐ��Ƃ���pLenInd�łɃ|�C���^�n���Ē��g������(-1:NULL)�B
		bool  BindResultStr(const char *sColumnName, char * ptr, int capacity, SQLLEN  *pLenInd = 0);

		bool  BindResultNum(const char *sColumnName, int  * ptr, SQLLEN  *pLenInd = 0);
		bool  BindResultNum(const char *sColumnName, short  * ptr, SQLLEN  *pLenInd = 0);
		bool  BindResultNum(const char *sColumnName, float  * ptr, SQLLEN  *pLenInd = 0);
		bool  BindResultNum(const char *sColumnName, double  * ptr, SQLLEN  *pLenInd = 0);
		bool  BindResultNum(const char *sColumnName, long long  * ptr, SQLLEN  *pLenInd = 0);
		bool  BindResult(const char *sColumnName, GUID* ptr, SQLLEN  *pLenInd = 0);
		
				//���ʂ�Native�^�Ŗ߂��B�i�ϊ����Ȃ��j
		bool  BindResultBin(const char *sColumnName, void *ptr, int capacity, SQLLEN  *pLenInd = 0);
	};

};
