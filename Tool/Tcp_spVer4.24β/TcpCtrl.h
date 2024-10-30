#ifndef _TCP_CTRL_H
#define _TCP_CTRL_H

#include    <windows.h>

#define		TCP_CTRL_NAME   	"tcp_ctrl.dat"
#define     LIST_DATA_MAX       1024
//#define     COM_BUF_MAX         10240       // ����M�ޯ̧����
#define     COM_BUF_MAX         102400       // ����M�ޯ̧����

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
#define     PORT3       3                       // ��������ݹ����߰� (UDP)

typedef struct {
    int     connectType[2];     // �ڑ��^�C�v�@1=CONNECT�@2=ACCEPT
    char    ip[2][28];          // IP
    char    port[2][12];        // PORT
    int     comType;            // �`���^�C�v�@0=ɰ�� 1=�O�H���ݻ 2=�\��
    int     traceCode;          // �ڰ��o�ͺ���
    int     traceOutSend;
    int     traceOutRecv;
    char    traceSave;
    char    traceName[256];
	int     recvChkFile;
	int		recvChkFileName;
	char    recvFileName[256];
    char    recvFileBinName[256];
    int     schCtrl;            // ���ޭ�يǗ��L��
    int     cycCtrl;            // ��ϰ�Ǘ��L��
	char    schFileName[256];	// ���ޭ�يǗ�̧�ٖ�
    char    homeDir[256];       // �����ިڸ�ب
    char    listData[256];		// ���Mؽ�BOX�ۑ�
    int     fileForm;			// �t�@�C���ǂݍ��݌`���@1:÷������  2:��`̧��  3:biņ��
    char    editName[512];		// ��ި���w��
	int		tcp_udp;			// �ʐM�敪 0:TCP	1:UDP
    int     sinaCtrl;           // ��ص�Ǘ��L��
	char    sinaFileName[256];	// ��ص�Ǘ�̧�ٖ�
	int		rsina_stop;			// ��M��ص�ꎞ��~
	int     sina_stop;			// ��ص�ꎞ��~
    int     retry_flg;          // �i�v���g���C�w��
    char    counterFile[256];   // �J�E���^�[�t�@�C��
    int     eb_mojicode;        // �d�������|�a�����R�[�h
    int     eb_transpar;        // �d�������|�a���߁^�񓧉�
    int     tm_ak1_atxt;        // ÷�đ��M��ACK1�܂ł̎���
    int     tm_ak0_aenq;        // ENQ���M��ACK0�܂ł̎���
    int     tm_txt_aak0;        // ACK0���M��÷�Ď�M�܂ł̎���
    int     tm_eot_aak1;        // ACK1���M��EOT�܂ł̎���
    int     tm_enq_anak;        // ACK0�҂���NAK��M����ENQ�đ��܂ł̎���
    int     nm_enq_nans;        // �������ɑ΂���ENQ���M��
    int     nm_txt_anak;        // ÷�đ��M��NAK�ɑ΂���÷�đ��M��
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
    int     length;                 /* �����ް���               */
    unsigned char    ptn[32];		/* ���������                */
} ScheduleItem;

typedef struct {
    int     port;                   /* ���M�|�[�g (1 or 2)      */
    int     num;                    /* ����������               */
    ScheduleItem    item[SCH_ITEM_MAX];    /* �T�u����          */
	int		fnum;					/* �I��̧�ِ�				*/
	int		randam;					/* �����̧��				*/
    char    fname[FNAME_MAX];       /* ��`̧�ٖ�               */
} ScheduleDef;


typedef struct {
    int     port;                   /* ���M�|�[�g (1 or 2)      */
    int     sec;                    /* ������i�b�j             */
    char    fname[FNAME_MAX];       /* ��`̧�ٖ�               */
    int     count;
} CycleSchDef;


typedef struct {
    int     port;                   /* ���M�|�[�g (1 or 2)      */
    int     sec;                    /* ������i�b�j             */
    char    fname[FNAME_MAX];       /* ��`̧�ٖ�               */
    int     count;
} SinaSchDef;

