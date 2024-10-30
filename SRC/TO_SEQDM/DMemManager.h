#pragma once

// 半固定
#include "..\..\Include\LineCommon.h"									// 疵検インクルード
#include "..\..\Library\KizuLib\KizuLib.h"								// 疵検ライブラリ
#include "..\..\Library\KizuLib\Class\KizuMacro.h"						// マクロ集
#include "..\..\Library\KizuLib\Class\KizuFunction.h"					// スタティックライブラリ集
#include "..\..\Library\KizuLibMFC\Class\LogFileManager.h"				// ログ管理クラス
#include "..\..\Library\KizuLib\Class\LockManager.h"					// スレッド間同期制御クラス

// 疵検クラス インクルード
#include "..\..\Library\KizuLib\Class\WorkThread.h"						// ワーカースレッドクラス

#include "..\..\Include\ToTbl.h"										// 統括共有メモリテーブル インクルード

// ローカル
#include "actqj71e71udp1.h"

using namespace KizuLib;

class DMemManager :	public WorkThread
{
private:
	//=======================================
	// 定数定義
	//=======================================
	static const int TIME_INTERVAL_EXEC			= 30;					// 定周期処理実行 タイマ周期 [ms]


public:
	DMemManager(void);
	virtual ~DMemManager(void);

	void Start() { WorkThread::Start(MainThread); };					// スレッド開始
	void Stop()  { WorkThread::Stop(); };								// スレッド終了


	void SetLogMgr(LogFileManager* pLog) { mcls_pLog = pLog; };			// ログファイルセット
	void SetMxComponent(CActqj71e71udp1* pCom) { m_pCom = pCom; };		// MXコンポーネト 通信クラス


	//// 外部アクセス
	void SetConnect(bool flg) { m_bIsConnect = flg; };					// 接続状態設定
	bool IsConnect() const { return m_bIsConnect; };					// 接続状態取得 
	bool IsStatus() const  { return m_bIsStatus; }						// 読み書き状態


private :
	static UINT WINAPI MainThread(LPVOID param);						// メインスレッド

	void Exec_ReadWrite();												// データの読込と書込み処理
	bool MxRead();														// シーケンサデータ取得
	bool MxWrite();														// データメモリ データ書込



	//// 各インスタンス
	LogFileManager*			mcls_pLog;									// LogFileManagerスレッドインスタンス

	//// MXコンポーネト
	CActqj71e71udp1*		m_pCom;										// MXコンポーネト 通信クラス
	char					m_cAdderssRead[16];							// MXコンポーネントでの 読込み開始アドレス
	char					m_cAdderssWrite[16];						// MXコンポーネントでの 書込み開始アドレス
	BYTE					m_cDMBuf[DM_AREA_MAX][DM_DATA_MAX];			// 送受信用バッファ [0:受信バッファ 1:送信バッファ]
	

	//// 共有メモリ情報
	HANDLE					m_hDataMem;									// データメモリ情報 ハンドル
	TO_SEQMEM_TBL*			mtbl_pDM;									// データメモリ情報 データ

	//// その他もろもろ
	bool					m_bIsConnect;								// 接続状態 (true:接続)
	bool					m_bIsStatus;								// 読み書き状態

};
