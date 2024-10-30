/***********************************************************************************
*
*  �d�������|�a�w�b�_�[
*
*  AUTHOR          : ����J ���
*  DATE            : 2006.09.28(�ҍ����c�[��)
*                    2010.07.14(TcpSp ����)
*
*  DEVISION 001    : ____.__.__ ( �Q�Q�Q�F�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q
*  DEVISION 002    : ____.__.__ ( �Q�Q�Q�F�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q
*  DEVISION 003    : ____.__.__ ( �Q�Q�Q�F�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q
*
***********************************************************************************/
#ifndef _ENET_B_H
#define _ENET_B_H


/**********************************************************************************/
/*       �C���N���[�h�錾                                                         */
/**********************************************************************************/

#include      "dump.h"
#include      "TcpCtrl.h"

/**********************************************************************************/
/*       ���M���e�L�X�g�i�[�o�b�t�@                                               */
/**********************************************************************************/

#define            ENETB_MAXSIZE    1400    /* �`���ő�T�C�Y */
#define            ENETB_MXPOINT      20    /* �o�b�t�@���R�[�h�� */

typedef struct {
    int     in_point;                                 /* �h�m�|�C���^ */
    int     out_point;                                /* �n�t�s�|�C���^ */
    int     snd_size[ENETB_MXPOINT];                  /* ���M�`���T�C�Y */
    char    snd_buff[ENETB_MXPOINT][ENETB_MAXSIZE];   /* ���M���o�b�t�@ */
} EnetbSndBuf;

EnetbSndBuf        eb_sndbuf;               /* �o�b�t�@�O���[�o���z�� */

/**********************************************************************************/
/*       ��M���e�L�X�g�i�[�o�b�t�@                                               */
/**********************************************************************************/

char               g_eb_rcvbuf1[ENETB_MAXSIZE];       /* �|�[�g�P��M���o�b�t�@ */
char               g_eb_rcvkey1[FNAME_MAX];           /* �|�[�g�P��M���L�[�R�[�h */
int                g_eb_rcvsiz1;                      /* �|�[�g�P��M���T�C�Y */
char               g_eb_rcvbuf3[ENETB_MAXSIZE];       /* �|�[�g�R��M���o�b�t�@ */
char               g_eb_rcvkey3[FNAME_MAX];           /* �|�[�g�R��M���L�[�R�[�h */
int                g_eb_rcvsiz3;                      /* �|�[�g�R��M���T�C�Y */

/**********************************************************************************/
/*       ���M�����                                                               */
/**********************************************************************************/

int                g_eb_sndst;              /* ���M���O���[�o���ϐ� */

#define            EB_NOTSND           0    /* ���M���łȂ� */
#define            EB_ENQSND           1    /* �d�m�p���M�� */
#define            EB_TXTSND           2    /* �e�L�X�g���M�� */
#define            EB_AK0SND           3    /* �`�b�j�O���M�� */
#define            EB_AK1SND           4    /* �`�b�j�P���M�� */

/**********************************************************************************/
/*       ���g���C�J�E���^                                                         */
/**********************************************************************************/

int                g_eb_cntenq;             /* �d�m�p���M���g���C�J�E���^ */
int                g_eb_cnttxt;             /* �e�L�X�g���M���g���C�J�E���^ */

/**********************************************************************************/
/*       ���߁^�񓧉ߒ萔�iTcpSp ���Łj                                           */
/**********************************************************************************/

#define            EB_NOTTRNS          0    /* �񓧉� */
#define            EB____TRNS          1    /* ���� */

/**********************************************************************************/
/*       �a�b�r�d�l�h��`�����l                                                   */
/**********************************************************************************/

