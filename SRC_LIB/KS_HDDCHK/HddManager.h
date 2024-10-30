#pragma once

// 疵検クラス インクルード
#include "..\..\Library\KizuLibMFC\CLASS\ThreadManager.h"				// ワーカースレッドマネージャー
#include "..\..\Library\KizuLib\Class\ProcessManager.h"					// プロセス管理

// 各ワーカースレッド
#include "MainConst.h"

class HddManager :	public ThreadManager
{
//// 定数
private:
#define	CMD_TXT_NAME		"hddstatus.txt"								// HDD状態テキスト
	static const int		MAX_CHK_TXT			= 10;					// 数
	static const int		TIME_EXEC_CYCLE		= 1 * 60 * 60 * 1000;	// HDDチェック周期 (1時間に1回)

//// 公開関数
public:
	HddManager(void);
	virtual ~HddManager(void);

	void Alloc();														// 関数初期化

	
	//// 画面用
	void SetEvManualExec()		{ SetEvent(m_evManualExec); }			// 手動実行シグナル
	void ShowEdit();													// 詳細結果表示
	
	bool	IsState() const		{ return m_bState; }
	CString	GetNgMsg() const	{ return m_sNgMsg; }



//// メンバー関数
private:
	int ThreadFirst();													// スレッド開始前処理 (スレッド実行直後(シグナルセット前)にコールバック)
	int ThreadLast();													// スレッド終了前処理 (終了シグナル後にコールバック)
    void ThreadEvent(int nEventNo);										// スレッドイベント発生
	
	void Exec();														// チェック
	// 20181220 Smart Storage Administrator対応 --->>>
	void ExecSSA();														// チェック(SSA)
	// 20181220 Smart Storage Administrator対応 ---<<<
	bool GetColumn(CString& sVal, int& curPos, CString& sToken);		// １行のデータから , 単位の文字列を取得


//// メンバー変数
private:

	//// ハンドル
	HANDLE					m_evManualExec;								// 手動実行イベント
	HANDLE					m_evTimerExec;								// 定周期実行イベント

	//// 結果
	bool					m_bState;									// HDD状態
	CString					m_sNgMsg;									// NG時の文字列



	//// iniファイル設定値
	int						m_nChkNum;									
	CString					m_sChkTxt[MAX_CHK_TXT];						// チェックする文字列
	int						m_nOkNum;
	CString					m_sOkTxt[MAX_CHK_TXT];						// 正常と判断する文字列
	int						m_nStatusID;								// 機器ID
	// 20181220 Smart Storage Administrator対応 --->>>
	int						m_nMode;									// モード(0:Universal Raid Utility, 1:Smart Storage Administrator)
	// 20181220 Smart Storage Administrator対応 ---<<<

};
