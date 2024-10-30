// *********************************************************************************
//	当タスクの基本定数
//	[Ver]
//		Ver.01    2007/08/01  vs2005 対応
//
//	[メモ]
//		
// *********************************************************************************

#pragma once

#include "..\..\Include\LineCommon.h"									// 疵検共通ヘッダー
#include "..\..\Include\HyTbl.h"										// 表示用共有メモリヘッダー
#include "TrackingFrame.h"												// FrameNoでトラッキング可能とするクラス

// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// スイッチ

//// 重要な切り替えスイッチ
//--------
// 完全流用
#define DISABLE_GAP_CALC												// 複眼ズレ補正機能無効時 ※色差の場合は、必ず定義
#define DISABLE_TINY_CALC												// 微小欠陥機能無効時

//--------
// 今回特別仕様
//#define DISABLE_AUTOSTART												// タスク起動時、自動探傷開始が無効

//--------
// ちゃんと作った奴
//#define DEBUG_TEST													// テスト時
//#define DEBUG_HYPER_V_TEST												// HYPER-V環境テスト時

//--------
#define ENABLE_GATE_KEN												// 検査用ゲート信号
#define ENABLE_GATE_DSP												// 表示用ゲート信号

//--------
// デバッグ確認用(本番時、必ず全て無効にすること)
#define ENABLE_DEBUG_LOG												// 定義時、各所に仕込んだデバッグ用内部ログを出力
#define	DISABLE_ZZ_EPC													// 定義時、擬似エンジン(ZZ_EPC)を使用
//#define	DISABLE_LED_DAISYA												// 定義時、LED点灯消灯制御に台車位置を見ない
#define ENABLE_DEBUG_DLG												// デバッグ用のDLG表示

// デバッグ用ログ出力設定
#ifdef ENABLE_DEBUG_LOG
////随時追加予定
	//#define DEBUG_PRINT_SAVEDEFCHK										// 定義時、疵情報保存のフロー制御内容をログ出力
	//#define DEBUG_PRINT_SAVEMAXCHK										// 定義時、疵情報保存の上限確認結果をログ出力
	//#define DEBUG_LOG_EDGE_RESTRICT
	//#define DEBUG_LOG_EDGE
	#define DEBUG_LOG_EDGE_UDP
	#define DEBUG_PATO_LOG
	//#define DEBUG_LOG_HT_HEALTHY
#endif

//--------
#define ENABLE_RECORDER_UDP												// 調整用レコーダーへのコイル情報UDP送信　無効時、コメント化

#define ADDDBCOUNT_QUE_SIZE_CHANGE										// DB書込みキューサイズを変更(有効時:200000、無効時:5000)

//#define DISABLE_DSP_PLG													// 表示PLG入力無し　※表示PLG入力ありの場合、コメント化

//--------
// ソフト共通化対策
//#define PLG_DSP_MAP_FIX													// 疵マップ固定モード
#define HT_IS_COLOR														// 色差/輝度切替。  定義時：色差　未定義時：輝度
#define RESV_KOTEI														// 完全固定分解能

#define SENDOP_LENGTH_PLG												// PLGクラスにて定周期で通板位置情報送信時 アンコメント

//--------
#define DSP_SOGYO_CHECK												// 操業管理クラスにて表示用距離管理時アンコメント


//=======================================
// 調査用ログ定義
// 使用する場合は、
//   1:mcls_pLogD をメンバー変数に追加している事
//   2:コンストラクタでNULLをちゃんとセットする事
// 使用例:  LOGD(em_MSG), "test_%d_%d", wk, wk );
//=======================================
#define LOGD(t)  if(mcls_pLogD!=NULL)       mcls_pLogD->Write(KizuLib::LogFileManager::t		// ログ出力
#define pLOGD(t) if(p->mcls_pLogD!=NULL) p->mcls_pLogD->Write(KizuLib::LogFileManager::t		// ログ出力(ポインタ版)

#define LOGG(t)  if(mcls_pLogG!=NULL)       mcls_pLogG->Write(KizuLib::LogFileManager::t		// ガイダンス用ログ出力
#define LOGSEQ(t) if(mcls_pLogSeq!=NULL)    mcls_pLogSeq->Write(KizuLib::LogFileManager::t		// シーケンサ受信用ログ出力
#define LOGRN(t) if(mcls_pLogRn!=NULL)   	 mcls_pLogRn->Write(KizuLib::LogFileManager::t		// 疵連携用ログ出力
#define LOGRN_ARR(t, id) if(mcls_pLogRn[id-1]!=NULL)   	 mcls_pLogRn[id-1]->Write(KizuLib::LogFileManager::t	// 疵連携用ログ出力(配列版)
#define LOGE(t) if(mcls_pLogE!=NULL)    mcls_pLogE->Write(KizuLib::LogFileManager::t	// エッジUDP用ログ出力

#define LOGA(t)  if(mcls_pLogA!=NULL)       mcls_pLogA->Write(KizuLib::LogFileManager::t		// 警報ログ出力
#define pLOGA(t) if(p->mcls_pLogA!=NULL) p->mcls_pLogA->Write(KizuLib::LogFileManager::t		// 警報ログ出力(ポインタ版)


// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 固有定数

//=======================================
// INIキー
//=======================================

#define INI_HT_RECV_TOP		"HT_RECV_TOP"								// 表判定機能通信定義 Iniキー
#define INI_HT_RECV_BOT		"HT_RECV_BOT"								// 裏判定機能通信定義 Iniキー

//#define INI_CY_RECV_TOP		"CY_RECV_TOP"								// 表周期機能通信定義 Iniキー
//#define INI_CY_RECV_BOT		"CY_RECV_BOT"								// 裏周期機能通信定義 Iniキー


#define INI_OP_SEND			"OP_SEND"									// オペレータ機能通信定義 Iniキー



//=======================================
// 定数定義
//=======================================
//// 異常区分
enum EM_ON_ERRWAR {
	// CoilManager
	ON_QUE_COIL_INF = 0,											// コイル情報登録時キューフル
	ON_QUE_COIL_RESULT,												// コイル実績登録時キューフル
	ON_QUE_COIL_RESULT_UPDATE,										// コイル実績更新時キューフル

	// DBManager
	ON_SAVE_COIL_INF,												// コイル情報登録時キューフル							
	ON_SAVE_COIL_RESULT,											// コイル実績登録時キューフル
	ON_SAVE_DEFECT_INF,												// 疵実績登録時キューフル

	// MainInstance
	ON_QUE_CAMLINKSW,												// カメラリンクスイッチ設定登録時キューフル
	
	// SeqFrameRecv
	ON_SEQ_FRAME_SOCK_CLOSE,										// 切断認識 

	// PlgManager
	ON_PLG_FRAME_DELETE,											// 受信フレーム消失

	// HtRecvBaseManager
	ON_SEQ_FRAME_DELAY,												// シーケンサフレーム受信遅れ

	// 外的要因
	ON_DB_ACCESS_ERR												// DB接続失敗
};

//// GroupID
enum ENUM_GROUP_DIV
{
	DIV_HANTEI01 = 1,												// 判定PC1
	DIV_HANTEI02,													// 判定PC2
	DIV_HANTEI03,													// 判定PC3
	DIV_HANTEI04,													// 判定PC4
	DIV_LOGIPARA,													// ロジックパラランPC
	DIV_GROUP_MAX = DIV_LOGIPARA
};
static CString GetHanteiKind(int em)
{
	CString sWk;

	switch (em)
	{
		case DIV_HANTEI01:	sWk = "判定グループ1";		break;
		case DIV_HANTEI02:	sWk = "判定グループ2";		break;
		case DIV_HANTEI03:	sWk = "判定グループ3";		break;
		case DIV_HANTEI04:	sWk = "判定グループ4";		break;
		case DIV_LOGIPARA:	sWk = "パララングループ";	break;
		default:			sWk = "バックアップ";		break;
	}
	return sWk;
}

#define	PLS_RATE_TANI				10000.0						// パルスレート単位(制御シーケンサからの入力は整数値)


//=======================================
// 疵情報ファイル定義
//=======================================
#define	DEF_DUMP_HEAD				"日付,管理No,出側管理No,表裏区分,疵連番,判定疵種,判定グレード,重複結果,カメラセット,長手位置,出側長手位置,DS位置,WS位置,DSWS区分,疵長さ,縦分解能,横分解能,フレームNo,フレーム外接Xmin,フレーム外接Xmax,フレーム外接Ymin,フレーム外接Ymax,検査パターンコード"


