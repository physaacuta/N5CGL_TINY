#include "StdAfx.h"
#include "PlgControlManager.h"

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
PlgControlManager::PlgControlManager(void) :
ThreadManager("PlgCntMgr")
{
	//// INI�t�@�C���ǂݍ���
	GetIniParam();

	//// ���L����������
	m_hTblSmyu = CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, sizeof(SMYU_TBL_PLG), m_sSmyuName);
	mtbl_pSmyu =(SMYU_TBL_PLG*) MapViewOfFile(m_hTblSmyu, FILE_MAP_ALL_ACCESS, 0, 0, 0);
	if(m_hTblSmyu == NULL) {
		_ASSERT(FALSE);
	}
	memset(mtbl_pSmyu, 0x00, sizeof(SMYU_TBL_PLG));

#ifdef SEQ_PLG
	m_hTblSeq = CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, sizeof(TO_SEQMEM_TBL), m_sSeqName);
	mtbl_pSeq = (TO_SEQMEM_TBL*)MapViewOfFile(m_hTblSeq, FILE_MAP_ALL_ACCESS, 0, 0, 0);
	if (m_hTblSeq == NULL) {
		_ASSERT(FALSE);
	}
	SetSeqMem_Clear(mtbl_pSeq);
#endif


	//// �n���h������
	m_evTimerFrame = CreateWaitableTimer(NULL, FALSE, NULL);
	m_evTimerCnt = CreateWaitableTimer(NULL, FALSE, NULL);
	for(int ii=0; ii<SMYU_PLG_MAX_CHANNEL; ii++) {
		m_evTimerSpeed[ii] = CreateWaitableTimer(NULL, FALSE, NULL);
	}
	m_evTimerHelcy = CreateWaitableTimer(NULL, FALSE, NULL);

#ifdef SEQ_PLG
	// �����l���
	m_dKenPlgDiff = 0;
	m_dKenPlgAdd = 0;
	m_dKenExsync = 0;
	m_dDspPlgAdd = 0;

	SetUdpSocket();

	SEQ_DM_RECV_BASE* typWk = (SEQ_DM_RECV_BASE*)&mtbl_pSeq->fb_dm[0];
	typWk->nPls = (WORD)(m_Setting.dPlsY[m_nIdPlgKen] * 10000.0);
	typWk->nPlsSub = (WORD)(m_Setting.dPlsY[m_nIdPlgSub] * 10000.0);
	typWk->nPlsDsp = (WORD)(m_Setting.dPlsY[m_nIdPlgDsp] * 10000.0);
#endif
}

//------------------------------------------
// �f�X�g���N�^
//------------------------------------------
PlgControlManager::~PlgControlManager(void)
{
	//// ���L�������J��
	UnmapViewOfFile(mtbl_pSmyu);
#ifndef SEQ_PLG
	CloseHandle(m_hTblSmyu);
#endif

#ifdef SEQ_PLG
	UnmapViewOfFile(mtbl_pSeq);
	CloseHandle(m_hTblSeq);
#endif


	//// �n���h�����
	CloseHandle(m_evTimerFrame);
	CloseHandle(m_evTimerCnt);
	for(int ii=0; ii<SMYU_PLG_MAX_CHANNEL; ii++) {
		CloseHandle(m_evTimerSpeed[ii]);
	}
	CloseHandle(m_evTimerHelcy);

#ifdef SEQ_PLG
	ReSetUdpSocket();
#endif
}


//------------------------------------------
// �X���b�h�J�n�O���� (�X���b�h���s����(�V�O�i���Z�b�g�O)�ɃR�[���o�b�N)
//------------------------------------------
int PlgControlManager::ThreadFirst()
{
	//// ����������J�n
	int ii_s = ( 1==m_Setting.nSamplingEnable, 2, 0 );		// �ʏ���r�����́A0,1ch�͌Œ�̂���
	for(int ii=ii_s; ii<SMYU_PLG_MAX_CHANNEL; ii++) {
		SetTimer_SpeedCycle(ii, true);
	}

	//// �C�x���g�ݒ�
	//HANDLE hArray[] = { m_evTimerFrame,
	//					m_evTimerSpeed[]
	//					};
	//SetEventNum(sizeof(hArray)/sizeof(HANDLE), hArray);

	AddEventNum(1, &m_evTimerFrame);
#ifdef SEQ_PLG
	__int64 ts;
	ts= TIME_CNT_INTERVAL * -10000;
	SetWaitableTimer(m_evTimerCnt, (LARGE_INTEGER *)&ts, TIME_CNT_INTERVAL, NULL, NULL, FALSE);

	ts = TIME_HELCY_INTERVAL * -10000;
	SetWaitableTimer(m_evTimerHelcy, (LARGE_INTEGER *)&ts, TIME_CNT_INTERVAL, NULL, NULL, FALSE);
#endif
	AddEventNum(1, &m_evTimerCnt);
	AddEventNum(1, &m_evTimerHelcy);
	AddEventNum(SMYU_PLG_MAX_CHANNEL, m_evTimerSpeed);
	return 0;
}

