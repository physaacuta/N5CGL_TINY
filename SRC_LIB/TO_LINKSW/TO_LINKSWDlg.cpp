// TO_LINKSWDlg.cpp : �����t�@�C��
//

#include "stdafx.h"
#include "TO_LINKSW.h"
#include "TO_LINKSWDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CTO_LINKSWDlg �_�C�A���O




CTO_LINKSWDlg::CTO_LINKSWDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CTO_LINKSWDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CTO_LINKSWDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, m_ListBox);
	DDX_Control(pDX, IDC_UNIT_DSP, m_cmbUnitDsp);
	DDX_Control(pDX, IDC_BOARD, m_ListBoard);
	DDX_Control(pDX, IDC_UNIT_EXEC, m_cmbUnitExec);
}

BEGIN_MESSAGE_MAP(CTO_LINKSWDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_WM_CLOSE()
	ON_WM_TIMER()
	ON_BN_CLICKED(IDOK, &CTO_LINKSWDlg::OnBnClickedOk)
	ON_LBN_DBLCLK(IDC_LIST1, &CTO_LINKSWDlg::OnLbnDblclkList1)
	ON_BN_CLICKED(IDC_DEBUG_PARAM, &CTO_LINKSWDlg::OnBnClickedDebugParam)
	ON_BN_CLICKED(IDC_DEBUG_START_SET, &CTO_LINKSWDlg::OnBnClickedDebugStartSet)
	ON_BN_CLICKED(IDC_DEBUG_CAMERA_SET, &CTO_LINKSWDlg::OnBnClickedDebugCameraSet)
	ON_BN_CLICKED(IDC_DEBUG_CCMODE, &CTO_LINKSWDlg::OnBnClickedDebugCcmode)
	ON_BN_CLICKED(IDC_DEBUG_SLAVE_SET, &CTO_LINKSWDlg::OnBnClickedDebugSlaveSet)
	ON_BN_CLICKED(IDC_DEBUG_COUNTER_RESET, &CTO_LINKSWDlg::OnBnClickedDebugCounterReset)
	ON_BN_CLICKED(IDC_DEBUG_COUNTER_CHECK, &CTO_LINKSWDlg::OnBnClickedDebugCounterCheck)
	ON_BN_CLICKED(IDC_DEBUG_SET_SERIAL_SPEED, &CTO_LINKSWDlg::OnBnClickedDebugSetSerialSpeed)
END_MESSAGE_MAP()


// CTO_LINKSWDlg ���b�Z�[�W �n���h��

BOOL CTO_LINKSWDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// ���̃_�C�A���O�̃A�C�R����ݒ肵�܂��B�A�v���P�[�V�����̃��C�� �E�B���h�E���_�C�A���O�łȂ��ꍇ�A
	//  Framework �́A���̐ݒ�������I�ɍs���܂��B
	SetIcon(m_hIcon, TRUE);			// �傫���A�C�R���̐ݒ�
	SetIcon(m_hIcon, FALSE);		// �������A�C�R���̐ݒ�

	// TODO: �������������ɒǉ����܂��B

	// ��ʈʒu (�T�C�Y�͂��܂̂܂�)
	::SetWindowPos(m_hWnd, NULL, 0, 1, 0, 0, SWP_NOSIZE | SWP_NOZORDER);

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

	SendDlgItemMessage(	IDC_DEBUG_SERIAL_SPEED_VALUE, CB_ADDSTRING, 0, (LPARAM)"9600");
	SendDlgItemMessage(	IDC_DEBUG_SERIAL_SPEED_VALUE, CB_ADDSTRING, 0, (LPARAM)"115200");
// <<<-------------------------------
	return TRUE;  // �t�H�[�J�X���R���g���[���ɐݒ肵���ꍇ�������ATRUE ��Ԃ��܂��B
}

// �_�C�A���O�ɍŏ����{�^����ǉ�����ꍇ�A�A�C�R����`�悷�邽�߂�
//  ���̃R�[�h���K�v�ł��B�h�L�������g/�r���[ ���f�����g�� MFC �A�v���P�[�V�����̏ꍇ�A
//  ����́AFramework �ɂ���Ď����I�ɐݒ肳��܂��B

void CTO_LINKSWDlg::OnPaint()
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
HCURSOR CTO_LINKSWDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

// ����ȍ~ �蓮�ǉ� -------------------->>>

//�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P
// �~�{�^��
//�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q
void CTO_LINKSWDlg::OnClose()
{
	//CDialog::OnClose();
}

//�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P
// ����{�^��
//�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q
void CTO_LINKSWDlg::OnBnClickedOk()
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
void CTO_LINKSWDlg::OnLbnDblclkList1()
{
	// ListBox�R���g���[���̃v���p�e�B�́A�\�[�g�֎~�ɂ��Ă�����
	// ListBox�R���g���[���̃_�u���N���b�N�C�x���g��ǉ�
	mcls_pLog->ShowEdit();
}

//�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P
// �e�v���Z�X����������
//�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q
void CTO_LINKSWDlg::InitProcess()
{
	// �^�X�N����������
	if( 0 != task_init(TO_LINKSW) )
	{
		syslog(SYSNO_TASK_INIT_ERR, "[%s]", TO_LINKSW);
		_ASSERT(0);
		exit(-9);		// ���v���Z�X�����I��
		return;
	}

	//// LogFileManager�C���X���^�X���� (����ȍ~�A���O�o�͉\)
	setlocale(LC_ALL, "Japanese");	// ���ꂪ�����Ɠ��{��p�X���܂ރt�H���_�̏ꍇ�A�ۑ��ł��Ȃ��B (���C�u�����̒��Ŏ��s����ׂ��������Ă��邪�A�ύX����ƃ^�X�N�S�̂ɉe������̂ŁA�Ƃ肠���������Őݒ�)
	mcls_pLog = new LogFileManager(getmytaskname(), &m_ListBox);
	LOG(em_MSG), "[Dlg] �^�X�N�N��");
	LOG(em_MSG), "[Dlg] �e�v���Z�X�J�n���� ���s���E�E�E");

	//-----------------------
	// �{�[�h�ڑ����
	if(true) {
		mcnt_pListBoard = new DspListView(&m_ListBoard);
		// �����ڒ�`
		m_ListBoard.SetExtendedStyle(m_ListBoard.GetExtendedStyle() | LVS_EX_GRIDLINES );			// �r���`��
		m_ListBoard.SetExtendedStyle(m_ListBoard.GetExtendedStyle() | LVS_EX_FULLROWSELECT );		// 1�s�S�̑I��
		m_ListBoard.ModifyStyle( 0, LVS_SHOWSELALWAYS);			// ��t�H�[�J�X�����I�����c��

		// ��ǉ�			0		1	
		//					2		3			4			5			6			7			8			9
		//					10		11			12			13			14			15			16
		//					17		18
		char name[][32] = { "ID",	"�ð��"
						, "�ް��1",  "2",  "3",  "4",  "5",  "6",  "7",  "8"
						, "9", "10", "11", "12", "13", "14", "15"
						, "���͉�","�޲��"};
		int  wid[]		= { 23, 	50
						, 50,		50,			50,			50,			50,			50,			50,			50
						, 50,		50,			50,			50,			50,			50,			50
						, 50,		50};
		mcnt_pListBoard->SetColumnAll(sizeof(wid) / sizeof(int), 32, wid, (char const*)name);
		// �s�ǉ�
		mcnt_pListBoard->SetRowAll(2 * NUM_CAMLINKSW * MAX_CAMLINKSW_FPGA);

	}

	//// ���C���C���X�^���X �J�n����
	mcls_MI.SetLogMgr(mcls_pLog);
	mcls_MI.Init();

	//// ��ʍX�V�p�f�[�^�Z�b�g
	// �R���{�{�b�N�X�ɃZ�b�V�������ǉ�
	m_cmbUnitExec.AddString("�S���j�b�g");
	for (int ii = 0; ii<MAX_THREAD; ii++) {
		////// �H�H �Ȃ���Release�̏ꍇ���� ������� _ �������Ă���Ɠo�^���s����
		if (NULL == mcls_MI.gcls_pLinksw[ii]) break;
		m_cmbUnitDsp.AddString(mcls_MI.gcls_pLinksw[ii]->GetMySession());
		m_cmbUnitExec.AddString(mcls_MI.gcls_pLinksw[ii]->GetMySession());
	}
	m_cmbUnitDsp.SetCurSel(0);				// 0�Ԗڂ������I���Ƃ���
	m_cmbUnitExec.SetCurSel(0);

	//// ��ʍX�V�p�^�C�}����
	SetTimer(1, 500, NULL);

	//// ���C���X���b�h�̎��s���s��
	LOG(em_MSG), "[Dlg] �e�v���Z�X�J�n���� ����");
	mcls_MI.Start();
	return;
}

//�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P
// �e�v���Z�X�I������
//�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q
void CTO_LINKSWDlg::ExitProcess()
{
	//// ���C���C���X���^�X �I������
	KillTimer(1);			// ��ʍX�V�^�C�}
	mcls_MI.Stop();
	mcls_MI.Exit();

	//// ���O ���X�g�{�b�N�X�\�������I��
	mcls_pLog->SetListOut(false);					// ����ȍ~ ���X�g�{�b�N�X�ւ̃��O�\���֎~

	//// ���O�N���X���
	LOG(em_MSG), "[Dlg] �^�X�N�I��");
	if( NULL != mcls_pLog )
	{
		delete mcls_pLog;
		mcls_pLog = NULL;
	}

	delete mcnt_pListBoard;

	//// �^�X�N�I������
	task_exit();

	// ��ʂ̏I���v��
	PostMessage(WM_QUIT);
}

//�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P
// ���C���X���b�h DlgThread
// LPVOID param �_�C�A���O�|�C���^
//�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q
unsigned WINAPI CTO_LINKSWDlg::MainThread(LPVOID param)
{
	CTO_LINKSWDlg * p = (CTO_LINKSWDlg *)param;
	COMMON_QUE que;			// ��M�\����

	//// �e�v���Z�X���������� (����ȑO�̓��O�o�͋֎~)
	p->InitProcess();

	//// �N���v����荞�� ���[�v�J�n
	while( true )
	{
		recv_mail(&que);			// �N���v����荞��

		switch( que.nEventNo )
		{
//-----------------------------------------------------------------------------------------------
		case FACT_TO_LINKSW_01:								// �p�����[�^�ύX�v�� (��TO_DEFECT)
			pLOG(em_MSG), "[MAIL] �p�����[�^�ύX�v�� (FACT_TO_LINKSW_01) [�����\�ʏ��=%d]", que.fl.data[0]);

			p->CommandSender(&que);
			break;

//-----------------------------------------------------------------------------------------------
		case FACT_TO_LINKSW_02:								// �����p�ݒ�R�}���h (��TO_DEFECT)
			pLOG(em_MSG), "[MAIL] �����p�ݒ�R�}���h (FACT_TO_LINKSW_02)");

			p->CommandSender(&que);
			break;

//-----------------------------------------------------------------------------------------------
		case FACT_TO_LINKSW_03:								// �J�����I��ݒ�R�}���h (��TO_DEFECT)
			// 2017.02.27 Master/Slave��DB�w�� ---------->>>>
			//pLOG(em_MSG), "[MAIL] �J�����I��ݒ�R�}���h (FACT_TO_LINKSW_03) [ID=%d �{�[�h=%d �J�����I��=%d]", que.fl.data[0], que.fl.data[1], que.fl.data[2]);
			pLOG(em_MSG), "[MAIL] �J�����I��ݒ�R�}���h (FACT_TO_LINKSW_03) [ID=%d �{�[�h=%d �J�����I��=%d Master/Slave�敪=%d]", que.fl.data[0], que.fl.data[1], que.fl.data[2], que.fl.data[3]);
			// 2017.02.27 Master/Slave��DB�w�� <<<<----------

			p->CommandSender(&que);
			break;

//-----------------------------------------------------------------------------------------------
		case FACT_TO_LINKSW_04:								// �r���Q���ݒ�R�}���h (��TO_DEFECT)
			pLOG(em_MSG), "[MAIL] �r���Q���ݒ�R�}���h (FACT_TO_LINKSW_04) [�\���敪=%d �r���Q���t���[��No=%u]", que.fl.data[0], (DWORD)que.fl.data[1]);

			p->CommandSender(&que);
			break;

//-----------------------------------------------------------------------------------------------
		case FACT_TO_LINKSW_05:								// �������Z�b�g�ʒm (��TO_DEFECT)
			pLOG(em_MSG), "[MAIL] �������Z�b�g�ʒm (FACT_TO_LINKSW_05) [�J�E���^���Z�b�g�t���[��No=%d]", que.fl.data[0]);

			p->CommandSender(&que);
			break;

//-----------------------------------------------------------------------------------------------
		case FACT_TO_LINKSW_06:								// AOC���x�ύX�w��( udp )

			// �����ʐM�������̏ꍇ�́A�����Ŏ~�߂�
			if(true) {
				int ii_s, ii_e;
				bool bRet = true;
				p->mcls_MI.GetChLoop(que.nLineNo, ii_s, ii_e);

				for (int ii = ii_s; ii < ii_e; ii++) {
					if (!p->mcls_MI.gcls_pLinksw[ii]->IsEnableRapidSerial()) bRet = false;
				}
				if( bRet ){
					pLOG(em_MSG), "[MAIL] AOC�V���A���ݒ� (FACT_TO_LINKSW_06) [BoardID=%d, Speed=%d]", que.fl.data[0], que.fl.data[1]);

					p->CommandSender(&que);
				}else{

					pLOG(em_WAR), "[MAIL] AOC�V���A���ݒ� (FACT_TO_LINKSW_06) �ʐM���x�֖ؑ����ׁ̈A�˗���j���I [BoardID=%d, Speed=%d]", que.fl.data[0], que.fl.data[1]);
				}
			}
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

//-----------------------------------------------------------------------------------------------
		default:
			// ���肦�Ȃ��͂��I�I
			pLOG(em_MSG), "[MAIL] �C�x���gNo���� [EventNo=%d]", que.nEventNo);
			syslog(SYSNO_MAILNO_ERR, "[EventNo=%d]", que.nEventNo);
			break;
		}
	}

	return 0;
}

//�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P
// �R�}���h�����J�n�v��
// COMMON_QUE * que
//�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q
void CTO_LINKSWDlg::CommandSender(COMMON_QUE * que)
{
	//COMMON_QUE * pCq = new COMMON_QUE;
	//memcpy(pCq, que, sizeof(COMMON_QUE));
	//mcls_MI.gcls_pLinksw->gque_Deli.AddToTail(pCq);
	int ii_s, ii_e;
	mcls_MI.GetChLoop(que->nLineNo, ii_s, ii_e);

	for (int ii = ii_s; ii<ii_e; ii++) {
		COMMON_QUE *pCq = new COMMON_QUE;
		memcpy(pCq, que, sizeof(COMMON_QUE));

		mcls_MI.gcls_pLinksw[ii]->gque_Deli.AddToTail(pCq);
	}
}

//�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P
// ��ʍX�V�^�C�}
//�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q
void CTO_LINKSWDlg::OnTimer(UINT_PTR nIDEvent)
{
	//// �C���X�^���X�I��
	int nIdx = m_cmbUnitDsp.GetCurSel();
	if (-1 == nIdx) return;					// ini�t�@�C���̐ݒ薳��

	CString sWk;
	LinkswManager::PARAM	param = mcls_MI.gcls_pLinksw[nIdx]->GetParamInf();	// �p�����[�^

	static bool bIsInitEvent = true;		// ����̍X�V�^�C�}�H

	// ����X�V���̂ݎ��s
	if( bIsInitEvent ){

		bIsInitEvent = false;

		if( ! mcls_MI.gcls_pLinksw[nIdx]->IsEnableRapidSerial() ){

			// �����ؑւ������̏ꍇ�́A�����ؑ֗p�̃f�o�b�O���j���[�𖳌��ɂ���
			CWnd::GetDlgItem(IDC_DEBUG_SET_SERIAL_SPEED)->EnableWindow(false);		// �uAOC���x�ύX�v �{�^��
			CWnd::GetDlgItem(IDC_DEBUG_CAMNO)->EnableWindow(false);					// �u�{�[�hNO�v    �e�L�X�g�{�b�N�X
			CWnd::GetDlgItem(IDC_DEBUG_SERIAL_SPEED_VALUE)->EnableWindow(false);	// �u�V���A�����x�v�R���{�{�b�N�X
			CWnd::GetDlgItem(IDC_STATIC_CAMNO)->EnableWindow(false);				// �u�{�[�hNO�v    ���x��
			CWnd::GetDlgItem(IDC_STATIC_SERIAL_SPEED)->EnableWindow(false);			// �u�V���A�����x�v���x��
		}
	}

	// ������
	SetDlgItemText(IDC_EDIT_RS232C,		(mcls_MI.gcls_pLinksw[nIdx]->IsConnect() ? "��" : "�~"));

	// �Œ�p�����[�^
	SetDlgItemInt(IDC_PARAM_CAMTAPNUM,	param.nLineBuffer);
	SetDlgItemInt(IDC_PARAM_YSIZE,		param.nYSize);

	// �J���������N�X�C�b�`�ݒ�
	//int txtID[MAX_CAMLINKSW_DAISYCHAIN * MAX_CAMLINKSW_FPGA] =
	//{
	//	IDC_ID0,  IDC_ID1,  IDC_ID2,  IDC_ID3,
	//	IDC_ID4,  IDC_ID5,  IDC_ID6,  IDC_ID7,
	//	IDC_ID8,  IDC_ID9,  IDC_ID10, IDC_ID11,
	//	IDC_ID12, IDC_ID13, IDC_ID14, IDC_ID15
	//};
	//int txtStatus[MAX_CAMLINKSW_DAISYCHAIN * MAX_CAMLINKSW_FPGA] =
	//{
	//	IDC_STATUS0,  IDC_STATUS1,  IDC_STATUS2,  IDC_STATUS3,
	//	IDC_STATUS4,  IDC_STATUS5,  IDC_STATUS6,  IDC_STATUS7,
	//	IDC_STATUS8,  IDC_STATUS9,  IDC_STATUS10, IDC_STATUS11,
	//	IDC_STATUS12, IDC_STATUS13, IDC_STATUS14, IDC_STATUS15
	//};
	//int txtBoard[MAX_CAMLINKSW_DAISYCHAIN * MAX_CAMLINKSW_FPGA][MAX_CAMLINKSW_OUTPUT] =
	//{
	//	{
	//		IDC_BOARD0_0,  IDC_BOARD1_0,  IDC_BOARD2_0,  IDC_BOARD3_0,  IDC_BOARD4_0,
	//		IDC_BOARD5_0,  IDC_BOARD6_0,  IDC_BOARD7_0,  IDC_BOARD8_0,  IDC_BOARD9_0,
	//		IDC_BOARD10_0, IDC_BOARD11_0, IDC_BOARD12_0, IDC_BOARD13_0, IDC_BOARD14_0
	//	},
	//	{
	//		IDC_BOARD0_1,  IDC_BOARD1_1,  IDC_BOARD2_1,  IDC_BOARD3_1,  IDC_BOARD4_1,
	//		IDC_BOARD5_1,  IDC_BOARD6_1,  IDC_BOARD7_1,  IDC_BOARD8_1,  IDC_BOARD9_1,
	//		IDC_BOARD10_1, IDC_BOARD11_1, IDC_BOARD12_1, IDC_BOARD13_1, IDC_BOARD14_1
	//	},
	//	{
	//		IDC_BOARD0_2,  IDC_BOARD1_2,  IDC_BOARD2_2,  IDC_BOARD3_2,  IDC_BOARD4_2,
	//		IDC_BOARD5_2,  IDC_BOARD6_2,  IDC_BOARD7_2,  IDC_BOARD8_2,  IDC_BOARD9_2,
	//		IDC_BOARD10_2, IDC_BOARD11_2, IDC_BOARD12_2, IDC_BOARD13_2, IDC_BOARD14_2
	//	},
	//	{
	//		IDC_BOARD0_3,  IDC_BOARD1_3,  IDC_BOARD2_3,  IDC_BOARD3_3,  IDC_BOARD4_3,
	//		IDC_BOARD5_3,  IDC_BOARD6_3,  IDC_BOARD7_3,  IDC_BOARD8_3,  IDC_BOARD9_3,
	//		IDC_BOARD10_3, IDC_BOARD11_3, IDC_BOARD12_3, IDC_BOARD13_3, IDC_BOARD14_3
	//	},
	//	{
	//		IDC_BOARD0_4,  IDC_BOARD1_4,  IDC_BOARD2_4,  IDC_BOARD3_4,  IDC_BOARD4_4,
	//		IDC_BOARD5_4,  IDC_BOARD6_4,  IDC_BOARD7_4,  IDC_BOARD8_4,  IDC_BOARD9_4,
	//		IDC_BOARD10_4, IDC_BOARD11_4, IDC_BOARD12_4, IDC_BOARD13_4, IDC_BOARD14_4
	//	},
	//	{
	//		IDC_BOARD0_5,  IDC_BOARD1_5,  IDC_BOARD2_5,  IDC_BOARD3_5,  IDC_BOARD4_5,
	//		IDC_BOARD5_5,  IDC_BOARD6_5,  IDC_BOARD7_5,  IDC_BOARD8_5,  IDC_BOARD9_5,
	//		IDC_BOARD10_5, IDC_BOARD11_5, IDC_BOARD12_5, IDC_BOARD13_5, IDC_BOARD14_5
	//	},
	//	{
	//		IDC_BOARD0_6,  IDC_BOARD1_6,  IDC_BOARD2_6,  IDC_BOARD3_6,  IDC_BOARD4_6,
	//		IDC_BOARD5_6,  IDC_BOARD6_6,  IDC_BOARD7_6,  IDC_BOARD8_6,  IDC_BOARD9_6,
	//		IDC_BOARD10_6, IDC_BOARD11_6, IDC_BOARD12_6, IDC_BOARD13_6, IDC_BOARD14_6
	//	},
	//	{
	//		IDC_BOARD0_7,  IDC_BOARD1_7,  IDC_BOARD2_7,  IDC_BOARD3_7,  IDC_BOARD4_7,
	//		IDC_BOARD5_7,  IDC_BOARD6_7,  IDC_BOARD7_7,  IDC_BOARD8_7,  IDC_BOARD9_7,
	//		IDC_BOARD10_7, IDC_BOARD11_7, IDC_BOARD12_7, IDC_BOARD13_7, IDC_BOARD14_7
	//	},
	//	{
	//		IDC_BOARD0_8,  IDC_BOARD1_8,  IDC_BOARD2_8,  IDC_BOARD3_8,  IDC_BOARD4_8,
	//		IDC_BOARD5_8,  IDC_BOARD6_8,  IDC_BOARD7_8,  IDC_BOARD8_8,  IDC_BOARD9_8,
	//		IDC_BOARD10_8, IDC_BOARD11_8, IDC_BOARD12_8, IDC_BOARD13_8, IDC_BOARD14_8
	//	},
	//	{
	//		IDC_BOARD0_9,  IDC_BOARD1_9,  IDC_BOARD2_9,  IDC_BOARD3_9,  IDC_BOARD4_9,
	//		IDC_BOARD5_9,  IDC_BOARD6_9,  IDC_BOARD7_9,  IDC_BOARD8_9,  IDC_BOARD9_9,
	//		IDC_BOARD10_9, IDC_BOARD11_9, IDC_BOARD12_9, IDC_BOARD13_9, IDC_BOARD14_9
	//	},
	//	{
	//		IDC_BOARD0_10,  IDC_BOARD1_10,  IDC_BOARD2_10,  IDC_BOARD3_10,  IDC_BOARD4_10,
	//		IDC_BOARD5_10,  IDC_BOARD6_10,  IDC_BOARD7_10,  IDC_BOARD8_10,  IDC_BOARD9_10,
	//		IDC_BOARD10_10, IDC_BOARD11_10, IDC_BOARD12_10, IDC_BOARD13_10, IDC_BOARD14_10
	//	},
	//	{
	//		IDC_BOARD0_11,  IDC_BOARD1_11,  IDC_BOARD2_11,  IDC_BOARD3_11,  IDC_BOARD4_11,
	//		IDC_BOARD5_11,  IDC_BOARD6_11,  IDC_BOARD7_11,  IDC_BOARD8_11,  IDC_BOARD9_11,
	//		IDC_BOARD10_11, IDC_BOARD11_11, IDC_BOARD12_11, IDC_BOARD13_11, IDC_BOARD14_11
	//	},
	//	{
	//		IDC_BOARD0_12,  IDC_BOARD1_12,  IDC_BOARD2_12,  IDC_BOARD3_12,  IDC_BOARD4_12,
	//		IDC_BOARD5_12,  IDC_BOARD6_12,  IDC_BOARD7_12,  IDC_BOARD8_12,  IDC_BOARD9_12,
	//		IDC_BOARD10_12, IDC_BOARD11_12, IDC_BOARD12_12, IDC_BOARD13_12, IDC_BOARD14_12
	//	},
	//	{
	//		IDC_BOARD0_13,  IDC_BOARD1_13,  IDC_BOARD2_13,  IDC_BOARD3_13,  IDC_BOARD4_13,
	//		IDC_BOARD5_13,  IDC_BOARD6_13,  IDC_BOARD7_13,  IDC_BOARD8_13,  IDC_BOARD9_13,
	//		IDC_BOARD10_13, IDC_BOARD11_13, IDC_BOARD12_13, IDC_BOARD13_13, IDC_BOARD14_13
	//	},
	//	{
	//		IDC_BOARD0_14,  IDC_BOARD1_14,  IDC_BOARD2_14,  IDC_BOARD3_14,  IDC_BOARD4_14,
	//		IDC_BOARD5_14,  IDC_BOARD6_14,  IDC_BOARD7_14,  IDC_BOARD8_14,  IDC_BOARD9_14,
	//		IDC_BOARD10_14, IDC_BOARD11_14, IDC_BOARD12_14, IDC_BOARD13_14, IDC_BOARD14_14
	//	},
	//	{
	//		IDC_BOARD0_15,  IDC_BOARD1_15,  IDC_BOARD2_15,  IDC_BOARD3_15,  IDC_BOARD4_15,
	//		IDC_BOARD5_15,  IDC_BOARD6_15,  IDC_BOARD7_15,  IDC_BOARD8_15,  IDC_BOARD9_15,
	//		IDC_BOARD10_15, IDC_BOARD11_15, IDC_BOARD12_15, IDC_BOARD13_15, IDC_BOARD14_15
	//	}
	//};
	//int txtAmbientTemp[MAX_CAMLINKSW_DAISYCHAIN * MAX_CAMLINKSW_FPGA] =
	//					  { IDC_AMBIENT_TEMP_0,  IDC_AMBIENT_TEMP_1,  IDC_AMBIENT_TEMP_2,  IDC_AMBIENT_TEMP_3,  IDC_AMBIENT_TEMP_4,  IDC_AMBIENT_TEMP_5,
	//						IDC_AMBIENT_TEMP_6,  IDC_AMBIENT_TEMP_7,  IDC_AMBIENT_TEMP_8,  IDC_AMBIENT_TEMP_9,  IDC_AMBIENT_TEMP_10, IDC_AMBIENT_TEMP_11,
	//						IDC_AMBIENT_TEMP_12, IDC_AMBIENT_TEMP_13, IDC_AMBIENT_TEMP_14, IDC_AMBIENT_TEMP_15 };
	//int txtDieTemp[MAX_CAMLINKSW_DAISYCHAIN * MAX_CAMLINKSW_FPGA] =
	//					  { IDC_FPGADIE_TEMP_0,  IDC_FPGADIE_TEMP_1,  IDC_FPGADIE_TEMP_2,  IDC_FPGADIE_TEMP_3,  IDC_FPGADIE_TEMP_4,  IDC_FPGADIE_TEMP_5,
	//						IDC_FPGADIE_TEMP_6,  IDC_FPGADIE_TEMP_7,  IDC_FPGADIE_TEMP_8,  IDC_FPGADIE_TEMP_9,  IDC_FPGADIE_TEMP_10, IDC_FPGADIE_TEMP_11,
	//						IDC_FPGADIE_TEMP_12, IDC_FPGADIE_TEMP_13, IDC_FPGADIE_TEMP_14, IDC_FPGADIE_TEMP_15 };
	for( int ii=0; ii<NUM_CAMLINKSW * MAX_CAMLINKSW_FPGA; ii++ )
	{
		//SetDlgItemInt(txtID[ii], ii);
		//SetDlgItemText(txtStatus[ii], param.linksw[ii].bStatus ? "Ͻ�" : "�ڰ��");
		mcnt_pListBoard->SetItem(ii*2, EM_DSP_COL_NAME::COL_ID, ii);
		mcnt_pListBoard->SetItem(ii*2, EM_DSP_COL_NAME::COL_STATUS, param.linksw[ii].bStatus ? "Ͻ�" : "�ڰ��");
		for( int jj=0; jj<MAX_CAMLINKSW_OUTPUT; jj++ )
		{
			//SetDlgItemInt(txtBoard[ii][jj], param.linksw[ii].nSelectCam[jj]);
			mcnt_pListBoard->SetItem(ii*2,   jj+ EM_DSP_COL_NAME::COL_BOARD, param.linksw[ii].nSelectCam[jj]);
			if(0 != param.linksw[ii].nSelectCam[jj]) {
				sWk.Format("%d",
					mcls_MI.gcls_pLinksw[nIdx]->GetCounterNow(ii, jj)
				);
			}
			else {
				sWk.Format("");
			}
			mcnt_pListBoard->SetItem(ii*2+1, jj+ EM_DSP_COL_NAME::COL_BOARD, sWk);
		}
		//SetDlgItemInt(txtAmbientTemp[ii], mcls_MI.gcls_pLinksw[nIdx]->GetAmbTemp(ii));
		//SetDlgItemInt(txtDieTemp[ii], mcls_MI.gcls_pLinksw[nIdx]->GetDieTemp(ii));
		mcnt_pListBoard->SetItem(ii*2, EM_DSP_COL_NAME::COL_TEMP_SYU, mcls_MI.gcls_pLinksw[nIdx]->GetAmbTemp(ii));
		mcnt_pListBoard->SetItem(ii*2, EM_DSP_COL_NAME::COL_TEMP_DAI, mcls_MI.gcls_pLinksw[nIdx]->GetDieTemp(ii));
	}
	for( int ii=0; ii<NUM_CAMLINKSW * MAX_CAMLINKSW_FPGA; ii++ )
	{
		for( int jj=0; jj<MAX_CAMLINKSW_ALLINPUT; jj++ )
		{
			if( 0 == param.nCamSelBoardNo[ii][jj] ) continue;

			int nCamSelBoard =  param.nCamSelBoardNo[ii][jj]-1;
			//int nRelativeInputCh = GetDlgItemInt(txtBoard[ii][nCamSelBoard])-1;
			int nRelativeInputCh = atoi(mcnt_pListBoard->GetItem(ii*2, nCamSelBoard+ EM_DSP_COL_NAME::COL_BOARD))-1;
			int nInputCh = nRelativeInputCh + (ii/MAX_CAMLINKSW_FPGA)*MAX_CAMLINKSW_ALLINPUT ;

			//sWk.Format("%u (��)", GetDlgItemInt(txtBoard[ii][ param.nCamSelBoardNo[ii][jj]-1 ]));
			sWk.Format("%u (%d)", 
				nRelativeInputCh + 1,
				param.GetSerialSpeed(nInputCh)/1000
			
				);
			//SetDlgItemText(txtBoard[ii][ nCamSelBoard ], sWk);
			mcnt_pListBoard->SetItem(ii*2,   nCamSelBoard + EM_DSP_COL_NAME::COL_BOARD, sWk);
		}
	}

	CDialog::OnTimer(nIDEvent);
}