//=======================================
// 表示基点以降を入側単位でトラッキング管理
//=======================================
static const int MAX_POS_INDEX = 8;									// 各位置でのトラッキング用

// 入側単位の測長位置 (表示位置順にセット)
enum EM_POS_DSP {

	//// EM_DIV_DISP の個別部分と合わせておくこと

	// システム全体での管理対象 (MAX_DISP_POS分)
	POS_DSP_P1,														// 検査台1位置
	POS_DSP_P2,														// 検査台2位置（検査台1位置と同じ画面を表示）
//	POS_DSP_P3,														// 検査台3位置
//	POS_DSP_P4,														// 検査台4位置
//	POS_DSP_P5,
	POS_DSP_P_END,													// 検査台 位置終端

	// 統括管理内だけ
	POS_DSP_DSP1_P = POS_DSP_P1,									// [表示位置] 表示基点1 基準の各検査台開始位置
	POS_DSP_DSP2_P = POS_DSP_P2,									// [表示位置] 表示基点2 基準の各検査台開始位置
	POS_DSP_SHR_I = POS_DSP_P_END,									// シャー位置

	//----- ここまで半固定
	POS_DSP_END,													// 最後に使用
	
};
//
//// 入側単位の測長位置 (上流面) (バラバラにセットOK)
//enum EM_POS_KEN_U {
//	POS_KEN_U_END = 0												// 最後に使用
//};
//// 下流
//enum EM_POS_KEN_D {
//	POS_KEN_D_END = 0												// 最後に使用
//};
//
//// 入側単位の測長位置 (上流面) (バラバラにセットOK)
//enum EM_POS_DSP_U {
//	POS_DSP_U_SPM = 0,												// SPM
//	POS_DSP_U_END													// 最後に使用
//};
//// 下流
//enum EM_POS_DSP_D {
//	POS_DSP_D_SPM = 0,												// SPM
//	POS_DSP_D_END													// 最後に使用
//};

// シーケンサ状態
enum EM_SEQ_STAT {
	SEQ_STAT_INIT=-1,				// 初期値
	SEQ_STAT_OK=0,					// 正常
	SRQ_STAT_WAR,					// 注意
	SEQ_STAT_ERR,					// 異常
};

// シーケンサ稼動状態
enum EM_SEQ_KADOU {
	SRQ_KADOU_INI=-1,				// 初期値
	SEQ_KADOU_RESET=0,				// リセット済み 
	SEQ_KADOU_INIT,					// 初期化済み
	SEQ_KADOU_YOBI,					// 予備
	SEQ_KADOU_START					// 撮像
};

// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 簡易共通関数


// タイマー起動		nTime=[ms]
static void SetEvWaitableTimer(HANDLE evTimer, int nTime, int nCycle = 0)
{
	LARGE_INTEGER ts;
	ts.QuadPart = (__int64)nTime * -10000;	// 1[ms]→100[ns] (マイナス指定で、指定時間経過でシグナル) (第3引数は2回目以降の起動間隔)
	SetWaitableTimer(evTimer, &ts, nCycle, NULL, NULL, FALSE);
}


// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// フレームNoトラッキング 構造体

// フレームNoトラッキング情報
struct TR_FRAME : public TR_FRAME_BASE {
	int			nCoilChg[NUM_MEN];									// コイル切替フラグ
	int			nSpeed;												// ライン速度[mm/sec]
	int			nFCut;												// 出側物理コイル ファーストカット 切り替えフラグ
	int			nRecProcTime[2];									// 1フレーム取込完了時のQueryPerformanceCounter
};
#define	MAX_TR_FRAME		2										// トラッキング数(表裏 単発のみ)

static int	GetTrId(int nHtCy, int nTorB) { return (((nHtCy - DIV_HANTEI01) * NUM_MEN) + nTorB); }	// フレームトラッキングID取得
static int	GetTrOtherId(int nTrId) { return (0 == (nTrId % NUM_MEN) ? nTrId + 1 : nTrId - 1); }	// 他方面のフレームトラッキングID取得

// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 警報出力 構造体

// チェック用情報
struct OUTPUT_ITEM {
	bool		bUmu;												// セット有無
	int			nMen;												// 対象面 (0:表 1:裏)
	int			nKizuNo;											// 疵No
	int			nTid;												// 疵種
	int			nGid;												// グレード
	int			nTYuu;												// 疵種優先度
	int			nGYuu;												// グレード優先度
	float		dAria;												// 面積
	long		nLen;												// 長手位置 [mm]
	int			nWidDiv;											// 幅分割位置 (0オリジン)
	int			nDs;												// DSからの距離 [mm]
	int			nWs;												// WSからの距離 [mm]
	bool		bPatoFlg[MAX_DISP_POS];								// 疵接近ボイス出力済フラグ(True:出力済)[各検査台位置ごと]
	int			cycle_div;											// 周期疵区分(0:一般疵, 1:簡易周期疵,連続疵 )

	int			nKind;												// 警報種類	
};


//=======================================
// 欠陥警報出力用 内部メッシュ 構造体
//=======================================
// 欠陥警報出力判定用
#define OUTPUT_MESYU_SEKKIN_ROW		500								// 疵接近 のメッシュ [mm]
#define OUTPUT_MESYU_SEKKIN_MAX		(COIL_LEN_MAX*1000/OUTPUT_MESYU_SEKKIN_ROW)		// 疵接近 のメッシュ 行数
#define OUTPUT_MESYU_SEKKIN_COL		LAMP_COL_NUM					// 疵接近ランプ分割数
#define OUTPUT_MESYU_VOIC_ROW		500								// 微小疵や表裏面疵 判定ボイスのメッシュ [mm]
#define OUTPUT_MESYU_VOIC_COL		50								// [mm]
#define OUTPUT_MESYU_ROW			COIL_LEN_MAX*1000/OUTPUT_MESYU_VOIC_ROW	// ボイス出力判定用 内部メッシュ行数 (500mm間隔)
#define OUTPUT_MESYU_COL			COIL_WID_MAX/OUTPUT_MESYU_VOIC_COL		// ボイス出力判定用 内部メッシュ列数 (50mm間隔)
#define OUTPUT_VOICE_RE_ROW			5								// 連続欠陥 連続何個続いたら該当欠陥とするか
#define OUTPUT_ALARM_COL			ALARM_COL_NUM					// 疵接近アラーム分割数


////=========================================
//// ボイス出力条件
#define OUTPUT_VOICE_ERR_NUM		em_VOICE_ERR_END			 		// 設備異常ボイス数
#define OUTPUT_VOICE_DEF_NUM		em_VOICE_DEF_END - em_VOICE_ERR_END	// 欠陥検出ボイス数
#define OUTPUT_VOICE_NUM			em_VOICE_DEF_END					// ボイス出力条件数

enum ENUM_VOICE_ERR_TYPE {
	em_VOICE_ERR_NON = -1,											// 無し

	em_VOICE_ERR_KEN,												// 「疵検異常です。」
	em_VOICE_ERR_RECV_COILINF,										// 「コイル情報が受信できません。」
	em_VOICE_ERR_SEND_ZISEKI,										// 「疵検実績送信異常です。」
	em_VOICE_ERR_LIGHT,												// 「照明装置異常です。」
	em_VOICE_ERR_CAM,												// 「カメラ異常です。」
	em_VOICE_ERR_TEMP,												// 「温度異常です。」
	em_VOICE_ERR_END,

	em_VOICE_DEF_TOP = em_VOICE_ERR_END,							// 表
	em_VOICE_DEF_BOT,												// 裏
	em_VOICE_DEF_HE,												// ヘゲ
	em_VOICE_DEF_SU,												// スリバ
	em_VOICE_DEF_RE,												// 連続疵
	em_VOICE_DEF_JU,												// 重大疵
	em_VOICE_DEF_SY,												// 周期疵
	em_VOICE_DEF_BI,												// 微小疵
	em_VOICE_DEF_OT,												// 疵接近
	em_VOICE_DEF_TB,												// 表裏同一疵
	em_VOICE_DEF_END
};

//=========================================
// 欠陥検出ボイス (優先順位順。一番下が最強)
#define OUTPUT_VOICE_PRI_NUM		em_VOICE_PRI_END				// 優先対象の欠陥検出ボイス数
enum ENUM_VOICE_PRI_TYPE {
	em_VOICE_PRI_NON = 0,											// 無し
	em_VOICE_PRI_OT,												// 疵接近
	em_VOICE_PRI_BI,												// 周期疵 単発
	em_VOICE_PRI_SY,												// 周期疵
	em_VOICE_PRI_TB,												// 表裏同一疵
	em_VOICE_PRI_SU,												// スリバ
	em_VOICE_PRI_HE,												// ヘゲ
	em_VOICE_PRI_JU,												// 重大疵
	em_VOICE_PRI_RE,												// 連続疵
	em_VOICE_PRI_END
};

