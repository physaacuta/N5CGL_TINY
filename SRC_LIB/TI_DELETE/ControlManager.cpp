#include "StdAfx.h"
#include "ControlManager.h"

// �������[���[�N���o ----->>>
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif
// <<<------------------------

//------------------------------------------
// �R���X�g���N�^
// CString cSession �Z�b�V����
// LogFileManager* pLog ���O�t�@�C���}�l�[�W���[�|�C���^
//------------------------------------------
ControlManager::ControlManager(char* cSession, LogFileManager* pLog) :
m_cSession(cSession),
m_bTranStat(DIV_BOOL_NON),
m_bExecFast(false),
mcls_pLog(pLog)
{
	int ii;
	CString strVal;
	char	cWk[1024];

	//// �V�O�i������
	m_evEndCycle	 = CreateEvent(NULL, FALSE, FALSE, NULL);	
	m_evEndSizeCheck = CreateEvent(NULL, FALSE, FALSE, NULL);		

	//// ini�t�@�C���ǂݍ���
	// ODBC�̃Z�b�V����
	GetPrivateProfileString(m_cSession, "DB_ODBC", "", cWk, sizeof(cWk), TASKINI_NAME);
	m_cOdbcSession = cWk;
	// �g�����U�N�V�������O�T�C�Y�`�F�b�N
	m_nIniTranSizeErrID = GetPrivateProfileInt(m_cSession, "DB_TRAN_SIZE_ERR_ID", 0, TASKINI_NAME);	
	// �g�����U�N�V�������O�T�C�Y
	m_nIniTranErrSize	= GetPrivateProfileInt(m_cSession, "DB_TRAN_ERR_SIZE", 10000, TASKINI_NAME);	

	// ��������s����
	m_nIniTimeCycle = GetPrivateProfileInt(m_cSession, "TIME_CYCLE", 0, TASKINI_NAME);		
	m_nIniTimeCycle = m_nIniTimeCycle * 60 * 1000;// *1000;//* 60 * 1000;	// ����ms 
	// DB�e�ʃ`�F�b�N����
	m_nIniTimeSizeCheck = GetPrivateProfileInt(m_cSession, "TIME_SIZECHECK", 0, TASKINI_NAME);		
	m_nIniTimeSizeCheck = m_nIniTimeSizeCheck * 60 * 1000;// *1000;//* 60 * 1000;	// ����ms 
	// DB�e�ʒቺ���̍폜�X���b�h��
	m_nIniThreadCount = GetPrivateProfileInt(m_cSession, "THREAD_COUNT", 0, TASKINI_NAME);		
	if(m_nIniThreadCount < 0 || m_nIniThreadCount > THREAD_MAXCNT) _ASSERT(FALSE);
	// �󂫗e�ʗ�
	for(ii=0; ii<THREAD_MAXCNT; ii++) m_dIniDeleteRitu[ii] = 0.0;
	for(ii=0; ii<m_nIniThreadCount; ii++ ) {
		strVal.Format("DEL_RITU_%d", ii+1);
		m_dIniDeleteRitu[ii] = (double)GetPrivateProfileInt(m_cSession, strVal, 0, TASKINI_NAME);		
		if(m_dIniDeleteRitu[ii] == 0.0) {		// �I���
			break;
		}
		m_dIniDeleteRitu[ii] = m_dIniDeleteRitu[ii] / 10000;	// [%-2]��[1.0]
	}
	if(0 != m_nIniThreadCount && 0 == ii) _ASSERT(FALSE);

	// �����D�掞�̑҂����� [ms]
	m_nIniTimeMaxWait = GetPrivateProfileInt(m_cSession, "TIME_MAX_WAIT", 0, TASKINI_NAME);		
	// �폜�D�掞�̑҂����� [ms]
	m_nIniTimeMinWait = GetPrivateProfileInt(m_cSession, "TIME_MIN_WAIT", 0, TASKINI_NAME);	

	//// �p�����[�^����
	memset(&m_Param, 0x00, sizeof(m_Param));
	// ���O�ۑ�����
	m_Param.nLogSaveDay = GetPrivateProfileInt(m_cSession, "LOG_SAVEDAY", 0, TASKINI_NAME);
	if(m_Param.nLogSaveDay < 0) _ASSERT(FALSE);
	// �폜�R�C������
	m_Param.nDelCoilCnt = GetPrivateProfileInt(m_cSession, "COIL_DEL_COUNT", 0, TASKINI_NAME);	
	// �R�C�����уe�[�u��
	GetPrivateProfileString(m_cSession, "COILRESULT_TBL", "", m_Param.sCoilResultTbl, sizeof(m_Param.sCoilResultTbl), TASKINI_NAME);
	// �R�C������ ��L�[
	GetPrivateProfileString(m_cSession, "COILRESULT_KEY", "", m_Param.sCoilResultKey, sizeof(m_Param.sCoilResultKey), TASKINI_NAME);
	// �R�C������ �\�[�g�L�[
	GetPrivateProfileString(m_cSession, "COILRESULT_SORT", "", m_Param.sCoilResultSort, sizeof(m_Param.sCoilResultSort), TASKINI_NAME);
	if( 0 == strlen(m_Param.sCoilResultSort) ) memcpy(m_Param.sCoilResultSort, m_Param.sCoilResultKey, sizeof(m_Param.sCoilResultSort));


	//// ������
	memset(&m_DiskSpace, 0x00, sizeof(m_DiskSpace));
	for(ii=0; ii<THREAD_MAXCNT; ii++) {
		mcls_pDele[ii] = NULL;
	}
	mcls_pCycle = NULL;
	m_bWaitMode = true;
	m_nWaitTime = m_nIniTimeMinWait;	
}

