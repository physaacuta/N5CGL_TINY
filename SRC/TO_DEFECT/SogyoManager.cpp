#include "StdAfx.h"
#include "SogyoManager.h"

// �������[���[�N���o ----->>>
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif
// <<<------------------------

//------------------------------------------
// �R���X�g���N�^
// LPCSTR cSession ini�t�@�C���Z�b�V����
//------------------------------------------
SogyoManager::SogyoManager(LPCSTR cSession) :
m_cSession(cSession),
mcls_pStatus(NULL),
ThreadManager("SogyolMgr")
{
	// ������
	m_pcNow = m_pcPre = 0;
	m_nNowSampCnt = m_nPreSampCnt = 0;
	m_nMpm = 0;
#ifdef DSP_SOGYO_CHECK
	m_pcNowDsp = m_pcPreDsp = 0;
	m_nNowSampCntDsp = m_nPreSampCntDsp = 0;
	m_nMpmDsp = 0;
#endif


	// ������`�F�b�N�^�C�}�[ ����
	m_evCheckTimer = CreateWaitableTimer(NULL, FALSE, NULL);

	// �N���e�B�J���Z�N�V����
	InitializeCriticalSection(&m_csLock);
}

//------------------------------------------
// �f�X�g���N�^
//------------------------------------------
SogyoManager::~SogyoManager(void)
{
	// ������`�F�b�N�^�C�}�[ �J��
	CancelWaitableTimer(m_evCheckTimer);
	CloseHandle(m_evCheckTimer);

	// �N���e�B�J���Z�N�V����
	DeleteCriticalSection(&m_csLock);
}


//------------------------------------------
// �X���b�h�J�n�O���� (�X���b�h���s����(�V�O�i���Z�b�g�O)�ɃR�[���o�b�N)
//------------------------------------------
int SogyoManager::ThreadFirst()
{
	//// �C�x���g�ݒ�
	HANDLE hArray[] = { m_evCheckTimer
						};
	SetEventNum(sizeof(hArray)/sizeof(HANDLE), hArray);
	return 0;
}

//------------------------------------------
// �X���b�h�I���O���� (�I���V�O�i����ɃR�[���o�b�N)
//------------------------------------------
int SogyoManager::ThreadLast()
{
	return 0;
}

//------------------------------------------
// �X���b�h�C�x���g����
// int nEventNo �C�x���gNo (0�I���W��)
//------------------------------------------
void SogyoManager::ThreadEvent(int nEventNo)
{
	////// �V�O�i���̏���
	enum {	EV_CHECK_TIMER = 0,				// ������Ď��^�C�}�[
	};

	////// �V�O�i����������
	switch (nEventNo) {

//-----------------------------------------------------------------------------------------------
	case EV_CHECK_TIMER:					// ������Ď��^�C�}�[
		// ������`�F�b�N
		CheckSampCntDiff();
#ifdef DSP_SOGYO_CHECK
		CheckSampCntDiffDsp();
#endif
		break;

//-----------------------------------------------------------------------------------------------
	default :
		// ���肦�Ȃ��I�I
		LOG(em_ERR), "[%s] ThreadEvent EventNo NG=%d", my_sThreadName, nEventNo );
		return;
	}

}

// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// �O������֐�

