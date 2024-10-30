#include "StdAfx.h"
#include "KizuDeleteManager.h"

// �������[���[�N���o ----->>>
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif
// <<<------------------------


// //////////////////////////////////////////////////
// ���ʂ̃��\�b�h
// //////////////////////////////////////////////////

//------------------------------------------
// �R���X�g���N�^
// int id ������ID (1�I���W��)
// int* waittime �҂����� [ms]
// HANDLE evEndCycle �I���V�O�i��
// HANDLE evEndSizeMin �I���V�O�i��
// CString cSession �����̐ݒ肪��`����Ă���ini�t�@�C���̃Z�b�V����
// CString cOdbcSession ODBC�ݒ�Z�b�V����
//------------------------------------------
KizuDeleteManager::KizuDeleteManager(int id, int* nWaitTime, HANDLE evEndCycle, HANDLE evEndSizeMin, CString cSession, CString cOdbcSession) :
my_nID(id),
my_pnWaitTime(nWaitTime),
my_evEndCycle(evEndCycle),
my_evEndSizeMin(evEndSizeMin),
my_cSession(cSession),
my_cOdbcSession(cOdbcSession),
my_bIsExec_Cycle(false),
my_bIsExec_SizeMin(false),
my_bStopFlg(false),
my_nTranSizeMin(0),
my_nTranSizeNow(0),
mcls_pLock(NULL),
mcls_pLog(NULL)
{
	// �C�x���g����
	my_evCycle		= CreateEvent(NULL, FALSE, FALSE, NULL);
	my_evSizeMin	= CreateEvent(NULL, FALSE, FALSE, NULL);
	my_evExecFast	= CreateEvent(NULL, FALSE, FALSE, NULL);
}

//------------------------------------------
// �f�X�g���N�^
//------------------------------------------
KizuDeleteManager::~KizuDeleteManager(void)
{
	// �C�x���g�j��
	CloseHandle(my_evCycle);
	CloseHandle(my_evSizeMin);
	CloseHandle(my_evExecFast);
}

//------------------------------------------
// �X���b�h
//------------------------------------------
UINT WINAPI KizuDeleteManager::MainThread(LPVOID param)
{
	////// ���C���C���X�^���X���o��
	KizuDeleteManager *p = (KizuDeleteManager *)param;
	p->SetTid();								// ���X���b�hID�Z�b�g

	// WaitForMultipleObjects�C�x���g
	HANDLE	hArray[] = {p->my_evStop,
						p->my_evCycle,
						p->my_evSizeMin, 
						p->my_evExecFast
						};
	enum {	EV_STOP = WAIT_OBJECT_0,		// �X���b�h�I��
			EV_CYCLE,						// ��������s�C�x���g
			EV_SIZEMIN,						// DB�e�ʒቺ���s�C�x���g
			EV_EXEC_FAST,					// ������s
			EV_END	};
	
	// ���b�Z�[�W�L���[�ǂݍ���
	pLOG(em_MSG), "[KD] [%s] �X���b�h<%d>�J�n [0x%X]", p->my_cSession, p->my_nID, p->GetTid());
	while (1) {
		DWORD ret = WaitForMultipleObjects(EV_END, hArray, FALSE, INFINITE);
		switch (ret) {

//-----------------------------------------------------------------------------------------------
		case EV_STOP:												// �X���b�h�I��
			pLOG(em_MSG), "[KD] [%s] �X���b�h<%d>�I�� [0x%X]", p->my_cSession, p->my_nID, p->GetTid());
			return 0;

//-----------------------------------------------------------------------------------------------
		case EV_CYCLE:												// ��������s�C�x���g
			pLOG(em_INF), "[KD] [%s] �X���b�h<%d> ��������s�J�n", p->my_cSession, p->my_nID);
			p->my_bIsExec_Cycle = true;

			// --------------------->>>
			// �V�X�e�����̓Ǝ��������s
			p->ExecCycle();															// �C�ӂ̏���
			// <<<---------------------
			
			if(0 < p->my_Param.nLogSaveDay) {
				p->Delete_Log(p->my_Param.nLogSaveDay);								// ���O�폜����
			}
			p->Exec_Shrink();														// �g�����U�N�V�������k����
			
			p->my_bIsExec_Cycle = false;
			pLOG(em_INF), "[KD] [%s] �X���b�h<%d> ��������s�I��", p->my_cSession, p->my_nID);

			// ��������s���I�������ׁA�ʒm
			SetEvent(p->my_evEndCycle);
			break;

