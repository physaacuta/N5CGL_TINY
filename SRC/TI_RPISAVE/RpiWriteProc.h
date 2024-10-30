// *********************************************************************************
//	PRIファイル書込クラス
//	[Ver]
//		Ver.01    2010/03/04	初版
//
//	[メモ]
//		・
// *********************************************************************************

#pragma once

// 共通ヘッダー
#include "..\..\Include\KizuCommon.h"									// システム用インクルード


// 疵検クラス インクルード
#include "..\..\Library\KizuLibMFC\CLASS\ThreadManager.h"				// ワーカースレッドマネージャー

// ローカル
#include "MainConst.h"													// 自クラス定数ヘッダー
#include "ParamManager.h"												// パラメータ管理クラス
#include "RpiFileManager.h"
#include "FrameDataManager.h"											// フレームデータ管理クラス
#include "ProcessQueThread.h"											// ProcThread基底

// STL
#include <vector>														// リスト用
#include <algorithm>													// ソート用


//// 公開定数
class RpiWriteProc : public RpiFileManager,
					 public ProcessQueThread<DELIVERY_RPIWRITE, FrameDataManager>
{


//// 公開定数
public:


//// 公開関数
public:
	RpiWriteProc(int nIns, int nColor);
	virtual ~RpiWriteProc(void);

	void Reset();														// リセット
	void SetIsExec(bool bMode) { m_bIsExec = bMode; }					// 処理実行有無

	// プロパティ
	void SetParamMgr(ParamManager* pCls) { mcls_pParam = pCls; }		// パラメータ管理
	void SetQueRpiWrite(ThreadQueue<DELIVERY_RPIWRITE>* pQue){ mque_pRpiWrite = pQue; }	// RpiProcに渡すキュー
	void SetQueRpiMove(ThreadQueue<RPI_MOVE>* pQue) { mque_pRpiMove = pQue; }		//


	bool SetData_Setting();												// 設定情報 セット
	bool SetData_CoilInfo(FrameDataManager* p);							// コイル単位の設定情報 セット (コイル先端)
	bool SetData_CoilResult(FrameDataManager* p);						// コイル単位の実績情報 セット (コイル尾端)
	bool SetData_FrameInfo(FrameDataManager* p);						// フレーム単位の設定情報 セット
	

//// メンバー関数
private:
	int ThreadFirst();													// スレッド開始前処理 (スレッド実行直後(シグナルセット前)にコールバック)
	int ThreadLast();													// スレッド終了前処理 (終了シグナル後にコールバック)
    void ThreadEvent(int nEventNo);										// スレッドイベント発生
	
	// 処理
	virtual FrameDataManager* OnAddNewItem( DELIVERY_RPIWRITE* pIn );	// 実処理

	void Exec_WriteFrame(FrameDataManager* p);							// 書込み処理処理
	void Exec_FileClose(FrameDataManager* p=NULL);						// 終了処理


	//=========================================
	// 仮想関数
	// 今回追加
	virtual void		OnSetFileName(RpiHeader const& h, RPI_SETTING const& s, RPI_COIL_INFO const& c, CString* strName);	// ファイル名決定
	virtual void		OnRpiFileSave(char const* strPath, CString* strName);	// RPIファイル生成完了


//// メンバー変数
private:
	//// 受け渡しキュー
	ThreadQueue<DELIVERY_RPIWRITE>*		mque_pRpiWrite;					// RPIファイル書き込みへ受け渡しキュー	[in]
	ThreadQueue<RPI_MOVE>*				mque_pRpiMove;					// RPIファイル移動要求キュー通知	[out]

	//// 各インスタンス
	ParamManager*						mcls_pParam;					// パラメータ管理

	// 共通的な情報
	int									m_nIns;							// インスタンス番号 (0オリジン)
	int									m_nColor;						// 画像色番号 (0オリジン)
	bool								m_bIsExec;						// 実行許可時 true (撮像開始時true)
};
