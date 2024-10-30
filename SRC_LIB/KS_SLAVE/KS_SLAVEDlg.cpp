// KS_SLAVEDlg.cpp : �����t�@�C��
//

#include "stdafx.h"
#include "KS_SLAVE.h"
#include "KS_SLAVEDlg.h"

#include "AllUserLogOff.h"		// �����[�g�������O�I�t

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CKS_SLAVEDlg �_�C�A���O




CKS_SLAVEDlg::CKS_SLAVEDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CKS_SLAVEDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CKS_SLAVEDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, m_ListBox);
}

BEGIN_MESSAGE_MAP(CKS_SLAVEDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_LBN_DBLCLK(IDC_LIST1, &CKS_SLAVEDlg::OnLbnDblclkList1)
	ON_BN_CLICKED(IDOK, &CKS_SLAVEDlg::OnBnClickedOk)
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDC_MYDOWN2, &CKS_SLAVEDlg::OnBnClickedMydown2)
	ON_BN_CLICKED(IDC_MYREBOOT, &CKS_SLAVEDlg::OnBnClickedMyreboot)
	ON_BN_CLICKED(IDC_MYLOGOFF, &CKS_SLAVEDlg::OnBnClickedMylogoff)
	ON_BN_CLICKED(IDC_MYTASKEND, &CKS_SLAVEDlg::OnBnClickedMytaskend)
END_MESSAGE_MAP()


// CKS_SLAVEDlg ���b�Z�[�W �n���h��

BOOL CKS_SLAVEDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// ���̃_�C�A���O�̃A�C�R����ݒ肵�܂��B�A�v���P�[�V�����̃��C�� �E�B���h�E���_�C�A���O�łȂ��ꍇ�A
	//  Framework �́A���̐ݒ�������I�ɍs���܂��B
	SetIcon(m_hIcon, TRUE);			// �傫���A�C�R���̐ݒ�
	SetIcon(m_hIcon, FALSE);		// �������A�C�R���̐ݒ�

	// TODO: �������������ɒǉ����܂��B
//---------------------
	// �Ǝ�
	m_evManualEnd	= CreateEvent(NULL, FALSE, FALSE, NULL);

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

void CKS_SLAVEDlg::OnPaint()
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
HCURSOR CKS_SLAVEDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

// ����ȍ~ �蓮�ǉ� -------------------->>>

//�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P
// �~�{�^��
//�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q
void CKS_SLAVEDlg::OnClose()
{
	// TODO: �����Ƀ��b�Z�[�W �n���h�� �R�[�h��ǉ����邩�A����̏������Ăяo���܂��B
	//CDialog::OnClose();
}
//�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P
// ����{�^��
//�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q
void CKS_SLAVEDlg::OnBnClickedOk()
{
	if( IDYES == MessageBox("�I�����܂����H", "�d�v�Ȃ��m�点", MB_YESNO | MB_DEFBUTTON2 | MB_ICONQUESTION) ) {
		LOG(em_MSG), "����{�^��");
		COMMON_QUE que;
		que.nEventNo = FACT_KS_END;
		que.nLineNo = getlineid();
		send_mail(getmytaskname(), ".", &que);	
		// �N���҂��̂Ƃ��̑΍�
		SetEvent(m_evManualEnd);
	}
}
//�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P
// �������N��
//�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q
void CKS_SLAVEDlg::OnLbnDblclkList1()
{
	// ListBox�R���g���[���̃v���p�e�B�́A�\�[�g�֎~�ɂ��Ă�����
	// ListBox�R���g���[���̃_�u���N���b�N�C�x���g��ǉ�
	mcls_pLog->ShowEdit();
}

