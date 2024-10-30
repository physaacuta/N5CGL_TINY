#include "StdAfx.h"
#include "DbBaseManager.h"

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
DbBaseManager::DbBaseManager(const char *cThreadName) :
ThreadManager(cThreadName),
m_nJpgQuality(80),
m_bQueState(false),
m_bBcpState(false),
m_bDeleteDbMode(true),
mque_AddDB(MAX_ADDDBCOUNT),
gque_AddSQLData(MAX_SQLCOUNT)
{
	// ������
	for(int ii=0; ii<THREAD_DBSAVE; ii++) mcls_pDbs[ii] = NULL;

	// �V�O�i��������
	m_evTimeBcpAutoCommit  = CreateWaitableTimer(NULL, FALSE, NULL);
	m_evTimeBcpStatusCheck = CreateWaitableTimer(NULL, FALSE, NULL);

	// ���ѕۑ��X�L�b�v
	for(int ii=0; ii<NUM_MEN; ii++)	{
		m_nSaveSkipId[ii]		= 0;
		m_bSaveSkipState[ii]	= false;
		m_evNowSaveSkip[ii]		= CreateEvent(NULL, FALSE, FALSE, NULL);
		m_evTimeSaveSkip[ii]	= CreateWaitableTimer(NULL, FALSE, NULL);
	}
}

//------------------------------------------
// �f�X�g���N�^
//------------------------------------------
DbBaseManager::~DbBaseManager(void)
{
	// �O�̂���
	Free();

	// �J��
	CloseHandle(m_evTimeBcpAutoCommit);
	CloseHandle(m_evTimeBcpStatusCheck);
	for(int ii=0; ii<NUM_MEN; ii++)	{
		CloseHandle(m_evNowSaveSkip[ii]);
		CloseHandle(m_evTimeSaveSkip[ii]);
	}
}

// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ��{

//------------------------------------------
// �֐�������
//------------------------------------------
void DbBaseManager::Alloc()
{
	//// DB�o�^�N���X����
	for(int ii=0; ii<THREAD_DBSAVE; ii++) {
		mcls_pDbs[ii] = new DBSaveManager( ii, &mque_AddDB );
		mcls_pDbs[ii]->SetLogMgr( mcls_pLog );
		mcls_pDbs[ii]->SetReTry(true);			// SQL���s���s�����g���C���� 
		mcls_pDbs[ii]->Start();
	}


	// ����́A�p����Œ�`���邱��
	//// �擪�̃C���X�^���X�̂� DB�폜�Ǘ��ɒʒm����Ƃ���
	//mcls_pDbs[0]->SetSendDbManager(true);
		
	//// �o�b�t�@������
	memset(m_SqlInf, 0x00, sizeof(m_SqlInf));

	//// ������^�C�}�[����
	__int64 ts = (__int64)TIME_BCP_STATUS_CHECK * -10000;
	SetWaitableTimer(m_evTimeBcpStatusCheck, (LARGE_INTEGER*)&ts, TIME_BCP_STATUS_CHECK, NULL, NULL, FALSE);


	//// �p����ɒʒm
	OnAlloc();
}

//------------------------------------------
// �֐��J��
//------------------------------------------
void DbBaseManager::Free()
{
	//// DB�o�^�N���X����
	for(int ii=0; ii<THREAD_DBSAVE; ii++) {
		if(NULL == mcls_pDbs[ii]) continue;

		mcls_pDbs[ii]->Stop(30000);
		SAFE_DELETE( mcls_pDbs[ii] );
	}

	//// ������^�C�}�[��~
	CancelWaitableTimer(m_evTimeBcpStatusCheck);

	//// �p����ɒʒm
	OnFree();
}

//------------------------------------------
// ���Z�b�g
//------------------------------------------
void DbBaseManager::ReSetMaxCount()
{
	gque_AddSQLData.ReSetMaxCount();
	mque_AddDB.ReSetMaxCount();

	//// ���Ԍv������
	mcls_Time.InitAve();
	for(int ii=0; ii<THREAD_DBSAVE; ii++) {
		if(NULL == mcls_pDbs[ii]) continue;

		mcls_pDbs[ii]->InitTime();
	}
	OnReSetMaxCount();
}

