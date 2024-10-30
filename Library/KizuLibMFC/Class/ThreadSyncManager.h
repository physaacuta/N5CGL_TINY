// *********************************************************************************
//	並列スレッド間順序保障制御クラス(順序を保障するスレッドキューみたいなもの)
//	[Ver]
//		Ver.01    2009/12/15  vs2005 対応
//		Ver.02	  2011/11/10  多段並列処理に対応
//
//	[メモ]
//		・このクラスは、たぶんそのままインスタンスを作る。もしくは、継承して並列処理のワーカースレッドも一緒に管理するようにする。
//		・で実際にワーカースレッドで、通常のThreadManagerで作って、ThreadEventがコールされた後で以下のようになる。
//
//	[サンプル]
//		<前提>
//		・Aクラス が 画像取り込みクラス
//		・Bクラス が 画像処理をする並列スレッド
//		・Cクラス が 画像処理完了後の結果判定クラス
//		・各.hに、ThreadSyncManagerのインスタンス mcls を保持
//		・要素が p
//		とした場合
//
//
//		<Aクラスでは>
//			mcls->AddToTail(p)
//
//		<Bクラスでは>
//			int ThreadFirst() {
//				HANDLE hArray[] = { mcls->GetExecQueEvSem(), *** }
//			}
//
//			void ThreadEvent(int nEventNo) {
//				// 必須のキー取得処理
//				HANDLE evEnd;
//				p = mcls->GetExecQueFromHead(&evEnd);
//
//				//いろんな処理
//				p->***
//
//				// 必須のエンド処理
//				mcls->SetExecQueEnd(evEnd);
//			}
//
//		<Cクラスでは>
//			int ThreadFirst() {
//				HANDLE hArray[] = { mcls->GetEndQueEvSem(), *** }
//			}
//
//			void ThreadEvent(int nEventNo) {
//				// キー取得処理
//				p = mcls->GetEndQueFromHead();
//
//				// 後はいつも通り
//			}
//
// *********************************************************************************

#pragma once

// 疵検クラス インクルード
#include "..\..\KizuLib\Class\KizuMacro.h"								// マクロ集
#include "..\..\KizuLib\Class\KizuFunction.h"							// スタティックライブラリ集
#include "..\..\KizuLibMFC\Class\LogFileManager.h"						// ログ管理クラス

#include "ThreadManager.h"												// ワーカースレッドマネージャー

using namespace KizuLib;

namespace KizuLib
{

	/// スレッド間同期マネージャークラス
	template <class data_t>			// ← in,outが異なる場合は、void型で良い
	class ThreadSyncManager : public ThreadManager
	{

	//// 構造体
	public:
		//=============================
		// 本クラス受け渡し用クラス
		//=============================
		//// スレッド間受け渡しキュー
		struct DELIVERY_QUE {
			HANDLE				evExecEnd;									// スレッド間同期用シグナル(処理クラスで処理完了時にシグナルにすること)
			data_t*				p;											// 受け渡し実データ
		};


	//// 基本
	public:
		ThreadSyncManager(const char* pThreadName="", int InSemCnt = 256, int OutSemCnt = 512, int nMaxWaitNum = 50);	// コンストラクタ
		virtual ~ThreadSyncManager(void);									// デストラクタ

		void AttachNextSyncMgr(ThreadSyncManager<void>* cls) {mcls_pNextSyncMgr=cls;}		// 多段並列処理 (処理前に実行すること)

		void ReSetMaxCount()		{ myque_Sync.ReSetMaxCount(); myque_Exec.ReSetMaxCount(); myque_End.ReSetMaxCount(); };	// 瞬間最大登録件数初期化



	//// 要素操作 (myque_Sync)
		BOOL		AddToTail(data_t * newData);							// 要素を末尾に一つ追加

		// 必要そうなプロパティ
		int  GetSyncQueCount()const		{return myque_Sync.GetCount();};	// 現在の登録件数取得
		int  GetSyncQueSemCount() const {return myque_Sync.GetSemCount();};	// 登録出来る最大件数
		int  GetSyncQueMaxCount() const {return myque_Sync.GetMaxCount();};	// 瞬間最大登録件数取得
		int  GetSyncQueFreeCount()const {return min(myque_Sync.GetFreeCount(),myque_Exec.GetFreeCount());}	// 残登録件数



