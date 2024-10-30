// *********************************************************************************
//	各ラインで共通のインクルード
//	[Ver]
//		Ver.01    2007/01/30  vs2005 対応
//
//	[メモ]
//		基本的に、書き換え可
// *********************************************************************************

// 個々のシステム固有のインクルード
#pragma once

// φωインクルード
#include "KizuCommon.h"										// φω共通ヘッダー
#include "OutInterface.h"									// 外部インターフェース用ヘッダー


//***************************************************************
// 設定可変定数 定義
//***************************************************************
//=======================================
// 設定定数 定義
//=======================================

//=======================================
// 固定サイズ
//=======================================
// ライン情報
#ifndef LOCAL
	#define COIL_LEN_MAX				10000				// 入側コイル長MAX[m] (どの項目でもこの長さ未満となる)
	#define COIL_LEN_MIN				200					// 入側コイル長MIN[m] (よく言う小コイル)
	#define COIL_WID_MAX				1750				// コイル幅MAX[mm] (どの項目でもこの幅未満となる)
	#define COIL_THICK_MAX				1600				// 材料板厚MAX[μm]
#else
	#define COIL_LEN_MAX				10000				// 入側コイル長MAX[m] (どの項目でもこの長さ未満となる)
	#define COIL_LEN_MIN				200					// 入側コイル長MIN[m] (よく言う小コイル)
	#define COIL_WID_MAX				1750				// コイル幅MAX[mm] (どの項目でもこの幅未満となる)
	#define COIL_THICK_MAX				1600				// 材料板厚MAX[μm]
#endif

// 装置
#define NUM_MEN						2										// 装置 面単位 (この単位で、検査停止が可能) ※ 表/裏じゃない事に注意
#define NUM_MAX_HANTEI				7										// 片面 判定PCの数 (オフライン機も含めた台数)
#define NUM_MAX_CYCLE				1										// 片面 周期PCの数 (オフライン機も含めた台数) ※ 0だとコンパイルが通らない所があるので、0でも1にしておく
#define NUM_MAX_PARA				1										// 片面 ロジックパラランPCの数
#ifndef LOCAL															// オンライン
	#define NUM_HANTEI				5 //NUM_CAM_POS							// 片面 判定PCの数 (片面に繋がる判定PCの数) (オンライン機のみ)
	#define NUM_HANTEI_CYCLE		1										// 片面 周期PCの数 (片面に繋がる周期PCの数) (オンライン機のみ) ※ 0だとコンパイルが通らない所があるので、0でも1にしておく
	#define NUM_HANTEI_PARA			1										// 片面 ロジックパラランPCの数
	#define NUM_RPIEXEC				1										// 片面 全長処理PCの数 (片面に繋がる全長処理PCの数) (オンライン機のみ)
	#define NUM_CAMPAIR				10										// カメラペア数 (カメラペアとは、複数のカメラセットが１判定PC上で動作する場合のカメラセットの塊の個数)(今回は、1PC対2カメラセット)
	#define NUM_CAMPAIR_CYCLE		1										// カメラペア数 (周期PC１台で動作するカメラセット) ※ 0だとコンパイルが通らない所があるので、0でも1にしておく
	#define NUM_CAMPAIR_PARA		1										// カメラペア数 (ロジックパラランPC１台で動作するカメラセット)(今回は、1PC対2カメラセット)
	#define NUM_SENDER				2										// 片面 全長レコーダーPCの数 (片面に繋がる全長レコーダーPCの数)
#else																	// ローカルテスト
	#define NUM_HANTEI				5										// 片面 判定PCの数 (片面に繋がる判定PCの数) (オンライン機のみ)
	#define NUM_HANTEI_CYCLE		1										// 片面 周期PCの数 (片面に繋がる周期PCの数) (オンライン機のみ)
	#define NUM_HANTEI_PARA			1										// ロジックパラランPCの数
	#define NUM_RPIEXEC				1										// 片面 全長処理PCの数 (片面に繋がる全長処理PCの数) (オンライン機のみ)
	#define NUM_CAMPAIR				10										// カメラペア数 (カメラペアとは、複数のカメラセットが１判定PC上で動作する場合のカメラセットの塊の個数)(今回は、1PC対2カメラセット)
	#define NUM_CAMPAIR_CYCLE		1										// カメラペア数 (周期PC１台で動作するカメラセット)(今回は、1対4)
	#define NUM_CAMPAIR_PARA		1										// カメラペア数 (ロジックパラランPC１台で動作するカメラセット)(今回は、1PC対2カメラセット)
	#define NUM_SENDER				2										// 片面 全長レコーダーPCの数 (片面に繋がる全長レコーダーPCの数)
#endif
#define	NUM_ALL_HTBACK				4										// 全面の判定バックアップPCの台数
#define	NUM_ALL_HTPARA				(NUM_HANTEI_PARA*NUM_MEN)				// 全面の判定ロジックパラランPCの台数
#define NUM_ALL_HANTEI				(NUM_HANTEI*NUM_MEN) + NUM_ALL_HTBACK + NUM_ALL_HTPARA	// 全面の判定PCの台数
#define	NUM_ALL_HT					(NUM_HANTEI*NUM_MEN) + NUM_ALL_HTBACK	// 全面の判定バックアップPCの台数（判定ロジックパラランPC含まず）
#define NUM_CAMERA					(MAX_CAMSET*MAX_PHYSICAL_CAMANGLE)		// 全面のカメラ台数 (カメラセット × カメラ角度)
#define MAX_IMAGE_CNT				4										// 画像枚数 (=エンジンから出力される結果数) (色差まではMAX_CAMANGLEとMAX_IMAGE_CNTは同義だったが、式差では、カメラ１台で複数画像が取れる対策)

#define NUM_LIGHT					2										// 片面 照明装置の数
#define NUM_LED_COLOR				3										// 照明装置の色数（RGB）
#define NUM_LED_BLOCK				48										// 画面設定可能な配光ブロック数 (iniファイルは実際のブロック数) (MAX256)
//#define NUM_LED_BLOCK				36										// 画面設定可能な配光ブロック数 (iniファイルは実際のブロック数) (MAX256)

#define	NUM_CAMLINKSW				4										// カメラリンクスイッチ台数
#define	NUM_CAMLINK_BOARD			2										// PC1台当たりの最大入力数 (BOARDになっているが、実際は取り込み口数。)
#define	NUM_CAMLINKSW_CH_MAX		15										// チャンネル上限
#define	NUM_CAMLINKSW_CAM_MAX		8										// カメラセット上限
#define	NUM_CAMLINKSW_CAM_MIN		-8										// カメラセット下限

// システム情報
// 20200520 疵保存個数上限拡張 --->>>
#define	VRCL_DEFECT_NO_START		64001									// VRCL新規疵の入力範囲の疵No(開始)
#define	VRCL_DEFECT_NO_END			65000									// VRCL新規疵の入力範囲の疵No(終了)
#define	VRCL_DEFECT_NUM				(VRCL_DEFECT_NO_END - VRCL_DEFECT_NO_START + 1)	// VRCL新規疵の入力疵個数
//#define MAX_DETECT_SAVE				32000									// 1コイルあたりの保持する疵個数 (表裏共通)
#define MAX_DETECT_SAVE				524000 - (VRCL_DEFECT_NUM / NUM_MEN)	// 1コイルあたりの保持する疵個数 (表裏別)
// 20200520 疵保存個数上限拡張 ---<<<
#define MAX_DETECT_LOWGR_SAVE		25000									// 1コイルあたりの保持する無害Gr疵個数 (表裏別)※1CAPL中央では不使用
//#define MAX_DETECT_IMAGE_SAVE		32000									// 1コイルあたりの保持する疵画像 (表裏別)
//#define MAX_DEFECT_LOWER_IMG_SAVE	7000									// 1コイルあたりの保持する無害疵画像（表裏別）
#define MAX_MAP_DEFECT_NUM			200000									// 1コイルあたりの表示疵個数 (昔は、列数×行数×面でよかったが、深さが加わったため、最大件数を決めるようにする)

#define MAP_REPRES_MIN				5										// 最小代表疵間隔 [m]
#define MAP_COL_INTERVAL			200 									// マップ列1列あたりの幅 [mm]
#define MAP_COL_NUM					8										// マップ最大列数
#define MAP_ROW_NUM					(COIL_LEN_MAX/MAP_REPRES_MIN)			// マップ最大行数 (2000)
#define MAP_STAGE_NUM				3										// マップの切り替え段数 (0:最小間隔 1:実際の表示エリア小 2:実際の表示エリア大)
#define MAP_NEST_NUM				5										// マップのセル内のリスト件数
#define MAP_CYCLE_COLNUM			1										// 周期疵マップ列数
#define LAMP_COL_NUM				8								// 疵接近ランプ最大列数
#define ALARM_COL_NUM				4										// 疵接近アラーム最大列数

#define SIZE_FRAME_IMG_X			4096									// 横フレーム画像サイズ [pixel] ※ 画像処理フレームサイズ
#define SIZE_FRAME_IMG_Y			4096									// 縦フレーム画像サイズ [pixel]
#define SIZE_GRAB_IMG_X				4080									// 横フレーム画像サイズ [pixel] ※ 撮像フレームサイズ (Decaの場合、16と10で割り切れる値)
#define SIZE_GRAB_IMG_Y				4096									// 縦フレーム画像サイズ [pixel]

// 周期条件
//////#define MAP_CYCLE_COLNUM			1									// 周期疵マップ列数
//////#define STARTINDEX_CYCLE_PCID		(NUM_ALL_HANTEI+1)					// 周期PCのPCIDの先頭位置
//////#define GET_CYCLE_PCID(pcid)		(pcid-STARTINDEX_CYCLE_PCID+1)		// 周期PCのPCIDから、1始まりの連番を算出する。
//////#define GET_CYCLE_PCKIND(pckind)	(pckind-PCNO_CYCLE)					// 周期PCのPCKINDから、1始まりの連番を算出する。


// カメラ左側がDS/WS対応
#define CAMLEFT_IS_DS_T				0											// 0:カメラ左側がDS  1:カメラ左側がWS (表面)
#define CAMLEFT_IS_DS_B				0											// 0:カメラ左側がDS  1:カメラ左側がWS (裏面)
#define CAMLEFT_IS_DS(men)			(0==men?CAMLEFT_IS_DS_T:CAMLEFT_IS_DS_B)	// ループが使えるように

// 全長保存
#define	NUM_JOIN_FRAME				1000									// 現フレームから途中参加させるフレーム数
																			// 　途中開始コマンドの処理実績：約56[sec] ⇒ タイムアウト：60[sec]
																			// 　16[fps] × 60[sec] = 960[frame] ⇒ 途中参加フレーム：1000[frame]

