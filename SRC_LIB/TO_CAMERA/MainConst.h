// *********************************************************************************
//	当タスクの基本定数
//	[Ver]
//		Ver.01    2014/10/11  vs2005 対応
//
//	[メモ]
//		
// *********************************************************************************

#pragma once

// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// スイッチ


/////// AGC機能の有無
#define AGCOFF														// AGC機能を無効にする場合アンコメント
																			// 無効にする機能は、以下の通り
																			// ・コイル切り替え時の、初期露光時間セット
																			// ・タスク初期化時の、露光時間セット
																			// ・メールスロットで通知が来れば処理はする。
																			// ※画面からの強制露光時間変更は有効とする。

#define ONLINE_INPUT_C			1										// オンライン入力選択
																			// 1 : オンライン１ (トラッキング)
																			// 2 : オンライン２ (カメラ)
#define	VER_AVALDATA													// AVALDATA製バージョンの場合アンコメント
																			// ※ｶﾒﾗﾘﾝｸSWの場合、露光時間のｶﾒﾗNoは4コ飛ばしCC1〜CC4まであるため

#ifndef AGCOFF			// AGC有効時のみ
  #define	VER_P4_CAMERA												// P4カメラの場合アンコメント
  #define	VER_SCODE_CHG												// 表面状態の変化判断を有効にする場合アンコメント
  //#define VER_IDULE_CAMERA												// iDule ADP4M-B-CLSNTカメラの場合アンコメント	
#endif

#ifdef VER_AVALDATA		// 以下 AVALDATA時のみ、選択可能
  #define	LINKSW_CAM_SELECT_MODE	1									// 各カメラをカメラリンクスイッチのどの入力CHに割り振るかモード
																			// 0:前詰めで接続Ver
																			// 1:等分配で接続Ver

#endif


// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 固有定数

#define MAX_THREAD			8											// 最大スレッド管理数
	
#ifndef FACT_TO_CAMERA_09
  #define FACT_TO_CAMERA_09			2209
#endif

//#define IS_TO_CAMERA_TBL



#ifndef	VER_AVALDATA													// カメラコントローラの場合
#define	NUM_CAMLINKSW			1										// カメラリンクスイッチ台数は1固定
#endif
