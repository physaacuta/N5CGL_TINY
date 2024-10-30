// *********************************************************************************
//	メイン管理スレッド
//	[Ver]
//		Ver.01    2007/01/30  vs2005 対応
//
//	[メモ]
//		
// *********************************************************************************

#pragma once

// 疵検クラス インクルード
#include "..\..\Library\KizuLibMFC\CLASS\ThreadManager.h"				// ワーカースレッドマネージャー
#include "..\..\Library\DefectLib\KizuToSend.h"							// 判定⇒統括間通信 クラス


// EPCインクルード
//#include "..\..\Include\EPC\NCcommand.h"								// 制御コマンドの定義
//#include "..\..\Include\EPC\NCsmemData.h"								// 共有メモリ上のデータ構造
#include "..\..\Include\EPC\NCL2DETECT_if.h"							// I/F定義


// 各ワーカースレッド
#include "MainConst.h"													// 自クラス定数ヘッダー
#include "StatusManager.h"												// ステータス管理クラス
#include "CommandSender.h"												// コマンド管理クラス
#include "DataRecver.h"													// EPC→HT管理クラス
#include "DataSender.h"													// HT→TO管理クラス
#include "CheckLogic.h"													// ロジックファイル有無チェッククラス
#include "ParamManager.h"												// パラメータクラス


// 全長がらみ
#include "..\..\Library\DefectLib\SockOneToAnySend.h"					// 回転寿司形式の送信クラス
#include "..\..\Include\OutIF_HT_RAW.h"									// 全長定義
#include "CullManager.h"

using namespace KizuLib;


class MainInctance : public ThreadManager
{
//// メンバー定数
private :
	//=============================
	// 本クラス受け渡し用クラス
	//=============================
	//// メール情報受け渡しキュー
	struct DELIVERY_MAIL {
		ENUM_MAININCTANCE	emNo;										// イベントNo
		COMMON_QUE			que;										// 付加情報
	};

	//=============================
	// APP 異常項目 パラメータ
	//=============================
	#define	APP_PARAM_EXEC				0x01		// EXE実行
	#define	APP_PARAM_PARAM				0x02		// パラメータ変更
	#define	APP_PARAM_COILINF			0x04		// 上位設定情報

	#define	APP_PARAM_SET_POSITION		0x10		// 内部距離カウンタ
	#define	APP_PARAM_PRELOAD_PARAM		0x20		// パラメータ事前読込
	#define	APP_PARAM_CLEAR_QUEUE		0x40		// 未実行設定のキャンセル


	#define DEFAULT_GCP_CHECK_INTERVAL	1*60*60		// カメラGCPチェック間隔[sec] ※ INIファイルに定義ない場合のデフォルト値
	#define GCP_CHECK_KEY_MAX			20			// カメラGCPチェック項目の最大

//// 公開関数
public:
	MainInctance(void);
	virtual ~MainInctance(void);

	void Start() { SetEvent(m_evThStart); }								// スレッド開始 (MainInctanceの時だけはオーバーライド)

	BOOL Init();														// 初期化処理
	void Exit();														// 終了処理


	// 外部アクセス
	void SetDeliveryMail(ENUM_MAININCTANCE emNo, COMMON_QUE * que);		// スレッドキューにメールデータ登録
	void SetLogMgrCull(LogFileManager* pCls) { mcls_pLogCull = pCls; }	// 全長処理用ログクラス

	void ResumeResponse(int nStartFrameNo, EM_DIV_HANTEI emKind, int nAns);		// 途中参加アンサー送信処理


	//// 画面用
	int			 	GetCamPair()		const { return m_nCamPair;}			// 現在のカメラペア 1～ (0:バックアップ=割り当て無し)
	int				GetMen()			const { return m_nMen; }
	int				GetNowScode(int id)	const { return m_nNowScode[id];}	// 現在の検査表面状態No
	EM_DIV_HANTEI	GetKindHt()			const { return m_emKindHt; }		// バックアップ:0 単発判定:1 周期判定:2 パララン判定:3



//// 公開変数
public:

	// 各ワーカースレッド
	ParamManager			gcls_PrmMgr;								// パラメータクラス
	StatusManager			gcls_Status;								// ステータス制御クラス
	CommandSender*			gcls_pCom;									// コマンド送信スレッド
	DataRecver*				gcls_pData;									// データ受信スレッド
	DataSender*				gcls_pSend;									// データ送信スレッド
	KizuToSend*				gcls_pSock[HT_CONNECT_NUM];					// 判定⇒統括間通信 クラス
	CheckLogic				gcls_Logic;									// ロジックファイル有無チェッククラス



//// メンバー関数
private:		
	// スレッド動作定義
	int ThreadFirst();													// スレッド開始前処理 (スレッド実行直後(シグナルセット前)にコールバック)
	int ThreadLast();													// スレッド終了前処理 (終了シグナル後にコールバック)
    void ThreadEvent(int nEventNo);										// スレッドイベント発生
	

