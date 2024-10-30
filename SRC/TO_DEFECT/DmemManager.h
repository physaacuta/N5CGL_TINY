#pragma once

// 疵検クラス インクルード
#include "..\..\Library\KizuLibMFC\CLASS\ThreadManager.h"				// ワーカースレッドマネージャー

// 半固定
#include "..\..\Include\LineCommon.h"									// 疵検インクルード
#include "..\..\Include\OutInterface.h"									// 他機器インクルード

#include "..\..\Include\ToTbl.h"


// ローカルのクラス
#include "MainConst.h"
#include "StatusManager.h"												// ステータス制御クラス
#include "ParamManager.h"												// パラメータ管理クラス
#include "DBManager.h"													// DB管理クラス

using namespace KizuLib;


class DmemManager :	public ThreadManager
{
/////////////////////////////////////////////////////////////////////////////////////////////////////////
public :
	//=========================================
	// 外部受け渡し情報
	enum ENUM_KUBUN {
		em_RAMP,														// 疵接近ランプ
		em_ALARM,														// 疵接近アラーム
		em_PATO,														// パトライト
		em_VOICE,														// ボイス
	};

	struct OCCUR_RAMP {
		ENUM_RAMP_STAT	ranp[NUM_MEN][OUTPUT_MESYU_SEKKIN_COL][OUTPUT_RAMP_NUM];	// 疵接近ランプの状態
	};

	struct OCCUR_ALARM {
		ENUM_RAMP_STAT	alarmPos[OUTPUT_MESYU_SEKKIN_COL][OUTPUT_RAMP_NUM];			// 疵接近ランプの状態（８列）
		ENUM_RAMP_STAT	alarm[OUTPUT_ALARM_COL][OUTPUT_RAMP_NUM];					// 疵接近アラームの状態（４列 PIO出力用）
	};

	struct OCCUR_PATO {
		int				pato[OUTPUT_PATO_POS_NUM][OUTPUT_PATO_KIND_NUM];			// パトライトの状態
	};

	struct OCCUR_VOICE {
		ENUM_VOICE_STAT	voice[OUTPUT_VOICE_NUM];									// ボイスの状態
	};

	//========================================
	//// 外部情報受け渡しキュー
	struct DELIVERY {
		ENUM_KUBUN			emNo;										// イベントNo
		void*				data;										// 構造体のポインタ
	};
	
/////////////////////////////////////////////////////////////////////////////////////////////////////////
private :
	
	//=========================================
	// 定数
	static const int PARAM_TIME_HLS_READ		= 5000;					// ヘルシー読込 定周期 タイマ周期 [ms]
	static const int PARAM_TIME_HLS_WRIT		= 1000;					// ヘルシー書込 定周期 タイマ周期 [ms]
	static const int PARAM_TIME_STATUS_READ		= 30;					// ステータス信号読み込み タイマ周期 [ms] 高速=重要 WPDなど
	static const int PARAM_TIME_STATUS_READ_LOW	= 3000;					// ステータス信号読み込み タイマ周期 [ms] 低速=単純な状態
	static const int PARAM_TIME_STATUS_WRIT		= 60;					// ステータス信号書き込み タイマ周期 [ms]
	static const int PARAM_TIME_PULSE			= 1000;					// 通常 パルス周期 [ms]
	static const int PARAM_TIME_PULSE_DEF		= 2000;					// 疵発生警報有無 パルス周期 [ms]

	static const int HB_MAX_VAL					= 9999;					// ヘルシー最大チェック値
	
/////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
	DmemManager();
	virtual ~DmemManager();

	//=========================================
	// プロパティ
	void SetLogGMgr(LogFileManager* pCls)	{ mcls_pLogG = pCls;}		// ガイダンス用ログクラスセット
	void SetLogMgrA(LogFileManager* pCls)	{ mcls_pLogA = pCls; }		// 警報用ログクラスセット
	void SetStatusMgr(StatusManager* pCls)	{ mcls_pStatus = pCls;}		// ステータス制御クラスセット
	void SetParamMgr(ParamManager* pCls)	{ mcls_pParam = pCls; }		// パラメータ管理クラスセット
	void SetQueAddSQL(ThreadQueue<DBManager::DeliveryDB>* pQue) { mque_pAddSQLData = pQue; }	// DB登録用のキュー


