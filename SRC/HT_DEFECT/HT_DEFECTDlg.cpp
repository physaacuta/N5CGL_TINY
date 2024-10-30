// HT_DEFECTDlg.cpp : À‘•ƒtƒ@ƒCƒ‹
//

#include "stdafx.h"
#include "HT_DEFECT.h"
#include "HT_DEFECTDlg.h"

// ƒƒ‚ƒŠ[ƒŠ[ƒNŒŸo è“®’Ç‰Á ----->>>
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif
// <<<------------------------


// CHT_DEFECTDlg ƒ_ƒCƒAƒƒO


CHT_DEFECTDlg::CHT_DEFECTDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CHT_DEFECTDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CHT_DEFECTDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, m_ListBox);
	//DDX_Control(pDX, IDC_COMBO_MEN, m_cmbMen);
	DDX_Control(pDX, IDC_LIST_ST, m_ListSt);
	DDX_Control(pDX, IDC_LIST_TO, m_ListTo);
}

BEGIN_MESSAGE_MAP(CHT_DEFECTDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDOK, &CHT_DEFECTDlg::OnBnClickedOk)
	ON_WM_CLOSE()
	ON_WM_TIMER()
	ON_LBN_DBLCLK(IDC_LIST1, &CHT_DEFECTDlg::OnLbnDblclkList1)
	ON_BN_CLICKED(IDC_DEBUG_START, &CHT_DEFECTDlg::OnBnClickedDebugStart)
	ON_BN_CLICKED(IDC_DEBUG_STOP, &CHT_DEFECTDlg::OnBnClickedDebugStop)
	ON_BN_CLICKED(IDC_DEBUG_PARAM, &CHT_DEFECTDlg::OnBnClickedDebugParam)
	ON_BN_CLICKED(IDC_DEBUG_COILINF, &CHT_DEFECTDlg::OnBnClickedDebugCoilinf)
	ON_BN_CLICKED(IDC_DEBUG_CANOK, &CHT_DEFECTDlg::OnBnClickedDebugCanok)
//	ON_BN_CLICKED(IDC_DEBUG_FRAME, &CHT_DEFECTDlg::OnBnClickedDebugFrame)
//	ON_BN_CLICKED(IDC_DEBUG_PROFILE, &CHT_DEFECTDlg::OnBnClickedDebugProfile)
//	ON_BN_CLICKED(IDC_DEBUG_DLYCANCEL, &CHT_DEFECTDlg::OnBnClickedDebugDlycancel)
	ON_BN_CLICKED(IDC_DEBUG_IMGKIND, &CHT_DEFECTDlg::OnBnClickedDebugImgkind)
	ON_BN_CLICKED(IDC_DEBUG_PARAM_PRELOAD, &CHT_DEFECTDlg::OnBnClickedDebugParamPreload)
	ON_BN_CLICKED(IDC_DEBUG_SET_POSITION, &CHT_DEFECTDlg::OnBnClickedDebugSetPosition)
	ON_BN_CLICKED(IDC_DEBUG_RESUME_START, &CHT_DEFECTDlg::OnBnClickedDebugResumeStart)
	ON_BN_CLICKED(IDC_DEBUG_RESUME_STOP, &CHT_DEFECTDlg::OnBnClickedDebugResumeStop)
	ON_BN_CLICKED(IDD_DEBUG_KIND1, &CHT_DEFECTDlg::OnBnClickedDebugKind1)
	ON_BN_CLICKED(IDD_DEBUG_KIND2, &CHT_DEFECTDlg::OnBnClickedDebugKind2)
	ON_BN_CLICKED(IDD_DEBUG_KIND3, &CHT_DEFECTDlg::OnBnClickedDebugKind3)
END_MESSAGE_MAP()


// CHT_DEFECTDlg ƒƒbƒZ[ƒW ƒnƒ“ƒhƒ‰

BOOL CHT_DEFECTDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// ‚±‚Ìƒ_ƒCƒAƒƒO‚ÌƒAƒCƒRƒ“‚ğİ’è‚µ‚Ü‚·BƒAƒvƒŠƒP[ƒVƒ‡ƒ“‚ÌƒƒCƒ“ ƒEƒBƒ“ƒhƒE‚ªƒ_ƒCƒAƒƒO‚Å‚È‚¢ê‡A
	//  Framework ‚ÍA‚±‚Ìİ’è‚ğ©“®“I‚És‚¢‚Ü‚·B
	SetIcon(m_hIcon, TRUE);			// ‘å‚«‚¢ƒAƒCƒRƒ“‚Ìİ’è
	SetIcon(m_hIcon, FALSE);		// ¬‚³‚¢ƒAƒCƒRƒ“‚Ìİ’è

	// TODO: ‰Šú‰»‚ğ‚±‚±‚É’Ç‰Á‚µ‚Ü‚·B

	//// ‰æ–Ê‰Šúİ’è
	CWnd::CheckDlgButton(IDC_DEBUG_MANUAL, 1);

	CWnd::CheckRadioButton(IDD_DEBUG_KIND1, IDD_DEBUG_KIND3, IDD_DEBUG_KIND1);						// ”»’è‹@”\
	CWnd::CheckRadioButton(IDC_DEBUG_START_MODE1, IDC_DEBUG_START_MODE4, IDC_DEBUG_START_MODE1);	// ‰^“]ƒ‚[ƒh

	SetDlgItemInt(IDC_DEBUG_CAMPAIR, 1);
	SetDlgItemInt(IDC_DEBUG_CAMSET1, 1);
	SetDlgItemInt(IDC_DEBUG_CAMSET2, 2);
	SetDlgItemInt(IDC_DEBUG_SCODE, MAX_SCODE);

	CWnd::GetDlgItem(IDC_DEBUG_CAMSET1)->EnableWindow(FALSE);
	CWnd::GetDlgItem(IDC_DEBUG_CAMSET2)->EnableWindow(FALSE);
	
	// è“®’Ç‰Á -------------------->>>
	// ‰æ–ÊˆÊ’u (ƒTƒCƒY‚Í‚¢‚Ü‚Ì‚Ü‚Ü)
	::SetWindowPos(m_hWnd, NULL, 400, 600, 0, 0, SWP_NOSIZE | SWP_NOZORDER);

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

void CHT_DEFECTDlg::OnPaint()
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
HCURSOR CHT_DEFECTDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



// ‚±‚êˆÈ~ è“®’Ç‰Á -------------------->>>

//PPPPPPPPPPPPPPPPPPPPP
// ~ƒ{ƒ^ƒ“
//QQQQQQQQQQQQQQQQQQQQQ
void CHT_DEFECTDlg::OnClose()
{
	//CDialog::OnClose();
}

