// ContecCnt8mManager.cpp: ContecCnt8mManager �N���X�̃C���v�������e�[�V����
//
//////////////////////////////////////////////////////////////////////
#include "stdafx.h"

#include "ContecCnt8mManager.h"
#ifndef  DEFECTLIB_CONTEC_CNT_NON		////// ���S������

//////////////////////////////////////////////////////////////////////
// �\�z/����
//////////////////////////////////////////////////////////////////////
//------------------------------------------
// �R���X�g���N�^
// WORD nChannelNum			�g�p�`�����l����
// WORD nSampChannelNum		�T���v�����O����`�����l����
// DWORD nSampBuffSize		�T���v�����O�o�b�t�@�̃T�C�Y
// DWORD nSampBuffNum		�T���v�����O�o�b�t�@�̃`���l����
//------------------------------------------
ContecCnt8mManager::ContecCnt8mManager(WORD nChannelNum, WORD nSampChannelNum, DWORD nSampBuffSize, DWORD nSampBuffNum) :
m_hDrv(CONTEC_CNT_NOID),
m_nChannelNum(nChannelNum),
m_nSampChannelNum(nSampChannelNum),
m_nSampBufSize(nSampBuffSize),
m_nSampBufNum(nSampBuffNum),
m_errCode(0),
m_pSampBuf(NULL),
m_bStatus(true),
my_nDistPos(0),
my_nDistNotPos(1),
my_nDistLen(0),
my_bSendflg(false),
m_bSmyu(false)
{
	_ASSERT(0 != m_nChannelNum);
	_ASSERT(mc_nMaxCannel >= m_nChannelNum);

	//// �p�����[�^�����ݒ� (�f�t�H���g�l�Ƃ��ăZ�b�g���Ă���)
	// �J�E���^�[�{�[�h�S�ʂ̐ݒ�p�����[�^
	m_parmCntBase.m_bTest = true;
	// �J�E���^�[�{�[�h�`�����l�����̐ݒ�p�����[�^
	for(WORD ii=0; ii<m_nChannelNum; ii++ ) {
		m_nChannel[ii] = ii;
		m_parmCntCH[ii].m_nSigType		= CNT_SIGTYPE_LINERECEIVER;
		m_parmCntCH[ii].m_nPhase		= CNT_MODE_1PHASE;
		m_parmCntCH[ii].m_nMul			= CNT_MUL_X1;
		m_parmCntCH[ii].m_nSyncClr		= CNT_CLR_ASYNC;
		m_parmCntCH[ii].m_nZmode		= CNT_ZPHASE_NOT_USE;
		m_parmCntCH[ii].m_nZlogic		= CNT_ZLOGIC_NEGATIVE;
		m_parmCntCH[ii].m_nDir			= CNT_DIR_UP;
		m_parmCntCH[ii].m_nFilterValue	= 0;
	}
	// �J�E���^�[�{�[�h�T���v�����O�����ݒ�p�����[�^
#ifndef DEFECTLIB_CONTEC_CNT_WMD		// �]����
	m_parmCntSamp.m_nStart				= CNT8M_START_SOFT;
	m_parmCntSamp.m_nCountStart			= CNT8M_START_CNT_NOT_WITH_SAMP;
	m_parmCntSamp.m_nClock				= CNT8M_CLOCK_EXT_TRG;
	m_parmCntSamp.m_nStop				= CNT8M_STOP_SOFT;
	m_parmCntSamp.m_nCountStop			= CNT8M_STOP_CNT_NOT_WITH_SAMP;
#else									// WMD��
	m_parmCntSamp.m_nStart				= CNTS_START_SOFT;
	m_parmCntSamp.m_nCountStart			= CNTS_START_CNT_NOT_WITH_SAMP;
	m_parmCntSamp.m_nClock				= CNTS_CLOCK_EXT_TRG;
	m_parmCntSamp.m_nStop				= CNTS_STOP_SOFT;
	m_parmCntSamp.m_nCountStop			= CNTS_STOP_CNT_NOT_WITH_SAMP;
#endif	

	//// �T���v�����O�o�b�t�@�����A������
	if( m_nSampBufSize*m_nSampBufNum > 0 ) {
		//m_pSampBuf = new SampData[m_nSampBufSize*m_nSampBufNum];
		//memset(m_pSampBuf, 0x00, sizeof(SampData)*(m_nSampBufSize*m_nSampBufNum));
		m_pSampBuf = new DWORD[m_nSampBufSize*m_nSampBufNum*m_nSampChannelNum];
		memset(m_pSampBuf, 0x00, sizeof(DWORD)*(m_nSampBufSize*m_nSampBufNum*m_nSampChannelNum));
	}

	//// UDP�\�P�b�g������
	for( int ii=0; ii<mc_nMaxUdp; ii++) {
		m_sock[ii] = INVALID_SOCKET;
	}

	//// �����I�u�W�F�N�g����
	my_evSamplingStart	= CreateEvent(NULL, FALSE, FALSE, NULL);
	my_evSamplingStop	= CreateEvent(NULL, FALSE, FALSE, NULL);
	my_evCntError		= CreateEvent(NULL, FALSE, FALSE, NULL);
	my_evTimer			= CreateWaitableTimer(NULL, FALSE, NULL);

	InitializeCriticalSection(&my_csLock);						// �N���e�B�J���Z�N�V�����I�u�W�F�N�g��������


	//// �V�~�����[�^�p
	m_sSmyuName = "";
	m_hTblSmyu = NULL;
	mtbl_pSmyu = NULL; 
}

