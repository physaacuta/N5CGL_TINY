// XX_CPPSMPEXDlg.cpp : �����t�@�C��
//

#include "stdafx.h"
#include "XX_CPPSMPEX.h"
#include "XX_CPPSMPEXDlg.h"


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


// �e�X�g�p
#define		XX_CPPSMPEX		"XX_CPPSMPEX"

#define		DEBUG_LOG_BCPTIME		// BCP�������Ԃ̃��O�o��

// CXX_CPPSMPEXDlg �_�C�A���O


CXX_CPPSMPEXDlg::CXX_CPPSMPEXDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CXX_CPPSMPEXDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CXX_CPPSMPEXDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, m_ListBox);
}

BEGIN_MESSAGE_MAP(CXX_CPPSMPEXDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_LBN_DBLCLK(IDC_LIST1, &CXX_CPPSMPEXDlg::OnLbnDblclkList1)
	ON_BN_CLICKED(IDOK, &CXX_CPPSMPEXDlg::OnBnClickedOk)
	ON_WM_CLOSE()
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_BUTTON1, &CXX_CPPSMPEXDlg::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON2, &CXX_CPPSMPEXDlg::OnBnClickedButton2)
	ON_BN_CLICKED(IDC_BUTTON3, &CXX_CPPSMPEXDlg::OnBnClickedButton3)
END_MESSAGE_MAP()


// CXX_CPPSMPEXDlg ���b�Z�[�W �n���h��

BOOL CXX_CPPSMPEXDlg::OnInitDialog()
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

void CXX_CPPSMPEXDlg::OnPaint()
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
HCURSOR CXX_CPPSMPEXDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

// ����ȍ~ �蓮�ǉ� -------------------->>>

//�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P
// �~�{�^��
//�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q
void CXX_CPPSMPEXDlg::OnClose()
{
	//CDialog::OnClose();		// �����̏���������
}

//�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P
// �����I���{�^��
//�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q
void CXX_CPPSMPEXDlg::OnBnClickedOk()
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
void CXX_CPPSMPEXDlg::OnLbnDblclkList1()
{
	// ListBox�R���g���[���̃v���p�e�B�́A�\�[�g�֎~�ɂ��Ă�����
	// ListBox�R���g���[���̃_�u���N���b�N�C�x���g��ǉ�
	mcls_pLog->ShowEdit();
}


