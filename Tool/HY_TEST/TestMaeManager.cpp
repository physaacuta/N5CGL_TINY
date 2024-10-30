#include "StdAfx.h"
#include "TestMaeManager.h"

// �������[���[�N���o ----->>>
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif
//------------------------------------------
// �R���X�g���N�^
//------------------------------------------
TestMaeManager::TestMaeManager(void) :
ThreadManager("MaeTest")
{
}

TestMaeManager::~TestMaeManager(void)
{
}

////////////////////////////////////////////////////////////////////////////////
// �X���b�h
////////////////////////////////////////////////////////////////////////////////
//------------------------------------------
// �X���b�h�J�n�O���� (�X���b�h���s����(�V�O�i���Z�b�g�O)�ɃR�[���o�b�N)
//------------------------------------------
int TestMaeManager::ThreadFirst()
{
//// ���O����
		//// �C�x���g�ݒ�
	HANDLE hArray[] = { gque_Deli.g_evSem,
						};
	SetEventNum(sizeof(hArray)/sizeof(HANDLE), hArray);

	return 0;
}

//------------------------------------------
// �X���b�h�I���O���� (�I���V�O�i����ɃR�[���o�b�N)
//------------------------------------------
int TestMaeManager::ThreadLast()
{
	DELIVERY_KEY *pDeli;				// �L���[�f�[�^

	// �������̃L���[��j��
	while(true) {
		pDeli = gque_Deli.GetFromHead(0);
		if( pDeli == NULL ) break;
		
		// �폜
		LOG(em_WAR), "[%s] �������L���[�폜 [%s]", my_sThreadName, pDeli->cKizukenNo);
		delete pDeli;
	}

	return 0;
}


//------------------------------------------
// �X���b�h�C�x���g����
// int nEventNo �C�x���gNo (0�I���W��)
//------------------------------------------
void TestMaeManager::ThreadEvent(int nEventNo)
{
	DELIVERY_KEY*	pDeli;			// �󂯓n���˗��f�[�^

	////// �V�O�i���̏���
	enum {	EV_QUE = 0,						// �L���[�˗�
	};

	////// �V�O�i����������
	switch (nEventNo) {

//-----------------------------------------------------------------------------------------------
	case EV_QUE:
		pDeli = gque_Deli.GetFromHead(); 
		Exec( pDeli );
		SAFE_DELETE(pDeli);
		break;
		
//-----------------------------------------------------------------------------------------------
	default :
		// ���肦�Ȃ��I�I
		LOG(em_ERR), "[%s] ThreadEvent EventNo NG=%d", my_sThreadName, nEventNo );
		return;
	}
}

// �O�H�����C���ʃR�C�����o�^
bool TestMaeManager::SetMaeCoilLine(char const* cKizukenNo,int ou, int nKouteiID)
{
	CString sql;				// SQL��
	CString cDbName;
	cDbName = DB_MAE_COIL_LINE ;

	//// ODBC�ڑ��N���X �C���X�^���X����
	OdbcBase clsDB;
	if( ! clsDB.Connect(TASKINI_NAME)) {
		// �f�[�^�x�[�X�ڑ��G���[
		LOG(em_ERR), "[%s] DB�ڑ��G���[", my_sThreadName);
		return false;
	}

	////// �O�H���r���
	// �w�b�_�[��
	sql = "INSERT INTO ";
	sql += cDbName;
	sql += " VALUES(";	

	// �f�[�^��
	CreateSQL::sqlColumnInsert( &sql, cKizukenNo, SIZE_KIZUKEN_NO );	// �r���Ǘ�No
	CreateSQL::sqlColumnInsert( &sql, nKouteiID);						// �H�����
	CreateSQL::sqlColumnInsert( &sql, ou );								// �\���敪
	CreateSQL::sqlColumnInsert( &sql, 0 );								// �����X�e�[�^�X
	CreateSQL::sqlColumnInsert( &sql, "22", 2 );						// �r���o�H��
	CreateSQL::sqlColumnInsert( &sql,  cKizukenNo, SIZE_KIZUKEN_NO );	// �O�H���r���Ǘ�No
	CreateSQL::sqlColumnInsert( &sql, 0 );								// �r���������[�h
	CreateSQL::sqlColumnInsert( &sql, 1 );								// �r�ݒ��
	CreateSQL::sqlColumnInsert( &sql, 1 );								// �r��OV
	CreateSQL::sqlColumnTrim( &sql );									// �Ō�̃J���}����菜��
	sql += ")";

	// ���s
	if( ! clsDB.ExecuteDirect( sql ) ) {
		LOG(em_ERR), "[%s] �O�H�����C���ʃR�C�����o�^�G���[ [%s]", my_sThreadName, cDbName );
		LOG(em_ERR), "[%s] SQL [%s]", my_sThreadName, sql );
		syslog(SYSNO_DB_EXECUTEDIRECT_ERR, "[%s]", cDbName);
		return false;
	} else {
		LOG(em_MSG), "[%s] �O�H�����C���ʃR�C�����o�^ �r���Ǘ�No[%s]", my_sThreadName, cKizukenNo );
		return true;
	}


}