//------------------------------------------
// �f�X�g���N�^
//------------------------------------------
ContecCnt8mManager::~ContecCnt8mManager()
{
	// �����I�u�W�F�N�g�j��
	CloseHandle(my_evSamplingStart);
	CloseHandle(my_evSamplingStop);
	CloseHandle(my_evCntError);
	CloseHandle(my_evTimer);

	DeleteCriticalSection(&my_csLock);							// �N���e�B�J���Z�N�V�����I�u�W�F�N�g���J��

	// UDP�\�P�b�g�j��
	for( int ii=0; ii<mc_nMaxUdp; ii++) {
		if( INVALID_SOCKET != m_sock[ii] ) closesocket(m_sock[ii]);
	}

	// delete Sampling Buffer
	if( NULL != m_pSampBuf ) delete [] m_pSampBuf;

	//// �V�~�����[�^�p
	SetSmyuMode(false);
}

//------------------------------------------
// �J�E���^�[�{�[�h�ݒ�
// char* dev_name �{�[�h����
//------------------------------------------
DWORD ContecCnt8mManager::CountOpen(char* dev_name)
{
	//// �V�~�����[�^�p����
	m_sSmyuName.Format(SMYU_PLG_TBL_NAME, dev_name );


#ifndef NO_HW
	//// CntOpen
#ifndef DEFECTLIB_CONTEC_CNT_WMD		// �]����
	CNTMESS	lpCntMess;
	DWORD nRet = CntOpenEx(dev_name, &m_hDrv, 0, 0, &lpCntMess);
#else									// WMD��
	long  nRet = CntInit  (dev_name, &m_hDrv);
#endif	
	if( 0 != nRet ) {
		m_errCode	= nRet;		
		return CntOpenExErr;
	}

	//// �e�X�g�p���X
	if( m_parmCntBase.m_bTest ) {
//#ifndef DEFECTLIB_CONTEC_CNT_WMD		// �]����
//		nRet = CntTestPulse   (m_hDrv, 0, 1);
//#else									// WMD��
//		nRet = CntSetTestPulse(m_hDrv, 0, 1, 0);
//#endif
		nRet = TestPulse(0);
		if( 0 != nRet ) {
			m_errCode	= nRet;		
			return CntTestPulseErr;
		}
	}

	//// �g�p�`�����l�����ݒ���s��
	for(WORD ii=0; ii<m_nChannelNum; ii++ ) {

		// �O���M����
		nRet = CntSelectChannelSignal(m_hDrv, m_nChannel[ii], m_parmCntCH[ii].m_nSigType);
		if( 0 != nRet ) {
			m_errCode	= nRet;		
			return CntSelectChannelSignalErr;
		}
	
		// ���샂�[�h
		nRet = CntSetOperationMode(m_hDrv, m_nChannel[ii],
							m_parmCntCH[ii].m_nPhase, m_parmCntCH[ii].m_nMul, m_parmCntCH[ii].m_nSyncClr);
		if( 0 != nRet ) {
			m_errCode	= nRet;		
			return CntSetOperationModeErr;
		}

		// Z���g�p���@
		nRet = CntSetZMode(m_hDrv, m_nChannel[ii], m_parmCntCH[ii].m_nZmode);
		if( 0 != nRet ) {
			m_errCode	= nRet;		
			return CCntSetZModeErr;
		}

		// Z���_��
		nRet = CntSetZLogic(m_hDrv, m_nChannel[ii], m_parmCntCH[ii].m_nZlogic);
		if( 0 != nRet ) {
			m_errCode	= nRet;	
			return CntSetZLogicErr;
		}

		// �J�E���g����
		nRet = CntSetCountDirection(m_hDrv, m_nChannel[ii], m_parmCntCH[ii].m_nDir);
		if( 0 != nRet ) {
			m_errCode	= nRet;		
			return CntSetCountDirectionErr;
		}

		// �f�W�^���t�B���^
		nRet = CntSetDigitalFilter(m_hDrv, m_nChannel[ii],	m_parmCntCH[ii].m_nFilterValue);
		if( 0 != nRet ) {
			m_errCode	= nRet;		
			return CntSetDigitalFilterErr;
		}
	}
#endif	
	return 0;
}