//------------------------------------------
// その他 (各システム固有)
#define SIZE_COIL_KEY				20										// コイル特定するためのキー情報 (今回は、コイルNo、命令No)
#define SIZE_COIL_NO				PRO_SIZE_COIL_NO						// コイルNo		サイズ
#define SIZE_MEI_NO					PRO_SIZE_ORDER_NO						// 命令No		サイズ
//#define MAX_CAMERA_GAP_WIDDIV		10										// 複眼ズレ補正 幅方向分割数(=最大グループ数)
#define NUM_SPM_STAT				2										// SPM状態数

#define NUM_TEMPMT_UNIT				4										// 温度パネルメータ個数

#define MAX_SHACUT_COUNT			10										// 最大カット管理回数
#define MAX_PROGRP_TYPE				10										// プロコンへ送信する疵種グループ（0：無し  1～：各対応するグループ）
#define MAX_PROGRP_GRADE			6										// プロコンへ送信するグレードグループ（0：無し  1～：各対応するグループ）

#define NUM_DSP_WPD					1										// 表示基点数
#define BASE_DSP_MEN				1										// 表示基点の基準面（裏面）(表示機能への通板距離送信で使用)

#define MAX_SHALINK_COUNT			3										// 最大出側合成管理回数

// 前工程関係
#define MAX_MAE_LINE				16										// 前工程登録数
#define MAX_MAE_TYPE				64										// 前工程疵種最大登録数
#define MAX_MAE_GRADE				10										// 前工程グレード最大登録数
#define MAX_MAE_IMAGE				4										// 前工程画像最大数
#define MAE_MAP_COL_NUM				1										// 前工程MAP最大列数 

// 重欠陥一覧画面
#define MAX_DEFLIST_BUF				25										// 重欠陥一覧画面用 バッファ件数

// 2024.07.26 Kouno 2CGL検査装置φω精度向上機能導入(長周期) ---------------->>>>>
// 長周期欠陥
#define MAX_LONGPD_FILTER			16										// 長周期の最大フィルタ数
// 2024.07.26 Kouno 2CGL検査装置φω精度向上機能導入(長周期) <<<<<----------------

// 表示ソケット数
#define HY_SOCK_NUM					3										// 表示ソケット数

// 表示位置
enum EM_DIV_DISP {											// 基本
	//------共通
	DIV_LEN_POS_KEN_TOP = 0,									// [通板距離] 検査基点 表　(カメラ位置)
	DIV_LEN_POS_KEN_BOT,										// [通板距離] 検査基点 裏　(カメラ位置)
	DIV_LEN_POS_DSP_TOP,										// [通板距離] 表示基点 表　(検査台位置1)
	DIV_LEN_POS_DSP_BOT,										// [通板距離] 表示基点 裏　(検査台位置2)（検査台位置1と同じ画面を表示）
	DIV_LEN_POS_BASE_END,										// 共通項目の最後

	// 個別
	DIV_DISP_POS_DSP_P1,										// [表示位置] 検査台1位置
	DIV_DISP_POS_DSP_P2,										// [表示位置] 検査台2位置
//	DIV_DISP_POS_DSP_P3,										// [表示位置] 検査台3位置 
//	DIV_DISP_POS_DSP_P4,										// [表示位置] 検査台4位置 
//	DIV_DISP_POS_DSP_P5,										// [表示位置] 検査台5位置 
//	DIV_DISP_POS_DSP_SHR_I = DIV_DISP_POS_BASE_END,

	DIV_DISP_POS_END = DIV_LEN_POS_BASE_END						// 最後に使用
};

#define MAX_DISP_POS				DIV_DISP_POS_END			// 表示基準位置 個数 (0:検査基点 表面 1:検査基点 裏面 2:表示基点 表面 3:表示基点 裏面)
#define MAX_ALARME_POS				2							// 警報出力位置 個数 (0:表面検査台, 1:裏面検査台)

//=======================================
// 固定No算出マクロ (カメラ番号からの算出)
//  0～7の連番から TorB  Pos  Angle  Set の値を決定する
//	0～7の順番は、以下の固定とする
//	  表DS,表DQ,表WQ,表WS
//	  裏DS,裏DQ,裏WQ,裏WS
//		TorB = 0:表 1:裏
//		CamPos = 0:DS 1:WS
//		CamAngle = 0:正
//=======================================
#define NUM_TorB(x)			(x / (NUM_CAM_POS*MAX_PHYSICAL_CAMANGLE))
#define NUM_CamPos(x)		((x % MAX_CAMSET) / MAX_PHYSICAL_CAMANGLE)
#define NUM_CamAngle(x)		(x % MAX_PHYSICAL_CAMANGLE)
#define NUM_CamSet(x)		(x / MAX_PHYSICAL_CAMANGLE)


//=======================================
// データベース
//=======================================
#define		INI_DBZI				"DB_ZI"							// DB接続 用セッション (実績)

#define DB_COIL_INF					"T_COIL_INF"					// [DB] コイル情報テーブル
#define DB_COIL_RESULT				"T_COIL_RESULT"					// [DB] コイル実績テーブル
#define DB_COIL_CAMERA				"T_COIL_CAMERA"					// [DB] カメラ実績テーブル
//#define DB_COIL_OUTINF_R			"T_COIL_OUTINF_R"				// [DB] 出側物理コイル実績テーブル
#define DB_COIL_POSITION			"T_COIL_POSITION"				// [DB] コイル位置情報テーブル
#define DB_COIL_TRAND				"T_COIL_TRAND"					// [DB] コイルトレンド情報テーブル
#define DB_COIL_CUT_POS				"T_COIL_CUT_POS"				// [DB] カット位置テーブル
#define DB_COIL_CUT_RESULT			"T_COIL_CUT_RESULT"				// [DB] カット実績テーブル
#define DB_COIL_SAISUN_RESULT		"T_COIL_SAISUN_RESULT"			// [DB] 採寸実績テーブル
#define DB_COIL_ORDER				"T_COIL_ORDER"					// [DB] コイル通板順位テーブル

#define DB_DEFECT_INF_T				"T_DEFECT_INF_T"				// [DB] 表疵情報テーブル
#define DB_DEFECT_IMG_T				"T_DEFECT_IMG_T"				// [DB] 表疵画像テーブル
#define DB_DEFECT_ATTR0_T			"T_DEFECT_ATTR0_T"				// [DB] 表基本特徴量テーブル
#define DB_DEFECT_ATTR1_T			"T_DEFECT_ATTR1_T"				// [DB] 表カメラ角度１特徴量テーブル
#define DB_DEFECT_ATTR2_T			"T_DEFECT_ATTR2_T"				// [DB] 表カメラ角度２特徴量テーブル
#define DB_DEFECT_ATTR3_T			"T_DEFECT_ATTR3_T"				// [DB] 表カメラ角度３特徴量テーブル
#define DB_DEFECT_ATTR4_T			"T_DEFECT_ATTR4_T"				// [DB] 表カメラ角度４特徴量テーブル

#define DB_DEFECT_INF_B				"T_DEFECT_INF_B"				// [DB] 裏疵情報テーブル
#define DB_DEFECT_IMG_B				"T_DEFECT_IMG_B"				// [DB] 裏疵画像テーブル
#define DB_DEFECT_ATTR0_B			"T_DEFECT_ATTR0_B"				// [DB] 裏基本特徴量テーブル
#define DB_DEFECT_ATTR1_B			"T_DEFECT_ATTR1_B"				// [DB] 裏カメラ角度１特徴量テーブル
#define DB_DEFECT_ATTR2_B			"T_DEFECT_ATTR2_B"				// [DB] 裏カメラ角度２特徴量テーブル
#define DB_DEFECT_ATTR3_B			"T_DEFECT_ATTR3_B"				// [DB] 裏カメラ角度３特徴量テーブル
#define DB_DEFECT_ATTR4_B			"T_DEFECT_ATTR4_B"				// [DB] 裏カメラ角度４特徴量テーブル

//// エッジ検出強化
#define DB_EDGE_COIL_INF			"T_EDGE_COIL_INF"				// [DB] エッジ検出実績テーブル
#define DB_EDGE_CHANGE_INF			"T_EDGE_CHANGE_INF"				// [DB] エッジ検出モード変化管理テーブル

//// 次工程
#define DB_ACCESS_NEXT_COIL_INF		"T_ACCESS_NEXT_COIL_INF"		// [DB] 次工程コイル情報テーブル
#define DB_ACCESS_NEXT_DEFECT_INF	"T_ACCESS_NEXT_DEFECT_INF"		// [DB] 次工程疵情報テーブル

//// 送受信伝文バッファ
#define DB_BUF_PRO_R_COILINF		"BUF_PRO_R_COILINF"				// [DB] プロコンコイル情報受信バッファテーブル
#define DB_BUF_PRO_R_SAISUN			"BUF_PRO_R_SAISUN"				// [DB] プロコン採寸実績受信バッファテーブル
#define DB_BUF_PRO_R_CUT			"BUF_PRO_R_CUT"					// [DB] プロコンカット伝文受信バッファテーブル
#define DB_BUF_PRO_S_ZISEKI			"BUF_PRO_S_ZISEKI"				// [DB] プロコン検査実績送信バッファテーブル
#define DB_BUF_FF_S_ZISEKI			"BUF_FF_S_ZISEKI"				// [DB] FF検査実績送信バッファテーブル
#define DB_BUF_FF_R_MAEINF			"BUF_FF_R_MAEINF"				// [DB] FF前工程疵情報受信バッファテーブル

// 前工程用
#define DB_MAE_MASTER				"M_MAE_MASTER"					// [DB] 前工程マスターテーブル
#define DB_MAE_TYPE_NAME			"M_MAE_TYPE_NAME"				// [DB] 前工程疵種マスターテーブル
#define DB_MAE_GRADE_NAME			"M_MAE_GRADE_NAME"				// [DB] 前工程グレードマスターテーブル

#define DB_MAE_COIL_INF				"T_MAE_COIL_INF"				// [DB] 前工程コイル情報テーブル
#define DB_MAE_COIL_LINE			"T_MAE_COIL_LINE"				// [DB] 前工程ライン別コイル情報テーブル
#define DB_MAE_FF_INF				"T_MAE_FF_INF"					// [DB] 前工程ＦＦ疵情報テーブル
#define DB_MAE_DEFECT_INF			"T_MAE_DEFECT_INF"				// [DB] 前工程疵情報テーブル
#define DB_MAE_DEFECT_IMG			"T_MAE_DEFECT_IMG"				// [DB] 前工程疵画像テーブル

// 2024.07.26 Kouno 2CGL検査装置φω精度向上機能導入(長周期) ---------------->>>>>
// 長周期用
#define DB_LONGPD_COMMON				"M_LONGPD_COMMON"				// [DB] 長周期欠陥検出_マスター設定テーブル
#define DB_LONGPD_PCODE					"M_LONGPD_PCODE"				// [DB] 長周期欠陥検出_検査パターンコード別設定テーブル
#define DB_LONGPD_PCODE_FILTER			"M_LONGPD_PCODE_FILTER"			// [DB] 長周期欠陥検出_検査パターン×フィルター設定テーブル
#define DB_LONGPD_PCODE_FILTER_OUTPUT	"M_LONGPD_PCODE_FILTER_OUTPUT"	// [DB] 長周期欠陥_疵種設定テーブル