//------------------------------------------
// DB�폜�Ǘ��ɒʒm
// bool bWrite	true:�������ݗD��
//------------------------------------------
void DbBaseManager::Send_To_DbManage(bool bWrite)
{
	if(  m_bDeleteDbMode == bWrite) return ;

	m_bDeleteDbMode = bWrite;

	// ���M
	COMMON_QUE que;
	que.nEventNo = FACT_TI_DELETE_01;
	que.nLineNo = LINE_ID;				
	que.fl.data[0] = (bWrite ? 1 : 0 );	// 0:�폜�D�� 1:�����ݗD��
	send_mail(TI_DELETE, ".", &que);
}


// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ���׌��R�t���p

//------------------------------------------
// �o�b�t�@�؂�ւ�
// const char* cKizukenNo �Ǘ�No
//------------------------------------------
void DbBaseManager::ChangeSQLInf(const char* cKizukenNo)
{
	int ii;

	//// �z���g�ɓ������̂��Ȃ����B��������s����邽�ߑΉ�
	if( -1 != GetRecSQLInf(cKizukenNo) ) return;

	//// �o�b�t�@���R�[�h�ړ�
	for(ii=SIZE_SQLINF_NUM-1; ii>0; ii--) {
		memcpy(&m_SqlInf[ii], &m_SqlInf[ii-1], sizeof(TYP_SQL_INF));
	}
	//// �擪������
	memset(&m_SqlInf[0], 0x00, sizeof(TYP_SQL_INF));
	
	//// �Ǘ�No�Z�b�g
	memcpy(m_SqlInf[0].cKizukenNo, cKizukenNo, SIZE_KIZUKEN_NO );
}

//------------------------------------------
// �o�b�t�@���R�[�h����
// const char* cKizukenNo �Ǘ�No
//------------------------------------------
int DbBaseManager::GetRecSQLInf(const char* cKizukenNo)
{
	int ii;
	// ����
	for(ii=0; ii<SIZE_SQLINF_NUM; ii++) {
		if(0 == strcmp(m_SqlInf[ii].cKizukenNo, cKizukenNo) ) {
			return ii;
		}
	}
	// �����o
	return -1;
}


// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// BCP�֘A

//------------------------------------------
// BCP�N���X �R�l�N�g
// BCPBase* cls_bcp BCP�N���X
// char const* pTableName �ڑ�����e�[�u������
//------------------------------------------
bool DbBaseManager::BCP_Connect(BCPBase* cls_bcp, char const* pTableName)
{
	//// ���ɃR�l�N�g��ԁH
	if( cls_bcp->IsConnect() ) return true;

	//// DB�ڑ�
	if( ! cls_bcp->Connect(TASKINI_NAME) ) {
		LOG(em_ERR), "[%s] DB �R�l�N�g���s", my_sThreadName);
		return false;
	}

	//// BCP�C�j�V����
	if( !cls_bcp->BCP_Init(pTableName) ) {
		LOG(em_ERR), "[%s] BCP Init���s [%s]", my_sThreadName, cls_bcp->GetTableName());
		cls_bcp->DisConnect();
		return false;
	}

	LOG(em_MSG), "[%s] BCP �R�l�N�g���� [%s]", my_sThreadName, cls_bcp->GetTableName());
	return true;
}

//------------------------------------------
// BCP�N���X �J��
// BCPBase* cls_bcp BCP�N���X
//------------------------------------------
void DbBaseManager::BCP_Dispose(BCPBase* cls_bcp)
{
	//// �R�l�N�g���ĂȂ��H
	if( ! cls_bcp->IsConnect() ) return;
	
	//// �����܂����o�^�̓z�������
	cls_bcp->BCP_Batch(); 

	//// BCP�̊J��
	if( cls_bcp->BCP_Down() ) {
		LOG(em_MSG), "[%s] BCP �J������ [%s]", my_sThreadName, cls_bcp->GetTableName());
	} else {
		LOG(em_ERR), "[%s] BCP �J�����s [%s]", my_sThreadName, cls_bcp->GetTableName());
	}
}

//------------------------------------------
// BCP�N���X �f�[�^�]��
// BCPBase* cls_bcp BCP�N���X
//------------------------------------------
bool DbBaseManager::BCP_SendRow(BCPBase* cls_bcp)
{
	//// �f�[�^ �]��
	if( ! cls_bcp->BCP_SendRow() ) {
		LOG(em_ERR), "[%s] BCP sendrow���s [%s]", my_sThreadName, cls_bcp->GetTableName());
		return false;
	}
	return true;
}

