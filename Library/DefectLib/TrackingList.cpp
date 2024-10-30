// TrackingList.cpp: TrackingList クラスのインプリメンテーション
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "TrackingList.h"

/*
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif
*/

//////////////////////////////////////////////////////////////////////
// 構築/消滅
//////////////////////////////////////////////////////////////////////

//------------------------------------------
// コンストラクタ
// const double* length チェックする距離ポインタ [mm] (通常は、カウンターボードで管理している距離のアドレスを渡す)
//------------------------------------------
TrackingList::TrackingList(const double* length) :
m_dLength(length),
gque_Deli(1024)
{
	// 初期化
	InitializeCriticalSection(&m_csLock);						// クリティカルセクションオブジェクトを初期化
}

//------------------------------------------
// デストラクタ
//------------------------------------------
TrackingList::~TrackingList()
{
	// 解放
	DeleteCriticalSection(&m_csLock);							// クリティカルセクションオブジェクトを解放
}

//------------------------------------------
// 新規トラッキング アイテムを追加する
// DWORD len 何mの位置を通過したら通知? [mm]
// int nKey キー情報 (0:キーチェック無しの意味となる)
// long nAddr1 ユーザー用ワークエリア
// long nAddr2 ユーザー用ワークエリア
// long const* pAddr ユーザー用ワークエリア (通常クラスのポインタ等セット)(未使用時はNULL)
//------------------------------------------
void TrackingList::TrackingAdd_Len(DWORD len, int nKey, long nAddr1, long nAddr2, long const* pAddr)
{
	long dist = (long)(len - *m_dLength);	// 後mで通過
	if(0 > dist) dist = 0;					// 既に通過していたら、次チェックで即時起動とする

	this->TrackingAdd_Dist(dist, nKey, nAddr1, nAddr2, pAddr);
}

//------------------------------------------
// 新規トラッキング アイテムを追加する
// DWORD dist 何mm後に通知? [mm]
// int nKey キー情報 (0:キーチェック無しの意味となる)
// long nAddr1 ユーザー用ワークエリア
// long nAddr2 ユーザー用ワークエリア
// long const* pAddr ユーザー用ワークエリア (通常クラスのポインタ等セット)(未使用時はNULL)
//------------------------------------------
void TrackingList::TrackingAdd_Dist(DWORD dist, int nKey, long nAddr1, long nAddr2, long const* pAddr)
{
	AutoLock autolock(&m_csLock);

	// 新規トラッキングアイテムを生成
	TR_INF*	pInf = new TR_INF();
	pInf->nDist		 = dist;
	pInf->dDistNow	 = *m_dLength;
	pInf->dDistEnd	 = 0.0;
	pInf->dChangeLengthMax = 0;
	pInf->emEndState = TR_STOP;
	pInf->nKey		 = nKey;
	pInf->nAddr1	 = nAddr1;
	pInf->nAddr2	 = nAddr2;	
	pInf->pAddr		 = (long*)pAddr;

	// リスト 追加
	m_List.push_back(pInf);
}


//------------------------------------------
// コイル切替等が発生時、前回コイル長を今あるアイテムにセットする
// double dOldLen 前回コイル長 [mm]
//------------------------------------------
void TrackingList::TrackingLengthChange(double dOldLen)
{
	AutoLock autolock(&m_csLock);

	//// リスト格納分 全件チェックを行う
	TR_INF*							pInf;					// トラッキング情報
	std::list<TR_INF*>::iterator	itr;					// イテレータ
	for( itr = m_List.begin(); itr != m_List.end(); itr++ ) {
		pInf = *itr;

		// ゲタはかせ分を加算
		pInf->dChangeLengthMax += dOldLen;
	}
}

//------------------------------------------
// 指定キーNoのアイテムのクローンを取得する (現状を知りたい場合等)
// int nKey 取得するキーNo
// TR_INF* inf 一番最初に一致したキーのアイテムのクローン (実体では無いので注意)
// 戻り値 true:データ有り  false:該当キー無し
//------------------------------------------
bool TrackingList::GetCloneKeyItem(int nKey, TR_INF* inf)
{
	//// リスト格納分 全件チェックを行う
	TR_INF*							pInf;					// トラッキング情報
	std::list<TR_INF*>::iterator	itr = m_List.begin();	// イテレータ
	for( itr = m_List.begin(); itr != m_List.end(); itr++ ) {
		pInf  = *itr;
		// 一致するキー有り？
		if( pInf->nKey == nKey ) {
			memcpy(inf, pInf, sizeof(TR_INF));
			return true;
		}
	}

	// 一致無し
	return false;
}

