// ///////////////////////////////////////////////////////////////////
// 色差エンジンv2のインターフェースヘッダファイル
//
// 2016 NSSMC all rights reserved.

#pragma once

#include <Windows.h>
#include <tchar.h>

#pragma pack(push, 1)

enum {
	CH_R = 0,
	CH_G,
	CH_B,
	CH_DIFF
};

// ////////////////////
// 探傷プロセス状態
// ////////////////////
enum NCL_ENGINE_STATE
{
	NCL_PREINIT = 1,	// 初期化前
	NCL_TRANSITION,		// 状態遷移中
	NCL_INITIALIZED,	// 初期化済み
	NCL_RUNNING,		// 探傷中
	NCL_RUNNING_PRELOAD	// 探傷中かつPRELOAD実行中
};

// ////////////////////
// プロセスのリターンコード (実装するか未定）
// ////////////////////
enum NCL_PROCESS_ERR
{
    NCL_PROCESS_NOERR = 0,
    NCL_PROCESS_ERR_STARTUP = -1,
    NCL_PROCESS_ERR_UNKNOWN = -999,
};

// ////////////////////
// 制御コマンド・構造体
// ////////////////////

enum NCL_CMDCODE
{
    NCL_INIT = 0,			// 初期化
    NCL_DISPOSE,			// 反初期化（終了化）
    NCL_EXIT,				// 終了
    NCL_START,				// 開始
    NCL_STOP,				// 停止
    NCL_PARAM,				// パラメータ設定
	NCL_COILINFO,			// 上位情報設定
	NCL_PRELOAD_PARAM,		// DBからメモリへのパラメータ読み込み
	NCL_SET_POSITION,		// 内部距離カウンタ設定
	NCL_QUERY_STATUS,		// 状態問い合わせ
	NCL_CLEAR_QUEUE,		// 未実行のPARAM,SET_POSITIONを消去
	NCL_RESUME_START,		// 途中参加による開始(NCL_STARTの代わり）。
    NCL_UNKNOWN,


	NCL_SAVE_DDS	= 98,		// DDS自前保存
	NCL_INIT_RPISRC = 99,		// 初期化(Input = RPI )
	NCL_ASYNC_ERR = -1,		// 非同期エラーのときのコマンドコード
	NCL_ASYNC_EVENT = -2		// 非同期イベントのときのコマンドコード
};

// 各コマンドに対するエラーコード
enum NCL_ERRCODE
{
	// コマンドに共通
	NCL_SUCCESS = 0,
	NCL_ERR_CMD_UNEXPECTED,		// モードが違うとき
	NCL_ERR_CMD_UNKNOWN,		// 知らないコマンドコード

	// INIT
	NCL_ERR_INIT_DB = 100,		// DB
	NCL_ERR_INIT_GRABBER,		// APX-3326,3323
	NCL_ERR_INIT_ARBITERNOTFOUND,	// エッジ調停が見つからない
	NCL_ERR_INIT_BADSTATE,		// すでに初期化等
	NCL_ERR_INIT_NOGPU,			// GPUがない

	// DISPOSE
	NCL_ERR_DISPOSE_FAILED = 200,

	// EXIT
	NCL_ERR_EXIT_FAILED = 300,

	// START
	NCL_ERR_START_FAILED = 400,
	NCL_ERR_RESET_FAILED,
	NCL_ERR_CONNECT_NCLEDGE_FAILED,
	NCL_ERR_NOT_INITIALIZED,

	// STOP
	NCL_ERR_STOP_FAILED = 500,

	// PARAM
	NCL_ERR_PARAM_NOT_LOADED = 600,
	NCL_ERR_PARAM_DB_OPEN_FAILED,
	NCL_ERR_PARAM_FAILED,

	// COILINFO
	NCL_ERR_COILINFO_PARSE_FAILED = 700,

	// PRELOAD_PARAM
    NCL_ERR_PRELOAD_DB_OPEN_FAILED = 800,
	NCL_ERR_PRELOAD_DB_FETCH_FAILED,

	// PRESET_POSITION
	NCL_ERR_SET_POSITION_FAILED = 1000,

	// QUERY_STATUS
	NCL_ERR_QUERY_STATUS_FAILED = 1100,