//PPPPPPPPPPPPPPPPPPPPP
// ‹­§I—¹ƒ{ƒ^ƒ“
//QQQQQQQQQQQQQQQQQQQQQ
void CHT_DEFECTDlg::OnBnClickedOk()
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
void CHT_DEFECTDlg::OnLbnDblclkList1()
{
	// ListBoxƒRƒ“ƒgƒ[ƒ‹‚ÌƒvƒƒpƒeƒB‚ÍAƒ\[ƒg‹Ö~‚É‚µ‚Ä‚¨‚­–
	// ListBoxƒRƒ“ƒgƒ[ƒ‹‚Ìƒ_ƒuƒ‹ƒNƒŠƒbƒNƒCƒxƒ“ƒg‚ğ’Ç‰Á
	mcls_pLog->ShowEdit();
}

//------------------------------------------
// ŠeƒvƒƒZƒX‰Šú‰»ˆ—
//------------------------------------------
void CHT_DEFECTDlg::InitProcess()
{
	//// ƒ^ƒXƒN‰Šú‰»ˆ—
	//if( 0 != task_init(XX_CPPSMPEX, 0) ) {		// PCID‚ª‚È‚¢ê‡B
	if( 0 != task_init(HT_DEFECT) ) {
		syslog(SYSNO_TASK_INIT_ERR, "[%s]", HT_DEFECT);
		_ASSERT(0);
		exit(-9);		// ©ƒvƒƒZƒX‹­§I—¹
		return;
	}

	//// LogFileManagerƒCƒ“ƒXƒ^ƒ“ƒX¶¬ (‚±‚êˆÈ~ ƒƒOo—Í‰Â”\)
	setlocale(LC_ALL, "Japanese");	// ‚±‚ê‚ª–³‚¢‚Æ“ú–{ŒêƒpƒX‚ğŠÜ‚ŞƒtƒHƒ‹ƒ_‚Ìê‡A•Û‘¶‚Å‚«‚È‚¢B(ƒ‰ƒCƒuƒ‰ƒŠ‚Ì’†‚ÅÀs‚·‚é‚×‚«‚©–À‚Á‚Ä‚¢‚é‚ªA•ÏX‚·‚é‚Æƒ^ƒXƒN‘S‘Ì‚É‰e‹¿‚·‚é‚Ì‚ÅA‚Æ‚è‚ ‚¦‚¸‚±‚±‚Åİ’è)
	mcls_pLog = new LogFileManager(getmytaskname(), &m_ListBox, 50, 5);
	LOG(em_MSG), "[Dlg] ƒ^ƒXƒN‹N“®");
	LOG(em_MSG), "[Dlg] ŠeƒvƒƒZƒXŠJnˆ— Às’†EEE");

	mcls_pLogCull = new LogFileManager("CULL", NULL);
	LOGCULL(em_MSG), "[Dlg] ƒ^ƒXƒN‹N“®");

	//// ƒƒCƒ“ƒCƒ“ƒXƒ^ƒ“ƒX ŠJnˆ—
	mcls_MI.SetLogMgr(mcls_pLog);
	mcls_MI.SetLogMgrCull(mcls_pLogCull);
	mcls_MI.Init();
	

	//// ‰æ–ÊXV—pƒf[ƒ^ƒZƒbƒg
	//// ƒRƒ“ƒ{ƒ{ƒbƒNƒX‚ÉƒZƒbƒVƒ‡ƒ“–¼’Ç‰Á
	//CString sWk;
	//for(int ii=0; ii<HT_CONNECT_NUM; ii++) {
	//	sWk.Format("%d", ii+1);
	//	m_cmbMen.AddString( sWk );
	//}
	//m_cmbMen.SetCurSel(0);				// 0”Ô–Ú‚ğ‰Šú‘I‘ğ‚Æ‚·‚é


	//-----------------------
	// ó‘Ô ƒŠƒXƒgƒrƒ…[ƒCƒ“ƒXƒ^ƒ“ƒX¶¬ & ’è‹`
	if(true) {
		mcnt_pListSt = new DspListView(&m_ListSt);
		// Œ©‚½–Ú’è‹`
		m_ListSt.SetExtendedStyle(m_ListSt.GetExtendedStyle() | LVS_EX_GRIDLINES );			// Œrü•`‰æ
		m_ListSt.SetExtendedStyle(m_ListSt.GetExtendedStyle() | LVS_EX_FULLROWSELECT );		// 1s‘S‘Ì‘I‘ğ

		// —ñ’Ç‰Á			0		1			
		char name[][32] = {	"€–Ú",	"’l"};
		int  wid[]		= {	60, 	160	};
		mcnt_pListSt->SetColumnAll(sizeof(wid)/sizeof(int), 32, wid, (char const*)name);
		// s’Ç‰Á
		mcnt_pListSt->SetRowAll(8);
		// ŒÅ’è’lƒZƒbƒg
		mcnt_pListSt->SetItem(0, 0, "ƒXƒe[ƒ^ƒX");
		mcnt_pListSt->SetItem(1, 0, "‰^“]ó‘Ô");
		mcnt_pListSt->SetItem(2, 0, "‘ÎÛ–Ê");
		mcnt_pListSt->SetItem(3, 0, "¶Ò×¾¯Ä");
		mcnt_pListSt->SetItem(4, 0, "–¼Ìˆê——");
		mcnt_pListSt->SetItem(5, 0, "”»’è‹@”\");
		mcnt_pListSt->SetItem(6, 0, "•\–Êó‘Ô");
		mcnt_pListSt->SetItem(7, 0, "‰æ‘œŒ`®");
	}

	//-----------------------
	// ‘Î“Š‡ó‘Ô ƒŠƒXƒgƒrƒ…[ƒCƒ“ƒXƒ^ƒ“ƒX¶¬ & ’è‹`
	if (true) {
		mcnt_pListTo = new DspListView(&m_ListTo);
		// Œ©‚½–Ú’è‹`
		m_ListTo.SetExtendedStyle(m_ListTo.GetExtendedStyle() | LVS_EX_GRIDLINES);			// Œrü•`‰æ
		m_ListTo.SetExtendedStyle(m_ListTo.GetExtendedStyle() | LVS_EX_FULLROWSELECT);		// 1s‘S‘Ì‘I‘ğ

		// —ñ’Ç‰Á			0		1			
		char name[][32] = { "€–Ú",	"’l" };
		int  wid[] = { 100, 	120 };
		mcnt_pListTo->SetColumnAll(sizeof(wid) / sizeof(int), 32, wid, (char const*)name);
		// s’Ç‰Á
		mcnt_pListTo->SetRowAll(16);
		// ŒÅ’è’lƒZƒbƒg
		mcnt_pListTo->SetItem(0, 0, "“Š‡‰ñüó‘Ô");
		mcnt_pListTo->SetItem(1, 0, "To·­° Œ»/Å‘å");
		mcnt_pListTo->SetItem(2, 0, "EngóMFrameNo");
		mcnt_pListTo->SetItem(3, 0, "“Š‡‘—MFrameNo");

		mcnt_pListTo->SetItem(4, 0, "óMŠÔŠu[ms]");
		mcnt_pListTo->SetItem(5, 0, "œ‘Sˆ—ŠÔ");
		mcnt_pListTo->SetItem(6, 0, "1ÌÚ°Ñˆ—[ms]");
		mcnt_pListTo->SetItem(7, 0, "‘S’·ŠÖ˜A[ms]");
		mcnt_pListTo->SetItem(8, 0, "“Š‡ŠÖ˜A[ms]");
		mcnt_pListTo->SetItem(9, 0, "ÌÚ°ÑJpeg[ms]");

		mcnt_pListTo->SetItem(11, 0, "1‘S’·‰ñüó‘Ô");
		mcnt_pListTo->SetItem(12, 0, "1‘S’·‹ó‚«/Max");
		mcnt_pListTo->SetItem(13, 0, "2‘S’·‰ñüó‘Ô");
		mcnt_pListTo->SetItem(14, 0, "2‘S’·‹ó‚«/Max");
		mcnt_pListTo->SetItem(15, 0, "‘S’·ŠÖ˜A[ms]");
	}


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
void CHT_DEFECTDlg::ExitProcess()
{
	//// ƒƒCƒ“ƒCƒ“ƒXƒ^ƒ“ƒX I—¹ˆ—
	KillTimer(1);			// ‰æ–ÊXVƒ^ƒCƒ}
	mcls_MI.Stop(20000);
	mcls_MI.Exit();

	//// ƒƒO ƒŠƒXƒgƒ{ƒbƒNƒX•\¦ˆ—I—¹
	mcls_pLog->SetListOut(false);					// ‚±‚êˆÈ~ ƒŠƒXƒgƒ{ƒbƒNƒX‚Ö‚ÌƒƒO•\¦‹Ö~

	//// ƒƒOƒNƒ‰ƒXŠJ•ú
	LOG(em_MSG), "[Dlg] ƒ^ƒXƒNI—¹");
	if(NULL != mcls_pLog){
		delete mcls_pLog;
		mcls_pLog = NULL;
	}
	LOGCULL(em_MSG), "[Dlg] ƒ^ƒXƒNI—¹");
	if(NULL != mcls_pLogCull){
		delete mcls_pLogCull;
		mcls_pLogCull = NULL;
	}
	
	//// ‰æ–ÊƒRƒ“ƒgƒ[ƒ‹ŠJ•ú
	delete mcnt_pListSt;
	delete mcnt_pListTo;


	// ƒ^ƒXƒNI—¹ˆ—
	task_exit();

	// ‰æ–Ê‚ÌI—¹—v‹
	PostMessage(WM_QUIT);
}

