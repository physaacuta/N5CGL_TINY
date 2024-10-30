// *********************************************************************************
//	当タスクの基本定数
//	[Ver]
//		Ver.01    2007/08/01  vs2005 対応
//
//	[メモ]
//		
// *********************************************************************************

#pragma once

#include "..\..\Include\LineCommon.h"								// 疵検共通ヘッダー

//=======================================
// 可変
//=======================================

//#define	AOI_HEIGHT													// AOIheight有効時、アンコメント

#define ENABLE_MAIL_FORCIBLY_MASTER									// 有効時、メールスロットによるカメラ選択chを強制的にMasterにする

#define	COUNTER_MODE	0											// 遅延カウント信号選択
																		// 0 : LVALカウント
																		// 1 : FVALカウント
static const int LINKW_SERIAL_SPEED = 9600;						// カメラリンクスイッチ・ＰＣ間の速度
//static const int LINKW_SERIAL_SPEED = 115200;						// カメラリンクスイッチ・ＰＣ間の速度

//=======================================
// 固定
//=======================================

#define MAX_THREAD			8											// 最大スレッド管理数

// 表示用項目名
enum EM_DSP_COL_NAME {
	COL_ID = 0,											// ID
	COL_STATUS,											// ｽﾃｰﾀｽ
	COL_BOARD,											// ﾎﾞｰﾄﾞ
	COL_TEMP_SYU = COL_BOARD + MAX_CAMLINKSW_OUTPUT,	// 周囲温度
	COL_TEMP_DAI,										// ダイ温度
};

// ハーフ版カメラリンクスイッチのCC-LINKソケット存在有無
static const bool HARF_CC_SOCK_EXIST[MAX_CAMLINKSW_OUTPUT] = {
	 false		// Ch01	// Ch16
	,false		// Ch02	// Ch17
	,false		// Ch03	// Ch18
	,false		// Ch04	// Ch19
	,false		// Ch05	// Ch20
	,false		// Ch06	// Ch21
	,false		// Ch07	// Ch22
	,false		// Ch08	// Ch23
	,false		// Ch09	// Ch24
	,true		// Ch10	// Ch25
	,true		// Ch11	// Ch26
	,true		// Ch12	// Ch27
	,true		// Ch13	// Ch28
	,true		// Ch14	// Ch29
	,true		// Ch15	// Ch30
};