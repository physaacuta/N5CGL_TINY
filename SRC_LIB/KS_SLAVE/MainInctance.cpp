#include "StdAfx.h"
#include "MainInctance.h"


//------------------------------------------
// �R���X�g���N�^
//------------------------------------------
MainInctance::MainInctance(void) :
m_bInitalize(false),
m_bStateMaster(false),
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
	//// PC�}�X�^�[�Ǘ��`�F�b�N�L��
	int nWk = GetPrivateProfileInt(KS_SLAVE, "DEBUG", 0, PCINI_NAME);
	if( 0 != nWk && 1 != nWk && 2 != nWk ) nWk = 0;
	m_nDebug = nWk;

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
	gcls_pCSta = new CheckStatus(false);
	gcls_pCSta->SetLogMgr(mcls_pLog);
	gcls_pCSta->SetMainPostTid(GetTid());

	// �f�[�^�x�[�X�Ǘ��X���b�h
	gcls_pCDb = new CheckDB(false);
	gcls_pCDb->SetLogMgr(mcls_pLog);
	gcls_pCDb->SetMainPostTid(GetTid());

	//// ���[�J�[�X���b�h �N��
	if(2 == m_nDebug) {
		//PostThreadMessage(GetTid(), E_DEF_DB_FAST_CONNECT, 0, 0);
		gcls_pCDb->SetFastExecDebug();	// ����OK���
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
	if(2 != m_nDebug ) {
		gcls_pCDb->Stop();
	}

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
	COMMON_QUE que;
	UINT_PTR tId_Check = 0;					// ����� �}�X�^�[�m�F�^�C�}�[ID
	int	nMasterRecvCnt = 0;					// PC�}�X�^�[�Ǘ�����̗����グ�����ʒm��M��

	////// ���C���C���X�^���X���o��
	MainInctance *p = (MainInctance *)param;						// ���N���X�̃C���X�^���X���擾
	PeekMessage(&msg, NULL, WM_USER, WM_USER, PM_NOREMOVE);			// ���b�Z�[�W�쐬
	p->SetTid();													// �X���b�hID�Z�b�g

	////// �����[�N�X���b�h�̏����������܂őҋ@
	pLOG(em_MSG), "[MainInctance] �����[�N�X���b�h�̏����������܂őҋ@���E�E�E [0x%X]", p->GetTid() );
	WaitForSingleObject(p->m_evThStart, INFINITE);
	
	//// ���[�J���^�C�}�[�n��
	SAFE_TIMER_SET(tId_Check,	  PARAM_TIMEID_CHECK,	  PARAM_TIMEOUT_CHECK);		// ����� �}�X�^�[��Ԗ₢���킹�^�C�}�[

	////// ���b�Z�[�W�L���[�ǂݍ���
	pLOG(em_MSG), "[MainInctance] �X���b�h�J�n [0x%X]", p->GetTid());
	while( int ret = GetMessage(&msg, NULL, 0, 0) ){
		_ASSERT( -1 != ret );
		switch (msg.message) {

//-----------------------------------------------------------------------------------------------
		case WM_TIMER:							// �^�C�}�[�N��
			if( msg.wParam == tId_Check ) {						// ����� �}�X�^�[�m�F�^�C�}�[
				// PC�}�X�^�[�Ǘ��֏�Ԗ₢���킹
				que.nEventNo = FACT_KS_MASTER_03;
				que.nLineNo = getlineid();															// ���C��ID
				strcpy(que.fc.data, getmypcname() );  
				int retc = send_mail( KS_MASTER, MASTER_PC_NAME, &que );		
				if( 0 != retc ) syslog(SYSNO_MAIL_SNED_ERR, "[%d:PC�}�X�^�[�Ǘ��֏�Ԗ₢���킹 <err=%d>]", FACT_KS_MASTER_03, retc);
				pLOG(em_MSG), "PC�}�X�^�[�Ǘ��֏�Ԗ₢���킹�����(%s)", getmypcname());
				
				// �f�o�b�N�p
				if( 0 != p->m_nDebug ) {
					memset( &que, 0x00, sizeof(que));
					que.nEventNo = FACT_KS_SLAVE_03;
					que.nLineNo = getlineid();
					send_mail( getmytaskname(), ".", &que );
				}
			}
			break;

//-----------------------------------------------------------------------------------------------
		case E_DEF_DB_FAST_CONNECT:				// DB�����ڑ������ʒm
			//// �C�j�V�������ҋ@��ʂ�DB�ڑ������ʒm���M
			que.nEventNo = FACT_KS_WAKEUP_02; 
			que.nLineNo = getlineid();															// ���C��ID
			send_mail( KS_WAKEUP, ".", &que );	

			// DB, PC�}�X�^�[�Ǘ� ����������̎��̂�
			if( ! p->gcls_pCDb->GetFastExec() && p->m_bStateMaster ) {
				p->Init_Process();							// �v���Z�X����
			}
			break;

//-----------------------------------------------------------------------------------------------
		case E_DEF_ML_MASTEROK:					// PC�}�X�^�[�Ǘ������グ�����ʒm
			SAFE_TIMER_KILL(tId_Check);					// PC�}�X�^�[�Ǘ�OK�ׁ̈A�^�C�}�[�I��
			nMasterRecvCnt ++;

			// �O�ׂ̈ɂ�����x�}�X�^�[�̊m�F (NEC�̃p�\�R���ɂ��Ă���A�}�X�^�[�����[���𐳏�Ɏ�M�o���Ȃ��������������߂̏��u)
			if( 1 == nMasterRecvCnt ) {
				// 1���
				SAFE_TIMER_SET(tId_Check,	  PARAM_TIMEID_CHECK,	  PARAM_TIMEOUT_CHECK2);		// ����� �}�X�^�[��Ԗ₢���킹�^�C�}�[
				pLOG(em_MSG), "�����グ������ ��t1��ڂȂ̂ŁA�ēx�₢���킹���{�J�n");
				// �f�o�b�N�p
				if( 0 != p->m_nDebug ) {
					memset( &que, 0x00, sizeof(que));
					que.nEventNo = FACT_KS_SLAVE_03;
					que.nLineNo = getlineid();
					send_mail( getmytaskname(), ".", &que );
				}

			} else {
				if( ! p->m_bStateMaster ) {
					//// PC�}�X�^�[�Ǘ������オ�芮���ς�
					p->m_bStateMaster = true;

					//// �C�j�V�������ҋ@��ʂ�PC�}�X�^�[�Ǘ������グ�����ʒm���M
					que.nEventNo = FACT_KS_WAKEUP_01; 
					que.nLineNo = getlineid();															// ���C��ID
					send_mail( KS_WAKEUP, ".", &que );


					// DB, PC�}�X�^�[�Ǘ� ����������̎��̂�
					if( ! p->gcls_pCDb->GetFastExec() ) {
						p->Init_Process();							// �v���Z�X����
					}

				} else {
					// ������ �����グ�����ʒm���M
					p->Send_MASTER_OK();
				}
			}
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
	LOG(em_MSG), "Init_Process�J�n");
	Sleep(1000);					// DB���N��������̓���s������Ȃ����ׂɂ�����Ƒ҂��B

	COMMON_QUE que;
	if(m_bInitalize) return;		// 2�^�C�~���O���邩��O�ׂ̈� �������m�F
	m_bInitalize = true;

