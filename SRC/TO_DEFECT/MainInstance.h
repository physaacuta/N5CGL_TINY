// *********************************************************************************
//	メイン管理スレッド
//	[Ver]
//		Ver.01    2007/01/30  vs2005 対応
//
//	[メモ]
//		
// *********************************************************************************

#pragma once

// 標準ライブラリ
#include <vector>

// 疵検クラス インクルード
#include "..\..\Library\KizuLibMFC\CLASS\ThreadManager.h"				// ワーカースレッドマネージャー

// 疵検ライブラリ インクルード
#include "..\..\Library\DefectLib\Tracking.h"							// トラッキング管理クラス
#include "..\..\Library\DefectLib\KizuOpSend.h"							// 統括→表示・操作PCへの送信クラス
#include "..\..\Library\KizuLibMFC\Class\DivNameManager.h"				// 疵検ライブラリヘッダー

// 各ワーカースレッド
#include "MainConst.h"
#include "ParamManager.h"												// パラメータ管理クラス
#include "StatusManager.h"												// ステータス制御クラス
#include "DBManager.h"													// DB登録制御クラス
#include "CoilManager.h"												// コイル情報管理クラス
#include "PlgManager.h"													// PLG管理クラス
#include "HtRecv.h"														// 判定⇒統括間通信管理クラス
#include "SogyoManager.h"												// 操業状態管理クラス
#include "SeqFrameRecv.h"												// シーケンサ通信受信クラス
#include "AlarmeManager.h"												// 警報出力管理クラス
#include "DmemManager.h"												// データメモリ通信管理クラス

// 単機能クラス
#include "EdgePosControl.h"												// エッジ検出整合性管理クラス
#include "StatusLogFunc.h"												// ステータスログクラス
#include "CamLinkSpeedManager.h"										// カメラリンクスイッチ用 シリアル通信速度変更クラス
#include "EdgeRestrictSender.h"											// エッジ検出範囲指定 UDP送信クラス

// ファンクションクラス
#include "OpSendFunc.h"													// 統括→表示 送信情報生成ツール
#include "IPortViewerManager.h"											// iPortViewerコマンド受付クラス
#include "CmdSender.h"													// 調整用レコーダー UDP通信クラス
#include "ToLinkswUtil.h"												// カメラリンクスイッチ設定ユーティリティ
#include "LogicParaRunFunc.h"											// ロジックパララン関連処理モジュール

using namespace KizuLib;


class MainInstance : public ThreadManager
{
public:
	MainInstance(void);
	virtual ~MainInstance(void);

