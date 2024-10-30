#include "StdAfx.h"
#include "MaeManager.h"

// �������[���[�N���o ----->>>
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif
// <<<------------------------

#include "MainInstance.h"

//------------------------------------------
// �R���X�g���N�^
//------------------------------------------
MaeManager::MaeManager(void) :
ThreadManager("MaeMgr")
{
	m_pCoilInf = NULL;
}

//------------------------------------------
// �f�X�g���N�^
//------------------------------------------
MaeManager::~MaeManager(void)
{
	delete m_pCoilInf;
}


//------------------------------------------
// �X���b�h�J�n�O���� (�X���b�h���s����(�V�O�i���Z�b�g�O)�ɃR�[���o�b�N)
//------------------------------------------
int MaeManager::ThreadFirst()
{
	//// ���O����
	m_pCoilInf = new COIL_INF;

	//// �C�x���g�ݒ�
	HANDLE hArray[] = { mque_MaeRequest.g_evSem 
						};
	SetEventNum(sizeof(hArray)/sizeof(HANDLE), hArray);
	return 0;
}

//------------------------------------------
// �X���b�h�I���O���� (�I���V�O�i����ɃR�[���o�b�N)
//------------------------------------------
int MaeManager::ThreadLast()
{
	return 0;
}

//------------------------------------------
// �X���b�h�C�x���g����
// int nEventNo �C�x���gNo (0�I���W��)
//------------------------------------------
void MaeManager::ThreadEvent(int nEventNo)
{
	////// �V�O�i���̏���
	enum {	EV_QUR = 0,						// �����˗�
	};

	////// �V�O�i����������
	switch (nEventNo) {

//-----------------------------------------------------------------------------------------------
	case EV_QUR: 					// �����˗�
		if(true) {
			TYP_MAE_REQUEST* pDeli = mque_MaeRequest.GetFromHead(); 
			em_EXEC_MODE emMode = pDeli->emMode;
			switch (emMode) {
			//------------------------------------------------------------------------------------
			// �S�O�H�����X�V
			case UPDATE_MAE_REQUEST:
				Exec_MaeRequest(pDeli);

				// �\��PC�ɒʒm(�O�H�����̗L���Ɋւ�炸�ʒm)
				MainInstance::Send_HyDefect_MaeRead(pDeli->cKizukenNo);
				break;
			//------------------------------------------------------------------------------------
			// �t���O�̂ݍX�V
			case UPDATE_FRAG_ONLY:
				Exec_FragOnly(pDeli);
				break;
			default:
				break;
			}

			delete pDeli;
		}
		return;

	
//-----------------------------------------------------------------------------------------------
	default :
		// ���肦�Ȃ��I�I
		LOG(em_ERR), "[%s] ThreadEvent EventNo NG=%d", my_sThreadName, nEventNo );
		return;
	}
}


// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// �O���A�N�Z�X
//------------------------------------------
// �R�C������M
// COIL_INF const* pCoil	�X�V�Ώۂ̃R�C�����
//------------------------------------------
void MaeManager::RecvCoilInf(COIL_INF const* pCoil)
{
	memcpy(m_pCoilInf, pCoil, sizeof(COIL_INF));
	//// ODBC�ڑ��N���X �C���X�^���X����
	OdbcBase clsDB;
	if (!clsDB.Connect(TASKINI_NAME)) {
		// �f�[�^�x�[�X�ڑ��G���[
		LOG(em_ERR), "[%s] DB�ڑ��G���[", my_sThreadName);
		syslog(SYSNO_DB_CONNECT_ERR, "");
		return;
	}

	//================================================
	//// FF�O�H���r��� ��M / ����M�`�F�b�N
	SYSTEMTIME STime;
	GetLocalTime(&STime);
	CString sWk;
	int nFfRecvFrag = -1;
	sWk.Format( "SELECT TOP(1) FF�O�H���r����M�t���O FROM %s WHERE ����No='%4s' AND ���Ǘ�No�t���O=-1 AND �o�^���t+30 > '%d/%d/%d'",
		DB_MAE_COIL_INF, pCoil->data.cMeiNo, STime.wYear, STime.wMonth, STime.wDay);

	bool bRetc = clsDB.GetSelectKey( sWk, &nFfRecvFrag);
	if( ! bRetc ) {
		// �f�[�^���������肦��
		LOG(em_WAR), "[%s] �Ώۂ̑O�H���R�C����񖳂� [����No%4s:%s]", my_sThreadName, pCoil->data.cMeiNo, pCoil->cKizukenNo);
		//LOG(em_NON), "%s", sWk);
		return;
	}
	else if(0 != nFfRecvFrag) {
		// FF����O�H���r��񂪖���M
		LOG(em_WAR), "[%s] FF�O�H���r��񖢎�M [����No%4s:%s]", my_sThreadName, pCoil->data.cMeiNo, pCoil->cKizukenNo);
		return;
	}

	// �����܂ŗ�����ʏ�ʂ�ʒm
	AddQue_MaeRequest(pCoil, UPDATE_MAE_REQUEST);
}

