#include "StdAfx.h"
#include "DefragManager.h"

// �������[���[�N���o ----->>>
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif
// <<<------------------------

//------------------------------------------
// �R���X�g���N�^
// int id ������ID (1�I���W��)
// CString cSession �Z�b�V����
//------------------------------------------
DefragManager::DefragManager(int id, char* cSession) :
ThreadManager( GetThreadNameArray("DefragMgr", id) ),
m_cSession(cSession),
m_pExecDefrag(NULL),
m_nTblNum(0),
m_pTblDef(NULL),
m_nTblExecNum(0),
m_nIdxExecNum(0),
m_bRunCancel(false)
{
	//// �n���h������
	m_evTimerExec  = CreateWaitableTimer(NULL, FALSE, NULL);

	m_nDefragIntervalDay = GetPrivateProfileInt(TI_DELETE, "DEFRAG_INTERVAL_DAY", 0, TASKINI_NAME);
	m_LastDefragTime = CTime::GetCurrentTime();


	//----------------------------------------
	////// �e�[�u�����擾
	int		nTblCnt;				// ����
	char    cWk[1024];
	CString strVal;
	GetPrivateProfileString(m_cSession, "DB_ODBC", "", cWk, sizeof(cWk), TASKINI_NAME);
	m_cOdbcSession = cWk;


	//// �������擾
	nTblCnt = 0;
	while(true) {
		strVal.Format("ZI_%d", nTblCnt+1);
		GetPrivateProfileString(m_cSession, strVal, "", cWk, sizeof(cWk), TASKINI_NAME);		
		if(0 >= strlen(cWk) ) break;
		nTblCnt++;
	}
	m_nTblNum = nTblCnt;
	//// 0���ȊO�̏ꍇ �e�[�u������
	if(0 != m_nTblNum) {
		nTblCnt = 0;
		m_pTblDef = new DEF_TBL[m_nTblNum];
		for(int ii=0; ii<m_nTblNum; ii++) {
			strVal.Format("ZI_%d", ii+1);
			GetPrivateProfileString(m_cSession, strVal, "", cWk, sizeof(cWk), TASKINI_NAME);		
		
			if( 0 < strlen(cWk) ) {
				strVal = cWk;
				
				// �g�[�N������
				int		rec = 0;
				int		curPos = 0;
				CString strToken = strVal.Tokenize(",", curPos);
				while ( strToken != "" ) {
					strToken.Trim(_T(" 	"));		// ini�t�@�C���ݒ蕶���̑O��̃X�y�[�X�A�^�u����菜��

					switch( rec ) {
						case 0:						// �e�[�u����
							m_pTblDef[ii].cDefTable = strToken; 
							break;
						default:
							break;
					}

					// ����
					strToken = strVal.Tokenize(",", curPos);
					rec ++;
				}
			}
		}
	}
}

//------------------------------------------
// �f�X�g���N�^
//------------------------------------------
DefragManager::~DefragManager(void)
{
	//// �n���h�����
	CloseHandle(m_evTimerExec);
	// �J��
	SAFE_DELETE_ARRAY(m_pTblDef); 
}

//------------------------------------------
// �X���b�h�J�n�O���� (�X���b�h���s����(�V�O�i���Z�b�g�O)�ɃR�[���o�b�N)
//------------------------------------------
int DefragManager::ThreadFirst()
{
	//// ���O����
	//const int nCycle = (2 * 60 * 1000) ;
	const int nCycle = (3 * 1000) ;	
	__int64 ts = nCycle * -10000;			// 2������
	SetWaitableTimer(m_evTimerExec, (LARGE_INTEGER *)&ts, nCycle, NULL, NULL, FALSE);

	//// �C�x���g�ݒ�
	HANDLE hArray[] = { m_evTimerExec
						};
	SetEventNum(sizeof(hArray)/sizeof(HANDLE), hArray);
	return 0;
}

