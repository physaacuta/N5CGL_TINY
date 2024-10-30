#include "stdafx.h"
#include "DmemManager.h"


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
DmemManager::DmemManager() :
m_nState(-1),
m_bInitReadOk(false),
m_nHBOldVal(-1),					// �Ⴄ�l�ɂ���ׁA���肦�Ȃ����l�������l�Ƃ��Ă���
ThreadManager("DmenMgr")
{
//*/----> ��
#ifdef LOCAL
	m_hDataMem = CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, sizeof(TO_SEQMEM_TBL), TBL_TO_SEQMEM_NAME);
	mtbl_pDM =(TO_SEQMEM_TBL*) MapViewOfFile(m_hDataMem, FILE_MAP_ALL_ACCESS, 0, 0, 0);
#endif
//*///<<---

	//// ���L������
	// �V�[�P���T��n���e�[�u��
	mem_connect(&m_hDataMem, (LPVOID *)&mtbl_pDM, TBL_TO_SEQMEM_NAME);
	if(m_hDataMem == NULL) {
		syslog(SYSNO_MEM_OPEN, "�f�[�^���������e�[�u�� �A�N�Z�X���s");
		_ASSERT(FALSE);
	}

	// �g���₷���悤�ɃG���A�Z�b�g
	mtbl_pRecv = (SEQ_DM_RECV_BASE*)&mtbl_pDM->fb_dm[DM_RECV_REC];
	mtbl_pSend = (SEQ_DM_SEND_DEFECT*)&mtbl_pDM->fb_dm[DM_SEND_REC];
	memset(&m_typOldDmRecv, 0x00, sizeof(m_typOldDmRecv));
	memset(&m_typOldDmRecvLow, 0x00, sizeof(m_typOldDmRecvLow));


	// �V�O�i��
	m_evTimeHlsRead		= CreateWaitableTimer(NULL, FALSE, NULL);
	m_evTimeHlsWrit		= CreateWaitableTimer(NULL, FALSE, NULL);
	m_evTimeStatusRead	= CreateWaitableTimer(NULL, FALSE, NULL);
	m_evTimeStatusWrit	= CreateWaitableTimer(NULL, FALSE, NULL);
	m_evTimeAlarmePulse = CreateWaitableTimer(NULL, FALSE, NULL);
	m_evTimeCntResetPulse = CreateWaitableTimer(NULL, FALSE, NULL);
	m_evTimeCntStartPulse = CreateWaitableTimer(NULL, FALSE, NULL);
	m_evTimeTestWpdPulse = CreateWaitableTimer(NULL, FALSE, NULL);
	InitializeCriticalSection(&m_csLock);								// �N���e�B�J���Z�N�V�����I�u�W�F�N�g��������
	
	m_evPI_WPD_KenBaseGateOn = CreateEvent(NULL, FALSE, FALSE, NULL);
	m_evPI_WPD_KenBaseGateOff = CreateEvent(NULL, FALSE, FALSE, NULL);
	m_evPI_WPD_DspBaseGateOn = CreateEvent(NULL, FALSE, FALSE, NULL);
	m_evPI_WPD_DspBaseGateOff = CreateEvent(NULL, FALSE, FALSE, NULL);
	m_evPI_WPD_KenBase = CreateEvent(NULL, FALSE, FALSE, NULL);
	m_evPI_WPD_DspBase = CreateEvent(NULL, FALSE, FALSE, NULL);
	m_evPI_ShrcutF = CreateEvent(NULL, FALSE, FALSE, NULL);
	m_evPI_ShrcutL = CreateEvent(NULL, FALSE, FALSE, NULL);
	m_evPI_PatoReset = CreateEvent(NULL, FALSE, FALSE, NULL);


	// ���̑�
	memset( m_nOutPls, 0x00, sizeof(m_nOutPls));
	m_nVoiceMen = -1;
	m_nVoice = -1;
	memset(m_nRamp, -1, sizeof(m_nRamp));
	memset(m_nPato, 0x00, sizeof(m_nPato));
	memset(m_nAlarm, -1, sizeof(m_nAlarm));
	// ===>> �⊪ 2021.06.03 ���R�C�����ޒʔ̎��A�R�C������r������ɂł��Ȃ��s��C��
	m_nWpdOnLength = 0;				// WPD��ON���̌����R�C����
	m_nWpdOnLengthDsp = 0;			// WPD��ON���̕\���R�C����
	m_nWpdOnLengthSub = 0;			// WPD��ON���̔�r�R�C����
	// <<===

}

//------------------------------------------
// �f�X�g���N�^
//------------------------------------------
DmemManager::~DmemManager()
{
	//// ���L�������J��
	mtbl_pRecv = NULL;
	mtbl_pSend = NULL;
	mem_close(&m_hDataMem, (LPVOID *)&mtbl_pDM);				// �f�[�^���������

	// �V�O�i��
	CancelWaitableTimer(m_evTimeHlsRead);
	CancelWaitableTimer(m_evTimeHlsWrit);
	CancelWaitableTimer(m_evTimeStatusRead);
	CancelWaitableTimer(m_evTimeStatusWrit);
	CancelWaitableTimer(m_evTimeAlarmePulse);
	CancelWaitableTimer(m_evTimeCntResetPulse);
	CancelWaitableTimer(m_evTimeCntStartPulse);
	CancelWaitableTimer(m_evTimeTestWpdPulse);
	CloseHandle(m_evTimeHlsRead);
	CloseHandle(m_evTimeHlsWrit);
	CloseHandle(m_evTimeStatusRead);
	CloseHandle(m_evTimeStatusWrit);
	CloseHandle(m_evTimeAlarmePulse);
	CloseHandle(m_evTimeCntResetPulse);
	CloseHandle(m_evTimeCntStartPulse);
	CloseHandle(m_evTimeTestWpdPulse);
	DeleteCriticalSection(&m_csLock);									// �N���e�B�J���Z�N�V�����I�u�W�F�N�g���J��

	CloseHandle(m_evPI_WPD_KenBaseGateOn);
	CloseHandle(m_evPI_WPD_KenBaseGateOff);
	CloseHandle(m_evPI_WPD_DspBaseGateOn);
	CloseHandle(m_evPI_WPD_DspBaseGateOff);
	CloseHandle(m_evPI_WPD_KenBase);
	CloseHandle(m_evPI_WPD_DspBase);
	CloseHandle(m_evPI_ShrcutF);
	CloseHandle(m_evPI_ShrcutL);
	CloseHandle(m_evPI_PatoReset);
}

//------------------------------------------
// ��ԕω��`�F�b�N
// int now	����l
// int old	�O��l
// �߂�l	�ύX��� (9:�ύX����(ON) -9:�ύX����(OFF) 1:OFF��ON -1:ON��OFF)
//------------------------------------------
int DmemManager::ChangeState(int now, int old )
{
	if( 1 == now ) {		// ���� ����
		if( 0 == old )	return 1;	// OFF��ON
		else			return 9;	// ON�̂܂�
	} else {				// ���� �ُ�
		if( 1 == old )	return -1;	// ON��OFF
		else			return -9;	// OFF�̂܂�
	}
}


// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// �X���b�h����

//------------------------------------------
// �X���b�h�J�n�O���� (�X���b�h���s����(�V�O�i���Z�b�g�O)�ɃR�[���o�b�N)
//------------------------------------------
int DmemManager::ThreadFirst()
{
	//// �X�e�[�^�X�p�����o�ϐ���������
	m_nPlsRate[0] = (int)(mcls_pParam->GetParamCommon().dPls * PLS_RATE_TANI);
	m_nPlsRate[1] = (int)(mcls_pParam->GetParamCommon().dPlsSub * PLS_RATE_TANI);
	m_nPlsRate[2] = (int)(mcls_pParam->GetParamCommon().dPlsDsp * PLS_RATE_TANI);
	// �p���X���[�g�A���蕪��\��DB�X�V(���蕪��\�Ƃ̕s��v�h�~�΍�)
	UpdatePlsRate(m_nPlsRate);

	//// ������V�O�i���ݒ�
	SetEvWaitableTimer(m_evTimeHlsRead,		100, PARAM_TIME_HLS_READ);
	SetEvWaitableTimer(m_evTimeHlsWrit,		100, PARAM_TIME_HLS_WRIT);
	SetEvWaitableTimer(m_evTimeStatusRead,	100, PARAM_TIME_STATUS_READ);
	SetEvWaitableTimer(m_evTimeStatusWrit,	100, PARAM_TIME_STATUS_WRIT);


	//// �C�x���g�ݒ�
	AddEventNum(1, &mque_AddData.g_evSem);
	AddEventNum(1, &m_evTimeHlsRead);
	AddEventNum(1, &m_evTimeHlsWrit);
	AddEventNum(1, &m_evTimeStatusRead);
	AddEventNum(1, &m_evTimeStatusWrit);
	AddEventNum(1, &m_evTimeAlarmePulse);
	AddEventNum(1, &m_evTimeCntResetPulse);
	AddEventNum(1, &m_evTimeCntStartPulse);
	AddEventNum(1, &m_evTimeTestWpdPulse);

	//// �����l�Z�b�g
	m_nStatusReadLowInterval = GetTickCount();
	return 0;
}

//------------------------------------------
// �X���b�h�C�x���g����
// int nEventNo �C�x���gNo (0�I���W��)
//------------------------------------------
void DmemManager::ThreadEvent(int nEventNo)
{
	//AutoLock autolock(&m_csLock);			// �L���[�n���������h�N�T�C�Ƃ��낪����̂ŁB


	////// �V�O�i���̏���
	enum {	EV_QUE = 0,							// �O������L���[
			EV_HLS_READ,						// �w���V�[�Ǎ�
			EV_HLS_WRIT,						// �w���V�[����
			EV_STATUS_READ,						// �X�e�[�^�X�M���ǂݍ���
			EV_STATUS_WRIT,						// �X�e�[�^�X�M����������
			EV_ALARME_PULSE,					// �r�ڋ߃{�C�X�o�̓p���X��~
			EV_CNT_RESET_PULSE,					// ���@�\�J�E���^���Z�b�g�p���X��~
			EV_CNT_START_PULSE,					// ���@�\�J�E���^�J�n�p���X��~
			EV_TEST_WPD_PULSE					// �e�X�g�p WPD�M��ON�v���p���X��~
	};

	////// �V�O�i����������
	switch (nEventNo) {

//-----------------------------------------------------------------------------------------------
	case EV_QUE: 									// �O������L���[
	{
		DELIVERY* pDeli = mque_AddData.GetFromHead();		// �o�^�˗��f�[�^
		switch (pDeli->emNo) {
			//-----------------------
		case em_RAMP:										// �r�ڋ߃����v
			if (true) {
				OCCUR_RAMP typWk;
				memcpy(&typWk, pDeli->data, sizeof(OCCUR_RAMP));

				WritDmemRamp(typWk);
			}
			break;
			//-----------------------
		case em_ALARM:										// �r�ڋ߃A���[��
			if (true) {
				OCCUR_ALARM typWk;
				memcpy(&typWk, pDeli->data, sizeof(OCCUR_ALARM));

				WritDmemAlarm(typWk);
			}
			break;
			//-----------------------
		case em_PATO:										// �p�g���C�g
			if (true) {
				OCCUR_PATO typWk;
				memcpy(&typWk, pDeli->data, sizeof(OCCUR_PATO));

				WritDmemPato(typWk);
			}
			break;
			//-----------------------
		case em_VOICE:										// �{�C�X
			if (m_nVoiceMen != -1 && m_nVoice != -1)
			{
				LOGA(em_MSG), "[%s] �{�C�X�o�͒��ɂ��v���j��", my_sThreadName);
			}
			else
			{
				OCCUR_VOICE typWk;
				memcpy(&typWk, pDeli->data, sizeof(OCCUR_VOICE));

				WritDmemVoice(typWk);
			}
			break;
		}
		// �J��
		SAFE_DELETE(pDeli->data);
		SAFE_DELETE(pDeli);

		break;
	}
//-----------------------------------------------------------------------------------------------
	case EV_HLS_READ:										// �w���V�[�`�F�b�N
		ReadDmemHls();
		break;
	
//-----------------------------------------------------------------------------------------------
	case EV_HLS_WRIT:										// �w���V�[��������
		WritDmemHls();
		//Out1sPls();										// 1�b�p���X�o��
		break;
	

//-----------------------------------------------------------------------------------------------
	case EV_STATUS_READ:									// �X�e�[�^�X��ԓǂݍ���
		if (true) {
			//================================================
			// ���O����
			//// �ꊇ�ŋ��L�������ɏ����ׁA��x�o�b�t�@���ŕҏW
			SEQ_DM_RECV_BASE tbl;
			memcpy(&tbl, mtbl_pRecv, sizeof(tbl));

			// �����p
			ReadDmemStatus(tbl);

			// �ᑬ�p
			if( GetTickCount() > m_nStatusReadLowInterval+ PARAM_TIME_STATUS_READ_LOW) {
				ReadDmemStatusLow(tbl);
				m_nStatusReadLowInterval = GetTickCount();
				// �O��l�R�s�[
				memcpy(&m_typOldDmRecvLow, &tbl, sizeof(SEQ_DM_RECV_BASE));		// �O��l�ۑ�

			}
		
			// �O��l�R�s�[
			memcpy(&m_typOldDmRecv, &tbl, sizeof(SEQ_DM_RECV_BASE));		// �O��l�ۑ�
		}
		break;
	

//-----------------------------------------------------------------------------------------------
	case EV_STATUS_WRIT:									// �X�e�[�^�X��ԏ�������
		WritDmemStatus();
		break;
	
//-----------------------------------------------------------------------------------------------
	case EV_ALARME_PULSE:									// �r�ڋ߃{�C�X�o�̓p���X��~
		WritDmemVoiceReset();
		break;

//-----------------------------------------------------------------------------------------------
	case EV_CNT_RESET_PULSE:								// ���@�\�J�E���^���Z�b�g�p���X��~
		mtbl_pSend->nCntReset = 0;

		//// PLG�J�E���^���Z�b�g�v�����t���O��OFF
		//m_bPlgReset = false;
		break;

//-----------------------------------------------------------------------------------------------
	case EV_CNT_START_PULSE:								// ���@�\�J�E���^�J�n�p���X��~
		mtbl_pSend->nCntStart = 0;

		//// PLG�J�E���^���Z�b�g�v�����t���O��OFF
		//m_bPlgReset = false;
		break;

//-----------------------------------------------------------------------------------------------
	case EV_TEST_WPD_PULSE:									// �e�X�g�p WPD�M��ON�v���p���X��~
		mtbl_pSend->nTestWpdReq = 0;
		break;
	

//-----------------------------------------------------------------------------------------------
	default:
		// ���肦�Ȃ��I�I
		LOG(em_ERR), "[%s] ThreadEvent EventNo NG=%d", my_sThreadName, nEventNo );
		break;
	}
}

// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ����
//	 �� ���̒��ł́A�u���b�N����Ȃ��悤�ɂ��邱�ƁB
//   �� ���ɁA�@���ԕύX��DB�Ńu���b�N�����\��������̂ŃL���[�^�C�v�ɂ��邱��


//------------------------------------------
// �w���V�[�ǂݍ���
//------------------------------------------
void DmemManager::ReadDmemHls()
{
	//// �O��Ɠ���l�Ȃ�ُ�
	if( m_nHBOldVal == mtbl_pRecv->nHBCnt ) {						// �ُ�F��
		// ���큨�ُ�
		if(1 != m_nState) {
			LOG(em_ERR), "[%s] �w���V�[�ُ�", my_sThreadName);
			syslog(601, "");
			mcls_pStatus->SetStatusQue("DATAMEM_SEQ_HB_ID", false);
			m_nState = 1;
		}
		
	} else {														// ����F��
		// �ُ큨����
		if(0 != m_nState) {
			LOG(em_MSG), "[%s] �w���V�[���한�A", my_sThreadName);
			syslog(600, "");
			mcls_pStatus->SetStatusQue("DATAMEM_SEQ_HB_ID", true);
			m_nState = 0;
		}

		//// �^�X�N�N���㏉�߂Ă̐���F��
		if (!m_bInitReadOk) {
			m_bInitReadOk = true;
		}
	}

	//// �O��l�擾
	m_nHBOldVal = mtbl_pRecv->nHBCnt;
}

//------------------------------------------
// �w���V�[��������
//------------------------------------------
void DmemManager::WritDmemHls()
{
	if( HB_MAX_VAL <= mtbl_pSend->typKensaStat.nHBCnt )	mtbl_pSend->typKensaStat.nHBCnt  = 1;
	else											mtbl_pSend->typKensaStat.nHBCnt += 1;

	//// PC����
 //   SYSTEMTIME time;
 //   GetLocalTime(&time);
	//mtbl_pSend->nPcTime[0] = time.wYear;
	//mtbl_pSend->nPcTime[1] = time.wMonth;
	//mtbl_pSend->nPcTime[2] = time.wDay;
	//mtbl_pSend->nPcTime[3] = time.wHour;
	//mtbl_pSend->nPcTime[4] = time.wMinute;
	//mtbl_pSend->nPcTime[5] = time.wSecond;
}

//------------------------------------------
// �X�e�[�^�X��ԏ�������
//------------------------------------------
void DmemManager::WritDmemStatus()
{
	int nWk;

	//================================================
	// ���O����
	//// �ꊇ�ŋ��L�������ɏ����ׁA��x�o�b�t�@���ŕҏW
	SEQ_DM_SEND_DEFECT tbl;
	memcpy( &tbl, mtbl_pSend, sizeof(tbl));


	//================================================
	// �r���X�e�[�^�X �o�^

	//// �ғ����

	nWk = mcls_pStatus->GetKadou();
		 if(DIV_KADOU_INIT == nWk )		tbl.typKensaStat.nKadou = 1;
	else if(DIV_KADOU_STOP == nWk )		tbl.typKensaStat.nKadou = 2;
	else if(DIV_KADOU_START == nWk )	tbl.typKensaStat.nKadou = 3;
	else if(DIV_KADOU_RETRY == nWk )	tbl.typKensaStat.nKadou = 4;
	else								tbl.typKensaStat.nKadou = 0;

	for(int ii=0; ii<NUM_MEN; ii++) {
		//// �@����
		nWk = mcls_pStatus->GetKiki(ii);
			 if(DIV_KIKI_OK == nWk )		tbl.typKensaStat.nKiki[ii] = 0;
		else if(DIV_KIKI_KEI == nWk )		tbl.typKensaStat.nKiki[ii] = 1;
		else if(DIV_KIKI_JYU == nWk )		tbl.typKensaStat.nKiki[ii] = 3;
		else								tbl.typKensaStat.nKiki[ii] = 3;

		//// �������
		nWk = mcls_pStatus->GetKensa(ii);
			 if(DIV_KENSA_NON == nWk )		tbl.typKensaStat.nKensa[ii] = 9;
		else if(DIV_KENSA_OK == nWk )		tbl.typKensaStat.nKensa[ii] = 0;
		else if(DIV_KENSA_NG == nWk )		tbl.typKensaStat.nKensa[ii] = 1;
		//else if(DIV_KENSA_MENTE == nWk )	tbl.typKensaStat.nKensa[ii] = 1;
		else if(DIV_KENSA_EDGENG == nWk )	tbl.typKensaStat.nKensa[ii] = 4;
		else if(DIV_KENSA_DBNON == nWk )	tbl.typKensaStat.nKensa[ii] = 5;
		else if(DIV_KENSA_SKIP == nWk )		tbl.typKensaStat.nKensa[ii] = 6;
		else if(DIV_KENSA_STOP == nWk )		tbl.typKensaStat.nKensa[ii] = 9;
		else								tbl.typKensaStat.nKensa[ii] = 1;
	}

	// PLG���
	tbl.typKensaStat.nPlg = (int)mcls_pStatus->GetPlg() - 1;
	
	// �ʏ�^�]���[�h�̂�
	if(DIV_UNTEN_NOMAL == mcls_pStatus->GetUnten() ||
	   DIV_UNTEN_STOP  == mcls_pStatus->GetUnten() ) {
		memcpy( &mtbl_pSend->typKensaStat, &tbl.typKensaStat, sizeof(tbl.typKensaStat));
	}
}