// 優先度順欠陥検出ボイス条件からDMアドレス順ボイス条件を取得
static ENUM_VOICE_ERR_TYPE GetVoiceDefType(ENUM_VOICE_PRI_TYPE emVoiceDefType) {
	switch (emVoiceDefType) {
		case em_VOICE_PRI_NON:	return em_VOICE_ERR_NON;		// 無し
		case em_VOICE_PRI_OT:	return em_VOICE_DEF_OT;			// 疵接近
		case em_VOICE_PRI_BI:	return em_VOICE_DEF_BI;			// 微小疵
		case em_VOICE_PRI_SY:	return em_VOICE_DEF_SY;			// 周期疵
		case em_VOICE_PRI_TB:	return em_VOICE_DEF_TB;			// 表裏同一疵
		case em_VOICE_PRI_SU:	return em_VOICE_DEF_SU;			// スリバ
		case em_VOICE_PRI_HE:	return em_VOICE_DEF_HE;			// ヘゲ
		case em_VOICE_PRI_JU:	return em_VOICE_DEF_JU;			// 重大疵
		case em_VOICE_PRI_RE:	return em_VOICE_DEF_RE;			// 連続疵
		default:				return em_VOICE_DEF_END;
	}
};

// DMアドレス順ボイス条件から優先度順欠陥検出ボイス条件を取得
static ENUM_VOICE_PRI_TYPE GetVoicePriType(ENUM_VOICE_ERR_TYPE emVoiceDefType) {
	switch (emVoiceDefType) {
		case em_VOICE_ERR_NON:	return em_VOICE_PRI_NON;		// 無し
		case em_VOICE_DEF_OT:	return em_VOICE_PRI_OT;			// 疵接近
		case em_VOICE_DEF_BI:	return em_VOICE_PRI_BI;			// 微小疵
		case em_VOICE_DEF_SY:	return em_VOICE_PRI_SY;			// 周期疵
		case em_VOICE_DEF_TB:	return em_VOICE_PRI_TB;			// 表裏同一疵
		case em_VOICE_DEF_SU:	return em_VOICE_PRI_SU;			// スリバ
		case em_VOICE_DEF_HE:	return em_VOICE_PRI_HE;			// ヘゲ
		case em_VOICE_DEF_JU:	return em_VOICE_PRI_JU;			// 重大疵
		case em_VOICE_DEF_RE:	return em_VOICE_PRI_RE ;		// 連続疵
		default:				return em_VOICE_PRI_NON;
	}
};

static CString GetVoiceTypeName(int emVoiceDefType)
{
	switch (emVoiceDefType) {
	case em_VOICE_ERR_KEN:				return "φωERR";		// 「疵検異常です。」
	case em_VOICE_ERR_RECV_COILINF:		return "ｺｲﾙ情報ERR";	// 「コイル情報が受信できません。」
	case em_VOICE_ERR_SEND_ZISEKI:		return "実績送信ERR";	// 「疵検実績送信異常です。」
	case em_VOICE_ERR_LIGHT:			return "照明ERR";		// 「照明装置異常です。」
	case em_VOICE_ERR_CAM:				return "ｶﾒﾗERR";		// 「カメラ異常です。」
	case em_VOICE_ERR_TEMP:				return "温度ERR";		// 「温度異常です。」
	case em_VOICE_ERR_NON:				return "";				// 無し
	case em_VOICE_DEF_OT:				return "疵接近";		// 疵接近
	case em_VOICE_DEF_BI:				return "微小";			// 微小疵
	case em_VOICE_DEF_SY:				return "周期";			// 周期疵
	case em_VOICE_DEF_TB:				return "表裏";			// 表裏同一疵
	case em_VOICE_DEF_SU:				return "ｽﾘﾊﾞ";			// スリバ
	case em_VOICE_DEF_HE:				return "ﾍｹﾞ";			// ヘゲ
	case em_VOICE_DEF_JU:				return "重大";			// 重大疵
	case em_VOICE_DEF_RE:				return "連続";			// 連続疵
	default:							return "";
	}

}

//=========================================
// ボイス出力 状態
enum ENUM_VOICE_STAT {
	em_VOICE_STAT_OFF = 0,											// 停止
	em_VOICE_STAT_ON,												// 出力
};

//=========================================
// パトライト設置位置
#define OUTPUT_PATO_POS_NUM			em_PATO_POS_END					// パトライト設置位置の数
enum ENUM_PATO_POS_TYPE {
	em_PATO_POS_KENSA = 0,											// 検査台
	em_PATO_POS_UNTEN,												// 運転室
	em_PATO_POS_END													// 最後
};

//=========================================
// パトライト種類
#define OUTPUT_PATO_KIND_NUM		em_PATO_KIND_END				// パトライト種類の数
enum ENUM_PATO_KIND_TYPE {
	em_PATO_KIND_RAMP = 0,											// ランプ
	em_PATO_KIND_BUZZ,												// ブザー
	em_PATO_KIND_END												// 最後
};

//=========================================
// 疵接近ランプ種類
#define OUTPUT_RAMP_NUM		em_RAMP_END								// 疵接近ランプの数
enum ENUM_RAMP_TYPE {
	em_RAMP_NON = -1,												// 非対象(=ランプ出力無し)
	em_RAMP_KEI,													// 軽欠陥検出
	em_RAMP_JYU,													// 重欠陥検出
	em_RAMP_END
};

//=========================================
// 疵接近ランプ状態
enum ENUM_RAMP_STAT {
	em_RAMP_STAT_OFF = 0,											// 停止
	em_RAMP_STAT_ON,												// 出力
};

//=========================================
//// 欠陥警報情報
// 欠陥警報出力条件
struct ALARME_INF
{
	// 構造体
	OUTPUT_ITEM		ranp[NUM_MEN][OUTPUT_MESYU_SEKKIN_MAX][OUTPUT_MESYU_SEKKIN_COL];			// 疵接近ランプの状態
	OUTPUT_ITEM		voice[NUM_MEN][OUTPUT_MESYU_ROW];											// ボイス出力
	bool			voic_re[NUM_MEN][OUTPUT_MESYU_ROW][OUTPUT_MESYU_COL];						// 連続疵判断用		(センター基準) (コイル単位でスライドさせながらチェックしていく)
	bool			voic_tb[NUM_MEN][OUTPUT_MESYU_ROW][OUTPUT_MESYU_COL];						// 表裏同一疵判断用	(センター基準) (コイル単位)
};

// 警報出力結果受け渡し用
struct ALARME_OUTPUT {
	ENUM_RAMP_STAT	ranp[NUM_MEN][OUTPUT_MESYU_SEKKIN_COL][OUTPUT_RAMP_NUM];	// 疵接近ランプの状態
	ENUM_RAMP_STAT	alarm[NUM_MEN][OUTPUT_MESYU_SEKKIN_COL][OUTPUT_RAMP_NUM];	// 疵接近アラームの状態
	int				pato[OUTPUT_PATO_POS_NUM][OUTPUT_PATO_KIND_NUM];			// パトライトの状態
	ENUM_VOICE_STAT	voice[OUTPUT_VOICE_NUM];									// ボイスの状態
};

// パトライト用情報 (リセットPB対応)
struct PATO_INF
{
	// 疵特定キー
	char					cKizukenNo[SIZE_KIZUKEN_NO];			// 疵検管理No
	int						nMen;									// 表裏
	int						nKizuSeq;								// 疵連番
	int						rec;									// 1m間隔のレコード (警報出力用の内部メッシュの配列位置)

	long					nY;										// 長手位置 (疵中心位置) [mm] 

																	// 状態
	bool					bRun;									// パトライト実行中 (true:実行対象(パトライト出力開始位置を過ぎた疵)  false:対象外 )
};

//=======================================
// 出側物理コイル単位の状態関連
//=======================================
//// トラッキングイベント区分
enum EM_TR_KEY_COILOUT_ST {
	TR_KEY_KENSA_TOP = 1,											// 検査状態 表
	TR_KEY_KENSA_BOT,												// 検査状態 裏
	TR_KEY_KIKI_TOP,												// 機器状態 表
	TR_KEY_KIKI_BOT,												// 機器状態 裏
};


// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 全コイル情報 構造体