//-----------------------------------------------------------------------------------------------
		case EV_SIZEMIN:											// DB�e�ʒቺ���s�C�x���g
			pLOG(em_INF), "[KD] [%s] �X���b�h<%d> DB�e�ʒቺ���s�J�n", p->my_cSession, p->my_nID);
			p->my_bIsExec_SizeMin = true;
				
			// --------------------->>>
			// �V�X�e�����̓Ǝ��������s
			p->ExecSizeMin(); 														// �C�ӂ̏���
			// <<<---------------------

			p->Exec_Shrink();														// �g�����U�N�V�������k����

			p->my_bIsExec_SizeMin = false;
			pLOG(em_INF), "[KD] [%s] �X���b�h<%d> DB�e�ʒቺ���s�I��", p->my_cSession, p->my_nID);

			// �e�ʒቺ���s���I�������ׁA�ēx �e�ʃ`�F�b�N�ʒm
			SetEvent(p->my_evEndSizeMin);
			break;

//-----------------------------------------------------------------------------------------------
		case EV_EXEC_FAST:											// ������s
			pLOG(em_INF), "[KD] [%s] �X���b�h<%d> ������s�J�n", p->my_cSession, p->my_nID);
			p->my_bIsExec_SizeMin = true;
				
			// --------------------->>>
			// �V�X�e�����̓Ǝ��������s
			p->ExecFast(); 															// �C�ӂ̏���
			// <<<---------------------

			p->Exec_Shrink();														// �g�����U�N�V�������k����

			p->my_bIsExec_SizeMin = false;
			pLOG(em_INF), "[KD] [%s] �X���b�h<%d> ������s�I��", p->my_cSession, p->my_nID);

			// �e�ʒቺ���s���I�������ׁA�ēx �e�ʃ`�F�b�N�ʒm
			SetEvent(p->my_evEndSizeMin);
			break;

//-----------------------------------------------------------------------------------------------
		default :
			// ���肦�Ȃ��I�I
			break;
		}
	}
	return 0;
}


// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// �悭�g�p�����ȃ��\�b�h��` (���ʓI�Ɏg����悤�ɍ���Ă�����)
// �擾�֘A

//------------------------------------------
// �w��e�[�u���̍ŌẪL�[���擾
// char const *tbl �폜�e�[�u��
// char const *keyName ��L�[���� (���ڂ̌^�́A������^)
// int size �擾���镶����
// char *keyVal �擾��������
//------------------------------------------
bool KizuDeleteManager::Select_OldKey(char const *tbl, char const *keyName, int size, char *keyVal)
{
	CString sql;
	memset(keyVal, 0x00, size);
	//// ODBC�ڑ��N���X �C���X�^���X����
	OdbcBase clsDB;
	if( ! clsDB.Connect(TASKINI_NAME, my_cOdbcSession)) {
		// �f�[�^�x�[�X�ڑ��G���[
		LOG(em_ERR), "[KD] [%s] DB�ڑ��G���[", my_cSession);
		syslog(SYSNO_DB_CONNECT_ERR, "");
		return false;
	}
	//// �ŌẪL�[�擾
	sql.Format("SELECT ISNULL( Min(%s), '') FROM %s ", keyName, tbl);
	if( !clsDB.ExecuteDirect(sql)) {
		LOG(em_ERR), "[KD] [%s] �X���b�h<%d> �L�[�擾 [%s]", my_cSession, my_nID, sql);
		syslog(SYSNO_DB_EXECUTEDIRECT_ERR, "[�L�[�擾]");
		return false;
	}
	int sqlrc = clsDB.Fetch();
	if (sqlrc == KIZUODBC_FETCH_OK) {						// �f�[�^�L��
		clsDB.GetData(1, keyVal, size);
		if( 1 >= strlen(keyVal) ) return false;				// �f�[�^���� 
	} else if( sqlrc == KIZUODBC_FETCH_NODATE ) {			// �f�[�^����
		return false;	// ���肦�Ȃ�
	} else {								// �G���[
		syslog(SYSNO_DB_FETCH_ERR, "[��L�[�擾����]");
		return false;
	}
	return true;
}
bool KizuDeleteManager::Select_OldKey(char const *tbl, char const *keyName, char const *keySort, int size, char *keyVal)
{
	CString sql;
	memset(keyVal, 0x00, size);
	//// ODBC�ڑ��N���X �C���X�^���X����
	OdbcBase clsDB;
	if( ! clsDB.Connect(TASKINI_NAME, my_cOdbcSession)) {
		// �f�[�^�x�[�X�ڑ��G���[
		LOG(em_ERR), "[KD] [%s] DB�ڑ��G���[", my_cSession);
		syslog(SYSNO_DB_CONNECT_ERR, "");
		return false;
	}
	//// �ŌẪL�[�擾
	sql.Format("SELECT TOP 1 %s FROM %s ORDER BY %s ASC", keyName, tbl, keySort);
	if( !clsDB.ExecuteDirect(sql)) {
		LOG(em_ERR), "[KD] [%s] �X���b�h<%d> �L�[�擾 [%s]", my_cSession, my_nID, sql);
		syslog(SYSNO_DB_EXECUTEDIRECT_ERR, "[�L�[�擾]");
		return false;
	}
	int sqlrc = clsDB.Fetch();
	if (sqlrc == KIZUODBC_FETCH_OK) {						// �f�[�^�L��
		clsDB.GetData(1, keyVal, size);
		if( 1 >= strlen(keyVal) ) return false;				// �f�[�^���� 
	} else if( sqlrc == KIZUODBC_FETCH_NODATE ) {			// �f�[�^����
		return false;	// ���肦�Ȃ�
	} else {								// �G���[
		syslog(SYSNO_DB_FETCH_ERR, "[��L�[�擾����]");
		return false;
	}
	return true;
}


