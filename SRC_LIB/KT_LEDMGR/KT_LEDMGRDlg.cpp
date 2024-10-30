// KT_LEDMGRDlg.cpp : �����t�@�C��
//

#include "stdafx.h"
#include "KT_LEDMGR.h"
#include "KT_LEDMGRDlg.h"

// �������[���[�N���o �蓮�ǉ� ----->>>
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif
// <<<------------------------

// CKT_LEDMGRDlg �_�C�A���O


CKT_LEDMGRDlg::CKT_LEDMGRDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CKT_LEDMGRDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CKT_LEDMGRDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, m_ListBox);
	DDX_Control(pDX, IDC_CMB_DISP, m_cmbSession);
	DDX_Control(pDX, IDC_CMB_CH, m_cmbCh);
	DDX_Control(pDX, IDC_CMB_DEBAG_CH, m_cmbDebagCh);
	DDX_Control(pDX, IDC_LIST_VAL, m_ListVal);
	DDX_Control(pDX, IDC_LIST_TEMP, m_lstTemp);
}

BEGIN_MESSAGE_MAP(CKT_LEDMGRDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_LBN_DBLCLK(IDC_LIST1, &CKT_LEDMGRDlg::OnLbnDblclkList1)
	ON_BN_CLICKED(IDOK, &CKT_LEDMGRDlg::OnBnClickedOk)
	ON_WM_CLOSE()
	ON_WM_TIMER()
//	ON_WM_ENDSESSION()
	ON_BN_CLICKED(IDC_DEBAG_STATE, &CKT_LEDMGRDlg::OnBnClickedDebagState)
	ON_BN_CLICKED(IDC_DEBAG_TEMP, &CKT_LEDMGRDlg::OnBnClickedDebagTemp)
	ON_BN_CLICKED(IDC_DEBAG_ON, &CKT_LEDMGRDlg::OnBnClickedDebagOn)
	ON_BN_CLICKED(IDC_DEBAG_OFF, &CKT_LEDMGRDlg::OnBnClickedDebagOff)
	ON_BN_CLICKED(IDC_DEBAG_RESET, &CKT_LEDMGRDlg::OnBnClickedDebagReset)
	ON_BN_CLICKED(IDC_DEBAG_LED_SCODE, &CKT_LEDMGRDlg::OnBnClickedDebagLedScode)
	ON_BN_CLICKED(IDC_DEBAG_LED_VAL, &CKT_LEDMGRDlg::OnBnClickedDebagLedVal)
	ON_BN_CLICKED(IDC_DEBAG_LED_HOSEI, &CKT_LEDMGRDlg::OnBnClickedDebagLedHosei)
	ON_BN_CLICKED(IDC_DEBAG_LED_HOSEI_ALL, &CKT_LEDMGRDlg::OnBnClickedDebagLedHoseiAll)
END_MESSAGE_MAP()


// CKT_LEDMGRDlg ���b�Z�[�W �n���h��

BOOL CKT_LEDMGRDlg::OnInitDialog()
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

void CKT_LEDMGRDlg::OnPaint()
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
HCURSOR CKT_LEDMGRDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

// ����ȍ~ �蓮�ǉ� -------------------->>>

//�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P
// �~�{�^��
//�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q
void CKT_LEDMGRDlg::OnClose()
{
//	CDialog::OnClose();
}


//�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P
// �����I���{�^��
//�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q
void CKT_LEDMGRDlg::OnBnClickedOk()
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
// Windows�V���b�g�_�E��
//�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q
//void CKT_LEDMGRDlg::OnEndSession(BOOL bEnding)
//{
//	// �����Ȃ�Windows���I�������΍�
//	if( bEnding ) ExitProcess();
//
//	CDialog::OnEndSession(bEnding);
//}

//�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P
// �������N��
//�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q
void CKT_LEDMGRDlg::OnLbnDblclkList1()
{
	// ListBox�R���g���[���̃v���p�e�B�́A�\�[�g�֎~�ɂ��Ă�����
	// ListBox�R���g���[���̃_�u���N���b�N�C�x���g��ǉ�
	mcls_pLog->ShowEdit();
}


//------------------------------------------
// �e�v���Z�X����������
//------------------------------------------
void CKT_LEDMGRDlg::InitProcess()
{
	CString sWk;

	//// �^�X�N����������
	//if( 0 != task_init(XX_CPPSMPEX, 0) ) {		// PCID���Ȃ��ꍇ�B
	if( 0 != task_init(KT_LEDMGR) ) {
		syslog(SYSNO_TASK_INIT_ERR, "[%s]", KT_LEDMGR);
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
	

	//// ��ʍX�V�p�f�[�^�Z�b�g

	//-----------------------
	// �R���{�{�b�N�X�ɃZ�b�V�������ǉ�
	for(int ii=0; ii<MAX_THREAD; ii++) {
		////// �H�H �Ȃ���Release�̏ꍇ���� ������� _ �������Ă���Ɠo�^���s����
		if( NULL == mcls_MI.gcls_pLedMgr[ii] ) break;
		m_cmbSession.AddString( mcls_MI.gcls_pLedMgr[ii]->GetMyName() );
	}
	m_cmbSession.SetCurSel(0);				// 0�Ԗڂ������I���Ƃ���

	for(int ii=0; ii<MAX_LED_CH; ii++) {
		sWk.Format("%d", ii+1);
		m_cmbCh.AddString( sWk );
	}
	m_cmbCh.SetCurSel(0);	
	for(int ii=0; ii<=MAX_LED_CH; ii++) {
		sWk.Format("%d", ii);
		m_cmbDebagCh.AddString( sWk );
	}
	m_cmbDebagCh.SetCurSel(0);	


	//-----------------------
	// �z���֌W ���X�g�r���[�C���X�^���X���� & ��`
	if(true) {
		mcnt_pListVal = new DspListView(&m_ListVal);
		// �����ڒ�`
		m_ListVal.SetExtendedStyle( m_ListVal.GetExtendedStyle() | LVS_EX_GRIDLINES );			// �r���`��
		m_ListVal.SetExtendedStyle( m_ListVal.GetExtendedStyle() | LVS_EX_FULLROWSELECT );		// 1�s�S�̑I��
		m_ListVal.ModifyStyle( 0, LVS_SHOWSELALWAYS);			// ��t�H�[�J�X�����I�����c��

	
		// ��ǉ�
		// ��{�̃u���b�N�����擾(CH�ɐݒ肳��ĂȂ���΁A���̒l�ɂȂ�)
		int nBlock		= GetPrivateProfileInt(KT_LEDMGR, "BLOCK", 0, TASKINI_NAME);

		// �SCH�̃L�[���`�F�b�N���āA�ő�̔z���u���b�N�����擾
		CString sWk;
		char	cKey[64];
		for(int ii=0; ii<MAX_THREAD; ii++) {
			// �L�[�`�F�b�N(���u�P��)
			sWk.Format("NAME_%d", ii+1);
			GetPrivateProfileString(KT_LEDMGR, sWk, "", cKey, sizeof(cKey), TASKINI_NAME);
			if(0 == strlen(cKey) ) break;

			for(int jj=1; jj<=MAX_LED_CH; jj++){
				sWk.Format("KT_LEDMGR_%s_CH%d", cKey, jj);
				int nWk = GetPrivateProfileInt(sWk, "BLOCK", -1, TASKINI_NAME);
				nBlock = max(nBlock, nWk);
			}
		}

		m_nColCntVal = nBlock;

		mcnt_pListVal->SetColumnAdd(40, ""); 
		for(int ii=1; ii<=nBlock; ii++) {
			sWk.Format("%d", ii); 
			mcnt_pListVal->SetColumnAdd(40, sWk); 
		}

		// �s�ǉ�
		mcnt_pListVal->SetRowAll(2);	
		// �Œ�l�Z�b�g
		mcnt_pListVal->SetItem( 0, 0, "�␳");
		mcnt_pListVal->SetItem( 1, 0, "�P�x");

	}

	//-----------------------
	// LED���x ���X�g�r���[�C���X�^���X���� & ��`
	if(true) {
		mcnt_pListTemp = new DspListView(&m_lstTemp);
		// �����ڒ�`
		m_lstTemp.SetExtendedStyle( m_lstTemp.GetExtendedStyle() | LVS_EX_GRIDLINES );			// �r���`��
		m_lstTemp.SetExtendedStyle( m_lstTemp.GetExtendedStyle() | LVS_EX_FULLROWSELECT );		// 1�s�S�̑I��
		m_lstTemp.ModifyStyle( 0, LVS_SHOWSELALWAYS);			// ��t�H�[�J�X�����I�����c��

	
		// ��ǉ�
		// ��{�̌v���_�����擾(CH�ɐݒ肳��ĂȂ���΁A���̒l�ɂȂ�)
		int nTemp		= GetPrivateProfileInt(KT_LEDMGR, "TEMP_UNIT", 0, TASKINI_NAME);

		// �SCH�̃L�[���`�F�b�N���āA�ő�̔z���u���b�N�����擾
		CString sWk;
		char	cKey[64];
		for(int ii=0; ii<MAX_THREAD; ii++) {
			// �L�[�`�F�b�N(���u�P��)
			sWk.Format("NAME_%d", ii+1);
			GetPrivateProfileString(KT_LEDMGR, sWk, "", cKey, sizeof(cKey), TASKINI_NAME);
			if(0 == strlen(cKey) ) break;

			for(int jj=1; jj<=MAX_LED_CH; jj++){
				sWk.Format("KT_LEDMGR_%s_CH%d", cKey, jj);
				int nWk = GetPrivateProfileInt(sWk, "TEMP_UNIT", -1, TASKINI_NAME);
				nTemp = max(nTemp, nWk);
			}
		}

		m_nColCntTemp = nTemp;

//		mcnt_pListTemp->SetColumnAdd(40, ""); 
		for(int ii=0; ii<nTemp; ii++) {
			sWk.Format("%d", ii+1); 
			mcnt_pListTemp->SetColumnAdd(40, sWk); 
		}

		// �s�ǉ�
		mcnt_pListTemp->SetRowAll(1);	
		// �Œ�l�Z�b�g
//		mcnt_pListTemp->SetItem( 0, 0, "���x");

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
void CKT_LEDMGRDlg::ExitProcess()
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
	

	// ���X�g�r���[�̏I��
	delete(mcnt_pListTemp);
	delete(mcnt_pListVal);


	// �^�X�N�I������
	task_exit();

	// ��ʂ̏I���v��
	PostMessage(WM_QUIT);
}

//------------------------------------------
// ���C���X���b�h DlgThread
// LPVOID param �_�C�����O�|�C���^
//------------------------------------------
unsigned WINAPI CKT_LEDMGRDlg::MainThread(void* param)
{
	CKT_LEDMGRDlg* p = (CKT_LEDMGRDlg *)param;
	COMMON_QUE que;			// ��M�\����

	//// �e�v���Z�X���������� (����ȑO�̓��O�o�͋֎~)
	p->InitProcess();

	//// �N���v����荞�� ���[�v�J�n
	while(true) {
		recv_mail(&que);			// �N���v����荞��

		switch(que.nEventNo) {
//-----------------------------------------------------------------------------------------------
		case FACT_KT_LEDMGR_01:								// �p�����[�^�ύX�v�� (��TO_DEFECT)
			pLOG(em_MSG), "[MAIL] �p�����[�^�ύX�v�� (FACT_KT_LEDMGR_01) [scode=%d]", que.fl.data[0]); 
			// ���C���C���X�^���X�֒ʒm
			p->mcls_MI.SetDeliveryMail( E_DEF_ML_PARAM, &que);
			break;

//-----------------------------------------------------------------------------------------------
		case FACT_KT_LEDMGR_02:								// ���ʕύX�v��(�����\�ʏ�Ԏw��) (��TO_DEFECT)
			/// 2014.10.28 ����(���P�xLED�Ή�)------------------------------------------------>>>>
			//pLOG(em_MSG), "[MAIL] ���ʕύX�v��(�����\�ʏ�Ԏw��) (FACT_KT_LEDMGR_02) [�\��=%d][CH=%d][scode=%d]", que.fl.data[0], que.fl.data[1], que.fl.data[2]); 
			//if( 0 > que.fl.data[0] || que.fl.data[0] >= NUM_MEN ) break;
			//if( 0 > que.fl.data[1] || que.fl.data[1] > MAX_LED_CH ) break;
			//if( 0 >= que.fl.data[2] || que.fl.data[2] > MAX_SCODE ) break;

			//for(int ii=0; ii<MAX_THREAD; ii++) {
			//	if(NULL == p->mcls_MI.gcls_pLedMgr[ii]) continue;	

			//	if( que.fl.data[0] == p->mcls_MI.gcls_pLedMgr[ii]->GetMen() ) {

			//		p->mcls_MI.gcls_pLedMgr[ii]->AddQue_SetLigthMastVal(que.fl.data[1], que.fl.data[2]);
			//	}
			//}
			pLOG(em_MSG), "[MAIL] ���ʕύX�v��(�����\�ʏ�Ԏw��) (FACT_KT_LEDMGR_02) [�\��=%d][INDEX=%d][scode=%d]", que.fl.data[0], que.fl.data[1], que.fl.data[2]); 
			if( 0 > que.fl.data[0] || que.fl.data[0] > NUM_MEN ) break;
			//if( 0 > que.fl.data[0] || que.fl.data[0] >= NUM_MEN ) break;
			if( 0 > que.fl.data[1] || que.fl.data[1] > DEF_INDEX_END ) break;
			if( 0 >= que.fl.data[2] || que.fl.data[2] > MAX_SCODE ) break;

			for(int ii=0; ii<MAX_THREAD; ii++) {
				if(NULL == p->mcls_MI.gcls_pLedMgr[ii]) continue;	
				if(que.fl.data[0] == p->mcls_MI.gcls_pLedMgr[ii]->GetMen() ){
					p->mcls_MI.gcls_pLedMgr[ii]->AddQue_SetLightMastVal_Index(que.fl.data[1], que.fl.data[2]);
				}
			}
			/// 2014.10.28 ����(���P�xLED�Ή�)<<<<------------------------------------------------
			break;

//-----------------------------------------------------------------------------------------------
		case FACT_KT_LEDMGR_03:								// ���ʕύX�v��(���ʎw��) (��TO_DEFECT)
			//pLOG(em_MSG), "[MAIL] ���ʕύX�v��(���ʎw��) (FACT_KT_LEDMGR_03) [�\��=%d][CH=%d][val=%d,%d]", que.fl.data[0], que.fl.data[1], que.fl.data[2], que.fl.data[3]); 
			//if( 0 > que.fl.data[0] || que.fl.data[0] >= NUM_MEN	) break;
			//if( 0 > que.fl.data[1] || que.fl.data[1] > MAX_LED_CH ) break;		
			//if( 0 > que.fl.data[2] || que.fl.data[2] >= 256 ) break;
			//if( 0 > que.fl.data[3] || que.fl.data[3] >= 256 ) break;

			//for(int ii=0; ii<MAX_THREAD; ii++) {
			//	if(NULL == p->mcls_MI.gcls_pLedMgr[ii]) continue;	

			//	if( que.fl.data[0] == p->mcls_MI.gcls_pLedMgr[ii]->GetMen() ) {
			//		p->mcls_MI.gcls_pLedMgr[ii]->AddQue_SetLigthMastVal(que.fl.data[1], (BYTE)que.fl.data[2], (BYTE)que.fl.data[3]);
			//	}
			//}
			pLOG(em_MSG), "[MAIL] ���ʕύX�v��(���ʎw��) (FACT_KT_LEDMGR_03) [�\��=%d][INDEX=%d][val=%d,%d]", que.fl.data[0], que.fl.data[1], que.fl.data[2], que.fl.data[3]); 
			//if( 0 > que.fl.data[0] || que.fl.data[0] >= NUM_MEN	) break;
			if( 0 > que.fl.data[0] || que.fl.data[0] > NUM_MEN	) break;
			if( 0 > que.fl.data[1] || que.fl.data[1] > DEF_INDEX_END ) break;		
			if( 0 > que.fl.data[2] || que.fl.data[2] >= 256 ) break;
			if( 0 > que.fl.data[3] || que.fl.data[3] >= 256 ) break;

			for(int ii=0; ii<MAX_THREAD; ii++) {
				if(NULL == p->mcls_MI.gcls_pLedMgr[ii]) continue;
				if( que.fl.data[0] == p->mcls_MI.gcls_pLedMgr[ii]->GetMen() ){
					p->mcls_MI.gcls_pLedMgr[ii]->AddQue_SetLightMastVal_Index(que.fl.data[1], que.fl.data[2], (BYTE)que.fl.data[3]);
				}
			}
			break;

//-----------------------------------------------------------------------------------------------
		case FACT_KT_LEDMGR_04:								// �_�������w�� (��TO_DEFECT)
			//pLOG(em_MSG), "[MAIL] �_�������w�� (FACT_KT_LEDMGR_04) [�\��=%d][CH=%d][%s]", que.fl.data[0], que.fl.data[1], (1==que.fl.data[2]?"�_��":"����") ); 
			//if( 0 > que.fl.data[0] || que.fl.data[0] >= NUM_MEN ) break;
			//if( 0 > que.fl.data[1] || que.fl.data[1] > MAX_LED_CH ) break;	

			//for(int ii=0; ii<MAX_THREAD; ii++) {
			//	if(NULL == p->mcls_MI.gcls_pLedMgr[ii]) continue;	

			//	if( que.fl.data[0] == p->mcls_MI.gcls_pLedMgr[ii]->GetMen() ) {
			//		if(1==que.fl.data[2]) p->mcls_MI.gcls_pLedMgr[ii]->AddQue_SendCommand(LedManager::SEND_EXEC_ON, que.fl.data[1]);
			//		else				  p->mcls_MI.gcls_pLedMgr[ii]->AddQue_SendCommand(LedManager::SEND_EXEC_OFF, que.fl.data[1]);		
			//	}
			//}
			pLOG(em_MSG), "[MAIL] �_�������w�� (FACT_KT_LEDMGR_04) [�\��=%d][Index=%d][%s]", que.fl.data[0], que.fl.data[1], (1==que.fl.data[2]?"�_��":"����") ); 
			if( 0 > que.fl.data[0] || que.fl.data[0] > NUM_MEN ) break;
			//if( 0 > que.fl.data[0] || que.fl.data[0] >= NUM_MEN ) break;
			if( 0 > que.fl.data[1] || que.fl.data[1] > DEF_INDEX_END ) break;	

			for(int ii=0; ii<MAX_THREAD; ii++) {
				if( NULL == p->mcls_MI.gcls_pLedMgr[ii] ) continue;
				if( que.fl.data[0] == p->mcls_MI.gcls_pLedMgr[ii]->GetMen() ){
					if(1==que.fl.data[2])	p->mcls_MI.gcls_pLedMgr[ii]->AddQue_SendCommand_Index(LedManager::SEND_EXEC_ON, que.fl.data[1]);
					else					p->mcls_MI.gcls_pLedMgr[ii]->AddQue_SendCommand_Index(LedManager::SEND_EXEC_OFF, que.fl.data[1]);
				}
			}
			break;

//-----------------------------------------------------------------------------------------------
		case FACT_KT_LEDMGR_05:								// �_���ݐώ��ԃ��Z�b�g (��SO_GAMENN)
			//pLOG(em_MSG), "[MAIL] �_���ݐώ��ԃ��Z�b�g (FACT_KT_LEDMGR_05) [�\��=%d][CH=%d]", que.fl.data[0], que.fl.data[1] ); 
			//if( 0 > que.fl.data[0] || que.fl.data[0] >= NUM_MEN ) break;
			//if( 0 > que.fl.data[1] || que.fl.data[1] > MAX_LED_CH ) break;	

			//for(int ii=0; ii<MAX_THREAD; ii++) {
			//	if(NULL == p->mcls_MI.gcls_pLedMgr[ii]) continue;	

			//	if( que.fl.data[0] == p->mcls_MI.gcls_pLedMgr[ii]->GetMen() ) {
			//		p->mcls_MI.gcls_pLedMgr[ii]->AddTime_Reset(que.fl.data[1]);
			//	}
			//}
			pLOG(em_MSG), "[MAIL] �_���ݐώ��ԃ��Z�b�g (FACT_KT_LEDMGR_05) [�\��=%d][INDEX=%d]", que.fl.data[0], que.fl.data[1] ); 
			if( 0 > que.fl.data[0] || que.fl.data[0] > NUM_MEN ) break;
			//if( 0 > que.fl.data[0] || que.fl.data[0] >= NUM_MEN ) break;
			if( 0 > que.fl.data[1] || que.fl.data[1] > DEF_INDEX_END ) break;	

			for(int ii=0; ii<MAX_THREAD; ii++) {
				if(NULL == p->mcls_MI.gcls_pLedMgr[ii]) continue;	
				if( que.fl.data[0] == p->mcls_MI.gcls_pLedMgr[ii]->GetMen() ){
					p->mcls_MI.gcls_pLedMgr[ii]->AddTime_Reset_Index(que.fl.data[1]);
				}
			}
			break;

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
void CKT_LEDMGRDlg::OnTimer(UINT_PTR nIDEvent)
{
	CString		sWk;
	int			nWk;

	//// ���ݑI������Ă���C���X�^���X���擾����
	int nIndex = m_cmbSession.GetCurSel();
	if( -1 == nIndex) return;					// ini�t�@�C���̐ݒ薳��
	LedManager* pCls = mcls_MI.gcls_pLedMgr[nIndex];
	if( NULL == pCls ) return;

	int nCh = m_cmbCh.GetCurSel();
	if( -1 == nCh) return;



	//// �X�e�[�^�X�Z�b�g
	SetDlgItemText(IDC_STAT_CONNECT,	pCls->GetSendStatName(0));
	SetDlgItemText(IDC_STAT_MODE,		pCls->GetMainModeName());

	SetDlgItemInt(IDC_NOWSCODE,			pCls->GetNowScode() );
	SetDlgItemInt(IDC_NOWLEDBASE,		pCls->GetNowLedBase(nCh) );

	/// 2014.10.28 ����(���P�xLED�Ή�)------------------------->>>>
	int nBlock = pCls->GetBlock(nCh);
	for(int ii=0; ii<m_nColCntVal; ii++){
		if(ii < nBlock){
			mcnt_pListVal->SetItem(0, 1+ii, pCls->GetNowLedHosei(nCh, ii) ); 
			mcnt_pListVal->SetItem(1, 1+ii, pCls->GetNowLedValue(nCh, ii) ); 
		}else{
			mcnt_pListVal->SetItem(0, 1+ii, "-" ); 
			mcnt_pListVal->SetItem(1, 1+ii, "-" ); 
		}
	}

	int nTempUnit = pCls->GetTempUnit(nCh);
	for(int ii=0; ii<m_nColCntTemp; ii++){
		if(ii < nTempUnit){
			mcnt_pListTemp->SetItem(0, ii, pCls->GetLedTemp(nCh, ii) ); 
		}else{
			mcnt_pListTemp->SetItem(0, ii, "-" ); 
		}
	}
	/// 2014.10.28 ����(���P�xLED�Ή�)<<<<-------------------------


	////SetDlgItemInt(IDC_NOWLEDHOSEI,		pCls->GetNowLedHosei(nCh) );
	////SetDlgItemInt(IDC_NOWLEDVALUE,		pCls->GetNowLedValue(nCh) );

	nWk = pCls->GetStateOnOff(nCh);
	if(0==nWk)		SetDlgItemText(IDC_STATEONOFF, "��");
	else if(1==nWk) SetDlgItemText(IDC_STATEONOFF, "�~");
	else			SetDlgItemText(IDC_STATEONOFF, "�s��");
	nWk = pCls->GetStateTemp(nCh);
	if(0==nWk)		SetDlgItemText(IDC_STATETEMP, "��");
	else if(1==nWk) SetDlgItemText(IDC_STATETEMP, "���M");
	else if(2==nWk) SetDlgItemText(IDC_STATETEMP, "�����޳�");
	else			SetDlgItemText(IDC_STATETEMP, "�s��");
	nWk = pCls->GetStateBreak(nCh);
	if(0==nWk)		SetDlgItemText(IDC_STATEBREAK, "��");
	else if(1==nWk) SetDlgItemText(IDC_STATEBREAK, "�~");
	else			SetDlgItemText(IDC_STATEBREAK, "�s��");

	SetDlgItemInt(IDC_TEMP1, pCls->GetLedTemp(nCh, 0));
	SetDlgItemInt(IDC_TEMP2, pCls->GetLedTemp(nCh, 1));
	SetDlgItemInt(IDC_TEMP3, pCls->GetLedTemp(nCh, 2));

	CDialog::OnTimer(nIDEvent);
}


//�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P
// �S��ԥ�P�x�擾
//�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q
void CKT_LEDMGRDlg::OnBnClickedDebagState()
{
	if ( BST_CHECKED != CWnd::IsDlgButtonChecked(IDC_DEBUG_MANUAL) ) return;
	//// ���ݑI������Ă���C���X�^���X���擾����
	int nIndex = m_cmbSession.GetCurSel();
	if( -1 == nIndex) return;					// ini�t�@�C���̐ݒ薳��
	LedManager* pCls = mcls_MI.gcls_pLedMgr[nIndex];
	if( NULL == pCls ) return;

	int nCh = m_cmbDebagCh.GetCurSel();
	if( -1 == nCh) return;

	LOG(em_WAR), "[Debag] �S��Ԏ擾 <%s>[CH=%d]", pCls->GetMyName(), nCh); 

	pCls->AddQue_SendCommand(LedManager::SEND_CHECK_BREAK, nCh);
	pCls->AddQue_SendCommand(LedManager::SEND_CHECK_HEATING, nCh);
	pCls->AddQue_SendCommand(LedManager::SEND_CHECK_TEMP, nCh);
	pCls->AddQue_SendCommand(LedManager::SEND_CHECK_ONOFF, nCh);
	pCls->AddQue_SendCommand(LedManager::SEND_READ_EEPROM, nCh);
}

//�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P
// LED���x
//�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q
void CKT_LEDMGRDlg::OnBnClickedDebagTemp()
{
	if ( BST_CHECKED != CWnd::IsDlgButtonChecked(IDC_DEBUG_MANUAL) ) return;
	//// ���ݑI������Ă���C���X�^���X���擾����
	int nIndex = m_cmbSession.GetCurSel();
	if( -1 == nIndex) return;					// ini�t�@�C���̐ݒ薳��
	LedManager* pCls = mcls_MI.gcls_pLedMgr[nIndex];
	if( NULL == pCls ) return;

	int nCh = m_cmbDebagCh.GetCurSel();
	if( -1 == nCh) return;

	LOG(em_WAR), "[Debag] LED���x�v���w�� <%s>[CH=%d]", pCls->GetMyName(), nCh); 

	pCls->AddQue_SendCommand(LedManager::SEND_CHECK_TEMP, nCh);
}
//�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P
// �_��
//�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q
void CKT_LEDMGRDlg::OnBnClickedDebagOn()
{
	if ( BST_CHECKED != CWnd::IsDlgButtonChecked(IDC_DEBUG_MANUAL) ) return;
	//// ���ݑI������Ă���C���X�^���X���擾����
	int nIndex = m_cmbSession.GetCurSel();
	if( -1 == nIndex) return;					// ini�t�@�C���̐ݒ薳��
	LedManager* pCls = mcls_MI.gcls_pLedMgr[nIndex];
	if( NULL == pCls ) return;

	int nCh = m_cmbDebagCh.GetCurSel();
	if( -1 == nCh) return;

	LOG(em_WAR), "[Debag] �_���w�� <%s>[CH=%d]", pCls->GetMyName(), nCh); 

	pCls->AddQue_SendCommand(LedManager::SEND_EXEC_ON, nCh);
}
//�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P
// ����
//�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q
void CKT_LEDMGRDlg::OnBnClickedDebagOff()
{
	if ( BST_CHECKED != CWnd::IsDlgButtonChecked(IDC_DEBUG_MANUAL) ) return;
	//// ���ݑI������Ă���C���X�^���X���擾����
	int nIndex = m_cmbSession.GetCurSel();
	if( -1 == nIndex) return;					// ini�t�@�C���̐ݒ薳��
	LedManager* pCls = mcls_MI.gcls_pLedMgr[nIndex];
	if( NULL == pCls ) return;

	int nCh = m_cmbDebagCh.GetCurSel();
	if( -1 == nCh) return;

	LOG(em_WAR), "[Debag] �����w�� <%s>[CH=%d]", pCls->GetMyName(), nCh); 

	pCls->AddQue_SendCommand(LedManager::SEND_EXEC_OFF, nCh);
}
//�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P
// �݌v���ԃ��Z�b�g
//�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q
void CKT_LEDMGRDlg::OnBnClickedDebagReset()
{
	if ( BST_CHECKED != CWnd::IsDlgButtonChecked(IDC_DEBUG_MANUAL) ) return;
	//// ���ݑI������Ă���C���X�^���X���擾����
	int nIndex = m_cmbSession.GetCurSel();
	if( -1 == nIndex) return;					// ini�t�@�C���̐ݒ薳��
	LedManager* pCls = mcls_MI.gcls_pLedMgr[nIndex];
	if( NULL == pCls ) return;

	int nCh = m_cmbDebagCh.GetCurSel();
	if( -1 == nCh) return;

	LOG(em_WAR), "[Debag] LED�\���ݐώ��ԃ��Z�b�g <�\��:%d>[CH=%d]", pCls->GetMen(), nCh);

	COMMON_QUE que;
	que.nEventNo = FACT_KT_LEDMGR_05;
	que.nLineNo = getlineid();
	que.fl.data[0] = pCls->GetMen();
	que.fl.data[1] = nCh; 
	send_mail(KT_LEDMGR, ".", &que);
}
//�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P
// scode �w��
//�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q
void CKT_LEDMGRDlg::OnBnClickedDebagLedScode()
{
	if ( BST_CHECKED != CWnd::IsDlgButtonChecked(IDC_DEBUG_MANUAL) ) return;
	//// ���ݑI������Ă���C���X�^���X���擾����
	int nIndex = m_cmbSession.GetCurSel();
	if( -1 == nIndex) return;					// ini�t�@�C���̐ݒ薳��
	LedManager* pCls = mcls_MI.gcls_pLedMgr[nIndex];
	if( NULL == pCls ) return;

	int nCh = m_cmbDebagCh.GetCurSel();
	if( -1 == nCh) return;

	int val;
	val = GetDlgItemInt(IDC_SET_SCODE);	// �ُ�l���͂ł� 0 ���A�邩�� ���ُ͈�̃`�F�b�N�͂��Ȃ���
	if( 0 >= val || val > MAX_SCODE ) return;

	LOG(em_WAR), "[Debag] ���ʃZ�b�g <%s>[CH=%d][%d]", pCls->GetMyName(), nCh, val); 
	pCls->AddQue_SetLightMastVal(nCh, val);
}
//�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P
// �S�̒��� �w��
//�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q
void CKT_LEDMGRDlg::OnBnClickedDebagLedVal()
{
	if ( BST_CHECKED != CWnd::IsDlgButtonChecked(IDC_DEBUG_MANUAL) ) return;
	//// ���ݑI������Ă���C���X�^���X���擾����
	int nIndex = m_cmbSession.GetCurSel();
	if( -1 == nIndex) return;					// ini�t�@�C���̐ݒ薳��
	LedManager* pCls = mcls_MI.gcls_pLedMgr[nIndex];
	if( NULL == pCls ) return;

	int nCh = m_cmbDebagCh.GetCurSel();
	if( -1 == nCh) return;

	int val;
	val = GetDlgItemInt(IDC_SET_BASE);		// �ُ�l���͂ł� 0 ���A�邩�� ���ُ͈�̃`�F�b�N�͂��Ȃ���
	if( 0 > val || val >= 256 ) return;

	LOG(em_WAR), "[Debag] �S�̔z���Z�b�g <%s>[CH=%d][%d]", pCls->GetMyName(), nCh, val); 
	pCls->AddQue_SetLightMaster(nCh, val);
}

//�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P
// �u���b�N�z�� �w��
//�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q
void CKT_LEDMGRDlg::OnBnClickedDebagLedHosei()
{
	if ( BST_CHECKED != CWnd::IsDlgButtonChecked(IDC_DEBUG_MANUAL) ) return;
	//// ���ݑI������Ă���C���X�^���X���擾����
	int nIndex = m_cmbSession.GetCurSel();
	if( -1 == nIndex) return;					// ini�t�@�C���̐ݒ薳��
	LedManager* pCls = mcls_MI.gcls_pLedMgr[nIndex];
	if( NULL == pCls ) return;

	int nCh = m_cmbDebagCh.GetCurSel();
	if( -1 == nCh) return;

	int val;
	int block;
	val = GetDlgItemInt(IDC_SET_HOSEI_VAL);	// �ُ�l���͂ł� 0 ���A�邩�� ���ُ͈�̃`�F�b�N�͂��Ȃ���
	block = GetDlgItemInt(IDC_SET_HOSEI_BLOCK);	// �ُ�l���͂ł� 0 ���A�邩�� ���ُ͈�̃`�F�b�N�͂��Ȃ���
	if( 0 > val || val >= 256 ) return;
	/// 2014.10.28 ����(���P�xLED�Ή�)------------------------->>>>
	//if( 0 >= block || block > mcls_MI.gcls_pLedMgr[0]->GetBlock() ) return;
	if( 0 < nCh)	if( 0 >= block || block > pCls->GetBlock(nCh-1) ) return;
	/// 2014.10.28 ����(���P�xLED�Ή�)<<<<-------------------------

	LOG(em_WAR), "[Debag] ���ʃZ�b�g <%s>[CH=%d, Block=%d][%d]", pCls->GetMyName(), nCh, block, val); 
	pCls->AddQue_SetLightValue(nCh, block-1, val);
}
//�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P
// �u���b�N�z�� �S��
//�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q
void CKT_LEDMGRDlg::OnBnClickedDebagLedHoseiAll()
{
	if ( BST_CHECKED != CWnd::IsDlgButtonChecked(IDC_DEBUG_MANUAL) ) return;
	//// ���ݑI������Ă���C���X�^���X���擾����
	int nIndex = m_cmbSession.GetCurSel();
	if( -1 == nIndex) return;					// ini�t�@�C���̐ݒ薳��
	LedManager* pCls = mcls_MI.gcls_pLedMgr[nIndex];
	if( NULL == pCls ) return;

	int nCh = m_cmbDebagCh.GetCurSel();
	if( -1 == nCh) return;

	int val;
	val = GetDlgItemInt(IDC_SET_HOSEI_VAL);	// �ُ�l���͂ł� 0 ���A�邩�� ���ُ͈�̃`�F�b�N�͂��Ȃ���
	if( 0 > val || val >= 256 ) return;

	LOG(em_WAR), "[Debag] ���ʃZ�b�g <%s>[CH=%d][%d]", pCls->GetMyName(), nCh, val); 
	pCls->AddQue_SetLightValue(nCh, val);
}