//------------------------------------------
// ���I ���Ɣ��������ǂݍ���
//   �� �������ɐݒ�ύX�Ŗ���T�����Ƃ߂�̂��C���������̂Ŏb��Ƃ��Ă��̂悤�ɂ���
//------------------------------------------
void SogyoManager::GetIniParam()
{
	SogyoSetting.InfUpToDown_Slow.nLimitSpeed = GetPrivateProfileInt(m_cSession, "UpToDown_SLOW_MPM", 0, TASKINI_NAME);
	SogyoSetting.InfUpToDown_Slow.nCheckCnt   = GetPrivateProfileInt(m_cSession, "UpToDown_SLOW_CNT", 0, TASKINI_NAME);
	SogyoSetting.InfUpToDown_Stop.nLimitSpeed = GetPrivateProfileInt(m_cSession, "UpToDown_STOP_MPM", 0, TASKINI_NAME);
	SogyoSetting.InfUpToDown_Stop.nCheckCnt   = GetPrivateProfileInt(m_cSession, "UpToDown_STOP_CNT", 0, TASKINI_NAME);
	SogyoSetting.InfDownToUp_Slow.nLimitSpeed = GetPrivateProfileInt(m_cSession, "DownToUp_SLOW_MPM", 0, TASKINI_NAME);
	SogyoSetting.InfDownToUp_Slow.nCheckCnt   = GetPrivateProfileInt(m_cSession, "DownToUp_SLOW_CNT", 0, TASKINI_NAME);
	SogyoSetting.InfDownToUp_Run.nLimitSpeed  = GetPrivateProfileInt(m_cSession, "DownToUp_RUN_MPM", 0, TASKINI_NAME);
	SogyoSetting.InfDownToUp_Run.nCheckCnt    = GetPrivateProfileInt(m_cSession, "DownToUp_RUN_CNT", 0, TASKINI_NAME);
	// ���̏������L���������`�F�b�N
	for(int ii=0; ii<4; ii++) {
		if(0 <= SogyoSetting.array[ii].nLimitSpeed && 0 < SogyoSetting.array[ii].nCheckCnt) SogyoSetting.array[ii].bUmu = true;
	}
#ifdef DSP_SOGYO_CHECK
	// �\���p�Z�b�g�i�Ƃ肠���������p�Ɠ���臒l�j
	SogyoSettingDsp.InfUpToDown_Slow.nLimitSpeed = GetPrivateProfileInt(m_cSession, "UpToDown_SLOW_MPM", 0, TASKINI_NAME);
	SogyoSettingDsp.InfUpToDown_Slow.nCheckCnt   = GetPrivateProfileInt(m_cSession, "UpToDown_SLOW_CNT", 0, TASKINI_NAME);
	SogyoSettingDsp.InfUpToDown_Stop.nLimitSpeed = GetPrivateProfileInt(m_cSession, "UpToDown_STOP_MPM", 0, TASKINI_NAME);
	SogyoSettingDsp.InfUpToDown_Stop.nCheckCnt   = GetPrivateProfileInt(m_cSession, "UpToDown_STOP_CNT", 0, TASKINI_NAME);
	SogyoSettingDsp.InfDownToUp_Slow.nLimitSpeed = GetPrivateProfileInt(m_cSession, "DownToUp_SLOW_MPM", 0, TASKINI_NAME);
	SogyoSettingDsp.InfDownToUp_Slow.nCheckCnt   = GetPrivateProfileInt(m_cSession, "DownToUp_SLOW_CNT", 0, TASKINI_NAME);
	SogyoSettingDsp.InfDownToUp_Run.nLimitSpeed  = GetPrivateProfileInt(m_cSession, "DownToUp_RUN_MPM", 0, TASKINI_NAME);
	SogyoSettingDsp.InfDownToUp_Run.nCheckCnt    = GetPrivateProfileInt(m_cSession, "DownToUp_RUN_CNT", 0, TASKINI_NAME);
	// ���̏������L���������`�F�b�N
	for(int ii=0; ii<4; ii++) {
		if(0 <= SogyoSettingDsp.array[ii].nLimitSpeed && 0 < SogyoSettingDsp.array[ii].nCheckCnt) SogyoSettingDsp.array[ii].bUmu = true;
	}
#endif
}

