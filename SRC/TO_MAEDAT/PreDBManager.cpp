#include "StdAfx.h"
#include "PreDBManager.h"

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
PreDBManager::PreDBManager():
ThreadManager("PreDBManager")
{
#ifdef LOCAL	// �f�o�b�N���́A������
	//*/----> ���e�X�g�p�R�[�h
	CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, 2048 * 5, TBL_TO_COIL_NAME);
	//*///<<--- ���e�X�g�p�R�[�h
#endif			// �f�o�b�N���́A������

	// �����T�[�o�[�Q��
	mcls_pSelect2TCM	= new PreDBSelectManager(TO_MAEDAT,"TO_MAEDAT_RETRY_2TCM","LINE_2TCM_SQL");
	mcls_pSelect3TCM    = new PreDBSelectManager(TO_MAEDAT,"TO_MAEDAT_RETRY_3TCM","LINE_3TCM_SQL");
	// 20200830 �≄�@�������u�����@�@�n�[�h�E�\�t�g�i2TCM�����X�V�j �a�� ----->>>
	mcls_pSelectN2TCM_ANA = new PreDBSelectManager(TO_MAEDAT, "TO_MAEDAT_RETRY_2TCM", "LINE_N2TCM_ANA_SQL");
	// 20200830 �≄�@�������u�����@�@�n�[�h�E�\�t�g�i2TCM�����X�V�j �a�� <<<-----
	// 20221021 2TCM���@���ɔ������H������ ���� --->>>
	mcls_pSelectN2TCM_KIZU = new PreDBSelectManager(TO_MAEDAT, "TO_MAEDAT_RETRY_2TCM", "LINE_N2TCM_KIZU_SQL");
	// 20221021 2TCM���@���ɔ������H������ ���� ---<<<

	m_emLastResult_2TCM	= PreDBSelectManager::INIT_DB;
	m_emLastResult_3TCM = PreDBSelectManager::INIT_DB;
	// 20200830 �≄�@�������u�����@�@�n�[�h�E�\�t�g�i2TCM�����X�V�j �a�� ----->>>
	m_emLastResult_N2TCM_ANA = PreDBSelectManager::NON_DB;
	// 20200830 �≄�@�������u�����@�@�n�[�h�E�\�t�g�i2TCM�����X�V�j �a�� <<<-----
	// 20221021 2TCM���@���ɔ������H������ ���� --->>>
	m_emLastResult_N2TCM_KIZU = PreDBSelectManager::NON_DB;
	// 20221021 2TCM���@���ɔ������H������ ���� ---<<<

	//// ���L������
	// �R�C�����e�[�u��
	mem_connect(&m_hCoil, (LPVOID *)&mtbl_pCoil, TBL_TO_COIL_NAME);
	if (m_hCoil == NULL) {
		syslog(SYSNO_MEM_OPEN, "�R�C�����e�[�u��[%s] �A�N�Z�X���s", TBL_TO_COIL_NAME);
		_ASSERT(FALSE);
	}
}

//------------------------------------------
// �f�X�g���N�^
//------------------------------------------
PreDBManager::~PreDBManager(void)
{
	//// ���L�������J��
	mem_close(&m_hCoil, (LPVOID *)&mtbl_pCoil);							// �R�C�����

	delete mcls_pSelect2TCM;
	delete mcls_pSelect3TCM;
	// 20200830 �≄�@�������u�����@�@�n�[�h�E�\�t�g�i2TCM�����X�V�j �a�� ----->>>
	delete mcls_pSelectN2TCM_ANA;
	// 20200830 �≄�@�������u�����@�@�n�[�h�E�\�t�g�i2TCM�����X�V�j �a�� <<<-----
	// 20221021 2TCM���@���ɔ������H������ ���� --->>>
	delete mcls_pSelectN2TCM_KIZU;
	// 20221021 2TCM���@���ɔ������H������ ���� ---<<<

}

//------------------------------------------
// �X���b�h�J�n�O���� (�X���b�h���s����(�V�O�i���Z�b�g�O)�ɃR�[���o�b�N)
//------------------------------------------
int PreDBManager::ThreadFirst()
{
	//// �C�x���g�ݒ�
	HANDLE	hArray[] = {gque_AddSQLData.g_evSem
						};
	SetEventNum( sizeof(hArray)/sizeof(HANDLE), hArray );
	return 0;
}

//------------------------------------------
// �X���b�h�I���O���� (�I���V�O�i����ɃR�[���o�b�N)
//------------------------------------------
int PreDBManager::ThreadLast()
{
	//int		nKizuKenNoCnt;	// 2014.05.09 �R�����g��
	//bool	bDbMode;			// 2014.05.09 �R�����g��
	DeliveryPreDB* pDeli;

	// �󂯓n���L���[�Ɋi�[����Ă��鍀�ڂ�S�ĊJ��
	while( TRUE ) {
		pDeli = gque_AddSQLData.GetFromHead( 0 );
		if( pDeli == NULL ) break;

		// �J��
		SAFE_DELETE_ARRAY( pDeli )
	}

	return 0;
}

//------------------------------------------
// �X���b�h�C�x���g����
// int nEventNo �C�x���gNo (0�I���W��)
//------------------------------------------
void PreDBManager::ThreadEvent( int nEventNo )
{
	DeliveryPreDB* pDeli;				// �X���b�h�L���[�󂯓n��

	// �V�O�i���̏���
	enum {	EV_DB = 0					// �O�H�����я��擾
			};

	////// �V�O�i����������
	switch ( nEventNo ) {

//-----------------------------------------------------------------------------------------------
		case EV_DB:

			LOG(em_MSG), "[%s] �O�H�����ю擾�v��", my_sThreadName );

			pDeli = gque_AddSQLData.GetFromHead();
			
			// DB�o�^����
			DBInsMain( pDeli );

			// ���
			SAFE_DELETE_ARRAY( pDeli )

			break;

//-----------------------------------------------------------------------------------------------
	default :
		// ���肦�Ȃ��I�I
		LOG(em_ERR), "[%s] ThreadEvent EventNo NG=%d", my_sThreadName, nEventNo );
		break;
	}
	return;
}

