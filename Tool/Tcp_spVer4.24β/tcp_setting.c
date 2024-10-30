/***********************************************************************
*                                                                      *
***********************************************************************/
//
#include	<windows.h>
#include	<commctrl.h>
#include	"resource.h"
#include	"AP_func.h"
#include	"dump.h"
#include    "TcpCtrl.h"
#include    "enet_b.h"

#define		WM_USER_ASYNC_SELECT	(WM_USER + 1)		// ���[�U��`���b�Z�[�W
#define     LOAD        100

BOOL	CALLBACK DlgProc1(HWND hWnd, UINT uMessage, WPARAM wParam, LPARAM lParam);
BOOL    ChkRadioProc(HWND hWnd);
BOOL    ChkTraceSave(HWND hWnd);
BOOL	ChkRecvSave(HWND hWnd);
BOOL	MakeDir(HWND hWnd, char name[256], int size);
BOOL	CALLBACK DlgProc2(HWND hWnd, UINT uMessage, WPARAM wParam, LPARAM lParam);
BOOL	CALLBACK DlgProc3(HWND hWnd, UINT uMessage, WPARAM wParam, LPARAM lParam);
BOOL    CALLBACK DlgProc6(HWND hWnd, UINT uMessage, WPARAM wParam, LPARAM lParam);


void    getSchFileName(HWND hWnd);
void    getSinaFileName(HWND hWnd);
void    getEditName(HWND hWnd);
void    getCountName(HWND hWnd);
void    getTraceName(HWND hWnd);
void    getRecvName(HWND hWnd);
void    getAutoRecvName(HWND hWnd);
void    schEditBtn(HWND hWnd, char* fname);
void    schReReadBtn(HWND hWnd);
void    sinaReReadBtn(HWND hWnd);
void    clsTraceFileName(HWND hWnd);
void    clsRecvFileName(HWND hWnd);
void    clsSinaFileName(HWND hWnd);
void    clsSchFileName(HWND hWnd);
void    typeReadBtn(HWND hWnd);

int     TcpEnetb ( HWND hWnd );

static	char	MsgBuf[128];
static  HWND    main_hWnd;

extern  int     check_command(HWND hWnd, char*name, int sLen);
extern  HINSTANCE  hThisInst;

int		TcpSetting( HWND hWnd )
{
    main_hWnd = hWnd;
	DialogBox( hThisInst, (const char *)"IDD_TCP_SETTING", hWnd, DlgProc1 );
	return 0;
}

int		TcpVersion( HWND hWnd )
{
	DialogBox( hThisInst, (const char *)"IDD_TCP_VERSION", hWnd, DlgProc2 );
	return 0;
}

int     UserMsgBox( HWND hWnd, char* user_msg, char* user_title )
{
    g_user_msg = user_msg;
    if(NULL != user_title) {
        g_user_title = user_title;
    } else {
        g_user_title = NULL;
    }
	DialogBox( hThisInst, (const char *)"IDD_MSGBOX", hWnd, DlgProc3 );

	return 0;
}

int        TcpEnetb ( HWND hWnd )
{
    DialogBox( hThisInst, (const char *)"IDD_TCP_ENETB", hWnd, DlgProc6 );
    return 0;
}

