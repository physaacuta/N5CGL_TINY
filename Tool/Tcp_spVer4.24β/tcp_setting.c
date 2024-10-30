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

#define		WM_USER_ASYNC_SELECT	(WM_USER + 1)		// ユーザ定義メッセージ
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
	case WM_INITDIALOG:								// 初期表示
    case WM_USER_ASYNC_SELECT+LOAD:
        setting_hWnd = hWnd;
		if(0 != g_flag) {
			EnableWindow(GetDlgItem(hWnd, IDC_CTRL_LOAD), FALSE);
			EnableWindow(GetDlgItem(hWnd, IDC_CTRL_SAVE), FALSE);
//			EnableWindow(GetDlgItem(hWnd, IDC_SCH_READ_BTN), FALSE);
		}

        // トレース出力コード指定
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

		// トレース出力チェックＢＯＸ
		SendDlgItemMessage(hWnd, IDC_CHECK1_SEND, BM_SETCHECK, tcpCtrl.traceOutSend, 0);
		SendDlgItemMessage(hWnd, IDC_CHECK1_RECV, BM_SETCHECK, tcpCtrl.traceOutRecv, 0);

		// ﾌｧｲﾙｾｰﾌﾞの表示
		SendDlgItemMessage(hWnd, IDC_CHECK2_TRACE, BM_SETCHECK, tcpCtrl.traceSave, 0);

		SetDlgItemText(hWnd, IDC_EDIT1_TRACE, tcpCtrl.traceName);
		if(tcpCtrl.traceSave == 0) {
			EnableWindow(GetDlgItem(hWnd, IDC_EDIT1_TRACE), FALSE);
        } else {
			EnableWindow(GetDlgItem(hWnd, IDC_EDIT1_TRACE), TRUE);
        }

		// 受信データファイル格納
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
            // ﾌｧｲﾙ名指定がﾁｪｯｸONの時
			SendDlgItemMessage(hWnd, IDC_RECV_USR_FILE, BM_SETCHECK, 1, 0); 
			EnableWindow(GetDlgItem(hWnd, IDC_RECV_USR_NAME), TRUE);
			EnableWindow(GetDlgItem(hWnd, IDC_RECV_AUTO_NAME), FALSE);
            EnableWindow(GetDlgItem(hWnd, IDC_STATIC_DIR), FALSE);
		} else {
            // 日時による自動格納がﾁｪｯｸONの時
			SendDlgItemMessage(hWnd, IDC_RECV_AUTO_FILE, BM_SETCHECK, 1, 0);
			EnableWindow(GetDlgItem(hWnd, IDC_RECV_USR_NAME), FALSE);
			EnableWindow(GetDlgItem(hWnd, IDC_RECV_AUTO_NAME), TRUE);
            EnableWindow(GetDlgItem(hWnd, IDC_STATIC_DIR), TRUE);
		}
        // スケジュール管理有無
        SetDlgItemText(hWnd, IDC_SCH_NAME, tcpCtrl.schFileName);
   		SendDlgItemMessage(hWnd, IDC_SCH_CHECK, BM_SETCHECK, tcpCtrl.schCtrl, 0);
        // 定周期管理有無
   		SendDlgItemMessage(hWnd, IDC_CYC_CHECK, BM_SETCHECK, tcpCtrl.cycCtrl, 0);
		//
		SetDlgItemText(hWnd, IDC_EDIT_NAME, tcpCtrl.editName);
		SetDlgItemText(hWnd, IDC_COUNT_NAME, tcpCtrl.counterFile);
        // シナリオ管理有無
        SetDlgItemText(hWnd, IDC_SINA_NAME, tcpCtrl.sinaFileName);
   		SendDlgItemMessage(hWnd, IDC_SINA_CHECK, BM_SETCHECK, tcpCtrl.sinaCtrl, 0);

        // 永久リトライ指定
   		SendDlgItemMessage(hWnd, IDC_RETRY_CHECK, BM_SETCHECK, tcpCtrl.retry_flg, 0);
   		
   		// Ｅｎｅｔ－Ｂ設定ボタン有効／無効指定
   		if ( ENET_B == tcpCtrl.comType || ENET_BX == tcpCtrl.comType ) {
   			EnableWindow ( GetDlgItem ( hWnd, IDC_ENETB ), TRUE );
   		} else {
   			EnableWindow ( GetDlgItem ( hWnd, IDC_ENETB ), FALSE );
   		}

        break;

    case WM_COMMAND :
		switch (LOWORD(wParam)) {
		/////////////////////////////////////////
		// ファイルセーブチェックＢＯＸ
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
		// トレース出力送信チェックＢＯＸ
		case IDC_CHECK1_SEND:
			if(tcpCtrl.traceOutSend == 1) {
				tcpCtrl.traceOutSend = 0;
			} else {
				tcpCtrl.traceOutSend = 1;
			}
			break;

		/////////////////////////////////////////
		// トレース出力受信チェックＢＯＸ
		case IDC_CHECK1_RECV:
			if(tcpCtrl.traceOutRecv == 1) {
				tcpCtrl.traceOutRecv = 0;
			} else {
				tcpCtrl.traceOutRecv = 1;
			}
			break;

		/////////////////////////////////////////
		// 受信データファイル格納
		case IDC_RECV_FILE:
			if(tcpCtrl.recvChkFile == 1) {
                // 受信ﾃﾞｰﾀをﾌｧｲﾙ格納するがﾁｪｯｸON->OFFの時
				tcpCtrl.recvChkFile = 0;
			} else {
                // 受信ﾃﾞｰﾀをﾌｧｲﾙ格納するがﾁｪｯｸOFF->ONの時
				tcpCtrl.recvChkFile = 1;
			}
			// 格納ﾌｧｲﾙ名が選択されているかﾁｪｯｸする
			if(FALSE == ChkRecvSave(hWnd)) {
				tcpCtrl.recvChkFile = 0;
				SendDlgItemMessage(hWnd, IDC_RECV_FILE, BM_SETCHECK, 0, 0); 
		        break;
			}
			break;

		/////////////////////////////////////////
		// 受信ﾃﾞｰﾀﾌｧｲﾙ格納の自動格納がﾁｪｯｸON
        //　(tcpCtrl.recvChkFileName=0:自動設定)
		case IDC_RECV_AUTO_FILE:
			if(tcpCtrl.recvChkFileName != 0) {
				tcpCtrl.recvChkFileName = 0;
				EnableWindow(GetDlgItem(hWnd, IDC_RECV_USR_NAME), FALSE);
				EnableWindow(GetDlgItem(hWnd, IDC_RECV_AUTO_NAME), TRUE);
           		EnableWindow(GetDlgItem(hWnd, IDC_STATIC_DIR), TRUE);
			}
			break;

		/////////////////////////////////////////
		// 受信ﾃﾞｰﾀﾌｧｲﾙ格納のﾌｧｲﾙ名指定がﾁｪｯｸON
        //　(tcpCtrl.recvChkFileName=1:ﾌｧｲﾙ名指定)
		case IDC_RECV_USR_FILE:
			if(tcpCtrl.recvChkFileName != 1) {
				tcpCtrl.recvChkFileName = 1;
				EnableWindow(GetDlgItem(hWnd, IDC_RECV_USR_NAME), TRUE);
				EnableWindow(GetDlgItem(hWnd, IDC_RECV_AUTO_NAME), FALSE);
           		EnableWindow(GetDlgItem(hWnd, IDC_STATIC_DIR), FALSE);
			}
			break;

		/////////////////////////////////////////
		// スケジュール管理有無
        // (tcpCtrl.schCtrl=1:管理する)
        case IDC_SCH_CHECK:
			if(tcpCtrl.schCtrl == 0) {
                if(0x00 == tcpCtrl.schFileName[0]) {
                    UserMsgBox(hWnd, "\nｽｹｼﾞｭｰﾙ管理ﾌｧｲﾙを設定して下さい", NULL);
                    SendDlgItemMessage(hWnd, IDC_SCH_CHECK, BM_SETCHECK, tcpCtrl.schCtrl, 0);
                    break;
                }
				if(_stat(tcpCtrl.schFileName, &buf) < 0) {
		   	        sprintf(MsgBuf, "ｽｹｼﾞｭｰﾙ管理ﾌｧｲﾙ\n\n[%s]\n\nが見つかりません", tcpCtrl.schFileName);
                    MessageBox( hWnd, MsgBuf, "確認" , MB_ICONEXCLAMATION | MB_OK );
                    SendDlgItemMessage(hWnd, IDC_SCH_CHECK, BM_SETCHECK, tcpCtrl.schCtrl, 0);
                } else {
    				tcpCtrl.schCtrl = 1;
                }
			} else {
				tcpCtrl.schCtrl = 0;
			}
			break;

		/////////////////////////////////////////
		// シナリオ管理有無
        // (tcpCtrl.sinaCtrl=1:管理する)
        case IDC_SINA_CHECK:
			if(tcpCtrl.sinaCtrl == 0) {
                if(0x00 == tcpCtrl.sinaFileName[0]) {
                    UserMsgBox(hWnd, "\nｼﾅﾘｵﾌｧｲﾙを設定して下さい", NULL);
               		SendDlgItemMessage(hWnd, IDC_SINA_CHECK, BM_SETCHECK, tcpCtrl.sinaCtrl, 0);
                    break;
                }
				if(_stat(tcpCtrl.sinaFileName, &buf) < 0) {
		   	        sprintf(MsgBuf, "ｼﾅﾘｵﾌｧｲﾙ\n\n[%s]\n\nが見つかりません", tcpCtrl.sinaFileName);
		   	        MessageBox( hWnd, MsgBuf, "確認" , MB_ICONEXCLAMATION | MB_OK );
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
		// 永久リトライ指定
        // (tcpCtrl.retry_flg)
        case IDC_RETRY_CHECK:
			if(tcpCtrl.retry_flg == 0) {
  				tcpCtrl.retry_flg = 1;
			} else {
				tcpCtrl.retry_flg = 0;
			}
			break;

		/////////////////////////////////////////
		// 定周期管理有無
        // (tcpCtrl.cycCtrl=1:管理する)
        case IDC_CYC_CHECK:
			if(tcpCtrl.cycCtrl == 0) {
                if(0x00 == tcpCtrl.schFileName[0]) {
                    UserMsgBox(hWnd, "\nｽｹｼﾞｭｰﾙ管理ﾌｧｲﾙを設定して下さい", NULL);
              		SendDlgItemMessage(hWnd, IDC_CYC_CHECK, BM_SETCHECK, tcpCtrl.cycCtrl, 0);
                    break;
                }
				if(_stat(tcpCtrl.schFileName, &buf) < 0) {
		   	        sprintf(MsgBuf, "ｽｹｼﾞｭｰﾙ管理ﾌｧｲﾙ\n\n[%s]\n\nが見つかりません", tcpCtrl.schFileName);
		   	        MessageBox( hWnd, MsgBuf, "確認" , MB_ICONEXCLAMATION | MB_OK );
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
		// 情報ﾎﾞﾀﾝON
		case IDC_VERSION:
            TcpVersion(hWnd);
            break;

        //////////////////////////////////////////////
        // ｽｹｼﾞｭｰﾙ管理ﾌｧｲﾙの読み込みﾎﾞﾀﾝON
		case IDC_SCH_READ_BTN:
            getSchFileName(hWnd);
            break;

        //////////////////////////////////////////////
        // ｼﾅﾘｵ管理ﾌｧｲﾙの読み込みﾎﾞﾀﾝON
		case IDC_SINA_READ_BTN:
            getSinaFileName(hWnd);
            break;

        //////////////////////////////////////////////
        // ｽｹｼﾞｭｰﾙ管理ﾌｧｲﾙの編集ﾎﾞﾀﾝON
		case IDC_SCH_EDIT_BTN:
            schEditBtn(hWnd, tcpCtrl.schFileName);
            break;

		//////////////////////////////////////////////
        // ｼﾅﾘｵ管理ﾌｧｲﾙの編集ﾎﾞﾀﾝON
		case IDC_SINA_EDIT_BTN:
            schEditBtn(hWnd, tcpCtrl.sinaFileName);
            break;

        //////////////////////////////////////////////
        // ｽｹｼﾞｭｰﾙ管理ﾌｧｲﾙの再読込ﾎﾞﾀﾝON
		case IDC_SCH_REREAD_BTN:
            schReReadBtn(hWnd);
            break;

        //////////////////////////////////////////////
        // ｼﾅﾘｵ管理ﾌｧｲﾙの再読込ﾎﾞﾀﾝON
		case IDC_SINA_REREAD_BTN:
            sinaReReadBtn(hWnd);
			break;

        //////////////////////////////////////////////
        // ｼﾅﾘｵ管理ｷｬﾝｾﾙﾎﾞﾀﾝON
//		case IDC_SINA_BTN:
//			timerResetProc1(hWnd);
//            break;

		//////////////////////////////////////////////
        // 受信ｼﾅﾘｵ管理ｷｬﾝｾﾙﾎﾞﾀﾝON
		case IDC_RECV_SINA_BTN:
			timerResetProc2(main_hWnd);
            break;

		//////////////////////////////////////////////
        // 伝送ﾀｲﾌﾟﾌｧｲﾙの参照ﾎﾞﾀﾝON
		case IDC_TYPE_READ_BTN:
            typeReadBtn(hWnd);
            break;

        //////////////////////////////////////////////
        // ｴﾃﾞｨﾀﾌｧｲﾙの読み込みﾎﾞﾀﾝON
		case IDC_EDIT_BTN:
            getEditName(hWnd);
            break;

        //////////////////////////////////////////////
        // ｴﾃﾞｨﾀﾌｧｲﾙ名初期化
		case IDC_EDITCLS_BTN:
	        memset(tcpCtrl.editName, 0x00, sizeof(tcpCtrl.editName));
			SetDlgItemText(hWnd, IDC_EDIT_NAME, tcpCtrl.editName);
            break;

        //////////////////////////////////////////////
        // ｶｳﾝﾀﾌｧｲﾙの読み込みﾎﾞﾀﾝON
		case IDC_COUNT_BTN:
            getCountName(hWnd);
            break;

        //////////////////////////////////////////////
        // ｶｳﾝﾀﾌｧｲﾙ名初期化
		case IDC_COUNTCLS_BTN:
	        memset(tcpCtrl.counterFile, 0x00, sizeof(tcpCtrl.counterFile));
			SetDlgItemText(hWnd, IDC_COUNT_NAME, tcpCtrl.counterFile);
            break;

        //////////////////////////////////////////////
        // ﾄﾚｰｽﾌｧｲﾙ名指定
		case IDC_TRACE_BTN:
            getTraceName(hWnd);
            break;

        //////////////////////////////////////////////
        // 受信ﾃﾞｰﾀ格納ﾌｧｲﾙ名指定
		case IDC_RECV_BTN:
            getRecvName(hWnd);
            break;

        //////////////////////////////////////////////
        // 自動受信ﾃﾞｰﾀ格納ﾃﾞｨﾚｸﾄﾘｨ名指定
		case IDC_AUTO_RECV_BTN:
            getAutoRecvName(hWnd);
            break;

        //////////////////////////////////////////////
        // ﾄﾚｰｽﾃﾞｰﾀ格納ﾌｧｲﾙ名初期化
		case IDC_TRACE_CLS_BTN:
            clsTraceFileName(hWnd);
            break;

        //////////////////////////////////////////////
        // 受信ﾃﾞｰﾀ格納ﾌｧｲﾙ名初期化
		case IDC_RECV_CLS_BTN:
            clsRecvFileName(hWnd);
            break;

        //////////////////////////////////////////////
        // ｼﾅﾘｵﾌｧｲﾙ名初期化
		case IDC_SINA_CLS_BTN:
            clsSinaFileName(hWnd);
            break;


        //////////////////////////////////////////////
        // ｽｹｼﾞｭｰﾙﾌｧｲﾙ名初期化
		case IDC_SCH_CLS_BTN:
            clsSchFileName(hWnd);
            break;

        case IDCANCEL :										// ｷｬﾝｾﾙ & 終了
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
		// ｺﾝﾄﾛｰﾙﾌｧｲﾙ保存
        case IDC_CTRL_SAVE:		    //
            ChkRadioProc(hWnd);
            CtrlFileSave(main_hWnd);
            break;

    	/////////////////////////////////////////
		// ｺﾝﾄﾛｰﾙﾌｧｲﾙ読み込み
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
//            	MessageBox( hWnd, "回線接続中です。読込みできません", "確認" ,
//							MB_ICONEXCLAMATION | MB_OK );
                UserMsgBox(hWnd, "\n回線接続中です。読込みできません", NULL);

            }
            break;

        /////////////////////////////////////////
        // Ｅｎｅｔ－Ｂ設定ボタンON
        case IDC_ENETB:
            TcpEnetb ( hWnd );
            break;

        default :
			return FALSE;
		}
		break;

	case WM_DESTROY :												// 終了
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
//　終了時のラジオボタンの状態を格納する
//
//////////////////////////////////////////////////////////////////////////
BOOL	ChkRadioProc(HWND hWnd)
{
    // トレース出力区分
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
//　終了時のトレース出力ファイル名をグローバルに格納する
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
			// ファイル名入力文字チェック
			sLen = strlen(name);
            // シリアルＮｏチェック
            rtc=check_command(main_hWnd, name, sLen);
            if(1 == rtc) {
                tcpCtrl.traceSave = 0;
				SendDlgItemMessage(hWnd, IDC_CHECK2_TRACE, BM_SETCHECK, 0, 0); 
				memset(tcpCtrl.traceName, 0x00, sizeof(tcpCtrl.traceName));
			    SetDlgItemText(hWnd, IDC_EDIT1_TRACE, tcpCtrl.traceName);
			   	MessageBox( hWnd, "送金ありがとうございます。m(_ _)m  \n\n再度 TcpSp を起動してください！！", "感謝！！(^ﾍ^)v ",
  								MB_ICONEXCLAMATION | MB_OK );
                return TRUE;
            }
			memset(tcpCtrl.traceName, 0x00, sizeof(tcpCtrl.traceName));
			memcpy(tcpCtrl.traceName, name, sLen);
		} else {
//              MessageBox( hWnd, "トレース格納ファイル名を入力して下さい", "確認" , MB_ICONEXCLAMATION | MB_OK );
                UserMsgBox(hWnd, "\nトレース格納ファイル名を設定して下さい", NULL);
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
//　終了時の受信データ格納ファイル名をグローバルに格納する
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
        // ﾌｧｲﾙ名指定の格納時の処理
		if(GetDlgItemText(hWnd, IDC_RECV_USR_NAME, name, sizeof(name))) {
			// ファイル名入力文字チェック
//			sLen = strlen(name);
//			memset(tcpCtrl.recvFileName, 0x00, sizeof(tcpCtrl.recvFileName));
//            memcpy(tcpCtrl.recvFileName, name, sLen);
		} else {
//		   	MessageBox( hWnd, "受信データ格納ファイル名を入力して下さい", "確認" , MB_ICONEXCLAMATION | MB_OK );
            UserMsgBox(hWnd, "\n受信データ格納ファイル名を設定して下さい", NULL);
			return FALSE;
		}
	} else {
        // ﾌｧｲﾙ名自動作成時の処理      
		if(GetDlgItemText(hWnd, IDC_RECV_AUTO_NAME, name, sizeof(name))) {
			// ファイル名入力文字チェック
//			sLen = strlen(name);
//			memset(tcpCtrl.recvFileBinName, 0x00, sizeof(tcpCtrl.recvFileBinName));
//            memcpy(tcpCtrl.recvFileBinName, name, sLen);
		} else {
//		   	MessageBox( hWnd, "受信データ格納ディレクトリィ名を入力して下さい", "確認" , MB_ICONEXCLAMATION | MB_OK );
            UserMsgBox(hWnd, "受信データ格納フォルダを\n\n設定して下さい", NULL);
			return FALSE;
		}
    }
    return TRUE;
}

//////////////////////////////////////////////////////////////////////////
//
//　設定情報を読み込む
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
            MyMessage(hWnd, "ｶﾚﾝﾄﾃﾞｨﾚｸﾄﾘｨが取得できませんでした" );
        }
        MyMessage(hWnd, "ﾎｰﾑﾃﾞｨﾚｸﾄﾘｨ取得 : [%s]", homedir);
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
            // ｽｹｼﾞｭｰﾙ管理ﾌｧｲﾙ　指定なし
        } else {
            MyMessage(hWnd, "*****  ｽｹｼﾞｭｰﾙ管理ﾌｧｲﾙ　読込み ***********************************************");
            ScheduleDataSet(hWnd);
            MyMessage(hWnd, "******************************************************************************");
        }
	    if(0x00 == tcpCtrl.sinaFileName[0] || 0x20 == tcpCtrl.sinaFileName[0]) {
            // ｼﾅﾘｵ管理ﾌｧｲﾙ　指定なし
        } else {
            MyMessage(hWnd, "*****  ｼﾅﾘｵ管理ﾌｧｲﾙ　読込み **************************************************");
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
        // ｺﾝﾄﾛｰﾙﾌｧｲﾙを読込む為に、一時的にｶﾚﾝﾄﾃﾞｨﾚｸﾄﾘｨを設定する。
        if(0 == GetCurrentDirectory(sizeof(tcpCtrl.homeDir), tcpCtrl.homeDir)) {
            MyMessage(hWnd, "ｶﾚﾝﾄﾃﾞｨﾚｸﾄﾘｨが取得できませんでした" );
        }
        // MyMessage(hWnd, "ﾎｰﾑﾃﾞｨﾚｸﾄﾘｨ取得 : [%s]", tcpCtrl.homeDir);
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
            MyMessage ( hWnd, "LINE:%d 文字コードエラー：初期値=EBCDICをＳＥＴ", __LINE__ );
        }
        tcpCtrl.eb_mojicode = INIT_MJCOD;
    }
    if ( tcpCtrl.eb_transpar != EB_NOTTRNS && tcpCtrl.eb_transpar != EB____TRNS ) {
        if( tcpCtrl.comType == ENET_B || tcpCtrl.comType == ENET_BX) {
            MyMessage ( hWnd, "LINE:%d 透過／非透過エラー：初期値=非透過をＳＥＴ", __LINE__ );
        }
        tcpCtrl.eb_transpar = INIT_TRNSP;
    }
    if ( tcpCtrl.tm_ak1_atxt < IBCDMN_TIM || IBCDMX_TIM < tcpCtrl.tm_ak1_atxt ) {
        if( tcpCtrl.comType == ENET_B || tcpCtrl.comType == ENET_BX) {
            MyMessage ( hWnd, "LINE:%d ACK1時間上下限エラー：初期値=%dをＳＥＴ", __LINE__, INIT_TMAK1 );
        }
        tcpCtrl.tm_ak1_atxt = INIT_TMAK1;
    }
    if ( tcpCtrl.tm_ak0_aenq < IBCDMN_TIM || IBCDMX_TIM < tcpCtrl.tm_ak0_aenq ) {
        if( tcpCtrl.comType == ENET_B || tcpCtrl.comType == ENET_BX) {
            MyMessage ( hWnd, "LINE:%d ACK0時間上下限エラー：初期値=%dをＳＥＴ", __LINE__, INIT_TMAK0 );
        }
        tcpCtrl.tm_ak0_aenq = INIT_TMAK0;
    }
    if ( tcpCtrl.tm_txt_aak0 < IBCDMN_TIM || IBCDMX_TIM < tcpCtrl.tm_txt_aak0 ) {
        if( tcpCtrl.comType == ENET_B || tcpCtrl.comType == ENET_BX) {
            MyMessage ( hWnd, "LINE:%d ﾃｷｽﾄ時間上下限エラー：初期値=%dをＳＥＴ", __LINE__, INIT_TMTXT );
        }
        tcpCtrl.tm_txt_aak0 = INIT_TMTXT;
    }
    if ( tcpCtrl.tm_eot_aak1 < IBCDMN_TIM || IBCDMX_TIM < tcpCtrl.tm_eot_aak1 ) {
        if( tcpCtrl.comType == ENET_B || tcpCtrl.comType == ENET_BX) {
            MyMessage ( hWnd, "LINE:%d EOT時間上下限エラー：初期値=%dをＳＥＴ", __LINE__, INIT_TMEOT );
        }
        tcpCtrl.tm_eot_aak1 = INIT_TMEOT;
    }
    if ( tcpCtrl.tm_enq_anak < IBCDMN_TIM || IBCDMX_TIM < tcpCtrl.tm_enq_anak ) {
        if( tcpCtrl.comType == ENET_B || tcpCtrl.comType == ENET_BX) {
            MyMessage ( hWnd, "LINE:%d ENQ再送待ち時間上下限エラー：初期値=%dをＳＥＴ", __LINE__, INIT_TMENQ );
        }
        tcpCtrl.tm_enq_anak = INIT_TMENQ;
    }
    if ( tcpCtrl.nm_enq_nans < IBCDMN_NUM || IBCDMX_NUM < tcpCtrl.nm_enq_nans ) {
        if( tcpCtrl.comType == ENET_B || tcpCtrl.comType == ENET_BX) {
            MyMessage ( hWnd, "LINE:%d ENQ送信回数上下限エラー：初期値=%dをＳＥＴ", __LINE__, INIT_NMENQ );
        }
        tcpCtrl.nm_enq_nans = INIT_NMENQ;
    }
    if ( tcpCtrl.nm_txt_anak < IBCDMN_NUM || IBCDMX_NUM < tcpCtrl.nm_txt_anak ) {
        if( tcpCtrl.comType == ENET_B || tcpCtrl.comType == ENET_BX) {
            MyMessage ( hWnd, "LINE:%d ﾃｷｽﾄ送信回数上下限エラー：初期値=%dをＳＥＴ", __LINE__, INIT_NMTXT );
        }
        tcpCtrl.nm_txt_anak = INIT_NMTXT;
    }

    memset(tcpCtrl.homeDir, 0x00, sizeof(tcpCtrl.homeDir));
    if(0 == GetCurrentDirectory(sizeof(tcpCtrl.homeDir), tcpCtrl.homeDir)) {
        MyMessage(hWnd, "ｶﾚﾝﾄﾃﾞｨﾚｸﾄﾘｨが取得できませんでした" );
    }
    MyMessage(hWnd, "ﾎｰﾑﾃﾞｨﾚｸﾄﾘｨ取得 : [%s]", tcpCtrl.homeDir);
    MyMessage(hWnd, "");

