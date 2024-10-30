#include "StdAfx.h"
#include "PreProcManager.h"

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
PreProcManager::PreProcManager(void) :
ThreadManager("PreProcManager")
{

	//// ���L������
	// �R�C�����e�[�u��
	mem_connect( &m_hCoil, (LPVOID *)&mtbl_pCoil, TBL_TO_COIL_NAME );
	if( m_hCoil == NULL ) {
		syslog(SYSNO_MEM_OPEN, "�R�C�����e�[�u��[%s] �A�N�Z�X���s", TBL_TO_COIL_NAME);
		_ASSERT( FALSE );
	}

}

//------------------------------------------
// �f�X�g���N�^
//------------------------------------------
PreProcManager::~PreProcManager(void)
{

	//// ���L�������J��
	mem_close( &m_hCoil, (LPVOID *)&mtbl_pCoil );							// �R�C�����
}

//------------------------------------------
// �֐�������
//------------------------------------------
void PreProcManager::Alloc()
{
	while( true ) {
		if( mcls_pParam->ReadParam_All() ) break;

		LOG( em_MSG), "[%s] �p�����[�^�Ǎ����s(DB�ڑ��ُ�H�I)�B���g���C���E�E�E", my_sThreadName );
		Sleep( 10000 );			
	}
}

//------------------------------------------
// �X���b�h�J�n�O���� (�X���b�h���s����(�V�O�i���Z�b�g�O)�ɃR�[���o�b�N)
//------------------------------------------
int PreProcManager::ThreadFirst()
{

	//// �C�x���g�ݒ�
	HANDLE	hArray[] = {gque_Deli.g_evSem
						};
	SetEventNum( sizeof(hArray)/sizeof(HANDLE), hArray );

	return 0;
}

//------------------------------------------
// �X���b�h�C�x���g����
// int nEventNo �C�x���gNo (0�I���W��)
//------------------------------------------
void PreProcManager::ThreadEvent( int nEventNo )
{
	COMMON_QUE* pDeli;				// �X���b�h�L���[�󂯓n��

	////// �V�O�i���̏���
	enum {	EV_QUE = 0,						// ���[���v��(�R�C������M�ʒm,FF�O�H���r����M�ʒm)
			};

	////// �V�O�i����������
	switch ( nEventNo ) {
//-----------------------------------------------------------------------------------------------
	case EV_QUE: 						// ���[���X���b�g �ʒm
		pDeli = gque_Deli.GetFromHead();

		switch( pDeli->nEventNo ) {
//-----------------------------------------------------------
		case FACT_TO_MAEDAT_02:			// �R�C������M�ʒm

			// �R�C���ʔ��ʃe�[�u���o�^
			// �O�H���R�C�����o�^
			if( ! MakeMaeCoilInf( pDeli ) ) {
				LOG(em_ERR), "[%s] �R�C�����o�^���s [�Ǘ�No=%s]", my_sThreadName, pDeli->mix.cdata[0] );
				syslog(PARAM_SYSLOG_BASENO + 1, "[�Ǘ�No=%s]", pDeli->mix.cdata[0] );
			}
			
			// FF���M�v��
			mcls_pFFSend->SetFFSend();
			break;

//-----------------------------------------------------------
		case FACT_TO_MAEDAT_03:			// FF�O�H���r����M�ʒm

			// �p�����[�^�ǂݍ���
			mcls_pParam->ReadParam_All();

			// FF�O�H�����o�^
			MakeMaeDefectInf( pDeli );

			// �O�H���ҏW�Ǘ��֒ʒm
			mcls_pPreDefect->SetGetData();
			break;

//-----------------------------------------------------------
		case E_PD_END:
			break;

//-----------------------------------------------------------
		}
		// �J��
		SAFE_DELETE(pDeli);

		return;

//-----------------------------------------------------------------------------------------------
	default :
		// ���肦�Ȃ��I�I
		LOG(em_ERR), "[%s] ThreadEvent EventNo NG=%d", my_sThreadName, nEventNo );
		return;
	}
}

//------------------------------------------
// �X���b�h�I���O���� (�I���V�O�i����ɃR�[���o�b�N)
//------------------------------------------
int PreProcManager::ThreadLast()
{
	COMMON_QUE* pDeli;

	// �󂯓n���L���[�Ɋi�[����Ă��鍀�ڂ�S�ĊJ��
	while( true ) {
		pDeli = gque_Deli.GetFromHead( 0 );
		if( pDeli == NULL ) break;

		// �J��
		SAFE_DELETE_ARRAY( pDeli )
	}

	return 0;
}

//------------------------------------------
// �O�H���R�C�����o�^���C������
// COMMON_QUE* pQue �L���[�󂯓n��
//-----------------------------------------
bool PreProcManager::MakeMaeCoilInf( COMMON_QUE* pQue )
{
	PRO_TYPE_DATA_COILINF& coil = ((PRO_TYPE_COILINF*)mtbl_pCoil->rec[pQue->mix.idata[0]].val)->data;	// �R�C�����
	int nDataCnt;								// �f�[�^����
	char cMeiNo[ SIZE_MEI_NO+1 ];				// ����No
	char cKizukenNo[ SIZE_KIZUKEN_NO+1 ];		// �Ǘ�No
	PRO_TYPE_DATA_COILINF::NEXT_COIL* pnext_coil;	// ���R�C�����

	//// ������
	nDataCnt = 0;
	memset( cKizukenNo, 0, sizeof(cKizukenNo) );
	memcpy( cKizukenNo, pQue->mix.cdata[0], SIZE_KIZUKEN_NO );



	//// ODBC�ڑ��N���X �C���X�^���X����
	OdbcBase clsDB;
	if( ! clsDB.Connect( TASKINI_NAME ) ) {
		// �f�[�^�x�[�X�ڑ��G���[
		LOG(em_ERR), "[%s] DB�ڑ��G���[", my_sThreadName );
		syslog(SYSNO_DB_CONNECT_ERR, "");
		return false;
	}

	// �\��R�C�������[�v
	for( int ii = 0; ii < PRO_SIZE_NEXTINF_CNT; ii++ ) {

		// ���R�C�����
		pnext_coil = &coil.next_coil[ii];

		// �L�[��� ����No
		memset( cMeiNo, 0x00, sizeof(cMeiNo) );
		memcpy( cMeiNo, pnext_coil->KeyInf.cMeiNo, SIZE_MEI_NO );

		//����No���󔒂̎��́A�o�^���Ȃ�
		if( ' ' == cMeiNo[0] || 0x00 == cMeiNo[0] ) continue;

		// �E�H�[�}�[�ނ͗v�����Ȃ�
		//if('1' == pnext_coil->cWalmKubun[0]) continue;

		//// �R�C���ʔ��ʃe�[�u���o�^
		if( ! InsertCoilOrder( &clsDB, cKizukenNo, (ii + 1), pnext_coil ) ) {
			return false;
		}

		//// �O�H���R�C�����L���`�F�b�N
		if( ! SelectCountMaeCoilInf( &clsDB, &pnext_coil->KeyInf, &nDataCnt ) ) {
			return false;
		}

		if( 0 == nDataCnt ) {
			// ���o�^�̏ꍇ�́A�o�^����

			//// �O�H���R�C�����o�^
			if( ! InsertMaeCoilInf( &clsDB, cKizukenNo, (ii + 1), pnext_coil ) ) {
				return false;
			}
		}
	}

	//// �����t�����g�[���� �X�V
	if( ! UpdateMaeCoilInf( &clsDB, mtbl_pCoil->rec[pQue->mix.idata[0]].basedata.cCoilKey, mtbl_pCoil->rec[pQue->mix.idata[0]].basedata.nCutLenF) ) {
		return false;
	}

	return true;
}

