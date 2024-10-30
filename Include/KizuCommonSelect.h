// *********************************************************************************
//	全φωでの切替定数のインクルード
//	[Ver]
//		Ver.01    2016/06/09  初版作成
//
//	[メモ]
//		基本的に、書き換え可 (切り替えスイッチ用のみ)
// *********************************************************************************

#ifndef	_KIZUCOMMONSELECT_H
#define	_KIZUCOMMONSELECT_H

// φωインクルード
#include "LibSelect.h"										// ライブラリ切替ヘッダー
#include "KizuListID.h"										// 疵検共通ヘッダー

//***************************************************************
// 設定可変定数 定義
//***************************************************************
//=======================================
// 横展開用ライン区分
//=======================================
// LINEスイッチ (必ず1コのみ有効にする事)
// #define LINE_		←基本系

// φω
#define LINE_5CGL_TINY



//=======================================
// 機能追加 過渡期用スイッチ (define定義で機能有効化となるように作ること)
//=======================================
#define ENABLE_RPI_SAVE										// 全長保存機能有効
#define ENABLE_RPI_TRANS									// フレーム画像転送(HT→RPI)有効


//=======================================
//// 疵検設備関係
//=======================================
//// カメラ設置 幅方向位置
//#define NUM_CAM_POS_1				1						// 面当りのカメラ位置数 (C)
//#define NUM_CAM_POS_2				2						// 面当りのカメラ位置数 (DS,WS)
//#define NUM_CAM_POS_3				3						// 面当りのカメラ位置数 (DS,C,WS)
//#define NUM_CAM_POS_4				4						// 面当りのカメラ位置数 (DS,D1,W1,WS)
//#define NUM_CAM_POS_5				5						// 面当りのカメラ位置数 (DS,D1,C,W1,WS)
//#define NUM_CAM_POS_6				6						// 面当りのカメラ位置数 (DS,D1,D2,W2,W1,WS)
//#define NUM_CAM_POS_7				7						// 面当りのカメラ位置数 (DS,D1,D2,C,W2,W1,WS)
//#define NUM_CAM_POS_8				8						// 面当りのカメラ位置数 (DS,D1,D2,D3,W3,W2,W1,WS)
//#define NUM_CAM_POS_9				9						// 面当りのカメラ位置数 (DS,D1,D2,D3,C,W3,W2,W1,WS)
#define NUM_CAM_POS_10			10						// 面当りのカメラ位置数 (DS,D1,D2,D3,D4,W4,W3,W2,W1,WS)
//#define NUM_CAM_POS_11			11						// 面当りのカメラ位置数 (DS,D1,D2,D3,D4,C,W4,W3,W2,W1,WS)
//#define NUM_CAM_POS_12			12						// 面当りのカメラ位置数 (DS,D1,D2,D3,D4,D5,W5,W4,W3,W2,W1,WS)
//#define NUM_CAM_POS_13			13						// 面当りのカメラ位置数 (DS,D1,D2,D3,D4,D5,C,W5,W4,W3,W2,W1,WS)
//#define NUM_CAM_POS_14			14						// 面当りのカメラ位置数 (DS,D1,D2,D3,D4,D5,D6,W6,W5,W4,W3,W2,W1,WS)
//#define NUM_CAM_POS_15			15						// 面当りのカメラ位置数 (DS,D1,D2,D3,D4,D5,D6,C,W6,W5,W4,W3,W2,W1,WS)
//#define NUM_CAM_POS_16			16						// 面当りのカメラ位置数 (DS,D1,D2,D3,D4,D5,D6,D7,W7,W6,W5,W4,W3,W2,W1,WS)




