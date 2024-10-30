/***********************************************************************************
*
*  ‚d‚‚…‚”|‚aŠÖ”ƒOƒ[ƒoƒ‹—Ìˆæ
*
*  AUTHOR          : ‰Á‰ê’J ‘å•ã
*  DATE            : 2006.09.27
*
*  DEVISION 001    : 2013.08.31 (  “c’† Fƒx[ƒVƒbƒNè‡“`‘—ƒZƒ~‚Ì“§‰ßƒ‚[ƒh‚É‘Î‰B
*                                         Šùİˆ—‚Ö‚Ì‰e‹¿‚ğl‚¦A]—ˆŠÖ”‚ÉŠ±Â
*                                         ‚µ‚È‚¢‚æ‚¤‚ÉŠJ”­B
*  DEVISION 002    : ____.__.__ ( QQQFQQQQQQQQQQQQQQQQQQQQ
*  DEVISION 003    : ____.__.__ ( QQQFQQQQQQQQQQQQQQQQQQQQ
*
***********************************************************************************/

                             /* ‚h‚m‚b‚k‚t‚c‚déŒ¾ */
#include    <stdio.h>
#include    <windows.h>

#include    "enet_b.h"
#include    "TcpCtrl.h"
#include    "Pcterm.h"

                             /* ƒOƒ[ƒoƒ‹•Ï” */
UINT    EnqSnd_timer_id = 0;
UINT    Ak0Rcv_timer_id = 0;
UINT    Ak1Rcv_timer_id = 0;
UINT    TxtRcv_timer_id = 0;
UINT    EotRcv_timer_id = 0;

/***********************************************************************************
*
*  ‚d‚‚…‚”|‚aƒOƒ[ƒoƒ‹•Ï”‰Šú‰»ˆ—
*
*  AUTHOR          : ‰Á‰ê’J ‘å•ã
*  DATE            : 2006.10.03
*
*  DEVISION 001    : ____.__.__ ( QQQFQQQQQQQQQQQQQQQQQQQQ
*  DEVISION 002    : ____.__.__ ( QQQFQQQQQQQQQQQQQQQQQQQQ
*  DEVISION 003    : ____.__.__ ( QQQFQQQQQQQQQQQQQQQQQQQQ
*
*  PARAMETER
*        HWND        hWnd               F ƒEƒCƒ“ƒhƒEƒnƒ“ƒhƒ‹
*
***********************************************************************************/
void EnetB_Gbl_init ( HWND hWnd ) {

                             /* ‘—Mƒoƒbƒtƒ@ƒOƒ[ƒoƒ‹ */
    eb_sndbuf.in_point = 0;
    eb_sndbuf.out_point = 0;
    memset ( eb_sndbuf.snd_size, 0x00, sizeof ( eb_sndbuf.snd_size ) );
    memset ( eb_sndbuf.snd_buff, 0x00, sizeof ( eb_sndbuf.snd_buff ) );
                             /* ‘—M’†ó‘ÔƒOƒ[ƒoƒ‹ */
    g_eb_sndst = EB_NOTSND;
                             /* ƒŠƒgƒ‰ƒCƒJƒEƒ“ƒ^ƒOƒ[ƒoƒ‹ */
    g_eb_cntenq = 0;
    g_eb_cnttxt = 0;
                             /* óMƒoƒbƒtƒ@ƒOƒ[ƒoƒ‹ */
    memset ( g_eb_rcvbuf1, 0x00, sizeof ( g_eb_rcvbuf1 ) );
    memset ( g_eb_rcvkey1, 0x00, sizeof ( g_eb_rcvkey1 ) );
    g_eb_rcvsiz1 = 0;
    memset ( g_eb_rcvbuf3, 0x00, sizeof ( g_eb_rcvbuf3 ) );
    memset ( g_eb_rcvkey3, 0x00, sizeof ( g_eb_rcvkey3 ) );
    g_eb_rcvsiz3 = 0;

    return;
}

/***********************************************************************************
*
*  ‚d‚m‚pÄ‘—‹N“®ƒ^ƒCƒ}[ƒZƒbƒg
*
*  AUTHOR          : ‰Á‰ê’J ‘å•ã
*  DATE            : 2006.09.27
*
*  DEVISION 001    : ____.__.__ ( QQQFQQQQQQQQQQQQQQQQQQQQ
*  DEVISION 002    : ____.__.__ ( QQQFQQQQQQQQQQQQQQQQQQQQ
*  DEVISION 003    : ____.__.__ ( QQQFQQQQQQQQQQQQQQQQQQQQ
*
*  PARAMETER
*        HWND        hWnd               F ƒEƒCƒ“ƒhƒEƒnƒ“ƒhƒ‹
*
***********************************************************************************/
void EnqTimerSet ( HWND hWnd ) {

    if ( 0 == EnqSnd_timer_id ) {
                             /* ƒ^ƒCƒ}[ƒZƒbƒg */
        EnqSnd_timer_id = SetTimer ( hWnd, EB_TMRID_ENQSD, tcpCtrl.tm_enq_anak, (TIMERPROC) EnqTimeOut );
        if ( 0 == EnqSnd_timer_id ) {
            MyMessage ( hWnd, "‚d‚m‚pÄ‘—ƒ^ƒCƒ}[‹N“®¸”s" );
        } else {
            MyMessage ( hWnd, "‚d‚m‚pÄ‘—ƒ^ƒCƒ}[‹N“®¬Œ÷ LINE=%d", __LINE__ );
        }
    }

    return;
}