//------------------------------------------
// �f�X�g���N�^
//------------------------------------------
ControlManager::~ControlManager(void)
{
	// �J��
	CloseHandle(m_evEndCycle);
	CloseHandle(m_evEndSizeCheck);
}

//------------------------------------------
// �폜�X���b�h���������� (��x�̂݋N��)
//------------------------------------------
void ControlManager::WorkerInit()
{
	//// ���[�J�[�X���b�h ������
	// ��������s
	mcls_pCycle = new DeleTable(-1, &m_nWaitTime, m_evEndCycle, NULL, m_cSession, m_cOdbcSession);
	mcls_pCycle->SetLogMgr(mcls_pLog); 
	mcls_pCycle->SetParamInf(&m_Param);
	// �e�ʎ��s
	for(int ii=0; ii<m_nIniThreadCount; ii++) {
		mcls_pDele[ii] = new DeleTable(ii+1, &m_nWaitTime, NULL, m_evEndSizeCheck, m_cSession, m_cOdbcSession);
		mcls_pDele[ii]->SetLogMgr(mcls_pLog); 
		mcls_pDele[ii]->SetLockMgr(&mcls_Lock); 
		mcls_pDele[ii]->SetParamInf(&m_Param);
	}	

	//// ���[�J�[�X���b�h �N��
	mcls_pCycle->Start();
	for(int ii=0; ii<m_nIniThreadCount; ii++) {
		mcls_pDele[ii]->Start();
	}
}


//------------------------------------------
// �폜�X���b�h��~���� (��x�̂݋N��)
//------------------------------------------
void ControlManager::WorkerStop()
{
	//// ���[�J�[�X���b�h ��~
	mcls_pCycle->SetStop();
	// �܂��S�̂ɒ�~�w����ʒm����
	for(int ii=0; ii<m_nIniThreadCount; ii++) {
		mcls_pDele[ii]->SetStop();
	}
	LOG(em_MSG), "[Cont] [%s] �e�q�X���b�h��~�v������", m_cSession);
}

//------------------------------------------
// �폜�X���b�h�I������ (��x�̂݋N��)
//------------------------------------------
void ControlManager::WorkerExit()
{
	//// ���[�J�[�X���b�h �J��
	mcls_pCycle->Stop();
	SAFE_DELETE(mcls_pCycle);
	for(int ii=0; ii<m_nIniThreadCount; ii++) {
		mcls_pDele[ii]->Stop();
		SAFE_DELETE( mcls_pDele[ii] );
	}
}