//------------------------------------------
// �O�H���r��񏑍��݊����ʒm��M
// char const* cMeiNo		����No
// char const* cCoilNo		�R�C��No
//------------------------------------------
void MaeManager::RecvMaeWrite(char const* cMeiNo, char const* cCoilNo)
{
	// �擾�����O�H���r��񏑍��݊����ʒm�����R�C���������ꍇ�̋~�Ϗ��u
	if(	0 == memcmp(m_pCoilInf->data.cMeiNo,	  cMeiNo,  sizeof(SIZE_MEI_NO)) &&
		0 == memcmp(m_pCoilInf->data.cCoilNoIn, cCoilNo, sizeof(SIZE_COIL_NO))) {
		// �����܂ŗ�����ʏ�ʂ�ʒm
		AddQue_MaeRequest((const COIL_INF*)m_pCoilInf, UPDATE_MAE_REQUEST);
	}
}

//------------------------------------------
// �R�C�����я����݊����ʒm��M
// char const* cMeiNo		����No
// char const* cCoilNo		�R�C��No
//------------------------------------------
void MaeManager::RecvCoilResultWrite(COIL_INF const* pCoil)
{
	AddQue_MaeRequest((const COIL_INF*)pCoil, UPDATE_FRAG_ONLY);
}


// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// �����o�[�ϐ�
//------------------------------------------
// ���Ǘ�No�X�V �˗�
// COIL_INF const* pCoil	�X�V�Ώۂ̃R�C�����
//------------------------------------------
void MaeManager::AddQue_MaeRequest(COIL_INF const* pCoil, em_EXEC_MODE emMode)
{
	//================================================
	//// ���O����
	TYP_MAE_REQUEST *pDeli = new TYP_MAE_REQUEST;

	memcpy( pDeli->cKizukenNo, pCoil->cKizukenNo, SIZE_KIZUKEN_NO);
	memcpy( pDeli->cMeiNo, pCoil->data.cMeiNo, sizeof(pDeli->cMeiNo));
	memcpy( pDeli->cCoilNo, pCoil->data.cCoilNoIn, sizeof(pDeli->cCoilNo));
	pDeli->nNobiSPM = pCoil->data_u.CoilInf.nNobiSPM;	// SPM�L�ї�[10^2%]
	pDeli->nCutF	= pCoil->data_u.CoilInf.nCutF;		// �����ɂ�����t�����g�[�J�b�g�̎��ђ�[mm]
	pDeli->emMode	= emMode;							// �S�O�H�����X�V���[�h


	//================================================
	//// �o�^�˗�
	if( ! mque_MaeRequest.AddToTailFreeCheck(pDeli) ) {	// �o�^
		LOG(em_ERR), "[%s] �O�H����� ���r���Ǘ�No�X�V�˗��L���[�t��", my_sThreadName);
		syslog(SYSNO_QUEFULL_WAR, "[�O�H����� ���r���Ǘ�No�X�V]");
		delete pDeli;
	}
}

// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ���C������

