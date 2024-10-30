#include "StdAfx.h"
#include "CoilBaseManager.h"

#include "MainInstance.h"

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
CoilBaseManager::CoilBaseManager(void) :
ThreadManager("CoilMgr")
{

//*/----> ��
#ifdef LOCAL
	m_hCoil = CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, sizeof(TO_COIL_TBL), TBL_TO_COIL_NAME);
	mtbl_pCoil =(TO_COIL_TBL*) MapViewOfFile(m_hCoil, FILE_MAP_ALL_ACCESS, 0, 0, 0);
#ifdef IS_TO_CAMERA_TBL
	m_hCam = CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, sizeof(TO_CAMERA_TBL), TBL_TO_CAMERA_NAME);
	mtbl_pCam =(TO_CAMERA_TBL*) MapViewOfFile(m_hCam, FILE_MAP_ALL_ACCESS, 0, 0, 0);
#endif
#endif
//*///<<---

	//// ���L������
	// �R�C�����e�[�u��
	mem_connect(&m_hCoil, (LPVOID *)&mtbl_pCoil, TBL_TO_COIL_NAME);
	if(m_hCoil == NULL) {
		syslog(SYSNO_MEM_OPEN, "�R�C�����e�[�u�� �A�N�Z�X���s");
		_ASSERT(FALSE);
	}



	// ������
	InitializeCriticalSection(&m_csLock);								// �N���e�B�J���Z�N�V�����I�u�W�F�N�g��������

	//// �n���h������
	m_evNextCoilKen  = CreateEvent(NULL, FALSE, FALSE, NULL);
	m_evNextCoilDsp = CreateEvent(NULL, FALSE, FALSE, NULL);
	m_evNextCoilKen_Recv = CreateEvent(NULL, FALSE, FALSE, NULL);




	// ������
	for(int ii=0; ii<NUM_MEN; ii++) {
		m_nInitScode[ii] = MAX_SCODE;
		m_nInitPcode[ii] = MAX_PCODE;	
	}

	AllDelete();

	//// �����p���R�[�_�[�pUDP����
	SetUdpAllFrame(EPC_UDP_IP, EPC_UDP_PORT_ALLFRAME);

#ifdef ENABLE_RECORDER_UDP
	for (int ii = 0; ii < NUM_MEN; ii++)
	{
		m_nScode[ii]		= MAX_SCODE;
		m_nScode_spmoff[ii] = MAX_SCODE;
	}
	memset(&m_typEpcCoil, 0x00, sizeof(TYPE_EPC_COIL));
#endif

}

//------------------------------------------
// �f�X�g���N�^
//------------------------------------------
CoilBaseManager::~CoilBaseManager(void)
{
	//// ���L�������J��
	mem_close(&m_hCoil, (LPVOID *)&mtbl_pCoil);							// �R�C�����
#ifdef IS_TO_CAMERA_TBL
	mem_close(&m_hCam,  (LPVOID *)&mtbl_pCam);							// �J���R�����
#endif
	//// �J��
	DeleteCriticalSection(&m_csLock);									// �N���e�B�J���Z�N�V�����I�u�W�F�N�g���J��

	//// �n���h�����
	CloseHandle(m_evNextCoilKen);
	CloseHandle(m_evNextCoilDsp);
	CloseHandle(m_evNextCoilKen_Recv);

	//// �S���ۑ�UDP�j��
	ReSetUdpAllFrame();
}

//------------------------------------------
// �X���b�h�J�n�O���� (�X���b�h���s����(�V�O�i���Z�b�g�O)�ɃR�[���o�b�N)
//------------------------------------------
int CoilBaseManager::ThreadFirst()
{
	//// ���O����

	//// �C�x���g�ݒ�
	this->AddEventNum(1, &m_evNextCoilKen);
	this->AddEventNum(1, &m_evNextCoilDsp);
	this->AddEventNum(1, &mcls_pTlCoilOutSt->gque_Deli.g_evSem);
	return 0;
}

//------------------------------------------
// �X���b�h�I���O���� (�I���V�O�i����ɃR�[���o�b�N)
//------------------------------------------
int CoilBaseManager::ThreadLast()
{
	return 0;
}

//------------------------------------------
// �X���b�h�C�x���g����
// int nEventNo �C�x���gNo (0�I���W��)
//------------------------------------------
void CoilBaseManager::ThreadEvent(int nEventNo)
{
	//// �V�O�i���̏���
	enum {	EV_NEXT_COIL_KEN = 0,				// �����p �R�C������M�ʒm (���R�C������M)
			EV_NEXT_COIL_DSP					// �\���p �R�C������M�ʒm (���R�C������M)
			//EV_QUE,								// �o�������R�C���P�ʂ̏�ԕύX�ʒm
			};

		//// �V�O�i����������
		switch (nEventNo) {

	//-----------------------------------------------------------------------------------------------
		case EV_NEXT_COIL_KEN:										// �����p �R�C������M�ʒm (���R�C������M)
			RecvNextCoil_Ken();
			break;
			
	//-----------------------------------------------------------------------------------------------
		case EV_NEXT_COIL_DSP:										// �\���p �R�C������M�ʒm (���R�C������M)
			RecvNextCoil_Dsp();
			break;

	//-----------------------------------------------------------------------------------------------
		default :
			// ���肦�Ȃ��I�I
			LOG(em_ERR), "[%s] ThreadEvent EventNo NG=%d", my_sThreadName, nEventNo );
			return;
		}
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// �V�[�P���X����
//==========================================
// �S�R�C����񂫂ꂢ�����ς菉����
//------------------------------------------
// �y�Ăяo�����z	
//	MainInstance:
//		��DetectWaitStart
//		��DetectStop
//  ��:
//		���R���X�g���N�^
//		��Init
//==========================================
void CoilBaseManager::AllDelete()
{
	AutoLock autolock(&m_csLock);

	// ���R�[�h��
	m_nKenRecBase = -1;					// �㗬���̌�����_�ʒu�ł̓����_���R�C���o�b�t�@�̔z��ʒu (-1:����) (���ݒl����-1����O�̃R�C�����ƂȂ�)
	m_nDspRecBase = -1;
	m_dLastSpm = 0.0f;

	// ���o�b�t�@
#ifndef TRACKING_NON_DSP_COIL
	memset( &m_CoilNext_Dsp[0], 0x00, sizeof(m_CoilNext_Dsp) );
#endif
	CoilClear(&m_CoilNext_Ken);

	for(int ii=0; ii<SIZE_COIL_BUF; ii++) {
		CoilClear(&m_CoilInf[ii]);
		m_CoilInf[ii].rec = ii;
	}

	// �e�ʒu �ʉߒ��̃R�C�����|�C���^
	for(int ii=0; ii<NUM_MEN; ii++) {
		m_pCoilKenV[ii]  = &m_CoilInf[0];
		m_pCoilKenR[ii]  = &m_CoilInf[0];
	}

#ifndef DSP_NO_TRACKING		// �g���b�L���O�\��
	m_pCoilBaseDsp = NULL;
	for(int ii=0; ii<MAX_POS_INDEX; ii++) {
		m_pCoilDsp[ii]  = NULL;
	}
#else						// ���A���^�C���\��
	m_pCoilBaseDsp = &m_CoilInf[0];
	for(int ii=0; ii<MAX_POS_INDEX; ii++) {
		m_pCoilDsp[ii]		= &m_CoilInf[0];
		m_pCoilKenUD[0][ii]	= &m_CoilInf[0];
		m_pCoilKenUD[1][ii]	= &m_CoilInf[0];
	}
#endif

	// �o���R�C�����
	for(int ii=0; ii<SIZE_COIL_OUT_BUF; ii++){
		OutCoilClear(&m_CoilOutInf);
	}

	// �V���[�J�b�g�ʒu
	memset(&m_typCut, 0x00, sizeof(m_typCut));


	// ������ʒu��ʔ��̏o�������R�C�����
	m_pCoilOutKen[0] = &m_CoilOutInf;
	m_pCoilOutKen[1] = &m_CoilOutInf;


	// �P�x
	memset(m_nBrCenter, 0x00, sizeof(m_nBrCenter));

}

//==========================================
// �R�C�����̏�����
//------------------------------------------
// �y�Ăяo�����z
//	��:
//		AllDelete�i�ΏہF�S�R�C���j
//		RecvNextCoil_Ken(�ΏہF���R�C��)
//		ChangeCoilKenBase(�ΏہF���R�C��)
//------------------------------------------
// COIL_INF*	pCoil	: ����������R�C�����
//==========================================
void CoilBaseManager::CoilClear(COIL_INF* pCoil)
{
	//// �ʏ�̏�����
	memset( pCoil, 0x00, sizeof(COIL_INF) );

	// 0�ȊO�̏����l�̓z
	
	for(int ii=0; ii<NUM_MEN; ii++) {
		for(int jj=0; jj<MAP_ROW_NUM; jj++) {
			pCoil->emKensa[ii][jj] = DIV_KENSA_NON;
			pCoil->emKiki [ii][jj] = DIV_KIKI_OK;
			for (int kk = 0; kk<SIZE_COIL_ROWVAL; kk++) pCoil->nRowVal[ii][jj][kk] = 0;

		}
		pCoil->nSetRec_St[ii]   = -1;
		pCoil->nSetRec_Last[ii] = -1;
		pCoil->nSetRec_Edge[ii] = -1;
	}
	pCoil->nLastRec_Postion		= -1;

	pCoil->nLen_Yotei			= COIL_LEN_MAX * 1000;
	pCoil->dSpmHosei			= 1.0;
	
}

//==========================================
// �o�������R�C�����̏�����
// �y�Ăяo�����z
//  ���F
//			AllDelete�i�ΏہF�S�R�C���j
//------------------------------------------
// COIL_OUT_INF* PCoil	: ����������R�C�����	
//==========================================
void CoilBaseManager::OutCoilClear(COIL_OUT_INF* pCoil)
{
	// �ʏ�̏�����
	memset(pCoil, 0x00, sizeof(COIL_OUT_INF));

}

//==========================================
// �����J�n���̏���������
//------------------------------------------
// �y�Ăяo�����z
//	MainInstance::DetectRun
//------------------------------------------
// bool			restart	: ���������ċN���t���O (true:���� false:�V�X�e���J�n��)
// const char*	cID		: �L�[���(BUF_PRO_R_COILINF��ID)
//==========================================
void CoilBaseManager::Init(bool restart)
{
	AutoLock autolock(&m_csLock);

	//// �R�C�����J��
	AllDelete();

	//// �K�v�ȏ��𐶐�
#ifndef DSP_NO_TRACKING							// �x��ĕ\��
	m_nKenRecBase = 0;
	m_nDspRecBase = -1;
#else											// �����\��
	m_nKenRecBase = 0;
	m_nDspRecBase = m_nKenRecBase;		// �����㑦���\���̂���
	m_nShaRecBase = m_nKenRecBase;
#endif

	// �ꔭ�ڂ����ʕ\���̂���
	// �����p
	m_nKenRecBase = 0;
	m_nDspRecBase = m_nKenRecBase;
	
	// �\���p
	m_pCoilBaseDsp = &m_CoilInf[m_nDspRecBase];
	for(int ii=0; ii<POS_DSP_END; ii++) {
		m_pCoilDsp[ii]  = m_pCoilBaseDsp;
	}

	//// �����_���R�C��No�𐶐�
	DummyCoilInf(m_nKenRecBase, true);

#ifdef DEBUG_LOG_OUT_CHANGE
	LOG(em_FIL), "[%s] Init()��1�ɏ�����[%d]", my_sThreadName, m_CoilOutInf[m_nShaRecBase].nCnt);
#endif

	//// �@���ԕύX
	KizuFunction::SetStatus("COIL_KEN_ID", true, false);	// �����p�R�C�����͊֌W�Ȃ��ׁ̈A���������Ă���
#ifndef TRACKING_NON_DSP_COIL
	KizuFunction::SetStatus("COIL_DSP_ID", true, false);	// �\���p�R�C�����͊֌W�Ȃ��ׁ̈A���������Ă���
#endif
	KizuFunction::SetStatus("COIL_TIMING_ERR_ID", true, false);		// �����J�n2�{�ڂ̃R�C�����Ȃ�
	if( ! restart ) {
		KizuFunction::SetStatus("COIL_INIT_ID",     false, false);	// �����J�n�P�{��
		KizuFunction::SetStatus("COIL_RESTART_ID",  true,  false);	// ���������ċN���P�{��
	} else {
		KizuFunction::SetStatus("COIL_INIT_ID",     true,  false);	// �����J�n�P�{��
		KizuFunction::SetStatus("COIL_RESTART_ID",  false, false);	// ���������ċN���P�{��
	}


	//// �X�e�[�^�X�Ǘ��N���X�ɃR�C�������Z�b�g
	mcls_pStatus->SetCoilInfStatus(&m_CoilInf[m_nKenRecBase]); 

	//// ������n�߂�R�C���� �R�C������DB�֕ۑ� (1�{�ڃR�C���Ƃ��ēo�^)
	SaveDB_CoilInf();

	//// �����J�n��{�ڂ���\��
	m_pCoilBaseDsp->bDspOk = true;

	// �Ǘ�No���Z�b�g����^�C�~���O�͈ȉ��̒ʂ�B
	// 1�{�ڃR�C���@�F�@�����J�n��
	// 2�{�ڃR�C���@�F�@�R�C������M��
	// ����āA�Ǘ�No�� 1�{�� �� 2�{�ځB
	// �����ŁA1�{�ڂ�2�{�ڂ̃R�C���̊Ǘ�No���t�ɕύX����B
	//// �������������l���R�C�����v���̏ꍇ�̂݁A2�{�ڃR�C���̊Ǘ�No�����Z�b�g
	if( 0!=strlen(m_CoilNext_Ken.cKizukenNo) ) {
		do {
			memset(m_CoilNext_Ken.cKizukenNo, 0x00, sizeof(m_CoilNext_Ken.cKizukenNo));
			KizuFunction::GetKizukenNo( m_CoilNext_Ken.cKizukenNo );
			Sleep(100);
		} while( 0==strcmp(m_CoilInf[m_nKenRecBase].cKizukenNo, m_CoilNext_Ken.cKizukenNo) );
	}

}