//*******************************************************************************
//
//*******************************************************************************
BOOL CALLBACK DlgProc1(HWND hWnd, UINT uMessage, WPARAM wParam, LPARAM lParam)
{
	struct	_stat	buf;

	switch (uMessage) {
	case WM_INITDIALOG:								// �����\��
    case WM_USER_ASYNC_SELECT+LOAD:
        setting_hWnd = hWnd;
		if(0 != g_flag) {
			EnableWindow(GetDlgItem(hWnd, IDC_CTRL_LOAD), FALSE);
			EnableWindow(GetDlgItem(hWnd, IDC_CTRL_SAVE), FALSE);
//			EnableWindow(GetDlgItem(hWnd, IDC_SCH_READ_BTN), FALSE);
		}

        // �g���[�X�o�̓R�[�h�w��
		if(tcpCtrl.traceCode == _ASCII) {
			SendDlgItemMessage(hWnd, IDC_RADIO2_ASC, BM_SETCHECK, 1, 0);
			SendDlgItemMessage(hWnd, IDC_RADIO2_EBC, BM_SETCHECK, 0, 0);
        } else if(tcpCtrl.traceCode == _EBCDIC) {
			SendDlgItemMessage(hWnd, IDC_RADIO2_EBC, BM_SETCHECK, 1, 0);
			SendDlgItemMessage(hWnd, IDC_RADIO2_ASC, BM_SETCHECK, 0, 0);
        } else {
            tcpCtrl.traceCode = _ASCII;
			SendDlgItemMessage(hWnd, IDC_RADIO2_ASC, BM_SETCHECK, 1, 0);
			SendDlgItemMessage(hWnd, IDC_RADIO2_EBC, BM_SETCHECK, 0, 0);
        }

		// �g���[�X�o�̓`�F�b�N�a�n�w
		SendDlgItemMessage(hWnd, IDC_CHECK1_SEND, BM_SETCHECK, tcpCtrl.traceOutSend, 0);
		SendDlgItemMessage(hWnd, IDC_CHECK1_RECV, BM_SETCHECK, tcpCtrl.traceOutRecv, 0);

		// ̧�پ��ނ̕\��
		SendDlgItemMessage(hWnd, IDC_CHECK2_TRACE, BM_SETCHECK, tcpCtrl.traceSave, 0);

		SetDlgItemText(hWnd, IDC_EDIT1_TRACE, tcpCtrl.traceName);
		if(tcpCtrl.traceSave == 0) {
			EnableWindow(GetDlgItem(hWnd, IDC_EDIT1_TRACE), FALSE);
        } else {
			EnableWindow(GetDlgItem(hWnd, IDC_EDIT1_TRACE), TRUE);
        }

		// ��M�f�[�^�t�@�C���i�[
		SendDlgItemMessage(hWnd, IDC_RECV_FILE, BM_SETCHECK, tcpCtrl.recvChkFile, 0);
		SetDlgItemText(hWnd, IDC_RECV_USR_NAME, tcpCtrl.recvFileName);
		SetDlgItemText(hWnd, IDC_RECV_AUTO_NAME, tcpCtrl.recvFileBinName);
		if(tcpCtrl.recvChkFileName == 1) {
			SendDlgItemMessage(hWnd, IDC_RECV_USR_FILE, BM_SETCHECK, 1, 0); 
			EnableWindow(GetDlgItem(hWnd, IDC_RECV_AUTO_NAME), FALSE);
        } else {
			SendDlgItemMessage(hWnd, IDC_RECV_AUTO_FILE, BM_SETCHECK, 1, 0);
			EnableWindow(GetDlgItem(hWnd, IDC_RECV_USR_NAME), FALSE);
 		}
		if(tcpCtrl.recvChkFileName == 1) {
            // ̧�ٖ��w�肪����ON�̎�
			SendDlgItemMessage(hWnd, IDC_RECV_USR_FILE, BM_SETCHECK, 1, 0); 
			EnableWindow(GetDlgItem(hWnd, IDC_RECV_USR_NAME), TRUE);
			EnableWindow(GetDlgItem(hWnd, IDC_RECV_AUTO_NAME), FALSE);
            EnableWindow(GetDlgItem(hWnd, IDC_STATIC_DIR), FALSE);
		} else {
            // �����ɂ�鎩���i�[������ON�̎�
			SendDlgItemMessage(hWnd, IDC_RECV_AUTO_FILE, BM_SETCHECK, 1, 0);
			EnableWindow(GetDlgItem(hWnd, IDC_RECV_USR_NAME), FALSE);
			EnableWindow(GetDlgItem(hWnd, IDC_RECV_AUTO_NAME), TRUE);
            EnableWindow(GetDlgItem(hWnd, IDC_STATIC_DIR), TRUE);
		}
        // �X�P�W���[���Ǘ��L��
        SetDlgItemText(hWnd, IDC_SCH_NAME, tcpCtrl.schFileName);
   		SendDlgItemMessage(hWnd, IDC_SCH_CHECK, BM_SETCHECK, tcpCtrl.schCtrl, 0);
        // ������Ǘ��L��
   		SendDlgItemMessage(hWnd, IDC_CYC_CHECK, BM_SETCHECK, tcpCtrl.cycCtrl, 0);
		//
		SetDlgItemText(hWnd, IDC_EDIT_NAME, tcpCtrl.editName);
		SetDlgItemText(hWnd, IDC_COUNT_NAME, tcpCtrl.counterFile);
        // �V�i���I�Ǘ��L��
        SetDlgItemText(hWnd, IDC_SINA_NAME, tcpCtrl.sinaFileName);
   		SendDlgItemMessage(hWnd, IDC_SINA_CHECK, BM_SETCHECK, tcpCtrl.sinaCtrl, 0);

        // �i�v���g���C�w��
   		SendDlgItemMessage(hWnd, IDC_RETRY_CHECK, BM_SETCHECK, tcpCtrl.retry_flg, 0);
   		
   		// �d�������|�a�ݒ�{�^���L���^�����w��
   		if ( ENET_B == tcpCtrl.comType || ENET_BX == tcpCtrl.comType ) {
   			EnableWindow ( GetDlgItem ( hWnd, IDC_ENETB ), TRUE );
   		} else {
   			EnableWindow ( GetDlgItem ( hWnd, IDC_ENETB ), FALSE );
   		}

        break;

    case WM_COMMAND :
		switch (LOWORD(wParam)) {
		/////////////////////////////////////////
		// �t�@�C���Z�[�u�`�F�b�N�a�n�w
		case IDC_CHECK2_TRACE:
			if(tcpCtrl.traceSave == 1) {
				tcpCtrl.traceSave = 0;
				EnableWindow(GetDlgItem(hWnd, IDC_EDIT1_TRACE), FALSE);
			} else {
				tcpCtrl.traceSave = 1;
				EnableWindow(GetDlgItem(hWnd, IDC_EDIT1_TRACE), TRUE);
			}
            if(FALSE == ChkTraceSave(hWnd)) {
				tcpCtrl.traceSave = 0;
				SendDlgItemMessage(hWnd, IDC_CHECK2_TRACE, BM_SETCHECK, 0, 0); 
				EnableWindow(GetDlgItem(hWnd, IDC_EDIT1_TRACE), FALSE);
                break;
            }

			break;

		/////////////////////////////////////////
		// �g���[�X�o�͑��M�`�F�b�N�a�n�w
		case IDC_CHECK1_SEND:
			if(tcpCtrl.traceOutSend == 1) {
				tcpCtrl.traceOutSend = 0;
			} else {
				tcpCtrl.traceOutSend = 1;
			}
			break;

		/////////////////////////////////////////
		// �g���[�X�o�͎�M�`�F�b�N�a�n�w
		case IDC_CHECK1_RECV:
			if(tcpCtrl.traceOutRecv == 1) {
				tcpCtrl.traceOutRecv = 0;
			} else {
				tcpCtrl.traceOutRecv = 1;
			}
			break;

		/////////////////////////////////////////
		// ��M�f�[�^�t�@�C���i�[
		case IDC_RECV_FILE:
			if(tcpCtrl.recvChkFile == 1) {
                // ��M�ް���̧�يi�[���邪����ON->OFF�̎�
				tcpCtrl.recvChkFile = 0;
			} else {
                // ��M�ް���̧�يi�[���邪����OFF->ON�̎�
				tcpCtrl.recvChkFile = 1;
			}
			// �i�[̧�ٖ����I������Ă��邩��������
			if(FALSE == ChkRecvSave(hWnd)) {
				tcpCtrl.recvChkFile = 0;
				SendDlgItemMessage(hWnd, IDC_RECV_FILE, BM_SETCHECK, 0, 0); 
		        break;
			}
			break;

		/////////////////////////////////////////
		// ��M�ް�̧�يi�[�̎����i�[������ON
        //�@(tcpCtrl.recvChkFileName=0:�����ݒ�)
		case IDC_RECV_AUTO_FILE:
			if(tcpCtrl.recvChkFileName != 0) {
				tcpCtrl.recvChkFileName = 0;
				EnableWindow(GetDlgItem(hWnd, IDC_RECV_USR_NAME), FALSE);
				EnableWindow(GetDlgItem(hWnd, IDC_RECV_AUTO_NAME), TRUE);
           		EnableWindow(GetDlgItem(hWnd, IDC_STATIC_DIR), TRUE);
			}
			break;

		/////////////////////////////////////////
		// ��M�ް�̧�يi�[��̧�ٖ��w�肪����ON
        //�@(tcpCtrl.recvChkFileName=1:̧�ٖ��w��)
		case IDC_RECV_USR_FILE:
			if(tcpCtrl.recvChkFileName != 1) {
				tcpCtrl.recvChkFileName = 1;
				EnableWindow(GetDlgItem(hWnd, IDC_RECV_USR_NAME), TRUE);
				EnableWindow(GetDlgItem(hWnd, IDC_RECV_AUTO_NAME), FALSE);
           		EnableWindow(GetDlgItem(hWnd, IDC_STATIC_DIR), FALSE);
			}
			break;

		/////////////////////////////////////////
		// �X�P�W���[���Ǘ��L��
        // (tcpCtrl.schCtrl=1:�Ǘ�����)
        case IDC_SCH_CHECK:
			if(tcpCtrl.schCtrl == 0) {
                if(0x00 == tcpCtrl.schFileName[0]) {
                    UserMsgBox(hWnd, "\n���ޭ�يǗ�̧�ق�ݒ肵�ĉ�����", NULL);
                    SendDlgItemMessage(hWnd, IDC_SCH_CHECK, BM_SETCHECK, tcpCtrl.schCtrl, 0);
                    break;
                }
				if(_stat(tcpCtrl.schFileName, &buf) < 0) {
		   	        sprintf(MsgBuf, "���ޭ�يǗ�̧��\n\n[%s]\n\n��������܂���", tcpCtrl.schFileName);
                    MessageBox( hWnd, MsgBuf, "�m�F" , MB_ICONEXCLAMATION | MB_OK );
                    SendDlgItemMessage(hWnd, IDC_SCH_CHECK, BM_SETCHECK, tcpCtrl.schCtrl, 0);
                } else {
    				tcpCtrl.schCtrl = 1;
                }
			} else {
				tcpCtrl.schCtrl = 0;
			}
			break;

		/////////////////////////////////////////
		// �V�i���I�Ǘ��L��
        // (tcpCtrl.sinaCtrl=1:�Ǘ�����)
        case IDC_SINA_CHECK:
			if(tcpCtrl.sinaCtrl == 0) {
                if(0x00 == tcpCtrl.sinaFileName[0]) {
                    UserMsgBox(hWnd, "\n��ص̧�ق�ݒ肵�ĉ�����", NULL);
               		SendDlgItemMessage(hWnd, IDC_SINA_CHECK, BM_SETCHECK, tcpCtrl.sinaCtrl, 0);
                    break;
                }
				if(_stat(tcpCtrl.sinaFileName, &buf) < 0) {
		   	        sprintf(MsgBuf, "��ص̧��\n\n[%s]\n\n��������܂���", tcpCtrl.sinaFileName);
		   	        MessageBox( hWnd, MsgBuf, "�m�F" , MB_ICONEXCLAMATION | MB_OK );
               		SendDlgItemMessage(hWnd, IDC_SINA_CHECK, BM_SETCHECK, tcpCtrl.sinaCtrl, 0);
                } else {
    				tcpCtrl.sinaCtrl = 1;
                    timerSetProc1(main_hWnd);
                }
			} else {
				tcpCtrl.sinaCtrl = 0;
                timerResetProc1(main_hWnd);
			}
			break;

		/////////////////////////////////////////
		// �i�v���g���C�w��
        // (tcpCtrl.retry_flg)
        case IDC_RETRY_CHECK:
			if(tcpCtrl.retry_flg == 0) {
  				tcpCtrl.retry_flg = 1;
			} else {
				tcpCtrl.retry_flg = 0;
			}
			break;

		/////////////////////////////////////////
		// ������Ǘ��L��
        // (tcpCtrl.cycCtrl=1:�Ǘ�����)
        case IDC_CYC_CHECK:
			if(tcpCtrl.cycCtrl == 0) {
                if(0x00 == tcpCtrl.schFileName[0]) {
                    UserMsgBox(hWnd, "\n���ޭ�يǗ�̧�ق�ݒ肵�ĉ�����", NULL);
              		SendDlgItemMessage(hWnd, IDC_CYC_CHECK, BM_SETCHECK, tcpCtrl.cycCtrl, 0);
                    break;
                }
				if(_stat(tcpCtrl.schFileName, &buf) < 0) {
		   	        sprintf(MsgBuf, "���ޭ�يǗ�̧��\n\n[%s]\n\n��������܂���", tcpCtrl.schFileName);
		   	        MessageBox( hWnd, MsgBuf, "�m�F" , MB_ICONEXCLAMATION | MB_OK );
              		SendDlgItemMessage(hWnd, IDC_CYC_CHECK, BM_SETCHECK, tcpCtrl.cycCtrl, 0);
                } else {
    				tcpCtrl.cycCtrl = 1;
                    timerSetProc(main_hWnd);
                }
			} else {
				tcpCtrl.cycCtrl = 0;
                timerResetProc(main_hWnd);
			}
			break;

		/////////////////////////////////////////
		// �������ON
		case IDC_VERSION:
            TcpVersion(hWnd);
            break;

        //////////////////////////////////////////////
        // ���ޭ�يǗ�̧�ق̓ǂݍ�������ON
		case IDC_SCH_READ_BTN:
            getSchFileName(hWnd);
            break;

        //////////////////////////////////////////////
        // ��ص�Ǘ�̧�ق̓ǂݍ�������ON
		case IDC_SINA_READ_BTN:
            getSinaFileName(hWnd);
            break;

        //////////////////////////////////////////////
        // ���ޭ�يǗ�̧�ق̕ҏW����ON
		case IDC_SCH_EDIT_BTN:
            schEditBtn(hWnd, tcpCtrl.schFileName);
            break;

		//////////////////////////////////////////////
        // ��ص�Ǘ�̧�ق̕ҏW����ON
		case IDC_SINA_EDIT_BTN:
            schEditBtn(hWnd, tcpCtrl.sinaFileName);
            break;

        //////////////////////////////////////////////
        // ���ޭ�يǗ�̧�ق̍ēǍ�����ON
		case IDC_SCH_REREAD_BTN:
            schReReadBtn(hWnd);
            break;

        //////////////////////////////////////////////
        // ��ص�Ǘ�̧�ق̍ēǍ�����ON
		case IDC_SINA_REREAD_BTN:
            sinaReReadBtn(hWnd);
			break;

        //////////////////////////////////////////////
        // ��ص�Ǘ���ݾ�����ON
//		case IDC_SINA_BTN:
//			timerResetProc1(hWnd);
//            break;

		//////////////////////////////////////////////
        // ��M��ص�Ǘ���ݾ�����ON
		case IDC_RECV_SINA_BTN:
			timerResetProc2(main_hWnd);
            break;

		//////////////////////////////////////////////
        // �`������̧�ق̎Q������ON
		case IDC_TYPE_READ_BTN:
            typeReadBtn(hWnd);
            break;

        //////////////////////////////////////////////
        // ��ި�̧�ق̓ǂݍ�������ON
		case IDC_EDIT_BTN:
            getEditName(hWnd);
            break;

        //////////////////////////////////////////////
        // ��ި�̧�ٖ�������
		case IDC_EDITCLS_BTN:
	        memset(tcpCtrl.editName, 0x00, sizeof(tcpCtrl.editName));
			SetDlgItemText(hWnd, IDC_EDIT_NAME, tcpCtrl.editName);
            break;

        //////////////////////////////////////////////
        // ����̧�ق̓ǂݍ�������ON
		case IDC_COUNT_BTN:
            getCountName(hWnd);
            break;

        //////////////////////////////////////////////
        // ����̧�ٖ�������
		case IDC_COUNTCLS_BTN:
	        memset(tcpCtrl.counterFile, 0x00, sizeof(tcpCtrl.counterFile));
			SetDlgItemText(hWnd, IDC_COUNT_NAME, tcpCtrl.counterFile);
            break;

        //////////////////////////////////////////////
        // �ڰ�̧�ٖ��w��
		case IDC_TRACE_BTN:
            getTraceName(hWnd);
            break;

        //////////////////////////////////////////////
        // ��M�ް��i�[̧�ٖ��w��
		case IDC_RECV_BTN:
            getRecvName(hWnd);
            break;

        //////////////////////////////////////////////
        // ������M�ް��i�[�ިڸ�ب���w��
		case IDC_AUTO_RECV_BTN:
            getAutoRecvName(hWnd);
            break;

        //////////////////////////////////////////////
        // �ڰ��ް��i�[̧�ٖ�������
		case IDC_TRACE_CLS_BTN:
            clsTraceFileName(hWnd);
            break;

        //////////////////////////////////////////////
        // ��M�ް��i�[̧�ٖ�������
		case IDC_RECV_CLS_BTN:
            clsRecvFileName(hWnd);
            break;

        //////////////////////////////////////////////
        // ��ص̧�ٖ�������
		case IDC_SINA_CLS_BTN:
            clsSinaFileName(hWnd);
            break;


        //////////////////////////////////////////////
        // ���ޭ��̧�ٖ�������
		case IDC_SCH_CLS_BTN:
            clsSchFileName(hWnd);
            break;

        case IDCANCEL :										// ��ݾ� & �I��
            ChkRadioProc(hWnd);
            if(FALSE == ChkTraceSave(hWnd)) {
                break;
            }
			if(FALSE == ChkRecvSave(hWnd)) {
                break;
            }
            setting_hWnd = 0;
			EndDialog(hWnd, 0);
			break;

    	/////////////////////////////////////////
		// ���۰�̧�ٕۑ�
        case IDC_CTRL_SAVE:		    //
            ChkRadioProc(hWnd);
            CtrlFileSave(main_hWnd);
            break;

    	/////////////////////////////////////////
		// ���۰�̧�ٓǂݍ���
        case IDC_CTRL_LOAD :
			if ( g_sSock[0] == INVALID_SOCKET && g_sSock[1] == INVALID_SOCKET) {
                CtrlFileLoad(main_hWnd);
                PostMessage( hWnd, WM_USER_ASYNC_SELECT+LOAD, 0, WSAMAKESELECTREPLY( 0, 0 ) );
                if(1 == tcpCtrl.cycCtrl) {
                    timerSetProc(main_hWnd);
                } else {
		            timerResetProc(main_hWnd);
                }
                if(1 == tcpCtrl.sinaCtrl) {
                    timerSetProc1(main_hWnd);
                } else {
		            timerResetProc1(main_hWnd);
                }
            } else {
//            	MessageBox( hWnd, "����ڑ����ł��B�Ǎ��݂ł��܂���", "�m�F" ,
//							MB_ICONEXCLAMATION | MB_OK );
                UserMsgBox(hWnd, "\n����ڑ����ł��B�Ǎ��݂ł��܂���", NULL);

            }
            break;

        /////////////////////////////////////////
        // �d�������|�a�ݒ�{�^��ON
        case IDC_ENETB:
            TcpEnetb ( hWnd );
            break;

        default :
			return FALSE;
		}
		break;

	case WM_DESTROY :												// �I��
        putTcpCtrl(hWnd, NULL);
        setting_hWnd = 0;
		DestroyWindow( 0 );
		break;
	default :
		return FALSE;
	}
	return TRUE;
}

