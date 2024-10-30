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
ThreadManager("ParamMgr")
{
	//// �n���h������
}

//------------------------------------------
// �f�X�g���N�^
//------------------------------------------
ParamManager::~ParamManager(void)
{
	//// �n���h�����
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
// �X���b�h�C�x���g����
// int nEventNo �C�x���gNo (0�I���W��)
//------------------------------------------
void ParamManager::ThreadEvent(int nEventNo)
{
	////// �V�O�i���̏���
	enum {	EV_QUE = 0,						// �X���b�h�L���[�ʒm
	};

	////// �V�O�i����������
	switch (nEventNo) {

//-----------------------------------------------------------------------------------------------
	case EV_QUE: 							// �X���b�h�L���[�ʒm
		COMMON_QUE* pDeli;				// �X���b�h�L���[�󂯓n��
		pDeli = gque_Deli.GetFromHead();
		ExecParam(pDeli);
		delete pDeli;
		break;
	
//-----------------------------------------------------------------------------------------------
	default :
		// ���肦�Ȃ��I�I
		LOG(em_ERR), "[%s] ThreadEvent EventNo NG=%d", my_sThreadName, nEventNo );
		return;
	}
}

//------------------------------------------
// �p�����[�^�Ǎ�����
// COMMON_QUE* pQue �L���[�󂯓n��
//------------------------------------------
void ParamManager::ExecParam(COMMON_QUE* pQue)
{
	// �ʓ|�Ȃ̂őS���ǂނɕύX
	ReadParam_All(false);
}

// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// �p�����[�^�Ǎ�


//------------------------------------------
// �S�p�����[�^�ǂݍ���
// bool init true �����N�����̂�
//------------------------------------------
bool ParamManager::ReadParam_All(bool init)
{
	// �����N�����̂�
	if(init) {
		if( ! Read_CommonMast() )	return false;
	}
	// ����ȊO
	if( ! Read_TypeName() )		return false;
	if( ! Read_GradeName() )	return false;
	if( ! Read_PcodeOutput() )	return false;
	if( ! Read_PallOutput() )	return false;

	// �O�H�����֘A
	//if( ! Read_MaeMaster() )	return false;
	//if( ! Read_MaeTypeName() )	return false;
	//if( ! Read_MaeGradeName() )	return false;

	return true;
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
		return false;
	}

	//// SQL���쐬
	sql =  "SELECT";
	sql += " �T����ʕ\����ԏ�, �T����ʕ\����ԑ�";
	sql += " ,�ݔ���_������__�㗬�J����";
	sql += " ,�ݔ���_������__SPM";
	sql += " ,�ݔ���_�\����__��������";
	sql += " ,�ݔ���_�\����__�\������";
	sql += " ,�ݔ���_�\����__�V���[";
	sql += " FROM ";
	sql += cDbName;

	//// ���s
	if( !clsDB.ExecuteDirect(sql)) {
		LOG(em_ERR), "[%s] SQL���s�G���[ %s", my_sThreadName, sql);
		return false;
	}

	//// �擾
	while (TRUE) {
		int sqlrc = clsDB.Fetch();
		if (sqlrc == KIZUODBC_FETCH_OK) {					// �f�[�^�L��
	
			// �f�[�^��
			buf.nMapDsp[0] = MAP_REPRES_MIN;					// �V�X�e���ŏ��́A�C���N���[�h���擾
			clsDB.GetData(1, &buf.nMapDsp[1] );
			clsDB.GetData(2, &buf.nMapDsp[2]);
			clsDB.GetData(3, &buf.nBaseToCamera);
			clsDB.GetData(4, &buf.nBaseToSpm);
			clsDB.GetData(5, &buf.nDspToUraCamera);
			clsDB.GetData(6, &buf.nDspToOmoteCamera);
			clsDB.GetData(7, &buf.nDspToCut);

		} else if( sqlrc == KIZUODBC_FETCH_NODATE ) {		// �f�[�^����
			break;	
		} else {								// �G���[
			LOG(em_ERR), "[%s] %s", my_sThreadName, sql);
			return false;
		}
	}
	
	// 5CGL
	buf.nSpmToUraCamera  = 82296;		// SPM �` ��������[mm]
	buf.nCameraToDsp = 276639;			// �J�����ʒu �` �\����_[mm]
	buf.nCameraToCamera = 23594;		// �������ʒu �` �\�����ʒu[mm]
	buf.nDeflistFilter = 0;				// �d���׈ꗗ���_�t�B���^

	//// �擾�f�[�^�������o�[�ϐ��ɃZ�b�g
	memcpy( &p, &buf, sizeof(buf));
	LOG(em_MSG), "[%s] %s �擾����", my_sThreadName, cDbName);
	return true;
}

