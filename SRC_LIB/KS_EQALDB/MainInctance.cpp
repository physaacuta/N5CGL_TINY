#include "StdAfx.h"
#include "MainInctance.h"

// �������[���[�N���o ----->>>
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif
// <<<------------------------

//------------------------------------------
// �R���X�g���N�^
//------------------------------------------
MainInctance::MainInctance(void) :
mcls_pLog(NULL)
{
	// �V�O�i������
	m_evThStart  = CreateEvent(NULL, FALSE, FALSE, NULL);				// ���C���X���b�h�����p


	//// �e�N���X�֘A������
	memset(g_nClassCnt, 0x00, sizeof(g_nClassCnt));
	for(int ii=0; ii<KizuSyncroManager::PARAM_MAX_THREAD; ii++) {
		gcls_pSync[ii] = NULL;
	}
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
	int ii, jj;

	//// ���C���C���X�^���X�̂݁A�X���b�h�����͍���Ă��� (�����ɔ�V�O�i���Ƃ���)
	//// �p�����N���X���s (���C������X���b�h�ׁ̈A������ƗD��x����)
	WorkThread::Start(MainThread, THREAD_PRIORITY_ABOVE_NORMAL );

	// �X���b�hID��������܂ł�����Ƌx�e
	while( 0==GetTid() ) {
		LOG(em_MSG), "[MainInctance] �X���b�hID���蓖�Ē��E�E�E");
		Sleep(1);			// �X���b�h�N���܂ł�����Ƒ҂�
	}



	//// ini�t�@�C���ǂݍ��� 
	int		nCnt = 1;
	int		nType;				// �Z�b�V�����̃^�C�v
	CString	cSession;			// �Z�b�V����

	char    cWk[1024];
	CString strVal;

	while(true) {
		nType = -1;
		cSession = "";

		strVal.Format("SESSION_%d", nCnt);
		GetPrivateProfileString(KS_EQALDB, strVal, "", cWk, sizeof(cWk), TASKINI_NAME);		
	
		if( 0 < strlen(cWk) ) {
			strVal = cWk;
			
			//// �g�[�N������
			int		rec = 0;	
			int		curPos = 0;
			CString strToken = strVal.Tokenize(",", curPos);
			while ( strToken != "" ) {
				strToken.Trim(_T(" 	"));		// ini�t�@�C���ݒ蕶���̑O��̃X�y�[�X�A�^�u����菜��

				switch( rec ) {
				case 0:
					cSession = strToken;
					break;

				case 1:
					nType = atoi(strToken); 
					break;
				}

				// ����
				strToken = strVal.Tokenize(",", curPos);
				rec ++;
			}
			if( 2 != rec || -1 == nType || 0 == cSession.GetLength() ) break;		// ���������g�[�N�����L���H
			if( KizuSyncroManager::PARAM_MAX_THREAD <= g_nClassCnt[nType] ) break;

			//// ����擾������񂩂�N���X�𐶐�
			if		(TYPE_DB_SYNC == nType)		gcls_pSync[ g_nClassCnt[nType] ] = new SyncManager(cSession, mcls_pLog);
			else if	(TYPE_DB_COPY == nType)		gcls_pCopy[ g_nClassCnt[nType] ] = new CopyManager(cSession, mcls_pLog);
			else if	(TYPE_DB_MOVE == nType)		gcls_pMove[ g_nClassCnt[nType] ] = new MoveManager(cSession, mcls_pLog);
			else if	(TYPE_DB_SYNCOPY == nType)	gcls_pSynCopy[ g_nClassCnt[nType] ] = new SyncopyManager(cSession, mcls_pLog);

			g_nClassCnt[nType] ++;

			// ����
			nCnt++;

		} else {
			break;
		}
	}

	
	//// �����N�T�[�o�[��`
	nCnt = 1;
	while(true) {
		strVal.Format("LINKSRV_%d", nCnt);
		GetPrivateProfileString(strVal, "SELECT_ODBC_KEY", "", cWk, sizeof(cWk), TASKINI_NAME);		
		if( 0 >= strlen(cWk) ) break;

		AddLinkServer(strVal);		// �����N�T�[�o�[��`

		// ����
		nCnt++;
	}


	//// �g���₷���悤�Ɋe�N���X�̎Q�Ƃ��Z�b�g���Ă���
	for(ii=0; ii<KizuSyncroManager::PARAM_MAX_THREAD; ii++) {
		if(NULL != gcls_pSync[ii] ) gcls_pDb[TYPE_DB_SYNC][ii] = gcls_pSync[ii];
		else						gcls_pDb[TYPE_DB_SYNC][ii] = NULL;

		if(NULL != gcls_pCopy[ii] ) gcls_pDb[TYPE_DB_COPY][ii] = gcls_pCopy[ii];
		else						gcls_pDb[TYPE_DB_COPY][ii] = NULL;

		if(NULL != gcls_pMove[ii] ) gcls_pDb[TYPE_DB_MOVE][ii] = gcls_pMove[ii];
		else						gcls_pDb[TYPE_DB_MOVE][ii] = NULL;

		if(NULL != gcls_pSynCopy[ii] ) gcls_pDb[TYPE_DB_SYNCOPY][ii] = gcls_pSynCopy[ii];
		else						   gcls_pDb[TYPE_DB_SYNCOPY][ii] = NULL;
	}