//------------------------------------------
// ƒƒCƒ“ƒXƒŒƒbƒh DlgThread
// LPVOID param ƒ_ƒCƒ„ƒƒOƒ|ƒCƒ“ƒ^
//------------------------------------------
unsigned WINAPI CHT_DEFECTDlg::MainThread(void* param)
{
	CHT_DEFECTDlg* p = (CHT_DEFECTDlg *)param;
	COMMON_QUE que;			// óM\‘¢‘Ì

	//// ŠeƒvƒƒZƒX‰Šú‰»ˆ— (‚±‚êˆÈ‘O‚ÍƒƒOo—Í‹Ö~)
	p->InitProcess();

	//// ‹N“®—vˆöæ‚è‚İ ƒ‹[ƒvŠJn
	while(true) {
		recv_mail(&que);			// ‹N“®—vˆöæ‚è‚İ

		switch(que.nEventNo) {

//-----------------------------------------------------------------------------------------------
		case FACT_HT_DEFECT_01:								// ƒpƒ‰ƒ[ƒ^•ÏX’Ê’m (©TO_DEFECT)
			pLOG(em_MSG), "[MAIL] ƒpƒ‰ƒ[ƒ^•ÏX’Ê’m (FACT_HT_DEFECT_01) [dly=%d, scode=%d, id=%d(0:•\,1:— ,2:—¼–Ê), kind=%d(1:nc,2:pd,3:pr)]",
				que.fl.data[0], que.fl.data[2], que.fl.data[1], que.fl.data[3]); 
			
			// ƒƒCƒ“ƒCƒ“ƒXƒ^ƒ“ƒX‚Ö’Ê’m
			p->mcls_MI.SetDeliveryMail( E_DEF_ML_PARAM, &que);
			break;


//-----------------------------------------------------------------------------------------------
		case FACT_HT_DEFECT_02:								// ŒŸ¸ŠJn—v‹ (©TO_DEFECT)
			if(true) {
				int campair = que.fl.data[ 5 + (getpcid() -1)*2 + 0 ]; 
				int kind    = que.fl.data[ 5 + (getpcid() -1)*2 + 1 ]; 
				if(0 == campair) {										// ‘ÎÛŠO‚©ƒ`ƒFƒbƒN
					pLOG(em_MSG), "[MAIL] ŒŸ¸ŠJn—v‹ ‘ÎÛŠOII (FACT_HT_DEFECT_02) [mode=%s, scode=%d:%d, pair=%d, kind=%d(1:nc,2:pd,3:pr)]",
						p->mcls_MI.gcls_Status.GetUntenName((EM_DIV_UNTEN)que.fl.data[0]), que.fl.data[1], que.fl.data[2], campair, kind); 
					break;
				}

				pLOG(em_MSG), "[MAIL] ŒŸ¸ŠJn—v‹ (FACT_HT_DEFECT_02) [mode=%s, scode=%d:%d, pair=%d, kind=%d(1:nc,2:pd,3:pr) len=%d:%d]",
					p->mcls_MI.gcls_Status.GetUntenName((EM_DIV_UNTEN)que.fl.data[0]), que.fl.data[1], que.fl.data[2], campair, kind, que.fl.data[3], que.fl.data[4]); 

				//// ƒƒCƒ“ƒCƒ“ƒXƒ^ƒ“ƒX‚Ö’Ê’m
				p->mcls_MI.SetDeliveryMail( E_DEF_ML_START, &que);
			}
			break;


