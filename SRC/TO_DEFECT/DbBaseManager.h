//=====================================================================
// データベース ベース管理クラス
//	【全ライン】
//		・スレッド間受け渡し、スレッド起動など基本処理を担当
//		・BCP制御処理を提供
//=====================================================================

#pragma once

// 疵検クラス インクルード
#include "..\..\Library\KizuLibMFC\CLASS\ThreadManager.h"				// ワーカースレッドマネージャー
#include "..\..\Library\DefectLib\DBSaveManager.h"						// DB書込み基本クラス
#include "..\..\Library\KizuLibMFC\Class\CreateSQL.h"					// SQL文生成補助クラス
#include "..\..\Library\KizuBCP\BCPBase.h"								// DB超高速書込みクラス
#ifndef JPEGLIB_LJT
#include "..\..\Library\DefectLib\KizuJpegManager.h"					// Bitmap⇔Jpeg変換クラス
#else
#include "..\..\Library\DefectLib\KizuLjtJpegManager.h"					// Bitmap⇔Jpeg変換クラス
#endif
#include "..\..\Library\KizuLib\Class\KizuPerformanceCounter.h"			// 時間計測管理クラス

// 半固定
#include "..\..\Include\LineCommon.h"									// 疵検インクルード

// ローカル
#include "StatusManager.h"											// ステータス制御クラス

using namespace KizuLib;

class DbBaseManager :	public ThreadManager
{
//// 公開定数
public:

	//=========================================
	// 受け渡し構造体
	typedef struct {
		int					kubun;										// どの構造体か (ENUM_KUBUN)
		BYTE*				data;										// 構造体のポインタ
	} DeliveryDB;
	static const int		SIZE_SQLINF_NUM				= 10;			// 欠陥紐付け用バッファ件数


//// メンバー定数
protected :
	//=========================================
	// ローカル定数
	static const int		THREAD_DBSAVE				= 4;			// SQL文を実際に書き込む人の数
#ifndef ADDDBCOUNT_QUE_SIZE_CHANGE
	static const int		MAX_SQLCOUNT				= 10000;		// SQL文生成クラス(自クラス)の受け渡しキューサイズ
	static const int		MAX_ADDDBCOUNT				= 5000;			// DB書込みクラスへの受け渡しキューサイズ
#else
	static const int		MAX_SQLCOUNT				= 200000;		// SQL文生成クラス(自クラス)の受け渡しキューサイズ
	static const int		MAX_ADDDBCOUNT				= 200000;		// DB書込みクラスへの受け渡しキューサイズ
#endif


	// 疵情報保存制御フロー用
	static const int		RATE_LOWGR_IMGSKIP_SQL		= 60;			// 無害Gr画像歯抜け保存[%]
	static const int		RATE_LOWGR_IMGSKIP_DB		= 60;
	static const int		RATE_DEST_LOWGR_SQL			= 45;			// 無害Grを破棄[%]
	static const int		RATE_DEST_LOWGR_DB			= 45;
	static const int		RATE_ONLY_MAXDEF_SQL		= 35;			// 1フレーム中の最大疵のみ保存[%]
	static const int		RATE_ONLY_MAXDEF_DB			= 35;
	static const int		RATE_SKIP_END_SQL			= 30;			// 疵保存スキップ終了[%]
	static const int		RATE_SKIP_END_DB			= 30;
	static const int		RATE_ONLY_MAXHIGH_SQL		= 25;			// 1フレーム中の最大疵 かつ 有害Grのみ保存[%]
	static const int		RATE_ONLY_MAXHIGH_DB		= 25;
	static const int		RATE_SKIP_START_SQL			= 15;			// 疵保存スキップ開始[%]
	static const int		RATE_SKIP_START_DB			= 15;



			


	static const int		TIME_BCP_AUTO_COMMIT		= 5*60*1000;	// 最後に疵情報を書き込んで、この時間以上経過した場合は、自動的にコミットさせる
	static const int		TIME_BCP_STATUS_CHECK		= 2*60*1000;	// 状態チェック用定周期タイマー
	static const int		TIME_SAVE_SKIP_WAIT			= 3*1000;		// 1回実績書込み遅れが発生した場合、この時間中異常にさせる


	//=========================================
	// 欠陥個数紐付け用
	struct TYP_SQL_INF
	{
		bool				bResult;									// コイル実績登録済み時 true

