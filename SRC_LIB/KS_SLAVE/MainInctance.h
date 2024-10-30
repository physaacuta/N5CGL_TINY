// *********************************************************************************
//	メイン管理スレッド
//	[Ver]
//		Ver.01    2007/01/30  vs2005 対応
//
//	[メモ]
//		
// *********************************************************************************

#pragma once

// インクルード
#include <process.h>

// 疵検 インクルード
#include "..\..\include\KizuCommon.h"									// 疵検共通ヘッダー
#include "..\..\Library\KizuLib\KizuLib.h"								// 疵検ライブラリヘッダー

// 疵検クラス インクルード
#include "..\..\Library\KizuLib\CLASS\KizuMacro.h"						// 疵検マクロ
#include "..\..\Library\KizuLib\CLASS\WorkThread.h"						// ワーカースレッド
#include "..\..\Library\KizuLibMFC\CLASS\LogFileManager.h"				// ログマネージャー

#include "..\..\Library\KizuLib\CLASS\SmemManager.h"					// 共有メモリ
#include "..\..\Library\KizuLib\CLASS\ProcessManager.h"					// PC・タスク制御

// 各ワーカースレッド
#include "MainConst.h"
#include "..\KS_MASTER\CheckDB.h"										// DB接続確認クラス
#include "..\KS_MASTER\CheckStatus.h"									// 機器状態管理クラス

using namespace KizuLib;


class MainInctance : public WorkThread
{
private:
	// タイマー
	static const int PARAM_TIMEID_CHECK			= 10;					// 定周期実行タイマーID
	static const int PARAM_TIMEOUT_CHECK		= 3000;					// 定周期実行周期
	static const int PARAM_TIMEOUT_CHECK2		= 5000;					// 定周期実行周期 2回目の通知

public:
	MainInctance(void);
	virtual ~MainInctance(void);

	void Start() { SetEvent(m_evThStart); };							// スレッド開始
	void Stop()  { WorkThread::StopMSG(WM_USER_TH_END); };				// スレッド終了
	BOOL Init();														// 初期化処理
	void Exit();														// 終了処理
	void SetLogMgr(LogFileManager* pLog) { mcls_pLog = pLog; }			// ログファイルセット

	// 各ワーカースレッド
	CheckStatus*			gcls_pCSta;									// 機器状態管理スレッド
	CheckDB*				gcls_pCDb;									// データベース管理スレッド

	// 外部アクセス
	// Set
	void SetStateMaster(bool mode) { m_bStateMaster=mode; }
	// Get
	bool GetStateMaster() const { return m_bStateMaster; }
	bool GetDebug() const { return (0==m_nDebug ? false : true); }

private:
	static UINT WINAPI MainThread(LPVOID param);						// メインスレッド
	
	void Init_Process();												// プロセス スタート
	void Exit_Process();												// プロセス エンド
//	void ExecFileCopy();												// ファイルコピー実行
	void Send_MASTER_OK();												// 統括へ 立ち上げ完了通知 アンサー

	void Exec_Cmd(char const* cIniKey);									// コマンド実行



	//// 各インスタンス
	LogFileManager*			mcls_pLog;									// LogFileManagerスレッドインスタンス
	KizuLib::SmemManager	mcls_Smem;									// SmemManagerインスタンス

	// 同期シグナル
	HANDLE					m_evThStart;								// MainInctanceスレッドの実行要求

	// その他もろもろ
	int						m_bInitalize;								// 初期化完了時true
	bool					m_bStateMaster;								// PCマスター管理OK時 treu
	int						m_nDebug;									// デバック時 (0：本番用 1:デバック用(マスター確認無し)  2:マスター確認+DB確認も無し)
};