//------------------------------------------
// �X�e�[�^�X��ԓǂݍ���
//------------------------------------------
void DmemManager::ReadDmemStatus(SEQ_DM_RECV_BASE& tbl)
{
//#ifdef LOCAL
//	return;
//#endif
	
	//================================================
	// �p���X�֌W����   

	// ����WPD�Q�[�g�M��
	if (1 == ChangeState(tbl.nGateKen, m_typOldDmRecv.nGateKen)) {
		LOG(em_MSG), "[%s] ����WPD�Q�[�g�M�� ON", my_sThreadName);
		SetEvent(m_evPI_WPD_KenBaseGateOn);
	}
	else if(-1 == ChangeState(tbl.nGateKen, m_typOldDmRecv.nGateKen)){
		LOG(em_MSG), "[%s] ����WPD�Q�[�g�M�� OFF", my_sThreadName);
		SetEvent(m_evPI_WPD_KenBaseGateOff);
	}

	// �\��WPD�Q�[�g�M��
	if (1 == ChangeState(tbl.nGateDsp, m_typOldDmRecv.nGateDsp)) {
		LOG(em_MSG), "[%s] �\��WPD�Q�[�g�M�� ON", my_sThreadName);
		SetEvent(m_evPI_WPD_DspBaseGateOn);
	}
	else if(-1 == ChangeState(tbl.nGateDsp, m_typOldDmRecv.nGateDsp)){
		LOG(em_MSG), "[%s] �\��WPD�Q�[�g�M�� OFF", my_sThreadName);
		SetEvent(m_evPI_WPD_DspBaseGateOff);
	}	

	// ����WPD  �������オ��̂�
	if( 1 == ChangeState(tbl.pls.nWpd[0], m_typOldDmRecv.pls.nWpd[0]) ) {
		LOG(em_MSG), "[%s] ����WPD ON [�t���[��No:%d]", my_sThreadName, tbl.ken.nWpdOnFrameNo);
		// ===>> �⊪ 2021.06.03 ���R�C�����ޒʔ̎��A�R�C������r������ɂł��Ȃ��s��C��
		m_nWpdOnLength = tbl.nWpdOnLength;
		m_nWpdOnLengthSub = tbl.nWpdOnLengthSub;
		// <<===
		SetEvent(m_evPI_WPD_KenBase);
	}

	// �\��WPD  �������オ��̂�
	if( 1 == ChangeState(tbl.pls.nWpd[2], m_typOldDmRecv.pls.nWpd[2]) ) {
		LOG(em_MSG), "[%s] �\��WPD ON", my_sThreadName);
		// ===>> �⊪ 2021.06.03 ���R�C�����ޒʔ̎��A�R�C������r������ɂł��Ȃ��s��C��
		m_nWpdOnLengthDsp = tbl.nWpdOnLengthDsp;
		// <<===
		SetEvent(m_evPI_WPD_DspBase);
	}

	// �o���t�@�[�X�g�J�b�g�M��
	if (1 == ChangeState(tbl.pls.nSharCutF, m_typOldDmRecv.pls.nSharCutF)) {
		LOG(em_MSG), "[%s] �o���t�@�[�X�g�J�b�g�M�� ON", my_sThreadName);
		SetEvent(m_evPI_ShrcutF);
	}

	// �o�����X�g�J�b�g�M��
	if (1 == ChangeState(tbl.pls.nSharCutL, m_typOldDmRecv.pls.nSharCutL)) {
		LOG(em_MSG), "[%s] �o�����X�g�J�b�g�M�� ON", my_sThreadName);
		SetEvent(m_evPI_ShrcutL);
	}

	// �p�g���C�g���Z�b�gPB
	if (1 == ChangeState(tbl.nPatoResetPb, m_typOldDmRecv.nPatoResetPb)) {
		LOG(em_MSG), "[%s] �p�g���C�g���Z�b�gPB ON", my_sThreadName);
		LOGA(em_MSG), "[%s] �p�g���C�g���Z�b�gPB ON", my_sThreadName);
		SetEvent(m_evPI_PatoReset);
	}
}

