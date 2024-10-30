#pragma once

// 疵検クラス インクルード
#include "..\..\Library\KizuLibMFC\CLASS\ThreadManager.h"				// ワーカースレッドマネージャー
#include "..\..\Library\KizuLib\Class\AutoLock.h"						// クリティカルセクション制御クラス
#include "..\..\Library\KizuLib\Class\LockManager.h"					// スレッド間同期制御クラス
#include "..\..\Library\KizuLib\Class\KizuPerformanceCounter.h"			// 時間計測管理クラス
#include "..\..\Library\KizuLibMFC\Class\DivNameManager.h"				// DIV定数 名称取得クラス

#include "..\..\Library\DefectLib\KizuHtRecv.h"							// 判定⇒統括間通信管理クラス


// 共通ヘッダー
#include "..\..\Include\LineCommon.h"									// ラインインクルード

// EPCヘッダー
//#include "..\..\Include\EPC\NCsmemData.h"
#include "..\..\Include\EPC\NCL2DETECT_if.h"							// I/F定義
//#include "..\..\Include\EPC\NCAttr.h"

//=====================================================================
// 判定受信ベース管理クラス
//	【全ライン】
//		・判定→統括間の通信制御、スレッド起動など基本処理を担当
//=====================================================================

// ローカル
#include "StatusBaseManager.h"											// ステータス制御クラス
#include "ParamManager.h"												// パラメータ管理クラス


using namespace KizuLib;