	// 非同期エラー
	NCL_ERR_RECV_EDGE_INFO = -1000,			// エッジ位置通信エラー
	NCL_ERR_SEND_EDGE_INFO,
	NCL_ERR_RECV_EDGE_INFO_TIMEOUT,			// エッジ位置通信タイムアウト
	NCL_ERR_SEND_EDGE_INFO_TIMEOUT,
	NCL_ERR_ASYNC_PARAM_FAILED,				// フレーム番号指定のPARAMコマンドの失敗
	NCL_ERR_ASYNC_SET_POSITION_FAILED,		// フレーム番号指定のSET_POSITIONコマンドの失敗
	// イベント
	NCL_ERR_FRAME_SKIPPED = -2000,			// フレームスキップ発生
	NCL_ERR_ASYNC_PARAM_SUCCESS,			// フレーム番号指定のPARAMコマンドの成功
	NCL_ERR_ASYNC_SET_POSITION_SUCCESS		// フレーム番号指定のSET_POSITIONコマンドの成功
};

// NCL_ERRCODEを文字列に変換する
#define ERR_ITEM(x) case x: return _T(#x)
static const TCHAR* GetErrMsg(NCL_ERRCODE err)
{
    switch( err )
    {
        ERR_ITEM(NCL_SUCCESS);
		/*
        ERR_ITEM(ERR_CMD_UNEXPECTED);
        ERR_ITEM(ERR_CMD_UNKNOWN);
        ERR_ITEM(ERR_INIT_SMEM);
        ERR_ITEM(ERR_INIT_CPIPE);
        ERR_ITEM(ERR_INIT_RPIPE);
        ERR_ITEM(ERR_INIT_SOCKET);
        ERR_ITEM(ERR_INIT_MIL);
        ERR_ITEM(ERR_INIT_JPEG);
        ERR_ITEM(ERR_INIT_IMAGEDISP);
        ERR_ITEM(ERR_INIT_DB);
        ERR_ITEM(ERR_INIT_GRABBER);
        ERR_ITEM(ERR_INIT_IPC);
        ERR_ITEM(ERR_INIT_MANAGER);
        ERR_ITEM(ERR_INIT_INVALID_ARGS);
        ERR_ITEM(ERR_EXIT_FAILED);
        ERR_ITEM(ERR_START_FAILED);
        ERR_ITEM(ERR_RESET_FAILED);
        ERR_ITEM(ERR_STOP_FAILED);
        ERR_ITEM(ERR_COILINFO_INVALID);
        ERR_ITEM(ERR_PARAM_DB_OPEN_EXCEPTION);
        ERR_ITEM(ERR_PARAM_RETRIEVE_FAILED);
        ERR_ITEM(ERR_PARAM_RETRIEVE_EXCEPTION);
        ERR_ITEM(ERR_PARAM_EDGE_FAILED);
        ERR_ITEM(ERR_PARAM_DETECT_FAILED);
        ERR_ITEM(ERR_PARAM_POST_FAILED);
        ERR_ITEM(ERR_PARAM_DCD_LOAD_FAILED);
        ERR_ITEM(ERR_PARAM_CLFR_FAILED);
        ERR_ITEM(ERR_PARAM_SYNCADD_FAILED);
        ERR_ITEM(ERR_PARAM_HDRI_FAILED);
        ERR_ITEM(ERR_NOCOILINFO);
        ERR_ITEM(ERR_STARTGRAB_FAILED);
        ERR_ITEM(ERR_NOP_FAILED);
        ERR_ITEM(ERR_ABORT_FAILED);
        ERR_ITEM(ERR_FRAMEIMAGE_TOO_MANY_REQ);
        ERR_ITEM(ERR_FRAMEIMAGE_NOT_FOUND);
        ERR_ITEM(ERR_FRAMEIMAGE_ENCODE_FAILED);
        ERR_ITEM(ERR_FRAMEIMAGE_ENCODE_BUFF_TOO_SMALL);
        ERR_ITEM(ERR_TOP_NOT_DETECTED);
        ERR_ITEM(ERR_BOT_NOT_DETECTED);
        ERR_ITEM(ERR_SMEM_OVERFLOW);
        ERR_ITEM(ERR_TOO_MANY_WAITING_EDGEPOS);
        ERR_ITEM(ERR_NO_FREE_EDGEPOS_LEFT);
		*/
    }

	return _T("Unknown");
}