/***********************************************************************************
*
*  ‚d‚m‚pÄ‘—ƒ^ƒCƒ}[‹N“®
*
*  AUTHOR          : ‰Á‰ê’J ‘å•ã
*  DATE            : 2006.09.27
*
*  DEVISION 001    : ____.__.__ ( QQQFQQQQQQQQQQQQQQQQQQQQ
*  DEVISION 002    : ____.__.__ ( QQQFQQQQQQQQQQQQQQQQQQQQ
*  DEVISION 003    : ____.__.__ ( QQQFQQQQQQQQQQQQQQQQQQQQ
*
*  PARAMETER
*        HWND        hWnd                : ƒEƒCƒ“ƒhƒEƒnƒ“ƒhƒ‹
*        UINT        uMessage            : ƒƒbƒZ[ƒW
*        UINT        idEvent             : ƒ^ƒCƒ}[ID
*        DWORD       dwTime              : ƒ^ƒCƒ}[Œo‰ßŠÔ
*
***********************************************************************************/
void CALLBACK EnqTimeOut ( HWND hWnd, UINT uMessage, UINT idEvent, DWORD dwTime ) {

    char      c_enqstr[1];             /* ‚d‚m‚p•¶š—ñ */

    MyMessage ( hWnd, "‚d‚m‚pÄ‘—ƒ^ƒCƒ€ƒAƒEƒgI LINE=%d ", __LINE__ );
                             /* ƒ^ƒCƒ}[ƒLƒƒƒ“ƒZƒ‹ */
    EnqTimerCancel ( hWnd );
                             /* ‘—M’†î•ñXV */
    g_eb_sndst = EB_NOTSND;
                             /* ƒŠƒgƒ‰ƒC‘—MH */
    if ( g_eb_cntenq < tcpCtrl.nm_enq_nans ) {
        MyMessage ( hWnd, "Ä‘—ŠJnI LINE=%d ", __LINE__ );
                             /* ‚d‚m‚pÄ‘— */
        if ( _ASCII == tcpCtrl.eb_mojicode ) {
            memcpy ( c_enqstr, ENB_CTLASC[ENBCTL_IXENQ], sizeof ( c_enqstr ) );
        } else {
            memcpy ( c_enqstr, ENB_CTLEBC[ENBCTL_IXENQ], sizeof ( c_enqstr ) );
        }
        SetSndMsg ( hWnd, sizeof ( c_enqstr ), c_enqstr );

        g_eb_cntenq = g_eb_cntenq + 1;

    } else {
        MyMessage ( hWnd, "‚d‚m‚pƒŠƒgƒ‰ƒC‰ñ”ƒI[ƒo[I LINE=%d ", __LINE__ );
                             /* ‘—MƒeƒLƒXƒgƒoƒbƒtƒ@ÅŒÃƒŒƒR[ƒhíœ */
        Ebsdbuf_oldrecdel ( hWnd );
                             /* ƒŠƒgƒ‰ƒCƒJƒEƒ“ƒ^‰Šú‰» */
        g_eb_cntenq = 0;
                             /* “`‘—I—¹ˆ— */
        EnetB_DBend ( hWnd );
    }

    return;
}

/***********************************************************************************
*
*  ‚d‚m‚pÄ‘—ƒ^ƒCƒ}[ƒLƒƒƒ“ƒZƒ‹
*
*  AUTHOR          : ‰Á‰ê’J ‘å•ã
*  DATE            : 2006.09.28
*
*  DEVISION 001    : ____.__.__ ( QQQFQQQQQQQQQQQQQQQQQQQQ
*  DEVISION 002    : ____.__.__ ( QQQFQQQQQQQQQQQQQQQQQQQQ
*  DEVISION 003    : ____.__.__ ( QQQFQQQQQQQQQQQQQQQQQQQQ
*
*  PARAMETER
*        HWND        hWnd                : ƒEƒCƒ“ƒhƒEƒnƒ“ƒhƒ‹
*
***********************************************************************************/
void EnqTimerCancel ( HWND hWnd ) {

    if ( 0 != EnqSnd_timer_id ) {
        KillTimer ( hWnd, EnqSnd_timer_id );
        EnqSnd_timer_id = 0;
    }

    return;
}

/***********************************************************************************
*
*  ‚`‚b‚j‚OóM‘Ò‚¿ƒ^ƒCƒ}[ƒZƒbƒg
*
*  AUTHOR          : ‰Á‰ê’J ‘å•ã
*  DATE            : 2006.09.28
*
*  DEVISION 001    : ____.__.__ ( QQQFQQQQQQQQQQQQQQQQQQQQ
*  DEVISION 002    : ____.__.__ ( QQQFQQQQQQQQQQQQQQQQQQQQ
*  DEVISION 003    : ____.__.__ ( QQQFQQQQQQQQQQQQQQQQQQQQ
*
*  PARAMETER
*        HWND        hWnd               F ƒEƒCƒ“ƒhƒEƒnƒ“ƒhƒ‹
*
***********************************************************************************/
void Ack0TimerSet ( HWND hWnd ) {

    if ( 0 == Ak0Rcv_timer_id ) {
                             /* ƒ^ƒCƒ}[ƒZƒbƒg */
        Ak0Rcv_timer_id = SetTimer ( hWnd, EB_TMRID_AK0RV, tcpCtrl.tm_ak0_aenq, (TIMERPROC) Ack0TimeOut );
        if ( 0 == Ak0Rcv_timer_id ) {
            MyMessage ( hWnd, "‚`‚b‚j‚O‘Ò‚¿ƒ^ƒCƒ}[¸”s" );
        } else {
            MyMessage ( hWnd, "‚`‚b‚j‚O‘Ò‚¿ƒ^ƒCƒ}[¬Œ÷ LINE=%d", __LINE__ );
        }
    }

    return;
}

