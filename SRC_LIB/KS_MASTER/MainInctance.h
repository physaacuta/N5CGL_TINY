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
#include "CheckDB.h"													// DB接続確認クラス
#include "CheckStatus.h"												// 機器状態管理クラス

using namespace KizuLib;


class MainInctance : public WorkThread
{
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
	void Send_MASTER_OK(char const * cPc);								// 他PCへ PCマスター管理立ち上げ完了通知
	void Send_MASTER_TIME();											// 全PCへ 時刻変更通知

private:
	static UINT WINAPI MainThread(LPVOID param);						// メインスレッド
	
	void Init_Process();												// プロセス スタート
	void Exit_Process();												// プロセス エンド

	void Exec_Cmd(char const* cIniKey);									// コマンド実行


	//// 各インスタンス
	LogFileManager*			mcls_pLog;									// LogFileManagerスレッドインスタンス
	KizuLib::SmemManager	mcls_Smem;									// SmemManagerインスタンス

	// 同期シグナル
	HANDLE					m_evThStart;								// MainInctanceスレッドの実行要求

	// その他もろもろ
	int						m_bInitalize;								// 初期化完了時true
};