//------------------------------------------
// �O�H���R�C����� �����擾����
// OdbcBase* clspDB		DB�C���X�^���X
// PRO_TYPE_KEY* keyinf	�L�[���
// int* nDataCnt		�f�[�^����
//------------------------------------------
bool PreProcManager::SelectCountMaeCoilInf( OdbcBase* clspDB, PRO_TYPE_KEY* keyinf, int* nDataCnt )
{
	char cMeiNo[SIZE_MEI_NO + 1];			// ����No(������)
	CString sql;							// SQL��
	CString cDbName;
	cDbName = DB_MAE_COIL_INF;
	CString wk;
	CTime t = CTime::GetCurrentTime() - CTimeSpan( 1,0,0,0 );		// 1���O
	CString strTime = t.Format("'%Y/%m/%d %H:%M:%S'");

	// FF�ւ̗v���͖���No�݂̂Ŏ��{�B
	// FF����̃R�C��No�́A"-"���폜����邽�߃L�[���̈�v�m�F�͂��Ȃ�
	memset(cMeiNo, 0x00, sizeof(cMeiNo));
	memcpy(cMeiNo, keyinf->cMeiNo, sizeof(SIZE_MEI_NO));

	sql =  "SELECT COUNT(*)";
	sql += " FROM ";
	sql += cDbName;
	wk.Format( " WHERE �L�[��� LIKE '%s%%' AND �o�^���t > %s", cMeiNo, strTime );
	//wk.Format( " WHERE �L�[��� = '%s' AND �o�^���t > %s", cKeyinf, strTime );
	sql += wk;
	
	//// ���s
	// �����擾
	if( ! clspDB->GetSelectKey( sql, nDataCnt ) ) {
		LOG(em_NON), "%s", sql );
		LOG(em_ERR), "[%s] �O�H���R�C�������s�G���[ [%s]", my_sThreadName, cDbName );
		syslog(SYSNO_DB_FETCH_ERR, "[%s]", cDbName);
		return false;
	}

	return true;
}

//------------------------------------------
// �O�H���R�C����� �擾����
// OdbcBase* clspDB		DB�C���X�^���X
// PRO_TYPE_KEY* keyinf	�L�[���
// char* cKizuKenNo		���Ǘ�NO
// char* nFFFlg			FF�O�H���r����M�t���O
// char* nKariFlg		���Ǘ�No�t���O
// �߂�l�@ture:�f�[�^�L�� false:�f�[�^����
//-----------------------------------------
bool PreProcManager::SelectMaeCoilInf( OdbcBase* clspDB, PRO_TYPE_KEY* keyinf, char* cKizuKenNo, int* nFFFlg, int* nKariFlg )
{
	
	char cMeiNo[SIZE_MEI_NO + 1];			// ����No(������)
	CString sql;							// SQL��
	CString cDbName;
	cDbName = DB_MAE_COIL_INF;
	CString wk;
	CTime t = CTime::GetCurrentTime() - CTimeSpan( 1, 0, 0, 0 );	// 1���O
	CString strTime = t.Format("'%Y/%m/%d %H:%M:%S'");

	// FF�ւ̗v���͖���No�݂̂Ŏ��{�B
	// FF����̃R�C��No�́A"-"���폜����邽�߃L�[���̈�v�m�F�͂��Ȃ�
	memset(cMeiNo, 0x00, sizeof(cMeiNo));
	memcpy(cMeiNo, keyinf->cMeiNo, sizeof(SIZE_MEI_NO));

	//// SQL���쐬
	sql =  "SELECT";
	sql += "  �Ǘ�No";
	sql += " ,FF�O�H���r����M�t���O";
	sql += " ,���Ǘ�No�t���O ";
	sql += " FROM ";
	sql += cDbName;
	wk.Format(" WHERE �L�[��� LIKE '%s%%' AND �o�^���t > %s", cMeiNo, strTime);
	//wk.Format(" WHERE �L�[��� = '%s' AND �o�^���t > %s", cKeyinf, strTime);
	sql += wk;
	
	//// ���s
	if( ! clspDB->ExecuteDirect( sql ) ) {
		LOG(em_NON), "%s", sql );
		LOG(em_ERR), "[%s] �O�H���R�C�������s�G���[ [%s]", my_sThreadName, cDbName );
		syslog(SYSNO_DB_EXECUTEDIRECT_ERR, "[%s]", cDbName);
		return false;
	}
	
	//// �擾
	int sqlrc = clspDB->Fetch();
	if( sqlrc == KIZUODBC_FETCH_OK ) {					// �f�[�^�L��
		
		clspDB->GetData( 1, cKizuKenNo, SIZE_KIZUKEN_NO );		// �Ǘ�No
		clspDB->GetData( 2, nFFFlg );							// FF�O�H���r����M�t���O
		clspDB->GetData( 3, nKariFlg );							// ���Ǘ�No�t���O


	} else if( sqlrc == KIZUODBC_FETCH_NODATE ) {		// �f�[�^����
		return false;

	} else {								// �G���[
		LOG(em_NON), "%s", sql );
		LOG(em_ERR), "[%s] �O�H���R�C�����擾�G���[ [%s]", my_sThreadName, cDbName );
		syslog(SYSNO_DB_FETCH_ERR, "[%s]", cDbName);
		return false;
	}

	return true;
}