#define DB_LONGPD_COIL_RESULT			"T_LONGPD_COIL_RESULT"			// [DB] 長周期欠陥検出_実績テーブル
#define DB_LONGPD_EXEC_RESULT_WIDTH		"T_LONGPD_EXEC_RESULT_WIDTH"	// [DB] 長周期欠陥検出_フィルター実績テーブル
#define DB_LONGPD_EXEC_RESULT_DEFECT_T	"T_LONGPD_EXEC_RESULT_DEFECT_T"	// [DB] 長周期欠陥_表疵情報テーブル
#define DB_LONGPD_EXEC_RESULT_DEFECT_B	"T_LONGPD_EXEC_RESULT_DEFECT_B"	// [DB] 長周期欠陥_裏疵情報テーブル
// 2024.07.26 Kouno 2CGL検査装置φω精度向上機能導入(長周期) <<<<<----------------

//=======================================
// パラメータ変更区分 （ライン固有部分）
//   疵検共通画面の場合は、KizuCommonに記述すること
//=======================================
#define CH_PARAM_DEFLIST				200							// 疵一覧
#define CH_PARAM_SETTIG_ALARM			201							// 疵検警報出力設定
#define CH_PARAM_MAE_MAST				209							// 前工程設定
#define CH_PARAM_TAJUUTHRESHOLD			210							// 多重分解能閾値設定
#define CH_PARAM_REDUCTION				211							// 縮小率設定
#define CH_PARAM_FILTER					212							// フィルタ設定
////#define CH_PARAM_CYCLETHRESHOLD			213						// 周期用分解能閾値設定
#define CH_PARAM_PARA					214                         // ロジックパララン設定
#define CH_PARAM_CROP_SHRINK			215                         // 間引き設定
// 2024.07.26 Kouno 2CGL検査装置φω精度向上機能導入(長周期) ---------------->>>>>
#define CH_PARAM_LONGPD					301							// 長周期欠陥検出
// 2024.07.26 Kouno 2CGL検査装置φω精度向上機能導入(長周期) <<<<<----------------

//=======================================
// タスク名称
//=======================================
// 共通タスク (若干ラインよりのやつ。LineCommonを使わないでよいのであれば、KizuCommonに定義)
#define KT_LEDMGR					"KT_LEDMGR"						// 対LED通信 (アイテック製) (ifdefのスイッチにも使用)
#define KT_LLBGMG					"KT_LLBGMG"						// 対高輝度LED通信 (アイテック製) 

// 統括機能
#define TO_DEFECT					"TO_DEFECT"						// 統括管理
#define TO_CAMERA					"TO_CAMERA"						// カメラコントロール回路通信
#define TO_LINKSW					"TO_LINKSW"						// 対カメラリンクスイッチ通信
#define TO_SEQDM					"TO_SEQDM"						// 対シーケンサ データメモリ通信
#define TO_GETDAT					"TO_GETDAT"						// 入力データ編集管理
#define TO_PUTDAT					"TO_PUTDAT"						// 実績編集処理
#define TO_MAEDAT					"TO_MAEDAT"						// 前工程実績編集処理
#define TO_PROCON					"TO_PROCON"						// 対プロコン通信
#define TO_FFSERV					"TO_FFSERV"						// 対FFサーバー通信
// 2024.07.26 Kouno 2CGL検査装置φω精度向上機能導入(長周期) ---------------->>>>>
#define TO_CHKLONG					"TO_CHKLONG"					// 長周期欠陥監視
// 2024.07.26 Kouno 2CGL検査装置φω精度向上機能導入(長周期) <<<<<----------------

// 判定機能
#define HT_DEFECT					"HT_DEFECT"						// 判定管理

// 表示機能
#define HY_DEFECT					"HY_DEFECT"						// 表示管理
#define HY_TANSHO					"HY_TANSHO"						// 探傷画面
#define HY_DEFLIST					"HY_DEFLIST"					// 疵一覧表示画面
//#define HY_DEFMAP					"HY_DEFMAP"						// 出側単位 疵点マップ画面

// 操作機能
#define SO_GAMENN					"SO_GAMENN"						// 操作画面
#define SO_TRAND					"SO_TRAND"						// トレンド画面
#define SO_DEFMAP					"SO_DEFMAP"						// 入側単位 疵点マップ画面

// 解析機能
#define KA_PARSET					"KA_PARSET"						// パラメータ設定画面
#define KA_PARCOLOR					"KA_PARCOLOR"					// 画像処理エンジン用パラメータ設定画面

//***************************************************************
// 起動要因
//***************************************************************
//=======================================
// 共通タスク機能
//=======================================
// KT_LEDMGR (アイテックTCP/IP)
#define FACT_KT_LEDMGR_01			2401					// パラメータ変更要求		(←TO_DEFECT)
#define FACT_KT_LEDMGR_02			2402					// 設定要求(表面状態指定)	(←TO_DEFECT)
#define FACT_KT_LEDMGR_03			2403					// 設定要求(光量指定)		(←TO_DEFECT)
#define FACT_KT_LEDMGR_04			2404					// 点灯・消灯指示			(←TO_DEFECT)
#define FACT_KT_LEDMGR_05			2405					// 点灯累積時間リセット		(←SO_GAMENN)

// KT_LLBGMG (アイテックTCP/IP)
#define FACT_KT_LLBGMG_01			2801					// パラメータ変更要求		(←TO_DEFECT)
#define FACT_KT_LLBGMG_02			2802					// 設定要求(表面状態指定)	(←TO_DEFECT)
#define FACT_KT_LLBGMG_03			2803					// 設定要求(光量指定)		(←TO_DEFECT)
#define FACT_KT_LLBGMG_04			2804					// 点灯・消灯指示			(←TO_DEFECT)
#define FACT_KT_LLBGMG_05			2805					// 点灯累積時間リセット		(←SO_GAMENN)

//=======================================
// 統括機能
//=======================================
// TO_DEFECT
#define FACT_TO_DEFECT_01			2101					// パラメータ変更要求 (←KA_PARSET)
#define FACT_TO_DEFECT_02			2102					// 検査開始要求 (←SO_GAMENN)
#define FACT_TO_DEFECT_03			2103					// 検査停止要求 (←SO_GAMENN)
#define FACT_TO_DEFECT_04			2104					// 稼動状態取得要求 (←SO_GAMENN)
//#define FACT_TO_DEFECT_05			2105					// ※今回未使用
#define FACT_TO_DEFECT_06			2106					// 判定開始アンサー (←HT_DEFECT)
#define FACT_TO_DEFECT_07			2107					// 判定停止アンサー (←HT_DEFECT)
//#define FACT_TO_DEFECT_08			2108					// 判定パラメータ変更アンサー (←HT_DEFECT)
#define FACT_TO_DEFECT_09			2109					// 判定状態問合せ報告 (←HT_DEFECT)
//#define FACT_TO_DEFECT_10			2110					// フレーム画像取得失敗通知 (←HT_DEFECT)
#define FACT_TO_DEFECT_11			2111					// コイル情報受信通知 (←TO_GETDAT)
#define FACT_TO_DEFECT_12			2112					// 前工程疵情報書込み完了(←TO_MAEDAT)
#define FACT_TO_DEFECT_13			2113					// 実績書き込み状況問合せ (←TO_PUTDAT)
#define FACT_TO_DEFECT_14			2114					// 現露光時間報告 (←TO_CAMERA)
#define FACT_TO_DEFECT_15			2115					// イニシャル伝文受信通知 (←TO_PROCON)
//#define FACT_TO_DEFECT_16			2116					// カット範囲情報通知 (←TO_PUTDAT)
//#define FACT_TO_DEFECT_17			2117					// ※今回未使用
//#define FACT_TO_DEFECT_18			2118					// ※今回未使用
//#define FACT_TO_DEFECT_19			2119					// ※今回未使用
#define FACT_TO_DEFECT_20			2120					// ボイス警報出力解除要求 (←HY_TANSHO)
#define FACT_TO_DEFECT_21			2121					// 途中開始要求 (←SO_GAMENN)
#define FACT_TO_DEFECT_22			2122					// 途中停止要求 (←SO_GAMENN)
#define FACT_TO_DEFECT_23			2123					// 途中開始アンサー (←HT_DEFECT)
#define FACT_TO_DEFECT_24			2124					// 途中停止アンサー (←HT_DEFECT)
// FACT_TO_DEFECT_30以降はTO_LINKSW
#define FACT_TO_DEFECT_30			2130					// カメラリンクスイッチ設定完了通知 (←TO_LINKSW)
#define FACT_TO_DEFECT_31			2131					// 途中参加設定完了通知 (←TO_LINKSW)
#define FACT_TO_DEFECT_32			2132					// カメラ選択設定完了通知 (←TO_LINKSW)

// TO_CAMERA
#define FACT_TO_CAMERA_01			2201					// パラメータ変更通知 (←TO_DEFECT)
#define FACT_TO_CAMERA_02			2202					// 検査用設定コマンド (←TO_DEFECT)
#define FACT_TO_CAMERA_03			2203					// パルス出力設定 (←TO_DEFECT)
#define FACT_TO_CAMERA_04			2204					// AGC設定 (←TO_DEFECT)
#define FACT_TO_CAMERA_05			2205					// コイル切替 (←TO_DEFECT)
#define FACT_TO_CAMERA_06			2206					// 現露光時間問い合わせ (←TO_DEFECT)
#define	FACT_TO_CAMERA_07			2207					// 校正用設定コマンド (←TO_DEFECT)

// TO_LINKSW
#define FACT_TO_LINKSW_01			2301					// パラメータ変更通知 (←TO_DEFECT)
#define FACT_TO_LINKSW_02			2302					// 検査用設定コマンド (←TO_DEFECT)
#define FACT_TO_LINKSW_03			2303					// カメラ選択設定コマンド (←TO_DEFECT)
#define FACT_TO_LINKSW_04			2304					// 途中参加設定コマンド	(←TO_DEFECT)
#define FACT_TO_LINKSW_05			2305					// 高速リセット通知	(←TO_DEFECT)
#define FACT_TO_LINKSW_06			2306					// AOC速度変更指示	(←TO_LINKSW)

// TO_SEQDM
//#define FACT_TO_SEQDM_01			2401					// ※今回未使用

// TO_GETDAT
//#define FACT_TO_GETDAT_01			2501					// パラメータ変更要求 (←TO_DEFECT)
#define FACT_TO_GETDAT_02			2502					// コイル情報受信通知 (←TO_PROCON)

