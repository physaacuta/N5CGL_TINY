// *********************************************************************************
//	外部インターフェース用ヘッダー
//	[Ver]
//		Ver.01    2015/06/18  初版作成
//
//	[メモ]
//		基本的に、書き換え可
//
// *********************************************************************************

// 個々のシステム固有のインクルード

#ifndef	_OUTINTERFACE_H
#define	_OUTINTERFACE_H

// φωインクルード
#include "KizuCommon.h"								// φω共通ヘッダー

//***************************************************************
// 設定可変定数 定義
//***************************************************************

//***************************************************************
// プロコン通信　定義
//***************************************************************
//=======================================
// プロコン 定数
//=======================================
//#define PRO_SIZE_TC					4					// トランザクションコード MID
#define PRO_MAX_CUT_COUNT			16					// 最大カット回数
#define PRO_SIZE_COIL_NO			12					// コイルNo (入側、出側同じ)
#define PRO_SIZE_ORDER_NO			4					// 命令No

#define PRO_SIZE_NEXTINF_CNT		4					// 次コイル情報件数

// トランザクションコード
#define	PRO_TC_COIL					1000				// コイル情報伝文
#define	PRO_TC_SAISUN				1001				// 採寸実績伝文
#define	PRO_TC_CUT					1002				// カット実績伝文
#define	PRO_TC_INIT					1003				// イニシャル伝文
#define	PRO_TC_ZISEKI				2000				// 疵検査実績伝文
//#define	PRO_TC_HELS					3000				// ヘルシー伝文
#define	PRO_TC_HELS					-1					// ヘルシー伝文

// 処理区分
#define	PRO_KUBUN_NOMAL				0					// 業務伝文
#define PRO_KUBUN_ANSER				1					// アンサー伝文

// グラントフラグ
#define PRO_GRAND_OK				0					// 伝文の中身 正常
#define PRO_GRAND_NG				1					// 伝文の中身 異常

// プロコン伝文サブヘッダー [0]
static const BYTE	PRO_HEAD_H_GYOUMU	= 0x60;			// 業務伝文 又は 応答伝文

// プロコン伝文サブヘッダー [1]
static const BYTE	PRO_HEAD_L_OK		= 0x00;			// 正常

// TCからの長さ		※ メッセージNoからの長さ
// ワード単位 (ヘッダー部含まない) ※Csemi対応
#define	PRO_TYPE_LENGTH(size)			((size-4)/2)		// 伝文のデータ長にセットする値
#define	PRO_TYPE_LENGTH_ORIG(size)		(size*2+4)			// 伝文のデータ長から本来のサイズに変換
// バイト単位 (ヘッダー部含む)
//#define	PRO_TYPE_LENGTH(size)			(size)			// 伝文のデータ長にセットする値
//#define	PRO_TYPE_LENGTH_ORIG(size)		(size)			// 伝文のデータ長から本来のサイズに変換

//=======================================
// プロコン 構造体
//=======================================

#pragma pack(push, 2)

//// レスポンス伝文 (2byte)
struct PRO_TYPE_RESP
{
	BYTE			bHead[2];							// サブヘッダー
};

//// ヘッダー部 (8byte)
struct PRO_TYPE_HEAD
{
	BYTE			bHead[2];							// サブヘッダー (ホントはここに入れたくなかったけど longが入っているのでバウンダリがズレル)

	short			nLen;								// データ長 (byte単位)
	long			nTC;								// メッセージNo
};

//// データ部

//// 共通情報 (56byte)
struct PRO_TYPE_COMMON
{
	short			nKubun;								// 伝文区分 (0:業務伝文 1:応答伝文)
	short			nDate[6];							// 送信時刻 (年月日時分秒)
	short			nGrandFlg;							// グラントフラグ (0:伝文の中身 正常 1:伝文の中身 異常)
	char			cYobi[40];
};

struct PRO_TYPE_KEY
{
	char			cMeiNo[PRO_SIZE_ORDER_NO];			// 命令No
	char			cCoilNo[PRO_SIZE_COIL_NO];			// キー情報 コイルNo

};

// イニシャル伝文
struct PRO_TYPE_DATA_INIT
{
	PRO_TYPE_COMMON		Common;							// 共通情報

	char				cYobi[64];						// 予備
};

// コイル情報伝文
struct PRO_TYPE_COIL
{
	PRO_TYPE_KEY		KeyInf;							// キー情報
	long				nSendPoint;						// 送信位置
	long				nLenIn;							// 入側論理コイル 長さ [m]
	long				nAtuZai;						// 材料板厚 [μm] (入側)
	long				nWidZai;						// 材料板幅 [mm]
	long				nAtuSei;						// 注文板厚 [μm] (出側)
	long				nWidSei;						// 注文板幅 [mm]
	long				nWidTrim;						// 現工程トリム幅 [mm] (ノートリム時は、0)
	long				nNobiSPM;						// SPM伸び率 [10-2%]
	long				nNobiTLV;						// TLV伸び率 [10-2%]
	long				nCutF;							// 入側におけるフロント端板カットの実績長[mm]
	long				nCutT;							// 入側におけるテール端板カットの実績長[mm]
	long				nSPMRoolKei_Up;					// SPMワークロール径 (上) [10-1mm]
	long				nSPMRoolKei_Down;				// SPMワークロール径 (下) [10-1mm]
	long				nYobi[31];

