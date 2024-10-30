
// TI_TESTDlg.cpp : �����t�@�C��
//

#include "stdafx.h"
#include "TI_TEST.h"
#include "TI_TESTDlg.h"
#include "afxdialogex.h"

// �������[���[�N���o �蓮�ǉ� ----->>>
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif
// <<<------------------------


// CTI_TESTDlg �_�C�A���O



CTI_TESTDlg::CTI_TESTDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_TI_TEST_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CTI_TESTDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, m_ListBox);
	DDX_Control(pDX, IDC_LIST_ST, m_ListSt);
}

BEGIN_MESSAGE_MAP(CTI_TESTDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDOK, &CTI_TESTDlg::OnBnClickedOk)
	ON_LBN_DBLCLK(IDC_LIST1, &CTI_TESTDlg::OnLbnDblclkList1)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_RAD_INPUT_MAN, &CTI_TESTDlg::OnBnClickedRadInputMan)
	ON_BN_CLICKED(IDC_RAD_INPUT_BMP, &CTI_TESTDlg::OnBnClickedRadInputBmp)
	ON_BN_CLICKED(IDC_RAD_INPUT_AVI, &CTI_TESTDlg::OnBnClickedRadInputAvi)
	ON_BN_CLICKED(IDC_BTN_MAN, &CTI_TESTDlg::OnBnClickedBtnMan)
	ON_BN_CLICKED(IDC_CHK_CYCLE, &CTI_TESTDlg::OnBnClickedChkCycle)
	ON_BN_CLICKED(IDC_DEBUG_MANUAL, &CTI_TESTDlg::OnBnClickedDebugManual)
	ON_BN_CLICKED(IDC_BTN_START, &CTI_TESTDlg::OnBnClickedBtnStart)
	ON_BN_CLICKED(IDC_BTN_STOP, &CTI_TESTDlg::OnBnClickedBtnStop)
	ON_BN_CLICKED(IDC_BTN_FRAME, &CTI_TESTDlg::OnBnClickedBtnFrame)
	ON_WM_DROPFILES()
	ON_BN_CLICKED(IDC_BTN_NEXT_COIL, &CTI_TESTDlg::OnBnClickedBtnNextCoil)
	ON_BN_CLICKED(IDC_CHK_RENZOKU, &CTI_TESTDlg::OnBnClickedChkRenzoku)
END_MESSAGE_MAP()


// CTI_TESTDlg ���b�Z�[�W �n���h���[

BOOL CTI_TESTDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// ���̃_�C�A���O�̃A�C�R����ݒ肵�܂��B�A�v���P�[�V�����̃��C�� �E�B���h�E���_�C�A���O�łȂ��ꍇ�A
	//  Framework �́A���̐ݒ�������I�ɍs���܂��B
	SetIcon(m_hIcon, TRUE);			// �傫���A�C�R���̐ݒ�
	SetIcon(m_hIcon, FALSE);		// �������A�C�R���̐ݒ�

	// TODO: �������������ɒǉ����܂��B
	// ��ʏ������Z�b�g----------------------------------------->>>
	CWnd::CheckRadioButton(IDC_RAD_INPUT_MAN, IDC_RAD_INPUT_AVI, IDC_RAD_INPUT_MAN);			// ���[�h�I��
	CWnd::GetDlgItem(IDC_CHK_CYCLE)->EnableWindow(false);										// �������s
	CWnd::CheckRadioButton(IDC_RAD_O, IDC_RAD_U, IDC_RAD_O);									// ������
	CWnd::CheckDlgButton(IDC_DEBUG_MANUAL, 1);

// �蓮�ǉ� -------------------->>>
// ��ʈʒu (�T�C�Y�͂��܂̂܂�)
	::SetWindowPos(m_hWnd, NULL, 400, 600, 0, 0, SWP_NOSIZE | SWP_NOZORDER);

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

