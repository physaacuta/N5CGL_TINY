// KS_MASTERDlg.cpp : �����t�@�C��
//

#include "stdafx.h"
#include "KS_MASTER.h"
#include "KS_MASTERDlg.h"

#include "AllUserLogOff.h"		// �����[�g�������O�I�t

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CKS_MASTERDlg �_�C�A���O




CKS_MASTERDlg::CKS_MASTERDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CKS_MASTERDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CKS_MASTERDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, m_ListBox);
}

BEGIN_MESSAGE_MAP(CKS_MASTERDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDOK, &CKS_MASTERDlg::OnBnClickedOk)
	ON_WM_CLOSE()
//	ON_WM_TIMER()
	ON_LBN_DBLCLK(IDC_LIST1, &CKS_MASTERDlg::OnLbnDblclkList1)
	ON_BN_CLICKED(IDC_ALLDOWN, &CKS_MASTERDlg::OnBnClickedAlldown)
	ON_BN_CLICKED(IDC_ALLREBOOT, &CKS_MASTERDlg::OnBnClickedAllreboot)
//	ON_BN_CLICKED(IDC_MYDOWN, &CKS_MASTERDlg::OnBnClickedMydown)
	ON_BN_CLICKED(IDC_MYREBOOT, &CKS_MASTERDlg::OnBnClickedMyreboot)
	ON_BN_CLICKED(IDC_ALLLOGOFF, &CKS_MASTERDlg::OnBnClickedAlllogoff)
	ON_BN_CLICKED(IDC_MYLOGOFF, &CKS_MASTERDlg::OnBnClickedMylogoff)
	ON_BN_CLICKED(IDC_ALLTASKEND, &CKS_MASTERDlg::OnBnClickedAlltaskend)
END_MESSAGE_MAP()


// CKS_MASTERDlg ���b�Z�[�W �n���h��

BOOL CKS_MASTERDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// ���̃_�C�A���O�̃A�C�R����ݒ肵�܂��B�A�v���P�[�V�����̃��C�� �E�B���h�E���_�C�A���O�łȂ��ꍇ�A
	//  Framework �́A���̐ݒ�������I�ɍs���܂��B
	SetIcon(m_hIcon, TRUE);			// �傫���A�C�R���̐ݒ�
	SetIcon(m_hIcon, FALSE);		// �������A�C�R���̐ݒ�

	// TODO: �������������ɒǉ����܂��B
//---------------------
	// �Ǝ�
	m_evManualEnd	= CreateEvent(NULL, FALSE, FALSE, NULL);

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
// <<<-------------------------------

	return TRUE;  // �t�H�[�J�X���R���g���[���ɐݒ肵���ꍇ�������ATRUE ��Ԃ��܂��B
}

// �_�C�A���O�ɍŏ����{�^����ǉ�����ꍇ�A�A�C�R����`�悷�邽�߂�
//  ���̃R�[�h���K�v�ł��B�h�L�������g/�r���[ ���f�����g�� MFC �A�v���P�[�V�����̏ꍇ�A
//  ����́AFramework �ɂ���Ď����I�ɐݒ肳��܂��B

void CKS_MASTERDlg::OnPaint()
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
HCURSOR CKS_MASTERDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

// ����ȍ~ �蓮�ǉ� -------------------->>>

//�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P
// �~�{�^��
//�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q
void CKS_MASTERDlg::OnClose()
{
	//CDialog::OnClose();
}
//�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P
// ����{�^��
//�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q
void CKS_MASTERDlg::OnBnClickedOk()
{
	if( IDYES == MessageBox("�I�����܂����H", "�d�v�Ȃ��m�点", MB_YESNO | MB_DEFBUTTON2 | MB_ICONQUESTION) ) {
		LOG(em_MSG), "����{�^��");
		COMMON_QUE que;
		que.nEventNo = FACT_KS_END;
		que.nLineNo = getlineid();
		send_mail(getmytaskname(), ".", &que);	
		// �N���҂��̂Ƃ��̑΍�
		SetEvent(m_evManualEnd);
	}
}

//�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P
// �������N��
//�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q
void CKS_MASTERDlg::OnLbnDblclkList1()
{
	// ListBox�R���g���[���̃v���p�e�B�́A�\�[�g�֎~�ɂ��Ă�����
	// ListBox�R���g���[���̃_�u���N���b�N�C�x���g��ǉ�
	mcls_pLog->ShowEdit();
}

