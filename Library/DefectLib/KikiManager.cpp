#include "StdAfx.h"
#include "KikiManager.h"


//////////////////////////////////////////////////////////////////////
// �\�z/����
//////////////////////////////////////////////////////////////////////
//------------------------------------------
// �R���X�g���N�^
// int nCycleTime �@���Ԃ̎�荞�ݎ��� [ms]
// LPCSTR cIniPath �ڑ�DB�̒�`ini�t�@�C��
// LPCSTR cSession �ڑ�DB�̒�`�Z�b�V����
//------------------------------------------
KikiManager::KikiManager(int nCycleTime, LPCSTR cIniPath, LPCSTR cSession) :
m_nCycleTime(nCycleTime),
m_nMsgReadCnt(0),
m_bOutPutLog(false),
m_bIsKadou(false),
m_nSysArea_BitPattan(0),
mque_Deli(512),		// ��ԓo�^������������128����512�ɕύX
mcls_pDB(NULL),
mcls_pLog(NULL)
{
	// ������
	InitializeCriticalSection(&m_csLock);									// �N���e�B�J���Z�N�V�����I�u�W�F�N�g��������
	m_evTimeCycleRead = CreateWaitableTimer(NULL, FALSE, NULL);	
	m_evReadEnd		  = CreateEvent(NULL, FALSE, FALSE, NULL);					// �񓯊��Ǎ��݊����C�x���g
	m_evLockReadStart = CreateEvent(NULL, FALSE, FALSE, NULL);				// �����Ǎ��݊J�n�C�x���g
	m_evLockReadEnd	  = CreateEvent(NULL, FALSE, FALSE, NULL);				// �����Ǎ��݊����C�x���g
	m_evSyncRead	  = CreateEvent(NULL, FALSE, FALSE, NULL);	
	memset(m_typStatInf, 0x00, sizeof(m_typStatInf));
	memset(&m_typStatNow, 0x00, sizeof(m_typStatNow));

	// ini�t�@�C���̒�`
	memset(m_cIniPath, 0x00, sizeof(m_cIniPath));
	memset(m_cIniSession, 0x00, sizeof(m_cIniSession));
	strcpy(m_cIniPath, cIniPath);
	strcpy(m_cIniSession, cSession);

	//// ODBC�ڑ��N���X �C���X�^���X����
	mcls_pDB = new OdbcBase(); 
	// DB�����ڑ�
	ConnectDB(true);

	// ���f�[�^���擾���Ă���
	CheckStatus();
	SetEvent(m_evReadEnd);										// �Ǎ��݊����V�O�i���Z�b�g
}

//------------------------------------------
// �f�X�g���N�^
//------------------------------------------
KikiManager::~KikiManager(void)
{
	// ODBC�ڑ��N���X �J��
	mcls_pDB->DisConnect();
	SAFE_DELETE(mcls_pDB);

	//// �J��
	CancelWaitableTimer(m_evTimeCycleRead);
	CloseHandle(m_evTimeCycleRead);
	CloseHandle(m_evReadEnd);
	CloseHandle(m_evLockReadStart);
	CloseHandle(m_evLockReadEnd);
	CloseHandle(m_evSyncRead);
	DeleteCriticalSection(&m_csLock);							// �N���e�B�J���Z�N�V�����I�u�W�F�N�g���J��
}