	char				cKenKijun_O[4];					// 検査基準 表
	char				cKenKijun_U[4];					// 検査基準 裏
	char				cKurometo_Kind[2];				// クロメート 実績種類
	char				cKurometo_Umu[2];				// クロメート 実績有無
	char				cHinCode[1];					// 品種
	char				cMekkiKind[1];					// メッキ種類
	char				cSyuZai[2];						// 主材質
	char				cDifMaker[1];					// ディファレンシャルマーカー
	char				cKenban[1];						// 原板区分
	char				cMekkiCycle[1];					// メッキサイクル
	char				cInLineSPM[1];					// レベラー インラインSPM
	char				cKagakuToyu[1];					// 化学処理塗油
	char				cAtukkaSPM[1];					// オフラインSPM圧下率
	char				cMekkiKubun[1];					// メッキ区分
	char				cOmoteSyori1[1];				// 表面処理1桁目
	char				cOmoteSyori2[1];				// 表面処理2桁目
	char				cToyu[1];						// 塗油
	char				cKouteiYobi[1];					// 工程記号予備
	char				cBaund[1];						// バウンダリ合わせ
	char				cCaplSyuZai[10];				// CAPL 主材質
	char				cMetuke[8];						// 目付規格
	char				cNi[2];							// Ni付着量
	char				cSodoTCM[2];					// TCMロール粗度
	char				cSpmUmu[2];						// SPM圧下有無 (有り:O  無し:X)
	char				cNextLine[2];					// 計画次工程コード
	char				cMuke[32];						// 向先
	char				cYouto[4];						// 用途
	char				cCaplSiage[4];					// CAPL表面仕上げ
	char				cCaplSanUmu[2];					// CAPL酸洗有無
	char				cHinMode[1];					// 品種区分
	
	char				cGaCrKubun[1];					// GA/CR区分
	char				cSpmJisekiUmu[2];				// SPM圧下実績有無（有り：0 無し:X）
	char				cKouteiCode[2];					// 工程コード
	char				cYobi1[42];
};

// 検査用コイル情報伝文
struct PRO_TYPE_DATA_COILINF
{
	PRO_TYPE_COMMON		Common;							// 共通情報

	PRO_TYPE_COIL		CoilInf;						// コイル情報

	// 次コイル以降の情報
	struct NEXT_COIL {
		PRO_TYPE_KEY	KeyInf;							// キー情報
		
		long			nFrontLen;						// 入側フロント端板長さ [mm]
		char			cKouteiCode[2];					// 自工程コード
		char			cYobi[2];

	} next_coil[PRO_SIZE_NEXTINF_CNT];
	char				cYobi[16];						// 予備
};

// 表示用コイル情報伝文
typedef PRO_TYPE_DATA_COILINF PRO_TYPE_DATA_COILDSP;

// 採寸実績伝文
struct PRO_TYPE_DATA_SAISUN
{
	PRO_TYPE_COMMON		Common;							// 共通情報

	PRO_TYPE_KEY		KeyInf;							// キー情報

	long			nLen;								// 実績コイル長[0.1m]
	long			nMakiLenF;							// フロント巻き込まれ長さ[0.1m]
	long			nSetCnt;							// セットした採寸個数
	long			nYobi[9];

	struct _SAISUN_INF{
		long		nStart;								// 採寸開始位置[0.1m]
		long		nEnd;								// 採寸終了位置[0.1m]
		long		nYobi[2];
	} saisun[PRO_MAX_CUT_COUNT];
	char			cYobi[128];							// 予備
};

// カット実績
struct PRO_TYPE_DATA_CUT
{
	PRO_TYPE_COMMON		Common;							// 共通情報

	PRO_TYPE_KEY		KeyInf;							// キー情報
	
	long			nMode;								// 実績区分 (0:溶接点通過 1:Fカット)
	long			nStart;								// 採寸開始位置[0.1m]
	long			nEnd;								// 採寸終了位置[0.1m]
	long			nKoban;								// 小番 (1CAPLのみ)
	char			cYobi[32];
};

// 検査実績
struct PRO_TYPE_DATA_ZISEKI
{
	PRO_TYPE_COMMON		Common;							// 共通情報

	long			nZiMode;							// 実績区分 (0:正常 1:異常(カット実績で受信した該当コイル無し) )
	long			nKikiMode;							// 収集区分 (0:正常 1:異常)

	PRO_TYPE_KEY	KeyInf;								// キー情報
	long			nKoban;								// 小番 (1CAPLのみ カット実績の小番をそのままセットする)
	char			cYobi1[36];

	// グレード 1～6毎の疵混入率情報
	struct GRADE_INF {
		short		nMixedRate;							// 混入率 [10-2%]
		short		nYobi1[2];
		short		nTypeCnt[10];						// 疵個数 (疵種1～10)
		short		nYobi2[3];
	} grade_inf[6];

	char			cYobi[192];
};

//// ヘルシー伝文
//struct PRO_TYPE_DATA_HELS
//{
//	char			cYobi[64];
//};

