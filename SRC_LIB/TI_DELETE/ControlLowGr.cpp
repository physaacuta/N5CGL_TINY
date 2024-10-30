#include "StdAfx.h"
#include "ControlLowGr.h"


//------------------------------------------
// �R���X�g���N�^
//------------------------------------------
ControlLowGr::ControlLowGr() :
ThreadManager("LowGrCont")
{
	char		cWk[1024];
	CString		sWk;


	//// ini�t�@�C���Ǎ�
	m_nIniThreadCount = GetPrivateProfileInt(TI_DELETE, "LOWGR_THREAD_COUNT", 0, TASKINI_NAME);
	m_nIniCycle = GetPrivateProfileInt(TI_DELETE, "LOWGR_CYCLE", 60, TASKINI_NAME);		
	m_nIniCycle = m_nIniCycle * 60 * 1000;					// ����ms 
	m_nIniDeleDay = GetPrivateProfileInt(TI_DELETE, "LOWGR_DELEDAY", 0, TASKINI_NAME);

	GetPrivateProfileString(TI_DELETE, "SQL_SELECT_KEY", "", cWk, sizeof(cWk), TASKINI_NAME );
	m_cIniSqlSelectKey = cWk;


	// �e�[�u���擾
	GetPrivateProfileString(TI_DELETE, "COILRESULT_TBL", "", cWk, sizeof(cWk), TASKINI_NAME );
	m_typSet.cTblResult = cWk;
	GetPrivateProfileString(TI_DELETE, "COILRESULT_TIME", "", cWk, sizeof(cWk), TASKINI_NAME );
	m_typSet.cKeyResultTime = cWk;
	GetPrivateProfileString(TI_DELETE, "MAINKEY_NAME", "", cWk, sizeof(cWk), TASKINI_NAME );
	m_typSet.cKeyMain = cWk;
	GetPrivateProfileString(TI_DELETE, "SUBKEY_NAME", "", cWk, sizeof(cWk), TASKINI_NAME );
	m_typSet.cKeySub = cWk;

	m_typSet.nMainCnt = 0;
	for(int ii=0; ii<MAX_TBL_MAIN; ii++) {
		CString		sKey;
		int			curPos	= 0;				// ��؂�ʒu

		// ���C���f�[�^�擾
		sKey.Format("TBL_KEY_%d", ii+1); 
		GetPrivateProfileString(TI_DELETE, sKey, "", cWk, sizeof(cWk), TASKINI_NAME );
		if(0==strlen(cWk)) continue;
		sWk = cWk;

		// ���C���e�[�u���ݒ�擾
		SETTING_TBL::MAIN& typ = m_typSet.main[m_typSet.nMainCnt];
		typ.nSubCnt	= 0;
		GetColumnVal(sWk, curPos, typ.cTblMain );
		GetColumnVal(sWk, curPos, typ.cSqlWhere);

		// �T�u�e�[�u���擾
		for(int jj=0; jj<MAX_TBL_SUB; jj++) {
			sKey.Format("TBL_SUB_%d_%d", ii+1, jj+1); 
			GetPrivateProfileString(TI_DELETE, sKey, "", cWk, sizeof(cWk), TASKINI_NAME );
			if(0==strlen(cWk)) continue;

			typ.cTblSub[ typ.nSubCnt ] = cWk;
			typ.nSubCnt ++;
		}
		m_typSet.nMainCnt ++;
	}


	//// ���̑��������
	m_bEnable = __BOOL m_nIniThreadCount;

	//// �V�O�i������
	m_evTimeCycle	= CreateWaitableTimer(NULL, FALSE, NULL);	
	m_evEndDelete	= CreateEvent(NULL, FALSE, FALSE, NULL);	
	for(int ii=0; ii<THREAD_MAXCNT; ii++) {
		mcls_pLowGr[ii] = NULL;
	}
}