// �O�H��FF�r���e�[�u���o�^
bool TestMaeManager::SetMaeFFInf(char const* cKizukenNo,int nKizuNo, int ou, double dNobiSPM, int nKouteiID)
{

	CString sql;				// SQL��
	CString cDbName;
	cDbName = DB_MAE_FF_INF;

	//// ODBC�ڑ��N���X �C���X�^���X����
	OdbcBase clsDB;
	if( ! clsDB.Connect(TASKINI_NAME)) {
		// �f�[�^�x�[�X�ڑ��G���[
		LOG(em_ERR), "[%s] DB�ڑ��G���[", my_sThreadName);
		return false;
	}

	////// �O�H���r���
	// �w�b�_�[��
	sql = "INSERT INTO ";
	sql += cDbName;
	sql += " VALUES(";	

	// FF�r�O���[�h�����肷�� "1 "�`"7 "
	char cFFgrade[2];
	cFFgrade[0] = '1' + rand() % 7;
	cFFgrade[1] = ' ';

	// FF�r������肷��
	//char* FF_Tname[] = { "B1","B4","B0","71","61","10","77","B9","C0" };
	//int FF_Tid[] = { 2,17,20,6,7,5,13,19,18 };
	char* FF_Tname[] = { "2M","2N" };
	int FF_Tid[] = { 1,2 };
	int nTidx = rand()%(sizeof(FF_Tid)/sizeof(int));

	int nPitch; // [mm]
	nPitch = 5000;

	// �f�[�^��
	CreateSQL::sqlColumnInsert( &sql, cKizukenNo, SIZE_KIZUKEN_NO );										// �r���Ǘ�No
	CreateSQL::sqlColumnInsert( &sql, nKizuNo + 1);															// �rNo
	CreateSQL::sqlColumnInsert( &sql, nKouteiID);																	// �H�����
	CreateSQL::sqlColumnInsert( &sql, ou );																	// �\���敪
	CreateSQL::sqlColumnInsert( &sql, 0 );																	// ���O�ҏW�t���O
	CreateSQL::sqlColumnInsert( &sql, cKizukenNo, SIZE_KIZUKEN_NO );										// �O�H���r���Ǘ�No
	CreateSQL::sqlColumnInsert( &sql, nKizuNo + 1);															// �O�H���rNo
	if (6 == nKouteiID) {
		CreateSQL::sqlColumnInsert(&sql,"TT", 2);												// FF�r��
	}
	else {
		CreateSQL::sqlColumnInsert(&sql, FF_Tname[nTidx], 2);												// FF�r��
	}
	
	CreateSQL::sqlColumnInsert( &sql, cFFgrade , 2 );														// FF�O���[�h
	CreateSQL::sqlColumnInsert( &sql, "22", 2);																// FF�r���o�H��
	CreateSQL::sqlColumnInsert( &sql, 1 );																	// FF�r�敪
	CreateSQL::sqlColumnInsert( &sql, 30000 + (2 * (nKizuNo+1) * nPitch) );											// FF�r���������ʒu
	CreateSQL::sqlColumnInsert( &sql, 150 );																// FF�r�Е����ʒu
	CreateSQL::sqlColumnInsert( &sql, 30 );																	// FF�r����
	CreateSQL::sqlColumnInsert( &sql, FF_Tid[nTidx] );														// �����r��
	CreateSQL::sqlColumnInsert( &sql, cFFgrade[0] - '0' );													// ����O���[�h
	if (0!=dNobiSPM) {
		CreateSQL::sqlColumnInsert(&sql, (3000 + (2 * (nKizuNo+1) * nPitch))* dNobiSPM);								// �ŏI����ʒu
	}
	else {
		CreateSQL::sqlColumnInsert(&sql, 3000 + (2 * (nKizuNo+1) * nPitch));											// �ŏI����ʒu
	}

	CreateSQL::sqlColumnTrim( &sql );					// �Ō�̃J���}����菜��
	sql += ")";

	// ���s
	if( ! clsDB.ExecuteDirect( sql ) ) {
		LOG(em_ERR), "[%s] FF�r���o�^�G���[ [%s]", my_sThreadName, cDbName );
		LOG(em_ERR), "[%s] SQL [%s]", my_sThreadName, sql );
		syslog(SYSNO_DB_EXECUTEDIRECT_ERR, "[%s]", cDbName);
		return false;
	} else {
		LOG(em_MSG), "[%s] FF�r���o�^ [�r�����Ǘ�No = %s] �rNo=%d",
			my_sThreadName, cKizukenNo, nKizuNo+1 );
		return true;
	}
	

}