//////////////////////////////////////////////////////////////////////////
//
//�@�I�����̃��W�I�{�^���̏�Ԃ��i�[����
//
//////////////////////////////////////////////////////////////////////////
BOOL	ChkRadioProc(HWND hWnd)
{
    // �g���[�X�o�͋敪
	if(SendDlgItemMessage(hWnd, IDC_RADIO2_ASC, BM_GETCHECK, 0, 0)) {
		tcpCtrl.traceCode = _ASCII;
    } else if(SendDlgItemMessage(hWnd, IDC_RADIO2_EBC, BM_GETCHECK, 0, 0)) {
		tcpCtrl.traceCode = _EBCDIC;
    } else {
		tcpCtrl.traceCode = _ASCII;
    }
    return TRUE;
}

//////////////////////////////////////////////////////////////////////////
//
//�@�I�����̃g���[�X�o�̓t�@�C�������O���[�o���Ɋi�[����
//
//////////////////////////////////////////////////////////////////////////
BOOL	ChkTraceSave(HWND hWnd)
{
//	int		i;
	int		sLen;
    int     rtc;
    char    name[256];

    memset(name, 0x00, sizeof(name));

	if(tcpCtrl.traceSave == 1) {
		if(GetDlgItemText(hWnd, IDC_EDIT1_TRACE, name, sizeof(name))) {
			// �t�@�C�������͕����`�F�b�N
			sLen = strlen(name);
            // �V���A���m���`�F�b�N
            rtc=check_command(main_hWnd, name, sLen);
            if(1 == rtc) {
                tcpCtrl.traceSave = 0;
				SendDlgItemMessage(hWnd, IDC_CHECK2_TRACE, BM_SETCHECK, 0, 0); 
				memset(tcpCtrl.traceName, 0x00, sizeof(tcpCtrl.traceName));
			    SetDlgItemText(hWnd, IDC_EDIT1_TRACE, tcpCtrl.traceName);
			   	MessageBox( hWnd, "�������肪�Ƃ��������܂��Bm(_ _)m  \n\n�ēx TcpSp ���N�����Ă��������I�I", "���ӁI�I(^�^)v ",
  								MB_ICONEXCLAMATION | MB_OK );
                return TRUE;
            }
			memset(tcpCtrl.traceName, 0x00, sizeof(tcpCtrl.traceName));
			memcpy(tcpCtrl.traceName, name, sLen);
		} else {
//              MessageBox( hWnd, "�g���[�X�i�[�t�@�C��������͂��ĉ�����", "�m�F" , MB_ICONEXCLAMATION | MB_OK );
                UserMsgBox(hWnd, "\n�g���[�X�i�[�t�@�C������ݒ肵�ĉ�����", NULL);
			return FALSE;
		}
	} else {
		if(GetDlgItemText(hWnd, IDC_EDIT1_TRACE, name, sizeof(name))) {
             // nothing to do
        } else {
            memset(tcpCtrl.traceName, 0x00, sizeof(tcpCtrl.traceName));
        }
    }
     return TRUE;
}

//////////////////////////////////////////////////////////////////////////
//
//�@�I�����̎�M�f�[�^�i�[�t�@�C�������O���[�o���Ɋi�[����
//
//////////////////////////////////////////////////////////////////////////
BOOL	ChkRecvSave(HWND hWnd)
{
    char    name[256];

    memset(name, 0x00, sizeof(name));
    if(GetDlgItemText(hWnd, IDC_RECV_USR_NAME, tcpCtrl.recvFileName, sizeof(tcpCtrl.recvFileName))) {
    } else {
         memset(tcpCtrl.recvFileName, 0x00, sizeof(tcpCtrl.recvFileName));
    }

    memset(name, 0x00, sizeof(name));
    if(GetDlgItemText(hWnd, IDC_RECV_AUTO_NAME, tcpCtrl.recvFileBinName, sizeof(tcpCtrl.recvFileBinName))) {
    } else {
         memset(tcpCtrl.recvFileBinName, 0x00, sizeof(tcpCtrl.recvFileBinName));
    }

	if(tcpCtrl.recvChkFile == 0) {
		return TRUE;
	}

	if(tcpCtrl.recvChkFileName == 1) {
        // ̧�ٖ��w��̊i�[���̏���
		if(GetDlgItemText(hWnd, IDC_RECV_USR_NAME, name, sizeof(name))) {
			// �t�@�C�������͕����`�F�b�N
//			sLen = strlen(name);
//			memset(tcpCtrl.recvFileName, 0x00, sizeof(tcpCtrl.recvFileName));
//            memcpy(tcpCtrl.recvFileName, name, sLen);
		} else {
//		   	MessageBox( hWnd, "��M�f�[�^�i�[�t�@�C��������͂��ĉ�����", "�m�F" , MB_ICONEXCLAMATION | MB_OK );
            UserMsgBox(hWnd, "\n��M�f�[�^�i�[�t�@�C������ݒ肵�ĉ�����", NULL);
			return FALSE;
		}
	} else {
        // ̧�ٖ������쐬���̏���      
		if(GetDlgItemText(hWnd, IDC_RECV_AUTO_NAME, name, sizeof(name))) {
			// �t�@�C�������͕����`�F�b�N
//			sLen = strlen(name);
//			memset(tcpCtrl.recvFileBinName, 0x00, sizeof(tcpCtrl.recvFileBinName));
//            memcpy(tcpCtrl.recvFileBinName, name, sLen);
		} else {
//		   	MessageBox( hWnd, "��M�f�[�^�i�[�f�B���N�g���B������͂��ĉ�����", "�m�F" , MB_ICONEXCLAMATION | MB_OK );
            UserMsgBox(hWnd, "��M�f�[�^�i�[�t�H���_��\n\n�ݒ肵�ĉ�����", NULL);
			return FALSE;
		}
    }
    return TRUE;
}

