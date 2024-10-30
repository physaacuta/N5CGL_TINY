#pragma once

#include "LibSelect.h"										// ライブラリ切替ヘッダー

//***************************************************************
// PIO関係
//***************************************************************
//=======================================
// 定数

//// 共有メモリ名称
#ifndef DEFECTLIB_CONTEC_PIO_WMD		// 従来版
	#define SMYU_PIO_TBL_NAME  "TBL_GOT_PIO_%d"
#else									// WMD版
	#define SMYU_PIO_TBL_NAME  "TBL_GOT_PIO_%s"
#endif
#define SMYU_PIO_NET_TBL_NAME  "TBL_GOT_PIO_%s"

//// 定数
#define SMYU_PIO_MAX_BOOD		4							// 最大ボード数
#define SMYU_PIO_MAX_PORT		8							// 最大ポート数
#define SMYU_STORY_MAX_FILE		4							// 最大シナリオ数


//=======================================
// 構造体

// PIO共有メモリ構造体
struct SMYU_TBL_PIO {
	BYTE		nDi[SMYU_PIO_MAX_PORT];						// 読み込みバッファ
	BYTE		nDo[SMYU_PIO_MAX_PORT];						// 書き込みバッファ
};


//***************************************************************
// PLG関係
//***************************************************************
//=======================================
// 定数

//// 共有メモリ名称
#define SMYU_PLG_TBL_NAME  "TBL_GOT_PLG_%s"

//// 定数
#define SMYU_PLG_MAX_CHANNEL		8						// 最大チャンネル数
#define SMYU_PLG_MAX_SAMPBUFNUM		16						// サンプリングバッファ数
//#define SMYU_PLG_MAX_SAMPBUFNUM		32						// サンプリングバッファ数
#define SMYU_PLG_MAX_SAMPBUFSIZE	4096					// サンプリングバッファサイズ
#define SMYU_PLG_BUNSYU				16//4						// 分周 (指定したフレーム周期/この値) (=1フレームでこの回数分だけ分割してセットする) ※ 縦フレームサイズで割り切れる値にしてね


//=======================================
// 構造体

// PLG共有メモリ構造体
struct SMYU_TBL_PLG {
	DWORD		nSamplingCount;								// 最新のサンプリングカウント値

	DWORD		nCount[SMYU_PLG_MAX_CHANNEL];				// 最新のカウント値
	DWORD		nSampBuf[SMYU_PLG_MAX_SAMPBUFNUM*SMYU_PLG_MAX_SAMPBUFSIZE][SMYU_PLG_MAX_CHANNEL];	// サンプリングバッファ

	double		dLen[SMYU_PLG_MAX_CHANNEL];					// 通板距離
};


//***************************************************************
// ＧＯＴシミュレータ関係
//***************************************************************

//=======================================
// タスク名

#define GOT_CONTROL					"GOT_CONTROL"			// GOTシミュレータ管理
#define GOT_CAMERA					"GOT_CAMERA"			// 擬似カメラコントローラ
#define GOT_LIGHTS					"GOT_LIGHTS"			// 擬似メタルハライド光源装置



//=======================================
// 起動要因
#define FACT_GOT_CONTROL_01			10						// 開始停止指示 (fl.data[0]= 0:停止 1:開始)	[← GOT_CAMERA]

//=======================================
// 拡張バスシャーシレス化フラグ
#define SEQ_PLG												// カウンターボードレス化の対応版は、宣言有効
#define SEQ_PIO												// PIOボードレス化の対応版は、宣言有効