typedef struct {
    int     use;                    /* �g�p�L��                 */
    int     port;                   /* ���M�|�[�g (1 or 2)      */
    int     sec;                    /* ������i�b�j             */
    char    fname[FNAME_MAX];       /* ��`̧�ٖ�               */
    int     count;
} RecvSinaSchDef;


typedef struct {
    int     idata;                  /* ���ݒl                   */
    int     target[MAX_TARGET];     /* �ڕW�l                   */
    int     count;                  /* �J�E���g�l               */
    int     kubun;                  /* �f�[�^�敪               */
    int     iyobi[9];               /* �\��                     */
    char    msg[32];                /* ���b�Z�[�W               */
    char    cyobi[32];              /* �\��                     */
} CountFile;

typedef struct {
    char    cdata[24];              /* ������                   */
    int     kubun;                  /* �f�[�^�敪               */
    int     iyobi[9];               /* �\��                     */
    char    msg[32];                /* ���b�Z�[�W               */
    char    cyobi[32];              /* �\��                     */
} StrFile;

typedef struct {
    int     num;                    /* ����������               */
    ScheduleItem    item[SCH_ITEM_MAX];    /* �T�u����          */
    int     type;
    int     kind;
    int     addr;
    int     len;
    int     rec;
    int     set;
    char    ctext[32];
} CountDef;



static char  connectType[TYPE_MAX][20] = {
	{"ɰ��       "},
	{"MELSEC     "},
    {"��PCTERM   "},
    {"intel      "},
    {"intel X    "},
    {"�s�xFMT    "},
    {"�s�xFMT X  "},
    {"����CP-218 "},
    {"ڽ��ݽ�w�� "},
    {"ɰ�� 01��02"},
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
// ��۰��ِ錾
//----------------------------------------------------------------
TCP_CTRL        tcpCtrl;                        // ������ر

SOCKET			g_sSock[3];					    // ����
SOCKET			g_sSockClient[3];               // �ײ��Ŀ���

int             g_CounterSendPort;
int             g_CounterRecvPort;

unsigned char   g_pSendDmy[COM_BUF_MAX];        // ���Mܰ��ر
unsigned char   g_sInitBuf[16];
unsigned char   g_lInitBuf[16];
unsigned char   g_xsInitBuf[16];
unsigned char   g_xlInitBuf[16];
unsigned char   g_pcterm_pa;

ScheduleDef     SchDef[SCH_TBL_MAX];            // ��M���ޭ�يi�[�ر
CycleSchDef     CycDef[SCH_TBL_MAX];            // ��������ޭ�يi�[�ر
SinaSchDef      SinaDef[SINA_TBL_MAX+1];        // ��ص�i�[�ر
CountDef        CntDef[SCH_TBL_MAX];            // ��M���Ĵر
int             SinaDefCount;                   // �o�ߕb��
int             SinaPoint;                      // ���Ɏ��s�����ص�i�[�ر���߲��
                                                // ��M��ص�i�[�ر
RecvSinaSchDef  RecvSinaDef[RECV_SINA_MAX][RECV_SINA_TBL_MAX+1];
RecvSinaSchDef  NextRecvSinaDef[RECV_SINA_MAX]; // ���Ɏ��s�����M��ص���
int             RecvSinaPoint[RECV_SINA_MAX];   // ���Ɏ��s�����M��ص�i�[�ر���߲��
                                                // ��M�����ް��i�[�ޯ̧
unsigned char   RecvSinaData[RECV_SINA_MAX][COM_BUF_MAX];


int			    Sch_no[SCH_TBL_MAX];
unsigned char	G_respons[3];
unsigned char	G_hed[3];

int             g_flag;
int             g_setAddr;                      // ��`̧�ٓ`���쐬���ڽ
unsigned char	g_OpenName[1024];               // ����ݑI������̧�ٖ���

LPSTR           GlpszCmdLine;
int             GnCmdShow;
HWND            setting_hWnd;

static  char*   g_ok_msg;
static  int     g_okng;
static  char*   g_user_msg;
static  char*   g_user_title;
//----------------------------------------------------------------
// �֐��錾
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


