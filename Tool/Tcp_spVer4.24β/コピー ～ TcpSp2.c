#include    <conio.h>
#include    <stdio.h>
#include    <windows.h>
#include    <winsock.h>
#include    <process.h>
#include    <commctrl.h>
#include    <winbase.h>
#include	"AP_Func.h"
#include    "TcpCtrl.h"
#include    "MakeDataSet.h"
#include    "MakeCntSet.h"
#include    "SchDataSet.h"
#include    "resource.h"
#include    "dump.h"

#define		SUBHED		0xE0
#define		RESP_OK		0x00
#define		RESP_NG1	0x50
#define		RESP_NG2	0x52

#define     PORT1       1
#define     PORT2       2
#define     LOAD        100

#define	    TIMER_ID			1			    // �������ϰID
#define	    SINA_TIMER_ID		2			    // ��ص��ϰID
#define	    RECV_SINA_TIMER_ID	3			    // ��M��ص��ϰID
#define     RETRY_TIMER_ID1     4
#define     RETRY_TIMER_ID2     5

#define		WM_USER_ASYNC_SELECT  (WM_USER + 1)	// հ�ް��`ү����

int				g_nTotalToSend;					// �����M�ް���
int             close_btn[2];
int             g_SinaSozOpen;
unsigned char   g_pSendBuf[COM_BUF_MAX];	    // ���M�ް��i�[�p
unsigned char   g_pRecvBuf[2][COM_BUF_MAX];		// ��M�ް��i�[�p
unsigned char   g_pRecvWork[COM_BUF_MAX];		// ��M�ް�WORK�i�[�p

unsigned char	g_listName[256];
unsigned char   fileName[256];

unsigned char   MsgBuf[128];

HINSTANCE       hThisInst;
LPTSTR          lpMsgBuf;

struct sockaddr_in  UdpMyAddr[2];               // �ڑ����A�h���X (UDP)
struct sockaddr_in  UdpDsAddr[2];               // �ڑ���A�h���X (UDP)

UINT	timerId = 0;							// �^�C�}�C�x���g�h�c
UINT	SinaTimerId = 0;						// �^�C�}�C�x���g�h�c
UINT	RecvSinaTimerId = 0;					// �^�C�}�C�x���g�h�c
UINT	RetryTimerId[2] = {0, 0};				// �^�C�}�C�x���g�h�c

unsigned char   g_edit_temp[256];

//-----------------------------------------------------------------------------
// �֐��錾
//-----------------------------------------------------------------------------
BOOL    CALLBACK DlgProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
BOOL    InitWinsock( void );
BOOL    ConnectSocket( HWND hWnd, int );
void    CloseSocket( HWND hWnd, int ) ;
void    SendMsg( HWND hWnd, int );
void    RecvMsg( HWND hWnd, int );
BOOL    OnAsyncSelect( HWND hWnd, LPARAM lParam, int );
BOOL    OnAccept( HWND hWnd, int );
void    getFileData( HWND hWnd);
BOOL	FileTypeTextRead(HWND hWnd);
BOOL	FileTypeDefNameRead(HWND hWnd);
BOOL	FileTypeBinNameRead(HWND hWnd);
BOOL	SendBtnProc(HWND hWnd);
BOOL	FileTypeDefDataRead(HWND hWnd);
BOOL	FileTypeBinDataRead(HWND hWnd);
BOOL    MakeDefData(HWND hWnd, unsigned char *fileName, unsigned char *recvbuff);
BOOL    MakeRecvSinaData(HWND hWnd, unsigned char *fileName, unsigned char *recvbuff, int port);
BOOL    PtnDataSet(HWND hWnd, unsigned char* buff, unsigned char *fileName);
BOOL	GetAddressData(HWND hWnd);
BOOL	DispComCtrl(HWND hWnd);
BOOL    MakeScheduleDataSet(HWND hWnd, unsigned char* buff, int setAddr, int itemAddr);
BOOL    MakeCycleSchDataSet(HWND hWnd, unsigned char* buff, int setAddr);
BOOL    MakeSinaSchDataSet(HWND hWnd, unsigned char* buff, int setAddr);
BOOL    MakeRecvSinaSchDataSet(HWND hWnd, unsigned char* buff, int setAddr, int point, int rcv_port);
void    CALLBACK timeUpProc(HWND, UINT, UINT, DWORD);
void    CALLBACK timeUpProc1(HWND, UINT, UINT, DWORD);
void    CALLBACK timeUpProc2(HWND, UINT, UINT, DWORD);
void    CALLBACK timeUpProc3(HWND, UINT, UINT, DWORD);
int		TcpSetting( HWND hWnd );
BOOL    isTcpType(void);
BOOL    isConnectType(int port);
BOOL    isAcceptType(int port);
BOOL    isConnectPort(int port);
BOOL    isTcpAccept(int port);
BOOL    isTcpConnect(int port);
BOOL    isSOZ(void);
BOOL    saveListData(HWND hWnd);
BOOL    dropFileList(HWND hWnd, WPARAM wParam);
int		chkTime( HWND hWnd );
int     check_command(HWND hWnd, char*name, int sLen);
int     makeDllFileName(HWND hWnd, char* cmd_fname);
int     SchBinDataRead(HWND hWnd, unsigned char* filename);
int     listBoxEdit(HWND hWnd);
int     listDataEdit(HWND hWnd);
void	last_error();
BOOL	moveListData(HWND, DWORD);
void	dispScheduleData(HWND hWnd, int addr);
int     SchDefSub(int tblno, int addr, int chkAddr);
int		makePcterm(HWND hWnd, unsigned char* g_pSendBuf, unsigned char* g_pSendDmy, int g_nTotalToSend, unsigned char g_pcterm_pa);
int     rdata_to_sdata(HWND hWnd, unsigned char *buff, unsigned char* recvbuff);
int     delTempfile(HWND hwnd);
BOOL    ConnectProc(HWND hWnd, int port);
char*   GetNowTime();
BOOL	CALLBACK DlgProcOkNg(HWND hWnd, UINT uMessage, WPARAM wParam, LPARAM lParam);
int     getOffsetByComType(void);

//************************************
// WinMain
//************************************
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdLine, int nCmdShow)
{

	GlpszCmdLine = lpszCmdLine;
	GnCmdShow    = nCmdShow;

	hThisInst = hInstance;

	g_sSock[0]    = INVALID_SOCKET;
	g_sSock[1]    = INVALID_SOCKET;
	g_sSockClient[0] = INVALID_SOCKET;
	g_sSockClient[1] = INVALID_SOCKET;

	// ���X�|���X�E�T�u�w�b�_�̏����l�Z�b�g�I�I
	G_respons[0] = SUBHED;
	G_respons[1] = RESP_OK;

	G_hed[0] = 0x60;
	G_hed[1] = 0x00;

	/* ���s���邽�тɈႤ�l��������悤�ɁA���݂̎����l���g����	*/
	/* �����W�F�l���[�^�����������܂��B								*/
	srand( (unsigned)time( NULL ) );

	if ( InitWinsock( ) ) {
		DialogBox( hInstance, (LPCTSTR)IDD_TCPSP, NULL, DlgProc );
		WSACleanup( );
	}

	return 0;
}

//************************************
// �_�C�A���O�v���V�W���[
// �����FTRUE
// ���s�FFALSE
//************************************
BOOL CALLBACK DlgProc( HWND hWnd, UINT uMessage, WPARAM wParam, LPARAM lParam )
{
	int		iSel, cnt, i;
	HICON	hicon;

	switch (uMessage) {
	case WM_INITDIALOG:
	case WM_USER_ASYNC_SELECT+LOAD:		// �����񃍁[�h
		// �A�C�R���̕ύX
		hicon = LoadIcon(hThisInst, MAKEINTRESOURCE(IDI_ICON1));
		SendMessage( hWnd, WM_SETICON, TRUE, (LPARAM)hicon);
		
        // �ݒ���ǂݍ���
        if(WM_INITDIALOG == uMessage) {
            getTcpCtrl(hWnd);

			iSel=SendDlgItemMessage(hWnd, IDC_COMBO2, CB_GETCOUNT, 0, 0);
			for(i=0; i<TYPE_MAX && 0 == iSel; i++) {
				SendDlgItemMessage( hWnd, IDC_COMBO2, CB_ADDSTRING, 0, (LPARAM)connectType[i]);
//				MyMessage(hWnd, "[%s] ", connectType[i]);
			}
            delTempfile(hWnd);
        }
        if(isTcpType()) {
			// TCP
			EnableWindow( GetDlgItem( hWnd, IDC_RADIO_CONNECT ), TRUE );
			EnableWindow( GetDlgItem( hWnd, IDC_RADIO_ACCEPT  ), TRUE );
			EnableWindow( GetDlgItem( hWnd, IDC_RADIO_CONNECT3), TRUE );
			EnableWindow( GetDlgItem( hWnd, IDC_RADIO_ACCEPT3 ), TRUE );

		    SendDlgItemMessage( hWnd, IDC_TCP, BM_SETCHECK, 1, 0 );
		    SendDlgItemMessage( hWnd, IDC_UDP, BM_SETCHECK, 0, 0 );
			if(isConnectType(PORT1)) {
    			SendDlgItemMessage( hWnd, IDC_RADIO_CONNECT, BM_SETCHECK, 1, 0 );
    			SendDlgItemMessage( hWnd, IDC_RADIO_ACCEPT,  BM_SETCHECK, 0, 0 );
			} else {
    			SendDlgItemMessage( hWnd, IDC_RADIO_CONNECT, BM_SETCHECK, 0, 0 );
    			SendDlgItemMessage( hWnd, IDC_RADIO_ACCEPT,  BM_SETCHECK, 1, 0 );
				EnableWindow( GetDlgItem( hWnd, IDC_EDIT_IP ), FALSE );
			}
			if(isConnectType(PORT2)) {
    			SendDlgItemMessage( hWnd, IDC_RADIO_CONNECT3, BM_SETCHECK, 1, 0 );
    			SendDlgItemMessage( hWnd, IDC_RADIO_ACCEPT3,  BM_SETCHECK, 0, 0 );
			} else {
    			SendDlgItemMessage( hWnd, IDC_RADIO_CONNECT3, BM_SETCHECK, 0, 0 );
    			SendDlgItemMessage( hWnd, IDC_RADIO_ACCEPT3,  BM_SETCHECK, 1, 0 );
				EnableWindow( GetDlgItem( hWnd, IDC_EDIT_IP3 ), FALSE );
			}
        } else {
			// UDP
		    SendDlgItemMessage( hWnd, IDC_TCP, BM_SETCHECK, 0, 0 );
		    SendDlgItemMessage( hWnd, IDC_UDP, BM_SETCHECK, 1, 0 );

			EnableWindow( GetDlgItem( hWnd, IDC_RADIO_CONNECT ), FALSE );
			EnableWindow( GetDlgItem( hWnd, IDC_RADIO_ACCEPT  ), FALSE );
			EnableWindow( GetDlgItem( hWnd, IDC_RADIO_CONNECT3), FALSE );
			EnableWindow( GetDlgItem( hWnd, IDC_RADIO_ACCEPT3 ), FALSE );

			EnableWindow( GetDlgItem( hWnd, IDC_RADIO_CONNECT ), FALSE );
			EnableWindow( GetDlgItem( hWnd, IDC_RADIO_ACCEPT  ), FALSE );
			EnableWindow( GetDlgItem( hWnd, IDC_RADIO_CONNECT3), FALSE );
			EnableWindow( GetDlgItem( hWnd, IDC_RADIO_ACCEPT3 ), FALSE );

			EnableWindow( GetDlgItem( hWnd, IDC_EDIT_IP  ), TRUE );
			EnableWindow( GetDlgItem( hWnd, IDC_EDIT_IP3 ), TRUE );

		}
		SendDlgItemMessage( hWnd, IDC_RADIO2_TEXT, BM_SETCHECK, 1, 0 );
		SendDlgItemMessage( hWnd, IDC_RADIO2_FILE, BM_SETCHECK, 0, 0 );

//		MyMessage(hWnd, "tcpCtrl.comType=%d ", tcpCtrl.comType);
//	    MyMessage(hWnd, "pszCmdLine=%s", GlpszCmdLine);
//	    MyMessage(hWnd, "nCmdShow=%d",  GnCmdShow);

		SendDlgItemMessage( hWnd, IDC_COMBO2, CB_SELECTSTRING, 0, (LPARAM)connectType[tcpCtrl.comType] );

        SetDlgItemText(hWnd, IDC_EDIT_IP,    tcpCtrl.ip[0]);        
        SetDlgItemText(hWnd, IDC_EDIT_PORT,  tcpCtrl.port[0]);        
        SetDlgItemText(hWnd, IDC_EDIT_IP3,   tcpCtrl.ip[1]);        
        SetDlgItemText(hWnd, IDC_EDIT_PORT3, tcpCtrl.port[1]);        
 
        // ���M�C�ؒf�{�^���̖�����
		EnableWindow( GetDlgItem( hWnd, IDC_BUTTON_SEND ),   FALSE );
		EnableWindow( GetDlgItem( hWnd, IDC_BUTTON_CLOSE ),  FALSE );
		EnableWindow( GetDlgItem( hWnd, IDC_BUTTON_SEND3 ),  FALSE );
		EnableWindow( GetDlgItem( hWnd, IDC_BUTTON_CLOSE3 ), FALSE );
//		EnableWindow( GetDlgItem( hWnd, IDC_EDIT_MSG ), FALSE );
		SendDlgItemMessage( hWnd, IDC_LIST_STATUS, LB_SETHORIZONTALEXTENT, 1024,  0 );	// ؽ��ޯ���@�o�^���ڑ����@�擾
		SendDlgItemMessage( hWnd, IDC_LIST_DATA,   LB_SETHORIZONTALEXTENT, 2048, 0 );	// ؽ��ޯ���@�o�^���ڑ����@�擾

		EnableWindow( GetDlgItem( hWnd, IDC_RSINA_STOP ),   FALSE );
		EnableWindow( GetDlgItem( hWnd, IDC_SINA_STOP ),   FALSE );

		DispComCtrl(hWnd);
        chkTime(hWnd);

		if(0 != g_flag) {
			EnableWindow( GetDlgItem( hWnd, IDC_BUTTON_CONNECT  ), FALSE );
			EnableWindow( GetDlgItem( hWnd, IDC_BUTTON_CONNECT3 ), FALSE );
			EnableWindow( GetDlgItem( hWnd, IDC_LIST_LOAD ), FALSE );
			EnableWindow( GetDlgItem( hWnd, IDC_LIST_SAVE ), FALSE );
			EnableWindow( GetDlgItem( hWnd, IDC_CHECK2_TRACE ), FALSE );
			EnableWindow( GetDlgItem( hWnd, IDC_BUTTON_SEND ), FALSE );
			EnableWindow( GetDlgItem( hWnd, IDC_BUTTON_SEND3 ), FALSE );
            EnableWindow( GetDlgItem( hWnd, IDC_BUTTON1_FILERD ), FALSE );

            EnableWindow( GetDlgItem( hWnd, IDC_EDIT_IP ), FALSE );
            EnableWindow( GetDlgItem( hWnd, IDC_EDIT_IP3 ), FALSE );
            EnableWindow( GetDlgItem( hWnd, IDC_COMBO2 ), FALSE );
            EnableWindow( GetDlgItem( hWnd, IDC_BUTTON1_FILERD3 ), FALSE );
            EnableWindow( GetDlgItem( hWnd, IDC_BUTTON1_FILERD2 ), FALSE );

            EnableWindow( GetDlgItem( hWnd, IDC_EDIT_PORT ), FALSE );
            EnableWindow( GetDlgItem( hWnd, IDC_EDIT_PORT3 ), FALSE );
            EnableWindow( GetDlgItem( hWnd, IDC_RADIO_CONNECT ), FALSE );
            EnableWindow( GetDlgItem( hWnd, IDC_RADIO_CONNECT3 ), FALSE );
            EnableWindow( GetDlgItem( hWnd, IDC_RADIO_ACCEPT ), FALSE );
            EnableWindow( GetDlgItem( hWnd, IDC_RADIO_ACCEPT3 ), FALSE );
            EnableWindow( GetDlgItem( hWnd, IDC_TCP ), FALSE );
            EnableWindow( GetDlgItem( hWnd, IDC_UDP ), FALSE );
            EnableWindow( GetDlgItem( hWnd, IDC_RADIO2_TEXT ), FALSE );
            EnableWindow( GetDlgItem( hWnd, IDC_RADIO2_FILE ), FALSE );
            EnableWindow( GetDlgItem( hWnd, IDC_EDIT_MSG ), FALSE );
            EnableWindow( GetDlgItem( hWnd, IDC_BUTTON1_UP ), FALSE );
            EnableWindow( GetDlgItem( hWnd, IDC_BUTTON1_DOWN ), FALSE );
            EnableWindow( GetDlgItem( hWnd, IDC_LIST_DATA ), FALSE );
            EnableWindow( GetDlgItem( hWnd, IDC_LIST_STATUS ), FALSE );

		}
//		uDLMsg = RegisterWindowMessage(DRAGLISTMSGSTRING);
 		break;

    case WM_DESTROY:
        tcpCtrl.retry_flg = 0;
        timerResetProc(hWnd);
        timerResetProc1(hWnd);
        timerResetProc2(hWnd);
//        GetAddressData(hWnd);
//        putTcpCtrl(hWnd, NULL);
		CloseSocket(hWnd, PORT1);
		CloseSocket(hWnd, PORT2);
        break;

	case WM_COMMAND:
		switch ( LOWORD(wParam) )	{
        /////////////////////////////////////////
		// �`���^�C�v
		case IDC_COMBO2:
			if(HIWORD(wParam) == LBN_SELCHANGE) {
				iSel=SendDlgItemMessage(hWnd, IDC_COMBO2, CB_GETCURSEL, 0, 0);
				tcpCtrl.comType = iSel;
			}
            break;
        /////////////////////////////////////////
		// �ڑ��^�C�v�@�\����(CONNECT)
		case IDC_RADIO_CONNECT:
			tcpCtrl.connectType[0] = 1;
			SendDlgItemMessage( hWnd, IDC_RADIO_CONNECT, BM_SETCHECK, 1, 0 );
			SendDlgItemMessage( hWnd, IDC_RADIO_ACCEPT, BM_SETCHECK, 0, 0 );
			EnableWindow( GetDlgItem( hWnd, IDC_EDIT_IP ), TRUE );
			break;

        /////////////////////////////////////////
		// �ڑ��^�C�v�@�\����(CONNECT)
		case IDC_RADIO_CONNECT3:
			tcpCtrl.connectType[1] = 1;
			SendDlgItemMessage( hWnd, IDC_RADIO_CONNECT3, BM_SETCHECK, 1, 0 );
			SendDlgItemMessage( hWnd, IDC_RADIO_ACCEPT3, BM_SETCHECK, 0, 0 );
			EnableWindow( GetDlgItem( hWnd, IDC_EDIT_IP3 ), TRUE );
			break;

    	/////////////////////////////////////////
		// �ڑ��^�C�v�@�󓮑�(ACCEPT)
		case IDC_RADIO_ACCEPT:
			tcpCtrl.connectType[0] = 2;
			SendDlgItemMessage( hWnd, IDC_RADIO_CONNECT, BM_SETCHECK, 0, 0 );
			SendDlgItemMessage( hWnd, IDC_RADIO_ACCEPT, BM_SETCHECK, 1, 0 );
			EnableWindow( GetDlgItem( hWnd, IDC_EDIT_IP ), FALSE );
			break;

    	/////////////////////////////////////////
		// �ڑ��^�C�v�@�󓮑�(ACCEPT)
		case IDC_RADIO_ACCEPT3:
			tcpCtrl.connectType[1] = 2;
			SendDlgItemMessage( hWnd, IDC_RADIO_CONNECT3, BM_SETCHECK, 0, 0 );
			SendDlgItemMessage( hWnd, IDC_RADIO_ACCEPT3, BM_SETCHECK, 1, 0 );
			EnableWindow( GetDlgItem( hWnd, IDC_EDIT_IP3 ), FALSE );
			break;

    	/////////////////////////////////////////
		// TCP
		case IDC_TCP:
			tcpCtrl.tcp_udp = 0;
		    SendDlgItemMessage( hWnd, IDC_TCP, BM_SETCHECK, 1, 0 );
		    SendDlgItemMessage( hWnd, IDC_UDP, BM_SETCHECK, 0, 0 );

			EnableWindow( GetDlgItem( hWnd, IDC_RADIO_CONNECT ), TRUE );
			EnableWindow( GetDlgItem( hWnd, IDC_RADIO_ACCEPT  ), TRUE );
			EnableWindow( GetDlgItem( hWnd, IDC_RADIO_CONNECT3), TRUE );
			EnableWindow( GetDlgItem( hWnd, IDC_RADIO_ACCEPT3 ), TRUE );

			if(isConnectType(PORT1)) {
    			SendDlgItemMessage( hWnd, IDC_RADIO_CONNECT, BM_SETCHECK, 1, 0 );
    			SendDlgItemMessage( hWnd, IDC_RADIO_ACCEPT,  BM_SETCHECK, 0, 0 );
			} else {
    			SendDlgItemMessage( hWnd, IDC_RADIO_CONNECT, BM_SETCHECK, 0, 0 );
    			SendDlgItemMessage( hWnd, IDC_RADIO_ACCEPT,  BM_SETCHECK, 1, 0 );
				EnableWindow( GetDlgItem( hWnd, IDC_EDIT_IP ), FALSE );
			}
			if(isConnectType(PORT2)) {
    			SendDlgItemMessage( hWnd, IDC_RADIO_CONNECT3, BM_SETCHECK, 1, 0 );
    			SendDlgItemMessage( hWnd, IDC_RADIO_ACCEPT3,  BM_SETCHECK, 0, 0 );
			} else {
    			SendDlgItemMessage( hWnd, IDC_RADIO_CONNECT3, BM_SETCHECK, 0, 0 );
    			SendDlgItemMessage( hWnd, IDC_RADIO_ACCEPT3,  BM_SETCHECK, 1, 0 );
				EnableWindow( GetDlgItem( hWnd, IDC_EDIT_IP3 ), FALSE );
			}
			break;

		/////////////////////////////////////////
		// UDP
		case IDC_UDP:
			tcpCtrl.tcp_udp = 1;
		    SendDlgItemMessage( hWnd, IDC_TCP, BM_SETCHECK, 0, 0 );
		    SendDlgItemMessage( hWnd, IDC_UDP, BM_SETCHECK, 1, 0 );

			EnableWindow( GetDlgItem( hWnd, IDC_RADIO_CONNECT ), FALSE );
			EnableWindow( GetDlgItem( hWnd, IDC_RADIO_ACCEPT  ), FALSE );
			EnableWindow( GetDlgItem( hWnd, IDC_RADIO_CONNECT3), FALSE );
			EnableWindow( GetDlgItem( hWnd, IDC_RADIO_ACCEPT3 ), FALSE );
			EnableWindow( GetDlgItem( hWnd, IDC_EDIT_IP ), TRUE );
			EnableWindow( GetDlgItem( hWnd, IDC_EDIT_IP3 ), TRUE );
			break;
		/////////////////////////////////////////
        // ���M�敪÷���ް�
        // ���M�敪̧���ް�
        case IDC_RADIO2_TEXT:
        case IDC_RADIO2_FILE:
            break;

        /////////////////////////////////////////
        // ���M�ް����ʹر
		case IDC_EDIT_MSG:
			if(HIWORD(wParam) == EN_SETFOCUS) {
				SendDlgItemMessage(hWnd, IDC_RADIO2_TEXT, BM_SETCHECK, 1, 0); 
				SendDlgItemMessage(hWnd, IDC_RADIO2_FILE, BM_SETCHECK, 0, 0); 
			}
			break;

	  	/////////////////////////////////////////
    	// ���M�ް�ؽ�BOX
		case IDC_LIST_DATA:
			if(HIWORD(wParam) == LBN_SETFOCUS) {
				SendDlgItemMessage(hWnd, IDC_RADIO2_TEXT, BM_SETCHECK, 0, 0); 
				SendDlgItemMessage(hWnd, IDC_RADIO2_FILE, BM_SETCHECK, 1, 0); 
			}
			if(HIWORD(wParam) == LBN_DBLCLK) { 
				listDataEdit(hWnd);
			}
			break;

	   	/////////////////////////////////////////
        // �t�@�C���ǂݍ���
        case IDC_BUTTON1_FILERD:
            getFileData(hWnd);
            break;

	   	/////////////////////////////////////////
        // ���X�g�f�[�^�S�t�@�C���N���A
        case IDC_BUTTON1_FILERD2:
			if(1 > SendDlgItemMessage(hWnd, IDC_LIST_DATA, LB_GETCOUNT, 0, 0)) {
				break;
			}
            OkNgMsgBox( hWnd, "��낵���ł����H", "���X�g�f�[�^�����������܂��B");
	        if(1 == g_okng) {
                SendDlgItemMessage(hWnd, IDC_LIST_DATA, LB_RESETCONTENT, 0, 0);
            }

//			if(IDOK == MessageBox( hWnd, "��낵���ł����H",
//				                         "���X�g�f�[�^�����������܂��B" , 
//										  MB_ICONQUESTION | MB_OKCANCEL )) {
//
//	            SendDlgItemMessage(hWnd, IDC_LIST_DATA, LB_RESETCONTENT, 0, 0);
//			}
            break;

	   	/////////////////////////////////////////
        // ���X�g�f�[�^�ꌏ�t�@�C���N���A
        case IDC_BUTTON1_FILERD3:
			if(1 > SendDlgItemMessage(hWnd, IDC_LIST_DATA, LB_GETCOUNT, 0, 0)) {
				break;
			}
            if((iSel= SendDlgItemMessage(hWnd, IDC_LIST_DATA, LB_GETCURSEL, 0, 0)) == LB_ERR) {
				break;
			}
			SendDlgItemMessage(hWnd, IDC_LIST_DATA, LB_DELETESTRING, (WPARAM)iSel, 0);
			cnt = SendDlgItemMessage(hWnd, IDC_LIST_DATA, LB_GETCOUNT, 0, 0);

			if(cnt <= iSel) iSel = cnt - 1;
			if(0 > iSel) break; 
			SendDlgItemMessage(hWnd, IDC_LIST_DATA, LB_SETCURSEL, iSel, 0);

            break;

	   	/////////////////////////////////////////
        // ���X�g�f�[�^�ړ�
        case IDC_BUTTON1_UP:
        case IDC_BUTTON1_DOWN:
			moveListData(hWnd, LOWORD(wParam));
            break;

		/////////////////////////////////////////
		// �\�P�b�g�̐�������ڑ�
        case IDC_BUTTON_CONNECT:
            close_btn[0] = 0;
            g_SinaSozOpen = 0;
            ConnectProc(hWnd, PORT1);
			break;

        case IDC_BUTTON_CONNECT3:
            g_SinaSozOpen = 0;
            close_btn[1] = 0;
            ConnectProc(hWnd, PORT2);
			break;


    	/////////////////////////////////////////
		// ���M�{�^��
        case IDC_BUTTON_SEND:		// ���M
			// ���M�f�[�^�̃Z�b�g
            if(FALSE == SendBtnProc(hWnd)) {
                break;
            }
			PostMessage( hWnd, WM_USER_ASYNC_SELECT+PORT1, g_sSock[0], WSAMAKESELECTREPLY( FD_WRITE, 0 ) );

            break;

        /////////////////////////////////////////
		// ���M�{�^��
        case IDC_BUTTON_SEND3:		// ���M
			// ���M�f�[�^�̃Z�b�g
            if(FALSE == SendBtnProc(hWnd)) {
                break;
            }
			PostMessage( hWnd, WM_USER_ASYNC_SELECT+PORT2, g_sSock[1], WSAMAKESELECTREPLY( FD_WRITE, 0 ) );

            break;

    	/////////////////////////////////////////
		// �ؒf�{�^��
        case IDC_BUTTON_CLOSE:		// �ؒf 
            close_btn[0] = 1;
			if ( g_sSock[0] != INVALID_SOCKET ) {
				PostMessage( hWnd, WM_USER_ASYNC_SELECT+PORT1, g_sSock[0], WSAMAKESELECTREPLY( FD_CLOSE, 0 ) );
				EnableWindow( GetDlgItem(hWnd, IDC_BUTTON_SEND ), FALSE );
				EnableWindow( GetDlgItem(hWnd, IDC_BUTTON_CLOSE ), FALSE );
				if(g_sSock[1] == INVALID_SOCKET) {
					EnableWindow( GetDlgItem( hWnd, IDC_TCP ), TRUE );
					EnableWindow( GetDlgItem( hWnd, IDC_UDP ), TRUE );
				}
//				EnableWindow( GetDlgItem(hWnd, IDC_EDIT_MSG ), FALSE );
			}
			break;

        /////////////////////////////////////////
		// �ؒf�{�^��
        case IDC_BUTTON_CLOSE3:		// �ؒf 
            close_btn[1] = 1;
			if ( g_sSock[1] != INVALID_SOCKET ) {
				PostMessage( hWnd, WM_USER_ASYNC_SELECT+PORT2, g_sSock[1], WSAMAKESELECTREPLY( FD_CLOSE, 0 ) );
				EnableWindow( GetDlgItem(hWnd, IDC_BUTTON_SEND3 ), FALSE );
				EnableWindow( GetDlgItem(hWnd, IDC_BUTTON_CLOSE3 ), FALSE );
				if(g_sSock[0] == INVALID_SOCKET) {
					EnableWindow( GetDlgItem( hWnd, IDC_TCP ), TRUE );
					EnableWindow( GetDlgItem( hWnd, IDC_UDP ), TRUE );
				}
//				EnableWindow( GetDlgItem(hWnd, IDC_EDIT_MSG3 ), FALSE );
            }

			break;

		/////////////////////////////////////////
		// �ڰ��ر�ر
		case IDC_CHECK2_TRACE:
			SendDlgItemMessage(hWnd, IDC_LIST_STATUS, LB_RESETCONTENT, 0, 0);
			break;

    	/////////////////////////////////////////
		// ��M��ص�ꎞ��~
        case IDC_RSINA_STOP:
			if(0 == tcpCtrl.rsina_stop) {
				tcpCtrl.rsina_stop = 1;
            } else {
				tcpCtrl.rsina_stop = 0;
			}
            break;

    	/////////////////////////////////////////
		// ��ص�ꎞ��~
        case IDC_SINA_STOP:
			if(0 == tcpCtrl.sina_stop) {
				tcpCtrl.sina_stop = 1;
            } else {
				tcpCtrl.sina_stop = 0;
			}
            break;
    	/////////////////////////////////////////
		// �I���{�^��
		case IDCANCEL_BTN:				// �I��
            close_btn[0] = 1;
            close_btn[1] = 1;

            tcpCtrl.retry_flg = 0;
			timerResetProc(hWnd);
            timerResetProc1(hWnd);
            timerResetProc2(hWnd);
            GetAddressData(hWnd);
            putTcpCtrl(hWnd, NULL);
	    	CloseSocket(hWnd, PORT1);
		    CloseSocket(hWnd, PORT2);
            delTempfile(hWnd);
			EndDialog(hWnd, 0);
			break;

    	/////////////////////////////////////////
		// �ݒ�{�^��
		case IDC_BUTTON_SET:		// �����ݒ�
			TcpSetting(hWnd);
			DispComCtrl(hWnd);
			break;

    	/////////////////////////////////////////
		// ���X�g���[�h�{�^��
		case IDC_LIST_LOAD:	 
			loadListData(hWnd, IDC_LIST_LOAD);
			break;

    	/////////////////////////////////////////
		// ���X�g�Z�[�u�{�^��
		case IDC_LIST_SAVE:	 
            saveListData(hWnd);
			break;
		
    	/////////////////////////////////////////
		// �ڰ��\������ؽ�BOX
		case IDC_LIST_STATUS:
			// ����ٸد�����
			if(HIWORD(wParam) == LBN_DBLCLK) { 
				listBoxEdit(hWnd);
			}
			break;

        /////////////////////////////////////////
		// ���̑�
        default:
			return FALSE;
		}
		break;

    //////////////////////////////////////////
    // �\�P�b�g���b�Z�[�W��M
	case WM_USER_ASYNC_SELECT+PORT1:		// �\�P�b�g���b�Z�[�W
		OnAsyncSelect( hWnd, lParam, PORT1 );
		break;

	case WM_USER_ASYNC_SELECT+PORT2:		// �\�P�b�g���b�Z�[�W
		OnAsyncSelect( hWnd, lParam, PORT2);
		break;

	case WM_DROPFILES:
		dropFileList(hWnd, wParam);
		break;

	case WM_CLOSE:
        close_btn[0] = 1;
        close_btn[1] = 1;

        tcpCtrl.retry_flg = 0;
		timerResetProc(hWnd);
		timerResetProc1(hWnd);
		timerResetProc2(hWnd);
		GetAddressData(hWnd);
		putTcpCtrl(hWnd, NULL);
		CloseSocket(hWnd, PORT1);
		CloseSocket(hWnd, PORT2);
		delTempfile(hWnd);
		EndDialog(hWnd, 0);
	  break;

	default:
		return FALSE;
	}
	return TRUE;
}


