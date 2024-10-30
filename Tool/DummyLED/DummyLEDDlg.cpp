// DummyLEDDlg.cpp : �����t�@�C��
//

#include "stdafx.h"
#include "DummyLED.h"
#include "DummyLEDDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif
// �������[���[�N���o �蓮�ǉ� ----->>>
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif
// <<<------------------------


// CDummyLEDDlg �_�C�A���O




CDummyLEDDlg::CDummyLEDDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CDummyLEDDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CDummyLEDDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, m_ListBox);
	DDX_Control(pDX, IDC_SESSION, m_cmbSession);
	DDX_Control(pDX, IDC_COMBO3, m_cmbCh);
	DDX_Control(pDX, IDC_DEBUG, m_btnDebug);
	DDX_Control(pDX, IDC_COMBO4, debug_ch);
	DDX_Control(pDX, IDC_COMBO5, debug_uni);
	DDX_Control(pDX, IDC_STATE_TEMP, debug_tmp);
	DDX_Control(pDX, IDC_STATE_ONOFF_CHK, StatOnOff);
}

BEGIN_MESSAGE_MAP(CDummyLEDDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDOK, &CDummyLEDDlg::OnBnClickedOk)
	ON_LBN_DBLCLK(IDC_LIST1, &CDummyLEDDlg::OnLbnDblclkList1)
	ON_WM_TIMER()
//	ON_BN_CLICKED(IDC_DEBUG_BREAK, &CDummyLEDDlg::OnBnClickedDebugBreak)
	ON_BN_CLICKED(IDC_BTN_TEMP, &CDummyLEDDlg::OnBnClickedBtnTemp)
	ON_BN_CLICKED(IDC_BTN_BREAK, &CDummyLEDDlg::OnBnClickedBtnBreak)
	ON_BN_CLICKED(IDC_STATE_ONOFF_CHK, &CDummyLEDDlg::OnBnClickedStateOnoff)
	ON_CBN_SELCHANGE(IDC_COMBO4, &CDummyLEDDlg::OnCbnSelchangeCombo4)
	ON_CBN_SELCHANGE(IDC_COMBO5, &CDummyLEDDlg::OnCbnSelchangeCombo5)
END_MESSAGE_MAP()


// CDummyLEDDlg ���b�Z�[�W �n���h��

BOOL CDummyLEDDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// ���̃_�C�A���O�̃A�C�R����ݒ肵�܂��B�A�v���P�[�V�����̃��C�� �E�B���h�E���_�C�A���O�łȂ��ꍇ�A
	//  Framework �́A���̐ݒ�������I�ɍs���܂��B
	SetIcon(m_hIcon, TRUE);			// �傫���A�C�R���̐ݒ�
	SetIcon(m_hIcon, FALSE);		// �������A�C�R���̐ݒ�

	// TODO: �������������ɒǉ����܂��B
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

void CDummyLEDDlg::OnPaint()
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
HCURSOR CDummyLEDDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


// ����ȍ~ �蓮�ǉ� -------------------->>>

//�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P
// �~�{�^��
//�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q
void CDummyLEDDlg::OnClose()
{
	//CDialog::OnClose();		// �����̏���������
}

//�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P
// �����I���{�^��
//�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q
void CDummyLEDDlg::OnBnClickedOk()
{
	if( IDYES == MessageBox("�I�����܂����H", "�d�v�Ȃ��m�点", MB_YESNO | MB_DEFBUTTON2 | MB_ICONQUESTION) ) {
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
void CDummyLEDDlg::OnLbnDblclkList1()
{
	// ListBox�R���g���[���̃v���p�e�B�́A�\�[�g�֎~�ɂ��Ă�����
	// ListBox�R���g���[���̃_�u���N���b�N�C�x���g��ǉ�
	mcls_pLog->ShowEdit();
}


//------------------------------------------
// �e�v���Z�X����������
//------------------------------------------
void CDummyLEDDlg::InitProcess()
{
	CString sWk;
	//// ��ԏ��߂ɂ��낢�돉����
	mcls_pLog = NULL;

	//// �^�X�N����������
	//if( 0 != task_init(XX_CPPSMPEX, 0) ) {		// PCID���Ȃ��ꍇ�B
	//if( 0 != task_init(XX_CPPSMPEX) ) {
	//	syslog(SYSNO_TASK_INIT_ERR, "[%s]", XX_CPPSMPEX);
	//	_ASSERT(0);
	//	exit(-9);		// ���v���Z�X�����I��
	//	return;
	//}
	int nRetc = task_init(DummyLED);
	if( 0 != nRetc ) {
		if(-1 == nRetc) { syslog(SYSNO_TASK_INIT_ERR, "[%s] ���Ƀ^�X�N�N���ς݂ɂ��A�N�����f�B", DummyLED); }
		else			{ syslog(SYSNO_TASK_INIT_ERR, "[%s]", DummyLED); }
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
	
	//// ��ʍX�V�p�f�[�^�Z�b�g

	//-----------------------
	// �R���{�{�b�N�X�ɃZ�b�V�������ǉ�
	for(int ii=0; ii<MAX_THREAD; ii++) {
		if( NULL == mcls_MI.gcls_pDmLedMgr[ii] ) break;
		m_cmbSession.AddString( mcls_MI.gcls_pDmLedMgr[ii]->GetMyName() );
	}
	m_cmbSession.SetCurSel(0);				// 0�Ԗڂ������I���Ƃ���

	for(int ii=0; ii<MAX_LED_CH; ii++) {
		sWk.Format("%d", ii+1);
		m_cmbCh.AddString( sWk );
	}
	m_cmbCh.SetCurSel(0);

	for(int ii=0; ii<MAX_LED_CH; ii++) {
		sWk.Format("%d", ii+1);
		debug_ch.AddString( sWk );
	}
	debug_ch.SetCurSel(0);

	for(int ii=0; ii<MAX_TEMP_UNIT; ii++) {
		sWk.Format("%d", ii+1);
		debug_uni.AddString( sWk );
	}	
	debug_uni.SetCurSel(0);

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
void CDummyLEDDlg::ExitProcess()
{
	//// ���C���C���X�^���X �I������
	KillTimer(1);			// ��ʍX�V�^�C�}
	mcls_MI.Stop();
	mcls_MI.Exit();

	//// ���O ���X�g�{�b�N�X�\�������I��
	mcls_pLog->SetListOut(false);					// ����ȍ~ ���X�g�{�b�N�X�ւ̃��O�\���֎~

	//// ���O�N���X�J��
	LOG(em_MSG), "[Dlg] �^�X�N�I��");
	if(NULL != mcls_pLog){
		delete mcls_pLog;
		mcls_pLog = NULL;
	}
	
	// �^�X�N�I������
	task_exit();

	// ��ʂ̏I���v��
	PostMessage(WM_QUIT);
}

//------------------------------------------
// ���C���X���b�h DlgThread
// LPVOID param �_�C�����O�|�C���^
//------------------------------------------
unsigned WINAPI CDummyLEDDlg::MainThread(void* param)
{
	CDummyLEDDlg* p = (CDummyLEDDlg *)param;
	COMMON_QUE que;			// ��M�\����

	//// �e�v���Z�X���������� (����ȑO�̓��O�o�͋֎~)
	p->InitProcess();

	//// �N���v����荞�� ���[�v�J�n
	while(true) {
		recv_mail(&que);			// �N���v����荞��

		switch(que.nEventNo) {
//*/
//-----------------------------------------------------------------------------------------------
		case FACT_KS_PCNAME_CHANGE:								// PC���̕ύX�ʒm (��KS_SLAVE)
			pLOG(em_MSG), "[MAIL] �p�����[�^�ύX�v�� (FACT_TO_DEFECT_01) [�e�[�u��=%d]", que.fl.data[0]); 
			// ���C���C���X�^���X�֒ʒm
			p->mcls_MI.SetDeliveryMail( E_DEF_ML_PARAM, &que);
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
void CDummyLEDDlg::OnTimer(UINT_PTR nIDEvent)
{
	CString		sWk;
	int			nWk;
	//// ���ݑI������Ă���C���X�^���X���擾����
	int nIndex = m_cmbSession.GetCurSel();
	if( -1 == nIndex) return;					// ini�t�@�C���̐ݒ薳��
	DummyLedManager* pCls = mcls_MI.gcls_pDmLedMgr[nIndex];
	if( NULL == pCls ) return;

	int nCh = m_cmbCh.GetCurSel();
	if( -1 == nCh) return;
	//int nUni = debug_uni.GetCurSel();
	//if (-1 == nUni) return;

	SetDlgItemInt(IDC_STATE_VALUE,		pCls->GetNowLedBase(nCh));

	SetDlgItemInt(IDC_STATE_VALUE21,	pCls->GetLedTemp(nCh,0));
	SetDlgItemInt(IDC_STATE_VALUE22,	pCls->GetLedTemp(nCh,1));
	SetDlgItemInt(IDC_STATE_VALUE23,	pCls->GetLedTemp(nCh,2));

	int nbreak = pCls->GetStateBreak(nCh);
	if( 0 == nbreak)SetDlgItemText(IDC_STATE_VALUE3,"��");
	else if( 1 == nbreak)SetDlgItemText(IDC_STATE_VALUE3,"�~");
	else			SetDlgItemText(IDC_STATE_VALUE3,"�H");

	//int nStTmp = pCls->SetLedTempSts(nCh, nUni);
	int nStTmp = pCls->GetStateTemp(nCh);
	if( 0 == nStTmp)SetDlgItemText(IDC_STATE_VALUE4,"��");
	else if( 1 == nStTmp)SetDlgItemText(IDC_STATE_VALUE4,"��");
	else if( 2 == nStTmp)SetDlgItemText(IDC_STATE_VALUE4,"�~");
	else			SetDlgItemText(IDC_STATE_VALUE4,"�H");


	int nOnOff = pCls->GetStateOnOff(nCh);
	if( 0 == nOnOff)SetDlgItemText(IDC_STATE_ONOFF,"��");
	else			SetDlgItemText(IDC_STATE_ONOFF, "�~");
	//if( 0 == nOnOff)CheckDlgButton(IDC_STATE_ONOFF,true);
	//else			CheckDlgButton(IDC_STATE_ONOFF,false);

	CDialog::OnTimer(nIDEvent);
}



void CDummyLEDDlg::OnBnClickedBtnTemp()
{
	if ( BST_CHECKED != CWnd::IsDlgButtonChecked(IDC_DEBUG) ) return;


	//// ���ݑI������Ă���C���X�^���X���擾����
	int nIndex = m_cmbSession.GetCurSel();
	if( -1 == nIndex) return;					// ini�t�@�C���̐ݒ薳��
	DummyLedManager* pCls = mcls_MI.gcls_pDmLedMgr[nIndex];
	if( NULL == pCls ) return;

	int nCh = debug_ch.GetCurSel();
	if( -1 == nCh) return;
	int nUni = debug_uni.GetCurSel();
	if( -1 == nUni) return;
	int nTemp = GetDlgItemInt(IDC_STATE_TEMP);
	if( -1 == nTemp) return;
	pCls->SetTemp(nCh,nUni,nTemp);

	pCls->SetLedTempSts(nCh, nUni);
	//pCls->SetStateTemp(nCh);

	LOG(em_WAR), "[DEBUG] ���x�ύX CH:%d Unit:%d Temp:%d(Sts:%d)", nCh, nUni, nTemp, pCls->GetLedTempSts(nCh, nUni));
}

void CDummyLEDDlg::OnBnClickedBtnBreak()
{
	if ( BST_CHECKED != CWnd::IsDlgButtonChecked(IDC_DEBUG) ) return;
	int nIndex = m_cmbSession.GetCurSel();
	if( -1 == nIndex) return;					// ini�t�@�C���̐ݒ薳��
	DummyLedManager* pCls = mcls_MI.gcls_pDmLedMgr[nIndex];
	if( NULL == pCls ) return;
	int nCh = debug_ch.GetCurSel();
	if( -1 == nCh) return;
	pCls->SetBreak(nCh);

	LOG(em_WAR), "[DEBUG] �f����ԕύX CH:%d Break:%d", nCh, pCls->GetStateBreak(nCh));

}


//==========================================
// �_��/�����ؑ�
//==========================================
void CDummyLEDDlg::OnBnClickedStateOnoff()
{
	if (BST_CHECKED != CWnd::IsDlgButtonChecked(IDC_DEBUG)) return;
	int nIndex = m_cmbSession.GetCurSel();
	if (-1 == nIndex) return;					// ini�t�@�C���̐ݒ薳��
	DummyLedManager* pCls = mcls_MI.gcls_pDmLedMgr[nIndex];
	if (NULL == pCls) return;
	int nCh = debug_ch.GetCurSel();
	if (-1 == nCh) return;

	// �_��/�����ؑ�
	int nOnOff = 0;		// �_��/�����v���ɏ]��
	if (BST_CHECKED == CWnd::IsDlgButtonChecked(IDC_STATE_ONOFF_CHK)) {
		nOnOff = 1;		// �����Œ�
	}
	pCls->SetLedOnOff(nCh, nOnOff);		// �_��/��������ؑ�

	LOG(em_WAR), "[DEBUG] �_��/�������� �ύX CH:%d %s", nCh, (0 == nOnOff ? "�v���ɏ]��" : "�����Œ�"));
}


void CDummyLEDDlg::OnCbnSelchangeCombo4()
{
	int nIndex = m_cmbSession.GetCurSel();
	if (-1 == nIndex) return;					// ini�t�@�C���̐ݒ薳��
	DummyLedManager* pCls = mcls_MI.gcls_pDmLedMgr[nIndex];
	if (NULL == pCls) return;
	int nCh = debug_ch.GetCurSel();
	if (-1 == nCh) return;

	int nOnOff = pCls->GetLedOnOff(nCh);	// �_��/��������擾


	if( 0 == nOnOff) CheckDlgButton(IDC_STATE_ONOFF_CHK, false);
	else             CheckDlgButton(IDC_STATE_ONOFF_CHK, true);

	LOG(em_WAR), "[DEBUG] CH�ύX �_��/�������� CH:%d %s", nCh, (0 == nOnOff ? "�v���ɏ]��" : "�����Œ�"));
}


void CDummyLEDDlg::OnCbnSelchangeCombo5()
{
	int nIndex = m_cmbSession.GetCurSel();
	if (-1 == nIndex) return;					// ini�t�@�C���̐ݒ薳��
	DummyLedManager* pCls = mcls_MI.gcls_pDmLedMgr[nIndex];
	if (NULL == pCls) return;
	int nCh = debug_ch.GetCurSel();
	if (-1 == nCh) return;

	int nOnOff = pCls->GetLedOnOff(nCh);	// �_��/��������擾


	if (0 == nOnOff) CheckDlgButton(IDC_STATE_ONOFF_CHK, false);
	else             CheckDlgButton(IDC_STATE_ONOFF_CHK, true);

	LOG(em_WAR), "[DEBUG] CH�ύX �_��/�������� CH:%d %s", nCh, (0 == nOnOff ? "�v���ɏ]��" : "�����Œ�"));
}


