// *********************************************************************************
//	当タスクの基本定数
//	[Ver]
//		Ver.01    2007/08/01  vs2005 対応
//
//	[メモ]
//		
// *********************************************************************************

#pragma once

#include <string.h>

#include "..\..\Include\LineCommon.h"									// 疵検共通ヘッダー

//#define FRAME_HT_SEND_PORT 2	// 暫定
//#define FRAME_RAW_SEND	// 暫定

//=======================================
// デバック

//#define SOT_TEST								// 単体テスト時 コメントアウト
//#define SOT_IMG_OUTPUT						// 単体テスト用中間画像出力時 コメントアウト 
//#define DEBUG_LOG_HT_HEALTHY					// オンライン時 コメントアウト


//=======================================
// 切り替えスイッチ
//   ※これは使える
//=======================================

	
//=======================================
// 固定サイズ
//=======================================
#define PROCESS_INIT_WAIT_TIME		8000		// プロセス開始待ち時間 (画像処理エンジンのエッジ交換待ち時間が10秒のため)
//#define PROCESS_INIT_WAIT_TIME		10000		// プロセス開始待ち時間
#define PROCESS_EXIT_WAIT_TIME		5000		// プロセス終了待ち時間

#define PROCESS_PIPE_WAIT_TIME		2000		// プロセス間通信確立待ち時間
#define COMMAND_INIT_WAIT			90000		// 1分近く掛かることがある//20000 //▲	12000	// 初期化コマンド発行時 アンサーの待ち時間 (統括でアンサー待ちをしている時間より短くする事)
#define COMMAND_DISPOSE_WAIT		20000		// 資源開放コマンド発行時 アンサーの待ち時間 (統括でアンサー待ちをしている時間より短くする事)
#define COMMAND_PARAM_WAIT			30000		// パラメータコマンド発行時 アンサーの待ち時間 (SPMの条件でコイル切り替え時に振り返る可能性がある為)


#define STAT_CHECK_INTERVAL			(3*60*1000)		// 定周期監視周期
#define ST_SEND_INTERVAL			(1*60*1000)		// 統括へのヘルシー送信周期


// ヤメ
//#define HT_ONE_TOP_BOT							// 判定機能 １台で表面、裏面の両面 構成時コメントアウト  ※ かなり怪しいので、当面使わない
//#ifndef HT_ONE_TOP_BOT
//	#define HT_CONNECT_NUM			1			// 1PC 片面動作
//#else
//	#define HT_CONNECT_NUM			2			// 1PC 両面動作
//#endif
#define HT_CONNECT_NUM			1				// 1PC 片面動作 のみ作りこみ



//=======================================
// ログマクロ
#define LOGSOCK(t)  if(mcls_pLogSock!=NULL)       mcls_pLogSock->Write(KizuLib::LogFileManager::t		// ソケット用ログ出力
#define LOGCULL(t)  if(mcls_pLogCull!=NULL)       mcls_pLogCull->Write(KizuLib::LogFileManager::t		// 全長処理用ログ出力




//=======================================
// メッセージ通知
//=======================================
//// 全てMainInctanceで受ける
//       場合によっては、MainInctanceが各ワーカースレッドにたれ流しする
enum ENUM_MAININCTANCE {
	E_DEF_ML_PARAM = WM_USER + 100,				// パラメータ変更通知

	E_DEF_ML_START,								// 検査開始要求
	E_DEF_ML_STOP,								// 検査停止要求
	E_DEF_ML_COILINF,							// 上位設定情報
	E_DEF_ML_CANOK,								// 続行不可能解除
	E_DEF_ML_STATUS,							// 状態問い合わせ
	E_DEF_ML_CLEAR_QUEUE,						// 未実行設定のキャンセル
	E_DEF_ML_SET_POSITION,						// 内部距離カウンタ設定
	E_DEF_ML_PRELOAD_PARAM,						// パラメータ事前読込
	E_DEF_ML_RESUME_START,						// 途中開始要求
	E_DEF_ML_RESUME_STOP,						// 途中停止要求

	E_DEF_END									// 終了
};


//=======================================
// スレッド間受け渡しデータ用 構造体
//=======================================

//// コマンドパイプエラー
struct CPIPE_ERROR
{
	int								nErrCode;						// エラーコード (中身はCommandSender::EM_PIPE_ERR)
	int								nDetailCode;					// エラー詳細コード
	BYTE*							pData;							// 受信データそのもの。※受け渡すものが無い場合はNULL。NULLで無ければ受け取ったほうでdelete必須
};