	void Start() { SetEvent(m_evThStart); }								// スレッド開始 (MainInstanceの時だけはオーバーライド)
	BOOL Init();														// 初期化処理
	void Exit();														// 終了処理
	void SetLogMgr(LogFileManager* pLog) { mcls_pLog = pLog; }			// ログファイルセット
	
private:
	int ThreadFirst();													// スレッド開始前処理 (スレッド実行直後(シグナルセット前)にコールバック)
	int ThreadLast() { return 0; }										// スレッド終了前処理 (終了シグナル後にコールバック)
	void ThreadEvent(int nEventNo);										// スレッドイベント発生


////////////////////////////////////////////////////////////////////////////////////////////////////////////
//// 本クラス受け渡し用クラス
//// メール情報受け渡しキュー
private :
	struct DELIVERY_MAIL {
		ENUM_MAININSTANCE	emNo;										// イベントNo
		COMMON_QUE			que;										// 付加情報
	};
	
////////////////////////////////////////////////////////////////////////////////////////////////////////////
//// タイマー用定数
private:
#ifndef LOCAL
	static const int PARAM_TIMEOUT_RESTART				= 30000;//15000//3000;	// 検査強制再起動 待ちタイマー
	static const int PARAM_TIMEOUT_DETECT_INIT_START	= 120000;		// カメラリンクスイッチ設定アンサー待ちタイマー
	static const int PARAM_TIMEOUT_WAIT_START			= 60000;//120000;		// 検査開始 判定アンサー待ちタイマー
	static const int PARAM_TIMEOUT_WAIT_STOP			= 7000;			// 検査停止 判定アンサー待ちタイマー
#else
	static const int PARAM_TIMEOUT_RESTART				= 15000;		// 検査強制再起動 待ちタイマー
	static const int PARAM_TIMEOUT_DETECT_INIT_START	= 120000;		// カメラリンクスイッチ設定アンサー待ちタイマー
	static const int PARAM_TIMEOUT_WAIT_START			= 60000;		// 検査開始 判定アンサー待ちタイマー (HT_DEFECTの"PROCESS_INIT_WAIT_TIME"+"PROCESS_EXIT_WAIT_TIME"より長く設定すること)
	static const int PARAM_TIMEOUT_WAIT_STOP			= 7000;			// 検査停止 判定アンサー待ちタイマー
#endif
	
////////////////////////////////////////////////////////////////////////////////////////////////////////////
//// 外部アクセス
public:
	void SetDeliveryMail(ENUM_MAININSTANCE emNo, COMMON_QUE * que);		// スレッドキューにメールデータ登録
	void SetLogMgrSeq(LogFileManager* pCls) { mcls_pLogSeq = pCls; }	// シーケンサ送受信用ログクラス
	void SetLogMgrE(LogFileManager* pCls) {  mcls_pLogE = pCls; }		// エッジUDP用ログクラス
	void SetLogMgrG(LogFileManager* pCls) { mcls_pLogG = pCls; }		// ガイダンス用ログクラス
	void SetLogMgrD(LogFileManager* pCls) { mcls_pLogD = pCls; }		// 調査用ログクラス
	void SetLogMgrA(LogFileManager* pCls) { mcls_pLogA = pCls; }		// 警報用ログクラス
////////////////////////////////////////////////////////////////////////////////////////////////////////////
//// シーケンス制御
public:
	void AllDetectStop(bool bMode, bool bRetry);						// 全停止
	void DetectReStart();												// 強制検査再起動 要求 (強制検査再起動中へ)
	void DetectInitStart(COMMON_QUE * que);								// 検査初期化処理			(カメラリンクスイッチ設定完了アンサー待ちへ)
	void DetectWaitStart();												// 検査開始待ち開始処理 (判定検査開始アンサー待ちへ)
	void DetectRun();													// 検査中開始処理 (検査中へ)
	void DetectStop(bool bMode);										// 検査停止 処理 (判定検査停止アンサー待ちへ)
	void WaitInit_TimeOut();											// 検査初期化待ち処理		(タイムアウト発生)
	void WaitInit_Answer(COMMON_QUE* que);								// 検査初期化待ち処理		(アンサー受信)
	void WaitStart_Answer(COMMON_QUE* que);								// 検査開始待ち 処理 (アンサー受信)
	void WaitStop_Answer(COMMON_QUE* que);								// 検査停止待ち 処理 (アンサー受信)
	void WaitStart_TimeOut();											// 検査開始待ち 処理 (タイムアウト発生)
	void DetectRun_Cancel(int ou);										// 指定面のみの検査中断
	void HT_StatAnswer(COMMON_QUE* que);								// 判定問い合わせアンサー

