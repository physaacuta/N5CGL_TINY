/***********************************************************************************
*
*  Ｅｎｅｔ−Ｂヘッダー
*
*  AUTHOR          : 加賀谷 大輔
*  DATE            : 2006.09.28(待合せツール)
*                    2010.07.14(TcpSp 初版)
*
*  DEVISION 001    : ____.__.__ ( ＿＿＿：＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿
*  DEVISION 002    : ____.__.__ ( ＿＿＿：＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿
*  DEVISION 003    : ____.__.__ ( ＿＿＿：＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿
*
***********************************************************************************/
#ifndef _ENET_B_H
#define _ENET_B_H


/**********************************************************************************/
/*       インクルード宣言                                                         */
/**********************************************************************************/

#include      "dump.h"
#include      "TcpCtrl.h"

/**********************************************************************************/
/*       送信中テキスト格納バッファ                                               */
/**********************************************************************************/

#define            ENETB_MAXSIZE    1400    /* 伝文最大サイズ */
#define            ENETB_MXPOINT      20    /* バッファレコード数 */

typedef struct {
    int     in_point;                                 /* ＩＮポインタ */
    int     out_point;                                /* ＯＵＴポインタ */
    int     snd_size[ENETB_MXPOINT];                  /* 送信伝文サイズ */
    char    snd_buff[ENETB_MXPOINT][ENETB_MAXSIZE];   /* 送信中バッファ */
} EnetbSndBuf;

EnetbSndBuf        eb_sndbuf;               /* バッファグローバル配列 */

/**********************************************************************************/
/*       受信中テキスト格納バッファ                                               */
/**********************************************************************************/

char               g_eb_rcvbuf1[ENETB_MAXSIZE];       /* ポート１受信中バッファ */
char               g_eb_rcvkey1[FNAME_MAX];           /* ポート１受信中キーコード */
int                g_eb_rcvsiz1;                      /* ポート１受信中サイズ */
char               g_eb_rcvbuf3[ENETB_MAXSIZE];       /* ポート３受信中バッファ */
char               g_eb_rcvkey3[FNAME_MAX];           /* ポート３受信中キーコード */
int                g_eb_rcvsiz3;                      /* ポート３受信中サイズ */

/**********************************************************************************/
/*       送信中状態                                                               */
/**********************************************************************************/

int                g_eb_sndst;              /* 送信中グローバル変数 */

#define            EB_NOTSND           0    /* 送信中でない */
#define            EB_ENQSND           1    /* ＥＮＱ送信中 */
#define            EB_TXTSND           2    /* テキスト送信中 */
#define            EB_AK0SND           3    /* ＡＣＫ０送信中 */
#define            EB_AK1SND           4    /* ＡＣＫ１送信中 */

/**********************************************************************************/
/*       リトライカウンタ                                                         */
/**********************************************************************************/

int                g_eb_cntenq;             /* ＥＮＱ送信リトライカウンタ */
int                g_eb_cnttxt;             /* テキスト送信リトライカウンタ */

/**********************************************************************************/
/*       透過／非透過定数（TcpSp 初版）                                           */
/**********************************************************************************/

#define            EB_NOTTRNS          0    /* 非透過 */
#define            EB____TRNS          1    /* 透過 */

/**********************************************************************************/
/*       ＢＣＳＥＭＩ定義初期値                                                   */
/**********************************************************************************/

#define            INIT_MJCOD    _EBCDIC    /* 文字ｺｰﾄﾞ */
#define            INIT_TMAK1       3000    /* ﾃｷｽﾄ送信後ACK1までの時間 */
#define            INIT_TMAK0       3000    /* ENQ送信後ACK0までの時間 */
#define            INIT_TMTXT       8000    /* ACK0送信後ﾃｷｽﾄ受信までの時間 */
#define            INIT_TMEOT       4000    /* ACK1送信後EOTまでの時間 */
#define            INIT_TMENQ       1000    /* ACK0待ちでNAK受信時のENQ再送までの時間 */
#define            INIT_NMENQ          3    /* 無応答に対するENQ送信回数 */
#define            INIT_NMTXT          3    /* ﾃｷｽﾄ送信後NAKに対するﾃｷｽﾄ送信回数 */

#define            INIT_TRNSP EB_NOTTRNS    /* （TcpSp 初版）透過／非透過 */

/**********************************************************************************/
/*       ＢＣＳＥＭＩ上下限値チェック                                             */
/**********************************************************************************/