//======================================================
// 入側論理コイル用
#define SIZE_COIL_ROWVAL			1								// 行情報  0:特になし


#define SIZE_COIL_BUF				11								// 検査中/検査完了用の入側論理コイル情報のバッファ
struct	COIL_INF
{
	//// 基本
	int						rec;									// 自分自身の配列位置 (0オリジン)

	//// 上位情報
	COIL_BASEDATA			data;									// システム共通コイル情報
	PRO_TYPE_DATA_COILINF	data_u;									// コイル情報
	char					cCoilKey[SIZE_COIL_KEY];				// キー情報
	double					dSpmHosei;								// SPM補正率（1.0 + SPM伸び率[10^2%]）
	
	//// 命令時に確定
	char					cKizukenNo[SIZE_KIZUKEN_NO];			// 管理No
	bool					bDummeyCoil;							// ダミーコイルか (true:ダミーコイル)
	bool					bCoilInit;								// 検査開始１本目コイルか (true:検査開始１本目)
	COIL_SETTING			setting;								// φωで使うコイル基本情報
	int						nMapCol;								// マップの幅数 (コイル幅より算出。不可能時は、最大。よく使うから持たせておく)
	int						nHtID[NUM_MEN][NUM_HANTEI];				// 使用判定PC (その検査で使用した判定PCID)
	int						nRpiID[NUM_MEN][NUM_RPIEXEC];			// 使用全長処理PC (その検査で使用した全長処理PCID)
		
	//// コイル切替時に確定
	int						nStartWpdOk;							// コイル先端が検査基点信号(0:正常WPD  1:異種点  2:ゲート信号)
	int						nCoilCnt;								// 検査開始からx本目のコイル（検査開始1本目が0。以降論理コイル切替のタイミングで+1）
	// ===>> 坂巻 2021.06.03 小コイル長材通販時、コイル長比較が正常にできない不具合修正
	double					dWpdOnLength;							// 検査用WPDON時のシーケンサ管理側コイル長
	// <<===
	
	//// 検査中に随時更新
	int						nSt_Hantei[NUM_MEN];					// 判定状態 表裏	 (0:正常 1:異常)
	int						nSt_Light[NUM_MEN];						// 照明装置状態 表裏
	int						nSt_CamControl;							// カメラコントロール回路
	int						nSt_CamSwitch;							// カメラリンクスイッチ回路
	int						nSt_Procon;								// プロコン状態
	int						nSt_FF;									// FF状態
	int						nSt_Sqence;								// シーケンサ状態
	int						nSt_SqeDm;								// データメモリ状態
	int						nSt_SqeUdp;								// シーケンサUDP状態
	int						nSt_Daisya[NUM_MEN];					// カメラ台車状態[0:表 1:裏]
	int						nSt_DbSkip[NUM_MEN];					// DB未保存
	int						nSt_FrameSkip[NUM_MEN];					// フレームスキップ
	int						nKizuNoNum;								// 疵Noのベースとなる。
	int						nSt_RpiSave[NUM_MEN];					// 全長画像保存状態
	int						nSt_ToyuSplr;							// 塗油スプレー装置状態


	// 位置情報関連
	EM_DIV_KENSA 			emKensa[NUM_MEN][MAP_ROW_NUM];			// 検査状態 (5mピッチ) [←PlgManager]
	EM_DIV_KIKI				emKiki[NUM_MEN][MAP_ROW_NUM];			// 機器状態 (5mピッチ) [←PlgManager]
	//int						nDstyCycle[NUM_MEN][MAP_ROW_NUM];		// 密度周期
	int						nRowVal[NUM_MEN][MAP_ROW_NUM][SIZE_COIL_ROWVAL];	// 付加情報 (5mピッチ) 
	float					dEdge[NUM_MEN][MAP_ROW_NUM][2];			// エッジ位置 (5mピッチ) [←EdgePosControl]

	int						nSetRec_St[NUM_MEN];					// 検査状態・機器状態の最終セット位置 (1オリジン)
	int						nSetRec_Edge[NUM_MEN];					// エッジ位置の最終セット位置(1オリジン)
	int						nSetRec_Last[NUM_MEN];					// 位置情報の付加データ(一番遅くセットするデータ)の最終セット位置 (1オリジン)
	int						nLastRec_Postion;						// 最終書き込み位置 (1オリジン)
	
	// 欠陥警報関連
	bool					bAlarmCancel;							// 該当コイルのみ欠陥警報禁止 (true:禁止)
	ALARME_INF				alarme;									// 警報出力用欠陥情報

		
	// 検査位置通板完了時に確定
	int						nEndMode;								// 実績格納区分		 (1:通常切替 2:バックアップ切替 3:手動停止 4:強制停止 0:不定またはまだ通板中)
	long					nLength;								// φω実測長 （mm）
	long					nLengthSub;								// 検査比較実測長 (mm)
	bool					bCoilResultAddDb;						// コイル実績をDBに登録完了済み時 true

	EM_DIV_UNTEN 			emUnten;								// 運転状態
	EM_DIV_KENSA			emOneKensa[NUM_MEN];					// 検査状態 (一区間でもその値が有った場合)
	EM_DIV_KIKI				emOneKiki[NUM_MEN];						// 機器状態 (一区間でもその値が有った場合)
	EM_DIV_PLG				emPlg;									// PLG状態

	int						nAgcVal[NUM_CAMERA];					// 露光時間 (そのコイルの落ち着いた時の露光時間。次コイル用の検査用WPD ON時に確定 = 今通板中コイル尾端の方なら露光時間も落ち着いているはず)
	int						nBrVal[NUM_CAMERA];						// 輝度 (そのコイルの落ち着いた時の輝度。次コイル用の検査用WPD ON時に確定 = 今通板中コイル尾端の方なら輝度も落ち着いているはず)
	int						nGain[NUM_CAMERA];						// ゲイン（そのコイルの落ち着いた時の輝度。次コイル用の検査用WPD ON時に確定 = 今通板中コイル尾端の方なら輝度も落ち着いているはず）
	int						nValue[NUM_MEN];						// 調光値（そのコイルの落ち着いた時の輝度。次コイル用の検査用WPD ON時に確定 = 今通板中コイル尾端の方なら輝度も落ち着いているはず）
	
	// 出側で締めるやつ。(=アップデートの可能性有り)
	long					nDspLength;								// 表示実測長 [mm]
	long					nOutCoilPos;							// 出側単位先端位置 [mm]
	int						nCutCount;								// カット回数
	int						nCutLen[MAX_SHACUT_COUNT];				// 疵検で管理するカット位置 [mm]

	// 表示時に確定
	int						nSt_Spm;								// SPM圧下有無　0：開放状態 1：圧下状態
	bool					bDspOk;									// 表示基点を通過時 true

	// 検査基点WPD ON時の上流カメラ位置通過コイル
	long					nLen_Yotei;								// 予定コイル長 [mm]

	int						nDefCnt[NUM_MEN][MAP_ROW_NUM];			// 最小区間毎の疵検出数
};

//======================================================
//// 出側コイル用
#define SIZE_COIL_OUT_BUF			1								// 出側物理コイル情報のバッファ

struct	COIL_OUT_INF
{
	//// 基本
	//int						rec;									// 自分自身の配列位置（0オリジン）


	//char					cOutKizukenNo[SIZE_KIZUKEN_NO];			// 出側管理No
	//int						nDivResult;								// 実績格納区分(1:通常切替,2:バックアップ切替,3:手動停止,4:強制停止)
	char					cID[SIZE_SERIAL_FF_NO];					// ID
	int						nCnt;									// 今何個ある？
	//char					cKizukenNo[SIZE_KIZUKEN_NO];			// 最内周入側管理No(基本は[0]を最内周管理Noとして採用)
	//char					cOutCoilNo[PRO_SIZE_COIL_NO+2];			// 出側コイルNo(分割時は子番付与)
	
	//int						nLength;								// 検査実測長 [mm]
	//int						nFCutLen;								// F部端板カット量 [mm]
	//int						nTCutLen;								// T部端板カット量 [mm]
	//int						nCutLen;								// 端板カット量 [mm]
	
	//EM_DIV_UNTEN			emUnten;								// 運転状態
	//EM_DIV_KENSA			emOneKensa[NUM_MEN];					// 検査状態 (一区間でもその値が有った場合)
	//EM_DIV_KIKI				emOneKiki[NUM_MEN];						// 機器状態 (一区間でもその値が有った場合)