	//=========================================
	// イベント公開
	HANDLE		GetEvPI_WPD_KenBaseGateOn() const	{ return m_evPI_WPD_KenBaseGateOn; }	// 検査基点WPDゲート信号 [← MI]
	HANDLE		GetEvPI_WPD_KenBaseGateOff() const	{ return m_evPI_WPD_KenBaseGateOff; }	// 検査基点WPDゲート信号 [← MI]
	HANDLE		GetEvPI_WPD_DspBaseGateOn() const	{ return m_evPI_WPD_DspBaseGateOn; }	// 検査基点WPDゲート信号 [← MI]
	HANDLE		GetEvPI_WPD_DspBaseGateOff() const	{ return m_evPI_WPD_DspBaseGateOff; }	// 検査基点WPDゲート信号 [← MI]
	HANDLE		GetEvPI_WPD_KenBase() const { return m_evPI_WPD_KenBase; }					// 検査基点WPD [← MI]
	HANDLE		GetEvPI_WPD_DspBase() const { return m_evPI_WPD_DspBase; }					// 表示基点WPD [← MI]
	HANDLE		GetEvPI_WPD_ShacutF()  const { return m_evPI_ShrcutF; }						// ファーストカット信号 [← MI]
	HANDLE		GetEvPI_WPD_ShacutL()  const { return m_evPI_ShrcutL; }						// ラストカット信号 [← MI]
	HANDLE		GetEvPI_WPD_PatoReset() const { return m_evPI_PatoReset; }					// パトライトリセットPB [← MI]

	
	//========================================
	// 参照用
	SEQ_DM_RECV_BASE&	GetDmRecv() {AutoLock autolock(&m_csLock); return m_typOldDmRecv; }
	// ===>> 坂巻 2021.06.03 小コイル長材通販時、コイル長比較が正常にできない不具合修正
	DWORD				GetWpdOnLength() { return m_nWpdOnLength; }
	DWORD				GetWpdOnLengthDsp() { return m_nWpdOnLengthDsp; }
	DWORD				GetWpdOnLengthSub() { return m_nWpdOnLengthSub; }
	// <<===
	
	//========================================
	// デバック操作用
	SEQ_DM_RECV_BASE*	DebugDmRecv() { return mtbl_pRecv;}
	
	//========================================
	// スレッドキュー
	ThreadQueue<DELIVERY>			mque_AddData;						// IN 受け渡しデータ用キュー
private:
	ThreadQueue<DBManager::DeliveryDB>*	mque_pAddSQLData;				// DBクラス 受渡しデータ用キュー
	
/////////////////////////////////////////////////////////////////////////////////////////////////////////
//// 公開受渡
public : 
	void SetData_Pls(EM_EVENT_PLS_OUT emPls, int nVal = 1);				// 1秒パルス出力
	
	
/////////////////////////////////////////////////////////////////////////////////////////////////////////
//// PioManager関数
public :
	void				PoPuls_CountReset();							// カウンタリセット要求信号 [←MI]
	void				PoPuls_CountStart();							// カウンタ開始要求信号 [←MI]
	void				PoPuls_TesetWpdOn(int nPos);					// テスト用 WPD信号 ON要求信号 [←MI]
	//// 取得
	//bool				IsPlgReset()		{ return m_bPlgReset; }		// PLGカウンタリセット要求中[←MI、Dlg]
	

	bool				GetPI_KenGateYou()	const { return 1 == mtbl_pRecv->nGateKen ? true : false; }		// 溶接点ゲート信号（検査用）
	bool				GetPI_DspGateYou()	const { return 1 == mtbl_pRecv->nGateDsp ? true : false; }		// 溶接点ゲート信号（表示用）

/////////////////////////////////////////////////////////////////////////////////////////////////////////
//// メンバー関数
private :
	int ThreadFirst();													// スレッド開始前処理 (スレッド実行直後(シグナルセット前)にコールバック)
	int ThreadLast() { return 0; };										// スレッド終了前処理 (終了シグナル後にコールバック)
    void ThreadEvent(int nEventNo);										// スレッドイベント発生
	
	//========================================
	//// 処理
	void	ReadDmemHls();												// ヘルシー読み込み
	void	WritDmemHls();												// ヘルシー書き込み
	void	ReadDmemStatus(SEQ_DM_RECV_BASE& tbl);						// ステータス状態読み込み
	void	ReadDmemStatusLow(SEQ_DM_RECV_BASE& tbl);					// 低速用ステータス状態読み込み
	void	WritDmemStatus();											// ステータス状態書き込み
	void 	WritDmemRamp(OCCUR_RAMP typRamp);							// 疵接近ランプ状態書き込み
	void	WritDmemAlarm(OCCUR_ALARM typAlarm);						// 疵接近アラーム状態書き込み
	void 	WritDmemPato(OCCUR_PATO typPato);							// パトライト状態書き込み
	void 	WritDmemVoice(OCCUR_VOICE typVoice);						// ボイス状態書き込み
	void 	WritDmemVoiceReset();										// ボイス状態リセット

	int		ChangeState(int now, int old );								// 状態変化チェック
	void	UpdatePlsRate(int* nPls);									// パルスレート、長手分解能のDB更新

public:
	int		GetVoiceSts(void)	{ return m_nVoice; };					// ボイス出力状態[←Dlg]
	int		GetVoiceMen(void)	{ return m_nVoiceMen; };				// ボイス出力面[←Dlg]
	int		GetRampSts(int men, int col) { return m_nRamp[men][col]; };	// ランプ出力状態[←Dlg]
	int		GetPatoSts(int pos, int kind)	{ return m_nPato[pos][kind]; };	// パトライト出力状態[←Dlg]
	int		GetAlrmSts(int col) { return m_nAlarm[col]; };				// アラーム出力状態[←Dlg]

	
/////////////////////////////////////////////////////////////////////////////////////////////////////////
//// メンバー変数
private :