//------------------------------------------
// �ᑬ�p�X�e�[�^�X��ԓǂݍ���
//------------------------------------------
void DmemManager::ReadDmemStatusLow(SEQ_DM_RECV_BASE& tbl)
{
//#ifdef LOCAL
//	return;
//#endif
//	
	//int nVal;
	int nWk[2];

	//================================================
	// ����

	// �@���Ԋ֌W
	// �V�[�P���T���
	nWk[0] = tbl.nSeqSt;
	if( 0 == nWk[0] && 0 != mcls_pStatus->GetStatusRead().SeqState.stat ) {
		mcls_pStatus->SetStatusQue( mcls_pStatus->GetStatusRead().SeqState.id, 0, false);
		mcls_pStatus->SetStatusQue( mcls_pStatus->GetStatusRead().SeqState_War.id, 0, false);
		mcls_pStatus->SetStatusQue( mcls_pStatus->GetStatusRead().SeqState_Err.id, 0, false);
		LOG(em_MSG), "[%s] �V�[�P���T��ԕ���", my_sThreadName);
		syslog(602, "���한�A");
	}
	else if( 1 == nWk[0] && 1 != mcls_pStatus->GetStatusRead().SeqState.stat ) {
		mcls_pStatus->SetStatusQue( mcls_pStatus->GetStatusRead().SeqState.id, 1, false);
		mcls_pStatus->SetStatusQue(mcls_pStatus->GetStatusRead().SeqState_War.id, 1, false);
		mcls_pStatus->SetStatusQue(mcls_pStatus->GetStatusRead().SeqState_Err.id, 0, false);
		LOG(em_WAR), "[%s] �V�[�P���T �y�̏���", my_sThreadName);
		syslog(603, "[�y�̏�]");
	}
	else if( 2 == nWk[0] && 2 != mcls_pStatus->GetStatusRead().SeqState.stat ) {
		mcls_pStatus->SetStatusQue( mcls_pStatus->GetStatusRead().SeqState.id, 2, false);
		mcls_pStatus->SetStatusQue(mcls_pStatus->GetStatusRead().SeqState_War.id, 0, false);
		mcls_pStatus->SetStatusQue(mcls_pStatus->GetStatusRead().SeqState_Err.id, 1, false);
		LOG(em_ERR), "[%s] �V�[�P���T �d�̏���", my_sThreadName);
		syslog(603, "[�d�̏�]");
	}

	// �V�[�P���T�ғ����
	nWk[0] = tbl.nSeqKadou;
	if (0 == nWk[0] && 0 != mcls_pStatus->GetStatusRead().SeqKadou.stat) {
		mcls_pStatus->SetStatusQue(mcls_pStatus->GetStatusRead().SeqKadou.id, 0, false);
		LOG(em_MSG), "[%s] �V�[�P���T�ғ����: ����", my_sThreadName);
		syslog(604, "[����]");
	}
	else if (1 == nWk[0] && 1 != mcls_pStatus->GetStatusRead().SeqKadou.stat) {
		mcls_pStatus->SetStatusQue(mcls_pStatus->GetStatusRead().SeqKadou.id, 1, false);
		LOG(em_MSG), "[%s] �V�[�P���T�ғ����: �������ς�", my_sThreadName);
		syslog(604, "[�������ς�]");
	}
	else if (2 == nWk[0] && 2 != mcls_pStatus->GetStatusRead().SeqKadou.stat) {
		mcls_pStatus->SetStatusQue(mcls_pStatus->GetStatusRead().SeqKadou.id, 2, false);
		LOG(em_MSG), "[%s] �V�[�P���T�ғ����: �\��", my_sThreadName);
		syslog(604, "[�\��]");
	}
	else if (3 == nWk[0] && 3 != mcls_pStatus->GetStatusRead().SeqKadou.stat) {
		mcls_pStatus->SetStatusQue(mcls_pStatus->GetStatusRead().SeqKadou.id, 3, false);
		LOG(em_MSG), "[%s] �V�[�P���T�ғ����: �B����", my_sThreadName);
		syslog(604, "[�B����]");
	}

	// ��Ԉʒu���
	for (int ii = 0; ii < NUM_MEN; ii++) {
		nWk[0] = tbl.daisya[ii].nOnline;
		nWk[1] = tbl.daisya[ii].nOffline;
		if (1 == nWk[0] && 0 == nWk[1] && 0 != mcls_pStatus->GetStatusRead().DaisyaSt[ii].stat) {
			mcls_pStatus->SetStatusQue(mcls_pStatus->GetStatusRead().DaisyaSt[ii].id, 0, false);
			LOG(em_MSG), "[%s] %s�J������ԃI�����C���ʒu", my_sThreadName, DivNameManager::GetTorB(ii));
			syslog(295, "[%s:�I�����C���ʒu]", DivNameManager::GetTorB(ii));
			
		}
		else if (0 == nWk[0] && 1 == nWk[1]  && 1 != mcls_pStatus->GetStatusRead().DaisyaSt[ii].stat) {
			mcls_pStatus->SetStatusQue(mcls_pStatus->GetStatusRead().DaisyaSt[ii].id, 1, false);
			LOG(em_ERR), "[%s] %s�J������ԃI�t���C���ʒu", my_sThreadName, DivNameManager::GetTorB(ii));
			syslog(295, "[%s:�I�t���C���ʒu]", DivNameManager::GetTorB(ii));
		}
		else if ( ( (0 == nWk[0] && 0 == nWk[1]) || (1 == nWk[0] && 1 == nWk[1]) )&&
			-1 != mcls_pStatus->GetStatusRead().DaisyaSt[ii].stat) {
			mcls_pStatus->SetStatusQue(mcls_pStatus->GetStatusRead().DaisyaSt[ii].id, -1, false);
			LOG(em_ERR), "[%s] %s�J������ԕs���ʒu", my_sThreadName, DivNameManager::GetTorB(ii));
			syslog(295, "[%s:�s���ʒu]", DivNameManager::GetTorB(ii));
		}
	}

	// 20210312 --->>>
	// ��Ԉʒu�I�����C���ȊO
	for (int ii = 0; ii < NUM_MEN; ii++) {
		nWk[0] = tbl.daisya[ii].nOnline;
		nWk[1] = tbl.daisya[ii].nOffline;
		if (1 == nWk[0] && 0 == nWk[1] && 0 != mcls_pStatus->GetStatusRead().DaisyaEx_St[ii].stat) {
			mcls_pStatus->SetStatusQue(mcls_pStatus->GetStatusRead().DaisyaEx_St[ii].id, 0, false);
			LOG(em_MSG), "[%s] %s�Ӄ֑�Ԉʒu�I�����C���ȊO�i0:�r�ۑ����j Frame:%d", my_sThreadName, DivNameManager::GetTorB(ii), tbl.ken.nFrameNo);
			syslog(298, "[%s:�r�ۑ��� Frame:%d]", DivNameManager::GetTorB(ii), tbl.ken.nFrameNo);
		}
		else if (0 == nWk[0] && 1 != mcls_pStatus->GetStatusRead().DaisyaEx_St[ii].stat) {
			mcls_pStatus->SetStatusQue(mcls_pStatus->GetStatusRead().DaisyaEx_St[ii].id, 1, false);
			LOG(em_MSG), "[%s] %s�Ӄ֑�Ԉʒu�I�����C���ȊO�i1:�r�ۑ��X�L�b�v�J�n Frame:%d�j", my_sThreadName, DivNameManager::GetTorB(ii), tbl.ken.nFrameNo);
			syslog(298, "[%s:�r�ۑ��X�L�b�v�J�n Frame:%d]", DivNameManager::GetTorB(ii), tbl.ken.nFrameNo);
		}
		else if ((1 == nWk[0] && 1 == nWk[1]) && 1 != mcls_pStatus->GetStatusRead().DaisyaEx_St[ii].stat) {
			mcls_pStatus->SetStatusQue(mcls_pStatus->GetStatusRead().DaisyaEx_St[ii].id, 1, false);
			LOG(em_MSG), "[%s] %s�Ӄ֑�Ԉʒu�I�����C���ȊO�i1:�r�ۑ��X�L�b�v�J�n Frame:%d�j", my_sThreadName, DivNameManager::GetTorB(ii), tbl.ken.nFrameNo);
			syslog(298, "[%s:�r�ۑ��X�L�b�v�J�n Frame:%d]", DivNameManager::GetTorB(ii), tbl.ken.nFrameNo);
		}
	}
	// <<<---

	// �`���[�^�]�M��
	CString sChiler[3] = { "�\��","����","BK" };
	for(int ii=0; ii<3; ii++) {
		nWk[0] = tbl.nChilerSt[ii];
		if (1 == nWk[0] && 0 != mcls_pStatus->GetStatusRead().ChilerPiStop[ii].stat) {
			mcls_pStatus->SetStatusQue(mcls_pStatus->GetStatusRead().ChilerPiStop[ii].id, 0, false);
			LOG(em_MSG), "[%s] %s PI�`���[: �^�]��", my_sThreadName, sChiler[ii]);
			syslog(610, "[%s]", sChiler[ii]);
		}
		else if (0 == nWk[0] && 1 != mcls_pStatus->GetStatusRead().ChilerPiStop[ii].stat) {
			mcls_pStatus->SetStatusQue(mcls_pStatus->GetStatusRead().ChilerPiStop[ii].id, 1, false);
			LOG(em_ERR), "[%s] %s PI�`���[: ��~��", my_sThreadName, sChiler[ii]);
			syslog(611, "[%s]", sChiler[ii]);
		}
	}

	// �`���[�ُ�M��
	for(int ii=0; ii<3; ii++) {
		nWk[0] = tbl.nChilerErr[ii];
		if (1 == nWk[0] && 0 != mcls_pStatus->GetStatusRead().ChilerPiErr[ii].stat) {
			mcls_pStatus->SetStatusQue(mcls_pStatus->GetStatusRead().ChilerPiErr[ii].id, 0, true);
			LOG(em_MSG), "[%s] %s PI�`���[: ����", my_sThreadName, sChiler[ii]);
		}
		else if (0 == nWk[0] && 1 != mcls_pStatus->GetStatusRead().ChilerPiErr[ii].stat) {
			mcls_pStatus->SetStatusQue(mcls_pStatus->GetStatusRead().ChilerPiErr[ii].id, 1, true);
			LOG(em_ERR), "[%s] %s PI�`���[: �ُ�", my_sThreadName, sChiler[ii]);
		}
	}

	// �G�A�h���C���[��~�M��
	for (int ii = 0; ii < NUM_MEN; ii++) {
		nWk[0] = tbl.AirBlow[ii].nAirBlowSt;
		if (1 == nWk[0] && 0 != mcls_pStatus->GetStatusRead().airblow[ii].AirBlowPiStop.stat) {
			mcls_pStatus->SetStatusQue(mcls_pStatus->GetStatusRead().airblow[ii].AirBlowPiStop.id, 0, true);
			LOG(em_MSG), "[%s] %s �G�A�h���C���[: �^�]��", my_sThreadName, DivNameManager::GetTorB(ii));
			syslog(613, "%s", DivNameManager::GetTorB(ii));
		}
		else if (0 == nWk[0] && 1 != mcls_pStatus->GetStatusRead().airblow[ii].AirBlowPiStop.stat) {
			mcls_pStatus->SetStatusQue(mcls_pStatus->GetStatusRead().airblow[ii].AirBlowPiStop.id, 1, true);
			LOG(em_WAR), "[%s] %s �G�A�h���C���[: ��~��", my_sThreadName, DivNameManager::GetTorB(ii));
			syslog(612, "%s", DivNameManager::GetTorB(ii));
		}

		// �G�A�h���C���[�ُ�M��
		nWk[0] = tbl.AirBlow[ii].nAirBlowErr;
		if (1 == nWk[0] && 0 != mcls_pStatus->GetStatusRead().airblow[ii].AirBlowPiErr.stat) {
			mcls_pStatus->SetStatusQue(mcls_pStatus->GetStatusRead().airblow[ii].AirBlowPiErr.id, 0, true);
			LOG(em_MSG), "[%s] %s �G�A�h���C���[: ����", my_sThreadName, DivNameManager::GetTorB(ii));
		}
		else if (0 == nWk[0] && 1 != mcls_pStatus->GetStatusRead().airblow[ii].AirBlowPiErr.stat) {
			mcls_pStatus->SetStatusQue(mcls_pStatus->GetStatusRead().airblow[ii].AirBlowPiErr.id, 1, true);
			LOG(em_ERR), "[%s] %s �G�A�h���C���[: �ُ�", my_sThreadName, DivNameManager::GetTorB(ii));
		}
	}

	// ��Ԑ����INV�d��OFF
	nWk[0] = tbl.nDaisyaInvSt;
	if (1 == nWk[0] && 0 == m_typOldDmRecvLow.nDaisyaInvSt) {
		LOG(em_ERR), "[%s] ��Ԑ���� INV�d��: �d��OFF", my_sThreadName);
	}
	else if (0 == nWk[0] && 1 == m_typOldDmRecvLow.nDaisyaInvSt) {
		LOG(em_MSG), "[%s] ��Ԑ���� INV�d��: �d��ON", my_sThreadName);
	}


	// �d�C�� �� �J���
	nWk[0] = tbl.Sec.nDoor;
	// �J��ԁiPI:0�ASTATUS:1�j���l���t�Ȃ̂Œ��ӁI
	if (0 == nWk[0] && 1 != mcls_pStatus->GetStatusRead().SecurityDoor.stat) {
		mcls_pStatus->SetStatusQue(mcls_pStatus->GetStatusRead().SecurityDoor.id, 1, false);
		LOG(em_MSG), "[%s] �d�C���@���@�J���", my_sThreadName);
		syslog(616, "[��; ��ԁ��J���]");
	}
	// ��ԁiPI:1�ASTATUS:0�j
	else if (1 == nWk[0] && 0 != mcls_pStatus->GetStatusRead().SecurityDoor.stat) {
		mcls_pStatus->SetStatusQue(mcls_pStatus->GetStatusRead().SecurityDoor.id, 0, false);
		LOG(em_MSG), "[%s] �d�C���@���@���", my_sThreadName);
		syslog(616, "[��; �J��ԁ����]");
	}

	// �d�C�� �� �J���
	nWk[0] = tbl.Sec.nKey;
	// �J��ԁiPI:0�ASTAT1US:1�j���l���t�Ȃ̂Œ��ӁI
	if (0 == nWk[0] && 1 != mcls_pStatus->GetStatusRead().SecurityKey.stat) {
		mcls_pStatus->SetStatusQue(mcls_pStatus->GetStatusRead().SecurityKey.id, 1, false);
		LOG(em_MSG), "[%s] �d�C���@���@�J���", my_sThreadName);
		syslog(617, "[��; ��ԁ��J���]");
	}
	// ��ԁiPI:1�ASTATUS:0�j
	else if (1 == nWk[0] && 0 != mcls_pStatus->GetStatusRead().SecurityKey.stat) {
		mcls_pStatus->SetStatusQue(mcls_pStatus->GetStatusRead().SecurityKey.id, 0, false);
		LOG(em_MSG), "[%s] �d�C���@���@���", my_sThreadName);
		syslog(617, "[��; �J��ԁ����]");
	}

	// �p���X���[�g�ύX�m�F
	int nNowPls[3];	// 0.0001mm/pls
					// [0:�����pPLG, 1:������r�pPLG, 2:�\���pPLG]

	nNowPls[0] = tbl.nPls;
	nNowPls[1] = tbl.nPlsSub;
	nNowPls[2] = tbl.nPlsDsp;

	if( (m_nPlsRate[0] != nNowPls[0] && 0 != nNowPls[0]) ||
		(m_nPlsRate[1] != nNowPls[1] && 0 != nNowPls[1]) ||
		(m_nPlsRate[2] != nNowPls[2] && 0 != nNowPls[2]) ){

		if( (0 >= nNowPls[0] || 0 >= nNowPls[1] || 0 >= nNowPls[2] ) && m_bInitReadOk ){
			LOG(em_WAR), "[%s] �V�[�P���T�����͂��ꂽ�p���X���[�g���ُ�[�����p:%.4fmm/pls][������r�p:%.4fmm/pls][�\���p:%.4fmm/pls][�����l:%d,%d,%d]", my_sThreadName, nNowPls[0]/ PLS_RATE_TANI, nNowPls[1]/ PLS_RATE_TANI, nNowPls[2]/ PLS_RATE_TANI, nNowPls[0], nNowPls[1], nNowPls[2]);
			syslog(605, "�����p:%.4fmm/pls, ������r�p:%.4fmm/pls, �\���p:%.4fmm/pls", nNowPls[0] / PLS_RATE_TANI, nNowPls[1] / PLS_RATE_TANI, nNowPls[2] / PLS_RATE_TANI);

		}else{
			// ���O�o��
			for(int ii=0;ii<3; ii++) {
				if(m_nPlsRate[ii] != nNowPls[ii]) {
					CString sWk[3] = { "�����p", "������r�p", "�\���p" };
					LOG(em_MSG),"[%s] �V�[�P���T���%s�p���X���[�g�ύX[%d �� %d]", sWk[ii], my_sThreadName, m_nPlsRate[ii], nNowPls[ii]);
					syslog(606, "[%s�F%.4fmm/pls��%.4fmm/pls]", sWk[ii], m_nPlsRate[ii] / PLS_RATE_TANI, nNowPls[ii] / PLS_RATE_TANI);
				}
			}

			// �p���X���[�g�A���蕪��\��DB�X�V
			UpdatePlsRate(nNowPls);
		}
		memcpy(m_nPlsRate, nNowPls, sizeof(m_nPlsRate) );
	}

	//// SPM�J���M��
	bool bLog = false;		// true���A��ԕύX�̃��O���o��
	for (int ii = 0; ii < 2; ii++) {
		nWk[0] = tbl.nSpm;
		nWk[1] = mcls_pStatus->GetSpmStat();
		if (0 == nWk[0] && 1 != nWk[1]) {
			// ������ԁiPI:0�ASTAT1US:1�j���l���t�Ȃ̂Œ��ӁI
			mcls_pStatus->SetSpmStat(DIV_SPM_TRUE);
			LOG(em_MSG), "[%s] SPM�J���M�� ������", my_sThreadName);
			syslog(294, "[SPM��� �J��������]");
		}
		else if(1 == nWk[0] && 0 != nWk[1]) {
			// �J����ԁiPI:1�ASTATUS:0�j
			mcls_pStatus->SetSpmStat(DIV_SPM_FALSE);
			LOG(em_MSG), "[%s] SPM�J���M�� �J����", my_sThreadName);
			syslog(294, "[SPM��� �������J��]");
		}
	}

	//// �h���X�v���[�����t�����
	nWk[0] = tbl.nToyuSply;
	// �����t��ԁiPI:0�ASTAT1US:0�j
	if (0 == nWk[0] && 0 != mcls_pStatus->GetStatusRead().ToyuSprSt.stat) {
		mcls_pStatus->SetStatusQue(mcls_pStatus->GetStatusRead().ToyuSprSt.id, 0, false);
		LOG(em_MSG), "[%s] �h���X�v���[���u ���t��� �����t���", my_sThreadName);
		syslog(618, "[���t���; ���t����ԁ������t���]");
	}
	// ���t����ԁiPI:1�ASTATUS:1�j
	else if (1 == nWk[0] && 1 != mcls_pStatus->GetStatusRead().ToyuSprSt.stat) {
		mcls_pStatus->SetStatusQue(mcls_pStatus->GetStatusRead().ToyuSprSt.id, 1, false);
		LOG(em_MSG), "[%s] �h���X�v���[���u ���t��� ���t�����", my_sThreadName);
		syslog(618, "[���t���; �����t��ԁ����t�����]");
	}
}