//************************************
// WinSock �̏�����
// �����FTRUE
// ���s�FFALSE
//************************************
BOOL InitWinsock( void )
{
	WSADATA wsaData;

	WORD wVersionRequired = MAKEWORD(1, 1);		// Windows95 WinSock 1.1
//	WORD wVersionRequired = MAKEWORD(2, 0);		// WindowsNT WinSock 2.0

	if ( WSAStartup(wVersionRequired, &wsaData) == 0 ) {
		if ( wVersionRequired == wsaData.wVersion ) return TRUE;
		WSACleanup( );
		return FALSE;
	}
	return FALSE;
}

//************************************
// �\�P�b�g�̐�������ڑ�
// �����FTRUE
// ���s�FFALSE
//************************************
BOOL ConnectSocket( HWND hWnd, int port )
{
	unsigned short	usport;
	unsigned short	udp_send_port;
	char			cip[20];
	char			cbuf[128];
	int 			nerror;
	SOCKADDR_IN 	addr;
    int             tblno;
    int             i;
	char            recv_port[32];
	char            send_port[32];
	char            tcp_port[32];
	char            tcp_port_c[32];
	int             tcp_port1;
	int				num, rc;
	unsigned short  flg;
	struct linger	l;

    unsigned int    ipTbl[2][2] = {
        { IDC_EDIT_IP,  IDC_EDIT_PORT },
        { IDC_EDIT_IP3, IDC_EDIT_PORT3 } };

	memset( cbuf,'\0',sizeof(cbuf) );
	memset( cip, '\0',sizeof(cip) );

    tblno = port - 1;

	if ( tcpCtrl.connectType[tblno] == 1 ) {
		GetDlgItemText( hWnd, ipTbl[tblno][0], cbuf, sizeof(cbuf) );	// IP�捞
        for(i=0; i<sizeof(cbuf); i++){
            if(cbuf[i] == 0x20) {
                cbuf[i] = 0x00;
                break;
            }
        }
		memcpy( cip, cbuf, sizeof(cip) );
	}

	GetDlgItemText( hWnd, ipTbl[tblno][1], cbuf, sizeof(cbuf) );	// PORT�捞
//		usport = atoi( cbuf );
    if(isTcpType()) {
        // TCP
//		num=sscanf(cbuf, "%s", tcp_port);
		num=sscanf(cbuf, "%s %s", tcp_port, tcp_port_c);
		usport = atoi( tcp_port );
        if(num != 2) {
            tcp_port1 = 0;     
        } else {
            tcp_port1 = atoi(tcp_port_c);
        }
	} else {
        // UDP
		num=sscanf(cbuf, "%s %s", recv_port, send_port);
		if(1 == num) {
			usport = atoi( recv_port );
			udp_send_port = atoi( recv_port );
		} else {
			usport    = atoi( recv_port );
			udp_send_port = atoi( send_port );
		}
	}
	// �\�P�b�g�̐���
//	MyMessage( hWnd, "PORT %d  �\�P�b�g�̐�����...", port );
    if(isTcpType()) {
        // TCP
//    	g_sSock[tblno] = socket( PF_INET, SOCK_STREAM, 0 );
    	g_sSock[tblno] = socket( AF_INET, SOCK_STREAM, 0 );
    } else {
        // UDP
//	    g_sSock[tblno] = socket( PF_INET, SOCK_DGRAM, 0 );
	    g_sSock[tblno] = socket( AF_INET, SOCK_DGRAM, 0 );
    }
	if ( g_sSock[tblno] == INVALID_SOCKET ) {
		MyMessage( hWnd, "PORT %d  �\�P�b�g�𐶐��ł��܂���.", port );
		return FALSE;
	}

    if(isTcpType()) {
        // TCP
    	if(isConnectType(port)) {
    		MyMessage( hWnd, "PORT %d  ���ް�֐ڑ���...", port );
            if(PCTERM == tcpCtrl.comType || 0 != tcp_port1) {      // PCTERM ? �߰Đݒ�L��H
                /* PCTERM ��connect��MyPort�� 997 �Œ�Ƃ���  */
			    /* REUSEADDR�̐ݒ� */
			    flg = 1;
			    rc = setsockopt( g_sSock[tblno], SOL_SOCKET, SO_REUSEADDR, (char *)&flg, sizeof(int) );
			    if ( rc < 0 ) {
    		        closesocket( g_sSock[tblno] );
	    	        g_sSock[tblno] = INVALID_SOCKET;
    			    MyMessage( hWnd, "PORT %d  setsockopt error");
    			    return FALSE;
			    }

        		/* LINGER�̐ݒ� */
		        l.l_onoff = 1;
		        l.l_linger = 0;
		        rc = setsockopt( g_sSock[tblno], SOL_SOCKET, SO_LINGER, (char *)&l, sizeof(l) );
		        if ( rc < 0 ) {
    		        closesocket( g_sSock[tblno] );
	    	        g_sSock[tblno] = INVALID_SOCKET;
        			MyMessage( hWnd, "PORT %d  setsockopt error");
    	    		return FALSE;
                }
        		addr.sin_family = AF_INET;
                if(PCTERM == tcpCtrl.comType) {
            		addr.sin_port = htons(997);
                } else {
            		addr.sin_port = htons((unsigned short)tcp_port1);
                }
    	    	addr.sin_addr.s_addr = htonl(INADDR_ANY);  
    		    if ( bind( g_sSock[tblno], (LPSOCKADDR)&addr, sizeof(addr)) != 0 ) {
    		        closesocket( g_sSock[tblno] );
	    	        g_sSock[tblno] = INVALID_SOCKET;
        			MyMessage( hWnd, "PORT %d  �o�C���h�G���[(bind error).", port );
        			return FALSE;
    		    }
            }

			// �T�[�o�̃A�h���X�̃Z�b�g
    		addr.sin_family = AF_INET;
    		addr.sin_port = htons( usport );
    		addr.sin_addr.s_addr = inet_addr( cip );
    		// �񓯊� select �̎��s
    		// �i�ڑ��������� FD_CONNECT ���b�Z�[�W���󂯎��j
            // SOZ�w�莞�ż�ص���s���ɉ�����۰�ޒ��Ȃ�񓯊��Ƃ��Ȃ�
            if(1 != g_SinaSozOpen) {
    		    if ( WSAAsyncSelect( g_sSock[tblno], hWnd, WM_USER_ASYNC_SELECT+port, FD_CONNECT )
    				    		== SOCKET_ERROR ) {
    		        closesocket( g_sSock[tblno] );
	    	        g_sSock[tblno] = INVALID_SOCKET;
    			    MyMessage( hWnd, "PORT %d  �񓯊�select�̎��s�G���[." , port);
    			    return FALSE;
    		    }
            }
    		// �ڑ�
    		if ( connect( g_sSock[tblno], (LPSOCKADDR)&addr, sizeof(SOCKADDR_IN) ) == SOCKET_ERROR ) {
    			nerror = WSAGetLastError( );
    			if ( nerror != WSAEWOULDBLOCK ) {			// �ڑ��u���b�N�ȊO�̃G���[
    				MyMessage( hWnd, "PORT %d  �ڑ��ł��܂���.", port );
    		        closesocket( g_sSock[tblno] );
	    	        g_sSock[tblno] = INVALID_SOCKET;
    				return FALSE;
    			}
    		}
    	} else {
    		// �T�[�o�̃A�h���X�̃Z�b�g
    		addr.sin_family = AF_INET;
    		addr.sin_port = htons(usport);
    		addr.sin_addr.s_addr = htonl(INADDR_ANY);
    
    		if ( bind( g_sSock[tblno], (LPSOCKADDR)&addr, sizeof(addr) )
    				== SOCKET_ERROR ) {
		        closesocket( g_sSock[tblno] );
		        g_sSock[tblno] = INVALID_SOCKET;
    			MyMessage( hWnd, "PORT %d  �o�C���h�G���[(bind error).", port );
    			return FALSE;
    		}
    
    		// �񓯊� select �̎��s
    		// �i�ڑ��������� FD_ACCEPT ���b�Z�[�W���󂯎��j
    		if ( WSAAsyncSelect( g_sSock[tblno], hWnd, WM_USER_ASYNC_SELECT+port, FD_ACCEPT )
    						== SOCKET_ERROR) {
   		        closesocket( g_sSock[tblno] );
    	        g_sSock[tblno] = INVALID_SOCKET;
    			MyMessage( hWnd, "PORT %d  �񓯊�select�̎��s�G���[." , port);
    			return FALSE;
    		}
    
    		if ( listen( g_sSock[tblno], 1 ) == SOCKET_ERROR ) {
   		        closesocket( g_sSock[tblno] );
    	        g_sSock[tblno] = INVALID_SOCKET;
    			MyMessage( hWnd, "PORT %d  listen() error." , port);
    			return FALSE;
    		}
    		MyMessage( hWnd, "PORT %d  �ײ��Ă���̐ڑ��v���҂�...", port );
    	}
    } else {
        // UDP
		GetDlgItemText( hWnd, ipTbl[tblno][0], cbuf, sizeof(cbuf) );	// IP�捞
        for(i=0; i<sizeof(cbuf); i++){
            if(cbuf[i] == 0x20) {
                cbuf[i] = 0x00;
                break;
            }
        }
		memcpy( cip, cbuf, sizeof(cip) );

    	ZeroMemory( &UdpMyAddr[tblno], sizeof( SOCKADDR_IN ) );
    	ZeroMemory( &UdpDsAddr[tblno], sizeof( SOCKADDR_IN ) );

        if(0 == tblno) {
    	    EnableWindow( GetDlgItem( hWnd, IDC_BUTTON_SEND ), TRUE );
        } else {
    	    EnableWindow( GetDlgItem( hWnd, IDC_BUTTON_SEND3 ), TRUE );
        }

    	//���ڑ���IP(PC)�A�h���X
    	UdpMyAddr[tblno].sin_addr.s_addr  = INADDR_ANY;
    	UdpMyAddr[tblno].sin_port         = htons(usport);
    	UdpMyAddr[tblno].sin_family       = AF_INET;
    	ZeroMemory( UdpMyAddr[tblno].sin_zero, sizeof(UdpMyAddr[tblno].sin_zero) );
    
    	if( bind( g_sSock[tblno], (SOCKADDR *)&UdpMyAddr[tblno], sizeof(SOCKADDR_IN )) ) {
	        closesocket( g_sSock[tblno] );
   	        g_sSock[tblno] = INVALID_SOCKET;
    		MyMessage( hWnd, "UDP (bind)�|�[�g�̃I�[�v���Ɏ��s");
    		return FALSE;
    	}
    	//���ڑ���IP(EPN)�A�h���X
    	UdpDsAddr[tblno].sin_addr.s_addr = inet_addr( cip );         /* �ڑ���IP�A�h���X     */
    	UdpDsAddr[tblno].sin_family      = AF_INET;                  /* �C���^�[�l�b�g�̏ꍇ */
//    	UdpDsAddr[tblno].sin_port        = htons(usport);            /* �|�[�g�ԍ�           */
    	UdpDsAddr[tblno].sin_port        = htons(udp_send_port);     /* �|�[�g�ԍ�           */
    	ZeroMemory( UdpDsAddr[tblno].sin_zero, sizeof(UdpDsAddr[tblno].sin_zero) );

		if ( WSAAsyncSelect( g_sSock[tblno], hWnd, WM_USER_ASYNC_SELECT+port, FD_READ | FD_WRITE | FD_CLOSE )
						== SOCKET_ERROR ) {
	        closesocket( g_sSock[tblno] );
   	        g_sSock[tblno] = INVALID_SOCKET;
			MyMessage( hWnd, "PORT %d  �񓯊�select�̎��s�G���[", port );
    		return FALSE;
		}
		MyMessage( hWnd, "PORT %d  �I�[�v������", port );
    }
	return TRUE;
}
//************************************
// �\�P�b�g�̃N���[�Y
//************************************
void CloseSocket( HWND hWnd, int port ) 
{
    int    tblno;

    tblno = port - 1;
	if ( g_sSockClient[tblno] != INVALID_SOCKET ) {
        closesocket( g_sSockClient[tblno] );
		g_sSockClient[tblno] = INVALID_SOCKET;

        MyMessage( hWnd, " ");
        MyMessage( hWnd, "PORT %d  �ײ��Ă���̐ڑ����Ă�۰�ނ��܂���.    [%s]", port, GetNowTime());
	}

	if ( g_sSock[tblno] != INVALID_SOCKET ) {
        if(isTcpAccept(port)) {
            if(close_btn[tblno] == 1) {
		        closesocket( g_sSock[tblno] );
		        g_sSock[tblno] = INVALID_SOCKET;
                MyMessage( hWnd, " ");
                MyMessage( hWnd, "PORT %d  ���ް���Ă�۰�ނ��܂���.               [%s]", port, GetNowTime());
//		        MyMessage( hWnd, "PORT:%d  ACCEPT SOCK �N���[�Y����", port );
            }
        } else {
	        closesocket( g_sSock[tblno] );
	        g_sSock[tblno] = INVALID_SOCKET;
            if(isTcpType()) {
                MyMessage( hWnd, " ");
                MyMessage( hWnd, "PORT %d  ���ް�֐ڑ������Ă�۰�ނ��܂���.       [%s]", port, GetNowTime());
            } else {
                MyMessage( hWnd, "PORT %d  ���Ă�۰�ނ��܂���.   [%s]", port, GetNowTime());
            }
        }
	}
    if(PORT1 == port) {
        if(isTcpType()) {
            // TCP
            // �ؒf���ݖ���connect���߂̏ꍇ�͸۰�ނ���
            // accept���߂ͻ��ް���Ă̂ݸ۰�ނ���
            if(1 == close_btn[0] || 1 == isConnectType(port)) {
                if( isConnectType(port) ) {
	      			EnableWindow( GetDlgItem( hWnd, IDC_EDIT_IP ), TRUE );
	    		}
		    	EnableWindow( GetDlgItem( hWnd, IDC_RADIO_CONNECT ), TRUE );
			    EnableWindow( GetDlgItem( hWnd, IDC_RADIO_ACCEPT ), TRUE );
			    EnableWindow( GetDlgItem( hWnd, IDC_EDIT_PORT ), TRUE );
			    EnableWindow( GetDlgItem( hWnd, IDC_BUTTON_CONNECT ), TRUE );
			    EnableWindow( GetDlgItem( hWnd, IDC_BUTTON_SEND ), FALSE );
			    EnableWindow( GetDlgItem( hWnd, IDC_BUTTON_CLOSE ), FALSE );
//		        EnableWindow( GetDlgItem( hWnd, IDC_EDIT_MSG ), FALSE );
			    if(g_sSock[1] == INVALID_SOCKET) {
				    EnableWindow( GetDlgItem( hWnd, IDC_TCP ), TRUE );
				    EnableWindow( GetDlgItem( hWnd, IDC_UDP ), TRUE );
        		    EnableWindow( GetDlgItem( hWnd, IDC_COMBO2 ), TRUE );           
			    }
            }
            if(isAcceptType(port)){
			    EnableWindow( GetDlgItem( hWnd, IDC_BUTTON_SEND ), FALSE );
            }
        } else {
            // UDP
    		EnableWindow( GetDlgItem( hWnd, IDC_EDIT_IP ), TRUE );
			EnableWindow( GetDlgItem( hWnd, IDC_EDIT_PORT ), TRUE );
			EnableWindow( GetDlgItem( hWnd, IDC_BUTTON_CONNECT ), TRUE );
			EnableWindow( GetDlgItem( hWnd, IDC_BUTTON_SEND ), FALSE );
			EnableWindow( GetDlgItem( hWnd, IDC_BUTTON_CLOSE ), FALSE );
			if(g_sSock[1] == INVALID_SOCKET) {
				EnableWindow( GetDlgItem( hWnd, IDC_TCP ), TRUE );
				EnableWindow( GetDlgItem( hWnd, IDC_UDP ), TRUE );
       		    EnableWindow( GetDlgItem( hWnd, IDC_COMBO2 ), TRUE );           
			}
		}
    }
    if(PORT2 == port) {
	    if(isTcpType()) {
            // TCP
            // �ؒf���ݖ���connect���߂̏ꍇ�͸۰�ނ���
            // accept���߂ͻ��ް���Ă̂ݸ۰�ނ���
            if(1 == close_btn[1] || 1 == isConnectType(port)) {
       			if(isConnectType(port)) {
	       			EnableWindow( GetDlgItem( hWnd, IDC_EDIT_IP3 ), TRUE );
	    		}
		    	EnableWindow( GetDlgItem( hWnd, IDC_RADIO_CONNECT3 ), TRUE );
			    EnableWindow( GetDlgItem( hWnd, IDC_RADIO_ACCEPT3 ), TRUE );
			    EnableWindow( GetDlgItem( hWnd, IDC_EDIT_PORT3 ), TRUE );
			    EnableWindow( GetDlgItem( hWnd, IDC_BUTTON_CONNECT3 ), TRUE );
	    	    EnableWindow( GetDlgItem( hWnd, IDC_BUTTON_SEND3 ), FALSE );
			    EnableWindow( GetDlgItem( hWnd, IDC_BUTTON_CLOSE3 ), FALSE );
	//	        EnableWindow( GetDlgItem( hWnd, IDC_EDIT_MSG ), FALSE );
			    if(g_sSock[0] == INVALID_SOCKET) {
				    EnableWindow( GetDlgItem( hWnd, IDC_TCP ), TRUE );
				    EnableWindow( GetDlgItem( hWnd, IDC_UDP ), TRUE );
        		    EnableWindow( GetDlgItem( hWnd, IDC_COMBO2 ), TRUE );           
			    }
            }
            if(isAcceptType(port)){
			    EnableWindow( GetDlgItem( hWnd, IDC_BUTTON_SEND3 ), FALSE );
            }
        } else {
            // UDP
    		EnableWindow( GetDlgItem( hWnd, IDC_EDIT_IP3 ), TRUE );
			EnableWindow( GetDlgItem( hWnd, IDC_EDIT_PORT3 ), TRUE );
			EnableWindow( GetDlgItem( hWnd, IDC_BUTTON_CONNECT3 ), TRUE );
			EnableWindow( GetDlgItem( hWnd, IDC_BUTTON_SEND3 ), FALSE );
			EnableWindow( GetDlgItem( hWnd, IDC_BUTTON_CLOSE3 ), FALSE );
			if(g_sSock[0] == INVALID_SOCKET) {
				EnableWindow( GetDlgItem( hWnd, IDC_TCP ), TRUE );
				EnableWindow( GetDlgItem( hWnd, IDC_UDP ), TRUE );
       		    EnableWindow( GetDlgItem( hWnd, IDC_COMBO2 ), TRUE );           
			}
		}
    }
}
//************************************
// �f�[�^�̑��M 1
//************************************
void SendMsg( HWND hWnd, int port )
{
    char    cpszmsg[256];
    char    msg[128];
    char    fname[256];
    char    slen_wk[2];
    short   slen=0;
    int     nerror;
    int     nsent=0;
    int     nstart=0;
    int     nlp1=0;
    int     nlp2=0;
    int     tblno;
    int     offset;

    if ( g_nTotalToSend == 0 )                  // ���M�f�[�^�Ȃ�
        return;                                 // �ڑ����ɂ����̊֐��͌Ă΂��

    tblno = port - 1;

    memset(msg,   0x00, sizeof(msg));
    memset(fname, 0x00, sizeof(fname));
    // ���M
    nstart = g_nTotalToSend;        // ���M������
    offset = getOffsetByComType();

    //-----------------------------------------------------
    // ���M�^�C�v����
    //-----------------------------------------------------
    if ( tcpCtrl.comType == NORMAL ) {
        // ���M�^�C�v�m�[�}��
        memcpy(g_pSendBuf, g_pSendDmy, g_nTotalToSend);
        // MyMessage( hWnd, "���M�J�n..." );
        sprintf(msg, "PORT%d �f�[�^���M", port);
    } else if( tcpCtrl.comType == MELSEC ) {
        // ���M�^�C�v�O�H�V�[�P���T
        if ( g_pSendBuf[0] != SUBHED ) {
            memset(g_pSendBuf, '\0', sizeof(g_pSendBuf));
            nstart = nstart + offset;
            slen = g_nTotalToSend / 2;
            g_pSendBuf[0] = 0x60;
            g_pSendBuf[1] = 0x00;
            memcpy(&g_pSendBuf[2], (char *)&slen, 2);
            memcpy(&g_pSendBuf[offset], g_pSendDmy, g_nTotalToSend);
            // MyMessage( hWnd, "���M�J�n..." );
            sprintf(msg, "PORT%d �f�[�^���M", port);
        } else {
            // MyMessage( hWnd, "ڽ��ݽ���M..." );
            sprintf(msg, "PORT%d ���X�|���X���M", port);
        }
    } else if( tcpCtrl.comType == RESPONS ) {
        // ���M�^�C�v���̑�
        if ( g_nTotalToSend == 2 && memcmp(g_pSendBuf ,G_respons, 2) == 0 ) {
            // MyMessage( hWnd, "ڽ��ݽ���M..." );
            sprintf(msg, "PORT%d ���X�|���X���M", port);
        } else {
            memset(g_pSendBuf, '\0', sizeof(g_pSendBuf));
            nstart = nstart + offset;
            slen = g_nTotalToSend / 2;
            g_pSendBuf[0] = G_hed[0];
            g_pSendBuf[1] = G_hed[1];
            memcpy(&g_pSendBuf[2], (char *)&slen, 2);
            memcpy(&g_pSendBuf[offset], g_pSendDmy, g_nTotalToSend);
            // MyMessage( hWnd, "���M�J�n..." );
            sprintf(msg, "PORT%d �f�[�^���M", port);
        }
    } else if( tcpCtrl.comType == PCTERM ) {
        nstart = makePcterm(hWnd, g_pSendBuf, g_pSendDmy, g_nTotalToSend, g_pcterm_pa);
        // PCTERM!!
        sprintf(msg, "PORT%d �f�[�^���M", port);
    } else if( tcpCtrl.comType == INTEL ) {
        // ���M�^�C�vINTEL�^
        if ( g_pSendBuf[0] != SUBHED ) {
            memset(g_pSendBuf, '\0', sizeof(g_pSendBuf));
            nstart = nstart + offset;
            slen = (g_nTotalToSend - 4) / 2;       // ү����No�͊܂܂Ȃ�
            g_pSendBuf[0] = 0x60;
            g_pSendBuf[1] = 0x00;
            memcpy(&g_pSendBuf[2], (char *)&slen, 2);
            memcpy(&g_pSendBuf[offset], g_pSendDmy, g_nTotalToSend);
            // MyMessage( hWnd, "���M�J�n..." );
            sprintf(msg, "PORT%d �f�[�^���M", port);
        }
    } else if( tcpCtrl.comType == X_INTEL ) {
        // ���M�^�C�v ��INTEL�^
        if ( g_pSendBuf[1] != SUBHED ) {
            memset(g_pSendBuf, '\0', sizeof(g_pSendBuf));
            nstart = nstart + offset;
            slen = (g_nTotalToSend - 4) / 2;       // ү����No�͊܂܂Ȃ�
            g_pSendBuf[0] = 0x00;
            g_pSendBuf[1] = 0x60;
            memcpy(slen_wk, (char *)&slen, 2);
            g_pSendBuf[2] = slen_wk[1];
            g_pSendBuf[3] = slen_wk[0];
            memcpy(&g_pSendBuf[offset], g_pSendDmy, g_nTotalToSend);
            // MyMessage( hWnd, "���M�J�n..." );
            sprintf(msg, "PORT%d �f�[�^���M", port);
        } else {
            // MyMessage( hWnd, "ڽ��ݽ���M..." );
            sprintf(msg, "PORT%d ���X�|���X���M", port);
        }
    } else if( tcpCtrl.comType == SOZ_FMT ) {
        // ���M�^�C�v �s�x�ڑ�̫�ϯ� intel�n
        memset(g_pSendBuf, '\0', sizeof(g_pSendBuf));
        nstart = nstart + offset;
        slen = g_nTotalToSend / 2;
        g_pSendBuf[0] = 0x60;
        g_pSendBuf[1] = 0x00;
        memcpy(&g_pSendBuf[2], (char *)&slen, 2);
        memcpy(&g_pSendBuf[offset], g_pSendDmy, g_nTotalToSend);
        // MyMessage( hWnd, "���M�J�n..." );
        sprintf(msg, "PORT%d �f�[�^���M", port);
    } else if( tcpCtrl.comType == X_SOZ_FMT ) {
        // ���M�^�C�v �s�x�ڑ�̫�ϯ� ��intel�n
        memset(g_pSendBuf, '\0', sizeof(g_pSendBuf));
        nstart = nstart + offset;
        slen = g_nTotalToSend / 2;
        g_pSendBuf[0] = 0x60;
        g_pSendBuf[1] = 0x00;
        memcpy(slen_wk, (char *)&slen, 2);
        g_pSendBuf[2] = slen_wk[1];
        g_pSendBuf[3] = slen_wk[0];
        memcpy(&g_pSendBuf[offset], g_pSendDmy, g_nTotalToSend);
        // MyMessage( hWnd, "���M�J�n..." );
        sprintf(msg, "PORT%d �f�[�^���M", port);
    } else if( tcpCtrl.comType == YASUKAWA ) {
        // ���M�^�C�v���� intel�^
        if ( g_pSendBuf[0] != 0x19 ) {
            memset(g_pSendBuf, '\0', sizeof(g_pSendBuf));
            nstart = nstart + offset;
            slen = g_nTotalToSend / 2;
            g_pSendBuf[0]  = 0x11;      // ���������
            g_pSendBuf[14] = 0x20;      // MFC
            g_pSendBuf[15] = 0x0B;      // SFC
            // ڼ޽���
            memcpy(&g_pSendBuf[20], (char *)&slen, 2);

            // LENGTH 08H+ڼ޽���*2
            slen = 8 + slen * 2;
            memcpy(&g_pSendBuf[12], (char *)&slen, 2);

            // �ް��� LENGTH+14
            slen = slen + 14;
            memcpy(&g_pSendBuf[6], (char *)&slen, 2);

            memcpy(&g_pSendBuf[offset], g_pSendDmy, g_nTotalToSend);
            // MyMessage( hWnd, "���M�J�n..." );
            sprintf(msg, "PORT%d �f�[�^���M", port);
        } else {
            // MyMessage( hWnd, "ڽ��ݽ���M..." );
            sprintf(msg, "PORT%d ���X�|���X���M", port);
        }
    } else if( tcpCtrl.comType == PASS_1TO2) {
        // ���M�^�C�v�m�[�}��
        memcpy(g_pSendBuf, g_pSendDmy, g_nTotalToSend);
        // MyMessage( hWnd, "���M�J�n..." );
        sprintf(msg, "PORT%d �f�[�^���M", port);
    } else {
        // ���M�^�C�v�m�[�}��
        memcpy(g_pSendBuf, g_pSendDmy, g_nTotalToSend);
        // MyMessage( hWnd, "���M�J�n..." );
        sprintf(msg, "PORT%d �f�[�^���M", port);
    }


    // �g���[�X�o��
    if(1 == tcpCtrl.traceOutSend) {
        if(tcpCtrl.traceSave == 1) {
//            MakeFullName(hWnd, tcpCtrl.traceName, fname);
            wkDump( hWnd, IDC_LIST_STATUS, g_pSendBuf, _DISP_OFF, tcpCtrl.traceName,
                          tcpCtrl.traceCode, nstart, msg );
        } else {
            wkDump( hWnd, IDC_LIST_STATUS, g_pSendBuf, _DISP_OFF, NULL,
                          tcpCtrl.traceCode, nstart, msg );
        }
    }

    // �ڑ��^�C�v����
    if(isTcpType()) {
        // TCP
        if (isConnectType(port)) {
            nsent = send( g_sSock[tblno], g_pSendBuf, nstart, 0 );
        } else {
            nsent = send( g_sSockClient[tblno], g_pSendBuf, nstart, 0 );
        }
    } else {
        // UDP
        nsent = sendto( g_sSock[tblno], g_pSendBuf, nstart, 0,
            (struct sockaddr *)&UdpDsAddr[tblno],   // ���M��\�P�b�g�̃A�h���X�ւ̃|�C���^
             sizeof(struct sockaddr_in));           // ���M��\�P�b�g�̃A�h���X�̃T�C�Y
    }

    if ( nsent == SOCKET_ERROR ) {
        nerror = WSAGetLastError( );
        if ( nerror != WSAEWOULDBLOCK ) {
            memset( cpszmsg, '\0', sizeof(cpszmsg) );
            wsprintf( cpszmsg, "PORT %d  ���M�G���[�@Error No. %d",port, nerror );
            MyMessage( hWnd, cpszmsg );
        }
    } else {
        memset( g_pSendBuf, '\0', sizeof(g_pSendBuf) );
        g_nTotalToSend = 0;
    }
}