//	setDialogName(hWnd, fullName);

    return TRUE;
}

//////////////////////////////////////////////////////////////////////////
//
//　設定情報を書き込む
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
//　出力ファイル名のディレクトリィを作成する
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

//    MyMessage(main_hWnd, "MakeDir ﾃﾞｨﾚｸﾄﾘｨ名作成 (%s) 完了", tmp);

    for(i=0; (unsigned int)i<strlen(tmp); i++) {
        if(0 != i && '/' == tmp[i]) {
            // 受信ﾃﾞｰﾀﾊﾞｲﾅﾘｨ格納ﾃﾞｨﾚｸﾄﾘｨ作成
        	if(_stat(mkdir, &buf) < 0) {
                if(CreateDirectory(mkdir, 0)) {
               	    MyMessage(main_hWnd, "ﾃﾞｨﾚｸﾄﾘｨ生成 (%s) 完了", mkdir);
                } else {
            	    MyMessage(main_hWnd, "ﾃﾞｨﾚｸﾄﾘｨ生成 (%s) 失敗", mkdir);
                }   
            }
        }
        mkdir[i] = tmp[i];
    }
    return TRUE;
}

/*--------------------------------------------------------------
* ファイル読み込み処理
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
	// ファイル種類の指定
	ofn.lpstrFilter = "ﾃﾞｰﾀﾌｧｲﾙ　  {*.txt}\0*.txt\0"\
					  "  C ﾌｧｲﾙ     {*.c}\0*.c; *.cpp; *.h\0"\
					  "全てのﾌｧｲﾙ {*.*}\0*.*\0\0";

	ofn.lpstrFile	= openName;
	ofn.nMaxFile = MAX_PATH;
	ofn.Flags = OFN_FILEMUSTEXIST;
//	ofn.Flags = OFN_FILEMUSTEXIST | OFN_ALLOWMULTISELECT | OFN_LONGNAMES | OFN_EXPLORER;
	// 拡張子が入力されない時のﾃﾞﾌｫﾙﾄの拡張子
	ofn.lpstrDefExt = "txt";
	if(GetOpenFileName(&ofn)){
		memset(dirName, 0x00, sizeof(dirName));
		strcpy(dirName, openName);
		len = strlen(openName);
//  MyMessage(main_hWnd, "ファイル指定 (%s) 完了", openName);
        SetDlgItemText(hWnd, IDC_SCH_NAME, openName);
        memset(tcpCtrl.schFileName, 0x00, sizeof(tcpCtrl.schFileName));
        memcpy(tcpCtrl.schFileName, openName, strlen(openName));

        MyMessage(main_hWnd, "");
        MyMessage(main_hWnd, "*****  ｽｹｼﾞｭｰﾙ管理ﾌｧｲﾙ　読込み ***********************************************");
        ScheduleDataSet(main_hWnd);
        MyMessage(main_hWnd, "******************************************************************************");
	}

}

/*--------------------------------------------------------------
* シナリオファイル読み込み処理
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
	// ファイル種類の指定
	ofn.lpstrFilter = "ﾃﾞｰﾀﾌｧｲﾙ　  {*.txt}\0*.txt\0"\
					  "  C ﾌｧｲﾙ     {*.c}\0*.c; *.cpp; *.h\0"\
					  "全てのﾌｧｲﾙ {*.*}\0*.*\0\0";

	ofn.lpstrFile	= openName;
	ofn.nMaxFile = MAX_PATH;
	ofn.Flags = OFN_FILEMUSTEXIST;
//	ofn.Flags = OFN_FILEMUSTEXIST | OFN_ALLOWMULTISELECT | OFN_LONGNAMES | OFN_EXPLORER;
	// 拡張子が入力されない時のﾃﾞﾌｫﾙﾄの拡張子
	ofn.lpstrDefExt = "txt";
	if(GetOpenFileName(&ofn)){
		memset(dirName, 0x00, sizeof(dirName));
		strcpy(dirName, openName);
		len = strlen(openName);
//  MyMessage(main_hWnd, "ファイル指定 (%s) 完了", openName);
        SetDlgItemText(hWnd, IDC_SINA_NAME, openName);
        memset(tcpCtrl.sinaFileName, 0x00, sizeof(tcpCtrl.sinaFileName));
        memcpy(tcpCtrl.sinaFileName, openName, strlen(openName));

        MyMessage(main_hWnd, "");
        MyMessage(main_hWnd, "*****  ｼﾅﾘｵ管理ﾌｧｲﾙ　読込み ***********************************************");
        SinaDataSet(main_hWnd);
        MyMessage(main_hWnd, "******************************************************************************");
	}
    tcpCtrl.sinaCtrl = 0;
	SendDlgItemMessage(hWnd, IDC_SINA_CHECK, BM_SETCHECK, tcpCtrl.sinaCtrl, 0);
    timerResetProc1(main_hWnd);
}


/*--------------------------------------------------------------
* エディター読み込み処理
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
	// ファイル種類の指定
	ofn.lpstrFilter = "実行ﾌｧｲﾙ　  {*.exe}\0*.exe\0"\
					  "全てのﾌｧｲﾙ {*.*}\0*.*\0\0";

	ofn.lpstrFile	= openName;
	ofn.nMaxFile = MAX_PATH;
	ofn.Flags = OFN_FILEMUSTEXIST;
//	ofn.Flags = OFN_FILEMUSTEXIST | OFN_ALLOWMULTISELECT | OFN_LONGNAMES | OFN_EXPLORER;
	// 拡張子が入力されない時のﾃﾞﾌｫﾙﾄの拡張子
	ofn.lpstrDefExt = "exe";
	if(GetOpenFileName(&ofn)){
		memset(dirName, 0x00, sizeof(dirName));
		strcpy(dirName, openName);
		len = strlen(openName);
//  MyMessage(main_hWnd, "ファイル指定 (%s) 完了", openName);
        SetDlgItemText(hWnd, IDC_EDIT_NAME, openName);
        memset(tcpCtrl.editName, 0x00, sizeof(tcpCtrl.editName));
        memcpy(tcpCtrl.editName, openName, strlen(openName));

        MyMessage(main_hWnd, "");
        MyMessage(main_hWnd, "ｴﾃﾞｨﾀﾌｧｲﾙ　読込み [%s]", tcpCtrl.editName);
	}
}

/*--------------------------------------------------------------
* カウンタファイル読み込み処理
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
	// ファイル種類の指定
	ofn.lpstrFilter = "ｶｳﾝﾀﾌｧｲﾙ　  {*.cnt}\0*.cnt\0"\
					  "全てのﾌｧｲﾙ {*.*}\0*.*\0\0";

	ofn.lpstrFile	= openName;
	ofn.nMaxFile = MAX_PATH;
	ofn.Flags = OFN_LONGNAMES | OFN_EXPLORER;
//	ofn.Flags = OFN_FILEMUSTEXIST | OFN_ALLOWMULTISELECT | OFN_LONGNAMES | OFN_EXPLORER;
	// 拡張子が入力されない時のﾃﾞﾌｫﾙﾄの拡張子
	ofn.lpstrDefExt = "cnt";
	if(GetOpenFileName(&ofn)){
		memset(dirName, 0x00, sizeof(dirName));
		strcpy(dirName, openName);
		len = strlen(openName);
//  MyMessage(main_hWnd, "ファイル指定 (%s) 完了", openName);
        SetDlgItemText(hWnd, IDC_COUNT_NAME, openName);
        memset(tcpCtrl.counterFile, 0x00, sizeof(tcpCtrl.counterFile));
        memcpy(tcpCtrl.counterFile, openName, strlen(openName));

        MyMessage(main_hWnd, "");
        MyMessage(main_hWnd, "ｶｳﾝﾀﾌｧｲﾙ　読込み [%s]", tcpCtrl.counterFile);
	}
}



/*--------------------------------------------------------------
* トレース格納ファイル指定処理
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
	// ファイル種類の指定
	ofn.lpstrFilter = "ﾄﾚｰｽﾌｧｲﾙ　 {*.log}\0*.log\0"\
					  "全てのﾌｧｲﾙ {*.*}\0*.*\0\0";

	ofn.lpstrFile	= openName;
	ofn.nMaxFile = MAX_PATH;
	ofn.Flags = OFN_LONGNAMES | OFN_EXPLORER;
//	ofn.Flags = OFN_FILEMUSTEXIST;
//	ofn.Flags = OFN_FILEMUSTEXIST | OFN_ALLOWMULTISELECT | OFN_LONGNAMES | OFN_EXPLORER;
	// 拡張子が入力されない時のﾃﾞﾌｫﾙﾄの拡張子
	ofn.lpstrDefExt = "log";
	if(GetOpenFileName(&ofn)){
		memset(dirName, 0x00, sizeof(dirName));
		strcpy(dirName, openName);
		len = strlen(openName);
//  MyMessage(main_hWnd, "ファイル指定 (%s) 完了", openName);
        SetDlgItemText(hWnd, IDC_EDIT1_TRACE, openName);
        memset(tcpCtrl.traceName, 0x00, sizeof(tcpCtrl.traceName));
        memcpy(tcpCtrl.traceName, openName, strlen(openName));

        MyMessage(main_hWnd, "");
        MyMessage(main_hWnd, "ﾄﾚｰｽﾌｧｲﾙ設定 [%s]", openName);
	}
}

/*--------------------------------------------------------------
* 受信データ格納ファイル指定処理
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
	// ファイル種類の指定
	ofn.lpstrFilter = "受信ﾌｧｲﾙ　 {*.bin}\0*.bin\0"\
					  "全てのﾌｧｲﾙ {*.*}\0*.*\0\0";

	ofn.lpstrFile	= openName;
	ofn.nMaxFile = MAX_PATH;
	ofn.Flags = OFN_LONGNAMES | OFN_EXPLORER;
//	ofn.Flags = OFN_FILEMUSTEXIST;
//	ofn.Flags = OFN_FILEMUSTEXIST | OFN_ALLOWMULTISELECT | OFN_LONGNAMES | OFN_EXPLORER;
	// 拡張子が入力されない時のﾃﾞﾌｫﾙﾄの拡張子
	ofn.lpstrDefExt = "bin";
	if(GetOpenFileName(&ofn)){
		memset(dirName, 0x00, sizeof(dirName));
		strcpy(dirName, openName);
		len = strlen(openName);
//  MyMessage(main_hWnd, "ファイル指定 (%s) 完了", openName);
        memset(tcpCtrl.recvFileName, 0x00, sizeof(tcpCtrl.recvFileName));

        SetDlgItemText(hWnd, IDC_RECV_USR_NAME, openName);
        memcpy(tcpCtrl.recvFileName, openName, strlen(openName));

        MyMessage(main_hWnd, "");
        MyMessage(main_hWnd, "受信ﾌｧｲﾙ設定 [%s]", openName);
	}
}

/*--------------------------------------------------------------
* 自動受信データ格納処理
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
	// ファイル種類の指定
//	ofn.lpstrFilter = "受信ﾌｧｲﾙ　 {*.bin}\0*.bin\0"\
//					  "全てのﾌｧｲﾙ {*.*}\0*.*\0\0";
	ofn.lpstrFilter = "受信ﾌｧｲﾙ　 {recv_HHMMSS_MMM.bin}\0recv_HHMMSS_MMM.bin\0\0";

	ofn.lpstrFile	= openName;
	ofn.nMaxFile = MAX_PATH;
	ofn.Flags = OFN_LONGNAMES | OFN_EXPLORER;
//	ofn.Flags = OFN_FILEMUSTEXIST;
//	ofn.Flags = OFN_FILEMUSTEXIST | OFN_ALLOWMULTISELECT | OFN_LONGNAMES | OFN_EXPLORER;
	// 拡張子が入力されない時のﾃﾞﾌｫﾙﾄの拡張子
	ofn.lpstrDefExt = "bin";
	if(GetOpenFileName(&ofn)){
		memset(dirName, 0x00, sizeof(dirName));
		strcpy(dirName, openName);
		len = strlen(openName);

        MyMessage(main_hWnd, "");
        MyMessage(main_hWnd, "AUTO受信ﾌｧｲﾙ設定 [%s]", openName);

//		memcpy(work, openName, len-strlen("recv_HHMMSS_MMM.bin"));
        for(i=len; i>0; i--) {
            if(openName[i-1] == 0x5c) {
                memcpy(work, openName, i);
                break;
            }
        }
//        MyMessage(main_hWnd, "AUTO受信ﾌｧｲﾙ設定 [%s]", tcpCtrl.recvFileBinName);
        SetDlgItemText(hWnd, IDC_RECV_AUTO_NAME, work);
        memcpy(tcpCtrl.recvFileBinName, work, strlen(work));
	}
}

void clsTraceFileName(HWND hWnd)
{
    if(0 != tcpCtrl.traceSave) {
//       	MessageBox( hWnd, "トレース格納のチェックをはずして下さい", "確認" ,
//                            MB_ICONEXCLAMATION | MB_OK );
        UserMsgBox(hWnd, "\nトレース格納のチェックをはずして下さい", NULL);
        return;
    }
	memset(tcpCtrl.traceName, 0x00, sizeof(tcpCtrl.traceName));

    SetDlgItemText(hWnd, IDC_EDIT1_TRACE, tcpCtrl.traceName);

	return;
}

void clsSinaFileName(HWND hWnd)
{
    if(0 != tcpCtrl.sinaCtrl) {
//       	MessageBox( hWnd, "シナリオ実行のチェックをはずして下さい", "確認" ,
//                            MB_ICONEXCLAMATION | MB_OK );
        UserMsgBox(hWnd, "\nシナリオ実行のチェックをはずして下さい", NULL);
        return;
    }
    memset(tcpCtrl.sinaFileName, 0x00, sizeof(tcpCtrl.sinaFileName));
	SetDlgItemText(hWnd, IDC_SINA_NAME, tcpCtrl.sinaFileName);

	return;
}

void clsSchFileName(HWND hWnd)
{
    if(0 != tcpCtrl.schCtrl || 0 != tcpCtrl.cycCtrl) {
//       	MessageBox(hWnd, "受信／定周期スケジュール実行のチェックをはずして下さい", "確認" ,
//                            MB_ICONEXCLAMATION | MB_OK );
        UserMsgBox(hWnd, "受信／定周期スケジュール実行のチェック\n\nをはずして下さい", NULL);
        return;
    }
    memset(tcpCtrl.schFileName, 0x00, sizeof(tcpCtrl.schFileName));
	SetDlgItemText(hWnd, IDC_SCH_NAME, tcpCtrl.sinaFileName);

    return;
}

void clsRecvFileName(HWND hWnd)
{
	if(0 != tcpCtrl.recvChkFile)  {
//       	MessageBox( hWnd, "受信データ格納のチェックをはずして下さい", "確認" ,
//                            MB_ICONEXCLAMATION | MB_OK );
        UserMsgBox(hWnd, "\n受信データ格納のチェックをはずして下さい", NULL);
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
	case WM_INITDIALOG:								// 初期表示
        break;
    case WM_COMMAND :
		switch (LOWORD(wParam)) {
        case IDOK :									// 終了ﾎﾞﾀﾝ
        case IDCANCEL :								// ｷｬﾝｾﾙ & 終了
            EndDialog(hWnd, 0);
			break;
        }
	case WM_DESTROY :								// 終了
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
	case WM_INITDIALOG:								// 初期表示
		hicon = LoadIcon(hThisInst, MAKEINTRESOURCE(IDI_ICON1));
		SendMessage( hWnd, WM_SETICON, TRUE, (LPARAM)hicon);
        if(NULL != g_user_title) {
           	SetWindowText(hWnd,   g_user_title);
        }
    	SetDlgItemText(hWnd, IDC_MSGBOX_TEXT, g_user_msg);
        break;
    case WM_COMMAND :
		switch (LOWORD(wParam)) {
        case IDOK :									// 終了ﾎﾞﾀﾝ
        case IDCANCEL :								// ｷｬﾝｾﾙ & 終了
			EndDialog(hWnd, 0);
			break;
        }
	case WM_DESTROY :								// 終了
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
    MyMessage(main_hWnd, "*****  ｽｹｼﾞｭｰﾙ管理ﾌｧｲﾙ　再読込み ***********************************************");
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
    MyMessage(main_hWnd, "*****  ｼﾅﾘｵ管理ﾌｧｲﾙ　再読込み ************************************************");
    SinaDataSet(main_hWnd);
    MyMessage(main_hWnd, "******************************************************************************");

    tcpCtrl.sinaCtrl = 0;
	SendDlgItemMessage(hWnd, IDC_SINA_CHECK, BM_SETCHECK, tcpCtrl.sinaCtrl, 0);
    timerResetProc1(main_hWnd);
	MyMessage(main_hWnd, "<<<<<  ｼﾅﾘｵﾀｲﾏｰ再設定 >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>");

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

    MakeFullName(hWnd, "伝文ﾀｲﾌﾟ説明.txt", fullName);
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
    case WM_INITDIALOG:                /* 初期表示要求 */
                                       /* 文字コード */
        if(tcpCtrl.eb_mojicode == _ASCII) {
            SendDlgItemMessage ( hWnd, IDC_RADIO3_ASC, BM_SETCHECK, 1, 0 );
            SendDlgItemMessage ( hWnd, IDC_RADIO3_EBC, BM_SETCHECK, 0, 0 );
        } else {
            SendDlgItemMessage ( hWnd, IDC_RADIO3_ASC, BM_SETCHECK, 0, 0 );
            SendDlgItemMessage ( hWnd, IDC_RADIO3_EBC, BM_SETCHECK, 1, 0 );
        }
                                       /* 透過／非透過 */
        if ( tcpCtrl.eb_transpar == EB_NOTTRNS ) {
            SendDlgItemMessage ( hWnd, IDC_RADIO3_NTRN, BM_SETCHECK, 1, 0 );
            SendDlgItemMessage ( hWnd, IDC_RADIO3__TRN, BM_SETCHECK, 0, 0 );
        } else {
            SendDlgItemMessage ( hWnd, IDC_RADIO3_NTRN, BM_SETCHECK, 0, 0 );
            SendDlgItemMessage ( hWnd, IDC_RADIO3__TRN, BM_SETCHECK, 1, 0 );
        }
                                       /* ACK1待ち時間 */
        _itoa ( tcpCtrl.tm_ak1_atxt, cinput_str, 10 );
        SetDlgItemText ( hWnd, IDC_EDIT_EB00, cinput_str );
                                       /* ACK0待ち時間 */
        _itoa ( tcpCtrl.tm_ak0_aenq, cinput_str, 10 );
        SetDlgItemText ( hWnd, IDC_EDIT_EB01, cinput_str );
                                       /* ﾃｷｽﾄ待ち時間 */
        _itoa ( tcpCtrl.tm_txt_aak0, cinput_str, 10 );
        SetDlgItemText ( hWnd, IDC_EDIT_EB02, cinput_str );
                                       /* EOT待ち時間 */
        _itoa ( tcpCtrl.tm_eot_aak1, cinput_str, 10 );
        SetDlgItemText ( hWnd, IDC_EDIT_EB03, cinput_str );
                                       /* ENQ再送待ち時間 */
        _itoa ( tcpCtrl.tm_enq_anak, cinput_str, 10 );
        SetDlgItemText ( hWnd, IDC_EDIT_EB04, cinput_str );
                                       /* ENQ送信回数 */
        _itoa ( tcpCtrl.nm_enq_nans, cinput_str, 10 );
        SetDlgItemText ( hWnd, IDC_EDIT_EB05, cinput_str );
                                       /* ﾃｷｽﾄ送信回数 */
        _itoa ( tcpCtrl.nm_txt_anak, cinput_str, 10 );
        SetDlgItemText ( hWnd, IDC_EDIT_EB06, cinput_str );
        break;

    case WM_COMMAND:
        switch (LOWORD(wParam)) {
        case IDC_SETEB:                /* 設定要求 */
                                       /* 文字コード */
            if ( SendDlgItemMessage ( hWnd, IDC_RADIO3_ASC, BM_GETCHECK, 0, 0 ) ) {
                tcpCtrl.eb_mojicode = _ASCII;
            } else {
                tcpCtrl.eb_mojicode = _EBCDIC;
            }
                                       /* 透過／非透過 */
            if ( SendDlgItemMessage ( hWnd, IDC_RADIO3_NTRN, BM_GETCHECK, 0, 0 ) ) {
                tcpCtrl.eb_transpar = EB_NOTTRNS;
            } else {
                tcpCtrl.eb_transpar = EB____TRNS;
            }
                                       /* ACK1待ち時間 */
            memset ( cinput_str, '\0', sizeof ( cinput_str ) );
            if ( GetDlgItemText ( hWnd, IDC_EDIT_EB00, cinput_str, sizeof ( cinput_str ) ) ) {
                if ( strlen ( cinput_str ) > IBCLMX_TIM ) {
                    MyMessage ( main_hWnd, "LINE:%d ACK1時間上下限エラー：初期値=%dをＳＥＴ", __LINE__, INIT_TMAK1 );
                    tcpCtrl.tm_ak1_atxt = INIT_TMAK1;
                    _itoa ( tcpCtrl.tm_ak1_atxt, cinput_str, 10 );
                    SetDlgItemText ( hWnd, IDC_EDIT_EB00, cinput_str );
                } else {
                    input_dat = atoi ( cinput_str );
                    if ( input_dat < IBCDMN_TIM || IBCDMX_TIM < input_dat ) {
                        MyMessage ( main_hWnd, "LINE:%d ACK1時間上下限エラー：初期値=%dをＳＥＴ", __LINE__, INIT_TMAK1 );
                        tcpCtrl.tm_ak1_atxt = INIT_TMAK1;
                        _itoa ( tcpCtrl.tm_ak1_atxt, cinput_str, 10 );
                        SetDlgItemText ( hWnd, IDC_EDIT_EB00, cinput_str );
                    } else {
                        tcpCtrl.tm_ak1_atxt = input_dat;
                    }
                }
            }
                                       /* ACK0待ち時間 */
            memset ( cinput_str, '\0', sizeof ( cinput_str ) );
            if ( GetDlgItemText ( hWnd, IDC_EDIT_EB01, cinput_str, sizeof ( cinput_str ) ) ) {
                if ( strlen ( cinput_str ) > IBCLMX_TIM ) {
                    MyMessage ( main_hWnd, "LINE:%d ACK0時間上下限エラー：初期値=%dをＳＥＴ", __LINE__, INIT_TMAK0 );
                    tcpCtrl.tm_ak0_aenq = INIT_TMAK0;
                    _itoa ( tcpCtrl.tm_ak0_aenq, cinput_str, 10 );
                    SetDlgItemText ( hWnd, IDC_EDIT_EB01, cinput_str );
                } else {
                    input_dat = atoi ( cinput_str );
                    if ( input_dat < IBCDMN_TIM || IBCDMX_TIM < input_dat ) {
                        MyMessage ( main_hWnd, "LINE:%d ACK0時間上下限エラー：初期値=%dをＳＥＴ", __LINE__, INIT_TMAK0 );
                        tcpCtrl.tm_ak0_aenq = INIT_TMAK0;
                        _itoa ( tcpCtrl.tm_ak0_aenq, cinput_str, 10 );
                        SetDlgItemText ( hWnd, IDC_EDIT_EB01, cinput_str );
                    } else {
                        tcpCtrl.tm_ak0_aenq = input_dat;
                    }
                }
            }
                                       /* ﾃｷｽﾄ待ち時間 */
            memset ( cinput_str, '\0', sizeof ( cinput_str ) );
            if ( GetDlgItemText ( hWnd, IDC_EDIT_EB02, cinput_str, sizeof ( cinput_str ) ) ) {
                if ( strlen ( cinput_str ) > IBCLMX_TIM ) {
                    MyMessage ( main_hWnd, "LINE:%d ﾃｷｽﾄ時間上下限エラー：初期値=%dをＳＥＴ", __LINE__, INIT_TMTXT );
                    tcpCtrl.tm_txt_aak0 = INIT_TMTXT;
                    _itoa ( tcpCtrl.tm_txt_aak0, cinput_str, 10 );
                    SetDlgItemText ( hWnd, IDC_EDIT_EB02, cinput_str );
                } else {
                    input_dat = atoi ( cinput_str );
                    if ( input_dat < IBCDMN_TIM || IBCDMX_TIM < input_dat ) {
                        MyMessage ( main_hWnd, "LINE:%d ﾃｷｽﾄ時間上下限エラー：初期値=%dをＳＥＴ", __LINE__, INIT_TMTXT );
                        tcpCtrl.tm_txt_aak0 = INIT_TMTXT;
                        _itoa ( tcpCtrl.tm_txt_aak0, cinput_str, 10 );
                        SetDlgItemText ( hWnd, IDC_EDIT_EB02, cinput_str );
                    } else {
                        tcpCtrl.tm_txt_aak0 = input_dat;
                    }
                }
            }
                                       /* EOT待ち時間 */
            memset ( cinput_str, '\0', sizeof ( cinput_str ) );
            if ( GetDlgItemText ( hWnd, IDC_EDIT_EB03, cinput_str, sizeof ( cinput_str ) ) ) {
                if ( strlen ( cinput_str ) > IBCLMX_TIM ) {
                    MyMessage ( main_hWnd, "LINE:%d EOT時間上下限エラー：初期値=%dをＳＥＴ", __LINE__, INIT_TMEOT );
                    tcpCtrl.tm_eot_aak1 = INIT_TMEOT;
                    _itoa ( tcpCtrl.tm_eot_aak1, cinput_str, 10 );
                    SetDlgItemText ( hWnd, IDC_EDIT_EB03, cinput_str );
                } else {
                    input_dat = atoi ( cinput_str );
                    if ( input_dat < IBCDMN_TIM || IBCDMX_TIM < input_dat ) {
                        MyMessage ( main_hWnd, "LINE:%d EOT時間上下限エラー：初期値=%dをＳＥＴ", __LINE__, INIT_TMEOT );
                        tcpCtrl.tm_eot_aak1 = INIT_TMEOT;
                        _itoa ( tcpCtrl.tm_eot_aak1, cinput_str, 10 );
                        SetDlgItemText ( hWnd, IDC_EDIT_EB03, cinput_str );
                    } else {
                        tcpCtrl.tm_eot_aak1 = input_dat;
                    }
                }
            }
                                       /* ENQ再送待ち時間 */
            memset ( cinput_str, '\0', sizeof ( cinput_str ) );
            if ( GetDlgItemText ( hWnd, IDC_EDIT_EB04, cinput_str, sizeof ( cinput_str ) ) ) {
                if ( strlen ( cinput_str ) > IBCLMX_TIM ) {
                    MyMessage ( main_hWnd, "LINE:%d ENQ再送待ち時間上下限エラー：初期値=%dをＳＥＴ", __LINE__, INIT_TMENQ );
                    tcpCtrl.tm_enq_anak = INIT_TMENQ;
                    _itoa ( tcpCtrl.tm_enq_anak, cinput_str, 10 );
                    SetDlgItemText ( hWnd, IDC_EDIT_EB04, cinput_str );
                } else {
                    input_dat = atoi ( cinput_str );
                    if ( input_dat < IBCDMN_TIM || IBCDMX_TIM < input_dat ) {
                        MyMessage ( main_hWnd, "LINE:%d ENQ再送待ち時間上下限エラー：初期値=%dをＳＥＴ", __LINE__, INIT_TMENQ );
                        tcpCtrl.tm_enq_anak = INIT_TMENQ;
                        _itoa ( tcpCtrl.tm_enq_anak, cinput_str, 10 );
                        SetDlgItemText ( hWnd, IDC_EDIT_EB04, cinput_str );
                    } else {
                        tcpCtrl.tm_enq_anak = input_dat;
                    }
                }
            }
                                       /* ENQ送信回数 */
            memset ( cinput_str, '\0', sizeof ( cinput_str ) );
            if ( GetDlgItemText ( hWnd, IDC_EDIT_EB05, cinput_str, sizeof ( cinput_str ) ) ) {
                if ( strlen ( cinput_str ) > IBCLMX_NUM ) {
                    MyMessage ( main_hWnd, "LINE:%d ENQ送信回数上下限エラー：初期値=%dをＳＥＴ", __LINE__, INIT_NMENQ );
                    tcpCtrl.nm_enq_nans = INIT_NMENQ;
                    _itoa ( tcpCtrl.nm_enq_nans, cinput_str, 10 );
                    SetDlgItemText ( hWnd, IDC_EDIT_EB05, cinput_str );
                } else {
                    input_dat = atoi ( cinput_str );
                    if ( input_dat < IBCDMN_NUM || IBCDMX_NUM < input_dat ) {
                        MyMessage ( main_hWnd, "LINE:%d ENQ送信回数上下限エラー：初期値=%dをＳＥＴ", __LINE__, INIT_NMENQ );
                        tcpCtrl.nm_enq_nans = INIT_NMENQ;
                        _itoa ( tcpCtrl.nm_enq_nans, cinput_str, 10 );
                        SetDlgItemText ( hWnd, IDC_EDIT_EB05, cinput_str );
                    } else {
                        tcpCtrl.nm_enq_nans = input_dat;
                    }
                }
            }
                                       /* ﾃｷｽﾄ送信回数 */
            memset ( cinput_str, '\0', sizeof ( cinput_str ) );
            if ( GetDlgItemText ( hWnd, IDC_EDIT_EB06, cinput_str, sizeof ( cinput_str ) ) ) {
                if ( strlen ( cinput_str ) > IBCLMX_NUM ) {
                    MyMessage ( main_hWnd, "LINE:%d ﾃｷｽﾄ送信回数上下限エラー：初期値=%dをＳＥＴ", __LINE__, INIT_NMTXT );
                    tcpCtrl.nm_txt_anak = INIT_NMTXT;
                    _itoa ( tcpCtrl.nm_txt_anak, cinput_str, 10 );
                    SetDlgItemText ( hWnd, IDC_EDIT_EB06, cinput_str );
                } else {
                    input_dat = atoi ( cinput_str );
                    if ( input_dat < IBCDMN_NUM || IBCDMX_NUM < input_dat ) {
                        MyMessage ( main_hWnd, "LINE:%d ﾃｷｽﾄ送信回数上下限エラー：初期値=%dをＳＥＴ", __LINE__, INIT_NMTXT );
                        tcpCtrl.nm_txt_anak = INIT_NMTXT;
                        _itoa ( tcpCtrl.nm_txt_anak, cinput_str, 10 );
                        SetDlgItemText ( hWnd, IDC_EDIT_EB06, cinput_str );
                    } else {
                        tcpCtrl.nm_txt_anak = input_dat;
                    }
                }
            }
            MyMessage ( main_hWnd, "LINE:%d Enet-B設定値変更完了", __LINE__ );
            break;

        case IDCANCEL3:                /* 終了要求 */
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
