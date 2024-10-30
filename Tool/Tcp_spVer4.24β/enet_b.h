/***********************************************************************************
*
*  ‚d‚‚…‚”|‚aƒwƒbƒ_[
*
*  AUTHOR          : ‰Á‰ê’J ‘å•ã
*  DATE            : 2006.09.28(‘Ò‡‚¹ƒc[ƒ‹)
*                    2010.07.14(TcpSp ‰”Å)
*
*  DEVISION 001    : ____.__.__ ( QQQFQQQQQQQQQQQQQQQQQQQQ
*  DEVISION 002    : ____.__.__ ( QQQFQQQQQQQQQQQQQQQQQQQQ
*  DEVISION 003    : ____.__.__ ( QQQFQQQQQQQQQQQQQQQQQQQQ
*
***********************************************************************************/
#ifndef _ENET_B_H
#define _ENET_B_H


/**********************************************************************************/
/*       ƒCƒ“ƒNƒ‹[ƒhéŒ¾                                                         */
/**********************************************************************************/

#include      "dump.h"
#include      "TcpCtrl.h"

/**********************************************************************************/
/*       ‘—M’†ƒeƒLƒXƒgŠi”[ƒoƒbƒtƒ@                                               */
/**********************************************************************************/

#define            ENETB_MAXSIZE    1400    /* “`•¶Å‘åƒTƒCƒY */
#define            ENETB_MXPOINT      20    /* ƒoƒbƒtƒ@ƒŒƒR[ƒh” */

typedef struct {
    int     in_point;                                 /* ‚h‚mƒ|ƒCƒ“ƒ^ */
    int     out_point;                                /* ‚n‚t‚sƒ|ƒCƒ“ƒ^ */
    int     snd_size[ENETB_MXPOINT];                  /* ‘—M“`•¶ƒTƒCƒY */
    char    snd_buff[ENETB_MXPOINT][ENETB_MAXSIZE];   /* ‘—M’†ƒoƒbƒtƒ@ */
} EnetbSndBuf;

EnetbSndBuf        eb_sndbuf;               /* ƒoƒbƒtƒ@ƒOƒ[ƒoƒ‹”z—ñ */

/**********************************************************************************/
/*       óM’†ƒeƒLƒXƒgŠi”[ƒoƒbƒtƒ@                                               */
/**********************************************************************************/

char               g_eb_rcvbuf1[ENETB_MAXSIZE];       /* ƒ|[ƒg‚PóM’†ƒoƒbƒtƒ@ */
char               g_eb_rcvkey1[FNAME_MAX];           /* ƒ|[ƒg‚PóM’†ƒL[ƒR[ƒh */
int                g_eb_rcvsiz1;                      /* ƒ|[ƒg‚PóM’†ƒTƒCƒY */
char               g_eb_rcvbuf3[ENETB_MAXSIZE];       /* ƒ|[ƒg‚RóM’†ƒoƒbƒtƒ@ */
char               g_eb_rcvkey3[FNAME_MAX];           /* ƒ|[ƒg‚RóM’†ƒL[ƒR[ƒh */
int                g_eb_rcvsiz3;                      /* ƒ|[ƒg‚RóM’†ƒTƒCƒY */

/**********************************************************************************/
/*       ‘—M’†ó‘Ô                                                               */
/**********************************************************************************/

int                g_eb_sndst;              /* ‘—M’†ƒOƒ[ƒoƒ‹•Ï” */

#define            EB_NOTSND           0    /* ‘—M’†‚Å‚È‚¢ */
#define            EB_ENQSND           1    /* ‚d‚m‚p‘—M’† */
#define            EB_TXTSND           2    /* ƒeƒLƒXƒg‘—M’† */
#define            EB_AK0SND           3    /* ‚`‚b‚j‚O‘—M’† */
#define            EB_AK1SND           4    /* ‚`‚b‚j‚P‘—M’† */

/**********************************************************************************/
/*       ƒŠƒgƒ‰ƒCƒJƒEƒ“ƒ^                                                         */
/**********************************************************************************/

int                g_eb_cntenq;             /* ‚d‚m‚p‘—MƒŠƒgƒ‰ƒCƒJƒEƒ“ƒ^ */
int                g_eb_cnttxt;             /* ƒeƒLƒXƒg‘—MƒŠƒgƒ‰ƒCƒJƒEƒ“ƒ^ */

/**********************************************************************************/
/*       “§‰ß^”ñ“§‰ß’è”iTcpSp ‰”Åj                                           */
/**********************************************************************************/

#define            EB_NOTTRNS          0    /* ”ñ“§‰ß */
#define            EB____TRNS          1    /* “§‰ß */

/**********************************************************************************/
/*       ‚a‚b‚r‚d‚l‚h’è‹`‰Šú’l                                                   */
/**********************************************************************************/

