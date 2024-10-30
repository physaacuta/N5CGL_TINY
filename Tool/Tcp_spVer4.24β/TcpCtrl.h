#ifndef _TCP_CTRL_H
#define _TCP_CTRL_H

#include    <windows.h>

#define		TCP_CTRL_NAME   	"tcp_ctrl.dat"
#define     LIST_DATA_MAX       1024
//#define     COM_BUF_MAX         10240       // 送受信ﾊﾞｯﾌｧｻｲｽﾞ
#define     COM_BUF_MAX         102400       // 送受信ﾊﾞｯﾌｧｻｲｽﾞ

#define     SHORT_TYPE          1
#define     LONG_TYPE           2
#define     CHAR_TYPE           3
#define     HEX1_TYPE           4
#define     HEX2_TYPE           5
#define     HEX4_TYPE           6
#define     EBCDIC_TYPE         7
#define     XSHORT_TYPE         11
#define     XLONG_TYPE          12
#define     XHEX2_TYPE          15
#define     XHEX4_TYPE          16

#define     SCH_TBL_MAX         1024
#define     SCH_ITEM_MAX		5
#define     FNAME_MAX           128
#define     SINA_TBL_MAX        512
#define     RECV_SINA_MAX		64
#define     RECV_SINA_TBL_MAX   128

#define     CNT_CHAR_LONG       1
#define     CNT_CHAR_CHAR       2
#define     CNT_CHAR_SET        3
#define     CNT_LONG_LONG       4
#define     CNT_LONG_SET        5
#define     CNT_CNT_ON          6

#define     MAX_TARGET          4
#define		TYPE_MAX			12

#define     COUNTER_LINK_PORT   49890
#define     COUNTER_LINK_IP     "127.0.0.1"

#define     PORT1       1
#define     PORT2       2
#define     PORT3       3                       // ｶｳﾝﾀ画面ﾘﾝｹｰｼﾞﾎﾟｰﾄ (UDP)

typedef struct {
    int     connectType[2];     // 接続タイプ　1=CONNECT　2=ACCEPT
    char    ip[2][28];          // IP
    char    port[2][12];        // PORT
    int     comType;            // 伝文タイプ　0=ﾉｰﾏﾙ 1=三菱ｼｰｹﾝｻ 2=予備
    int     traceCode;          // ﾄﾚｰｽ出力ｺｰﾄﾞ
    int     traceOutSend;
    int     traceOutRecv;
    char    traceSave;
    char    traceName[256];
	int     recvChkFile;
	int		recvChkFileName;
	char    recvFileName[256];
    char    recvFileBinName[256];
    int     schCtrl;            // ｽｹｼﾞｭｰﾙ管理有無
    int     cycCtrl;            // ﾀｲﾏｰ管理有無
	char    schFileName[256];	// ｽｹｼﾞｭｰﾙ管理ﾌｧｲﾙ名
    char    homeDir[256];       // ｶﾚﾝﾄﾃﾞｨﾚｸﾄﾘｨ
    char    listData[256];		// 送信ﾘｽﾄBOX保存
    int     fileForm;			// ファイル読み込み形式　1:ﾃｷｽﾄﾀｲﾌﾟ  2:定義ﾌｧｲﾙ  3:binﾌｧｲﾙ
    char    editName[512];		// ｴﾃﾞｨﾀｰ指定
	int		tcp_udp;			// 通信区分 0:TCP	1:UDP
    int     sinaCtrl;           // ｼﾅﾘｵ管理有無
	char    sinaFileName[256];	// ｼﾅﾘｵ管理ﾌｧｲﾙ名
	int		rsina_stop;			// 受信ｼﾅﾘｵ一時停止
	int     sina_stop;			// ｼﾅﾘｵ一時停止
    int     retry_flg;          // 永久リトライ指定
    char    counterFile[256];   // カウンターファイル
    int     eb_mojicode;        // Ｅｎｅｔ－Ｂ文字コード
    int     eb_transpar;        // Ｅｎｅｔ－Ｂ透過／非透過
    int     tm_ak1_atxt;        // ﾃｷｽﾄ送信後ACK1までの時間
    int     tm_ak0_aenq;        // ENQ送信後ACK0までの時間
    int     tm_txt_aak0;        // ACK0送信後ﾃｷｽﾄ受信までの時間
    int     tm_eot_aak1;        // ACK1送信後EOTまでの時間
    int     tm_enq_anak;        // ACK0待ちでNAK受信時のENQ再送までの時間
    int     nm_enq_nans;        // 無応答に対するENQ送信回数
    int     nm_txt_anak;        // ﾃｷｽﾄ送信後NAKに対するﾃｷｽﾄ送信回数
	char	yobi[216];
} TCP_CTRL; 

