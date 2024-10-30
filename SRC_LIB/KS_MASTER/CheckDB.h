// *********************************************************************************
//	DB接続確認クラス
//	[Ver]
//		Ver.01    2007/04/29  vs2005 対応
//
//	[メモ]
//		・タスク起動時にDB接続可能かチェック
// *********************************************************************************

#pragma once

// 半固定
#include "..\..\Include\KizuCommon.h"									// 疵検インクルード
#include "..\..\Library\KizuLib\KizuLib.h"								// 疵検ライブラリ
#include "..\..\Library\KizuLib\Class\KizuMacro.h"						// マクロ集
#include "..\..\Library\KizuLib\Class\KizuFunction.h"					// スタティックライブラリ集
#include "..\..\Library\KizuLibMFC\Class\LogFileManager.h"				// ログ管理クラス

// 疵検クラス インクルード
#include "..\..\Library\KizuLib\Class\WorkThread.h"						// ワーカースレッドクラス
#include "..\..\Library\KizuLib\Class\ThreadQueue.h"					// スレッドキュークラス
//#include "..\KizuLib\Class\AutoLock.h"								// クリティカルセクションクラス
#include "..\..\Library\KizuLib\Class\KizuPerformanceCounter.h"			// 時間測定クラス

#include "..\..\Library\KizuODBC\OdbcBase.h"							// DB操作クラス

using namespace KizuLib;
using namespace KizuODBC;

class CheckDB :	public WorkThread
{
public :
#define	 E_DEF_DB_FAST_CONNECT				 WM_USER+200				// PostThreadMassageのID
#define	 E_DEF_DB_NG_TO_OK					 WM_USER+201				// PostThreadMassageのID


private :
	static const int HEART_BEAT_DBCHECK_CYCLE		= 30000;			// DB状態確認周期
	static const int MAX_SEND_TASK					= 50;				// 送信タスク最大登録件数

public:
	CheckDB(bool isMaster);
	virtual ~CheckDB(void);


	void Start() { WorkThread::Start(MainThread); };					// スレッド開始
	void Stop()  { WorkThread::Stop(10000); };							// スレッド終了
	void SetLogMgr(LogFileManager* pLog) { mcls_pLog = pLog; }			// ログファイルセット

	// 外部アクセス
	bool GetStateDB()  const {return m_bStateDB; }						// DB接続可能時 true
	bool GetFastExec() const {return m_bFastExec; }						// 初期起動中 true 一度でもDBにつながればfalse
	void SetFastExecDebug()  { m_bFastExec=false; }						// デバック用


private :
	static UINT WINAPI MainThread(LPVOID param);						// メインスレッド

	void Get_IniData();													// iniファイルデータ取得
	void Check_DataBase();												// DBチェック
	bool OpenCheckEx();													// DB接続確認


	//// 各インスタンス
	LogFileManager*			mcls_pLog;									// LogFileManagerスレッドインスタンス

	// シグナル
	HANDLE					m_evDBCheck;								// DB監視タイマイベント

	// その他もろもろ
	bool					m_bIsMaster;								// PCマスター管理時true
	bool					m_bStateDB;									// DBの現在の状態
	bool					m_bFastExec;								// 初期起動中 (true:初期起動中 false:一度でもDBに繋がった)
	bool					m_bCheckDB;									// DBチェック (true:する)
	int						m_nSendTaskCnt;								// 送信タスク数
	char					m_cSendTaskName[MAX_SEND_TASK][SIZE_NAME_TASK];	// 送信タスク名称

};