//------------------------------------------
// BCP�N���X �f�[�^������
// BCPBase* cls_bcp BCP�N���X
//------------------------------------------
bool DbBaseManager::BCP_Batch(BCPBase* cls_bcp)
{
	int retc = cls_bcp->BCP_Batch();
	if( -1 == retc ) {
		LOG(em_ERR), "[%s] BCP batch���s [%s]", my_sThreadName, cls_bcp->GetTableName());
		return false;
	} else {
		//LOG(em_MSG), "[%s] BCP batch [%s] %d��", my_sThreadName, cls_bcp->GetTableName(), retc);
	}
	return true;
}
//------------------------------------------
// BCP�N���X �o�C���h�f�[�^�T�C�Y�w�� (varbinary�o�^���Ȃ����ۂ́A�b��΍��p)
// BCPBase* cls_bcp BCP�N���X
// int coulmn �e�[�u���̃J�����ʒu (1�I���W��)
// long nSize �T�C�Y
//------------------------------------------
bool DbBaseManager::BCP_ColLen(BCPBase* cls_bcp, int coulmn, long nSize)
{
	//// �o�C���h�f�[�^�T�C�Y�w��
	if( ! cls_bcp->BCP_ColLen(coulmn, nSize) ) {
		LOG(em_ERR), "[%s] BCP collen���s [%s]", my_sThreadName, cls_bcp->GetTableName());
		return false;
	}
	return true;
}

//------------------------------------------
// BCP�����R�~�b�g�^�C�}�[�J�n
//------------------------------------------
void DbBaseManager::BCP_AutoCommitTimerOn()
{
	// �����R�~�b�g�p (�P��̂�)
	__int64 ts = (__int64)TIME_BCP_AUTO_COMMIT * -10000;
	SetWaitableTimer(m_evTimeBcpAutoCommit, (LARGE_INTEGER*)&ts, 0, NULL, NULL, FALSE);
}

// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// �X���b�h����


//------------------------------------------
// �X���b�h�J�n�O���� (�X���b�h���s����(�V�O�i���Z�b�g�O)�ɃR�[���o�b�N)
//------------------------------------------
int DbBaseManager::ThreadFirst()
{
	//// ���O����

	//// �C�x���g�ݒ�
	AddEventNum(2, m_evNowSaveSkip );
	AddEventNum(2, m_evTimeSaveSkip );				// ��ʓo�^���ł��A�����ɔ��f�����������߁A�L���[�����O

	AddEventNum(1, &gque_AddSQLData.g_evSem);
	AddEventNum(1, &m_evTimeBcpAutoCommit);
	AddEventNum(1, &m_evTimeBcpStatusCheck);

	//// �C�x���g�ǉ�
	ThreadFirst_AddHandle();

	return 0;
}

//------------------------------------------
// �X���b�h�I���O���� (�I���V�O�i����ɃR�[���o�b�N)
//------------------------------------------
int DbBaseManager::ThreadLast()
{
	return 0;
}