// ����ȍ~ �f�o�b�N�p�R�}���h -------------------->>>


//�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P
// �p�����[�^�ύX
//�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q
void CTO_LINKSWDlg::OnBnClickedDebugParam()
{
	//// �C���X�^���X�I��
	int nIdx = m_cmbUnitExec.GetCurSel();
	if (-1 == nIdx) return;					// ini�t�@�C���̐ݒ薳��
	if( BST_CHECKED != CWnd::IsDlgButtonChecked(IDC_DEBUG_MANUAL) ) return;

	LOG(em_WAR), "[DEBUG] �p�����[�^�ύX�{�^��");

	COMMON_QUE que;
	que.nEventNo = FACT_TO_LINKSW_01;				// �p�����[�^�ύX�v��
	//que.nLineNo = getlineid();
	que.nLineNo = nIdx;
	que.fl.data[0] = CH_SCODE_ALL;
	send_mail(TO_LINKSW, ".", &que);
}

//�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P
// �����J�n�ݒ�
//�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q
void CTO_LINKSWDlg::OnBnClickedDebugStartSet()
{
	//// �C���X�^���X�I��
	int nIdx = m_cmbUnitExec.GetCurSel();
	if (-1 == nIdx) return;					// ini�t�@�C���̐ݒ薳��
	if( BST_CHECKED != CWnd::IsDlgButtonChecked(IDC_DEBUG_MANUAL) ) return;

	LOG(em_WAR), "[DEBUG] �����J�n�ݒ�{�^��");

	COMMON_QUE que;
	que.nEventNo = FACT_TO_LINKSW_02;				// �����J�n�ݒ�R�}���h
	//que.nLineNo = getlineid();
	que.nLineNo = nIdx;
	send_mail(TO_LINKSW, ".", &que);
}

//�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P
// �J�����I��ݒ�
//�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q
void CTO_LINKSWDlg::OnBnClickedDebugCameraSet()
{
	//// �C���X�^���X�I��
	int nIdx = m_cmbUnitExec.GetCurSel();
	if (-1 == nIdx) return;					// ini�t�@�C���̐ݒ薳��
	if( BST_CHECKED != CWnd::IsDlgButtonChecked(IDC_DEBUG_MANUAL) ) return;

	LOG(em_WAR), "[DEBUG] �J�����I��ݒ�{�^��");

	COMMON_QUE que;
	que.nEventNo = FACT_TO_LINKSW_03;				// �J�����I��ݒ�R�}���h
	//que.nLineNo = getlineid();
	que.nLineNo = nIdx;
	que.fl.data[0] = GetDlgItemInt(IDC_DEBUG_ID);
	que.fl.data[1] = GetDlgItemInt(IDC_DEBUG_BOARD);
	que.fl.data[2] = GetDlgItemInt(IDC_DEBUG_CAMERA);
	// 2017.02.27 Master/Slave��DB�w�� ---------->>>>
	que.fl.data[3] = GetDlgItemInt(IDC_DEBUG_DIV_MS);
	// 2017.02.27 Master/Slave��DB�w�� <<<<----------
	send_mail(TO_LINKSW, ".", &que);
}

//�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P
// �t���[���O���o����CC���M
//�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q
void CTO_LINKSWDlg::OnBnClickedDebugCcmode()
{
	int nIdx = m_cmbUnitExec.GetCurSel();	if (-1 == nIdx) return;

	int ii_s, ii_e;
	mcls_MI.GetChLoop(nIdx, ii_s, ii_e);

	for (int ii = ii_s; ii < ii_e; ii++) {
		if (BST_CHECKED != CWnd::IsDlgButtonChecked(IDC_DEBUG_MANUAL))
		{
			if (BST_CHECKED != CWnd::IsDlgButtonChecked(IDC_DEBUG_CCMODE))
			{
				CWnd::CheckDlgButton(IDC_DEBUG_CCMODE, 1);
			}
			else
			{
				CWnd::CheckDlgButton(IDC_DEBUG_CCMODE, 0);
			}
			return;
		}

		bool bCCMode = BST_CHECKED == CWnd::IsDlgButtonChecked(IDC_DEBUG_CCMODE) ? true : false;

		LOG(em_WAR), "[DEBUG] �t���[���O���o����CC���M [%s]", bCCMode ? "ON" : "OFF");

		mcls_MI.gcls_pLinksw[ii]->SetCCMode(bCCMode);
	}
}

//�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P
// �r���Q���ݒ�
//�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q
void CTO_LINKSWDlg::OnBnClickedDebugSlaveSet()
{
	//// �C���X�^���X�I��
	int nIdx = m_cmbUnitExec.GetCurSel();
	if (-1 == nIdx) return;					// ini�t�@�C���̐ݒ薳��
	if( BST_CHECKED != CWnd::IsDlgButtonChecked(IDC_DEBUG_MANUAL) ) return;

	LOG(em_WAR), "[DEBUG] �r���Q���ݒ�{�^��");

	COMMON_QUE que;
	que.nEventNo = FACT_TO_LINKSW_04;				// �r���Q���ݒ�R�}���h
	//que.nLineNo = getlineid();
	que.nLineNo = nIdx;
	que.fl.data[0] = GetDlgItemInt(IDC_DEBUG_TORB);
	que.fl.data[1] = GetDlgItemInt(IDC_DEBUG_FNO);
	send_mail(TO_LINKSW, ".", &que);
}

//�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P
// �������Z�b�g�ʒm
//�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q
void CTO_LINKSWDlg::OnBnClickedDebugCounterReset()
{
	//// �C���X�^���X�I��
	int nIdx = m_cmbUnitExec.GetCurSel();
	if (-1 == nIdx) return;					// ini�t�@�C���̐ݒ薳��
	if( BST_CHECKED != CWnd::IsDlgButtonChecked(IDC_DEBUG_MANUAL) ) return;

	LOG(em_WAR), "[DEBUG] �������Z�b�g�ʒm�{�^��");

	COMMON_QUE que;
	que.nEventNo = FACT_TO_LINKSW_05;				// �������Z�b�g�ʒm
	//que.nLineNo = getlineid();
	que.nLineNo = nIdx;
	que.fl.data[0] = GetDlgItemInt(IDC_DEBUG_FNO);
	send_mail(TO_LINKSW, ".", &que);
}

//�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P
// �J�E���^�`�F�b�N
//�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q
void CTO_LINKSWDlg::OnBnClickedDebugCounterCheck()
{
	int nIdx = m_cmbUnitExec.GetCurSel();	if (-1 == nIdx) return;

	int ii_s, ii_e;
	mcls_MI.GetChLoop(nIdx, ii_s, ii_e);

	for (int ii = ii_s; ii < ii_e; ii++) {

		if (BST_CHECKED != CWnd::IsDlgButtonChecked(IDC_DEBUG_MANUAL)) return;

		LOG(em_WAR), "[DEBUG] �J�E���^�`�F�b�N�{�^��");

		// �J�E���^�`�F�b�N�C�x���g�ʒm
		mcls_MI.gcls_pLinksw[ii]->SetCounterCheck();
	}
}
//�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P
// AOC�V���A�����x�ݒ�
//�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q
void CTO_LINKSWDlg::OnBnClickedDebugSetSerialSpeed()
{
	//// �C���X�^���X�I��
	int nIdx = m_cmbUnitExec.GetCurSel();
	if (-1 == nIdx) return;					// ini�t�@�C���̐ݒ薳��
	if( BST_CHECKED != CWnd::IsDlgButtonChecked(IDC_DEBUG_MANUAL) ) return;

	LOG(em_WAR), "[DEBUG] AOC�V���A���{�^��");

	COMMON_QUE que;
	que.nEventNo = FACT_TO_LINKSW_06;				// �r���Q���ݒ�R�}���h
	//que.nLineNo = getlineid();
	que.nLineNo = nIdx;
	que.fl.data[0] = GetDlgItemInt(IDC_DEBUG_CAMNO);
	que.fl.data[1] = GetDlgItemInt(IDC_DEBUG_SERIAL_SPEED_VALUE);
	send_mail(TO_LINKSW, ".", &que);
}
