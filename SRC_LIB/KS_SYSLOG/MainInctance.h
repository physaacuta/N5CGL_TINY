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
#include "..\..\Library\KizuLib\CLASS\ThreadQueue.h"					// スレッドキュー

// 疵検ライブラリ インクルード
#include "..\..\Library\KizuODBC\OdbcBase.h"							// ODBC操作クラス

// 各ワーカースレッド
#include "..\..\Library\DefectLib\DBSaveManager.h"						// DB書込み基本クラス


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
	DBSaveManager* gcls_pDb;											// DB書込みクラス

	// 外部アクセス
	void AddSysLogSqlQue(COMMON_QUE const* que);						// Syslogをスレッドキューに登録
	void AddStateLogSqlQue(COMMON_QUE const* que);						// Statelogをスレッドキューに登録

	void SetSyslogDsp(bool flg) {m_bSyslogDsp = flg; }					// シスログ画面表示有無

private:
	static UINT WINAPI MainThread(LPVOID param);						// メインスレッド

	void InitAutoNo();													// AutoNoに必要なデータを初期セットする
	void RefrashAutoNo(int nDay);										// 日付をチェックして、日が変われば、オートNoをリセットする。


	//// スレッドキュー
	ThreadQueue<DBSaveManager::DeliverySQL> mque_AddDB;					// 受け渡しデータ用キュー (ここに入ったキューは全て登録する)

	//// 各インスタンス
	LogFileManager*			mcls_pLog;									// LogFileManagerスレッドインスタンス
	
	// 同期シグナル
	HANDLE					m_evThStart;								// MainInctanceスレッドの実行要求

	// その他もろもろ
	bool					m_bSyslogDsp;								// シスログ画面表示有無

	// 主キー関連
	//  ・主キー１: 登録日付
	//  ・主キー２: m_nAutoNo*10 + m_nODBCID
	int						m_nDay;										// 現在日
	int						m_nAutoNo;									// 主キーの一つとなる連番
																			//・基本的には、日が変わるタイミングでリセット(1)する
																			//・立ち上げたタイミングでは、最新日付のAutoNoの値とする
	int						m_nOdbcID;									// ODBCの設定位置

};
