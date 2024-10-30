// *********************************************************************************
//	フレーム画像 幅方向連結処理クラス
//	[Ver]
//		Ver.01    2010/03/04	初版
//
//	[メモ]
//		・面単位で処理
// *********************************************************************************

#pragma once

// 共通ヘッダー
#include "..\..\Include\KizuCommon.h"									// システム用インクルード


// 疵検クラス インクルード
#include "..\..\Library\KizuLibMFC\CLASS\ThreadManager.h"				// ワーカースレッドマネージャー
#include "..\..\Library\KizuLibMFC\Class\ThreadSyncManager.h"			// 並列スレッド間順序保障制御クラス


// ローカル
#include "MainConst.h"													// 自クラス定数ヘッダー
#include "StatusManager.h"												// ステータス制御クラス
#include "ParamManager.h"												// パラメータ管理クラス
#include "FrameDataManager.h"											// フレームデータ管理クラス
#include "ProcessQueThread.h"											// ProcThread基底

#include "..\..\Library\KizuLib\Class\KizuBitmapManager.h"


using namespace KizuLib;

class ImgUnionProc : public ThreadManager,
					 public ProcessQueThread<FRAME_DATA_ARRAY, FRAME_DATA_ARRAY>
{
//// 公開関数
public:
	ImgUnionProc(int nId);
	virtual ~ImgUnionProc(void);

	void Reset();														// リセット
	void SetIsExec(bool bMode) { m_bIsExec = bMode; }					// 処理実行有無


	// プロパティ
	void SetStatusMgr(StatusManager* pCls) { mcls_pStatus = pCls; }		// ステータス管理
	void SetParamMgr(ParamManager* pCls) { mcls_pParam = pCls; }		// パラメータ管理

	void SetQueFrame(ThreadLoopQueue<FrameDataManager>* pQue) { mque_pFrame = pQue; }	// 自分が最初に取得するキュー
	void SetQueComp (ThreadSyncManager<FrameDataManager>* pQue){ mque_pComp = pQue; }	// キュー (数を取得したいだけ)

	void SetDetailLog( KizuLib::LogFileManager *log) { mcls_detail = log; }

//// メンバー関数
private:
	int ThreadFirst();													// スレッド開始前処理 (スレッド実行直後(シグナルセット前)にコールバック)
	int ThreadLast();													// スレッド終了前処理 (終了シグナル後にコールバック)
    void ThreadEvent(int nEventNo);										// スレッドイベント発生
	

	// 処理
	virtual FRAME_DATA_ARRAY* OnAddNewItem( FRAME_DATA_ARRAY* pIn );	// 実処理
	void Exec(FRAME_DATA_ARRAY* p);										// 処理実行
	void Exec_Union(FRAME_DATA_ARRAY* p, bool bSkip);					// 画像連結

	void Exec_Kido(FRAME_DATA_ARRAY* p);								// 輝度補正

	KizuLib::LogFileManager *			mcls_detail;


//// メンバー変数
private:

	// 受け渡しキュー
	ThreadSyncManager<FrameDataManager>*mque_pComp;						// フレーム画像受け渡しスレッド順序保障ｸﾗｽ  [データ参照用だけ]
	ThreadLoopQueue<FrameDataManager>*	mque_pFrame;					// フレーム画像受け渡しキュー(回転寿司形式)	[取得]


	// 各インスタンス
	StatusManager*						mcls_pStatus;					// ステータス管理
	ParamManager*						mcls_pParam;					// パラメータ管理

	// 共通的な情報
	bool								m_bIsExec;						// 実行許可時 true (撮像開始時true)
	static const  BYTE							*m_BlankData;
};