//------------------------------------------
// ������`�F�b�N�^�C�}�[ �J�n
// DWORD sampCnt ����l (-1�Ȃ�l��ς��Ȃ�)
//------------------------------------------
void SogyoManager::CheckStart(DWORD sampCnt)
{
	AutoLock autolock(&m_csLock);

	// ����
	QueryPerformanceFrequency((LARGE_INTEGER *)&m_pf);
	
	// ����l�Z�b�g
	if( -1 != sampCnt ) m_nPreSampCnt = sampCnt;
	QueryPerformanceCounter((LARGE_INTEGER *)&m_pcPre);

	// ini�t�@�C���擾
	memset(&SogyoSetting, 0x00, sizeof(SogyoSetting));

#ifdef DSP_SOGYO_CHECK
	// ����l�Z�b�g
	if (-1 != sampCnt) m_nPreSampCntDsp = sampCnt;
	QueryPerformanceCounter((LARGE_INTEGER *)&m_pcPreDsp);

	// ini�t�@�C���擾
	memset(&SogyoSettingDsp, 0x00, sizeof(SogyoSettingDsp));
#endif

	GetIniParam();										// ���I�p�����[�^��ini�t�@�C������Ǎ�
	int nCheckInterval = GetPrivateProfileInt(m_cSession, "CHECK_INTERVAL", 1000, TASKINI_NAME);

	// ����
	__int64 ts = (__int64)nCheckInterval * -10000; //0;	// 1�ʕb��100�i�m�b (�}�C�i�X�w��ŁA�w�莞�Ԍo�߂ŃV�O�i��) (��3������2��ڈȍ~�̋N���Ԋu)
	SetWaitableTimer(m_evCheckTimer,   (LARGE_INTEGER *)&ts, nCheckInterval, NULL, NULL, FALSE);
}

//------------------------------------------
// ������`�F�b�N�^�C�}�[ ��~
//------------------------------------------
void SogyoManager::CheckStop()
{
	AutoLock autolock(&m_csLock);

	// �^�C�}�[��~
	CancelWaitableTimer(m_evCheckTimer);

	// ���Ə�� �s��
	m_nMpm = 0;
	mcls_pStatus->SetSogyo( DIV_SOGYO_NON, m_nMpm );
	mcls_pStatus->SetLineSpeed(0);
#ifdef DSP_SOGYO_CHECK
	m_nMpmDsp = 0;
	mcls_pStatus->SetSogyoDsp(DIV_SOGYO_NON, m_nMpmDsp);
	mcls_pStatus->SetLineSpeedDsp(0);
#endif
}

//------------------------------------------
// ���݂̌����p�T���v�����O�N���b�N�l �擾
// DWORD sampCnt ���݂̃T���v�����O�N���b�N�l
// double dResV ���߂̏c����\ [mm/pixel]
//------------------------------------------
void SogyoManager::SetNowSampCnt(DWORD sampCnt, double dResV)
{
	AutoLock autolock(&m_csLock);

	m_nNowSampCnt = sampCnt;
	m_dResV = dResV;
	QueryPerformanceCounter((LARGE_INTEGER *)&m_pcNow);	// ���ݎ��Ԃ��擾
}


#ifdef DSP_SOGYO_CHECK
//------------------------------------------
// ���݂̕\���p�T���v�����O�N���b�N�l �擾
// DWORD sampCnt ���݂̃T���v�����O�N���b�N�l
// double dResV ���߂̏c����\ [mm/pixel]
//------------------------------------------
void SogyoManager::SetNowSampCntDsp(DWORD sampCnt, double dResV)
{
	AutoLock autolock(&m_csLock);

	m_nNowSampCntDsp = sampCnt;
	m_dResVDsp = dResV;
	QueryPerformanceCounter((LARGE_INTEGER *)&m_pcNowDsp);	// ���ݎ��Ԃ��擾
}
#endif

// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ����

