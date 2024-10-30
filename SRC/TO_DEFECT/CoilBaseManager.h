//=====================================================================
// コイル情報基本管理クラス
//	【全ライン】
//		・コイルの基本シーケンス、バッファ領域の管理を行う
//		・基本的には、全ラインほとんど同一構成とできると思うが、完全流用にはできないと思っている
//=====================================================================

#pragma once

#include <winsock2.h>													// UDP送信用

// φωクラス インクルード
#include "..\..\Library\KizuLibMFC\CLASS\ThreadManager.h"				// ワーカースレッドマネージャー
#include "..\..\Library\DefectLib\TrackingList.h"						// 複数トラッキング管理クラス

// 共通ヘッダー
#include "..\..\Include\LineCommon.h"									// 疵検インクルード
#include "..\..\Include\ToTbl.h"										// 統括共有メモリテーブル インクルード
#include "..\..\Include\OutInterface.h"									// 外部インターフェース用インクルード

// ローカルのクラス
#include "MainConst.h"
#include "StatusManager.h"												// ステータス制御クラス
#include "ParamManager.h"												// パラメータ管理
#include "DBManager.h"													// DB登録制御クラス
#include "EdgePosControl.h"												// エッジ検出整合性管理クラス
#include "MaeManager.h"													// 前工程情報管理クラス
#include "DmemManager.h"												// データメモリ管理クラス

#ifdef ENABLE_RPI_SAVE
	#include "IfRpiSave.h"												// 太平製全長画像処理とのI/Fクラス
#endif


using namespace KizuLib;

class CoilBaseManager :	public ThreadManager
{

public:
	CoilBaseManager(void);
	virtual ~CoilBaseManager(void);

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
// プロパティ
public:
	//// ワークスレッド
	void SetStatusMgr(StatusManager* pCls)		{ mcls_pStatus = pCls; }					// ステータス制御クラスセット	
	void SetParamMgr(ParamManager* pCls)		{ mcls_pParam = pCls; }						// パラメータ管理クラスセット
	void SetDbMgr(DBManager* pCls)				{ mcls_pDbMgr = pCls; }						// DB登録制御クラス
	void SetEdgePosCont(EdgePosControl* p)		{ mcls_pEdgePosCont = p; }					// エッジ検出整合性管理クラス
	void SetMaeMgr(MaeManager* pCls)			{ mcls_pMae = pCls; }						// 前工程情報管理
	void SetDmemMgr(DmemManager* pCls)			{ mcls_pDmemMgr  = pCls; }					// データメモリ管理クラスセット

	//// 受け渡し用キュー
	void SetQueAddSQL(ThreadQueue<DBManager::DeliveryDB>* pQue) { mque_pAddSQLData = pQue;}	// DB登録用のキュー

	//// その他諸々
	void SetDistPostion(int men){ m_nDistPos=men; m_nDistNotPos= 0==men?1:0; }				// 基準面(より上流の方)が裏か表か(0:表,1:裏)

	//// スレッドへの通知用シグナル
	void SetEvNextCoilKen()		{ SetEvent(m_evNextCoilKen); }								// 検査用 コイル情報受信シグナル
	void SetEvNextCoilDsp()		{ SetEvent(m_evNextCoilDsp); }								// 表示用 コイル情報受信シグナル

	//// クリティカルセクション
	CRITICAL_SECTION* GetCsLock() { return &m_csLock; }										// [←MainInstance経由で OpSendFunc]


	void SetTrackingList(TrackingList* pTl) { mcls_pTlCoilOutSt = pTl; }					// 出側物理コイル単位の状態管理用トラッキング



/////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 画面アクセス用
public:
	//// クリティカルセクション
	void SetCsLock()			{ EnterCriticalSection(&m_csLock); }
	void SetCsUnLock()			{ LeaveCriticalSection(&m_csLock); }

