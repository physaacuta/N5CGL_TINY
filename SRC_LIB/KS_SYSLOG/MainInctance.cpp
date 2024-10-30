#include "StdAfx.h"
#include "MainInctance.h"

//------------------------------------------
// �R���X�g���N�^
//------------------------------------------
MainInctance::MainInctance(void) :
m_bSyslogDsp(false),
m_nDay(0),
m_nAutoNo(0),
m_nOdbcID(0),
mque_AddDB(512),
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

	// AutoNo�ɕK�v�ȃf�[�^�������Z�b�g����
	InitAutoNo();

	//// ���[�J�[�X���b�h ������
	// DB�����݃N���X
	gcls_pDb = new DBSaveManager(0, &mque_AddDB);
	gcls_pDb->SetLogMgr(mcls_pLog);
	gcls_pDb->SetMainPostTid(GetTid());

	//// ���[�J�[�X���b�h �N��
	gcls_pDb->Start();

	return TRUE;
}

//------------------------------------------
// �I������ (��x�̂݋N��)
//------------------------------------------
void MainInctance::Exit()
{
	//// ���[�J�[�X���b�h ��~
	gcls_pDb->Stop();

	//// ���[�J�[�X���b�h �J��
	SAFE_DELETE(gcls_pDb);
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
			pLOG(em_INF), "[MainInctance] ���݂�AutoNo=%d", p->m_nAutoNo);
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
// Syslog���X���b�h�L���[�ɓo�^
// COMMON_QUE const* que ���[���X���b�g�f�[�^
//------------------------------------------
void MainInctance::AddSysLogSqlQue(COMMON_QUE const* que)
{
	// SQL���쐬
	CString cSql;
	CTime t = CTime::GetCurrentTime();

	// ���t�ɂ��I�[�gNo���Z�b�g
	RefrashAutoNo(t.GetDay());

	// SQL������
	cSql.Format( "INSERT INTO %s VALUES('%d/%d/%d %d:%d:%d', '%s', '%s', %d, %d, '%s', '%s', %d)",
					DB_LOG_INF,
					que->syslog_que.date[0],	que->syslog_que.date[1], que->syslog_que.date[2], 
					que->syslog_que.date[3],	que->syslog_que.date[4], que->syslog_que.date[5],
					que->syslog_que.cTaskName,	que->syslog_que.cPcName, que->syslog_que.nPcKind,
					que->syslog_que.nSysNo,		que->syslog_que.cMsg,
					t.Format("%Y/%m/%d %H:%M:%S"), (m_nAutoNo*10 + m_nOdbcID)
	);

	// �o�^�\�H
	if( mque_AddDB.GetFreeCount() < 10 ) {
		// ���t
		LOG(em_ERR), "[MainInctance] �L���[�t���I�I [%s]", cSql );
	} else {
		// �f�[�^�Z�b�g
		DBSaveManager::DeliverySQL * pDeli = new DBSaveManager::DeliverySQL();
		pDeli->strSql = cSql;
		pDeli->addmode	= false;
		pDeli->cnt = 0;

		// �o�^
		mque_AddDB.AddToTail(pDeli); 
	}

	// ���O�o��
	if(m_bSyslogDsp) {
		LOG(em_NON), "[%04d/%02d/%02d %02d:%02d:%02d] %04d  %s \t %s \t PCKIND=%02d | %s ",
					que->syslog_que.date[0],	que->syslog_que.date[1],	que->syslog_que.date[2], 
					que->syslog_que.date[3],	que->syslog_que.date[4],	que->syslog_que.date[5],
					que->syslog_que.nSysNo,		que->syslog_que.cTaskName,	que->syslog_que.cPcName,
					que->syslog_que.nPcKind,	que->syslog_que.cMsg );
	}
}