////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////

typedef struct {
    int     addr;
    int     type;                   /* 1: SHOET                 */
                                    /* 2: LONG                  */
                                    /* 3: CHAR                  */
                                    /* 4: HEX                   */
    int     length;                 /* ﾁｪｯｸﾃﾞｰﾀ長               */
    unsigned char    ptn[32];		/* ﾁｪｯｸﾊﾟﾀｰﾝ                */
} ScheduleItem;

typedef struct {
    int     port;                   /* 送信ポート (1 or 2)      */
    int     num;                    /* 検索条件数               */
    ScheduleItem    item[SCH_ITEM_MAX];    /* サブ条件          */
	int		fnum;					/* 選択ﾌｧｲﾙ数				*/
	int		randam;					/* ﾗﾝﾀﾞﾑﾌｧｲﾙ				*/
    char    fname[FNAME_MAX];       /* 定義ﾌｧｲﾙ名               */
} ScheduleDef;


typedef struct {
    int     port;                   /* 送信ポート (1 or 2)      */
    int     sec;                    /* 定周期（秒）             */
    char    fname[FNAME_MAX];       /* 定義ﾌｧｲﾙ名               */
    int     count;
} CycleSchDef;


typedef struct {
    int     port;                   /* 送信ポート (1 or 2)      */
    int     sec;                    /* 定周期（秒）             */
    char    fname[FNAME_MAX];       /* 定義ﾌｧｲﾙ名               */
    int     count;
} SinaSchDef;

typedef struct {
    int     use;                    /* 使用有無                 */
    int     port;                   /* 送信ポート (1 or 2)      */
    int     sec;                    /* 定周期（秒）             */
    char    fname[FNAME_MAX];       /* 定義ﾌｧｲﾙ名               */
    int     count;
} RecvSinaSchDef;


typedef struct {
    int     idata;                  /* 現在値                   */
    int     target[MAX_TARGET];     /* 目標値                   */
    int     count;                  /* カウント値               */
    int     kubun;                  /* データ区分               */
    int     iyobi[9];               /* 予備                     */
    char    msg[32];                /* メッセージ               */
    char    cyobi[32];              /* 予備                     */
} CountFile;

typedef struct {
    char    cdata[24];              /* 文字列                   */
    int     kubun;                  /* データ区分               */
    int     iyobi[9];               /* 予備                     */
    char    msg[32];                /* メッセージ               */
    char    cyobi[32];              /* 予備                     */
} StrFile;

typedef struct {
    int     num;                    /* 検索条件数               */
    ScheduleItem    item[SCH_ITEM_MAX];    /* サブ条件          */
    int     type;
    int     kind;
    int     addr;
    int     len;
    int     rec;
    int     set;
    char    ctext[32];
} CountDef;



static char  connectType[TYPE_MAX][20] = {
	{"ﾉｰﾏﾙ       "},
	{"MELSEC     "},
    {"対PCTERM   "},
    {"intel      "},
    {"intel X    "},
    {"都度FMT    "},
    {"都度FMT X  "},
    {"安川CP-218 "},
    {"ﾚｽﾎﾟﾝｽ指定 "},
    {"ﾉｰﾏﾙ 01→02"},
    {"Enet-B     "},
    {"Enet-BX    "} };

#define     NORMAL      0
#define     MELSEC      1
#define     PCTERM      2
#define     INTEL       3
#define     X_INTEL     4
#define     SOZ_FMT     5
#define     X_SOZ_FMT   6
#define     YASUKAWA    7
#define     RESPONS     8
#define     PASS_1TO2   9
#define     ENET_B      10
#define     ENET_BX     11


