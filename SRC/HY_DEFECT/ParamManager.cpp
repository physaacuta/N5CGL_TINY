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
// 2024.07.26 Kouno 2CGL�������u�Ӄ֐��x����@�\����(������) ---------------->>>>>
	if( ! Read_CommonMast_LongPd() )   return false;
// 2024.07.26 Kouno 2CGL�������u�Ӄ֐��x����@�\����(������) <<<<<----------------
	if( ! Read_TypeName() )		return false;
	if( ! Read_GradeName() )	return false;
	if( ! Read_PcodeOutput() )	return false;

	// �O�H���֘A
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
		syslog(SYSNO_DB_CONNECT_ERR, "");
		return false;
	}

	//// SQL���쐬
	sql =  "SELECT";
	sql += " �T����ʕ\����ԏ�, �T����ʕ\����ԑ�";
	sql += " FROM ";
	sql += cDbName;

	//// ���s
	if( !clsDB.ExecuteDirect(sql)) {
		LOG(em_ERR), "[%s] SQL���s�G���[", my_sThreadName);
		LOG(em_NON), "%s", sql);
		syslog(SYSNO_DB_EXECUTEDIRECT_ERR, "[%s]", cDbName);
		return false;
	}

	//// �擾
	while (TRUE) {
		int sqlrc = clsDB.Fetch();
		if (sqlrc == KIZUODBC_FETCH_OK) {					// �f�[�^�L��
	
			// �f�[�^��
			buf.nMapDsp[0] = MAP_REPRES_MIN;					// �V�X�e���ŏ��́A�C���N���[�h���擾
			clsDB.GetData(1, &buf.nMapDsp[1]);
			clsDB.GetData(2, &buf.nMapDsp[2]);

		} else if( sqlrc == KIZUODBC_FETCH_NODATE ) {		// �f�[�^����
			break;	
		} else {								// �G���[
			syslog(SYSNO_DB_FETCH_ERR, "[%s]", cDbName);
			LOG(em_ERR), "[%s] FETCH ERR SQL", my_sThreadName);
			LOG(em_NON), "%s", sql);
			return false;
		}
	}

	//// �擾�f�[�^�������o�[�ϐ��ɃZ�b�g
	memcpy( &p, &buf, sizeof(buf));
	LOG(em_MSG), "[%s] %s �擾����", my_sThreadName, cDbName);
	return true;
}