// TO_PUTDAT
//#define FACT_TO_PUTDAT_01			2601					// パラメータ変更要求 (←TO_DEFECT)
#define FACT_TO_PUTDAT_02			2602					// 採寸実績伝文受信通知 (←TO_PROCON)
#define FACT_TO_PUTDAT_03			2603					// カット実績伝文受信通知（←TO_PROCON）
#define FACT_TO_PUTDAT_04			2604					// 実績書き込み完了通知(←TO_DEFECT)

// TO_MAEDAT
//#define FACT_TO_MAEDAT_01			2701					// パラメータ変更要求 (←TO_DEFECT)
#define FACT_TO_MAEDAT_02			2702					// コイル情報受信通知 (←TO_DEFECT)
#define FACT_TO_MAEDAT_03			2703					// 前工程疵情報受信完了通知 (←TO_FFSERV)

// TO_PROCON
#define FACT_TO_PROCON_01			2801					// プロコン検査実績送信要求 (←TO_PUTDAT)
#define FACT_TO_PROCON_09			2809					// デバック用伝文受信通知 (←GOT_CONTROL)

// TO_FFSERV
#define FACT_TO_FFSERV_01			2901					// 検査実績伝文送信要求 (←TO_PUTDAT)
#define FACT_TO_FFSERV_02			2902					// 前工程疵情報要求通知 (←TO_MAEDAT)


//=======================================
// 判定機能
//=======================================
// HT_DEFECT
#define FACT_HT_DEFECT_01			3101					// パラメータ変更通知 (←TO_DEFECT)
#define FACT_HT_DEFECT_02			3102					// 検査開始要求 (←TO_DEFECT)
#define FACT_HT_DEFECT_03			3103					// 検査停止要求 (←TO_DEFECT)
#define FACT_HT_DEFECT_04			3104					// JPEG圧縮率設定 (←TO_DEFECT)
#define FACT_HT_DEFECT_05			3105					// 上位情報設定要求 (←TO_DEFECT)
//#define FACT_HT_DEFECT_06			3106					// 遅延実行キャンセル (←TO_DEFECT)
#define FACT_HT_DEFECT_07			3107					// 続行不可能解除 (←TO_DEFECT)
#define FACT_HT_DEFECT_08			3108					// 状態問合せ (←TO_DEFECT)
//#define FACT_HT_DEFECT_09			3109					// フレーム画像取得要求 (←TO_DEFECT)
//#define FACT_HT_DEFECT_10			3110					// エッジ検出サーチ範囲変更依頼 (←TO_DEFECT)
//#define FACT_HT_DEFECT_11			3111					// 健全性評価データ収集要求 (←TO_DEFECT)
//#define FACT_HT_DEFECT_12			3112					// 画像レコーディング要求 (←TO_DEFECT)
//#define FACT_HT_DEFECT_13			3113					// 判定AGC設定 (←TO_DEFECT)
//#define FACT_HT_DEFECT_14			3114					// 上位情報設定変更要求(分割版) (←TO_DEFECT)
//#define FACT_HT_DEFECT_15			3115					// カメラゲイン変更要求 (←TO_DEFECT)
//#define FACT_HT_DEFECT_16			3116					// 露光時間変更通知 (←TO_DEFECT)
#define FACT_HT_DEFECT_19			3119					// 未実行設定のキャンセル要求 (←TO_DEFECT)
#define FACT_HT_DEFECT_20			3120					// 内部距離カウンタ設定要求 (←TO_DEFECT)
#define FACT_HT_DEFECT_21			3121					// パラメータ事前読込要求 (←TO_DEFECT)
#define FACT_HT_DEFECT_22			3122					// 途中開始要求 (←TO_DEFECT)
#define FACT_HT_DEFECT_23			3123					// 途中停止要求 (←TO_DEFECT)

//=======================================
// 表示機能
//=======================================
// HY_DEFECT
#define FACT_HY_DEFECT_01			5101					// パラメータ変更通知 (←TO_DEFECT)
#define FACT_HY_DEFECT_02			5102					// イニシャル通知 (←TO_DEFECT)
#define FACT_HY_DEFECT_03			5103					// 前工程疵情報読込通知 (←TO_DEFECT)
// 2023.11.17 最終工程検査装置高機能化その４ -------------------->>>>>
#define FACT_HY_DEFECT_04			5104					// 前工程疵情報読込完了通知 (←HY_DEFECT)
// 2023.11.17 最終工程検査装置高機能化その４ <<<<<--------------------
// 2024.07.26 Kouno 2CGL検査装置φω精度向上機能導入(長周期) ---------------->>>>>
#define FACT_HY_DEFECT_05			5105					// 長周期欠陥発生通知 (←TO_LONGCHK)
// 2024.07.26 Kouno 2CGL検査装置φω精度向上機能導入(長周期) <<<<<----------------

// HY_TANSHO
#define FACT_HY_TANSHO_01			5201					// パラメータ変更通知 (←HY_DEFECT)
#define FACT_HY_TANSHO_02			5202					// イニシャル通知 (←HY_DEFECT)

// HY_DEFLIST
#define FACT_HY_DEFLIST_01			5301					// パラメータ変更通知 (←HY_DEFECT)
//#define FACT_HY_DEFLIST_02			5302					// イニシャル通知 (←HY_DEFECT)

// HY_DEFMAP
//#define FACT_HY_DEFMAP_01			5401					// パラメータ変更通知 (←HY_DEFECT)
//#define FACT_HY_DEFMAP_02			5402					// イニシャル通知 (←HY_DEFECT)
//#define	FACT_HY_DEFMAP_03			5403					// 表示更新要求 (←HY_DEFECT)

//=======================================
// 操作機能
//=======================================
// SO_GAMENN
#define FACT_SO_GAMENN_01			6101					// 稼動状態通知 (←TO_DEFECT)
// SO_TRAND
#define FACT_SO_TRAND_01			6201					// 画面更新 (←TO_DEFECT)
// SO_DEFMAP
#define FACT_SO_DEFMAP_01			6301					// パラメータ変更通知 (←TO_DEFECT)
#define FACT_SO_DEFMAP_02			6302					// 画面更新 (←SO_TRAND)


//=======================================
// 解析機能
//=======================================
// KA_PARSET
///// 7101

// KA_CEDATA
///// 7201

//=======================================
// 全長画像保存機能
//=======================================
// TI_RPISAVE
// OutIF_TI_ROISAVE.hに定義する為、ここには不要
//#define FACT_TI_RPISAVE_01			7101					// 検査開始要求 (←TO_DEFECT)
//#define FACT_TI_RPISAVE_02			7102					// 検査停止要求 (←TO_DEFECT)
//#define FACT_TI_RPISAVE_03			7103					// 撮像開始/停止要求 (←TO_DEFECT)
//#define FACT_TI_RPISAVE_04			7104					// 次コイル情報事前通知 (←TO_DEFECT)
//#define FACT_TI_RPISAVE_05			7105					// 外部コイル切替通知 (←TO_DEFECT)
//#define FACT_TI_RPISAVE_06			7106					// 問い合わせ要求 (←TO_DEFECT)
//#define FACT_TI_RPISAVE_07			7107					// 再同期要求 (←TO_DEFECT)
//#define FACT_TI_RPISAVE_08			7108					// 途中参加完了通知 (←TO_DEFECT)

//***************************************************************
// DIV 定義
//   ToHtCommon.h で名称を取得可能
//***************************************************************
//=======================================
// システム状態
//=======================================	
enum EM_DIV_SYS {
	DIV_SYS_INIT	= 1,									// [システム状態] 停止モード
	DIV_SYS_IDLE,											// [システム状態] 準備完了
	DIV_SYS_INITSTART,										// [システム状態] 検査開始初期化中
	DIV_SYS_WAITSTART,										// [システム状態] 検査開始待ち

	DIV_SYS_RUN,											// [システム状態] 検査中
	
	DIV_SYS_WAITSTOP,										// [システム状態] 検査停止待ち
	DIV_SYS_STOP,											// [システム状態] 検査停止
	DIV_SYS_RETRY											// [システム状態] 強制検査再起動中
};

//=======================================
// 周期機能関連
//=======================================
// 周期機能状態
enum EM_DIV_CYCLESTAT {
	DIV_CYCLESTAT_NON  = 1,									// [周期機能状態] 対象外 (SPM開放時等)
	DIV_CYCLESTAT_OK   = 0,									// [周期機能状態] 正常
	DIV_CYCLESTAT_STOP = -9									// [周期機能状態] 停止  (片面異常の場合、検査停止中)
};

//=======================================
// ロジックパララン 稼動状態
//=======================================	
enum EM_DIV_PARA_KADOU {
	DIV_PARA_KADOU_STOP = 0,								// [稼動状態] 停止
	DIV_PARA_KADOU_WAITSTOP,								// [稼動状態] 停止中
	DIV_PARA_KADOU_WAITSTART,								// [稼動状態] 開始中
	DIV_PARA_KADOU_START,									// [稼動状態] 検査中
};

//=======================================
// 担当判定
//=======================================
enum EM_DIV_HANTEI 
{
	DIV_HANTEI_NON = -1,									// [担当判定] 関係無し

	DIV_HANTEI_BK = 0,										// [担当判定] バックアップ
	DIV_HANTEI_HT,											// [担当判定] 単発判定
	DIV_HANTEI_CY,											// [担当判定] 周期判定
	DIV_HANTEI_PR											// [担当判定] パララン判定
};

//=======================================
// 表示管理の受信状態
//=======================================
enum EM_DIV_HY_RECVST
{
	DIV_HY_RECVST_SKIP = -1,								// [受信状態] スキップ (処理遅れにより受信データ破棄区間)
	DIV_HY_RECVST_NON = 0,									// [受信状態] 未受信
	DIV_HY_RECVST_OK										// [受信状態] 正常受信
};

//=======================================
// LED照明装置 カラー 
//=======================================
enum EM_DIV_LED_COLOR
{
	DIV_LED_COLOR_R = 0,									// [LED照明装置] R
	DIV_LED_COLOR_G1,										// [LED照明装置] G1
	DIV_LED_COLOR_G2,										// [LED照明装置] G2
	DIV_LED_COLOR_B1,										// [LED照明装置] B1
	DIV_LED_COLOR_B2,										// [LED照明装置] B2

	//DIV_LED_COLOR_W = 0,									// [LED照明装置] W
	DIV_LED_COLOR_END										// 終端で使用するため注意
};

//=======================================
// LED照明装置 ユニット 
//=======================================
enum EM_DIV_LED_UNIT
{
	DIV_LED_UNIT_R = 0,										// ユニット1 [RGG]
	DIV_LED_UNIT_G = DIV_LED_UNIT_R,						// 
	DIV_LED_UNIT_B,											// ユニット2 [ BB]
	DIV_LED_UNIT_END										// 終端で使用するため注意
};

#define MAX_LED_CH			3								// 1ユニット辺りのチャンネル数
#define MAX_LED_TEMP_UNIT	3								// 温度ユニット数

