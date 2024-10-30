// HT_DEFECTDlg.cpp : �����t�@�C��
//

#include "stdafx.h"
#include "HT_DEFECT.h"
#include "HT_DEFECTDlg.h"

// �������[���[�N���o �蓮�ǉ� ----->>>
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif
// <<<------------------------


// CHT_DEFECTDlg �_�C�A���O


CHT_DEFECTDlg::CHT_DEFECTDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CHT_DEFECTDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CHT_DEFECTDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, m_ListBox);
	//DDX_Control(pDX, IDC_COMBO_MEN, m_cmbMen);
	DDX_Control(pDX, IDC_LIST_ST, m_ListSt);
	DDX_Control(pDX, IDC_LIST_TO, m_ListTo);
}

BEGIN_MESSAGE_MAP(CHT_DEFECTDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDOK, &CHT_DEFECTDlg::OnBnClickedOk)
	ON_WM_CLOSE()
	ON_WM_TIMER()
	ON_LBN_DBLCLK(IDC_LIST1, &CHT_DEFECTDlg::OnLbnDblclkList1)
	ON_BN_CLICKED(IDC_DEBUG_START, &CHT_DEFECTDlg::OnBnClickedDebugStart)
	ON_BN_CLICKED(IDC_DEBUG_STOP, &CHT_DEFECTDlg::OnBnClickedDebugStop)
	ON_BN_CLICKED(IDC_DEBUG_PARAM, &CHT_DEFECTDlg::OnBnClickedDebugParam)
	ON_BN_CLICKED(IDC_DEBUG_COILINF, &CHT_DEFECTDlg::OnBnClickedDebugCoilinf)
	ON_BN_CLICKED(IDC_DEBUG_CANOK, &CHT_DEFECTDlg::OnBnClickedDebugCanok)
//	ON_BN_CLICKED(IDC_DEBUG_FRAME, &CHT_DEFECTDlg::OnBnClickedDebugFrame)
//	ON_BN_CLICKED(IDC_DEBUG_PROFILE, &CHT_DEFECTDlg::OnBnClickedDebugProfile)
//	ON_BN_CLICKED(IDC_DEBUG_DLYCANCEL, &CHT_DEFECTDlg::OnBnClickedDebugDlycancel)
	ON_BN_CLICKED(IDC_DEBUG_IMGKIND, &CHT_DEFECTDlg::OnBnClickedDebugImgkind)
	ON_BN_CLICKED(IDC_DEBUG_PARAM_PRELOAD, &CHT_DEFECTDlg::OnBnClickedDebugParamPreload)
	ON_BN_CLICKED(IDC_DEBUG_SET_POSITION, &CHT_DEFECTDlg::OnBnClickedDebugSetPosition)
	ON_BN_CLICKED(IDC_DEBUG_RESUME_START, &CHT_DEFECTDlg::OnBnClickedDebugResumeStart)
	ON_BN_CLICKED(IDC_DEBUG_RESUME_STOP, &CHT_DEFECTDlg::OnBnClickedDebugResumeStop)
	ON_BN_CLICKED(IDD_DEBUG_KIND1, &CHT_DEFECTDlg::OnBnClickedDebugKind1)
	ON_BN_CLICKED(IDD_DEBUG_KIND2, &CHT_DEFECTDlg::OnBnClickedDebugKind2)
	ON_BN_CLICKED(IDD_DEBUG_KIND3, &CHT_DEFECTDlg::OnBnClickedDebugKind3)
END_MESSAGE_MAP()


// CHT_DEFECTDlg ���b�Z�[�W �n���h��

BOOL CHT_DEFECTDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// ���̃_�C�A���O�̃A�C�R����ݒ肵�܂��B�A�v���P�[�V�����̃��C�� �E�B���h�E���_�C�A���O�łȂ��ꍇ�A
	//  Framework �́A���̐ݒ�������I�ɍs���܂��B
	SetIcon(m_hIcon, TRUE);			// �傫���A�C�R���̐ݒ�
	SetIcon(m_hIcon, FALSE);		// �������A�C�R���̐ݒ�

	// TODO: �������������ɒǉ����܂��B

	//// ��ʏ����ݒ�
	CWnd::CheckDlgButton(IDC_DEBUG_MANUAL, 1);

	CWnd::CheckRadioButton(IDD_DEBUG_KIND1, IDD_DEBUG_KIND3, IDD_DEBUG_KIND1);						// ����@�\
	CWnd::CheckRadioButton(IDC_DEBUG_START_MODE1, IDC_DEBUG_START_MODE4, IDC_DEBUG_START_MODE1);	// �^�]���[�h

	SetDlgItemInt(IDC_DEBUG_CAMPAIR, 1);
	SetDlgItemInt(IDC_DEBUG_CAMSET1, 1);
	SetDlgItemInt(IDC_DEBUG_CAMSET2, 2);
	SetDlgItemInt(IDC_DEBUG_SCODE, MAX_SCODE);

	CWnd::GetDlgItem(IDC_DEBUG_CAMSET1)->EnableWindow(FALSE);
	CWnd::GetDlgItem(IDC_DEBUG_CAMSET2)->EnableWindow(FALSE);
	
	// �蓮�ǉ� -------------------->>>
	// ��ʈʒu (�T�C�Y�͂��܂̂܂�)
	::SetWindowPos(m_hWnd, NULL, 400, 600, 0, 0, SWP_NOSIZE | SWP_NOZORDER);

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

void CHT_DEFECTDlg::OnPaint()
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
HCURSOR CHT_DEFECTDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



// ����ȍ~ �蓮�ǉ� -------------------->>>

//�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P
// �~�{�^��
//�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q
void CHT_DEFECTDlg::OnClose()
{
	//CDialog::OnClose();
}

//�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P
// �����I���{�^��
//�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q
void CHT_DEFECTDlg::OnBnClickedOk()
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
void CHT_DEFECTDlg::OnLbnDblclkList1()
{
	// ListBox�R���g���[���̃v���p�e�B�́A�\�[�g�֎~�ɂ��Ă�����
	// ListBox�R���g���[���̃_�u���N���b�N�C�x���g��ǉ�
	mcls_pLog->ShowEdit();
}

//------------------------------------------
// �e�v���Z�X����������
//------------------------------------------
void CHT_DEFECTDlg::InitProcess()
{
	//// �^�X�N����������
	//if( 0 != task_init(XX_CPPSMPEX, 0) ) {		// PCID���Ȃ��ꍇ�B
	if( 0 != task_init(HT_DEFECT) ) {
		syslog(SYSNO_TASK_INIT_ERR, "[%s]", HT_DEFECT);
		_ASSERT(0);
		exit(-9);		// ���v���Z�X�����I��
		return;
	}

	//// LogFileManager�C���X�^���X���� (����ȍ~ ���O�o�͉\)
	setlocale(LC_ALL, "Japanese");	// ���ꂪ�����Ɠ��{��p�X���܂ރt�H���_�̏ꍇ�A�ۑ��ł��Ȃ��B(���C�u�����̒��Ŏ��s����ׂ��������Ă��邪�A�ύX����ƃ^�X�N�S�̂ɉe������̂ŁA�Ƃ肠���������Őݒ�)
	mcls_pLog = new LogFileManager(getmytaskname(), &m_ListBox, 50, 5);
	LOG(em_MSG), "[Dlg] �^�X�N�N��");
	LOG(em_MSG), "[Dlg] �e�v���Z�X�J�n���� ���s���E�E�E");

	mcls_pLogCull = new LogFileManager("CULL", NULL);
	LOGCULL(em_MSG), "[Dlg] �^�X�N�N��");

	//// ���C���C���X�^���X �J�n����
	mcls_MI.SetLogMgr(mcls_pLog);
	mcls_MI.SetLogMgrCull(mcls_pLogCull);
	mcls_MI.Init();
	

	//// ��ʍX�V�p�f�[�^�Z�b�g
	//// �R���{�{�b�N�X�ɃZ�b�V�������ǉ�
	//CString sWk;
	//for(int ii=0; ii<HT_CONNECT_NUM; ii++) {
	//	sWk.Format("%d", ii+1);
	//	m_cmbMen.AddString( sWk );
	//}
	//m_cmbMen.SetCurSel(0);				// 0�Ԗڂ������I���Ƃ���


	//-----------------------
	// ��� ���X�g�r���[�C���X�^���X���� & ��`
	if(true) {
		mcnt_pListSt = new DspListView(&m_ListSt);
		// �����ڒ�`
		m_ListSt.SetExtendedStyle(m_ListSt.GetExtendedStyle() | LVS_EX_GRIDLINES );			// �r���`��
		m_ListSt.SetExtendedStyle(m_ListSt.GetExtendedStyle() | LVS_EX_FULLROWSELECT );		// 1�s�S�̑I��

		// ��ǉ�			0		1			
		char name[][32] = {	"����",	"�l"};
		int  wid[]		= {	60, 	160	};
		mcnt_pListSt->SetColumnAll(sizeof(wid)/sizeof(int), 32, wid, (char const*)name);
		// �s�ǉ�
		mcnt_pListSt->SetRowAll(8);
		// �Œ�l�Z�b�g
		mcnt_pListSt->SetItem(0, 0, "�X�e�[�^�X");
		mcnt_pListSt->SetItem(1, 0, "�^�]���");
		mcnt_pListSt->SetItem(2, 0, "�Ώۖ�");
		mcnt_pListSt->SetItem(3, 0, "��׾��");
		mcnt_pListSt->SetItem(4, 0, "���̈ꗗ");
		mcnt_pListSt->SetItem(5, 0, "����@�\");
		mcnt_pListSt->SetItem(6, 0, "�\�ʏ��");
		mcnt_pListSt->SetItem(7, 0, "�摜�`��");
	}

	//-----------------------
	// �Γ������ ���X�g�r���[�C���X�^���X���� & ��`
	if (true) {
		mcnt_pListTo = new DspListView(&m_ListTo);
		// �����ڒ�`
		m_ListTo.SetExtendedStyle(m_ListTo.GetExtendedStyle() | LVS_EX_GRIDLINES);			// �r���`��
		m_ListTo.SetExtendedStyle(m_ListTo.GetExtendedStyle() | LVS_EX_FULLROWSELECT);		// 1�s�S�̑I��

		// ��ǉ�			0		1			
		char name[][32] = { "����",	"�l" };
		int  wid[] = { 100, 	120 };
		mcnt_pListTo->SetColumnAll(sizeof(wid) / sizeof(int), 32, wid, (char const*)name);
		// �s�ǉ�
		mcnt_pListTo->SetRowAll(16);
		// �Œ�l�Z�b�g
		mcnt_pListTo->SetItem(0, 0, "����������");
		mcnt_pListTo->SetItem(1, 0, "To��� ��/�ő�");
		mcnt_pListTo->SetItem(2, 0, "Eng��MFrameNo");
		mcnt_pListTo->SetItem(3, 0, "�������MFrameNo");

		mcnt_pListTo->SetItem(4, 0, "��M�Ԋu[ms]");
		mcnt_pListTo->SetItem(5, 0, "���S��������");
		mcnt_pListTo->SetItem(6, 0, "1�ڰя���[ms]");
		mcnt_pListTo->SetItem(7, 0, "�S���֘A[ms]");
		mcnt_pListTo->SetItem(8, 0, "�����֘A[ms]");
		mcnt_pListTo->SetItem(9, 0, "�ڰ�Jpeg[ms]");

		mcnt_pListTo->SetItem(11, 0, "1�S��������");
		mcnt_pListTo->SetItem(12, 0, "1�S����/Max");
		mcnt_pListTo->SetItem(13, 0, "2�S��������");
		mcnt_pListTo->SetItem(14, 0, "2�S����/Max");
		mcnt_pListTo->SetItem(15, 0, "�S���֘A[ms]");
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
void CHT_DEFECTDlg::ExitProcess()
{
	//// ���C���C���X�^���X �I������
	KillTimer(1);			// ��ʍX�V�^�C�}
	mcls_MI.Stop(20000);
	mcls_MI.Exit();

	//// ���O ���X�g�{�b�N�X�\�������I��
	mcls_pLog->SetListOut(false);					// ����ȍ~ ���X�g�{�b�N�X�ւ̃��O�\���֎~

	//// ���O�N���X�J��
	LOG(em_MSG), "[Dlg] �^�X�N�I��");
	if(NULL != mcls_pLog){
		delete mcls_pLog;
		mcls_pLog = NULL;
	}
	LOGCULL(em_MSG), "[Dlg] �^�X�N�I��");
	if(NULL != mcls_pLogCull){
		delete mcls_pLogCull;
		mcls_pLogCull = NULL;
	}
	
	//// ��ʃR���g���[���J��
	delete mcnt_pListSt;
	delete mcnt_pListTo;


	// �^�X�N�I������
	task_exit();

	// ��ʂ̏I���v��
	PostMessage(WM_QUIT);
}