//------------------------------------------
// �O�H���r���o�^
//------------------------------------------
bool TestMaeManager::SetMaeDefectInf(MAE_DEFECT_IMAGE_INF* typData)
{
	//// ODBC�ڑ��N���X �C���X�^���X����
	OdbcBase clsDB;
	if( ! clsDB.Connect(TASKINI_NAME)) {
		// �f�[�^�x�[�X�ڑ��G���[
		LOG(em_ERR), "[%s] DB�ڑ��G���[", my_sThreadName);
		return false;
	}
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
	CreateSQL::sqlColumnTrim( &sql );											// �Ō�̃J���}����菜��
	sql += ")";

	// ���s
	if( ! clsDB.ExecuteDirect( sql ) ) {
		LOG(em_ERR), "[%s] �r���o�^�G���[ [%s]", my_sThreadName, cDbName );
		LOG(em_ERR), "[%s] SQL [%s]", my_sThreadName, sql );
		syslog(SYSNO_DB_EXECUTEDIRECT_ERR, "[%s]", cDbName);
		return false;
	}
	
	return true;
}

//------------------------------------------
// �O�r�摜��DB�ɓo�^
//------------------------------------------
bool TestMaeManager::InsertDefectImg(MAE_DEFECT_IMAGE_INF* typData)
{
	//// ODBC�ڑ��N���X �C���X�^���X����
	OdbcBase clsDB;
	if( ! clsDB.Connect(TASKINI_NAME)) {
		// �f�[�^�x�[�X�ڑ��G���[
		LOG(em_ERR), "[%s] DB�ڑ��G���[", my_sThreadName);
		return false;
	}
	CString sql;					// SQL��
	CString cDbName;
	cDbName = DB_MAE_DEFECT_IMG;


	////// �O�H���r���
	// �w�b�_�[��
	sql = "INSERT INTO ";
	sql += cDbName;
	sql += " VALUES(";	

	// �f�[�^��
	CreateSQL::sqlColumnInsert( &sql, typData->cKizuKenNo, SIZE_KIZUKEN_NO );	// �r���Ǘ�No 
	CreateSQL::sqlColumnInsert( &sql, typData->nKizuNo );						// �rNo
	for( int ii = 0; ii< MAX_MAE_IMAGE; ii++ ) {
		if( 0 < typData->nSize[ii] ) {
			sql += " ?,";
		} else {
			sql += " NULL,";
		}
	}
	CreateSQL::sqlColumnTrim( &sql );									// �Ō�̃J���}����菜��
	sql += ")";

	// ���s
	if( ! clsDB.SqlBinalyWrite( sql, typData->nCnt, typData->nSize, (LPVOID*)typData->pImg) ) {
		LOG(em_ERR), "[%s] �摜�o�^���s", my_sThreadName );
		LOG(em_ERR), "[%s] SQL [%s]", my_sThreadName, sql );
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
// �O�r����DB�ɓo�^
//------------------------------------------
bool TestMaeManager::CreateDefectInf(char const* cKizukenNo, int nKizuNo, int ou, int nKouteiID )
{
	MAE_DEFECT_IMAGE_INF typData;
	int nGazoCnt = 1;
	int nPitch = 5000;

	memcpy(typData.cKizuKenNo , cKizukenNo , SIZE_KIZUKEN_NO);
	memcpy(typData.cPreKizuKenNo , cKizukenNo , SIZE_KIZUKEN_NO);
	typData.nKizuNo = nKizuNo + 1;
	typData.nKouteiSyubetu = nKouteiID;
	typData.nOuDiv = ou ;
	typData.typDefectData.cycle_div = 0;
	typData.typDefectData.y = (float)(3000 + (5 * (nKizuNo+1) * nPitch));
	typData.typDefectData.dsws = 1;
	typData.typDefectData.ds = 300;
	typData.typDefectData.ws = 200;
	typData.typDefectData.speed = 300;
	typData.typDefectData.pitch = 0;
	typData.typDefectData.roolkei = 0;
	typData.typDefectData.kizu_wid = 30;
	typData.typDefectData.kizu_len = 150;
	typData.typDefectData.w_res = 1.0;
	typData.typDefectData.h_res = 1.0;
	typData.typDefectData.box_x_min = 30;
	typData.typDefectData.box_x_max = 130;
	typData.typDefectData.box_y_min = 50;
	typData.typDefectData.box_y_max = 150;
	typData.typDefectData.center_x = 80;
	typData.typDefectData.center_y = 100;
	typData.typDefectData.w_size = 256;
	typData.typDefectData.h_size = 256;
	typData.nCnt = nGazoCnt;

	CString cfileName[MAX_CAMANGLE];		// �t�@�C���p�X
	CFile cfile[MAX_CAMANGLE];				// �t�@�C���I�u�W�F�N�g
	long nFileLen[MAX_CAMANGLE];			// �t�@�C���T�C�Y

	nFileLen[0] = typData.nSize[0] = 0;		
	nFileLen[1] = typData.nSize[1] = 0;
	nFileLen[2] = typData.nSize[2] = 0;
	nFileLen[3] = typData.nSize[3] = 0;

	for (int jj = 0; jj< nGazoCnt; jj++) {
		if (6 == nKouteiID) {
			cfileName[jj] = ".\\TCM�r.jpg";
		}
		else {
			cfileName[jj] = ".\\TCM��.jpg";
		}

		//cfileName[jj] = ".\\�r01_1.jpg";
		//cfileName[jj] = ".\\1.bmp";
		// �Ώۉ摜�̃t�@�C�����I�[�v������
		if(cfile[jj].Open(cfileName[jj], 0, 0)) {
			nFileLen[jj] = cfile[jj].GetLength();				// �Ώۉ摜�t�@�C���̃T�C�Y���擾����
			typData.nSize[jj] = nFileLen[jj];					// �摜�t�@�C���̃T�C�Y���Z�b�g����
			typData.pImg[jj] = new BYTE[typData.nSize[jj]];		// �摜�t�@�C���ǂݍ��ݗp�o�b�t�@���m�ۂ���
			memset(typData.pImg[jj], 0x00, nFileLen[jj]);		//
			cfile[jj].Read(typData.pImg[jj], nFileLen[jj]);		// �摜�t�@�C����ǂݍ���
			cfile[jj].Close();									// �摜�t�@�C�����N���[�Y����
		}
	}

	// MAE_DEFECT_IMG�ɉ摜�o�^
	InsertDefectImg(&typData);

	SetMaeDefectInf(&typData);

	// �J��
	for( int ii = 0; ii < 1; ii++ ) {
		SAFE_DELETE_ARRAY( typData.pImg[ii] )
	}

	return true;
}


