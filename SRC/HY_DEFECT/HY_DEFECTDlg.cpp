
// HY_DEFECTDlg.cpp : �����t�@�C��
//

#include "stdafx.h"
#include "HY_DEFECT.h"
#include "HY_DEFECTDlg.h"
#include "afxdialogex.h"

// �������[���[�N���o �蓮�ǉ� ----->>>
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif
// <<<------------------------


// CHY_DEFECTDlg �_�C�A���O



CHY_DEFECTDlg::CHY_DEFECTDlg(CWnd* pParent /*=NULL*/)
	: CDialog(IDD_HY_DEFECT_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CHY_DEFECTDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, m_ListBox);
	DDX_Control(pDX, IDC_LIST_ST, m_ListSt);
	DDX_Control(pDX, IDC_LIST_REC, m_ListRec);
	DDX_Control(pDX, IDC_LIST_PTR, m_ListPtr);
}

BEGIN_MESSAGE_MAP(CHY_DEFECTDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDOK, &CHY_DEFECTDlg::OnBnClickedOk)
	ON_WM_CLOSE()
	ON_LBN_DBLCLK(IDC_LIST1, &CHY_DEFECTDlg::OnLbnDblclkList1)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_DEBUG_PARAM, &CHY_DEFECTDlg::OnBnClickedDebugParam)
	ON_BN_CLICKED(IDC_MEM_OUTPUT, &CHY_DEFECTDlg::OnBnClickedMemOutput)
	ON_BN_CLICKED(IDC_MEM_INPUT, &CHY_DEFECTDlg::OnBnClickedMemInput)
// 2024.07.26 Kouno 2CGL�������u�Ӄ֐��x����@�\����(������) ---------------->>>>>
	ON_BN_CLICKED(IDC_DEBUG_LONGPD, &CHY_DEFECTDlg::OnBnClickedDebugLongpd)
// 2024.07.26 Kouno 2CGL�������u�Ӄ֐��x����@�\����(������) <<<<<----------------
END_MESSAGE_MAP()


// CHY_DEFECTDlg ���b�Z�[�W �n���h���[

BOOL CHY_DEFECTDlg::OnInitDialog()
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

void CHY_DEFECTDlg::OnPaint()
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
HCURSOR CHY_DEFECTDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

// ����ȍ~ �蓮�ǉ� -------------------->>>

//�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P
// �~�{�^��
//�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q
void CHY_DEFECTDlg::OnClose()
{
	//CDialog::OnClose();
}
//�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P
// �����I���{�^��
//�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q
void CHY_DEFECTDlg::OnBnClickedOk()
{
	if (IDYES == MessageBox("�I�����܂����H", "�d�v�Ȃ��m�点", MB_YESNO | MB_DEFBUTTON2 | MB_ICONQUESTION)) {
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
void CHY_DEFECTDlg::OnLbnDblclkList1()
{
	// ListBox�R���g���[���̃v���p�e�B�́A�\�[�g�֎~�ɂ��Ă�����
	// ListBox�R���g���[���̃_�u���N���b�N�C�x���g��ǉ�
	mcls_pLog->ShowEdit();
}

//------------------------------------------
// �e�v���Z�X����������
//------------------------------------------
void CHY_DEFECTDlg::InitProcess()
{
	CString ss;

	//// �^�X�N����������
	if (0 != task_init(HY_DEFECT)) {
		syslog(SYSNO_TASK_INIT_ERR, "[%s]", HY_DEFECT);
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

	//-----------------------
	// �X�e�[�^�X��� ���X�g�r���[�C���X�^���X���� & ��`
	if (true) {
		mcnt_pListSt = new DspListView(&m_ListSt);
		// �����ڒ�`
		m_ListSt.SetExtendedStyle(m_ListSt.GetExtendedStyle() | LVS_EX_GRIDLINES);			// �r���`��
		m_ListSt.SetExtendedStyle(m_ListSt.GetExtendedStyle() | LVS_EX_FULLROWSELECT);		// 1�s�S�̑I��

																							// ��ǉ�			0		1			
		char name[][32] = { "����",	"���" };
		int  wid[] = { 120, 	120 };
		mcnt_pListSt->SetColumnAll(sizeof(wid) / sizeof(int), 32, wid, (char const*)name);
		// �s�ǉ�
		mcnt_pListSt->SetRowAll(LIST_ST_ROWMAX);
		// �Œ�l�Z�b�g
		mcnt_pListSt->SetItem(EM_ROW_STATUS, 0, "�X�e�[�^�X");
		mcnt_pListSt->SetItem(EM_ROW_INTERVAL, 0, "�Ԋu[m]");
		mcnt_pListSt->SetItem(EM_ROW_RECV_QUE, 0, "��MQue(Max)");
		mcnt_pListSt->SetItem(EM_ROW_IMG_QUE, 0, "�摜�D��Que");
		mcnt_pListSt->SetItem(EM_ROW_IMG_LOW_QUE, 0, "�摜���Que");
	}

	//-----------------------
	// �|�C���^��� ���X�g�r���[�C���X�^���X���� & ��`
	if (true) {
		mcnt_pListPtr = new DspListView(&m_ListPtr);
		// �����ڒ�`
		m_ListPtr.SetExtendedStyle(m_ListPtr.GetExtendedStyle() | LVS_EX_GRIDLINES);			// �r���`��
		m_ListPtr.SetExtendedStyle(m_ListPtr.GetExtendedStyle() | LVS_EX_FULLROWSELECT);		// 1�s�S�̑I��

		char name[][32] = { "����",	"���" };
		int  wid[] = { 160, 	80 };
		mcnt_pListPtr->SetColumnAll(sizeof(wid) / sizeof(int), 32, wid, (char const*)name);
		// �s�ǉ�
		mcnt_pListPtr->SetRowAll(LIST_PTR_ROWMAX);
		// �Œ�l�Z�b�g
		mcnt_pListPtr->SetItem(EM_ROW_REFRESH, 0, "�X�V���t���O");
		mcnt_pListPtr->SetItem(EM_ROW_REC_WRITE_BASE, 0, "������ں���");
		mcnt_pListPtr->SetItem(EM_ROW_REC_WRITE_TOP, 0, "�������u�ʒu�ł̌���ں���");
		mcnt_pListPtr->SetItem(EM_ROW_REC_WRITE_BOT, 0, "�������u�ʒu�ł̌���ں���");
		mcnt_pListPtr->SetItem(EM_ROW_KEN_TOP, 0, "�J�����ʒu");
		mcnt_pListPtr->SetItem(EM_ROW_KEN_BOT, 0, "�J�����ʒu");
		mcnt_pListPtr->SetItem(EM_ROW_DSP_TOP, 0, "�\����_");
		mcnt_pListPtr->SetItem(EM_ROW_DSP_BOT, 0, "�\����_");
	}

	//-----------------------
	// ���R�[�h��� ���X�g�r���[�C���X�^���X���� & ��`
	if (true) {
		mcnt_pListRec = new DspListView(&m_ListRec);
		// �����ڒ�`
		m_ListRec.SetExtendedStyle(m_ListRec.GetExtendedStyle() | LVS_EX_GRIDLINES);			// �r���`��
		m_ListRec.SetExtendedStyle(m_ListRec.GetExtendedStyle() | LVS_EX_FULLROWSELECT);		// 1�s�S�̑I��

		char name[LIST_REC_COLMAX][64];
		int	wid[LIST_REC_COLMAX];

		memset(name, 0, sizeof(name));
		memset(wid, 0, sizeof(wid));

		strcpy(name[EM_COL_NO], "ں���");	wid[EM_COL_NO] = 30;
		strcpy(name[EM_COL_KANRINO], "�Ǘ�No");	wid[EM_COL_KANRINO] = 100;
		strcpy(name[EM_COL_DSP_STATUS], "�\�����");	wid[EM_COL_DSP_STATUS] = 60;
		strcpy(name[EM_COL_KIZU_NUM], "�r��");	wid[EM_COL_KIZU_NUM] = 40;
		strcpy(name[EM_COL_KEN_TOP], "�J�����ʒu(�\��) �ʔ���(���ݕ\���s)");	wid[EM_COL_KEN_TOP] = 108;
		strcpy(name[EM_COL_KEN_BOT], "�J�����ʒu(����) �ʔ���(���ݕ\���s)");	wid[EM_COL_KEN_BOT] = 108;
		strcpy(name[EM_COL_DSP_TOP], "�\����_(�\��) �ʔ���(���ݕ\���s)");	wid[EM_COL_DSP_TOP] = 108;
		strcpy(name[EM_COL_DSP_BOT], "�\����_(����) �ʔ���(���ݕ\���s)");	wid[EM_COL_DSP_BOT] = 108;
		strcpy(name[EM_COL_ROW_WRITE_TOP], "���ݏ����s");	wid[EM_COL_ROW_WRITE_TOP] = 76;
		strcpy(name[EM_COL_ROW_WRITE_BOT], "���ݏ����s");	wid[EM_COL_ROW_WRITE_BOT] = 76;
		strcpy(name[EM_COL_ROW_LAST], "���������s");	wid[EM_COL_ROW_LAST] = 80;
// 2024.07.26 Kouno 2CGL�������u�Ӄ֐��x����@�\����(������) ---------------->>>>>
		strcpy(name[EM_COL_LONGPD_FLAG], "������");	wid[EM_COL_LONGPD_FLAG] = 60;
// 2024.07.26 Kouno 2CGL�������u�Ӄ֐��x����@�\����(������) <<<<<----------------
		mcnt_pListRec->SetColumnAll(sizeof(wid) / sizeof(int), 64, wid, (char const*)name);

		// �s�ǉ�
		mcnt_pListRec->SetRowAll(TBL_HY_REC_MAX);
		for (int ii = 0; ii<TBL_HY_REC_MAX; ii++) mcnt_pListRec->SetItem(ii, EM_COL_NO, ii + 1);
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
void CHY_DEFECTDlg::ExitProcess()
{
	//// ���C���C���X�^���X �I������
	KillTimer(1);			// ��ʍX�V�^�C�}
	mcls_MI.Stop();
	mcls_MI.Exit();

	//// ���O ���X�g�{�b�N�X�\�������I��
	mcls_pLog->SetListOut(false);					// ����ȍ~ ���X�g�{�b�N�X�ւ̃��O�\���֎~

													//// ���O�N���X�J��
	LOG(em_MSG), "[Dlg] �^�X�N�I��");
	if (NULL != mcls_pLog) {
		delete mcls_pLog;
		mcls_pLog = NULL;
	}

	// �^�X�N�I������
	task_exit();

	if (mcnt_pListSt) delete(mcnt_pListSt);
	if (mcnt_pListPtr) delete(mcnt_pListPtr);
	if (mcnt_pListRec) delete(mcnt_pListRec);

	// ��ʂ̏I���v��
	PostMessage(WM_QUIT);
}

//------------------------------------------
// ���C���X���b�h DlgThread
// LPVOID param �_�C�����O�|�C���^
//------------------------------------------
unsigned WINAPI CHY_DEFECTDlg::MainThread(LPVOID param)
{
	CHY_DEFECTDlg* p = (CHY_DEFECTDlg *)param;
	COMMON_QUE que;			// ��M�\����

							//// �e�v���Z�X���������� (����ȑO�̓��O�o�͋֎~)
	p->InitProcess();

	//// �N���v����荞�� ���[�v�J�n
	while (true) {
		recv_mail(&que);			// �N���v����荞��

		switch (que.nEventNo) {

			//-----------------------------------------------------------------------------------------------
		case FACT_HY_DEFECT_01:		// �p�����[�^�ύX�ʒm
			pLOG(em_MSG), "[Mail] �p�����[�^�ύX�ʒm (FACT_HY_DEFECT_01)");
			// ���C���C���X�^���X�֒ʒm
			p->mcls_MI.SetDeliveryMail(E_DEF_ML_PARAM, &que);
			break;

			//-----------------------------------------------------------------------------------------------
		case FACT_HY_DEFECT_02:		// �C�j�V�����ʒm
			pLOG(em_MSG), "[Mail] �C�j�V�����ʒm (FACT_HY_DEFECT_02)");
			// ���C���C���X�^���X�֒ʒm
			p->mcls_MI.SetDeliveryMail(E_DEF_ML_INIT, &que);
			break;

			//-----------------------------------------------------------------------------------------------
		case FACT_HY_DEFECT_03:		// �O�H���r���Ǎ��ʒm (��TO_DEFECT)
			pLOG(em_MSG), "[Mail] �O�H���r���Ǎ��ʒm (FACT_HY_DEFECT_03) [%s]", que.fc.data);
			// ���C���C���X�^���X�֒ʒm
			p->mcls_MI.SetDeliveryMail(E_DEF_ML_MAE_READ, &que);
			break;

// 2023.11.17 �ŏI�H���������u���@�\�����̂S -------------------->>>>>
		case FACT_HY_DEFECT_04:		// �O�H���r���Ǎ������ʒm (��HY_DEFECT)
			pLOG(em_MSG), "[Mail] �O�H���r���Ǎ������ʒm (FACT_HY_DEFECT_04) [Rec=%d]", que.fl.data[0]);
			// ���C���C���X�^���X�֒ʒm
			p->mcls_MI.SetDeliveryMail(E_DEF_ML_MAE_READ_END, &que);
			break;
// 2023.11.17 �ŏI�H���������u���@�\�����̂S <<<<<--------------------

// 2024.07.26 Kouno 2CGL�������u�Ӄ֐��x����@�\����(������) ---------------->>>>>
		case FACT_HY_DEFECT_05:		// ���������ה����ʒm (��TO_DEFECT)
			pLOG(em_MSG), "[Mail] ���������ה����ʒm (FACT_HY_DEFECT_05)");
			// ���C���C���X�^���X�֒ʒm
			p->mcls_MI.SetDeliveryMail(E_DEF_ML_LONGPD_OCCUR, &que);
			break;
// 2024.07.26 Kouno 2CGL�������u�Ӄ֐��x����@�\����(������) <<<<<----------------

			//-----------------------------------------------------------------------------------------------
		case FACT_KS_PCNAME_CHANGE:								// PC���̕ύX�ʒm (��KS_SLAVE)
			break;

		case FACT_KS_END:									// �I���v��
															// �e�v���Z�X�I������
			p->ExitProcess();
			return 0;

		default:
			pLOG(em_MSG), "[Mail] �C�x���g�R�[�h�s��[EventNo=%d]", que.nEventNo);
			syslog(SYSNO_MAILNO_ERR, "[EventNo=%d]", que.nEventNo);
			break;
		}
	}

	return 0;
}

