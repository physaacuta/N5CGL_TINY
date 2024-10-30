#include "stdafx.h"
#include "PlgManager.h"

// �ÓI�N���X
#include "OpSendFunc.h"													// �\���Ǘ����M�N���X

// �������[���[�N���o ----->>>
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif
// <<<------------------------

LogFileManager*			mcls_pLogPlg = NULL;
#define LOGPLG(t)  if(mcls_pLogPlg!=NULL)       mcls_pLogPlg->Write(KizuLib::LogFileManager::t			// ���O�o��
LogFileManager*			mcls_pLogPlgDsp = NULL;
#define LOGPLGDSP(t)  if(mcls_pLogPlgDsp!=NULL)       mcls_pLogPlgDsp->Write(KizuLib::LogFileManager::t			// ���O�o��

//------------------------------------------
// �R���X�g���N�^
//------------------------------------------
PlgManager::PlgManager() :
ThreadManager("PlgMgr")
{
	// �V�O�i��
	m_evSamplingStart	= CreateEvent(NULL, FALSE, FALSE, NULL);
	m_evSamplingStop	= CreateEvent(NULL, FALSE, FALSE, NULL);
	m_evTimer			= CreateWaitableTimer(NULL, FALSE, NULL);
	m_evKenNotPos		= CreateEvent(NULL, FALSE, FALSE, NULL);
	m_evTimerDspLen		= CreateWaitableTimer(NULL, FALSE, NULL);

	// UDP�\�P�b�g������
	memset(&m_addr, 0x00, sizeof(m_addr));
	for( int ii=0; ii<MAX_NUM_UDP; ii++) {
		m_sock[ii] = INVALID_SOCKET;
	}

	// ���̑�
	m_emIsLinePLG			= DIV_PLG_LINE;
	m_emUnten				= DIV_UNTEN_NOMAL;
#ifdef DISABLE_DSP_PLG
	m_bIsDisableDspPLG		= true;
#else
	m_bIsDisableDspPLG		= false;
#endif
	m_nDistPos				= 0;
	m_nDistLen = 0;
	m_bRecvInit				= false;
	m_nMin					= 0;
	m_nMax					= 0;
	m_nPreExSync			= 0;

	setlocale(LC_ALL, "Japanese");	// ���ꂪ�����Ɠ��{��p�X���܂ރt�H���_�̏ꍇ�A�ۑ��ł��Ȃ��B(���C�u�����̒��Ŏ��s����ׂ��������Ă��邪�A�ύX����ƃ^�X�N�S�̂ɉe������̂ŁA�Ƃ肠���������Őݒ�)

	mcls_pLogPlg = new LogFileManager("PLG���O_����", NULL, 20, 3);	// �������R�����g�����邾���Ń��O�͏o�Ȃ��B
	LOGPLG(em_INF), "�X�^�[�g");
	mcls_pLogPlgDsp = new LogFileManager("PLG���O_�\��", NULL, 20, 3);	// �������R�����g�����邾���Ń��O�͏o�Ȃ��B
	LOGPLGDSP(em_INF), "�X�^�[�g");
}

//------------------------------------------
// �f�X�g���N�^
//------------------------------------------
PlgManager::~PlgManager()
{
	// �V�O�i��
	CloseHandle(m_evSamplingStart);
	CloseHandle(m_evSamplingStop);
	CloseHandle(m_evTimer);
	CloseHandle(m_evKenNotPos);
	CloseHandle(m_evTimerDspLen);


	// UDP�\�P�b�g�j��
	for( int ii=0; ii<MAX_NUM_UDP; ii++) {
		if( INVALID_SOCKET != m_sock[ii] ) {
			closesocket(m_sock[ii]);
			m_sock[ii] = INVALID_SOCKET;
		}
	}

	if (NULL != mcls_pLogPlg) delete mcls_pLogPlg;
	if (NULL != mcls_pLogPlgDsp) delete mcls_pLogPlgDsp;
}

//------------------------------------------
// �֐�������
//------------------------------------------
void PlgManager::Alloc()
{
	//======================================================
	// �ʏ�̏������֘A


	// ������
	m_bMaxLen_Ken = false;								// �R�C����������B
	m_bMaxLen_Dsp = false;
	m_emIsLinePLG = DIV_PLG_LINE;
	m_emUnten = DIV_UNTEN_NOMAL;

	ClearMember();
	
	// �p���X���[�g
	m_dPlsR_Ken = 0.0;
	m_dPlsR_Dsp = 0.0;
	m_dPlsR_Sub = 0.0;
}


// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ������

//------------------------------------------
// ������
//------------------------------------------
void PlgManager::ClearMember()
{
	//================================================
	// PLG�֘A
	memset(&m_trFrm, 0x00, sizeof(m_trFrm));
	for (int ii = 0; ii<NUM_MEN; ii++) {
		m_nDly[ii] = 0;
		m_nChgFrameNo[ii] = 0;
	}
	m_nTime = 0;
	m_dDistance = 0.0;

	m_nFrameNo = 0;
	m_nSpeed = 0;
	m_nSpeedDsp = 0;
	m_dResV = mcls_pParam->CalcResV(m_emIsLinePLG);
	m_dResVCycle = 0.0;
	m_nPreExSync = 0;
	m_preLen_KenSub = 0;

#ifndef PLG_DSP_KEN_EQAL
		m_preCountDsp = 0;
#endif

	m_bCoilOutChange = false;

	//// ������
	CoilInit();
}

