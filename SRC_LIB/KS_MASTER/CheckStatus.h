// *********************************************************************************
//	機器状態管理クラス
//	[Ver]
//		Ver.01    2007/04/30  vs2005 対応
//
//	[メモ]
//		
// *********************************************************************************

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
#include "..\..\Library\KizuLib\Class\AutoLock.h"						// クリティカルセクションクラス
//#include "..\..\Library\KizuLib\Class\KizuPerformanceCounter.h"			// 時間測定クラス

#include "..\..\Library\KizuODBC\OdbcBase.h"							// DB操作クラス

using namespace KizuLib;
using namespace KizuODBC;


class CheckStatus : public WorkThread
{
private :
	static const int HEART_BEAT_SEND_CYCLE	= 7000;						// ハートビート送信周期
	static const int HEART_BEAT_CHECK_CYCLE = 20000;					// ハートビートチェック周期


	struct CheckStatus_INF {										// 機器情報
		int  id;														// ハートビートチェックする機器ID
		char name[32];													// 機器名称
		int  status;													// 機器状態 (0:正常)
		int	 nHb;														// ハートビート確認有無 (0:無し 1:有り)
		bool statusHb;													// ハートビート受信有無 (false:まだ来てない  true:来た)
	};


public:
	CheckStatus(bool isMaster);
	virtual ~CheckStatus(void);

	void Alloc();														// データ確保
	void Start() { WorkThread::Start(MainThread); };					// スレッド開始
	void Stop()  { WorkThread::Stop(10000); };							// スレッド終了
	void SetLogMgr(LogFileManager* pLog) { mcls_pLog = pLog; }			// ログファイルセット

	// Set
	void Set_StatusID(int id, bool status);								// ステータスセット
	void Set_StatusIndex(int index, bool status);
	void Set_HeatBeatID(int id, bool status);							// ハートビートが来たよフラグセット
	void Set_HeatBeatIndex(int index, bool status);

	// Get
	int GetIndex(int id);												// 機器IDから配列位置を取得する
	char const * GetName(int id);										// 該当の機器IDの機器名称を取得

private :
	static UINT WINAPI MainThread(LPVOID param);						// メインスレッド

	void Get_IniData();													// iniファイルデータ取得
	bool Init_StatusInf();												// 機器状態テーブル初期化 (マスター時のみ)
	bool Get_Status_ID(int mode=0);										// 機器IDを取得
	void Check_HeatBeat();												// ハートビートチェック
	void Send_HeatBeat();												// ハートビート送信


	//// 各インスタンス
	LogFileManager*			mcls_pLog;									// LogFileManagerスレッドインスタンス

	// クリティカルセクション
	CRITICAL_SECTION		m_csLock;									// 処理ロック用

	// シグナル
	HANDLE					m_evHBeatCheck;								// ハートビート監視タイマイベント
	HANDLE					m_evHBeatSend;								// ハートビート送信タイマイベント

	// 機器状態関連
	int						m_nStateCount;								// 機器状態情報件数
	CheckStatus_INF			m_typStateCheck[MAX_STATUS];				// 機器状態情報

	// その他もろもろ
	bool					m_bIsMaster;								// PCマスター管理時true
	int						m_nMasterCheckID;							// マスター→スレーブのハートビート確認用の機器ID  (0:確認無し 1～:確認有り)
	int						m_nSendHBID;								// ハートビート送信機器ID (0:送信しない 1～:送信する)
	
};
