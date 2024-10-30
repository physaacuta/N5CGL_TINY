#include "StdAfx.h"
#include "FileSaveManager.h"

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
FileSaveManager::FileSaveManager(void) :
mque_pImgSave(NULL),
mque_pImgSaveLow(NULL),
ThreadManager("FileSaveMgr")
{
	//// ハンドル生成
	m_evClearQue  = CreateEvent(NULL, FALSE, FALSE, NULL);
}

//------------------------------------------
// デストラクタ
//------------------------------------------
FileSaveManager::~FileSaveManager(void)
{
	//// ハンドル解放
	CloseHandle(m_evClearQue);
}


//------------------------------------------
// スレッド開始前処理 (スレッド実行直後(シグナルセット前)にコールバック)
//------------------------------------------
int FileSaveManager::ThreadFirst()
{
	_ASSERT(NULL != mque_pImgSave);

	//// イベント設定
	HANDLE hArray[] = { m_evClearQue,
						mque_pImgSave->g_evSem 
						};
	SetEventNum(sizeof(hArray)/sizeof(HANDLE), hArray);

	// 追加
	if(NULL != mque_pImgSaveLow) this->AddEventNum(1, &mque_pImgSaveLow->g_evSem );
	return 0;
}

//------------------------------------------
// スレッド終了前処理 (終了シグナル後にコールバック)
//------------------------------------------
int FileSaveManager::ThreadLast()
{
	// 全キュー開放
	if(NULL != mque_pImgSave)	 AllQueFree(mque_pImgSave);
	if(NULL != mque_pImgSaveLow) AllQueFree(mque_pImgSaveLow);
	return 0;
}

//------------------------------------------
// スレッドイベント発生
// int nEventNo イベントNo (0オリジン)
//------------------------------------------
void FileSaveManager::ThreadEvent(int nEventNo)
{
	////// シグナルの条件
	enum {	EV_CLEAR = 0,					// 未処理キューを全削除
			EV_QUE,							// 画像保存依頼キュー (代表)
			EV_QUE_LOW						// 画像保存依頼キュー (リスト部で隠れる疵)
	};

	TYP_SAVE_IMG* pDeli = NULL;				// 受け渡し情報

	////// シグナル条件分け
	switch (nEventNo) {

//-----------------------------------------------------------------------------------------------
	case EV_CLEAR: 					// 未処理キューを全削除
		if(NULL != mque_pImgSave)	 AllQueFree(mque_pImgSave);
		if(NULL != mque_pImgSaveLow) AllQueFree(mque_pImgSaveLow);
		return;

//-----------------------------------------------------------------------------------------------
	case EV_QUE: 					// 画像保存依頼キュー (代表)
		pDeli = mque_pImgSave->GetFromHead(); 
		
		// 画像ファイル書き込み
		SaveImg(pDeli);

		// 開放
		for(int ii=0; ii<pDeli->nSetCnt; ii++ ) { 
			//SAFE_DELETE(pDeli->pImg[ii]);
			VirtualFree(pDeli->pImg[ii], 0, MEM_RELEASE);
		}
		delete pDeli;
		return;

//-----------------------------------------------------------------------------------------------
	case EV_QUE_LOW: 					// 画像保存依頼キュー (リスト部で隠れる疵)
		pDeli = mque_pImgSaveLow->GetFromHead(); 
		
		// 画像ファイル書き込み
		SaveImg(pDeli);

		// 開放
		for(int ii=0; ii<pDeli->nSetCnt; ii++ ) { 
			if(NULL == pDeli->pImg[ii] ) continue;
			//SAFE_DELETE(pDeli->pImg[ii]);
			VirtualFree(pDeli->pImg[ii], 0, MEM_RELEASE);
		}
		delete pDeli;
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
// 全キュー開放
// ThreadQueue<TYP_SAVE_IMG>* pQue キュークラス
//------------------------------------------
void FileSaveManager::AllQueFree(ThreadQueue<TYP_SAVE_IMG>* pQue)
{
	TYP_SAVE_IMG* pDeli = NULL;				// 受け渡し情報

	// 受け渡しキューに格納されている項目を全て開放
	while(true) {
		pDeli = pQue->GetFromHead(0);
		if( pDeli == NULL ) break;

		// 開放
		for(int ii=0; ii<pDeli->nSetCnt; ii++ ) { 
			if(NULL == pDeli->pImg[ii] ) continue;
//			SAFE_DELETE(pDeli->pImg[ii]);
			VirtualFree(pDeli->pImg[ii], 0, MEM_RELEASE);
		}
		delete pDeli;
	}
}

//------------------------------------------
// 画像ファイル書き込み
// TYP_SAVE_IMG* p 受け渡し情報
//------------------------------------------
void FileSaveManager::SaveImg(TYP_SAVE_IMG* p)
{
	
	// 画像作成
	//std::ofstream	stream;
	for(int ii=0; ii<p->nSetCnt; ii++) {
		if( 0 >= p->nSize[ii]) continue;

		//// 通常
		//stream.open(p->cPath[ii], std::ios::out | std::ios::trunc | std::ios::binary);
		//stream.write((char const*)p->pImg[ii], p->nSize[ii]);
		//stream.close();

		// ファイルコピー
		FILE *fpw = fopen(p->cPath[ii], "wb");
		if(NULL == fpw) {
			LOG(em_ERR), "[%s] 画像読み込み失敗(%d, %s)", my_sThreadName, errno, strerror(errno));
			return;
		}
		fwrite((char const*)p->pImg[ii], 1, p->nSize[ii], fpw);
		fclose(fpw);


		//// 画像パスセット (これが無いと、パスを先にセットした後で画像保存となってしまうため)
		if( NULL != p->pMemPath[ii] ) {

			char * pOldFile = p->pMemPath[ii];
			if ( NULL != pOldFile[0] ) {
				// 既にパスが登録されていたら、そのファイルは削除しておく
				DeleteFile(p->pMemPath[ii]);
			}

			strcpy(p->pMemPath[ii], p->cPath[ii]);
		}

	}
}

