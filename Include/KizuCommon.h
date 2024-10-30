// *********************************************************************************
//	全疵検で共通のインクルード
//	[Ver]
//		Ver.01    2007/01/30  vs2005 対応
//
//	[メモ]
//		基本的に、書き換え不可
// *********************************************************************************

#ifndef	_KIZUCOMMON_H
#define	_KIZUCOMMON_H

#include <windef.h>

// 疵検インクルード
#include "KizuListID.h"										// 疵検工程コード定義ヘッダー
#include "KizuCommonSelect.h"								// 全疵検での切替定数のインクルード



//***************************************************************
// 設定定数 定義
//***************************************************************

//=======================================
// 固定文字
//=======================================

// 疵検設定ファイル (機能単位の設定ファイル)
#ifndef LOCAL	// 本番用
	#define		TASKINI_NAME		".\\KizuTask.ini"		// 疵検設定ファイル
#else			// 開発用
	#define		TASKINI_NAME		".\\KizuTask.ini"		// 疵検設定ファイル
#endif

// 疵検設定ファイル (PC単位の設定ファイル)
#ifndef LOCAL	// 本番用
	#define		PCINI_NAME			".\\KizuPc.ini"			// PC設定ファイル
#else			// 開発用
	#define		PCINI_NAME			".\\KizuPc.ini"			// PC設定ファイル
#endif


// 疵検設定ファイル (PCのローカル単位の設定ファイル)
	// 特殊なiniファイルなので、以下の事に注意
		// ・個々のパソコンの一時的な設定情報を保存しておくファイル
		// ・ログオフ等しても上書きされない
		// ・当iniファイルが無くても、必ずタスク側は動作する(デフォルト値などで)ように、作っておく事
#ifndef LOCAL	// 本番用
	#define		LOCALINI_NAME		".\\KizuLocal.ini"		// Local設定ファイル
#else			// 開発用
	#define		LOCALINI_NAME		".\\KizuLocal.ini"		// Local設定ファイル
#endif



// iniファイル 共通セッション (色んなタスクで使用する項目のみ)
#define		INI_COMMON				"COMMON"				// 共通設定 用セッション
#define		INI_DB					"DB"					// DB接続 用セッション (オンライン)
#define		INI_DB_A				"DB_A"					// DB接続 用セッション (A系)
#define		INI_DB_B				"DB_B"					// DB接続 用セッション (B系)
#define		INI_DB_PARA				"DB_1"					// DB接続 用セッション (ロジックパララン用)
#define		INI_STATUS				"STATUS"				// 機器ID 用セッション

//#define		INI_MEM					"MEM"					// 共有メモリ 用セッション
//#define		INI_EXE					"EXECUTE"				// プロセス 用セッション


//=======================================
// 固定サイズ
//=======================================
#define SIZE_MSL					256//400				// send_mail,recv_mailサイズ
#define SIZE_MSL_DATA				(SIZE_MSL-sizeof(long)*2)	// nEventNo以降のデータ長
#define SIZE_NAME_PC				16						// パソコン名称
#define SIZE_NAME_TASK				16						// タスク名称
#define SIZE_IMAGE_PATH				32						// 画像保存用のパス長
#define	SIZE_KIZUKEN_NO				20						// 疵検管理No (=管理No)  ([XXyyyymmdd hhnnss\0]の形式)
#define SIZE_SERIAL_NO				16						// 一意な管理No ([yyyymmdd hhnnss\0]の形式)
#define SIZE_SERIAL_FF_NO			20						// 一意な管理No ([yyyymmdd hhnnssfff\0]の形式)

// --->>> C++のみ
#define	M_PI					3.1415926535897932384626	// π
#define M_STEEL_RATE			7.87						// 鉄比重
// <<<------

// ちょっとEPC関連のマスター
#define MAX_HT1_CAMSET				16						// 判定PC1台の最大カメラセット (EPC側8セットまでしか対応していない)
#define MAX_HT1_CAMANGLE			4						// 判定PC1台の最大カメラ角度   (EPC側4角度までしか対応していない)
#define MAX_ATTR_BASE				256						// 最大特徴量数 (基本特徴量)
//#define MAX_ATTR_ANGLE				128						// 最大特徴量数 (角度別特徴量)		※ 複眼
#define MAX_ATTR_ANGLE				256						// 最大特徴量数 (角度別特徴量)		※ 色差

#define MAX_TAJUUTHRESHOLD			30						// 多重閾値情報数 MAX
#define MAX_REDUCTION				30						// 縮小率情報数 MAX
#define MAX_FILTER					30						// フィルタ情報数 MAX
#define MAX_CROP_SHRINK             4                       // 間引き数 MAX

// DBマスター絡み
#define MAX_STATUS					1024					// 最大 機器状態マスター数
#define MAX_TYPE					256						// 疵種情報MAX
// 2023.11.17 最終工程検査装置高機能化その３ -------------------->>>>>
//#define MAX_GRADE					8						// グレード情報MAX
#define MAX_GRADE					32						// グレード情報MAX
// 2023.11.17 最終工程検査装置高機能化その３ <<<<<--------------------
#define MAX_CAMSET					(NUM_CAM_POS*2)			// カメラセット数MAX (両面)
#define MAX_CAMANGLE				NUM_CAM_ANGLE			// カメラ角度数MAX (論理的な角度)
#define MAX_PHYSICAL_CAMANGLE		1						// カメラ角度数MAX (物理的な角度)
#define MAX_SCODE					64						// 検査表面状態MAX (デフォルト項目1個含む) (1～64)
#define MAX_PCODE					64						// 検査パターンMAX (デフォルト項目1個含む) (1～64) ﾔﾒ(A～Z)
#define MAX_CCODE					64						// カウントパターンMAX (デフォルト項目1個含む) (1～64)
#define MAX_CODE_JUDGE				32						// 検査表面状態・検査パターンの条件MAX
#define MAX_IMAGE_CNT				4						// 画像枚数 (=エンジンから出力される結果数) (式差まではMAX_CAMANGLEとMAX_IMAGE_CNTは同義だったが、式差では、カメラ１台で複数画像が取れる対策)

// カメラリンクスイッチ定義
#define	MAX_CAMLINKSW_DAISYCHAIN	8						// デイジーチェーン接続可能台数
#define	MAX_CAMLINKSW_INPUT			4						// スイッチFPGAボード1枚当たりの最大入力数
#define	MAX_CAMLINKSW_OUTPUT		15						// スイッチFPGAボード1枚当たりの最大出力数
#define	MAX_CAMLINKSW_FPGA			2						// カメラリンクスイッチ1台当たりのスイッチFPGAボード枚数
#define	MAX_CAMLINKSW_ALLINPUT		MAX_CAMLINKSW_INPUT*MAX_CAMLINKSW_FPGA	// カメラリンクスイッチ1台当たりの最大入力数
#define	MAX_CAMLINKSW_ALLOUTPUT		MAX_CAMLINKSW_OUTPUT*MAX_CAMLINKSW_FPGA	// カメラリンクスイッチ1台当たりの最大出力数


// NOA定義
#define NOA_ONLINE					0						// オンライン
#define NOA_OFFLINE					1						// オフライン
#define NOA_DEBUG					9						// デバッグ用 (使いやすい用に全部入り)
#define NOA_KAISEKI					-1						// 解析用(ほぼデバッグと同じ)

