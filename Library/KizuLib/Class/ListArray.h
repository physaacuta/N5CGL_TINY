// *********************************************************************************
//	挿入可能なスレッドキュー クラス (ThreadQueueの高機能版)
//	[Ver]
//		Ver.01    2007/03/08  vs2005 対応
//
//	[メモ]
//		スレッドキューに 挿入機能を提供
// *********************************************************************************


#pragma once

#include <windows.h>

namespace KizuLib
{
	template <class data_t>	class ListArray : public ThreadQueue<data_t>
	{	
	public:
		ListArray(int SemCnt = 256);										// コンストラクタ
		virtual ~ListArray();												// デストラクタ (要素は解放されない)

		
		//// 挿入に必要な機能を提供
		// 要素にアクセスしている最中に、別スレッドがQueのアクセスをするのを禁止する為
		void LockQue()			{ EnterCriticalSection(&my_cs); }			// クリティカルセクション
		void UnlockQue()		{ LeaveCriticalSection(&my_cs); }			// 

		data_t * Array(int index);											// 要素へのアクセス

		BOOL AddToInsert(data_t * newData, int index);									// 要素を挿入する
		BOOL AddToInsert(data_t * newData, int index, int retryCnt, int waitTime=1);	// 要素を挿入する (追加されるまで待ち)

	protected:

	};

	// //////////////////////////////////////////////////////////////////////////////
	// テンプレートなのでヘッダファイルに入れておく必要あり
	// //////////////////////////////////////////////////////////////////////////////

	//------------------------------------------
	// コンストラクタ
	// int SemCnt セマフォのサイズ (デフォルト256)
	//------------------------------------------
	template <class data_t>
	ListArray<data_t>::ListArray(int SemCnt) : ThreadQueue(SemCnt)
	{
	}

	//------------------------------------------
	// デストラクタ
	//------------------------------------------
	template <class data_t>
	ListArray<data_t>::~ListArray()
	{
	}

	//------------------------------------------
	// 要素にアクセス (必ずLockしておく)
	// int index 先頭からの配列位置 (0オリジン)
	// 戻り値：要素
	//------------------------------------------
	template <class data_t>
	data_t * ListArray<data_t>::Array(int index)
	{
		// 引数チェック
		if( 0 > index )					return NULL;
		if( this->GetCount() <= index )	return NULL;
 
		// 要素を返す
		return my_deque[index];
	}

	//------------------------------------------
	// 要素を挿入する
	// data_t * newData 挿入するインスタンス
	// int index 先頭からの挿入する配列位置 (0オリジン) ※ 1を指定したら[1]に挿入。[1]に入っていたのは[2]に移動
	// 戻り値：復帰情報
	//------------------------------------------
	template <class data_t>
	BOOL ListArray<data_t>::AddToInsert(data_t * newData, int index)
	{
		// 引数チェック
		if( 0 > index )					return FALSE;
		if( this->GetCount() < index )	return FALSE;

		// 挿入するイレテータを作り出す
		std::deque<data_t*>::iterator it = my_deque.begin(); 
		it += index;

		// 要素を挿入
		BOOL ret;

		EnterCriticalSection(&my_cs);
		my_deque.insert(it, newData);
		if (GetCount() > my_nMaxCount) my_nMaxCount = GetCount();
		ret = ReleaseSemaphore(g_evSem, 1, NULL);
		LeaveCriticalSection(&my_cs);
		ASSERT(ret);
		return ret;
	}

	//------------------------------------------
	// 要素を挿入する (追加されるまで待ち)
	// data_t * p 挿入するインスタンス
	// int index 先頭からの挿入する配列位置 (0オリジン) ※ 1を指定したら[1]に挿入。[1]に入っていたのは[2]に移動
	// int retryCnt リトライ回数
	// int waitTime Sleep間隔 [ms]
	// 戻り値 復帰情報
	//------------------------------------------
	template <class data_t>
	BOOL ListArray<data_t>::AddToInsert(data_t * newData, int index, int retryCnt, int waitTime)
	{
		BOOL ret;
		
		EnterCriticalSection(&my_csSeq);

		for ( int ii=0; ii<retryCnt; ii++ ) {
			// 追加可能かチェック
			if (GetCount() < my_nSemCount ) {	// 追加可能
				ret = AddToInsert(newData, index);
				LeaveCriticalSection(&my_csSeq);	
				return ret;
			} else {						// 待ち
				Sleep(waitTime);
			}
		}
		LeaveCriticalSection(&my_csSeq);
		return FALSE;
	}
};