// 2024.07.26 Kouno 2CGL�������u�Ӄ֐��x����@�\����(������) ---------------->>>>>
//------------------------------------------
// �r���萔�p�����[�^�i�������֌W�j�Ǎ�
//------------------------------------------
bool ParamManager::Read_CommonMast_LongPd()
{
	CString sql;					// SQL��


	//// �p�����[�^������
	CString cDbName;
	cDbName = DB_COMMON_MASTER;
	P_COMMON_MAST_LONGPD&	p = m_CommonMast_LongPd;
	P_COMMON_MAST_LONGPD	buf;
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
	sql += " LongPd_GuidanceAvailable ";
	sql += " FROM ";
	sql += cDbName;

	//// ���s
	if( !clsDB.ExecuteDirect(sql)) {
		LOG(em_ERR), "[%s] SQL���s�G���[", my_sThreadName);
		LOG(em_NON), "%s", sql);
		syslog(SYSNO_DB_EXECUTEDIRECT_ERR, "[%s]", cDbName);
		return false;
	}

	//// �擾
	while (TRUE) {
		int sqlrc = clsDB.Fetch();
		if (sqlrc == KIZUODBC_FETCH_OK) {					// �f�[�^�L��
			clsDB.GetData(1, &buf.nLongPd_GuidanceAvailable);
		} else if( sqlrc == KIZUODBC_FETCH_NODATE ) {		// �f�[�^����
			break;	
		} else {								// �G���[
			syslog(SYSNO_DB_FETCH_ERR, "[%s]", cDbName);
			LOG(em_ERR), "[%s] FETCH ERR SQL", my_sThreadName);
			LOG(em_NON), "%s", sql);
			return false;
		}
	}

	//// �擾�f�[�^�������o�[�ϐ��ɃZ�b�g
	memcpy( &p, &buf, sizeof(buf));
	LOG(em_MSG), "[%s] %s �������֌W �擾����", my_sThreadName, cDbName);
	return true;
}
// 2024.07.26 Kouno 2CGL�������u�Ӄ֐��x����@�\����(������) <<<<<----------------

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
		syslog(SYSNO_DB_CONNECT_ERR, "");
		return false;
	}

	//// SQL���쐬
    // �؂�o���摜��NULL���ݒ肳��Ă�����A�؂�o���摜���1=1 / �؂�o���摜���2=2
    // �Ƃ��Đݒ肷��
	sql =  "SELECT";
	sql += " ID, EdasysID, �D��x";
	sql += ", ISNULL(�؏o�摜���1,1), ISNULL(�؏o�摜���2,2)";
	sql += " FROM ";
	sql += cDbName;
	sql += " ORDER BY �D��x ASC ";

	//// ���s
	if( !clsDB.ExecuteDirect(sql)) {
		LOG(em_ERR), "[%s] SQL���s�G���[", my_sThreadName);
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
			if( MAX_TYPE <= id || 0 > id ) {
				// ��L�[�s��
				LOG(em_ERR), "[%s] [%s ��L�[�s�� id=%d]", my_sThreadName, cDbName, id);
				syslog(SYSNO_DB_DATA_ERR, "[%s id=%d]", cDbName, id);					
				continue;
			}
			
			// �f�[�^��
			clsDB.GetData(2, &buf.type_id[buf.num].edasysID);
			clsDB.GetData(3, &buf.type_id[buf.num].priority);

			clsDB.GetData(4, &imgidx );
			if(0<imgidx || imgidx<=MAX_IMAGE_CNT) buf.type_id[buf.num].bEnable[ imgidx-1 ] = true; 

			clsDB.GetData(5, &imgidx );
			if(0<imgidx || imgidx<=MAX_IMAGE_CNT) buf.type_id[buf.num].bEnable[ imgidx-1 ] = true; 

			buf.num++;

		} else if( sqlrc == KIZUODBC_FETCH_NODATE ) {		// �f�[�^����
			break;	
		} else {								// �G���[
			LOG(em_ERR), "[%s] FETCH ERR SQL", my_sThreadName);
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
		syslog(SYSNO_DB_CONNECT_ERR, "");
		return false;
	}

	//// SQL���쐬
	sql =  "SELECT";
	sql += " ID, EdasysID, �D��x";
	sql += " FROM ";
	sql += cDbName;
	sql += " ORDER BY �D��x ASC ";

	//// ���s
	if( !clsDB.ExecuteDirect(sql)) {
		LOG(em_ERR), "[%s] SQL���s�G���[", my_sThreadName);
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
			if( MAX_GRADE <= id || 0 > id ) {
				// ��L�[�s��
				LOG(em_ERR), "[%s] [%s ��L�[�s�� id=%d]", my_sThreadName, cDbName, id);
				syslog(SYSNO_DB_DATA_ERR, "[%s id=%d]", cDbName, id);					
				continue;
			}
			
			// �f�[�^��
			clsDB.GetData(2, &buf.grade_id[buf.num].edasysID);
			clsDB.GetData(3, &buf.grade_id[buf.num].priority);
			buf.num++;

		} else if( sqlrc == KIZUODBC_FETCH_NODATE ) {		// �f�[�^����
			break;	
		} else {								// �G���[
			LOG(em_ERR), "[%s] FETCH ERR SQL", my_sThreadName);
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

	//// ���s
	if( !clsDB.ExecuteDirect(sql)) {
		LOG(em_ERR), "[%s] SQL���s�G���[", my_sThreadName);
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
			id = id -1;		// �g���₷���悤��0�I���W���ɕύX


			// -----------------
			// �f�[�^��
			clsDB.GetData(2, &buf.rec[id].umu);

		} else if( sqlrc == KIZUODBC_FETCH_NODATE ) {		// �f�[�^����
			break;	
		} else {								// �G���[
			LOG(em_ERR), "[%s] FETCH ERR SQL", my_sThreadName);
			LOG(em_NON), "%s", sql);
			syslog(SYSNO_DB_FETCH_ERR, "[%s]", cDbName);
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
	sql += ", �r��EdasysID, �T����ʕ\��_�\, �T����ʕ\��_�� ";
	sql += " FROM ";
	sql += cDbName;

	//// ���s
	if( !clsDB.ExecuteDirect(sql)) {
		LOG(em_ERR), "[%s] SQL���s�G���[", my_sThreadName);
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
			id = id -1;		// �g���₷���悤��0�I���W���ɕύX

			// -----------------
			clsDB.GetData(2, &typid);
			if(-1 == typid) {								// �f�t�H���g
				buf.rec[id].defput.edasysID = typid;

				clsDB.GetData(3, (int*)&buf.rec[id].defput.kind[OUTPUTFLG_DSP_O]);
				clsDB.GetData(4, (int*)&buf.rec[id].defput.kind[OUTPUTFLG_DSP_U]);
			} else {										// �ʏ�
				no = buf.rec[id].nPutNum;
				buf.rec[id].output[no].edasysID = typid;

				clsDB.GetData(3, (int*)&buf.rec[id].output[no].kind[OUTPUTFLG_DSP_O]);
				clsDB.GetData(4, (int*)&buf.rec[id].output[no].kind[OUTPUTFLG_DSP_U]);

				buf.rec[id].nPutNum = no + 1;
			}

		} else if( sqlrc == KIZUODBC_FETCH_NODATE ) {		// �f�[�^����
			break;	
		} else {								// �G���[
			LOG(em_ERR), "[%s] FETCH ERR SQL", my_sThreadName);
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