//------------------------------------------
// DB�o�^���C������
// DeliveryPreDB* pDeli  �L���[�f�[�^
//------------------------------------------
void  PreDBManager::DBInsMain( DeliveryPreDB* pDeli )
{
	//int   nKizuKenNoCnt;  // 2014.05.09 �R�����g��
	//bool  bDbMode;		// 2014.05.09 �R�����g��

    CString     cMyDbSection;   // DB�Z�N�V����
    OdbcBase    clsPreDB;       // �O�H��DB�C���X�^���X
    EM_PRE_LINE PreLine;        // �O�H������

	//// ODBC�ڑ��N���X �C���X�^���X����
	OdbcBase clsDB;
	if( ! clsDB.Connect( TASKINI_NAME ) ) {
		// �f�[�^�x�[�X�ڑ��G���[
		LOG(em_ERR), "[%s] DB�ڑ��G���[", my_sThreadName );
		syslog(SYSNO_DB_CONNECT_ERR, "");
		return;
	}

    LOG(em_MSG), "[%s] �O�H���o�^�����J�n [�Ǘ�No = %s] [�O�H���Ǘ�No = %s] [�\���敪 = %d]",
		my_sThreadName, pDeli->cKizuKenNo, pDeli->cPreKizuKenNo, pDeli->nOuDiv );

    //// �ꉞ�Ώۂ��`�F�b�N
	if( ! CheckInsert( &clsDB, pDeli ) ) {
		LOG(em_WAR), "[%s] �o�^�ΏۊO [�Ǘ�No = %s] [�O�H���Ǘ�No = %s] [�\���敪 = %d]",
			my_sThreadName, pDeli->cKizuKenNo, pDeli->cPreKizuKenNo, pDeli->nOuDiv );
		return;
	}

	// �r���o�H������O�H��������
	if (0 == strcmp(KOUTEI_CODE_2TCM, pDeli->cLineCode)){
		// 20200830 �≄�@�������u�����@�@�n�[�h�E�\�t�g�i2TCM�����X�V�j �a�� ----->>>
		//PreLine = PRE_LINE_2TCM;		// 2TCM	
		//cMyDbSection = "DB_2TCM";
		// �r��ID�ɂĐV���̔�����s��
		if (0 == strncmp(pDeli->cPreKizuKenNo, KIZUKEN_CODE_2TCM, SIZE_KIZUKEN_CODE)) {
			PreLine = PRE_LINE_2TCM;		// 2TCM	
			cMyDbSection = "DB_2TCM";
		}
		else {
			// 20221021 2TCM���@���ɔ������H������ ���� --->>>
			//PreLine = PRE_LINE_N2TCM_ANA;	// N2TCM_ANA	
			//cMyDbSection = "DB_N2TCM_ANA";
			if (0 == strncmp(pDeli->cPreKizuKenNo, KIZUKEN_CODE_N2TCM_VZ, SIZE_KIZUKEN_CODE)) {
				PreLine = PRE_LINE_N2TCM_KIZU;	// N2TCM_KIZU	
				cMyDbSection = "DB_N2TCM_KIZU";
			}
			else {
				PreLine = PRE_LINE_N2TCM_ANA;	// N2TCM_ANA	
				cMyDbSection = "DB_N2TCM_ANA";
			}
			// 20221021 2TCM���@���ɔ������H������ ���� ---<<<
		}
		// 20200830 �≄�@�������u�����@�@�n�[�h�E�\�t�g�i2TCM�����X�V�j �a�� <<<-----
	} else if(0 == strcmp(KOUTEI_CODE_3TCM, pDeli->cLineCode)){
		PreLine = PRE_LINE_3TCM;		// 3TCM	
		cMyDbSection = "DB_3TCM";
	} else {
		// �ΏۊO�̑O�H���R�[�h
		return;
	}

	// �ڑ���DB������(�O�H���~�� or ����)
	bool bResult;
	if (PRE_LINE_2TCM == PreLine)	bResult = mcls_pSelect2TCM->SelectConnectDB("DB_2TCM", "DB_2TCM_SHARE", pDeli->cPreKizuKenNo, &clsPreDB);
	if (PRE_LINE_3TCM == PreLine)	bResult = mcls_pSelect3TCM->SelectConnectDB("DB_3TCM", "DB_3TCM_SHARE", pDeli->cPreKizuKenNo, &clsPreDB);
	// 20200830 �≄�@�������u�����@�@�n�[�h�E�\�t�g�i2TCM�����X�V�j �a�� ----->>>
	if (PRE_LINE_N2TCM_ANA == PreLine)	bResult = mcls_pSelectN2TCM_ANA->SelectConnectDB("DB_N2TCM_ANA", "DB_N2TCM_ANA_SHARE", pDeli->cPreKizuKenNo, &clsPreDB);
	// 20200830 �≄�@�������u�����@�@�n�[�h�E�\�t�g�i2TCM�����X�V�j �a�� <<<-----
	// 20221021 2TCM���@���ɔ������H������ ���� --->>>
	if (PRE_LINE_N2TCM_KIZU == PreLine)	bResult = mcls_pSelectN2TCM_KIZU->SelectConnectDB("DB_N2TCM_KIZU", "DB_N2TCM_KIZU_SHARE", pDeli->cPreKizuKenNo, &clsPreDB);
	// 20221021 2TCM���@���ɔ������H������ ���� ---<<<

	// �ڑ����ʂɂ��C�x���g���O�E�V�X���O���o��
	if (bResult){
		//--------------------
		// �ڑ�����OK
		//--------------------
		PreDBSelectManager::EM_SELECT_DB	nSelectDB;
		//int								AlertNo = GetAlertNo(PreDBSelectManager::PRE_DB,PreLine);

		// �ڑ���DB���擾
		if (PRE_LINE_2TCM == PreLine) nSelectDB = mcls_pSelect2TCM->GetSelectMode();
		if (PRE_LINE_3TCM == PreLine) nSelectDB = mcls_pSelect3TCM->GetSelectMode();
		// 20200830 �≄�@�������u�����@�@�n�[�h�E�\�t�g�i2TCM�����X�V�j �a�� ----->>>
		if (PRE_LINE_N2TCM_ANA == PreLine) nSelectDB = mcls_pSelectN2TCM_ANA->GetSelectMode();
		// 20200830 �≄�@�������u�����@�@�n�[�h�E�\�t�g�i2TCM�����X�V�j �a�� <<<-----
		// 20221021 2TCM���@���ɔ������H������ ���� --->>>
		if (PRE_LINE_N2TCM_KIZU == PreLine) nSelectDB = mcls_pSelectN2TCM_KIZU->GetSelectMode();
		// 20221021 2TCM���@���ɔ������H������ ���� ---<<<

		int AlertNo = GetAlertNo(nSelectDB, PreLine);

		// �O��Ɛڑ���DB�������ꍇ�́A�V�X���O�o�͖���
		if (PRE_LINE_2TCM == PreLine){
			if (m_emLastResult_2TCM == nSelectDB)	AlertNo = NON_ALERT;
		}
		if (PRE_LINE_3TCM == PreLine){
			if (m_emLastResult_3TCM == nSelectDB)	AlertNo = NON_ALERT;
		}
		// 20200830 �≄�@�������u�����@�@�n�[�h�E�\�t�g�i2TCM�����X�V�j �a�� ----->>>
		if (PRE_LINE_N2TCM_ANA == PreLine) {
			if (m_emLastResult_N2TCM_ANA == nSelectDB)	AlertNo = NON_ALERT;
		}
		// 20200830 �≄�@�������u�����@�@�n�[�h�E�\�t�g�i2TCM�����X�V�j �a�� <<<-----
		// 20221021 2TCM���@���ɔ������H������ ���� --->>>
		if (PRE_LINE_N2TCM_KIZU == PreLine) {
			if (m_emLastResult_N2TCM_KIZU == nSelectDB)	AlertNo = NON_ALERT;
		}
		// 20221021 2TCM���@���ɔ������H������ ���� ---<<<

		switch (nSelectDB){
		case PreDBSelectManager::PRE_DB:
			if (PRE_LINE_2TCM == PreLine) m_emLastResult_2TCM = PreDBSelectManager::PRE_DB;
			if (PRE_LINE_3TCM == PreLine) m_emLastResult_3TCM = PreDBSelectManager::PRE_DB;
			// 20200830 �≄�@�������u�����@�@�n�[�h�E�\�t�g�i2TCM�����X�V�j �a�� ----->>>
			if (PRE_LINE_N2TCM_ANA == PreLine) m_emLastResult_N2TCM_ANA = PreDBSelectManager::PRE_DB;
			// 20200830 �≄�@�������u�����@�@�n�[�h�E�\�t�g�i2TCM�����X�V�j �a�� <<<-----
			// 20221021 2TCM���@���ɔ������H������ ���� --->>>
			if (PRE_LINE_N2TCM_KIZU == PreLine) m_emLastResult_N2TCM_KIZU = PreDBSelectManager::PRE_DB;
			// 20221021 2TCM���@���ɔ������H������ ���� ---<<<

			if (NON_ALERT != AlertNo) syslog(AlertNo, "[�~��][�H�� = %s]", pDeli->cLineCode);
			LOG(em_MSG), "[%s] �O�H��DB�ڑ�����[TCM:�~��]", my_sThreadName);
			break;
		case PreDBSelectManager::SHARE_DB:
			if (PRE_LINE_2TCM == PreLine) m_emLastResult_2TCM = PreDBSelectManager::SHARE_DB;
			if (PRE_LINE_3TCM == PreLine) m_emLastResult_3TCM = PreDBSelectManager::SHARE_DB;
			// 20200830 �≄�@�������u�����@�@�n�[�h�E�\�t�g�i2TCM�����X�V�j �a�� ----->>>
			if (PRE_LINE_N2TCM_ANA == PreLine) m_emLastResult_N2TCM_ANA = PreDBSelectManager::SHARE_DB;
			// 20200830 �≄�@�������u�����@�@�n�[�h�E�\�t�g�i2TCM�����X�V�j �a�� <<<-----
			// 20221021 2TCM���@���ɔ������H������ ���� --->>>
			if (PRE_LINE_N2TCM_KIZU == PreLine) m_emLastResult_N2TCM_KIZU = PreDBSelectManager::SHARE_DB;
			// 20221021 2TCM���@���ɔ������H������ ���� ---<<<

			if (NON_ALERT != AlertNo) syslog(AlertNo, "[����][�H�� = %s]", pDeli->cLineCode);
			LOG(em_MSG), "[%s] �O�H��DB�ڑ�����[����][�H�� = %s]", my_sThreadName, pDeli->cLineCode);
			break;
		default:
			// ���肦�Ȃ�
			break;
		}
	} else{
		//--------------------
		// �ڑ�����NG
		//--------------------
		// ���nDB�̃V�X���O�o��
		int AlertNo = GetAlertNo(PreDBSelectManager::PRE_DB, PreLine);

		// �O��Ɛڑ���DB�������ꍇ�́A�V�X���O�o�͖���
		if (PRE_LINE_2TCM == PreLine){
			if (m_emLastResult_2TCM == PreDBSelectManager::NON_DB)	AlertNo = NON_ALERT;
		}
		if (PRE_LINE_3TCM == PreLine){
			if (m_emLastResult_3TCM == PreDBSelectManager::NON_DB)	AlertNo = NON_ALERT;
		}
		// 20200830 �≄�@�������u�����@�@�n�[�h�E�\�t�g�i2TCM�����X�V�j �a�� ----->>>
		if (PRE_LINE_N2TCM_ANA == PreLine) {
			if (m_emLastResult_N2TCM_ANA == PreDBSelectManager::NON_DB)	AlertNo = NON_ALERT;
		}
		// 20200830 �≄�@�������u�����@�@�n�[�h�E�\�t�g�i2TCM�����X�V�j �a�� <<<-----
		// 20221021 2TCM���@���ɔ������H������ ���� --->>>
		if (PRE_LINE_N2TCM_KIZU == PreLine) {
			if (m_emLastResult_N2TCM_KIZU == PreDBSelectManager::NON_DB)	AlertNo = NON_ALERT;
		}
		// 20221021 2TCM���@���ɔ������H������ ���� ---<<<
		if (NON_ALERT != AlertNo) syslog(AlertNo, "[�~��][�H�� = %s]", pDeli->cLineCode);		// �V�X���O�o��(�O�H���~��)


		// �����̃V�X���O�o��
		AlertNo = GetAlertNo(PreDBSelectManager::SHARE_DB, PreLine);

		// �O��Ɛڑ���DB�������ꍇ�́A�V�X���O�o�͖���
		if (PRE_LINE_2TCM == PreLine){
			if (m_emLastResult_2TCM == PreDBSelectManager::NON_DB)	AlertNo = NON_ALERT;
		}
		if (PRE_LINE_3TCM == PreLine){
			if (m_emLastResult_3TCM == PreDBSelectManager::NON_DB)	AlertNo = NON_ALERT;
		}
		// 20200830 �≄�@�������u�����@�@�n�[�h�E�\�t�g�i2TCM�����X�V�j �a�� ----->>>
		if (PRE_LINE_N2TCM_ANA == PreLine) {
			if (m_emLastResult_N2TCM_ANA == PreDBSelectManager::NON_DB)	AlertNo = NON_ALERT;
		}
		// 20200830 �≄�@�������u�����@�@�n�[�h�E�\�t�g�i2TCM�����X�V�j �a�� <<<-----
		// 20221021 2TCM���@���ɔ������H������ ���� --->>>
		if (PRE_LINE_N2TCM_KIZU == PreLine) {
			if (m_emLastResult_N2TCM_KIZU == PreDBSelectManager::NON_DB)	AlertNo = NON_ALERT;
		}
		// 20221021 2TCM���@���ɔ������H������ ���� ---<<<

		if (NON_ALERT != AlertNo) syslog(AlertNo, "[����][�H�� = %s]", pDeli->cLineCode);		// �V�X���O�o��(����)
		LOG(em_ERR), "[%s] �O�H��DB�ڑ��G���[ [%s]", my_sThreadName, cMyDbSection );

		if (PRE_LINE_2TCM == PreLine) m_emLastResult_2TCM = PreDBSelectManager::NON_DB;
		if (PRE_LINE_3TCM == PreLine) m_emLastResult_3TCM = PreDBSelectManager::NON_DB;
		// 20200830 �≄�@�������u�����@�@�n�[�h�E�\�t�g�i2TCM�����X�V�j �a�� ----->>>
		if (PRE_LINE_N2TCM_ANA == PreLine) m_emLastResult_N2TCM_ANA = PreDBSelectManager::NON_DB;
		// 20200830 �≄�@�������u�����@�@�n�[�h�E�\�t�g�i2TCM�����X�V�j �a�� <<<-----
		// 20221021 2TCM���@���ɔ������H������ ���� --->>>
		if (PRE_LINE_N2TCM_KIZU == PreLine) m_emLastResult_N2TCM_KIZU = PreDBSelectManager::NON_DB;
		// 20221021 2TCM���@���ɔ������H������ ���� ---<<<

		return;
	}

	// �O�H���r���A�摜���o�^
	if (!DBIns(PreLine, pDeli, &clsDB, &clsPreDB)) {
		LOG(em_ERR), "[%s] �O�H���r���A�r�摜�o�^���s [�r���Ǘ�No = %s] [�O�H���r���Ǘ�No = %s] [�\���敪 = %d]",
			my_sThreadName, pDeli->cKizuKenNo, pDeli->cPreKizuKenNo, pDeli->nOuDiv );
			syslog(PARAM_SYSLOG_BASENO + 1, "[�Ǘ�No = %s] [�O�H���Ǘ�No = %s] [�\���敪 = %d]", pDeli->cKizuKenNo, pDeli->cPreKizuKenNo, pDeli->nOuDiv); // �O�H���r���A�r�摜�o�^���s
			return;
	}

	// �O�H�����C���ʃR�C�����X�V�@�O�H���f�[�^�擾�ςݔF���Z�b�g
	UpdateCoilLine(&clsDB, pDeli);

	LOG(em_MSG), "[%s] �O�H���o�^�����I�� [�Ǘ�No = %s] [�O�H���Ǘ�No = %s] [�\���敪 = %d]",
		my_sThreadName, pDeli->cKizuKenNo, pDeli->cPreKizuKenNo, pDeli->nOuDiv );


	PRO_TYPE_COILINF& denbun = (PRO_TYPE_COILINF&)mtbl_pCoil->rec[0].val;	// �R�C�����`��
	// ���R�C�������ʂ̏ꍇ�͓����ɒʒm����
	if (0 == memcmp(pDeli->cMeiNo, denbun.data.CoilInf.KeyInf.cMeiNo, SIZE_MEI_NO) &&
		1 == pDeli->nOuDiv) {
		SendMailDefect(pDeli->cMeiNo, pDeli->cCoilNo);
	}

	// 2014.05.09 ���� �����T�[�o�[�Q�� <<<-----
}