/***********************************************************************************
*
*  ‚`‚b‚j‚OóM‘Ò‚¿ƒ^ƒCƒ€ƒAƒEƒg
*
*  AUTHOR          : ‰Á‰ê’J ‘å•ã
*  DATE            : 2006.09.28
*
*  DEVISION 001    : ____.__.__ ( QQQFQQQQQQQQQQQQQQQQQQQQ
*  DEVISION 002    : ____.__.__ ( QQQFQQQQQQQQQQQQQQQQQQQQ
*  DEVISION 003    : ____.__.__ ( QQQFQQQQQQQQQQQQQQQQQQQQ
*
*  PARAMETER
*        HWND        hWnd                : ƒEƒCƒ“ƒhƒEƒnƒ“ƒhƒ‹
*        UINT        uMessage            : ƒƒbƒZ[ƒW
*        UINT        idEvent             : ƒ^ƒCƒ}[ID
*        DWORD       dwTime              : ƒ^ƒCƒ}[Œo‰ßŠÔ
*
***********************************************************************************/
void CALLBACK Ack0TimeOut ( HWND hWnd, UINT uMessage, UINT idEvent, DWORD dwTime ) {

    char      c_enqstr[1];             /* ‚d‚m‚p•¶š—ñ */

    MyMessage ( hWnd, "‚`‚b‚j‚OóM‘Ò‚¿ƒ^ƒCƒ€ƒAƒEƒgI LINE=%d ", __LINE__ );
                             /* ƒ^ƒCƒ}[ƒLƒƒƒ“ƒZƒ‹ */
    Ack0TimerCancel ( hWnd );
                             /* ‘—M’†î•ñ‰Šú‰» */
    g_eb_sndst = EB_NOTSND;
                             /* ƒŠƒgƒ‰ƒC‘—MH */
    if ( g_eb_cntenq < tcpCtrl.nm_enq_nans ) {
                             /* ‚d‚m‚pÄ‘— */
        if ( _ASCII == tcpCtrl.eb_mojicode ) {
            memcpy ( c_enqstr, ENB_CTLASC[ENBCTL_IXENQ], sizeof ( c_enqstr ) );
        } else {
            memcpy ( c_enqstr, ENB_CTLEBC[ENBCTL_IXENQ], sizeof ( c_enqstr ) );
        }
        SetSndMsg ( hWnd, sizeof ( c_enqstr ), c_enqstr );
                             /* Ä‘—‰ñ”XV */
        g_eb_cntenq = g_eb_cntenq + 1;
    } else {
        MyMessage ( hWnd, "‚d‚m‚pƒŠƒgƒ‰ƒC‰ñ”ƒI[ƒo[I LINE=%d ", __LINE__ );
                             /* ‘—MƒeƒLƒXƒgƒoƒbƒtƒ@ÅŒÃƒŒƒR[ƒhíœ */
        Ebsdbuf_oldrecdel ( hWnd );
                             /* ƒŠƒgƒ‰ƒCƒJƒEƒ“ƒ^‰Šú‰» */
        g_eb_cntenq = 0;
                             /* “`‘—I—¹ˆ— */
        EnetB_DBend ( hWnd );
    }

    return;
}

/***********************************************************************************
*
*  ‚`‚b‚j‚OóM‘Ò‚¿ƒ^ƒCƒ}[ƒLƒƒƒ“ƒZƒ‹
*
*  AUTHOR          : ‰Á‰ê’J ‘å•ã
*  DATE            : 2006.09.28
*
*  DEVISION 001    : ____.__.__ ( QQQFQQQQQQQQQQQQQQQQQQQQ
*  DEVISION 002    : ____.__.__ ( QQQFQQQQQQQQQQQQQQQQQQQQ
*  DEVISION 003    : ____.__.__ ( QQQFQQQQQQQQQQQQQQQQQQQQ
*
*  PARAMETER
*        HWND        hWnd                : ƒEƒCƒ“ƒhƒEƒnƒ“ƒhƒ‹
*
***********************************************************************************/
void Ack0TimerCancel ( HWND hWnd ) {

    if ( 0 != Ak0Rcv_timer_id ) {
        KillTimer ( hWnd, Ak0Rcv_timer_id );
        Ak0Rcv_timer_id = 0;
    }

    return;
}

/***********************************************************************************
*
*  ‚`‚b‚j‚PóM‘Ò‚¿ƒ^ƒCƒ}[ƒZƒbƒg
*
*  AUTHOR          : ‰Á‰ê’J ‘å•ã
*  DATE            : 2006.09.28
*
*  DEVISION 001    : ____.__.__ ( QQQFQQQQQQQQQQQQQQQQQQQQ
*  DEVISION 002    : ____.__.__ ( QQQFQQQQQQQQQQQQQQQQQQQQ
*  DEVISION 003    : ____.__.__ ( QQQFQQQQQQQQQQQQQQQQQQQQ
*
*  PARAMETER
*        HWND        hWnd               F ƒEƒCƒ“ƒhƒEƒnƒ“ƒhƒ‹
*
***********************************************************************************/
void Ack1TimerSet ( HWND hWnd ) {

    if ( 0 == Ak1Rcv_timer_id ) {
                             /* ƒ^ƒCƒ}[ƒZƒbƒg */
        Ak1Rcv_timer_id = SetTimer ( hWnd, EB_TMRID_AK1RV, tcpCtrl.tm_ak1_atxt, (TIMERPROC) Ack1TimeOut );
        if ( 0 == Ak1Rcv_timer_id ) {
            MyMessage ( hWnd, "‚`‚b‚j‚P‘Ò‚¿ƒ^ƒCƒ}[¸”s" );
        } else {
            MyMessage ( hWnd, "‚`‚b‚j‚P‘Ò‚¿ƒ^ƒCƒ}[¬Œ÷ LINE=%d", __LINE__ );
        }
    }

    return;
}

/***********************************************************************************
*
*  ‚`‚b‚j‚PóM‘Ò‚¿ƒ^ƒCƒ€ƒAƒEƒg
*
*  AUTHOR          : ‰Á‰ê’J ‘å•ã
*  DATE            : 2006.09.28
*
*  DEVISION 001    : ____.__.__ ( QQQFQQQQQQQQQQQQQQQQQQQQ
*  DEVISION 002    : ____.__.__ ( QQQFQQQQQQQQQQQQQQQQQQQQ
*  DEVISION 003    : ____.__.__ ( QQQFQQQQQQQQQQQQQQQQQQQQ
*
*  PARAMETER
*        HWND        hWnd                : ƒEƒCƒ“ƒhƒEƒnƒ“ƒhƒ‹
*        UINT        uMessage            : ƒƒbƒZ[ƒW
*        UINT        idEvent             : ƒ^ƒCƒ}[ID
*        DWORD       dwTime              : ƒ^ƒCƒ}[Œo‰ßŠÔ
*
***********************************************************************************/
void CALLBACK Ack1TimeOut ( HWND hWnd, UINT uMessage, UINT idEvent, DWORD dwTime ) {

    MyMessage ( hWnd, "‚`‚b‚j‚PóM‘Ò‚¿ƒ^ƒCƒ€ƒAƒEƒgI LINE=%d ", __LINE__ );
                             /* ƒ^ƒCƒ}[ƒLƒƒƒ“ƒZƒ‹ */
    Ack1TimerCancel ( hWnd );
                             /* ‘—MƒeƒLƒXƒgƒoƒbƒtƒ@ÅŒÃƒŒƒR[ƒhíœ */
    Ebsdbuf_oldrecdel ( hWnd );
                             /* ƒŠƒgƒ‰ƒCƒJƒEƒ“ƒ^‰Šú‰» */
    g_eb_cnttxt = 0;
                             /* “`‘—I—¹ˆ— */
    EnetB_DBend ( hWnd );

    return;
}