//------------------------------------------
// �X���b�h�C�x���g����
// int nEventNo �C�x���gNo (0�I���W��)
//------------------------------------------
void DbBaseManager::ThreadEvent(int nEventNo)
{
	DbBaseManager::DeliveryDB* pDeli;		// DB�o�^�˗��f�[�^

	////// �V�O�i���̏���
	enum {	EV_NOW_SAVE_SKIP_O = 0,			// ���ѕۑ��x�ꔭ�� �\
			EV_NOW_SAVE_SKIP_U,				// ���ѕۑ��x�ꔭ�� ��
			EV_TIME_SAVE_SKIP_O,			// ���ѕۑ��x��I�� �\
			EV_TIME_SAVE_SKIP_U,			// ���ѕۑ��x��I�� ��

			EV_QUE,							// DB�o�^�˗�
			EV_TIME_BCP_AUTO_COMMIT,		// BCP�����R�~�b�g�p
			EV_TIME_BCP_STATUS_CHECK,		// BCP��ԃ`�F�b�N
			
			EV_LAST
	};


	////// �V�O�i����������
	//======================================================
	// DB�o�^�˗�
	if(nEventNo == EV_QUE) {
		//// ���o��	
		pDeli = gque_AddSQLData.GetFromHead();

		//// �p����ɒʒm
		OnQueDb(pDeli->kubun, pDeli->data );

		//// �J��
		SAFE_DELETE(pDeli->data);
		SAFE_DELETE(pDeli);


	//======================================================
	// BCP�����R�~�b�g�p
	} else if(nEventNo == EV_TIME_BCP_AUTO_COMMIT)	{

		//// �p����ɒʒm
		OnBcpAutoCommit(true);

	//======================================================
	// BCP��ԃ`�F�b�N�p
	} else if(nEventNo == EV_TIME_BCP_STATUS_CHECK)	{

		//// �p����ɒʒm
		OnBcpStatusCheck();

	//======================================================
	// ���ѕۑ��x��I��
	} else if(nEventNo == EV_TIME_SAVE_SKIP_O || nEventNo == EV_TIME_SAVE_SKIP_U)	{
		
		SaveSkipEnd(nEventNo-EV_TIME_SAVE_SKIP_O);

	//======================================================
	// ���ѕۑ��x�ꔭ��
	} else if(nEventNo == EV_NOW_SAVE_SKIP_O || nEventNo == EV_NOW_SAVE_SKIP_U)	{
		SaveSkipStart(nEventNo-EV_NOW_SAVE_SKIP_O);

	//======================================================
	// ����ȊO�B�p����Œǉ������V�O�i��
	} else {
		//// �p����ɒʒm
		OnAddSignal( nEventNo - EV_LAST );
	}
}


// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ���ѕۑ��X�L�b�v
//------------------------------------------
// ���ѕۑ��X�L�b�v �J�n
// int nMen �\��
//------------------------------------------
void DbBaseManager::SaveSkipStart(int nMen)
{
	// ���ɃX�L�b�v���ł���� �@���Ԃɓo�^�͂��Ȃ�
	if( ! m_bSaveSkipState[nMen] ) {	
		// �@���ԃZ�b�g
		m_bSaveSkipState[nMen] = true;
		mcls_pStatus->SetStatusQue(m_nSaveSkipId[nMen], false, false);		// �L�QGr�X�L�b�v
		mcls_pStatus->SetStatusRead();										// �񓯊������ǂݍ��݃V�O�i���Z�b�g (�����ɏ�Ԃ�ʒm����������)
	}

	// �O�����J�V�O�i��
	__int64 ts = TIME_SAVE_SKIP_WAIT * -10000;
	SetWaitableTimer(m_evTimeSaveSkip[nMen], (LARGE_INTEGER*)&ts, 0, NULL, NULL, FALSE);
}

//------------------------------------------
// ���ѕۑ��X�L�b�v ����
// int nMen �\��
//------------------------------------------
void DbBaseManager::SaveSkipEnd(int nMen)
{
	m_bSaveSkipState[nMen] = false;
	mcls_pStatus->SetStatusQue(m_nSaveSkipId[nMen], true);	// �L�QGr�X�L�b�v
	mcls_pStatus->SetStatusRead();							// �񓯊������ǂݍ��݃V�O�i���Z�b�g (�����ɏ�Ԃ�ʒm����������)
}

//------------------------------------------
// �O�����ѕۑ��X�L�b�v�� �� �폜�����Z
// char const* cKizukenNo
// int nMen �\��
// int nLowGrFlg ���QGr�t���O (1:���Q�rGr  0:�L�Q�rGr)
//------------------------------------------
void DbBaseManager::SetSaveSkipAdd(char const* cKizukenNo, int nMen, int nLowGrFlg)
{
	int infRec  = GetRecSQLInf(cKizukenNo);
	if( -1 == infRec ) return;

	// ���Z
	if( 1 == nLowGrFlg ) m_SqlInf[infRec].nLowGrDeleCnt[nMen] ++;
	else				 m_SqlInf[infRec].nDeleCnt[nMen] ++;
	m_SqlInf[infRec].nTotalCnt[nMen] ++;
}