	//// 要素操作 (myque_Exec)
		HANDLE		GetExecQueEvSem() const { return myque_Exec.g_evSem; }	// 並列スレッドで使用するキュー待ちシグナル
		data_t *	GetExecQueFromHead(HANDLE* evExecEnd);					// 先頭要素取り出し(待ちなしバージョン) (セマフォが減らないから注意する事)
		void*		GetExecQueFromHead_InOutChange();
		void		SetExecQueEnd(HANDLE evExecEnd) { SetEvent(evExecEnd);} // 並列処理完了通知 (必須)
	
		// 必要そうなプロパティ
		int  GetEexcQueCount()const		{return myque_Exec.GetCount();};	// 現在の登録件数取得


	//// 要素操作 (myque_End)
		HANDLE		GetEndQueEvSem() const { return myque_End.g_evSem; }	// 並列スレッドで使用するキュー待ちシグナル
		data_t *	GetEndQueFromHead()	{ return myque_End.GetFromHead();}	// 先頭要素取り出し(待ちなしバージョン) (セマフォが減らないから注意する事)

		// 必要そうなプロパティ
		int  GetEndQueCount()const		{return myque_End.GetCount();};		// 現在の登録件数取得
		int  GetEndQueMaxCount()const	{return myque_End.GetMaxCount();};	// 瞬間最大登録件数取得




	//// 継承元プロテクト関数
	protected:
		int			ThreadFirst();											// スレッド開始前処理 (スレッド実行直後(シグナルセット前)にコールバック)
		int			ThreadLast();											// スレッド終了前処理 (終了シグナル後にコールバック)
		void		ThreadEvent(int nEventNo);								// スレッドイベント発生
		


	//// 同期制御
	protected:
		ThreadQueue<DELIVERY_QUE>	myque_Sync;								// 並列スレッド間順序保障管理用キュー
		ThreadQueue<DELIVERY_QUE>	myque_Exec;								// 並列処理用キュー
		ThreadQueue<data_t>			myque_End;								// 並列処理が完了した用キュー(キューイングした順番通り)
		
		ThreadSyncManager*			mcls_pNextSyncMgr;						// 多段並列処理 (次に実行する並列処理クラスのポインタ)
	};



	// //////////////////////////////////////////////////////////////////////////////
	// テンプレートなのでヘッダファイルに入れておく必要あり
	// //////////////////////////////////////////////////////////////////////////////



	//------------------------------------------
	// コンストラクタ
	// const char* pThreadName 自スレッド名
	// int InSemCnt キューの最大サイズ (デフォルト256)
	// int OutSemCnt キューの最大サイズ (デフォルト512)
	// int nMaxWaitNum WaitForMultipleObjectsの最大待ち行列
	//------------------------------------------
	template <class data_t>
	ThreadSyncManager<data_t>::ThreadSyncManager(const char* pThreadName, int InSemCnt, int OutSemCnt, int nMaxWaitNum) :
	ThreadManager(pThreadName, nMaxWaitNum),
	myque_Sync(InSemCnt),
	myque_Exec(InSemCnt),
	myque_End(OutSemCnt),
	mcls_pNextSyncMgr(NULL)
	{

	}

	//------------------------------------------
	// デストラクタ
	//------------------------------------------
	template <class data_t>
	ThreadSyncManager<data_t>::~ThreadSyncManager()
	{
	}


// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 要素操作 (myque_Sync)

	//------------------------------------------
	// 要素を末尾に一つ追加
	// data_t * p 登録するインスタンス
	// 戻り値 復帰情報
	//------------------------------------------
	template <class data_t>
	BOOL ThreadSyncManager<data_t>::AddToTail(data_t * newData)
	{
		//// 管理用の構造体を生成
		DELIVERY_QUE* pDeli = new DELIVERY_QUE;
		pDeli->evExecEnd	= CreateEvent(NULL, FALSE, FALSE, NULL);		// 並列処理完了シグナル
		pDeli->p			= newData;

		//// キューイング
		// 最初に 管理用
		if( ! myque_Sync.AddToTail(pDeli) ) {
			CloseHandle(pDeli->evExecEnd);
			delete pDeli;
			return FALSE;
		}
		// 次に処理用
		if( ! myque_Exec.AddToTail(pDeli) ) {
			CloseHandle(pDeli->evExecEnd);
			delete pDeli;
			return FALSE;
		}

		return TRUE;
	}

// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 要素操作 (myque_Exec)

