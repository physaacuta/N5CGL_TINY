// TO_MAEDATDlg.cpp : �����t�@�C��
//

#include "stdafx.h"
#include "TO_MAEDAT.h"
#include "TO_MAEDATDlg.h"

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

// CTO_MAEDATDlg �_�C�A���O




CTO_MAEDATDlg::CTO_MAEDATDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CTO_MAEDATDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CTO_MAEDATDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, m_ListBox);
}

BEGIN_MESSAGE_MAP(CTO_MAEDATDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_LBN_DBLCLK(IDC_LIST1, &CTO_MAEDATDlg::OnLbnDblclkList1)
	ON_BN_CLICKED(IDOK, &CTO_MAEDATDlg::OnBnClickedOk)
	ON_WM_CLOSE()
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_BUTTON1, &CTO_MAEDATDlg::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON2, &CTO_MAEDATDlg::OnBnClickedButton2)
END_MESSAGE_MAP()


// CTO_MAEDATDlg ���b�Z�[�W �n���h��

BOOL CTO_MAEDATDlg::OnInitDialog()
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

void CTO_MAEDATDlg::OnPaint()
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
HCURSOR CTO_MAEDATDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


// ����ȍ~ �蓮�ǉ� -------------------->>>

//�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P
// �~�{�^��
//�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q
void CTO_MAEDATDlg::OnClose()
{
	//CDialog::OnClose();		// �����̏���������
}

//�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P
// �����I���{�^��
//�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q
void CTO_MAEDATDlg::OnBnClickedOk()
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
void CTO_MAEDATDlg::OnLbnDblclkList1()
{
	// ListBox�R���g���[���̃v���p�e�B�́A�\�[�g�֎~�ɂ��Ă�����
	// ListBox�R���g���[���̃_�u���N���b�N�C�x���g��ǉ�
	mcls_pLog->ShowEdit();
}


//------------------------------------------
// �e�v���Z�X����������
//------------------------------------------
void CTO_MAEDATDlg::InitProcess()
{
	//// ��ԏ��߂ɂ��낢�돉����
	mcls_pLog = NULL;

	//// �^�X�N����������
	//if( 0 != task_init(TO_MAEDAT, 0) ) {		// PCID���Ȃ��ꍇ�B
	//if( 0 != task_init(TO_MAEDAT) ) {
	//	syslog(SYSNO_TASK_INIT_ERR, "[%s]", TO_MAEDAT);
	//	_ASSERT(0);
	//	exit(-9);		// ���v���Z�X�����I��
	//	return;
	//}
	int nRetc = task_init(TO_MAEDAT);
	if( 0 != nRetc ) {
		if(-1 == nRetc) { syslog(SYSNO_TASK_INIT_ERR, "[%s] ���Ƀ^�X�N�N���ς݂ɂ��A�N�����f�B", TO_MAEDAT); }
		else			{ syslog(SYSNO_TASK_INIT_ERR, "[%s]", TO_MAEDAT); }
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
void CTO_MAEDATDlg::ExitProcess()
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
unsigned WINAPI CTO_MAEDATDlg::MainThread(void* param)
{
	CTO_MAEDATDlg* p = (CTO_MAEDATDlg *)param;
	COMMON_QUE que;			// ��M�\����

	//// �e�v���Z�X���������� (����ȑO�̓��O�o�͋֎~)
	p->InitProcess();

	//// �N���v����荞�� ���[�v�J�n
	while (true) {
		recv_mail( &que );			// �N���v����荞��

		switch( que.nEventNo ) {

//-----------------------------------------------------------------------------------------------
		//case FACT_TO_MAEDAT_01:									// �p�����[�^�ύX�v�� (��TO_DEFECT)
		//	pLOG(em_MSG), "[MAIL] �p�����[�^�ύX�v���ʒm (FACT_TO_MAEDAT_01) [�e�[�u��=%d]", que.fl.data[0] ); 
		//	
		//	// �R�C����M���ɖ���p�����[�^�ǂݍ��݂��s���Ă���̂�
		//	// �p�����[�^�ύX�v�����ɂ͏������s���K�v�͖���
		//	break;

//-----------------------------------------------------------------------------------------------
		case FACT_TO_MAEDAT_02:			// �R�C������M (��TO_DEFECT)
			pLOG(em_MSG), "[Mail]�R�C������M�ʒm (FACT_TO_MAEDAT_02) [�Ǘ�No=%s]", que.mix.cdata[0] );
			p->mcls_MI.SetDeliveryMail( E_PD_ML_COIL, &que );
			break;

//-----------------------------------------------------------------------------------------------
		case FACT_TO_MAEDAT_03:			// FF�O�H���r����M�ʒm (��TO_FFSERV)
			pLOG(em_MSG), "[Mail]FF�O�H���r����M�ʒm (FACT_TO_MAEDAT_03) [ID=%s]", que.fc.data );
			p->mcls_MI.SetDeliveryMail( E_PD_ML_FF, &que );
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
void CTO_MAEDATDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: �����Ƀ��b�Z�[�W �n���h�� �R�[�h��ǉ����邩�A����̏������Ăяo���܂��B

	CDialog::OnTimer(nIDEvent);
}

// �R�C������M�ʒm���M
void CTO_MAEDATDlg::OnBnClickedButton1()
{
	// TODO: �����ɃR���g���[���ʒm�n���h�� �R�[�h��ǉ����܂��B
	// 2014.05.09 ���� �f�o�b�O�{�^���ǉ� ------->>>
	if( BST_CHECKED != CWnd::IsDlgButtonChecked(IDC_DEBUG) ) return;
	// 2014.05.09 ���� �f�o�b�O�{�^���ǉ� <<<-------
	if( IDYES != MessageBox("�R�C������M�ʒm�𑗐M���܂����H", "�d�v�Ȃ��m�点",MB_YESNO | MB_DEFBUTTON2 | MB_ICONQUESTION) ) {
		return;
	}
	COMMON_QUE que;
	memset( &que, 0x00, sizeof(que) );
	que.nEventNo = FACT_TO_MAEDAT_02;
	que.mix.idata[0] = 0;				// ��M���R�[�h�ʒu
	GetDlgItemText( IDC_KizuKenNo, que.mix.cdata[0], SIZE_KIZUKEN_NO );		// �Ǘ�No
	que.nLineNo = getlineid();
	
	send_mail(getmytaskname(), ".", &que);
}

// FF�O�H����M�ʒm���M
void CTO_MAEDATDlg::OnBnClickedButton2()
{
	// TODO: �����ɃR���g���[���ʒm�n���h�� �R�[�h��ǉ����܂��B
	// 2014.05.09 ���� �f�o�b�O�{�^���ǉ� ------->>>
	if( BST_CHECKED != CWnd::IsDlgButtonChecked(IDC_DEBUG) ) return;
	// 2014.05.09 ���� �f�o�b�O�{�^���ǉ� <<<-------

	if( IDYES != MessageBox("FF�O�H����M�ʒm�𑗐M���܂����H", "�d�v�Ȃ��m�点",MB_YESNO | MB_DEFBUTTON2 | MB_ICONQUESTION) ) {
		return;
	}
	COMMON_QUE que;
	memset( &que, 0x00, sizeof(que) );
	que.nEventNo = FACT_TO_MAEDAT_03;
	GetDlgItemText( IDC_ID, &que.fc.data[0], SIZE_SERIAL_FF_NO );
	que.nLineNo = getlineid();
	
	send_mail(getmytaskname(), ".", &que);

}