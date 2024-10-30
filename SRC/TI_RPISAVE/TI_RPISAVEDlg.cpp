// TI_RPISAVEDlg.cpp : �����t�@�C��
//

#include "stdafx.h"
#include "TI_RPISAVE.h"
#include "TI_RPISAVEDlg.h"

// �������[���[�N���o �蓮�ǉ� ----->>>
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif
// <<<------------------------

// CTI_RPISAVEDlg �_�C�A���O




CTI_RPISAVEDlg::CTI_RPISAVEDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CTI_RPISAVEDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CTI_RPISAVEDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, m_ListBox);
	DDX_Control(pDX, IDC_LIST_All, m_ViewAll);
	DDX_Control(pDX, IDC_LIST_EXEC, m_ViewExec);
	DDX_Control(pDX, IDC_LIST_QUE, m_ViewQue);
	DDX_Control(pDX, IDC_LIST_RPI, m_ViewRpi);
	DDX_Control(pDX, IDC_LIST_STS, m_ViewSts);
}

BEGIN_MESSAGE_MAP(CTI_RPISAVEDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
//	ON_LBN_SELCHANGE(IDC_LIST1, &CTI_RPISAVEDlg::OnLbnSelchangeList1)
	ON_BN_CLICKED(IDOK, &CTI_RPISAVEDlg::OnBnClickedOk)
	ON_WM_CLOSE()
	ON_WM_TIMER()
	ON_LBN_DBLCLK(IDC_LIST1, &CTI_RPISAVEDlg::OnLbnDblclkList1)
	ON_BN_CLICKED(IDC_DEBUG_START, &CTI_RPISAVEDlg::OnBnClickedDebugStart)
	ON_BN_CLICKED(IDC_DEBUG_STOP, &CTI_RPISAVEDlg::OnBnClickedDebugStop)
	ON_BN_CLICKED(IDC_DSPWINDOW1, &CTI_RPISAVEDlg::OnBnClickedDspwindow1)
	ON_BN_CLICKED(IDC_DEBUG_RESYNC, &CTI_RPISAVEDlg::OnBnClickedDebugResync)
END_MESSAGE_MAP()


// CTI_RPISAVEDlg ���b�Z�[�W �n���h��

BOOL CTI_RPISAVEDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// ���̃_�C�A���O�̃A�C�R����ݒ肵�܂��B�A�v���P�[�V�����̃��C�� �E�B���h�E���_�C�A���O�łȂ��ꍇ�A
	//  Framework �́A���̐ݒ�������I�ɍs���܂��B
	SetIcon(m_hIcon, TRUE);			// �傫���A�C�R���̐ݒ�
	SetIcon(m_hIcon, FALSE);		// �������A�C�R���̐ݒ�

	// TODO: �������������ɒǉ����܂��B
// ��ʏ����l�Z�b�g ============================>>>>
	CWnd::CheckDlgButton(IDC_DEBUG_MANUAL, 1);


	// TODO: �������������ɒǉ����܂��B
// �蓮�ǉ� -------------------->>>
	// ��ʈʒu (�T�C�Y�͂��܂̂܂�)
	::SetWindowPos(m_hWnd, NULL, 335, 430, 0, 0, SWP_NOSIZE | SWP_NOZORDER);

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

void CTI_RPISAVEDlg::OnPaint()
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
HCURSOR CTI_RPISAVEDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

// ����ȍ~ �蓮�ǉ� -------------------->>>

//�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P
// �~�{�^��
//�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q
void CTI_RPISAVEDlg::OnClose()
{
	//CDialog::OnClose();
}

//�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P
// �����I���{�^��
//�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q
void CTI_RPISAVEDlg::OnBnClickedOk()
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
void CTI_RPISAVEDlg::OnLbnDblclkList1()
{
	// ListBox�R���g���[���̃v���p�e�B�́A�\�[�g�֎~�ɂ��Ă�����
	// ListBox�R���g���[���̃_�u���N���b�N�C�x���g��ǉ�
	mcls_pLog->ShowEdit();
}

#include <locale.h>
#include "..\..\Library\KizuLibMFC\Class\LogFileManager.h"				// ���O�Ǘ��N���X



//------------------------------------------
// �e�v���Z�X����������
//------------------------------------------
void CTI_RPISAVEDlg::InitProcess()
{
	CString sWk;

	//// ��ԏ��߂ɂ��낢�돉����
	mcls_pLog = NULL;

	//// �^�X�N����������
	int nRetc = task_init(TI_RPISAVE);
	if( 0 != nRetc ) {
		if(-1 == nRetc) { syslog(SYSNO_TASK_INIT_ERR, "[%s] ���Ƀ^�X�N�N���ς݂ɂ��A�N�����f�B", TI_RPISAVE); }
		else			{ syslog(SYSNO_TASK_INIT_ERR, "[%s]", TI_RPISAVE); }
		_ASSERT(0);
		exit(-9);		// ���v���Z�X�����I��
		return;
	}

	//// LogFileManager�C���X�^���X���� (����ȍ~ ���O�o�͉\)
	setlocale(LC_ALL, "Japanese");	// ���ꂪ�����Ɠ��{��p�X���܂ރt�H���_�̏ꍇ�A�ۑ��ł��Ȃ��B(���C�u�����̒��Ŏ��s����ׂ��������Ă��邪�A�ύX����ƃ^�X�N�S�̂ɉe������̂ŁA�Ƃ肠���������Őݒ�)
	mcls_pLog = new LogFileManager(getmytaskname(), &m_ListBox, 100, 5);
	for( int ii = 0; ii < MAX_INS; ii++){
		CString logtitle;logtitle.Format("sockins%d", ii);

		mcls_MI.gcls_InsLog[ii] = new LogFileManager(logtitle, NULL, 100, 5);
	}
	mcls_MI.gcls_InsLog_ImgBuf_Union = new LogFileManager("ImgUnion", NULL, 100, 5);

	LOG(em_MSG), "[Dlg] �^�X�N�N��");
	LOG(em_MSG), "[Dlg] �e�v���Z�X�J�n���� ���s���E�E�E");

	//// ���C���C���X�^���X �J�n����
	mcls_MI.SetLogMgr(mcls_pLog);
	mcls_MI.Init();
	

	// =========================================================
	// ��ʒ�`
	ParamManager::P_MAIN		const& prm	= mcls_MI.gcls_pParam->GetMain();
	int nMaxCam = 0;			// �ő�J������
	int nRpiNum = 0;			// RPI������
	for(int ii=0; ii<prm.nIns; ii++) {
		ParamManager::P_MAIN::INS   const& prmI	= mcls_MI.gcls_pParam->GetMain().ins[ii];
		nMaxCam = __max(prmI.nCamsetNum, nMaxCam);
		nRpiNum += prm.nImgNumTotal;
	}

	//-----------------------
	// �L���[���
	mcnt_pQue = new DspListView(&m_ViewQue);
	m_ViewQue.SetExtendedStyle( m_ViewQue.GetExtendedStyle() | LVS_EX_GRIDLINES );			// �r���`��
	m_ViewQue.SetExtendedStyle( m_ViewQue.GetExtendedStyle() | LVS_EX_FULLROWSELECT );		// 1�s�S�̑I��
	
	// ��
	mcnt_pQue->SetColumnAdd(100, "QUE���");
	mcnt_pQue->SetColumnAdd(50, "�ő�Que");
	for(int ii=0; ii<prm.nIns; ii++) { sWk.Format("Ins%d", ii); mcnt_pQue->SetColumnAdd(35, sWk); }
	// �s
	mcnt_pQue->SetRowAll(11);	
	// �Œ�l
	mcnt_pQue->SetItem( 0, 0, "�e��M�����");
	mcnt_pQue->SetItem( 1, 0, "�A���㷭���");
	mcnt_pQue->SetItem( 2, 0, "�������҂���");

	mcnt_pQue->SetItem( 4, 0, "��ד����҂�");
	mcnt_pQue->SetItem( 5, 0, "�A���҂�");
	mcnt_pQue->SetItem( 6, 0, "���k�҂�");
	mcnt_pQue->SetItem( 7, 0, "�����ݑ҂�");
	mcnt_pQue->SetItem( 8, 0, "�\���҂�");



	//-----------------------
	// ��������
	mcnt_pExe = new DspListView(&m_ViewExec);
	m_ViewExec.SetExtendedStyle( m_ViewExec.GetExtendedStyle() | LVS_EX_GRIDLINES );			// �r���`��
	m_ViewExec.SetExtendedStyle( m_ViewExec.GetExtendedStyle() | LVS_EX_FULLROWSELECT );		// 1�s�S�̑I��
	
	// ��
	mcnt_pExe->SetColumnAdd(70, "��������");
	mcnt_pExe->SetColumnAdd(40, "��");
	mcnt_pExe->SetColumnAdd(40, "�ő�");
	// �s
	mcnt_pExe->SetRowAll(6);	
	// �Œ�l
	mcnt_pExe->SetItem( 0, 0, "��בS�擾");
	mcnt_pExe->SetItem( 1, 0, "PLG�R�t");
	mcnt_pExe->SetItem( 2, 0, "�A��");
	mcnt_pExe->SetItem( 3, 0, "�摜Buf");
	mcnt_pExe->SetItem( 4, 0, "�摜���k");
	mcnt_pExe->SetItem( 5, 0, "���k��Th");


	//-----------------------
	//// Ins���
	mcnt_pAll = new DspListView(&m_ViewAll);
	m_ViewAll.SetExtendedStyle( m_ViewAll.GetExtendedStyle() | LVS_EX_GRIDLINES );			// �r���`��
	m_ViewAll.SetExtendedStyle( m_ViewAll.GetExtendedStyle() | LVS_EX_FULLROWSELECT );		// 1�s�S�̑I��
	
	// ��
	mcnt_pAll->SetColumnAdd(30, "Ins");
	mcnt_pAll->SetColumnAdd(25, "��");
	mcnt_pAll->SetColumnAdd(40, "����");
	mcnt_pAll->SetColumnAdd(25, "S");
	mcnt_pAll->SetColumnAdd(25, "cam");
	mcnt_pAll->SetColumnAdd(25, "E");

	mcnt_pAll->SetColumnAdd(42, "FNo"); 
	for(int ii=1; ii<nMaxCam; ii++) { sWk.Format("%d", ii+1); mcnt_pAll->SetColumnAdd(42, sWk); }
	// �s
	mcnt_pAll->SetRowAll(prm.nIns);	
	for(int ii=0; ii<mcnt_pAll->GetRowCount(); ii++) {
		sWk.Format("Ins%d", ii); 
		mcnt_pAll->SetItem( ii, 0, sWk);
	}


	//-----------------------
	//// Sts���
	mcnt_pSts = new DspListView(&m_ViewSts);
	m_ViewSts.SetExtendedStyle(m_ViewSts.GetExtendedStyle() | LVS_EX_GRIDLINES);			// �r���`��
	m_ViewSts.SetExtendedStyle(m_ViewSts.GetExtendedStyle() | LVS_EX_FULLROWSELECT);		// 1�s�S�̑I��

																							// ��
	mcnt_pSts->SetColumnAdd(30, "Ins");
	mcnt_pSts->SetColumnAdd(60, "���");

	for (int ii = 0; ii<nMaxCam; ii++) { sWk.Format("%d", ii + 1); mcnt_pSts->SetColumnAdd(25, sWk); }
	mcnt_pSts->SetColumnAdd(120, "���R�C�� �Ǘ�No");
	// �s
	mcnt_pSts->SetRowAll(prm.nIns);
	for (int ii = 0; ii<mcnt_pSts->GetRowCount(); ii++) {
		sWk.Format("Ins%d", ii);
		mcnt_pSts->SetItem(ii, 0, sWk);
	}


	//-----------------------
	// Rpi���
	mcnt_pRpi = new DspListView(&m_ViewRpi);
	m_ViewRpi.SetExtendedStyle( m_ViewRpi.GetExtendedStyle() | LVS_EX_GRIDLINES );			// �r���`��
	m_ViewRpi.SetExtendedStyle( m_ViewRpi.GetExtendedStyle() | LVS_EX_FULLROWSELECT );		// 1�s�S�̑I��
	
	// ��
	mcnt_pRpi->SetColumnAdd(30, "Ins");
	mcnt_pRpi->SetColumnAdd(25, "��");
	mcnt_pRpi->SetColumnAdd(25, "�F");
	mcnt_pRpi->SetColumnAdd(120, "�Ǘ�No");
	mcnt_pRpi->SetColumnAdd(50, "CNo");
	mcnt_pRpi->SetColumnAdd(50, "������");
	mcnt_pRpi->SetColumnAdd(60, "����MB/s");
	mcnt_pRpi->SetColumnAdd(50, "����ms");
	mcnt_pRpi->SetColumnAdd(50, "�ő�ms");
	mcnt_pRpi->SetColumnAdd(25, "��");
	mcnt_pRpi->SetColumnAdd(400, "�p�X");
	// �s
	mcnt_pRpi->SetRowAll(nRpiNum);



	//// ��ʍX�V�p�^�C�}����
	SetTimer(1, 500, NULL);

	//// ���C���X���b�h�̎��s���s��
	LOG(em_MSG), "[Dlg] �e�v���Z�X�J�n���� ����");
	mcls_MI.Start();	

	mcls_MI.gcls_pDispProc->SetEv_ShowWin();
	return;
}

//�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P
// �e�v���Z�X�J������
//�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q
void CTI_RPISAVEDlg::ExitProcess()
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
	for( int ii = 0; ii < MAX_INS; ii++){
		delete mcls_MI.gcls_InsLog[ii];
		mcls_MI.gcls_InsLog[ii] = 0;
	}	
	delete mcls_MI.gcls_InsLog_ImgBuf_Union; mcls_MI.gcls_InsLog_ImgBuf_Union =0;
	//// ��ʃR���g���[���J��
	delete mcnt_pAll;
	delete mcnt_pRpi;
	delete mcnt_pQue;
	delete mcnt_pExe;
	delete mcnt_pSts;

	// �^�X�N�I������
	task_exit();

	// ��ʂ̏I���v��
	PostMessage(WM_QUIT);
}

//------------------------------------------
// ���C���X���b�h DlgThread
// LPVOID param �_�C�����O�|�C���^
//------------------------------------------
unsigned WINAPI CTI_RPISAVEDlg::MainThread(void* param)
{
	CTI_RPISAVEDlg* p = (CTI_RPISAVEDlg *)param;
	COMMON_QUE que;			// ��M�\����

	//// �e�v���Z�X���������� (����ȑO�̓��O�o�͋֎~)
	p->InitProcess();

	//// �N���v����荞�� ���[�v�J�n
	while(true) {
		recv_mail(&que);			// �N���v����荞��

		switch(que.nEventNo) {

//-----------------------------------------------------------------------------------------------
		case FACT_TI_RPISAVE_01:								// �����J�n�v�� (��TO_DEFECT)
			pLOG(em_MSG), "[MAIL] �����J�n�v�� (FACT_HT_DEFECT_01) fno=%d", que.fl.data[0]); 

			// ���C���C���X�^���X�֒ʒm
			p->mcls_MI.SetDeliveryMail( E_DEF_ML_START, &que);
			break;

//-----------------------------------------------------------------------------------------------
		case FACT_TI_RPISAVE_02:								// ������~�v�� (��TO_DEFECT)
			pLOG(em_MSG), "[MAIL] ������~�v�� (FACT_HT_DEFECT_02)"); 
			if( StatusManager::STATE_RUN != p->mcls_MI.gcls_Status.GetMainSt() ) {
				pLOG(em_MSG), "[MAIL] ������~�v�� �����B");	// �J��Ԃ��h�~�΍�
				break;
			}

			// ���C���C���X�^���X�֒ʒm
			p->mcls_MI.SetDeliveryMail( E_DEF_ML_STOP, &que);
			break;