//------------------------------------------
// �o�^�Ώۂ��`�F�b�N����
// OdbcBase* clspDB DB�o�^�C���X�^���X
// DeliveryPreDB* pDeli �L���[�f�[�^
// �߂�l ���A��� true:�f�[�^���� false:�f�[�^�Ȃ�
//------------------------------------------
bool PreDBManager::CheckInsert( OdbcBase* clspDB, DeliveryPreDB* pDeli )
{
	CString sql;					// SQL��
	CString wk;
	int		nCnt = 0;
	CString cDbName;
	CString cDbName2;
	cDbName = DB_MAE_COIL_INF;
	cDbName2 = DB_MAE_COIL_LINE;

	//// SQL���쐬(�Ǘ�No�擾)
	sql =  "SELECT";
	sql += " INF.�Ǘ�No ";
	sql += " FROM ";
	sql += cDbName;
	sql += " AS INF, ";
	sql += cDbName2;
	sql += " AS LINE ";
	sql += " WHERE ";
	sql += " INF.�Ǘ�No = LINE.�Ǘ�No ";
	sql += " AND INF.FF�O�H���r����M�t���O = 0 ";
	sql += " AND INF.���Ǘ�No�t���O = -1 ";
	sql += " AND LINE.�����X�e�[�^�X = -1 ";
	wk.Format( " AND INF.�Ǘ�No = '%s' AND LINE.�\���敪 = %d ", pDeli->cKizuKenNo, pDeli->nOuDiv );
	sql += wk;
	
	return clspDB->GetSelectKey( sql, &nCnt );
}