//-----------------------------------------------------------------------------------------------
		case FACT_HT_DEFECT_03:								// ŒŸ¸’â~—v‹ (©TO_DEFECT)
			if(true) {
				int umu = que.fl.data[ getpcid() -1]; 
				pLOG(em_MSG), "[MAIL] ŒŸ¸’â~—v‹ (FACT_HT_DEFECT_03) [%s]", 0==umu?"‘ÎÛ":"‘ÎÛŠO"); 
				if(0 != umu) break;

				// ƒƒCƒ“ƒCƒ“ƒXƒ^ƒ“ƒX‚Ö’Ê’m
				p->mcls_MI.SetDeliveryMail( E_DEF_ML_STOP, &que);
			}
			break;

//-----------------------------------------------------------------------------------------------
		case FACT_HT_DEFECT_04:								// Jpegˆ³k—¦İ’è (©TO_DEFECT)
			pLOG(em_MSG), "[MAIL] Jpegˆ³k—¦İ’è (FACT_HT_DEFECT_04) [JpegQuality=%d]", que.fl.data[0]); 

			// ƒƒCƒ“ƒCƒ“ƒXƒ^ƒ“ƒX‚Ö’Ê’m
			p->mcls_MI.gcls_pSend->SetJpegQuality(que.fl.data[0]); 
			break;


//-----------------------------------------------------------------------------------------------
		case FACT_HT_DEFECT_05:								// ãˆÊî•ñİ’è—v‹ (©TO_DEFECT)
			pLOG(em_MSG), "[MAIL] ãˆÊî•ñİ’è—v‹ (FACT_HT_DEFECT_05) [dly=%d, size=%d] (%d/%d) scode(%d)", 
				que.fl.data[0], que.fl.data[1], que.fl.data[2], que.fl.data[3], que.fl.data[4]);

			// ƒƒCƒ“ƒCƒ“ƒXƒ^ƒ“ƒX‚Ö’Ê’m
			p->mcls_MI.SetDeliveryMail( E_DEF_ML_COILINF, &que);
			break;


//-----------------------------------------------------------------------------------------------
		case FACT_HT_DEFECT_07:								// ‘±s•s‰Â”\‰ğœ (©SO_GAMENN)
			pLOG(em_MSG), "[MAIL] ‘±s•s‰Â”\‰ğœ (FACT_HT_DEFECT_07)"); 

			// ƒƒCƒ“ƒCƒ“ƒXƒ^ƒ“ƒX‚Ö’Ê’m
			p->mcls_MI.SetDeliveryMail( E_DEF_ML_CANOK, &que);
			break;


//-----------------------------------------------------------------------------------------------
		case FACT_HT_DEFECT_08:								// ó‘Ô–â‚¢‡‚í‚¹ (©TO_DEFECT)
			pLOG(em_MSG), "[MAIL] ó‘Ô–â‚¢‡‚í‚¹ (FACT_HT_DEFECT_08)"); 

			// ƒƒCƒ“ƒCƒ“ƒXƒ^ƒ“ƒX‚Ö’Ê’m
			p->mcls_MI.SetDeliveryMail( E_DEF_ML_STATUS, &que);
			break;

//-----------------------------------------------------------------------------------------------
		case FACT_HT_DEFECT_19:								// –¢Àsİ’è‚ÌƒLƒƒƒ“ƒZƒ‹ˆË—Š (©TO_DEFECT)
			pLOG(em_MSG), "[MAIL] –¢Àsİ’è‚ÌƒLƒƒƒ“ƒZƒ‹ˆË—Š (FACT_HT_DEFECT_19) [–Ê=%d Kind=%d]", que.fl.data[0], que.fl.data[1]);

			// ƒƒCƒ“ƒCƒ“ƒXƒ^ƒ“ƒX‚Ö’Ê’m
			p->mcls_MI.SetDeliveryMail(E_DEF_ML_CLEAR_QUEUE, &que);
			break;

//-----------------------------------------------------------------------------------------------
		case FACT_HT_DEFECT_20:								// “à•”‹——£ƒJƒEƒ“ƒ^İ’èˆË—Š (©TO_DEFECT)
			pLOG(em_MSG), "[MAIL] “à•”‹——£ƒJƒEƒ“ƒ^İ’èˆË—Š (FACT_HT_DEFECT_20) [–Ê=%d Kind=%d]", que.fl.data[0], que.fl.data[1]);

			// ƒƒCƒ“ƒCƒ“ƒXƒ^ƒ“ƒX‚Ö’Ê’m
			p->mcls_MI.SetDeliveryMail( E_DEF_ML_SET_POSITION, &que);
			break;

//-----------------------------------------------------------------------------------------------
		case FACT_HT_DEFECT_21:								// ƒpƒ‰[ƒ[ƒ^–‘O“ÇˆË—Š (©TO_DEFECT)
			pLOG(em_MSG), "[MAIL] ƒpƒ‰[ƒ[ƒ^–‘O“ÇˆË—Š (FACT_HT_DEFECT_21) [–Ê=%d Kind=%d scode=%d]", que.fl.data[0], que.fl.data[1], que.fl.data[2]);  

			// ƒƒCƒ“ƒCƒ“ƒXƒ^ƒ“ƒX‚Ö’Ê’m
			p->mcls_MI.SetDeliveryMail( E_DEF_ML_PRELOAD_PARAM, &que);
			break;


//-----------------------------------------------------------------------------------------------
		case FACT_HT_DEFECT_22:								// “r’†ŠJn—v‹ (©TO_DEFECT)
			if (true)
			{
				int	pcid = (getpcid() - 1);
				int	nStartFrameNo = que.fl.data[5];
				int camset1 = que.fl.data[6 + pcid * 3 + 0];
				int camset2 = que.fl.data[6 + pcid * 3 + 1];
				EM_DIV_HANTEI emKind = (EM_DIV_HANTEI)que.fl.data[6 + pcid * 3 + 2];

				if (DIV_HANTEI_PR == emKind)
				{
					// ƒpƒ‰ƒ‰ƒ“”»’è‚Ì‚İˆ—‘ÎÛ‚Æ‚·‚é
					if (0 != camset1 && 0 != camset2)
					{
						// 0‚Ì‚Ü‚Ü‘—‚é‚Æ‰½ˆ‚É‘Î‚µ‚Ä‚Ì—v‹‚È‚Ì‚©•ª‚©‚ç‚È‚¢‚½‚ßA
						// -1‚Å‘—M‚µA”»’è‘¤‚Å0‚É–ß‚·
						if (-1 == camset1) que.fl.data[6 + pcid * 3 + 0] = 0;
						if (-1 == camset2) que.fl.data[6 + pcid * 3 + 1] = 0;

						pLOG(em_MSG), "[MAIL] “r’†ŠJn—v‹ (FACT_HT_DEFECT_22) [mode=%s, scode=%d:%d, camset1=%d, camset2=%d, frame=%d, kind=%d(1:nc,2:pd,3:pr) len=%d:%d]",
							p->mcls_MI.gcls_Status.GetUntenName((EM_DIV_UNTEN)que.fl.data[0]), que.fl.data[1], que.fl.data[2], camset1, camset2, nStartFrameNo, emKind, que.fl.data[3], que.fl.data[4]);

						// ƒƒCƒ“ƒCƒ“ƒXƒ^ƒ“ƒX‚Ö’Ê’m
						p->mcls_MI.SetDeliveryMail(E_DEF_ML_RESUME_START, &que);
					}
					else
					{
						pLOG(em_MSG), "[MAIL] “r’†ŠJn—v‹ ‘ÎÛŠOII (FACT_HT_DEFECT_22) [mode=%s, scode=%d:%d, camset1=%d, camset2=%d, frame=%d, kind=%d(1:nc,2:pd,3:pr)]",
							p->mcls_MI.gcls_Status.GetUntenName((EM_DIV_UNTEN)que.fl.data[0]), que.fl.data[1], que.fl.data[2], camset1, camset2, nStartFrameNo, emKind);
						break;
					}
				}
			}
			break;