void CTI_TESTDlg::OnPaint()
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
HCURSOR CTI_TESTDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



// ����ȍ~ �蓮�ǉ� -------------------->>>

//�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P
// �~�{�^��
//�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q
void CTI_TESTDlg::OnClose()
{
	//CDialog::OnClose();
}

//�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P
// �����I���{�^��
//�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q
void CTI_TESTDlg::OnBnClickedOk()
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
void CTI_TESTDlg::OnLbnDblclkList1()
{
	// ListBox�R���g���[���̃v���p�e�B�́A�\�[�g�֎~�ɂ��Ă�����
	// ListBox�R���g���[���̃_�u���N���b�N�C�x���g��ǉ�
	mcls_pLog->ShowEdit();
}

//------------------------------------------
// �e�v���Z�X����������
//------------------------------------------
void CTI_TESTDlg::InitProcess()
{
	//// �^�X�N����������
	int nCamStart = GetPrivateProfileInt(TI_TEST, "CAM_START", 1, TASKINI_NAME);
	CString sTaskName;
	sTaskName.Format("%s%02d", TI_TEST, nCamStart);
	if (0 != task_init(sTaskName)) {
	//if (0 != task_init(TI_TEST)) {
			_ASSERT(0);
		exit(-9);		// ���v���Z�X�����I��
		return;
	}

	//// LogFileManager�C���X�^���X���� (����ȍ~ ���O�o�͉\)
	setlocale(LC_ALL, "Japanese");	// ���ꂪ�����Ɠ��{��p�X���܂ރt�H���_�̏ꍇ�A�ۑ��ł��Ȃ��B(���C�u�����̒��Ŏ��s����ׂ��������Ă��邪�A�ύX����ƃ^�X�N�S�̂ɉe������̂ŁA�Ƃ肠���������Őݒ�)
	mcls_pLog = new LogFileManager(getmytaskname(), &m_ListBox, 50, 5);
	LOG(em_MSG), "[Dlg] �^�X�N�N�� [%s]", sTaskName);
	LOG(em_MSG), "[Dlg] �e�v���Z�X�J�n���� ���s���E�E�E");

	mcls_pLogCull = new LogFileManager("CULL", NULL);
	LOGCULL(em_MSG), "[Dlg] �^�X�N�N��");

	//-----------------------
	// �X�e�[�^�X��� ���X�g�r���[�C���X�^���X���� & ��`
	if (true) {
		mcnt_pListSt = new DspListView(&m_ListSt);
		// �����ڒ�`
		m_ListSt.SetExtendedStyle(m_ListSt.GetExtendedStyle() | LVS_EX_GRIDLINES);			// �r���`��
		m_ListSt.SetExtendedStyle(m_ListSt.GetExtendedStyle() | LVS_EX_FULLROWSELECT);		// 1�s�S�̑I��

		// ��ǉ�			0		1			
		char name[][32] = { "���",	"1", "2", "3", "4", "5", "6", "7", "8", "9", "10" };
		int  wid[] = { 140, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40 };
		mcnt_pListSt->SetColumnAll(sizeof(wid) / sizeof(int), 32, wid, (char const*)name);
		// �s�ǉ�
		mcnt_pListSt->SetRowAll(6);
		// �Œ�l�Z�b�g
		//mcnt_pListSt->SetItem(1, 0, "���Ə�Ԍ�");
		mcnt_pListSt->SetItem(1, 0, "FrameNo");
		mcnt_pListSt->SetItem(2, 0, "QUE  Free");
		mcnt_pListSt->SetItem(3, 0, "QUE  Max");
		mcnt_pListSt->SetItem(4, 0, "���M Ave");
		mcnt_pListSt->SetItem(5, 0, "���M Max");
	}

	// �����p�����l
	if (true) {
		CWnd::SetDlgItemTextA(IDC_ED_INPUT_BMP, "data\\test_Set%d_Ang%d_No%d.bmp");
		CWnd::SetDlgItemTextA(IDC_CYCLE_BMP, "60");
		CWnd::SetDlgItemTextA(IDC_ST_BMP, "0");
		CWnd::SetDlgItemTextA(IDC_ED_BMP, "3");
		CWnd::CheckRadioButton(IDC_RAD_INPUT_MAN, IDC_RAD_INPUT_AVI, IDC_RAD_INPUT_BMP);			// ���[�h�I��
		CWnd::GetDlgItem(IDC_CHK_CYCLE)->EnableWindow(true);										// �������s

		// �A���ғ�ON
		CWnd::CheckDlgButton(IDC_CHK_RENZOKU, 1);
	}

	//// ���C���C���X�^���X �J�n����
	mcls_MI.SetLogMgr(mcls_pLog);
	mcls_MI.SetLogMgrCull(mcls_pLogCull);
	mcls_MI.Init();

	//// ��ʍX�V�p�^�C�}����
	SetTimer(1, 500, NULL);

	//// ���C���X���b�h�̎��s���s��
	LOG(em_MSG), "[Dlg] �e�v���Z�X�J�n���� ����");
	mcls_MI.Start();
	return;
}

//�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P
// �e�v���Z�X�J������
//�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q
void CTI_TESTDlg::ExitProcess()
{
	//// ���C���C���X�^���X �I������
	KillTimer(1);			// ��ʍX�V�^�C�}
	mcls_MI.Stop(20000);
	mcls_MI.Exit();

	//// ���O ���X�g�{�b�N�X�\�������I��
	mcls_pLog->SetListOut(false);					// ����ȍ~ ���X�g�{�b�N�X�ւ̃��O�\���֎~

													//// ���O�N���X�J��
	LOG(em_MSG), "[Dlg] �^�X�N�I��");
	if (NULL != mcls_pLog) {
		delete mcls_pLog;
		mcls_pLog = NULL;
	}
	LOGCULL(em_MSG), "[Dlg] �^�X�N�I��");
	if (NULL != mcls_pLogCull) {
		delete mcls_pLogCull;
		mcls_pLogCull = NULL;
	}

	//// ��ʃR���g���[���J��
	delete mcnt_pListSt;

	// �^�X�N�I������
	task_exit();

	// ��ʂ̏I���v��
	PostMessage(WM_QUIT);
}

//------------------------------------------
// ���C���X���b�h DlgThread
// LPVOID param �_�C�����O�|�C���^
//------------------------------------------
unsigned WINAPI CTI_TESTDlg::MainThread(void* param)
{
	CTI_TESTDlg* p = (CTI_TESTDlg *)param;
	COMMON_QUE que;			// ��M�\����

							//// �e�v���Z�X���������� (����ȑO�̓��O�o�͋֎~)
	p->InitProcess();

	//// �N���v����荞�� ���[�v�J�n
	while (true) {
		recv_mail(&que);			// �N���v����荞��

		switch (que.nEventNo) {

			//-----------------------------------------------------------------------------------------------
		case FACT_TI_TEST_01:								// �����p �����J�n�ʒm (��TI_TEST)
			pLOG(em_MSG), "[MAIL] �����p �����J�n�ʒm (%d) [%d]", que.nEventNo, que.fl.data[0] );

			if (0 == que.fl.data[0]) {
				p->CWnd::CheckDlgButton(IDC_CHK_CYCLE, BST_UNCHECKED);			// �����{�^��OFF

				// �������M��~
				TestManager::DELIVERY_KEY* pQue = new TestManager::DELIVERY_KEY;
				memset(pQue, 0x00, sizeof(TestManager::DELIVERY_KEY));
				pQue->nMode = p->mcls_MI.gcls_pTest->GRAB_MODE_BMP;			// �I�����[�h
				pQue->nCycle = 0;											// ����(0:��~)
				p->mcls_MI.gcls_pTest->gque_Deli.AddToTail(pQue);
			}
			else {
				if (BST_UNCHECKED == p->CWnd::IsDlgButtonChecked(IDC_CHK_CYCLE)) {
					p->CWnd::CheckDlgButton(IDC_CHK_CYCLE, BST_CHECKED);	// �����{�^��ON
					p->OnBnClickedChkCycle();								// �������M�J�n
				}
			}
			break;

			//-----------------------------------------------------------------------------------------------
		case FACT_TI_TEST_02:								// �����p �t���[��No���Z�b�g�ʒm (��TI_TEST)
			pLOG(em_MSG), "[MAIL] �����p �t���[��No���Z�b�g�ʒm (%d) [%d][%d]", que.nEventNo, que.fl.data[0], que.fl.data[1] );

			p->mcls_MI.gcls_pTest->SetFrameNo(que.fl.data[0]);
			p->CWnd::SetDlgItemInt(IDC_CYCLE_BMP, que.fl.data[1]);
			break;

			//-----------------------------------------------------------------------------------------------
		case FACT_KS_PCNAME_CHANGE:							// PC���̕ύX�ʒm (��KS_SLAVE)
			pLOG(em_MSG), "[MAIL] PC���̕ύX�ʒm (FACT_KS_PCNAME_CHANGE)" );
			// PC���̎擾
			getpcname();
			break;

			//-----------------------------------------------------------------------------------------------
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
void CTI_TESTDlg::OnTimer(UINT_PTR nIDEvent)
{
	CString cwk;
	CString ss;


	//==================================================
	// �X�e�[�^�X���
#ifdef FRAME_RAW_SEND
	mcnt_pListSt->SetItem(0, 0, (0==mcls_MI.GetSysMode() ? "��~" : "�J�n"));
	for (int ii = 0; ii < NUM_CAM_POS; ii++) {
		//	for (int ii = 0; ii<FRAME_HT_SEND_PORT; ii++) {
		mcnt_pListSt->SetItem(0, 1 + ii, (mcls_MI.gcls_pSockRpi[ii]->IsConnect() ? "��" : "�~"));
		mcnt_pListSt->SetItem(1, 1 + ii, mcls_MI.gcls_pCull[ii]->GetFrameNo());
		mcnt_pListSt->SetItem(2, 1 + ii, mcls_MI.gque_pSockRpi[ii]->GetCountFree());
		mcnt_pListSt->SetItem(3, 1 + ii, mcls_MI.gque_pSockRpi[ii]->GetMaxCountPool());
		mcnt_pListSt->SetItem(4, 1 + ii, mcls_MI.gcls_pCull[ii]->g_nTime_Rpi[0]);
		mcnt_pListSt->SetItem(5, 1 + ii, mcls_MI.gcls_pCull[ii]->g_nTime_Rpi[1]);
	}
#endif

//#ifdef FRAME_RAW_SEND
//	for (int ii = 0; ii<FRAME_HT_SEND_PORT; ii++) {
//		mcnt_pListTo->SetItem(11 + 2 * ii, 1, mcls_MI.gcls_pSockRpi[ii]->GetSockStateName());
//		ss.Format("%d (%d)", mcls_MI.gque_pSockRpi[ii]->GetCountFree(), mcls_MI.gque_pSockRpi[ii]->GetMaxCountPool());	mcnt_pListTo->SetItem(12 + ii * 2, 1, ss);
//	}
//
//	double dAve = (mcls_MI.gcls_pCull[0]->g_nTime_Rpi[0] + mcls_MI.gcls_pCull[1]->g_nTime_Rpi[0]) / 2;
//	double dMax = max(mcls_MI.gcls_pCull[0]->g_nTime_Rpi[1], mcls_MI.gcls_pCull[1]->g_nTime_Rpi[1]);
//	ss.Format("%.0f (%.0f)", dAve, dMax);	mcnt_pListTo->SetItem(15, 1, ss);
//#endif

	int	nFrameNo = mcls_MI.gcls_pTest->GetFrameNo();
	CWnd::SetDlgItemInt(IDC_EDIT_FRAME_NEXT, nFrameNo);

	int nChgFrame = mcls_MI.gcls_pTest->GetChangeFrm();
	int nRenFrame = 0;
	if (0 < nChgFrame) { nRenFrame = ((nFrameNo / nChgFrame) + 1) * nChgFrame; }
	CWnd::SetDlgItemInt(IDC_ED_REN_NEXT, nRenFrame);


	CDialog::OnTimer(nIDEvent);
}


