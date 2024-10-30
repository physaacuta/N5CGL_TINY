// TO_PROCONDlg.cpp : À‘•ƒtƒ@ƒCƒ‹
//

#include "stdafx.h"
#include "TO_PROCON.h"
#include "TO_PROCONDlg.h"

//#ifdef _DEBUG
//#define new DEBUG_NEW
//#endif
// ƒƒ‚ƒŠ[ƒŠ[ƒNŒŸo è“®’Ç‰Á ----->>>
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif
// <<<------------------------


// CTO_PROCONDlg ƒ_ƒCƒAƒƒO




CTO_PROCONDlg::CTO_PROCONDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CTO_PROCONDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CTO_PROCONDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, m_ListBox);
}

BEGIN_MESSAGE_MAP(CTO_PROCONDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDOK, &CTO_PROCONDlg::OnBnClickedOk)
	ON_WM_CLOSE()
	ON_WM_TIMER()
	ON_LBN_DBLCLK(IDC_LIST1, &CTO_PROCONDlg::OnLbnDblclkList1)
//	ON_EN_CHANGE(IDC_STATE_PORT_RECV, &CTO_PROCONDlg::OnEnChangeStatePortRecv)
ON_BN_CLICKED(IDC_DUMP1, &CTO_PROCONDlg::OnBnClickedDump1)
ON_BN_CLICKED(IDC_DUMP2, &CTO_PROCONDlg::OnBnClickedDump2)
ON_BN_CLICKED(IDC_CHK_DEBUG_FUNC, &CTO_PROCONDlg::OnBnClickedChkDebugFunc)
ON_BN_CLICKED(IDC_CHK_LOG_ALLDUMP, &CTO_PROCONDlg::OnBnClickedChkLogAlldump)
ON_BN_CLICKED(IDC_BTN_TCP_CLOSE, &CTO_PROCONDlg::OnBnClickedBtnTcpClose)
ON_BN_CLICKED(IDC_BTN_DBG_SEND, &CTO_PROCONDlg::OnBnClickedBtnDbgSend)
ON_BN_CLICKED(IDC_BTN_DBG_RECV, &CTO_PROCONDlg::OnBnClickedBtnDbgRecv)
END_MESSAGE_MAP()


// CTO_PROCONDlg ƒƒbƒZ[ƒW ƒnƒ“ƒhƒ‰

BOOL CTO_PROCONDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// ‚±‚Ìƒ_ƒCƒAƒƒO‚ÌƒAƒCƒRƒ“‚ğİ’è‚µ‚Ü‚·BƒAƒvƒŠƒP[ƒVƒ‡ƒ“‚ÌƒƒCƒ“ ƒEƒBƒ“ƒhƒE‚ªƒ_ƒCƒAƒƒO‚Å‚È‚¢ê‡A
	//  Framework ‚ÍA‚±‚Ìİ’è‚ğ©“®“I‚És‚¢‚Ü‚·B
	SetIcon(m_hIcon, TRUE);			// ‘å‚«‚¢ƒAƒCƒRƒ“‚Ìİ’è
	SetIcon(m_hIcon, FALSE);		// ¬‚³‚¢ƒAƒCƒRƒ“‚Ìİ’è

	// TODO: ‰Šú‰»‚ğ‚±‚±‚É’Ç‰Á‚µ‚Ü‚·B
	
	//// ‰æ–Ê‰Šúİ’è
	CWnd::CheckRadioButton(IDC_DUMP1, IDC_DUMP2, IDC_DUMP1);	// ƒ_ƒ“ƒvo—Íó‘Ô

// ------------------------------->>>
	// ~ƒ{ƒ^ƒ“–³Œø
	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL) pSysMenu->EnableMenuItem(SC_CLOSE, MF_GRAYED);
	// ƒƒCƒ“ƒXƒŒƒbƒhŠJn
	unsigned int tid;	// ƒXƒŒƒbƒhID
	_beginthreadex(NULL, 0, MainThread, (LPVOID)this, 0, &tid);
// <<<-------------------------------
	return TRUE;  // ƒtƒH[ƒJƒX‚ğƒRƒ“ƒgƒ[ƒ‹‚Éİ’è‚µ‚½ê‡‚ğœ‚«ATRUE ‚ğ•Ô‚µ‚Ü‚·B
}