//------------------------------------------
// �e�v���Z�X����������
//------------------------------------------
void CKS_MASTERDlg::InitProcess()
{
	//// ��ԏ��߂ɂ��낢�돉����
	mcls_pLog = NULL;

	// PC�������グ����肷��܂ł�����Ƒ҂�
	int nWaitTime = GetPrivateProfileInt(KS_MASTER, "START_TIMEWAIT", 5, PCINI_NAME);
	nWaitTime *= 1000;	// [s] �� [ms]
	//Sleep(nWaitTime);	
	if( WAIT_TIMEOUT != WaitForSingleObject(m_evManualEnd, nWaitTime))	{
		// �^�C���A�E�g����Ȃ��B�r�����f
		exit(-9);		// ���v���Z�X�����I��
		return;
	}

	// �^�X�N����������
	setlineid(LINE_ID);
	int nRetc = task_init(KS_MASTER);
	if( 0 != nRetc ) {
		if(-1 == nRetc) { syslog(SYSNO_TASK_INIT_ERR, "[%s] ���Ƀ^�X�N�N���ς݂ɂ��A�N�����f�B", KS_MASTER); }
		else			{ syslog(SYSNO_TASK_INIT_ERR, "[%s]", KS_MASTER); }
		_ASSERT(0);
		exit(-9);		// ���v���Z�X�����I��
		return;
	}
	m_nEndStatus = ProcessManager::P_NOTHING;

	// LogFileManager�C���X�^���X���� (����ȍ~ ���O�o�͉\)
	setlocale(LC_ALL, "Japanese");	// ���ꂪ�����Ɠ��{��p�X���܂ރt�H���_�̏ꍇ�A�ۑ��ł��Ȃ��B(���C�u�����̒��Ŏ��s����ׂ��������Ă��邪�A�ύX����ƃ^�X�N�S�̂ɉe������̂ŁA�Ƃ肠���������Őݒ�)
	mcls_pLog = new LogFileManager(getmytaskname(), &m_ListBox);
	LOG(em_MSG), "�^�X�N�N��");
	LOG(em_MSG), "�e�v���Z�X�J�n���� ���s���E�E�E");

	// ���C���C���X�^���X �J�n����
	mcls_MI.SetLogMgr(mcls_pLog);
	mcls_MI.Init();
	
	//// ��ʍX�V�p�^�C�}����
	//SetTimer(1, 500, NULL);

	// ���C���X���b�h�̎��s���s��
	LOG(em_MSG), "�e�v���Z�X�J�n���� ����");
	mcls_MI.Start();	
	return;
}

//�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P
// �e�v���Z�X�J������
//�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q
void CKS_MASTERDlg::ExitProcess()
{
	//// ���O ���X�g�{�b�N�X�\�������I��
	mcls_pLog->SetListOut(false);					// ����ȍ~ ���X�g�{�b�N�X�ւ̃��O�\���֎~

	// ���C���C���X�^���X �I������
	//KillTimer(1);			// ��ʍX�V�^�C�}
	mcls_MI.Stop();
	mcls_MI.Exit();

	//// ���O�N���X�J��
	LOG(em_MSG), "�^�X�N�I��");
	//if(NULL != mcls_pLog){
	//	delete mcls_pLog;
	//	mcls_pLog = NULL;
	//}
	mcls_pLog->SetListOut(false);					// ����ȍ~ ���X�g�{�b�N�X�ւ̃��O�\���֎~

	// �^�X�N�I������
	task_exit();
	CloseHandle( m_evManualEnd );

	// PC�I���敪
	Sleep(2000);			// �풓�^�X�N���S���I������܂ő҂�
	if( ProcessManager::P_NOTHING != m_nEndStatus) {				// �V���b�g�_�E�� // �ċN��
		// �����[�g�Z�b�V�����I��
		AllUserLogOff	clsLogoff;
		clsLogoff.SetLogMgr(mcls_pLog);
		clsLogoff.Exec();


		// PC�I��
		int retc = ProcessManager::ExitWin(m_nEndStatus);
		if( -1 == retc) {			// �V���b�g�_�E�����s
			syslog(SYSNO_SHUTDOWN_ERR, "");
		} else if(-2 == retc) {		// �ċN�����s
			syslog(SYSNO_RESTAET_ERR, "");
		} else if(-3 == retc) {		// ���O�I�t���s
			syslog(SYSNO_LOGOFF_ERR, "");
		}
	}

	//// ���O�N���X�J��
	if(NULL != mcls_pLog){
		delete mcls_pLog;
		mcls_pLog = NULL;
	}

	// ��ʂ̏I���v��
	PostMessage(WM_QUIT);
}