	// 入側論理単位の情報
	struct INCOIL {
		char				cKizukenNo[SIZE_KIZUKEN_NO];			// 入側管理No
		int					nInCutNum;								// 入側小番 (1～)	※ 入側論理コイル内の何分割目か
		int					nFront;									// 先頭位置 [mm]	※ 入側論理コイル先頭から何mmか
		int					nLength;								// 長さ [mm]
		//int					nWidth;									// 材料板幅[0.1mm]
		int					nOutPos;								// 出側距離位置 [mm]※ 出側物理コイル先頭から何mmか
		//int					nEnable;								// 子番割付有効(0:有効 0以外:無効(999:尾端部, 998:先端部))
		//char				cInCoilNo[PRO_SIZE_COIL_NO+2];			// 入側コイルNo(分割時は子番付与)
		//COIL_SETTING		setting;								// コイル基本設定

		//COIL_INF*			pCoilIn;								// 入側論理コイル情報のポインタ
		
		int						nSt_Spm;							// SPM圧下有無
		double				dSpmHosei;								// SPM圧下補正率
	} incoil[MAX_SHALINK_COUNT];									// MAX出側合成4コイル(念のため、倍のエリアを保持する)
};


//// シャーカット
struct SHRCUT_ITEM {
	char					cKizukenNo[SIZE_KIZUKEN_NO];			// 管理No
	double					dPos;									// 入側単位の長手位置 [mm]
	//double					dPosOut;								// 出側単位の長手位置 [mm]
};


// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// DB保存用 構造体

//======================================================
// DB保存用 定義

//---------------------->>
// 流用クラス内で使う場合は、こちらに定義
enum ENUM_DB_KUBUN {
	em_STATUS_LOG = 0,												// ステータスログ
	em_DB_KUBUN_END													// 終わり (使っている)
};
// StatusMgr と DBMgr で循環依存関係になった対策
//// 受け渡し構造体
struct DELIVERY_DB {
	ENUM_DB_KUBUN		kubun;										// どの構造体か
	BYTE*				data;										// 構造体のポインタ
};
// ---------------------

//======================================================
// 完全流用 関係

//// 機器状態ログ用
struct DBDATA_STATUS_LOG
{
	EM_DIV_KADOU			emKadou;								// 稼動状態
	EM_DIV_KENSA			emKensa[NUM_MEN];						// 検査状態
	EM_DIV_KIKI				emKiki[NUM_MEN];						// 機器状態
	char					cMsg[256];								// ガイダンス

																	// 拡張分
	char					cKizukenNo[SIZE_KIZUKEN_NO];			// 疵検管理No
	char					cCoilNo[32];							// コイルNo

																	// 主キー (共通クラスでセット)
	CTime					time;									// 登録日付
	int						nAutoNo;								// 連番
};


//======================================================
// コイル情報 (物理コイル)
struct DBDATA_COIL_INF
{
	char			cKizukenNo[SIZE_KIZUKEN_NO];		// 管理No
	int				scode[NUM_MEN];						// 検査表面状態 (表/裏)(1オリジン)
	int				pcode[NUM_MEN];						// 検査パターン (表/裏)(1オリジン)

	//-------------------- プロコンコイル情報
	char			cCoilKey[SIZE_COIL_KEY];			// キー情報
	char			cMeiNo[SIZE_MEI_NO];				// 命令No
	char			cCoilNo[PRO_SIZE_COIL_NO];			// コイルNo
	long			nSendPoint;							// 送信位置 (0:WPD3 1:WPD4 2:WPD6-50m)
	long			nLenIn;								// 入側論理コイル 長さ [m]
	long			nAtuZai;							// 材料板厚 [μm] (入側)
	long			nWidZai;							// 材料板幅 [mm]
	long			nAtuSei;							// 注文板厚 [μm] (出側)
	long			nWidSei;							// 注文板幅 [mm]
	long			nWidTrim;							// 現工程トリム幅 [mm] (ノートリム時は、0)
	long			nNobiSPM;							// SPM伸び率 [10-2%]
	long			nNobiTLV;							// TLV伸び率 [10-2%]
	long			nCutF;								// 入側におけるフロント端板カットの実績長[mm]
	long			nCutT;								// 入側におけるテール端板カットの実績長[mm]
	long			nSPMRoolKei_Up;						// SPMワークロール径 (上) [10-1mm]
	long			nSPMRoolKei_Down;					// SPMワークロール径 (下) [10-1mm]

	char			cKenKijun_O[4];						// 検査基準 表
	char			cKenKijun_U[4];						// 検査基準 裏
	char			cKurometo_Kind[2];					// クロメート 実績種類
	char			cKurometo_Umu[2];					// クロメート 実績有無
	char			cHinCode[1];						// 品種
	char			cMekkiKind[1];						// メッキ種類
	char			cSyuZai[2];							// 主材質
	char			cDifMaker[1];						// ディファレンシャルマーカー
	char			cKenban[1];							// 原板区分
	char			cMekkiCycle[1];						// メッキサイクル
	char			cInLineSPM[1];						// レベラー インラインSPM
	char			cKagakuToyu[1];						// 化学処理塗油
	char			cAtukkaSPM[1];						// オフラインSPM圧下率
	char			cMekkiKubun[1];						// メッキ区分
	char			cOmoteSyori1[1];					// 表面処理1桁目
	char			cOmoteSyori2[1];					// 表面処理2桁目
	char			cToyu[1];							// 塗油
	char			cKouteiYobi[1];						// 工程記号予備
	char			cBaund[1];							// バウンダリ合わせ
	char			cCaplSyuZai[10];					// CAPL 主材質
	char			cMetuke[8];							// 目付規格
	char			cNi[2];								// Ni付着量
	char			cSodoTCM[2];						// TCMロール粗度
	char			cSpmUmu[2];							// SPM圧下有無 (有り:O  無し:X)
	char			cNextLine[2];						// 計画次工程コード
	char			cMuke[32];							// 向先
	char			cYouto[4];							// 用途
	char			cCaplSiage[4];						// CAPL表面仕上げ
	char			cCaplSanUmu[2];						// CAPL酸洗有無
	char			cHinMode[1];						// 品種区分

	char			cGaCrKubun[1];						// GA/CR区分
	char			cSpmJisekiUmu[2];					// SPM圧下実績有無（有り：0 無し:X）
	char			cKouteiCode[2];						// 工程コード

};

//======================================================
// コイル実績
struct DBDATA_COIL_RESULT
{
	char			cKizukenNo[SIZE_KIZUKEN_NO];		// 管理No
	int				nEndMode;							// 実績格納区分		 (1:通常切替 2:バックアップ切替 3:手動停止 4:強制停止 0:不定)
	int				nSt_Hantei[NUM_MEN];				// 判定状態[表裏]	 (0:正常 1:異常)
	int				nSt_Light[NUM_MEN];					// 照明装置状態[表裏]
	int				nSt_CamControl;						// カメラコントロール回路状態
	int				nSt_CamSwitch;						// カメラリンクスイッチ回路状態
	int				nSt_Procon;							// プロコン状態
	int				nSt_FF;								// FF状態
	int				nSt_Sqence;							// シーケンサ状態
	int				nSt_Cam_Inline[NUM_MEN];			// カメラ台車状態[表裏]
	int				nSt_DbSkip[NUM_MEN];				// DB未保存[表裏]
	int				nSt_FrameSkip[NUM_MEN];				// フレームスキップ[表裏]
	int				nHtID[NUM_MEN][NUM_HANTEI];			// 使用判定PC[表裏] (その検査で使用した判定PCID)
	int				nRpiID[NUM_MEN][NUM_RPIEXEC];		// 使用全長PC[表裏] (その検査で使用した全長処理PCID)
	int				bStartWpdOk;						// 検査基点信号検出可否

	COIL_SETTING	setting;							// φωで使うコイル基本情報

	int				nSpmStat;							// SPM圧下有無[0:SPM圧下無し,1:SPM圧下有り]
	long			nLength;							// φω実測長 [SPM圧下無し/SPM圧下有り][mm]
	long			nLengthSub;							// 検査比較実測長 [mm]
	long			nLengthDsp;							// 表示実測長 [mm]
	int				nOutCoilPos;						// 出側単位先端位置 [mm] ※ 今わかっている分だけ