//-----------------------------------------------------------------------------------------------
		case FACT_HT_DEFECT_23:								// “r’†’â~—v‹ (©TO_DEFECT)
			if (true)
			{
				int umu = que.fl.data[getpcid() - 1];
				pLOG(em_MSG), "[MAIL] “r’†’â~—v‹ (FACT_HT_DEFECT_23) [%s]", 0 == umu ? "‘ÎÛ" : "‘ÎÛŠO");
				if (0 != umu) break;

				// ƒƒCƒ“ƒCƒ“ƒXƒ^ƒ“ƒX‚Ö’Ê’m
				p->mcls_MI.SetDeliveryMail(E_DEF_ML_RESUME_STOP, &que);
			}
			break;

//-----------------------------------------------------------------------------------------------
		case FACT_KS_PCNAME_CHANGE:							// PC–¼Ì•ÏX’Ê’m (©KS_SLAVE)
			pLOG(em_MSG), "[MAIL] PC–¼Ì•ÏX’Ê’m (FACT_KS_PCNAME_CHANGE)" );
			// PC–¼Ìæ“¾
			getpcname(); 
			break;

//-----------------------------------------------------------------------------------------------
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
void CHT_DEFECTDlg::OnTimer(UINT_PTR nIDEvent)
{
	CString cwk;
	CString ss;

	////// Œ»İ‘I‘ğ‚³‚ê‚Ä‚¢‚éƒCƒ“ƒXƒ^ƒ“ƒX‚ğæ“¾‚·‚é
	//int idx = m_cmbMen.GetCurSel();			// 0ƒIƒŠƒWƒ“
	//if( -1 == idx) return;					// İ’è–³‚µ
	int idx = 0;

	//// ƒXƒe[ƒ^ƒXî•ñ
	mcnt_pListSt->SetItem(0, 1, mcls_MI.gcls_Status.GetMainStateName());
	mcnt_pListSt->SetItem(1, 1, mcls_MI.gcls_Status.GetUntenName());
	mcnt_pListSt->SetItem(2, 1, DivNameManager::GetTorB(mcls_MI.GetMen()));	// 2   ‘ÎÛ–Ê

	EM_DIV_HANTEI	emKindHt = mcls_MI.GetKindHt();
	ss = DIV_HANTEI_HT == emKindHt ? "”»’è‹@”\" : "üŠú‹@”\";
	ss = (DIV_HANTEI_PR == emKindHt ? "ƒpƒ‰ƒ‰ƒ“‹@”\" : ss);
	mcnt_pListSt->SetItem(5, 1, ss);

	mcnt_pListSt->SetItem(6, 1, mcls_MI.GetNowScode(0));
	
	ss = mcls_MI.gcls_pSend->GetJpegQuality() == 0 ? "BMP" : "JPEGˆ³k";
	mcnt_pListSt->SetItem(7, 1, ss);

	// ƒJƒƒ‰ƒZƒbƒg
	ss = "";
	cwk = "";
	int	nCamPair = mcls_MI.GetCamPair();
	if (0 < nCamPair)
	{
		// ƒpƒ‰ƒ‰ƒ“”»’è‚Ìê‡AƒJƒƒ‰ƒyƒA‚Å‚Í‚È‚­ƒJƒƒ‰ƒZƒbƒg‚Åw¦‚ª—ˆ‚é‚½‚ß
		// nCamPair‚Í©PC‚ª—LŒø‚Å‚ ‚é‚©”Û‚©‚Ì”»’f‚É‚Ì‚İg—p
		// ƒJƒƒ‰ƒZƒbƒg‚Íæ“ªŒÅ’è‚ÅŠi”[‚µ‚Ä‚ ‚é‚Ì‚ÅA‚±‚Ì“_‚Å‚O‚ğƒZƒbƒg‚µ•¶š—ñæ“¾‚·‚é
		if (DIV_HANTEI_PR == emKindHt) nCamPair = 0;

		int	nCamNum = mcls_MI.gcls_Status.GetCamPairItemNum(nCamPair, mcls_MI.GetKindHt());
		for (int ii = 0; ii < nCamNum; ii++)
		{
			int nCamSet = mcls_MI.gcls_Status.GetCamPairItemCamset(nCamPair, ii, emKindHt);
			ss.AppendFormat("%d/ ", nCamSet);
			if (DIV_HANTEI_PR != emKindHt)
			{
				// ’P”­”»’è‚Ìê‡AƒJƒƒ‰ƒZƒbƒg‚O‚ÍƒoƒbƒNƒAƒbƒv‚ğˆÓ–¡‚·‚é
				cwk.AppendFormat("%s/ ", DivNameManager::GetDivCamSet(nCamSet));
			}
			else
			{
				// ƒpƒ‰ƒ‰ƒ“”»’è‚Ìê‡AƒJƒƒ‰ƒZƒbƒg‚O‚Í–³ŒøƒJƒƒ‰‚Ì‘I‘ğ‚ğˆÓ–¡‚·‚é
				cwk.AppendFormat("%s/ ", (0 != nCamSet ? DivNameManager::GetDivCamSet(nCamSet) : "‘ÎÛŠO"));
			}
		}
	}
	mcnt_pListSt->SetItem(3, 1, ss);
	mcnt_pListSt->SetItem(4, 1, cwk);


	//// ˆ—ó‘Ô
	mcnt_pListTo->SetItem(0, 1, mcls_MI.gcls_pSock[idx]->GetSockStateName());
	ss.Format("%d (%d)", mcls_MI.gcls_pSock[idx]->gque_Deli.GetCount(), mcls_MI.gcls_pSock[idx]->gque_Deli.GetMaxCount());	mcnt_pListTo->SetItem(1, 1, ss);
	mcnt_pListTo->SetItem(2, 1, mcls_MI.gcls_pSend->GetDspRecvFrameNo());
	mcnt_pListTo->SetItem(3, 1, mcls_MI.gcls_pSend->GetDspFrameNo());

	mcnt_pListTo->SetItem(4, 1, mcls_MI.gcls_pSend->GetTimeIntval());
	ss.Format("%d (%d)", mcls_MI.gcls_pSend->GetTimeAllNow(), mcls_MI.gcls_pSend->GetTimeAllMax());	mcnt_pListTo->SetItem(5, 1, ss);
	ss.Format("%d (%d)", mcls_MI.gcls_pSend->g_nTime_All[0], mcls_MI.gcls_pSend->g_nTime_All[1]);	mcnt_pListTo->SetItem(6, 1, ss);
	ss.Format("%d (%d)", mcls_MI.gcls_pSend->g_nTime_Rpi[0], mcls_MI.gcls_pSend->g_nTime_Rpi[1]);	mcnt_pListTo->SetItem(7, 1, ss);
	ss.Format("%d (%d)", mcls_MI.gcls_pSend->g_nTime_To[0], mcls_MI.gcls_pSend->g_nTime_To[1]);	mcnt_pListTo->SetItem(8, 1, ss);
	ss.Format("%d (%d)", mcls_MI.gcls_pSend->g_nTime_ToFrame[0], mcls_MI.gcls_pSend->g_nTime_ToFrame[1]);	mcnt_pListTo->SetItem(9, 1, ss);

#ifdef FRAME_RAW_SEND
	for (int ii=0; ii<FRAME_HT_SEND_PORT; ii++) {
		mcnt_pListTo->SetItem(11+2*ii, 1, mcls_MI.gcls_pSockRpi[ii]->GetSockStateName());
		ss.Format("%d (%d)", mcls_MI.gque_pSockRpi[ii]->GetCountFree(), mcls_MI.gque_pSockRpi[ii]->GetMaxCountPool());	mcnt_pListTo->SetItem(12+ii*2, 1, ss);
	}

	double dAve = (mcls_MI.gcls_pCull[0]->g_nTime_Rpi[0] + mcls_MI.gcls_pCull[1]->g_nTime_Rpi[0]) / 2;
	double dMax = max(mcls_MI.gcls_pCull[0]->g_nTime_Rpi[1], mcls_MI.gcls_pCull[1]->g_nTime_Rpi[1]);
	ss.Format("%.0f (%.0f)", dAve, dMax);	mcnt_pListTo->SetItem(15, 1, ss);
#endif



	CDialog::OnTimer(nIDEvent);
}

// ‚±‚êˆÈ~ ƒfƒoƒbƒN—pƒRƒ}ƒ“ƒh -------------------->>>
//PPPPPPPPPPPPPPPPPPPPP
// ŒŸ¸ŠJn
//QQQQQQQQQQQQQQQQQQQQQ
void CHT_DEFECTDlg::OnBnClickedDebugStart()
{
	if ( BST_CHECKED != CWnd::IsDlgButtonChecked(IDC_DEBUG_MANUAL) ) return;

	int pcid = getpcid() -1;

	EM_DIV_UNTEN emMode = DIV_UNTEN_NOMAL;
	if ( BST_CHECKED == CWnd::IsDlgButtonChecked( IDC_DEBUG_START_MODE1 ) ) emMode = DIV_UNTEN_NOMAL;
	if ( BST_CHECKED == CWnd::IsDlgButtonChecked( IDC_DEBUG_START_MODE2 ) ) emMode = DIV_UNTEN_SAMP;
	if ( BST_CHECKED == CWnd::IsDlgButtonChecked( IDC_DEBUG_START_MODE3 ) ) emMode = DIV_UNTEN_CAMERA;
	if ( BST_CHECKED == CWnd::IsDlgButtonChecked( IDC_DEBUG_START_MODE4 ) ) emMode = DIV_UNTEN_SMYU;

	EM_DIV_HANTEI emKind= DIV_HANTEI_BK;
	if ( BST_CHECKED == CWnd::IsDlgButtonChecked( IDD_DEBUG_KIND1 ) ) emKind = DIV_HANTEI_HT;
	if ( BST_CHECKED == CWnd::IsDlgButtonChecked( IDD_DEBUG_KIND2 ) ) emKind = DIV_HANTEI_CY;
	if ( BST_CHECKED == CWnd::IsDlgButtonChecked( IDD_DEBUG_KIND3 ) ) emKind = DIV_HANTEI_PR;

	COMMON_QUE	que;
	que.nEventNo		= FACT_HT_DEFECT_02;
	que.nLineNo			= getlineid();
	que.fl.data[0]		= (int)emMode;
	que.fl.data[1]		= GetDlgItemInt(IDC_DEBUG_SCODE);
	que.fl.data[2]		= GetDlgItemInt(IDC_DEBUG_SCODE);
	que.fl.data[3]		= 0;
	que.fl.data[4]		= 0;

	que.fl.data[5+ 2*pcid + 0] = GetDlgItemInt(IDC_DEBUG_CAMPAIR);	// ©•ª‚Ì‚Æ‚±‚ë‚¾‚¯ƒZƒbƒg
	que.fl.data[5+ 2*pcid + 1] = (int)emKind;

	send_mail(HT_DEFECT, ".", &que);
}
//PPPPPPPPPPPPPPPPPPPPP
// ŒŸ¸’â~
//QQQQQQQQQQQQQQQQQQQQQ
void CHT_DEFECTDlg::OnBnClickedDebugStop()
{
	if ( BST_CHECKED != CWnd::IsDlgButtonChecked(IDC_DEBUG_MANUAL) ) return;
	COMMON_QUE que;
	memset(&que, 0x00, sizeof(que));
	que.nEventNo = FACT_HT_DEFECT_03;
	que.nLineNo = getlineid();

	send_mail(HT_DEFECT, ".", &que);
}
//PPPPPPPPPPPPPPPPPPPPP
// ƒpƒ‰ƒ[ƒ^•ÏX
//QQQQQQQQQQQQQQQQQQQQQ
void CHT_DEFECTDlg::OnBnClickedDebugParam()
{
	if ( BST_CHECKED != CWnd::IsDlgButtonChecked(IDC_DEBUG_MANUAL) ) return;
	COMMON_QUE que;
	que.nEventNo = FACT_HT_DEFECT_01;
	que.nLineNo = getlineid();
	que.fl.data[0] = 0;						// ’x‰„Às–³‚µ
	que.fl.data[1] = 2;						// —¼–Ê
	que.fl.data[2] = CH_SCODE_ALL;			// ‘S•”

	EM_DIV_HANTEI emKind= DIV_HANTEI_BK;
	if ( BST_CHECKED == CWnd::IsDlgButtonChecked( IDD_DEBUG_KIND1 ) ) emKind = DIV_HANTEI_HT;
	if ( BST_CHECKED == CWnd::IsDlgButtonChecked( IDD_DEBUG_KIND2 ) ) emKind = DIV_HANTEI_CY;
	if ( BST_CHECKED == CWnd::IsDlgButtonChecked( IDD_DEBUG_KIND3 ) ) emKind = DIV_HANTEI_PR;
	que.fl.data[3]	= (int)emKind;

	send_mail(HT_DEFECT, ".", &que);	
}

//PPPPPPPPPPPPPPPPPPPPP
// ãˆÊî•ñİ’è—v‹
//QQQQQQQQQQQQQQQQQQQQQ
void CHT_DEFECTDlg::OnBnClickedDebugCoilinf()
{
	if ( BST_CHECKED != CWnd::IsDlgButtonChecked(IDC_DEBUG_MANUAL) ) return;
	COMMON_QUE que;
	que.nEventNo = FACT_HT_DEFECT_05;
	que.nLineNo = getlineid();

	TYPE_EPC_COIL wk;
	memset(&wk, 0x00, sizeof(wk));
	// ‹[—ƒf[ƒ^ƒZƒbƒg
	memcpy(wk.cCoilNo, "1234567890", PRO_SIZE_COIL_NO);
	memcpy(wk.cMeiNo, "123 ", PRO_SIZE_ORDER_NO);

	wk.nAtuSei = 1100;
	wk.nWidZai = 1000;
	wk.nLenIn = 3000*1000;

	// ƒZƒbƒgŠ®—¹
	int nCnt = GET_JUST_INDEX( sizeof(TYPE_EPC_COIL) , DIVCOIL_ONE_SIZE_SEND) + 1;
	for (int ii = 0; ii < nCnt; ii++) {
		int nSize = sizeof(TYPE_EPC_COIL) - (DIVCOIL_ONE_SIZE_SEND*ii);
		if (nSize>DIVCOIL_ONE_SIZE_SEND) nSize = DIVCOIL_ONE_SIZE_SEND;
		
		que.fl.data[0] = 0;				// 0ŒÅ’èB
		que.fl.data[1] = nSize;
		que.fl.data[2] = ii+1;
		que.fl.data[3] = nCnt;
		for(int jj=0; jj<NUM_MEN; jj++) que.fl.data[4+jj] = MAX_SCODE;
		memcpy(&que.fl.data[4+NUM_MEN+1], &((BYTE*)&wk)[ii*DIVCOIL_ONE_SIZE_SEND], nSize);
		send_mail(HT_DEFECT, ".", &que);
	}
}

//PPPPPPPPPPPPPPPPPPPPP
// ’x‰„ÀsƒLƒƒƒ“ƒZƒ‹
//QQQQQQQQQQQQQQQQQQQQQ
//void CHT_DEFECTDlg::OnBnClickedDebugDlycancel()
//{
//	if ( BST_CHECKED != CWnd::IsDlgButtonChecked(IDC_DEBUG_MANUAL) ) return;
//	COMMON_QUE que;
//	que.nEventNo = FACT_HT_DEFECT_06;
//	que.nLineNo = getlineid();
//	send_mail(HT_DEFECT, ".", &que);
//}
//PPPPPPPPPPPPPPPPPPPPP
// ‘±s•s‰Â”\‰ğœ
//QQQQQQQQQQQQQQQQQQQQQ
void CHT_DEFECTDlg::OnBnClickedDebugCanok()
{
	if ( BST_CHECKED != CWnd::IsDlgButtonChecked(IDC_DEBUG_MANUAL) ) return;
	COMMON_QUE que;
	que.nEventNo = FACT_HT_DEFECT_07;
	que.nLineNo = getlineid();
	send_mail(HT_DEFECT, ".", &que);
}
//PPPPPPPPPPPPPPPPPPPPP
// ƒtƒŒ[ƒ€‰æ‘œæ“¾—v‹
//QQQQQQQQQQQQQQQQQQQQQ
//void CHT_DEFECTDlg::OnBnClickedDebugFrame()
//{
//	if ( BST_CHECKED != CWnd::IsDlgButtonChecked(IDC_DEBUG_MANUAL) ) return;
//	COMMON_QUE que;
//	que.nEventNo	= FACT_HT_DEFECT_09;
//	que.nLineNo		= getlineid();
//	que.fl.data[0]	= 0;
//	que.fl.data[1]	= GetDlgItemInt(IDC_DEBUG_CAMPAIR);
//	que.fl.data[2]	= 2;
//	send_mail(HT_DEFECT, ".", &que);
//}

//PPPPPPPPPPPPPPPPPPPPP
// Œ’‘S«•]‰¿ƒf[ƒ^ûW—v‹
//QQQQQQQQQQQQQQQQQQQQQ
//void CHT_DEFECTDlg::OnBnClickedDebugProfile()
//{
//	if ( BST_CHECKED != CWnd::IsDlgButtonChecked(IDC_DEBUG_MANUAL) ) return;
//	COMMON_QUE que;
//	que.nEventNo	= FACT_HT_DEFECT_11;
//	que.nLineNo		= getlineid();
//	int pcid = getpcid() -1;
//	que.fl.data[pcid]	= 0;
//	send_mail(HT_DEFECT, ".", &que);
//}


//PPPPPPPPPPPPPPPPPPPPP
// ‰æ‘œˆ³k
//QQQQQQQQQQQQQQQQQQQQQ
void CHT_DEFECTDlg::OnBnClickedDebugImgkind()
{
	if ( BST_CHECKED != CWnd::IsDlgButtonChecked(IDC_DEBUG_MANUAL) ) return;
	COMMON_QUE que;
	que.nEventNo	= FACT_HT_DEFECT_04;
	que.nLineNo		= getlineid();
	que.fl.data[0]	= GetDlgItemInt(IDC_DEBUG_JPEGQUALITY);
	send_mail(HT_DEFECT, ".", &que);
}


//PPPPPPPPPPPPPPPPPPPPP
// “à•”‹——£ƒJƒEƒ“ƒ^İ’è
//QQQQQQQQQQQQQQQQQQQQQ
void CHT_DEFECTDlg::OnBnClickedDebugSetPosition()
{
	if ( BST_CHECKED != CWnd::IsDlgButtonChecked(IDC_DEBUG_MANUAL) ) return;
	COMMON_QUE que;
	que.nEventNo	= FACT_HT_DEFECT_20;
	que.nLineNo		= getlineid();
	que.fl.data[0]	= 2;
	que.fl.data[1]	= -1;

	NCL_CMD_SET_POSITION& wk = (NCL_CMD_SET_POSITION&)que.fl.data[2];
	memset(&wk, 0x00, sizeof(NCL_CMD_SET_POSITION));

	wk.cmd = NCL_SET_POSITION;
	wk.framenum = 0;
	wk.update_mask = 0xFFFFFFFF;
	wk.front_pos = 1000;

	send_mail(HT_DEFECT, ".", &que);
}

//PPPPPPPPPPPPPPPPPPPPP
// ƒpƒ‰ƒ[ƒ^–‘O“Ç
//QQQQQQQQQQQQQQQQQQQQQ
void CHT_DEFECTDlg::OnBnClickedDebugParamPreload()
{
	if ( BST_CHECKED != CWnd::IsDlgButtonChecked(IDC_DEBUG_MANUAL) ) return;
	COMMON_QUE que;
	que.nEventNo	= FACT_HT_DEFECT_21;
	que.nLineNo		= getlineid();
	que.fl.data[0]	= 2;
	que.fl.data[1]	= -1;
	que.fl.data[2]	= 0;
	send_mail(HT_DEFECT, ".", &que);
}

//PPPPPPPPPPPPPPPPPPPPP
// ”»’è‹æ•ª‚Ì•ÏXi’P”­j
//QQQQQQQQQQQQQQQQQQQQQ
void CHT_DEFECTDlg::OnBnClickedDebugKind1()
{
	CWnd::GetDlgItem(IDC_DEBUG_CAMPAIR)->EnableWindow(TRUE);
	CWnd::GetDlgItem(IDC_DEBUG_CAMSET1)->EnableWindow(FALSE);
	CWnd::GetDlgItem(IDC_DEBUG_CAMSET2)->EnableWindow(FALSE);
}

//PPPPPPPPPPPPPPPPPPPPP
// ”»’è‹æ•ª‚Ì•ÏXiüŠúj
//QQQQQQQQQQQQQQQQQQQQQ
void CHT_DEFECTDlg::OnBnClickedDebugKind2()
{
	CWnd::GetDlgItem(IDC_DEBUG_CAMPAIR)->EnableWindow(TRUE);
	CWnd::GetDlgItem(IDC_DEBUG_CAMSET1)->EnableWindow(FALSE);
	CWnd::GetDlgItem(IDC_DEBUG_CAMSET2)->EnableWindow(FALSE);
}

//PPPPPPPPPPPPPPPPPPPPP
// ”»’è‹æ•ª‚Ì•ÏXiƒpƒ‰ƒ‰ƒ“j
//QQQQQQQQQQQQQQQQQQQQQ
void CHT_DEFECTDlg::OnBnClickedDebugKind3()
{
	CWnd::GetDlgItem(IDC_DEBUG_CAMPAIR)->EnableWindow(FALSE);
	CWnd::GetDlgItem(IDC_DEBUG_CAMSET1)->EnableWindow(TRUE);
	CWnd::GetDlgItem(IDC_DEBUG_CAMSET2)->EnableWindow(TRUE);
}

//PPPPPPPPPPPPPPPPPPPPP
// “r’†ŠJn
//QQQQQQQQQQQQQQQQQQQQQ
void CHT_DEFECTDlg::OnBnClickedDebugResumeStart()
{
	if (BST_CHECKED != CWnd::IsDlgButtonChecked(IDC_DEBUG_MANUAL)) return;

	int pcid = getpcid() - 1;

	EM_DIV_UNTEN emMode = DIV_UNTEN_NOMAL;
	if (BST_CHECKED == CWnd::IsDlgButtonChecked(IDC_DEBUG_START_MODE1)) emMode = DIV_UNTEN_NOMAL;
	if (BST_CHECKED == CWnd::IsDlgButtonChecked(IDC_DEBUG_START_MODE2)) emMode = DIV_UNTEN_SAMP;
	if (BST_CHECKED == CWnd::IsDlgButtonChecked(IDC_DEBUG_START_MODE3)) emMode = DIV_UNTEN_CAMERA;
	if (BST_CHECKED == CWnd::IsDlgButtonChecked(IDC_DEBUG_START_MODE4)) emMode = DIV_UNTEN_SMYU;

	EM_DIV_HANTEI emKind = DIV_HANTEI_BK;
	if (BST_CHECKED == CWnd::IsDlgButtonChecked(IDD_DEBUG_KIND1)) emKind = DIV_HANTEI_HT;
	if (BST_CHECKED == CWnd::IsDlgButtonChecked(IDD_DEBUG_KIND2)) emKind = DIV_HANTEI_CY;
	if (BST_CHECKED == CWnd::IsDlgButtonChecked(IDD_DEBUG_KIND3)) emKind = DIV_HANTEI_PR;

	COMMON_QUE	que;
	que.nEventNo = FACT_HT_DEFECT_22;
	que.nLineNo = getlineid();
	que.fl.data[0] = (int)emMode;
	que.fl.data[1] = GetDlgItemInt(IDC_DEBUG_SCODE);
	que.fl.data[2] = GetDlgItemInt(IDC_DEBUG_SCODE);
	que.fl.data[3] = 0;
	que.fl.data[4] = 0;
	que.fl.data[5] = NUM_JOIN_FRAME;
	
	if (DIV_HANTEI_PR != emKind)
	{
		// ’P”­EüŠú
		que.fl.data[6 + 3 * pcid + 0] = GetDlgItemInt(IDC_DEBUG_CAMPAIR);
		que.fl.data[6 + 3 * pcid + 1] = 0;
	}
	else
	{
		// ƒƒWƒbƒNƒpƒ‰ƒ‰ƒ“
		que.fl.data[6 + 3 * pcid + 0] = GetDlgItemInt(IDC_DEBUG_CAMSET1);
		que.fl.data[6 + 3 * pcid + 1] = GetDlgItemInt(IDC_DEBUG_CAMSET2);
	}
	que.fl.data[6 + 3 * pcid + 2] = (int)emKind;

	send_mail(HT_DEFECT, ".", &que);
}

//PPPPPPPPPPPPPPPPPPPPP
// “r’†’â~
//QQQQQQQQQQQQQQQQQQQQQ
void CHT_DEFECTDlg::OnBnClickedDebugResumeStop()
{
	if (BST_CHECKED != CWnd::IsDlgButtonChecked(IDC_DEBUG_MANUAL)) return;
	COMMON_QUE que;
	memset(&que, 0x00, sizeof(que));
	que.nEventNo = FACT_HT_DEFECT_23;
	que.nLineNo = getlineid();

	send_mail(HT_DEFECT, ".", &que);
}
