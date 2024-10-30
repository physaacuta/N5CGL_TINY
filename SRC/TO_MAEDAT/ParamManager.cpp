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
ThreadManager("ParamManager")
{
	// �V�O�i������
	m_evReadParam = CreateEvent( NULL, FALSE, FALSE, NULL );
}

//------------------------------------------
// �f�X�g���N�^
//------------------------------------------
ParamManager::~ParamManager(void)
{
	// �C�x���g�J��
	CloseHandle( m_evReadParam );
}

//------------------------------------------
// �X���b�h�J�n�O���� (�X���b�h���s����(�V�O�i���Z�b�g�O)�ɃR�[���o�b�N)
//------------------------------------------
int ParamManager::ThreadFirst()
{
	//// �C�x���g�ݒ�
	HANDLE	hArray[] = {m_evReadParam
						};
	SetEventNum( sizeof(hArray)/sizeof(HANDLE), hArray );
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
void ParamManager::ThreadEvent( int nEventNo )
{
	//// �V�O�i���̏���
	enum {	EV_READ = 0,
			};						// �p�����[�^�ǂݍ���

	////// �V�O�i����������
	switch ( nEventNo ) {

//-----------------------------------------------------------------------------------------------
	case EV_READ:					// �p�����[�^�ǂݍ���
		ReadParam_All();			
		break;

//-----------------------------------------------------------------------------------------------
	default :
		// ���肦�Ȃ��I�I
		break;
	}
}

//------------------------------------------
// �S�p�����[�^�ǂݍ���
//------------------------------------------
bool ParamManager::ReadParam_All()
{
	if( ! ReadParam_Type() )		return false;
	if( ! ReadParam_Grade() )		return false;
	if( ! ReadParam_Master() )		return false;
	return true;
}

//------------------------------------------
// �r��p�����[�^�Ǎ�
//------------------------------------------
bool ParamManager::ReadParam_Type()
{
	CString sql;					// SQL��
	int id;

	//// �p�����[�^������
	CString cDbName;
	cDbName = DB_MAE_TYPE_NAME;
	PARAM_TYPE	buf;
	memset( &buf, 0x00, sizeof(buf) );

	//// ODBC�ڑ��N���X �C���X�^���X����
	OdbcBase clsDB;
	if( ! clsDB.Connect( TASKINI_NAME ) ) {
		// �f�[�^�x�[�X�ڑ��G���[
		LOG(em_ERR), "[%s] DB�ڑ��G���[", my_sThreadName );
		syslog(SYSNO_DB_CONNECT_ERR, "");
		return false;
	}

	//// SQL���쐬
	sql =  "SELECT";
	sql += " �O�H���R�[�h, FFName, ID ";
	sql += " FROM ";
	sql += cDbName;

	//// ���s
	if( ! clsDB.ExecuteDirect( sql ) ) {
		LOG(em_NON), "%s", sql );
		LOG(em_ERR), "[%s] �O�H���r��}�X�^�[���s�G���[ [%s]", my_sThreadName, cDbName );
		syslog(SYSNO_DB_EXECUTEDIRECT_ERR, "[%s]", cDbName);
		return false;
	}
	
	id = 0;
	//// �擾
	while (true) {
		int sqlrc = clsDB.Fetch();
		if( sqlrc == KIZUODBC_FETCH_OK ) {					// �f�[�^�L��

			// �f�[�^��
			clsDB.GetDataImage( 1, &buf.type_id[id].cProc, sizeof(buf.type_id[0].cProc) );
			KizuFunction::SpaceToNull( buf.type_id[id].cProc, sizeof(buf.type_id[0].cProc) );
			clsDB.GetDataImage( 2, &buf.type_id[id].cFFName, sizeof(buf.type_id[0].cFFName) );
			KizuFunction::SpaceToNull( buf.type_id[id].cFFName, sizeof(buf.type_id[0].cFFName) );
			clsDB.GetData( 3, &buf.type_id[id].edasysID );
			id++;
			buf.num++;

		} else if( sqlrc == KIZUODBC_FETCH_NODATE ) {		// �f�[�^����
			break;	
		} else {								// �G���[
			LOG(em_NON), "%s", sql );
			LOG(em_ERR), "[%s] �O�H���r��}�X�^�[�擾�G���[ [%s]", my_sThreadName, cDbName );
			syslog(SYSNO_DB_FETCH_ERR, "[%s]", cDbName);
			return false;
		}
	}

	//// �擾�f�[�^�������o�[�ϐ��ɃZ�b�g
	memcpy( &m_Param_Type, &buf, sizeof(buf) );
	LOG(em_MSG), "[%s] %s �擾����", my_sThreadName, cDbName );
	return true;
}

