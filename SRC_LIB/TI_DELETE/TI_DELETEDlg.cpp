// TI_DELETEDlg.cpp : �����t�@�C��
//

#include "stdafx.h"
#include "TI_DELETE.h"
#include "TI_DELETEDlg.h"


// �������[���[�N���o ----->>>
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif
// <<<------------------------


// CTI_DELETEDlg �_�C�A���O




CTI_DELETEDlg::CTI_DELETEDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CTI_DELETEDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CTI_DELETEDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, m_ListBox);
	DDX_Control(pDX, IDC_CMB_DISP, m_cmbSession);
}

BEGIN_MESSAGE_MAP(CTI_DELETEDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_LBN_DBLCLK(IDC_LIST1, &CTI_DELETEDlg::OnLbnDblclkList1)
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDOK, &CTI_DELETEDlg::OnBnClickedOk)
	ON_WM_TIMER()
END_MESSAGE_MAP()


// CTI_DELETEDlg ���b�Z�[�W �n���h��

BOOL CTI_DELETEDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// ���̃_�C�A���O�̃A�C�R����ݒ肵�܂��B�A�v���P�[�V�����̃��C�� �E�B���h�E���_�C�A���O�łȂ��ꍇ�A
	//  Framework �́A���̐ݒ�������I�ɍs���܂��B
	SetIcon(m_hIcon, TRUE);			// �傫���A�C�R���̐ݒ�
	SetIcon(m_hIcon, FALSE);		// �������A�C�R���̐ݒ�

	// TODO: �������������ɒǉ����܂��B
// ------------------------------->>>
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

void CTI_DELETEDlg::OnPaint()
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
HCURSOR CTI_DELETEDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}




// ����ȍ~ �蓮�ǉ� -------------------->>>

//�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P
// �~�{�^��
//�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q
void CTI_DELETEDlg::OnClose()
{
	//CDialog::OnClose();
}
//�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P
// ����{�^��
//�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q
void CTI_DELETEDlg::OnBnClickedOk()
{
	if( IDYES == MessageBox("�I�����܂����H", "�d�v�Ȃ��m�点", MB_YESNO | MB_DEFBUTTON2 | MB_ICONQUESTION) ) {
		LOG(em_MSG), "����{�^��");
		COMMON_QUE que;
		que.nEventNo = FACT_KS_END;
		que.nLineNo = getlineid();		
		send_mail(getmytaskname(), ".", &que);	
	}
}


//�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P
// �������N��
//�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q
void CTI_DELETEDlg::OnLbnDblclkList1()
{
	// ListBox�R���g���[���̃v���p�e�B�́A�\�[�g�֎~�ɂ��Ă�����
	// ListBox�R���g���[���̃_�u���N���b�N�C�x���g��ǉ�
	mcls_pLog->ShowEdit();
}

//------------------------------------------
// �e�v���Z�X����������
//------------------------------------------
void CTI_DELETEDlg::InitProcess()
{
	// �^�X�N����������
	setlineid(LINE_ID);
	if( 0 != task_init(TI_DELETE) ) {
		syslog(SYSNO_TASK_INIT_ERR, "[%s]", TI_DELETE);
		_ASSERT(0);
		exit(-9);		// ���v���Z�X�����I��
		return;
	}

	// LogFileManager�C���X�^���X���� (����ȍ~ ���O�o�͉\)
	setlocale(LC_ALL, "Japanese");	// ���ꂪ�����Ɠ��{��p�X���܂ރt�H���_�̏ꍇ�A�ۑ��ł��Ȃ��B(���C�u�����̒��Ŏ��s����ׂ��������Ă��邪�A�ύX����ƃ^�X�N�S�̂ɉe������̂ŁA�Ƃ肠���������Őݒ�)
	mcls_pLog = new LogFileManager(getmytaskname(), &m_ListBox);
	LOG(em_MSG), "�^�X�N�N��");
	LOG(em_MSG), "�e�v���Z�X�J�n���� ���s���E�E�E");

	// ���C���C���X�^���X �J�n����
	mcls_MI.SetLogMgr(mcls_pLog);
	mcls_MI.Init();
	
	//// ��ʍX�V�p�f�[�^�Z�b�g
	// �R���{�{�b�N�X�ɃZ�b�V�������ǉ�
	for(int ii=0; ii<mcls_MI.g_nClassCnt; ii++) {
		m_cmbSession.AddString( mcls_MI.gcls_pContMgr[ii]->GetSession() );
	}
	m_cmbSession.SetCurSel(0);				// 0�Ԗڂ������I���Ƃ���
	SetTimer(1, 500, NULL);

	// ���C���X���b�h�̎��s���s��
	LOG(em_MSG), "�e�v���Z�X�J�n���� ����");
	mcls_MI.Start();	
	return;
}

//�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P
// �e�v���Z�X�J������
//�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q
void CTI_DELETEDlg::ExitProcess()
{
	//// ���O ���X�g�{�b�N�X�\�������I��
	mcls_pLog->SetListOut(false);					// ����ȍ~ ���X�g�{�b�N�X�ւ̃��O�\���֎~

	// ���C���C���X�^���X �I������
	KillTimer(1);			// ��ʍX�V�^�C�}
	mcls_MI.Stop();
	mcls_MI.Exit();

	//// ���O�N���X�J��
	LOG(em_MSG), "�^�X�N�I��");
	if(NULL != mcls_pLog){
		delete mcls_pLog;
		mcls_pLog = NULL;
	}
	
	// �^�X�N�I������
	task_exit();

	// �R���{�{�b�N�X������
	m_cmbSession.ResetContent();

	// ��ʂ̏I���v��
	PostMessage(WM_QUIT);
}

//------------------------------------------
// ���C���X���b�h DlgThread
// LPVOID param �_�C�����O�|�C���^
//------------------------------------------
unsigned WINAPI CTI_DELETEDlg::MainThread(void* param)
{
	CTI_DELETEDlg* p = (CTI_DELETEDlg *)param;
	COMMON_QUE que;			// ��M�\����

	//// �e�v���Z�X���������� (����ȑO�̓��O�o�͋֎~)
	p->InitProcess();

	//// �N���v����荞�� ���[�v�J�n
	while(true) {
		recv_mail(&que);			// �N���v����荞��

		switch(que.nEventNo) {

//-----------------------------------------------------------------------------------------------
		case FACT_TI_DELETE_01:								// �폜������D�惂�[�h�ʒm (��TO_DEFECT)
			pLOG(em_MSG), "%s�D�惂�[�h", (0 == que.fl.data[0] ? "�폜" : "����"));
			p->mcls_MI.SetWaitMode( (0==que.fl.data[0] ? true : false) );
			break;


//-----------------------------------------------------------------------------------------------
		case FACT_KS_PCNAME_CHANGE:							// PC���̕ύX�ʒm
			// ������
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
void CTI_DELETEDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: �����Ƀ��b�Z�[�W �n���h�� �R�[�h��ǉ����邩�A����̏������Ăяo���܂��B
	int ii;
	CString strVal;	
	int nWk;

	//// ���ݑI������Ă���C���X�^���X���擾����
	CString strSession = "";
	int nCmbIndex = m_cmbSession.GetCurSel();
	if( -1 == nCmbIndex) return;					// ini�t�@�C���̐ݒ薳��

	// �C���X�^���X�̓���
	ControlManager* pCnt = mcls_MI.gcls_pContMgr[nCmbIndex];


	//// �������
	SetDlgItemText( IDC_DSP_ODBC, pCnt->GetOdbcSession() );
	SetDlgItemText(IDC_STATE_DELETE,  (pCnt->GetWaitMode() ? "��" : "�~"));
	

	//// HDD���
	strVal.Format("%.2f", pCnt->GetDiskSpace().dTotalSize /1024 );	SetDlgItemText( IDC_HDD_TOTAL, strVal );
	strVal.Format("%.2f", pCnt->GetDiskSpace().dFreeSize  /1024 );	SetDlgItemText( IDC_HDD_FREE,  strVal );
	strVal.Format("%.2f", pCnt->GetDiskSpace().dFreeRitu  * 100 );	SetDlgItemText( IDC_HDD_RITU,  strVal );

	//// �X���b�h���s���
	int txtTH[9] = {IDC_TH_1, IDC_TH_2, IDC_TH_3, IDC_TH_4, IDC_TH_5, IDC_TH_6, IDC_TH_7, IDC_TH_8, IDC_TH_9};
	for(ii=0; ii<9; ii++) {
		nWk = pCnt->IsExec(ii);

		if( 0 == nWk )    strVal = "�~";
		else if(1 == nWk) strVal = "��";
		else			  strVal = "�|";
		SetDlgItemText( txtTH[ii], strVal);
	}
	// ������X���b�h
	nWk = pCnt->IsExec();
	if( 0 == nWk )    strVal = "�~";
	else if(1 == nWk) strVal = "��";
	else			  strVal = "�|";
	SetDlgItemText( IDC_TH_0, strVal);



	CDialog::OnTimer(nIDEvent);
}
