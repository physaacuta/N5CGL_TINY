// *********************************************************************************
//	統括共有メモリ用ヘッダー
//	[Ver]
//		Ver.01    2015/06/15	初版作成
//
//	[メモ]
//		基本的に、書き換え可
// *********************************************************************************

// 個々のシステム固有のインクルード
#pragma once

// 疵検インクルード
#include "LineCommon.h"										// 疵検共通ヘッダー

//=======================================
// 共有メモリ名称
//=======================================
#define		TBL_TO_COIL_NAME		"TBL_TO_COIL"				// コイル情報受け渡しテーブル
#define		TBL_TO_SEQMEM_NAME		"TBL_TO_SEQMEM"				// シーケンサ情報テーブル（データメモリ）
//#define	TBL_TO_CAMERA_NAME		"TBL_TO_CAMERA"				// カメラコントローラ設定情報テーブル（データメモリ）

//=======================================
// コイル情報テーブル
//=======================================
struct TO_COIL_TBL {
	struct TO_COIL_REC {
		COIL_SETTING		setting;							// 基本設定
		COIL_BASEDATA		basedata;							// 基本情報
		BYTE				val[2048
								-sizeof(COIL_SETTING)
								-sizeof(COIL_BASEDATA)];		// コイル情報受け渡しバッファ
	} rec[2];
};

//=======================================
// シーケンサ情報テーブル（データメモリ）
//=======================================
#define DM_DATA_MAX			256									// シーケンサーデータ数
#define DM_AREA_MAX			2									// シーケンサーデータAREA数
#define DM_RECV_REC			0									// 受信エリア (用は、シーケンサ→統括)
#define DM_SEND_REC			1									// 送信エリア (用は、シーケンサ←統括)

struct TO_SEQMEM_TBL {
	// BYTEデータ
	struct FB_DM {
			BYTE	Data[DM_DATA_MAX];							// データ	
	} fb_dm[DM_AREA_MAX];										// DMAREA	( 0:受信エリア 1:送信エリア )
};

#ifdef TBL_TO_CAMERA_NAME
//=======================================
// カメラコントローラ設定情報テーブル（データメモリ）
//=======================================
#define IS_TO_CAMERA_TBL										// 共有メモリ無効時、コメント化

struct TO_CAMERA_TBL {
	struct CAM_INF{
		int		nExpTime;										// 現在露光時間 [clk]
		int		nMode;											// 現在状態 (0:正常範囲   -1:下限到達  1:上限到達)
	} caminf[NUM_CAMERA*MAX_CAMANGLE];							// カメラセット/角度 順
};
#endif