//------------------------------------------
// Gr�p�����[�^�Ǎ�
//------------------------------------------
bool ParamManager::ReadParam_Grade()
{
	CString sql;					// SQL��
	int id;

	//// �p�����[�^������
	CString cDbName;
	cDbName = DB_MAE_GRADE_NAME;
	PARAM_GRADE	buf;
	memset( &buf, 0x00, sizeof(buf) );

	//// ODBC�ڑ��N���X �C���X�^���X����
	OdbcBase clsDB;
	if( ! clsDB.Connect( TASKINI_NAME ) ) {
		// �f�[�^�x�[�X�ڑ��G���[
		LOG(em_ERR), "[%s] DB�ڑ��G���[", my_sThreadName );
		syslog(SYSNO_DB_CONNECT_ERR, "");
		return false;
	}

	//// SQL���쐬
	sql =  "SELECT";
	sql += " �O�H���R�[�h, FFName, �O���[�hID ";
	sql += " FROM ";
	sql += cDbName;

	//// ���s
	if( ! clsDB.ExecuteDirect( sql ) ) {
		LOG(em_NON), "%s", sql );
		LOG(em_ERR), "[%s] �O�H���O���[�h�}�X�^�[���s�G���[ [%s]", my_sThreadName, cDbName );
		syslog(SYSNO_DB_EXECUTEDIRECT_ERR, "[%s]", cDbName);
		return false;
	}
	
	id = 0;

	//// �擾
	while (true) {
		int sqlrc = clsDB.Fetch();
		if( sqlrc == KIZUODBC_FETCH_OK ) {					// �f�[�^�L��

			// �f�[�^��
			clsDB.GetDataImage( 1, &buf.grade_id[id].cProc, sizeof(buf.grade_id[0].cProc) );
			KizuFunction::SpaceToNull( buf.grade_id[id].cProc, sizeof(buf.grade_id[0].cProc) );
			clsDB.GetDataImage( 2, &buf.grade_id[id].cFFName, sizeof(buf.grade_id[0].cFFName) );
			KizuFunction::SpaceToNull( buf.grade_id[id].cFFName, sizeof(buf.grade_id[0].cFFName) );
			clsDB.GetData( 3, &buf.grade_id[id].edasysID );
			id++;
			buf.num++;

		} else if( sqlrc == KIZUODBC_FETCH_NODATE ) {		// �f�[�^����
			break;	
		} else {								// �G���[
			LOG(em_NON), "%s", sql );
			LOG(em_ERR), "[%s] �O�H���O���[�h�}�X�^�[�擾�G���[ [%s]", my_sThreadName, cDbName );
			syslog(SYSNO_DB_FETCH_ERR, "[%s]", cDbName);
			return false;
		}
	}

	//// �擾�f�[�^�������o�[�ϐ��ɃZ�b�g
	memcpy( &m_Param_Grade, &buf, sizeof(buf) );
	LOG(em_MSG), "[%s] %s �擾����", my_sThreadName, cDbName );
	return true;
}

//------------------------------------------
// �O�H���}�X�^�[�e�[�u���Ǎ�
//------------------------------------------
bool ParamManager::ReadParam_Master()
{
	CString sql;					// SQL��
	int id;

	//// �p�����[�^������
	CString cDbName;
	cDbName = DB_MAE_MASTER;
	PARAM_MASTER	buf;
	memset( &buf, 0x00, sizeof(buf) );

	//// ODBC�ڑ��N���X �C���X�^���X����
	OdbcBase clsDB;
	if( ! clsDB.Connect( TASKINI_NAME ) ) {
		// �f�[�^�x�[�X�ڑ��G���[
		LOG(em_ERR), "[%s] DB�ڑ��G���[", my_sThreadName );
		syslog(SYSNO_DB_CONNECT_ERR, "");
		return false;
	}

	//// SQL���쐬
	sql =  "SELECT";
	sql += " �O�H���R�[�h";
	sql += ",�L������";
    sql += ",�O�H���\�����[�h";
	sql += ",�摜�`��";
    // 2014.07.25 �쌴  ���ԘA�g    >>>>>>>>
    sql += ",�����ޔ���";
    // 2014.07.25 �쌴  ���ԘA�g    <<<<<<<<
	sql += " FROM ";
	sql += cDbName;
	sql += " WHERE �L������ <> 0";				// 0�F�����łȂ�
	sql += "   AND �O�H���\�����[�h <> 0";		// 0�F�{�V�X�e���Ŗ��Ǘ��łȂ�

    //// ���s
	if( ! clsDB.ExecuteDirect( sql ) ) {
		LOG(em_NON), "%s", sql );
		LOG(em_ERR), "[%s] �O�H���}�X�^�[���s�G���[ [%s]", my_sThreadName, cDbName );
		syslog(SYSNO_DB_EXECUTEDIRECT_ERR, "[%s]", cDbName);
		return false;
	}
	
	id = 0;

	//// �擾
	while (true) {
		int sqlrc = clsDB.Fetch();
		if( sqlrc == KIZUODBC_FETCH_OK ) {					// �f�[�^�L��

			// �f�[�^��

			clsDB.GetDataImage( 1, &buf.master_id[id].cProc, sizeof(buf.master_id[id].cProc) );
			KizuFunction::SpaceToNull( buf.master_id[id].cProc, sizeof(buf.master_id[id].cProc) );
			clsDB.GetData( 2, &buf.master_id[id].nEnable );
			clsDB.GetData( 3, &buf.master_id[id].nMode );
			clsDB.GetData( 4, &buf.master_id[id].nImgFmt );
			
            // 2014.07.25 �쌴  ���ԘA�g    >>>>>>>>
			clsDB.GetData( 5, &buf.master_id[id].nTasyoZai );
            // 2014.07.25 �쌴  ���ԘA�g    <<<<<<<<

            id++;
			buf.num++;

		} else if( sqlrc == KIZUODBC_FETCH_NODATE ) {		// �f�[�^����
			break;	
		} else {								// �G���[
			LOG(em_NON), "%s", sql );
			LOG(em_ERR), "[%s] �O�H���}�X�^�[�擾�G���[ [%s]", my_sThreadName, cDbName );
			syslog(SYSNO_DB_FETCH_ERR, "[%s]", cDbName);
			return false;
		}
	}

	//// �擾�f�[�^�������o�[�ϐ��ɃZ�b�g
	memcpy( &m_Param_Master, &buf, sizeof(buf) );
	LOG(em_MSG), "[%s] %s �擾����", my_sThreadName, cDbName );
	return true;
}