//  ///////////////////////////////////
// 定数
static const int NCL_ncam_max = 8;			// 判定１台あたりのカメラ台数
static const long NCL_nattr_common = 256;	// 共通特徴量
static const long NCL_nattr_angle  = 256;	// 角度毎の個数
static const int NCL_nperiod_hist = 8;		// 単発簡易周期検出の個数

// ///////////////////////////////////
// コマンドの構造体 (追加データなし)
// DISPOSE, EXIT, START, STOP, QUERY_STATUS
struct NCL_CMD
{
	NCL_CMDCODE	cmd;
};


struct NCL_CMD_RESUME_START : public NCL_CMD {
	DWORD	start_frame_num;		//最初にGrabできるフレームの番号。
};

// コマンドの構造体(初期化コマンド)
struct NCL_CMD_INIT
{
	NCL_CMDCODE	cmd;			// NCL_INIT
	DWORD	    camnum;			// カメラ台数
	int         camid[NCL_ncam_max];			// ±1〜, 負の値は、エッジが受信のみであることを示す。カメラ番号としては絶対値を使う。
	long		initial_scode;	// scode初期値
	TCHAR       dbconnstr[256];
};

// コマンドの構造体(初期化コマンド。RPI入力)
struct NCL_CMD_INIT_RPISRC : public NCL_CMD_INIT
{
	long		frame_start;	// skip frame.
	long		frame_end;		// end frame.
	TCHAR		rpi_filename[256];//camera２は 256*1~, 3は256*2~に書くこと。扱いは、宣言がrpi_filename[1]と同じ。
};

struct NCL_CMD_SAVE_DDS : public NCL_CMD
{
	long long free_space;	//容量管理を行う場合、あいていてほしいサイズ(byte単位)。0は容量管理しない。
	TCHAR	dds_path[256];	//保存先パス。次のコイル変更で利く。カラ文字列を渡すと、DDS保存停止。
};

// コマンドの構造体(パラメータ設定)
struct NCL_CMD_PARAM
{
	NCL_CMDCODE	cmd;			// NCL_PARAM
	DWORD       scode;			// 表面状態コード
	DWORD		framenum;		// 実行フレーム番号
};

// コマンドの構造体(上位情報設定)
struct NCL_CMD_COILINFO
{
	NCL_CMDCODE	cmd;			// NCL_COILINFO
	DWORD		info_size;
	BYTE		coil_info[1];
};

// コマンドの構造体(パラメータ先読み)
struct NCL_CMD_PRELOAD_PARAM
{
	NCL_CMDCODE	cmd;			// NCL_PARAM
	DWORD       scode;			// 表面状態コード
};

// コマンドの構造体(距離カウンタセット)
enum NCL_SET_POSITION_MASK
{
	FRONT =	0x1,
	TAIL  = 0x2,
	SHEAR = 0x4,
	UNIV_UP0 = 0x8,
	UNIV_UP1 = 0x10,
	UNIV_UP2 = 0x20,
	UNIV_UP3 = 0x40,
	UNIV_DN0 = 0x80,
	UNIV_DN1 = 0x100,
	UNIV_DN2 = 0x200,
	UNIV_DN3 = 0x400,
	ALL = 0xfff
};

struct NCL_CMD_SET_POSITION
{
	NCL_CMDCODE		cmd;			// NCL_SET_POSITION
	DWORD			update_mask;	// 更新するカウンタを指定するビットパターン
	DWORD			framenum;		// 実行フレーム番号
	float			front_pos;		// 加算カウンタ
	float			tail_pos;		// 減算カウンタ
	float			shear_cut_pos;	// 加算カウンタ
	float			univ_cnt_up_pos[4];		// 汎用加算カウンタ
	float			univ_cnt_dn_pos[4];		// 汎用減算カウンタ
};


// ////////////////////////////////////
// 戻り値(非同期エラー)構造体
struct NCL_ANSWER
{
	NCL_ERRCODE			retcode;
	NCL_CMDCODE			cmd;		// 何のコマンドに対するエラーコードか？
};