//------------------------------------------
// ������
//------------------------------------------
void PlgManager::CoilInit()
{
	ParamManager::P_COMMON_MAST	prm	= mcls_pParam->GetCommonMast();

	//================================================
	// �����Ǘ�
	// �O��l�p (������_�p)
	m_dCoilOldLen_Ken				= 0.0;
	m_bCoilChange_Ken				= false;

	// �O��l�p (�\����_�p)
	m_dCoilOldLen_BaseDsp			= 0.0;
	for(int ii=0; ii<MAX_POS_INDEX; ii++) {
		m_dLen_Dsp[ii]				= 0.0;
		m_bCoilChange_Dsp[ii]		= false;
	}

	//================================================
	// �����Ǘ�
	//// �ŉ����̕\���ʒu����

	double dDspLen[NUM_DSP_WPD];
	dDspLen[0] = prm.Dist.nDspSharCut;										// �Ǘ��Ώۂ̈�ԉ������̋����B����̊�Ƃ��� ������̓V���[�J�b�g�ʒu

	// ����
	m_dLen_Ken[m_nDistPos]			= m_nDistLen;							// ��ʂ̕�����ɐi��ł��鎖�Ƃ���B
	m_dLen_Ken[m_nDistNotPos]		= 0.0;
	m_dLen_WpdKenPer				= -1 * m_nDistLen;
	m_dLen_KenSub					= m_nDistLen;

	// �\��	(�����Ǘ��̈�ԉ����ɃR�C����[�����Ă����Ԃ���X�^�[�g)
	m_dLen_BaseDsp				= 0.0;
	m_dLen_Dsp[POS_DSP_DSP1_P]	= - prm.Dist.nDspBaseKensaTop;				// �\�ʌ�����
	m_dLen_Dsp[POS_DSP_DSP2_P]	= - prm.Dist.nDspBaseKensaBot;				// ���ʌ�����
	m_dLen_Dsp[POS_DSP_SHR_I]	= - dDspLen[0];								// �V���[�J�b�g�ʒu
	m_dLen_OutCut				= m_dLen_Dsp[POS_DSP_SHR_I];				// �V���[�ʒu�Ɠ�������	
	
	//// �����ʒu (�e�V�X�e����)
	// ����͖���

	//// �\���ʒu (�e�V�X�e����)
	// ����͖���
	

	//================================================
	// �R�C�����ُ��ԃ`�F�b�N
	mcls_pStatus->SetStatusQue("MAXLEN_KEN_ID", true, false);
	mcls_pStatus->SetStatusQue("MAXLEN_DSP_ID", true, false);
	m_bMaxLen_Ken = false;							
	m_bMaxLen_Dsp = false;
}

// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ���C���X���b�h
//------------------------------------------
// �X���b�h�J�n�O���� (�X���b�h���s����(�V�O�i���Z�b�g�O)�ɃR�[���o�b�N)
//------------------------------------------
int PlgManager::ThreadFirst()
{
	//================================================
	// UDP����
	while( TRUE ) {
		if( SetUdp(0, EPC_UDP_IP, EPC_UDP_PORT ) ) break;
		LOG(em_ERR), "[%s] UDP �������E�E�E [%d]", my_sThreadName, EPC_UDP_PORT);
		Sleep(2000);
	}

	//================================================
	// �\���ʒu���M�^�C�}�[
	__int64 ts = TIME_INTERVAL_DSP_LEN * -10000; ;	// 1�ʕb��100�i�m�b (�}�C�i�X�w��ŁA�w�莞�Ԍo�߂ŃV�O�i��) (��3������2��ڈȍ~�̋N���Ԋu)
	SetWaitableTimer(m_evTimerDspLen, (LARGE_INTEGER *)&ts, TIME_INTERVAL_DSP_LEN, NULL, NULL, FALSE);



	//================================================
	// �ʏ�̃X���b�h����
	//// �C�x���g�ݒ�
	AddEventNum(1, &m_evSamplingStart);
	AddEventNum(1, &m_evSamplingStop);
	AddEventNum(1, &mcls_pSeqFrameR->GetQueFrame()->g_evSem);
	AddEventNum(1, &m_evTimer);
	AddEventNum(1, &m_evTimerDspLen);
	return 0;
}

//------------------------------------------
// �X���b�h�I���O���� (�I���V�O�i����ɃR�[���o�b�N)
//------------------------------------------
int PlgManager::ThreadLast()
{
	//// �^�C�}�[����
	CancelWaitableTimer(m_evTimer);
	CancelWaitableTimer(m_evTimerDspLen);
	return 0;
}


//------------------------------------------
// �X���b�h�C�x���g����
// int nEventNo �C�x���gNo (0�I���W��)
//------------------------------------------
void PlgManager::ThreadEvent(int nEventNo)
{
	////// �V�O�i���̏���
	enum {	EV_SAMP_START = 0,				// ����� �X�^�[�g
			EV_SAMP_STOP,					// ����� �X�g�b�v
			EV_QUE_FRAME,					// �t���[������M
			EV_TIMER,						// ������`�F�b�N�p
			EV_DSP_LEN_CHECK,				// �\���p �ʔ������M
	};
	
	////// �V�O�i����������
	switch (nEventNo) {

//-----------------------------------------------------------------------------------------------
	case EV_SAMP_START: 							// ������X�^�[�g
	{
		//====================================
		// �I������Ă���PLG��Ԃ��`�F�b�N
		m_emIsLinePLG = mcls_pStatus->GetPlg();
		m_emUnten	  = mcls_pStatus->GetUnten();

		LOGPLG(em_WAR), "===========================================================");
		LOGPLG(em_INF), ",UDP FNo,�\[m],��[m],�\�c[m],���c[m],�׸�1,2,3,���x[mpm],���ԍ�[ms],���Ԏ���[ms],�G�b�W�ʒu����_�\[mm],�G�b�W�ʒu����_�\[mm],�G�b�W�ʒu����_��[mm],�G�b�W�ʒu����_��[mm],�G�b�W�ʒu�t���[��No_�\,�G�b�W�ʒu�t���[��No_��,�s�x����\");
		LOGPLG(em_INF), "%s�F��, %s�F��", DivNameManager::GetDivPlg(m_emIsLinePLG), DivNameManager::GetDivUnten(m_emUnten));

		LOGPLGDSP(em_WAR), "===========================================================");
		LOGPLGDSP(em_INF), ",�\����_[m],�\������[m],��������[m],����ʒu[m],�ؑ��׸�1,2,3,���x[mpm],������[ms],���Ԏ���[ms]");
		LOGPLGDSP(em_INF), "%s�F��, %s�F��", DivNameManager::GetDivPlg(m_emIsLinePLG), DivNameManager::GetDivUnten(m_emUnten));

		LOG(em_INF), "[%s] %s�F��, %s�F��, �\��PLG�F%s", my_sThreadName, DivNameManager::GetDivPlg(m_emIsLinePLG), DivNameManager::GetDivUnten(m_emUnten), (m_bIsDisableDspPLG ? "����" : "�L��"));

		// �����e�X�g��
		if( DIV_UNTEN_SMYU == m_emUnten)	m_bDebugMode = true;
		else														m_bDebugMode = false;

		//================================================	
		// ������
		ClearMember();

		//================================================
		// �J�n
		// �����J�E���g�l�擾
		m_preCountDsp = mcls_pDmem->GetDmRecv().dsp.nPlgAdd;
		LOG(em_INF), "[%s] �J�E���g�l�����l�擾�@[%d]", my_sThreadName, m_preCountDsp);

		//================================================
		// �p���X���[�g
		if (true) {
			m_dPlsR_Ken = mcls_pParam->GetParamCommon().dPls;
			m_dPlsR_Dsp = mcls_pParam->GetParamCommon().dPlsDsp;
			m_dPlsR_Sub = mcls_pParam->GetParamCommon().dPlsSub;
		}
		
		//================================================
		// ���Ə�ԊĎ� �J�n
		if (true) {
			DWORD nLvalAdd = mcls_pDmem->GetDmRecv().ken.nLvalAdd;
			//if (mcls_pParam->IsLedCycle()) nLvalAdd /= 2;			// ���ݓ_���Ȃ甼��
			mcls_pSogyo->CheckStart(nLvalAdd);						// ����̓T���v�����O�J�E���g�̑���ɁA���C��PLG�J�E���g���g�p����
		}

		//================================================
		// �^�C�}
		SetEvWaitableTimer(m_evTimer, TIME_INTERVAL_DSP_PLG, TIME_INTERVAL_DSP_PLG);
		SetEvWaitableTimer(m_evTimerDspLen, TIME_INTERVAL_DSP_LEN, TIME_INTERVAL_DSP_LEN);

		mcls_Time.InitAve();
		mcls_Time.CounterStart();

		mcls_TimeDspSpeed.InitAve();
		mcls_TimeDspSpeed.CounterStart();
	}
		break;

