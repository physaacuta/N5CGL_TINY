// *********************************************************************************
//	メイン管理スレッド
//	[Ver]
//		Ver.01    2012/08/30
//
//	[メモ]
//		
// *********************************************************************************

#pragma once

// 疵検クラス インクルード
#include "..\..\Library\KizuLibMFC\CLASS\ThreadManager.h"				// ワーカースレッドマネージャー


// 各ワーカースレッド
#include "MainConst.h"
#include "SockManager.h"												// 通信制御ワーカースレッド
#include "SockDataExec.h"												// 送受信データ受け渡しクラス


using namespace KizuLib;


class MainInctance : public ThreadManager
{
private :
	//=============================
	// 本クラス受け渡し用クラス
	//=============================
	//// メール情報受け渡しキュー
	struct DELIVERY_MAIL {
		QUE_SEND_STAT		emNo;										// イベントNo
		COMMON_QUE			que;										// 付加情報
	};



//// 公開関数
public:
	MainInctance(void);
	virtual ~MainInctance(void);

	void Start() { SetEvent(m_evThStart); };							// スレッド開始 (MainInctanceの時だけはオーバーライド)

	BOOL Init();														// 初期化処理
	void Exit();														// 終了処理

	// 外部アクセス
	void SetDeliveryMail(QUE_SEND_STAT emNo, COMMON_QUE * que);			// スレッドキューにメールデータ登録


//// 公開変数
public:
	// 各ワーカースレッド
	SockManager*		gcls_pSock;										// 通信制御ワーカースレッド
	SockDataExec*		gcls_pData;										// 送受信データ受け渡しクラス



	// 受け渡しキュー
	ThreadQueue<DELIVERY_MAIL>	mque_DeliMail;							// メール情報受け渡しキュー

//// メンバー関数
private:		
	int ThreadFirst();													// スレッド開始前処理 (スレッド実行直後(シグナルセット前)にコールバック)
	int ThreadLast() { return 0; }										// スレッド終了前処理 (終了シグナル後にコールバック)
    void ThreadEvent(int nEventNo);										// スレッドイベント発生
	

//// メンバー変数
private:
	HANDLE						m_evThStart;							// MainInctanceスレッドの実行要求


};
