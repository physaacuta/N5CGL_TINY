// *********************************************************************************
//	スレッドキュー クラス
//	[Ver]
//		Ver.01    2007/01/30  vs2005 対応 (コンストラクタでサイズ指定に変更)
//
//	[メモ]
//		わざとポインタしか使えないようにしています。必ずインスタンスを渡しましょう。
// *********************************************************************************

#pragma once

#include <deque>
#include <windows.h>
//using namespace std;

namespace KizuLib
{
	template <class data_t> class ThreadQueue
	{
	public:
		ThreadQueue(int SemCnt = 256);										// コンストラクタ
		virtual ~ThreadQueue();												// デストラクタ (要素は解放されない)

		// 要素操作
		void Free();														// 全要素を解放（data_tのデストラクタを呼ぶ)
		BOOL AddToTail(data_t * newData);									// 要素を末尾に一つ追加
		BOOL AddToTail(data_t * newData, int retryCnt, int waitTime=1);		// 要素を末尾に一つ追加 (追加されるまで待ち)
		BOOL AddToTailFreeCheck( data_t * newData, int FreeCount=0);		// 残登録件数をチェック後 要素を末尾に一つ追加
		BOOL AddToHead(data_t * newData);									// 要素を先頭に一つ追加
																			// 使い終わった直後のものを使うことで
																			// L2キャッシュ効率が上がることを期待
		data_t * GetFromHead(DWORD waitTime);								// 先頭要素取り出し(セマフォ待ちバージョン)
		data_t * GetFromHead();												// 先頭要素取り出し(待ちなしバージョン) (セマフォが減らないから注意する事)

		// プロパティ
		BOOL IsEmpty()		const {return my_deque.empty();};				// 空かどうか
		int  GetCount()		const {return (int)my_deque.size();};			// 現在の登録件数取得
		int  GetSemCount()	const {return my_nSemCount;};					// 登録出来る最大件数
		int  GetMaxCount()	const {return my_nMaxCount;};					// 瞬間最大登録件数取得
		void ReSetMaxCount()	  { my_nMaxCount = 0; };					// 瞬間最大登録件数初期化
		int  GetFreeCount()	const {return GetSemCount() - GetCount();}		// 残登録件数
		

		// 公開変数
		HANDLE					g_evSem;									// セマフォ ハンドル
		

	protected:
		CRITICAL_SECTION		my_cs;										// クリティカルセクション
		CRITICAL_SECTION		my_csSeq;									// クリティカルセクション (一連の流れ用)
		std::deque<data_t*>		my_deque;									// キュー
		int						my_nSemCount;								// セマフォサイズ
		int						my_nMaxCount;								// 瞬間最大登録件数
	};


	// //////////////////////////////////////////////////////////////////////////////
	// テンプレートなのでヘッダファイルに入れておく必要あり
	// //////////////////////////////////////////////////////////////////////////////

	//------------------------------------------
	// コンストラクタ
	// int SemCnt セマフォのサイズ (デフォルト256)
	//------------------------------------------
	template <class data_t>
	ThreadQueue<data_t>::ThreadQueue(int SemCnt) :
	my_nMaxCount(0),
	my_nSemCount(SemCnt)
	{
		g_evSem = CreateSemaphore(NULL, 0, my_nSemCount, NULL);
		InitializeCriticalSection(&my_cs);							// クリティカルセクションオブジェクトを初期化
		InitializeCriticalSection(&my_csSeq);
	}

	//------------------------------------------
	// デストラクタ
	//------------------------------------------
	template <class data_t>
	ThreadQueue<data_t>::~ThreadQueue()
	{
		Free();
		CloseHandle(g_evSem);
		DeleteCriticalSection(&my_cs);								// クリティカルセクションオブジェクトを開放
		DeleteCriticalSection(&my_csSeq);
	}

	//------------------------------------------
	// 全要素を解放（data_tのデストラクタを呼ぶ)
	//------------------------------------------
	template <class data_t>
	void ThreadQueue<data_t>::Free()
	{
		int n = GetCount();
		data_t * p;
		for (int i = 0; i < n; i++) {
			p = GetFromHead(0);
			ASSERT(p);
			delete p;
		}
		my_nMaxCount = 0;
	}