	//------------------------------------------
	// 先頭要素取り出し(待ちなしバージョン) (セマフォが減らないから注意する事)
	// HANDLE * evExecEnd 受け渡し用。（取り出し元では何もしない）
	// 戻り値 取得したインスタンス
	//------------------------------------------
	template <class data_t>
	data_t * ThreadSyncManager<data_t>::GetExecQueFromHead(HANDLE * evExecEnd)
	{
		//// データ取り出し
		DELIVERY_QUE* pDeli = myque_Exec.GetFromHead();
		if(NULL == pDeli) return NULL;

		//// もともとの形に分離
		* evExecEnd = pDeli->evExecEnd;
		return pDeli->p;
	}

	//------------------------------------------
	// 先頭要素取り出し(待ちなしバージョン) (セマフォが減らないから注意する事) (IN,OUTの方を変更する場合用)
	// HANDLE * evExecEnd 受け渡し用。（取り出し元では何もしない）
	// 戻り値 取得したインスタンス
	//------------------------------------------
	template <class data_t>
	void * ThreadSyncManager<data_t>::GetExecQueFromHead_InOutChange()
	{
		// 使うほうでバラスこと
		return myque_Exec.GetFromHead();
	}


// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 要素操作 (myque_Exec)



// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// スレッド間同期制御処理 (メイン)


	//------------------------------------------
	// スレッド開始前処理 (スレッド実行直後(シグナルセット前)にコールバック)
	//------------------------------------------
	template <class data_t>
	int ThreadSyncManager<data_t>::ThreadFirst()
	{
		//// イベント設定
		HANDLE hArray[] = { myque_Sync.g_evSem
							};
		SetEventNum(sizeof(hArray)/sizeof(HANDLE), hArray);
		return 0;
	}
	//------------------------------------------
	// スレッド終了前処理 (終了シグナル後にコールバック)
	//------------------------------------------
	template <class data_t>
	int ThreadSyncManager<data_t>::ThreadLast()
	{

		return 0;
	}


	//------------------------------------------
	// スレッドイベント発生
	// int nEventNo イベントNo (0オリジン)
	//------------------------------------------
	template <class data_t>
	void ThreadSyncManager<data_t>::ThreadEvent(int nEventNo)
	{
		DELIVERY_QUE* pDeli = NULL;

		////// シグナルの条件
		enum {	EV_QUE = 0,				// メールスロット 通知 (メールは一番最後にしようかな)
		};

		////// シグナル条件分け
		switch (nEventNo) {

	//-----------------------------------------------------------------------------------------------
		case EV_QUE: 						// メールスロット 通知
			// 並列スレッド間順序保障
			if(true) {
				//// 並列スレッドで処理が完了するまで待機
				pDeli = myque_Sync.GetFromHead();
				WaitForSingleObject( pDeli->evExecEnd, INFINITE );


				////// 並列スレッドで処理が完了したら、終わったよキューにキューイング
				//// 絶対登録
				//if( ! myque_End.AddToTail(pDeli->p, 20, 15) ) {
				//	// 消失
				//	SAFE_DELETE(pDeli->p);
				//	LOG(em_ERR), "[%s] 管理データ消失！！myque_Endキューフル", my_sThreadName);
				//}
				
				//// 多段並列処理対応
				if(NULL == mcls_pNextSyncMgr) {
					// 従来通り。自分のEndキューに登録。

					//// 並列スレッドで処理が完了したら、終わったよキューにキューイング
					// 絶対登録
					if( ! myque_End.AddToTail(pDeli->p, 20, 15) ) {
						// 消失
						SAFE_DELETE(pDeli->p);
						LOG(em_ERR), "[%s] 管理データ消失！！myque_Endキューフル", my_sThreadName);
					}
				
				} else {

					// 多段用に、直接次の並列処理に登録
					if( NULL != pDeli->p ) {
						if( ! mcls_pNextSyncMgr->AddToTail(pDeli->p)) {
							// 消失
							SAFE_DELETE(pDeli->p);
							LOG(em_ERR), "[%s] 多段並列処理受け渡しでキューフル", my_sThreadName);
						}
					} else {
						LOG(em_WAR), "[%s] 多段並列処理中間でキュー返却の可能性有り", my_sThreadName);
					}
				}


				//// 後始末
				CloseHandle(pDeli->evExecEnd);
				SAFE_DELETE(pDeli);
			}
			break;

	//-----------------------------------------------------------------------------------------------
		default :
			// ありえない！！
			LOG(em_ERR), "[%s] ThreadEvent EventNo NG=%d", my_sThreadName, nEventNo );
			return;
		}
	}

}
