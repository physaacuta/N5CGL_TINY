// KS_MAILTRDlg.cpp : �����t�@�C��
//

#include "stdafx.h"
#include "KS_MAILTR.h"
#include "KS_MAILTRDlg.h"

//#ifdef _DEBUG
//#define new DEBUG_NEW
//#endif
// �������[���[�N���o �蓮�ǉ� ----->>>
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif
// <<<------------------------


// CKS_MAILTRDlg �_�C�A���O




CKS_MAILTRDlg::CKS_MAILTRDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CKS_MAILTRDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CKS_MAILTRDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, m_ListBox);
}

BEGIN_MESSAGE_MAP(CKS_MAILTRDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDOK, &CKS_MAILTRDlg::OnBnClickedOk)
	ON_LBN_DBLCLK(IDC_LIST1, &CKS_MAILTRDlg::OnLbnDblclkList1)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_DEBUG_MANUAL, &CKS_MAILTRDlg::OnBnClickedDebugManual)
	ON_BN_CLICKED(IDC_BTN_TRANS_FL, &CKS_MAILTRDlg::OnBnClickedBtnTransFl)
	ON_BN_CLICKED(IDC_BTN_TRANS_FC, &CKS_MAILTRDlg::OnBnClickedBtnTransFc)
	ON_BN_CLICKED(IDC_BTN_TRANS_MIX, &CKS_MAILTRDlg::OnBnClickedBtnTransMix)
	ON_BN_CLICKED(IDC_BTN_CLEAR_R, &CKS_MAILTRDlg::OnBnClickedBtnClearR)
END_MESSAGE_MAP()


// CKS_MAILTRDlg ���b�Z�[�W �n���h��

BOOL CKS_MAILTRDlg::OnInitDialog()
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
// ��ʏ������Z�b�g----------------------------------------->>>
	SetDlgItemTextA(IDC_PC, ".");
	SetDlgItemTextA(IDC_LINE, "0");
	GetDlgItem(IDC_IPADDR_UDP)->EnableWindow(false);
// <<<---------------------------------------------------------
	
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

void CKS_MAILTRDlg::OnPaint()
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
HCURSOR CKS_MAILTRDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


// ����ȍ~ �蓮�ǉ� -------------------->>>

//�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P
// �~�{�^��
//�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q
void CKS_MAILTRDlg::OnClose()
{
	//CDialog::OnClose();		// �����̏���������
}

//�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P
// �����I���{�^��
//�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q
void CKS_MAILTRDlg::OnBnClickedOk()
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
void CKS_MAILTRDlg::OnLbnDblclkList1()
{
	// ListBox�R���g���[���̃v���p�e�B�́A�\�[�g�֎~�ɂ��Ă�����
	// ListBox�R���g���[���̃_�u���N���b�N�C�x���g��ǉ�
	mcls_pLog->ShowEdit();
}