//------------------------------------------
// �r��EdasysID�擾
// char* cProc		�O�H���R�[�h
// char* cFFName�@�@FF�r��
// �߂�l EdasysID (�����o 0)
//------------------------------------------
int ParamManager::GetTyppeEdasysID( char* cProc, char* cFFName ) const 
{
	for( int ii=0; ii<m_Param_Type.num; ii++ ) {
		
		// �O�H����FF�r�킪��v
		if( 0 == memcmp( m_Param_Type.type_id[ii].cProc, cProc, sizeof(m_Param_Type.type_id[ii].cProc) )
			&& 0 == memcmp( m_Param_Type.type_id[ii].cFFName, cFFName, sizeof(m_Param_Type.type_id[ii].cFFName) ) ) {

			return m_Param_Type.type_id[ii].edasysID;
		}

	}
	return 0;
}

//------------------------------------------
// �O���[�hEdasysID�擾
// char* cProc		�O�H���R�[�h
// char* cFFName�@�@FF�O���[�h
// �߂�l EdasysID (�����o 0)
//------------------------------------------
int ParamManager::GetGradeEdasysID(char* cProc, char* cFFName) const 
{
	for( int ii=0; ii<m_Param_Grade.num; ii++ ) {
		
		// �O�H����FF�O���[�h����v
		if( 0 == memcmp( m_Param_Grade.grade_id[ii].cProc, cProc, sizeof(m_Param_Grade.grade_id[ii].cProc) )
			&& 0 == memcmp( m_Param_Grade.grade_id[ii].cFFName, cFFName, sizeof(m_Param_Grade.grade_id[ii].cFFName) ) ) {
			return m_Param_Grade.grade_id[ii].edasysID;
		}
	}
	return 0;
}

//------------------------------------------
// �O�H���}�X�^�[�O�H���\�����[�h�擾
// char* cProc		�O�H���R�[�h
// �߂�l �O�H���\�����[�h (�����o -1)
//------------------------------------------
int ParamManager::GetDspMode(char* cProc) const 
{
	for( int ii=0; ii<m_Param_Master.num; ii++ ) {
		
		// �O�H���R�[�h����v
		if( 0 == memcmp( m_Param_Master.master_id[ii].cProc, cProc, sizeof(m_Param_Master.master_id[ii].cProc) ) ) {
			return m_Param_Master.master_id[ii].nMode;
		}
	}
	return -1;
}

// 2014.07.25 �쌴  ���ԘA�g    >>>>>>>>
//------------------------------------------
// �����ޔ���擾
//------------------------------------------
// char*	cProc	�O�H���R�[�h
//------------------------------------------
// �߂�l	�����ޔ���[0:�����ށA1:������]
//------------------------------------------
int ParamManager::GetTasyoZai(CString cProc)
{
	int	ii;					// �J�E���^�[
	int nTasyoZai   = 0;    // �����ޔ���[0:������]

    // �O�H���R�[�h����
	for(ii=0; ii<m_Param_Master.num; ii++){ // �o�^����
		// �O�H����v
		if( 0 == memcmp(cProc, m_Param_Master.master_id[ii].cProc, 2) ){
			// �����ޔ���
			nTasyoZai	= m_Param_Master.master_id[ii].nTasyoZai;
			break;
		}
	}

	return	nTasyoZai;
}
// 2014.07.25 �쌴  ���ԘA�g    <<<<<<<<
