// *********************************************************************************
//	表示PC共有メモリ用ヘッダー
//	[Ver]
//		Ver.01    2017/06/27  初版作成
//
//	[メモ]
//		基本的に、書き換え可
// *********************************************************************************

// 個々のシステム固有のインクルード
#pragma once

// φωインクルード
#include "LineCommon.h"	// φω共通ヘッダー

//=======================================
// 共有メモリ名称
//=======================================
#define		TBL_HY_STATUS_NAME		"TBL_HY_STATUS"				// ステータス情報テーブル
#define		TBL_HY_POINT_NAME		"TBL_HY_POINT"				// ポインタ情報テーブル
#define		TBL_HY_COILINF_NAME		"TBL_HY_COILINF"			// コイル情報テーブル
#define		TBL_HY_COILRESULT_NAME	"TBL_HY_COILRESULT"			// コイル実績テーブル
//#define		TBL_HY_MAP_COLROW_NAME	"TBL_HY_MAP_COLROW"			// マップ情報テーブル
#define		TBL_HY_MAP_ROW_NAME		"TBL_HY_MAP_ROW"			// マップ位置テーブル
#define		TBL_HY_DEFECTINF_NAME	"TBL_HY_DEFECTINF"			// 疵情報テーブル
#define		TBL_HY_ALARM_NAME		"TBL_HY_ALARM"				// 警報情報テーブル

#define		TBL_HY_DEFLIST_NAME		"TBL_HY_DEFLIST"			// 重欠陥一覧表示ポインタ情報テーブル
//#define		TBL_HY_DSPLENGTH_NAME	"TBL_HY_DSPLENGTH_NAME"		// 表示位置情報テーブル
#define		TBL_HY_DEFMAP_NAME		"TBL_HY_DEFMAP"				// 疵マップ表示情報テーブル

//#define		TBL_HY_MAE_MAP_NAME		"TBL_HY_MAE_MAP"			// 前工程マップ情報テーブル
#define		TBL_HY_MAE_INF_NAME		"TBL_HY_MAE_INF"			// 前工程疵情報テーブル

//#define		TBL_HY_CYCLE_MAP_NAME	"TBL_HY_CYCLE_MAP"			// 周期マップ情報テーブル
//#define		TBL_HY_CYCLE_INF_NAME	"TBL_HY_CYCLE_INF"			// 周期疵情報テーブル

// 2024.07.26 Kouno 2CGL検査装置φω精度向上機能導入(長周期) ---------------->>>>>
#define		TBL_HY_LONGPD_NAME		"TBL_HY_LONGPD"				// 長周期情報テーブル
// 2024.07.26 Kouno 2CGL検査装置φω精度向上機能導入(長周期) <<<<<----------------

//=======================================
// 固定サイズ
//=======================================
#ifndef LOCAL	// 本番用
	#define		TBL_HY_REC_MAX			6						// 表示PC共有メモリ レコード数
	#define		HY_LST_DEFLIST_MAX		25						// 重欠陥一覧表示疵情報 最大件数
	// 前工程関連
	#define		TBL_HY_MAE_INF_MAX		2000					// 前工程疵情報
	//// 周期疵関連
	//#define		TBL_HY_CYCLE_INF_MAX	2000					// 周期疵情報
	#define		DSP_COIL_MAX			5                       // 探傷画面表示コイルMAX(現在コイル＋過去4コイル表示可能)
#else			// 開発用
	#define		TBL_HY_REC_MAX			6						// 表示PC共有メモリ レコード数
	#define		HY_LST_DEFLIST_MAX		25						// 重欠陥一覧表示疵情報 最大件数
	// 前工程関連
	#define		TBL_HY_MAE_INF_MAX		2000					// 前工程疵情報
	//// 周期疵関連
	//#define		TBL_HY_CYCLE_INF_MAX	2000					// 周期疵情報
	#define		DSP_COIL_MAX			2                       // 探傷画面表示コイルMAX(現在コイル＋過去4コイル表示可能)
#endif