		char				cKizukenNo[SIZE_KIZUKEN_NO];				// 管理No
		int					nTotalCnt[NUM_MEN];							// 全件数
		int					nDeleCnt[NUM_MEN];							// 有害Grの破棄件数
		int					nLowGrDeleCnt[NUM_MEN];						// 無害Grの破棄件数
		int					nSaveCnt[NUM_MEN];							// 保存個数
		int					nSaveImgAllCnt[NUM_MEN];					// 保存画像個数
		int					nSaveImgCnt[NUM_MEN];						// 保存画像個数(上限チェック用) 1CAPL中央では疵情報と同じ件数保存のため値は入れるが不使用
		bool				bMaxSave[NUM_MEN];							// 最大保存件数到達時 true

		int					nImgDeleCnt[NUM_MEN];						// 有害Grの疵画像破棄件数
		int					nImgLowGrDeleCnt[NUM_MEN];					// 無害Grの疵画像破棄件数
	};


//// 公開関数
public:
	DbBaseManager(const char *cThreadName);
	virtual ~DbBaseManager(void);

	void Alloc();														// 関数初期化
	void Free();														// 関数開放

	void Send_To_DbManage(bool bWrite);									// DB削除管理に通知


	//=========================================
	// プロパティ
	void SetStatusMgr(StatusManager* pCls) { mcls_pStatus = pCls;};		// ステータス制御クラスセット


	//=========================================
	// パラメータ
	void	SetJpgQuality(int nQuality) { m_nJpgQuality = nQuality; }


	//=========================================
	// 画面用アクセス
	void ReSetMaxCount();
	int GetQueData_Count()	const { return gque_AddSQLData.GetCount();}		// 登録キュー
	int GetQueData_Max()	const { return gque_AddSQLData.GetMaxCount();}	// 登録最大キュー
	int	GetQueDB_Count()	const { return mque_AddDB.GetCount(); }			// 登録キュー
	int	GetQueDB_Max()		const { return mque_AddDB.GetMaxCount(); }		// 登録最大キュー

	double	GetAveTime()	const { return mcls_Time.GetAveTime(); }		// アプリ側の平均処理時間 [ms]
	double	GetMaxTime()	const { return mcls_Time.GetMaxTime(); }		// アプリ側の最大処理時間 [ms]
	
	double	GetAveSqlTime()	const {											// LIB側の平均処理時間 [ms]
		double dWk = 0.0;
		for(int ii=0; ii<THREAD_DBSAVE; ii++) dWk += mcls_pDbs[ii]->GetAveTime();
		return dWk/THREAD_DBSAVE; }	
	double	GetMaxSqlTime()	const {											// LIB側の最大処理時間 [ms]
		double dWk = 0.0;
		for(int ii=0; ii<THREAD_DBSAVE; ii++) dWk = max(dWk, mcls_pDbs[ii]->GetMaxTime());
		return dWk; }		
	

	const char* GetInf_KizukenNo(int rec) const { return m_SqlInf[rec].cKizukenNo; }
	int	GetInf_TotalCnt(int rec, int men) const { return m_SqlInf[rec].nTotalCnt[men]; }
	int	GetInf_DeleCnt(int rec, int men)  const { return m_SqlInf[rec].nDeleCnt[men]; }
	int	GetInf_LowGrDeleCnt(int rec, int men)  const { return m_SqlInf[rec].nLowGrDeleCnt[men]; }
	int GetInf_ImgDeleCnt(int rec, int men) const{ return m_SqlInf[rec].nImgDeleCnt[men]; }
	int GetInf_ImgLowGrDeleCnt(int rec, int men) const { return m_SqlInf[rec].nImgLowGrDeleCnt[men]; }
	int GetInf_SaveCnt(int rec, int men)  const { return m_SqlInf[rec].nSaveCnt[men]; }
	int GetInf_SaveImgAllCnt(int rec, int men)  const { return m_SqlInf[rec].nSaveImgAllCnt[men]; }
	int GetInf_SaveImgCnt(int rec, int men)  const { return m_SqlInf[rec].nSaveImgCnt[men]; }


//// 公開変数
public : 
	//// スレッドキュー
	ThreadQueue<DeliveryDB> gque_AddSQLData;							// 受け渡しデータ用キュー



//// メンバー関数
protected:
	int ThreadFirst();													// スレッド開始前処理 (スレッド実行直後(シグナルセット前)にコールバック)
	int ThreadLast();													// スレッド終了前処理 (終了シグナル後にコールバック)
    void ThreadEvent(int nEventNo);										// スレッドイベント発生
	