//------------------------------------------
// ���C���X���b�h DlgThread
// LPVOID param �_�C�����O�|�C���^
//------------------------------------------
unsigned WINAPI CKS_MASTERDlg::MainThread(void* param)
{
	CKS_MASTERDlg* p = (CKS_MASTERDlg *)param;
	COMMON_QUE que;			// ��M�\����
	COMMON_QUE sque;		// ���M�\����
	char sendPC[SIZE_NAME_PC];		// ���MPC����
	int retc;
	int nCnt;

	//// �e�v���Z�X���������� (����ȑO�̓��O�o�͋֎~)
	p->InitProcess();

	//// �N���v����荞�� ���[�v�J�n
	while(true) {
		recv_mail(&que);			// �N���v����荞��

		switch(que.nEventNo) {

//-----------------------------------------------------------------------------------------------
		case FACT_KS_MASTER_01 :							// �����ύX�v�� (��SO_GAMENN�H)
			// �����ɓ������Ԃ��Y����ׂ̑΍�
			//   �T���J�n���ɖ��񓝊��̎����Őݒ���s���l�ɂ����ׁA�~���b������Ă̌덷�������ł��Ȃ��Ȃ����B
//			SetLocalTime(&que.timeset_que.systemtime);				// �����ݒ�
//			p->mcls_MI.Send_MASTER_TIME( );							// ��PC�֎����ύX�v��
			if(true) {
				SYSTEMTIME nowtime;
				GetLocalTime(&nowtime);								// ��PC�̌��ݎ��Ԃ��擾

				// �N���������b�܂� ���ݎ����ƈꏏ�Ȃ�A��PC�̎����͍Đݒ肵�Ȃ�
				if( nowtime.wYear   != que.timeset_que.systemtime.wYear ||
				    nowtime.wMonth  != que.timeset_que.systemtime.wMonth ||
					nowtime.wDay    != que.timeset_que.systemtime.wDay ||
					nowtime.wHour   != que.timeset_que.systemtime.wHour ||
					nowtime.wMinute != que.timeset_que.systemtime.wMinute ||
					nowtime.wSecond != que.timeset_que.systemtime.wSecond ) {					
					SetLocalTime(&que.timeset_que.systemtime);		// ��PC�̎����ݒ�
				}
				p->mcls_MI.Send_MASTER_TIME( );						// ��PC�֎����ύX�v��
			}
			pLOG(em_MSG), "[MAIL] �����ύX�v�� (%4d/%2d/%2d %2d:%2d:%2d)", 
				que.timeset_que.systemtime.wYear, que.timeset_que.systemtime.wMonth, que.timeset_que.systemtime.wDay,
				que.timeset_que.systemtime.wHour, que.timeset_que.systemtime.wMinute, que.timeset_que.systemtime.wSecond );
			break;

//-----------------------------------------------------------------------------------------------
		case FACT_KS_MASTER_02 :							// �n�[�g�r�[�g (��KS_SLAVE)
			if( 0 == que.fl.data[0]) break;							// �@��ID==0�͖���

			p->mcls_MI.gcls_pCSta->Set_HeatBeatID( que.fl.data[0], true ); 
			break;

//-----------------------------------------------------------------------------------------------
		case FACT_KS_MASTER_03 :							// PC�}�X�^�[�Ǘ���Ԗ₢���킹 (��KS_SLAVE)
			pLOG(em_MSG), "[MAIL] PC�}�X�^�[�Ǘ���Ԗ₢���킹 (PC=%s)", que.fc.data);
			// DB�����ڑ��m����̂݁A�ԐM
			p->mcls_MI.Send_MASTER_OK(que.fc.data);
			break;

//-----------------------------------------------------------------------------------------------
		case FACT_KS_MASTER_04 :							// �����グ������ (��KS_SLAVE, KS_MASTER)
			if( 0 == que.fl.data[0]) break;							// �@��ID==0�͖���

			// �Y���̋@��ID�͑��݂���H
			if( -1 != p->mcls_MI.gcls_pCSta->GetIndex(que.fl.data[0]) ) {
				// �����グ����ł́A���s����ꍇ������΍�B����Ŏ��s����ƊY��PC�������オ���Ă��Ă��A�ُ�ƂȂ��
				nCnt = 0;
				while(true) {	
					if( KizuFunction::SetStatus( que.fl.data[0], true) ) break;
					pLOG(em_WAR), "[Dlg] SetStatus ���s�ɂ�胊�g���C�B �����グ������ [%d]", que.fl.data[0]);
					Sleep(50);
					nCnt++;
					if(5 < nCnt) {
						pLOG(em_ERR), "[Dlg] SetStatus ���s�I�I �����グ������ [%d]", que.fl.data[0]);
						break;
					}
				}
				pLOG(em_MSG), "[MAIL] �����グ������ [PC��:%s]", p->mcls_MI.gcls_pCSta->GetName(que.fl.data[0]) );
			} else {
				pLOG(em_ERR), "[MAIL] �����グ�����񍐈ُ� [%d]", que.fl.data[0] );
			}
			// HB�����������Ă���
			if( 0 < que.fl.data[1] ) {
				p->mcls_MI.gcls_pCSta->Set_HeatBeatID( que.fl.data[1], true ); 
				// �����グ����ł́A���s����ꍇ������΍�B
				nCnt = 0;
				while(true) {	
					if( KizuFunction::SetStatus( que.fl.data[1], true) ) break;
					pLOG(em_WAR), "[Dlg] SetStatus ���s�ɂ�胊�g���C�B HB�� [%d]", que.fl.data[0]);
					Sleep(50);
					nCnt++;
					if(5 < nCnt) {
						pLOG(em_ERR), "[Dlg] SetStatus ���s�I�I HB�� [%d]", que.fl.data[0]);
						break;
					}
				}
			}
			break;

