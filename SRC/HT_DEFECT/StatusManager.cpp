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
m_emMainState(STATE_NON),
m_emUnten(DIV_UNTEN_STOP),
ThreadManager("StatusMgr")
{
	// ������
	InitializeCriticalSection(&m_csLock);						// �N���e�B�J���Z�N�V�����I�u�W�F�N�g��������
	// ������돈����
	memset(&m_CamPairInf, 0x00, sizeof(m_CamPairInf) );

	m_bGetEngStatNew = false;
	memset(&m_EngStat, 0x00, sizeof(m_EngStat));

}

//------------------------------------------
// �f�X�g���N�^
//------------------------------------------
StatusManager::~StatusManager(void)
{
	//// �J��
	DeleteCriticalSection(&m_csLock);							// �N���e�B�J���Z�N�V�����I�u�W�F�N�g���J��
}

// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// �X�e�[�^�X����

//------------------------------------------
// �w��̃X�e�[�^�X���̎擾
// MAIN_STATE st �X�e�[�^�X
//------------------------------------------
CString StatusManager::GetMainStateName(MAIN_STATE st) const
{
	CString wk;
	if		(st == STATE_NON)		{ wk = "��������"; }
	else if (st == STATE_INIT)		{ wk = "������� (�����J�����)"; }
	else if (st == STATE_WAIT)		{ wk = "�������� (������L���)"; }
	else if (st == STATE_RUN)		{ wk = "������";}
	else if (st == STATE_CANNOT)	{ wk = "���s�s�\";}
	else							{ wk.Format("�X�e�[�^�X�ُ� <%d>", st); }
	return wk;
}


//------------------------------------------
// �X�e�[�^�X�ύX
// MAIN_STATE st �X�e�[�^�X
//------------------------------------------
bool StatusManager::SetMainState(MAIN_STATE st)
{
	////// ���C���C���X�^���X���o��
	MAIN_STATE	old;
	old = GetMainState();
	if( old == st ) {
		LOG(em_ERR), "�X�e�[�^�X�ύXNG [%s] �� [%s]", GetMainStateName(old), GetMainStateName(st)); 
		return false;
	}

	//// �N���e�B�J���Z�N�V�����˓�
	AutoLock lock(&m_csLock);

	//// �ύX�X�e�[�^�X�ɂ�菈������
	m_emMainState = st;
	syslog(102, "[%s] �� [%s]", GetMainStateName(old), GetMainStateName(st)); 

	//// ��ʏ�ԕύX
	LOG(em_MSG), "============= <<�X�e�[�^�X�ύX>> [%s] �� [%s] =============", GetMainStateName(old), GetMainStateName(st)); 
	return true;

}


//------------------------------------------
// �^�]���[�h�ύX
// M_DIV_UNTEN st �^�]���[�h
//------------------------------------------
bool StatusManager::SetUnten(EM_DIV_UNTEN st)
{
	////// ���C���C���X�^���X���o��
	EM_DIV_UNTEN	old;
	old = GetUnten();
	if( old == st ) {
		//LOG(em_ERR), "�^�]��ԕύXNG [%s] �� [%s]", GetUntenName(old), GetUntenName(st)); 
		return false;
	}

	//// �N���e�B�J���Z�N�V�����˓�
	AutoLock lock(&m_csLock);

	//// �ύX�X�e�[�^�X�ɂ�菈������
	m_emUnten = st;
	syslog(103, "[%s] �� [%s]", GetUntenName(old), GetUntenName(st)); 
	LOG(em_MSG), "<<�^�]��ԕύX>> [%s] �� [%s]", GetUntenName(old), GetUntenName(st)); 
	return true;

}





// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// �J�����y�A�֌W

//------------------------------------------
// �p�����[�^�擾
//------------------------------------------
bool StatusManager::GetParam_CamPairInf()
{
	CString sql;					// SQL��
	int camset;
	int campair;
	int campair_cy;

	//// �̈揉����
	memset(&m_CamPairInf, 0x00, sizeof(m_CamPairInf) );

	//// ODBC�ڑ��N���X �C���X�^���X����
	OdbcBase clsDB;
	if( ! clsDB.Connect(TASKINI_NAME)) {
		// �f�[�^�x�[�X�ڑ��G���[
		LOG(em_ERR), "[%s] DB�ڑ��G���[", my_sThreadName);
		syslog(SYSNO_DB_CONNECT_ERR, "");
		return false;
	}


	////// �J�����Z�b�g��
	//// SQL���쐬
	sql =  "SELECT";
	sql += " camset, CamPair, ISNULL(CamPairCycle,1)";
	sql += " FROM ";
	sql += DB_PARAM_CAMSET;
	sql += " ORDER BY camset ASC";
	//// ���s
	if( !clsDB.ExecuteDirect(sql)) {
		syslog(SYSNO_DB_EXECUTEDIRECT_ERR, "[%s]", DB_PARAM_CAMSET);
		return false;
	}
	//// �擾
	while (TRUE) {
		int sqlrc = clsDB.Fetch();
		if (sqlrc == KIZUODBC_FETCH_OK) {					// �f�[�^�L��
			// ��L�[
			clsDB.GetData(1, &camset);
			clsDB.GetData(2, &campair);
			clsDB.GetData(3, &campair_cy);

			if( camset > MAX_CAMSET || camset <= 0 || campair > NUM_CAMPAIR || campair <= 0 ||
				campair_cy > NUM_CAMPAIR_CYCLE || campair_cy <= 0 ) {

				// ��L�[�s��
				LOG(em_ERR), "[%s] [%s ��L�[�s�� camset=%d campair=%d campair_cy=%d]", my_sThreadName, DB_PARAM_CAMSET, camset, campair, campair_cy);
				syslog(SYSNO_DB_DATA_ERR, "[%s camset=%d campair=%d campair_cy=%d]", DB_PARAM_CAMSET, camset, campair, campair_cy);	
				continue;
			}

			// �f�[�^�Z�b�g
			m_CamPairInf.item[campair-1].camset[ m_CamPairInf.item[campair-1].num ] = camset;
			m_CamPairInf.item[campair-1].num ++;
			
			m_CamPairInf.item_cycle[campair_cy-1].camset[ m_CamPairInf.item_cycle[campair_cy-1].num ] = camset;
			m_CamPairInf.item_cycle[campair_cy-1].num ++;

		} else if( sqlrc == KIZUODBC_FETCH_NODATE ) {		// �f�[�^����
			break;
		} else {								// �G���[
			syslog(SYSNO_DB_FETCH_ERR, "[%s]", DB_PARAM_CAMSET);
			return false;
		}
	}

	LOG(em_MSG), "[%s] �p�����[�^�擾����", my_sThreadName);
	return true;
}