	//=========================================
	// 仮想関数
	virtual void ThreadFirst_AddHandle() {};							// シグナル待ち行列の追加
	virtual void OnAddSignal(int nEventNo) {};							// シグナルを追加した奴のイベント (追加したものを0始まりとする)
	virtual void OnAlloc(bool bMode=true) {};							// 関数初期化
	virtual void OnFree() {};											// 関数開放
	virtual void OnQueDb(int emNo, BYTE* pData) {};						// DB登録依頼
	virtual void OnBcpAutoCommit(bool bAuto) {};						// BCP自動コミット用
	virtual void OnBcpStatusCheck() {};									// BCP状態チェック用(定周期タイマーイベント)
	virtual void OnReSetMaxCount() {};									// リセット処理追加

	//=========================================
	// 欠陥個数紐付け用
public:
	void ChangeSQLInf(const char* cKizukenNo);							// バッファ切り替え
	int  GetRecSQLInf(const char* cKizukenNo);							// バッファレコード特定

protected:
	//=========================================
	// BCP関連
	bool BCP_Connect(BCPBase* cls_bcp, char const* pTableName);			// BCPクラス コネクト
	void BCP_Dispose(BCPBase* cls_bcp);									// BCPクラス 開放
	bool BCP_SendRow(BCPBase* cls_bcp);									// BCPクラス データ転送
	bool BCP_Batch(BCPBase* cls_bcp);									// BCPクラス データ書込み
	bool BCP_ColLen(BCPBase* cls_bcp, int coulmn, long nSize);			// BCPクラス バインドデータサイズ指定 (varbinary登録少ない現象の、暫定対策用)

	void BCP_AutoCommitTimerOn();										// BCP自動コミットタイマー開始

//// メンバー変数
protected :

	//=========================================
	// 各インスタンス
	DBSaveManager*			mcls_pDbs[THREAD_DBSAVE];					// DB書込みクラス	
	KizuPerformanceCounter	mcls_Time;									// 時間測定スレッド
	StatusManager*			mcls_pStatus;								// ステータス制御クラス


	// スレッドキュー
	ThreadQueue<DBSaveManager::DeliverySQL> mque_AddDB;					// 受け渡しデータ用キュー (ここに入ったキューは全て登録する)


	//=========================================
	// シグナル
	HANDLE					m_evTimeBcpAutoCommit;						// 自動コミット待ちタイマー
	HANDLE					m_evTimeBcpStatusCheck;						// ステータスチェックタイマー

	//=========================================
	// その他もろもろ
	TYP_SQL_INF				m_SqlInf[SIZE_SQLINF_NUM];					// 直近10件分の情報 (0:最新 1:一個前 ･･･)
	bool					m_bQueState;								// キュー空き状態
	bool					m_bBcpState;								// BCP接続状態

	//=========================================
	// パラメータ
	int						m_nJpgQuality;								// Jpeg品質
	bool					m_bDeleteDbMode;							// 削除モード (true:書き込み優先)


	//=========================================
	// 実績保存スキップ 用
public:
	void	SetEvSaveSkip(int nMen)			  { SetEvent(m_evNowSaveSkip[nMen]); }
	void	SetSaveSkipId(int nMen, int nVal) { m_nSaveSkipId[nMen] = nVal; }
	bool	IsSaveSkipState(int nMen) const   { return m_bSaveSkipState[nMen]; }

	void	SetSaveSkipAdd(char const* cKizukenNo, int nMen, int nLowGrFlg);

	int		SaveDefectCheck(char const* cKizukenNo, int nMen, int nLowGrFlg, int nFrameDefectMax);	// 疵実績保存 フロー制御
	int		SaveDefectCheck(int nRec, int nMen,	int nLowGrFlg, int nFrameDefectMax);					// 疵実績保存 フロー制御
	int		SaveMaxCheck(char const* cKizukenNo, int nMen, int nLowGrFlg);											// 疵実績保存 上限確認
	int		SaveMaxCheck(int nRec, int nMen, int nLowGrFlg);														// 疵実績保存 上限確認
	void	SaveMaxReset(int nRec);																					// 疵実績保存 上限リセット

	int		ResetInf_SaveImgCnt(char const* cKizukenNo, int nTorB);													// 疵画像保存数リセット


private:
	// ハンドル
	HANDLE	m_evNowSaveSkip[NUM_MEN];										// 実績保存スキップ発生
	HANDLE	m_evTimeSaveSkip[NUM_MEN];										// 実績保存スキップタイマー

	// 保持情報
	int		m_nSaveSkipId[NUM_MEN];											// 実績保存スキップ機器状態ID
	bool	m_bSaveSkipState[NUM_MEN];										// 実績保存スキップ状態

	void	SaveSkipStart(int nMen);										// 実績保存スキップ 開始
	void	SaveSkipEnd(int nMen);											// 実績保存スキップ 完了
};
