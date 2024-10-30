
// HY_TESTDlg.cpp : �����t�@�C��
//

#include "stdafx.h"
#include "HY_TEST.h"
#include "HY_TESTDlg.h"
#include "afxdialogex.h"

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

// �e�X�g�p
#define		HY_TEST		"HY_TEST"

// CHY_TESTDlg �_�C�A���O



CHY_TESTDlg::CHY_TESTDlg(CWnd* pParent /*=NULL*/)
	: CDialog(IDD_HY_TEST_DIALOG, pParent)
	, m_rdoKadou(0)
	, m_rdoUnten(0)
	, m_rdoKiki_o(0)
	, m_rdoKensa_o(0)
	, m_rdoKiki_u(0)
	, m_rdoKensa_u(0)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CHY_TESTDlg::DoDataExchange(CDataExchange* pDX)
{
	// �����l�ݒ�
	m_rdoKadou = 2;
	m_rdoUnten = 1;
	m_rdoKiki_o = 0;
	m_rdoKensa_o = 0;
	m_rdoKiki_u = 0;
	m_rdoKensa_u = 0;

	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, m_ListBox);
	DDX_Radio(pDX, IDC_RADIO1, m_rdoKadou);
	DDV_MinMaxInt(pDX, m_rdoKadou, 0, 3);
	DDX_Radio(pDX, IDC_RADIO5, m_rdoUnten);
	DDV_MinMaxInt(pDX, m_rdoUnten, 0, 4);
	DDX_Radio(pDX, IDC_RADIO10, m_rdoKiki_o);
	DDV_MinMaxInt(pDX, m_rdoKiki_o, 0, 3);
	DDX_Radio(pDX, IDC_RADIO14, m_rdoKensa_o);
	DDV_MinMaxInt(pDX, m_rdoKensa_o, 0, 7);
	DDX_Radio(pDX, IDC_RADIO22, m_rdoKiki_u);
	DDV_MinMaxInt(pDX, m_rdoKiki_u, 0, 3);
	DDX_Radio(pDX, IDC_RADIO26, m_rdoKensa_u);
	DDV_MinMaxInt(pDX, m_rdoKensa_u, 0, 7);
	DDX_Control(pDX, IDC_LIST_COIL, m_ListCoil);
	DDX_Control(pDX, IDC_LIST_DSP, m_ListDsp);
	DDX_Control(pDX, IDC_CHECK_DSWS, m_chkDsWs);
}

BEGIN_MESSAGE_MAP(CHY_TESTDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDOK, &CHY_TESTDlg::OnBnClickedOk)
	ON_WM_CLOSE()
	ON_LBN_DBLCLK(IDC_LIST1, &CHY_TESTDlg::OnLbnDblclkList1)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_BTN_STATUS, &CHY_TESTDlg::OnBnClickedBtnStatus)
	ON_BN_CLICKED(IDC_BTN_INIT, &CHY_TESTDlg::OnBnClickedBtnInit)
	ON_BN_CLICKED(IDC_BTN_PARAM, &CHY_TESTDlg::OnBnClickedBtnParam)
	ON_BN_CLICKED(IDC_BTN_ADDCOIL, &CHY_TESTDlg::OnBnClickedBtnAddcoil)
	ON_BN_CLICKED(IDC_BTN_CUT, &CHY_TESTDlg::OnBnClickedBtnCut)
	ON_BN_CLICKED(IDC_BTN_SPEED, &CHY_TESTDlg::OnBnClickedBtnSpeed)
	ON_BN_CLICKED(IDC_CHK_PLG, &CHY_TESTDlg::OnBnClickedChkPlg)
	ON_BN_CLICKED(IDC_BTN_SLOW, &CHY_TESTDlg::OnBnClickedBtnSlow)
	ON_BN_CLICKED(IDC_BTN_COIL_LEN, &CHY_TESTDlg::OnBnClickedBtnCoilLen)
	ON_BN_CLICKED(IDC_BTN_COIL_WID, &CHY_TESTDlg::OnBnClickedBtnCoilWid)
	ON_BN_CLICKED(IDC_CHK_DEF, &CHY_TESTDlg::OnBnClickedChkDef)
	ON_BN_CLICKED(IDC_BTN_DEF_RATE, &CHY_TESTDlg::OnBnClickedBtnDefRate)
	ON_BN_CLICKED(IDC_BTN_SETCODE, &CHY_TESTDlg::OnBnClickedBtnSetcode)
	ON_BN_CLICKED(IDC_BTN_DEF_SEND, &CHY_TESTDlg::OnBnClickedBtnDefSend)
	ON_BN_CLICKED(IDC_CHK_POS, &CHY_TESTDlg::OnBnClickedChkPos)
	ON_BN_CLICKED(IDC_CHK_MAE, &CHY_TESTDlg::OnBnClickedCheckMae)
	ON_BN_CLICKED(IDC_CHK_YOUSETUCUT, &CHY_TESTDlg::OnBnClickedCheckYousetuCut)
	ON_BN_CLICKED(IDC_BTN_DEFLST_SEND, &CHY_TESTDlg::OnBnClickedBtnLstCancelSend)
	ON_BN_CLICKED(IDC_BTN_ALARM_SEND, &CHY_TESTDlg::OnBnClickedBtnAlarmSend)
	ON_BN_CLICKED(IDC_BTN_DEFCNT_SEND, &CHY_TESTDlg::OnBnClickedBtnDefcntSend)
