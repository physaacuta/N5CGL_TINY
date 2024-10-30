// TO_GETDATDlg.cpp : �����t�@�C��
//

#include "stdafx.h"
#include "TO_GETDAT.h"
#include "TO_GETDATDlg.h"

// �������[���[�N���o �蓮�ǉ� ----->>>
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif
// <<<------------------------


// CTO_GETDATDlg �_�C�A���O




CTO_GETDATDlg::CTO_GETDATDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CTO_GETDATDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CTO_GETDATDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, m_ListBox);
}

BEGIN_MESSAGE_MAP(CTO_GETDATDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDOK, &CTO_GETDATDlg::OnBnClickedOk)
	ON_LBN_DBLCLK(IDC_LIST1, &CTO_GETDATDlg::OnLbnDblclkList1)
	ON_WM_CLOSE()
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_RESEND, &CTO_GETDATDlg::OnBnClickedResend)
END_MESSAGE_MAP()


// CTO_GETDATDlg ���b�Z�[�W �n���h��

BOOL CTO_GETDATDlg::OnInitDialog()
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

void CTO_GETDATDlg::OnPaint()
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
HCURSOR CTO_GETDATDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

// ����ȍ~ �蓮�ǉ� -------------------->>>

//�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P
// �~�{�^��
//�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q
void CTO_GETDATDlg::OnClose()
{
	//CDialog::OnClose();		// �����̏���������
}

//�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P
// �����I���{�^��
//�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q
void CTO_GETDATDlg::OnBnClickedOk()
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
void CTO_GETDATDlg::OnLbnDblclkList1()
{
	// ListBox�R���g���[���̃v���p�e�B�́A�\�[�g�֎~�ɂ��Ă�����
	// ListBox�R���g���[���̃_�u���N���b�N�C�x���g��ǉ�
	mcls_pLog->ShowEdit();
}

//------------------------------------------
// �e�v���Z�X����������
//------------------------------------------
void CTO_GETDATDlg::InitProcess()
{
	//// �^�X�N����������
	if( 0 != task_init(TO_GETDAT) ) {
		syslog(SYSNO_TASK_INIT_ERR, "[%s]", TO_GETDAT);
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
void CTO_GETDATDlg::ExitProcess()
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
unsigned WINAPI CTO_GETDATDlg::MainThread(void* param)
{
	CTO_GETDATDlg* p = (CTO_GETDATDlg *)param;
	COMMON_QUE que;			// ��M�\����

	//// �e�v���Z�X���������� (����ȑO�̓��O�o�͋֎~)
	p->InitProcess();

	//// �N���v����荞�� ���[�v�J�n
	while(true) {
		recv_mail(&que);			// �N���v����荞��

		switch(que.nEventNo) {
//-----------------------------------------------------------------------------------------------
		//case FACT_TO_GETDAT_01:									// �p�����[�^�ύX�v�� (��TO_DEFECT)
		//	pLOG(em_MSG), "[MAIL] �p�����[�^�ύX�v���ʒm (FACT_TO_GETDAT_01) [�e�[�u��=%d]", que.fl.data[0] ); 
		//	
		//	// �R�C����M���ɖ���p�����[�^�ǂݍ��݂��s���Ă���̂�
		//	// �p�����[�^�ύX�v�����ɂ͏������s���K�v�͖���
		//	break;

//-----------------------------------------------------------------------------------------------
		case FACT_TO_GETDAT_02:									// �R�C������M�ʒm (��TO_SEQDM)
			pLOG(em_MSG), "[MAIL] �R�C������M�ʒm (FACT_TO_GETDAT_02) [��M���R�[�h=%d]", que.fl.data[0] ); 
			if (0 != que.fl.data[0] && 1 != que.fl.data[0]) {
				pLOG(em_ERR), "[MAIL] �R�C����� ���R�[�h�ʒu�ُ� [%d]", que.fl.data[0]);
				break;		// �ΏۊO
			}

			// ���C���C���X�^���X�֒ʒm
			p->mcls_MI.SetDeliveryMail( E_DEF_ML_COIL, &que);
			break;

//-----------------------------------------------------------------------------------------------
		case FACT_KS_PCNAME_CHANGE:								// PC���̕ύX�ʒm (��KS_SLAVE)
			// ������
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
void CTO_GETDATDlg::OnTimer(UINT_PTR nIDEvent)
{
	// �ŐV�R�C��
	SetDlgItemText(IDC_COILNO,	mcls_MI.gcls_pCoil->GetNewCoil()->basedata.cCoilNoIn);

	SetDlgItemInt(IDC_PCODE1,	mcls_MI.gcls_pCoil->GetNewCoil()->setting.pcode[0] );
	SetDlgItemInt(IDC_PCODE2,	mcls_MI.gcls_pCoil->GetNewCoil()->setting.pcode[1] );

	SetDlgItemInt(IDC_SCODE1,	mcls_MI.gcls_pCoil->GetNewCoil()->setting.scode[0] );
	SetDlgItemInt(IDC_SCODE2,	mcls_MI.gcls_pCoil->GetNewCoil()->setting.scode[1] );

	SetDlgItemInt(IDC_PCODE1_SPMOFF,	mcls_MI.gcls_pCoil->GetNewCoil()->setting.pcode_spmoff[0] );
	SetDlgItemInt(IDC_PCODE2_SPMOFF,	mcls_MI.gcls_pCoil->GetNewCoil()->setting.pcode_spmoff[1] );

	SetDlgItemInt(IDC_SCODE1_SPMOFF,	mcls_MI.gcls_pCoil->GetNewCoil()->setting.scode_spmoff[0] );
	SetDlgItemInt(IDC_SCODE2_SPMOFF,	mcls_MI.gcls_pCoil->GetNewCoil()->setting.scode_spmoff[1] );

	CDialog::OnTimer(nIDEvent);
}

//�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P
// �R�C�����đ��M�{�^��
//�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q
void CTO_GETDATDlg::OnBnClickedResend()
{
	if( IDYES != MessageBox("[�f�o�b�O] �R�C�������đ����܂����H", "�d�v�Ȃ��m�点",MB_YESNO | MB_DEFBUTTON2 | MB_ICONQUESTION) ) {
		return;
	}

	//// ���̓f�[�^�ҏW�Ǘ��ɒʒm
	COMMON_QUE		que;
	memset(&que, 0x00, sizeof(que));
	que.nEventNo = FACT_TO_GETDAT_02;	// �R�C������M�ʒm (��TO_SEQDM)
	que.nLineNo = getlineid();

	int nRec;
	char cRec[20];
	memset(cRec,0x00,sizeof(cRec));
	GetDlgItemText( IDC_READREC , cRec , sizeof(cRec) );
	nRec = atoi(cRec);

	que.fl.data[0] = nRec;

	send_mail(TO_GETDAT, ".", &que);	
}
