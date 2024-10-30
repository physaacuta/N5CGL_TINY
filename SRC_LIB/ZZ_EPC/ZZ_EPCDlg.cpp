// ZZ_EPCDlg.cpp : �����t�@�C��
//

#include "stdafx.h"
#include "ZZ_EPC.h"
#include "ZZ_EPCDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CZZ_EPCDlg �_�C�A���O




CZZ_EPCDlg::CZZ_EPCDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CZZ_EPCDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CZZ_EPCDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, m_ListBox);
	DDX_Control(pDX, IDC_CAM_ID, m_cmbCamId);
	DDX_Control(pDX, IDC_SHRINKRATIO, m_cmbShrinkRatio);
}

BEGIN_MESSAGE_MAP(CZZ_EPCDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDOK, &CZZ_EPCDlg::OnBnClickedOk)
	ON_LBN_DBLCLK(IDC_LIST1, &CZZ_EPCDlg::OnLbnDblclkList1)
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDC_UDPRECV, &CZZ_EPCDlg::OnBnClickedUdprecv)
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_CHANGE_BRIGHTNESS, &CZZ_EPCDlg::OnBnClickedChangeBrightness)
	ON_BN_CLICKED(IDC_CHANGE_CAMGAIN, &CZZ_EPCDlg::OnBnClickedChangeCamgain)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_CHANGE_EDGE, &CZZ_EPCDlg::OnBnClickedChangeEdge)
	ON_BN_CLICKED(IDC_PUTEDGE, &CZZ_EPCDlg::OnBnClickedPutedge)
	ON_BN_CLICKED(IDC_CNG_KIZU1, &CZZ_EPCDlg::OnBnClickedCngKizu1)
	ON_BN_CLICKED(IDC_STOP_KIZU1, &CZZ_EPCDlg::OnBnClickedStopKizu1)
	ON_BN_CLICKED(IDC_CNG_KIZU2, &CZZ_EPCDlg::OnBnClickedCngKizu2)
	ON_BN_CLICKED(IDC_CNG_KIZU3, &CZZ_EPCDlg::OnBnClickedCngKizu3)
	ON_BN_CLICKED(IDC_CNG_KIZU4, &CZZ_EPCDlg::OnBnClickedCngKizu4)
	ON_BN_CLICKED(IDC_CNG_KIZU5, &CZZ_EPCDlg::OnBnClickedCngKizu5)
	ON_BN_CLICKED(IDC_STOP_KIZU2, &CZZ_EPCDlg::OnBnClickedStopKizu2)
	ON_BN_CLICKED(IDC_STOP_KIZU3, &CZZ_EPCDlg::OnBnClickedStopKizu3)
	ON_BN_CLICKED(IDC_STOP_KIZU4, &CZZ_EPCDlg::OnBnClickedStopKizu4)
	ON_BN_CLICKED(IDC_STOP_KIZU5, &CZZ_EPCDlg::OnBnClickedStopKizu5)
	ON_BN_CLICKED(IDC_UPDATE, &CZZ_EPCDlg::OnBnClickedUpdate)
	ON_BN_CLICKED(IDC_SKIP, &CZZ_EPCDlg::OnBnClickedSkip)
	ON_BN_CLICKED(IDC_CHECK2, &CZZ_EPCDlg::OnBnClickedCheck2)
	ON_BN_CLICKED(IDC_CHANGE_CAM_GAIN, &CZZ_EPCDlg::OnBnClickedChangeCamGain)
	ON_BN_CLICKED(IDC_CHANGE_CAM_EXP, &CZZ_EPCDlg::OnBnClickedChangeCamExp)
	ON_BN_CLICKED(IDC_CHANGE_CAM_VOLT, &CZZ_EPCDlg::OnBnClickedChangeCamVolt)
	ON_BN_CLICKED(IDC_CHANGE_CAM_TEMP, &CZZ_EPCDlg::OnBnClickedChangeCamTemp)
	ON_CBN_SELCHANGE(IDC_CAM_ID, &CZZ_EPCDlg::OnCbnSelchangeCamId)
	ON_BN_CLICKED(IDC_USE_SMEM, &CZZ_EPCDlg::OnBnClickedUseSmem)
	ON_CBN_SELCHANGE(IDC_SHRINKRATIO, &CZZ_EPCDlg::OnCbnSelchangeShrinkratio)
END_MESSAGE_MAP()


// CZZ_EPCDlg ���b�Z�[�W �n���h��

BOOL CZZ_EPCDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// ���̃_�C�A���O�̃A�C�R����ݒ肵�܂��B�A�v���P�[�V�����̃��C�� �E�B���h�E���_�C�A���O�łȂ��ꍇ�A
	//  Framework �́A���̐ݒ�������I�ɍs���܂��B
	SetIcon(m_hIcon, TRUE);			// �傫���A�C�R���̐ݒ�
	SetIcon(m_hIcon, FALSE);		// �������A�C�R���̐ݒ�

	// TODO: �������������ɒǉ����܂��B
// ------------------------------->>>
	// �~�{�^������
	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL) pSysMenu->EnableMenuItem(SC_CLOSE, MF_GRAYED);
	// ���C���X���b�h�J�n
	InitProcess();
// <<<-------------------------------
	return TRUE;  // �t�H�[�J�X���R���g���[���ɐݒ肵���ꍇ�������ATRUE ��Ԃ��܂��B
}

// �_�C�A���O�ɍŏ����{�^����ǉ�����ꍇ�A�A�C�R����`�悷�邽�߂�
//  ���̃R�[�h���K�v�ł��B�h�L�������g/�r���[ ���f�����g�� MFC �A�v���P�[�V�����̏ꍇ�A
//  ����́AFramework �ɂ���Ď����I�ɐݒ肳��܂��B

void CZZ_EPCDlg::OnPaint()
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
HCURSOR CZZ_EPCDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}







// ����ȍ~ �蓮�ǉ� -------------------->>>

void CZZ_EPCDlg::OnDestroy()
{
	ExitProcess();

	CDialog::OnDestroy();

	// TODO: �����Ƀ��b�Z�[�W �n���h�� �R�[�h��ǉ����܂��B
}

//�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P
// �~�{�^��
//�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q
void CZZ_EPCDlg::OnClose()
{
	//CDialog::OnClose();
}
//�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P
// ����{�^��
//�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q
void CZZ_EPCDlg::OnBnClickedOk()
{
	//ExitProcess();
	// ��ʂ̏I���v��
	PostMessage(WM_QUIT);
}

//�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P
// �������N��
//�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q
void CZZ_EPCDlg::OnLbnDblclkList1()
{
	// ListBox�R���g���[���̃v���p�e�B�́A�\�[�g�֎~�ɂ��Ă�����
	// ListBox�R���g���[���̃_�u���N���b�N�C�x���g��ǉ�
	mcls_pLog->ShowEdit();
}

//------------------------------------------
// �e�v���Z�X����������
//------------------------------------------
void CZZ_EPCDlg::InitProcess()
{
	// �^�X�N����������
	if( 0 != task_init("ZZ_EPC", 0) ) _ASSERT(0);

	// LogFileManager�C���X�^���X���� (����ȍ~ ���O�o�͉\)
	setlocale(LC_ALL, "Japanese");	// ���ꂪ�����Ɠ��{��p�X���܂ރt�H���_�̏ꍇ�A�ۑ��ł��Ȃ��B(���C�u�����̒��Ŏ��s����ׂ��������Ă��邪�A�ύX����ƃ^�X�N�S�̂ɉe������̂ŁA�Ƃ肠���������Őݒ�)
	mcls_pLog = new LogFileManager("ZZ_EPC", &m_ListBox);
	LOG(em_MSG), "�^�X�N�N��");
	LOG(em_MSG), "�e�v���Z�X�J�n���� ���s���E�E�E");

	// ���C���C���X�^���X �J�n����
	mcls_MI.SetLogMgr(mcls_pLog);
	mcls_MI.Init();


	//// ��ʍX�V�p�f�[�^�Z�b�g
	for(int ii=0;ii<8;ii++) {
		CString strWk;
		strWk.Format("%d" ,ii+1);
		m_cmbCamId.AddString(strWk);
	}
	m_cmbCamId.SetCurSel(0);

	//// ��ʍX�V�p�^�C�}����
	SetTimer(1, 500, NULL);

	((CButton *)GetDlgItem(IDC_USE_SMEM))->SetCheck(true);
	OnCbnSelchangeCamId();

	// �؂�o���摜�̏k����
	CString strWk;
	strWk.Format("%0.2f", mcls_MI.gcls_pData->GetShrinkRatio());
	SetDlgItemText(IDC_SHRINKRATIO, strWk);
	
	OnBnClickedUseSmem();

	m_cmbShrinkRatio.AddString("1");
	m_cmbShrinkRatio.AddString("2");
	m_cmbShrinkRatio.AddString("4");
	m_cmbShrinkRatio.SetCurSel(0);

	// �r���ݒ�̏����lIni���f
	CString sWk;
	for (int ii = 0; ii < 5; ii++) {
		// �r��
		sWk.Format("PUT_KIZU_%d", ii + 1);
		mcls_MI.gcls_pData->g_nPutKizu[ii] = GetPrivateProfileInt("ZZ_EPC", sWk, 0, TASKINI_NAME);
		// �r��
		sWk.Format("PUT_TYPE_%d", ii + 1);
		mcls_MI.gcls_pData->g_nPutType[ii] = GetPrivateProfileInt("ZZ_EPC", sWk, 0, TASKINI_NAME);
		// �O���[�h
		sWk.Format("PUT_GRADE_%d", ii + 1);
		mcls_MI.gcls_pData->g_nPutGrade[ii] = GetPrivateProfileInt("ZZ_EPC", sWk, 0, TASKINI_NAME);
		// �t���[���Ԋu
		sWk.Format("PUT_FRAME_%d", ii + 1);
		mcls_MI.gcls_pData->g_nPutFrm[ii] = GetPrivateProfileInt("ZZ_EPC", sWk, 0, TASKINI_NAME);
		// �t���[���Ԋu
		sWk.Format("PUT_CYCLE_%d", ii + 1);
		mcls_MI.gcls_pData->g_nPutCycle[ii] = GetPrivateProfileInt("ZZ_EPC", sWk, 0, TASKINI_NAME);

	}
	OnBnClickedUpdate();		// �X�V�{�^������

	// ���C���X���b�h�̎��s���s��
	LOG(em_MSG), "�e�v���Z�X�J�n���� ����");
	mcls_MI.Start();	
	return;
}

//�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P
// �e�v���Z�X�J������
//�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q
void CZZ_EPCDlg::ExitProcess()
{
	//// ���O ���X�g�{�b�N�X�\�������I��
	mcls_pLog->SetListOut(false);					// ����ȍ~ ���X�g�{�b�N�X�ւ̃��O�\���֎~

	// ���C���C���X�^���X �I������
	mcls_MI.Stop();
	mcls_MI.Exit();

	//// ���O�N���X�J��
	LOG(em_MSG), "�^�X�N�I��");
	if(NULL != mcls_pLog){
		delete mcls_pLog;
		mcls_pLog = NULL;
	}
	
	// �^�X�N�I������
	KillTimer(1);			// ��ʍX�V�^�C�}
	task_exit();

	// ��ʂ̏I���v��
//	PostMessage(WM_QUIT);
}

//====================================
// UDP��M�������Ƃɂ���
//====================================
void CZZ_EPCDlg::OnBnClickedUdprecv()
{
	DEBUG_SPEED_INFO readInf;
	memset(&readInf, 0x00, sizeof(readInf));

	readInf.framenum = mcls_MI.gcls_pData->m_nFrameNo;
	readInf.vRes = 0.1f;

	mcls_MI.gcls_pData->SetFrameData(&readInf);
}




void CZZ_EPCDlg::OnBnClickedChangeBrightness()
{
	const int nID[4][3] = {	{IDC_BR_CENTER_ANGLE1, IDC_BR_OV_R_ANGLE1, IDC_BR_OV_L_ANGLE1},
							{IDC_BR_CENTER_ANGLE2, IDC_BR_OV_R_ANGLE2, IDC_BR_OV_L_ANGLE2},
							{IDC_BR_CENTER_ANGLE3, IDC_BR_OV_R_ANGLE3, IDC_BR_OV_L_ANGLE3},
							{IDC_BR_CENTER_ANGLE4, IDC_BR_OV_R_ANGLE4, IDC_BR_OV_L_ANGLE4}};

	int nBrightness[4][3];
	for(int ii=0; ii<4; ii++){
		for(int jj=0; jj<3; jj++){
			nBrightness[ii][jj] = GetDlgItemInt(nID[ii][jj]);
			if(0 > nBrightness[ii][jj])		nBrightness[ii][jj] = 0;
			if(255 < nBrightness[ii][jj])	nBrightness[ii][jj] = 255;
		}
	}

	mcls_MI.gcls_pData->SetBrightness(&nBrightness[0][0]);

}