//////////////////////////////////////////////////////////////////////////
//
//�@�ݒ����ǂݍ���
//
//////////////////////////////////////////////////////////////////////////
BOOL getTcpCtrl(HWND hWnd)
{
	int		nfp;
	char    homedir[256];
    char    fullName[256];
	struct	_stat	buf;

    memset(&tcpCtrl,   0x00, sizeof(TCP_CTRL));
    memset(fullName,   0x00, sizeof(fullName));
    memset(homedir,    0x00, sizeof(homedir));

//	MyMessage(hWnd, "pszCmdLine=%s", GlpszCmdLine);
//	MyMessage(hWnd, "nCmdShow=%d",  GnCmdShow);


	if(_stat(GlpszCmdLine, &buf) == 0) {
        if(0 == GetCurrentDirectory(sizeof(homedir), homedir)) {
            MyMessage(hWnd, "�����ިڸ�ب���擾�ł��܂���ł���" );
        }
        MyMessage(hWnd, "ΰ��ިڸ�ب�擾 : [%s]", homedir);
        MyMessage(hWnd, "");

		strcpy(g_OpenName, GlpszCmdLine);
        nfp = AP_Sfopen(g_OpenName, AP_RDWR);
	    if ( nfp < 0 ) {
            return FALSE;
        }
        AP_Sgrec(nfp, sizeof(TCP_CTRL), 0, &tcpCtrl);
    	AP_Sfclose(nfp);
        memset(tcpCtrl.homeDir, 0x00, sizeof(tcpCtrl.homeDir));
		strcpy(tcpCtrl.homeDir, homedir);

	    if(0x00 == tcpCtrl.schFileName[0] || 0x20 == tcpCtrl.schFileName[0]) {
            // ���ޭ�يǗ�̧�ف@�w��Ȃ�
        } else {
            MyMessage(hWnd, "*****  ���ޭ�يǗ�̧�ف@�Ǎ��� ***********************************************");
            ScheduleDataSet(hWnd);
            MyMessage(hWnd, "******************************************************************************");
        }
	    if(0x00 == tcpCtrl.sinaFileName[0] || 0x20 == tcpCtrl.sinaFileName[0]) {
            // ��ص�Ǘ�̧�ف@�w��Ȃ�
        } else {
            MyMessage(hWnd, "*****  ��ص�Ǘ�̧�ف@�Ǎ��� **************************************************");
            SinaDataSet(hWnd);
            MyMessage(hWnd, "******************************************************************************");
        }
	    if(0x00 != tcpCtrl.listData[0]) {
			loadListData(hWnd, 0);
		}

        PostMessage( hWnd, WM_USER_ASYNC_SELECT+LOAD, 0, WSAMAKESELECTREPLY( 0, 0 ) );
        if(1 == tcpCtrl.cycCtrl) {
            timerSetProc(hWnd);
        } else {
            timerResetProc(hWnd);
        }
        if(1 == tcpCtrl.sinaCtrl) {
            timerSetProc1(hWnd);
        } else {
            timerResetProc1(hWnd);
        }

		setDialogName(hWnd, g_OpenName);

        return TRUE;
	} else {
        // ���۰�̧�ق�Ǎ��ވׂɁA�ꎞ�I�ɶ����ިڸ�ب��ݒ肷��B
        if(0 == GetCurrentDirectory(sizeof(tcpCtrl.homeDir), tcpCtrl.homeDir)) {
            MyMessage(hWnd, "�����ިڸ�ب���擾�ł��܂���ł���" );
        }
        // MyMessage(hWnd, "ΰ��ިڸ�ب�擾 : [%s]", tcpCtrl.homeDir);
        MakeFullName(hWnd, TCP_CTRL_NAME, fullName);
	}
	nfp = AP_Sfopen(fullName, AP_RDWR);
	if ( nfp < 0 ) {
        tcpCtrl.connectType[0]  = 1;
        tcpCtrl.connectType[1]  = 1;
        tcpCtrl.traceCode    = _ASCII;
        tcpCtrl.traceOutSend = 1;
        tcpCtrl.traceOutRecv = 1;
        tcpCtrl.schCtrl = 0;
        tcpCtrl.cycCtrl = 0;
        tcpCtrl.fileForm = 1;
        tcpCtrl.tcp_udp = 0;
        memset(tcpCtrl.schFileName, 0x00, sizeof(tcpCtrl.schFileName));
        memset(tcpCtrl.editName, 0x00, sizeof(tcpCtrl.editName));
        memset(tcpCtrl.listData, 0x00, sizeof(tcpCtrl.listData));
        memset(tcpCtrl.sinaFileName, 0x00, sizeof(tcpCtrl.sinaFileName));
        memset(tcpCtrl.counterFile, 0x00, sizeof(tcpCtrl.counterFile));
        
        tcpCtrl.eb_mojicode = INIT_MJCOD;
        tcpCtrl.eb_transpar = INIT_TRNSP;
        tcpCtrl.tm_ak1_atxt = INIT_TMAK1;
        tcpCtrl.tm_ak0_aenq = INIT_TMAK0;
        tcpCtrl.tm_txt_aak0 = INIT_TMTXT;
        tcpCtrl.tm_eot_aak1 = INIT_TMEOT;
        tcpCtrl.tm_enq_anak = INIT_TMENQ;
        tcpCtrl.nm_enq_nans = INIT_NMENQ;
        tcpCtrl.nm_txt_anak = INIT_NMTXT;
        
    	nfp = AP_Sfopen(fullName, AP_CREATRDWR);
	    AP_Sprec(nfp, sizeof(TCP_CTRL), 0, &tcpCtrl);
	}
    AP_Sgrec(nfp, sizeof(TCP_CTRL), 0, &tcpCtrl);
	AP_Sfclose(nfp);

    memset(tcpCtrl.schFileName, 0x00, sizeof(tcpCtrl.schFileName));
    memset(tcpCtrl.listData, 0x00, sizeof(tcpCtrl.listData));
    memset(tcpCtrl.sinaFileName, 0x00, sizeof(tcpCtrl.sinaFileName));
    memset(tcpCtrl.counterFile, 0x00, sizeof(tcpCtrl.counterFile));

    tcpCtrl.schCtrl = 0;
    tcpCtrl.cycCtrl = 0;
    tcpCtrl.recvChkFile = 0;
    tcpCtrl.traceSave = 0;
	tcpCtrl.sinaCtrl = 0;
	tcpCtrl.rsina_stop = 0;
	tcpCtrl.sina_stop = 0;

    if( tcpCtrl.eb_mojicode != _ASCII && tcpCtrl.eb_mojicode != _EBCDIC ) {
        if( tcpCtrl.comType == ENET_B || tcpCtrl.comType == ENET_BX) {
            MyMessage ( hWnd, "LINE:%d �����R�[�h�G���[�F�����l=EBCDIC���r�d�s", __LINE__ );
        }
        tcpCtrl.eb_mojicode = INIT_MJCOD;
    }
    if ( tcpCtrl.eb_transpar != EB_NOTTRNS && tcpCtrl.eb_transpar != EB____TRNS ) {
        if( tcpCtrl.comType == ENET_B || tcpCtrl.comType == ENET_BX) {
            MyMessage ( hWnd, "LINE:%d ���߁^�񓧉߃G���[�F�����l=�񓧉߂��r�d�s", __LINE__ );
        }
        tcpCtrl.eb_transpar = INIT_TRNSP;
    }
    if ( tcpCtrl.tm_ak1_atxt < IBCDMN_TIM || IBCDMX_TIM < tcpCtrl.tm_ak1_atxt ) {
        if( tcpCtrl.comType == ENET_B || tcpCtrl.comType == ENET_BX) {
            MyMessage ( hWnd, "LINE:%d ACK1���ԏ㉺���G���[�F�����l=%d���r�d�s", __LINE__, INIT_TMAK1 );
        }
        tcpCtrl.tm_ak1_atxt = INIT_TMAK1;
    }
    if ( tcpCtrl.tm_ak0_aenq < IBCDMN_TIM || IBCDMX_TIM < tcpCtrl.tm_ak0_aenq ) {
        if( tcpCtrl.comType == ENET_B || tcpCtrl.comType == ENET_BX) {
            MyMessage ( hWnd, "LINE:%d ACK0���ԏ㉺���G���[�F�����l=%d���r�d�s", __LINE__, INIT_TMAK0 );
        }
        tcpCtrl.tm_ak0_aenq = INIT_TMAK0;
    }
    if ( tcpCtrl.tm_txt_aak0 < IBCDMN_TIM || IBCDMX_TIM < tcpCtrl.tm_txt_aak0 ) {
        if( tcpCtrl.comType == ENET_B || tcpCtrl.comType == ENET_BX) {
            MyMessage ( hWnd, "LINE:%d ÷�Ď��ԏ㉺���G���[�F�����l=%d���r�d�s", __LINE__, INIT_TMTXT );
        }
        tcpCtrl.tm_txt_aak0 = INIT_TMTXT;
    }
    if ( tcpCtrl.tm_eot_aak1 < IBCDMN_TIM || IBCDMX_TIM < tcpCtrl.tm_eot_aak1 ) {
        if( tcpCtrl.comType == ENET_B || tcpCtrl.comType == ENET_BX) {
            MyMessage ( hWnd, "LINE:%d EOT���ԏ㉺���G���[�F�����l=%d���r�d�s", __LINE__, INIT_TMEOT );
        }
        tcpCtrl.tm_eot_aak1 = INIT_TMEOT;
    }
    if ( tcpCtrl.tm_enq_anak < IBCDMN_TIM || IBCDMX_TIM < tcpCtrl.tm_enq_anak ) {
        if( tcpCtrl.comType == ENET_B || tcpCtrl.comType == ENET_BX) {
            MyMessage ( hWnd, "LINE:%d ENQ�đ��҂����ԏ㉺���G���[�F�����l=%d���r�d�s", __LINE__, INIT_TMENQ );
        }
        tcpCtrl.tm_enq_anak = INIT_TMENQ;
    }
    if ( tcpCtrl.nm_enq_nans < IBCDMN_NUM || IBCDMX_NUM < tcpCtrl.nm_enq_nans ) {
        if( tcpCtrl.comType == ENET_B || tcpCtrl.comType == ENET_BX) {
            MyMessage ( hWnd, "LINE:%d ENQ���M�񐔏㉺���G���[�F�����l=%d���r�d�s", __LINE__, INIT_NMENQ );
        }
        tcpCtrl.nm_enq_nans = INIT_NMENQ;
    }
    if ( tcpCtrl.nm_txt_anak < IBCDMN_NUM || IBCDMX_NUM < tcpCtrl.nm_txt_anak ) {
        if( tcpCtrl.comType == ENET_B || tcpCtrl.comType == ENET_BX) {
            MyMessage ( hWnd, "LINE:%d ÷�đ��M�񐔏㉺���G���[�F�����l=%d���r�d�s", __LINE__, INIT_NMTXT );
        }
        tcpCtrl.nm_txt_anak = INIT_NMTXT;
    }

    memset(tcpCtrl.homeDir, 0x00, sizeof(tcpCtrl.homeDir));
    if(0 == GetCurrentDirectory(sizeof(tcpCtrl.homeDir), tcpCtrl.homeDir)) {
        MyMessage(hWnd, "�����ިڸ�ب���擾�ł��܂���ł���" );
    }
    MyMessage(hWnd, "ΰ��ިڸ�ب�擾 : [%s]", tcpCtrl.homeDir);
    MyMessage(hWnd, "");

//	setDialogName(hWnd, fullName);

    return TRUE;
}