//=======================================
// プロコン 伝文完成版
//=======================================
// イニシャル伝文
struct PRO_TYPE_INIT
{
	PRO_TYPE_HEAD			head;
	PRO_TYPE_DATA_INIT		data;
};
// コイル情報伝文
struct PRO_TYPE_COILINF
{
	PRO_TYPE_HEAD			head;
	PRO_TYPE_DATA_COILINF	data;
};
// 採寸実績伝文
struct PRO_TYPE_SAISUN
{
	PRO_TYPE_HEAD			head;
	PRO_TYPE_DATA_SAISUN	data;
};
// カット実績伝文
struct PRO_TYPE_CUT
{
	PRO_TYPE_HEAD			head;
	PRO_TYPE_DATA_CUT		data;
};
// 疵検査実績伝文
struct PRO_TYPE_ZISEKI
{
	PRO_TYPE_HEAD			head;
	PRO_TYPE_DATA_ZISEKI	data;
};
// ヘルシー伝文
struct PRO_TYPE_HEALTHY{
	PRO_TYPE_HEAD			head;
};

#pragma pack(pop)

//***************************************************************
// FF通信　定義
//***************************************************************
//=======================================
// FF通信 定数
//=======================================
#define FFSERV_OFFLINE		// 本番では、コメントにすること(FFサーバー試験用)

#ifndef FFSERV_OFFLINE			// ---------------本番系---------------

//// FF伝管コマンド
#define FF_DENKAN_NO			"TE"					// 

//// トランザクションコード

// 検査実績伝文の伝文コード　※停止
#define FF_TC_CODE_JI_O			"85"					// 表
#define FF_TC_CODE_JI_U			"86"					// 裏

// 応答伝文の伝文コード　※停止
#define FF_TC_CODE_ANS_O		"8G"					// 表
#define FF_TC_CODE_ANS_U		"8H"					// 裏

// 検査実績伝文の伝文コード　※停止
#define FF_TC_CODE_STSJI_O		"95"					// 表
#define FF_TC_CODE_STSJI_U		"96"					// 裏

// 応答伝文の伝文コード　※停止
#define FF_TC_CODE_STSANS_O		"9G"					// 表
#define FF_TC_CODE_STSANS_U		"9H"					// 裏

// 前工程疵情報要求伝文の伝文コード
#define FF_TC_CODE_MAEREQ		"A4"					// 

// 前工程疵情報の伝文コード
#define FF_TC_CODE_MAEJI_O		"C4"					// 表
#define FF_TC_CODE_MAEJI_U		"D4"					// 裏

// ヘルシーの伝文コード
#define FF_TC_CODE_HELCY		"ZZ"					// ヘルシー伝文

#else							// ---------------試験系---------------

//// FF伝管コマンド
#define FF_DENKAN_NO			"TT"					// 


//// トランザクションコード

// 検査実績伝文の伝文コード
#define FF_TC_CODE_JI_O			"TU"					// 表
#define FF_TC_CODE_JI_U			"TV"					// 裏

// 応答伝文の伝文コード
#define FF_TC_CODE_ANS_O		"TW"					// 表
#define FF_TC_CODE_ANS_U		"TX"					// 裏

// 検査状態伝文の伝文コード
#define FF_TC_CODE_STSJI_O		"TM"					// 表
#define FF_TC_CODE_STSJI_U		"TN"					// 裏

// 検査状態応答伝文の伝文コード
#define FF_TC_CODE_STSANS_O		"TP"					// 表
#define FF_TC_CODE_STSANS_U		"TQ"					// 裏

// 前工程疵情報要求伝文の伝文コード
#define FF_TC_CODE_MAEREQ		"TR"					// 

// 前工程疵情報の伝文コード
#define FF_TC_CODE_MAEJI_O		"C4"					// 表
#define FF_TC_CODE_MAEJI_U		"D4"					// 裏

// ヘルシーの伝文コード
#define FF_TC_CODE_HELCY		"ZZ"					// ヘルシー伝文

#endif

//=======================================
// FF 通信系ステータス　定数定義
//=======================================

// FF応答伝文 処理結果　定数
#define FF_ANS_OK						"00"			// 正常
#define FF_ANS_IPCHK_NG					"01"			// インプットチェックNG
#define FF_ANS_DBERRER					"03"			// DB登録異常

//=======================================
// FF通信系　固定サイズ
//=======================================
#define MAX_FF_DFCT_CNT					1800			// ＦＦ検査実績伝文疵情報
#define MAX_FF_STS_CNT					30000			// ＦＦ検査状態伝文疵情報

#define MAX_FF_MAE_DINF_CNT				7				// ＦＦ前工程疵実績伝文 疵検出情報

#define MAX_FF_MAE_DEF_CNT				2000			// ＦＦ前工程疵実績伝文 (全部)
//#define MAX_FF_MAE_CC_CNT				10				// ＦＦ前工程疵実績伝文 ＣＣ疵情報
//#define MAX_FF_MAE_HOT_CNT				50				// ＦＦ前工程疵実績伝文 ＨＯＴ疵情報
//#define MAX_FF_MAE_PL_CNT				80				// ＦＦ前工程疵実績伝文 ＰＬ疵情報
//#define MAX_FF_MAE_TCM_ANA_CNT			30				// ＦＦ前工程疵実績伝文 ＴＣＭ穴情報
//#define MAX_FF_MAE_YOBI_CNT				30				// ＦＦ前工程疵実績伝文 疵情報予備
//#define MAX_FF_MAE_MAE_CNT				1800			// ＦＦ前工程疵実績伝文 直近疵情報
// 20221021 2TCM実機化に伴う下工程改造 小島 --->>>
#define	MAX_FF_MAE_N2TCM_KIZU_CNT		900				// ＦＦ前工程疵実績伝文 ＴＣＭ情報（N2TCM実機）
// 20221021 2TCM実機化に伴う下工程改造 小島 ---<<<

