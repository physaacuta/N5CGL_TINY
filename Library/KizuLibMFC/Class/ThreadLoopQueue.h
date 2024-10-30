// *********************************************************************************
//	ThreadLoopQueueを使用した 回転すし形式のスレッドキュー クラス
//	[Ver]
//		Ver.01    2007/01/30  vs2005 対応 (コンストラクタでサイズ指定に変更)
//		Ver.02	  2010/01/22  ThreadTypedQueue.hを使わないようにした
//
//	[メモ]
//
// *********************************************************************************

#pragma once

#include "..\..\KizuLib\Class\ThreadQueue.h"								// スレッドキュー クラス

namespace KizuLib
{
	//------------------------------------------
	// 器確保用のインターフェース
	//------------------------------------------
	interface IThreadLoopItem
	{
	public:
		virtual	void Alloc(int const* p=0) = 0;								// 器の生成
		virtual	void Free() = 0;											// 器の解放
	};


	//------------------------------------------
	// 回転すし形式のスレッドキュー クラス
	//------------------------------------------
	template <class data_t> 
	class ThreadLoopQueue
	{
	public:
		ThreadLoopQueue(int SemCnt=256);									// コンストラクタ
		virtual ~ThreadLoopQueue();											// デストラクタ (要素は解放されない)

		// 器 操作
		BOOL QueueAlloc(int n, int const* p=0);								// 空の器生成 (キューのクラスにAllocメソッドを実装しておく事)
		void QueueFree();													// 器を開放   (キューのクラスにFreeメソッドを実装しておく事)
		
		// 要素操作
		BOOL AddFree( data_t * p) {return mque_Free.AddToTail(p);};						// 食べ終わった空の器を返却
//		BOOL AddFree( data_t * p) {return mque_Free.AddToHead(p);};						// 食べ終わった空の器を返却
		BOOL AddPool( data_t * p) {return mque_Pool.AddToTail(p);};						// 具をセットした具有器を登録	
		data_t * GetFree(DWORD waitTime) {return mque_Free.GetFromHead(waitTime);};		// 空の器を取得 (セマフォ待ちバージョン)
		data_t * GetFree() {return mque_Free.GetFromHead();};							// 空の器を取得 (待ちなしバージョン)
		data_t * GetPool() {return mque_Pool.GetFromHead();};							// 具有器を取得 (イベント待ち無しバージョンなので WaitForMultipleObjects でセマフォを減算させておく必要あり)

		// プロパティ
		int GetSemCount()  const {return mque_Free.GetSemCount();}			// 登録出来る最大件数

		int GetCountFree() const {return (int)mque_Free.GetCount();};		// 現在の空の器数
		int GetCountPool() const {return (int)mque_Pool.GetCount();};		// 現在の具有器数
		int GetRunningCount() const { return mque_Free.GetSemCount() - (mque_Free.GetCount() + mque_Pool.GetCount()); }	// 現在 どっかで取り出して使っている器の数
		int GetMaxCountPool()	const {return mque_Pool.GetMaxCount();};	// 瞬間最大登録件数取得
		void ReSetMaxCountPool()	  { mque_Pool.ReSetMaxCount(); };		// 瞬間最大登録件数初期化


		// 排他制御用 ※外部で制御するため、扱いは十分注意が必要
		void EnterCS() { EnterCriticalSection(&m_csSeq); }
		void LeaveCS() { LeaveCriticalSection(&m_csSeq); }


		// 公開変数
		HANDLE					g_evSemFree;								// 空の器キューのセマフォ ハンドル
		HANDLE					g_evSemPool;								// 具有器キューのセマフォ ハンドル


	private:
		// 各インスタンス
		ThreadQueue<data_t>		mque_Free;									// 空の器
		ThreadQueue<data_t>		mque_Pool;									// 具有器

		CRITICAL_SECTION		m_csSeq;									// クリティカルセクション (一連の流れ用)
	};


	// //////////////////////////////////////////////////////////////////////////////
	// テンプレートなのでヘッダファイルに入れておく必要あり
	// //////////////////////////////////////////////////////////////////////////////

	//------------------------------------------
	// コンストラクタ
	// int SemCnt セマフォのサイズ (デフォルト256)
	//------------------------------------------
	template <class data_t>
	ThreadLoopQueue<data_t>::ThreadLoopQueue(int SemCnt) :
	mque_Free(SemCnt),
	mque_Pool(SemCnt)
	{
		g_evSemFree = mque_Free.g_evSem;
		g_evSemPool = mque_Pool.g_evSem;

		InitializeCriticalSection(&m_csSeq);
	}

	//------------------------------------------
	// デストラクタ
	//------------------------------------------
	template <class data_t>
	ThreadLoopQueue<data_t>::~ThreadLoopQueue()
	{
		DeleteCriticalSection(&m_csSeq);
	}

	//------------------------------------------
	// 空の器生成 (キューのクラスにAllocメソッドを実装しておく事)
	// int n 確保する配列数
	// int const* p 付帯する情報
	// 戻り値 復帰情報
	//------------------------------------------
	template <class data_t>
	BOOL ThreadLoopQueue<data_t>::QueueAlloc(int n, int const* p)
	{	
		data_t *pNewData;

		if(n > mque_Free.GetSemCount() ) return FALSE;

		//// 空の器を作成
		for (int ii = 0; ii < n; ii++) {
			pNewData = new data_t;
			((IThreadLoopItem*)pNewData)->Alloc(p);	// 生成したクラスのメッソドを実行 (遅延バインディング)
			mque_Free.AddToTail(pNewData);			// 器を追加
		}
		return TRUE;
	}

	//------------------------------------------
	// 器を開放 (キューのクラスにFreeメソッドを実装しておく事)
	//------------------------------------------
	template <class data_t>
	void ThreadLoopQueue<data_t>::QueueFree()
	{
		data_t *p = NULL;
		int		n = 0;

		//// 器を全て開放
		n = mque_Pool.GetCount();
		for (int ii = 0; ii < n; ii++) {
			p = mque_Pool.GetFromHead(0);
			ASSERT(p);
			((IThreadLoopItem*)p)->Free();	// 生成したクラスのメッソドを実行 (遅延バインディング)
			delete p;
			p = NULL;
		}

		n = mque_Free.GetCount();
		for (int ii = 0; ii < n; ii++) {
			p = mque_Free.GetFromHead(0);
			ASSERT(p);
			((IThreadLoopItem*)p)->Free();	// 生成したクラスのメッソドを実行 (遅延バインディング)
			delete p;
			p = NULL;
		}
	}
};
