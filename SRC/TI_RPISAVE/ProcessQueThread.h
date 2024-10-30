// *********************************************************************************
//	Proc関係のベースクラス
//	[Ver]
//		Ver.01    2013/07/09	初版
//
//	[メモ]
// *********************************************************************************

#pragma once

// 疵検クラス インクルード
#include "ProcessQueImpl.h"												// 元

/*/ メモリーリーク検出 ----->>>
#ifdef _DEBUG
#undef THIS_FILE
#define new DEBUG_NEW
#endif
//*/// <<<------------------------


//======================================================================
// プロセス関係スレッドのベースクラス
template <class Tin, class Tout >			// 入力Que, 出力Que
class ProcessQueThread 
{
//// 必須
protected:
	IQueue<Tin > *mque_pIn;						// 入力Que
	IQueue<Tout> *mque_pOut;					// 出力Que


//// 公開関数
public:
	//------------------------------------------
	// コンストラクタ
	//------------------------------------------
	ProcessQueThread()
	{
		mque_pIn  = NULL;
		mque_pOut = NULL;
	}

	//=========================================
	// 受渡キュー登録
	virtual void SetQueIn ( IQueue<Tin >  *pQueIn ) { mque_pIn  = pQueIn; }		// 入力Queセット
	virtual void SetQueOut( IQueue<Tout>  *pQueOut) { mque_pOut = pQueOut;}		// 出力Queセット


//// メンバー処理
protected:
	//=========================================
	// 仮想関数

	//------------------------------------------
	// 実処理
	// Tin* pIn		入力Queデータ
	// 戻り値		NULL:返却しない 出力Queデータ:次のスレッドへ返却する場合  
	//------------------------------------------
	virtual Tout* OnAddNewItem( Tin* pIn ) = 0;


	//------------------------------------------
	// Queデータ受付時のデフォルト処理
	//------------------------------------------
	virtual void ExecQue() {
		HANDLE	evEnd = NULL;				// 並列処理用

		// 入力Que 取得
		Tin* pInData = mque_pIn->GetFromHead(&evEnd);

		// 継承先実行
		Tout*	pOutData = OnAddNewItem(pInData);

		// キュー返却
		// evEnd がある場合は、並列処理なので、絶対に完了通知を送る
		// pOutData がNULLの場合は、この処理で一時中断なので、次の処理への通知はしない
		if(NULL != evEnd || NULL != pOutData) mque_pOut->AddToTail(pOutData, evEnd);
	}
};


//======================================================================
// スレッド間キュー接続		※ 作ったけどあまり使わない。
// キューを必要としているスレッド２つを、キューにより接続する。
// FromThread -> pQue -> ToThread
template< class T1, class T2, class T3>		// 前処理、受渡Que、後処理
void ConnectQueue( ProcessQueThread<T1, T2> *pFromThread = NULL, 
				   IQueue<T2> *pQue = NULL,
				   ProcessQueThread<T2, T3> *pToThread = NULL )
{
	_ASSERT(pQue);
	if( NULL != pFromThread) pFromThread->SetQueOut( pQue );				// 前処理スレッドの出力Queにセット
	if( NULL != pToThread)	 pToThread->SetQueIn( pQue );					// 後処理スレッドの入力Queにセット
}

// pQue -> ToThread
template< class T2, class T3>		// 受渡Que、後処理
void ConnectQueueIn( IQueue<T2> *pQue = NULL,
					   ProcessQueThread<T2, T3> *pToThread = NULL )
{
	_ASSERT(pQue);
	if( NULL != pToThread)	 pToThread->SetQueIn( pQue );					// 後処理スレッドの入力Queにセット
}
// FromThread -> pQue
template< class T1, class T2>		// 前処理、受渡Que
void ConnectQueueOut( ProcessQueThread<T1, T2> *pFromThread = NULL, 
					   IQueue<T2> *pQue = NULL)
{
	_ASSERT(pQue);
	if( NULL != pFromThread) pFromThread->SetQueOut( pQue );				// 前処理スレッドの出力Queにセット
}



