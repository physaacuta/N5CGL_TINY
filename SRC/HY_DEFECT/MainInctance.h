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
#include "..\..\Library\DefectLib\KizuDefaultRecv.h"					// ソケット関連(PC間)クラス

// 疵検 インクルード
#include "..\..\include\LineCommon.h"									// 疵検共通ヘッダー


// 各ワーカースレッド
#include "MainConst.h"
#include "ParamManager.h"												// パラメータ管理クラス
#include "DataSetManager.h"												// 共有メモリデータセットクラス
#include "FileDeleteManager.h"											// ファイル削除管理クラス
#include "FileSaveManager.h"											// ファイル保存管理クラス
#include "MaeManager.h"													// 前工程疵情報管理クラス

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
	KizuDefaultRecv*		gcls_pRecv;									// 受信クラス
	ParamManager*			gcls_pParam;								// パラメータ管理クラス
	DataSetManager*			gcls_pData;									// データセットクラス
	FileDeleteManager		gcls_FDele;									// ファイル削除管理クラス
	FileSaveManager*		gcls_pFSave[THREAD_IMG_SAVE];				// ファイル保存管理クラス

	MaeManager*				gcls_pMaeMgr;								// 前工程疵情報管理クラス


	// 受け渡しキュー
	ThreadQueue<SOCK_BASE_HEAD>					gque_OpRecv;			// 統括→表示送信通知のキュー
	ThreadQueue<FileSaveManager::TYP_SAVE_IMG>	gque_ImgSave;			// 画像保存依頼のキュー (代表疵)
	ThreadQueue<FileSaveManager::TYP_SAVE_IMG>	gque_ImgSaveLow;		// 画像保存依頼のキュー (リスト部で隠れる疵)

	ThreadQueue<MaeManager::TYP_MAE_REQUEST>	gque_MaeRequest;		// 前工程管理への依頼キュー
// 2023.11.17 最終工程検査装置高機能化その４ -------------------->>>>>
	ThreadQueue<DataSetManager::TYP_DATASET_REQMAE>	gque_DataRequest;	//データセットへの前工程疵編集依頼キュー
// 2023.11.17 最終工程検査装置高機能化その４ <<<<<--------------------
// 2024.07.26 Kouno 2CGL検査装置φω精度向上機能導入(長周期) ---------------->>>>>
	ThreadQueue<DataSetManager::TYP_DATASET_REQLONGPD>	gque_DataRequestLongPd;		//データセットへの長周期情報編集依頼キュー
// 2024.07.26 Kouno 2CGL検査装置φω精度向上機能導入(長周期) <<<<<----------------

//// メンバー関数
private:		
	int ThreadFirst();													// スレッド開始前処理 (スレッド実行直後(シグナルセット前)にコールバック)
	int ThreadLast() { return 0; }										// スレッド終了前処理 (終了シグナル後にコールバック)
    void ThreadEvent(int nEventNo);										// スレッドイベント発生
	


	//// 外部タスク通知
	void Send_Hy_Param(COMMON_QUE* pQue);								// 他タスクにパラメータ変更通知を送信
	void Send_Hy_Init();												// 他タスクに初期化通知を送信



//// メンバー変数
private:
	// 同期制御
	ThreadQueue<DELIVERY_MAIL>	mque_DeliMail;							// メール情報受け渡しキュー
	HANDLE						m_evThStart;							// MainInctanceスレッドの実行要求
};
