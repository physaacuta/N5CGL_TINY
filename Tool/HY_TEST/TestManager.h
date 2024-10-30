

#pragma once

// 疵検ライブラリ インクルード
#include "..\..\Library\DefectLib\KizuOpSend.h"	
#include "..\..\Library\DefectLib\DBSaveManager.h"						// DB書込み基本クラス
#include "..\..\Library\KizuLibMFC\Class\CreateSQL.h"					// SQL文生成補助クラス

#include "ParamManager.h"	
#include "ImgSendManager.h"
#include "TestMaeManager.h"

class TestManager : public ThreadManager
{
#define DIV_LEN_POS_SHR_I		DIV_DISP_POS_END
#define DIV_LEN_POS_FINAL		(DIV_LEN_POS_SHR_I+1)

////　公開関数
public:
	//=============================
	// 本クラス受け渡し用クラス
	//=============================
	//// メール情報受け渡しキュー
	struct DELIVERY_KEY {
		int		id;
		int		speed;								// mpm
		char	cKizukenNo[SIZE_KIZUKEN_NO];		// 主キー
		int		nUnten;								// 運転状態
		int		nKadou;								// 稼動状態
		int		nKiki[2];							// 機器状態(表/裏)
		int		nKensa[2];							// 検査状態(表/裏)

		int		nMen;								// 面
		int		nY;									// 長手距離(mm)
		int		dDS;								// DSからの距離(mm)
		int		Tid;								// 疵種EdasysID
		int		Gid;								// グレードEdasysID
		float	dArea;								// 面積

		int		nCancelNo;							// キャンセル対象の疵連番
		int		nSystem;							// 検出システム (0:中央、1:出側、2:穴検)

		int     nLanp[2][2][8];						// 警報情報
		int		nCnt;								// 疵個数
		bool    bNotChkDsWs;						// コイル幅の確認無フラグ
	};
	ThreadQueue<DELIVERY_KEY>	gque_Deli;

	TestManager(void);
	virtual ~TestManager(void);


	// コイル情報構造体
	struct TEST_COIL_INF {
		char	cKizukenNo[SIZE_KIZUKEN_NO];
		char	cCoilNo[SIZE_COIL_NO+1];
		char	cMeiNo[SIZE_MEI_NO+1];
		long	nCoilLen;					// コイル長[mm] 
		int		nCoilWidth;					// コイル幅[mm] とりあえず[mm]単位とするが、[0.1mm]単位が必要か？
		int		nLen;						// 通板距離(現在の検査基点WPD位置におけるコイル先端からの距離)[mm] 
		long	scode[NUM_MEN];				// 検査表面状態  (1オリジン)
		long	pcode[NUM_MEN];				// 検査パターン  (1オリジン)
		long	nMapColNum;					// マップ列数 
		double	dNobiSPM;					// SPM伸び率[10^2%]
		long	nSpmUmu;					// SPM圧下有無
		int		nKizuSeq;					// 現在検査対象の疵連番
		//int		nCycSeq;					// 現在検査対象の疵連番(周期)
		int		nCutNum;					// シャーカット回数
	};

	// 検査台情報
	struct DSP_INF {
		char	Name[128];						// 検査台名称
		int		nDistance;						// 検査基点からの距離
		char	cKizukenNo[SIZE_KIZUKEN_NO];	// 検査台位置通過中コイルの管理No
		int		nRec;							// 検査台位置コイルのレコード番号(1オリジン)
	};

	//=========================================
	enum DELIVERY_ID {
		EM_TIMER = 0,
		EM_COIL_ADD,							// コイル情報追加
		EM_PLG_STOP,							// PLG停止
		EM_PLG_START,							// PLG起動
		EM_PLG_SLOW,							// PLG１周期
		EM_SPEED_SETTING,						// 速度設定
		EM_INIT_NOTIFY,							// 初期化通知
		EM_PARAM_NOTIFY,						// パラメータ変更通知
		EM_STATUS_NOTIFY,						// 状態通知
		EM_COIL_CUT,							// カット情報送信
		EM_DEFECT,								// 疵情報送信
		EM_DEFLST_CANCEL,						// 重欠陥疵情報削除要求
		EM_ALARM,								// 警報情報送信
		EM_DEFECT_NUM							// 疵個数情報 (最小区間内)
	};

