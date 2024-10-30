#pragma once

// 疵検クラス インクルード
#include "..\..\Library\KizuLibMFC\CLASS\ThreadManager.h"				// ワーカースレッドマネージャー
#include "..\..\Library\KizuODBC\OdbcBase.h"							// ODBC操作クラス

// ローカル
#include "MainConst.h"													// 自クラス定数ヘッダー


class FileCheckManager :	public ThreadManager
{

//// 公開関数
public:
	FileCheckManager(int nId);
	virtual ~FileCheckManager(void);


	void		SetEv_CheckStart() { SetEvent(m_evCheckStart);}
	EM_DIV_BOOL	GetRpiFile() { return m_emRpiFile; }
	char*		GetKizukenNo() { return m_cKizukenNo; }


//// メンバー関数
private:
	int ThreadFirst();													// スレッド開始前処理 (スレッド実行直後(シグナルセット前)にコールバック)
	int ThreadLast();													// スレッド終了前処理 (終了シグナル後にコールバック)
    void ThreadEvent(int nEventNo);										// スレッドイベント発生
	

	EM_DIV_BOOL CheckRpiFile();											// RPIファイルチェック


//// メンバー変数
private:

	//// ハンドル
	HANDLE					m_evTimerExec;								// 定周期実行イベント
	HANDLE					m_evCheckStart;								// 実行イベント

	//// Iniファイル
	int						m_nTimeFirst;								// 初期実行までの時間 [ms]
	int						m_nCheckTime;								// チェック周期 [ms]
	int						m_nCheckOkNum;								// 過去xコイルを見て、１つもRPIファイルが無ければNG判定 (1～)
	int						m_nStateId;									// 機器ID

	int						m_nRpiCheckMode;							// チェックモード (0:Iniファイルパス指定  1:直接指定)
	char					m_cRpiCheckSql[1024];						// 実績検索用SQL文

	// iniファイル指定時のみ
	char					m_cRpiSaveIniPath[256];						// TI_RPISAVEのINIファイル絶対パス(統括IniSettingの中)
	char					m_cRpiSaveInsSec[256];						// TI_RPISAVEセクション名 ([TI_RPISAVE_INS_1])

	char					m_cRebootPcname[SIZE_NAME_PC];				// 再起動PC名称


	//// その他もろもろ
	int						m_nId;										// ID (1オリジン)
	EM_DIV_BOOL				m_emRpiFile;								// RPIファイル 正常
	char					m_cKizukenNo[SIZE_KIZUKEN_NO];				// 前回チェック時の最新疵検管理No
};
