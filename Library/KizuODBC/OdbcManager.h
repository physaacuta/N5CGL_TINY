// *********************************************************************************
//	ODBC接続基底クラス
//	[Ver]
//		Ver.01    2007/01/30  vs2005 対応
//
//	[メモ]
//		使用するタスクで 『odbc32.lib』 をリンクする必要無くしました。
//	[追加ライブラリファイル]
//		Odbc32.lib
// *********************************************************************************
#pragma once

#include <windows.h>
#include <sql.h>
#include <sqlext.h>

#include "OdbcEnv.h"

#pragma warning(disable : 4996)							// ***が古い形式として宣言されました。を出さない対策

namespace KizuODBC
{
	class OdbcManager
	{
	public:
		// Fetchエラーコード
		#define KIZUODBC_FETCH_ERR							-1					// 異常
		#define KIZUODBC_FETCH_OK							0					// 正常
		#define KIZUODBC_FETCH_NODATE						1					// データ無し

	protected:
		#define KIZUODBC_ODBCMANAGER_SESSION				"DB"				// iniファイルセッション

	public:
		OdbcManager(void);
		virtual ~OdbcManager(void);

		// 状態
		bool IsConnect()	  const {return m_bIsConnect;}						// 接続状態
		bool IsCreateHandle() const {return m_bIsCreateHandle;}					// ハンドル生成状態


		// 接続
		int Connect(LPCSTR cIniPath);											// デフォルトセッションでの接続
		int Connect(LPCSTR cIniPath, LPCSTR cSession);							// セッション指定での接続
		int Connect(char const* dsn, char const* uid, char const* pwd);			// 任意指定での接続
		int ConnectDriver(char const* addr, char const* dsn, char const* uid, char const* pwd, bool bNative=false);	// ODBC未定義で接続
		void DisConnect();														// 切断
		SQLRETURN AutoCommitEnable(bool flg);									// オートコミット ON/OFF


		// 実行
		SQLRETURN ExecuteDirect(LPCSTR cSql);									// SQL文実行
		SQLRETURN ExecuteDirectAsync(LPCSTR cSql, bool bFlg);					// 非同期SQL文実行
		SQLRETURN ExecuteDirectAsyncCancel();									// 非同期SQL文実行の明示的なキャンセル

		void TranCommit()	{ SQLTransact(OdbcEnv::GetHandle(), m_hdbc, SQL_COMMIT);}			// コミット
		void TranRollBack() { SQLTransact(OdbcEnv::GetHandle(), m_hdbc, SQL_ROLLBACK);}		// ロールバック
///		void TranCommit()	{ SQLEndTran(SQL_HANDLE_ENV, m_henv, SQL_COMMIT);}		// コミット
///		void TranRollBack() { SQLEndTran(SQL_HANDLE_ENV, m_henv, SQL_ROLLBACK);}	// ロールバック
		//SQLRETURN Fetch()	{ return SQLFetch(m_hstmt); }						// フェッチ
		SQLRETURN Fetch();														// フェッチ

		// バインド実行
		SQLRETURN Prepare(LPCSTR cSql);											// パラメータ付きSQL文セット
		SQLRETURN BindParameter(SQLSMALLINT ipar, SQLUINTEGER cbColDef, SQLLEN* rgbValue);	// バインド
		SQLRETURN Execute() { return SQLExecute(m_hstmt); }						// パラメータ付きSQL文実行
		SQLRETURN ParamData(SQLPOINTER* pVal);									// パラメータ番号取得
		SQLRETURN PutData(SQLPOINTER pVal, SQLINTEGER size);					// パラメータデータをDBへ送信

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
		void GetNumCols(SQLSMALLINT *clos) {SQLNumResultCols(m_hstmt, clos); }			// 列数取得
		void GetNumCols(WORD *clos) {SQLNumResultCols(m_hstmt, (SQLSMALLINT*)clos); }	// 列数取得 (互換の為、オーバーロードを残しておく)
		void GetNumRows(SQLLEN *rows) {SQLRowCount(m_hstmt, rows);}						// 行数取得 (DELETEの場合等)
		void GetNumRows(DWORD *rows) {SQLLEN wk; GetNumRows(&wk); *rows=(DWORD)wk; }	// 行数取得 (互換の為、オーバーロードを残しておく)
		

		// デバック調査用
		void GetErrMsg();				
		void GetErrMsg(SQLSMALLINT iType);										// 異常発生時の詳細メッセージ取得
		SQLCHAR					g_cLastErrMsgMain[256];
		SQLCHAR					g_cLastErrMsgSub[256];
		SQLINTEGER				g_nLastErrMsgNo;
		

	protected:
		void Alloc();															// 接続ハンドル生成
		void Free();															// 接続ハンドル解放

		SQLHDBC					m_hdbc;											// 接続ハンドル
		SQLHSTMT				m_hstmt;										// ステートメントハンドル
		SQLCHAR					m_dsn[128];										// データベース名

		bool					m_bIsConnect;									// DBに接続中？
		bool					m_bIsCreateHandle;								// 環境・接続ハンドルの生成が正常完了？
		int						m_nLastErrNo;									// OdbcエラーNo
		int						m_nColumnIndexCheck;							// カラムアクセス(Set:GetData Reset:ExecuteDirect or Fetch) (GetData でカラムをテレコで取得すると ODBCの仕様上正常に取り込めないためチェック)

	};
};