//------------------------------------------
// �X���b�h�I���O���� (�I���V�O�i����ɃR�[���o�b�N)
//------------------------------------------
int PlgControlManager::ThreadLast()
{
	// �������~
//	SetTimer_FrameCycle(false);
	for(int ii=0; ii<SMYU_PLG_MAX_CHANNEL; ii++) {
		SetTimer_SpeedCycle(ii, false);
	}
	return 0;
}

//------------------------------------------
// �X���b�h�C�x���g����
// int nEventNo �C�x���gNo (0�I���W��)
//------------------------------------------
void PlgControlManager::ThreadEvent(int nEventNo)
{
	////// �V�O�i���̏���
	enum {	EV_TIMER_FRAME = 0,				// �t���[���o�͒�������s�C�x���g
			EV_TIMER_CNT,					// ���C�u�J�E���^
			EV_TIMER_HELCY,					// �w���V�[���M
			EV_TIMER_SPEED					// �ʔ����֌W
	};

	////// �V�O�i����������
//-----------------------------------------------------------------------------------------------
	// �t���[���o�͒�������s�C�x���g
	if (EV_TIMER_FRAME == nEventNo) {
		SetMem_Sampling();
//-----------------------------------------------------------------------------------------------
	// ���C�u�J�E���^�C�x���g
	} else if (EV_TIMER_CNT == nEventNo) {
		SetMem_Cnt();
//-----------------------------------------------------------------------------------------------
	// �w���V�[���M
	} else if (EV_TIMER_HELCY == nEventNo) {
		SendHelcy();

//-----------------------------------------------------------------------------------------------
	// �ʔ����֌W
	} else {
		SetMem_Speed(nEventNo - EV_TIMER_SPEED);
	}
}

// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ���L������ ����

//------------------------------------------
// �T���v�����O�N���b�N�p
//------------------------------------------
void PlgControlManager::SetMem_Sampling()
{
	//// ����̃T���v�����O�ʒu���擾
	DWORD nCnt   = (m_Setting.nFrameSizeY/SMYU_PLG_BUNSYU);

	DWORD nSamp	 = mtbl_pSmyu->nSamplingCount;				// ����̐擪
	DWORD nSampE = nSamp + nCnt;	

	DWORD nIndex = nSamp % (SMYU_PLG_MAX_SAMPBUFNUM*m_Setting.nFrameSizeY);
	DWORD nIndexE= nIndex + nCnt;							// ����̍Ō�
	long nPre = nIndex -1;									// �O��̍Ō�
	if( 0 > nPre ) nPre = SMYU_PLG_MAX_SAMPBUFNUM*m_Setting.nFrameSizeY - 1;

	//// �x�[�X�l������
	// �O��̍Ō�̒l
	DWORD nPlgCnt  = mtbl_pSmyu->nSampBuf[nPre][0];
	DWORD nTimeCnt = mtbl_pSmyu->nSampBuf[nPre][1];

//	LOG(em_MSG), "[%s] �����ݸ� %d��%d ch0=%d, ch1=%d", my_sThreadName, nSamp, nSampE , nPlgCnt, nTimeCnt);


	// 10nsec�e�X�g�p���X�́A
	//	1pixel�ӂ�̎��ԃJ�E���g���Z�o
	//		[mm/sec]    = [mpm] / 60 * 1000.0
	//		[sec/pixel] = [mm/pixel] / [mm/sec]
	//		[sec] �� 10nsec �P�ʂɕϊ�
	//	���ꂪ�A1���C���ӂ�̃e�X�g�p���X���ƂȂ�͂�	
	double dWk = m_SaveData.nSpeed[0] / 60.0 * 1000.0;	// [mm/sec] 
	dWk = m_Setting.dResY[0] / dWk;						// [sec/pixel] 
	DWORD nTimeAdd = (DWORD)(dWk * 100000);

	//// �T���v�����O�o�b�t�@�Ƀf�[�^�Z�b�g
	for(DWORD ii=nIndex; ii<nIndexE; ii++) {
		nPlgCnt++;							// PLG�l�́A�O���1���Z�ł悢
		nTimeCnt += nTimeAdd;				// ���ԃJ�E���g�l�́A1���C���ӂ��10nsec�����Z�b�g
		mtbl_pSmyu->nSampBuf[ii][0] = nPlgCnt;	
		mtbl_pSmyu->nSampBuf[ii][1] = nTimeCnt;
	}

	//// �S���Z�b�g���I��������ƂɁA�ŐV���R�[�h�ʒu���X�V
	mtbl_pSmyu->nCount[0] = nPlgCnt;
	mtbl_pSmyu->nCount[1] = nTimeCnt;
	mtbl_pSmyu->nSamplingCount = nSampE;

	mtbl_pSmyu->dLen[0] += (nCnt * m_Setting.dResY[0]);

//	LOG(em_MSG), "[%s] ch0=%d, ch1=%d", my_sThreadName, nPlgCnt, nTimeCnt );

#ifdef SEQ_PLG
	long tm = m_SaveData.nFrameCycle / SMYU_PLG_BUNSYU;
	dWk = (tm / 1000.0) / dWk;				// 30ms�����̂���
	SEQ_DM_RECV_BASE* typWk = (SEQ_DM_RECV_BASE*)&mtbl_pSeq->fb_dm[0];
	m_dKenPlgDiff += dWk * m_Setting.dResY[0] / m_Setting.dPlsY[0];
	typWk->ken.nPlgDiff = (DWORD)m_dKenPlgDiff;				// �����p PLG ����
	typWk->ken.nTimeDiff += tm * 100;						// �����p ���� ���� [10��s]
	m_dKenExsync += dWk;
	typWk->ken.nLvalAdd = (DWORD)m_dKenExsync;				// �����p ExSync�@�ώZ�l
	m_dKenPlgAdd += dWk* m_Setting.dResY[0] / m_Setting.dPlsY[0];
	typWk->ken.nPlgAdd = (DWORD)m_dKenPlgAdd;				// �����p PLG �ώZ
	typWk->ken.nTimeAdd += tm * 100;						// �����p ���� �ώZ [10��s]
															// �t���[����1������
	typWk->nLength += (DWORD)(nCnt * m_Setting.dResY[0]);

	if (m_Setting.nFrameSizeY <= typWk->ken.nPlgDiff* m_Setting.dPlsY[0] / m_Setting.dResY[0]) {
		SendUpd();
		typWk->ken.nFrameNo += 1;
		m_dKenPlgDiff -= m_Setting.nFrameSizeY * m_Setting.dResY[0] / m_Setting.dPlsY[0];
		typWk->ken.nPlgDiff = (DWORD)m_dKenPlgDiff;
		typWk->ken.nTimeDiff = 0;
	}
	//m_dDspPlgAdd += dWk;
	//typWk->dsp.nPlgAdd = (DWORD)m_dDspPlgAdd;				// �����p PLG �ώZ
#endif
}

//------------------------------------------
// �T���v�����O�N���b�N�p
//------------------------------------------
void PlgControlManager::SetMem_Cnt()
{
#ifdef SEQ_PLG
	SEQ_DM_RECV_BASE* typWk = (SEQ_DM_RECV_BASE*)&mtbl_pSeq->fb_dm[0];
	typWk->nHBCnt += 1;
	if(9999 < typWk->nHBCnt)typWk->nHBCnt = 0;
#endif
}

