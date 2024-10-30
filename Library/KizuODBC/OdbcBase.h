// *********************************************************************************
//	ODBC クラス
//	[Ver]
//		Ver.01    2007/01/30  vs2005 対応
//
//	[メモ]
//		使用するタスクで 『odbc32.lib』 をリンクする必要無くしました。
//		OdbcManager を使いやすいように皮被せ
//	[追加ライブラリファイル]
//		Odbc32.lib
// *********************************************************************************

#pragma once

#include <stdio.h>
#include "OdbcManager.h"														// ODBC接続基底クラス

namespace KizuODBC
{
	class OdbcBase
	{
	private:
		static const __int64 BinalyFileOneWriteSize=2*1024*1024;				// バイナリファイル1回の最大書き込みサイズ


	public:

		OdbcBase(void);
		virtual ~OdbcBase(void);
		
	//------------------------------------------
	// OdbcManager の皮被せ関連
	//------------------------------------------
		static bool OpenCheck(LPCSTR cIniPath, LPCSTR cSession = NULL);			// 接続チェック
		static bool OpenCheck(char const* dsn, char const* uid, char const* pwd); // 任意指定で接続チェック
		static bool OpenCheck(char const* addr, char const* dsn, char const* uid, char const* pwd, bool bNative=false); // ODBCを未定義で接続チェック


		// 接続
		bool Connect(LPCSTR cIniPath, LPCSTR cSession = NULL);					// セッション指定での接続
		bool Connect(char const* dsn, char const* uid, char const* pwd);		// 任意指定での接続
		bool ConnectDriver(char const* addr, char const* dsn, char const* uid, char const* pwd, bool bNative=false); // ODBCを未定義で接続


//		bool Connect();															// デフォルトセッションで接続
		void DisConnect();														// 切断
		bool IsConnect() const {return mycls_pDB->IsConnect();}					// 接続中か確認
		bool AutoCommitEnable(bool flg);										// オートコミット ON/OFF

		// 実行
		bool ExecuteDirect(LPCSTR cSql, bool bErrAutoClose = true);				// SQL文実行
		bool TranCommit();														// コミット
		bool TranRollBack();													// ロールバック
		int  Fetch();															// フェッチ

		// バイナリデータ登録関連
		bool SqlBinalyWrite(LPCSTR cSql, int paramCnt, DWORD *size, LPVOID *pVal, bool bErrAutoClose = true);				// 複数件バイナリ登録
		bool SqlBinalyWrite(LPCSTR cSql, DWORD size, LPVOID pVal, bool bErrAutoClose = true);								// 1件バイナリ登録 (簡易版)
		bool SqlBinalyWrite(LPCSTR cSql, DWORD size1, DWORD size2, LPVOID pVal1, LPVOID pVal2, bool bErrAutoClose = true);	// 2件バイナリ登録 (簡易版)

		// 超巨大なバイナリファイルを登録 (取得時も注意が必要)
		bool SqlBinalyFileWrite(LPCSTR cSql, int paramCnt, int pathsize, const char* cFPath, bool bErrAutoClose = true);	// 複数件バイナリ登録

		// データ取得
		long GetData(WORD n, LPVOID pVal, DWORD size );							// 文字 データ取得
		long GetData(WORD n, short *pVal );										// 整数 データ取得
		long GetData(WORD n, int *pVal );										// 整数 データ取得
		long GetData(WORD n, long *pVal );										// 整数 データ取得
		long GetData(WORD n, float *pVal );										// 実数 データ取得
		long GetData(WORD n, double *pVal );									// 実数 データ取得
		
		long GetDataDate(WORD n, int *pVal);									// 日付 (年月日) データ取得
		long GetDataDateTime(WORD n, int *pVal);								// 日付 (年月日時分秒) データ取得

		long GetDataImage(WORD n, LPVOID pVal, DWORD size );					// バイナリ データ取得