#define		TBL_HY_COIL_MAX				TBL_HY_REC_MAX			// 表示PC共有メモリ コイル数

//***************************************************************
// DIV 定義
//   表示管理共有メモリ特有のものを定義する
//***************************************************************
//=======================================
// 画面表示状態
//=======================================
enum DIV_DSP_STATUS {
	DIV_DSP_NG = -1,											// 画面表示不可 (表示通知無しで、次のコイルにいっちゃった)
	DIV_DSP_INITIALIZE,											// データなし   (初期値)
	DIV_DSP_STANDBY,											// 画面表示用意 (検査中だが、対象材がまだ、表示基点を通過していない)
	DIV_DSP_OK,													// 画面表示可
};

//=======================================
// 共有メモリ構造体
//=======================================

//------------------------------
// ステータス情報テーブル
//
struct TBL_HY_STATUS {
	EM_DIV_UNTEN			emUnten;							// 運転状態 (DIV_UNTEN_STOP･･･)
	EM_DIV_KADOU			emKadou;							// 稼動状態 (DIV_KADOU_INIT･･･) 
	TYP_STATUS_NOWSYSTEM	status;								// ステータス
};

//------------------------------
// ポインタ情報テーブル
//
// 各検査台の行情報
struct POINT_ROW_INFO {
	long			nLenDisp[MAX_DISP_POS];								// 通板距離 [m]
																		// [0,1,2]：上流,下流カメラ,表示基点位置固定
	long			nRowDisp[MAX_DISP_POS][MAP_STAGE_NUM];				// 通板行 (最小間隔, 実際の表示エリア小, 実際の表示エリア大) (1オリジン)	(200m単位の場合 0～199mまでは 0)
																		// 	※ 位置情報受信で毎回データセット
																		// 	[0,1,2]：上流,下流カメラ,表示基点位置固定
	long			nRowWrite[NUM_MEN][MAP_STAGE_NUM];					// 現在書き込み行(検査終了時点で最終行になる)(0:表,1:裏) (0オリジン) (200m単位の場合 0～199mまでは 0)
																		// 	※ 行情報受信で毎回データセット
																		// 	※ 該当面停止の場合、位置情報受信時に更新
	long			nRowLast[MAP_STAGE_NUM];							// 検査最終行(検査終了時点で最終行になる) (1オリジン)
																		// 	※ コイル実績で最終値を確定させる
	long			yobi[2];											// 予備
};

// 各レコード単位のポインタ情報
struct POINT_TBL {
	char			cKizukenNo[SIZE_KIZUKEN_NO];						// 管理No
	DIV_DSP_STATUS	emDispStatus;										// 画面表示状態
	POINT_ROW_INFO	typ;												// 位置情報
};
	
//// 全体付加情報
//struct POINT_TBL_INF_NOW {
//	long			nLenZan;											// コイル残長 [mm] (伸び率考慮コイル長 - 検査台位置での通板距離)
//};
// 本体
struct TBL_HY_POINT {
	// nRecWrite,nRecDispBaseは、うまくいけば今後削除予定

	BOOL					nRefresh;									// 更新中フラグ  ※TRUEの時は、[nRecDisp]参照禁止
	long					nRecWriteBase;								// 書込みレコード(表裏共通)   [0:無し  1～:値-1がpntの配列位置となる]					※ 検査用コイル情報受信 で更新
	long					nRecWrite[NUM_MEN];							// 各検査装置位置での 現在レコード(表裏別) [0:無し  1～:値-1がpntの配列位置となる]		※ 行情報受信 で更新

	long					nRecDispBase;								// 表示基点位置での 現在表示レコード(表裏別) [0:無し  1～:値-1がpntの配列位置となる]	※ 表示用コイル情報受信 で更新
	long					nRecDisp[MAX_DISP_POS];						// 各検査台位置での 現在表示レコード(表裏共通) [0:無し  1～:値-1がpntの配列位置となる]	※ TO_SO_DATA_KIND_LENGTH受信で更新 (トラッキング表示時に使用)
																		// 	[0]：上流カメラ位置固定
																		// 	[1]：下流カメラ位置固定
																		// 	[2]：表示基点位置固定 表面
																		// 	[3]：表示基点位置固定 裏面
																		// 	[4]：検査台位置 表面
																		// 	[5]：検査台位置 裏面