//------------------------------------------
// �ʔ� ������`�F�b�N
//------------------------------------------
void SogyoManager::CheckSampCntDiff()
{
	//// �K�v�ȃ����o�[�����[�J���ɕێ�
	// ������StatusMgr�̃X�e�[�^�X�ύX�����DB Update�Ȃ̂ŁAPlgMgr�����b�N����Ă��܂��΍�
	////AutoLock autolock(&m_csLock);
	//EnterCriticalSection(&m_csLock);			// ---------------->>>
	//	double	pcDiff		= (double)(m_pcNow - m_pcPre) / m_pf;		// �Q�_�Ԃ̎���[s]
	//	long	sampCntDiff = m_nNowSampCnt - m_nPreSampCnt;			// �Q�_�Ԃ̃J�E���g

	//	double	dResV		= m_dResV;
	//	DWORD	nNowSampCnt = m_nNowSampCnt;
	//	__int64	pcNow		= m_pcNow;
	//LeaveCriticalSection(&m_csLock);			// <<<------------------
	//	
	AutoLock autolock(&m_csLock);			// ---------------->>>
		double	pcDiff		= (double)(m_pcNow - m_pcPre) / m_pf;		// �Q�_�Ԃ̎���[s]
		long	sampCntDiff = m_nNowSampCnt - m_nPreSampCnt;			// �Q�_�Ԃ̃J�E���g

		double	dResV		= m_dResV;
		DWORD	nNowSampCnt = m_nNowSampCnt;
		__int64	pcNow		= m_pcNow;
	autolock.UnLock();						// <<<------------------



	////////////////////////////////////
	// ���C�����x���Z�o
	////////////////////////////////////
	//// �O��l�Ƃ��ĕۑ�
	m_nPreSampCnt = nNowSampCnt;
	m_pcPre = pcNow;

	if(0 > sampCntDiff) {
		return;	// 1������ (�ʏ킠�肦�Ȃ�)
	}


	//// ����̃��C�����x
	//   ���܂萳�m�ł͖������A����Ȃɐ��x���K�v�Ȗ�ł͂Ȃ��̂ŁAOK�Ƃ���
	int nMpm;
	if( 0 == sampCntDiff ) {
		nMpm = 0;
	} else {
		nMpm = (int)(((sampCntDiff*dResV) / pcDiff) * 60.0 / 1000.0);	// [mm/s]��[mpm]
		if( 0 > nMpm || 3000 < nMpm ) {		// ��������ُ̈�
			LOG(em_ERR), "[%s] �������x=%d, samp=%d/%d, time=%lf", my_sThreadName, nMpm, m_nNowSampCnt, m_nPreSampCnt, pcDiff );
			return;
		}
	}
	//// �O��l�Ƃ��ĕۑ�
	m_nMpm = nMpm;

	////////////////////////////////////
	// ���Ə�� ����
	////////////////////////////////////
	// ���̂P�񂾂����������ɂǂ���
	EM_DIV_SOGYO	emNowSogyo   = mcls_pStatus->GetSogyo();	// ���݂̑��Ə��
	EM_DIV_SOGYO	emTotalSogyo = DIV_SOGYO_NON;				// �S�Ă̏�����������ł̑��Ə��
	EM_DIV_SOGYO	emOneSogyo   = DIV_SOGYO_NON;				// ����1�񂾂����������̑��Ə��
	// �ᑬ���� (�ғ� �� ��~)
	if(SogyoSetting.InfUpToDown_Slow.bUmu && nMpm <= SogyoSetting.InfUpToDown_Slow.nLimitSpeed ) {
		emOneSogyo = DIV_SOGYO_SLOW;
		
		if( DIV_SOGYO_STOP != emNowSogyo ) {
			SogyoSetting.InfUpToDown_Slow.nNowCnt ++;
			if( SogyoSetting.InfUpToDown_Slow.nNowCnt >= SogyoSetting.InfUpToDown_Slow.nCheckCnt ) emTotalSogyo = DIV_SOGYO_SLOW;
		}
	}
	// ���C����~ (�ғ� �� ��~)
	if(SogyoSetting.InfUpToDown_Stop.bUmu && nMpm <= SogyoSetting.InfUpToDown_Stop.nLimitSpeed ) {
		emOneSogyo = DIV_SOGYO_STOP;
	
		if( DIV_SOGYO_STOP != emNowSogyo ) {
			SogyoSetting.InfUpToDown_Stop.nNowCnt ++;
			if( SogyoSetting.InfUpToDown_Stop.nNowCnt >= SogyoSetting.InfUpToDown_Stop.nCheckCnt ) emTotalSogyo = DIV_SOGYO_STOP;
		}
	}
	// �ᑬ���� (��~ �� �ғ�)
	if(SogyoSetting.InfDownToUp_Slow.bUmu && nMpm > SogyoSetting.InfDownToUp_Slow.nLimitSpeed ) {
		emOneSogyo = DIV_SOGYO_SLOW;

		if( (DIV_SOGYO_SLOW != emNowSogyo && DIV_SOGYO_RUN != emNowSogyo) ) {
			SogyoSetting.InfDownToUp_Slow.nNowCnt ++;
			if( SogyoSetting.InfDownToUp_Slow.nNowCnt >= SogyoSetting.InfDownToUp_Slow.nCheckCnt ) emTotalSogyo = DIV_SOGYO_SLOW;
		}
	}
	// ���C������ (��~ �� �ғ�)
	if(SogyoSetting.InfDownToUp_Run.bUmu  && nMpm > SogyoSetting.InfDownToUp_Run.nLimitSpeed ) {
		emOneSogyo = DIV_SOGYO_RUN;

		if( (DIV_SOGYO_RUN != emNowSogyo) ) {
			SogyoSetting.InfDownToUp_Run.nNowCnt ++;
			if( SogyoSetting.InfDownToUp_Run.nNowCnt >= SogyoSetting.InfDownToUp_Run.nCheckCnt ) emTotalSogyo = DIV_SOGYO_RUN;
		}
	}
