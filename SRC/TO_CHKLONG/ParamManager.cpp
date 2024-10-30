#include "StdAfx.h"
#include "ParamManager.h"

// �������[���[�N���o ----->>>
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif
// <<<------------------------

#include "MainInctance.h"

//------------------------------------------
// �R���X�g���N�^
//------------------------------------------
ParamManager::ParamManager(void) :
ThreadManager("ParamMgr")
{
}

//------------------------------------------
// �f�X�g���N�^
//------------------------------------------
ParamManager::~ParamManager(void)
{
}


//------------------------------------------
// �X���b�h�J�n�O���� (�X���b�h���s����(�V�O�i���Z�b�g�O)�ɃR�[���o�b�N)
//------------------------------------------
int ParamManager::ThreadFirst()
{

	//// �C�x���g�ݒ�
	HANDLE hArray[] = { gque_Deli.g_evSem
						};
	SetEventNum(sizeof(hArray)/sizeof(HANDLE), hArray);
	return 0;
}

//------------------------------------------
// �X���b�h�I���O���� (�I���V�O�i����ɃR�[���o�b�N)
//------------------------------------------
int ParamManager::ThreadLast()
{
	return 0;
}

//------------------------------------------
// �X���b�h�C�x���g����
// int nEventNo �C�x���gNo (0�I���W��)
//------------------------------------------
void ParamManager::ThreadEvent(int nEventNo)
{
	////// �V�O�i���̏���
	enum {	EV_QUE = 0,
	};

	////// �V�O�i����������
	switch (nEventNo) {

//-----------------------------------------------------------------------------------------------
	case EV_QUE: 
		return;

//-----------------------------------------------------------------------------------------------
	default :
		// ���肦�Ȃ��I�I
		return;
	}
}

// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// �p�����[�^�Ǎ�

//------------------------------------------
// �S�p�����[�^�ǂݍ���
//------------------------------------------
bool ParamManager::ReadParam_All()
{
	if( ! Read_CommonMast() )	return false;

	return true;
}
//------------------------------------------
// �Ӄ֒萔�p�����[�^�Ǎ�
//------------------------------------------
bool ParamManager::Read_CommonMast()
{
	CString sql;					// SQL��

	//// �p�����[�^������
	CString cDbName;
	cDbName = DB_COMMON_MASTER;
	P_COMMON_MAST&	p = m_CommonMast;
	P_COMMON_MAST	buf;
	memset(&buf, 0x00, sizeof(buf));


	//// ODBC�ڑ��N���X �C���X�^���X����
	OdbcBase clsDB;
	if( ! clsDB.Connect(TASKINI_NAME)) {
		// �f�[�^�x�[�X�ڑ��G���[
		LOG(em_ERR), "[%s] DB�ڑ��G���[[%s]", my_sThreadName, clsDB.GetLastErrMsg());
		syslog(SYSNO_DB_CONNECT_ERR, "");
		return false;
	}

	//// SQL���쐬
	sql  = "SELECT ";
	sql += " LongPd_GuidanceAvailable ";					// [���������׃K�C�_���X�L��]
	sql += ",LongPd_GuidanceCheckInterval ";				// [���������׃K�C�_���X�m�F���{�Ԋu]
	sql += ",LongPd_NearFurnaceRangeFrom ";					// [���������טF�`�J����͈�From]
	sql += ",LongPd_NearFurnaceRangeTo ";					// [���������טF�`�J����͈�To]
	sql += "FROM ";
	sql += cDbName;

	//// ���s
	if( !clsDB.ExecuteDirect(sql)) {
		syslog(SYSNO_DB_EXECUTEDIRECT_ERR, "[%s][%s]", cDbName, clsDB.GetLastErrMsg());
		return false;
	}

	//// �擾
	int nRecord = 0;
	while (TRUE) {
		int sqlrc = clsDB.Fetch();
		if (sqlrc == KIZUODBC_FETCH_OK) {					// �f�[�^�L��
	
			// �f�[�^��
			clsDB.GetData(1, &buf.nLongPd_GuidanceAvailable);		// [���������׃K�C�_���X�L��]
			clsDB.GetData(2, &buf.nLongPd_GuidanceCheckInterval);	// [���������׃K�C�_���X�m�F���{�Ԋu]
			clsDB.GetData(3, &buf.nLongPd_NearFurnaceRangeFrom);	// [���������טF�`�J����͈�From]
			clsDB.GetData(4, &buf.nLongPd_NearFurnaceRangeTo);		// [���������טF�`�J����͈�To]

			nRecord++;

		} else if( sqlrc == KIZUODBC_FETCH_NODATE ) {		// �f�[�^����
			break;	
		} else {								// �G���[
			syslog(SYSNO_DB_FETCH_ERR, "[%s][%s]", cDbName, clsDB.GetLastErrMsg());
			return false;
		}
	}

	//// ����Ă������R�[�h���͐���H�H
	if( 1 != nRecord ){
		LOG(em_ERR), "[%s] %s ���R�[�h���ُ�[%d]", my_sThreadName, cDbName, nRecord);
		return false;
	}

	//// �擾�f�[�^�������o�[�ϐ��ɃZ�b�g
	memcpy( &p, &buf, sizeof(buf));
	LOG(em_MSG), "[%s] %s �擾����", my_sThreadName, cDbName);
	return true;
}