//------------------------------------------
// �w��e�[�u���̍ŌẪL�[���擾
// char const *tbl �폜�e�[�u��
// char const *keyName ��L�[���� (���ڂ̌^�́A���l�^)
// int *keyVal �擾��������
//------------------------------------------
bool KizuDeleteManager::Select_OldKey(char const *tbl, char const *keyName, int *keyVal)
{
	CString sql;
	*keyVal = 0;
	//// ODBC�ڑ��N���X �C���X�^���X����
	OdbcBase clsDB;
	if( ! clsDB.Connect(TASKINI_NAME, my_cOdbcSession)) {
		// �f�[�^�x�[�X�ڑ��G���[
		LOG(em_ERR), "[KD] [%s] DB�ڑ��G���[", my_cSession);
		syslog(SYSNO_DB_CONNECT_ERR, "");
		return false;
	}
	//// �ŌẪL�[�擾
	sql.Format("SELECT ISNULL( Min(%s), '') FROM %s ", keyName, tbl);
	if( !clsDB.ExecuteDirect(sql)) {
		LOG(em_ERR), "[KD] [%s] �X���b�h<%d> �L�[�擾 [%s]", my_cSession, my_nID, sql);
		syslog(SYSNO_DB_EXECUTEDIRECT_ERR, "[�L�[�擾]");
		return false;
	}
	int sqlrc = clsDB.Fetch();
	if (sqlrc == KIZUODBC_FETCH_OK) {						// �f�[�^�L��
		clsDB.GetData(1, keyVal);
	} else if( sqlrc == KIZUODBC_FETCH_NODATE ) {			// �f�[�^����
		return false;	// ���肦�Ȃ�
	} else {								// �G���[
		syslog(SYSNO_DB_FETCH_ERR, "[��L�[�擾����]");
		return false;
	}
	return true;
}
bool KizuDeleteManager::Select_OldKey(char const *tbl, char const *keyName, char const *keySort, int *keyVal)
{
	CString sql;
	*keyVal = 0;
	//// ODBC�ڑ��N���X �C���X�^���X����
	OdbcBase clsDB;
	if( ! clsDB.Connect(TASKINI_NAME, my_cOdbcSession)) {
		// �f�[�^�x�[�X�ڑ��G���[
		LOG(em_ERR), "[KD] [%s] DB�ڑ��G���[", my_cSession);
		syslog(SYSNO_DB_CONNECT_ERR, "");
		return false;
	}
	//// �ŌẪL�[�擾
	sql.Format("SELECT TOP 1 %s FROM %s ORDER BY %s ASC", keyName, tbl, keySort);
	if( !clsDB.ExecuteDirect(sql)) {
		LOG(em_ERR), "[KD] [%s] �X���b�h<%d> �L�[�擾 [%s]", my_cSession, my_nID, sql);
		syslog(SYSNO_DB_EXECUTEDIRECT_ERR, "[�L�[�擾]");
		return false;
	}
	int sqlrc = clsDB.Fetch();
	if (sqlrc == KIZUODBC_FETCH_OK) {						// �f�[�^�L��
		clsDB.GetData(1, keyVal);
	} else if( sqlrc == KIZUODBC_FETCH_NODATE ) {			// �f�[�^����
		return false;	// ���肦�Ȃ�
	} else {								// �G���[
		syslog(SYSNO_DB_FETCH_ERR, "[��L�[�擾����]");
		return false;
	}
	return true;
}
//------------------------------------------
// �w��e�[�u���̃T�u�L�[���擾
// char const *tbl �폜�e�[�u��
// char const *keyName ��L�[���� (���ڂ̌^�́A������^)
// char const *subName �T�u��L�[���� (���ڂ̌^�́A���ł�OK)
// char const *keyVal ��L�[
// int size �擾���镶����
// char *subVal �擾��������
//------------------------------------------
bool KizuDeleteManager::Select_SubKey(char const *tbl, char const *keyName, char const *subName, char const *keyVal, int size, char *subVal)
{
	CString sql;
	memset(subVal, 0x00, size);

	//// ODBC�ڑ��N���X �C���X�^���X����
	OdbcBase clsDB;
	if( ! clsDB.Connect(TASKINI_NAME, my_cOdbcSession)) {
		// �f�[�^�x�[�X�ڑ��G���[
		LOG(em_ERR), "[KD] [%s] DB�ڑ��G���[", my_cSession);
		syslog(SYSNO_DB_CONNECT_ERR, "");
		return false;
	}

	//// �ŌẪL�[�擾
	sql.Format("SELECT ISNULL( %s, '') FROM %s WHERE %s='%s'", subName, tbl, keyName, keyVal);
	if( !clsDB.ExecuteDirect(sql)) {
		LOG(em_ERR), "[KD] [%s] �X���b�h<%d> �L�[�擾 [%s]", my_cSession, my_nID, sql);
		syslog(SYSNO_DB_EXECUTEDIRECT_ERR, "[�L�[�擾]");
		return false;
	}
	int sqlrc = clsDB.Fetch();
	if (sqlrc == KIZUODBC_FETCH_OK) {						// �f�[�^�L��
		clsDB.GetData(1, subVal, size);
		if( 1 >= strlen(keyVal) ) return false;				// �f�[�^���� 
	} else if( sqlrc == KIZUODBC_FETCH_NODATE ) {			// �f�[�^����
		return false;	// ���肦�Ȃ�
	} else {								// �G���[
		syslog(SYSNO_DB_FETCH_ERR, "[�T�u�L�[�擾����]");
		return false;
	}
	return true;
}
bool KizuDeleteManager::Select_SubKey(char const *tbl, char const *keyName, char const *subName, char const *keyVal, int *subVal)
{
	CString sql;
	*subVal = 0;

	//// ODBC�ڑ��N���X �C���X�^���X����
	OdbcBase clsDB;
	if( ! clsDB.Connect(TASKINI_NAME, my_cOdbcSession)) {
		// �f�[�^�x�[�X�ڑ��G���[
		LOG(em_ERR), "[KD] [%s] DB�ڑ��G���[", my_cSession);
		syslog(SYSNO_DB_CONNECT_ERR, "");
		return false;
	}

	//// �ŌẪL�[�擾
	sql.Format("SELECT ISNULL( %s, '') FROM %s WHERE %s='%s'", subName, tbl, keyName, keyVal);
	if( !clsDB.ExecuteDirect(sql)) {
		LOG(em_ERR), "[KD] [%s] �X���b�h<%d> �L�[�擾 [%s]", my_cSession, my_nID, sql);
		syslog(SYSNO_DB_EXECUTEDIRECT_ERR, "[�L�[�擾]");
		return false;
	}
	int sqlrc = clsDB.Fetch();
	if (sqlrc == KIZUODBC_FETCH_OK) {						// �f�[�^�L��
		clsDB.GetData(1, subVal);
	} else if( sqlrc == KIZUODBC_FETCH_NODATE ) {			// �f�[�^����
		return false;	// ���肦�Ȃ�
	} else {								// �G���[
		syslog(SYSNO_DB_FETCH_ERR, "[�T�u�L�[�擾����]");
		return false;
	}
	return true;
}


// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// �f�[�^�e�ʊǗ�

//------------------------------------------
// �g�����U�N�V�������O ���k
//    (���э폜����ƃg�����U�N�V�������O���������剻����ׂ̏��u)
//------------------------------------------
bool KizuDeleteManager::Exec_Shrink()
{
	CString sql;					// SQL��

	//// ODBC�ڑ��N���X �C���X�^���X����
	OdbcBase clsDB;
	if( ! clsDB.Connect(TASKINI_NAME, my_cOdbcSession)) {
		// �f�[�^�x�[�X�ڑ��G���[
		LOG(em_ERR), "[KD] [%s] DB�ڑ��G���[", my_cSession);
		syslog(SYSNO_DB_CONNECT_ERR, "");
		return false;
	}

	//// SQL���쐬
	if( ! clsDB.ExecuteDirect_ShrinkFile() ) {
		LOG(em_ERR), "[KD] [%s] �g�����U�N�V�������k�������s", my_cSession);
		syslog(SYSNO_DB_EXECUTEDIRECT_ERR, "[�g�����U�N�V�������k����]");
		return false;
	}


	//// �t�F�b�`
	int now;							// ���ݒl
	int min;							// �ŏ��l
	int sqlrc = clsDB.Fetch();
	if (sqlrc == KIZUODBC_FETCH_OK) {						// �f�[�^�L��
		clsDB.GetData(3, &now);
		clsDB.GetData(4, &min);
		// 8K�P�ʂȂ̂ŁAM�P�ʂɕϊ�
		now = now * 8 /1024;
		min = min * 8 /1024;
	} else if( sqlrc == KIZUODBC_FETCH_NODATE ) {			// �f�[�^����
		//���肦�Ȃ�
	} else {												// �G���[
		LOG(em_ERR), "[KD] [%s] �g�����U�N�V�������k�������s(�t�F�b�`�G���[)", my_cSession);
		syslog(SYSNO_DB_FETCH_ERR, "[�g�����U�N�V�������k����]");
		return false;
	}
