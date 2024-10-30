// *********************************************************************************
//	ワーカースレッドを用いたスレッド基本クラス
//	[Ver]
//		Ver.01    2009/04/10  vs2005 対応
//
//	[メモ]
//		シグナル時にコールバックされる
// *********************************************************************************
#pragma once

#include <process.h>

// 半固定
#include "..\..\..\Include\KizuCommon.h"								// 疵検インクルード
#include "..\..\KizuLib\KizuLib.h"										// 疵検ライブラリ
#include "..\..\KizuLib\Class\KizuMacro.h"								// マクロ集
#include "..\..\KizuLib\Class\KizuFunction.h"							// スタティックライブラリ集
#include "..\..\KizuLibMFC\Class\LogFileManager.h"						// ログ管理クラス

// 疵検クラス インクルード
#include "..\..\KizuLib\Class\WorkThread.h"								// ワーカースレッドクラス


using namespace KizuLib;

class ThreadManager : public WorkThread
{
protected :
	const int PARAM_MAX_WAIT_NUM;										// WaitFor最大待ち数


public:
	ThreadManager(const char* pThreadName="", int nMaxWaitNum = 50);	// コンストラクタ
	virtual ~ThreadManager(void);										// デストラクタ

	void Start(int nPriority = THREAD_PRIORITY_NORMAL)					// スレッド開始
		{ 
			WorkThread::Start(MainThread, nPriority); 
		};
	bool StartLock(int nPriority = THREAD_PRIORITY_NORMAL, int nWaitTime=3000)	// 実行待機版スレッド開始
		{
			return  !! WorkThread::StartLock(MainThread, nPriority, nWaitTime); 
		};
	void Stop()							 {WorkThread::Stop(); };		// スレッド終了
	void Stop(int interval)				 {WorkThread::Stop(interval);};	// スレッド終了
	void SetLogMgr(LogFileManager* pCls) {mcls_pLog = pCls;}			// ログクラスセット

	char const*		GetThreadName()		 {return my_sThreadName.GetBuffer();}		// 自スレッド名

	// シグナル追加処理
	bool SetEventNum(int num, LPHANDLE pWait);							// イベントセット
	bool AddEventNum(HANDLE pWait);										// イベントの追加 (1コだけ)
	bool AddEventNum(int num, LPHANDLE pWait);							// イベントの追加 (配列)
	// シグナル制御
	bool ChangeWaitNum(int num);										// WaitForの待ち数を変更 (通信タスク等で使用する為)
//	int  GetDefaultWaitNum() const {return my_nWaitNum;}				// 本来のWaitForの待ち数

protected:
	static UINT WINAPI MainThread(LPVOID param);						// メインスレッド

	virtual int ThreadFirst() = 0;										// スレッド開始前処理 (スレッド実行直後(シグナルセット前)にコールバック)
	virtual int ThreadLast() = 0;										// スレッド終了前処理 (終了シグナル後にコールバック)
	virtual void ThreadWaitFor(int nDefWaitNum, int* nNowWaitNum) {};	// スレッドのWaitFor待ちの直前処理
	virtual void ThreadEvent(int nEventNo) = 0;							// スレッドイベント発生 (nEventNoは0オリジン)
	virtual int	ThreadMainLoop();										// スレッドメインループ


	// 各インスタンス
	LogFileManager*				mcls_pLog;								// LogFileManagerスレッドインスタンス

	// シグナル
	HANDLE*						my_pEvWait;								// WaitFor待ちシグナル
	int							my_nWaitNum;							// WaitFor待ち数 (Stop用のシグナルは加算してない)
	int							my_nWaitNumNow;							// 今回のWaitFor待ち数

	// その他もろもろ
	CString						my_sThreadName;							// 自スレッド名
};