	//// 共有メモリ
	TO_COIL_TBL::TO_COIL_REC* GetCoilTbl(int rec)	{return (TO_COIL_TBL::TO_COIL_REC*)&mtbl_pCoil->rec[rec];}	



/////////////////////////////////////////////////////////////////////////////////////////////////////////////
// シーケンス制御
public:
	void AllDelete();													// 全コイル情報きれいさっぱり初期化 [←MainInstance]
	void CoilClear(COIL_INF* pCoil);									// コイル情報の初期化
	void OutCoilClear(COIL_OUT_INF* pCoil);								// 出側物理コイル情報の初期化
	void Init(bool restart);											// 検査開始時の初期化処理 [←MainInstance]
protected:
	void DummyCoilInf(int rec, bool init, bool isWpd=true);				// ダミーの入側論理コイル情報を生成
	void Create_CoilInf(COIL_INF* pCoil);								// 入側論理コイル情報を生成
	void RecvNextCoil_Ken();											// 検査基点のコイル情報を 次コイル情報エリアにセット
	void RecvNextCoil_Dsp();											// 表示基点のコイル情報を 次コイル情報エリアにセット
	void SetInCoilInf_Init(COIL_INF* pCoilIn, COIL_OUT_INF* pCoilOut, double dOutLen);	// 出側コイルの次の入側コイル情報をセット
	void SetInCoilInf_End(COIL_OUT_INF* pCoilOut, double dInLen, double dOutLen);		// 最後の入側コイル情報の確定情報をセット



/////////////////////////////////////////////////////////////////////////////////////////////////////////////
// コイル切り替え
public:
	void ChangeCoilKenBase(bool isWpd=true, bool isNowCoil=false);				// 検査対象コイル切り替え(基準面) [←MainInstance]
	void ChangeCoilKenBase_NotPos();											// 検査対象コイル切り替え(他方面) [←MainInstance]
	bool ChangeCoilDspBase(double dLen, int nDspWpd=0);							// 表示対象コイル切り替え [←MainInstance]

	void ChangeCoilKenPointer(int men);											// 検査対象コイルの参照位置変更 [←HtRecv]
	void ChangeCoilDspPointer(EM_POS_DSP pos, double dLenIn, double dLenOut);	// 表示用 各表示位置 対象コイルの参照位置変更 (今各位置のレコードの奴にする) [←PlgManager]
	void ChangeCoilShaPointer(double dLen_In, double dLen_Out);					// 表示用 シャーカット位置 対象コイルの参照位置変更
	
	void ChangeCoilOutKenPointer(int ou);										// 検査装置位置 対象出側物理コイルの参照位置変更 (今検査装置位置のレコードの奴にする) [←HtRecv]
	void ReloadInCoil();														// 検査装置位置 出側物理コイルの入側コイル情報を更新 [←MI]


	
	
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 実績保存関連
protected:
	void SaveDB_CoilInf();												// コイル情報部 DB保存
	void SaveDB_CoilPosition(int ou, int row);							// コイル位置情報 DB保存
	void SaveDB_CoilResult();											// コイル実績部 DB保存
	void SaveDB_UpdateCoilResult(EM_MODE_COILRESULT_UPDATE emMode,
		COIL_INF const* pCoilInf);
	void SaveDB_CoilCutPos(int nMode, SHRCUT_ITEM const* cut);			// カット位置 DB保存 [←MI]
	void ResetDB_DefectImg(char const* cKizukenNo);						// 疵画像保存数リセット要求


/////////////////////////////////////////////////////////////////////////////////////////////////////////////
// カメラコントローラ露光時間
public:
	int	GetCamExpTime(int nMen, int nCamPos, int nAngId);				// 指定カメラの露光時間を取得
#ifdef IS_TO_CAMERA_TBL
	int	GetCamMode(int nMen, int nCamPos, int nAngId);					// 指定カメラの状態を取得
#endif
	float	GetCamGain(int nMen, int nCamPos, int nColor);				// 指定カメラのカメラゲインを取得

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 照明装置
public:
	int GetLedValue(int nMen, int nColor);								// 照明装置の調光値を取得

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 小物
protected:
	void SetTypeEpcCoil(TYPE_EPC_COIL& data, const PRO_TYPE_DATA_COILINF& p);	// TYPE_EPC_COIL完成
	void SetTypeCoilInf(DBDATA_COIL_INF& data, const PRO_TYPE_DATA_COILINF& p);	// DBDATA_COIL_INF作成



/////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Get
public:
	int				GetDistPos() const					{ return m_nDistPos; }							// 基準面(より上流の方)が裏か表か(0:表,1:裏)
	int				GetRecIndex(int rec, int index);													// レコード位置取得 (0:現 1:1コ前 2: 3: )
	int				GetRecKizukenNo(char const* pKizukenNo);											// レコード位置取得 (管理No)
	int				GetKenRecBase()						{ return m_nKenRecBase;}
	const COIL_INF* GetCoilNextKen() const				{ return &m_CoilNext_Ken;}						// 検査用 次コイル情報
	const COIL_INF* GetCoilIndex(int rec, int index)	{ return &m_CoilInf[GetRecIndex(rec, index)]; }	// 指定された前コイル等のコイル情報 (0:現 1:1コ前 2: 3: ) (物の流れ)
	const COIL_INF* GetCoilRec(int rec) 				{ return &m_CoilInf[rec]; }						// 指定されたレコードのコイル情報
	const COIL_INF* GetCoilKizukenNo(char const* pKizukenNo);											// 指定された管理Noのコイル情報