//==========================================
// �r�ڋ߃����v��ԏ�������
//------------------------------------------
// OCCUR_RAMP typRamp: �����v���
//==========================================
void DmemManager::WritDmemRamp(OCCUR_RAMP typRamp)
{
	//memcpy(mtbl_pSend->nSekkin, typRamp.ranp, sizeof(mtbl_pSend->nSekkin));
	for (int ii = 0; ii<NUM_MEN; ii++) {
		for (int jj = 0; jj < OUTPUT_MESYU_SEKKIN_COL; jj++) {
			for (int kk = 0; kk < OUTPUT_RAMP_NUM; kk++) {
				mtbl_pSend->nSekkin[ii][jj][kk] = typRamp.ranp[ii][jj][kk];
			}
		}
	}

	for(int ii=0; ii<NUM_MEN; ii++) {
		for (int jj=0; jj < OUTPUT_MESYU_SEKKIN_COL; jj++) {
			for (int kk=OUTPUT_RAMP_NUM-1; kk>=0 ; kk--) {
				if(em_RAMP_STAT_ON == typRamp.ranp[ii][jj][kk]) {
					LOGA(em_MSG), "[%s] �r�ڋ߃����v ����[men=%s, �ʒu=%d, %s]", my_sThreadName, DivNameManager::GetTorB(ii), jj, 0==kk?"�y":"�d");
					m_nRamp[ii][jj] = kk;
					break;
				}
				m_nRamp[ii][jj] = -1;
			}
		}
	}
}

