#pragma once

// STL
#include <vector>														// リスト用

// 半固定
#include "..\..\Include\LineCommon.h"									// 疵検インクルード
#include "..\..\Library\KizuLib\KizuLib.h"								// 疵検ライブラリ
#include "..\..\Library\KizuLib\Class\KizuMacro.h"						// マクロ集
#include "..\..\Library\KizuLib\Class\KizuFunction.h"					// スタティックライブラリ集
#include "..\..\Library\KizuLibMFC\Class\LogFileManager.h"				// ログ管理クラス

// 疵検クラス インクルード
#include "..\..\Library\KizuLib\Class\WorkThread.h"						// ワーカースレッドクラス
#include "..\..\Library\KizuLib\Class\ThreadQueue.h"					// スレッドキュークラス

#include "..\..\Library\KizuODBC\OdbcBase.h"							// DB操作クラス
#include "..\..\Library\Kizu232C\Rs232cBase.h"							// RS-232C操作クラス

#include "..\..\Include\ToTbl.h"										// 統括共有メモリテーブル インクルード

#include "MainConst.h"

using namespace KizuLib;
using namespace KizuODBC;
using namespace Kizu232C;



class CameraManager : public WorkThread
{

private :
	//------------------------------------------
	// 固定定数
	//------------------------------------------
	// RS232C設定
	static const int PARAM_232C_SENDBUF_SIZE		= 1024;				// 232Cの送信バッファサイズ
	static const int PARAM_232C_RECVBUF_SIZE		= 1024;				// 232Cの受信バッファサイズ
	
	static const int PARAM_TIMEOUT_RETRY			= 3000;				// リトライ周期
	static const int PARAM_TIMEOUT_ANSER			= 5000;				// 応答伝文タイムアウト時間


#ifdef AGCOFF
	// 露光時間機能OFF時のデフォルト値セット用 （画面からの露光時間変更のみ対応させる為の対応）
	static const int AGCOFF_DEF_InitilaExpTime		= 1800;				// 露光時間初期値 [25ｎs]
	static const int AGCOFF_DEF_AGCstep				= 5;				// AGCステップサイズ
#endif


	// カメラコントローラー応答伝文
	//#define R_SIZE										2				// 受信伝文サイズ
	#define R_NG										"?\r"			// コマンド解釈失敗
	#define R_OK										"#\r"			// コマンド正常終了
	#define R_NOTHING									">\r"			// コマンド伝文無し
	#define CR											0x0D			// 終端コード

	// メインモード
	enum ENUM_MAINMODE
	{
		MODE_NONE = 0,													// 初期状態
		MODE_OPEN_CHECK,												// オープンチェック
		MODE_START_INIT,												// 初期化
		MODE_CHECK_INIT,												// 校正初期化
		MODE_WAIT,														// 設定待ち

		MODE_PULSE_ON,													// パルス出力開始
		MODE_PULSE_OFF,													// パルス出力停止

		MODE_RO,														// 露光時間設定中
		MODE_RETRY,														// オープンリトライ中
		MODE_MANYU
	};

	// AGCセットコマンド用
	struct AGCINF {
		int				nSetCnt;										// 送信件数

		// 送信情報
		struct AGCINFDATA {
			int			camid;											// 送信するカメラID (0オリジン)
			int			nExpTime;										// 露光時間
		} AgcInfData[NUM_CAMERA];
	};
	



public:
	//------------------------------------------
	// 固定構造体
	//------------------------------------------
	// パラメータ
	struct PARAM {
		// カメラリンクスイッチ設定 (※ カメラコントローラの場合でも使用)
		int					nSwitchMasterID[NUM_CAMLINKSW];				// スイッチマスタのFPGAボードID
		char				cSwitchMasterID[NUM_CAMLINKSW][4];			// スイッチマスタのFPGAボードID ("@+ID"形式で格納)

		// 共通 (ﾗｲﾝPLG,表,裏,テストパスルの順)
		int					nPlgMul[4];									// カメラPLG逓倍率
		int					nPlgDiv[4];									// カメラPLG分周率
		int					nPlgPulse[4];								// カメラPLG信号特性 (0:差動 1:TTL)
		int					nPlgIsou[4];								// カメラPLG位相 (0:2相 1:単相)
		int					nPlgIsouSa[4];								// カメラPLG位相差 (0:アップカウント 1:ダウンカウント)

