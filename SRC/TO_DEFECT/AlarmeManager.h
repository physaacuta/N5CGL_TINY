#pragma once

// 疵検クラス インクルード
#include "..\..\Library\KizuLibMFC\CLASS\ThreadManager.h"				// ワーカースレッドマネージャー

#include "..\..\Include\LineCommon.h"									// 疵検インクルード

// ローカルのクラス
#include "MainConst.h"
#include "StatusManager.h"												// ステータス制御クラス
#include "ParamManager.h"												// パラメータ管理クラス
#include "CoilManager.h"												// コイル情報管理クラス
#include "PlgManager.h"													// PLG管理クラス
//#include "PioManager.h"													// Pio管理クラス
#include "DmemManager.h"												// データメモリ管理クラス

#include "OpSendFunc.h"													// 表示管理送信クラス

using namespace KizuLib;

class AlarmeManager :	public ThreadManager
{

//// ローカル定数
private :

	//=========================================
	// 定数
	static const int TIME_INTERVAL_EXEC			= 60;					// 定周期処理実行 タイマ周期 [ms] (360mpmのとき 100msで 600mm進む)
	static const int TIME_INTERVAL_ALARM		= 2000;					// 欠陥検出ボイス出力期間 [ms]

	//=========================================
	// 仮想マップ
	static const int VIRTUALROW					= 1000;					// 前後100mという意味だと 100を指定

	// 警報出力用 仮想マップ
	struct TYP_VirtualMap {
		struct ARY {
			int rec;						// レコード
			int row;						// 行 (0オリジン)
		} ary[VIRTUALROW*2+1];			// 前後100mと自分の位置 (100が各表示位置 0:下流側 200:上流側)
	};
	

//// 公開関数
public:
	AlarmeManager(void);
	virtual ~AlarmeManager(void);

	//=========================================
	// プロパティ
	void SetLockMgr(LockManager* pCls)		{ mcls_pLock = pCls;}		// スレッド間同期制御クラス
	void SetStatusMgr(StatusManager* pCls)	{ mcls_pStatus = pCls;}		// ステータス制御クラスセット
	void SetParamMgr(ParamManager* pCls)	{ mcls_pParam = pCls; }		// パラメータ管理
	void SetCoilMgr(CoilManager* pCls)		{ mcls_pCoil = pCls;}		// コイル情報管理
	void SetPlgMgr(PlgManager* pCls)		{ mcls_pPlg = pCls; }		// PLG管理クラス
	void SetLogGMgr(LogFileManager* pCls)	{ mcls_pLogG = pCls;}		// ガイダンス用ログクラス
	void SetLogMgrA(LogFileManager* pCls)	{ mcls_pLogA = pCls; }		// 警報用ログクラス

	void SetQue_Pio(ThreadQueue<DmemManager::DELIVERY>* pQue) { mque_pAddDmData = pQue;}


	////=========================================
	//// 外部操作関数
	void			Init_Pato();										// 初期化 (← MI, local)
	bool			Add_Pato(char const* cKizukenNo, int nMen, int nKizuSeq, long y);	// パトライト疵 追加 (←HYRecv)
	void 			AlarmCancel_Pato(char const* cKizukenNo);			// 欠陥検出警報解除
	void 			Reset_Pato();										// リセットPB押下


	//=========================================
	// 外部アクセス
	ALARME_OUTPUT const*	GetAlarmeData() const	{ return &m_NowAlarme; }
	void SetVoicErrCancel()			{ m_bVoicErrCancel = true;}			// 設備異常ボイス出力キャンセル実行 (正常復帰するまで有効)
	void SetPatoManualOn(bool b)	{ m_bPatoManualOn = b; }			// パトライト強制ON


	//=========================================
	// 画面専用
	int				GetAlmBufCnt(int ou)const { return (int)mlst_AlmBuf[ou].size(); }	// パトライトバッファ件数


//// メンバー関数
private:
	int ThreadFirst();													// スレッド開始前処理 (スレッド実行直後(シグナルセット前)にコールバック)
	int ThreadLast();													// スレッド終了前処理 (終了シグナル後にコールバック)
    void ThreadEvent(int nEventNo);										// スレッドイベント発生
	
	void Output_Alarme();												// 疵接近 警報出力
	void Check_Alarme(ALARME_OUTPUT* p);								// 疵接近 警報出力判定

	//// データメモリ出力チェック関連
	void Check_VoiceErr();												// 設備異常ボイス出力チェック
	void Check_DetectSekkin();											// 疵接近判定 (CAPLのみ)

	// 共通処理
	void Create_VirtualMap(int pos, TYP_VirtualMap* map, int nMapMax, int nMesyu, int nRowMax);	// 表示位置を基点として、前後100mのマップを作る
	bool Check_NoAlarm(int rec, int row, int nMesyu);					// 不感帯範囲内かチェック
	bool Check_NoLamp(int rec, int row, int nMesyu);					// 不感帯範囲内かチェック

//// メンバー変数
private:
	
	//=========================================
	// 各インスタンス
	LockManager*			mcls_pLock;									// スレッド間同期制御クラス

	StatusManager*			mcls_pStatus;								// ステータス制御クラス
	ParamManager*			mcls_pParam;								// パラメータ管理
	CoilManager*			mcls_pCoil;									// コイル情報管理	
	PlgManager*				mcls_pPlg;									// PLG管理クラス
	LogFileManager*			mcls_pLogG;									// ガイダンス用ログクラス
	LogFileManager*			mcls_pLogA;									// 警報用ログクラス

	//// キュー
	ThreadQueue<DmemManager::DELIVERY>* mque_pAddDmData;				// 受け渡しデータ用キュー

	//// ハンドル
	HANDLE					m_evTimerExec;								// 定周期実行イベント

	
	//// データメモリセット判定用
	bool					m_bVoicErrCancel;							// 設備異常ボイス出力キャンセル (true:キャンセル中) (正常復帰するまで有効)
	int						m_nVoicErrNo;								// 現在出力中の設備異常ボイスNo
	int						m_nVoicDetectNo;							// 現在出力中の欠陥検出ボイスNo
	int						m_nVoicDetectTorB;							// 現在出力中の欠陥検出ボイスの表裏区分 (0:表 1:裏)
	int						m_nPato;									// パトライト出力件数
	int						m_nSekkin[2][NUM_MEN][MAP_COL_NUM];			// 疵接近ランプ/アラーム [0:表検査台 1:裏検査台][0:表面 1:裏面][列]  (2:重 1:軽 0:無し)
	
	
	//// デバック用
	bool					m_bPatoManualOn;							// パトライト強制ON

	//// タイマー関連
	UINT_PTR				m_tId_Alarme;								// 欠陥検出アラームタイマーID

	//=========================================
	// 保持データ
	ALARME_OUTPUT			m_NowAlarme;								// 現在の疵接近警報の状態


	//// 同期制御
	CRITICAL_SECTION		m_csLock;									// クリティカルセクション

	//// バッファ管理
	std::list<PATO_INF*>	mlst_AlmBuf[NUM_MEN];						// パトライト対象疵用のバッファ
};