//------------------------------------------
// �ʔ����p
// int nIndex �z��ʒu (0�I���W��)
//------------------------------------------
void PlgControlManager::SetMem_Speed(int nIndex)
{
	//		[mm/sec]    = [mpm] / 60 * 1000.0
	//		[pixel/sec] = [mm/sec] / [mm/pixel]
	double dWk = m_SaveData.nSpeed[nIndex] / 60.0 * 1000.0;	// [mm/sec] 
	dWk = dWk / m_Setting.dResY[nIndex];					// [pixel/sec] 
	dWk = dWk / (1000.0/TIME_SPEED_INTERVAL);				// 30ms�����̂���

	mtbl_pSmyu->nCount[nIndex] += (DWORD)dWk;
	mtbl_pSmyu->dLen[nIndex]   +=  (DWORD)dWk * m_Setting.dResY[nIndex];

#ifdef SEQ_PLG
	SEQ_DM_RECV_BASE* typWk = (SEQ_DM_RECV_BASE*)&mtbl_pSeq->fb_dm[0];
	if (nIndex == m_nIdPlgKen) {
		typWk->nLength += (DWORD)(dWk * m_Setting.dResY[nIndex]);
		m_dKenPlgDiff += dWk* m_Setting.dResY[0] / m_Setting.dPlsY[0];
		typWk->ken.nPlgDiff = (DWORD)m_dKenPlgDiff;			// �����p PLG ����
		typWk->ken.nTimeDiff += TIME_SPEED_INTERVAL * 100;		// �����p ���� ���� [10��s]
		m_dKenExsync += dWk;
		typWk->ken.nLvalAdd = (DWORD)m_dKenExsync;				// �����p ExSync�@�ώZ�l
		m_dKenPlgAdd += dWk* m_Setting.dResY[0] / m_Setting.dPlsY[0];
		typWk->ken.nPlgAdd = (DWORD)m_dKenPlgAdd;				// �����p PLG �ώZ
		typWk->ken.nTimeAdd += TIME_SPEED_INTERVAL * 100;		// �����p ���� �ώZ [10��s]
																// �t���[����1������

		if (m_Setting.nFrameSizeY <= typWk->ken.nPlgDiff* m_Setting.dPlsY[0] / m_Setting.dResY[0]) {
			SendUpd();
			typWk->ken.nFrameNo += 1;
			m_dKenPlgDiff -= m_Setting.nFrameSizeY * m_Setting.dResY[0] / m_Setting.dPlsY[0];
			typWk->ken.nPlgDiff = (DWORD)m_dKenPlgDiff;
			typWk->ken.nTimeDiff = 0;
		}
	}
	if (nIndex == m_nIdPlgDsp) {
		typWk->nLengthDsp += (DWORD)(dWk * m_Setting.dResY[nIndex]);
		m_dDspPlgAdd += dWk* m_Setting.dResY[0] / m_Setting.dPlsY[0];
		typWk->dsp.nPlgAdd = (DWORD)m_dDspPlgAdd;				// �����p PLG �ώZ
	}
	if (nIndex == m_nIdPlgSub) {
		typWk->nLengthSub += (DWORD)(dWk * m_Setting.dResY[nIndex]);
	}
#endif
}

//------------------------------------------
// �V�[�P���T���L�������N���A
// int nIndex �z��ʒu (0�I���W��)
//------------------------------------------
void PlgControlManager::SetSeqMem_Clear(TO_SEQMEM_TBL* tbl)
{
	SEQ_DM_RECV_BASE* typWk = (SEQ_DM_RECV_BASE*)&tbl->fb_dm[0];
	memset(&typWk->ken, 0x00, sizeof(typWk->ken));
	memset(&typWk->dsp, 0x00, sizeof(typWk->dsp));
	typWk->nLength = 0;
	typWk->nLengthSub = 0;
	typWk->nLengthDsp = 0;
	typWk->nWpdOnLength = 0;
	typWk->nWpdOnLengthSub = 0;
	typWk->nWpdOnLengthDsp = 0;
}

// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ����

