#include "StdAfx.h"
#include "RaiseEventManager.h"

// メモリーリーク検出 ----->>>
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif
// <<<------------------------

//------------------------------------------
// コンストラクタ
//------------------------------------------
RaiseEventManager::RaiseEventManager(void) :
ThreadManager("RaiseEvent")
{
	//// ハンドル生成
}

//------------------------------------------
// デストラクタ
//------------------------------------------
RaiseEventManager::~RaiseEventManager(void)
{
	//// ハンドル解放
}

//------------------------------------------
// イベント発生
// EM_DIV_KIND emEvent イベント種別
// int nEventSub サブ種別
// BYTE* pEvent 構造体ポインタ
//------------------------------------------
void RaiseEventManager::RaiseEvents(EM_DIV_KIND emEvent, int nEventSub, BYTE* pEvent)
{
	////// シナリオ条件分け
	switch (emEvent) {
//-----------------------------------------------------------------------------------------------
	case DIV_KIND_PLG:										
		if(true) {
			TYP_EVENT_PLG_SPEED* p = (TYP_EVENT_PLG_SPEED*)pEvent;
			
			mcls_pPlg->SetSaveData().nSpeed[p->nCh] = p->nMpm;
			// 変更通知。 (Trueを渡しておけば、関数内で勝手に停止開始を判断してくれる)			
			mcls_pPlg->SetTimer_SpeedCycle(p->nCh, true);
		}
		return;

//-----------------------------------------------------------------------------------------------
	case DIV_KIND_PI:										
		if(0==nEventSub) {
			TYP_EVENT_PI_BIT* p = (TYP_EVENT_PI_BIT*)pEvent;
			
			mcls_pPio->SetValPi( p->nBood, p->nPort, p->nBit, __BOOL p->nMode); 


		} else if(1==nEventSub) {
			TYP_EVENT_PI_PORT* p = (TYP_EVENT_PI_PORT*)pEvent;

			mcls_pPio->SetValPi( p->nBood, p->nPort, p->nBitSet); 


		} else if(2==nEventSub) {
			TYP_EVENT_PI_BITPULS* p = (TYP_EVENT_PI_BITPULS*)pEvent;

			// △
		
		}
		return;

////-----------------------------------------------------------------------------------------------
//	case DIV_KIND_PO:										
//		return;

//-----------------------------------------------------------------------------------------------
	case DIV_KIND_MEM:										
		if(true) {
			TYP_EVENT_MEM_WRITE* p = (TYP_EVENT_MEM_WRITE*)pEvent;
			
			// 共有メモリアクセス
			HANDLE h;
			BYTE*  t;
			mem_connect(&h, (LPVOID *)t, p->cTblName);
			if(h == NULL) {
				LOG(em_ERR), "[%s] 共有メモリ アクセス失敗 [%s]", my_sThreadName, p->cTblName);
				return;
			}

			// バイナリファイルセット
			std::ifstream   is;
			is.open(p->cWriteDataPath, std::ios::binary);
			is.read( (char*)&t[p->nIndex], 1024*1024*1024);
			is.close();

			// 共有メモリ開放
			mem_close(&h, (LPVOID *)t);
		}
		return;

//-----------------------------------------------------------------------------------------------
	case DIV_KIND_MAIL:				
		if(true) {
			TYP_EVENT_MAIL_SEND* p = (TYP_EVENT_MAIL_SEND*)pEvent;

			send_mail(p->cTaskName, p->cPcName, &p->que);
		}
		return;

////-----------------------------------------------------------------------------------------------
//	case DIV_KIND_TIME:										
//		return;

//-----------------------------------------------------------------------------------------------
	default :
		// ありえない！！
		LOG(em_ERR), "[%s] 不明=%d", my_sThreadName, emEvent );
		return;
	}
}