//�O�H���R�C�����o�^
// FF�O�H���r����M�t���O��0���Z�b�g����
bool TestMaeManager::SetMaeCoilInf(char const* cKizukenNo , char const* cKeyInf,char const* cMeiNo , char const* cCoilNo)
{
	CString sql;					// SQL��
	CString cDbName;
	cDbName = DB_MAE_COIL_INF;

	char cKey[SIZE_SERIAL_FF_NO];

	//// ODBC�ڑ��N���X �C���X�^���X����
	OdbcBase clsDB;
	if( ! clsDB.Connect(TASKINI_NAME)) {
		// �f�[�^�x�[�X�ڑ��G���[
		LOG(em_ERR), "[%s] DB�ڑ��G���[", my_sThreadName);
		return false;
	}

	// ID�쐬
	KizuFunction::GetSerialNoFF(cKey);									

	////// �R�C���ʒu���
	// �w�b�_�[��
	sql = "INSERT INTO ";
	sql += cDbName;
	sql += " VALUES(";	

	// �f�[�^��
	CreateSQL::sqlColumnInsert( &sql, cKizukenNo, SIZE_KIZUKEN_NO );				// �Ǘ�No
	CreateSQL::sqlColumnAddTime(&sql, true);										// �o�^���t
	CreateSQL::sqlColumnInsert( &sql, cKeyInf, SIZE_COIL_KEY);						// �L�[���
	CreateSQL::sqlColumnInsert( &sql, cKey, SIZE_SERIAL_FF_NO );					// ID
	CreateSQL::sqlColumnInsert( &sql, 0 );											// ���Ǘ�No�t���O 0:�����Ǘ�No���Z�b�g
	CreateSQL::sqlColumnInsert( &sql, 0 );											// FF�O�H���r����M�t���O
	CreateSQL::sqlColumnInsert( &sql, KOUTEI_CODE_2TCM, SIZE_KOUTEI_CODE );			// �H���R�[�h
	CreateSQL::sqlColumnInsert( &sql, cMeiNo,  SIZE_MEI_NO );						// ����No
	CreateSQL::sqlColumnInsert( &sql, cCoilNo, SIZE_COIL_NO );						// �R�C��No
	CreateSQL::sqlColumnInsert( &sql, 0 );											// �����t�����g�[����
	CreateSQL::sqlColumnInsert( &sql, NULL, 0 );									// �≄�R�C���ԍ�
	CreateSQL::sqlColumnInsert( &sql, NULL, 0 );									// �≄�R�C���d��
	CreateSQL::sqlColumnInsert( &sql, NULL, 0 );									// �≄�R�C������
	CreateSQL::sqlColumnInsert( &sql, NULL, 0 );									// �≄��
	CreateSQL::sqlColumnInsert( &sql, NULL, 0 );									// ������MIN
	CreateSQL::sqlColumnInsert( &sql, NULL, 0 );									// ������MAX
	CreateSQL::sqlColumnInsert( &sql, NULL, 0 );									// ��_����1
	CreateSQL::sqlColumnInsert( &sql, NULL, 0 );									// �����
	CreateSQL::sqlColumnInsert( &sql, NULL, 0 );									// ��������
	CreateSQL::sqlColumnInsert( &sql, NULL, 0 );									// �ޗ���
	CreateSQL::sqlColumnInsert( &sql, NULL, 0 );									// ������MIN_����1
	CreateSQL::sqlColumnInsert( &sql, NULL, 0 );									// ������MAX_����1
	CreateSQL::sqlColumnInsert( &sql, NULL, 0 );									// ����_���b�g����_����1
	CreateSQL::sqlColumnInsert( &sql, NULL, 0 );									// ���a_����1
	CreateSQL::sqlColumnInsert( &sql, NULL, 0 );									// ��
	CreateSQL::sqlColumnInsert( &sql, NULL, 0 );									// �������a
	CreateSQL::sqlColumnInsert( &sql, NULL, 0 );									// ���f��_����1
	CreateSQL::sqlColumnInsert( &sql, NULL, 0 );									// �O�H���R�[�h
	CreateSQL::sqlColumnInsert( &sql, NULL, 0 );									// ���zRCL�Ǘ�No
	CreateSQL::sqlColumnInsert( &sql, NULL, 0 );									// ��̂ƂȂ�≄�R�C��No
	CreateSQL::sqlColumnInsert( &sql, NULL, 0 );									// �̎�J�n�ʒu����
	CreateSQL::sqlColumnInsert( &sql, NULL, 0 );									// �̎�I���ʒu����
	CreateSQL::sqlColumnInsert( &sql, NULL, 0 );									// �̎�q��
	CreateSQL::sqlColumnTrim(&sql);													// �Ō�̃J���}����菜��
	sql += ")";


	if( ! clsDB.ExecuteDirect( sql ) ) {
		LOG(em_ERR), "[%s] �O�H���R�C�����o�^�G���[ [%s]", my_sThreadName, cDbName );
		LOG(em_ERR), "[%s] SQL [%s]", my_sThreadName, sql );
		return false;
	} else {
		LOG(em_MSG), "[%s] �O�H���R�C�����o�^ [�r�����Ǘ�No = %s] [�L�[��� = %s]",
			my_sThreadName, cKizukenNo, cKeyInf );
		return true;
	}
}


