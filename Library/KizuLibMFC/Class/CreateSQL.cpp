#include "StdAfx.h"
#include "CreateSQL.h"

using namespace KizuLib;

CreateSQL::CreateSQL(void)
{
}

CreateSQL::~CreateSQL(void)
{
}

//------------------------------------------
// insert
// CString *cSql 編集するSQL文
// const char *val 追加する文字列
// int size 文字列の長さ (0の場合、NULLを自動セット)
// 戻り値 取得バイト数
//------------------------------------------
// 文字列
void CreateSQL::sqlColumnInsert(CString *cSql, const char *val, int size)
{
	CString str;
	CString wk(val, size);		// 指定文字数セットコンストラクタ
	if( 0 == strlen(wk) ) str = "NULL,";
	else str.Format("'%s',", wk);
	*cSql += str;				// 追加
	return;
}
// 整数
void CreateSQL::sqlColumnInsert(CString *cSql, short val)
{
	char str[256];
	sprintf(str, "%d,", val);
	*cSql += str;				// 追加
}
void CreateSQL::sqlColumnInsert(CString *cSql, int val)
{
	char str[256];
	sprintf(str, "%d,", val);
	*cSql += str;				// 追加
}
void CreateSQL::sqlColumnInsert(CString *cSql, long val)
{
	char str[256];
	sprintf(str, "%d,", val);
	*cSql += str;				// 追加
}
void CreateSQL::sqlColumnInsert(CString *cSql, DWORD val)
{
	char str[256];
	sprintf(str, "%d,", val);
	*cSql += str;				// 追加
}
// 実数
void CreateSQL::sqlColumnInsert(CString *cSql, float val)
{
	char str[256];
	sprintf(str, "%0.6f,", val);
	*cSql += str;				// 追加
}
void CreateSQL::sqlColumnInsert(CString *cSql, double val)
{
	char str[256*2];
	sprintf(str, "%0.15lf,", val);
	*cSql += str;				// 追加
}

// 文字列
void CreateSQL::sqlColumnInsert(CString *cSql, const char *val)
{
	*cSql += "'";	
	*cSql += val;
	*cSql += "',";
}
void CreateSQL::sqlColumnInsert(CString *cSql, CString val)
{
	*cSql += "'";	
	*cSql += val;
	*cSql += "',";
	return;
}

//------------------------------------------
// update
// CString *cSql 編集するSQL文
// const char *name 項目名称
// const char *val 追加する文字列
// int size 文字列の長さ (0の場合、NULLを自動セット)
// 戻り値 取得バイト数
//------------------------------------------

// 文字列
void CreateSQL::sqlColumnUpdate(CString *cSql, const char *name, const char *val, int size)
{
	CString str;
	CString wk(val, size);		// 指定文字数セットコンストラクタ

	// NULLかチェック
	if( 0 == strlen(wk)) str.Format("%s=NULL,", name);
	else str.Format("%s='%s',", name, wk);
	*cSql += str;				// 追加
}
// 整数
void CreateSQL::sqlColumnUpdate(CString *cSql, const char *name, short val)
{
	char str[256];
	sprintf(str, "%s=%d,", name, val);
	*cSql += str;				// 追加
}

void CreateSQL::sqlColumnUpdate(CString *cSql, const char *name, int val)
{
	char str[256];
	sprintf(str, "%s=%d,", name, val);
	*cSql += str;				// 追加
}
void CreateSQL::sqlColumnUpdate(CString *cSql, const char *name, long val)
{
	char str[256];
	sprintf(str, "%s=%d,", name, val);
	*cSql += str;				// 追加
}
void CreateSQL::sqlColumnUpdate(CString *cSql, const char *name, DWORD val)
{
	char str[256];
	sprintf(str, "%s=%d,", name, val);
	*cSql += str;				// 追加
}
// 実数
void CreateSQL::sqlColumnUpdate(CString *cSql, const char *name, float val)
{
	char str[256];
	sprintf(str, "%s=%0.6f,", name, val);
	*cSql += str;				// 追加
}
void CreateSQL::sqlColumnUpdate(CString *cSql, const char *name, double val)
{
	char str[256];
	sprintf(str, "%s=%0.15lf,", name, val);
	*cSql += str;				// 追加
}
// 文字列
void CreateSQL::sqlColumnUpdate(CString *cSql, const char *name, CString val)
{
	CString str;
	str.Format("%s='%s',", name, val);
	*cSql += str;				// 追加
}

//------------------------------------------
// sqlColumnInsert,sqlColumnUpdateで最後についた ","を取っ払う
// CString *cSql 編集するSQL文
//------------------------------------------
void CreateSQL::sqlColumnTrim(CString *cSql)
{
	if(cSql->Right(1) == ",") 
		cSql->Delete(cSql->GetLength()-1);	// 最後の","を削除
}

//------------------------------------------
// SQL文の登録日付を生成 (文字列)
// CString *cSql 編集するSQL文
// bool bAddCanma 最後のカンマを付与するしない
//------------------------------------------
void CreateSQL::sqlColumnAddTime(CString *cSql, bool bAddCanma)
{
	CTime t = CTime::GetCurrentTime();
	CString str = t.Format("'%Y/%m/%d %H:%M:%S'");
	if(bAddCanma) str += ",";

	*cSql += str;				// 追加 
}

//------------------------------------------
// SQL文の登録日付を生成 (文字列)
// CString *cSql 編集するSQL文
// CTime t セットする時刻
//------------------------------------------
void CreateSQL::sqlColumnAddTime(CString *cSql, CTime t, bool bAddCanma)
{
	CString str = t.Format("'%Y/%m/%d %H:%M:%S'");
	if(bAddCanma) str += ",";
	*cSql += str;				// 追加 
}

//------------------------------------------
// 文字列[yyyymmddhhnnss]形式から[yyyy/mm/dd hh:nn:ss]形式に変換
// char const* pWk [yyyymmddhhnnss]形式の文字列
//------------------------------------------
CString CreateSQL::StringToTime(char const* pWk)
{
	CString		wk;
	char cTime[20];
	memset(cTime, 0x00, sizeof(cTime));

	// yyyy/mm/dd hh:nn:ss
	// 0123456789012345678
	// yyyymmddhhnnss
	memcpy(&cTime[0], &pWk[0], 4);
	cTime[4] = '/';
	memcpy(&cTime[5], &pWk[4], 2);
	cTime[7] = '/';
	memcpy(&cTime[8], &pWk[6], 2);
	cTime[10] = ' ';
	memcpy(&cTime[11], &pWk[8], 2);
	cTime[13] = ':';
	memcpy(&cTime[14], &pWk[10], 2);
	cTime[16] = ':';
	memcpy(&cTime[17], &pWk[12], 2);
	
	wk = cTime;
	return wk;
}

//------------------------------------------
// 文字列から前後のスペースを取り除く
// CString *cSql 編集するSQL文
//------------------------------------------
CString CreateSQL::StringTrim(const char *val, int size)
{
	int	nSize = (int)strlen(val);
	if(size > nSize) size = nSize;
	CString	wk(val, size);		// どうやら sizeより実際の文字列が少ない場合、残りのエリアはNULLでないへんなデータが付加されそう
	return wk.Trim();
}