	void ChangeCoil_Ken(int ou);										// 各検査装置位置でのコイル切り替え

////////////////////////////////////////////////////////////////////////////////////////////////////////////
//// WPD制御
private:
	void WPD_KenBase(int bIsWpd, double *pSpmDiff);						// 検査用WPD認識
	void WPD_DspBase();													// 表示用WPD認識



////////////////////////////////////////////////////////////////////////////////////////////////////////////
//// その他
private:
	//void Cut_PiOn();													// PI シャーカット信号ON
	void SetPosition_CoilChange(DWORD nNowFrame, double dKenDist, double dKenDistNot, double dSpmDiff);	// コイル切替時の内部距離カウンタ設定要求
	void SetPosition_ForceCoilChange(DWORD nNowFrame, double dKenDist, double dKenDistNot);				// コイル強制切替時の内部距離カウンタ設定要求
	void SetParam_NextCoilInfRecv();									// 次コイル情報受信時のパラメータ設定要求
	void Add_Synclock(COMMON_QUE* que);									// DB登録状況問い合わせ

////////////////////////////////////////////////////////////////////////////////////////////////////////////
//// 外部タスク通知
public:
	// HT_DEFECT
	static void Send_HtDefect_Param(int frame, int id, int scode, EM_DIV_HANTEI hantei = DIV_HANTEI_NON);	// 判定PC へ パラメータ変更を通知 [←ParamManager, GapInfManger]
	void		Send_HtDefect_Start();																		// 判定PC へ 検査開始を通知 [←MI]
	static void Send_HtDefect_Pcinfo(TYPE_EPC_COIL* pData);													// 判定PC へ 上位情報送信
	static void Send_HtDefect_JpegQuality(int nJpegQuality);												// 判定PC へ JPEG画像圧縮率設定送信  [←MI, ParamManager]
	void		Send_HtDefect_PcCanOk();																	// 判定PC へ 続行不可能解除	[←Dlg, MI]
	static void Send_HtDefect_ClearParam(int id, EM_DIV_HANTEI hantei);										// 判定PC へ 未実行設定のキャンセル依頼 [←MI]
	static void Send_HtDefect_SetPosition(int id, EM_DIV_HANTEI hantei, NCL_CMD_SET_POSITION* cmd);			// 判定PC へ 内部距離カウンタ設定要求 [←PlgManager]
	static void Send_HtDefect_PreLoadParam(int id, EM_DIV_HANTEI hantei, int scode);						// 判定PC へ パラメータ事前読込 [←CoilManager]
	// HY_DEFECT
	static void	Send_HyDefect_Param(int id);																// 表示管理に パラメータ変更を通知 [←ParamManager]
	void		Send_HyDefect_MemDataInit();																// 表示管理に表示情報初期化通知
	static void Send_HyDefect_MaeRead(char const* cKizukenNo);												// 表示管理に 前工程疵情報読込みを通知 [←MaeManager, CoilBaseManager]
	// TO_GETDAT
	//static void Send_ToGetdat_Param(int id);																// 入力データ編集管理に パラメータ変更を通知 [←ParamManager]
	// TO_PUTDAT
	//static void Send_ToPutdat_Param(int id);																// 実績編集管理に パラメータ変更を通知 [←ParamManager]
	//static void Send_ToPutDat_Synclock(int okng);															// 実績編集タスクへ 登録完了通知 [←DBManager]
	// TO_MAEDAT
	//static void Send_ToMaedat_Param(int id);																// 前工程実績編集管理に パラメータ変更を通知 [←ParamManager]
	static void Send_ToMaedat_CoilRecv(char* KizukenNo, int rec);											// 前工程実績編集に 検査用コイル情報受信完了 を通知 [←CoilManager]
	// SO_GAMENN
	void		Send_SoGamenn_MailStatus();																	// オペレータ画面に現在の状態通知 [←Dlg]
	// KS_SLAVE
	static void	Send_KsMaster_RebootPc(char* cPcName);														// PCマスター管理に再起動要求通知 [←StatusManager]
	// SO_DEFMAP
	static void Send_SoDefMap_Param(int id);																// 疵点マップ画面にパラメータ変更を通知 [←ParamManager]

private:
	// HT_DEFECT
	void		Send_HtDefect_Status();																		// 判定PC へ 状態問合せ
	void		Send_HtDefect_Stop();																		// 全判定PC へ 検査停止を通知
	void		Send_HtDefect_Stop(int men);																// 該当面の判定PC、周期PC へ 検査停止を通知
	
////////////////////////////////////////////////////////////////////////////////////////////////////////////
//// 公開変数
public:
	// 各ワーカースレッド
	LockManager					gcls_Lock;									// スレッド間同期制御クラス
	ParamManager*				gcls_pParam;								// パラメータ管理クラス
	StatusManager				gcls_Status;								// ステータス制御クラス
	DBManager					gcls_DB;									// DB登録制御クラス
	CoilManager*				gcls_pCoil;									// コイル情報管理クラス
	HtRecv*						gcls_pHtRecv[NUM_MEN];						// 判定⇒統括間通信管理クラス (0:表 1:裏)