#define            INIT_MJCOD    _EBCDIC    /* �������� */
#define            INIT_TMAK1       3000    /* ÷�đ��M��ACK1�܂ł̎��� */
#define            INIT_TMAK0       3000    /* ENQ���M��ACK0�܂ł̎��� */
#define            INIT_TMTXT       8000    /* ACK0���M��÷�Ď�M�܂ł̎��� */
#define            INIT_TMEOT       4000    /* ACK1���M��EOT�܂ł̎��� */
#define            INIT_TMENQ       1000    /* ACK0�҂���NAK��M����ENQ�đ��܂ł̎��� */
#define            INIT_NMENQ          3    /* �������ɑ΂���ENQ���M�� */
#define            INIT_NMTXT          3    /* ÷�đ��M��NAK�ɑ΂���÷�đ��M�� */

#define            INIT_TRNSP EB_NOTTRNS    /* �iTcpSp ���Łj���߁^�񓧉� */

/**********************************************************************************/
/*       �a�b�r�d�l�h�㉺���l�`�F�b�N                                             */
/**********************************************************************************/

#define            IBCDMN_TIM          1    /* ���ԃf�[�^�l�h�m�l */
#define            IBCDMX_TIM      99999    /* ���ԃf�[�^�l�`�w�l */
#define            IBCLMX_TIM          5    /* ���ԕ������l�`�w�l */
#define            IBCDMN_NUM          1    /* �񐔃f�[�^�l�h�m�l */
#define            IBCDMX_NUM         99    /* �񐔃f�[�^�l�`�w�l */
#define            IBCLMX_NUM          2    /* �񐔕������l�`�w�l */

/**********************************************************************************/
/*       �`�����䕶���E�����R�[�h                                                 */
/**********************************************************************************/

#define            ICTL_CHRNM        16     /* ���䕶����     */
#define            ICTL_CHRSZ         2     /* ���䕶���T�C�Y */

                                            /* ���䕶���R�[�h�i�d�a�b�c�h�b�j*/
static const char  ENB_CTLEBC[ICTL_CHRNM][ICTL_CHRSZ] = {
                           ( char ) 0x32, ( char ) 0x00,   /* �r�x�m   */
                           ( char ) 0x01, ( char ) 0x00,   /* �r�n�g   */
                           ( char ) 0x02, ( char ) 0x00,   /* �r�s�w   */
                           ( char ) 0x26, ( char ) 0x00,   /* �d�s�a   */
                           ( char ) 0x1F, ( char ) 0x00,   /* �h�s�a   */
                           ( char ) 0x03, ( char ) 0x00,   /* �d�s�w   */
                           ( char ) 0x37, ( char ) 0x00,   /* �d�n�s   */
                           ( char ) 0x2D, ( char ) 0x00,   /* �d�m�p   */
                           ( char ) 0x10, ( char ) 0x00,   /* �c�k�d   */
                           ( char ) 0x10, ( char ) 0x70,   /* �`�b�j�O */
                           ( char ) 0x10, ( char ) 0x61,   /* �`�b�j�P */
                           ( char ) 0x3D, ( char ) 0x00,   /* �m�`�j   */
                           ( char ) 0x10, ( char ) 0x6B,   /* �v�`�b�j */
                           ( char ) 0x10, ( char ) 0x7C,   /* �q�u�h   */
                           ( char ) 0x02, ( char ) 0x2D,   /* �s�s�c   */
                           ( char ) 0x10, ( char ) 0x37    /* �c�h�r�b */
};

                                            /* ���䕶���R�[�h�i�`�r�b�h�h�j */
