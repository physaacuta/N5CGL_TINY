
#include "StdAfx.h"
#include "ParamManager.h"


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
ParamManager::ParamManager(void) :
FuncBase("ParamMgr")
{
}
//------------------------------------------
// �f�X�g���N�^
//------------------------------------------
ParamManager::~ParamManager(void)
{
}

// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// �p�����[�^�Ǎ�

//------------------------------------------
// �S�p�����[�^�ǂݍ���
//------------------------------------------
bool ParamManager::ReadParam_All()
{
	bool retc = true;
	if( ! Read_CommonMast() )	retc = false;
	if( ! Read_ScodeOutput() )	retc = false;
	if( ! Read_PcodeOutput() )	retc = false;
	return retc;
}

//------------------------------------------
// �r���萔�p�����[�^�Ǎ�
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
		LOG(em_ERR), "[%s] DB�ڑ��G���[", my_sThreadName);
		syslog(SYSNO_DB_CONNECT_ERR, "");
		return false;
	}

	//// SQL���쐬
	sql =  "SELECT";
	sql += " [�E�H�[�}�ގw��ʒu] ";
	sql += " FROM ";
	sql += cDbName;

	//// ���s
	if( !clsDB.ExecuteDirect(sql)) {
		LOG(em_ERR), "[%s] DB_EXECUTEDIRECT [%s]", my_sThreadName, cDbName);
		LOG(em_NON), "%s", sql);
		syslog(SYSNO_DB_EXECUTEDIRECT_ERR, "[%s]", cDbName);
		return false;
	}

	//// �擾
	while (TRUE) {
		int sqlrc = clsDB.Fetch();
		if (sqlrc == KIZUODBC_FETCH_OK) {					// �f�[�^�L��
	
			// �f�[�^��
			clsDB.GetData(1, &buf.nWalm );


		} else if( sqlrc == KIZUODBC_FETCH_NODATE ) {		// �f�[�^����
			break;	
		} else {								// �G���[
			LOG(em_ERR), "[%s] SYSNO_DB_FETCH_ERR [%s]", my_sThreadName, cDbName);
			LOG(em_NON), "%s", sql);
			syslog(SYSNO_DB_FETCH_ERR, "[%s]", cDbName);
			return false;
		}
	}

	//// �擾�f�[�^�������o�[�ϐ��ɃZ�b�g
	memcpy( &p, &buf, sizeof(buf));
	LOG(em_MSG), "[%s] %s �擾����", my_sThreadName, cDbName);
	return true;
}