//=======================================
// PC/タスク設定
//=======================================
#ifndef LOCAL	// 本番用
	#define MASTER_PC_NAME			TOKATU					// 疵検状態管理送り先PC [KS_MASTERの起動PC名称を指定]("."指定はダメ)
	#define SYSLOG_PC_NAME			TOKATU					// syslog送り先PC名
#else			// 開発用
	#define MASTER_PC_NAME			TOKATU					// 疵検状態管理送り先PC [KS_MASTERの起動PC名称を指定]("."指定はダメ)
	#define SYSLOG_PC_NAME			"." //TOKATU					// syslog送り先PC名
#endif


//=======================================
// パラメータ変更区分 （疵検共通部分）
//   各ライン固有画面の場合は、LineCommonに記述すること
//=======================================
#define CH_SCODE_NON				0						// 検査表面状態対象外
#define CH_SCODE_ALL				999						// 検査表面状態全部変更区分

// 今野さんロジックが、全パラメータ一括読込みとなった為、画面単位での通知に変更
#define CH_PARAM_ALL				0						// 全テーブル (デバック用)
#define CH_PARAM_TYPE_GRADE			1						// 疵種・Gr

#define CH_PARAM_ATTR_NAME			5						// 特徴量名称
#define CH_PARAM_PCINFODESC			6						// 上位情報

#define CH_PARAM_COMMON				10						// 固定パラメータ
#define CH_PARAM_CAMERA				11						// カメラ設定
#define CH_PARAM_EDGE				12						// エッジ検出
#define CH_PARAM_SHD				13						// シェーディング補正
#define CH_PARAM_THRESHOLD			14						// 閾値設定
#define CH_PARAM_DETECT				15						// 疵検出
#define CH_PARAM_CYCLE				16						// 単発用周期疵判定
#define CH_PARAM_MERGE				17						// カメラ境界処理(疵統合処理)
#define CH_PARAM_LIGHT				18						// 照明
#define CH_PARAM_TINY				19						// 微小欠陥
#define CH_PARAM_SYNCADD			20						// 画像加算 (+ロール) ※周期PCがあるラインのみ

#define CH_PARAM_COLOR_DIV			21						// 混色分離
//#define CH_PARAM_YOUSET				22						// 溶接点検出


#define CH_PARAM_SETTIG_SCODE		110						// 検査表面状態 マスター・条件設定・出力設定
#define CH_PARAM_SETTIG_PCODE		111						// 検査パターン マスター・条件設定・出力設定
#define CH_PARAM_SETTIG_COMMON		112						// 検査定数設定
#define CH_PARAM_SETTIG_PCOMMON		113						// 検査パターン 固有 出力設定
#define CH_PARAM_SETTIG_PALL		114						// 検査パターン 共通 出力設定
#define CH_PARAM_SETTIG_LOG			115						// シスログテーブル
#define CH_PARAM_SETTIG_STATUS		116						// 機器状態テーブル




//***************************************************************
// DIV 定義
//   KizuLibMFC の DivNameManager で名称を取得可能
//***************************************************************

//=======================================
// 拡張BOOL
//=======================================
enum EM_DIV_BOOL {
	DIV_BOOL_TRUE = 1,										// 正常 ○
	DIV_BOOL_FALSE = 0,										// 異常 ×
	DIV_BOOL_NON = -1,										// 無し －
	DIV_BOOL_NULL = -9										// 対象外 ＊
};

//=======================================
// PLG区分
//=======================================
enum EM_DIV_PLG {
	DIV_PLG_LINE	= 1,									// [PLG区分] ラインPLG
	DIV_PLG_TOP,											// [PLG区分] 表校正用パルス
	DIV_PLG_BOT,											// [PLG区分] 裏校正用パルス
	DIV_PLG_TEST											// [PLG区分] テストパルス
};

//=======================================
// 運転状態
//=======================================	
enum EM_DIV_UNTEN {
	DIV_UNTEN_STOP	= 0,									// [運転状態] 停止モード ← これって必要？
	DIV_UNTEN_NOMAL,										// [運転状態] 通常運転モード
	DIV_UNTEN_SAMP,											// [運転状態] テスト運転モード (サンプル撮影)
	DIV_UNTEN_CAMERA,										// [運転状態] 校正モード (カメラ調整)
	DIV_UNTEN_SMYU											// [運転状態] シミュレーションモード
};

//=======================================
// 操業状態
//=======================================	
enum EM_DIV_SOGYO {
	DIV_SOGYO_NON = -1,										// [操業状態] 不明

	DIV_SOGYO_STOP = 0,										// [操業状態] ライン休止中
	DIV_SOGYO_SLOW,											// [操業状態] ライン一時停止or低速操業
	DIV_SOGYO_RUN											// [操業状態] ライン操業中
};

//=======================================
// 稼動状態
//=======================================	
enum EM_DIV_KADOU {
	DIV_KADOU_INIT	= 1,									// [稼動状態] 初期化中
	DIV_KADOU_STOP,											// [稼動状態] 停止中 (準備完了)
	DIV_KADOU_START,										// [稼動状態] 稼動中 & テスト中
	DIV_KADOU_RETRY											// [稼動状態] リトライ中
};

//=======================================
// 検査状態 (値が大きい方が、より強い異常となるようにする事)
//=======================================	
enum EM_DIV_KENSA {
	DIV_KENSA_NON  = -9,									// [検査状態] 対象外 (停止中)

// 拡張部---->>
	DIV_KENSA_INIT = -1,									// [検査状態] 停止 (表裏実績別管理時には、これで初期化しておく。9 _STOPと同じ画面表示)
// <----

	DIV_KENSA_OK   = 0,										// [検査状態] 正常
	DIV_KENSA_NG   = 1,										// [検査状態] 縮退
	DIV_KENSA_MENTE= 2,										// [検査状態] ﾒﾝﾃ中   ※プロビジョン

// 拡張部---->>
	DIV_KENSA_EDGENG= 4,									// [検査状態] ｴｯｼﾞ不正 (エッジ未検出、エッジ整合性異常)
	DIV_KENSA_DBNON	= 5,									// [検査状態] DB実績未保存(有害Gr実績遅れ、保存件数上限到達)
	DIV_KENSA_SKIP	= 6,									// [検査状態] 未検査区間  (画像処理エンジンがスキップ)
// <----

	DIV_KENSA_STOP = 9										// [検査状態] 停止  (片面異常の場合、検査停止中)
};

//=======================================
// 機器状態 (値が大きい方が、より強い異常となるようにする事)
//=======================================
enum EM_DIV_KIKI {
	DIV_KIKI_OK  = 0,										// [機器状態] 正常
	DIV_KIKI_KEI = 1,										// [機器状態] 軽故障
	//DIV_KIKI_TYU = 2,										// [機器状態] 中故障 (プロビジョン)
	DIV_KIKI_JYU = 3										// [機器状態] 重故障
};