//==========================================
// �_�~�[�̓����_���R�C�����𐶐�
//------------------------------------------
// �y�Ăяo�����z
//  ��
//		ChangeCoilKenBase
//		Init
//------------------------------------------
// int	rec		: �Z�b�g���郌�R�[�h�ʒu (0�I���W��)
// bool init	: true:������  false:�R�C����񓙂������ꍇ
// bool isWpd	: false: �R�C����������B
//==========================================
void CoilBaseManager::DummyCoilInf(int rec, bool init, bool isWpd)
{
    SYSTEMTIME time;
    GetLocalTime(&time);

	//====================================================================================
	// ���ߎ��Ɋm�肷����
	
	if(isWpd) KizuFunction::GetKizukenNo( m_CoilInf[rec].cKizukenNo );			// �Ǘ�No
	else {
		if( 0!=strlen(m_CoilNext_Ken.cKizukenNo) ) {
			// ���R�C������M�ς݂ł���΁A�����Ǘ�No�Ƃ���
			memcpy(m_CoilInf[rec].cKizukenNo, m_CoilNext_Ken.cKizukenNo, SIZE_KIZUKEN_NO);
		} else {
			KizuFunction::GetKizukenNo( m_CoilInf[rec].cKizukenNo );			// �Ǘ�No
		}
	}

	char cWk[SIZE_SERIAL_NO];
	int size = sprintf(cWk, "%02d%02d%02d%02d%02d", 
								time.wMonth, time.wDay, time.wHour, time.wMinute, time.wSecond);

	memset(m_CoilInf[rec].data_u.CoilInf.KeyInf.cCoilNo, 0x20, sizeof(m_CoilInf[rec].data_u.CoilInf.KeyInf.cCoilNo));
	memcpy(m_CoilInf[rec].data_u.CoilInf.KeyInf.cCoilNo, cWk, size);			// �R�C��No
	strcpy(m_CoilInf[rec].cCoilKey, "DummyCoil");								// �L�[���

	m_CoilInf[rec].setting.bWalm = false;
	for (int ii = 0; ii<NUM_MEN; ii++) {
		m_CoilInf[rec].setting.bDefScode[ii] = true;
		m_CoilInf[rec].setting.bDefPcode[ii] = true;
		m_CoilInf[rec].setting.scode[ii]		= (init ? m_nInitScode[ii] : MAX_SCODE);
		m_CoilInf[rec].setting.pcode[ii]		= (init ? m_nInitPcode[ii] : MAX_PCODE);
		m_CoilInf[rec].setting.scode_spmoff[ii] = (init ? m_nInitScode[ii] : MAX_SCODE);
		m_CoilInf[rec].setting.pcode_spmoff[ii] = (init ? m_nInitPcode[ii] : MAX_PCODE);
	}
	m_CoilInf[rec].bDummeyCoil		= true;										// �_�~�[�R�C���ݒ�
	m_CoilInf[rec].bCoilInit		= init;										// �R�C�������J�n�P�{��(true:�P�{��)
	m_CoilInf[rec].nMapCol			= MAP_COL_NUM;								// �}�b�v�̕���
	m_CoilInf[rec].nStartWpdOk		= 2;										// �o�b�N�A�b�v�ɂėn�ڐؑ�(0:����WPD  1:�َ�_  2:�Q�[�g�M��)

	// ��{�R�C�����
	m_CoilInf[rec].data.nLenIn = COIL_LEN_MAX;							// �R�C����[mm]
	m_CoilInf[rec].data.nWid = COIL_WID_MAX;
	m_CoilInf[rec].data.nAtu = COIL_THICK_MAX;


	//====================================================================================
	// �v���R���R�C�������[���I�ɃZ�b�g�@�R�C����������B�o�[�W����(��O�̃R�C����񂩂���p��)
	if( ! isWpd ){

		int nPre = (0 != rec ? rec - 1 : SIZE_COIL_BUF-1);
		//// �O�R�C����������p�����
		memcpy(&m_CoilInf[rec].data_u.CoilInf, &m_CoilInf[nPre].data_u.CoilInf, sizeof(m_CoilInf[rec].data_u.CoilInf));	// �R�C�����
		memcpy(&m_CoilInf[rec].setting, &m_CoilInf[nPre].setting, sizeof(m_CoilInf[nPre].setting));	// �ӃւŎg���R�C����{���

		memcpy(m_CoilInf[rec].data_u.CoilInf.KeyInf.cCoilNo, cWk, size);							// �R�C��No�߂�

																								//// ���̑��R�C�����
		m_CoilInf[rec].nMapCol		= m_CoilInf[nPre].nMapCol;										// �}�b�v�̕���
																							
	//====================================================================================
	// �v���R���R�C�������[���I�ɃZ�b�g�@���R�C����񖳂��o�[�W����
	}else{

		int nCoilLen = 2500;			// �����̏�ł̃R�C����
		// �[���v���R���R�C�������Z�b�g
		memset(&m_CoilInf[rec].data_u.CoilInf.KeyInf.cMeiNo,	0x00,	sizeof(m_CoilInf[rec].data_u.CoilInf.KeyInf.cMeiNo));	// ����No
		memcpy(&m_CoilInf[rec].data_u.CoilInf.KeyInf.cMeiNo,	"0123",	sizeof(m_CoilInf[rec].data_u.CoilInf.KeyInf.cMeiNo));

		m_CoilInf[rec].data_u.CoilInf.nWidZai= m_CoilInf[rec].data.nWid;							// �ޗ���(����) [mm]
		m_CoilInf[rec].data_u.CoilInf.nWidTrim = (long)((double)m_CoilInf[rec].data.nWid * 0.9);	// �g������ [mm] (�g�����������́A0)
		m_CoilInf[rec].data_u.CoilInf.nAtuSei=m_CoilInf[rec].data.nAtu;								// ������ [��m]
		m_CoilInf[rec].data_u.CoilInf.nLenIn= nCoilLen;													// �R�C���� [m]
#if 0		// �����p�f�[�^
		m_CoilInf[rec].data_u.CoilInf.nAtuZai = m_CoilInf[rec].data.nAtu;							// �ޗ���
		m_CoilInf[rec].data_u.CoilInf.nWidSei = m_CoilInf[rec].data.nWid;							// �ޗ���
		m_CoilInf[rec].data_u.CoilInf.nNobiSPM = 100;												// SPM�L�ї�
		m_CoilInf[rec].data_u.CoilInf.nNobiTLV = 100;												// TLV�L�ї�
		m_CoilInf[rec].data_u.CoilInf.nCutF = nCoilLen * 0.01;										// �����ɂ�����t�����g�[�J�b�g�̎�����
		m_CoilInf[rec].data_u.CoilInf.nCutT = nCoilLen * 0.02;										// �����ɂ�����e�[���[�J�b�g�̎�����
		m_CoilInf[rec].data_u.CoilInf.nSPMRoolKei_Up = 100;											// SPM���[�N���[���a�i��j
		m_CoilInf[rec].data_u.CoilInf.nSPMRoolKei_Down = 200;										// SPM���[�N���[���a�i���j
		memcpy(m_CoilInf[rec].data_u.CoilInf.cKenKijun_O, "    ", 4);								// ����� �\
		memcpy(m_CoilInf[rec].data_u.CoilInf.cKenKijun_U, "    ", 4);								// ����� ��
		memcpy(m_CoilInf[rec].data_u.CoilInf.cKurometo_Kind, "  ", 2);								// �N�����[�g ���ю��
		memcpy(m_CoilInf[rec].data_u.CoilInf.cKurometo_Umu, "  ", 2);								// �N�����[�g ���їL��
		memcpy(m_CoilInf[rec].data_u.CoilInf.cHinCode, " ", 1);										// �i��
		memcpy(m_CoilInf[rec].data_u.CoilInf.cMekkiKind, " ", 1);									// ���b�L���
		memcpy(m_CoilInf[rec].data_u.CoilInf.cSyuZai, "  ", 2);										// ��ގ�
		memcpy(m_CoilInf[rec].data_u.CoilInf.cDifMaker, " ", 1);									// �f�B�t�@�����V�����}�[�J�[
		memcpy(m_CoilInf[rec].data_u.CoilInf.cKenban, " ", 1);										// ���敪
		memcpy(m_CoilInf[rec].data_u.CoilInf.cMekkiCycle, " ", 1);									// ���b�L�T�C�N��
		memcpy(m_CoilInf[rec].data_u.CoilInf.cInLineSPM, " ", 1);									// ���x���[ �C�����C��SPM
		memcpy(m_CoilInf[rec].data_u.CoilInf.cKagakuToyu, " ", 1);									// ���w�����h��
		memcpy(m_CoilInf[rec].data_u.CoilInf.cAtukkaSPM, " ", 1);									// �I�t���C��SPM������
		memcpy(m_CoilInf[rec].data_u.CoilInf.cMekkiKubun, " ", 1);									// ���b�L�敪
		memcpy(m_CoilInf[rec].data_u.CoilInf.cOmoteSyori1, " ", 1);									// �\�ʏ���1����
		memcpy(m_CoilInf[rec].data_u.CoilInf.cOmoteSyori2, " ", 1);									// �\�ʏ���2����
		memcpy(m_CoilInf[rec].data_u.CoilInf.cToyu, " ", 1);										// �h��
		memcpy(m_CoilInf[rec].data_u.CoilInf.cKouteiYobi, " ", 1);									// �H���L���\��
		memcpy(m_CoilInf[rec].data_u.CoilInf.cCaplSyuZai, "          ", 10);						// CAPL ��ގ�
		memcpy(m_CoilInf[rec].data_u.CoilInf.cMetuke, "        ", 8);								// �ڕt�K�i
		memcpy(m_CoilInf[rec].data_u.CoilInf.cNi, "  ", 2);											// Ni�t����
		memcpy(m_CoilInf[rec].data_u.CoilInf.cSodoTCM, "  ", 2);									// TCM���[���e�x
		memcpy(m_CoilInf[rec].data_u.CoilInf.cSpmUmu, "  ", 2);										// SPM�����L��
		memcpy(m_CoilInf[rec].data_u.CoilInf.cNextLine, "  ", 2);									// �v�掟�H���R�[�h
		memcpy(m_CoilInf[rec].data_u.CoilInf.cMuke, "ABCD                            ", 32);		// ����
		memcpy(m_CoilInf[rec].data_u.CoilInf.cYouto, "    ", 4);									// �p�r
		memcpy(m_CoilInf[rec].data_u.CoilInf.cCaplSiage, "    ", 4);								// CAPL�\�ʎd�グ
		memcpy(m_CoilInf[rec].data_u.CoilInf.cCaplSanUmu, "  ", 2);									// CAPL�_���L��
		memcpy(m_CoilInf[rec].data_u.CoilInf.cHinMode, " ", 1);										// �i��敪
		memcpy(m_CoilInf[rec].data_u.CoilInf.cGaCrKubun, " ", 1);									// GA/CR�敪
		memcpy(m_CoilInf[rec].data_u.CoilInf.cSpmJisekiUmu, "  ", 2);								// SPM�������їL��
		memcpy(m_CoilInf[rec].data_u.CoilInf.cKouteiCode, "  ", 2);									// �H���R�[�h
#endif
	}

	//====================================================================================
	// �[���v���R���R�C����񂩂�A���߂ăV�X�e�����ʃR�C�������Z�b�g
	memset(m_CoilInf[rec].data.cMeiNo, 0x00, sizeof(m_CoilInf[rec].data.cMeiNo));
	memcpy(m_CoilInf[rec].data.cMeiNo, m_CoilInf[rec].data_u.CoilInf.KeyInf.cMeiNo, SIZE_MEI_NO);				// ����No
	memset(m_CoilInf[rec].data.cCoilNoIn, 0x00, sizeof(m_CoilInf[rec].data.cCoilNoIn));
	memcpy(m_CoilInf[rec].data.cCoilNoIn, m_CoilInf[rec].data_u.CoilInf.KeyInf.cCoilNo, SIZE_COIL_NO);			// �R�C��No
	memset(m_CoilInf[rec].data.cCoilKey, 0x00, sizeof(m_CoilInf[rec].data.cCoilKey));
	memcpy(m_CoilInf[rec].data.cCoilKey, &m_CoilInf[rec].data_u.CoilInf.KeyInf.cMeiNo[0], (SIZE_MEI_NO+ SIZE_COIL_NO));		// �L�[���
	m_CoilInf[rec].data.nWid = COIL_WID_MAX;					// ��

	// ���̑����ʂ̃R�C�����
	Create_CoilInf(&m_CoilInf[rec]);
}

