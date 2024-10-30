
// TO_DEFECTDlg.cpp : �����t�@�C��
//

#include "stdafx.h"
#include "TO_DEFECT.h"
#include "TO_DEFECTDlg.h"
#include "afxdialogex.h"
#include "LogicParaRunFunc.h"											// ���W�b�N�p�������֘A�������W���[��

//#ifdef _DEBUG
//#define new DEBUG_NEW
//#endif
// �������[���[�N���o �蓮�ǉ� ----->>>
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif
// <<<------------------------


// CTO_DEFECTDlg �_�C�A���O



CTO_DEFECTDlg::CTO_DEFECTDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_TO_DEFECT_DIALOG, pParent)
	, m_nParaKind(0)
	, m_nCamsetResult(0)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CTO_DEFECTDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, m_ListBox);
	DDX_Control(pDX, IDC_LIST_ST, m_ListSt);
	DDX_Control(pDX, IDC_LIST_SYS, m_ListSys);
	DDX_Control(pDX, IDC_LIST_HT, m_ListHt);
	DDX_Control(pDX, IDC_LIST_COIL, m_ListCoil);
	DDX_Control(pDX, IDC_LIST_PLG_KEN, m_ListPlgKen);
	DDX_Control(pDX, IDC_LIST_PLG_DSP, m_ListPlgDsp);
	DDX_Control(pDX, IDC_LIST_PLG, m_ListPlg);
	DDX_Control(pDX, IDC_LIST_DB, m_ListDb);
	DDX_Control(pDX, IDC_LIST_DBINFO, m_ListDbInfo);
	DDX_Control(pDX, IDC_LIST_OP, m_ListOp);
	DDX_Control(pDX, IDC_LIST_EDGE, m_ListEdge);
	DDX_Control(pDX, IDC_LIST_KIKI, m_ListKiki);
	DDX_Control(pDX, IDC_LIST_SETU_TR, m_ListSetuDsp);
	DDX_Control(pDX, IDC_LIST_HTTIME, m_ListHtTime);
	DDX_Control(pDX, IDC_LIST_RAMP, m_ListRamp);
	DDX_Control(pDX, IDC_LIST_PATO, m_ListPato);
	DDX_Control(pDX, IDC_LIST_VOICE, m_ListVoice);
	DDX_Control(pDX, IDC_LIST_PR, m_ListPr);
	DDX_Radio(pDX, IDC_BTN_PARA1, m_nParaKind);
	DDX_Radio(pDX, IDC_BTN_CAMSET_RESULT1, m_nCamsetResult);
}

BEGIN_MESSAGE_MAP(CTO_DEFECTDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDOK, &CTO_DEFECTDlg::OnBnClickedOk)
	ON_WM_TIMER()
	ON_LBN_DBLCLK(IDC_LIST1, &CTO_DEFECTDlg::OnLbnDblclkList1)
	ON_NOTIFY(NM_CLICK, IDC_LIST_CMD, &CTO_DEFECTDlg::OnNMClickListCmd)
	ON_BN_CLICKED(IDC_SMYU_PIO, &CTO_DEFECTDlg::OnBnClickedSmyuPio)
	ON_BN_CLICKED(IDC_SMYU_PLG, &CTO_DEFECTDlg::OnBnClickedSmyuPlg)
	ON_BN_CLICKED(IDC_DEBUG_LEDONOFF, &CTO_DEFECTDlg::OnBnClickedDebugLedonoff)
	ON_NOTIFY(NM_CLICK, IDC_LIST_INPUT, &CTO_DEFECTDlg::OnNMClickListInput)
END_MESSAGE_MAP()


// CTO_DEFECTDlg ���b�Z�[�W �n���h���[

BOOL CTO_DEFECTDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// ���̃_�C�A���O�̃A�C�R����ݒ肵�܂��B�A�v���P�[�V�����̃��C�� �E�B���h�E���_�C�A���O�łȂ��ꍇ�A
	//  Framework �́A���̐ݒ�������I�ɍs���܂��B
	SetIcon(m_hIcon, TRUE);			// �傫���A�C�R���̐ݒ�
	SetIcon(m_hIcon, FALSE);		// �������A�C�R���̐ݒ�

	// TODO: �������������ɒǉ����܂��B
	
// ��ʏ������Z�b�g----------------------------------------->>>
#ifdef LOCAL
	CWnd::CheckDlgButton(IDC_DEBUG_MANUAL, 1);
#else
	CWnd::CheckDlgButton(IDC_DEBUG_MANUAL, 1);
#endif
	//// �f�o�b�O���ځ@�����ݒ�
	CString ss;

	CWnd::CheckRadioButton(IDC_RAD_DEBUG_MEN1, IDC_RAD_DEBUG_MEN2, IDC_RAD_DEBUG_MEN1);			// �Жʌ����s����
	CWnd::CheckRadioButton(IDC_RAD_DEBUG_UNTEN1, IDC_RAD_DEBUG_UNTEN4, IDC_RAD_DEBUG_UNTEN1);	// �ʏ�^�]���[�h
	CWnd::CheckRadioButton(IDC_RAD_DEBUG_PLG1, IDC_RAD_DEBUG_PLG4, IDC_RAD_DEBUG_PLG1);			// ���C��PLG

	ss.Format("%d", MAX_SCODE);
	CWnd::SetDlgItemTextA(IDC_DEF_SCODE1, ss);
	CWnd::SetDlgItemTextA(IDC_DEF_SCODE2, ss);

	ss.Format("%d", MAX_PCODE);
	CWnd::SetDlgItemTextA(IDC_DEF_PCODE1, ss);
	CWnd::SetDlgItemTextA(IDC_DEF_PCODE2, ss);

	//-----------------------
	// �R�}���h �{�^���`���̃��X�g�r���[
	if (true) {
		// ��`
		mlst_Cmd.SubclassWindow(*GetDlgItem(IDC_LIST_CMD));
		mcnt_pListCmd = new DspListView(&mlst_Cmd);
		// �����ڒ�`
		//	mlst_Cmd.SetExtendedStyle( mlst_Cmd.GetExtendedStyle() | LVS_EX_GRIDLINES );			// �r���`��
		//	mlst_Cmd.SetExtendedStyle( mlst_Cmd.GetExtendedStyle() | LVS_EX_FULLROWSELECT );		// 1�s�S�̑I��
		// ��ǉ�			0						
		char name[][32] = { "����(�د�����)" };
		int  wid[] = { 148 };
		mcnt_pListCmd->SetColumnAll(sizeof(wid) / sizeof(int), 32, wid, (char const*)name);

		// �s���� (=�{�^������)
		int nHeight = 16;
		CImageList image;
		image.Create(1, nHeight, ILC_COLOR, 0, 0);
		mlst_Cmd.SetImageList(&image, LVSIL_STATE);

		// �{�^������
		for (int ii = 0; ii<E_DEBUG_CMD_END; ii++) {
			mlst_Cmd.InsertItem(ii, DebugCmd[ii]);
		}
		mlst_Cmd.SyncButtons(0);			// 0��ڌŒ�		

	}

	//-----------------------
	// �R�}���h �{�^���`���̃��X�g�r���[
	if (true) {
		// ��`
		mlst_Input.SubclassWindow(*GetDlgItem(IDC_LIST_INPUT));
		mcnt_pListInput = new DspListView(&mlst_Input);
		// �����ڒ�`
		//mlst_Input.SetExtendedStyle( mlst_Input.GetExtendedStyle() | LVS_EX_GRIDLINES );			// �r���`��
		//mlst_Input.SetExtendedStyle( mlst_Input.GetExtendedStyle() | LVS_EX_FULLROWSELECT );		// 1�s�S�̑I��
		// ��ǉ�			0						
		char name[][32] = { "����(�د�����)" };
		int  wid[] = { 148 };
		mcnt_pListInput->SetColumnAll(sizeof(wid) / sizeof(int), 32, wid, (char const*)name);

		// �s���� (=�{�^������)
		int nHeight = 16;
		CImageList image;
		image.Create(1, nHeight, ILC_COLOR, 0, 0);
		mlst_Input.SetImageList(&image, LVSIL_STATE);

		// �{�^������
		for (int ii = 0; ii<E_DEBUG_INPUT_END; ii++) {
			mlst_Input.InsertItem(ii, DebugInput[ii]);
		}

		mlst_Input.SyncButtons(0);			// 0��ڌŒ�		

		m_bInit = false;
	}

	// <<<---------------------------------------------------------

// �蓮�ǉ� -------------------->>>
	// ��ʈʒu (�T�C�Y�͂��܂̂܂�)
	//::SetWindowPos(m_hWnd, NULL, 1, 1, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
	
	// ���s�����ʗp
	CString		cTitle;
	GetWindowText(cTitle);
#ifdef _M_X64			// x64 �R���p�C��
	cTitle += " x64";
#else					// x86 �R���p�C��
	cTitle += " x86";
#endif
	SetWindowText(cTitle);

	// �~�{�^������
	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL) pSysMenu->EnableMenuItem(SC_CLOSE, MF_GRAYED);
	
	// ���C���X���b�h�J�n
	unsigned int tid;	// �X���b�hID
	_beginthreadex(NULL, 0, MainThread, (LPVOID)this, 0, &tid);
// <<<-------------------------------

	return TRUE;  // �t�H�[�J�X���R���g���[���ɐݒ肵���ꍇ�������ATRUE ��Ԃ��܂��B
}

// �_�C�A���O�ɍŏ����{�^����ǉ�����ꍇ�A�A�C�R����`�悷�邽�߂�
//  ���̃R�[�h���K�v�ł��B�h�L�������g/�r���[ ���f�����g�� MFC �A�v���P�[�V�����̏ꍇ�A
//  ����́AFramework �ɂ���Ď����I�ɐݒ肳��܂��B

void CTO_DEFECTDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // �`��̃f�o�C�X �R���e�L�X�g

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// �N���C�A���g�̎l�p�`�̈���̒���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// �A�C�R���̕`��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// ���[�U�[���ŏ��������E�B���h�E���h���b�O���Ă���Ƃ��ɕ\������J�[�\�����擾���邽�߂ɁA
//  �V�X�e�������̊֐����Ăяo���܂��B
HCURSOR CTO_DEFECTDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



//�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P
// �~�{�^��
//�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q
void CTO_DEFECTDlg::OnClose()
{
	//CDialogEx::OnClose();
}

//�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P
// �����I���{�^��
//�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q
void CTO_DEFECTDlg::OnBnClickedOk()
{
	if (IDYES == MessageBox("�I�����܂����H", "�d�v�Ȃ��m�点", MB_YESNO | MB_DEFBUTTON2 | MB_ICONQUESTION)) {
		LOG(em_MSG), "[Dlg] �����I���{�^��");
		COMMON_QUE que;
		que.nEventNo = FACT_KS_END;
		que.nLineNo = LINE_ID;
		send_mail(getmytaskname(), ".", &que);
	}
}

//�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P
// �������N��
//�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q
void CTO_DEFECTDlg::OnLbnDblclkList1()
{
	// ListBox�R���g���[���̃v���p�e�B�́A�\�[�g�֎~�ɂ��Ă�����
	// ListBox�R���g���[���̃_�u���N���b�N�C�x���g��ǉ�
	mcls_pLog->ShowEdit();
}