//------------------------------------------
// �T���v�����O�ݒ�
//------------------------------------------
DWORD ContecCnt8mManager::SampOpen()
{
#ifdef NO_HW
	return 0;
#endif

	_ASSERT(CONTEC_CNT_NOID != m_hDrv);
	_ASSERT(0 != m_nSampChannelNum);
	
	// �T���v�����O�X�^�[�g����
#ifndef DEFECTLIB_CONTEC_CNT_WMD		// �]����
	DWORD nRet = CntSetSamplingStartTrg    (m_hDrv, m_parmCntSamp.m_nStart, m_parmCntSamp.m_nCountStart);
#else									// WMD��
	long  nRet = CntSetSamplingStartTrigger(m_hDrv, m_parmCntSamp.m_nStart, m_parmCntSamp.m_nCountStart, m_nSampChannelNum);
#endif	
	if( 0 != nRet ) {
		m_errCode	= nRet;		
		return CntSetSamplingStartTrgErr;
	}

	// �T���v�����O�N���b�N����
#ifndef DEFECTLIB_CONTEC_CNT_WMD		// �]����
	nRet = CntSetSamplingClockTrg    (m_hDrv, m_parmCntSamp.m_nClock);
#else									// WMD��
	nRet = CntSetSamplingClockTrigger(m_hDrv, m_parmCntSamp.m_nClock);
#endif	
	if( 0 != nRet ) {
		m_errCode	= nRet;		
		return CntSetSamplingClockTrgErr;
	}

	// �����N���b�N
#ifndef DEFECTLIB_CONTEC_CNT_WMD		// �]����
	nRet = CntSetSamplingClock        (m_hDrv, 1, CNT8M_TIM_UNIT_US);
#else									// WMD��
	nRet = CntSetSamplingInternalClock(m_hDrv, 1, CNTS_TIM_UNIT_US);
#endif	
	if( 0 != nRet ) {
		m_errCode	= nRet;	
		return CntSetSamplingClockErr;
	}
	
	// �T���v�����O�X�g�b�v����
#ifndef DEFECTLIB_CONTEC_CNT_WMD		// �]����
	nRet = CntSetSamplingStopTrg    (m_hDrv, m_parmCntSamp.m_nStop, m_parmCntSamp.m_nCountStop);
#else									// WMD��
	nRet = CntSetSamplingStopTrigger(m_hDrv, m_parmCntSamp.m_nStop, m_parmCntSamp.m_nCountStop, m_nSampChannelNum);
#endif	
	if( 0 != nRet ) {
		m_errCode	= nRet;	
		return CntSetSamplingStopTrgErr;
	}

	// �T���v�����O���s��CH��
#ifndef DEFECTLIB_CONTEC_CNT_WMD		// �]����
	nRet = CntSetSamplingChannel(m_hDrv, m_nSampChannelNum);
	if( nRet != 0 ) {
		m_errCode	= nRet;	
		return CntSetSamplingChannelErr;
	}
#else									// WMD��
		// �X�^�[�g���ɐݒ肷��悤�ɂȂ����ׁA�K�v�Ȃ�
#endif	
	return 0;
}

//------------------------------------------
// �J�E���^�[�X�^�[�g
//------------------------------------------
DWORD ContecCnt8mManager::CountStart()
{
#ifndef NO_HW
	_ASSERT(CONTEC_CNT_NOID != m_hDrv);

	// �J�E���g�l���[���N���A
  #ifndef DEFECTLIB_CONTEC_CNT_WMD		// �]����
	DWORD nRet = CntResetChannel(m_hDrv, m_nChannel, m_nChannelNum);
  #else									// WMD��
	DWORD nPresetData[mc_nMaxCannel];
	memset(nPresetData, 0x00, sizeof(nPresetData));
	long  nRet = CntPreset(m_hDrv, m_nChannel, m_nChannelNum, nPresetData);
  #endif	
	if( 0 != nRet ) {
		m_errCode	= nRet;	
		return CntResetChannelErr;
	}

	// �J�E���^���X�^�[�g
  #ifndef DEFECTLIB_CONTEC_CNT_WMD		// �]����
	nRet = CntStartCount(m_hDrv, m_nChannel, m_nChannelNum, 0);
  #else									// WMD��
	nRet = CntStartCount(m_hDrv, m_nChannel, m_nChannelNum);
  #endif	
	if( 0 != nRet ) {
		m_errCode	= nRet;	
		return CntStartCountErr;
	}
#endif
	return 0;
}