//	//// �t�@�C���R�s�[
//	ExecFileCopy();


	//// �R�}���h���s
	Exec_Cmd( "START" );


	//// �SPC���̂�DB���擾
	if(2 != m_nDebug ) {
		if( getpcname() ) {
			LOG(em_MSG), "�SPC���̂�DB���擾");
		} else {
			LOG(em_ERR), "�SPC���̂�DB���擾���s");		// �����ŃG���[�͂��肦�Ȃ��͂�
		}
	}

	//// �@���ԊǗ��N���X�̏�����
	gcls_pCSta->Alloc();

	//// ���L����������
	mcls_Smem.CreateMem(true); 
	LOG(em_MSG), "���L��������������");

	//// �풓�v���Z�X�N��
	ProcessManager::ExecProcess(); 
	LOG(em_MSG), "�풓�v���Z�X�N������");
	
	//// �@���ԊǗ��N���X�N��
	gcls_pCSta->Start();

	//// PC�}�X�^�[�Ǘ��� �����グ������
	Send_MASTER_OK();

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
	que.fl.data[0] = GetPrivateProfileInt(KS_SLAVE, "APP_ENABLE_ID", 0, PCINI_NAME);
	que.fl.data[1] = GetPrivateProfileInt(KS_SLAVE, "HB_SEND_ENABLE", 0, PCINI_NAME);
	int retc = send_mail( KS_MASTER, MASTER_PC_NAME, &que );
	if( 0 != retc ) syslog(SYSNO_MAIL_SNED_ERR, "[%d:�������������� <err=%d>]", FACT_KS_MASTER_05, retc);
}


//------------------------------------------
// ������ �����グ�����ʒm �A���T�[
//------------------------------------------
void MainInctance::Send_MASTER_OK()
{
	COMMON_QUE que;

	//// PC�}�X�^�[�Ǘ��� �����グ������
	que.nEventNo = FACT_KS_MASTER_04; 
	que.nLineNo = getlineid();															// ���C��ID
	que.fl.data[0] = GetPrivateProfileInt(KS_SLAVE, "APP_ENABLE_ID", 0, PCINI_NAME);
	que.fl.data[1] = GetPrivateProfileInt(KS_SLAVE, "HB_SEND_ENABLE", 0, PCINI_NAME);
	int retc = send_mail( KS_MASTER, MASTER_PC_NAME, &que );	
	if( 0 != retc ) syslog(SYSNO_MAIL_SNED_ERR, "[%d:�����グ������ <err=%d>]", FACT_KS_MASTER_04, retc);
	LOG(em_MSG), "�����グ������");
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
		GetPrivateProfileString(KS_SLAVE,	sKey, "", cWk, sizeof(cWk), PCINI_NAME); 
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
		int nWait = GetPrivateProfileInt(KS_SLAVE, sKey, 0, PCINI_NAME);
		nWait *= 1000;		// [s]��[ms]
		if(0 != nWait) Sleep(nWait);
	
		LOG(em_MSG), "[%s]���̃R�}���h���s����", cIniKey);
	}
}




/*
//------------------------------------------
// �t�@�C���R�s�[���s
//------------------------------------------
void MainInctance::ExecFileCopy()
{
	char		cFrom[MAX_PATH];		// �R�s�[��
	char		cTo[MAX_PATH];			// �R�s�[��
	CString		cKey;

	// �R�s�[���s
	int cnt = 1;
	while(true) {
		cKey.Format("F_%d", cnt); 
		GetPrivateProfileString("FILECOPY", cKey, "", cFrom, MAX_PATH, TASKINI_NAME);
		cKey.Format("T_%d", cnt); 
		GetPrivateProfileString("FILECOPY", cKey, "", cTo,   MAX_PATH, TASKINI_NAME);

		// �L��
		if(0 == strlen(cFrom) || 0 == strlen(cTo)) break;

		// �R�s�[
		if( ! CopyFile(cFrom, cTo, FALSE) ) {
			LOG(em_ERR), "[FILECOPY] [%s��%s]", cFrom, cTo);
			return;
		}
		cnt ++;
	}
}
*/