//------------------------------------------
// �����\�ʏ�ԃp�����[�^�Ǎ�
//------------------------------------------
bool ParamManager::Read_ScodeOutput()
{
	CString sql;					// SQL��
	int id;							// scode,pcode��ID (0�I���W��)
	int no;
		
	//// �p�����[�^������
	CString cDbName;
	P_SCODE_OUTPUT&	buf = (P_SCODE_OUTPUT&)m_ScodeOutput;
	memset(&buf, 0x00, sizeof(P_SCODE_OUTPUT));


	//// ODBC�ڑ��N���X �C���X�^���X����
	OdbcBase clsDB;
	if( ! clsDB.Connect(TASKINI_NAME)) {
		// �f�[�^�x�[�X�ڑ��G���[
		LOG(em_ERR), "[%s] DB�ڑ��G���[", my_sThreadName);
		syslog(SYSNO_DB_CONNECT_ERR, "");
		return false;
	}

	////////////////////////////////////
	// �}�X�^�[
	////////////////////////////////////
	cDbName = DB_SCODE_MASTER;
	//// SQL���쐬
	sql =  "SELECT";
	sql += " scode, �L������";
	sql += " FROM ";
	sql += cDbName;
	sql += " ORDER BY �D��x ASC ";

	//// ���s
	if( !clsDB.ExecuteDirect(sql)) {
		LOG(em_ERR), "[%s] DB_EXECUTEDIRECT [%s]",my_sThreadName,cDbName);
		LOG(em_NON), "%s", sql);
		syslog(SYSNO_DB_EXECUTEDIRECT_ERR, "[%s]", cDbName);
		return false;
	}

	//// �擾
	while (TRUE) {
		int sqlrc = clsDB.Fetch();
		if (sqlrc == KIZUODBC_FETCH_OK) {					// �f�[�^�L��

			// ��L�[
			clsDB.GetData(1, &id);
			if( MAX_SCODE < id || 0 >= id ) {
				// ��L�[�s��
				LOG(em_ERR), "[%s] [%s ��L�[�s�� id=%d]", my_sThreadName, cDbName, id);
				syslog(SYSNO_DB_DATA_ERR, "[%s id=%d]", cDbName, id);					
				continue;
			}
			id -= 1;										// 0�I���W���ɕύX����


			// �f�[�^��
			clsDB.GetData(2, &buf.rec[id].bUmu );


			//// �D��x���Ɋi�[
			buf.nPriority[ buf.nMasterNum ] = id;
			buf.nMasterNum++;

		} else if( sqlrc == KIZUODBC_FETCH_NODATE ) {		// �f�[�^����
			break;	
		} else {								// �G���[
			LOG(em_ERR), "[%s] DB FETCH_ERR [%s]", my_sThreadName, cDbName);
			LOG(em_NON), "%s", sql);
			syslog(SYSNO_DB_FETCH_ERR, "[%s]", cDbName);
			return false;
		}
	}

	////////////////////////////////////
	// �����ݒ�
	////////////////////////////////////
	cDbName = DB_SCODE_JUDGE;
	//// SQL���쐬
	sql =  "SELECT";
	sql += " scode, ����No, �L������";

	sql += " ,ISNULL(SPM�����L��, -1)";							// 4.SPM�����L��
	sql += " ,����No";											// 5.����No
	sql += " ,���YNo";											// 6.����No
	sql += " ,ISNULL(����������, 0), ISNULL(���������, 0)";// 7.����������[��m] 8.���������[��m]
	sql += " ,�����";										// 9.�����
	sql += " ,����";											// 10.����
	sql += " ,�p�r";											// 11.�p�r
	sql += " ,��ގ�";											// 12.��ގ�
	sql += " ,�\�ʏ���1����";									// 13.�\�ʏ���1����
	sql += " ,�i��";											// 14.�i��

	sql += " FROM ";
	sql += cDbName;
	sql += " WHERE �L������=1";		// �]���ʍ팸�΍�
	sql += " ORDER BY scode, ����No";

	//// ���s
	if( !clsDB.ExecuteDirect(sql)) {
		LOG(em_ERR), "[%s] DB_EXECUTEDIRECT [%s]",my_sThreadName,cDbName);
		LOG(em_NON), "%s", sql);
		syslog(SYSNO_DB_EXECUTEDIRECT_ERR, "[%s]", cDbName);
		return false;
	}

	//// �擾
	while (TRUE) {
		int sqlrc = clsDB.Fetch();
		if (sqlrc == KIZUODBC_FETCH_OK) {					// �f�[�^�L��

			// ��L�[
			clsDB.GetData(1, &id);
			clsDB.GetData(2, &no);
			if( (MAX_SCODE < id || 0 >= id) || (MAX_CODE_JUDGE < no || 0 >= no) ) {
				// ��L�[�s��
				LOG(em_ERR), "[%s] [%s ��L�[�s�� id=%d no=%d]", my_sThreadName, cDbName, id, no);
				syslog(SYSNO_DB_DATA_ERR, "[%s id=%d no=%d]", cDbName, id, no);					
				continue;
			}
			id -= 1;										// 0�I���W���ɕύX����
			no -= 1;										// 0�I���W���ɕύX����
			clsDB.GetData(3, &buf.rec[id].typJudge[no].bUmu );

			// �f�[�^��
			clsDB.GetData(4, &buf.rec[id].typJudge[no].nSpm);				// SPM�����L��
			clsDB.GetData(5, &buf.rec[id].typJudge[no].cMeiNo, 32);			// ����No
			clsDB.GetData(6, &buf.rec[id].typJudge[no].cCoilNo, 32);		// ���YNo
			clsDB.GetData(7, &buf.rec[id].typJudge[no].nAtuSei[0]);			// ����������
			clsDB.GetData(8, &buf.rec[id].typJudge[no].nAtuSei[1]);			// ���������
			clsDB.GetData(9, &buf.rec[id].typJudge[no].cKenKijun, 32);		// �����
			clsDB.GetData(10, &buf.rec[id].typJudge[no].cMuke, 32);			// ����
			clsDB.GetData(11, &buf.rec[id].typJudge[no].cYouto, 32);		// �p�r
			clsDB.GetData(12, &buf.rec[id].typJudge[no].cSyuZai, 32);		// ��ގ�
			clsDB.GetData(13, &buf.rec[id].typJudge[no].cOmoteSyori1, 32);	// �\�ʏ���1����
			clsDB.GetData(14, &buf.rec[id].typJudge[no].cHinCode, 32);		// �i��

		} else if( sqlrc == KIZUODBC_FETCH_NODATE ) {		// �f�[�^����
			break;	
		} else {								// �G���[
			LOG(em_ERR), "[%s] DB FETCH_ERR [%s]", my_sThreadName, cDbName);
			LOG(em_NON), "%s", sql);
			syslog(SYSNO_DB_FETCH_ERR, "[%s]", cDbName);
			return false;
		}
	}

	LOG(em_MSG), "[%s] %s �擾����", my_sThreadName, cDbName);

	return true;
}