#define            IBCDMN_TIM          1    /* 時間データＭＩＮ値 */
#define            IBCDMX_TIM      99999    /* 時間データＭＡＸ値 */
#define            IBCLMX_TIM          5    /* 時間文字数ＭＡＸ値 */
#define            IBCDMN_NUM          1    /* 回数データＭＩＮ値 */
#define            IBCDMX_NUM         99    /* 回数データＭＡＸ値 */
#define            IBCLMX_NUM          2    /* 回数文字数ＭＡＸ値 */

/**********************************************************************************/
/*       伝送制御文字・文字コード                                                 */
/**********************************************************************************/

#define            ICTL_CHRNM        16     /* 制御文字数     */
#define            ICTL_CHRSZ         2     /* 制御文字サイズ */

                                            /* 制御文字コード（ＥＢＣＤＩＣ）*/
static const char  ENB_CTLEBC[ICTL_CHRNM][ICTL_CHRSZ] = {
                           ( char ) 0x32, ( char ) 0x00,   /* ＳＹＮ   */
                           ( char ) 0x01, ( char ) 0x00,   /* ＳＯＨ   */
                           ( char ) 0x02, ( char ) 0x00,   /* ＳＴＸ   */
                           ( char ) 0x26, ( char ) 0x00,   /* ＥＴＢ   */
                           ( char ) 0x1F, ( char ) 0x00,   /* ＩＴＢ   */
                           ( char ) 0x03, ( char ) 0x00,   /* ＥＴＸ   */
                           ( char ) 0x37, ( char ) 0x00,   /* ＥＯＴ   */
                           ( char ) 0x2D, ( char ) 0x00,   /* ＥＮＱ   */
                           ( char ) 0x10, ( char ) 0x00,   /* ＤＬＥ   */
                           ( char ) 0x10, ( char ) 0x70,   /* ＡＣＫ０ */
                           ( char ) 0x10, ( char ) 0x61,   /* ＡＣＫ１ */
                           ( char ) 0x3D, ( char ) 0x00,   /* ＮＡＫ   */
                           ( char ) 0x10, ( char ) 0x6B,   /* ＷＡＣＫ */
                           ( char ) 0x10, ( char ) 0x7C,   /* ＲＶＩ   */
                           ( char ) 0x02, ( char ) 0x2D,   /* ＴＴＤ   */
                           ( char ) 0x10, ( char ) 0x37    /* ＤＩＳＣ */
};

                                            /* 制御文字コード（ＡＳＣＩＩ） */
static const char  ENB_CTLASC[ICTL_CHRNM][ICTL_CHRSZ] = {
                           ( char ) 0x16, ( char ) 0x00,   /* ＳＹＮ   */
                           ( char ) 0x01, ( char ) 0x00,   /* ＳＯＨ   */
                           ( char ) 0x02, ( char ) 0x00,   /* ＳＴＸ   */
                           ( char ) 0x17, ( char ) 0x00,   /* ＥＴＢ   */
                           ( char ) 0x1F, ( char ) 0x00,   /* ＩＴＢ   */
                           ( char ) 0x03, ( char ) 0x00,   /* ＥＴＸ   */
                           ( char ) 0x04, ( char ) 0x00,   /* ＥＯＴ   */
                           ( char ) 0x05, ( char ) 0x00,   /* ＥＮＱ   */
                           ( char ) 0x10, ( char ) 0x00,   /* ＤＬＥ   */
                           ( char ) 0x10, ( char ) 0x30,   /* ＡＣＫ０ */
                           ( char ) 0x10, ( char ) 0x31,   /* ＡＣＫ１ */
                           ( char ) 0x15, ( char ) 0x00,   /* ＮＡＫ   */
                           ( char ) 0x10, ( char ) 0x2C,   /* ＷＡＣＫ */
                           ( char ) 0x10, ( char ) 0x40,   /* ＲＶＩ   */
                           ( char ) 0x02, ( char ) 0x05,   /* ＴＴＤ   */
                           ( char ) 0x10, ( char ) 0x04    /* ＤＩＳＣ */
};

                                            /* 制御文字サイズ */
static const int   ENB_CTLSIZ[ICTL_CHRNM] = {
                                                      1,   /* ＳＹＮ   */
                                                      1,   /* ＳＯＨ   */
                                                      1,   /* ＳＴＸ   */
                                                      1,   /* ＥＴＢ   */
                                                      1,   /* ＩＴＢ   */
                                                      1,   /* ＥＴＸ   */
                                                      1,   /* ＥＯＴ   */
                                                      1,   /* ＥＮＱ   */
                                                      1,   /* ＤＬＥ   */
                                                      2,   /* ＡＣＫ０ */
                                                      2,   /* ＡＣＫ１ */
                                                      1,   /* ＮＡＫ   */
                                                      2,   /* ＷＡＣＫ */
                                                      2,   /* ＲＶＩ   */
                                                      2,   /* ＴＴＤ   */
                                                      2    /* ＤＩＳＣ */
};

                                            /* 制御文字配列ＩＮＤＥＸ */
