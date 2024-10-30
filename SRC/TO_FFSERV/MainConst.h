// *********************************************************************************
//	当タスクの基本定数
//	[Ver]
//		Ver.01    2012/08/30
//
//	[メモ]
//		
// *********************************************************************************

#pragma once

// 共通インクルード
#include "..\..\Include\LineCommon.h"								// 疵検共通ヘッダー
//=======================================
// 共通定数
//=======================================
static const int PARAM_SYSLOG_BASENO	= 380;						// シスログの既定No
						// 0 : 実績送信
						// 1 : 実績応答伝文受信
						// 2 : 実績応答伝文異常
						// 3 : 前工程疵情報要求伝文送信
						// 4 : 前工程疵実績伝文受信
						// 5 : 前工程疵情報が要求した命令Noと異なる
						// 6 : ヘルシー伝文受信
						// 7 : 応答伝文の表裏整合性異常
						// 8 : FFサーバー通信正常
						// 9 : FFサーバー通信異常
						//10 : FFサーバー伝文送信失敗
						//11 : FFサーバー応答伝文待ちタイムアウト
						//12 : FFサーバー伝文受信異常
						//13 : 送信シーケンス中に応答伝文以外を受信

//=======================================
// メッセージ通知
//=======================================
//// 全てMainInctanceで受ける
//       場合によっては、MainInctanceが各ワーカースレッドにたれ流しする
enum QUE_SEND_STAT {
	QUE_SEND_ZISEKI = WM_USER + 100,					// 疵実績送信要求
	QUE_SEND_MAE,										// 前工程実績要求
	QUE_SEND_HELCY										// ヘルシー伝文送信要求
};

//// 受け渡し構造体
struct Delivery
{
	QUE_SEND_STAT			eStat;							// 要求モード
	PRO_TYPE_KEY 			keyinf;							// コイル特定するためのキー情報
	char					cLine_Code[SIZE_KOUTEI_CODE];	// 工程コード
	char					cMeiNo[SIZE_MEI_NO];			// 命令No
	char					cCoilNo[SIZE_COIL_NO];			// コイルNo
	char					cOmtFileName[32];				// デバック時 実績表ファイル名称
	char					cUraFileName[32];				// デバック時 実績裏ファイル名称
	char					cStOmtFileName[32];				// デバック時 状態表ファイル名称
	char					cStUraFileName[32];				// デバック時 状態裏ファイル名称
};


//// 送信伝文受け渡し構造体
struct ExecItem
{
	QUE_SEND_STAT			emStat;						// 伝文種類
	char					cKeyId[SIZE_SERIAL_FF_NO];	// DBのID (無ければNULL)
  	char					cMeiNo[SIZE_MEI_NO+1];		// 命令No
	char					cCoilNo[SIZE_COIL_NO+1];	// コイルNo
	PRO_TYPE_KEY			keyinf;						// コイル特定するためのキー情報

	int						nSendNowNo;					// 何番目の伝文 (0オリジン)
	int						nRecvNowNo;


	int						nSendMaxNo;					// 何個セットしているか
	int						nSendSize[4];				// 伝文のサイズ
	BYTE					cSendBuf[4][65000];			// 送信データ

	int						nRecvMaxNo;					// 何個受信するべきか
	int						nRecvSize[4];				// 伝文のサイズ
	BYTE					cRecvBuf[4][65000];			// 受信データ
};
