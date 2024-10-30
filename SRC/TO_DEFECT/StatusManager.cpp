#include "StdAfx.h"
#include "StatusManager.h"

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
StatusManager::StatusManager(void) :
m_bIsDoorOpen(false),
m_bIsKeyOpen(false),
m_pCoil(NULL)
{
	int ii;

	//// ����@�\ ������
	memset( m_nHt_ID, 0x00, sizeof(m_nHt_ID));
	//memset( m_nCy_ID, 0x00, sizeof(m_nCy_ID));
	memset(m_nHt_Para_ID, 0x00, sizeof(m_nHt_Para_ID));
	for(ii=0; ii< NUM_MEN; ii++) SetHtState_AllOff(ii);
	//for(ii=0; ii< NUM_MEN; ii++) SetCyState_AllOff(ii);	
	SetHtParaState_AllOff();

	//// �^�C�}�[
	m_evTimerWriteSt    = CreateWaitableTimer(NULL, FALSE, NULL);
	m_evTimerSogyoSt    = CreateWaitableTimer(NULL, FALSE, NULL);
	

	//// LED�Ɩ��֌W
	m_bLedAutoCheckOff = false;
	m_nLedLineStopOffTime = GetPrivateProfileInt(TO_DEFECT, "LED_LINESTOP_OFFTIME", 5, TASKINI_NAME);	// LED�Ɩ� ���C���x�~OFF���� [��]
	m_nLedDaisyaOffLineTime = GetPrivateProfileInt(TO_DEFECT, "LED_DAISYA_OFFTIME", -1, TASKINI_NAME);	// LED�Ɩ� ��ԃI�t���C��OFF���� [�b]
	for(int ii=0; ii< NUM_MEN; ii++) {
		m_nLedLineStopCnt[ii]	= 0;						// ���C����~���̌o�߉�
		m_nLedLineRunOffCnt[ii]	= 0;
		m_nLedDaisyaOffLineCnt[ii] = 0;						// ��ԃI�t���C�����̌o�߉�
	}

	//// �@���Ԋ֘A ������
	// �J�������x
	memset(m_nCamTemp, 0x00, sizeof(m_nCamTemp));

	// �J�����Q�C��
	memset(m_nCamGain, 0x00, sizeof(m_nCamGain));

	// �J�����I������
	memset(m_nExpTime, 0x00, sizeof(m_nExpTime));

	// �J�����I������
	memset(m_nBrValue, 0x00, sizeof(m_nBrValue));

	// �J�����d��
	memset(m_nCamVolt, 0x00, sizeof(m_nCamVolt));
	
	// �ʔ�Ԋ֌W
	m_nLineSpeed	= 0;
	m_nLength		= 0;
#ifdef DSP_SOGYO_CHECK
	m_nLineSpeedDsp	= 0;
#endif

	// ���C���M���֌W
	m_emSpmStat= DIV_SPM_INI;				// SPM���

	// LED�Ɩ����u�����l
	memset(m_nLedValue, 0x00, sizeof(m_nLedValue));

	// �G�b�WUDP���
	for(int ii=0; ii<NUM_MEN; ii++) {
		m_bEdgeUdpHealthySt[ii] = false;
		m_bEdgeUdpTrackingSt[ii] = false;
	}

	//// ���L������
	// �J���R�����e�[�u��
#ifdef IS_TO_CAMERA_TBL
#ifdef LOCAL
	m_hCam = CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, sizeof(TO_CAMERA_TBL), TBL_TO_CAMERA_NAME);
	mtbl_pCam =(TO_CAMERA_TBL*) MapViewOfFile(m_hCam, FILE_MAP_ALL_ACCESS, 0, 0, 0);
#endif

	mem_connect(&m_hCam, (LPVOID *)&mtbl_pCam, TBL_TO_CAMERA_NAME);
	if(m_hCam == NULL) {
		LOG(em_ERR), "[CameraManager] �J���R�����e�[�u�� �A�N�Z�X���s");
		syslog(SYSNO_MEM_OPEN, "�J���R�����e�[�u�� �A�N�Z�X���s");
		_ASSERT(FALSE);
	}
	LOG(em_INF), "[CameraManager] �J���R�����e�[�u�� �A�N�Z�X���[�h");
#endif

}

//------------------------------------------
// �f�X�g���N�^
//------------------------------------------
StatusManager::~StatusManager(void)
{
	CloseHandle(m_evTimerWriteSt);
	CloseHandle(m_evTimerSogyoSt);

	// �J���R�����e�[�u��
#ifdef IS_TO_CAMERA_TBL
	if( NULL != m_hCam ) mem_close(&m_hCam, (LPVOID *)&mtbl_pCam);							// �J���R�����
#endif
}

// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// �X���b�h����

//------------------------------------------
// �V�O�i���҂��s��̒ǉ�
//------------------------------------------
void StatusManager::ThreadFirst_AddHandle() 
{
	//// �g���ݒ�


	// ������^�C�}�N��
	__int64 ts;
	ts = (__int64)PARAM_ST_WRITE_INTERVAL_TIME * -10000;			// 1�~���b��100�i�m�b (�}�C�i�X�w��ŁA�w�莞�Ԍo�߂ŃV�O�i��) (��3������2��ڈȍ~�̋N���Ԋu)
	SetWaitableTimer(m_evTimerWriteSt,   (LARGE_INTEGER *)&ts, PARAM_ST_WRITE_INTERVAL_TIME, NULL, NULL, FALSE);

	ts = (__int64)PARAM_ST_SOGYO_INTERVAL_TIME * -10000;			// 1�~���b��100�i�m�b (�}�C�i�X�w��ŁA�w�莞�Ԍo�߂ŃV�O�i��) (��3������2��ڈȍ~�̋N���Ԋu)
	SetWaitableTimer(m_evTimerSogyoSt,   (LARGE_INTEGER *)&ts, PARAM_ST_SOGYO_INTERVAL_TIME, NULL, NULL, FALSE);

	// �ǉ��V�O�i���̒�`
	AddEventNum(1, &m_evTimerWriteSt);
	AddEventNum(1, &m_evTimerSogyoSt);
}

//------------------------------------------
// �V�O�i����ǉ������z�̃C�x���g (�ǉ��������̂�0�n�܂�Ƃ���)
// int nEventNo �C�x���gNo (0�I���W��)
//------------------------------------------
void StatusManager::OnAddSignal(int nEventNo) 
{
	////// �V�O�i���̏���
	enum {	EV_STATUS_WRIT = 0,					// �X�e�[�^�X��������
			EV_SOGYO_WRIT						// ���Ə�ԏ�������
	};

	////// �V�O�i����������
	switch (nEventNo) {
	case EV_STATUS_WRIT:						// �X�e�[�^�X��������
		WriteStatus();
		break;

	case EV_SOGYO_WRIT:							// ���Ə�ԏ�������
		SogyoStatus_LedOnOff();
		break;

	}
}


// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// �@���ԏ�������

