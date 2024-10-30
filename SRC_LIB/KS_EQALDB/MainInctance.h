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
#include <winsock2.h>
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
//#include "..\..\Library\KizuODBC\OdbcBase.h"							// ODBC操作クラス
//#include "..\..\Library\Kizu232C\Rs232cBase.h"							// 232C操作クラス

#include "..\..\Library\DefectLib\BaseTcpIp.h"							// TCP/IP操作クラス

// 各ワーカースレッド
#include "SyncManager.h"
#include "MoveManager.h"
#include "CopyManager.h"
#include "SyncopyManager.h"

using namespace KizuLib;

class MainInctance : public WorkThread
{
private:

public:
	enum EM_TYPE_CLASS {
		TYPE_DB_SYNC	= 0,											// マスター同期
		TYPE_DB_COPY,													// 実績追加コピー
		TYPE_DB_MOVE,													// 実績移動
		TYPE_DB_SYNCOPY	,												// 実績完全同期
		TYPE_DB_END
	};



public:
	MainInctance(void);
	virtual ~MainInctance(void);

	void Start() { SetEvent(m_evThStart); };							// スレッド開始
	void Stop()  { WorkThread::StopMSG(WM_USER_TH_END); };				// スレッド終了
	BOOL Init();														// 初期化処理
	void Exit();														// 終了処理
	void SetLogMgr(LogFileManager* pLog) { mcls_pLog = pLog; }			// ログファイルセット


	void AddLinkServer(char const* cKey);								// リンクサーバーを設定
	void SelectNowExec(int nAtoB, int nMailKey);						// 即時実行を振り分け

	// 各ワーカースレッド
	int					g_nClassCnt[TYPE_DB_END];							// 各インスタンスの生成数
	KizuSyncroManager*	gcls_pDb[TYPE_DB_END][KizuSyncroManager::PARAM_MAX_THREAD];	// 各クラスの参照用

	SyncManager*		gcls_pSync[KizuSyncroManager::PARAM_MAX_THREAD];	// マスター同期クラス
	CopyManager*		gcls_pCopy[KizuSyncroManager::PARAM_MAX_THREAD];	// 実績追加コピークラス		
	MoveManager*		gcls_pMove[KizuSyncroManager::PARAM_MAX_THREAD];	// 実績移動クラス
	SyncopyManager*		gcls_pSynCopy[KizuSyncroManager::PARAM_MAX_THREAD];	// 実績同期クラス



private:
	static UINT WINAPI MainThread(LPVOID param);						// メインスレッド

	//// 各インスタンス
	LogFileManager*		mcls_pLog;										// LogFileManagerスレッドインスタンス
		

	// 同期シグナル
	HANDLE	m_evThStart;												// MainInctanceスレッドの実行要求

	// その他もろもろ
};