/***********************************************************************************
*
*  ‚`‚b‚j‚PóM‘Ò‚¿ƒ^ƒCƒ}[ƒLƒƒƒ“ƒZƒ‹
*
*  AUTHOR          : ‰Á‰ê’J ‘å•ã
*  DATE            : 2006.09.28
*
*  DEVISION 001    : ____.__.__ ( QQQFQQQQQQQQQQQQQQQQQQQQ
*  DEVISION 002    : ____.__.__ ( QQQFQQQQQQQQQQQQQQQQQQQQ
*  DEVISION 003    : ____.__.__ ( QQQFQQQQQQQQQQQQQQQQQQQQ
*
*  PARAMETER
*        HWND        hWnd                : ƒEƒCƒ“ƒhƒEƒnƒ“ƒhƒ‹
*
***********************************************************************************/
void Ack1TimerCancel ( HWND hWnd ) {

    if ( 0 != Ak1Rcv_timer_id ) {
        KillTimer ( hWnd, Ak1Rcv_timer_id );
        Ak1Rcv_timer_id = 0;
    }

    return;
}

/***********************************************************************************
*
*  ƒeƒLƒXƒgóM‘Ò‚¿ƒ^ƒCƒ}[ƒZƒbƒg
*
*  AUTHOR          : ‰Á‰ê’J ‘å•ã
*  DATE            : 2006.09.28
*
*  DEVISION 001    : ____.__.__ ( QQQFQQQQQQQQQQQQQQQQQQQQ
*  DEVISION 002    : ____.__.__ ( QQQFQQQQQQQQQQQQQQQQQQQQ
*  DEVISION 003    : ____.__.__ ( QQQFQQQQQQQQQQQQQQQQQQQQ
*
*  PARAMETER
*        HWND        hWnd               F ƒEƒCƒ“ƒhƒEƒnƒ“ƒhƒ‹
*
***********************************************************************************/
void TextTimerSet ( HWND hWnd ) {

    if ( 0 == TxtRcv_timer_id ) {
                             /* ƒ^ƒCƒ}[ƒZƒbƒg */
        TxtRcv_timer_id = SetTimer ( hWnd, EB_TMRID_TXTRV, tcpCtrl.tm_txt_aak0, (TIMERPROC) TextTimeOut );
        if ( 0 == TxtRcv_timer_id ) {
            MyMessage ( hWnd, "ƒeƒLƒXƒg‘Ò‚¿ƒ^ƒCƒ}[¸”s" );
        } else {
            MyMessage ( hWnd, "ƒeƒLƒXƒg‘Ò‚¿ƒ^ƒCƒ}[¬Œ÷ LINE=%d", __LINE__ );
        }
    }

    return;
}

/***********************************************************************************
*
*  ƒeƒLƒXƒgóM‘Ò‚¿ƒ^ƒCƒ€ƒAƒEƒg
*
*  AUTHOR          : ‰Á‰ê’J ‘å•ã
*  DATE            : 2006.09.28
*
*  DEVISION 001    : ____.__.__ ( QQQFQQQQQQQQQQQQQQQQQQQQ
*  DEVISION 002    : ____.__.__ ( QQQFQQQQQQQQQQQQQQQQQQQQ
*  DEVISION 003    : ____.__.__ ( QQQFQQQQQQQQQQQQQQQQQQQQ
*
*  PARAMETER
*        HWND        hWnd                : ƒEƒCƒ“ƒhƒEƒnƒ“ƒhƒ‹
*        UINT        uMessage            : ƒƒbƒZ[ƒW
*        UINT        idEvent             : ƒ^ƒCƒ}[ID
*        DWORD       dwTime              : ƒ^ƒCƒ}[Œo‰ßŠÔ
*
***********************************************************************************/
void CALLBACK TextTimeOut ( HWND hWnd, UINT uMessage, UINT idEvent, DWORD dwTime ) {

    MyMessage ( hWnd, "ƒeƒLƒXƒgóM‘Ò‚¿ƒ^ƒCƒ€ƒAƒEƒgI LINE=%d ", __LINE__ );
                             /* ƒ^ƒCƒ}[ƒLƒƒƒ“ƒZƒ‹ */
    TextTimerCancel ( hWnd );
                             /* “`‘—I—¹ˆ— */
    EnetB_DBend ( hWnd );

    return;
}

/***********************************************************************************
*
*  ƒeƒLƒXƒgóM‘Ò‚¿ƒ^ƒCƒ}[ƒLƒƒƒ“ƒZƒ‹
*
*  AUTHOR          : ‰Á‰ê’J ‘å•ã
*  DATE            : 2006.09.28
*
*  DEVISION 001    : ____.__.__ ( QQQFQQQQQQQQQQQQQQQQQQQQ
*  DEVISION 002    : ____.__.__ ( QQQFQQQQQQQQQQQQQQQQQQQQ
*  DEVISION 003    : ____.__.__ ( QQQFQQQQQQQQQQQQQQQQQQQQ
*
*  PARAMETER
*        HWND        hWnd                : ƒEƒCƒ“ƒhƒEƒnƒ“ƒhƒ‹
*
***********************************************************************************/
void TextTimerCancel ( HWND hWnd ) {

    if ( 0 != TxtRcv_timer_id ) {
        KillTimer ( hWnd, TxtRcv_timer_id );
        TxtRcv_timer_id = 0;
    }

    return;
}