//-----------------------------------------------------------------------------------------------
	case EV_SAMP_STOP:								// �������~	
		CancelWaitableTimer(m_evTimer);
		CancelWaitableTimer(m_evTimerDspLen);

		//// ���Ə�ԊĎ� ��~
		mcls_pSogyo->CheckStop();
		mcls_pStatus->SetLength( 0 );

		break;

//-----------------------------------------------------------------------------------------------
	case EV_QUE_FRAME:								// �t���[������M
	{
		mcls_pLock->LockTh();	//// <<<<<--------------

		mcls_Time.CounterCycle();

		SEQ_TYPE_DATA_FRAME* p = mcls_pSeqFrameR->GetQueFrame()->GetFromHead();

		if (NULL != p)
		{
			// 1�t���[���ʔ���
			NextLength_Ken(p);

			SAFE_DELETE(p);
		}

		mcls_pLock->UnlockTh();	//// <<<<<--------------
		break;
	}

//-----------------------------------------------------------------------------------------------
	case EV_TIMER:
		mcls_pLock->LockTh();	//// <<<<<--------------

		//================================================
		// ������r�pPLG �����������i�[
		{
			DWORD nowLength = mcls_pDmem->GetDmRecv().nLengthSub;
			if (m_preLen_KenSub != nowLength)
			{
				DWORD diff = nowLength - m_preLen_KenSub;
				m_preLen_KenSub = nowLength;
				m_dLen_KenSub += (double)diff;
			}
		}

		//================================================
		// �\���ʒu �ł� �ʔ����Ǘ�
		NextLength_Dsp();

		ExecCycle();

		//================================================
		// �ŏ���Ԃł̋@���Ԕc��
		CheckIntervalStatus();

		mcls_pLock->UnlockTh();	//// <<<<<--------------
		break;

//-----------------------------------------------------------------------------------------------
	case EV_DSP_LEN_CHECK:										// �\���p �ʔ������M
		mcls_pLock->LockTh();	//// <<<<<--------------
		OpSendFunc::SendOp_Length( );
		mcls_pLock->UnlockTh();	//// <<<<<--------------
		break;



//-----------------------------------------------------------------------------------------------
	default :
		// ���肦�Ȃ��I�I
		LOG(em_ERR), "[%s] ThreadEvent EventNo NG=%d", my_sThreadName, nEventNo );
		return;
	}
}

//////////////////////////////////////////////////////////////////////
// UDP

//------------------------------------------
// UDP SOCKET����
// int no ����No(0�I���W��)
// const char* ip IP�A�h���X (��F192.9.200.255)
// WORD port �|�[�gNo (��F8003)
// �߂�l ���A���
//------------------------------------------
BOOL PlgManager::SetUdp(int no, const char* ip, WORD port)
{
	_ASSERT(0 <= no);
	_ASSERT(MAX_NUM_UDP > no);


	WSADATA wsaData;
	WORD wVersionRequired = MAKEWORD(2, 2);
	WSAStartup(wVersionRequired, &wsaData);
	m_sock[no] = (int)socket(AF_INET, SOCK_DGRAM, 0);
	if (INVALID_SOCKET == m_sock[no]) return FALSE;
	m_addr[no].sin_family = AF_INET;
	m_addr[no].sin_addr.s_addr = inet_addr(ip);
    m_addr[no].sin_port = htons(port);
	return TRUE;
}

// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// �ʔ����Ǘ��֘A

