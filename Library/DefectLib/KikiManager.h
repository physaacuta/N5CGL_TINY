// *********************************************************************************
//	機器状態マネージャー
//	[Ver]
//		Ver.01    2007/07/16  vs2005 対応
//		Ver.02	  2009/02/16  STATUS_MASTERのシステム範囲追加に対応
//		Ver.03	  2009/03/04  非同期のステータス書き込みに対応
//		Ver.04	  2009/06/24  正常でのメッセージ出力に対応
//		Ver.05	  2013/04/30  LOG_MASTERのオペガイ有無に対応
//
//	[メモ]
//		DBのSTATUS_MASTERとSTATUS_INFを読み込み、機器状態をチェックする
// *********************************************************************************
#pragma once

// 半固定
#include "..\..\Include\KizuCommon.h"									// 疵検インクルード
#include "..\KizuLib\KizuLib.h"											// 疵検ライブラリ
#include "..\KizuLib\Class\KizuMacro.h"									// マクロ集
#include "..\KizuLibMFC\Class\LogFileManager.h"							// ログ管理クラス

// 疵検クラス インクルード
#include "..\KizuLib\Class\WorkThread.h"								// ワーカースレッドクラス
#include "..\KizuLib\Class\AutoLock.h"									// クリティカルセクションクラス
#include "..\KizuLib\Class\KizuPerformanceCounter.h"					// 時間測定クラス

#include "..\KizuODBC\OdbcBase.h"										// DB操作クラス

using namespace KizuLib;
using namespace KizuODBC;


class KikiManager : public WorkThread
{
private:
	static const int STATUS_OK_MSG_YUUSENID_MIN = 900;					// 機器状態・検査状態 正常でもこの優先度以下であれば、メッセージ出力 Ver.04
	static const int STATUS_OK_MSG_YUUSENID_MAX = 999;

	struct DELIVERYSTATUS {											// ステータス変更受け渡し構造体
		int		id;														// int id 機器ID
		int		state;													// 機器状態 (0:正常  1～:異常)
		bool	bLogDsp;												// シスログ出力の有無
		CString	strMsg;													// 追加メッセージ
	};

public:
	KikiManager(int nCycleTime, LPCSTR cIniPath=TASKINI_NAME, LPCSTR cSession=INI_DB);
	virtual ~KikiManager(void);

	void Start() { WorkThread::Start(MainThread); };					// スレッド開始
	void Stop()  { WorkThread::Stop(10000); };							// スレッド終了
	void SetLogMgr(LogFileManager* pLog) { mcls_pLog = pLog; }			// ログファイルセット

	void SetOutPutLog() { m_bOutPutLog = true; }						// LOG_MASTERの[ｵﾍﾟｶﾞｲ有無]対応時 true

	//// 外部アクセス用
	void	SetKadou(bool isRun) { m_bIsKadou = isRun; }				// 稼動状態
	HANDLE	GetEvReadEnd() const { return m_evReadEnd; }				// 非同期読込み完了イベント
	void	Refresh();													// 強制更新 (読込み完了するまでブロックされる)
	void	GetStatusInf(TYP_STATUS_INF* buf, int size) {				// 機器状態一覧取得
				AutoLock lock(&m_csLock);
				memcpy(buf, m_typStatInf, size);
			}		
	void	GetStatusNow(TYP_STATUS_NOWSYSTEM* buf) {					// 現異常状態取得
				AutoLock lock(&m_csLock);
				memcpy(buf, &m_typStatNow, sizeof(m_typStatNow));
			}
			
	void	CheckBitSysArea(int bitPattan) {m_nSysArea_BitPattan = bitPattan; }	// 対象となるシステム範囲のビットパターンをセット Ver.02

	bool CheckStatus();													// 機器状態取得 (セーフスレッドでは無い為注意すること)


	//// 機器情報 非同期書き込み
	void SetStatusQue(int id, int state, bool bLogDsp =true, char const * cMsg = "" );			// DB_SYSTEM_STATEに機器状態をセットする (0:正常)
	void SetStatusQue(int id, bool state, bool bLogDsp =true, char const * cMsg  = "" );		//										 (true:正常)
	void SetStatusQue(char const * cKey, bool state, bool bLogDsp =true, char const * cMsg  = "" );// iniファイルから機器IDを取得する (true:正常)
	void SetStatusQue(char const * cKey, int state, bool bLogDsp =true, char const * cMsg  = "" );// iniファイルから機器IDを取得する (true:正常)

	//// 機器情報 非同期読み込み要求
	void SetStatusRead() { SetEvent(m_evSyncRead); }					// (非同期)強制読込み開始イベント。完了したら通常通り m_evReadEnd がシグナルとなる


private :
	static UINT WINAPI MainThread(LPVOID param);						// メインスレッド
	bool SetStatusThread(DELIVERYSTATUS* pDeli);						// ステータス 非同期書き込み
	void GetLogInfo(TYP_STATUS_NOWSYSTEM* inf);							// オペガイするメッセージを取得

	bool ConnectDB(bool bLogDsp);										// DB接続
	void CloseDB();														// DB切断


	// 各インスタンス
	LogFileManager*			mcls_pLog;									// LogFileManagerスレッドインスタンス
	OdbcBase*				mcls_pDB;									// ODBC操作クラス
//	KizuPerformanceCounter mcls_Counter;								// 時間測定スレッド
	
	// 同期ハンドル
	HANDLE					m_evTimeCycleRead;							// 定周期読み込み
	HANDLE					m_evReadEnd;								// 非同期読込み完了イベント
	HANDLE					m_evLockReadStart;							// (同期)強制読込み開始イベント
	HANDLE					m_evLockReadEnd;							// (同期)強制読込み完了イベント
	HANDLE					m_evSyncRead;								// (非同期)強制読込み開始イベント

	// クリティカルセクション
	CRITICAL_SECTION		m_csLock;									// 情報操作ロック用

	// 受け渡しキュー
	ThreadQueue<DELIVERYSTATUS> mque_Deli;								// ステータス変更 受け渡しキュー


	// 機器状態読込関連
	TYP_STATUS_INF			m_typStatInf[MAX_STATUS];					// DB取得の機器状態一覧
	TYP_STATUS_NOWSYSTEM	m_typStatNow;								// DB取得の現異常状態
	int						m_nCycleTime;								// DB読込周期 [ms]
	int						m_nMsgReadCnt;								// メッセージを読込むタイミング
	bool					m_bOutPutLog;								// LOG_MASTERの[ｵﾍﾟｶﾞｲ有無]対応時 true		//Ver.05

	// その他もろもろ
	char					m_cIniPath[128];							// 接続対象のIniファイルパス
	char					m_cIniSession[128];							// 接続対象のセッション
	bool					m_bDBConnect;								// DB接続状態 (true:接続中)
	bool					m_bIsKadou;									// 稼動状態 (true:検査中 false:停止中) 停止時は、検査状態を判断しない

	int						m_nSysArea_BitPattan;						// 対象となるシステム範囲のビットパターン (0の時は、システム範囲 未対応と認識) Ver.02
};