/***********************************************************************************
*
*  ‚d‚n‚sóM‘Ò‚¿ƒ^ƒCƒ}[ƒZƒbƒg
*
*  AUTHOR          : ‰Á‰ê’J ‘å•ã
*  DATE            : 2006.09.28
*
*  DEVISION 001    : ____.__.__ ( QQQFQQQQQQQQQQQQQQQQQQQQ
*  DEVISION 002    : ____.__.__ ( QQQFQQQQQQQQQQQQQQQQQQQQ
*  DEVISION 003    : ____.__.__ ( QQQFQQQQQQQQQQQQQQQQQQQQ
*
*  PARAMETER
*        HWND        hWnd               F ƒEƒCƒ“ƒhƒEƒnƒ“ƒhƒ‹
*
***********************************************************************************/
void EotTimerSet ( HWND hWnd ) {

    if ( 0 == EotRcv_timer_id ) {
                             /* ƒ^ƒCƒ}[ƒZƒbƒg */
        EotRcv_timer_id = SetTimer ( hWnd, EB_TMRID_EOTRV, tcpCtrl.tm_eot_aak1, (TIMERPROC) EotTimeOut );
        if ( 0 == EotRcv_timer_id ) {
            MyMessage ( hWnd, "‚d‚n‚s‘Ò‚¿ƒ^ƒCƒ}[¸”s" );
        } else {
            MyMessage ( hWnd, "‚d‚n‚s‘Ò‚¿ƒ^ƒCƒ}[¬Œ÷ LINE=%d", __LINE__ );
        }
    }

    return;
}

/***********************************************************************************
*
*  ‚d‚n‚sóM‘Ò‚¿ƒ^ƒCƒ€ƒAƒEƒg
*
*  AUTHOR          : ‰Á‰ê’J ‘å•ã
*  DATE            : 2006.09.28
*
*  DEVISION 001    : ____.__.__ ( QQQFQQQQQQQQQQQQQQQQQQQQ
*  DEVISION 002    : ____.__.__ ( QQQFQQQQQQQQQQQQQQQQQQQQ
*  DEVISION 003    : ____.__.__ ( QQQFQQQQQQQQQQQQQQQQQQQQ
*
*  PARAMETER
*        HWND        hWnd                : ƒEƒCƒ“ƒhƒEƒnƒ“ƒhƒ‹
*        UINT        uMessage            : ƒƒbƒZ[ƒW
*        UINT        idEvent             : ƒ^ƒCƒ}[ID
*        DWORD       dwTime              : ƒ^ƒCƒ}[Œo‰ßŠÔ
*
***********************************************************************************/
void CALLBACK EotTimeOut ( HWND hWnd, UINT uMessage, UINT idEvent, DWORD dwTime ) {

    MyMessage ( hWnd, "‚d‚n‚sóM‘Ò‚¿ƒ^ƒCƒ€ƒAƒEƒgI LINE=%d ", __LINE__ );
                             /* ƒ^ƒCƒ}[ƒLƒƒƒ“ƒZƒ‹ */
    EotTimerCancel ( hWnd );
                             /* “`‘—I—¹ˆ— */
    EnetB_DBend ( hWnd );

    return;
}

/***********************************************************************************
*
*  ‚d‚n‚sóM‘Ò‚¿ƒ^ƒCƒ}[ƒLƒƒƒ“ƒZƒ‹
*
*  AUTHOR          : ‰Á‰ê’J ‘å•ã
*  DATE            : 2006.09.28
*
*  DEVISION 001    : ____.__.__ ( QQQFQQQQQQQQQQQQQQQQQQQQ
*  DEVISION 002    : ____.__.__ ( QQQFQQQQQQQQQQQQQQQQQQQQ
*  DEVISION 003    : ____.__.__ ( QQQFQQQQQQQQQQQQQQQQQQQQ
*
*  PARAMETER
*        HWND        hWnd                : ƒEƒCƒ“ƒhƒEƒnƒ“ƒhƒ‹
*
***********************************************************************************/
void EotTimerCancel ( HWND hWnd ) {

    if ( 0 != EotRcv_timer_id ) {
        KillTimer ( hWnd, EotRcv_timer_id );
        EotRcv_timer_id = 0;
    }

    return;
}

/***********************************************************************************
*
*  ‘—MƒeƒLƒXƒgƒoƒbƒtƒ@EÅŒÃƒŒƒR[ƒhíœ
*
*  AUTHOR          : ‰Á‰ê’J ‘å•ã
*  DATE            : 2006.09.28
*
*  DEVISION 001    : ____.__.__ ( QQQFQQQQQQQQQQQQQQQQQQQQ
*  DEVISION 002    : ____.__.__ ( QQQFQQQQQQQQQQQQQQQQQQQQ
*  DEVISION 003    : ____.__.__ ( QQQFQQQQQQQQQQQQQQQQQQQQ
*
*  PARAMETER
*        HWND        hWnd                : ƒEƒCƒ“ƒhƒEƒnƒ“ƒhƒ‹
*
***********************************************************************************/
void Ebsdbuf_oldrecdel ( HWND hWnd ) {

    if ( eb_sndbuf.out_point == eb_sndbuf.in_point ) {
        MyMessage ( hWnd, "‘—Mƒoƒbƒtƒ@‚d‚l‚o‚s‚xI LINE=%d ", __LINE__ );
    } else {
                             /* ƒeƒLƒXƒgƒoƒbƒtƒ@‰Šú‰» */
        memset ( eb_sndbuf.snd_buff[eb_sndbuf.out_point], '\0', ENETB_MAXSIZE );
                             /* ƒTƒCƒYƒoƒbƒtƒ@‰Šú‰» */
        eb_sndbuf.snd_size[eb_sndbuf.out_point] = 0;
                             /* ‚n‚t‚sƒ|ƒCƒ“ƒ^XV */
        if ( eb_sndbuf.out_point < ENETB_MXPOINT - 1 ) {
            eb_sndbuf.out_point = eb_sndbuf.out_point + 1;
        } else {
            eb_sndbuf.out_point = 0;
        }
    }

    return;
}