//-----------------------------------------------------------------------------------------------
		case FACT_TI_RPISAVE_04:								// ���R�C����� (��TO_DEFECT)
			pLOG(em_MSG), "[MAIL] ���R�C����� (FACT_TO_DEFECT_04) [men=%d][%s][%s][fno=%d]", que.mix.idata[0], que.mix.cdata[0], que.mix.cdata[1], que.fl.data[5] ); 

			// ���C���C���X�^���X�֒ʒm
			p->mcls_MI.SetDeliveryMail( E_DEF_ML_NEXTCOIL, &que);
			break;

//-----------------------------------------------------------------------------------------------
		case FACT_KS_PCNAME_CHANGE:								// PC���̕ύX�ʒm (��KS_SLAVE)
			pLOG(em_MSG), "[MAIL] �p�����[�^�ύX�v�� (FACT_TO_DEFECT_01) [�e�[�u��=%d]", que.fl.data[0]); 
			// ���C���C���X�^���X�֒ʒm
			p->mcls_MI.SetDeliveryMail( E_DEF_ML_PARAM, &que);
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
void CTI_RPISAVEDlg::OnTimer(UINT_PTR nIDEvent)
{
	ParamManager::P_MAIN		const& prm	= mcls_MI.gcls_pParam->GetMain();
	int nInsNum = mcls_MI.gcls_pParam->GetMain().nIns;

	CString ss, sWk, sWk2;
	int ii, jj;
	int nWk;
	int nWkMax;



	// =========================================================
	//// �L���[���

	// �e�摜��M�����
	for(ii=0; ii<nInsNum; ii++) {
		mcnt_pQue->SetItem(0, 2+ii, mcls_MI.gque_pSockRecv[ii]->GetCountFree() );
	}

	// �A���㷭���
	mcnt_pQue->SetItem(1, 2, mcls_MI.gque_pFrame->GetCountFree() );			

	// �������҂���
	nWkMax = 0;
	for(ii=0; ii<nInsNum; ii++) {
		mcnt_pQue->SetItem(2, 2+ii, mcls_MI.gcls_pSyncProc[ii]->GetMaxBufFrameNum() );
		nWkMax = __max(nWkMax, mcls_MI.gcls_pSyncProc[ii]->GetMaxBufFrameNum());
	}
	mcnt_pQue->SetItem(2, 1, nWkMax);

	// PLG�R�t���҂�
	nWkMax = 0;
	for(ii=0; ii<nInsNum; ii++) {
		mcnt_pQue->SetItem(4, 2+ii, mcls_MI.gque_pSockRecv[ii]->GetCountPool() );
		nWkMax = __max(nWkMax, mcls_MI.gque_pSockRecv[ii]->GetMaxCountPool());
	}
	mcnt_pQue->SetItem(4, 1, nWkMax);

	// �A���҂�
	mcnt_pQue->SetItem(5, 1, mcls_MI.gque_pUnion->GetSyncQueMaxCount()); 
	mcnt_pQue->SetItem(5, 2, mcls_MI.gque_pUnion->GetSyncQueCount() );		// �A���҂�


	// ���k�҂�
	mcnt_pQue->SetItem(6, 1, mcls_MI.gque_pComp->GetSyncQueMaxCount()); 
	mcnt_pQue->SetItem(6, 2, mcls_MI.gque_pComp->GetSyncQueCount() );		// ���k�҂�

	// �����ݑ҂�
	nWkMax = 0;
	for(ii=0; ii<nInsNum; ii++) {
		nWk = 0;
		for(int jj=0; jj<MAX_COLOR_NUM; jj++) {
			if( NULL == mcls_MI.gque_pRpiWrite[ii][jj] ) continue;
			nWk = __max(nWk, mcls_MI.gque_pRpiWrite[ii][jj]->GetCount());
		} 
		mcnt_pQue->SetItem(7, 2+ii, nWk );		
		nWkMax = __max(nWkMax, nWk);
	}
	mcnt_pQue->SetItem(7, 1, nWkMax);

	// �\���҂�
	mcnt_pQue->SetItem(8, 1, mcls_MI.gque_Disp.GetMaxCount()); 
	mcnt_pQue->SetItem(8, 2, mcls_MI.gque_Disp.GetCount() );		// �\���҂�



	// =========================================================
	//// ��������
	for(int ii=0; ii<2; ii++) {
		mcnt_pExe->SetItem(0, 1+ii, (int)mcls_MI.gcls_pDispProc->g_dTime_Iport[ii] );
		mcnt_pExe->SetItem(1, 1+ii, (int)mcls_MI.gcls_pDispProc->g_dTime_PlgSync[ii] );
		mcnt_pExe->SetItem(2, 1+ii, (int)mcls_MI.gcls_pDispProc->g_dTime_ImgUnion[ii] );
		mcnt_pExe->SetItem(3, 1+ii, (int)mcls_MI.gcls_pDispProc->g_dTime_ImgBuf[ii] );
		mcnt_pExe->SetItem(4, 1+ii, (int)mcls_MI.gcls_pDispProc->g_dTime_ImgComp[ii] );
	}


	// ���k���s��
	nWk=0;
	for(int ii=0; ii<MAX_THREAD_MULTI_P; ii++) {
		if( NULL == mcls_MI.gcls_pCompProc[ii] ) continue;
		if( mcls_MI.gcls_pCompProc[ii]->IsComp() ) nWk++;
		mcnt_pExe->SetItem(5, 1, nWk );		
	}


	// =========================================================
	// Ins���
	for(ii=0; ii<prm.nIns; ii++) {
		if(NULL == mcls_MI.gcls_pSyncProc[ii]) return;
		ParamManager::P_MAIN::INS   const& prmI	= mcls_MI.gcls_pParam->GetMain().ins[ii];

		// �p�����[�^
		mcnt_pAll->SetItem(ii, 1, prmI.nMen);
		ss = (mcls_MI.gcls_Status.GetFrameSyncSt(ii) ? "��" : "�~");	mcnt_pAll->SetItem(ii, 2, ss );
		mcnt_pAll->SetItem(ii, 3, prmI.nStartCamset);
		mcnt_pAll->SetItem(ii, 4, "�`");
		mcnt_pAll->SetItem(ii, 5, prmI.nStartCamset + prmI.nCamsetNum -1);

		// iPortNo
		for(jj=0; jj<prm.ins[ii].nCamsetNum; jj++) {
			sWk.Format("%05d", mcls_MI.gcls_pSyncProc[ii]->GetNowExecFrameNo(jj) );
			mcnt_pAll->SetItem(ii, jj+6, sWk);
		}
	}

	// =========================================================
	// Sts���
	mcnt_pSts->SetItem(0, 1, mcls_MI.gcls_Status.GetMainStName());		// �������
	for (ii = 0; ii<prm.nIns; ii++) {
		// Camset LAN�ڑ�
		for (jj = 0; jj<prm.ins[ii].nCamsetNum; jj++) {
			sWk.Format("%s", (mcls_MI.gcls_pSock[ii][jj]->IsConnect(0) ? "��" : "�~"));
			mcnt_pSts->SetItem(ii, jj + 2, sWk);
		}
		mcnt_pSts->SetItem(0, 2 + prm.ins[ii].nCamsetNum, mcls_MI.gcls_pCoilProc[ii]->GetNextCoil());		// ���R�C���Ǘ�No
	}

	// =========================================================
	// RPI���
	nWk = 0;
	for(int ii=0; ii<prm.nIns; ii++) {
		for(int jj=0; jj<prm.nImgNumTotal; jj++) {
			if( NULL == mcls_MI.gcls_pRpiWProc[ii][jj] ) continue;
			ParamManager::P_MAIN::INS   const& prmI	= mcls_MI.gcls_pParam->GetMain().ins[ii];

			sWk.Format("Ins%d", ii ); mcnt_pRpi->SetItem(nWk, 0, sWk);
			mcnt_pRpi->SetItem(nWk, 1, prmI.nMen);
			mcnt_pRpi->SetItem(nWk, 2, jj);
			mcnt_pRpi->SetItem(nWk, 3, mcls_MI.gcls_pRpiWProc[ii][jj]->GetRpiHeader()->theader.cKizukenNo );
			mcnt_pRpi->SetItem(nWk, 4, mcls_MI.gcls_pRpiWProc[ii][jj]->GetRpiHeader()->theader.nFrameNumStart );
			mcnt_pRpi->SetItem(nWk, 5, mcls_MI.gcls_pRpiWProc[ii][jj]->GetRpiHeader()->nframe );

			sWk.Format("%.01f", mcls_MI.gcls_pRpiWProc[ii][jj]->GetWriteSpeedAve() );	mcnt_pRpi->SetItem(nWk, 6, sWk );
			mcnt_pRpi->SetItem(nWk, 7, (int)mcls_MI.gcls_pDispProc->g_dTime_RpiWrite[ii][0] );
			mcnt_pRpi->SetItem(nWk, 8, (int)mcls_MI.gcls_pDispProc->g_dTime_RpiWrite[ii][1] );
			sWk.Format("%s", mcls_MI.gcls_pRpiWProc[ii][jj]->IsOpen() ? "��":"�~"); mcnt_pRpi->SetItem(nWk, 9, sWk );
			mcnt_pRpi->SetItem(nWk, 10, mcls_MI.gcls_pRpiWProc[ii][jj]->GetRpiFullPath() );
			nWk++;
		}
	}





	CDialog::OnTimer(nIDEvent);
}