//************************************
// �f�[�^�̎�M
//************************************
void RecvMsg( HWND hWnd, int port )
{
    FILE            *pass_fp;
	int  			nerror;
	int				nrecv=0;
	int				nlp1=0;
	int				nlp2=0;
	struct	timeb	tp;
	struct	tm		*ltm;
	char			recvFileName[256];
    char            fname[256];
    char            tmp[256];
	int             fp;
	int				i;
    int             loop;
    int             tblno;
    int             chkAddr;
	struct	_stat	buf;
    char            msg[128];

	int				rand_buf;
	char			cbuff[16];
    char			fbuff[FNAME_MAX];       /* ��`̧�ٖ�               */
    int             length;
    int             SockaddrSize;
    short           len;
    char            clen[2];
    int             bun;
    int             fwrite;
    int             offset;
    int             pass_offset;
    char            pass_buff[32];

    memset(msg, 0x00, sizeof(msg));
    memset(fname, 0x00, sizeof(fname));

    tblno = port - 1;

    // �f�[�^�̎�M
    memset( g_pRecvBuf, '\0', sizeof(g_pRecvBuf) );

    bun = 0;
    if(isTcpType()) {
        // TCP
        if (isConnectType(port)) {
            nrecv = recv( g_sSock[tblno], g_pRecvBuf[tblno], (sizeof(g_pRecvBuf)/2) - 1, 0 );
        } else {
            nrecv = recv( g_sSockClient[tblno], g_pRecvBuf[tblno], (sizeof(g_pRecvBuf)/2) - 1, 0 );
        }
    } else {
        // UDP
        SockaddrSize = sizeof(struct sockaddr_in);

        nrecv = recvfrom( g_sSock[tblno],                 // �\�P�b�g�iSoket Descriptor�j
                        g_pRecvBuf[tblno],               // ��M�o�b�t�@�ւ̃|�C���^
                        (sizeof(g_pRecvBuf)/2) - 1,      // ��M�o�b�t�@�̃T�C�Y
                        0,                               // �Ăяo�����@�̎w��iMSG_PEEK , MSG_OOB�j
                        (struct sockaddr *)&UdpDsAddr[tblno],   // ���M���̃A�h���X�����o�b�t�@�ւ̃|�C���^
                        &SockaddrSize );                 // ���M���̃A�h���X�����o�b�t�@�̃T�C�Y�ւ̃|�C���^
    }

    if ( nrecv == SOCKET_ERROR ) {
        nerror = WSAGetLastError( );
        if ( nerror != WSAEWOULDBLOCK ) {
            MyMessage( hWnd, "PORT %d  ��M�G���[�@Error No. %d", port, nerror );
        }
        return;
    }

    // ڽ��ݽ��M����  �ڰ��o�͂��I��
    if(( g_pRecvBuf[tblno][0] == 0xE0 && tcpCtrl.comType == MELSEC   ) ||
       ( g_pRecvBuf[tblno][0] == 0xE0 && tcpCtrl.comType == INTEL    ) || 
       ( g_pRecvBuf[tblno][1] == 0xE0 && tcpCtrl.comType == X_INTEL  ) ||
       ( g_pRecvBuf[tblno][0] == 0x19 && tcpCtrl.comType == YASUKAWA ) ||
       ( g_pRecvBuf[tblno][0] == G_respons[0] && tcpCtrl.comType == RESPONS && nrecv == 2 )){
        // MyMessage( hWnd, "PORT %d  ڽ��ݽ��M", port );
        // �g���[�X�o��
        if(1 == tcpCtrl.traceOutRecv) {
            sprintf(msg, "PORT%d  ���X�|���X��M", port);
            if(tcpCtrl.traceSave == 1) {
//                    MakeFullName(hWnd, tcpCtrl.traceName, fname);
                wkDump( hWnd, IDC_LIST_STATUS, g_pRecvBuf[tblno], _DISP_OFF, tcpCtrl.traceName,
                              tcpCtrl.traceCode, nrecv, msg );
            } else {
                wkDump( hWnd, IDC_LIST_STATUS, g_pRecvBuf[tblno], _DISP_OFF, NULL,
                              tcpCtrl.traceCode, nrecv, msg );
            }
        }
        return;
    }

    // ---------------------------------------------------------
    // �ʏ�`����M����
    // ---------------------------------------------------------
    g_pRecvBuf[tblno][nrecv] = '\0';
    // �g���[�X�o��
    if(1 == tcpCtrl.traceOutRecv) {
        sprintf(msg, "PORT%d  �f�[�^��M", port);
        if(tcpCtrl.traceSave == 1) {
            wkDump( hWnd, IDC_LIST_STATUS, g_pRecvBuf[tblno], _DISP_OFF, tcpCtrl.traceName,
                          tcpCtrl.traceCode, nrecv, msg );
        } else {
            wkDump( hWnd, IDC_LIST_STATUS, g_pRecvBuf[tblno], _DISP_OFF, NULL,
                          tcpCtrl.traceCode, nrecv, msg );
        }
    }

    // ��M�f�[�^�t�@�C���i�[
    memset(recvFileName, 0x00, sizeof(recvFileName));
    if(tcpCtrl.recvChkFile == 1) {
        fwrite = 0;
        if(tcpCtrl.recvChkFileName == 1 && 0x00 != tcpCtrl.recvFileName[0]) {
            memset(recvFileName, 0x00, sizeof(recvFileName));
            memcpy(recvFileName, tcpCtrl.recvFileName, strlen(tcpCtrl.recvFileName));
            if(_stat(recvFileName, &buf) < 0) {
                // nothing to do
            } else {
                unlink(recvFileName);
            }
            fwrite = 1;
        }
        if(tcpCtrl.recvChkFileName == 0 && 0x00 != tcpCtrl.recvFileBinName[0]) {
            memset(recvFileName, 0x00, sizeof(recvFileName));
            ftime(&tp);
            ltm = localtime(&tp.time);

            sprintf(recvFileName, "%srecv%02d%02d%02d_%03d.bin",
                    tcpCtrl.recvFileBinName,
                    ltm->tm_hour, ltm->tm_min, ltm->tm_sec,tp.millitm);

//                  MakeFullName(hWnd, tmp, recvFileName);

            if(_stat(recvFileName, &buf) < 0) {
                // �쐬�����i�[̧�ٖ������ɑ��݂��Ȃ���΂���̧�ٖ��Ŋi�[����B
            } else {
                // �쐬�����i�[̧�ٖ������݂��Ă���ꍇ�́A�ǔԂ�ǉ�����B
                for(i = 1; i<100; i++) {
                    memset(recvFileName, 0x00, sizeof(recvFileName));
                    sprintf(tmp, "%srecv%02d%02d%02d_%03d_%02d.bin",
                             tcpCtrl.recvFileBinName,
                             ltm->tm_hour, ltm->tm_min, ltm->tm_sec,tp.millitm, i);
//                          MakeFullName(hWnd, tmp, recvFileName);

                    // �ǔԂ�ǉ�����̧�ٖ��̑�������
                    if(_stat(recvFileName, &buf) < 0) {
                        // ̧�ق����݂��Ȃ��ꍇ�A̧�ٖ�����
                            break;
                    }
                }
            }
            fwrite = 1;
        }
        if(1 == fwrite) {
            fp = AP_Sfopen(recvFileName, AP_CREATRDWR);
            if(fp > 0) {
                AP_Sprec(fp, nrecv, 0, g_pRecvBuf[tblno]);
                AP_Sfclose(fp);
                MyMessage(hWnd, "PORT %d  ��M�ް��� [%s] ̧�قɏo�͂��܂���", port, recvFileName);
            }
        }
    }

    // ���X�|���X���M�v������
    if ( tcpCtrl.comType == MELSEC ) {
        memset( g_pSendBuf, '\0', sizeof(g_pSendBuf) );
        g_pSendBuf[0] = SUBHED;
        g_pSendBuf[1] = RESP_OK;
        g_nTotalToSend = 2;
        if (isConnectType(port)) {
            SendMsg(hWnd, port );
//          PostMessage( hWnd, WM_USER_ASYNC_SELECT+port, g_sSock[tblno], WSAMAKESELECTREPLY( FD_WRITE, 0 ));
        } else {
            SendMsg(hWnd, port );
//          PostMessage( hWnd, WM_USER_ASYNC_SELECT+port, g_sSockClient[tblno], WSAMAKESELECTREPLY( FD_WRITE, 0 ));
        }
    } else if( tcpCtrl.comType == RESPONS ) {
        memset( g_pSendBuf, '\0', sizeof(g_pSendBuf) );
        g_pSendBuf[0] = G_respons[0];
        g_pSendBuf[1] = G_respons[1];
        g_nTotalToSend = 2;
        if (isConnectType(port)) {
            SendMsg(hWnd, port );
//          PostMessage( hWnd, WM_USER_ASYNC_SELECT+port, g_sSock[tblno], WSAMAKESELECTREPLY( FD_WRITE, 0 ));
        } else {
            SendMsg(hWnd, port );
//          PostMessage( hWnd, WM_USER_ASYNC_SELECT+port, g_sSockClient[tblno], WSAMAKESELECTREPLY( FD_WRITE, 0 ));
        }
    } else if ( tcpCtrl.comType == INTEL ) {
        memset( g_pSendBuf, '\0', sizeof(g_pSendBuf) );
        g_pSendBuf[0] = SUBHED;
        g_pSendBuf[1] = RESP_OK;
        g_nTotalToSend = 2;
        if (isConnectType(port)) {
            SendMsg(hWnd, port );
//          PostMessage( hWnd, WM_USER_ASYNC_SELECT+port, g_sSock[tblno], WSAMAKESELECTREPLY( FD_WRITE, 0 ));
        } else {
            SendMsg(hWnd, port );
//          PostMessage( hWnd, WM_USER_ASYNC_SELECT+port, g_sSockClient[tblno], WSAMAKESELECTREPLY( FD_WRITE, 0 ));
        }
    } else if ( tcpCtrl.comType == X_INTEL ) {
        memset( g_pSendBuf, '\0', sizeof(g_pSendBuf) );
        g_pSendBuf[0] = RESP_OK;
        g_pSendBuf[1] = SUBHED;
        g_nTotalToSend = 2;
        if (isConnectType(port)) {
            SendMsg(hWnd, port );
//          PostMessage( hWnd, WM_USER_ASYNC_SELECT+port, g_sSock[tblno], WSAMAKESELECTREPLY( FD_WRITE, 0 ));
        } else {
            SendMsg(hWnd, port );
//          PostMessage( hWnd, WM_USER_ASYNC_SELECT+port, g_sSockClient[tblno], WSAMAKESELECTREPLY( FD_WRITE, 0 ));
        }
    } else if ( tcpCtrl.comType == YASUKAWA ) {
        memset( g_pSendBuf, '\0', sizeof(g_pSendBuf) );
        g_pSendBuf[0] = 0x19;
        g_pSendBuf[1]  = g_pRecvBuf[tblno][1];
        g_pSendBuf[2]  = g_pRecvBuf[tblno][2];
        g_pSendBuf[6]  = 0x16;
        g_pSendBuf[12] = 0x08;
        g_pSendBuf[14] = 0x20;
        g_pSendBuf[15] = 0x0B;
        g_pSendBuf[18] = g_pRecvBuf[tblno][18];
        g_pSendBuf[19] = g_pRecvBuf[tblno][19];
        g_pSendBuf[20] = g_pRecvBuf[tblno][20];
        g_pSendBuf[21] = g_pRecvBuf[tblno][21];
    
        g_nTotalToSend = 22;
        if (isConnectType(port)) {
            SendMsg(hWnd, port );
//          PostMessage( hWnd, WM_USER_ASYNC_SELECT+port, g_sSock[tblno], WSAMAKESELECTREPLY( FD_WRITE, 0 ));
        } else {
            SendMsg(hWnd, port );
//          PostMessage( hWnd, WM_USER_ASYNC_SELECT+port, g_sSockClient[tblno], WSAMAKESELECTREPLY( FD_WRITE, 0 ));
        }
    } else if ( tcpCtrl.comType == PASS_1TO2 ) {
        if(port == PORT1) {
            if (isConnectPort(PORT2)) {
                pass_offset = 0;
	            if((pass_fp=fopen("PASS_OFFSET", "rt")) == NULL) {
		            wsprintf(MsgBuf, "%s �t�@�C���I�[�v���G���[", "PASS_OFFSET");
		            MessageBox(hWnd, MsgBuf, "�e�L�X�g�t�@�C���Ǎ���", MB_ICONEXCLAMATION | MB_OK);
                } else {
                    memset(pass_buff, 0x00, sizeof(pass_buff));
	                fgets(pass_buff, 16, pass_fp);        
	                fclose(pass_fp);
                    pass_offset = atoi(pass_buff);
                    if( 0 > pass_offset) pass_offset = 0;
                }
//                MyMessage( hWnd, "pass_offset=%d", pass_offset);

                if(0 < nrecv - pass_offset) { 
                    memset( g_pSendDmy, '\0', sizeof(g_pSendDmy) );
                    memcpy( g_pSendDmy, &g_pRecvBuf[tblno][pass_offset], nrecv - pass_offset );
                    g_nTotalToSend = nrecv - pass_offset;
                    SendMsg(hWnd, PORT2);
//                  PostMessage( hWnd, WM_USER_ASYNC_SELECT+port, g_sSock[tblno], WSAMAKESELECTREPLY( FD_WRITE, 0 ));
                }
            }
        }
    }
    if ( tcpCtrl.schCtrl == 1 ) {
        // �`�����߂ɂ��̾�Ď�肾��
        offset = getOffsetByComType();

        for(loop=0; loop<SCH_TBL_MAX; loop++) {
            if(CntDef[loop].item[0].type != 0) {
                // �������ڽ�v�Z
                chkAddr = CntDef[loop].item[0].addr + offset;
                if(0 == memcmp(CntDef[loop].item[0].ptn, &g_pRecvBuf[tblno][chkAddr], CntDef[loop].item[0].length)) {
                    if(1 < CntDef[loop].num) {
                        if(-1 == CntDefSub(tblno, loop, chkAddr)) {
                            continue;
                        }
                    }
                    CntFilePut(hWnd, tblno, loop);
                }
            }
        }

MURIYARI:
        for(loop=0; loop<SCH_TBL_MAX; loop++) {
            if(SchDef[loop].item[0].type == 0) {
                continue;
            }

            // �������ڽ�v�Z
            chkAddr = SchDef[loop].item[0].addr + offset;
            if(0 == memcmp(SchDef[loop].item[0].ptn, &g_pRecvBuf[tblno][chkAddr], SchDef[loop].item[0].length)) {
                if(1 < SchDef[loop].num) {
                    if(-1 == SchDefSub(tblno, loop, chkAddr)) {
                        continue;
                    }
                }

                length = strlen(SchDef[loop].fname);
// Ver3.46,Ver3.4  --->>>
                // ��M�f�[�^�ɂ���ĕԐM�����`�t�@�C���̖��̂������_���ŕύX����B
                if(SchDef[loop].fnum > 0 && SchDef[loop].fnum <= 10) {
                    memset(fbuff, 0x00, sizeof(fbuff));
                    memcpy(fbuff, SchDef[loop].fname, FNAME_MAX);
                    if(SchDef[loop].randam != 0) {
                        // �����
                        rand_buf = rand() % SchDef[loop].fnum;
                        if(0 == memcmp(&SchDef[loop].fname[length-3], "bin", 3)) {
                            sprintf(cbuff, "%d.bin", rand_buf+1);
                        } else if(0 == memcmp(&SchDef[loop].fname[length-3], "def", 3)) {
                            sprintf(cbuff, "%d.def", rand_buf+1);
                        } else if(0 == memcmp(&SchDef[loop].fname[length-3], "txt", 3)) {
                            sprintf(cbuff, "%d.txt", rand_buf+1);
                        }
                    } else {
                        // ���ݼ��
                        if(Sch_no[loop] > SchDef[loop].fnum) {
                            Sch_no[loop] = 1;
                        }
                        if(0 == memcmp(&SchDef[loop].fname[length-3], "bin", 3)) {
                            sprintf(cbuff, "%d.bin", Sch_no[loop]);
                        } else if(0 == memcmp(&SchDef[loop].fname[length-3], "def", 3)) {
                            sprintf(cbuff, "%d.def", Sch_no[loop]);
                        } else if(0 == memcmp(&SchDef[loop].fname[length-3], "txt", 3)) {
                            sprintf(cbuff, "%d.txt", Sch_no[loop]);
                        }
                        Sch_no[loop]++;
                    }
                    memcpy(&SchDef[loop].fname[strlen(SchDef[loop].fname)-5], cbuff, 4);
                }
// Ver3.46,Ver3.47 ---<<<
                if(0 == memcmp(&SchDef[loop].fname[length-3], "bin", 3)) {
                    if(FALSE == SchBinDataRead(hWnd, SchDef[loop].fname)) {
                        return;
                    }
                    if(0 == SchDef[loop].port) {
                        SendMsg(hWnd, port );
                    } else {
                        SendMsg(hWnd, SchDef[loop].port );
                    }
                } else if(0 == memcmp(&SchDef[loop].fname[length-3], "def", 3)) {
                    if(FALSE == MakeDefData(hWnd, SchDef[loop].fname, g_pRecvBuf[tblno])) {
                        return;
                    }
                    if(0 == SchDef[loop].port) {
                        SendMsg(hWnd, port );
                    } else {
                        SendMsg(hWnd, SchDef[loop].port );
                    }
                } else if(0 == memcmp(&SchDef[loop].fname[length-3], "txt", 3)) {
                    if(FALSE == MakeRecvSinaData(hWnd, SchDef[loop].fname, g_pRecvBuf[tblno], port)) {
                        return;
                    }
                }
            }
        }
        // ��PCTERM�p�̓`������������鎞�̏���
        // ��������������ł� (^^�U
        if(PCTERM == tcpCtrl.comType) {
            memset(g_pRecvWork, 0x00, sizeof(g_pRecvWork));
            clen[0] = g_pRecvBuf[tblno][3];
            clen[1] = g_pRecvBuf[tblno][2];
            memcpy(&len, clen, 2);
            len = len + 6;
            /* �����d�l ��M�ް�������޲Ă̎��A1�޲ĉ��Z���� */
            len = len + (len % 2);
            /* �����d�l ��M�ް�������޲Ă̎��A1�޲ĉ��Z���� */

            //MyMessage(hWnd, "len = [%d]  nrecv=%d", len, nrecv);
            if(len < nrecv) {
                nrecv = nrecv - len;
                memcpy(g_pRecvWork, &g_pRecvBuf[tblno][len], nrecv);
                memset(g_pRecvBuf[tblno], 0x00, sizeof(g_pRecvBuf[tblno]));
                memcpy(g_pRecvBuf[tblno], g_pRecvWork, nrecv);
                MyMessage(hWnd, "����  ��M�ް��̕�������  ���s�I�I ����");
                goto MURIYARI;
            }
        }
    }
}