//------------------------------------------
// �X���b�h
//------------------------------------------
UINT WINAPI ControlManager::MainThread(LPVOID param)
{
//	int ii;

	////// ���C���C���X�^���X���o��
	ControlManager *p = (ControlManager *)param;				// ���N���X�̃C���X�^���X���擾
	p->SetTid();										// ���X���b�hID�Z�b�g


	////////////////////////////////////
	// ���O����
	////////////////////////////////////

	//// ������^�C�}�[�Z�b�g
	HANDLE		evTimeCycle;										// ��������s����
	HANDLE		evTimeSizeCheck;									// DB�e�ʃ`�F�b�N����
	evTimeCycle		= CreateWaitableTimer(NULL, FALSE, NULL);	
	evTimeSizeCheck	= CreateWaitableTimer(NULL, FALSE, NULL);	
	if(0 != p->m_nIniTimeCycle) {
		__int64 ts = 10000 * -10000;	// 1�ʕb��100�i�m�b (�}�C�i�X�w��ŁA�w�莞�Ԍo�߂ŃV�O�i��) (��3������2��ڈȍ~�̋N���Ԋu)
		SetWaitableTimer(evTimeCycle,		(LARGE_INTEGER *)&ts, p->m_nIniTimeCycle, NULL, NULL, FALSE);
	}
	if(0 != p->m_nIniTimeSizeCheck) {
		__int64 ts = 10000 * -10000;	// 1�ʕb��100�i�m�b (�}�C�i�X�w��ŁA�w�莞�Ԍo�߂ŃV�O�i��) (��3������2��ڈȍ~�̋N���Ԋu)
		SetWaitableTimer(evTimeSizeCheck,	(LARGE_INTEGER *)&ts, p->m_nIniTimeSizeCheck, NULL, NULL, FALSE);
	}


	////////////////////////////////////
	// �ʏ�̃��C���X���b�h����
	////////////////////////////////////

	// WaitForMultipleObjects�C�x���g
	HANDLE	hArray[] = {p->my_evStop,
						evTimeCycle,
						p->m_evEndCycle,
						evTimeSizeCheck,
						p->m_evEndSizeCheck
						};
	enum {	EV_STOP = WAIT_OBJECT_0,		// �X���b�h�I��
			EV_TIMECYCLE,					// ������^�C�}�[
			EV_END_TIMECYCLE,				// ������^�C�}�[����
			EV_SIZECHECK,					// DB�e�ʃ`�F�b�N
			EV_END_SIZECHECK,				// DB�e�ʃ`�F�b�N����
			EV_END	};

	// ���b�Z�[�W�L���[�ǂݍ���
	pLOG(em_MSG), "[Cont] [%s] �X���b�h�J�n [0x%X]", p->m_cSession, p->GetTid());
	while (1) {
		DWORD ret = WaitForMultipleObjects(EV_END, hArray, FALSE, INFINITE);

		switch (ret) {

//-----------------------------------------------------------------------------------------------
		case EV_STOP:												// �X���b�h�I��
			pLOG(em_MSG), "[Cont] [%s] �X���b�h�I�� [0x%X]", p->m_cSession, p->GetTid());
			CancelWaitableTimer(evTimeCycle);
			CancelWaitableTimer(evTimeSizeCheck);
			CloseHandle(evTimeCycle);
			CloseHandle(evTimeSizeCheck);
			return 0;


//-----------------------------------------------------------------------------------------------
		case EV_TIMECYCLE:											// ������^�C�}�[
			if( ! OdbcBase::OpenCheck( TASKINI_NAME, p->m_cOdbcSession) ) {
				pLOG(em_ERR), "[Cont] [%s] DB�ڑ����s ", p->m_cSession);
				break;
			}

			if( p->mcls_pCycle->IsExec_Cycle()) {
				pLOG(em_WAR), "[Cont] [%s] ���ɁA��������s ", p->m_cSession);
				break;
			}

			// ���s�˗�
			pLOG(em_MSG), "[Cont] [%s] ��������s�˗�", p->m_cSession);
			p->mcls_pCycle->SetEvCycle();
			break;