/*
	//// ���[�J�[�X���b�h ������
	for(ii=0; ii<TYPE_DB_END; ii++) {
		for(jj=0; jj<g_nClassCnt[ii]; jj++) {
//			gcls_pDb[ii][jj]->SetLogMgr(mcls_pLog); 
		}
	}
*/

	//// ���[�J�[�X���b�h �N��
	for(ii=0; ii<TYPE_DB_END; ii++) {
		for(jj=0; jj<g_nClassCnt[ii]; jj++) {
			gcls_pDb[ii][jj]->Start(); 
		}
	}

	return TRUE;
}

//------------------------------------------
// �I������ (��x�̂݋N��)
//------------------------------------------
void MainInctance::Exit()
{
	int ii, jj;

	//// ���[�J�[�X���b�h ��~�ʒm
	for(ii=0; ii<TYPE_DB_END; ii++) {
		for(jj=0; jj<g_nClassCnt[ii]; jj++) {
			gcls_pDb[ii][jj]->SetStop(); 
		}
	}
	//// ���[�J�[�X���b�h ��~
	for(ii=0; ii<TYPE_DB_END; ii++) {
		for(jj=0; jj<g_nClassCnt[ii]; jj++) {
			gcls_pDb[ii][jj]->Stop(); 
		}
	}

	//// ���[�J�[�X���b�h�J��
	for(ii=0; ii<TYPE_DB_END; ii++) {
		for(jj=0; jj<g_nClassCnt[ii]; jj++) {
			SAFE_DELETE( gcls_pDb[ii][jj] );
		}
	}
}


//------------------------------------------
// �������s��U�蕪��
//------------------------------------------
void MainInctance::SelectNowExec(int nAtoB, int nMailKey)
{
	int ii, jj;

	for(ii=0; ii<TYPE_DB_END; ii++) {
		for(jj=0; jj<g_nClassCnt[ii]; jj++) {
			gcls_pDb[ii][jj]->SetEvNowExec(nAtoB, nMailKey); 
		}
	}
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

		case WM_USER_TH_END: 					// �X���b�h�I���v��
			pLOG(em_MSG), "[MainInctance] �X���b�h�I�� [0x%X]", p->GetTid());
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
// �����N�T�[�o�[��ݒ�
// char const* cKey ��`�ݒ�L�[
//------------------------------------------
void MainInctance::AddLinkServer(char const* cKey)
{
	char	cOdbc[64];
	char	cDbOnLink[64];
	char	cDbOnAddr[64];
	char	cDbOnDsn [64];
	char	cDbOnUser[64];
	char	cDbOnPass[64];
	char	cSrvproduct[64];
	char	cProvider[64];
						
	// ini�t�@�C����`�擾
	GetPrivateProfileString(cKey, "SELECT_ODBC_KEY", "", cOdbc, sizeof(cOdbc), TASKINI_NAME);		
	GetPrivateProfileString(cKey, "LINK_NAME", "",	cDbOnLink, sizeof(cDbOnLink), TASKINI_NAME);
	GetPrivateProfileString(cKey, "DBADDLESS", "",	cDbOnAddr, sizeof(cDbOnAddr), TASKINI_NAME);		
	GetPrivateProfileString(cKey, "DSNAME", "",		cDbOnDsn, sizeof(cDbOnDsn), TASKINI_NAME);		
	GetPrivateProfileString(cKey, "USERID", "",		cDbOnUser, sizeof(cDbOnUser), TASKINI_NAME);		
	GetPrivateProfileString(cKey, "PASSWORD", "",	cDbOnPass, sizeof(cDbOnPass), TASKINI_NAME);		
	GetPrivateProfileString(cKey, "SRVPRODUCT", "", cSrvproduct, sizeof(cSrvproduct), TASKINI_NAME);		
	GetPrivateProfileString(cKey, "PROVIDER", "",	cProvider, sizeof(cProvider), TASKINI_NAME);



	// �ǉ�������DB�ɐڑ�
	OdbcBase	clsDB;
	if( ! clsDB.Connect(TASKINI_NAME, cOdbc)) {
		// �f�[�^�x�[�X�ڑ��G���[
		LOG(em_ERR), "[MainInctance] [%s] DB�ڑ��G���[", cOdbc);
		syslog(SYSNO_DB_CONNECT_ERR, "[MainInctance] [%s]", cOdbc);
		return;
	}

	// �����N�T�[�o�[�ǉ�
	if( ! clsDB.ExecuteDirect_AddLinkServer(
		cDbOnLink, cDbOnAddr, cDbOnDsn, cDbOnUser, cDbOnPass, true, cSrvproduct, cProvider) ){
		LOG(em_ERR), "[MainInctance] �����N�T�[�o�[�ǉ����s");
		return;
	}

	LOG(em_MSG), "[MainInctance] �����N�T�[�o�[�ǉ����� [%s] [%s:%s:%s]", cOdbc, cDbOnLink, cDbOnAddr, cDbOnDsn);
}