//==========================================
// �r���ѕۑ� �t���[����
//------------------------------------------
// char const* cKizukenNo      : �Ǘ�No
// int         nRec	           : �z��ʒu 0�I���W��
// int         nMen            : �\��
// int         nLowGrFlg       : ���QGr�t���O (1:���Q�rGr  0:�L�Q�rGr)
// int         nFrameDefectMax : 1�t���[�����ōő��r (0:�ő��r����Ȃ� 1:�ő��r)
// �ߒl        int             : -1:�ۑ�NG   0:�ۑ�OK   9:�摜�ۑ������o�[�W����
//==========================================
int DbBaseManager::SaveDefectCheck(char const* cKizukenNo, int nMen, int nLowGrFlg, int nFrameDefectMax)
{
	int nRec = GetRecSQLInf(cKizukenNo);
	if( -1 == nRec ) return -1;

	return SaveDefectCheck(nRec, nMen, nLowGrFlg, nFrameDefectMax);
}
int DbBaseManager::SaveDefectCheck(int nRec, int nMen, int nLowGrFlg, int nFrameDefectMax)
{
	if( 0 > nRec || SIZE_SQLINF_NUM <= nRec ) return -1;

	//======================================================
	// �o�^�L���[�̋󂫐��ɂ�� �o�^����
	int nQueFreeCntSQL = gque_AddSQLData.GetFreeCount();		// SQL�o�^�L���[
	int nQueFreeCntDB  = mque_AddDB.GetFreeCount();				// DB�o�^�L���[
	double dRateSQL = ((double)nQueFreeCntSQL / MAX_SQLCOUNT) * 100;
	double dRateDB  = ((double)nQueFreeCntDB / MAX_ADDDBCOUNT) * 100;

	//// �����Ԃɍ���Ȃ��ׁA�L�QGr�ł��j��
	// �X�L�b�v�I��
	if( m_bQueState && (RATE_SKIP_END_DB <= dRateDB && RATE_SKIP_END_SQL <= dRateSQL) ){
		m_bQueState = false;
		LOG(em_WAR), "[%s] �r���єj���I���B�L�Q�j��=%d:%d QUE��[SQL����=%d, DB�o�^=%d]", my_sThreadName, m_SqlInf[nRec].nDeleCnt[0], m_SqlInf[nRec].nDeleCnt[1], nQueFreeCntSQL, nQueFreeCntDB );

		gque_AddSQLData.ReSetMaxCount();
		mque_AddDB.ReSetMaxCount(); 
	}
	// �X�L�b�v�J�n
	if(m_bQueState || RATE_SKIP_START_DB > dRateDB || RATE_SKIP_START_SQL > dRateSQL) {
		// �����Ԃɍ���Ȃ�
		if( ! m_bQueState ) {
			m_bQueState = true;
			LOG(em_WAR), "[%s] �o�^�Ԃɍ���Ȃ��ׁA�r���єj�� �J�n�I�I QUE��[SQL����=%d, DB�o�^=%d]", my_sThreadName, nQueFreeCntSQL, nQueFreeCntDB );
		}

		// �X�L�b�v���ł��L�Q�͋ɗ͎c�������΍�
		if( RATE_SKIP_START_DB < dRateDB || RATE_SKIP_START_SQL < dRateSQL ) {	// �X�L�b�v���̎኱�]�T������ꍇ�A�L�Q�ȍő��r�݂͕̂ۑ�
			if( (0 == nLowGrFlg && 1 == nFrameDefectMax) ){
#ifdef DEBUG_PRINT_SAVEDEFCHK
				LOG(em_FIL), "[%s] �X�L�b�v�������A�L�Q�r�Ȃ̂Ŏc�� [LowFlg=%d]QUE��[SQL����=%.2f��, DB�o�^=%.2f��]", my_sThreadName, nLowGrFlg, dRateSQL, dRateDB );
#endif
				return 0;
			}
		}

		// �X�L�b�v
		if( 1 == nLowGrFlg ) {
			m_SqlInf[nRec].nLowGrDeleCnt[nMen] ++;
#ifdef DEBUG_PRINT_SAVEDEFCHK
			LOG(em_FIL), "[%s] ���Q�r�Ȃ̂ŃX�L�b�v [LorFlg=%d][�폜��:%d]QUE��[SQL����=%.2f��, DB�o�^=%.2f��]", my_sThreadName, nLowGrFlg, m_SqlInf[nRec].nLowGrDeleCnt[nMen], dRateSQL, dRateDB );
#endif
		} else {
			// �L�Q�O���[�h
			m_SqlInf[nRec].nDeleCnt[nMen] ++;
			this->SetEvSaveSkip(nMen);		// �L�QGr�X�L�b�v�̋@���ԊǗ��J�n 
#ifdef DEBUG_PRINT_SAVEDEFCHK
			LOG(em_FIL), "[%s] �L�Q�r���X�L�b�v [LorFlg=%d][�폜��:%d]QUE��[SQL����=%.2f��, DB�o�^=%.2f��]", my_sThreadName, nLowGrFlg, m_SqlInf[nRec].nDeleCnt[nMen], dRateSQL, dRateDB );