//------------------------------------------
// ���C���X���b�h DlgThread
// LPVOID param �_�C�����O�|�C���^
//------------------------------------------
unsigned WINAPI CHT_DEFECTDlg::MainThread(void* param)
{
	CHT_DEFECTDlg* p = (CHT_DEFECTDlg *)param;
	COMMON_QUE que;			// ��M�\����

	//// �e�v���Z�X���������� (����ȑO�̓��O�o�͋֎~)
	p->InitProcess();

	//// �N���v����荞�� ���[�v�J�n
	while(true) {
		recv_mail(&que);			// �N���v����荞��

		switch(que.nEventNo) {

//-----------------------------------------------------------------------------------------------
		case FACT_HT_DEFECT_01:								// �p�����[�^�ύX�ʒm (��TO_DEFECT)
			pLOG(em_MSG), "[MAIL] �p�����[�^�ύX�ʒm (FACT_HT_DEFECT_01) [dly=%d, scode=%d, id=%d(0:�\,1:��,2:����), kind=%d(1:nc,2:pd,3:pr)]",
				que.fl.data[0], que.fl.data[2], que.fl.data[1], que.fl.data[3]); 
			
			// ���C���C���X�^���X�֒ʒm
			p->mcls_MI.SetDeliveryMail( E_DEF_ML_PARAM, &que);
			break;


//-----------------------------------------------------------------------------------------------
		case FACT_HT_DEFECT_02:								// �����J�n�v�� (��TO_DEFECT)
			if(true) {
				int campair = que.fl.data[ 5 + (getpcid() -1)*2 + 0 ]; 
				int kind    = que.fl.data[ 5 + (getpcid() -1)*2 + 1 ]; 
				if(0 == campair) {										// �ΏۊO���`�F�b�N
					pLOG(em_MSG), "[MAIL] �����J�n�v�� �ΏۊO�I�I (FACT_HT_DEFECT_02) [mode=%s, scode=%d:%d, pair=%d, kind=%d(1:nc,2:pd,3:pr)]",
						p->mcls_MI.gcls_Status.GetUntenName((EM_DIV_UNTEN)que.fl.data[0]), que.fl.data[1], que.fl.data[2], campair, kind); 
					break;
				}

				pLOG(em_MSG), "[MAIL] �����J�n�v�� (FACT_HT_DEFECT_02) [mode=%s, scode=%d:%d, pair=%d, kind=%d(1:nc,2:pd,3:pr) len=%d:%d]",
					p->mcls_MI.gcls_Status.GetUntenName((EM_DIV_UNTEN)que.fl.data[0]), que.fl.data[1], que.fl.data[2], campair, kind, que.fl.data[3], que.fl.data[4]); 

				//// ���C���C���X�^���X�֒ʒm
				p->mcls_MI.SetDeliveryMail( E_DEF_ML_START, &que);
			}
			break;


//-----------------------------------------------------------------------------------------------
		case FACT_HT_DEFECT_03:								// ������~�v�� (��TO_DEFECT)
			if(true) {
				int umu = que.fl.data[ getpcid() -1]; 
				pLOG(em_MSG), "[MAIL] ������~�v�� (FACT_HT_DEFECT_03) [%s]", 0==umu?"�Ώ�":"�ΏۊO"); 
				if(0 != umu) break;

				// ���C���C���X�^���X�֒ʒm
				p->mcls_MI.SetDeliveryMail( E_DEF_ML_STOP, &que);
			}
			break;

//-----------------------------------------------------------------------------------------------
		case FACT_HT_DEFECT_04:								// Jpeg���k���ݒ� (��TO_DEFECT)
			pLOG(em_MSG), "[MAIL] Jpeg���k���ݒ� (FACT_HT_DEFECT_04) [JpegQuality=%d]", que.fl.data[0]); 

			// ���C���C���X�^���X�֒ʒm
			p->mcls_MI.gcls_pSend->SetJpegQuality(que.fl.data[0]); 
			break;


