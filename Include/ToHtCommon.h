// *********************************************************************************
//	EPCソフト ＆ 統括 ＆ 判定 機能 関連ヘッダー
//	[Ver]
//		Ver.01    2015/06/18  初版作成
//
//	[メモ]
//		基本的に、書き換え可
// *********************************************************************************

// 個々のシステム固有のインクルード

#ifndef	_TOHTCOMMON_H
#define	_TOHTCOMMON_H

// インクルード
#include <windows.h>

#include "..\..\Include\LineCommon.h"									// 疵検インクルード
#include "..\..\Include\EPC\NCL2DETECT_if.h"							// I/F定義

//=======================================
// 判定機能 担当
//=======================================
struct CAMPAIR_ITEM {													// カメラペアに含まれるカメラセット
	int				num;													// その判定機能が担当するカメラセット数
	int				camset[MAX_HT1_CAMSET];									// その判定機能が担当するカメラセット(0:無し)
};

struct CAMPAIR_INF {													// 全カメラペア定義
	CAMPAIR_ITEM	item[NUM_CAMPAIR];										// (各判定機能が担当するカメラセット群) [カメラペア順]
	CAMPAIR_ITEM	item_cycle[NUM_CAMPAIR_CYCLE];							// (各周期機能が担当するカメラセット群) [カメラペア順]
	CAMPAIR_ITEM	item_para[NUM_CAMPAIR_PARA];							// (各ロジックパララン機能が担当するカメラセット群) [カメラペア順]
};

