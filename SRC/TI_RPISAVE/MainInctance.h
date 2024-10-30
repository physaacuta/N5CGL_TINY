// *********************************************************************************
//	メイン管理スレッド
//	[Ver]
//		Ver.01    2007/01/30  vs2005 対応
//
//	[メモ]
//		
// *********************************************************************************

#pragma once

// 疵検ライブラリ インクルード
#include "..\..\Library\KizuLibMFC\CLASS\ThreadManager.h"				// ワーカースレッドマネージャー
#include "..\..\Library\KizuLibMFC\CLASS\ThreadLoopQueue.h"				// 回転寿司形式のスレッドキュー
#include "..\..\Library\KizuLibMFC\Class\ThreadSyncManager.h"			// 並列スレッド間順序保障制御クラス
#include "..\..\Library\KizuLibMFC\Class\ImageWindow.h"					// 画像表示制御クラス

#include "..\..\Library\DefectLib\SockOneToAnyRecv.h"					// 回転すし形式受信


// 基底
#include "ProcessQueThread.h"
#include "ProcessQueImpl.h"
#include "FrameDataManager.h"											// フレームデータ管理クラス

// 各ワーカースレッド
#include "MainConst.h"
#include "ParamManager.h"
#include "StatusManager.h"												// ステータス制御クラス
#include "HddManager.h"													// HDD管理クラス


//// Proc関係
#include "ImgSyncProc.h"												// 幅方向板把握
#include "CoilControlProc.h"											// コイル管理クラス
#include "ImgUnionProc.h"												// 幅方向画像連結クラス
#include "ImgBufProc.h"													// フレーム画像 バッファリング管理クラス
#include "ImgCompProc.h"												// 画像圧縮クラス
#include "RpiControlProc.h"												// RPIファイル制御クラス
#include "RpiWriteProc.h"												// RPIファイル書込クラス
#include "DispProc.h"													// 結果表示クラス
#include "CudaAlloc.h"													// CUDAメモリ管理クラス


using namespace KizuLib;


class MainInctance : public ThreadManager
{
private :
	//=============================
	// 本クラス受け渡し用クラス
	//=============================
	//// メール情報受け渡しキュー
	struct DELIVERY_MAIL {
		ENUM_MAININCTANCE	emNo;										// イベントNo
		COMMON_QUE			que;										// 付加情報
	};



//// 公開関数
public:
	MainInctance(void);
	virtual ~MainInctance(void);

	void Start() { SetEvent(m_evThStart); }								// スレッド開始 (MainInctanceの時だけはオーバーライド)

	BOOL Init();														// 初期化処理
	void Exit();														// 終了処理

	// 外部アクセス
	void SetDeliveryMail(ENUM_MAININCTANCE emNo, COMMON_QUE * que);		// スレッドキューにメールデータ登録


//// 公開変数
public:
	// 各ワーカースレッド
	ParamManager*						gcls_pParam;					// パラメータ管理クラス
	StatusManager						gcls_Status;					// ステータス制御クラス
	ImageWindow							gcls_ImgWin;					// 画像表示クラス
	HddManager							gcls_Hdd;						// HDD管理クラス


	KizuLib::LogFileManager *			gcls_InsLog[MAX_INS];
	KizuLib::LogFileManager *			gcls_InsLog_ImgBuf_Union;
	// 画像の受け渡し順番に記述
	SockOneToAnyRecv*					gcls_pSock[MAX_INS][NUM_CAM_POS];		// 画像取込クラス
	ImgSyncProc*						gcls_pSyncProc[MAX_INS];				// 幅方向紐付け管理クラス	
	CoilControlProc*					gcls_pCoilProc[MAX_INS];				// コイル管理クラス
	ImgUnionProc*						gcls_pUnionProc[MAX_THREAD_MULTI_P];	// 幅方向画像連結クラス (並列処理のため)
	ImgBufProc*							gcls_pImgBufProc;						// フレーム画像 バッファリング管理クラス
	ImgCompProc*						gcls_pCompProc[MAX_THREAD_MULTI_P];		// 画像圧縮クラス (並列処理のため)
	RpiControlProc*						gcls_pRpiCProc;							// PRIファイル制御クラス (単一クラス) この中で、RPI書込クラスをいっぱい生成する
	RpiWriteProc*						gcls_pRpiWProc[MAX_INS][MAX_COLOR_NUM];	// PRIファイル書込クラス
	DispProc*							gcls_pDispProc;							// 結果表示クラス