//	LOG(em_INF), "�����=%s, %s", DivNameManager::GetDivSogyo(emOneSogyo), DivNameManager::GetDivSogyo(emTotalSogyo));
//	LOG(em_INF), "�����=%s, %s, %d,%d,%d,%d", DivNameManager::GetDivSogyo(emOneSogyo), DivNameManager::GetDivSogyo(emTotalSogyo), SogyoSetting.array[0].nNowCnt, SogyoSetting.array[1].nNowCnt, SogyoSetting.array[2].nNowCnt, SogyoSetting.array[3].nNowCnt);


	//// ����ƈႤ�H
	EM_DIV_SOGYO  emAns = DIV_SOGYO_NON;				// �ŏI����
	if( emOneSogyo == emNowSogyo ) {
		// ��Ԃɕω��������ׁA�J�E���g�l�����Z�b�g
		for(int ii=0; ii<4; ii++) SogyoSetting.array[ii].nNowCnt = 0;

	} else {
		// ���Ə�Ԃ��ω�����
		if( DIV_SOGYO_NON ==emNowSogyo ) {		// ������ ���Ə�Ԃ��s���ׁ̈A����̌��ʂ݂̂Ŕ��肵�����Ə�Ԃ��Z�b�g
			emAns = emOneSogyo;
		} else if( emNowSogyo != emTotalSogyo && DIV_SOGYO_NON != emTotalSogyo ) {
			emAns = emTotalSogyo;
		}
	}


	////////////////////////////////////
	// �ŏI
	////////////////////////////////////
	//// ���Ə�Ԃ�ύX
	if(DIV_SOGYO_NON != emAns) {
		mcls_pStatus->SetSogyo( emAns, nMpm );
		for(int ii=0; ii<4; ii++) SogyoSetting.array[ii].nNowCnt = 0;
	}
	mcls_pStatus->SetLineSpeed(nMpm);
}