//=======================================
// メッセージ通知
//=======================================
static CString GetDivCamPair(int pair)
{
#ifdef NUM_CAM_POS_1
	if     (1 == pair) {return "表 C";}
	else if(2 == pair) {return "裏 C";}
	else			   {return "ﾊﾞｯｸｱｯﾌﾟ";}
#endif
#ifdef NUM_CAM_POS_2
	if     (1 == pair) {return "表 DS";}
	else if(2 == pair) {return "表 WS";}

	else if(3 == pair) {return "裏 DS";}
	else if(4 == pair) {return "裏 WS";}
	else			   {return "ﾊﾞｯｸｱｯﾌﾟ";}
#endif
#ifdef NUM_CAM_POS_3
	if     (1 == pair) {return "表 DS";}
	else if(2 == pair) {return "表 C";}
	else if(3 == pair) {return "表 WS";}

	else if(4 == pair) {return "裏 DS";}
	else if(5 == pair) {return "裏 C";}
	else if(6 == pair) {return "裏 WS";}
	else			   {return "ﾊﾞｯｸｱｯﾌﾟ";}
#endif
#ifdef NUM_CAM_POS_4
	if     (1 == pair) {return "表 DS";}
	else if(2 == pair) {return "表 D1";}
	else if(3 == pair) {return "表 W1";}
	else if(4 == pair) {return "表 WS";}

	else if(5 == pair) {return "裏 DS";}
	else if(6 == pair) {return "裏 D1";}
	else if(7 == pair) {return "裏 W1";}
	else if(8 == pair) {return "裏 WS";}
	else			   {return "ﾊﾞｯｸｱｯﾌﾟ";}
#endif
#ifdef NUM_CAM_POS_5
	if     (1 == pair) {return "表 DS";}
	else if(2 == pair) {return "表 D1";}
	else if(3 == pair) {return "表 C";}
	else if(4 == pair) {return "表 W1";}
	else if(5 == pair) {return "表 WS";}

	else if(6 == pair) {return "裏 DS";}
	else if(7 == pair) {return "裏 D1";}
	else if(8 == pair) {return "裏 C";}
	else if(9 == pair) {return "裏 W1";}
	else if(10 == pair) {return "裏 WS";}
	else			   {return "ﾊﾞｯｸｱｯﾌﾟ";}
#endif
#ifdef NUM_CAM_POS_6
	if     (1 == pair) {return "表 DS";}
	else if(2 == pair) {return "表 D1";}
	else if(3 == pair) {return "表 D2";}
	else if(4 == pair) {return "表 W2";}
	else if(5 == pair) {return "表 W1";}
	else if(6 == pair) {return "表 WS";}

	else if(7 == pair) {return "裏 DS";}
	else if(8 == pair) {return "裏 D1";}
	else if(9 == pair) {return "裏 D2";}
	else if(10== pair) {return "裏 W2";}
	else if(11== pair) {return "裏 W1";}
	else if(12== pair) {return "裏 WS";}
	else			   {return "ﾊﾞｯｸｱｯﾌﾟ";}
#endif
#ifdef NUM_CAM_POS_7
	if     (1 == pair) {return "表 DS";}
	else if(2 == pair) {return "表 D1";}
	else if(3 == pair) {return "表 D2";}
	else if(4 == pair) {return "表 C";}
	else if(5 == pair) {return "表 W2";}
	else if(6 == pair) {return "表 W1";}
	else if(7 == pair) {return "表 WS";}

	else if(8 == pair) {return "裏 DS";}
	else if(9 == pair) {return "裏 D1";}
	else if(10== pair) {return "裏 D2";}
	else if(11== pair) {return "裏 C";}
	else if(12== pair) {return "裏 W2";}
	else if(13== pair) {return "裏 W1";}
	else if(14== pair) {return "裏 WS";}
	else			   {return "ﾊﾞｯｸｱｯﾌﾟ";}
#endif
#ifdef NUM_CAM_POS_8
	if     (1 == pair) {return "表 DS";}
	else if(2 == pair) {return "表 D1";}
	else if(3 == pair) {return "表 D2";}
	else if(4 == pair) {return "表 D3";}
	else if(5 == pair) {return "表 W3";}
	else if(6 == pair) {return "表 W2";}
	else if(7 == pair) {return "表 W1";}
	else if(8 == pair) {return "表 WS";}

	else if(9 == pair) {return "裏 DS";}
	else if(10== pair) {return "裏 D1";}
	else if(11== pair) {return "裏 D2";}
	else if(12== pair) {return "裏 D3";}
	else if(13== pair) {return "裏 W3";}
	else if(14== pair) {return "裏 W2";}
	else if(15== pair) {return "裏 W1";}
	else if(16== pair) {return "裏 WS";}
	else			   {return "ﾊﾞｯｸｱｯﾌﾟ";}
#endif
#ifdef NUM_CAM_POS_9
	if     (1 == pair) {return "表 DS";}
	else if(2 == pair) {return "表 D1";}
	else if(3 == pair) {return "表 D2";}
	else if(4 == pair) {return "表 D3";}
	else if(5 == pair) {return "表 C";}
	else if(6 == pair) {return "表 W3";}
	else if(7 == pair) {return "表 W2";}
	else if(8 == pair) {return "表 W1";}
	else if(9 == pair) {return "表 WS";}

	else if(10== pair) {return "裏 DS";}
	else if(11== pair) {return "裏 D1";}
	else if(12== pair) {return "裏 D2";}
	else if(13== pair) {return "裏 D3";}
	else if(14== pair) {return "裏 C";}
	else if(15== pair) {return "裏 W3";}
	else if(16== pair) {return "裏 W2";}
	else if(17== pair) {return "裏 W1";}
	else if(18== pair) {return "裏 WS";}
	else			   {return "ﾊﾞｯｸｱｯﾌﾟ";}
#endif
#ifdef NUM_CAM_POS_10
	if     (1 == pair) {return "表 DS";}
	else if(2 == pair) {return "表 D1";}
	else if(3 == pair) {return "表 D2";}
	else if(4 == pair) {return "表 D3";}
	else if(5 == pair) {return "表 D4";}
	else if(6 == pair) {return "表 W4";}
	else if(7 == pair) {return "表 W3";}
	else if(8 == pair) {return "表 W2";}
	else if(9 == pair) {return "表 W1";}
	else if(10== pair) {return "表 WS";}

	else if(11== pair) {return "裏 DS";}
	else if(12== pair) {return "裏 D1";}
	else if(13== pair) {return "裏 D2";}
	else if(14== pair) {return "裏 D3";}
	else if(15== pair) {return "裏 D4";}
	else if(16== pair) {return "裏 W4";}
	else if(17== pair) {return "裏 W3";}
	else if(18== pair) {return "裏 W2";}
	else if(19== pair) {return "裏 W1";}
	else if(20== pair) {return "裏 WS";}
	else			   {return "ﾊﾞｯｸｱｯﾌﾟ";}
#endif
#ifdef NUM_CAM_POS_11
	if     (1 == pair) {return "表 DS";}
	else if(2 == pair) {return "表 D1";}
	else if(3 == pair) {return "表 D2";}
	else if(4 == pair) {return "表 D3";}
	else if(5 == pair) {return "表 D4";}
	else if(6 == pair) {return "表 C";}
	else if(7 == pair) {return "表 W4";}
	else if(8 == pair) {return "表 W3";}
	else if(9 == pair) {return "表 W2";}
	else if(10== pair) {return "表 W1";}
	else if(11== pair) {return "表 WS";}

	else if(12== pair) {return "裏 DS";}
	else if(13== pair) {return "裏 D1";}
	else if(14== pair) {return "裏 D2";}
	else if(15== pair) {return "裏 D3";}
	else if(16== pair) {return "裏 D4";}
	else if(17== pair) {return "裏 C";}
	else if(18== pair) {return "裏 W4";}
	else if(19== pair) {return "裏 W3";}
	else if(20== pair) {return "裏 W2";}
	else if(21== pair) {return "裏 W1";}
	else if(22== pair) {return "裏 WS";}
	else			   {return "ﾊﾞｯｸｱｯﾌﾟ";}
#endif
#ifdef NUM_CAM_POS_12
	if     (1 == pair) {return "表 DS";}
	else if(2 == pair) {return "表 D1";}
	else if(3 == pair) {return "表 D2";}
	else if(4 == pair) {return "表 D3";}
	else if(5 == pair) {return "表 D4";}
	else if(6 == pair) {return "表 D5";}
	else if(7 == pair) {return "表 W5";}
	else if(8 == pair) {return "表 W4";}
	else if(9 == pair) {return "表 W3";}
	else if(10== pair) {return "表 W2";}
	else if(11== pair) {return "表 W1";}
	else if(12== pair) {return "表 WS";}

	else if(13== pair) {return "裏 DS";}
	else if(14== pair) {return "裏 D1";}
	else if(15== pair) {return "裏 D2";}
	else if(16== pair) {return "裏 D3";}
	else if(17== pair) {return "裏 D4";}
	else if(18== pair) {return "裏 D5";}
	else if(19== pair) {return "裏 W5";}
	else if(20== pair) {return "裏 W4";}
	else if(21== pair) {return "裏 W3";}
	else if(22== pair) {return "裏 W2";}
	else if(23== pair) {return "裏 W1";}
	else if(24== pair) {return "裏 WS";}
	else			   {return "ﾊﾞｯｸｱｯﾌﾟ";}
#endif
#ifdef NUM_CAM_POS_13
	if     (1 == pair) {return "表 DS";}
	else if(2 == pair) {return "表 D1";}
	else if(3 == pair) {return "表 D2";}
	else if(4 == pair) {return "表 D3";}
	else if(5 == pair) {return "表 D4";}
	else if(6 == pair) {return "表 D5";}
	else if(7 == pair) {return "表 C";}
	else if(8 == pair) {return "表 W5";}
	else if(9 == pair) {return "表 W4";}
	else if(10== pair) {return "表 W3";}
	else if(11== pair) {return "表 W2";}
	else if(12== pair) {return "表 W1";}
	else if(13== pair) {return "表 WS";}

	else if(14== pair) {return "裏 DS";}
	else if(15== pair) {return "裏 D1";}
	else if(16== pair) {return "裏 D2";}
	else if(17== pair) {return "裏 D3";}
	else if(18== pair) {return "裏 D4";}
	else if(19== pair) {return "裏 D5";}
	else if(20== pair) {return "裏 C";}
	else if(21== pair) {return "裏 W5";}
	else if(22== pair) {return "裏 W4";}
	else if(23== pair) {return "裏 W3";}
	else if(24== pair) {return "裏 W2";}
	else if(25== pair) {return "裏 W1";}
	else if(26== pair) {return "裏 WS";}
	else			   {return "ﾊﾞｯｸｱｯﾌﾟ";}
#endif
#ifdef NUM_CAM_POS_14
	if     (1 == pair) {return "表 DS";}
	else if(2 == pair) {return "表 D1";}
	else if(3 == pair) {return "表 D2";}
	else if(4 == pair) {return "表 D3";}
	else if(5 == pair) {return "表 D4";}
	else if(6 == pair) {return "表 D5";}
	else if(7 == pair) {return "表 D6";}
	else if(8 == pair) {return "表 W6";}
	else if(9 == pair) {return "表 W5";}
	else if(10== pair) {return "表 W4";}
	else if(11== pair) {return "表 W3";}
	else if(12== pair) {return "表 W2";}
	else if(13== pair) {return "表 W1";}
	else if(14== pair) {return "表 WS";}

	else if(15== pair) {return "裏 DS";}
	else if(16== pair) {return "裏 D1";}
	else if(17== pair) {return "裏 D2";}
	else if(18== pair) {return "裏 D3";}
	else if(19== pair) {return "裏 D4";}
	else if(20== pair) {return "裏 D5";}
	else if(21== pair) {return "裏 D6";}
	else if(22== pair) {return "裏 W6";}
	else if(23== pair) {return "裏 W5";}
	else if(24== pair) {return "裏 W4";}
	else if(25== pair) {return "裏 W3";}
	else if(26== pair) {return "裏 W2";}
	else if(27== pair) {return "裏 W1";}
	else if(28== pair) {return "裏 WS";}
	else			   {return "ﾊﾞｯｸｱｯﾌﾟ";}
#endif
#ifdef NUM_CAM_POS_15
	if     (1 == pair) {return "表 DS";}
	else if(2 == pair) {return "表 D1";}
	else if(3 == pair) {return "表 D2";}
	else if(4 == pair) {return "表 D3";}
	else if(5 == pair) {return "表 D4";}
	else if(6 == pair) {return "表 D5";}
	else if(7 == pair) {return "表 D6";}
	else if(8 == pair) {return "表 C";}
	else if(9 == pair) {return "表 W6";}
	else if(10== pair) {return "表 W5";}
	else if(11== pair) {return "表 W4";}
	else if(12== pair) {return "表 W3";}
	else if(13== pair) {return "表 W2";}
	else if(14== pair) {return "表 W1";}
	else if(15== pair) {return "表 WS";}

	else if(16== pair) {return "裏 DS";}
	else if(17== pair) {return "裏 D1";}
	else if(18== pair) {return "裏 D2";}
	else if(19== pair) {return "裏 D3";}
	else if(20== pair) {return "裏 D4";}
	else if(21== pair) {return "裏 D5";}
	else if(22== pair) {return "裏 D6";}
	else if(23== pair) {return "裏 C";}
	else if(24== pair) {return "裏 W6";}
	else if(25== pair) {return "裏 W5";}
	else if(26== pair) {return "裏 W4";}
	else if(27== pair) {return "裏 W3";}
	else if(28== pair) {return "裏 W2";}
	else if(29== pair) {return "裏 W1";}
	else if(30== pair) {return "裏 WS";}
	else			   {return "ﾊﾞｯｸｱｯﾌﾟ";}
#endif
#ifdef NUM_CAM_POS_16
	if     (1 == pair) {return "表 DS";}
	else if(2 == pair) {return "表 D1";}
	else if(3 == pair) {return "表 D2";}
	else if(4 == pair) {return "表 D3";}
	else if(5 == pair) {return "表 D4";}
	else if(6 == pair) {return "表 D5";}
	else if(7 == pair) {return "表 D6";}
	else if(8 == pair) {return "表 D7";}
	else if(9 == pair) {return "表 W7";}
	else if(10== pair) {return "表 W6";}
	else if(11== pair) {return "表 W5";}
	else if(12== pair) {return "表 W4";}
	else if(13== pair) {return "表 W3";}
	else if(14== pair) {return "表 W2";}
	else if(15== pair) {return "表 W1";}
	else if(16== pair) {return "表 WS";}

	else if(17== pair) {return "裏 DS";}
	else if(18== pair) {return "裏 D1";}
	else if(19== pair) {return "裏 D2";}
	else if(20== pair) {return "裏 D3";}
	else if(21== pair) {return "裏 D4";}
	else if(22== pair) {return "裏 D5";}
	else if(23== pair) {return "裏 D6";}
	else if(24== pair) {return "裏 D7";}
	else if(25== pair) {return "裏 W7";}
	else if(26== pair) {return "裏 W6";}
	else if(27== pair) {return "裏 W5";}
	else if(28== pair) {return "裏 W4";}
	else if(29== pair) {return "裏 W3";}
	else if(30== pair) {return "裏 W2";}
	else if(31== pair) {return "裏 W1";}
	else if(32== pair) {return "裏 WS";}
	else			   {return "ﾊﾞｯｸｱｯﾌﾟ";}
#endif

}

