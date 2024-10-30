#pragma once


// 共通ヘッダー
#include "..\..\Include\KizuCommon.h"									// システム用インクルード

// 疵検クラス インクルード
#include "..\..\Library\KizuLibMFC\CLASS\ThreadManager.h"				// ワーカースレッドマネージャー
#include "..\..\Library\KizuLibMFC\Class\DivNameManager.h"				// DIV定数 名称取得クラス
#include "..\..\Library\KizuLib\Class\AutoLock.h"						// クリティカルセクション制御クラス

#include "..\..\Library\DefectLib\KikiManager.h"						// 機器状態管理クラス


// 画像処理用ライブラリ インクルード
#include "..\..\Library\IportLib\IportCommon.h"							// iPort用クラス定数

// ローカル
#include "MainConst.h"
#include "ParamManager.h"												// パラメータ管理クラス

using namespace KizuLib;

class StatusManager : public ThreadManager
{
//// 公開定数
public:
	//// タスク ステータス
	enum EM_MAIN_STATE
	{		
		STATE_NON = 0,			// 初期化中
		STATE_INIT,				// 初期状態 (資源開放状態)
		STATE_RUN,				// 検査中   (オンライン) (ただし、撮像待ちの可能性はあり)
	};

	// 内部ステータス状態
	enum EM_SYS_STATE
	{	
		SYS_STATE_NON = 0,			// 初期化中
		SYS_STATE_WAIT,				// 撮像待ち

		// 撮像中--- >>>
		SYS_STATE_OK,
		SYS_STATE_WAR,
		SYS_STATE_ERR
	};



//// 公開関数
public:
	StatusManager(void);
	virtual ~StatusManager(void);

	// プロパティ
	void SetParamMgr(ParamManager* pCls) { mcls_pParam = pCls; }		// パラメータ管理



	//// タスクステータス制御
	bool			IsRun()				const { return STATE_RUN == m_emMainSt ? true : false;}	// 探傷中:true
	// ステータス
	EM_MAIN_STATE	GetMainSt()			const { return m_emMainSt; }							// 現在のステータス取得
	CString			GetMainStName()		const { return GetMainStName(m_emMainSt); };			// 現在のステータス名称取得
	CString			GetMainStName(EM_MAIN_STATE st) const;										// 指定のステータス名称取得
	bool			SetMainSt(EM_MAIN_STATE st);												// ステータス変更

	//// フレームズレ状態
	bool			GetFrameSyncAllSt(int nInsNum);												// フレームズレ状態取得
	bool			GetFrameSyncSt(int nIns) { return m_bFrameSync[nIns]; };													// フレームズレ状態変更
	void			SetFrameSyncSt(int nIns, bool bMode);										// フレームズレ状態変更
	void			ResetFrameSyncAllSt();														// フレームズレ状態リセット
	void			ResetFrameSyncSt(int nInsNum, int nFno);
	DWORD			GetReSyncFno(int nIns) { return m_nFrameSyncNo[nIns]; }
	void			ResetAp();		// ソフト強制再起動


//// メンバー関数
private:
	int ThreadFirst();													// スレッド開始前処理 (スレッド実行直後(シグナルセット前)にコールバック)
	int	ThreadLast()	{return 0;};									// スレッド終了前処理 (終了シグナル後にコールバック)
    void ThreadEvent(int nEventNo);										// スレッドイベント発生



//// メンバー変数
private:
	//// 各インスタンス
	ParamManager*						mcls_pParam;					// パラメータ管理


	// ハンドル
	HANDLE					m_evReFrameSync;							// 再同期要求 (必須)
	HANDLE					m_evReFrameSyncAuto;						// 再同期要求 (ズレ時のみ)


	//// 同期制御
	CRITICAL_SECTION		m_csLock;									// クリティカルセクション


	//// ステータス情報
	EM_MAIN_STATE			m_emMainSt;									// タスクステータス
	bool					m_bFrameSync[MAX_INS];						// フレームズレ状態 (ture:同期)
	DWORD					m_nFrameSyncNo[MAX_INS];					// フレーム同期No
};