#define            ENBCTL_IXSYN        0    /* ＳＹＮ   */
#define            ENBCTL_IXSOH        1    /* ＳＯＨ   */
#define            ENBCTL_IXSTX        2    /* ＳＴＸ   */
#define            ENBCTL_IXETB        3    /* ＥＴＢ   */
#define            ENBCTL_IXITB        4    /* ＩＴＢ   */
#define            ENBCTL_IXETX        5    /* ＥＴＸ   */
#define            ENBCTL_IXEOT        6    /* ＥＯＴ   */
#define            ENBCTL_IXENQ        7    /* ＥＮＱ   */
#define            ENBCTL_IXDLE        8    /* ＤＬＥ   */
#define            ENBCTL_IXAK0        9    /* ＡＣＫ０ */
#define            ENBCTL_IXAK1       10    /* ＡＣＫ１ */
#define            ENBCTL_IXNAK       11    /* ＮＡＫ   */
#define            ENBCTL_IXWCK       12    /* ＷＡＣＫ */
#define            ENBCTL_IXRVI       13    /* ＲＶＩ   */
#define            ENBCTL_IXTTD       14    /* ＴＴＤ   */
#define            ENBCTL_IXDSC       15    /* ＤＩＳＣ */
#define            ENBCTL_IXTXT       -1    /* テキスト（初期値） */

/**********************************************************************************/
/*       タイマー番号                                                             */
/**********************************************************************************/

#define            EB_TMRID_ENQSD     10    /* ＥＮＱ再送タイマー       */
#define            EB_TMRID_AK0RV     11    /* ＡＣＫ０受信待ちタイマー */
#define            EB_TMRID_AK1RV     12    /* ＡＣＫ１受信待ちタイマー */
#define            EB_TMRID_TXTRV     13    /* テキスト受信待ちタイマー */
#define            EB_TMRID_EOTRV     14    /* ＥＯＴ受信待ちタイマー   */

/**********************************************************************************/
/*       プロトタイプ宣言                                                         */
/**********************************************************************************/
void          EnqTimerSet       ( HWND hWnd );
void CALLBACK EnqTimeOut        ( HWND hWnd, UINT uMessage, UINT idEvent, DWORD dwTime );
void          EnqTimerCancel    ( HWND hWnd );
void          Ack0TimerSet      ( HWND hWnd );
void CALLBACK Ack0TimeOut       ( HWND hWnd, UINT uMessage, UINT idEvent, DWORD dwTime );
void          Ack0TimerCancel   ( HWND hWnd );
void          Ack1TimerSet      ( HWND hWnd );
void CALLBACK Ack1TimeOut       ( HWND hWnd, UINT uMessage, UINT idEvent, DWORD dwTime );
void          Ack1TimerCancel   ( HWND hWnd );
void          TextTimerSet      ( HWND hWnd );
void CALLBACK TextTimeOut       ( HWND hWnd, UINT uMessage, UINT idEvent, DWORD dwTime );
void          TextTimerCancel   ( HWND hWnd );
void          EotTimerSet       ( HWND hWnd );
void CALLBACK EotTimeOut        ( HWND hWnd, UINT uMessage, UINT idEvent, DWORD dwTime );
void          EotTimerCancel    ( HWND hWnd );
void          Ebsdbuf_oldrecdel ( HWND hWnd );
int           Ebsdbuf_recset    ( HWND hWnd, int itxsiz, char* ctxstr );
int           Ebchk_ctrlstr     ( HWND hWnd, char* cdbstr );
void          EnetB_1stSnd      ( HWND hWnd, int send_port, int itxsiz, char* ctxstr );
void          EnetB_DBend       ( HWND hWnd );
void          EnetB_Gbl_init    ( HWND hWnd );

void          SetSndMsg         ( HWND hWnd, int idtsiz, char* cdtstr );

void          EnetBX_1stSnd     ( HWND hWnd, int send_port, int itxsiz, char* ctxstr );
void          EnetBX_Dle_ins    ( HWND hWnd, int* itxsiz, char* ctxstr );
void          EnetBX_Dle_del    ( HWND hWnd, int* itxsiz, char* ctxstr );

#endif
