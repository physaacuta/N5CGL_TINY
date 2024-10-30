#include "StdAfx.h"
#include "MainInctance.h"

//------------------------------------------
// �R���X�g���N�^
//------------------------------------------
MainInctance::MainInctance(void) :
m_bInitalize(false),
mcls_pLog(NULL)
{
	// �V�O�i������
	m_evThStart  = CreateEvent(NULL, FALSE, FALSE, NULL);				// ���C���X���b�h�����p
}

//------------------------------------------
// �f�X�g���N�^
//------------------------------------------
MainInctance::~MainInctance(void)
{
	// �V�O�i���J��
	CloseHandle(m_evThStart);
}

//------------------------------------------
// ���������� (��x�̂݋N��)
//------------------------------------------
BOOL MainInctance::Init()
{
	//// ���C���C���X�^���X�̂݁A�X���b�h�����͍���Ă��� (�����ɔ�V�O�i���Ƃ���)
	//// �p�����N���X���s (���C������X���b�h�ׁ̈A������ƗD��x����)
	WorkThread::Start(MainThread, THREAD_PRIORITY_ABOVE_NORMAL );

	// �X���b�hID��������܂ł�����Ƌx�e
	while( 0==GetTid() ) {
		LOG(em_MSG), "[MainInctance] �X���b�hID���蓖�Ē��E�E�E");
		Sleep(1);			// �X���b�h�N���܂ł�����Ƒ҂�
	}

	//// ���[�J�[�X���b�h ������
	// �@���ԊǗ��N���X
	gcls_pCSta = new CheckStatus(true);
	gcls_pCSta->SetLogMgr(mcls_pLog);
	gcls_pCSta->SetMainPostTid(GetTid());

	// �f�[�^�x�[�X�Ǘ��X���b�h
	gcls_pCDb = new CheckDB(true);
	gcls_pCDb->SetLogMgr(mcls_pLog);
	gcls_pCDb->SetMainPostTid(GetTid());
	if( 0==getpcid() || -1 == getpcid() ) {
		gcls_pCDb->SetFastExecDebug();	// ����OK���
		PostThreadMessage(GetTid(), E_DEF_DB_FAST_CONNECT, 0, 0);
	} else {
		gcls_pCDb->Start();
	}

	return TRUE;
}

//------------------------------------------
// �I������ (��x�̂݋N��)
//------------------------------------------
void MainInctance::Exit()
{

	//// ���[�J�[�X���b�h ��~
	gcls_pCSta->Stop();
	gcls_pCDb->Stop();

	//// ���[�J�[�X���b�h �J��
	delete gcls_pCSta;
	delete gcls_pCDb;
}

//------------------------------------------
// �X���b�h
//------------------------------------------
UINT WINAPI MainInctance::MainThread(LPVOID param)
{
	MSG	msg;								// MSG�\����

	////// ���C���C���X�^���X���o��
	MainInctance *p = (MainInctance *)param;						// ���N���X�̃C���X�^���X���擾
	PeekMessage(&msg, NULL, WM_USER, WM_USER, PM_NOREMOVE);			// ���b�Z�[�W�쐬
	p->SetTid();													// �X���b�hID�Z�b�g


	////// �����[�N�X���b�h�̏����������܂őҋ@
	pLOG(em_MSG), "[MainInctance] �����[�N�X���b�h�̏����������܂őҋ@���E�E�E [0x%X]", p->GetTid() );
	WaitForSingleObject(p->m_evThStart, INFINITE);


	////// ���b�Z�[�W�L���[�ǂݍ���
	pLOG(em_MSG), "[MainInctance] �X���b�h�J�n [0x%X]", p->GetTid());
	while( int ret = GetMessage(&msg, NULL, 0, 0) ){
		_ASSERT( -1 != ret );
		switch (msg.message) {

//-----------------------------------------------------------------------------------------------
		case E_DEF_DB_FAST_CONNECT:					// DB�����ڑ������ʒm
			p->Init_Process();							// �v���Z�X����
			break;


//-----------------------------------------------------------------------------------------------
		case E_DEF_DB_NG_TO_OK:						// DB�ڑ���� NG��OK (����������)
			// PC��ԓ����H������Ă���ꍇ�΍�
			p->Send_MASTER_OK("*");
			break;

//-----------------------------------------------------------------------------------------------
		case WM_USER_TH_END: 					// �X���b�h�I���v��
			pLOG(em_MSG), "[MainInctance] �X���b�h�I�� [0x%X]", p->GetTid());
			p->Exit_Process();							// �v���Z�X�J��
			return 0;

		default :
			// ���肦�Ȃ��I�I
			break;
		}
	}

	// ���肦�Ȃ�
	pLOG(em_ERR), "[MainInctance] ���C�����[�v���甲�����I�I[0x%X]", p->GetTid());
	return 0;
}