// ����ȍ~ �f�o�b�N�p�R�}���h -------------------->>>





//�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P
// �����J�n
//�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q
void CTI_RPISAVEDlg::OnBnClickedDebugStart()
{
	if ( BST_CHECKED != CWnd::IsDlgButtonChecked(IDC_DEBUG_MANUAL) ) return;

	// �����_�̃t���[��No����ē�������t���[��No�����肷��B
	DWORD nFno = mcls_MI.gcls_pSyncProc[0]->GetRecvFrameNo(0);
	if(0 != nFno) {
		nFno += MAX_RESYNC_FNO;
	}

	COMMON_QUE que;
	memset(&que, 0x00, sizeof(que));
	que.nEventNo   = FACT_TI_RPISAVE_01;
	que.nLineNo	   = getlineid();

//	que.fl.data[0] = nFno; 


	LOG(em_WAR), "[DEBUG] �����J�n�{�^�� Fno=%d", nFno ); 
	send_mail(getmytaskname(), ".", &que);
}

//�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P
// ������~
//�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q
void CTI_RPISAVEDlg::OnBnClickedDebugStop()
{
	if ( BST_CHECKED != CWnd::IsDlgButtonChecked(IDC_DEBUG_MANUAL) ) return;
	LOG(em_WAR), "[DEBUG] ������~�{�^��" ); 

	COMMON_QUE que;
	memset(&que, 0x00, sizeof(que));
	que.nEventNo   = FACT_TI_RPISAVE_02;
	que.nLineNo    = getlineid();

	send_mail(getmytaskname(), ".", &que);
}

//�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P
// �摜�\���f�o�b�N��� �\��/��\��
//�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q
void CTI_RPISAVEDlg::OnBnClickedDspwindow1()
{
	if(BST_CHECKED == CWnd::IsDlgButtonChecked(IDC_DSPWINDOW1)){
		mcls_MI.gcls_pDispProc->WindowIsOnOff(true);
	} else {
		mcls_MI.gcls_pDispProc->WindowIsOnOff(false);
	}
}
//�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P
// �����J�n
//�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q
void CTI_RPISAVEDlg::OnBnClickedDebugResync()
{
	if ( BST_CHECKED != CWnd::IsDlgButtonChecked(IDC_DEBUG_MANUAL) ) return;

	// �����_�̃t���[��No����ē�������t���[��No�����肷��B
	DWORD nFno = mcls_MI.gcls_pSyncProc[0]->GetRecvFrameNo(0);
	if(0 != nFno) {
		nFno += MAX_RESYNC_FNO;
	}

	COMMON_QUE que;
	memset(&que, 0x00, sizeof(que));
	que.nEventNo   = FACT_TI_RPISAVE_05;
	que.nLineNo	   = getlineid();

	que.fl.data[0] = -1; 
	que.fl.data[1] = nFno; 

	LOG(em_WAR), "[DEBUG] �ē����{�^�� Fno=%d", nFno ); 
	send_mail(getmytaskname(), ".", &que);
}