//------------------------------------------
// �e�v���Z�X����������
//------------------------------------------
void CTO_DEFECTDlg::InitProcess()
{
	CString sWk;

	//// ��ԏ��߂ɂ��낢�돉����
	mcls_pLog = NULL;
	mcls_pLogSeq = NULL;
	mcls_pLogE = NULL;
	mcls_pLogG = NULL;
	mcls_pLogD = NULL;
	mcls_pLogA = NULL;

	//// �^�X�N����������
	int nRetc = task_init(TO_DEFECT);
	if (0 != nRetc) {
		if (-1 == nRetc) { syslog(SYSNO_TASK_INIT_ERR, "[%s] ���Ƀ^�X�N�N���ς݂ɂ��A�N�����f�B", TO_DEFECT); }
		else { syslog(SYSNO_TASK_INIT_ERR, "[%s]", TO_DEFECT); }
		_ASSERT(0);
		exit(-9);		// ���v���Z�X�����I��
		return;
	}

	//// LogFileManager�C���X�^���X���� (����ȍ~ ���O�o�͉\)
	setlocale(LC_ALL, "Japanese");	// ���ꂪ�����Ɠ��{��p�X���܂ރt�H���_�̏ꍇ�A�ۑ��ł��Ȃ��B(���C�u�����̒��Ŏ��s����ׂ��������Ă��邪�A�ύX����ƃ^�X�N�S�̂ɉe������̂ŁA�Ƃ肠���������Őݒ�)
	mcls_pLog = new LogFileManager(getmytaskname(), &m_ListBox);
	LOG(em_MSG), "[Dlg] �^�X�N�N��");
	LOG(em_MSG), "[Dlg] �e�v���Z�X�J�n���� ���s���E�E�E");

	//// ��ʂɏo���Ȃ����O�֌W
	mcls_pLogSeq = new LogFileManager("SeqUDP��M", NULL, 5, 5);
	LOGSEQ(em_MSG), "[Dlg] �^�X�N�N��");

	mcls_pLogE = new LogFileManager("EdgeUDP", NULL);
	LOGE(em_MSG), "[Dlg] �^�X�N�N��");

	mcls_pLogG = new LogFileManager("GuidanceLog", NULL);
	LOGG(em_MSG), "[Dlg] �^�X�N�N��");

	mcls_pLogD = new LogFileManager("HtCheckD", NULL);
	LOGD(em_MSG), "[Dlg] �^�X�N�N��");

	mcls_pLogA = new LogFileManager("AlarmLog", NULL);
	LOGA(em_MSG), "[Dlg] �^�X�N�N��");


	//-----------------------
	// �X�e�[�^�X��� ���X�g�r���[�C���X�^���X���� & ��`
	if (true) {
		mcnt_pListSt = new DspListView(&m_ListSt);
		// �����ڒ�`
		m_ListSt.SetExtendedStyle(m_ListSt.GetExtendedStyle() | LVS_EX_GRIDLINES);			// �r���`��
		m_ListSt.SetExtendedStyle(m_ListSt.GetExtendedStyle() | LVS_EX_FULLROWSELECT);		// 1�s�S�̑I��

		// ��ǉ�			0		1			
		char name[][32] = { "����",	"�l" };
		int  wid[] = { 70, 	160 };
		mcnt_pListSt->SetColumnAll(sizeof(wid) / sizeof(int), 32, wid, (char const*)name);
		// �s�ǉ�
		mcnt_pListSt->SetRowAll(6);
		// �Œ�l�Z�b�g
		mcnt_pListSt->SetItem(0, 0, "�ғ����");
		mcnt_pListSt->SetItem(1, 0, "���Ə�Ԍ�");
#ifdef DSP_SOGYO_CHECK
		mcnt_pListSt->SetItem(2, 0, "���Ə�ԕ\");
#endif
		mcnt_pListSt->SetItem(3, 0, "�^�]���");
		mcnt_pListSt->SetItem(4, 0, "PLG�敪");
		mcnt_pListSt->SetItem(5, 0, "��������");
	}

	//-----------------------
	// �V�X�e����� ���X�g�r���[�C���X�^���X���� & ��`
	if (true) {
		mcnt_pListSys = new DspListView(&m_ListSys);
		// �����ڒ�`
		m_ListSys.SetExtendedStyle(m_ListSys.GetExtendedStyle() | LVS_EX_GRIDLINES);			// �r���`��
		m_ListSys.SetExtendedStyle(m_ListSys.GetExtendedStyle() | LVS_EX_FULLROWSELECT);		// 1�s�S�̑I��

		// ��ǉ�			0		1			
		char name[][32] = { "����",	"�\��",	"����" };
		int  wid[] = { 80, 	75,		75 };
		mcnt_pListSys->SetColumnAll(sizeof(wid) / sizeof(int), 32, wid, (char const*)name);
		// �s�ǉ�
		mcnt_pListSys->SetRowAll(4);
		// �Œ�l�Z�b�g
		mcnt_pListSys->SetItem(0, 0, "HT���я��");
		mcnt_pListSys->SetItem(1, 0, "�������");
		mcnt_pListSys->SetItem(2, 0, "�@����");
		mcnt_pListSys->SetItem(3, 0, "��������");
	}


	//// ����֌W ���X�g�r���[�C���X�^���X���� & ��`
	if (true) {		// �P��
		mcnt_pListHt = new DspListView(&m_ListHt);
		// �����ڒ�`
		m_ListHt.SetExtendedStyle(m_ListHt.GetExtendedStyle() | LVS_EX_GRIDLINES);			// �r���`��
		m_ListHt.SetExtendedStyle(m_ListHt.GetExtendedStyle() | LVS_EX_FULLROWSELECT);		// 1�s�S�̑I��

#ifndef HT_IS_COLOR
		// ��ǉ�			0		1		2		3		4			
		char name[][32] = { "�@�\",	"PCID",	"���",	"���",	"FrameNo", "Edge",	"��", "", "��", "" };
		int  wid[] = { 52,		40,		36,		36,		62,			45,		32,	  18,	32,	18 };
#else
		// ��ǉ�			0		1		2		3		4			
		char name[][32] = {	"HT�@�\",	"PCID",	"���",	"���",	"FrameNo", 	"R", "�P", "�I", "�Q",  "G", "�P", "�I", "�Q", "B", "�P", "�I", "�Q"};
		int  wid[]		= {	52,			40,		36,		36,		62,			 18,   32,   32,   32,   18,   32,   32,   32,  18,   32,   32,  32};

#endif
		mcnt_pListHt->SetColumnAll(sizeof(wid) / sizeof(int), 32, wid, (char const*)name);
		// �s�ǉ�
		mcnt_pListHt->SetRowAll(NUM_CAM_POS*NUM_MEN);
		// �Œ�l�Z�b�g
		int nRow = 0;
		for (int ii = 1; ii <= MAX_CAMSET; ii++) {
			mcnt_pListHt->SetItem(nRow, 0, DivNameManager::GetDivCamSet(ii));		// �@�\
			nRow++;
		}
	}
//	if (true) {		// ����
//		mcnt_pListCy = new DspListView(&m_ListCy);
//		// �����ڒ�`
//		m_ListCy.SetExtendedStyle(m_ListHt.GetExtendedStyle() | LVS_EX_GRIDLINES);			// �r���`��
//		m_ListCy.SetExtendedStyle(m_ListHt.GetExtendedStyle() | LVS_EX_FULLROWSELECT);		// 1�s�S�̑I��
//
//#ifndef HT_IS_COLOR
//		// ��ǉ�			0		1		2		3		4			
//		char name[][32] = { "�@�\",	"PCID",	"���",	"���",	"FrameNo", "��", "", "��", "" };
//		int  wid[] = { 52,		40,		36,		36,		62,			32,	  18,	32,	18 };
//#else
//		// ��ǉ�			0		1		2		3		4			
//		char name[][32] = { "CY�@�\",	"PCID",	"���",	"���",	"FrameNo" };
//		int  wid[] = { 52,			40,		36,		36,		62 };
//
//#endif
//		mcnt_pListCy->SetColumnAll(sizeof(wid) / sizeof(int), 32, wid, (char const*)name);
//		// �s�ǉ�
//		mcnt_pListCy->SetRowAll(NUM_CAM_POS*NUM_MEN);
//		// �Œ�l�Z�b�g
//		int nRow = 0;
//		for (int ii = 1; ii <= MAX_CAMSET; ii++) {
//			mcnt_pListCy->SetItem(nRow, 0, DivNameManager::GetDivCamSet(ii));		// �@�\
//			nRow++;
//		}
//	}
	if (true) {		// �p������
		mcnt_pListPr = new DspListView(&m_ListPr);
		
		// �����ڒ�`
		m_ListPr.SetExtendedStyle(m_ListHt.GetExtendedStyle() | LVS_EX_GRIDLINES);			// �r���`��
		m_ListPr.SetExtendedStyle(m_ListHt.GetExtendedStyle() | LVS_EX_FULLROWSELECT);		// 1�s�S�̑I��
		
		char name[][32] = { "�@�\", "PCID", "���", "c1", "c2", "���я��" };
		int  wid[]		= {     45,		38,		36,	  25,	25,		  67 };

		mcnt_pListPr->SetColumnAll(sizeof(wid) / sizeof(int), 32, wid, (char const*)name);
		// �s�ǉ�
		mcnt_pListPr->SetRowAll(NUM_ALL_HTPARA);
		// �Œ�l�Z�b�g
		int nRow = 0;
		CString sWk;
		for (int ii = 0; ii < NUM_ALL_HTPARA; ii++)
		{
			sWk.Format("�����%d", ii + 1);
			mcnt_pListPr->SetItem(nRow, 0, sWk);			// �@�\
			nRow++;
		}
	}
	//// ���菈�����Ԋ֌W ���X�g�r���[�C���X�^���X���� & ��`
	if (true) {
		mcnt_pListHtTime = new DspListView(&m_ListHtTime);
		m_ListHtTime.SetExtendedStyle(m_ListHtTime.GetExtendedStyle() | LVS_EX_GRIDLINES);			// �r���`��
		m_ListHtTime.SetExtendedStyle(m_ListHtTime.GetExtendedStyle() | LVS_EX_FULLROWSELECT);		// 1�s�S�̑I��

		// ��ǉ�
		char name[][32] = { "",	"�\Ave",	"�\Max",	"��Ave",	"��Max" };
		int  wid[] = { 48,	45,			45,			45,			45 };
		mcnt_pListHtTime->SetColumnAll(sizeof(wid) / sizeof(int), 32, wid, (char const*)name);
		// �s�ǉ�
		mcnt_pListHtTime->SetRowAll(7);
		// �Œ�l�Z�b�g
		mcnt_pListHtTime->SetItem(0, 0, "DS HT");
		mcnt_pListHtTime->SetItem(1, 0, "WS HT");
		mcnt_pListHtTime->SetItem(2, 0, "�`��1");
		mcnt_pListHtTime->SetItem(3, 0, "�`��2");
		mcnt_pListHtTime->SetItem(4, 0, "HT-TO");
		mcnt_pListHtTime->SetItem(5, 0, "TO");
		mcnt_pListHtTime->SetItem(6, 0, "İ��");

	}


	//// �ݔ��ʒu�g���b�L���O ���X�g�r���[�C���X�^���X���� & ��`
	if(true){
		mcnt_pListSetuDsp = new DspListView(&m_ListSetuDsp);
		// �����ڒ�`
		m_ListSetuDsp.SetExtendedStyle( m_ListSetuDsp.GetExtendedStyle() | LVS_EX_GRIDLINES );			// �r���`��
		m_ListSetuDsp.SetExtendedStyle( m_ListSetuDsp.GetExtendedStyle() | LVS_EX_FULLROWSELECT );		// 1�s�S�̑I��
		//m_ListSetuDsp.GetHeaderCtrl()->ModifyStyle(0, HDS_HIDDEN);	// �s�w�b�_�[����

		// ��ǉ�			0		1		2	
		char name[][32] = {	"",		"rec",	"�R�C��No"};
		int  wid[]		= {	73,		31,		80};
		int	 fmt[]		= {LVCFMT_LEFT, LVCFMT_CENTER, LVCFMT_LEFT};
		mcnt_pListSetuDsp->SetColumnAll(sizeof(wid)/sizeof(int), 32, wid, (char const*)name, fmt);
		// �s�ǉ�
		mcnt_pListSetuDsp->SetRowAll(7);		// �����ʒu+�\����_+�V���[
		// �Œ�l�Z�b�g
		int nRow = 0;
		mcnt_pListSetuDsp->SetItem( nRow, 0, "�\ �����ʒu");	nRow ++;
		mcnt_pListSetuDsp->SetItem( nRow, 0, "�� �����ʒu");	nRow ++;
		mcnt_pListSetuDsp->SetItem( nRow, 0, "�������R�C��");	nRow ++;
		mcnt_pListSetuDsp->SetItem( nRow, 0, "�\ ������ʒu");	nRow ++;
		mcnt_pListSetuDsp->SetItem( nRow, 0, "�� ������ʒu");	nRow ++;
		mcnt_pListSetuDsp->SetItem( nRow, 0, "�V���[�ʒu");		nRow ++;
		mcnt_pListSetuDsp->SetItem( nRow, 0, "���\���R�C��");	nRow++;
	}

	//-----------------------
	// �\���֌W
	if (true) {
		mcnt_pListOp = new DspListView(&m_ListOp);
		// �����ڒ�`
		m_ListOp.SetExtendedStyle(m_ListOp.GetExtendedStyle() | LVS_EX_GRIDLINES);			// �r���`��
		m_ListOp.SetExtendedStyle(m_ListOp.GetExtendedStyle() | LVS_EX_FULLROWSELECT);		// 1�s�S�̑I��

		// ��ǉ�			0		1		2				
		char name[][32] = { "HY���",	"��",	"�ő�" };
		int  wid[] = { 70,		40,		40, };
		mcnt_pListOp->SetColumnAll(sizeof(wid) / sizeof(int), 32, wid, (char const*)name);
		// �s�ǉ�
		mcnt_pListOp->SetRowAll(6);
		mcnt_pListOp->SetItem(0, 0, "[1]�ڑ�[��]");
		mcnt_pListOp->SetItem(1, 0, "[1]���MQue");
		mcnt_pListOp->SetItem(2, 0, "[2]�ڑ�[��]");
		mcnt_pListOp->SetItem(3, 0, "[2]���MQue");
		mcnt_pListOp->SetItem(4, 0, "[3]�ڑ�[��]");
		mcnt_pListOp->SetItem(5, 0, "[3]���MQue");
	}

	//-----------------------
	// �G�b�W�֌W
	if (true) {
		mcnt_pListEdge = new DspListView(&m_ListEdge);
		// �����ڒ�`
		m_ListEdge.SetExtendedStyle(m_ListEdge.GetExtendedStyle() | LVS_EX_GRIDLINES);			// �r���`��
		m_ListEdge.SetExtendedStyle(m_ListEdge.GetExtendedStyle() | LVS_EX_FULLROWSELECT);		// 1�s�S�̑I��

		// ��ǉ�			0		1		2		3			4		5
		char name[][32] = { "",	"���[�h",		"DS",	"WS",	"��DS",	"��WS", };
		int  wid[]		= { 20,		60,		50,		50,		50,		50 };
		mcnt_pListEdge->SetColumnAll(sizeof(wid) / sizeof(int), 32, wid, (char const*)name);
		// �s�ǉ�
		mcnt_pListEdge->SetRowAll(2);
		mcnt_pListEdge->SetItem(0, 0, "�\");
		mcnt_pListEdge->SetItem(1, 0, "��");
	}

	//-----------------------
	// �@����֌W
	if (true) {
		mcnt_pListKiki = new DspListView(&m_ListKiki);
		// �����ڒ�`
		m_ListKiki.SetExtendedStyle(m_ListKiki.GetExtendedStyle() | LVS_EX_GRIDLINES);			// �r���`��
		m_ListKiki.SetExtendedStyle(m_ListKiki.GetExtendedStyle() | LVS_EX_FULLROWSELECT);		// 1�s�S�̑I��
																								//m_ListKiki.GetHeaderCtrl()->ModifyStyle(0, HDS_HIDDEN);	// �s�w�b�_�[����

		// ��ǉ�			0		1
		char name[][32] = { "",		 "���" };
		int  wid[]		= { 130,		140 };
		mcnt_pListKiki->SetColumnAll(sizeof(wid) / sizeof(int), 32, wid, (char const*)name);
		// �s�ǉ�
		mcnt_pListKiki->SetRowAll(11);
		mcnt_pListKiki->SetItem(0, 0, "���ݻ���");
		mcnt_pListKiki->SetItem(1, 0, "���ݻ�ғ����");
		mcnt_pListKiki->SetItem(2, 0, "��Ԉʒu(�\)");
		mcnt_pListKiki->SetItem(3, 0, "��Ԉʒu(��)");
		mcnt_pListKiki->SetItem(4, 0, "SPM���");
		mcnt_pListKiki->SetItem(5, 0, "�h�����ڰ���");
		mcnt_pListKiki->SetItem(6, 0, "�װ�^�](�\/��/BK)");
		mcnt_pListKiki->SetItem(7, 0, "�װ�ُ�(�\/��/BK)");
		mcnt_pListKiki->SetItem(8, 0, "����ײ�(�\) �^�]/�ُ�");
		mcnt_pListKiki->SetItem(9, 0, "����ײ�(��) �^�]/�ُ�");
		mcnt_pListKiki->SetItem(10, 0, "��/�����");
	}

	//-----------------------
	// �{�C�X���
	if (true) {
		mcnt_pListVoice = new DspListView(&m_ListVoice);
		// �����ڒ�`
		m_ListVoice.SetExtendedStyle(m_ListVoice.GetExtendedStyle() | LVS_EX_GRIDLINES);			// �r���`��
		m_ListVoice.SetExtendedStyle(m_ListVoice.GetExtendedStyle() | LVS_EX_FULLROWSELECT);		// 1�s�S�̑I��

		// ��ǉ�			0		1
		char name[][32] = { "��",	"����" };
		int  wid[]		= { 25,70 };
		mcnt_pListVoice->SetColumnAll(sizeof(wid) / sizeof(int), 32, wid, (char const*)name);
		// �s�ǉ�
		mcnt_pListVoice->SetRowAll(1);
	}

	//-----------------------
	// �r�ڋ߃����v���
	if (true) {
		mcnt_pListRamp = new DspListView(&m_ListRamp);
		// �����ڒ�`
		m_ListRamp.SetExtendedStyle(m_ListRamp.GetExtendedStyle() | LVS_EX_GRIDLINES);			// �r���`��
		m_ListRamp.SetExtendedStyle(m_ListRamp.GetExtendedStyle() | LVS_EX_FULLROWSELECT);		// 1�s�S�̑I��

		// ��ǉ�			0	1		2		3		4		5		6		7		8
		char name[][32] = { "",	"DS",	"2",	"3",	"4",	"5",	"6",	"7",	"WS" };
		int  wid[]		= { 40,	28 ,	28 ,	28 ,	28 ,	28 ,	28 ,	28 ,	28 };
		mcnt_pListRamp->SetColumnAll(sizeof(wid) / sizeof(int), 32, wid, (char const*)name);
		// �s�ǉ�
		mcnt_pListRamp->SetRowAll(3);
		mcnt_pListRamp->SetItem(0, 0, "�\Lmp");
		mcnt_pListRamp->SetItem(1, 0, "��Lmp");
		mcnt_pListRamp->SetItem(2, 0, "��Alm");
	}

	//-----------------------
	// �p�g���C�g���
	if (true) {
		mcnt_pListPato = new DspListView(&m_ListPato);
		// �����ڒ�`
		m_ListPato.SetExtendedStyle(m_ListPato.GetExtendedStyle() | LVS_EX_GRIDLINES);			// �r���`��
		m_ListPato.SetExtendedStyle(m_ListPato.GetExtendedStyle() | LVS_EX_FULLROWSELECT);		// 1�s�S�̑I��

		// ��ǉ�			0	�@�@1		2
		char name[][32] = { "�ʒu",	"���",	"�޻�" };
		int  wid[]		= { 65, 35,	35 };
		mcnt_pListPato->SetColumnAll(sizeof(wid) / sizeof(int), 32, wid, (char const*)name);
		// �s�ǉ�
		mcnt_pListPato->SetRowAll(3);
		mcnt_pListPato->SetItem(0, 0, "�\����(�\)");
		mcnt_pListPato->SetItem(1, 0, "�@�@�@�@ (��)");
		mcnt_pListPato->SetItem(2, 0, "���o�͐�");
	}

	////-----------------------
	//// �f�[�^������
	//if (true) {
	//	mcnt_pListDmem = new DspListView(&m_ListDmem);
	//	// �����ڒ�`
	//	m_ListDmem.SetExtendedStyle(m_ListDmem.GetExtendedStyle() | LVS_EX_GRIDLINES);			// �r���`��
	//	m_ListDmem.SetExtendedStyle(m_ListDmem.GetExtendedStyle() | LVS_EX_FULLROWSELECT);		// 1�s�S�̑I��

	//	// ��ǉ�			0		1
	//	char name[][32] = { "����",	"���",	"����",	"���" };
	//	int  wid[] = { 100,	45,		100,	45 };
	//	mcnt_pListDmem->SetColumnAll(sizeof(wid) / sizeof(int), 32, wid, (char const*)name);
	//	// �s�ǉ�
	//	mcnt_pListDmem->SetRowAll(6);
	//	mcnt_pListDmem->SetItem(0, 0, "ϰ�ݸ�PB");
	//	mcnt_pListDmem->SetItem(1, 0, "ϰ��w�ߋ���");
	//	mcnt_pListDmem->SetItem(2, 0, "ϰ��Ď��ټ�");
	//	mcnt_pListDmem->SetItem(3, 0, "ϰ��Ď��ғ�");
	//	mcnt_pListDmem->SetItem(4, 0, "ϰ��Ď�����");
	//	mcnt_pListDmem->SetItem(5, 0, "ϰ��Ď��@��");
	//	mcnt_pListDmem->SetItem(0, 2, "ϰ��x��");
	//	mcnt_pListDmem->SetItem(1, 2, "ϰ��ُ�");
	//	mcnt_pListDmem->SetItem(2, 2, "ϰ��󎚉\");
	//	mcnt_pListDmem->SetItem(3, 2, "ϰ��g�p");
	//	mcnt_pListDmem->SetItem(4, 2, "�ں�ғ�");
	//	mcnt_pListDmem->SetItem(5, 2, "�ں�ُ�");

	//}

	//// �R�C���֌W ���X�g�r���[�C���X�^���X���� & ��`
	if (true) {
		mcnt_pListCoil = new DspListView(&m_ListCoil);
		// �����ڒ�`
		m_ListCoil.SetExtendedStyle(m_ListCoil.GetExtendedStyle() | LVS_EX_GRIDLINES);			// �r���`��
		m_ListCoil.SetExtendedStyle(m_ListCoil.GetExtendedStyle() | LVS_EX_FULLROWSELECT);		// 1�s�S�̑I��

		// ��ǉ�			0	1		2			3			4			5		6			7			8			9			10			11
		char name[][32]	= { "",	"rec",	"�Ǘ�No",	"����No",	"�R�C��No",	"��ْ�",	"��",	"Scode�\",	"Scode��",	"Pcode�\",	"Pcode��",	"�����L��"};
		int  wid[]		= { 30,	28,		119,		40,			80,			45,			36,		25,			25,			25,			25,			25};
		mcnt_pListCoil->SetColumnAll(sizeof(wid) / sizeof(int), 32, wid, (char const*)name);
		// �s�ǉ�
		mcnt_pListCoil->SetRowAll(2 + SIZE_COIL_BUF - 1);
		// �Œ�l�Z�b�g
		int nRow = 0;
		mcnt_pListCoil->SetItem(0, 0, "��");
		mcnt_pListCoil->SetItem(1, 0, "��");
		for (int ii = 0; ii<SIZE_COIL_BUF - 1; ii++) {
			sWk.Format("�O%d", ii + 1);
			mcnt_pListCoil->SetItem(2 + ii, 0, sWk);
		}
	}

	//// PLG ������_��� ���X�g�r���[�C���X�^���X���� & ��`
	if (true) {
		mcnt_pListPlgKen = new DspListView(&m_ListPlgKen);
		// �����ڒ�`
		m_ListPlgKen.SetExtendedStyle(m_ListPlgKen.GetExtendedStyle() | LVS_EX_GRIDLINES);		// �r���`��
		m_ListPlgKen.SetExtendedStyle(m_ListPlgKen.GetExtendedStyle() | LVS_EX_FULLROWSELECT);	// 1�s�S�̑I��

		// ��ǉ�
		char name[][32] = { "������_",	"",	"" };
		int  wid[] = { 115, 65, 33 };
		mcnt_pListPlgKen->SetColumnAll(sizeof(wid) / sizeof(int), 32, wid, (char const*)name);
		// �s�ǉ�
		mcnt_pListPlgKen->SetRowAll(5);
		// �Œ�l�Z�b�g
		mcnt_pListPlgKen->SetItem(0, 0, "�\ �����ʒu ����");
		mcnt_pListPlgKen->SetItem(1, 0, "�� �����ʒu ����");
		mcnt_pListPlgKen->SetItem(2, 0, "��r�pPLG");
		mcnt_pListPlgKen->SetItem(3, 0, "�R�C���c��");
		mcnt_pListPlgKen->SetItem(4, 0, "�O�R�C��������");
		for (int ii = 0; ii<5; ii++) {
			mcnt_pListPlgKen->SetItem(ii, 2, "[m]");
		}
	}

	//// PLG �\����_��� ���X�g�r���[�C���X�^���X���� & ��`
	if (true) {
		mcnt_pListPlgDsp = new DspListView(&m_ListPlgDsp);
		// �����ڒ�`
		m_ListPlgDsp.SetExtendedStyle(m_ListPlgDsp.GetExtendedStyle() | LVS_EX_GRIDLINES);		// �r���`��
		m_ListPlgDsp.SetExtendedStyle(m_ListPlgDsp.GetExtendedStyle() | LVS_EX_FULLROWSELECT);	// 1�s�S�̑I��

		// ��ǉ�
		char name[][32] = { "�\����_",	"",	"" };
		int  wid[] = { 115, 65, 33 };
		mcnt_pListPlgDsp->SetColumnAll(sizeof(wid) / sizeof(int), 32, wid, (char const*)name);
		// �s�ǉ�
		mcnt_pListPlgDsp->SetRowAll(6);
		// �Œ�l�Z�b�g
		mcnt_pListPlgDsp->SetItem(0, 0, "�\����_ ����");
		mcnt_pListPlgDsp->SetItem(1, 0, "�\������ ����");
		mcnt_pListPlgDsp->SetItem(2, 0, "�������� ����");
		mcnt_pListPlgDsp->SetItem(3, 0, "�V���[(�����_��)");
		mcnt_pListPlgDsp->SetItem(4, 0, "�V���[(�o���_��)");
		mcnt_pListPlgDsp->SetItem(5, 0, "�O�R�C��������");
		for (int ii = 0; ii<6; ii++) {
			mcnt_pListPlgDsp->SetItem(ii, 2, "[m]");
		}
	}

	//// PLG��{��� ���X�g�r���[�C���X�^���X���� & ��`
	if (true) {
		mcnt_pListPlg = new DspListView(&m_ListPlg);
		// �����ڒ�`
		m_ListPlg.SetExtendedStyle(m_ListPlg.GetExtendedStyle() | LVS_EX_GRIDLINES);		// �r���`��
		m_ListPlg.SetExtendedStyle(m_ListPlg.GetExtendedStyle() | LVS_EX_FULLROWSELECT);	// 1�s�S�̑I��

		// ��ǉ�
		char name[][32] = { "��{���",	"",	"" };
		int  wid[] = { 90, 60, 60 };
		mcnt_pListPlg->SetColumnAll(sizeof(wid) / sizeof(int), 32, wid, (char const*)name);
		// �s�ǉ�
		mcnt_pListPlg->SetRowAll(11);
		// �Œ�l�Z�b�g
		mcnt_pListPlg->SetItem(0, 0, "����FrameNo");
		mcnt_pListPlg->SetItem(1, 0, "1�ڰю���");
		mcnt_pListPlg->SetItem(2, 0, "1�ڰђ���");
		mcnt_pListPlg->SetItem(3, 0, "���C�����x");
		mcnt_pListPlg->SetItem(4, 0, "�c����\");
		mcnt_pListPlg->SetItem(5, 0, "(�s�x)");
		mcnt_pListPlg->SetItem(6, 0, "�Ȉ�ײݑ��x��");
		mcnt_pListPlg->SetItem(7, 0, "�Ȉ�ײݑ��x�\");
		mcnt_pListPlg->SetItem(8, 0, "SEQ FrameNo");
		mcnt_pListPlg->SetItem(9, 0, "SEQ Exsync");
		mcnt_pListPlg->SetItem(10, 0, "SEQ Plg");

		mcnt_pListPlg->SetItem(0, 2, "");
		mcnt_pListPlg->SetItem(1, 2, "[ms]");
		mcnt_pListPlg->SetItem(2, 2, "[mm]");
		mcnt_pListPlg->SetItem(3, 2, "[mpm]");
		mcnt_pListPlg->SetItem(4, 2, "[mm/px]");
		mcnt_pListPlg->SetItem(5, 2, "[mm/px]");
		mcnt_pListPlg->SetItem(6, 2, "[mpm]");
		mcnt_pListPlg->SetItem(7, 2, "[mpm]");
		mcnt_pListPlg->SetItem(8, 2, "");
		mcnt_pListPlg->SetItem(9, 2, "[pls]");
		mcnt_pListPlg->SetItem(10, 2, "[pls]");
	}

	//-----------------------
	// DB�֌W ���X�g�r���[�C���X�^���X���� & ��`
	if (true) {
		mcnt_pListDb = new DspListView(&m_ListDb);
		// �����ڒ�`
		m_ListDb.SetExtendedStyle(m_ListDb.GetExtendedStyle() | LVS_EX_GRIDLINES);			// �r���`��
		m_ListDb.SetExtendedStyle(m_ListDb.GetExtendedStyle() | LVS_EX_FULLROWSELECT);		// 1�s�S�̑I��

		// ��ǉ�			0		1				2		3		4			5			6				7	
		char name[][32] = { "",		"�Ǘ�No",		"�ۑ�",	"�S��",	"���Q�폜",	"�L�Q�폜", "���Q�摜�폜", "�L�Q�摜�폜" };
		int  wid[]		= { 28,		115,			70,		70,			70,		70,			70,				70 };
		mcnt_pListDb->SetColumnAll(sizeof(wid) / sizeof(int), 32, wid, (char const*)name);
		// �s�ǉ�
		mcnt_pListDb->SetRowAll(DbBaseManager::SIZE_SQLINF_NUM);
		// �Œ�l�Z�b�g
		int nRow = 0;
		mcnt_pListDb->SetItem(0, 0, "��");
		for (int ii = 0; ii<mcnt_pListDb->GetRowCount(); ii++) {
			sWk.Format("�O%d", ii + 1);
			mcnt_pListDb->SetItem(1 + ii, 0, sWk);
		}
	}
	if (true) {
		mcnt_pListDbInfo = new DspListView(&m_ListDbInfo);
		// �����ڒ�`
		m_ListDbInfo.SetExtendedStyle(m_ListDbInfo.GetExtendedStyle() | LVS_EX_GRIDLINES);			// �r���`��
		m_ListDbInfo.SetExtendedStyle(m_ListDbInfo.GetExtendedStyle() | LVS_EX_FULLROWSELECT);		// 1�s�S�̑I��

		// ��ǉ�			0		1		2				
		char name[][32] = { "����",	"��",	"�ő�" };
		int  wid[]		= { 115,		40,		40, };
		mcnt_pListDbInfo->SetColumnAll(sizeof(wid) / sizeof(int), 32, wid, (char const*)name);
		// �s�ǉ�
		mcnt_pListDbInfo->SetRowAll(10);
		// �Œ�l�Z�b�g
		mcnt_pListDbInfo->SetItem(0, 0, "(SQL) SQL����[��]");
		mcnt_pListDbInfo->SetItem(1, 0, "(SQL) DB�o�^[��]");
		mcnt_pListDbInfo->SetItem(2, 0, "(SQL) �摜�o�^[ms]");
		mcnt_pListDbInfo->SetItem(3, 0, "(SQL) SQL�o�^[ms]");
		mcnt_pListDbInfo->SetItem(4, 0, "(BCP) SQL����[��]");
		mcnt_pListDbInfo->SetItem(5, 0, "(BCP) �摜�o�^[ms]");
		mcnt_pListDbInfo->SetItem(6, 0, "(BCP) ATTR SR[ms]");
		mcnt_pListDbInfo->SetItem(7, 0, "(BCP) ATTR Bat[ms]");
		mcnt_pListDbInfo->SetItem(8, 0, "(BCP) IMG SR[ms]");
		mcnt_pListDbInfo->SetItem(9, 0, "(BCP) IMG Bat[ms]");
	}
	
	//// ���C���C���X�^���X �J�n����
	mcls_MI.SetLogMgr(mcls_pLog);
	mcls_MI.SetLogMgrSeq(mcls_pLogSeq);
	mcls_MI.SetLogMgrE(mcls_pLogE);
	mcls_MI.SetLogMgrG(mcls_pLogG);
	mcls_MI.SetLogMgrD(mcls_pLogD);
	mcls_MI.SetLogMgrA(mcls_pLogA);
	mcls_MI.Init();

	//// ��ʍX�V�p�^�C�}����
	SetTimer(1, 500, NULL);

	//// �f�o�b�N�p
	m_nCoil = 0;

	//// ���C���X���b�h�̎��s���s��
	LOG(em_MSG), "[Dlg] �e�v���Z�X�J�n���� ����");
	mcls_MI.Start();
	return;
}

//�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P
// �e�v���Z�X�J������
//�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q
void CTO_DEFECTDlg::ExitProcess()
{
	//// ���C���C���X�^���X �I������
	KillTimer(1);			// ��ʍX�V�^�C�}
	mcls_MI.Stop();
	mcls_MI.Exit();

	//// ���O ���X�g�{�b�N�X�\�������I��
	mcls_pLog->SetListOut(false);					// ����ȍ~ ���X�g�{�b�N�X�ւ̃��O�\���֎~

													//// ���O�N���X�J��
	LOG(em_MSG), "[Dlg] �^�X�N�I��");
	if (NULL != mcls_pLog) {
		delete mcls_pLog;
		mcls_pLog = NULL;
	}

	LOGSEQ(em_MSG), "[Dlg] �^�X�N�I��");
	if (NULL != mcls_pLogSeq) {
		delete mcls_pLogSeq;
		mcls_pLogSeq = NULL;
	}

	LOGE(em_MSG), "[Dlg] �^�X�N�I��");
	if (NULL != mcls_pLogE) {
		delete mcls_pLogE;
		mcls_pLogE = NULL;
	}

	LOGG(em_MSG), "[Dlg] �^�X�N�I��");
	if (NULL != mcls_pLogG) {
		delete mcls_pLogG;
		mcls_pLogG = NULL;
	}

	LOGD(em_MSG), "[Dlg] �^�X�N�I��");
	if (NULL != mcls_pLogD) {
		delete mcls_pLogD;
		mcls_pLogD = NULL;
	}

	LOGA(em_MSG), "[Dlg] �^�X�N�I��");
	if (NULL != mcls_pLogA) {
		delete mcls_pLogA;
		mcls_pLogA = NULL;
	}

	//// ��ʃR���g���[���J��
	delete mcnt_pListSt;
	delete mcnt_pListSys;
	delete mcnt_pListHt;
	delete mcnt_pListHtTime;
	delete mcnt_pListOp;
	delete mcnt_pListPr;

	delete mcnt_pListDb;
	delete mcnt_pListDbInfo;

	delete mcnt_pListSetuDsp;
	delete mcnt_pListEdge;
	delete mcnt_pListKiki;
	delete mcnt_pListRamp;
	delete mcnt_pListPato;
	delete mcnt_pListVoice;
	//delete mcnt_pListDmem;

	delete mcnt_pListCoil;
	delete mcnt_pListPlgKen;
	delete mcnt_pListPlgDsp;
	delete mcnt_pListPlg;

	delete mcnt_pListCmd;
	delete mcnt_pListInput;

	// �^�X�N�I������
	task_exit();

	// ��ʂ̏I���v��
	PostMessage(WM_QUIT);
}