////------------------------------------------
//// �O�H���}�X�^�[�e�[�u���Ǎ�
////------------------------------------------
//bool ParamManager::Read_MaeMaster()
//{
//	int	nRet;							// ���^�[���R�[�h
//
//	CString	sSql;						// SQL��
//	CString	sTbl;						// �e�[�u����
//
//	P_MAE_MASTER	typTmp;				// �O�H���}�X�^�[
//
//	OdbcBase	clsDB;					// ODBC�N���X
//
//	// ������
//	memset( &typTmp, 0x00, sizeof(typTmp) );
//
//	// �f�[�^�x�[�X�ڑ�
//	if( false == clsDB.Connect(TASKINI_NAME) ){				// �f�[�^�x�[�X�ڑ��G���[
//		LOG(em_ERR), "[%s] DB�ڑ��G���[", my_sThreadName);
//		syslog(SYSNO_DB_CONNECT_ERR, "");
//		return false;
//	}
//
//	// �e�[�u����
//	sTbl	= DB_MAE_MASTER;								// �O�H���}�X�^�[
//
//	// SQL���쐬
//	sSql.Format("SELECT �O�H���R�[�h, �O�H���\�����[�h, �摜�`�� FROM %s WHERE �L������=1", sTbl);
//
//	// SQL���s
//	if( false == clsDB.ExecuteDirect(sSql) ){
//		LOG(em_ERR), "[%s] SQL���s�G���[ %s", my_sThreadName, sSql);
//		syslog(SYSNO_DB_EXECUTEDIRECT_ERR, "[%s]", sTbl);
//		return false;
//	}
//
//	// �e�[�u���f�[�^�擾
//	while( TRUE ){											// �L�����R�[�h��
//		nRet	= clsDB.Fetch();							// ���R�[�h�Q��
//		if( KIZUODBC_FETCH_OK == nRet ){					// ���R�[�h�L��
//			if( typTmp.nRec < MAX_MAE_LINE ){				// �L���o�^��
//				// ���R�[�h�f�[�^�擾
//															// �O�H���R�[�h
//				clsDB.GetData(1, typTmp.typRec[typTmp.nRec].cMaeCode, sizeof(typTmp.typRec[typTmp.nRec].cMaeCode));
//															// �O�H���\�����[�h
//				clsDB.GetData(2, &typTmp.typRec[typTmp.nRec].nMaeDsp);
//															// �摜�`��
//				clsDB.GetData(3, &typTmp.typRec[typTmp.nRec].nImgFmt);
//				typTmp.nRec++;								// ���R�[�h���X�V
//			}
//		} else if( KIZUODBC_FETCH_NODATE == nRet ){			// ���R�[�h����
//			break;
//		} else {											// �Q�ƃG���[
//			LOG(em_ERR), "[%s] FETCH ERR SQL[%s]", my_sThreadName, sSql);
//			syslog(SYSNO_DB_FETCH_ERR, "[%s]", sTbl);
//			return false;
//		}
//	}
//
//	// �ǂݍ��݃f�[�^�i�[
//	memcpy(&m_MaeMaster, &typTmp, sizeof(typTmp));
//	LOG(em_MSG), "[%s] %s �擾���� ����[%d]", my_sThreadName, sTbl, typTmp.nRec);
//
//	return true;
//}
//
////------------------------------------------
//// �O�H���r��}�X�^�[�e�[�u���Ǎ�
////------------------------------------------
//bool ParamManager::Read_MaeTypeName()
//{
//	int	nRet;							// ���^�[���R�[�h
//
//	CString	sSql;						// SQL��
//	CString	sTbl;						// �e�[�u����
//
//	P_MAE_TYPE_NAME	typTmp;				// �O�H���r��}�X�^�[�e�[�u��
//
//	OdbcBase	clsDB;					// ODBC�N���X
//
//	// ������
//	memset( &typTmp, 0x00, sizeof(typTmp) );
//
//	// �f�[�^�x�[�X�ڑ�
//	if( false == clsDB.Connect(TASKINI_NAME) ){				// �f�[�^�x�[�X�ڑ��G���[
//		LOG(em_ERR), "[%s] DB�ڑ��G���[", my_sThreadName);
//		syslog(SYSNO_DB_CONNECT_ERR, "");
//		return false;
//	}
//
//	// �e�[�u����
//	sTbl	= DB_MAE_TYPE_NAME;								// �O�H���r��}�X�^�[
//
//	// SQL���쐬
//	sSql.Format("SELECT �O�H���R�[�h, FFName, ID, �D��x, �\���L�� FROM %s", sTbl);
//
//	// SQL���s
//	if( false == clsDB.ExecuteDirect(sSql) ){
//		LOG(em_ERR), "[%s] SQL���s�G���[ %s", my_sThreadName, sSql);
//		syslog(SYSNO_DB_EXECUTEDIRECT_ERR, "[%s]", sTbl);
//		return false;
//	}
//
//	// �e�[�u���f�[�^�擾
//	while( TRUE ){											// �L�����R�[�h��
//		nRet	= clsDB.Fetch();							// ���R�[�h�Q��
//		if( KIZUODBC_FETCH_OK == nRet ){					// ���R�[�h�L��
//			if( typTmp.nRec < MAX_MAE_TYPE ){				// �L�����R�[�h��
//				// ���R�[�h�f�[�^�擾
//															// �O�H���R�[�h
//				clsDB.GetData(1, typTmp.typRec[typTmp.nRec].cMaeCode, sizeof(typTmp.typRec[typTmp.nRec].cMaeCode));
//															// FFName
//				clsDB.GetData(2, typTmp.typRec[typTmp.nRec].cFFName, sizeof(typTmp.typRec[typTmp.nRec].cFFName));
//				KizuFunction::Trim(typTmp.typRec[typTmp.nRec].cFFName);
//															// ID
//				clsDB.GetData(3, &typTmp.typRec[typTmp.nRec].nTypeID);
//															// �D��x
//				clsDB.GetData(4, &typTmp.typRec[typTmp.nRec].nPriority);
//															// �\���L��
//				clsDB.GetData(5, &typTmp.typRec[typTmp.nRec].nDispID);
//
//				typTmp.nRec++;								// ���R�[�h���X�V
//			}
//		} else if( KIZUODBC_FETCH_NODATE == nRet ){			// ���R�[�h����
//			break;
//		} else {											// �Q�ƃG���[
//			LOG(em_ERR), "[%s] FETCH ERR SQL[%s]", my_sThreadName, sSql);
//			syslog(SYSNO_DB_FETCH_ERR, "[%s]", sTbl);
//			return false;
//		}
//	}
//
//	// �ǂݍ��݃f�[�^�i�[
//	memcpy(&m_MaeTypeName, &typTmp, sizeof(typTmp));
//	LOG(em_MSG), "[%s] %s �擾���� ����[%d]", my_sThreadName, sTbl, typTmp.nRec);
//
//	return true;
//}
//
////------------------------------------------
//// �O�H���r�O���[�h�}�X�^�[�e�[�u���Ǎ�
////------------------------------------------
//bool ParamManager::Read_MaeGradeName()
//{
//	int	nRet;							// ���^�[���R�[�h
//
//	CString	sSql;						// SQL��
//	CString	sTbl;						// �e�[�u����
//
//	P_MAE_GRADE_NAME	typTmp;			// �O�H���r�O���[�h�}�X�^�[�e�[�u��
//
//	OdbcBase	clsDB;					// ODBC�N���X
//
//	// ������
//	memset( &typTmp, 0x00, sizeof(typTmp) );
//
//	// �f�[�^�x�[�X�ڑ�
//	if( false == clsDB.Connect(TASKINI_NAME) ){				// �f�[�^�x�[�X�ڑ��G���[
//		LOG(em_ERR), "[%s] DB�ڑ��G���[", my_sThreadName);
//		syslog(SYSNO_DB_CONNECT_ERR, "");
//		return false;
//	}
//
//	// �e�[�u����
//	sTbl	= DB_MAE_GRADE_NAME;							// �O�H���r�O���[�h�}�X�^�[
//
//	// SQL���쐬
//	sSql.Format("SELECT �O�H���R�[�h, FFName, �O���[�hID, �D��x FROM %s", sTbl);
//
//	// SQL���s
//	if( false == clsDB.ExecuteDirect(sSql) ){
//		LOG(em_ERR), "[%s] SQL���s�G���[ %s", my_sThreadName, sSql);
//		syslog(SYSNO_DB_EXECUTEDIRECT_ERR, "[%s]", sTbl);
//		return false;
//	}
//
//	// �e�[�u���f�[�^�擾
//	while( TRUE ){											// �L�����R�[�h��
//		nRet	= clsDB.Fetch();							// ���R�[�h�Q��
//		if( KIZUODBC_FETCH_OK == nRet ){					// ���R�[�h�L��
//			if( typTmp.nRec < MAX_MAE_GRADE ){				// �L�����R�[�h��
//				// ���R�[�h�f�[�^�擾
//															// �O�H���R�[�h
//				clsDB.GetData(1, typTmp.typRec[typTmp.nRec].cMaeCode, sizeof(typTmp.typRec[typTmp.nRec].cMaeCode));
//															// FFName
//				clsDB.GetData(2, typTmp.typRec[typTmp.nRec].cFFName, sizeof(typTmp.typRec[typTmp.nRec].cFFName));
//				KizuFunction::Trim(typTmp.typRec[typTmp.nRec].cFFName);
//															// ID
//				clsDB.GetData(3, &typTmp.typRec[typTmp.nRec].nGradeID);
//															// �D��x
//				clsDB.GetData(4, &typTmp.typRec[typTmp.nRec].nPriority);
//				typTmp.nRec++;								// ���R�[�h���X�V
//			}
//		} else if( KIZUODBC_FETCH_NODATE == nRet ){			// ���R�[�h����
//			break;
//		} else {											// �Q�ƃG���[
//			LOG(em_ERR), "[%s] FETCH ERR SQL[%s]", my_sThreadName, sSql);
//			syslog(SYSNO_DB_FETCH_ERR, "[%s]", sTbl);
//			return false;
//		}
//	}
//
//	// �ǂݍ��݃f�[�^�i�[
//	memcpy(&m_MaeGradeName, &typTmp, sizeof(typTmp));
//	LOG(em_MSG), "[%s] %s �擾���� ����[%d]", my_sThreadName, sTbl, typTmp.nRec);
//
//	return true;
//}

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
// �߂�l �D��x (�����o-1)
//------------------------------------------
int ParamManager::GetYusenT(long tid) const 
{
	for(int ii=0; ii<m_TypeName.num; ii++) {
		if( m_TypeName.type_id[ii].edasysID == tid ) return m_TypeName.type_id[ii].priority;
	}
	return -1;
}

