// TO_FFSERVDlg.cpp : �����t�@�C��
//

#include "stdafx.h"
#include "TO_FFSERV.h"
#include "TO_FFSERVDlg.h"


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


// CTO_FFSERVDlg �_�C�A���O




CTO_FFSERVDlg::CTO_FFSERVDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CTO_FFSERVDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CTO_FFSERVDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_LOG, m_ListBox);
}

BEGIN_MESSAGE_MAP(CTO_FFSERVDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_LBN_DBLCLK(IDC_LIST_LOG, &CTO_FFSERVDlg::OnLbnDblclkListLog)
	ON_WM_CLOSE()
	ON_WM_TIMER()
	ON_BN_CLICKED(IDOK, &CTO_FFSERVDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDC_SEND_ZISEKI, &CTO_FFSERVDlg::OnBnClickedSendZiseki)
	ON_BN_CLICKED(IDC_SEND_MAE, &CTO_FFSERVDlg::OnBnClickedSendMae)
	ON_BN_CLICKED(IDC_DB, &CTO_FFSERVDlg::OnBnClickedDb)
	ON_BN_CLICKED(IDC_BIN, &CTO_FFSERVDlg::OnBnClickedBin)
	ON_BN_CLICKED(IDC_CHK_LOG_ALLDUMP, &CTO_FFSERVDlg::OnBnClickedChkLogAlldump)
	ON_BN_CLICKED(IDC_CHK_DEBUG_FUNCTION, &CTO_FFSERVDlg::OnBnClickedChkDebugFunction)
END_MESSAGE_MAP()


// CTO_FFSERVDlg ���b�Z�[�W �n���h��

BOOL CTO_FFSERVDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// ���̃_�C�A���O�̃A�C�R����ݒ肵�܂��B�A�v���P�[�V�����̃��C�� �E�B���h�E���_�C�A���O�łȂ��ꍇ�A
	//  Framework �́A���̐ݒ�������I�ɍs���܂��B
	SetIcon(m_hIcon, TRUE);			// �傫���A�C�R���̐ݒ�
	SetIcon(m_hIcon, FALSE);		// �������A�C�R���̐ݒ�

	// TODO: �������������ɒǉ����܂��B
	//// ��ʏ����ݒ�
	CWnd::CheckRadioButton(IDC_DB, IDC_BIN, IDC_DB);			// �e�X�g�f�[�^�̎擾��


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

void CTO_FFSERVDlg::OnPaint()
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
HCURSOR CTO_FFSERVDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

// ����ȍ~ �蓮�ǉ� -------------------->>>

//�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P
// �~�{�^��
//�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q
void CTO_FFSERVDlg::OnClose()
{
	//CDialog::OnClose();		// �����̏���������
}

//�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P
// �����I���{�^��
//�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q
void CTO_FFSERVDlg::OnBnClickedOk()
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
void CTO_FFSERVDlg::OnLbnDblclkListLog()
{
	// ListBox�R���g���[���̃v���p�e�B�́A�\�[�g�֎~�ɂ��Ă�����
	// ListBox�R���g���[���̃_�u���N���b�N�C�x���g��ǉ�
	mcls_pLog->ShowEdit();
}

//------------------------------------------
// �e�v���Z�X����������
//------------------------------------------
void CTO_FFSERVDlg::InitProcess()
{
	//// ��ԏ��߂ɂ��낢�돉����
	mcls_pLog = NULL;

	//// �^�X�N����������
	//if( 0 != task_init(TO_FFSERV, 0) ) {		// PCID���Ȃ��ꍇ�B
	//if( 0 != task_init(TO_FFSERV) ) {
	//	syslog(SYSNO_TASK_INIT_ERR, "[%s]", TO_FFSERV);
	//	_ASSERT(0);
	//	exit(-9);		// ���v���Z�X�����I��
	//	return;
	//}
	int nRetc = task_init(TO_FFSERV);
	if( 0 != nRetc ) {
		if(-1 == nRetc) { syslog(SYSNO_TASK_INIT_ERR, "[%s] ���Ƀ^�X�N�N���ς݂ɂ��A�N�����f�B", TO_FFSERV); }
		else			{ syslog(SYSNO_TASK_INIT_ERR, "[%s]", TO_FFSERV); }
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
void CTO_FFSERVDlg::ExitProcess()
{
	//// ���C���C���X�^���X �I������
	KillTimer(1);			// ��ʍX�V�^�C�}
	mcls_MI.Stop();
	mcls_MI.Exit();

	//// ���O ���X�g�{�b�N�X�\�������I��
	mcls_pLog->SetListOut(false);					// ����ȍ~ ���X�g�{�b�N�X�ւ̃��O�\���֎~

	//// ���O�N���X�J��
	LOG(em_MSG), "[Dlg] �^�X�N�I��");
	SAFE_DELETE(mcls_pLog);
	
	// �^�X�N�I������
	task_exit();

	// ��ʂ̏I���v��
	PostMessage(WM_QUIT);
}

//------------------------------------------
// ���C���X���b�h DlgThread
// LPVOID param �_�C�����O�|�C���^
//------------------------------------------
unsigned WINAPI CTO_FFSERVDlg::MainThread(void* param)
{
	CTO_FFSERVDlg* p = (CTO_FFSERVDlg *)param;
	COMMON_QUE que;			// ��M�\����

	//// �e�v���Z�X���������� (����ȑO�̓��O�o�͋֎~)
	p->InitProcess();

	//// �N���v����荞�� ���[�v�J�n
	while(true) {
		recv_mail(&que);			// �N���v����荞��

		switch(que.nEventNo) {

//-----------------------------------------------------------------------------------------------
		case FACT_TO_FFSERV_01:								// �������ё��M�v�� (��TO_PUTDAT)
			pLOG(em_MSG), "[Mail] �������ё��M�v�� (FACT_TO_FFSERV_01)");

			// ����ؒf���͑��M�v�����L���[�ɗ��ߍ��܂Ȃ��A�ڑ����ɗv�����쐬���L���[�ɗ��ߍ���
			if( ! p->mcls_MI.gcls_pSock->IsConnect() ) {
				pLOG(em_ERR), "[Mail] �\�P�b�g�ؒf���� �������ё��M�v�� �L��");	
				break;
			}
			// ���C���C���X�^���X�֒ʒm
			p->mcls_MI.SetDeliveryMail( QUE_SEND_ZISEKI, &que);
			break;


//-----------------------------------------------------------------------------------------------
		case FACT_TO_FFSERV_02:								// �O�H���r���v�� (��TO_MAEDAT)
			pLOG(em_MSG), "[Mail] �O�H���r���v�� (FACT_TO_FFSERV_02) [�R�C��No=%s][����No=%s]", que.mix.cdata[3], que.mix.cdata[2]);

			if( ! p->mcls_MI.gcls_pSock->IsConnect() ) {
				pLOG(em_ERR), "[Mail] �\�P�b�g�ؒf���� �O�H���r���v�� �L��B[�R�C��No=%s][����No=%s]", que.mix.cdata[3], que.mix.cdata[2]);
				break;
			} else if( (PRO_SIZE_NEXTINF_CNT + 1) <= p->mcls_MI.gcls_pSock->gque_Deli.GetCount() ) {	// �o�b�t�@�[�ɂǂ�ǂ񂽂܂�΍�
				pLOG(em_ERR), "[Mail] ��ʗv���ɂ�� �O�H���r���v�� �j��!! [�R�C��No=%s][����No=%s]", que.mix.cdata[3], que.mix.cdata[2]);
				break;
			}
			// ���C���C���X�^���X�֒ʒm
			p->mcls_MI.SetDeliveryMail( QUE_SEND_MAE, &que);
			break;

