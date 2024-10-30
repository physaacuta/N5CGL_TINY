#include "stdafx.h"
#include "SQLBindExec.h"
#include <stdlib.h>
#include <crtdbg.h>


const int m_nDefMaxStringWidth = 64;
const int MAXCOLUMN = 1024;

namespace KizuODBC {

	//胡散臭い継承クラス。メンバ変数を足さなければ親から子供にstatic_castしても大丈夫。
	class OdbcHelper : public KizuODBC::OdbcManager
	{
	public:
		HSTMT GetDBC() const { return m_hdbc; }

		void SetStmt(HSTMT hStmt) {

			if (m_hstmt) { SQLFreeHandle(SQL_HANDLE_STMT, m_hstmt); m_hstmt = NULL; }
			m_hstmt = hStmt;
		}
	};

	// managaer: odbc connection,
	// sQuery  : Query String
	// bPrep   : true:Prepared, Or Not(default)
	SQLBindExec::SQLBindExec(KizuODBC::OdbcManager	 &manager, const char *sQuery, bool bPrep)
		:m_sQuery(sQuery)
		, m_bPrep(bPrep)
		, BlobDataRequested(&SQLBindExec::OnBlobData_SendFile)
	{
		m_hDBC = static_cast<OdbcHelper &>(manager).GetDBC();
		SQLAllocHandle(SQL_HANDLE_STMT, m_hDBC, &m_hStmt);
		m_original_sql = 0;
		m_replacedSQL = 0;

		m_cLastErrMsgMain[0] = 0;
	}


	SQLBindExec::~SQLBindExec(void)
	{
		if (m_hStmt) {
			SQLFreeHandle(SQL_HANDLE_STMT, m_hStmt);
			m_hStmt = NULL;
		}
		free(m_replacedSQL);
		m_replacedSQL = 0;
	}


	int SQLBindExec::RecordError()
	{
		SQLCHAR					cLastErrMsgSub[256] = "";
		SQLINTEGER nLastErrMsgNo = 0;
		memset(m_cLastErrMsgMain, 0x00, sizeof(m_cLastErrMsgMain));

		if (m_hStmt && m_hDBC) {
			SQLSMALLINT retc = 0;
			SQLError(0, m_hDBC, m_hStmt, cLastErrMsgSub, &nLastErrMsgNo, m_cLastErrMsgMain, (SQLSMALLINT)sizeof(m_cLastErrMsgMain), &retc);

			_RPT0(_CRT_WARN, m_cLastErrMsgMain);
			_RPT0(_CRT_WARN, "\n");
		}
		return nLastErrMsgNo;
		//return (char*)m_cLastErrMsgMain;
	}

#define CHECK_RET( x ) \
	do{ SQLRETURN rc = x; \
		if( !SQL_SUCCEEDED(rc) ){ \
			RecordError(); \
		}\
		return rc;\
	}while(0)



	SQLLEN* SQLBindExec::set_holder(const char *sColName, int len) {
		m_sColNames.push_back(sColName);
		m_szHolder.push_back(len);

		return &m_szHolder[m_szHolder.size()-1];
	}

	//パラメタをバインド。数値版
	int SQLBindExec::BindNum(const char *sColName, const int *nPrepKey)
	{
		SQLLEN *pIndLen = set_holder(sColName, 4);

		CHECK_RET(SQLBindParameter(m_hStmt, (SQLSMALLINT)m_szHolder.size(), SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, 0, 0, (SQLPOINTER)nPrepKey, 0, pIndLen));
	}

	//パラメタをバインド。数値版
	int SQLBindExec::BindNum(const char *sColName, const float *dPrepKey)
	{
		SQLLEN *pIndLen = set_holder(sColName, 4);

		CHECK_RET(SQLBindParameter(m_hStmt, (SQLSMALLINT)m_szHolder.size(), SQL_PARAM_INPUT, SQL_C_FLOAT, SQL_REAL, 0, 0, (SQLPOINTER)dPrepKey, 0, pIndLen));
	}

	//パラメタをバインド。数値版
	int SQLBindExec::BindNum(const char *sColName, const double *dPrepKey)
	{
		SQLLEN *pIndLen = set_holder(sColName, 8);

		CHECK_RET(SQLBindParameter(m_hStmt, (SQLSMALLINT)m_szHolder.size(), SQL_PARAM_INPUT, SQL_C_DOUBLE, SQL_DOUBLE, 0, 0, (SQLPOINTER)dPrepKey, 0, pIndLen));
	}

	//パラメタをバインド。文字列版
	int SQLBindExec::BindStr(const char *sColName, const char *pPrepKey, int nMaxColumnWidth)
	{
		//最大幅。クエリの組立て(Cacheの当たり具合)に効いてくる。
		if (nMaxColumnWidth < 0) {
			nMaxColumnWidth = m_nDefMaxStringWidth;
		}
		SQLLEN *pIndLen = set_holder(sColName, SQL_NTS);
		CHECK_RET(SQLBindParameter(m_hStmt, (SQLSMALLINT)m_szHolder.size(), SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR, nMaxColumnWidth, 0, (SQLPOINTER)pPrepKey, 0, pIndLen));
	}