int SchDefSub(int tblno, int addr, int chkAddr1)
{
    int     offset;
    int     chkAddr;

    // �`�����߂���̾�Ď��o��
    offset = getOffsetByComType();

    if(2 <= SchDef[addr].num) {
        chkAddr = SchDef[addr].item[1].addr + offset;
        if(0 != memcmp(SchDef[addr].item[1].ptn, &g_pRecvBuf[tblno][chkAddr], SchDef[addr].item[1].length)) {
            return -1;
        }
    }
    if(3 <= SchDef[addr].num) {
        chkAddr = SchDef[addr].item[2].addr + offset;
        if(0 != memcmp(SchDef[addr].item[2].ptn, &g_pRecvBuf[tblno][chkAddr], SchDef[addr].item[2].length)) {
            return -1;
        }
    }
    if(4 <= SchDef[addr].num) {
        chkAddr = SchDef[addr].item[3].addr + offset;
        if(0 != memcmp(SchDef[addr].item[3].ptn, &g_pRecvBuf[tblno][chkAddr], SchDef[addr].item[3].length)) {
            return -1;
        }
    }
    if(5 <= SchDef[addr].num) {
        chkAddr = SchDef[addr].item[4].addr + offset;
        if(0 != memcmp(SchDef[addr].item[4].ptn, &g_pRecvBuf[tblno][chkAddr], SchDef[addr].item[4].length)) {
            return -1;
        }
    }

    return 0;
}



//----------------------------------------------------------------------------
//
//	�ʐM�ݒ�\������
//
//----------------------------------------------------------------------------
BOOL	DispComCtrl(HWND hWnd)
{
	char	work[40];

	if(tcpCtrl.traceCode == 0){
		wsprintf(work, " �`�r�b�h�h" );
		SetDlgItemText(hWnd, IDC_EDIT_PORT2, work);
	}else if(tcpCtrl.traceCode == 2){
		wsprintf(work, "�d�a�b�c�h�b");
		SetDlgItemText(hWnd, IDC_EDIT_PORT2, work);
	}else if(tcpCtrl.traceCode == 1){
		wsprintf(work, " S_JIS����");
		SetDlgItemText(hWnd, IDC_EDIT_PORT2, work);
	}

	return TRUE;
}


//************************************
// �\�P�b�g���b�Z�[�W����
// �����FTRUE
// ���s�FFALSE
//************************************
BOOL OnAsyncSelect( HWND hWnd, LPARAM lParam, int port )
{
    int    tblno;

    tblno = port - 1;

	// �G���[�̗L���̒���
	if ( WSAGETSELECTERROR( lParam ) != 0 ) {
        MyMessage( hWnd, "PORT %d  async select �G���[" , port);

		CloseSocket(hWnd,  port);

        // TCP �̎��A�Đڑ����g���C�������s��
        if(isTcpConnect(port)) {
            if(1 == tcpCtrl.retry_flg) {
                if(PORT1 == port) {
		            timerSetProc3(hWnd, RETRY_TIMER_ID1);
                } else {
    		        timerSetProc3(hWnd, RETRY_TIMER_ID2);
                }
            }
        }
		return TRUE;
	}

	switch ( WSAGETSELECTEVENT( lParam ) ) {
	case FD_CONNECT:		// �ڑ�����
		MyMessage( hWnd, "PORT %d  ���ް�֐ڑ��������܂���.  [%s]", port, GetNowTime() );
		if ( WSAAsyncSelect( g_sSock[tblno], hWnd, WM_USER_ASYNC_SELECT+port, FD_READ | FD_WRITE | FD_CLOSE )
						== SOCKET_ERROR ) {
			MyMessage( hWnd, "PORT %d  �񓯊�select�̎��s�G���[", port );
		} else {
            if(PORT1 == port) {
    			EnableWindow( GetDlgItem( hWnd, IDC_RADIO_CONNECT ), FALSE );
	    		EnableWindow( GetDlgItem( hWnd, IDC_RADIO_ACCEPT ), FALSE );
		    	EnableWindow( GetDlgItem( hWnd, IDC_BUTTON_CONNECT ), FALSE );
			    EnableWindow( GetDlgItem( hWnd, IDC_BUTTON_CLOSE ), TRUE );
			    EnableWindow( GetDlgItem( hWnd, IDC_BUTTON_SEND ), TRUE );
//			    EnableWindow( GetDlgItem( hWnd, IDC_EDIT_MSG ), TRUE );
                timerResetProc3(hWnd, RETRY_TIMER_ID1);
            }
            if(PORT2 == port) {
    			EnableWindow( GetDlgItem( hWnd, IDC_RADIO_CONNECT3 ), FALSE );
	    		EnableWindow( GetDlgItem( hWnd, IDC_RADIO_ACCEPT3 ), FALSE );
		    	EnableWindow( GetDlgItem( hWnd, IDC_BUTTON_CONNECT3 ), FALSE );
			    EnableWindow( GetDlgItem( hWnd, IDC_BUTTON_CLOSE3 ), TRUE );
			    EnableWindow( GetDlgItem( hWnd, IDC_BUTTON_SEND3 ), TRUE );
//			    EnableWindow( GetDlgItem( hWnd, IDC_EDIT_MSG ), TRUE );
                timerResetProc3(hWnd, RETRY_TIMER_ID2);
            }
		}
		break;

	case FD_ACCEPT:		// �ڑ�����
		// MyMessage(hWnd, "PORT %d  �ڑ����� ACCEPT", port);
		if ( OnAccept( hWnd, port ) == FALSE ) {
			PostMessage( hWnd, WM_USER_ASYNC_SELECT+port, g_sSock[tblno], WSAMAKESELECTREPLY( FD_CLOSE, 0 ));
		} else {
            if(PORT1 == port) {
    			EnableWindow( GetDlgItem( hWnd, IDC_RADIO_CONNECT ), FALSE );
	    		EnableWindow( GetDlgItem( hWnd, IDC_RADIO_ACCEPT ), FALSE );
		    	EnableWindow( GetDlgItem( hWnd, IDC_BUTTON_CONNECT ), FALSE );
			    EnableWindow( GetDlgItem( hWnd, IDC_BUTTON_CLOSE ), TRUE );
			    EnableWindow( GetDlgItem( hWnd, IDC_BUTTON_SEND ), TRUE );
//			    EnableWindow( GetDlgItem( hWnd, IDC_EDIT_MSG ), TRUE );
            }
            if(PORT2 == port) {
    			EnableWindow( GetDlgItem( hWnd, IDC_RADIO_CONNECT3 ), FALSE );
	    		EnableWindow( GetDlgItem( hWnd, IDC_RADIO_ACCEPT3 ), FALSE );
		    	EnableWindow( GetDlgItem( hWnd, IDC_BUTTON_CONNECT3 ), FALSE );
			    EnableWindow( GetDlgItem( hWnd, IDC_BUTTON_CLOSE3 ), TRUE );
			    EnableWindow( GetDlgItem( hWnd, IDC_BUTTON_SEND3 ), TRUE );
//			    EnableWindow( GetDlgItem( hWnd, IDC_EDIT_MSG ), TRUE );
            }
		}
		break;

	case FD_CLOSE:			// �T�[�o����̐ؒf
		CloseSocket(hWnd,  port);
//		MyMessage( hWnd, "PORT %d  �ؒf���܂���.     [%s]", port, GetNowTime() );

        // TCP & CONNECT �̎��A�Đڑ����g���C�������s��
        if(1 == isTcpConnect(port) && 1 == tcpCtrl.retry_flg) {
            if(PORT1 == port) {
                timerSetProc3(hWnd, RETRY_TIMER_ID1);
            } else {
                timerSetProc3(hWnd, RETRY_TIMER_ID2);
            }
        }

		break;
	case FD_WRITE:			// ���M
		SendMsg( hWnd, port );
		break;
	case FD_READ:			// ��M
		RecvMsg( hWnd, port );
		break;
	default:
		return FALSE;
	}
	return TRUE;
}
//************************************
// �N���C�A���g����̐ڑ�
// �����FTRUE
// ���s�FFALSE
//************************************
BOOL OnAccept( HWND hWnd, int port )
{
	int			naddrlen;
	int			nerror;
//	char		cpszmsg[100];
	IN_ADDR		in_addr;
	SOCKADDR_IN	addr;
    int         tblno;

    tblno = port - 1;

	nerror = 0;
	if ( g_sSockClient[tblno] == INVALID_SOCKET ) {
		naddrlen = sizeof( addr );
		g_sSockClient[tblno] = accept( g_sSock[tblno], (LPSOCKADDR)&addr, &naddrlen );
		if ( g_sSockClient[tblno] == INVALID_SOCKET ) {
			nerror = WSAGetLastError( );
			if ( nerror != WSAEWOULDBLOCK) {			// �ڑ��u���b�N�ȊO�̃G���[
				MyMessage( hWnd, "PORT %d  �ڑ��ł��܂���(accept error).", port );
				return FALSE;
			}
		} else {
			// �ڑ�����
			memcpy( &in_addr, &addr.sin_addr.s_addr, 4 );
        	MyMessage( hWnd, "�|�|�|�|�|�|�|�|�|�|�|�|�|�|�|�|�|�|�|�|�|�|�|�|�|�|�|�|�|�|�|�|�|�|�|�|�|�|�|�|�|�|�|");
			MyMessage( hWnd, "PORT %d  �ײ��� [%s] ����ڑ�����܂����D(port:%d)  [%s]",port, inet_ntoa(in_addr), ntohs(addr.sin_port), GetNowTime());
//			MyMessage( hWnd, "PORT %d  �ײ��� [%s] ����ڑ�����܂����D[%s]",port, inet_ntoa(in_addr), GetNowTime());
			// �񓯊� select �̎��s
			// �i�ڑ��������� FD_ACCEPT ���b�Z�[�W���󂯎��j
			if ( WSAAsyncSelect( g_sSockClient[tblno], hWnd, WM_USER_ASYNC_SELECT+port, FD_READ | FD_WRITE | FD_CLOSE )
							== SOCKET_ERROR) {
				MyMessage( hWnd, "PORT %d  �񓯊�select�̎��s�G���[." , port);
				return FALSE;
			}
            if(PORT1 == port) {
                timerResetProc3(hWnd, RETRY_TIMER_ID1);
            } else {
                timerResetProc3(hWnd, RETRY_TIMER_ID2);
            }
		}
	}
	return TRUE;
}
//************************************
// �f�[�^�\��
//************************************
void MyMessage( HWND hWnd, const char *__cBuffer, ... )
{
	int				ikensu=0;
	unsigned char	cbuff[2048];
	va_list			vmarker;

	memset( cbuff, '\0', sizeof(cbuff));
	va_start( vmarker, __cBuffer );   										// �ό̈����̏�����
	vsprintf( cbuff, __cBuffer, vmarker );
	va_end( vmarker );              										// �ό̈����̃��Z�b�g
	// ؽĕ\��
	SendDlgItemMessage( hWnd, IDC_LIST_STATUS, LB_ADDSTRING, 0, (LPARAM)(unsigned char *)cbuff );
	ikensu = SendDlgItemMessage( hWnd, IDC_LIST_STATUS, LB_GETCOUNT, 0, 0 );		// ؽĕ\�������擾
	SendDlgItemMessage( hWnd, IDC_LIST_STATUS, LB_SETCURSEL, ikensu-1, 0 );			// �ŐV���ް��ɶ��پ��
	// ؽĕ\����������
	if( ikensu > uiListMaxCt ){
		SendDlgItemMessage( hWnd, IDC_LIST_STATUS, LB_DELETESTRING, (WPARAM)0, 0 );	// �ŌÂ��ް�������
	}
}

/*--------------------------------------------------------------
* �t�@�C���ǂݍ��ݏ���
*--------------------------------------------------------------*/
void getFileData(HWND hWnd)
{
	OPENFILENAME	ofn;
    int             i;
	unsigned char   dirName[256];
	unsigned char   tempName[256];
	int             len, set, fileChk;

	memset(g_OpenName, 0x00, sizeof(g_OpenName));
	memset(&ofn, 0, sizeof(OPENFILENAME));
	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = hWnd;
	// �t�@�C����ނ̎w��
	ofn.lpstrFilter = "�ް�̧�ف@  {*.txt *.def *.bin}\0*.txt; *.def; *.bin\0"\
					  "  C ̧��     {*.c}\0*.c; *.cpp; *.h\0"\
					  "�S�Ă�̧�� {*.*}\0*.*\0\0";

	ofn.lpstrFile	= g_OpenName;
//	ofn.nMaxFile = MAX_PATH;
	ofn.nMaxFile = sizeof(g_OpenName);

//	ofn.Flags = OFN_FILEMUSTEXIST;
	ofn.Flags = OFN_FILEMUSTEXIST | OFN_ALLOWMULTISELECT | OFN_LONGNAMES | OFN_EXPLORER;
	// �g���q�����͂���Ȃ�������̫�Ă̊g���q
	ofn.lpstrDefExt = "txt";
	if(GetOpenFileName(&ofn)){
		memset(dirName, 0x00, sizeof(dirName));
		strcpy(dirName, g_OpenName);
		len = strlen(g_OpenName);
//		MyMessage( hWnd, "[%s]", &g_OpenName[len+1]);
		if(0x00 == g_OpenName[len+1] && 0x00 == g_OpenName[len+2]) {
			strcpy(fileName, g_OpenName);
	        for(i=0; (unsigned int)i<strlen(g_OpenName); i++) {
		        if('.' == g_OpenName[i]) {
			        if(0 == strcmp(&g_OpenName[i], ".txt")) {
				        tcpCtrl.fileForm = 1;
					    FileTypeTextRead(hWnd);
						break;
					} else if(0 == strcmp(&g_OpenName[i], ".def")) {
						tcpCtrl.fileForm = 2;
					    FileTypeDefNameRead(hWnd);
						break;
					} else if(0 == strcmp(&g_OpenName[i], ".bin")) {
						tcpCtrl.fileForm = 3;
						FileTypeBinNameRead(hWnd);
						break;
					}
				}
			}
		} else {
			memset(fileName, 0x00, sizeof(fileName));
			memset(tempName, 0x00, sizeof(tempName));
			set = 0;
			fileChk = 0;
		    for(i=len+1; i<sizeof(g_OpenName); i++) {
				tempName[set] = g_OpenName[i];
		        if('.' == g_OpenName[i] && 0 == fileChk) {
			        if(0 == strcmp(&g_OpenName[i], ".txt")) {
				        tcpCtrl.fileForm = 1;
					} else if(0 == strcmp(&g_OpenName[i], ".def")) {
						tcpCtrl.fileForm = 2;
					} else if(0 == strcmp(&g_OpenName[i], ".bin")) {
						tcpCtrl.fileForm = 3;
					}
				}

				if(0x00 == g_OpenName[i]) {
					sprintf(fileName, "%s\\%s", dirName, tempName);
					if(tcpCtrl.fileForm == 1) {
					    FileTypeTextRead(hWnd);
					} else if(tcpCtrl.fileForm == 2) {
						FileTypeDefNameRead(hWnd);
					} else if(tcpCtrl.fileForm == 3) {
						FileTypeBinNameRead(hWnd);
					}
					memset(fileName, 0x00, sizeof(fileName));
					memset(tempName, 0x00, sizeof(tempName));
					set = 0;
					if(1 == fileChk) break;
				} else {
					set++;
				}
				if(0x00 == g_OpenName[i] && 0x00 == g_OpenName[i+1]) break;
			}
		}
	} else {
		if(CommDlgExtendedError()) {
			MyMessage( hWnd, "GetOpenFileName�G���[. (%04x)", CommDlgExtendedError());
		}
	}
}


//----------------------------------------------------------------------------
//
//	�e�L�X�g�t�@�C���Ǎ��ݏ���
//
//----------------------------------------------------------------------------
BOOL	FileTypeTextRead(HWND hWnd)
{
	FILE		*fp;

	unsigned	char	FileBuff[10250];

	////////////////////////////////////////////////////////////////////////
	//�@�t�@�C���Ǎ��݁����X�g�a�n�w�\��
	////////////////////////////////////////////////////////////////////////
	if((fp=fopen(fileName, "rt")) == NULL) {
		wsprintf(MsgBuf, "%s �t�@�C���I�[�v���G���[", fileName);
		MessageBox(hWnd, MsgBuf, "�e�L�X�g�t�@�C���Ǎ���", MB_ICONEXCLAMATION | MB_OK);
		return FALSE;
	}

	while(fgets(FileBuff, COM_BUF_MAX, fp)) {

		// 0x0A�R�[�h�폜
		if(FileBuff[strlen(FileBuff)-1] < 0x20) {
			FileBuff[strlen(FileBuff)-1] = 0x00;
		}
		SendDlgItemMessage(hWnd, IDC_LIST_DATA, LB_ADDSTRING, 0, (LPARAM)FileBuff);
	}
	fclose(fp);

	return TRUE;
}

//----------------------------------------------------------------------------
//
//	��`�t�@�C�����̎�荞�ݏ���
//
//----------------------------------------------------------------------------
BOOL	FileTypeDefNameRead(HWND hWnd)
{
	////////////////////////////////////////////////////////////////////////
	//�@�t�@�C���Ǎ��݁����X�g�a�n�w�\��
	////////////////////////////////////////////////////////////////////////
	SendDlgItemMessage(hWnd, IDC_LIST_DATA, LB_ADDSTRING, 0, (LPARAM)fileName);

	return TRUE;
}

//----------------------------------------------------------------------------
//
//	�o�C�i���B�t�@�C�����̎�荞�ݏ���
//
//----------------------------------------------------------------------------
BOOL	FileTypeBinNameRead(HWND hWnd)
{
	////////////////////////////////////////////////////////////////////////
	//�@�t�@�C���Ǎ��݁����X�g�a�n�w�\��
	////////////////////////////////////////////////////////////////////////
	SendDlgItemMessage(hWnd, IDC_LIST_DATA, LB_ADDSTRING, 0, (LPARAM)fileName);

	return TRUE;
}

//----------------------------------------------------------------------------
//
//	���M�{�^����������
//
//----------------------------------------------------------------------------
BOOL	SendBtnProc(HWND hWnd)
{
	int				iSel;
    char            fname[512];

    memset(fname, 0x00, sizeof(fname));
    // ���M�敪���t�@�C���f�[�^�Ƀ`�F�b�N����Ă��邩����
	if(SendDlgItemMessage(hWnd, IDC_RADIO2_FILE, BM_GETCHECK, 0, 0)) {
        // �I�����ꂽ���X�g�a�n�w�̓��e���`�F�b�N���āA�t�@�C���敪��ύX���� 
        //          �i�g���q�ɂ��ύX����j
        if((iSel= SendDlgItemMessage(hWnd, IDC_LIST_DATA, LB_GETCURSEL, 0, 0)) == LB_ERR) {
//	        MessageBox( hWnd, "���M�����`�t�@�C����I�����ĉ�����", "��`�t�@�C�����M" , MB_ICONEXCLAMATION | MB_OK );
            UserMsgBox( hWnd, "\n���M�����`�t�@�C����I�����ĉ�����", "��`�t�@�C�����M");
      	    return FALSE;
        }
       	//////////////////////////////////////////////////////////////////////
   	    //	��`�t�@�C�����̓Ǎ���
   	    //////////////////////////////////////////////////////////////////////
   	    if(SendDlgItemMessage(hWnd, IDC_LIST_DATA, LB_GETTEXT, iSel, (LPARAM)fileName)) {
            // Nothing to do
   	    } else {
//   		    MessageBox( hWnd, "���M��`�t�@�C���Ǎ��ُ݈�", "��`�t�@�C�����M" , MB_ICONEXCLAMATION | MB_OK );
            UserMsgBox( hWnd, "\n���M��`�t�@�C���Ǎ��ُ݈�", "��`�t�@�C�����M" );
   		    return FALSE;
   	    }
        if(SendDlgItemMessage(hWnd, IDC_LIST_DATA, LB_GETTEXT, iSel, (LPARAM)fname)) {
            // Nothing to do
        } else {
//    		MessageBox( hWnd, "���M�e�L�X�g�Ǎ��ُ݈�", "�t�@�C���f�[�^���M" , MB_ICONEXCLAMATION | MB_OK );
            UserMsgBox( hWnd, "\n���M�e�L�X�g�Ǎ��ُ݈�", "�t�@�C���f�[�^���M" );
   			return FALSE;
        }

        if(0 == memcmp(&fname[strlen(fname)-4], ".bin", 4)) {
			tcpCtrl.fileForm = 3;
        } else if(0 == memcmp(&fname[strlen(fname)-4], ".def", 4)) {
			tcpCtrl.fileForm = 2;
		} else {
	        tcpCtrl.fileForm = 1;
        }
        /////////////////////////////////////////////////////////////////////////////

        //////////////////////////////////////////////////////////////
        // �t�@�C���f�[�^���M
        //////////////////////////////////////////////////////////////
        // �t�@�C���敪���e�L�X�g�Ƀ`�F�b�N����Ă��邩����
		if(1 == tcpCtrl.fileForm) {
            if((iSel= SendDlgItemMessage(hWnd, IDC_LIST_DATA, LB_GETCURSEL, 0, 0)) == LB_ERR) {
//	    	   	MessageBox( hWnd, "���M���鍀�ڂ�I�����ĉ�����", "�t�@�C���f�[�^���M" , MB_ICONEXCLAMATION | MB_OK );
                UserMsgBox( hWnd, "\n���M���鍀�ڂ�I�����ĉ�����", "�t�@�C���f�[�^���M\n" );
		    	return FALSE;
		    }

   			//////////////////////////////////////////////////////////////////////
   			//	�e�L�X�g�t�@�C�����M�`���Ǎ���
   			//////////////////////////////////////////////////////////////////////
   			if(SendDlgItemMessage(hWnd, IDC_LIST_DATA, LB_GETTEXT, iSel, (LPARAM)g_pSendDmy)) {
   				g_nTotalToSend=strlen(g_pSendDmy);
   			} else {
//   				MessageBox( hWnd, "���M�e�L�X�g�Ǎ��ُ݈�", "�t�@�C���f�[�^���M" , MB_ICONEXCLAMATION | MB_OK );
                UserMsgBox( hWnd, "\n���M�e�L�X�g�Ǎ��ُ݈�", "�t�@�C���f�[�^���M" );
   				return FALSE;
   			}
		} else if(2 == tcpCtrl.fileForm) {
            // ��`�t�@�C���Ƀ`�F�b�N����Ă���B
            if(TRUE != FileTypeDefDataRead(hWnd)) {
                return FALSE;
            }
		} else if(3 == tcpCtrl.fileForm) {
            if(TRUE != FileTypeBinDataRead(hWnd)) {
                return FALSE;
            }
		}
	} else {
        // �e�L�X�g�f�[�^���M
		if(GetDlgItemText(hWnd, IDC_EDIT_MSG, g_pSendDmy, sizeof(g_pSendDmy))) {
			g_nTotalToSend=strlen(g_pSendDmy);
		} else {
//		   	MessageBox( hWnd, "�e�L�X�g�Ǎ��ُ݈�", "�e�L�X�g���M" , MB_ICONEXCLAMATION | MB_OK );
            UserMsgBox( hWnd, "\n�e�L�X�g�Ǎ��ُ݈�", "�e�L�X�g���M");
			return FALSE;
		}
	}

	return TRUE;
}