//////////////////////////////////////////////////////////////////////////
//
//�@�ݒ������������
//
//////////////////////////////////////////////////////////////////////////
BOOL putTcpCtrl(HWND hWnd, char* name)
{
	int		nfp;
    char    fullName[256];

    memset(fullName, 0x00, sizeof(fullName));

    if(NULL == name) {
        MakeFullName(hWnd, TCP_CTRL_NAME, fullName);
    } else {
        strcpy(fullName, name); 
    }
//    MyMessage(hWnd, "putTcpCtrl(): [%s]", fullName);

//	nfp = AP_Sfopen(fullName, AP_RDWR);
	nfp = AP_Sfopen(fullName, AP_CREATRDWR);
	if (nfp < 0) {
		return FALSE;
	}
    AP_Sprec(nfp, sizeof(TCP_CTRL), 0, &tcpCtrl);
	AP_Sfclose(nfp);

	return TRUE;
}


//////////////////////////////////////////////////////////////////////////
//
//�@�o�̓t�@�C�����̃f�B���N�g���B���쐬����
//
//////////////////////////////////////////////////////////////////////////
BOOL	MakeDir(HWND hWnd, char name[256], int size)
{
	int		i;
    char    mkdir[256];
    char    tmp[256];
	struct	_stat	buf;

    memset(mkdir, 0x00, sizeof(mkdir));
    memset(tmp,   0x00, sizeof(tmp));

    MakeFullName(main_hWnd, name, tmp);

//    MyMessage(main_hWnd, "MakeDir �ިڸ�ب���쐬 (%s) ����", tmp);

    for(i=0; (unsigned int)i<strlen(tmp); i++) {
        if(0 != i && '/' == tmp[i]) {
            // ��M�ް��޲�ب�i�[�ިڸ�ب�쐬
        	if(_stat(mkdir, &buf) < 0) {
                if(CreateDirectory(mkdir, 0)) {
               	    MyMessage(main_hWnd, "�ިڸ�ب���� (%s) ����", mkdir);
                } else {
            	    MyMessage(main_hWnd, "�ިڸ�ب���� (%s) ���s", mkdir);
                }   
            }
        }
        mkdir[i] = tmp[i];
    }
    return TRUE;
}

/*--------------------------------------------------------------
* �t�@�C���ǂݍ��ݏ���
*--------------------------------------------------------------*/
void getSchFileName(HWND hWnd)
{
	OPENFILENAME	ofn;
	unsigned char   dirName[256];
	int             len;
	unsigned char   openName[256];

	memset(openName, 0x00, sizeof(openName));
	memset(&ofn, 0, sizeof(OPENFILENAME));
	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = hWnd;
	// �t�@�C����ނ̎w��
	ofn.lpstrFilter = "�ް�̧�ف@  {*.txt}\0*.txt\0"\
					  "  C ̧��     {*.c}\0*.c; *.cpp; *.h\0"\
					  "�S�Ă�̧�� {*.*}\0*.*\0\0";

	ofn.lpstrFile	= openName;
	ofn.nMaxFile = MAX_PATH;
	ofn.Flags = OFN_FILEMUSTEXIST;
//	ofn.Flags = OFN_FILEMUSTEXIST | OFN_ALLOWMULTISELECT | OFN_LONGNAMES | OFN_EXPLORER;
	// �g���q�����͂���Ȃ�������̫�Ă̊g���q
	ofn.lpstrDefExt = "txt";
	if(GetOpenFileName(&ofn)){
		memset(dirName, 0x00, sizeof(dirName));
		strcpy(dirName, openName);
		len = strlen(openName);
//  MyMessage(main_hWnd, "�t�@�C���w�� (%s) ����", openName);
        SetDlgItemText(hWnd, IDC_SCH_NAME, openName);
        memset(tcpCtrl.schFileName, 0x00, sizeof(tcpCtrl.schFileName));
        memcpy(tcpCtrl.schFileName, openName, strlen(openName));

        MyMessage(main_hWnd, "");
        MyMessage(main_hWnd, "*****  ���ޭ�يǗ�̧�ف@�Ǎ��� ***********************************************");
        ScheduleDataSet(main_hWnd);
        MyMessage(main_hWnd, "******************************************************************************");
	}

}

/*--------------------------------------------------------------
* �V�i���I�t�@�C���ǂݍ��ݏ���
*--------------------------------------------------------------*/
void getSinaFileName(HWND hWnd)
{
	OPENFILENAME	ofn;
	unsigned char   dirName[256];
	int             len;
	unsigned char   openName[256];

	memset(openName, 0x00, sizeof(openName));
	memset(&ofn, 0, sizeof(OPENFILENAME));
	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = hWnd;
	// �t�@�C����ނ̎w��
	ofn.lpstrFilter = "�ް�̧�ف@  {*.txt}\0*.txt\0"\
					  "  C ̧��     {*.c}\0*.c; *.cpp; *.h\0"\
					  "�S�Ă�̧�� {*.*}\0*.*\0\0";

	ofn.lpstrFile	= openName;
	ofn.nMaxFile = MAX_PATH;
	ofn.Flags = OFN_FILEMUSTEXIST;
//	ofn.Flags = OFN_FILEMUSTEXIST | OFN_ALLOWMULTISELECT | OFN_LONGNAMES | OFN_EXPLORER;
	// �g���q�����͂���Ȃ�������̫�Ă̊g���q
	ofn.lpstrDefExt = "txt";
	if(GetOpenFileName(&ofn)){
		memset(dirName, 0x00, sizeof(dirName));
		strcpy(dirName, openName);
		len = strlen(openName);
//  MyMessage(main_hWnd, "�t�@�C���w�� (%s) ����", openName);
        SetDlgItemText(hWnd, IDC_SINA_NAME, openName);
        memset(tcpCtrl.sinaFileName, 0x00, sizeof(tcpCtrl.sinaFileName));
        memcpy(tcpCtrl.sinaFileName, openName, strlen(openName));

        MyMessage(main_hWnd, "");
        MyMessage(main_hWnd, "*****  ��ص�Ǘ�̧�ف@�Ǎ��� ***********************************************");
        SinaDataSet(main_hWnd);
        MyMessage(main_hWnd, "******************************************************************************");
	}
    tcpCtrl.sinaCtrl = 0;
	SendDlgItemMessage(hWnd, IDC_SINA_CHECK, BM_SETCHECK, tcpCtrl.sinaCtrl, 0);
    timerResetProc1(main_hWnd);
}


/*--------------------------------------------------------------
* �G�f�B�^�[�ǂݍ��ݏ���
*--------------------------------------------------------------*/
void getEditName(HWND hWnd)
{
	OPENFILENAME	ofn;
	unsigned char   dirName[256];
	int             len;
	unsigned char   openName[256];

	memset(openName, 0x00, sizeof(openName));
	memset(&ofn, 0, sizeof(OPENFILENAME));
	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = hWnd;
	// �t�@�C����ނ̎w��
	ofn.lpstrFilter = "���ş�ف@  {*.exe}\0*.exe\0"\
					  "�S�Ă�̧�� {*.*}\0*.*\0\0";

	ofn.lpstrFile	= openName;
	ofn.nMaxFile = MAX_PATH;
	ofn.Flags = OFN_FILEMUSTEXIST;
//	ofn.Flags = OFN_FILEMUSTEXIST | OFN_ALLOWMULTISELECT | OFN_LONGNAMES | OFN_EXPLORER;
	// �g���q�����͂���Ȃ�������̫�Ă̊g���q
	ofn.lpstrDefExt = "exe";
	if(GetOpenFileName(&ofn)){
		memset(dirName, 0x00, sizeof(dirName));
		strcpy(dirName, openName);
		len = strlen(openName);
//  MyMessage(main_hWnd, "�t�@�C���w�� (%s) ����", openName);
        SetDlgItemText(hWnd, IDC_EDIT_NAME, openName);
        memset(tcpCtrl.editName, 0x00, sizeof(tcpCtrl.editName));
        memcpy(tcpCtrl.editName, openName, strlen(openName));

        MyMessage(main_hWnd, "");
        MyMessage(main_hWnd, "��ި�̧�ف@�Ǎ��� [%s]", tcpCtrl.editName);
	}
}

