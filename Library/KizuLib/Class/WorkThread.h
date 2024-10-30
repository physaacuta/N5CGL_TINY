// *********************************************************************************
//	スレッド クラス
//	[Ver]
//		Ver.01    2007/01/30  vs2005 対応
//
//	[メモ]
//
// *********************************************************************************

//#ifndef KIZULIB_WORKTHREAD_H
//#define KIZULIB_WORKTHREAD_H

#pragma once

#include <windows.h>

namespace KizuLib
{
	class WorkThread
	{
	public:
		WorkThread(DWORD postTid = 0);						// コンストラクタ
		virtual ~WorkThread(void);							// デストラクタ


		BOOL Start(UINT (__stdcall threadFunc(void*)), int nPriority = THREAD_PRIORITY_NORMAL);		// スレッド実行 (p有り)
		BOOL Start(int nPriority = THREAD_PRIORITY_NORMAL);											// スレッド実行 (p無し)
		void Stop();																		// スレッド停止
		void Stop(int interval);															// スレッド停止
		void StopMSG(int msgNo, int interval = 10000);										// スレッド停止 (PostThreadMessageで終了通知)

		void StopSignal() { SetEvent(my_evStop); };											// スレッド停止シグナルのみ実行


		// プロパティ
		bool  IsThreadRun();								// スレッド実行中かチェック (実行中true)
		bool  IsThreadEvOn() { return my_bThreadEvOn;}		// Start発行時 true, Stop発行時 false

		DWORD GetTid() const { return my_Tid;}				// 自スレッドID
		void  SetTid() { my_Tid = GetCurrentThreadId(); }	// 自スレッドIDセット
		static void  SetThreadName(const char *name, DWORD tid=-1);	// VCデバッガウインドウの、「スレッド名」を変更する

		DWORD GetMainPostTid() const { return my_PostTid;}	// PostThreadMessageで通知する先
		void  SetMainPostTid(int tid) {my_PostTid = tid; }	// PostThreadMessageで通知する先 (よくMainInctanceにメッセージを飛ばす時に使用する)	


	protected :
	
		virtual UINT MainThread(void);// = 0;				// スレッドを実行するメンバ関数 (継承先に同一名の関数が必要)
		static  UINT WINAPI m_thread_entry(LPVOID p);		// AfxBeginThreadに渡す関数


		//CWinThread* my_pThread;							// スレッドインスタンス
		HANDLE		my_hThread;								// スレッドハンドル
		HANDLE		my_evStop;								// 終了シグナル
		DWORD		my_Tid;									// 自スレッドID
		DWORD		my_PostTid;								// PostThreadMessageで通知する先 (よくMainInctanceにメッセージを飛ばす時に使用する)	
		bool		my_bThreadEvOn;							// Start発行時 true, Stop発行時 false
	



		// メインスレッド実行待機
	public:
		BOOL StartLock(UINT (__stdcall threadFunc(void*)), int nPriority = THREAD_PRIORITY_NORMAL, int nWaitTime=3000);		// 実行待ち版スレッド実行 (p有り)
		BOOL StartLock(int nPriority = THREAD_PRIORITY_NORMAL, int nWaitTime=3000);		// 実行待ち版スレッド実行 (p無し)
	protected:
		HANDLE		my_hThreadReady;						// ホントにスレッドが実行するまで待機用

	};
};
//#endif