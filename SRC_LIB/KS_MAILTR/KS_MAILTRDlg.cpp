// KS_MAILTRDlg.cpp : À‘•ƒtƒ@ƒCƒ‹
//

#include "stdafx.h"
#include "KS_MAILTR.h"
#include "KS_MAILTRDlg.h"

//#ifdef _DEBUG
//#define new DEBUG_NEW
//#endif
// ƒƒ‚ƒŠ[ƒŠ[ƒNŒŸo è“®’Ç‰Á ----->>>
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif
// <<<------------------------


// CKS_MAILTRDlg ƒ_ƒCƒAƒƒO




CKS_MAILTRDlg::CKS_MAILTRDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CKS_MAILTRDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CKS_MAILTRDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, m_ListBox);
}

BEGIN_MESSAGE_MAP(CKS_MAILTRDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDOK, &CKS_MAILTRDlg::OnBnClickedOk)
	ON_LBN_DBLCLK(IDC_LIST1, &CKS_MAILTRDlg::OnLbnDblclkList1)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_DEBUG_MANUAL, &CKS_MAILTRDlg::OnBnClickedDebugManual)
	ON_BN_CLICKED(IDC_BTN_TRANS_FL, &CKS_MAILTRDlg::OnBnClickedBtnTransFl)
	ON_BN_CLICKED(IDC_BTN_TRANS_FC, &CKS_MAILTRDlg::OnBnClickedBtnTransFc)
	ON_BN_CLICKED(IDC_BTN_TRANS_MIX, &CKS_MAILTRDlg::OnBnClickedBtnTransMix)
	ON_BN_CLICKED(IDC_BTN_CLEAR_R, &CKS_MAILTRDlg::OnBnClickedBtnClearR)
END_MESSAGE_MAP()


// CKS_MAILTRDlg ƒƒbƒZ[ƒW ƒnƒ“ƒhƒ‰

BOOL CKS_MAILTRDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// ‚±‚Ìƒ_ƒCƒAƒƒO‚ÌƒAƒCƒRƒ“‚ğİ’è‚µ‚Ü‚·BƒAƒvƒŠƒP[ƒVƒ‡ƒ“‚ÌƒƒCƒ“ ƒEƒBƒ“ƒhƒE‚ªƒ_ƒCƒAƒƒO‚Å‚È‚¢ê‡A
	//  Framework ‚ÍA‚±‚Ìİ’è‚ğ©“®“I‚És‚¢‚Ü‚·B
	SetIcon(m_hIcon, TRUE);			// ‘å‚«‚¢ƒAƒCƒRƒ“‚Ìİ’è
	SetIcon(m_hIcon, FALSE);		// ¬‚³‚¢ƒAƒCƒRƒ“‚Ìİ’è

	// TODO: ‰Šú‰»‚ğ‚±‚±‚É’Ç‰Á‚µ‚Ü‚·B
// è“®’Ç‰Á -------------------->>>
	// ‰æ–ÊˆÊ’u (ƒTƒCƒY‚Í‚¢‚Ü‚Ì‚Ü‚Ü)
	//::SetWindowPos(m_hWnd, NULL, 1, 1, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
// ‰æ–Ê‰Šú‰»ƒZƒbƒg----------------------------------------->>>
	SetDlgItemTextA(IDC_PC, ".");
	SetDlgItemTextA(IDC_LINE, "0");
	GetDlgItem(IDC_IPADDR_UDP)->EnableWindow(false);
// <<<---------------------------------------------------------
	
	// ÀsŠÂ‹«”»•Ê—p
	CString		cTitle;
	GetWindowText(cTitle);
#ifdef _M_X64			// x64 ƒRƒ“ƒpƒCƒ‹
	cTitle += " x64";
#else					// x86 ƒRƒ“ƒpƒCƒ‹
	cTitle += " x86";
#endif
	SetWindowText(cTitle);

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

void CKS_MAILTRDlg::OnPaint()
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
HCURSOR CKS_MAILTRDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


// ‚±‚êˆÈ~ è“®’Ç‰Á -------------------->>>

//PPPPPPPPPPPPPPPPPPPPP
// ~ƒ{ƒ^ƒ“
//QQQQQQQQQQQQQQQQQQQQQ
void CKS_MAILTRDlg::OnClose()
{
	//CDialog::OnClose();		// Šù‘¶‚Ìˆ—‚ğƒ„ƒ
}