	int				GetShaRecIndex(int rec, int index);
	const COIL_INF* GetCoilAll(int pos) const;								// 全部入り (実際の流れ)
	const COIL_INF* GetCoilBaseKen() const									// 検査基点位置でのコイル情報 (実際の流れ)
			{ if(-1==m_nKenRecBase){return NULL;} else { return &m_CoilInf[m_nKenRecBase];} }	
	const COIL_INF* GetCoilKenV(int men)const	{ return m_pCoilKenV[men];}	// 検査装置位置でのコイル情報 (データの流れ)
	const COIL_INF* GetCoilKenR(int men)const	{ return m_pCoilKenR[men];}	// 検査装置位置でのコイル情報 (実際の流れ)   ※ これが後で追加されたから、GetCoilBaseKenの必要性がかなり薄れた。将来無くすかも・・・

	const COIL_INF* GetCoilBaseDsp() const		{ return m_pCoilBaseDsp; }	// 表示基点位置でのコイル情報 (実際の流れ)
	const COIL_INF* GetCoilDsp(int pos) const { return m_pCoilDsp[pos]; }	// 表示位置でのコイル情報 (実際の流れ)

#ifndef TRACKING_NON_DSP_COIL
	const PRO_TYPE_DATA_COILDSP* GetCoilNextDsp(int nDspWpd) const	{ return &m_CoilNext_Dsp[nDspWpd];}		// 表示用 次コイル情報
#endif

	const COIL_OUT_INF*	GetCoilOutKen(int ou) const { return m_pCoilOutKen[ou]; }		// 検査装置位置での出側物理コイル情報

	int GetKizuNo(COIL_INF* pCoil);											// 疵Noを取得(呼ばれるたびにインクリンメントされるため注意)

	HANDLE GetNextCoilKen_Recv() { return m_evNextCoilKen_Recv; }		// 検査用 次コイル情報受信完了通知

	const double    GetLastSpm()			{ return m_dLastSpm; }		// 前回コイルのSPM伸び率

																		/////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Set
public:
	void SetBrCenter(int men, int cam, int angle, BYTE br){m_nBrCenter[men][cam][angle] = br;};	// エッジ内平均明るさをセット [← HtRecv]



/////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 初期用検査条件
public:
	void SetInitScode(int ou, int s) { if(s<1 || s>MAX_SCODE) {m_nInitScode[ou]=MAX_SCODE;} else {m_nInitScode[ou]=s;} }	// 初期検査表面状態 セット
	void SetInitPcode(int ou, int p) { if(p<1 || p>MAX_PCODE) {m_nInitPcode[ou]=MAX_PCODE;} else {m_nInitPcode[ou]=p;} }	// 初期検査パターン セット
	int	 GetInitScode(int ou) const	{ return m_nInitScode[ou]; }		// 初期検査表面状態
	int	 GetInitPcode(int ou) const	{ return m_nInitPcode[ou]; }		// 初期検査パターン




/////////////////////////////////////////////////////////////////////////////////////////////////////////////
// メンバー関数
protected:
	int  ThreadFirst();													// スレッド開始前処理 (スレッド実行直後(シグナルセット前)にコールバック)
	int  ThreadLast();													// スレッド終了前処理 (終了シグナル後にコールバック)
	void ThreadEvent(int nEventNo);										// スレッドイベント発生

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
// メンバ変数
protected:
	
	//=========================================
	// 各インスタンス
	StatusManager*			mcls_pStatus;								// ステータス制御クラス	
	ParamManager*			mcls_pParam;								// パラメータ管理クラス
	DBManager*				mcls_pDbMgr;								// DB登録制御クラス		
	EdgePosControl*			mcls_pEdgePosCont;							// エッジ検出整合性管理クラス
	TrackingList*			mcls_pTlCoilOutSt;							// 出側物理コイル単位の状態管理用トラッキング
	MaeManager*				mcls_pMae;									// 前工程情報管理クラス
	DmemManager*			mcls_pDmemMgr;								// データメモリ管理クラス
//#ifdef ENABLE_RPI_SAVE
//	IfRpiSave*				mcls_pIfRpiSave;
//#endif


	//=========================================
	// 受け渡しキュー
	ThreadQueue<DBManager::DeliveryDB>*		mque_pAddSQLData;			// DBクラス 受け渡しデータ用キュー


	//=========================================
	// ハンドル
	HANDLE					m_evNextCoilKen;							// 検査用 コイル情報受信通知
	HANDLE					m_evNextCoilDsp;							// 表示用 コイル情報受信通知


	//=========================================
	// 同期制御
	CRITICAL_SECTION		m_csLock;									// クリティカルセクション