static const char  ENB_CTLASC[ICTL_CHRNM][ICTL_CHRSZ] = {
                           ( char ) 0x16, ( char ) 0x00,   /* �r�x�m   */
                           ( char ) 0x01, ( char ) 0x00,   /* �r�n�g   */
                           ( char ) 0x02, ( char ) 0x00,   /* �r�s�w   */
                           ( char ) 0x17, ( char ) 0x00,   /* �d�s�a   */
                           ( char ) 0x1F, ( char ) 0x00,   /* �h�s�a   */
                           ( char ) 0x03, ( char ) 0x00,   /* �d�s�w   */
                           ( char ) 0x04, ( char ) 0x00,   /* �d�n�s   */
                           ( char ) 0x05, ( char ) 0x00,   /* �d�m�p   */
                           ( char ) 0x10, ( char ) 0x00,   /* �c�k�d   */
                           ( char ) 0x10, ( char ) 0x30,   /* �`�b�j�O */
                           ( char ) 0x10, ( char ) 0x31,   /* �`�b�j�P */
                           ( char ) 0x15, ( char ) 0x00,   /* �m�`�j   */
                           ( char ) 0x10, ( char ) 0x2C,   /* �v�`�b�j */
                           ( char ) 0x10, ( char ) 0x40,   /* �q�u�h   */
                           ( char ) 0x02, ( char ) 0x05,   /* �s�s�c   */
                           ( char ) 0x10, ( char ) 0x04    /* �c�h�r�b */
};

                                            /* ���䕶���T�C�Y */
static const int   ENB_CTLSIZ[ICTL_CHRNM] = {
                                                      1,   /* �r�x�m   */
                                                      1,   /* �r�n�g   */
                                                      1,   /* �r�s�w   */
                                                      1,   /* �d�s�a   */
                                                      1,   /* �h�s�a   */
                                                      1,   /* �d�s�w   */
                                                      1,   /* �d�n�s   */
                                                      1,   /* �d�m�p   */
                                                      1,   /* �c�k�d   */
                                                      2,   /* �`�b�j�O */
                                                      2,   /* �`�b�j�P */
                                                      1,   /* �m�`�j   */
                                                      2,   /* �v�`�b�j */
                                                      2,   /* �q�u�h   */
                                                      2,   /* �s�s�c   */
                                                      2    /* �c�h�r�b */
};

                                            /* ���䕶���z��h�m�c�d�w */
#define            ENBCTL_IXSYN        0    /* �r�x�m   */
#define            ENBCTL_IXSOH        1    /* �r�n�g   */
#define            ENBCTL_IXSTX        2    /* �r�s�w   */
#define            ENBCTL_IXETB        3    /* �d�s�a   */
#define            ENBCTL_IXITB        4    /* �h�s�a   */
#define            ENBCTL_IXETX        5    /* �d�s�w   */
#define            ENBCTL_IXEOT        6    /* �d�n�s   */
#define            ENBCTL_IXENQ        7    /* �d�m�p   */
#define            ENBCTL_IXDLE        8    /* �c�k�d   */
#define            ENBCTL_IXAK0        9    /* �`�b�j�O */
#define            ENBCTL_IXAK1       10    /* �`�b�j�P */
#define            ENBCTL_IXNAK       11    /* �m�`�j   */
#define            ENBCTL_IXWCK       12    /* �v�`�b�j */
#define            ENBCTL_IXRVI       13    /* �q�u�h   */
#define            ENBCTL_IXTTD       14    /* �s�s�c   */
#define            ENBCTL_IXDSC       15    /* �c�h�r�b */
#define            ENBCTL_IXTXT       -1    /* �e�L�X�g�i�����l�j */

/**********************************************************************************/
/*       �^�C�}�[�ԍ�                                                             */
/**********************************************************************************/

#define            EB_TMRID_ENQSD     10    /* �d�m�p�đ��^�C�}�[       */
#define            EB_TMRID_AK0RV     11    /* �`�b�j�O��M�҂��^�C�}�[ */
#define            EB_TMRID_AK1RV     12    /* �`�b�j�P��M�҂��^�C�}�[ */
#define            EB_TMRID_TXTRV     13    /* �e�L�X�g��M�҂��^�C�}�[ */
#define            EB_TMRID_EOTRV     14    /* �d�n�s��M�҂��^�C�}�[   */

/**********************************************************************************/
/*       �v���g�^�C�v�錾                                                         */
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
