// TO_SEQDMDlg.cpp : �����t�@�C��
//

#include "stdafx.h"
#include "TO_SEQDM.h"
#include "TO_SEQDMDlg.h"

// �������[���[�N���o ----->>>
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif
// <<<------------------------

// CTO_SEQDMDlg �_�C�A���O




CTO_SEQDMDlg::CTO_SEQDMDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CTO_SEQDMDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CTO_SEQDMDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, m_ListBox);
	DDX_Control(pDX, IDC_ACTQJ71E71UDP1, m_cCom);
}

BEGIN_MESSAGE_MAP(CTO_SEQDMDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_WM_CLOSE()
	ON_WM_TIMER()
	ON_LBN_DBLCLK(IDC_LIST1, &CTO_SEQDMDlg::OnLbnDblclkList1)
	ON_BN_CLICKED(IDOK, &CTO_SEQDMDlg::OnBnClickedOk)
END_MESSAGE_MAP()


// CTO_SEQDMDlg ���b�Z�[�W �n���h��

BOOL CTO_SEQDMDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// ���̃_�C�A���O�̃A�C�R����ݒ肵�܂��B�A�v���P�[�V�����̃��C�� �E�B���h�E���_�C�A���O�łȂ��ꍇ�A
	//  Framework �́A���̐ݒ�������I�ɍs���܂��B
	SetIcon(m_hIcon, TRUE);			// �傫���A�C�R���̐ݒ�
	SetIcon(m_hIcon, FALSE);		// �������A�C�R���̐ݒ�

	// TODO: �������������ɒǉ����܂��B
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

void CTO_SEQDMDlg::OnPaint()
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
HCURSOR CTO_SEQDMDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}




// ����ȍ~ �蓮�ǉ� -------------------->>>

//�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P
// �~�{�^��
//�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q
void CTO_SEQDMDlg::OnClose()
{
//	CDialog::OnClose();
}


//�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P
// ����{�^��
//�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q
void CTO_SEQDMDlg::OnBnClickedOk()
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
void CTO_SEQDMDlg::OnLbnDblclkList1()
{
	// ListBox�R���g���[���̃v���p�e�B�́A�\�[�g�֎~�ɂ��Ă�����
	// ListBox�R���g���[���̃_�u���N���b�N�C�x���g��ǉ�
	mcls_pLog->ShowEdit();
}