void CZZ_EPCDlg::OnBnClickedChangeCamgain()
{
	const int nID[3] = {IDC_GAIN_R, IDC_GAIN_G, IDC_GAIN_B  };
	double    dGain[3];

	for(int ii=0; ii<3; ii++){
		dGain[ii] = (double)GetDlgItemInt(nID[ii]);
		if(1 > dGain[ii])	dGain[ii] = 1;
		if(10 < dGain[ii])	dGain[ii] = 10;
	}

	mcls_MI.gcls_pData->SetCamGain(&dGain[0]);
}

void CZZ_EPCDlg::OnBnClickedChangeEdge()
{
	double    dEdge[2];
	CString	  sWk;

	GetDlgItemText(IDC_EDGE, sWk);
	dEdge[0] = (double)KizuFunction::AtoFex(sWk, strlen(sWk));
	GetDlgItemText(IDC_EDGE3, sWk);
	dEdge[1] = (double)KizuFunction::AtoFex(sWk, strlen(sWk));
	mcls_MI.gcls_pData->SetEdge(0, &dEdge[0]);
	mcls_MI.gcls_pData->SetEdge(1, &dEdge[1]);
}

//====================================
// �G�b�W�ύX��~
//====================================
void CZZ_EPCDlg::OnBnClickedPutedge()
{
	bool bVal = (BST_CHECKED == CWnd::IsDlgButtonChecked(IDC_PUTEDGE) ? true : false );
	mcls_MI.gcls_pData->g_bPutEdge = bVal;
}
//====================================
// ��������ԏo��
//====================================
void CZZ_EPCDlg::OnBnClickedSkip()
{
	bool bVal = (BST_CHECKED == CWnd::IsDlgButtonChecked(IDC_SKIP) ? true : false );
	mcls_MI.gcls_pData->g_bPutSkip = bVal;
}

//====================================
// ��ʍX�V
//====================================
void CZZ_EPCDlg::OnTimer(UINT_PTR nIDEvent)
{
}

//====================================
// �r�o�͐ݒ��ύX
//------------------------------------
// int n : �r��n��(0�I���W��)
//====================================
void CZZ_EPCDlg::ChangeKizuInfo(int n)
{
	// �o�͏������擾����I�u�W�F�N�gID
	int KizuCntArray[5]    = { IDC_KIZUCNT1,   IDC_KIZUCNT2,   IDC_KIZUCNT3,   IDC_KIZUCNT4,   IDC_KIZUCNT5};		// �r��
	int KizuTypeArray[5]   = { IDC_KIZUTYPE1,  IDC_KIZUTYPE2,  IDC_KIZUTYPE3,  IDC_KIZUTYPE4,  IDC_KIZUTYPE5};		// �r��
	int KizuGrArray[5]     = { IDC_KIZUGRADE1, IDC_KIZUGRADE2, IDC_KIZUGRADE3, IDC_KIZUGRADE4, IDC_KIZUGRADE5};		// �r�O���[�h
	int KizuFrmCntArray[5] = { IDC_FRMCNT1,    IDC_FRMCNT2,    IDC_FRMCNT3,    IDC_FRMCNT4,    IDC_FRMCNT5};		// �t���[���Ԋu
	int KizuCycleArray[5]  = { IDC_CYCLE1,     IDC_CYCLE2,     IDC_CYCLE3,     IDC_CYCLE4,     IDC_CYCLE5};			// ����
	int nWk;

	// �r��
	nWk = GetDlgItemInt(KizuCntArray[n]);
	if(0 > nWk) nWk = 0;
	if(10 < nWk) nWk = 10;
	mcls_MI.gcls_pData->g_nPutKizu[n] = nWk;

	// �r��
	nWk = GetDlgItemInt(KizuTypeArray[n]);
	if(0 > nWk) nWk = 1;
	mcls_MI.gcls_pData->g_nPutType[n] = nWk;

	// �rGr
	nWk = GetDlgItemInt(KizuGrArray[n]);
	if(0 > nWk) nWk = 1;
	mcls_MI.gcls_pData->g_nPutGrade[n] = nWk;

	// �t���[���Ԋu
	nWk = GetDlgItemInt(KizuFrmCntArray[n]);
	if(0 > nWk) nWk = 0;
	mcls_MI.gcls_pData->g_nPutFrm[n] = nWk;

	// ����
	nWk = GetDlgItemInt(KizuCycleArray[n]);
	if(0 > nWk) nWk = 0;
	mcls_MI.gcls_pData->g_nPutCycle[n] = nWk;

	// �����W
	nWk = GetDlgItemInt(IDC_POS_X);
	if (0 > nWk) nWk = 1;
	mcls_MI.gcls_pData->g_nPosX = nWk;
}