//==========================================
// �����_���R�C�����𐶐�
//------------------------------------------
// �y�Ăяo�����z
//  ��
//		DummyCoilInf
//		RecvNextCoil_Ken
//------------------------------------------
// COIL_INF* pCoil �Z�b�g����|�C���^
//==========================================
void CoilBaseManager::Create_CoilInf(COIL_INF* pCoil)
{
	//// ���ߎ��Ɋm��
	for(int ii=0; ii<NUM_MEN; ii++) {
		for(int jj=0; jj<NUM_HANTEI; jj++) {
			pCoil->nHtID[ii][jj] = mcls_pStatus->GetHtID(ii, jj);
		}
	}
	pCoil->emUnten = mcls_pStatus->GetUnten();

	//// �����X�V
	// ���̎��_�ł́A�Ŏ�̐�����Z�b�g
	for(int ii=0; ii<NUM_MEN; ii++) {
		pCoil->emOneKensa[ii] = DIV_KENSA_OK;
		pCoil->emOneKiki [ii] = DIV_KIKI_OK;
	}

	//// �R�C���ʔ������Ɋm��
	pCoil->bCoilResultAddDb = false;

	//// �f�[�^�Z�b�g (���ю��Ɋm��)
	pCoil->bDspOk = false;
}


//==========================================
// ������_�̃R�C������ ���R�C�����G���A�ɃZ�b�g
//==========================================
void CoilBaseManager::RecvNextCoil_Ken()
{
	AutoLock autolock(&m_csLock);
	//double dNobi = 1.0;					// �L�ї�

	//================================================
	// ���L�������f�[�^�A�N�Z�X
	int nRec = 0;	// �����p�̃R�C�����ɃA�N�Z�X
 	COIL_SETTING& setting		= (COIL_SETTING&)mtbl_pCoil->rec[nRec].setting;
	COIL_BASEDATA& basedata		= (COIL_BASEDATA&)mtbl_pCoil->rec[nRec].basedata;
	PRO_TYPE_COILINF& coil		= (PRO_TYPE_COILINF&)mtbl_pCoil->rec[nRec].val;
	PRO_TYPE_DATA_COILINF *pData = (PRO_TYPE_DATA_COILINF *)&coil.data;

	//================================================
	// ���O�`�F�b�N
	// �����f�[�^�H�i�O�����g�t���O�̒l���`�F�b�N�j
	if (0 != pData->Common.nGrandFlg) {
		LOG(em_ERR), "[CoilManager] �����p��M�R�C����� �ُ� [�敪=%d �R�C��No=%.12s]", pData->Common.nGrandFlg, coil.data.CoilInf.KeyInf.cCoilNo);
		syslog(263, "[�����p�R�C����� �敪=%d �R�C��No=%.12s]", pData->Common.nGrandFlg, coil.data.CoilInf.KeyInf.cCoilNo);
		return;
	}

	// ���R�C���o�b�t�@�Ɋ��Ƀf�[�^����
	//bool bOldCoil = false;
	char cOldKizukenNo[SIZE_KIZUKEN_NO];			// �R�C������M�ŁA�\��PC�ɃR�C�����𑗂�悤�ɂ������߁A�ʊǗ�No�������Ⴄ�ƕ\�����R�[�h���ؑւ��Ă��܂��΍�
	if( 0 != strlen(m_CoilNext_Ken.cKizukenNo) ) {
		LOG(em_ERR), "[%s] �����p���R�C���o�b�t�@�Ɋ��Ƀf�[�^�L��[�R�C��No=%s ID=%s]", my_sThreadName, m_CoilNext_Ken.data.cCoilNoIn, m_CoilNext_Ken.cKizukenNo);
		syslog(252, "[�����p�F�R�C��No=%s ID=%s]", m_CoilNext_Ken.data.cCoilNoIn, m_CoilNext_Ken.cKizukenNo);
		
		//bOldCoil = true;
		memcpy(cOldKizukenNo, m_CoilNext_Ken.cKizukenNo, SIZE_KIZUKEN_NO);
	}

	// ���R�C���̎��R�C�����ƍ���M�������R�C����񂪕s��v�H
	if (0 != strlen(m_pCoilKenV[m_nDistPos]->data_u.next_coil[0].KeyInf.cCoilNo)) {
		if (0 != memcmp(m_pCoilKenV[m_nDistPos]->data_u.next_coil[0].KeyInf.cCoilNo, coil.data.CoilInf.KeyInf.cCoilNo, sizeof(coil.data.CoilInf.KeyInf.cCoilNo))) {
			LOG(em_WAR), "[CoilManager] ���R�C���̎��R�C��No�Ǝ��R�C��No���s��v [���̎��R�C��No=%.12s ���R�C��No=%.12s]", m_pCoilKenV[m_nDistPos]->data_u.next_coil[0].KeyInf.cCoilNo, coil.data.CoilInf.KeyInf.cCoilNo);
			syslog(262, "[���R�C���̎��R�C��No=%.12s ���R�C��No=%.12s]", m_pCoilKenV[m_nDistPos]->data_u.next_coil[0].KeyInf.cCoilNo, coil.data.CoilInf.KeyInf.cCoilNo);
		}
	}

	//================================================
	// �f�[�^�Z�b�g (���ߎ��Ɋm��)

	//// ��{
	CoilClear(&m_CoilNext_Ken);
	memcpy( &m_CoilNext_Ken.setting, &setting, sizeof(COIL_SETTING));			// �ӃւŎg���R�C����{���
	memcpy( &m_CoilNext_Ken.data,    &basedata, sizeof(COIL_BASEDATA));			// �R�C����� (�V�X�e������)
	memcpy( &m_CoilNext_Ken.data_u,  &coil.data, sizeof(PRO_TYPE_DATA_COILINF));		// �R�C����� (���C���ŗL)

	m_CoilNext_Ken.dSpmHosei = 1.0 + (double)( coil.data.CoilInf.nNobiSPM ) / 10000;

	//if(!bOldCoil) {
	KizuFunction::GetKizukenNo( m_CoilNext_Ken.cKizukenNo );						// �Ǘ�No
	//} else {
	//	memcpy(m_CoilNext_Ken.cKizukenNo, cOldKizukenNo, SIZE_KIZUKEN_NO);				// ����������̂����������g�p
	//}
	m_CoilNext_Ken.bDummeyCoil = false;
	m_CoilNext_Ken.bCoilInit = false;											// �����J�n�P�{�ڈȊO

	////// �g����
	//memcpy(m_CoilNext_Ken.data.cKizukenNo, m_CoilNext_Ken.cKizukenNo, SIZE_KIZUKEN_NO);

	// �E�H�[�}�ށH
	//// ����������TO_GETDAT��setting.bWalm�Ƀt���O����ꂽ�񂶂�Ȃ��́H
	//if( mcls_pParam->GetCommonMast().walm == m_CoilNext_Ken.setting.pcode[0] ) {			// �E�H�[�}��
	//	m_CoilNext_Ken.setting.bWalm = true;
	//}
	if( m_CoilNext_Ken.setting.bWalm ) {			// �E�H�[�}��
		LOG(em_WAR), "[%s] �E�H�[�}�� �F���I�I [�R�C��No=%s]", my_sThreadName, basedata.cCoilNoIn);
	}

	// ���ɑΉ�����}�b�v�񐔂́H
	if (0 >= m_CoilNext_Ken.data.nWid)			m_CoilNext_Ken.data.nWid = COIL_WID_MAX;
	if (COIL_WID_MAX < m_CoilNext_Ken.data.nWid) m_CoilNext_Ken.data.nWid = COIL_WID_MAX;
#ifndef PLG_DSP_MAP_FIX
	m_CoilNext_Ken.nMapCol = KizuFunction::SelectGridCol2(m_CoilNext_Ken.data.nWid, MAP_COL_INTERVAL, MAP_COL_NUM);
#else
	m_CoilNext_Ken.nMapCol = MAP_COL_NUM;
#endif
	LOG(em_MSG), "[%s] ������_�R�C������M [%s][%s] MAP��=%d", my_sThreadName, basedata.cCoilNoIn, m_CoilNext_Ken.cKizukenNo, m_CoilNext_Ken.nMapCol);
	syslog(250, "[�����p �R�C��No=%s%s]", m_CoilNext_Ken.data.cCoilNoIn, m_CoilNext_Ken.setting.bWalm ? " <�E�H�[�}��>" : "");

	//// ���̑����ʂ̃R�C�����
	Create_CoilInf(&m_CoilNext_Ken);

	//================================================
	// �ʒm

	// �����p ���R�C������M�����ʒm�C�x���g
	SetEvent(m_evNextCoilKen_Recv);

	// ��ʏ����e����PC�֑��M
	TYPE_EPC_COIL data;
	SetTypeEpcCoil(data, m_CoilNext_Ken.data_u);
	MainInstance::Send_HtDefect_Pcinfo(&data);
#ifdef ENABLE_RECORDER_UDP
	// �����p���R�[�_�[�ɑ��M����R�C������ێ�(���ۂɑ��M����̂̓R�C���ؑ֎�)
	memset(&m_typEpcCoil, 0x00, sizeof(TYPE_EPC_COIL));
	memcpy(&m_typEpcCoil, &data, sizeof(TYPE_EPC_COIL));
	for (int ii = 0; ii < NUM_MEN; ii++)
	{
		m_nScode[ii]		= m_CoilNext_Ken.setting.scode[ii];
		m_nScode_spmoff[ii] = m_CoilNext_Ken.setting.scode_spmoff[ii];
	}
#endif


	// �p�����[�^�ύX�ʒm (�����\�ʏ�ԕύX�A���񑗂��Ă��ǂ��Ƃ̎�)
	for(int ii=0; ii<NUM_MEN; ii++) {
		// SPM�������
		LOG(em_MSG), "[%s] ������_ �p�����[�^���O�Ǎ��v��(FACT_HT_DEFECT_21) [%s][%s] SPM���� scode=%d",
			my_sThreadName, m_CoilNext_Ken.data.cCoilNoIn, DivNameManager::GetTorB(ii), m_CoilNext_Ken.setting.scode[ii]);
		MainInstance::Send_HtDefect_PreLoadParam(ii, DIV_HANTEI_NON, m_CoilNext_Ken.setting.scode[ii]);
		// SPM�J�����
		LOG(em_MSG), "[%s] ������_ �p�����[�^���O�Ǎ��v��(FACT_HT_DEFECT_21) [%s][%s] SPM�J�� scode=%d",
			my_sThreadName, m_CoilNext_Ken.data.cCoilNoIn, DivNameManager::GetTorB(ii), m_CoilNext_Ken.setting.scode_spmoff[ii]);
		MainInstance::Send_HtDefect_PreLoadParam(ii, DIV_HANTEI_NON, m_CoilNext_Ken.setting.scode_spmoff[ii]);
	}
	//	
	////// �R�C����� ��\��PC�ɑ��� 
	//// �� �]���́A�㗬�J�����ʒu�̃R�C���؂�ւ��ő����Ă������A�O�H������O�����Ď擾������
	////     ����ŁA�\��PC�̃��R�[�h�����ɐ؂�ւ��邽�߂ɑ���B
	//OpSendFunc::SendOp_CoilInf_Ken( &m_CoilNext_Ken );

	//// �ʔJ�n�R�C���̑O�H�������擾 �� �ʒm
	if( ! m_CoilNext_Ken.bDummeyCoil ) {
		// �����p�R�C��������M���� & �Ǘ�No���m�肵������O�H�����ѕҏW�֒ʒm
		MainInstance::Send_ToMaedat_CoilRecv(m_CoilNext_Ken.cKizukenNo, nRec);

		//// ���Ǘ�No����Ǘ�No�ɐU��Ȃ���
		//mcls_pMae->AddQue_MaeRequest(&m_CoilNext_Ken, dNobi);
	}else{
		//// �\��PC�ɒʒm(�\��PC���ł̋��L�������������ׁ̈A�S�R�C���ʒm���K�v)x
		//MainInstance::Send_HyDefect_MaeRead(m_CoilNext_Ken.cKizukenNo);
	}

}


//------------------------------------------
// �\����_�̃R�C������ ���R�C�����G���A�ɃZ�b�g
//------------------------------------------
void CoilBaseManager::RecvNextCoil_Dsp()
{
#ifndef TRACKING_NON_DSP_COIL
	// ��
	AutoLock autolock(&m_csLock);

	//================================================
	// ���L�������f�[�^�A�N�Z�X
	int nRec = 1;
	PRO_TYPE_COILINF& coil = (PRO_TYPE_COILINF&)mtbl_pCoil->rec[nRec].val;
	PRO_TYPE_DATA_COILINF *pData = (PRO_TYPE_DATA_COILINF *)&coil.data;

	for (int ii = 0; ii<NUM_DSP_WPD; ii++) {
		//================================================
		// ���O�`�F�b�N
		// �����f�[�^�H
		if (0 != pData->Common.nGrandFlg) {
			LOG(em_ERR), "[CoilManager] �\���p��M�R�C����� �ُ� [�敪=%d �R�C��No=%.12s]", pData->Common.nGrandFlg, coil.data.CoilInf.KeyInf.cCoilNo);
			syslog(263, "[�\���p�R�C����� �敪=%d �R�C��No=%.12s]", pData->Common.nGrandFlg, coil.data.CoilInf.KeyInf.cCoilNo);
			return;
		}
		if( 0!= coil.data.CoilInf.nAtuSei)
		// ���R�C���o�b�t�@�Ɋ��Ƀf�[�^����
		if (0 != strlen(m_CoilNext_Dsp[ii].CoilInf.KeyInf.cCoilNo)) {
			LOG(em_ERR), "[%s] �\���p���R�C���o�b�t�@�Ɋ��Ƀf�[�^�L��[�R�C��No=%12.12s]", my_sThreadName, m_CoilNext_Dsp[ii].CoilInf.KeyInf.cCoilNo);
			syslog(253, "[�\���p�F�R�C��No=%12.12s]", m_CoilNext_Dsp[ii].CoilInf.KeyInf.cCoilNo);
			memset(&m_CoilNext_Dsp[ii], 0x00, sizeof(PRO_TYPE_DATA_COILDSP));
		}


		//================================================
		// �f�[�^�Z�b�g
		memcpy(&m_CoilNext_Dsp[ii], &coil.data, sizeof(PRO_TYPE_DATA_COILDSP));		// �v���R���f�[�^�Z�b�g
	}

	char cWk[SIZE_COIL_NO + 1];
	memset(cWk, 0x00, sizeof(cWk));
	memcpy(cWk, m_CoilNext_Dsp[0].CoilInf.KeyInf.cCoilNo, SIZE_COIL_NO);
	LOG(em_MSG), "[%s] �\����_�R�C������M [%s]", my_sThreadName, cWk);
	syslog(250, "[�\���p �R�C��No=%s]", cWk);