	// プロセス制御
	bool ExecuteProcess_DetectProc();									// EPC画像処理ソフト起動
	bool ExecuteProcess_iPortViewer();									// カメラ調整ソフト 起動

	bool DisposeProcess_DetectProc();									// EPC画像処理ソフト終了
	bool DisposeProcess_iPortViewer();									// カメラ調整ソフト 終了


	// 外形算出プロセス
	bool ExecuteProcess_RectProc();										// 外形算出プロセス 起動
	bool DisposeProcess_RectProc();										// 外形算出プロセス 終了


	// シーケンス制御
	void DetectStart(COMMON_QUE* que);									// 検査開始処理
	void DetectStop(int mode, int ansRetc);								// 検査停止処理
	void HelcyStat();													// 状態取得
	void ResumeStart(COMMON_QUE* que);									// 途中参加処理

	bool DoSeqenc(int nMen, int nKind);									// 実行可能状態かチェック




	// 処理制御
	int CheckCommandPipe_Err(CommandSender::EM_PIPE_ERR emErrCode, int nDetail, BYTE const* pData); // コマンドパイプクラスよりエラー
	void SendState_App(DWORD state);									// APP異常項目パラメータ 切り替え
	void SendState_App(DWORD addr, bool onoff);							// APP異常項目パラメータ 切り替え
	void SendState_PcCanOk(bool reboot);								// 続行不可能解除
	void SendState_PcCanNot();											// 当判定PC が 続行不能
	void SendState_Grab(int nCamId, bool state);						// 画像入力異常
	void Send_ToDefect_State();											// 状態を通知
	//void Send_ToDefect_FrameReqFail(DWORD camset, DWORD FrameNo);		// フレーム画像取得失敗を通知
	void Send_CoilInfDiv(const COMMON_QUE* pQue);						// 上位情報(分割版)取得&送信メイン処理

	// コマンド制御
	int SendCommand_Init();												// 画像処理タスク 初期化コマンド
	int SendCommand_Dispose();											// 画像処理タスク 資源開放コマンド
	int SendCommand_Exit();												// 画像処理タスク 終了コマンド
	int SendCommand_Start();											// 画像処理タスク 検査開始コマンド
	int SendCommand_Stop();												// 画像処理タスク 検査停止コマンド
	int SendCommand_Param(DWORD framenum, DWORD scode);					// 画像処理タスク パラーメータ変更コマンド
	int SendCommand_CoilInf(DWORD size, BYTE const* pData);				// 画像処理タスク 上位情報設定コマンド    ※分割送信時
	int SendCommand_EdgeSearch(int ll, int lr, int rl, int rr);			// 画像処理タスク エッジ検出サーチ範囲変更コマンド

	int SendCommand_PreLoad_Param(DWORD scode);							// 画像処理タスク DBからメモリへのパラメータ読み込み設定コマンド
	int SendCommand_SetPosition(NCL_CMD_SET_POSITION* pVal);			// 画像処理タスク 内部距離カウンタ設定コマンド
	int SendCommand_ClearQueue();										// 画像処理タスク 未実行設定のキャンセルコマンド
	int SendCommand_Query_State(NCL_ANSWER_QUERY_STATE* pVal);			// 画像処理タスク 状態問い合わせ

	int SendCommand_Resume(int nStartFrameNo);							// 画像処理タスク 検査途中開始
	void ResumeStop(int mode, int ansRetc);								// 画像処理タスク 検査途中停止

	void CommandAnswerProc(int retc, DWORD addr);						// 画像処理タスク アンサー時処理

	// カメラGCPチェック関連
	void GcpCheck_StartTimer();											// カメラGCP結果の定周期チェックを開始
	void GcpCheck();													// カメラGCP結果の定周期チェック
	bool FindLine(const char* src, const char* target, char* retStr, const char* delimiter = "\r\n");	// GCP結果から指定の行を抽出


//// メンバー変数
private:
	// 各ワーカースレッド
	LogFileManager*				mcls_pLogCull;							// 全長処理用ログクラス