//------------------------------------------
// ���C���X���b�h DlgThread
// LPVOID param �_�C�����O�|�C���^
//------------------------------------------
unsigned WINAPI CTO_DEFECTDlg::MainThread(void* param)
{
	CTO_DEFECTDlg* p = (CTO_DEFECTDlg *)param;
	COMMON_QUE que;			// ��M�\����
	bool bWk;

							//// �e�v���Z�X���������� (����ȑO�̓��O�o�͋֎~)
	p->InitProcess();

	//// �N���v����荞�� ���[�v�J�n
	while (true) {
		recv_mail(&que);			// �N���v����荞��

		switch (que.nEventNo) {

//-----------------------------------------------------------------------------------------------
		case FACT_TO_DEFECT_01:								// �p�����[�^�ύX�v���ʒm (��KA_PARSET, KA_PARENG)
			pLOG(em_MSG), "[MAIL] �p�����[�^�ύX�v�� (FACT_TO_DEFECT_01) [�e�[�u��=%d]", que.fl.data[1]); 
			// ���C���C���X�^���X�֒ʒm
			p->mcls_MI.SetDeliveryMail( E_DEF_ML_PARAM, &que);
			break;

//-----------------------------------------------------------------------------------------------
		case FACT_TO_DEFECT_02:								// �����J�n�v�� (��SO_GAMENN)
			bWk = ( 0 == que.fl.data[2] ? false : true);
			pLOG(em_MSG), "[MAIL] �����J�n�v�� (FACT_TO_DEFECT_02) [%s, %s, %s, s=%d:%d, p=%d:%d, �p�������P=%d:%d, �p�������Q=%d:%d]",
				DivNameManager::GetDivUnten(que.fl.data[0]), DivNameManager::GetDivPlg(que.fl.data[1]), p->mcls_MI.gcls_Status.GetMenModeName(bWk), que.fl.data[3], que.fl.data[4], que.fl.data[5], que.fl.data[6], que.fl.data[7], que.fl.data[8], que.fl.data[9], que.fl.data[10] );
			syslog(230, "[%s; %s; %s; s=%d:%d; p=%d:%d]", DivNameManager::GetDivUnten(que.fl.data[0]), DivNameManager::GetDivPlg(que.fl.data[1]), p->mcls_MI.gcls_Status.GetMenModeName(bWk), que.fl.data[3], que.fl.data[4], que.fl.data[5], que.fl.data[6]);

			//// �����Ŕ��f���� �����Ԃ͏��������Ă���
			for(int ii=0; ii<NUM_ALL_HANTEI; ii++ ) {
				CString wk;
				wk.Format("HANTEI_ANS_%d_ID", ii+1 );
				KizuFunction::SetStatus(wk, true, false);
			}
			for (int ii = 0; ii < NUM_CAMERA; ii++)
			{
				CString wk;
				wk.Format("CAM_GCP_ERR_%d_ID ", ii + 1);
				KizuFunction::SetStatus(wk, true, false);
			}
			
			// ���C���C���X�^���X�֒ʒm
			p->mcls_MI.SetDeliveryMail( E_DEF_ML_START, &que);
			break;

//-----------------------------------------------------------------------------------------------
		case FACT_TO_DEFECT_03:								// ������~�v�� (��SO_GAMENN)
			pLOG(em_MSG), "[MAIL] ������~�v�� (FACT_TO_DEFECT_03)"); 
			syslog(231, "");

			// ���C���C���X�^���X�֒ʒm
			p->mcls_MI.SetDeliveryMail( E_DEF_ML_STOP, &que);
			break;

//-----------------------------------------------------------------------------------------------
		case FACT_TO_DEFECT_04:								// �ғ���Ԏ擾�v�� (��SO_GAMENN)
			pLOG(em_MSG), "[MAIL] �ғ���Ԏ擾�v�� (FACT_TO_DEFECT_04)");

			// �I�y���[�^��ʂփA���T�[�ԋp
			p->mcls_MI.Send_SoGamenn_MailStatus();
			break;