	EM_DIV_UNTEN 	emUnten;							// 運転状態
	EM_DIV_KENSA	emOneKensa[NUM_MEN];				// 検査状態 (一区間でもその値が有った場合)
	EM_DIV_KIKI		emOneKiki[NUM_MEN];					// 機器状態 (一区間でもその値が有った場合)
	EM_DIV_PLG		emPlg;								// PLG区分
	int				nResultSendOk;						// 外部実績送信許可 (0：送信  1:送信しない)
	int				nBr[MAX_CAMSET][MAX_CAMANGLE][3];	// 輝度[カメラセット][カメラ角度][C,L,R]		// CしかDBにはセットしない
	int				nExp[MAX_CAMSET][MAX_CAMANGLE];		// 露光時間[カメラセット][カメラ角度]
	int				nGain[MAX_CAMSET][MAX_CAMANGLE];	// ゲイン[カメラセット][カメラ角度]
	int				nCoilCnt;							// 検査開始からx本目のコイル（検査開始1本目が0。以降論理コイル切替のタイミングで+1）
	int				nToyuSply;							// 塗油スプレー装置状態 (0：未吹付  1:1区間でも吹付中有り)
};

////======================================================
//// 出側コイル(物理・論理両方)
//struct	DBDATA_COIL_OUT_INF
//{
//	COIL_OUT_INF	data;											// 出側コイル情報
//};



//======================================================
// コイル実績アップデート （実績確定後のUpDate用）
enum EM_MODE_COILRESULT_UPDATE
{
	MODE_RECV_SHRCUT = 0,						// シャーカット信号受信
	MODE_DSP_WPD_ON								// 表示WPD信号ON
};

struct DBDATA_COIL_RESULT_UPDATE
{
	EM_MODE_COILRESULT_UPDATE	emMode;								// 実行種類
	char						cKizukenNo[SIZE_KIZUKEN_NO];		// 管理No

	// ---- シャーカット信号受信 --------------------
	int							nOutCoilPos;						// 出側単位先端位置 [mm]

	// ---- 表示WPD信号ON --------------------
	int							nDspLength;							// 表示実測長
	int							nSpmUmu;							// SPM圧下有無
};


//======================================================
// コイル位置情報
struct DBDATA_COIL_POSITION
{
	char					cKizukenNo[SIZE_KIZUKEN_NO];			// 管理No
	int						nID;									// ID (1～連番)

	EM_DIV_KENSA			emKensa[NUM_MEN];						// 検査状態
	EM_DIV_KIKI				emKiki[NUM_MEN];						// 機器状態

	float					dEdge[2];								// エッジ位置 [0]左 [1]右
	float					dWalk;									// ウォーク量
};


//======================================================
// コイルトレンド情報
struct DBDATA_COIL_TRAND
{
	char					cKizukenNo[SIZE_KIZUKEN_NO];			// 管理No
	int						nMen;									// 表裏区分 (0:表 1:裏)
	int						nLen;									// 長手位置 [mm] (測定位置)
	
	// トレンドデータ
	int						nSpeed;									// ライン速度 [mpm]
	float					dCoilWid;								// コイル幅 [mm]
	float					dCoilWidMax;							// コイル幅最大 [mm]
	float					dCoilWidMin;							// コイル幅最小 [mm]
	float					dWalk;									// ウォーク量 [mm]
#ifndef HT_IS_COLOR
	int						nBr[NUM_CAM_POS][NUM_CAM_ANGLE][3];		// 平均輝度 [0:DS/1:WS][0:正/1:乱][0:C/1:L/2:R]
	int						nExp[NUM_CAM_POS][NUM_CAM_ANGLE];		// 露光時間 [clk] (正/乱)
#else
	int						nBr[NUM_CAM_POS][MAX_CAMANGLE][3];		// 平均輝度 (R, G, B)(C, L, R の順)	// CしかDBにはセットしない
	int						nExp[NUM_CAM_POS][MAX_CAMANGLE];		// 露光時間 [clk]
	float					dGain[NUM_CAM_POS][3];					// カメラゲイン値(R, G, Bの順)
#endif
	int						nLightValue[DIV_LED_COLOR_END];			// 照明装置_調光値(表,裏)(R,G1,G2,B1,B2)

	double					dSpmHosei;								// SPM補正率（1.0 + SPM伸び率[10^2%]）
};

//======================================================
// カット位置
struct DBDATA_COIL_CUT_POS
{
	char					cKizukenNo[SIZE_KIZUKEN_NO];			// 管理No
	int						nCutNo;									// 分割No
	int						nMode;									// 区分
																		// 1:シャーカット （ファーストカット）
																		// 2:シャーカット （ラストカット）
																		// 3:分割         （※ 分割は、１点のみ。カットは、範囲）
																		// 4:異種点
	//int						nPos[NUM_SPM_STAT];						// 先頭位置 [0:SPM圧下無し,1:SPM圧下有り][mm]
	//int						nOutLen[NUM_SPM_STAT];					// 保有長さ [0:SPM圧下無し,1:SPM圧下有り]
	//int						nOutPos[NUM_SPM_STAT];					// 出側距離位置 [0:SPM圧下無し,1:SPM圧下有り]
	COIL_OUT_INF			data;									// 出側コイル情報
};


//======================================================
// 疵情報

struct DBDATA_DEFECT_INF
{
	char					cKizukenNo[SIZE_KIZUKEN_NO];			// 管理No
	DETECT_BASE_DATA		data;									// 基本疵情報
	float					dLen;									// 疵発生位置

	double					attr_base[MAX_ATTR_BASE];				// 基本特徴量
	double					attr_angle[MAX_HT1_CAMANGLE][MAX_ATTR_ANGLE];	// 角度別特徴量

	// 拡張部分
	int						nLowGrFlg;								// 無害疵一時保存フラグ (低DB用保存 <=x< 優先DB保存 の範囲内の場合、1)
	int						nIsDspFlg;								// 表示対象区分_探傷画面 (1:探傷画面表示対象)
	bool					bImgSkip;								// 疵画像スキップフラグ(true:スキップ)
	//int						nRequiredTime[MAX_REQUIRED_TIME];		// 画像取込開始～統括処理完了までの各所要時間

	// 画像データがJPEGの場合
	bool					isBmp;									// trueの場合BMP画像と判定する
	int						nImgsize[MAX_HT1_CAMANGLE];				// 画像サイズ
	BYTE*					pImg[MAX_HT1_CAMANGLE];					// 正/乱画像 ポインタ

	double					dSpmHosei;								// SPM補正率（1.0 + SPM伸び率[10^2%]）

	// BCP書込みクラス受渡し用
	int						nKizuNo;								// 今回の疵No

};

//======================================================
//// エッジ
// エッジ検出実績
struct DBDATA_EDGE_COIL_INF
{
	char					cKizukenNo[SIZE_KIZUKEN_NO];			// 疵検管理No
	int						nMen;									// 表裏区分

	float					dCoilWidMin;							// 板幅最小
	float					dCoilWidMax;							// 板幅最大

	int						nLimitEdgeErrNum;						// 板幅許容誤差範囲外数
	int						nLostEdgeNum[2];						// エッジ未検出数 (左エッジ/右エッジ)

	int						nFrameNumCoil;							// コイル単位フレーム数
	int						nFrameNumSkip[NUM_CAM_POS][5];			// カメラセット (1～10) (未検査種類1～５)
};

// エッジ検出モード変化管理
struct DBDATA_EDGE_CHANGE_INF
{
	char					cKizukenNo[SIZE_KIZUKEN_NO];			// 管理No
	int						nPos;									// 発生位置 [mm]
	int						nMen;									// 表裏区分 (0:表 1:裏)
	int						nFrameNo;								// フレームNo
	int						nEdgeMode;								// 検出区分 (0:正常  1:片ｴｯｼﾞ優先  2:ｾﾝﾀｰﾗｲﾝ振り分け  -1:スキップ   -9:対象外   -99:初期状態）
	float					dEdgeRawPos[2];							// 生エッジ位置 [mm] (左エッジ/右エッジ)
	float					dEdgeAnsPos[2];							// 最終エッジ位置 [mm] (左エッジ/右エッジ)
	int						nEdgeErrReason;							// エッジ不正理由の論理和
																	//	0x00:正常
																	//	0x01:左エッジ検出範囲外
																	//	0x02:右エッジ検出範囲外
																	//	0x04:板幅許容誤差範囲外
																	//	0x08:前フレームとの許容範囲外
};

//======================================================
//// カメラリンクスイッチ設定
struct DBDATA_CAMERA_LINKSWITCH
{
	int						nPCID[NUM_MEN][NUM_HANTEI];						// 判定PCID
	int						nParaPCID[NUM_ALL_HTPARA];						// ロジックパラランPCID
	int						nLogiParaCam[NUM_ALL_HTPARA][NUM_CAMLINK_BOARD];// ロジックパララン用カメラセット
};

//======================================================
// その他