//=======================================
// 画像情報 (エンジンから出力される画像セット順)
//=======================================
enum EM_DIM_IMAGE_KIND
{
	DIM_IMAGE_KIND_R = 0,									//
	DIM_IMAGE_KIND_G,
	DIM_IMAGE_KIND_B,
	DIM_IMAGE_KIND_GB
};

//=======================================
// カット区分等
//=======================================
enum EM_DIV_CUT
{
	DIV_CUT_FCUT = 1,										// Fカット
	DIV_CUT_LCUT,											// Lカット
	DIV_CUT_DIV,											// 分割
	DIV_CUT_WPD_OTHER										// 異種点

	//DIV_CUT_CUT = 1			0								// 成品 先端-尾端位置
};

#define	ROW_KIND_UPPER		0x01							// 成品先端
#define	ROW_KIND_BOTTOM		0x02							// 成品尾端
#define	ROW_KIND_OTHER		0x04							// 異種点
#define	ROW_KIND_YOSETU		0x08							// 溶接点

//=======================================
// SPM圧下区分
//=======================================
enum EM_DIV_SPM_STAT
{
	DIV_SPM_INI = -1,										// SPM圧下初期値
	DIV_SPM_FALSE = 0,										// SPM圧下無し
	DIV_SPM_TRUE											// SPM圧下有り
};

//=======================================
// 実績格納区分
//=======================================
enum ENUM_END_MODE {
	DIV_END_NON = 0,												// 不明
	DIV_END_NORMAL = 1,												// 通常切り替え
	DIV_END_BACKUP,													// バックアップ切替(検査用コイル長が管理上限に到達時)
	DIV_END_STOP_MANUAL,											// 手動停止
	DIV_END_STOP_AUTO												// 強制停止
};

//=======================================
// プロコン採寸区分
//=======================================
enum EM_DIV_PRO_CUT
{
	DIV_PRO_CUT_NORMAL = 0,									// [プロコン採寸区分] 通常カット
	DIV_PRO_CUT_FRONT_UNION,								// [プロコン採寸区分] 前コイルと結合
	DIV_PRO_CUT_TAIL_UNION,									// [プロコン採寸区分] 後コイルと結合
	DIV_PRO_CUT_UNION									    // [プロコン採寸区分] 両方(前コイル、後ろコイル)結合
};

//***************************************************************
// 機器状態 C++のみ
//***************************************************************
// 情報 (必ずマスターと項目の並びを合わせておく事)
struct TYP_STATUS_READ
{	

#ifndef LOCAL												// オンライン
	TYP_STATUS_INF		Ht_App[NUM_MEN][NUM_HANTEI];							// 判定機能 App
#else														// ローカルテスト
	TYP_STATUS_INF		Ht_App[NUM_MEN][5];										// 判定機能 App
#endif
	TYP_STATUS_INF		Ht_Para_App[NUM_ALL_HTPARA];							// 判定ロジックパララン機能 App

	TYP_STATUS_INF		Ht_Cannot[NUM_ALL_HANTEI];								// 判定PC 続行不能
	TYP_STATUS_INF		AppEnable_Ht[NUM_ALL_HANTEI];							// 判定PC 起動
	TYP_STATUS_INF		HbSend_Ht[NUM_ALL_HANTEI];								// 判定PC ハートビート
	TYP_STATUS_INF		ApErr_Ht[NUM_ALL_HANTEI];								// 判定PC アプリケーション異常 (イベントログ)
	TYP_STATUS_INF		EvtLog_Ht[NUM_ALL_HANTEI];								// 判定PC イベントログで異常検出 (イベントログ)
	TYP_STATUS_INF		HddErr_Ht[NUM_ALL_HANTEI];								// 判定PC HDD異常

	// PC状態
	struct TYP_PC_ST1 {
		TYP_STATUS_INF		AppEnable;											// 起動
		TYP_STATUS_INF		HbSend;												// ハートビート
		TYP_STATUS_INF		ApErr;												// アプリケーション異常 (イベントログ)
		TYP_STATUS_INF		EvtLog;												// イベントログで異常検出 (イベントログ)
	};
	struct TYP_PC_ST2 : public TYP_PC_ST1 {
		TYP_STATUS_INF		HddErr;												// HDD異常
	};

	TYP_PC_ST2			To;														// 統括PC
	TYP_PC_ST2			Bk;														// バックアップPC
	TYP_PC_ST2			Rps;													// 全長保存PC
	TYP_PC_ST2			Rpe[3];													// 全長処理PC
	TYP_PC_ST2			Hy[2];													// 表示PC
	TYP_PC_ST2			So;														// 操作PC

	TYP_STATUS_INF		Ht_NonAnswer[NUM_ALL_HANTEI];							// 判定PC 応答無し (統括判断)
	TYP_STATUS_INF		Ht_Ans[NUM_MEN];										// 片面判定 異常発生

	TYP_STATUS_INF		CamGrab[NUM_CAMERA];									// カメラ 画像入力 (1Port2口)
	TYP_STATUS_INF		CamTempSt[NUM_CAMERA];									// カメラ 温度異常
	TYP_STATUS_INF		CamTemp[NUM_CAMERA];									// カメラ 温度
	TYP_STATUS_INF		CamVolt[NUM_CAMERA];									// カメラ 電圧
	TYP_STATUS_INF		CamGain[MAX_CAMANGLE - 1][NUM_CAMERA];					// カメラゲイン[RGB][カメラ台数]
	TYP_STATUS_INF		CamExpTime[MAX_CAMANGLE - 1][NUM_CAMERA];				// カメラ 露光時間[RGB][カメラ台数]
	TYP_STATUS_INF		CamBrValue[MAX_CAMANGLE - 1][NUM_CAMERA];				// カメラ 輝度[RGB][カメラ台数]


	struct _LED {													// LED装置
		TYP_STATUS_INF		Connect[NUM_MEN][NUM_LIGHT];						// 通信
		struct _CH {
			TYP_STATUS_INF		Break[NUM_MEN][DIV_LED_COLOR_END];				// 断線状態
			TYP_STATUS_INF		TempWar[NUM_MEN][DIV_LED_COLOR_END];			// 温度加熱
			TYP_STATUS_INF		TempErr[NUM_MEN][DIV_LED_COLOR_END];			// 温度異常
			TYP_STATUS_INF		OnOff[NUM_MEN][DIV_LED_COLOR_END];				// 点灯消灯状態 (0:ON  1:OFF)
			TYP_STATUS_INF		ValNow[NUM_MEN][DIV_LED_COLOR_END];				// 設定輝度値
			TYP_STATUS_INF		TotalTime[NUM_MEN][DIV_LED_COLOR_END];			// 稼動累計時間 [分]
		} ch;																	// Ch数
		struct _COLOR {
			TYP_STATUS_INF		TempSt[NUM_MEN][NUM_LED_COLOR][MAX_LED_TEMP_UNIT];	// 温度状態
			TYP_STATUS_INF		Temp[NUM_MEN][NUM_LED_COLOR][MAX_LED_TEMP_UNIT];	// 温度
		} color;																// 3色
	} led;

	TYP_STATUS_INF		LedRunOff[NUM_MEN];										// LED照明消灯01(ライン稼動時)

	TYP_STATUS_INF		CamSwitch_NonAnswer;									// カメラリンクスイッチ通信 応答無し (統括判断)
	TYP_STATUS_INF		CamControl;												// カメラリンクスイッチ カメラコントロールFPGA通信
	TYP_STATUS_INF		CamSwitch;												// カメラリンクスイッチ スイッチFPGA通信
	TYP_STATUS_INF		CamFPGADieTempSt[NUM_CAMLINKSW*MAX_CAMLINKSW_FPGA];		// カメラリンクスイッチ ダイ温度異常
	TYP_STATUS_INF		CamFPGADieTemp[NUM_CAMLINKSW*MAX_CAMLINKSW_FPGA];		// カメラリンクスイッチ ダイ温度
	TYP_STATUS_INF		CamAmbientTempSt[NUM_CAMLINKSW*MAX_CAMLINKSW_FPGA];		// カメラリンクスイッチ 周囲温度異常
	TYP_STATUS_INF		CamAmbientTemp[NUM_CAMLINKSW*MAX_CAMLINKSW_FPGA];		// カメラリンクスイッチ 周囲温度
	TYP_STATUS_INF		CamSwStat[NUM_CAMLINKSW*MAX_CAMLINKSW_FPGA*MAX_CAMLINKSW_INPUT][2];		// カメラリンクスイッチ_CH状態[入力数][B/MF]

	TYP_STATUS_INF		ChilerPiStop[3];										// チラー装置 運転状態 [0:表 1:裏 2:BK]
	TYP_STATUS_INF		ChilerPiErr[3];											// チラー装置 異常状態 [0:表 1:裏 2:BK]

	struct _CHILER {															// チラー装置
		TYP_STATUS_INF		Connect[NUM_MEN];									// 通信
		TYP_STATUS_INF		Sts[NUM_MEN];										// 運転状態
		TYP_STATUS_INF		Err[NUM_MEN];										// 装置状態
		TYP_STATUS_INF		TempSt[NUM_MEN];									// 温度状態
		TYP_STATUS_INF		Temp[NUM_MEN];										// 温度
	} chiler;

	struct _AIRBLOW {															// エアドライヤ装置
		TYP_STATUS_INF		AirBlowPiStop;										// エアドライヤ装置 運転状態
		TYP_STATUS_INF		AirBlowPiErr;										// エアドライヤ装置 異常状態
	} airblow[NUM_MEN];

	//TYP_STATUS_INF		CoolerPiErr[NUM_MEN];									// クーラー装置PI[0:表 1:裏]

	TYP_STATUS_INF		Procon;													// プロコン 通信回線

	TYP_STATUS_INF		DataMem_Ap;												// データメモリ通信
	TYP_STATUS_INF		DataMem_Hb;												// データメモリHB
	TYP_STATUS_INF		SeqState_War;											// シーケンサ状態_注意
	TYP_STATUS_INF		SeqState_Err;											// シーケンサ状態_重故障
	TYP_STATUS_INF		SeqState;												// シーケンサ状態
	TYP_STATUS_INF		SeqKadou;												// シーケンサ稼動状態
	TYP_STATUS_INF		UdpSeq;													// シーケンサUDP回線異常

	TYP_STATUS_INF		FFserv;													// FFサーバー 通信回線
	TYP_STATUS_INF		FFConnectErr;											// FFサーバーイニシャル中


	TYP_STATUS_INF		Div_Kadou;												// 稼動状態
	TYP_STATUS_INF		Div_Unten;												// 運転モード
	TYP_STATUS_INF		Div_Sogyo;												// 操業状態
	TYP_STATUS_INF		Div_PlgMode;											// PLG区分

