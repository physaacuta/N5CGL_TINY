#pragma once

#include "..\..\Library\KizuLibMFC\CLASS\ThreadManager.h"				// ワーカースレッドマネージャー
#include "..\..\Library\KizuLib\Class\LockManager.h"					// スレッド間同期制御クラス


// 疵検 インクルード
#include "..\..\Include\LineCommon.h"									// 疵検インクルード

//#ifdef DISABLE_ZZ_EPC
//// EPC インクルード
//#include "..\..\Include\EPC\NCspeed.h"									// EPC製 UDP情報インクルード
//#endif


// ローカルのクラス
#include "MainConst.h"													// 自クラス定数ヘッダ
#include "StatusManager.h"												// ステータス制御クラス
#include "ParamManager.h"												// パラメータ管理
#include "CoilManager.h"												// コイル情報管理クラス

#include "SeqFrameRecv.h"												// シーケンサ通信受信クラス
#include "DmemManager.h"												// データメモリ管理クラス
#include "SogyoManager.h"												// 操業状態管理クラス
#include "EdgePosControl.h"												// エッジ検出整合性管理クラス
#include "TrackingFrame.h"												// フレームトラッキングクラス
#include "EdgeRestrictSender.h"											// エッジ検出範囲指定 UDP送信クラス

#include "..\..\Library\DefectLib\Tracking.h"							// トラッキング管理クラス

#include "..\..\Include\KizuSmyu.h"										// シミュレーションヘッダー

class PlgManager : public ThreadManager
{

public:
	PlgManager();
	virtual ~PlgManager();

private :
	int ThreadFirst();													// スレッド開始前処理 (スレッド実行直後(シグナルセット前)にコールバック)
	int ThreadLast();													// スレッド終了前処理 (終了シグナル後にコールバック)
    void ThreadEvent(int nEventNo);										// スレッドイベント発生


/////////////////////////////////////////////////////////////////////////////////////////////////////////
//// メンバー定数
private:
	static const int MAX_NUM_UDP				= 1;					// UDP管理最大数
	static const int TIME_INTERVAL_DSP_PLG		= 20;					// 表示PLGの通板距離 認識タイマ周期 [ms]
	static const int TIME_INTERVAL_DSP_LEN		= 200;					// 表示基点位置のコイル通板距離 送信タイマ周期 [ms]

	static const int DSP_PLG_NUM				= 1;					// 表示用PLG数

	// シミュレーション用
	#define SAMPBUFFNUM				16									// サンプリングバッファのチャネル数
	#define SAMPBUFFSIZE			SIZE_FRAME_IMG_Y					// サンプリングバッファのサイズ
	#define SAMPCOPYNUM				64									// サンプリングバッファをコピーする数

/////////////////////////////////////////////////////////////////////////////////////////////////////////
//// プロパティ
public:
	void SetLogPlgMgr(LogFileManager* pCls)	{ mcls_pLogPlg = pCls;}		// ログクラス

	void SetLockMgr(LockManager* pCls)		{ mcls_pLock = pCls;}		// スレッド間同期制御クラス
	void SetStatusMgr(StatusManager* pCls)	{ mcls_pStatus = pCls;};	// ステータス制御クラス
	void SetParamMgr(ParamManager* pCls)	{ mcls_pParam = pCls; }		// パラメータ管理クラス
	void SetCoilMgr(CoilManager* pCls)		{ mcls_pCoil = pCls;}		// コイル情報管理クラス
	void SetSogyoMgr(SogyoManager* pCls)	{ mcls_pSogyo = pCls; }		// 操業状態管理クラス
	void SetEdgePosCont(EdgePosControl* p)	{ mcls_pEdgePosCont = p; }	// エッジ検出整合性管理クラス
	void SetDmemMgr(DmemManager* pCls)		{ mcls_pDmem = pCls; }		// データメモリ管理クラス