//------------------------------------------
// �X���b�h�I���O���� (�I���V�O�i����ɃR�[���o�b�N)
//------------------------------------------
int DefragManager::ThreadLast()
{
	// �������~
	CancelWaitableTimer(m_evTimerExec);
	return 0;
}

//------------------------------------------
// �X���b�h�C�x���g����
// int nEventNo �C�x���gNo (0�I���W��)
//------------------------------------------
void DefragManager::ThreadEvent(int nEventNo)
{
	////// �V�O�i���̏���
	enum {	EV_TIMER_EXEC = 0,				// �������s ������^�C�}
	};

	////// �V�O�i����������
	switch (nEventNo) {

//-----------------------------------------------------------------------------------------------
	case EV_TIMER_EXEC: 					// �������s ������^�C�}
		Exec();
		return;

	
//-----------------------------------------------------------------------------------------------
	default :
		// ���肦�Ȃ��I�I
		LOG(em_ERR), "[%s] ThreadEvent EventNo NG=%d", my_sThreadName, nEventNo );
		return;
	}
}

//------------------------------------------
// �C���f�b�N�X �f�t���O���s
//------------------------------------------
void DefragManager::Exec()
{
	if( ! IsExec() ) return;
	
	//------------------------------------
	// ���O����
	//// �o�ߎ��ԂŁA�Ď��s�\���`�F�b�N
	CTimeSpan	ts(m_nDefragIntervalDay, 0, 0, 0);	// ��
//��	CTimeSpan	ts(0,0,0,20);	// ��
	CTime		ans = m_LastDefragTime + ts;
	if( ans >= CTime::GetCurrentTime() ) return;


	if( 0 != m_nTblExecNum || 0 != m_nIdxExecNum) {
		LOG(em_INF), "[%s] �f�t���O�ĊJ (%d:%d)", my_sThreadName, m_nTblExecNum, m_nIdxExecNum);
	}



	//------------------------------------
	// ���s
	int nRetc;			// �I����� (0:���� 1:�L�����Z��  -1:�ُ�)
	for(int ii=m_nTblExecNum; ii<m_nTblNum; ii++) {
		std::vector<CString>			vIdxName;			// �C���f�b�N�X�ꗗ

		//// �C���f�b�N�X�ꗗ�擾
		if( ! GetIndexTables(m_pTblDef[ii].cDefTable, &vIdxName) ) continue;
		if( ! IsExec() ) break; 
	
		//// �f�t���O���s
		LOG(em_MSG), "[%s] Index�e�[�u���擾 %s=%d", my_sThreadName, m_pTblDef[ii].cDefTable, vIdxName.size());
		for(int jj=m_nIdxExecNum; jj<vIdxName.size(); jj++) {
			if(ii >= m_nTblNum) continue;
			if(jj >= vIdxName.size()) continue;

			// ���s
			nRetc = ExecDefrag(m_pTblDef[ii].cDefTable, vIdxName[jj]);
			if( ! IsExec() ) break; 
			
			// index�P�� �ł����ƏI�����
			if( IsExec() && 0 == nRetc ) {
				m_nIdxExecNum = jj+1;
			}
		}

		// table�P�� �ł����ƏI�����
		if( IsExec() && 0 == nRetc ) {
			m_nTblExecNum = ii+1;
		}
		m_nIdxExecNum = 0;
	}

	//------------------------------------
	// �G���h
	// �S�� �����ƏI������ꍇ�A���s�r���t���O�͏�����
	if( IsExec() && 0 == nRetc) {
		LOG(em_INF), "[%s] �f�t���O����", my_sThreadName);

		m_nTblExecNum = 0;
		m_nIdxExecNum = 0;

		m_LastDefragTime = CTime::GetCurrentTime();// = CTime.GetCurrentTime();
	}
}