//------------------------------------------
// �X���b�h
//------------------------------------------
UINT WINAPI KikiManager::MainThread(LPVOID param)
{
	KikiManager::DELIVERYSTATUS* pDeli;			// �L���[�f�[�^

	////// ���C���C���X�^���X���o��
	KikiManager *p = (KikiManager *)param;
	p->SetTid();								// ���X���b�hID�Z�b�g

	// ������^�C�}�[
	if(0 != p->m_nCycleTime) {
		__int64 ts = 100 * -10000;			// 1�~���b��100�i�m�b (�}�C�i�X�w��ŁA�w�莞�Ԍo�߂ŃV�O�i��) (��3������2��ڈȍ~�̋N���Ԋu)
		SetWaitableTimer(p->m_evTimeCycleRead,   (LARGE_INTEGER *)&ts, p->m_nCycleTime,   NULL, NULL, FALSE);
	}

	// WaitForMultipleObjects�C�x���g
	HANDLE	hArray[] = {p->my_evStop,
						p->m_evLockReadStart,
						p->mque_Deli.g_evSem,
						p->m_evTimeCycleRead,
						p->m_evSyncRead
						};
	enum {	EV_STOP = WAIT_OBJECT_0,		// �X���b�h�I��
			EV_READ_START,					// �����Ǎ���
			EV_QUE,							// �L���[�C�x���g
			EV_READ_CYCLE,					// ������ǂݍ���
			EV_SYNC_READ,					// �񓯊��ǂݍ���
			EV_END	};


//p->mcls_Counter.InitAve();
	// ���b�Z�[�W�L���[�ǂݍ���
	pLOG(em_MSG), "[KikiManager] �X���b�h�J�n [0x%X]", p->GetTid());
	while (1) {
		DWORD ret = WaitForMultipleObjects(EV_END, hArray, FALSE, INFINITE);

		switch (ret) {

//-----------------------------------------------------------------------------------------------
		case EV_STOP:												// �X���b�h�I��
			CancelWaitableTimer(p->m_evTimeCycleRead);

			// �҂��f�[�^�͑S���o�^���Ă���
			if(true) {
				int nCnt = p->mque_Deli.GetCount();		// �c��
				if(0 != nCnt) pLOG(em_WAR), "[KikiManager] �X�e�[�^�X�ύX�����Z�b�g %d��", nCnt);

				while(true) {
					pDeli = p->mque_Deli.GetFromHead(0);
					if( pDeli == NULL ) break;

					p->SetStatusThread(pDeli);			
					delete pDeli;
				}	
			}
			pLOG(em_MSG), "[KikiManager] �X���b�h�I�� [0x%X]", p->GetTid());

			return 0;

//-----------------------------------------------------------------------------------------------
		case EV_READ_START:											// �����Ǎ���
//pLOG(em_MSG), "[KikiManager] �����X�e�[�^�X�Ǎ��J�n");
			p->m_nMsgReadCnt = 1;											// �����Ǎ��݂͑����ǂ݂�������MSG�͖��擾�Ƃ���
			p->CheckStatus();
			SetEvent(p->m_evLockReadEnd);									// �Ǎ��݊����V�O�i���Z�b�g
//pLOG(em_MSG), "[KikiManager] �����X�e�[�^�X�Ǎ�����");
			break;
	
//-----------------------------------------------------------------------------------------------
		case EV_QUE:												// �񓯊��X�e�[�^�X�ύX�ʒm
			pDeli = p->mque_Deli.GetFromHead();
			if( pDeli == NULL ) break;

			p->SetStatusThread(pDeli);			
			delete pDeli;

			// �ǂݍ��݈˗����w�肵�Ă���
			SetEvent(p->m_evSyncRead);
			break;

//-----------------------------------------------------------------------------------------------
		case EV_READ_CYCLE:											// ����� �@���ԃ`�F�b�N
		case EV_SYNC_READ:											// �񓯊��ǂݍ���
//pLOG(em_MSG), "[KikiManager] �X�e�[�^�X�Ǎ��J�n");
//p->mcls_Counter.CounterStart(); 
			p->CheckStatus();
			SetEvent(p->m_evReadEnd);										// �Ǎ��݊����V�O�i���Z�b�g
//p->mcls_Counter.CounterEnd(); 
//pLOG(em_MSG), "[KikiManager] �X�e�[�^�X�Ǎ�����");
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
// �����X�V (�Ǎ��݊�������܂Ńu���b�N�����)
//------------------------------------------
void KikiManager::Refresh()
{
	SetEvent(m_evLockReadStart);
	WaitForSingleObject(m_evLockReadEnd, 2000);
}