//------------------------------------------
// �e�v���Z�X����������
//------------------------------------------
void CKS_MAILTRDlg::InitProcess()
{
	//// ��ԏ��߂ɂ��낢�돉����
	mcls_pLog = NULL;

	int nRetc = task_init(KS_MAILTR);
	if( 0 != nRetc ) {
		if(-1 == nRetc) { syslog(SYSNO_TASK_INIT_ERR, "[%s] ���Ƀ^�X�N�N���ς݂ɂ��A�N�����f�B", KS_MAILTR); }
		else			{ syslog(SYSNO_TASK_INIT_ERR, "[%s]", KS_MAILTR); }
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
void CKS_MAILTRDlg::ExitProcess()
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
unsigned WINAPI CKS_MAILTRDlg::MainThread(void* param)
{
	CKS_MAILTRDlg* p = (CKS_MAILTRDlg *)param;
	COMMON_QUE que;			// ��M�\����

	//// �e�v���Z�X���������� (����ȑO�̓��O�o�͋֎~)
	p->InitProcess();

	//// �N���v����荞�� ���[�v�J�n
	while(true) {
		recv_mail(&que);			// �N���v����荞��

		switch(que.nEventNo) {
//*/
//-----------------------------------------------------------------------------------------------
		case FACT_KS_MAILTR_01:								// ���[���X���b�g�]���v�� (���e�^�X�N)
			pLOG(em_MSG), "[MAIL] ���[���X���b�g�]���v�� (FACT_KS_MAILTR_01) [Task=%s][Pc=%s]", que.mailtr_que.cTaskName, que.mailtr_que.cPcName); 
			// ���C���C���X�^���X�֒ʒm
			p->mcls_MI.SetDeliveryMail( E_DEF_ML_TRANS, &que);
			break;
//*/
//-----------------------------------------------------------------------------------------------
		case FACT_KS_DB_STATEUS:							// DB��ԕύX�ʒm(��KS_SLAVE)
			break;

		case FACT_KS_PCNAME_CHANGE:								// PC���̕ύX�ʒm (��KS_SLAVE)
			pLOG(em_MSG), "[MAIL] PC���̕ύX�ʒm (FACT_KS_PCNAME_CHANGE)" );
			// PC���̎擾
			getpcname(); 
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
void CKS_MAILTRDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: �����Ƀ��b�Z�[�W �n���h�� �R�[�h��ǉ����邩�A����̏������Ăяo���܂��B

	// UDP�]���v�� ��M���擾
	UdpMail* tRecvUdp = mcls_MI.gcls_TrUdpMgr.GetRecvUdp();
	COMMON_QUE	que;
	CString		ss;

	memcpy(&que, tRecvUdp->data, min(sizeof(que), tRecvUdp->datalen));

	SetDlgItemTextA(IDC_PC_R, tRecvUdp->hostname);
	SetDlgItemTextA(IDC_TASK_R, tRecvUdp->mailname);
	ss.Format("%d", que.nEventNo);
	SetDlgItemTextA(IDC_EVENT_R, ss);
	ss.Format("%d", que.nLineNo);
	SetDlgItemTextA(IDC_LINE_R, ss);

	CDialog::OnTimer(nIDEvent);
}

//�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P
// DEBUG�{�^������
//�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q
void CKS_MAILTRDlg::OnBnClickedDebugManual()
{
	bool bVal = ( BST_CHECKED == CWnd::IsDlgButtonChecked(IDC_DEBUG_MANUAL) ? true : false );
	GetDlgItem(IDC_BTN_TRANS_FL)->EnableWindow(bVal);
	GetDlgItem(IDC_BTN_TRANS_FC)->EnableWindow(bVal);
	GetDlgItem(IDC_BTN_TRANS_MIX)->EnableWindow(bVal);
	GetDlgItem(IDC_IPADDR_UDP)->EnableWindow(bVal);
	GetDlgItem(IDC_PC)->EnableWindow(bVal);
	GetDlgItem(IDC_TASK)->EnableWindow(bVal);
	GetDlgItem(IDC_EVENT)->EnableWindow(bVal);
	GetDlgItem(IDC_LINE)->EnableWindow(bVal);
	GetDlgItem(IDC_FL_1)->EnableWindow(bVal);
	GetDlgItem(IDC_FC_1)->EnableWindow(bVal);
	GetDlgItem(IDC_BTN_CLEAR_R)->EnableWindow(bVal);
}

//�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P
// ���l�]���{�^������
//�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q
void CKS_MAILTRDlg::OnBnClickedBtnTransFl()
{
	COMMON_QUE	que;
	CString		sUdpAddr;
	CString		sTaskName;
	CString		sPcName;

	GetDlgItemTextA(IDC_IPADDR_UDP, sUdpAddr);
	GetDlgItemTextA(IDC_TASK, sTaskName);
	GetDlgItemTextA(IDC_PC, sPcName);
	memset(&que, 0x00, sizeof(que));
	que.nEventNo = GetDlgItemInt(IDC_EVENT);
	que.nLineNo = GetDlgItemInt(IDC_LINE);
	que.fl.data[0] = GetDlgItemInt(IDC_FL_1);

	// ���[���]��
	send_mail_trans(sUdpAddr, sTaskName, sPcName, &que);
}

//�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P
// �����]���{�^������
//�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q
void CKS_MAILTRDlg::OnBnClickedBtnTransFc()
{
	COMMON_QUE	que;
	CString		sUdpAddr;
	CString		sTaskName;
	CString		sPcName;

	GetDlgItemTextA(IDC_IPADDR_UDP, sUdpAddr);
	GetDlgItemTextA(IDC_TASK, sTaskName);
	GetDlgItemTextA(IDC_PC, sPcName);
	memset(&que, 0x00, sizeof(que));
	que.nEventNo = GetDlgItemInt(IDC_EVENT);
	que.nLineNo = GetDlgItemInt(IDC_LINE);
	GetDlgItemTextA(IDC_FC_1, que.fc.data, sizeof(que.fc.data));

	// ���[���]��
	send_mail_trans(sUdpAddr, sTaskName, sPcName, &que);
}

//�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P
// �����]���{�^������
//�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q
void CKS_MAILTRDlg::OnBnClickedBtnTransMix()
{
	COMMON_QUE	que;
	CString		sUdpAddr;
	CString		sTaskName;
	CString		sPcName;

	GetDlgItemTextA(IDC_IPADDR_UDP, sUdpAddr);
	GetDlgItemTextA(IDC_TASK, sTaskName);
	GetDlgItemTextA(IDC_PC, sPcName);
	memset(&que, 0x00, sizeof(que));
	que.nEventNo = GetDlgItemInt(IDC_EVENT);
	que.nLineNo = GetDlgItemInt(IDC_LINE);
	que.mix.idata[0] = GetDlgItemInt(IDC_FL_1);
	GetDlgItemTextA(IDC_FC_1, que.mix.cdata[0], sizeof(que.mix.cdata[0]));

	// ���[���]��
	send_mail_trans(sUdpAddr, sTaskName, sPcName, &que);
}

//�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P
// �\���N���A�{�^������
//�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q
void CKS_MAILTRDlg::OnBnClickedBtnClearR()
{
	// ��M���N���A
	mcls_MI.gcls_TrUdpMgr.ClearRecvUdp();
}