//------------------------------------------
// ������_�ł̋����Ǘ� (1�t���[����荞�݊���)
// SEQ_TYPE_DATA_FRAME const* p �V�[�P���T�����1�t���[���`��
//------------------------------------------
void PlgManager::NextLength_Ken(SEQ_TYPE_DATA_FRAME const* p)
{
	double	dLen_Ken[2];								// �e�������u�ʒu�ł̒ʔ��� [mm] (�\/�� ��)
	DWORD	nFrameNo;									// �t���[��No (0�I���W��)
	
	// ���ݒl�ێ�
	for(int ii=0; ii<NUM_MEN; ii++) dLen_Ken[ii] = m_dLen_Ken[ii];
	nFrameNo = m_nFrameNo;

	//================================================
	// �V�[�P���T��� �������`�F�b�N
	if( m_nFrameNo != p->nFrameNo ) {
		LOG(em_ERR), "[%s] �V�[�P���T �t���[���`���ُ� Fno(��:seq)%d!=%d", my_sThreadName, m_nFrameNo, p->nFrameNo);
		syslog(246, "[Fno(��:seq)%d!=%d]", m_nFrameNo, p->nFrameNo);

		mcls_pStatus->AddQueMyErrWar(true, ON_PLG_FRAME_DELETE);		// �t���[�������B������~
	}

	//================================================
	// �ʔ����Ǘ�
	double tm = p->nTimeDiff;														// 10��s
	long frame_diff;
	if (DIV_PLG_TEST == m_emIsLinePLG) {
		double rate = (double)mcls_pParam->GetParamCommon().typCamCon[DIV_PLG_TEST - 1].nDiv / (double)mcls_pParam->GetParamCommon().typCamCon[DIV_PLG_TEST - 1].nMul;
		frame_diff = (long)((double)((long)p->nExSyncAdd - (long)m_nPreExSync) * rate);
		//LOG(em_INF), "������_ PLG ExSyncAdd:%d PreExSync:%d rate(nDiv:%d nMul:%d):%f diff:%d", (long)p->nExSyncAdd, (long)m_nPreExSync, mcls_pParam->GetParamCommon().typCamCon[DIV_PLG_TEST - 1].nDiv, mcls_pParam->GetParamCommon().typCamCon[DIV_PLG_TEST - 1].nMul, rate, frame_diff);
		m_nPreExSync = p->nExSyncAdd;
	}
	else {
		frame_diff = p->nPlgDiff;
	}
//LOG(em_INF), "PLG SampCnt:%d GetSampBuffPlg(now:%d,%d, next:%d,%d) diff:%d", m_nCurrentSampCnt, buff_pos, GetSampBuffPlg(buff_pos), next_buff_pos, GetSampBuffPlg(next_buff_pos), frame_diff);


	//================================================
	// ����̏�񌈒�
	m_nTime = (int)(tm / 100.0 + 0.5F);					// 10us �� ms
	m_dDistance = m_dPlsR_Ken * (double)(frame_diff);				// �P�t���[���Ői�񂾋���
	if( m_dDistance < 0 ) m_dDistance = 0;							// �O�N���A�̏ꍇ�͖���

	m_nSpeed	= (DWORD)(m_dDistance / ((double)(m_nTime/1000.0) ) + 0.5F);	// [mm/sec]

#ifndef RESV_KOTEI
	m_dResV		= (double)(m_dDistance / (SAMPBUFFSIZE));			// ����\����
#else
	m_dResVCycle= (double)(m_dDistance / (SIZE_FRAME_IMG_Y));			// ����\���� 
#endif

	
	////====================================
	//// UDP���Ǘ�(ZZ_EPC�p)

#ifdef DISABLE_ZZ_EPC
	//// ���ʏ��
	m_udp.framenum	= m_nFrameNo;
	m_udp.vRes		= (float)m_dResV;

	// UDP�p�P�b�g���M
#ifdef DEBUG_LOG_UDP
	LOG(em_INF), "[%s] <frm:%d> vRes:%.3f", my_sThreadName, m_udp.framenum, m_udp.vRes);
#endif

	sendto( GetSock(0), (char *)&m_udp, sizeof(m_udp), 0, (sockaddr *)GetAddr(0), sizeof(sockaddr_in));
#endif

	////====================================
	//// �G�b�W�ʒu��� UDP���M
	int		nUseLimit[NUM_MEN];
	float	dEdge_Left[NUM_MEN];
	float	dEdge_Right[NUM_MEN];
	for(int ii=0; ii<NUM_MEN; ii++) {
		nUseLimit[ii] = (mcls_pEdgePosCont->GetEdgeAns(ii) ? 1 : 0);		// �ŏI�G�b�W�ʒu�����펞�A�G�b�W�͈͎w��Ō��o
		dEdge_Left[ii] = mcls_pEdgePosCont->GetEdgeAnsPos(ii, 0);			// ���G�b�W
		dEdge_Right[ii] = mcls_pEdgePosCont->GetEdgeAnsPos(ii, 1);			// �E�G�b�W
	}
	mcls_pEdgeRestSd->SendEdgeRestrict(m_nFrameNo, nUseLimit, dEdge_Left, dEdge_Right);

	////====================================
	//// �t���[��No�g���b�L���O���Ǘ�
	memset(&m_trFrm, 0x00, sizeof(m_trFrm));
	for(int ii=0; ii<NUM_MEN; ii++) {
		if (0 != m_nDly[ii]) {
			LOG(em_MSG), "[%s] <%s> ���x�����s�L�[���s=%d FrameNo=%d ChgFNo:%d", my_sThreadName, DivNameManager::GetTorB(ii), m_nDly[ii], m_nFrameNo, m_nChgFrameNo[ii]);
			m_nDly[ii] = 0;
		}
		if(m_nFrameNo >= m_nChgFrameNo[ii] && 0 < m_nChgFrameNo[ii]) {
			m_trFrm.nCoilChg[ii] = 1;

			LOG(em_MSG), "[%s] <%s> �x�����s�L�[���s=%d FrameNo=%d ChgFNo:%d", my_sThreadName, DivNameManager::GetTorB(ii), m_nDly[ii], m_nFrameNo, m_nChgFrameNo[ii]);
			syslog(271, "[%s]", DivNameManager::GetTorB(ii));
			m_nChgFrameNo[ii] = 0;
		}
	}
	m_trFrm.nFrameNo = m_nFrameNo;
	m_trFrm.nSpeed = m_nSpeed;
	QueryPerformanceCounter((LARGE_INTEGER*)&m_trFrm.nRecProcTime[0]);		// 1�t���[����荞�݊�������QueryPerformanceCounter�l
	//// �o�������R�C���؂�ւ�
	if( m_bCoilOutChange ) m_trFrm.nFCut = 1;							// �t�@�[�X�g�J�b�g�؂�ւ�

	mcls_pTrFrm->AddItem(-1, &m_trFrm);										// �S���ɓo�^

#ifdef ENABLE_CYCLE
	for(int ii=0; ii<NUM_MEN; ii++) {
		if( DIV_SYS_RUN != mcls_pStatus->GetCycleSys(ii) ) {
			// �����@�\���~�܂��Ă���ꍇ�́A�o�^�ςݏ����폜
			int nId = GetTrId(DIV_CYCLE, ii);
			mcls_pTrFrm->DelItem(nId, m_nFrameNo);
		}
	}