//------------------------------------------
// �R�C���ʔ��ʃe�[�u���o�^
// OdbcBase* clspDB		DB�C���X�^���X
// char* cKizukenNo		�r���Ǘ�NO
// int nTujun			�ʏ�
// PRO_TYPE_KEY* keyinf	�L�[���
// PRO_TYPE_DATA_COILINF::NEXT_COIL* pnext_coil ���R�C�����
//------------------------------------------
bool PreProcManager::InsertCoilOrder( OdbcBase* clspDB, char* cKizukenNo, int nTujun, PRO_TYPE_DATA_COILINF::NEXT_COIL* pnext_coil )
{
	CString sql;					// SQL��
	CString cDbName;
	cDbName = DB_COIL_ORDER;

	//// �R�C���ʒu���
	// �w�b�_�[��
	sql = "INSERT INTO ";
	sql += cDbName;
	sql += " VALUES(";	

	// �f�[�^��
	CreateSQL::sqlColumnInsert( &sql, cKizukenNo, SIZE_KIZUKEN_NO );						// �Ǘ�No
	CreateSQL::sqlColumnInsert( &sql, nTujun );												// �ʏ�
	CreateSQL::sqlColumnInsert( &sql, (char*)&pnext_coil->KeyInf, sizeof(PRO_TYPE_KEY) );	// �L�[���
	CreateSQL::sqlColumnInsert(&sql, pnext_coil->nFrontLen);								// �����t�����g�[����
	CreateSQL::sqlColumnInsert( &sql, pnext_coil->cKouteiCode, SIZE_KOUTEI_CODE );			// �O�H���R�[�h
	CreateSQL::sqlColumnTrim( &sql );														// �Ō�̃J���}����菜��
	sql += ")";

	// ���s
	if( ! clspDB->ExecuteDirect( sql ) ) {
		LOG(em_NON), "%s", sql );
		LOG(em_ERR), "[%s] �R�C���ʔ��ʃe�[�u���o�^�G���[ [%s]", my_sThreadName, cDbName );
		syslog(SYSNO_DB_EXECUTEDIRECT_ERR, "[%s]", cDbName);
		return false;
	}

	LOG(em_MSG), "[%s] �R�C���ʔ��ʃe�[�u���o�^ [�Ǘ�No=%s][�ʏ�=%.4d]", my_sThreadName, cKizukenNo, nTujun );
	return true;
}

//------------------------------------------
// �O�H���R�C�����o�^
// OdbcBase* clspDB			 DB�C���X�^���X
// char* cKizukenNo          �Ǘ�NO
// int   nTujun		         �ʏ�
// PRO_TYPE_DATA_COILINF::NEXT_COIL* pnext_coil ���R�C�����
//-----------------------------------------
bool PreProcManager::InsertMaeCoilInf( OdbcBase* clspDB, char* cKizukenNo, int nTujun,
									   PRO_TYPE_DATA_COILINF::NEXT_COIL* pnext_coil )
{
	CString sql;					// SQL��
	CString cDbName;
	cDbName = DB_MAE_COIL_INF;
	char cWkKizukenNo[SIZE_KIZUKEN_NO];
	char cKeyinf[SIZE_COIL_KEY+1];

	sprintf( cWkKizukenNo, "%.17s%02d",cKizukenNo, nTujun );

	memset(cKeyinf, 0x00, sizeof(cKeyinf));
	memcpy(cKeyinf, &pnext_coil->KeyInf, sizeof(PRO_TYPE_KEY));

	////// �R�C���ʒu���
	// �w�b�_�[��
	sql = "INSERT INTO ";
	sql += cDbName;
	sql += " VALUES(";	

	// �f�[�^��
	CreateSQL::sqlColumnInsert(&sql, cWkKizukenNo, SIZE_KIZUKEN_NO);				// ���Ǘ�No
	CreateSQL::sqlColumnAddTime(&sql, true);										// �o�^���t
	CreateSQL::sqlColumnInsert(&sql, (char*)&pnext_coil->KeyInf, SIZE_COIL_KEY);	// �L�[���
	CreateSQL::sqlColumnInsert(&sql, NULL, 0);										// ID
	CreateSQL::sqlColumnInsert(&sql, -1);											// ���Ǘ�No�t���O(-1:���Ǘ�No)
	CreateSQL::sqlColumnInsert(&sql, -1);											// FF�O�H���r����M�t���O(-1:�O�H���r��񖢎�M)

	CreateSQL::sqlColumnInsert(&sql, pnext_coil->cKouteiCode, SIZE_KOUTEI_CODE);	// �H���R�[�h

	CreateSQL::sqlColumnInsert(&sql, pnext_coil->KeyInf.cMeiNo,  SIZE_MEI_NO);		// ����No
	CreateSQL::sqlColumnInsert(&sql, pnext_coil->KeyInf.cCoilNo, SIZE_COIL_NO);		// �R�C��No
	CreateSQL::sqlColumnInsert(&sql, 0);											// �����t�����g�[����
	CreateSQL::sqlColumnInsert(&sql, NULL, 0);										// �≄�R�C���ԍ�
	CreateSQL::sqlColumnInsert(&sql, NULL, 0);										// �≄�R�C���d��
	CreateSQL::sqlColumnInsert(&sql, NULL, 0);										// �≄�R�C������
	CreateSQL::sqlColumnInsert(&sql, NULL, 0);										// �≄��
	CreateSQL::sqlColumnInsert(&sql, NULL, 0);										// ������MIN
	CreateSQL::sqlColumnInsert(&sql, NULL, 0);										// ������MAX
	CreateSQL::sqlColumnInsert(&sql, NULL, 0);										// ��_����1
	CreateSQL::sqlColumnInsert(&sql, NULL, 0);										// �����
	CreateSQL::sqlColumnInsert(&sql, NULL, 0);										// ��������
	CreateSQL::sqlColumnInsert(&sql, NULL, 0);										// �ޗ���
	CreateSQL::sqlColumnInsert(&sql, NULL, 0);										// ������MIN_����1
	CreateSQL::sqlColumnInsert(&sql, NULL, 0);										// ������MAX_����1
	CreateSQL::sqlColumnInsert(&sql, NULL, 0);										// ����_���b�g����_����1
	CreateSQL::sqlColumnInsert(&sql, NULL, 0);										// ���a_����1
	CreateSQL::sqlColumnInsert(&sql, NULL, 0);										// ��
	CreateSQL::sqlColumnInsert(&sql, NULL, 0);										// �������a
	CreateSQL::sqlColumnInsert(&sql, NULL, 0);										// ���f��_����1
	CreateSQL::sqlColumnInsert(&sql, NULL, 0);										// �O�H���R�[�h
	CreateSQL::sqlColumnInsert(&sql, NULL, 0);										// ���zRCL�r���Ǘ�No
	CreateSQL::sqlColumnInsert(&sql, NULL, 0);										// ��̂ƂȂ�≄�R�C��No
	CreateSQL::sqlColumnInsert(&sql, NULL, 0);										// �̎�J�n�ʒu����
	CreateSQL::sqlColumnInsert(&sql, NULL, 0);										// �̎�I���ʒu����
	CreateSQL::sqlColumnInsert(&sql, NULL, 0);										// �̎�q��
	CreateSQL::sqlColumnTrim(&sql);													// �Ō�̃J���}����菜��
	sql += ")";

	// ���s
	if( ! clspDB->ExecuteDirect( sql ) ) {
		LOG(em_NON), "%s", sql );
		LOG(em_ERR), "[%s] �O�H���R�C�����o�^�G���[ [%s]", my_sThreadName, cDbName );
		syslog(SYSNO_DB_EXECUTEDIRECT_ERR, "[%s]", cDbName);
		return false;
	}
	LOG(em_MSG), "[%s] �O�H���R�C�����o�^ [���Ǘ�No=%s][�L�[���=%s]",
		my_sThreadName, cWkKizukenNo, cKeyinf);
	return true;
}