	// 同期制御
	ThreadQueue<DELIVERY_MAIL>	mque_DeliMail;							// メール情報受け渡しキュー
	HANDLE						m_evThStart;							// MainInctanceスレッドの実行要求
	HANDLE						m_evHelcy;								// ヘルシー取得タイマー
	HANDLE						m_evGcpCheck;							// カメラGCPチェックタイマー


	// パラメータ
	int			 				m_nCamPair;								// 現在のカメラペア 1～ (0:バックアップ=割り当て無し)
	int							m_nMen;									// 現在の面 (-1:不明  0:表  1:裏)
	int							m_nNowScode[NUM_MEN];					// 現在の検査表面状態No(0:対象Scode)
	EM_DIV_HANTEI				m_emKindHt;								// 判定機能
	DWORD						m_nAppParam;							// APP異常項目パラメータ
	bool						m_bNgCam;								// 異常カメラ担当 true


	// 画像処理エンジン
	PROCESS_INFORMATION			m_pInfo;								// 実行中のプロセスステータス
	bool						m_bDetectProc;							// 探傷プロセスが起動されているか(true:起動中)
	bool						m_bDetectProc_pd;						// 探傷プロセス(周期)が起動されているか(true:起動中)
	bool						m_bDetectProc_pr;						// 探傷プロセス(パララン)が起動されているか(true:起動中)
	bool						m_bAdjustProc;							// カメラ調整プロセスが起動されているか(true:起動中)


	// 外形算出タスク
	PROCESS_INFORMATION			m_pInfoRect;							// 実行中のプロセスステータス
	bool						m_bRectProc;							// 外形算出プロセスが実行されているか(true:起動中)
	//DWORD						m_nReSendMailSeqNo;						// 再送受信シーケンスNo格納バッファ※メールスロット抜け暫定対応


	// コイル情報分割受信対応
	DIVCOIL_MERGED_PCINFO		m_typMergedInfo;						// 分割情報受信用構造体


//// 全長画像用
public:
	SockOneToAnySend*				gcls_pSockRpi[FRAME_HT_SEND_PORT];	// 判定⇒全長処理間通信 クラス
	ThreadLoopQueue<HT_RPI_FDATA>*	gque_pSockRpi[FRAME_HT_SEND_PORT];	// フレーム画像受け渡しキュー(回転寿司形式)	[LIB⇔AP間] ※ キューサイズをAPから指定したいため、ポインタにしておく
	CullManager*					gcls_pCull[FRAME_HT_SEND_PORT];		// 間引きスレッド

private:
	void QueueAlloc(int idx, int n, int const* p);						// 全長処理送信用の回転寿司キュー確保
	void QueueFree();													// 全長処理送信用の回転寿司キュー解放

	// GCP結果チェックのテスト用
	const char test_gcp_result[2048] =
		"Model          P4_CC_04K07T_00_R\r\n"
		"Microcode      03 - 081 - 20282 - 18\r\n"
		"CCI            03 - 110 - 20277 - 05\r\n"
		"FPGA           03 - 056 - 20444 - 05\r\n"
		"Serial #       12069009\r\n"
		"User ID\r\n"
		"BiST :			Good\r\n"
		"\r\n"
		"DefaultSet     1\r\n"
		"Ext Trig       On\r\n"
		"Meas L.R.      7[Hz]\r\n"
		"Max  L.R.      31250[Hz]\r\n"
		"Exp.Mode      TriggerWidth\r\n"
		"Meas E.T.      14500[ns]\r\n"
		"Max  E.T.      3000000[ns]\r\n"
		"\r\n"
		"Test Pat.      0:Off\r\n"
		"Direction      Internal, Forward\r\n"
		"Line Delay     3.00\r\n"
		"Hor Alig Mode  Off\r\n"
		"Hor.Bin		1\r\n"
		"Flat Field     Off\r\n"
		"Color Plane Selector : All\r\n"
		"Offset         0\r\n"
		"Gain           6.00\r\n"
		"Red Gain       4.51\r\n"
		"Green Gain     5.00\r\n"
		"Blue Gain      1.00\r\n"
		"Color Matrix Selector : Factory 2\r\n"
		"Color Correction Matrix : 1.000000, 0.000000, 0.000000, 0\r\n"
		"						   0.000000, 1.000000, 0.000000, 0\r\n"
		"						   0.000000, 0.000000, 1.000000, 0\r\n"
		"Mirror         Off\r\n"
		"AOI Mode :		Off\r\n"
		"CL Speed       85MHz\r\n"
		"CL Config      Full\r\n"
		"Pixel Fmt      RGB8\r\n"
		"CPA ROI        1 - 4096\r\n"
		"USER>	\r\n";
};
