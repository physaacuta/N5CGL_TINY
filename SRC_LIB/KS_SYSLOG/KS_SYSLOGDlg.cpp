// KS_SYSLOGDlg.cpp : �����t�@�C��
//

#include "stdafx.h"
#include "KS_SYSLOG.h"
#include "KS_SYSLOGDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CKS_SYSLOGDlg �_�C�A���O




CKS_SYSLOGDlg::CKS_SYSLOGDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CKS_SYSLOGDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CKS_SYSLOGDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, m_ListBox);
	DDX_Control(pDX, IDC_LIST1, m_ListBox);
}

BEGIN_MESSAGE_MAP(CKS_SYSLOGDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_LOGDSP, &CKS_SYSLOGDlg::OnBnClickedLogdsp)
	ON_BN_CLICKED(IDOK, &CKS_SYSLOGDlg::OnBnClickedOk)
	ON_LBN_DBLCLK(IDC_LIST1, &CKS_SYSLOGDlg::OnLbnDblclkList1)
	ON_WM_CLOSE()
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_BUTTON1, &CKS_SYSLOGDlg::OnBnClickedButton1)
END_MESSAGE_MAP()


// CKS_SYSLOGDlg ���b�Z�[�W �n���h��

BOOL CKS_SYSLOGDlg::OnInitDialog()
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

void CKS_SYSLOGDlg::OnPaint()
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
HCURSOR CKS_SYSLOGDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

// ����ȍ~ �蓮�ǉ� -------------------->>>

//�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P
// �~�{�^��
//�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q
void CKS_SYSLOGDlg::OnClose()
{
	//CDialog::OnClose();
}

//�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P
// ����{�^��
//�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q
void CKS_SYSLOGDlg::OnBnClickedOk()
{
	// TODO: �����ɃR���g���[���ʒm�n���h�� �R�[�h��ǉ����܂��B
	if( IDYES == MessageBox("�I�����܂����H", "�d�v�Ȃ��m�点", MB_YESNO | MB_DEFBUTTON2 | MB_ICONQUESTION) ) {
		LOG(em_MSG), "�����I���{�^��");
		COMMON_QUE que;
		que.nEventNo = FACT_KS_END;
		que.nLineNo = getlineid();
		send_mail(getmytaskname(), ".", &que);
	}
}

//�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P
// �������N��
//�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q
void CKS_SYSLOGDlg::OnLbnDblclkList1()
{
	// ListBox�R���g���[���̃v���p�e�B�́A�\�[�g�֎~�ɂ��Ă�����
	// ListBox�R���g���[���̃_�u���N���b�N�C�x���g��ǉ�
	mcls_pLog->ShowEdit();
}

//------------------------------------------
// �e�v���Z�X����������
//------------------------------------------
void CKS_SYSLOGDlg::InitProcess()
{
	// �^�X�N����������
	setlineid(LINE_ID);
	if( 0 != task_init(KS_SYSLOG) ) {
		syslog(SYSNO_TASK_INIT_ERR, "[%s]", KS_SYSLOG);
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
void CKS_SYSLOGDlg::ExitProcess()
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

// 20170321 ҰٽۯĻ��ނ��قȂ�����ԒʐM�̑Ή� --->
//------------------------------------------
// ���C���X���b�h DlgThread
// LPVOID param �_�C�����O�|�C���^
//------------------------------------------
unsigned WINAPI CKS_SYSLOGDlg::MainThread(void* param)
{
	CKS_SYSLOGDlg* p = (CKS_SYSLOGDlg *)param;

	//// �e�v���Z�X���������� (����ȑO�̓��O�o�͋֎~)
	p->InitProcess();

	//// �N���v����荞�� ���[�v�J�n
	while(true) {
		COMMON_QUE que[2];									// ��M�\���́i�O�ׂ̈ɖ��񏉊����A[2]��MAIL_SLOT�����������ꍇ���l���j
		int ncode = recv_mail(&que[0]);						// �N���v����荞��

		if (ncode > 0) {
			// ���������邾���Ő��폈���o���邩������Ȃ��̂�COINTINUE���Ȃ�
			if (ERROR_MORE_DATA == ncode) {
				pLOG(em_WAR), "recv_mail Size UnMatch [ErrCode=%d][EventNo=%d]", ncode, que[0].nEventNo);
			} else {
				pLOG(em_WAR), "recv_mail Err [ErrCode=%d][EventNo=%d]", ncode, que[0].nEventNo);
			}
		}

		switch(que[0].nEventNo) {

		case FACT_KS_SYSLOG_01:								// �V�X���O�o�^
			p->mcls_MI.AddSysLogSqlQue(&que[0]);
			break;

		case FACT_KS_SYSLOG_02:								// �@���ԃ��O�o�^
			p->mcls_MI.AddStateLogSqlQue(&que[0]);
			break;


		case FACT_KS_PCNAME_CHANGE:							// PC���̕ύX�ʒm (��KS_SLAVE)
			// ������
			break;

		case FACT_KS_END:									// �I���v��
			// �e�v���Z�X�I������
			p->ExitProcess();
			return 0;

		default:
			pLOG(em_ERR), "MAILNO_ERR [EventNo=%d]", que[0].nEventNo);
			syslog(SYSNO_MAILNO_ERR, "[EventNo=%d]", que[0].nEventNo);
			break;
		}
	}
	return 0;
}
// 20170321 ҰٽۯĻ��ނ��قȂ�����ԒʐM�̑Ή� <---

//�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P
// ��ʍX�V�^�C�}
//�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q
void CKS_SYSLOGDlg::OnTimer(UINT_PTR nIDEvent)

{
	// TODO: �����Ƀ��b�Z�[�W �n���h�� �R�[�h��ǉ����邩�A����̏������Ăяo���܂��B
	// �Ƃ肠�����ǉ��������ǁA���̏��K�v�Ȃ������E�E�E
	CDialog::OnTimer(nIDEvent);
}

//�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P
// ���O�\���`�F�b�N�{�b�N�X
//�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q
void CKS_SYSLOGDlg::OnBnClickedLogdsp()
{
	if ( BST_CHECKED == CWnd::IsDlgButtonChecked( IDC_LOGDSP) ) {		// ���O�\��
		if( IDYES == MessageBox("�p�t�H�[�}���X���ቺ���܂��B�f�o�b�N���̂ݎg�p���Ă�������", "�d�v�Ȋm�F",	MB_YESNO | MB_DEFBUTTON2 | MB_ICONQUESTION) ) {
			mcls_MI.SetSyslogDsp(true); 
		} else {
			CWnd::CheckDlgButton(IDC_LOGDSP, FALSE);	// ���݃L�����Z��
		}
	} else {															// ���O�o�͖���
		mcls_MI.SetSyslogDsp(false);
	}

}

void CKS_SYSLOGDlg::OnBnClickedButton1()
{
	syslog(0, "test [%s]", KS_SYSLOG);

}