//----------------------------------------------------------------------------
//
//	�I�����̂h�o�A�o�n�q�s����ǂݍ��ޏ���
//
//----------------------------------------------------------------------------
BOOL	GetAddressData(HWND hWnd)
{
	int		sLen;
    char    work[256];

    memset(work, 0x00, sizeof(work));
    memset(tcpCtrl.ip,   0x00, sizeof(tcpCtrl.ip));
    memset(tcpCtrl.port, 0x00, sizeof(tcpCtrl.port));
    // �o�n�q�s�P�̐ݒ�
	if(GetDlgItemText(hWnd, IDC_EDIT_IP, work, 32)) {
	    sLen = strlen(work);
        if(31 < sLen) {
            work[31] = 0x00;
        }
        strcpy(tcpCtrl.ip[0], work);
    }
    memset(work, 0x00, sizeof(work));
	if(GetDlgItemText(hWnd, IDC_EDIT_PORT, work, 12)) {
	    sLen = strlen(work);
        if(11 < sLen) {
            work[11] = 0x00;
        }
        strcpy(tcpCtrl.port[0], work);
    }

    // �o�n�q�s�Q�̐ݒ�
    memset(work, 0x00, sizeof(work));
	if(GetDlgItemText(hWnd, IDC_EDIT_IP3, work, 32)) {
	    sLen = strlen(work);
        if(31 < sLen) {
            work[31] = 0x00;
        }
        strcpy(tcpCtrl.ip[1], work);
    }
    memset(work, 0x00, sizeof(work));
	if(GetDlgItemText(hWnd, IDC_EDIT_PORT3, work, 12)) {
	    sLen = strlen(work);
        if(11 < sLen) {
            work[11] = 0x00;
        }
        strcpy(tcpCtrl.port[1], work);
    }

    return TRUE;
}

//----------------------------------------------------------------------------
//
//	��`�t�@�C���Ǎ��ݏ���
//
//----------------------------------------------------------------------------
BOOL	FileTypeDefDataRead(HWND hWnd)
{
    int      iSel;
    int      rc;
    unsigned char  fileName[256];;


    memset(fileName, 0x00, sizeof(fileName));

    if((iSel= SendDlgItemMessage(hWnd, IDC_LIST_DATA, LB_GETCURSEL, 0, 0)) == LB_ERR) {
//	   	MessageBox( hWnd, "���M�����`�t�@�C����I�����ĉ�����", "��`�t�@�C�����M" , MB_ICONEXCLAMATION | MB_OK );
        UserMsgBox( hWnd, "\n���M�����`�t�@�C����I�����ĉ�����", "��`�t�@�C�����M" );
	   	return FALSE;
	}

   	//////////////////////////////////////////////////////////////////////
   	//	��`�t�@�C�����̓Ǎ���
   	//////////////////////////////////////////////////////////////////////
   	if(SendDlgItemMessage(hWnd, IDC_LIST_DATA, LB_GETTEXT, iSel, (LPARAM)fileName)) {
        // Nothing to do
   	} else {
//   		MessageBox( hWnd, "���M��`�t�@�C���Ǎ��ُ݈�", "��`�t�@�C�����M" , MB_ICONEXCLAMATION | MB_OK );
        UserMsgBox( hWnd, "\n���M��`�t�@�C���Ǎ��ُ݈�", "��`�t�@�C�����M" );
   		return FALSE;
   	}

    rc=MakeDefData(hWnd, fileName, NULL);

    return rc;
}

//----------------------------------------------------------------------------
//
//	�o�C�i���B�t�@�C���Ǎ��ݏ���
//
//----------------------------------------------------------------------------
BOOL	FileTypeBinDataRead(HWND hWnd)
{
    int      iSel;
    int      rc;
	int		 fp;
    unsigned char  fileName[256];
	HANDLE	 hFile;
	DWORD	 FileSizeLow, FileSizeHigh;

    memset(fileName, 0x00, sizeof(fileName));

    if((iSel= SendDlgItemMessage(hWnd, IDC_LIST_DATA, LB_GETCURSEL, 0, 0)) == LB_ERR) {
//	   	MessageBox( hWnd, "���M�����`�t�@�C����I�����ĉ�����", "�o�C�i���B�t�@�C�����M" , MB_ICONEXCLAMATION | MB_OK );
        UserMsgBox( hWnd, "\n���M�����`�t�@�C����I�����ĉ�����", "�o�C�i���B�t�@�C�����M" );
	   	return FALSE;
	}

   	//////////////////////////////////////////////////////////////////////
   	//	��`�t�@�C�����̓Ǎ���
   	//////////////////////////////////////////////////////////////////////
   	if(SendDlgItemMessage(hWnd, IDC_LIST_DATA, LB_GETTEXT, iSel, (LPARAM)fileName)) {
        // Nothing to do
   	} else {
//   		MessageBox( hWnd, "���M�t�@�C���Ǎ��ُ݈�", "�o�C�i���B�t�@�C�����M" , MB_ICONEXCLAMATION | MB_OK );
        UserMsgBox( hWnd, "\n���M�t�@�C���Ǎ��ُ݈�", "�o�C�i���B�t�@�C�����M" ); 
        return FALSE;
   	}

	hFile = CreateFile(
		fileName,
		0,
		FILE_SHARE_READ | FILE_SHARE_WRITE,
		NULL,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		NULL);

	FileSizeLow = GetFileSize(hFile, &FileSizeHigh);
	CloseHandle(hFile);
    if(sizeof(g_pSendDmy) < FileSizeLow) {
    	wsprintf(MsgBuf, "\n���M�f�[�^���� %d Byte �܂łł�", sizeof(g_pSendDmy));
//	   	MessageBox(hWnd, MsgBuf, "�o�C�i���B�t�@�C���Ǎ���", MB_ICONEXCLAMATION | MB_OK);
        UserMsgBox( hWnd, MsgBuf, "�o�C�i���B�t�@�C���Ǎ���" ); 
	    return FALSE;
    }

	fp = AP_Sfopen(fileName, AP_CREATRDWR);
	if(fp > 0) {
		g_nTotalToSend = FileSizeLow;
	    rc = AP_Sgrec(fp, g_nTotalToSend, 0, g_pSendDmy);
		AP_Sfclose( fp );
	}
     return TRUE;
}

//----------------------------------------------------------------------------
//
//	�o�C�i���B�t�@�C���Ǎ��ݏ���
//
//----------------------------------------------------------------------------
BOOL	SchBinDataRead(HWND hWnd, unsigned char* filename)
{
    int      rc;
	int		 fp;
	HANDLE	 hFile;
	DWORD	 FileSizeLow, FileSizeHigh;
//	LPVOID   lpMsgBuf;
   	//////////////////////////////////////////////////////////////////////
   	//	��`�t�@�C�����̓Ǎ���
   	//////////////////////////////////////////////////////////////////////
	hFile = CreateFile(
		filename,
		0,
		FILE_SHARE_READ | FILE_SHARE_WRITE,
		NULL,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		NULL);

	FileSizeLow = GetFileSize(hFile, &FileSizeHigh);

//FormatMessage( 
//    FORMAT_MESSAGE_ALLOCATE_BUFFER | 
//    FORMAT_MESSAGE_FROM_SYSTEM | 
//    FORMAT_MESSAGE_IGNORE_INSERTS,
//    NULL,
//    GetLastError(),
//    MAKELANGID(LANG_JAPANESE, SUBLANG_DEFAULT), // �f�t�H���g����
//    (LPTSTR) &lpMsgBuf,
//    0,
//    NULL 
//);
//MessageBox( NULL, lpMsgBuf, "Error", MB_OK | MB_ICONINFORMATION );
	CloseHandle(hFile);
    if(sizeof(g_pSendDmy) < FileSizeLow) {
    	wsprintf(MsgBuf, "\n���M�f�[�^���� %d Byte �܂łł�", sizeof(g_pSendDmy));
//	   	MessageBox(hWnd, MsgBuf, "�o�C�i���B�t�@�C���Ǎ���", MB_ICONEXCLAMATION | MB_OK);
        UserMsgBox( hWnd, MsgBuf, "�o�C�i���B�t�@�C���Ǎ���" ); 
	    return FALSE;
    }

	fp = AP_Sfopen(filename, AP_CREATRDWR);
	if(fp > 0) {
		g_nTotalToSend = FileSizeLow;
	    rc = AP_Sgrec(fp, g_nTotalToSend, 0, g_pSendDmy);
		AP_Sfclose( fp );
	}
     return TRUE;
}


//----------------------------------------------------------------------------
//
//	��`�t�@�C���̃f�[�^��ǂݍ��݁A���M�f�[�^���쐬����B
//
//----------------------------------------------------------------------------
BOOL  MakeDefData(HWND hWnd, unsigned char *fileName, unsigned char *recvbuff)
{
    FILE    *fp;
    unsigned char buff[256];
    unsigned char dmy[256];
    unsigned char ptn;
    int     length;
    int     iwork;
	char    cdmy[32];

	g_nTotalToSend = 0;
    ptn = 0x00;
    memset(g_pSendDmy, 0x00, sizeof(g_pSendDmy));
	memset(g_sInitBuf, 0x00, 16);
	memset(g_lInitBuf, 0x00, 16);
	memset(g_xsInitBuf, 0x00, 16);
	memset(g_xlInitBuf, 0x00, 16);
	g_pcterm_pa = 0x01;				// ��̫�Ă�0x01

	if((fp=fopen(fileName, "rt")) == NULL) {
		wsprintf(MsgBuf, "%s �t�@�C���I�[�v���G���[", fileName);
		MessageBox(hWnd, MsgBuf, "�e�L�X�g�t�@�C���Ǎ���", MB_ICONEXCLAMATION | MB_OK);
		return FALSE;
	}

    g_setAddr = 0;
    memset(buff, 0x00, sizeof(buff));
	while(fgets(buff, sizeof(buff), fp)) {
		if(buff[0] == ';')	continue;
        if(buff[0] == '#')	continue;
        if(buff[0] < 0x20 && buff[0] != '\t')	continue;

//		MyMessage(hWnd, "main buff=[%s]", buff );              

        if(0 == memcmp(buff, "DataLength", 10)) {
            sscanf(buff, "%s %d", dmy, &length);
            if(sizeof(g_pSendDmy) < length) {
        		wsprintf(MsgBuf, "\n���M�f�[�^���� %d Byte �܂łł�", sizeof(g_pSendDmy));
//	        	MessageBox(hWnd, MsgBuf, "��`�t�@�C���Ǎ���", MB_ICONEXCLAMATION | MB_OK);
                UserMsgBox( hWnd, MsgBuf, "��`�t�@�C���Ǎ���" ); 
            	fclose(fp);
		        return FALSE;
            }
			g_nTotalToSend = length;
            memset(buff, 0x00, sizeof(buff));
            continue;
        }
        if(0 == memcmp(buff, "InitPatern", 10)) {
            sscanf(buff, "%s %s", dmy, cdmy);
			if(0 == memcmp(cdmy, "0x", 2)){
				sscanf(&cdmy[2], "%2x", &iwork);
			} else {
				sscanf(&cdmy[0], "%2x", &iwork);
			}
            ptn = iwork;
            memset(g_pSendDmy, ptn, g_nTotalToSend);
            memset(buff, 0x00, sizeof(buff));
            continue;
        }
        /* Ver 4.03 --==>>>             */
        /* patern(��)�@pattern(��) �Ή� */
        if(0 == memcmp(buff, "InitPattern", 11)) {
            sscanf(buff, "%s %s", dmy, cdmy);
			if(0 == memcmp(cdmy, "0x", 2)){
				sscanf(&cdmy[2], "%2x", &iwork);
			} else {
				sscanf(&cdmy[0], "%2x", &iwork);
			}
            ptn = iwork;
            memset(g_pSendDmy, ptn, g_nTotalToSend);
            memset(buff, 0x00, sizeof(buff));
            continue;
        }
        /* Ver 4.03 <<<==--             */
		if(0 == memcmp(buff, "ShortInit", 9)) {
			sscanf(buff, "%s %s", dmy, cdmy);
			if(0 == memcmp(cdmy, "0x", 2)){
				sscanf(&cdmy[2], "%4x", &g_sInitBuf);
			} else {
				sscanf(&cdmy[0], "%4x", &g_sInitBuf);
			}
			memset(buff, 0x00, sizeof(buff));
			continue;
		}
		if(0 == memcmp(buff, "LongInit", 8)) {
			sscanf(buff, "%s %s", dmy, cdmy);
			if(0 == memcmp(cdmy, "0x", 2)){
				sscanf(&cdmy[2], "%8x", &g_lInitBuf);
			} else {
				sscanf(&cdmy[0], "%8x", &g_lInitBuf);
			}
			memset(buff, 0x00, sizeof(buff));
			continue;
		}
		if(0 == memcmp(buff, "XShortInit", 10)) {
			sscanf(buff, "%s %s", dmy, cdmy);
			if(0 == memcmp(cdmy, "0x", 2)){
				sscanf(&cdmy[2], "%4x", &g_xsInitBuf);
			} else {
				sscanf(&cdmy[0], "%4x", &g_xsInitBuf);
			}
			memset(buff, 0x00, sizeof(buff));
			continue;
		}
		if(0 == memcmp(buff, "XLongInit", 9)) {
			sscanf(buff, "%s %s", dmy, cdmy);
//			g_xlInitBuf[0] = cWork[3];
//			g_xlInitBuf[1] = cWork[2];
//			g_xlInitBuf[2] = cWork[1];
//			g_xlInitBuf[3] = cWork[0];
			if(0 == memcmp(cdmy, "0x", 2)){
				sscanf(&cdmy[2], "%8x", &g_xlInitBuf);
			} else {
				sscanf(&cdmy[0], "%8x", &g_xlInitBuf);
			}
			memset(buff, 0x00, sizeof(buff));
			continue;
		}

        if(0 == memcmp(buff, "PctermPA", 8)) {
            sscanf(buff, "%s %s", dmy, cdmy);
			if(0 == memcmp(cdmy, "0x", 2)){
				sscanf(&cdmy[2], "%2x", &iwork);
			} else {
				sscanf(&cdmy[0], "%2x", &iwork);
			}
			g_pcterm_pa = iwork;
            continue;
        }

        if(FALSE == PtnDataSet(hWnd, buff, fileName)) {
	        fclose(fp);
            return FALSE;
        }

        if('M' == buff[0] && NULL != recvbuff) {
            rdata_to_sdata(hWnd, buff, recvbuff);
        }

        memset(buff, 0x00, sizeof(buff));
	}
	fclose(fp);

    return TRUE;
}


//----------------------------------------------------------------------------
//
//	��`�t�@�C���̃f�[�^�^�ɂ��e�����֕��򂵃f�[�^�쐬����B
//
//----------------------------------------------------------------------------
BOOL  PtnDataSet(HWND hWnd, unsigned char* buff, unsigned char *fileName)
{
    int     addr;
    int     i;
    int     ret;
    char    type[50];


    for(i=0; i<32; i++) {
        if('0' <= buff[i]) {
            break;
        }
    }

    if('0' <= buff[i] &&  buff[i] <= '9') {
        sscanf(buff, "%d %s", &addr, type);
    } else {
        sscanf(buff, "%s", type);
    }

//  MyMessage(hWnd, "buff=[%s]", buff );              
//  MyMessage(hWnd, "type=[%s] addr=%d", type, g_setAddr );              

    if(0 == strcmp(type, "SHORT")) {
        ret=ShortDataSet(hWnd, &buff[i]);
    } else if(0 == strcmp(type, "LONG")) {
        ret=LongDataSet(hWnd, &buff[i]);
    } else if(0 == strcmp(type, "CHAR")) {
        ret=CharDataSet(hWnd, &buff[i]);
    } else if(0 == strcmp(type, "HEX1")) {
        ret=HexDataSet(hWnd, 1, &buff[i]);
    } else if(0 == strcmp(type, "HEX2")) {
        ret=HexDataSet(hWnd, 2, &buff[i]);
    } else if(0 == strcmp(type, "HEX4")) {
        ret=HexDataSet(hWnd, 4, &buff[i]);
    } else if(0 == strcmp(type, "XHEX2")) {
        ret=XHexDataSet(hWnd, 2, &buff[i]);
    } else if(0 == strcmp(type, "XHEX4")) {
        ret=XHexDataSet(hWnd, 4, &buff[i]);
    } else if(0 == strcmp(type, "BIT")) {
        ret=BinDataSet(hWnd, &buff[i]);
    } else if(0 == strcmp(type, "SPACE")) {
        ret=SpaceDataSet(hWnd, &buff[i]);
    } else if(0 == strcmp(type, "EBCSPACE")) {
        ret=EbcSpaceDataSet(hWnd, &buff[i]);
    } else if(0 == strcmp(type, "XSHORT")) {
        ret=XShortDataSet(hWnd, &buff[i]);
    } else if(0 == strcmp(type, "XLONG")) {
        ret=XLongDataSet(hWnd, &buff[i]);
    } else if(0 == strcmp(type, "EBCDIC")) {
        ret=EbcdicDataSet(hWnd, &buff[i]);
    } else if(0 == strcmp(type, "DATETIME2")) {
        ret=DateTimeSet(hWnd, &buff[i], 2);
    } else if(0 == strcmp(type, "DATETIME4")) {
        ret=DateTimeSet(hWnd, &buff[i], 4);
    } else if(0 == strcmp(type, "DATETIMEC")) {
        ret=CDateTimeSet(hWnd, &buff[i]);
    } else if(0 == strcmp(type, "DATETIMEE")) {
        ret=EDateTimeSet(hWnd, &buff[i]);
    } else if(0 == strcmp(type, "XDATETIME2")) {
        ret=XDateTimeSet(hWnd, &buff[i], 2);
    } else if(0 == strcmp(type, "XDATETIME4")) {
        ret=XDateTimeSet(hWnd, &buff[i], 4);
    } else if(0 == strcmp(type, "FLOAT")) {
        ret=FloatDataSet(hWnd, &buff[i]);
    } else if(0 == strcmp(type, "DOUBLE")) {
        ret=DoubleDataSet(hWnd, &buff[i]);
    } else if(0 == strcmp(type, "CNT_SHORT")) {
        ret=CountShortDataSet(hWnd, &buff[i], fileName);
    } else if(0 == strcmp(type, "CNT_LONG")) {
        ret=CountLongDataSet(hWnd, &buff[i], fileName);
    } else if(0 == strcmp(type, "CNT_XSHORT")) {
        ret=CountXShortDataSet(hWnd, &buff[i], fileName);
    } else if(0 == strcmp(type, "CNT_XLONG")) {
        ret=CountXLongDataSet(hWnd, &buff[i], fileName);
    } else if(0 == strcmp(type, "CNT_CHAR")) {
        ret=CountCharDataSet(hWnd, &buff[i], fileName);
    } else if(0 == strcmp(type, "CNT_EBC")) {
        ret=CountEbcDataSet(hWnd, &buff[i], fileName);
    } else if(0 == strcmp(type, "FILE_CHAR")) {
        ret=FileCharSet(hWnd, &buff[i]);
    }

//      	wkDump( hWnd, IDC_LIST_STATUS, g_pSendDmy, _DISP_OFF, NULL,
//                        tcpCtrl.traceCode, g_setAddr, "???");

    return ret;
}

//--------------------------------------------------------------------
// ��M�ް����瑗�M�ް����쐬����
//--------------------------------------------------------------------
int     rdata_to_sdata(HWND hWnd, unsigned char *buff, unsigned char* recvbuff)
{
    char    dmy[32];
    int     from;
    int     to;
    int     len;
    int     offset;

    if(NULL == buff || NULL == recvbuff) return 1;

    offset = getOffsetByComType();

    if(0 == memcmp(buff, "MOVEPA", 6)) {
        // ��M����PA��Ă���
        g_pcterm_pa = recvbuff[9];        // PA
    } else if(0 == memcmp(buff, "MOVE", 4)) {
        sscanf(buff, "%s %d %d %d", dmy, &from, &to, &len);
        memcpy(&g_pSendDmy[to], &recvbuff[from+offset], len);
    }
    return 0;
}

BOOL  ScheduleDataSet(HWND hWnd)
{
    FILE    *fp;
    int     addr;
    int     cycAddr;
    unsigned char buff[256];
    int     i;
    int     flag;
    int     f_addr;
    int     itemAddr;
    int     dispOut;

    flag = 0;
    memset(SchDef, 0x00, sizeof(ScheduleDef)*SCH_TBL_MAX);
    memset(CycDef, 0x00, sizeof(CycleSchDef)*SCH_TBL_MAX);
    memset(CntDef, 0x00, sizeof(CountDef)*SCH_TBL_MAX);
    for(i=0; i<SCH_TBL_MAX; i++) {
        Sch_no[i] = 1;  
    }

    if((fp=fopen(tcpCtrl.schFileName, "rt")) == NULL) {
        MessageBox(hWnd, tcpCtrl.schFileName, "���ޭ�يǗ�̧�ٓǍ��ݎ��s", MB_ICONEXCLAMATION | MB_OK);
        tcpCtrl.schCtrl = 0;
        tcpCtrl.cycCtrl = 0;
        return FALSE;
    }

    addr = -1;
    f_addr = -1;
    cycAddr = 0;
    memset(buff, 0x00, sizeof(buff));
    while(fgets(buff, sizeof(buff), fp)) {
        if(buff[0] == ';' || buff[0] == '#' || buff[0] <= 0x20)	{
//          if(1 == dispOut) {
//              dispScheduleData(hWnd, addr);
//              dispOut = 0;
//          }
            continue;
        }
        // ��������ǂݍ���
        if(0 == memcmp(buff, "9", 1)) {
            flag = 0;
            if(1 == dispOut) {
                dispScheduleData(hWnd, addr);
                dispOut = 0;
            }

            if(cycAddr >= SCH_TBL_MAX) {
                continue;
            }
            if(TRUE != MakeCycleSchDataSet(hWnd, buff, cycAddr)) {
                continue;
            }
//              MyMessage(hWnd, "��������ޭ��(%02d)  ���M�߰�=%2d  ����=%3d(�b)",
//                               cycAddr, CycDef[cycAddr].port,
//                                        CycDef[cycAddr].sec);
//              MyMessage(hWnd, "                   ��`̧��=[%s]", CycDef[cycAddr].fname  );              
            MyMessage(hWnd, "�������`(%02d)  ���M�߰�=%2d  ����=%3d(�b)",
                               cycAddr, CycDef[cycAddr].port,
                                        CycDef[cycAddr].sec);
            MyMessage(hWnd, "                ��`̧��=[%s]", CycDef[cycAddr].fname  );              
            cycAddr++;
        } else if(0 == memcmp(buff, "5", 1)) {
            flag = 0;
            HexDataSetSch2(hWnd, buff);
            MyMessage(hWnd, "ڽ��ݽ��`=%02x %02x  ���ͯ�ޒ�`=%02x %02x",
                             G_respons[0],  G_respons[1], G_hed[0], G_hed[1]);
        } else if(0x30 <= buff[0] && buff[0] <= 0x39) {
            if(addr >= SCH_TBL_MAX) {
                continue;
            }
            if(1 == dispOut) {
                dispScheduleData(hWnd, addr);
                dispOut = 0;
            }
            dispOut = 1;
            flag = 1;
            addr++;
            itemAddr = 0;
            MakeScheduleDataSet(hWnd, buff, addr, itemAddr);
            SchDef[addr].num = 1;
        } else if('&' == buff[0] && 1 == flag) {
            itemAddr++;
            if(itemAddr >= SCH_ITEM_MAX) continue;
            MakeScheduleDataSet(hWnd, buff, addr, itemAddr);
            SchDef[addr].num++;
        } else if('F' == buff[0]) {
            if(f_addr >= SCH_TBL_MAX) {
                continue;
            }
            if(1 == dispOut) {
                dispScheduleData(hWnd, addr);
                dispOut = 0;
            }
            dispOut = 2;
            flag = 2;
            f_addr++;
            itemAddr = 0;
            MakeCountDataSet(hWnd, buff, f_addr, itemAddr);
            CntDef[f_addr].num = 1;
        } else if('&' == buff[0] && 2 == flag) {
            itemAddr++;
            if(itemAddr >= SCH_ITEM_MAX) continue;
            MakeCountDataSet(hWnd, buff, f_addr, itemAddr);
            CntDef[f_addr].num++;
        } /* COUNTER */

        memset(buff, 0x00, sizeof(buff));
    }
    fclose(fp);

    if(1 == dispOut) {
        dispScheduleData(hWnd, addr);
        dispOut = 0;
    }
    return TRUE;
}