	LacManager							gcls_Lac;								// 高度圧縮クラス
	CudaAlloc*							gcls_pCudaAlloc;						// CUDAメモリ管理クラス


	//---------------------------
	// キュー
	
	//ダミーを判定して、SockRecvのFree側に返すようのプロキシキュー。
	std::vector< IQueue<HT_RPI_FDATA> *> que_pSockRecv_Free;

	// これで大きなループを構築。 IQueue サポートクラス
	ThreadLoopQueueImpl<HT_RPI_FDATA>*		gque_pSockRecv[MAX_INS];	// 判定からのRAW画像受け渡しキュー(回転寿司形式)	[LIB⇔AP間] (LIB→→･･･→ChainProc→LIB)
	ThreadLoopQueueImpl<FrameDataManager>*	gque_pFrame;				// フレーム画像受け渡しキュー(回転寿司形式)	[AP⇔AP間] (ImgUnionProc→･･･→DispProc)

	// 1カメラ単位
	ThreadQueueImpl<FRAME_DATA_ARRAY>*		gque_pCoilC[MAX_INS];		// コイル管理スレッドへ  [PlgSyncProc⇒CoilControlProc→ImgUnionProc] バッチラインのみ
	ThreadSyncManagerImpl<FRAME_DATA_ARRAY>*gque_pUnion;				// フレーム画像受け渡しスレッド順序保障ｸﾗｽ  [CoilControlProc⇒ImgUnionProc→ChainProc]

	// 連結画像単位
	ThreadSyncManagerImpl<FrameDataManager>*gque_pComp;					// フレーム画像受け渡しスレッド順序保障ｸﾗｽ  [ImgBufProc⇒ImgCompProc→RpiControlProc]
	ThreadQueueImpl<DELIVERY_RPIWRITE>*		gque_pRpiWrite[MAX_INS][MAX_COLOR_NUM];	// フレーム画像受け渡しキュー	[RpiControlProc⇒RpiWriteProc]
	ThreadQueueImpl<FrameDataManager>		gque_Disp;					// フレーム画像受け渡しキュー				[RpiWriteProc→DispProc]

	// その他
	ThreadQueue<RPI_MOVE>				gque_RpiMove;					// RPIファイル移動要求キュー				[RpiWriteProc→HddManager]


//// メンバー関数
private:		
	int ThreadFirst();													// スレッド開始前処理 (スレッド実行直後(シグナルセット前)にコールバック)
	int ThreadLast() { return 0; }										// スレッド終了前処理 (終了シグナル後にコールバック)
    void ThreadEvent(int nEventNo);										// スレッドイベント発生
	
	// 小物
	void GetInsLoop(int nMode, int& ii_s, int& ii_e)					// インスタンスNo指定時の対象算出
	{
		if(-1==nMode) { ii_s=0; ii_e=gcls_pParam->GetMain().nIns-1; }
		else		  { ii_s=ii_e=nMode;}
	}

	// シーケンス制御
	int	 DetectStart(COMMON_QUE* que);									// 検査開始処理
	int  DetectStop(int mode, int ansRetc);								// 検査停止処理



//// メンバー変数
private:
	// 同期制御
	ThreadQueue<DELIVERY_MAIL>	mque_DeliMail;							// メール情報受け渡しキュー
	HANDLE						m_evThStart;							// MainInctanceスレッドの実行要求
};