//==========================================
// �f�o�b�O���s�{�^������
//==========================================
void CTI_TESTDlg::OnBnClickedDebugManual()
{
	//if (BST_CHECKED != CWnd::IsDlgButtonChecked(IDC_DEBUG_MANUAL)) return;		// �f�o�b�O���s
	if (BST_CHECKED == CWnd::IsDlgButtonChecked(IDC_CHK_CYCLE)) {
		CWnd::CheckDlgButton(IDC_CHK_CYCLE, BST_UNCHECKED);
	}
}

//==========================================
// �蓮�I���{�^������
//==========================================
void CTI_TESTDlg::OnBnClickedRadInputMan()
{
	// TODO: �����ɃR���g���[���ʒm�n���h���[ �R�[�h��ǉ����܂��B
	CWnd::GetDlgItem(IDC_BTN_MAN)->EnableWindow(true);				// �蓮�摜�捞
	CWnd::GetDlgItem(IDC_CHK_CYCLE)->EnableWindow(false);			// �������s
}


//==========================================
// BMP�I���{�^������
//==========================================
void CTI_TESTDlg::OnBnClickedRadInputBmp()
{
	//if (BST_CHECKED != CWnd::IsDlgButtonChecked(IDC_DEBUG_MANUAL)) return;		// �f�o�b�O���s
	if (BST_CHECKED == CWnd::IsDlgButtonChecked(IDC_CHK_CYCLE)) {
		CWnd::CheckDlgButton(IDC_CHK_CYCLE, BST_UNCHECKED);
	}
	CWnd::GetDlgItem(IDC_BTN_MAN)->EnableWindow(false);				// �蓮�摜�捞
	CWnd::GetDlgItem(IDC_CHK_CYCLE)->EnableWindow(true);			// �������s
}