//	clsDB.TranCommit(); 20081208

	//// �g�����U�N�V�������O�T�C�Y�Z�b�g
	my_nTranSizeMin = min;
	my_nTranSizeNow = now;
	
	if(now != min) {
		LOG(em_MSG), "[KD] [%s] �X���b�h<%d> �g�����U�N�V�������k���� [%d]/[%d]", my_cSession, my_nID, now, min);
	}
	return true;
}

//------------------------------------------
// Log�폜����
// int nDelLogDay ���O�̕ۑ�����
//------------------------------------------
bool KizuDeleteManager::Delete_Log(int nDelLogDay)
{
	// ���O�폜
	return Delete_Day(DB_LOG_INF, "�o�^���t", nDelLogDay);
}


//------------------------------------------
// �w������ȑO�f�[�^���ꊇ�폜
// char const *tbl �폜�e�[�u��
// char const *keyDayName �L�[�� (���t�^�F�ʏ� �o�^���t)
// int day �폜���� (1���ƍ������1���O�̃f�[�^���폜)
//------------------------------------------
bool KizuDeleteManager::Delete_Day(char const *tbl, char const *keyDayName, int day)
{
	CString sql;					// SQL��

	//// ODBC�ڑ��N���X �C���X�^���X����
	OdbcBase clsDB;
	if( ! clsDB.Connect(TASKINI_NAME, my_cOdbcSession)) {
		// �f�[�^�x�[�X�ڑ��G���[
		LOG(em_ERR), "[KD] [%s] DB�ڑ��G���[", my_cSession);
		syslog(SYSNO_DB_CONNECT_ERR, "");
		return false;
	}

	//// SQL���쐬(���O�̈����Ԓ��ߕ����폜���鎞���Ɏg�p����)
	SYSTEMTIME STime;
	GetLocalTime(&STime);
	sql.Format("DELETE FROM %s WHERE %s + %d < '%d/%d/%d'", tbl, keyDayName, day, STime.wYear, STime.wMonth, STime.wDay);
	if( !clsDB.ExecuteDirect(sql) ) {
		LOG(em_ERR), "[KD] [%s] %s�폜���s", my_cSession, tbl);
		syslog(SYSNO_DB_EXECUTEDIRECT_ERR, "[%s]", tbl);
		return false;
	}
//	clsDB.TranCommit(); 20081208
	LOG(em_MSG), "[KD] [%s] �X���b�h<%d> %s�폜����", my_cSession, my_nID, tbl );
	return true;
}

//------------------------------------------
// �w������ȑO�f�[�^���P���P�ʂō폜
// char const *tbl �폜�e�[�u��
// char const *keyDayName �L�[�� (���t�^�F�ʏ� �o�^���t)
// int day �폜���� (1���ƍ������1���O�̃f�[�^���폜)
//------------------------------------------
bool KizuDeleteManager::Delete_Day2(char const *tbl, char const *keyDayName, int day)
{
	CString sql;					// SQL��
	char selectKeyVal[128];		// ���݂̃L�[
	int nExecCnt = 0;			// ���s��

	//// ODBC�ڑ��N���X �C���X�^���X����
	OdbcBase clsDB;
	if( ! clsDB.Connect(TASKINI_NAME, my_cOdbcSession)) {
		// �f�[�^�x�[�X�ڑ��G���[
		LOG(em_ERR), "[KD] [%s] DB�ڑ��G���[", my_cSession);
		syslog(SYSNO_DB_CONNECT_ERR, "");
		return false;
	}

	//// SQL���쐬(���t�̂P���P�ʂł̍폜)�B������������ �폜�������Â����̂��폜
	SYSTEMTIME STime;
	GetLocalTime(&STime);
	while(true) {
		sql.Format("SELECT TOP 1 CONVERT(varchar, %s,111) FROM %s WHERE %s + %d < '%d/%d/%d' ORDER BY %s ASC", 
			keyDayName, tbl, keyDayName, day, STime.wYear, STime.wMonth, STime.wDay, keyDayName);
		if( !clsDB.ExecuteDirect(sql) ) {
			LOG(em_ERR), "[KD] [%s] %s�폜���s", my_cSession, tbl);
			syslog(SYSNO_DB_EXECUTEDIRECT_ERR, "[%s]", tbl);
			return false;
		}
		int sqlrc = clsDB.Fetch();
		if (sqlrc == KIZUODBC_FETCH_OK) {						// �f�[�^�L��
			clsDB.GetData(1, selectKeyVal, sizeof(selectKeyVal));
			if( 1 > strlen(selectKeyVal) ) break;				// �f�[�^���� 
		} else if( sqlrc == KIZUODBC_FETCH_NODATE ) {			// �f�[�^����
			break;	// ���肦�Ȃ�
		} else {								// �G���[
			syslog(SYSNO_DB_FETCH_ERR, "[�Â����t�폜����]");
			return false;
		}
	
		// �폜
		LOG(em_MSG), "[KD] [%s] �X���b�h<%d> ���э폜 [%s] [%s] [%s�܂�]", my_cSession, my_nID, tbl, keyDayName, selectKeyVal);
		sql.Format("DELETE FROM %s WHERE %s<='%s 23:59:59'", tbl, keyDayName, selectKeyVal);
		if( !clsDB.ExecuteDirect(sql) ) {
			LOG(em_ERR), "[KD] [%s] %s���t���ʍ폜���s", my_cSession, tbl);
			syslog(SYSNO_DB_EXECUTEDIRECT_ERR, "[DB���t���ʍ폜����]");
			return false;
		}

		//// �ꎞ��~
		Sleep(*my_pnWaitTime);
		if(my_bStopFlg) return true; // �I���H

		nExecCnt ++;
		if(10 <= nExecCnt) return true;	// 10����������Ƃ��đł��؂�B�܂�����ɍ폜����B�����Y��ő�ʂɍ폜����ꍇ�̑΍�
	}

	LOG(em_MSG), "[KD] [%s] �X���b�h<%d> %s�폜����", my_cSession, my_nID, tbl );
	return true;
}