//------------------------------------------
// FF�O�H���r���擾����
// OdbcBase* clspDB		DB�C���X�^���X
// char* cId			ID
// PRO_TYPE_KEY* keyinf	�L�[���
// FF_MAE_INF* typData	FF�O�H������
// �߂�l�@ture:���� false:�ُ�
//------------------------------------------
bool PreProcManager::SelectBufFfRMaeinf( OdbcBase* clspDB, char* cId, PRO_TYPE_KEY* pkeyinf, FF_MAE_INF* typData ) 
{
	CString sql;					// SQL��
	CString cDbName;
	cDbName = DB_BUF_FF_R_MAEINF;
	CString wk;

	//// SQL���쐬(��M�`���擾)
	sql =  "SELECT";
	sql += " ���`��1, ���`��2, �L�[���";
	sql += " FROM ";
	sql += cDbName;
	wk.Format(" WHERE ID = '%s'",cId);
	sql += wk;

	//// ���s
	if( ! clspDB->ExecuteDirect( sql ) ) {
		LOG(em_NON), "%s", sql );
		LOG(em_ERR), "[%s] FF�O�H���r����M�o�b�t�@���s�G���[ [%s]", my_sThreadName, cDbName );
		syslog(SYSNO_DB_EXECUTEDIRECT_ERR, "[%s]", cDbName);
		return false;
	}

	//// �擾
	int sqlrc = clspDB->Fetch();
	if( sqlrc == KIZUODBC_FETCH_OK ) {					// �f�[�^�L��


		clspDB->GetDataImage( 1, &typData->torb[0], sizeof(FF_MAE_JISSEKI) );		// ���`��1
		clspDB->GetDataImage( 2, &typData->torb[1], sizeof(FF_MAE_JISSEKI) );		// ���`��2
		clspDB->GetDataImage( 3, pkeyinf, SIZE_COIL_KEY );							// �L�[���

	} else if( sqlrc == KIZUODBC_FETCH_NODATE ) {		// �f�[�^����
		return false;	
	} else {								// �G���[
		LOG(em_NON), "%s", sql );
		LOG(em_ERR), "[%s]  FF�O�H���r����M�o�b�t�@�擾�G���[ [%s]", my_sThreadName, cDbName );
		syslog(SYSNO_DB_FETCH_ERR, "[%s]", cDbName);
		return false;
	}

	return true;

}

//------------------------------------------
// FF�O�H�����o�^���C������
// COMMON_QUE* pQue �L���[�󂯓n��
//-----------------------------------------
void PreProcManager::MakeMaeDefectInf( COMMON_QUE* pQue )
{
	PRO_TYPE_KEY keyinf;				// �L�[���
	FF_MAE_INF typMaejis;				// FF�O�H������
	int nFFFlg = 0;						// FF�O�H���r���W�J�t���O
	int nKariFlg = 0;					// ���Ǘ�No�t���O
	char cMeiNo[SIZE_MEI_NO+1];			// ����No
	char cKeyinf[SIZE_COIL_KEY+1];		// �L�[���(������)
	char cKizuKenNO[SIZE_KIZUKEN_NO];	// �Ǘ�No
	

	// ������
	memset( &keyinf, 0x00, sizeof(keyinf) );
	memset( &typMaejis, 0x00, sizeof(typMaejis) );
	memset( cKizuKenNO, 0x00, sizeof(cKizuKenNO) );


	//// ODBC�ڑ��N���X �C���X�^���X����
	OdbcBase clsDB;
	if( ! clsDB.Connect( TASKINI_NAME ) ) {
		// �f�[�^�x�[�X�ڑ��G���[
		LOG(em_ERR), "[%s] DB�ڑ��G���[", my_sThreadName );
		syslog(SYSNO_DB_CONNECT_ERR, "");
		return;
	}

	//// FF�O�H���r��� �擾
	if( ! SelectBufFfRMaeinf( &clsDB, pQue->fc.data, &keyinf, &typMaejis ) ) {
		LOG(em_ERR), "[%s] FF�O�H�����я��擾���s [ID=%s]", my_sThreadName, &pQue->fc.data[0] );
		return;
	}

	// ����No
	memset( cMeiNo, 0x00, sizeof(cMeiNo) );
	memcpy( cMeiNo, keyinf.cMeiNo, SIZE_MEI_NO );
	memset( cKeyinf, 0x00, sizeof(cKeyinf) );
	memcpy( cKeyinf, &keyinf, sizeof(PRO_TYPE_KEY) );
	
	// FF�ɓ��Y�R�C���ԍ��̃f�[�^�����݂�����
	if( 0 == memcmp( typMaejis.torb[0].typFFData.Hd.ReiCoilNo, "XXXXXXXXXX", sizeof(typMaejis.torb[0].typFFData.Hd.ReiCoilNo) ) ) {
		LOG(em_ERR), "[%s] FF�T�[�o�[�ɊY������No�R�C���Ȃ� [����No=%s][�L�[���=%s]", my_sThreadName, cMeiNo, cKeyinf );
		syslog(PARAM_SYSLOG_BASENO + 0 ,"[����No=%s][�L�[���=%s]", cMeiNo, cKeyinf); // // 0 : FF�T�[�o�[�ɊY������No�R�C������
		return;
	}

	//// ���Ǘ�No�t���O�AFF�O�H���r����M�t���O�擾
	if( ! SelectMaeCoilInf( &clsDB, &keyinf, cKizuKenNO, &nFFFlg, &nKariFlg ) ) {
		LOG(em_ERR), "[%s] �O�H���R�C�����Ȃ� [�L�[���=%s]", my_sThreadName, cKeyinf );
		return;
	}
	// FF�O�H���r����M�`�F�b�N
	if( -1 != nFFFlg ) {
		LOG(em_WAR), "[%s] �O�H���r��񐳏��M�ς� [�L�[���=%s]", my_sThreadName, cKeyinf );
		return;
	}
	// ���Ǘ�No�`�F�b�N
	if( -1 != nKariFlg ) {
		LOG(em_WAR), "[%s] �Ǘ�No�W�J�ς� [�L�[���=%s]", my_sThreadName, cKeyinf );
		return;
	}

	//// �O�H���r���o�^
	if( ! InsertDefectInfMain( &clsDB, cKizuKenNO, &typMaejis ) ) {
		LOG(em_ERR), "[%s] �O�H���r���X�V���s [����No=%s][���Ǘ�No=%s]", my_sThreadName, cMeiNo, cKizuKenNO );
		return;
	}

	//// �O�H���R�C�����X�V
	if( ! UpdateCoilInf( &clsDB, cKizuKenNO, pQue->fc.data, &typMaejis ) ) {
		LOG(em_ERR), "[%s] �O�H���R�C�����X�V���s [����No=%s] [���Ǘ�No=%s]", my_sThreadName, cMeiNo, cKizuKenNO );
		return;
	}


	LOG(em_MSG), "[%s] �O�H���R�C�����X�V [����No=%s] [���Ǘ�No=%s]", my_sThreadName, cMeiNo, cKizuKenNO );
}