//------------------------------------------
// ���ҏW����
// EM_PRE_LINE PreLine �O�H�����C��
// DeliveryPreDB* pDeli �L���[�f�[�^
// OdbcBase* clsInsDB	�o�^DB�C���X�^���X
// OdbcBase* clsPreDB   �O�H��DB�C���X�^���X
// �߂�l ���A��� true:���� false:�ُ�
//------------------------------------------
bool PreDBManager::DBIns( EM_PRE_LINE PreLine, DeliveryPreDB* pDeli, OdbcBase* clsInsDB, OdbcBase* clsPreDB )
{

	CString sql;					// SQL��
	CString wk;
	int ii;
	MAE_DEFECT_IMAGE_INF  typData;	// �O�H�����
	CString cDbName;
	cDbName = DB_MAE_FF_INF;
	
	// ODBC�ڑ��N���X �C���X�^���X����(��DB)
	OdbcBase clsDB;
	if( ! clsDB.Connect( TASKINI_NAME ) ) {
		// �f�[�^�x�[�X�ڑ��G���[
		LOG(em_ERR), "[%s] DB�ڑ��G���[", my_sThreadName );
		syslog(SYSNO_DB_CONNECT_ERR, "");
		return false;
	}

	sql = "SELECT";
	sql += " �rNo, �H�����, �\���敪, �O�H���rNo ";
	sql += " FROM ";
	sql += cDbName;
	// �e�e�T�[�o����rNo���O�̃��R�[�h�͑O�H���f�[�^�擾�ΏۊO
	wk.Format( " WHERE �Ǘ�No = '%s' AND �O�H���Ǘ�No = '%s' AND �\���敪 = %d",
		pDeli->cKizuKenNo , pDeli->cPreKizuKenNo, pDeli->nOuDiv );
	sql += wk;
	sql += " AND �O�H���rNo >= 1 ";

	LOG(em_INF), "[%s] �O�H��FF�r���擾�Ώی��� [%s]", my_sThreadName, sql );
	//// ���s
	if( ! clsDB.ExecuteDirect( sql ) ) {
		LOG(em_NON), "%s", sql );
		LOG(em_ERR), "[%s] �O�H��FF�r�����s�G���[ [%s]", my_sThreadName, cDbName );
		syslog(SYSNO_DB_EXECUTEDIRECT_ERR, "[%s]", cDbName);
		return false;
	}
	
	//// �擾
	while (true) {

		int sqlrc = clsDB.Fetch();
		if( sqlrc == KIZUODBC_FETCH_OK ) {					// �f�[�^�L��
			
			// ������
			memset( &typData, 0x00, sizeof(typData) );

			memcpy( typData.cKizuKenNo, pDeli->cKizuKenNo, sizeof(typData.cKizuKenNo) );			// �Ǘ�No
			memcpy( typData.cPreKizuKenNo, pDeli->cPreKizuKenNo, sizeof(typData.cPreKizuKenNo) );	// �O�H���Ǘ�NO

			clsDB.GetData( 1, &typData.nKizuNo );													// �rNo
			clsDB.GetData( 2, &typData.nKouteiSyubetu );											// �H�����
			clsDB.GetData( 3, &typData.nOuDiv );													// �\���敪
			clsDB.GetData( 4, &typData.nMaeKizuNo );												// �O�H���rNo

			// 2014.05.09 ���� �����T�[�o�[�Q�� ------------>>>
			switch(PreLine){
				case PRE_LINE_2TCM:						//// �O�H�����擾
					if( ! GetNew2TCMInf( clsPreDB, &typData ) )	goto Ending;
					break;
				case PRE_LINE_3TCM:						//// �O�H�����擾
					if (!GetNew3TCMInf(clsPreDB, &typData))	goto Ending;
					break;
					// 20200830 �≄�@�������u�����@�@�n�[�h�E�\�t�g�i2TCM�����X�V�j �a�� ----->>>
				case PRE_LINE_N2TCM_ANA:				//// �O�H�����擾
					if (!GetNewN2TCM_ANAInf(clsPreDB, &typData))	goto Ending;
					break;
					// 20200830 �≄�@�������u�����@�@�n�[�h�E�\�t�g�i2TCM�����X�V�j �a�� <<<-----
				// 20221021 2TCM���@���ɔ������H������ ���� --->>>
				case PRE_LINE_N2TCM_KIZU:				//// �O�H�����擾
					if (!GetNewN2TCM_KIZUInf(clsPreDB, &typData))	goto Ending;
					break;
				// 20221021 2TCM���@���ɔ������H������ ���� ---<<<
				default:
					// ���肦�Ȃ�
					break;
			}
			// 2014.05.09 ���� �����T�[�o�[�Q�� <<<------------

            //// �O�H���r���e�[�u���X�V
			if( ! InsertDefectInf( clsInsDB, &typData ) ) {
				goto Ending;
			}

            //// �O�H���r�摜�e�[�u���o�^ ���r�摜�L�肩�`�F�b�N
			if( 0 != typData.nCnt ) {
				if( ! InsertDefectImg( clsInsDB, &typData ) ) {
					goto Ending;
				}
			}

			// �J��
			for( ii = 0; ii< MAX_MAE_IMAGE; ii++ ) {
				SAFE_DELETE_ARRAY( typData.pImg[ii] )
			}
			
		} else if( sqlrc == KIZUODBC_FETCH_NODATE ) {		// �f�[�^����
			break;

		} else {								// �G���[
			LOG(em_NON), "%s", sql );
			LOG(em_ERR), "[%s] �O�H��FF�r���擾�G���[ [%s]", my_sThreadName, cDbName );
			syslog(SYSNO_DB_FETCH_ERR, "[%s]", cDbName);
			return false;
		}
	}

	return true;

Ending: 
	
	// �J��
	for( ii = 0; ii < MAX_MAE_IMAGE; ii++ ) {
		SAFE_DELETE_ARRAY( typData.pImg[ii] )
	}
	return false;
}