//------------------------------------------
// �X�e�[�^�X�������ݏ���
//------------------------------------------
void StatusManager::WriteStatus() 
{
	if( NULL == mcls_pParam) return;

	//CString wk;
	int ii, jj;
	int nId;

	//================================================
	// �J�������x
	for(ii=0; ii<MAX_CAMSET; ii++) {
		for(jj=0; jj<MAX_PHYSICAL_CAMANGLE; jj++) {
			// �J�������x�ُ�`�F�b�N
			//wk.Format("CAM_TEMPSTATE_%d_%d_ID", ii+1, jj+1);
			nId	 = this->GetStatusRead().CamTempSt[ii*MAX_PHYSICAL_CAMANGLE+jj].id;
			if( m_nCamTemp[ii][jj] >= mcls_pParam->GetCommonMast().nCamTempThr ) {		// �ُ�
				if( 0 == m_oldStateRead.CamTempSt[ii*MAX_PHYSICAL_CAMANGLE+jj].stat )	this->SetStatusQue(nId, false);
			} else {																	// ���한�A
				if( 0 != m_oldStateRead.CamTempSt[ii*MAX_PHYSICAL_CAMANGLE+jj].stat )	this->SetStatusQue(nId, true);
			}

			// �J�������x�Z�b�g
			if( m_oldStateRead.CamTemp[ii*MAX_PHYSICAL_CAMANGLE+jj].stat != m_nCamTemp[ii][jj]) {
				//wk.Format("CAM_TEMP_%d_%d_ID", ii+1, jj+1);
				nId	 = this->GetStatusRead().CamTemp[ii*MAX_PHYSICAL_CAMANGLE+jj].id;
				this->SetStatusQue(nId, m_nCamTemp[ii][jj], false);
			}
		}
	}

	//================================================
	// �J�����Q�C��
	for (ii = 0; ii<MAX_CAMANGLE - 1; ii++) {
		for (jj = 0; jj<MAX_CAMSET; jj++) {
				nId = this->GetStatusRead().CamGain[ii][jj].id;
			int nNowGain = this->GetStatusRead().CamGain[ii][jj].stat;

			if (0 != m_nCamGain[jj][ii] && nNowGain != m_nCamGain[jj][ii]) {
				this->SetStatusQue(nId, m_nCamGain[jj][ii], false);
			}
		}
	}

	//================================================
	// �J�����I������
	for (ii = 0; ii<MAX_CAMANGLE - 1; ii++) {
		for (jj = 0; jj<MAX_CAMSET; jj++) {
			nId = this->GetStatusRead().CamExpTime[ii][jj].id;
			int nNowExp = this->GetStatusRead().CamExpTime[ii][jj].stat;
			if( m_nExpTime[jj][ii] != nNowExp ){
				this->SetStatusQue(nId, m_nExpTime[jj][ii], false);
			}
		}
	}

	//================================================
	// �J�����P�x
	for (ii = 0; ii<MAX_CAMANGLE - 1; ii++) {
		for (jj = 0; jj<MAX_CAMSET; jj++) {
			nId = this->GetStatusRead().CamBrValue[ii][jj].id;
			int nNowBr = this->GetStatusRead().CamBrValue[ii][jj].stat;
			if(m_nBrValue[jj][ii] != nNowBr){
				this->SetStatusQue(nId, m_nBrValue[jj][ii], false);
			}
		}
	}

	//================================================
	// �J�����d������
	for(ii=0; ii<NUM_CAMERA; ii++){
		nId = this->GetStatusRead().CamVolt[ii].id;
		int nNowVolt = this->GetStatusRead().CamVolt[ii].stat;
		if(m_nCamVolt[ii] != nNowVolt){
			this->SetStatusQue(nId, m_nCamVolt[ii], false);
		}
	}

	//================================================
	// LED�Ɩ� �����`�F�b�N
	// ���C���������Ă���̂ɁA�������Ă�����NG��
	for( ii=0; ii<NUM_MEN; ii++) {
		bool bSate = false;			// 1���ł������� ����   �ʔ� or �ᑬ����

		// ��ԃ`�F�b�N
		// ��Ԃ��I�����C���ʒu�ɂ��邱�Ƃ��O�� �A���d�l 20150206
		for( jj=0; jj<DIV_LED_COLOR_END; jj++) {
#ifdef DISABLE_LED_DAISYA
			if( 0 == m_oldStateRead.led[ii].ch[jj].OnOff.stat &&
				//0 == m_oldStateRead.DaisyaErr[ii].stat &&
				(DIV_SOGYO_RUN == m_emSogyo || DIV_SOGYO_SLOW == m_emSogyo) ) {
#else
			if( 0 == m_oldStateRead.led.ch.OnOff[ii][jj].stat &&
				0 == m_oldStateRead.DaisyaErr[ii].stat &&
				(DIV_SOGYO_RUN == m_emSogyo || DIV_SOGYO_SLOW == m_emSogyo) ) {
#endif

				bSate = true;
			}
		}

		// �X�e�[�^�X�ύX
		if(bSate) {
			if( 0 == m_oldStateRead.LedRunOff[ii].stat ) {
				m_nLedLineRunOffCnt[ii]++;
				if(2 <= m_nLedLineRunOffCnt[ii]) {
					//wk.Format("LED_RUNOFF_%d_ID", ii+1);
					nId	 = this->GetStatusRead().LedRunOff[ii].id;
					this->SetStatusQue(nId, false);
					LOG(em_ERR), "[%s] %s LED�Ɩ����u ���C���ғ����ɏ��� �ُ�", my_sThreadName, DivNameManager::GetTorB(ii));
				}
			}	

		} else {
			m_nLedLineRunOffCnt[ii] = 0;
			if( 0 != m_oldStateRead.LedRunOff[ii].stat ) {
				//wk.Format("LED_RUNOFF_%d_ID", ii+1);
				nId	 = this->GetStatusRead().LedRunOff[ii].id;
				this->SetStatusQue(nId, true);
			}
		}

		// �����l�ύX
		for(int jj=0; jj<DIV_LED_COLOR_END; jj++) {
			// LED��OFF�̎��͖���
			if(1 == this->GetStatusRead().led.ch.OnOff[ii][jj].stat) {
				m_nLedValue[ii][jj] =  this->GetStatusRead().led.ch.ValNow[ii][jj].stat;
			}
			else {
				m_nLedValue[ii][jj] = 0;
			}
		}
	}

	//================================================
	// ���C�����
	// ����No
	int nWk = 0;
	if (NULL != m_pCoil) {
		// �ėp�I�ɁA�������܂ޓz�̒����� ���l�� ���擾����
		CString sWk = m_pCoil->data.cMeiNo;			// �I�[�͕K��NULL�Ȃ̂ł��̂܂ܓ˂�����
		nWk = atoi((LPCTSTR)sWk);
	}
	if (m_oldStateRead.Line_Mei.stat != nWk) {
		this->SetStatusQue(this->GetStatusRead().Line_Mei.id, nWk, false);
	}
	// �R�C��No
	if( NULL != m_pCoil ) {
		// �ėp�I�ɁA�������܂ޓz�̒����� ���l�� ���擾����
		CString sWk = m_pCoil->data.cCoilNoIn;			// �I�[�͕K��NULL�Ȃ̂ł��̂܂ܓ˂�����
		sWk.Replace("-", "");									// �R�C��No��9-9999�̂悤�Ƀn�C�t�������邽�߁A���O

		int nNumstart = sWk.FindOneOf("0123456789");	// �擪�� �����΍�
		CString sNum = sWk.Mid(nNumstart).SpanIncluding("0123456789");
		nWk = atoi((LPCTSTR)sNum);
	}
	if( m_oldStateRead.Line_No.stat != nWk ) {
		//this->SetStatusQue("LINE_CNO_ID", nWk, false);
		this->SetStatusQue(this->GetStatusRead().Line_No.id, nWk, false);
	}
	// �ʔ���
	if( m_oldStateRead.Line_Len.stat != m_nLength ) {
		//this->SetStatusQue("LINE_LEN_ID", m_nLength, false);
		this->SetStatusQue(this->GetStatusRead().Line_Len.id, m_nLength, false);
	}
	// �ʔc��
	nWk = 0;
	if( NULL != m_pCoil ) {
		//nWk = (m_pCoil->nCoilLenIn/1000) - m_nLength;
		nWk = (m_pCoil->data.nLenIn) - m_nLength;
	}
	if( m_oldStateRead.Line_ZanLen.stat != nWk ) {
		//this->SetStatusQue("LINE_ZANLEN_ID", nWk, false);
		this->SetStatusQue(this->GetStatusRead().Line_ZanLen.id, nWk, false);
	}
	// ���C�����x
	if( m_oldStateRead.Line_Speed.stat != m_nLineSpeed ) {
		//this->SetStatusQue("LINE_SPEED_ID", m_nLineSpeed, false);
		this->SetStatusQue(this->GetStatusRead().Line_Speed.id, m_nLineSpeed, false);
	}


	//================================================
	//// �J������Ԉʒu�`�F�b�N
	// ���C���������Ă���̂ɁA�I�t���C���ʒu�Ȃ�NG
	for (int ii = 0; ii<NUM_MEN; ii++) {
		// �I�����C���ʒu����Ȃ� ����
		// �ʔ� or �ᑬ����
		if (0 != m_oldStateRead.DaisyaSt[ii].stat &&
			(DIV_SOGYO_RUN == m_emSogyo || DIV_SOGYO_SLOW == m_emSogyo)) {

			if (0 == m_oldStateRead.DaisyaErr[ii].stat) {
				this->SetStatusQue(this->GetStatusRead().DaisyaErr[ii].id, false, false);
				LOG(em_ERR), "[StatusManager] %s ���C���ғ����ɃJ������Ԃ��I�����C���ʒu�Ŗ���", DivNameManager::GetTorB(ii));
			}
		}
		else {
			if (0 != m_oldStateRead.DaisyaErr[ii].stat) {
				this->SetStatusQue(this->GetStatusRead().DaisyaErr[ii].id, true, false);
				LOG(em_MSG), "[StatusManager] %s �J������Ԃ��I�����C���ʒu�Ŗ����`�F�b�N��", DivNameManager::GetTorB(ii));
			}
		}
	}
}