	TYP_STATUS_INF		Line_Mei;												// 命令No
	TYP_STATUS_INF		Line_No;												// コイルNo
	TYP_STATUS_INF		Line_Len;												// 通板距離
	TYP_STATUS_INF		Line_ZanLen;											// 通板残長
	TYP_STATUS_INF		Line_Speed;												// ライン速度

	TYP_STATUS_INF		Scode[NUM_MEN];											// 検査表面状態
	TYP_STATUS_INF		Pcode[NUM_MEN];											// 検査パターン

	TYP_STATUS_INF		Coil_Init;												// 検査開始1本目
	TYP_STATUS_INF		Coil_ReStart;											// 強制検査再起動
	TYP_STATUS_INF		Coil_Timing_Err;										// 検査開始タイミング不良

	TYP_STATUS_INF		Coil_Ken;												// 検査用コイル情報
	TYP_STATUS_INF		Coil_Dsp;												// 表示用コイル情報

	TYP_STATUS_INF		MaxLen_Ken;												// 検査用コイル長上限到達
	TYP_STATUS_INF		MaxLen_Dsp;												// 表示用コイル長上限到達

	TYP_STATUS_INF		PlgLengthCheckKen;										// 検査用PLG整合性異常
	TYP_STATUS_INF		PlgLengthCheckDsp;										// 表示用PLG整合性異常
	TYP_STATUS_INF		ProHoseiRitu;											// プロコン長補正率

	TYP_STATUS_INF		ZiProCreateErr;											// プロコン検査実績生成失敗
	TYP_STATUS_INF		ZiFfCreateErr;											// FF検査実績送信失敗

	TYP_STATUS_INF		SaveRpiErr[NUM_MEN];									// 全長画像保存

	TYP_STATUS_INF		DB_Tran_A;												// DBトランザクションログ増加 A
	TYP_STATUS_INF		DB_Tran_B;												// DBトランザクションログ増加 B

	TYP_STATUS_INF		DB_BcpErr;												// 画像書き込み失敗

	TYP_STATUS_INF		DB_SaveMax[NUM_MEN];									// 検査実績保存上限到達
	TYP_STATUS_INF		DB_SaveSkip[NUM_MEN];									// 検査実績保存スキップ
	TYP_STATUS_INF		FrameSkip[NUM_MEN];										// 未検査区間
	TYP_STATUS_INF		EdgeLost[NUM_MEN];										// エッジ検出位置不確定
	TYP_STATUS_INF		FrameUnmatch[NUM_MEN];									// フレームNoずれ

	TYP_STATUS_INF		PatoNg[NUM_MEN];										// パトライト出力疵フル

	TYP_STATUS_INF		SecurityDoor;											// 扉状態
	TYP_STATUS_INF		SecurityKey;											// 鍵状態

	TYP_STATUS_INF		DaisyaSt[NUM_MEN];										// 台車位置状態
	TYP_STATUS_INF		DaisyaErr[NUM_MEN];										// 台車位置異常

	struct _TEMPMT {															// 温度パネルメーター
		TYP_STATUS_INF		TempMtConnect;										// 通信
		struct _UNIT {
			TYP_STATUS_INF	Temp[NUM_TEMPMT_UNIT];								// 温度
			TYP_STATUS_INF	TempSt[NUM_TEMPMT_UNIT];							// 温度状態
			TYP_STATUS_INF	Unten[NUM_TEMPMT_UNIT];								// 運転状態
			TYP_STATUS_INF	Status[NUM_TEMPMT_UNIT];							// ステータス
		} unit;
	} tempmt[1];

	TYP_STATUS_INF		ToyuSprSt;												// 塗油スプレー 吹付状態

	TYP_STATUS_INF		RpiImgComp_BcpErr[NUM_MEN];								// 全長画像圧縮失敗

	TYP_STATUS_INF		ParaKadou[NUM_ALL_HTPARA];								// ロジックパララン機能 稼動状態
	TYP_STATUS_INF		ParaCam[NUM_ALL_HTPARA][NUM_CAMLINK_BOARD];				// ロジックパララン機能 入力カメラNo
	TYP_STATUS_INF		ParaState[NUM_ALL_HTPARA][NUM_CAMLINK_BOARD];			// ロジックパララン機能 入力カメラ選択状態
	TYP_STATUS_INF		FrameGrabStop[NUM_MEN];									// 撮像フレーム停止
	TYP_STATUS_INF		DaisyaEx_St[NUM_MEN];									// 台車位置オンライン以外		20210312
	TYP_STATUS_INF		CamGcpErr[NUM_CAMERA];									// カメラ GCP異常

// 2024.07.26 Kouno 2CGL検査装置φω精度向上機能導入(長周期) ---------------->>>>>
	TYP_PC_ST2			LongPd;													// 長周期PC
// 2024.07.26 Kouno 2CGL検査装置φω精度向上機能導入(長周期) <<<<<----------------
};


//***************************************************************
// システム共通構造体
//***************************************************************
//=======================================
// 基本疵情報 構造体 (100byte+α)
//=======================================
struct DETECT_BASE_DATA
{
	int						nKizuNo;						// 疵No (1～の連番)
	
	WORD					TorB;							// 表裏区分 (0:表 1:裏)
	WORD					nTid;							// 疵種EdasysID
	WORD					nGid;							// グレードEdasysID

	WORD					cycle_div;						// 周期疵区分(0:一般疵, 1:簡易周期疵,連続疵 )
	WORD					dsws;							// DSWS区分 (0:DS 1:WS)
	WORD					camset;							// カメラセットID (1～)	表DS, 表DQ, 表WQ, 表WS, 裏DS, 裏DQ, 裏WQ, 裏WS

	float					y;								// 入側先端からの位置[mm]
	float					y_out;							// 出側物理コイル先端からの位置[mm]	※ 統括ではyと同じ距離。 表示管理で、出側距離に振り直し
	float					ds;								// DS側エッジからの位置[mm]
	float					ws;								// WS側エッジからの位置[mm]
	float					dsc;							// 板センターからの距離[mm]
	float					wsc;							// 板センターからの距離[mm]

	int						speed;							// 速度[mpm]
	int						pitch;							// ピッチ[mm]
	float					roolkei;						// ロール径[mm]

	float					aria;							// 面積 [mm^2]
	int						kizu_wid;						// 疵幅 [mm]
	int						kizu_len;						// 疵長さ [mm]

	float					h_res;							// 縦分解能[mm/pixel]
	float					w_res;							// 横分解能[mm/pixel]
	DWORD					nFrameNo;						// フレームNo

	// フレーム画像の左上からの位置
	short					frm_x_min;						// フレーム外接Xmin[pixel]
	short					frm_x_max;						// フレーム外接Xmax[pixel]
	short					frm_y_min;						// フレーム外接Ymin[pixel]
	short					frm_y_max;						// フレーム外接Ymax[pixel]

	// フレーム画像の左上からの間引きした位置
	short					cull_frm_x_min;					// 間引きフレーム外接Xmin[pixel]
	short					cull_frm_x_max;					// 間引きフレーム外接Xmax[pixel]
	short					cull_frm_y_min;					// 間引きフレーム外接Ymin[pixel]
	short					cull_frm_y_max;					// 間引きフレーム外接Ymax[pixel]

	// 切り出し画像関係 (原点は、切り出し画像の左上からの位置)
	short					box_x_min;						// 外接Xmin[pixel]		(切り出し画像の左上からの位置) (マイナス値ありえる)
	short					box_x_max;						// 外接Xmax[pixel]
	short					box_y_min;						// 外接Ymin[pixel]
	short					box_y_max;						// 外接Ymax[pixel]
	short					center_x;						// 疵中心位置X [pixel]  (切り出し画像の左上からの位置)
	short					center_y;						// 疵中心位置Y [pixel]
	WORD					h_size;							// 縦画素数[pixel]
	WORD					w_size;							// 横画素数[pixel]

	// ここまでで 108byte (残84byte以内)------------------------------------------

	// 付帯情報
	WORD					nTYuu;							// 疵種優先度 (1最強)
	WORD					nGYuu;							// グレード優先度 (1最強)
	int						nWidAbsPos;						// 幅方向絶対位置
	short					nWidDiv;						// 幅分割区分 (0:DS ～ 7:WS)
	short					nObjTansho;						// 探傷画面出力対象疵(1:対象)
	short					nObjDeflist;					// 重欠陥一覧画面対象疵(1:対象)
	short					nModeDeflist;					// 重欠陥一覧強調表示対象疵  (0:強調枠無し(自動疵削除)  1:強調枠表示(手動疵削除))  
	short					time[6];						// 疵発生時刻 0:年 1:月 2:日 3:時 4:分 5:秒
	// 元画像の切り出し画像関係 (原点は、切り出し画像の左上からの位置) ※間引き前
	short					base_box_x_min;					// 元画像_外接Xmin[pixel]		(切り出し画像の左上からの位置) (マイナス値ありえる)
	short					base_box_x_max;					// 元画像_外接Xmax[pixel]
	short					base_box_y_min;					// 元画像_外接Ymin[pixel]
	short					base_box_y_max;					// 元画像_外接Ymax[pixel]
	short					base_center_x;					// 元画像_疵中心位置X [pixel]  (切り出し画像の左上からの位置)
	short					base_center_y;					// 元画像_疵中心位置Y [pixel]
	WORD					base_h_size;					// 元画像_縦画素数[pixel]
	WORD					base_w_size;					// 元画像_横画素数[pixel]
	float					base_h_res;						// 元画像_縦分解能[mm/pixel]
	float					base_w_res;						// 元画像_横分解能[mm/pixel]
	int						nShrinkRatio;					// 縮小率 (1, 2, 4)

// 2023.09.14 画像処理装置 H・Sソフト改造 -------------------->>>>>
//	char					cYobi[28];

	int						nIFMergeFlg;					// フレーム間連結フラグ(0:単独疵 1:連結疵先頭 2:連結疵先頭以外)
	char					cYobi[24];
// 2023.09.14 画像処理装置 H・Sソフト改造 <<<<<--------------------

};

//// コイル基本情報 (コイル情報から疵検で必要な情報のみ取り出し)
struct COIL_BASEDATA
{
	// 基本部 (TO_GETDATでセット)
	char				cMeiNo[SIZE_MEI_NO+1];				// 命令No
	char				cCoilNoIn[SIZE_COIL_NO+1];			// 入側コイルNo+NULL  (コイルNo) ガイダンス用
	char				cCoilKey[SIZE_COIL_KEY];			// コイルを一意に特定するためのキー   (コイルNo)
	char				cYobi1[2];							// バウンダリあわせ

	int					nLenIn;								// 入側コイル長 [m] (上位情報の長さ)
	int					nAtu;								// 板厚 [μm]
	int					nWid;								// 板幅 [mm](=通常上位の板幅だが、上位情報が無い場合等は、最大板幅をセットしておく)				

	int					nCutLenF;							// 入側フロント端板長さ[mm]
	int					nCutLenT;							// 入側テール端板長さ[mm]
	char				cYobi2[4];							// 予備
	double				dNobiSPM;							// SPM伸び率[10^2%]
	int					nLenOut;							// 出側コイル長[m]
	char				cYobi3[20];							// 予備

