// *********************************************************************************
//	当タスクの基本定数
//	[Ver]
//		Ver.01    2012/09/06
//
//	[メモ]
//		
// *********************************************************************************

#ifndef	_MAINCONST_H
#define _MAINCONST_H

// 疵検 インクルード
#include "..\..\include\LineCommon.h"									// 疵検共通ヘッダー

//=======================================
// 共通定数
//=======================================
static const int PARAM_SYSLOG_BASENO = 370;						// シスログの既定No
																// 0 : FFサーバーに該当命令Noコイル無し
																// 1 : 前工程疵情報、疵画像登録失敗
																// 2 : 前工程蓄積DBに接続成功
																// 3 : 前工程蓄積DB Ping応答異常
																// 4 : 前工程蓄積DB ODBC接続異常
																// 5 : 前工程蓄積DB コイル実績なし


// 2TCM DB
#define DB_2TCM_DEFECT_INF_IN		"DEFECT_INF_IN"			// [DB] 入側疵情報テーブル
#define DB_2TCM_DEFECT_IMG_IN		"DEFECT_IMG_IN"			// [DB] 入側疵画像テーブル

// 3TCM DB
#define DB_3TCM_DEFECT_INF_IN		"DEFECT_INF_IN"			// [DB] 入側疵情報テーブル
#define DB_3TCM_DEFECT_IMG_IN		"DEFECT_IMG_IN"			// [DB] 入側疵画像テーブル

// 20200830 冷延　検査装置試験機　ハード・ソフト（2TCM穴検更新） 溝口 ----->>>
// N2TCM_ANA DB
#define DB_N2TCM_ANA_DEFECT_INF		"T_DEFECT_INF"			// [DB] 疵情報テーブル
#define DB_N2TCM_ANA_DEFECT_IMG		"T_DEFECT_IMG"			// [DB] 疵画像テーブル
// 20200830 冷延　検査装置試験機　ハード・ソフト（2TCM穴検更新） 溝口 <<<-----

// 20221021 2TCM実機化に伴う下工程改造 小島 --->>>
// N2TCM_ANA DB
#define DB_N2TCM_KIZU_DEFECT_INF		"T_DEFECT_INF_T"			// [DB] 疵情報テーブル
#define DB_N2TCM_KIZU_DEFECT_IMG		"T_DEFECT_IMG_T"			// [DB] 疵画像テーブル
// 20221021 2TCM実機化に伴う下工程改造 小島 ---<<<


enum ENUM_MAEDATOPERATION {
	// メールスロット受信時
	E_PD_ML_COIL = WM_USER + 100,		// コイル情報受信通知
	E_PD_ML_FF,							// FF前工程疵情報受信通知
	E_PD_END							// 終了
};

// FF前工程実績情報構造体
struct FF_MAE_INF
{
	struct TORB{
		FF_MAE_JISSEKI	typFFData;						
	} torb[NUM_MEN];						// 表 裏				
};

// 前工程疵情報構造体
struct MAE_DEFECT_IMAGE_INF
{
	char cKizuKenNo[SIZE_KIZUKEN_NO];		// 管理No
	char cPreKizuKenNo[SIZE_KIZUKEN_NO];	// 前工程管理No
	int  nKouteiSyubetu;					// 工程種別「疵検出工程判」部分の何番目か
	int  nKizuNo;							// 疵No
	int  nMaeKizuNo;						// 前工程疵No
	int  nOuDiv;							// 表裏区分
	int  nCnt;								// 画像データ数
	DETECT_BASE_DATA typDefectData;			// 疵情報
	int  nGazouNo;							// 画像No
	DWORD nSize[MAX_MAE_IMAGE];				// 画像サイズ
	BYTE* pImg[MAX_MAE_IMAGE];				// 画像データ
};

#endif