// ƒ_ƒCƒAƒƒO‚ÉÅ¬‰»ƒ{ƒ^ƒ“‚ğ’Ç‰Á‚·‚éê‡AƒAƒCƒRƒ“‚ğ•`‰æ‚·‚é‚½‚ß‚Ì
//  ‰º‚ÌƒR[ƒh‚ª•K—v‚Å‚·BƒhƒLƒ…ƒƒ“ƒg/ƒrƒ…[ ƒ‚ƒfƒ‹‚ğg‚¤ MFC ƒAƒvƒŠƒP[ƒVƒ‡ƒ“‚Ìê‡A
//  ‚±‚ê‚ÍAFramework ‚É‚æ‚Á‚Ä©“®“I‚Éİ’è‚³‚ê‚Ü‚·B

void CTO_PROCONDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // •`‰æ‚ÌƒfƒoƒCƒX ƒRƒ“ƒeƒLƒXƒg

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ƒNƒ‰ƒCƒAƒ“ƒg‚ÌlŠpŒ`—Ìˆæ“à‚Ì’†‰›
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ƒAƒCƒRƒ“‚Ì•`‰æ
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// ƒ†[ƒU[‚ªÅ¬‰»‚µ‚½ƒEƒBƒ“ƒhƒE‚ğƒhƒ‰ƒbƒO‚µ‚Ä‚¢‚é‚Æ‚«‚É•\¦‚·‚éƒJ[ƒ\ƒ‹‚ğæ“¾‚·‚é‚½‚ß‚ÉA
//  ƒVƒXƒeƒ€‚ª‚±‚ÌŠÖ”‚ğŒÄ‚Ño‚µ‚Ü‚·B
HCURSOR CTO_PROCONDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}






// ‚±‚êˆÈ~ è“®’Ç‰Á -------------------->>>

//PPPPPPPPPPPPPPPPPPPPP
// ~ƒ{ƒ^ƒ“
//QQQQQQQQQQQQQQQQQQQQQ
void CTO_PROCONDlg::OnClose()
{
	//CDialog::OnClose();
}
//PPPPPPPPPPPPPPPPPPPPP
// •Â‚¶‚éƒ{ƒ^ƒ“
//QQQQQQQQQQQQQQQQQQQQQ
void CTO_PROCONDlg::OnBnClickedOk()
{
	if( IDYES == MessageBox("I—¹‚µ‚Ü‚·‚©H", "d—v‚È‚¨’m‚ç‚¹", MB_YESNO | MB_DEFBUTTON2 | MB_ICONQUESTION) ) {
		LOG(em_MSG), "•Â‚¶‚éƒ{ƒ^ƒ“");
		COMMON_QUE que;
		que.nEventNo = FACT_KS_END;
		que.nLineNo = getlineid();					
		send_mail(getmytaskname(), ".", &que);	
	}
}

//PPPPPPPPPPPPPPPPPPPPP
// ƒƒ‚’ ‹N“®
//QQQQQQQQQQQQQQQQQQQQQ
void CTO_PROCONDlg::OnLbnDblclkList1()
{
	// ListBoxƒRƒ“ƒgƒ[ƒ‹‚ÌƒvƒƒpƒeƒB‚ÍAƒ\[ƒg‹Ö~‚É‚µ‚Ä‚¨‚­–
	// ListBoxƒRƒ“ƒgƒ[ƒ‹‚Ìƒ_ƒuƒ‹ƒNƒŠƒbƒNƒCƒxƒ“ƒg‚ğ’Ç‰Á
	mcls_pLog->ShowEdit();
}