//=======================================
// システム状態 名称
//=======================================
static CString GetDivSys(int em)   {return GetDivSys((EM_DIV_SYS)em); };	// システム状態
static CString GetDivSys(EM_DIV_SYS em) 
{
	CString wk;
	if		(DIV_SYS_INIT == em)				{ wk = "初期化中"; }
	else if (DIV_SYS_IDLE == em)				{ wk = "準備完了"; }
	else if (DIV_SYS_INITSTART == em)			{ wk = "検査開始初期化中"; }
	else if (DIV_SYS_WAITSTART == em)			{ wk = "検査開始待ち";}
	else if (DIV_SYS_RUN == em)					{ wk = "検査中";}
	else if (DIV_SYS_WAITSTOP == em)			{ wk = "検査停止待ち"; }
	else if (DIV_SYS_STOP == em)				{ wk = "検査停止";}
	else if (DIV_SYS_RETRY == em)				{ wk = "強制検査再起動中";}
	else										{ wk.Format("ステータス異常 <%d>", em);}
	return wk;
}

//=======================================
// 対EPC製 画像処理タスク 用
//=======================================
// 画像処理タスク関連
//#define EPC_PIPE_COMMAND		"\\\\.\\pipe\\nccntrl"						// コマンド用パイプ
//#define EPC_PIPE_DATA			"\\\\.\\pipe\\ncdata"						// データ用パイプ
//#define EPC_SMEM_DATA			"ncsmem"									// データ用共有メモリ
#define EPC_PIPE_COMMAND		name_cpipe									// コマンド用パイプ
#define EPC_PIPE_DATA			name_rpipe									// データ用パイプ
#define EPC_SMEM_DATA			name_shmem									// データ用共有メモリ
#define EPC_ODBC_FORMAT			"dsn=%s;uid=%s;pwd=%s;"						// ODBC接続用フォーマット(ODBC名称, 接続ユーザー名, 接続パスワード)