//=======================================
// DS, WS 等のポジション
//=======================================
enum EM_DIV_POS {
#ifdef NUM_CAM_POS_1
	DIV_POS_C = 0											// C
#endif
#ifdef NUM_CAM_POS_2
	DIV_POS_DS = 0,											// DS
	DIV_POS_WS												// WS
#endif
#ifdef NUM_CAM_POS_3
	DIV_POS_DS = 0,											// DS
	DIV_POS_C,												// C
	DIV_POS_WS												// WS
#endif
#ifdef NUM_CAM_POS_4
	DIV_POS_DS = 0,											// DS
	DIV_POS_D1,												// D1
	DIV_POS_W1,												// W1
	DIV_POS_WS												// WS
#endif
#ifdef NUM_CAM_POS_5
	DIV_POS_DS = 0,											// DS
	DIV_POS_D1,												// D1
	DIV_POS_C,												// C
	DIV_POS_W1,												// W1
	DIV_POS_WS												// WS
#endif
#ifdef NUM_CAM_POS_6
	DIV_POS_DS = 0,											// DS
	DIV_POS_D1,												// D1
	DIV_POS_D2,												// D2
	DIV_POS_W2,												// W2
	DIV_POS_W1,												// W1
	DIV_POS_WS												// WS
#endif
#ifdef NUM_CAM_POS_7
	DIV_POS_DS = 0,											// DS
	DIV_POS_D1,												// D1
	DIV_POS_D2,												// D2
	DIV_POS_C,												// C
	DIV_POS_W2,												// W2
	DIV_POS_W1,												// W1
	DIV_POS_WS												// WS
#endif
#ifdef NUM_CAM_POS_8
	DIV_POS_DS = 0,											// DS
	DIV_POS_D1,												// D1
	DIV_POS_D2,												// D2
	DIV_POS_D3,												// D3
	DIV_POS_W3,												// W3
	DIV_POS_W2,												// W2
	DIV_POS_W1,												// W1
	DIV_POS_WS												// WS
#endif
#ifdef NUM_CAM_POS_9
	DIV_POS_DS = 0,											// DS
	DIV_POS_D1,												// D1
	DIV_POS_D2,												// D2
	DIV_POS_D3,												// D3
	DIV_POS_C,												// C
	DIV_POS_W3,												// W3
	DIV_POS_W2,												// W2
	DIV_POS_W1,												// W1
	DIV_POS_WS												// WS
#endif
#ifdef NUM_CAM_POS_10
	DIV_POS_DS = 0,											// DS
	DIV_POS_D1,												// D1
	DIV_POS_D2,												// D2
	DIV_POS_D3,												// D3
	DIV_POS_D4,												// D4
	DIV_POS_W4,												// W4
	DIV_POS_W3,												// W3
	DIV_POS_W2,												// W2
	DIV_POS_W1,												// W1
	DIV_POS_WS												// WS
#endif
#ifdef NUM_CAM_POS_11
	DIV_POS_DS = 0,											// DS
	DIV_POS_D1,												// D1
	DIV_POS_D2,												// D2
	DIV_POS_D3,												// D3
	DIV_POS_D4,												// D4
	DIV_POS_C,												// C
	DIV_POS_W4,												// W4
	DIV_POS_W3,												// W3
	DIV_POS_W2,												// W2
	DIV_POS_W1,												// W1
	DIV_POS_WS												// WS
#endif
#ifdef NUM_CAM_POS_12
	DIV_POS_DS = 0,											// DS
	DIV_POS_D1,												// D1
	DIV_POS_D2,												// D2
	DIV_POS_D3,												// D3
	DIV_POS_D4,												// D4
	DIV_POS_D5,												// D5
	DIV_POS_W5,												// W5
	DIV_POS_W4,												// W4
	DIV_POS_W3,												// W3
	DIV_POS_W2,												// W2
	DIV_POS_W1,												// W1
	DIV_POS_WS												// WS
#endif
#ifdef NUM_CAM_POS_13
	DIV_POS_DS = 0,											// DS
	DIV_POS_D1,												// D1
	DIV_POS_D2,												// D2
	DIV_POS_D3,												// D3
	DIV_POS_D4,												// D4
	DIV_POS_D5,												// D5
	DIV_POS_C,												// C
	DIV_POS_W5,												// W5
	DIV_POS_W4,												// W4
	DIV_POS_W3,												// W3
	DIV_POS_W2,												// W2
	DIV_POS_W1,												// W1
	DIV_POS_WS												// WS
#endif
#ifdef NUM_CAM_POS_14
	DIV_POS_DS = 0,											// DS
	DIV_POS_D1,												// D1
	DIV_POS_D2,												// D2
	DIV_POS_D3,												// D3
	DIV_POS_D4,												// D4
	DIV_POS_D5,												// D5
	DIV_POS_D6,												// D6
	DIV_POS_W6,												// W6
	DIV_POS_W5,												// W5
	DIV_POS_W4,												// W4
	DIV_POS_W3,												// W3
	DIV_POS_W2,												// W2
	DIV_POS_W1,												// W1
	DIV_POS_WS												// WS
#endif
#ifdef NUM_CAM_POS_15
	DIV_POS_DS = 0,											// DS
	DIV_POS_D1,												// D1
	DIV_POS_D2,												// D2
	DIV_POS_D3,												// D3
	DIV_POS_D4,												// D4
	DIV_POS_D5,												// D5
	DIV_POS_D6,												// D6
	DIV_POS_C,												// C
	DIV_POS_W6,												// W6
	DIV_POS_W5,												// W5
	DIV_POS_W4,												// W4
	DIV_POS_W3,												// W3
	DIV_POS_W2,												// W2
	DIV_POS_W1,												// W1
	DIV_POS_WS												// WS
#endif
#ifdef NUM_CAM_POS_16
	DIV_POS_DS = 0,											// DS
	DIV_POS_D1,												// D1
	DIV_POS_D2,												// D2
	DIV_POS_D3,												// D3
	DIV_POS_D4,												// D4
	DIV_POS_D5,												// D5
	DIV_POS_D6,												// D6
	DIV_POS_D7,												// D7
	DIV_POS_W7,												// W7
	DIV_POS_W6,												// W6
	DIV_POS_W5,												// W5
	DIV_POS_W4,												// W4
	DIV_POS_W3,												// W3
	DIV_POS_W2,												// W2
	DIV_POS_W1,												// W1
	DIV_POS_WS												// WS
#endif
};

//=======================================
// カメラセットNo
//=======================================
enum EM_DIV_CAMSET {
	DIV_CAMSET_BACK = 0,									// [カメラペアNo] バックアップ

#ifdef NUM_CAM_POS_1
	DIV_CAMSET_T_C,
	DIV_CAMSET_B_C
#endif
#ifdef NUM_CAM_POS_2
	DIV_CAMSET_T_DS,										// 表
	DIV_CAMSET_T_WS,
	DIV_CAMSET_B_DS,
	DIV_CAMSET_B_WS
#endif
#ifdef NUM_CAM_POS_3
	DIV_CAMSET_T_DS,										// 表
	DIV_CAMSET_T_C,
	DIV_CAMSET_T_WS,
	DIV_CAMSET_B_DS,
	DIV_CAMSET_B_C,
	DIV_CAMSET_B_WS
#endif
#ifdef NUM_CAM_POS_4
	DIV_CAMSET_T_DS,										// 表
	DIV_CAMSET_T_D1,
	DIV_CAMSET_T_W1,
	DIV_CAMSET_T_WS,

	DIV_CAMSET_B_DS,
	DIV_CAMSET_B_D1,
	DIV_CAMSET_B_W1,
	DIV_CAMSET_B_WS
#endif
#ifdef NUM_CAM_POS_5
	DIV_CAMSET_T_DS,										// 表
	DIV_CAMSET_T_D1,
	DIV_CAMSET_T_C,
	DIV_CAMSET_T_W1,
	DIV_CAMSET_T_WS,