	//パラメタをバインド。固定長文字列版（NUL文字がない）
	int SQLBindExec::BindFix(const char *sColName, const char *pPrepKey, int nMaxColumnWidth)
	{
		SQLLEN *pIndLen = set_holder(sColName, nMaxColumnWidth);
		CHECK_RET(SQLBindParameter(m_hStmt, (SQLSMALLINT)m_szHolder.size(), SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR, nMaxColumnWidth, 0, (SQLPOINTER)pPrepKey, 0, pIndLen));
	}

	//パラメタをバインド。UUID版. ファイルテーブルで使用するかも。
	int SQLBindExec::Bind(const char *sColName, const UUID *pPrepKey)
	{
		SQLLEN *pIndLen = set_holder(sColName, 16);
		CHECK_RET(SQLBindParameter(m_hStmt, (SQLSMALLINT)m_szHolder.size(), SQL_PARAM_INPUT, SQL_C_GUID, SQL_GUID, 0, 0, (SQLPOINTER)pPrepKey, 0, pIndLen));
	}

	//パラメタをバインド。日時型。
	int SQLBindExec::Bind(const char *sColName, const SQL_TIMESTAMP_STRUCT *pTimeStamp)
	{
		SQLLEN *pIndLen = set_holder(sColName, 16);
		CHECK_RET(SQLBindParameter(m_hStmt, (SQLSMALLINT)m_szHolder.size(), SQL_PARAM_INPUT, SQL_C_TIMESTAMP, SQL_DATETIME, 0, 0, (SQLPOINTER)pTimeStamp, 0, pIndLen));
	}

	//BLOB (IMAGE, VarBniary(MAX) )パラメタをバインド。データはあとから突っ込む
	int SQLBindExec::BindBlobBin(const char *sColName, const void *pToken, int nMaxSize)
	{
		SQLLEN *pIndLen = set_holder(sColName, SQL_LEN_DATA_AT_EXEC(0));
		CHECK_RET(SQLBindParameter(m_hStmt, (SQLSMALLINT)m_szHolder.size(), SQL_PARAM_INPUT, SQL_C_BINARY, SQL_LONGVARBINARY, nMaxSize, 0, (SQLPOINTER)pToken, 0, pIndLen));
	}

	//BLOB( VarChar(MAX) )パラメタをバインド。データはあとから突っ込む
	int SQLBindExec::BindBlobChar(const char *sColName, const void *pToken, int nMaxSize)
	{
		SQLLEN *pIndLen = set_holder(sColName, SQL_LEN_DATA_AT_EXEC(0));
		CHECK_RET(SQLBindParameter(m_hStmt, (SQLSMALLINT)m_szHolder.size(), SQL_PARAM_INPUT, SQL_C_CHAR, SQL_LONGVARCHAR, nMaxSize, 0, (SQLPOINTER)pToken, 0, pIndLen));
	}

	void SQLBindExec::CreateSQL()
	{
		static const char insert_statement[] = "INSERT INTO ";

		if (_strnicmp(m_sQuery, insert_statement, sizeof(insert_statement) - 1) == 0) {

			//already contain placeholder.
			if (strchr(m_sQuery, '?') != NULL)
				return;

			char sql_key[8192] = " VALUES (";
			int di = strlen(sql_key);
			for (int ii = 0; ii < (int)m_szHolder.size(); ii++) {
				if (m_szHolder[ii] != 0) {
					sql_key[di] = '?'; di++;
					sql_key[di] = ','; di++;
				}
				else if (m_sColNames[ii]) {
					di += sprintf_s(sql_key + di, sizeof(sql_key) - di, "%s,", m_sColNames[ii]);
				}
			}
			if (di > 0) {
				sql_key[di - 1] = ')';
			}

			m_replacedSQL = (char*)malloc(di + strlen(m_sQuery) + 2);
			sprintf(m_replacedSQL, "%s %s", m_sQuery, sql_key);
			m_sQuery = m_replacedSQL;
		}
		
		static const char update_statement[] = "UPDATE ";
		if (_strnicmp(m_sQuery, update_statement, sizeof(update_statement) - 1) == 0) {
			char sql_key[8192] = " SET ";
			int di = strlen(sql_key);
			for (int ii = 0; ii < (int)m_szHolder.size(); ii++) {
				const char *column = m_sColNames[ii];
				if (column) {
					if (m_szHolder[ii] != 0) {
						di += sprintf_s(sql_key + di, sizeof(sql_key) - di, "%s=?,", column);
					}
					else {
						di += sprintf_s(sql_key + di, sizeof(sql_key) - di, "%s,", column);
					}
				}
			}
			if (di > 0) {
				sql_key[di - 1] = ' ';
			}
			const char *tablestart = m_sQuery + sizeof(update_statement) - 1;

			char table[256];
			int rempos;
			sscanf(tablestart, "%s %n", table, &rempos);

			m_replacedSQL = (char*)malloc(di + strlen(m_sQuery) + 2);
			sprintf(m_replacedSQL, "UPDATE %s %s %s", table, sql_key, &tablestart[rempos]);
			m_sQuery = m_replacedSQL;

		}
	}
	