// HY_DEFECT�ɑO�H���r���Ǎ��ʒm�𑗂�
void TestMaeManager::Send_Hy_MaeRead(char const* cKizukenNo)
{
	COMMON_QUE  que;
	memset(&que, 0x00, sizeof(que));
	que.nLineNo = getlineid();

	// HY_DEFECT
	que.nEventNo = FACT_HY_DEFECT_03;
	memcpy( que.fc.data , cKizukenNo, SIZE_KIZUKEN_NO); 
	send_mail(HY_DEFECT,".", &que);

}

//�C�x���g��M����
void TestMaeManager::Exec( DELIVERY_KEY* pDeli )
{
	int nDefInfoCnt = 5;
	// �O�R�C�����쐬
	SetMaeCoilInf(pDeli->cKizukenNo, pDeli->cKeyInf, pDeli->cMeiNo, pDeli->cCoilNo);

	for(int ou = 0 ; ou < 1 ; ou++ ){
		// �O���C�����쐬
		SetMaeCoilLine(pDeli->cKizukenNo, ou, 4);

		for (int ii = 0; ii < nDefInfoCnt; ii++) {
			// �OFF���쐬
			SetMaeFFInf(pDeli->cKizukenNo, ii + ou * nDefInfoCnt, ou, pDeli->dNobiSPM, 4);
			// �O�r���쐬
			CreateDefectInf(pDeli->cKizukenNo, ii + ou*nDefInfoCnt, ou, 4);
		}
		// �O���C�����쐬
		SetMaeCoilLine(pDeli->cKizukenNo, ou, 6);

		for (int ii = 0; ii < nDefInfoCnt; ii++) {
			// �OFF���쐬
			SetMaeFFInf(pDeli->cKizukenNo, ii + ou * nDefInfoCnt + nDefInfoCnt, ou, pDeli->dNobiSPM, 6);
			// �O�r���쐬
			CreateDefectInf(pDeli->cKizukenNo, ii + ou*nDefInfoCnt + nDefInfoCnt, ou, 6);
		}
	}

	Send_Hy_MaeRead(pDeli->cKizukenNo);
}