#endif
	//// �o�������R�C���؂�ւ�
	// �t�@�[�X�g�J�b�g
	if( m_bCoilOutChange ) {
		LOG(em_MSG), "[%s] <%s> �o�������R�C�� �t�@�[�X�g�J�b�g���s[�o:%.2f(m)]", my_sThreadName, DivNameManager::GetTorB(0), m_dLen_OutCut/1000.0);
		//syslog();
		m_bCoilOutChange = false;
	}
	//// �C�j�V�����`����M�t���O�ɂ��؂�ւ�
	if(m_bRecvInit) {
		LOG(em_MSG), "[%s] <%s> �C�j�V�����`����M�ɂ��R�C���؂�ւ�[��:%.2f(m),�\:%.2f(m)]", my_sThreadName, DivNameManager::GetTorB(0), m_dLen_Ken[0]/1000.0, m_dLen_Ken[1]/1000.0);
		//syslog();
		m_bRecvInit = false;
	}

	//================================================
	// ��� �X�V

	//// �t���[��No���Z
	m_nFrameNo = p->nFrameNo + 1;

	//// �ʔ������Z
	m_dLen_Ken[m_nDistPos]		+= m_dDistance;							// ���
	m_dLen_Ken[m_nDistNotPos]	+= m_dDistance;							// �������
	m_dLen_WpdKenPer			-= m_dDistance;

	// ��������̕��ɂ��n�ړ_���B��c��
	if (m_bCoilChange_Ken && m_dCoilOldLen_Ken <= m_dLen_Ken[m_nDistNotPos]) {
		// ���C���C���X�^���X�ɒʒm
		//  (�������đ��N���X�Ƃ��ƈ�Ăɏ������������ׁA��xMainInctance�ɒʒm����)
		// �܂�A�����ł͔c�����邾���ŁA���ې؂�ւ��́AMainInctance���珈�����Ăяo�����
		SetEvent(m_evKenNotPos);
	}

	//// ������B�H
	if(COIL_LEN_MAX *1000 < m_dLen_Ken[m_nDistPos] ) {
		if(!m_bMaxLen_Ken) {
			// �R�C����������B
			LOG(em_ERR), "[%s] �����p�R�C����������B [�R�C��No=%s]", my_sThreadName, mcls_pCoil->GetCoilKenR(m_nDistPos)->data.cCoilNoIn );
			syslog(275, "[�����p�R�C��: �R�C��No=%s]", mcls_pCoil->GetCoilKenR(m_nDistPos)->data.cCoilNoIn);
			mcls_pStatus->SetStatusQue("MAXLEN_KEN_ID", false, false);
			m_bMaxLen_Ken = true;
		
			//// �����R�C���ؑ� 
			//SetEvent(m_evCoilLenMaxKen);
		}
	}

	mcls_Time.PutTimestamp(1);

	//// �����p
	if (true) {			// PLG�̃��O�o��
		// �R�C���c��
		long nLenZan[NUM_MEN];		// �R�C���c��[mm] = �����R�C���� - �ʔ��� [mm] ���_�~�[�R�C���͂l�`�w�R�C����
		for (int ii = 0; ii<NUM_MEN; ii++) {
			if (mcls_pCoil->GetCoilKenR(ii)->bDummeyCoil) nLenZan[ii] = (COIL_LEN_MAX * 1000) - (long)dLen_Ken[ii];	// �_�~�[�R�C��
			else nLenZan[ii] = mcls_pCoil->GetCoilKenR(ii)->data.nLenIn * 1000 - (long)dLen_Ken[ii];							// �v���R���R�C��
		}
		//// �o���R�C����
		// �G�b�W�ʒu & ���f�t���[��No
		DWORD	nFNoWk[NUM_MEN];
		float	edgePosL[NUM_MEN];		// �G�b�W�ʒu����(mm)
		float	edgePosR[NUM_MEN];		// �G�b�W�ʒu����(mm)
		for (int ii = 0; ii<NUM_MEN; ii++) mcls_pEdgePosCont->SearchEdgePos(ii, edgePosL[ii], edgePosR[ii], &nFNoWk[ii]); 	// �G�b�W�ʒu����
		
		//LOGPLG(em_INF), ",UDP FNo,�\[m],��[m],�\�c[m],���c[m],�o�\[m],�o��[m],���x[mpm],��������[ms],���ԍ�[ms],�G�b�W�ʒu����_�\[mm],�G�b�W�ʒu����_�\[mm],�G�b�W�ʒu����_��[mm],�G�b�W�ʒu����_��[mm],�G�b�W�ʒu�t���[��No_�\,�G�b�W�ʒu�t���[��No_��");	
		//LOGPLG(em_NON), ",%06d, %.3f, %.3f, %.3f, %.3f, %.3f, %.3f,%d,%d,%d,%d, %d, %d, %d, %.3f, %.3f, %.3f, %.3f, %d,%d",
		LOGPLG(em_NON), ",%06d, %.3f, %.3f, %.3f, %.3f,%d,%d,%d, %d, %d, %d, %.3f, %.3f, %.3f, %.3f, %d,%d, %lf",
			nFrameNo, dLen_Ken[0]/1000.0, dLen_Ken[1]/1000.0, nLenZan[0]/1000.0, nLenZan[1]/1000.0,
			m_trFrm.nCoilChg[0], m_trFrm.nCoilChg[1], m_trFrm.nFCut,
			GetSpeed(),	(int)mcls_Time.GetTimestamp(1,0), (int)mcls_Time.GetCycleTime(),
			edgePosL[0], edgePosR[0], edgePosL[1], edgePosR[1], nFNoWk[0], nFNoWk[1],
			m_dResVCycle);
	}

	////================================================
	//// WPD�F��
	//if( -1 != p->nWpdFramePos ) {
	//	WpdTrKenStart();
	//}
}