//------------------------------------------
// �T���v�����O�X�^�[�g
//------------------------------------------
DWORD ContecCnt8mManager::SampStart()
{
	if( ! m_bSmyu) {	
#ifndef NO_HW
		_ASSERT(CONTEC_CNT_NOID != m_hDrv);
		_ASSERT(0 != m_nSampChannelNum);
		_ASSERT(m_nSampBufNum*m_nSampBufSize> 0);

		// FIFO�����Z�b�g
	#ifndef DEFECTLIB_CONTEC_CNT_WMD		// �]����
		DWORD nRet = CntResetSampling(m_hDrv, CNT8M_RESET_FIFO);
	#else									// WMD��
		long nRet  = CntSamplingFifoReset(m_hDrv);
	#endif	
		if( 0 != nRet ) {
			m_errCode	= nRet;	
			return CntResetSamplingErr;
		}
			
		// �T���v�����O�o�b�t�@�̐ݒ�
		//memset(m_pSampBuf, 0x00, sizeof(SampData)*(m_nSampBufSize*m_nSampBufNum));
		//nRet = CntSetSamplingBuff(m_hDrv, (DWORD *)m_pSampBuf, m_nSampChannelNum, m_nSampBufNum*m_nSampBufSize, CNT8M_WRITE_RING);
		memset(m_pSampBuf, 0x00, sizeof(DWORD)*(m_nSampBufSize*m_nSampBufNum*m_nSampChannelNum));
	#ifndef DEFECTLIB_CONTEC_CNT_WMD		// �]����
		nRet = CntSetSamplingBuff  (m_hDrv, m_pSampBuf, m_nSampChannelNum, m_nSampBufNum*m_nSampBufSize, CNT8M_WRITE_RING);
	#else									// WMD��
		nRet = CntSamplingSetBuffer(m_hDrv, m_pSampBuf,	m_nSampChannelNum, m_nSampBufNum*m_nSampBufSize, CNTS_WRITE_RING);
	#endif	
		if( 0 != nRet ) {
			m_errCode	= nRet;	
			return CntSetSamplingBuffErr;
		}

		// �T���v�����O���X�^�[�g
	#ifndef DEFECTLIB_CONTEC_CNT_WMD		// �]����
		nRet = CntStartSampling(m_hDrv);
	#else									// WMD��
		nRet = CntSamplingStart(m_hDrv);
	#endif	
		if( 0 != nRet ) {
			m_errCode	= nRet;	
			return CntStartSamplingErr;
		}
#endif
		return 0;
	} else {
		SmyuCntResetSampling();
		return 0;
	}
}
//------------------------------------------
// �T���v�����O�X�g�b�v
//------------------------------------------
DWORD ContecCnt8mManager::SampStop()
{
#ifndef NO_HW
	_ASSERT(CONTEC_CNT_NOID != m_hDrv);

	// �T���v�����O��������~���܂��B
  #ifndef DEFECTLIB_CONTEC_CNT_WMD		// �]����
	DWORD nRet = CntStopSampling(m_hDrv);
  #else									// WMD��
	long nRet  = CntSamplingStop(m_hDrv);
  #endif	
	if( 0 != nRet ) {
		m_errCode	= nRet;	
		return CntStopSamplingErr;
	}
#endif
	return 0;
}
//------------------------------------------
// �J�E���^�[�X�g�b�v
//------------------------------------------
DWORD ContecCnt8mManager::CountStop()
{
#ifndef NO_HW
	_ASSERT(CONTEC_CNT_NOID != m_hDrv);

	// �J�E���^�X�g�b�v
  #ifndef DEFECTLIB_CONTEC_CNT_WMD		// �]����
	CntStopCount(m_hDrv, m_nChannel, m_nChannelNum, 0);
  #else									// WMD��
	CntStopCount(m_hDrv, m_nChannel, m_nChannelNum);
  #endif
#endif
	return 0;
}
//------------------------------------------
// �T���v�����O�X�g�b�v
//------------------------------------------
DWORD ContecCnt8mManager::CountClose()
{
#ifndef NO_HW
	_ASSERT(CONTEC_CNT_NOID != m_hDrv);

	// CntClose
  #ifndef DEFECTLIB_CONTEC_CNT_WMD		// �]����
	CntClose(m_hDrv);
  #else									// WMD��
	CntExit(m_hDrv);
  #endif
#endif
	return 0;
}