//------------------------------------------
// DB�ڑ�
// bool bLogDsp ���O�o�͗L�� (false:���s���O�o���Ȃ�)
//------------------------------------------
bool KikiManager::ConnectDB(bool bLogDsp)
{
	// �`�F�b�N
	if(mcls_pDB->IsConnect()) return true;			// ���ɂɐڑ��ς�
	// DB�ڑ�
	//if( ! mcls_pDB->Connect(TASKINI_NAME)) {
	if( ! mcls_pDB->Connect(m_cIniPath, m_cIniSession)) {
		// �f�[�^�x�[�X�ڑ��G���[
		if(bLogDsp) {
			LOG(em_ERR), "[KikiManager] �ڑ��G���[");
			syslog(SYSNO_DB_CONNECT_ERR,"[KikiManager]");
		}
		return false;

	} else {
		LOG(em_MSG), "[KikiManager] �ڑ�����");
		return true;
	}
}

//------------------------------------------
// DB�ؒf
//------------------------------------------
void KikiManager::CloseDB()
{
	if( ! mcls_pDB->IsConnect() ) return;		// ���ɐؒf�ς�
	
	// �ؒf
	mcls_pDB->DisConnect();
}


// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// �X�e�[�^�X�Ǎ��֌W

//------------------------------------------
// �@���Ԏ擾 (�Z�[�t�X���b�h�ł͖����ג��ӂ��邱��)
//------------------------------------------
bool KikiManager::CheckStatus()
{
	TYP_STATUS_INF			wkInf[MAX_STATUS];				// �擾�@���ԃ��[�N
	TYP_STATUS_NOWSYSTEM	wkNow;							// ���ُ���
	TYP_STATUS_NOWSYSTEM	wkOkNow;						// OK�̏ꍇ���b�Z�[�W�Z�b�g�p���[�N Ver.04

	CString cSql;											// SQL��
	int		id;
	bool	isRun = m_bIsKadou;								// �r���ŏ���������Ă������悤�Ƀ��[�J���Ɏ���
	int		area_sys;										// �V�X�e���͈�

	//memset( wkInf, 0x00, sizeof(wkInf));
	memcpy( &wkInf, &m_typStatInf, sizeof(wkInf));	// �O��l�ŃR�s�[
	memset( &wkNow, 0x00, sizeof(wkNow));
	memset( &wkOkNow, 0x00, sizeof(wkOkNow));

	// �ڑ��m�F
	if( ! ConnectDB(false) ) {
		LOG(em_ERR), "[KikiManager] DB�ڑ����s�I�I");
		return false;					// DB�ڑ��ł��Ȃ��ׁA����
	}

	//// �����l�Z�b�g
	if(isRun) {
		wkNow.kensa[0] = DIV_KENSA_OK;
		wkNow.kensa[1] = DIV_KENSA_OK;
	} else {
		wkNow.kensa[0] = DIV_KENSA_NON;
		wkNow.kensa[1] = DIV_KENSA_NON;
	}

	//// �@���Ԏ擾
	// �@���ԁA�@��}�X�^�[ �����łȂ��APC�ɕR�t����Ԃ��擾�B�����āA�I�t���C����PC��������ُ�Ƃ������Ȃ�����
	// Where�������̋֎~�B�@���Ԃ́A����ُ�֌W�Ȃ��S���擾���邱��
	cSql = "SELECT m.�@��ID, m.PCID, m.�ُ�͈�, m.�����ُ�x, m.�@��ُ�x, m.�D��x, ISNULL(p.PC�敪,0), i.�@���� ";
	// �V�X�e���͈͂ɑΉ�(0,NULL �͑S���Ώ�) Ver.02
	if( 0 != m_nSysArea_BitPattan ) cSql += ", (CASE WHEN m.�V�X�e���͈� IS NULL OR m.�V�X�e���͈�=0 THEN 0xffffffff ELSE m.�V�X�e���͈� END) ";		
	else							cSql += ", 0 ";
	// ���גጸ�΍�
	if( 0 == m_nMsgReadCnt)			cSql += ", m.�ُ�MSG ";

	cSql += "FROM (";
		cSql += DB_STATUS_MASTER;
		cSql += " m ";
			cSql += "INNER JOIN ";
			cSql += DB_STATUS_INF;
			cSql += " i ON m.�@��ID = i.�@��ID ) ";		// ACCESS�ł�SQL�����ʂ�悤����ׁA�J�b�R�ň͂ނ�
			cSql += " LEFT OUTER JOIN ";
			cSql += DB_PC_INF;
			cSql += " p ON m.PCID = p.PCID ";
	//cSql += "ORDER BY ISNULL(m.�D��x, 1000) asc, m.�@��ُ�x desc, m.�����ُ�x desc ";		// Ver.04 �����ُ�x��ǉ�
	cSql += "ORDER BY ISNULL(m.�D��x, 500) asc, m.�@��ُ�x desc, m.�����ُ�x desc ";		// Ver.04 �����ُ�x��ǉ�


//��	LOG(em_ERR), "[KikiManager] �J�n");

	bool bRetc = mcls_pDB->ExecuteDirect(cSql);
	if( ! bRetc ) {
		LOG(em_WAR), "[KikiManager] �X�e�[�^�X�擾���s�I�I�������g���C�J�n");
		CloseDB();
		if( ! ConnectDB(false) ) {
			LOG(em_ERR), "[KikiManager] DB�ڑ����s�I�I");
			return false;					// DB�ڑ��ł��Ȃ��ׁA����
		}
		bRetc = mcls_pDB->ExecuteDirect(cSql);
	}
	if( ! bRetc ) {
		LOG(em_ERR), "[KikiManager] ExecuteDirect�G���[");
		syslog(SYSNO_DB_EXECUTEDIRECT_ERR,"[KikiManager]");
		CloseDB();
		return false;
	}


	while (TRUE) {
		int sqlrc = mcls_pDB->Fetch();
		if (sqlrc == KIZUODBC_FETCH_OK) {						// �f�[�^�L�� 
			//============================
			// ��L�[���o��
			mcls_pDB->GetData(1, &id);
			if( 0>=id || MAX_STATUS < id) {
				LOG(em_ERR), "[KikiManager] �f�[�^�s��[%d]", id);
				syslog(SYSNO_DB_DATA_ERR,"[KikiManager data=%d]", id);
				CloseDB();
				return false;
			}

			id = id -1;
			// �f�[�^�Z�b�g
			wkInf[id].id = id+1;
			mcls_pDB->GetData(2, &wkInf[id].pcid);
			mcls_pDB->GetData(3, &wkInf[id].area);
			mcls_pDB->GetData(4, (int*)&wkInf[id].kensa); 
			mcls_pDB->GetData(5, (int*)&wkInf[id].kiki); 
			mcls_pDB->GetData(6, &wkInf[id].yusen); 
			mcls_pDB->GetData(7, &wkInf[id].pcmode);
			mcls_pDB->GetData(8, &wkInf[id].stat); 
				
			// �V�X�e���͈͂ɑΉ� Ver.02 --->>>
			if( 0 != m_nSysArea_BitPattan ) {
				mcls_pDB->GetData(9, &area_sys); 
				
				// �ΏۊO�͈̔́H
				// �r�b�g�p�^�[���Ɉ�v����r�b�g��1�ł�ON��������Ώ�
				if( (area_sys & m_nSysArea_BitPattan ) == 0 ) continue;
			}
			// <<<-----------------------------

			// ���גጸ�΍�
			if( 0 == m_nMsgReadCnt)	mcls_pDB->GetData(10, &wkInf[id].msg, sizeof(wkInf[id].msg));
			

			//============================
			// �ُ��Ԏ擾
				// �ُ�擾�̓I�����C���̂�
				// �ُ�͈͂�-1�́A�I�����C���ւ̉e������(���䕔�ł͐���F���BRAS��ʓ���ʂɂ̂݉e��) 20081216
			if(0 == wkInf[id].pcmode && 0 != wkInf[id].stat && -1 != wkInf[id].area ) {
				// �\
				if(0 == wkInf[id].area || 1 == wkInf[id].area) {
					// �������
					if( wkNow.kensa[0] < wkInf[id].kensa ) wkNow.kensa[0] = wkInf[id].kensa;
					if( ! isRun) wkNow.kensa[0] = DIV_KENSA_NON;			// �T����~���́A������� ��~�Œ�
					// �@����
					if( wkNow.kiki[0]  < wkInf[id].kiki )  wkNow.kiki[0]  = wkInf[id].kiki;
				}
				// ��
				if(0 == wkInf[id].area || 2 == wkInf[id].area) {
					// �������
					if( wkNow.kensa[1] < wkInf[id].kensa ) wkNow.kensa[1] = wkInf[id].kensa;
					if( ! isRun) wkNow.kensa[1] = DIV_KENSA_NON;			// �T����~���A������� ��~�Œ�
					// �@����
					if( wkNow.kiki[1]  < wkInf[id].kiki )  wkNow.kiki[1]  = wkInf[id].kiki;
				}	
				
				// �@��ُ�̂�Z�b�g
				if( wkNow.nCnt < 5 && 
					( ( isRun && ( 0 != wkInf[id].kensa || 0 != wkInf[id].kiki)) || (!isRun && 0 != wkInf[id].kiki) )	) {

					memcpy( &wkNow.ngInf[wkNow.nCnt], &wkInf[id], sizeof(wkInf[id]));
					wkNow.nCnt ++;
				}
			}


			//============================
			// ���탁�b�Z�[�W���o�� Ver.04 ------>>>
			// �D��x��900�ȉ��̏ꍇ�́A���b�Z�[�W�o��
			if( wkOkNow.nCnt < 5 && 0 != wkInf[id].stat &&
				(STATUS_OK_MSG_YUUSENID_MIN <= wkInf[id].yusen && STATUS_OK_MSG_YUUSENID_MAX >= wkInf[id].yusen) ) {

				// ���탁�b�Z�[�W�Z�b�g
				memcpy( &wkOkNow.ngInf[wkOkNow.nCnt], &wkInf[id], sizeof(wkInf[id]));
				wkOkNow.nCnt ++;
			}
			// <<<-------------


		} else if( sqlrc == KIZUODBC_FETCH_NODATE ) {		// �f�[�^����
			break;
		} else {								// �G���[
			LOG(em_ERR), "[KikiManager] Fetch�G���[");
			syslog(SYSNO_DB_FETCH_ERR, "[KikiManager]");
			CloseDB();
			return false;
		}
	}
	