/***********************************************************************************
*
*  ‘—MƒeƒLƒXƒgƒoƒbƒtƒ@EƒeƒLƒXƒg“o˜^
*
*  AUTHOR          : ‰Á‰ê’J ‘å•ã
*  DATE            : 2006.09.29
*
*  DEVISION 001    : ____.__.__ ( QQQFQQQQQQQQQQQQQQQQQQQQ
*  DEVISION 002    : ____.__.__ ( QQQFQQQQQQQQQQQQQQQQQQQQ
*  DEVISION 003    : ____.__.__ ( QQQFQQQQQQQQQQQQQQQQQQQQ
*
*  PARAMETER
*        HWND        hWnd                : ƒEƒCƒ“ƒhƒEƒnƒ“ƒhƒ‹
*        int         itxsiz              : ƒeƒLƒXƒgƒTƒCƒY
*        char*       ctxstr              : ƒeƒLƒXƒg•¶š—ñ
*
*  return
*        0 : ³í
*       -1 : ƒoƒbƒtƒ@‚e‚t‚k‚k
*
***********************************************************************************/
int Ebsdbuf_recset ( HWND hWnd, int itxsiz, char* ctxstr ) {

    int  iretc;

    iretc = 0;

    if ( eb_sndbuf.out_point == eb_sndbuf.in_point + 1 ||
       ( eb_sndbuf.out_point == 0 && eb_sndbuf.in_point == ENETB_MXPOINT - 1 ) ) {
        iretc = -1;
        MyMessage ( hWnd, "‘—Mƒoƒbƒtƒ@‚e‚t‚k‚kI LINE=%d ", __LINE__ );
    } else {
                             /* ƒeƒLƒXƒgƒoƒbƒtƒ@“o˜^ */
        memcpy ( eb_sndbuf.snd_buff[eb_sndbuf.in_point], ctxstr, itxsiz );
                             /* ƒTƒCƒYƒoƒbƒtƒ@“o˜^ */
        eb_sndbuf.snd_size[eb_sndbuf.in_point] = itxsiz;
                             /* ‚h‚mƒ|ƒCƒ“ƒ^XV */
        if ( eb_sndbuf.in_point < ENETB_MXPOINT - 1 ) {
            eb_sndbuf.in_point = eb_sndbuf.in_point + 1;
        } else {
            eb_sndbuf.in_point = 0;
        }
    }

    return iretc;
}

/***********************************************************************************
*
*  §Œä•¶šƒR[ƒhƒ`ƒFƒbƒN
*
*  AUTHOR          : ‰Á‰ê’J ‘å•ã
*  DATE            : 2006.09.28
*
*  DEVISION 001    : ____.__.__ ( QQQFQQQQQQQQQQQQQQQQQQQQ
*  DEVISION 002    : ____.__.__ ( QQQFQQQQQQQQQQQQQQQQQQQQ
*  DEVISION 003    : ____.__.__ ( QQQFQQQQQQQQQQQQQQQQQQQQ
*
*  PARAMETER
*        HWND        hWnd                : ƒEƒCƒ“ƒhƒEƒnƒ“ƒhƒ‹
*        char*       cdbstr              : “`•¶•¶š—ñ
*
*  return
*        §Œä•¶šƒe[ƒuƒ‹‚Ì‚h‚m‚c‚d‚w
*
***********************************************************************************/
int Ebchk_ctrlstr ( HWND hWnd, char* cdbstr ) {

    int  iloop1;             /* ƒ‹[ƒv•Ï” */
    int  indxwk;             /* §Œä•¶š”z—ñ‚h‚m‚c‚d‚w */

                             /* ‰Šú‰» */
    indxwk = ENBCTL_IXTXT;
    if ( _ASCII == tcpCtrl.eb_mojicode ) {
                             /* ‚`‚r‚b‚h‚hƒR[ƒh */
        for ( iloop1 = 0; iloop1 < ICTL_CHRNM; iloop1++ ) {
                             /* §Œä•¶šƒ`ƒFƒbƒN */
            if ( 0 == memcmp ( cdbstr, ENB_CTLASC[iloop1], ENB_CTLSIZ[iloop1] ) ) {
                indxwk = iloop1;
            }
        }
    } else {
                             /* ‚d‚a‚b‚c‚h‚bƒR[ƒh */
        for ( iloop1 = 0; iloop1 < ICTL_CHRNM; iloop1++ ) {
                             /* §Œä•¶šƒ`ƒFƒbƒN */
            if ( 0 == memcmp ( cdbstr, ENB_CTLEBC[iloop1], ENB_CTLSIZ[iloop1] ) ) {
                indxwk = iloop1;
            }
        }
    }

    return indxwk;
}

/***********************************************************************************
*
*  ‚d‚‚…‚”|‚a‘—MŠJnˆ—
*
*  AUTHOR          : ‰Á‰ê’J ‘å•ã
*  DATE            : 2006.09.29
*
*  DEVISION 001    : ____.__.__ ( QQQFQQQQQQQQQQQQQQQQQQQQ
*  DEVISION 002    : ____.__.__ ( QQQFQQQQQQQQQQQQQQQQQQQQ
*  DEVISION 003    : ____.__.__ ( QQQFQQQQQQQQQQQQQQQQQQQQ
*
*  PARAMETER
*        HWND        hWnd                : ƒEƒCƒ“ƒhƒEƒnƒ“ƒhƒ‹
*        int         send_port           : ‘—Mƒ|[ƒg
*        int         itxsiz              : ƒeƒLƒXƒgƒTƒCƒY
*        char*       ctxstr              : ƒeƒLƒXƒg•¶š—ñ
*
***********************************************************************************/
void EnetB_1stSnd ( HWND hWnd, int send_port, int itxsiz, char* ctxstr ) {

    char      c_enqstr[1];    /* ‚d‚m‚p•¶š—ñ */
    int       iebrtc;         /* •œ‹Aî•ñ */

                              /* ‘—MƒeƒLƒXƒgƒoƒbƒtƒ@“o˜^ */
    iebrtc = Ebsdbuf_recset ( hWnd, itxsiz, ctxstr );
                              /* ƒoƒbƒtƒ@“o˜^Š®—¹•‘—M’†‚Å‚È‚¢ */
    if ( EB_NOTSND == g_eb_sndst && 0 == iebrtc ) {
                             /* ‚d‚m‚pÄ‘— */
        if ( _ASCII == tcpCtrl.eb_mojicode ) {
            memcpy ( c_enqstr, ENB_CTLASC[ENBCTL_IXENQ], sizeof ( c_enqstr ) );
        } else {
            memcpy ( c_enqstr, ENB_CTLEBC[ENBCTL_IXENQ], sizeof ( c_enqstr ) );
        }
        SetSndMsg ( hWnd, sizeof ( c_enqstr ), c_enqstr );
    }

    return;
}

