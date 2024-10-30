// DBManager.cpp: DBManager �N���X�̃C���v�������e�[�V����
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "DBSaveManager.h"

// static ����
CRITICAL_SECTION	DBSaveManager::m_csLock;
DWORD				DBSaveManager::m_nEndLockNo = 0;
DWORD				DBSaveManager::m_nPushLockNo= 0;


//////////////////////////////////////////////////////////////////////
// �\�z/����
//////////////////////////////////////////////////////////////////////
//------------------------------------------
// �R���X�g���N�^
// int myID ������ID (0�n�܂�)
// ThreadQueue<DeliverySQL>* que_pDeli �󂯓n���p�X���b�h�L���[
//------------------------------------------
DBSaveManager::DBSaveManager(int myID, ThreadQueue<DeliverySQL>* que_pDeli) :
m_nMyID(myID),
mque_pDeli(que_pDeli),
m_bDBConnect(false),
m_bDBDelete(true),
m_bSendDbManager(false),
m_bReTry(false), 
m_bReTryExecOk(false),
m_cSession(""),
m_evSynclock(NULL),
mcls_pLog(NULL)
{
	//// �V�O�i������
	m_evReTryConnect = CreateWaitableTimer(NULL, FALSE, NULL);
	m_evReSet		 = CreateEvent(NULL, FALSE, FALSE, NULL);


	//// ODBC�ڑ��N���X �C���X�^���X����
	mcls_pDB = new OdbcBase();


	// �����ۏ�
	if(0 == m_nMyID) {
		InitializeCriticalSection(&m_csLock);
	}
}

//------------------------------------------
// �f�X�g���N�^
//------------------------------------------
DBSaveManager::~DBSaveManager()
{
	m_bDBConnect = false;

	// �V�O�i���J��
	CloseHandle(m_evReTryConnect);
	CloseHandle(m_evReSet);

	// ODBC�ڑ��N���X �J��
	mcls_pDB->DisConnect();
	SAFE_DELETE(mcls_pDB);

	// �����ۏ�
	if(0 == m_nMyID) {
		DeleteCriticalSection(&m_csLock);
	}
}

//------------------------------------------
// �X���b�h
//------------------------------------------
UINT WINAPI DBSaveManager::MainThread(LPVOID param)
{
	WORD ii;
	bool bCalcTime;
	DBSaveManager::DeliverySQL *pDeli;				// �L���[�f�[�^

	////// ���C���C���X�^���X���o��
	DBSaveManager *p = (DBSaveManager *)param;
	p->SetTid();								// ���X���b�hID�Z�b�g

	// WaitForMultipleObjects�C�x���g
	HANDLE	hArray[] = {p->my_evStop,
						p->m_evReTryConnect,
						p->m_evReSet,
						p->mque_pDeli->g_evSem 
						};
	enum {	EV_STOP = WAIT_OBJECT_0,		// �X���b�h�I��
			EV_RETRY,						// �Đڑ��v���^�C�}�C�x���g
			EV_RESET,						// DB���Z�b�g�v��
			EV_QUE,							// �L���[�C�x���g
			EV_END	};

	//// ���x����p
	p->mcls_Counter.InitAve();

	//// DB�Ǘ��ɒʒm
	p->Send_To_DbManage();

	//// DB�����ڑ�
	p->ConnectDB(true);

	// ���b�Z�[�W�L���[�ǂݍ���
	pLOG(em_MSG), "[DBSaveManager] <%d> �X���b�h�J�n [0x%X]", p->m_nMyID, p->GetTid());
	while (1) {
		DWORD waitnum = ( p->m_bDBConnect==true ? EV_END : 2);
		DWORD ret = WaitForMultipleObjects(waitnum, hArray, FALSE, INFINITE);

		switch (ret) {

//-----------------------------------------------------------------------------------------------
		case EV_STOP:												// �X���b�h�I��
			pLOG(em_MSG), "[DBSaveManager] <%d> �X���b�h�I�� [0x%X]", p->m_nMyID, p->GetTid());
			// �҂��f�[�^�͊J�����Ă���
			if(0==p->m_nMyID) {
				int nCnt = p->mque_pDeli->GetCount();		// �c��
				if(0 != nCnt) pLOG(em_WAR), "[DBSaveManager] <%d> SQL�������J�� %d��", p->m_nMyID, nCnt);

				while(true) {
					pDeli = p->mque_pDeli->GetFromHead(0);
					if( pDeli == NULL ) break;
					// �K�{�o�^�H
					if( pDeli->addmode ) p->sqlExecute(pDeli);
					
					// �J��
					for(ii=0; ii< pDeli->cnt; ii++) {
						SAFE_DELETE_ARRAY(pDeli->pImg[ii]);
					}
					SAFE_DELETE(pDeli->SendMailItem);
					delete pDeli;
				}
		
				// DB�폜�Ǘ��ɒʒm
				p->Send_To_DbManage();
			}
			return 0;