//////////////////////////////////////////////////////////////////////
// �V�i���I�Ǘ��t�@�C���ǂݍ���
//////////////////////////////////////////////////////////////////////
BOOL  SinaDataSet(HWND hWnd)
{
    FILE    *fp;
    int     addr;
    int     sinaAddr;
    unsigned char buff[256];

    memset(SinaDef, 0x00, sizeof(SinaSchDef)*SINA_TBL_MAX);
	SinaPoint = 0;

	if((fp=fopen(tcpCtrl.sinaFileName, "rt")) == NULL) {
		MessageBox(hWnd, tcpCtrl.sinaFileName, "��ص�Ǘ�̧�ٓǍ��ݎ��s", MB_ICONEXCLAMATION | MB_OK);
        tcpCtrl.sinaCtrl = 0;
		return FALSE;
	}

    addr = -1;
    sinaAddr = 0;
    memset(buff, 0x00, sizeof(buff));
	while(fgets(buff, sizeof(buff), fp)) {
		if(buff[0] == ';' || buff[0] == '#' || buff[0] <= 0x20)	{
			continue;
		}
        // ���ǂݍ���
        if('1' == buff[0] || '2' == buff[0]) {
            if(sinaAddr >= SINA_TBL_MAX) {
				MyMessage(hWnd, "��ص�ϐ�OVER MAX=%d��", SINA_TBL_MAX);
                break;
            }
            if(TRUE != MakeSinaSchDataSet(hWnd, buff, sinaAddr)) {
				continue;
			}
       		MyMessage(hWnd, "��صt��`(%02d)   ���M�߰�=%2d  �Ԋu=%3d(�b)",
							   sinaAddr, SinaDef[sinaAddr].port,
										 SinaDef[sinaAddr].sec);
       		MyMessage(hWnd, "                ��`̧��=[%s]", SinaDef[sinaAddr].fname  );              
			sinaAddr++;
        }
		if(0 == memcmp(buff, "LOOP", 4)) {
			SinaDef[sinaAddr].port = 999;
			MyMessage(hWnd, "�V�i���I���[�v�ݒ�");
			break;
		}
		if(0 == memcmp(buff, "PAUSE", 5)) {
			SinaDef[sinaAddr].port = 9999;
			MyMessage(hWnd, "��صt��`(%02d)   �V�i���I�ꎞ��~�ݒ�", sinaAddr);
			sinaAddr++;
		}
        memset(buff, 0x00, sizeof(buff));
	}
	fclose(fp);

    return TRUE;
}




//////////////////////////////////////////////////////////////////////
// �V�i���I�Ǘ��t�@�C���ǂݍ���
//////////////////////////////////////////////////////////////////////
BOOL    MakeRecvSinaData(HWND hWnd, unsigned char *fileName, unsigned char *recvbuff, int rcv_port)
{
    FILE     *fp;
    int      addr;
    int      sinaAddr;
	int		 point;
	int      i;
    unsigned char buff[256];

    point = -1;
    for(i=0; i<RECV_SINA_MAX; i++) {
        if(0 == RecvSinaDef[i][0].use) {
            point = i;
            break;
        }
    }

//	MyMessage(hWnd, "TABLE ADDRESS = %d", point);
    if(-1 == point) {
//		MessageBox(hWnd, "ð��ٖ��t", "��M��ص�Ǘ�ð���", MB_ICONEXCLAMATION | MB_OK);
        UserMsgBox( hWnd, "\nð��ٖ��t", "��M��ص�Ǘ�ð���"); 
        return FALSE;
    }

    memcpy(RecvSinaData[point], recvbuff, COM_BUF_MAX);

    memset(RecvSinaDef[point],  0x00, sizeof(RecvSinaSchDef)*RECV_SINA_TBL_MAX);
    memset(&NextRecvSinaDef[point], 0x00, sizeof(RecvSinaSchDef));
	RecvSinaPoint[point] = 0;
	if((fp=fopen(fileName, "rt")) == NULL) {
		MessageBox(hWnd, fileName, "��M��ص�Ǘ�̧�ٓǍ��ݎ��s", MB_ICONEXCLAMATION | MB_OK);
		return FALSE;
	}

	MyMessage(hWnd, " ");
    addr = -1;
    sinaAddr = 0;
    memset(buff, 0x00, sizeof(buff));
	while(fgets(buff, sizeof(buff), fp)) {
		if(buff[0] == ';' || buff[0] == '#' || buff[0] <= 0x20)	{
			continue;
		}
        // ���ǂݍ���
        if('0' == buff[0] ||'1' == buff[0] || '2' == buff[0]) {
            if(sinaAddr >= RECV_SINA_TBL_MAX) {
				MyMessage(hWnd, "��M��ص�ϐ�OVER MAX=%d��", RECV_SINA_TBL_MAX);
                break;
            }
            if(TRUE != MakeRecvSinaSchDataSet(hWnd, buff, sinaAddr, point, rcv_port)) {
				continue;
			}
       		MyMessage(hWnd, "��M��ص��`(%02d)   ���M�߰�=%2d  �Ԋu=%3d(�b)",
                               sinaAddr, RecvSinaDef[point][sinaAddr].port,
                                         RecvSinaDef[point][sinaAddr].sec);
       		MyMessage(hWnd, "                ��`̧��=[%s]", RecvSinaDef[point][sinaAddr].fname  );
			if(0 == sinaAddr) {
	            RecvSinaDef[point][0].use = 1;

				NextRecvSinaDef[point].use = 1;
				NextRecvSinaDef[point].port  = RecvSinaDef[point][sinaAddr].port;
				NextRecvSinaDef[point].sec   = RecvSinaDef[point][sinaAddr].sec;
				memcpy(NextRecvSinaDef[point].fname, RecvSinaDef[point][sinaAddr].fname, sizeof(NextRecvSinaDef[i].fname));
				NextRecvSinaDef[point].count = 0;
			}
            sinaAddr++;
        }
		if(0 == memcmp(buff, "LOOP", 4)) {
			RecvSinaDef[point][sinaAddr].port = 999;
			MyMessage(hWnd, "��M�V�i���I���[�v�ݒ�");
			break;
		}
        memset(buff, 0x00, sizeof(buff));
	}
	fclose(fp);

    timerSetProc2(hWnd);

	if(0 == NextRecvSinaDef[point].sec) {
		timeUpProc2(hWnd, WM_TIMER, 0, 0);
	}

    return TRUE;
}


//----------------------------------------------------------------------------
//
//----------------------------------------------------------------------------
BOOL  MakeScheduleDataSet(HWND hWnd, unsigned char* buff, int setAddr, int itemAddr)
{
    int     port;
    int     addr;
    char    type[10];
	char    dmy[32];

//	MyMessage(hWnd, "[%s]", buff);

	if(0 == itemAddr) {
	    sscanf(buff, "%d %d %s", &port, &addr, type);
	} else {
	    sscanf(buff, "%s %d %s", dmy, &addr, type);
	}
//	MyMessage(hWnd, "[%d %d %s]", port, addr, type);

    if(0 == strcmp(type, "SHORT")) {
        ShortDataSetSch(hWnd, buff, setAddr, itemAddr);
    } else if(0 == strcmp(type, "LONG")) {
        LongDataSetSch(hWnd, buff, setAddr, itemAddr);
    } else if(0 == strcmp(type, "CHAR")) {
        CharDataSetSch(hWnd, buff, setAddr, itemAddr);
    } else if(0 == strcmp(type, "HEX1")) {
        HexDataSetSch(hWnd, 1, buff, setAddr, itemAddr);
    } else if(0 == strcmp(type, "HEX2")) {
        HexDataSetSch(hWnd, 2, buff, setAddr, itemAddr);
    } else if(0 == strcmp(type, "HEX4")) {
        HexDataSetSch(hWnd, 4, buff, setAddr, itemAddr);
    } else if(0 == strcmp(type, "EBCDIC")) {
        EbcdicDataSetSch(hWnd, buff, setAddr, itemAddr);
    } else if(0 == strcmp(type, "XSHORT")) {
        XShortDataSetSch(hWnd, buff, setAddr, itemAddr);
    } else if(0 == strcmp(type, "XLONG")) {
        XLongDataSetSch(hWnd, buff, setAddr, itemAddr);
    } else if(0 == strcmp(type, "XHEX2")) {
        XHexDataSetSch(hWnd, 2, buff, setAddr, itemAddr);
    } else if(0 == strcmp(type, "XHEX4")) {
        XHexDataSetSch(hWnd, 4, buff, setAddr, itemAddr);
    } 
    return TRUE;
}


void	dispScheduleData(HWND hWnd, int addr)
{

	static char ctype[][7] = {"SHORT",
		                      "LONG",
							  "CHAR",
							  "HEX1",
							  "HEX2",
							  "HEX4",
							  "EBCDIC",
							  "----",
							  "----",
							  "----",
							  "XSHORT",
							  "XLONG",
							  "----",
							  "XHEX1",
							  "XHEX2",
							  "XHEX4"
	};
	long  ldata;
	short sdata;
	int   itemAddr;
	char  work[32];
    char  cData[1024];

	if(0 == SchDef[addr].item[0].type) {
		MyMessage(hWnd, "��M���ޭ��(%02d) ��`�ُ�I�I", addr);
		return;
	}

	MyMessage(hWnd, "��M��`(%02d)  ���M�߰�=%2d", addr, SchDef[addr].port);

	for(itemAddr=0; itemAddr<SchDef[addr].num; itemAddr++) {
		if(SHORT_TYPE == SchDef[addr].item[itemAddr].type) {
			memcpy(&sdata, SchDef[addr].item[itemAddr].ptn, sizeof(short));
			MyMessage(hWnd, "              ���ڽ=%4d  ����=%s  ���������=[%d](0x%02X%02X)  �ް���=%d(byte)",
									SchDef[addr].item[itemAddr].addr, ctype[SchDef[addr].item[itemAddr].type-1],
									sdata,
									SchDef[addr].item[itemAddr].ptn[0], SchDef[addr].item[itemAddr].ptn[1],
									SchDef[addr].item[itemAddr].length);
		}
		if(XSHORT_TYPE == SchDef[addr].item[itemAddr].type) {
			work[0] = SchDef[addr].item[itemAddr].ptn[1];
			work[1] = SchDef[addr].item[itemAddr].ptn[0];
			memcpy(&sdata, work, sizeof(short));
			MyMessage(hWnd, "              ���ڽ=%4d  ����=%s ���������=[%d](0x%02X%02X)  �ް���=%d(byte)",
									SchDef[addr].item[itemAddr].addr, ctype[SchDef[addr].item[itemAddr].type-1],
									sdata,
									SchDef[addr].item[itemAddr].ptn[0], SchDef[addr].item[itemAddr].ptn[1],
									SchDef[addr].item[itemAddr].length);
		}

		if(LONG_TYPE == SchDef[addr].item[itemAddr].type) {
			memcpy(&ldata, SchDef[addr].item[itemAddr].ptn, sizeof(long));

			MyMessage(hWnd, "              ���ڽ=%4d  ����=%s  ���������=[%d](0x%02X%02X%02X%02X)  �ް���=%d(byte)",
									SchDef[addr].item[itemAddr].addr, ctype[SchDef[addr].item[itemAddr].type-1],
									ldata,
									SchDef[addr].item[itemAddr].ptn[0], SchDef[addr].item[itemAddr].ptn[1],
									SchDef[addr].item[itemAddr].ptn[2], SchDef[addr].item[itemAddr].ptn[3],
									SchDef[addr].item[itemAddr].length);
		}

		if(XLONG_TYPE == SchDef[addr].item[itemAddr].type) {
			work[0] = SchDef[addr].item[itemAddr].ptn[3];
			work[1] = SchDef[addr].item[itemAddr].ptn[2];
			work[2] = SchDef[addr].item[itemAddr].ptn[1];
			work[3] = SchDef[addr].item[itemAddr].ptn[0];
			memcpy(&ldata, work, sizeof(long));
			MyMessage(hWnd, "              ���ڽ=%4d  ����=%s  ���������=[%d](0x%02X%02X%02X%02X)  �ް���=%d(byte)",
									SchDef[addr].item[itemAddr].addr, ctype[SchDef[addr].item[itemAddr].type-1],
									ldata,
									SchDef[addr].item[itemAddr].ptn[0], SchDef[addr].item[itemAddr].ptn[1],
									SchDef[addr].item[itemAddr].ptn[2], SchDef[addr].item[itemAddr].ptn[3],
									SchDef[addr].item[itemAddr].length);
		}

		if(CHAR_TYPE == SchDef[addr].item[itemAddr].type) {
			MyMessage(hWnd, "              ���ڽ=%4d  ����=%s   ���������=[%s]  �ް���=%d(byte)",
									SchDef[addr].item[itemAddr].addr, ctype[SchDef[addr].item[itemAddr].type-1],
									SchDef[addr].item[itemAddr].ptn, SchDef[addr].item[itemAddr].length);
		}

		if(EBCDIC_TYPE == SchDef[addr].item[itemAddr].type) {
			memset(cData, 0x00, sizeof(cData));
            strnetoa(SchDef[addr].item[itemAddr].ptn, cData, SchDef[addr].item[itemAddr].length);
			MyMessage(hWnd, "              ���ڽ=%4d  ����=%s   ���������=[%s]  �ް���=%d(byte)",
									SchDef[addr].item[itemAddr].addr, ctype[SchDef[addr].item[itemAddr].type-1],
									cData, SchDef[addr].item[itemAddr].length);
		}

		if(HEX1_TYPE == SchDef[addr].item[itemAddr].type) {
			MyMessage(hWnd, "              ���ڽ=%4d  ����=%s   ���������=[0x%02X]  �ް���=%d(byte)",
									SchDef[addr].item[itemAddr].addr, ctype[SchDef[addr].item[itemAddr].type-1],
									SchDef[addr].item[itemAddr].ptn[0], sizeof(char));
			MyMessage(hWnd, "                          �J�Ԃ���=%d ��������=%d(byte)",
									SchDef[addr].item[itemAddr].length/sizeof(char), SchDef[addr].item[itemAddr].length);

		}
		if(HEX2_TYPE == SchDef[addr].item[itemAddr].type) {
			MyMessage(hWnd, "              ���ڽ=%4d  ����=%s   ���������=[0x%02X%02X]  �ް���=%d(byte)",
									SchDef[addr].item[itemAddr].addr, ctype[SchDef[addr].item[itemAddr].type-1],
									SchDef[addr].item[itemAddr].ptn[0], SchDef[addr].item[itemAddr].ptn[1],
									sizeof(short));
			MyMessage(hWnd, "                          �J�Ԃ���=%d  ��������=%d(byte)",
									SchDef[addr].item[itemAddr].length/sizeof(short), SchDef[addr].item[itemAddr].length);

		}
		if(XHEX2_TYPE == SchDef[addr].item[itemAddr].type) {
			MyMessage(hWnd, "              ���ڽ=%4d  ����=%s  ���������=[0x%02X%02X]  �ް���=%d(byte)",
									SchDef[addr].item[itemAddr].addr, ctype[SchDef[addr].item[itemAddr].type-1],
									SchDef[addr].item[itemAddr].ptn[0], SchDef[addr].item[itemAddr].ptn[1],
									sizeof(short));
			MyMessage(hWnd, "                          �J�Ԃ���=%d  ��������=%d(byte)",
									SchDef[addr].item[itemAddr].length/sizeof(short), SchDef[addr].item[itemAddr].length);

		}
		if(HEX4_TYPE == SchDef[addr].item[itemAddr].type) {
			MyMessage(hWnd, "              ���ڽ=%4d  ����=%s   ���������=[0x%02X%02X%02X%02X]  �ް���=%d(byte)",
									SchDef[addr].item[itemAddr].addr, ctype[SchDef[addr].item[itemAddr].type-1],
									SchDef[addr].item[itemAddr].ptn[0], SchDef[addr].item[itemAddr].ptn[1], 
									SchDef[addr].item[itemAddr].ptn[2], SchDef[addr].item[itemAddr].ptn[3], 
									sizeof(long));
			MyMessage(hWnd, "                          �J�Ԃ���=%d  ��������=%d(byte)",
									SchDef[addr].item[itemAddr].length/sizeof(long), SchDef[addr].item[itemAddr].length);
		}
		if(XHEX4_TYPE == SchDef[addr].item[itemAddr].type) {
			MyMessage(hWnd, "              ���ڽ=%4d  ����=%s  ���������=[0x%02X%02X%02X%02X]  �ް���=%d(byte)",
									SchDef[addr].item[itemAddr].addr, ctype[SchDef[addr].item[itemAddr].type-1],
									SchDef[addr].item[itemAddr].ptn[0], SchDef[addr].item[itemAddr].ptn[1], 
									SchDef[addr].item[itemAddr].ptn[2], SchDef[addr].item[itemAddr].ptn[3], 
									sizeof(long));
			MyMessage(hWnd, "                          �J�Ԃ���=%d  ��������=%d(byte)",
									SchDef[addr].item[itemAddr].length/sizeof(long), SchDef[addr].item[itemAddr].length);
		}

	}
    // MyMessage(hWnd, " ");
    MyMessage(hWnd, "              ��`̧��=[%s]", SchDef[addr].fname  );
	if(SchDef[addr].fnum > 0) {
		if(SchDef[addr].randam != 0) {
			MyMessage(hWnd, "              ��`̧�ِ�=%d��  ̧�ّI��=�����", SchDef[addr].fnum );
		}else {
			MyMessage(hWnd, "              ��`̧�ِ�=%d��  ̧�ّI��=���ݼ��", SchDef[addr].fnum );
		}
	}
    MyMessage(hWnd, " ");

	return;
}

void  MakeFullName(HWND hWnd, char* fname, char* fullName)
{
    int     n;

    *(fullName) = 0x00;
    strcat(fullName, tcpCtrl.homeDir);
    strcat(fullName, "\\");
    n = 0;
    if('\\' == *(fname)) {
        n = 1;
    }
    strcat(fullName, fname+n);
//    MyMessage(hWnd, "fname (%s) ����", fname);
//    MyMessage(hWnd, "MakeFullName (%s) ����", fullName);
}

/*******************************************************************************
*
*  �^�C�}�[�Z�b�g����
*
*  PARAMETER
*		HWND		hWnd				: ����޳�����
*	
*  return
*
*******************************************************************************/
void timerSetProc(HWND hWnd)
{
	/* �^�C�}�[�C�x���g�ݒ�			*/
	if( 0 == timerId ) {
        // �P�b�^�C�}�Z�b�g
	 	timerId=SetTimer(hWnd, TIMER_ID, 1000, (TIMERPROC)timeUpProc);
		if( 0 == timerId ) {
			return;
		}
		MyMessage(hWnd, "");
		MyMessage(hWnd, "�������@������^�C�}�[�Z�b�g���� (id=%d)�@������", timerId);
	}
	return;

}

/*******************************************************************************
*
*  �^�C�}�[�L�����Z������
*
*  PARAMETER
*		HWND		hWnd				: ����޳�����
*	
*  return
*
*******************************************************************************/
void timerResetProc(HWND hWnd)
{
	/* �^�C�}�L�����Z������	*/
	if(0 != timerId) {
        MyMessage(hWnd, "");
	    MyMessage(hWnd, "�������@������^�C�}�[�L�����Z�� (id=%d)�@������", timerId);

		KillTimer(hWnd, timerId);
		timerId = 0;
	}
}

/*******************************************************************************
*
*  �^�C�}�[�N������
*
*  PARAMETER
*		HWND		hWnd				: ����޳�����
*		UINT		uMessage			: ү����
*		UINT		idEvent				: ��ϰID
*		DWORD		dwTime				: ��ϰ�o�ߎ���
*	
*  return
*		0 : ����
*		1 : �ُ�
*
*******************************************************************************/
void CALLBACK timeUpProc(HWND hWnd, UINT uMessage, UINT idEvent, DWORD dwTime)
{
	int		addr;
    int     len;

	if( WM_TIMER != uMessage) {
		return;
	}

    if(0 == tcpCtrl.cycCtrl) {
        return;
    }
    for(addr=0; addr<SCH_TBL_MAX; addr++) {
        if(0 >= CycDef[addr].sec) {
            break;
        }
        CycDef[addr].count++;
        if(CycDef[addr].sec == CycDef[addr].count) {
//        	MyMessage(hWnd, "addr[%d]  �^�C�}�[�t�o (id=%d)", addr, timerId);
            CycDef[addr].count = 0;
            if(1 == isConnectPort(CycDef[addr].port)) {
                len = strlen(CycDef[addr].fname);
                if(0 == memcmp(&CycDef[addr].fname[len-3], "bin", 3)) {
                    SchBinDataRead(hWnd, CycDef[addr].fname);
	                SendMsg(hWnd, CycDef[addr].port);
                } else if(0 == memcmp(&CycDef[addr].fname[len-3], "def", 3)) {
                    if(MakeDefData(hWnd, CycDef[addr].fname, NULL)) {
                        SendMsg(hWnd, CycDef[addr].port);
                    }
                }
            }
        }
    }

	return;
}


/*******************************************************************************
*
*  �^�C�}�[�Z�b�g����
*
*  PARAMETER
*		HWND		hWnd				: ����޳�����
*	
*  return
*
*******************************************************************************/
void timerSetProc1(HWND hWnd)
{
	/* �^�C�}�[�C�x���g�ݒ�			*/
	if( 0 == SinaTimerId ) {
        // �P�b�^�C�}�Z�b�g
	 	SinaTimerId=SetTimer(hWnd, SINA_TIMER_ID, 1000, (TIMERPROC)timeUpProc1);
		if( 0 == SinaTimerId ) {
			return;
		}

        SinaPoint = 0;
        SinaDefCount = 0;

		MyMessage(hWnd, "");
		MyMessage(hWnd, "�������@�V�i���I�^�C�}�[�Z�b�g���� (id=%d)�@������", SinaTimerId);

		EnableWindow( GetDlgItem( hWnd, IDC_SINA_STOP ), TRUE );

	}
	return;

}

/*******************************************************************************
*
*  �^�C�}�[�L�����Z������
*
*  PARAMETER
*		HWND		hWnd				: ����޳�����
*	
*  return
*
*******************************************************************************/
void timerResetProc1(HWND hWnd)
{
	/* �^�C�}�L�����Z������	*/
	if(0 != SinaTimerId) {
        MyMessage(hWnd, "");
	    MyMessage(hWnd, "�������@�V�i���I�^�C�}�[�L�����Z�� (id=%d)�@������", SinaTimerId);

		KillTimer(hWnd, SinaTimerId);

		tcpCtrl.sina_stop = 0;
		SendDlgItemMessage(hWnd, IDC_SINA_STOP, BM_SETCHECK, tcpCtrl.sina_stop, 0);
		EnableWindow( GetDlgItem( hWnd, IDC_SINA_STOP ), FALSE );

		SinaTimerId = 0;
	}
}