//-----------------------------------------------------------------------------------------------
		case EV_END_TIMECYCLE:										// ������^�C�}�[����
			// �g�����U�N�V�������O�T�C�Y�`�F�b�N
			p->Check_TranSize();
			break;

//-----------------------------------------------------------------------------------------------
		case EV_SIZECHECK:											// DB�e�ʃ`�F�b�N
			if( ! OdbcBase::OpenCheck( TASKINI_NAME, p->m_cOdbcSession) ) {
				pLOG(em_ERR), "[Cont] [%s] DB�ڑ����s ", p->m_cSession);
				break;
			}

			// ������s�H
			if( ! p->m_bExecFast) {
				if( 1 <= p->m_nIniThreadCount ) p->mcls_pDele[0]->SetEvExecFast();
				p->m_bExecFast = true;
				break;
			}

			// DB�̎g�p�e�ʃ`�F�b�N
			p->CheckDB_Size();			
			break;


//-----------------------------------------------------------------------------------------------
		case EV_END_SIZECHECK:										// DB�e�ʃ`�F�b�N����
			// DB�e�ʃ`�F�b�N�����������ׁA�ēx�e�ʃ`�F�b�N
			p->CheckDB_Size();
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
// �폜/���� �D�惂�[�h (true:�폜�D��)
// bool mode True�F�폜�D�� False:�����D��
//------------------------------------------
void ControlManager::SetWaitMode(bool mode)
{
	// �����D��ōŏI�X���b�h�����s���̏ꍇ�� �폜�D��Ƃ���
	if ( ! mode ) {
		if( 0 == m_nIniThreadCount) {
			// �`�F�b�N�Ȃ�
		} else if ( mcls_pDele[m_nIniThreadCount-1]->IsExec() ) {
			LOG(em_ERR), "[Cont] [%s] �ŏI�X���b�h���s���ɂ��A�����I�ɍ폜�D�惂�[�h�֍s����", m_cSession);
			mode = true;	
		}
	}

	// ��ԕύX
	if( mode != m_bWaitMode ) {
		if (mode) {			// �폜�D��
			m_nWaitTime = m_nIniTimeMinWait;
			m_bWaitMode = true;
		} else {			// �������ݗD��
			m_nWaitTime = m_nIniTimeMaxWait;
			m_bWaitMode = false;
		}
	}
}


//------------------------------------------
// DB�̎g�p�e�ʃ`�F�b�N
//------------------------------------------
void ControlManager::CheckDB_Size() 
{
	int retc;				// �X���b�h���s��
	retc = GetDB_Size();
	LOG(em_MSG), "[Cont] [%s] DB�e�� [%.2f%%]", m_cSession, m_DiskSpace.dFreeRitu  * 100);

	if(retc > 0) {
		LOG(em_WAR), "[Cont] [%s] DB�e�ʒቺ [%.2f%%] �X���b�h%d�܂Ŏ��s", m_cSession, m_DiskSpace.dFreeRitu  * 100, retc);
		// DB�e�ʒቺ���s
		for(int ii=0; ii<retc; ii++) {
			if( ! mcls_pDele[ii]->IsExec()) mcls_pDele[ii]->SetEvSizeMin();
		}
	}
}