//-----------------------------------------------------------------------------------------------
		case FACT_KS_MASTER_05 :							// �������������� (��KS_SLAVE)
			if( 0 == que.fl.data[0]) break;							// �@��ID==0�͖���

			// �Y���̋@��ID�͑��݂���H
			if( -1 != p->mcls_MI.gcls_pCSta->GetIndex(que.fl.data[0]) ) {
				KizuFunction::SetStatus( que.fl.data[0], false);
				pLOG(em_MSG), "[MAIL] �������������� [%s]", p->mcls_MI.gcls_pCSta->GetName(que.fl.data[0]) );
			} else {
				pLOG(em_ERR), "[MAIL] �������������񍐈ُ� [%d]", que.fl.data[0] );
			}
			// HB���ُ�ɂ��Ă���
			if( 0 < que.fl.data[1] ) {
				p->mcls_MI.gcls_pCSta->Set_HeatBeatID( que.fl.data[1], false ); 
				KizuFunction::SetStatus( que.fl.data[1], false);
			}
			break;

//-----------------------------------------------------------------------------------------------
		case FACT_KS_MASTER_06 :							// �V�X�e���I���v�� (��SO_GAMENN)
			pLOG(em_MSG), "[MAIL] �V�X�e���I���v��");
			syslog(SYSNO_SHUTDOWN_OK, "");

			// ������~�ʒm
			p->SendMail_Stop();

			// �SPC�փV�X�e���I���v��
			sque.nEventNo = FACT_KS_SLAVE_04;
			sque.nLineNo = getlineid();															// ���C��ID
			retc = send_mail( KS_SLAVE, "*", &sque );
			if( 0 != retc ) syslog(SYSNO_MAIL_SNED_ERR, "[%d:�V�X�e���I���v�� <err=%d>]", FACT_KS_MASTER_06, retc);
			
			// �V�X�e���I�����́A�������K��PC�V���b�g�_�E���Ώ�
			Sleep(200);				// ����PC����ʒm���󂯎���悤�ɂ�����Ƒ҂�
			p->m_nEndStatus = ProcessManager::P_POWEROFF;
			sque.nEventNo = FACT_KS_END;
			sque.nLineNo = getlineid();															// ���C��ID
			send_mail(getmytaskname(), ".", &sque);	
			break;

