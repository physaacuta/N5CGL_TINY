// TO_PROCONDlg.cpp : �����t�@�C��
//

#include "stdafx.h"
#include "TO_PROCON.h"
#include "TO_PROCONDlg.h"

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


// CTO_PROCONDlg �_�C�A���O




CTO_PROCONDlg::CTO_PROCONDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CTO_PROCONDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CTO_PROCONDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, m_ListBox);
}

BEGIN_MESSAGE_MAP(CTO_PROCONDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDOK, &CTO_PROCONDlg::OnBnClickedOk)
	ON_WM_CLOSE()
	ON_WM_TIMER()
	ON_LBN_DBLCLK(IDC_LIST1, &CTO_PROCONDlg::OnLbnDblclkList1)
//	ON_EN_CHANGE(IDC_STATE_PORT_RECV, &CTO_PROCONDlg::OnEnChangeStatePortRecv)
ON_BN_CLICKED(IDC_DUMP1, &CTO_PROCONDlg::OnBnClickedDump1)
ON_BN_CLICKED(IDC_DUMP2, &CTO_PROCONDlg::OnBnClickedDump2)
ON_BN_CLICKED(IDC_CHK_DEBUG_FUNC, &CTO_PROCONDlg::OnBnClickedChkDebugFunc)
ON_BN_CLICKED(IDC_CHK_LOG_ALLDUMP, &CTO_PROCONDlg::OnBnClickedChkLogAlldump)
ON_BN_CLICKED(IDC_BTN_TCP_CLOSE, &CTO_PROCONDlg::OnBnClickedBtnTcpClose)
ON_BN_CLICKED(IDC_BTN_DBG_SEND, &CTO_PROCONDlg::OnBnClickedBtnDbgSend)
ON_BN_CLICKED(IDC_BTN_DBG_RECV, &CTO_PROCONDlg::OnBnClickedBtnDbgRecv)
END_MESSAGE_MAP()


// CTO_PROCONDlg ���b�Z�[�W �n���h��

BOOL CTO_PROCONDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// ���̃_�C�A���O�̃A�C�R����ݒ肵�܂��B�A�v���P�[�V�����̃��C�� �E�B���h�E���_�C�A���O�łȂ��ꍇ�A
	//  Framework �́A���̐ݒ�������I�ɍs���܂��B
	SetIcon(m_hIcon, TRUE);			// �傫���A�C�R���̐ݒ�
	SetIcon(m_hIcon, FALSE);		// �������A�C�R���̐ݒ�

	// TODO: �������������ɒǉ����܂��B
	
	//// ��ʏ����ݒ�
	CWnd::CheckRadioButton(IDC_DUMP1, IDC_DUMP2, IDC_DUMP1);	// �_���v�o�͏��

// ------------------------------->>>
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

void CTO_PROCONDlg::OnPaint()
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
HCURSOR CTO_PROCONDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}






// ����ȍ~ �蓮�ǉ� -------------------->>>

//�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P
// �~�{�^��
//�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q
void CTO_PROCONDlg::OnClose()
{
	//CDialog::OnClose();
}
//�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P
// ����{�^��
//�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q
void CTO_PROCONDlg::OnBnClickedOk()
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
void CTO_PROCONDlg::OnLbnDblclkList1()
{
	// ListBox�R���g���[���̃v���p�e�B�́A�\�[�g�֎~�ɂ��Ă�����
	// ListBox�R���g���[���̃_�u���N���b�N�C�x���g��ǉ�
	mcls_pLog->ShowEdit();
}

//------------------------------------------
// �e�v���Z�X����������
//------------------------------------------
void CTO_PROCONDlg::InitProcess()
{
	// �^�X�N����������
	setlineid(LINE_ID);
	if( 0 != task_init(TO_PROCON) ) {
		syslog(SYSNO_TASK_INIT_ERR, "[%s]", TO_PROCON);
		_ASSERT(0);
		exit(-9);		// ���v���Z�X�����I��
		return;
	}

	// LogFileManager�C���X�^���X���� (����ȍ~ ���O�o�͉\)
	setlocale(LC_ALL, "Japanese");	// ���ꂪ�����Ɠ��{��p�X���܂ރt�H���_�̏ꍇ�A�ۑ��ł��Ȃ��B(���C�u�����̒��Ŏ��s����ׂ��������Ă��邪�A�ύX����ƃ^�X�N�S�̂ɉe������̂ŁA�Ƃ肠���������Őݒ�)
	mcls_pLog = new LogFileManager(getmytaskname(), &m_ListBox);
	LOG(em_MSG), "[Dlg] �^�X�N�N��");
	LOG(em_MSG), "[Dlg] �e�v���Z�X�J�n���� ���s���E�E�E");

	// ���C���C���X�^���X �J�n����
	mcls_MI.SetLogMgr(mcls_pLog);
	mcls_MI.Init();
	
	//// ��ʍX�V�p�^�C�}����
	SetTimer(1, 500, NULL);

	// ���C���X���b�h�̎��s���s��
	LOG(em_MSG), "[Dlg] �e�v���Z�X�J�n���� ����");
	mcls_MI.Start();
	return;
}

//�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P
// �e�v���Z�X�J������
//�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q
void CTO_PROCONDlg::ExitProcess()
{
	//// ���O ���X�g�{�b�N�X�\�������I��
	mcls_pLog->SetListOut(false);					// ����ȍ~ ���X�g�{�b�N�X�ւ̃��O�\���֎~

	// ���C���C���X�^���X �I������
	KillTimer(1);			// ��ʍX�V�^�C�}
	mcls_MI.Stop();
	mcls_MI.Exit();

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
unsigned WINAPI CTO_PROCONDlg::MainThread(void* param)
{
	CTO_PROCONDlg* p = (CTO_PROCONDlg *)param;
	COMMON_QUE que;			// ��M�\����

	//// �e�v���Z�X���������� (����ȑO�̓��O�o�͋֎~)
	p->InitProcess();

	//// �N���v����荞�� ���[�v�J�n
	while(true) {
		recv_mail(&que);			// �N���v����荞��

		switch(que.nEventNo) {

//-----------------------------------------------------------------------------------------------
		case FACT_TO_PROCON_01:								// �v���R���������ё��M�v�� (��TO_DEFECT)
			if(true) {
				pLOG(em_MSG), "[Mail] ���ё��M�`�����M�ʒm (FACT_TO_PROCON_01) [%s]", que.fc.data);
				p->mcls_MI.gcls_pSock->AddQue_Ziseki(&que);
			}
			break;

//-----------------------------------------------------------------------------------------------
		case FACT_TO_PROCON_09:						// �f�o�b�N�p�`����M�ʒm (��GOT_CONTROL)
			pLOG(em_MSG), "==========================================================================");
			pLOG(em_MSG), "[Mail] �f�o�b�N�p�`����M�ʒm (FACT_TO_PROCON_09) [TC=%d][%s]", que.mix.idata[0], que.mix.cdata[0]);

			// ���C���C���X�^���X�֒ʒm
			p->mcls_MI.SetDeliveryMail(E_DEF_DEBUG_RECV, &que);

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

//-----------------------------------------------------------------------------------------------
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
void CTO_PROCONDlg::OnTimer(UINT_PTR nIDEvent)
{
	CString strVal;	
	// TODO: �����Ƀ��b�Z�[�W �n���h�� �R�[�h��ǉ����邩�A����̏������Ăяo���܂��B

	// �X�e�[�^�X
	SetDlgItemText(IDC_STATE_PORT_RECV, mcls_MI.gcls_pSock->GetStateName(0) );
	SetDlgItemText(IDC_STATE_PORT_SEND, mcls_MI.gcls_pSock->GetStateName(1) );
	SetDlgItemText(IDC_STATE_PORT_RECV2, mcls_MI.gcls_pSock->GetSockStatName(0));
	SetDlgItemText(IDC_STATE_PORT_SEND2, mcls_MI.gcls_pSock->GetSockStatName(1));
	// TCP���
	SetDlgItemText(IDC_EDIT_TCP_R, (mcls_MI.gcls_pSock->IsConnect(0) ? "��" : "�~"));
	SetDlgItemText(IDC_EDIT_TCP_S, (mcls_MI.gcls_pSock->IsConnect(1) ? "��" : "�~"));

	CDialog::OnTimer(nIDEvent);
}

//�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P
// �_���v�o�� 1��
//�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q
void CTO_PROCONDlg::OnBnClickedDump1()
{
	
	// �_���v�o�̓��[�h�Z�b�g
	mcls_MI.gcls_pSock->SetDumpMode(false);
}

//�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P
// �_���v�o�� �S��
//�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q
void CTO_PROCONDlg::OnBnClickedDump2()
{
	// �_���v�o�̓��[�h�Z�b�g
	mcls_MI.gcls_pSock->SetDumpMode(true);
}

//�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P
// [�f�o�b�O�pFunction]�`�F�b�N�{�b�N�X
//�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q
void CTO_PROCONDlg::OnBnClickedChkDebugFunc()
{
	bool bVal = (BST_CHECKED == CWnd::IsDlgButtonChecked(IDC_CHK_DEBUG_FUNC) ? true : false);
	GetDlgItem(IDC_CHK_LOG_ALLDUMP)->EnableWindow(bVal);
	GetDlgItem(IDC_BTN_TCP_CLOSE)->EnableWindow(bVal);
	GetDlgItem(IDC_BTN_DBG_SEND)->EnableWindow(bVal);
	GetDlgItem(IDC_BTN_DBG_RECV)->EnableWindow(bVal);
	GetDlgItem(IDC_EDIT_ID)->EnableWindow(bVal);
	GetDlgItem(IDC_EDIT_DBG_TC)->EnableWindow(bVal);
	GetDlgItem(IDC_EDIT_DBG_BINFILE)->EnableWindow(bVal);
	GetDlgItem(IDC_STATIC_ID)->EnableWindow(bVal);
	GetDlgItem(IDC_STATIC_TC)->EnableWindow(bVal);
	GetDlgItem(IDC_STATIC_BINFILE)->EnableWindow(bVal);
	GetDlgItem(IDC_LIST1)->SetFocus();
}



//�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P
// [���O�S���o��]�`�F�b�N�{�b�N�X�C�x���g 
//�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q
void CTO_PROCONDlg::OnBnClickedChkLogAlldump()
{
	bool bVal = (BST_CHECKED == CWnd::IsDlgButtonChecked(IDC_CHK_LOG_ALLDUMP) ? true : false);
	mcls_MI.gcls_pSock->SetDumpMode(bVal);
	mcls_MI.gcls_pSock->SetDumpMode(bVal);
}

//�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P
// [�����ؒf]�{�^���C�x���g
//�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q
void CTO_PROCONDlg::OnBnClickedBtnTcpClose()
{
	//	if( BST_CHECKED != CWnd::IsDlgButtonChecked(IDC_DEBUG_MANUAL) ) return;
	LOG(em_WAR), "�e�X�g �����ؒf");
	mcls_MI.gcls_pSock->UserClose(false);
}

//�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P
// �[�� ���M����
//�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q
void CTO_PROCONDlg::OnBnClickedBtnDbgSend()
{
	if (IDYES != MessageBox("�e�X�g ���ѓ`���𑗐M���܂����H", "�d�v�Ȃ��m�点", MB_YESNO | MB_DEFBUTTON2 | MB_ICONQUESTION)) {
		return;
	}
	COMMON_QUE que;
	memset(&que, 0x00, sizeof(que));
	que.nEventNo = FACT_TO_PROCON_01;
	que.nLineNo = getlineid();
	GetDlgItemText(IDC_EDIT_ID, &que.fc.data[0], SIZE_SERIAL_FF_NO);
	send_mail(getmytaskname(), ".", &que);
}

//�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P
// �[�� ��M����
//�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q
void CTO_PROCONDlg::OnBnClickedBtnDbgRecv()
{
	if (IDYES != MessageBox("��M���܂����H", "�d�v�Ȃ��m�点", MB_YESNO | MB_DEFBUTTON2 | MB_ICONQUESTION)) {
		return;
	}

		COMMON_QUE que;
		memset( &que, 0x00, sizeof(que) );
		que.nEventNo = FACT_TO_PROCON_09;
		que.nLineNo = getlineid();
	
		char cWk1[256];
		char cWk2[256];
		memset( cWk1, 0x00, sizeof(cWk1) );
		memset( cWk2, 0x00, sizeof(cWk2) );
		GetDlgItemText( IDC_EDIT_DBG_TC     , cWk1, sizeof(cWk1) );
		GetDlgItemText( IDC_EDIT_DBG_BINFILE, cWk2, sizeof(cWk2) );
	
	//	que.mix.idata[0] = atoi( cWk1 );
		que.mix.idata[0] = strtol( cWk1 , NULL, 0); // 10�i/16�i(0x??)���͑Ή�
		memcpy( que.mix.cdata[0], cWk2, sizeof( que.mix.cdata[0]) * 5);
		send_mail( getmytaskname(), ".", &que );
}

