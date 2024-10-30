#pragma once

// 半固定
#include "..\..\Include\KizuCommon.h"									// 疵検インクルード
#include "..\..\Library\KizuLib\KizuLib.h"								// 疵検ライブラリ
#include "..\..\Library\KizuLib\Class\KizuMacro.h"						// マクロ集
#include "..\..\Library\KizuLib\Class\KizuFunction.h"					// スタティックライブラリ集
#include "..\..\Library\KizuLibMFC\Class\LogFileManager.h"				// ログ管理クラス

// 疵検クラス インクルード
#include "..\..\Library\KizuLib\Class\WorkThread.h"						// ワーカースレッドクラス
#include "..\..\Library\KizuLib\Class\ThreadQueue.h"					// スレッドキュークラス
#include "..\..\Library\KizuLib\Class\LockManager.h"					// スレッド間同期制御クラス

#include "..\..\Library\KizuODBC\OdbcBase.h"							// DB操作クラス

using namespace KizuLib;
using namespace KizuODBC;

class KizuDeleteManager : public WorkThread
{
public:
//	//// メッセージ定義
//	static const int WM_SIZEMIN_END					= WM_USER+100;		// 容量低下削除実行完了通知
	
	//// 受け渡し構造体
	struct DEL_TBL{
		CString		cDelTable;											// 削除対象の実績テーブル名称
		CString		cKeyName;											// 主キー項目名称
		CString		cSubName;											// サブキー項目名称 (一括削除時、指定無し)
		int			nDelRow;											// 1回の削除行数	(一括削除時、0)			
	};
	// 指定件数保持削除用
	struct DEL_TBL_CNT : implements DEL_TBL{
		int			nMaxCnt;											// 保持上限数		
	};


	//// 必要なパラメータ構造体
	struct PARAM_INF {
		int			nLogSaveDay;										// ログ保存日数

		int			nDelCoilCnt;										// 1回の処理での削除コイル件数
		char		sCoilResultTbl[64];									// コイル実績テーブル
		char		sCoilResultKey[64];									// コイル実績の主キー
		char		sCoilResultSort[64];								// コイル実績のソートキー

	};

public:
	KizuDeleteManager(int id, int* nWaitTime, HANDLE evEndCycle, HANDLE evEndSizeMin, CString cSession, CString cOdbcSession);
	virtual ~KizuDeleteManager(void);

	void Start() { WorkThread::Start(MainThread); };					// スレッド開始
	void Stop()  { WorkThread::Stop(10000); };							// スレッド終了
	void SetLogMgr(LogFileManager* pLog) { mcls_pLog = pLog; }			// ログファイルセット
	void SetLockMgr(LockManager* pLock)	 { mcls_pLock = pLock;}			// スレッド間同期制御クラス

	void SetParamInf(PARAM_INF* prm) { memcpy(&my_Param, prm, sizeof(my_Param)); }	// パラメータ設定

	//// 外部アクセス
	void SetStop()				{ my_bStopFlg = true; SetEvent(my_evStop);}	// 終了要求
	void SetEvCycle()			{ SetEvent(my_evCycle); }				// 定周期実行イベント
	void SetEvSizeMin() 		{ SetEvent(my_evSizeMin); }				// DB容量低下実行イベント
	void SetEvExecFast() 		{ SetEvent(my_evExecFast); }			// 初回実行イベント
	bool IsExec_Cycle() const	{ return my_bIsExec_Cycle; }			// 処理実行中 (定周期処理で実行)
	
	int	 GetTranSizeMin() const { return my_nTranSizeMin;}				// トランザクションログサイズ 最小定義
	int	 GetTranSizeNow() const { return my_nTranSizeNow;}				// トランザクションログサイズ 現状


	//// 画面用
	bool IsExec() const			{ return (my_bIsExec_Cycle||my_bIsExec_SizeMin) ? true : false ;}	// 処理実行中


protected :
	static UINT WINAPI MainThread(LPVOID param);						// メインスレッド

