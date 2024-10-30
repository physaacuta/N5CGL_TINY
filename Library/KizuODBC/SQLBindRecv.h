#pragma once

// *********************************************************************************
//	VB.NET側のSQLCommandみたいなクラスのODBC版。
//	[Ver]
//		Ver.01    2015/09/07  
//
//	[メモ]
//		ExecuteまえにBindResult...で、列と結果格納変数を紐づけておく。
//		Executeして、Fetchしたら、その変数が書き換わるしくみ。
//		BindResultはポインタにするので、Fetch完了まで実体が存在すること。（特に自動変数の寿命に注意）。

//	[SQL]
//		"SELECT FROM Table WHERE..." などと、列名を記載しない形式で書く
//		列名は、Execute直前に埋まる。
//		WHERE句側のBindは列名はNULLであること。
// *********************************************************************************


#include "SQLBindExec.h"
namespace KizuODBC {

	class SQLBindRecvColumn : public SQLBindExec
	{
	protected:
		friend class SqlAccessor;
		std::deque<SQLLEN>		m_OutHolder;		//Bind変数のサイズホルダー.[1]が第一列。[0]は未使用。
		std::deque<const char *>m_sOutColNames;


	public:
        //sQuery; "SELECT FROM Table" などと、列名を記載しない形式で書く
        //  列名は、Execute直前で挿入される
		SQLBindRecvColumn(OdbcManager &ncli, const char *sQuery);
		~SQLBindRecvColumn();

		virtual void CreateSQL() override;
		
		
        SQLRETURN  Fetch();

		//結果のBinding。(Fetch毎にBindされた変数の中身が変わる）

		//NULLかどうかをチェックしたいばあいは、別変数としてpLenIndでにポインタ渡して中身を見る(-1:NULL)。
		bool  BindResultStr(const char *sColumnName, char * ptr, int capacity, SQLLEN  *pLenInd = 0);

		bool  BindResultNum(const char *sColumnName, int  * ptr, SQLLEN  *pLenInd = 0);
		bool  BindResultNum(const char *sColumnName, short  * ptr, SQLLEN  *pLenInd = 0);
		bool  BindResultNum(const char *sColumnName, float  * ptr, SQLLEN  *pLenInd = 0);
		bool  BindResultNum(const char *sColumnName, double  * ptr, SQLLEN  *pLenInd = 0);
		bool  BindResultNum(const char *sColumnName, long long  * ptr, SQLLEN  *pLenInd = 0);
		bool  BindResult(const char *sColumnName, GUID* ptr, SQLLEN  *pLenInd = 0);
		
				//結果をNative型で戻す。（変換しない）
		bool  BindResultBin(const char *sColumnName, void *ptr, int capacity, SQLLEN  *pLenInd = 0);
	};

};