//------------------------------------------
// �r��p�����[�^�Ǎ�
//------------------------------------------
bool ParamManager::Read_TypeName()
{
	CString sql;					// SQL��
	int id;
	int imgidx;

	//// �p�����[�^������
	CString cDbName;
	cDbName = DB_TYPE_NAME;
	P_TYPE_NAME&	p = m_TypeName;
	P_TYPE_NAME		buf;
	memset(&buf, 0x00, sizeof(buf));


	//// ODBC�ڑ��N���X �C���X�^���X����
	OdbcBase clsDB;
	if( ! clsDB.Connect(TASKINI_NAME)) {
		// �f�[�^�x�[�X�ڑ��G���[
		LOG(em_ERR), "[%s] DB�ڑ��G���[", my_sThreadName);
		return false;
	}

	//// SQL���쐬
	sql =  "SELECT";
	sql += " ID, EdasysID, �D��x";
	sql += ", ISNULL(�؏o�摜���1,1), ISNULL(�؏o�摜���2,2)";
	sql += " FROM ";
	sql += cDbName;
	sql += " ORDER BY �D��x ASC ";


	//// ���s
	if( !clsDB.ExecuteDirect(sql)) {
		return false;
	}

	//// �擾
	while (TRUE) {
		int sqlrc = clsDB.Fetch();
		if (sqlrc == KIZUODBC_FETCH_OK) {					// �f�[�^�L��

			// ��L�[
			clsDB.GetData(1, &id);
			if( MAX_TYPE <= id || 0 > id ) {
				// ��L�[�s��
				LOG(em_ERR), "[%s] [%s ��L�[�s�� id=%d]", my_sThreadName, cDbName, id);
				continue;
			}
			
			// �f�[�^��
			clsDB.GetData(2, &buf.type_id[buf.num].edasysID );
			clsDB.GetData(3, &buf.type_id[buf.num].priority );

			clsDB.GetData(4, &imgidx );
			if(0<imgidx || imgidx<=MAX_IMAGE_CNT) buf.type_id[buf.num].bEnable[ imgidx-1 ] = true; 

			clsDB.GetData(5, &imgidx );
			if(0<imgidx || imgidx<=MAX_IMAGE_CNT) buf.type_id[buf.num].bEnable[ imgidx-1 ] = true; 

			buf.num++;

		} else if( sqlrc == KIZUODBC_FETCH_NODATE ) {		// �f�[�^����
			break;	
		} else {								// �G���[
			return false;
		}
	}

	//// �擾�f�[�^�������o�[�ϐ��ɃZ�b�g
	memcpy( &p, &buf, sizeof(buf));
	LOG(em_MSG), "[%s] %s �擾����", my_sThreadName, cDbName);
	return true;
}