//-----------------------------------------------------------------------------------------------
		case EV_RETRY:												// �Đڑ��v���^�C�}�C�x���g
		case EV_RESET:												// DB���Z�b�g�v��
			// ��x�ؒf����
			p->CloseDB();
			// �ڑ�
			p->ConnectDB(false);										// DB�����ڑ�
			break;
		
//-----------------------------------------------------------------------------------------------
		case EV_QUE:												// SQL���s
			// �f�[�^�擾
			pDeli = p->mque_pDeli->GetFromHead();							// �Z�}�t�H�[���Z�����o�[�W������
			if(NULL == pDeli) break;

			// �����ۏ�`�F�b�N
			if( pDeli->bIsLock ) {
				while(true) {
					DWORD no = p->ReadEndLockNo();
					if( no+1 == pDeli->nLockNo ) break;
					pLOG(em_INF), "[DBSaveManager] <%d> ���s���ԑ҂��I(��=%d �v��=%d)", p->m_nMyID, no, pDeli->nLockNo);
					Sleep(60);
				}
			}


			// ���x����p
			bCalcTime = pDeli->bCalcTime;
			if(bCalcTime) p->mcls_Counter.CounterStart();
			// �o�^
			p->sqlExecute(pDeli);


			// �����ۏኮ��
			if( pDeli->bIsLock ) {
				pLOG(em_INF), "[DBSaveManager] <%d> �����ۏ�SQL���s���� (%d��%d)", p->m_nMyID, m_nEndLockNo, pDeli->nLockNo);
				m_nEndLockNo = pDeli->nLockNo;
			}


			// �J��
			for(ii=0; ii<pDeli->cnt; ii++) {
				SAFE_DELETE_ARRAY(pDeli->pImg[ii]);
			}
			SAFE_DELETE(pDeli->SendMailItem);
			delete pDeli;
			// ���x����p
			if(bCalcTime) p->mcls_Counter.CounterEnd();

			// DB�폜�Ǘ��ɒʒm
			p->Send_To_DbManage();
			break;

//-----------------------------------------------------------------------------------------------
		default :
			// ���肦�Ȃ��I�I
			break;
		}
	}
	return 0;
}

//------------------------------------------
// DB�폜�Ǘ��ɒʒm
//------------------------------------------
void DBSaveManager::Send_To_DbManage()
{
	if( ! m_bSendDbManager ) return ;

	// ���݂�Que�����擾
	int cnt = mque_pDeli->GetCount();
	// ���[�h����
	bool flg;
	if(cnt <= 100) flg = false;			// �폜�D��
	else		   flg = true;			// �������ݗD��
	// ���[�h�ύX�����̏ꍇ�� �I���
	if(flg == m_bDBDelete) return;
	m_bDBDelete = flg;

	// ���M
	COMMON_QUE que;
	que.nEventNo = FACT_TI_DELETE_01;
	que.nLineNo = LINE_ID;				
	que.fl.data[0] = (flg ? 1 : 0 );	// 0:�폜�D�� 1:�����ݗD��
	send_mail(TI_DELETE, ".", &que);
}