	//=========================================
	// セット
	void SetParamMgr(ParamManager* pCls)		{ mcls_pParam = pCls; };	// パラメータ管理
	void SetOpSend(KizuOpSend *pCls)			{ mcls_pOpSend = pCls; };	// ソケット管理
	void SetImgSend(ImgSendManager *pCls)		{ mcls_pImgSend = pCls; };	// 疵画像送信管理
	void SetMaeMgr(TestMaeManager *pCls)		{ mcls_pMaeMgr = pCls; };	// 前工程管理
	void SetCoilLenMax(int len)					{ m_nCoilLenMax = len; };	// コイル長最大値(mm)
	void SetCoilLenMin(int len)					{ m_nCoilLenMin = len; };	// コイル長最小値(mm)
	void SetCoilWidthMax(int len)				{ m_nCoilWidthMax = len; };	// コイル幅最大値(mm)
	void SetCoilWidthMin(int len)				{ m_nCoilWidthMin = len; };	// コイル幅最小値(mm)
	void SetMaeInfoStatus(bool flg)				{ m_bMaeInfo = flg; };		// 前工程情報セット
	void SetYousetuCut(bool flg)				{ m_bYousetuCut = flg; };	// 溶接点カット実施セット
	void SetDefectCreate(bool flg)				{ m_bDefCreate = flg; };	// 疵発生有り・無し
	void SetDispPlg(bool flg)					{ m_bDispPlg = flg;	};		// 表示PLG
	void SetDefRate(int rate)					{ m_nDefRate = rate; };		// 疵発生数
	void SetSendPos(bool flg)					{ m_bSendPos = flg; };		// 通板位置情報送信有り・無し
	void SetSCode(int scode)					{ m_scode = scode; };		// SCODE設定
	void SetPCode(int pcode)					{ m_pcode = pcode; };		// PCODE設定


	//=========================================
	// ゲット
	PRO_TYPE_DATA_COILINF const* GetCoilInf() { return m_pCoilInf; };		// コイル情報取得
	TEST_COIL_INF const* GetTestCoilInf() { return m_pTestCoilInf; };		// テスト用コイル情報取得
	DSP_INF const* GetDspInf() { return m_pDspInf; };						// 検査台位置情報
	int GetSpeed() { return m_nSpeed; };									// 現在速度情報
	int GetCoilLenMin() { return m_nCoilLenMin; };							// 現在コイル長 最小値(mm)
	int GetCoilLenMax() { return m_nCoilLenMax; };							// 現在コイル長 最大値(mm)
	int GetCoilWidthMin() { return m_nCoilWidthMin; };						// 現在コイル幅 最小値(mm)
	int GetCoilWidthMax() { return m_nCoilWidthMax; };						// 現在コイル幅 最大値(mm)
	bool GetMaeInfoStatus() { return m_bMaeInfo; };							// 前工程情報 true:作成する false:作成しない
	int GetDefRate(){ return m_nDefRate; };									// 疵発生率

	//=========================================


//// メンバ関数
private:


	int ThreadFirst();													// スレッド開始前処理 (スレッド実行直後(シグナルセット前)にコールバック)
	int ThreadLast();													// スレッド終了前処理 (終了シグナル後にコールバック)
    void ThreadEvent(int nEventNo);				