//------------------------------------------
// �����p�^�[���ʃp�����[�^�Ǎ�
//------------------------------------------
bool ParamManager::Read_PcodeOutput()
{
	CString sql;					// SQL��
	int id;							// scode,pcode��ID (0�I���W��)
	int no;

	//// �p�����[�^������
	CString cDbName;
	P_PCODE_OUTPUT&	buf = (P_PCODE_OUTPUT&)m_PcodeOutput;
	memset(&buf, 0x00, sizeof(P_PCODE_OUTPUT));


	//// ODBC�ڑ��N���X �C���X�^���X����
	OdbcBase clsDB;
	if( ! clsDB.Connect(TASKINI_NAME)) {
		// �f�[�^�x�[�X�ڑ��G���[
		LOG(em_ERR), "[%s] DB�ڑ��G���[", my_sThreadName);
		syslog(SYSNO_DB_CONNECT_ERR, "");
		return false;
	}

	////////////////////////////////////
	// �}�X�^�[
	////////////////////////////////////
	cDbName = DB_PCODE_MASTER;
	//// SQL���쐬
	sql =  "SELECT";
	sql += " pcode, �L������";
	sql += " FROM ";
	sql += cDbName;
	sql += " ORDER BY �D��x ASC ";

	//// ���s
	if( !clsDB.ExecuteDirect(sql)) {
		LOG(em_ERR), "[%s] DB_EXECUTEDIRECT [%s]",my_sThreadName,cDbName);
		LOG(em_NON), "%s", sql);
		syslog(SYSNO_DB_EXECUTEDIRECT_ERR, "[%s]", cDbName);
		return false;
	}

	//// �擾
	while (TRUE) {
		int sqlrc = clsDB.Fetch();
		if (sqlrc == KIZUODBC_FETCH_OK) {					// �f�[�^�L��

			// ��L�[
			clsDB.GetData(1, &id);
			if( MAX_PCODE < id || 0 >= id ) {
				// ��L�[�s��
				LOG(em_ERR), "[%s] [%s ��L�[�s�� id=%d]", my_sThreadName, cDbName, id);
				syslog(SYSNO_DB_DATA_ERR, "[%s id=%d]", cDbName, id);					
				continue;
			}
			id -= 1;										// 0�I���W���ɕύX����


			// �f�[�^��
			clsDB.GetData(2, &buf.rec[id].bUmu );


			//// �D��x���Ɋi�[
			buf.nPriority[ buf.nMasterNum ] = id;
			buf.nMasterNum++;

		} else if( sqlrc == KIZUODBC_FETCH_NODATE ) {		// �f�[�^����
			break;	
		} else {								// �G���[
			LOG(em_ERR), "[%s] DB FETCH_ERR [%s]", my_sThreadName, cDbName);
			LOG(em_NON), "%s", sql);
			syslog(SYSNO_DB_FETCH_ERR, "[%s]", cDbName);
			return false;
		}
	}

	////////////////////////////////////
	// �����ݒ�
	////////////////////////////////////
	cDbName = DB_PCODE_JUDGE;
	//// SQL���쐬
	sql = "SELECT";
	sql += " pcode, ����No, �L������";

	sql += " ,����No";											// 4.����No
	sql += " ,���YNo";											// 5.����No
	sql += " ,ISNULL(����������, 0), ISNULL(���������, 0)";// 6.����������[��m] 7.���������[��m]
	sql += " ,�����";										// 8.�����
	sql += " ,����";											// 9.����
	sql += " ,�p�r";											// 10.�p�r
	sql += " ,��ގ�";											// 11.��ގ�
	sql += " ,�\�ʏ���1����";									// 12.�\�ʏ���1����
	sql += " ,�i��";											// 13.�i��

	sql += " FROM ";
	sql += cDbName;
	sql += " WHERE �L������=1";		// �]���ʍ팸�΍�
	sql += " ORDER BY pcode, ����No";

	//// ���s
	if( !clsDB.ExecuteDirect(sql)) {
		LOG(em_ERR), "[%s] DB_EXECUTEDIRECT [%s]", my_sThreadName, cDbName);
		LOG(em_NON), "%s", sql);
		syslog(SYSNO_DB_EXECUTEDIRECT_ERR, "[%s]", cDbName);
		return false;
	}

	//// �擾
	while (TRUE) {
		int sqlrc = clsDB.Fetch();
		if (sqlrc == KIZUODBC_FETCH_OK) {					// �f�[�^�L��

			// ��L�[
			clsDB.GetData(1, &id);
			clsDB.GetData(2, &no);
			if( (MAX_PCODE < id || 0 >= id) || (MAX_CODE_JUDGE < no || 0 >= no) ) {
				// ��L�[�s��
				LOG(em_ERR), "[%s] [%s ��L�[�s�� id=%d no=%d]", my_sThreadName, cDbName, id, no);
				syslog(SYSNO_DB_DATA_ERR, "[%s id=%d no=%d]", cDbName, id, no);
				continue;
			}
			id -= 1;										// 0�I���W���ɕύX����
			no -= 1;										// 0�I���W���ɕύX����
			clsDB.GetData(3, &buf.rec[id].typJudge[no].bUmu );

			// �f�[�^��
			clsDB.GetData(4, &buf.rec[id].typJudge[no].cMeiNo, 32);			// ����No
			clsDB.GetData(5, &buf.rec[id].typJudge[no].cCoilNo, 32);		// ���YNo
			clsDB.GetData(6, &buf.rec[id].typJudge[no].nAtuSei[0]);			// ����������
			clsDB.GetData(7, &buf.rec[id].typJudge[no].nAtuSei[1]);			// ���������
			clsDB.GetData(8, &buf.rec[id].typJudge[no].cKenKijun, 32);		// �����
			clsDB.GetData(9, &buf.rec[id].typJudge[no].cMuke, 32);			// ����
			clsDB.GetData(10, &buf.rec[id].typJudge[no].cYouto, 32);		// �p�r
			clsDB.GetData(11, &buf.rec[id].typJudge[no].cSyuZai, 32);		// ��ގ�
			clsDB.GetData(12, &buf.rec[id].typJudge[no].cOmoteSyori1, 32);	// �\�ʏ���1����
			clsDB.GetData(13, &buf.rec[id].typJudge[no].cHinCode,32);		// �i��

		} else if( sqlrc == KIZUODBC_FETCH_NODATE ) {		// �f�[�^����
			break;	
		} else {								// �G���[
			LOG(em_ERR), "[%s] DB FETCH_ERR [%s]", my_sThreadName, cDbName);
			LOG(em_NON), "%s", sql);
			syslog(SYSNO_DB_FETCH_ERR, "[%s]", cDbName);
			return false;
		}
	}

	LOG(em_MSG), "[%s] %s �擾����", my_sThreadName, cDbName);

	return true;
}