	//=========================================
	// 各インスタンス
	LogFileManager*			mcls_pLogG;									// ガイダンス用ログクラス
	LogFileManager*			mcls_pLogA;									// 警報用ログクラス
	StatusManager*			mcls_pStatus;								// ステータス制御クラス
	ParamManager*			mcls_pParam;								// パラメータ管理


			
	//========================================
	//// ハンドル
	HANDLE					m_evTimeHlsRead;							// ヘルシー信号出力
	HANDLE					m_evTimeHlsWrit;							// ヘルシー信号出力
	HANDLE					m_evTimeStatusRead;							// PIOステータス信号読み込み
	HANDLE					m_evTimeStatusWrit;							// PIOステータス信号書き込み
	HANDLE					m_evTimeAlarmePulse;						// 疵発生警報 パルス出力タイマー
	HANDLE					m_evTimeCntResetPulse;						// 制御シーケンサ 高機能カウンタリセット パルス出力タイマー
	HANDLE					m_evTimeCntStartPulse;						// 制御シーケンサ 高機能カウンタ開始 パルス出力タイマー
	HANDLE					m_evTimeTestWpdPulse;						// テスト用 WPD信号 ON要求 パルス出力タイマー
	CRITICAL_SECTION		m_csLock;									// クリティカルセクション


	//========================================
	//// ヘルシー監視用
	int						m_nState;									// シーケンサ データメモリヘルシー状態
	int						m_nHBOldVal;								// 前回の読み込みカウンター値
	
	//========================================
	// 警報関連
	int				m_nVoice;											// ボイス出力中状態
	int				m_nVoiceMen;										// ボイス出力中状態
	int				m_nPato[OUTPUT_PATO_POS_NUM][OUTPUT_PATO_KIND_NUM];	// パトライト出力中状態
	int				m_nRamp[NUM_MEN][OUTPUT_MESYU_SEKKIN_COL];			// 疵接近ランプ出力中状態
	int				m_nAlarm[OUTPUT_MESYU_SEKKIN_COL];					// 疵接近アラーム出力中状態

	//========================================
	// 共有メモリ情報
	HANDLE					m_hDataMem;									// データメモリ情報 ハンドル
	TO_SEQMEM_TBL*			mtbl_pDM;									// データメモリ情報 データ
	SEQ_DM_RECV_BASE*		mtbl_pRecv;									// データメモリ情報 の 受信エリア
	SEQ_DM_SEND_DEFECT*		mtbl_pSend;									// データメモリ情報 の 送信エリア
	SEQ_DM_RECV_BASE		m_typOldDmRecv;								// 前回値
	SEQ_DM_RECV_BASE		m_typOldDmRecvLow;							// 前回値(低速読み込み)
	
	//========================================
	// 各種ステータス
	int						m_nPlsRate[3];								// 制御シーケンサから取得したパルスレート設定値[0.0001mm/pls]
																		// [0:検査用PLG, 1:検査比較用PLG, 2:表示用PLG]
	bool					m_bInitReadOk;								// True:タスク起動後、初回読込成功 / False:タスク起動後、初回読込がまだ
	
	// ===>> 坂巻 2021.06.03 小コイル長材通販時、コイル長比較が正常にできない不具合修正
	//========================================
	//// 外部参照用
	DWORD					m_nWpdOnLength;								// WPDがON時の検査コイル長
	DWORD					m_nWpdOnLengthDsp;							// WPDがON時の表示コイル長
	DWORD					m_nWpdOnLengthSub;							// WPDがON時の比較コイル長
	// <<===

	//========================================
	//// もろもろ
	DWORD					m_nStatusReadLowInterval;					// 低速ステータス読込前回時間 [ms]

	
	//========================================
	// 今回用
	HANDLE					m_evPI_WPD_KenBaseGateOn;					// WPD 検査ゲートON
	HANDLE					m_evPI_WPD_KenBaseGateOff;					// WPD 検査ゲートOFF
	HANDLE					m_evPI_WPD_DspBaseGateOn;					// WPD 表示ゲートON
	HANDLE					m_evPI_WPD_DspBaseGateOff;					// WPD 表示ゲートOFF
	HANDLE					m_evPI_WPD_KenBase;							// WPD 検査
	HANDLE					m_evPI_WPD_DspBase;							// WPD 表示
	HANDLE					m_evPI_ShrcutF;								// ファーストカット信号
	HANDLE					m_evPI_ShrcutL;								// ラストカット信号
	HANDLE					m_evPI_PatoReset;							// パトライトリセットPB

	int						m_nOutPls[EVENT_PLS_OUT_END];				// データセット1sパルス出力 依頼
	
//private:
//	// PLGカウンタリセット
//	bool					m_bPlgReset;								// True：PLGカウンタリセット中(PLGカウンタリセット要求でTrue、Z相入力ONでFalse)
};