//��	LOG(em_ERR), "[KikiManager] ����");

	// Ver.04 ------>>>
	// ����̏ꍇ�A�D��x��900�ȉ��̏ꍇ�́A���b�Z�[�W�o��
	if( 0 == wkNow.nCnt && 0 != wkOkNow.nCnt ) {
		wkNow.nCnt = wkOkNow.nCnt;
		memcpy(wkNow.ngInf, wkOkNow.ngInf, sizeof(wkOkNow.ngInf));  
	}
	// <<<-------------

	// Ver.05 ------>>>
	if( m_bOutPutLog ) {
//��	LOG(em_ERR), "[KikiManager] �̏Ⴭ�O�Q�ƊJ�n");

		GetLogInfo(&wkNow);

//��	LOG(em_ERR), "[KikiManager] �̏Ⴭ�O�Q�Ɗ���");
	}
	// <<<-------------
	
//LOG(em_ERR), "[KikiManager] ����");
	// ���C���ɃZ�b�g
	AutoLock	lock(&m_csLock);
	memcpy( m_typStatInf, wkInf, sizeof(m_typStatInf));
	memcpy( &m_typStatNow, &wkNow, sizeof(m_typStatNow));

	// ����p
	m_nMsgReadCnt ++;
	if(20 < m_nMsgReadCnt) m_nMsgReadCnt = 0;		// ���3�`5�b�����Ŏ擾
	return true;
}


