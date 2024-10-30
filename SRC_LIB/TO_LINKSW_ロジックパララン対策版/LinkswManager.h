#pragma once

// 疵検クラス インクルード
#include "..\..\Library\KizuLibMFC\CLASS\ThreadManager.h"				// ワーカースレッドマネジャー

#include "..\..\Library\KizuODBC\OdbcBase.h"							// DB操作クラス
#include "..\..\Library\Kizu232C\Rs232cBase.h"							// RS-232C操作クラス

// ローカル
#include "MainConst.h"													// 自クラス定数ヘッダー

using namespace KizuLib;
using namespace KizuODBC;
using namespace Kizu232C;

class LinkswManager : public ThreadManager
{
//// メンバー定数
private:
	//------------------------------------------
	// 固定定数
	//------------------------------------------
	// RS232C設定
	static const int PARAM_232C_SENDBUF_SIZE		= 1024;				// 232Cの送信バッファサイズ
	static const int PARAM_232C_RECVBUF_SIZE		= 1024;				// 232Cの受信バッファサイズ
	
	// タイマ設定
	static const int PARAM_TIMEOUT_RETRY			= 3000;				// リトライ周期
	static const int PARAM_TIMEOUT_ANSWER			= 5000;				// 応答伝文タイムアウト時間

	// 起動要因
	#define	FACT_CYCLE_CHECK						-1					// 定周期チェック
	#define	FACT_COUNTER_CHECK						-2					// カウンタチェック

	// カメラリンクスイッチ応答伝文
	#define	R_OK									"OK"				// 応答区分
	#define	CR										0x0D				// 終端コード

	// メインモード
	enum ENUM_MAINMODE
	{
		MODE_NONE = 0,													// 初期状態
		MODE_OPEN_CHECK,												// オープンチェック
		MODE_START_UP,													// 初期化
		MODE_START_INIT,												// 検査開始設定
		MODE_SLAVE_JOIN,												// 途中参加設定
		MODE_COUNTER_RESET,												// カウンタリセット
		MODE_COUNTER_CHECK,												// カウンタチェック
		MODE_CYCLE_CHECK,												// 定周期チェック
		MODE_WAIT,														// 設定待ち
		MODE_RETRY,														// オープンリトライ中
		MODE_CHANGE_AOC_SERIALSPEED,									// AOCのシリアル速度変更
		MODE_START_SELECT_INIT,											// 検査開始設定(カメラ選択設定)
	};

//// 公開定数
public:

	// 2017.02.27 Master/SlaveをDB指定 ---------->>>>

	// CH選択モード
	enum ENUM_CH_SELECT_MODE
	{
		CH_SELECT_NON = 0,												// 未選択
		CH_SELECT_MASTER,												// Master指定
		CH_SELECT_SLAVE													// Slave指定
	};
	// 2017.02.27 Master/SlaveをDB指定 <<<<----------


	//------------------------------------------
	// 固定構造体
	//------------------------------------------
	// パラメータ
	struct PARAM
	{
		// 共通
		short				nLineBuffer;								// 格納バス幅レジスタ (1,2,3,4,6,8,10)	→ カメラタップ数
		short				nYSize;										// Ysizeレジスタ						→ 縦フレーム画像サイズ [pixel]

		// カメラリンクスイッチ設定
		struct LINKSW
		{
			BOOL			bStatus;									// ステータス (TRUE:スイッチマスタ FALSE:スイッチスレーブ)
			short			nSelectCam[MAX_CAMLINKSW_OUTPUT];			// カメラ選択 (0:出力停止 1～8:入力CH)

			// 2017.02.27 Master/SlaveをDB指定 ---------->>>>
			ENUM_CH_SELECT_MODE emChSelMode[MAX_CAMLINKSW_OUTPUT];
			// 2017.02.27 Master/SlaveをDB指定 <<<<----------

			// 2020.05.07 対象カメラのみ設定 ---------->>>>
			BOOL			bChSelCam[MAX_CAMLINKSW_OUTPUT];			// カメラ選択設定 (TRUE:対象 FALSE:対象外) ※SLAVE取り込み時のみ
			// 2020.05.07 対象カメラのみ設定 <<<<----------

		} linksw[NUM_CAMLINKSW*MAX_CAMLINKSW_FPGA];