/*--------------------------------------------------------------
* �J�E���^�t�@�C���ǂݍ��ݏ���
*--------------------------------------------------------------*/
void getCountName(HWND hWnd)
{
	OPENFILENAME	ofn;
	unsigned char   dirName[256];
	int             len;
	unsigned char   openName[256];

	memset(openName, 0x00, sizeof(openName));
	memset(&ofn, 0, sizeof(OPENFILENAME));
	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = hWnd;
	// �t�@�C����ނ̎w��
	ofn.lpstrFilter = "����̧�ف@  {*.cnt}\0*.cnt\0"\
					  "�S�Ă�̧�� {*.*}\0*.*\0\0";

	ofn.lpstrFile	= openName;
	ofn.nMaxFile = MAX_PATH;
	ofn.Flags = OFN_LONGNAMES | OFN_EXPLORER;
//	ofn.Flags = OFN_FILEMUSTEXIST | OFN_ALLOWMULTISELECT | OFN_LONGNAMES | OFN_EXPLORER;
	// �g���q�����͂���Ȃ�������̫�Ă̊g���q
	ofn.lpstrDefExt = "cnt";
	if(GetOpenFileName(&ofn)){
		memset(dirName, 0x00, sizeof(dirName));
		strcpy(dirName, openName);
		len = strlen(openName);
//  MyMessage(main_hWnd, "�t�@�C���w�� (%s) ����", openName);
        SetDlgItemText(hWnd, IDC_COUNT_NAME, openName);
        memset(tcpCtrl.counterFile, 0x00, sizeof(tcpCtrl.counterFile));
        memcpy(tcpCtrl.counterFile, openName, strlen(openName));

        MyMessage(main_hWnd, "");
        MyMessage(main_hWnd, "����̧�ف@�Ǎ��� [%s]", tcpCtrl.counterFile);
	}
}



/*--------------------------------------------------------------
* �g���[�X�i�[�t�@�C���w�菈��
*--------------------------------------------------------------*/
void getTraceName(HWND hWnd)
{
	OPENFILENAME	ofn;
	unsigned char   dirName[256];
	int             len;
	unsigned char   openName[256];

	memset(openName, 0x00, sizeof(openName));
	memset(&ofn, 0, sizeof(OPENFILENAME));
	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = hWnd;
	// �t�@�C����ނ̎w��
	ofn.lpstrFilter = "�ڰ�̧�ف@ {*.log}\0*.log\0"\
					  "�S�Ă�̧�� {*.*}\0*.*\0\0";

	ofn.lpstrFile	= openName;
	ofn.nMaxFile = MAX_PATH;
	ofn.Flags = OFN_LONGNAMES | OFN_EXPLORER;
//	ofn.Flags = OFN_FILEMUSTEXIST;
//	ofn.Flags = OFN_FILEMUSTEXIST | OFN_ALLOWMULTISELECT | OFN_LONGNAMES | OFN_EXPLORER;
	// �g���q�����͂���Ȃ�������̫�Ă̊g���q
	ofn.lpstrDefExt = "log";
	if(GetOpenFileName(&ofn)){
		memset(dirName, 0x00, sizeof(dirName));
		strcpy(dirName, openName);
		len = strlen(openName);
//  MyMessage(main_hWnd, "�t�@�C���w�� (%s) ����", openName);
        SetDlgItemText(hWnd, IDC_EDIT1_TRACE, openName);
        memset(tcpCtrl.traceName, 0x00, sizeof(tcpCtrl.traceName));
        memcpy(tcpCtrl.traceName, openName, strlen(openName));

        MyMessage(main_hWnd, "");
        MyMessage(main_hWnd, "�ڰ�̧�ِݒ� [%s]", openName);
	}
}

/*--------------------------------------------------------------
* ��M�f�[�^�i�[�t�@�C���w�菈��
*--------------------------------------------------------------*/
void getRecvName(HWND hWnd)
{
	OPENFILENAME	ofn;
	unsigned char   dirName[256];
	int             len;
	unsigned char   openName[256];

	memset(openName, 0x00, sizeof(openName));
	memset(&ofn, 0, sizeof(OPENFILENAME));

	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = hWnd;
	// �t�@�C����ނ̎w��
	ofn.lpstrFilter = "��M̧�ف@ {*.bin}\0*.bin\0"\
					  "�S�Ă�̧�� {*.*}\0*.*\0\0";

	ofn.lpstrFile	= openName;
	ofn.nMaxFile = MAX_PATH;
	ofn.Flags = OFN_LONGNAMES | OFN_EXPLORER;
//	ofn.Flags = OFN_FILEMUSTEXIST;
//	ofn.Flags = OFN_FILEMUSTEXIST | OFN_ALLOWMULTISELECT | OFN_LONGNAMES | OFN_EXPLORER;
	// �g���q�����͂���Ȃ�������̫�Ă̊g���q
	ofn.lpstrDefExt = "bin";
	if(GetOpenFileName(&ofn)){
		memset(dirName, 0x00, sizeof(dirName));
		strcpy(dirName, openName);
		len = strlen(openName);
//  MyMessage(main_hWnd, "�t�@�C���w�� (%s) ����", openName);
        memset(tcpCtrl.recvFileName, 0x00, sizeof(tcpCtrl.recvFileName));

        SetDlgItemText(hWnd, IDC_RECV_USR_NAME, openName);
        memcpy(tcpCtrl.recvFileName, openName, strlen(openName));

        MyMessage(main_hWnd, "");
        MyMessage(main_hWnd, "��M̧�ِݒ� [%s]", openName);
	}
}

/*--------------------------------------------------------------
* ������M�f�[�^�i�[����
*--------------------------------------------------------------*/
void getAutoRecvName(HWND hWnd)
{
	OPENFILENAME	ofn;
	unsigned char   dirName[256];
	int             len;
	unsigned char   openName[256];
	unsigned char   work[256];
    int             i;

	memset(openName, 0x00, sizeof(openName));
	memset(work, 0x00, sizeof(work));
	memset(tcpCtrl.recvFileBinName, 0x00, sizeof(tcpCtrl.recvFileBinName));
	memset(&ofn, 0, sizeof(OPENFILENAME));

	strcpy(openName, "recv_HHMMSS_MMM.bin");

	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = hWnd;
	// �t�@�C����ނ̎w��
//	ofn.lpstrFilter = "��M̧�ف@ {*.bin}\0*.bin\0"\
//					  "�S�Ă�̧�� {*.*}\0*.*\0\0";
	ofn.lpstrFilter = "��M̧�ف@ {recv_HHMMSS_MMM.bin}\0recv_HHMMSS_MMM.bin\0\0";

	ofn.lpstrFile	= openName;
	ofn.nMaxFile = MAX_PATH;
	ofn.Flags = OFN_LONGNAMES | OFN_EXPLORER;
//	ofn.Flags = OFN_FILEMUSTEXIST;
//	ofn.Flags = OFN_FILEMUSTEXIST | OFN_ALLOWMULTISELECT | OFN_LONGNAMES | OFN_EXPLORER;
	// �g���q�����͂���Ȃ�������̫�Ă̊g���q
	ofn.lpstrDefExt = "bin";
	if(GetOpenFileName(&ofn)){
		memset(dirName, 0x00, sizeof(dirName));
		strcpy(dirName, openName);
		len = strlen(openName);

        MyMessage(main_hWnd, "");
        MyMessage(main_hWnd, "AUTO��M̧�ِݒ� [%s]", openName);

//		memcpy(work, openName, len-strlen("recv_HHMMSS_MMM.bin"));
        for(i=len; i>0; i--) {
            if(openName[i-1] == 0x5c) {
                memcpy(work, openName, i);
                break;
            }
        }
//        MyMessage(main_hWnd, "AUTO��M̧�ِݒ� [%s]", tcpCtrl.recvFileBinName);
        SetDlgItemText(hWnd, IDC_RECV_AUTO_NAME, work);
        memcpy(tcpCtrl.recvFileBinName, work, strlen(work));
	}
}

void clsTraceFileName(HWND hWnd)
{
    if(0 != tcpCtrl.traceSave) {
//       	MessageBox( hWnd, "�g���[�X�i�[�̃`�F�b�N���͂����ĉ�����", "�m�F" ,
//                            MB_ICONEXCLAMATION | MB_OK );
        UserMsgBox(hWnd, "\n�g���[�X�i�[�̃`�F�b�N���͂����ĉ�����", NULL);
        return;
    }
	memset(tcpCtrl.traceName, 0x00, sizeof(tcpCtrl.traceName));

    SetDlgItemText(hWnd, IDC_EDIT1_TRACE, tcpCtrl.traceName);

	return;
}

void clsSinaFileName(HWND hWnd)
{
    if(0 != tcpCtrl.sinaCtrl) {
//       	MessageBox( hWnd, "�V�i���I���s�̃`�F�b�N���͂����ĉ�����", "�m�F" ,
//                            MB_ICONEXCLAMATION | MB_OK );
        UserMsgBox(hWnd, "\n�V�i���I���s�̃`�F�b�N���͂����ĉ�����", NULL);
        return;
    }
    memset(tcpCtrl.sinaFileName, 0x00, sizeof(tcpCtrl.sinaFileName));
	SetDlgItemText(hWnd, IDC_SINA_NAME, tcpCtrl.sinaFileName);

	return;
}

void clsSchFileName(HWND hWnd)
{
    if(0 != tcpCtrl.schCtrl || 0 != tcpCtrl.cycCtrl) {
//       	MessageBox(hWnd, "��M�^������X�P�W���[�����s�̃`�F�b�N���͂����ĉ�����", "�m�F" ,
//                            MB_ICONEXCLAMATION | MB_OK );
        UserMsgBox(hWnd, "��M�^������X�P�W���[�����s�̃`�F�b�N\n\n���͂����ĉ�����", NULL);
        return;
    }
    memset(tcpCtrl.schFileName, 0x00, sizeof(tcpCtrl.schFileName));
	SetDlgItemText(hWnd, IDC_SCH_NAME, tcpCtrl.sinaFileName);

    return;
}

void clsRecvFileName(HWND hWnd)
{
	if(0 != tcpCtrl.recvChkFile)  {
//       	MessageBox( hWnd, "��M�f�[�^�i�[�̃`�F�b�N���͂����ĉ�����", "�m�F" ,
//                            MB_ICONEXCLAMATION | MB_OK );
        UserMsgBox(hWnd, "\n��M�f�[�^�i�[�̃`�F�b�N���͂����ĉ�����", NULL);
        return;
    }

	memset(tcpCtrl.recvFileBinName, 0x00, sizeof(tcpCtrl.recvFileBinName));
	memset(tcpCtrl.recvFileName, 0x00, sizeof(tcpCtrl.recvFileName));

    SetDlgItemText(hWnd, IDC_RECV_AUTO_NAME, tcpCtrl.recvFileBinName);
    SetDlgItemText(hWnd, IDC_RECV_USR_NAME, tcpCtrl.recvFileName);

	return;
}