//------------------------------------------
// ���Ə�ԃ`�F�b�N����_LED�Ɩ��_��/��������
//------------------------------------------
void StatusManager::SogyoStatus_LedOnOff() 
{
	bool bSate = false;							// true:LED�Ɩ����u �_�������ʒm�L��

	// �J�����������͖��� || �����`�F�b�N����
	if( DIV_UNTEN_CAMERA == m_emUnten || m_bLedAutoCheckOff) return;
	for(int ii=0; ii<NUM_MEN; ii++) {

		bSate = false;
		for (int jj = 0; jj<DIV_LED_COLOR_END; jj++) {
			if (1 == m_oldStateRead.led.ch.OnOff[ii][jj].stat) {
				bSate = true;					// �_��������
			}
		}
		if (0 == m_oldStateRead.DaisyaErr[ii].stat)	m_nLedDaisyaOffLineCnt[ii] = 0;		// ��Ԃ��I�����C���ʒu
		else if(bSate)								m_nLedDaisyaOffLineCnt[ii]  ++;		// ��Ԃ��I�t���C���ʒu

		// �ʔ� or �ᑬ����
		if( DIV_SOGYO_RUN == m_emSogyo || DIV_SOGYO_SLOW == m_emSogyo ) {

			// ���� �� �_��
			m_nLedLineStopCnt[ii] = 0;			// LED�Ɩ��_������������J�E���^�N���A


			bSate = false;					// 1���ł������� ����   �ʔ� or �ᑬ����
			for(int jj=0; jj<DIV_LED_COLOR_END; jj++) {
#ifdef DISABLE_LED_DAISYA
				if (0 == m_oldStateRead.led[ii].ch[jj].OnOff.stat) {			// ���A���v�] �������͑�ԃI�t���C���̂��߁A��Ԃ����Ȃ�
#else
				if (0 == m_oldStateRead.led.ch.OnOff[ii][jj].stat && 0 == m_oldStateRead.DaisyaErr[ii].stat) {
#endif
					bSate = true;				// ����������
				}
			}

			// ���C���ғ����ł���Ԃ��I�����C���ʒu�ł���΁ALED�Ɩ��_��
			if(bSate) {
//��				LOG(em_MSG), "[StatusManager] %s LED�Ɩ����u �_���w�� ���M", DivNameManager::GetTorB(ii));
				ToOutMailSender::Send_KtLedMgr_OnOff(ii, TRUE);
			}

		// �x�~�� or �s��
		} else {

			// �_�� �� ����
			bSate = false;
			for(int jj=0; jj<DIV_LED_COLOR_END; jj++) {
				if (1 == m_oldStateRead.led.ch.OnOff[ii][jj].stat) {
					bSate = true;				// �_��������
				}
			}

			if(bSate) {
				m_nLedLineStopCnt[ii]++;

				int nLimitCnt = (int)(m_nLedLineStopOffTime*60*1000 / PARAM_ST_SOGYO_INTERVAL_TIME );

				if( m_nLedLineStopCnt[ii] >= nLimitCnt ) {
					LOG(em_MSG), "[StatusManager] %s LED�Ɩ����u �����w�� ���M", DivNameManager::GetTorB(ii));
					ToOutMailSender::Send_KtLedMgr_OnOff(ii, FALSE);
					m_nLedLineStopCnt[ii] = 0;
				}
			}
		}
		// ��Ԃ��I�t���C���ʒu�ň�莞�Ԍo�ߌ�ALED�Ɩ�������
		if (0 <= m_nLedDaisyaOffLineTime) {
			// �_�� �� ����
			bSate = false;
			for(int jj=0; jj<DIV_LED_COLOR_END; jj++) {
				if (1 == m_oldStateRead.led.ch.OnOff[ii][jj].stat) {
					bSate = true;				// �_��������
				}
			}
			if(bSate) {

				int nDaisyaOffLineCnt = (int)(m_nLedDaisyaOffLineTime * 1000 / PARAM_ST_SOGYO_INTERVAL_TIME);

				if (m_nLedDaisyaOffLineCnt[ii] >= nDaisyaOffLineCnt) {
					LOG(em_MSG), "[StatusManager] %s ��ԃI�t���C�� LED�Ɩ����u �����w�� ���M", DivNameManager::GetTorB(ii));
#ifdef DISABLE_LED_DAISYA
					// ���A���v�] �������͑�ԃI�t���C���̂��߁A��Ԃ����Ȃ�
#else
					ToOutMailSender::Send_KtLedMgr_OnOff(ii, FALSE);
#endif
					m_nLedDaisyaOffLineCnt[ii] = 0;
				}
			}
		}
	}
}


// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// �@���Ԕc��

//------------------------------------------
// �@���Ԏ擾 & ��ԃ`�F�b�N ������
// bool bChange �O��Ɣ�ׂĒl���ς�����Ƃ� true
//------------------------------------------
void StatusManager::OnReadKikiStatusEnd(bool bChange)
{
	//// ����̋@���Ԃ��������̃R�C�����ɕR�t��
	CheckCoilInfStatus();

	//// �\���@�\�֑��M
	if(bChange) OpSendFunc::SendOp_Status();

}

// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// �������ُ�c��

//------------------------------------------
// �������̏�Ԕc�� (���^�X�N���ُ̈�c��)
// �߂�l true���T����~
//------------------------------------------
bool StatusManager::OnMyErrWar(TYP_MY_ERRWAR_INF const* pInf)
{
	CString s;
	int n = pInf->nKind;

	if		(ON_QUE_COIL_INF == n)			 { s = "<CoilMgr> �R�C�����o�^���L���[�t��"; }
	else if (ON_QUE_COIL_RESULT == n)		 { s = "<CoilMgr> �R�C�����ѓo�^���L���[�t��"; }
	else if (ON_QUE_COIL_RESULT_UPDATE == n) { s = "<CoilMgr> �R�C�����уA�b�v�f�[�g���L���[�t��"; }
	else if (ON_SAVE_COIL_INF == n)			 { s = "<DbMgr> �R�C�����o�^���L���[�t��"; }
	else if (ON_SAVE_COIL_RESULT == n)		 { s = "<DbMgr> �R�C�����ѓo�^���L���[�t��"; }
	else if (ON_SAVE_DEFECT_INF == n)		 { s = "<DbMgr> �r���o�^���L���[�t��"; }
	else if( ON_QUE_CAMLINKSW	== n)		 { s = "<MI> �J���������N�X�C�b�`�ݒ�o�^���L���[�t��"; }
	
	else if (ON_DB_ACCESS_ERR == n)			 { s = "DB�ڑ����s"; }
	else if (ON_SEQ_FRAME_DELAY == n)		 { s = "�V�[�P���T�t���[����M�x��"; }
	else if (ON_PLG_FRAME_DELETE == n)		 { s = "��M�t���[������"; }
	else if (ON_SEQ_FRAME_SOCK_CLOSE == n)	 { s = "�ؒf�F��"; }
	else if (ON_SEQ_FRAME_SOCK_CLOSE == n)	 { s = "�ؒf�F��"; }
	else								 	 { s = "�X�e�[�^�X�ُ�";}



	LOG(em_ERR), "[%s] AP�ُ�B[�ُ�敪:%s]", my_sThreadName, s );
	return pInf->bErr;
}