		// 算出
		BOOL				bCamInputCh[NUM_CAMLINKSW*MAX_CAMLINKSW_FPGA][MAX_CAMLINKSW_INPUT];			// カメラ入力／未入力Ch (TRUE:入力 FALSE:未入力)
		short				nCamSelBoardNo[NUM_CAMLINKSW*MAX_CAMLINKSW_FPGA][MAX_CAMLINKSW_ALLINPUT];	// カメラ選択ボードNo (1～15:ボードNo 16:Arria5)

		//AOCシリアル関係
		int					nSerialSpeeds[NUM_CAMLINKSW*MAX_CAMLINKSW_FPGA*MAX_CAMLINKSW_INPUT];			// AOCに設定するシリアル速度

		int GetSerialSpeed( int nInput = -1) const 
		{
			int speed = 9600;
			if(0 <= nInput && nInput < _countof(nSerialSpeeds)){
				speed = nSerialSpeeds[nInput];
			}
			return speed;
		}
		int nSamplingDelay(int nInput=-1) const {
			return 0x7ff/(GetSerialSpeed(nInput)/9600);
		}
		int nSamplingTiming(int nInput=-1) const {
			if( nInput < 0 ) return 0x1046;
			return 0x1046/(GetSerialSpeed(nInput)/9600);
		}
	};


//// 公開関数
public:
	LinkswManager(void);
	virtual ~LinkswManager(void);

	void SetClass(int nUnit, char* sSession);							// 自クラスセット

	// スレッドキュー
	ThreadQueue<COMMON_QUE>	gque_Deli;									// 受け渡しデータ用キュー

	// 外部アクセス
	bool IsConnect()			const { return m_bIsConnect; }			// 接続中？

	// Get
	const PARAM & GetParamInf()	const { return m_Param; }				// パラメータ

	CString GetMainModeName(ENUM_MAINMODE emMainMode);					// 指定モード名称取得
	CString GetMainModeName();											// 現モード名称取得

	int GetDieTemp(int board)	const { return m_nDieTemp[board]; }		// FPGAダイ温度
	int GetAmbTemp(int board)	const { return m_nAmbTemp[board]; }		// 周囲温度
	__int64 GetCounterNow(int Fpga, int board) const { return m_nCounterNow[Fpga][board]; } // 現在カウンタ値
	__int64 GetCounterSet(int Fpga, int board) const { return m_nCounterSet[Fpga][board]; } // 設定カウンタ値

	// Set
	void SetCounterCheck()												// カウンタチェックイベント通知
	{
		SetEvent(m_evCounterCheck);
	}
	void SetCCMode(BOOL bMode)											// CCモード設定
	{
		m_bCCMode = bMode;
	}

	// 複数台対応用
	CString GetMyName() { return my_sThreadName; }
	CString GetMySession() { return m_cMySession; }

	// 2017.02.27 Master/SlaveをDB指定 ---------->>>>

	CString GetChSelectModeName(ENUM_CH_SELECT_MODE em)					// CH選択モード名称
	{
		if( CH_SELECT_NON    == em) { return "未選択"; }
		if( CH_SELECT_MASTER == em) { return "Master指定"; }
		if( CH_SELECT_SLAVE	 == em) { return "Slave指定"; }

		return "不明";
	}
	// 2017.02.27 Master/SlaveをDB指定 <<<<----------

	bool IsEnableRapidSerial() { return m_bEnableRapidSerial; }			// 高速通信の有効/無効


//// メンバー関数
private:
	// スレッド動作定義
	int ThreadFirst();													// スレッド開始前処理 (スレッド実行直後(シグナルセット前)にコールバック)
	int ThreadLast();													// スレッド終了前処理 (終了シグナル後にコールバック)
    void ThreadEvent(int nEventNo);										// スレッドイベント発生

	// シーケンス制御
	bool SetDataMailCommand(COMMON_QUE * pQue);							// コマンド解釈
	bool CheckNextSend();												// 次送信制御
	bool CheckDataRecv();												// 受信伝文チェック
	bool CheckDataRecv_AOC();											// 受信伝文チェック(AOC設定）