// 周期機能
#define USE_PD_EXT_CTRL														// NCspeedPDの周期機能で必要の為、定義だけする

// iPortViewer 関連
#define EPC_MAIL_CAMCONT		"camcont"									// iPortViewerからのメールスロット名称
#define EPC_MAIL_IPORTVIEWER	"iPortViewer"								// iPortViewerのメールスロット名称
#define FACT_EPC_IPORTVIEWER	6000										// iPortViewerからのメールスロット

//=======================================
// UDP設定
//		※今回は、ZZ_EPC用
//=======================================
#ifndef LOCAL
	#define EPC_UDP_IP				"192.9.200.255"							// マルチキャストIP
#else
	#define EPC_UDP_IP				"192.9.200.255"							// マルチキャストIP
#endif
#define EPC_UDP_IP_EDGE				"192.9.200.255"								// マルチキャストIP


#ifndef LOCAL
  //#define EPC_UDP_PORT			8003										// UDPポート
  #define EPC_UDP_PORT			8143										// UDPポート
#else
  #define EPC_UDP_PORT			8143										// 開発環境でテストするときにかち合わないように
#endif
#define EPC_UDP_PORT_EDGE		8902										// ※ZZ_EPC用 UDPポート(PARAM_COMMONのEdgeRestrictPortと同値にしておくこと) 