//------------------------------------------
// �� �����J�E���^�[�l�擾
// WORD ChNum   �ǂݍ��݂����`�����l����
// short* ChNo   �ǂݍ��݂����`�����l���ԍ��̔z�� (0�I���W��)
// DWORD* count �ǂݍ��񂾃J�E���g�l�̔z��
// �߂�l ���A���
//------------------------------------------
DWORD ContecCnt8mManager::GetCount(WORD ChNum, short* ChNo, DWORD* count)
{
	// �J�E���g�l�̎擾
	long  nRet = 0;
	if( ! m_bSmyu) {	
#ifndef NO_HW
		_ASSERT(CONTEC_CNT_NOID != m_hDrv);

	#ifndef DEFECTLIB_CONTEC_CNT_WMD		// �]����
		nRet = CntReadCnt(m_hDrv, (WORD*)ChNo, ChNum, count);
	#else									// WMD��
		nRet = CntReadCount(m_hDrv, ChNo, ChNum, count);
	#endif	
#endif
	} else {
		nRet = SmyuGetReadCount(ChNum, ChNo, count);
	}

	if( nRet != 0 ) {
		m_errCode	= nRet;	
		return ContecCnt8mManagerErr;
	}
	return 0;
}

//------------------------------------------
// �� �P��J�E���^�[�l�擾
// DWORD* count �ǂݍ��񂾃J�E���g�l
// WORD channel �ǂݍ��݂����`�����l�����R�[�h (0�I���W��)
// �߂�l ���A���
//------------------------------------------
DWORD ContecCnt8mManager::GetCount(DWORD* count, WORD channel)
{
	//// �z���g�́AGetCount(WORD ChNum, WORD* ChNo, DWORD* count) ������ŃR�[�����邾���ł悢���A���܂�e�X�g���ł��Ȃ��ׁA�������Ă���

	// �J�E���g�l�̎擾
	long  nRet = 0;
	if( ! m_bSmyu) {	
#ifndef NO_HW
	_ASSERT(CONTEC_CNT_NOID != m_hDrv);
	#ifndef DEFECTLIB_CONTEC_CNT_WMD		// �]����
		nRet = CntReadCnt(m_hDrv, &m_nChannel[channel], 1, count);
	#else									// WMD��
		nRet = CntReadCount(m_hDrv, &m_nChannel[channel], 1, count);
	#endif
#endif
	} else {
		nRet = SmyuGetReadCount(1, (short*)&m_nChannel[channel], count);
	}

	if( nRet != 0 ) {
		m_errCode	= nRet;	
		return ContecCnt8mManagerErr;
	}
	return 0;
}

//------------------------------------------
// ���T���v�����O�J�E���^�[�l�擾
// DWORD* sampCnt �ǂݍ��񂾃J�E���g�l   ����32bit
// DWORD* sampCarry �ǂݍ��񂾃J�E���g�l ���32bit�H
// �߂�l ���A���
//------------------------------------------
DWORD ContecCnt8mManager::GetSamplingCount(DWORD* sampCnt, DWORD* sampCarry)
{
	// �T���v�����O�񐔂̎擾
	DWORD nRet = 0;
	if(!m_bSmyu) {
#ifndef NO_HW		// �{��
		_ASSERT(CONTEC_CNT_NOID != m_hDrv);
		nRet = CntGetSamplingCount(m_hDrv, sampCnt, sampCarry);
#endif
	} else {
		nRet = SmyuGetSamplingCount(sampCnt);
	}

	if( nRet != 0 ) {
		m_errCode	= nRet;
		return CntGetSamplingCountErr;
	}
	return 0;
}
//------------------------------------------
// ���T���v�����O�J�E���^�[�l�擾
// DWORD* sampCnt �ǂݍ��񂾃J�E���g�l
// �߂�l ���A���
//------------------------------------------
DWORD ContecCnt8mManager::GetSamplingCount(DWORD* sampCnt)
{
/*
	//// �z���g�́AGetSamplingCount(DWORD* sampCnt, DWORD* sampCarry) ������ŃR�[�����邾���ł悢���A���܂�e�X�g���ł��Ȃ��ׁA�������Ă���
	_ASSERT(CONTEC_CNT_NOID != m_hDrv);

	// �T���v�����O�񐔂̎擾
	DWORD sampCarry;
	DWORD nRet = CntGetSamplingCount(m_hDrv, sampCnt, &sampCarry);
	if( nRet != 0 ) {
		m_errCode	= nRet;
		return CntGetSamplingCountErr;
	}
	return 0;
	*/

	//// �z���g�́AGetSamplingCount(DWORD* sampCnt, DWORD* sampCarry) ������ŃR�[�����邾���ł悢���A���܂�e�X�g���ł��Ȃ��ׁA�������Ă���
	// �T���v�����O�񐔂̎擾
	DWORD sampCarry;
	return GetSamplingCount(sampCnt, &sampCarry);
}