//------------------------------------------
// トラッキングアイテム をチェックして、指定距離を通板していたら TR_END でキューイング
//------------------------------------------
void TrackingList::TrackingCheck()
{
	AutoLock autolock(&m_csLock);
	
	//// 前準備
	double	dLength = *m_dLength;							// 現在の距離を保持

	//// リスト格納分 全件チェックを行う
	bool							bNext;					// 次イテレータチェック用
	TR_INF*							pInf;					// トラッキング情報
	std::list<TR_INF*>::iterator	itr = m_List.begin();	// イテレータ
	while( itr != m_List.end() ) {
		pInf  = *itr;
		bNext = true;

		// 通知t対象距離に達した場合に通知
		if( dLength - (pInf->dDistNow - pInf->dChangeLengthMax) >= (double)pInf->nDist ) {
			// スレッドキューに登録出来た場合のみ、リストから削除
			if( gque_Deli.GetFreeCount() > 0 ) {
				pInf->emEndState = TR_END;
				pInf->dDistEnd	 = dLength;
				gque_Deli.AddToTail( pInf );
				// リストから削除
				itr = m_List.erase(itr);	// 削除した後、次のイテレータを戻す
				bNext = false;
			}
		}
		
		// 次のイテレータへ
		if(bNext) itr++;
	}
}

//------------------------------------------
// 強制停止 (これをコールすると TR_CANCEL でキューイングする)
// double dLength 何ｍ進んだ事としてトラッキングを発生させる [mm]
// int nKey キー情報 (0:キーチェック無しの意味となる)
//------------------------------------------
void TrackingList::TrackingCancel(double dLength, int nKey)
{
	AutoLock autolock(&m_csLock);

	//// 前準備
	double	dNowLength = *m_dLength;						// 現在の距離を保持

	//// リスト格納分 全件チェックを行う
	bool							bNext;					// 次イテレータチェック用
	TR_INF*							pInf;					// トラッキング情報
	std::list<TR_INF*>::iterator	itr = m_List.begin();	// イテレータ
	while( itr != m_List.end() ) {
		pInf  = *itr;
		bNext = true;

		
		// 通知t対象距離に達した場合に通知
		if( ( dLength - (pInf->dDistNow - pInf->dChangeLengthMax) >= (double)pInf->nDist ) &&
			( (0==nKey) || (nKey==pInf->nKey)) ){

			// スレッドキューに登録出来た場合のみ、リストから削除
			if( gque_Deli.GetFreeCount() > 0 ) {
				pInf->emEndState = TR_CANCEL;
				pInf->dDistEnd	 = dNowLength;
				gque_Deli.AddToTail( pInf );
				// リストから削除
				itr = m_List.erase(itr);	// 削除した後、次のイテレータを戻す
				bNext = false;
			}
		}
		
		// 次のイテレータへ
		if(bNext) itr++;
	}
}

//------------------------------------------
// 強制停止 指定キーのみをキャンセル
// int nKey キー情報 (0:キーチェック無しの意味となる)
//------------------------------------------
void TrackingList::TrackingCancelKey(int nKey)
{
	AutoLock autolock(&m_csLock);
	
	//// 前準備
	double	dLength = *m_dLength;							// 現在の距離を保持

	//// リスト格納分 全件チェックを行う
	bool							bNext;					// 次イテレータチェック用
	TR_INF*							pInf;					// トラッキング情報
	std::list<TR_INF*>::iterator	itr = m_List.begin();	// イテレータ
	while( itr != m_List.end() ) {
		pInf  = *itr;
		bNext = true;

		
		// 通知t対象距離に達した場合に通知
		if( (0==nKey) || (nKey==pInf->nKey) ) {

			// スレッドキューに登録出来た場合のみ、リストから削除
			if( gque_Deli.GetFreeCount() > 0 ) {
				pInf->emEndState = TR_CANCEL;
				pInf->dDistEnd	 = dLength;
				gque_Deli.AddToTail( pInf );
				// リストから削除
				itr = m_List.erase(itr);	// 削除した後、次のイテレータを戻す
				bNext = false;
			}
		}
		
		// 次のイテレータへ
		if(bNext) itr++;
	}
}