	DIV_CAMSET_B_DS,
	DIV_CAMSET_B_D1,
	DIV_CAMSET_B_C,
	DIV_CAMSET_B_W1,
	DIV_CAMSET_B_WS
#endif
#ifdef NUM_CAM_POS_6
	DIV_CAMSET_T_DS,										// 表
	DIV_CAMSET_T_D1,
	DIV_CAMSET_T_D2,
	DIV_CAMSET_T_W2,
	DIV_CAMSET_T_W1,
	DIV_CAMSET_T_WS,

	DIV_CAMSET_B_DS,
	DIV_CAMSET_B_D1,
	DIV_CAMSET_B_D2,
	DIV_CAMSET_B_W2,
	DIV_CAMSET_B_W1,
	DIV_CAMSET_B_WS
#endif
#ifdef NUM_CAM_POS_7
	DIV_CAMSET_T_DS,										// 表
	DIV_CAMSET_T_D1,
	DIV_CAMSET_T_D2,
	DIV_CAMSET_T_C,
	DIV_CAMSET_T_W2,
	DIV_CAMSET_T_W1,
	DIV_CAMSET_T_WS,

	DIV_CAMSET_B_DS,
	DIV_CAMSET_B_D1,
	DIV_CAMSET_B_D2,
	DIV_CAMSET_B_C,
	DIV_CAMSET_B_W2,
	DIV_CAMSET_B_W1,
	DIV_CAMSET_B_WS
#endif
#ifdef NUM_CAM_POS_8
	DIV_CAMSET_T_DS,										// 表
	DIV_CAMSET_T_D1,
	DIV_CAMSET_T_D2,
	DIV_CAMSET_T_D3,
	DIV_CAMSET_T_W3,
	DIV_CAMSET_T_W2,
	DIV_CAMSET_T_W1,
	DIV_CAMSET_T_WS,

	DIV_CAMSET_B_DS,
	DIV_CAMSET_B_D1,
	DIV_CAMSET_B_D2,
	DIV_CAMSET_B_D3,
	DIV_CAMSET_B_W3,
	DIV_CAMSET_B_W2,
	DIV_CAMSET_B_W1,
	DIV_CAMSET_B_WS
#endif
#ifdef NUM_CAM_POS_9
	DIV_CAMSET_T_DS,										// 表
	DIV_CAMSET_T_D1,
	DIV_CAMSET_T_D2,
	DIV_CAMSET_T_D3,
	DIV_CAMSET_T_C,
	DIV_CAMSET_T_W3,
	DIV_CAMSET_T_W2,
	DIV_CAMSET_T_W1,
	DIV_CAMSET_T_WS,

	DIV_CAMSET_B_DS,
	DIV_CAMSET_B_D1,
	DIV_CAMSET_B_D2,
	DIV_CAMSET_B_D3,
	DIV_CAMSET_B_C,
	DIV_CAMSET_B_W3,
	DIV_CAMSET_B_W2,
	DIV_CAMSET_B_W1,
	DIV_CAMSET_B_WS
#endif
#ifdef NUM_CAM_POS_10
	DIV_CAMSET_T_DS,										// 表
	DIV_CAMSET_T_D1,
	DIV_CAMSET_T_D2,
	DIV_CAMSET_T_D3,
	DIV_CAMSET_T_D4,
	DIV_CAMSET_T_W4,
	DIV_CAMSET_T_W3,
	DIV_CAMSET_T_W2,
	DIV_CAMSET_T_W1,
	DIV_CAMSET_T_WS,

	DIV_CAMSET_B_DS,
	DIV_CAMSET_B_D1,
	DIV_CAMSET_B_D2,
	DIV_CAMSET_B_D3,
	DIV_CAMSET_B_D4,
	DIV_CAMSET_B_W4,
	DIV_CAMSET_B_W3,
	DIV_CAMSET_B_W2,
	DIV_CAMSET_B_W1,
	DIV_CAMSET_B_WS
#endif
#ifdef NUM_CAM_POS_11
	DIV_CAMSET_T_DS,										// 表
	DIV_CAMSET_T_D1,
	DIV_CAMSET_T_D2,
	DIV_CAMSET_T_D3,
	DIV_CAMSET_T_D4,
	DIV_CAMSET_T_C,
	DIV_CAMSET_T_W4,
	DIV_CAMSET_T_W3,
	DIV_CAMSET_T_W2,
	DIV_CAMSET_T_W1,
	DIV_CAMSET_T_WS,

	DIV_CAMSET_B_DS,
	DIV_CAMSET_B_D1,
	DIV_CAMSET_B_D2,
	DIV_CAMSET_B_D3,
	DIV_CAMSET_B_D4,
	DIV_CAMSET_B_C,
	DIV_CAMSET_B_W4,
	DIV_CAMSET_B_W3,
	DIV_CAMSET_B_W2,
	DIV_CAMSET_B_W1,
	DIV_CAMSET_B_WS
#endif
#ifdef NUM_CAM_POS_12
	DIV_CAMSET_T_DS,										// 表
	DIV_CAMSET_T_D1,
	DIV_CAMSET_T_D2,
	DIV_CAMSET_T_D3,
	DIV_CAMSET_T_D4,
	DIV_CAMSET_T_D5,
	DIV_CAMSET_T_W5,
	DIV_CAMSET_T_W4,
	DIV_CAMSET_T_W3,
	DIV_CAMSET_T_W2,
	DIV_CAMSET_T_W1,
	DIV_CAMSET_T_WS,

	DIV_CAMSET_B_DS,
	DIV_CAMSET_B_D1,
	DIV_CAMSET_B_D2,
	DIV_CAMSET_B_D3,
	DIV_CAMSET_B_D4,
	DIV_CAMSET_B_D5,
	DIV_CAMSET_B_W5,
	DIV_CAMSET_B_W4,
	DIV_CAMSET_B_W3,
	DIV_CAMSET_B_W2,
	DIV_CAMSET_B_W1,
	DIV_CAMSET_B_WS
#endif
#ifdef NUM_CAM_POS_13
	DIV_CAMSET_T_DS,										// 表
	DIV_CAMSET_T_D1,
	DIV_CAMSET_T_D2,
	DIV_CAMSET_T_D3,
	DIV_CAMSET_T_D4,
	DIV_CAMSET_T_D5,
	DIV_CAMSET_T_C,
	DIV_CAMSET_T_W5,
	DIV_CAMSET_T_W4,
	DIV_CAMSET_T_W3,
	DIV_CAMSET_T_W2,
	DIV_CAMSET_T_W1,
	DIV_CAMSET_T_WS,

	DIV_CAMSET_B_DS,
	DIV_CAMSET_B_D1,
	DIV_CAMSET_B_D2,
	DIV_CAMSET_B_D3,
	DIV_CAMSET_B_D4,
	DIV_CAMSET_B_D5,
	DIV_CAMSET_B_C,
	DIV_CAMSET_B_W5,
	DIV_CAMSET_B_W4,
	DIV_CAMSET_B_W3,
	DIV_CAMSET_B_W2,
	DIV_CAMSET_B_W1,
	DIV_CAMSET_B_WS
#endif
#ifdef NUM_CAM_POS_14
	DIV_CAMSET_T_DS,										// 表
	DIV_CAMSET_T_D1,
	DIV_CAMSET_T_D2,
	DIV_CAMSET_T_D3,
	DIV_CAMSET_T_D4,
	DIV_CAMSET_T_D5,
	DIV_CAMSET_T_D6,
	DIV_CAMSET_T_W6,
	DIV_CAMSET_T_W5,
	DIV_CAMSET_T_W4,
	DIV_CAMSET_T_W3,
	DIV_CAMSET_T_W2,
	DIV_CAMSET_T_W1,
	DIV_CAMSET_T_WS,