//PPPPPPPPPPPPPPPPPPPPP
// ‹­§I—¹ƒ{ƒ^ƒ“
//QQQQQQQQQQQQQQQQQQQQQ
void CKS_MAILTRDlg::OnBnClickedOk()
{
	if( IDYES == MessageBox("I—¹‚µ‚Ü‚·‚©H", "d—v‚È‚¨’m‚ç‚¹", MB_YESNO | MB_DEFBUTTON2 | MB_ICONQUESTION) ) {
		LOG(em_MSG), "[Dlg] ‹­§I—¹ƒ{ƒ^ƒ“");
		COMMON_QUE que;
		que.nEventNo = FACT_KS_END;
		que.nLineNo = LINE_ID;			
		send_mail(getmytaskname(), ".", &que);
	}
}

//PPPPPPPPPPPPPPPPPPPPP
// ƒƒ‚’ ‹N“®
//QQQQQQQQQQQQQQQQQQQQQ
void CKS_MAILTRDlg::OnLbnDblclkList1()
{
	// ListBoxƒRƒ“ƒgƒ[ƒ‹‚ÌƒvƒƒpƒeƒB‚ÍAƒ\[ƒg‹Ö~‚É‚µ‚Ä‚¨‚­–
	// ListBoxƒRƒ“ƒgƒ[ƒ‹‚Ìƒ_ƒuƒ‹ƒNƒŠƒbƒNƒCƒxƒ“ƒg‚ğ’Ç‰Á
	mcls_pLog->ShowEdit();
}


//------------------------------------------
// ŠeƒvƒƒZƒX‰Šú‰»ˆ—
//------------------------------------------
void CKS_MAILTRDlg::InitProcess()
{
	//// ˆê”Ô‰‚ß‚É‚¢‚ë‚¢‚ë‰Šú‰»
	mcls_pLog = NULL;

	int nRetc = task_init(KS_MAILTR);
	if( 0 != nRetc ) {
		if(-1 == nRetc) { syslog(SYSNO_TASK_INIT_ERR, "[%s] Šù‚Éƒ^ƒXƒN‹N“®Ï‚İ‚É‚æ‚èA‹N“®’†’fB", KS_MAILTR); }
		else			{ syslog(SYSNO_TASK_INIT_ERR, "[%s]", KS_MAILTR); }
		_ASSERT(0);
		exit(-9);		// ©ƒvƒƒZƒX‹­§I—¹
		return;
	}

	//// LogFileManagerƒCƒ“ƒXƒ^ƒ“ƒX¶¬ (‚±‚êˆÈ~ ƒƒOo—Í‰Â”\)
	setlocale(LC_ALL, "Japanese");	// ‚±‚ê‚ª–³‚¢‚Æ“ú–{ŒêƒpƒX‚ğŠÜ‚ŞƒtƒHƒ‹ƒ_‚Ìê‡A•Û‘¶‚Å‚«‚È‚¢B(ƒ‰ƒCƒuƒ‰ƒŠ‚Ì’†‚ÅÀs‚·‚é‚×‚«‚©–À‚Á‚Ä‚¢‚é‚ªA•ÏX‚·‚é‚Æƒ^ƒXƒN‘S‘Ì‚É‰e‹¿‚·‚é‚Ì‚ÅA‚Æ‚è‚ ‚¦‚¸‚±‚±‚Åİ’è)
	mcls_pLog = new LogFileManager(getmytaskname(), &m_ListBox);
	LOG(em_MSG), "[Dlg] ƒ^ƒXƒN‹N“®");
	LOG(em_MSG), "[Dlg] ŠeƒvƒƒZƒXŠJnˆ— Às’†EEE");

	//// ƒƒCƒ“ƒCƒ“ƒXƒ^ƒ“ƒX ŠJnˆ—
	mcls_MI.SetLogMgr(mcls_pLog);
	mcls_MI.Init();
	
	//// ‰æ–ÊXV—pƒ^ƒCƒ}¶¬
	SetTimer(1, 500, NULL);

	//// ƒƒCƒ“ƒXƒŒƒbƒh‚ÌÀs‚ğs‚¤
	LOG(em_MSG), "[Dlg] ŠeƒvƒƒZƒXŠJnˆ— Š®—¹");
	mcls_MI.Start();	
	return;
}