//-----------------------------------------------------------------------------------------------
		case FACT_HT_DEFECT_05:								// ��ʏ��ݒ�v�� (��TO_DEFECT)
			pLOG(em_MSG), "[MAIL] ��ʏ��ݒ�v�� (FACT_HT_DEFECT_05) [dly=%d, size=%d] (%d/%d) scode(%d)", 
				que.fl.data[0], que.fl.data[1], que.fl.data[2], que.fl.data[3], que.fl.data[4]);

			// ���C���C���X�^���X�֒ʒm
			p->mcls_MI.SetDeliveryMail( E_DEF_ML_COILINF, &que);
			break;


//-----------------------------------------------------------------------------------------------
		case FACT_HT_DEFECT_07:								// ���s�s�\���� (��SO_GAMENN)
			pLOG(em_MSG), "[MAIL] ���s�s�\���� (FACT_HT_DEFECT_07)"); 

			// ���C���C���X�^���X�֒ʒm
			p->mcls_MI.SetDeliveryMail( E_DEF_ML_CANOK, &que);
			break;


//-----------------------------------------------------------------------------------------------
		case FACT_HT_DEFECT_08:								// ��Ԗ₢���킹 (��TO_DEFECT)
			pLOG(em_MSG), "[MAIL] ��Ԗ₢���킹 (FACT_HT_DEFECT_08)"); 

			// ���C���C���X�^���X�֒ʒm
			p->mcls_MI.SetDeliveryMail( E_DEF_ML_STATUS, &que);
			break;

//-----------------------------------------------------------------------------------------------
		case FACT_HT_DEFECT_19:								// �����s�ݒ�̃L�����Z���˗� (��TO_DEFECT)
			pLOG(em_MSG), "[MAIL] �����s�ݒ�̃L�����Z���˗� (FACT_HT_DEFECT_19) [��=%d Kind=%d]", que.fl.data[0], que.fl.data[1]);

			// ���C���C���X�^���X�֒ʒm
			p->mcls_MI.SetDeliveryMail(E_DEF_ML_CLEAR_QUEUE, &que);
			break;

//-----------------------------------------------------------------------------------------------
		case FACT_HT_DEFECT_20:								// ���������J�E���^�ݒ�˗� (��TO_DEFECT)
			pLOG(em_MSG), "[MAIL] ���������J�E���^�ݒ�˗� (FACT_HT_DEFECT_20) [��=%d Kind=%d]", que.fl.data[0], que.fl.data[1]);

			// ���C���C���X�^���X�֒ʒm
			p->mcls_MI.SetDeliveryMail( E_DEF_ML_SET_POSITION, &que);
			break;

//-----------------------------------------------------------------------------------------------
		case FACT_HT_DEFECT_21:								// �p���[���[�^���O�Ǎ��˗� (��TO_DEFECT)
			pLOG(em_MSG), "[MAIL] �p���[���[�^���O�Ǎ��˗� (FACT_HT_DEFECT_21) [��=%d Kind=%d scode=%d]", que.fl.data[0], que.fl.data[1], que.fl.data[2]);  

			// ���C���C���X�^���X�֒ʒm
			p->mcls_MI.SetDeliveryMail( E_DEF_ML_PRELOAD_PARAM, &que);
			break;


//-----------------------------------------------------------------------------------------------
		case FACT_HT_DEFECT_22:								// �r���J�n�v�� (��TO_DEFECT)
			if (true)
			{
				int	pcid = (getpcid() - 1);
				int	nStartFrameNo = que.fl.data[5];
				int camset1 = que.fl.data[6 + pcid * 3 + 0];
				int camset2 = que.fl.data[6 + pcid * 3 + 1];
				EM_DIV_HANTEI emKind = (EM_DIV_HANTEI)que.fl.data[6 + pcid * 3 + 2];

				if (DIV_HANTEI_PR == emKind)
				{
					// �p����������̂ݏ����ΏۂƂ���
					if (0 != camset1 && 0 != camset2)
					{
						// 0�̂܂ܑ���Ɖ����ɑ΂��Ă̗v���Ȃ̂�������Ȃ����߁A
						// -1�ő��M���A���葤��0�ɖ߂�
						if (-1 == camset1) que.fl.data[6 + pcid * 3 + 0] = 0;
						if (-1 == camset2) que.fl.data[6 + pcid * 3 + 1] = 0;

						pLOG(em_MSG), "[MAIL] �r���J�n�v�� (FACT_HT_DEFECT_22) [mode=%s, scode=%d:%d, camset1=%d, camset2=%d, frame=%d, kind=%d(1:nc,2:pd,3:pr) len=%d:%d]",
							p->mcls_MI.gcls_Status.GetUntenName((EM_DIV_UNTEN)que.fl.data[0]), que.fl.data[1], que.fl.data[2], camset1, camset2, nStartFrameNo, emKind, que.fl.data[3], que.fl.data[4]);

						// ���C���C���X�^���X�֒ʒm
						p->mcls_MI.SetDeliveryMail(E_DEF_ML_RESUME_START, &que);
					}
					else
					{
						pLOG(em_MSG), "[MAIL] �r���J�n�v�� �ΏۊO�I�I (FACT_HT_DEFECT_22) [mode=%s, scode=%d:%d, camset1=%d, camset2=%d, frame=%d, kind=%d(1:nc,2:pd,3:pr)]",
							p->mcls_MI.gcls_Status.GetUntenName((EM_DIV_UNTEN)que.fl.data[0]), que.fl.data[1], que.fl.data[2], camset1, camset2, nStartFrameNo, emKind);
						break;
					}
				}
			}
			break;