	DIV_CAMSET_B_DS,
	DIV_CAMSET_B_D1,
	DIV_CAMSET_B_D2,
	DIV_CAMSET_B_D3,
	DIV_CAMSET_B_D4,
	DIV_CAMSET_B_D5,
	DIV_CAMSET_B_D6,
	DIV_CAMSET_B_W6,
	DIV_CAMSET_B_W5,
	DIV_CAMSET_B_W4,
	DIV_CAMSET_B_W3,
	DIV_CAMSET_B_W2,
	DIV_CAMSET_B_W1,
	DIV_CAMSET_B_WS
#endif
#ifdef NUM_CAM_POS_15
	DIV_CAMSET_T_DS,										// 表
	DIV_CAMSET_T_D1,
	DIV_CAMSET_T_D2,
	DIV_CAMSET_T_D3,
	DIV_CAMSET_T_D4,
	DIV_CAMSET_T_D5,
	DIV_CAMSET_T_D6,
	DIV_CAMSET_T_C,
	DIV_CAMSET_T_W6,
	DIV_CAMSET_T_W5,
	DIV_CAMSET_T_W4,
	DIV_CAMSET_T_W3,
	DIV_CAMSET_T_W2,
	DIV_CAMSET_T_W1,
	DIV_CAMSET_T_WS,

	DIV_CAMSET_B_DS,
	DIV_CAMSET_B_D1,
	DIV_CAMSET_B_D2,
	DIV_CAMSET_B_D3,
	DIV_CAMSET_B_D4,
	DIV_CAMSET_B_D5,
	DIV_CAMSET_B_D6,
	DIV_CAMSET_B_C,
	DIV_CAMSET_B_W6,
	DIV_CAMSET_B_W5,
	DIV_CAMSET_B_W4,
	DIV_CAMSET_B_W3,
	DIV_CAMSET_B_W2,
	DIV_CAMSET_B_W1,
	DIV_CAMSET_B_WS
#endif
#ifdef NUM_CAM_POS_16
	DIV_CAMSET_T_DS,										// 表
	DIV_CAMSET_T_D1,
	DIV_CAMSET_T_D2,
	DIV_CAMSET_T_D3,
	DIV_CAMSET_T_D4,
	DIV_CAMSET_T_D5,
	DIV_CAMSET_T_D6,
	DIV_CAMSET_T_D7,
	DIV_CAMSET_T_W7,
	DIV_CAMSET_T_W6,
	DIV_CAMSET_T_W5,
	DIV_CAMSET_T_W4,
	DIV_CAMSET_T_W3,
	DIV_CAMSET_T_W2,
	DIV_CAMSET_T_W1,
	DIV_CAMSET_T_WS,

	DIV_CAMSET_B_DS,
	DIV_CAMSET_B_D1,
	DIV_CAMSET_B_D2,
	DIV_CAMSET_B_D3,
	DIV_CAMSET_B_D4,
	DIV_CAMSET_B_D5,
	DIV_CAMSET_B_D6,
	DIV_CAMSET_B_D7,
	DIV_CAMSET_B_W7,
	DIV_CAMSET_B_W6,
	DIV_CAMSET_B_W5,
	DIV_CAMSET_B_W4,
	DIV_CAMSET_B_W3,
	DIV_CAMSET_B_W2,
	DIV_CAMSET_B_W1,
	DIV_CAMSET_B_WS
#endif
};

//=======================================
// 欠陥グレード
//=======================================
enum EM_DETECT_GR {
	DETECT_GR_NON = 0,										// 無害
	DETECT_GR_KEI,											// 軽欠陥
	DETECT_GR_TYU,											// 中欠陥
	DETECT_GR_JU											// 重欠陥
};
enum EM_DETECT_GR2 {
	DETECT_GR2_NON = 0,										// 無害
	DETECT_GR2_KEI,											// 軽欠陥
	DETECT_GR2_JU											// 重欠陥
};


//***************************************************************
// 基本的に これ以降 全疵検共通仕様
//	※ これ以外の項目については、LineCommonに定義する
//***************************************************************
//=======================================
// プロセス名称一覧
//=======================================
// 支援機能
#define KS_MASTER					"KS_MASTER"				// PCマスター管理
#define KS_SLAVE					"KS_SLAVE"				// PCスレーブ管理
#define KS_START					"KS_START"				// スタートアップ管理
#define KS_WAKEUP					"KS_WAKEUP"				// イニシャル中待機画面
#define KS_EVTLOG					"KS_EVTLOG"				// イベントビューアー情報管理
#define KS_SYSLOG					"KS_SYSLOG"				// SYSLOG登録タスク
#define KS_UPSMGR					"KS_UPSMGR"				// UPS異常終了タスク
#define KS_BACKUP					"KS_BACKUP"				// バックアップ切替支援タスク
#define KS_EQALDB					"KS_EQALDB"				// DB同期管理タスク

#define KS_HDDCHK					"KS_HDDCHK"				// HDD状態監視タスク
#define KS_HISTRY					"KS_HISTRY"				// 機器状態履歴管理タスク
#define KS_APCHECK					"KS_APCHECK"			// AP状態監視タスク
#define KS_MAILTR					"KS_MAILTR"				// メールスロット転送タスク

// 共通タスク
	// ※ ここに未定義のKTタスクは、LineCommon.hに起動要因等定義しているのがあるので注意
#define KT_CHILER					"KT_CHILER"				// 対チラー通信
//#define KT_LEDMGR					"KT_LEDMGR"				// 対LED照明通信
#define KT_TEMPMT					"KT_TEMPMT"				// 対温度パネルメータ (プロビジョン)
#define KT_RPICHK					"KT_RPICHK"				// RPIファイル管理


// 蓄積機能	
#define TI_DELETE					"TI_DELETE"				// DB削除管理タスク	


//=======================================
// データベーステーブル名称
//=======================================
#define DB_ADD_M											// M_,T_付きの場合付与
// 太平マスター関連
#define DB_STATUS_MASTER			"M_STATUS_MASTER"		// [DB] 機器マスターテーブル
#define DB_STATUS_INF				"M_STATUS_INF"			// [DB] 機器状態テーブル
#define DB_LOG_MASTER				"M_LOG_MASTER"			// [DB] ログマスターテーブル
#define DB_LOG_INF					"T_LOG_INF"				// [DB] ログ情報テーブル
#define DB_PC_MASTER				"M_PC_MASTER"			// [DB] PCマスター
#define DB_PC_INF					"M_PC_INF"				// [DB] PC種別
#define DB_ST_HISTORY_MASTER		"M_ST_HISTORY_MASTER"	// [DB] 機器履歴マスターテーブル
#define DB_ST_HISTORY_INF			"T_ST_HISTORY_INF"		// [DB] 機器履歴テーブル
#define DB_STATUS_LOG				"T_STATUS_LOG"			// [DB] 機器状態ログテーブル
#define DB_BASE_ACTION_MASTER		"M_BASE_ACTION_MASTER"	// [DB] 基本動作一覧設定

