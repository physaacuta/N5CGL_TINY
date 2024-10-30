// *********************************************************************************
//	PRIファイル制御クラス
//	[Ver]
//		Ver.01    2010/03/04	初版
//
//	[メモ]
//		・
// *********************************************************************************

#pragma once

// 疵検クラス インクルード
#include "..\..\Library\KizuLibMFC\CLASS\ThreadManager.h"				// ワーカースレッドマネージャー
#include "..\..\Library\KizuLibMFC\Class\ThreadSyncManager.h"			// 並列スレッド間順序保障制御クラス

// ローカル
#include "MainConst.h"													// 自クラス定数ヘッダー
#include "ParamManager.h"												// パラメータ管理クラス
#include "FrameDataManager.h"											// フレームデータ管理クラス
#include "RpiWriteProc.h"												// RPIファイル書込クラス
#include "ProcessQueThread.h"											// ProcThread基底

using namespace KizuLib;

class RpiControlProc : public ThreadManager,
						public ProcessQueThread<FrameDataManager, DELIVERY_RPIWRITE>
{
//// 公開関数
public:
	RpiControlProc(void);
	virtual ~RpiControlProc(void);

	bool Init();														// 検査開始時 準備
	void ClosingRpi();													// RPIファイルクローズ要求
	void ClosingRpi(int nIns);											// RPIファイルクローズ要求
	void Reset();														// リセット

	// プロパティ
	void SetParamMgr(ParamManager* pCls) { mcls_pParam = pCls; }		// パラメータ管理	
	void SetRpiWProc(RpiWriteProc* pCls[][MAX_COLOR_NUM]) { for(int ii=0; ii<MAX_INS; ii++) for(int jj=0; jj<MAX_COLOR_NUM; jj++) mcls_pRpiWProc[ii][jj] = pCls[ii][jj]; }	// RPIファイル書込クラス

	virtual void SetQueOut(ThreadQueueImpl<DELIVERY_RPIWRITE>* pQue[][MAX_COLOR_NUM]){ for(int ii=0; ii<MAX_INS; ii++) for(int jj=0; jj<MAX_COLOR_NUM; jj++) mque_pOut[ii][jj] = pQue[ii][jj]; }	// RpiProcに渡すキュー

//// メンバー関数
private:
	int ThreadFirst();													// スレッド開始前処理 (スレッド実行直後(シグナルセット前)にコールバック)
	int ThreadLast();													// スレッド終了前処理 (終了シグナル後にコールバック)
    void ThreadEvent(int nEventNo);										// スレッドイベント発生
	
	// 処理
	void AddNewItem();													// 画像取得
	virtual DELIVERY_RPIWRITE* OnAddNewItem( FrameDataManager* pIn ) {return NULL;};	// 実処理


//// メンバー変数
private:
	//// 受け渡しキュー
	ThreadQueueImpl<DELIVERY_RPIWRITE>*	mque_pOut[MAX_INS][MAX_COLOR_NUM]; // RPIファイル書き込みへ受け渡しキュー	[out] (配列のポインタ)


	//// 各インスタンス
	ParamManager*						mcls_pParam;					// パラメータ管理
	RpiWriteProc*						mcls_pRpiWProc[MAX_INS][MAX_COLOR_NUM];		// RPIファイル書込クラス
};