//------------------------------------------
// ���Ǘ�No�X�V
// TYP_MAE_REQUEST const* p	�˗�
//------------------------------------------
void MaeManager::Exec_MaeRequest(TYP_MAE_REQUEST const* p)
{
	char	cKariKizukenNo[SIZE_KIZUKEN_NO];				// ���Ǘ�No
	double	dNobi = 1.0 + ((double)p->nNobiSPM / 10000.0);	// [10-2%����]
	int		nCutF = p->nCutF;								// �v���R��������炤�����[������[mm]�����̂܂܎g�p

	//// ODBC�ڑ��N���X �C���X�^���X����
	OdbcBase clsDB;
	if( ! clsDB.Connect(TASKINI_NAME)) {
		// �f�[�^�x�[�X�ڑ��G���[
		LOG(em_ERR), "[%s] DB�ڑ��G���[", my_sThreadName);
		syslog(SYSNO_DB_CONNECT_ERR, "");
		return;
	}


	//================================================
	//// ���Ǘ�No�̓���
	SYSTEMTIME STime;
	GetLocalTime(&STime);
	CString sWk;
	sWk.Format( "SELECT TOP(1) �Ǘ�No FROM %s WHERE �R�C��No='%12s' AND ����No='%4s' AND ���Ǘ�No�t���O=-1 AND �o�^���t+30 > '%d/%d/%d'",
		DB_MAE_COIL_INF, p->cCoilNo, p->cMeiNo, STime.wYear, STime.wMonth, STime.wDay);

	bool bRetc = clsDB.GetSelectKey( sWk, sizeof(cKariKizukenNo), cKariKizukenNo);
	if( ! bRetc || 0x00 == cKariKizukenNo[0] ) {
		// �f�[�^���������肦��
		LOG(em_WAR), "[%s] �Ώۂ̑O�H���R�C����񖳂� [�R�C��No%12s:����No%4s:%s]", my_sThreadName, p->cCoilNo, p->cMeiNo, p->cKizukenNo);
		return;
	}
	
	//================================================
	// �O�H�����ҏW�@5CGL�ɔ����Ӄ֓Ǝ��i���W�s�j
	// �E���Ǘ�No �� �Ǘ�No��
	// �E�����[�������Z
	// �E�L�ї� ���f

	// SQL������
	static const int nSqlCnt =5;
	CString sql[nSqlCnt];
	CString table[nSqlCnt] = {DB_MAE_DEFECT_IMG, DB_MAE_DEFECT_INF, DB_MAE_FF_INF, DB_MAE_COIL_LINE, DB_MAE_COIL_INF };
	// �O�H���r�摜�e�[�u��
	sql[0].Format( "UPDATE %s SET �Ǘ�No='%s' WHERE �Ǘ�No='%s'", DB_MAE_DEFECT_IMG, p->cKizukenNo, cKariKizukenNo);
	// �O�H���r���e�[�u��
	sql[1].Format( "UPDATE %s SET �Ǘ�No='%s' WHERE �Ǘ�No='%s'", DB_MAE_DEFECT_INF, p->cKizukenNo, cKariKizukenNo);
	// FF�O�H���r���e�[�u��
	sql[2].Format( "UPDATE %s SET �Ǘ�No='%s', "
		//"FF�r���������ʒu=(CASE WHEN FF�r���������ʒu - %d <= 0 THEN 0 ELSE FF�r���������ʒu - %d END) * %f,"
		//"�ŏI����ʒu= �ŏI����ʒu * %f "
		//DB_MAE_FF_INF, p->cKizukenNo, nCutF, nCutF, dNobi, dNobi, cKariKizukenNo);
		"�ŏI����ʒu= (CASE WHEN FF�r���������ʒu - %d <= 0 THEN 0 ELSE FF�r���������ʒu - %d END) * %f "
		"WHERE �Ǘ�No='%s'",
		DB_MAE_FF_INF, p->cKizukenNo, nCutF, nCutF, dNobi, cKariKizukenNo);
	// �O�H�����C���ʃR�C�����e�[�u��
	sql[3].Format( "UPDATE %s SET �Ǘ�No='%s' WHERE �Ǘ�No='%s'",
		DB_MAE_COIL_LINE, p->cKizukenNo, cKariKizukenNo);
	// �O�H���R�C�����e�[�u��
	sql[4].Format("UPDATE %s SET �Ǘ�No='%s', ���Ǘ�No�t���O=0, "
		"�����t�����g�[����=�����t�����g�[���� * %f, "
		"�≄�R�C������=�≄�R�C������ * %f, "
		"�̎�J�n�ʒu����=�̎�J�n�ʒu���� * %f, "
		"�̎�I���ʒu����=�̎�I���ʒu���� * %f "
		"WHERE �Ǘ�No='%s'",
		DB_MAE_COIL_INF, p->cKizukenNo, dNobi, dNobi, dNobi, dNobi, cKariKizukenNo);
	LOG(em_MSG), "[%s] ���Ǘ�No�t���ւ��J�n [%s]��[%s] �L�ї�:%.04f �t�����g�[�J�b�g��:%d", my_sThreadName, cKariKizukenNo, p->cKizukenNo, dNobi, nCutF);

	//for (int ii = 0; ii < nSqlCnt; ii++)
	//{
	//	LOG(em_MSG), "[%s] ���Ǘ�No�t���ւ� SQL%d [%s] [%s]", my_sThreadName, ii + 1, cKariKizukenNo, sql[ii]);
	//}

	//// �Ǘ�No �t���ւ�
	DWORD nUpDateRow;				// �ύX�s��
	for(int ii=0; ii<nSqlCnt; ii++) {
		// UpDate
		if( ! clsDB.ExecuteDirect(sql[ii]) ) {
			LOG(em_ERR), "[%s] ���Ǘ�No�t���ւ��ُ� [%s] [%s]", my_sThreadName, cKariKizukenNo, sql[ii]);
			syslog(SYSNO_DB_EXECUTEDIRECT_ERR, "[%s][�Ǘ�No:%s]", table[ii], cKariKizukenNo);
		} else {
			clsDB.GetNumRows(&nUpDateRow); // �ύX�s���擾

			LOG(em_MSG), "[%s] ���Ǘ�No�t���ւ� SQL%d�ڊ��� (�ύX�Ώۍs:%d)", my_sThreadName, ii+1, nUpDateRow);
		}
	}
}


