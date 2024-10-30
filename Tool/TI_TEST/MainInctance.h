// *********************************************************************************
//	メイン管理スレッド
//	[Ver]
//		Ver.01    2007/01/30  vs2005 対応
//
//	[メモ]
//		
// *********************************************************************************

#pragma once

// 疵検クラス インクルード
#include "..\..\Library\KizuLibMFC\CLASS\ThreadManager.h"				// ワーカースレッドマネージャー
#include "..\..\Library\DefectLib\KizuToSend.h"							// 判定⇒統括間通信 クラス


// 各ワーカースレッド
#include "MainConst.h"													// 自クラス定数ヘッダー
#include "ParamManager.h"												// パラメータクラス
#include "TestManager.h"												// テストクラス


// 全長がらみ
#include "..\..\Library\DefectLib\SockOneToAnySend.h"					// 回転寿司形式の送信クラス
#include "..\..\Include\OutIF_HT_RAW.h"									// 全長定義
#include "CullManager.h"

using namespace KizuLib;


class MainInctance : public ThreadManager
{
//// メンバー定数
private :
	//=============================
	// 本クラス受け渡し用クラス
	//=============================
	//// メール情報受け渡しキュー
	struct DELIVERY_MAIL {
		ENUM_MAININCTANCE	emNo;										// イベントNo
		COMMON_QUE			que;										// 付加情報
	};

	static const int PARAM_TIMEOUT_AUTO_START = 10 * 1000;				// 検査開始 自動開始時間[ms]


//// 公開関数
public:
	MainInctance(void);
	virtual ~MainInctance(void);

	void Start() { SetEvent(m_evThStart); }								// スレッド開始 (MainInctanceの時だけはオーバーライド)

	BOOL Init();														// 初期化処理
	void Exit();														// 終了処理


	// 外部アクセス
	void SetDeliveryMail(ENUM_MAININCTANCE emNo, COMMON_QUE * que);		// スレッドキューにメールデータ登録
	void SetLogMgrCull(LogFileManager* pCls) { mcls_pLogCull = pCls; }	// 全長処理用ログクラス

	void	SetEvKenStart() { SetEvent(m_evKenStart); }					// シグナルセット 開始
	void	SetEvKenStop() { SetEvent(m_evKenStop); }					// シグナルセット 停止

	void	SetMen(int nMen)	 { m_nMen = nMen; }						// 検査面

	int		GetSysMode() { return m_nSysMode; }							// 検査状態(0:停止 1:開始)

	void	Send_TiTest_Rendo(int nCycle);								// 全長保存試験用ツール連動起動 開始／停止
	void	Send_TiTest_FrameNo(int nFrameNo, int nCycleTime);			// 全長保存試験用ツール連動起動 フレームNo初期化

//// 公開変数
public:

	// 各ワーカースレッド
	ParamManager			gcls_PrmMgr;								// パラメータクラス
	TestManager*			gcls_pTest;									// TestManagerインスタンス

	int						g_nNumCam;									// カメラ台数
	int						g_nCamStart;								// カメラ開始位置


//// メンバー関数
private:		
	// スレッド動作定義
	int ThreadFirst();													// スレッド開始前処理 (スレッド実行直後(シグナルセット前)にコールバック)
	int ThreadLast();													// スレッド終了前処理 (終了シグナル後にコールバック)
    void ThreadEvent(int nEventNo);										// スレッドイベント発生
	

//// メンバー変数
private:
	// 各ワーカースレッド
	LogFileManager*				mcls_pLogCull;							// 全長処理用ログクラス

	// 同期制御
	ThreadQueue<DELIVERY_MAIL>	mque_DeliMail;							// メール情報受け渡しキュー
	HANDLE						m_evThStart;							// MainInctanceスレッドの実行要求
	HANDLE						m_evKenStart;							// 検査開始		
	HANDLE						m_evKenStop;							// 検査停止		
	HANDLE						m_evTimerExec;							// 自動検査開始タイマーイベント


	// パラメータ
	int			 				m_nCamPair;								// 現在のカメラペア 1～ (0:バックアップ=割り当て無し)
	int							m_nMen;									// 現在の面 (-1:不明  0:表  1:裏)
	int							m_nNowScode[NUM_MEN];					// 現在の検査表面状態No(0:対象Scode)
	EM_DIV_HANTEI				m_emKindHt;								// 判定機能
	DWORD						m_nAppParam;							// APP異常項目パラメータ
	bool						m_bNgCam;								// 異常カメラ担当 true

	int							m_nSysMode;								// 検査状態(0:停止 1:開始)

//// 全長画像用
public:
	SockOneToAnySend*				gcls_pSockRpi[NUM_CAM_POS];	// 判定⇒全長処理間通信 クラス
	ThreadLoopQueue<HT_RPI_FDATA>*	gque_pSockRpi[NUM_CAM_POS];	// フレーム画像受け渡しキュー(回転寿司形式)	[LIB⇔AP間] ※ キューサイズをAPから指定したいため、ポインタにしておく
	CullManager*					gcls_pCull[NUM_CAM_POS];		// 間引きスレッド

	//SockOneToAnySend*				gcls_pSockRpi[FRAME_HT_SEND_PORT];	// 判定⇒全長処理間通信 クラス
	//ThreadLoopQueue<HT_RPI_FDATA>*	gque_pSockRpi[FRAME_HT_SEND_PORT];	// フレーム画像受け渡しキュー(回転寿司形式)	[LIB⇔AP間] ※ キューサイズをAPから指定したいため、ポインタにしておく
	//CullManager*					gcls_pCull[FRAME_HT_SEND_PORT];		// 間引きスレッド

private:
	void QueueAlloc(int idx, int n, int const* p);						// 全長処理送信用の回転寿司キュー確保
	void QueueFree();													// 全長処理送信用の回転寿司キュー解放


};