	//POINT_TBL_INF_NOW		inf[MAX_DISP_POS];							// 全体付加情報

	long					yobi[2];									// 予備

	POINT_TBL				pnt[TBL_HY_REC_MAX];						// 各通板ポイントでの 現在レコード
};

//------------------------------
// コイル情報テーブル
//
struct TBL_HY_COILINF {
	struct REC {
		COINFO_BASE			coilinf;									// コイル情報構造体
	} rec[TBL_HY_COIL_MAX];
};

//------------------------------
// コイル実績テーブル
//
struct TBL_HY_COILRESULT {
	struct REC {
		long				nLen;										// 検査実測長 [mm]

		long				nCutCnt;									// カット情報セット数
		TO_SO_DATA_CUT		cut[MAX_SHACUT_COUNT];						// カット情報等

	} rec[TBL_HY_COIL_MAX];
};

//------------------------------
// マップ情報テーブル
//
//struct TBL_HY_MAPCOLROW {
//	DSP_MAP_COLROW			rec[TBL_HY_REC_MAX];
//};

//------------------------------
// マップ位置テーブル
//
struct TBL_HY_MAPROW {
	DSP_MAP_ROW				rec[TBL_HY_REC_MAX];
};

//------------------------------
// 疵情報テーブル
//
struct TBL_HY_DEFECTINF {
	DSP_MAP_DEFECTINF		rec[TBL_HY_REC_MAX];
};


//------------------------------
// 重欠陥一覧表示ポインタ情報
//
struct TBL_HY_DEFLIST
{
	struct DSP_LIST_HEADER
	{
		int		nPos[NUM_MEN];											// 最新レコード[ 0:表 1:裏 ] 0:データなし 1～100 データ部の最新レコード 
		char	cYobi1[24];
	} head;
	struct DSP_LIST_DATA
	{
		DSP_TYP_DEFLIST	defect[HY_LST_DEFLIST_MAX];
	} data[NUM_MEN];
};


//------------------------------
// 前工程マップ情報テーブル
//
// 前工程検査情報
struct DSP_TYP_MAE_DEFECTINF
{
	short					nImageFlg;									// 画像形式 (0:Jpeg 1:BMP)
	short					nDspMode;									// 事前編集フラグ (-1:非表示  0:通常表示  1:強調表示) ※プロビジョン

	WORD					men;										// 表裏区分  (0:表 1:裏)
	WORD					cycle_div;									// 周期疵区分(0:一般疵, 1:簡易周期疵,連続疵 2:周期欠陥)
	WORD					dsws;										// DSWS区分  (0:DS 1:WS)
	WORD					yobi;	

	// 長手、幅位置は、FF情報を使用
	int						speed;										// 速度[mpm]
	int						pitch;										// ピッチ[mm]
	float					roolkei;									// ロール径[mm]
	short					box_wid;									// 疵幅 [mm]
	short					box_len;									// 疵長さ [mm]

	// 画像表示に必要
	float					h_res;										// 縦分解能[mm/pixel]
	float					w_res;										// 横分解能[mm/pixel]
	short					box_x_min;									// 外接Xmin[pixel]		(切り出し画像の左上からの位置) (マイナス値ありえる)
	short					box_x_max;									// 外接Xmax[pixel]
	short					box_y_min;									// 外接Ymin[pixel]
	short					box_y_max;									// 外接Ymax[pixel]
	short					center_x;									// 疵中心位置X [pixel]  (切り出し画像の左上からの位置)
	short					center_y;									// 疵中心位置Y [pixel]
	WORD					h_size;										// 縦画素数[pixel]
	WORD					w_size;										// 横画素数[pixel]

};