//------------------------------------------
// �e�v���Z�X����������
//------------------------------------------
void CXX_CPPSMPEXDlg::InitProcess()
{
	//// ��ԏ��߂ɂ��낢�돉����
	mcls_pLog = NULL;

	//// �^�X�N����������
	//if( 0 != task_init(XX_CPPSMPEX, 0) ) {		// PCID���Ȃ��ꍇ�B
	//if( 0 != task_init(XX_CPPSMPEX) ) {
	//	syslog(SYSNO_TASK_INIT_ERR, "[%s]", XX_CPPSMPEX);
	//	_ASSERT(0);
	//	exit(-9);		// ���v���Z�X�����I��
	//	return;
	//}
	int nRetc = task_init(XX_CPPSMPEX);
	if( 0 != nRetc ) {
		if(-1 == nRetc) { syslog(SYSNO_TASK_INIT_ERR, "[%s] ���Ƀ^�X�N�N���ς݂ɂ��A�N�����f�B", XX_CPPSMPEX); }
		else			{ syslog(SYSNO_TASK_INIT_ERR, "[%s]", XX_CPPSMPEX); }
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
void CXX_CPPSMPEXDlg::ExitProcess()
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
unsigned WINAPI CXX_CPPSMPEXDlg::MainThread(void* param)
{
	CXX_CPPSMPEXDlg* p = (CXX_CPPSMPEXDlg *)param;
	COMMON_QUE que;			// ��M�\����

	//// �e�v���Z�X���������� (����ȑO�̓��O�o�͋֎~)
	p->InitProcess();

	//// �N���v����荞�� ���[�v�J�n
	while(true) {
		recv_mail(&que);			// �N���v����荞��

		switch(que.nEventNo) {
//*/
//-----------------------------------------------------------------------------------------------
		case FACT_KS_PCNAME_CHANGE:								// PC���̕ύX�ʒm (��KS_SLAVE)
			pLOG(em_MSG), "[MAIL] �p�����[�^�ύX�v�� (FACT_TO_DEFECT_01) [�e�[�u��=%d]", que.fl.data[0]); 
			// ���C���C���X�^���X�֒ʒm
			p->mcls_MI.SetDeliveryMail( E_DEF_ML_PARAM, &que);
			break;
//*/
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
void CXX_CPPSMPEXDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: �����Ƀ��b�Z�[�W �n���h�� �R�[�h��ǉ����邩�A����̏������Ăяo���܂��B

	CDialog::OnTimer(nIDEvent);
}







//�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P
// ��ʍX�V�^�C�}
//�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q

#include "..\..\Library\KizuBCP\BCPBase.h"								// DB�����������݃N���X
#include "..\..\Library\KizuLib\Class\KizuPerformanceCounter.h"			// ���Ԍv���Ǘ��N���X


	struct TYP_BCP_DEFECT_IMG
	{
		char					cKizukenNo[20];			// �r���Ǘ�No
		int						nKizuNo;				// �rNo
		BCPBase::typBindDataEx 	img[4];					// �摜�f�[�^ G/B/R/R-B  (�ϒ��Ή���)
	};

	//// ������ (��{�A�p�x ����\����)
	struct TYP_BCP_DEFECT_ATTR
	{
		char					cKizukenNo[SIZE_KIZUKEN_NO];						// �Ǘ�No
		int						nKizuNo;											// �rNo
		double					attr[MAX_ATTR_BASE];								// ������
	};

	BCPBase*				mcls_pBcpDefectImg;			// DB�����������݃N���X
	TYP_BCP_DEFECT_IMG*		m_pBcpDefectImgBuf;			// BCP�������ݗp�̌Œ胁�����o�b�t�@

	BCPBase*				mcls_pBcpDefectAttr;			// DB�����������݃N���X
	TYP_BCP_DEFECT_ATTR*	m_pBcpDefectAttrBuf;			// BCP�������ݗp�̌Œ胁�����o�b�t�@

	static const int MAX_KIZU = 1300;
	static const int MAX_BATCH = 500;
	//static const int MAX_KIZU = 10;
	//static const int MAX_BATCH = 3;
	//static const int MAX_KIZU = 10044;
	//static const int MAX_BATCH = 50;
	static const int IMG_SIZE = 30*1024;

void CXX_CPPSMPEXDlg::OnBnClickedButton1()
{		
	//// ���O����

	BYTE	bData[IMG_SIZE];
	char	cKizuKen[20] = "test";
	int		nCnt = 0;

	//// �ڑ�
	mcls_pBcpDefectImg = new BCPBase();
	if( ! mcls_pBcpDefectImg->Connect(TASKINI_NAME) ) {
		LOG(em_ERR), "DB �R�l�N�g���s");
		return ;
	}

	//// BCP�C�j�V����
#ifdef DEBUG_LOG_BCPTIME
	LOG(em_INF), " BCP_Init --->>>");
#endif
	if( !mcls_pBcpDefectImg->BCP_Init(DB_DEFECT_IMG_T) ) {
		LOG(em_ERR), "BCP Init���s [%s]", DB_DEFECT_IMG_T);
		mcls_pBcpDefectImg->DisConnect();
		return ;
	}
#ifdef DEBUG_LOG_BCPTIME
	LOG(em_INF), " BCP_Init ---<<<");
#endif

	m_pBcpDefectImgBuf = new TYP_BCP_DEFECT_IMG;
	for(int jj=0; jj<4; jj++) m_pBcpDefectImgBuf->img[jj].Alloc(KIZUBCP_MAX_IMGSIZE);	
#ifdef DEBUG_LOG_BCPTIME
	LOG(em_INF), " BCP_Bind --->>>");
#endif
	mcls_pBcpDefectImg->BCP_Bind_String	(1, m_pBcpDefectImgBuf->cKizukenNo);
	mcls_pBcpDefectImg->BCP_Bind_Value 	(2, (BYTE*)&m_pBcpDefectImgBuf->nKizuNo,	BCPBase::em_Int);
	for(int jj=0; jj<4; jj++) mcls_pBcpDefectImg->BCP_Bind_Binary(3+jj, &m_pBcpDefectImgBuf->img[jj], BCPBase::em_VerBinary);
#ifdef DEBUG_LOG_BCPTIME
	LOG(em_INF), " BCP_Bind ---<<<");