/***********************************************************************************
*
*  ‚d‚‚…‚”|‚a‚w‘—MŠJnˆ—
*
*  AUTHOR          : ‰Á‰ê’J ‘å•ã
*  DATE            : 2006.09.29
*
*  DEVISION 001    : 2013.08.31 (  “c’† F“§‰ßƒ‚[ƒhA‚c‚k‚d‚ğ‘}“ü‚·‚é‚æ‚¤‚É‰ü‘¢
*                                         EnetB_1stSnd ‚ğŒ³‚É‰ü‘¢
*  DEVISION 002    : ____.__.__ ( QQQFQQQQQQQQQQQQQQQQQQQQ
*  DEVISION 003    : ____.__.__ ( QQQFQQQQQQQQQQQQQQQQQQQQ
*
*  PARAMETER
*        HWND        hWnd                : ƒEƒCƒ“ƒhƒEƒnƒ“ƒhƒ‹
*        int         send_port           : ‘—Mƒ|[ƒg
*        int         itxsiz              : ƒeƒLƒXƒgƒTƒCƒY
*        char*       ctxstr              : ƒeƒLƒXƒg•¶š—ñ
*
***********************************************************************************/
void EnetBX_1stSnd ( HWND hWnd, int send_port, int itxsiz, char* ctxstr ) {

    char      c_enqstr[1];    /* ‚d‚m‚p•¶š—ñ */
    int       iebrtc;         /* •œ‹Aî•ñ */

                              /* ‚c‚k‚d‚Ì‘}“üEœ‹‚ğs‚¤i“§‰ßƒ‚[ƒh‚Ì‚İj */
    EnetBX_Dle_ins ( hWnd, &itxsiz, ctxstr );

                              /* ‘—MƒeƒLƒXƒgƒoƒbƒtƒ@“o˜^ */
    iebrtc = Ebsdbuf_recset ( hWnd, itxsiz, ctxstr );
                              /* ƒoƒbƒtƒ@“o˜^Š®—¹•‘—M’†‚Å‚È‚¢ */
    if ( EB_NOTSND == g_eb_sndst && 0 == iebrtc ) {
                             /* ‚d‚m‚pÄ‘— */
        if ( _ASCII == tcpCtrl.eb_mojicode ) {
            memcpy ( c_enqstr, ENB_CTLASC[ENBCTL_IXENQ], sizeof ( c_enqstr ) );
        } else {
            memcpy ( c_enqstr, ENB_CTLEBC[ENBCTL_IXENQ], sizeof ( c_enqstr ) );
        }
        SetSndMsg ( hWnd, sizeof ( c_enqstr ), c_enqstr );
    }

    return;
}

/***********************************************************************************
*
*  ‚d‚‚…‚”|‚a“`‘—I—¹ˆ—
*
*  AUTHOR          : ‰Á‰ê’J ‘å•ã
*  DATE            : 2006.10.03
*
*  DEVISION 001    : ____.__.__ ( QQQFQQQQQQQQQQQQQQQQQQQQ
*  DEVISION 002    : ____.__.__ ( QQQFQQQQQQQQQQQQQQQQQQQQ
*  DEVISION 003    : ____.__.__ ( QQQFQQQQQQQQQQQQQQQQQQQQ
*
*  PARAMETER
*        HWND        hWnd                : ƒEƒCƒ“ƒhƒEƒnƒ“ƒhƒ‹
*
***********************************************************************************/
void EnetB_DBend ( HWND hWnd ) {

    char      c_enqstr[1];   /* ‚d‚m‚p•¶š—ñ */

                             /* ‘—M’†î•ñ‰Šú‰» */
    g_eb_sndst = EB_NOTSND;
                             /* –¢‘—MƒeƒLƒXƒg—L‚èH */
    if ( eb_sndbuf.in_point != eb_sndbuf.out_point ) {
        MyMessage ( hWnd, "–¢‘—MƒeƒLƒXƒg—L‚èII LINE:%d ", __LINE__ );
                             /* ŠJ”­‹@ƒ|[ƒg‚Ö‚d‚m‚p‘—M */
        if ( _ASCII == tcpCtrl.eb_mojicode ) {
            memcpy ( c_enqstr, ENB_CTLASC[ENBCTL_IXENQ], sizeof ( c_enqstr ) );
        } else {
            memcpy ( c_enqstr, ENB_CTLEBC[ENBCTL_IXENQ], sizeof ( c_enqstr ) );
        }
        SetSndMsg ( hWnd, sizeof ( c_enqstr ), c_enqstr );
    }

    return;
}

/***********************************************************************************
*
*  ƒf[ƒ^‚É•\‚ê‚½‚c‚k‚d‚Ì‘O‚É‚c‚k‚d‚ğ‘}“ü
*
*  AUTHOR          : “c’† »
*  DATE            : 2013.08.21
*
*  DEVISION 001    : ____.__.__ ( QQQFQQQQQQQQQQQQQQQQQQQQ
*  DEVISION 002    : ____.__.__ ( QQQFQQQQQQQQQQQQQQQQQQQQ
*  DEVISION 003    : ____.__.__ ( QQQFQQQQQQQQQQQQQQQQQQQQ
*
*  PARAMETER
*        HWND        hWnd                : ƒEƒCƒ“ƒhƒEƒnƒ“ƒhƒ‹
*        int         insize              : ƒeƒLƒXƒgƒTƒCƒY
*        char*       intext              : ƒeƒLƒXƒg•¶š—ñ
*
***********************************************************************************/
void EnetBX_Dle_ins ( HWND hWnd, int* itxsiz, char* ctxstr ) {

    int       iloop1;         /* ƒ‹[ƒv•Ï” */
    int       isetpt;         /* Ši”[ƒ|ƒCƒ“ƒ^ */
                              /* ’†ŠÔƒoƒbƒtƒ@ */
    char      ctemp[ENETB_MAXSIZE];

    isetpt = 0;
    memset( ctemp, 0x00, sizeof( ctemp ) );

    if ( EB____TRNS != tcpCtrl.eb_transpar ) {
                             /* “§‰ßƒ‚[ƒhˆÈŠO‚Ìê‡‚Íˆ—‚È‚µ */
        return;
    }

    if ( _ASCII == tcpCtrl.eb_mojicode ) {
                             /* ‚`‚r‚b‚h‚hƒR[ƒh */
        for ( iloop1 = 0; iloop1 < *itxsiz; iloop1++ ) {
                             /* ‚c‚k‚dƒR[ƒhƒ`ƒFƒbƒN */
            if ( 0 == memcmp ( (char*)&ctxstr[iloop1], ENB_CTLASC[ENBCTL_IXDLE], ENB_CTLSIZ[ENBCTL_IXDLE] ) ) {
                             /* ‚c‚k‚dƒR[ƒh‚Ìê‡‚ÍA‘O‚É‚c‚k‚d‚ğ‘}“ü‚·‚é */
                memcpy( &ctemp[isetpt + 0], ENB_CTLASC[ENBCTL_IXDLE], 1);
                memcpy( &ctemp[isetpt + 1], (char*)&ctxstr[iloop1], 1);
                isetpt = isetpt + 2;
            } else {
                memcpy( &ctemp[isetpt], (char*)&ctxstr[iloop1], 1);
                isetpt = isetpt + 1;
            }
        }

    } else {
                             /* ‚d‚a‚b‚c‚h‚bƒR[ƒh */
        for ( iloop1 = 0; iloop1 < *itxsiz; iloop1++ ) {
                             /* ‚c‚k‚dƒR[ƒhƒ`ƒFƒbƒN */
            if ( 0 == memcmp ( (char*)&ctxstr[iloop1], ENB_CTLEBC[ENBCTL_IXDLE], ENB_CTLSIZ[ENBCTL_IXDLE] ) ) {
                             /* ‚c‚k‚dƒR[ƒh‚Ìê‡‚ÍA‘O‚É‚c‚k‚d‚ğ‘}“ü‚·‚é */
                memcpy( &ctemp[isetpt + 0], ENB_CTLEBC[ENBCTL_IXDLE], 1);
                memcpy( &ctemp[isetpt + 1], (char*)&ctxstr[iloop1], 1);
                isetpt = isetpt + 2;
            } else {
                memcpy( &ctemp[isetpt], (char*)&ctxstr[iloop1], 1);
                isetpt = isetpt + 1;
            }
        }

    }

    memcpy(ctxstr, &ctemp, isetpt);
    *itxsiz = isetpt;

    return;
}