//------------------------------------------
// FF�O�H�����o�^���C������
// OdbcBase* clsInsDB	�o�^DB�C���X�^���X
// char* cKizuKenNO�@�r���Ǘ�No
// FF_MAE_INF* typData FF�O�H������
// �߂�l�@ture:���� false:�ُ�
//-----------------------------------------
bool PreProcManager::InsertDefectInfMain( OdbcBase* clsInsDB, char* cKizuKenNO, FF_MAE_INF* typData )
{
	int ii;							// �J�E���^
	int jj;							// �J�E���^
	int kk;							// �J�E���^
	int nKizuNo;					// �rNo
	FF_MAE_JISSEKI*		pFfMae;		// �ϐ��ȗ����p �O�H���r���`���{��
	FF_MAE_TYPE_INF*	pKizuInf;	// �ϐ��ȗ����p �r���o���
	FF_MAE_TYPE_DATA*	pKizuData;	// �ϐ��ȗ����p �r���
	int					nDspMode;	// �O�H���\�����[�h

	//������
	nKizuNo = 1;

	// �\�Ɨ��̃��[�v����
	for( ii = 0; ii < NUM_MEN; ii++ ) {
		// �O�H���r���`���{�� �ϐ��ȗ���
		pFfMae = &typData->torb[ii].typFFData;


		// �r���o��񃋁[�v
		for(jj = 0; jj < MAX_FF_MAE_DINF_CNT; jj++) {


			// �r���o��� �ϐ��ȗ���
			pKizuInf = &pFfMae->Inf[jj];

			//�r���o�H�����󔒂̎��́A�o�^���Ȃ�
			if( 0 == memcmp( pKizuInf->Kizukbn, "  ", sizeof(pKizuInf->Kizukbn) ) ||
				0x00 == pKizuInf->Kizukbn[0] ) {
				continue;
			}

			//// �O�H���\�����[�h�擾
			nDspMode = mcls_pParam->GetDspMode( pKizuInf->Kizukbn );

			if( -1 == nDspMode ) {
				// �O�H���\�����[�h�擾�G���[(DB�o�^����)
				LOG(em_INF), "[%s] �O�H���}�X�^�[���o�^ [�H���R�[�h=%.2s]", my_sThreadName, pKizuInf->Kizukbn );
#if 0	// �O�H���}�X�^�[���o�^�̓A���[���o�͕s�v(�\���ΏۊO�̍H���͓o�^���Ȃ�����)
				syslog(373, "[�H���R�[�h=%.2s]", pKizuInf->Kizukbn );
#endif
			}

			//// �O�H�����C���ʃR�C�����e�[�u���o�^
			if( ! InsertMaeCoilLine( clsInsDB, cKizuKenNO, ii, (jj+1), nDspMode, pKizuInf ) ) {
				return false;
			}

			// �r���ѓ`�� (�S��)�̃��[�v
			for( kk = 0; kk < MAX_FF_MAE_DEF_CNT; kk++ ) {
				pKizuData = &pFfMae->def[kk];

				// �r���o�H������v
				if( 0 == memcmp( pKizuInf->Kizukbn, pKizuData->Kizukbn, sizeof(pKizuInf->Kizukbn) ) ) {
					// 20221021 2TCM���@���ɔ������H������ ���� --->>>
					if (DIV_PROC_KIND_INDEX_TCM_ANA == jj && MAX_FF_MAE_DEF_CNT - MAX_FF_MAE_N2TCM_KIZU_CNT <= kk) { continue; }		// N2TCM����N2TCM�̋�ʂ�����Ă���ꏊ�ŔF��
					if (DIV_PROC_KIND_INDEX_TCM_KIZU == jj && MAX_FF_MAE_DEF_CNT - MAX_FF_MAE_N2TCM_KIZU_CNT > kk) { continue; }
					// 20221021 2TCM���@���ɔ������H������ ���� ---<<<

					//// �O�H��FF�r���e�[�u���o�^
					if( ! InsertMaeFfInf( clsInsDB, cKizuKenNO, nKizuNo, (jj+1), ii, nDspMode, pKizuInf, pKizuData ) ) {
						return false;
					}

					// �rNo �J�E���g�A�b�v
					nKizuNo += 1;

				}
			}
		}
	}

	return true;
}