//------------------------------------------
// �w�茏���ȊO�̃f�[�^�폜
// char const *tbl �폜�e�[�u��
// char const *keyName ��L�[���� (���ڂ̌^�́A������^)
// char const *subName �T�u��L�[���� (���ڂ̌^�́A���ł�OK)
// int delCnt ��x�ɍ폜���鐔
// int nMaxNum �ۑ������
//------------------------------------------
bool KizuDeleteManager::Delete_Cnt(char const *tbl, char const *keyName, char const *subName, int delCnt, int nMaxNum)
{
	CString sql;						// SQL��
	char cKey[256];		// �L�[
	memset( cKey, 0x00, sizeof(cKey));

	//// ODBC�ڑ��N���X �C���X�^���X����
	OdbcBase clsDB;
	if( ! clsDB.Connect(TASKINI_NAME, my_cOdbcSession)) {
		// �f�[�^�x�[�X�ڑ��G���[
		LOG(em_ERR), "[KD] [%s] DB�ڑ��G���[", my_cSession);
		syslog(SYSNO_DB_CONNECT_ERR, "");
		return false;
	}

	sql.Format("SELECT COUNT(*), Min(a.%s) FROM (SELECT Top %d %s FROM %s GROUP BY %s ORDER BY %s DESC) a",
		keyName, (nMaxNum+1), keyName, tbl, keyName, keyName, keyName);
	if( !clsDB.ExecuteDirect(sql)) {
		syslog(SYSNO_DB_EXECUTEDIRECT_ERR, "[KD] [%s] [�w�茏���ێ��J�E���g���s %s]", my_cSession, tbl);
		return false;
	}

	int sqlrc = clsDB.Fetch();
	int cnt = 0;
	if (sqlrc == KIZUODBC_FETCH_OK) {				// �f�[�^�L��
		clsDB.GetData(1, &cnt);
		clsDB.GetData(2, cKey, sizeof(cKey));
	} else if( sqlrc == KIZUODBC_FETCH_NODATE ) {	// �f�[�^����
	} else {										// �G���[
		syslog(SYSNO_DB_FETCH_ERR, "[KD] [%s] [�폜�ΏۃR�C���Ǎ��ݎ��s %s]", my_cSession, tbl);
		return false;
	}

	// �w�茏�����ȏ�H
	if ( cnt >= (nMaxNum+1)) {
		// �w��L�[���Â����т����ʍ폜����
		if( ! Delete_OldInf(tbl, keyName, subName, delCnt, cKey) ) {
			LOG(em_ERR), "[KD] [%s] %s�폜���s (%s:%s)", my_cSession, tbl, keyName, cKey);
			return false;	// ���s
		} else {
			LOG(em_MSG), "[KD] [%s] �w��L�[���Â����т����ʍ폜����[%s] (%s:%s)", my_cSession, tbl, keyName, cKey);
		}
	}

	LOG(em_MSG), "[KD] [%s] �X���b�h<%d> %�폜���� (%s:%s)", my_cSession, my_nID, tbl, keyName, cKey );
	return true;
}

// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// INF

//------------------------------------------
// �w���L�[�� �ꊇ�폜/���ʍ폜 �G���g���[
// DEL_TBL const *tbl �폜�e�[�u����`
// char *keyVal ��L�[�̒l (�ʏ� �r���Ǘ�No) 
//------------------------------------------
bool KizuDeleteManager::Delete_Inf(DEL_TBL const *dtbl, char const *keyVal)		
{
	if( 0 == dtbl->nDelRow ) {			// �ꊇ�폜�J�n
		return Delete_Inf(dtbl->cDelTable, dtbl->cKeyName, keyVal);
	} else {							// ���ʍ폜�J�n
		return Delete_Inf(dtbl->cDelTable, dtbl->cKeyName, dtbl->cSubName, dtbl->nDelRow, keyVal);
	}
}