//------------------------------------------
// 2TCM���擾
// OdbcBase* clsPreDB				�O�H��DB
// MAE_DEFECT_IMAGE_INF* typData	�O�H�����
// �߂�l ���A��� true:���� false:�ُ�
//------------------------------------------
bool PreDBManager::GetNew2TCMInf( OdbcBase* clsPreDB, MAE_DEFECT_IMAGE_INF* typData )
{
	
	CString sql;					// SQL��
	CString wk;
	int sqlrc;
	CString cDbName;
	cDbName  = DB_2TCM_DEFECT_INF_IN;

	LOG(em_MSG), "[%s] 2TCM���擾 [�Ǘ�No:%s] [�rNo=%d]", my_sThreadName, typData->cPreKizuKenNo, typData->nMaeKizuNo );

	// �O�H���r���擾
	sql = "SELECT ";
	sql += "DSWS�敪, ����ʒu, DS�ʒu, WS�ʒu,";
	sql += "���C�����x,�c����\,������\,�O��Xmin,�O��Xmax,�O��Ymin,�O��Ymax,";
	sql += "�r���S�ʒuX,�r���S�ʒuY,�c��f��,����f��,�摜�T�C�Y";
	sql += " FROM ";
	sql += cDbName;
	wk.Format( " WHERE �r���Ǘ�No= '%.20s' AND �rNo = %d ", typData->cPreKizuKenNo, typData->nMaeKizuNo );
	sql += wk;

	//// ���s
	if( ! clsPreDB->ExecuteDirect( sql ) ) {
		LOG(em_NON), "%s", sql );
		LOG(em_ERR), "[%s] 2TCM�r�����s�G���[ [%s]", my_sThreadName, cDbName);
		syslog(SYSNO_DB_EXECUTEDIRECT_ERR, "[%s]", cDbName);
		return false;
	}

	sqlrc = clsPreDB->Fetch();
	if( sqlrc == KIZUODBC_FETCH_OK ) {					// �f�[�^�L��

		clsPreDB->GetData(1, (int*)&typData->typDefectData.dsws);			// DSWS�敪		
		clsPreDB->GetData(2, &typData->typDefectData.y);					// ����ʒu
		clsPreDB->GetData(3, &typData->typDefectData.ds);					// DS�ʒu
		clsPreDB->GetData(4, &typData->typDefectData.ws);					// WS�ʒu
		clsPreDB->GetData(5, &typData->typDefectData.speed);				// ���C�����x
		clsPreDB->GetData(6, &typData->typDefectData.h_res);				// �c����\
		clsPreDB->GetData(7, &typData->typDefectData.w_res);				// ������\
		clsPreDB->GetData(8, (int*)&typData->typDefectData.box_x_min);		// �O��Xmin
		clsPreDB->GetData(9, (int*)&typData->typDefectData.box_x_max);		// �O��Xmax
		clsPreDB->GetData(10, (int*)&typData->typDefectData.box_y_min);		// �O��Ymin
		clsPreDB->GetData(11, (int*)&typData->typDefectData.box_y_max);		// �O��Ymax
		clsPreDB->GetData(12, (int*)&typData->typDefectData.center_x);		// �r���S�ʒuX
		clsPreDB->GetData(13, (int*)&typData->typDefectData.center_y);		// �r���S�ʒuY
		clsPreDB->GetData(14, (int*)&typData->typDefectData.h_size);		// �c��f��
		clsPreDB->GetData(15, (int*)&typData->typDefectData.w_size);		// ����f��
		clsPreDB->GetData(16, (int*)&typData->nSize[0]);					// �摜�T�C�Y

	} else if( sqlrc == KIZUODBC_FETCH_NODATE ) {		// �f�[�^����

	} else {								// �G���[
		LOG(em_NON), "%s", sql );
		LOG(em_ERR), "[%s] 2TCM�r���擾�G���[ [%s]", my_sThreadName, cDbName);
		syslog(SYSNO_DB_FETCH_ERR, "[%s]", cDbName);
		return false;
	}


	cDbName  = DB_2TCM_DEFECT_IMG_IN;


	// �O�H���r�摜�擾	
	sql = "SELECT �r�摜 ";
	sql += " FROM ";
	sql += cDbName;
	wk.Format(" WHERE �r���Ǘ�No= '%s' AND �rNo = %d ", typData->cPreKizuKenNo, typData->nMaeKizuNo );
	sql += wk;

	//// ���s
	if( ! clsPreDB->ExecuteDirect( sql ) ) {
		LOG(em_NON), "%s", sql );
		LOG(em_ERR), "[%s] 2TCM�r�摜���s�G���[ [%s]", my_sThreadName, cDbName);
		syslog(SYSNO_DB_EXECUTEDIRECT_ERR, "[%s]", cDbName);
		return false;
	}

	sqlrc = clsPreDB->Fetch();
	if( sqlrc == KIZUODBC_FETCH_OK ) {					// �f�[�^�L��
		
		// �摜���쐬 2014.05.27 J.Wakazono �������O�f�[�^�擾���s�Ή�
		for(int ii=0; ii<MAX_MAE_IMAGE; ii++) {
			if(0 < typData->nSize[ii]) {
				typData->pImg[ii] = new BYTE[ typData->nSize[ii] ];
				typData->nCnt++;
			} else {
				break;
			}
		}

		clsPreDB->GetDataImage( 1, typData->pImg[0], typData->nSize[0] );	// �r�摜
		
	} else if( sqlrc == KIZUODBC_FETCH_NODATE ) {		// �f�[�^����
		
		// �摜���쐬
		LOG(em_INF), "[%s] �摜���� %s", my_sThreadName, sql );
		typData->nCnt = 0;	// �摜�f�[�^�� 0

	} else {								// �G���[
		LOG(em_NON), "%s", sql );
		LOG(em_ERR), "[%s] 2TCM�r�摜�擾�G���[ [%s]", my_sThreadName, cDbName);
		syslog(SYSNO_DB_FETCH_ERR, "[%s]", cDbName);
		return false;
	}

	return true;
}

//------------------------------------------
// 3TCM���擾
// OdbcBase* clsPreDB				�O�H��DB
// MAE_DEFECT_IMAGE_INF* typData	�O�H�����
// �߂�l ���A��� true:���� false:�ُ�
//------------------------------------------
bool PreDBManager::GetNew3TCMInf(OdbcBase* clsPreDB, MAE_DEFECT_IMAGE_INF* typData)
{

	CString sql;					// SQL��
	CString wk;
	int sqlrc;
	CString cDbName;
	cDbName = DB_3TCM_DEFECT_INF_IN;

	LOG(em_MSG), "[%s] 3TCM���擾 [�Ǘ�No:%s] [�rNo=%d]", my_sThreadName, typData->cPreKizuKenNo, typData->nMaeKizuNo );

	// �O�H���r���擾
	sql = "SELECT ";
	sql += "DSWS�敪, ����ʒu, DS�ʒu, WS�ʒu,";
	sql += "���C�����x,�c����\,������\,�O��Xmin,�O��Xmax,�O��Ymin,�O��Ymax,";
	sql += "�r���S�ʒuX,�r���S�ʒuY,�c��f��,����f��,�摜�T�C�Y";
	sql += " FROM ";
	sql += cDbName;
	wk.Format(" WHERE �r���Ǘ�No= '%.20s' AND �rNo = %d ", typData->cPreKizuKenNo, typData->nMaeKizuNo);
	sql += wk;

	//// ���s
	if (!clsPreDB->ExecuteDirect(sql)) {
		LOG(em_NON), "%s", sql );
		LOG(em_ERR), "[%s] 3TCM�r�����s�G���[ [%s]", my_sThreadName, cDbName);
		syslog(SYSNO_DB_EXECUTEDIRECT_ERR, "[%s]", cDbName);
		return false;
	}

	sqlrc = clsPreDB->Fetch();
	if (sqlrc == KIZUODBC_FETCH_OK) {					// �f�[�^�L��

		clsPreDB->GetData(1, (int*)&typData->typDefectData.dsws);			// DSWS�敪		
		clsPreDB->GetData(2, &typData->typDefectData.y);					// ����ʒu
		clsPreDB->GetData(3, &typData->typDefectData.ds);					// DS�ʒu
		clsPreDB->GetData(4, &typData->typDefectData.ws);					// WS�ʒu
		clsPreDB->GetData(5, &typData->typDefectData.speed);				// ���C�����x
		clsPreDB->GetData(6, &typData->typDefectData.h_res);				// �c����\
		clsPreDB->GetData(7, &typData->typDefectData.w_res);				// ������\
		clsPreDB->GetData(8, (int*)&typData->typDefectData.box_x_min);		// �O��Xmin
		clsPreDB->GetData(9, (int*)&typData->typDefectData.box_x_max);		// �O��Xmax
		clsPreDB->GetData(10, (int*)&typData->typDefectData.box_y_min);		// �O��Ymin
		clsPreDB->GetData(11, (int*)&typData->typDefectData.box_y_max);		// �O��Ymax
		clsPreDB->GetData(12, (int*)&typData->typDefectData.center_x);		// �r���S�ʒuX
		clsPreDB->GetData(13, (int*)&typData->typDefectData.center_y);		// �r���S�ʒuY
		clsPreDB->GetData(14, (int*)&typData->typDefectData.h_size);		// �c��f��
		clsPreDB->GetData(15, (int*)&typData->typDefectData.w_size);		// ����f��
		clsPreDB->GetData(16, (int*)&typData->nSize[0]);					// �摜�T�C�Y

	} else if (sqlrc == KIZUODBC_FETCH_NODATE) {		// �f�[�^����

	} else {								// �G���[
		LOG(em_NON), "%s", sql );
		LOG(em_ERR), "[%s] 3TCM�r���擾�G���[ [%s]", my_sThreadName, cDbName);
		syslog(SYSNO_DB_FETCH_ERR, "[%s]", cDbName);
		return false;
	}


	cDbName = DB_3TCM_DEFECT_IMG_IN;


	// �O�H���r�摜�擾	
	sql = "SELECT";
	sql += " �r�摜 ";
	sql += " FROM ";
	sql += cDbName;
	wk.Format(" WHERE �r���Ǘ�No= '%s' AND �rNo = %d ", typData->cPreKizuKenNo, typData->nMaeKizuNo);
	sql += wk;

	//// ���s
	if (!clsPreDB->ExecuteDirect(sql)) {
		LOG(em_NON), "%s", sql );
		LOG(em_ERR), "[%s] 3TCM�r�摜���s�G���[ [%s]", my_sThreadName, cDbName);
		syslog(SYSNO_DB_EXECUTEDIRECT_ERR, "[%s]", cDbName);
		return false;
	}

	sqlrc = clsPreDB->Fetch();
	if (sqlrc == KIZUODBC_FETCH_OK) {					// �f�[�^�L��

		// �摜���쐬 2014.05.27 J.Wakazono �������O�f�[�^�擾���s�Ή�
		for (int ii = 0; ii<MAX_MAE_IMAGE; ii++) {
			if (0 < typData->nSize[ii]) {
				typData->pImg[ii] = new BYTE[typData->nSize[ii]];
				typData->nCnt++;
			} else {
				break;
			}
		}

		clsPreDB->GetDataImage(1, typData->pImg[0], typData->nSize[0]);	// �r�摜

	} else if (sqlrc == KIZUODBC_FETCH_NODATE) {		// �f�[�^����

														// �摜���쐬
		typData->nCnt = 0;	// �摜�f�[�^�� 0

	} else {								// �G���[
		LOG(em_NON), "%s", sql );
		LOG(em_ERR), "[%s] 3TCM�r�摜�擾�G���[ [%s]", my_sThreadName, cDbName);
		syslog(SYSNO_DB_FETCH_ERR, "[%s]", cDbName);
		return false;
	}

	return true;
}

