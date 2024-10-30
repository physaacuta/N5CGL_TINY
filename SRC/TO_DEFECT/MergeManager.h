//=====================================================================
// マージクラス
//	【全ライン】
//		・左右カメラのオーバーラップ部疵の重なり判定を行う
//=====================================================================

#pragma once


// 半固定
#include "..\..\Include\LineCommon.h"									// 疵検インクルード
#include "..\..\Library\KizuLib\KizuLib.h"								// 疵検ライブラリ
#include "..\..\Library\KizuLib\Class\KizuMacro.h"						// マクロ集
#include "..\..\Library\KizuLib\Class\KizuFunction.h"					// スタティックライブラリ集
#include "..\..\Library\KizuLibMFC\Class\LogFileManager.h"				// ログ管理クラス


// EPCインクルード
//#include "..\..\Include\EPC\NCattr.h"
//#include "..\..\Include\EPC\NCsmemData.h"
#include "..\..\Include\EPC\NCL2DETECT_if.h"							// I/F定義
#include "..\..\Include\EPC\NCL2attr.h"									// 特徴量定義

#include "..\..\Include\TohtCommon.h"


using namespace KizuLib;

class MergeManager
{
public :

	// パラメータ
	struct  PARAM_MERGE {
		double			dOffset_x[NUM_CAM_POS];							// 片面判定数分
		double			dLapRitu;										// 外接長方形重なり
	};

//private:

	//=======================================
	// 構造体
	//=======================================

	// 重なりリストのデータ構造 
	struct  nodeLap {
		int				vertex;											// 重なり番号
		nodeLap*		pNext;											// ポインタ
	};


	// 欠陥対象 構造体
	struct MergeInfo {
		double*			pAttr;											// 特徴量ポインタ

		int				nCamset;										// カメラセット

		// 代表疵選別と同じ重なり判定用
		int				yusenG;											// グレード優先度
		int				yusenT;											// 疵種優先度
		double			area;											// 疵面積
		double			len;											// 長手位置


		// よく使う項目を使いやすいように取り出しておく
		double			xmin;											// 全カメラ共通の絶対座標
		double			xmax;
		double			ymin;
		double			ymax;


		// 重なりリスト (グラフの結合性判定用)
		nodeLap*		lstLap;											// 重なりリスト (重なっている疵がある場合は、こいつにどんどん追加していく)
		bool			isChecked;										// 連結性判定用
	};
	


public :
	MergeManager(void);
	virtual ~MergeManager(void);

	void SetLogMgr(LogFileManager* pLog)   { mcls_pLog = pLog; };		// ログファイルセット
	void SetParam(PARAM_MERGE* prm)	{ memcpy(&m_prm, prm, sizeof(m_prm)); }	// パラメータセット


	void	DoMerge(int* nDetectCnt, MergeInfo* lst);	// マージ処理実行

private:
	double	CheckLap(const MergeInfo& a, const MergeInfo& b);			// 重なり率チェック
	int		Visit(int myrec, int maxrec, MergeInfo* lst);				// 深さ優先探索

	//// 各インスタンス
	LogFileManager*			mcls_pLog;									// LogFileManagerスレッドインスタンス


	PARAM_MERGE				m_prm;										// パラメータ
};
