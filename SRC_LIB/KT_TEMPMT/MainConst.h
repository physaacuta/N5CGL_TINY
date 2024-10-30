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


//=======================================
// 管理定数
//=======================================
#define MAX_THREAD			8										// 最大スレッド管理数
#define MAX_TEMP_UNIT		16										// 温度ユニット最大数


//=======================================
// 通信状態
//=======================================
// 温度パネルメータ通信状態
enum EM_TEMP_MAINMODE
{
	MODE_NONE = 0,														// 初期状態
	MODE_OPEN_CHECK,													// 初期準備確認中
	MODE_WAIT,															// 待機

	MODE_READ_STATUS,													// コントローラステータス読出し中
//	MODE_READ_MONITOR,													// モニタ値読出し中

	MODE_RETRY															// オープンリトライ中
};

//=======================================
// 伝文
//=======================================
//// 伝文開始・終端文字
#define	STX_CODE					0x02								// STX
#define	ETX_CODE					0x03								// ETX

//// レスポンスフレーム
// 終了コード
#define EXT_SIZE					2								// 終了コードのサイズ
#define	EXT_OK						"00"							// 正常終了
#define	EXT_ERR_FINS				"0F"							// FINSコマンドエラー
#define EXT_ERR_PARITY				"10"							// パリティエラー
#define	EXT_ERR_FRAMING				"11"							// フレーミングエラー
#define	EXT_ERR_OVERRUN				"12"							// オーバーランエラー
#define	EXT_ERR_BCC					"13"							// BCCエラー
#define	EXT_ERR_FORMAT				"14"							// フォーマットエラー
#define	EXT_ERR_SUBADDRESS			"16"							// サブアドレスエラー
#define	EXT_ERR_FRAMELEN			"18"							// フレーム長エラー

//// レスポンスコード
#define	RES_SIZE					4								// レスポンスコードのサイズ
#define	RES_OK						"0000"							// 正常終了
#define	RES_ERR_CMDLEN_OVER			"1001"							// コマンド長オーバー
#define	RES_ERR_CMDLEN_SHORT		"1002"							// コマンド長不足
#define	RES_ERR_AREAKIND			"1101"							// エリア種別エラー
#define	RES_ERR_RESLEN_OVER			"110B"							// レスポンス長オーバー
#define	RES_ERR_PARAM				"1100"							// パラメータエラー
#define	RES_ERR_SYSTEM				"2203"							// 動作エラー

//// FINS-miniコマンドのサービス一覧
//// MCR + SCR
#define	FINS_READ_DATA				"0101"							// モニタ値/設定データ読出し
#define	FINS_WRITE_DATA				"0102"							// 設定データ書込み
#define	FINS_READ_MULTI_DATA		"0104"							// モニタ値/設定データ複合読出し
#define	FINS_WRITE_MULTI_DATA		"0113"							// 設定データ複合書込み
#define	FINS_ENTRY_READ_DATA		"0110"							// モニタ値/設定データ複合登録読出
#define	FINS_READ_ENTRY_WRITE_DATA	"0111"							// モニタ値/設定データ複合読出登録(書込)
#define	FINS_READ_ENTRY_CHECK_DATA	"0112"							// モニタ値/設定データ複合読出登録確認(読出)
#define	FINS_READ_ATTRIBUTE			"0503"							// 本体属性読出し
#define	FINS_READ_STATUS			"0601"							// コントローラステータス読出し
#define	FINS_ECHOBACK_TEST			"0801"							// エコーバックテスト
#define	FINS_COMMAND				"3005"							// 動作指令

//// ステータス
#define	MT_STATUS_OK				0x00							// 正常
#define	MT_STATUS_ERR_NO_MEASURE	0x01							// 未計測状態
#define	MT_STATUS_ERR_DISPLAY_OVER	0x02							// 表示範囲オーバー
#define	MT_STATUS_ERR_INPUT_A		0x04							// 入力異常A
#define	MT_STATUS_ERR_INPUT_B		0x08							// 入力異常B