//------------------------------------------
// �O�H�����C���ʃR�C�����o�^
// OdbcBase* clsInsDB	DB�o�^�C���X�^���X
// char cKizuKenNO		�r���Ǘ�No
// int nOuDiv			�\���敪
// int nProcKind		�H�����(1�`7)
// int nDspMode			�O�H���\�����[�h
// FF_MAE_TYPE_INF* pKizuInf	
// �߂�l�@ture:���� false:�ُ�
//------------------------------------------
bool PreProcManager::InsertMaeCoilLine( OdbcBase* clsInsDB, char* cKizuKenNO, int nOuDiv, int nProcKind,
									    int nDspMode, FF_MAE_TYPE_INF* pKizuInf )
{
	CString sql;					// SQL��
	CString cDbName;
	cDbName = DB_MAE_COIL_LINE;
	char cId[SIZE_SERIAL_FF_NO+1];

	////// �O�H���r���
	// �w�b�_�[��
	sql = "INSERT INTO ";
	sql += cDbName;
	sql += " VALUES(";	

	// �f�[�^��
	CreateSQL::sqlColumnInsert( &sql, cKizuKenNO, SIZE_KIZUKEN_NO );								// �Ǘ�No
	CreateSQL::sqlColumnInsert( &sql, nProcKind );													// �H�����
	CreateSQL::sqlColumnInsert( &sql, nOuDiv );														// �\���敪
	// �����X�e�[�^�X
	if ( 1 == nDspMode ) {								// �O�H���Ӄ֏��擾���r�摜�\��
		CreateSQL::sqlColumnInsert( &sql, -1 );			// -1�F���擾
	} else if ( 2 == nDspMode ) {						// �e�e���̂ݕ\��
		CreateSQL::sqlColumnInsert( &sql,  -9 );		//  -9 : �擾�擾�ΏۊO�̑O�H��
	} else {											// �\���ΏۊO
		CreateSQL::sqlColumnInsert( &sql, 0 );			// 0�F�Y������O�H���Ӄ֏����擾�ς�
	}
	CreateSQL::sqlColumnInsert( &sql, pKizuInf->Kizukbn, sizeof(pKizuInf->Kizukbn) );				// �r���o�H��
	memset( cId, 0x00, sizeof(cId) );
	memcpy( cId, pKizuInf->Id, sizeof(pKizuInf->Id) );
	KizuFunction::Trim( cId );
	CreateSQL::sqlColumnInsert( &sql, cId, sizeof(pKizuInf->Id) );															// �O�H���Ǘ�No
	CreateSQL::sqlColumnInsert( &sql, KizuFunction::AtoIex( pKizuInf->Mode, sizeof(pKizuInf->Mode) ) );						// �������u�������[�h
	CreateSQL::sqlColumnInsert( &sql, KizuFunction::AtoIex( pKizuInf->DataDefault, sizeof(pKizuInf->DataDefault) ) );		// �r�ݒ��
	CreateSQL::sqlColumnInsert( &sql, KizuFunction::AtoIex( pKizuInf->OverStatus, sizeof(pKizuInf->OverStatus) ) );			// �r��OV
	CreateSQL::sqlColumnTrim( &sql );					// �Ō�̃J���}����菜��
	sql += ")";

	// ���s
	if( ! clsInsDB->ExecuteDirect( sql ) ) {
		LOG(em_NON), "%s", sql );
		LOG(em_ERR), "[%s] �O�H�����C���ʃR�C�����o�^�G���[ [%s]", my_sThreadName, cDbName );
		syslog(SYSNO_DB_EXECUTEDIRECT_ERR, "[%s]", cDbName);
		return false;
	}

	return true;
}

//------------------------------------------
// �O�H��FF�r���e�[�u���o�^
// OdbcBase* clsInsDB	DB�o�^�C���X�^���X
// char* cKizuKenNO		�r���Ǘ�No
// int nKizuNo			�rNo
// int nProcKind		�H�����(1�`7)
// int nOuDiv			�\���敪
// int nDspMode			�O�H���\�����[�h
// FF_MAE_TYPE_INF* pKizuInf	�r���o���
// FF_MAE_TYPE_DATA* pKizuData	�r���
// �߂�l�@ture:���� false:�ُ�
//------------------------------------------
bool PreProcManager::InsertMaeFfInf( OdbcBase* clsInsDB, char* cKizuKenNO, int nKizuNo, int nProcKind, int nOuDiv,
	                                 int nDspMode, FF_MAE_TYPE_INF* pKizuInf, FF_MAE_TYPE_DATA* pKizuData )
{
	int nTypeEdasysID;			// �r��EdasysID
	int nGradeEdasysID;			// �O���[�hEdasysID
	CString sql;				// SQL��
	CString cDbName;
	cDbName = DB_MAE_FF_INF;
	char cKind[2];				// �r��
	char cGrade[2];				// �r�O���[�h
	char cMaeNo[5];
	int nDefectPos;				// FF�r���������ʒu
	int nWidPos;				// FF�r�Е����ʒu
	int nLen;					// FF�r����

	memset( cKind, 0x00, sizeof(cKind) );
	memcpy( cKind, pKizuData->Kind, sizeof(cKind) );
	memset( cGrade, 0x00, sizeof(cGrade) );
	memcpy( cGrade, pKizuData->Grade, sizeof(cGrade) );
	memset( cMaeNo, 0x00, sizeof(cMaeNo) );
	memcpy( cMaeNo, pKizuData->No, sizeof(pKizuData->No) );

	////// �O�H���r���
	// �w�b�_�[��
	sql = "INSERT INTO ";
	sql += cDbName;
	sql += " VALUES(";	

	// �f�[�^��
	CreateSQL::sqlColumnInsert( &sql, cKizuKenNO, SIZE_KIZUKEN_NO );										// �Ǘ�No
	CreateSQL::sqlColumnInsert( &sql, nKizuNo );															// �rNo
	CreateSQL::sqlColumnInsert( &sql, nProcKind );															// �H�����
	CreateSQL::sqlColumnInsert( &sql, nOuDiv );																// �\���敪
	CreateSQL::sqlColumnInsert( &sql, 0 );																	// ���O�ҏW�t���O
	CreateSQL::sqlColumnInsert( &sql, pKizuInf->Id, sizeof(pKizuInf->Id) );									// �O�H���r���Ǘ�No
	// 20221213 2TCM���@���ɔ������H������(�s��Ή�) �a�� --->>>
	//CreateSQL::sqlColumnInsert( &sql, strtol(cMaeNo, NULL, 16));											// �O�H���rNo
	if (DIV_PROC_KIND_INDEX_TCM_ANA  == nProcKind -1 ) {
		CreateSQL::sqlColumnInsert(&sql, strtol(cMaeNo, NULL, 10));											// �O�H���rNo
	}
	else {
		CreateSQL::sqlColumnInsert( &sql, strtol(cMaeNo, NULL, 16));										// �O�H���rNo
	}
	// 20221213 2TCM���@���ɔ������H������(�s��Ή�) �a�� --->>>
	CreateSQL::sqlColumnInsert( &sql, pKizuData->Kind, sizeof(pKizuData->Kind) );							// FF�r��
	CreateSQL::sqlColumnInsert( &sql, pKizuData->Grade, sizeof(pKizuData->Grade) );							// FF�O���[�h
	CreateSQL::sqlColumnInsert( &sql, pKizuInf->Kizukbn, sizeof(pKizuInf->Kizukbn) );						// FF�r���o�H��
	CreateSQL::sqlColumnInsert( &sql, KizuFunction::AtoIex( pKizuData->kbn, sizeof(pKizuData->kbn) ) );		// FF�r�敪

	// FF�r���������ʒu
	nDefectPos = KizuFunction::AtoIex( pKizuData->DefectPos, sizeof(pKizuData->DefectPos) );
	nDefectPos *= 100;		// �P�� [0.1m]����[mm]�֕ϊ�
	CreateSQL::sqlColumnInsert( &sql, nDefectPos );
	// FF�r�Е����ʒu
	nWidPos = KizuFunction::AtoIex( pKizuData->WidPos, sizeof(pKizuData->WidPos) );
	CreateSQL::sqlColumnInsert( &sql, nWidPos );
	// FF�r����
	nLen = KizuFunction::AtoIex( pKizuData->Len, sizeof(pKizuData->Len) );
	nLen *= 100;			// �P�� [0.1m]����[mm]�֕ϊ�
	CreateSQL::sqlColumnInsert( &sql, nLen );

	// �����r��A����O���[�h�擾
	KizuFunction::SpaceToNull( cKind, sizeof(cKind) );
	KizuFunction::SpaceToNull( cGrade, sizeof(cGrade) );
	nTypeEdasysID = mcls_pParam->GetTyppeEdasysID( pKizuData->Kizukbn, cKind );
	if( 0 == nTypeEdasysID && 0 < nDspMode ) {
		// �V�X�e����ŕ\���Ώۂł����r�햢�o�^
		LOG(em_INF), "[%s] �O�H���r��}�X�^�[���o�^ [�H���R�[�h=%.2s , �r��=%.2s]", my_sThreadName, pKizuData->Kizukbn , cKind );
		//syslog(374, "[�H���R�[�h=%.2s, �r��=%.2s]", pKizuData->Kizukbn , cKind );
	}
	nGradeEdasysID = mcls_pParam->GetGradeEdasysID( pKizuData->Kizukbn, cGrade );
	if( 0 == nGradeEdasysID && 0 < nDspMode ) {
		// �V�X�e����ŕ\���Ώۂł��O���[�h���o�^
		LOG(em_INF), "[%s] �O�H���O���[�h�}�X�^�[���o�^ [�H���R�[�h=%.2s, �r�O���[�h=%.2s]", my_sThreadName, pKizuData->Kizukbn , cGrade );
		//syslog(375, "[�H���R�[�h=%.2s, �r�O���[�h=%.2s]", pKizuData->Kizukbn , cGrade );
	}
	CreateSQL::sqlColumnInsert( &sql, nTypeEdasysID );														// �����r��
	CreateSQL::sqlColumnInsert( &sql, nGradeEdasysID );														// ����O���[�h
	CreateSQL::sqlColumnInsert( &sql, nDefectPos );															// �ŏI����ʒu(�o�^����FF�r���������ʒu�Ɠ���)
	CreateSQL::sqlColumnTrim( &sql );					// �Ō�̃J���}����菜��
	sql += ")";

	// ���s
	if( ! clsInsDB->ExecuteDirect( sql ) ) {
		LOG(em_NON), "%s", sql );
		LOG(em_ERR), "[%s] �r���o�^�G���[ [%s]", my_sThreadName, cDbName );
		syslog(SYSNO_DB_EXECUTEDIRECT_ERR, "[%s]", cDbName);
		return false;
	}

	return true;
}