//------------------------------------------
// �f�X�g���N�^
//------------------------------------------
ControlLowGr::~ControlLowGr(void)
{
	// �J��
	CancelWaitableTimer(m_evTimeCycle);
	CloseHandle(m_evTimeCycle);
	CloseHandle(m_evEndDelete);
}


//------------------------------------------
// �폜�X���b�h���������� (��x�̂݋N��)
//------------------------------------------
void ControlLowGr::WorkerInit()
{
	//// ���[�J�[�X���b�h ������
	for(int ii=0; ii<m_nIniThreadCount; ii++) {
		mcls_pLowGr[ii] = new DeleLowGr(ii+1, m_evEndDelete, &m_typSet);
		mcls_pLowGr[ii]->SetLogMgr(mcls_pLog); 
	}	

	//// ���[�J�[�X���b�h �N��
	for(int ii=0; ii<m_nIniThreadCount; ii++) {
		mcls_pLowGr[ii]->Start();
	}
}


//------------------------------------------
// �폜�X���b�h��~���� (��x�̂݋N��)
//------------------------------------------
void ControlLowGr::WorkerStop()
{
	//// ���[�J�[�X���b�h ��~
	// �܂��S�̂ɒ�~�w����ʒm����
	for(int ii=0; ii<THREAD_MAXCNT; ii++) {
		if(NULL == mcls_pLowGr[ii]) continue;
		mcls_pLowGr[ii]->SetStop();
	}
	LOG(em_MSG), "[%s] �e�q�X���b�h��~�v������", my_sThreadName );
}

//------------------------------------------
// �폜�X���b�h�I������ (��x�̂݋N��)
//------------------------------------------
void ControlLowGr::WorkerExit()
{
	//// ���[�J�[�X���b�h �J��
	for(int ii=0; ii<THREAD_MAXCNT; ii++) {
		if(NULL == mcls_pLowGr[ii]) continue;
		mcls_pLowGr[ii]->Stop(20000);
		SAFE_DELETE( mcls_pLowGr[ii] );
	}
}


// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// �X���b�h����

//------------------------------------------
// �X���b�h�J�n�O���� (�X���b�h���s����(�V�O�i���Z�b�g�O)�ɃR�[���o�b�N)
//------------------------------------------
int ControlLowGr::ThreadFirst()
{
	//// ������^�C�}�[�Z�b�g
	__int64 ts = (__int64)m_nIniCycle * -10000;	// 1�ʕb��100�i�m�b (�}�C�i�X�w��ŁA�w�莞�Ԍo�߂ŃV�O�i��) (��3������2��ڈȍ~�̋N���Ԋu)
	SetWaitableTimer(m_evTimeCycle, (LARGE_INTEGER *)&ts, m_nIniCycle, NULL, NULL, FALSE);


	//// �C�x���g�ݒ�
	HANDLE hArray[] = { m_evTimeCycle,			// ������^�C�}�[
						m_evEndDelete			// DB�폜����
						};
	SetEventNum(sizeof(hArray)/sizeof(HANDLE), hArray);
	return 0;
}

//------------------------------------------
// �X���b�h�I���O���� (�I���V�O�i����ɃR�[���o�b�N)
//------------------------------------------
int ControlLowGr::ThreadLast()
{
	return 0;
}

//------------------------------------------
// �X���b�h�C�x���g����
// int nEventNo �C�x���gNo (0�I���W��)
//------------------------------------------
void ControlLowGr::ThreadEvent(int nEventNo)
{
	////// �V�O�i���̏���
	enum {	EV_TIMECYCLE = 0,				// ������^�C�}�[
			EV_END_DELETE					// DB�폜����
	};

	////// �V�O�i����������
	switch (nEventNo) {

//-----------------------------------------------------------------------------------------------
	case EV_TIMECYCLE:											// ������^�C�}�[
		if( ! CheckDeleListItem()) {
			LOG(em_WAR), "[%s] ��������s���s", my_sThreadName );
			break;
		}
		if( 0 != m_nIniDeleDay ) {
			if( ! BackUpDeleListItem()) {
				LOG(em_WAR), "[%s] �o�b�N�A�b�v���s���s", my_sThreadName );
				break;
			}
		}
		break;