#endif
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// �R�C���؂�ւ�
//==========================================================================
// �����ΏۃR�C���؂�ւ�(���) [��MI]
//--------------------------------------------------------------------------
// bool isWpd : true:�ʏ�ؑ� false:�R�C����������B�ɂ��o�b�N�A�b�v�ؑ�
// bool isNowCoil : true:���R�C�����f�̈ꔭ�ڐؑ� false:����ȊO
//==========================================================================
void CoilBaseManager::ChangeCoilKenBase(bool isWpd, bool isNowCoil)
{
	AutoLock autolock(&m_csLock);
	bool bDummyFlg = false;					// �_�~�[�R�C����������true
	int rec = m_nKenRecBase + 1;
	if(rec >= SIZE_COIL_BUF) rec = 0;

	if(isWpd){
		//================================================
		//// �����֘A
		// ���R�C����񂪂����Ƃ������H
		if( 0 == strlen(m_CoilNext_Ken.cKizukenNo) ) { 
			LOG(em_ERR), "[%s] �����p���R�C����񖳂�", my_sThreadName);
			syslog(254, "[�����p]");
			bDummyFlg = true;
		}

		//================================================
		//// �R�C���؂�ւ�
		memcpy( &m_CoilInf[rec], &m_CoilNext_Ken, sizeof(COIL_INF));	// ���R�C�� ���� ���R�C���ɏ��Z�b�g
		
	}
	m_CoilInf[rec].rec = rec;
	
	// �R�C������SPM��ԃZ�b�g
	// �������ʒu�̏�ԂŌ���
	m_CoilInf[rec].nSt_Spm = mcls_pStatus->IsSpmStat() ? 1 : 0;
	LOG(em_INF), "[%s] [%s] �����ʒu SPM��ԁF%s", my_sThreadName, m_CoilInf[rec].data.cCoilNoIn, GetSpmStat((EM_DIV_SPM_STAT)mcls_pStatus->GetSpmStat()));

	// SPM���J������Ă����ꍇ�A�p�����[�^�ύX�v���𑗐M����
	if (0 == m_CoilInf[rec].nSt_Spm)
	{
		// ��ʂ̂ݕύX
		// �����Ɍ�����ɓ��B���Ă���̂ő������f
		LOG(em_INF), "[%s] �����ʒu �p�����[�^�ύX�v��(FACT_HT_DEFECT_01) <SPM%s> [%s][%s] scode=%d",
			my_sThreadName, GetSpmStat((EM_DIV_SPM_STAT)mcls_pStatus->GetSpmStat()), m_CoilInf[rec].data.cCoilNoIn, DivNameManager::GetTorB(m_nDistPos), m_CoilInf[rec].setting.scode_spmoff[m_nDistPos]);
		MainInstance::Send_HtDefect_Param(0, m_nDistPos, m_CoilInf[rec].setting.scode_spmoff[m_nDistPos], DIV_HANTEI_NON);
	}

	// �X�e�[�^�X�Ǘ��N���X�ɃR�C�������Z�b�g
	mcls_pStatus->SetCoilInfStatus(&m_CoilInf[rec]); 

	// ���G���A������
	int nextrec = rec + 1;
	if(nextrec >= SIZE_COIL_BUF) nextrec = 0;
	CoilClear(&m_CoilInf[nextrec]);			// ���G���A�̈揉����
	m_CoilInf[nextrec].rec = nextrec;

	// ���Ǘ�No����Ǘ�No�ɐU��Ȃ������R�C���؂�ւ����Ƀ^�C�~���O�ύX
	mcls_pMae->RecvCoilInf(&m_CoilNext_Ken);

	//================================================
	//// �R�C�����L��
	if(bDummyFlg || ! isWpd) {
		// �R�C����񂪖��������ׁA�_�~�[�R�C������
		DummyCoilInf(rec, false, isWpd);

#ifdef ENABLE_RECORDER_UDP
		// �_�~�[�R�C���̃��R�[�_�[PC���M�p�R�C�����𐶐�
		// �����p���R�[�_�[�ɑ��M����R�C������ێ�(���ۂɑ��M����̂̓R�C���ؑ֎�)
		TYPE_EPC_COIL data;
		SetTypeEpcCoil(data, m_CoilInf[rec].data_u);
		memset(&m_typEpcCoil, 0x00, sizeof(TYPE_EPC_COIL));
		memcpy(&m_typEpcCoil, &data, sizeof(TYPE_EPC_COIL));
		for (int ii = 0; ii < NUM_MEN; ii++)
		{
			m_nScode[ii]		= m_CoilInf[rec].setting.scode[ii];
			m_nScode_spmoff[ii] = m_CoilInf[rec].setting.scode_spmoff[ii];
		}
#endif

		// �@���ԕύX
		// �^�C�~���O�s�ǎ�
		if( (0 != mcls_pStatus->GetStatusRead().Coil_Init.stat || 0 != mcls_pStatus->GetStatusRead().Coil_ReStart.stat) && isWpd ) {
			mcls_pStatus->SetStatusQue("COIL_TIMING_ERR_ID", false);
		}
		// �����p�R�C����񖢎�M��
		else {
			if(0 == mcls_pStatus->GetStatusRead().MaxLen_Ken.stat) {
				mcls_pStatus->SetStatusQue("COIL_TIMING_ERR_ID", true, false);
				if( 0 == mcls_pStatus->GetStatusRead().Coil_Ken.stat && isWpd ) mcls_pStatus->SetStatusQue("COIL_KEN_ID", false);
			}
		}
	} else {
		// �@���ԕύX
		if( 0 != mcls_pStatus->GetStatusRead().Coil_Ken.stat && isWpd  ) mcls_pStatus->SetStatusQue("COIL_KEN_ID", true);
		if( 0 != mcls_pStatus->GetStatusRead().Coil_Timing_Err.stat && isWpd ) mcls_pStatus->SetStatusQue("COIL_TIMING_ERR_ID", true);
	}
	// �����J�n�P�{�ڈȍ~(���R�C�����f�̈ꔭ�ڂ͎��{���Ȃ�)
	if( ! isNowCoil ){
		if( 0 != mcls_pStatus->GetStatusRead().Coil_Init.stat )    mcls_pStatus->SetStatusQue("COIL_INIT_ID", true);
		if( 0 != mcls_pStatus->GetStatusRead().Coil_ReStart.stat ) mcls_pStatus->SetStatusQue("COIL_RESTART_ID", true);
	}
	//// �����\�ʏ�� / �����p�^�[�� �ۑ� (���������ċN���p)
	for(int ii=0; ii<NUM_MEN; ii++) {
		m_nInitScode[ii] = m_CoilInf[rec].setting.scode[ii];
		m_nInitPcode[ii] = m_CoilInf[rec].setting.pcode[ii];
	}

	// ������_�ł̃R�C����SPM�L�ї���ێ����Ă���
	if (bDummyFlg)
	{
		// SPM�L�ї�0%�Œ�
		m_dLastSpm = 0.0;
	}
	else
	{
		// [10^2%]
		m_dLastSpm = m_CoilInf[rec].data.dNobiSPM;
	}
	LOG(em_INF), "[%s] [%s] �����ʒu SPM�L�ї�=%.4f%%", my_sThreadName, m_CoilInf[rec].data.cCoilNoIn, m_dLastSpm);

	//================================================
	//// ���R�[�h�X�V
	m_nKenRecBase = rec;
	m_pCoilKenR[m_nDistPos] = &m_CoilInf[rec];

	//// ������n�߂�R�C���� �R�C������DB�֕ۑ�
	SaveDB_CoilInf();

	//// ���R�C���̈揉����
	CoilClear(&m_CoilNext_Ken);				// ���R�C���̈揉����

	//// ���@�\�֒ʒm
	// �R�C�����
	OpSendFunc::SendOp_CoilInf_Ken( &m_CoilInf[m_nKenRecBase] );
	// �X�e�[�^�X���
	OpSendFunc::SendOp_Status();
}

//==========================================
// �����ΏۃR�C���؂�ւ�(������) [��MainInstance]
//==========================================
void CoilBaseManager::ChangeCoilKenBase_NotPos()
{
	AutoLock autolock(&m_csLock);

	// �Q�ƃ|�C���^��ύX
	m_pCoilKenR[m_nDistNotPos]	= &m_CoilInf[m_nKenRecBase];

	// SPM���J������Ă����ꍇ�A�p�����[�^�ύX�v���𑗐M����
	if (0 == m_pCoilKenR[m_nDistNotPos]->nSt_Spm)
	{
		// ��ʂƋt�̖ʂ̂ݕύX
		// �����Ɍ�����ɓ��B���Ă���̂ő������f
		LOG(em_INF), "[%s] �����ʒu �p�����[�^�ύX�v��(FACT_HT_DEFECT_01) <SPM%s> [%s][%s] scode=%d",
			my_sThreadName, GetSpmStat((EM_DIV_SPM_STAT)mcls_pStatus->GetSpmStat()), m_CoilInf[m_nKenRecBase].data.cCoilNoIn, DivNameManager::GetTorB(m_nDistNotPos), m_CoilInf[m_nKenRecBase].setting.scode_spmoff[m_nDistNotPos]);
		MainInstance::Send_HtDefect_Param(0, m_nDistNotPos, m_CoilInf[m_nKenRecBase].setting.scode_spmoff[m_nDistNotPos], DIV_HANTEI_NON);
	}
}


//==========================================
// �\���ΏۃR�C���؂�ւ� [��MainInstance]
//------------------------------------------
// double	dLen	: �\����_�̃R�C����
// int		nDspWpd	: �\����_�i1CAPL�����ł�0�̂݁j
// �߂�l	bool	: �؂�ւ���
//==========================================
bool CoilBaseManager::ChangeCoilDspBase(double dLen, int nDspWpd)
{
	AutoLock autolock(&m_csLock);
	int rec = -1;							// �����ێ����R�[�h�Ƃ̕R�t���s�\���́A-1
	int wkRec = 0;							// �����R�[�h�����p

#ifndef TRACKING_NON_DSP_COIL			// �g���b�L���O�\��
	char cCoilKey[SIZE_COIL_KEY + 1];					// �g���₷���悤�ɃL�[�ƂȂ��񂾂��ɂ���
	char cCoilNo[SIZE_COIL_NO+1];						// ���O�\���p�ɃR�C��No��
	char cMeiNo[SIZE_MEI_NO + 1];						// ���O�\���p�ɖ���No��
	memset(cCoilKey, 0x00, sizeof(cCoilKey));
	memset(cCoilNo, 0x00, sizeof(cCoilNo));
	memset(cMeiNo, 0x00, sizeof(cMeiNo));
	memcpy(cCoilKey, &m_CoilNext_Dsp[nDspWpd].CoilInf.KeyInf, sizeof(PRO_TYPE_KEY));
	memcpy(cCoilNo, &m_CoilNext_Dsp[nDspWpd].CoilInf.KeyInf.cCoilNo, SIZE_COIL_NO);
	memcpy(cMeiNo, &m_CoilNext_Dsp[nDspWpd].CoilInf.KeyInf.cMeiNo, SIZE_MEI_NO);


	//// ���R�C����񂪂����Ƃ������H
	if( 0 == strlen(cCoilKey) ) { 
		LOG(em_ERR), "[%s] �\���p���R�C����񖳂�", my_sThreadName);
		syslog(255, "[�\���p]");
		if( 0 == mcls_pStatus->GetStatusRead().Coil_Dsp.stat ) mcls_pStatus->SetStatusQue("COIL_DSP_ID", false);
		rec = -1;

	} else {
		// �����p�R�C���Ƃ̕R�t��
		for(int ii=1; ii<SIZE_COIL_BUF-1; ii++) {
			wkRec = GetRecIndex(m_nDspRecBase, -ii);		// 1�� 2���Ə��ԂɒT���Ă���
			if( 0 == memcmp(cCoilKey, m_CoilInf[wkRec].data.cCoilKey, SIZE_COIL_KEY) ) {
				// �R�t������
				if( 0 != mcls_pStatus->GetStatusRead().Coil_Dsp.stat ) mcls_pStatus->SetStatusQue("COIL_DSP_ID", true);
				rec = wkRec;
				break;
			}
		}

		if(-1 == rec) {
			LOG(em_WAR), "[%s] �\���p�R�C�����؂�ւ������ǁA�܂����������R�C���������B�R�C��No=%s, ����No=%s", my_sThreadName, cCoilNo, cMeiNo);
			syslog(256, "[�R�C��No=%s, ����No=%s]", cCoilNo, cMeiNo);
			memset( &m_CoilNext_Dsp, 0x00, sizeof(m_CoilNext_Dsp));
			return false;
		}
	}

	//// �����J�n��{�ڂ̏ꍇ�̂݁AWPD��������A���������Ă���z���o�͂��Ă�����  (���Ȃ�f�o�b�N�p)
	if(NULL == m_pCoilBaseDsp) {
		if(-1 == rec) rec=0;
	}

	//// �R�t�����s���̃t�H���[
	if(-1 == rec) {
		// ���Ԓʂ�ɒʔ���͂��Ȃ̂ŁA���̎��̓z������Ă���Ƃ���
		wkRec = GetRecIndex(m_nDspRecBase, -1);
		if( -1 != m_nDspRecBase && 0 > strcmp( m_CoilInf[m_nDspRecBase].cKizukenNo, m_CoilInf[wkRec].cKizukenNo) ) {	// �Ǘ�No�� �� < �� �ɂȂ�Ȃ��Ƃ�������
			rec = wkRec;	// 1�����̃��R�[�h

			LOG(em_WAR), "[%s] �\���p�R�C�� ���R�t�� �R�C��No=%s, ����No=%s", my_sThreadName, m_CoilInf[rec].data.cCoilNoIn, m_CoilInf[rec].data.cMeiNo);
			syslog(257, "[�R�C��No=%s, ����No=%s]", m_CoilInf[rec].data.cCoilNoIn, m_CoilInf[rec].data.cMeiNo);


			// �z���g�ɉ����킩��Ȃ� (�����J�n����Ƃ��������肦�Ȃ��͂�)
		}else {
			LOG(em_ERR), "[%s] �\���p�R�C����� �R�t�����s�I�I", my_sThreadName);
			syslog(258, "");
		}
	}
	memset( &m_CoilNext_Dsp, 0x00, sizeof(m_CoilNext_Dsp));			// �\���p���R�C���o�b�t�@������


#else			// ���A���^�C���\��
	// �\�����R�[�h�ƌ������R�[�h�͈ꏏ
	rec = m_nKenRecBase;
#endif

	//// �������`�F�b�N
	if(-1 == rec) return false;					// ���R�[�h�������s
	if( rec == m_nDspRecBase) return false;		// ���\�����R�C���Ɠ��� (���肦�Ȃ�)


	//// ���ʔ��̃R�C���̎��уZ�b�g
	//// (�㗬���̕\����_�̂ݕۑ����{)
	if(NULL != m_pCoilBaseDsp) {
		m_pCoilBaseDsp->nDspLength = (long)dLen; 
		SaveDB_UpdateCoilResult(MODE_DSP_WPD_ON, m_pCoilBaseDsp);

	}

	//// �\���p�R�C���؂�ւ�
	m_nDspRecBase				= rec;
	m_pCoilBaseDsp				= &m_CoilInf[rec];
	m_pCoilBaseDsp->bDspOk		= true;
	LOG(em_MSG), "[%s] �\���p�R�C���؂�ւ����� rec=%d �R�C��No=%s", my_sThreadName, rec, m_CoilInf[rec].data.cCoilNoIn);
	syslog(260, "[�R�C��No=%s]", m_CoilInf[rec].data.cCoilNoIn);

	//// ���@�\�֒ʒm
	OpSendFunc::SendOp_CoilInf_Dsp(m_pCoilBaseDsp);
	return true;
}

//==========================================
// �������u�ʒu �ΏۃR�C���̎Q�ƈʒu�ύX (���������u�ʒu�̃��R�[�h�̓z�ɂ���) [��HtRecv]
//------------------------------------------
// int	men	: �\��
//==========================================
void CoilBaseManager::ChangeCoilKenPointer(int men)
{
	AutoLock autolock(&m_csLock);
	
	//// �������u�ʒu �̎Q�ƃ|�C���^��ύX
	m_pCoilKenV[men] = &m_CoilInf[m_nKenRecBase];
	//LOG(em_MSG), "[%s] �������u�ʒu �ΏۃR�C���̎Q�ƈʒu�ύX���� �Ǘ�No=%s ID=%d rec=%d SetRec[0][0]=%d SetRec[0][1]=%d SetRec[1][0]=%d SetRec[1][1]=%d men=%d"
	//	, my_sThreadName, m_pCoilKenV[men]->cKizukenNo, m_nKenRecBase, m_pCoilKenV[men]->rec
	//	, m_pCoilKenV[men]->nSetRec_St[0][0], m_pCoilKenV[men]->nSetRec_St[0][1]
	//	, m_pCoilKenV[men]->nSetRec_St[1][0], m_pCoilKenV[men]->nSetRec_St[1][1]
	//	, men);
}

//==========================================
// �\���p �e�ʒu �ΏۃR�C���̎Q�ƈʒu�ύX (���e�ʒu�̃��R�[�h�̓z�ɂ���) [��PlgManager]
//------------------------------------------
// EM_POS_DSP	pos		: �Ώۈʒu (���3���ڂ́A���Ȃ�)
// double		dLenIn	: ���݂̒ʉ߈ʒu�ł̓����P�ʂ̋��� [mm]
// double		dLenOut	: �V���[�ʒu�ł̏o���P�ʂ̋��� [mm]
//==========================================
void CoilBaseManager::ChangeCoilDspPointer(EM_POS_DSP pos, double dLenIn, double dLenOut)
{
	AutoLock autolock(&m_csLock);

	//// �Q�ƃ|�C���^��ύX (�\����_�`��ԉ����̈ʒu�܂ł̊ԂɈ�{�R�C��������Ȃ��O��)
	// �e�ʒu�̃|�C���^�ɃZ�b�g����
	m_pCoilDsp[pos]    = m_pCoilBaseDsp;

	//// ���̃R�C���̐�[�����̃V���[�J�b�g�P�ʂ̋����ƂȂ�
	//m_pCoilDsp[pos]->nOutCoilPos = (long)dLenOut;
}

//==========================================
// �\���p �V���[�J�b�g�ʒu �ΏۃR�C���̎Q�ƈʒu�ύX
// (���e�ʒu�̃��R�[�h�̓z�ɂ���) [��PlgManager]
//------------------------------------------
// double dLen_In	
// double dLen_Out	
//==========================================
void CoilBaseManager::ChangeCoilShaPointer(double dLen_In, double dLen_Out)
{
	AutoLock autolock(&m_csLock);

	//// ���܂ŃV���[�J�b�g�ʒu��ʉ߂��Ă����R�C�����Ώ�
	if( NULL != m_pCoilDsp[POS_DSP_SHR_I]) {
		//// �����P�ʂ̃z���g�̈�ԍŌ�̎��тׁ̈A����updete���鎖�ɂ���
		// �R�C�����т��r���Ǘ��̏o����������ύX
		SaveDB_UpdateCoilResult(MODE_RECV_SHRCUT, m_pCoilDsp[POS_DSP_SHR_I]);
	}

	//---------------------------------------
	// �o���֘A
	//---------------------------------------
	if (0 != strlen(m_CoilOutInf.cID))
	{
		// �O��̗n�ړ_����V���[�J�b�g����ĂȂ�
		// �����o���R�C�����ɗn�ړ_���Q�_��
		if ((MAX_SHALINK_COUNT - 1) <= m_CoilOutInf.nCnt)
		{
			LOG(em_MSG), "[%s] ���o���R�C�����ɂ�����%d�_�ڂ̗n�ړ_", my_sThreadName, m_CoilOutInf.nCnt);
		}
		// �P�o���R�C���ɂR�����R�C���ȏ�͋��e�ł��Ȃ�
		if (MAX_SHALINK_COUNT <= m_CoilOutInf.nCnt)
		{
			LOG(em_ERR), "[%s] �V���[�J�b�g�Ǘ��񐔏�����B�ɂ��A�V���[�J�b�g����j��", my_sThreadName);
			syslog(297, "[�R�C��No=%.12s]", m_pCoilBaseDsp->data.cCoilNoIn);
		}
		else
		{
			// �O�����̎��т��m�肳����
			SetInCoilInf_End(&m_CoilOutInf, dLen_In, dLen_Out);

				// �������̏����Z�b�g����(�V���[�J�b�g���ȊO)
			SetInCoilInf_Init(m_pCoilBaseDsp, &m_CoilOutInf, dLen_Out);
#ifdef DEBUG_LOG_OUT_CHANGE
			LOG(em_FIL), "[%s] ChangeCoilShaPointer()�ō����R�C���F��[%d]", my_sThreadName, m_CoilOutInf[m_nShaRecBase].nCnt);
#endif
		}
	}

	//// �Q�ƃ|�C���^��ύX (�\����_�`��ԉ����̈ʒu�܂ł̊ԂɈ�{�R�C��������Ȃ��O��)
	int pos = POS_DSP_SHR_I;
	// �e�ʒu�̃|�C���^�ɃZ�b�g����
	m_pCoilDsp[pos]    = m_pCoilBaseDsp;

	// ���̃R�C���̐�[�����̃V���[�J�b�g�P�ʂ̋����ƂȂ�
	m_pCoilDsp[pos]->nOutCoilPos = (long)dLen_Out;
	LOG(em_MSG), "[%s] �V���[�J�b�g�ʒu �R�C���؂�ւ� �����R�C��(%.10s)%.20s In=%dmm Out=%dmm", my_sThreadName, m_pCoilDsp[pos]->data.cCoilNoIn, m_pCoilDsp[pos]->cKizukenNo, (long)dLen_In, (long)dLen_Out );

}

//==========================================
// �������u�ʒu �Ώۏo�������R�C���̎Q�ƈʒu�ύX
// (���������u�ʒu�̃��R�[�h�̓z�ɂ���) [��HtRecv]
//------------------------------------------
// int	ou	: �\��(0:�\ 1:��)
//==========================================
void CoilBaseManager::ChangeCoilOutKenPointer(int ou)
{
	AutoLock autolock(&m_csLock);

	//// �Q�ƃ|�C���^��ύX (�\����_�`��ԉ����̈ʒu�܂ł̊ԂɈ�{�R�C��������Ȃ��O��)
	// �e�ʒu�̃|�C���^�ɃZ�b�g����
	m_pCoilOutKen[ou] = &m_CoilOutInf;


	// �e�ʒu�̎Q�ƃ|�C���g���ύX���ꂽ�H
	if( m_pCoilOutKen[0] != &m_CoilOutInf ||
		m_pCoilOutKen[1] != &m_CoilOutInf ) return;

	// �o���R�C�����L��H
	if( 0 == strlen( m_CoilOutInf.cID) ) return;

}

//==========================================
// �������u�ʒu �o�������R�C���̓����R�C�������X�V
// �����R�C�����f�̌����J�n���Ɏg�p
//------------------------------------------
// �y�Ăяo�����z
//  ��
//		MainInstance
//==========================================
void CoilBaseManager::ReloadInCoil()
{
	AutoLock autolock(&m_csLock);
	
	memset(&m_CoilOutInf.incoil[0], 0x00, sizeof(m_CoilOutInf.incoil[0]));
	m_CoilOutInf.nCnt = 0;

#ifdef DEBUG_LOG_OUT_CHANGE
	LOG(em_FIL), "[%s] ���R�C�����f�ɂ��킹�čX�V[%d]", my_sThreadName, m_CoilOutInf[m_nShaRecBase].nCnt);
#endif
}

//==========================================
// �o���R�C���̎��̓����R�C�������Z�b�g
//------------------------------------------
// COIL_INF*		pCoilIn		: ���������R�C�����
// COIL_OUT_INF*	pCoilOut	: �o�������R�C�����
// double			dOutLen		: �o���ʔ���
//==========================================
void CoilBaseManager::SetInCoilInf_Init(COIL_INF* pCoilIn, COIL_OUT_INF* pCoilOut, double dOutLen)
{
	// �������̏����Z�b�g����(�V���[�J�b�g���ȊO)
	int nRec = pCoilOut->nCnt;

	memcpy( pCoilOut->incoil[nRec].cKizukenNo, pCoilIn->cKizukenNo, SIZE_KIZUKEN_NO);
	pCoilOut->incoil[nRec].nInCutNum = 1;
	pCoilOut->incoil[nRec].nFront = 0;
	pCoilOut->incoil[nRec].nOutPos = (int)dOutLen;
	pCoilOut->incoil[nRec].nSt_Spm = pCoilIn->nSt_Spm;
	pCoilOut->incoil[nRec].dSpmHosei = pCoilIn->dSpmHosei;

	pCoilOut->nCnt += 1;
}