//------------------------------------------
// �e�v���Z�X����������
//------------------------------------------
void CKS_SLAVEDlg::InitProcess()
{
	//// ��ԏ��߂ɂ��낢�돉����
	mcls_pLog = NULL;

	// PC�������グ����肷��܂ł�����Ƒ҂�
	int nWaitTime = GetPrivateProfileInt(KS_SLAVE, "START_TIMEWAIT", 5, PCINI_NAME);
	nWaitTime *= 1000;	// [s] �� [ms]
	//Sleep(nWaitTime);	
	if( WAIT_TIMEOUT != WaitForSingleObject(m_evManualEnd, nWaitTime))	{
		// �^�C���A�E�g����Ȃ��B�r�����f
		exit(-9);		// ���v���Z�X�����I��
		return;
	}

	// �^�X�N����������
	setlineid(LINE_ID);
	int nRetc = task_init(KS_SLAVE);
	if( 0 != nRetc ) {
		if(-1 == nRetc) { syslog(SYSNO_TASK_INIT_ERR, "[%s] ���Ƀ^�X�N�N���ς݂ɂ��A�N�����f�B", KS_SLAVE); }
		else			{ syslog(SYSNO_TASK_INIT_ERR, "[%s]", KS_SLAVE); }
		_ASSERT(0);
		exit(-9);		// ���v���Z�X�����I��
		return;
	}
	m_nEndStatus = ProcessManager::P_NOTHING;

	// LogFileManager�C���X�^���X���� (����ȍ~ ���O�o�͉\)
	setlocale(LC_ALL, "Japanese");	// ���ꂪ�����Ɠ��{��p�X���܂ރt�H���_�̏ꍇ�A�ۑ��ł��Ȃ��B(���C�u�����̒��Ŏ��s����ׂ��������Ă��邪�A�ύX����ƃ^�X�N�S�̂ɉe������̂ŁA�Ƃ肠���������Őݒ�)
	mcls_pLog = new LogFileManager(getmytaskname(), &m_ListBox);
	LOG(em_MSG), "�^�X�N�N��");
	LOG(em_MSG), "�e�v���Z�X�J�n���� ���s���E�E�E");

	// ���C���C���X�^���X �J�n����
	mcls_MI.SetLogMgr(mcls_pLog);
	mcls_MI.Init();
	
	//// ��ʍX�V�p�^�C�}����
	//SetTimer(1, 500, NULL);

	// ���C���X���b�h�̎��s���s��
	LOG(em_MSG), "�e�v���Z�X�J�n���� ����");
	mcls_MI.Start();

	// PC�}�X�^�[�Ǘ��m�F�L��
	if(mcls_MI.GetDebug()) {
		// �����m�F
		LOG(em_INF), "�f�o�b�N���s" );
		COMMON_QUE que;
		memset(&que, 0x00, sizeof(que));
		que.nEventNo = FACT_KS_SLAVE_03;
		que.nLineNo = getlineid();															// ���C��ID
		int retc = send_mail( getmytaskname(), ".", &que );
	}
	return;
}

//�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P
// �e�v���Z�X�J������
//�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q
void CKS_SLAVEDlg::ExitProcess()
{
	//// ���O ���X�g�{�b�N�X�\�������I��
	mcls_pLog->SetListOut(false);					// ����ȍ~ ���X�g�{�b�N�X�ւ̃��O�\���֎~

	// ���C���C���X�^���X �I������
	//KillTimer(1);			// ��ʍX�V�^�C�}
	mcls_MI.Stop();
	mcls_MI.Exit();

	//// ���O ���X�g�{�b�N�X�\�������I��
	LOG(em_MSG), "�^�X�N�I��");
	//if(NULL != mcls_pLog){
	//	delete mcls_pLog;
	//	mcls_pLog = NULL;
	//}
	mcls_pLog->SetListOut(false);					// ����ȍ~ ���X�g�{�b�N�X�ւ̃��O�\���֎~


	// �^�X�N�I������
	task_exit();
	CloseHandle( m_evManualEnd );

	// PC�I���敪
	Sleep(2000);			// �풓�^�X�N���S���I������܂ő҂�
	if( ProcessManager::P_NOTHING != m_nEndStatus) {				// �V���b�g�_�E�� // �ċN��
		// �����[�g�Z�b�V�����I��
		AllUserLogOff	clsLogoff;
		clsLogoff.SetLogMgr(mcls_pLog);
		clsLogoff.Exec();


		// PC�I��
		int retc = ProcessManager::ExitWin(m_nEndStatus);
		if( -1 == retc) {			// �V���b�g�_�E�����s
			syslog(SYSNO_SHUTDOWN_ERR, "");
		} else if(-2 == retc) {		// �ċN�����s
			syslog(SYSNO_RESTAET_ERR, "");
		} else if(-3 == retc) {		// ���O�I�t���s
			syslog(SYSNO_LOGOFF_ERR, "");
		}
	}

	//// ���O�N���X�J��
	if(NULL != mcls_pLog){
		delete mcls_pLog;
		mcls_pLog = NULL;
	}


	// ��ʂ̏I���v��
	PostMessage(WM_QUIT);
}

