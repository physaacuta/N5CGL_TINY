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
#include "CommandRecvre.h"
#include "DataSender.h"
#include "PLGReceive.h"
#include "EdgeReceive.h"

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
	CommandRecvre*	gcls_pCommand;										// コマンド受信
	DataSender*	gcls_pData;												// データ送信
	PLGReceive* gcls_pPlg;												// 速度UDP受信
	EdgeReceive* gcls_pEdge;											// エッジ位置情報UDP受信


private:
	static UINT WINAPI MainThread(LPVOID param);						// メインスレッド




	//// 各インスタンス
	LogFileManager*		mcls_pLog;										// LogFileManagerスレッドインスタンス
	
	// 同期シグナル
	HANDLE	m_evThStart;												// MainInctanceスレッドの実行要求

	// その他もろもろ
	//BYTE *m_wk;

};
