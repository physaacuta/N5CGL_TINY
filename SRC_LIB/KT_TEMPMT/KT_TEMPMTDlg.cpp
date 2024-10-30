// KT_TEMPMTDlg.cpp : �����t�@�C��
//

#include "stdafx.h"
#include "KT_TEMPMT.h"
#include "KT_TEMPMTDlg.h"

// �������[���[�N���o �蓮�ǉ� ----->>>
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif
// <<<------------------------


// CKT_TEMPMTDlg �_�C�A���O


CKT_TEMPMTDlg::CKT_TEMPMTDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CKT_TEMPMTDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CKT_TEMPMTDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, m_ListBox);
	DDX_Control(pDX, IDC_CMB_DISP, m_cmbSession);
	DDX_Control(pDX, IDC_LIST_VAL, m_ListVal);
}

BEGIN_MESSAGE_MAP(CKT_TEMPMTDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_LBN_DBLCLK(IDC_LIST1, &CKT_TEMPMTDlg::OnLbnDblclkList1)
	ON_WM_TIMER()
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDOK, &CKT_TEMPMTDlg::OnBnClickedOk)
END_MESSAGE_MAP()


// CKT_TEMPMTDlg ���b�Z�[�W �n���h��

BOOL CKT_TEMPMTDlg::OnInitDialog()
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

void CKT_TEMPMTDlg::OnPaint()
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
HCURSOR CKT_TEMPMTDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


// ����ȍ~ �蓮�ǉ� -------------------->>>

//�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P
// �~�{�^��
//�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q
void CKT_TEMPMTDlg::OnClose()
{
	//CDialog::OnClose();		// �����̏���������
}

//�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P
// �����I���{�^��
//�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q
void CKT_TEMPMTDlg::OnBnClickedOk()
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
void CKT_TEMPMTDlg::OnLbnDblclkList1()
{
	// ListBox�R���g���[���̃v���p�e�B�́A�\�[�g�֎~�ɂ��Ă�����
	// ListBox�R���g���[���̃_�u���N���b�N�C�x���g��ǉ�
	mcls_pLog->ShowEdit();
}

//------------------------------------------
// �e�v���Z�X����������
//------------------------------------------
void CKT_TEMPMTDlg::InitProcess()
{
	//// ��ԏ��߂ɂ��낢�돉����
	mcls_pLog = NULL;

	//// �^�X�N����������
	int nRetc = task_init(KT_TEMPMT);
	if( 0 != nRetc ) {
		if(-1 == nRetc) { syslog(SYSNO_TASK_INIT_ERR, "[%s] ���Ƀ^�X�N�N���ς݂ɂ��A�N�����f�B", KT_TEMPMT); }
		else			{ syslog(SYSNO_TASK_INIT_ERR, "[%s]", KT_TEMPMT); }
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
	

	//======================================================
	//// ��ʍX�V�p�f�[�^�Z�b�g

	//-----------------------
	// �R���{�{�b�N�X�ɃZ�b�V�������ǉ�
	for(int ii=0; ii<MAX_THREAD; ii++) {
		////// �H�H �Ȃ���Release�̏ꍇ���� ������� _ �������Ă���Ɠo�^���s����
		if( NULL == mcls_MI.gcls_pTempMgr[ii] ) break;
		m_cmbSession.AddString( mcls_MI.gcls_pTempMgr[ii]->GetMyName() );
	}
	m_cmbSession.SetCurSel(0);				// 0�Ԗڂ������I���Ƃ���

	//-----------------------
	// ���X�g�r���[�C���X�^���X���� & ��`
	if(true) {
		mcnt_pListVal = new DspListView(&m_ListVal);
		// �����ڒ�`
		m_ListVal.SetExtendedStyle( m_ListVal.GetExtendedStyle() | LVS_EX_GRIDLINES );			// �r���`��
		m_ListVal.SetExtendedStyle( m_ListVal.GetExtendedStyle() | LVS_EX_FULLROWSELECT );		// 1�s�S�̑I��
		m_ListVal.ModifyStyle( 0, LVS_SHOWSELALWAYS);			// ��t�H�[�J�X�����I�����c��

	
		// ��ǉ�			0			1			2		3			4		5	
		char name[][32] = {	"UnitNo",	"���u��",	"���x",	"���xST",	"�^�]",	"�ð��"	};
		int  wid[]		= {	50,			90,			50,		50,			50,		50		};
		mcnt_pListVal->SetColumnAll(sizeof(wid)/sizeof(int), 32, wid, (char const*)name);
		// �s�ǉ�
		mcnt_pListVal->SetRowAll(MAX_TEMP_UNIT);	
		// �Œ�l�Z�b�g
		for(int ii=0; ii<MAX_TEMP_UNIT; ii++) {
			mcnt_pListVal->SetItem( ii, 0, ii+1);		
		}
	}


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
void CKT_TEMPMTDlg::ExitProcess()
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
	
	//// ��ʃR���g���[���J��
	delete mcnt_pListVal;

	// �^�X�N�I������
	task_exit();

	// ��ʂ̏I���v��
	PostMessage(WM_QUIT);
}

//------------------------------------------
// ���C���X���b�h DlgThread
// LPVOID param �_�C�����O�|�C���^
//------------------------------------------
unsigned WINAPI CKT_TEMPMTDlg::MainThread(void* param)
{
	CKT_TEMPMTDlg* p = (CKT_TEMPMTDlg *)param;
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
void CKT_TEMPMTDlg::OnTimer(UINT_PTR nIDEvent)
{
	CString		sWk;

	//// ���ݑI������Ă���C���X�^���X���擾����
	int nIndex = m_cmbSession.GetCurSel();
	if( -1 == nIndex) return;					// ini�t�@�C���̐ݒ薳��
	TempManager* pCls = mcls_MI.gcls_pTempMgr[nIndex];
	if( NULL == pCls ) return;


	// ���ʕ���
	SetDlgItemText(IDC_CONNECT, pCls->GetConnect());

	// ���x�p�l�����[�^�[��
	int nCnt = pCls->GetUnitNum();
	for(int ii=0; ii<nCnt; ii++) {
		// �e�f�[�^�Z�b�g
		mcnt_pListVal->SetItem( ii, 1, pCls->GetUnitName(ii) );
		sWk.Format("%.1f��", pCls->GetTemp(ii));	mcnt_pListVal->SetItem( ii, 2, sWk );
		sWk = pCls->GetTempSt(ii)?"��":"�~";		mcnt_pListVal->SetItem( ii, 3, sWk );
		mcnt_pListVal->SetItem( ii, 4, pCls->GetUnten(ii) );
		mcnt_pListVal->SetItem( ii, 5, pCls->GetStatus(ii) );

	}


	CDialog::OnTimer(nIDEvent);
}