//------------------------------------------
// ���C���X���b�h DlgThread
// LPVOID param �_�C�����O�|�C���^
//------------------------------------------
unsigned WINAPI CKS_SLAVEDlg::MainThread(void* param)
{
	CKS_SLAVEDlg* p = (CKS_SLAVEDlg *)param;
	COMMON_QUE que;			// ��M�\����
	COMMON_QUE sque;		// ���M�\����

	//// �e�v���Z�X���������� (����ȑO�̓��O�o�͋֎~)
	p->InitProcess();

	//// �N���v����荞�� ���[�v�J�n
	while(true) {
		recv_mail(&que);			// �N���v����荞��

		switch(que.nEventNo) {

//-----------------------------------------------------------------------------------------------
		case FACT_KS_SLAVE_01 :								// �����ύX�v�� (��KS_MASTER)
			pLOG(em_MSG), "[MAIL] �����ύX�v�� (%04d/%02d/%02d %02d:%02d:%02d)", 
				que.timeset_que.systemtime.wYear, que.timeset_que.systemtime.wMonth, que.timeset_que.systemtime.wDay,
				que.timeset_que.systemtime.wHour, que.timeset_que.systemtime.wMinute, que.timeset_que.systemtime.wSecond );
			SetLocalTime(&que.timeset_que.systemtime);				// �����ݒ�
			break;

//-----------------------------------------------------------------------------------------------
		case FACT_KS_SLAVE_02 :								// �n�[�g�r�[�g (��KS_MASTER)
			p->mcls_MI.gcls_pCSta->Set_HeatBeatID( que.fl.data[0], true ); 
			break;

//-----------------------------------------------------------------------------------------------
		case FACT_KS_SLAVE_03 :								// PC�}�X�^�[�Ǘ������グ�����ʒm (��KS_MASTER)
			pLOG(em_MSG), "[MAIL] PC�}�X�^�[�Ǘ������グ�����ʒm");

			// ���C���C���X�^���X�֒ʒm
			PostThreadMessage(p->mcls_MI.GetTid(), E_DEF_ML_MASTEROK, 0, 0);
			break;

//-----------------------------------------------------------------------------------------------
		case FACT_KS_SLAVE_04 :								// �V���b�g�_�E���v�� (��KS_MASTER)
			pLOG(em_MSG), "[MAIL] �V���b�g�_�E���v��");
			syslog(SYSNO_SHUTDOWN_OK, "");

			p->m_nEndStatus = ProcessManager::P_POWEROFF;
			sque.nEventNo = FACT_KS_END;
			que.nLineNo = getlineid();															// ���C��ID
			send_mail(getmytaskname(), ".", &sque);
			break;

//-----------------------------------------------------------------------------------------------
		case FACT_KS_SLAVE_05 :								// PC�ċN���v�� (��KS_MASTER)
			pLOG(em_MSG), "[MAIL] PC�ċN���v�� [�Ώۋ敪:%d]", que.fl.data[0] );

			// �~��PC�ȊO�̍ċN���v�����́A�������~��PC�̎��͍ċN�����
			if( 1 == que.fl.data[0] && 0 == strcmp(getmypcname(), TIKUSEKI) ) {
				pLOG(em_MSG), "�~�ϋ@�\�ɑ΂��ċN���������̂ŁA�ċN���v�������");
				break;
			}

			syslog(SYSNO_RESTAET_OK, "");
			// �����܂ŗ�����A�ċN���v�����󂯕t������K���A�ċN�������B
			p->m_nEndStatus = ProcessManager::P_REBOOT;
			sque.nEventNo = FACT_KS_END;
			que.nLineNo = getlineid();															// ���C��ID
			send_mail(getmytaskname(), ".", &sque);	
			break;

//-----------------------------------------------------------------------------------------------
		case FACT_KS_SLAVE_06 :								// PC���O�I�t�v�� (��KS_MASTER)
			pLOG(em_MSG), "[MAIL] PC���O�I�t�v�� [�Ώۋ敪:%d]", que.fl.data[0] );

			// �~��PC�ȊO�̍ċN���v�����́A�������~��PC�̎��͍ċN�����
			if( 1 == que.fl.data[0] && 0 == strcmp(getmypcname(), TIKUSEKI) ) {
				pLOG(em_MSG), "�~�ϋ@�\�ɑ΂����O�I�t�������̂ŁA���O�I�t�v�������");
				break;
			}

			syslog(SYSNO_LOGOFF_OK, "");
			// �����܂ŗ�����A�ċN���v�����󂯕t������K���A�ċN�������B
			p->m_nEndStatus = ProcessManager::P_LOGOFF;
			sque.nEventNo = FACT_KS_END;
			que.nLineNo = getlineid();															// ���C��ID
			send_mail(getmytaskname(), ".", &sque);	
			break;