	void SetSeqFrameR(SeqFrameRecv* pCls)	{ mcls_pSeqFrameR = pCls; }	// シーケンサ通信受信クラス
	void SetEdgeRestMgr(EdgeRestrictSender* pCls)	{ mcls_pEdgeRestSd = pCls; }	// エッジ検出範囲指定送信クラス
	void SetTrFrmMgr(TrackingFrame<TR_FRAME>* pCls)	{ mcls_pTrFrm = pCls; }			// フレームNoトラッキングクラス
	
	
/////////////////////////////////////////////////////////////////////////////////////////////////////////
//// イベント公開
public:
	HANDLE	GetEvPLG_KenNotPos()		{ return m_evKenNotPos; }						// 基準面と異なる方の検査装置位置 溶接点通過シグナル [← MI]
	
/////////////////////////////////////////////////////////////////////////////////////////////////////////
//// 外部参照用プロパティ
public:	
	// 距離参照用
	const double* Get_pLenKenDist()			{ return &m_dLen_Ken[m_nDistPos];}	// 基準面検査装置位置での通板距離 [mm]
	const double* Get_pLenKenDistNot()		{ return &m_dLen_Ken[m_nDistNotPos]; }	// 基準面とは逆面の検査装置位置での通板距離 [mm]
	const double* Get_pLenKenDist(int men)		{ return &m_dLen_Ken[men]; }		// 指定した面の検査装置位置での通板距離 [mm]
	const double* Get_pLenDspDist()			{ return &m_dLen_BaseDsp;}			// 基準面検査装置位置での通板距離 [mm]
	bool	IsCoilChange_Ken() const		{ return m_bCoilChange_Ken; }		// 検査用コイル切り替え中時 true
	bool	IsCoilChange_Dsp(int pos) const	{ return m_bCoilChange_Dsp[pos]; }	// 表示用コイル切り替え中時 true (位置指定)
	bool	IsCoilChange_Dsp() const		{ 									// 表示用コイル切り替え中時 true（全位置）
		bool bRet = false;
		for (int ii = 0; ii < POS_DSP_END; ii++) bRet |= m_bCoilChange_Dsp[ii];
		return bRet;
	}


	// 画面用アクセス
	int		GetFrameIntervalTime()const		{ return m_nTime; }					
	double	GetDistance() const				{ return m_dDistance; }
	DWORD	GetFrameNo() const				{ return m_nFrameNo; }				 
	long	GetSpeed() const				{ return (long)((m_nSpeed * 60.0f / 1000.0f)+0.5f);}	// [mpm]
	long	GetSpeedDsp() const				{ return (long)((m_nSpeedDsp * 60.0f / 1000.0f)+0.5f);}	// [mpm]
	double	GetResV() const					{ return m_dResV; }
	double	GetResVCycle() const			{ return m_dResVCycle; }

	// 通板距離
	double	GetLen_All(int pos);												// 全部入り距離
	double	GetLen_KenDist() const			{ return m_dLen_Ken[m_nDistPos];}	// 基準面検査装置位置での通板距離 [mm]
	double	GetLen_KenDistNot() const		{ return m_dLen_Ken[m_nDistNotPos];}// 他方面検査装置位置での通板距離 [mm]
	double	GetLen_OutCut() const			{ return m_dLen_OutCut; }			// シャー位置の出側カット単位での通板距離 [mm]

	// 検査基点
	double	GetLen_Ken(int men) const		{ return m_dLen_Ken[men]; }			// 各検査装置位置での通板距離 [mm] (表/裏 別)
	double	GetLen_KenSub() const			{ return m_dLen_KenSub; }			// 基準面の検査装置位置でのPLGサブの通板距離
	double	GetCoilOldLen_Ken() const		{ return m_dCoilOldLen_Ken; }		// 基準面側検査位置の前回コイル長 [mm]
	double  GetLen_WpdKenPer() const		{ return m_dLen_WpdKenPer;}			// 検査基点溶接点手前距離 [mm]


	// 表示基点
	double	GetLen_BaseDsp() const			{ return m_dLen_BaseDsp; }			// 表示基点での通板距離 [mm]
	double	GetLen_Dsp(int pos) const			{ return m_dLen_Dsp[pos]; }		// 各表示  位置での通板距離 [mm]
	double	GetCoilOldLen_BaseDsp() const	{ return m_dCoilOldLen_BaseDsp;}	// 表示基点の前回コイル長 [mm]

	
/////////////////////////////////////////////////////////////////////////////////////////////////////////
//// 外部アクセス(コイル切替関連)
public:
	void	CoilInit();													// 初期化
	void	SetEvSampStart()		{ SetEvent(m_evSamplingStart);	}	// シグナルセット サンプリングスタート
	void	SetEvSampStop()			{ SetEvent(m_evSamplingStop);	}	// シグナルセット サンプリングストップ


