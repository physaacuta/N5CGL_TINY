#pragma once

// 標準
#include <direct.h>

// 疵検ファンクション
#include "..\..\Library\KizuLib\CLASS\KizuBitmapManager.h"	// Bitmap画像生成クラス

// 疵検クラス インクルード
#include "..\..\Library\KizuLibMFC\CLASS\ThreadManager.h"	// ワーカースレッドマネージャー

// 疵検共通ヘッダー
#include "..\..\include\LineCommon.h"						// 疵検ライン共通ヘッダー
#include "..\..\include\HyTbl.h"							// 表示PC共有メモリヘッダー

// 個別ヘッダー
#include "ParamManager.h"									// パラメータ管理クラス
#include "DataSetManager.h"									// データセットクラス

#define LEN_KIZUKEN_NO_HOT      15                          // 熱延仕上げ、管理No長さ

class MaeManager :	public ThreadManager
{
//// 公開変数
public:
	//// 構造体
	struct TYP_MAE_REQUEST									// 前工程情報読込要求キュー
	{
		int			nMode;									// 種別 (0:前工程疵情報読込)
		char		cKanriNo[SIZE_KIZUKEN_NO];				// 処理対象の管理No
		void*		pData;									// 付加情報 (無ければNULL)
	};

//// 公開関数
public:
	MaeManager(void);										// コンストラクタ
	virtual ~MaeManager(void);								// デストラクタ

	//// プロパティ
															// パラメータ管理クラス
	void SetParamMgr(ParamManager* pCls)	{mcls_pParam   = pCls;}
															// データセットクラス
	void SetDataSetMgr(DataSetManager* pCls){mcls_pDataSet = pCls;}
															// 前工程情報読込要求キュー
	void SetQue_MaeRequest(ThreadQueue<TYP_MAE_REQUEST>* pQue){mque_pMaeRequest = pQue;}

//// メンバー変数
private:
	//==================================
	// 構造体
	// 前工程ＦＦ情報
	struct TYP_FF_INF
	{
		// 前工程ＦＦ疵情報
		char		cKizukenNo[SIZE_KIZUKEN_NO];	// 管理№
		int			nKizuNo;						// 疵№
		int			nKouteiID;						// 工程種別
		int			nMenID;							// 表裏区分[0:表、1:裏]
		int			nPreEdit;						// 事前編集フラグ
		char		cMaeKizukenNo[SIZE_KIZUKEN_NO];	// 前工程管理№
		int			nMaeKizuNo;						// 前工程疵№
		char		cFF_Type[4];					// ＦＦ疵種
		char		cFF_Grade[4];					// ＦＦ疵グレード
		char		cFF_Koutei[4];					// ＦＦ疵検出工程
		int			nFF_Kubun;						// ＦＦ疵区分
		int			nFF_LPos;						// ＦＦ疵長さ方向位置[mm]
		int			nFF_WPos;						// ＦＦ疵巾方向位置[mm]
		int			nFF_Length;						// ＦＦ疵長さ[mm]
		int			nType;							// 判定疵種
		int			nGrade;							// 判定疵グレード
		int			nLastPos;						// 最終長手位置[mm]
		// 前工程ライン別コイル情報
		char		cLwk01[SIZE_KIZUKEN_NO];		// 管理No
		int			nLwk02;							// 工程種別
		int			nLwk03;							// 表裏区分
		int			nStatus;						// 処理ステータス
		// 前工程マスター
		char		cMwk01[4];						// 前工程コード
		int			nCheck;							// 有効判定
		int			nMaeDsp;						// 前工程表示モード
		int			nImgFmt;						// 画像形式
	};

	// 前工程疵情報
	struct TYP_MAE_DEFECT
	{
		char		cKizukenNo[SIZE_KIZUKEN_NO];	// 管理№
		int			nKizuNo;						// 疵№
		int			nKouteiID;						// 工程種別
		int			nMenID;							// 表裏区分[0:表、1:裏]
		int			nCycle;							// 周期フラグ
		int			nLPos;							// 長手位置[mm]
		int			nDSWS;							// DSWS区分[0:DS、1:WS]
		float		fDSPos;							// DS位置[mm]
		float		fWSPos;							// WS位置[mm]
		int			nLSpeed;						// ライン速度[mpm]
		int			nPitch;							// ピッチ[mm]
		float		fKei;							// 径[mm]
		int			nKizuW;							// 疵幅[mm]
		int			nKizuL;							// 疵長さ[mm]
		float		fVDenc;							// 縦分解能[mm/pixel]
		float		fHDenc;							// 横分解能[mm/pixel]
		int			nEXmin;							// 外接Xmin[pixel]
		int			nEXmax;							// 外接Xmax[pixel]
		int			nEYmin;							// 外接Ymin[pixel]
		int			nEYmax;							// 外接Ymax[pixel]
		int			nCPosX;							// 疵中心位置X[pixel]
		int			nCPosY;							// 疵中心位置Y[pixel]
		int			nVPixel;						// 縦画素数[pixel]
		int			nHPixel;						// 横画素数[pixel]
		int			nPSize1;						// 画像サイズ1[byte]
		int			nPSize2;						// 画像サイズ2[byte]
		int			nPSize3;						// 画像サイズ3[byte]
		int			nPSize4;						// 画像サイズ4[byte]
	};

	//==================================
	// 共有メモリ
	int	m_nRec;							// 格納レコード
	TBL_HY_MAPROW*	m_pMaeMap;			// 前工程マップ情報テーブル
	TBL_HY_MAE_INF*	m_pMaeInf;			// 前工程疵情報テーブル

	//==================================
	// 各インスタンス
	ParamManager*		mcls_pParam;	// パラメータ管理クラス
	DataSetManager*		mcls_pDataSet;	// データセットクラス
	// スレッドキュー
	ThreadQueue<TYP_MAE_REQUEST>*	mque_pMaeRequest;		// 前工程情報読込要求キュー

//// メンバー関数
private:
	//=========================================
	// スレッド
	int		ThreadFirst();				// スレッド開始前処理 (スレッド実行直後(シグナルセット前)にコールバック)
	int		ThreadLast();				// スレッド終了前処理 (終了シグナル後にコールバック)
    void	ThreadEvent(int nEventNo);	// スレッドイベント発生

	//=========================================
	// 前工程
	int		GetEnableData(TYP_MAE_REQUEST* pQue);			// 前工程取得状況取得
	void	RegMaeMap();									// 前工程マップ情報登録
	int		RegFFInf(TYP_FF_INF* pFFInf, const int nPnt, const int nMode);	// 前工程ＦＦ情報登録
															// 前工程疵情報登録
	void	RegMaeDefect(TYP_FF_INF* pFFInf, TYP_MAE_DEFECT* pMaeDef);
															// 前工程疵画像読込
	bool	ReadMaeImg(TYP_FF_INF* pFFInf, TYP_MAE_DEFECT* pMaeDef);
	bool	ReadMaeDefect(TYP_FF_INF* pFFInf);				// 前工程疵情報読込
	bool	ReadFFInf(TYP_MAE_REQUEST* pQue);				// 前工程ＦＦ情報読込
	void	ReadMaeInf(TYP_MAE_REQUEST* pQue);				// 前工程情報読込
	int		GetMaeInfSetPos(TYP_FF_INF* pFFInf, int* nMode);
};