#define            INIT_MJCOD    _EBCDIC    /* •¶šº°ÄŞ */
#define            INIT_TMAK1       3000    /* Ã·½Ä‘—MŒãACK1‚Ü‚Å‚ÌŠÔ */
#define            INIT_TMAK0       3000    /* ENQ‘—MŒãACK0‚Ü‚Å‚ÌŠÔ */
#define            INIT_TMTXT       8000    /* ACK0‘—MŒãÃ·½ÄóM‚Ü‚Å‚ÌŠÔ */
#define            INIT_TMEOT       4000    /* ACK1‘—MŒãEOT‚Ü‚Å‚ÌŠÔ */
#define            INIT_TMENQ       1000    /* ACK0‘Ò‚¿‚ÅNAKóM‚ÌENQÄ‘—‚Ü‚Å‚ÌŠÔ */
#define            INIT_NMENQ          3    /* –³‰“š‚É‘Î‚·‚éENQ‘—M‰ñ” */
#define            INIT_NMTXT          3    /* Ã·½Ä‘—MŒãNAK‚É‘Î‚·‚éÃ·½Ä‘—M‰ñ” */

#define            INIT_TRNSP EB_NOTTRNS    /* iTcpSp ‰”Åj“§‰ß^”ñ“§‰ß */

/**********************************************************************************/
/*       ‚a‚b‚r‚d‚l‚hã‰ºŒÀ’lƒ`ƒFƒbƒN                                             */
/**********************************************************************************/

#define            IBCDMN_TIM          1    /* ŠÔƒf[ƒ^‚l‚h‚m’l */
#define            IBCDMX_TIM      99999    /* ŠÔƒf[ƒ^‚l‚`‚w’l */
#define            IBCLMX_TIM          5    /* ŠÔ•¶š”‚l‚`‚w’l */
#define            IBCDMN_NUM          1    /* ‰ñ”ƒf[ƒ^‚l‚h‚m’l */
#define            IBCDMX_NUM         99    /* ‰ñ”ƒf[ƒ^‚l‚`‚w’l */
#define            IBCLMX_NUM          2    /* ‰ñ”•¶š”‚l‚`‚w’l */

/**********************************************************************************/
/*       “`‘—§Œä•¶šE•¶šƒR[ƒh                                                 */
/**********************************************************************************/

#define            ICTL_CHRNM        16     /* §Œä•¶š”     */
#define            ICTL_CHRSZ         2     /* §Œä•¶šƒTƒCƒY */

                                            /* §Œä•¶šƒR[ƒhi‚d‚a‚b‚c‚h‚bj*/
static const char  ENB_CTLEBC[ICTL_CHRNM][ICTL_CHRSZ] = {
                           ( char ) 0x32, ( char ) 0x00,   /* ‚r‚x‚m   */
                           ( char ) 0x01, ( char ) 0x00,   /* ‚r‚n‚g   */
                           ( char ) 0x02, ( char ) 0x00,   /* ‚r‚s‚w   */
                           ( char ) 0x26, ( char ) 0x00,   /* ‚d‚s‚a   */
                           ( char ) 0x1F, ( char ) 0x00,   /* ‚h‚s‚a   */
                           ( char ) 0x03, ( char ) 0x00,   /* ‚d‚s‚w   */
                           ( char ) 0x37, ( char ) 0x00,   /* ‚d‚n‚s   */
                           ( char ) 0x2D, ( char ) 0x00,   /* ‚d‚m‚p   */
                           ( char ) 0x10, ( char ) 0x00,   /* ‚c‚k‚d   */
                           ( char ) 0x10, ( char ) 0x70,   /* ‚`‚b‚j‚O */
                           ( char ) 0x10, ( char ) 0x61,   /* ‚`‚b‚j‚P */
                           ( char ) 0x3D, ( char ) 0x00,   /* ‚m‚`‚j   */
                           ( char ) 0x10, ( char ) 0x6B,   /* ‚v‚`‚b‚j */
                           ( char ) 0x10, ( char ) 0x7C,   /* ‚q‚u‚h   */
                           ( char ) 0x02, ( char ) 0x2D,   /* ‚s‚s‚c   */
                           ( char ) 0x10, ( char ) 0x37    /* ‚c‚h‚r‚b */
};

                                            /* §Œä•¶šƒR[ƒhi‚`‚r‚b‚h‚hj */