//------------------------------------------
// SQL���s
// DeliverySQL * pDeli �󂯓n���\���̃|�C���^
//------------------------------------------
void DBSaveManager::sqlExecute(DeliverySQL * pDeli)
{
	bool bRetc;

	//// SQL���`�F�b�N
	if( 0 == pDeli->strSql.GetLength() ) {		// SQL������
		if(NULL != m_evSynclock) {
			// �ʃX���b�h�őO��SQL���܂����s���̉\��������ׁA������Ƃ����҂�
			Sleep(500);
			// �o�^�󋵖₢���킹�ʒm������
			SetEvent(m_evSynclock);
		}

	} else {									// SQL���L��

		////// DB�o�^
/*
		if (0 != pDeli->cnt) {						// �摜�f�[�^
			// �����o�C�i���f�[�^�ꊇ������
			if( ! mcls_pDB->SqlBinalyWrite((LPCSTR)pDeli->strSql, pDeli->cnt, pDeli->size, (LPVOID*)pDeli->pImg ) ) {
				LOG(em_ERR), "[DBSaveManager] <%d> SQL�o�C�i���o�^�G���[", m_nMyID);
				LOG(em_NON), "%s", pDeli->strSql);
				syslog(SYSNO_DB_BINALYWRITE_ERR, "[DBSaveManager]");
				goto Ending;
			}

		} else {									// �ʏ��SQL���̂�		
			if( !mcls_pDB->ExecuteDirect((LPCSTR)pDeli->strSql)) {
				LOG(em_ERR), "[DBSaveManager] <%d> SQL�o�^�G���[", m_nMyID);
				LOG(em_NON), "%s", pDeli->strSql);
				syslog(SYSNO_DB_EXECUTEDIRECT_ERR, "[DBSaveManager]");
				goto Ending;
			}
		}
*/
		if (0 != pDeli->cnt) {						// �摜�f�[�^

			// �����o�C�i���f�[�^�ꊇ������
			bRetc = mcls_pDB->SqlBinalyWrite((LPCSTR)pDeli->strSql, pDeli->cnt, pDeli->size, (LPVOID*)pDeli->pImg );
			// ���g���C����
			if(!bRetc && m_bReTry && m_bReTryExecOk) {
				LOG(em_INF), "[DBSaveManager] <%d> SQL�o�C�i���o�^�G���[�ɂ��A�������g���C�J�n�I", m_nMyID);
				m_bReTryExecOk = false;
				CloseDB();
				ConnectDB(false);										// DB�����ڑ�
				bRetc = mcls_pDB->SqlBinalyWrite((LPCSTR)pDeli->strSql, pDeli->cnt, pDeli->size, (LPVOID*)pDeli->pImg );
			}

			if( ! bRetc ) {
				LOG(em_ERR), "[DBSaveManager] <%d> SQL�o�C�i���o�^�G���[", m_nMyID);
				LOG(em_NON), "%s", pDeli->strSql);
				syslog(SYSNO_DB_BINALYWRITE_ERR, "[DBSaveManager]");
				goto Ending;
			}

		} else {									// �ʏ��SQL���̂�	
			bRetc = mcls_pDB->ExecuteDirect((LPCSTR)pDeli->strSql);
			// ���g���C����
			if(!bRetc && m_bReTry && m_bReTryExecOk) {
				LOG(em_INF), "[DBSaveManager] <%d> SQL�o�C�i���o�^�G���[�ɂ��A�������g���C�J�n�I", m_nMyID);
				m_bReTryExecOk = false;
				CloseDB();
				ConnectDB(false);										// DB�����ڑ�
				bRetc = mcls_pDB->ExecuteDirect((LPCSTR)pDeli->strSql);
			}

			if( ! bRetc ) {
				LOG(em_ERR), "[DBSaveManager] <%d> SQL�o�^�G���[", m_nMyID);
				LOG(em_NON), "%s", pDeli->strSql);
				syslog(SYSNO_DB_EXECUTEDIRECT_ERR, "[DBSaveManager]");
				goto Ending;
			}
		}

		////// �R�~�b�g
		mcls_pDB->TranCommit();
		m_bReTryExecOk = true;			// 1��ł�SQL������������A���g���C���Ă�OK
	}

	////// �o�^�������Ƀ��[���X���b�g���M�p
	if(NULL != pDeli->SendMailItem) {
		LOG(em_INF), "[DBSaveManager] <%d> SQL�o�^���Mail���M [%s:%s:%d]", m_nMyID, pDeli->SendMailItem->cTask, pDeli->SendMailItem->cPc, pDeli->SendMailItem->Que.nEventNo);
		send_mail(pDeli->SendMailItem->cTask, pDeli->SendMailItem->cPc, &pDeli->SendMailItem->Que);
	}
	
	return;