// 20200830 �≄�@�������u�����@�@�n�[�h�E�\�t�g�i2TCM�����X�V�j �a�� ----->>>
//------------------------------------------
// N2TCM_ANA���擾
// OdbcBase* clsPreDB				�O�H��DB
// MAE_DEFECT_IMAGE_INF* typData	�O�H�����
// �߂�l ���A��� true:���� false:�ُ�
//------------------------------------------
bool PreDBManager::GetNewN2TCM_ANAInf(OdbcBase* clsPreDB, MAE_DEFECT_IMAGE_INF* typData)
{

	CString sql;					// SQL��
	CString wk;
	int sqlrc;
	CString cDbName;
	cDbName = DB_N2TCM_ANA_DEFECT_INF;

	LOG(em_MSG), "[%s] N2TCM_ANA���擾 [�Ǘ�No:%s] [�rNo=%d]", my_sThreadName, typData->cPreKizuKenNo, typData->nMaeKizuNo );

	// �O�H���r���擾
	sql = "SELECT ";
	sql += "DSWS�敪, ����ʒu, DS�ʒu, WS�ʒu,";
	sql += "���C�����x,�c����\,������\,�O��Xmin,�O��Xmax,�O��Ymin,�O��Ymax,";
	sql += "�r���S�ʒuX,�r���S�ʒuY,�c��f��,����f��,�摜�T�C�Y��";
	sql += " FROM ";
	sql += cDbName;
	wk.Format(" WHERE �Ǘ�No= '%.20s' AND �rNo = %d ", typData->cPreKizuKenNo, typData->nMaeKizuNo);
	sql += wk;

	//// ���s
	if (!clsPreDB->ExecuteDirect(sql)) {
		LOG(em_NON), "%s", sql );
		syslog(SYSNO_DB_EXECUTEDIRECT_ERR, "[%s]", cDbName);
		return false;
	}

	sqlrc = clsPreDB->Fetch();
	if (sqlrc == KIZUODBC_FETCH_OK) {					// �f�[�^�L��

		clsPreDB->GetData(1, (int*)&typData->typDefectData.dsws);			// DSWS�敪		
		clsPreDB->GetData(2, &typData->typDefectData.y);					// ����ʒu
		clsPreDB->GetData(3, &typData->typDefectData.ds);					// DS�ʒu
		clsPreDB->GetData(4, &typData->typDefectData.ws);					// WS�ʒu
		clsPreDB->GetData(5, &typData->typDefectData.speed);				// ���C�����x
		clsPreDB->GetData(6, &typData->typDefectData.h_res);				// �c����\
		clsPreDB->GetData(7, &typData->typDefectData.w_res);				// ������\
		clsPreDB->GetData(8, (int*)&typData->typDefectData.box_x_min);		// �O��Xmin
		clsPreDB->GetData(9, (int*)&typData->typDefectData.box_x_max);		// �O��Xmax
		clsPreDB->GetData(10, (int*)&typData->typDefectData.box_y_min);		// �O��Ymin
		clsPreDB->GetData(11, (int*)&typData->typDefectData.box_y_max);		// �O��Ymax
		clsPreDB->GetData(12, (int*)&typData->typDefectData.center_x);		// �r���S�ʒuX
		clsPreDB->GetData(13, (int*)&typData->typDefectData.center_y);		// �r���S�ʒuY
		clsPreDB->GetData(14, (int*)&typData->typDefectData.h_size);		// �c��f��
		clsPreDB->GetData(15, (int*)&typData->typDefectData.w_size);		// ����f��
		clsPreDB->GetData(16, (int*)&typData->nSize[0]);					// �摜�T�C�Y

	}
	else if (sqlrc == KIZUODBC_FETCH_NODATE) {		// �f�[�^����

	}
	else {								// �G���[
		syslog(SYSNO_DB_FETCH_ERR, "[%s]", cDbName);
		return false;
	}


	cDbName = DB_N2TCM_ANA_DEFECT_IMG;


	// �O�H���r�摜�擾	
	sql = "SELECT ���摜 ";
	sql += " FROM ";
	sql += cDbName;
	wk.Format(" WHERE �Ǘ�No= '%s' AND �rNo = %d ", typData->cPreKizuKenNo, typData->nMaeKizuNo);
	sql += wk;

	//// ���s
	if (!clsPreDB->ExecuteDirect(sql)) {
		LOG(em_NON), "%s", sql );
		syslog(SYSNO_DB_EXECUTEDIRECT_ERR, "[%s]", cDbName);
		return false;
	}

	sqlrc = clsPreDB->Fetch();
	if (sqlrc == KIZUODBC_FETCH_OK) {					// �f�[�^�L��

														// �摜���쐬 2014.05.27 J.Wakazono �������O�f�[�^�擾���s�Ή�
		for (int ii = 0; ii<MAX_MAE_IMAGE; ii++) {
			if (0 < typData->nSize[ii]) {
				typData->pImg[ii] = new BYTE[typData->nSize[ii]];
				typData->nCnt++;
			}
			else {
				break;
			}
		}

		clsPreDB->GetDataImage(1, typData->pImg[0], typData->nSize[0]);	// �r�摜

	}
	else if (sqlrc == KIZUODBC_FETCH_NODATE) {		// �f�[�^����

													// �摜���쐬
		typData->nCnt = 0;	// �摜�f�[�^�� 0

	}
	else {								// �G���[
		syslog(SYSNO_DB_FETCH_ERR, "[%s]", cDbName);
		return false;
	}

	return true;
}
// 20200830 �≄�@�������u�����@�@�n�[�h�E�\�t�g�i2TCM�����X�V�j �a�� <<<-----