//==========================================
// �r�ڋ߃A���[����ԏ�������
//------------------------------------------
// OCCUR_ALARM typAlarm: �A���[�����
//==========================================
void DmemManager::WritDmemAlarm(OCCUR_ALARM typAlarm)
{
	for (int ii = 0; ii < OUTPUT_ALARM_COL; ii++) {
		for (int jj = 0; jj < OUTPUT_RAMP_NUM; jj++) {
			mtbl_pSend->nSekalm[ii][jj] = typAlarm.alarm[ii][jj];
		}
	}

	for (int ii = 0; ii < OUTPUT_MESYU_SEKKIN_COL; ii++) {
		for (int jj = OUTPUT_RAMP_NUM - 1; jj >= 0; jj--) {
			if (em_RAMP_STAT_ON == typAlarm.alarmPos[ii][jj]) {
				LOGA(em_MSG), "[%s] �r�ڋ߃A���[�� ����[�ʒu=%d, %s]", my_sThreadName, ii, 0 == jj ? "�y" : "�d");
				m_nAlarm[ii] = jj;
				break;
			}
			m_nAlarm[ii] = -1;
		}
	}
}

//==========================================
// �p�g���C�g��ԏ�������
//------------------------------------------
// OCCUR_PATO typPato: �p�g���C�g���
//==========================================
void DmemManager::WritDmemPato(OCCUR_PATO typPato)
{
	memcpy(mtbl_pSend->nPato, typPato.pato, sizeof(mtbl_pSend->nPato));
	int nPatoCnt = 0;
	for (int ii = 0; ii<OUTPUT_PATO_POS_NUM; ii++) {
		nPatoCnt += typPato.pato[ii][0];
	}
	for (int ii = 0; ii<OUTPUT_PATO_POS_NUM; ii++) {
		for (int jj = 0; jj < OUTPUT_PATO_KIND_NUM; jj++) {
			mtbl_pSend->nPato[ii][jj] = nPatoCnt;
		}
	}
	memcpy( m_nPato, typPato.pato, sizeof(m_nPato));
	//LOG(em_INF), "[%s] �p�g���C�g��M[�ۗL��=%d/%d/%d/%d]", my_sThreadName, typPato.pato[0][0], typPato.pato[0][1], typPato.pato[1][0], typPato.pato[1][1]);
	LOGA(em_MSG), "[%s] �p�g���C�g���ύX[�ۗL��=%d/%d/%d/%d ����=%d]", my_sThreadName, m_nPato[0][0], m_nPato[0][1], m_nPato[1][0], m_nPato[1][1], nPatoCnt);
}

//==========================================
// ���׌��o�{�C�X��ԏ�������
//------------------------------------------
// OCCUR_VOICE_ERR	typVoice: �{�C�X���
//==========================================
void DmemManager::WritDmemVoice(OCCUR_VOICE typVoice)
{
	int nVoiceMen = -1;
	int nVoice = -1;

	// �ݔ��ُ�H or ���׌��o�H
	bool bWk = false;
	for (int ii = 0; ii <OUTPUT_VOICE_NUM; ii++) {
		if (em_VOICE_DEF_TOP == ii) continue;
		if (em_VOICE_DEF_BOT == ii) continue;
		if (em_VOICE_STAT_ON == typVoice.voice[ii]) {
			if (OUTPUT_VOICE_ERR_NUM > ii) {
				nVoiceMen = -1;
				nVoice = ii;
				bWk = true;
			}
			else {
				nVoiceMen = (em_VOICE_STAT_ON == typVoice.voice[em_VOICE_DEF_TOP])? 0 : 1;
				nVoice = ii;
			}
			break;
		}
	}
	// �Y���Ȃ� 
	if(-1 == nVoice) {
		WritDmemVoiceReset();
		return;
	}

	// �����o�[�ϐ��ɃZ�b�g
	m_nVoiceMen = nVoiceMen;
	m_nVoice = nVoice;
	memset(&mtbl_pSend->typKensaVoice, 0x00, sizeof(mtbl_pSend->typKensaVoice));
	// �ݔ��ُ�
	if(bWk) {
		mtbl_pSend->typKensaVoice.nVoice_Err[m_nVoice] = 1;
		CancelWaitableTimer(m_evTimeAlarmePulse);
		LOGA(em_MSG), "[%s] �ݔ��ُ�{�C�X �o��[id=%d]", my_sThreadName, m_nVoice);
	}
	// ���׌��o
	else {
		mtbl_pSend->typKensaVoice.nVoice_Def[nVoiceMen] = 1;
		mtbl_pSend->typKensaVoice.nVoice_Def[m_nVoice - OUTPUT_VOICE_ERR_NUM] = 1;
		// �p���X�������^�C�}�[�Z�b�g
		__int64 ts = PARAM_TIME_PULSE_DEF * -10000;	// 1�~���b��100�i�m�b (�}�C�i�X�w��ŁA�w�莞�Ԍo�߂ŃV�O�i��) (��3������2��ڈȍ~�̋N���Ԋu)
		SetWaitableTimer(m_evTimeAlarmePulse, (LARGE_INTEGER *)&ts, NULL, NULL, NULL, FALSE);
		LOGA(em_MSG), "[%s] ���׌��o�{�C�X �o��[men=%d, id=%d]", my_sThreadName, nVoiceMen, m_nVoice);
	}
}