//-----------------------------------------------------------------------------------------------
	case EV_END_DELETE:											// DB�폜����
		if( ! CheckDeleListItem()) {
			LOG(em_WAR), "[%s] ��������s���s", my_sThreadName );
			break;
		}

		if( 0 != m_nIniDeleDay ) {
			if( ! BackUpDeleListItem()) {
				LOG(em_WAR), "[%s] �o�b�N�A�b�v���s���s", my_sThreadName );
				break;
			}
		}
	
		break;

//-----------------------------------------------------------------------------------------------
	default :
		// ���肦�Ȃ��I�I
		LOG(em_ERR), "[%s] ThreadEvent EventNo NG=%d", my_sThreadName, nEventNo );
		return;
	}
}


// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ���C��

//------------------------------------------
// �폜�Ώۂ��r���Ǘ�No���`�F�b�N
// char const* cKey �`�F�b�N�����L�[
// �߂�l�F 0�`:�폜�ΏۃC���f�b�N�X  -1:�󂫖���  -9:���Ɏ��s��
//------------------------------------------
int ControlLowGr::CheckExecItem(char const* cKey)
{
	int nIsWait = -1;
	for(int ii=0; ii<m_nIniThreadCount; ii++) {
		// �󂫂͂���
		if(! mcls_pLowGr[ii]->IsExec() ) {		
			nIsWait = ii;
			continue;
		}

		// ������r���Ǘ�No�Ŋ��Ɏ��s���Ă�����̂�����B
		if( 0 == strcmp(cKey, mcls_pLowGr[ii]->ExecKey()) ) {
			return -9;
		}
	}
	return nIsWait;
}

//------------------------------------------
// ���QGr�폜�Ώۃ`�F�b�N
//   �����T�[�o�[���擾���������ꍇ�̂�
//------------------------------------------
bool ControlLowGr::CheckDeleListItem()
{
	char	cKey[64];
	int		nRetc;

	//// �󂫗L��H
	if( 0 > CheckExecItem("") ) return true;

	//// ODBC�ڑ��N���X �C���X�^���X����
	OdbcBase clsDB;
	if( ! clsDB.Connect(TASKINI_NAME, INI_DB)) {
		// �f�[�^�x�[�X�ڑ��G���[
		LOG(em_ERR), "[%s] [%s] DB�ڑ��G���[", my_sThreadName, INI_DB);
		syslog(SYSNO_DB_CONNECT_ERR, "");
		return false;
	}

	//// ���QGr�폜�Ώۂ��r���Ǘ�No���擾
	if( !clsDB.ExecuteDirect(m_cIniSqlSelectKey)) {
		LOG(em_ERR), "[%s] �L�[�擾 [%s]", my_sThreadName, m_cIniSqlSelectKey);
		syslog(SYSNO_DB_EXECUTEDIRECT_ERR, "[%s:�L�[�擾]", my_sThreadName);
		return false;
	}

	while(true) {
		int sqlrc = clsDB.Fetch();
		if (sqlrc == KIZUODBC_FETCH_OK) {						// �f�[�^�L��
			
			clsDB.GetData(1, cKey, sizeof(cKey));

			//// �󂫃`�F�b�N
			nRetc = CheckExecItem(cKey);
			if( 0 <= nRetc ) {
				// �҂���Ԃ̃X���b�h�Ɏ��s�˗����s��
				mcls_pLowGr[nRetc]->SetExec(cKey);
			} else if( -1 == nRetc) {
				// �󂫖���
				return true;
			}
	
		} else if( sqlrc == KIZUODBC_FETCH_NODATE ) {			// �f�[�^����
			break;
		} else {								// �G���[
			syslog(SYSNO_DB_FETCH_ERR, "[%s:��L�[�擾����]", my_sThreadName);
			return false;
		}
	}
	return true;
}