	void AddNewCoil();													// コイル情報新規生成
	void SendStatusInfo();												// 0.ステータス情報セット
	void SendCoilInfo(int nRec,int nDiv);								// 3.コイル情報 (検査用) / 	5.コイル情報 (表示用) セット
	void SendCoilResult(int nRec);										// 4.コイル実績 (1コイル検査完了時)	セット
	void SendPosInfo(int nRec);											// 5.通板位置情報(検査装置位置、表示基点位置、各表示コイル通板位置情報)
	void SendCutInfo(char const* cKizukenNo , int nPos, int nMode=0);	// 6.カット情報等
	void SendAlarm(DELIVERY_KEY* pDeli);								// 7:警報状態
	void SendDefCount(DELIVERY_KEY* pDeli);								// 9:疵個数情報 (最小区間内)
	void SendDefLstCancel(char const* cKizukenNo, int nMen, int nCancelNo, int nSystem);	// 9:重欠陥疵情報削除要求

	void MakeDefect(int nRec , int nMen, int from , int to );			// 疵生成要求処理
	void MakeDefectManual(char const* cKizukenNo, int nMen, int nY , int nDSdiv , int Tid, int Gid, float nArea, bool bNotChkDsWs);
																		// 指定された疵情報を送信
	void UpdateDispInf(int nRec);										// 検査台位置情報更新
	void TimerEvent( void );
	void Exec( int id ,DELIVERY_KEY* pDeli);
	void Send_Hy_Init( void );											// HY_DEFECTに初期化通知を送信する
	void Send_Hy_Param( void );											// HY_DEFECTにパラメータ変更通知を送信する
	void SetKensadaiName( void );										// 検査台名称設定

	void SendMaeInf(int nRec);											// 前工程情報管理にデータ登録要求

private:
//// メンバ変数

	//=========================================
	//// ハンドル
	HANDLE					m_evTimerExec;								// 定周期実行イベント（検査基点PLGイベント）
	HANDLE					m_evTimerDisp;								// 表示基点PLGイベント

	//=========================================
	//// 各インスタンス
	ParamManager*					mcls_pParam;	
	KizuOpSend*						mcls_pOpSend;
	ImgSendManager*					mcls_pImgSend;
	TestMaeManager*					mcls_pMaeMgr;

	DBSaveManager*					mcls_pDbs;							// DB書込みクラス	
	ThreadQueue<DBSaveManager::DeliverySQL> mque_AddDB;					// 受け渡しデータ用キュー (ここに入ったキューは全て登録する)

	//// 変数
	PRO_TYPE_DATA_COILINF*			m_pCoilInf;
	TEST_COIL_INF*					m_pTestCoilInf;
	DSP_INF*						m_pDspInf;
	COIL_BASEDATA*					m_pCoilBase;


	int m_nRecNo;			// 現在検査位置通板中のコイルレコード番号 0:情報無し 1～ レコード番号

	int m_nSpeed;			// 現在速度 (mpm)

	int m_nUnten;			// 運転状態
	int m_nKadou;			// 稼動状態
	int m_nKiki[NUM_MEN];	// 機器状態
	int m_nKensa[NUM_MEN];	// 検査状態
	int m_nCoilLenMin;		// コイル長 最小[mm]
	int m_nCoilLenMax;		// コイル長 最大[mm]
	int m_nCoilWidthMin;	// コイル幅最小[mm]
	int m_nCoilWidthMax;	// コイル幅最大[mm]

	bool m_bMaeInfo;	// 前工程情報を作成する・しない
	bool m_bYousetuCut;	// 溶接点カットを実施する・しない
	bool m_bDefCreate;	// 疵発生をする・しない

	bool m_bDispPlg;		// 表示PLG true:発生 false:停止
	bool m_bSendPos;		// 通板位置情報を送信する・しない

	int m_scode;				// 表面検査パターン
	int m_pcode;				// 検査パターン
	int m_nDefRate;				// 疵発生率 最小区間内にいくつ疵を発生させるか(MAX 10)
	bool m_bInitial;			// 初期化済み

};
