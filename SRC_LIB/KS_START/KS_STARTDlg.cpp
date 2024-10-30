// KS_STARTDlg.cpp : �����t�@�C��
//

#include "stdafx.h"
#include "KS_START.h"
#include "KS_STARTDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CKS_STARTDlg �_�C�A���O




CKS_STARTDlg::CKS_STARTDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CKS_STARTDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CKS_STARTDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, m_ListBox);
}

BEGIN_MESSAGE_MAP(CKS_STARTDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDOK, &CKS_STARTDlg::OnBnClickedOk)
	ON_LBN_DBLCLK(IDC_LIST1, &CKS_STARTDlg::OnLbnDblclkList1)
END_MESSAGE_MAP()


// CKS_STARTDlg ���b�Z�[�W �n���h��

BOOL CKS_STARTDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// ���̃_�C�A���O�̃A�C�R����ݒ肵�܂��B�A�v���P�[�V�����̃��C�� �E�B���h�E���_�C�A���O�łȂ��ꍇ�A
	//  Framework �́A���̐ݒ�������I�ɍs���܂��B
	SetIcon(m_hIcon, TRUE);			// �傫���A�C�R���̐ݒ�
	SetIcon(m_hIcon, FALSE);		// �������A�C�R���̐ݒ�

	// TODO: �������������ɒǉ����܂��B
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
	// ��ʈʒu
	//::SetWindowPos(m_hWnd, NULL, 1, 1, 10, 10, SWP_NOZORDER);
	::SetWindowPos(m_hWnd, NULL, 1, 1, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
	// ���C���X���b�h�J�n
	unsigned int tid;	// �X���b�hID
	_beginthreadex(NULL, 0, MainThread, (LPVOID)this, 0, &tid);
// <<<-------------------------------
	return TRUE;  // �t�H�[�J�X���R���g���[���ɐݒ肵���ꍇ�������ATRUE ��Ԃ��܂��B
}

// �_�C�A���O�ɍŏ����{�^����ǉ�����ꍇ�A�A�C�R����`�悷�邽�߂�
//  ���̃R�[�h���K�v�ł��B�h�L�������g/�r���[ ���f�����g�� MFC �A�v���P�[�V�����̏ꍇ�A
//  ����́AFramework �ɂ���Ď����I�ɐݒ肳��܂��B

void CKS_STARTDlg::OnPaint()
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
HCURSOR CKS_STARTDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


// ����ȍ~ �蓮�ǉ� -------------------->>>

//�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P
// �~�{�^��
//�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q
void CKS_STARTDlg::OnClose()
{
	//CDialog::OnClose();
}
//�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P
// ����{�^��
//�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q
void CKS_STARTDlg::OnBnClickedOk()
{
	if( IDYES == MessageBox("�I�����܂����H", "�d�v�Ȃ��m�点", MB_YESNO | MB_DEFBUTTON2 | MB_ICONQUESTION) ) {
		// �����I���ʒm
		m_bCancel = true;
		LOG(em_MSG), "�����I�����s���܂��B�b�����҂��������B");

		CDialog::OnClose();
	}
}

//------------------------------------------
// ���C���X���b�h DlgThread
// LPVOID param �_�C�����O�|�C���^
//------------------------------------------
unsigned WINAPI CKS_STARTDlg::MainThread(void* param)
{
	CKS_STARTDlg* p = (CKS_STARTDlg *)param;

	// LogFileManager�C���X�^���X���� (����ȍ~ ���O�o�͉\)
	setlocale(LC_ALL, "Japanese");	// ���ꂪ�����Ɠ��{��p�X���܂ރt�H���_�̏ꍇ�A�ۑ��ł��Ȃ��B(���C�u�����̒��Ŏ��s����ׂ��������Ă��邪�A�ύX����ƃ^�X�N�S�̂ɉe������̂ŁA�Ƃ肠���������Őݒ�)
	p->mcls_pLog = new LogFileManager(KS_START, &p->m_ListBox);
	pLOG(em_MSG), "�^�X�N�N��");


	//// �����[�g�f�X�N�g�b�v�ڑ��ɂ��A�Q�d�N���΍�
	bool bExec = true;
	bExec &= p->CheckMultiExec(KS_MASTER);
	bExec &= p->CheckMultiExec(KS_SLAVE);


	//// ���C������
	if(bExec) {
		p->m_bCancel = false;
		MainExec*	cls_pExec = new MainExec(p->mcls_pLog, &p->m_bCancel);
		cls_pExec->DoExec();
		delete cls_pExec;
	}

	//// ���O�N���X�J��
	pLOG(em_MSG), "�^�X�N�I��");
	Sleep(100);
	if(NULL != p->mcls_pLog){
		delete p->mcls_pLog;
		p->mcls_pLog = NULL;
	}

	// ��ʂ̏I���v��
	p->PostMessage(WM_QUIT);

	return 0;
}
//�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P
// �������N��
//�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q
void CKS_STARTDlg::OnLbnDblclkList1()
{
	// ListBox�R���g���[���̃v���p�e�B�́A�\�[�g�֎~�ɂ��Ă�����
	// ListBox�R���g���[���̃_�u���N���b�N�C�x���g��ǉ�
	mcls_pLog->ShowEdit();
}

//------------------------------------------
// �풓�^�X�N �������s�`�F�b�N
// LPCSTR SendSlotName �ڑ��`�F�b�N�^�X�N����
// �߂�l: false:���Ɏ��s��
//------------------------------------------
bool CKS_STARTDlg::CheckMultiExec(LPCSTR SendSlotName)
{
	HANDLE hSend = INVALID_HANDLE_VALUE;
	char fullname[MAX_PATH];

	// �ڑ��m�F
	sprintf(fullname, "\\\\.\\mailslot\\%s", SendSlotName);
	hSend = CreateFile(fullname, 
						GENERIC_WRITE, 
						FILE_SHARE_WRITE | FILE_SHARE_READ,
						NULL, 
						OPEN_EXISTING, 
						FILE_ATTRIBUTE_NORMAL,	
						NULL);
	if(hSend == INVALID_HANDLE_VALUE) return true;		// �풓�^�X�N����

	// �ڑ��ł������������A�ؒf
	CloseHandle(hSend);
	return false;
}