//====================================
// �r�o�͂��~/�J�n
//------------------------------------
// int n : �r��n��(0�I���W��)
//====================================
void CZZ_EPCDlg::StopKizuOutPut(int n)
{
	int StopKizuArray[5]    = { IDC_STOP_KIZU1, IDC_STOP_KIZU2, IDC_STOP_KIZU3, IDC_STOP_KIZU4, IDC_STOP_KIZU5};

	bool bVal = (BST_CHECKED == CWnd::IsDlgButtonChecked(StopKizuArray[n]) ? false : true );
	mcls_MI.gcls_pData->g_bPutKizu[n] = bVal;
}

//====================================
// �r�o�͏��� ��ʕ\�����e���X�V
//====================================
void CZZ_EPCDlg::OnBnClickedUpdate()
{
	int KizuCntArray[5]    = { IDC_KIZUCNT1,   IDC_KIZUCNT2,   IDC_KIZUCNT3,   IDC_KIZUCNT4,   IDC_KIZUCNT5};		// �r��
	int KizuTypeArray[5]   = { IDC_KIZUTYPE1,  IDC_KIZUTYPE2,  IDC_KIZUTYPE3,  IDC_KIZUTYPE4,  IDC_KIZUTYPE5};		// �r��
	int KizuGrArray[5]     = { IDC_KIZUGRADE1, IDC_KIZUGRADE2, IDC_KIZUGRADE3, IDC_KIZUGRADE4, IDC_KIZUGRADE5};		// �r�O���[�h
	int KizuFrmCntArray[5] = { IDC_FRMCNT1,    IDC_FRMCNT2,    IDC_FRMCNT3,    IDC_FRMCNT4,    IDC_FRMCNT5};		// �t���[���Ԋu
	int KizuCycleArray[5]  = { IDC_CYCLE1,     IDC_CYCLE2,     IDC_CYCLE3,     IDC_CYCLE4,     IDC_CYCLE5};			// ����
	int StopKizuArray[5]    = { IDC_STOP_KIZU1, IDC_STOP_KIZU2, IDC_STOP_KIZU3, IDC_STOP_KIZU4, IDC_STOP_KIZU5};	// �o�͒�/��~��

	for(int ii=0; ii<5; ii++){
		SetDlgItemInt(KizuCntArray[ii],    mcls_MI.gcls_pData->g_nPutKizu[ii]);		// �r��
		SetDlgItemInt(KizuTypeArray[ii],   mcls_MI.gcls_pData->g_nPutType[ii]);		// �r��
		SetDlgItemInt(KizuGrArray[ii],     mcls_MI.gcls_pData->g_nPutGrade[ii]);	// �r�O���[�h
		SetDlgItemInt(KizuFrmCntArray[ii], mcls_MI.gcls_pData->g_nPutFrm[ii]);		// �t���[���Ԋu
		SetDlgItemInt(KizuCycleArray[ii],  mcls_MI.gcls_pData->g_nPutCycle[ii]);	// ����
	}
}



void CZZ_EPCDlg::OnBnClickedCheck2()
{
	bool bVal = ((BST_CHECKED == CWnd::IsDlgButtonChecked(IDC_CHECK2)) ? true : false);
	LOG(em_MSG), "�����t���O�ύX(%d)", bVal);
	mcls_MI.gcls_pData->SetRandomParam(bVal);
	return;
}


void CZZ_EPCDlg::OnBnClickedChangeCamGain()
{
	int cam = m_cmbCamId.GetCurSel();
	int id[3] = {IDC_CAM_GAIN_1, IDC_CAM_GAIN_2, IDC_CAM_GAIN_3};
	float value[3];
	CString sWk;

	for (int ii = 0; ii < 3; ii++)
	{
		GetDlgItemText(id[ii], sWk);
		value[ii] = KizuFunction::AtoFex(sWk, strlen(sWk));
	}

	LOG(em_MSG), "GAIN %.2f, %.2f, %.2f", value[0], value[1], value[2] );
	mcls_MI.gcls_pCommand->SetGain(cam, value);

	OnBnClickedUseSmem();
}


void CZZ_EPCDlg::OnBnClickedChangeCamExp()
{
	int cam = m_cmbCamId.GetCurSel();
	int id[3] = { IDC_CAM_EXP_1, IDC_CAM_EXP_2, IDC_CAM_EXP_3 };
	float value[3];
	for (int ii = 0; ii < 3; ii++) {
		value[ii] = (float)GetDlgItemInt(id[ii]);
	}

	LOG(em_MSG), "EXP %lf, %lf, %lf", value[0], value[1], value[2] );
	mcls_MI.gcls_pCommand->SetExp(cam, value);

	OnBnClickedUseSmem();
}