//------------------------------------------
// �T���v�����O�X�e�[�^�X���擾
// DWORD* status	�X�e�[�^�X���
// DWORD* err		�G���[���
// �߂�l ���A���
//------------------------------------------
DWORD ContecCnt8mManager::GetSampStat(DWORD* status, DWORD* err)
{
#ifndef NO_HW		// �{��
	_ASSERT(CONTEC_CNT_NOID != m_hDrv);

	DWORD nRet = CntGetSamplingStatus(m_hDrv, status, err);
	if( 0 != nRet ) {
		m_errCode	= nRet;
		return CntGetSamplingStatusErr;
	}
#endif
	return 0;
}

//------------------------------------------
// �e�X�g�p���X�̓����^�O���ؑ֐���
// short int_ext	on_off 1:�����A0:�O��
// �߂�l ���A���
//------------------------------------------
DWORD ContecCnt8mManager::TestPulse(short int_ext)
{
	long nRet = 0;
	if( ! m_bSmyu) {	
#ifndef NO_HW
		_ASSERT(CONTEC_CNT_NOID != m_hDrv);
	#ifndef DEFECTLIB_CONTEC_CNT_WMD		// �]����
		nRet = CntTestPulse   (m_hDrv, int_ext, 1);
	#else									// WMD��
		nRet = CntSetTestPulse(m_hDrv, int_ext, 1, 0);
	#endif	
#endif
	}

	if( nRet != 0 ) {
		m_errCode	= nRet;	
		return CntTestPulseErr;
	}
	return 0;
}

//////////////////////////////////////////////////////////////////////
// �\�P�b�g