//------------------------------------------
// ŠeƒvƒƒZƒX‰Šú‰»ˆ—
//------------------------------------------
void CTO_PROCONDlg::InitProcess()
{
	// ƒ^ƒXƒN‰Šú‰»ˆ—
	setlineid(LINE_ID);
	if( 0 != task_init(TO_PROCON) ) {
		syslog(SYSNO_TASK_INIT_ERR, "[%s]", TO_PROCON);
		_ASSERT(0);
		exit(-9);		// ©ƒvƒƒZƒX‹­§I—¹
		return;
	}

	// LogFileManagerƒCƒ“ƒXƒ^ƒ“ƒX¶¬ (‚±‚êˆÈ~ ƒƒOo—Í‰Â”\)
	setlocale(LC_ALL, "Japanese");	// ‚±‚ê‚ª–³‚¢‚Æ“ú–{ŒêƒpƒX‚ğŠÜ‚ŞƒtƒHƒ‹ƒ_‚Ìê‡A•Û‘¶‚Å‚«‚È‚¢B(ƒ‰ƒCƒuƒ‰ƒŠ‚Ì’†‚ÅÀs‚·‚é‚×‚«‚©–À‚Á‚Ä‚¢‚é‚ªA•ÏX‚·‚é‚Æƒ^ƒXƒN‘S‘Ì‚É‰e‹¿‚·‚é‚Ì‚ÅA‚Æ‚è‚ ‚¦‚¸‚±‚±‚Åİ’è)
	mcls_pLog = new LogFileManager(getmytaskname(), &m_ListBox);
	LOG(em_MSG), "[Dlg] ƒ^ƒXƒN‹N“®");
	LOG(em_MSG), "[Dlg] ŠeƒvƒƒZƒXŠJnˆ— Às’†EEE");

	// ƒƒCƒ“ƒCƒ“ƒXƒ^ƒ“ƒX ŠJnˆ—
	mcls_MI.SetLogMgr(mcls_pLog);
	mcls_MI.Init();
	
	//// ‰æ–ÊXV—pƒ^ƒCƒ}¶¬
	SetTimer(1, 500, NULL);

	// ƒƒCƒ“ƒXƒŒƒbƒh‚ÌÀs‚ğs‚¤
	LOG(em_MSG), "[Dlg] ŠeƒvƒƒZƒXŠJnˆ— Š®—¹");
	mcls_MI.Start();
	return;
}

//PPPPPPPPPPPPPPPPPPPPP
// ŠeƒvƒƒZƒXŠJ•úˆ—
//QQQQQQQQQQQQQQQQQQQQQ
void CTO_PROCONDlg::ExitProcess()
{
	//// ƒƒO ƒŠƒXƒgƒ{ƒbƒNƒX•\¦ˆ—I—¹
	mcls_pLog->SetListOut(false);					// ‚±‚êˆÈ~ ƒŠƒXƒgƒ{ƒbƒNƒX‚Ö‚ÌƒƒO•\¦‹Ö~

	// ƒƒCƒ“ƒCƒ“ƒXƒ^ƒ“ƒX I—¹ˆ—
	KillTimer(1);			// ‰æ–ÊXVƒ^ƒCƒ}
	mcls_MI.Stop();
	mcls_MI.Exit();

	//// ƒƒOƒNƒ‰ƒXŠJ•ú
	LOG(em_MSG), "[Dlg] ƒ^ƒXƒNI—¹");
	if(NULL != mcls_pLog){
		delete mcls_pLog;
		mcls_pLog = NULL;
	}
	
	// ƒ^ƒXƒNI—¹ˆ—
	task_exit();

	// ‰æ–Ê‚ÌI—¹—v‹
	PostMessage(WM_QUIT);
}

//------------------------------------------
// ƒƒCƒ“ƒXƒŒƒbƒh DlgThread
// LPVOID param ƒ_ƒCƒ„ƒƒOƒ|ƒCƒ“ƒ^
//------------------------------------------
unsigned WINAPI CTO_PROCONDlg::MainThread(void* param)
{
	CTO_PROCONDlg* p = (CTO_PROCONDlg *)param;
	COMMON_QUE que;			// óM\‘¢‘Ì

	//// ŠeƒvƒƒZƒX‰Šú‰»ˆ— (‚±‚êˆÈ‘O‚ÍƒƒOo—Í‹Ö~)
	p->InitProcess();

	//// ‹N“®—vˆöæ‚è‚İ ƒ‹[ƒvŠJn
	while(true) {
		recv_mail(&que);			// ‹N“®—vˆöæ‚è‚İ

		switch(que.nEventNo) {

//-----------------------------------------------------------------------------------------------
		case FACT_TO_PROCON_01:								// ƒvƒƒRƒ“ŒŸ¸ÀÑ‘—M—v‹ (©TO_DEFECT)
			if(true) {
				pLOG(em_MSG), "[Mail] ÀÑ‘—M“`•¶‘—M’Ê’m (FACT_TO_PROCON_01) [%s]", que.fc.data);
				p->mcls_MI.gcls_pSock->AddQue_Ziseki(&que);
			}
			break;

//-----------------------------------------------------------------------------------------------
		case FACT_TO_PROCON_09:						// ƒfƒoƒbƒN—p“`•¶óM’Ê’m (©GOT_CONTROL)
			pLOG(em_MSG), "==========================================================================");
			pLOG(em_MSG), "[Mail] ƒfƒoƒbƒN—p“`•¶óM’Ê’m (FACT_TO_PROCON_09) [TC=%d][%s]", que.mix.idata[0], que.mix.cdata[0]);

			// ƒƒCƒ“ƒCƒ“ƒXƒ^ƒ“ƒX‚Ö’Ê’m
			p->mcls_MI.SetDeliveryMail(E_DEF_DEBUG_RECV, &que);

			break;
