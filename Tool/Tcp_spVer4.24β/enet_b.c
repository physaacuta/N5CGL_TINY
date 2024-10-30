/***********************************************************************************
*
*  �d�������|�a�֐��O���[�o���̈�
*
*  AUTHOR          : ����J ���
*  DATE            : 2006.09.27
*
*  DEVISION 001    : 2013.08.31 (  �c�� �F�x�[�V�b�N�菇�`���Z�~�̓��߃��[�h�ɑΉ��B
*                                         ���ݏ����ւ̉e�����l���A�]���֐��Ɋ���
*                                         ���Ȃ��悤�ɊJ���B
*  DEVISION 002    : ____.__.__ ( �Q�Q�Q�F�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q
*  DEVISION 003    : ____.__.__ ( �Q�Q�Q�F�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q
*
***********************************************************************************/

                             /* �h�m�b�k�t�c�d�錾 */
#include    <stdio.h>
#include    <windows.h>

#include    "enet_b.h"
#include    "TcpCtrl.h"
#include    "Pcterm.h"

                             /* �O���[�o���ϐ� */
UINT    EnqSnd_timer_id = 0;
UINT    Ak0Rcv_timer_id = 0;
UINT    Ak1Rcv_timer_id = 0;
UINT    TxtRcv_timer_id = 0;
UINT    EotRcv_timer_id = 0;

