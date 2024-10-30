// *********************************************************************************
//	検査用情報との紐付けクラス
//	幅方向の画像紐付けクラス
//	[Ver]
//		Ver.01    2010/03/04	初版
//
//	[メモ]
// *********************************************************************************

#pragma once

// 疵検クラス インクルード
#include "..\..\Library\KizuLibMFC\CLASS\ThreadManager.h"				// ワーカースレッドマネージャー
#include "..\..\Library\KizuLibMFC\CLASS\ThreadLoopQueue.h"				// 回転寿司形式のスレッドキュー


// ローカル
#include "MainConst.h"													// 自クラス定数ヘッダー
#include "ProcessQueThread.h"											// ProcThread基底

#include "ParamManager.h"												// パラメータ管理クラス

class CoilControlProc : public ThreadManager,
						public ProcessQueThread<FRAME_DATA_ARRAY, FRAME_DATA_ARRAY>
{

private:


	//=========================================
	// コイル情報格納エリア
	struct COIL_INFO_BUF {
		char							cKanriNo[SIZE_KIZUKEN_NO];
		char							cCoilNo[32];
		int								pcode[2];			// 1オリジン 
		int								scode[2];			// 1オリジン 

		DWORD							nChangeFno;			// 切替フレームNo
		int								mmFromHead;		// 溶接点からの距離[mm]

		// 内部セット
		EM_COMP_TYPE					emCompType;			// 画像圧縮タイプ
		int								nCompQuality;		// 圧縮品質
		bool							bDummyCoil;			// true:ダミーコイル
	};


//// 公開関数
public:
	CoilControlProc(int nIns);
	virtual ~CoilControlProc(void);


	void Reset();														// リセット
	void SetIsExec(bool bMode) { m_bIsExec = bMode; }					// 処理実行有無
	void SetNextCoil(COMMON_QUE const* p);								// 次コイル情報セット


	// プロパティ
	void SetParamMgr(ParamManager* pCls) { mcls_pParam = pCls; }		// パラメータ管理	
	void SetQueRecvFree(IQueue<HT_RPI_FDATA>* pQue) { mque_pRecvFree = pQue; }		// 途中返却用


	// 画面アクセス用
	char const* GetNextCoil() const {return m_NextCoil.cKanriNo;}


//// メンバー関数
private:
	int ThreadFirst();													// スレッド開始前処理 (スレッド実行直後(シグナルセット前)にコールバック)
	int ThreadLast();													// スレッド終了前処理 (終了シグナル後にコールバック)
    void ThreadEvent(int nEventNo);										// スレッドイベント発生

	// 処理
	void AddNewItem();													// 画像取得
	virtual FRAME_DATA_ARRAY* OnAddNewItem( FRAME_DATA_ARRAY* pIn ) {return NULL;};	// 実処理

	// コイル情報
	void SetDummeyCoil(COIL_INFO_BUF& typCoil);							// 次コイル情報エリアにダミーコイルをセット
	bool ChangeCoil(FRAME_DATA_ARRAY* p);								// コイル切替 (連続、バッチ 両方共通)

	// 連続ライン用
	void Exec_Len(FRAME_DATA_ARRAY* p);									// 連続ライン時の処理実行


//// メンバー変数
private:

	// 受け渡しキュー
	IQueue<HT_RPI_FDATA>*		mque_pRecvFree;					// [LIBに返すキュー] 
//	ThreadQueue<DELIVERY_MAIL>			mque_Mail;						// 外部情報受け渡しキュー


	// 各インスタンス
	ParamManager*						mcls_pParam;					// パラメータ管理

	// 共通的な情報
	int									m_nIns;							// インスタンス番号 (0オリジン)
	bool								m_bIsExec;						// 実行許可時 true (撮像開始時true)
	DWORD								m_nRecvFno;						// 受信フレームNo

	// コイル情報
	bool								m_bDebugNextWpd;				// 次コイル強制切替時 true
	COIL_INFO_BUF						m_NextCoil;						// 次コイル情報エリア

	// 現状態
	COIL_INFO_BUF						m_NowCoil;						// 現コイル情報エリア
	DWORD								m_nCFno;						// コイル先頭のフレームNo

};