	// 拡張部 (TO_DEFECTでセット)
};

//// コイル基本設定 (コイル情報等から作りこんだ情報)
struct COIL_SETTING
{
	bool				bWalm;								// ウォーマ材区分 (ture:ウォーマ材)
	bool				bDefScode[NUM_MEN];					// 検査表面状態 デフォルト値使用 (true:何かしらの異常でデフォルト値使用時)
	bool				bDefPcode[NUM_MEN];					// 検査パターン デフォルト値使用 (true:何かしらの異常でデフォルト値使用時)
	char				yobi[3];

	int					scode[NUM_MEN];						// 検査表面状態 (表/裏)(1オリジン)
	int					pcode[NUM_MEN];						// 検査パターン (表/裏)(1オリジン)
	int					scode_spmoff[NUM_MEN];				// SPM開放時の検査表面状態 (表/裏)(1オリジン)
	int					pcode_spmoff[NUM_MEN];				// SPM開放時の検査パターン (表/裏)(1オリジン)
	char				cYobi[24];							// 予備
};


//=======================================
// 画面表示用 構造体
//=======================================
/////////////////////
//// コイル情報共通部 128 + 1024 byte
struct COINFO_BASE
{
	char				cKizukenNo[SIZE_KIZUKEN_NO];		// 管理No
	int					scode[NUM_MEN];						// 検査表面状態 (1オリジン)
	int					pcode[NUM_MEN];						// 検査パターン (1オリジン)
	int					nMapColNum;							// マップ列数

	int					nCoilKensa;							// そのコイルで一区間でも縮退時:1 正常:0
	char				cYobi1[84];

	// この間で 1024byte --->
	COIL_BASEDATA			base;							// システム共通のコイル情報
	PRO_TYPE_DATA_COILINF	data;							// コイル情報
	char reserve[1024-sizeof(PRO_TYPE_DATA_COILINF)-sizeof(COIL_BASEDATA)];		// 予備
	// <--------
};



/////////////////////
//// マップ表示用 (行×列テーブル)
// データ
struct DSP_TYP_COLROW								// 16 byte
{
	long					pnt;							// 疵情報テーブルへのポインタ[1～] (0:疵なし)
	short					nTid;							// 疵種EasysID					 (0:疵なし)
	short					nGid;							// グレードEdasysID				(0:疵なし)
	short					yobi[4];
};
// 配置
struct DSP_MAP_COLROW
{
	struct STAGE {
		struct MEN {
			struct ROW {
				struct COL {
					short				nSt;				// ステータス (0:マップ内, 1:エッジ外)
					short				nTotalCnt;			// セルにセット対象の疵トータル個数 (統括、表示両方のトータル件数)
					short				nSetCnt;			// 実際にリストにセットした個数		
					short				nYobi[1];		
					DSP_TYP_COLROW		typ[MAP_NEST_NUM];	// 優先度の高い順にセット

				} col[MAP_COL_NUM];							// 列
			} row[MAP_ROW_NUM];								// 行 (最大コイル長/最小マップ間隔)
		} men[NUM_MEN];										// 表 裏
	} stage[MAP_STAGE_NUM];									// マップ切替用 (0:最小間隔 1:実際の表示エリア小 2:実際の表示エリア大)
};

/////////////////////
//// マップ表示用 (行テーブル)
// データ
struct DSP_TYP_ROW											// 64 byte
{
	int						nEnable;						// 情報受信状態 (昔は、行列情報の-1で判断していた奴) (0:未受信 1:受信)
	EM_DIV_KADOU			emKadou;						// 稼動状態 (DIV_KADOU_INIT･･･)

	EM_DIV_KENSA			emDspKensa;						// 表示用検査状態 (DIV_KENSA_NON･･･)
	EM_DIV_KIKI				emDspKiki;						// 表示用機器状態 (DIV_KIKI_OK･･･)

	int						nKizuCnt;						// 行単位の表示対象疵個数

	int						nLen_i;							// 入側距離 (用は溶接点からの距離) [m]
	int						nLen_o;							// 出側距離 (用はカットからの距離) [m]
	int						nCut;							// シャーカット区分（0:シャーカット無し、1:シャーカットされた）
	int						nKind;							// 付加情報 (0:付加情報無し 0x01:シャーカット)

	int						pnt[2];							// 疵情報へのポインタ[0:最小/1:最大]
	int						pntMae[2];						// 前工程疵情報へのポインタ[0:最小/1:最大]
	
	int						yobi[3];						// 予備
};
// 配置
struct DSP_MAP_ROW
{
	struct STAGE {
		struct MEN {
			struct ROW {	

				DSP_TYP_ROW			typ;					// 詳細情報

			} row[MAP_ROW_NUM];								// 行 (最大コイル長/最小マップ間隔)
		} men[NUM_MEN];										// 表 裏
	} stage[MAP_STAGE_NUM];									// マップ切替用 (0:最小間隔 1:実際の表示エリア小 2:実際の表示エリア大)
};

/////////////////////
//// 疵情報用
// データ
struct DSP_TYP_DEFECTINF									// 440 byte
{
	DETECT_BASE_DATA				data;					// 疵情報

	// VBで2次元配列のマーシャルの方法が分からん為 ヤメ
	//  式差の場合、G,B,R,R-Bの順でセット
	char							image_path_s [SIZE_IMAGE_PATH];		// 画像のパス[正] (画像無し時はALL NULL) (絶対パスでもOK。極力相対パスにしてね)
	char							image_path_r [SIZE_IMAGE_PATH];		// 画像のパス[乱] (画像無し時はALL NULL) (絶対パスでもOK。極力相対パスにしてね)
	char							image_path_r2[SIZE_IMAGE_PATH];		// 画像のパス[乱2](画像無し時はALL NULL) (絶対パスでもOK。極力相対パスにしてね)
	char							image_path_r3[SIZE_IMAGE_PATH];		// 画像のパス[乱3](画像無し時はALL NULL) (絶対パスでもOK。極力相対パスにしてね)

	char reserve[440-sizeof(DETECT_BASE_DATA)-SIZE_IMAGE_PATH*4];		// 予備
};

// 配置
struct DSP_MAP_DEFECTINF
{
	char							KizukenNo[SIZE_KIZUKEN_NO];			// 管理No
	char							cMeiNo[SIZE_MEI_NO+1];				// 命令No
	char							cyobi[3];
	DWORD							num;								// 現在の格納件数
	DWORD							yobi;	
	struct PNT {					
		DSP_TYP_DEFECTINF			typ;
	} pnt[MAX_MAP_DEFECT_NUM];
};

/////////////////////
//// 重欠陥一覧表示ポインタ情報用
// データ
struct DSP_TYP_DEFLIST													// 484 byte	( 44 byte + 440 byte )
{
	char					KizuKenNo[SIZE_KIZUKEN_NO];					// 管理No
	char					CoilNo[SIZE_COIL_NO+1];						// コイルNo
	char					MeiNo[SIZE_MEI_NO+1];						// 命令No
	char					yobi1[2];									// 予備
	DSP_TYP_DEFECTINF		typ;										// 疵情報
};


/////////////////////
//// 前工程疵マップ用
// データ
struct DSP_TYP_TCM_ROW
{
	long					pnt;										// TCM穴情報テーブルへのポインタ
	char					ff_t[2];									// FF疵種
	char					ff_g[2];									// FFグレード
	char					proc[2];									// 前工程コード
	char					yobi[2];									// 予備
	short					t_id;										// 疵種EdasysID
	short					g_id;										// グレードEdasysID
};

// 配置
struct DSP_MAP_ROW_TCM 
{
	struct INTVAL {
		struct TORB {
			struct ROW {

				DSP_TYP_TCM_ROW		typ;		

			} row[COIL_LEN_MAX/MAP_REPRES_MIN];							// 行 (最大コイル長/最小マップ間隔)
		} torb[2];														// 表 裏
	} stage[MAP_STAGE_NUM];
};



//=======================================
// 本システム固有の構造体
//=======================================

// 画像取込開始～統括処理完了までの所要時間
#define MAX_REQUIRED_TIME		10
enum EM_REQUIRED_TIME
{
	EM_TIME_HT_IMGGETSTART_TO_RECVEND = 0,								// 画像取込～判定→統括伝送完了までの所要時間
	EM_TIME_HT_IMGGETSTART_TO_EXECEND,									// 画像取込～統括処理完了までの所要時間
	EM_TIME_HT_IMGEXECEND_TO_RECVSTART,									// 画像処理エンジン処理完了～判定→統括伝送開始までの所要時間
	EM_TIME_TO_RECVSTART_TO_RECVEND_CAM,								// DS(WS)の疵情報受信開始～DS(WS)の疵情報受信開始までの所要時間
	EM_TIME_TO_RECVSTART_TO_RECVEND_ALL,								// DS(WS)の疵情報受信開始～WS(DS)の疵情報受信完了までの所要時間
	EM_TIME_HT_IMGGETSTART_HT_IMGEXECEND								// 画像取込～画像処理エンジン処理完了までの所要時間

};

	
//=======================================
// PC間通信
//=======================================
//-----------------------------------------------------------------------------
//// [統括PC]　⇒　[表示PC]

//// データ部種別
enum EM_TO_SO_DATA_KIND {
											//管理No,表裏,付加情報
	TO_SO_DATA_KIND_STATUS = 0,				// 0 :×:×:○: ステータス情報
	TO_SO_DATA_KIND_COIL_KEN,				// 1 :○:×:○: コイル情報 (検査用)		※ PointTbl nRecWriteBase を更新
	TO_SO_DATA_KIND_COIL_RESULT,			// 2 :○:×:×: コイル実績 (1コイル検査完了時)
	TO_SO_DATA_KIND_COIL_DSP,				// 3 :○:×:×: コイル情報 (表示用)		※ PointTbl nRecDispBase を更新 + nDispOk をTrue
	TO_SO_DATA_KIND_DETECT,					// 4 :○:○:○: 疵情報 (疵情報+疵画像)
	TO_SO_DATA_KIND_LENGTH,					// 5 :×:×:○: 通板位置情報(検査装置位置、表示基点位置、各表示コイル通板位置情報)	
	TO_SO_DATA_KIND_CUT,					// 6 :○:×:○: カット情報（カットPIO信号受信タイミング）
	TO_SO_DATA_KIND_ALARM,					// 7 :×:×:○: 警報状態
	TO_SO_DATA_KIND_DEFLIST_CANCEL,			// 8 :○:○:×: 重欠陥疵情報削除要求
	TO_SO_DATA_KIND_DETECT_NUM				// 9 :○:○:×: 疵個数情報 (最小区間内)
};

