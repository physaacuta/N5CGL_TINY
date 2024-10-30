#pragma once

// 疵検 インクルード
#include "..\..\Library\KizuLibMFC\CLASS\ThreadManager.h"				// ワーカースレッドマネージャー
#include "..\..\Library\KizuLib\CLASS\KizuMacro.h"						// 疵検マクロ

// 疵検ライブラリ インクルード
#include "..\..\Library\KizuODBC\OdbcBase.h"							// ODBC操作クラス

// 各ワーカースレッド
#include "MainConst.h"
#include "DeleLowGr.h"													// 無害Gr削除クラス


using namespace KizuLib;
using namespace KizuODBC;


class ControlLowGr : public ThreadManager
{
private:
	//------------------------------------------
	// 固定定数
	//------------------------------------------
	static const int THREAD_MAXCNT				= 9;					// 最大DB実行スレッド数

//// 公開関数
public:
	ControlLowGr();
	virtual ~ControlLowGr(void);

	bool	IsEnable() {return m_bEnable;}

	void WorkerInit();													// 削除スレッド初期化処理
	void WorkerStop();													// 削除スレッド停止処理
	void WorkerExit();													// 削除スレッド終了処理


//// メンバー関数
private :
	int ThreadFirst();													// スレッド開始前処理 (スレッド実行直後(シグナルセット前)にコールバック)
	int ThreadLast();													// スレッド終了前処理 (終了シグナル後にコールバック)
    void ThreadEvent(int nEventNo);										// スレッドイベント発生


	int CheckExecItem(char const* cKizukenNo);							// 削除対象の疵検管理Noかチェック
	
	bool CheckDeleListItem();											// 無害Gr削除対象チェック
	bool BackUpDeleListItem();											// 無害Gr削除対象チェック


//// メンバー変数
private :

	//=========================================
	// 各インスタンス
	DeleLowGr*				mcls_pLowGr[THREAD_MAXCNT];					// 無害Gr削除クラス


	//// 同期シグナル
	HANDLE					m_evEndDelete;								// 削除完了シグナル
	HANDLE					m_evTimeCycle;								// 定周期シグナル

	//// iniファイル
	int						m_nIniThreadCount;							// 同時実行削除スレッド数
	int						m_nIniCycle;								// DELETE_DEFECT_LISTを何分周期でチェックするか [ms]
	int						m_nIniDeleDay;								// 何日以前のデータを強制削除するか [日] (0のときは強制削除しない)
	CString					m_cIniSqlSelectKey;							// 一覧テーブルから削除対象キー取得SQL (削除対象主キーを昇順で一覧取得)
	SETTING_TBL				m_typSet;									// 設定テーブル情報

	// その他もろもろ
	bool					m_bEnable;									// 実行対象の場合 true
};