// QUERY_STATUSの戻り構造体
struct NCL_ANSWER_QUERY_STATE
{
	NCL_ERRCODE			retcode;
	NCL_CMDCODE			cmd;			// NCL_QUERY_STATUS
	NCL_ENGINE_STATE	state;
	DWORD				camnum;						// カメラ台数
	DWORD				camid[NCL_ncam_max];		// カメラ番号
	float				exptime[3][NCL_ncam_max];	// 色別
	float				gain[3][NCL_ncam_max];		// 色別
	float				temp[NCL_ncam_max];			// カメラ温度
	float				volt[NCL_ncam_max];			// カメラ電圧
};

// {467D5741-9583-45FA-92A1-E824704482B2}
static const GUID ID_FIRST_SHMEM_INFO_IDv1 =
{ 0x467d5741, 0x9583, 0x45fa,{ 0x92, 0xa1, 0xe8, 0x24, 0x70, 0x44, 0x82, 0xb2 } };

static const int FIRST_SHMEM_SIZE = 4096;

static const char SerialResultTimeout[] = "<Timeout>";

struct SerialResult 
{
	DWORD Tick;
	char SendTo[32];
	char Result[32];
	DWORD TickGcp;
	char Gcp[2048];
};

struct FIRST_SHMEM_INFOv1 {
	GUID first_smem_req_id;
	long long			UpdateTime;						// 項目更新シリアル(GetTickCount())
	DWORD				grab_frame;						// 現在撮像中のした最新フレームNo
	DWORD				camnum;							// カメラ台数
	DWORD				camid[NCL_ncam_max];			// カメラ番号

	long				exptime[NCL_ncam_max][3];		// 色別
	float				gain[NCL_ncam_max][3];			// 色別
	float				matrix[NCL_ncam_max][9];	
	float				temp[NCL_ncam_max];				// カメラ温度
	float				volt[NCL_ncam_max];				// カメラ電圧
	SerialResult		serial_result[NCL_ncam_max];	// GCPコマンドのリターン文字列
};

// ////////////////////
// エッジ範囲制限UDPパケット
// ////////////////////
struct NCL_EDGE_LIMIT
{
	BOOL	use_limit;
	float	left_outer;
	float	left_inner;
	float	right_inner;
	float	right_outer;
};

struct NCL_EDGE_RESTRICT_DATA
{
	DWORD	frame_num;
	NCL_EDGE_LIMIT	limit[2];	// 0:top, 1:bot
};


// ////////////////////
// パイプデータ
// ////////////////////

struct NCL_SMEM_PIPE_DATA
{
	DWORD    frame_num;		// 探傷開始からの連番
	DWORD    camid;			// カメラ番号
	long     smem_offset;
	long     smem_size;
};

// NCL_SMEM_PIPE_DATAのsizeが0のときにoffsetに格納する値
// フレーム画像を常に返すために、共有メモリがいっぱいの場合しかエラーは無い。
// 他のエラーはヘッダに書かれる
enum SMEM_PIPEDATA_OFFSET
{
    NCL_SMEMPIPE_SMEMFULL = -1 // 共有メモリがいっぱいだった
};

// ////////////////////
// 共有メモリ
// ////////////////////

// //////////////////////
// ステータスエリアは無い


// メモリレイアウト  ////////////////////////////////////////////////
// フレームヘッダ
// 絶対エッジ位置[mm] float left[height], right[height]
// フレーム画像	width*height*4
// 結果 NCL_DEFECT_RESULT * defect_num
// 特徴量 double[NCL_nattr_common+4*NCL_nattr_angle] * defect_num
// 切り出し画像	NCL_IMAGE_HEADER * image_num


// フレームヘッダでの探傷状態定義
enum	NCL_FRAME_STATUS
{
	STATUS_VALID = 0,		// このフレームに板がある
	STATUS_INVALID,			// 板が無い
	STATUS_EDGELOST,		// 処理をスキップした(エッジ検出に失敗したため)
    STATUS_PROCSKIP,		// 処理をスキップした(何らかの理由で)
    STATUS_TOOMANY,			// 処理をスキップした(疵候補の数が上限を超えたため)
    STATUS_QFULL,			// 処理をスキップした(処理待ち数が上限を超えそうだったため)
	STATUS_UNKNOWN
};