//シーケンサからパルスレート変更
struct DBDATA_PULSE_RATE_UPDATE
{
	double					dPls[3];								// 変更後のパルスレート[0:検査用PLG, 1:検査比較用PLG, 2:表示用PLG][mm/pls]
	double					dVres[3];								// 変更後の長手方向分解能[0:検査用PLG, 1:検査比較用PLG, 2:表示用PLG][mm/pixel]
																	// たぶん[0](検査用PLG)以外使わない^^;
};


//DB問い合わせ状況確認
struct DBDATA_ADD_SYNC
{
	MAIL_ITEM				sendItem;								// 結果返却先
};


//BCP batch 強制実行
struct DBDATA_BCP_BATCH_EXEC
{
	bool					bMode;									// モード
};

//疵画像登録数リセット
struct DBDATA_COUNT_RESET
{
	char					cKizukenNo[SIZE_KIZUKEN_NO];			// 管理No
	int						nTorB;									// 表裏区分(0:表 1:裏 2:両面)
};



/////////////////////////////////////////////////////////////////////////////////////////////////////////
//// 独自の分類など

// POR方向
enum DIV_POR_DIRECTION
{
	DIV_POR_DOWN = 0,	// 下出し
	DIV_POR_UP			// 上出し
};


static CString GetPorDirection(DIV_POR_DIRECTION dir)
{
	CString sWk;

	switch (dir) {
	case DIV_POR_DOWN:	sWk = "下出し";		break;
	case DIV_POR_UP:	sWk = "上出し";		break;
	default:			sWk = "不明";		break;
	}
	return sWk;
}


// 台車位置
enum DIV_DAISYA_POS
{
	DIV_DAISYA_IN = 0,			// インライン位置
	DIV_DAISYA_OFF				// オフライン位置

};
static CString GetDaisyaPos(DIV_DAISYA_POS pos)
{
	CString	sWk;
	switch (pos) {
	case DIV_DAISYA_IN:		sWk = "インライン位置";		break;
	case DIV_DAISYA_OFF:	sWk = "オフライン位置";		break;
	default:				sWk = "位置不明";			break;
	}
	return sWk;
}

static CString GetSpmStat(EM_DIV_SPM_STAT spm)
{
	CString	sWk;
	switch (spm) {
	case DIV_SPM_FALSE:		sWk = "開放中";		break;
	case DIV_SPM_TRUE:		sWk = "圧下中";		break;
	default:				sWk = "初期";		break;
	}
	return sWk;
}

enum EM_DIV_SEQ_STAT
{
	DIV_SEQ_NORMAL = 0,			// 正常
	DIV_SEQ_WARNING,			// 注意
	DIV_SEQ_ABNORMAL			// 異常
};
static CString GetSeqStat(EM_DIV_SEQ_STAT stat)
{
	CString	sWk;
	switch (stat) {
	case DIV_SEQ_NORMAL:	sWk = "正常";		break;
	case DIV_SEQ_WARNING:	sWk = "注意";		break;
	case DIV_SEQ_ABNORMAL:	sWk = "異常";		break;
	default:				sWk = "不明";		break;
	}
	return sWk;
}

enum EM_DIV_SEQ_KADOU
{
	DIV_SEQ_RESET = 0,			// リセット済み
	DIV_SEQ_INIT,				// 初期化済み
	DIV_SEQ_RESERVE,			// 予備
	DIV_SEQ_IMAGING				// 撮像中
};
static CString GetSeqKadou(EM_DIV_SEQ_KADOU stat)
{
	CString	sWk;
	switch (stat) {
	case DIV_SEQ_RESET:		sWk = "ﾘｾｯﾄ済み";	break;
	case DIV_SEQ_INIT:		sWk = "初期化済み";	break;
	case DIV_SEQ_RESERVE:	sWk = "予備";		break;
	case DIV_SEQ_IMAGING:	sWk = "撮像中";		break;
	default:				sWk = "不明";		break;
	}
	return sWk;
}

enum DIV_CHILER_STOP
{
	DIV_CHILER_RUN = 0,			// 運転中
	DIV_CHILER_STOPPED			// 停止中
};
static CString GetChilerStop(DIV_CHILER_STOP stat)
{
	CString	sWk;
	switch (stat) {
	case DIV_CHILER_RUN:	sWk = "運転中";		break;
	case DIV_CHILER_STOPPED:sWk = "停止中";		break;
	default:				sWk = "不明";		break;
	}
	return sWk;
}

enum DIV_CHILER_ERR
{
	DIV_CHILER_NORMAL = 0,		// 正常
	DIV_CHILER_ABNORMAL			// 異常
};
static CString GetChilerErr(DIV_CHILER_ERR stat)
{
	CString	sWk;
	switch (stat) {
	case DIV_CHILER_NORMAL:	sWk = "正常";		break;
	case DIV_CHILER_ABNORMAL:sWk = "異常";		break;
	default:				sWk = "不明";		break;
	}
	return sWk;
}

enum DIV_DRYER_STOP
{
	DIV_DRYER_RUN = 0,			// 運転中
	DIV_DRYER_STOPPED			// 停止中
};
static CString GetDryerStop(DIV_DRYER_STOP stat)
{
	CString	sWk;
	switch (stat) {
	case DIV_DRYER_RUN:		sWk = "運転中";		break;
	case DIV_DRYER_STOPPED:	sWk = "停止中";		break;
	default:				sWk = "不明";		break;
	}
	return sWk;
}

enum DIV_DRYER_ERR
{
	DIV_DRYER_NORMAL = 0,		// 正常
	DIV_DRYER_ABNORMAL			// 異常
};
static CString GetDryerErr(DIV_DRYER_ERR stat)
{
	CString	sWk;
	switch (stat) {
	case DIV_DRYER_NORMAL:	sWk = "正常";		break;
	case DIV_DRYER_ABNORMAL:sWk = "異常";		break;
	default:				sWk = "不明";		break;
	}
	return sWk;
}

enum DIV_COOLER_STOP
{
	DIV_COOLER_RUN = 0,			// 運転中
	DIV_COOLER_STOPPED			// 停止中
};
static CString GetCoolerStop(DIV_COOLER_STOP stat)
{
	CString	sWk;
	switch (stat) {
	case DIV_COOLER_RUN:	sWk = "運転中";		break;
	case DIV_COOLER_STOPPED:sWk = "停止中";		break;
	default:				sWk = "不明";		break;
	}
	return sWk;
}

enum DIV_COOLER_ERR
{
	DIV_COOLER_NORMAL = 0,		// 正常
	DIV_COOLER_ABNORMAL			// 異常
};
static CString GetCoolerErr(DIV_COOLER_ERR stat)
{
	CString	sWk;
	switch (stat) {
	case DIV_COOLER_NORMAL:	sWk = "正常";		break;
	case DIV_COOLER_ABNORMAL:sWk= "異常";		break;
	default:				sWk = "不明";		break;
	}
	return sWk;
}

enum DIV_DOOR_STAT
{
	DIV_DOOR_CLOSE = 0,			// 閉状態
	DIV_DOOR_OPEN				// 開状態
};
static CString GetDoorStat(DIV_DOOR_STAT stat)
{
	CString	sWk;
	switch (stat) {
	case DIV_DOOR_CLOSE:	sWk = "閉状態";		break;
	case DIV_DOOR_OPEN:		sWk = "開状態";		break;
	default:				sWk = "不明";		break;
	}
	return sWk;
}

enum DIV_KEY_STAT
{
	DIV_KEY_LOCK = 0,			// 閉状態
	DIV_KEY_UNLOCK				// 開状態
};
static CString GetKeyStat(DIV_KEY_STAT stat)
{
	CString	sWk;
	switch (stat) {
	case DIV_KEY_LOCK:		sWk = "閉状態";		break;
	case DIV_KEY_UNLOCK:	sWk = "開状態";		break;
	default:				sWk = "不明";		break;
	}
	return sWk;
}

enum DIV_SPRAY_STAT
{
	DIV_NOT_BLOWING = 0,		// 未吹付
	DIV_BLOWING					// 吹付中
};
static CString GetSprayStat(DIV_SPRAY_STAT stat)
{
	CString	sWk;
	switch (stat) {
	case DIV_NOT_BLOWING:	sWk = "未吹付";		break;
	case DIV_BLOWING:		sWk = "吹付中";		break;
	default:				sWk = "不明";		break;
	}
	return sWk;
}

// ストップマーク関係
#define STOPMARK_INTERVAL_LEN		10000							// ストップマーク連続入力防止距離 [mm]