#define MAX_FF_MAE_THAN_CNT				10				// ＦＦ前工程疵実績伝文 通板実績情報
#define MAX_FF_MAE_BNKT_CNT				20				// ＦＦ前工程疵実績伝文 分割情報

//=======================================
// 構造体（対FFサーバー伝送）
//=======================================
//// 検査実績伝文
struct FF_TYPE_HEAD {
	char			Header_Code[2];						// 伝文コード 検査実績[83/84] 検査状態[95/96]
	char			Rec_Code[2];						// 連番(00)
	char			Date[14];							// 伝文発生時刻 形式 YYYYMMDHHNNSS
	char			Line_Code[2];						// 工程コード [5CGL: 85]
	char			Meino[4];							// 命令番号
	char			Coilno[10];							// 冷延コイルNo
};

struct FF_TYPE_DATA {
	char			Kizukbn[1];							// 疵検出区分 0:φω
	char			Kind[2];							// 疵種
	char			Grade[2];							// 疵グレード
	char			WidPos[4];							// 疵巾方向位置
	char			Len[4];								// 疵長さ
	char			DefectPos[6];						// 疵総長さ方向位置
	char			kbn[1];								// 疵区分
	char			No[4];								// 疵NO (疵以外は、4桁0設定)
	char			OpeDsp[1];							// オペガイ表示フラグ
	char			SysKbn[1];							// システム区分
	char			Filler[6];							// 予備
};

struct FF_JISSEKI {										// 本体
	FF_TYPE_HEAD	Hd;									// 伝文ヘッダー
	char			CoilLen[6];							// コイル長さ [10-1m]
	char			Mode[2];							// 検査処理モード (0:両面縮退、1:両面正常、2:裏面縮退、3:表面縮退、4:疵検停止、5:エッジ未検出区間有り)
	char			OverStatus[2];						// 疵個数オーバーステータス (2:1800件ｵｰﾊﾞｰ時、1:1800件以内時)
	char			Id[20];								// 管理NO
	char			DataDefault[4];						// データ設定数
	char			cYobi1[32];							// 予備
	FF_TYPE_DATA	Dt[MAX_FF_DFCT_CNT];				// 疵情報		1800件
	FF_TYPE_DATA	Dtyobi[2000-MAX_FF_DFCT_CNT];		// 予備疵情報 200件
	char			cYobi[900];							// 予備
};

//// 応答伝文
struct FF_ANS_HD {
	char			Header_Code[2];						// 伝文コード [M7/M8]
	char			Rec_Code[2];						// 連番(00)
	char			Date[14];							// 伝文発生時刻 形式 YYYYMMDHHNNSS
	char			Line_Code[2];						// 工程コード [CTL:A1]
	char			Meino[4];							// 命令番号
	char			Coilno[10];							// 入側コイル番号
};

struct FF_ANS_DATA {
	char			Status[2];							// 処理結果 (00:OK, 01:INPUT CHACK NG 03:DB登録異常)
};

struct FF_ANS {
	FF_ANS_HD		Hd;									// 伝文ヘッダー
	FF_ANS_DATA		Data;								// データ
	char			cYobi[64964];						// 予備
};

//// 前工程疵実績要求伝文
struct FF_MAE_REQUEST {
	char			HeaderCode[2];						// 伝文コード
	char			Rec_Code[2];						// 連番 ("00"固定)
	char			DateTime[14];						// 伝文発生時刻 形式 YYYYMMDDHHNNSS
	char			Line_Code[2];						// 工程コード  (2PL=12 3PL=13 4PL=14)
	char			MeireiNo[4];						// 命令番号４桁('０'＋命令番号３桁)を右詰設定
	char			Filler[64976];						// 予備 空白(0x20)
};

// 前工程疵実績伝文
// 伝文ヘッダー
struct FF_MAE_TYPE_HEAD {
	char			Header_Code[2];						// 伝文コード (??:表, ??:裏)
	char			Rec_Code[2];						// 連番 ("00"固定)
	char			DateTime[14];						// 伝文発生時刻 形式 YYYYMMDDHHNNSS
	char			Line_Code[2];						// 工程コード  (5CGL=85)
	char			Meino[4];							// 命令番号
	char			O_UKbn[2];							// 表裏区分(1:表, 2:裏)
	char			ReiCoilNo[10];						// 冷延コイル番号
	char			ReiCoilG[5];						// 冷延コイル重量 [10-3t]
	char			ReiCoilL[6];						// 冷延コイル長さ [10-1m]
	char			ReiCoilAtu[5];						// 冷延厚 [10-3mm]
	char			ItaatuMin[5];						// 板厚　公差　MIN [10-3mm]
	char			ItaatuMax[5];						// 板厚　公差　MAX [10-3mm]

