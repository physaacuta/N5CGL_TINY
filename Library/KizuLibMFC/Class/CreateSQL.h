// *********************************************************************************
//	SQL文作成サポート クラス
//	[Ver]
//		Ver.01    2007/04/21  vs2005 対応
//
//	[メモ]
//		OdbcBaseではCStringが使えない為、このクラスを作ってみた
//		ちょっとした奴なら,OdbcBaseのchar型のSQL文作成でも問題ない
// *********************************************************************************
#pragma once
#pragma warning(disable : 4996)							// ***が古い形式として宣言されました。を出さない対策

namespace KizuLib
{
	class CreateSQL
	{
	public:
		CreateSQL(void);
		~CreateSQL(void);
		
		static void sqlColumnInsert(CString *cSql, const char *val, int size);
		static void sqlColumnInsert(CString *cSql, short val);	
		static void sqlColumnInsert(CString *cSql, int val);						
		static void sqlColumnInsert(CString *cSql, long val);
		static void sqlColumnInsert(CString *cSql, DWORD val);
		static void sqlColumnInsert(CString *cSql, float val);
		static void sqlColumnInsert(CString *cSql, double val);
		static void sqlColumnInsert(CString *cSql, const char *val);
		static void sqlColumnInsert(CString *cSql, CString val);

		// update
		static void sqlColumnUpdate(CString *cSql, const char *name, const char *val, int size);
		static void sqlColumnUpdate(CString *cSql, const char *name, short val);
		static void sqlColumnUpdate(CString *cSql, const char *name, int val);
		static void sqlColumnUpdate(CString *cSql, const char *name, long val);
		static void sqlColumnUpdate(CString *cSql, const char *name, DWORD val);
		static void sqlColumnUpdate(CString *cSql, const char *name, float val);
		static void sqlColumnUpdate(CString *cSql, const char *name, double val);
		static void sqlColumnUpdate(CString *cSql, const char *name, CString val);

		// それ以外
		static void sqlColumnTrim(CString *cSql);									// sqlColumnInsert,sqlColumnUpdateで最後についた ","を取っ払う
		static void sqlColumnAddTime(CString *cSql, bool bAddCanma=false);			// 時刻を付与
		static void sqlColumnAddTime(CString *cSql, CTime t, bool bAddCanma=false);


		static CString StringToTime(char const* pWk);								// 文字列[yyyymmddhhnnss]形式から[yyyy/mm/dd hh:nn:ss]形式に変換
		static CString StringTrim(const char *val, int size);						// 文字列から前後のスペースを取り除く

	};
};