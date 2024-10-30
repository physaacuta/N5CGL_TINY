// *********************************************************************************
//	当タスクの基本定数
//	[Ver]
//		Ver.01    2007/08/01  vs2005 対応
//
//	[メモ]
//		
// *********************************************************************************

#pragma once

#include "..\..\Include\KizuCommon.h"									// 疵検共通ヘッダー
#include "..\..\Include\LineCommon.h"									// 疵検共通ヘッダー

enum QUE_SEND_STAT {
	QUE_SEND_ZISEKI_T = WM_USER + 100,					// 疵実績応答要求 表
	QUE_SEND_ZISEKI_B,									// 疵実績応答要求 裏
	QUE_SEND_STAT_T,									// 検査状態応答要求 表
	QUE_SEND_STAT_B,									// 検査状態応答要求 裏
	QUE_SEND_MAE_T,										// 前工程実績送信要求 表
	QUE_SEND_MAE_B,										// 前工程実績送信要求 裏
	QUE_SEND_HELCY										// ヘルシー伝文送信要求
};

//=======================================
// メッセージ通知
//=======================================
//// 全てMainInctanceで受ける
//       場合によっては、MainInctanceが各ワーカースレッドにたれ流しする
enum ENUM_MAININCTANCE {
	E_DEF_ML_PARAM = WM_USER + 100,									// パラメータ変更通知


	E_DEF_END														// 終了
};

//// 受け渡し構造体
struct Delivery
{
	QUE_SEND_STAT			eStat;							// 要求モード
	PRO_TYPE_KEY 			keyinf;							// コイル特定するためのキー情報
	char					cLine_Code[SIZE_KOUTEI_CODE];	// 工程コード
	char					cMeiNo[4];			// 命令No
	char					cCoilNo[10];			// コイルNo
	char					cFileName[32];					// デバック時 前工程情報伝文ファイル名称
};


//// 送信伝文受け渡し構造体
struct ExecItem
{
	QUE_SEND_STAT			emStat;						// 伝文種類
	//char					cKeyId[SIZE_SERIAL_FF_NO];	// DBのID (無ければNULL)
  	char					cMeiNo[SIZE_MEI_NO+1];		// 命令No
	char					cCoilNo[SIZE_COIL_NO+1];	// コイルNo
	//PRO_TYPE_KEY			keyinf;						// コイル特定するためのキー情報

	//int						nSendNowNo;					// 何番目の伝文 (0オリジン)
	//int						nRecvNowNo;


	//int						nSendMaxNo;					// 何個セットしているか
	int						nSendSize;					// 伝文のサイズ
	BYTE					cSendBuf[65000];			// 送信データ

	//int						nRecvMaxNo;					// 何個受信するべきか
	//int						nRecvSize[4];				// 伝文のサイズ
	//BYTE					cRecvBuf[4][65000];			// 受信データ
};

#define DummyFF "DummyFF"
#define LINE_CODE "54"
#define FILE_FORMAT "FF\\ff_%.4s_%d.bin"