	char			Mukesaki1i[5];						// 向け先１　板巾 [10-1mm]
	char			Kensa[4];							// 検査基準
	char			Maki[1];							// 巻き方向
	char			Zairyou[5];							// 材料巾 [10-1mm]
	char			Mukesaki1kMin[5];					// 向け先１　板巾公差　MIN [10-1mm]
	char			Mukesaki1kMax[5];					// 向け先１　板巾公差　MAX [10-1mm]
	char			Mukesaki1L[5];						// 向け先１　長さ(ロッド枚数) [m]
	char			Mukesaki1N[3];						// 向け先１　内径 [10-1ｲﾝﾁ]
	char			Itaatu[5];							// 板厚 [10-3mm]
	char			Irigawa[3];							// 入側内径 [10-1ｲﾝﾁ]
	char			Mukesaki1KL[5];						// 向け先１　剪断長 [10-1mm]
	char			Makoutei[2];						// 前工程コード
	char			RealMakoutei[2];					// 実前工程コード
	char			TCMFlag[1];							// 厚手/TCM直送材フラグ
};

struct FF_MAE_TYPE_INF {
	char			Kizukbn[2];							// 疵検出工程
	char			Id[20];								// 管理NO
	char			Mode[2];							// 検査処理モード
	char			DataDefault[4];						// 疵設定個数
	char			OverStatus[2];						// 疵個数オーバーステータス
};

struct FF_MAE_TYPE_VRCLINF {
	char			KizukenNo[20];						// VRCL管理NO
};

struct FF_MAE_TYPE_FROMTO {
	char			CoilNo[10];							// 冷延コイルNo
	char			From[6];							// 採取開始位置長さ（From）	[0.1m]
	char			To[6];								// 採取終了位置長さ（To）	[0.1m]
	char			Koban[2];							// 採取子番
};

struct FF_MAE_TYPE_DATA {
	char			Kind[2];							// 疵種
	char			Grade[2];							// 疵グレード
	char			Kizukbn[2];							// 疵検出工程
	char			kbn[1];								// 疵区分
	char			DefectPos[6];						// 疵長さ方向位置 [10-1m]
	char			WidPos[4];							// 疵巾方向位置 [mm]
	char			Len[4];								// 疵長さ [10-1m]
	char			No[4];								// 疵NO
	char			SysKbn[1];							// システム区分
	char			Filler[4];							// 予備
};

struct FF_MAE_TYPE_BUNKATSU {
	char			MakiHoukou[1];						// 実績巻方向 (1:上出上捲, 2:上出下捲, 3:下出上捲, 4:下出下捲)
	char			CoilLen[6];							// コイル長
	char			Cutkbn[1];							// カット場所区分 (I:入側カット, O:出側カット)
	char			GenpinNo[10];						// 分割現品管理NO
	char			Filler[2];							// 予備
};

struct FF_MAE_TYPE_THUHAN {
	char					JissekiCode[2];				// 実績工程コード
	char					MeireiNo[4];				// 命令番号
	char					ThuhanCnt[1];				// 通板回数
	char					GenpinNo[10];				// 入側現品管理NO
	char					CoilLen[6];					// 入側コイル長さ
	char					Yobi[2];					// 予備
	FF_MAE_TYPE_BUNKATSU	Bu[MAX_FF_MAE_BNKT_CNT];	// 分割情報
};


// 前工程疵情報伝文本体
struct FF_MAE_JISSEKI {
	FF_MAE_TYPE_HEAD	Hd;								// ヘッダー部
	char				Yobi1[27];						// 予備
	FF_MAE_TYPE_INF		Inf[MAX_FF_MAE_DINF_CNT];		// 疵検出情報
	char				Yobi2[90];						// 予備
	FF_MAE_TYPE_VRCLINF	VRcl;							// VRCL情報
	FF_MAE_TYPE_FROMTO	FromTo;							// From-To
	char				Yobi3[116];						// 予備

	FF_MAE_TYPE_DATA	def[MAX_FF_MAE_DEF_CNT];		// 疵情報 (ヘッダーの疵検出工程 と 個々の疵の疵検出工程 を 全部見比べて紐付ける)

	FF_MAE_TYPE_THUHAN	Th[MAX_FF_MAE_THAN_CNT];		// 通板実績情報

	char				Yobi[150];						// 予備
};

// 疵検出工程 ヘッダー部 配列INDEX
enum EM_DIV_PROC_KIND_INDEX {
	DIV_PROC_KIND_INDEX_CC = 0,							// [前工程情報配列INDEX] CC
	DIV_PROC_KIND_INDEX_HOT,							// [前工程情報配列INDEX] HOT
	DIV_PROC_KIND_INDEX_PL,								// [前工程情報配列INDEX] PL
	DIV_PROC_KIND_INDEX_TCM_ANA,						// [前工程情報配列INDEX] TCM穴検

	DIV_PROC_KIND_INDEX_CGL,							// [前工程情報配列INDEX] 厚手系疵検
	// 20221021 2TCM実機化に伴う下工程改造 小島 --->>>
	//DIV_PROC_KIND_INDEX_FLL2,							// [前工程情報配列INDEX] FLLラミ後疵検
	DIV_PROC_KIND_INDEX_TCM_KIZU,						// [前工程情報配列INDEX] TCM穴検
	// 20221021 2TCM実機化に伴う下工程改造 小島 ---<<<
	DIV_PROC_KIND_INDEX_FLL3							// [前工程情報配列INDEX] FLL出側疵検
};