//------------------------------------------
// �v���Z�X �X�^�[�g
//------------------------------------------
void MainInctance::Init_Process()
{
	COMMON_QUE que;

	m_bInitalize = true;

	//// �R�}���h���s
	Exec_Cmd( "START" );

	//// �SPC���̂�DB���擾
	if( getpcname() ) {
		LOG(em_MSG), "�SPC���̂�DB���擾");
	} else {
		LOG(em_ERR), "�SPC���̂�DB���擾���s");		// �����ŃG���[�͂��肦�Ȃ��͂�
	}

	//// �@���ԊǗ��N���X�̏�����
	gcls_pCSta->Alloc();


	//// �C�j�V�������ҋ@��ʂ�DB�ڑ������ʒm���M
	que.nEventNo = FACT_KS_WAKEUP_02; 
	que.nLineNo = getlineid();															// ���C��ID
	send_mail( KS_WAKEUP, ".", &que );	


	//// ���L����������
	mcls_Smem.CreateMem(true); 
	LOG(em_MSG), "���L��������������");

	//// �풓�v���Z�X�N��
	ProcessManager::ExecProcess(); 
	LOG(em_MSG), "�풓�v���Z�X�N������");
	
	//// �@���ԊǗ��N���X�N��
	gcls_pCSta->Start();

	//// �SPC�� �����ݒ�v��
	Send_MASTER_TIME(); 

	//// �������g�� �����グ������
	que.nEventNo = FACT_KS_MASTER_04; 
	que.nLineNo = getlineid();															// ���C��ID
	que.fl.data[0] = GetPrivateProfileInt(KS_MASTER, "APP_ENABLE_ID", 0, PCINI_NAME);
	que.fl.data[1] = GetPrivateProfileInt(KS_MASTER, "HB_SEND_ENABLE", 0, PCINI_NAME);
	int retc = send_mail( getmytaskname(), ".", &que );	
	if( 0 != retc ) syslog(SYSNO_MAIL_SNED_ERR, "[%d:�����グ������ <err=%d>]", FACT_KS_MASTER_04, retc);

	//// ��PC�� PC�}�X�^�[�Ǘ������グ�����ʒm
	Send_MASTER_OK("*");

	//// �C�j�V�������ҋ@��� �������������ʒm (����܂�Ӗ��Ȃ�)
	que.nEventNo = FACT_KS_WAKEUP_01;
	que.nLineNo = getlineid();															// ���C��ID
	send_mail( KS_WAKEUP, ".", &que );

	//// �C�j�V�������ҋ@��� �I��
	que.nEventNo = FACT_KS_END;
	que.nLineNo = getlineid();															// ���C��ID
	send_mail( KS_WAKEUP, ".", &que );
}

//------------------------------------------
// �v���Z�X �G���h
//------------------------------------------
void MainInctance::Exit_Process()
{
	//// ���������Ă��Ȃ��ꍇ�́A�Ȃɂ����Ȃ�
	if( m_bInitalize) {
		//// �@���ԊǗ��N���X��~
		gcls_pCSta->Stop();

		//// �풓�v���Z�X�I��
		ProcessManager::ExitProcess(); 
		Sleep(1000);						// �S�^�X�N���I���������󂯕t����܂ł�����Ƒ҂�

		//// ���L�������J��
		mcls_Smem.DeleteMem(); 
	}
	
	//// �R�}���h���s
	Exec_Cmd( "END" );

	//// �������g�� ��������������
	COMMON_QUE que;
	que.nEventNo = FACT_KS_MASTER_05; 
	que.nLineNo = getlineid();															// ���C��ID
	que.fl.data[0] = GetPrivateProfileInt(KS_MASTER, "APP_ENABLE_ID", 0, PCINI_NAME);
	que.fl.data[1] = GetPrivateProfileInt(KS_MASTER, "HB_SEND_ENABLE", 0, PCINI_NAME);
	int retc = send_mail( getmytaskname(), ".", &que );
	if( 0 != retc ) syslog(SYSNO_MAIL_SNED_ERR, "[%d:�������������� <err=%d>]", FACT_KS_MASTER_05, retc);
}

