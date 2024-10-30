// *********************************************************************************
//	ODBC接続用環境ハンドル生成クラス
//	[Ver]
//		Ver.01    2008/01/13  vs2005 対応
//
//	[メモ]
//		singletonデザインパターンで設計 (= privateコンストラクタ+staticインスタンス)
//		exeで唯一のインスタンス
//		staticコンストラクタで生成されて、deleteで解放される。
//	[追加ライブラリファイル]
//		Odbc32.lib
// *********************************************************************************
#pragma once

#include <windows.h>
#include <sql.h>
#include <sqlext.h>

#pragma warning(disable : 4996)							// ***が古い形式として宣言されました。を出さない対策


namespace KizuODBC
{
	//// 環境ハンドル(singleton)
	class OdbcEnv
	{
	private:
		SQLHENV					m_henv;											// 環境ハンドル
		CRITICAL_SECTION		m_cs;											// スレッド間の排他制御

		static OdbcEnv			m_inst;											// exeで唯一のインスタンス。 初期化時にnewされて、終了時にdeleteされる.

		OdbcEnv();																// privateコンストラクタ

	public:
		virtual ~OdbcEnv();														// デストラクタ

		// インライン
		static inline void Lock()			{ EnterCriticalSection(&m_inst.m_cs); }
		static inline void Unlock()			{ LeaveCriticalSection(&m_inst.m_cs); }
//		static inline BOOL TryLock()		{ return TryEnterCriticalSection(&m_inst.m_cs); }	// クリティカルセクション NGの場合は即時結果が返る

		static inline SQLHENV GetHandle()	{ return m_inst.m_henv; }


		// デバック調査用
	public:	
		void GetErrMsg();														// 異常発生時の詳細メッセージ取得
		SQLCHAR					g_cLastErrMsgMain[256];
		SQLCHAR					g_cLastErrMsgSub[256];
		SQLINTEGER				g_nLastErrMsgNo;
	};



	//// 環境ハンドル操作 & 排他付き制御クラス
	class OdbcLockedEnv
	{
	private:
		BOOL					m_bCsLocked;									// クリティカルセクションロック状態
			
	public:
	    
		//OdbcLockedEnv(BOOL bWait = TRUE);										// コンストラクタでlock 疵検では必要ない為、ヤメ
		OdbcLockedEnv();														// コンストラクタでlock
		virtual ~OdbcLockedEnv();												// デストラクタでunlock

		// インライン
		inline BOOL IsCsLocked() const		{ return m_bCsLocked; }
		//inline operator SQLHENV() const		{ return OdbcEnv::GetHandle(); }	// キャスト演算子をオーバロードしてそのまま使えるようにする
		inline SQLHENV GetHandle() const	{ return OdbcEnv::GetHandle(); }	// 環境ハンドル取得
	};
};