END_MESSAGE_MAP()


// CHY_TESTDlg ���b�Z�[�W �n���h���[

BOOL CHY_TESTDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// ���̃_�C�A���O�̃A�C�R����ݒ肵�܂��B�A�v���P�[�V�����̃��C�� �E�B���h�E���_�C�A���O�łȂ��ꍇ�A
	//  Framework �́A���̐ݒ�������I�ɍs���܂��B
	SetIcon(m_hIcon, TRUE);			// �傫���A�C�R���̐ݒ�
	SetIcon(m_hIcon, FALSE);		// �������A�C�R���̐ݒ�

	// TODO: �������������ɒǉ����܂��B

	CheckDlgButton(IDC_CHK_POS, 1);
	CheckDlgButton(IDC_CHK_DEF, 0);

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

void CHY_TESTDlg::OnPaint()
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
		CDialog::OnPaint();
	}
}

// ���[�U�[���ŏ��������E�B���h�E���h���b�O���Ă���Ƃ��ɕ\������J�[�\�����擾���邽�߂ɁA
//  �V�X�e�������̊֐����Ăяo���܂��B
HCURSOR CHY_TESTDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

// ����ȍ~ �蓮�ǉ� -------------------->>>

//�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P
// �~�{�^��
//�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q
void CHY_TESTDlg::OnClose()
{
	//CDialog::OnClose();		// �����̏���������
}

//�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P
// �����I���{�^��
//�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q
void CHY_TESTDlg::OnBnClickedOk()
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
void CHY_TESTDlg::OnLbnDblclkList1()
{
	// ListBox�R���g���[���̃v���p�e�B�́A�\�[�g�֎~�ɂ��Ă�����
	// ListBox�R���g���[���̃_�u���N���b�N�C�x���g��ǉ�
	mcls_pLog->ShowEdit();
}

//------------------------------------------
// �e�v���Z�X����������
//------------------------------------------
void CHY_TESTDlg::InitProcess()
{
	//// ��ԏ��߂ɂ��낢�돉����
	mcls_pLog = NULL;

	int nRetc = task_init(HY_TEST);
	if (0 != nRetc) {
		if (-1 == nRetc) { syslog(SYSNO_TASK_INIT_ERR, "[%s] ���Ƀ^�X�N�N���ς݂ɂ��A�N�����f�B", HY_TEST); }
		else { syslog(SYSNO_TASK_INIT_ERR, "[%s]", HY_TEST); }
		_ASSERT(0);
		exit(-9);		// ���v���Z�X�����I��
		return;
	}

	//// LogFileManager�C���X�^���X���� (����ȍ~ ���O�o�͉\)
	setlocale(LC_ALL, "Japanese");	// ���ꂪ�����Ɠ��{��p�X���܂ރt�H���_�̏ꍇ�A�ۑ��ł��Ȃ��B(���C�u�����̒��Ŏ��s����ׂ��������Ă��邪�A�ύX����ƃ^�X�N�S�̂ɉe������̂ŁA�Ƃ肠���������Őݒ�)
	mcls_pLog = new LogFileManager(getmytaskname(), &m_ListBox);
	LOG(em_MSG), "[Dlg] �^�X�N�N��");
	LOG(em_MSG), "[Dlg] �e�v���Z�X�J�n���� ���s���E�E�E");

	//// ���C���C���X�^���X �J�n����
	mcls_MI.SetLogMgr(mcls_pLog);
	mcls_MI.Init();

	//-----------------------
	// �R�C����� ���X�g�r���[�C���X�^���X���� & ��`
	if (true) {
		mcnt_pListCoil = new DspListView(&m_ListCoil);
		// �����ڒ�`
		m_ListCoil.SetExtendedStyle(m_ListCoil.GetExtendedStyle() | LVS_EX_GRIDLINES);			// �r���`��
		m_ListCoil.SetExtendedStyle(m_ListCoil.GetExtendedStyle() | LVS_EX_FULLROWSELECT);		// 1�s�S�̑I��

																							// ��ǉ�			0		1			
		char name[LIST_COIL_COLMAX][32];
		int  wid[LIST_COIL_COLMAX];
		
		memset(name, 0, sizeof(name));
		memset(wid, 0, sizeof(wid));

		strcpy(name[EM_COIL_LEN], "�����ʒu");		wid[EM_COIL_LEN] = 100;
		strcpy(name[EM_COIL_KANRINO], "�Ǘ�No");	wid[EM_COIL_KANRINO] = 140;
		strcpy(name[EM_COIL_COILNO], "�R�C��No");	wid[EM_COIL_COILNO] = 100;
		strcpy(name[EM_COIL_SPM_UMU], "SPM");		wid[EM_COIL_SPM_UMU] = 40;

		mcnt_pListCoil->SetColumnAll(LIST_COIL_COLMAX, 32, wid, (char const*)name);
		// �s�ǉ�
		mcnt_pListCoil->SetRowAll(MAX_COIL_NUM);
	}

	//-----------------------
	// �������� ���X�g�r���[�C���X�^���X���� & ��`
	if (true) {
		mcnt_pListDsp = new DspListView(&m_ListDsp);
		// �����ڒ�`
		m_ListDsp.SetExtendedStyle(m_ListDsp.GetExtendedStyle() | LVS_EX_GRIDLINES);			// �r���`��
		m_ListDsp.SetExtendedStyle(m_ListDsp.GetExtendedStyle() | LVS_EX_FULLROWSELECT);		// 1�s�S�̑I��

																								// ��ǉ�			0		1			
		char name[LIST_DSP_COLMAX][32];
		int  wid[LIST_DSP_COLMAX];

		memset(name, 0, sizeof(name));
		memset(wid, 0, sizeof(wid));

		strcpy(name[EM_DSP_NO], "");				wid[EM_DSP_NO] = 20;
		strcpy(name[EM_DSP_NAME], "�����䖼");		wid[EM_DSP_NAME] = 100;
		strcpy(name[EM_DSP_KANRINO], "�Ǘ�No");		wid[EM_DSP_KANRINO] = 140;
		strcpy(name[EM_DSP_DISTANCE], "�ʔʒu");	wid[EM_DSP_DISTANCE] = 100;

		mcnt_pListDsp->SetColumnAll(LIST_DSP_COLMAX, 32, wid, (char const*)name);
		// �s�ǉ�
		mcnt_pListDsp->SetRowAll(DIV_LEN_POS_FINAL);
		for (int ii = 0; ii<DIV_LEN_POS_FINAL; ii++) mcnt_pListDsp->SetItem(ii, EM_DSP_NO, ii + 1);
	}


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
void CHY_TESTDlg::ExitProcess()
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

	// �^�X�N�I������
	task_exit();

	if (mcnt_pListDsp) delete(mcnt_pListDsp);
	if (mcnt_pListCoil) delete(mcnt_pListCoil);

	// ��ʂ̏I���v��
	PostMessage(WM_QUIT);
}