//*******************************************************************************
//
//*******************************************************************************
BOOL CALLBACK DlgProc2(HWND hWnd, UINT uMessage, WPARAM wParam, LPARAM lParam)
{

	switch (uMessage) {
	case WM_INITDIALOG:								// �����\��
        break;
    case WM_COMMAND :
		switch (LOWORD(wParam)) {
        case IDOK :									// �I������
        case IDCANCEL :								// ��ݾ� & �I��
            EndDialog(hWnd, 0);
			break;
        }
	case WM_DESTROY :								// �I��
		break;
	default :
		return FALSE;
	}
	return TRUE;
}

//*******************************************************************************
//
//*******************************************************************************
BOOL CALLBACK DlgProc3(HWND hWnd, UINT uMessage, WPARAM wParam, LPARAM lParam)
{
	HICON	hicon;

	switch (uMessage) {
	case WM_INITDIALOG:								// �����\��
		hicon = LoadIcon(hThisInst, MAKEINTRESOURCE(IDI_ICON1));
		SendMessage( hWnd, WM_SETICON, TRUE, (LPARAM)hicon);
        if(NULL != g_user_title) {
           	SetWindowText(hWnd,   g_user_title);
        }
    	SetDlgItemText(hWnd, IDC_MSGBOX_TEXT, g_user_msg);
        break;
    case WM_COMMAND :
		switch (LOWORD(wParam)) {
        case IDOK :									// �I������
        case IDCANCEL :								// ��ݾ� & �I��
			EndDialog(hWnd, 0);
			break;
        }
	case WM_DESTROY :								// �I��
		break;
	default :
		return FALSE;
	}
	return TRUE;
}
void    schEditBtn(HWND hWnd, char* fname)
{
	STARTUPINFO si;
	PROCESS_INFORMATION pi;
	char	ebuff[1024];
    struct	_stat	buf;

	memset(ebuff, 0x00, sizeof(ebuff));

	if(0x00 == fname[0] || 0x20 == fname[0]) {
		return;
	}
	if(_stat(fname, &buf) != 0) {
		return;
	}

	if(_stat(tcpCtrl.editName, &buf) == 0) {
		sprintf(ebuff, "%s \"%s\"", tcpCtrl.editName, fname);
	} else {
		sprintf(ebuff, "notepad \"%s\"", fname);
	}

    ZeroMemory(&si, sizeof(si));
    ZeroMemory(&pi, sizeof(pi));
    si.cb = sizeof(si);
	GetStartupInfo(&si);
	CreateProcess(NULL, ebuff,
		NULL, NULL, FALSE, 0,
		NULL, NULL, &si, &pi);
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);

	return;
}

void    schReReadBtn(HWND hWnd)
{
    struct	_stat	buf;

	if(0x00 == tcpCtrl.schFileName[0] || 0x20 == tcpCtrl.schFileName[0]) {
		return;
	}
	if(_stat(tcpCtrl.schFileName, &buf) != 0) {
		return;
	}
    MyMessage(main_hWnd, "");
    MyMessage(main_hWnd, "*****  ���ޭ�يǗ�̧�ف@�ēǍ��� ***********************************************");
    ScheduleDataSet(main_hWnd);
    MyMessage(main_hWnd, "******************************************************************************");

	return;
}

void    sinaReReadBtn(HWND hWnd)
{
    struct	_stat	buf;

	if(0x00 == tcpCtrl.sinaFileName[0] || 0x20 == tcpCtrl.sinaFileName[0]) {
		return;
	}
	if(_stat(tcpCtrl.sinaFileName, &buf) != 0) {
		return;
	}
    MyMessage(main_hWnd, "");
    MyMessage(main_hWnd, "*****  ��ص�Ǘ�̧�ف@�ēǍ��� ************************************************");
    SinaDataSet(main_hWnd);
    MyMessage(main_hWnd, "******************************************************************************");

    tcpCtrl.sinaCtrl = 0;
	SendDlgItemMessage(hWnd, IDC_SINA_CHECK, BM_SETCHECK, tcpCtrl.sinaCtrl, 0);
    timerResetProc1(main_hWnd);
	MyMessage(main_hWnd, "<<<<<  ��ص��ϰ�Đݒ� >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>");

	return;
}

void    typeReadBtn(HWND hWnd)
{
	STARTUPINFO si;
	PROCESS_INFORMATION pi;
	char	ebuff[1024];
    struct	_stat	buf;
    char    fullName[256];

	memset(ebuff, 0x00, sizeof(ebuff));

    MakeFullName(hWnd, "�`�����ߐ���.txt", fullName);
    MyMessage(main_hWnd, fullName);

	if(_stat(fullName, &buf) != 0) {
		return;
	}

	if(_stat(tcpCtrl.editName, &buf) == 0) {
		sprintf(ebuff, "%s \"%s\"", tcpCtrl.editName, fullName);
	} else {
		sprintf(ebuff, "notepad \"%s\"", fullName);
	}

    ZeroMemory(&si, sizeof(si));
    ZeroMemory(&pi, sizeof(pi));
    si.cb = sizeof(si);
	GetStartupInfo(&si);
	CreateProcess(NULL, ebuff,
		NULL, NULL, FALSE, 0,
		NULL, NULL, &si, &pi);
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);

	return;
}

