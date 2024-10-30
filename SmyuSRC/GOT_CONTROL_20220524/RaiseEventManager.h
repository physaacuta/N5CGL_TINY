#pragma once

// 疵検クラス インクルード
#include "..\..\Library\KizuLibMFC\CLASS\ThreadManager.h"				// ワーカースレッドマネージャー


// ローカル
#include "MainConst.h"													// 自クラス定数ヘッダー
#include "PlgControlManager.h"
#include "PioControlManager.h"


class RaiseEventManager :	public ThreadManager
{

//// 公開関数
public:
	RaiseEventManager(void);
	virtual ~RaiseEventManager(void);

	void SetPlgMgr(PlgControlManager* pCls) { mcls_pPlg = pCls;};		// ステータス制御クラスセット
	void SetPioMgr(PioControlManager* pCls) { mcls_pPio = pCls;};		// ステータス制御クラスセット


	//// イベント発生
	void RaiseEvents(EM_DIV_KIND emEvent, int nEventSub, BYTE* pEvent);	// イベント発生


//// メンバー関数
private:
	int ThreadFirst() {return 0;};										// スレッド開始前処理 (スレッド実行直後(シグナルセット前)にコールバック)
	int ThreadLast() {return 0;};										// スレッド終了前処理 (終了シグナル後にコールバック)
    void ThreadEvent(int nEventNo) {return;};							// スレッドイベント発生
	

//// メンバー変数
private:

	//=========================================
	// 各インスタンス
	PlgControlManager*			mcls_pPlg;								// PLG距離管理クラス
	PioControlManager*			mcls_pPio;								// PIO管理クラス


	//=========================================
	// 同期制御
	CRITICAL_SECTION		m_csLock;									// クリティカルセクション

};