//------------------------------------------
// �V�~�����[�^�J�n
//------------------------------------------
void PlgControlManager::SmyuStart()
{
	//// ������
	memset(mtbl_pSmyu, 0x00, sizeof(SMYU_TBL_PLG));
}
/*
//------------------------------------------
// �t���[���o�͒�����^�C�}�[ ����
// bool bMode true:�J�n false:��~
//------------------------------------------
void PlgControlManager::SetTimer_FrameCycle(bool bMode)
{
	//// ���O�`�F�b�N
	if( 0 >= m_SaveData.nFrameCycle ) bMode=false;

	//// ����
	if(bMode) {
		//// ���O����
		__int64 ts = (m_SaveData.nFrameCycle /SMYU_PLG_BUNSYU) * -10000; 
		SetWaitableTimer(m_evTimerFrame, (LARGE_INTEGER *)&ts, m_SaveData.nFrameCycle/SMYU_PLG_BUNSYU, NULL, NULL, FALSE);

	} else {
		CancelWaitableTimer(m_evTimerFrame);

	}
}
*/
//------------------------------------------
// ���C�����x�o�͒�����^�C�}�[ ����
// int nIndex �z��ʒu (0�I���W��)
// bool bMode true:�J�n false:��~
//------------------------------------------
void PlgControlManager::SetTimer_SpeedCycle(int nIndex, bool bMode)
{
	if(1==m_Setting.nSamplingEnable && 0 == nIndex) {
		//======================================================
		//// �ʏ���r����
		if( 0 >= m_SaveData.nFrameCycle ) bMode=false;

		//// ����
		if(bMode) {
			//// ���O����
			__int64 ts = (m_SaveData.nFrameCycle /SMYU_PLG_BUNSYU) * -10000; 
			SetWaitableTimer(m_evTimerFrame, (LARGE_INTEGER *)&ts, m_SaveData.nFrameCycle/SMYU_PLG_BUNSYU, NULL, NULL, FALSE);
		} else {
			CancelWaitableTimer(m_evTimerFrame);
		}

	} else {
		//======================================================
		//// ���̑�
		if( 0 >= m_SaveData.nSpeed[nIndex] ) bMode=false;

		//// ����
		if(bMode) {
			//// ���O����
			__int64 ts = TIME_SPEED_INTERVAL * -10000; 	// 1�ʕb��100�i�m�b (�}�C�i�X�w��ŁA�w�莞�Ԍo�߂ŃV�O�i��) (��3������2��ڈȍ~�̋N���Ԋu)
			SetWaitableTimer(m_evTimerSpeed[nIndex], (LARGE_INTEGER *)&ts, TIME_SPEED_INTERVAL, NULL, NULL, FALSE);

		} else {
			CancelWaitableTimer(m_evTimerSpeed[nIndex]);
		}
	}


	//// ��ʂɌ��ʔ��f
	int chkSpeedSelect[SMYU_PLG_MAX_CHANNEL]= { IDC_PLG_SPEED_S0,IDC_PLG_SPEED_S1,IDC_PLG_SPEED_S2,IDC_PLG_SPEED_S3,	IDC_PLG_SPEED_S4,IDC_PLG_SPEED_S5,IDC_PLG_SPEED_S6,IDC_PLG_SPEED_S7 };
	AfxGetApp()->GetMainWnd()->CheckDlgButton(chkSpeedSelect[nIndex], bMode);//(bMode?1:0)); 
}

//------------------------------------------
// �����pWPD�M��ON
//------------------------------------------
void PlgControlManager::Set_WpdKen(void)
{
#ifdef SEQ_PLG
	SEQ_DM_RECV_BASE* typWk = (SEQ_DM_RECV_BASE*)&mtbl_pSeq->fb_dm[0];
	typWk->ken.nWpdOnFrameNo = typWk->ken.nFrameNo;
	typWk->ken.nWpdOnLvalAdd = typWk->ken.nLvalAdd;
	typWk->ken.nWpdOnYCnt = (DWORD)(typWk->ken.nPlgDiff* m_Setting.dPlsY[0] / m_Setting.dResY[0]);

	typWk->nWpdOnLength = typWk->nLength;
	typWk->nLength = 0;
#endif
}

//------------------------------------------
// ������r�pWPD�M��ON
//------------------------------------------
void PlgControlManager::Set_WpdSub(void)
{
#ifdef SEQ_PLG
	SEQ_DM_RECV_BASE* typWk = (SEQ_DM_RECV_BASE*)&mtbl_pSeq->fb_dm[0];
	typWk->nWpdOnLengthSub = typWk->nLengthSub;
	typWk->nLengthSub = 0;
#endif
}

//------------------------------------------
// �����\���pWPD�M��ON
//------------------------------------------
void PlgControlManager::Set_WpdDsp(void)
{
#ifdef SEQ_PLG
	SEQ_DM_RECV_BASE* typWk = (SEQ_DM_RECV_BASE*)&mtbl_pSeq->fb_dm[0];
	typWk->dsp.nWpdOnPlgAdd = typWk->dsp.nPlgAdd;

	typWk->nWpdOnLengthDsp = typWk->nLengthDsp;
	typWk->nLengthDsp = 0;
#endif
}

//------------------------------------------
// ���Z�b�g�M��ON
//------------------------------------------
void PlgControlManager::Set_ResetSignal(void)
{
#ifdef SEQ_PLG
	SEQ_DM_RECV_BASE* typWk = (SEQ_DM_RECV_BASE*)&mtbl_pSeq->fb_dm[0];

	memset(&typWk->ken, 0x00, sizeof(typWk->ken));
	memset(&typWk->dsp, 0x00, sizeof(typWk->dsp));

	typWk->nWpdOnLength = 0;
	typWk->nWpdOnLengthSub = 0;
	typWk->nWpdOnLengthDsp = 0;

	typWk->nLength = 0;
	typWk->nLengthSub = 0;
	typWk->nLengthDsp = 0;
	typWk->nWpdOnLength = 0;
	typWk->nWpdOnLengthSub = 0;
	typWk->nWpdOnLengthDsp = 0;

	m_dKenPlgDiff = 0;
	m_dKenPlgAdd = 0;
	m_dKenExsync = 0;
	m_dDspPlgAdd = 0;
#endif
}