	PlgManager					gcls_Plg;									// PLG管理クラス
	AlarmeManager				gcls_Arm;									// 警報出力管理クラス
	DmemManager					gcls_Dmem;									// データメモリ通信管理クラス
	SeqFrameRecv*				gcls_pSeqFrameR;							// シーケンサ通信受信クラス
	KizuOpSend*					gcls_pOpS[HY_SOCK_NUM];						// 統括→表示 送信クラス
	OpSendFunc					gcls_OpSend;								// 統括→表示 送信情報生成モジュール
	SogyoManager*				gcls_pSogyo;								// 操業状態管理クラス
	MaeManager					gcls_Mae;									// 前工程情報管理クラス

	DefectDumpManager*			gcls_pDefDump[NUM_MEN];						// 疵情報ファイル管理クラス
	
	CamLinkSpeedManager			gcls_CamLinkSpdMgr;							// カメラリンクスイッチ用 シリアル通信速度変更クラス
		
	// トラッキング関連
	Tracking*					gcls_pTrKenDistPos;							// 検査基点～基準面検査装置位置までの トラッキング
	TrackingList*				gcls_pTlCoilOutSt;							// 検査基点～シャー位置までの トラッキング (出側物理コイル単位の状態管理用トラッキング)

	// 完全流用
	EdgePosControl				gcls_EdgePosCont;							// エッジ検出整合性管理クラス
	StatusLogFunc*				gcls_pStatusFunc;							// ステータスログクラス
	IPortViewerManager			gcls_IPort;									// IPortViewerManagerコマンド受付クラス

	TrackingFrame<TR_FRAME>		gcls_TrFrm;									// FrameNoでトラッキング可能とするクラス
#ifdef ENABLE_RECORDER_UDP
	CmdSender					gcls_CmdSender;								// 調整用レコーダー UDP通信クラス
#endif
#ifdef ENABLE_RPI_SAVE
	IfRpiSave*					mcls_pIfRpiSave;
#endif
	EdgeRestrictSender			gcls_EdgeRestSd;							// エッジ検出範囲指定 UDP通信クラス

////////////////////////////////////////////////////////////////////////////////////////////////////////////
//// メンバー変数
private:

	// 各ワーカースレッド
	LogFileManager*				mcls_pLogSeq;								// シーケンサ受信用ログ出力
	LogFileManager*				mcls_pLogE;									// エッジUDP用ログ出力
	LogFileManager*				mcls_pLogG;									// ガイダンス用ログ出力
	LogFileManager*				mcls_pLogD;									// 調査用ログクラス
	LogFileManager*				mcls_pLogA;									// 警報用ログクラス


	// 同期制御
	ThreadQueue<DELIVERY_MAIL>	mque_DeliMail;								// メール情報受け渡しキュー
	HANDLE						m_evThStart;								// MainInstanceスレッドの実行要求

	// 同期シグナル（タイマー）
	HANDLE						m_evTimeReStart;							// 強制検査再起動待ちタイマー
	HANDLE						m_evTimeInitStart;							// カメラリンクスイッチ設定アンサー待ちタイマー
	HANDLE						m_evTimeWaitStart;							// 検査開始 判定アンサー待ちタイマー


	// 同期シグナル (トラッキング)
	HANDLE								m_evTr_Ken;							// Tr 検査基点WPD～基準面検査装置位置までのトラッキング
	HANDLE								m_evTr_KenGate;						// Tr ゲート信号ON～検査基点までのトラッキング
	HANDLE								m_evTr_DspGate;						// Tr ゲート信号ON～表示基点までのトラッキング

	NCL_CMD_SET_POSITION		m_strCmd[NUM_MEN];							// 前回WPD信号受信時の内部距離カウンタ構造体
	
	bool						m_bIsWpdGateTrKen;							// 検査用WPDゲート信号ON～検査用WPD信号ONの間trueになるフラグ
	bool						m_bIsWpdGateTrDsp;							// 表示用WPDゲート信号ON～表示用WPD信号ONの間trueになるフラグ

	// 全長画像保存関連
#ifdef ENABLE_RPI_SAVE
	
	IfRpiSave							gcls_IfRpiSave;
#endif

	DWORD						m_nFrameNo_V[NUM_MEN][NUM_CAM_POS];			// 論理フレームNo (WPD ON時のフレームNoを保持)

};
