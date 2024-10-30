#include "StdAfx.h"
#include "RpiControlProc.h"

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
RpiControlProc::RpiControlProc(void) :
ThreadManager("RpiCP")
{
}

//------------------------------------------
// デストラクタ
//------------------------------------------
RpiControlProc::~RpiControlProc(void)
{
}

//------------------------------------------
// スレッド開始前処理 (スレッド実行直後(シグナルセット前)にコールバック)
//------------------------------------------
int RpiControlProc::ThreadFirst()
{
	//// イベント設定
	HANDLE hArray[] = { this->mque_pIn->GetEvQue() 
						};
	SetEventNum(sizeof(hArray)/sizeof(HANDLE), hArray);
	return 0;
}

//------------------------------------------
// スレッド終了前処理 (終了シグナル後にコールバック)
//------------------------------------------
int RpiControlProc::ThreadLast()
{
	return 0;
}

//------------------------------------------
// スレッドイベント発生
// int nEventNo イベントNo (0オリジン)
//------------------------------------------
void RpiControlProc::ThreadEvent(int nEventNo)
{
	////// シグナルの条件
	enum {	EV_ADD_IMG = 0,					// フレーム画像キューイング
	};

	////// シグナル条件分け
	switch (nEventNo) {

//-----------------------------------------------------------------------------------------------
	case EV_ADD_IMG: 						// フレーム取込
		AddNewItem();
		return;

//-----------------------------------------------------------------------------------------------
	default :
		// ありえない！！
		LOG(em_ERR), "[%s] ThreadEvent EventNo NG=%d", my_sThreadName, nEventNo );
		return;
	}
}

// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 処理

//------------------------------------------
// 画像取得
//------------------------------------------
void RpiControlProc::AddNewItem()
{
	//// フレーム画像キューを取得
	FrameDataManager* p = mque_pIn->GetFromHead();
	if( NULL == p ) {															// 通常ありえないはず
		LOG(em_ERR), "[%s] スレッドループキュー無し!", my_sThreadName);
		return;
	}

	//// 処理振り分け
	int nIns = p->nIns;
	int nColor = p->nColor;
	if( 0 > nIns || nIns >= mcls_pParam->GetMain().nIns ) {						// 通常ありえないはず
		LOG(em_ERR), "[%s] インスタンス番号異常=%d", my_sThreadName, nIns);
		return;
	}
	if( 0 > nColor || nColor >= MAX_COLOR_NUM  ) {		// 通常ありえないはず
		LOG(em_ERR), "[%s] 画像色番号異常=%d", my_sThreadName, nColor);
		return;
	}

	//// 指定のRPI書き込みクラスへ
	DELIVERY_RPIWRITE* pDeli = new DELIVERY_RPIWRITE;
	pDeli->emNo = ENUM_KUBUN_RPIWRITE::em_FRAME_INFO;
	pDeli->data = (BYTE*) p;
	mque_pOut[nIns][nColor]->AddToTail(pDeli);
}

//------------------------------------------
// RPIファイルクローズ要求
//------------------------------------------
void RpiControlProc::ClosingRpi()
{
	//// 書込み中の RPIファイルをクローズする
	for(int ii=0; ii<MAX_INS; ii++) {
		ClosingRpi(ii);
	}
}

//------------------------------------------
// RPIファイルクローズ要求
// int nIns 指定Insのみ (0オリジン)
//------------------------------------------
void RpiControlProc::ClosingRpi(int nIns)
{
	for(int jj=0; jj<MAX_COLOR_NUM; jj++) {
		if( NULL == mque_pOut[nIns][jj]) continue;

		//// 指定のRPI書き込みクラスへ
		DELIVERY_RPIWRITE* pDeli = new DELIVERY_RPIWRITE;
		pDeli->emNo = ENUM_KUBUN_RPIWRITE::em_WRITE_END;
		pDeli->data = NULL;
		mque_pOut[nIns][jj]->AddToTail(pDeli);
	}
}
//------------------------------------------
// 検査開始時 準備
//------------------------------------------
bool RpiControlProc::Init()
{
	for(int ii=0; ii<MAX_INS; ii++) {
		for(int jj=0; jj<MAX_COLOR_NUM; jj++) {
			if( NULL == mcls_pRpiWProc[ii][jj]) continue;

			mcls_pRpiWProc[ii][jj]->SetData_Setting();
		}
	}
	return true;
}
