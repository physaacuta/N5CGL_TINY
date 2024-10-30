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
#include "..\..\Library\KizuLibMFC\CLASS\ThreadManager.h"						// ワーカースレッドマネージャー

// 疵検ライブラリ インクルード
#include "..\..\Library\KizuODBC\OdbcBase.h"									// ODBC操作クラス

// 各ワーカースレッド
#include "MainConst.h"															// 自クラス定数ヘッダー
#include "MakeProManager.h"														// プロコン検査実績作成管理クラス
#include "MakeFFManager.h"														// ＦＦ検査実績作成管理クラス

using namespace KizuLib;

class MainInstance : public ThreadManager
{
private :
	//=============================
	// 本クラス受け渡し用クラス
	//=============================
	//// メール情報受け渡しキュー
	struct DELIVERY_MAIL
	{
		ENUM_MAININSTANCE		emNo;											// イベントNo
		COMMON_QUE				que;											// 付加情報
	};

//// 公開関数
public:
	MainInstance(void);
	virtual ~MainInstance(void);

	void Start() { SetEvent(m_evThStart); };									// スレッド開始 (MainInstanceの時だけはオーバーライド)

	BOOL Init();																// 初期化処理
	void Exit();																// 終了処理
	
	//// 外部アクセス
	void SetDeliveryMail(ENUM_MAININSTANCE emNo, COMMON_QUE * que);				// スレッドキューにメールデータ登録

//// 公開変数
public:
	//// 各ワーカースレッド
	MakeProManager				gcls_Pro;										// プロコン検査実績作成管理クラス
	MakeFfManager				gcls_FF;										// ＦＦ検査実績作成管理クラス

//// メンバー関数
private:		
	int ThreadFirst();															// スレッド開始前処理 (スレッド実行直後(シグナルセット前)にコールバック)
	int ThreadLast() { return 0;};												// スレッド終了前処理 (終了シグナル後にコールバック)
    void ThreadEvent(int nEventNo);												// スレッドイベント発生
	
//// メンバー変数
private:
	// 同期制御
	ThreadQueue<DELIVERY_MAIL>	mque_DeliMail;									// メール情報受け渡しキュー
	HANDLE						m_evThStart;									// MainInstanceスレッドの実行要求
};