	//実行する。
	bool SQLBindExec::Execute()
	{
		if (m_original_sql == NULL)
		{
			m_original_sql = m_sQuery;
			CreateSQL();

			if (m_bPrep)
			{
				if (!SQL_SUCCEEDED(SQLPrepare(m_hStmt, (SQLCHAR *)m_sQuery, SQL_NTS))) {
					RecordError();
					return false;
				}
			}
		}
		SQLRETURN rc;
		if (m_bPrep) {
			rc = SQLExecute(m_hStmt);
		}
		else {
			rc = SQLExecDirect(m_hStmt, (SQLCHAR *)m_sQuery, SQL_NTS);
		}

		//継続データが必要なばあい。
		while (rc == SQL_NEED_DATA) {
			SQLPOINTER pPtr = NULL;
			rc = SQLParamData(m_hStmt, &pPtr);

			if (rc == SQL_NEED_DATA) {
				// PutDataさせるための仮想関数呼び出し。
				SQLRETURN rc2 = OnRequestBlobData(pPtr);
				if (!SQL_SUCCEEDED(rc2)) {
					return false;
				}
			}
		}

		if (!SQL_SUCCEEDED(rc)) {
			RecordError();
			return false;
		}
		return true;
	}

	//実行する。
	SQLRETURN SQLBindExec::SelectExecute(KizuODBC::OdbcManager &manager)
	{
		SQLRETURN rc = Execute();
		//実行後は、hStmtをOdbcManagerに譲る
		static_cast<OdbcHelper &>(manager).SetStmt(m_hStmt);
		m_hStmt = NULL;
		if (!SQL_SUCCEEDED(rc)) {
			manager.GetErrMsg();
		}
		return rc;
	}


	//BLOBデータを実際に送信する。
	SQLRETURN SQLBindExec::PutData(SQLPOINTER pData, SQLINTEGER nLen)
	{
		CHECK_RET(SQLPutData(m_hStmt, pData, nLen));
	}

	//Blobデータを送信する必要がある。仮想関数
	SQLRETURN SQLBindExec::OnRequestBlobData(SQLPOINTER pToken)
	{
		//コールバックに丸投げ。
		if (BlobDataRequested) {
			return BlobDataRequested(this, pToken);
		}
		return SQL_ERROR;
	}

	//読み込み用にファイルを開く。
	static HANDLE OpenForRead(const char *pFilePath)
	{
		return CreateFile(pFilePath, GENERIC_READ,
			FILE_SHARE_READ | FILE_SHARE_WRITE,
			NULL, OPEN_EXISTING, 0, NULL);

	}


	//Blobデータを送信する必要がある。コールバック。サンプル。デフォルトハンドラ。
	//pTokenを絶対ファイル名として、送信する。
	SQLRETURN SQLBindExec::OnBlobData_SendFile(SQLBindExec *that, SQLPOINTER pToken)
	{
		SQLRETURN rc;
		if (pToken) {
			const char* pFilePath = reinterpret_cast<const char*>(pToken);
			HANDLE hFile = OpenForRead(pFilePath);
			if (INVALID_HANDLE_VALUE != hFile) {
				const DWORD sz_want = 128 * 1024;
				BYTE *uccdata = (BYTE*)malloc(sz_want);
				DWORD sz_read;
				do {
					sz_read = 0;
					ReadFile(hFile, &uccdata[0], sz_want, &sz_read, NULL);
					rc = that->PutData(&uccdata[0], sz_read);
				} while (sz_want == sz_read);

				free(uccdata);
				CloseHandle(hFile);
			}
			else {
				rc = that->PutEmpty();
			}
		}
		else {
			rc = that->PutNULL();
		}
		return rc;

	}

	long long SQLBindExec::GetFileSize(const char *pFilePath)
	{
		LARGE_INTEGER filesize;
		filesize.QuadPart = -1;

		HANDLE hFile = OpenForRead(pFilePath);
		if (INVALID_HANDLE_VALUE != hFile) {
			GetFileSizeEx(hFile, &filesize);

			CloseHandle(hFile);
		}

		return filesize.QuadPart;
	}

	//Blobデータを送信する必要がある。コールバック。サンプル2。
	//pTokenをメモリとして、送信する。
	SQLRETURN SQLBindExec::OnBlobData_SendMemory(SQLBindExec *that, SQLPOINTER pToken)
	{
		SQLRETURN rc;
		if (pToken) {
			SendMemoryType *pMem = reinterpret_cast<SendMemoryType *>(pToken);

			if (pMem->size > 0) {
				rc = that->PutData((SQLPOINTER)pMem->data, (SQLINTEGER)pMem->size);
			}
			else {
				rc = that->PutEmpty();
			}
		}
		else {
			rc = that->PutNULL();
		}
		return rc;

	}
};