//------------------------------------------
// �t���O�̂ݍX�V
// TYP_MAE_REQUEST const* p	�˗�
//------------------------------------------
void MaeManager::Exec_FragOnly(TYP_MAE_REQUEST const* p)
{
	char	cKariKizukenNo[SIZE_KIZUKEN_NO];		// ���Ǘ�No


	//// ODBC�ڑ��N���X �C���X�^���X����
	OdbcBase clsDB;
	if( ! clsDB.Connect(TASKINI_NAME)) {
		// �f�[�^�x�[�X�ڑ��G���[
		LOG(em_ERR), "[%s] DB�ڑ��G���[", my_sThreadName);
		syslog(SYSNO_DB_CONNECT_ERR, "");
		return;
	}


	//================================================
	//// ���Ǘ�No�̓���
	SYSTEMTIME STime;
	GetLocalTime(&STime);
	CString sWk;
	sWk.Format( "SELECT TOP(1) �Ǘ�No FROM %s WHERE �R�C��No='%12s' AND ����No='%4s' AND ���Ǘ�No�t���O=-1 AND �o�^���t+30 > '%d/%d/%d'",
		DB_MAE_COIL_INF, p->cCoilNo, p->cMeiNo, STime.wYear, STime.wMonth, STime.wDay);

	bool bRetc = clsDB.GetSelectKey( sWk, sizeof(cKariKizukenNo), cKariKizukenNo);
	if( ! bRetc || 0x00 == cKariKizukenNo[0] ) {
		// �f�[�^���������肦��
		LOG(em_INF), "[%s] �Ώۂ̑O�H���R�C����񖳂��i���ɍX�V�ς݁H�j [�R�C��No%12s:����No%4s:%s]", my_sThreadName, p->cCoilNo, p->cMeiNo, p->cKizukenNo);
		return;
	}
	
	//================================================
	// �O�H�����ҏW�@1CAPL�����Ӄ֓Ǝ��i���W�s�j
	// �E���Ǘ�No�t���O �� -9(�ʔ̍ς�)��

	// SQL������
	static const int nSqlCnt =1;
	CString sql[nSqlCnt];
	CString table[nSqlCnt] = { DB_MAE_COIL_LINE };
	// �O�H���R�C�����e�[�u��
	sql[0].Format("UPDATE %s SET ���Ǘ�No�t���O=-9 "
		"WHERE �Ǘ�No='%s'",
		DB_MAE_COIL_INF, cKariKizukenNo);
	LOG(em_MSG), "[%s] ���Ǘ�No�t���O�̂ݍX�V�J�n [%s]��[%s]", my_sThreadName, cKariKizukenNo, p->cKizukenNo);

	//// �Ǘ�No �t���ւ�
	DWORD nUpDateRow;				// �ύX�s��
	for(int ii=0; ii<nSqlCnt; ii++) {
		// UpDate
		if( ! clsDB.ExecuteDirect(sql[ii]) ) {
			LOG(em_ERR), "[%s] ���Ǘ�No�t���O�̂ݍX�V�ُ� [%s] [%s]", my_sThreadName, cKariKizukenNo, sql[ii]);
			syslog(SYSNO_DB_EXECUTEDIRECT_ERR, "[%s][�Ǘ�No:%s]", table[ii], cKariKizukenNo);
		} else {
			clsDB.GetNumRows(&nUpDateRow); // �ύX�s���擾

			LOG(em_MSG), "[%s] ���Ǘ�No�t���O�̂ݍX�V SQL%d�ڊ��� (�ύX�Ώۍs:%d)", my_sThreadName, ii+1, nUpDateRow);
		}
	}
}