//------------------------------------------------------------------------------
//	前工程接続サーバー管理クラス
//	[Ver]
//		Ver.01	2013/09/19	初版
//
//	[メモ]
//
//------------------------------------------------------------------------------
#pragma once

// 疵検ライブラリ
#include "..\..\Library\KizuLib\KizuLib.h"					// 疵検ライブラリ

#include "..\..\Library\KizuLib\Class\KizuMacro.h"			// マクロ集
#include "..\..\Library\KizuLib\Class\KizuFunction.h"		// スタティックライブラリ集
#include "..\..\Library\KizuLib\Class\PingManager.h"		// Ping管理クラス

#include "..\..\Library\KizuLibMFC\Class\LogFileManager.h"	// ログ管理クラス
#include "..\..\Library\KizuLibMFC\Class\CreateSQL.h"		// SQL文生成補助クラス

#include "..\..\Library\KizuODBC\OdbcBase.h"				// DB操作クラス

using namespace KizuLib;

class PreDBSelectManager
{
public:
	//------------------------------------------------------
	// 列挙体定義
	//------------------------------------------------------
	// DB選択モード
	enum EM_SELECT_DB{
		INIT_DB		= -1,									// 初期値
		NON_DB		= 0,									// 未選択
		PRE_DB		= 1,									// 蓄積DB
		SHARE_DB	= 2										// 統合DB
	};

	// 選択状態
	enum EM_SELECT_STATUS{
		SELECT_NON		= -1,								// 未実施
		SELECT_OK		=  0,								// 正常
		SELECT_NG		=  1								// 異常
	};

	//------------------------------------------------------
	// 構造体定義
	//------------------------------------------------------
	// 選択情報
	typedef struct TYP_SELECT_STATUS{
		EM_SELECT_STATUS	nPing;							// Ping結果				[-1:未実施、0:正常、1:異常]
		EM_SELECT_STATUS	nConnect;						// DB接続結果			[-1:未実施、0:正常、1:異常]
		EM_SELECT_STATUS	nSearchCoil;					// コイル実績検索結果	[-1:未実施、0:正常、1:異常]
	};

public:
	//------------------------------------------------------
	// メソッド定義
	//------------------------------------------------------
	// コンストラクタ
	PreDBSelectManager(CString strTaskSection, CString strRetrySection, CString strSqlKey);

	// デストラクタ
	~PreDBSelectManager(void);

	// ログ管理クラス設定
	void SetLogManager(LogFileManager* pLog){mcls_pLog = pLog;}

	// 前工程蓄積サーバーDBセクション設定
	void SetPreDBSection(CString strSection){m_strDBSectionPre = strSection;}

	// 統合サーバーDBセクション設定
	void SetShareDBSection(CString strSection){m_strDBSectionShare = strSection;}

	// 接続サーバー選択モード取得
	EM_SELECT_DB GetSelectMode(void){return	m_nSelectMode;}

	// 接続サーバー選択状態取得
	void GetSelectStatus(EM_SELECT_DB nSelectMode, TYP_SELECT_STATUS* typStatus);

	// 前工程接続DB選択
	bool SelectConnectDB(char* cPreKizukenNo, OdbcBase* pConnectDB);

	// 前工程接続DB選択
	bool SelectConnectDB(CString strPreDB, CString strShareDB, char* cPreKizukenNo, OdbcBase* pConnectDB);

	// 選択サーバーDB接続
	bool ConnectSelectDB(OdbcBase* pConnectDB);

	// 前工程蓄積サーバーDB接続
	bool ConnectPreDB(OdbcBase* pConnectDB);

	// 統合サーバーDB接続
	bool ConnectShareDB(OdbcBase* pConnectDB);

	// 前工程蓄積サーバーDB接続チェック
	bool CheckPreDB(void);

	// 統合サーバーDB接続チェック
	bool CheckShareDB(void);

	// DB接続チェック
	bool CheckConnectDB(CString strDBSection, EM_SELECT_DB nSelect);

    // 2014.07.25 川原  所間連携    >>>>>>>>
	// 前工程蓄積サーバー接続
	bool ConnectPreServer(CString strDBSection, char* cPreKizukenNo, OdbcBase* pConnectDB);
	// 統合サーバー接続
	bool ConnectShareServer(CString strDBSection, OdbcBase* pConnectDB);
    // 2014.07.25 川原  所間連携    <<<<<<<<

private:
	//------------------------------------------------------
	// 構造体定義
	//------------------------------------------------------
	// リトライ情報
	typedef struct TYP_RETRY_INF{
		int		nCount;										// リトライ回数
		int		nTime;										// リトライ時間[ms]
	};

private:
	//------------------------------------------------------
	// メソッド定義
	//------------------------------------------------------
	// Pingサーバー
	bool PingServer(CString strDBSection, TYP_RETRY_INF* typRetry);

	// DB接続
	bool ConnectDB(CString strDBSection, TYP_RETRY_INF* typRetry, OdbcBase* pConnectDB);

	// コイル実績検索
	bool SearchCoilResult(CString strPreKizukenNo, OdbcBase* pConnectDB);

    // 2014.07.25 川原  所間連携    >>>>>>>>
	// 前工程蓄積サーバー接続
	//bool ConnectPreServer(CString strDBSection, CString strPreKizukenNo, OdbcBase* pConnectDB);
	// 統合サーバー接続
	//bool ConnectShareServer(CString strDBSection, OdbcBase* pConnectDB);
    // 2014.07.25 川原  所間連携    <<<<<<<<


private:
	//------------------------------------------------------
	// 変数定義
	//------------------------------------------------------
	LogFileManager*	mcls_pLog;								// ログ管理クラス
	PingManager		mcls_Ping;								// Ping管理クラス

	EM_SELECT_DB	m_nSelectMode;							// 接続サーバー選択モード

	CString			m_strTaskSection;						// 自タスクセクション[KizuTask.ini]
	CString			m_strRetrySection;						// リトライセクション[KizuTask.ini]
	CString			m_strCoilResult;						// コイル実績テーブル名[COIL_RESULT]
	CString			m_strSqlKey;							// 実績検索SQLキー
	CString			m_strSqlFmt;							// 実績検索SQLフォーマット

	// 前工程蓄積サーバー
	CString				m_strDBSectionPre;					// DBセクション[KizuTask.ini]
	TYP_RETRY_INF		m_typPingRetryPre;					// Pingリトライ
	TYP_RETRY_INF		m_typConnectRetryPre;				// DB接続リトライ
	TYP_SELECT_STATUS	m_typStatusPre;						// 選択状態

	// 統合サーバー
	CString				m_strDBSectionShare;				// DBセクション[KizuTask.ini]
	TYP_RETRY_INF		m_typPingRetryShare;				// Pingリトライ
	TYP_RETRY_INF		m_typConnectRetryShare;				// DB接続リトライ
	TYP_SELECT_STATUS	m_typStatusShare;					// 選択状態
};