//------------------------------------------
// ���QGr�폜�Ώۃ`�F�b�N
//   3���ȏ�o�߂������QGr (�c�[�̃o�b�N�A�b�v�폜����)
//------------------------------------------
bool ControlLowGr::BackUpDeleListItem()
{
	CString sql;
	char	cKey[64];
	int		nRetc;

	//// �󂫗L��H
	if( 0 > CheckExecItem("") ) return true;

	//// ODBC�ڑ��N���X �C���X�^���X����
	OdbcBase clsDB;
	if( ! clsDB.Connect(TASKINI_NAME, INI_DB)) {
		// �f�[�^�x�[�X�ڑ��G���[
		LOG(em_ERR), "[%s] [%s] DB�ڑ��G���[", my_sThreadName, INI_DB);
		syslog(SYSNO_DB_CONNECT_ERR, "");
		return false;
	}


	//// 3���O�̋��E���r���Ǘ�No
	char cOldKey[64];
	CString wk;
	SYSTEMTIME STime;
	GetLocalTime(&STime);
	sql.Format("SELECT %s FROM %s WHERE %s+%d<'%d/%d/%d'",
		m_typSet.cKeyMain, m_typSet.cTblResult, m_typSet.cKeyResultTime, m_nIniDeleDay, STime.wYear, STime.wMonth, STime.wDay);
	if( ! clsDB.GetSelectKey(sql, sizeof(cOldKey), cOldKey) ) {
		LOG(em_INF), "[%s] 3���O�̎�L�[�����B���QGr�폜��� [%s]", my_sThreadName, sql);
		//LOG(em_ERR), "[%s] ��L�[�擾���s [%s]", my_sThreadName, sql);
		//syslog(SYSNO_DB_EXECUTEDIRECT_ERR, "[%s:��L�[�擾]", m_typSet.cTblResult);
		//return false;
	}


	//// 3���O�� ���QGr�����݂��� �r���Ǘ�No���擾
	sql = "";
	for(int ii=0; ii<m_typSet.nMainCnt; ii++) {
		wk.Format("SELECT DISTINCT %s FROM %s WHERE %s<'%s' %s",
			m_typSet.cKeyMain, m_typSet.main[ii].cTblMain, m_typSet.cKeyMain, cOldKey, m_typSet.main[ii].cSqlWhere );
		sql += wk;

		if(ii != m_typSet.nMainCnt-1) sql += " UNION ALL ";
	}


	if( !clsDB.ExecuteDirect(sql)) {
		LOG(em_ERR), "[%s] �L�[�擾 [%s]", my_sThreadName, sql);
		syslog(SYSNO_DB_EXECUTEDIRECT_ERR, "[%s:�L�[�擾]", m_typSet.main[0].cTblMain);
		return false;
	}


	while(true) {
		int sqlrc = clsDB.Fetch();
		if (sqlrc == KIZUODBC_FETCH_OK) {						// �f�[�^�L��
			
			clsDB.GetData(1, cKey, sizeof(cKey));

			//// �󂫃`�F�b�N
			nRetc = CheckExecItem(cKey);
			if( 0 <= nRetc ) {
				LOG(em_WAR), "[%s] �o�b�N�A�b�v�ŋ������QGr�폜���{�I [%s] <%d>", my_sThreadName, cKey, nRetc+1);
				syslog(500, "[%s]", cKey);

				// �҂���Ԃ̃X���b�h�Ɏ��s�˗����s��
				mcls_pLowGr[nRetc]->SetExec(cKey);
			} else if( -1 == nRetc) {
				// �󂫖���
				return true;
			}
	
		} else if( sqlrc == KIZUODBC_FETCH_NODATE ) {			// �f�[�^����
			break;
		} else {								// �G���[
			syslog(SYSNO_DB_FETCH_ERR, "[%s:��L�[�擾����]", m_typSet.main[0].cTblMain);
			return false;
		}
	}
	return true;
}