#define DB_COMMON_MASTER			"M_COMMON_MASTER"		// [DB] 疵検定数マスター
#define DB_TYPE_NAME				"TYPE_NAME"				// [DB] 疵種マスター
#define DB_GRADE_NAME				"GRADE_NAME"			// [DB] グレードマスター
#define DB_SCODE_MASTER				"M_SCODE_MASTER"		// [DB] 検査表面状態マスター
#define DB_SCODE_JUDGE				"M_SCODE_JUDGE"			// [DB] 検査表面状態条件設定
#define DB_SCODE_OUTPUT				"M_SCODE_OUTPUT"		// [DB] 検査表面状態別出力設定
#define DB_PCODE_MASTER				"M_PCODE_MASTER"		// [DB] 検査パターンマスター
#define DB_PCODE_JUDGE				"M_PCODE_JUDGE"			// [DB] 検査パターン条件設定
#define DB_PCODE_OUTPUT				"M_PCODE_OUTPUT"		// [DB] 検査パターン別出力設定
#define DB_PALL_OUTPUT				"M_PALL_OUTPUT"			// [DB] 検査パターン固有出力設定
#define DB_PCOMMON_OUTPUT			"M_PCOMMON_OUTPUT"		// [DB] 検査パターン共通出力設定

#define	DB_CAMERA_LINKSWITCH		"M_CAMERA_LINKSWITCH"	// [DB] カメラリンクスイッチ設定
#define DB_PAR_GRID_SETTING			"M_PAR_GRID_SETTING"	// [DB] 自動生成画面データグリッド表示設定
#define DB_PAR_LOV_ITEM				"M_PAR_LOV_ITEM"		// [DB] 自動生成画面選択項目アイテムマスタ

// パラメータ
#define DB_ATTR_NAME				"ATTR_NAME"				// [DB] 特徴量名称マスター
#define DB_PARAM_COMMON				"PARAM_COMMON"			// [DB] 共通パラメータ
#define DB_PARAM_SCODE				"PARAM_SCODE"			// [DB] 表面状態別パラメータ
#define DB_PARAM_CAMSET				"PARAM_CAMSET"			// [DB] カメラセット別パラメータ
#define DB_PARAM_CAMSET_SCODE		"PARAM_CAMSET_SCODE"	// [DB] カメラセット別×表面状態別パラメータ
#define DB_PARAM_CAMSET_ANGLE		"PARAM_CAMSET_ANGLE"	// [DB] カメラセット別×カメラ角度別パラメータ
#define DB_PARAM_CAMANGLE_SCODE		"PARAM_CAMANGLE_SCODE"	// [DB] カメラ角度別×表面状態別パラメータ
#define DB_PARAM_CAMSET_ANGLE_SCODE	"PARAM_CAMSET_ANGLE_SCODE"	// [DB] カメラセット別×カメラ角度別×表面状態別パラメータ
#define DB_ROLL_INFO				"ROLL_INFO"             // [DB] ロール情報
#define DB_PARAM_PCINFO_DESC		"PCINFO_DESC"			// [DB] 上位情報内容
#define DB_PARAM_REDUCTION			"PARAM_REDUCTION"		// [DB] 縮小率
#define DB_PARAM_FILTER				"PARAM_FILTER"			// [DB] フィルタ 
#define DB_PARAM_THRESHOLD			"PARAM_THRESHOLD"		// [DB] 多重用閾値
#define DB_PARAM_CYCLE_THRESHOLD	"PD_PARAM_THRESHOLD"	// [DB] 周期用閾値
#define DB_PARAM_CROP_SHRINK        "PARAM_CROP_SHRINK"     // [DB] 間引き

// <20221117> <小島> <最終工程高機能化その1> --->>>
#define DB_PARAM_LABELIMAGE			"PARAM_LABELIMAGE"		// [DB]
#define DB_PARAM_SIMPLEBIN			"PARAM_SIMPLEBIN"		// [DB]
#define DB_PARAM_FILTERBIN			"PARAM_FILTERBIN"		// [DB]
// <20221117> <小島> <最終工程高機能化その1> <<<---

//=======================================
// 起動要因
//=======================================

//// 共通 (9000番台を使用)
//// 支援機能

// KS_MASTER
#define FACT_KS_MASTER_01			9101					// 時刻変更要求 (←TO_DEFECT, SO_GAMENN?)
#define FACT_KS_MASTER_02			9102					// ハートビート (←KS_SLAVE)
#define FACT_KS_MASTER_03			9103					// PCマスター管理状態問い合わせ (←KS_SLAVE)
#define FACT_KS_MASTER_04			9104					// 立ち上げ完了報告 (←KS_SLAVE, KS_MASTER)
#define FACT_KS_MASTER_05			9105					// 立ち下げ完了報告 (←KS_SLAVE, KS_MASTER)
#define FACT_KS_MASTER_06			9106					// システム終了要求 (←SO_GAMENN, KS_UPSMGR)
#define FACT_KS_MASTER_07			9107					// PC再起動要求 (←SO_GAMENN)
#define FACT_KS_MASTER_08			9108					// PCログオフ要求 (←SO_GAMENN)
#define FACT_KS_MASTER_09			9109					// PC常駐タスク終了要求 (←KS_MASTER)

// KS_SLAVE
#define FACT_KS_SLAVE_01			9121					// 時刻変更要求 (←KS_MASTER)
#define FACT_KS_SLAVE_02			9122					// 統括ハートビート (←KS_MASTER)
#define FACT_KS_SLAVE_03			9123					// PCマスター管理起動完了通知 (←KS_MASTER)
#define FACT_KS_SLAVE_04			9124					// シャットダウン要求 (←KS_MASTER)
#define FACT_KS_SLAVE_05			9125					// PC再起動要求 (←KS_MASTER)
#define FACT_KS_SLAVE_06			9126					// PCログオフ要求 (←KS_MASTER)
#define FACT_KS_SLAVE_07			9127					// PC常駐タスク終了要求 (←KS_MASTER)

// KS_START
////////9131

// KS_SYSLOG
#define	FACT_KS_SYSLOG_01			9141					// シスログ出力
#define	FACT_KS_SYSLOG_02			9142					// 機器状態ログ

// KS_EVTLOG
////9151					// イベントログ取得 (←KS_EVTLOG)

// KS_WAKEUP
#define FACT_KS_WAKEUP_01			9161					// 統括立ち上げ完了報告 (←KS_MASTER, KS_SLAVE)
#define FACT_KS_WAKEUP_02			9162					// 蓄積立ち上げ完了報告 (←KS_MASTER, KS_SLAVE)

// KS_UPSMGR
///////9171

// KS_BACKUP
///////9181

// KS_EQALDB
///////9191
#define FACT_KS_EQALDB_01			9191					// 即時実行通知（←KS_BACKUP）
//#define FACT_KS_EQALDB_02			9192					// 実績移動通知（←KS_BACKUP）

// KS_MAILTR
///////9231
#define FACT_KS_MAILTR_01			9231					// メールスロット転送要求(mailtr_que)

//// 蓄積機能
// TI_DELETE
#define FACT_TI_DELETE_01			9411					// 削除･書込優先モード通知 (←TO_DEFECT)


// 9800～9899 はツール等で使用の為予約

// ホントに共通で使用
#define FACT_KS_TASK_STATEUS		9901					// タスク状態取得要求 (～9920まで予約)
#define FACT_KS_TASK_STATEUS_ANS	9902					// タスク状態取得アンサー
#define FACT_KS_DB_STATEUS			9997					// DB状態変更通知 (fl.data[0]= 0:正常 1:異常)	
#define FACT_KS_PCNAME_CHANGE		9998					// PC名称変更通知起動要因
#define FACT_KS_END					9999					// タスク終了起動要因