// 検査状態伝文
struct FF_STSTYPE_HEAD
{
	char			Header_Code[2];						// 伝文コード 表:95 裏:96
	char			Rec_Code[2];						// 連番(00)
	char			Date[14];							// 伝文発生時刻 形式 YYYYMMDHHNNSS
	char			Line_Code[2];						// 疵検ラインの工程コード['81','85','54','82(53)']設定
	char			Meino[4];							// 命令番号
	char			Coilno[10];							// 冷延コイル番号
};

struct FF_KENSA_STATUS {
	char			status[1];							// 検査状態(1m間の検査状態を1byteに格納(上限30000m))
};

struct FF_STSTYPE_DATA {
	char			Id[20];								// 疵検管理No
	char			dataSize[5];						// 検査状態セット数：検査状態を格納したbyte数数を右詰設定(０埋め)
	char			yobi1[2];							// 予備１：検査状態情報の検査工程コードを設定
	char			yobi2[1];							// 予備２：FF側の判断した状態をセット("0":正常 / "1":疵検から該当情報：無)
	char			yobi[38];							// 予備
	FF_KENSA_STATUS data[MAX_FF_STS_CNT];				// 検査状態(1m間の検査状態を1byteに格納(上限30000m))
};

struct FF_STS_JISSEKI {									// 本体
	FF_STSTYPE_HEAD		Hd;								// 伝文ヘッダー
	FF_STSTYPE_DATA		Dt;								// データ
	char				Filler2[34900];					// 予備
};

enum { FF_STSREC_SIZE = sizeof(FF_STS_JISSEKI) };		// 検査状態伝文 伝文長

// 応答伝文
struct FF_STSANS_HD {
	char			Header_Code[2];						// 伝文コード 表:9G 裏:9H
	char			Rec_Code[2];						// 連番(00)
	char			Date[14];							// 伝文発生時刻 形式 YYYYMMDHHNNSS
	char			Line_Code[2];						// 疵検ラインの工程コード['81','85','54','82(53)']設定
	char			Meino[4];							// 命令番号
	char			Coilno[10];							// 入側コイル番号
};

struct FF_STSANS_DATA {
	char			Status[2];							// 処理結果 00:OK, 01:INPUT CHACK NG 03:DB登録異常
};

struct FF_STSANS {
	FF_STSANS_HD	Hd;									// 伝文ヘッダー
	FF_STSANS_DATA	Data;								// データ
	char			Filler[64964];						// 予備
};
enum { FF_STSANS_REC_SIZE = sizeof(FF_STSANS) };		// 検査状態　応答伝文　伝文長


//// ヘルシー伝文＆ヘルシー応答伝文
struct FF_HELCY {
	char			Header_Code[2];						// 伝文コード (ZZ)
	char			Rec_Code[2];						// 連番 (FF)
	char			DateTime[14];						// 伝文発生時刻 形式 YYYYMMDDHHNNSS
	char			Line_Code[2];						// 工程コード [A1]
	char			Denkan_No[2];						// 伝管コマンドNo [AA]
	char			Yobi[64978];						// 予備
};

//#endif


//***************************************************************
// シーケンサ通信　定義
//**************************************************************
// SEQ 標準 (UDP)
#define SEQ_SIZE_HEAD_BASE			4					// シーケンサヘッダーサイズ	
#define SEQ_SIZE_HEAD				8					// シーケンサヘッダーサイズ	

// SEQヘッダー用
//	標準：ワード単位でメッセージ部からの長さ
#define	SEQ_TYPE_LENGTH(size)		((size-4)/2)		// 伝文のデータ長にセットする値			(本来→伝文長)
#define	SEQ_TYPE_LENGTH_ORIG(size)	(size*2+4)			// 伝文のデータ長から本来のサイズに変換 (伝文長→本来)

//   CSEMIのように使う場合。ワード単位でデータ部からの長さ
//#define	SEQ_SEMI_TYPE_LENGTH(size)		(((size-4)/2)-2)	// 伝文のデータ長にセットする値			(本来→伝文長)
//#define	SEQ_SEMI_TYPE_LENGTH_ORIG(size)	((size*2+4))		// 伝文のデータ長から本来のサイズに変換	(伝文長→本来)



// TC
#define	SEQ_TC_HELCY				-1					// ヘルシー伝文
#define	SEQ_TC_FRAME				1					// フレーム情報伝文

// その他
#define SEQ_UDP_PORT				51000				// UDP受信ポート


#pragma pack(push, 1)

//// レスポンス伝文 (2byte)
struct SEQ_TYPE_RESP
{
	BYTE			bHead[2];						// サブヘッダー
};

//// 本来のシーケンサヘッダー(4byte)
struct SEQ_TYPE_HEAD_BASE : public SEQ_TYPE_RESP
{
	short			nSeqLen;						// データ長 (ワード単位)
};

//// 今回のシーケンサヘッダー(8byte)	※使いやすいようにCSEMIヘッダーと合わせる
struct SEQ_TYPE_HEAD : public SEQ_TYPE_HEAD_BASE
{
	int				nTC;							// トランザクションコード
};