#define EPC_UDP_PORT_ALLFRAME	8100										// UDPポート(レコーダPC用)


// ZZ_EPC用の速度UDP。これを受信したら、1フレームの結果を返す
//   実エンジン時には送信しないこと
struct DEBUG_SPEED_INFO {
	DWORD	framenum;		// フレーム番号
	float	vRes;			// 縦分解能(mm/pixel)
};



////=======================================
//// 速度UDPにのせる 統括処理用フラグ (ToVal)
////=======================================
//#define TOVAL_TOP_COILCHANGE		0										// 表面 コイル切り替えフラグ
//#define TOVAL_BOT_COILCHANGE		1 										// 裏面 コイル切り替えフラグ
//#define TOVAL_FCUT_COILOUTCHANGE	2										// 出側物理コイル ファーストカット 切り替えフラグ
//#define TOVAL_LCUT_COILOUTCHANGE	3										// 出側物理コイル ラストカット 切り替えフラグ
//
//#define TOVAL_REC_PROCTIME			4										// 1フレーム取込完了時のQueryPerformanceCounter
//
//#define TOVAL_SET_FRAMENO			6 										// エッジ検出結果取得フレームNo (表/裏で2つ使用)

//=======================================
// TEX使用 特徴量ID
//=======================================
#define EPC_ATTR_BASE_MERGE				255									// マージ結果 (0:マージ対象外 1:マージ対象)
#define EPC_ATTR_BASE_KIZUNO			254									// 疵NOワーク　(コイル単位、表裏で1～の連番)