//------------------------------------------
// Ini�t�@�C���擾
//------------------------------------------
void PlgControlManager::GetIniParam()
{
	CString	sWk;
	int		nWk;
	char	cWk[1024];

	//// ODBC�ڑ��N���X �C���X�^���X����
	//OdbcBase	clsDb;
	//if( ! clsDb.Connect(TASKINI_NAME)) {
	//	_ASSERT(FALSE);
	//	return;
	//}

	memset(&m_Setting, 0x00, sizeof(m_Setting));
	memset(&m_SaveData,0x00, sizeof(m_SaveData));


	//================================================
	// ��{�f�[�^
	GetPrivateProfileString(GOT_CONTROL, "TBL_NAME_PLG", "", cWk, sizeof(cWk), TASKINI_NAME);	m_sSmyuName = cWk;
	GetPrivateProfileString(GOT_CONTROL, "TBL_NAME_SEQ", TBL_TO_SEQMEM_NAME, cWk, sizeof(cWk), TASKINI_NAME);	m_sSeqName = cWk;
	m_nIdPlgKen = GetPrivateProfileInt(GOT_CONTROL, "INDEX_PLG_KEN", 0, TASKINI_NAME);
	m_nIdPlgSub = GetPrivateProfileInt(GOT_CONTROL, "INDEX_PLG_SUB", 1, TASKINI_NAME);
	m_nIdPlgDsp = GetPrivateProfileInt(GOT_CONTROL, "INDEX_PLG_DSP", 2, TASKINI_NAME);
	//================================================
	// �ݒ�f�[�^

	// �T���v�����O�L�� �L��
	m_Setting.nSamplingEnable = GetPrivateProfileInt(GOT_CONTROL, "SAMPLING_ENABLE", 0, TASKINI_NAME);
	
	
	//if(1==m_Setting.nSamplingEnable) {
		// �c�t���[���T�C�Y
		GetPrivateProfileString(GOT_CONTROL, "FRAMESIZE_Y_SQL", "", cWk, sizeof(cWk), TASKINI_NAME);
		if(0!=strlen(cWk)) {
			if( ! OdbcBase::GetSelectKey_Static(cWk, &m_Setting.nFrameSizeY, TASKINI_NAME)) {
				_ASSERT(FALSE);
			}
		} else {
			m_Setting.nFrameSizeY = GetPrivateProfileInt(GOT_CONTROL, "FRAMESIZE_Y_VAL", 0, TASKINI_NAME);
		}
	//}

	// �L��Ch��
	m_Setting.nChNum = GetPrivateProfileInt(GOT_CONTROL, "PLG_CH_NUM", 0, TASKINI_NAME);
	
	// ����\
	for(int ii=0; ii<m_Setting.nChNum; ii++) {
		sWk.Format("PLG_RES_SQL_%d", ii); 
		GetPrivateProfileString(GOT_CONTROL, sWk, "", cWk, sizeof(cWk), TASKINI_NAME);
		if(0!=strlen(cWk)) {
			if( ! OdbcBase::GetSelectKey_Static(cWk, &m_Setting.dResY[ii], TASKINI_NAME)) {
				_ASSERT(FALSE);
			}
		} else {
			sWk.Format("PLG_RES_VAL_%d", ii); 
			GetPrivateProfileString(GOT_CONTROL, sWk, "", cWk, sizeof(cWk), TASKINI_NAME);
			m_Setting.dResY[ii] = atof(cWk);
		}
	}

	// �p���X���[�g
	for(int ii=0; ii<m_Setting.nChNum; ii++) {
		sWk.Format("PLG_PLS_SQL_%d", ii); 
		GetPrivateProfileString(GOT_CONTROL, sWk, "", cWk, sizeof(cWk), TASKINI_NAME);
		if(0!=strlen(cWk)) {
			if( ! OdbcBase::GetSelectKey_Static(cWk, &m_Setting.dPlsY[ii], TASKINI_NAME)) {
				_ASSERT(FALSE);
			}
		} else {
			sWk.Format("PLG_PLS_VAL_%d", ii); 
			GetPrivateProfileString(GOT_CONTROL, sWk, "", cWk, sizeof(cWk), TASKINI_NAME);
			m_Setting.dPlsY[ii] = atof(cWk);
		}
	}

	//================================================
	// ��ʗp�f�[�^
	
	CWnd*   cwnd		= AfxGetApp()->GetMainWnd();	// �g���₷���悤�ɃG�C���A�X

	// �t���[���o�͎��� (�����l) [ms]
	if(1==m_Setting.nSamplingEnable) {
		nWk = GetPrivateProfileInt(GOT_CONTROL, "INI_FRAME_OUTPUT_INTERVAL", 0, TASKINI_NAME);
		//cwnd->SetDlgItemInt(IDC_PLG_FRAMECYCLE_D, nWk, TRUE);
		m_SaveData.nFrameCycle = nWk;
	} else {
		cwnd->GetDlgItem(IDC_PLG_FRAMECYCLE_D)->EnableWindow(false); 
		cwnd->GetDlgItem(IDC_PLG_FRAMECYCLE_W)->EnableWindow(false); 
		cwnd->GetDlgItem(IDC_PLG_FRAMECYCLE_C)->EnableWindow(false); 
		//cwnd->GetDlgItem(IDC_PLG_FRAMECYCLE_S)->EnableWindow(false); 
	}

	// ���C�����x
	int txtSpeedDsp[SMYU_PLG_MAX_CHANNEL]	= { IDC_PLG_SPEED_D0,IDC_PLG_SPEED_D1,IDC_PLG_SPEED_D2,IDC_PLG_SPEED_D3,	IDC_PLG_SPEED_D4,IDC_PLG_SPEED_D5,IDC_PLG_SPEED_D6,IDC_PLG_SPEED_D7 };
	int txtSpeedWrite[SMYU_PLG_MAX_CHANNEL]	= { IDC_PLG_SPEED_W0,IDC_PLG_SPEED_W1,IDC_PLG_SPEED_W2,IDC_PLG_SPEED_W3,	IDC_PLG_SPEED_W4,IDC_PLG_SPEED_W5,IDC_PLG_SPEED_W6,IDC_PLG_SPEED_W7 };
	int btnSpeedChange[SMYU_PLG_MAX_CHANNEL]= { IDC_PLG_SPEED_C0,IDC_PLG_SPEED_C1,IDC_PLG_SPEED_C2,IDC_PLG_SPEED_C3,	IDC_PLG_SPEED_C4,IDC_PLG_SPEED_C5,IDC_PLG_SPEED_C6,IDC_PLG_SPEED_C7 };
	int chkSpeedSelect[SMYU_PLG_MAX_CHANNEL]= { IDC_PLG_SPEED_S0,IDC_PLG_SPEED_S1,IDC_PLG_SPEED_S2,IDC_PLG_SPEED_S3,	IDC_PLG_SPEED_S4,IDC_PLG_SPEED_S5,IDC_PLG_SPEED_S6,IDC_PLG_SPEED_S7 };

	if(1==m_Setting.nSamplingEnable) {
		//cwnd->GetDlgItem(chkSpeedSelect[0])->EnableWindow(false); 

		cwnd->GetDlgItem(txtSpeedDsp[1])->EnableWindow(false); 
		cwnd->GetDlgItem(txtSpeedWrite[1])->EnableWindow(false); 
		cwnd->GetDlgItem(btnSpeedChange[1])->EnableWindow(false); 
		cwnd->GetDlgItem(chkSpeedSelect[1])->EnableWindow(false); 
	}

	for(int ii=0; ii<m_Setting.nChNum; ii++) {
		sWk.Format("INI_PLG_SPEED_%d", ii); 
		nWk = GetPrivateProfileInt(GOT_CONTROL, sWk, 0, TASKINI_NAME);
		//cwnd->SetDlgItemInt(txtSpeedDsp[ii], nWk, TRUE);
		m_SaveData.nSpeed[ii] = nWk;
	}
	for(int ii=m_Setting.nChNum; ii<SMYU_PLG_MAX_CHANNEL; ii++) {
		cwnd->GetDlgItem(txtSpeedDsp[ii])->EnableWindow(false); 
		cwnd->GetDlgItem(txtSpeedWrite[ii])->EnableWindow(false); 
		cwnd->GetDlgItem(btnSpeedChange[ii])->EnableWindow(false); 
		cwnd->GetDlgItem(chkSpeedSelect[ii])->EnableWindow(false); 
	}


	// �����ύX
	if(1==m_Setting.nSamplingEnable) {
		double dWk =  m_SaveData.nSpeed[0] / 60.0 * 1000.0;
		dWk  = dWk / (m_Setting.dResY[0]*m_Setting.nFrameSizeY);
		nWk = (int)(1.0 / dWk * 1000.0);
		m_SaveData.nFrameCycle = nWk;
	}

#ifdef SEQ_PLG
	GetPrivateProfileString(GOT_CONTROL, "UDP_IP", "192.9.200.255", cWk, sizeof(cWk), TASKINI_NAME);		m_SeqUdpIp = cWk;
	m_SeqUdpPort = GetPrivateProfileInt(GOT_CONTROL, "UDP_PORT", SEQ_UDP_PORT, TASKINI_NAME);
#endif

}