//------------------------------------------
// �O�H���R�C�����X�V
// char* cKizukenNo �r���Ǘ�NO
// char* cId        ID
// FF_MAE_JISSEKI* typMaeJis
// �߂�l�@ture:���� false:�ُ�
//-----------------------------------------
bool PreProcManager::UpdateCoilInf( OdbcBase* clsInsDB, char* cKizukenNo, char* cId, FF_MAE_INF* typData )
{
	CString sql;					// SQL��
	CString wk;
	CString cDbName;
	cDbName = DB_MAE_COIL_INF;
	FF_MAE_JISSEKI* pFfMaeO;		// �ϐ��ȗ����p
	int nMukesaki1i;				// ��_����1
	int nMukesaki1KL;				// ���f��_����1

	// �ϐ��̊ȗ���
	pFfMaeO = &typData->torb[0].typFFData;

	////// �O�H���R�C�����
	//// SQL���쐬
	sql = "UPDATE ";
	sql += cDbName;
	sql += " SET ";

	// �f�[�^��
	// �\�����ʂ͂��Ȃ̂ŕ\�̃f�[�^���Z�b�g
	CreateSQL::sqlColumnUpdate( &sql, "ID", cId, SIZE_SERIAL_FF_NO );																		// ID
	CreateSQL::sqlColumnUpdate( &sql, "FF�O�H���r����M�t���O", 0 );																		// FF�O�H���r����M�t���O
	CreateSQL::sqlColumnUpdate( &sql, "�≄�R�C���ԍ�", pFfMaeO->Hd.ReiCoilNo, sizeof(pFfMaeO->Hd.ReiCoilNo) );								// �≄�R�C���ԍ�
	CreateSQL::sqlColumnUpdate( &sql, "�≄�R�C���d��", KizuFunction::AtoIex( pFfMaeO->Hd.ReiCoilG, sizeof(pFfMaeO->Hd.ReiCoilG) ) );		// �≄�R�C���d��
	CreateSQL::sqlColumnUpdate( &sql, "�≄�R�C������", KizuFunction::AtoIex( pFfMaeO->Hd.ReiCoilL, sizeof(pFfMaeO->Hd.ReiCoilL) ) );		// �≄�R�C������
	CreateSQL::sqlColumnUpdate( &sql, "�≄��", KizuFunction::AtoIex( pFfMaeO->Hd.ReiCoilAtu, sizeof(pFfMaeO->Hd.ReiCoilAtu) ) );			// �≄��
	CreateSQL::sqlColumnUpdate( &sql, "������MIN", KizuFunction::AtoIex( pFfMaeO->Hd.ItaatuMin, sizeof(pFfMaeO->Hd.ItaatuMin) ) );		// ������MIN
	CreateSQL::sqlColumnUpdate( &sql, "������MAX", KizuFunction::AtoIex( pFfMaeO->Hd.ItaatuMax, sizeof(pFfMaeO->Hd.ItaatuMax) ) );		// ������MAX
	nMukesaki1i = KizuFunction::AtoIex( pFfMaeO->Hd.Mukesaki1i, sizeof(pFfMaeO->Hd.Mukesaki1i) );
	nMukesaki1i /= 10;				// �P�� [0.1mm]����[mm]�֕ϊ�
	CreateSQL::sqlColumnUpdate( &sql, "��_����1", nMukesaki1i );																			// ��_����1
	CreateSQL::sqlColumnUpdate( &sql, "�����", pFfMaeO->Hd.Kensa, sizeof(pFfMaeO->Hd.Kensa) );											// �����
	CreateSQL::sqlColumnUpdate( &sql, "��������", KizuFunction::AtoIex( pFfMaeO->Hd.Maki, sizeof(pFfMaeO->Hd.Maki) ) );						// ��������
	CreateSQL::sqlColumnUpdate( &sql, "�ޗ���", KizuFunction::AtoIex( pFfMaeO->Hd.Zairyou, sizeof(pFfMaeO->Hd.Zairyou) ) );					// �ޗ���
	CreateSQL::sqlColumnUpdate( &sql, "������MIN_����1", KizuFunction::AtoIex( pFfMaeO->Hd.Mukesaki1kMin, sizeof(pFfMaeO->Hd.Mukesaki1kMin) ) );	// ������MIN_����1
	CreateSQL::sqlColumnUpdate( &sql, "������MAX_����1", KizuFunction::AtoIex( pFfMaeO->Hd.Mukesaki1kMax, sizeof(pFfMaeO->Hd.Mukesaki1kMax) ) );	// ������MAX_����1
	CreateSQL::sqlColumnUpdate( &sql, "����_���b�g����_����1", KizuFunction::AtoIex( pFfMaeO->Hd.Mukesaki1L, sizeof(pFfMaeO->Hd.Mukesaki1L) ) );	// ����_���b�g����_����1
	CreateSQL::sqlColumnUpdate( &sql, "���a_����1", KizuFunction::AtoIex( pFfMaeO->Hd.Mukesaki1N, sizeof(pFfMaeO->Hd.Mukesaki1N) ) );				// ���a_����1
	CreateSQL::sqlColumnUpdate( &sql, "��", KizuFunction::AtoIex( pFfMaeO->Hd.Itaatu, sizeof(pFfMaeO->Hd.Itaatu) ) );						// ��
	CreateSQL::sqlColumnUpdate( &sql, "�������a", KizuFunction::AtoIex( pFfMaeO->Hd.Irigawa, sizeof(pFfMaeO->Hd.Irigawa) ) );				// �������a
	nMukesaki1KL = KizuFunction::AtoIex( pFfMaeO->Hd.Mukesaki1KL, sizeof(pFfMaeO->Hd.Mukesaki1KL) );
	nMukesaki1KL /= 1000;			// �P�� [0.1mm]����[0.1m]�֕ϊ�
	CreateSQL::sqlColumnUpdate( &sql, "���f��_����1", nMukesaki1KL );																		// ���f��_����1
	CreateSQL::sqlColumnUpdate( &sql, "�O�H���R�[�h", pFfMaeO->Hd.Makoutei, sizeof(pFfMaeO->Hd.Makoutei) );									// �O�H���R�[�h
	CreateSQL::sqlColumnUpdate( &sql, "���zRCL�Ǘ�No", pFfMaeO->VRcl.KizukenNo, sizeof(pFfMaeO->VRcl.KizukenNo) );							// ���zRCL�r���Ǘ�No
	CreateSQL::sqlColumnUpdate( &sql, "��̂ƂȂ�≄�R�C��No", pFfMaeO->FromTo.CoilNo, sizeof(pFfMaeO->FromTo.CoilNo) );					// ��̂ƂȂ�HOT�R�C��No
	CreateSQL::sqlColumnUpdate( &sql, "�̎�J�n�ʒu����", KizuFunction::AtoIex( pFfMaeO->FromTo.From, sizeof(pFfMaeO->FromTo.From) ) );		// �̎�J�n�ʒu����
	CreateSQL::sqlColumnUpdate( &sql, "�̎�I���ʒu����", KizuFunction::AtoIex( pFfMaeO->FromTo.To, sizeof(pFfMaeO->FromTo.To) ) );			// �̎�I���ʒu����
	CreateSQL::sqlColumnUpdate( &sql, "�̎�q��", KizuFunction::AtoIex( pFfMaeO->FromTo.Koban, sizeof(pFfMaeO->FromTo.Koban) ) );			// �̎�q��
	CreateSQL::sqlColumnTrim( &sql );										// �Ō�̃J���}����菜��

	wk.Format( " WHERE �Ǘ�No = '%s'", cKizukenNo );
	sql += wk;
	
	// ���s
	if( ! clsInsDB->ExecuteDirect( sql ) ) {
		LOG(em_NON), "%s", my_sThreadName, sql );
		LOG(em_ERR), "[%s] �O�H���R�C�����X�V�G���[ [%s]", my_sThreadName, cDbName );
		syslog(SYSNO_DB_EXECUTEDIRECT_ERR, "[%s]", cDbName);
		return false;
	}

	return true;
}