//------------------------------------------
// ���݂̋@���Ԃ��R�C�����ɃZ�b�g
//------------------------------------------
void StatusManager::CheckCoilInfStatus()
{
	int ii;
	if(NULL == m_pCoil) return;

	//================================================
	// ���Ԃł��t���O�̃`�F�b�N
	for(ii=0; ii<NUM_MEN; ii++) {
		// ������
		if( 0 != m_oldStateRead.Ht_Ans[ii].stat )		m_pCoil->nSt_Hantei[ii] = 1;
		// LED���
		for(int jj=0; jj<NUM_LIGHT; jj++) {
			if( 0 != m_oldStateRead.led.Connect[ii][jj].stat ) m_pCoil->nSt_Light[ii] = 1;
		}
		for(int jj=0; jj<DIV_LED_COLOR_END; jj++) {
			if( 0 != m_oldStateRead.led.ch.Break[ii][jj].stat ) m_pCoil->nSt_Light[ii] = 1;
		}
		for(int jj=0; jj<NUM_LED_COLOR; jj++) {
			for (int kk = 0; kk < MAX_LED_TEMP_UNIT; kk++) {
				if (0 != m_oldStateRead.led.color.TempSt[ii][jj][kk].stat) m_pCoil->nSt_Light[ii] = 1;
			}
		}

		// �X�L�b�v
		if( 0 != m_oldStateRead.FrameSkip[ii].stat )	m_pCoil->nSt_FrameSkip[ii] = 1;
		if( 0 != m_oldStateRead.DB_SaveMax[ii].stat || 0 != m_oldStateRead.DB_SaveSkip[ii].stat )
														m_pCoil->nSt_DbSkip[ii] = 1;

		// �J������ԏ��
		if( 0 != m_oldStateRead.DaisyaErr[ii].stat )		m_pCoil->nSt_Daisya[ii] = 1;
	}

	// �J�����R���g���[��
	if( 0 != m_oldStateRead.CamControl.stat )			m_pCoil->nSt_CamControl = 1;
	// �J���������N�X�C�b�`
	if( 0 != m_oldStateRead.CamSwitch.stat )			m_pCoil->nSt_CamSwitch = 1;
	// �v���R��
	if( 0 != m_oldStateRead.Procon.stat  )				m_pCoil->nSt_Procon = 1;
	// FF
	if( 0 != m_oldStateRead.FFserv.stat  )				m_pCoil->nSt_FF = 1;
	// �V�[�P���T
	if (0 != m_oldStateRead.DataMem_Ap.stat || 0 != m_oldStateRead.DataMem_Hb.stat)
														m_pCoil->nSt_SqeDm = 1;
	if (0 != m_oldStateRead.UdpSeq.stat) 				m_pCoil->nSt_SqeUdp = 1;
	// �h���X�v���[���u
	if( 0 != m_oldStateRead.ToyuSprSt.stat )			m_pCoil->nSt_ToyuSplr = 1;
}