//-----------------------------------------------------------------------------------------------
		case FACT_KS_PCNAME_CHANGE:							// PC–¼Ì•ÏX’Ê’m
			// –³ˆ—
			break;

//-----------------------------------------------------------------------------------------------
		case FACT_KS_END:									// I—¹—v‹
			// ŠeƒvƒƒZƒXI—¹ˆ—
			p->ExitProcess();
			return 0;

//-----------------------------------------------------------------------------------------------
		default:
			syslog(SYSNO_MAILNO_ERR, "[EventNo=%d]", que.nEventNo);
			break;
		}
	}

	return 0;
}

//PPPPPPPPPPPPPPPPPPPPP
// ‰æ–ÊXVƒ^ƒCƒ}
//QQQQQQQQQQQQQQQQQQQQQ
void CTO_PROCONDlg::OnTimer(UINT_PTR nIDEvent)
{
	CString strVal;	
	// TODO: ‚±‚±‚ÉƒƒbƒZ[ƒW ƒnƒ“ƒhƒ‰ ƒR[ƒh‚ğ’Ç‰Á‚·‚é‚©AŠù’è‚Ìˆ—‚ğŒÄ‚Ño‚µ‚Ü‚·B

	// ƒXƒe[ƒ^ƒX
	SetDlgItemText(IDC_STATE_PORT_RECV, mcls_MI.gcls_pSock->GetStateName(0) );
	SetDlgItemText(IDC_STATE_PORT_SEND, mcls_MI.gcls_pSock->GetStateName(1) );
	SetDlgItemText(IDC_STATE_PORT_RECV2, mcls_MI.gcls_pSock->GetSockStatName(0));
	SetDlgItemText(IDC_STATE_PORT_SEND2, mcls_MI.gcls_pSock->GetSockStatName(1));
	// TCPó‘Ô
	SetDlgItemText(IDC_EDIT_TCP_R, (mcls_MI.gcls_pSock->IsConnect(0) ? "›" : "~"));
	SetDlgItemText(IDC_EDIT_TCP_S, (mcls_MI.gcls_pSock->IsConnect(1) ? "›" : "~"));

	CDialog::OnTimer(nIDEvent);
}

//PPPPPPPPPPPPPPPPPPPPP
// ƒ_ƒ“ƒvo—Í 1Œ
//QQQQQQQQQQQQQQQQQQQQQ
void CTO_PROCONDlg::OnBnClickedDump1()
{
	
	// ƒ_ƒ“ƒvo—Íƒ‚[ƒhƒZƒbƒg
	mcls_MI.gcls_pSock->SetDumpMode(false);
}

//PPPPPPPPPPPPPPPPPPPPP
// ƒ_ƒ“ƒvo—Í ‘SŒ
//QQQQQQQQQQQQQQQQQQQQQ
void CTO_PROCONDlg::OnBnClickedDump2()
{
	// ƒ_ƒ“ƒvo—Íƒ‚[ƒhƒZƒbƒg
	mcls_MI.gcls_pSock->SetDumpMode(true);
}

