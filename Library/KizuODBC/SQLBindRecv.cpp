#include "stdafx.h"
#include "SQLBindRecv.h"

#include "SQLBindRecv.h"
#include <stdlib.h>
#include <crtdbg.h>


const int MAXCOLUMN = 1024;
#define CHECK_RET( x ) \
	do{ int rc = x; \
		if( !SQL_SUCCEEDED(rc) ){ \
			RecordError(); \
			return false;\
		}\
		return true;\
	}while(0)

namespace KizuODBC {

	template<typename T> struct C2SQL {  enum { SQL = SQL_C_BINARY }; };
	template<> struct C2SQL<int> {  enum { SQL = SQL_C_LONG }; };
	template<> struct C2SQL<short>{  enum { SQL = SQL_C_SHORT	}; };
	template<> struct C2SQL<float> {  enum { SQL = SQL_C_FLOAT}; };
	template<> struct C2SQL<double> {  enum { SQL = SQL_C_DOUBLE }; };
	template<> struct C2SQL<long long> {  enum { SQL = SQL_C_SBIGINT }; };
	//template<> struct C2SQL<GUID>{  enum { SQL = SQL_C_GUID	}; };
	//
	class SqlAccessor {
		SQLBindRecvColumn *that;
	public:
		SqlAccessor(SQLBindRecvColumn *t) : that(t) {}
		template<typename T>
		bool bindcolt(const char *sColumn, T*resultptr, SQLLEN *pLenInd)
		{
			that->m_OutHolder.push_back(0);
			that->m_sOutColNames.push_back(sColumn);
			if (pLenInd == 0)
				pLenInd = &that->m_OutHolder[that->m_OutHolder.size()-1];
			CHECK_RET(SQLBindCol(that->GetStmt(), (SQLSMALLINT)that->m_OutHolder.size(), C2SQL<T>::SQL, resultptr, sizeof(T), pLenInd));
		}
		void RecordError()
		{
			that->RecordError();
		}
	};
};


namespace KizuODBC {

	SQLBindRecvColumn::SQLBindRecvColumn(OdbcManager &ncli, const char *sQuery)
		:SQLBindExec(ncli, sQuery, false)
	{
	}

	SQLBindRecvColumn::~SQLBindRecvColumn()
	{
	}

	
	void SQLBindRecvColumn::CreateSQL()
	{
		static const char select_statement[] = "SELECT ";
		if (_strnicmp(m_sQuery, select_statement, sizeof(select_statement) - 1) == 0) {

			char sql_key[8192];
			int len = 0;
			for (int ii = 0; ii < (int)m_sOutColNames.size(); ii++) {
				if (m_sOutColNames[ii]) {
					len += sprintf_s(sql_key + len, sizeof(sql_key) - len, "%s,", m_sOutColNames[ii]);
				}
			}
			if (len > 0) {
				sql_key[len - 1] = ' ';
			}

			m_original_sql = m_sQuery;
			m_replacedSQL = (char*)malloc(len + strlen(m_sQuery) + 2);
			sprintf(m_replacedSQL, "SELECT %s %s", sql_key, m_sQuery + sizeof(select_statement) - 1);
			m_sQuery = m_replacedSQL;
		}

	}
	SQLRETURN SQLBindRecvColumn::Fetch() {
		SQLRETURN rc = SQLFetch(m_hStmt);
		if (!SQL_SUCCEEDED(rc)) { RecordError(); }
		return rc;
	}

	///push_backが先なので、size()は、push_backしたアイテムの 1のindexとなる
	bool  SQLBindRecvColumn::BindResultNum(const char *sColumnName, int  * ptr, SQLLEN  *pLenInd)
	{

		return SqlAccessor(this).bindcolt(sColumnName, ptr, pLenInd);
	}

	bool  SQLBindRecvColumn::BindResultNum(const char *sColumnName, short  * ptr, SQLLEN  *pLenInd)
	{

		return  SqlAccessor(this).bindcolt(sColumnName, ptr, pLenInd);
	}
	bool  SQLBindRecvColumn::BindResultNum(const char *sColumnName, float  * ptr, SQLLEN  *pLenInd)
	{

		return  SqlAccessor(this).bindcolt(sColumnName, ptr, pLenInd);
	}
	bool  SQLBindRecvColumn::BindResultNum(const char *sColumnName, double  * ptr, SQLLEN  *pLenInd)
	{

		return  SqlAccessor(this).bindcolt(sColumnName, ptr, pLenInd);
	}
	bool  SQLBindRecvColumn::BindResultNum(const char *sColumnName, long long  * ptr, SQLLEN  *pLenInd)
	{

		return  SqlAccessor(this).bindcolt(sColumnName, ptr, pLenInd);
	}
	bool  SQLBindRecvColumn::BindResult(const char *sColumnName, GUID  * ptr, SQLLEN  *pLenInd)
	{

		return  SqlAccessor(this).bindcolt(sColumnName, ptr, pLenInd);
	}
	bool  SQLBindRecvColumn::BindResultStr(const char *sColumnName, char * ptr, int capacity, SQLLEN  *pLenInd)
	{
		this->m_OutHolder.push_back(0);
		this->m_sOutColNames.push_back(sColumnName);
		if (pLenInd == 0)
			pLenInd = &this->m_OutHolder[this->m_OutHolder.size() - 1];

		CHECK_RET(SQLBindCol(m_hStmt, (SQLSMALLINT)m_szHolder.size(), SQL_C_CHAR, ptr, capacity, pLenInd));
	}

	bool  SQLBindRecvColumn::BindResultBin(const char *sColumnName, void *ptr, int capacity, SQLLEN  *pLenInd)
	{
		this->m_OutHolder.push_back(0);
		this->m_sOutColNames.push_back(sColumnName);
		if (pLenInd == 0)
			pLenInd = &this->m_OutHolder[this->m_OutHolder.size() - 1];
		CHECK_RET(SQLBindCol(m_hStmt, (SQLSMALLINT)m_szHolder.size(), SQL_C_BINARY, ptr, capacity, pLenInd));
	}
};
