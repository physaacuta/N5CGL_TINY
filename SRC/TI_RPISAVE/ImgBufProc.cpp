#include "StdAfx.h"
#include "ImgBufProc.h"


// メモリーリーク検出 ----->>>
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif
// <<<------------------------

#define LOG2( em, ... ) do { \
	if(mcls_detail){   mcls_detail->Write(KizuLib::LogFileManager:: em, __VA_ARGS__ ); }\
	if(!mcls_detail || \
		KizuLib::LogFileManager::em == KizuLib::LogFileManager::em_ERR ||\
	    KizuLib::LogFileManager::em == KizuLib::LogFileManager::em_WAR) {\
		if(mcls_pLog){       mcls_pLog->Write(KizuLib::LogFileManager:: em, __VA_ARGS__ ); }\
	}\
}while(0)

//------------------------------------------
// コンストラクタ
//------------------------------------------
ImgBufProc::ImgBufProc(void) :
ThreadManager("ImgBufP"),
m_bIsExec(false),
mcls_detail(NULL)
{
	Reset();
}

//------------------------------------------
// デストラクタ
//------------------------------------------
ImgBufProc::~ImgBufProc(void)
{
	Free();
}

//------------------------------------------
// 領域確保
//------------------------------------------
void ImgBufProc::Alloc()
{
	ParamManager::P_MAIN		const& prm	= mcls_pParam->GetMain();
}

//------------------------------------------
// 領域開放
//------------------------------------------
void ImgBufProc::Free()
{
}

//------------------------------------------
// リセット
//------------------------------------------
void ImgBufProc::Reset()
{
}

//------------------------------------------
// 処理実行有無
//------------------------------------------
void ImgBufProc::SetIsExec(bool bMode)
{
	m_bIsExec = bMode;
}

//------------------------------------------
// スレッド開始前処理 (スレッド実行直後(シグナルセット前)にコールバック)
//------------------------------------------
int ImgBufProc::ThreadFirst()
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
int ImgBufProc::ThreadLast()
{
	return 0;
}

//------------------------------------------
// スレッドイベント発生
// int nEventNo イベントNo (0オリジン)
//------------------------------------------
void ImgBufProc::ThreadEvent(int nEventNo)
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
		LOG2(em_ERR, "[%s] ThreadEvent EventNo NG=%d", my_sThreadName, nEventNo );
		return;
	}
}

// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 処理

//------------------------------------------
// 画像取得
//------------------------------------------
void ImgBufProc::AddNewItem()
{
	//// フレーム画像キューを取得
	FRAME_DATA_ARRAY* p = mque_pIn->GetFromHead();
	if( NULL == p ) {
		LOG2(em_ERR, "[%s] スレッドループキュー無し!", my_sThreadName);
		return;
	}
	LOG2(em_INF, "[%s] IN (Fno=%d/%d)", my_sThreadName, p->nFno, p->nCFno, p->emImgSaveStat );

	//// 処理
	if(m_bIsExec) {
		for(int ii=0; ii<MAX_COLOR_NUM; ii++ ) {
			if(NULL == p->Fdc.pFrameDataMgr[ii]) continue;
			((FrameDataManager*)p->Fdc.pFrameDataMgr[ii])->cls_Time.PutTimestamp(FrameDataManager::TIME_ImgBuf_START);
		}

		Exec(p);

		for(int ii=0; ii<MAX_COLOR_NUM; ii++ ) {
			if(NULL == p->Fdc.pFrameDataMgr[ii]) continue;
			((FrameDataManager*)p->Fdc.pFrameDataMgr[ii])->cls_Time.PutTimestamp(FrameDataManager::TIME_ImgBuf_END);
		}
	} else {
		LOG(em_WAR), "[%s] キャンセル", my_sThreadName);
	}


	//// ばらして次へ
	for(int ii=0; ii<MAX_COLOR_NUM; ii++) {
		if( NULL == p->Fdc.pFrameDataMgr[ii] ) continue;

		FrameDataManager* pF = (FrameDataManager*)p->Fdc.pFrameDataMgr[ii];

		if( ! pF->bRpiSave || ! m_bIsExec ) {
//◆
	LOG2(em_INF, "[%s] [%d] 結果表示へ。スキップ (Fno=%d/%d) st=%d (%s/%s)", my_sThreadName, ii, p->nFno, p->nCFno, p->emImgSaveStat, (pF->bRpiSave?"○":"×"), (m_bIsExec?"○":"×") );

			mque_pOutSkip->AddToTail( pF );	// 結果表示のみ

		} else {
//◆
	LOG2(em_INF, "[%s] [%d] 下流へ (Fno=%d/%d) st=%d", my_sThreadName, ii, p->nFno, p->nCFno, p->emImgSaveStat );

			mque_pOut->AddToTail( pF);	// 次へ
		}
	}

	//// iPortQueを返却
	// 纏めて送るようは、もう必要ないので、削除
	PoolBack_RecvFreeQue(mque_pRecvFree, p);		// 自身の解放と合わせて、iPortデータを返却

	//LOG(em_MSG),"[%s] カメラ単位=%d/%d", my_sThreadName, mque_pRecvFree[p->nIns]->GetCountFree(), mque_pRecvFree[p->nIns]->GetCountPool() );
}

//------------------------------------------
// 処理実行
// FRAME_DATA_ARRAY* p フレーム情報
//------------------------------------------
void ImgBufProc::Exec(FRAME_DATA_ARRAY* p)
{
	// 今回何も無しでよい
}