//PPPPPPPPPPPPPPPPPPPPP
// [ƒfƒoƒbƒO—pFunction]ƒ`ƒFƒbƒNƒ{ƒbƒNƒX
//QQQQQQQQQQQQQQQQQQQQQ
void CTO_PROCONDlg::OnBnClickedChkDebugFunc()
{
	bool bVal = (BST_CHECKED == CWnd::IsDlgButtonChecked(IDC_CHK_DEBUG_FUNC) ? true : false);
	GetDlgItem(IDC_CHK_LOG_ALLDUMP)->EnableWindow(bVal);
	GetDlgItem(IDC_BTN_TCP_CLOSE)->EnableWindow(bVal);
	GetDlgItem(IDC_BTN_DBG_SEND)->EnableWindow(bVal);
	GetDlgItem(IDC_BTN_DBG_RECV)->EnableWindow(bVal);
	GetDlgItem(IDC_EDIT_ID)->EnableWindow(bVal);
	GetDlgItem(IDC_EDIT_DBG_TC)->EnableWindow(bVal);
	GetDlgItem(IDC_EDIT_DBG_BINFILE)->EnableWindow(bVal);
	GetDlgItem(IDC_STATIC_ID)->EnableWindow(bVal);
	GetDlgItem(IDC_STATIC_TC)->EnableWindow(bVal);
	GetDlgItem(IDC_STATIC_BINFILE)->EnableWindow(bVal);
	GetDlgItem(IDC_LIST1)->SetFocus();
}



//PPPPPPPPPPPPPPPPPPPPP
// [ƒƒO‘SŒo—Í]ƒ`ƒFƒbƒNƒ{ƒbƒNƒXƒCƒxƒ“ƒg 
//QQQQQQQQQQQQQQQQQQQQQ
void CTO_PROCONDlg::OnBnClickedChkLogAlldump()
{
	bool bVal = (BST_CHECKED == CWnd::IsDlgButtonChecked(IDC_CHK_LOG_ALLDUMP) ? true : false);
	mcls_MI.gcls_pSock->SetDumpMode(bVal);
	mcls_MI.gcls_pSock->SetDumpMode(bVal);
}

//PPPPPPPPPPPPPPPPPPPPP
// [‹­§Ø’f]ƒ{ƒ^ƒ“ƒCƒxƒ“ƒg
//QQQQQQQQQQQQQQQQQQQQQ
void CTO_PROCONDlg::OnBnClickedBtnTcpClose()
{
	//	if( BST_CHECKED != CWnd::IsDlgButtonChecked(IDC_DEBUG_MANUAL) ) return;
	LOG(em_WAR), "ƒeƒXƒg ‹­§Ø’f");
	mcls_MI.gcls_pSock->UserClose(false);
}

//PPPPPPPPPPPPPPPPPPPPP
// ‹[— ‘—Mˆ—
//QQQQQQQQQQQQQQQQQQQQQ
void CTO_PROCONDlg::OnBnClickedBtnDbgSend()
{
	if (IDYES != MessageBox("ƒeƒXƒg ÀÑ“`•¶‚ğ‘—M‚µ‚Ü‚·‚©H", "d—v‚È‚¨’m‚ç‚¹", MB_YESNO | MB_DEFBUTTON2 | MB_ICONQUESTION)) {
		return;
	}
	COMMON_QUE que;
	memset(&que, 0x00, sizeof(que));
	que.nEventNo = FACT_TO_PROCON_01;
	que.nLineNo = getlineid();
	GetDlgItemText(IDC_EDIT_ID, &que.fc.data[0], SIZE_SERIAL_FF_NO);
	send_mail(getmytaskname(), ".", &que);
}

//PPPPPPPPPPPPPPPPPPPPP
// ‹[— óMˆ—
//QQQQQQQQQQQQQQQQQQQQQ
void CTO_PROCONDlg::OnBnClickedBtnDbgRecv()
{
	if (IDYES != MessageBox("óM‚µ‚Ü‚·‚©H", "d—v‚È‚¨’m‚ç‚¹", MB_YESNO | MB_DEFBUTTON2 | MB_ICONQUESTION)) {
		return;
	}

		COMMON_QUE que;
		memset( &que, 0x00, sizeof(que) );
		que.nEventNo = FACT_TO_PROCON_09;
		que.nLineNo = getlineid();
	
		char cWk1[256];
		char cWk2[256];
		memset( cWk1, 0x00, sizeof(cWk1) );
		memset( cWk2, 0x00, sizeof(cWk2) );
		GetDlgItemText( IDC_EDIT_DBG_TC     , cWk1, sizeof(cWk1) );
		GetDlgItemText( IDC_EDIT_DBG_BINFILE, cWk2, sizeof(cWk2) );
	
	//	que.mix.idata[0] = atoi( cWk1 );
		que.mix.idata[0] = strtol( cWk1 , NULL, 0); // 10i/16i(0x??)“ü—Í‘Î‰
		memcpy( que.mix.cdata[0], cWk2, sizeof( que.mix.cdata[0]) * 5);
		send_mail( getmytaskname(), ".", &que );
}