#define EPC_ATTR_BASE_FRAME_DEFECT_MAX	253									// 1フレーム内で最大疵 (0:最大疵じゃない 1:最大疵)
#define EPC_ATTR_BASE_YU_G				252									// グレード優先度 (マージで必須)
#define EPC_ATTR_BASE_YU_T				251									// 疵種優先度 (マージで必須)
#define EPC_ATTR_BASE_RANK_G			250									// 欠陥グレード (0:無害 1:軽欠陥 2:中欠陥 3:重欠陥)

#define EPC_ATTR_BASE_HTTO_PROC_TIME	249									// 画像取込～判定→統括伝送完了までの所要時間 [msec]
#define EPC_ATTR_BASE_TOTAL_PROC_TIME	248									// 画像取込～統括処理完了までの所要時間 [msec]
#define EPC_ATTR_BASE_HT_PROC_TIME		247									// 画像処理エンジン処理完了～判定→統括伝送開始までの所要時間 [msec]
#define EPC_ATTR_BASE_TR1_PROC_TIME		246									// DS(WS)の疵情報受信開始～DS(WS)の疵情報受信開始までの所要時間 [msec]
#define EPC_ATTR_BASE_TR2_PROC_TIME		245									// DS(WS)の疵情報受信開始～WS(DS)の疵情報受信完了までの所要時間 [msec]
#define	EPC_ATTR_BASE_NC_PROC_TIME		244									// 画像取込～画像処理エンジン処理完了までの所要時間 [msec]

#define EPC_ATTR_BASE_IMG_CULL_X		243									// 全長画像間引き率横
#define EPC_ATTR_BASE_IMG_CULL_Y		242									// 全長画像間引き率縦


//=======================================
// 判定→統括 切り出し画像JPEG化の場合の構造体
//=======================================
// 切り出し画像配列の要素(判定管理で使用)
struct DEFECT_HEAD_IMAGE {
	DWORD					nWidth;											// 切り出し画像幅
	DWORD					nHeight;										// 切り出し画像高さ
	float					fShrinkRatio;									// 画像の圧縮率
	DWORD					nSize[MAX_IMAGE_CNT];							// Jpegサイズ
	DWORD					nImgOffset[MAX_IMAGE_CNT];						// 画像の先頭位置(本構造体先頭からの画像先頭オフセット)
	DWORD					nNextImgOffset;									// 自分の画像+ピッチも含めたサイズ(=本構造体先頭から次の画像配列までのオフセットとなる) (jepgが8の倍数でないとダメだから) (最後は0)
	// 実際には、この後に nSize分の画像がある
};

// 切り出し画像の実体(統括管理で使用)
struct DEFECT_DATA_IMAGE {
	bool					bIsBmp;											// 画像がBmp時True
	DWORD					nWidth;											// 切り出し画像幅
	DWORD					nHeight;										// 切り出し画像高さ
	DWORD					nSize[MAX_IMAGE_CNT];							// Jpegサイズ
	BYTE*					pImg[MAX_IMAGE_CNT];							// 画像の先頭位置
};