#ifdef DSP_SOGYO_CHECK
//------------------------------------------
// �ʔ� ������`�F�b�N
//------------------------------------------
void SogyoManager::CheckSampCntDiffDsp()
{
	//// �K�v�ȃ����o�[�����[�J���ɕێ�
	// ������StatusMgr�̃X�e�[�^�X�ύX�����DB Update�Ȃ̂ŁAPlgMgr�����b�N����Ă��܂��΍�
	////AutoLock autolock(&m_csLock);
	//EnterCriticalSection(&m_csLock);			// ---------------->>>
	//	double	pcDiff		= (double)(m_pcNow - m_pcPre) / m_pf;		// �Q�_�Ԃ̎���[s]
	//	long	sampCntDiff = m_nNowSampCnt - m_nPreSampCnt;			// �Q�_�Ԃ̃J�E���g

	//	double	dResV		= m_dResV;
	//	DWORD	nNowSampCnt = m_nNowSampCnt;
	//	__int64	pcNow		= m_pcNow;
	//LeaveCriticalSection(&m_csLock);			// <<<------------------
	//	
	AutoLock autolock(&m_csLock);			// ---------------->>>
	double	pcDiff = (double)(m_pcNowDsp - m_pcPreDsp) / m_pf;			// �Q�_�Ԃ̎���[s]
	long	sampCntDiff = m_nNowSampCntDsp - m_nPreSampCntDsp;			// �Q�_�Ԃ̃J�E���g

	double	dResV = m_dResVDsp;
	DWORD	nNowSampCnt = m_nNowSampCntDsp;
	__int64	pcNow		= m_pcNowDsp;
	autolock.UnLock();						// <<<------------------



	////////////////////////////////////
	// ���C�����x���Z�o
	////////////////////////////////////
	//// �O��l�Ƃ��ĕۑ�
	m_nPreSampCntDsp = nNowSampCnt;
	m_pcPreDsp = pcNow;

	if(0 > sampCntDiff) {
		return;	// 1������ (�ʏ킠�肦�Ȃ�)
	}


	//// ����̃��C�����x
	//   ���܂萳�m�ł͖������A����Ȃɐ��x���K�v�Ȗ�ł͂Ȃ��̂ŁAOK�Ƃ���
	int nMpm;
	if( 0 == sampCntDiff ) {
		nMpm = 0;
	} else {
		nMpm = (int)(((sampCntDiff*dResV) / pcDiff) * 60.0 / 1000.0);	// [mm/s]��[mpm]
		if( 0 > nMpm || 3000 < nMpm ) {		// ��������ُ̈�
			LOG(em_ERR), "[%s] �\�����x=%d, samp=%d/%d, time=%lf", my_sThreadName, nMpm, m_nNowSampCntDsp, m_nPreSampCntDsp, pcDiff );
			return;
		}
	}
	//// �O��l�Ƃ��ĕۑ�
	m_nMpmDsp = nMpm;

	////////////////////////////////////
	// ���Ə�� ����
	////////////////////////////////////
	// ���̂P�񂾂����������ɂǂ���
	EM_DIV_SOGYO	emNowSogyo   = mcls_pStatus->GetSogyoDsp();	// ���݂̑��Ə��
	EM_DIV_SOGYO	emTotalSogyo = DIV_SOGYO_NON;				// �S�Ă̏�����������ł̑��Ə��
	EM_DIV_SOGYO	emOneSogyo   = DIV_SOGYO_NON;				// ����1�񂾂����������̑��Ə��
	// �ᑬ���� (�ғ� �� ��~)
	if(SogyoSettingDsp.InfUpToDown_Slow.bUmu && nMpm <= SogyoSettingDsp.InfUpToDown_Slow.nLimitSpeed ) {
		emOneSogyo = DIV_SOGYO_SLOW;
		
		if( DIV_SOGYO_STOP != emNowSogyo ) {
			SogyoSettingDsp.InfUpToDown_Slow.nNowCnt ++;
			if( SogyoSettingDsp.InfUpToDown_Slow.nNowCnt >= SogyoSettingDsp.InfUpToDown_Slow.nCheckCnt ) emTotalSogyo = DIV_SOGYO_SLOW;
		}
	}
	// ���C����~ (�ғ� �� ��~)
	if(SogyoSettingDsp.InfUpToDown_Stop.bUmu && nMpm <= SogyoSettingDsp.InfUpToDown_Stop.nLimitSpeed ) {
		emOneSogyo = DIV_SOGYO_STOP;
	
		if( DIV_SOGYO_STOP != emNowSogyo ) {
			SogyoSettingDsp.InfUpToDown_Stop.nNowCnt ++;
			if( SogyoSettingDsp.InfUpToDown_Stop.nNowCnt >= SogyoSettingDsp.InfUpToDown_Stop.nCheckCnt ) emTotalSogyo = DIV_SOGYO_STOP;
		}
	}
	// �ᑬ���� (��~ �� �ғ�)
	if(SogyoSettingDsp.InfDownToUp_Slow.bUmu && nMpm > SogyoSettingDsp.InfDownToUp_Slow.nLimitSpeed ) {
		emOneSogyo = DIV_SOGYO_SLOW;

		if( (DIV_SOGYO_SLOW != emNowSogyo && DIV_SOGYO_RUN != emNowSogyo) ) {
			SogyoSettingDsp.InfDownToUp_Slow.nNowCnt ++;
			if( SogyoSettingDsp.InfDownToUp_Slow.nNowCnt >= SogyoSettingDsp.InfDownToUp_Slow.nCheckCnt ) emTotalSogyo = DIV_SOGYO_SLOW;
		}
	}
	// ���C������ (��~ �� �ғ�)
	if(SogyoSettingDsp.InfDownToUp_Run.bUmu  && nMpm > SogyoSettingDsp.InfDownToUp_Run.nLimitSpeed ) {
		emOneSogyo = DIV_SOGYO_RUN;

		if( (DIV_SOGYO_RUN != emNowSogyo) ) {
			SogyoSettingDsp.InfDownToUp_Run.nNowCnt ++;
			if( SogyoSettingDsp.InfDownToUp_Run.nNowCnt >= SogyoSettingDsp.InfDownToUp_Run.nCheckCnt ) emTotalSogyo = DIV_SOGYO_RUN;
		}
	}
