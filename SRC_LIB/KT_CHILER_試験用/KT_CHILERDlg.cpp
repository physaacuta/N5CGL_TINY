// KS_CHILERDlg.cpp : �����t�@�C��
//

#include "stdafx.h"
#include "KT_CHILER.h"
#include "KT_CHILERDlg.h"

// �������[���[�N���o �蓮�ǉ� ----->>>
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif
// <<<------------------------

// CKT_CHILERDlg �_�C�A���O




CKT_CHILERDlg::CKT_CHILERDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CKT_CHILERDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CKT_CHILERDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, m_ListBox);
	DDX_Control(pDX, IDC_CMB_DISP, m_cmbSession);
}

BEGIN_MESSAGE_MAP(CKT_CHILERDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_LBN_DBLCLK(IDC_LIST1, &CKT_CHILERDlg::OnLbnDblclkList1)
	ON_BN_CLICKED(IDOK, &CKT_CHILERDlg::OnBnClickedOk)
	ON_WM_CLOSE()
	ON_WM_TIMER()
//	ON_WM_ENDSESSION()
	ON_BN_CLICKED(IDC_REQ_UNTEN, &CKT_CHILERDlg::OnBnClickedReqUnten)
	ON_BN_CLICKED(IDC_REQ_NOWTEMP, &CKT_CHILERDlg::OnBnClickedReqNowtemp)
	ON_BN_CLICKED(IDC_REQ_SETTEMP, &CKT_CHILERDlg::OnBnClickedReqSettemp)
END_MESSAGE_MAP()


// CKT_CHILERDlg ���b�Z�[�W �n���h��

BOOL CKT_CHILERDlg::OnInitDialog()
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

void CKT_CHILERDlg::OnPaint()
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
HCURSOR CKT_CHILERDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

// ����ȍ~ �蓮�ǉ� -------------------->>>

//�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P
// �~�{�^��
//�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q
void CKT_CHILERDlg::OnClose()
{
//	CDialog::OnClose();
}

//�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P
// �����I���{�^��
//�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q
void CKT_CHILERDlg::OnBnClickedOk()
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
// Windows�V���b�g�_�E��
//�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q
//void CKT_CHILERDlg::OnEndSession(BOOL bEnding)
//{
//	// �����Ȃ�Windows���I�������΍�
//	if( bEnding ) ExitProcess();
//
//	CDialog::OnEndSession(bEnding);
//}

//�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P
// �������N��
//�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q
void CKT_CHILERDlg::OnLbnDblclkList1()
{
	// ListBox�R���g���[���̃v���p�e�B�́A�\�[�g�֎~�ɂ��Ă�����
	// ListBox�R���g���[���̃_�u���N���b�N�C�x���g��ǉ�
	mcls_pLog->ShowEdit();
}
//------------------------------------------
// �e�v���Z�X����������
//------------------------------------------
void CKT_CHILERDlg::InitProcess()
{
	//// �^�X�N����������
	//if( 0 != task_init(XX_CPPSMPEX, 0) ) {		// PCID���Ȃ��ꍇ�B
	if( 0 != task_init(KT_CHILER) ) {
		syslog(SYSNO_TASK_INIT_ERR, "[%s]", KT_CHILER);
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
	// �R���{�{�b�N�X�ɃZ�b�V�������ǉ�
	for(int ii=0; ii<MAX_THREAD; ii++) {
		////// �H�H �Ȃ���Release�̏ꍇ���� ������� _ �������Ă���Ɠo�^���s����
		if( NULL == mcls_MI.gcls_pClrMgr[ii] ) break;
		m_cmbSession.AddString( mcls_MI.gcls_pClrMgr[ii]->GetMyName() );
	}
	m_cmbSession.SetCurSel(0);				// 0�Ԗڂ������I���Ƃ���


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
void CKT_CHILERDlg::ExitProcess()
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
unsigned WINAPI CKT_CHILERDlg::MainThread(void* param)
{
	CKT_CHILERDlg* p = (CKT_CHILERDlg *)param;
	COMMON_QUE que;			// ��M�\����

	//// �e�v���Z�X���������� (����ȑO�̓��O�o�͋֎~)
	p->InitProcess();

	//// �N���v����荞�� ���[�v�J�n
	while(true) {
		recv_mail(&que);			// �N���v����荞��

		switch(que.nEventNo) {

//-----------------------------------------------------------------------------------------------
		case FACT_KS_DB_STATEUS:								// DB��ԕύX�ʒm (��KS_SLAVE)
			break;
		case FACT_KS_PCNAME_CHANGE:								// PC���̕ύX�ʒm (��KS_SLAVE)
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
void CKT_CHILERDlg::OnTimer(UINT_PTR nIDEvent)
{
	CString		sWk;

	//// ���ݑI������Ă���C���X�^���X���擾����
	int nIndex = m_cmbSession.GetCurSel();
	if( -1 == nIndex) return;					// ini�t�@�C���̐ݒ薳��
	ChillerManager* pCls = mcls_MI.gcls_pClrMgr[nIndex];
	if( NULL == pCls ) return;


	//// �X�e�[�^�X�Z�b�g
	SetDlgItemText(IDC_STAT_OPEN,		pCls->IsOpen() ? "��" : "�~" );
	SetDlgItemText(IDC_STAT_CONNECT,	pCls->IsConnect() ? "��" : "�~" );
	SetDlgItemText(IDC_MAIN_MODE,		pCls->GetMainModeName() );
	SetDlgItemText(IDC_UNTEN,			pCls->GetUntenName() );
	sWk.Format("%3.1f", pCls->GetNowTemp());	SetDlgItemText(IDC_GET_TEMP, sWk );
	sWk.Format("%3.1f", pCls->GetSetTemp());	SetDlgItemText(IDC_SET_TEMP, sWk );
	SetDlgItemText(IDC_ERR,				pCls->GetAlmNo() );



	CDialog::OnTimer(nIDEvent);
}

//�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P
// �^�]��ԗv��
//�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q
void CKT_CHILERDlg::OnBnClickedReqUnten()
{
	if ( BST_CHECKED != CWnd::IsDlgButtonChecked(IDC_DEBUG_MANUAL) ) return;
	//// ���ݑI������Ă���C���X�^���X���擾����
	int nIndex = m_cmbSession.GetCurSel();
	if( -1 == nIndex) return;					// ini�t�@�C���̐ݒ薳��
	ChillerManager* pCls = mcls_MI.gcls_pClrMgr[nIndex];
	if( NULL == pCls ) return;

	pCls->AddQue_SendCommand(ChillerManager::SEND_CHECK_MODE);

}
//�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P
// �t������l�v��
//�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q
void CKT_CHILERDlg::OnBnClickedReqNowtemp()
{
	if ( BST_CHECKED != CWnd::IsDlgButtonChecked(IDC_DEBUG_MANUAL) ) return;
	//// ���ݑI������Ă���C���X�^���X���擾����
	int nIndex = m_cmbSession.GetCurSel();
	if( -1 == nIndex) return;					// ini�t�@�C���̐ݒ薳��
	ChillerManager* pCls = mcls_MI.gcls_pClrMgr[nIndex];
	if( NULL == pCls ) return;
	
	pCls->AddQue_SendCommand(ChillerManager::SEND_CHECK_NOW_TEMP);
}
//�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P
// �t���ݒ�l�v��
//�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q
void CKT_CHILERDlg::OnBnClickedReqSettemp()
{
	if ( BST_CHECKED != CWnd::IsDlgButtonChecked(IDC_DEBUG_MANUAL) ) return;
	//// ���ݑI������Ă���C���X�^���X���擾����
	int nIndex = m_cmbSession.GetCurSel();
	if( -1 == nIndex) return;					// ini�t�@�C���̐ݒ薳��
	ChillerManager* pCls = mcls_MI.gcls_pClrMgr[nIndex];
	if( NULL == pCls ) return;
	
	pCls->AddQue_SendCommand(ChillerManager::SEND_CHECK_SET_TEMP);
}