//----------------------------------------------------------------
// ｸﾞﾛｰﾊﾞﾙ宣言
//----------------------------------------------------------------
TCP_CTRL        tcpCtrl;                        // 制御情報ｴﾘｱ

SOCKET			g_sSock[3];					    // ｿｹｯﾄ
SOCKET			g_sSockClient[3];               // ｸﾗｲｱﾝﾄｿｹｯﾄ

int             g_CounterSendPort;
int             g_CounterRecvPort;

unsigned char   g_pSendDmy[COM_BUF_MAX];        // 送信ﾜｰｸｴﾘｱ
unsigned char   g_sInitBuf[16];
unsigned char   g_lInitBuf[16];
unsigned char   g_xsInitBuf[16];
unsigned char   g_xlInitBuf[16];
unsigned char   g_pcterm_pa;

ScheduleDef     SchDef[SCH_TBL_MAX];            // 受信ｽｹｼﾞｭｰﾙ格納ｴﾘｱ
CycleSchDef     CycDef[SCH_TBL_MAX];            // 定周期ｽｹｼﾞｭｰﾙ格納ｴﾘｱ
SinaSchDef      SinaDef[SINA_TBL_MAX+1];        // ｼﾅﾘｵ格納ｴﾘｱ
CountDef        CntDef[SCH_TBL_MAX];            // 受信ｶｳﾝﾄｴﾘｱ
int             SinaDefCount;                   // 経過秒数
int             SinaPoint;                      // 次に実行するｼﾅﾘｵ格納ｴﾘｱのﾎﾟｲﾝﾀ
                                                // 受信ｼﾅﾘｵ格納ｴﾘｱ
RecvSinaSchDef  RecvSinaDef[RECV_SINA_MAX][RECV_SINA_TBL_MAX+1];
RecvSinaSchDef  NextRecvSinaDef[RECV_SINA_MAX]; // 次に実行する受信ｼﾅﾘｵ情報
int             RecvSinaPoint[RECV_SINA_MAX];   // 次に実行する受信ｼﾅﾘｵ格納ｴﾘｱのﾎﾟｲﾝﾀ
                                                // 受信したﾃﾞｰﾀ格納ﾊﾞｯﾌｧ
unsigned char   RecvSinaData[RECV_SINA_MAX][COM_BUF_MAX];


int			    Sch_no[SCH_TBL_MAX];
unsigned char	G_respons[3];
unsigned char	G_hed[3];

int             g_flag;
int             g_setAddr;                      // 定義ﾌｧｲﾙ伝文作成ｱﾄﾞﾚｽ
unsigned char	g_OpenName[1024];               // ｵｰﾌﾟﾝ選択したﾌｧｲﾙ名称

LPSTR           GlpszCmdLine;
int             GnCmdShow;
HWND            setting_hWnd;

static  char*   g_ok_msg;
static  int     g_okng;
static  char*   g_user_msg;
static  char*   g_user_title;
//----------------------------------------------------------------
// 関数宣言
//----------------------------------------------------------------
void    MyMessage( HWND hWnd, const char *__cBuffer, ... );
void    timerSetProc(HWND hWnd);
void    timerResetProc(HWND hWnd);
void    timerSetProc1(HWND hWnd);
void    timerResetProc1(HWND hWnd);
void    timerSetProc2(HWND hWnd);
void    timerResetProc2(HWND hWnd);
void    timerSetProc3(HWND hWnd, int timerid);
void    timerResetProc3(HWND hWnd, int timerid);
void    MakeFullName(HWND hWnd, char* fname, char* fullName);
void	setDialogName(HWND hwnd, char* fullname);

BOOL    loadListData(HWND hWnd, int kind);
BOOL    ScheduleDataSet(HWND hWnd);
BOOL    SinaDataSet(HWND hWnd);
BOOL    CtrlFileSave(HWND hWnd);
BOOL    CtrlFileLoad(HWND hWnd);
BOOL    getTcpCtrl(HWND hWnd);
BOOL    putTcpCtrl(HWND hWnd, char*);

int     UserMsgBox( HWND hWnd, char* msg, char* title );
void    OkNgMsgBox(HWND hWnd, char * msg, char* title);
int     getOffsetByComType(void);

#endif