//------------------------------------------
// �I�y�K�C���郁�b�Z�[�W���擾
// TYP_STATUS_NOWSYSTEM* inf �Z�b�g�G���A
//------------------------------------------
void KikiManager::GetLogInfo(TYP_STATUS_NOWSYSTEM* inf)
{
//��	LOG(em_ERR), "[KikiManager] LOG�Ǎ���");

	// �ڑ��m�F
	if( ! ConnectDB(false) ) {
		LOG(em_ERR), "[KikiManager] DB�ڑ����s�I�I");
		return;					// DB�ڑ��ł��Ȃ��ׁA����
	}
/*** ������ �S���Ԃ�LOGINF�Ȃ̂Ŗc��Ȏ��Ԃ��|����B
	CString cSql;											// SQL��
	cSql = "SELECT TOP 1 '�y'+SUBSTRING(CONVERT(varchar,I.�o�^���t,111),6,5)+' '+LEFT(CONVERT(varchar,I.�o�^���t,108),5)+'�z '+ M.���e+' '+I.���e ";
	cSql += "FROM ";
	cSql += DB_LOG_INF;
	cSql += " as I LEFT OUTER JOIN ";
	cSql += DB_LOG_MASTER;
	cSql += " as M ON I.�A���[��No = M.�A���[��No ";
	cSql += "WHERE M.�I�y�K�C�L��=1 AND DATEDIFF(SECOND, I.�o�^���t, GETDATE()) <= M.�I�y�K�C���� ORDER BY I.�o�^���t DESC";
***/

	CString cTime;
	CTime	tWk = CTime::GetCurrentTime();
	cTime.Format( "CONVERT(datetime,'%d/%d/%d')-%d", tWk.GetYear(), tWk.GetMonth(), tWk.GetDay(), 3);	// ����3���Ɍ���

	CString cSql;											// SQL��
	cSql = "SELECT TOP 1 '�y'+SUBSTRING(CONVERT(varchar,I.�o�^���t,111),6,5)+' '+LEFT(CONVERT(varchar,I.�o�^���t,108),5)+'�z '+ M.���e+' '+I.���e ";
	cSql += "FROM ";
	cSql += DB_LOG_INF;
	cSql += " as I INNER JOIN ";

	cSql += DB_LOG_MASTER;
	cSql += " as M ON I.�A���[��No = M.�A���[��No ";
	cSql += " WHERE M.�I�y�K�C�L��=1 ";
	cSql += " AND I.�o�^���t >= " + cTime;			// ����3���ԁB�p�����[�^�w���2���ԕ�
	cSql += " AND DATEDIFF(SECOND, I.�o�^���t, GETDATE()) <= M.�I�y�K�C���� ";
	cSql += " ORDER BY I.�o�^���t DESC";



	//// �f�[�^�擾
	char cWk[256];
	memset(cWk, 0x00, sizeof(cWk));
	if( ! mcls_pDB->GetSelectKey(cSql, sizeof(cWk), cWk) ) return;

	//// �f�[�^�Z�b�g
	for(int ii=5-1; ii>0; ii--) {
		memcpy(&inf->ngInf[ii], &inf->ngInf[ii-1], sizeof(TYP_STATUS_INF));
	}

	//// �擪�̃��b�Z�[�W�̂ݕύX
	memcpy(&inf->ngInf[0].msg, cWk, sizeof(cWk));
	if( 5 > inf->nCnt ) inf->nCnt++;

//��	LOG(em_ERR), "[KikiManager] %s ����", inf->ngInf[0].msg);
}

// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// �X�e�[�^�X�ύX�֌W