/*******************************************************************************
*
*  �^�C�}�[�N������
*
*  PARAMETER
*		HWND		hWnd				: ����޳�����
*		UINT		uMessage			: ү����
*		UINT		idEvent				: ��ϰID
*		DWORD		dwTime				: ��ϰ�o�ߎ���
*	
*  return
*		0 : ����
*		1 : �ُ�
*
*******************************************************************************/
void CALLBACK timeUpProc1(HWND hWnd, UINT uMessage, UINT idEvent, DWORD dwTime)
{
    int     len;
    char    fwork[512];

    struct	_stat	buf;


	if( WM_TIMER != uMessage) {
		MyMessage(hWnd, "WM_TIMER");
		return;
	}

    g_SinaSozOpen = 0;

    if(0 == tcpCtrl.sinaCtrl) {
        return;
    }

    if(1 == tcpCtrl.sina_stop) {
        SinaDefCount = 0;
        return;
    }

	SinaDefCount++;
    for( ; ; ) {
    	if(999 == SinaDef[SinaPoint].port) {
       		SinaDefCount = 0;
    		SinaPoint = 0;
    		MyMessage(hWnd, "");
    		MyMessage(hWnd, "�������@�V�i���I���[�v�ݒ� (id=%d)�@������", SinaTimerId);
            return;
    	}

        if(9999 == SinaDef[SinaPoint].port) {
       		SinaDefCount = 0;
            SinaPoint++;
            if(0 == SinaDef[SinaPoint].port)  SinaPoint = 0;
            MyMessage(hWnd, "");
            MyMessage(hWnd, "�������@�V�i���I�ꎞ��~�w���L��@������");
    	    tcpCtrl.sina_stop = 1;
    	    SendDlgItemMessage(hWnd, IDC_SINA_STOP, BM_SETCHECK, tcpCtrl.sina_stop, 0);
            return;
        }

		if(0 == SinaDef[SinaPoint].port || SinaPoint >= SINA_TBL_MAX) {
			MyMessage(hWnd, "");
			MyMessage(hWnd, "�������@�V�i���I�I��  ������");
			timerResetProc1(hWnd);
			tcpCtrl.sinaCtrl = 0;
   			SendDlgItemMessage(hWnd, IDC_SINA_CHECK, BM_SETCHECK, tcpCtrl.sinaCtrl, 0);
			return;
		}

        if(SinaDef[SinaPoint].sec <= SinaDefCount) {
       	    if(_stat(SinaDef[SinaPoint].fname, &buf) != 0) {
			    timerResetProc1(hWnd);
			    tcpCtrl.sinaCtrl = 0;
   			    SendDlgItemMessage(hWnd, IDC_SINA_CHECK, BM_SETCHECK, tcpCtrl.sinaCtrl, 0);
			    MyMessage(hWnd, "");
		        MyMessage(hWnd, "[%s] ��`̧�ٖ���", fwork);
			    MyMessage(hWnd, "�������@�V�i���I���`�t�@�C���ُ� �V�i���I�I��  ������");
       		    wsprintf(MsgBuf, "%s", SinaDef[SinaPoint].fname);
        	    MessageBox(hWnd, MsgBuf, "��ص��`̧�وُ�", MB_ICONEXCLAMATION | MB_OK);
                return;
            }

            if(1 == SinaDef[SinaPoint].port || 2 == SinaDef[SinaPoint].port) {
                if(1 != isConnectPort(SinaDef[SinaPoint].port)) {
                    if(1 == isTcpConnect(SinaDef[SinaPoint].port) &&
                       1 == isSOZ()) {
                        // connect type
                        g_SinaSozOpen = 1;
                        if(ConnectProc(hWnd, SinaDef[SinaPoint].port)) {
                        	MyMessage( hWnd, "PORT %d  ���ް�֐ڑ��������܂���.  [%s]", SinaDef[SinaPoint].port, GetNowTime() );
                        } else {
                            g_SinaSozOpen = 0;
                        	// MyMessage( hWnd, "%d SOZ ���ް�֐ڑ�NG.  [%s]", SinaDef[SinaPoint].port, GetNowTime() );
                        }
                    }
                }
                if(0 == isConnectPort(SinaDef[SinaPoint].port)) {
                    SinaDefCount = 0;
                    return;
                }
            }

            len = strlen(SinaDef[SinaPoint].fname);
            if(0 == memcmp(&SinaDef[SinaPoint].fname[len-3], "bin", 3)) {
                SchBinDataRead(hWnd, SinaDef[SinaPoint].fname);
                SendMsg(hWnd, SinaDef[SinaPoint].port);
                if(1 == g_SinaSozOpen) {
                    CloseSocket(hWnd, SinaDef[SinaPoint].port); 
                    g_SinaSozOpen = 0;
                }
		        SinaPoint++;
            } else if(0 == memcmp(&SinaDef[SinaPoint].fname[len-3], "def", 3)) {
                if(MakeDefData(hWnd, SinaDef[SinaPoint].fname, NULL)) {
                    SendMsg(hWnd, SinaDef[SinaPoint].port);
                    if(1 == g_SinaSozOpen) {
                        CloseSocket(hWnd, SinaDef[SinaPoint].port); 
                        g_SinaSozOpen = 0;
                    }
			        SinaPoint++;
                }
            } else {
                if(1 == g_SinaSozOpen) {
                    CloseSocket(hWnd, SinaDef[SinaPoint].port); 
                    g_SinaSozOpen = 0;
                }
            }
			SinaDefCount = 0;
        }
		if(0 != SinaDef[SinaPoint].sec) {
			break;
		}
    }
}


/*******************************************************************************
*
*  �^�C�}�[�Z�b�g����
*
*  PARAMETER
*		HWND		hWnd				: ����޳�����
*	
*  return
*
*******************************************************************************/
void timerSetProc2(HWND hWnd)
{
	/* �^�C�}�[�C�x���g�ݒ�			*/
	if( 0 == RecvSinaTimerId ) {
        // �P�b�^�C�}�Z�b�g
	 	RecvSinaTimerId=SetTimer(hWnd, RECV_SINA_TIMER_ID, 1000, (TIMERPROC)timeUpProc2);
		if( 0 == RecvSinaTimerId ) {
			return;
		}
		MyMessage(hWnd, "");
		MyMessage(hWnd, "�������@��M�V�i���I�^�C�}�[�Z�b�g���� (id=%d)�@������", RecvSinaTimerId);

		EnableWindow( GetDlgItem( hWnd, IDC_RSINA_STOP ), TRUE );

	}
	return;

}

/*******************************************************************************
*
*  �^�C�}�[�L�����Z������
*
*  PARAMETER
*		HWND		hWnd				: ����޳�����
*	
*  return
*
*******************************************************************************/
void timerResetProc2(HWND hWnd)
{
	/* �^�C�}�L�����Z������	*/
	if(0 != RecvSinaTimerId) {
        MyMessage(hWnd, "");
	    MyMessage(hWnd, "�������@�V�i���I�^�C�}�[�L�����Z�� (id=%d)�@������", RecvSinaTimerId);

		KillTimer(hWnd, RecvSinaTimerId);
		RecvSinaTimerId = 0;

		memset(RecvSinaPoint, 0x00, sizeof(RecvSinaPoint));
		memset(&NextRecvSinaDef, 0x00, sizeof(NextRecvSinaDef));
		memset(&RecvSinaDef,     0x00, sizeof(RecvSinaDef));


		tcpCtrl.rsina_stop = 0;
		SendDlgItemMessage(hWnd, IDC_RSINA_STOP, BM_SETCHECK, tcpCtrl.rsina_stop, 0);
		EnableWindow( GetDlgItem( hWnd, IDC_RSINA_STOP ), FALSE );

	}
}

/*******************************************************************************
*
*  �^�C�}�[�N������
*
*  PARAMETER
*		HWND		hWnd				: ����޳�����
*		UINT		uMessage			: ү����
*		UINT		idEvent				: ��ϰID
*		DWORD		dwTime				: ��ϰ�o�ߎ���
*	
*  return
*		0 : ����
*		1 : �ُ�
*
*******************************************************************************/
void CALLBACK timeUpProc2(HWND hWnd, UINT uMessage, UINT idEvent, DWORD dwTime)
{
	int		i;
	int     len;
    int     ok;
    struct	_stat	buf;
    char    fwork[512];

	if( WM_TIMER != uMessage) {
		return;
	}
    g_SinaSozOpen = 0;
    if(1 == tcpCtrl.rsina_stop) {
        return;
    }

    for(i=0; i<RECV_SINA_MAX; i++) {
SEC_0_SEND:
        if(1 != NextRecvSinaDef[i].use) continue;
/*
        if(1 != isConnectPort(NextRecvSinaDef[i].port)) {
            if(1 == isTcpConnect(NextRecvSinaDef[i].port) &&
               1 == isSOZ()) {
//    		    MyMessage(hWnd, "SOZ SOCKET OPEN");
                ConnectProc(hWnd, NextRecvSinaDef[i].port);
            } else {
                continue;
            }
        }
*/
        NextRecvSinaDef[i].count++;
        if(NextRecvSinaDef[i].sec <= NextRecvSinaDef[i].count) {

        	if(_stat(NextRecvSinaDef[i].fname, &buf) != 0) {
                strcpy(fwork, NextRecvSinaDef[i].fname);
                // ��`̧�وُ�̂��ߏI��
				RecvSinaPoint[i] = 0;
                memset(&NextRecvSinaDef[i].use , 0x00, sizeof(RecvSinaSchDef));
                memset(&RecvSinaDef[i][0].use,   0x00, sizeof(RecvSinaSchDef)*RECV_SINA_TBL_MAX+1);

			    MyMessage(hWnd, "");
			    MyMessage(hWnd, "[%s] ��`̧�ٖ���", fwork);
			    MyMessage(hWnd, "�������@��M�V�i���I��`�t�@�C���ُ�I�I�@�V�i���I�I��  ������");

        		wsprintf(MsgBuf, "%s", fwork);
	        	MessageBox(hWnd, MsgBuf, "��M��ص��`̧�وُ�", MB_ICONEXCLAMATION | MB_OK);
               continue;
			}

            if(1 != isConnectPort(NextRecvSinaDef[i].port)) {
                if(1 == isTcpConnect(NextRecvSinaDef[i].port) &&
                   1 == isSOZ()) {
//    		        MyMessage(hWnd, "SOZ SOCKET OPEN");
                    g_SinaSozOpen = 1;
                    if(ConnectProc(hWnd, NextRecvSinaDef[i].port)) {
                   		MyMessage( hWnd, "PORT %d  ���ް�֐ڑ��������܂���.  [%s]", NextRecvSinaDef[i].port, GetNowTime() );
                    } else {
                        g_SinaSozOpen = 0;
                       	// MyMessage( hWnd, "PORT %d ���ް�ւ̐ڑ��G���[.  [%s]", NextRecvSinaDef[i].port, GetNowTime() );
                    }
                }
            }
            if(0 == isConnectPort(NextRecvSinaDef[i].port)) {
                g_SinaSozOpen = 0;
                continue;
            }

            ok = 0;
            len = strlen(NextRecvSinaDef[i].fname);
            if(0 == memcmp(&NextRecvSinaDef[i].fname[len-3], "bin", 3)) {
                SchBinDataRead(hWnd, NextRecvSinaDef[i].fname);
                SendMsg(hWnd, NextRecvSinaDef[i].port);
                ok = 1;
            } else if(0 == memcmp(&NextRecvSinaDef[i].fname[len-3], "def", 3)) {
                if(MakeDefData(hWnd, NextRecvSinaDef[i].fname, RecvSinaData[i])) {
                    SendMsg(hWnd, NextRecvSinaDef[i].port);
                    ok = 1;
                }
            }

            if(1 == g_SinaSozOpen) {
                CloseSocket(hWnd, NextRecvSinaDef[i].port); 
                g_SinaSozOpen = 0;
            }

            if(1 == ok) {
                RecvSinaPoint[i]++;
				
//				MyMessage(hWnd, "RecvSinaDef[%d][%d].port=%d", i, RecvSinaPoint[i], RecvSinaDef[i][RecvSinaPoint[i]].port);

        		if(999 == RecvSinaDef[i][RecvSinaPoint[i]].port) {
		        	RecvSinaPoint[i] = 0;
			        MyMessage(hWnd, "");
			        MyMessage(hWnd, "�������@��M�V�i���I���[�v�ݒ� �@������");
		        }

                if(0 == RecvSinaDef[i][RecvSinaPoint[i]].port || RecvSinaPoint[i] >= RECV_SINA_TBL_MAX) {
                    // �I�� -=> ������
					RecvSinaPoint[i] = 0;
                    memset(&NextRecvSinaDef[i].use , 0x00, sizeof(RecvSinaSchDef));
                    memset(&RecvSinaDef[i][0].use,   0x00, sizeof(RecvSinaSchDef)*RECV_SINA_TBL_MAX+1);
			        MyMessage(hWnd, "");
			        MyMessage(hWnd, "�������@��M�V�i���I�I��  ������");
                    continue;
                }

				NextRecvSinaDef[i].use = 1;
        		NextRecvSinaDef[i].port  = RecvSinaDef[i][RecvSinaPoint[i]].port;
		        NextRecvSinaDef[i].sec   = RecvSinaDef[i][RecvSinaPoint[i]].sec;
		        memcpy(NextRecvSinaDef[i].fname, RecvSinaDef[i][RecvSinaPoint[i]].fname, sizeof(NextRecvSinaDef[i].fname));
		        NextRecvSinaDef[i].count = 0;
            }

            if(0 == NextRecvSinaDef[i].sec) {
		        goto SEC_0_SEND;
	        }
        }
    }

    for(i=0 ; i<RECV_SINA_MAX; i++) {
		if(0 != NextRecvSinaDef[i].use) {
			return;
		}
	}
	// ���ޭ�ٖ���������ϰ��ݾ�
	timerResetProc2(hWnd);
	return;
}

/*******************************************************************************
*
*  �^�C�}�[�Z�b�g����
*
*  PARAMETER
*		HWND		hWnd				: ����޳�����
*	
*  return
*
*******************************************************************************/
void timerSetProc3(HWND hWnd, int timerid)
{
	/* �^�C�}�[�C�x���g�ݒ�			*/
    if(close_btn[timerid-RETRY_TIMER_ID1]) {
        return;
    }

	if( 0 == RetryTimerId[timerid-RETRY_TIMER_ID1] ) {
        // �R�b�^�C�}�Z�b�g
	 	RetryTimerId[timerid-RETRY_TIMER_ID1]=SetTimer(hWnd, timerid, 3000, (TIMERPROC)timeUpProc3);
		if( 0 == RetryTimerId[timerid-RETRY_TIMER_ID1]) {
			return;
		}
	}
	return;

}

/*******************************************************************************
*
*  �^�C�}�[�L�����Z������
*
*  PARAMETER
*		HWND		hWnd				: ����޳�����
*	
*  return
*
*******************************************************************************/
void timerResetProc3(HWND hWnd, int timerid)
{
	/* �^�C�}�L�����Z������	*/
	if(0 != RetryTimerId[timerid-RETRY_TIMER_ID1]) {

		KillTimer(hWnd, RetryTimerId[timerid-RETRY_TIMER_ID1]);
		RetryTimerId[timerid-RETRY_TIMER_ID1] = 0;

	}
}

/*******************************************************************************
*
*  �^�C�}�[�N������
*
*  PARAMETER
*		HWND		hWnd				: ����޳�����
*		UINT		uMessage			: ү����
*		UINT		idEvent				: ��ϰID
*		DWORD		dwTime				: ��ϰ�o�ߎ���
*	
*  return
*		0 : ����
*		1 : �ُ�
*
*******************************************************************************/
void CALLBACK timeUpProc3(HWND hWnd, UINT uMessage, UINT idEvent, DWORD dwTime)
{

	if( WM_TIMER != uMessage) {
		return;
	}
    timerResetProc3(hWnd, idEvent);

    if(0 == tcpCtrl.retry_flg) {
        return;
    }

    if(close_btn[idEvent-RETRY_TIMER_ID1]) {
        return;
    }

    if(RETRY_TIMER_ID1 == idEvent) {
        ConnectProc(hWnd, PORT1);
    } else {
        ConnectProc(hWnd, PORT2);
    }

	return;
}




BOOL    CtrlFileSave(HWND hWnd)
{
	OPENFILENAME	ofn;
	unsigned char   dirName[256];
	int             len;
	unsigned char   openName[256];
	int				cnt;
	int				iSel;
	unsigned char   buff[256];
	FILE*			fp;

	memset(openName, 0x00, sizeof(openName));
	memset(&ofn, 0, sizeof(OPENFILENAME));
	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = hWnd;
	// �t�@�C����ނ̎w��
	ofn.lpstrFilter = "Ctrļ�ف@  {*.dat}\0*.dat\0"\
					  "�S�Ă�̧�� {*.*}\0*.*\0\0";

	ofn.lpstrFile	= openName;
	ofn.nMaxFile = sizeof(openName);
	ofn.Flags = OFN_SHOWHELP | OFN_OVERWRITEPROMPT;
//	ofn.Flags = OFN_FILEMUSTEXIST;
//	ofn.Flags = OFN_FILEMUSTEXIST | OFN_ALLOWMULTISELECT | OFN_LONGNAMES | OFN_EXPLORER;
	// �g���q�����͂���Ȃ�������̫�Ă̊g���q
	ofn.lpstrDefExt = "dat";
	if(GetSaveFileName(&ofn)){
		memset(dirName, 0x00, sizeof(dirName));
		strcpy(dirName, openName);
		len = strlen(openName);

        MyMessage(hWnd, "CtrlFileSave() %s", ofn.lpstrFile);
        GetAddressData(hWnd);
		memset(&tcpCtrl.listData[0], 0x00, sizeof(tcpCtrl.listData));
	    cnt=SendDlgItemMessage(hWnd, IDC_LIST_DATA, LB_GETCOUNT, 0, 0);
		if(0 < cnt) {
			if(0x20 >= g_listName[0]) {
				memset(g_listName, 0x00, sizeof(g_listName));
				memcpy(g_listName, ofn.lpstrFile, strlen(ofn.lpstrFile)-4);
				strcat(g_listName, "(ؽď��).txt");
				memcpy(&tcpCtrl.listData[0], g_listName, strlen(g_listName));
			} else {
				memcpy(&tcpCtrl.listData[0], g_listName, strlen(g_listName));
			}

			if((fp=fopen(g_listName, "wt")) == NULL) {
				MessageBox(hWnd, g_listName, "ؽď��̧�ُ����ݎ��s", MB_ICONEXCLAMATION | MB_OK);
				return FALSE;
			}

			if(cnt > LIST_DATA_MAX) {
				cnt = LIST_DATA_MAX;
			}
			for(iSel=0; iSel<cnt; iSel++) {
       			if(SendDlgItemMessage(hWnd, IDC_LIST_DATA, LB_GETTEXT, iSel, (LPARAM)buff)) {
					// MyMessage(hWnd, "-- %s ", buff);
					fprintf(fp, "%s\n", buff);
				}
			}
			fclose(fp);
		}
        if(TRUE == putTcpCtrl(hWnd, ofn.lpstrFile)) {
			if(0 < cnt) {
	            MyMessage(hWnd, "ؽď����ނ��܂����B[%s]", tcpCtrl.listData);
			}
            MyMessage(hWnd, "���۰ُ����ނ��܂����B[%s]", ofn.lpstrFile);
			setDialogName(hWnd, ofn.lpstrFile);
        }
	}
    return TRUE;
}

/*--------------------------------------------------------------
* �t�@�C���ǂݍ��ݏ���
*--------------------------------------------------------------*/
BOOL    CtrlFileLoad(HWND hWnd)
{
	OPENFILENAME	ofn;
	unsigned char   dirName[256];
	int		        nfp;
    TCP_CTRL        tcpWk;

    memset(&tcpWk, 0x00, sizeof(TCP_CTRL));
    memcpy(&tcpWk, &tcpCtrl, sizeof(TCP_CTRL));

	memset(g_OpenName, 0x00, sizeof(g_OpenName));
	memset(&ofn, 0, sizeof(OPENFILENAME));
	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = hWnd;
	// �t�@�C����ނ̎w��
	ofn.lpstrFilter = "CTRĻ��   {*.dat}\0*.dat\0"\
					  "�S�Ă�̧�� {*.*}\0*.*\0\0";

	ofn.lpstrFile	= g_OpenName;
	ofn.nMaxFile = sizeof(g_OpenName);
//	ofn.Flags = OFN_FILEMUSTEXIST;
	ofn.Flags = OFN_FILEMUSTEXIST | OFN_LONGNAMES | OFN_EXPLORER;
	// �g���q�����͂���Ȃ�������̫�Ă̊g���q
	ofn.lpstrDefExt = "dat";
	if(GetOpenFileName(&ofn)){
		memset(dirName, 0x00, sizeof(dirName));
		strcpy(dirName, g_OpenName);
        nfp = AP_Sfopen(g_OpenName, AP_RDWR);
	    if ( nfp < 0 ) {
            return FALSE;
        }
        AP_Sgrec(nfp, sizeof(TCP_CTRL), 0, &tcpCtrl);
    	AP_Sfclose(nfp);
        memcpy(tcpCtrl.homeDir, tcpWk.homeDir, sizeof(tcpCtrl.homeDir));

        PostMessage( hWnd, WM_USER_ASYNC_SELECT+LOAD, 0, WSAMAKESELECTREPLY( 0, 0 ) );

        MyMessage(hWnd, "���۰ُ���۰�ނ��܂����B[%s]", g_OpenName);
        MyMessage(hWnd, "");

		setDialogName(hWnd, g_OpenName);

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
        return TRUE;
    } else {
        if(0 != CommDlgExtendedError()) {
            MyMessage(hWnd, "���۰�̧��۰�ވُ� : [%s]", g_OpenName);
            MyMessage(hWnd, "");
            return FALSE;
        }
    }

    return TRUE;
}


/*--------------------------------------------------------------
* �w���߰Ă̵���݊m�F
*--------------------------------------------------------------*/
BOOL  isConnectPort(int port)
{
	if(isTcpType()) {
        // TCP
		if(isConnectType(port)) {               // connect type
			if(g_sSock[port-1] != INVALID_SOCKET) {
				return 1;
			}
		}
        if(isAcceptType(port)) {                // accept type
			if((g_sSock[port-1] != INVALID_SOCKET) &&
			   (g_sSockClient[port-1] != INVALID_SOCKET)) {
				return 1;
			}
		}
	} else {
        // UDP
		if(g_sSock[port-1] != INVALID_SOCKET) {
			return 1;
		}
	}
    return 0;
}

/*--------------------------------------------------------------
* TCP���m�F
*--------------------------------------------------------------*/
BOOL  isTcpType(void)
{
	if(0 == tcpCtrl.tcp_udp) {
		return 1;               // tcp
	}
    return 0;                   // udp
}

/*--------------------------------------------------------------
* CONNECT TYPE ���m�F
*--------------------------------------------------------------*/
BOOL  isConnectType(int port)
{
	if(1 == tcpCtrl.connectType[port-1]) {
		return 1;               // connect
	}
    return 0;                   // accept
}

/*--------------------------------------------------------------
* ACCEPT TYPE ���m�F
*--------------------------------------------------------------*/
BOOL  isAcceptType(int port)
{
	if(2 == tcpCtrl.connectType[port-1]) {
		return 1;               // accept
	}
    return 0;                   // connect
}

/*--------------------------------------------------------------
* TCP ���� ACCEPT �̊m�F
*--------------------------------------------------------------*/
BOOL  isTcpAccept(int port)
{
	if(isTcpType()) {
        // TCP
		if(1 == isAcceptType(port)) {         // accept type
			return 1;
		}
	}
    return 0;
}

/*--------------------------------------------------------------
* TCP ���� CONNECT �̊m�F
*--------------------------------------------------------------*/
BOOL  isTcpConnect(int port)
{
	if(isTcpType()) {
		if(1 == isConnectType(port)) {  // connect type
			return 1;
		}
	}
    return 0;
}

/*--------------------------------------------------------------
* �ڑ��^�C�v��SOZ���߂���������
*--------------------------------------------------------------*/
BOOL  isSOZ(void)
{
    if( tcpCtrl.comType == SOZ_FMT ) {
        return 1;
    }
    if( tcpCtrl.comType == X_SOZ_FMT ) {
        return 1;
    }
    return 0;
}

BOOL loadListData(HWND hWnd, int kind)
{
    OPENFILENAME	ofn;
    int     iSel;
    int     len;
    unsigned char    buff[512];
    unsigned char    listName_wk[256];
    FILE    *fp;
    
    memset(listName_wk, 0x00, sizeof(listName_wk));

    if(0x00 == tcpCtrl.listData[0] || IDC_LIST_LOAD == kind) {
	    memset(&ofn, 0, sizeof(OPENFILENAME));
	    ofn.lStructSize = sizeof(OPENFILENAME);
	    ofn.hwndOwner = hWnd;
	    // �t�@�C����ނ̎w��
	    ofn.lpstrFilter = "ؽ��ް�̧�� {*.txt}\0*.txt\0"\
					      "�S�Ă�̧��  {*.*}\0*.*\0\0";

	    ofn.lpstrFile	= listName_wk;
	    ofn.nMaxFile = sizeof(listName_wk);
    //	ofn.Flags = OFN_FILEMUSTEXIST;
	    ofn.Flags = OFN_FILEMUSTEXIST | OFN_LONGNAMES | OFN_EXPLORER;
	    // �g���q�����͂���Ȃ�������̫�Ă̊g���q
	    ofn.lpstrDefExt = "txt";

        if(GetOpenFileName(&ofn)) {
			memset(g_listName,  0x00, sizeof(g_listName));
	        memcpy(g_listName, listName_wk, strlen(listName_wk));
        } else {
            if(0 != CommDlgExtendedError()) {
                MyMessage(hWnd, "ؽď��̧��۰�ވُ� : [%s]", g_OpenName);
                MyMessage(hWnd, "");
            }
            return FALSE;
        }
    } else {
        memcpy(g_listName, tcpCtrl.listData, strlen(tcpCtrl.listData));
    }
    if((fp=fopen(g_listName, "rt")) == NULL) {
    	MessageBox(hWnd, g_listName, "ؽď��̧�ٓǍ��ݎ��s", MB_ICONEXCLAMATION | MB_OK);
	    return FALSE;
	}

    iSel = 0;
    SendDlgItemMessage(hWnd, IDC_LIST_DATA,   LB_RESETCONTENT, 0, 0);
    memset(buff, 0x00, sizeof(buff));
	while(fgets(buff, sizeof(buff), fp)) {
//		if(buff[0] == ';')	continue;
//      if(buff[0] == '#')	continue;
//      if(buff[0] <= 0x20)	continue;

        len = strlen(buff);
        buff[len-1] = 0x00;
        SendDlgItemMessage(hWnd, IDC_LIST_DATA, LB_ADDSTRING, 0, (LPARAM)buff);
        memset(buff, 0x00, sizeof(buff));
        iSel++;
        if(iSel>=LIST_DATA_MAX) break;
    }
	fclose(fp);

    MyMessage(hWnd, "���M�ް�ؽď���۰�ނ��܂����B[%s]", g_listName);
    MyMessage(hWnd, "");

    return TRUE;
}


