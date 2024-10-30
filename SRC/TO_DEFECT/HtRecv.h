#pragma once

// インクルード
#include "..\..\Library\KizuLibMFC\CLASS\ThreadManager.h"				// ワーカースレッドマネージャー
#include "..\..\Library\KizuLib\Class\AutoLock.h"						// クリティカルセクション制御クラス

#include "..\..\Library\KizuLib\Class\KizuBitMapManager.h"			// デバッグ用
#include "..\..\Library\KizuLib\Class\KizuMacro.h"

// 共通ヘッダー
#include "..\..\Include\LineCommon.h"									// ラインインクルード
#include "..\..\Include\ToHtCommon.h"									// 統括判定専用インクルード
#include "..\..\Include\OutIF_HT_RAW.h"									// 全長定義

// EPCヘッダー
//#include "..\..\Include\EPC\NCsmemData.h"
//#include "..\..\Include\EPC\NCAttr.h"
#include "..\..\Include\EPC\NCL2DETECT_if.h"							// I/F定義
#include "..\..\Include\EPC\NCL2attr.h"									// 特徴量定義


// ローカルのクラス
#include "HtRecvBaseManager.h"											// 判定→統括間通信制御クラス
#include "StatusManager.h"												// ステータス制御クラス
#include "ParamManager.h"												// パラメータ管理クラス
#include "CoilManager.h"												// コイル情報管理クラス
#include "PlgManager.h"													// PLG管理クラス
#include "DBManager.h"													// DB登録制御クラス
#include "MergeManager.h"												// マージ処理
#include "DmemManager.h"												// データメモリ通信管理クラス
#include "AlarmeManager.h"												// 警報管理クラス

#include "EdgePosControl.h"												// エッジ検出整合性管理クラス

#ifndef DISABLE_TINY_CALC
	#include "TinyManager.h"												// 微小欠陥管理クラス
#endif
#include "DefectDumpManager.h"											// 疵情報ファイル管理クラス

// ファンクションクラス
#include "ToOutMailSender.h"											// 外部メールスロット送信モジュール


using namespace KizuLib;


class HtRecv :	public HtRecvBaseManager
{
public:
	HtRecv(const char *cSession, int TorB, bool bDistPosition);
	virtual ~HtRecv(void);

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// プライベート定数
private:
	static const int PARAM_FRAME_SKIP_CNT				= 100;			// 所要時間算出スキップフレーム数
	static const int DEFECT_BUF_CNT = MAP_COL_NUM * MAP_NEST_NUM;		// システム最小間隔での最大疵バッファリング数

public:
	// 疵情報
	// ※疵位置がフレーム範囲外の場合、次フレーム処理まで溜めておくバッファ用
	struct TYP_DEFECT_BUF
	{
		double dLen;													// 疵発生位置
		bool bRowKizuCnt;												// 疵件数加算対象(true:対象 false:対象外)
		DETECT_BASE_DATA data;											// 基本疵情報
		DEFECT_DATA_IMAGE img;											// 切り出し画像の実体
	};


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 全疵デバッグ出力
#define LOG_DEF_OUTPUT	// 定義時、全疵をデバッグ出力
#ifdef LOG_DEF_OUTPUT
private:
	LogFileManager*		mcls_pLogDef;
#endif
#ifdef DEBUG_LOG_EDGE_UDP
	LogFileManager*		mcls_pLogUdp;									// エッジ連動用ログクラス
#endif
	LogFileManager*		mcls_pLogA;										// 警報用ログクラス


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// プロパティ
public:
	//// ワーカースレッド
	void SetCoilMgr(CoilManager* pCls)		{ mcls_pCoil = pCls;}		// コイル情報管理
	void SetPlgMgr(PlgManager* pCls)		{ mcls_pPlg = pCls; }		// PLG管理クラス
	void SetDbMgr(DBManager* pCls)			{ mcls_pDbMgr = pCls; }		// DB登録制御クラス

	void SetEdgePosCont(EdgePosControl* p)	{ mcls_pEdgePosCont = p; }	// エッジ検出整合性管理クラス
#ifndef DISABLE_GAP_CALC
	void SetGapInfMgr(GapInfManager* p)		{ mcls_pGap = p; }			// 複眼ズレ集計クラス
#endif
#ifndef DISABLE_TINY_CALC
	void SetTinyMgr(TinyManager* p)			{ mcls_pTiny = p; }			// 微小欠陥クラス
#endif
	void SetDumpMgr(DefectDumpManager* p)	{ mcls_pDefDump = p; }		// 疵情報ファイル管理クラス
	void SetArmMgr(AlarmeManager* pCls)		{ mcls_pArm = pCls; }		// 警報管理クラス
	void SetLogMgrA(LogFileManager* pLog)	{ mcls_pLogA = pLog; }		// 警報用ログファイルセット

