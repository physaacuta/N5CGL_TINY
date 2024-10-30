
#pragma once


// インクルード
#include <process.h>

// 疵検 インクルード
#include "..\..\include\KizuCommon.h"									// 疵検共通ヘッダー
#include "..\..\Library\KizuLib\KizuLib.h"								// 疵検ライブラリヘッダー

// 疵検クラス インクルード
#include "..\..\Library\KizuLib\CLASS\KizuMacro.h"						// 疵検マクロ
#include "..\..\Library\KizuLib\CLASS\WorkThread.h"						// ワーカースレッド
#include "..\..\Library\KizuLib\Class\LockManager.h"					// スレッド間同期制御クラス
#include "..\..\Library\KizuLibMFC\CLASS\LogFileManager.h"				// ログマネージャー

// 疵検ライブラリ インクルード
#include "..\..\Library\KizuODBC\OdbcBase.h"							// ODBC操作クラス


// 各ワーカースレッド
#include "DeleTable.h"													// DBデータ削除クラス


using namespace KizuLib;
using namespace KizuODBC;

class ControlManager : public WorkThread
{
public:
	//// ディスク容量
	typedef struct {
		double		dTotalSize;											// ディスクの総容量 [Mbyte]
		double		dFreeSize;											// ディスクの空き容量 [Mbyte]
		double		dFreeRitu;											// ディスクの空き率 [%]
	} DISK_SPACE;


private:
	//------------------------------------------
	// 固定定数
	//------------------------------------------
	static const int THREAD_MAXCNT				= 9;					// 最大DB実行スレッド数


public:
	ControlManager(char* cSession, LogFileManager* pLog);
	virtual ~ControlManager(void);


	void Start() { WorkThread::Start(MainThread); };					// スレッド開始
	void Stop()  { WorkThread::Stop(20000); };							// スレッド終了

	void WorkerInit();													// 削除スレッド初期化処理
	void WorkerStop();													// 削除スレッド停止処理
	void WorkerExit();													// 削除スレッド終了処理


	void SetWaitMode(bool mode);										// 削除/書込 優先モード (true:削除優先)


	// 画面アクセス用
	char const*	GetSession() const { return m_cSession; };				// セッション名称
	char const* GetOdbcSession() const { return m_cOdbcSession; };		// 接続するODBCが定義されてるセッション
	const DISK_SPACE& GetDiskSpace() const {return m_DiskSpace;}		// ディスク容量状態取得
	bool GetWaitMode() const {return m_bWaitMode;}						// 削除/書込 優先モード (true:削除優先)
	int  IsExec(int rec) const	   { if(NULL != mcls_pDele[rec]) { return mcls_pDele[rec]->IsExec()? 1: 0; } else { return -1;} }	// 処理実行中
	int  IsExec() const	   { if(NULL != mcls_pCycle) { return mcls_pCycle->IsExec()? 1: 0; } else { return -1;} }	// 処理実行中


private:
	static UINT WINAPI MainThread(LPVOID param);						// メインスレッド

	void CheckDB_Size();												// DBの使用容量チェック
	int  GetDB_Size();													// DBの使用容量取得
	void Check_TranSize();												// トランザクションログサイズチェック


	// 各ワーカースレッド
	LogFileManager*			mcls_pLog;									// LogFileManagerスレッドインスタンス
	DeleTable*				mcls_pDele[THREAD_MAXCNT];					// DB削除管理クラス (容量低下用)
	DeleTable*				mcls_pCycle;								// 定周期実行用
	LockManager				mcls_Lock;									// ロックマネージャー

	//// 同期シグナル
	HANDLE					m_evEndCycle;								// 定周期チェック完了シグナル
	HANDLE					m_evEndSizeCheck;							// DB容量チェック完了シグナル

	//// クリティカルセクション
	CRITICAL_SECTION		m_csLock;									// 子スレッド間 同期項目削除用

	// その他もろもろ
	DISK_SPACE				m_DiskSpace;								// 現在のディスク容量状態
	bool					m_bWaitMode;								// 削除/書込 優先モード (true:削除優先)
	int						m_nWaitTime;								// 現在の待ち時間 [ms]
	EM_DIV_BOOL				m_bTranStat;								// トランザクションチェック状態 (-1:初回 1:正常 0:異常)
	bool					m_bExecFast;								// 初回実行後true

	//// iniファイル
	CString					m_cSession;									// 自分の設定が定義されているiniファイルのセッション
	CString					m_cOdbcSession;								// 接続するODBCが定義されてるセッション
	int						m_nIniTranSizeErrID;						// トランザクションログ異常機器ID (0:チェック無し) 
	int						m_nIniTranErrSize;							// トランザクションエラー認識サイズ

	long					m_nIniTimeCycle;							// 定周期実行周期 [ms]
	long					m_nIniTimeSizeCheck;						// DB容量チェック周期 [ms]
	int						m_nIniThreadCount;							// DB容量低下時の削除スレッド数
	double					m_dIniDeleteRitu[THREAD_MAXCNT];			// 空き容量率
	int						m_nIniTimeMaxWait;							// 書込優先時の待ち時間 [ms]
	int						m_nIniTimeMinWait;							// 削除優先時の待ち時間 [ms]
//	int						m_nIniLogDay;								// ログ保存日数
//	int						m_nIniDelCoilCnt;							// 1回の処理での削除コイル件数 (0件時、実績削除しない)

	KizuDeleteManager::PARAM_INF	m_Param;							// 条件パラメータ

};