//------------------------------------------
// �O�H���R�C�����X�V(�����t�����g�[����)
// OdbcBase* clspDB		DB�C���X�^���X
// char* cCoilKey		�L�[���
// int	nFrontLen		�����t�����g�[����
//------------------------------------------
bool PreProcManager::UpdateMaeCoilInf( OdbcBase* clspDB, char* cCoilKey, int nFrontLen )
{
	CString sql;					// SQL��
	CString cDbName;
	cDbName = DB_MAE_COIL_INF;
	CString wk;
	CTime t = CTime::GetCurrentTime() - CTimeSpan(1, 0, 0, 0);		// 1���O
	CString strTime = t.Format("'%Y/%m/%d %H:%M:%S'");
	char cKeyinf[SIZE_COIL_KEY + 1];

	// �R�C���ؑփ^�C�~���O�ŁA���Ǘ�No����Ǘ�No�̐U��Ȃ��������邽�߁A
	// �R�C������M�^�C�~���O�́A�L�[��񂩂�ΏۃR�C������肷��
	memset(cKeyinf, 0x00, sizeof(cKeyinf));
	memcpy(cKeyinf, cCoilKey, SIZE_COIL_KEY);

	////// �O�H���R�C�����
	//// SQL���쐬
	sql = "UPDATE ";
	sql += cDbName;
	sql += " SET ";

	// �f�[�^��
	CreateSQL::sqlColumnUpdate( &sql, "�����t�����g�[����", nFrontLen );
	CreateSQL::sqlColumnTrim( &sql );
	wk.Format( " WHERE �L�[��� = '%s' AND �o�^���t > %s", cKeyinf, strTime);
	sql += wk;
	
	// ���s
	if( ! clspDB->ExecuteDirect( sql ) ) {
		LOG(em_NON), "%s", sql );
		LOG(em_ERR), "[%s] �O�H���R�C�����X�V�G���[ [%s]", my_sThreadName, cDbName );
		syslog(SYSNO_DB_EXECUTEDIRECT_ERR, "[%s]", cDbName);
		return false;
	}

	return true;
}