//-----------------------------------------------------------------------------------------------

		case FACT_KS_PCNAME_CHANGE:							// PC���̕ύX�ʒm (��KS_SLAVE)
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
void CTO_FFSERVDlg::OnTimer(UINT_PTR nIDEvent)
{
	// �X�e�[�^�X
	SetDlgItemText(IDC_STATE_SOCK, mcls_MI.gcls_pSock->GetSockStatName() );
	SetDlgItemText(IDC_STATE_SEND, mcls_MI.gcls_pSock->GetSendStatName() );

	CDialog::OnTimer(nIDEvent);
}

//�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P
// [�f�o�b�O�pFunction]�`�F�b�N�{�b�N�X
//�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q
void CTO_FFSERVDlg::OnBnClickedChkDebugFunction()
{
	bool bVal = (BST_CHECKED == CWnd::IsDlgButtonChecked(IDC_CHK_DEBUG_FUNCTION) ? true : false );
	GetDlgItem(IDC_SEND_ZISEKI    )->EnableWindow(bVal);
	GetDlgItem(IDC_DB             )->EnableWindow(bVal);
	GetDlgItem(IDC_BIN            )->EnableWindow(bVal);
	GetDlgItem(IDC_SEND_MAE       )->EnableWindow(bVal);
	GetDlgItem(IDC_STATIC_MEI_NO  )->EnableWindow(bVal);
	GetDlgItem(IDC_EDIT_KEY		  )->EnableWindow(bVal);
	GetDlgItem(IDC_CHK_LOG_ALLDUMP)->EnableWindow(bVal);
	GetDlgItem(IDC_LIST_LOG       )->SetFocus();
}

//�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P
// �������ё��M�{�^��
//�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q
void CTO_FFSERVDlg::OnBnClickedSendZiseki()
{
	if( IDYES != MessageBox("[�f�o�b�N] ���ѓ`���𑗐M���܂����H", "�d�v�Ȃ��m�点",MB_YESNO | MB_DEFBUTTON2 | MB_ICONQUESTION) ) {
		return;
	}
	COMMON_QUE que;
	memset(&que, 0x00, sizeof(que));
	que.nEventNo = FACT_TO_FFSERV_01;
	que.nLineNo = getlineid();
	// �f�o�b�N�� ���ѕ\�t�@�C������
	strcpy( que.mix.cdata[0], "KizuZissekiOmote.bin" );
	// �f�o�b�N�� ���ї��t�@�C������
	strcpy( que.mix.cdata[1], "KizuZissekiUra.bin" );
	// �f�o�b�N�� ��ԕ\�t�@�C������
	strcpy(que.mix.cdata[2], "KizuStOmote.bin");
	// �f�o�b�N�� ��ԗ��t�@�C������
	strcpy(que.mix.cdata[3], "KizuStUra.bin");
	send_mail(getmytaskname(), ".", &que);
}

//------------------------------------------
// DB�w��{�^���N���b�N�C�x���g
//	
//------------------------------------------
void CTO_FFSERVDlg::OnBnClickedDb()
{
	mcls_MI.gcls_pData->SetReadDataMode( true );
}

//------------------------------------------
// �t�@�C���w��{�^���N���b�N�C�x���g
//	
//------------------------------------------
void CTO_FFSERVDlg::OnBnClickedBin()
{
	mcls_MI.gcls_pData->SetReadDataMode( false );
}

//�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P
// �O�H���r���v�����M�{�^��
//�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q
void CTO_FFSERVDlg::OnBnClickedSendMae()
{
	CString msg;
	char cKey[SIZE_COIL_KEY+1];				// �R�C�����肷�邽�߂̃L�[���
	char cLine_Code[SIZE_KOUTEI_CODE+1];	// �H���R�[�h
	PRO_TYPE_KEY typKey;


	// ������
	memset(&cKey, 0x20, sizeof(cKey));
	memset(&cLine_Code, 0x00, sizeof(cLine_Code));

	if( IDYES != MessageBox("[�f�o�b�N] �O�H���r���v���`���𑗐M���܂����H", "�d�v�Ȃ��m�点",MB_YESNO | MB_DEFBUTTON2 | MB_ICONQUESTION) ) {
		return;
	}
	// ��ʂ��L�[���擾
	GetDlgItemText(IDC_EDIT_KEY, cKey, SIZE_COIL_KEY + 1);
	memset(&typKey, 0x00, sizeof(typKey));
	memcpy(&typKey, cKey, sizeof(typKey));

	// �Œ�
	memcpy(&cLine_Code, KOUTEI_CODE_5CGL, sizeof(KOUTEI_CODE_5CGL));

	COMMON_QUE que;
	memset(&que, 0x00, sizeof(que));
	que.nEventNo = FACT_TO_FFSERV_02;
	que.nLineNo = getlineid();

	memcpy( que.mix.cdata[0], cKey, SIZE_COIL_KEY );			// �R�C�����肷�邽�߂̃L�[���
	memcpy( que.mix.cdata[1], cLine_Code, SIZE_KOUTEI_CODE );	// �H���R�[�h
	memcpy( que.mix.cdata[2], typKey.cMeiNo, SIZE_MEI_NO );		// ����No
	memcpy( que.mix.cdata[3], typKey.cCoilNo, SIZE_COIL_NO );	// �R�C��No

	send_mail(getmytaskname(), ".", &que);
}

//�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P
// [���O�S���o��]�`�F�b�N�{�b�N�X�C�x���g 
//�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q
void CTO_FFSERVDlg::OnBnClickedChkLogAlldump()
{
	bool bVal = (BST_CHECKED == CWnd::IsDlgButtonChecked(IDC_CHK_LOG_ALLDUMP) ? true : false );
	mcls_MI.gcls_pSock->SetDumpMode(bVal);
}