//////////////////////////////
//// コマンド伝文
// エコーバックテスト
struct CMD_TYPE_ECHO_BACK
{
	char			cSTX[1];										// STX
	char			cNodeNo[2];										// ノードNo (ユニットNo)
	char			cSubAddr[2];									// サブアドレス ("00"固定)
	char			cServiceID[1];									// サービスID ("0"固定)

	// FINS-mini
	char			cMRC[2];										// MRC ("08")
	char			cSRC[2];										// SRC ("01")

	char			cETX[1];										// ETX
	char			cBCC[1];										// BCC (ノードNo～ETXのXOR)
};

// コントローラステータス読出し
struct CMD_TYPE_CONTROLER_STATUS
{
	char			cSTX[1];										// STX
	char			cNodeNo[2];										// ノードNo (ユニットNo)
	char			cSubAddr[2];									// サブアドレス ("00"固定)
	char			cServiceID[1];									// サービスID ("0"固定)

	// FINS-mini
	char			cMRC[2];										// MRC ("06")
	char			cSRC[2];										// SRC ("01")

	char			cETX[1];										// ETX
	char			cBCC[1];										// BCC (ノードNo～ETXのXOR)
};

// モニター値読出し
struct CMD_TYPE_MONITOR_READ
{
	char			cSTX[1];										// STX
	char			cNodeNo[2];										// ノードNo (ユニットNo)
	char			cSubAddr[2];									// サブアドレス ("00"固定)
	char			cServiceID[1];									// サービスID ("0"固定)

	// FINS-mini
	char			cMRC[2];										// MRC ("01")
	char			cSRC[2];										// SRC ("01")
	char			cKind[2];										// 変数種別
	char			cAddr[4];										// アドレス
	char			cBitPos[2];										// ビット位置
	char			cCount[4];										// 要素数

	char			cETX[1];										// ETX
	char			cBCC[1];										// BCC (ノードNo～ETXのXOR)
};


//////////////////////////////
//// レスポンス伝文
// エコーバックテスト
struct RES_TYPE_ECHO_BACK
{
	char			cSTX[1];										// STX
	char			cNodeNo[2];										// ノードNo (ユニットNo)
	char			cSubAddr[2];									// サブアドレス ("00"固定)
	char			cEXT[2];										// 終了コード

	// FINS-mini
	char			cMRC[2];										// MRC ("08")
	char			cSRC[2];										// SRC ("01")
	char			cResCode[4];									// レスポンスコード

	char			cETX[1];										// ETX
	char			cBCC[1];										// BCC
};

// コントローラステータス読出し
struct RES_TYPE_CONTROLER_STATUS
{
	char			cSTX[1];										// STX
	char			cNodeNo[2];										// ノードNo (ユニットNo)
	char			cSubAddr[2];									// サブアドレス ("00"固定)
	char			cEXT[2];										// 終了コード

	// FINS-mini
	char			cMRC[2];										// MRC ("06")
	char			cSRC[2];										// SRC ("01")
	char			cResCode[4];									// レスポンスコード
	char			cData1[2];										// 運転状態
	char			cData2[2];										// ステータス

	char			cETX[1];										// ETX
	char			cBCC[1];										// BCC
};

// モニター値読出し
struct RES_TYPE_MONITOR_READ
{
	char			cSTX[1];										// STX
	char			cNodeNo[2];										// ノードNo (ユニットNo)
	char			cSubAddr[2];									// サブアドレス ("00"固定)
	char			cEXT[2];										// 終了コード

	// FINS-mini
	char			cMRC[2];										// MRC ("01")
	char			cSRC[2];										// SRC ("01")
	char			cResCode[4];									// レスポンスコード
	char			cData[8];										// 

	char			cETX[1];										// ETX
	char			cBCC[1];										// BCC
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

