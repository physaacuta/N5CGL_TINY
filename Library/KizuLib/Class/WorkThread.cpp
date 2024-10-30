#include "Stdafx.h"
#include "WorkThread.h"

#include <process.h>

using namespace KizuLib;

/*
//------------------------------------------
// コンストラクタ
//------------------------------------------
WorkThread::WorkThread(void) :
my_pThread(NULL),
my_Tid(0),
my_PostTid(0)
{
	my_evStop = CreateEvent(NULL, FALSE, FALSE, NULL);
}
*/
//------------------------------------------
// コンストラクタ
// DWORD postTid PostThreadMessageで通知する先 (よくMainInctanceにメッセージを飛ばす時に使用する)	
//------------------------------------------
WorkThread::WorkThread(DWORD postTid) :
my_hThread(NULL),
my_hThreadReady(INVALID_HANDLE_VALUE),
my_Tid(0),
my_bThreadEvOn(false),
my_PostTid(postTid)
{
	my_evStop = CreateEvent(NULL, FALSE, FALSE, NULL);
}

//------------------------------------------
// デストラクタ
//------------------------------------------
WorkThread::~WorkThread(void)
{
	CloseHandle(my_evStop);
}

//------------------------------------------
// スレッドが実行中かチェック (Stopモジュールをコールしてなくても正確に判断可能)
// 戻り値 : true:実行中  false:スレッドが動いていない
//------------------------------------------
bool WorkThread::IsThreadRun()
{
	//// そもそも スレッドハンドルがある？
	if( NULL==my_hThread ) return false;
	
	//// 実行中かチェック
	// my_hThread のNULLだけでチェックすると、Stop()をコールせずに、直にmy_evStopシグナルを発行した場合、正常に把握できない
	if( WAIT_TIMEOUT == WaitForSingleObject(my_hThread, 0))	return true;	// タイムアウト = スレッドが動いている
	else													return false;
}

//------------------------------------------
// スレッド実行 (p有り)
// UINT (__stdcall threadFunc(void*)) フレンドメソッドのアドレス
// int nPriority スレッド優先度
//------------------------------------------
BOOL WorkThread::Start(UINT (__stdcall threadFunc(void*)), int nPriority)
{
	ResetEvent(my_evStop);
	unsigned int tid;
	my_hThread = (HANDLE)_beginthreadex(NULL, 0, threadFunc, (LPVOID)this, 0, &tid);
	if(NULL == my_hThread) return FALSE;
	my_bThreadEvOn = true;

	// スレッド優先度指定
	SetThreadPriority(my_hThread, nPriority);
	return TRUE;
}

//------------------------------------------
// スレッド実行 (p無し)
// int nPriority スレッド優先度
//------------------------------------------
BOOL WorkThread::Start(int nPriority)
{
	ResetEvent(my_evStop);
	unsigned int tid;
	my_hThread = (HANDLE)_beginthreadex(NULL, 0, m_thread_entry, (LPVOID)this, 0, &tid);
	if(NULL == my_hThread) return FALSE;
	my_bThreadEvOn = true;

	// スレッド優先度指定
	SetThreadPriority(my_hThread, nPriority);
	return TRUE;
}