// 20221021 2TCM���@���ɔ������H������ ���� --->>>
//------------------------------------------
// N2TCM_KIZU���擾
// OdbcBase* clsPreDB				�O�H��DB
// MAE_DEFECT_IMAGE_INF* typData	�O�H�����
// �߂�l ���A��� true:���� false:�ُ�
//------------------------------------------
bool PreDBManager::GetNewN2TCM_KIZUInf(OdbcBase* clsPreDB, MAE_DEFECT_IMAGE_INF* typData)
{

	CString sql;					// SQL��
	CString wk;
	int sqlrc;
	CString cDbName;
	cDbName = DB_N2TCM_KIZU_DEFECT_INF;

	LOG(em_MSG), "[%s] N2TCM_KIZU���擾 [�Ǘ�No:%s] [�rNo=%d]", my_sThreadName, typData->cPreKizuKenNo, typData->nMaeKizuNo );

	// �O�H���r���擾
	sql = "SELECT ";
	sql += "DSWS�敪, ����ʒu, DS�ʒu, WS�ʒu,";
	sql += "���C�����x,�c����\,������\,�O��Xmin,�O��Xmax,�O��Ymin,�O��Ymax,";
	sql += "�r���S�ʒuX,�r���S�ʒuY,�c��f��,����f��,�摜�T�C�Y��";
	sql += " FROM ";
	sql += cDbName;
	wk.Format(" WHERE �Ǘ�No= '%.20s' AND �rNo = %d ", typData->cPreKizuKenNo, typData->nMaeKizuNo);
	sql += wk;

	//// ���s
	if (!clsPreDB->ExecuteDirect(sql)) {
		LOG(em_NON), "%s", sql );
		syslog(SYSNO_DB_EXECUTEDIRECT_ERR, "[%s]", cDbName);
		return false;
	}

	sqlrc = clsPreDB->Fetch();
	if (sqlrc == KIZUODBC_FETCH_OK) {					// �f�[�^�L��

		clsPreDB->GetData(1, (int*)&typData->typDefectData.dsws);			// DSWS�敪		
		clsPreDB->GetData(2, &typData->typDefectData.y);					// ����ʒu
		clsPreDB->GetData(3, &typData->typDefectData.ds);					// DS�ʒu
		clsPreDB->GetData(4, &typData->typDefectData.ws);					// WS�ʒu
		clsPreDB->GetData(5, &typData->typDefectData.speed);				// ���C�����x
		clsPreDB->GetData(6, &typData->typDefectData.h_res);				// �c����\
		clsPreDB->GetData(7, &typData->typDefectData.w_res);				// ������\
		clsPreDB->GetData(8, (int*)&typData->typDefectData.box_x_min);		// �O��Xmin
		clsPreDB->GetData(9, (int*)&typData->typDefectData.box_x_max);		// �O��Xmax
		clsPreDB->GetData(10, (int*)&typData->typDefectData.box_y_min);		// �O��Ymin
		clsPreDB->GetData(11, (int*)&typData->typDefectData.box_y_max);		// �O��Ymax
		clsPreDB->GetData(12, (int*)&typData->typDefectData.center_x);		// �r���S�ʒuX
		clsPreDB->GetData(13, (int*)&typData->typDefectData.center_y);		// �r���S�ʒuY
		clsPreDB->GetData(14, (int*)&typData->typDefectData.h_size);		// �c��f��
		clsPreDB->GetData(15, (int*)&typData->typDefectData.w_size);		// ����f��
		clsPreDB->GetData(16, (int*)&typData->nSize[0]);					// �摜�T�C�Y

	}
	else if (sqlrc == KIZUODBC_FETCH_NODATE) {		// �f�[�^����

	}
	else {								// �G���[
		syslog(SYSNO_DB_FETCH_ERR, "[%s]", cDbName);
		return false;
	}


	cDbName = DB_N2TCM_KIZU_DEFECT_IMG;


	// �O�H���r�摜�擾	
	sql = "SELECT ���摜 ";
	sql += " FROM ";
	sql += cDbName;
	wk.Format(" WHERE �Ǘ�No= '%s' AND �rNo = %d ", typData->cPreKizuKenNo, typData->nMaeKizuNo);
	sql += wk;

	//// ���s
	if (!clsPreDB->ExecuteDirect(sql)) {
		LOG(em_NON), "%s", sql );
		syslog(SYSNO_DB_EXECUTEDIRECT_ERR, "[%s]", cDbName);
		return false;
	}

	sqlrc = clsPreDB->Fetch();
	if (sqlrc == KIZUODBC_FETCH_OK) {					// �f�[�^�L��

														// �摜���쐬 2014.05.27 J.Wakazono �������O�f�[�^�擾���s�Ή�
		for (int ii = 0; ii<MAX_MAE_IMAGE; ii++) {
			if (0 < typData->nSize[ii]) {
				typData->pImg[ii] = new BYTE[typData->nSize[ii]];
				typData->nCnt++;
			}
			else {
				break;
			}
		}

		clsPreDB->GetDataImage(1, typData->pImg[0], typData->nSize[0]);	// �r�摜

	}
	else if (sqlrc == KIZUODBC_FETCH_NODATE) {		// �f�[�^����

													// �摜���쐬
		typData->nCnt = 0;	// �摜�f�[�^�� 0

	}
	else {								// �G���[
		syslog(SYSNO_DB_FETCH_ERR, "[%s]", cDbName);
		return false;
	}

	return true;
}
// 20221021 2TCM���@���ɔ������H������ ���� ---<<<

//------------------------------------------
// �O�H���r���o�^
// OdbcBase* �@�@�@�@clsDB   DB�C���X�^���X
// MAE_DEFECT_INF* �@typData �O�H�����
//------------------------------------------
bool PreDBManager::InsertDefectInf( OdbcBase* clsDB, MAE_DEFECT_IMAGE_INF* typData )
{
	CString sql;					// SQL��
	CString cDbName;
	cDbName = DB_MAE_DEFECT_INF;


	////// �O�H���r���
	// �w�b�_�[��
	sql = "INSERT INTO ";
	sql += cDbName;
	sql += " VALUES(";	

	// �f�[�^��
	CreateSQL::sqlColumnInsert( &sql, typData->cKizuKenNo, SIZE_KIZUKEN_NO );	// �Ǘ�No
	CreateSQL::sqlColumnInsert( &sql, typData->nKizuNo );						// �rNo
	CreateSQL::sqlColumnInsert( &sql, typData->nKouteiSyubetu );				// �H�����
	CreateSQL::sqlColumnInsert( &sql, typData->nOuDiv );						// �\���敪
	CreateSQL::sqlColumnInsert( &sql, typData->typDefectData.cycle_div );		// �����t���O
	CreateSQL::sqlColumnInsert( &sql, typData->typDefectData.y );				// ����ʒu
	CreateSQL::sqlColumnInsert( &sql, typData->typDefectData.dsws );			// DSWS�敪
	CreateSQL::sqlColumnInsert( &sql, typData->typDefectData.ds );				// DS�ʒu
	CreateSQL::sqlColumnInsert( &sql, typData->typDefectData.ws );				// WS�ʒu
	CreateSQL::sqlColumnInsert( &sql, typData->typDefectData.speed );			// ���C�����x
	CreateSQL::sqlColumnInsert( &sql, typData->typDefectData.pitch );			// �s�b�`
	CreateSQL::sqlColumnInsert( &sql, typData->typDefectData.roolkei );			// �a
	CreateSQL::sqlColumnInsert( &sql, typData->typDefectData.kizu_wid );		// �r��
	CreateSQL::sqlColumnInsert( &sql, typData->typDefectData.kizu_len );		// �r����
	CreateSQL::sqlColumnInsert( &sql, typData->typDefectData.h_res );			// �c����\
	CreateSQL::sqlColumnInsert( &sql, typData->typDefectData.w_res );			// ������\
	CreateSQL::sqlColumnInsert( &sql, typData->typDefectData.box_x_min );		// �O��Xmin
	CreateSQL::sqlColumnInsert( &sql, typData->typDefectData.box_x_max );		// �O��Xmax
	CreateSQL::sqlColumnInsert( &sql, typData->typDefectData.box_y_min );		// �O��Ymin
	CreateSQL::sqlColumnInsert( &sql, typData->typDefectData.box_y_max );		// �O��Ymax
	CreateSQL::sqlColumnInsert( &sql, typData->typDefectData.center_x );		// �r���S�ʒuX
	CreateSQL::sqlColumnInsert( &sql, typData->typDefectData.center_y );		// �r���S�ʒuY
	CreateSQL::sqlColumnInsert( &sql, typData->typDefectData.h_size );			// �c��f��
	CreateSQL::sqlColumnInsert( &sql, typData->typDefectData.w_size );			// ����f��
	CreateSQL::sqlColumnInsert( &sql, (int)typData->nSize[0] );					// �摜�T�C�Y1
	CreateSQL::sqlColumnInsert( &sql, (int)typData->nSize[1] );					// �摜�T�C�Y2
	CreateSQL::sqlColumnInsert( &sql, (int)typData->nSize[2] );					// �摜�T�C�Y3
	CreateSQL::sqlColumnInsert( &sql, (int)typData->nSize[3] );					// �摜�T�C�Y4
	CreateSQL::sqlColumnTrim( &sql );									// �Ō�̃J���}����菜��
	sql += ")";

	// ���s
	if( ! clsDB->ExecuteDirect( sql ) ) {
		LOG(em_NON), "%s", sql );
		LOG(em_ERR), "[%s] �r���o�^�G���[ [%s]", my_sThreadName, cDbName );
		syslog(SYSNO_DB_EXECUTEDIRECT_ERR, "[%s]", cDbName);
		return false;
	}
	
	return true;

}