//------------------------------------------
// UDP SOCKET����
//------------------------------------------
BOOL PlgControlManager::SetUdpSocket()
{
	// ������
	m_SeqUdpsock = INVALID_SOCKET;
	memset(&m_SeqUdpaddr, 0x00, sizeof(m_SeqUdpaddr));

	// �\�P�b�g����
	WSADATA wsaData;
	WORD wVersionRequired = MAKEWORD(2, 2);
	WSAStartup(wVersionRequired, &wsaData);
	m_SeqUdpsock = socket(AF_INET, SOCK_DGRAM, 0);
	if (INVALID_SOCKET == m_SeqUdpsock) return FALSE;
	m_SeqUdpaddr.sin_family = AF_INET;
	m_SeqUdpaddr.sin_addr.s_addr = inet_addr(m_SeqUdpIp);
	m_SeqUdpaddr.sin_port = htons(m_SeqUdpPort);
	return TRUE;
}

//------------------------------------------
//// UDP SOCKET�j��
//------------------------------------------
void PlgControlManager::ReSetUdpSocket()
{
	if (INVALID_SOCKET != m_SeqUdpsock) closesocket(m_SeqUdpsock);
	m_SeqUdpsock = INVALID_SOCKET;
}

//------------------------------------------
// UDP���M
//------------------------------------------
void PlgControlManager::SendUpd(void)
{
#ifdef SEQ_PLG
	// ������^�C�}�[���Z�b�g
	CancelWaitableTimer(m_evTimerHelcy);

	SEQ_DM_RECV_BASE* typWk = (SEQ_DM_RECV_BASE*)&mtbl_pSeq->fb_dm[0];
	SEQ_TYPE_FRAME *p = new SEQ_TYPE_FRAME;
	memset(p, 0x00, sizeof(SEQ_TYPE_FRAME));
	p->head.nTC = SEQ_TC_FRAME;
	p->head.nSeqLen = sizeof(SEQ_TYPE_DATA_FRAME);
	p->data.nFrameNo = typWk->ken.nFrameNo;
	p->data.nPlgDiff = typWk->ken.nPlgDiff;
	p->data.nTimeDiff = typWk->ken.nTimeDiff;

	p->data.nExSyncAdd = typWk->ken.nLvalAdd;
	p->data.nPlgAdd = typWk->ken.nPlgAdd;
	p->data.nTimeAdd = typWk->ken.nTimeAdd;

	p->data.nWpdFramePos = typWk->ken.nWpdOnYCnt;
	sendto(m_SeqUdpsock, (char*)p, sizeof(SEQ_TYPE_DATA_FRAME), 0, (sockaddr*)&m_SeqUdpaddr, sizeof(sockaddr_in));
	delete p;

	// ������^�C�}�[�ăZ�b�g
	__int64 ts = TIME_HELCY_INTERVAL * -10000;
	SetWaitableTimer(m_evTimerHelcy, (LARGE_INTEGER *)&ts, TIME_CNT_INTERVAL, NULL, NULL, FALSE);
#endif

}
//------------------------------------------
// �w���V�[���M
//------------------------------------------
void PlgControlManager::SendHelcy(void)
{
#ifdef SEQ_PLG
	SEQ_DM_RECV_BASE* typWk = (SEQ_DM_RECV_BASE*)&mtbl_pSeq->fb_dm[0];
	SEQ_TYPE_HEAD *p = new SEQ_TYPE_HEAD;
	memset(p, 0x00, sizeof(SEQ_TYPE_HEAD));
	p->nTC = SEQ_TC_HELCY;
	p->nSeqLen = sizeof(SEQ_TYPE_HEAD);
	sendto(m_SeqUdpsock, (char*)p, sizeof(SEQ_TYPE_HEAD), 0, (sockaddr*)&m_SeqUdpaddr, sizeof(sockaddr_in));
	delete p;
#endif

}
//------------------------------------------
// �V�[�P���T���L�������N���A
//------------------------------------------
void PlgControlManager::Clear_SeqMem(void)
{
#ifdef SEQ_PLG
	SetSeqMem_Clear(mtbl_pSeq);
#endif
}