// フレームヘッダ
struct NCL_SMEM_FRAME_HEADER
{
	DWORD	frame_num;
	DWORD   camid;
	NCL_FRAME_STATUS	status;

	long     defect_num;
	long     image_num;

	DWORD	 edge_offset;
	DWORD    frame_offset;
	DWORD    result_offset;
	DWORD    attr_offset;
	DWORD    image_offset;

	BYTE	frame_brightness[4];	// RGB

	// 現カウンタ値
	float			front_pos;		// 加算カウンタ
	float			tail_pos;		// 減算カウンタ
	float			shear_cut_pos;	// 加算カウンタ
	float			univ_cnt_up_pos[4];		// 汎用加算カウンタ
	float			univ_cnt_dn_pos[4];		// 汎用減算カウンタ

	DWORD executed_position_mask;	  //NCL_SET_POSITION_MASK このフレームで初期化されたカウンタ。

	//PDのみ。加算画像へのオフセット
	DWORD pd_frame_offset;
};

enum NCL_DEFECT_FLAGS
{
	NORMAL = 0,		// 通常疵
	PERIODIC = 1,		// 簡易周期での周期疵
	SERIES = 2,			// 簡易周期での連続疵
	START = 4,			// 簡易周期での最初の疵フラグ
	SYNC_ADD = 8,		// 画像加算での周期疵
	OTHER
};

struct NCL_DEFECT_RESULT
{
	long    type;
	long    grade;
	long    group;
	long    flags;				// NCL_DEFECT_FLAGSの値
	long    im_idx;
};

struct NCL_IMAGE_HEADER			// 切り出し画像
{
	DWORD	left;				// 切り出し左上座標。（縮小座標系）
	DWORD	top;				// 同上
	DWORD   width;
	DWORD   height;
	float	shrinkratio;			//切り出し画像縮小率
	DWORD	res[3];

	BYTE    image[1];			// width*height*(RGB,DIFF)4枚
};

// frame_offsetの指し示すさき。
//生画像（縮小あり）
struct NCL_FRAME_IMAGE{
    DWORD width;
    DWORD height;
    DWORD stride;
    DWORD channel;
    BYTE image[1];              // stride*height*channel枚
};

// pd_frame_offsetの指し示す先
// 周期加算画像
struct NCL_PERIOD_IMAGE {
	DWORD   nAddIndex;      //加算番号（何番目のロールか）
	DWORD	nFrameStart;    //加算開始フレームNO
	DWORD   width;			//周期重ねあわせ画像の幅
	DWORD   height;			//周期重ねあわせ画像の高
	BYTE    image[1];		// width*height*(RGB,DIFF)4枚
};
// UDPでカメラのシリアル速度切り替えを統括に依頼する
static const WORD cam_ser_udp_port = 8999;
static const int cam_ser_packet_id = 1111;
struct CAM_SERIAL_SPEED
{
	int packetid;
	DWORD	camid;		// カメラ番号(1〜)
	DWORD	speed;		// 9600 or 115200
};

#pragma pack(pop)

#ifndef _DEBUG
// ////////////////////
// 共有メモリの名前
// ////////////////////
static const char* name_shmem = "shmem_ncldetect";

// ////////////////////
// コマンドパイプの名前
// ////////////////////
static const char* name_cpipe = "\\\\.\\pipe\\NCL_cpipe";

// ////////////////////
// 探傷結果パイプの名前
// ////////////////////
static const char* name_rpipe = "\\\\.\\pipe\\NCL_rpipe";
#else
static const char* name_shmem = "shmem_ncldetect_debug";
static const char* name_cpipe = "\\\\.\\pipe\\NCL_cpipe_debug";
static const char* name_rpipe = "\\\\.\\pipe\\NCL_rpipe_debug";
#endif

// DB列名
static const char* CAMNAME = "[CAMSET]";		// データベースの検索キー列名
static const char* SCODENAME = "[SCODE]";		// データベースの検索キー列名
static const char* ANGLENAME = "[CAMANGLE]";	// データベースの検索キー列名
