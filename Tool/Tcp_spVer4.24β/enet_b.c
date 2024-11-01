/***********************************************************************************
*
*  Ｅｎｅｔ−Ｂ関数グローバル領域
*
*  AUTHOR          : 加賀谷 大輔
*  DATE            : 2006.09.27
*
*  DEVISION 001    : 2013.08.31 (  田中 ：ベーシック手順伝送セミの透過モードに対応。
*                                         既設処理への影響を考え、従来関数に干渉
*                                         しないように開発。
*  DEVISION 002    : ____.__.__ ( ＿＿＿：＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿
*  DEVISION 003    : ____.__.__ ( ＿＿＿：＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿
*
***********************************************************************************/

                             /* ＩＮＣＬＵＤＥ宣言 */
#include    <stdio.h>
#include    <windows.h>

#include    "enet_b.h"
#include    "TcpCtrl.h"
#include    "Pcterm.h"

                             /* グローバル変数 */
UINT    EnqSnd_timer_id = 0;
UINT    Ak0Rcv_timer_id = 0;
UINT    Ak1Rcv_timer_id = 0;
UINT    TxtRcv_timer_id = 0;
UINT    EotRcv_timer_id = 0;

/***********************************************************************************
*
*  Ｅｎｅｔ−Ｂグローバル変数初期化処理
*
*  AUTHOR          : 加賀谷 大輔
*  DATE            : 2006.10.03
*
*  DEVISION 001    : ____.__.__ ( ＿＿＿：＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿
*  DEVISION 002    : ____.__.__ ( ＿＿＿：＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿
*  DEVISION 003    : ____.__.__ ( ＿＿＿：＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿
*
*  PARAMETER
*        HWND        hWnd               ： ウインドウハンドル
*
***********************************************************************************/
void EnetB_Gbl_init ( HWND hWnd ) {

                             /* 送信バッファグローバル */
    eb_sndbuf.in_point = 0;
    eb_sndbuf.out_point = 0;
    memset ( eb_sndbuf.snd_size, 0x00, sizeof ( eb_sndbuf.snd_size ) );
    memset ( eb_sndbuf.snd_buff, 0x00, sizeof ( eb_sndbuf.snd_buff ) );
                             /* 送信中状態グローバル */
    g_eb_sndst = EB_NOTSND;
                             /* リトライカウンタグローバル */
    g_eb_cntenq = 0;
    g_eb_cnttxt = 0;
                             /* 受信バッファグローバル */
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
*  ＥＮＱ再送起動タイマーセット
*
*  AUTHOR          : 加賀谷 大輔
*  DATE            : 2006.09.27
*
*  DEVISION 001    : ____.__.__ ( ＿＿＿：＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿
*  DEVISION 002    : ____.__.__ ( ＿＿＿：＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿
*  DEVISION 003    : ____.__.__ ( ＿＿＿：＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿
*
*  PARAMETER
*        HWND        hWnd               ： ウインドウハンドル
*
***********************************************************************************/
void EnqTimerSet ( HWND hWnd ) {

    if ( 0 == EnqSnd_timer_id ) {
                             /* タイマーセット */
        EnqSnd_timer_id = SetTimer ( hWnd, EB_TMRID_ENQSD, tcpCtrl.tm_enq_anak, (TIMERPROC) EnqTimeOut );
        if ( 0 == EnqSnd_timer_id ) {
            MyMessage ( hWnd, "ＥＮＱ再送タイマー起動失敗" );
        } else {
            MyMessage ( hWnd, "ＥＮＱ再送タイマー起動成功 LINE=%d", __LINE__ );
        }
    }

    return;
}

/***********************************************************************************
*
*  ＥＮＱ再送タイマー起動
*
*  AUTHOR          : 加賀谷 大輔
*  DATE            : 2006.09.27
*
*  DEVISION 001    : ____.__.__ ( ＿＿＿：＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿
*  DEVISION 002    : ____.__.__ ( ＿＿＿：＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿
*  DEVISION 003    : ____.__.__ ( ＿＿＿：＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿
*
*  PARAMETER
*        HWND        hWnd                : ウインドウハンドル
*        UINT        uMessage            : メッセージ
*        UINT        idEvent             : タイマーID
*        DWORD       dwTime              : タイマー経過時間
*
***********************************************************************************/
void CALLBACK EnqTimeOut ( HWND hWnd, UINT uMessage, UINT idEvent, DWORD dwTime ) {

    char      c_enqstr[1];             /* ＥＮＱ文字列 */

    MyMessage ( hWnd, "ＥＮＱ再送タイムアウト！ LINE=%d ", __LINE__ );
                             /* タイマーキャンセル */
    EnqTimerCancel ( hWnd );
                             /* 送信中情報更新 */
    g_eb_sndst = EB_NOTSND;
                             /* リトライ送信？ */
    if ( g_eb_cntenq < tcpCtrl.nm_enq_nans ) {
        MyMessage ( hWnd, "再送開始！ LINE=%d ", __LINE__ );
                             /* ＥＮＱ再送 */
        if ( _ASCII == tcpCtrl.eb_mojicode ) {
            memcpy ( c_enqstr, ENB_CTLASC[ENBCTL_IXENQ], sizeof ( c_enqstr ) );
        } else {
            memcpy ( c_enqstr, ENB_CTLEBC[ENBCTL_IXENQ], sizeof ( c_enqstr ) );
        }
        SetSndMsg ( hWnd, sizeof ( c_enqstr ), c_enqstr );

        g_eb_cntenq = g_eb_cntenq + 1;

    } else {
        MyMessage ( hWnd, "ＥＮＱリトライ回数オーバー！ LINE=%d ", __LINE__ );
                             /* 送信テキストバッファ最古レコード削除 */
        Ebsdbuf_oldrecdel ( hWnd );
                             /* リトライカウンタ初期化 */
        g_eb_cntenq = 0;
                             /* 伝送終了処理 */
        EnetB_DBend ( hWnd );
    }

    return;
}

/***********************************************************************************
*
*  ＥＮＱ再送タイマーキャンセル
*
*  AUTHOR          : 加賀谷 大輔
*  DATE            : 2006.09.28
*
*  DEVISION 001    : ____.__.__ ( ＿＿＿：＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿
*  DEVISION 002    : ____.__.__ ( ＿＿＿：＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿
*  DEVISION 003    : ____.__.__ ( ＿＿＿：＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿
*
*  PARAMETER
*        HWND        hWnd                : ウインドウハンドル
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
*  ＡＣＫ０受信待ちタイマーセット
*
*  AUTHOR          : 加賀谷 大輔
*  DATE            : 2006.09.28
*
*  DEVISION 001    : ____.__.__ ( ＿＿＿：＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿
*  DEVISION 002    : ____.__.__ ( ＿＿＿：＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿
*  DEVISION 003    : ____.__.__ ( ＿＿＿：＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿
*
*  PARAMETER
*        HWND        hWnd               ： ウインドウハンドル
*
***********************************************************************************/
void Ack0TimerSet ( HWND hWnd ) {

    if ( 0 == Ak0Rcv_timer_id ) {
                             /* タイマーセット */
        Ak0Rcv_timer_id = SetTimer ( hWnd, EB_TMRID_AK0RV, tcpCtrl.tm_ak0_aenq, (TIMERPROC) Ack0TimeOut );
        if ( 0 == Ak0Rcv_timer_id ) {
            MyMessage ( hWnd, "ＡＣＫ０待ちタイマー失敗" );
        } else {
            MyMessage ( hWnd, "ＡＣＫ０待ちタイマー成功 LINE=%d", __LINE__ );
        }
    }

    return;
}

/***********************************************************************************
*
*  ＡＣＫ０受信待ちタイムアウト
*
*  AUTHOR          : 加賀谷 大輔
*  DATE            : 2006.09.28
*
*  DEVISION 001    : ____.__.__ ( ＿＿＿：＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿
*  DEVISION 002    : ____.__.__ ( ＿＿＿：＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿
*  DEVISION 003    : ____.__.__ ( ＿＿＿：＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿
*
*  PARAMETER
*        HWND        hWnd                : ウインドウハンドル
*        UINT        uMessage            : メッセージ
*        UINT        idEvent             : タイマーID
*        DWORD       dwTime              : タイマー経過時間
*
***********************************************************************************/
void CALLBACK Ack0TimeOut ( HWND hWnd, UINT uMessage, UINT idEvent, DWORD dwTime ) {

    char      c_enqstr[1];             /* ＥＮＱ文字列 */

    MyMessage ( hWnd, "ＡＣＫ０受信待ちタイムアウト！ LINE=%d ", __LINE__ );
                             /* タイマーキャンセル */
    Ack0TimerCancel ( hWnd );
                             /* 送信中情報初期化 */
    g_eb_sndst = EB_NOTSND;
                             /* リトライ送信？ */
    if ( g_eb_cntenq < tcpCtrl.nm_enq_nans ) {
                             /* ＥＮＱ再送 */
        if ( _ASCII == tcpCtrl.eb_mojicode ) {
            memcpy ( c_enqstr, ENB_CTLASC[ENBCTL_IXENQ], sizeof ( c_enqstr ) );
        } else {
            memcpy ( c_enqstr, ENB_CTLEBC[ENBCTL_IXENQ], sizeof ( c_enqstr ) );
        }
        SetSndMsg ( hWnd, sizeof ( c_enqstr ), c_enqstr );
                             /* 再送回数更新 */
        g_eb_cntenq = g_eb_cntenq + 1;
    } else {
        MyMessage ( hWnd, "ＥＮＱリトライ回数オーバー！ LINE=%d ", __LINE__ );
                             /* 送信テキストバッファ最古レコード削除 */
        Ebsdbuf_oldrecdel ( hWnd );
                             /* リトライカウンタ初期化 */
        g_eb_cntenq = 0;
                             /* 伝送終了処理 */
        EnetB_DBend ( hWnd );
    }

    return;
}

/***********************************************************************************
*
*  ＡＣＫ０受信待ちタイマーキャンセル
*
*  AUTHOR          : 加賀谷 大輔
*  DATE            : 2006.09.28
*
*  DEVISION 001    : ____.__.__ ( ＿＿＿：＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿
*  DEVISION 002    : ____.__.__ ( ＿＿＿：＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿
*  DEVISION 003    : ____.__.__ ( ＿＿＿：＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿
*
*  PARAMETER
*        HWND        hWnd                : ウインドウハンドル
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
*  ＡＣＫ１受信待ちタイマーセット
*
*  AUTHOR          : 加賀谷 大輔
*  DATE            : 2006.09.28
*
*  DEVISION 001    : ____.__.__ ( ＿＿＿：＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿
*  DEVISION 002    : ____.__.__ ( ＿＿＿：＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿
*  DEVISION 003    : ____.__.__ ( ＿＿＿：＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿
*
*  PARAMETER
*        HWND        hWnd               ： ウインドウハンドル
*
***********************************************************************************/
void Ack1TimerSet ( HWND hWnd ) {

    if ( 0 == Ak1Rcv_timer_id ) {
                             /* タイマーセット */
        Ak1Rcv_timer_id = SetTimer ( hWnd, EB_TMRID_AK1RV, tcpCtrl.tm_ak1_atxt, (TIMERPROC) Ack1TimeOut );
        if ( 0 == Ak1Rcv_timer_id ) {
            MyMessage ( hWnd, "ＡＣＫ１待ちタイマー失敗" );
        } else {
            MyMessage ( hWnd, "ＡＣＫ１待ちタイマー成功 LINE=%d", __LINE__ );
        }
    }

    return;
}

/***********************************************************************************
*
*  ＡＣＫ１受信待ちタイムアウト
*
*  AUTHOR          : 加賀谷 大輔
*  DATE            : 2006.09.28
*
*  DEVISION 001    : ____.__.__ ( ＿＿＿：＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿
*  DEVISION 002    : ____.__.__ ( ＿＿＿：＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿
*  DEVISION 003    : ____.__.__ ( ＿＿＿：＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿
*
*  PARAMETER
*        HWND        hWnd                : ウインドウハンドル
*        UINT        uMessage            : メッセージ
*        UINT        idEvent             : タイマーID
*        DWORD       dwTime              : タイマー経過時間
*
***********************************************************************************/
void CALLBACK Ack1TimeOut ( HWND hWnd, UINT uMessage, UINT idEvent, DWORD dwTime ) {

    MyMessage ( hWnd, "ＡＣＫ１受信待ちタイムアウト！ LINE=%d ", __LINE__ );
                             /* タイマーキャンセル */
    Ack1TimerCancel ( hWnd );
                             /* 送信テキストバッファ最古レコード削除 */
    Ebsdbuf_oldrecdel ( hWnd );
                             /* リトライカウンタ初期化 */
    g_eb_cnttxt = 0;
                             /* 伝送終了処理 */
    EnetB_DBend ( hWnd );

    return;
}

/***********************************************************************************
*
*  ＡＣＫ１受信待ちタイマーキャンセル
*
*  AUTHOR          : 加賀谷 大輔
*  DATE            : 2006.09.28
*
*  DEVISION 001    : ____.__.__ ( ＿＿＿：＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿
*  DEVISION 002    : ____.__.__ ( ＿＿＿：＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿
*  DEVISION 003    : ____.__.__ ( ＿＿＿：＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿
*
*  PARAMETER
*        HWND        hWnd                : ウインドウハンドル
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
*  テキスト受信待ちタイマーセット
*
*  AUTHOR          : 加賀谷 大輔
*  DATE            : 2006.09.28
*
*  DEVISION 001    : ____.__.__ ( ＿＿＿：＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿
*  DEVISION 002    : ____.__.__ ( ＿＿＿：＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿
*  DEVISION 003    : ____.__.__ ( ＿＿＿：＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿
*
*  PARAMETER
*        HWND        hWnd               ： ウインドウハンドル
*
***********************************************************************************/
void TextTimerSet ( HWND hWnd ) {

    if ( 0 == TxtRcv_timer_id ) {
                             /* タイマーセット */
        TxtRcv_timer_id = SetTimer ( hWnd, EB_TMRID_TXTRV, tcpCtrl.tm_txt_aak0, (TIMERPROC) TextTimeOut );
        if ( 0 == TxtRcv_timer_id ) {
            MyMessage ( hWnd, "テキスト待ちタイマー失敗" );
        } else {
            MyMessage ( hWnd, "テキスト待ちタイマー成功 LINE=%d", __LINE__ );
        }
    }

    return;
}

/***********************************************************************************
*
*  テキスト受信待ちタイムアウト
*
*  AUTHOR          : 加賀谷 大輔
*  DATE            : 2006.09.28
*
*  DEVISION 001    : ____.__.__ ( ＿＿＿：＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿
*  DEVISION 002    : ____.__.__ ( ＿＿＿：＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿
*  DEVISION 003    : ____.__.__ ( ＿＿＿：＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿
*
*  PARAMETER
*        HWND        hWnd                : ウインドウハンドル
*        UINT        uMessage            : メッセージ
*        UINT        idEvent             : タイマーID
*        DWORD       dwTime              : タイマー経過時間
*
***********************************************************************************/
void CALLBACK TextTimeOut ( HWND hWnd, UINT uMessage, UINT idEvent, DWORD dwTime ) {

    MyMessage ( hWnd, "テキスト受信待ちタイムアウト！ LINE=%d ", __LINE__ );
                             /* タイマーキャンセル */
    TextTimerCancel ( hWnd );
                             /* 伝送終了処理 */
    EnetB_DBend ( hWnd );

    return;
}

/***********************************************************************************
*
*  テキスト受信待ちタイマーキャンセル
*
*  AUTHOR          : 加賀谷 大輔
*  DATE            : 2006.09.28
*
*  DEVISION 001    : ____.__.__ ( ＿＿＿：＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿
*  DEVISION 002    : ____.__.__ ( ＿＿＿：＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿
*  DEVISION 003    : ____.__.__ ( ＿＿＿：＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿
*
*  PARAMETER
*        HWND        hWnd                : ウインドウハンドル
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
*  ＥＯＴ受信待ちタイマーセット
*
*  AUTHOR          : 加賀谷 大輔
*  DATE            : 2006.09.28
*
*  DEVISION 001    : ____.__.__ ( ＿＿＿：＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿
*  DEVISION 002    : ____.__.__ ( ＿＿＿：＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿
*  DEVISION 003    : ____.__.__ ( ＿＿＿：＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿
*
*  PARAMETER
*        HWND        hWnd               ： ウインドウハンドル
*
***********************************************************************************/
void EotTimerSet ( HWND hWnd ) {

    if ( 0 == EotRcv_timer_id ) {
                             /* タイマーセット */
        EotRcv_timer_id = SetTimer ( hWnd, EB_TMRID_EOTRV, tcpCtrl.tm_eot_aak1, (TIMERPROC) EotTimeOut );
        if ( 0 == EotRcv_timer_id ) {
            MyMessage ( hWnd, "ＥＯＴ待ちタイマー失敗" );
        } else {
            MyMessage ( hWnd, "ＥＯＴ待ちタイマー成功 LINE=%d", __LINE__ );
        }
    }

    return;
}

/***********************************************************************************
*
*  ＥＯＴ受信待ちタイムアウト
*
*  AUTHOR          : 加賀谷 大輔
*  DATE            : 2006.09.28
*
*  DEVISION 001    : ____.__.__ ( ＿＿＿：＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿
*  DEVISION 002    : ____.__.__ ( ＿＿＿：＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿
*  DEVISION 003    : ____.__.__ ( ＿＿＿：＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿
*
*  PARAMETER
*        HWND        hWnd                : ウインドウハンドル
*        UINT        uMessage            : メッセージ
*        UINT        idEvent             : タイマーID
*        DWORD       dwTime              : タイマー経過時間
*
***********************************************************************************/
void CALLBACK EotTimeOut ( HWND hWnd, UINT uMessage, UINT idEvent, DWORD dwTime ) {

    MyMessage ( hWnd, "ＥＯＴ受信待ちタイムアウト！ LINE=%d ", __LINE__ );
                             /* タイマーキャンセル */
    EotTimerCancel ( hWnd );
                             /* 伝送終了処理 */
    EnetB_DBend ( hWnd );

    return;
}

/***********************************************************************************
*
*  ＥＯＴ受信待ちタイマーキャンセル
*
*  AUTHOR          : 加賀谷 大輔
*  DATE            : 2006.09.28
*
*  DEVISION 001    : ____.__.__ ( ＿＿＿：＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿
*  DEVISION 002    : ____.__.__ ( ＿＿＿：＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿
*  DEVISION 003    : ____.__.__ ( ＿＿＿：＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿
*
*  PARAMETER
*        HWND        hWnd                : ウインドウハンドル
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
*  送信テキストバッファ・最古レコード削除
*
*  AUTHOR          : 加賀谷 大輔
*  DATE            : 2006.09.28
*
*  DEVISION 001    : ____.__.__ ( ＿＿＿：＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿
*  DEVISION 002    : ____.__.__ ( ＿＿＿：＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿
*  DEVISION 003    : ____.__.__ ( ＿＿＿：＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿
*
*  PARAMETER
*        HWND        hWnd                : ウインドウハンドル
*
***********************************************************************************/
void Ebsdbuf_oldrecdel ( HWND hWnd ) {

    if ( eb_sndbuf.out_point == eb_sndbuf.in_point ) {
        MyMessage ( hWnd, "送信バッファＥＭＰＴＹ！ LINE=%d ", __LINE__ );
    } else {
                             /* テキストバッファ初期化 */
        memset ( eb_sndbuf.snd_buff[eb_sndbuf.out_point], '\0', ENETB_MAXSIZE );
                             /* サイズバッファ初期化 */
        eb_sndbuf.snd_size[eb_sndbuf.out_point] = 0;
                             /* ＯＵＴポインタ更新 */
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
*  送信テキストバッファ・テキスト登録
*
*  AUTHOR          : 加賀谷 大輔
*  DATE            : 2006.09.29
*
*  DEVISION 001    : ____.__.__ ( ＿＿＿：＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿
*  DEVISION 002    : ____.__.__ ( ＿＿＿：＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿
*  DEVISION 003    : ____.__.__ ( ＿＿＿：＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿
*
*  PARAMETER
*        HWND        hWnd                : ウインドウハンドル
*        int         itxsiz              : テキストサイズ
*        char*       ctxstr              : テキスト文字列
*
*  return
*        0 : 正常
*       -1 : バッファＦＵＬＬ
*
***********************************************************************************/
int Ebsdbuf_recset ( HWND hWnd, int itxsiz, char* ctxstr ) {

    int  iretc;

    iretc = 0;

    if ( eb_sndbuf.out_point == eb_sndbuf.in_point + 1 ||
       ( eb_sndbuf.out_point == 0 && eb_sndbuf.in_point == ENETB_MXPOINT - 1 ) ) {
        iretc = -1;
        MyMessage ( hWnd, "送信バッファＦＵＬＬ！ LINE=%d ", __LINE__ );
    } else {
                             /* テキストバッファ登録 */
        memcpy ( eb_sndbuf.snd_buff[eb_sndbuf.in_point], ctxstr, itxsiz );
                             /* サイズバッファ登録 */
        eb_sndbuf.snd_size[eb_sndbuf.in_point] = itxsiz;
                             /* ＩＮポインタ更新 */
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
*  制御文字コードチェック
*
*  AUTHOR          : 加賀谷 大輔
*  DATE            : 2006.09.28
*
*  DEVISION 001    : ____.__.__ ( ＿＿＿：＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿
*  DEVISION 002    : ____.__.__ ( ＿＿＿：＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿
*  DEVISION 003    : ____.__.__ ( ＿＿＿：＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿
*
*  PARAMETER
*        HWND        hWnd                : ウインドウハンドル
*        char*       cdbstr              : 伝文文字列
*
*  return
*        制御文字テーブルのＩＮＤＥＸ
*
***********************************************************************************/
int Ebchk_ctrlstr ( HWND hWnd, char* cdbstr ) {

    int  iloop1;             /* ループ変数 */
    int  indxwk;             /* 制御文字配列ＩＮＤＥＸ */

                             /* 初期化 */
    indxwk = ENBCTL_IXTXT;
    if ( _ASCII == tcpCtrl.eb_mojicode ) {
                             /* ＡＳＣＩＩコード */
        for ( iloop1 = 0; iloop1 < ICTL_CHRNM; iloop1++ ) {
                             /* 制御文字チェック */
            if ( 0 == memcmp ( cdbstr, ENB_CTLASC[iloop1], ENB_CTLSIZ[iloop1] ) ) {
                indxwk = iloop1;
            }
        }
    } else {
                             /* ＥＢＣＤＩＣコード */
        for ( iloop1 = 0; iloop1 < ICTL_CHRNM; iloop1++ ) {
                             /* 制御文字チェック */
            if ( 0 == memcmp ( cdbstr, ENB_CTLEBC[iloop1], ENB_CTLSIZ[iloop1] ) ) {
                indxwk = iloop1;
            }
        }
    }

    return indxwk;
}

/***********************************************************************************
*
*  Ｅｎｅｔ−Ｂ送信開始処理
*
*  AUTHOR          : 加賀谷 大輔
*  DATE            : 2006.09.29
*
*  DEVISION 001    : ____.__.__ ( ＿＿＿：＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿
*  DEVISION 002    : ____.__.__ ( ＿＿＿：＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿
*  DEVISION 003    : ____.__.__ ( ＿＿＿：＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿
*
*  PARAMETER
*        HWND        hWnd                : ウインドウハンドル
*        int         send_port           : 送信ポート
*        int         itxsiz              : テキストサイズ
*        char*       ctxstr              : テキスト文字列
*
***********************************************************************************/
void EnetB_1stSnd ( HWND hWnd, int send_port, int itxsiz, char* ctxstr ) {

    char      c_enqstr[1];    /* ＥＮＱ文字列 */
    int       iebrtc;         /* 復帰情報 */

                              /* 送信テキストバッファ登録 */
    iebrtc = Ebsdbuf_recset ( hWnd, itxsiz, ctxstr );
                              /* バッファ登録完了＆送信中でない */
    if ( EB_NOTSND == g_eb_sndst && 0 == iebrtc ) {
                             /* ＥＮＱ再送 */
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
*  Ｅｎｅｔ−ＢＸ送信開始処理
*
*  AUTHOR          : 加賀谷 大輔
*  DATE            : 2006.09.29
*
*  DEVISION 001    : 2013.08.31 (  田中 ：透過モード時、ＤＬＥを挿入するように改造
*                                         EnetB_1stSnd を元に改造
*  DEVISION 002    : ____.__.__ ( ＿＿＿：＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿
*  DEVISION 003    : ____.__.__ ( ＿＿＿：＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿
*
*  PARAMETER
*        HWND        hWnd                : ウインドウハンドル
*        int         send_port           : 送信ポート
*        int         itxsiz              : テキストサイズ
*        char*       ctxstr              : テキスト文字列
*
***********************************************************************************/
void EnetBX_1stSnd ( HWND hWnd, int send_port, int itxsiz, char* ctxstr ) {

    char      c_enqstr[1];    /* ＥＮＱ文字列 */
    int       iebrtc;         /* 復帰情報 */

                              /* ＤＬＥの挿入・除去を行う（透過モードのみ） */
    EnetBX_Dle_ins ( hWnd, &itxsiz, ctxstr );

                              /* 送信テキストバッファ登録 */
    iebrtc = Ebsdbuf_recset ( hWnd, itxsiz, ctxstr );
                              /* バッファ登録完了＆送信中でない */
    if ( EB_NOTSND == g_eb_sndst && 0 == iebrtc ) {
                             /* ＥＮＱ再送 */
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
*  Ｅｎｅｔ−Ｂ伝送終了処理
*
*  AUTHOR          : 加賀谷 大輔
*  DATE            : 2006.10.03
*
*  DEVISION 001    : ____.__.__ ( ＿＿＿：＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿
*  DEVISION 002    : ____.__.__ ( ＿＿＿：＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿
*  DEVISION 003    : ____.__.__ ( ＿＿＿：＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿
*
*  PARAMETER
*        HWND        hWnd                : ウインドウハンドル
*
***********************************************************************************/
void EnetB_DBend ( HWND hWnd ) {

    char      c_enqstr[1];   /* ＥＮＱ文字列 */

                             /* 送信中情報初期化 */
    g_eb_sndst = EB_NOTSND;
                             /* 未送信テキスト有り？ */
    if ( eb_sndbuf.in_point != eb_sndbuf.out_point ) {
        MyMessage ( hWnd, "未送信テキスト有り！！ LINE:%d ", __LINE__ );
                             /* 開発機ポートへＥＮＱ送信 */
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
*  データに表れたＤＬＥの前にＤＬＥを挿入
*
*  AUTHOR          : 田中 晶
*  DATE            : 2013.08.21
*
*  DEVISION 001    : ____.__.__ ( ＿＿＿：＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿
*  DEVISION 002    : ____.__.__ ( ＿＿＿：＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿
*  DEVISION 003    : ____.__.__ ( ＿＿＿：＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿
*
*  PARAMETER
*        HWND        hWnd                : ウインドウハンドル
*        int         insize              : テキストサイズ
*        char*       intext              : テキスト文字列
*
***********************************************************************************/
void EnetBX_Dle_ins ( HWND hWnd, int* itxsiz, char* ctxstr ) {

    int       iloop1;         /* ループ変数 */
    int       isetpt;         /* 格納ポインタ */
                              /* 中間バッファ */
    char      ctemp[ENETB_MAXSIZE];

    isetpt = 0;
    memset( ctemp, 0x00, sizeof( ctemp ) );

    if ( EB____TRNS != tcpCtrl.eb_transpar ) {
                             /* 透過モード以外の場合は処理なし */
        return;
    }

    if ( _ASCII == tcpCtrl.eb_mojicode ) {
                             /* ＡＳＣＩＩコード */
        for ( iloop1 = 0; iloop1 < *itxsiz; iloop1++ ) {
                             /* ＤＬＥコードチェック */
            if ( 0 == memcmp ( (char*)&ctxstr[iloop1], ENB_CTLASC[ENBCTL_IXDLE], ENB_CTLSIZ[ENBCTL_IXDLE] ) ) {
                             /* ＤＬＥコードの場合は、前にＤＬＥを挿入する */
                memcpy( &ctemp[isetpt + 0], ENB_CTLASC[ENBCTL_IXDLE], 1);
                memcpy( &ctemp[isetpt + 1], (char*)&ctxstr[iloop1], 1);
                isetpt = isetpt + 2;
            } else {
                memcpy( &ctemp[isetpt], (char*)&ctxstr[iloop1], 1);
                isetpt = isetpt + 1;
            }
        }

    } else {
                             /* ＥＢＣＤＩＣコード */
        for ( iloop1 = 0; iloop1 < *itxsiz; iloop1++ ) {
                             /* ＤＬＥコードチェック */
            if ( 0 == memcmp ( (char*)&ctxstr[iloop1], ENB_CTLEBC[ENBCTL_IXDLE], ENB_CTLSIZ[ENBCTL_IXDLE] ) ) {
                             /* ＤＬＥコードの場合は、前にＤＬＥを挿入する */
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
*  データに表れたＤＬＥ１文字目はエスケープ文字として削除
*  ただし、STX／ETX は除外
*
*  AUTHOR          : 田中 晶
*  DATE            : 2013.08.21
*
*  DEVISION 001    : ____.__.__ ( ＿＿＿：＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿
*  DEVISION 002    : ____.__.__ ( ＿＿＿：＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿
*  DEVISION 003    : ____.__.__ ( ＿＿＿：＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿
*
*  PARAMETER
*        HWND        hWnd                : ウインドウハンドル
*        int         insize              : テキストサイズ
*        char*       intext              : テキスト文字列
*
***********************************************************************************/
void EnetBX_Dle_del ( HWND hWnd, int* itxsiz, char* ctxstr ) {

    int       iloop1;         /* ループ変数 */
    int       isetpt;         /* 格納ポインタ */
                              /* 中間バッファ */
    char      ctemp[ENETB_MAXSIZE];
    int       iflg_dle;       /* ＤＬＥフラグ */

    isetpt = 0;
    iflg_dle = 0;
    memset( ctemp, 0x00, sizeof( ctemp ) );

    if ( EB____TRNS != tcpCtrl.eb_transpar ) {
                             /* 透過モード以外の場合は処理なし */
        return;
    }

    if ( _ASCII == tcpCtrl.eb_mojicode ) {
                             /* ＡＳＣＩＩコード */
        for ( iloop1 = 0; iloop1 < *itxsiz; iloop1++ ) {
                             /* ＤＬＥコードチェック */
            if ( 0 == memcmp ( (char*)&ctxstr[iloop1], ENB_CTLASC[ENBCTL_IXDLE], ENB_CTLSIZ[ENBCTL_IXDLE] ) ) {
                             /* ＤＬＥコードの場合は、次のＤＬＥを削除する */
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
                             /* ＥＢＣＤＩＣコード */
        for ( iloop1 = 0; iloop1 < *itxsiz; iloop1++ ) {
                             /* ＤＬＥコードチェック */
            if ( 0 == memcmp ( (char*)&ctxstr[iloop1], ENB_CTLEBC[ENBCTL_IXDLE], ENB_CTLSIZ[ENBCTL_IXDLE] ) ) {
                             /* ＤＬＥコードの場合は、次のＤＬＥを削除する */
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