//-----------------------------------------------------------------------------------------------
		case FACT_TO_DEFECT_06:								// ����J�n�A���T�[ (��HT_DEFECT)
			pLOG(em_MSG), "[MAIL] ����J�n�A���T�[ (FACT_TO_DEFECT_06) [�J�����y�A=%d PCID=%d ����=%d kind=%d(1:nc,2:pd,3:pr)]", que.fl.data[0], que.fl.data[1], que.fl.data[2], que.fl.data[3] );

			// ���C���C���X�^���X�֒ʒm
			p->mcls_MI.SetDeliveryMail( E_DEF_ML_WAIT_START_ANSWER, &que);
			break;

//-----------------------------------------------------------------------------------------------
		case FACT_TO_DEFECT_07:								// �����~�A���T�[ (��HT_DEFECT)
			pLOG(em_MSG), "[MAIL] �����~�A���T�[ (FACT_TO_DEFECT_07) [�J�����y�A=%d PCID=%d ����=%d <%s> kind=%d(1:nc,2:pd,3:pr)]", que.fl.data[0], que.fl.data[1], que.fl.data[2], que.fl.data[3]==0 ? "����":"�ُ�F��", que.fl.data[4] );
			
			// ���C���C���X�^���X�֒ʒm
			p->mcls_MI.SetDeliveryMail( E_DEF_ML_WAIT_STOP_ANSWER, &que );
			break;

//-----------------------------------------------------------------------------------------------
		case FACT_TO_DEFECT_09:								// �����Ԗ⍇���� (��HT_DEFECT)
			pLOG(em_MSG), "[MAIL] �����Ԗ⍇���� (FACT_TO_DEFECT_09) [�J�����y�A=%d PCID=%d ����=%d kind=%d(1:nc,2:pd,3:pr)]", que.fl.data[0], que.fl.data[1], que.fl.data[2], que.fl.data[3] );

			// ���C���C���X�^���X�֒ʒm
			p->mcls_MI.SetDeliveryMail( E_DEF_ML_STATE_ANSWER, &que);
			break;

//-----------------------------------------------------------------------------------------------
		case FACT_TO_DEFECT_11:								// �R�C������M�ʒm (��TO_GETDAT)
			pLOG(em_MSG), "[MAIL] �R�C������M�ʒm (FACT_TO_DEFECT_11) [��M�ʒu=%d (0:������_ 1:�\����_)]", que.fl.data[0]);
			
			//// �s����M�ʒu�̓`����M
			if (0 != que.fl.data[0] && 1 != que.fl.data[0]) {
				pLOG(em_ERR), "[MI] ��M�ʒu�s���̓`����M [��M�ʒu=%d]", que.fl.data[0]);
				syslog(251, "[��M�ʒu=%d]", que.fl.data[0]);
				break;
			}

			//// �ʏ팟�����ȊO�͖���
			if( ! p->mcls_MI.gcls_Status.IsDoSequence() ) {
				pLOG(em_ERR), "[MI] �ʏ팟�����ȊO��%s�R�C������M", 0 == que.fl.data[0] ? "�����p" : "�\���p");
				syslog(251, "[%s�R�C������M]", 0 == que.fl.data[0]? "�����p":"�\���p");
				break;
			}
			// ���R�C�����Z�b�g�v��
			if(0 == que.fl.data[0])p->mcls_MI.gcls_pCoil->SetEvNextCoilKen();		// �����p
			if(1 == que.fl.data[0])p->mcls_MI.gcls_pCoil->SetEvNextCoilDsp();		// �\���p
			break;

//-----------------------------------------------------------------------------------------------
		case FACT_TO_DEFECT_12:								// �O�H���r��񏑍��݊����ʒm (��TO_MAEDAT)
			pLOG(em_MSG), "[MAIL] �O�H���r�����݊����ʒm (FACT_TO_DEFECT_12) [����No=%.4s, �R�C��No=%s]", que.mix.cdata[0], que.mix.cdata[1]);

			// ���C���C���X�^���X�֒ʒm
			p->mcls_MI.SetDeliveryMail(E_DEF_ML_MAE_WRITE, &que);
			break;

//-----------------------------------------------------------------------------------------------
		case FACT_TO_DEFECT_13:								// ���я������ݏ󋵖₢���킹�ʒm (��TO_PUTDAT)
			pLOG(em_MSG), "[MAIL] ���я������ݏ󋵖⍇�� (FACT_TO_DEFECT_13)  [�R�C��No=%s][�Ǘ�No=%s]", que.mix.cdata[0], que.mix.cdata[1]);

			// ���C���C���X�^���X�֒ʒm
			p->mcls_MI.SetDeliveryMail(E_DEF_ML_DB_ADD_SYNC, &que);
			break;

//-----------------------------------------------------------------------------------------------
		case FACT_TO_DEFECT_14:								// ���I�����ԕ� (��TO_CAMERA)
			break;

//-----------------------------------------------------------------------------------------------
		case FACT_TO_DEFECT_15:								// �C�j�V�����`����M�ʒm (��TO_PROCON)
			pLOG(em_MSG), "[MAIL] �C�j�V�����`����M (FACT_TO_DEFECT_15)");

			// ���C���C���X�^���X�֒ʒm
			p->mcls_MI.SetDeliveryMail(E_DEF_ML_INITIAL, &que);
			break;
			
//-----------------------------------------------------------------------------------------------
		case FACT_TO_DEFECT_20:								// �{�C�X�x��o�͉����v��
			pLOG(em_MSG), "[MAIL] �{�C�X�x��o�͉����v�� (FACT_TO_DEFECT_20) [%s]", (0==que.fl.data[0] ? "�ݔ��ُ�x�����": "���׌��o�x�����") );

			// ���C���C���X�^���X�֒ʒm
			p->mcls_MI.SetDeliveryMail( E_DEF_ML_ALAMECANCEL, &que);
			break;

//-----------------------------------------------------------------------------------------------
		case FACT_TO_DEFECT_21:								// �r���J�n�v�� (��SO_GAMENN)
			bWk = (0 == que.fl.data[2] ? false : true);
			pLOG(em_MSG), "[MAIL] �r���J�n�v�� (FACT_TO_DEFECT_21) [%s, %s, s=%d:%d, p=%d:%d, ���W�b�N�p������PC%d, �J��������=%d:%d]",
				DivNameManager::GetDivUnten(que.fl.data[0]), DivNameManager::GetDivPlg(que.fl.data[1]), que.fl.data[3], que.fl.data[4], que.fl.data[5], que.fl.data[6], que.fl.data[7] + 1, que.fl.data[8], que.fl.data[9]);
			syslog(247, "[%s; %s; s=%d:%d; p=%d:%d, ���W�b�N�p������PC%d, �J��������=%d:%d]", DivNameManager::GetDivUnten(que.fl.data[0]), DivNameManager::GetDivPlg(que.fl.data[1]), que.fl.data[3], que.fl.data[4], que.fl.data[5], que.fl.data[6], que.fl.data[7] + 1, que.fl.data[8], que.fl.data[9]);

			// ���C���C���X�^���X�֒ʒm
			p->mcls_MI.SetDeliveryMail(E_DEF_ML_RESUME_START, &que);
			break;

//-----------------------------------------------------------------------------------------------
		case FACT_TO_DEFECT_22:								// �r����~�v�� (��SO_GAMENN)
			pLOG(em_MSG), "[MAIL] �r����~�v�� (FACT_TO_DEFECT_22) [������~�敪=���W�b�N�p������PC%d]", que.fl.data[0] + 1);
			syslog(248, "������~�敪=���W�b�N�p������PC%d", que.fl.data[0] + 1);

			// ���C���C���X�^���X�֒ʒm
			p->mcls_MI.SetDeliveryMail(E_DEF_ML_RESUME_STOP, &que);
			break;

//-----------------------------------------------------------------------------------------------
		case FACT_TO_DEFECT_23:								// �r���J�n�A���T�[ (��HT_DEFECT)
			pLOG(em_MSG), "[MAIL] �r���J�n�A���T�[ (FACT_TO_DEFECT_23) [PCID=%d ����=%d �r���Q���t���[��No=%d]", que.fl.data[1], que.fl.data[2], que.fl.data[3]);

			// ���C���C���X�^���X�֒ʒm
			p->mcls_MI.SetDeliveryMail(E_DEF_ML_RESUME_START_ANSWER, &que);
			break;

//-----------------------------------------------------------------------------------------------
		case FACT_TO_DEFECT_24:								// �r����~�A���T�[ (��HT_DEFECT)
			pLOG(em_MSG), "[MAIL] �r����~�A���T�[ (FACT_TO_DEFECT_24) [PCID=%d ����=%d �敪=%d]", que.fl.data[1], que.fl.data[2], que.fl.data[3]);

			// ���C���C���X�^���X�֒ʒm
			p->mcls_MI.SetDeliveryMail(E_DEF_ML_RESUME_STOP_ANSWER, &que);
			break;

//-----------------------------------------------------------------------------------------------
		case FACT_TO_DEFECT_30:								// �J���������N�X�C�b�`�ݒ芮���ʒm
			pLOG(em_MSG), "[MAIL] �J���������N�X�C�b�`�ݒ芮���ʒm (FACT_TO_DEFECT_30)");

			// ���C���C���X�^���X�֒ʒm
			p->mcls_MI.SetDeliveryMail( E_DEF_ML_LINKSW_ANSWER, &que );
			break;

//-----------------------------------------------------------------------------------------------
		case FACT_TO_DEFECT_31:								// �r���Q���ݒ芮���ʒm (��TO_LINKSW)
			pLOG(em_MSG), "[MAIL] �r���Q���ݒ芮���ʒm (FACT_TO_DEFECT_31) [�\���敪=%d �r���Q���t���[��No=%d]", que.fl.data[0], que.fl.data[1]);

			// ���C���C���X�^���X�֒ʒm
			p->mcls_MI.SetDeliveryMail( E_DEF_ML_LINKSW_JOIN_ANSWER, &que);
			break;

//-----------------------------------------------------------------------------------------------
		case FACT_TO_DEFECT_32:								// �J�����I��ݒ芮���ʒm (��TO_LINKSW)
			pLOG(em_MSG), "[MAIL] �J�����I��ݒ芮���ʒm (FACT_TO_DEFECT_32) [�X�C�b�`FPGA�{�[�hID=%d, �t���[���O���o�[�{�[�hNo=%d, �J����No=%d, Master�^Slave�ݒ�=%d, �ݒ茋��=%d]",
				que.fl.data[0], que.fl.data[1], que.fl.data[2], que.fl.data[3], que.fl.data[4]);

			// ���C���C���X�^���X�֒ʒm
			p->mcls_MI.SetDeliveryMail(E_DEF_ML_LINKSW_CAMSET_ANSWER, &que);
			break;

//-----------------------------------------------------------------------------------------------
		case FACT_KS_DB_STATEUS:							// DB��ԕύX�ʒm(��KS_SLAVE)
			if( 1 == que.fl.data[0] ) {
				if( ! p->mcls_MI.gcls_Status.IsRun() ) break;
				pLOG(em_ERR), "[MAIL] DB�ُ�ɂ�� ������~" );
				syslog(233, "");

				// DB���ُ� (��ōŒጸ�`�F�b�N���Ă���ׁA�ʏ킠�肦�Ȃ�)
				p->mcls_MI.gcls_Status.AddQueMyErrWar(true, ON_DB_ACCESS_ERR);
			}
			break;

//-----------------------------------------------------------------------------------------------
		case FACT_KS_PCNAME_CHANGE:							// PC���̕ύX�ʒm (��KS_SLAVE)
			pLOG(em_MSG), "[MAIL] �p�����[�^�ύX�v�� (FACT_TO_DEFECT_01) [�e�[�u��=%d]", que.fl.data[0]);
			// ���C���C���X�^���X�֒ʒm
			p->mcls_MI.SetDeliveryMail(E_DEF_ML_PARAM, &que);
			break;
			//*/
		case FACT_KS_END:									// �I���v��
															// �e�v���Z�X�I������
			p->ExitProcess();
			return 0;

		default:
			syslog(SYSNO_MAILNO_ERR, "[EventNo=%d]", que.nEventNo);
			break;
		}
	}

	return 0;
}

//�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P
// ��ʍX�V�^�C�}
//�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q
void CTO_DEFECTDlg::OnTimer(UINT_PTR nIDEvent)
{
	CString	ss, sWk;
	int		nWk;
	int		nRow;

	//==================================================
	// �X�e�[�^�X���
	mcnt_pListSt->SetItem(0, 1, DivNameManager::GetDivKadou(mcls_MI.gcls_Status.GetKadou()));
	mcnt_pListSt->SetItem(1, 1, DivNameManager::GetDivSogyo(mcls_MI.gcls_Status.GetSogyo()));
#ifdef DSP_SOGYO_CHECK
	mcnt_pListSt->SetItem(2, 1, DivNameManager::GetDivSogyo(mcls_MI.gcls_Status.GetSogyoDsp()));
#endif
	mcnt_pListSt->SetItem(3, 1, DivNameManager::GetDivUnten(mcls_MI.gcls_Status.GetUnten()));
	mcnt_pListSt->SetItem(4, 1, DivNameManager::GetDivPlg(mcls_MI.gcls_Status.GetPlg()));
	mcnt_pListSt->SetItem(5, 1, mcls_MI.gcls_Status.GetMenModeName(mcls_MI.gcls_Status.GetMenMode()));

	//==================================================
	// �V�X�e�����
	for (int ii = 0; ii<NUM_MEN; ii++) {
		mcnt_pListSys->SetItem(0, 1 + ii, GetDivSys(mcls_MI.gcls_Status.GetSys(ii)));
		mcnt_pListSys->SetItem(1, 1 + ii, DivNameManager::GetDivKensa(mcls_MI.gcls_Status.GetKensa(ii)));
		mcnt_pListSys->SetItem(2, 1 + ii, DivNameManager::GetDivKiki(mcls_MI.gcls_Status.GetKiki(ii)));

		// ���菈������
		ss.Format("%0.3f", mcls_MI.gcls_pHtRecv[ii]->GetAveTime());	mcnt_pListSys->SetItem(3, 1 + ii, ss);
	}

	//==================================================
	// ����
	nRow = 0;
//	int nVCnt = NUM_CAM_POS / NUM_HANTEI;		// �������1�ɂ������āA���_��������邩�H  (���̂܂ܕ��ʂ̃Ӄւɂ��g����I)
	int nVCnt = NUM_CAMLINK_BOARD;
	for (int ii = 0; ii<NUM_MEN; ii++) {
		for (int jj = 0; jj<NUM_CAM_POS; jj++) {
			if ((NUM_HANTEI * NUM_CAMLINK_BOARD) > jj)
			{
				// ��������P��
				mcnt_pListHt->SetItem(nRow, 1, mcls_MI.gcls_Status.GetHtID(ii, (jj / nVCnt)));
				if (0 == (jj % nVCnt))
				{
					ss = (mcls_MI.gcls_pHtRecv[ii]->IsConnect((jj / nVCnt) + 1) ? "��" : "�~");				mcnt_pListHt->SetItem(nRow, 2, ss);
					ss = DivNameManager::GetDivBool(mcls_MI.gcls_Status.GetHtState(ii, (jj / nVCnt)));		mcnt_pListHt->SetItem(nRow, 3, ss);
				}
				else
				{
					mcnt_pListHt->SetItem(nRow, 2, "");
					mcnt_pListHt->SetItem(nRow, 3, "");
				}

				// �_������P��
				mcnt_pListHt->SetItem(nRow, 4, mcls_MI.gcls_pHtRecv[ii]->GetFrameNo(jj));	// Fno	
				//mcnt_pListHt->SetItem(nRow, 5, (int)mcls_MI.gcls_EdgePosCont.GetEdgePos(ii, jj) );	// �G�b�W

				for (int kk = 0; kk < 3; kk++)
				{
					//mcnt_pListHt->SetItem(nRow, 5 + (2 * kk) + 0, mcls_MI.gcls_pHtRecv[ii]->GetBr(jj, kk));
					mcnt_pListHt->SetItem(nRow, 5 + (4 * kk) + 0, "");
					mcnt_pListHt->SetItem(nRow, 6 + (4 * kk) + 0, mcls_MI.gcls_pHtRecv[ii]->GetBr(jj, kk));
					mcnt_pListHt->SetItem(nRow, 7 + (4 * kk) + 0, mcls_MI.gcls_Status.GetExpTime((ii + 1)*jj, kk));
					mcnt_pListHt->SetItem(nRow, 8 + (4 * kk) + 0, mcls_MI.gcls_Status.GetCamGain((ii + 1)*jj, kk));
				}
			}
			else
			{
				mcnt_pListHt->SetItem(nRow, 1, "");
				mcnt_pListHt->SetItem(nRow, 2, "");
				mcnt_pListHt->SetItem(nRow, 3, "");
				mcnt_pListHt->SetItem(nRow, 4, "");
				for (int kk = 0; kk<3; kk++) {
					mcnt_pListHt->SetItem(nRow, 5 + (4 * kk) + 0, "");
					mcnt_pListHt->SetItem(nRow, 6 + (4 * kk) + 0, mcls_MI.gcls_pHtRecv[ii]->GetBr(jj, kk));
					mcnt_pListHt->SetItem(nRow, 7 + (4 * kk) + 0, mcls_MI.gcls_Status.GetExpTime((ii + 1)*jj, kk));
					mcnt_pListHt->SetItem(nRow, 8 + (4 * kk) + 0, mcls_MI.gcls_Status.GetCamGain((ii + 1)*jj, kk));
				}
			}

			nRow++;
		}
	}
#ifdef ENABLE_CYCLE
	nRow = 0;
	nVCnt = NUM_CAM_POS / NUM_HANTEI_CYCLE;		// �������1�ɂ������āA���_��������邩�H  (���̂܂ܕ��ʂ̃Ӄւɂ��g����I)
	for (int ii = 0; ii<NUM_MEN; ii++) {
		for (int jj = 0; jj<NUM_CAM_POS; jj++) {
			mcnt_pListCy->SetItem(nRow, 1, mcls_MI.gcls_Status.GetCyID(ii, (jj / nVCnt)));
			// ��������P��
			if (0 == (jj % nVCnt)) {
				ss = (mcls_MI.gcls_pCyRecv[ii]->IsConnect((jj / nVCnt) + 1) ? "��" : "�~");				mcnt_pListCy->SetItem(nRow, 2, ss);
				ss = DivNameManager::GetDivBool(mcls_MI.gcls_Status.GetCyState(ii, (jj / nVCnt)));		mcnt_pListCy->SetItem(nRow, 3, ss);
			}
			else {
				mcnt_pListCy->SetItem(nRow, 2, "");
				mcnt_pListCy->SetItem(nRow, 3, "");
			}

			// �_������P��
			mcnt_pListCy->SetItem(nRow, 4, mcls_MI.gcls_pCyRecv[ii]->GetFrameNo(jj));	// Fno	
			nRow++;
		}
	}
#endif
	//==================================================
	// ���W�b�N�p������
	nRow = 0;
	for (int ii = 0; ii < NUM_ALL_HTPARA; ii++)
	{
		mcnt_pListPr->SetItem(nRow, 1, mcls_MI.gcls_Status.GetHtParaID(ii));
		ss = DivNameManager::GetDivBool(mcls_MI.gcls_Status.GetHtParaState(ii));
		mcnt_pListPr->SetItem(nRow, 2, ss);
		for (int jj = 0; jj < NUM_CAMLINK_BOARD; jj++)
		{
			mcnt_pListPr->SetItem(nRow, 3 + jj, LogicParaRunFunc::GetHtParaCamNo(ii, jj));
		}
		mcnt_pListPr->SetItem(nRow, 3 + NUM_CAMLINK_BOARD, GetDivSys(mcls_MI.gcls_Status.GetParaSys(ii)));
		nRow++;
	}

	//==================================================
	// ���菈������
	//  1. TO_DEFECT	1�t���[���捞����(PLG)
	//  2. ncldetect	�摜��荞��
	//  3. ncldetect	�摜����
	//  4. HT_DEFECT	�����ɑ��M
	//  5. TO_DEFECT	�t���[������M(�t���[������1�Ԗڎ�M)
	//  6. TO_DEFECT	�t���[������M(�t���[������2�Ԗڎ�M)
	//  7. TO_DEFECT	�t���[������M(�S��������M)
	//  8. TO_DEFECT	��M����(�r���ۑ�, �}�[�L���O�w��)
	//  9. TO_DEFECT	1�t���[����������

	for (int ii = 0; ii<NUM_MEN; ii++) {
		// [DS HT]  �摜�����G���W�����������`�����`���J�n�܂ł̕��ρA�ő及�v����[ms]			(No.4)
		ss.Format("%0.2f", mcls_MI.gcls_pHtRecv[ii]->GetNcHtProcAveTime(0));	mcnt_pListHtTime->SetItem(0, (ii * 2) + 1, ss);
		ss.Format("%0.2f", mcls_MI.gcls_pHtRecv[ii]->GetNcHtProcMaxTime(0));	mcnt_pListHtTime->SetItem(0, (ii * 2) + 2, ss);
		// [WS HT]
		ss.Format("%0.2f", mcls_MI.gcls_pHtRecv[ii]->GetNcHtProcAveTime(1));	mcnt_pListHtTime->SetItem(1, (ii * 2) + 1, ss);
		ss.Format("%0.2f", mcls_MI.gcls_pHtRecv[ii]->GetNcHtProcMaxTime(1));	mcnt_pListHtTime->SetItem(1, (ii * 2) + 2, ss);

		// [�`��1]  �t���[����1�Ԗځ`2�Ԗڎ�M�܂ł̕��ρA�ő及�v����[ms]						(No.5-6)
		ss.Format("%0.2f", mcls_MI.gcls_pHtRecv[ii]->GetRecvAveTime());			mcnt_pListHtTime->SetItem(2, (ii * 2) + 1, ss);
		ss.Format("%0.2f", mcls_MI.gcls_pHtRecv[ii]->GetRecvMaxTime());			mcnt_pListHtTime->SetItem(2, (ii * 2) + 2, ss);

		// [�`��2]  �t���[����1�Ԗځ`�S������M�܂ł̕��ρA�ő及�v����[ms]						(No.5-7)
		ss.Format("%0.2f", mcls_MI.gcls_pHtRecv[ii]->GetRecvTotalAveTime());	mcnt_pListHtTime->SetItem(3, (ii * 2) + 1, ss);
		ss.Format("%0.2f", mcls_MI.gcls_pHtRecv[ii]->GetRecvTotalMaxTime());	mcnt_pListHtTime->SetItem(3, (ii * 2) + 2, ss);

		// [HT-TO]  �摜�捞�`���聨�����`�������܂ł̕��ρA�ő及�v����[ms]					(No.4-7)
		ss.Format("%0.2f", mcls_MI.gcls_pHtRecv[ii]->GetHtProcAveTime());		mcnt_pListHtTime->SetItem(4, (ii * 2) + 1, ss);
		ss.Format("%0.2f", mcls_MI.gcls_pHtRecv[ii]->GetHtProcMaxTime());		mcnt_pListHtTime->SetItem(4, (ii * 2) + 2, ss);

		// [TO]     �����`���������������������܂ł̕��C�A�ő及�v����[ms]						(No.8-9)
		ss.Format("%0.2f", mcls_MI.gcls_pHtRecv[ii]->GetAveTime());				mcnt_pListHtTime->SetItem(5, (ii * 2) + 1, ss);
		ss.Format("%0.2f", mcls_MI.gcls_pHtRecv[ii]->GetMaxTime());				mcnt_pListHtTime->SetItem(5, (ii * 2) + 2, ss);

		// [İ��]   �摜�捞�`�������������܂ł̕��ρA�ő及�v����[ms]							(No.4-9)
		ss.Format("%0.2f", mcls_MI.gcls_pHtRecv[ii]->GetToProcAveTime());		mcnt_pListHtTime->SetItem(6, (ii * 2) + 1, ss);
		ss.Format("%0.2f", mcls_MI.gcls_pHtRecv[ii]->GetToProcMaxTime());		mcnt_pListHtTime->SetItem(6, (ii * 2) + 2, ss);

	}

	//==================================================
	// PLG���
	nWk = mcls_MI.gcls_pCoil->GetKenRecBase();
	if (0>nWk) nWk = 0;		// �������J�n����-1�ׂ̈̏��u
	double	dCoilZan = mcls_MI.gcls_pCoil->GetCoilIndex(nWk, 0)->data.nLenIn - (mcls_MI.gcls_Plg.GetLen_KenDist()) / 1000.0;

	// ������_
	ss.Format("%0.3f", mcls_MI.gcls_Plg.GetLen_Ken(0) / 1000.0);			mcnt_pListPlgKen->SetItem(0, 1, ss);
	ss.Format("%0.3f", mcls_MI.gcls_Plg.GetLen_Ken(1) / 1000.0);			mcnt_pListPlgKen->SetItem(1, 1, ss);
	ss.Format("%0.3f", mcls_MI.gcls_Plg.GetLen_KenSub() / 1000.0);		mcnt_pListPlgKen->SetItem(2, 1, ss);
	ss.Format("%0.3f", dCoilZan);										mcnt_pListPlgKen->SetItem(3, 1, ss);
	ss.Format("%0.3f", mcls_MI.gcls_Plg.GetCoilOldLen_Ken() / 1000.0);	mcnt_pListPlgKen->SetItem(4, 1, ss);

	// �\����_(�V���[�ʒu)
	ss.Format("%0.3f", mcls_MI.gcls_Plg.GetLen_BaseDsp() / 1000.0);			mcnt_pListPlgDsp->SetItem(0, 1, ss);
	ss.Format("%0.3f", mcls_MI.gcls_Plg.GetLen_Dsp(0) / 1000.0);			mcnt_pListPlgDsp->SetItem(1, 1, ss);
	ss.Format("%0.3f", mcls_MI.gcls_Plg.GetLen_Dsp(1) / 1000.0);			mcnt_pListPlgDsp->SetItem(2, 1, ss);
	ss.Format("%0.3f", mcls_MI.gcls_Plg.GetLen_Dsp(2) / 1000.0);			mcnt_pListPlgDsp->SetItem(3, 1, ss);
	ss.Format("%0.3f", mcls_MI.gcls_Plg.GetLen_OutCut() / 1000.0);			mcnt_pListPlgDsp->SetItem(4, 1, ss);
	ss.Format("%0.3f", mcls_MI.gcls_Plg.GetCoilOldLen_BaseDsp() / 1000.0);	mcnt_pListPlgDsp->SetItem(5, 1, ss);


	// PLG ��{���
	ss.Format("%d", mcls_MI.gcls_Plg.GetFrameNo());						mcnt_pListPlg->SetItem(0, 1, ss);
	ss.Format("%d", mcls_MI.gcls_Plg.GetFrameIntervalTime());			mcnt_pListPlg->SetItem(1, 1, ss);
	ss.Format("%0.3f", mcls_MI.gcls_Plg.GetDistance());					mcnt_pListPlg->SetItem(2, 1, ss);
	ss.Format("%d", mcls_MI.gcls_Plg.GetSpeed());						mcnt_pListPlg->SetItem(3, 1, ss);
	ss.Format("%0.6f", mcls_MI.gcls_Plg.GetResV());						mcnt_pListPlg->SetItem(4, 1, ss);
#ifdef RESV_KOTEI
	ss.Format("%0.6f", mcls_MI.gcls_Plg.GetResVCycle());				mcnt_pListPlg->SetItem(5, 1, ss);
#endif
	ss.Format("%d", mcls_MI.gcls_pSogyo->GetSpeed());					mcnt_pListPlg->SetItem(6, 1, ss);
#ifdef DSP_SOGYO_CHECK
	ss.Format("%d", mcls_MI.gcls_pSogyo->GetSpeedDsp());				mcnt_pListPlg->SetItem(7, 1, ss);
#endif
	ss.Format("%u", mcls_MI.gcls_Dmem.GetDmRecv().ken.nFrameNo);		mcnt_pListPlg->SetItem(8, 1, ss);
	ss.Format("%u", mcls_MI.gcls_Dmem.GetDmRecv().ken.nLvalAdd);		mcnt_pListPlg->SetItem(9, 1, ss);
	ss.Format("%u", mcls_MI.gcls_Dmem.GetDmRecv().ken.nPlgAdd);			mcnt_pListPlg->SetItem(10, 1, ss);

	//======================================================
	// �R�C�����֘A����
	// ------------------>>>
	mcls_MI.gcls_pCoil->SetCsLock();		// �N���e�B�J���Z�N�V���� ���b�N 	
											//// �����p�R�C�����
											// ��
	nRow = 0;
	mcnt_pListCoil->SetItem(nRow, 1, mcls_MI.gcls_pCoil->GetCoilNextKen()->rec);
	mcnt_pListCoil->SetItem(nRow, 2, mcls_MI.gcls_pCoil->GetCoilNextKen()->cKizukenNo);
	mcnt_pListCoil->SetItem(nRow, 3, mcls_MI.gcls_pCoil->GetCoilNextKen()->data.cMeiNo);
	mcnt_pListCoil->SetItem(nRow, 4, mcls_MI.gcls_pCoil->GetCoilNextKen()->data.cCoilNoIn);
	mcnt_pListCoil->SetItem(nRow, 5, mcls_MI.gcls_pCoil->GetCoilNextKen()->data.nLenIn);
	mcnt_pListCoil->SetItem(nRow, 6, mcls_MI.gcls_pCoil->GetCoilNextKen()->data.nWid);

	for (int jj = 0; jj<NUM_MEN; jj++) {
		mcnt_pListCoil->SetItem(nRow, 7 + jj, mcls_MI.gcls_pCoil->GetCoilNextKen()->setting.scode[jj]);
		mcnt_pListCoil->SetItem(nRow, 9 + jj, mcls_MI.gcls_pCoil->GetCoilNextKen()->setting.pcode[jj]);
	}
	mcnt_pListCoil->SetItem(nRow, 11, mcls_MI.gcls_pCoil->GetCoilNextKen()->nSt_Spm);
	// ��, �O1, �O2, �O3
	nWk = mcls_MI.gcls_pCoil->GetKenRecBase();
	if (0>nWk) nWk = 0;		// �������J�n����-1�ׂ̈̏��u
	for (int ii = 0; ii<SIZE_COIL_BUF; ii++) {
		nRow = 1 + ii;
		mcnt_pListCoil->SetItem(nRow, 1, mcls_MI.gcls_pCoil->GetCoilIndex(nWk, ii)->rec);
		mcnt_pListCoil->SetItem(nRow, 2, mcls_MI.gcls_pCoil->GetCoilIndex(nWk, ii)->cKizukenNo);
		mcnt_pListCoil->SetItem(nRow, 3, mcls_MI.gcls_pCoil->GetCoilIndex(nWk, ii)->data.cMeiNo);
		mcnt_pListCoil->SetItem(nRow, 4, mcls_MI.gcls_pCoil->GetCoilIndex(nWk, ii)->data.cCoilNoIn);
		mcnt_pListCoil->SetItem(nRow, 5, mcls_MI.gcls_pCoil->GetCoilIndex(nWk, ii)->data.nLenIn);
		mcnt_pListCoil->SetItem(nRow, 6, mcls_MI.gcls_pCoil->GetCoilIndex(nWk, ii)->data.nWid);
		for (int jj = 0; jj < NUM_MEN; jj++)
		{
			int nScode = (0 != mcls_MI.gcls_pCoil->GetCoilIndex(nWk, ii)->nSt_Spm ? mcls_MI.gcls_pCoil->GetCoilIndex(nWk, ii)->setting.scode[jj] : mcls_MI.gcls_pCoil->GetCoilIndex(nWk, ii)->setting.scode_spmoff[jj]);
			int nPcode = (0 != mcls_MI.gcls_pCoil->GetCoilIndex(nWk, ii)->nSt_Spm ? mcls_MI.gcls_pCoil->GetCoilIndex(nWk, ii)->setting.pcode[jj] : mcls_MI.gcls_pCoil->GetCoilIndex(nWk, ii)->setting.pcode_spmoff[jj]);
			mcnt_pListCoil->SetItem(nRow, 7 + jj, nScode);
			mcnt_pListCoil->SetItem(nRow, 9 + jj, nPcode);
		}
		mcnt_pListCoil->SetItem(nRow, 11, mcls_MI.gcls_pCoil->GetCoilIndex(nWk, ii)->nSt_Spm);
	}

	////// �o���R�C�����
	//nRow = 0;
	//nWk = mcls_MI.gcls_pCoil->GetShaRecBase();

	//if (0>nWk) nWk = 0;		//  �������J�n����-1�ׂ̈̏��u
	//for (int ii = 0; ii<SIZE_COIL_OUT_BUF; ii++) {
	//	nRow = (ii * 2);
	//	//ss.Format("%d", mcls_MI.gcls_pCoil->GetShaRecIndex(nWk, ii));						mcnt_pListOutCoil->SetItem(nRow, 1, ss);
	//	ss.Format("N%d", mcls_MI.gcls_pCoil->GetCoilOutIndex(nWk, ii)->nCnt);				mcnt_pListOutCoil->SetItem(nRow + 1, 1, ss);
	//	ss.Format("%s", mcls_MI.gcls_pCoil->GetCoilOutIndex(nWk, ii)->cOutKizukenNo);		mcnt_pListOutCoil->SetItem(nRow, 2, ss);
	//	for (int jj = 0; jj<MAX_SHALINK_COUNT / 2; jj++) {
	//		if (NULL != mcls_MI.gcls_pCoil->GetCoilOutIndex(nWk, ii)->incoil[jj].pCoilIn) {
	//			ss.Format("%s", mcls_MI.gcls_pCoil->GetCoilOutIndex(nWk, ii)->incoil[jj].pCoilIn->data.cCoilNoIn);	mcnt_pListOutCoil->SetItem(nRow, 3 + jj, ss);
	//			ss.Format("%0.4f", mcls_MI.gcls_pCoil->GetCoilOutIndex(nWk, ii)->incoil[jj].nLength / 1000.0);		mcnt_pListOutCoil->SetItem(nRow + 1, 3 + jj, ss);
	//		}
	//		else {
	//			mcnt_pListOutCoil->SetItem(nRow, 3 + jj, "");
	//			mcnt_pListOutCoil->SetItem(nRow + 1, 3 + jj, "");
	//		}
	//	}

	//	//ss.Format("N:%d", mcls_MI.gcls_pCoil->GetCoilOutIndex(nWk, ii)->nCnt);				mcnt_pListOutCoil->SetItem(nRow+1, 1, ss);
	//	ss.Format("F:%0.4f / T:%0.4f", mcls_MI.gcls_pCoil->GetCoilOutIndex(nWk, ii)->nFCutLen / 1000.0, mcls_MI.gcls_pCoil->GetCoilOutIndex(nWk, ii)->nTCutLen / 1000.0);
	//	mcnt_pListOutCoil->SetItem(nRow + 1, 2, ss);
	//}

	//======================================================
	// �ݔ��ʒu�g���b�L���O (�����ʒu+�\����_)
	mcnt_pListSetuDsp->SetItem(0, 1, mcls_MI.gcls_pCoil->GetCoilKenR(0)->rec);
	mcnt_pListSetuDsp->SetItem(1, 1, mcls_MI.gcls_pCoil->GetCoilKenR(1)->rec);
	mcnt_pListSetuDsp->SetItem(2, 1, mcls_MI.gcls_pCoil->GetCoilNextKen()->rec);
	if(NULL != mcls_MI.gcls_pCoil->GetCoilBaseDsp())
		mcnt_pListSetuDsp->SetItem(3, 1, mcls_MI.gcls_pCoil->GetCoilBaseDsp()->rec);
	mcnt_pListSetuDsp->SetItem(0, 2, mcls_MI.gcls_pCoil->GetCoilKenR(0)->data.cCoilNoIn);
	mcnt_pListSetuDsp->SetItem(1, 2, mcls_MI.gcls_pCoil->GetCoilKenR(1)->data.cCoilNoIn);
	mcnt_pListSetuDsp->SetItem(2, 2, mcls_MI.gcls_pCoil->GetCoilNextKen()->data.cCoilNoIn);
	if (NULL != mcls_MI.gcls_pCoil->GetCoilBaseDsp())
		mcnt_pListSetuDsp->SetItem(3, 2, mcls_MI.gcls_pCoil->GetCoilBaseDsp()->data.cCoilNoIn);

	// �\����_ �ȍ~
	nRow = 3;
	for (int ii = POS_DSP_P1; ii < POS_DSP_END; ii++) {
		COIL_INF const* pC = mcls_MI.gcls_pCoil->GetCoilDsp(ii);

		if (NULL != pC) {
			mcnt_pListSetuDsp->SetItem(nRow, 1, pC->rec);
			mcnt_pListSetuDsp->SetItem(nRow, 2, pC->data.cCoilNoIn);

		}
		else {
			mcnt_pListSetuDsp->SetItem(nRow, 1, -1);
			mcnt_pListSetuDsp->SetItem(nRow, 2, "");
		}
		nRow++;
	}
	mcnt_pListSetuDsp->SetItem(nRow, 2, mcls_MI.gcls_pCoil->GetCoilNextDsp(0)->CoilInf.KeyInf.cCoilNo);
	
	mcls_MI.gcls_pCoil->SetCsUnLock();		// �N���e�B�J���Z�N�V���� �A�����b�N
	
	//======================================================
	// DB���
	mcnt_pListDbInfo->SetItem(0, 1, mcls_MI.gcls_DB.GetQueData_Count());
	mcnt_pListDbInfo->SetItem(0, 2, mcls_MI.gcls_DB.GetQueData_Max());
	mcnt_pListDbInfo->SetItem(1, 1, mcls_MI.gcls_DB.GetQueDB_Count());
	mcnt_pListDbInfo->SetItem(1, 2, mcls_MI.gcls_DB.GetQueDB_Max());

	ss.Format("%0.1f", mcls_MI.gcls_DB.GetAveTime()); mcnt_pListDbInfo->SetItem(2, 1, ss);
	ss.Format("%0.1f", mcls_MI.gcls_DB.GetMaxTime()); mcnt_pListDbInfo->SetItem(2, 2, ss);
	ss.Format("%0.1f", mcls_MI.gcls_DB.GetAveSqlTime()); mcnt_pListDbInfo->SetItem(3, 1, ss);
	ss.Format("%0.1f", mcls_MI.gcls_DB.GetMaxSqlTime()); mcnt_pListDbInfo->SetItem(3, 2, ss);
	
	// BCP���
	mcnt_pListDbInfo->SetItem(4, 1, mcls_MI.gcls_DB.GetBcpSaveMgr().GetQueData_Count());
	mcnt_pListDbInfo->SetItem(4, 2, mcls_MI.gcls_DB.GetBcpSaveMgr().GetQueData_Max());
	//mcnt_pListDbInfo->SetItem(5, 1, mcls_MI.gcls_DB.GetBcpSaveMgr().GetQueDB_Count());
	//mcnt_pListDbInfo->SetItem(5, 2, mcls_MI.gcls_DB.GetBcpSaveMgr().GetQueDB_Max());

	ss.Format("%0.1f", mcls_MI.gcls_DB.GetBcpSaveMgr().GetAveTime()); mcnt_pListDbInfo->SetItem(5, 1, ss);
	ss.Format("%0.1f", mcls_MI.gcls_DB.GetBcpSaveMgr().GetMaxTime()); mcnt_pListDbInfo->SetItem(5, 2, ss);

	ss.Format("%0.1f", mcls_MI.gcls_DB.GetBcpSaveMgr().GetAttrSendRowAveTime()); mcnt_pListDbInfo->SetItem(6, 1, ss);
	ss.Format("%0.1f", mcls_MI.gcls_DB.GetBcpSaveMgr().GetAttrSendRowMaxTime()); mcnt_pListDbInfo->SetItem(6, 2, ss);
	ss.Format("%0.1f", mcls_MI.gcls_DB.GetBcpSaveMgr().GetAttrBcpBatchAveTime()); mcnt_pListDbInfo->SetItem(7, 1, ss);
	ss.Format("%0.1f", mcls_MI.gcls_DB.GetBcpSaveMgr().GetAttrBcpBatchMaxTime()); mcnt_pListDbInfo->SetItem(7, 2, ss);
	ss.Format("%0.1f", mcls_MI.gcls_DB.GetBcpSaveMgr().GetImgSendRowAveTime()); mcnt_pListDbInfo->SetItem(8, 1, ss);
	ss.Format("%0.1f", mcls_MI.gcls_DB.GetBcpSaveMgr().GetImgSendRowMaxTime()); mcnt_pListDbInfo->SetItem(8, 2, ss);
	ss.Format("%0.1f", mcls_MI.gcls_DB.GetBcpSaveMgr().GetImgBcpBatchAveTime()); mcnt_pListDbInfo->SetItem(9, 1, ss);
	ss.Format("%0.1f", mcls_MI.gcls_DB.GetBcpSaveMgr().GetImgBcpBatchMaxTime()); mcnt_pListDbInfo->SetItem(9, 2, ss);

	// DB
	for (int ii = 0; ii<DbBaseManager::SIZE_SQLINF_NUM; ii++) {
		mcnt_pListDb->SetItem(ii, 1, mcls_MI.gcls_DB.GetInf_KizukenNo(ii));
		ss.Format("%d/%d", mcls_MI.gcls_DB.GetInf_SaveCnt(ii, 0), mcls_MI.gcls_DB.GetInf_SaveCnt(ii, 1));					mcnt_pListDb->SetItem(ii, 2, ss);
		ss.Format("%d/%d", mcls_MI.gcls_DB.GetInf_TotalCnt(ii, 0), mcls_MI.gcls_DB.GetInf_TotalCnt(ii, 1));					mcnt_pListDb->SetItem(ii, 3, ss);
		ss.Format("%d/%d", mcls_MI.gcls_DB.GetInf_LowGrDeleCnt(ii, 0), mcls_MI.gcls_DB.GetInf_LowGrDeleCnt(ii, 1));			mcnt_pListDb->SetItem(ii, 4, ss);
		ss.Format("%d/%d", mcls_MI.gcls_DB.GetInf_DeleCnt(ii, 0), mcls_MI.gcls_DB.GetInf_DeleCnt(ii, 1));					mcnt_pListDb->SetItem(ii, 5, ss);
		ss.Format("%d/%d", mcls_MI.gcls_DB.GetInf_ImgLowGrDeleCnt(ii, 0), mcls_MI.gcls_DB.GetInf_ImgLowGrDeleCnt(ii, 1));	mcnt_pListDb->SetItem(ii, 6, ss);
		ss.Format("%d/%d", mcls_MI.gcls_DB.GetInf_ImgDeleCnt(ii, 0), mcls_MI.gcls_DB.GetInf_ImgDeleCnt(ii, 1));				mcnt_pListDb->SetItem(ii, 7, ss);
	}



#ifdef DEBUG_DB_TIME
	mcls_pL->Write(KizuLib::LogFileManager::em_NON, "SQL=,%d,DB=,%d,Cnt=,%d,%d",
		mcls_MI.gcls_DB.GetQueData_Count(), mcls_MI.gcls_DB.GetQueDB_Count(),
		mcls_MI.gcls_DB.GetInf_TotalCnt(0, 0), mcls_MI.gcls_DB.GetInf_TotalCnt(0, 1));
#endif


	//======================================================
	// OP���
	for(int ii=0; ii<HY_SOCK_NUM; ii++) {
		mcnt_pListOp->SetItem(ii * 2 + 0, 1, mcls_MI.gcls_pOpS[ii]->GetConnectCount());
		mcnt_pListOp->SetItem(ii * 2 + 1, 1, mcls_MI.gcls_pOpS[ii]->gque_Deli.GetCount());
		mcnt_pListOp->SetItem(ii * 2 + 1, 2, mcls_MI.gcls_pOpS[ii]->gque_Deli.GetMaxCount());
	}



	//======================================================
	// �G�b�W���o
	float dWk1, dWk2;

	mcnt_pListEdge->SetItem(0, 1, mcls_MI.gcls_EdgePosCont.GetDivEdgeMode(mcls_MI.gcls_EdgePosCont.GetEdgeMode(0)));
	mcnt_pListEdge->SetItem(1, 1, mcls_MI.gcls_EdgePosCont.GetDivEdgeMode(mcls_MI.gcls_EdgePosCont.GetEdgeMode(1)));

	mcls_MI.gcls_EdgePosCont.SearchEdgePos(0, dWk1, dWk2);
	ss.Format("%d", (int)dWk1);		mcnt_pListEdge->SetItem(0, 2, ss);
	ss.Format("%d", (int)dWk2);		mcnt_pListEdge->SetItem(0, 3, ss);
	mcls_MI.gcls_EdgePosCont.SearchEdgePos(1, dWk1, dWk2);
	ss.Format("%d", (int)dWk1);		mcnt_pListEdge->SetItem(1, 2, ss);
	ss.Format("%d", (int)dWk2);		mcnt_pListEdge->SetItem(1, 3, ss);

	mcls_MI.gcls_EdgePosCont.SearchEdgeRawPos(0, dWk1, dWk2);
	ss.Format("%d", (int)dWk1);		mcnt_pListEdge->SetItem(0, 4, ss);
	ss.Format("%d", (int)dWk2);		mcnt_pListEdge->SetItem(0, 5, ss);
	mcls_MI.gcls_EdgePosCont.SearchEdgeRawPos(1, dWk1, dWk2);
	ss.Format("%d", (int)dWk1);		mcnt_pListEdge->SetItem(1, 4, ss);
	ss.Format("%d", (int)dWk2);		mcnt_pListEdge->SetItem(1, 5, ss);
	
	//======================================================
	//// �@����֘A
	mcnt_pListKiki->SetItem(0, 1, GetSeqStat((EM_DIV_SEQ_STAT)mcls_MI.gcls_Status.GetStatusRead().SeqState.stat));
	mcnt_pListKiki->SetItem(1, 1, GetSeqKadou((EM_DIV_SEQ_KADOU)mcls_MI.gcls_Status.GetStatusRead().SeqKadou.stat));
	mcnt_pListKiki->SetItem(2, 1, GetDaisyaPos((DIV_DAISYA_POS)mcls_MI.gcls_Status.GetStatusRead().DaisyaSt[0].stat));
	mcnt_pListKiki->SetItem(3, 1, GetDaisyaPos((DIV_DAISYA_POS)mcls_MI.gcls_Status.GetStatusRead().DaisyaSt[1].stat));
	mcnt_pListKiki->SetItem(4, 1, GetSpmStat((EM_DIV_SPM_STAT)mcls_MI.gcls_Status.GetSpmStat()));
	mcnt_pListKiki->SetItem(5, 1, GetSprayStat((DIV_SPRAY_STAT)mcls_MI.gcls_Status.GetStatusRead().ToyuSprSt.stat));

	ss.Format("%s/%s/%s", GetChilerStop((DIV_CHILER_STOP)mcls_MI.gcls_Status.GetStatusRead().ChilerPiStop[0].stat),
		GetChilerStop((DIV_CHILER_STOP)mcls_MI.gcls_Status.GetStatusRead().ChilerPiStop[1].stat),
		GetChilerStop((DIV_CHILER_STOP)mcls_MI.gcls_Status.GetStatusRead().ChilerPiStop[2].stat));
	mcnt_pListKiki->SetItem(6, 1, ss);
	ss.Format("%s/%s/%s", GetChilerErr((DIV_CHILER_ERR)mcls_MI.gcls_Status.GetStatusRead().ChilerPiErr[0].stat),
		GetChilerErr((DIV_CHILER_ERR)mcls_MI.gcls_Status.GetStatusRead().ChilerPiErr[1].stat),
		GetChilerErr((DIV_CHILER_ERR)mcls_MI.gcls_Status.GetStatusRead().ChilerPiErr[2].stat));
	mcnt_pListKiki->SetItem(7, 1, ss);
	for (int ii = 0; ii < NUM_MEN; ii++) {
		ss.Format("%s/%s", GetDryerStop((DIV_DRYER_STOP)mcls_MI.gcls_Status.GetStatusRead().airblow[ii].AirBlowPiStop.stat),
			GetDryerErr((DIV_DRYER_ERR)mcls_MI.gcls_Status.GetStatusRead().airblow[ii].AirBlowPiErr.stat));
		mcnt_pListKiki->SetItem(8 + ii, 1, ss);
	}
	ss.Format("%s/%s", GetDoorStat((DIV_DOOR_STAT)mcls_MI.gcls_Status.GetStatusRead().SecurityDoor.stat),
		GetKeyStat((DIV_KEY_STAT)mcls_MI.gcls_Status.GetStatusRead().SecurityKey.stat));
	mcnt_pListKiki->SetItem(10, 1, ss);

	//======================================================
	//// �{�C�X���
	if (true) {
		nWk = mcls_MI.gcls_Dmem.GetVoiceMen();
		if (0 == nWk)		ss = "�\";
		else if (1 == nWk)	ss = "��";
		else				ss = "";
		mcnt_pListVoice->SetItem(0, 0, ss);
		mcnt_pListVoice->SetItem(0, 1, GetVoiceTypeName( mcls_MI.gcls_Dmem.GetVoiceSts()));
	}
	//======================================================
	//// �r�ڋ߃����v���
	if (true) {
		for (int ii = 0; ii < 3; ii++) {
			for (int jj = 0; jj < 8; jj++) {
				if (2 > ii)
				{
					nWk = mcls_MI.gcls_Dmem.GetRampSts(ii, jj);
				}
				else
				{
					nWk = mcls_MI.gcls_Dmem.GetAlrmSts(jj);
				}
				if (0 == nWk)		ss = "��";
				else if (1 == nWk)	ss = "�d";
				else				ss = "";
				mcnt_pListRamp->SetItem(ii, jj + 1, ss);
			}
		}
	}

	//======================================================
	//// �p�g���C�g���
	if (true) {
		for (int ii = 0; ii < 2; ii++) {
			for (int jj = 0; jj < 2; jj++) {
				mcnt_pListPato->SetItem(ii, jj+1, mcls_MI.gcls_Dmem.GetPatoSts(ii, jj));
			}
		}
		for (int ii = 0; ii < 2; ii++)
		{
			int nOutputCnt = 0;
			for (int jj = 0; jj < 2; jj++)
			{
				nOutputCnt += mcls_MI.gcls_Dmem.GetPatoSts(jj, ii);
			}
			mcnt_pListPato->SetItem(2, ii + 1, nOutputCnt);
		}
	}

	//======================================================
	//// �f�[�^������
	if (true) {
		//mcnt_pListDmem->SetItem(0, 1, (mcls_MI.gcls_Dmem.GetIjmPb() ? "��" : "�~"));				// �}�[�L���OPB���
		//mcnt_pListDmem->SetItem(1, 1, (mcls_MI.gcls_Dmem.GetIjmAllow() ? "��" : "�~"));				// �}�[�N�w�ߋ��M�����
		//mcnt_pListDmem->SetItem(2, 1, (mcls_MI.gcls_Dmem.GetIjmHealthy() ? "��" : "�~"));			// �}�[�N�Ď����u�w���V�[���
		//mcnt_pListDmem->SetItem(3, 1, (mcls_MI.gcls_Dmem.GetIjmKado() ? "��" : "�~"));				// �}�[�N�Ď����u �ғ���Ԍ�����
		//mcnt_pListDmem->SetItem(4, 1, (mcls_MI.gcls_Dmem.GetIjmKensa() ? "��" : "�~"));				// �}�[�N�Ď����u ������Ԉُ�
		//mcnt_pListDmem->SetItem(5, 1, (mcls_MI.gcls_Dmem.GetIjmKiki() ? "��" : "�~"));				// �}�[�N�Ď����u �@���Ԉُ�
		//mcnt_pListDmem->SetItem(0, 3, (mcls_MI.gcls_Dmem.GetIjmWarning() ? "��" : "�~"));			// �}�[�N�x��
		//mcnt_pListDmem->SetItem(1, 3, (mcls_MI.gcls_Dmem.GetIjmError() ? "��" : "�~"));				// �}�[�N�ُ�
		//mcnt_pListDmem->SetItem(2, 3, (mcls_MI.gcls_Dmem.GetIjmEnable() ? "��" : "�~"));			// �}�[�N�󎚉\
		//mcnt_pListDmem->SetItem(3, 3, (1 == mcls_MI.gcls_Status.GetStatusRead().IjmUse.stat ? "�L��" : "����"));		// �}�[�N�g�p�L��
		//mcnt_pListDmem->SetItem(4, 3, (0 == mcls_MI.gcls_Status.GetStatusRead().IjmNirekoKado.stat ? "��" : "�~"));		// �j���R�Z���T�[�ғ���
		//mcnt_pListDmem->SetItem(5, 3, (0 == mcls_MI.gcls_Status.GetStatusRead().IjmNirekoErr.stat ? "��" : "�~"));		// �j���R�Z���T�[�ُ풆
	}

	if (!m_bInit)
	{
		m_bInit = true;

		if (NULL != mcls_MI.gcls_pParam)
		{
			CString sWk;
			sWk.Format("%d", mcls_MI.gcls_pParam->GetCommonMast().nLogiParaCam[0][0]);
			CWnd::SetDlgItemTextA(IDC_PARA_CAM1_1, sWk);
			sWk.Format("%d", mcls_MI.gcls_pParam->GetCommonMast().nLogiParaCam[0][1]);
			CWnd::SetDlgItemTextA(IDC_PARA_CAM1_2, sWk);
			sWk.Format("%d", mcls_MI.gcls_pParam->GetCommonMast().nLogiParaCam[1][0]);
			CWnd::SetDlgItemTextA(IDC_PARA_CAM2_1, sWk);
			sWk.Format("%d", mcls_MI.gcls_pParam->GetCommonMast().nLogiParaCam[1][1]);
			CWnd::SetDlgItemTextA(IDC_PARA_CAM2_2, sWk);
		}
	}
}