//------------------------------------------
// ���������
//------------------------------------------
void PlgManager::ExecCycle()
{
	// ���Ə�Ԋ֌W
	SEQ_DM_RECV_BASE dm = mcls_pDmem->GetDmRecv();			// �R�s�[�����璍��
	DWORD nLvalAdd = dm.ken.nLvalAdd;
	//if (mcls_pParam->IsLedCycle()) nLvalAdd /= 2;			// ���ݓ_���Ȃ甼��	 �����͔����ɂ���K�v�Ȃ��B�n������\���p���X���[�g�̂���

	mcls_pSogyo->SetNowSampCnt(nLvalAdd, m_dResV);		// ���݂̃T���v�����O�N���b�N�l �擾
#ifdef DSP_SOGYO_CHECK
	if (DIV_PLG_TEST == mcls_pStatus->GetPlg()) {
		DWORD nPlgAddDsp = dm.ken.nPlgAdd;
		mcls_pSogyo->SetNowSampCntDsp(nPlgAddDsp, m_dResV);		// ���݂̃T���v�����O�N���b�N�l �擾
	}
	else {
		DWORD nPlgAddDsp = dm.dsp.nPlgAdd;
		mcls_pSogyo->SetNowSampCntDsp(nPlgAddDsp, m_dPlsR_Dsp);	// ���݂̃T���v�����O�N���b�N�l �擾
	}
#endif


	// ����
	mcls_pStatus->SetLength( (int)(m_dLen_Ken[m_nDistPos]/1000.0) );
}

//------------------------------------------
// �\����_�ł� �����Ǘ�
//------------------------------------------
void PlgManager::NextLength_Dsp()
{

	//================================================
	// �������Z

	// �f�[�^�擾
	SEQ_DM_RECV_BASE dm = mcls_pDmem->GetDmRecv();		// �R�s�[�����璍��


	//// �\������ �Z�o
	int sa;											// ����̐i��PLG�J�E���g
	DWORD nOldCount = m_preCountDsp;
	double dDistance;									// ����̐i�񂾋���
	// �e�X�g�p���X���͌����������g�p
	if (DIV_PLG_TEST == m_emIsLinePLG) {
		double rate = (double)mcls_pParam->GetParamCommon().typCamCon[DIV_PLG_TEST - 1].nDiv / (double)mcls_pParam->GetParamCommon().typCamCon[DIV_PLG_TEST - 1].nMul;
		sa = (int)((int)(dm.ken.nLvalAdd - m_preCountDsp) * rate);
		dDistance = sa * m_dPlsR_Ken;
		//LOG(em_INF), "�\����_ PLG FrameNo:%d nLvalAdd:%d preCountDsp:%d rate(nDiv:%d nMul:%d):%f diff:%d", dm.ken.nFrameNo, dm.ken.nLvalAdd, m_preCountDsp, mcls_pParam->GetParamCommon().typCamCon[DIV_PLG_TEST - 1].nDiv, mcls_pParam->GetParamCommon().typCamCon[DIV_PLG_TEST - 1].nMul, rate, sa);
		m_preCountDsp = dm.ken.nLvalAdd;					// ���Z�b�g
	}
	// �\��PLG�������͌����������g�p
	// �����n�������̊��Ԍ���
	else if (m_bIsDisableDspPLG) {
		sa = dm.ken.nPlgAdd - m_preCountDsp;
		dDistance = sa * m_dPlsR_Ken;
		m_preCountDsp = dm.ken.nPlgAdd;						// ���Z�b�g
	}
	// ����ȊO�ł͕\���������g�p
	else {
		sa = dm.dsp.nPlgAdd - m_preCountDsp;
		dDistance = sa * m_dPlsR_Dsp;
		m_preCountDsp = dm.dsp.nPlgAdd;						// ���Z�b�g
	}
	if (m_nMin > m_preCountDsp) { m_nMin = m_preCountDsp; }
	if (m_nMax < m_preCountDsp) { m_nMax = m_preCountDsp; }
	
	// �\�t�g�΍�
	if (COIL_LEN_MAX * 1000 < dDistance) {
		LOG(em_ERR), "[PlgManager] �\���������ُ�[���F%d][DM:%lu��%lu][%.3lfmm][min:%lu][max:%lu]", sa, nOldCount, m_preCountDsp, dDistance, m_nMin, m_nMax);
		//�����f�o�b�O�p�i������폜�\��j
		syslog(901, "[���F%d][DM:%lu��%lu][%.3lfmm][min:%lu][max:%lu]", sa, nOldCount, m_preCountDsp, dDistance, m_nMin, m_nMax);
		m_preCountDsp = nOldCount;		// �t�]���Ƃ̎��́A�O��l�͕ێ����Ȃ�(�t�]���̋����Ǐ]�͂��Ȃ�)
		return;
	}

	if( 0 != sa) {
		if (0>sa) {
			if (-20>sa)LOG(em_MSG), "[PlgManager] �t�]����[���F%d][DM:%lu��%lu][%.3lfmm][min:%lu][max:%lu]",	sa, nOldCount, m_preCountDsp, dDistance, m_nMin, m_nMax);
			m_preCountDsp = nOldCount;		// �t�]���Ƃ̎��́A�O��l�͕ێ����Ȃ�(�t�]���̋����Ǐ]�͂��Ȃ�)
			return;
		}
		mcls_TimeDspSpeed.CounterCycle();

		NextLength_Dsp(dDistance);
	}

}