/////////////////////////////////////////////////////////////////////////////////////////////////////////
// 通信状態 ステータス定義
//=======================================
// メッセージ通知
//=======================================
//// 全てMainInstanceで受ける
//       場合によっては、MainInstanceが各ワーカースレッドにたれ流しする
enum ENUM_MAININSTANCE {
	E_DEF_ML_PARAM = WM_USER + 100,									// パラメータ変更通知
	E_DEF_ML_START,													// 検査開始要求
	E_DEF_ML_STOP,													// 検査停止要求
	E_DEF_ML_WAIT_START_ANSWER,										// 判定開始アンサー
	E_DEF_ML_WAIT_STOP_ANSWER,										// 判定停止アンサー
	E_DEF_ML_STATE_ANSWER,											// 判定状態問合せアンサー
	E_DEF_ML_INITIAL,												// イニシャル伝文受信
	E_DEF_ML_RECV_CUTAREA,											// カット範囲情報通知
	E_DEF_ML_ALAMECANCEL,											// 警報出力解除通知
	E_DEF_ML_MAE_WRITE,												// 前工程書込み完了通知
	E_DEF_ML_DB_ADD_SYNC,											// DB実績保存状況問い合わせ通知
	E_DEF_ML_LINKSW_ANSWER,											// カメラリンクスイッチ設定完了通知
	E_DEF_ML_LINKSW_JOIN_ANSWER,									// 途中参加設定完了通知
	E_DEF_ML_LINKSW_CAMSET_ANSWER,									// カメラ選択設定完了通知
	E_DEF_ML_RESUME_START_ANSWER,									// 途中開始アンサー
	E_DEF_ML_RESUME_STOP_ANSWER,									// 途中停止アンサー
	E_DEF_ML_RESUME_START,											// 途中開始要求
	E_DEF_ML_RESUME_STOP,											// 途中停止要求
	E_DEF_END														// 終了
};

//=======================================
// エッジ不正理由
//=======================================
enum EM_EDGE_ERR_REASON
{
	EDGE_NORMAL = 0x00,												// 正常
	EDGE_ERR_LEFT = 0x01,											// 左エッジ検出範囲外
	EDGE_ERR_RIGHT = 0x02,											// 右エッジ検出範囲外
	EDGE_ERR_WIDTH = 0x04,											// 板幅許容誤差範囲外
	EDGE_ERR_PREFRAME = 0x08										// 前フレームとの許容誤差範囲外
};

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
// デバッグボタン メニューリスト
// ここに定義を追加した後、XX_SAMPEXDlg::OnNMClickListCmd2()内のSwitch文に処理追加する
enum	ENUM_DEBUG_CMD {
	E_DEBUG_START,						// 検査開始
	E_DEBUG_STOP,						// 検査停止
	E_DEBUG_RESUME_START,				// 途中開始
	E_DEBUG_RESUME_STOP,				// 途中停止
	E_DEBUG_HT_CANNOT_CLEAR,			// 判定続行不可能解除
	E_DEBUG_PARAM,						// パラメータ取得
	E_DEBUG_CAMLINKSW_READY,			// カメラリンクスイッチ設定完了
	E_DEBUG_CAMLINKSW_CAMSET,			// カメラ選択設定完了
	E_DEBUG_RESUME_START_CMPL,			// 途中参加設定完了
	E_DEBUG_CMD_END						// 終端
};
static	CString	DebugCmd[E_DEBUG_CMD_END] = {
	"検査開始"
	,"検査停止"
	,"途中開始"
	,"途中停止"
	,"続行不可能解除"
	,"パラメータ取得"
	,"LINKSW設定完了"
	,"カメラ選択設定完了"
	,"途中参加設定完了"
};

// ここに定義を追加した後、XX_SAMPEXDlg::OnNMClickListInput()内のSwitch文に処理追加する
enum	ENUM_DEBUG_INPUT {
	E_DEBUG_RECV_COILINF_KEN,			// 擬似検査コイル受信
	E_DEBUG_RECV_COILINF_DSP,			// 擬似表示コイル受信
	E_DEBUG_REGET_COILINF_KEN,			// 検査用情報 再受信
	E_DEBUG_REGET_COILINF_DSP,			// 表示用情報 再受信
	E_DEBUG_PI_WPD_KEN_DM,				// PI 検査基点WPD検出DMのみ		(WPD3)
	E_DEBUG_PI_WPD_DSP_DM,				// PI 表示基点WPD検出DMのみ		(WPD5)
	E_DEBUG_PI_SHAR_CUT,				// PI シャーカット信号
	E_DEBUG_PATO_RESET,					// PI パトライトリセット
	E_DEBUG_SEQ_CNT_RESET,				// PO 制御シーケンサカウンタリセット
	E_DEBUG_INITIAL,					// イニシャル伝文受信通知
	E_DEBUG_VOICE_RESET_SETU,			// ボイス警報出力解除要求（設備異常警報解除）
	E_DEBUG_VOICE_RESET_DEF,			// ボイス警報出力解除要求（欠陥検出警報解除）
	E_DEBUG_PI_WPD_KEN,					// PI 検査基点WPD検出			(WPD3)
	E_DEBUG_PI_WPD_DSP,					// PI 表示基点WPD検出			(WPD5)
	E_DEBUG_INPUT_END					// 終端
};
static	CString	DebugInput[E_DEBUG_INPUT_END] = {
	"擬似検査コイル受信"
	,"擬似表示コイル受信"
	,"検査用情報 再受信"
	,"表示用情報 再受信"
	,"PI 検査基点WPD検出DM"
	,"PI 表示基点WPD検出DM"
	,"PI ｼｬｰｶｯﾄ信号"
	,"PI ﾊﾟﾄﾗｲﾄﾘｾｯﾄﾎﾞﾀﾝ"
	,"PO SEQｶｳﾝﾀﾘｾｯﾄﾎﾞﾀﾝ"
	,"ｲﾆｼｬﾙ伝文受信"
	,"設備異常警報解除"
	,"欠陥検出警報解除"
	,"PI 検査基点WPD検出"
	,"PI 表示基点WPD検出"
};

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 簡単な共通関数

//=======================================
// 疵出力ファイル名称取得
//=======================================
static CString GetDefectDumpName(int nMen) {
	CString s;
	s.Format("Def_Men=%d", nMen);
	return s;
}

//==========================================
// 数値→文字列変換
//------------------------------------------
// const long	val			: 変換前の数値
// char*		result		: 変換後の文字列
// const long	size		: 変換後の文字数
// const bool	isLeftPad	: True=左詰め False=右詰め
// const char	padding		: 空欄を埋める文字
// 戻り値		int			: 0=成功 
//==========================================
static int ItoA_Size(const long val, char* result, const long size, const bool isLeftPad, const char padding)
{
	char wk[256];					// 変換後の文字列
	char wkNum[256];				// 変換後の数値
	int  nStartPos;					// 数値をセットする開始位置(0オリジン)

	memset(wk, padding, 256);
	memset(wkNum, 0x00, 256);
	_itoa(val, wkNum, 10);

	// 何文字目から数値をセットする？	
	if (isLeftPad)	nStartPos = 0;
	else			nStartPos = size - (int)strlen(wkNum);

	memcpy(&wk[nStartPos], wkNum, strlen(wkNum));

	memcpy(result, wk, size);

	return 0;
}


//==========================================
// システム名取得
//==========================================
static const char* GetSystemName()
{
	return "5CGL極微小φω";
}

//=======================================
// 最小間隔 代表疵情報 構造体
//=======================================
// 疵情報
struct TYP_COLROW_BUF
{
	bool					umu;									// データ有無
	DETECT_BASE_DATA		Data;									// 結果マトメ
	double					dLen;									// 疵発生位置

	bool					bIsBmp;									// 画像がBmp時True
	int						nImgsize[MAX_CAMANGLE];					// 画像サイズ
	BYTE*					pImg[MAX_CAMANGLE];						// 画像領域
};
struct TYP_COLROW_INFO
{
	TYP_COLROW_BUF			BufDefCol[MAP_COL_NUM];					// 代表疵情報
	TYP_COLROW_BUF			BufDefAry[MAP_COL_NUM][MAP_NEST_NUM];	// リスト疵情報
	int						nSetCnt[MAP_COL_NUM];					// 実際のリスト領域にセットした疵個数
};
struct TYP_OP_SEND_DEFECT_INFO
{
	TYP_COLROW_INFO			ColnowBuf_Now;							// カレント最小区間の疵情報
	TYP_COLROW_INFO			ColnowBuf_Next;							// 次の最小区間の疵情報
};