	void	CoilChange_KenBase();										// 検査基点でコイル切り替え
	void	CoilChange_Ken(int ou);										// 検査装置位置でコイル切り替え [←MainInctance]
	void	CoilChange_BaseDsp();										// 表示基点位置でコイル切り替え [←MainInctance]
	void	CoilChange_OutCut();										// シャーカット位置で出側物理コイル切り替え (PIOとかで即時通知時) [←MainInctance]
	void	CoilChange_FirstCut() { m_bCoilOutChange = true; }			// ラストカット位置で出側物理コイル切り替え [←MI]
	void	CoilChange_RecvInit() { m_bRecvInit = true; }				// イニシャル伝文受信によるコイル切り替え [←MI]

	void	SetCoilChange_Frame(int ou, int nFrameNo) { m_nChgFrameNo[ou] = nFrameNo; }	// コイル切替フレームNoセット

/////////////////////////////////////////////////////////////////////////////////////////////////////////
//// 公開関数
public:
	void Alloc();														// 関数初期化

/////////////////////////////////////////////////////////////////////////////////////////////////////////
//// ContecCnt8mManager関数
public:
	// Set (必須設定)
	void SetDistPostion(int men)	{ m_nDistPos = men; m_nDistNotPos = men==0?1:0; }	// 基準面(より上流の方)
	void SetDistLen(DWORD len)		{ m_nDistLen = len;}							// 表裏間距離 [mm]
	// Get
	DWORD	GetDistLen()	const			{ return m_nDistLen; }		// 表裏間距離 [mm]
	int		GetDistPos()	const			{ return m_nDistPos; }		// 基準面(より上流の方)が裏か表か(0:表,1:裏)
	int		GetDistNotPos()	const			{ return m_nDistNotPos; }	// 基準面と逆の方(0:表,1:裏)

/////////////////////////////////////////////////////////////////////////////////////////////////////////
//// メンバー関数
private:

	//// 初期化
	void	ClearMember();												// 初期化

	// 通板距離管理関連
	void NextLength_Ken(SEQ_TYPE_DATA_FRAME const* p);					// 検査基点での距離管理 (1フレーム取り込み完了)
	void NextLength_Dsp();												// 表示基点での距離管理
	void NextLength_Dsp(double dDistance);								// 表示基点での距離管理
	void ExecCycle();													// 定周期処理

	//// 実績収集関連
	void	CheckIntervalStatus();										// 最小区間の状態把握
	
/////////////////////////////////////////////////////////////////////////////////////////////////////////
//// メンバー変数
private:
	// 各インスタンス
	LogFileManager*			mcls_pLogPlg;
	LockManager*			mcls_pLock;									// スレッド間同期制御クラス
	StatusManager*			mcls_pStatus;								// ステータス制御クラス
	ParamManager*			mcls_pParam;								// パラメータ管理
	CoilManager*			mcls_pCoil;									// コイル情報管理
	SogyoManager*			mcls_pSogyo;								// 操業状態管理クラス
	DmemManager*			mcls_pDmem;									// データメモリ管理クラス
	EdgePosControl*			mcls_pEdgePosCont;							// エッジ検出整合性管理クラス
	SeqFrameRecv*			mcls_pSeqFrameR;							// シーケンサ通信受信クラス
	EdgeRestrictSender*		mcls_pEdgeRestSd;							// エッジ検出範囲指定送信クラス
	TrackingFrame<TR_FRAME>*	mcls_pTrFrm;							// フレームNoトラッキングクラス

	// 自クラス内で生成
	KizuPerformanceCounter	mcls_Time;									// 時間測定スレッド
	KizuPerformanceCounter	mcls_TimeDspSpeed;							// 表示用速度算出用

	// 同期シグナル
	HANDLE					m_evKenNotPos;								// 基準面と異なる方の検査装置位置 溶接点通過シグナル		
	HANDLE					m_evSamplingStart;							// サンプリングスタート
	HANDLE					m_evSamplingStop;							// サンプリングストップ
	HANDLE					m_evTimer;									// 定周期タイマー用
	HANDLE					m_evTimerDspLen;							// 表示距離把握用に定周期タイマー

	// パルスレート
	double					m_dPlsR_Ken;								// 検査用 パルスレート
	double					m_dPlsR_Dsp;								// 表示用 パルスレート
	double					m_dPlsR_Sub;								// 比較用 パルスレート