//------------------------------------------
// Gr�p�����[�^�Ǎ�
//------------------------------------------
bool ParamManager::Read_GradeName()
{
	CString sql;					// SQL��
	int id;

	//// �p�����[�^������
	CString cDbName;
	cDbName = DB_GRADE_NAME;
	P_GRADE_NAME&	p = m_GradeName;
	P_GRADE_NAME	buf;
	memset(&buf, 0x00, sizeof(buf));

	//// ODBC�ڑ��N���X �C���X�^���X����
	OdbcBase clsDB;
	if( ! clsDB.Connect(TASKINI_NAME)) {
		// �f�[�^�x�[�X�ڑ��G���[
		LOG(em_ERR), "[%s] DB�ڑ��G���[", my_sThreadName);
		return false;
	}

	//// SQL���쐬
	sql =  "SELECT";
	sql += " ID, EdasysID, �D��x, ���׃O���[�h";
	sql += " FROM ";
	sql += cDbName;
	sql += " ORDER BY �D��x ASC ";

	//// ���s
	if( !clsDB.ExecuteDirect(sql)) {
		//syslog(SYSNO_DB_EXECUTEDIRECT_ERR, "[%s]", cDbName);
		return false;
	}

	//// �擾
	while (TRUE) {
		int sqlrc = clsDB.Fetch();
		if (sqlrc == KIZUODBC_FETCH_OK) {					// �f�[�^�L��

			// ��L�[
			clsDB.GetData(1, &id);
			if( MAX_GRADE <= id || 0 > id ) {
				// ��L�[�s��
				LOG(em_ERR), "[%s] [%s ��L�[�s�� id=%d]", my_sThreadName, cDbName, id);
				//syslog(SYSNO_DB_DATA_ERR, "[%s id=%d]", cDbName, id);					
				continue;
			}
			
			// �f�[�^��
			clsDB.GetData(2, &buf.grade_id[buf.num].edasysID );
			clsDB.GetData(3, &buf.grade_id[buf.num].priority );
			clsDB.GetData(4, &buf.grade_id[buf.num].nDefDiv );
			buf.num++;

		} else if( sqlrc == KIZUODBC_FETCH_NODATE ) {		// �f�[�^����
			break;	
		} else {								// �G���[
			//syslog(SYSNO_DB_FETCH_ERR, "[%s]", cDbName);
			return false;
		}
	}

	//// �擾�f�[�^�������o�[�ϐ��ɃZ�b�g
	memcpy( &p, &buf, sizeof(buf));
	LOG(em_MSG), "[%s] %s �擾����", my_sThreadName, cDbName);
	return true;
}