//=======================================
// スレッド間 起動要因 C++ のみ
//=======================================
#define WM_USER_TH_END				WM_USER+999				// スレッド終了要求

//=======================================
// メールスロット通信用構造体
//=======================================
struct COMMON_QUE {
	long nEventNo;											// イベントコード
	long nLineNo;											// ラインNo (通常は、LINE_ID固定。1PCで複数のシステムが同居している場合に 0は1RCL 1は2RCL用とかに使用する。)

	union {
		// 標準[数字のみ]
		struct FL{
			long data[(SIZE_MSL_DATA / sizeof(long))];
		} fl;

		// 標準[文字のみ]
		struct FC{
			char data[SIZE_MSL_DATA];
		} fc;

		// 標準[数字&文字 混合] (最初の80byteが数値)
		struct MIX{
			long idata[20];
			char cdata[5][32];
			char ydata[SIZE_MSL_DATA-sizeof(long)*20-32*5];
		} mix;



		// システムログメッセージ
		struct {
			long	nSysNo;									// メッセージNo.
			WORD	date[6];								// 自PC時刻
			long	nPcKind;								// PCKIND
			char	cTaskName[SIZE_NAME_TASK];				// タスク名
			char	cPcName[SIZE_NAME_PC];					// ＰＣ名
															// メッセージ内容
			char	cMsg[SIZE_MSL_DATA-sizeof(long)*2-sizeof(WORD)*6-SIZE_NAME_TASK-SIZE_NAME_PC];	
		} syslog_que;
		// 機器状態変更メッセージ
		struct {
			long	nSysNo;									// メッセージNo.
			WORD	date[6];								// 自PC時刻
			long	nPcKind;								// PCKIND
			char	cTaskName[SIZE_NAME_TASK];				// タスク名
			char	cPcName[SIZE_NAME_PC];					// ＰＣ名
			long	nStateId;								// 機器状態ID
			long	nStateVal;								// 機器状態値
															// メッセージ内容
			char	cMsg[SIZE_MSL_DATA-sizeof(long)*4-sizeof(WORD)*6-SIZE_NAME_TASK-SIZE_NAME_PC];	
		} statelog_que;

		// 時刻設定要求
		struct {
			SYSTEMTIME systemtime;
			char reserve[SIZE_MSL_DATA-sizeof(SYSTEMTIME)];
		} timeset_que;

		// メールスロット転送要求
		struct {
			unsigned long nUdpIpAddr;								// このメールを受けたタスクが、UDPを打つ相手
																	// inet_addrした結果。表示するときは、inet_ntoa()とか使う。
			char cTaskName[SIZE_NAME_TASK];							// UDPを受けたタスクが、転送するタスク名
			char cPcName[SIZE_NAME_PC];								// UDPを受けたタスクが、転送するホスト名
			char cMsg[SIZE_MSL_DATA - sizeof(unsigned long) - SIZE_NAME_TASK - SIZE_NAME_PC];
		} mailtr_que;
	};

public:
	COMMON_QUE() {											// デフォルトコンストラクタ
		nEventNo = 0;
		nLineNo	 = 0;
		memset(fc.data, 0x00, sizeof(fc.data));
	}
};

// メール用アイテムセット一式 (あると便利なので追加)
struct MAIL_ITEM {
	char		cTask[SIZE_NAME_TASK];						// タスク名称
	char		cPc[SIZE_NAME_PC];							// PC名称
	COMMON_QUE	Que;										// 実データ
};

//=======================================
// 機器状態 基本構造体
//=======================================
// 個々の機器状態
struct TYP_STATUS_INF 
{
	int					id;									// 機器ID
	int					pcid;								// PCID (0:該当無し。機能に割り付き   1～：PCに割り付き)
	int					area;								// 異常範囲 (0:全体 1:表面 2:裏面)
	EM_DIV_KENSA		kensa;								// 検査異常度 (0:無害 1:縮退)
	EM_DIV_KIKI			kiki;								// 機器異常度 (0:無害 1:軽故障 2:中故障 3:重故障)
	int					yusen;								// 優先度 (1:最強 NULL=99999：最弱)
	char				msg[256];							// 異常時のメッセージ
	int					pcmode;								// PC区分 (0:オンライン  1:オフライン)

	int					stat;								// 現状態

};
// システムの機器状態
struct TYP_STATUS_NOWSYSTEM 
{
	// (オフラインPCは対象外)
	EM_DIV_KENSA		kensa[2];							// 表裏別 検査異常度 (0:無害 1:縮退 9:停止) DIV_KENSA
	EM_DIV_KIKI			kiki[2];							// 表裏別 機器異常度 (0:無害 1:軽故障 2:中故障 3:重故障) DIV_KIKI

	int					nCnt;								// 異常セット件数
	TYP_STATUS_INF		ngInf[5];							// 異常の機器情報上位5件
};

//=======================================
// 疵検ソケット通信用構造体 C++のみ
//=======================================
// トランザクションコード
#define T_CODE_NAME				"NM"						// [判定PC]-[統括PC]間通信 イニシャル伝文
#define T_CODE_DEFECT			"KZ"						// [判定PC]-[統括PC]間通信 疵情報伝文
															// [統括PC]-[操作PC]間通信 疵情報伝文
#define T_CODE_HELCY			"HL"						// [統括PC]-[操作PC]間通信 ヘルシー伝文

// 伝文サイズ
#define SIZE_SOCK_TC			2							// トランザクションコードサイズ
#define SIZE_SOCK_HEAD			256							// ソケット共通部ヘッダー部サイズ
#define SIZE_SOCK_DATA_MAX		2*1024*1024					// データ部の最大送信サイズ (2M) (このサイズ以上のデータ部を送信しようとするとライブラリ内で自動的に分割される)

// ヘッダー　共通部 [48byte]
struct SOCK_BASE_HEAD {
	char			code[SIZE_SOCK_TC];						// トランザクションコード
	short			subinf;									// サブ情報 (各PC間で意味合いが変化)
	char			pname[SIZE_NAME_PC];					// 送信元PC種別
	long			len;									// 伝文長(ヘッダー長＋データ長)
	long*			addr[2];								// データ部のアドレス (NULLでない場合は、使い終わった後に必ず deleteを行う)
															// (だから、使わない時でも必ずNULLにしておく事)
															// ※この値は、伝送されないので注意。
	// x86時は 16byte。x64時は 4byte の空きができる
	char			yobi[48-SIZE_SOCK_TC-sizeof(short)-SIZE_NAME_PC-sizeof(long)-sizeof(long*)*2 ];
};

// ダミーのヘッダー部 (ライブラリ等で使用する為のヘッダ構造体)
struct SOCK_F_HEAD {
	SOCK_BASE_HEAD	base_head;								// 共通ヘッダ部
	char			reserve[SIZE_SOCK_HEAD-sizeof(SOCK_BASE_HEAD)];	// 各PC間通信で可変ヘッダー部
};


//=======================================
// シスログNo
//=======================================
//// 名称								// No		// サブメッセージ形式		// メッセージ

// 全般 0
#define SYSNO_NON						0			//							// [メールスロット] No未割り当て