//==========================================
// AVI�I���{�^������
//==========================================
void CTI_TESTDlg::OnBnClickedRadInputAvi()
{
	//if (BST_CHECKED != CWnd::IsDlgButtonChecked(IDC_DEBUG_MANUAL)) return;		// �f�o�b�O���s
	if (BST_CHECKED == CWnd::IsDlgButtonChecked(IDC_CHK_CYCLE)) {
		CWnd::CheckDlgButton(IDC_CHK_CYCLE, BST_UNCHECKED);
	}
	CWnd::GetDlgItem(IDC_BTN_MAN)->EnableWindow(false);				// �蓮�摜�捞
	CWnd::GetDlgItem(IDC_CHK_CYCLE)->EnableWindow(true);			// �������s
}


//==========================================
// �蓮�摜�捞�{�^������
//==========================================
void CTI_TESTDlg::OnBnClickedBtnMan()
{
	if (BST_CHECKED != CWnd::IsDlgButtonChecked(IDC_DEBUG_MANUAL)) return;		// �f�o�b�O���s
	if (BST_CHECKED != CWnd::IsDlgButtonChecked(IDC_RAD_INPUT_MAN)) return;		// �蓮�ݒ�

	CString	sPath;
	CWnd::GetDlgItemTextA(IDC_ED_INPUT_MAN, sPath);

	TestManager::DELIVERY_KEY* pQue = new TestManager::DELIVERY_KEY;
	memset(pQue, 0x00, sizeof(TestManager::DELIVERY_KEY));
	pQue->nMode = mcls_MI.gcls_pTest->GRAB_MODE_MAN;			// �I�����[�h
	memcpy(pQue->cPath, sPath, sizeof(pQue->cPath));			// �t�@�C���p�X
	mcls_MI.gcls_pTest->gque_Deli.AddToTail(pQue);
}