//==========================================
// �Ō�̓����R�C�����̊m������Z�b�g
//------------------------------------------
// COIL_OUT_INF*	pCoilOut	: �o�������R�C�����
// double			dInLen		: �����ʔ���
// double			dOutLen		: �o���ʔ���
//==========================================
void CoilBaseManager::SetInCoilInf_End(COIL_OUT_INF* pCoilOut, double dInLen, double dOutLen)
{
	// �Ō�̓����̎��т��m�肳����
	int nRec = pCoilOut->nCnt;
	if(0 <= nRec-1) {
		pCoilOut->incoil[nRec - 1].nLength = (int)dInLen - pCoilOut->incoil[nRec - 1].nFront;
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ���ѕۑ��֘A

//==========================================
// �R�C����� DB�ۑ�
// �� ���ۂ̕��̗���� ��ʂ̌������u�ʒu�ʉߎ�
//------------------------------------------
// �y�Ăяo�����z
//  ��
//		Init
//		ChangeCoilKenBase
//==========================================
void CoilBaseManager::SaveDB_CoilInf()
{
	if(-1 == m_nKenRecBase ) {
		LOG(em_ERR), "[%s] DB�ۑ��� �R�C�����ُ�", my_sThreadName);
		syslog(261, "[�R�C�����Ǘ��Ō��݃��R�[�h�ʒu���s��=%d]", m_nKenRecBase);
		return;
	}

	//================================================
	//// ���O����
	DBManager::DeliveryDB *deliDB = new DBManager::DeliveryDB;
	deliDB->kubun = DBManager::em_COILINF;
	deliDB->data = (BYTE*)new DBDATA_COIL_INF;
	DBDATA_COIL_INF* pWk   = (DBDATA_COIL_INF*)deliDB->data;			// �G�C���A�X
	memset(pWk, 0x00, sizeof(DBDATA_COIL_INF));
	
	const COIL_INF*	 pCoil = &m_CoilInf[m_nKenRecBase];					// �G�C���A�X


	//================================================
	//// �f�[�^�Z�b�g
	memcpy( pWk->cKizukenNo, pCoil->cKizukenNo, SIZE_KIZUKEN_NO);
	for(int ii=0; ii<NUM_MEN; ii++) {
		pWk->scode[ii] = pCoil->setting.scode[ii];
		pWk->pcode[ii] = pCoil->setting.pcode[ii];
	}

	SetTypeCoilInf(*pWk, pCoil->data_u);

	//================================================
	//// �o�^�˗�
	if( ! mque_pAddSQLData->AddToTail( deliDB, 20, 20) ) {			// ��Γo�^ 
		// ��Γo�^�������E�E�E
		LOG(em_ERR), "[%s] DB�o�^�˗��L���[�t�� [DBDATA_COIL_INF]", my_sThreadName);
		syslog(SYSNO_QUEFULL_ERR, "[DBDATA_COIL_INF]");
		delete deliDB->data;
		delete deliDB;
		// DB���ُ� (�ʏ킠�肦�Ȃ�)
		mcls_pStatus->AddQueMyErrWar( true, ON_QUE_COIL_INF );
	}
}

//==========================================
// �R�C���ʒu��� DB�ۑ�
// �� �f�[�^�̗���� ��ʂłȂ����̖ʂ� �ŏ��Ԋu���X�V���ꂽ�^�C�~���O
//------------------------------------------
// int	ou	: �\�� (���ʌ������́A��ʂłȂ����̖�)
// int	row	: �Z�b�g�ʒu (�ŏ��Ԋu=5m) (0�I���W��)
//==========================================
void CoilBaseManager::SaveDB_CoilPosition(int ou, int row)
{
	//if(DIV_UNTEN_SMYU == mcls_pStatus->GetUnten()) return;
	if( 0>row || MAP_ROW_NUM <=row) return;				// �ʒu�ُ�

	//// ���O����
	DBManager::DeliveryDB *deliDB = new DBManager::DeliveryDB;
	deliDB->kubun = DBManager::em_COILPOSITION;
	deliDB->data = (BYTE*)new DBDATA_COIL_POSITION;
	DBDATA_COIL_POSITION* pWk = (DBDATA_COIL_POSITION*)deliDB->data;			// �G�C���A�X
	
	const COIL_INF*		  pCoil = m_pCoilKenV[ou];								// �G�C���A�X

	//// �f�[�^�Z�b�g
	// �w�b�_��
	memcpy( pWk->cKizukenNo, pCoil->cKizukenNo, SIZE_KIZUKEN_NO);
	pWk->nID = row;
	// �f�[�^��
	for(int ii=0; ii<NUM_MEN; ii++) {
		pWk->emKensa[ii]	= pCoil->emKensa[ii][row];
		pWk->emKiki[ii]		= pCoil->emKiki[ii][row];
	}
	for(int ii=0; ii<2; ii++) {
		pWk->dEdge[ii] = pCoil->dEdge[m_nDistPos][row][ii];						// �G�b�W�ʒu
	}
	pWk->dWalk      = (pWk->dEdge[0] + pWk->dEdge[1]) / 2.0f;			// �E�H�[�N��(���C���Z���^�[����̋���)

	//// �o�^�˗�
	if( ! mque_pAddSQLData->AddToTailFreeCheck(deliDB, 50) ) {	// �o�^
		LOG(em_ERR), "[%s] DB�o�^�˗��L���[�t�� [DB_COIL_POSITION] <%s> [%s] row:%d", my_sThreadName, DivNameManager::GetTorB(ou), pCoil->cKizukenNo, row);
		syslog(SYSNO_QUEFULL_WAR, "[DB_COIL_POSITION]");
		delete deliDB->data;
		delete deliDB;
	}
}

//==========================================
// �R�C�����ѕ� DB�ۑ�
// �� �f�[�^�̗���� ��ʂłȂ����̌������u�ʒu�ʉߎ�
//------------------------------------------
// �y�Ăяo�����z
//  ���X���b�h
//		SetData_CoilResult_Virtual
//==========================================
void CoilBaseManager::SaveDB_CoilResult()
{
	//if(DIV_UNTEN_SMYU == mcls_pStatus->GetUnten()) return;
	if(-1 == m_nKenRecBase ) {
		LOG(em_ERR), "[%s] DB�ۑ��� �R�C�����шُ�", my_sThreadName);
		return;
	}

	//// �����܂ŗ����� ���ѓo�^�ς݂Ƃ����
	m_pCoilKenV[m_nDistNotPos]->bCoilResultAddDb	= true;


	//================================================
	//// ���O����
	DBManager::DeliveryDB *deliDB = new DBManager::DeliveryDB;
	deliDB->kubun = DBManager::em_COILRESULT;
	deliDB->data = (BYTE*)new DBDATA_COIL_RESULT;
	DBDATA_COIL_RESULT* pWk = (DBDATA_COIL_RESULT*)deliDB->data;			// �G�C���A�X

	const COIL_INF*		pCoil = m_pCoilKenV[m_nDistNotPos];					// �G�C���A�X


	//================================================
	//// �f�[�^�Z�b�g
	memcpy( pWk->cKizukenNo, pCoil->cKizukenNo, SIZE_KIZUKEN_NO);

	pWk->nEndMode					= pCoil->nEndMode;
	for(int ii=0; ii<NUM_MEN; ii++) pWk->nSt_Hantei[ii] = pCoil->nSt_Hantei[ii];
	for(int ii=0; ii<NUM_MEN; ii++) pWk->nSt_Light[ii]  = pCoil->nSt_Light[ii];
	for(int ii=0; ii<NUM_MEN; ii++) pWk->nSt_Cam_Inline[ii] = pCoil->nSt_Daisya[ii];
	pWk->nSt_CamControl				= pCoil->nSt_CamControl;
	pWk->nSt_CamSwitch				= pCoil->nSt_CamSwitch;
	pWk->nSt_Procon					= pCoil->nSt_Procon;
	pWk->nSt_FF						= pCoil->nSt_FF;
	pWk->nSt_Sqence					= pCoil->nSt_Sqence;
	for(int ii=0; ii<NUM_MEN; ii++) pWk->nSt_Cam_Inline[ii]	= pCoil->nSt_Daisya[ii];
	for(int ii=0; ii<NUM_MEN; ii++) pWk->nSt_DbSkip[ii]		= pCoil->nSt_DbSkip[ii];
	for(int ii=0; ii<NUM_MEN; ii++) pWk->nSt_FrameSkip[ii]  = pCoil->nSt_FrameSkip[ii];
	for(int ii=0; ii<NUM_MEN; ii++) {
		for(int jj=0; jj<NUM_HANTEI; jj++) {
			pWk->nHtID[ii][jj] = pCoil->nHtID[ii][jj];
		}
	}
	for(int ii=0; ii<NUM_MEN; ii++) {
		for(int jj=0; jj<NUM_RPIEXEC; jj++) {
			pWk->nRpiID[ii][jj] = pCoil->nRpiID[ii][jj];
		}
	}
	memcpy( &pWk->setting, &pCoil->setting, sizeof(COIL_SETTING));
	
	pWk->nSpmStat					= pCoil->nSt_Spm;
	pWk->nLength					= pCoil->nLength;
	pWk->nLengthSub					= pCoil->nLengthSub;
	pWk->nLengthDsp					= pCoil->nDspLength;
	pWk->nOutCoilPos				= pCoil->nOutCoilPos;

	pWk->bStartWpdOk				= (0 == pCoil->nStartWpdOk ? true:false);
	pWk->nCoilCnt					= pCoil->nCoilCnt;

	pWk->emUnten					= pCoil->emUnten;
	for(int ii=0; ii<NUM_MEN; ii++) pWk->emOneKensa[ii]  = pCoil->emOneKensa[ii]; 
	for(int ii=0; ii<NUM_MEN; ii++) pWk->emOneKiki[ii]   = pCoil->emOneKiki[ii]; 

	pWk->emPlg						= pCoil->emPlg;
	pWk->nToyuSply					= pCoil->nSt_ToyuSplr;

	// �����������ё��M�\������
	bool bPutOk = true;			// ���M����
	if( pCoil->bDummeyCoil )				bPutOk = false;		// �R�C����񂪊���t���Ă��Ȃ�
	if( 1 != pCoil->nEndMode )				bPutOk = false;		// �r���R�C���I���́A�ΏۊO
	if( DIV_PLG_TEST == pCoil->emPlg )		bPutOk = false;		// �e�X�g�p���X���́A�ΏۊO
	if( DIV_UNTEN_NOMAL != pCoil->emUnten )	bPutOk = false;		// �^�]��Ԃ�����ȊO�͎��́A�ΏۊO
	if( pCoil->setting.bWalm )				bPutOk = false;		// �E�H�[�}�ނ̏ꍇ�́A�ΏۊO
	
	pWk->nResultSendOk		= bPutOk ? 0 : 1;

	// �P�x(�J���� - �p�x - �ʒu(CLR))
	for(int men=0; men<NUM_MEN; men++){
		for(int cam=0; cam<NUM_CAM_POS; cam++){
			for(int angle=0; angle<MAX_CAMANGLE; angle++){
				int camset = men * NUM_CAM_POS + cam;
				pWk->nBr[camset][angle][0] = m_nBrCenter[men][cam][angle];
			}
		}
	}

	// �I������(�\�� - �J���� - �A���O��)
	for(int men=0; men<NUM_MEN; men++) {
		for(int cam=0; cam<NUM_CAM_POS; cam++) {
			for(int angle=0; angle<MAX_CAMANGLE; angle++) {
				int camset = men * NUM_CAM_POS + cam;
				pWk->nExp[camset][angle] = GetCamExpTime(men, cam, angle);	// �I������
			}
		}
	}

	// �Q�C��(�\�� - �J���� - �A���O��)
	for(int men=0; men<NUM_MEN; men++) {
		for(int cam=0; cam<NUM_CAM_POS; cam++) {
			for(int angle=0; angle<MAX_CAMANGLE; angle++) {
				int camset = men * NUM_CAM_POS + cam;
				pWk->nGain[camset][angle] = (int)GetCamGain(men, cam, angle);	// �Q�C��
			}
		}
	}

	LOG(em_MSG), "[%s] DB COIL_RESULT �o�^ [%s][%s] �O�����M[%s]", my_sThreadName, pCoil->cKizukenNo, pCoil->data.cCoilNoIn, (bPutOk?"��":"�~"));
	LOG(em_MSG), "<<<----------------------------------------------");



	//================================================
	//// �o�^�˗�
	if( ! mque_pAddSQLData->AddToTail( deliDB, 20, 20) ) {			// ��Γo�^ 
		// ��Γo�^�������E�E�E
		LOG(em_ERR), "[%s] DB�o�^�˗��L���[�t�� [DBDATA_COIL_RESULT]", my_sThreadName);
		syslog(SYSNO_QUEFULL_ERR, "[DBDATA_COIL_RESULT]");
		delete deliDB->data;
		delete deliDB;
		// DB���ُ� (�ʏ킠�肦�Ȃ�)
		mcls_pStatus->AddQueMyErrWar( true, ON_QUE_COIL_RESULT );
	}
}

//==========================================
// �J�b�g�ʒu DB�ۑ�
// �� �V���[�J�b�g�m�莞
//------------------------------------------
// int					nMode	: �J�b�g�敪
// SHRCUT_ITEM const*	cut		: �J�b�g���
//==========================================
void CoilBaseManager::SaveDB_CoilCutPos(int nMode, SHRCUT_ITEM const* cut)
{
	//if(DIV_UNTEN_SMYU == mcls_pStatus->GetUnten()) return;
	COIL_INF* pCoil = m_pCoilDsp[POS_DSP_SHR_I];		// ���V���[�ʒu�ʉߒ��̃R�C�����|�C���^

		//================================================
		//// ���O����
		DBManager::DeliveryDB *deliDB = new DBManager::DeliveryDB;
		deliDB->kubun = DBManager::em_COILCUTPOS;
		deliDB->data = (BYTE*)new DBDATA_COIL_CUT_POS;
		DBDATA_COIL_CUT_POS* pWk   = (DBDATA_COIL_CUT_POS*)deliDB->data;		// �G�C���A�X
	
		//================================================
		//// �f�[�^�Z�b�g
		memcpy( pWk->cKizukenNo, cut->cKizukenNo, SIZE_KIZUKEN_NO);
		pWk->nMode		= nMode;
		memcpy(&pWk->data, &m_CoilOutInf, sizeof(m_CoilOutInf));
		
		//================================================
		//// �o�^�˗�
		if( ! mque_pAddSQLData->AddToTailFreeCheck(deliDB, 30) ) {	// �o�^
			LOG(em_ERR), "[%s] DB�o�^�˗��L���[�t�� [DB_COIL_CUT_POS]", my_sThreadName);
			syslog(SYSNO_QUEFULL_WAR, "[DB_COIL_CUT_POS]");
			delete deliDB->data;
			delete deliDB;
	}
}