#endif


	KizuPerformanceCounter  ti;
	ti.InitAve(); 
	ti.CounterStart();

	LOG(em_MSG), "�J�n");



	for(int ii=0; ii<MAX_KIZU; ii++) {
		for(int jj=0; jj<4; jj++) {
			memcpy(m_pBcpDefectImgBuf->img[jj].pData, bData, IMG_SIZE);
			m_pBcpDefectImgBuf->img[jj].SetSize( IMG_SIZE);
#ifdef DEBUG_LOG_BCPTIME
	//LOG(em_INF), " BCP_ColLen --->>>");
#endif
			mcls_pBcpDefectImg->BCP_ColLen( 3+jj, IMG_SIZE );	// varbinary(MAX) �ɒ�`�ύX���Ă���A0xFFFF�܂ł����o�^�ł��Ȃ��b��΍�
#ifdef DEBUG_LOG_BCPTIME
	//LOG(em_INF), " BCP_ColLen ---<<<");
#endif
		}

		memcpy(m_pBcpDefectImgBuf->cKizukenNo, cKizuKen, sizeof(cKizuKen));		// �r���Ǘ�No
		m_pBcpDefectImgBuf->nKizuNo = ii;													// �rNo
#ifdef DEBUG_LOG_BCPTIME
		//LOG(em_INF), " BCP_SendRow --->>>");
#endif
		bool bWk = mcls_pBcpDefectImg->BCP_SendRow(  );
#ifdef DEBUG_LOG_BCPTIME
		//LOG(em_INF), " BCP_SendRow ---<<<");
#endif

		
		nCnt+= 1;
		if( nCnt >= MAX_BATCH ) {
#ifdef DEBUG_LOG_BCPTIME
			LOG(em_INF), " BCP_Batch --->>>");
#endif
			mcls_pBcpDefectImg->BCP_Batch();
#ifdef DEBUG_LOG_BCPTIME
			LOG(em_INF), " BCP_Batch ---<<<");
#endif
			nCnt = 0;
			LOG(em_INF), "�r���E�E�E %d�s", ii);

		}
	}
	if( nCnt != 0) {
#ifdef DEBUG_LOG_BCPTIME
		LOG(em_INF), " BCP_Batch --->>>");
#endif
		mcls_pBcpDefectImg->BCP_Batch();
#ifdef DEBUG_LOG_BCPTIME
		LOG(em_INF), " BCP_Batch ---<<<");
#endif
	}

	ti.CounterEnd(); 
	LOG(em_MSG), "���� %.3fms", ti.GetCycleTime());
	double dSize = ((IMG_SIZE*4)*MAX_KIZU)/1024/1024;		// DB�����݃T�C�Y(MB)
	LOG(em_MSG), "�����ݑ��x %.3fMB/sec = %.3fMB / %.3fms", dSize/(ti.GetCycleTime()/1000), dSize, ti.GetCycleTime());


	//// �ؒf
#ifdef DEBUG_LOG_BCPTIME
	//LOG(em_INF), " BCP_Down --->>>");
#endif
	mcls_pBcpDefectImg->BCP_Down() ;
#ifdef DEBUG_LOG_BCPTIME
	//LOG(em_INF), " BCP_Down ---<<<");
#endif
	delete m_pBcpDefectImgBuf;
	delete mcls_pBcpDefectImg;

#ifdef DEBUG_LOG_BCPTIME
	LOG(em_INF), " DELETE --->>>");