#endif
		}

		m_SqlInf[nRec].nTotalCnt[nMen] ++;
		return -1;
	}

	//// ����n�͉摜�X�L�b�v�������Ȃ����߃R�����g
//	//// ���QGr�̎������摜�X�L�b�v�@�\
//	//// 5�r��1�r�����摜��ۑ�
//	if( 1 == nLowGrFlg && 1 != nFrameDefectMax && 
//		( RATE_LOWGR_IMGSKIP_DB > dRateDB || RATE_LOWGR_IMGSKIP_SQL > dRateSQL ) &&
//		( 0 != m_SqlInf[nRec].nTotalCnt[nMen] % 5) ) {
//#ifdef DEBUG_PRINT_SAVEDEFCHK
//		LOG(em_FIL), "[%s] ���Q�r�������摜�X�L�b�v[LorFlg=%d]QUE��[SQL����=%.2f��, DB�o�^=%.2f��]", my_sThreadName, nLowGrFlg, dRateSQL, dRateDB );
//#endif
//		
//		if( 1 == nLowGrFlg )	m_SqlInf[nRec].nImgLowGrDeleCnt[nMen]++;
//		else					m_SqlInf[nRec].nImgDeleCnt[nMen]++;
//		
//		return 9;		// �摜����
//	}


	//// �o�b�t�@�t���������ȈׁA���QGr�͔j��
	// �L�QGr�� �ɗ͔j���������Ȃ��ׁA�]�T���������Ė��QGr��j������B
	if( 1 == nLowGrFlg && 1 != nFrameDefectMax &&
		(RATE_DEST_LOWGR_DB > dRateDB || RATE_DEST_LOWGR_SQL > dRateSQL)){

		m_SqlInf[nRec].nLowGrDeleCnt[nMen] ++;
		m_SqlInf[nRec].nTotalCnt[nMen] ++;
#ifdef DEBUG_PRINT_SAVEDEFCHK
		LOG(em_FIL), "[%s] ���QGr��j��[LorFlg=%d][�폜��:%d]QUE��[SQL����=%.2f��, DB�o�^=%.2f��]", my_sThreadName, nLowGrFlg, m_SqlInf[nRec].nLowGrDeleCnt[nMen], dRateSQL, dRateDB );
#endif
		return -1;
	}


	//// �o�b�t�@�t���������ȈׁA�P�t���[���P�r�����ۑ��ɂ���
	if( 1 != nFrameDefectMax &&
		( RATE_ONLY_MAXDEF_DB > dRateDB || RATE_ONLY_MAXDEF_SQL > dRateSQL ) ) {

		m_SqlInf[nRec].nLowGrDeleCnt[nMen] ++;		// �L�QGr�̕ۑ��\���͗L�邪�A�P�t���[���ɂP�R�͗L�Q�r��ۑ����Ă��邽�߁A���Q�J�E���g�̕������Z����
		m_SqlInf[nRec].nTotalCnt[nMen] ++;
#ifdef DEBUG_PRINT_SAVEDEFCHK
		LOG(em_FIL), "[%s] 1�t���[��1�r�ۑ��ׁ̈A�X�L�b�v[LorFlg=%d][�폜��:%d]QUE��[SQL����=%.2f��, DB�o�^=%.2f��]", my_sThreadName, nLowGrFlg, m_SqlInf[nRec].nLowGrDeleCnt[nMen], dRateSQL, dRateDB );
#endif
		return -1;
	}

	//// �o�b�t�@�t���������ȈׁA�L�Q�r�ȂP�t���[���P�r�����ۑ��ɂ���
	if( 1 == nLowGrFlg && 1 != nFrameDefectMax &&
		( RATE_ONLY_MAXHIGH_DB > dRateDB || RATE_ONLY_MAXHIGH_SQL > dRateSQL ) ) {

		m_SqlInf[nRec].nLowGrDeleCnt[nMen] ++;		// �L�QGr�̕ۑ��\���͗L�邪�A�P�t���[���ɂP�R�͗L�Q�r��ۑ����Ă��邽�߁A���Q�J�E���g�̕������Z����
		m_SqlInf[nRec].nTotalCnt[nMen] ++;
#ifdef DEBUG_PRINT_SAVEDEFCHK
		LOG(em_FIL), "[%s] 1�t���[��1�L�Q�r�ۑ��ׁ̈A�X�L�b�v[LorFlg=%d][�폜��:%d]QUE��[SQL����=%.2f��, DB�o�^=%.2f��]", my_sThreadName, nLowGrFlg, m_SqlInf[nRec].nLowGrDeleCnt[nMen], dRateSQL, dRateDB );