	// PLG関連
//#ifdef DISABLE_ZZ_EPC
//	nc_speed_info			m_udp;										// UDP送信情報 (表裏共通)
//#endif
#ifdef DISABLE_ZZ_EPC
	DEBUG_SPEED_INFO		m_udp;										// PLG	UDP送信情報 (表裏共通) ※ZZ_EPC用の速度UDP
#endif
	TR_FRAME				m_trFrm;									// トラッキングフレーム情報
	int						m_nDly[NUM_MEN];							// 遅延実行キー 実行識別 (0:無し 1～:その値で実行)
	int						m_nTime;									// フレーム取込周期 [ms]
	double					m_dDistance;								// １フレームで進んだ距離 [mm]

	DWORD					m_nFrameNo;									// フレームNo (0オリジン)
	DWORD					m_nSpeed;									// ライン速度 [mm/sec]
	DWORD					m_nSpeedDsp;								// ライン速度（表示PLG） [mm/sec]
	double					m_dResV;									// 縦分解能 [mm/pixel]
	double					m_dResVCycle;								// 毎回縦分解能 [mm/pixel] (こっちが従来の縦分解能)

	DWORD					m_nPreExSync;								// ExSyncの前回値

#ifndef PLG_DSP_KEN_EQAL
	// 表示関連
	DWORD					m_preCountDsp;								// PLGの前回値
#endif
	
	DWORD					m_preLen_KenSub;							// 比較用PLG測長距離の前回値

	DWORD					m_nChgFrameNo[NUM_MEN];						// コイル切替フレームNo (0オリジン)

	//=========================================
	// 距離管理
	double					m_dLen_Ken[NUM_MEN];						// 各検査装置位置での通板距離 [mm] (面別)
	double					m_dLen_KenSub;								// 基準面の検査装置位置でのPLGサブの通板距離 [mm] (単なるデバック用)
	double					m_dLen_WpdKenPer;							// 溶接点手前距離 WPDON時に検査装置位置で溶接点を見たときの距離 [mm] (ようは、WPD ONで 10,9･･･0, -1, -2･･･となる)

	double					m_dLen_BaseDsp;								// 表示基点での通板距離       [mm] (ようは表示基点WPD位置での通板距離)
	double					m_dLen_Dsp[MAX_POS_INDEX];					// 各表示位置での通板距離 [mm] (入側単位)
	double					m_dLen_OutCut;								// 出側カット単位での通板距離 [mm] (ようは出側物理コイル単位での通板距離)

	// 現在 / 前回値用 (検査基点用)
	double					m_dCoilOldLen_Ken;							// 基準面側検査位置の前回コイル長 [mm]
	bool					m_bCoilChange_Ken;							// 基準面ともう片方の面の切り替え中時 (切り替え中 true) (検査基点で全部ON)

	// 現在 / 前回値用 (表示基点用)
	double					m_dCoilOldLen_BaseDsp;						// 表示基点WPD位置の前回コイル長 [mm]
	bool					m_bCoilChange_Dsp[MAX_POS_INDEX];			// 各表示の切り替え中時 (切り替え中 true)  (表示基点で全部ON)
	
	//
	//=========================================
	// その他もろもろ
	bool					m_bMaxLen_Ken;								// コイル長上限到達
	bool					m_bMaxLen_Dsp;

	EM_DIV_PLG				m_emIsLinePLG;								// ラインPLGの状態
	EM_DIV_UNTEN			m_emUnten;									// 運転状態
	bool					m_bIsDisableDspPLG;							// 表示PLG無効(false:初期値,true:無効)
	bool					m_bDebugMode;								// デバック時 true
	bool				 	m_bCoilOutChange;							// 出側物理コイル切り替えフラグ (false:初期値,true:ファーストカット)	
	bool				 	m_bRecvInit;								// イニシャル伝文受信フラグ(false:初期値,true:イニシャル伝文受信)	

	int						m_nDistPos;									// 基準面(より上流の方) (0オリジン)
	int						m_nDistNotPos;								// 基準面(より上流の方) (0オリジン)
	int						m_nDistLen;									// 表裏間距離 [mm]

	// UDP関連
	BOOL			SetUdp(int no, const char* ip, WORD port);			// UDP設定
	SOCKET			GetSock(int no) const	{ return m_sock[no]; }		// UDPソケットインスタンス
	sockaddr_in*	GetAddr(int no)			{ return &m_addr[no]; }		// UDPソケットsockaddr_in構造体

	SOCKET					m_sock[MAX_NUM_UDP];						// ソケット本体		
	sockaddr_in				m_addr[MAX_NUM_UDP];						// sockaddr_in構造体

	DWORD					m_nMin;
	DWORD					m_nMax;

};