		// 共通
		int					nPlgTestInterval;							// カメラPLGテストパルス周期 (輝度測定モード時の周期) [μs]	
		int					nExSyncLT;									// LT最小周期 [25ｎs]
		int					nExSyncMinH;								// LT周期の最小値 [25ｎs]
		int					nExSyncMinL;								// LT周期の最大値の幅 最大値=(nExSyncLT-nExSyncMinL) [25ｎs]

		BOOL				bScodeChg;									// 表面状態変化判断 (false:OFF true:ON)

// AGCOFF 時には、デフォルト値をセットする ------>>
		// カメラセット・カメラ角度・表面状態別
		int					nInitialExpTime[MAX_SCODE][NUM_CAMERA];		// 露光時間初期値 [検査表面状態][カメラ台数] [25ｎs]
#ifndef VER_IDULE_CAMERA
		// 表面状態別
		double				dAGCstep[MAX_SCODE];						// AGCステップサイズ[検査表面状態]
#else
		// 表面状態・カメラセット別
		double				dAGCstep[MAX_SCODE][NUM_CAMERA];			// AGCステップサイズ[検査表面状態][カメラ台数]
#endif
//<<-----------------------------------------------
	};


public:
	CameraManager(void);
	virtual ~CameraManager(void);

	void Start() { WorkThread::Start(MainThread); };					// スレッド開始
	void Stop()  { WorkThread::Stop(); };								// スレッド終了
	void SetLogMgr(LogFileManager* pLog) { mcls_pLog = pLog; };			// ログファイルセット
	void SetClass(int nUnit, char* sSession);							// 自クラスセット


	// スレッドキュー
	ThreadQueue<COMMON_QUE>	gque_Deli;									// 受け渡しデータ用キュー

	//// 外部アクセス
	bool  IsPulsu() const { return m_bIsPulsu; }						// パルス出力中?
	// Get
	const PARAM& GetParamInf() const { return m_Param; }				// パラメータ
	int   GetExpTime(int id)   const { return m_nNowExpTime[id]; }		// 露光時間 [25ｎs]
	int   GetNowScode(int ou)  const { return m_nNowScode[ou]; }		// 現在の検査表面状態No (表/裏)
	EM_DIV_PLG GetPlgMode()	   const { return m_emPLGmode; }			// PLG区分
	
	CString GetMainModeName(ENUM_MAINMODE emMainMode);					// 指定モード名称取得
	CString GetMainModeName();											// 現モード名称取得

	// Set


	// 複数台対応用
	CString GetMyName()			{ return my_sThreadName;}
	CString GetMySession()		{ return m_cMySession;}
	int		GetMyCamIdStart()	{ return m_nMyCamIdStart;}
	int		GetMyCamNum()		{ return m_nMyCamNum;}

	bool	IsCheckMyCamId(int id) {									// 自クラス対象のカメラであればtrue
				if( id < m_nMyCamIdStart) return false;
				if( id >=m_nMyCamIdStart + m_nMyCamNum ) return false;
				return true;
	}
	int		GetCamIdxToId(int idx) { return m_nMyCamIdStart + idx;}	// 自クラスの連番からカメラIDへ
	int		GetCamIdToIdx(int id)  {									// カメラIDから自クラスの連番に変換。対象外であれば-1
				if( ! IsCheckMyCamId(id) ) return -1;
				return id - m_nMyCamIdStart;
	}	


private :
	static UINT WINAPI MainThread(LPVOID param);						// メインスレッド
	bool SetDataMailCommand(COMMON_QUE *pQue);							// 統括管理からのメールスロットのコマンド解釈
	bool SetExposureTime(int cam, int mode, int div );					// 露光時間決定
	void Send_ToDefect_AgcVal();										// 現露光時間を統括管理へ送信

	// シーケンス制御
	bool CheckNextSend();												// 次送信制御
	bool CheckDataRecv();												// 受信伝文チェック

	// 232C関連
	bool Open(bool bLogDsp);											// 232C オープン
	void Close();														// 232C クローズ
	bool Recv();														// 232C 非同期 受信
	bool Send();														// 232C 非同期 送信