//------------------------------------------
// スレッド実行 (p有り)
// UINT (__stdcall threadFunc(void*)) フレンドメソッドのアドレス
// int nPriority スレッド優先度
// int nWaitTime 最大待ち時間
//------------------------------------------
BOOL WorkThread::StartLock(UINT (__stdcall threadFunc(void*)), int nPriority, int nWaitTime)
{
	// 実行待ちシグナル
	my_hThreadReady = CreateEvent(NULL, FALSE, FALSE, NULL);

	// 通常のスレッド機能
	if( ! Start(threadFunc, nPriority) ) {
		CloseHandle(my_hThreadReady);	my_hThreadReady=INVALID_HANDLE_VALUE;
		return FALSE;
	}

	// 待機
    DWORD wait = WaitForSingleObject(my_hThreadReady, nWaitTime);
	BOOL ret = WAIT_OBJECT_0 == wait ? TRUE : FALSE;
	CloseHandle(my_hThreadReady);	my_hThreadReady=INVALID_HANDLE_VALUE;
	return ret;
}
//------------------------------------------
// 実行待ち版スレッド実行 (p無し)
// int nPriority スレッド優先度
// int nWaitTime 最大待ち時間
//------------------------------------------
BOOL WorkThread::StartLock(int nPriority, int nWaitTime)
{
	// 実行待ちシグナル
	my_hThreadReady = CreateEvent(NULL, FALSE, FALSE, NULL);

	// 通常のスレッド機能
	if( ! Start(nPriority) ) {
		CloseHandle(my_hThreadReady);	my_hThreadReady=INVALID_HANDLE_VALUE;
		return FALSE;
	}

	// 待機
    DWORD wait = WaitForSingleObject(my_hThreadReady, nWaitTime);
	BOOL ret = WAIT_OBJECT_0 == wait ? TRUE : FALSE;
	CloseHandle(my_hThreadReady);	my_hThreadReady=INVALID_HANDLE_VALUE;
	return ret;
}





//------------------------------------------
// p無し用 スレッドに渡す関数
// int nPriority スレッド優先度
//------------------------------------------
UINT WINAPI WorkThread::m_thread_entry(LPVOID parg)
{
    WorkThread* p = (WorkThread*)parg;
 	//return p->MainThread();			// 継承先の実体を実行
	UINT ret = p->MainThread();			// 継承先の実体を実行
	
	// スレッド実行完了
	if (INVALID_HANDLE_VALUE != p->my_hThreadReady) { SetEvent(p->my_hThreadReady); }		// 実行待機版

	return ret;
}

//------------------------------------------
// スレッド (継承先で実装する事)
//------------------------------------------
UINT WorkThread::MainThread()
{
	// これで例外となる場合は、StartLock未対応なので、Start()に変更すること。
	throw;				// 例外が発生するように変更。p有り版昔ライブラリを安易にStartLockにするとダメなので
	return 0;
}

//------------------------------------------
// スレッド停止
// int interval 強制終了までの待ち時間 [ms]
//------------------------------------------
void WorkThread::Stop(int interval)
{
	if (my_hThread == NULL) return;		// スレッド無し
	SetEvent(my_evStop);				// 停止シグナル
	my_bThreadEvOn = false;
	if (WAIT_TIMEOUT == WaitForSingleObject(my_hThread, interval)) {
		// 異常の為、強制的にスレッド終了
		TerminateThread(my_hThread, -1);
	} else {
		// 正常の為、ハンドル解放
		CloseHandle(my_hThread);
	}
	my_hThread = NULL;
}

//------------------------------------------
// スレッド停止
//------------------------------------------
void WorkThread::Stop()
{
	// デフォルト待ち時間
	Stop(10000);
}

//------------------------------------------
// スレッド停止 (PostThreadMessageで終了通知)
// int msgNo 通知メッセージNo
// int interval = 10000 強制終了までの待ち時間 [ms]
//------------------------------------------
void WorkThread::StopMSG(int msgNo, int interval)
{
	// デフォルト待ち時間
	PostThreadMessage(my_Tid, msgNo, 0, 0);
	Stop(interval);
}

//------------------------------------------
// VCデバッガウインドウの、「スレッド名」を変更する
// const char *name デバッカに表示する名称
// DWORD tid 対象スレッドID 自分なら-1
//------------------------------------------
void WorkThread::SetThreadName(const char *name, DWORD tid)
{
	struct ThreadName{
		DWORD dwType;
		LPCSTR szName;
		DWORD dwThreadID;
		DWORD dwFlags;
	};
	ThreadName threadname = { 0x1000, name, tid, 0 };
	__try{
		RaiseException( 0x406d1388, 0, sizeof( threadname ) / sizeof(DWORD) , (ULONG_PTR*)&threadname );
	}__except( EXCEPTION_CONTINUE_EXECUTION )
	{}
}