//------------------------------------------
// �w���L�[�� �ꊇ�폜 (1�R�C����)
// char const *tbl �폜�e�[�u��
// char *keyName ��L�[���� (���ڂ̌^�́A������^)
// char *keyVal ��L�[�̒l (�ʏ� �r���Ǘ�No) 
//------------------------------------------
bool KizuDeleteManager::Delete_Inf(char const *tbl, char const *keyName, char const *keyVal)
{
	CString sql;

	//// ODBC�ڑ��N���X �C���X�^���X����
	OdbcBase clsDB;
	if( ! clsDB.Connect(TASKINI_NAME, my_cOdbcSession)) {
		// �f�[�^�x�[�X�ڑ��G���[
		LOG(em_ERR), "[KD] [%s] DB�ڑ��G���[", my_cSession);
		syslog(SYSNO_DB_CONNECT_ERR, "");
		return false;
	}

	////// ���э폜
	LOG(em_MSG), "[KD] [%s] �X���b�h<%d> ���э폜 [%s] [%s]", my_cSession, my_nID, tbl, keyVal);
	sql.Format("DELETE FROM %s WHERE %s='%s'", tbl, keyName, keyVal);
	if( !clsDB.ExecuteDirect(sql) ) {
		LOG(em_ERR), "[KD] [%s] �X���b�h<%d> ���э폜�ُ� [%s] [%s]", my_cSession, my_nID, tbl, sql);
		syslog(SYSNO_DB_EXECUTEDIRECT_ERR, "[DB���э폜����]");
		return false;
	}
//	clsDB.TranCommit(); 20081208
	return true;
}

//------------------------------------------
// �w���L�[�� ���ʍ폜
// char const *tbl �폜�e�[�u��
// char const *keyName ��L�[���� (���ڂ̌^�́A������^)
// char const *subName �T�u��L�[���� (���ڂ̌^�́A���ł�OK)
// int delCnt �P��̍폜����
// char const *keyVal ��L�[�̒l (�ʏ� �r���Ǘ�No) 
//------------------------------------------
bool KizuDeleteManager::Delete_Inf(char const *tbl, char const *keyName, char const *subName, int delCnt, char const *keyVal)
{
	CString sql;
	int nowCnt;				// ���݂̍폜�ʒu

	//// ODBC�ڑ��N���X �C���X�^���X����
	OdbcBase clsDB;
	if( ! clsDB.Connect(TASKINI_NAME, my_cOdbcSession)) {
		// �f�[�^�x�[�X�ڑ��G���[
		LOG(em_ERR), "[KD] [%s] DB�ڑ��G���[", my_cSession);
		syslog(SYSNO_DB_CONNECT_ERR, "");
		return false;
	}

	////// ���э폜 (������ƂÂ폜)
	while(true) {
		// �폜�ʒu���� (�K�� TOP ���g�p����ꍇ�́AOrder������鎖�B���Ԃ��s��ƂȂ�)
		sql.Format("SELECT ISNULL( Max(a.%s), -1) FROM (SELECT TOP %d %s FROM %s WHERE %s = '%s' ORDER BY %s ASC) a",
			subName, delCnt, subName, tbl, keyName, keyVal, subName);
		if( !clsDB.ExecuteDirect(sql)) {
			LOG(em_ERR), "[KD] [%s] �X���b�h<%d> DB���я��ʍ폜���� [%s]", my_cSession, my_nID, sql);
			syslog(SYSNO_DB_EXECUTEDIRECT_ERR, "[DB���я��ʍ폜����]");
			return false;
		}
		int sqlrc = clsDB.Fetch();
		if (sqlrc == KIZUODBC_FETCH_OK) {						// �f�[�^�L��
			clsDB.GetData(1, &nowCnt);
			if( -1 == nowCnt ) break;					// �f�[�^���� 
		} else if( sqlrc == KIZUODBC_FETCH_NODATE ) {			// �f�[�^����
			break;	// ���肦�Ȃ�	
		} else {								// �G���[
			syslog(SYSNO_DB_FETCH_ERR, "[DB���я��ʍ폜����]");
			return false;
		}
		
		// �폜
		LOG(em_MSG), "[KD] [%s] �X���b�h<%d> ���э폜 [%s] [%s] [%d�܂�]", my_cSession, my_nID, tbl, keyVal, nowCnt);
		sql.Format("DELETE FROM %s WHERE %s='%s' AND %s<=%d", tbl, keyName, keyVal, subName, nowCnt);
		if( !clsDB.ExecuteDirect(sql) ) {
			LOG(em_ERR), "[KD] [%s] �X���b�h<%d> ���э폜�ُ� [%s] [%s]", my_cSession, my_nID, tbl, sql);
			syslog(SYSNO_DB_EXECUTEDIRECT_ERR, "[DB���я��ʍ폜����]");
			return false;
		}
		//clsDB.TranCommit(); 20081208


		//// �ꎞ��~
		Sleep(*my_pnWaitTime);
		if(my_bStopFlg) return true; // �I���H
	}
	return true;
}

// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// OLD

//------------------------------------------
// �ꊇ�폜/���ʍ폜 �G���g���[ (�w��L�[���Â����т��폜����)
// DEL_TBL const *tbl �폜�e�[�u����`
// char *keyVal ��L�[�̒l (�ʏ� �r���Ǘ�No) 
//------------------------------------------
bool KizuDeleteManager::Delete_OldInf(DEL_TBL const *dtbl, char const *keyVal)		
{
	if( 0 == dtbl->nDelRow ) {			// �ꊇ�폜�J�n
		return Delete_OldInf(dtbl->cDelTable, dtbl->cKeyName, NULL, 0, keyVal);
	} else {							// ���ʍ폜�J�n
		return Delete_OldInf(dtbl->cDelTable, dtbl->cKeyName, dtbl->cSubName, dtbl->nDelRow, keyVal);
	}
}

//------------------------------------------
// �w��L�[���Â����т��ꊇ�폜
// char const *tbl �폜�e�[�u��
// char const *keyName ��L�[���� (���ڂ̌^�́A������^)
// char const *keyVal ��L�[�̒l (�ʏ� �r���Ǘ�No) 
// int delCnt �P��̍폜����
//------------------------------------------
bool KizuDeleteManager::Delete_OldInf(char const *tbl, char const *keyName, char const *keyVal)
{
	return Delete_OldInf(tbl, keyName, NULL, 0, keyVal);
}

//------------------------------------------
// �w��L�[���Â����т����ʍ폜
//	�� �T�u��L�[ or 1��̍폜������������΁A��L�[�P�ʂŌÂ����т��폜���Ă���
// char const *tbl �폜�e�[�u��
// char const *keyName ��L�[���� (���ڂ̌^�́A������^)
// char const *subName �T�u��L�[���� (���ڂ̌^�́A���ł�OK)
// int delCnt �P��̍폜����
// char const *keyVal ��L�[�̒l (�ʏ� �r���Ǘ�No) 
//------------------------------------------
bool KizuDeleteManager::Delete_OldInf(char const *tbl, char const *keyName, char const *subName, int delCnt, char const *keyVal)
{
	CString sql;
	char selectKeyVal[128];		// ���݂̃L�[
	int nExecCnt = 0;			// ���s��

	//// ODBC�ڑ��N���X �C���X�^���X����
	OdbcBase clsDB;
	if( ! clsDB.Connect(TASKINI_NAME, my_cOdbcSession)) {
		// �f�[�^�x�[�X�ڑ��G���[
		LOG(em_ERR), "[KD] [%s] DB�ڑ��G���[", my_cSession);
		syslog(SYSNO_DB_CONNECT_ERR, "");
		return false;
	}

	////// ���э폜 (���ʍ폜)
	while(true) {
		//// �폜�ΏۃR�C������
		sql.Format("SELECT ISNULL( Min(%s), '') FROM %s WHERE %s<'%s'", keyName, tbl, keyName, keyVal);
		if( !clsDB.ExecuteDirect(sql)) {
			LOG(em_ERR), "[KD] [%s] �X���b�h<%d> �Â����э폜���� [%s]", my_cSession, my_nID, sql);
			syslog(SYSNO_DB_EXECUTEDIRECT_ERR, "[�Â����э폜����]");
			return false;
		}
		int sqlrc = clsDB.Fetch();
		if (sqlrc == KIZUODBC_FETCH_OK) {						// �f�[�^�L��
			clsDB.GetData(1, selectKeyVal, sizeof(selectKeyVal));
			if( 1 > strlen(selectKeyVal) ) break;				// �f�[�^���� 
		} else if( sqlrc == KIZUODBC_FETCH_NODATE ) {			// �f�[�^����
			break;	// ���肦�Ȃ�
		} else {								// �G���[
			syslog(SYSNO_DB_FETCH_ERR, "[�Â����э폜����]");
			return false;
		}

		//// �Ώ��r���Ǘ�No�̎��т��폜
		if(0 == delCnt || 1>strlen(subName) ) {
			// �w���L�[�݈̂ꊇ�폜
			if( ! Delete_Inf(tbl, keyName, selectKeyVal ) ) {
				return false;
			}
		} else {
			// ���ʍ폜
			if( ! Delete_Inf(tbl, keyName, subName, delCnt, selectKeyVal ) ) {
				return false;
			}
		}


		//// �ꎞ��~
		Sleep(*my_pnWaitTime);
		if(my_bStopFlg) return true;	// �I���H
		nExecCnt ++;
		if(10 <= nExecCnt) return true;	// 10��ʂ�����Ƃ��đł��؂�B�܂�����ɍ폜����B�����Y��ő�ʂɍ폜����ꍇ�̑΍�
	}
	return true;
}