//------------------------------------------
// �O�H���r�摜�o�^
// OdbcBase* �@�@�@�@clsDB   DB�C���X�^���X
// MAE_DEFECT_INF* �@typData �O�H�����
//------------------------------------------
bool PreDBManager::InsertDefectImg( OdbcBase* clsDB, MAE_DEFECT_IMAGE_INF* typData )	
{
	
	CString sql;					// SQL��
	CString cDbName;
	cDbName = DB_MAE_DEFECT_IMG;

	////// �O�H���r���
	// �w�b�_�[��
	sql = "INSERT INTO ";
	sql += cDbName;
	sql += " VALUES(";	

	// �f�[�^��
	CreateSQL::sqlColumnInsert( &sql, typData->cKizuKenNo, SIZE_KIZUKEN_NO );	// �Ǘ�No 
	CreateSQL::sqlColumnInsert( &sql, typData->nKizuNo );						// �rNo
	for( int ii = 0; ii< MAX_MAE_IMAGE; ii++ ) {
		// 2014.05.27 J.Wakazono �������O�f�[�^�擾���s�Ή�
		if( 0 < typData->nSize[ii] ) {
			sql += " ?,";
		} else {
			sql += " NULL,";
		}
	}
	CreateSQL::sqlColumnTrim( &sql );									// �Ō�̃J���}����菜��
	sql += ")";

	// ���s
	if( ! clsDB->SqlBinalyWrite( sql, typData->nCnt, typData->nSize, (LPVOID*)typData->pImg ) ) {
		LOG(em_NON), "%s", sql );
		LOG(em_ERR), "[%s] �摜�o�^���s", my_sThreadName );
		LOG(em_ERR), "[%s] �摜1 �T�C�Y [%d]", my_sThreadName, typData->nSize[0] );
		LOG(em_ERR), "[%s] �摜2 �T�C�Y [%d]", my_sThreadName, typData->nSize[1] );
		LOG(em_ERR), "[%s] �摜3 �T�C�Y [%d]", my_sThreadName, typData->nSize[2] );
		LOG(em_ERR), "[%s] �摜4 �T�C�Y [%d]", my_sThreadName, typData->nSize[3] );
		syslog(SYSNO_DB_EXECUTEDIRECT_ERR, "[%s]", cDbName);
		return false;
	}
	
	return true;

}

//------------------------------------------
// �O�H���r���X�V
// OdbcBase* clspDB		DB�C���X�^���X
// DeliveryPreDB* pDeli	�L���[�f�[�^
//------------------------------------------
void PreDBManager::UpdateCoilLine( OdbcBase* clspDB, DeliveryPreDB* pDeli )
{
	CString sql;					// SQL��
	CString wk;
	CString cDbName;
	cDbName = DB_MAE_COIL_LINE;

	////// �O�H�����C���ʃR�C�����̏����X�e�[�^�X��0�ɍX�V
	//// SQL���쐬
	sql = "UPDATE ";
	sql += cDbName;
	sql += " SET ";
	sql += " �����X�e�[�^�X = 0 ";
	wk.Format(" WHERE �Ǘ�No = '%s' ", pDeli->cKizuKenNo );
	sql += wk;
	wk.Format(" AND �O�H���Ǘ�No = '%s' ", pDeli->cPreKizuKenNo );
	sql += wk;
	wk.Format(" AND �\���敪 = %d ", pDeli->nOuDiv );
	sql += wk;
		
	// ���s
	if( ! clspDB->ExecuteDirect( sql ) ) {
		LOG(em_NON), "%s", sql );
		LOG(em_ERR), "[%s] �O�H�����C���ʃR�C�����UPDATE�G���[ [%s]", my_sThreadName, cDbName );
		syslog(SYSNO_DB_EXECUTEDIRECT_ERR, "[%s]", cDbName);
	}

}

// 2014.07.25 �쌴  ���ԘA�g    >>>>>>>>
//------------------------------------------
// �����ށA�����ނ̔���
// Char* LineCode		�O�H���R�[�h
// �߂�l:
//------------------------------------------
int PreDBManager::CheckAnotherMill(char* LineCode)
{
    int nRet;

    // �����ޔ���擾
    nRet    = mcls_pParam->GetTasyoZai(LineCode);

    return nRet;
}
// 2014.07.25 �쌴  ���ԘA�g    >>>>>>>>

//--------------------------------------------
// �ڑ����ʂ���A���[��No������
// EM_SELECT_DB nSelect		�O�H���~�� or ����
// �߂�l: �A���[��No
//--------------------------------------------
int PreDBManager::GetAlertNo(PreDBSelectManager::EM_SELECT_DB nSelect,EM_PRE_LINE nPreLine)
{
	PreDBSelectManager::TYP_SELECT_STATUS typStatus;

    CString	strWk;		// �C�x���g���O�o�͗p
	switch(nSelect){
		case PreDBSelectManager::PRE_DB:	strWk = "�~��";	break;
		case PreDBSelectManager::SHARE_DB:	strWk = "����";	break;
	}

    // �I���T�[�o�[�̐ڑ��`�F�b�N���ʂ��擾
	switch(nPreLine){
		case PRE_LINE_2TCM:
			mcls_pSelect2TCM->GetSelectStatus(nSelect,&typStatus);
			break;
		case PRE_LINE_3TCM:
			mcls_pSelect3TCM->GetSelectStatus(nSelect, &typStatus);
			break;
			// 20200830 �≄�@�������u�����@�@�n�[�h�E�\�t�g�i2TCM�����X�V�j �a�� ----->>>
		case PRE_LINE_N2TCM_ANA:
			mcls_pSelectN2TCM_ANA->GetSelectStatus(nSelect, &typStatus);
			break;
			// 20200830 �≄�@�������u�����@�@�n�[�h�E�\�t�g�i2TCM�����X�V�j �a�� <<<-----
		// 20221021 2TCM���@���ɔ������H������ ���� --->>>
		case PRE_LINE_N2TCM_KIZU:
			mcls_pSelectN2TCM_KIZU->GetSelectStatus(nSelect, &typStatus);
			break;
		// 20221021 2TCM���@���ɔ������H������ ���� ---<<<

	}

    // �`�F�b�N���ʂ���A���[��No������
	if(PreDBSelectManager::SELECT_NG == typStatus.nPing){
		LOG(em_ERR), "[%s] Ping�����G���[ [%s]", my_sThreadName, strWk);	
		return ALERT_NG_PING;
	}else if(PreDBSelectManager::SELECT_NG == typStatus.nConnect){
		LOG(em_ERR), "[%s] ODBC�ڑ��G���[ [%s]", my_sThreadName, strWk);		
		return ALERT_NG_CONNECT;
	}else if(PreDBSelectManager::SELECT_NG == typStatus.nSearchCoil){
		LOG(em_ERR), "[%s] �R�C�����тȂ� [%s]", my_sThreadName, strWk);		
		return ALERT_NG_SEARCH;
	}
	LOG(em_MSG), "[%s] �ڑ����� [%s]", my_sThreadName, strWk);	
	return ALERT_OK;
}

//------------------------------------------
// �O�H���r��񏑍��݊����ʒm���M
// char* cMeiNo		����No
// char* cCoilNo	�R�C��No
//------------------------------------------
void PreDBManager::SendMailDefect(char* cMeiNo, char* cCoilNo)
{
	char cMeiNoBuff[SIZE_MEI_NO + 1];
	char cCoilNoBuff[SIZE_COIL_NO + 1];
	COMMON_QUE		que;
	memset(&que, 0x00, sizeof(que));

	memset(&cMeiNoBuff, 0x00, sizeof(cMeiNoBuff));
	strcpy(cMeiNoBuff, cMeiNo);
	KizuFunction::Trim(cMeiNoBuff);

	memset(&cCoilNoBuff, 0x00, sizeof(cCoilNoBuff));
	strcpy(cCoilNoBuff, cCoilNo);
	KizuFunction::Trim(cCoilNoBuff);

	que.nLineNo = getlineid();

	//// �����֒ʒm
	que.nEventNo = FACT_TO_DEFECT_12;						// �v�����[�h
	sprintf(que.mix.cdata[0], "%04s", cMeiNoBuff);			// ����No
	sprintf(que.mix.cdata[1], "%.12s", cCoilNoBuff);		// �R�C��No


	send_mail(TO_DEFECT, ".", &que);

	LOG(em_MSG), "[%s] �O�H���r��񏑍��݊����ʒm���M [����No = %s,�R�C��No = %s]", my_sThreadName, cMeiNoBuff, cCoilNoBuff );

}