#pragma pack(pop)


//=======================================
// シーケンサ 構造体
#pragma pack(push, 1)
// フレーム情報
struct SEQ_TYPE_DATA_FRAME {
	DWORD					nFrameNo;				// フレームNo 検査開始してからの連番 (0オリジン) ※初回フレームは0
	DWORD					nPlgDiff;				// PLG差分 前フレームからの差分 [pls]
	DWORD					nTimeDiff;				// 時間差分 [10us]

	DWORD					nExSyncAdd;				// ExSync積算値 [pls]
	DWORD					nPlgAdd;				// PLG積算値 [pls]
	DWORD					nTimeAdd;				// 時間積算値 [10us]

	int						nWpdFramePos;			// WPDオン時のフレーム内ExSync位置 (-1:無し  0～:検査用WPD有りフレーム)
	BYTE					yobi[28];
};

//=======================================
// 伝文完成版

// フレーム情報
struct SEQ_TYPE_FRAME
{
	SEQ_TYPE_HEAD			head;
	SEQ_TYPE_DATA_FRAME		data;
};
#pragma pack(pop)

// シーケンサデータメモリ

// 1秒パルス出力名称   ※EM_EVENT_PLS_OUT でアクセス
static const char* pls_out_name[] = {
	{ "初期化指示" },	//0
	{ "開始指示" },	//1
	{ "" },			//2
	{ "" },			//3
	{ "" },			//4
	{ "" },			//5
	{ "疑似WPD発生指示" }	//6
};


#pragma pack(push, 2)
//=========================================
// パルス出力 タイミング
enum EM_EVENT_PLS_OUT {
	EVENT_PLS_OUT_INIT = 0,										// 初期化指示
	EVENT_PLS_OUT_START = 1,										// 開始指示

	EVENT_PLS_OUT_TEST_WPD = 6,									// WPD信号デバック用
	EVENT_PLS_OUT_END = 7
};


//=========================================
// 状態表示用  ※データメモリ出力にも使用
struct TYP_INF_KEN
{
	char			cKeyNo[20];					// KeyNo
	int				nPlsR;										// パルスレート [10-6]
	int				nLen;										// 通板距離 [10-1m]
	int				nSpeed;										// 速度 [10-1mpm]
	WORD			nYobi[4];
};
struct TYP_INF_DSP
{
	char			cKeyNo[20];					// KeyNo
	int				nPlsR;										// パルスレート [10-6]
	int				nLen;										// 通板距離 [10-1m]
	int				nSpeed;										// 速度 [10-1mpm]
	WORD			nYobi[4];
};




//***************************************************************
// 制御シーケンサ通信(MxComponent通信)　定義
//***************************************************************
//=======================================
// 制御シーケンサ通信(MxComponent通信) 定数
//=======================================
static const int HB_MAX_VAL = 9999;								// ライブカウンタ最大値

																// ＜対プロコンI/F＞ 受信伝文種類 (※ データメモリ配置順)
//=========================================
// データメモリ構造体
// 受信構造体	※MAX256byte
struct SEQ_DM_RECV_DEFECT {
	// 基本
	WORD				nHBCnt;										// シーケンサ ヘルシー	5s変化無しでシーケンサ異常
	WORD				nSeqSt;										// シーケンサ 状態 (0:正常 1:注意 2:異常)
	WORD				nSeqKadou;									// シーケンサ 稼動状態（0:リセット済み 1:初期化済み 2:予備 3:撮像開始）
	WORD				nYobi1[5];

	WORD				nPls;										// 検査用PLG パルスレート
	WORD				nPlsSub;									// 検査比較用PLG パルスレート
	WORD				nPlsDsp;									// 表示用PLG パルスレート
	WORD				nYobi2[5];

	DWORD				nLength;									// 検査用PLG 測長距離
	DWORD				nLengthSub;									// 検査比較用PLG 測長距離
	DWORD				nLengthDsp;									// 表示用PLG 測長距離
	DWORD				nWpdOnLength;								// WPD信号ON時の検査用PLG 測長距離
	DWORD				nWpdOnLengthSub;							// WPD信号ON時の検査比較用PLG 測長距離
	DWORD				nWpdOnLengthDsp;							// WPD信号ON時の表示用PLG 測長距離
	WORD				nYobi3[4];
};
struct SEQ_DM_RECV_BASE : public SEQ_DM_RECV_DEFECT
{

	// 検査用
	struct TYP_KEN {
		DWORD			nFrameNo;									// フレームNo
		DWORD			nPlgDiff;									// PLG差分
		DWORD			nTimeDiff;									// 時間差分 [10us]
		DWORD			nLvalAdd;									// ExSync(=LVAL)積算値
		DWORD			nPlgAdd;									// PLG積算値
		DWORD			nTimeAdd;									// 時間積算値 [10us]

		// 前回値保持
		DWORD			nWpdOnFrameNo;								// WPDオン時のフレームNo
		DWORD			nWpdOnLvalAdd;								// WPDオン時のExSync積算値
		DWORD			nWpdOnYCnt;									// WPDオン時のフレーム内ExSync位置 （0～縦フレームサイズ-1）
	} ken;
	DWORD				nYobi4[3];