//------------------------------------------
// �����p�^�[���ʃp�����[�^�Ǎ�
//------------------------------------------
bool ParamManager::Read_PcodeOutput()
{
	CString sql;					// SQL��
	int id;
	int no;

	//// �p�����[�^������
	CString cDbName;
	P_PCODE_OUTPUT&	p = m_PcodeOutput;
	P_PCODE_OUTPUT		buf;
	memset(&buf, 0x00, sizeof(buf));


	//// ODBC�ڑ��N���X �C���X�^���X����
	OdbcBase clsDB;
	if( ! clsDB.Connect(TASKINI_NAME)) {
		// �f�[�^�x�[�X�ڑ��G���[
		LOG(em_ERR), "[%s] DB�ڑ��G���[", my_sThreadName);
		//syslog(SYSNO_DB_CONNECT_ERR, "");
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

	//// ���s
	if( !clsDB.ExecuteDirect(sql)) {
		//syslog(SYSNO_DB_EXECUTEDIRECT_ERR, "[%s]", cDbName);
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
				//syslog(SYSNO_DB_DATA_ERR, "[%s id=%d]", cDbName, id);					
				continue;
			}
			id = id -1;		// �g���₷���悤��0�I���W���ɕύX


			// -----------------
			// �f�[�^��
			clsDB.GetData(2, &buf.rec[id].umu );

		} else if( sqlrc == KIZUODBC_FETCH_NODATE ) {		// �f�[�^����
			break;	
		} else {								// �G���[
			//syslog(SYSNO_DB_FETCH_ERR, "[%s]", cDbName);
			return false;
		}
	}

	////////////////////////////////////
	// �o�͏���
	////////////////////////////////////
	int typid;	
	no = 0;
	cDbName = DB_PCODE_OUTPUT;
	//// SQL���쐬
	sql =  "SELECT";
	sql += " pcode";
	sql += " , �r��EdasysID , �T����ʕ\��_�\, �T����ʕ\��_�� ";
	sql += " , �����P���r�_�}�b�v��ʕ\�� ";
	sql += " FROM ";
	sql += cDbName;

	//// ���s
	if( !clsDB.ExecuteDirect(sql)) {
		//syslog(SYSNO_DB_EXECUTEDIRECT_ERR, "[%s]", cDbName);
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
				//syslog(SYSNO_DB_DATA_ERR, "[%s id=%d]", cDbName, id);					
				continue;
			}
			id = id -1;		// �g���₷���悤��0�I���W���ɕύX

			// -----------------
			clsDB.GetData(2, &typid);
			if(-1 == typid) {								// �f�t�H���g
				buf.rec[id].defput.edasysID = typid;

				clsDB.GetData(3, (int*)&buf.rec[id].defput.kind[OUTPUTFLG_DSP_O]);
				clsDB.GetData(4, (int*)&buf.rec[id].defput.kind[OUTPUTFLG_DSP_U]);
				clsDB.GetData(5, (int*)&buf.rec[id].defput.kind[OUTPUTFLG_MAP]);

			} else {										// �ʏ�
				no = buf.rec[id].nPutNum;
				buf.rec[id].output[no].edasysID = typid;

				clsDB.GetData(3, (int*)&buf.rec[id].output[no].kind[OUTPUTFLG_DSP_O]);
				clsDB.GetData(4, (int*)&buf.rec[id].output[no].kind[OUTPUTFLG_DSP_U]);
				clsDB.GetData(5, (int*)&buf.rec[id].output[no].kind[OUTPUTFLG_MAP]);

				buf.rec[id].nPutNum = no + 1;
			}

		} else if( sqlrc == KIZUODBC_FETCH_NODATE ) {		// �f�[�^����
			break;	
		} else {								// �G���[
			//syslog(SYSNO_DB_FETCH_ERR, "[%s]", cDbName);
			return false;
		}
	}

	//// �擾�f�[�^�������o�[�ϐ��ɃZ�b�g
	memcpy( &p, &buf, sizeof(buf));
	LOG(em_MSG), "[%s] %s �擾����", my_sThreadName, cDbName);

	return true;
}

//------------------------------------------
// �����p�^�[�����ʃp�����[�^�Ǎ�
//------------------------------------------
bool ParamManager::Read_PallOutput()
{
	CString sql;					// SQL��
	int no;

	//// �p�����[�^������
	CString cDbName;
	P_PALL_OUTPUT&	p = m_PallOutput;
	P_PALL_OUTPUT		buf;
	memset(&buf, 0x00, sizeof(buf));


	//// ODBC�ڑ��N���X �C���X�^���X����
	OdbcBase clsDB;
	if (!clsDB.Connect(TASKINI_NAME)) {
		// �f�[�^�x�[�X�ڑ��G���[
		LOG(em_ERR), "[%s] DB�ڑ��G���[", my_sThreadName);
		//syslog(SYSNO_DB_CONNECT_ERR, "");
		return false;
	}


	////////////////////////////////////
	// �o�͏���
	////////////////////////////////////
	int typid;
	no = 0;
	cDbName = DB_PALL_OUTPUT;
	//// SQL���쐬
	sql = "SELECT";
	sql += " �r��EdasysID , �d���׈ꗗ��ʕ\��_�ڍ׏���, �d���׈ꗗ��ʕ\��_�ȈՏ��� ";
	sql += " FROM ";
	sql += cDbName;

	//// ���s
	if (!clsDB.ExecuteDirect(sql)) {
		//syslog(SYSNO_DB_EXECUTEDIRECT_ERR, "[%s]", cDbName);
		return false;
	}

	//// �擾
	while (TRUE) {
		int sqlrc = clsDB.Fetch();
		if (sqlrc == KIZUODBC_FETCH_OK) {					// �f�[�^�L��

								// ��L�[
								// -----------------
			clsDB.GetData(1, &typid);
			if (-1 == typid) {								// �f�t�H���g
				buf.inf.defput.edasysID = typid;

				clsDB.GetData(2, (int*)&buf.inf.defput.kind[PALLOUT_DETAIL]);
				clsDB.GetData(3, (int*)&buf.inf.defput.kind[PALLOUT_SIMPLE]);

			}
			else {										// �ʏ�
				no = buf.inf.nPutNum;
				buf.inf.output[no].edasysID = typid;

				clsDB.GetData(2, (int*)&buf.inf.output[no].kind[OUTPUTFLG_DSP_O]);
				clsDB.GetData(3, (int*)&buf.inf.output[no].kind[OUTPUTFLG_DSP_U]);

				buf.inf.nPutNum = no + 1;
			}

		}
		else if (sqlrc == KIZUODBC_FETCH_NODATE) {		// �f�[�^����
			break;
		}
		else {								// �G���[
											//syslog(SYSNO_DB_FETCH_ERR, "[%s]", cDbName);
			return false;
		}
	}

	//// �擾�f�[�^�������o�[�ϐ��ɃZ�b�g
	memcpy(&p, &buf, sizeof(buf));
	LOG(em_MSG), "[%s] %s �擾����", my_sThreadName, cDbName);

	return true;
}

// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// �o�͏����`�F�b�N
//------------------------------------------
// �r��EdasysID �̗L��
// long tid �r��EdasysID
// �߂�l ���A���
//------------------------------------------
bool ParamManager::CheckEdasysID_T(long tid) const 
{
	if(0==tid) return false;
	for(int ii=0; ii<m_TypeName.num; ii++) {
		if( m_TypeName.type_id[ii].edasysID == tid ) return true;
	}
	return false;
}
//------------------------------------------
// �O���[�hEdasysID �̗L��
// long tid �O���[�hEdasysID
// �߂�l ���A���
//------------------------------------------
bool ParamManager::CheckEdasysID_G(long gid) const 
{
	if(0==gid) return false;
	for(int ii=0; ii<m_GradeName.num; ii++) {
		if( m_GradeName.grade_id[ii].edasysID == gid ) return true;
	}
	return false;
}

//------------------------------------------
// �r��D��x�擾
// long tid �r��EdasysID
// �߂�l �D��x (�����o:0)
//------------------------------------------
int ParamManager::GetYusenT(long tid) const 
{
	for(int ii=0; ii<m_TypeName.num; ii++) {
		if( m_TypeName.type_id[ii].edasysID == tid ) return m_TypeName.type_id[ii].priority;
	}
	return 0;
}

//------------------------------------------
// �O���[�h�D��x�擾
// long gid �O���[�hEdasysID
// �߂�l �D��x (�����o:0)
//------------------------------------------
int ParamManager::GetYusenG(long gid) const
{
	for(int ii=0; ii<m_GradeName.num; ii++) {
		if( m_GradeName.grade_id[ii].edasysID == gid ) return m_GradeName.grade_id[ii].priority;
	}
	return 0;
}

//------------------------------------------
// ���׋敪�擾
// long gid �O���[�hEdasysID
// �߂�l �D��x (�����o:0)
//------------------------------------------
int ParamManager::GetDefDivG(long gid) const
{
	for(int ii=0; ii<m_GradeName.num; ii++) {
		if( m_GradeName.grade_id[ii].edasysID == gid ) return m_GradeName.grade_id[ii].nDefDiv;
	}
	return 0;
}