//-----------------------------------------------------------------------------------------------
		case FACT_KS_MASTER_07 :							// PC�ċN���v�� (��SO_GAMENN)
			// �����@�\�A�~�ϋ@�\ �̏ꍇ�́A"*"���w�肵�Ă����B
			// vb����̕�����́ANULL�Ŗ����ăX�y�[�X�Ȃ̂ŁA�X�y�[�X��NULL�ɕϊ�
			//   �� .net�ł���́A���v�Ȃ悤�ɂ����̂ŁAOK�̂͂��B�Ƃ肠��������Ă����B(���ł��́A�}�l�[�W�h�R�[�h���X�𗝉����Ă���l�ɂ͕�����͂�)
			KizuFunction::SpaceToNull(que.fc.data, sizeof(que.fc.data));
			pLOG(em_MSG), "[MAIL] PC�ċN���v�� [�Ώ�PC:%s]", que.fc.data);
			syslog(SYSNO_RESTAET_OK, "[%s]", que.fc.data);

			//// PC�ċN���v��
			memset(sendPC, 0x00, sizeof(sendPC));
			sque.nEventNo = FACT_KS_SLAVE_05;
			sque.nLineNo = getlineid();															// ���C��ID
			// �Ώ�PC����
			if( 0 == strcmp(que.fc.data, "*" ) || 0 == strcmp(que.fc.data, TIKUSEKI) ) {
				// �~��PC���ċN���Ώۂ̏ꍇ�́A�S�V�X�e����PC���ċN��
				sque.fl.data[0] = 0;														// ���[�����󂯎�����SPC���ċN��
				sendPC[0] = '*';
				p->SendMail_Stop();	// ������~�ʒm

			} else if(0 == strcmp(que.fc.data, MASTER_PC_NAME)) {
				// ����PC���ċN���Ώۂ̏ꍇ�́A�~��PC�������V�X�e����PC���ċN��
				sque.fl.data[0] = 1;														// ���[�����󂯎����PC�̒~�ψȊO���ċN��
				sendPC[0] = '*';
				p->SendMail_Stop();	// ������~�ʒm

			} else {
				// �w��PC�̂ݍċN��
				sque.fl.data[0] = 0;														// ���[�����󂯎�����SPC���ċN��
				memcpy(sendPC, que.fc.data, sizeof(sendPC));
			}
			retc = send_mail( KS_SLAVE, sendPC, &sque );
			if( 0 != retc ) syslog(SYSNO_MAIL_SNED_ERR, "[%d:PC�ċN���v�� <err=%d>]", FACT_KS_MASTER_07, retc);

/*			
			// ������PC���ΏہH (�ʏ킠�肦�Ȃ��͂��B�����Ǎ���̂��߂ɏ����ǉ�)
			if( 0 == strcmp(sendPC, "*" ) ||						// �������Ώ�
				0 == strcmp(sendPC, ".")  ||						// �������Ώ�
				0 == strcmp(sendPC, PCID_NAME(getpcid())) )	{		// ������PC���ƈ�v
*/
			// ������PC���ΏہH (�ʏ킠�肦�Ȃ��͂��B�����Ǎ���̂��߂ɏ����ǉ�)
			if( 0 == strcmp(sendPC, "*" ) ||						// �������Ώ�
				0 == strcmp(sendPC, ".")  ) {						// �������Ώ�

				Sleep(500);				// ����PC����ʒm���󂯎���悤�ɂ�����Ƒ҂�
				p->m_nEndStatus = ProcessManager::P_REBOOT;
				sque.nEventNo = FACT_KS_END;
				sque.nLineNo = getlineid();															// ���C��ID
				send_mail(getmytaskname(), ".", &sque);	
			}
			break;