//------------------------------------------
// �O���[�h�D��x�擾
// long gid �O���[�hEdasysID
// �߂�l �D��x (�����o-1)
//------------------------------------------
int ParamManager::GetYusenG(long gid) const
{
	for(int ii=0; ii<m_GradeName.num; ii++) {
		if( m_GradeName.grade_id[ii].edasysID == gid ) return m_GradeName.grade_id[ii].priority;
	}
	return -1;
}

////------------------------------------------
//// �O�H���r��D��x�擾
//// char*	cProc	�O�H���R�[�h
//// int		nTid	�r��EdasysID
//// �߂�l	�D��x (�����o-1)
////------------------------------------------
//int ParamManager::GetMaeYusenT(char* cProc, int nTid) const
//{
//	int	nPriority	= -1;				// �D��x
//
//	// �O�H���r�팟��
//	for(int ii=0; ii<m_MaeTypeName.nRec; ii++){
//		if( 0 == memcmp(cProc, m_MaeTypeName.typRec[ii].cMaeCode, SIZE_KOUTEI_CODE) ){
//			if( nTid == m_MaeTypeName.typRec[ii].nTypeID ){
//				nPriority	= m_MaeTypeName.typRec[ii].nPriority;
//				break;
//			}
//		}
//	}
//
//	return nPriority;
//}
//
////------------------------------------------
//// �O�H���O���[�h�D��x�擾
//// char*	cProc	�O�H���R�[�h
//// int		nGid	�O���[�hEdasysID
//// �߂�l	�D��x (�����o-1)
////------------------------------------------
//int ParamManager::GetMaeYusenG(char* cProc, int nGid) const
//{
//	int	nPriority	= -1;				// �D��x
//
//	// �O�H���O���[�h����
//	for(int ii=0; ii<m_MaeGradeName.nRec; ii++){
//		if( 0 == memcmp(cProc, m_MaeGradeName.typRec[ii].cMaeCode, SIZE_KOUTEI_CODE) ){
//			if( nGid == m_MaeGradeName.typRec[ii].nGradeID ){
//				nPriority	= m_MaeGradeName.typRec[ii].nPriority;
//				break;
//			}
//		}
//	}
//
//	return nPriority;
//}

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
// �r��ɑ΂��ĐF�����\�����`�F�b�N
// long tid		�r��EdasysID
// int index	�F�� (0�I���W��)
// �߂�l true:�Ώ�
//------------------------------------------
bool ParamManager::Check_ImgDsp(long tid, int index) const
{

	//// �ʏ�
	//return true;

	// �F��
	if(0==tid) return false;
	if(index>=MAX_IMAGE_CNT) return false;
	for(int ii=0; ii<m_TypeName.num; ii++) {
		if( m_TypeName.type_id[ii].edasysID == tid ) {
			return m_TypeName.type_id[ii].bEnable[ index ];
		}
	}

	return false;
}

