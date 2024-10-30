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


// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 事象用

//=======================================
// イベント種別
//=======================================
static const char	STR_KIND_PLG[]	= "PLG";
static const char	STR_KIND_PI[]	= "PI";
static const char	STR_KIND_PO[]	= "PO";
static const char	STR_KIND_MEM[]	= "MEM";
static const char	STR_KIND_MAIL[] = "MAIL";
static const char	STR_KIND_TIME[] = "TIME";
static const char	STR_KIND_ZZ[]	= "ZZ";

//=======================================
// 列挙対
//=======================================
//// 種別
enum EM_DIV_KIND {
	DIV_KIND_NON	= 0,
	DIV_KIND_PLG	= 1,
	DIV_KIND_PI,
	DIV_KIND_PO,
	DIV_KIND_MEM,
	DIV_KIND_MAIL,
	DIV_KIND_TIME,

	DIV_KIND_ZZ
};
//// メールスロットデータ部構造体
enum EM_DIV_MAIL {	
	DIV_MAIL_FL = 0,											// fl
	DIV_MAIL_FC,												// fc
	DIV_MAIL_MIX												// mix
};

//=======================================
// 構造体
//=======================================
//// シナリオデータ
struct TYP_SINA_INF {
	EM_DIV_KIND		emRaise;									// 発生条件部 種別
	EM_DIV_KIND		emEvent;									// イベント部 種別
	int				nEventSub;									// イベント部 サブ種別

	BYTE*			pRaise;										// 発生条件部 構造体ポインタ
	BYTE*			pEvent;										// イベント部 構造体ポインタ

public:
	// デフォルトコンストラクタ
	TYP_SINA_INF() {	
		emRaise = DIV_KIND_NON;
		emEvent	= DIV_KIND_NON;
		nEventSub= 0;
		pRaise	= NULL;
		pEvent	= NULL;
	}
};

//// 種別特定
static EM_DIV_KIND GetTextToKind(CString str)
{
	//// トリム
	// スペース, tab
	str.Trim("	");	

	//// 条件比較
	if(		0 == strcmp(str, "PLG") )	{return DIV_KIND_PLG;}
	else if(0 == strcmp(str, "PI") )	{return DIV_KIND_PI;}
	else if(0 == strcmp(str, "PO") )	{return DIV_KIND_PO;}
	else if(0 == strcmp(str, "MEM") )	{return DIV_KIND_MEM;}
	else if(0 == strcmp(str, "MAIL") )	{return DIV_KIND_MAIL;}
	else if(0 == strcmp(str, "TIME") )	{return DIV_KIND_TIME;}
	else if(0 == strcmp(str, "ZZ") )	{return DIV_KIND_ZZ;}
	else								{return DIV_KIND_NON;}
};

//=======================================
// シナリオ（発生条件部）構造体
//=======================================
//// 距離駆動
struct TYP_RAISE_PLG_POS {
	int		nCh;												// 距離参照チャンネル (0オリジン) 0～7
	int		nLen;												// 事象発生距離 [mm]
};
//// イベント駆動
// メール
struct TYP_RAISE_EVT_MAIL {
	char	cTaskName[SIZE_NAME_TASK];							// タスク名称
	int		nEventNo;											// 対象イベントNo
};
// PO
struct TYP_RAISE_EVT_PO {
	int		nBood;												// ボードNo (0～7)
	int		nPort;												// ポートNo (0～7)
	int		nBit;												// ビットNo (0～7)
	int		nMode;												// 認識タイミング (true:立ち上がり、false:立ち下がり)
};
//// 時間駆動
struct TYP_RAISE_TIM_CYCLE {
	int		nID;												// 配列位置 (0オリジン)
	int		nFirst;												// 1回目開始時間 [ms]
	int		nCycle;												// 2回目以降の周期 [ms] (0なら１回のみ)
};

//=======================================
// シナリオ（イベント部）構造体
//=======================================
//// 速度変更
struct TYP_EVENT_PLG_SPEED {
	int		nCh;												// 距離参照チャンネル (0オリジン) 0～7:ライン速度  -1:フレーム周期
	int		nMpm;												// 変更ライン速度 [mpm]
};
//// フレーム出力周期変更
struct TYP_EVENT_PLG_FRAMECYCLE {
	int		nCycle;												// 変更周期 [ms]
};
//// BIT指定書き込み（ステータス出力）
struct TYP_EVENT_PI_BIT {
	int		nBood;												// ボードNo (0～7)
	int		nPort;												// ポートNo (0～7)
	int		nBit;												// ビットNo (0～7)
	int		nMode;												// セット (1:ON、0:OFF)
};
//// ポート指定書き込み（ステータス出力）
struct TYP_EVENT_PI_PORT{
	int		nBood;												// ボードNo (0～7)
	int		nPort;												// ポートNo (0～7)
	BYTE	nBitSet;											// セットするBitパターン(1:ON、0:OFF)
};
//// BIT指定書き込み（パルス出力）
struct TYP_EVENT_PI_BITPULS {
	int		nBood;												// ボードNo (0～7)
	int		nPort;												// ポートNo (0～7)
	int		nBit;												// ビットNo (0～7)
	int		nMode;												// セット (1:ON、0:OFF)
	int		nTime;												// ON時間 [ms]
};
//// 共有メモリ書き込み
struct TYP_EVENT_MEM_WRITE {
	char	cTblName[MAX_PATH];									// 書き込む共有メモリ名称
	int		nIndex;												// 書き込み開始位置 (0オリジン)
	char	cWriteDataPath[MAX_PATH];							// 書き込むバイナリデータのパス
};
//// メールスロット送信
struct TYP_EVENT_MAIL_SEND {
	char	cPcName[SIZE_NAME_PC];								// 送信対象コンピュータ名
	char	cTaskName[SIZE_NAME_TASK];							// 送信対象タスク名
	
	EM_DIV_MAIL	emMainKind;										// 構造体タイプ
	COMMON_QUE	que;											// 送信構造体
};
//// 時間駆動
struct TYP_EVENT_TIM_MODE {
	int		nIndex;												// 時間駆動クラスの配列位置 (0オリジン)
	int		nID;												// 配列位置 (0オリジン)
	int		nMode;												// 0:停止指示  1:開始指示
};
//// 内部通板距離リセット
struct TYP_EVENT_ZZ_RESET {
	int		nCh;												// 距離参照チャンネル (0オリジン) 0～7
	int		nMode;												// シナリオ終了区分 (0:何もしない  1:最初に戻る)
};

//=======================================
// メッセージ通知
//=======================================
//// 全てMainInctanceで受ける
//       場合によっては、MainInctanceが各ワーカースレッドにたれ流しする
enum ENUM_MAININCTANCE {
	E_DEF_ML_SMYU_START = WM_USER + 100,						// シミュレータ開始通知
	E_DEF_ML_SMYU_STOP,											// シミュレータ停止通知

	E_DEF_END													// 終了
};

