
// TO_PUTDATDlg.cpp : �����t�@�C��
//

#include "stdafx.h"
#include "TO_PUTDAT.h"
#include "TO_PUTDATDlg.h"
#include "afxdialogex.h"

// �������[���[�N���o ----->>>
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif
// <<<------------------------

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CTO_PUTDATDlg �_�C�A���O



CTO_PUTDATDlg::CTO_PUTDATDlg(CWnd* pParent /*=NULL*/)
	: CDialog(IDD_TO_PUTDAT_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CTO_PUTDATDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, m_ListBox);
}

BEGIN_MESSAGE_MAP(CTO_PUTDATDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDOK, &CTO_PUTDATDlg::OnBnClickedOk)
	ON_WM_CLOSE()
	ON_WM_TIMER()
	ON_LBN_DBLCLK(IDC_LIST1, &CTO_PUTDATDlg::OnLbnDblclkList1)
	ON_BN_CLICKED(IDC_DBGCHKBOX, &CTO_PUTDATDlg::OnBnClickedDbgchkbox)
	ON_BN_CLICKED(IDC_BTN_SAISUN, &CTO_PUTDATDlg::OnBnClickedBtnSaisun)
	ON_BN_CLICKED(IDC_BTN_CUT, &CTO_PUTDATDlg::OnBnClickedBtnCut)
END_MESSAGE_MAP()


// CTO_PUTDATDlg ���b�Z�[�W �n���h���[

BOOL CTO_PUTDATDlg::OnInitDialog()
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

void CTO_PUTDATDlg::OnPaint()
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
HCURSOR CTO_PUTDATDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


// ����ȍ~ �蓮�ǉ� -------------------->>>


//�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P
// �~�{�^��
//�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q
void CTO_PUTDATDlg::OnClose()
{
	//CDialog::OnClose();
}

//�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P
// �����I���{�^��
//�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q
void CTO_PUTDATDlg::OnBnClickedOk()
{
	if (IDYES == MessageBox("�I�����܂����H", "�d�v�Ȃ��m�点", MB_YESNO | MB_DEFBUTTON2 | MB_ICONQUESTION))
	{
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
void CTO_PUTDATDlg::OnLbnDblclkList1()
{
	// ListBox�R���g���[���̃v���p�e�B�́A�\�[�g�֎~�ɂ��Ă�����
	// ListBox�R���g���[���̃_�u���N���b�N�C�x���g��ǉ�
	mcls_pLog->ShowEdit();
}

//------------------------------------------
// �e�v���Z�X����������
//------------------------------------------
void CTO_PUTDATDlg::InitProcess()
{
	CString ss;

	//// �^�X�N����������
	if (0 != task_init(TO_PUTDAT))
	{
		syslog(SYSNO_TASK_INIT_ERR, "[%s]", TO_PUTDAT);
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
// �e�v���Z�X�������
//�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q
void CTO_PUTDATDlg::ExitProcess()
{
	//// ���C���C���X�^���X �I������
	KillTimer(1);			// ��ʍX�V�^�C�}
	mcls_MI.Stop();
	mcls_MI.Exit();

	//// ���O ���X�g�{�b�N�X�\�������I��
	mcls_pLog->SetListOut(false);					// ����ȍ~ ���X�g�{�b�N�X�ւ̃��O�\���֎~

	//// ���O�N���X���
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
unsigned WINAPI CTO_PUTDATDlg::MainThread(LPVOID param)
{
	CTO_PUTDATDlg* p = (CTO_PUTDATDlg *)param;
	COMMON_QUE que;					// ��M�\����

	//// �e�v���Z�X���������� (����ȑO�̓��O�o�͋֎~)
	p->InitProcess();

	//// �N���v����荞�� ���[�v�J�n
	while (true)
	{
		recv_mail(&que);			// �N���v����荞��

		switch (que.nEventNo)
		{
//*/
//-----------------------------------------------------------------------------------------------
		//case FACT_TO_PUTDAT_01:									// �p�����[�^�ύX�v�� (��TO_DEFECT)
		//	pLOG(em_MSG), "[MAIL] �p�����[�^�ύX�v���ʒm (FACT_TO_PUTDAT_01) [�e�[�u��=%d]", que.fl.data[0] ); 
		//	
		//	break;
//-----------------------------------------------------------------------------------------------
		case FACT_TO_PUTDAT_02:						// �v���R���̐����ю�M�ʒm (��TO_PROCON)
			pLOG(em_MSG), "[MAIL] �v���R���̐����ю�M�ʒm (FACT_TO_PUTDAT_02) [ID=%s]", que.fc.data);
			// ���C���C���X�^���X�֒ʒm
			p->mcls_MI.SetDeliveryMail(E_DEF_ML_PRO_R_SAI, &que);
			break;

//-----------------------------------------------------------------------------------------------
		case FACT_TO_PUTDAT_03:						// �v���R���J�b�g���ю�M�ʒm (��TO_PROCON)
			pLOG(em_MSG), "[MAIL] �v���R���J�b�g���ю�M�ʒm (FACT_TO_PUTDAT_03) [ID=%s]", que.fc.data);
			// ���C���C���X�^���X�֒ʒm
			p->mcls_MI.SetDeliveryMail(E_DEF_ML_PRO_R_CUT, &que);
			break;

//-----------------------------------------------------------------------------------------------
		case FACT_TO_PUTDAT_04:						// ���я����݊����ʒm (��TO_DEFECT)
			pLOG(em_MSG), "[MAIL] ���я����݊����ʒm (FACT_TO_PUTDAT_04) [�A���T�[����=%d]", que.fl.data[0]);
			// ���C���C���X�^���X�֒ʒm
			p->mcls_MI.SetDeliveryMail(E_DEF_ML_COIL_RESULT_ADD, &que);
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
			pLOG(em_ERR), "[MAIL] �C�x���gNo ���� [EventNo=%d]", que.nEventNo);
			syslog(SYSNO_MAILNO_ERR, "[EventNo=%d]", que.nEventNo);
			break;
		}
	}
	return 0;
}

//�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P
// ��ʍX�V�^�C�}
//�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q
void CTO_PUTDATDlg::OnTimer(UINT_PTR nIDEvent)
{
	CDialog::OnTimer(nIDEvent);
}

//�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P
// �f�o�b�N�@�\�L��/�����`�F�b�N�{�b�N�X
//�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q
void CTO_PUTDATDlg::OnBnClickedDbgchkbox()
{
	bool bVal = (BST_CHECKED == CWnd::IsDlgButtonChecked(IDC_DBGCHKBOX) ? true : false);
	GetDlgItem(IDC_BTN_SAISUN)->EnableWindow(bVal);
	GetDlgItem(IDC_IDINPUT_SAISUN)->EnableWindow(bVal);
	GetDlgItem(IDC_BTN_CUT)->EnableWindow(bVal);
	GetDlgItem(IDC_IDINPUT_CUT)->EnableWindow(bVal);
	GetDlgItem(IDC_LIST1)->SetFocus();
}

//�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P
// �̐����ю�M�ʒm
//�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q
void CTO_PUTDATDlg::OnBnClickedBtnSaisun()
{
	if( IDYES != MessageBox("[�f�o�b�N] �̐����ю�M�ʒm�𑗐M���܂����H", "�d�v�Ȃ��m�点",MB_YESNO | MB_DEFBUTTON2 | MB_ICONQUESTION) )
	{
		return;
	}

	COMMON_QUE que;
	memset(&que, 0x00, sizeof(que));
	que.nEventNo = FACT_TO_PUTDAT_02;
	que.nLineNo = getlineid();
	GetDlgItemText(IDC_IDINPUT_SAISUN, que.fc.data, SIZE_SERIAL_FF_NO);

	LOG(em_WAR), "[DEBUG] �v���R���̐����ю�M�ʒm�{�^�� [ID=%s]", que.fc.data);

	// ���M
	send_mail(getmytaskname(), ".", &que);
}

//�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P
// �J�b�g���ю�M�ʒm
//�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q
void CTO_PUTDATDlg::OnBnClickedBtnCut()
{
	if (IDYES != MessageBox("[�f�o�b�N] �J�b�g���ю�M�ʒm�𑗐M���܂����H", "�d�v�Ȃ��m�点", MB_YESNO | MB_DEFBUTTON2 | MB_ICONQUESTION))
	{
		return;
	}

	COMMON_QUE que;
	memset(&que, 0x00, sizeof(que));
	que.nEventNo = FACT_TO_PUTDAT_03;
	que.nLineNo = getlineid();
	GetDlgItemText(IDC_IDINPUT_CUT, que.fc.data, SIZE_SERIAL_FF_NO);
		
	LOG(em_WAR), "[DEBUG] �v���R���J�b�g���ю�M�ʒm�{�^�� [ID=%s]", que.fc.data);

	// ���M
	send_mail(getmytaskname(), ".", &que);
}