//PPPPPPPPPPPPPPPPPPPPP
// ŠeƒvƒƒZƒXŠJ•úˆ—
//QQQQQQQQQQQQQQQQQQQQQ
void CKS_MAILTRDlg::ExitProcess()
{
	//// ƒƒCƒ“ƒCƒ“ƒXƒ^ƒ“ƒX I—¹ˆ—
	KillTimer(1);			// ‰æ–ÊXVƒ^ƒCƒ}
	mcls_MI.Stop();
	mcls_MI.Exit();

	//// ƒƒO ƒŠƒXƒgƒ{ƒbƒNƒX•\¦ˆ—I—¹
	mcls_pLog->SetListOut(false);					// ‚±‚êˆÈ~ ƒŠƒXƒgƒ{ƒbƒNƒX‚Ö‚ÌƒƒO•\¦‹Ö~

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
unsigned WINAPI CKS_MAILTRDlg::MainThread(void* param)
{
	CKS_MAILTRDlg* p = (CKS_MAILTRDlg *)param;
	COMMON_QUE que;			// óM\‘¢‘Ì

	//// ŠeƒvƒƒZƒX‰Šú‰»ˆ— (‚±‚êˆÈ‘O‚ÍƒƒOo—Í‹Ö~)
	p->InitProcess();

	//// ‹N“®—vˆöæ‚è‚İ ƒ‹[ƒvŠJn
	while(true) {
		recv_mail(&que);			// ‹N“®—vˆöæ‚è‚İ

		switch(que.nEventNo) {
//*/
//-----------------------------------------------------------------------------------------------
		case FACT_KS_MAILTR_01:								// ƒ[ƒ‹ƒXƒƒbƒg“]‘——v‹ (©Šeƒ^ƒXƒN)
			pLOG(em_MSG), "[MAIL] ƒ[ƒ‹ƒXƒƒbƒg“]‘——v‹ (FACT_KS_MAILTR_01) [Task=%s][Pc=%s]", que.mailtr_que.cTaskName, que.mailtr_que.cPcName); 
			// ƒƒCƒ“ƒCƒ“ƒXƒ^ƒ“ƒX‚Ö’Ê’m
			p->mcls_MI.SetDeliveryMail( E_DEF_ML_TRANS, &que);
			break;
//*/
//-----------------------------------------------------------------------------------------------
		case FACT_KS_DB_STATEUS:							// DBó‘Ô•ÏX’Ê’m(©KS_SLAVE)
			break;

		case FACT_KS_PCNAME_CHANGE:								// PC–¼Ì•ÏX’Ê’m (©KS_SLAVE)
			pLOG(em_MSG), "[MAIL] PC–¼Ì•ÏX’Ê’m (FACT_KS_PCNAME_CHANGE)" );
			// PC–¼Ìæ“¾
			getpcname(); 
			break;

		case FACT_KS_END:									// I—¹—v‹
			// ŠeƒvƒƒZƒXI—¹ˆ—
			p->ExitProcess();
			return 0;

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
void CKS_MAILTRDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: ‚±‚±‚ÉƒƒbƒZ[ƒW ƒnƒ“ƒhƒ‰ ƒR[ƒh‚ğ’Ç‰Á‚·‚é‚©AŠù’è‚Ìˆ—‚ğŒÄ‚Ño‚µ‚Ü‚·B

	// UDP“]‘——v‹ óMî•ñæ“¾
	UdpMail* tRecvUdp = mcls_MI.gcls_TrUdpMgr.GetRecvUdp();
	COMMON_QUE	que;
	CString		ss;

	memcpy(&que, tRecvUdp->data, min(sizeof(que), tRecvUdp->datalen));

	SetDlgItemTextA(IDC_PC_R, tRecvUdp->hostname);
	SetDlgItemTextA(IDC_TASK_R, tRecvUdp->mailname);
	ss.Format("%d", que.nEventNo);
	SetDlgItemTextA(IDC_EVENT_R, ss);
	ss.Format("%d", que.nLineNo);
	SetDlgItemTextA(IDC_LINE_R, ss);

	CDialog::OnTimer(nIDEvent);
}

//PPPPPPPPPPPPPPPPPPPPP
// DEBUGƒ{ƒ^ƒ“‰Ÿ‰º
//QQQQQQQQQQQQQQQQQQQQQ
void CKS_MAILTRDlg::OnBnClickedDebugManual()
{
	bool bVal = ( BST_CHECKED == CWnd::IsDlgButtonChecked(IDC_DEBUG_MANUAL) ? true : false );
	GetDlgItem(IDC_BTN_TRANS_FL)->EnableWindow(bVal);
	GetDlgItem(IDC_BTN_TRANS_FC)->EnableWindow(bVal);
	GetDlgItem(IDC_BTN_TRANS_MIX)->EnableWindow(bVal);
	GetDlgItem(IDC_IPADDR_UDP)->EnableWindow(bVal);
	GetDlgItem(IDC_PC)->EnableWindow(bVal);
	GetDlgItem(IDC_TASK)->EnableWindow(bVal);
	GetDlgItem(IDC_EVENT)->EnableWindow(bVal);
	GetDlgItem(IDC_LINE)->EnableWindow(bVal);
	GetDlgItem(IDC_FL_1)->EnableWindow(bVal);
	GetDlgItem(IDC_FC_1)->EnableWindow(bVal);
	GetDlgItem(IDC_BTN_CLEAR_R)->EnableWindow(bVal);
}

//PPPPPPPPPPPPPPPPPPPPP
// ”’l“]‘—ƒ{ƒ^ƒ“‰Ÿ‰º
//QQQQQQQQQQQQQQQQQQQQQ
void CKS_MAILTRDlg::OnBnClickedBtnTransFl()
{
	COMMON_QUE	que;
	CString		sUdpAddr;
	CString		sTaskName;
	CString		sPcName;

	GetDlgItemTextA(IDC_IPADDR_UDP, sUdpAddr);
	GetDlgItemTextA(IDC_TASK, sTaskName);
	GetDlgItemTextA(IDC_PC, sPcName);
	memset(&que, 0x00, sizeof(que));
	que.nEventNo = GetDlgItemInt(IDC_EVENT);
	que.nLineNo = GetDlgItemInt(IDC_LINE);
	que.fl.data[0] = GetDlgItemInt(IDC_FL_1);

	// ƒ[ƒ‹“]‘—
	send_mail_trans(sUdpAddr, sTaskName, sPcName, &que);
}

//PPPPPPPPPPPPPPPPPPPPP
// •¶š“]‘—ƒ{ƒ^ƒ“‰Ÿ‰º
//QQQQQQQQQQQQQQQQQQQQQ
void CKS_MAILTRDlg::OnBnClickedBtnTransFc()
{
	COMMON_QUE	que;
	CString		sUdpAddr;
	CString		sTaskName;
	CString		sPcName;

	GetDlgItemTextA(IDC_IPADDR_UDP, sUdpAddr);
	GetDlgItemTextA(IDC_TASK, sTaskName);
	GetDlgItemTextA(IDC_PC, sPcName);
	memset(&que, 0x00, sizeof(que));
	que.nEventNo = GetDlgItemInt(IDC_EVENT);
	que.nLineNo = GetDlgItemInt(IDC_LINE);
	GetDlgItemTextA(IDC_FC_1, que.fc.data, sizeof(que.fc.data));

	// ƒ[ƒ‹“]‘—
	send_mail_trans(sUdpAddr, sTaskName, sPcName, &que);
}

//PPPPPPPPPPPPPPPPPPPPP
// ¬‡“]‘—ƒ{ƒ^ƒ“‰Ÿ‰º
//QQQQQQQQQQQQQQQQQQQQQ
void CKS_MAILTRDlg::OnBnClickedBtnTransMix()
{
	COMMON_QUE	que;
	CString		sUdpAddr;
	CString		sTaskName;
	CString		sPcName;

	GetDlgItemTextA(IDC_IPADDR_UDP, sUdpAddr);
	GetDlgItemTextA(IDC_TASK, sTaskName);
	GetDlgItemTextA(IDC_PC, sPcName);
	memset(&que, 0x00, sizeof(que));
	que.nEventNo = GetDlgItemInt(IDC_EVENT);
	que.nLineNo = GetDlgItemInt(IDC_LINE);
	que.mix.idata[0] = GetDlgItemInt(IDC_FL_1);
	GetDlgItemTextA(IDC_FC_1, que.mix.cdata[0], sizeof(que.mix.cdata[0]));

	// ƒ[ƒ‹“]‘—
	send_mail_trans(sUdpAddr, sTaskName, sPcName, &que);
}

//PPPPPPPPPPPPPPPPPPPPP
// •\¦ƒNƒŠƒAƒ{ƒ^ƒ“‰Ÿ‰º
//QQQQQQQQQQQQQQQQQQQQQ
void CKS_MAILTRDlg::OnBnClickedBtnClearR()
{
	// óMî•ñƒNƒŠƒA
	mcls_MI.gcls_TrUdpMgr.ClearRecvUdp();
}