//==========================================
// �r�摜�ۑ������Z�b�g�v��
// �� ���X�g�J�b�g��
//------------------------------------------
// char const* cKizukenNo	�Ǘ�No
//==========================================
void CoilBaseManager::ResetDB_DefectImg(char const* cKizukenNo)
{

	//================================================
	//// ���O����
	DBManager::DeliveryDB *deliDB = new DBManager::DeliveryDB;
	deliDB->kubun = DBManager::em_DB_COUNT_RESET;
	deliDB->data = (BYTE*)new DBDATA_COUNT_RESET;
	DBDATA_COUNT_RESET* pWk   = (DBDATA_COUNT_RESET*)deliDB->data;		// �G�C���A�X
	
	//================================================
	//// �f�[�^�Z�b�g
	memcpy( pWk->cKizukenNo, cKizukenNo, SIZE_KIZUKEN_NO);
	pWk->nTorB	= 2;					// ����
		
	//================================================
	//// �o�^�˗�
	if( ! mque_pAddSQLData->AddToTail( deliDB, 20, 20 ) ) {			// ��Γo�^
		// ��Γo�^�������E�E�E
		LOG(em_ERR), "[%s] DB�o�^�˗��L���[�t�� [DBDATA_COUNT_RESET]", my_sThreadName);
		syslog(SYSNO_QUEFULL_ERR, "[DBDATA_COUNT_RESET]");
		delete deliDB->data;
		delete deliDB;
	}
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// �J�����R���g���[���I������

//==========================================
// �w��J�����̘I�����Ԃ��擾
//------------------------------------------
// int	nMen	: 0�c�\, 1�c��	
// int	nCamPos	: 0�cDS, 1�cWS
// int	nAngId	: 0�c��, 1�c�� ���F����"0"�Œ�
//==========================================
int	CoilBaseManager::GetCamExpTime(int nMen, int nCamPos, int nAngId)
{
	if (NULL == mcls_pStatus) return 0;
	int nCam = (nMen * NUM_CAM_POS) + nCamPos;
	return mcls_pStatus->GetExpTime(nCam, nAngId);
}
#ifdef IS_TO_CAMERA_TBL
//==========================================
// �w��J�����̏�Ԃ��擾
//------------------------------------------
// int	nMen	: 0�c�\, 1�c��	
// int	nCamPos	: 0�cDS, 1�cWS
// int	nAngId	: 0�c��, 1�c�� ���F����"0"�Œ�
//==========================================
int	CoilBaseManager::GetCamMode(int nMen, int nCamPos, int nAngId)
{
	if(NULL == mtbl_pCam) return 0;
	int nCam = (nMen * NUM_CAM_POS * MAX_PHYSICAL_CAMANGLE) + (nCamPos * MAX_PHYSICAL_CAMANGLE) + nAngId;
	return mtbl_pCam->caminf[nCam].nMode; 
}
#endif
//==========================================
// �w��J�����̃J�����Q�C�����擾
//------------------------------------------
// int	nMen	: 0�c�\, 1�c��	
// int	nCamPos	: 0�cDS �` 3�cWS
// int	nColor	: 0�cR, 1�cG, 2�cB
//==========================================
float	CoilBaseManager::GetCamGain(int nMen, int nCamPos, int nColor)
{
	if (NULL == mcls_pStatus) return 0;
	int nCam = (nMen * NUM_CAM_POS) + nCamPos;
	return (float)(mcls_pStatus->GetCamGain(nCam, nColor) / 100.0);
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// �Ɩ����u
//==========================================
// �Ɩ����u�����l���擾
//------------------------------------------
// int	nMen	: 0�c�\, 1�c��	
// int	nColor	: 0�cR, 1,2�cG, 3,4�cB
//==========================================
int	CoilBaseManager::GetLedValue(int nMen, int nColor)
{
	if (NULL == mcls_pStatus) return 0;
	return mcls_pStatus->GetLedValue(nMen, nColor);
}



//////////////////////////////////////////////////////////////////////////////////
// ����

#define SET_DATA_COIL_S(prm1, prm2)			memcpy(prm1, prm2, sizeof(prm1));	
#define SET_DATA_COIL_V(prm1, prm2, size)	prm1 = KizuFunction::AtoIex(prm2, size);	

//==========================================
// TYPE_EPC_COIL�쐬
//------------------------------------------
// TYPE_EPC_COIL&	data			: �ϊ���̃f�[�^		
// const PRO_TYPE_DATA_COILINF&	p	: �R�s�[���R�C�����
//==========================================
void CoilBaseManager::SetTypeEpcCoil(TYPE_EPC_COIL & data, const PRO_TYPE_DATA_COILINF & p)
{

	// ������
	memset(&data, 0x00, sizeof(data));

	// �f�[�^�Z�b�g
	SET_DATA_COIL_S(data.cMeiNo,			p.CoilInf.KeyInf.cMeiNo);										// [01] ����No
	SET_DATA_COIL_S(data.cCoilNo,			p.CoilInf.KeyInf.cCoilNo);										// [02] �R�C��No
	data.nSendPoint = p.CoilInf.nSendPoint;																	// [03] ���M�ʒu (0:WPD3 1:WPD4 2:WPD6-50m)
	data.nLenIn = p.CoilInf.nLenIn;																			// [04] �����_���R�C�� ���� [m]	
	data.nAtuZai = p.CoilInf.nAtuZai;																		// [05] �ޗ���
	data.nWidZai = p.CoilInf.nWidZai;																		// [06] �ޗ���
	data.nAtuSei = p.CoilInf.nAtuSei;																		// [07] ������
	data.nWidSei = p.CoilInf.nWidSei;																		// [08] ������
	data.nWidTrim = p.CoilInf.nWidTrim;																		// [09] ���H���g������
	data.nNobiSPM = p.CoilInf.nNobiSPM;																		// [10] SPM�L�ї�
	data.nNobiTLV = p.CoilInf.nNobiTLV;																		// [11] TLV�L�ї�
	data.nCutF = p.CoilInf.nCutF;																			// [12] �����ɂ�����t�����g�[�J�b�g�̎�����
	data.nCutT = p.CoilInf.nCutT;																			// [13] �����ɂ�����e�[���[�J�b�g�̎�����
	data.nSPMRoolKei_Up = p.CoilInf.nSPMRoolKei_Up;															// [14] SPM���[�N���[���a�i��j
	data.nSPMRoolKei_Down = p.CoilInf.nSPMRoolKei_Down;														// [15] SPM���[�N���[���a�i���j

	data.nYobi[0] = 0;																						// [16] int�\��10
	data.nYobi[1] = 0;																						// [17] int�\��9
	data.nYobi[2] = 0;																						// [18] int�\��8
	data.nYobi[3] = 0;																						// [19] int�\��7
	data.nYobi[4] = 0;																						// [20] int�\��6
	data.nYobi[5] = 0;																						// [21] int�\��5
	data.nYobi[6] = 0;																						// [22] int�\��4
	data.nYobi[7] = 0;																						// [23] int�\��3
	data.nYobi[8] = 0;																						// [24] int�\��2
	data.nYobi[9] = 0;																						// [25] int�\��1

	SET_DATA_COIL_S(data.cKenKijun_O, p.CoilInf.cKenKijun_O);												// [26] ������@�\
	SET_DATA_COIL_S(data.cKenKijun_U, p.CoilInf.cKenKijun_U);												// [27] ������@��
	SET_DATA_COIL_S(data.cKurometo_Kind, p.CoilInf.cKurometo_Kind);											// [28] �N�����[�g ���ю��
	SET_DATA_COIL_S(data.cKurometo_Umu, p.CoilInf.cKurometo_Umu);											// [29] �N�����[�g ���їL��
	SET_DATA_COIL_S(data.cHinCode, p.CoilInf.cHinCode);														// [30] �i��
	SET_DATA_COIL_S(data.cMekkiKind, p.CoilInf.cMekkiCycle);												// [31] ���b�L���
	SET_DATA_COIL_S(data.cSyuZai, p.CoilInf.cSyuZai);														// [32] ��ގ�
	SET_DATA_COIL_S(data.cDifMaker, p.CoilInf.cDifMaker);													// [33] �f�B�t�@�����V�����}�[�J�[
	SET_DATA_COIL_S(data.cKenban, p.CoilInf.cKenban);														// [34] ���敪
	SET_DATA_COIL_S(data.cMekkiCycle, p.CoilInf.cMekkiCycle);												// [35] ���b�L�T�C�N��
	SET_DATA_COIL_S(data.cInLineSPM, p.CoilInf.cInLineSPM);													// [36] ���x���[�@�C�����C��SPM
	SET_DATA_COIL_S(data.cKagakuToyu, p.CoilInf.cKagakuToyu);												// [37] ���w�����h��
	SET_DATA_COIL_S(data.cAtukkaSPM, p.CoilInf.cAtukkaSPM);													// [38] �I�t���C��SPM������
	SET_DATA_COIL_S(data.cMekkiKubun, p.CoilInf.cMekkiKubun);												// [39] ���b�L�敪
	SET_DATA_COIL_S(data.cOmoteSyori1, p.CoilInf.cOmoteSyori1);												// [40] �\�ʏ���1����
	SET_DATA_COIL_S(data.cOmoteSyori2, p.CoilInf.cOmoteSyori2);												// [41] �\�ʏ���2����
	SET_DATA_COIL_S(data.cToyu, p.CoilInf.cToyu);															// [42] �h��
	SET_DATA_COIL_S(data.cKouteiYobi, p.CoilInf.cKouteiYobi);												// [43] �H���L���\��
	SET_DATA_COIL_S(data.cBaund, p.CoilInf.cBaund);															// [44] �o�E���_�����킹
	SET_DATA_COIL_S(data.cCaplSyuZai, p.CoilInf.cCaplSyuZai);												// [45] CAPL ��ގ�
	SET_DATA_COIL_S(data.cMetuke, p.CoilInf.cMetuke);														// [46] �ڕt�K�i
	SET_DATA_COIL_S(data.cNi, p.CoilInf.cNi);																// [47] Ni�t����
	SET_DATA_COIL_S(data.cSodoTCM, p.CoilInf.cSodoTCM);														// [48] TCM���[���e�x
	SET_DATA_COIL_S(data.cSpmUmu, p.CoilInf.cSpmUmu);														// [49] SPM�����L��
	SET_DATA_COIL_S(data.cNextLine, p.CoilInf.cNextLine);													// [50] �v�掟�H���R�[�h
	SET_DATA_COIL_S(data.cMuke, p.CoilInf.cMuke);															// [51] ����
	SET_DATA_COIL_S(data.cYouto, p.CoilInf.cYouto);															// [52] �p�r
	SET_DATA_COIL_S(data.cCaplSiage, p.CoilInf.cCaplSiage);													// [53] CAPL�\�ʎd�グ
	SET_DATA_COIL_S(data.cCaplSanUmu, p.CoilInf.cCaplSanUmu);												// [54] CAPL�_���L��
	SET_DATA_COIL_S(data.cHinMode, p.CoilInf.cHinMode);														// [55] �i��敪
	SET_DATA_COIL_S(data.cGaCrKbn, p.CoilInf.cGaCrKubun);													// [56] GA/CR�敪
	SET_DATA_COIL_S(data.cSpmJiseki, p.CoilInf.cSpmJisekiUmu);												// [57] SPM���їL��
	SET_DATA_COIL_S(data.cKouteiCode, p.CoilInf.cKouteiCode);												// [58] �H���R�[�h
	SET_DATA_COIL_S(data.cYobi, p.CoilInf.cYobi1);															// [59] 
}

//==========================================
// DBDATA_COIL_INF�쐬
//------------------------------------------
// DBDATA_COIL_INF&			data	: �ϊ���̃f�[�^
// PRO_TYPE_DATA_COILINF&	coil	: �R�s�[���R�C�����
//==========================================
void CoilBaseManager::SetTypeCoilInf(DBDATA_COIL_INF& data, const PRO_TYPE_DATA_COILINF& p)
{

	//// �f�[�^�Z�b�g
	memcpy(data.cCoilKey, &p.CoilInf.KeyInf, sizeof(p.CoilInf.KeyInf));

	SET_DATA_COIL_S(data.cCoilNo,					p.CoilInf.KeyInf.cCoilNo);			// �R�C��No
	SET_DATA_COIL_S(data.cMeiNo,					p.CoilInf.KeyInf.cMeiNo);			// ����No
	
	data.nSendPoint = p.CoilInf.nSendPoint;												// ���M�ʒu
	data.nLenIn = p.CoilInf.nLenIn;														// �����_���R�C�� ����
	data.nAtuZai = p.CoilInf.nAtuZai;													// �ޗ���
	data.nWidZai = p.CoilInf.nWidZai;													// �ޗ���
	data.nAtuSei = p.CoilInf.nAtuSei;													// �ޗ���
	data.nWidSei = p.CoilInf.nWidSei;													// �ޗ���
	data.nWidTrim = p.CoilInf.nWidTrim;													// ���H���g������
	data.nNobiSPM = p.CoilInf.nNobiSPM;													// SPM�L�ї�
	data.nNobiTLV = p.CoilInf.nNobiTLV;													// TLV�L�ї�
	data.nCutF = p.CoilInf.nCutF;														// �����ɂ�����t�����g�[�J�b�g�̎�����
	data.nCutT = p.CoilInf.nCutT;														// �����ɂ�����e�[���[�J�b�g�̎�����
	data.nSPMRoolKei_Up = p.CoilInf.nSPMRoolKei_Up;										// PSM���[�N���[���a�i��j
	data.nSPMRoolKei_Down = p.CoilInf.nSPMRoolKei_Down;									// SPM���[�N���[���a�i���j

	SET_DATA_COIL_S(data.cKenKijun_O, p.CoilInf.cKenKijun_O);							// ����� �\
	SET_DATA_COIL_S(data.cKenKijun_U, p.CoilInf.cKenKijun_U);							// ����� ��
	SET_DATA_COIL_S(data.cKurometo_Kind, p.CoilInf.cKurometo_Kind);						// �N�����[�g ���ю��
	SET_DATA_COIL_S(data.cKurometo_Umu, p.CoilInf.cKurometo_Umu);						// �N�����[�g ���їL��
	SET_DATA_COIL_S(data.cHinCode, p.CoilInf.cHinCode);									// �i��
	SET_DATA_COIL_S(data.cMekkiKind, p.CoilInf.cMekkiKind);								// ���b�L���
	SET_DATA_COIL_S(data.cSyuZai, p.CoilInf.cSyuZai);									// ��ގ�
	SET_DATA_COIL_S(data.cDifMaker, p.CoilInf.cDifMaker);								// �f�B�t�@�����V�����}�[�J�[
	SET_DATA_COIL_S(data.cKenban, p.CoilInf.cKenban);									// ���敪
	SET_DATA_COIL_S(data.cMekkiCycle, p.CoilInf.cMekkiCycle);							// ���b�L�T�C�N��
	SET_DATA_COIL_S(data.cInLineSPM, p.CoilInf.cInLineSPM);								// ���x���[ �C�����C��SPM
	SET_DATA_COIL_S(data.cKagakuToyu, p.CoilInf.cKagakuToyu);							// ���w�����h��
	SET_DATA_COIL_S(data.cAtukkaSPM, p.CoilInf.cAtukkaSPM);								// �I�t���C��SPM������
	SET_DATA_COIL_S(data.cMekkiKubun, p.CoilInf.cMekkiKubun);							// ���b�L�敪
	SET_DATA_COIL_S(data.cOmoteSyori1, p.CoilInf.cOmoteSyori1);							// �\�ʏ���1����
	SET_DATA_COIL_S(data.cOmoteSyori2, p.CoilInf.cOmoteSyori2);							// �\�ʏ���2����
	SET_DATA_COIL_S(data.cToyu, p.CoilInf.cToyu);										// �h��
	SET_DATA_COIL_S(data.cKouteiYobi, p.CoilInf.cKouteiYobi);							// �H���L���\��
	SET_DATA_COIL_S(data.cBaund, p.CoilInf.cBaund);										// �o�E���_�����킹
	SET_DATA_COIL_S(data.cCaplSyuZai, p.CoilInf.cCaplSyuZai);							// CAPL ��ގ�
	SET_DATA_COIL_S(data.cMetuke, p.CoilInf.cMetuke);									// �ڕt�K�i
	SET_DATA_COIL_S(data.cNi, p.CoilInf.cNi);											// Ni�t����
	SET_DATA_COIL_S(data.cSodoTCM, p.CoilInf.cSodoTCM);									// TCM���[���e�x
	SET_DATA_COIL_S(data.cSpmUmu, p.CoilInf.cSpmUmu);									// SPM�����L��
	SET_DATA_COIL_S(data.cNextLine, p.CoilInf.cNextLine);								// �v�掟�H���R�[�h
	SET_DATA_COIL_S(data.cMuke, p.CoilInf.cMuke);										// ����
	SET_DATA_COIL_S(data.cYouto, p.CoilInf.cYouto);										// �p�r
	SET_DATA_COIL_S(data.cCaplSiage, p.CoilInf.cCaplSiage);								// CAPL�\�ʎd�グ
	SET_DATA_COIL_S(data.cCaplSanUmu, p.CoilInf.cCaplSanUmu);							// CAPL�_���L��
	SET_DATA_COIL_S(data.cHinMode, p.CoilInf.cHinMode);									// �i��敪
	SET_DATA_COIL_S(data.cGaCrKubun, p.CoilInf.cGaCrKubun);								// GA/CR�敪
	SET_DATA_COIL_S(data.cSpmJisekiUmu, p.CoilInf.cSpmJisekiUmu);						// SPM�������їL��
	SET_DATA_COIL_S(data.cKouteiCode, p.CoilInf.cKouteiCode);							// �H���R�[�h
}

#undef SET_DATA_COIL_S
#undef SET_DATA_COIL_V



/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Get

//==========================================
// ���R�[�h�ʒu�擾
//------------------------------------------
// int		rec		: ����R�[�h
// int		index	: 0:���݈ʒu 1:��O 2:��O 3:�O�O -1:��
// �߂�l	int		: ���R�[�h�ʒu
//==========================================
int CoilBaseManager::GetRecIndex(int rec, int index)
{
	if(0 == index) {					// ���݈ʒu
		return rec;			
	} else {							// �w��O
		int recval = rec - index;
		if( 0 > recval) recval = SIZE_COIL_BUF + recval;
		if( SIZE_COIL_BUF <= recval) recval = recval - SIZE_COIL_BUF;
		return recval;
	}
}

//==========================================
// ���R�[�h�ʒu�擾
//------------------------------------------
// char const*	pKizukenNo	: �Ǘ�No
// �߂�l		int			: ���R�[�h�ʒu
//==========================================
int CoilBaseManager::GetRecKizukenNo(char const* pKizukenNo)
{
	COIL_INF const* pWk;
	if(-1 == m_nKenRecBase) return -1;

	for(int ii=0; ii<SIZE_COIL_BUF; ii++) {
		pWk = GetCoilIndex(m_nKenRecBase, ii);

		if(0 == memcmp( pWk->cKizukenNo, pKizukenNo, SIZE_KIZUKEN_NO ) ) return pWk->rec;
	}
	// �����܂ŗ�����Y������
	return -1;
}

//==========================================
// �w�肳�ꂽ�Ǘ�No�̃R�C�����
//------------------------------------------
// char const*	pKizukenNo	: �Ǘ�No
// �߂�l		COIL_INF*	: �R�C�����
//==========================================
const COIL_INF* CoilBaseManager::GetCoilKizukenNo(char const* pKizukenNo)
{
	COIL_INF const* pWk;
	if(-1 == m_nKenRecBase) return NULL;

	for(int ii=0; ii<SIZE_COIL_BUF; ii++) {
		pWk = GetCoilIndex(m_nKenRecBase, ii);

		if(0 == memcmp( pWk->cKizukenNo, pKizukenNo, SIZE_KIZUKEN_NO ) ) return pWk;
	}
	// �����܂ŗ�����Y������
	return NULL;
}

//------------------------------------------
// �w�肳�ꂽ�Ǘ�No�̃R�C�����  (�Ă΂�邽�тɃC���N���������g����邽�ߒ���)
// COIL_INF* pCoil �Ώۂ̃R�C�����
// �߂�l:�rNo(1�I���W��)
//------------------------------------------
int CoilBaseManager::GetKizuNo(COIL_INF* pCoil)
{
	AutoLock autolock(&m_csLock);

	pCoil->nKizuNoNum ++;
	return pCoil->nKizuNoNum;
}

//==========================================
// �S�����苗��
//------------------------------------------
// int		pos			: 
// �߂�l	COIL_INF*	: �R�C�����	
//==========================================
const COIL_INF* CoilBaseManager::GetCoilAll(int pos) const
{
	if (pos == DIV_LEN_POS_KEN_TOP || pos == DIV_LEN_POS_KEN_BOT) {
		int nMen = pos - DIV_LEN_POS_KEN_TOP;
		return GetCoilKenR(nMen);
	} else if( pos == DIV_LEN_POS_DSP_TOP) {
		return GetCoilBaseDsp();

	} else if( pos == DIV_LEN_POS_DSP_BOT) {
		return GetCoilBaseDsp();

	} else {
		return GetCoilDsp( (pos- DIV_DISP_POS_DSP_P1) );
	}
}



/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// EPC�� �S���ۑ�PC�p

//==========================================
// UDP SOCKET����
//------------------------------------------
// const char*	ip		: IP�A�h���X (��F192.9.200.255)
// WORD			port	: �|�[�gNo (��F8100)
// �߂�l		BOOL	: ���A���
//==========================================
BOOL CoilBaseManager::SetUdpAllFrame(const char* ip, WORD port)
{
	// ������
	m_sockAllFrame = INVALID_SOCKET;
	memset(&m_addrAllFrame, 0x00, sizeof(m_addrAllFrame));

	// �\�P�b�g����
	WSADATA wsaData;
	WORD wVersionRequired = MAKEWORD(2, 2);
	WSAStartup(wVersionRequired, &wsaData);
	m_sockAllFrame = socket(AF_INET, SOCK_DGRAM, 0);
	if (INVALID_SOCKET == m_sockAllFrame) return FALSE;
	m_addrAllFrame.sin_family = AF_INET;
	m_addrAllFrame.sin_addr.s_addr = inet_addr(ip);
    m_addrAllFrame.sin_port = htons(port);
	return TRUE;
}

//==========================================
//// UDP SOCKET�j��
//==========================================
void CoilBaseManager::ReSetUdpAllFrame()
{
	if( INVALID_SOCKET != m_sockAllFrame ) closesocket(m_sockAllFrame);
	m_sockAllFrame = INVALID_SOCKET;
}

//==========================================
// �R�C����� �� ���摜PC�ɑ��M
//------------------------------------------
// TYPE_EPC_COIL*	pData	: �R�C�����
//==========================================
void CoilBaseManager::SendUdpAllFrame(TYPE_EPC_COIL* pData)
{
	// UDP�p�P�b�g���M
	sendto( m_sockAllFrame, (char *)pData, sizeof(TYPE_EPC_COIL), 0, (sockaddr *)&m_addrAllFrame, sizeof(sockaddr_in));
}

//==========================================
// ���R�[�h�ʒu�擾
//------------------------------------------
// int		rec		: ����R�[�h
// int		index	: 0:���݈ʒu 1:��O 2:��O 3:�O�O -1:��
// �߂�l	int		: ���R�[�h�ʒu
//==========================================
int CoilBaseManager::GetShaRecIndex(int rec, int index)
{
	if(0 == index) {					// ���݈ʒu
		return rec;			
	} else {							// �w��O
		int recval = rec - index;
		if( 0 > recval) recval = SIZE_COIL_OUT_BUF + recval;
		if( SIZE_COIL_BUF <= recval) recval = SIZE_COIL_OUT_BUF - recval;
		return recval;
	}
}



//------------------------------------------
// �R�C�����т̍X�V
// COIL_INF const*	pCoilInf �r���܂܂��R�C�����
//------------------------------------------
void CoilBaseManager::SaveDB_UpdateCoilResult(EM_MODE_COILRESULT_UPDATE emMode, COIL_INF const* pCoilInf)
{

	// ���O����
	DBManager::DeliveryDB* pDeli = new DBManager::DeliveryDB;
	pDeli->kubun = DBManager::em_COILRESULT_UPDATE;
	pDeli->data = (BYTE*)new DBDATA_COIL_RESULT_UPDATE;
	DBDATA_COIL_RESULT_UPDATE* pWk = (DBDATA_COIL_RESULT_UPDATE*)pDeli->data;
	memset(pWk, 0x00, sizeof(DBDATA_COIL_RESULT_UPDATE));

	// �f�[�^�Z�b�g
	memcpy(pWk->cKizukenNo, pCoilInf->cKizukenNo, SIZE_KIZUKEN_NO);
	pWk->emMode = emMode;

	switch (emMode) {
	case MODE_RECV_SHRCUT:
		pWk->nOutCoilPos = pCoilInf->nOutCoilPos;
		break;
	case MODE_DSP_WPD_ON:
		pWk->nDspLength = pCoilInf->nDspLength;
		pWk->nSpmUmu = pCoilInf->nSt_Spm;
		break;
	}


	// �o�^�˗�
	if (!mque_pAddSQLData->AddToTailFreeCheck(pDeli, 20)) {
		// �L���[�t��
		delete pDeli->data;
		delete pDeli;

		// DB���ُ� (�ʏ킠�肦�Ȃ�)
		LOG(em_ERR), "[%s] DB�o�^�˗��L���[�t�� [DBDATA_COIL_RESULT_UPDATE]", my_sThreadName);
		syslog(SYSNO_QUEFULL_WAR, "[DBDATA_COIL_RESULT_UPDATE]");
		mcls_pStatus->AddQueMyErrWar( true, ON_QUE_COIL_RESULT );
	}
}