//------------------------------------------
// �\����_�ł� �����Ǘ�
// double dDistance ����i�񂾋���
//------------------------------------------
void PlgManager::NextLength_Dsp(double dDistance)
{
	m_dLen_BaseDsp += dDistance;
	for(int ii=0; ii<POS_DSP_END; ii++) m_dLen_Dsp[ii] += dDistance;
	m_dLen_OutCut += dDistance;


	//================================================
	//// �e�\���ʒu�ɃR�C����[�����B
	for(int ii=0; ii<POS_DSP_END; ii++) {
	
		if( m_bCoilChange_Dsp[ii] && m_dCoilOldLen_BaseDsp <= m_dLen_Dsp[ii] ) {
			//double	dInitLen	= 0.0;											// �R�C���ؑ֎��̈ʒu

			// �e�|�C���g�ő��� (�������Ȃ��ƁA�^�C�~���O�ɂ���ẮA�ŏI�s������Ȃ�)
			OpSendFunc::SendOp_Length();

			// �؂�ւ�����
			if (POS_DSP_SHR_I > ii) {
				LOG(em_MSG), "[%s] �\�������Ǘ�[%d] �R�C���؂�ւ� ����������=%0.3fm [%s]", my_sThreadName, ii, m_dLen_Dsp[ii] / 1000.0, mcls_pCoil->GetCoilDsp(ii)->cKizukenNo );
				mcls_pCoil->ChangeCoilDspPointer((EM_POS_DSP)ii, m_dLen_Dsp[ii], m_dLen_OutCut);
				m_dLen_Dsp[ii] = 0.0;
				m_bCoilChange_Dsp[ii] = false;
			}

			// �V���[�ʒu�ʉߎ��Ǝ�����
			else if (POS_DSP_SHR_I == ii) {
				LOG(em_MSG), "[PlgManager] �V���[�ʒu �R�C���؂�ւ� ����������=%0.3fm", m_dLen_Dsp[ii] / 1000.0);

				//double dLen_OutSha = m_dLen_OutCut - dInitLen;		// �V���[�ʒu���R�C����[���ʉߎ��̏o������
				//mcls_pCoil->ChangeCoilShaPointer(m_dLen_Dsp[ii], dLen_OutSha);
				mcls_pCoil->ChangeCoilShaPointer(m_dLen_Dsp[ii], m_dLen_OutCut);
				m_dLen_Dsp[ii] = 0.0;
				m_bCoilChange_Dsp[ii] = false;
			}
		}
	}


	//// ������B�H
	if(COIL_LEN_MAX *1000 < m_dLen_BaseDsp ) {
		if(!m_bMaxLen_Dsp) {
			// �R�C����������B
			if(NULL != mcls_pCoil->GetCoilBaseDsp()) {
				LOG(em_ERR), "[%s] �\���p�R�C����������B [�R�C��No=%s]", my_sThreadName, mcls_pCoil->GetCoilBaseDsp()->data.cCoilNoIn );
				syslog(275, "[�\���p�R�C��: �R�C��No=%s]", mcls_pCoil->GetCoilBaseDsp()->data.cCoilNoIn);
//#ifndef PLG_DSP_KEN_EQAL		
				mcls_pStatus->SetStatusQue("MAXLEN_DSP_ID", false, false);
//#endif
				m_bMaxLen_Dsp = true;
				//// �����R�C���ؑ� 
				//SetEvent(m_evCoilLenMaxDsp);

			}
		}
	}

	mcls_TimeDspSpeed.PutTimestamp(1);

	// �O��Ƃ̎��ԍ�
	double dWk = mcls_TimeDspSpeed.GetTimestamp(1, 0);

	m_nSpeedDsp = (DWORD)(dDistance / ((double)(dWk / 1000.0)) + 0.5F);	// [mm/sec]

	LOGPLGDSP(em_NON), ", %.3f, %.3f, %.3f, %.3f, %.3f,%d,%d,%d, %d, %d, %d",
		m_dLen_BaseDsp/1000.0, m_dLen_Dsp[0]/1000.0, m_dLen_Dsp[1]/1000.0, m_dLen_Dsp[2]/1000.0, m_dLen_OutCut/1000.0,
		m_bCoilChange_Dsp[0]?1:0, m_bCoilChange_Dsp[1]?1:0, m_bCoilChange_Dsp[2]?1:0,
		GetSpeed(),	(int)dWk, (int)mcls_TimeDspSpeed.GetCycleTime());
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
//// ����

//==========================================
// �S�����苗��
//------------------------------------------
// int pos	
//==========================================
double PlgManager::GetLen_All(int pos)
{
	// ������_�@�����ʒu(�\��/����)
	if (pos == DIV_LEN_POS_KEN_TOP || pos == DIV_LEN_POS_KEN_BOT) {
		int nMen = pos - DIV_LEN_POS_KEN_TOP;
		return GetLen_Ken(nMen);
	}
	// �\����_�@�\�ʌ�����ʒu
	else if( pos == DIV_LEN_POS_DSP_TOP) {
		return GetLen_BaseDsp();

	}
	// �\����_�@���ʌ�����ʒu
	else if (pos == DIV_LEN_POS_DSP_BOT) {
		return GetLen_BaseDsp();

	}
	// ������ʒu
	else {
		return GetLen_Dsp((pos - DIV_DISP_POS_DSP_P1));
	}
}



// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// �؂�ւ�

//------------------------------------------
// ������_�ŃR�C���؂�ւ�
//------------------------------------------
void PlgManager::CoilChange_KenBase()
{
	m_dLen_WpdKenPer = mcls_pParam->GetCommonMast().Dist.nKenBase;
}

//------------------------------------------
// �������u�ʒu�ŃR�C���؂�ւ�
// int men 0�`
//------------------------------------------
void PlgManager::CoilChange_Ken(int men)
{


	//// �Ō�̒����𑗂��Ă���
	OpSendFunc::SendOp_Length();

	//================================================
	// ��ʂ̐؂�ւ�
	if( m_nDistPos == men ) {

		//// ����ύX
		m_dCoilOldLen_Ken = m_dLen_Ken[m_nDistPos];		// �O��l �ێ�


		//// ��ʂ̏��
		m_nDly[m_nDistPos]				= 1;								// �x�����s�L�[�Z�b�g
		m_dLen_Ken[m_nDistPos]			= 0.0;								// ����������
		m_dLen_KenSub					= 0.0;

		m_bCoilChange_Ken	= true;											// �؂�ւ���
		LOG(em_MSG), "[%s] ��ʌ����ʒu �R�C���؂�ւ� ����������=%0.3fm", my_sThreadName, m_dCoilOldLen_Ken/1000.0);

		//// �p���X���[�g�ύX
		int idx = 0;
		if (m_dPlsR_Ken != mcls_pParam->GetParamCommon().dPls) {
			LOG(em_MSG), "[%s] �����p�p���X���[�g�ύX %f��%f", my_sThreadName, m_dPlsR_Ken, mcls_pParam->GetParamCommon().dPls);
			syslog(276, "[�����p=%f��%f]", m_dPlsR_Ken, mcls_pParam->GetParamCommon().dPls);

			m_dPlsR_Ken	 = mcls_pParam->GetParamCommon().dPls;
			m_dResV		 = mcls_pParam->CalcResV( m_emIsLinePLG );
		}
		if (m_dPlsR_Sub != mcls_pParam->GetParamCommon().dPlsSub) {
			LOG(em_MSG), "[%s] ������r�p�p���X���[�g�ύX %f��%f", my_sThreadName, m_dPlsR_Sub, mcls_pParam->GetParamCommon().dPlsSub);
			syslog(276, "[������r�p=%f��%f]", m_dPlsR_Sub, mcls_pParam->GetParamCommon().dPlsSub);

			m_dPlsR_Sub = mcls_pParam->GetParamCommon().dPlsSub;
		}

		//// �R�C������ԃ`�F�b�N
		if( m_bMaxLen_Ken ) mcls_pStatus->SetStatusQue("MAXLEN_KEN_ID", true, false);
		m_bMaxLen_Ken = false;	
	} 
	
	//================================================
	// ��������̖ʂ̐؂�ւ�
	else {
		double wk = m_dLen_Ken[m_nDistNotPos]/1000.0;				// �O��l

		// ����������O��l�܂œ��B = ��������̈ʒu�܂ŃR�C���؂�ւ�肪����
		m_nDly[m_nDistNotPos]		= 1;							// �x�����s�L�[�Z�b�g
		m_dLen_Ken[m_nDistNotPos]	= m_dLen_Ken[m_nDistPos] - m_nDistLen;	// �����ʂ̃R�C���擪�t���[���́A0m�ƂȂ�Ȃ��B�\���ԋ������t���[���P�ʂŊ���؂�Ȃ����̌덷�������ŋz���B(�\���ԋ����������ɍl������)
		
		m_bCoilChange_Ken			= false;
		LOG(em_MSG), "[%s] ������(%d) �����ʒu �R�C���؂�ւ� ����������=%0.3fm, ��ʎ�����=%0.3fm, ����̃t���[���擪�ʒu=%0.3fm", my_sThreadName, men, wk, m_dCoilOldLen_Ken/1000.0, m_dLen_Ken[men]/1000.0);
	}
	
	syslog(259, "[%s]", DivNameManager::GetTorB(men));
}

