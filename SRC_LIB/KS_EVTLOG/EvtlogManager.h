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

using namespace KizuLib;

class EvtlogManager : public WorkThread
{
private:
	static const int MSG_CHECK_CNT				= 50;					// メッセージチェック件数
	static const int PARAM_TIME_CYCLE			= 5*60*1000;			// 定周期タイマー

public:
	enum EM_EVLOGNO {													// イベントログ種類
		em_APPLICATION = 0,														// Application
		em_SYSTEM,																// System
		em_SECURITY,															// Security
		em_END																	// ダミー
	};			


public:
	EvtlogManager(void);
	virtual ~EvtlogManager(void);

	void Start() { WorkThread::Start(MainThread); };					// スレッド開始
	void Stop()  { WorkThread::Stop(3000); };							// スレッド終了
	void SetLogMgr(LogFileManager* pLog) { mcls_pLog = pLog; }			// ログファイルセット

	void ResetErr();													// 異常状態を解除する

protected :
	static UINT WINAPI MainThread(LPVOID param);						// メインスレッド
	
	bool CheckEventLog();												// イベントログから異常状態を判定
	CString DispMessage( EM_EVLOGNO emEvLogNo, const char *SourceName, 
		const char **Args, DWORD MessageId) const;	// メッセージの取得
	bool GetModuleNameFromSourceName( EM_EVLOGNO emEvLogNo, const char *SourceName,			
		const char *EntryName, char *ExpandedName ) const;				// ソース名からモジュール名を取得

	void CheckApplicationPopup(EM_EVLOGNO ii,time_t time, const char *pSourceName,const char *pArg1, const CString &sMessage);
	void CheckStringInEvent(EM_EVLOGNO ii,time_t time, const char *pSourceName,const char *pArg1, const CString &sMessage);


	//// 各インスタンス
	LogFileManager*			mcls_pLog;									// LogFileManagerスレッドインスタンス

	//// iniファイル設定情報
	int						m_nEnable;									// イベントログ 0:チェックしない 1:チェックする
	int						m_nApId;									// アプリケーションポップアｐップ時の異常機器ID (=0:チェックなし)
	int						m_nStateId;									// イベントログメッセージ異常時の機器ID (=0:チェックなし)
	char					m_cMsg[MSG_CHECK_CNT][256];					// チェックメッセージ
	int						m_nMsgCheckCnt;								// チェックメッセージ件数

	//// イベントログチェック用
	DWORD					m_dayLastCheckTime;							// 最終チェック時刻
	time_t					m_tmRealBoot;								// KS_EVTLOG起動時刻
	/// 壊れたログのキャッシュ用（リソースDLLがない）
	static const int SZUNKNOWN = 16;
	mutable CString m_nUnknownEvent[SZUNKNOWN];
	mutable int   m_nUnknownEventIndex;
};
