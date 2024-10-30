// *********************************************************************************
//	フレーム画像 バッファリング管理
//	[Ver]
//		Ver.01    2010/03/04	初版
//
//	[メモ]
// *********************************************************************************

#pragma once

// 疵検クラス インクルード
#include "..\..\Library\KizuLibMFC\CLASS\ThreadManager.h"				// ワーカースレッドマネージャー
#include "..\..\Library\KizuLibMFC\Class\ThreadSyncManager.h"			// 並列スレッド間順序保障制御クラス
#include "..\..\Library\KizuLibMFC\CLASS\ThreadLoopQueue.h"				// 回転寿司形式のスレッドキュー
#include "..\..\Library\KizuLib\CLASS\KizuBitmapManager.h"				// ビットマップ画像クラス

// ローカル
#include "MainConst.h"													// 自クラス定数ヘッダー
#include "ParamManager.h"												// パラメータ管理クラス
#include "FrameDataManager.h"											// フレームデータ管理クラス
#include "ProcessQueThread.h"											// ProcThread基底

using namespace KizuLib;

class ImgBufProc : public ThreadManager,
					public ProcessQueThread<FRAME_DATA_ARRAY, FrameDataManager>
{
//// 公開関数
public:
	ImgBufProc(void);
	virtual ~ImgBufProc(void);

	void Reset();														// リセット
	void SetIsExec(bool bMode);											// 処理実行有無
	void Alloc();
	void Free();


	// プロパティ
	void SetParamMgr(ParamManager* pCls) { mcls_pParam = pCls; }		// パラメータ管理	

	void SetQueOutSkip(ThreadQueueImpl<FrameDataManager>* pQue){ mque_pOutSkip = pQue; }		// DispProcに渡すキュー (未保存時)
	void SetQueRecvFree(IQueue<HT_RPI_FDATA>** pQue) { mque_pRecvFree = pQue; }		// 途中返却用

	void SetDetailLog( KizuLib::LogFileManager *log) { mcls_detail = log; }

//// メンバー関数
private:
	int ThreadFirst();													// スレッド開始前処理 (スレッド実行直後(シグナルセット前)にコールバック)
	int ThreadLast();													// スレッド終了前処理 (終了シグナル後にコールバック)
    void ThreadEvent(int nEventNo);										// スレッドイベント発生
	
	// 処理
	void AddNewItem();													// 画像取得
	virtual FrameDataManager* OnAddNewItem( FRAME_DATA_ARRAY* pIn ) {return NULL;};	// 実処理
	void Exec(FRAME_DATA_ARRAY* p);										// 処理実行

	KizuLib::LogFileManager *			mcls_detail;

//// メンバー変数
private:
	//// 受け渡しキュー
	ThreadQueueImpl<FrameDataManager>*	mque_pOutSkip;					// [未保存時] 画像処理スキップ先
	IQueue<HT_RPI_FDATA>**		mque_pRecvFree;					// [LIBに返すキュー] 


	//// 各インスタンス
	ParamManager*						mcls_pParam;					// パラメータ管理

	// 共通的な情報
	bool								m_bIsExec;						// 実行許可時 true (撮像開始時true)
};