//------------------------------------------
// �\�����
//------------------------------------------
CString CHY_DEFECTDlg::GetName_DspStatus(DIV_DSP_STATUS em)
{
	CString s;
	if (DIV_DSP_NG == em)				s = "�\���s��";
	else if (DIV_DSP_INITIALIZE == em)	s = "�|";
	else if (DIV_DSP_STANDBY == em)		s = "�\������";
	else if (DIV_DSP_OK == em)			s = "�\����";
	else								s.Format("�ް��ُ�=%d", em);
	return s;
}


//------------------------------------------
// �z��P�ʂ̃t�H�[�}�b�g
//------------------------------------------
CString CHY_DEFECTDlg::GetArrayFormat(int const* p, char* sprit, int nCnt)
{
	CString cWk;
	cWk.Format("%d", p[0]);
	for (int ii = 1; ii<nCnt; ii++) {
		cWk.AppendFormat("%s%d", sprit, p[ii]);
	}
	return cWk;
}


//�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P
// ��ʍX�V�^�C�}
//�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q
void CHY_DEFECTDlg::OnTimer(UINT_PTR nIDEvent)
{
	CString ss;
	CString sWk;

	//======================================================
	// �X�e�[�^�X���
	mcnt_pListSt->SetItem(EM_ROW_STATUS, 1, mcls_MI.gcls_pRecv->GetSockStateName());
	mcnt_pListSt->SetItem(EM_ROW_INTERVAL, 1, GetArrayFormat(mcls_MI.gcls_pParam->GetCommonMast().nMapDsp, "/", MAP_STAGE_NUM));
	ss.Format("%d(%d)", mcls_MI.gque_OpRecv.GetCount(), mcls_MI.gque_OpRecv.GetMaxCount());
	mcnt_pListSt->SetItem(EM_ROW_RECV_QUE, 1, ss);
	ss.Format("%d(%d)", mcls_MI.gque_ImgSave.GetCount(), mcls_MI.gque_ImgSave.GetMaxCount());
	mcnt_pListSt->SetItem(EM_ROW_IMG_QUE, 1, ss);
	ss.Format("%d(%d)", mcls_MI.gque_ImgSaveLow.GetCount(), mcls_MI.gque_ImgSaveLow.GetMaxCount());
	mcnt_pListSt->SetItem(EM_ROW_IMG_LOW_QUE, 1, ss);


	//======================================================
	// �|�C���^���
	mcnt_pListPtr->SetItem(EM_ROW_REFRESH, 1, TRUE == mcls_MI.gcls_pData->GetPointTbl()->nRefresh ? "�~" : "");
	mcnt_pListPtr->SetItem(EM_ROW_REC_WRITE_BASE, 1, mcls_MI.gcls_pData->GetPointTbl()->nRecWriteBase);
	mcnt_pListPtr->SetItem(EM_ROW_REC_WRITE_TOP, 1, GetArrayFormat((int const*)mcls_MI.gcls_pData->GetPointTbl()->nRecWrite, "/", NUM_MEN));
	mcnt_pListPtr->SetItem(EM_ROW_REC_WRITE_BOT, 1, GetArrayFormat((int const*)mcls_MI.gcls_pData->GetPointTbl()->nRecWrite, "/", NUM_MEN));
	mcnt_pListPtr->SetItem(EM_ROW_KEN_TOP, 1, mcls_MI.gcls_pData->GetPointTbl()->nRecDisp[DIV_LEN_POS_KEN_TOP]);
	mcnt_pListPtr->SetItem(EM_ROW_KEN_BOT, 1, mcls_MI.gcls_pData->GetPointTbl()->nRecDisp[DIV_LEN_POS_KEN_BOT]);
	mcnt_pListPtr->SetItem(EM_ROW_DSP_TOP, 1, mcls_MI.gcls_pData->GetPointTbl()->nRecDisp[DIV_LEN_POS_DSP_TOP]);
	mcnt_pListPtr->SetItem(EM_ROW_DSP_BOT, 1, mcls_MI.gcls_pData->GetPointTbl()->nRecDisp[DIV_LEN_POS_DSP_BOT]);

	//======================================================
	// ���R�[�h���
	for (int ii = 0; ii<TBL_HY_REC_MAX; ii++) {

		POINT_TBL pPtr = mcls_MI.gcls_pData->GetPointTbl()->pnt[ii];
		mcnt_pListRec->SetItem(ii, EM_COL_KANRINO, pPtr.cKizukenNo);
		mcnt_pListRec->SetItem(ii, EM_COL_DSP_STATUS, GetName_DspStatus(pPtr.emDispStatus));

		mcnt_pListRec->SetItem(ii, EM_COL_KIZU_NUM, mcls_MI.gcls_pData->GetDefectInfTbl()->rec[ii].num);
		ss.Format("%dm (%s)", pPtr.typ.nLenDisp[DIV_LEN_POS_KEN_TOP], GetArrayFormat((int const*)pPtr.typ.nRowDisp[DIV_LEN_POS_KEN_TOP], "/", MAP_STAGE_NUM));
		mcnt_pListRec->SetItem(ii, EM_COL_KEN_TOP, ss);
		ss.Format("%dm (%s)", pPtr.typ.nLenDisp[DIV_LEN_POS_KEN_BOT], GetArrayFormat((int const*)pPtr.typ.nRowDisp[DIV_LEN_POS_KEN_BOT], "/", MAP_STAGE_NUM));
		mcnt_pListRec->SetItem(ii, EM_COL_KEN_BOT, ss);
		ss.Format("%dm (%s)", pPtr.typ.nLenDisp[DIV_LEN_POS_DSP_TOP], GetArrayFormat((int const*)pPtr.typ.nRowDisp[DIV_LEN_POS_DSP_TOP], "/", MAP_STAGE_NUM));
		mcnt_pListRec->SetItem(ii, EM_COL_DSP_TOP, ss);
		ss.Format("%dm (%s)", pPtr.typ.nLenDisp[DIV_LEN_POS_DSP_BOT], GetArrayFormat((int const*)pPtr.typ.nRowDisp[DIV_LEN_POS_DSP_BOT], "/", MAP_STAGE_NUM));
		mcnt_pListRec->SetItem(ii, EM_COL_DSP_BOT, ss);

		mcnt_pListRec->SetItem(ii, EM_COL_ROW_WRITE_TOP, GetArrayFormat((int const*)pPtr.typ.nRowWrite[0], "/", MAP_STAGE_NUM));
		mcnt_pListRec->SetItem(ii, EM_COL_ROW_WRITE_BOT, GetArrayFormat((int const*)pPtr.typ.nRowWrite[1], "/", MAP_STAGE_NUM));
		mcnt_pListRec->SetItem(ii, EM_COL_ROW_LAST, GetArrayFormat((int const*)pPtr.typ.nRowLast, "/", MAP_STAGE_NUM));
		// 2024.07.26 Kouno 2CGL�������u�Ӄ֐��x����@�\����(������) ---------------->>>>>
		mcnt_pListRec->SetItem(ii, EM_COL_LONGPD_FLAG, ( 0 == mcls_MI.gcls_pData->GetCoilLongPdTbl()->rec[ii].nLongPdOccurrenceFlag ? "�Ȃ�" : "����"));
		// 2024.07.26 Kouno 2CGL�������u�Ӄ֐��x����@�\����(������) <<<<<----------------
	}

	CDialog::OnTimer(nIDEvent);
}

//�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P
// �p�����[�^�ύX�ʒm�{�^��
//�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q
void CHY_DEFECTDlg::OnBnClickedDebugParam()
{
	if (BST_CHECKED != CWnd::IsDlgButtonChecked(IDC_DEBUG_MANUAL)) return;
	LOG(em_WAR), "[DEBUG] �p�����[�^�ύX�ʒm�{�^��" );

	COMMON_QUE que;
	que.nEventNo = FACT_HY_DEFECT_01;
	que.nLineNo = getlineid();
	que.fl.data[0] = CH_PARAM_ALL;
	send_mail(getmytaskname(), ".", &que);
}
//�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P
// �o�C�i���o�̓{�^��
//�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q
void CHY_DEFECTDlg::OnBnClickedMemOutput()
{
	if (BST_CHECKED != CWnd::IsDlgButtonChecked(IDC_DEBUG_MANUAL)) return;
	LOG(em_WAR), "[DEBUG] �o�C�i���o�̓{�^��" );

	// ���L�������o�C�i���o��
	mcls_MI.gcls_pData->OnOutputTbl();
}
//�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P
// �o�C�i���Ǎ��{�^��
//�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q
void CHY_DEFECTDlg::OnBnClickedMemInput()
{
	if (BST_CHECKED != CWnd::IsDlgButtonChecked(IDC_DEBUG_MANUAL)) return;
	LOG(em_WAR), "[DEBUG] �o�C�i���Ǎ��{�^��" );

	// ���L�������o�C�i���Ǎ�
	mcls_MI.gcls_pData->OnInputTbl();
}

// 2024.07.26 Kouno 2CGL�������u�Ӄ֐��x����@�\����(������) ---------------->>>>>
//�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P
// ���������ה����ʒm�ʒm�{�^��
//�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q
void CHY_DEFECTDlg::OnBnClickedDebugLongpd()
{
	if (BST_CHECKED != CWnd::IsDlgButtonChecked(IDC_DEBUG_MANUAL)) return;
	LOG(em_WAR), "[DEBUG] ���������ה����ʒm�{�^��" );

	COMMON_QUE que;
	memset(&que, 0x00, sizeof(que));

	que.nEventNo = FACT_HY_DEFECT_05;
	que.nLineNo = getlineid();
	
	char cManagementNo[SIZE_KIZUKEN_NO];
	memset(cManagementNo, 0x00, sizeof(cManagementNo));
	GetDlgItemText(IDC_MANAGEMENTNO, cManagementNo, SIZE_KIZUKEN_NO);

	memcpy(que.mix.cdata[0], cManagementNo, SIZE_KIZUKEN_NO );

	send_mail(getmytaskname(), ".", &que);
}
// 2024.07.26 Kouno 2CGL�������u�Ӄ֐��x����@�\����(������) <<<<<----------------