//==========================================
// �������s�{�^������
//==========================================
void CTI_TESTDlg::OnBnClickedChkCycle()
{
	if (BST_CHECKED != CWnd::IsDlgButtonChecked(IDC_DEBUG_MANUAL)) return;		// �f�o�b�O���s

	int		nMode;
	CString	sPath;
	CString sCycle;
	CString sStart;
	CString sEnd;
	int		nCycle	= 0;

	if (BST_CHECKED == CWnd::IsDlgButtonChecked(IDC_RAD_INPUT_BMP)) {
		// BMP
		CWnd::GetDlgItemTextA(IDC_ED_INPUT_BMP, sPath);
		CWnd::GetDlgItemTextA(IDC_CYCLE_BMP, sCycle);
		CWnd::GetDlgItemTextA(IDC_ST_BMP, sStart);
		CWnd::GetDlgItemTextA(IDC_ED_BMP, sEnd);
		nMode = mcls_MI.gcls_pTest->GRAB_MODE_BMP;
	}
	else if (BST_CHECKED == CWnd::IsDlgButtonChecked(IDC_RAD_INPUT_AVI)) {
		// AVI
		CWnd::GetDlgItemTextA(IDC_ED_INPUT_AVI, sPath);
		CWnd::GetDlgItemTextA(IDC_CYCLE_AVI, sCycle);
		sStart = "";
		sEnd = "";
		nMode = mcls_MI.gcls_pTest->GRAB_MODE_AVI;
	}
	else {
		return;
	}

	TestManager::DELIVERY_KEY* pQue = new TestManager::DELIVERY_KEY;
	memset(pQue, 0x00, sizeof(TestManager::DELIVERY_KEY));
	pQue->nMode = nMode;										// �I�����[�h
	memcpy(pQue->cPath, sPath, sizeof(pQue->cPath));			// �t�@�C���p�X
	if (BST_CHECKED == CWnd::IsDlgButtonChecked(IDC_CHK_CYCLE)) {
		pQue->nCycle = atoi(sCycle);							// ����
	}
	else {
		pQue->nCycle = 0;										// ����(0:��~)
	}
	pQue->nStart = atoi(sStart);								// �J�nNo
	pQue->nEnd = atoi(sEnd);									// �I��No

	nCycle = pQue->nCycle;										// �����ێ�
	mcls_MI.gcls_pTest->gque_Deli.AddToTail(pQue);

	mcls_MI.Send_TiTest_Rendo(nCycle);							// �c�[���ԘA��
}



//==========================================
// �J�n�{�^������
//==========================================
void CTI_TESTDlg::OnBnClickedBtnStart()
{
	if (BST_CHECKED != CWnd::IsDlgButtonChecked(IDC_DEBUG_MANUAL)) return;		// �f�o�b�O���s

	if (BST_CHECKED == CWnd::IsDlgButtonChecked(IDC_RAD_O)) mcls_MI.SetMen(0);
	if (BST_CHECKED == CWnd::IsDlgButtonChecked(IDC_RAD_U)) mcls_MI.SetMen(1);

	// �J�n�C�x���g
	mcls_MI.SetEvKenStart();

}

