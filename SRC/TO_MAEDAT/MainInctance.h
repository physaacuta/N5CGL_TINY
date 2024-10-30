// *********************************************************************************
//	メイン管理スレッド
//	[Ver]
//		Ver.01    2012/09/06
//
//	[メモ]
//		
// *********************************************************************************

#pragma once

// 疵検クラス インクルード
#include "..\..\Library\KizuLibMFC\CLASS\ThreadManager.h"				// ワーカースレッドマネージャー

// 各ワーカースレッド
#include "MainConst.h"
#include "PreProcManager.h"


using namespace KizuLib;


class MainInctance : public ThreadManager
{
private :
	//=============================
	// 本クラス受け渡し用クラス
	//=============================
	//// メール情報受け渡しキュー
	struct DELIVERY_MAIL {
		ENUM_MAEDATOPERATION	emNo;										// イベントNo
		COMMON_QUE				que;										// 付加情報
	};



//// 公開関数
public:
	MainInctance(void);
	virtual ~MainInctance(void);

	void Start() { SetEvent(m_evThStart); }								// スレッド開始 (MainInctanceの時だけはオーバーライド)

	BOOL Init();														// 初期化処理
	void Exit();														// 終了処理

	// 外部アクセス
	void SetDeliveryMail(ENUM_MAEDATOPERATION emNo, COMMON_QUE * que);		// スレッドキューにメールデータ登録


//// 公開変数
public:
	// 各ワーカースレッド
	PreProcManager*			gcls_PreProcManager;						// 前工程メイン管理クラス
	FFSendManager			gcls_FFSend;								// FF送信管理
	ParamManager			gcls_Param;									// パラメータ管理クラス
	PreDBManager*			gcls_pPreDB;								// 前工程実績情報DB登録管理
	PreDefectManager*       gcls_pPreDefect;							// 前工程編集クラス


//// メンバー関数
private:		
	int ThreadFirst();													// スレッド開始前処理 (スレッド実行直後(シグナルセット前)にコールバック)
	int ThreadLast() { return 0; }										// スレッド終了前処理 (終了シグナル後にコールバック)
    void ThreadEvent( int nEventNo );									// スレッドイベント発生
	

//// メンバー変数
private:
	// 同期制御
	ThreadQueue<DELIVERY_MAIL>	mque_DeliMail;							// メール情報受け渡しキュー
	HANDLE						m_evThStart;							// MainInctanceスレッドの実行要求




};
