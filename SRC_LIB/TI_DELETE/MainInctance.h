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

// 疵検ライブラリ インクルード
//#include "..\..\Library\KizuSOCK3\SockBase.h"							// ソケット操作クラス
#include "..\..\Library\KizuODBC\OdbcBase.h"							// ODBC操作クラス


// 各ワーカースレッド
#include "MainConst.h"
#include "ControlManager.h"												// 削除スレッドコントロールクラス
#include "ControlLowGr.h"												// 無害Gr削除管理
#include "ControlDefragManager.h"										// インデックス管理


using namespace KizuLib;
using namespace KizuODBC;

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

	void SetWaitMode(bool mode);										// 削除/書込 優先モード (true:削除優先)



	// 各ワーカースレッド
	int						g_nClassCnt;								// インスタンスの生成数
	ControlManager*			gcls_pContMgr[CONTROL_MAXCNT];				// ControlManagerスレッドインスタンス
	ControlLowGr*			gcls_pContLowGr;							// 無害Gr削除管理
	ControlDefragManager	gcls_ContDefrag;							// インデックス管理


private:
	static UINT WINAPI MainThread(LPVOID param);						// メインスレッド
	

	//// 各インスタンス
	LogFileManager*			mcls_pLog;									// LogFileManagerスレッドインスタンス

	// 同期シグナル
	HANDLE					m_evThStart;								// MainInctanceスレッドの実行要求

	// その他もろもろ

};