	//// キュー
	void SetQueAddSQL(ThreadQueue<DBManager::DeliveryDB>* pQue) { mque_pAddSQLData=pQue; }	// DB登録用のキュー
	void SetQueDmem(ThreadQueue<DmemManager::DELIVERY>* pQue)	{ mque_pDmemAlarme=pQue; }	// 疵発生警報情報登録用のキュー
#ifdef ENABLE_CYCLE
	void SetQueCycleData(ThreadQueue<DeliveryCYCLE>* pQue) { mque_pCycleData=pQue; }	// 周期欠陥受け渡し用のキュー
#endif




/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// イベント公開
public:


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 外部アクセス
public:
	static void Create_DetectBaseData(int men, int coilwid, int nMapCol, NCL_DEFECT_RESULT const* pResu, double const* attr, int nShrinkRatio, DETECT_BASE_DATA* data);		// 基本疵情報 生成 [← OpSendFunc]
	//////void		SetEnableAGC(bool bAgc)	{ m_bEnableAGC = bAgc; }			// AGC実行可能状態を変更[←MI] 
	int			GetBr(int cam, int angle) { return m_nBr[cam][angle]; }		// それぞれのカメラのエッジ内平均輝度 [←MI]


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 仮想関数
private:
	virtual void OnExecDetectWaitStart();											// 検査開始待ち開始処理
	virtual void OnExecDetectRun();													// 検査開始処理
	virtual void OnExecDetectStop();												// 検査停止処理

	virtual void OnOpNewConnect();													// 新規表示機能新規接続
	virtual void OnMainDefect();													// 業務伝文 受信後のメイン処理
	virtual void OnMainHls(SOCK_F_HEAD* p);											// ヘルシー伝文 メイン処理 ※この処理間はブロックされているので注意


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 個別処理
private:
	void ExecDefect(double dFramePos);												// 欠陥発生時処理
	void SetDetectInf(NCL_DEFECT_RESULT const* pResu, double const* pAttr, double dFramePos, int nShrinkRatio, DEFECT_DATA_IMAGE const* pImg);	// 個々の疵情報の処理


	//// 小物
	void ExecMerge();																// マージの実行
	void SetAlameInf(DETECT_BASE_DATA const* pD, double const* pAttr);				// 警報出力対象欠陥の判定を行う
	void Check_AGC(DWORD framenum);													// AGC実行
	void SetCoilTrand();															// コイルトレンド情報を登録
	void ClearDefectBuffer();														// 代表疵、疵リストクリア
	void ReSetOp_DetectData(int nFramePos);											// 代表疵選定バッファ初期化
	void DetectToColRow(TYP_COLROW_BUF *pBuf, double dLen, DETECT_BASE_DATA const* pData, DEFECT_DATA_IMAGE const* pImg);		// 疵情報を表示管理送信用構造体に変換
	void SetBuf_DetectData(COIL_INF const* pCoilInf, DETECT_BASE_DATA const* pData, DEFECT_DATA_IMAGE const* pImg, double dLen, double dFramePos);// 表示管理送信用 疵情報編集
	int	 SetOp_DetectData(COIL_INF const* pCoilInf, DETECT_BASE_DATA const* pData, DEFECT_DATA_IMAGE const* pImg, double dLen, TYP_COLROW_INFO *pColRow_Info);	// 代表疵選定
	bool SelectPriorityDetect(DETECT_BASE_DATA const* pNow, DETECT_BASE_DATA const* pPre);

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// メンバー変数
private:

	//// 各インスタンス
	CoilManager*			mcls_pCoil;									// コイル情報管理
	PlgManager*				mcls_pPlg;									// PLG管理クラス
	DBManager*				mcls_pDbMgr;								// DB登録制御クラス		
	EdgePosControl*			mcls_pEdgePosCont;							// エッジ検出整合性管理クラス
	DefectDumpManager*		mcls_pDefDump;								// 疵情報ファイル管理クラス
	AlarmeManager*			mcls_pArm;									// 警報管理クラス

	//// 受け渡しキュー
	ThreadQueue<DBManager::DeliveryDB>*		mque_pAddSQLData;			// DBクラス 受け渡しデータ用キュー
	ThreadQueue<DmemManager::DELIVERY>*		mque_pDmemAlarme;			// 疵発生警報情報 受け渡しデータ用キュー
#ifdef ENABLE_CYCLE
	ThreadQueue<DeliveryCYCLE>*		mque_pCycleData;			// 周期欠陥受け渡しデータ用キュー
#endif

	//// AGC関連
	//////bool					m_bEnableAGC;									// AGC実行可能(健全性評価用データ収集中はfalse)
	//////DWORD					m_nAGCSetFrameNo[NUM_CAM_POS][MAX_CAMANGLE];	// 最後にAGC設定したFrameNo [0:DS 1:WS][0:正 1:乱]
	//////int						m_nAGCLastCommand[NUM_CAM_POS][MAX_CAMANGLE];	// 最後にAGC設定したコマンド[0:DS 1:WS][0:正 1:乱](0:変更無し 1:上げ -1:下げ)	
	//////BYTE					m_nAGCBrCenter[NUM_CAM_POS][MAX_CAMANGLE];		// エッジ内平均明るさ       [0:DS 1:WS][0:正 1:乱](0～255でセット)

	
	//// その他もろもろ
	long					m_nNowGridTrandRow;							// 現在のトレンド情報グリッド位置 (0オリジン)
	int						m_nPantiTypeId;								// パンチ穴疵種ID (0なら実施しない)
	int						m_nBr[NUM_CAM_POS][MAX_CAMANGLE];			// それぞれのカメラのエッジ内平均輝度

	//=========================================
	// 表示管理への疵まとめ送信のためのワークエリア
	TYP_OP_SEND_DEFECT_INFO m_OpSendDefInfo;
};
