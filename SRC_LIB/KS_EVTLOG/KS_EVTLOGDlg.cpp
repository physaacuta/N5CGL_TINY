.// KS_EVTLOGDlg.cpp : �����t�@�C��
//

#include "stdafx.h"
#include "KS_EVTLOG.h"
#include "KS_EVTLOGDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CKS_EVTLOGDlg �_�C�A���O




CKS_EVTLOGDlg::CKS_EVTLOGDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CKS_EVTLOGDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CKS_EVTLOGDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, m_ListBox);
}

BEGIN_MESSAGE_MAP(CKS_EVTLOGDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_LBN_DBLCLK(IDC_LIST1, &CKS_EVTLOGDlg::OnLbnDblclkList1)
	ON_BN_CLICKED(IDOK, &CKS_EVTLOGDlg::OnBnClickedOk)
	ON_WM_CLOSE()
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_BUTTON1, &CKS_EVTLOGDlg::OnBnClickedButton1)
END_MESSAGE_MAP()


// CKS_EVTLOGDlg ���b�Z�[�W �n���h��

BOOL CKS_EVTLOGDlg::OnInitDialog()
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

void CKS_EVTLOGDlg::OnPaint()
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
HCURSOR CKS_EVTLOGDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


// ����ȍ~ �蓮�ǉ� -------------------->>>

//�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P
// �~�{�^��
//�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q
void CKS_EVTLOGDlg::OnClose()
{
	//CDialog::OnClose();
}
//�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P
// ����{�^��
//�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q
void CKS_EVTLOGDlg::OnBnClickedOk()
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
void CKS_EVTLOGDlg::OnLbnDblclkList1()
{
	// ListBox�R���g���[���̃v���p�e�B�́A�\�[�g�֎~�ɂ��Ă�����
	// ListBox�R���g���[���̃_�u���N���b�N�C�x���g��ǉ�
	mcls_pLog->ShowEdit();
}

//------------------------------------------
// �e�v���Z�X����������
//------------------------------------------
void CKS_EVTLOGDlg::InitProcess()
{
	// �^�X�N����������
	setlineid(LINE_ID);
	if( 0 != task_init(KS_EVTLOG) ) {
		syslog(SYSNO_TASK_INIT_ERR, "[%s]", KS_EVTLOG);
		_ASSERT(0);
		exit(-9);		// ���v���Z�X�����I��
		return;
	}

	// LogFileManager�C���X�^���X���� (����ȍ~ ���O�o�͉\)
	mcls_pLog = new LogFileManager(getmytaskname(), &m_ListBox);
	LOG(em_MSG), "�^�X�N�N��");
	LOG(em_MSG), "�e�v���Z�X�J�n���� ���s���E�E�E");

	// ���C���C���X�^���X �J�n����
	mcls_MI.SetLogMgr(mcls_pLog);
	mcls_MI.Init();
	
	//// ��ʍX�V�p�^�C�}����
	SetTimer(1, 500, NULL);

	// ���C���X���b�h�̎��s���s��
	LOG(em_MSG), "�e�v���Z�X�J�n���� ����");
	mcls_MI.Start();

	return;
}

//�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P
// �e�v���Z�X�J������
//�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q
void CKS_EVTLOGDlg::ExitProcess()
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

	// ��ʂ̏I���v��
	PostMessage(WM_QUIT);
}

//------------------------------------------
// ���C���X���b�h DlgThread
// LPVOID param �_�C�����O�|�C���^
//------------------------------------------
unsigned WINAPI CKS_EVTLOGDlg::MainThread(void* param)
{
	CKS_EVTLOGDlg* p = (CKS_EVTLOGDlg *)param;
	COMMON_QUE que;			// ��M�\����

	//// �e�v���Z�X���������� (����ȑO�̓��O�o�͋֎~)
	p->InitProcess();

	//// �N���v����荞�� ���[�v�J�n
	while(true) {
		recv_mail(&que);									// �N���v����荞��

		switch(que.nEventNo) {

		case FACT_KS_PCNAME_CHANGE:							// PC���̕ύX�ʒm
			break;


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
void CKS_EVTLOGDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: �����Ƀ��b�Z�[�W �n���h�� �R�[�h��ǉ����邩�A����̏������Ăяo���܂��B

	CDialog::OnTimer(nIDEvent);
}

//�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P
// �G���[��������
//�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q
void CKS_EVTLOGDlg::OnBnClickedButton1()
{
	LOG(em_WAR), "[Dlg] �G���[��������");
	this->mcls_MI.gcls_pEvt->ResetErr();
}
