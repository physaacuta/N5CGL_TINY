//=====================================================================
// エッジ検出整合性管理クラス
//	【全ライン】
//		・基本的に完全流用可能なように作る
//=====================================================================

#pragma once

// 疵検クラス インクルード
#include "..\..\Library\KizuLibMFC\CLASS\FuncBase.h"					// モジュールの基底クラス

// 半固定
#include "..\..\Include\LineCommon.h"									// 疵検インクルード

// 標準
#include <math.h>

// ローカルのクラス
#include "MainConst.h"
#include "ParamManager.h"												// パラメータ管理クラス
#include "DBManager.h"													// DB登録制御クラス


// EPCインクルード
//#include "..\..\Include\EPC\NCsmemData.h"
#include "..\..\Include\EPC\NCL2DETECT_if.h"							// I/F定義


//// 切り替え ----------------------------------------->>>
#define		EDGE_POS_CONTROL_DBSAVE										// 実績をDBに保存する場合コメントアウト。
																		// エッジ検出整合性機能のみの場合は、コメントにすること。
// <<<----------------------------------------------------

using namespace KizuLib;

// ファンクションの使い方
class EdgePosControl : public FuncBase //ThreadManager
{
//// 公開定数
public:

	//=======================================
	// エッジ検出状態
	enum EM_DIV_EDGE_MODE {
		DIV_EDGE_MODE_NON		= -99,							// [エッジ検出状態] 初期状態
		DIV_EDGE_MODE_BACK		= -9,							// [エッジ検出状態] バックアップ中 (停止中)
		DIV_EDGE_MODE_SKIP		= -1,							// [エッジ検出状態] スキップ
		DIV_EDGE_MODE_OK		= 0,							// [エッジ検出状態] 正常
		DIV_EDGE_MODE_ONE,										// [エッジ検出状態] 片エッジ優先
		DIV_EDGE_MODE_CENTER									// [エッジ検出状態] ラインセンター振り分け
	};


//// メンバー定数
private :

//// 公開関数
public:
	EdgePosControl(void);
	virtual ~EdgePosControl(void);


	//=========================================
	// プロパティ
	void SetParamMgr(ParamManager* pCls) { mcls_pParam = pCls; }	// パラメータ管理
	void SetQueAddSQL(ThreadQueue<DBManager::DeliveryDB>* pQue) { mque_pAddSQLData=pQue; }	// DB登録用のキュー

	void SetOldMode(bool bMode) { m_bOldMode = bMode; }					// 動作モード定義
	void SetFrameSkipAddDB(bool bAdd) {m_bFrameSkipAddDB = bAdd; }		// DBへの登録

	// 関数ポインタ


	//=========================================
	// 外部操作関数
	void	Init();														// 初期化 [←MI]
	void	CoilChange(int ou);											// 物理的なコイル切替 [←MI]
	void	CoilStart(int ou, char const* cKizukenNo, float coilWid, bool bIsInit=false);	// 論理的なコイル切り替え＆上位板幅セット [←HtRecv]
	void	CoilEnd(int ou);											// コイル実績締め [←HtRecv]

	void	SearchEdgeIndex(int ou, float* dEdgePos, int& edgeL, int& edgeR);			// エッジ検出位置決定 (スキャン方向はDS→WS)


	// エッジ検出関連
	static	CString GetDivEdgeMode(EM_DIV_EDGE_MODE em);				// エッジ検出モード 名称
	bool	SetEdgePos(int ou, long nFrameNo, long nPos, double dResY, float* edge, float* edgemax, float* edgemin, double dSpmNobiHosei);	// エッジ検出位置セット [←HtRecv]
	void	SearchEdgePos(int ou, float& edgeL, float& edgeR, DWORD* pFrameNo=NULL);	// エッジ検出位置決定 (スキャン方向はDS→WS) [←PlgMgr, Dlg]
	void	SearchEdgeRawPos(int ou, float& edgeL, float& edgeR, DWORD* pFrameNo=NULL);	// 生エッジ検出位置決定 (スキャン方向はDS→WS) [←CoilMgr]
	void	SearchEdgeRawPosMax(int ou, float& edgeL, float& edgeR, DWORD* pFrameNo = NULL);// 生エッジ検出位置決定 (スキャン方向はDS→WS) [←CoilMgr]
	void	SearchEdgeRawPosMin(int ou, float& edgeL, float& edgeR, DWORD* pFrameNo = NULL);// 生エッジ検出位置決定 (スキャン方向はDS→WS) [←CoilMgr]
	void	SearchEdgeIndex(int ou, int& edgeL, int& edgeR);			// エッジ検出位置決定 (スキャン方向はDS→WS)
	void	SetFrameSkip(int ou, int* nFrameSkip, int* nEdgeNg, int* nFrameSt);			// 検査状況セット