// //////////////////////////////////////////////////////////////////////////////////////////////////////
// �f�o�b�O�{�^��

//==========================================
// PIO�擾������L�������ɕύX
//==========================================

void CTO_DEFECTDlg::OnBnClickedSmyuPio()
{
	//if (BST_CHECKED != CWnd::IsDlgButtonChecked(IDC_DEBUG_MANUAL))	return;

	////// �V�~�����[�^�ݒ�
	//bool bMode = (BST_CHECKED == CWnd::IsDlgButtonChecked(IDC_SMYU_PIO) ? true : false);

	//LOG(em_WAR), "[DEBUG] PIO�ڑ��� (%s)", bMode ? "�Эڰ�" : "�ʏ�");
	////mcls_MI.gcls_Dmem.SetSmyuMode(bMode);
}

//==========================================
// PLG�擾������L�������ɕύX
//==========================================
void CTO_DEFECTDlg::OnBnClickedSmyuPlg()
{
	//if (BST_CHECKED != CWnd::IsDlgButtonChecked(IDC_DEBUG_MANUAL)) return;

	////// �V�~�����[�^�ݒ�
	//bool bMode = (BST_CHECKED == CWnd::IsDlgButtonChecked(IDC_SMYU_PLG) ? true : false);

	//LOG(em_WAR), "[DEBUG] PLG�ڑ��� (%s)", bMode ? "�Эڰ�" : "�ʏ�" );
	//if (!mcls_MI.gcls_Plg.SetSmyuMode(bMode)) {
	//	LOG(em_ERR), "[Dlg] PLG �V�~�����[�^�p���L�������A�N�Z�X���s" );
	//}
}