//-----------------------------------------------------------------------------------------------
		case FACT_HT_DEFECT_23:								// �r����~�v�� (��TO_DEFECT)
			if (true)
			{
				int umu = que.fl.data[getpcid() - 1];
				pLOG(em_MSG), "[MAIL] �r����~�v�� (FACT_HT_DEFECT_23) [%s]", 0 == umu ? "�Ώ�" : "�ΏۊO");
				if (0 != umu) break;

				// ���C���C���X�^���X�֒ʒm
				p->mcls_MI.SetDeliveryMail(E_DEF_ML_RESUME_STOP, &que);
			}
			break;

//-----------------------------------------------------------------------------------------------
		case FACT_KS_PCNAME_CHANGE:							// PC���̕ύX�ʒm (��KS_SLAVE)
			pLOG(em_MSG), "[MAIL] PC���̕ύX�ʒm (FACT_KS_PCNAME_CHANGE)" );
			// PC���̎擾
			getpcname(); 
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
void CHT_DEFECTDlg::OnTimer(UINT_PTR nIDEvent)
{
	CString cwk;
	CString ss;

	////// ���ݑI������Ă���C���X�^���X���擾����
	//int idx = m_cmbMen.GetCurSel();			// 0�I���W��
	//if( -1 == idx) return;					// �ݒ薳��
	int idx = 0;

	//// �X�e�[�^�X���
	mcnt_pListSt->SetItem(0, 1, mcls_MI.gcls_Status.GetMainStateName());
	mcnt_pListSt->SetItem(1, 1, mcls_MI.gcls_Status.GetUntenName());
	mcnt_pListSt->SetItem(2, 1, DivNameManager::GetTorB(mcls_MI.GetMen()));	// 2   �Ώۖ�

	EM_DIV_HANTEI	emKindHt = mcls_MI.GetKindHt();
	ss = DIV_HANTEI_HT == emKindHt ? "����@�\" : "�����@�\";
	ss = (DIV_HANTEI_PR == emKindHt ? "�p�������@�\" : ss);
	mcnt_pListSt->SetItem(5, 1, ss);

	mcnt_pListSt->SetItem(6, 1, mcls_MI.GetNowScode(0));
	
	ss = mcls_MI.gcls_pSend->GetJpegQuality() == 0 ? "BMP" : "JPEG���k";
	mcnt_pListSt->SetItem(7, 1, ss);

	// �J�����Z�b�g
	ss = "";
	cwk = "";
	int	nCamPair = mcls_MI.GetCamPair();
	if (0 < nCamPair)
	{
		// �p����������̏ꍇ�A�J�����y�A�ł͂Ȃ��J�����Z�b�g�Ŏw�������邽��
		// nCamPair�͎�PC���L���ł��邩�ۂ��̔��f�ɂ̂ݎg�p
		// �J�����Z�b�g�͐擪�Œ�Ŋi�[���Ă���̂ŁA���̎��_�łO���Z�b�g��������擾����
		if (DIV_HANTEI_PR == emKindHt) nCamPair = 0;

		int	nCamNum = mcls_MI.gcls_Status.GetCamPairItemNum(nCamPair, mcls_MI.GetKindHt());
		for (int ii = 0; ii < nCamNum; ii++)
		{
			int nCamSet = mcls_MI.gcls_Status.GetCamPairItemCamset(nCamPair, ii, emKindHt);
			ss.AppendFormat("%d/ ", nCamSet);
			if (DIV_HANTEI_PR != emKindHt)
			{
				// �P������̏ꍇ�A�J�����Z�b�g�O�̓o�b�N�A�b�v���Ӗ�����
				cwk.AppendFormat("%s/ ", DivNameManager::GetDivCamSet(nCamSet));
			}
			else
			{
				// �p����������̏ꍇ�A�J�����Z�b�g�O�͖����J�����̑I�����Ӗ�����
				cwk.AppendFormat("%s/ ", (0 != nCamSet ? DivNameManager::GetDivCamSet(nCamSet) : "�ΏۊO"));
			}
		}
	}
	mcnt_pListSt->SetItem(3, 1, ss);
	mcnt_pListSt->SetItem(4, 1, cwk);


	//// �������
	mcnt_pListTo->SetItem(0, 1, mcls_MI.gcls_pSock[idx]->GetSockStateName());
	ss.Format("%d (%d)", mcls_MI.gcls_pSock[idx]->gque_Deli.GetCount(), mcls_MI.gcls_pSock[idx]->gque_Deli.GetMaxCount());	mcnt_pListTo->SetItem(1, 1, ss);
	mcnt_pListTo->SetItem(2, 1, mcls_MI.gcls_pSend->GetDspRecvFrameNo());
	mcnt_pListTo->SetItem(3, 1, mcls_MI.gcls_pSend->GetDspFrameNo());

	mcnt_pListTo->SetItem(4, 1, mcls_MI.gcls_pSend->GetTimeIntval());
	ss.Format("%d (%d)", mcls_MI.gcls_pSend->GetTimeAllNow(), mcls_MI.gcls_pSend->GetTimeAllMax());	mcnt_pListTo->SetItem(5, 1, ss);
	ss.Format("%d (%d)", mcls_MI.gcls_pSend->g_nTime_All[0], mcls_MI.gcls_pSend->g_nTime_All[1]);	mcnt_pListTo->SetItem(6, 1, ss);
	ss.Format("%d (%d)", mcls_MI.gcls_pSend->g_nTime_Rpi[0], mcls_MI.gcls_pSend->g_nTime_Rpi[1]);	mcnt_pListTo->SetItem(7, 1, ss);
	ss.Format("%d (%d)", mcls_MI.gcls_pSend->g_nTime_To[0], mcls_MI.gcls_pSend->g_nTime_To[1]);	mcnt_pListTo->SetItem(8, 1, ss);
	ss.Format("%d (%d)", mcls_MI.gcls_pSend->g_nTime_ToFrame[0], mcls_MI.gcls_pSend->g_nTime_ToFrame[1]);	mcnt_pListTo->SetItem(9, 1, ss);

#ifdef FRAME_RAW_SEND
	for (int ii=0; ii<FRAME_HT_SEND_PORT; ii++) {
		mcnt_pListTo->SetItem(11+2*ii, 1, mcls_MI.gcls_pSockRpi[ii]->GetSockStateName());
		ss.Format("%d (%d)", mcls_MI.gque_pSockRpi[ii]->GetCountFree(), mcls_MI.gque_pSockRpi[ii]->GetMaxCountPool());	mcnt_pListTo->SetItem(12+ii*2, 1, ss);
	}

	double dAve = (mcls_MI.gcls_pCull[0]->g_nTime_Rpi[0] + mcls_MI.gcls_pCull[1]->g_nTime_Rpi[0]) / 2;
	double dMax = max(mcls_MI.gcls_pCull[0]->g_nTime_Rpi[1], mcls_MI.gcls_pCull[1]->g_nTime_Rpi[1]);
	ss.Format("%.0f (%.0f)", dAve, dMax);	mcnt_pListTo->SetItem(15, 1, ss);
#endif



	CDialog::OnTimer(nIDEvent);
}

// ����ȍ~ �f�o�b�N�p�R�}���h -------------------->>>
//�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P
// �����J�n
//�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q
void CHT_DEFECTDlg::OnBnClickedDebugStart()
{
	if ( BST_CHECKED != CWnd::IsDlgButtonChecked(IDC_DEBUG_MANUAL) ) return;

	int pcid = getpcid() -1;

	EM_DIV_UNTEN emMode = DIV_UNTEN_NOMAL;
	if ( BST_CHECKED == CWnd::IsDlgButtonChecked( IDC_DEBUG_START_MODE1 ) ) emMode = DIV_UNTEN_NOMAL;
	if ( BST_CHECKED == CWnd::IsDlgButtonChecked( IDC_DEBUG_START_MODE2 ) ) emMode = DIV_UNTEN_SAMP;
	if ( BST_CHECKED == CWnd::IsDlgButtonChecked( IDC_DEBUG_START_MODE3 ) ) emMode = DIV_UNTEN_CAMERA;
	if ( BST_CHECKED == CWnd::IsDlgButtonChecked( IDC_DEBUG_START_MODE4 ) ) emMode = DIV_UNTEN_SMYU;

	EM_DIV_HANTEI emKind= DIV_HANTEI_BK;
	if ( BST_CHECKED == CWnd::IsDlgButtonChecked( IDD_DEBUG_KIND1 ) ) emKind = DIV_HANTEI_HT;
	if ( BST_CHECKED == CWnd::IsDlgButtonChecked( IDD_DEBUG_KIND2 ) ) emKind = DIV_HANTEI_CY;
	if ( BST_CHECKED == CWnd::IsDlgButtonChecked( IDD_DEBUG_KIND3 ) ) emKind = DIV_HANTEI_PR;

	COMMON_QUE	que;
	que.nEventNo		= FACT_HT_DEFECT_02;
	que.nLineNo			= getlineid();
	que.fl.data[0]		= (int)emMode;
	que.fl.data[1]		= GetDlgItemInt(IDC_DEBUG_SCODE);
	que.fl.data[2]		= GetDlgItemInt(IDC_DEBUG_SCODE);
	que.fl.data[3]		= 0;
	que.fl.data[4]		= 0;

	que.fl.data[5+ 2*pcid + 0] = GetDlgItemInt(IDC_DEBUG_CAMPAIR);	// �����̂Ƃ��낾���Z�b�g
	que.fl.data[5+ 2*pcid + 1] = (int)emKind;

	send_mail(HT_DEFECT, ".", &que);
}
//�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P
// ������~
//�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q
void CHT_DEFECTDlg::OnBnClickedDebugStop()
{
	if ( BST_CHECKED != CWnd::IsDlgButtonChecked(IDC_DEBUG_MANUAL) ) return;
	COMMON_QUE que;
	memset(&que, 0x00, sizeof(que));
	que.nEventNo = FACT_HT_DEFECT_03;
	que.nLineNo = getlineid();

	send_mail(HT_DEFECT, ".", &que);
}
//�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P
// �p�����[�^�ύX
//�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q
void CHT_DEFECTDlg::OnBnClickedDebugParam()
{
	if ( BST_CHECKED != CWnd::IsDlgButtonChecked(IDC_DEBUG_MANUAL) ) return;
	COMMON_QUE que;
	que.nEventNo = FACT_HT_DEFECT_01;
	que.nLineNo = getlineid();
	que.fl.data[0] = 0;						// �x�����s����
	que.fl.data[1] = 2;						// ����
	que.fl.data[2] = CH_SCODE_ALL;			// �S��

	EM_DIV_HANTEI emKind= DIV_HANTEI_BK;
	if ( BST_CHECKED == CWnd::IsDlgButtonChecked( IDD_DEBUG_KIND1 ) ) emKind = DIV_HANTEI_HT;
	if ( BST_CHECKED == CWnd::IsDlgButtonChecked( IDD_DEBUG_KIND2 ) ) emKind = DIV_HANTEI_CY;
	if ( BST_CHECKED == CWnd::IsDlgButtonChecked( IDD_DEBUG_KIND3 ) ) emKind = DIV_HANTEI_PR;
	que.fl.data[3]	= (int)emKind;

	send_mail(HT_DEFECT, ".", &que);	
}

//�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P
// ��ʏ��ݒ�v��
//�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q
void CHT_DEFECTDlg::OnBnClickedDebugCoilinf()
{
	if ( BST_CHECKED != CWnd::IsDlgButtonChecked(IDC_DEBUG_MANUAL) ) return;
	COMMON_QUE que;
	que.nEventNo = FACT_HT_DEFECT_05;
	que.nLineNo = getlineid();

	TYPE_EPC_COIL wk;
	memset(&wk, 0x00, sizeof(wk));
	// �[���f�[�^�Z�b�g
	memcpy(wk.cCoilNo, "1234567890", PRO_SIZE_COIL_NO);
	memcpy(wk.cMeiNo, "123 ", PRO_SIZE_ORDER_NO);

	wk.nAtuSei = 1100;
	wk.nWidZai = 1000;
	wk.nLenIn = 3000*1000;

	// �Z�b�g����
	int nCnt = GET_JUST_INDEX( sizeof(TYPE_EPC_COIL) , DIVCOIL_ONE_SIZE_SEND) + 1;
	for (int ii = 0; ii < nCnt; ii++) {
		int nSize = sizeof(TYPE_EPC_COIL) - (DIVCOIL_ONE_SIZE_SEND*ii);
		if (nSize>DIVCOIL_ONE_SIZE_SEND) nSize = DIVCOIL_ONE_SIZE_SEND;
		
		que.fl.data[0] = 0;				// 0�Œ�B
		que.fl.data[1] = nSize;
		que.fl.data[2] = ii+1;
		que.fl.data[3] = nCnt;
		for(int jj=0; jj<NUM_MEN; jj++) que.fl.data[4+jj] = MAX_SCODE;
		memcpy(&que.fl.data[4+NUM_MEN+1], &((BYTE*)&wk)[ii*DIVCOIL_ONE_SIZE_SEND], nSize);
		send_mail(HT_DEFECT, ".", &que);
	}
}

//�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P
// �x�����s�L�����Z��
//�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q
//void CHT_DEFECTDlg::OnBnClickedDebugDlycancel()
//{
//	if ( BST_CHECKED != CWnd::IsDlgButtonChecked(IDC_DEBUG_MANUAL) ) return;
//	COMMON_QUE que;
//	que.nEventNo = FACT_HT_DEFECT_06;
//	que.nLineNo = getlineid();
//	send_mail(HT_DEFECT, ".", &que);
//}
//�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P
// ���s�s�\����
//�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q
void CHT_DEFECTDlg::OnBnClickedDebugCanok()
{
	if ( BST_CHECKED != CWnd::IsDlgButtonChecked(IDC_DEBUG_MANUAL) ) return;
	COMMON_QUE que;
	que.nEventNo = FACT_HT_DEFECT_07;
	que.nLineNo = getlineid();
	send_mail(HT_DEFECT, ".", &que);
}
//�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P
// �t���[���摜�擾�v��
//�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q
//void CHT_DEFECTDlg::OnBnClickedDebugFrame()
//{
//	if ( BST_CHECKED != CWnd::IsDlgButtonChecked(IDC_DEBUG_MANUAL) ) return;
//	COMMON_QUE que;
//	que.nEventNo	= FACT_HT_DEFECT_09;
//	que.nLineNo		= getlineid();
//	que.fl.data[0]	= 0;
//	que.fl.data[1]	= GetDlgItemInt(IDC_DEBUG_CAMPAIR);
//	que.fl.data[2]	= 2;
//	send_mail(HT_DEFECT, ".", &que);
//}

//�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P
// ���S���]���f�[�^���W�v��
//�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q
//void CHT_DEFECTDlg::OnBnClickedDebugProfile()
//{
//	if ( BST_CHECKED != CWnd::IsDlgButtonChecked(IDC_DEBUG_MANUAL) ) return;
//	COMMON_QUE que;
//	que.nEventNo	= FACT_HT_DEFECT_11;
//	que.nLineNo		= getlineid();
//	int pcid = getpcid() -1;
//	que.fl.data[pcid]	= 0;
//	send_mail(HT_DEFECT, ".", &que);
//}


//�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P
// �摜���k
//�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q
void CHT_DEFECTDlg::OnBnClickedDebugImgkind()
{
	if ( BST_CHECKED != CWnd::IsDlgButtonChecked(IDC_DEBUG_MANUAL) ) return;
	COMMON_QUE que;
	que.nEventNo	= FACT_HT_DEFECT_04;
	que.nLineNo		= getlineid();
	que.fl.data[0]	= GetDlgItemInt(IDC_DEBUG_JPEGQUALITY);
	send_mail(HT_DEFECT, ".", &que);
}


//�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P
// ���������J�E���^�ݒ�
//�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q
void CHT_DEFECTDlg::OnBnClickedDebugSetPosition()
{
	if ( BST_CHECKED != CWnd::IsDlgButtonChecked(IDC_DEBUG_MANUAL) ) return;
	COMMON_QUE que;
	que.nEventNo	= FACT_HT_DEFECT_20;
	que.nLineNo		= getlineid();
	que.fl.data[0]	= 2;
	que.fl.data[1]	= -1;

	NCL_CMD_SET_POSITION& wk = (NCL_CMD_SET_POSITION&)que.fl.data[2];
	memset(&wk, 0x00, sizeof(NCL_CMD_SET_POSITION));

	wk.cmd = NCL_SET_POSITION;
	wk.framenum = 0;
	wk.update_mask = 0xFFFFFFFF;
	wk.front_pos = 1000;

	send_mail(HT_DEFECT, ".", &que);
}

//�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P
// �p�����[�^���O�Ǎ�
//�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q
void CHT_DEFECTDlg::OnBnClickedDebugParamPreload()
{
	if ( BST_CHECKED != CWnd::IsDlgButtonChecked(IDC_DEBUG_MANUAL) ) return;
	COMMON_QUE que;
	que.nEventNo	= FACT_HT_DEFECT_21;
	que.nLineNo		= getlineid();
	que.fl.data[0]	= 2;
	que.fl.data[1]	= -1;
	que.fl.data[2]	= 0;
	send_mail(HT_DEFECT, ".", &que);
}

//�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P
// ����敪�̕ύX�i�P���j
//�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q
void CHT_DEFECTDlg::OnBnClickedDebugKind1()
{
	CWnd::GetDlgItem(IDC_DEBUG_CAMPAIR)->EnableWindow(TRUE);
	CWnd::GetDlgItem(IDC_DEBUG_CAMSET1)->EnableWindow(FALSE);
	CWnd::GetDlgItem(IDC_DEBUG_CAMSET2)->EnableWindow(FALSE);
}

//�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P
// ����敪�̕ύX�i�����j
//�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q
void CHT_DEFECTDlg::OnBnClickedDebugKind2()
{
	CWnd::GetDlgItem(IDC_DEBUG_CAMPAIR)->EnableWindow(TRUE);
	CWnd::GetDlgItem(IDC_DEBUG_CAMSET1)->EnableWindow(FALSE);
	CWnd::GetDlgItem(IDC_DEBUG_CAMSET2)->EnableWindow(FALSE);
}

//�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P
// ����敪�̕ύX�i�p�������j
//�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q
void CHT_DEFECTDlg::OnBnClickedDebugKind3()
{
	CWnd::GetDlgItem(IDC_DEBUG_CAMPAIR)->EnableWindow(FALSE);
	CWnd::GetDlgItem(IDC_DEBUG_CAMSET1)->EnableWindow(TRUE);
	CWnd::GetDlgItem(IDC_DEBUG_CAMSET2)->EnableWindow(TRUE);
}

//�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P
// �r���J�n
//�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q
void CHT_DEFECTDlg::OnBnClickedDebugResumeStart()
{
	if (BST_CHECKED != CWnd::IsDlgButtonChecked(IDC_DEBUG_MANUAL)) return;

	int pcid = getpcid() - 1;

	EM_DIV_UNTEN emMode = DIV_UNTEN_NOMAL;
	if (BST_CHECKED == CWnd::IsDlgButtonChecked(IDC_DEBUG_START_MODE1)) emMode = DIV_UNTEN_NOMAL;
	if (BST_CHECKED == CWnd::IsDlgButtonChecked(IDC_DEBUG_START_MODE2)) emMode = DIV_UNTEN_SAMP;
	if (BST_CHECKED == CWnd::IsDlgButtonChecked(IDC_DEBUG_START_MODE3)) emMode = DIV_UNTEN_CAMERA;
	if (BST_CHECKED == CWnd::IsDlgButtonChecked(IDC_DEBUG_START_MODE4)) emMode = DIV_UNTEN_SMYU;

	EM_DIV_HANTEI emKind = DIV_HANTEI_BK;
	if (BST_CHECKED == CWnd::IsDlgButtonChecked(IDD_DEBUG_KIND1)) emKind = DIV_HANTEI_HT;
	if (BST_CHECKED == CWnd::IsDlgButtonChecked(IDD_DEBUG_KIND2)) emKind = DIV_HANTEI_CY;
	if (BST_CHECKED == CWnd::IsDlgButtonChecked(IDD_DEBUG_KIND3)) emKind = DIV_HANTEI_PR;

	COMMON_QUE	que;
	que.nEventNo = FACT_HT_DEFECT_22;
	que.nLineNo = getlineid();
	que.fl.data[0] = (int)emMode;
	que.fl.data[1] = GetDlgItemInt(IDC_DEBUG_SCODE);
	que.fl.data[2] = GetDlgItemInt(IDC_DEBUG_SCODE);
	que.fl.data[3] = 0;
	que.fl.data[4] = 0;
	que.fl.data[5] = NUM_JOIN_FRAME;
	
	if (DIV_HANTEI_PR != emKind)
	{
		// �P���E����
		que.fl.data[6 + 3 * pcid + 0] = GetDlgItemInt(IDC_DEBUG_CAMPAIR);
		que.fl.data[6 + 3 * pcid + 1] = 0;
	}
	else
	{
		// ���W�b�N�p������
		que.fl.data[6 + 3 * pcid + 0] = GetDlgItemInt(IDC_DEBUG_CAMSET1);
		que.fl.data[6 + 3 * pcid + 1] = GetDlgItemInt(IDC_DEBUG_CAMSET2);
	}
	que.fl.data[6 + 3 * pcid + 2] = (int)emKind;

	send_mail(HT_DEFECT, ".", &que);
}

//�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P
// �r����~
//�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q
void CHT_DEFECTDlg::OnBnClickedDebugResumeStop()
{
	if (BST_CHECKED != CWnd::IsDlgButtonChecked(IDC_DEBUG_MANUAL)) return;
	COMMON_QUE que;
	memset(&que, 0x00, sizeof(que));
	que.nEventNo = FACT_HT_DEFECT_23;
	que.nLineNo = getlineid();

	send_mail(HT_DEFECT, ".", &que);
}