//==========================================
// �����\�Ȕ���PC�𔻕ʂ��� [��MainInstance]
// �\�E���������GroupID�o�[�W����
// NUM_MEN=2�̑O��ō���Ă܂�
//------------------------------------------
// bool*	bOk		 : �ʒP�ʂ̔��茋��
// �߂�l	bool	 : true:�����\ false:����PC����Ȃ�
//==========================================
bool StatusManager::SelectHanteiPC_SingleGroup(bool* bOk)
{
	// �擾
	int					nGetAllPcID[NUM_ALL_HANTEI];		// PCID�̏����ɁA�U�։\��PCID(1�I���W��)
	TYP_STATUS_READ		typStatus;							// ���݂̋@����
	int					nHtCount	= 0;					// ����PC�̑䐔(PC_MASTER)

	// �Z�o
	int					nSetAllPcID[NUM_ALL_HANTEI];		// PCKIND�ɑ΂���PCID�̐U�蒼���p (���픻��PC�͑O����A�ُ픻��PC�͌�납��Z�b�g)
	bool				bSetAllPcSt[NUM_ALL_HANTEI];		// nSetAllPcID�ɑ΂��鐳��^�ُ�̃��O�o�͗p (true:���� false:�ُ�)
	int					nOkCount    = 0;					// ���킾������
	int					nNgCount    = 0;					// �ُ킾������
	int					nOkMenCnt[NUM_MEN];					// �Жʂ��Ƃ̐���䐔

	// ����
	int					nNumHt		= 0;					// �Ж� ����PC�̑䐔

	//// ������
	memset(m_nHt_ID, 0x00, sizeof(m_nHt_ID));
	memset(nGetAllPcID, 0x00, sizeof(nGetAllPcID));
	memset(nSetAllPcID, 0x00, sizeof(nSetAllPcID));
	memset(bSetAllPcSt, 0x00, sizeof(bSetAllPcSt));
	memset(nOkMenCnt, 0x00, sizeof(nOkMenCnt));
	for(int ii=0; ii<NUM_MEN; ii++)	bOk[ii] = false;

	// �Жʔ���PC�̑䐔
	nNumHt = NUM_HANTEI;

	//// ���̋@���Ԃ��猟���J�n�\�Ȕ���PC��������

	int	nBkInfCount	= 0;					// �o�b�N�A�b�vPC�̑䐔 PC_INF
	int	nBkMstCount	= 0;					// �o�b�N�A�b�vPC�̑䐔 PC_MASTER
	// /////////////////////////////////////////////////////////////////////////
	// ���̋@���Ԃ��擾
	mcls_pKiki->Refresh();
	mcls_pKiki->GetStatusInf((TYP_STATUS_INF*)&typStatus, sizeof(typStatus));

	// �U�։\��PCID���擾(����PC)
	KizuPcInfManager::GetGroupPcID(TASKINI_NAME, DIV_HANTEI01, NUM_ALL_HANTEI, &nHtCount, nGetAllPcID, true);	// �擾���PC_MASTER

	if (NUM_HANTEI * NUM_MEN > nHtCount) {
		LOG(em_ERR), "[%s] �U�։\�Ȕ���PC�����ُ�=%d [DB�����������H]", my_sThreadName, nHtCount);
		return false;
	}

	// /////////////////////////////////////////////////////////////////////////
	// �����ɐ���Ȕ���PC�����߂�(���K�̔���PC���Ώ�)
	// �����ł́A�\�E�����ʂ̔z��ɃZ�b�g
	int nNgBk	= 0;
	int	nNgBkPcID[NUM_ALL_HANTEI];						// �ُ�Ȕ���PC���ꎞ�i�[�p
	memset(nNgBkPcID, 0x00, sizeof(nNgBkPcID));
	for(int ii=0; ii<nHtCount; ii++){
		int id = nGetAllPcID[ii] -1;

		// ����PC�ƃJ�����̊���t���͓����Ƃ���
		// �ُ�Ȕ���PC�̂Ƃ���ɁA�o�b�N�A�b�vPC������t����

		// �@���ԃ`�F�b�N
		if( !bOk[0] &&									// �܂������ɕK�v�ȑ䐔�������Ă��Ȃ��@�\
			nNumHt > ii &&								// �\�ʔ���PC
			0 == typStatus.Ht_Cannot[id].stat &&		// ���s�s�\
			0 == typStatus.Ht_NonAnswer[id].stat &&		// ��������
			0 == typStatus.AppEnable_Ht[id].stat &&		// �N��
			0 == typStatus.HbSend_Ht[id].stat )		{	// �n�[�g�r�[�g

			// �S������Ȃ�Ƃ肠�������̔���PC�͐���Ƃ݂Ȃ��B
			nSetAllPcID[ii] = nGetAllPcID[ii];
			bSetAllPcSt[ii] = true;						// ����
			nOkMenCnt[0]++;
			if( nNumHt <= nOkMenCnt[0] ) bOk[0] = true;	// �\�ʂ̌����ɕK�v�ȑ䐔��������

		} else if( !bOk[1] &&							// �܂������ɕK�v�ȑ䐔�������Ă��Ȃ��@��
			nNumHt*2 > ii &&							// ���ʔ���PC
			0 == typStatus.Ht_Cannot[id].stat &&		// ���s�s�\
			0 == typStatus.Ht_NonAnswer[id].stat &&		// ��������
			0 == typStatus.AppEnable_Ht[id].stat &&		// �N��
			0 == typStatus.HbSend_Ht[id].stat )		{	// �n�[�g�r�[�g

			// �S������Ȃ�Ƃ肠�������̔���PC�͐���Ƃ݂Ȃ��B
				nSetAllPcID[ii] = nGetAllPcID[ii];					// PCID�Z�b�g
			bSetAllPcSt[ii] = true;						// ����
			nOkMenCnt[1]++;
			if(nNumHt <= nOkMenCnt[1] )	bOk[1] = true;	// ���ʂ̌����ɕK�v�ȑ䐔��������
			

		} else if( 
			nNumHt*2 <= ii &&							// �o�b�N�A�b�v����PC
			0 == typStatus.Ht_Cannot[id].stat &&		// ���s�s�\
			0 == typStatus.Ht_NonAnswer[id].stat &&		// ��������
			0 == typStatus.AppEnable_Ht[id].stat &&		// �N��
			0 == typStatus.HbSend_Ht[id].stat )		{	// �n�[�g�r�[�g

			//�\�ʗ��ʂ̂ǂ���ɒǉ����邩�H
			int jj;
			for(jj=0; jj<(nNumHt*NUM_MEN); jj++) {
					if (0 == nSetAllPcID[jj]) {
						nSetAllPcID[jj] = nGetAllPcID[ii];			// PCID�Z�b�g
					bSetAllPcSt[jj] = true;					// ����

					int	nMen;
					if(nNumHt > jj ) nMen = 0;			// �\��
					else			 nMen = 1;			// ����

					nOkMenCnt[nMen]++;
					if(nNumHt <= nOkMenCnt[nMen] )	bOk[nMen] = true;	// �Жʂ̌����ɕK�v�ȑ䐔��������
						break;
					}
				}
			if(nNumHt*NUM_MEN == jj) {
				nSetAllPcID[ii] = nGetAllPcID[ii];		// PCID�Z�b�g
				bSetAllPcSt[ii] = false;				// ���킾���A�o�b�N�A�b�vPC�Ȃ̂�false
			}


			//// �S������Ȃ�Ƃ肠�������̔���PC�͐���Ƃ݂Ȃ��B
			//nSetAllPcID[ii] = nGetAllPcID[ii];			// PCID�Z�b�g
			//bSetAllPcSt[ii] = true;						// ����


			//nOkMenCnt[2]++;

			//if(nNumHt <= nOkMenCnt[2])
			//if(nNumHt <= nOkMenCnt[2] )	bOk[1] = true;	// ���ʂ̌����ɕK�v�ȑ䐔��������
		
		}else{
			if( ! bOk[0] || !bOk[1] ){
				LOG(em_ERR), "[%s] ����PC%02d �ُ� (���s�s�\=%d, �N��=%d, HB=%d, ��������=%d)", my_sThreadName,
					nGetAllPcID[ii], typStatus.Ht_Cannot[id].stat, typStatus.AppEnable_Ht[id].stat, typStatus.HbSend_Ht[id].stat, typStatus.Ht_NonAnswer[id].stat);
			}


			// ��������Z�b�g���Ă���
			//nSetAllPcID[(nHtCount + nBkCount) -nNgCount -1] = nGetAllPcID[ii];
			//nNgCount++;
			// ����PC�ƃJ�����̊��t��ێ����邽�߁A�P���ɖ�������Z�b�g�ł��Ȃ����߁A��x���[�N�ɃZ�b�g
			nNgBkPcID[nNgBk] = nGetAllPcID[ii];
			nNgBk++;
		}
	}
	nOkCount = nOkMenCnt[0] + nOkMenCnt[1];				// ���킾����PC�䐔

	for(int ii=0; ii<nNgBk; ii++) {
		for(int jj=0; jj<nHtCount; jj++) {
			if(0 == nSetAllPcID[jj]) {
				// �ُ픻��PC�ƃo�b�N�A�b�v�����{��PC�́A������t�����ɃZ�b�g
				nSetAllPcID[jj] = nNgBkPcID[ii];
			nNgCount++;
				break;
			}
		}
	}


	// /////////////////////////////////////////////////////////////////////////
	// ���O���
	// �\�E���ʂ����ꂼ��p�ӂ���(���O�o�͗p)
	CString sOkPcIdMen[NUM_MEN];			// OK PC��(�ʒP��)
	CString sNgPcIdMen;						// NG PC��
	CString sWk;

	//// �\�E������
	for(int ii=0; ii<nHtCount; ii++){
		if(0==nSetAllPcID[ii]) continue;
		sWk.Format("%d", nSetAllPcID[ii]);

		// �\�E���ǂ���ɃZ�b�g����H
		int men;
		if( nNumHt > ii )	men = 0;		// �\
		else					men = 1;		// ��
		
		if(bSetAllPcSt[ii]) {				// ���픻��PC
			if( 0 != (strlen(sOkPcIdMen[men])) )	sOkPcIdMen[men] += ", ";
		sOkPcIdMen[men] += sWk;
		} else {							// �ُ픻��PC
			if( 0 != (strlen(sNgPcIdMen)) )	sNgPcIdMen += ", ";
			sNgPcIdMen += sWk;
	}
	}

	//// �����ɕK�v�ȑ䐔���������H
	if(  !bOk[0] && !bOk[1] ){
		for(int ii=0; ii<NUM_MEN; ii++){
			LOG(em_ERR), "[%s] <%s:%s> �����ɕK�v�ȑ䐔������Ȃ� ���픻��䐔=%d�� [�\:%s][��:%s]", my_sThreadName,
				GetHanteiKind(DIV_HANTEI01), DivNameManager::GetTorB(ii), nOkCount, sOkPcIdMen[0], sOkPcIdMen[1]);
			syslog(239, "[���픻��PC<%s>(%d��)(�\:%s)(��:%s)]", GetHanteiKind(DIV_HANTEI01), nOkCount, sOkPcIdMen[0],sOkPcIdMen[1]);
		}
		return false; 
	}

	//// DB��PCID���X�V
	//KizuPcInfManager::SetGroupPcID(TASKINI_NAME, 1, NUM_ALL_HANTEI, nSetAllPcID);		// GroupNo��1�Œ�
	KizuPcInfManager::SetGroupPcID(TASKINI_NAME, DIV_HANTEI01, nHtCount, nSetAllPcID);	// GroupNo��1�Œ�

	//// �������g�͑������f
	getpcname();
		
	//// ��������͖ʒP�ʂŎ��{
	for(int ii=0; ii<NUM_MEN; ii++){
		
		//// �ݒ� ���K���ݒ�B����ɂ���āA�^�C���A�E�g���̉������������m�ł���B
		memcpy(&m_nHt_ID[ii], &nSetAllPcID[nNumHt*ii], sizeof(m_nHt_ID[ii]));

		// �Жʌ����ɑ����䐔����H
		if( nNumHt*(ii+1) <= nOkCount ){
	
			////////// �ݒ�
			//////if(DIV_HANTEI == nHtCy) memcpy(&m_nHt_ID[ii], &nSetAllPcID[nNumHt*ii], sizeof(m_nHt_ID[ii]));
			//////else					memcpy(&m_nCy_ID[ii], &nSetAllPcID[nNumHt*ii], sizeof(m_nCy_ID[ii]));
			LOG(em_MSG), "[%s] <%s:%s> �����\����PC�Z�o���� PCID[%s] �o�b�N�A�b�v[%s]", my_sThreadName,
				GetHanteiKind(DIV_HANTEI01), DivNameManager::GetTorB(ii), sOkPcIdMen[ii], sNgPcIdMen);
			syslog(240, "[<%s:%s> ���픻��PC(%s) �o�b�N�A�b�v(%s)]", GetHanteiKind(DIV_HANTEI01), DivNameManager::GetTorB(ii), sOkPcIdMen[ii], sNgPcIdMen);

		// �����ɕK�v�ȑ䐔������Ȃ�(�Жʌ�����������PC������Ȃ��ꍇ)
		}else{

			//// �ݒ�͂����A���O�o�͂̂�
			LOG(em_ERR), "[%s] <%s:%s> �����ɕK�v�ȑ䐔������Ȃ� ���픻��䐔=%d�� [%s]", my_sThreadName, 
				GetHanteiKind(DIV_HANTEI01), DivNameManager::GetTorB(ii), nOkCount - nNumHt, sOkPcIdMen[ii]);
			syslog(239, "[<%s:%s> ���픻��PC(%d��:%s)]", GetHanteiKind(DIV_HANTEI01), DivNameManager::GetTorB(ii), nOkCount-nNumHt, sOkPcIdMen[ii]);
		}
	}


	return true;


}