//==========================================
// �f�o�b�O���j���[ �R�}���h�{�^���N���b�N
//==========================================
void CTO_DEFECTDlg::OnNMClickListCmd(NMHDR *pNMHDR, LRESULT *pResult)
{
	//LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	//// TODO: �����ɃR���g���[���ʒm�n���h���[ �R�[�h��ǉ����܂��B
	*pResult = 0;

	if (BST_CHECKED != CWnd::IsDlgButtonChecked(IDC_DEBUG_MANUAL)) return;


	//// �I�����ꂽ�s�����
	int idx = SListBox::GetClickItemIndex(pNMHDR);		// 0�I���W���B�ςȂƂ����-1
	if (-1 == idx) return;

	switch (idx) {
		//------------------------------------------------------------------------------
	case E_DEBUG_START:								// �����J�n
		if (true) {
			EM_DIV_UNTEN		nUnten = DIV_UNTEN_NOMAL;
			if (BST_CHECKED == CWnd::IsDlgButtonChecked(IDC_RAD_DEBUG_UNTEN1)) nUnten = DIV_UNTEN_NOMAL;
			if (BST_CHECKED == CWnd::IsDlgButtonChecked(IDC_RAD_DEBUG_UNTEN2)) nUnten = DIV_UNTEN_SAMP;
			if (BST_CHECKED == CWnd::IsDlgButtonChecked(IDC_RAD_DEBUG_UNTEN3)) nUnten = DIV_UNTEN_CAMERA;
			if (BST_CHECKED == CWnd::IsDlgButtonChecked(IDC_RAD_DEBUG_UNTEN4)) nUnten = DIV_UNTEN_SMYU;
			EM_DIV_PLG			nPlg = DIV_PLG_LINE;
			if (BST_CHECKED == CWnd::IsDlgButtonChecked(IDC_RAD_DEBUG_PLG1)) nPlg = DIV_PLG_LINE;
			if (BST_CHECKED == CWnd::IsDlgButtonChecked(IDC_RAD_DEBUG_PLG2)) nPlg = DIV_PLG_TOP;
			if (BST_CHECKED == CWnd::IsDlgButtonChecked(IDC_RAD_DEBUG_PLG3)) nPlg = DIV_PLG_BOT;
			if (BST_CHECKED == CWnd::IsDlgButtonChecked(IDC_RAD_DEBUG_PLG4)) nPlg = DIV_PLG_TEST;
			bool				bMenMode = false;
			if (BST_CHECKED == CWnd::IsDlgButtonChecked(IDC_RAD_DEBUG_MEN1)) bMenMode = false;
			if (BST_CHECKED == CWnd::IsDlgButtonChecked(IDC_RAD_DEBUG_MEN2)) bMenMode = true;

			int scode[NUM_MEN], pcode[NUM_MEN];
			scode[0] = CWnd::GetDlgItemInt(IDC_DEF_SCODE1);
			scode[1] = CWnd::GetDlgItemInt(IDC_DEF_SCODE2);
			pcode[0] = CWnd::GetDlgItemInt(IDC_DEF_PCODE1);
			pcode[1] = CWnd::GetDlgItemInt(IDC_DEF_PCODE2);

			int	nLogiParaCam[NUM_ALL_HTPARA][NUM_CAMLINK_BOARD];
			int nWk = CWnd::GetDlgItemInt(IDC_PARA_CAM1_1);
			if (1 > nWk) CWnd::SetDlgItemInt(IDC_PARA_CAM1_1, 1);
			if (20 < nWk) CWnd::SetDlgItemInt(IDC_PARA_CAM1_1, 20);
			nWk = CWnd::GetDlgItemInt(IDC_PARA_CAM1_2);
			if (1 > nWk) CWnd::SetDlgItemInt(IDC_PARA_CAM1_2, 1);
			if (20 < nWk) CWnd::SetDlgItemInt(IDC_PARA_CAM1_2, 20);
			nWk = CWnd::GetDlgItemInt(IDC_PARA_CAM2_1);
			if (1 > nWk) CWnd::SetDlgItemInt(IDC_PARA_CAM2_1, 1);
			if (20 < nWk) CWnd::SetDlgItemInt(IDC_PARA_CAM2_1, 20);
			nWk = CWnd::GetDlgItemInt(IDC_PARA_CAM2_2);
			if (1 > nWk) CWnd::SetDlgItemInt(IDC_PARA_CAM2_2, 1);
			if (20 < nWk) CWnd::SetDlgItemInt(IDC_PARA_CAM2_2, 20);
			nLogiParaCam[0][0] = CWnd::GetDlgItemInt(IDC_PARA_CAM1_1);
			nLogiParaCam[0][1] = CWnd::GetDlgItemInt(IDC_PARA_CAM1_2);
			nLogiParaCam[1][0] = CWnd::GetDlgItemInt(IDC_PARA_CAM2_1);
			nLogiParaCam[1][1] = CWnd::GetDlgItemInt(IDC_PARA_CAM2_2);

			// �����J�n�����ɂ�菈���킯
			LOG(em_WAR), "[DEBUG] �T���J�n�{�^�� [%s, %s, %s, s=%d:%d, p=%d:%d para1(cam1:%d cam2:%d) para2(cam1:%d cam2:%d)]",
				DivNameManager::GetDivUnten(nUnten), DivNameManager::GetDivPlg(nPlg), mcls_MI.gcls_Status.GetMenModeName(bMenMode),
				scode[0], scode[1], pcode[0], pcode[1],
				nLogiParaCam[0][0], nLogiParaCam[0][1], nLogiParaCam[1][0], nLogiParaCam[1][1]);

				COMMON_QUE que;
				que.nEventNo = FACT_TO_DEFECT_02;
				que.nLineNo = getlineid();
				que.fl.data[0] = (int)nUnten;
				que.fl.data[1] = (int)nPlg;
				que.fl.data[2] = (int)bMenMode;
				que.fl.data[3] = scode[0];
				que.fl.data[4] = scode[1];
				que.fl.data[5] = pcode[0];
				que.fl.data[6] = pcode[1];
				que.fl.data[7] = nLogiParaCam[0][0];
				que.fl.data[8] = nLogiParaCam[0][1];
				que.fl.data[9] = nLogiParaCam[1][0];
				que.fl.data[10] = nLogiParaCam[1][1];
				send_mail(TO_DEFECT, ".", &que);
		}
		break;

		//------------------------------------------------------------------------------
	case E_DEBUG_STOP:								// ������~
		if (true) {
			LOG(em_WAR), "[DEBUG] �T����~�{�^��" );

			COMMON_QUE que;
			que.nEventNo = FACT_TO_DEFECT_03;
			que.nLineNo = getlineid();
			send_mail(TO_DEFECT, ".", &que);
		}
		break;

		//------------------------------------------------------------------------------
	case E_DEBUG_RESUME_START:						// �r���J�n
		if (true)
		{
			EM_DIV_UNTEN		nUnten = DIV_UNTEN_NOMAL;
			if (BST_CHECKED == CWnd::IsDlgButtonChecked(IDC_RAD_DEBUG_UNTEN1)) nUnten = DIV_UNTEN_NOMAL;
			if (BST_CHECKED == CWnd::IsDlgButtonChecked(IDC_RAD_DEBUG_UNTEN2)) nUnten = DIV_UNTEN_SAMP;
			if (BST_CHECKED == CWnd::IsDlgButtonChecked(IDC_RAD_DEBUG_UNTEN3)) nUnten = DIV_UNTEN_CAMERA;
			if (BST_CHECKED == CWnd::IsDlgButtonChecked(IDC_RAD_DEBUG_UNTEN4)) nUnten = DIV_UNTEN_SMYU;
			EM_DIV_PLG			nPlg = DIV_PLG_LINE;
			if (BST_CHECKED == CWnd::IsDlgButtonChecked(IDC_RAD_DEBUG_PLG1)) nPlg = DIV_PLG_LINE;
			if (BST_CHECKED == CWnd::IsDlgButtonChecked(IDC_RAD_DEBUG_PLG2)) nPlg = DIV_PLG_TOP;
			if (BST_CHECKED == CWnd::IsDlgButtonChecked(IDC_RAD_DEBUG_PLG3)) nPlg = DIV_PLG_BOT;
			if (BST_CHECKED == CWnd::IsDlgButtonChecked(IDC_RAD_DEBUG_PLG4)) nPlg = DIV_PLG_TEST;
			bool				bMenMode = false;
			if (BST_CHECKED == CWnd::IsDlgButtonChecked(IDC_RAD_DEBUG_MEN1)) bMenMode = false;
			if (BST_CHECKED == CWnd::IsDlgButtonChecked(IDC_RAD_DEBUG_MEN2)) bMenMode = true;

			int scode[NUM_MEN], pcode[NUM_MEN];
			scode[0] = CWnd::GetDlgItemInt(IDC_DEF_SCODE1);
			scode[1] = CWnd::GetDlgItemInt(IDC_DEF_SCODE2);
			pcode[0] = CWnd::GetDlgItemInt(IDC_DEF_PCODE1);
			pcode[1] = CWnd::GetDlgItemInt(IDC_DEF_PCODE2);

			int	nLogiParaCam[NUM_ALL_HTPARA][NUM_CAMLINK_BOARD];
			int nWk = CWnd::GetDlgItemInt(IDC_PARA_CAM1_1);
			if (1 > nWk) CWnd::SetDlgItemInt(IDC_PARA_CAM1_1, 1);
			if (20 < nWk) CWnd::SetDlgItemInt(IDC_PARA_CAM1_1, 20);
			nWk = CWnd::GetDlgItemInt(IDC_PARA_CAM1_2);
			if (1 > nWk) CWnd::SetDlgItemInt(IDC_PARA_CAM1_2, 1);
			if (20 < nWk) CWnd::SetDlgItemInt(IDC_PARA_CAM1_2, 20);
			nWk = CWnd::GetDlgItemInt(IDC_PARA_CAM2_1);
			if (1 > nWk) CWnd::SetDlgItemInt(IDC_PARA_CAM2_1, 1);
			if (20 < nWk) CWnd::SetDlgItemInt(IDC_PARA_CAM2_1, 20);
			nWk = CWnd::GetDlgItemInt(IDC_PARA_CAM2_2);
			if (1 > nWk) CWnd::SetDlgItemInt(IDC_PARA_CAM2_2, 1);
			if (20 < nWk) CWnd::SetDlgItemInt(IDC_PARA_CAM2_2, 20);
			nLogiParaCam[0][0] = CWnd::GetDlgItemInt(IDC_PARA_CAM1_1);
			nLogiParaCam[0][1] = CWnd::GetDlgItemInt(IDC_PARA_CAM1_2);
			nLogiParaCam[1][0] = CWnd::GetDlgItemInt(IDC_PARA_CAM2_1);
			nLogiParaCam[1][1] = CWnd::GetDlgItemInt(IDC_PARA_CAM2_2);

			int nParaKind = 0;
			if (BST_CHECKED == CWnd::IsDlgButtonChecked(IDC_BTN_PARA2)) nParaKind = 1;

			LOG(em_WAR), "[DEBUG] �r���J�n�{�^�� [%s, %s, %s, s=%d:%d, p=%d:%d para%d(cam1:%d cam2:%d)]",
				DivNameManager::GetDivUnten(nUnten), DivNameManager::GetDivPlg(nPlg), mcls_MI.gcls_Status.GetMenModeName(bMenMode),
				scode[0], scode[1], pcode[0], pcode[1],
				nParaKind + 1, nLogiParaCam[nParaKind][0], nLogiParaCam[nParaKind][1]);

				COMMON_QUE que;
				que.nEventNo = FACT_TO_DEFECT_21;
				que.nLineNo = getlineid();
				que.fl.data[0] = (int)nUnten;
				que.fl.data[1] = (int)nPlg;
				que.fl.data[2] = (int)bMenMode;
				que.fl.data[3] = scode[0];
				que.fl.data[4] = scode[1];
				que.fl.data[5] = pcode[0];
				que.fl.data[6] = pcode[1];
				que.fl.data[7] = nParaKind;
				que.fl.data[8] = nLogiParaCam[nParaKind][0];
				que.fl.data[9] = nLogiParaCam[nParaKind][1];
				send_mail(TO_DEFECT, ".", &que);
		}
		break;

		//------------------------------------------------------------------------------
	case E_DEBUG_RESUME_STOP:						// �r����~
		if (true)
		{
			int nParaKind = 0;
			if (BST_CHECKED == CWnd::IsDlgButtonChecked(IDC_BTN_PARA2)) nParaKind = 1;

			LOG(em_WAR), "[DEBUG] �r����~�{�^�� [�p������PC%d]", nParaKind + 1);

			COMMON_QUE que;
			que.nEventNo = FACT_TO_DEFECT_22;
			que.nLineNo = getlineid();
			que.fl.data[0] = nParaKind;
			send_mail(TO_DEFECT, ".", &que);
		}
		break;

		//------------------------------------------------------------------------------
	case E_DEBUG_HT_CANNOT_CLEAR:					// ���葱�s�s�\����
		if (true) {
			LOG(em_WAR), "[DEBUG] ���葱�s�s�\�����{�^��" );
			mcls_MI.Send_HtDefect_PcCanOk();
		}
		break;

		//------------------------------------------------------------------------------
	case E_DEBUG_CAMLINKSW_READY:					// �J���������N�X�C�b�`�ݒ芮��
		if (true) {
			LOG(em_WAR), "[DEBUG] �J���������N�X�C�b�`�ݒ芮���ʒm�{�^��" );

			COMMON_QUE que;
			que.nEventNo = FACT_TO_DEFECT_30;
			que.nLineNo = getlineid();
			que.fl.data[0] = 1;
			send_mail(TO_DEFECT, ".", &que);
		}
		break;

		//------------------------------------------------------------------------------
	case E_DEBUG_CAMLINKSW_CAMSET:					// �J�����I��ݒ芮��
		LOG(em_WAR), "[DEBUG] �J�����I��ݒ芮���ʒm�{�^��" );
		{
			int mode;
			if (BST_CHECKED == CWnd::IsDlgButtonChecked(IDC_BTN_CAMSET_RESULT2))
			{
				// �ُ퉞��
				mode = 1;
			}
			else if (BST_CHECKED == CWnd::IsDlgButtonChecked(IDC_BTN_CAMSET_RESULT3))
			{
				// �`���s��v
				mode = 2;
			}
			else
			{
				// ���퉞��
				mode = 0;
			}
			LogicParaRunFunc::DebugSend_CamLinksw_CamsetCmpl(mode);
		}
		break;

		//------------------------------------------------------------------------------
	case E_DEBUG_PARAM:								// �p�����[�^�擾
		if (true) {
			LOG(em_WAR), "[DEBUG] �p�����[�^�擾�{�^��" );

			COMMON_QUE que;
			que.nEventNo = FACT_TO_DEFECT_01;
			que.nLineNo = getlineid();
			que.fl.data[0] = 0;
			que.fl.data[1] = CH_PARAM_ALL;
			que.fl.data[2] = CH_SCODE_ALL;
			send_mail(TO_DEFECT, ".", &que);
		}
		break;

		//------------------------------------------------------------------------------
	case E_DEBUG_RESUME_START_CMPL:					// �r���Q���ݒ芮��
		if (true)
		{
			int nParaKind = 0;
			int nJoinFrameNo = mcls_MI.gcls_Plg.GetFrameNo() + NUM_JOIN_FRAME;
			if (BST_CHECKED == CWnd::IsDlgButtonChecked(IDC_BTN_PARA2)) nParaKind = 1;

			LOG(em_WAR), "[DEBUG] �r���Q���ݒ芮���{�^�� [�p������PC%d frame=%d]", nParaKind + 1, nJoinFrameNo);

			COMMON_QUE que;
			que.nEventNo = FACT_TO_DEFECT_31;
			que.nLineNo = getlineid();
			que.fl.data[0] = 2 + nParaKind;
			que.fl.data[1] = nJoinFrameNo;
			send_mail(TO_DEFECT, ".", &que);
		}
		break;

		//------------------------------------------------------------------------------
	default:
		// ���肦�Ȃ�
		break;
	}
}

