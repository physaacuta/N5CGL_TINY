#include "StdAfx.h"
#include "StatusManager.h"


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
StatusManager::StatusManager(void) :
ThreadManager("StMgr"),
m_vItem(NULL)
{
	m_nDbColumnKiki		= GetPrivateProfileInt(KS_HISTRY, "MAX_DB_COLUMN_KIKI",	1, TASKINI_NAME);
	m_nDbColumnTemp		= GetPrivateProfileInt(KS_HISTRY, "MAX_DB_COLUMN_TEMP",	0, TASKINI_NAME);

	//// �n���h������
	m_evTimerCheck	  = CreateWaitableTimer(NULL, FALSE, NULL);
}
//------------------------------------------
// �f�X�g���N�^
//------------------------------------------
StatusManager::~StatusManager(void)
{
	//// �n���h�����
	CloseHandle(m_evTimerCheck);
}

//------------------------------------------
// �X���b�h�J�n�O���� (�X���b�h���s����(�V�O�i���Z�b�g�O)�ɃR�[���o�b�N)
//------------------------------------------
int StatusManager::ThreadFirst()
{
	//// �}�X�^�[�����擾
	while(true) {
		if( ! Init() ) {
			Sleep(10000);
			continue;
		}
		break;
	}

	//// 10�b�Œ�^�C�}�[ �N��
	__int64 ts = DB_CHECK_TIME * -10000;		// 1�~���b��100�i�m�b (�}�C�i�X�w��ŁA�w�莞�Ԍo�߂ŃV�O�i��) (��3������2��ڈȍ~�̋N���Ԋu)
	SetWaitableTimer(m_evTimerCheck, (LARGE_INTEGER *)&ts, DB_CHECK_TIME, NULL, NULL, FALSE);


	//// �C�x���g�ݒ�
	HANDLE hArray[] = { m_evTimerCheck
						};
	SetEventNum(sizeof(hArray)/sizeof(HANDLE), hArray);
	return 0;
}
//------------------------------------------
// �X���b�h�I���O���� (�I���V�O�i����ɃR�[���o�b�N)
//------------------------------------------
int StatusManager::ThreadLast()
{
	//// �^�C�}�[��~
	CancelWaitableTimer(m_evTimerCheck);

	return 0;
}

//------------------------------------------
// �X���b�h�C�x���g����
// int nEventNo �C�x���gNo (0�I���W��)
//------------------------------------------
void StatusManager::ThreadEvent(int nEventNo)
{
	////// �V�O�i���̏���
	enum {	EV_TIEM_CHECK = 0,				// DB�A�N�Z�X�`�F�b�N
	};

	////// �V�O�i����������
	switch (nEventNo) {

//-----------------------------------------------------------------------------------------------
	case EV_TIEM_CHECK: 					// �ۑ��t�H���_�A�N�Z�X�`�F�b�N
		CheckItem();
		return;

//-----------------------------------------------------------------------------------------------
	default :
		// ���肦�Ȃ��I�I
		LOG(em_ERR), "[%s] ThreadEvent EventNo NG=%d", my_sThreadName, nEventNo );
		return;
	}
}

// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ���C������

//------------------------------------------
// DB����ݒ�����擾
//------------------------------------------
bool StatusManager::Init()
{
	CString sql;					// SQL��
	CString	sWk;
	char cWk[128];

	//// �p�����[�^������
	CString cDbName;
	cDbName = DB_ST_HISTORY_MASTER;

	//// ODBC�ڑ��N���X �C���X�^���X����
	OdbcBase clsDB;
	if( ! clsDB.Connect(TASKINI_NAME)) {
		// �f�[�^�x�[�X�ڑ��G���[
		LOG(em_ERR), "[%s] DB�ڑ��G���[", my_sThreadName);
		syslog(SYSNO_DB_CONNECT_ERR, "");
		return false;
	}

	//// SQL���쐬
	sql =  "SELECT";
	sql += " �敪, ���W����";
	for(int ii=0; ii<m_nDbColumnKiki; ii++) {
		sWk.Format(", ISNULL(����%d, '')", ii+1);
		sql += sWk;
	}
	for(int ii=0; ii<m_nDbColumnKiki; ii++) {
		sWk.Format(", ISNULL(�@��ID_%d, 0)", ii+1);
		sql += sWk;
	}
	sql += " FROM ";
	sql += cDbName;
	sql += " ORDER BY �敪 ASC";



	//// ���s
	if( !clsDB.ExecuteDirect(sql)) {
		syslog(SYSNO_DB_EXECUTEDIRECT_ERR, "[%s]", cDbName);
		return false;
	}

	//// �擾
	while (TRUE) {
		int sqlrc = clsDB.Fetch();
		if (sqlrc == KIZUODBC_FETCH_OK) {					// �f�[�^�L��
		
			TYP_LIST t;

			// DB����擾
			clsDB.GetData(1, &t.nKey);
			clsDB.GetData(2, &t.nCycle);

			for(int ii=0; ii<m_nDbColumnKiki; ii++) {
				//memset(cWk, 0x00, sizeof(cWk));
				clsDB.GetData(3 + ii, cWk, sizeof(cWk));
				t.sName[ii] = cWk;
			}
			for(int ii=0; ii<m_nDbColumnKiki; ii++) {
				clsDB.GetData(3 + m_nDbColumnKiki + ii, &t.nId[ii]);
			}


			// ���̑�
			if(0 < t.nCycle) {
				t.nNextCnt = 1;						// ����́A1����ɑS����x�ǂ�
			} else {
				t.nNextCnt = -1;					// �ΏۊO
			}
			for(int ii=0; ii<MAX_COLUMN; ii++) {
				t.nNewN[ii] = 0;
				t.sNewS[ii] = ""; 
			}

			// ���X�g�ɒǉ�
			m_vItem.push_back(t);
	

		} else if( sqlrc == KIZUODBC_FETCH_NODATE ) {		// �f�[�^����
			break;	
		} else {								// �G���[
			syslog(SYSNO_DB_FETCH_ERR, "[%s]", cDbName);
			return false;
		}
	}
	return true;
}