	// DB取得関連
	bool GetParam();													// 動的パラメータ取得
	bool GetParamStatic();												// 静的パラメータ取得

	// コマンド関連
	char GetChannelNo(int nCamID);										// カメラIDからチャンネル番号取得


	//// 各インスタンス
	LogFileManager*			mcls_pLog;									// LogFileManagerスレッドインスタンス
	Rs232cBase*				mcls_p232c;									// 232c接続インスタンス


	// シグナル
	HANDLE					m_evRecvEnd;								// 受信完了イベント認識用
	HANDLE					m_evSendEnd;								// 送信完了イベント認識用
	HANDLE					m_evReTry;									// 再接続要求 イベント
	HANDLE					m_evAnserTimeOut;							// 応答伝文待ちタイムアウト イベント


	// 232C関連
	char					m_cRecvBuf[PARAM_232C_RECVBUF_SIZE];		// 受信バッファ
	char					m_cSendBuf[PARAM_232C_SENDBUF_SIZE];		// 送信バッファ
	char					m_cSendManyu[PARAM_232C_SENDBUF_SIZE];		// 手動送信バッファ

	// ステータス関連
	ENUM_MAINMODE			m_emMainMode;								// 現在のメインモード
	int						m_nSubMode;									// 現在のサブモード
	int						m_nCamMode;									// 現在のカメラ台数繰り返しモード
	int						m_nSwMode;									// 現在のカメラリンクスイッチ台数繰り返しモード (※ カメラコントローラの場合でも使用)
	
	// カメラコントローラー 状態
	bool					m_bIsConnect;								// カメラコントローラー 接続状態(true:接続中, false:切断中)
	bool					m_bIsPulsu;									// パルス出力中
	bool					m_bNowSend;									// 現在送信中？ (1シーケンスが完了までの間true)
	int						m_nNowExpTime[NUM_CAMERA];					// 現在の露光時間 [25ｎs]


	// その他もろもろ (複数台対応)
	int						m_nUnit;									// 機器番号(0オリジン)
	CString					my_sThreadName;								// スレッド名
	CString					m_cMySession;								// 自識別子 (Ini設定項目)
	CString					m_cPrmCommonTblName;						// パラメータテーブル名
	int						m_nMyCamNum;								// 自クラス担当 カメラ台数
	int						m_nMyCamIdStart;							// 自クラス担当 カメラID開始位置 (0オリジン)
	int						m_nMyCamIdEnd;

#ifndef VER_AVALDATA
	int						m_nCamID[NUM_CAMERA];						// カメラID (配線ミスの為の振り替え用)
#else
	int						m_nCamNum[NUM_CAMLINKSW];							// 接続カメラ台数
	int						m_nCamID[NUM_CAMLINKSW][MAX_CAMLINKSW_ALLINPUT];	// カメラID
	int						m_nInpCh[NUM_CAMLINKSW][MAX_CAMLINKSW_ALLINPUT];	// 入力CH (配線ミスの為の振り替え用)
#endif
	bool					m_bAlame[NUM_CAMERA];						// アラーム出力区分 (true:出力OK)
	int						m_nNowScode[2];								// 現在の検査表面状態No (表/裏)
	EM_DIV_PLG				m_emPLGmode;								// PLG区分 (1:ライン 2:表校正 3:裏校正 4:テスト)
	int						m_nTestTime;								// テストパルス周期 [μs]
	int						m_nExpTime;									// 露光時間 [25ｎs]

	// パラメータ
	PARAM					m_Param;									// パラメータ
	AGCINF					m_typAgcInf;								// 露光時間変更中の情報
	int						m_nFilter;									// カメラコントローラーフィルター値 [0.1μs]	
	int						m_nH;										// Hコマンド値 (0～)

	int						m_nCmdIniInputNum;							// iniファイルから指定した送信コマンド数
	std::vector<CString>	m_lstCmdIniInput;							// iniファイルから指定した送信コマンド

	// 共有メモリ
#ifdef IS_TO_CAMERA_TBL
	HANDLE					m_hCam;										// カメコン情報 ハンドル
	TO_CAMERA_TBL*			mtbl_pCam;									// カメコン情報 データ
#endif
};