//------------------------------------------
// DB_SYSTEM_STATE�ɋ@���Ԃ��Z�b�g����
// int id �@��ID
// int state �@���� (0:����  1�`:�ُ�)
// bool bLogDsp �V�X���O�o�̗͂L��
// char const * cMsg �ǉ����b�Z�[�W
//------------------------------------------
void KikiManager::SetStatusQue(char const * cKey, int state, bool bLogDsp, char const * cMsg)
{
	int id = GetPrivateProfileInt(INI_STATUS, cKey, 0, TASKINI_NAME);
	SetStatusQue(id, state, bLogDsp, cMsg );
}
void KikiManager::SetStatusQue(char const * cKey, bool state, bool bLogDsp, char const * cMsg)
{
	int id = GetPrivateProfileInt(INI_STATUS, cKey, 0, TASKINI_NAME);
	SetStatusQue(id, (state ? 0 : 1), bLogDsp, cMsg );
}
void KikiManager::SetStatusQue(int id, bool state, bool bLogDsp, char const * cMsg)
{
	SetStatusQue(id, (state ? 0 : 1), bLogDsp, cMsg );
}
void KikiManager::SetStatusQue(int id, int state, bool bLogDsp, char const * cMsg)
{
	_ASSERT(0!=id);

	// �f�[�^�Z�b�g
	DELIVERYSTATUS* pDeli = new DELIVERYSTATUS;
	pDeli->id = id;
	pDeli->state = state;
	pDeli->bLogDsp = bLogDsp;
	pDeli->strMsg = cMsg;

	// �o�^
	if( ! mque_Deli.AddToTailFreeCheck(pDeli) ) {
		// ���^��
		delete pDeli;
		LOG(em_ERR), "[KikiManager] �X�e�[�^�X�o�^�˗��L���[�t��(id:%d)(st:%d)[%s]", id, state, cMsg);
		syslog(SYSNO_QUEFULL_WAR, "[KikiManager](id:%d)(st:%d)", id, state);
	}
}

//------------------------------------------
// �X�e�[�^�X �񓯊���������
// DELIVERYSTATUS * pDeli �󂯓n���\���̃|�C���^
//------------------------------------------
bool KikiManager::SetStatusThread(DELIVERYSTATUS* pDeli)
{
	CString cSql;											// SQL��

	// �ڑ��m�F
	if( ! ConnectDB(false) ) {
		LOG(em_ERR), "[KikiManager] DB�ڑ����s�I�I");
		return false;					// DB�ڑ��ł��Ȃ��ׁA����
	}

	//// SQL���s
	cSql.Format( "UPDATE %s SET �@����=%d WHERE �@��ID=%d", DB_STATUS_INF, pDeli->state, pDeli->id );
/*
	if( ! mcls_pDB->ExecuteDirect(cSql)) {
		LOG(em_ERR), "[KikiManager] �X�e�[�^�X�ύX ExecuteDirect�G���[");
		syslog(SYSNO_DB_EXECUTEDIRECT_ERR,"[KikiManager �X�e�[�^�X�ύX]");
		CloseDB();
		return false;
	}
*/
	bool bRetc = mcls_pDB->ExecuteDirect(cSql);
	if( ! bRetc ) {
		LOG(em_WAR), "[KikiManager] �񓯊��X�e�[�^�X�ύX���s�I�I�������g���C�J�n");
		CloseDB();
		if( ! ConnectDB(false) ) {
			LOG(em_ERR), "[KikiManager] DB�ڑ����s�I�I");
			return false;					// DB�ڑ��ł��Ȃ��ׁA����
		}
		bRetc = mcls_pDB->ExecuteDirect(cSql);
	}
	if( ! bRetc ) {
		LOG(em_ERR), "[KikiManager] �񓯊��X�e�[�^�X�ύX ExecuteDirect�G���[");
		syslog(SYSNO_DB_EXECUTEDIRECT_ERR,"[KikiManager �X�e�[�^�X�ύX]");
		CloseDB();
		return false;
	}


	//// �R�~�b�g
	mcls_pDB->TranCommit();
/*
	//// ���݂̃������ɂ����f���Ă����B	�v�����I�I
	// �������Ă������ƂŁA�������f��ۏႵ�Ă����B
	if(0 < pDeli->id && MAX_STATUS > pDeli->id) {
		m_typStatInf[pDeli->id-1].stat = 
	}
*/

	//// �V�X���O�o��
	if(pDeli->bLogDsp) {
		if(0==pDeli->state) statelog(SYSNO_STATUS_OK, pDeli->id, pDeli->state, pDeli->strMsg );
		else				statelog(SYSNO_STATUS_NG, pDeli->id, pDeli->state, pDeli->strMsg );
	}

	return true;
}