	// 画面関係
	EM_DIV_EDGE_MODE	GetEdgeMode(int ou) const { return m_EdgeInf[ou].emEdgeMode; }
	//float				GetEdgePos(int ou, int cam) const { return m_EdgeInf[ou].dEdgePos[cam]; }
	float				GetEdgePos(int ou, int nLR) const { return m_EdgeInf[ou].dEdgePos[nLR]; }
	float				GetEdgeAnsPos(int ou, int nLR) const { return m_EdgeInf[ou].dEdgeAnsPos[nLR]; }
	bool				GetEdgeAns(int ou) const { return m_EdgeInf[ou].bEdgeAns; }

//// メンバー関数
private:

	// 共通処理
	void			GetSearchPos(int nLR, float dCoilWidth, float dDetectRange, float* dPos);	// 正常のサーチ範囲


	// 関数ポインタ (メールスロット送信用)
	//(*Send_HtDefect_GetFrameImg)(1, 1, 1); で実行


//// メンバー変数
private:

	// インスタンス
	ParamManager*			mcls_pParam;								// パラメータ管理

	// 受け渡しキュー
	ThreadQueue<DBManager::DeliveryDB>*		mque_pAddSQLData;			// DBクラス 受け渡しデータ用キュー

	// エッジ状態格納
	struct EDGE_INF {
		DWORD				nFrameNo;									// フレームNo
		//float				dEdgePos[NUM_CAM_POS];						// 各判定機能の検出エッジ位置(生値) (0:エッジ検出失敗)
		float				dEdgePos[2];								// 検出エッジ位置(生値) (0:エッジ検出失敗)
		
		EM_DIV_EDGE_MODE	emEdgeMode;									// エッジ検出状態
		bool				bEdgeEnable[2];								// エッジ検出有無 (true:エッジ検出時) (左/右)
		float				dEdgeRawPos[2];								// 検出エッジ位置(生値)  (左/右)
		float				dEdgeRawPosMax[2];							// 検出最大エッジ位置(生値)  (左/右)
		float				dEdgeRawPosMin[2];							// 検出最小エッジ位置(生値)  (左/右)
		float				dEdgeAnsPos[2];								// 決定した最終エッジ位置(補正後)(左/右)
	
		bool				bEdgeAns;									// 最終エッジ位置判定結果 (true:正常)
	} m_EdgeInf[NUM_MEN];

	// 実績データ格納
	struct ZI_COIL_RESULT {
		char				cKizukenNo[SIZE_KIZUKEN_NO];				// 疵検管理No
		bool				bCoilWidth;									// 上位板幅有効
		float				dCoilWidth;									// 上位板幅 [mm] (データの流れでの板幅)

		// 随時更新
		float				dCoilWidMin;								// 正常検出時の最小板幅
		float				dCoilWidMax;								// 正常検出時の最大板幅
		int					nLimitEdgeErrNum;							// 板幅許容誤差範囲外数
		int					nLostEdgeNum[2];							// エッジ未検出数 (左エッジ/右エッジ)

		int					nFrameNumCoil;								// コイル単位のフレーム数
		int					nFrameNumSkip[NUM_CAM_POS][5];				// 画像処理スキップした数
	} m_Zi[NUM_MEN];

	// モード
	bool					m_bOldMode;									// 昔の状態で動作させる場合 true
	bool					m_bFrameSkipAddDB;							// フレームNo、スキップ個数をDBに登録させる場合 true

	// ワーク領域
	int						m_nEdgeSearchSkip[NUM_MEN];					// サーチ範囲条件設定スキップ数 (0:実行 1～:その数だけスキップ -1：ずっとスキップ)
	int						m_nFrameSaveInterval[NUM_MEN];				// フレーム画像保存までの待ちフレーム数
};