//------------------------------------------
// DB�̎g�p�e�ʎ擾
// �߂�l int -1:���s  0:�e�ʗL�� 1�`�e�ʖ����ɂ�肱�̐��܂ł̃X���b�h�����s
//------------------------------------------
int ControlManager::GetDB_Size() 
{
	CString sql;					// SQL��
	char buf[256];

	//// ODBC�ڑ��N���X �C���X�^���X����
	OdbcBase clsDB;
	if( ! clsDB.Connect(TASKINI_NAME, m_cOdbcSession )) {
		// �f�[�^�x�[�X�ڑ��G���[
		LOG(em_ERR), "[Cont] [%s] DB�ڑ��G���[[%s]", m_cSession, m_cOdbcSession);
		syslog(SYSNO_DB_CONNECT_ERR, "%s:%s", m_cSession, m_cOdbcSession);
		return -1;
	}


	////// �X�g�A�h�v���V�[�W���[���R�[��
	sql = "sp_spaceused";
	if( !clsDB.ExecuteDirect(sql)) {
		LOG(em_ERR), "[Cont] [%s] DBSizeCheck�G���[ [%s]", m_cSession, m_cOdbcSession);
		syslog(SYSNO_DB_EXECUTEDIRECT_ERR, "[DBSizeCheck = %s:%s]", m_cSession, m_cOdbcSession);
		return -1;
	}

	//// �擾
	while (TRUE) {
		int sqlrc = clsDB.Fetch();
		if (sqlrc == KIZUODBC_FETCH_OK) {						// �f�[�^�L��
			//clsDB.GetData(1, buf, sizeof(buf));										// DB����
			clsDB.GetData(2, buf, sizeof(buf));		m_DiskSpace.dTotalSize = atof(buf);	// �f�[�^�x�[�X�̃T�C�Y (%f MB)�ƂȂ��Ă��邩�璍��
			clsDB.GetData(3, buf, sizeof(buf));		m_DiskSpace.dFreeSize  = atof(buf);	// �����蓖�ė̈� (%f MB)�ƂȂ��Ă��邩�璍��																
			m_DiskSpace.dFreeRitu = m_DiskSpace.dFreeSize / m_DiskSpace.dTotalSize;		// �󂫗��Z�o												
			
		} else if( sqlrc == KIZUODBC_FETCH_NODATE ) {			// �f�[�^����
			break;	
		} else {												// �G���[
			syslog(SYSNO_DB_FETCH_ERR, "[DBSizeCheck = %s:%s]", m_cSession, m_cOdbcSession);
			return -1;
		}
	}

	//// �e�ʃ`�F�b�N (�����珇�Ƀ`�F�b�N)
	for(int ii=THREAD_MAXCNT-1; ii>=0; ii--) {
		if(0.0 == m_dIniDeleteRitu[ii]) continue;

		// �w���艺����Ă���H
		if( m_dIniDeleteRitu[ii] >= m_DiskSpace.dFreeRitu ) {
			return ii+1;
		}
	}
	return 0;
}

//------------------------------------------
// �g�����U�N�V�������O�T�C�Y�`�F�b�N
//------------------------------------------
void ControlManager::Check_TranSize() 
{
	// �`�F�b�N
	int min = mcls_pCycle->GetTranSizeMin();
	int now = mcls_pCycle->GetTranSizeNow();

	if( now == min ) {
		// �ُ� �� ����
		if( DIV_BOOL_TRUE != m_bTranStat ) {
			LOG(em_MSG) , "[Cont] [%s] �g�����U�N�V�������O�T�C�Y ���한�A", m_cSession);
			if( 0 != m_nIniTranSizeErrID ) KizuFunction::SetStatus(m_nIniTranSizeErrID, true);
			m_bTranStat = DIV_BOOL_TRUE;
		}

	} else {
		// ���� �� �ُ�
		//  �ʏ�10M��100�{�ɂȂ��������ς肨������
		//  10�{���炢�Ȃ�ł������o�C�i���f�[�^�o�^�������ȂǁA��u������\��������B
		if( now >= m_nIniTranErrSize ) {
			LOG(em_ERR) , "[Cont] [%s] �g�����U�N�V�������O�T�C�Y �ُ� [%d/%d]", m_cSession, now, min);
			if( DIV_BOOL_FALSE != m_bTranStat ) {
				if( 0 != m_nIniTranSizeErrID ) KizuFunction::SetStatus(m_nIniTranSizeErrID, false);

				m_bTranStat = DIV_BOOL_FALSE;
			}
		} else {
			LOG(em_WAR) , "[Cont] [%s] �g�����U�N�V�������O�T�C�Y �x�� [%d/%d]", m_cSession, now, min);
		}
	}
} 