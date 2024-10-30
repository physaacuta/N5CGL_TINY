// *********************************************************************************
//	器固定のスレッドキュー クラス
//	[Ver]
//		Ver.01    2007/01/30  vs2005 対応 (コンストラクタでサイズ指定に変更)
//
//	[メモ]
//
// *********************************************************************************

#if !defined(AFX_THREADTYPEDQUEUE_H__991FE91A_A838_40BC_B0FA_629EB6E83831__INCLUDED_)
#define AFX_THREADTYPEDQUEUE_H__991FE91A_A838_40BC_B0FA_629EB6E83831__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

namespace KizuLib
{
	template <class data_t> 
	class ThreadTypedQueue : public CTypedPtrList< CPtrList, data_t *>
	{
	public:
		ThreadTypedQueue(int SemCnt=256);									// コンストラクタ
		virtual ~ThreadTypedQueue();										// デストラクタ (要素は解放されない)
		
		
		// 要素操作
		BOOL Alloc(int n);													// エリア確保 (キューのクラスにAllocメソッドを実装しておく事)
		void Free();														// エリア開放 (キューのクラスにFreeメソッドを実装しておく事)
		BOOL AddToTail( data_t * p);										// 要素を末尾に一つ追加
		data_t * GetFromHead(DWORD waitTime);								// 先頭要素取り出し(セマフォ待ちバージョン)
		data_t * GetFromHead();												// 先頭要素取り出し(待ちなしバージョン) (セマフォが減らないから注意する事)
		

		// プロパティ
		BOOL IsEmpty()		const {return CPtrList::IsEmpty();};			// 空かどうか
		int GetCount()		const {return (int)CPtrList::GetCount();};		// 現在の登録件数
		int GetSemCount()	const {return my_nSemCount;};					// 登録出来る最大件数

		// 公開変数
		HANDLE					g_evSem;									// セマフォ ハンドル

	protected:
		CRITICAL_SECTION		my_cs;										// クリティカルセクション
		int						my_nSemCount;								// セマフォサイズ

	};

	// //////////////////////////////////////////////////////////////////////////////
	// テンプレートなのでヘッダファイルに入れておく必要あり
	// //////////////////////////////////////////////////////////////////////////////

	//------------------------------------------
	// コンストラクタ
	// int SemCnt セマフォのサイズ (デフォルト256)
	//------------------------------------------
	template <class data_t>
	ThreadTypedQueue<data_t>::ThreadTypedQueue(int SemCnt) :
	my_nSemCount(SemCnt)
	{
		// インスタンス生成
		g_evSem = CreateSemaphore(NULL, 0, my_nSemCount, NULL);
		InitializeCriticalSection(&my_cs);							// クリティカルセクションオブジェクトを初期化

	}
	//------------------------------------------
	// デストラクタ
	//------------------------------------------
	template <class data_t>
	ThreadTypedQueue<data_t>::~ThreadTypedQueue()
	{
		CloseHandle(g_evSem);
		DeleteCriticalSection(&my_cs);								// クリティカルセクションオブジェクトを開放
	}

	//------------------------------------------
	// エリア確保
	// int n 確保する配列数
	//------------------------------------------
	template <class data_t>
	BOOL ThreadTypedQueue<data_t>::Alloc(int n)
	{
		//// 引数セット & 初期化
		data_t *p;

		// 空の器用意
		for (int ii = 0; ii < n; ii++) {
			p = new data_t;
			p->Alloc();							// 生成したクラスのメッソドを実行 (遅延バインディング)
			AddToTail(p);		// 器を追加
		}
		return TRUE;
	}
	//------------------------------------------
	// エリア開放
	//------------------------------------------
	template <class data_t>
	void ThreadTypedQueue<data_t>::Free()
	{
		// キュー数 取得
		int n = GetCount();

		for (int ii = 0; ii < n; ii++) {
			data_t *p = GetFromHead(0);			// セマフォも含めてクリア
			ASSERT(p);
			p->Free();							// 取り出したクラスのメッソドを実行 (遅延バインディング)
			delete p;
		}
	}

	//------------------------------------------
	// 要素を末尾に一つ追加
	// data_t * p 登録するキュー
	// 戻り値 復帰情報
	//------------------------------------------
	template <class data_t>
	BOOL ThreadTypedQueue<data_t>::AddToTail(data_t * p)
	{
		BOOL ret;
		EnterCriticalSection(&my_cs);
		ret = ReleaseSemaphore(g_evSem, 1, NULL);
		AddTail(p);
		LeaveCriticalSection(&my_cs);
		ASSERT(ret);
		return ret;
	}

	//------------------------------------------
	// 先頭要素取り出し(セマフォ待ちバージョン)
	// DWORD waitTime 待ち時間
	// 戻り値 取り出したインスタンス
	//------------------------------------------
	template <class data_t>
	data_t * ThreadTypedQueue<data_t>::GetFromHead(DWORD waitTime)
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
	data_t * ThreadTypedQueue<data_t>::GetFromHead()
	{
		data_t *p = NULL;

		EnterCriticalSection(&my_cs);
		if (!IsEmpty()) {
			p = RemoveHead();
		}
		LeaveCriticalSection(&my_cs);

		return p;
	}
};
#endif // !defined(AFX_THREADTYPEDQUEUE_H__991FE91A_A838_40BC_B0FA_629EB6E83831__INCLUDED_)