	//=========================================
	// 共有メモリ情報
	HANDLE					m_hCoil;									// コイル情報 ハンドル
	TO_COIL_TBL*			mtbl_pCoil;									// コイル情報 データ
#ifdef IS_TO_CAMERA_TBL
	HANDLE					m_hCam;										// カメラ設定情報 ハンドル
	TO_CAMERA_TBL*			mtbl_pCam;									// カメラ設定情報 データ
#endif

	//=========================================
	// コイル情報バッファ
	int						m_nKenRecBase;								// 基準面の検査基点位置での入側論理コイルバッファの配列位置 (-1:無し) (現在値から-1が一個前のコイル情報となる) (実際のもの単位で切り替える)
	int						m_nDspRecBase;								// 表示基点位置での入側論理コイルバッファの配列位置 (-1:無し) (現在値から-1が一個前のコイル情報となる) (実際のもの単位で切り替える)
	double					m_dLastSpm;									// 最後にコイル切替した検査用コイル情報のSPM伸び率 (初期値は、0.00)
	COIL_INF				m_CoilInf[SIZE_COIL_BUF];					// 検査中/検査完了用バッファ (サイクリック) (基準面の検査装置位置通過で切り替わり)
	COIL_INF				m_CoilNext_Ken;								// 検査用 次コイル情報
#ifndef TRACKING_NON_DSP_COIL
	PRO_TYPE_DATA_COILDSP	m_CoilNext_Dsp[NUM_DSP_WPD];				// 表示用 次コイル情報
#endif


	//=========================================
	// 各位置 通過中のコイル情報ポインタ
	COIL_INF*				m_pCoilKenV[NUM_MEN];						// 各検査装置位置でのコイル情報 (NULLは有りえない) (データの流れで切り替える)
	COIL_INF*				m_pCoilKenR[NUM_MEN];						// 各検査装置位置でのコイル情報 (NULLは有りえない) (実際のもの単位で切り替える)
	COIL_INF*				m_pCoilBaseDsp;								// 表示基点でのコイル情報			(コイル切り替え時に次々と参照ポインタ位置が変化していく) (NULLの可能性有り)
	COIL_INF*				m_pCoilDsp[MAX_POS_INDEX];					// 表示用の各表示でのコイル情報   (コイル切り替え時に次々と参照ポインタ位置が変化していく) (NULLの可能性有り)

	//=========================================
	// 出側情報用
	COIL_OUT_INF			m_CoilOutInf;								// 出側コイル情報
	SHRCUT_ITEM				m_typCut;									// Fカット一時格納用

	COIL_OUT_INF*			m_pCoilOutKen[NUM_MEN];						// 各検査装置位置での出側物理コイル情報 (NULLは有りえない) (データの流れで切り替える)

	//=========================================
	// その他諸々
	int						m_nDistPos;									// 基準面(より上流の方)が裏か表か(0:表,1:裏)
	int						m_nDistNotPos;								// 基準面と逆の方(0:表,1:裏)
	int						m_nInitScode[NUM_MEN];						// 初期検査表面状態 (表/裏)(1オリジン)
	int						m_nInitPcode[NUM_MEN];						// 初期検査パターン (表/裏)(1オリジン)

	//=========================================
	// エッジ内平均明るさ(HtRecvがセット)
	BYTE					m_nBrCenter[NUM_MEN][NUM_CAM_POS][MAX_CAMANGLE];		// エッジ内平均明るさ(使用するのは、R,G,Bのみ)


	
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
// EPC製 全長保存PC用
private:
	BOOL SetUdpAllFrame(const char* ip, WORD port);						// UDP SOCKET生成
	void ReSetUdpAllFrame();											// UDP SOCKET破棄

	SOCKET					m_sockAllFrame;								// 全長保存用 ソケット本体
	sockaddr_in				m_addrAllFrame;								// 全長保存用 sockaddr_in構造体


#ifdef ENABLE_RECORDER_UDP
public:
	void SendUdpAllFrame(TYPE_EPC_COIL* pData);							// コイル情報 を 全長保存PCに送信
	TYPE_EPC_COIL	GetEpcCoilInfo(){ return m_typEpcCoil; }			// 送信用コイル情報を取得 [←MI]
	int				GetEpcScode(int ou){ return m_nScode[ou]; }			// 送信用検査表面状態を取得（SPM圧下中） [←MI]
	int				GetEpcScode_spmoff(int ou) { return m_nScode_spmoff[ou]; }	// 送信用検査表面状態を取得（SPM開放中） [←MI]

	HANDLE			m_evNextCoilKen_Recv;								// 検査用 次コイル情報受信完了イベント[←MI]

private:
	TYPE_EPC_COIL	m_typEpcCoil;										// 送信用コイル情報
	int				m_nScode[NUM_MEN];									// 送信用検査表面状態（SPM圧下中）
	int				m_nScode_spmoff[NUM_MEN];							// 送信用検査表面状態（SPM開放中）
#endif


};