//------------------------------------------
// Statelog���X���b�h�L���[�ɓo�^
// COMMON_QUE const* que ���[���X���b�g�f�[�^
//------------------------------------------
void MainInctance::AddStateLogSqlQue(COMMON_QUE const* que)
{
	// SQL���쐬
	CString cSql;
	CTime t = CTime::GetCurrentTime();

	// ���t�ɂ��I�[�gNo���Z�b�g
	RefrashAutoNo(t.GetDay());

	// SQL������
	cSql.Format( "INSERT INTO %s(�������t����,������,PC��,PCKIND,�A���[��No,���e,�o�^���t,AutoNo) SELECT '%d/%d/%d %d:%d:%d', '%s', '%s', %d, %d, '<'+ �@�햼 +'> [���l:'+ �ُ�MSG +'] %s (%d)', '%s', %d FROM %s WHERE �@��ID=%d",
					DB_LOG_INF,
					que->statelog_que.date[0],		que->statelog_que.date[1], que->statelog_que.date[2], 
					que->statelog_que.date[3],		que->statelog_que.date[4], que->statelog_que.date[5],
					que->statelog_que.cTaskName,	que->statelog_que.cPcName, que->statelog_que.nPcKind,
					que->statelog_que.nSysNo,		
					que->statelog_que.cMsg,			que->statelog_que.nStateVal,
					t.Format("%Y/%m/%d %H:%M:%S"),	(m_nAutoNo*10 + m_nOdbcID),
					DB_STATUS_MASTER,				que->statelog_que.nStateId 
	);


	// �o�^�\�H
	if( mque_AddDB.GetFreeCount() < 10 ) {
		// ���t
		LOG(em_ERR), "[MainInctance] �L���[�t���I�I [%s]", cSql );
	} else {
		// �f�[�^�Z�b�g
		DBSaveManager::DeliverySQL * pDeli = new DBSaveManager::DeliverySQL();
		pDeli->strSql = cSql;
		pDeli->addmode	= false;
		pDeli->cnt = 0;
		// �o�^
		mque_AddDB.AddToTail(pDeli); 
	}

	// ���O�o��
	if(m_bSyslogDsp) {
		LOG(em_NON), "[%04d/%02d/%02d %02d:%02d:%02d] %04d  %s \t %s \t PCKIND=%02d | %s<�@��ID=%d> %s (%d)",
					que->statelog_que.date[0],	que->statelog_que.date[1],		que->statelog_que.date[2], 
					que->statelog_que.date[3],	que->statelog_que.date[4],		que->statelog_que.date[5],
					que->statelog_que.nSysNo,	que->statelog_que.cTaskName,	que->statelog_que.cPcName,
					que->statelog_que.nPcKind,	
					"�@���ԕύX",				que->statelog_que.nStateId,		que->statelog_que.cMsg,
					que->statelog_que.nStateVal	 );
	}
}
/*
//------------------------------------------
// ���t���`�F�b�N���āA�����ς��΁A�I�[�gNo�����Z�b�g����B
// int nDay �o�^��
//------------------------------------------
void MainInctance::ResetAutoNo(int nDay)
{
	CString cSql;					// SQL��

	// �����ύX���ꂽ�H
	if(m_nDay == nDay) return;
	
	// �I�[�gNo�^�����Z�b�g
	cSql.Format("DBCC CHECKIDENT(%s, RESEED, 0)", DB_LOG_INF);

	// ��Γo�^
	DBSaveManager::DeliverySQL * pDeli = new DBSaveManager::DeliverySQL();
	pDeli->strSql = cSql;
	pDeli->addmode	= false;
	pDeli->cnt = 0;
	if( ! mque_AddDB.AddToTail(pDeli, 20, 30) ) {
		LOG(em_ERR), "[MI] �I�[�gNo�^�����Z�b�g �L���[�t���I�I" );
		SAFE_DELETE(pDeli);
		// �ُ�̏ꍇ�ł��A�����o�[�ϐ��̏����������Ă���!!
	}


	// �Z�b�g
	LOG(em_MSG), "[MI] �I�[�gNo���Z�b�g �ύX��[%d��%d]", m_nDay, nDay);
	m_nDay = nDay;
}
*/
//------------------------------------------
// AutoNo�ɕK�v�ȃf�[�^�������Z�b�g����
//------------------------------------------
void MainInctance::InitAutoNo()
{
	//// ���ݓ����Z�b�g
	m_nDay = CTime::GetCurrentTime().GetDay(); 

	//// ODBC�̐ݒ�ʒu���擾����
	// ���݂̃Z�b�V�������擾����
	char Session[128];
	GetPrivateProfileString("DB", "SESSION", "", (char *)Session, sizeof(Session), PCINI_NAME);
	// �ʒu����
	char SessionWK[128];
	CString		cKey;
	for(int ii=1; ii<10; ii++) {
		cKey.Format("ODBCSET_%d", ii); 
		GetPrivateProfileString("DB", cKey, "", SessionWK, sizeof(SessionWK), PCINI_NAME);
		if(0 == strcmp(Session, SessionWK)) {
			m_nOdbcID = ii;
			break;
		}
	}


	//// �ŐV���t��AutoNo���擾����
	CString cSql;					// SQL��

	// SQL��
	cSql.Format("SELECT TOP(1) AutoNo FROM %s ORDER BY �o�^���t desc, AutoNo desc", DB_LOG_INF);

	// ���s
	int nWk;
	if( ! KizuODBC::OdbcBase::GetSelectKey_Static(cSql, &nWk, TASKINI_NAME, INI_DB) ) {
		LOG(em_ERR), "[MI] �I�[�gNo �ŐV�l �擾���s�I�I" );
		m_nAutoNo = 0;
	} else {
		m_nAutoNo = nWk / 10;
		LOG(em_INF), "[MI] �I�[�gNo �ŐV�l DB��AutoNo=%d, ���ݓ��t=%d (�A��=%d, ID=%d)", nWk, m_nDay, m_nAutoNo, m_nOdbcID);
	}
}

//------------------------------------------
// ���t���`�F�b�N���āA�����ς��΁A�I�[�gNo�����Z�b�g����B
// int nDay �o�^��
//------------------------------------------
void MainInctance::RefrashAutoNo(int nDay)
{
	// �����ύX���ꂽ�H
	if(m_nDay == nDay) {
		// �������Ȃ� AutoNo���X�V���邾��
		m_nAutoNo ++;
		return;
	}

	// �Z�b�g
	LOG(em_MSG), "[MI] �I�[�gNo���Z�b�g �ύX��[%d��%d]", m_nDay, nDay);
	m_nDay	  = nDay;
	m_nAutoNo = 1;
}