#endif
#if 1
	CString sql;
	sql.Format("DELETE FROM %s WHERE �Ǘ�No='%s'", DB_DEFECT_IMG_T, cKizuKen);
	//sql.Format("TRUNCATE TABLE %s", DB_DEFECT_IMG_T);
	//sql.Format("SELECT * FROM %s WHERE �Ǘ�No='%s'", DB_DEFECT_IMG_T, cKizuKen);
	OdbcBase	cls;
	cls.Connect(TASKINI_NAME);
	cls.ExecuteDirect(sql);
	cls.DisConnect();
#endif
#ifdef DEBUG_LOG_BCPTIME
	LOG(em_INF), " DELETE ---<<<");
#endif


}


void CXX_CPPSMPEXDlg::OnBnClickedButton2()
{
	//// ���O����

	BYTE	bData[IMG_SIZE];
	char	cKizuKen[20] = "test";
	int		nCnt = 0;

	CString sql;
	sql.Format("DELETE FROM %s WHERE �Ǘ�No='%s'", DB_DEFECT_IMG_T, cKizuKen);
	OdbcBase	cls;
	cls.Connect(TASKINI_NAME);
	cls.ExecuteDirect(sql);
	cls.DisConnect();

	// �����݌v���J�n
	cls.Connect(TASKINI_NAME);

	KizuPerformanceCounter  ti;
	ti.InitAve();
	ti.CounterStart();

	LOG(em_MSG), "�J�n");


	for (int ii = 0; ii<MAX_KIZU; ii++) {
		DWORD wksize[4];
		LPVOID wkpVal[4];
		// �f�[�^�Z�b�g

		for (int jj = 0; jj<4; jj++) {
			wksize[jj] = IMG_SIZE;
			wkpVal[jj] = (BYTE*)&bData[0];
		}


		CString		cSql;

		cSql.Format("INSERT INTO %s VALUES(", DB_DEFECT_IMG_T);

		CreateSQL::sqlColumnInsert(&cSql, cKizuKen, SIZE_KIZUKEN_NO);	// �Ǘ�No
		CreateSQL::sqlColumnInsert(&cSql, ii);							// �rNo
		cSql += "?, ?, ?, ?";										// ���`��
		cSql += ")";

		// �o�^
		if (!cls.SqlBinalyWrite(cSql, 4, wksize, wkpVal, false)) {
			LOG(em_NON), "%s", cSql);
			LOG(em_ERR), "�������ѓo�^���s [%s]", cKizuKen);
		}

		nCnt += 1;
		if (nCnt >= MAX_BATCH) {
			nCnt = 0;
			LOG(em_INF), "�r���E�E�E %d�s", ii);

		}
	}

	ti.CounterEnd();
	LOG(em_MSG), "���� %.3fms", ti.GetCycleTime());
	double dSize = ((IMG_SIZE * 4)*MAX_KIZU) / 1024 / 1024;		// DB�����݃T�C�Y(MB)
	LOG(em_MSG), "�����ݑ��x %.3fMB/sec = %.3fMB / %.3fms", dSize / (ti.GetCycleTime() / 1000), dSize, ti.GetCycleTime());


	//// �ؒf
	cls.DisConnect();

}


void CXX_CPPSMPEXDlg::OnBnClickedButton3()
{
	//// ���O����

	char	cKizuKen[20] = "test";
	int		nCnt = 0;

	CString sql;
	sql.Format("DELETE FROM %s WHERE �Ǘ�No='%s'", DB_DEFECT_ATTR0_T, cKizuKen);
	OdbcBase	cls;
	cls.Connect(TASKINI_NAME);
	cls.ExecuteDirect(sql);
	cls.DisConnect();



	//// �ڑ�
	mcls_pBcpDefectAttr = new BCPBase();
	if (!mcls_pBcpDefectAttr->Connect(TASKINI_NAME)) {
		LOG(em_ERR), "DB �R�l�N�g���s");
		return;
	}

	//// BCP�C�j�V����
#ifdef DEBUG_LOG_BCPTIME
	LOG(em_INF), " BCP_Init --->>>");