//------------------------------------------
// UDP SOCKET����
// int no ����No(0�I���W��)
// const char* ip IP�A�h���X (��F192.9.200.255)
// WORD port �|�[�gNo (��F8003)
// �߂�l ���A���
//------------------------------------------
BOOL ContecCnt8mManager::SetUdp(int no, const char* ip, WORD port)
{
	_ASSERT(0 <= no);
	_ASSERT(mc_nMaxUdp > no);


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



//////////////////////////////////////////////////////////////////////
// ��v�o��


//------------------------------------------
// ����o�͐M���̃n�[�h�E�F�A�C�x���g�ݒ�
// WORD ChNo		�ݒ肷��`���l���ԍ� (0�I���W��)
// short EventType	�J�E���g��v�̃n�[�h�E�F�A�C�x���g�̎�� (CNT_OEVENT_GENOUT:�ėp�o�� CNT_OEVENT_MATCH0:�J�E���g��v���W�X�^0)
// short PulseWidth	�����V���b�g�p���X�̃p���X���̌W�� (0:10us  1:100us  2:1ms  3:10ms  4:100ms)
// �߂�l ���A���
//------------------------------------------
DWORD ContecCnt8mManager::OutputHardwareEvent(WORD ChNo, short EventType, short PulseWidth)
{
	// ����o�͂Ƀp���X����
	long nRet = 0;
	if( ! m_bSmyu) {	
#ifndef NO_HW
		_ASSERT(CONTEC_CNT_NOID != m_hDrv);
	#ifndef DEFECTLIB_CONTEC_CNT_WMD		// �]����
		nRet = CntSetOutputSignalEvent(m_hDrv, ChNo, CNT_OEVENT_NEGATIVE, EventType, PulseWidth);
	#else									// WMD��
		nRet = CntSetOutputHardwareEvent(m_hDrv, ChNo, CNT_OEVENT_NEGATIVE, EventType, PulseWidth);
	#endif	
#endif
	}

	if( nRet != 0 ) {
		m_errCode	= nRet;	
		return CntSetOutputHardwareEventErr;
	}
	return 0;
}

//------------------------------------------
// �J�E���g��v�̃n�[�h�E�F�A�C�x���g�ݒ�
// WORD ChNo		�ݒ肷��`���l���ԍ� (0�I���W��)
// short RegisterNo	��r���W�X�^No
// DWORD �J�E���g��v�̃n�[�h�E�F�A�C�x���g�̎��
// �߂�l ���A���
//------------------------------------------
DWORD ContecCnt8mManager::CountMatchHardwareEvent(WORD ChNo, short RegisterNo, short eventType)
{
	// �J�E���g��v�̃n�[�h�E�F�A�C�x���g�ݒ� (�J�E���g�l�[���N���A )
	long nRet = 0;
	if( ! m_bSmyu) {	
#ifndef NO_HW
		_ASSERT(CONTEC_CNT_NOID != m_hDrv);
	#ifndef DEFECTLIB_CONTEC_CNT_WMD		// �]����
		nRet = CntSetCountUpEvent(m_hDrv, ChNo, RegisterNo, eventType, 0);
	#else									// WMD��
		nRet = CntSetCountMatchHardwareEvent(m_hDrv, ChNo, RegisterNo, eventType, 0);
	#endif	
#endif
	}

	if( nRet != 0 ) {
		m_errCode	= nRet;	
		return CntSetCountMatchHardwareEventErr;
	}
	return 0;
}

//------------------------------------------
// �J�E���g�l���[���N���A���܂��B
// int nChIndex		�ݒ肷��`���l���ԍ� (0�I���W��)
// int ChNum		�`�����l����
// �߂�l ���A���
//------------------------------------------
DWORD ContecCnt8mManager::ResetChannel(int nChIndex, int ChNum)
{
	// �J�E���g��v�̃n�[�h�E�F�A�C�x���g�ݒ� (�J�E���g�l�[���N���A )
	long nRet = 0;
	if( ! m_bSmyu) {	
#ifndef NO_HW
		_ASSERT(CONTEC_CNT_NOID != m_hDrv);
	#ifndef DEFECTLIB_CONTEC_CNT_WMD		// �]����
		nRet = CntResetChannel(m_hDrv, &m_nChannel[ nChIndex ], ChNum);
	#else									// WMD��
		nRet = CntZeroClearCount(m_hDrv, &m_nChannel[ nChIndex ], ChNum);
	#endif	
#endif
	}

	if( nRet != 0 ) {
		m_errCode	= nRet;	
		return CntResetChannelErr;
	}
	return 0;
}

//------------------------------------------
// �J�E���g��v�ɂ��ʒm�̎w��ݒ�
// WORD ChNo		�ݒ肷��`���l���ԍ� (0�I���W��)
// short RegNo ��r���W�X�^�̃��W�X�^�ԍ� (��{�I��0��OK)
// DWORD Count ��r���W�X�^�ɐݒ肷���r�l
// �߂�l ���A���
//------------------------------------------
DWORD ContecCnt8mManager::NotifyCountUp(WORD ChNo, short RegNo, long plsCnt)
{
	// �J�E���g��v�̃n�[�h�E�F�A�C�x���g�ݒ� (�J�E���g�l�[���N���A )
	long nRet = 0;
	if( ! m_bSmyu) {	
#ifndef NO_HW
		_ASSERT(CONTEC_CNT_NOID != m_hDrv);
	#ifndef DEFECTLIB_CONTEC_CNT_WMD		// �]����
		nRet = CntNotifyCountUp(m_hDrv, ChNo, RegNo, plsCnt, 0, 0);
	#else									// WMD��
		nRet = CntNotifyCountUp(m_hDrv, ChNo, RegNo, plsCnt, 0);
	#endif	
#endif
	}

	if( nRet != 0 ) {
		m_errCode	= nRet;	
		return CntNotifyCountUpErr;
	}
	return 0;
}

//------------------------------------------
// �w����T���v�����O�����̒ʒm�ݒ�
// long Count	�ʒm����T���v�����O�l
// �߂�l ���A���
//------------------------------------------
DWORD ContecCnt8mManager::NotifySamplingCount(long Count)
{
	// �J�E���g��v�̃n�[�h�E�F�A�C�x���g�ݒ� (�J�E���g�l�[���N���A )
	long nRet = 0;
	if( ! m_bSmyu) {	
#ifndef NO_HW
		_ASSERT(CONTEC_CNT_NOID != m_hDrv);
	#ifndef DEFECTLIB_CONTEC_CNT_WMD		// �]����
		nRet = CntNotifySamplingCount(m_hDrv, Count, 0, 0);
	#else									// WMD��
		nRet = CntNotifySamplingCount(m_hDrv, Count, 0);
	#endif	
#endif
	}

	if( nRet != 0 ) {
		m_errCode	= nRet;	
		return CntNotifySamplingCountErr;
	}
	return 0;
}

//------------------------------------------
// CntNotifyCountUp�֐��Őݒ肵���J�E���g��v�ɂ��C�x���g�����������ꍇ�ɌĂяo�����R�[���o�b�N�֐���o�^
// void* CallBackProc �R�[���o�b�N�֐��̃A�h���X���w�肵�܂��B
// void* param �R�[���o�b�N�ɓn���p�����[�^�̃A�h���X���w�肵�܂��B�p�����[�^���K�v�Ȃ��ꍇ�́ANULL���w�肵�Ă��������B
// �߂�l ���A���
//------------------------------------------
DWORD ContecCnt8mManager::CountUpCallbackProc(void* CallBackProc, void* param)
{
	long nRet = 0;
	if( ! m_bSmyu) {	
#ifndef NO_HW
		_ASSERT(CONTEC_CNT_NOID != m_hDrv);
	#ifndef DEFECTLIB_CONTEC_CNT_WMD		// �]����
		// ����
	#else									// WMD��
		nRet = CntCountUpCallbackProc(m_hDrv, CallBackProc, param);
	#endif	
#endif
	}

	if( nRet != 0 ) {
		m_errCode	= nRet;	
		return CntCountUpCallbackProcErr;
	}
	return 0;
}

DWORD ContecCnt8mManager::SamplingCountCallbackProc(void* CallBackProc, void* param)
{
	long nRet = 0;
	if( ! m_bSmyu) {	
#ifndef NO_HW
		_ASSERT(CONTEC_CNT_NOID != m_hDrv);
	#ifndef DEFECTLIB_CONTEC_CNT_WMD		// �]����
		// ����
	#else									// WMD��
		nRet = CntSamplingCountCallbackProc(m_hDrv, CallBackProc, param);
	#endif	
#endif
	}

	if( nRet != 0 ) {
		m_errCode	= nRet;	
		return CntSamplingCountCallbackProcErr;
	}
	return 0;
}

//////////////////////////////////////////////////////////////////////
// �V�~�����[�V����

//------------------------------------------
// �V�~�����[�^���[�h�̐ݒ�
// bool bMode True:�V�~�����[�V������ false:�ʏ�
//------------------------------------------
bool ContecCnt8mManager::SetSmyuMode(bool bMode) 
{
	bool bRetc=true;
	//// �V�~�����[�V������ 
	if(bMode) {
		mem_connect(&m_hTblSmyu, (LPVOID *)&mtbl_pSmyu, m_sSmyuName);
		if(m_hTblSmyu == NULL) {
			m_hTblSmyu = NULL;
			mtbl_pSmyu = NULL; 
			bMode = false;
			bRetc = false;
			goto ending;
		}

	} else {
		if(NULL != mtbl_pSmyu) {
			mem_close(&m_hTblSmyu, (LPVOID *)&mtbl_pSmyu);
		}
		m_hTblSmyu = NULL;
		mtbl_pSmyu = NULL; 
	}

	//// �t���O�̕ύX
ending:
	m_bSmyu = bMode;
	return bRetc;
}
//------------------------------------------
// �T���v�����O�o�b�t�@�̏�����
//------------------------------------------
void ContecCnt8mManager::SmyuCntResetSampling()
{
	memset(mtbl_pSmyu, 0x00, sizeof(SMYU_TBL_PLG));
}
//------------------------------------------
// �ŐV�̃T���v�����O�J�E���g�l �擾
// DWORD* sampCnt �ŐV�̃T���v�����O�J�E���g�l
//------------------------------------------
DWORD ContecCnt8mManager::SmyuGetSamplingCount(DWORD* sampCnt) 
{
	*sampCnt = mtbl_pSmyu->nSamplingCount;
	return 0;
}

//------------------------------------------
// �ŐV�̃J�E���g�l �擾
// WORD ChNum   �ǂݍ��݂����`�����l����
// short* ChNo   �ǂݍ��݂����`�����l���ԍ��̔z�� (0�I���W��)
// DWORD* count �ǂݍ��񂾃J�E���g�l�̔z��
//------------------------------------------
DWORD ContecCnt8mManager::SmyuGetReadCount(WORD ChNum, short* ChNo, DWORD* count) 
{
	for(int ii=0; ii<ChNum; ii++) {
		count[ii] = mtbl_pSmyu->nCount[ ChNo[ii] ];
	}

	return 0;
}



#endif