	//------------------------------------------
	// 要素を末尾に一つ追加
	// data_t * p 登録するインスタンス
	// 戻り値 復帰情報
	//------------------------------------------
	template <class data_t>
	BOOL ThreadQueue<data_t>::AddToTail(data_t * newData)
	{
		BOOL ret;
		
		EnterCriticalSection(&my_cs);
		my_deque.push_back(newData);
		if (GetCount() > my_nMaxCount) my_nMaxCount = GetCount();
		ret = ReleaseSemaphore(g_evSem, 1, NULL);
		LeaveCriticalSection(&my_cs);
		ASSERT(ret);
		return ret;
	}

	//------------------------------------------
	// 要素を末尾に一つ追加 (追加されるまで待ち)
	// data_t * p 登録するインスタンス
	// int retryCnt リトライ回数
	// int waitTime Sleep間隔 [ms]
	// 戻り値 復帰情報
	//------------------------------------------
	template <class data_t>
	BOOL ThreadQueue<data_t>::AddToTail(data_t * newData, int retryCnt, int waitTime)
	{
		BOOL ret;
		
		EnterCriticalSection(&my_csSeq);

		for ( int ii=0; ii<retryCnt; ii++ ) {
			// 追加可能かチェック
			if (GetCount() < my_nSemCount ) {	// 追加可能
				ret = AddToTail(newData);
				LeaveCriticalSection(&my_csSeq);	
				return ret;
			} else {						// 待ち
				Sleep(waitTime);
			}
		}
		LeaveCriticalSection(&my_csSeq);
		return FALSE;
	}
	//------------------------------------------
	// 残登録件数をチェック後 要素を末尾に一つ追加
	// data_t * p 登録するインスタンス
	// int FreeCount 空き容量。この数値より大きい場合のみ登録する。数値以下の場合は、登録失敗とする
	// 戻り値 復帰情報
	//------------------------------------------
	template <class data_t>
	BOOL ThreadQueue<data_t>::AddToTailFreeCheck(data_t * newData, int FreeCount)
	{
		BOOL ret;
		
		EnterCriticalSection(&my_csSeq);
		if( FreeCount < GetFreeCount() ) {	
			ret = AddToTail(newData);
		} else {
			ret = FALSE;
		}
		LeaveCriticalSection(&my_csSeq);
		return ret;
	}


	//------------------------------------------
	// 要素を先頭に一つ追加
	// data_t * p 登録するインスタンス
	// 戻り値 復帰情報
	//------------------------------------------
	template <class data_t>
	BOOL ThreadQueue<data_t>::AddToHead(data_t * newData)
	{
		BOOL ret;
		EnterCriticalSection(&my_cs);
		my_deque.push_front(newData);
		if (GetCount() > my_nMaxCount) my_nMaxCount = GetCount();	// 最大件数更新
		ret = ReleaseSemaphore(g_evSem, 1, NULL);
		LeaveCriticalSection(&my_cs);
		ASSERT(ret);
		return ret;
	}

	//------------------------------------------
	// 先頭要素取り出し(セマフォ待ちバージョン)
	// DWORD waitTime 待ち時間 [ms]
	// 戻り値 取り出したインスタンス
	//------------------------------------------
	template <class data_t>
	data_t * ThreadQueue<data_t>::GetFromHead(DWORD waitTime)
	{
		if (WaitForSingleObject(g_evSem, waitTime) == WAIT_TIMEOUT) {
			return NULL;
		}
		return (GetFromHead());
	}

	//------------------------------------------
	// 先頭要素取り出し(待ちなしバージョン) (セマフォが減らないから注意する事)
	// 戻り値 取り出したインスタンス
	//------------------------------------------
	template <class data_t>
	data_t * ThreadQueue<data_t>::GetFromHead()
	{
		data_t * p = NULL;
		EnterCriticalSection(&my_cs);
		if (!IsEmpty()) {
			p = (data_t *)my_deque.front();
			my_deque.pop_front();
		}
		LeaveCriticalSection(&my_cs);
		return p;
	}
};
