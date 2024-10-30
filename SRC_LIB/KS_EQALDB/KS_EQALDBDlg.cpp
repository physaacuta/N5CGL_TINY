// KS_EQALDBDlg.cpp : �����t�@�C��
//

#include "stdafx.h"
#include "KS_EQALDB.h"
#include "KS_EQALDBDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CKS_EQALDBDlg �_�C�A���O




CKS_EQALDBDlg::CKS_EQALDBDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CKS_EQALDBDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CKS_EQALDBDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, m_ListBox);
	DDX_Control(pDX, IDC_CMB_DISP, m_cmbSession);
}

BEGIN_MESSAGE_MAP(CKS_EQALDBDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDOK, &CKS_EQALDBDlg::OnBnClickedOk)
	ON_LBN_DBLCLK(IDC_LIST1, &CKS_EQALDBDlg::OnLbnDblclkList1)
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDC_NOWEXEC_AB, &CKS_EQALDBDlg::OnBnClickedNowexecAb)
	ON_BN_CLICKED(IDC_NOWEXEC_BA, &CKS_EQALDBDlg::OnBnClickedNowexecBa)
	ON_WM_TIMER()
END_MESSAGE_MAP()


// CKS_EQALDBDlg ���b�Z�[�W �n���h��

BOOL CKS_EQALDBDlg::OnInitDialog()
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

void CKS_EQALDBDlg::OnPaint()
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
HCURSOR CKS_EQALDBDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

// ����ȍ~ �蓮�ǉ� -------------------->>>

//�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P
// �~�{�^��
//�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q
void CKS_EQALDBDlg::OnClose()
{
//	CDialog::OnClose();
}

//�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P
// ����{�^��
//�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q
void CKS_EQALDBDlg::OnBnClickedOk()
{
	if( IDYES == MessageBox("�I�����܂����H", "�d�v�Ȃ��m�点", MB_YESNO | MB_DEFBUTTON2 | MB_ICONQUESTION) ) {
		LOG(em_MSG), "����{�^��");
		COMMON_QUE que;
		que.nEventNo = FACT_KS_END;
		que.nLineNo = LINE_ID;						
		send_mail(getmytaskname(), ".", &que);	
	}
}
//�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P
// �������N��
//�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q
void CKS_EQALDBDlg::OnLbnDblclkList1()
{
	// ListBox�R���g���[���̃v���p�e�B�́A�\�[�g�֎~�ɂ��Ă�����
	// ListBox�R���g���[���̃_�u���N���b�N�C�x���g��ǉ�
	mcls_pLog->ShowEdit();
}


//------------------------------------------
// �e�v���Z�X����������
//------------------------------------------
void CKS_EQALDBDlg::InitProcess()
{
	int ii, jj;

	// �^�X�N����������
	if( 0 != task_init(KS_EQALDB) ) {
		syslog(SYSNO_TASK_INIT_ERR, "[%s]", KS_EQALDB);
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
	for(ii=0; ii<MainInctance::TYPE_DB_END; ii++) {
		for(jj=0; jj<mcls_MI.g_nClassCnt[ii]; jj++) {
			m_cmbSession.AddString( mcls_MI.gcls_pDb[ii][jj]->GetSession() );
		}
	}
	m_cmbSession.SetCurSel(0);				// 0�Ԗڂ������I���Ƃ���


	SetTimer(1, 500, NULL);					// ��ʍX�V�^�C�}�[


	// ���C���X���b�h�̎��s���s��
	LOG(em_MSG), "�e�v���Z�X�J�n���� ����");
	mcls_MI.Start();	
	return;
}

//�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P
// �e�v���Z�X�J������
//�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q
void CKS_EQALDBDlg::ExitProcess()
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
unsigned WINAPI CKS_EQALDBDlg::MainThread(void* param)
{
	CKS_EQALDBDlg* p = (CKS_EQALDBDlg *)param;
	COMMON_QUE que;			// ��M�\����

	//// �e�v���Z�X���������� (����ȑO�̓��O�o�͋֎~)
	p->InitProcess();

	//// �N���v����荞�� ���[�v�J�n
	while(true) {
		recv_mail(&que);			// �N���v����荞��

		switch(que.nEventNo) {

//-----------------------------------------------------------------------------------------------
		case FACT_KS_EQALDB_01:								// �������s�ʒm�i��KS_BACKUP�j
			pLOG(em_MSG), "[MAIL] �������s�ʒm (FACT_KS_EQALDB_01) [%s] ���s�L�[=%d", 0 == que.fl.data[0] ? "A��B" : "B��A", que.fl.data[1] );		
			p->mcls_MI.SelectNowExec( que.fl.data[0], que.fl.data[1]);
			break;

//-----------------------------------------------------------------------------------------------
		case FACT_KS_PCNAME_CHANGE:							// PC���̕ύX�ʒm (��KS_SLAVE)
				// ����
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
// ������^�C�}�[
//�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q
void CKS_EQALDBDlg::OnTimer(UINT_PTR nIDEvent)
{
	int ii, jj;
	KizuSyncroManager*	cls_pDb = NULL;

	//// ���ݑI������Ă���C���X�^���X���擾����
	CString strSession = "";
	int nCmbIndex = m_cmbSession.GetCurSel();
	if( -1 == nCmbIndex) return;					// ini�t�@�C���̐ݒ薳��
	
	// �C���X�^���X�̓���
	m_cmbSession.GetLBText( nCmbIndex, strSession);
	for(ii=0; ii<MainInctance::TYPE_DB_END; ii++) {
		for(jj=0; jj<mcls_MI.g_nClassCnt[ii]; jj++) {
			if( 0 == strcmp(strSession, mcls_MI.gcls_pDb[ii][jj]->GetSession() ) ) {
				cls_pDb = mcls_MI.gcls_pDb[ii][jj];
			}
		}
	}
	if(NULL == cls_pDb) goto Ending; 
	

	//// ���݂̏��
	SetDlgItemText(IDC_DSP_TYPE, cls_pDb->GetType() );
	SetDlgItemInt(IDC_DSP_MAILKEY, cls_pDb->GetMailkey() );
	SetDlgItemInt(IDC_DSP_TIMEFAST, cls_pDb->GetTimeFast() );
	SetDlgItemInt(IDC_DSP_TIMEINTERVAL, cls_pDb->GetTimeInterval() );

	SetDlgItemText(IDC_DSP_ODBC_A, cls_pDb->GetOdbc_A() );
	SetDlgItemText(IDC_DSP_ODBC_B, cls_pDb->GetOdbc_B() );
	SetDlgItemText(IDC_DSP_LINK_A, cls_pDb->GetLink_A() );
	SetDlgItemText(IDC_DSP_LINK_B, cls_pDb->GetLink_B() );


Ending:

	CDialog::OnTimer(nIDEvent);
}


//�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P
// �������s (A�n��B�n)
//�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q
void CKS_EQALDBDlg::OnBnClickedNowexecAb()
{
	// TODO: �����ɃR���g���[���ʒm�n���h�� �R�[�h��ǉ����܂��B
	if ( BST_CHECKED != CWnd::IsDlgButtonChecked(IDC_DEBUG_MANUAL) ) return;
	LOG(em_WAR), "[DEBUG] �������s�{�^�� (A�n��B�n)" ); 

	COMMON_QUE que;
	que.nEventNo = FACT_KS_EQALDB_01;
	que.nLineNo = getlineid();
	que.fl.data[0] = 0;
	que.fl.data[1] = GetDlgItemInt(IDC_MAILKEY);
	send_mail(getmytaskname(), ".", &que);
}

//�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P
// �������s (B�n��A�n)
//�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q
void CKS_EQALDBDlg::OnBnClickedNowexecBa()
{
	// TODO: �����ɃR���g���[���ʒm�n���h�� �R�[�h��ǉ����܂��B
	if ( BST_CHECKED != CWnd::IsDlgButtonChecked(IDC_DEBUG_MANUAL) ) return;
	LOG(em_WAR), "[DEBUG] �������s�{�^�� (B�n��A�n)" ); 

	COMMON_QUE que;
	que.nEventNo = FACT_KS_EQALDB_01;
	que.nLineNo = getlineid();
	que.fl.data[0] = 1;
	que.fl.data[1] = GetDlgItemInt(IDC_MAILKEY);
	send_mail(getmytaskname(), ".", &que);
}