//------------------------------------------
// ��PC�� PC�}�X�^�[�Ǘ������グ�����ʒm
// char const * cPc ���MPC����
//------------------------------------------
void MainInctance::Send_MASTER_OK(char const * cPc)
{
	if( ! gcls_pCDb->GetFastExec() ) {
		COMMON_QUE que;

		// �ʒm
		LOG(em_MSG), "PC�}�X�^�[�Ǘ������グ�����ʒm [���M��F%s]", cPc );
		que.nEventNo = FACT_KS_SLAVE_03;
		que.nLineNo = getlineid();															// ���C��ID
		int retc = send_mail( KS_SLAVE, cPc, &que );	
		if( 0 != retc ) syslog(SYSNO_MAIL_SNED_ERR, "[%d:PC�}�X�^�[�Ǘ������グ�����ʒm[%s] <err=%d>]", FACT_KS_SLAVE_03, cPc, retc);
	}
}
//------------------------------------------
// �SPC�� �����ύX�ʒm
//------------------------------------------
void MainInctance::Send_MASTER_TIME()
{
	LOG(em_MSG), "�SPC�� �����ύX�ʒm");
	COMMON_QUE que;
	que.nEventNo = FACT_KS_SLAVE_01;
	que.nLineNo = getlineid();															// ���C��ID
	GetLocalTime(&que.timeset_que.systemtime);	// ��PC�̎������擾	
	int retc = send_mail( KS_SLAVE, "*", &que );	
	if( 0 != retc ) syslog(SYSNO_MAIL_SNED_ERR, "[%d:�����ύX�v�� <err=%d>]", FACT_KS_SLAVE_01, retc);
}


//------------------------------------------
// �R�}���h���s
// char const* cIniKey	AAA_%s	��%s�ɑ������镔��
//------------------------------------------
void MainInctance::Exec_Cmd(char const* cIniKey)
{
	CString		sKey;
	char		cWk[1024];


	//// �R�}���h���s
	int			nNum = 0;		// ���s��
	while(true) {
		sKey.Format("CMD_EXEC_%s_%d", cIniKey, nNum+1); 	
		GetPrivateProfileString(KS_MASTER,	sKey, "", cWk, sizeof(cWk), PCINI_NAME); 
		if( 0 == strlen(cWk)) break;


		// �v���Z�X�N��
		int nRetc = system( cWk );
		if( -1 == nRetc ) {
			LOG(em_ERR), "[%s]���̃R�}���h���s���s(%d)(%s)", cIniKey, nRetc, cWk);
		} else {
			LOG(em_MSG), "[%s]���̃R�}���h���s����(%d)(%s)", cIniKey, nRetc, cWk);
		}

		// ���ꂾ�ƁA�R�}���h��������̂Ń���
		//STARTUPINFO si;
		//PROCESS_INFORMATION pi;
		//GetStartupInfo(&si);
		//si.dwFlags = 0;

		//CreateProcess(NULL, (LPSTR)cWk, NULL, NULL, FALSE, NORMAL_PRIORITY_CLASS, NULL, NULL, &si, &pi);
		//CreateProcess(NULL, (LPSTR)cWk, NULL, NULL, FALSE, NORMAL_PRIORITY_CLASS, NULL, NULL, &si, &pi);
		//CloseHandle(pi.hProcess);	// �v���Z�X�n���h���N���[�Y
		//CloseHandle(pi.hThread);	// �v���Z�X�E�X���b�h�E�n���h���N���[�Y

		nNum ++;
	}

	//// �P�ł����s�ł����Ƃ��̂�
	if( 0 != nNum ) {
		sKey.Format("CMD_ENDWAIT_%s", cIniKey); 	
		int nWait = GetPrivateProfileInt(KS_MASTER, sKey, 0, PCINI_NAME);
		nWait *= 1000;		// [s]��[ms]
		if(0 != nWait) Sleep(nWait);
	
		LOG(em_MSG), "[%s]���̃R�}���h���s����", cIniKey);
	}
}