//-----------------------------------------------------------------------------------------------
		case FACT_KS_MASTER_08 :							// PC���O�I�t�v�� (��SO_GAMENN)
			// �����@�\�A�~�ϋ@�\ �̏ꍇ�́A"*"���w�肵�Ă����B
			// vb����̕�����́ANULL�Ŗ����ăX�y�[�X�Ȃ̂ŁA�X�y�[�X��NULL�ɕϊ�
			//   �� .net�ł���́A���v�Ȃ悤�ɂ����̂ŁAOK�̂͂��B�Ƃ肠��������Ă����B(���ł��́A�}�l�[�W�h�R�[�h���X�𗝉����Ă���l�ɂ͕�����͂�)
			KizuFunction::SpaceToNull(que.fc.data, sizeof(que.fc.data));
			pLOG(em_MSG), "[MAIL] PC���O�I�t�v�� [�Ώ�PC:%s]", que.fc.data);
			syslog(SYSNO_LOGOFF_OK, "[%s]", que.fc.data);

			//// PC�ċN���v��
			memset(sendPC, 0x00, sizeof(sendPC));
			sque.nEventNo = FACT_KS_SLAVE_06;
			sque.nLineNo = getlineid();															// ���C��ID
			// �Ώ�PC����
			if( 0 == strcmp(que.fc.data, "*" ) || 0 == strcmp(que.fc.data, TIKUSEKI) ) {
				// �~��PC���ċN���Ώۂ̏ꍇ�́A�S�V�X�e����PC���ċN��
				sque.fl.data[0] = 0;														// ���[�����󂯎�����SPC���ċN��
				sendPC[0] = '*';
				p->SendMail_Stop();	// ������~�ʒm
			} else if(0 == strcmp(que.fc.data, MASTER_PC_NAME)) {
				// ����PC���ċN���Ώۂ̏ꍇ�́A�~��PC�������V�X�e����PC���ċN��
				sque.fl.data[0] = 1;														// ���[�����󂯎����PC�̒~�ψȊO���ċN��
				sendPC[0] = '*';
				p->SendMail_Stop();	// ������~�ʒm
			} else {
				// �w��PC�̂ݍċN��
				sque.fl.data[0] = 0;														// ���[�����󂯎�����SPC���ċN��
				memcpy(sendPC, que.fc.data, sizeof(sendPC));
			}
			retc = send_mail( KS_SLAVE, sendPC, &sque );
			if( 0 != retc ) syslog(SYSNO_MAIL_SNED_ERR, "[%d:PC���O�I�t�v�� <err=%d>]", FACT_KS_MASTER_08, retc);

/*
			// ������PC���ΏہH (�ʏ킠�肦�Ȃ��͂��B�����Ǎ���̂��߂ɏ����ǉ�)
			if( 0 == strcmp(sendPC, "*" ) ||						// �������Ώ�
				0 == strcmp(sendPC, ".")  ||						// �������Ώ�
				0 == strcmp(sendPC, PCID_NAME(getpcid())) )	{		// ������PC���ƈ�v
*/
			// ������PC���ΏہH (�ʏ킠�肦�Ȃ��͂��B�����Ǎ���̂��߂ɏ����ǉ�)
			if( 0 == strcmp(sendPC, "*" ) ||						// �������Ώ�
				0 == strcmp(sendPC, ".")  ) {						// �������Ώ�

				Sleep(500);				// ����PC����ʒm���󂯎���悤�ɂ�����Ƒ҂�
				p->m_nEndStatus = ProcessManager::P_LOGOFF;
				sque.nEventNo = FACT_KS_END;
				sque.nLineNo = getlineid();															// ���C��ID
				send_mail(getmytaskname(), ".", &sque);	
			}
			break;

//-----------------------------------------------------------------------------------------------
		case FACT_KS_MASTER_09 :							// PC�풓�^�X�N�I���v�� (��SO_GAMENN)
			// �����@�\�A�~�ϋ@�\ �̏ꍇ�́A"*"���w�肵�Ă����B
			// vb����̕�����́ANULL�Ŗ����ăX�y�[�X�Ȃ̂ŁA�X�y�[�X��NULL�ɕϊ�
			//   �� .net�ł���́A���v�Ȃ悤�ɂ����̂ŁAOK�̂͂��B�Ƃ肠��������Ă����B(���ł��́A�}�l�[�W�h�R�[�h���X�𗝉����Ă���l�ɂ͕�����͂�)
			KizuFunction::SpaceToNull(que.fc.data, sizeof(que.fc.data));
			pLOG(em_MSG), "[MAIL] PC�풓�^�X�N�I���v�� [�Ώ�PC:%s]", que.fc.data);
			syslog(SYSNO_SHUTDOWN_OK, "[%s] (�풓�^�X�N�̂ݏI��)", que.fc.data);

			//// PC�ċN���v��
			memset(sendPC, 0x00, sizeof(sendPC));
			sque.nEventNo = FACT_KS_SLAVE_07;
			sque.nLineNo = getlineid();															// ���C��ID
			// �Ώ�PC����
			if( 0 == strcmp(que.fc.data, "*" ) || 0 == strcmp(que.fc.data, TIKUSEKI) ) {
				// �~��PC���ċN���Ώۂ̏ꍇ�́A�S�V�X�e����PC���ċN��
				sque.fl.data[0] = 0;														// ���[�����󂯎�����SPC���ċN��
				sendPC[0] = '*';

				p->SendMail_Stop();	// ������~�ʒm
			} else if(0 == strcmp(que.fc.data, MASTER_PC_NAME)) {
				// ����PC���ċN���Ώۂ̏ꍇ�́A�~��PC�������V�X�e����PC���ċN��
				sque.fl.data[0] = 1;														// ���[�����󂯎����PC�̒~�ψȊO���ċN��
				sendPC[0] = '*';

				p->SendMail_Stop();	// ������~�ʒm
			} else {
				// �w��PC�̂ݍċN��
				sque.fl.data[0] = 0;														// ���[�����󂯎�����SPC���ċN��
				memcpy(sendPC, que.fc.data, sizeof(sendPC));
			}
			retc = send_mail( KS_SLAVE, sendPC, &sque );
			if( 0 != retc ) syslog(SYSNO_MAIL_SNED_ERR, "[%d:PC�풓�^�X�N�I���v�� <err=%d>]", FACT_KS_MASTER_09, retc);