		// その他情報取得
		void GetNumCols(WORD *clos) {if( ! mycls_pDB->IsConnect()) return; mycls_pDB->GetNumCols(clos); }	// 列数取得
		void GetNumRows(DWORD *rows){if( ! mycls_pDB->IsConnect()) return; mycls_pDB->GetNumRows(rows); }	// 行数取得 (DELETEの場合等で、SQLを発行した後で使用)
	
		const SQLCHAR *GetErrorDetail() const  { return mycls_pDB->g_cLastErrMsgMain; }

	//------------------------------------------
	// SQL文 文字操作関連
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
		
		// それ以外
		static int sqlColumnTrim(LPSTR pSql);									// sqlColumnUpdateで最後についた ","を取っ払う
		static int sqlColumnAddTime(LPSTR pSql);								// 時刻を付与


	//------------------------------------------
	// 標準的なSelect文の実行取得
	//------------------------------------------
		// 単一行1項目のintデータのみ取得 (おもにcount(*)のSQL文実行に使用)
		bool GetSelectKey(char const *cSql, int size, char *pVal);	
		bool GetSelectKey(char const *cSql, int *pVal);
		bool GetSelectKey(char const *cSql, float *pVal);
		bool GetSelectKey(char const *cSql, double *pVal);
		bool GetSelectDateTime(char const *cSql, int *pVal);
		bool GetSelectImage(char const *cSql, int size, char *pVal);
		static bool GetSelectKey_Static(char const *cSql, int size, char *pVal, LPCSTR cIniPath, LPCSTR cSession=NULL);		// スタティック (mycls_pDBを使用しない)
		static bool GetSelectKey_Static(char const *cSql, int *pVal, LPCSTR cIniPath, LPCSTR cSession=NULL);				// スタティック (mycls_pDBを使用しない)
		static bool GetSelectKey_Static(char const *cSql, float *pVal, LPCSTR cIniPath, LPCSTR cSession=NULL);				// スタティック (mycls_pDBを使用しない)
		static bool GetSelectKey_Static(char const *cSql, double *pVal, LPCSTR cIniPath, LPCSTR cSession=NULL);				// スタティック (mycls_pDBを使用しない)
		static bool GetSelectDateTime_Static(char const *cSql, int *pVal, LPCSTR cIniPath, LPCSTR cSession=NULL);			// スタティック (mycls_pDBを使用しない)
		static bool GetSelectImage_Static(char const *cSql, int size, char *pVal, LPCSTR cIniPath, LPCSTR cSession=NULL);	// スタティック (mycls_pDBを使用しない)


	//------------------------------------------
	// 便利実行SQL (コネクトしている状態で実行してね)
	//------------------------------------------
		
		bool ExecuteDirect_ShrinkFile() { return ExecuteDirect("DBCC SHRINKFILE(2, 0)"); };			// DB圧縮 (8K単位なので注意)
		
		bool ExecuteDirect_AddLinkServer															// リンクサーバーの追加
			(char const *cLinkName, char const* addr, char const* dsn, char const* uid, char const* pwd, bool bDelete=true, char const* srvproduct="mySQLServer", char const* provider="SQLNCLI");

	//------------------------------------------
	// 完全な補助機能関連
	//------------------------------------------
		static bool CheckIniSession(LPCSTR cIniPath1, LPCSTR cSession1, LPCSTR cIniPath2, LPCSTR cSession2);		// iniファイルのセッションが同一か判定
		char* GetLastErrMsg() { return (char*)mycls_pDB->g_cLastErrMsgMain; }
		
	//------------------------------------------
	// 非同期実行
	//------------------------------------------
		int	 ExecuteDirectAsync(LPCSTR cSql, bool bFast, bool bErrAutoClose=true);	// 非同期実行
		bool ExecuteDirectAsyncCancel();											// 非同期実行キャンセル

	protected:
		OdbcManager*	mycls_pDB;												// ODBC接続基底クラス (クラスを直で使う)
	};
};