//------------------------------------------
// �e�v���Z�X����������
//------------------------------------------
void CTO_SEQDMDlg::InitProcess()
{
	// �^�X�N����������
	setlineid(LINE_ID);
	if( 0 != task_init(TO_SEQDM) ) {
		syslog(SYSNO_TASK_INIT_ERR, "[%s]", TO_SEQDM);
		_ASSERT(0);
		exit(-9);		// ���v���Z�X�����I��
		return;
	}

	// LogFileManager�C���X�^���X���� (����ȍ~ ���O�o�͉\)
	mcls_pLog = new LogFileManager(getmytaskname(), &m_ListBox);
	LOG(em_MSG), "�^�X�N�N��");
	LOG(em_MSG), "�e�v���Z�X�J�n���� ���s���E�E�E");


	// MX�R���|�[�l���g�̃C���X�^���X��n�� (MainInctance��init�̑O�ɃN���X�ɓn���Ă���)
	KizuFunction::SetStatus("DATAMEM_SEQ_ID", false, false);
	mcls_MI.gcls_DMem.SetMxComponent(&m_cCom);

	// ���C���C���X�^���X �J�n����
	mcls_MI.SetLogMgr(mcls_pLog);
	mcls_MI.Init();

	
	//// MX�R���|�[�l���g�����ڑ�
	m_tidMxConnect = SetTimer(MX_OPEN_RETRY_NID_EVENT, MX_OPEN_RETRY_TIMER, NULL);	

		
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
void CTO_SEQDMDlg::ExitProcess()
{
	//// ���O ���X�g�{�b�N�X�\�������I��
	mcls_pLog->SetListOut(false);					// ����ȍ~ ���X�g�{�b�N�X�ւ̃��O�\���֎~

	// ���C���C���X�^���X �I������
	KillTimer(1);			// ��ʍX�V�^�C�}
	mcls_MI.Stop();
	mcls_MI.Exit();

	//// MX�R���|�[�l���g�I��
	MX_Close();

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
unsigned WINAPI CTO_SEQDMDlg::MainThread(void* param)
{
	CTO_SEQDMDlg* p = (CTO_SEQDMDlg *)param;
	COMMON_QUE que;			// ��M�\����

	//// �e�v���Z�X���������� (����ȑO�̓��O�o�͋֎~)
	p->InitProcess();

	//// �N���v����荞�� ���[�v�J�n
	while(true) {
		recv_mail(&que);			// �N���v����荞��

		switch(que.nEventNo) {
	
		case FACT_KS_PCNAME_CHANGE:							// PC���̕ύX�ʒm (��KS_SLAVE)
			// ��������
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
void CTO_SEQDMDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: �����Ƀ��b�Z�[�W �n���h�� �R�[�h��ǉ����邩�A����̏������Ăяo���܂��B

	if( nIDEvent == MX_OPEN_RETRY_NID_EVENT ) {

		// MX�R���|�[�l���g�����ڑ�
		MX_Connect(false); 
	
	} else {

		// MX�R���|�[�l���g�̏��
		SetDlgItemText(IDC_MX_CONNECT,		mcls_MI.gcls_DMem.IsConnect() ? "��" : "�~");
		SetDlgItemText(IDC_MX_RWSTATUS,		mcls_MI.gcls_DMem.IsStatus() ? "��" : "�~");
	}


	CDialog::OnTimer(nIDEvent);
}


//------------------------------------------
// MX�R���|�[�l���g �I�[�v��
// bool bLogDsp ���O�o��
//------------------------------------------
bool CTO_SEQDMDlg::MX_Connect(bool bLogDsp)
{
	if( mcls_MI.gcls_DMem.IsConnect() ) return true;  
	int  nWk;
	char cWk[32];

	// ----------------------------------------------------------
	// MX�R���|�[�l���g�ڑ���{�ݒ�
	// ----------------------------------------------------------
	//// �p�\�R���� (����)
	// �l�b�g���[�NNo
	nWk = GetPrivateProfileInt(TO_SEQDM, "PC_NETWORK_NO", 0, TASKINI_NAME);		_ASSERT( 0 != nWk);
	m_cCom.put_ActSourceNetworkNumber(nWk);			
	// �ǔ�
	nWk = GetPrivateProfileInt(TO_SEQDM, "PC_STATION_NO", 0, TASKINI_NAME);		_ASSERT( 0 != nWk);
	m_cCom.put_ActSourceStationNumber(nWk);			

	//// �V�[�P���T�� (����)
	// �l�b�g���[�NNo
	nWk = GetPrivateProfileInt(TO_SEQDM, "SEQ_NETWORK_NO", 0, TASKINI_NAME);	_ASSERT( 0 != nWk);
	m_cCom.put_ActNetworkNumber(nWk);	
	// �ǔ�
	nWk = GetPrivateProfileInt(TO_SEQDM, "SEQ_STATION_NO", 0, TASKINI_NAME);	_ASSERT( 0 != nWk);
	m_cCom.put_ActStationNumber(nWk);					

	//// CPU�^�C�v
	nWk = GetPrivateProfileInt(TO_SEQDM, "SEQ_CPU_TYPE", 0, TASKINI_NAME);		_ASSERT( 0 != nWk);
	m_cCom.put_ActCpuType(nWk);						// �}�j���A�� sh080275f.pdf  P53���Q�� 
	
	////// �ʐM
	// �^�C���A�E�g [ms]
	nWk = GetPrivateProfileInt(TO_SEQDM, "CONNECT_TIMEOUT", 0, TASKINI_NAME);	_ASSERT( 0 != nWk);
	m_cCom.put_ActTimeOut(nWk);					
	// �|�[�gNo	
	nWk = GetPrivateProfileInt(TO_SEQDM, "CONNECT_PORT", 0, TASKINI_NAME);		_ASSERT( 0 != nWk);
	m_cCom.put_ActPortNumber(nWk);								 
	// IP�A�h���X
	GetPrivateProfileString(TO_SEQDM,	"CONNECT_IP", "", cWk, sizeof(cWk), TASKINI_NAME ); _ASSERT( 0 != strlen(cWk));
	m_cCom.put_ActHostAddress(cWk);		


	// ----------------------------------------------------------
	// �ڑ�
	// ----------------------------------------------------------
	bool bStatOld = mcls_MI.gcls_DMem.IsConnect();
	// �I�[�v��
	long lRet = m_cCom.Open();
	if( 0 == lRet ) {					// �ڑ�
		KillTimer( m_tidMxConnect );

		//// �ڑ�����
		mcls_MI.gcls_DMem.SetConnect(true);

		LOG(em_MSG), "[Dlg] MX�R���|�[�l���g �I�[�v��");
		syslog(345, "");
		KizuFunction::SetStatus("DATAMEM_SEQ_ID", true);
		return true;
	} else {							// �ؒf
		// �ڑ����s 
		mcls_MI.gcls_DMem.SetConnect(false);

		if(bLogDsp || bStatOld) {	// �O�񂪐���B�v�͐��킩��ُ�ɕς�����Ƃ��̂�
			LOG(em_ERR), "[Dlg] MX�R���|�[�l���g �I�[�v�����s [%X]", lRet);
			syslog(340, "[errcode = %X]", lRet);
			KizuFunction::SetStatus("DATAMEM_SEQ_ID", false, false);
		}
		return false;
	}
}

//------------------------------------------
// MX�R���|�[�l���g �N���[�Y
//------------------------------------------
void CTO_SEQDMDlg::MX_Close()
{
	m_cCom.Close();
	mcls_MI.gcls_DMem.SetConnect(false);

	LOG(em_MSG), "[Dlg] MX�R���|�[�l���g �N���[�Y");
	syslog(346, "");
	KizuFunction::SetStatus("DATAMEM_SEQ_ID", false);
}