/***********************************************************************************
*
*  ƒf[ƒ^‚É•\‚ê‚½‚c‚k‚d‚P•¶š–Ú‚ÍƒGƒXƒP[ƒv•¶š‚Æ‚µ‚Äíœ
*  ‚½‚¾‚µASTX^ETX ‚ÍœŠO
*
*  AUTHOR          : “c’† »
*  DATE            : 2013.08.21
*
*  DEVISION 001    : ____.__.__ ( QQQFQQQQQQQQQQQQQQQQQQQQ
*  DEVISION 002    : ____.__.__ ( QQQFQQQQQQQQQQQQQQQQQQQQ
*  DEVISION 003    : ____.__.__ ( QQQFQQQQQQQQQQQQQQQQQQQQ
*
*  PARAMETER
*        HWND        hWnd                : ƒEƒCƒ“ƒhƒEƒnƒ“ƒhƒ‹
*        int         insize              : ƒeƒLƒXƒgƒTƒCƒY
*        char*       intext              : ƒeƒLƒXƒg•¶š—ñ
*
***********************************************************************************/
void EnetBX_Dle_del ( HWND hWnd, int* itxsiz, char* ctxstr ) {

    int       iloop1;         /* ƒ‹[ƒv•Ï” */
    int       isetpt;         /* Ši”[ƒ|ƒCƒ“ƒ^ */
                              /* ’†ŠÔƒoƒbƒtƒ@ */
    char      ctemp[ENETB_MAXSIZE];
    int       iflg_dle;       /* ‚c‚k‚dƒtƒ‰ƒO */

    isetpt = 0;
    iflg_dle = 0;
    memset( ctemp, 0x00, sizeof( ctemp ) );

    if ( EB____TRNS != tcpCtrl.eb_transpar ) {
                             /* “§‰ßƒ‚[ƒhˆÈŠO‚Ìê‡‚Íˆ—‚È‚µ */
        return;
    }

    if ( _ASCII == tcpCtrl.eb_mojicode ) {
                             /* ‚`‚r‚b‚h‚hƒR[ƒh */
        for ( iloop1 = 0; iloop1 < *itxsiz; iloop1++ ) {
                             /* ‚c‚k‚dƒR[ƒhƒ`ƒFƒbƒN */
            if ( 0 == memcmp ( (char*)&ctxstr[iloop1], ENB_CTLASC[ENBCTL_IXDLE], ENB_CTLSIZ[ENBCTL_IXDLE] ) ) {
                             /* ‚c‚k‚dƒR[ƒh‚Ìê‡‚ÍAŸ‚Ì‚c‚k‚d‚ğíœ‚·‚é */
                if ( ( 0 < iloop1 ) && ( iloop1 < ( *itxsiz - 2 ) ) && ( 0 == iflg_dle ) ) {
                    iflg_dle = 1;
                } else {
                    memcpy( &ctemp[isetpt], (char*)&ctxstr[iloop1], 1);
                    isetpt = isetpt + 1;
                    iflg_dle = 0;
                }

            } else {
                memcpy( &ctemp[isetpt], (char*)&ctxstr[iloop1], 1);
                isetpt = isetpt + 1;
                iflg_dle = 0;
            }
        }

    } else {
                             /* ‚d‚a‚b‚c‚h‚bƒR[ƒh */
        for ( iloop1 = 0; iloop1 < *itxsiz; iloop1++ ) {
                             /* ‚c‚k‚dƒR[ƒhƒ`ƒFƒbƒN */
            if ( 0 == memcmp ( (char*)&ctxstr[iloop1], ENB_CTLEBC[ENBCTL_IXDLE], ENB_CTLSIZ[ENBCTL_IXDLE] ) ) {
                             /* ‚c‚k‚dƒR[ƒh‚Ìê‡‚ÍAŸ‚Ì‚c‚k‚d‚ğíœ‚·‚é */
                if ( ( 0 < iloop1 ) && ( iloop1 < ( *itxsiz - 2 ) ) && ( 0 == iflg_dle ) ) {
                    iflg_dle = 1;
                } else {
                    memcpy( &ctemp[isetpt], (char*)&ctxstr[iloop1], 1);
                    isetpt = isetpt + 1;
                    iflg_dle = 0;
                }

            } else {
                memcpy( &ctemp[isetpt], (char*)&ctxstr[iloop1], 1);
                isetpt = isetpt + 1;
                iflg_dle = 0;
            }
        }

    }

    memcpy(ctxstr, &ctemp, isetpt);
    *itxsiz = isetpt;

    return;
}