//------------------------------------------
// ���W�^�C�~���O���Ď�
//------------------------------------------
void StatusManager::CheckItem()
{
	for(int ii=0; ii<(int)m_vItem.size(); ii++) {
		// ���������ΏۊO�� ����
		if( 0  >= m_vItem[ii].nCycle )   continue;
		if( -1 == m_vItem[ii].nNextCnt ) continue;

		// �^�C�}�[�`�F�b�N
		m_vItem[ii].nNextCnt -= 1;
		if( 0 < m_vItem[ii].nNextCnt) continue;

		// ���s
		GetStatusInf(&m_vItem[ii]);

		// ���� �Z�b�g
		m_vItem[ii].nNextCnt = m_vItem[ii].nCycle;
	}
}

//------------------------------------------
// DB����ݒ�����擾
// TYP_LIST* t �Ώۏ��
//------------------------------------------
bool StatusManager::GetStatusInf(TYP_LIST* t)
{
	CString sql;					// SQL��
	CString	sWk;
	int nSetCnt = 0;				// �@��ID����


	//// ODBC�ڑ��N���X �C���X�^���X����
	OdbcBase clsDB;
	if( ! clsDB.Connect(TASKINI_NAME)) {
		// �f�[�^�x�[�X�ڑ��G���[
		LOG(em_ERR), "[%s] DB�ڑ��G���[", my_sThreadName);
		syslog(SYSNO_DB_CONNECT_ERR, "");
		return false;
	}


	// ================================
	//// ���݂̋@���ԂƎ擾
	sql =  "SELECT �@��ID, �@���� ";
	sql += "FROM ";
	sql += DB_STATUS_INF;
	sql += " WHERE ";

	for(int ii=0; ii<m_nDbColumnKiki; ii++) {
		if( 0 >= t->nId[ii] ) continue;
		if( 0 != nSetCnt) sql += "OR ";

		sWk.Format("�@��ID=%d ", t->nId[ii]);
		sql += sWk;

		nSetCnt ++;
	}

	//// ���s
	if( !clsDB.ExecuteDirect(sql)) {
		syslog(SYSNO_DB_EXECUTEDIRECT_ERR, "[%s]", DB_STATUS_INF);
		return false;
	}


	// ================================
	// �o�b�t�@�ɕۑ�
	memset(t->bSetN, 0x00, sizeof(t->bSetN));
	memset(t->bSetS, 0x00, sizeof(t->bSetS));

	//// �擾
	int nId = 0;
	int nWk = 0;
	while (TRUE) {
		int sqlrc = clsDB.Fetch();
		if (sqlrc == KIZUODBC_FETCH_OK) {					// �f�[�^�L��

			// ����ID�ɃZ�b�g����
			clsDB.GetData(1, &nId);
			clsDB.GetData(2, &nWk);
			for(int ii=0; ii<m_nDbColumnKiki; ii++) {
				if( nId != t->nId[ii] ) continue;
				t->bSetN[ii] = true;
				t->nNewN[ii] = nWk;
				break;
			}

		} else if( sqlrc == KIZUODBC_FETCH_NODATE ) {		// �f�[�^����
			break;	
		} else {								// �G���[
			syslog(SYSNO_DB_FETCH_ERR, "[%s]", DB_STATUS_INF);
			return false;
		}
	}
	
	//// ���ݎ������Z�b�g
	CTime tWk = CTime::GetCurrentTime();
	t->sNewTime = tWk.Format("%Y/%m/%d %H:%M:%S");

	//// �p����ŕt����� �� �t�^
	SetStringData(t);						


	// ================================
	// �����ɓo�^
	sql = "INSERT INTO ";
	sql += DB_ST_HISTORY_INF;
	sql += " VALUES(";

	// �f�[�^��
	CreateSQL::sqlColumnInsert(&sql, t->sNewTime);
	CreateSQL::sqlColumnInsert(&sql, t->nKey);

	for(int ii=0; ii<m_nDbColumnKiki; ii++) {
		if( t->bSetN[ii] )	CreateSQL::sqlColumnInsert(&sql, t->nNewN[ii]);
		else				CreateSQL::sqlColumnInsert(&sql, NULL, 0);
	}
	for(int ii=0; ii<m_nDbColumnTemp; ii++) {
		if( t->bSetS[ii] )	CreateSQL::sqlColumnInsert(&sql, t->sNewS[ii]);
		else				CreateSQL::sqlColumnInsert(&sql, NULL, 0);
	}
	CreateSQL::sqlColumnTrim(&sql);		// �Ō�̃J���}����菜�� 
	sql += ")";

	//// ���s
	if( ! clsDB.ExecuteDirect(sql) ) {
		LOG(em_ERR), "[%s] �o�^���s [%s] [%s]", my_sThreadName, DB_ST_HISTORY_INF, sql);
		syslog(SYSNO_DB_EXECUTEDIRECT_ERR, "[%s]", DB_ST_HISTORY_INF);
		return false;
	}
	return true;
}