//// カメラ設置 角度
//#define NUM_CAM_ANGLE_1				1						// カメラ角度数 (1=単眼)
//#define NUM_CAM_ANGLE_2				2						// カメラ角度数 (2=正・乱)
//#define NUM_CAM_ANGLE_3				3						// カメラ角度数 (3=正・乱・乱)
#define NUM_CAM_ANGLE_4				4						// カメラ角度数 (4=正・乱・乱・乱)		式差時 (G,B,R, R-B)
//#define NUM_CAM_ANGLE_5				5						// カメラ角度数 (5=正・乱・乱・乱・乱)


// カメラ設置 幅方向位置 (自動スイッチ)
#ifdef NUM_CAM_POS_1
	#define NUM_CAM_POS					NUM_CAM_POS_1
#endif
#ifdef NUM_CAM_POS_2
	#define NUM_CAM_POS					NUM_CAM_POS_2
#endif
#ifdef NUM_CAM_POS_3
	#define NUM_CAM_POS					NUM_CAM_POS_3
#endif
#ifdef NUM_CAM_POS_4
	#define NUM_CAM_POS					NUM_CAM_POS_4
#endif
#ifdef NUM_CAM_POS_5
	#define NUM_CAM_POS					NUM_CAM_POS_5
#endif
#ifdef NUM_CAM_POS_6
	#define NUM_CAM_POS					NUM_CAM_POS_6
#endif
#ifdef NUM_CAM_POS_7
	#define NUM_CAM_POS					NUM_CAM_POS_7
#endif
#ifdef NUM_CAM_POS_8
	#define NUM_CAM_POS					NUM_CAM_POS_8
#endif
#ifdef NUM_CAM_POS_9
	#define NUM_CAM_POS					NUM_CAM_POS_9
#endif
#ifdef NUM_CAM_POS_10
	#define NUM_CAM_POS					NUM_CAM_POS_10
#endif
#ifdef NUM_CAM_POS_11
	#define NUM_CAM_POS					NUM_CAM_POS_11
#endif
#ifdef NUM_CAM_POS_12
	#define NUM_CAM_POS					NUM_CAM_POS_12
#endif
#ifdef NUM_CAM_POS_13
	#define NUM_CAM_POS					NUM_CAM_POS_13
#endif
#ifdef NUM_CAM_POS_14
	#define NUM_CAM_POS					NUM_CAM_POS_14
#endif
#ifdef NUM_CAM_POS_15
	#define NUM_CAM_POS					NUM_CAM_POS_15
#endif
#ifdef NUM_CAM_POS_16
	#define NUM_CAM_POS					NUM_CAM_POS_16
#endif


// カメラ設置 角度 (自動スイッチ)
#ifdef NUM_CAM_ANGLE_1
	#define NUM_CAM_ANGLE				NUM_CAM_ANGLE_1
#endif
#ifdef NUM_CAM_ANGLE_2
	#define NUM_CAM_ANGLE				NUM_CAM_ANGLE_2
#endif
#ifdef NUM_CAM_ANGLE_3
	#define NUM_CAM_ANGLE				NUM_CAM_ANGLE_3
#endif
#ifdef NUM_CAM_ANGLE_4
	#define NUM_CAM_ANGLE				NUM_CAM_ANGLE_4
#endif


//***************************************************************
// 設定定数 定義
//***************************************************************

// 横展開用ID
// #define LINE_ID_		←基本系
#define LINE_ID_N5CGL_TINY		0

// 現ラインID (必須：横展開無しなら0でOK)
#define LINE_ID					LINE_ID_N5CGL_TINY


// 管理Noの先頭の2桁コード
#define KIZUKEN_CODE			KIZUKEN_CODE_N5CGL_TINY


// 工程コード
#define KOUTEI_CODE				KOUTEI_CODE_5CGL


//=======================================
// PC名称 (C++のみ)
//	※ これ以外の項目については、LineCommonに定義する
//=======================================
#define MAX_PCCOUNT					64					// 本システムの全PC数 (バックアップ機含む)
extern char PCNAME[MAX_PCCOUNT][16];					// PC名称記憶領域 (PCKIND順) (基本的に直接参照禁止) (ここでは、宣言のみ。実体は、KizuLib.cpp内にある)

