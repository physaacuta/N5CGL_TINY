#pragma once

// 疵検クラス インクルード
#include "..\..\Library\KizuLibMFC\CLASS\ThreadManager.h"				// ワーカースレッドマネージャー

#include "..\..\Include\LineCommon.h"									// 疵検インクルード

// ローカルのクラス
#include "MainConst.h"
#include "ParamManager.h"												// パラメータ管理クラス

using namespace KizuLib;

class LongPdCheckManager :	public ThreadManager
{
//// 構造体
public:
	//=============================
	// 監視情報 構造体
	//=============================
	struct CEHCK_INF
	{
		char		cManagementNo[SIZE_KIZUKEN_NO];						// 管理No
		BOOL		bLongPdExist;										// 長周期欠陥有無(true:有り false:無し)
		int			nCount[NUM_MEN];									// 個数　ガイダンス用
		char		cCoilNoIn[SIZE_COIL_NO+1];							// 入側コイルNo+NULL  (コイルNo) ガイダンス用
	};

//// ローカル定数
private :

	//=========================================
	// 定数

	//static const int TIME_INTERVAL_DEFAULT_EXEC	= 5;					// 定周期処理実行 タイマ周期 [ms] デフォルト値　※パラメータ読込時までの値


//// 公開関数
public:
	LongPdCheckManager(void);
	virtual ~LongPdCheckManager(void);

	//=========================================
	// プロパティ
	void SetParamMgr(ParamManager* pCls)	{ mcls_pParam = pCls; }		// パラメータ管理

	////=========================================
	//// 外部操作関数


	//=========================================
	// 外部アクセス

	//=========================================
	// 画面専用
	CEHCK_INF const*  GetCheckInf(int index)	{ return &m_CheckInf[index]; }


//// メンバー関数
private:
	int ThreadFirst();													// スレッド開始前処理 (スレッド実行直後(シグナルセット前)にコールバック)
	int ThreadLast();													// スレッド終了前処理 (終了シグナル後にコールバック)
    void ThreadEvent(int nEventNo);										// スレッドイベント発生
	
	void CheckLongPd();													// 長周期欠陥チェック

//// メンバー変数
private:
	CEHCK_INF			m_CheckInf[CHECK_COIL_NUM];						// 監視情報 ※定周期で直近コイルの情報を取得
	int					m_nParamCycleTime;								// パラメータから読込した周期時間 ( 0以下は反映せずにデフォルト )

	//=========================================
	// 各インスタンス
	ParamManager*			mcls_pParam;								// パラメータ管理

	//// キュー

	//// ハンドル
	HANDLE					m_evTimerExec;								// 定周期実行イベント	
};