    // //////////////////////////////////////////////////
    // 純粋仮想関数 (必ず 継承先で実装する事)
    // //////////////////////////////////////////////////
	virtual void ExecCycle()   = 0;										// 定周期実行メソッド
	virtual void ExecSizeMin() = 0;										// DB容量低下実行メソッド
	virtual void ExecFast() = 0;										// 初回実行メソッド


	// 取得関連
	bool Select_OldKey(char const *tbl, char const *keyName, char const *keySort, int size, char *keyVal); // 指定テーブルの最古のキーを取得
	bool Select_OldKey(char const *tbl, char const *keyName, int size, char *keyVal);
	bool Select_OldKey(char const *tbl, char const *keyName, int *keyVal);
	bool Select_OldKey(char const *tbl, char const *keyName, char const *keySort, int *keyVal);
	bool Select_SubKey(char const *tbl, char const *keyName, char const *subName, char const *keyVal, int size, char *subVal);	// 指定テーブルのサブキーを取得
	bool Select_SubKey(char const *tbl, char const *keyName, char const *subName, char const *keyVal, int *subVal);

	// データ容量管理
	bool Exec_Shrink();													// トランザクションログ 圧縮
	bool Delete_Log(int nDelLogDay);									// Log削除処理
	bool Delete_Day(char const *tbl, char const *keyDayName, int day);	// 指定日数以前データを一括削除
	bool Delete_Day2(char const *tbl, char const *keyDayName, int day);	// 指定日数以前データを１日単位で削除
	bool Delete_Cnt(char const *tbl, char const *keyName, char const *subName, int delCnt, int nMaxNum);	// 指定件数以上の削除

	// Old
	bool Delete_Inf(DEL_TBL const *dtbl, char const *keyVal);			// 指定主キーの 一括削除/少量削除 エントリー
	bool Delete_Inf(char const *tbl, char const *keyName, char const *keyVal);									// 指定主キーの 一括削除 (1コイル分)
	bool Delete_Inf(char const *tbl, char const *keyName, char const *subName, int delCnt, char const *keyVal); // 指定主キーの 少量削除

	// Inf
	bool Delete_OldInf(DEL_TBL const *dtbl, char const *keyVal);								// 一括削除/少量削除 エントリー (指定キーより古い実績を削除処理)
	bool Delete_OldInf(char const *tbl, char const *keyName, char const *keyVal);				// 指定キーより古い実績を一括削除
	bool Delete_OldInf(char const *tbl, char const *keyName, char const *subName, int delCnt, char const *keyVal); // 指定キーより古い実績を少量削除



	//// 各インスタンス
	LogFileManager*			mcls_pLog;									// LogFileManagerスレッドインスタンス
	LockManager*			mcls_pLock;									// ロックマネージャー


	//// 同期シグナル
	HANDLE					my_evCycle;									// 定周期実行イベント
	HANDLE					my_evSizeMin;								// DB容量低下実行イベント
	HANDLE					my_evExecFast;								// 初回実行イベント

	// 外部からセット
	HANDLE					my_evEndCycle;								// 定周期完了イベント
	HANDLE					my_evEndSizeMin;							// DB容量低下完了イベント


	//// その他もろもろ
	int						my_nID;										// 自分のID (1オリジン)
	int*					my_pnWaitTime;								// 一時停止時間
	bool					my_bIsExec_Cycle;							// 処理実行中 (定周期処理で実行)
	bool					my_bIsExec_SizeMin;							// 処理実行中 (DB容量低下処理で実行)
	bool					my_bStopFlg;								// 終了要求時 true

	//// iniファイル & 設定値
	CString					my_cSession;								// 自分の設定が定義されているiniファイルのセッション
	CString					my_cOdbcSession;							// 接続するODBCが定義されてるセッション
	PARAM_INF				my_Param;									// 条件パラメータ
	/*
	int						my_nIniLogDay;								// ログ保存日数
	int						my_nIniDelCoilCnt;							// 1回の処理での削除コイル件数
	CString					m_cIniCoilResultTbl;						// コイル実績テーブル
	CString					m_cIniCoilResultKey;						// コイル実績の主キー
*/


	//// トランザクションログチェック関係
	int						my_nTranSizeMin;							// トランザクションログサイズ 最小定義
	int						my_nTranSizeNow;							// トランザクションログサイズ 現状
};