class HtRecvBaseManager : public ThreadManager,
						  public KizuHtRecv
{
//// メンバー定数
private :
	static const int		TIME_FRAME_SKIP_WAIT		= 10*1000;			// 1回フレームスキップが発生した場合、この時間中異常にさせる
	static const int		TIME_EDGE_NG_WAIT			= 10*1000;			// 1回エッジ不正が発生した場合、この時間中異常にさせる

	static const int		TRACKING_ITEM_REPEAT_NUM	= 20;				// トラッキングアイテムを取得する繰り返し回数
	static const int		TRACKING_ITEM_SLEEP			= 15;				// トラッキングアイテムを取得失敗後のスリープ時間(ms)

	static const int		HT_RECV_EVENT_MAX			= 60;				// 本クラスにおけるイベント数上限（※ThreadFirst内のAddEventNumの総数は本定義未満とすること）

//// 公開定数
public :
//=======================================
// 周期機能 送信依頼キュー
//=======================================
// 周期クラスから判定クラスへの受け渡し構造体
typedef struct {
	int						camset;
	int						framenum;
	int						framepos;

	NCL_DEFECT_RESULT		Resu;									// 検査結果情報
	double					AttrBase[MAX_ATTR_BASE];				// 基本特徴量
	double					AttrAngle[MAX_HT1_CAMANGLE][MAX_ATTR_ANGLE];	// 角度別特徴量
	
	// 画像データがJPEGの場合
	int						nHeight;								// 画像高さ
	int						nWidth;									// 画像幅
	bool					isBmp;									// trueの場合BMP画像と判定する
	int						nImgsize[MAX_HT1_CAMANGLE];				// 画像サイズ
	BYTE*					pImg[MAX_HT1_CAMANGLE];					// 正/乱画像 ポインタ
} DeliveryCYCLE;


//// 公開関数
public:
	HtRecvBaseManager(const char *cSession, int TorB, bool bDistPostion, int accept_num, int recv_num, int HtCy=0);
	virtual ~HtRecvBaseManager(void);


	//=========================================
	// プロパティ
	void SetLogMgr(LogFileManager* pCls)	{ mcls_pLog = pCls; ThreadManager::SetLogMgr(pCls); KizuHtRecv::SetLogMgr(pCls); };	// ログファイルセット 
	void SetLockMgr(LockManager* pCls)		{ mcls_pLock = pCls;}			// スレッド間同期制御クラス
	void SetStatusMgr(StatusBaseManager* pCls) { mcls_pStatus = pCls;};		// ステータス制御クラスセット
	void SetParamMgr(ParamManager* pCls)	{ mcls_pParam = pCls; }			// パラメータ管理クラスセット
	void SetOneFrameLen(double dVal) { m_dOneFrameLen = dVal; }
	void SetTrFrmMgr(TrackingFrame<TR_FRAME>* pCls)		{ mcls_pTrFrm = pCls; }	// フレームNoトラッキングクラス

	//void SetDistLen(DWORD len)				{ m_nDistLen = len;	}			// 表裏間距離 [mm]

	//=========================================
	// 外部アクセス
	bool	IsConnectAll()		const { for(int ii=0; ii<my_nAcceptNum; ii++) { if(STATE_NONE == my_emSockState[ii]) return false; } return true; }	// 全判定が正常 [←MainInstance]
	void	SetEvOpNewConnect()	  { SetEvent(m_evOpNewConnect); }		// 新規表示機能新規接続
	int		GetPerFramePos() const { return m_nPerFramePos; }


	// 画面用アクセス
	DWORD	GetFrameNo(int id)	const { return my_nFrameNo[id]; }		// フレームNo
	double	GetAveTime()		const { return mcls_Time.GetAveTime(); }// 平均処理時間 [ms]
	double	GetMaxTime()		const { return mcls_Time.GetMaxTime(); }// 最大処理時間 [ms]




//// メンバー関数
protected:
	virtual int ThreadFirst();											// スレッド開始前処理 (スレッド実行直後(シグナルセット前)にコールバック)
	virtual int ThreadLast();											// スレッド終了前処理 (終了シグナル後にコールバック)
    virtual void ThreadEvent(int nEventNo);								// スレッドイベント発生
	

	//=========================================
	// 仮想関数
	// 前準備段階
	virtual void ThreadFirst_AddHandle() {};							// シグナル待ち行列の追加
	virtual void OnAddSignal(int nEventNo) {};							// シグナルを追加した奴のイベント (追加したものを0始まりとする)

	// シグナル受信時
	virtual void OnRecvEnd(int id);										// 受信完了通知
	virtual void OnExecDetectWaitStart() {};//= 0;						// 検査開始待ち開始処理
	virtual void OnExecDetectRun() {};									// 検査開始処理
	virtual void OnExecDetectStop() {};//= 0;							// 検査停止処理

	virtual void OnMainDefect()	{};//= 0;								// 業務伝文 受信後のメイン処理
	virtual void OnOpNewConnect() {};//= 0;								// 新規表示機能新規接続

	virtual void OnMainHls(SOCK_F_HEAD* p) {};							// ヘルシー伝文 メイン処理 ※この処理間はブロックされているので注意


	// 小物
	virtual const char* OnGetPcName(int id)	{return HANTEI(id); };		// PC名称取得
	virtual const char* OnGetPcNameCycle(int id)	{return CYCLE(id); };		// PC名称取得

	// シーケンス制御
	void ExecDetectWaitStart();											// 検査開始待ち開始処理
	void ExecDetectRun();												// 検査開始処理
	void ExecDetectStop();												// 検査停止処理
	void MainDefect();													// 業務伝文 受信後のメイン処理



//// メンバー変数
protected:

	//=========================================
	// 各インスタンス
	LogFileManager*			mcls_pLog;									// ThreadManager,KizuHtRecv両方で定義されているため、あいまいさ解決のために宣言
	LockManager*			mcls_pLock;									// スレッド間同期制御クラス
	StatusBaseManager*		mcls_pStatus;								// ステータス制御クラス
	ParamManager*			mcls_pParam;								// パラメータ管理クラス
	KizuPerformanceCounter	mcls_Time;									// 時間測定スレッド

	TrackingFrame<TR_FRAME>*	mcls_pTrFrm;							// フレームNoトラッキングクラス

	//=========================================
	// シグナル
	HANDLE					m_evOpNewConnect;							// 新規表示機能新規接続

	//=========================================
	// その他もろもろ
	//DWORD					m_nDistLen;									// 表裏間距離 [mm]
	bool					m_bDistPostion;								// 基準面の場合ture
	int						m_nDistPos;									// 基準面(より上流の方)が裏か表か(0:表,1:裏)
	int						m_nDistNotPos;								// 基準面と逆の方(0:表,1:裏)

	//long					m_nPerFramePos;								// 前回フレームの長手位置 [mm]
	long					m_nNowMinRow;								// 現在の最小(1m)行位置 (0オリジン)
	//long					m_nNowKizuSeq;								// 疵連番 (表裏別  1～)

	int						m_nPerFramePos;								// 処理済みフレーム長手位置 [mm]
	long					m_nNowGridRow;								// 現在の送信グリッド位置 (0オリジン)
	double					m_dOneFrameLen;								// 1フレームの長さ [mm]

	int						m_nHtCy;									// 1:単発判定 2:周期判定 (GroupID)

	int						m_nTrId;									// フレームNoトラッキングID
	int						m_nTrOtherId;								// 他方面のフレームNoトラッキングID
	TR_FRAME				m_trFrm;									// フレームNoトラッキング情報

	//=========================================
	// フレームスキップ 用	
public:
	void	SetEvFrameSkip()			{ SetEvent(m_evNowFrameSkip); }
	void	SetFrameSkipId(int nVal)	{ m_nFrameSkipId = nVal; }
	bool	IsFrameSkipState() const	{ return m_bFrameSkipState; }

private:
	HANDLE					m_evNowFrameSkip;							// スキップ発生
	HANDLE					m_evTimeFrameSkip;							// スキップタイマー

	// 保持情報
	int						m_nFrameSkipId;								// スキップ機器状態ID
	bool					m_bFrameSkipState;							// スキップ状態

	void FrameSkipStart();												// スキップ 開始
	void FrameSkipEnd();												// スキップ 完了


	//=========================================
	// エッジ不正 用	
public:
	void	SetEvEdgeNg()			{ SetEvent(m_evNowEdgeNg); }
	void	SetEdgeNgId(int nVal)	{ m_nEdgeNgId = nVal; }
	bool	IsEdgeNgState() const	{ return m_bEdgeNgState; }

private:
	HANDLE					m_evNowEdgeNg;								// エッジ不正発生
	HANDLE					m_evTimeEdgeNg;								// エッジ不正タイマー

	// 保持情報
	int						m_nEdgeNgId;								// エッジ不正機器状態ID
	bool					m_bEdgeNgState;								// エッジ不正状態

	void EdgeNgStart();													// エッジ不正 開始
	void EdgeNgEnd();													// エッジ不正 完了
	

	//=========================================
	// フレームNoずれ 用	
public:
	void	SetnFrameUnmatchId(int nVal)	{ m_nFrameUnmatchId = nVal; }
	bool	IsnFrameUnmatchState() const	{ return m_bFrameUnmatchState; }

private:
	// 保持情報
	int						m_nFrameUnmatchId;							// フレームNoずれ機器状態ID
	bool					m_bFrameUnmatchState;						// フレームNoずれ状態

////////////////////////////////////////////////////////////////////////////////////////////////
// 所要時間計測 用

public:			// 画面用アクセス
	double	GetHtProcAveTime() const			{ return m_dHtProcAveTime;	}			// 画像取込～判定→統括伝送完了までの平均所要時間 [ms]
	double	GetHtProcMaxTime() const			{ return m_dHtProcMaxTime;	}			// 画像取込～判定→統括伝送完了までの最大所要時間 [ms]

	double	GetToProcAveTime() const			{ return m_dToProcAveTime;			}	// 画像取込～統括処理完了までの平均所要時間 [ms]
	double	GetToProcMaxTime() const			{ return m_dToProcMaxTime;			}	// 画像取込～統括処理完了までの最大所要時間 [ms]

	double	GetRecvAveTime() const				{ return m_dRecvProcAveTime;		}	// DS(WS)の疵情報受信開始～WS(DS)の疵情報受信開始までの平均処理時間 [ms]
	double	GetRecvMaxTime() const				{ return m_dRecvProcMaxTime;		}	// DS(WS)の疵情報受信開始～WS(DS)の疵情報受信開始までの最大処理時間 [ms]

	double	GetRecvTotalAveTime() const			{ return m_dRecvProcTotalAveTime;	}	// DS(WS)の疵情報受信開始～WS(DS)の疵情報受信完了までの平均処理時間 [ms]
	double	GetRecvTotalMaxTime() const			{ return m_dRecvProcTotalMaxTime;	}	// DS(WS)の疵情報受信開始～WS(DS)の疵情報受信完了までの最大処理時間 [ms]

	double	GetNcHtProcAveTime(int dsws) const	{ return m_dNcHtProcAveTime[dsws];	}	// 画像処理エンジン処理完了～統括伝送開始までの平均所要時間 [ms]
	double	GetNcHtProcMaxTime(int dsws) const	{ return m_dNcHtProcMaxTime[dsws];	}	// 画像処理エンジン処理完了～統括伝送開始までの最大所要時間 [ms]


protected:
	__int64					m_pf;														// 統括PC CPU周波数

	// 判定PC
	double					m_dHtProcNowTime;											// 画像取込～判定→統括伝送完了までの現在所要時間 [ms]			
	double					m_dHtProcAveTime;											// 画像取込～判定→統括伝送完了までの平均所要時間 [ms]
	double					m_dHtProcMaxTime;											// 画像取込～判定→統括伝送完了までの最大所要時間 [ms]
	// 統括PC
	double					m_dToProcNowTime;											// 画像取込～統括処理完了までの現在所要時間 [ms]
	double					m_dToProcAveTime;											// 画像取込～統括処理完了までの平均所要時間 [ms]
	double					m_dToProcMaxTime;											// 画像取込～統括処理完了までの最大所要時間 [ms]

	__int64					m_pc_RecvFirst;												// DS(WS)の疵情報受信時のタイムスタンプ
	__int64					m_pc_RecvSecond;											// WS(DS)の疵情報受信時のタイムスタンプ

	double					m_dRecvProcNowTime;											// DS(WS)の疵情報受信開始～DS(WS)の疵情報受信開始までの現在所要時間 [ms]
	double					m_dRecvProcAveTime;											// DS(WS)の疵情報受信開始～DS(WS)の疵情報受信開始までの平均所要時間 [ms]
	double					m_dRecvProcMaxTime;											// DS(WS)の疵情報受信開始～DS(WS)の疵情報受信開始までの最大所要時間 [ms]

	double					m_dRecvProcTotalNowTime;									// DS(WS)の疵情報受信開始～WS(DS)の疵情報受信完了までの現在所要時間 [ms]
	double					m_dRecvProcTotalAveTime;									// DS(WS)の疵情報受信開始～WS(DS)の疵情報受信完了までの平均所要時間 [ms]
	double					m_dRecvProcTotalMaxTime;									// DS(WS)の疵情報受信開始～WS(DS)の疵情報受信完了までの最大所要時間 [ms]

	double					m_dNcHtProcNowTime[NUM_CAM_POS];							// 画像処理エンジン処理完了～統括伝送開始までの現在所要時間 [ms]
	double					m_dNcHtProcAveTime[NUM_CAM_POS];							// 画像処理エンジン処理完了～統括伝送開始までの平均所要時間 [ms]
	double					m_dNcHtProcMaxTime[NUM_CAM_POS];							// 画像処理エンジン処理完了～統括伝送開始までの最大所要時間 [ms]



////////////////////////////////////////////////////////////////////////////////////////////////
// 出側物理距離算出用
public:
	void					InitNowOutGridRow()	{ m_nNowOutGridRow = 0; };				// 出側物理コイル単位のグリッド位置を初期化(←MI)
protected:
	long					m_nNowOutGridRow;											// 出側物理コイル単位のグリッド位置(実際にはHY送信はせず、カウント情報に用いる)
	double					m_dNowOutLength;											// 出側物理コイル単位の通板距離(判定から返って来たフレームを基に算出するので、PlgMgrの値と違う)
	double					m_dResV;													// 縦分解能 [mm/pixel]
	DWORD					m_nFrameNo;													// フレームNo


};