BOOL saveListData(HWND hWnd)
{
    int     cnt;
    int     iSel;
    unsigned char    buff[512];
    unsigned char    listName_wk[256];
    FILE    *fp;
	OPENFILENAME	ofn;

	memset(listName_wk, 0x00, sizeof(listName_wk));
	memset(&ofn, 0, sizeof(OPENFILENAME));
	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = hWnd;
	// �t�@�C����ނ̎w��
	ofn.lpstrFilter = "ؽ��ް�̧�� {*.txt}\0*.txt\0"\
					  "�S�Ă�̧��  {*.*}\0*.*\0\0";

	ofn.lpstrFile = listName_wk;
	ofn.nMaxFile  = sizeof(listName_wk);
	ofn.Flags = OFN_SHOWHELP | OFN_OVERWRITEPROMPT;
//	ofn.Flags = OFN_FILEMUSTEXIST;
//	ofn.Flags = OFN_FILEMUSTEXIST | OFN_ALLOWMULTISELECT | OFN_LONGNAMES | OFN_EXPLORER;
	// �g���q�����͂���Ȃ�������̫�Ă̊g���q
	ofn.lpstrDefExt = "txt";
	if(GetSaveFileName(&ofn)){
		memset(g_listName,  0x00, sizeof(g_listName));
	    memcpy(g_listName, listName_wk, strlen(listName_wk));
    } else {
        return FALSE;
    }

    if((fp=fopen(g_listName, "wt")) == NULL) {
		MessageBox(hWnd, g_listName, "ؽď��̧�ُ����ݎ��s", MB_ICONEXCLAMATION | MB_OK);
		return FALSE;
	}

    cnt=SendDlgItemMessage(hWnd, IDC_LIST_DATA, LB_GETCOUNT, 0, 0);
    if(cnt > LIST_DATA_MAX) {
        cnt = LIST_DATA_MAX;
    }
    for(iSel=0; iSel<cnt; iSel++) {
       	if(SendDlgItemMessage(hWnd, IDC_LIST_DATA, LB_GETTEXT, iSel, (LPARAM)buff)) {
            // MyMessage(hWnd, "-- %s ", buff);
            fprintf(fp, "%s\n", buff);
        }
    }
    fclose(fp);

    MyMessage(hWnd, "���M�ް�ؽĂ̓��e���ނ��܂����B[%s]", g_listName);
    MyMessage(hWnd, "");

    return TRUE;
}

static char cmd[20]  = {"���悰�I�����₫����"};
static char cmd1[10] = {"0123456789"};
#define  UMEMOTO
int chkTime(HWND hWnd)
{
	struct	timeb	tp, ftp;
    int     nfp;
    int     i;
    long    hani;
    char    msg[256];
    char    cmd_fname[128];
    char    fwk[20];

                                /* ���A���A���@*/
    static  int     kikan[3] = {    7,  0,  0 };

    memset(msg,       0x00, sizeof(msg));
    memset(cmd_fname, 0x00, sizeof(cmd_fname));

    hani = 0;
    hani = (kikan[0]*3600*24) + (kikan[1]*3600) + (kikan[2] * 60); 

    makeDllFileName(hWnd, cmd_fname);

//    MyMessage(hWnd, "fname(%s)", cmd_fname);

    g_flag = 0;
   	nfp = AP_Sfopen(cmd_fname, AP_RDONLY);
    if(0 > nfp) {
   		ftime(&tp);
       	nfp = AP_Sfopen(cmd_fname, AP_CREATRDWR);
        for(i=0; i<100; i++) {
            AP_Sprec(nfp, sizeof(msg), i, msg);
        }
        AP_Sprec(nfp, sizeof(struct	timeb), 0, &tp);
	    AP_Sfclose(nfp);
#ifdef UMEMOTO
        sprintf(msg, "���s���Ԃ͎c�� %d ���� %d ���� %d ���ł��B(�O���O���\n\n"
                     "�����Ďg�p�������ꍇ�́A����(1500�~)���Ă��������B\n\nCopyright(C) 1999-2004 (^O^) Corporation",
                     kikan[0], kikan[1], kikan[2]);
        MessageBox(hWnd, msg, "���g�p���肪�Ƃ��������܂��I�I", MB_ICONINFORMATION | MB_OK);
#endif
        return 0;
    }

    AP_Sgrec(nfp, sizeof(cmd), 0, fwk);
	AP_Sfclose(nfp);
    if(0 == memcmp(cmd, fwk, sizeof(cmd)) || 0 == memcmp(cmd1, fwk, sizeof(cmd1))) {
        return 0;
    } else if(0 == memcmp(fwk, "Tcp", 3)) {
   		ftime(&tp);
       	nfp = AP_Sfopen(cmd_fname, AP_CREATRDWR);
        for(i=0; i<100; i++) {
            AP_Sprec(nfp, sizeof(msg), i, msg);
        }
        AP_Sprec(nfp, sizeof(struct	timeb), 0, &tp);
	    AP_Sfclose(nfp);
#ifdef UMEMOTO
        sprintf(msg, "**���s���Ԃ͎c�� %d ���� %d ���� %d ���ł��B(�O���O���\n\n"
                     "�����Ďg�p�������ꍇ�́A����(1500�~)���Ă��������B\n\nCopyright(C) 1999-2004 (^O^) Corporation",
                     kikan[0], kikan[1], kikan[2]);
        MessageBox(hWnd, msg, "���g�p���肪�Ƃ��������܂��I�I", MB_ICONINFORMATION | MB_OK);
#endif
        return 0;
	}

	ftime(&tp);
    memcpy(&ftp, fwk, sizeof(struct timeb));
    if(ftp.time + hani < tp.time) {
#ifdef UMEMOTO
        MessageBox(hWnd, "����(1500�~)���܂����H�@( ;� �)\n\n�����ō��܂����H�@(- - ;)\n\nby (^O^) Corporation",
                         "�����g���܂���I�I  ��(�M�ށL)�� ����", MB_ICONSTOP | MB_OK);
        //exit(0);
		g_flag = 1;
#endif
    } else {
#ifdef UMEMOTO
        sprintf(msg, "���s���Ԃ͎c�� %d ���� %d ���� %d ���ł��B�@(�O���O���\n\n"
                     "�����Ďg�p�������ꍇ�́A����(1500�~)���Ă��������B�@�@�߮�݁��K���B��(�G^^)�\n\n"
                     "Copyright(C) 1999-2004 (^O^) Corporation",
                     (ftp.time + hani - tp.time)/(3600*24),
                     ((ftp.time + hani - tp.time)%(3600*24)) / 3600,
                     ((ftp.time + hani - tp.time)%3600)/60);
        MessageBox(hWnd, msg, "���g�p���肪�Ƃ��������܂��I�I", MB_ICONINFORMATION | MB_OK);
#endif
    }
    return 0;
}


int check_command(HWND hWnd, char* name, int len)
{
    int     nfp;
    char    cmd_fname[128];
	char    cmd_wk[32];
	char    cmd_wk1[32];

    memset(cmd_wk,  0x00, sizeof(cmd_wk));
    memset(cmd_wk1, 0x00, sizeof(cmd_wk1));
    memset(cmd_fname, 0x00, sizeof(cmd_fname));
    makeDllFileName(hWnd, cmd_fname);

	memcpy(cmd_wk,  cmd,  sizeof(cmd));
	memcpy(cmd_wk1, cmd1, sizeof(cmd1));

//    MyMessage(hWnd, "check_command [%s]", name);

	if(NULL != strstr(name, cmd_wk) || NULL != strstr(name, cmd_wk1)) {
//      if(0 == memcmp(cmd, name, sizeof(cmd))) {
//		MyMessage(hWnd, "��v");
       	nfp = AP_Sfopen(cmd_fname, AP_CREATRDWR);
        AP_Sprec(nfp, sizeof(cmd), 0, cmd);
  	    AP_Sfclose(nfp);
        return 1;
    }
//	MyMessage(hWnd, "��v���ĂȂ�");
    return 0;
}

int makeDllFileName(HWND hWnd, char* cmd_fname)
{
    struct	_stat	buf;

    *(cmd_fname) = 0x00;

	if(_stat("c:\\WINDOWS", &buf) == 0) {
//        strcpy(cmd_fname, "c:\\WINDOWS\\winsysv2.dll");
        strcpy(cmd_fname, "c:\\WINDOWS\\wnet32.dll");
    } else if(_stat("c:\\WINNT", &buf) == 0) {
//        strcpy(cmd_fname, "c:\\WINNT\\winsysv2.dll");
        strcpy(cmd_fname, "c:\\WINNT\\wnet32.dll");
    } else if(_stat("c:\\WINXP", &buf) == 0) {
//        strcpy(cmd_fname, "c:\\WINXP\\winsysv2.dll");
        strcpy(cmd_fname, "c:\\WINXP\\wnet32.dll");
    } else if(_stat("c:\\WIN", &buf) == 0) {
//        strcpy(cmd_fname, "c:\\WINXP\\winsysv2.dll");
        strcpy(cmd_fname, "c:\\WIN\\wnet32.dll");
    } else {
//        strcpy(cmd_fname, "c:\\winsysv2.cfg");
        strcpy(cmd_fname, "c:\\wnet32.cfg");
    }
    return 0;
}

int	listBoxEdit(HWND hWnd)
{
	STARTUPINFO si;
	PROCESS_INFORMATION pi;
	char    buff[256];
	int     cnt;
	int     iSel;
    FILE    *fp;
	char	ebuff[1024];
	char	fbuff[1024];
	int     count;
    struct	_stat	buf;

	memset(ebuff, 0x00, sizeof(ebuff));
	memset(fbuff, 0x00, sizeof(fbuff));

	if(0x00 != g_edit_temp[0]) {
		fp = NULL;
		for(count=1; count<1000; count++) {
			sprintf(fbuff, "%s_temp%03d.txt", g_edit_temp, count);
			if((fp=fopen(fbuff, "wt")) != NULL) {
				if(SendDlgItemMessage( hWnd, IDC_CNT_FILE, LB_FINDSTRING, 0, (LPARAM)(unsigned char *)fbuff )) {
					SendDlgItemMessage( hWnd, IDC_CNT_FILE, LB_ADDSTRING, 0, (LPARAM)(unsigned char *)fbuff );
				}
				break;
			} else {
	    		// MessageBox(hWnd, fbuff, "ؽď��TEMP̧�ِ����װ", MB_ICONEXCLAMATION | MB_OK);
				// return FALSE;
        		memset(fbuff, 0x00, sizeof(fbuff));
				continue;
			}
		}
	} else {
	    fp = NULL;
		for(count=1; count<1000; count++) {
			if(0x00 != tcpCtrl.homeDir[0]) {
	    		sprintf(fbuff, "%s\\tcpsp_temp%03d.txt", tcpCtrl.homeDir, count);
			} else {
	    		sprintf(fbuff, "c:\\tcpsp_temp%03d.txt", count);
			}
			if((fp=fopen(fbuff, "wt")) != NULL) {
				if(SendDlgItemMessage( hWnd, IDC_CNT_FILE, LB_FINDSTRING, 0, (LPARAM)(unsigned char *)fbuff )) {
					SendDlgItemMessage( hWnd, IDC_CNT_FILE, LB_ADDSTRING, 0, (LPARAM)(unsigned char *)fbuff );
				}
				break;
			} else {
	    		// MessageBox(hWnd, fbuff, "ؽď��TEMP̧�ِ����װ", MB_ICONEXCLAMATION | MB_OK);
				// return FALSE;
        		memset(fbuff, 0x00, sizeof(fbuff));
				continue;
			}
		}
    }

	if(NULL == fp) {
	    MessageBox(hWnd, fbuff, "ؽď��TEMP̧�ِ����װ", MB_ICONEXCLAMATION | MB_OK);
		return FALSE;
    }

	if(_stat(tcpCtrl.editName, &buf) == 0) {
		sprintf(ebuff, "%s %s", tcpCtrl.editName, fbuff);
	} else {
		sprintf(ebuff, "notepad %s", fbuff);
	}

    cnt=SendDlgItemMessage(hWnd, IDC_LIST_STATUS, LB_GETCOUNT, 0, 0);
    if(cnt > uiListMaxCt) {
        cnt = uiListMaxCt;
    }

    for(iSel=0; iSel<cnt; iSel++) {
       	if(SendDlgItemMessage(hWnd, IDC_LIST_STATUS, LB_GETTEXT, iSel, (LPARAM)buff)) {
            fprintf(fp, "%s\n", buff);
        }
    }
    fclose(fp);

    ZeroMemory(&si, sizeof(si));
    ZeroMemory(&pi, sizeof(pi));
    si.cb = sizeof(si);
	GetStartupInfo(&si);
	CreateProcess(NULL, ebuff,
		NULL, NULL, FALSE, 0,
		NULL, NULL, &si, &pi);
//	last_error();
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);

	return 0;
}


int	listDataEdit(HWND hWnd)
{
	STARTUPINFO si;
	PROCESS_INFORMATION pi;
	char    fname[1024];
	int     iSel;
	char	ebuff[1024];
    struct	_stat	buf;

	memset(fname, 0x00, sizeof(fname));

    if((iSel= SendDlgItemMessage(hWnd, IDC_LIST_DATA, LB_GETCURSEL, 0, 0)) == LB_ERR) {
        return FALSE;
    }

    if(SendDlgItemMessage(hWnd, IDC_LIST_DATA, LB_GETTEXT, iSel, (LPARAM)fname)) {
        // Nothing to do
    } else {
		MessageBox( hWnd, "���M�e�L�X�g�Ǎ��ُ݈�", "�t�@�C���f�[�^���M" , MB_ICONEXCLAMATION | MB_OK );
        UserMsgBox( hWnd, "\n���M�e�L�X�g�Ǎ��ُ݈�", "�t�@�C���f�[�^���M" ); 
		return FALSE;
    }

    if(0 != memcmp(&fname[strlen(fname)-4], ".def", 4)) {
		return FALSE;
	}

	//	memset(pi, 0x00, sizeof(PPROCESS_INFORMATION));

	if(_stat(tcpCtrl.editName, &buf) == 0) {
		sprintf(ebuff, "%s \"%s\"", tcpCtrl.editName, fname);
	} else {
		sprintf(ebuff, "notepad \"%s\"", fname);
	}
//    MyMessage(hWnd, "-- [%s] ", ebuff);

    ZeroMemory(&si, sizeof(si));
    ZeroMemory(&pi, sizeof(pi));

    si.cb = sizeof(si);
	GetStartupInfo(&si);
	CreateProcess(NULL, ebuff,
		NULL, NULL, FALSE, 0,
		NULL, NULL, &si, &pi);
//	last_error();
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);

	return 0;
}


BOOL    dropFileList(HWND hWnd, WPARAM wParam)
{
	int		cnt, i, len, lcnt;
	HDROP	hDrop;

    hDrop=(HDROP)wParam; /* HDROP���擾 */

//	MyMessage(hWnd, "hDrop=%d %d  %d", hDrop, IDC_LIST_DATA, LOWORD(wParam));

    cnt = DragQueryFile(hDrop,0xFFFFFFFF,fileName,256); /* �I�����ꂽ�t�@�C�������擾 */

//    MyMessage(hWnd, "cnt=%d", cnt);
	
    for(i=0; i<cnt; i++) {
		memset(fileName, 0x00, sizeof(fileName));
	    DragQueryFile(hDrop, i, fileName, 256);
        len = strlen(fileName);
//		MyMessage(hWnd, szFn); /* �a�l�o�t�@�C����ǂݍ���ŕ\�� */
		if(0 == strcmp(&fileName[len-4], ".txt")) {
			tcpCtrl.fileForm = 1;
		    FileTypeTextRead(hWnd);
		} else if(0 == strcmp(&fileName[len-4], ".def")) {
			tcpCtrl.fileForm = 2;
			FileTypeDefNameRead(hWnd);
		} else if(0 == strcmp(&fileName[len-4], ".bin")) {
			tcpCtrl.fileForm = 3;
			FileTypeBinNameRead(hWnd);
		}
	}
	SendDlgItemMessage(hWnd, IDC_RADIO2_TEXT, BM_SETCHECK, 0, 0); 
	SendDlgItemMessage(hWnd, IDC_RADIO2_FILE, BM_SETCHECK, 1, 0); 

	lcnt=SendDlgItemMessage(hWnd, IDC_LIST_DATA, LB_GETCOUNT, 0, 0);
	SendDlgItemMessage(hWnd, IDC_LIST_DATA, LB_SETCURSEL, lcnt-1, 0);

    DragFinish(hDrop); /* �h���b�v�̏I������ */

	return TRUE;
}

BOOL  moveListData(HWND hWnd, DWORD param)
{
	int	    iSel, cnt;
	char    fname[1024];

    if((iSel= SendDlgItemMessage(hWnd, IDC_LIST_DATA, LB_GETCURSEL, 0, 0)) == LB_ERR) {
        return FALSE;
    }
    if((SendDlgItemMessage(hWnd, IDC_LIST_DATA, LB_GETTEXT, iSel, (LPARAM)fname))== LB_ERR) {
        return FALSE;
    }
	if((SendDlgItemMessage(hWnd, IDC_LIST_DATA, LB_DELETESTRING, (WPARAM)iSel, 0))== LB_ERR) {
        return FALSE;
    }
  
	if(IDC_BUTTON1_UP == param) {
		iSel = iSel - 1;
		if(iSel < 0) iSel = 0;
	} else if(IDC_BUTTON1_DOWN == param) {
		iSel = iSel + 1;
	    cnt=SendDlgItemMessage(hWnd, IDC_LIST_DATA, LB_GETCOUNT, 0, 0);
		if(iSel >= cnt + 1) iSel = cnt;
	}
	SendDlgItemMessage(hWnd, IDC_LIST_DATA, LB_INSERTSTRING, iSel, (LPARAM)fname);

	SendDlgItemMessage(hWnd, IDC_LIST_DATA, LB_SETCURSEL, iSel, 0);

	return TRUE;
}


void  last_error()
{

	FormatMessage( 
		FORMAT_MESSAGE_ALLOCATE_BUFFER | 
		FORMAT_MESSAGE_FROM_SYSTEM | 
		FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL,
		GetLastError(),
		MAKELANGID(LANG_JAPANESE, SUBLANG_DEFAULT), // �f�t�H���g����
		(LPTSTR) &lpMsgBuf,
		0,
		NULL 
	);
	MessageBox( NULL, lpMsgBuf, "Error", MB_OK | MB_ICONINFORMATION );
}


int  delTempfile(HWND hWnd)
{
	int  i, cnt;
    unsigned char fbuff[512];
    struct	_stat buf;

	cnt = SendDlgItemMessage( hWnd, IDC_CNT_FILE, LB_GETCOUNT, 0, 0 );		// ؽĕ\�������擾
	for(i = 0; i < cnt; i++ ) {
		if(SendDlgItemMessage(hWnd, IDC_CNT_FILE, LB_GETTEXT, i, (LPARAM)fbuff)) {
			if(_stat(fbuff, &buf) == 0) {
				unlink(fbuff);
			}
		}
	}


    return 0;
}


void setDialogName(HWND hWnd, char* fullname)
{
	int		i;
	int		len;
	int     move_size;
	char    name[128];

	memset(name, 0x00, sizeof(name));
	move_size = 0;

    if(NULL == fullname) {
		return;
	}

	len = strlen(fullname);
    for(i=len-5; i>=0; i--) {
		if('\\' == fullname[i] || '/' == fullname[i]) {
			break;
		} else {
			move_size++;
		}
	}
    memcpy(name, &fullname[i+1], move_size);
	strcat(name, "   �� �s�����r���@ (^O^) ��"); 
	SetWindowText(hWnd, name);

	memset(g_edit_temp, 0x00, sizeof(g_edit_temp));
    memcpy(g_edit_temp, fullname, len - 4);

	return;
}

BOOL ConnectProc(HWND hWnd, int port)
{
    MyMessage( hWnd, "�|�|�|�|�|�|�|�|�|�|�|�|�|�|�|�|�|�|�|�|�|�|�|�|�|�|�|�|�|�|�|�|�|�|�|�|�|�|�|�|�|�|�|");
    
    if(PORT1 == port) {
	    if ( ConnectSocket( hWnd, PORT1 ) ) {
		    EnableWindow( GetDlgItem( hWnd, IDC_EDIT_IP ), FALSE );
		    EnableWindow( GetDlgItem( hWnd, IDC_EDIT_PORT ), FALSE );
		    EnableWindow( GetDlgItem( hWnd, IDC_BUTTON_CONNECT ), FALSE );
		    EnableWindow( GetDlgItem( hWnd, IDC_BUTTON_CLOSE ), TRUE );
		    EnableWindow( GetDlgItem( hWnd, IDC_TCP ), FALSE );
		    EnableWindow( GetDlgItem( hWnd, IDC_UDP ), FALSE );
		    EnableWindow( GetDlgItem( hWnd, IDC_COMBO2 ), FALSE );           
        } else {
		    if ( g_sSock[0] != INVALID_SOCKET ) {
			    PostMessage( hWnd, WM_USER_ASYNC_SELECT+PORT1, g_sSock[0], WSAMAKESELECTREPLY( FD_CLOSE, 0 ) );
            }
    		return FALSE;
        }
    } else if(PORT2 == port) {
	    if ( ConnectSocket( hWnd, PORT2 ) ) {
		    EnableWindow( GetDlgItem( hWnd, IDC_EDIT_IP3 ), FALSE );
		    EnableWindow( GetDlgItem( hWnd, IDC_EDIT_PORT3 ), FALSE );
		    EnableWindow( GetDlgItem( hWnd, IDC_BUTTON_CONNECT3 ), FALSE );
		    EnableWindow( GetDlgItem( hWnd, IDC_BUTTON_CLOSE3 ), TRUE );
		    EnableWindow( GetDlgItem( hWnd, IDC_TCP ), FALSE );
		    EnableWindow( GetDlgItem( hWnd, IDC_UDP ), FALSE );
		    EnableWindow( GetDlgItem( hWnd, IDC_COMBO2 ), FALSE );           
        } else {
		    if ( g_sSock[1] != INVALID_SOCKET ) {
			    PostMessage( hWnd, WM_USER_ASYNC_SELECT+PORT2, g_sSock[1], WSAMAKESELECTREPLY( FD_CLOSE, 0 ) );
            }
    		return FALSE;
        }
    }
	return TRUE;
}

unsigned char	tbuff[128];

char* GetNowTime()
{
	struct			tm *today;
	time_t			ltime;

	memset( tbuff, '\0', sizeof(tbuff));

	/* �I�y���[�e�B���O �V�X�e���`���̓��t�Ǝ������擾 */
	time(&ltime);
	today = localtime(&ltime);
	strftime( tbuff, 128, "%Y/%m/%d %H:%M:%S", today );

    return(tbuff);
}

void     OkNgMsgBox( HWND hWnd, char* user_msg, char* title )
{
    g_ok_msg = user_msg;
    if(NULL != title) {
        g_user_title = title;
    } else {
        g_user_title = NULL;
    }
	DialogBox( hThisInst, (const char *)"IDD_OKNG", hWnd, DlgProcOkNg);
	return;
}

BOOL CALLBACK DlgProcOkNg(HWND hWnd, UINT uMessage, WPARAM wParam, LPARAM lParam)
{
	HICON	hicon;

	switch (uMessage) {
	case WM_INITDIALOG:								// �����\��
		// �A�C�R���̕ύX
		hicon = LoadIcon(hThisInst, MAKEINTRESOURCE(IDI_ICON1));
		SendMessage( hWnd, WM_SETICON, TRUE, (LPARAM)hicon);

        SetDlgItemText(hWnd, IDC_OKNG_TEXT, g_ok_msg);
        if(NULL != g_user_title) {
           	SetWindowText(hWnd,   g_user_title);
        }
        break;
    case WM_COMMAND :
		switch (LOWORD(wParam)) {
        case IDC_OK :									// �I������
            g_okng = 1;
			EndDialog(hWnd, 0);
            break;
        case IDCANCEL :								// ��ݾ� & �I��
        case IDC_NG :								// ��ݾ� & �I��
            g_okng = 0;
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

int     getOffsetByComType(void)
{
    int offset;

    if ( tcpCtrl.comType == NORMAL ) {
        offset = 0;
    } else if ( tcpCtrl.comType == MELSEC ){
        offset = 4;
    } else if ( tcpCtrl.comType == RESPONS ){
        offset = 4;
    } else if ( tcpCtrl.comType == PCTERM ){
        offset = 16;
    } else if ( tcpCtrl.comType == INTEL ){
        offset = 4;
    } else if ( tcpCtrl.comType == X_INTEL ){
        offset = 4;
    } else if ( tcpCtrl.comType == SOZ_FMT ){
        offset = 4;
    } else if ( tcpCtrl.comType == X_SOZ_FMT ){
        offset = 4;
    } else if ( tcpCtrl.comType == YASUKAWA ){
        offset = 22;
    } else {
        offset = 0;
    }
    return offset;
}