static const char  ENB_CTLASC[ICTL_CHRNM][ICTL_CHRSZ] = {
                           ( char ) 0x16, ( char ) 0x00,   /* ‚r‚x‚m   */
                           ( char ) 0x01, ( char ) 0x00,   /* ‚r‚n‚g   */
                           ( char ) 0x02, ( char ) 0x00,   /* ‚r‚s‚w   */
                           ( char ) 0x17, ( char ) 0x00,   /* ‚d‚s‚a   */
                           ( char ) 0x1F, ( char ) 0x00,   /* ‚h‚s‚a   */
                           ( char ) 0x03, ( char ) 0x00,   /* ‚d‚s‚w   */
                           ( char ) 0x04, ( char ) 0x00,   /* ‚d‚n‚s   */
                           ( char ) 0x05, ( char ) 0x00,   /* ‚d‚m‚p   */
                           ( char ) 0x10, ( char ) 0x00,   /* ‚c‚k‚d   */
                           ( char ) 0x10, ( char ) 0x30,   /* ‚`‚b‚j‚O */
                           ( char ) 0x10, ( char ) 0x31,   /* ‚`‚b‚j‚P */
                           ( char ) 0x15, ( char ) 0x00,   /* ‚m‚`‚j   */
                           ( char ) 0x10, ( char ) 0x2C,   /* ‚v‚`‚b‚j */
                           ( char ) 0x10, ( char ) 0x40,   /* ‚q‚u‚h   */
                           ( char ) 0x02, ( char ) 0x05,   /* ‚s‚s‚c   */
                           ( char ) 0x10, ( char ) 0x04    /* ‚c‚h‚r‚b */
};

                                            /* §Œä•¶šƒTƒCƒY */
static const int   ENB_CTLSIZ[ICTL_CHRNM] = {
                                                      1,   /* ‚r‚x‚m   */
                                                      1,   /* ‚r‚n‚g   */
                                                      1,   /* ‚r‚s‚w   */
                                                      1,   /* ‚d‚s‚a   */
                                                      1,   /* ‚h‚s‚a   */
                                                      1,   /* ‚d‚s‚w   */
                                                      1,   /* ‚d‚n‚s   */
                                                      1,   /* ‚d‚m‚p   */
                                                      1,   /* ‚c‚k‚d   */
                                                      2,   /* ‚`‚b‚j‚O */
                                                      2,   /* ‚`‚b‚j‚P */
                                                      1,   /* ‚m‚`‚j   */
                                                      2,   /* ‚v‚`‚b‚j */
                                                      2,   /* ‚q‚u‚h   */
                                                      2,   /* ‚s‚s‚c   */
                                                      2    /* ‚c‚h‚r‚b */
};

                                            /* §Œä•¶š”z—ñ‚h‚m‚c‚d‚w */
#define            ENBCTL_IXSYN        0    /* ‚r‚x‚m   */
#define            ENBCTL_IXSOH        1    /* ‚r‚n‚g   */
#define            ENBCTL_IXSTX        2    /* ‚r‚s‚w   */
#define            ENBCTL_IXETB        3    /* ‚d‚s‚a   */
#define            ENBCTL_IXITB        4    /* ‚h‚s‚a   */
#define            ENBCTL_IXETX        5    /* ‚d‚s‚w   */
#define            ENBCTL_IXEOT        6    /* ‚d‚n‚s   */
#define            ENBCTL_IXENQ        7    /* ‚d‚m‚p   */
#define            ENBCTL_IXDLE        8    /* ‚c‚k‚d   */
#define            ENBCTL_IXAK0        9    /* ‚`‚b‚j‚O */
#define            ENBCTL_IXAK1       10    /* ‚`‚b‚j‚P */
#define            ENBCTL_IXNAK       11    /* ‚m‚`‚j   */
#define            ENBCTL_IXWCK       12    /* ‚v‚`‚b‚j */
#define            ENBCTL_IXRVI       13    /* ‚q‚u‚h   */
#define            ENBCTL_IXTTD       14    /* ‚s‚s‚c   */
#define            ENBCTL_IXDSC       15    /* ‚c‚h‚r‚b */
#define            ENBCTL_IXTXT       -1    /* ƒeƒLƒXƒgi‰Šú’lj */

/**********************************************************************************/
/*       ƒ^ƒCƒ}[”Ô†                                                             */
/**********************************************************************************/

#define            EB_TMRID_ENQSD     10    /* ‚d‚m‚pÄ‘—ƒ^ƒCƒ}[       */
#define            EB_TMRID_AK0RV     11    /* ‚`‚b‚j‚OóM‘Ò‚¿ƒ^ƒCƒ}[ */
#define            EB_TMRID_AK1RV     12    /* ‚`‚b‚j‚PóM‘Ò‚¿ƒ^ƒCƒ}[ */
#define            EB_TMRID_TXTRV     13    /* ƒeƒLƒXƒgóM‘Ò‚¿ƒ^ƒCƒ}[ */
#define            EB_TMRID_EOTRV     14    /* ‚d‚n‚sóM‘Ò‚¿ƒ^ƒCƒ}[   */

/**********************************************************************************/
/*       ƒvƒƒgƒ^ƒCƒvéŒ¾                                                         */
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