Ending: 
	//// �f�[�^�x�[�X�ؒf
	CloseDB();
	//mcls_pDB->DisConnect();
}

//------------------------------------------
// DB�ڑ�
// bool bLogDsp ���O�o�͗L�� (false:���s���O�o���Ȃ�)
//------------------------------------------
bool DBSaveManager::ConnectDB(bool bLogDsp)
{
	bool retc;
	
	// ���g���C�^�C�}�[���c���Ă�����L�����Z�����Ă���
	CancelWaitableTimer(m_evReTryConnect);

	// DB�ڑ�
	if( 0 == m_cSession.GetLength() ) {
		retc = mcls_pDB->Connect(TASKINI_NAME);
	} else {
		retc = mcls_pDB->Connect(TASKINI_NAME, m_cSession);
	}

	// DB�ڑ�
	if( ! retc ) {
		// �f�[�^�x�[�X�ڑ��G���[
		if(bLogDsp) {
			LOG(em_ERR), "[DBSaveManager] <%d> �ڑ��G���[", m_nMyID);
			syslog(SYSNO_DB_CONNECT_ERR,"[DBSaveManager]");
		}
		m_bDBConnect = false;

	} else {
		LOG(em_MSG), "[DBSaveManager] <%d> �ڑ�����", m_nMyID);
		m_bDBConnect = true;
	}

	//// �ڑ��ُ킾������ ���g���C�����J�n
	if( ! m_bDBConnect ) {
		LARGE_INTEGER ts;
		ts.QuadPart =(__int64) PARAM_DB_RETRY_TIME * -10000;	// 1[ms]��100[ns] (�}�C�i�X�w��ŁA�w�莞�Ԍo�߂ŃV�O�i��) (��3������2��ڈȍ~�̋N���Ԋu)
		SetWaitableTimer(m_evReTryConnect, &ts, 0, NULL, NULL, FALSE);
	}
	return m_bDBConnect;
}

//------------------------------------------
// DB�ؒf
//------------------------------------------
void DBSaveManager::CloseDB()
{
	if( ! m_bDBConnect ) return;		// ���ɐؒf�ς�
	
	// �ؒf
	mcls_pDB->DisConnect();
	m_bDBConnect = false;
	LOG(em_WAR), "[DBSaveManager] <%d> �ؒf", m_nMyID);

	// ���g���C
	LARGE_INTEGER ts;
	ts.QuadPart = (__int64)PARAM_DB_RETRY_TIME * -10000;	// 1[ms]��100[ns] (�}�C�i�X�w��ŁA�w�莞�Ԍo�߂ŃV�O�i��) (��3������2��ڈȍ~�̋N���Ԋu)
	SetWaitableTimer(m_evReTryConnect, &ts, 0, NULL, NULL, FALSE);
}