//==========================================
// ���׌��o�{�C�X��ԏ������݃��Z�b�g
//==========================================
void DmemManager::WritDmemVoiceReset()
{
	// �p���XON�o�̓`�F�b�N
	int ii;
	for (ii = 0; ii <OUTPUT_VOICE_NUM; ii++)
	{
		if (em_VOICE_DEF_TOP == ii) continue;
		if (em_VOICE_DEF_BOT == ii) continue;
		if (em_VOICE_STAT_ON == mtbl_pSend->typKensaVoice.nVoice_Def[ii]) break;
	}

	if (OUTPUT_VOICE_NUM != ii)
	{
		// OFF�o�͊J�n
		memset(&mtbl_pSend->typKensaVoice, 0x00, sizeof(mtbl_pSend->typKensaVoice));

		__int64 ts = PARAM_TIME_PULSE * -10000;	// 1�~���b��100�i�m�b (�}�C�i�X�w��ŁA�w�莞�Ԍo�߂ŃV�O�i��) (��3������2��ڈȍ~�̋N���Ԋu)
		SetWaitableTimer(m_evTimeAlarmePulse, (LARGE_INTEGER *)&ts, NULL, NULL, NULL, FALSE);
	}
	else
	{
		// �p���X�o�͊���
		m_nVoiceMen = -1;
		m_nVoice = -1;
	}
}
//------------------------------------------
// 1�b�p���X�o�͎w��
// EM_EVENT_PLS_OUT emPls  �w�����e
// int nVal �������ݒl �ʏ�1
//------------------------------------------
void DmemManager::SetData_Pls(EM_EVENT_PLS_OUT emPls, int nVal)
{
	AutoLock autolock(&m_csLock);

	// PLS�o�͈˗�
	m_nOutPls[emPls] = nVal;
	LOG(em_MSG), "[%s] %s�p���X�o�͈˗�", my_sThreadName, pls_out_name[emPls]);
}



//==========================================
// �J�E���^���Z�b�g�v���M�� [��MI]
//==========================================
void DmemManager::PoPuls_CountReset()
{
	AutoLock autolock(&m_csLock);
	LOG(em_MSG), "[%s] �V�[�P���T�J�E���^���Z�b�g�v��", my_sThreadName);
	mtbl_pSend->nCntReset = 1;

	//// PLG�J�E���^���Z�b�g�v�����t���O��ON
	//m_bPlgReset = true;
	
	// �p���X�������^�C�}�[�Z�b�g
	__int64 ts = PARAM_TIME_PULSE * -10000;	// 1�~���b��100�i�m�b (�}�C�i�X�w��ŁA�w�莞�Ԍo�߂ŃV�O�i��) (��3������2��ڈȍ~�̋N���Ԋu)
	SetWaitableTimer(m_evTimeCntResetPulse, (LARGE_INTEGER *)&ts, NULL, NULL, NULL, FALSE);
}

//==========================================
// �J�E���^�J�n�v���M�� [��MI]
//==========================================
void DmemManager::PoPuls_CountStart()
{
	AutoLock autolock(&m_csLock);
	LOG(em_MSG), "[%s] �V�[�P���T�J�E���^�J�n�v���M��", my_sThreadName);
	mtbl_pSend->nCntStart = 1;

	//// PLG�J�E���^���Z�b�g�v�����t���O��ON
	//m_bPlgReset = false;

	// �p���X�������^�C�}�[�Z�b�g
	__int64 ts = PARAM_TIME_PULSE * -10000;	// 1�~���b��100�i�m�b (�}�C�i�X�w��ŁA�w�莞�Ԍo�߂ŃV�O�i��) (��3������2��ڈȍ~�̋N���Ԋu)
	SetWaitableTimer(m_evTimeCntStartPulse, (LARGE_INTEGER *)&ts, NULL, NULL, NULL, FALSE);
}

//==========================================
// �e�X�g�p WPD�M�� ON�v���M�� [��MI]
//------------------------------------------
// int	nPos: �ʒu(1:������_WPD�A2:�\����_WPD)
//==========================================
void DmemManager::PoPuls_TesetWpdOn(int nPos)
{
	// 1:������_WPD�A2:�\����_WPD�A3:������_WPD(DM�̂�)�A4:�\����_WPD(DM�̂�)�ȊO��return
	if(0 >= nPos && 5 <= nPos) return;

	AutoLock autolock(&m_csLock);
	mtbl_pSend->nTestWpdReq = nPos;

	// �p���X�������^�C�}�[�Z�b�g
	__int64 ts = PARAM_TIME_PULSE * -10000;	// 1�~���b��100�i�m�b (�}�C�i�X�w��ŁA�w�莞�Ԍo�߂ŃV�O�i��) (��3������2��ڈȍ~�̋N���Ԋu)
	SetWaitableTimer(m_evTimeTestWpdPulse, (LARGE_INTEGER *)&ts, NULL, NULL, NULL, FALSE);
}

//==========================================
// �p���X���[�g�A���蕪��\��DB�X�V
//------------------------------------------
// int			nPls	: �p���X���[�g
//==========================================
void DmemManager::UpdatePlsRate(int* nPls)
{
	//// ���O����
	DBManager::DeliveryDB *deliDB = new DBManager::DeliveryDB;
	deliDB->kubun = DBManager::em_PULSE_RATE_UPDATE;
	deliDB->data = (BYTE*)new DBDATA_PULSE_RATE_UPDATE;
	DBDATA_PULSE_RATE_UPDATE* pWk = (DBDATA_PULSE_RATE_UPDATE*)deliDB->data;

	//// �f�[�^�Z�b�g
	for (int ii = 0; ii < 3; ii++) {
		pWk->dPls[ii] = ((double)nPls[ii]) / PLS_RATE_TANI;
		pWk->dVres[ii] = pWk->dPls[ii] * mcls_pParam->GetParamCommon().typCamCon[DIV_PLG_LINE - 1].nDiv / mcls_pParam->GetParamCommon().typCamCon[DIV_PLG_LINE - 1].nMul;
	}
	//// �o�^�˗�
	if (!mque_pAddSQLData->AddToTailFreeCheck(deliDB, 20)) {		// �l�܂�Ȃ��悤�ɋ󂫃`�F�b�N
																	// �o�^���s
		LOG(em_ERR), "[%s] DB�o�^�˗��L���[�t�� [DBDATA_PULSE_RATE_UPDATE]", my_sThreadName);
		syslog(SYSNO_QUEFULL_ERR, "[DBDATA_PULSE_RATE_UPDATE]");
		delete deliDB->data;
		delete deliDB;
		// DB���ُ�(�ʏ킠�肦�Ȃ�)
		mcls_pStatus->AddQueMyErrWar(true, ON_DB_ACCESS_ERR);
	}
}