// メール関連 1～4
#define SYSNO_MAIL_SNED_ERR				1			// [通知名 err_code = %d]	// [メールスロット] 送信失敗		
#define SYSNO_MAIL_RECV_ERR				2			// [通知名 err_code = %d]	// [メールスロット] 受信失敗	
#define SYSNO_MAILNO_ERR				3			// [event = %d]				// [メールスロット] イベントコード不明
// 手動コメント関連 5～7
#define SYSNO_COMMENT_ALL				5			//							// 手入力(全般)
#define SYSNO_COMMENT_SOFT				6			//							// 手入力(ソフト)
#define SYSNO_COMMENT_HARD				7			//							// 手入力(ハード)


// ソケット 10～24
#define SYSNO_SOCK_SEND_TIMEOUT			10			// [通信名]					// [SOCKET] 送信タイムアウト
#define SYSNO_SOCK_NEW_ERR				11			// [err_code = %d]			// [SOCKET] 生成失敗
#define SYSNO_SOCK_SEND_ERR				12			// [err_code = %d]			// [SOCKET] 送信エラー
#define SYSNO_SOCK_CONNECT				13			// [通信名]					// [SOCKET] 接続完了
#define SYSNO_SOCK_CLOSE				14			// [通信名]					// [SOCKET] 切断
#define SYSNO_SOCK_RECV_ERR				15			// [section=%s err_code=%d]	// [SOCKET] 受信エラー
#define SYSNO_SOCK_LISTEN_ERR			16			// [section=%s err_code=%d]	// [SOCKET] Listenエラー
#define SYSNO_SOCK_CLIENT_MAX			17			//							// [SOCKET] クライアント数オーバー
#define SYSNO_SOCK_INITIALIZ			18			// [name=%s]				// [SOCKET] イニシャル伝文受信
#define SYSNO_SOCK_TC_NG				19			// [section=%s code=%4.4s]	// [SOCKET] トランザクションコード異常
#define SYSNO_SOCK_RECV_TIMEOUT			20			//							// [SOCKET] 受信タイムアウト
#define SYSNO_SOCK_CONNECT_ERR			21			// [err_code = %d]			// [SOCKET] 接続異常
#define SYSNO_SOCK_HELCY_TIMEOUT		22			// [通信名]					// [SOCKET] ヘルシー監視タイムアウト

// 共有メモリ 25～29
#define SYSNO_MEM_OPEN					25			// [共有メモリ名]           // [共有メモリ] オープン失敗
#define SYSNO_MEM_READ				    26			// [共有メモリ名]           // [共有メモリ] 読み込み失敗
#define SYSNO_MEM_WRITE					27			// [共有メモリ名]           // [共有メモリ] 書き込み失敗

// DB 30～39
#define SYSNO_DB_EXECUTEDIRECT_ERR		30			// [テーブル名]				// [DB] SqlExecuteDirectエラー	
#define SYSNO_DB_FETCH_ERR				31			// [テーブル名]				// [DB] SqlFetchエラー	
#define SYSNO_DB_CONNECT_ERR			32			// [テーブル名]				// [DB] SqlConnectエラー	
#define SYSNO_DB_DATA_ERR				33			// [テーブル名, 列=%d]		// [DB] データ不正	
#define SYSNO_DB_BINALYWRITE_ERR		34			// [テーブル名]				// [DB] バイナリ書込みエラー	
#define SYSNO_DB_ODBC_SET_OK			35			// [ODBC名, アドレス]		// [DB] ODBC設定書換完了
#define SYSNO_DB_ODBC_SET_ERR			36			// [ODBC名, アドレス]		// [DB] ODBC設定書換失敗

// RS232C 40～45
#define SYSNO_232C_NEW_ERR				40			// [err_code=%d]			// [232C] 生成失敗
#define SYSNO_232C_SEND_ERR				41			// [err_code=%d]			// [232C] 送信エラー
#define SYSNO_232C_RECV_ERR				42			// [err_code=%d]			// [232C] 受信エラー
#define SYSNO_232C_OPEN					43			// [通信名]					// [232C] 接続完了
#define SYSNO_232C_CLOSE				44			// [通信名]					// [232C] 切断

// PIPE 46～49
#define SYSNO_PIPE_OPEN					45			//							// [PIPE] 接続完了
#define SYSNO_PIPE_CLOSE				46			//							// [PIPE] 切断完了
#define SYSNO_PIPE_CONNECT_ERR			47			// [err_code=%d]			// [PIPE] 接続失敗
#define SYSNO_PIPE_EXEC					48			//							// [PIPE] 操作正常完了
#define SYSNO_PIPE_EXEC_ERR				49			// [err_code=%d]			// [PIPE] 操作異常

// 50～69 その他グループ
#define SYSNO_CNT_ERR					50			// [err_code=%d sub_code=%d]	// [カウンターボード] 異常発生
#define SYSNO_PIO_ERR					51			// [err_code=%d sub_code=%d]	// [PIOボード] 異常発生
#define SYSNO_AIO_ERR					52			// [err_code=%d sub_code=%d]	// [アナログボード] 異常発生
#define SYSNO_PLG_ERR					53			// [検査用=%0.3f 比較用=%0.3f 差=%d]	// [カウンターボード] PLG整合性異常

#define SYSNO_STATUS_OK					55			// [機器名]					// [機器状態監視] 機器障害復旧
#define SYSNO_STATUS_NG					56			// [機器名]					// [機器状態監視] 機器障害検出
#define SYSNO_EVTLOG_MSG_NG				57			// [メッセージ名]			// [イベントログ監視] 異常検出
#define SYSNO_EVTLOG_AP_NG				58			// [プロセス名]				// [イベントログ監視] アプリケーション異常検出
#define SYSNO_STATUS_WAR				59			// [機器名]					// [機器状態監視] 機器警告検出 (軽故障)			※プロビジョン

// 70～99 一般もろもろ
#define SYSNO_TASK_INIT_ERR				70			// [タスク名]				// [LIB] タスク初期起動処理失敗
#define SYSNO_INI_READ_ERR				71			// [セッション,キー]		// [INI] 取得エラー
#define SYSNO_PCNAME_GET_ERR			72			//							// [LIB] PC名称取得失敗
#define SYSNO_QUEFULL_ERR				73			// [区分名称]				// [LIB] 内部バッファキューフル！！
#define SYSNO_QUEFULL_WAR				74			// [区分名称]				// [LIB] 内部バッファキューフル

#define SYSNO_UPS_NG					80			//							// 電源異常
#define SYSNO_TIMESET_NG				81			// [err_code = %d]			// 時刻設定失敗	
#define SYSNO_RESTAET_ERR				82			// 							// 再起動失敗	
#define SYSNO_SHUTDOWN_ERR				83			//							// シャットダウン失敗
#define SYSNO_TIMESET_OK				84			//							// 時刻設定完了
#define SYSNO_DELETE_ERR				85			// 							// DB削除失敗
#define SYSNO_COMPRESS_ERR				86			//							// DB圧縮失敗
#define SYSNO_RESTAET_OK				87			// [PC名]					// PC再起動要求受付						
#define SYSNO_SHUTDOWN_OK				88			//							// システム終了要求受付
#define SYSNO_SYSTEM_START				89			// [開始条件]				// 検査開始要求
#define SYSNO_SYSTEM_STOP				90			//							// 検査停止要求
#define SYSNO_LOGOFF_ERR				91			// 							// ログオフ失敗	
#define SYSNO_LOGOFF_OK					92			// [PC名]					// PCログオフ要求受付						
#define SYSNO_DB_CHECK_RETRY			93			// [リトライ回数]			// データベース接続チェックで接続失敗。
#endif
  