//------------------------------------------
// ���C���X���b�h DlgThread
// LPVOID param �_�C�����O�|�C���^
//------------------------------------------
unsigned WINAPI CHY_TESTDlg::MainThread(LPVOID param)
{
	CHY_TESTDlg* p = (CHY_TESTDlg *)param;
	COMMON_QUE que;			// ��M�\����

							//// �e�v���Z�X���������� (����ȑO�̓��O�o�͋֎~)
	p->InitProcess();

	//// �N���v����荞�� ���[�v�J�n
	while (true) {
		recv_mail(&que);			// �N���v����荞��

		switch (que.nEventNo) {
			//*/
			//-----------------------------------------------------------------------------------------------
		case FACT_KS_PCNAME_CHANGE:								// PC���̕ύX�ʒm (��KS_SLAVE)
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
void CHY_TESTDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: �����Ƀ��b�Z�[�W �n���h�� �R�[�h��ǉ����邩�A����̏������Ăяo���܂��B


	PRO_TYPE_DATA_COILINF const* pCoilInf = mcls_MI.gcls_pTest->GetCoilInf();
	TestManager::TEST_COIL_INF const* pTestCoilInf = mcls_MI.gcls_pTest->GetTestCoilInf();
	TestManager::DSP_INF const* pDspInf = mcls_MI.gcls_pTest->GetDspInf();

	// �R�C�����
	for (int ii = 0; ii < MAX_COIL_NUM; ii++) {
		CString wk;
		wk.Format("%5d/%5d m", int(pTestCoilInf[ii].nLen / 1000), int(pTestCoilInf[ii].nCoilLen / 1000));
		mcnt_pListCoil->SetItem(ii, EM_COIL_LEN, wk);								// �����ʒu
		mcnt_pListCoil->SetItem(ii, EM_COIL_KANRINO, pTestCoilInf[ii].cKizukenNo);	// �Ǘ�No.
		mcnt_pListCoil->SetItem(ii, EM_COIL_COILNO, pTestCoilInf[ii].cCoilNo);		// �R�C��No.
		if (strlen(pTestCoilInf[ii].cKizukenNo)) {
			wk = GetName_SpmUmu((EM_DIV_SPM)pTestCoilInf[ii].nSpmUmu);
		} else {
			// �Ǘ�No.�������Ƃ��͋�
			wk = "";
		}
		mcnt_pListCoil->SetItem(ii, EM_COIL_SPM_UMU, wk);							// SPM�����L��
	}

	// ��������
	for (int ii = 0; ii < DIV_LEN_POS_FINAL; ii++) {
		CString wk;
		wk.Format("%8d mm", pDspInf[ii].nDistance);
		mcnt_pListDsp->SetItem(ii, EM_DSP_NAME, pDspInf[ii].Name);			// �����䖼
		mcnt_pListDsp->SetItem(ii, EM_DSP_DISTANCE, wk);					// �ʔ̋���
		mcnt_pListDsp->SetItem(ii, EM_DSP_KANRINO, pDspInf[ii].cKizukenNo);	// �Ǘ�No.
	}

	// ���C�����x�\��
	{
		CString wk;

		wk.Format("%03d(mpm)", mcls_MI.gcls_pTest->GetSpeed());
		SetDlgItemText(IDC_SPEED, wk);
	}

	// �R�C�����͈͕\��
	{
		CString wk;

		wk.Format("%02d(m)", mcls_MI.gcls_pTest->GetCoilLenMin() / 1000);
		SetDlgItemText(IDC_COIL_LEN_MIN, wk);

		wk.Format("%02d(m)", mcls_MI.gcls_pTest->GetCoilLenMax() / 1000);
		SetDlgItemText(IDC_COIL_LEN_MAX, wk);

	}

	// �R�C�����͈͕\��
	{
		CString wk;

		wk.Format("%02d(mm)", mcls_MI.gcls_pTest->GetCoilWidthMin());
		SetDlgItemText(IDC_COIL_WID_MIN, wk);

		wk.Format("%02d(mm)", mcls_MI.gcls_pTest->GetCoilWidthMax());
		SetDlgItemText(IDC_COIL_WID_MAX, wk);
	}

	// �r�������\��
	{
		CString wk;

		wk.Format("%02d(��)", mcls_MI.gcls_pTest->GetDefRate());
		SetDlgItemText(IDC_DEF_RATE, wk);

	}

	CDialog::OnTimer(nIDEvent);
}


//// �������ʒm
void CHY_TESTDlg::OnBnClickedBtnInit()
{
	// TODO: �����ɃR���g���[���ʒm�n���h���[ �R�[�h��ǉ����܂��B
	if( IDYES == MessageBox("�������ʒm�𑗐M���܂����H", "�d�v�Ȃ��m�点", MB_YESNO | MB_DEFBUTTON2 | MB_ICONQUESTION) ) {
		LOG(em_MSG), "[Dlg] �������ʒm���M�{�^��");
		TestManager::DELIVERY_KEY* pQue = new TestManager::DELIVERY_KEY;
		memset(pQue, 0x00, sizeof(TestManager::DELIVERY_KEY));
		pQue->id = TestManager::EM_INIT_NOTIFY;
		mcls_MI.gcls_pTest->gque_Deli.AddToTail(pQue);
	}

}

//// �p�����[�^�ύX�ʒm
void CHY_TESTDlg::OnBnClickedBtnParam()
{
	// TODO: �����ɃR���g���[���ʒm�n���h���[ �R�[�h��ǉ����܂��B
	TestManager::DELIVERY_KEY* pQue = new TestManager::DELIVERY_KEY;
	memset(pQue, 0x00, sizeof(TestManager::DELIVERY_KEY));
	pQue->id = TestManager::EM_PARAM_NOTIFY;
	mcls_MI.gcls_pTest->gque_Deli.AddToTail(pQue);
}


//// �R�C�����ǉ�
void CHY_TESTDlg::OnBnClickedBtnAddcoil()
{
	// TODO: �����ɃR���g���[���ʒm�n���h���[ �R�[�h��ǉ����܂��B
	TestManager::DELIVERY_KEY* pQue = new TestManager::DELIVERY_KEY;
	memset(pQue, 0x00, sizeof(TestManager::DELIVERY_KEY));
	pQue->id = TestManager::EM_COIL_ADD;

	mcls_MI.gcls_pTest->gque_Deli.AddToTail(pQue);

}

//// �J�b�g��񑗐M
void CHY_TESTDlg::OnBnClickedBtnCut()
{
	// TODO: �����ɃR���g���[���ʒm�n���h���[ �R�[�h��ǉ����܂��B
	LOG(em_MSG), "[Dlg] �J�b�g��񑗐M�{�^��");
	TestManager::DELIVERY_KEY* pQue = new TestManager::DELIVERY_KEY;
	memset(pQue, 0x00, sizeof(TestManager::DELIVERY_KEY));
	pQue->id = TestManager::EM_COIL_CUT;
	mcls_MI.gcls_pTest->gque_Deli.AddToTail(pQue);
}

//// ��Ԓʒm
void CHY_TESTDlg::OnBnClickedBtnStatus()
{
	// TODO: �����ɃR���g���[���ʒm�n���h���[ �R�[�h��ǉ����܂��B
	UpdateData();

	TestManager::DELIVERY_KEY* pQue = new TestManager::DELIVERY_KEY;
	memset(pQue, 0x00, sizeof(TestManager::DELIVERY_KEY));
	pQue->id = TestManager::EM_STATUS_NOTIFY;

	// �^�]��ԃZ�b�g
	switch (m_rdoUnten) {
	case 0:	// [�^�]���] ��~���[�h
		pQue->nUnten = DIV_UNTEN_STOP;
		break;
	case 1:	// [�^�]���] �ʏ�^�]���[�h
		pQue->nUnten = DIV_UNTEN_NOMAL;
		break;
	case 2:	// [�^�]���] �e�X�g�^�]���[�h (�T���v���B�e)
		pQue->nUnten = DIV_UNTEN_SAMP;
		break;
	case 3:	// [�^�]���] �Z�����[�h (�J��������)
		pQue->nUnten = DIV_UNTEN_CAMERA;
		break;
	case 4:	// [�^�]���] �V�~�����[�V�������[�h
		pQue->nUnten = DIV_UNTEN_SMYU;
		break;
	default:
		pQue->nUnten = DIV_UNTEN_NOMAL;
		break;
	}

	// �ғ���ԃZ�b�g
	switch (m_rdoKadou) {
	case 0:	// [�ғ����] ��������
		pQue->nKadou = DIV_KADOU_INIT;
		break;
	case 1:	// [�ғ����] ��~�� (��������)
		pQue->nKadou = DIV_KADOU_STOP;
		break;
	case 2:	// [�ғ����] �ғ��� & �e�X�g��
		pQue->nKadou = DIV_KADOU_START;
		break;
	case 3:	// [�ғ����] ���g���C��
		pQue->nKadou = DIV_KADOU_RETRY;
		break;
	default:
		pQue->nKadou = DIV_KADOU_START;
		break;
	}

	// �@���ԃZ�b�g
	switch (m_rdoKiki_o) {
	case 0:	// ����
		pQue->nKiki[0] = DIV_KIKI_OK;
		break;
	case 1:	// �y�̏�
		pQue->nKiki[0] = DIV_KIKI_KEI;
		break;
	case 2:	// ���̏�
		pQue->nKiki[0] = 2;
		break;
	case 3:	// �d�̏�
		pQue->nKiki[0] = DIV_KIKI_JYU;
		break;
	default:
		pQue->nKiki[0] = DIV_KIKI_OK;
		break;
	}
	switch (m_rdoKiki_u) {
	case 0:	// ����
		pQue->nKiki[1] = DIV_KIKI_OK;
		break;
	case 1:	// �y�̏�
		pQue->nKiki[1] = DIV_KIKI_KEI;
		break;
	case 2:	// ���̏�
		pQue->nKiki[1] = 2;
		break;
	case 3:	// �d�̏�
		pQue->nKiki[1] = DIV_KIKI_JYU;
		break;
	default:
		pQue->nKiki[1] = DIV_KIKI_OK;
		break;
	}

	// ������ԃZ�b�g
	switch (m_rdoKensa_o) {
	case 0:	// ����
		pQue->nKensa[0] = DIV_KENSA_OK;
		break;
	case 1:	// �k��
		pQue->nKensa[0] = DIV_KENSA_NG;
		break;
	case 2:	// �����e��
		pQue->nKensa[0] = DIV_KENSA_MENTE;
		break;
	case 3: // ��~
		pQue->nKensa[0] = DIV_KENSA_STOP;
		break;
	case 4: // �G�b�W�s��
		pQue->nKensa[0] = DIV_KENSA_EDGENG;
		break;
	case 5: // DB���ۑ�
		pQue->nKensa[0] = DIV_KENSA_DBNON;
		break;
	case 6: // ��������
		pQue->nKensa[0] = DIV_KENSA_SKIP;
		break;
	case 7: // �ΏۊO
		pQue->nKensa[0] = DIV_KENSA_NON;
		break;
	default:
		pQue->nKensa[0] = DIV_KENSA_OK;
		break;
	}
	switch (m_rdoKensa_u) {
	case 0:	// ����
		pQue->nKensa[1] = DIV_KENSA_OK;
		break;
	case 1:	// �k��
		pQue->nKensa[1] = DIV_KENSA_NG;
		break;
	case 2:	// �����e��
		pQue->nKensa[1] = DIV_KENSA_MENTE;
		break;
	case 3: // ��~
		pQue->nKensa[1] = DIV_KENSA_STOP;
		break;
	case 4: // �G�b�W�s��
		pQue->nKensa[1] = DIV_KENSA_EDGENG;
		break;
	case 5: // DB���ۑ�
		pQue->nKensa[1] = DIV_KENSA_DBNON;
		break;
	case 6: // ��������
		pQue->nKensa[1] = DIV_KENSA_SKIP;
		break;
	case 7: // �ΏۊO
		pQue->nKensa[1] = DIV_KENSA_NON;
		break;
	default:
		pQue->nKensa[1] = DIV_KENSA_OK;
		break;
	}

	LOG(em_MSG), "[Dlg] �^�]���[%d] �ғ����[%d] �@����[%d/%d] �������[%d/%d]", pQue->nUnten, pQue->nKadou, pQue->nKiki[0], pQue->nKiki[1], pQue->nKensa[0], pQue->nKensa[1] );
	mcls_MI.gcls_pTest->gque_Deli.AddToTail(pQue);

}


//// ���x�ݒ�
void CHY_TESTDlg::OnBnClickedBtnSpeed()
{
	// TODO: �����ɃR���g���[���ʒm�n���h���[ �R�[�h��ǉ����܂��B
	TestManager::DELIVERY_KEY* pQue = new TestManager::DELIVERY_KEY;
	memset(pQue, 0x00, sizeof(TestManager::DELIVERY_KEY));
	pQue->id = TestManager::EM_SPEED_SETTING;
	pQue->speed = GetDlgItemInt(IDC_EDIT_SPEED);
	mcls_MI.gcls_pTest->gque_Deli.AddToTail(pQue);
}

//// PLG��~���N��
void CHY_TESTDlg::OnBnClickedChkPlg()
{
	// TODO: �����ɃR���g���[���ʒm�n���h���[ �R�[�h��ǉ����܂��B
	TestManager::DELIVERY_KEY* pQue = new TestManager::DELIVERY_KEY;
	memset(pQue, 0x00, sizeof(TestManager::DELIVERY_KEY));

	if (!IsDlgButtonChecked(IDC_CHK_PLG)) {
		// ��~
		SetDlgItemText(IDC_CHK_PLG, "PLG��~��");
		pQue->id = TestManager::EM_PLG_STOP;
	}
	else {
		// �N��
		SetDlgItemText(IDC_CHK_PLG, "PLG�N����");
		pQue->id = TestManager::EM_PLG_START;
	}
	mcls_MI.gcls_pTest->gque_Deli.AddToTail(pQue);
}

//// PLG�P����
void CHY_TESTDlg::OnBnClickedBtnSlow()
{
	// TODO: �����ɃR���g���[���ʒm�n���h���[ �R�[�h��ǉ����܂��B
	TestManager::DELIVERY_KEY* pQue = new TestManager::DELIVERY_KEY;
	memset(pQue, 0x00, sizeof(TestManager::DELIVERY_KEY));
	pQue->id = TestManager::EM_PLG_SLOW;
	mcls_MI.gcls_pTest->gque_Deli.AddToTail(pQue);
}

//// �R�C�����@�ő�E�ŏ��Z�b�g
void CHY_TESTDlg::OnBnClickedBtnCoilLen()
{
	// TODO: �����ɃR���g���[���ʒm�n���h���[ �R�[�h��ǉ����܂��B

	int nMin = GetDlgItemInt(IDC_EDIT_COIL_LEN_MIN);
	int nMax = GetDlgItemInt(IDC_EDIT_COIL_LEN_MAX);

	if ((nMin != 0) && (nMax != 0)) {
		mcls_MI.gcls_pTest->SetCoilLenMin(nMin * 1000);
		mcls_MI.gcls_pTest->SetCoilLenMax(nMax * 1000);
	}

}

//// �R�C�����@�ő�E�ŏ��Z�b�g
void CHY_TESTDlg::OnBnClickedBtnCoilWid()
{
	// TODO: �����ɃR���g���[���ʒm�n���h���[ �R�[�h��ǉ����܂��B

	int nMin = GetDlgItemInt(IDC_EDIT_COIL_WID_MIN);
	int nMax = GetDlgItemInt(IDC_EDIT_COIL_WID_MAX);

	if ((nMin != 0) && (nMax != 0)) {
		mcls_MI.gcls_pTest->SetCoilWidthMin(nMin);
		mcls_MI.gcls_pTest->SetCoilWidthMax(nMax);
	}

}

// �O�H�����̍쐬 ����E�Ȃ� �`�F�b�N�{�b�N�X
void CHY_TESTDlg::OnBnClickedCheckMae()
{
	// TODO: �����ɃR���g���[���ʒm�n���h�� �R�[�h��ǉ����܂��B
	if (!IsDlgButtonChecked(IDC_CHK_MAE)) {
		// �쐬����
		SetDlgItemText(IDC_CHK_MAE, "�O�H�����쐬����");
		mcls_MI.gcls_pTest->SetMaeInfoStatus(false);
	}
	else {
		// �쐬����
		SetDlgItemText(IDC_CHK_MAE, "�O�H�����쐬����");
		mcls_MI.gcls_pTest->SetMaeInfoStatus(true);
	}
}

// �n�ړ_�V���[�J�b�g���{�L�� �`�F�b�N�{�b�N�X
void CHY_TESTDlg::OnBnClickedCheckYousetuCut()
{
	// TODO: �����ɃR���g���[���ʒm�n���h�� �R�[�h��ǉ����܂��B
	if (!IsDlgButtonChecked(IDC_CHK_YOUSETUCUT)) {
		// �쐬����
		SetDlgItemText(IDC_CHK_YOUSETUCUT, "�n�ړ_�J�b�g�����{���Ȃ�");
		mcls_MI.gcls_pTest->SetYousetuCut(false);
	}
	else {
		// �쐬����
		SetDlgItemText(IDC_CHK_YOUSETUCUT, "�n�ړ_�J�b�g�����{����");
		mcls_MI.gcls_pTest->SetYousetuCut(true);
	}
}

//// �r�����@�L��E����
void CHY_TESTDlg::OnBnClickedChkDef()
{
	// TODO: �����ɃR���g���[���ʒm�n���h���[ �R�[�h��ǉ����܂��B
	if (!IsDlgButtonChecked(IDC_CHK_DEF)) {
		// �r�쐬����
		SetDlgItemText(IDC_CHK_DEF, "�r��������");
		mcls_MI.gcls_pTest->SetDefectCreate(false);
	}
	else {
		// �r�쐬����
		SetDlgItemText(IDC_CHK_DEF, "�r������");
		mcls_MI.gcls_pTest->SetDefectCreate(true);
	}

}

//// �r�������ύX
void CHY_TESTDlg::OnBnClickedBtnDefRate()
{
	// TODO: �����ɃR���g���[���ʒm�n���h���[ �R�[�h��ǉ����܂��B
	int wk = GetDlgItemInt(IDC_EDIT_DEF_RATE);
	mcls_MI.gcls_pTest->SetDefRate(wk);
}


//// �\�ʏ�ԃR�[�h�E�����p�^�[���R�[�h�A�J�E���g�p�^�[���R�[�h�ݒ�
void CHY_TESTDlg::OnBnClickedBtnSetcode()
{
	// TODO: �����ɃR���g���[���ʒm�n���h���[ �R�[�h��ǉ����܂��B
	int scode = GetDlgItemInt(IDC_EDIT_SCODE);
	int pcode = GetDlgItemInt(IDC_EDIT_PCODE);
//	int ccode = GetDlgItemInt(IDC_EDIT_CCODE);

	mcls_MI.gcls_pTest->SetSCode(scode);
	mcls_MI.gcls_pTest->SetPCode(pcode);
//	mcls_MI.gcls_pTest->SetCCode(ccode);
}


//// �r��񑗐M
void CHY_TESTDlg::OnBnClickedBtnDefSend()
{
	// TODO: �����ɃR���g���[���ʒm�n���h���[ �R�[�h��ǉ����܂��B
	char cTmpKizuKnNo[SIZE_KIZUKEN_NO];

	memset(cTmpKizuKnNo, 0x00, sizeof(cTmpKizuKnNo));
	GetDlgItemText(IDC_EDIT_DEF_SYSTEM, &cTmpKizuKnNo[0], SIZE_KIZUKEN_NO);		// �r���Ǘ�No

	TestManager::DELIVERY_KEY* pQue = new TestManager::DELIVERY_KEY;
	memset(pQue, 0x00, sizeof(TestManager::DELIVERY_KEY));
	pQue->id = TestManager::EM_DEFECT;
	if (strlen(cTmpKizuKnNo) > 0) {							// �Ǘ�No���͗L
		memcpy(pQue->cKizukenNo, &cTmpKizuKnNo[0], SIZE_KIZUKEN_NO);
	}
	pQue->nY = GetDlgItemInt(IDC_EDIT_DEF_Y);
	pQue->dDS = GetDlgItemInt(IDC_EDIT_DEF_DSWS);	// �����������ʒu���w��(0�`7)
	pQue->nMen = GetDlgItemInt(IDC_EDIT_DEF_MEN);
	pQue->Tid = GetDlgItemInt(IDC_EDIT_DEF_TID);
	pQue->Gid = GetDlgItemInt(IDC_EDIT_DEF_GID);
	pQue->bNotChkDsWs = m_chkDsWs.GetCheck();
	mcls_MI.gcls_pTest->gque_Deli.AddToTail(pQue);
}

//// �d�����r���폜�v��
void CHY_TESTDlg::OnBnClickedBtnLstCancelSend()
{
	// TODO: �����ɃR���g���[���ʒm�n���h���[ �R�[�h��ǉ����܂��B
	TestManager::DELIVERY_KEY* pQue = new TestManager::DELIVERY_KEY;
	memset(pQue, 0x00, sizeof(TestManager::DELIVERY_KEY));
	char	cTmpKizuKnNo[SIZE_KIZUKEN_NO];
	memset(cTmpKizuKnNo, 0x00, sizeof(cTmpKizuKnNo));
	GetDlgItemText(IDC_EDIT_DEFLST_SYSTEM, &cTmpKizuKnNo[0], SIZE_KIZUKEN_NO);		// �r���Ǘ�No

	pQue->id = TestManager::EM_DEFLST_CANCEL;
	pQue->nCancelNo = GetDlgItemInt(IDC_EDIT_DEFLST_KIZUSEQ);
	memcpy(pQue->cKizukenNo, &cTmpKizuKnNo[0], SIZE_KIZUKEN_NO);
	pQue->nMen = GetDlgItemInt(IDC_EDIT_DEFLST_MEN);
	mcls_MI.gcls_pTest->gque_Deli.AddToTail(pQue);
}

//// �ʔʒu��񑗐M
void CHY_TESTDlg::OnBnClickedChkPos()
{
	// TODO: �����ɃR���g���[���ʒm�n���h���[ �R�[�h��ǉ����܂��B
	if (!IsDlgButtonChecked(IDC_CHK_POS)) {
		// ���M�Ȃ�
		mcls_MI.gcls_pTest->SetSendPos(false);
	}
	else {
		// ���M����
		mcls_MI.gcls_pTest->SetSendPos(true);
	}
}

//// �x���񑗐M
void CHY_TESTDlg::OnBnClickedBtnAlarmSend()
{
	// TODO: �����ɃR���g���[���ʒm�n���h���[ �R�[�h��ǉ����܂��B
	TestManager::DELIVERY_KEY* pQue = new TestManager::DELIVERY_KEY;
	memset(pQue, 0x00, sizeof(TestManager::DELIVERY_KEY));
	pQue->id = TestManager::EM_ALARM;
	pQue->nLanp[0][0][0] = GetDlgItemInt(IDC_EDIT_ALARM0_0_1);
	pQue->nLanp[0][0][1] = GetDlgItemInt(IDC_EDIT_ALARM0_0_2);
	pQue->nLanp[0][0][2] = GetDlgItemInt(IDC_EDIT_ALARM0_0_3);
	pQue->nLanp[0][0][3] = GetDlgItemInt(IDC_EDIT_ALARM0_0_4);
	pQue->nLanp[0][0][4] = GetDlgItemInt(IDC_EDIT_ALARM0_0_5);
	pQue->nLanp[0][0][5] = GetDlgItemInt(IDC_EDIT_ALARM0_0_6);
	pQue->nLanp[0][0][6] = GetDlgItemInt(IDC_EDIT_ALARM0_0_7);
	pQue->nLanp[0][0][7] = GetDlgItemInt(IDC_EDIT_ALARM0_0_8);
	pQue->nLanp[0][1][0] = GetDlgItemInt(IDC_EDIT_ALARM0_1_1);
	pQue->nLanp[0][1][1] = GetDlgItemInt(IDC_EDIT_ALARM0_1_2);
	pQue->nLanp[0][1][2] = GetDlgItemInt(IDC_EDIT_ALARM0_1_3);
	pQue->nLanp[0][1][3] = GetDlgItemInt(IDC_EDIT_ALARM0_1_4);
	pQue->nLanp[0][1][4] = GetDlgItemInt(IDC_EDIT_ALARM0_1_5);
	pQue->nLanp[0][1][5] = GetDlgItemInt(IDC_EDIT_ALARM0_1_6);
	pQue->nLanp[0][1][6] = GetDlgItemInt(IDC_EDIT_ALARM0_1_7);
	pQue->nLanp[0][1][7] = GetDlgItemInt(IDC_EDIT_ALARM0_1_8);
	pQue->nLanp[1][0][0] = GetDlgItemInt(IDC_EDIT_ALARM1_0_1);
	pQue->nLanp[1][0][1] = GetDlgItemInt(IDC_EDIT_ALARM1_0_2);
	pQue->nLanp[1][0][2] = GetDlgItemInt(IDC_EDIT_ALARM1_0_3);
	pQue->nLanp[1][0][3] = GetDlgItemInt(IDC_EDIT_ALARM1_0_4);
	pQue->nLanp[1][0][4] = GetDlgItemInt(IDC_EDIT_ALARM1_0_5);
	pQue->nLanp[1][0][5] = GetDlgItemInt(IDC_EDIT_ALARM1_0_6);
	pQue->nLanp[1][0][6] = GetDlgItemInt(IDC_EDIT_ALARM1_0_7);
	pQue->nLanp[1][0][7] = GetDlgItemInt(IDC_EDIT_ALARM1_0_8);
	pQue->nLanp[1][1][0] = GetDlgItemInt(IDC_EDIT_ALARM1_1_1);
	pQue->nLanp[1][1][1] = GetDlgItemInt(IDC_EDIT_ALARM1_1_2);
	pQue->nLanp[1][1][2] = GetDlgItemInt(IDC_EDIT_ALARM1_1_3);
	pQue->nLanp[1][1][3] = GetDlgItemInt(IDC_EDIT_ALARM1_1_4);
	pQue->nLanp[1][1][4] = GetDlgItemInt(IDC_EDIT_ALARM1_1_5);
	pQue->nLanp[1][1][5] = GetDlgItemInt(IDC_EDIT_ALARM1_1_6);
	pQue->nLanp[1][1][6] = GetDlgItemInt(IDC_EDIT_ALARM1_1_7);
	pQue->nLanp[1][1][7] = GetDlgItemInt(IDC_EDIT_ALARM1_1_8);
	mcls_MI.gcls_pTest->gque_Deli.AddToTail(pQue);
}

//------------------------------------------
// SPM�������
//------------------------------------------
CString CHY_TESTDlg::GetName_SpmUmu(EM_DIV_SPM em)
{
	CString s;
	if (DIV_SPM_OFF == em)		s = "��";
	else if (DIV_SPM_ON == em)	s = "�L";
	else						s.Format("%d?", em);
	return s;
}

//------------------------------------------
// �r�����M
//------------------------------------------
void CHY_TESTDlg::OnBnClickedBtnDefcntSend()
{
	// TODO: �����ɃR���g���[���ʒm�n���h���[ �R�[�h��ǉ����܂��B
	char cTmpKizuKnNo[SIZE_KIZUKEN_NO];
	int nMen = GetDlgItemInt(IDC_EDIT_DEFCNT_MEN);
	int nY = GetDlgItemInt(IDC_EDIT_DEFCNT_Y);
	int nCnt = GetDlgItemInt(IDC_EDIT_DEFCNT_CNT);
	
	memset(cTmpKizuKnNo, 0x00, sizeof(cTmpKizuKnNo));
	GetDlgItemText(IDC_EDIT_DEFCNT_SYSTEM, &cTmpKizuKnNo[0], SIZE_KIZUKEN_NO);		// �r���Ǘ�No

	if (strlen(cTmpKizuKnNo) < 1 || nMen < 0 || nY < 0) {							// �����͍��ڗL
		return;
	}

	TestManager::DELIVERY_KEY* pQue = new TestManager::DELIVERY_KEY;
	memset(pQue, 0x00, sizeof(TestManager::DELIVERY_KEY));

	pQue->id = TestManager::EM_DEFECT_NUM;
	pQue->nMen = nMen;
	memcpy(pQue->cKizukenNo, &cTmpKizuKnNo[0], SIZE_KIZUKEN_NO);
	pQue->nY = nY;
	pQue->nCnt = nCnt;
	mcls_MI.gcls_pTest->gque_Deli.AddToTail(pQue);
}