//==========================================
// �����\�Ȕ���PC�𔻕ʂ��� [��MainInstance]
// �O���[�vID�P�ʂł̐U�փo�[�W����
// �����W�b�N�p������PC�͏���
//------------------------------------------
// bool*	bOk		 : �ʒP�ʂ̔��茋��
// �߂�l	bool	 : true:�����\ false:����PC����Ȃ�
//==========================================
bool StatusManager::SelectHanteiPC_MultiGroup(bool* bOk)
{
	CString sWk;

	// ����PC�O���[�v��
	int		nHT_GROUP_MAX = DIV_GROUP_MAX - 1;

	// Group���̕K�v�Ȕ���PC�̑䐔�iDB����擾�j
	//  [0] CameraLinkSW1(�\)
	//  [1] CameraLinkSW2(�\)
	//  [2] CameraLinkSW3(��)
	//  [3] CameraLinkSW4(��)�@�@
	int		nNumHt[DIV_GROUP_MAX - 1];

	// �擾
	TYP_STATUS_READ	typStatus;										// ���݂̋@����
	int				nHtCount[DIV_GROUP_MAX - 1];					// Group���̔���PC�̑䐔(PC_MASTER����̎擾)

	// �Z�o
	int				nSetAllPcID[DIV_GROUP_MAX - 1][NUM_ALL_HANTEI];	// Group����PCKIND�ɑ΂���PCID�̐U�蒼���p (���픻��PC�͑O����A�ُ픻��PC�͌�납��Z�b�g)
	bool			bSetAllPcSt[DIV_GROUP_MAX - 1][NUM_ALL_HANTEI];	// Group����nSetAllPcID�ɑ΂��鐳��^�ُ�̃��O�o�͗p (true:���� false:�ُ�)
	CString			sOkPcId[DIV_GROUP_MAX - 1];						// Group����OK PC��
	CString			sNgPcId[DIV_GROUP_MAX - 1];						// Group����NG PC��

	// ������
	memset(m_nHt_ID,		0x00, sizeof(m_nHt_ID));
	memset(m_nHt_Para_ID,	0x00, sizeof(m_nHt_Para_ID));
	memset(nHtCount,		0x00, sizeof(nHtCount));
	memset(nSetAllPcID,		0x00, sizeof(nSetAllPcID));
	memset(bSetAllPcSt,		0x00, sizeof(bSetAllPcSt));
	memset(nNumHt,			0x00, sizeof(nNumHt));
	for (int ii = 0; ii<NUM_MEN; ii++)	bOk[ii] = false;

	// �O���[�v���̕K�v����PC�䐔���擾
	mcls_pParam->GetHtGroupNum(nNumHt);

	// �e�ʂ̔���PC�䐔���Z�o
	int nTopPCNum = 0;
	int nBotPCNum = 0;
	for (int ii = 0; ii < nHT_GROUP_MAX; ii++)
	{
		if (ii < (nHT_GROUP_MAX / 2))	nTopPCNum += nNumHt[ii];
		else							nBotPCNum += nNumHt[ii];
	}

	// �e�ʂ̔���PC�䐔���K��䐔�ł��邩�`�F�b�N
	if ((NUM_HANTEI != nTopPCNum) || (NUM_HANTEI != nBotPCNum))
	{
		// �Жʂ̕K�v�䐔���s��v�i�f�o�b�O���j
		// �˕K�v�䐔�𒲐�
		for (int ii = 0; ii<NUM_MEN; ii++)
		{
			if (NUM_HANTEI < (nNumHt[(ii * 2)] + nNumHt[(ii * 2) + 1]))
			{
				// �\�ʂ̕K�v�䐔�𒲐�
				int nUnit1 = 0;
				int nUnit2 = 0;
				for (int jj = 0; jj < NUM_HANTEI; jj++)
				{
					if (nNumHt[(ii * 2)] > jj)	nUnit1++;
					else						nUnit2++;
				}
				nNumHt[(ii * 2)]	= nUnit1;
				nNumHt[(ii * 2) + 1]= nUnit2;
			}
		}
	}
	sWk.Format("[%s] ����PC�K�v�䐔", my_sThreadName);
	for (int ii = 0; ii < nHT_GROUP_MAX; ii++)
	{
		CString s;
		s.Format(" %s:<Group%d:%d>", (ii < (nHT_GROUP_MAX / 2) ? "�\��" : "����"), ii + 1, nNumHt[ii]);
		sWk += s;
	}
	LOG(em_MSG), sWk);

	// �e�����N�X�C�b�`(�O���[�v��)�̐擪�J����No���Z�b�g
	int nTopCam = 0;
	for (int ii = 0; ii < nHT_GROUP_MAX; ii++)
	{
		m_nHt_TopCamno[ii] = nTopCam;
		nTopCam += nNumHt[ii] * NUM_CAMLINK_BOARD; // �P����PC�ɂ�
	}

	// ���̋@���Ԃ��擾
	mcls_pKiki->Refresh();
	mcls_pKiki->GetStatusInf((TYP_STATUS_INF*)&typStatus, sizeof(typStatus));

	bool	bCompleted[DIV_GROUP_MAX - 1];			// �ΏۃO���[�v���K�v�Ƃ���U��PC���ɒB����
	for (int ii = 0; ii<nHT_GROUP_MAX; ii++)	bCompleted[ii] = false;

	// ���j�b�g�P��(LinkSwitch)�ŐU�ւ����{����
	for (int grp = 0; grp < nHT_GROUP_MAX; grp++)
	{
		int	nGrpPcID[NUM_ALL_HANTEI];							// PCID�̏����ɁA�U�։\��PCID(1�I���W��)

		memset(nGrpPcID, 0x00, sizeof(nGrpPcID));

		// �ΏۃO���[�v�̐U�։\��PCID���擾
		// ���擾���PC_MASTER
		KizuPcInfManager::GetGroupPcID(TASKINI_NAME, DIV_HANTEI01 + grp, NUM_ALL_HANTEI, &nHtCount[grp], nGrpPcID, true);

		LOG(em_MSG), "[%s] <%s> �䐔�F%d", my_sThreadName, GetHanteiKind(DIV_HANTEI01 + grp), nHtCount[grp]);

		// LinkSwitch(�n�[�t��)���̕K�vPC�ڑ��䐔�ɖ����Ȃ��ꍇ�̓G���[
		if (nNumHt[grp] > nHtCount[grp])
		{
			LOG(em_ERR), "[%s] <%s> �U�։\�Ȕ���PC�����ُ�=%d [DB�����������H]",
				my_sThreadName, GetHanteiKind(DIV_HANTEI01 + grp), nHtCount[grp]);
			return false;
		}

		// /////////////////////////////////////////////////////////////////////////
		// �����ɐ���Ȕ���PC�����߂�(���K�̔���PC���Ώ�)
		int		nNgBk = 0;									// �ُ픻��PC�i�[��
		int		nNgBkPcID[NUM_ALL_HANTEI];					// �ُ픻��PC�ꎞ�i�[�o�b�t�@
		int		nGrpOkCount = 0;							// �ΏۃO���[�v�ŐU�ւ�PC��

		memset(nNgBkPcID, 0x00, sizeof(nNgBkPcID));

		// �K�v�䐔����
		if (0 >= nNumHt[grp]) bCompleted[grp] = true;

		// �擾��������PC���A�@���Ԃ����ĐU�ւ����{����
		for (int ii = 0; ii<nHtCount[grp]; ii++)
		{
			int id = nGrpPcID[ii] - 1;

			// ����Ȕ���PC��PCID�̎Ⴂ���ɐU�ւ���
			// ����PC�ƃJ�����̊���t���͓����Ƃ���
			// �ُ�Ȕ���PC�̂Ƃ���ɁA�o�b�N�A�b�vPC������t����

			// �@���ԃ`�F�b�N
			if (!bCompleted[grp] &&							// �܂������ɕK�v�ȑ䐔�������Ă��Ȃ�
				0 == typStatus.Ht_Cannot[id].stat &&		// ���s�s�\
				0 == typStatus.Ht_NonAnswer[id].stat &&		// ��������
				0 == typStatus.AppEnable_Ht[id].stat &&		// �N��
				0 == typStatus.HbSend_Ht[id].stat)			// �n�[�g�r�[�g
			{
				if (ii < nNumHt[grp])
				{
					// ���K����PC�̊���

					// �S������Ȃ�Ƃ肠�������̔���PC�͐���Ƃ݂Ȃ��B
					nSetAllPcID[grp][ii] = nGrpPcID[ii];		// PCID�Z�b�g
					bSetAllPcSt[grp][ii] = true;				// ����
					nGrpOkCount++;
					// �Ώ�LinkSwitch�ւ̐ڑ��䐔��������
					// ��LinkSwitch���ɕK�v�Ȕ���PC�����܂��Ă��邱�Ƃ�O��Ƃ���
					// �@���̓J�����ڑ��ɂ���Ă͂P��LinkSwitch�ɔ���PC���W�񂷂邱�Ƃ��\�����l�����Ȃ�
					// �@�˃V�X�e���\���}�̍\���ɏ�����
					if (nNumHt[grp] <= nGrpOkCount) bCompleted[grp] = true;
				}
				else
				{
					// �o�b�N�A�b�v�@�̊���

					// �K�v�䐔�͈͓̔��Ŗ������̏ꏊ�͂��邩�H
					bool bAssign = false;
					for (int jj = 0; jj < nNumHt[grp]; jj++)
					{
						if (0 == nSetAllPcID[grp][jj])
						{
							// ���蓖�����Ă��Ȃ��ꏊ����������
							nSetAllPcID[grp][jj] = nGrpPcID[ii];// PCID�Z�b�g
							bSetAllPcSt[grp][jj] = true;		// ����
							nGrpOkCount++;
							// �Ώ�LinkSwitch�ւ̐ڑ��䐔��������
							// ��LinkSwitch���ɕK�v�Ȕ���PC�����܂��Ă��邱�Ƃ�O��Ƃ���
							// �@���̓J�����ڑ��ɂ���Ă͂P��LinkSwitch�ɔ���PC���W�񂷂邱�Ƃ��\�����l�����Ȃ�
							// �@�˃V�X�e���\���}�̍\���ɏ�����
							if (nNumHt[grp] <= nGrpOkCount) bCompleted[grp] = true;
							bAssign = true;
							break;
						}
					}
					if (!bAssign)
					{
						// �o�b�N�A�b�v�@���t����Ȃ������ꍇ
						// ���K�v�䐔�ɒB���ĂȂ��A���o�b�N�A�b�v�@��Index�Ȃ̂�
						// �@�K�v�䐔�͈͓̔��Ɉُ�PC�����݂��Ă��锤
						// �@�˂܂��{�I�ɂ͂��̃P�[�X�ɂ͓���Ȃ��i�O�̂��߁j
						nSetAllPcID[grp][ii] = nGrpPcID[ii];	// PCID�Z�b�g
						bSetAllPcSt[grp][ii] = false;			// ���킾���A�o�b�N�A�b�vPC�Ȃ̂�false
					}
				}
			}
			else if (
				0 == typStatus.Ht_Cannot[id].stat &&		// ���s�s�\
				0 == typStatus.Ht_NonAnswer[id].stat &&		// ��������
				0 == typStatus.AppEnable_Ht[id].stat &&		// �N��
				0 == typStatus.HbSend_Ht[id].stat)			// �n�[�g�r�[�g
			{
				// ���ɕK�v�䐔��������Ă���̂ŁA�o�b�N�A�b�v�@�Ƃ��ăZ�b�g
				nSetAllPcID[grp][ii] = nGrpPcID[ii];		// PCID�Z�b�g
				bSetAllPcSt[grp][ii] = false;				// ���킾���A�o�b�N�A�b�vPC�Ȃ̂�false
			}
			else
			{
				// �ُ�PC���o
				if (!bCompleted[grp])
				{
					LOG(em_ERR), "[%s] ����PC%02d �ُ� (���s�s�\=%d, �N��=%d, HB=%d, ��������=%d)", my_sThreadName,
						nGrpPcID[ii], typStatus.Ht_Cannot[id].stat, typStatus.AppEnable_Ht[id].stat, typStatus.HbSend_Ht[id].stat, typStatus.Ht_NonAnswer[id].stat);
				}

				// ��������Z�b�g���Ă���
				// ����PC�ƃJ�����̊��t��ێ����邽�߁A�P���ɖ�������Z�b�g�ł��Ȃ����߁A��x���[�N�ɃZ�b�g
				nNgBkPcID[nNgBk] = nGrpPcID[ii];
				nNgBk++;
			}
		}

		// PCID�̃}�[�W
		// ���ُ픻��PC�ƃo�b�N�A�b�v�����{��PC�𖢊���t�����ɃZ�b�g
		for (int ii = 0; ii<nNgBk; ii++)
		{
			for (int jj = 0; jj<nHtCount[grp]; jj++)
			{
				if (0 == nSetAllPcID[grp][jj])
				{
					nSetAllPcID[grp][jj] = nNgBkPcID[ii];
					bSetAllPcSt[grp][jj] = false;			// �ُ�
					break;
				}
			}
		}

		// /////////////////////////////////////////////////////////////////////////
		// ���O���(���O�o�͗p)
		//
		for (int ii = 0; ii<nHtCount[grp]; ii++)
		{
			if (0 == nSetAllPcID[grp][ii]) continue;

			sWk.Format("%d", nSetAllPcID[grp][ii]);

			if (bSetAllPcSt[grp][ii])
			{	// ���픻��PC
				if (0 != sOkPcId[grp].GetLength())	sOkPcId[grp] += ", ";
				sOkPcId[grp] += sWk;
			}
			else
			{	// �ُ픻��PC
				if (0 != sNgPcId[grp].GetLength())	sNgPcId[grp] += ", ";
				sNgPcId[grp] += sWk;
			}
		}

		// �����ɕK�v�ȑ䐔���������H
		if (!bCompleted[grp])
		{
			// ���U���s�i�������f�j
			LOG(em_ERR), "[%s] <%s> �����ɕK�v�ȑ䐔������Ȃ� ���픻��䐔=%d�� [%s]",
				my_sThreadName, GetHanteiKind(DIV_HANTEI01 + grp), nGrpOkCount, sOkPcId[grp]);
			syslog(239, "[���픻��PC<%s>(%d��) %s)]", GetHanteiKind(DIV_HANTEI01 + grp), nGrpOkCount, sOkPcId[grp]);
		}
	}

	// �Ăь��ɕԂ����ʃt���O���Z�b�g
	for (int ii = 0; ii<NUM_MEN; ii++)	bOk[ii] = true;
	for (int grp = 0; grp < nHT_GROUP_MAX; grp++)
	{
		// �䐔�������Ă��Ȃ��O���[�v�����݂���ꍇ�A���̖ʂ�NG
		if (!bCompleted[grp]) bOk[(grp < (nHT_GROUP_MAX / 2) ? 0 : 1)] = false;
	}

	// ���ʂm�f�̏ꍇ�A�������f
	if (!bOk[0] && !bOk[1])
	{
		LOG(em_ERR), "[%s] ���ʂƂ��K�v�Ȕ���䐔������Ȃ��B", my_sThreadName);
		return false;
	}
	else
	{
		if (bOk[0] && bOk[1])
		{
			LOG(em_MSG), "[%s] ���ʂƂ�����䐔�n�j�I", my_sThreadName);
		}
		else
		{
			LOG(em_MSG), "[%s] %s�̂ݔ���䐔�n�j�I", my_sThreadName, (bOk[0] ? "�\��" : "����"));
		}
	}

	// DB_PC_INF��PCID���X�V
	for (int grp = 0; grp < nHT_GROUP_MAX; grp++)
	{
		KizuPcInfManager::SetGroupPcID(TASKINI_NAME, DIV_HANTEI01 + grp, nHtCount[grp], nSetAllPcID[grp]);
	}

	// �������g�͑������f
	getpcname();

	// �����Ώۂ̔���PC��ݒ�
	// ���K���ݒ�B����ɂ���āA�^�C���A�E�g���̉������������m�ł���B
	int nSetIndex[NUM_MEN] = {0, 0};
	for (int grp = 0; grp < nHT_GROUP_MAX; grp++)
	{
		int nMen = ((grp < (nHT_GROUP_MAX / 2)) ? 0 : 1);

		// �m�f�̖ʂ�PCID���Z�b�g���Ȃ�
		if (!bOk[nMen]) continue;

		// PCID�Z�b�g
		memcpy(&m_nHt_ID[nMen][nSetIndex[nMen]], &nSetAllPcID[grp], sizeof(int) * nNumHt[grp]);
		nSetIndex[nMen] += nNumHt[grp];

		// �������O�o��
		LOG(em_MSG), "[%s] <%s> �����\����PC�Z�o���� �L��PC[%s] �o�b�N�A�b�vPC[%s]",
			my_sThreadName, GetHanteiKind(DIV_HANTEI01 + grp), sOkPcId[grp], sNgPcId[grp]);
		syslog(240, "[<%s> ���픻��PC(%s) �o�b�N�A�b�v(%s)]", GetHanteiKind(DIV_HANTEI01 + grp), sOkPcId[grp], sNgPcId[grp]);
	}

	return true;
}