	void ChangeWaitEventNum(bool bNowSend)								// 待ちイベント数変更
	{
		if( bNowSend )	ChangeWaitNum( m_nEventWaitNum - 3 );
		else			ChangeWaitNum( m_nEventWaitNum );
	}

	// 外部通知
	void Send_ToDefect_StartInit(bool bFlg=true);						// 統括管理 へ カメラリンクスイッチ設定完了通知
	void Send_ToDefect_SlaveJoin();										// 統括管理 へ 途中参加設定完了通知
	void Send_ToDefect_SelectInit(bool bFlg=true);						// 統括管理 へ カメラ選択設定完了通知 20190205 Add

	// 232C関連
	bool Open(bool bLogDsp);											// 232C オープン
	void Close();														// 232C クローズ
	bool Recv();														// 232C 非同期 受信
	bool Send();														// 232C 非同期 送信


	bool DirectSendSpeedChange();										// LinkSwitch速度変更。

	// パラメータ関連
	bool GetParam();													// 動的パラメータ取得 (但し、現時点では動的パラメータは無し！！)
	bool GetParamStatic();												// 静的パラメータ取得

	// 2017.02.27 Master/SlaveをDB指定 ---------->>>>
	void GetParamIni();																// Iniパラメータを反映
	void SetForciblyMaster(const int nId, const int nCh, const int nSelectCam);		// 強制的にMaster設定
	// 2017.02.27 Master/SlaveをDB指定 <<<<----------

	bool CalcParam();													// 算出パラメータ取得

//// メンバー変数
private:
	// 各インスタンス
	Rs232cBase *			mcls_p232c;									// 232c接続インスタンス

	// シグナル
	HANDLE					m_evRecvEnd;								// 受信完了イベント認識用
	HANDLE					m_evSendEnd;								// 送信完了イベント認識用
	HANDLE					m_evReTry;									// 再接続要求イベント
	HANDLE					m_evAnswerTimeOut;							// 応答伝文待ちタイムアウトイベント
	HANDLE					m_evTimerCycleCheck;						// 定周期チェックタイマイベント
	HANDLE					m_evCounterCheck;							// カウンタチェックイベント

	// 232C関連
	char					m_cRecvBuf[PARAM_232C_RECVBUF_SIZE];		// 受信バッファ
	char					m_cSendBuf[PARAM_232C_SENDBUF_SIZE];		// 送信バッファ

	// ステータス関連
	ENUM_MAINMODE			m_emMainMode;								// 現在のメインモード
	int						m_nSubMode;									// 現在のサブモード
	int						m_nCamMode;									// 現在のカメラ台数繰り返しモード
	int						m_nApxMode;									// 現在のボード台数繰り返しモード
	int						m_nAocMode;									// 現在のAOC台数繰り返しモード
	int						m_nAocSubMode;								// 現在のAOCサブモード
	int						m_nRegSizeMode;								// 現在のレジスタサイズ繰り返しモード (※ 1byteより大きいサイズのレジスタアクセスに使用)
	int						m_nSwMode;									// 現在のスイッチFPGA台数繰り返しモード

	// カメラリンクスイッチ状態
	bool					m_bIsConnect;								// 接続状態 (true:接続中 false:切断中)
	bool					m_bInitialSpeedChange;

	// その他もろもろ
	int						m_nEventWaitNum;							// イベント設定数
	BOOL					m_bCCMode;									// CCモード 
																		// FALSE : カメラコントロールFPGAのEXSYNC
																		// TRUE  : フレームグラバのCC1,CC2
	int						m_nCycleTime;								// 定周期チェックタイマ周期 [msec]

	short					m_nSelectCam[NUM_CAMLINKSW*MAX_CAMLINKSW_FPGA][MAX_CAMLINKSW_OUTPUT];
	short					m_nSelectTorB[NUM_CAMLINKSW*MAX_CAMLINKSW_FPGA][MAX_CAMLINKSW_OUTPUT];

	// ハーフ版対応
	bool					m_bHalfFlg[NUM_CAMLINKSW*MAX_CAMLINKSW_FPGA];