//------------------------------------------
// �o�͑Ώۂ̌��ׂ�����
// EM_OUTPUTFLG output �ǂ̏o�͏������`�F�b�N����́H
// long pcode	�����p�^�[�� (0�I���W��)
// long tid		�r��EdasysID
// long gid		�O���[�hEdasysID
// �߂�l true:�Ώ�
//------------------------------------------
bool ParamManager::Check_Output(EM_OUTPUTFLG output, long pcode, long tid, long gid) const
{
	// �����`�F�b�N
	if( 0 > pcode || MAX_PCODE <= pcode ) {
		LOG(em_ERR), "[%s] output=%d pcodeNG=%d", my_sThreadName, output, pcode);	
		return false;
	}

	//// �r��EdasysID �w��L��H
	for(int ii=0; ii<m_PcodeOutput.rec[pcode].nPutNum; ii++) {
		if( tid == m_PcodeOutput.rec[pcode].output[ii].edasysID ) {
			// Gr�ݒ薳�����r��̈� �S���ΏۊO
			if( 0 == m_PcodeOutput.rec[pcode].output[ii].kind[output] ) return false;

			// Gr�ݒ�L����r��̈� �D��x��r
			if( GetYusenG(gid) <= GetYusenG(m_PcodeOutput.rec[pcode].output[ii].kind[output]) )	return true;
			else																				return false;
		}
	}

	//// �����Ƃ��� �f�t�H���g�w�� (�R�R�܂ŗ����ꍇ�́A�r��ݒ薳���̎�)
	// Gr�ݒ薳�����r��̈� �S���ΏۊO
	if( 0 == m_PcodeOutput.rec[pcode].defput.kind[output] ) return false;

	// Gr�ݒ�L����r��̈� �D��x��r
	if( GetYusenG(gid) <= GetYusenG(m_PcodeOutput.rec[pcode].defput.kind[output]) )	return true;
	else																			return false;
}
//------------------------------------------
// �o�͑Ώۂ̌��ׂ�����
// EM_PALLOUT output �ǂ̏o�͏������`�F�b�N����́H
// long tid		�r��EdasysID
// long gid		�O���[�hEdasysID
// �߂�l true:�Ώ�
//------------------------------------------
bool ParamManager::Check_PallOutput(EM_PALLOUT output, long tid, long gid) const
{
	//// �r��EdasysID �w��L��H
	for (int ii = 0; ii<m_PallOutput.inf.nPutNum; ii++) {
		if (tid == m_PallOutput.inf.output[ii].edasysID) {
			// Gr�ݒ薳�����r��̈� �S���ΏۊO
			if (0 == m_PallOutput.inf.output[ii].kind[output]) return false;

			// Gr�ݒ�L����r��̈� �D��x��r
			if (GetYusenG(gid) <= GetYusenG(m_PallOutput.inf.output[ii].kind[output]))	return true;
			else																		return false;
		}
	}

	//// �����Ƃ��� �f�t�H���g�w�� (�R�R�܂ŗ����ꍇ�́A�r��ݒ薳���̎�)
	// Gr�ݒ薳�����r��̈� �S���ΏۊO
	if (0 == m_PallOutput.inf.defput.kind[output]) return false;

	// Gr�ݒ�L����r��̈� �D��x��r
	if (GetYusenG(gid) <= GetYusenG(m_PallOutput.inf.defput.kind[output]))	return true;
	else																	return false;
}
//------------------------------------------
// �r��ɑ΂��Ċe�p�x���\�����`�F�b�N
// long tid		�r��EdasysID
// int index	�p�x (0�I���W��)
// �߂�l true:�Ώ�
//------------------------------------------
bool ParamManager::Check_ImgDsp(long tid, int index) const
{
	// �ʏ�
	return true;

	// �F��
	if(0==tid) return false;
	for(int ii=0; ii<m_TypeName.num; ii++) {
		if( m_TypeName.type_id[ii].edasysID == tid ) {
			return m_TypeName.type_id[ii].bEnable[ index ];
		}
	}

	return false;
}


// �����_����Tid���擾����
int ParamManager::GetRandTid(int pcode)
{
	int nTid;

	// �w�肳�ꂽpcode ���� �g�p�ł����r��ID�̐������߂�

	// �g�p�ł����r��̐����ŗ����𔭐�������

	// TID�����߂�
	nTid = rand() % 3 + 1;

	return nTid;
}