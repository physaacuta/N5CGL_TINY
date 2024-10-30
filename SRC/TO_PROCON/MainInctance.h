// *********************************************************************************
//	メイン管理スレッド
//	[Ver]
//		Ver.01    2007/01/30  vs2005 対応
//
//	[メモ]
//		
// *********************************************************************************

#pragma once


// インクルード
#include <process.h>

// 疵検 インクルード
#include "..\..\include\KizuCommon.h"									// 疵検共通ヘッダー
#include "..\..\include\LineCommon.h"									// システム共通ヘッダー
#include "..\..\include\OutInterface.h"									// 外部機器用ヘッダー
#include "..\..\Library\KizuLib\KizuLib.h"								// 疵検ライブラリヘッダー

// 疵検クラス インクルード
#include "..\..\Library\KizuLib\CLASS\KizuMacro.h"						// 疵検マクロ
#include "..\..\Library\KizuLibMFC\CLASS\ThreadManager.h"				// ワーカースレッドマネージャー
#include "..\..\Library\KizuLibMFC\CLASS\LogFileManager.h"				// ログマネージャー

// 疵検ライブラリ インクルード
#include "..\..\Library\KizuODBC\OdbcBase.h"							// ODBC操作クラス


// 各ワーカースレッド
#include "MainConst.h"
#include "SockDataExec.h"												// 通信制御ワーカースレッド

using namespace KizuLib;
//using namespace KizuSock3;
//using namespace KizuODBC;

class MainInctance : public ThreadManager
{
private:
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

	void Start() { SetEvent(m_evThStart); };							// スレッド開始

	BOOL Init();														// 初期化処理
	void Exit();														// 終了処理

	// 外部アクセス
	void SetDeliveryMail(ENUM_MAININCTANCE emNo, COMMON_QUE * que);		// スレッドキューにメールデータ登録

//// 公開変数
public:
	// 各ワーカースレッド
	SockDataExec*		gcls_pSock;										// 通信制御ワーカースレッド

																		// 受け渡しキュー
	ThreadQueue<Delivery>		gque_DeliSend;							// 送信依頼用キュー(コイル情報要求伝文/テスト伝文)



//// メンバー関数
private:
	int ThreadFirst();													// スレッド開始前処理 (スレッド実行直後(シグナルセット前)にコールバック)
	int ThreadLast() { return 0; }										// スレッド終了前処理 (終了シグナル後にコールバック)
	void ThreadEvent(int nEventNo);										// スレッドイベント発生



//// メンバー変数
private:
	// 受け渡しキュー
	ThreadQueue<DELIVERY_MAIL>	mque_DeliMail;							// メール情報受け渡しキュー

	// 同期シグナル
	HANDLE	m_evThStart;												// MainInctanceスレッドの実行要求

	// その他もろもろ


};