//-----------------------------------------------------------------------------------------------
		case FACT_KS_SLAVE_07 :								// PC�풓�^�X�N�I���v�� (��KS_MASTER)
			pLOG(em_MSG), "[MAIL] PC�풓�^�X�N�I���v�� [�Ώۋ敪:%d]", que.fl.data[0] );

			// �~��PC�ȊO�̍ċN���v�����́A�������~��PC�̎��͍ċN�����
			if( 1 == que.fl.data[0] && 0 == strcmp(getmypcname(), TIKUSEKI) ) {
				pLOG(em_MSG), "�~�ϋ@�\�ɑ΂��ʒm�������̂ŁA�풓�^�X�N�I���v�������");
				break;
			}

			syslog(SYSNO_SHUTDOWN_OK, "(�풓�^�X�N�̂ݏI��)");
			// �����܂ŗ�����A�ċN���v�����󂯕t������K���A�ċN�������B
			p->m_nEndStatus = ProcessManager::P_NOTHING;
			sque.nEventNo = FACT_KS_END;
			que.nLineNo = getlineid();															// ���C��ID
			send_mail(getmytaskname(), ".", &sque);	
			break;

//-----------------------------------------------------------------------------------------------
		case FACT_KS_PCNAME_CHANGE:															// PC���̕ύX�ʒm�N���v��
			//// �SPC���̂�DB���擾
			if( getpcname() ) {
				pLOG(em_MSG), "[MAIL] �SPC���̂�DB���擾");
			} else {
				pLOG(em_ERR), "[MAIL] �SPC���̂�DB���擾���s");
			}
			//// �풓�v���Z�X�S���ɒʒm
			sque.nEventNo = FACT_KS_PCNAME_CHANGE;
			sque.nLineNo = getlineid();
			ProcessManager::AllProcessSend(&sque);
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
// �f�o�b�N�R�}���h
//�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q
void CKS_SLAVEDlg::OnBnClickedMydown2()
{
	if( IDYES == MessageBox("��PC���V���b�g�_�E�����܂����H", "�d�v�Ȃ��m�点", MB_YESNO | MB_DEFBUTTON2 | MB_ICONQUESTION) ) {
		LOG(em_MSG), "��PC�V���b�g�_�E���{�^��");
		COMMON_QUE que;
		que.nEventNo = FACT_KS_SLAVE_04;
		que.nLineNo = getlineid();															// ���C��ID
		send_mail(getmytaskname(), ".", &que);	
	}
}
void CKS_SLAVEDlg::OnBnClickedMyreboot()
{
	if( IDYES == MessageBox("��PC���ċN�����܂����H", "�d�v�Ȃ��m�点", MB_YESNO | MB_DEFBUTTON2 | MB_ICONQUESTION) ) {
		LOG(em_MSG), "��PC�ċN���{�^��");
		COMMON_QUE que;
		que.nEventNo = FACT_KS_SLAVE_05;
		que.nLineNo = getlineid();															// ���C��ID
		que.fl.data[0] = 0;
		send_mail(getmytaskname(), ".", &que);	
	}
}
void CKS_SLAVEDlg::OnBnClickedMylogoff()
{
	if( IDYES == MessageBox("��PC�����O�I�t���܂����H", "�d�v�Ȃ��m�点", MB_YESNO | MB_DEFBUTTON2 | MB_ICONQUESTION) ) {
		LOG(em_MSG), "��PC���O�I�t�{�^��");
		COMMON_QUE que;
		que.nEventNo = FACT_KS_SLAVE_06;
		que.nLineNo = getlineid();															// ���C��ID
		que.fl.data[0] = 0;
		send_mail(getmytaskname(), ".", &que);	
	}
}

void CKS_SLAVEDlg::OnBnClickedMytaskend()
{
	if( IDYES == MessageBox("��PC�̏풓�^�X�N���I�����܂����H", "�d�v�Ȃ��m�点", MB_YESNO | MB_DEFBUTTON2 | MB_ICONQUESTION) ) {
		LOG(em_MSG), "��PC�풓�^�X�N�I���{�^��");
		COMMON_QUE que;
		que.nEventNo = FACT_KS_SLAVE_07;
		que.nLineNo = getlineid();															// ���C��ID
		que.fl.data[0] = 0;
		send_mail(getmytaskname(), ".", &que);	
	}
}