//	LOG(em_INF), "�����=%s, %s", DivNameManager::GetDivSogyo(emOneSogyo), DivNameManager::GetDivSogyo(emTotalSogyo));
//	LOG(em_INF), "�����=%s, %s, %d,%d,%d,%d", DivNameManager::GetDivSogyo(emOneSogyo), DivNameManager::GetDivSogyo(emTotalSogyo), SogyoSettingDsp.array[0].nNowCnt, SogyoSettingDsp.array[1].nNowCnt, SogyoSettingDsp.array[2].nNowCnt, SogyoSettingDsp.array[3].nNowCnt);


	//// ����ƈႤ�H
	EM_DIV_SOGYO  emAns = DIV_SOGYO_NON;				// �ŏI����
	if( emOneSogyo == emNowSogyo ) {
		// ��Ԃɕω��������ׁA�J�E���g�l�����Z�b�g
		for(int ii=0; ii<4; ii++) SogyoSettingDsp.array[ii].nNowCnt = 0;

	} else {
		// ���Ə�Ԃ��ω�����
		if( DIV_SOGYO_NON ==emNowSogyo ) {		// ������ ���Ə�Ԃ��s���ׁ̈A����̌��ʂ݂̂Ŕ��肵�����Ə�Ԃ��Z�b�g
			emAns = emOneSogyo;
		} else if( emNowSogyo != emTotalSogyo && DIV_SOGYO_NON != emTotalSogyo ) {
			emAns = emTotalSogyo;
		}
	}


	////////////////////////////////////
	// �ŏI
	////////////////////////////////////
	//// ���Ə�Ԃ�ύX
	if(DIV_SOGYO_NON != emAns) {
		mcls_pStatus->SetSogyoDsp( emAns, nMpm );
		for(int ii=0; ii<4; ii++) SogyoSettingDsp.array[ii].nNowCnt = 0;
	}
	mcls_pStatus->SetLineSpeedDsp(nMpm);
}
#endif