/***********************************************************************************
*
*  �d�������|�a�O���[�o���ϐ�����������
*
*  AUTHOR          : ����J ���
*  DATE            : 2006.10.03
*
*  DEVISION 001    : ____.__.__ ( �Q�Q�Q�F�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q
*  DEVISION 002    : ____.__.__ ( �Q�Q�Q�F�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q
*  DEVISION 003    : ____.__.__ ( �Q�Q�Q�F�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q
*
*  PARAMETER
*        HWND        hWnd               �F �E�C���h�E�n���h��
*
***********************************************************************************/
void EnetB_Gbl_init ( HWND hWnd ) {

                             /* ���M�o�b�t�@�O���[�o�� */
    eb_sndbuf.in_point = 0;
    eb_sndbuf.out_point = 0;
    memset ( eb_sndbuf.snd_size, 0x00, sizeof ( eb_sndbuf.snd_size ) );
    memset ( eb_sndbuf.snd_buff, 0x00, sizeof ( eb_sndbuf.snd_buff ) );
                             /* ���M����ԃO���[�o�� */
    g_eb_sndst = EB_NOTSND;
                             /* ���g���C�J�E���^�O���[�o�� */
    g_eb_cntenq = 0;
    g_eb_cnttxt = 0;
                             /* ��M�o�b�t�@�O���[�o�� */
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
*  �d�m�p�đ��N���^�C�}�[�Z�b�g
*
*  AUTHOR          : ����J ���
*  DATE            : 2006.09.27
*
*  DEVISION 001    : ____.__.__ ( �Q�Q�Q�F�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q
*  DEVISION 002    : ____.__.__ ( �Q�Q�Q�F�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q
*  DEVISION 003    : ____.__.__ ( �Q�Q�Q�F�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q
*
*  PARAMETER
*        HWND        hWnd               �F �E�C���h�E�n���h��
*
***********************************************************************************/
void EnqTimerSet ( HWND hWnd ) {

    if ( 0 == EnqSnd_timer_id ) {
                             /* �^�C�}�[�Z�b�g */
        EnqSnd_timer_id = SetTimer ( hWnd, EB_TMRID_ENQSD, tcpCtrl.tm_enq_anak, (TIMERPROC) EnqTimeOut );
        if ( 0 == EnqSnd_timer_id ) {
            MyMessage ( hWnd, "�d�m�p�đ��^�C�}�[�N�����s" );
        } else {
            MyMessage ( hWnd, "�d�m�p�đ��^�C�}�[�N������ LINE=%d", __LINE__ );
        }
    }

    return;
}

/***********************************************************************************
*
*  �d�m�p�đ��^�C�}�[�N��
*
*  AUTHOR          : ����J ���
*  DATE            : 2006.09.27
*
*  DEVISION 001    : ____.__.__ ( �Q�Q�Q�F�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q
*  DEVISION 002    : ____.__.__ ( �Q�Q�Q�F�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q
*  DEVISION 003    : ____.__.__ ( �Q�Q�Q�F�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q
*
*  PARAMETER
*        HWND        hWnd                : �E�C���h�E�n���h��
*        UINT        uMessage            : ���b�Z�[�W
*        UINT        idEvent             : �^�C�}�[ID
*        DWORD       dwTime              : �^�C�}�[�o�ߎ���
*
***********************************************************************************/
void CALLBACK EnqTimeOut ( HWND hWnd, UINT uMessage, UINT idEvent, DWORD dwTime ) {

    char      c_enqstr[1];             /* �d�m�p������ */

    MyMessage ( hWnd, "�d�m�p�đ��^�C���A�E�g�I LINE=%d ", __LINE__ );
                             /* �^�C�}�[�L�����Z�� */
    EnqTimerCancel ( hWnd );
                             /* ���M�����X�V */
    g_eb_sndst = EB_NOTSND;
                             /* ���g���C���M�H */
    if ( g_eb_cntenq < tcpCtrl.nm_enq_nans ) {
        MyMessage ( hWnd, "�đ��J�n�I LINE=%d ", __LINE__ );
                             /* �d�m�p�đ� */
        if ( _ASCII == tcpCtrl.eb_mojicode ) {
            memcpy ( c_enqstr, ENB_CTLASC[ENBCTL_IXENQ], sizeof ( c_enqstr ) );
        } else {
            memcpy ( c_enqstr, ENB_CTLEBC[ENBCTL_IXENQ], sizeof ( c_enqstr ) );
        }
        SetSndMsg ( hWnd, sizeof ( c_enqstr ), c_enqstr );

        g_eb_cntenq = g_eb_cntenq + 1;

    } else {
        MyMessage ( hWnd, "�d�m�p���g���C�񐔃I�[�o�[�I LINE=%d ", __LINE__ );
                             /* ���M�e�L�X�g�o�b�t�@�ŌÃ��R�[�h�폜 */
        Ebsdbuf_oldrecdel ( hWnd );
                             /* ���g���C�J�E���^������ */
        g_eb_cntenq = 0;
                             /* �`���I������ */
        EnetB_DBend ( hWnd );
    }

    return;
}

/***********************************************************************************
*
*  �d�m�p�đ��^�C�}�[�L�����Z��
*
*  AUTHOR          : ����J ���
*  DATE            : 2006.09.28
*
*  DEVISION 001    : ____.__.__ ( �Q�Q�Q�F�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q
*  DEVISION 002    : ____.__.__ ( �Q�Q�Q�F�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q
*  DEVISION 003    : ____.__.__ ( �Q�Q�Q�F�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q
*
*  PARAMETER
*        HWND        hWnd                : �E�C���h�E�n���h��
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
*  �`�b�j�O��M�҂��^�C�}�[�Z�b�g
*
*  AUTHOR          : ����J ���
*  DATE            : 2006.09.28
*
*  DEVISION 001    : ____.__.__ ( �Q�Q�Q�F�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q
*  DEVISION 002    : ____.__.__ ( �Q�Q�Q�F�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q
*  DEVISION 003    : ____.__.__ ( �Q�Q�Q�F�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q
*
*  PARAMETER
*        HWND        hWnd               �F �E�C���h�E�n���h��
*
***********************************************************************************/
void Ack0TimerSet ( HWND hWnd ) {

    if ( 0 == Ak0Rcv_timer_id ) {
                             /* �^�C�}�[�Z�b�g */
        Ak0Rcv_timer_id = SetTimer ( hWnd, EB_TMRID_AK0RV, tcpCtrl.tm_ak0_aenq, (TIMERPROC) Ack0TimeOut );
        if ( 0 == Ak0Rcv_timer_id ) {
            MyMessage ( hWnd, "�`�b�j�O�҂��^�C�}�[���s" );
        } else {
            MyMessage ( hWnd, "�`�b�j�O�҂��^�C�}�[���� LINE=%d", __LINE__ );
        }
    }

    return;
}

/***********************************************************************************
*
*  �`�b�j�O��M�҂��^�C���A�E�g
*
*  AUTHOR          : ����J ���
*  DATE            : 2006.09.28
*
*  DEVISION 001    : ____.__.__ ( �Q�Q�Q�F�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q
*  DEVISION 002    : ____.__.__ ( �Q�Q�Q�F�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q
*  DEVISION 003    : ____.__.__ ( �Q�Q�Q�F�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q
*
*  PARAMETER
*        HWND        hWnd                : �E�C���h�E�n���h��
*        UINT        uMessage            : ���b�Z�[�W
*        UINT        idEvent             : �^�C�}�[ID
*        DWORD       dwTime              : �^�C�}�[�o�ߎ���
*
***********************************************************************************/
void CALLBACK Ack0TimeOut ( HWND hWnd, UINT uMessage, UINT idEvent, DWORD dwTime ) {

    char      c_enqstr[1];             /* �d�m�p������ */

    MyMessage ( hWnd, "�`�b�j�O��M�҂��^�C���A�E�g�I LINE=%d ", __LINE__ );
                             /* �^�C�}�[�L�����Z�� */
    Ack0TimerCancel ( hWnd );
                             /* ���M����񏉊��� */
    g_eb_sndst = EB_NOTSND;
                             /* ���g���C���M�H */
    if ( g_eb_cntenq < tcpCtrl.nm_enq_nans ) {
                             /* �d�m�p�đ� */
        if ( _ASCII == tcpCtrl.eb_mojicode ) {
            memcpy ( c_enqstr, ENB_CTLASC[ENBCTL_IXENQ], sizeof ( c_enqstr ) );
        } else {
            memcpy ( c_enqstr, ENB_CTLEBC[ENBCTL_IXENQ], sizeof ( c_enqstr ) );
        }
        SetSndMsg ( hWnd, sizeof ( c_enqstr ), c_enqstr );
                             /* �đ��񐔍X�V */
        g_eb_cntenq = g_eb_cntenq + 1;
    } else {
        MyMessage ( hWnd, "�d�m�p���g���C�񐔃I�[�o�[�I LINE=%d ", __LINE__ );
                             /* ���M�e�L�X�g�o�b�t�@�ŌÃ��R�[�h�폜 */
        Ebsdbuf_oldrecdel ( hWnd );
                             /* ���g���C�J�E���^������ */
        g_eb_cntenq = 0;
                             /* �`���I������ */
        EnetB_DBend ( hWnd );
    }

    return;
}

/***********************************************************************************
*
*  �`�b�j�O��M�҂��^�C�}�[�L�����Z��
*
*  AUTHOR          : ����J ���
*  DATE            : 2006.09.28
*
*  DEVISION 001    : ____.__.__ ( �Q�Q�Q�F�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q
*  DEVISION 002    : ____.__.__ ( �Q�Q�Q�F�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q
*  DEVISION 003    : ____.__.__ ( �Q�Q�Q�F�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q
*
*  PARAMETER
*        HWND        hWnd                : �E�C���h�E�n���h��
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
*  �`�b�j�P��M�҂��^�C�}�[�Z�b�g
*
*  AUTHOR          : ����J ���
*  DATE            : 2006.09.28
*
*  DEVISION 001    : ____.__.__ ( �Q�Q�Q�F�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q
*  DEVISION 002    : ____.__.__ ( �Q�Q�Q�F�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q
*  DEVISION 003    : ____.__.__ ( �Q�Q�Q�F�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q
*
*  PARAMETER
*        HWND        hWnd               �F �E�C���h�E�n���h��
*
***********************************************************************************/
void Ack1TimerSet ( HWND hWnd ) {

    if ( 0 == Ak1Rcv_timer_id ) {
                             /* �^�C�}�[�Z�b�g */
        Ak1Rcv_timer_id = SetTimer ( hWnd, EB_TMRID_AK1RV, tcpCtrl.tm_ak1_atxt, (TIMERPROC) Ack1TimeOut );
        if ( 0 == Ak1Rcv_timer_id ) {
            MyMessage ( hWnd, "�`�b�j�P�҂��^�C�}�[���s" );
        } else {
            MyMessage ( hWnd, "�`�b�j�P�҂��^�C�}�[���� LINE=%d", __LINE__ );
        }
    }

    return;
}

/***********************************************************************************
*
*  �`�b�j�P��M�҂��^�C���A�E�g
*
*  AUTHOR          : ����J ���
*  DATE            : 2006.09.28
*
*  DEVISION 001    : ____.__.__ ( �Q�Q�Q�F�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q
*  DEVISION 002    : ____.__.__ ( �Q�Q�Q�F�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q
*  DEVISION 003    : ____.__.__ ( �Q�Q�Q�F�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q
*
*  PARAMETER
*        HWND        hWnd                : �E�C���h�E�n���h��
*        UINT        uMessage            : ���b�Z�[�W
*        UINT        idEvent             : �^�C�}�[ID
*        DWORD       dwTime              : �^�C�}�[�o�ߎ���
*
***********************************************************************************/
void CALLBACK Ack1TimeOut ( HWND hWnd, UINT uMessage, UINT idEvent, DWORD dwTime ) {

    MyMessage ( hWnd, "�`�b�j�P��M�҂��^�C���A�E�g�I LINE=%d ", __LINE__ );
                             /* �^�C�}�[�L�����Z�� */
    Ack1TimerCancel ( hWnd );
                             /* ���M�e�L�X�g�o�b�t�@�ŌÃ��R�[�h�폜 */
    Ebsdbuf_oldrecdel ( hWnd );
                             /* ���g���C�J�E���^������ */
    g_eb_cnttxt = 0;
                             /* �`���I������ */
    EnetB_DBend ( hWnd );

    return;
}

/***********************************************************************************
*
*  �`�b�j�P��M�҂��^�C�}�[�L�����Z��
*
*  AUTHOR          : ����J ���
*  DATE            : 2006.09.28
*
*  DEVISION 001    : ____.__.__ ( �Q�Q�Q�F�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q
*  DEVISION 002    : ____.__.__ ( �Q�Q�Q�F�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q
*  DEVISION 003    : ____.__.__ ( �Q�Q�Q�F�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q
*
*  PARAMETER
*        HWND        hWnd                : �E�C���h�E�n���h��
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
*  �e�L�X�g��M�҂��^�C�}�[�Z�b�g
*
*  AUTHOR          : ����J ���
*  DATE            : 2006.09.28
*
*  DEVISION 001    : ____.__.__ ( �Q�Q�Q�F�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q
*  DEVISION 002    : ____.__.__ ( �Q�Q�Q�F�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q
*  DEVISION 003    : ____.__.__ ( �Q�Q�Q�F�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q
*
*  PARAMETER
*        HWND        hWnd               �F �E�C���h�E�n���h��
*
***********************************************************************************/
void TextTimerSet ( HWND hWnd ) {

    if ( 0 == TxtRcv_timer_id ) {
                             /* �^�C�}�[�Z�b�g */
        TxtRcv_timer_id = SetTimer ( hWnd, EB_TMRID_TXTRV, tcpCtrl.tm_txt_aak0, (TIMERPROC) TextTimeOut );
        if ( 0 == TxtRcv_timer_id ) {
            MyMessage ( hWnd, "�e�L�X�g�҂��^�C�}�[���s" );
        } else {
            MyMessage ( hWnd, "�e�L�X�g�҂��^�C�}�[���� LINE=%d", __LINE__ );
        }
    }

    return;
}

/***********************************************************************************
*
*  �e�L�X�g��M�҂��^�C���A�E�g
*
*  AUTHOR          : ����J ���
*  DATE            : 2006.09.28
*
*  DEVISION 001    : ____.__.__ ( �Q�Q�Q�F�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q
*  DEVISION 002    : ____.__.__ ( �Q�Q�Q�F�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q
*  DEVISION 003    : ____.__.__ ( �Q�Q�Q�F�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q
*
*  PARAMETER
*        HWND        hWnd                : �E�C���h�E�n���h��
*        UINT        uMessage            : ���b�Z�[�W
*        UINT        idEvent             : �^�C�}�[ID
*        DWORD       dwTime              : �^�C�}�[�o�ߎ���
*
***********************************************************************************/
void CALLBACK TextTimeOut ( HWND hWnd, UINT uMessage, UINT idEvent, DWORD dwTime ) {

    MyMessage ( hWnd, "�e�L�X�g��M�҂��^�C���A�E�g�I LINE=%d ", __LINE__ );
                             /* �^�C�}�[�L�����Z�� */
    TextTimerCancel ( hWnd );
                             /* �`���I������ */
    EnetB_DBend ( hWnd );

    return;
}

/***********************************************************************************
*
*  �e�L�X�g��M�҂��^�C�}�[�L�����Z��
*
*  AUTHOR          : ����J ���
*  DATE            : 2006.09.28
*
*  DEVISION 001    : ____.__.__ ( �Q�Q�Q�F�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q
*  DEVISION 002    : ____.__.__ ( �Q�Q�Q�F�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q
*  DEVISION 003    : ____.__.__ ( �Q�Q�Q�F�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q
*
*  PARAMETER
*        HWND        hWnd                : �E�C���h�E�n���h��
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
*  �d�n�s��M�҂��^�C�}�[�Z�b�g
*
*  AUTHOR          : ����J ���
*  DATE            : 2006.09.28
*
*  DEVISION 001    : ____.__.__ ( �Q�Q�Q�F�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q
*  DEVISION 002    : ____.__.__ ( �Q�Q�Q�F�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q
*  DEVISION 003    : ____.__.__ ( �Q�Q�Q�F�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q
*
*  PARAMETER
*        HWND        hWnd               �F �E�C���h�E�n���h��
*
***********************************************************************************/
void EotTimerSet ( HWND hWnd ) {

    if ( 0 == EotRcv_timer_id ) {
                             /* �^�C�}�[�Z�b�g */
        EotRcv_timer_id = SetTimer ( hWnd, EB_TMRID_EOTRV, tcpCtrl.tm_eot_aak1, (TIMERPROC) EotTimeOut );
        if ( 0 == EotRcv_timer_id ) {
            MyMessage ( hWnd, "�d�n�s�҂��^�C�}�[���s" );
        } else {
            MyMessage ( hWnd, "�d�n�s�҂��^�C�}�[���� LINE=%d", __LINE__ );
        }
    }

    return;
}

/***********************************************************************************
*
*  �d�n�s��M�҂��^�C���A�E�g
*
*  AUTHOR          : ����J ���
*  DATE            : 2006.09.28
*
*  DEVISION 001    : ____.__.__ ( �Q�Q�Q�F�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q
*  DEVISION 002    : ____.__.__ ( �Q�Q�Q�F�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q
*  DEVISION 003    : ____.__.__ ( �Q�Q�Q�F�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q
*
*  PARAMETER
*        HWND        hWnd                : �E�C���h�E�n���h��
*        UINT        uMessage            : ���b�Z�[�W
*        UINT        idEvent             : �^�C�}�[ID
*        DWORD       dwTime              : �^�C�}�[�o�ߎ���
*
***********************************************************************************/
void CALLBACK EotTimeOut ( HWND hWnd, UINT uMessage, UINT idEvent, DWORD dwTime ) {

    MyMessage ( hWnd, "�d�n�s��M�҂��^�C���A�E�g�I LINE=%d ", __LINE__ );
                             /* �^�C�}�[�L�����Z�� */
    EotTimerCancel ( hWnd );
                             /* �`���I������ */
    EnetB_DBend ( hWnd );

    return;
}

/***********************************************************************************
*
*  �d�n�s��M�҂��^�C�}�[�L�����Z��
*
*  AUTHOR          : ����J ���
*  DATE            : 2006.09.28
*
*  DEVISION 001    : ____.__.__ ( �Q�Q�Q�F�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q
*  DEVISION 002    : ____.__.__ ( �Q�Q�Q�F�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q
*  DEVISION 003    : ____.__.__ ( �Q�Q�Q�F�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q
*
*  PARAMETER
*        HWND        hWnd                : �E�C���h�E�n���h��
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
*  ���M�e�L�X�g�o�b�t�@�E�ŌÃ��R�[�h�폜
*
*  AUTHOR          : ����J ���
*  DATE            : 2006.09.28
*
*  DEVISION 001    : ____.__.__ ( �Q�Q�Q�F�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q
*  DEVISION 002    : ____.__.__ ( �Q�Q�Q�F�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q
*  DEVISION 003    : ____.__.__ ( �Q�Q�Q�F�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q
*
*  PARAMETER
*        HWND        hWnd                : �E�C���h�E�n���h��
*
***********************************************************************************/
void Ebsdbuf_oldrecdel ( HWND hWnd ) {

    if ( eb_sndbuf.out_point == eb_sndbuf.in_point ) {
        MyMessage ( hWnd, "���M�o�b�t�@�d�l�o�s�x�I LINE=%d ", __LINE__ );
    } else {
                             /* �e�L�X�g�o�b�t�@������ */
        memset ( eb_sndbuf.snd_buff[eb_sndbuf.out_point], '\0', ENETB_MAXSIZE );
                             /* �T�C�Y�o�b�t�@������ */
        eb_sndbuf.snd_size[eb_sndbuf.out_point] = 0;
                             /* �n�t�s�|�C���^�X�V */
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
*  ���M�e�L�X�g�o�b�t�@�E�e�L�X�g�o�^
*
*  AUTHOR          : ����J ���
*  DATE            : 2006.09.29
*
*  DEVISION 001    : ____.__.__ ( �Q�Q�Q�F�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q
*  DEVISION 002    : ____.__.__ ( �Q�Q�Q�F�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q
*  DEVISION 003    : ____.__.__ ( �Q�Q�Q�F�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q
*
*  PARAMETER
*        HWND        hWnd                : �E�C���h�E�n���h��
*        int         itxsiz              : �e�L�X�g�T�C�Y
*        char*       ctxstr              : �e�L�X�g������
*
*  return
*        0 : ����
*       -1 : �o�b�t�@�e�t�k�k
*
***********************************************************************************/
int Ebsdbuf_recset ( HWND hWnd, int itxsiz, char* ctxstr ) {

    int  iretc;

    iretc = 0;

    if ( eb_sndbuf.out_point == eb_sndbuf.in_point + 1 ||
       ( eb_sndbuf.out_point == 0 && eb_sndbuf.in_point == ENETB_MXPOINT - 1 ) ) {
        iretc = -1;
        MyMessage ( hWnd, "���M�o�b�t�@�e�t�k�k�I LINE=%d ", __LINE__ );
    } else {
                             /* �e�L�X�g�o�b�t�@�o�^ */
        memcpy ( eb_sndbuf.snd_buff[eb_sndbuf.in_point], ctxstr, itxsiz );
                             /* �T�C�Y�o�b�t�@�o�^ */
        eb_sndbuf.snd_size[eb_sndbuf.in_point] = itxsiz;
                             /* �h�m�|�C���^�X�V */
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
*  ���䕶���R�[�h�`�F�b�N
*
*  AUTHOR          : ����J ���
*  DATE            : 2006.09.28
*
*  DEVISION 001    : ____.__.__ ( �Q�Q�Q�F�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q
*  DEVISION 002    : ____.__.__ ( �Q�Q�Q�F�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q
*  DEVISION 003    : ____.__.__ ( �Q�Q�Q�F�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q
*
*  PARAMETER
*        HWND        hWnd                : �E�C���h�E�n���h��
*        char*       cdbstr              : �`��������
*
*  return
*        ���䕶���e�[�u���̂h�m�c�d�w
*
***********************************************************************************/
int Ebchk_ctrlstr ( HWND hWnd, char* cdbstr ) {

    int  iloop1;             /* ���[�v�ϐ� */
    int  indxwk;             /* ���䕶���z��h�m�c�d�w */

                             /* ������ */
    indxwk = ENBCTL_IXTXT;
    if ( _ASCII == tcpCtrl.eb_mojicode ) {
                             /* �`�r�b�h�h�R�[�h */
        for ( iloop1 = 0; iloop1 < ICTL_CHRNM; iloop1++ ) {
                             /* ���䕶���`�F�b�N */
            if ( 0 == memcmp ( cdbstr, ENB_CTLASC[iloop1], ENB_CTLSIZ[iloop1] ) ) {
                indxwk = iloop1;
            }
        }
    } else {
                             /* �d�a�b�c�h�b�R�[�h */
        for ( iloop1 = 0; iloop1 < ICTL_CHRNM; iloop1++ ) {
                             /* ���䕶���`�F�b�N */
            if ( 0 == memcmp ( cdbstr, ENB_CTLEBC[iloop1], ENB_CTLSIZ[iloop1] ) ) {
                indxwk = iloop1;
            }
        }
    }

    return indxwk;
}

/***********************************************************************************
*
*  �d�������|�a���M�J�n����
*
*  AUTHOR          : ����J ���
*  DATE            : 2006.09.29
*
*  DEVISION 001    : ____.__.__ ( �Q�Q�Q�F�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q
*  DEVISION 002    : ____.__.__ ( �Q�Q�Q�F�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q
*  DEVISION 003    : ____.__.__ ( �Q�Q�Q�F�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q
*
*  PARAMETER
*        HWND        hWnd                : �E�C���h�E�n���h��
*        int         send_port           : ���M�|�[�g
*        int         itxsiz              : �e�L�X�g�T�C�Y
*        char*       ctxstr              : �e�L�X�g������
*
***********************************************************************************/
void EnetB_1stSnd ( HWND hWnd, int send_port, int itxsiz, char* ctxstr ) {

    char      c_enqstr[1];    /* �d�m�p������ */
    int       iebrtc;         /* ���A��� */

                              /* ���M�e�L�X�g�o�b�t�@�o�^ */
    iebrtc = Ebsdbuf_recset ( hWnd, itxsiz, ctxstr );
                              /* �o�b�t�@�o�^���������M���łȂ� */
    if ( EB_NOTSND == g_eb_sndst && 0 == iebrtc ) {
                             /* �d�m�p�đ� */
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
*  �d�������|�a�w���M�J�n����
*
*  AUTHOR          : ����J ���
*  DATE            : 2006.09.29
*
*  DEVISION 001    : 2013.08.31 (  �c�� �F���߃��[�h���A�c�k�d��}������悤�ɉ���
*                                         EnetB_1stSnd �����ɉ���
*  DEVISION 002    : ____.__.__ ( �Q�Q�Q�F�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q
*  DEVISION 003    : ____.__.__ ( �Q�Q�Q�F�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q
*
*  PARAMETER
*        HWND        hWnd                : �E�C���h�E�n���h��
*        int         send_port           : ���M�|�[�g
*        int         itxsiz              : �e�L�X�g�T�C�Y
*        char*       ctxstr              : �e�L�X�g������
*
***********************************************************************************/
void EnetBX_1stSnd ( HWND hWnd, int send_port, int itxsiz, char* ctxstr ) {

    char      c_enqstr[1];    /* �d�m�p������ */
    int       iebrtc;         /* ���A��� */

                              /* �c�k�d�̑}���E�������s���i���߃��[�h�̂݁j */
    EnetBX_Dle_ins ( hWnd, &itxsiz, ctxstr );

                              /* ���M�e�L�X�g�o�b�t�@�o�^ */
    iebrtc = Ebsdbuf_recset ( hWnd, itxsiz, ctxstr );
                              /* �o�b�t�@�o�^���������M���łȂ� */
    if ( EB_NOTSND == g_eb_sndst && 0 == iebrtc ) {
                             /* �d�m�p�đ� */
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
*  �d�������|�a�`���I������
*
*  AUTHOR          : ����J ���
*  DATE            : 2006.10.03
*
*  DEVISION 001    : ____.__.__ ( �Q�Q�Q�F�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q
*  DEVISION 002    : ____.__.__ ( �Q�Q�Q�F�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q
*  DEVISION 003    : ____.__.__ ( �Q�Q�Q�F�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q
*
*  PARAMETER
*        HWND        hWnd                : �E�C���h�E�n���h��
*
***********************************************************************************/
void EnetB_DBend ( HWND hWnd ) {

    char      c_enqstr[1];   /* �d�m�p������ */

                             /* ���M����񏉊��� */
    g_eb_sndst = EB_NOTSND;
                             /* �����M�e�L�X�g�L��H */
    if ( eb_sndbuf.in_point != eb_sndbuf.out_point ) {
        MyMessage ( hWnd, "�����M�e�L�X�g�L��I�I LINE:%d ", __LINE__ );
                             /* �J���@�|�[�g�ւd�m�p���M */
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
*  �f�[�^�ɕ\�ꂽ�c�k�d�̑O�ɂc�k�d��}��
*
*  AUTHOR          : �c�� ��
*  DATE            : 2013.08.21
*
*  DEVISION 001    : ____.__.__ ( �Q�Q�Q�F�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q
*  DEVISION 002    : ____.__.__ ( �Q�Q�Q�F�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q
*  DEVISION 003    : ____.__.__ ( �Q�Q�Q�F�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q
*
*  PARAMETER
*        HWND        hWnd                : �E�C���h�E�n���h��
*        int         insize              : �e�L�X�g�T�C�Y
*        char*       intext              : �e�L�X�g������
*
***********************************************************************************/
void EnetBX_Dle_ins ( HWND hWnd, int* itxsiz, char* ctxstr ) {

    int       iloop1;         /* ���[�v�ϐ� */
    int       isetpt;         /* �i�[�|�C���^ */
                              /* ���ԃo�b�t�@ */
    char      ctemp[ENETB_MAXSIZE];

    isetpt = 0;
    memset( ctemp, 0x00, sizeof( ctemp ) );

    if ( EB____TRNS != tcpCtrl.eb_transpar ) {
                             /* ���߃��[�h�ȊO�̏ꍇ�͏����Ȃ� */
        return;
    }

    if ( _ASCII == tcpCtrl.eb_mojicode ) {
                             /* �`�r�b�h�h�R�[�h */
        for ( iloop1 = 0; iloop1 < *itxsiz; iloop1++ ) {
                             /* �c�k�d�R�[�h�`�F�b�N */
            if ( 0 == memcmp ( (char*)&ctxstr[iloop1], ENB_CTLASC[ENBCTL_IXDLE], ENB_CTLSIZ[ENBCTL_IXDLE] ) ) {
                             /* �c�k�d�R�[�h�̏ꍇ�́A�O�ɂc�k�d��}������ */
                memcpy( &ctemp[isetpt + 0], ENB_CTLASC[ENBCTL_IXDLE], 1);
                memcpy( &ctemp[isetpt + 1], (char*)&ctxstr[iloop1], 1);
                isetpt = isetpt + 2;
            } else {
                memcpy( &ctemp[isetpt], (char*)&ctxstr[iloop1], 1);
                isetpt = isetpt + 1;
            }
        }

    } else {
                             /* �d�a�b�c�h�b�R�[�h */
        for ( iloop1 = 0; iloop1 < *itxsiz; iloop1++ ) {
                             /* �c�k�d�R�[�h�`�F�b�N */
            if ( 0 == memcmp ( (char*)&ctxstr[iloop1], ENB_CTLEBC[ENBCTL_IXDLE], ENB_CTLSIZ[ENBCTL_IXDLE] ) ) {
                             /* �c�k�d�R�[�h�̏ꍇ�́A�O�ɂc�k�d��}������ */
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
*  �f�[�^�ɕ\�ꂽ�c�k�d�P�����ڂ̓G�X�P�[�v�����Ƃ��č폜
*  �������ASTX�^ETX �͏��O
*
*  AUTHOR          : �c�� ��
*  DATE            : 2013.08.21
*
*  DEVISION 001    : ____.__.__ ( �Q�Q�Q�F�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q
*  DEVISION 002    : ____.__.__ ( �Q�Q�Q�F�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q
*  DEVISION 003    : ____.__.__ ( �Q�Q�Q�F�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q
*
*  PARAMETER
*        HWND        hWnd                : �E�C���h�E�n���h��
*        int         insize              : �e�L�X�g�T�C�Y
*        char*       intext              : �e�L�X�g������
*
***********************************************************************************/
void EnetBX_Dle_del ( HWND hWnd, int* itxsiz, char* ctxstr ) {

    int       iloop1;         /* ���[�v�ϐ� */
    int       isetpt;         /* �i�[�|�C���^ */
                              /* ���ԃo�b�t�@ */
    char      ctemp[ENETB_MAXSIZE];
    int       iflg_dle;       /* �c�k�d�t���O */

    isetpt = 0;
    iflg_dle = 0;
    memset( ctemp, 0x00, sizeof( ctemp ) );

    if ( EB____TRNS != tcpCtrl.eb_transpar ) {
                             /* ���߃��[�h�ȊO�̏ꍇ�͏����Ȃ� */
        return;
    }

    if ( _ASCII == tcpCtrl.eb_mojicode ) {
                             /* �`�r�b�h�h�R�[�h */
        for ( iloop1 = 0; iloop1 < *itxsiz; iloop1++ ) {
                             /* �c�k�d�R�[�h�`�F�b�N */
            if ( 0 == memcmp ( (char*)&ctxstr[iloop1], ENB_CTLASC[ENBCTL_IXDLE], ENB_CTLSIZ[ENBCTL_IXDLE] ) ) {
                             /* �c�k�d�R�[�h�̏ꍇ�́A���̂c�k�d���폜���� */
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
                             /* �d�a�b�c�h�b�R�[�h */
        for ( iloop1 = 0; iloop1 < *itxsiz; iloop1++ ) {
                             /* �c�k�d�R�[�h�`�F�b�N */
            if ( 0 == memcmp ( (char*)&ctxstr[iloop1], ENB_CTLEBC[ENBCTL_IXDLE], ENB_CTLSIZ[ENBCTL_IXDLE] ) ) {
                             /* �c�k�d�R�[�h�̏ꍇ�́A���̂c�k�d���폜���� */
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