//==========================================
// �I���{�^������
//==========================================
void CTI_TESTDlg::OnBnClickedBtnStop()
{
	if (BST_CHECKED != CWnd::IsDlgButtonChecked(IDC_DEBUG_MANUAL)) return;		// �f�o�b�O���s

	if (BST_CHECKED == CWnd::IsDlgButtonChecked(IDC_CHK_CYCLE)) {				// �������M��~
		CWnd::CheckDlgButton(IDC_CHK_CYCLE, BST_UNCHECKED);
		OnBnClickedChkCycle();
	}
	// �I���C�x���g
	mcls_MI.SetEvKenStop();


}




//==========================================
// �t���[��No�{�^������
//==========================================
void CTI_TESTDlg::OnBnClickedBtnFrame()
{
	CString	sWk;
	int	nFrameNo;
	CWnd::GetDlgItemTextA(IDC_EDIT_FRAME, sWk);
	nFrameNo = atoi(sWk);
	mcls_MI.gcls_pTest->SetFrameNo(nFrameNo);

	CWnd::GetDlgItemTextA(IDC_CYCLE_BMP, sWk);
	int nCycleTime = atoi(sWk);
	mcls_MI.Send_TiTest_FrameNo(nFrameNo, nCycleTime);		// �c�[���ԘA��
}


void CTI_TESTDlg::OnDropFiles(HDROP hDropInfo)
{
	// TODO: �����Ƀ��b�Z�[�W �n���h���[ �R�[�h��ǉ����邩�A����̏������Ăяo���܂��B

	CDialogEx::OnDropFiles(hDropInfo);
}


//==========================================
// ���R�C����񎖑O�ʒm�{�^������
//==========================================
void CTI_TESTDlg::OnBnClickedBtnNextCoil()
{
	if (BST_CHECKED != CWnd::IsDlgButtonChecked(IDC_DEBUG_MANUAL)) return;		// �f�o�b�O���s


	COMMON_QUE que;
	que.nEventNo = FACT_TI_RPISAVE_04;		// ���R�C����񎖑O�ʒm 
	que.nLineNo = LINE_ID;
	que.mix.idata[0] = -1;					// �ΏۃC���X�^���X
	if (BST_CHECKED == CWnd::IsDlgButtonChecked(IDC_RAD_O)) que.mix.idata[0] = 0;
	if (BST_CHECKED == CWnd::IsDlgButtonChecked(IDC_RAD_U)) que.mix.idata[0] = 1;
	que.mix.idata[1] = 1;					// �����p�^�[��(�\)
	que.mix.idata[2] = 1;					// �����p�^�[��(��)
	que.mix.idata[3] = 1;					// �����\�ʏ��(�\)
	que.mix.idata[4] = 1;					// �����\�ʏ��(��)
	que.mix.idata[5] = CWnd::GetDlgItemInt(IDC_ED_FRAME);		// ���s�t���[��No
	que.mix.idata[6] = CWnd::GetDlgItemInt(IDC_ED_TOP_POS);		// �擪����ʒu[mm]

	CString	sWk;
	CWnd::GetDlgItemTextA(IDC_ED_KANRI, sWk);
	memcpy(que.mix.cdata[0], sWk, sizeof(que.mix.cdata[0]));
	CWnd::GetDlgItemTextA(IDC_ED_COILNO, sWk);
	memcpy(que.mix.cdata[1], sWk, sizeof(que.mix.cdata[1]));

	send_mail(TI_RPISAVE, "*", &que);
}


//==========================================
// �A���ғ��{�^������
//==========================================
void CTI_TESTDlg::OnBnClickedChkRenzoku()
{
	if (BST_CHECKED == CWnd::IsDlgButtonChecked(IDC_CHK_RENZOKU)) {
		// �A���ғ� ON
		CString	sFrame;
		int	nFrameNo;
		CWnd::GetDlgItemTextA(IDC_ED_REN_FRM, sFrame);
		nFrameNo = atoi(sFrame);
		mcls_MI.gcls_pTest->SetChangeFrm(nFrameNo);
	}
	else {
		// �A���ғ� OFF
		mcls_MI.gcls_pTest->SetChangeFrm(0);
	}
}