//// PC開始位置 (PCKINDの順番) ※0オリジン
#define PCNO_HANTEI			0
#define PCNO_CYCLE			PCNO_HANTEI					// 周期が無しの場合は、HANTEIと同じでよい
//#define PCNO_HTBACK			(5-1)						// 判定のバックアップPC
#define PCNO_PARA			(15-1)

#define PCNO_TOKATU			(21-1)
#define PCNO_TIKUSEKI		PCNO_TOKATU					// 統括兼蓄積PCの為、PCID同一
#define	PCNO_BACKUP			(22-1)
#define	PCNO_HYOZI			(26-1)
#define	PCNO_SOUSA			(28-1)
#define	PCNO_RPIEXEC		(31-1)
#define	PCNO_RPISAVE		(34-1)


//// PC名称取得マクロ
#ifndef LOCAL	// 本番用
		#define PCID_NAME(pckind)		PCNAME[pckind-1]			// PC名称 (pcid 1オリジン)　PCNO_*** を使う場合は、PCID_NAME(***+1) すること。

		#define HANTEI(no)				PCNAME[PCNO_HANTEI+no]		// 判定機能を有するPC名称 (0オリジン)
		#define CYCLE(no)				PCNAME[PCNO_CYCLE+no]		// 周期判定機能を有するPC名称 (0オリジン)
//		#define HTBACK(no)				PCNAME[PCNO_HTBACK+no]		// 判定のバックアップ機能を有するPC名称 (0オリジン)

		#define TOKATU					PCNAME[PCNO_TOKATU]			// 統括機能を有するPC名称
		#define TIKUSEKI				PCNAME[PCNO_TIKUSEKI]		// 蓄積機能を有するPC名称
		#define SOUSA					PCNAME[PCNO_SOUSA]			// 操作機能を有するPC名称				※実質使わない。操作単独の通知は無し
		#define HYOUZI(no)				PCNAME[PCNO_HYOZI+no]		// 表示機能を有するPC名称 (0オリジン)
		#define RPIEXEC(no)				PCNAME[PCNO_RPIEXEC+no]		// 全長画像処理機能を有するPC名称 (0オリジン)
		#define RPISAVE(no)				PCNAME[PCNO_RPISAVE+no]		// 全長画像保存機能を有するPC名称 (0オリジン)
#else			// 開発用
		#define PCID_NAME(pckind)		PCNAME[pckind-1]			// PC名称 (pcid 1オリジン)

		#define HANTEI(no)				PCNAME[PCNO_HANTEI+no]		// 判定機能を有するPC名称 (0オリジン)
		#define CYCLE(no)				PCNAME[PCNO_CYCLE+no]		// 周期判定機能を有するPC名称 (0オリジン)
//		#define HTBACK(no)				PCNAME[PCNO_HTBACK+no]		// 判定、周期、全長のバックアップ機能を有するPC名称 (0オリジン)
		#define PARA(no)				PCNAME[PCNO_PARA+no]		// パララン判定機能を有するPC名称 (0オリジン)

		#define TOKATU					PCNAME[PCNO_TOKATU]			// 統括機能を有するPC名称
		#define TIKUSEKI				PCNAME[PCNO_TIKUSEKI]		// 蓄積機能を有するPC名称
		#define SOUSA					PCNAME[PCNO_SOUSA]			// 操作機能を有するPC名称				※使わない。操作単独の通知は無し
		#define HYOUZI(no)				PCNAME[PCNO_HYOZI+no]		// 表示機能を有するPC名称 (0オリジン)
		#define RPIEXEC(no)				PCNAME[PCNO_RPIEXEC+no]		// 全長画像処理機能を有するPC名称 (0オリジン)
		#define RPISAVE(no)				PCNAME[PCNO_RPISAVE+no]		// 全長画像保存機能を有するPC名称 (0オリジン)
#endif

#endif
  