////// ヘッダー部[256byte]
struct TO_SO_HEAD {
	SOCK_BASE_HEAD			base_head;						// 共通部(48byte)
	char					KizukenNo[SIZE_KIZUKEN_NO];		// 管理No (自分の持っているレコードをすべて検索する事。同一キーが無い場合は、新規レコードとする) (関係無い場合は、NULLでもセットしておこうかな)
	long					men;							// 表:0 裏:1 (表裏関係無い場合は、-1でもセットしておこうかな)
	EM_TO_SO_DATA_KIND		kind;							// データ部の区分 ( EM_TO_SO_DATA_KIND を使う )

	// データ部を作るほど出ない場合にここにセットしようかなー (32byte)
	union 
	{
		// 基本  (0,1,4,5,6,7)
		struct FL{
			long data[8];
		} fl;
		
		// 2:コイル実績
		struct DATA_COILRESULT {
			long len;												// 実測長 [mm]
			long yobi[7];
		} data_coilresult;

		// 3:コイル情報（表示用）
		struct DATA_COILINF_DSP {
			long nSpmUmu;											// SPM圧下有無(0:圧下無し、1:圧下有り)
			long yobi[7];
		} data_coilinf_dsp;

		// 8:重欠陥表疵情報削除要求
		struct DEFLIST_CANCEL{
			long nKizuSeq;											// キャンセル対象の疵連番 (表裏別 1～)
			long yobi[7];
		} deflist_cancel;
		
		// 9:疵個数情報 (最小区間内)
		struct DATA_DEF_NUM {
			long len;												// 実測長 [mm]
			long num;												// 疵個数情報(最小区間内)
			long yobi[6];
		} data_defnum;
	};
	BYTE					reserve[SIZE_SOCK_HEAD-sizeof(SOCK_BASE_HEAD)-SIZE_KIZUKEN_NO-sizeof(long)*10];
};

//// データ部
// 0:ステータス情報
struct TO_SO_DATA_STATUS {
	int						nUnten;									// 運転状態 (DIV_UNTEN_STOP･･･)
	int						nKadou;									// 稼動状態 (DIV_KADOU_INIT･･･)

	TYP_STATUS_NOWSYSTEM	status;									// ステータス
};	

// 1:コイル情報 (検査用)
struct TO_SO_DATA_COIL {
	COIL_BASEDATA			coilbase;								// システム共通コイル情報

	PRO_TYPE_DATA_COILINF	coilinf;								// コイル情報構造体
	long					scode[NUM_MEN];							// SPM圧下時の検査表面状態 (表/裏)(1オリジン)
	long					pcode[NUM_MEN];							// SPM圧下時の検査パターン (表/裏)(1オリジン)
	long					scode_spmoff[NUM_MEN];					// SPM開放時の検査表面状態 (表/裏)(1オリジン)
	long					pcode_spmoff[NUM_MEN];					// SPM開放時の検査パターン (表/裏)(1オリジン)
	long					nMapColNum;								// マップ列数 (算出不能時は、最大値をセット)
};

// 4,9,10:疵情報 (行列単位)
struct TO_SO_DATA_DETECT {
	long					image_len[MAX_IMAGE_CNT];				// 画像データ長 [正/乱] (ひとつ分の画像サイズ) 

	DETECT_BASE_DATA		datas;									// 疵情報

// ↓どっちが使いやすいかな？
	BYTE					image[1];								// 画像データの先頭ポインタ (実際はJPEG画像がMAX_IMAGE_CNT分 連続している)
};	


// 5:通板位置情報時 (500msで送信)
struct TO_SO_DATA_LENGTH {
	struct TO_SO_DATA_LENGTH_INF {
		char				KizukenNo[SIZE_KIZUKEN_NO];				// 該当位置を通板中の管理No (NULLだったら、lenは不定値の為、使用しない事)
		long				len;									// 該当位置を通板中のコイル先頭からの位置 [m]
	};

	TO_SO_DATA_LENGTH_INF	dsp[MAX_DISP_POS];						// 通板位置情報
};

// ---------- ここからかなりシステム固有-----------
// 6:カット情報
struct TO_SO_DATA_CUT {
	int						nMode;									// 種類 (EM_DIV_CUT)
	int						nPos;									// コイル先端からの位置 [mm]	

};

// 7:警報状態
//		この場合は、管理Noは無いよ
struct TO_SO_DATA_ALARM {
	int						a_mode[MAX_ALARME_POS][NUM_MEN][LAMP_COL_NUM];	// 疵接近ランプ [表検査台位置/裏検査台位置][表/裏][列] (2:重故障 1:軽故障 0:停止中)
};

// 制御種別
enum EM_TO_SO_CTRL_KIND {
	TO_SO_DATA_KIND_START = 0,				// 0 : 検査開始
	TO_SO_DATA_KIND_LINE_CHANGE,			// 1 : 最小区間が切り替わった
	TO_SO_DATA_KIND_STOP					// 2 : 検査停止
};

// 疵情報送信管理制御情報
struct TO_SO_CTRL_SEND {
	long					men;			// 表:0 裏:1
	EM_TO_SO_CTRL_KIND		ctrl_kind;		// 制御部の区分 ( EM_TO_SO_CTRL_KIND を使う )
	char					KizukenNo[SIZE_KIZUKEN_NO];	// 該当位置を通板中の管理No (NULLだったら、lenは不定値の為、使用しない事)
	long					nBeforeRow;		// 前回の行
	long					nCurrentRow;	// 現在の行
};


//-----------------------------------------------------------------------------
//// [判定PC]　⇒　[統括PC]

//----------------
//// エンジンの状態 (判定⇒統括)
struct ENG_STAT {
//	int					nNum;						// セット個数	// 基本1つしかないはず
//	int					nCamId[1];		// (1オリジン)
//	float				fCamTemp[1];
//	float				fCamGain[3][1];	// (RGB)
//	float				fCamExpT[3][1];	// (RGB)

	// NCL_ANSWER_QUERY_STATE から抜粋。全部入れたら 256byteに収まらなかったから。。。

	int					state;										// NCL_ENGINE_STATE
	DWORD				camnum;										// カメラ台数	// 1PC 2カメラ
	DWORD				camid[2];									// カメラ番号	// (1オリジン)
	float				exptime[3][2];								// 露光時間
	float				gain[3][2];									// ゲイン
	float				temp[2];									// カメラ温度
	float				volt[2];									// カメラ電圧
};

//=======================================
// 通信

// 定周期
#pragma pack (4)
// 定周期
struct HT_TO_HEAD_HLS_BASE {	// 256byte固定
	SOCK_BASE_HEAD			base_head;								// 共通部(32byte)

	// データ
	BOOL					nEngStatSet;							// 最新情報更新時 TRUE
	ENG_STAT				EngStat;								// エンジン状態
};
struct HT_TO_HEAD_HLS : public HT_TO_HEAD_HLS_BASE {
	BYTE	reserve[SIZE_SOCK_HEAD - sizeof(HT_TO_HEAD_HLS_BASE)];	// 予備
};

//// 今回は、addrは使わない。1つの連続データとする。が、受信するときは、今まで同じ様に受信可能
struct HT_TO_HEAD_BASE {
	SOCK_BASE_HEAD			base_head;								// 共通部(32byte)
	
	DWORD					nFrameNo;								// フレームNo (0オリジン)
	int						nFrameSkip;								// 0:正常  0以外:フレームスキップ。(エンジンのエラーcodeをセット)
	int						nEdgeNg;								// 0:正常  0以外:エッジ検出失敗
	int						nFrameSt;								// エンジンからのフレーム状態生値 (NCL_FRAME_STATUS)
	int						nJpegCmp;								// JPEG圧縮モード (-1:JPEG圧縮無し 0:疵無し 1～:圧縮したサイズ。=Img以降のオフセット値を参照する場合は、この値分減算すること)

	DWORD					camset;									// カメラセット (1オリジン)
	long					offset;									// 共有メモリのオフセット値 (統括では使用しない) (判定でのみ使用)
	long					size;									// データ部のサイズ

	long					frame_pos;								// 加算カウンタ値 (=front_pos)
	float					edge_pos[2];							// この判定が検出したエッジ位置（カメラ座標系)mm単位  ※今回は左右エッジが入るはず
	float					edge_pos_max[2];						// この判定が検出した最大エッジ位置（カメラ座標系)mm単位  ※今回は左右エッジが入るはず
	float					edge_pos_min[2];						// この判定が検出した最小エッジ位置（カメラ座標系)mm単位  ※今回は左右エッジが入るはず
	BYTE					br[3];									// RGB

	LARGE_INTEGER			ht_freq[2];								// 判定PCのパフォーマンスカウンタの周波数
	LARGE_INTEGER			ht_timestamp_start[2];					// 判定管理 画像処理エンジンからフレーム受信時のタイムスタンプ
	LARGE_INTEGER			ht_timestamp_end[2];					// 判定管理 HT→TO送信時のタイムスタンプ
};
struct HT_TO_HEAD : public HT_TO_HEAD_BASE {
	BYTE	reserve[SIZE_SOCK_HEAD - sizeof(HT_TO_HEAD_BASE)];		// 予備
};

// Pipeデータや
struct HT_TO_DATA {


	// これ以降は、フレームスキップ時には、未セット----->>> 
	long	defect_num;		// このフレームの疵個数（＝結果配列と特徴量配列の数）
	long	image_num;		// このフレームの中の画像の数（＝画像配列の数）


	// この伝文頭からのオフセット量
	//  ※エンジン ヘッダーのオフセット量は使ったらダメ
	//
	//	※データ部内順番
	//	・フレームヘッダ	NCL_SMEM_FRAME_HEADER
	//	・結果 NCL_DEFECT_RESULT * defect_num
	//	・特徴量 double[768] * defect_num					RGB,色差の4CHだけ領域は取るが、Rには値が入らない。
	//	・切り出し画像	DEFECT_HEAD_IMAGE + 切り出し画像 * image_num
	//  ・Jpeg圧縮したフレーム画像。 DEFECT_HEAD_IMAGE + jpegフレーム画像 * image_num
	DWORD		header_offset;
	DWORD		result_offset;
	DWORD		attr_offset;
	DWORD		image_offset;

	DWORD		frame_jpeg_offset;
};
#pragma pack ()

//***************************************************************
// メールスロット構造体
//***************************************************************
struct EVENT_QUE {
	//long event;						// イベントコード
	long nEventNo;											// イベントコード
	long nLineNo;											// ラインNo (通常は、LINE_ID固定。1PCで複数のシステムが同居している場合に 0は1RCL 1は2RCL用とかに使用する。)

	union 
	{

/***
		// コイル情報通知 (TO_PROCOM→TO_DEFECT) 
		struct COILINF {
			COIL_INFO_DATA		data;
			char yobi[SIZE_MSL-sizeof(long) - sizeof(COIL_INFO_DATA)];
		} ev_coilinfo;


		struct TEST{
			long data[SIZE_MSL / sizeof(long) - 1];
		} test;
***/
	};
};