struct DSP_TYP_MAE_KIZUINF							// 256 byte
{
	DSP_TYP_MAE_DEFECTINF	data;										// 前工程 疵情報

	// VBで2次元配列のマーシャルの方法が分からん為 ヤメ
	char					image_path_1[SIZE_IMAGE_PATH];				// 画像のパス (画像無し時はALL NULL)
	char					image_path_2[SIZE_IMAGE_PATH];				// 画像のパス (画像無し時はALL NULL)
	char					image_path_3[SIZE_IMAGE_PATH];				// 画像のパス (画像無し時はALL NULL)
	char					image_path_4[SIZE_IMAGE_PATH];				// 画像のパス (画像無し時はALL NULL)

	char reserve[256-sizeof(DSP_TYP_MAE_DEFECTINF)-SIZE_IMAGE_PATH*4];		// 予備
};

// FF情報
struct DSP_TYP_MAE_FFINF
{
	char					cMaeKizukenNo[SIZE_KIZUKEN_NO];				// FF管理No (前工程φωでの管理No)
	int						nMaeKizuNo;									// FF疵No   (前工程φωでの疵No)
	char					cT[2];										// FF疵種
	char					cG[2];										// FFグレード
	char					cProc[2];									// FF疵検出工程
	char					yobi1[2];
	int						nDiv;										// FF疵区分
	int						nPos;										// FF疵長さ方向位置 [mm]	※これは使わない
	int						nWidth;										// FF疵巾方向位置 [mm]		(WSからの位置)
	int						nLen;										// FF疵長さ [mm]

	// 拡張
	int						nY;											// 最終長手位置 [mm]
// 2023.11.17 最終工程検査装置高機能化その４ -------------------->>>>>
	int						nY_Out;										// 最終出側長手位置 [mm]
// 2023.11.17 最終工程検査装置高機能化その４ <<<<<--------------------	
	short					nTid;										// MAE_TYPE_NAME  の ID  (0:不明   1～：前工程の対象疵) ※不明の場合は、FF情報で表示。マスター未定義や
	short					nGid;										// MAE_GRADE_NAME の ID  (0:不明)
																		// <20221030><溝口><2TCM実機化に伴う下工程の改造> --->>>
	int						nKouteiID;									// 工程種別
																		// <20221030><溝口><2TCM実機化に伴う下工程の改造> <<<---
};

// 本体
struct TBL_HY_MAE_INF
{
	struct REC {
		struct MEN {
			DWORD						num;							// 現在の格納件数

			struct MAE_INF {
				int						nMaeDsp;						// 前工程疵の表示種類 (0：疵無し   1:前工程疵情報、疵画像表示  2:FF情報のみ表示) ※ MAE_MASTERから取得した値

				DSP_TYP_MAE_FFINF		ff;								// FF情報 (常に有り)
				DSP_TYP_MAE_KIZUINF		inf;							// 前工程検査情報 (無い場合もあり)
				int						nKizuNo;						// 自工程DB疵No
			} maeinf[TBL_HY_MAE_INF_MAX];
		} men[NUM_MEN];													// 表 裏
	} rec[TBL_HY_REC_MAX];
};

//------------------------------
// 警報情報テーブル
//
struct TBL_HY_ALARM {
	TO_SO_DATA_ALARM data;
};

// 2024.07.26 Kouno 2CGL検査装置φω精度向上機能導入(長周期) ---------------->>>>>
//------------------------------
// 長周期情報テーブル
//
struct TBL_HY_LONGPD {
	// 全体ステータス
	int nGuidanceFlag;													// ガイダンス表示フラグ(0:表示なし 1:表示あり) ※現コイル、前コイルで判定

	// コイル単位
	struct REC {
		int			nLongPdOccurrenceFlag;								// 長周期発生有無 (0：長周期欠陥なし, 1：長周期欠陥あり）
	} rec[TBL_HY_COIL_MAX];
};
// 2024.07.26 Kouno 2CGL検査装置φω精度向上機能導入(長周期) <<<<<----------------
