#include "stdafx.h"


#include "..\..\Library\KizuLibMFC\CLASS\ThreadManager.h"				// ワーカースレッドマネージャー
#include "..\..\Library\KizuLibMFC\CLASS\ThreadLoopQueue.h"				// 回転寿司形式のスレッドキュー
#include "..\..\Library\KizuLibMFC\Class\ThreadSyncManager.h"			// 並列スレッド間順序保障制御クラス
#include "MainConst.h"
#include "DummyFrame.h"




extern HT_RPI_FDATA *new_dummy_frame()
{
	HT_RPI_FDATA *p = new HT_RPI_FDATA();
	p->base_head.addr[0] = (LONG*)CDummyQueueFree::GetMemory();
	return p;
}

void * CDummyQueueFree::GetAllocateMemory(int nSize) {
	static void *p;
	if (p == NULL && nSize > 0)
	{
		p = VirtualAlloc(NULL, nSize, MEM_COMMIT | MEM_TOP_DOWN, PAGE_READONLY);
	}
	return p;
}

 void CDummyQueueFree::InitMemory(int nSize) {
	GetAllocateMemory(nSize);
}

 void * CDummyQueueFree::GetMemory() {
	return GetAllocateMemory();
}

 BOOL CDummyQueueFree::AddToTail(HT_RPI_FDATA * pQueData, void * arg)
 {
	 if( pQueData == NULL )
		 return TRUE;
	 // new_dummy_frameで作られたダミーフレーム。
	 // addrへの参照を剥がして、delete(free防止）。
	 //次のキューには伝送しない
	 if (pQueData->base_head.addr[0] == GetMemory()) {
		 pQueData->base_head.addr[0] = 0;
		 delete pQueData;
		 return TRUE;
	 }

	 return CInterceptQueue<HT_RPI_FDATA>::AddToTail(pQueData, arg);
 }