	// 複数台対応
	int						m_nUnit;									// 機器番号(0オリジン)
	CString					m_cMySession;								// 自識別子 (Ini設定項目)
	//CString					m_cPrmCommonTblName;						// パラメータテーブル名
	//int						m_nMyCamNum;								// 自クラス担当 カメラ台数
	//int						m_nMyCamIdStart;							// 自クラス担当 カメラID開始位置 (0オリジン)

	//// 機器状態関連
	// FPGAダイ温度
	int						m_nDieTempLimit[NUM_CAMLINKSW*MAX_CAMLINKSW_FPGA];	// 温度閾値 [℃]
	int						m_nDieTempID[NUM_CAMLINKSW*MAX_CAMLINKSW_FPGA];		// 温度ID
	int						m_nDieTempStID[NUM_CAMLINKSW*MAX_CAMLINKSW_FPGA];	// 温度状態ID
	int						m_nDieTemp[NUM_CAMLINKSW*MAX_CAMLINKSW_FPGA];		// 温度 [℃]
	bool					m_bDieTempSt[NUM_CAMLINKSW*MAX_CAMLINKSW_FPGA];		// 温度状態
	// 周囲温度
	int						m_nAmbTempLimit[NUM_CAMLINKSW*MAX_CAMLINKSW_FPGA];	// 温度閾値 [℃]
	int						m_nAmbTempID[NUM_CAMLINKSW*MAX_CAMLINKSW_FPGA];		// 温度ID
	int						m_nAmbTempStID[NUM_CAMLINKSW*MAX_CAMLINKSW_FPGA];	// 温度状態ID
	int						m_nAmbTemp[NUM_CAMLINKSW*MAX_CAMLINKSW_FPGA];		// 温度 [℃]
	bool					m_bAmbTempSt[NUM_CAMLINKSW*MAX_CAMLINKSW_FPGA];		// 温度状態
	// Opt-C:Linkステータス
	int						m_nOptCLinkStID[NUM_CAMLINKSW*MAX_CAMLINKSW_FPGA][MAX_CAMLINKSW_INPUT][2];	// Opt-C:LinkステータスID
	bool					m_bOptCLinkSt[NUM_CAMLINKSW*MAX_CAMLINKSW_FPGA][MAX_CAMLINKSW_INPUT][2];	// Opt-C:Linkステータス

	// パラメータ関連
	PARAM					m_Param;									// パラメータ

	// 途中参加関連
	int						m_nSlaveJoinTorB;							// 途中参加表裏区分
	DWORD					m_nSlaveJoinFNo;							// 途中参加フレームNo (0オリジン)
	DWORD					m_nResetFNo;								// カウンタリセットフレームNo (0オリジン)
	__int64					m_nCounterNow[NUM_CAMLINKSW*MAX_CAMLINKSW_FPGA][MAX_CAMLINKSW_OUTPUT];	// 途中参加レジスタ現在値
	__int64					m_nCounterSet[NUM_CAMLINKSW*MAX_CAMLINKSW_FPGA][MAX_CAMLINKSW_OUTPUT];	// 途中参加レジスタ設定値

	// Opt-C:Linkステータス関連
	BYTE					m_nOptCLinkSt[NUM_CAMLINKSW*MAX_CAMLINKSW_FPGA][MAX_CAMLINKSW_ALLINPUT][2];			// Opt-C:Linkステータス現在値
	BOOL					m_bOptCLink_RxSt[NUM_CAMLINKSW*MAX_CAMLINKSW_FPGA][MAX_CAMLINKSW_ALLINPUT][2];		// Opt-C:Link RxStatus状態
	BOOL					m_bOptCLink_LinkSt[NUM_CAMLINKSW*MAX_CAMLINKSW_FPGA][MAX_CAMLINKSW_ALLINPUT][2];	// Opt-C:Link LinkStatus状態

	// 高速モードへの切替
	bool					m_bEnableRapidSerial;						// 高速通信(115200bps)の有効/無効

	// 要求に対する応答識別用
	COMMON_QUE				m_RecvQue;									// メールスロット通知 20190205 Add


};