//=======================================
// 特徴量用 コイル情報
//=======================================
#pragma pack(1)
struct TYPE_EPC_COIL			// 212 byte  (max240byteまで) ※ それ以上の場合は、不要な項目を削除
{
	char			cMeiNo[4];												// [01] 入側論理コイル 命令No
	char			cCoilNo[12];											// [02] 入側論理コイル 生産No
	long			nSendPoint;												// [03] 送信位置 (0:WPD3 1:WPD4 2:WPD6-50m)
	long			nLenIn;													// [04] 入側論理コイル 長さ [m]	
	long			nAtuZai;												// [05] 材料板厚 [μm] (出側)
	long			nWidZai;												// [06] 材料板幅 [mm]
	long			nAtuSei;												// [07] 注文板厚 [μm] (出側)
	long			nWidSei;												// [08] 注文板幅 [mm]
	long			nWidTrim;												// [09] 現工程トリム幅 [mm] (ノートリム時は、0)
	long			nNobiSPM;												// [10] SPM伸び率 [10-2%]
	long			nNobiTLV;												// [11] TLV伸び率 [10-2%]
	long			nCutF;													// [12] 入側におけるフロント端板カットの実績長
	long			nCutT;													// [13] 入側におけるテール端板カットの実績長
	long			nSPMRoolKei_Up;											// [14] SPMワークロール径 (上) [10-1mm]
	long			nSPMRoolKei_Down;										// [15] SPMワークロール径 (下) [10-1mm]
	long			nYobi[10];												// [16-25] int予備10～1

	char			cKenKijun_O[4];											// [26] 検査基準 表
	char			cKenKijun_U[4];											// [27] 検査基準 裏
	char			cKurometo_Kind[2];										// [28] クロメート 実績種類
	char			cKurometo_Umu[2];										// [29] クロメート 実績有無
	char			cHinCode[1];											// [30] 品種
	char			cMekkiKind[1];											// [31] メッキ種類
	char			cSyuZai[2];												// [32] 主材質
	char			cDifMaker[1];											// [33] ディファレンシャルマーカー
	char			cKenban[1];												// [34] 原板区分
	char			cMekkiCycle[1];											// [35] メッキサイクル
	char			cInLineSPM[1];											// [36] レベラー インラインSPM
	char			cKagakuToyu[1];											// [37] 化学処理塗油
	char			cAtukkaSPM[1];											// [38] オフラインSPM圧下率
	char			cMekkiKubun[1];											// [39] メッキ区分
	char			cOmoteSyori1[1];										// [40] 表面処理1桁目
	char			cOmoteSyori2[1];										// [41] 表面処理2桁目
	char			cToyu[1];												// [42] 塗油
	char			cKouteiYobi[1];											// [43] 工程記号予備
	char			cBaund[1];												// [44] バウンダリ合わせ
	char			cCaplSyuZai[10];										// [45] CAPL 主材質
	char			cMetuke[8];												// [46] 目付規格
	char			cNi[2];													// [47] Ni付着量
	char			cSodoTCM[2];											// [48] TCMロール粗度
	char			cSpmUmu[2];												// [49] SPM圧下有無 (有り:O  無し:X)
	char			cNextLine[2];											// [50] 計画次工程コード
	char			cMuke[32];												// [51] 向先
	char			cYouto[4];												// [52] 用途
	char			cCaplSiage[4];											// [53] CAPL表面仕上げ
	char			cCaplSanUmu[2];											// [54] CAPL酸洗有無
	char			cHinMode[1];											// [55] 品種区分
	char			cGaCrKbn[1];											// [56] GA/CR区分
	char			cSpmJiseki[2];											// [57] SPM実績有無
	char			cKouteiCode[2];											// [58] 工程コード
	char			cYobi[2];												// [59] 予備
};

//=======================================
// 分割コイル情報送信対応
//		分割送信時は、FACT_HT_DEFECT_14 を使うこと

#define DIVCOIL_ONE_SIZE_SEND		220												// 分割送信時、一度に送るバイト数
#define DIVCOIL_MAX_DIV_CNT			10												// 最大分割個数
#define DIVCOIL_MAX_SEND_SIZE		(DIVCOIL_ONE_SIZE_SEND * DIVCOIL_MAX_DIV_CNT)	// 最大送信サイズ

//// 分割情報受信用構造体
struct DIVCOIL_MERGED_PCINFO {
//	int		nDlyKey;																// 遅延実行キー
	int		nDivCnt;																// 分割個数
	bool	bGetFlg[DIVCOIL_ONE_SIZE_SEND];											// 分割データの受信フラグ
	int		nSize;																	// 合計のサイズ(データを受信するごとに加算)
	BYTE	bPcInfo[DIVCOIL_MAX_SEND_SIZE];											// 上位情報
};


#pragma pack()

#endif