//------------------------------------------
// �C���f�b�N�X�ꗗ�擾
// char const* pName		�e�[�u����
// std::vector<CString>* v	�e�[�u�����ɕR�t���C���f�b�N�X�ꗗ
//------------------------------------------
bool DefragManager::GetIndexTables(char const* pName, std::vector<CString>* v)
{
	char	cWk[256];

	//----------------------------------------
	// �ꗗ�擾
	CString		sql;
	sql = "SELECT i.name FROM sys.indexes AS i INNER JOIN sysobjects AS o ON i.object_id = o.id WHERE o.xtype='U'";
	sql.AppendFormat(" AND o.name='%s'", pName); 


	//// ODBC�ڑ��N���X �C���X�^���X����
	OdbcBase clsDB;
	if( ! clsDB.Connect(TASKINI_NAME, m_cOdbcSession)) {
		// �f�[�^�x�[�X�ڑ��G���[
		LOG(em_ERR), "[%s] DB�ڑ��G���[", my_sThreadName);
		syslog(SYSNO_DB_CONNECT_ERR, "");
		return false;
	}
	if( !clsDB.ExecuteDirect(sql)) {
		LOG(em_ERR), "[%s] SQL���s�G���[", my_sThreadName);
		syslog(SYSNO_DB_EXECUTEDIRECT_ERR, "�Ǘ��e�[�u��" );
		return false;
	}
	//// �擾
	while (TRUE) {
		int sqlrc = clsDB.Fetch();
		if (sqlrc == KIZUODBC_FETCH_OK) {					// �f�[�^�L��
		
			clsDB.GetData(1, cWk, sizeof(cWk));
			v->push_back( cWk ); 

		} else if( sqlrc == KIZUODBC_FETCH_NODATE ) {		// �f�[�^����
			break;	
		} else {								// �G���[
			syslog(SYSNO_DB_FETCH_ERR, "�Ǘ��e�[�u��");
			return false;
		}
	}
	return true;
}

//------------------------------------------
// �C���f�b�N�X �f�t���O���s
// ���A���		�I����� (0:���� 1:�L�����Z��  -1:�ُ�)
//------------------------------------------
int DefragManager::ExecDefrag(char const* pTblName, char const* pIdxName)
{
	//----------------------------------------
	// �č\��(�f�t���O)�FREORGANIZE : �I�����C������
	// �č\�z�FREBUILD : �I�����C�����s��
	CString		sql;
	sql.Format("ALTER INDEX %s on %s REORGANIZE", pIdxName, pTblName);



	//// ODBC�ڑ��N���X �C���X�^���X����
	OdbcBase clsDB;
	if( ! clsDB.Connect(TASKINI_NAME, m_cOdbcSession)) {
		// �f�[�^�x�[�X�ڑ��G���[
		LOG(em_ERR), "[%s] DB�ڑ��G���[", my_sThreadName);
		syslog(SYSNO_DB_CONNECT_ERR, "");
		return -1;
	}

	//// ���Ԍv��
	KizuPerformanceCounter clsT;
	clsT.InitAve(); 
	clsT.CounterStart();


	//// ���s
	int  nRetc = 0;
	bool bFast = true;
	while(true) {
		nRetc = clsDB.ExecuteDirectAsync(sql, bFast);
		if( 0 == nRetc || -1 == nRetc) break;

		// ���f�`�F�b�N
		if( ! IsExec() ) {
			LOG(em_MSG), "[%s] Index�f�t���O���f�F�� [%s:%s]", my_sThreadName, pTblName, pIdxName);
			if( ! clsDB.ExecuteDirectAsyncCancel() ) {
				LOG(em_MSG), "[%s] �L�����Z�����s [%s]", my_sThreadName, clsDB.GetLastErrMsg());
			} else {
				LOG(em_MSG), "[%s] �L�����Z������", my_sThreadName, pTblName, pIdxName);
			}
			return 1;
		}

//		LOG(em_MSG), "[%s] Index�f�t���O���c [%s:%s]", my_sThreadName, pTblName, pIdxName);
		bFast=false;
		Sleep(1000);
	}


	//// �I���
	clsT.CounterEnd();
	LOG(em_MSG), "[%s] Index�f�t���O���� [%s:%s] (%ds)", my_sThreadName, pTblName, pIdxName, (int)(clsT.GetCycleTime()/1000.0) );
	return nRetc;
}