//==========================================
// �����\�ȃ��W�b�N�p������PC�𔻕ʂ��� [��MainInstance]
//------------------------------------------
// �߂�l	bool	 : true:�����\�@false:���W�b�N�p������PC���s��
//==========================================
bool StatusManager::SelectHanteiPC_ResumeGroup()
{
	// �擾
	TYP_STATUS_READ	typStatus;						// ���݂̋@����
	int				nHtCount;						// ���W�b�N�p������Group�̔���PC�̑䐔(PC_MASTER����̎擾)

	// �Z�o
	int				nSetAllPcID[NUM_ALL_HANTEI];	// ���W�b�N�p������Group��PCKIND�ɑ΂���PCID�̐U�蒼���p (���픻��PC�͑O����A�ُ픻��PC�͌�납��Z�b�g)
	bool			bSetAllPcSt[NUM_ALL_HANTEI];	// ���W�b�N�p������Group��nSetAllPcID�ɑ΂��鐳��^�ُ�̃��O�o�͗p (true:���� false:�ُ�)
	CString			sOkPcId;						// ���W�b�N�p������Group��OK PC��
	CString			sNgPcId;						// ���W�b�N�p������Group��NG PC��

	// ������
	memset(m_nHt_Para_ID, 0x00, sizeof(m_nHt_Para_ID));
	memset(nSetAllPcID, 0x00, sizeof(nSetAllPcID));
	memset(bSetAllPcSt, 0x00, sizeof(bSetAllPcSt));
	nHtCount = 0;

	// ���̋@���Ԃ��擾
	mcls_pKiki->Refresh();
	mcls_pKiki->GetStatusInf((TYP_STATUS_INF*)&typStatus, sizeof(typStatus));

	// �ΏۃO���[�v��PCID���X�g(1�I���W��)
	int	nGrpPcID[NUM_ALL_HANTEI];
	memset(nGrpPcID, 0x00, sizeof(nGrpPcID));

	// �ΏۃO���[�v�̐U�։\��PCID���擾
	// ���擾���PC_MASTER
	KizuPcInfManager::GetGroupPcID(TASKINI_NAME, DIV_LOGIPARA, NUM_ALL_HANTEI, &nHtCount, nGrpPcID, true);

	// NUM_ALL_HTPARA�ƈ�v���Ȃ��ꍇ�̓G���[�Ƃ���
	// �����W�b�N�p������PC�̓o�^�͕K�{
	if (NUM_ALL_HTPARA > nHtCount)
	{
		LOG(em_ERR), "[%s] <%s> ���W�b�N�p������PC��`�����ُ�=%d [DB�����������H]",
			my_sThreadName, GetHanteiKind(DIV_LOGIPARA), nHtCount);
		return false;
	}

	LOG(em_MSG), "[%s] <%s> �䐔�F%d", my_sThreadName, GetHanteiKind(DIV_LOGIPARA), nHtCount);
		
	// �擾�������W�b�N�p������PC���A�@���Ԃ����Ċi�[����
	// ��PC�̎�����Ԃ̃`�F�b�N�̂ݎ��{����
	for (int ii = 0; ii < NUM_ALL_HTPARA; ii++)
	{
		if (0 == nGrpPcID[ii]) continue;

		int id = nGrpPcID[ii] - 1;

		// ����Ȕ���PC��PCID�̎Ⴂ���ɐU�ւ���
		// ����PC�ƃJ�����̊���t���͓����Ƃ���
		// �ُ�Ȕ���PC�̂Ƃ���ɁA�o�b�N�A�b�vPC������t����

		// �@���ԃ`�F�b�N
		if (0 == typStatus.Ht_Cannot[id].stat &&			// ���s�s�\
			0 == typStatus.Ht_NonAnswer[id].stat &&			// ��������
			0 == typStatus.AppEnable_Ht[id].stat &&			// �N��
			0 == typStatus.HbSend_Ht[id].stat)				// �n�[�g�r�[�g
		{
			// �S������Ȃ�Ƃ肠�������̔���PC�͐���Ƃ݂Ȃ��B
			nSetAllPcID[ii] = nGrpPcID[ii];
			bSetAllPcSt[ii] = true;							// ����
		}
		else
		{
			// �ُ�ȃ��W�b�N�p������PC
			LOG(em_ERR), "[%s] ���W�b�N�p������PC%02d �ُ� (���s�s�\=%d, �N��=%d, HB=%d, ��������=%d)", my_sThreadName,
				nGrpPcID[ii], typStatus.Ht_Cannot[id].stat, typStatus.AppEnable_Ht[id].stat, typStatus.HbSend_Ht[id].stat, typStatus.Ht_NonAnswer[id].stat);

				// �ُ�ȃ��W�b�N�p������PC
				nSetAllPcID[ii] = nGrpPcID[ii];				// PCID�Z�b�g
				bSetAllPcSt[ii] = false;					// �ُ�PC�Ȃ̂�false
		}
	}

	// /////////////////////////////////////////////////////////////////////////
	// ���O���(���O�o�͗p)
	CString sWk;
	for (int ii = 0; ii < NUM_ALL_HTPARA; ii++)
	{
		if (0 == nSetAllPcID[ii]) continue;

		sWk.Format("%d", nSetAllPcID[ii]);

		if (bSetAllPcSt[ii])
		{	// ���탍�W�b�N�p������PC
			if (0 != sOkPcId.GetLength())	sOkPcId += ", ";
			sOkPcId += sWk;
		}
		else
		{	// �ُ탍�W�b�N�p������PC
			if (0 != sNgPcId.GetLength())	sNgPcId += ", ";
			sNgPcId += sWk;
		}
	}

	// /////////////////////////////////////////////////////////////////////////
	// ���W�b�N�p������PCID���i�[
	// ������E�ُ�֌W�Ȃ��擪��2PC
	for (int ii = 0; ii < NUM_ALL_HTPARA; ii++)
	{
		m_nHt_Para_ID[ii] = nSetAllPcID[ii];
	}

	// �������O�o��
	LOG(em_MSG), "[%s] <%s> �����\����PC�Z�o���� �L��PC[%s] �o�b�N�A�b�vPC[%s]",
		my_sThreadName, GetHanteiKind(DIV_LOGIPARA), sOkPcId, sNgPcId);
	syslog(240, "[<%s> ���픻��PC(%s) �o�b�N�A�b�v(%s)]", GetHanteiKind(DIV_LOGIPARA), sOkPcId, sNgPcId);

	return true;
}

//==========================================
// �w��X�C�b�`FPGA�{�[�hID�̃O���[�v�̐擪�J����No��Ԃ�
//------------------------------------------
// �߂�l	int	 : �J����No (1 - 20)
//==========================================
int StatusManager::GetGroupTopCamNo(int nFPGABoardNo)
{
	// �X�C�b�`FPGA�{�[�hID����O���[�vID���Z�o�i0-7 �� 0-3�j
	int nTargetGroupID = nFPGABoardNo / 2;

	nTargetGroupID = (3 < nTargetGroupID ? 3 : nTargetGroupID);

	return m_nHt_TopCamno[nTargetGroupID];
}