#endif
	if (!mcls_pBcpDefectAttr->BCP_Init(DB_DEFECT_ATTR0_T)) {
		LOG(em_ERR), "BCP Init���s [%s]", DB_DEFECT_ATTR0_T);
		mcls_pBcpDefectAttr->DisConnect();
		return;
	}
#ifdef DEBUG_LOG_BCPTIME
	LOG(em_INF), " BCP_Init ---<<<");
#endif

	m_pBcpDefectAttrBuf = new TYP_BCP_DEFECT_ATTR;
#ifdef DEBUG_LOG_BCPTIME
	LOG(em_INF), " BCP_Bind --->>>");
#endif
	mcls_pBcpDefectAttr->BCP_Bind_String(1, m_pBcpDefectAttrBuf->cKizukenNo);
	mcls_pBcpDefectAttr->BCP_Bind_Value(2, (BYTE*)&m_pBcpDefectAttrBuf->nKizuNo, BCPBase::em_Int);
	for (int jj = 0; jj<MAX_ATTR_BASE; jj++) mcls_pBcpDefectAttr->BCP_Bind_Value(3 + jj, (BYTE*)&m_pBcpDefectAttrBuf->attr[jj], BCPBase::em_Double);
#ifdef DEBUG_LOG_BCPTIME
	LOG(em_INF), " BCP_Bind ---<<<");
#endif


	KizuPerformanceCounter  ti;
	ti.InitAve();
	ti.CounterStart();

	LOG(em_MSG), "�J�n");



	for (int ii = 0; ii<MAX_KIZU; ii++) {

		memcpy(m_pBcpDefectAttrBuf->cKizukenNo, cKizuKen, sizeof(cKizuKen));		// �r���Ǘ�No
		m_pBcpDefectAttrBuf->nKizuNo = ii;													// �rNo
		for (int jj = 0; jj < MAX_ATTR_BASE; jj++) m_pBcpDefectAttrBuf->attr[jj] = jj;
#ifdef DEBUG_LOG_BCPTIME
		//LOG(em_INF), " BCP_SendRow --->>>");
#endif
		bool bWk = mcls_pBcpDefectAttr->BCP_SendRow();
#ifdef DEBUG_LOG_BCPTIME
		//LOG(em_INF), " BCP_SendRow ---<<<");
#endif


		nCnt += 1;
		if (nCnt >= MAX_BATCH) {
#ifdef DEBUG_LOG_BCPTIME
			LOG(em_INF), " BCP_Batch --->>>");
#endif
			mcls_pBcpDefectAttr->BCP_Batch();
#ifdef DEBUG_LOG_BCPTIME
			LOG(em_INF), " BCP_Batch ---<<<");
#endif
			nCnt = 0;
			LOG(em_INF), "�r���E�E�E %d�s", ii);

		}
	}
	if (nCnt != 0) {
#ifdef DEBUG_LOG_BCPTIME
		LOG(em_INF), " BCP_Batch --->>>");
#endif
		mcls_pBcpDefectAttr->BCP_Batch();
#ifdef DEBUG_LOG_BCPTIME
		LOG(em_INF), " BCP_Batch ---<<<");
#endif
	}

	ti.CounterEnd();
	LOG(em_MSG), "���� %.3fms", ti.GetCycleTime());
	double dSize = ((MAX_ATTR_BASE * 8)*MAX_KIZU) / 1024 / 1024;		// DB�����݃T�C�Y(MB)
	LOG(em_MSG), "�����ݑ��x %.3fMB/sec = %.3fMB / %.3fms", dSize / (ti.GetCycleTime() / 1000), dSize, ti.GetCycleTime());


	//// �ؒf
#ifdef DEBUG_LOG_BCPTIME
	//LOG(em_INF), " BCP_Down --->>>");
#endif
	mcls_pBcpDefectAttr->BCP_Down();
#ifdef DEBUG_LOG_BCPTIME
	//LOG(em_INF), " BCP_Down ---<<<");
#endif
	delete m_pBcpDefectAttrBuf;
	delete mcls_pBcpDefectAttr;

}