#endif
		return -1;
	}

#ifdef DEBUG_PRINT_SAVEDEFCHK
		LOG(em_FIL), "[%s] �ۑ��Ώ�[LorFlg=%d]QUE��[SQL����=%.2f��, DB�o�^=%.2f��]", my_sThreadName, nLowGrFlg, dRateSQL, dRateDB );
#endif
	return 0;
}

//==========================================
// �r���ѕۑ� ����m�F
//------------------------------------------
// char const* cKizukenNo : �Ǘ�No
// int         nRec	      : �z��ʒu 0�I���W��
// int         nMen       : �\or�� (0:�\ 1:��)
// int         nLowGrFlg  : ���QGr�t���O (1:���Q�rGr  0:�L�Q�rGr)
// �ߒl        int        : �m�F���� (-1:�ۑ�NG 0:�ۑ�OK 9:�摜�̂ݕۑ�)
//------------------------------------------
int DbBaseManager::SaveMaxCheck(char const* cKizukenNo, int nMen, int nLowGrFlg)
{
	int nRec = GetRecSQLInf(cKizukenNo);
	if( -1 == nRec ) return -1;

	return SaveMaxCheck(nRec, nMen, nLowGrFlg);
}
int DbBaseManager::SaveMaxCheck(int nRec, int nMen, int nLowGrFlg)
{
	if( 0 > nRec || SIZE_SQLINF_NUM <= nRec ) return -1;

	//======================================================
	// �ۑ��r���`�F�b�N

	// �ő�ۑ������ɓ��B
	if( MAX_DETECT_SAVE <= (m_SqlInf[nRec].nSaveCnt[nMen]) ){
		if( !m_SqlInf[nRec].bMaxSave[nMen] ){
			m_SqlInf[nRec].bMaxSave[nMen] = true;

			if( ! m_SqlInf[nRec].bResult ) {			// �x���œ����Ă����ꍇ�̑΍�
				LOG(em_WAR), "[%s] <%s> %s �ۑ��r������", my_sThreadName, DivNameManager::GetTorB(nMen), m_SqlInf[nRec].cKizukenNo );
				syslog(284, "[<%s> %s]", DivNameManager::GetTorB(nMen), m_SqlInf[nRec].cKizukenNo);
				mcls_pStatus->SetStatusQue("DB_SAVE_MAX_%d_ID", nMen+1, 1, false);	// �������ѕۑ����
			}
		}
		if( 1 == nLowGrFlg ) m_SqlInf[nRec].nLowGrDeleCnt[nMen] ++;
		else				 m_SqlInf[nRec].nDeleCnt[nMen] ++;
		m_SqlInf[nRec].nTotalCnt[nMen] ++;
#ifdef DEBUG_PRINT_SAVEMAXCHK
		LOG(em_FIL), "[�ۑ��t���[] �r�ۑ��Ȃ� �ő�ۑ�������B[LowFlg:%d][�ۑ��r��:%d][�ۑ��摜��:%d/%d]", nLowGrFlg, m_SqlInf[nRec].nSaveCnt[nMen], m_SqlInf[nRec].nSaveImgCnt[nMen], m_SqlInf[nRec].nSaveImgAllCnt[nMen]);
#endif
		return -1;

	// ���QGr�̍ő�ۑ������ɓ��B
	}
	else if (MAX_DETECT_LOWGR_SAVE <= m_SqlInf[nRec].nSaveCnt[nMen] && 1 == nLowGrFlg) {
		// ���QGr�̏ꍇ�́A�j��
		m_SqlInf[nRec].nLowGrDeleCnt[nMen] ++;
		m_SqlInf[nRec].nTotalCnt[nMen] ++;
#ifdef DEBUG_PRINT_SAVEMAXCHK
		LOG(em_FIL), "[�ۑ��t���[] �r�ۑ��Ȃ� ���Q�ۑ�������B[LowFlg:%d][�ۑ��r��:%d][�ۑ��摜��:%d/%d]", nLowGrFlg, m_SqlInf[nRec].nSaveCnt[nMen], m_SqlInf[nRec].nSaveImgCnt[nMen], m_SqlInf[nRec].nSaveImgAllCnt[nMen]);