	// 表示用
	struct TYP_DSP {
		DWORD			nPlgAdd;									// PLG積算値
																	// 前回値保持
		DWORD			nWpdOnPlgAdd;								// WPDオン時のPLG積算値
	} dsp;
	DWORD				nYobi5[2];

	WORD				nGateKen;									// 検査用ゲート信号
	WORD				nGateDsp;									// 表示用ゲート信号
	// パルス  (0:OFF  1:ON)
	struct TYP_PLS {
		WORD			nWpd[3];									// WPD信号[0:#4検査用, 1:#5, 2:#6表示用]
		WORD			nSharCutF;									// 出側ファーストカット信号
		WORD			nSharCutL;									// 出側ラストカット信号
	} pls;
	WORD				nSpm;										// SPM開放信号
	WORD				nYobi6[8];

	WORD				nChilerErr[3];								// チラー異常信号[0:表面,1:裏面,2:BK]
	WORD				nChilerSt[3];								// チラー運転信号[0:表面,1:裏面,2:BK]
	struct TYP_DAISYA {
		WORD			nOnline;									// φω台車位置 オンライン
		WORD			nOffline;									// φω台車位置 オフライン
	} daisya[2];	// [0:表,1:裏]

	struct TYP_SECURITY {
		WORD			nDoor;										// 電気室 扉 開閉状態
		WORD			nKey;										// 電気室 鍵 開閉状態
	} Sec;

	WORD				nPatoResetPb;								// パトライトリセットPB

	struct TYP_AIRBLOW {
		WORD			nAirBlowSt;									// エアードライヤ運転信号
		WORD			nAirBlowErr;								// エアードライヤ異常信号
	} AirBlow[2];	// [0:表,1:裏]

	WORD				nDaisyaInvSt;								// 台車制御盤INV電源OFF


	WORD				nYobi7[6];

	WORD				nToyuSply;									// 塗油スプレー装置 吹付状態

	WORD				nYobi8[23];
};

// 送信構造体	※MAX256byte
struct SEQ_DM_SEND_DEFECT {
	struct  TYP_STAT {
		WORD			nHBCnt;										// ヘルシー（1～9999）
		WORD			nKadou;										// 稼動状態（0:不明, 1:立ち上げ中, 2:停止中, 3:稼働中, 4:リトライ中）
		WORD			nKiki[2];									// 機器状態[表/裏（穴検は予備）]（0:正常, 1:軽故障, 2:重故障）
		WORD			nKensa[2];									// 検査状態[表/裏（穴検は予備）]（0:正常, 1:縮退, 4:エッジ不正, 5:未保存, 6:未検査, 9:停止）
		WORD			nPlg;										// PLG選択区分（0:オンライン, 1:表校正, 2:裏校正, 3:テストパルス）
	};

	TYP_STAT			typKensaStat;								// φωステータス
	WORD				nYobi1[1];

	struct TYP_VOICE {
		WORD				nVoice_Err[6];							// 設備異常ボイス出力 条件（0:停止, 1:出力）
																	//  0: 設備異常 1: 「疵検異常です」
																	//  1: 設備異常 2: 「コイル情報が受信できません」
																	//  2: 設備異常 3: 「疵検査実績送信異常です」
																	//  3: 設備異常 4: 「照明装置異常です」
																	//  4: 設備異常 5: 「カメラ異常です」
																	//  5: 設備異常 6: 「温度異常です」
		WORD				nVoice_Def[10];							// 欠陥検出ボイス出力 条件（0:停止, 1:出力）
																	//  6: 欠陥検出 1: 「表」
																	//  7: 欠陥検出 2: 「裏」
																	//  8: 欠陥検出 3: 「ヘゲ接近」
																	//  9: 欠陥検出 4: 「スリバー接近」
																	// 10: 欠陥検出 5: 「連続疵接近」
																	// 11: 欠陥検出 6: 「重大疵接近」
																	// 12: 欠陥検出 7: 「周期疵注意」
																	// 13: 欠陥検出 8: 「周期疵注意 単発」
																	// 14: 欠陥検出 9: 「疵接近」
																	// 15: 欠陥検出10: 「表裏面疵接近」
	};

	TYP_VOICE			typKensaVoice;								// φωボイス

	//WORD				nVoice[18];									// ボイス出力 条件（0:停止, 1:出力）
	WORD				nYobi2[8];

	WORD				nSekkin[2][8][2];							// 疵接近ランプ [0:表, 1:裏][0:DS側, ..., 6:WS側][0:軽疵, 1:重疵]（0:停止, 1:出力）
	WORD				nSekalm[4][2];								// 疵接近アラーム (裏面のみ)[0:DS側, ..., 6:WS側][0:軽疵, 1:重疵]（0:停止, 1:出力）
	WORD				nPato[2][2];								// パトライト[0:検査台, 1:運転室][0:ランプ, 1:ブザー]（0:停止, 1～:出力（未チェック件数））

	WORD				nYobi3[12];

	WORD				nCntReset;									// シーケンサ 高機能カウンタリセット要求（0:初期値, 1:リセット要求）
	WORD				nCntStart;									// シーケンサ 高機能カウンタ開始要求（0:初期値, 1:開始要求）
	WORD				nTestWpdReq;								// テスト用 WPD信号 ON要求（※ゲート信号もON）

	WORD				nYobi4[25];
};
#pragma pack(pop)


#endif