//------------------------------------------
// トラッキングアイテム を全て解放＆キューイングする
//------------------------------------------
void TrackingList::TrackingStop()
{
	AutoLock autolock(&m_csLock);

	//// 前準備
	double	dLength = *m_dLength;							// 現在の距離を保持

	//// リスト格納分 全件チェックを行う
	TR_INF*							pInf;					// トラッキング情報
	std::list<TR_INF*>::iterator	itr;					// イテレータ
	for( itr = m_List.begin(); itr != m_List.end(); itr++ ) {
		pInf = *itr;

		// スレッドキューに登録出来た場合のみ、リストから削除
		if( gque_Deli.GetFreeCount() > 0 ) {
			pInf->emEndState = TR_STOP;
			pInf->dDistEnd	 = dLength;
			gque_Deli.AddToTail( pInf );
		}
	}
	
	// リストから削除
	m_List.clear();
}

//------------------------------------------
// 新規キュー アイテムを追加する (トラッキングせずに、即時にキューイング)
// int nKey キー情報 (0:キーチェック無しの意味となる)
// long nAddr1 ユーザー用ワークエリア
// long nAddr2 ユーザー用ワークエリア
// long const* pAddr ユーザー用ワークエリア (通常クラスのポインタ等セット)(未使用時はNULL)
//------------------------------------------
void TrackingList::QueAdd(DWORD dist, int nKey, long nAddr1, long nAddr2, long const* pAddr)
{
	AutoLock autolock(&m_csLock);

	// 新規トラッキングアイテムを生成
	TR_INF*	pInf = new TR_INF();
	pInf->nDist		 = dist;
	pInf->dDistNow	 = *m_dLength;
	pInf->dDistEnd	 = *m_dLength;
	pInf->dChangeLengthMax = 0;
	pInf->dDistNow	 = *m_dLength;
	pInf->emEndState = TR_STOP;
	pInf->nKey		 = nKey;
	pInf->nAddr1	 = nAddr1;
	pInf->nAddr2	 = nAddr2;	
	pInf->pAddr		 = (long*)pAddr;


	// 即時キューイング
	if( gque_Deli.GetFreeCount() > 0 ) {
		pInf->emEndState = TR_END;
		gque_Deli.AddToTail( pInf );
	}
}

//------------------------------------------
// すべてのキューを開放
//------------------------------------------
void TrackingList::QueAllFree()
{
	AutoLock autolock(&m_csLock);

	//// リスト格納分 全件チェックを行う
	TR_INF*							pInf;					// トラッキング情報
	std::list<TR_INF*>::iterator	itr;					// イテレータ
	for( itr = m_List.begin(); itr != m_List.end(); itr++ ) {
		pInf = *itr;

		// 開放
		if ( pInf->pAddr != NULL ) delete pInf->pAddr;		// データ部開放
		delete pInf;
		pInf = NULL;
	}
	
	// リストから削除
	m_List.clear();
}

//------------------------------------------
// スレッド
//------------------------------------------
UINT WINAPI TrackingList::MainThread(LPVOID param)
{
	////// メインインスタンス取り出し
	TrackingList *p = (TrackingList *)param;
	p->my_Tid = GetCurrentThreadId();

	// タイマーセット
	HANDLE evTimer = CreateWaitableTimer(NULL,FALSE, NULL);
	__int64 ts = 0;
	SetWaitableTimer(evTimer, (LARGE_INTEGER *)&ts, 20, NULL, NULL, FALSE);

	// WaitForMultipleObjectsイベント
	HANDLE	hArray[] = {p->my_evStop,  
						evTimer
						};
	enum {	EV_STOP = WAIT_OBJECT_0,
			EV_TIMER, 
			EV_END	};

	// メッセージキュー読み込み
	while (1) {
		DWORD ret = WaitForMultipleObjects(EV_END, hArray, FALSE, INFINITE);
		switch (ret) {

//-----------------------------------------------------------------------------------------------
		case EV_STOP:														// スレッド終了
			// ローカルのハンドル解放
			CancelWaitableTimer(evTimer);
			CloseHandle(evTimer);
			// トラッキングの停止
			p->TrackingStop();
			return 0;

//-----------------------------------------------------------------------------------------------
		case EV_TIMER:														// タイマー
			// 通知t対象距離に達した場合に通知
			p->TrackingCheck(); 
			break;
		}
	}
	return 0;
}