//==========================================
// �f�o�b�O���j���[ ���̓{�^���N���b�N
//==========================================
void CTO_DEFECTDlg::OnNMClickListInput(NMHDR *pNMHDR, LRESULT *pResult)
{
	*pResult = 0;

	if (BST_CHECKED != CWnd::IsDlgButtonChecked(IDC_DEBUG_MANUAL)) return;

	//// �I�����ꂽ�s�����
	int idx = SListBox::GetClickItemIndex(pNMHDR);		// 0�I���W���B�ςȂƂ����-1
	if (-1 == idx) return;

	switch (idx) {
		//------------------------------------------------------------------------------
	case E_DEBUG_RECV_COILINF_KEN:						// �[�������R�C����M
		if (true) {
			LOG(em_WAR), "[DEBUG] �[�������R�C����M�{�^��" );
			DummyCoilCreate(0);			// �[�������R�C������
			COMMON_QUE que;
			que.nEventNo = FACT_TO_DEFECT_11;
			que.nLineNo = getlineid();
			que.mix.idata[0] = 0;
			send_mail(TO_DEFECT, ".", &que);
		}
		break;
		//------------------------------------------------------------------------------
	case E_DEBUG_RECV_COILINF_DSP:						// �[���\���R�C����M
		if (true) {
			LOG(em_WAR), "[DEBUG] �[���\���R�C����M�{�^��" );
			DummyCoilCreate(1);			// �[���\���R�C������
			COMMON_QUE que;
			que.nEventNo = FACT_TO_DEFECT_11;
			que.nLineNo = getlineid();
			que.mix.idata[0] = 1;
			send_mail(TO_DEFECT, ".", &que);
		}
		break;
		//------------------------------------------------------------------------------
	case E_DEBUG_REGET_COILINF_KEN:						// �����p��� �Ď�M
		if (true) {
			LOG(em_WAR), "[DEBUG] �����p��� �Ď�M�{�^��" );
			COMMON_QUE que;
			que.nEventNo = FACT_TO_DEFECT_11;
			que.nLineNo = getlineid();
			que.mix.idata[0] = 0;
			send_mail(TO_DEFECT, ".", &que);
		}
		break;

		//------------------------------------------------------------------------------
	case E_DEBUG_REGET_COILINF_DSP:						// �\���p��� �Ď�M
		if(true){
			LOG(em_WAR), "[DEBUG] �\���p��� �Ď�M�{�^��" );
			COMMON_QUE que;
			que.nEventNo = FACT_TO_DEFECT_11;
			que.nLineNo  = getlineid();
			que.mix.idata[0] = 1;
			send_mail(TO_DEFECT, ".", &que);
		}
		break;

		//------------------------------------------------------------------------------
	case E_DEBUG_PI_WPD_KEN:							// PI ������_WPD���o
		LOG(em_WAR), "[DEBUG] ������_WPD���o�{�^��");
		mcls_MI.gcls_Dmem.PoPuls_TesetWpdOn(1);
		break;

		//------------------------------------------------------------------------------
	case E_DEBUG_PI_WPD_DSP:							// PI �\����_WPD���o
		LOG(em_WAR), "[DEBUG] �\����_WPD���o�{�^��");
		mcls_MI.gcls_Dmem.PoPuls_TesetWpdOn(2);
		break;

		//------------------------------------------------------------------------------
	case E_DEBUG_PI_WPD_KEN_DM:							// PI ������_WPD���o DM�̂�
		LOG(em_WAR), "[DEBUG] ������_WPD���o(DM�̂�)�{�^��");
		mcls_MI.gcls_Dmem.PoPuls_TesetWpdOn(3);
		break;

		//------------------------------------------------------------------------------
	case E_DEBUG_PI_WPD_DSP_DM:							// PI �\����_WPD���o DM�̂�
		LOG(em_WAR), "[DEBUG] �\����_WPD���o(DM�̂�)�{�^��");
		mcls_MI.gcls_Dmem.PoPuls_TesetWpdOn(4);
		break;

		//------------------------------------------------------------------------------
	case E_DEBUG_INITIAL:							// �C�j�V�����`����M�ʒm
		if (true) {
			LOG(em_WAR), "[DEBUG] �v���R�� �C�j�V�����`����M�{�^��");
			COMMON_QUE que;
			que.nEventNo = FACT_TO_DEFECT_12;
			que.nLineNo = getlineid();
			send_mail(TO_DEFECT, ".", &que);
		}
		break;

		//------------------------------------------------------------------------------
	case E_DEBUG_PI_SHAR_CUT:						// PI �V���[�J�b�g���M��
		LOG(em_WAR), "[DEBUG] �V���[�J�b�g���M���{�^��");
		SetEvent(mcls_MI.gcls_Dmem.GetEvPI_WPD_ShacutF());
		break;

		//------------------------------------------------------------------------------
	case E_DEBUG_PATO_RESET:						// PI �p�g���C�g���Z�b�g
		LOG(em_WAR), "[DEBUG] �p�g���C�g���Z�b�g�M���{�^��");
		SetEvent(mcls_MI.gcls_Dmem.GetEvPI_WPD_PatoReset());
		break;

		//------------------------------------------------------------------------------
	case E_DEBUG_SEQ_CNT_RESET:						// PO �V�[�P���T�J�E���^���Z�b�g 
		LOG(em_WAR), "[DEBUG] �V�[�P���T�J�E���^���Z�b�g�M���{�^��");
		mcls_MI.gcls_Dmem.PoPuls_CountReset();
		break;
		
		//------------------------------------------------------------------------------
	case E_DEBUG_VOICE_RESET_SETU:					// �{�C�X�x��o�͉����v���i�ݔ��ُ�x������j
		if (true) {
			LOG(em_WAR), "[DEBUG] �{�C�X�x��o�͉����v���i�ݔ��ُ�x������j" );

			COMMON_QUE que;
			que.nEventNo = FACT_TO_DEFECT_20;
			que.nLineNo = getlineid();
			que.fl.data[0] = 0;
			send_mail(TO_DEFECT, ".", &que);
		}
		break;

		//------------------------------------------------------------------------------
	case E_DEBUG_VOICE_RESET_DEF:					// �{�C�X�x��o�͉����v���i���׌��o�x������j
		if (true) {
			LOG(em_WAR), "[DEBUG] �{�C�X�x��o�͉����v���i���׌��o�x������j" );

			COMMON_QUE que;
			que.nEventNo = FACT_TO_DEFECT_20;
			que.nLineNo = getlineid();
			que.fl.data[0] = 1;
			send_mail(TO_DEFECT, ".", &que);
		}
		break;

		//------------------------------------------------------------------------------
	default:
		// ���肦�Ȃ�
		break;
	}
}

//==========================================
// LED����������ON/OFF�ؑ�
//==========================================
void CTO_DEFECTDlg::OnBnClickedDebugLedonoff()
{

	if (BST_CHECKED != CWnd::IsDlgButtonChecked(IDC_DEBUG_MANUAL)) return;

	bool bVal = (BST_CHECKED == CWnd::IsDlgButtonChecked(IDC_DEBUG_LEDONOFF) ? true : false);
	mcls_MI.gcls_Status.SetLedAutoCheckOff(bVal);
	LOG(em_WAR), "[DEBUG] LED�������� <%s>", (bVal ? "����" : "�L��"));

}


//==========================================
// �_�~�[�R�C����񐶐�
// nSendPoint: ���M�ʒu(0:������_ 1:�\����_)
//==========================================
void CTO_DEFECTDlg::DummyCoilCreate(int nSendPoint)
{
	SYSTEMTIME time;
	GetLocalTime(&time);

	TO_COIL_TBL::TO_COIL_REC* p = mcls_MI.gcls_pCoil->GetCoilTbl(nSendPoint);
	memset(p, 0x00, sizeof(TO_COIL_TBL::TO_COIL_REC));

	if(0 == nSendPoint)m_nCoil++;
	if (99 < m_nCoil) m_nCoil = 0;

	char cWk[SIZE_COIL_NO+1];
	//int size = sprintf(cWk, "%02d%02d%02d%02d%02d",
	//	time.wMonth, time.wDay, time.wHour, time.wMinute, time.wSecond);
	int size = sprintf(cWk, "test%02d      ", m_nCoil);

	// ��{���
	memset(&p->basedata.cCoilNoIn, 0x00, SIZE_COIL_NO+1);
	sprintf(p->basedata.cCoilNoIn, "%s", cWk);
	memset(&p->basedata.cMeiNo, 0x00, SIZE_MEI_NO+1);
	memcpy(&p->basedata.cMeiNo, "0123", SIZE_MEI_NO);
	memcpy(&p->basedata.cCoilKey[0], &p->basedata.cMeiNo, SIZE_MEI_NO);
	memcpy(&p->basedata.cCoilKey[SIZE_MEI_NO], &p->basedata.cCoilNoIn, SIZE_COIL_NO);

	p->basedata.nAtu = 600;
	//p->basedata.nWid = 567;
	p->basedata.nWid = COIL_WID_MAX;
	p->basedata.nLenIn = 1234;

	// ����
	p->setting.bWalm = true;
	for (int ii = 0; ii < NUM_MEN; ii++)
	{
		p->setting.pcode[ii]		= MAX_PCODE;
		p->setting.scode[ii]		= MAX_SCODE;
		p->setting.pcode_spmoff[ii]	= MAX_PCODE;
		p->setting.scode_spmoff[ii]	= MAX_SCODE;
	}

	// ���C���ŗL
	PRO_TYPE_COILINF* c = (PRO_TYPE_COILINF*)p->val;
	memset(&c->data.CoilInf, 0x20, sizeof(c->data.CoilInf));
	memcpy(c->data.CoilInf.KeyInf.cMeiNo, p->basedata.cMeiNo, sizeof(c->data.CoilInf.KeyInf.cMeiNo));
	memcpy(c->data.CoilInf.KeyInf.cCoilNo, p->basedata.cCoilNoIn, sizeof(c->data.CoilInf.KeyInf.cCoilNo));
	if (0 == nSendPoint)
		c->data.CoilInf.nSendPoint = 0;
	else if (1 == nSendPoint)
		c->data.CoilInf.nSendPoint = 1;
	else
		c->data.CoilInf.nSendPoint = 0;

	int nCoilLen = 2500;
	c->data.CoilInf.nWidZai = p->basedata.nWid;									// �ޗ���(����) [mm]
	c->data.CoilInf.nWidTrim = (int)((double)p->basedata.nWid * 0.9);			// �g������ [mm] (�g�����������́A0)
	c->data.CoilInf.nAtuSei = p->basedata.nAtu;									// ������ [��m]
	c->data.CoilInf.nLenIn = nCoilLen;											// �R�C���� [m]
	c->data.CoilInf.nAtuZai = p->basedata.nAtu;									// �ޗ���
	c->data.CoilInf.nWidSei = p->basedata.nWid;									// �ޗ���
	c->data.CoilInf.nNobiSPM = 80;												// SPM�L�ї�
	c->data.CoilInf.nNobiTLV = 50;												// TLV�L�ї�
	c->data.CoilInf.nCutF = (int)((double)nCoilLen * 0.01);						// �����ɂ�����t�����g�[�J�b�g�̎�����
	c->data.CoilInf.nCutT = (int)((double)nCoilLen * 0.02);						// �����ɂ�����e�[���[�J�b�g�̎�����
	c->data.CoilInf.nSPMRoolKei_Up = 100;										// SPM���[�N���[���a�i��j
	c->data.CoilInf.nSPMRoolKei_Down = 200;										// SPM���[�N���[���a�i���j
	memcpy(c->data.CoilInf.cKenKijun_O, "    ", 4);								// ����� �\
	memcpy(c->data.CoilInf.cKenKijun_U, "    ", 4);								// ����� ��
	memcpy(c->data.CoilInf.cKurometo_Kind, "  ", 2);							// �N�����[�g ���ю��
	memcpy(c->data.CoilInf.cKurometo_Umu, "  ", 2);								// �N�����[�g ���їL��
	memcpy(c->data.CoilInf.cHinCode, " ", 1);									// �i��
	memcpy(c->data.CoilInf.cMekkiKind, " ", 1);									// ���b�L���
	memcpy(c->data.CoilInf.cSyuZai, "  ", 2);									// ��ގ�
	memcpy(c->data.CoilInf.cDifMaker, " ", 1);									// �f�B�t�@�����V�����}�[�J�[
	memcpy(c->data.CoilInf.cKenban, " ", 1);									// ���敪
	memcpy(c->data.CoilInf.cMekkiCycle, " ", 1);								// ���b�L�T�C�N��
	memcpy(c->data.CoilInf.cInLineSPM, " ", 1);									// ���x���[ �C�����C��SPM
	memcpy(c->data.CoilInf.cKagakuToyu, " ", 1);								// ���w�����h��
	memcpy(c->data.CoilInf.cAtukkaSPM, " ", 1);									// �I�t���C��SPM������
	memcpy(c->data.CoilInf.cMekkiKubun, " ", 1);								// ���b�L�敪
	memcpy(c->data.CoilInf.cOmoteSyori1, " ", 1);								// �\�ʏ���1����
	memcpy(c->data.CoilInf.cOmoteSyori2, " ", 1);								// �\�ʏ���2����
	memcpy(c->data.CoilInf.cToyu, " ", 1);										// �h��
	memcpy(c->data.CoilInf.cKouteiYobi, " ", 1);								// �H���L���\��
	memcpy(c->data.CoilInf.cCaplSyuZai, "          ", 10);						// CAPL ��ގ�
	memcpy(c->data.CoilInf.cMetuke, "        ", 8);								// �ڕt�K�i
	memcpy(c->data.CoilInf.cNi, "  ", 2);										// Ni�t����
	memcpy(c->data.CoilInf.cSodoTCM, "  ", 2);									// TCM���[���e�x
	memcpy(c->data.CoilInf.cSpmUmu, "  ", 2);									// SPM�����L��
	memcpy(c->data.CoilInf.cNextLine, "  ", 2);									// �v�掟�H���R�[�h
	memcpy(c->data.CoilInf.cMuke, "ABCD                            ", 32);		// ����
	memcpy(c->data.CoilInf.cYouto, "    ", 4);									// �p�r
	memcpy(c->data.CoilInf.cCaplSiage, "    ", 4);								// CAPL�\�ʎd�グ
	memcpy(c->data.CoilInf.cCaplSanUmu, "  ", 2);								// CAPL�_���L��
	memcpy(c->data.CoilInf.cHinMode, " ", 1);									// �i��敪
	memcpy(c->data.CoilInf.cGaCrKubun, " ", 1);									// GA/CR�敪
	memcpy(c->data.CoilInf.cSpmJisekiUmu, "  ", 2);								// SPM�������їL��
	memcpy(c->data.CoilInf.cKouteiCode, "  ", 2);								// �H���R�[�h

	memcpy(&p->val, c, sizeof(PRO_TYPE_DATA_COILINF));
	return;
}