BOOL CALLBACK DlgProc6 ( HWND hWnd, UINT uMessage, WPARAM wParam, LPARAM lParam ) {

    char      cinput_str[10];
    int       input_dat;

    switch (uMessage) {
    case WM_INITDIALOG:                /* �����\���v�� */
                                       /* �����R�[�h */
        if(tcpCtrl.eb_mojicode == _ASCII) {
            SendDlgItemMessage ( hWnd, IDC_RADIO3_ASC, BM_SETCHECK, 1, 0 );
            SendDlgItemMessage ( hWnd, IDC_RADIO3_EBC, BM_SETCHECK, 0, 0 );
        } else {
            SendDlgItemMessage ( hWnd, IDC_RADIO3_ASC, BM_SETCHECK, 0, 0 );
            SendDlgItemMessage ( hWnd, IDC_RADIO3_EBC, BM_SETCHECK, 1, 0 );
        }
                                       /* ���߁^�񓧉� */
        if ( tcpCtrl.eb_transpar == EB_NOTTRNS ) {
            SendDlgItemMessage ( hWnd, IDC_RADIO3_NTRN, BM_SETCHECK, 1, 0 );
            SendDlgItemMessage ( hWnd, IDC_RADIO3__TRN, BM_SETCHECK, 0, 0 );
        } else {
            SendDlgItemMessage ( hWnd, IDC_RADIO3_NTRN, BM_SETCHECK, 0, 0 );
            SendDlgItemMessage ( hWnd, IDC_RADIO3__TRN, BM_SETCHECK, 1, 0 );
        }
                                       /* ACK1�҂����� */
        _itoa ( tcpCtrl.tm_ak1_atxt, cinput_str, 10 );
        SetDlgItemText ( hWnd, IDC_EDIT_EB00, cinput_str );
                                       /* ACK0�҂����� */
        _itoa ( tcpCtrl.tm_ak0_aenq, cinput_str, 10 );
        SetDlgItemText ( hWnd, IDC_EDIT_EB01, cinput_str );
                                       /* ÷�đ҂����� */
        _itoa ( tcpCtrl.tm_txt_aak0, cinput_str, 10 );
        SetDlgItemText ( hWnd, IDC_EDIT_EB02, cinput_str );
                                       /* EOT�҂����� */
        _itoa ( tcpCtrl.tm_eot_aak1, cinput_str, 10 );
        SetDlgItemText ( hWnd, IDC_EDIT_EB03, cinput_str );
                                       /* ENQ�đ��҂����� */
        _itoa ( tcpCtrl.tm_enq_anak, cinput_str, 10 );
        SetDlgItemText ( hWnd, IDC_EDIT_EB04, cinput_str );
                                       /* ENQ���M�� */
        _itoa ( tcpCtrl.nm_enq_nans, cinput_str, 10 );
        SetDlgItemText ( hWnd, IDC_EDIT_EB05, cinput_str );
                                       /* ÷�đ��M�� */
        _itoa ( tcpCtrl.nm_txt_anak, cinput_str, 10 );
        SetDlgItemText ( hWnd, IDC_EDIT_EB06, cinput_str );
        break;

    case WM_COMMAND:
        switch (LOWORD(wParam)) {
        case IDC_SETEB:                /* �ݒ�v�� */
                                       /* �����R�[�h */
            if ( SendDlgItemMessage ( hWnd, IDC_RADIO3_ASC, BM_GETCHECK, 0, 0 ) ) {
                tcpCtrl.eb_mojicode = _ASCII;
            } else {
                tcpCtrl.eb_mojicode = _EBCDIC;
            }
                                       /* ���߁^�񓧉� */
            if ( SendDlgItemMessage ( hWnd, IDC_RADIO3_NTRN, BM_GETCHECK, 0, 0 ) ) {
                tcpCtrl.eb_transpar = EB_NOTTRNS;
            } else {
                tcpCtrl.eb_transpar = EB____TRNS;
            }
                                       /* ACK1�҂����� */
            memset ( cinput_str, '\0', sizeof ( cinput_str ) );
            if ( GetDlgItemText ( hWnd, IDC_EDIT_EB00, cinput_str, sizeof ( cinput_str ) ) ) {
                if ( strlen ( cinput_str ) > IBCLMX_TIM ) {
                    MyMessage ( main_hWnd, "LINE:%d ACK1���ԏ㉺���G���[�F�����l=%d���r�d�s", __LINE__, INIT_TMAK1 );
                    tcpCtrl.tm_ak1_atxt = INIT_TMAK1;
                    _itoa ( tcpCtrl.tm_ak1_atxt, cinput_str, 10 );
                    SetDlgItemText ( hWnd, IDC_EDIT_EB00, cinput_str );
                } else {
                    input_dat = atoi ( cinput_str );
                    if ( input_dat < IBCDMN_TIM || IBCDMX_TIM < input_dat ) {
                        MyMessage ( main_hWnd, "LINE:%d ACK1���ԏ㉺���G���[�F�����l=%d���r�d�s", __LINE__, INIT_TMAK1 );
                        tcpCtrl.tm_ak1_atxt = INIT_TMAK1;
                        _itoa ( tcpCtrl.tm_ak1_atxt, cinput_str, 10 );
                        SetDlgItemText ( hWnd, IDC_EDIT_EB00, cinput_str );
                    } else {
                        tcpCtrl.tm_ak1_atxt = input_dat;
                    }
                }
            }
                                       /* ACK0�҂����� */
            memset ( cinput_str, '\0', sizeof ( cinput_str ) );
            if ( GetDlgItemText ( hWnd, IDC_EDIT_EB01, cinput_str, sizeof ( cinput_str ) ) ) {
                if ( strlen ( cinput_str ) > IBCLMX_TIM ) {
                    MyMessage ( main_hWnd, "LINE:%d ACK0���ԏ㉺���G���[�F�����l=%d���r�d�s", __LINE__, INIT_TMAK0 );
                    tcpCtrl.tm_ak0_aenq = INIT_TMAK0;
                    _itoa ( tcpCtrl.tm_ak0_aenq, cinput_str, 10 );
                    SetDlgItemText ( hWnd, IDC_EDIT_EB01, cinput_str );
                } else {
                    input_dat = atoi ( cinput_str );
                    if ( input_dat < IBCDMN_TIM || IBCDMX_TIM < input_dat ) {
                        MyMessage ( main_hWnd, "LINE:%d ACK0���ԏ㉺���G���[�F�����l=%d���r�d�s", __LINE__, INIT_TMAK0 );
                        tcpCtrl.tm_ak0_aenq = INIT_TMAK0;
                        _itoa ( tcpCtrl.tm_ak0_aenq, cinput_str, 10 );
                        SetDlgItemText ( hWnd, IDC_EDIT_EB01, cinput_str );
                    } else {
                        tcpCtrl.tm_ak0_aenq = input_dat;
                    }
                }
            }
                                       /* ÷�đ҂����� */
            memset ( cinput_str, '\0', sizeof ( cinput_str ) );
            if ( GetDlgItemText ( hWnd, IDC_EDIT_EB02, cinput_str, sizeof ( cinput_str ) ) ) {
                if ( strlen ( cinput_str ) > IBCLMX_TIM ) {
                    MyMessage ( main_hWnd, "LINE:%d ÷�Ď��ԏ㉺���G���[�F�����l=%d���r�d�s", __LINE__, INIT_TMTXT );
                    tcpCtrl.tm_txt_aak0 = INIT_TMTXT;
                    _itoa ( tcpCtrl.tm_txt_aak0, cinput_str, 10 );
                    SetDlgItemText ( hWnd, IDC_EDIT_EB02, cinput_str );
                } else {
                    input_dat = atoi ( cinput_str );
                    if ( input_dat < IBCDMN_TIM || IBCDMX_TIM < input_dat ) {
                        MyMessage ( main_hWnd, "LINE:%d ÷�Ď��ԏ㉺���G���[�F�����l=%d���r�d�s", __LINE__, INIT_TMTXT );
                        tcpCtrl.tm_txt_aak0 = INIT_TMTXT;
                        _itoa ( tcpCtrl.tm_txt_aak0, cinput_str, 10 );
                        SetDlgItemText ( hWnd, IDC_EDIT_EB02, cinput_str );
                    } else {
                        tcpCtrl.tm_txt_aak0 = input_dat;
                    }
                }
            }
                                       /* EOT�҂����� */
            memset ( cinput_str, '\0', sizeof ( cinput_str ) );
            if ( GetDlgItemText ( hWnd, IDC_EDIT_EB03, cinput_str, sizeof ( cinput_str ) ) ) {
                if ( strlen ( cinput_str ) > IBCLMX_TIM ) {
                    MyMessage ( main_hWnd, "LINE:%d EOT���ԏ㉺���G���[�F�����l=%d���r�d�s", __LINE__, INIT_TMEOT );
                    tcpCtrl.tm_eot_aak1 = INIT_TMEOT;
                    _itoa ( tcpCtrl.tm_eot_aak1, cinput_str, 10 );
                    SetDlgItemText ( hWnd, IDC_EDIT_EB03, cinput_str );
                } else {
                    input_dat = atoi ( cinput_str );
                    if ( input_dat < IBCDMN_TIM || IBCDMX_TIM < input_dat ) {
                        MyMessage ( main_hWnd, "LINE:%d EOT���ԏ㉺���G���[�F�����l=%d���r�d�s", __LINE__, INIT_TMEOT );
                        tcpCtrl.tm_eot_aak1 = INIT_TMEOT;
                        _itoa ( tcpCtrl.tm_eot_aak1, cinput_str, 10 );
                        SetDlgItemText ( hWnd, IDC_EDIT_EB03, cinput_str );
                    } else {
                        tcpCtrl.tm_eot_aak1 = input_dat;
                    }
                }
            }
                                       /* ENQ�đ��҂����� */
            memset ( cinput_str, '\0', sizeof ( cinput_str ) );
            if ( GetDlgItemText ( hWnd, IDC_EDIT_EB04, cinput_str, sizeof ( cinput_str ) ) ) {
                if ( strlen ( cinput_str ) > IBCLMX_TIM ) {
                    MyMessage ( main_hWnd, "LINE:%d ENQ�đ��҂����ԏ㉺���G���[�F�����l=%d���r�d�s", __LINE__, INIT_TMENQ );
                    tcpCtrl.tm_enq_anak = INIT_TMENQ;
                    _itoa ( tcpCtrl.tm_enq_anak, cinput_str, 10 );
                    SetDlgItemText ( hWnd, IDC_EDIT_EB04, cinput_str );
                } else {
                    input_dat = atoi ( cinput_str );
                    if ( input_dat < IBCDMN_TIM || IBCDMX_TIM < input_dat ) {
                        MyMessage ( main_hWnd, "LINE:%d ENQ�đ��҂����ԏ㉺���G���[�F�����l=%d���r�d�s", __LINE__, INIT_TMENQ );
                        tcpCtrl.tm_enq_anak = INIT_TMENQ;
                        _itoa ( tcpCtrl.tm_enq_anak, cinput_str, 10 );
                        SetDlgItemText ( hWnd, IDC_EDIT_EB04, cinput_str );
                    } else {
                        tcpCtrl.tm_enq_anak = input_dat;
                    }
                }
            }
                                       /* ENQ���M�� */
            memset ( cinput_str, '\0', sizeof ( cinput_str ) );
            if ( GetDlgItemText ( hWnd, IDC_EDIT_EB05, cinput_str, sizeof ( cinput_str ) ) ) {
                if ( strlen ( cinput_str ) > IBCLMX_NUM ) {
                    MyMessage ( main_hWnd, "LINE:%d ENQ���M�񐔏㉺���G���[�F�����l=%d���r�d�s", __LINE__, INIT_NMENQ );
                    tcpCtrl.nm_enq_nans = INIT_NMENQ;
                    _itoa ( tcpCtrl.nm_enq_nans, cinput_str, 10 );
                    SetDlgItemText ( hWnd, IDC_EDIT_EB05, cinput_str );
                } else {
                    input_dat = atoi ( cinput_str );
                    if ( input_dat < IBCDMN_NUM || IBCDMX_NUM < input_dat ) {
                        MyMessage ( main_hWnd, "LINE:%d ENQ���M�񐔏㉺���G���[�F�����l=%d���r�d�s", __LINE__, INIT_NMENQ );
                        tcpCtrl.nm_enq_nans = INIT_NMENQ;
                        _itoa ( tcpCtrl.nm_enq_nans, cinput_str, 10 );
                        SetDlgItemText ( hWnd, IDC_EDIT_EB05, cinput_str );
                    } else {
                        tcpCtrl.nm_enq_nans = input_dat;
                    }
                }
            }
                                       /* ÷�đ��M�� */
            memset ( cinput_str, '\0', sizeof ( cinput_str ) );
            if ( GetDlgItemText ( hWnd, IDC_EDIT_EB06, cinput_str, sizeof ( cinput_str ) ) ) {
                if ( strlen ( cinput_str ) > IBCLMX_NUM ) {
                    MyMessage ( main_hWnd, "LINE:%d ÷�đ��M�񐔏㉺���G���[�F�����l=%d���r�d�s", __LINE__, INIT_NMTXT );
                    tcpCtrl.nm_txt_anak = INIT_NMTXT;
                    _itoa ( tcpCtrl.nm_txt_anak, cinput_str, 10 );
                    SetDlgItemText ( hWnd, IDC_EDIT_EB06, cinput_str );
                } else {
                    input_dat = atoi ( cinput_str );
                    if ( input_dat < IBCDMN_NUM || IBCDMX_NUM < input_dat ) {
                        MyMessage ( main_hWnd, "LINE:%d ÷�đ��M�񐔏㉺���G���[�F�����l=%d���r�d�s", __LINE__, INIT_NMTXT );
                        tcpCtrl.nm_txt_anak = INIT_NMTXT;
                        _itoa ( tcpCtrl.nm_txt_anak, cinput_str, 10 );
                        SetDlgItemText ( hWnd, IDC_EDIT_EB06, cinput_str );
                    } else {
                        tcpCtrl.nm_txt_anak = input_dat;
                    }
                }
            }
            MyMessage ( main_hWnd, "LINE:%d Enet-B�ݒ�l�ύX����", __LINE__ );
            break;

        case IDCANCEL3:                /* �I���v�� */
            EndDialog ( hWnd, 0 );
            break;

        default:
            return FALSE;
        }

        break;

    default:
        return FALSE;
    }

    return TRUE;
}
