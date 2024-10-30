// *********************************************************************************
//	前工程編集クラス
//	[Ver]
//		Ver.01    2012/09/06
//
//	[メモ]
//		
// *********************************************************************************

#pragma once

// 疵検クラス インクルード
#include "..\..\Library\KizuLibMFC\CLASS\ThreadManager.h"				// ワーカースレッドマネージャー
#include "..\..\Library\KizuLibMFC\Class\CreateSQL.h"					// SQL文生成補助クラス
#include "..\..\Library\DefectLib\DBSaveManager.h"						// DB書込み基本クラス
#include "..\..\Library\KizuODBC\OdbcBase.h"							// DB操作クラス

// ローカルのクラス
#include "MainConst.h"
#include "PreDBManager.h"

using namespace KizuLib;
using namespace KizuODBC;
class PreDefectManager : public ThreadManager
{
public:
	PreDefectManager();
	~PreDefectManager(void);

	void SetGetData() {	SetEvent(m_evGetData); }	;					// 前工程実績編集処理要求
	void SetPreDBManager(PreDBManager* pCls) {mcls_pPreDB = pCls;}		// 前工程実績情報DB登録管理クラス

private:

	int ThreadFirst();													// スレッド開始前処理 (スレッド実行直後(シグナルセット前)にコールバック)
	int ThreadLast();													// スレッド終了前処理 (終了シグナル後にコールバック)
    void ThreadEvent( int nEventNo );									// スレッドイベント発生
	
	static const int PREPROC_TIMER = 60*60*1000;						// 定周期処理タイマーインターバル[ms]

	// シーケンス制御
	void PreDefectMain();												// 前工程実績編集メイン処理

	//// 各インスタンス
	PreDBManager*			mcls_pPreDB;								// 前工程実績情報DB登録管理

	// シグナル
	HANDLE m_evGetData;													// 前工程情報取得 
	HANDLE m_evTimeCycle;												// ヘルシー

	PreDBManager::DeliveryPreDB  m_PreDeliWk;							// 前回スレッド送信

};