//------------------------------------------
// �\����_�ʒu�ŃR�C���؂�ւ�
//------------------------------------------
void PlgManager::CoilChange_BaseDsp()
{
	ParamManager::P_COMMON_MAST	const& prm	= mcls_pParam->GetCommonMast();

	//// �\����_��WPD�̋�������Ƃ���
	m_dCoilOldLen_BaseDsp	= m_dLen_BaseDsp;							// �O��l �ێ�
	m_dLen_BaseDsp			= 0.0;										// ����������

	//// �e�����ʒu�̏��X�V
	double dWkLen[POS_DSP_END];
	dWkLen[POS_DSP_DSP1_P] = (double)prm.Dist.nDspBaseKensaTop;			// ������ʒu1�i�\������j
	dWkLen[POS_DSP_DSP2_P] = (double)prm.Dist.nDspBaseKensaBot;			// ������ʒu2�i��������j
	dWkLen[POS_DSP_SHR_I] = (double)prm.Dist.nDspSharCut;				// �V���[�ʒu

	for(int ii=0; ii<POS_DSP_END; ii++) {
		// �e�\���ʒu
		double wk = m_dCoilOldLen_BaseDsp - dWkLen[ii];
		if(POS_DSP_DSP2_P !=ii) // ���ʌ�����̎��̓��O�\���Ȃ�
			LOG(em_MSG), "[%s] �\���ʒu[%d] �ʒu�̋����ύX [%0.3fm]��[%0.3fm]", my_sThreadName, ii, m_dLen_Dsp[ii]/1000.0, wk/1000.0);	// ��{�I�ɓ����l�ƂȂ�͂�
		m_dLen_Dsp[ii]			= wk;										// ��_����ǂꂾ������Ă��邩
		m_bCoilChange_Dsp[ii]	= true;
	}
	
	//// �p���X���[�g�ύX
	if (m_dPlsR_Dsp != mcls_pParam->GetParamCommon().dPlsDsp) {
		LOG(em_MSG), "[%s] �\���p�p���X���[�g�ύX %f��%f", my_sThreadName, m_dPlsR_Dsp, mcls_pParam->GetParamCommon().dPlsDsp);
		syslog(276, "[�\���p=%f��%f]", m_dPlsR_Dsp, mcls_pParam->GetParamCommon().dPlsDsp);

		m_dPlsR_Dsp	 = mcls_pParam->GetParamCommon().dPlsDsp;
	}
  
	//// �R�C������ԃ`�F�b�N
	if( m_bMaxLen_Dsp ) mcls_pStatus->SetStatusQue("MAXLEN_DSP_ID", true, false);
	m_bMaxLen_Dsp = false;
}

//------------------------------------------
// �V���[�J�b�g�ʒu�ŏo�������R�C���؂�ւ� (PIO�Ƃ��ő����ʒm��)
// double dNow ���݂̒ʔ��� [mm]
//------------------------------------------
void PlgManager::CoilChange_OutCut()
{
	m_dLen_OutCut = 0.0;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
//// ���ю��W�֘A
//==========================================
// �ŏ���Ԃ̏�Ԕc��
//==========================================
void PlgManager::CheckIntervalStatus()
{
	//==============================================
	//// �ŏ���ԃs�b�`�ŋ@���Ԃ��T���v�����O����
	int				row;		// �Z�b�g�ʒu
	EM_DIV_KENSA	emKensa;	// ������ԃ��[�N
	EM_DIV_KIKI		emKiki;		// �@���ԃ��[�N

	for (int ii = 0; ii<NUM_MEN; ii++) {
		row = (int)(m_dLen_Ken[ii] / (MAP_REPRES_MIN * 1000));
		if (0>row || MAP_ROW_NUM <= row) continue;				// �ʒu�ُ�


																// ������ԃZ�b�g
		emKensa = mcls_pStatus->GetKensa(ii);
		mcls_pCoil->SetData_StKensa(ii, row, emKensa);
		// �@���ԃZ�b�g
		emKiki = mcls_pStatus->GetKiki(ii);
		mcls_pCoil->SetData_StKiki(ii, row, emKiki);

#ifdef ENABLE_CYCLE			// �����@�\�̏�Ԃ��Z�b�g
			//// �����@�\�̏�Ԃ��`�F�b�N
			EM_DIV_CYCLESTAT  emCycStat;
			if (DIV_SYS_RUN != mcls_pStatus->GetCycleSys(ii)) {
				// �����@�\���~�܂��Ă���ꍇ�́A�ΏۊO
				emCycStat = DIV_CYCLESTAT_STOP;
			}
			else {
				emCycStat = DIV_CYCLESTAT_OK;
			}

			// �����@�\��ԃZ�b�g
			mcls_pCoil->SetData_Row_R(ii, row, 0, emCycStat);
#endif
	}
}