/*
			// ������PC���ΏہH (�ʏ킠�肦�Ȃ��͂��B�����Ǎ���̂��߂ɏ����ǉ�)
			if( 0 == strcmp(sendPC, "*" ) ||						// �������Ώ�
				0 == strcmp(sendPC, ".")  ||						// �������Ώ�
				0 == strcmp(sendPC, PCID_NAME(getpcid())) )	{		// ������PC���ƈ�v
*/
			// ������PC���ΏہH (�ʏ킠�肦�Ȃ��͂��B�����Ǎ���̂��߂ɏ����ǉ�)
			if( 0 == strcmp(sendPC, "*" ) ||						// �������Ώ�
				0 == strcmp(sendPC, ".")  ) {						// �������Ώ�

				Sleep(500);				// ����PC����ʒm���󂯎���悤�ɂ�����Ƒ҂�
				p->m_nEndStatus = ProcessManager::P_NOTHING;
				sque.nEventNo = FACT_KS_END;
				sque.nLineNo = getlineid();															// ���C��ID
				send_mail(getmytaskname(), ".", &sque);	
			}
			break;
//-----------------------------------------------------------------------------------------------
		case FACT_KS_PCNAME_CHANGE:															// PC���̕ύX�ʒm�N���v��
			//// �SPC���̂�DB���擾
			if( getpcname() ) {
				pLOG(em_MSG), "[MAIL] �SPC���̂�DB���擾");
			} else {
				pLOG(em_ERR), "[MAIL] �SPC���̂�DB���擾���s");
			}
			//// PC�X���[�u�Ǘ��ɒʒm
			sque.nEventNo = FACT_KS_PCNAME_CHANGE;
			sque.nLineNo = getlineid();
			send_mail( KS_SLAVE, "*", &sque );
			//// �풓�v���Z�X�S���ɒʒm
			ProcessManager::AllProcessSend(&sque);
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


//------------------------------------------
// ������~�ʒm ���M
//------------------------------------------
void CKS_MASTERDlg::SendMail_Stop()
{
	char cPc[SIZE_NAME_PC];
	char cTask[SIZE_NAME_TASK];
	int	 nMailNo;
	int  nWaitTime;

	//// �����������{����H
	GetPrivateProfileString(KS_MASTER, "STOP_SEND_MAIL_PC", "", cPc, sizeof(cPc), PCINI_NAME);			
	GetPrivateProfileString(KS_MASTER, "STOP_SEND_MAIL_TASK", "", cTask, sizeof(cTask), PCINI_NAME);
	nMailNo = GetPrivateProfileInt(KS_MASTER, "STOP_SEND_MAIL_NO", 0, PCINI_NAME);
	nWaitTime = GetPrivateProfileInt(KS_MASTER, "STOP_AFFTER_WAIT_TIME", 0, PCINI_NAME);

	KizuFunction::Trim(cPc);
	KizuFunction::Trim(cTask);

	if( 1 > strlen(cPc) ) return;

	// ���[�����M
	COMMON_QUE sque;		// ���M�\����
	sque.nEventNo = nMailNo;
	sque.nLineNo = getlineid();															// ���C��ID
	send_mail(cTask, cPc, &sque);	

	Sleep(nWaitTime*1000);
}


//�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P
// ��ʍX�V�^�C�}
//�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q
//void CKS_MASTERDlg::OnTimer(UINT_PTR nIDEvent)
//{
//	// TODO: �����Ƀ��b�Z�[�W �n���h�� �R�[�h��ǉ����邩�A����̏������Ăяo���܂��B
//
//	CDialog::OnTimer(nIDEvent);
//}

//�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P
// �f�o�b�N�R�}���h
//�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q
void CKS_MASTERDlg::OnBnClickedAlldown()
{
	if( IDYES == MessageBox("�SPC���V���b�g�_�E�����܂����H", "�d�v�Ȃ��m�点", MB_YESNO | MB_DEFBUTTON2 | MB_ICONQUESTION) ) {
		LOG(em_MSG), "�SPC�V���b�g�_�E���{�^��");
		COMMON_QUE que;
		que.nEventNo = FACT_KS_MASTER_06;
		que.nLineNo = getlineid();															// ���C��ID
		send_mail(getmytaskname(), ".", &que);	
	}
}
void CKS_MASTERDlg::OnBnClickedAllreboot()
{
	if( IDYES == MessageBox("�SPC���ċN�����܂����H", "�d�v�Ȃ��m�点", MB_YESNO | MB_DEFBUTTON2 | MB_ICONQUESTION) ) {
		LOG(em_MSG), "�SPC�ċN���{�^��");
		COMMON_QUE que;
		que.nEventNo = FACT_KS_MASTER_07;
		que.nLineNo = getlineid();															// ���C��ID
		strcpy(que.fc.data, "*");
		send_mail(getmytaskname(), ".", &que);	
	}
}
void CKS_MASTERDlg::OnBnClickedAlllogoff()
{
	if( IDYES == MessageBox("�SPC�����O�I�t���܂����H", "�d�v�Ȃ��m�点", MB_YESNO | MB_DEFBUTTON2 | MB_ICONQUESTION) ) {
		LOG(em_MSG), "�SPC���O�I�t�{�^��");
		COMMON_QUE que;
		que.nEventNo = FACT_KS_MASTER_08;
		que.nLineNo = getlineid();															// ���C��ID
		strcpy(que.fc.data, "*");
		send_mail(getmytaskname(), ".", &que);	
	}
}

void CKS_MASTERDlg::OnBnClickedAlltaskend()
{
	if( IDYES == MessageBox("�SPC�̏풓�^�X�N���I�����܂����H", "�d�v�Ȃ��m�点", MB_YESNO | MB_DEFBUTTON2 | MB_ICONQUESTION) ) {
		LOG(em_MSG), "�SPC�풓�^�X�N�I���{�^��");
		COMMON_QUE que;
		que.nEventNo = FACT_KS_MASTER_09;
		que.nLineNo = getlineid();															// ���C��ID
		strcpy(que.fc.data, "*");
		send_mail(getmytaskname(), ".", &que);	
	}
}

void CKS_MASTERDlg::OnBnClickedMyreboot()
{
	if( IDYES == MessageBox("��PC���ċN�����܂����H", "�d�v�Ȃ��m�点", MB_YESNO | MB_DEFBUTTON2 | MB_ICONQUESTION) ) {
		LOG(em_MSG), "��PC�ċN���{�^��");
		COMMON_QUE que;
		que.nEventNo = FACT_KS_MASTER_07;
		que.nLineNo = getlineid();															// ���C��ID
		strcpy(que.fc.data, MASTER_PC_NAME);
		send_mail(getmytaskname(), ".", &que);	
	}
}

void CKS_MASTERDlg::OnBnClickedMylogoff()
{
	if( IDYES == MessageBox("��PC�����O�I�t���܂����H", "�d�v�Ȃ��m�点", MB_YESNO | MB_DEFBUTTON2 | MB_ICONQUESTION) ) {
		LOG(em_MSG), "��PC���O�I�t�{�^��");
		COMMON_QUE que;
		que.nEventNo = FACT_KS_MASTER_08;
		que.nLineNo = getlineid();															// ���C��ID
		strcpy(que.fc.data, MASTER_PC_NAME);
		send_mail(getmytaskname(), ".", &que);	
	}
}