#endif
		return -1;
	}
//
//	// �摜�̍ő�ۑ������ɓ��B
//	} else if(MAX_DETECT_IMAGE_SAVE <= m_SqlInf[nRec].nSaveImgCnt[nMen] ) {
//		// �r���̂ݓo�^
//#ifdef DEBUG_PRINT_SAVEMAXCHK
//		LOG(em_FIL), "[�ۑ��t���[] �r���̂� �摜�ۑ�������B[LowFlg:%d / MarkFlg:%d][�ۑ��r��:%d][�ۑ��摜��:%d/%d]", nLowGrFlg, nMarkFlg, m_SqlInf[nRec].nSaveCnt[nMen], m_SqlInf[nRec].nSaveImgCnt[nMen], m_SqlInf[nRec].nSaveImgAllCnt[nMen]);
//#endif
//
//		if( 1 == nLowGrFlg )	m_SqlInf[nRec].nImgLowGrDeleCnt[nMen]++;
//		else					m_SqlInf[nRec].nImgDeleCnt[nMen]++;
//
//		return 9;
//
//	// �ۑ����r������茏���ɓ��B
//	} else if(MAX_DEFECT_LOWER_IMG_SAVE <= m_SqlInf[nRec].nSaveCnt[nMen] && 1 == nLowGrFlg ){
//		// ���QGr���r���̂ݓo�^
//#ifdef DEBUG_PRINT_SAVEMAXCHK
//		LOG(em_FIL), "[�ۑ��t���[] �r���̂� ���Q�摜�ۑ�������B[LowFlg:%d][�ۑ��r��:%d][�ۑ��摜��:%d/%d]", nLowGrFlg, m_SqlInf[nRec].nSaveCnt[nMen], m_SqlInf[nRec].nSaveImgCnt[nMen], m_SqlInf[nRec].nSaveImgAllCnt[nMen]);
//#endif
//
//		m_SqlInf[nRec].nImgLowGrDeleCnt[nMen]++;
//		return 9;
//	}
#ifdef DEBUG_PRINT_SAVEMAXCHK
	LOG(em_FIL), "[�ۑ��t���[] �r�摜�ۑ�[LowFlg:%d][�ۑ��r��:%d][�ۑ��摜��:%d/%d]", nLowGrFlg, m_SqlInf[nRec].nSaveCnt[nMen], m_SqlInf[nRec].nSaveImgCnt[nMen], m_SqlInf[nRec].nSaveImgAllCnt[nMen]);
#endif
	return 0;
}
//------------------------------------------
// �r���ѕۑ� ������Z�b�g
// int nRec	�z��ʒu 0�I���W��
//------------------------------------------
void DbBaseManager::SaveMaxReset(int nRec)
{
	//// ������B���Ă��H
	for(int ii=0; ii<NUM_MEN; ii++) {
		if( true == m_SqlInf[nRec].bMaxSave[ii] ) {
			mcls_pStatus->SetStatusQue("DB_SAVE_MAX_%d_ID", ii+1, 0, false);	// �������ѕۑ���� ����
		}
	}
}

//------------------------------------------
// �r�摜�ۑ������Z�b�g
// char const*	cKizukenNo	: �Ǘ�No
// int			nTorB		: �\���敪(0:�\ 1:�� 2:����)
// �ߒl			int			: ���Z�b�g���� (-1:���Z�b�gNG 0:���Z�b�gOK)
//------------------------------------------
int DbBaseManager::ResetInf_SaveImgCnt(char const* cKizukenNo, int nTorB)
{
	int nRec = GetRecSQLInf(cKizukenNo);
	if( -1 == nRec ) return -1;

	if(0 == nTorB)						m_SqlInf[nRec].nSaveImgCnt[0] = 0;
	else if(1 == nTorB)					m_SqlInf[nRec].nSaveImgCnt[1] = 0;
	else {
		for(int ii=0; ii<NUM_MEN; ii++) m_SqlInf[nRec].nSaveImgCnt[ii] = 0;
	}
	return 0;
}