void CZZ_EPCDlg::OnBnClickedChangeCamVolt()
{
	int cam = m_cmbCamId.GetCurSel();
	float value[NCL_ncam_max];
	value[cam] = (float)GetDlgItemInt(IDC_CAM_VOLT_1);

	LOG(em_MSG), "VOLT %lf", value[cam] );
	mcls_MI.gcls_pCommand->SetVolt(cam, value);

	OnBnClickedUseSmem();
}


void CZZ_EPCDlg::OnBnClickedChangeCamTemp()
{
	int cam = m_cmbCamId.GetCurSel();
	float value[NCL_ncam_max];
	value[cam] = (float)GetDlgItemInt(IDC_CAM_TEMP_1);

	LOG(em_MSG), "TEMP %lf", value[cam] );
	mcls_MI.gcls_pCommand->SetTemp(cam, value);

	OnBnClickedUseSmem();
}


void CZZ_EPCDlg::OnCbnSelchangeCamId()
{
	int cam = m_cmbCamId.GetCurSel();

	int Gain[3] = { IDC_CAM_GAIN_1, IDC_CAM_GAIN_2, IDC_CAM_GAIN_3 };
	int Exp[3] = { IDC_CAM_EXP_1, IDC_CAM_EXP_2, IDC_CAM_EXP_3 };
	int Volt[1] = { IDC_CAM_VOLT_1 };
	int Temp[1] = { IDC_CAM_TEMP_1 };
	CString sWk;

	for (int jj = 0; jj < 3; jj++)
	{
		sWk.Format("%0.2f", mcls_MI.gcls_pCommand->GetGain(jj, cam));
		SetDlgItemText(Gain[jj], sWk);
		SetDlgItemInt(Exp[jj], (UINT)mcls_MI.gcls_pCommand->GetExp(jj, cam));
	};
	SetDlgItemInt(Volt[0], (UINT)mcls_MI.gcls_pCommand->GetVolt(cam));
	SetDlgItemInt(Temp[0], (UINT)mcls_MI.gcls_pCommand->GetTemp(cam));
}


void CZZ_EPCDlg::OnBnClickedUseSmem()
{
	bool bSHMemSet = ((BST_CHECKED == CWnd::IsDlgButtonChecked(IDC_USE_SMEM)) ? true : false);
	LOG(em_MSG), "���L�������ւ̃Z�b�g�F%s", (bSHMemSet ? "�ݒ�" : "����"));

	FIRST_SHMEM_INFOv1 info;
	memset(&info, 0, sizeof(FIRST_SHMEM_INFOv1));

	if (bSHMemSet)
	{
		memcpy(&info.first_smem_req_id, &ID_FIRST_SHMEM_INFO_IDv1, sizeof(GUID));
		mcls_MI.gcls_pData->GetCamset((int *)&info.camnum, &info.camid[0]);
		for (DWORD ii = 0; ii < info.camnum; ii++)
		{
			for (int jj = 0; jj < 3; jj++)
			{
				info.gain[ii][jj] = mcls_MI.gcls_pCommand->GetGain(jj, ii);
				info.exptime[ii][jj] = mcls_MI.gcls_pCommand->GetExp(jj, ii);
			}
			info.volt[ii] = mcls_MI.gcls_pCommand->GetVolt(ii);
			info.temp[ii] = mcls_MI.gcls_pCommand->GetTemp(ii);

			LOG(em_MSG), " �J������� [ID:%d][���x:%.3f][�I��:%.3f %.3f %.3f][�Q�C��:%.3f %.3f %.3f][�d��:%.3f]"
				, info.camid[ii]
				, info.temp[ii]
				, info.exptime[ii][0]
				, info.exptime[ii][1]
				, info.exptime[ii][2]
				, info.gain[ii][0]
				, info.gain[ii][1]
				, info.gain[ii][2]
				, info.volt[ii]
				);
		}
	}

	mcls_MI.gcls_pData->SetFirstSHMem(bSHMemSet, &info);
}


void CZZ_EPCDlg::OnCbnSelchangeShrinkratio()
{
	float fShrinkRatio[] = {1.0, 2.0, 4.0};
	mcls_MI.gcls_pData->SetShrinkRatio(fShrinkRatio[m_cmbShrinkRatio.GetCurSel()]);
}