////------------------------------------------
//// �r��ID�擾
//// char*	cProc	�O�H���R�[�h
//// char*	cType	�O�H���r��
//// �߂�l	�r��ID
////------------------------------------------
//int ParamManager::GetMaeTID(char* cProc, char* cType)
//{
//	int	nTypeID	= 0;
//	char	cName[SIZE_FFNAME_TYPE+1];
//
//	memset(cName, 0x00, sizeof(cName));
//	memcpy(cName, cType, SIZE_FFNAME_TYPE);
//	KizuFunction::Trim(cName);
//
//	// �O�H���r�팟��
//	for(int ii=0; ii<m_MaeTypeName.nRec; ii++){
//		// �r���v
//		if( 0 == memcmp(cProc, m_MaeTypeName.typRec[ii].cMaeCode, SIZE_KOUTEI_CODE)
//		&&  0 == memcmp(cName, m_MaeTypeName.typRec[ii].cFFName,  SIZE_FFNAME_TYPE) ){
//			nTypeID	= m_MaeTypeName.typRec[ii].nTypeID;
//			break;
//		}
//	}
//
//	return	nTypeID;
//}
//
////------------------------------------------
//// �O���[�hID�擾
//// char*	cProc	�O�H���R�[�h
//// char*	cGrade	�O�H���O���[�h
//// �߂�l	�O���[�hID
////------------------------------------------
//int ParamManager::GetMaeGID(char* cProc, char* cGrade)
//{
//	int	nGradeID	= 0;
//	char	cName[SIZE_FFNAME_GRADE+1];
//
//	memset(cName, 0x00, sizeof(cName));
//	memcpy(cName, cGrade, SIZE_FFNAME_GRADE);
//	KizuFunction::Trim(cName);
//
//	// �O�H���O���[�h����
//	for(int ii=0; ii<m_MaeGradeName.nRec; ii++){
//		// �O���[�h��v
//		if( 0 == memcmp(cProc, m_MaeGradeName.typRec[ii].cMaeCode, SIZE_KOUTEI_CODE)
//		&&  0 == memcmp(cName, m_MaeGradeName.typRec[ii].cFFName,  SIZE_FFNAME_GRADE) ){
//			nGradeID	= m_MaeGradeName.typRec[ii].nGradeID;
//			break;
//		}
//	}
//
//	return	nGradeID;
//}
//
////------------------------------------------
//// �\���Ώ��r����
//// char*	cProc	�O�H���R�[�h
//// char*	cType	�O�H���r��
//// char*	cGrade	�O�H���O���[�h
//// �߂�l	���茋��(true=�\���ΏہAfalse=�\���ΏۊO)
////------------------------------------------
//bool ParamManager::CheckDispDef(char* cProc, char* cType, char* cGrade)
//{
//	bool	bDisp		= false;
//	int		nDispID		= 0;
//	int		nGradeID	= 0;
//	char	cName[SIZE_FFNAME_TYPE+1];
//
//	memset(cName, 0x00, sizeof(cName));
//	memcpy(cName, cType, SIZE_FFNAME_TYPE );
//	KizuFunction::Trim(cName);
//
//	// �O���[�hID�擾
//	nGradeID	= GetMaeGID(cProc, cGrade);
//
//	// �O�H���r�팟��
//	for(int ii=0; ii<m_MaeTypeName.nRec; ii++){
//		// �r���v
//		if( 0 == memcmp(cProc, m_MaeTypeName.typRec[ii].cMaeCode, SIZE_KOUTEI_CODE)
//		&&  0 == memcmp(cName, m_MaeTypeName.typRec[ii].cFFName,  SIZE_FFNAME_TYPE) ){
//			// �\���L���擾
//			nDispID	= m_MaeTypeName.typRec[ii].nDispID;
//			// �\���L
//			if( 0 < nDispID ){
//				// �O���[�h�D��x����
//				if( GetMaeYusenG(cProc, nGradeID) <= GetMaeYusenG(cProc, nDispID) ){
//					// �\���Ώ��r
//					bDisp	= true;
//				}
//			}
//			break;
//		}
//	}
//
//	return	bDisp;
//}