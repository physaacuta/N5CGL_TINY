#include "StdAfx.h"
#include "AlarmeManager.h"

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
AlarmeManager::AlarmeManager(void) :
ThreadManager("AlarmeMgr"),
m_tId_Alarme(0),
m_bPatoManualOn(false)
{
	//// �n���h������
	m_evTimerExec  = CreateWaitableTimer(NULL, FALSE, NULL);

	//// ������
	memset(&m_NowAlarme, 0x00, sizeof(m_NowAlarme));
	//memset(&m_nSlowDown, 0x00, sizeof(m_nSlowDown));

	// ������
	InitializeCriticalSection(&m_csLock);								// �N���e�B�J���Z�N�V�����I�u�W�F�N�g��������

	//// ����p�f�[�^ ������
	m_bVoicErrCancel = false;
	m_nVoicErrNo = em_VOICE_ERR_NON;
}

//------------------------------------------
// �f�X�g���N�^
//------------------------------------------
AlarmeManager::~AlarmeManager(void)
{
	//// �o�b�t�@������
	std::list<PATO_INF*>::iterator		itr;					// �C�e���[�^
	for (int ii = 0; ii<NUM_MEN; ii++) {
		for (itr = mlst_AlmBuf[ii].begin(); itr != mlst_AlmBuf[ii].end(); ) {	
			SAFE_DELETE(*itr);						// �̈�J��
			itr = mlst_AlmBuf[ii].erase(itr);		// �v�f�폜
			continue;
		}
	}

	//// �n���h�����
	CloseHandle(m_evTimerExec);

	//// �J��
	DeleteCriticalSection(&m_csLock);									// �N���e�B�J���Z�N�V�����I�u�W�F�N�g���J��
}

//------------------------------------------
// �X���b�h�J�n�O���� (�X���b�h���s����(�V�O�i���Z�b�g�O)�ɃR�[���o�b�N)
//------------------------------------------
int AlarmeManager::ThreadFirst()
{
	//// ���O����
	__int64 ts = TIME_INTERVAL_EXEC * -10000; 
	SetWaitableTimer(m_evTimerExec, (LARGE_INTEGER *)&ts, TIME_INTERVAL_EXEC, NULL, NULL, FALSE);

	//// �C�x���g�ݒ�
	HANDLE hArray[] = { m_evTimerExec
						};
	SetEventNum(sizeof(hArray)/sizeof(HANDLE), hArray);
	return 0;
}

//------------------------------------------
// �X���b�h�I���O���� (�I���V�O�i����ɃR�[���o�b�N)
//------------------------------------------
int AlarmeManager::ThreadLast()
{
	// �������~
	CancelWaitableTimer(m_evTimerExec);
	return 0;
}

//------------------------------------------
// �X���b�h�C�x���g����
// int nEventNo �C�x���gNo (0�I���W��)
//------------------------------------------
void AlarmeManager::ThreadEvent(int nEventNo)
{
	MSG	msg;								// MSG�\����
	PeekMessage(&msg, NULL, WM_USER, WM_USER, PM_NOREMOVE);			// ���b�Z�[�W�쐬

	////// �V�O�i���̏���
	enum {	EV_TIMER_EXEC = 0,				// �������s ������^�C�}
	};

	////// �V�O�i����������
	switch (nEventNo) {

//-----------------------------------------------------------------------------------------------
	case EV_TIMER_EXEC: 					// �������s ������^�C�}
		Output_Alarme();
		//Output_Sizi();
		break;
	
//-----------------------------------------------------------------------------------------------
	default :
		// ���肦�Ȃ��I�I
		LOG(em_ERR), "[%s] ThreadEvent EventNo NG=%d", my_sThreadName, nEventNo );
		break;
	}

	// �����ɗ����Ƃ��́A���b�Z�[�W���������邩������Ȃ�
	// �]���ă��[�v�Ń��b�Z�[�W��S���z���o�� (���������� PeekMessage �łȂ��ƃ_��)
	int rc;
	while (rc = PeekMessage(&msg, NULL, 0, 0, PM_REMOVE) ) {
		switch( msg.message ) {

//-----------------------------------------------------------------------------------------------
		case WM_TIMER:								// ���׌��o�A���[��
			// ���׌��o �{�C�X�o�͒�~
			SAFE_TIMER_KILL(m_tId_Alarme);
			break;
		}
	}
}



// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// �o��

//------------------------------------------
// �r�ڋ� �x��o��
//------------------------------------------
void AlarmeManager::Output_Alarme()
{
	ALARME_OUTPUT	Output;
	memset(&Output, 0x00, sizeof(ALARME_OUTPUT));

	//// ����̏�Ԃ��擾
	Check_Alarme(&Output);

	// �����J�n1�{�ڂ̓p�g���C�g�A�r�ڋ߃����v�A���׃{�C�X�͏o�͂��Ȃ�
	if (0 != mcls_pStatus->GetStatusRead().Coil_Init.stat ||
		0 != mcls_pStatus->GetStatusRead().Coil_ReStart.stat) {
		memset(Output.ranp, em_RAMP_STAT_OFF, sizeof(Output.ranp));
		memset(Output.alarm, em_RAMP_STAT_OFF, sizeof(Output.alarm));
		memset(Output.pato, 0x00, sizeof(Output.pato));
		for(int ii=OUTPUT_VOICE_ERR_NUM-1; ii<OUTPUT_VOICE_NUM;ii++) {
			Output.voice[ii] = em_VOICE_STAT_OFF;
		}
	}

	//// �x��o�͂̏�Ԃɕω�������΁A�\���Ǘ��ƃV�[�P���T�iDM�o�R�j�ɒʒm
	// �r�ڋ߃����v
	bool bEmargOut = false;
	if (0 != memcmp(&m_NowAlarme.ranp, &Output.ranp, sizeof(m_NowAlarme.ranp)))
	{
		LOGA(em_MSG), "[%s] �r�ڋ߃����v��ԕω����m", my_sThreadName);

		bEmargOut = true;

		// �V�[�P���T
		DmemManager::DELIVERY* pDeli = new DmemManager::DELIVERY;
		pDeli->data = new DmemManager::OCCUR_RAMP;
		DmemManager::OCCUR_RAMP *pData = (DmemManager::OCCUR_RAMP*)pDeli->data;
		pDeli->emNo = DmemManager::em_RAMP;
		memcpy(pData, &Output.ranp, sizeof(DmemManager::OCCUR_RAMP));

		// �o�^
		if (!mque_pAddDmData->AddToTailFreeCheck(pDeli, 0)) {		// �o�^
			LOGA(em_WAR), "[%s] DM�x���� �L���[�t��(�r�ڋ߃����v)", my_sThreadName);
			LOGG(em_WAR), "[%s] DM�x���� �L���[�t��(�r�ڋ߃����v)", my_sThreadName);
			delete pDeli->data;
			delete pDeli;
		}
	}
	// �r�ڋ߃A���[��
	if (0 != memcmp(&m_NowAlarme.alarm, &Output.alarm, sizeof(m_NowAlarme.alarm)))
	{
		LOGA(em_MSG), "[%s] �r�ڋ߃A���[����ԕω����m", my_sThreadName);

		bEmargOut = true;

		// �V�[�P���T
		DmemManager::DELIVERY* pDeli = new DmemManager::DELIVERY;
		pDeli->data = new DmemManager::OCCUR_ALARM;
		DmemManager::OCCUR_ALARM *pData = (DmemManager::OCCUR_ALARM*)pDeli->data;
		pDeli->emNo = DmemManager::em_ALARM;
		memset(pData, em_RAMP_STAT_OFF, sizeof(DmemManager::OCCUR_ALARM));

		// �W�񂩂�S��ɕϊ�
		// �����ʂ̂�
		for (int ii = 0; ii < OUTPUT_ALARM_COL; ii++)
		{
			// �y�r
			if (em_RAMP_STAT_OFF != Output.alarm[1][(ii * 2)][0] ||
				em_RAMP_STAT_OFF != Output.alarm[1][(ii * 2) + 1][0])
			{
				pData->alarm[ii][0] = em_RAMP_STAT_ON;
			}
			// �d�r
			if (em_RAMP_STAT_OFF != Output.alarm[1][(ii * 2)][1] ||
				em_RAMP_STAT_OFF != Output.alarm[1][(ii * 2) + 1][1])
			{
				pData->alarm[ii][1] = em_RAMP_STAT_ON;
			}
		}
		// �W��̏����ێ�����i�\���p�j
		for (int ii = 0; ii < OUTPUT_MESYU_SEKKIN_COL; ii++)
		{
			for (int jj = 0; jj < OUTPUT_RAMP_NUM; jj++)
			{
				pData->alarmPos[ii][jj] = Output.alarm[1][ii][jj];
			}
		}
		//LOG(em_MSG), "[%s] �A���[����ԕύX[1:%d/%d 2:%d/%d 3:%d/%d 4:%d/%d]", my_sThreadName,
		//	pData->alarm[0][0], pData->alarm[0][1], pData->alarm[1][0], pData->alarm[1][1],
		//	pData->alarm[2][0], pData->alarm[2][1], pData->alarm[3][0], pData->alarm[3][1]);

		// �o�^
		if (!mque_pAddDmData->AddToTailFreeCheck(pDeli, 0)) {		// �o�^
			LOGA(em_WAR), "[%s] DM�x���� �L���[�t��(�r�ڋ߃A���[��)", my_sThreadName);
			LOGG(em_WAR), "[%s] DM�x���� �L���[�t��(�r�ڋ߃A���[��)", my_sThreadName);
			delete pDeli->data;
			delete pDeli;
		}
	}
	// �r�ڋ߃����v�A�A���[���̏�ԕω�����
	if (bEmargOut)
	{
		//// �\���Ǘ�
		OpSendFunc::SendOp_AlarmData(&Output);
	}
	// �p�g���C�g
	if (0 != memcmp(&m_NowAlarme.pato, &Output.pato, sizeof(DmemManager::OCCUR_PATO)))
	{
		LOGA(em_MSG), "[%s] �p�g���C�g��ԕω����m", my_sThreadName);

		//// �V�[�P���T
		DmemManager::DELIVERY* pDeli = new DmemManager::DELIVERY;
		pDeli->data = new DmemManager::OCCUR_PATO;
		DmemManager::OCCUR_PATO *pData = (DmemManager::OCCUR_PATO*)pDeli->data;
		pDeli->emNo = DmemManager::em_PATO;
		memcpy(pData, &Output.pato, sizeof(DmemManager::OCCUR_PATO));
		//LOG(em_MSG), "[%s] �p�g���C�g���ύX[�ۗL��=%d/%d/%d/%d]", my_sThreadName, pData->pato[0][0], pData->pato[0][1], pData->pato[1][0], pData->pato[1][1]);

		// �o�^
		if (!mque_pAddDmData->AddToTailFreeCheck(pDeli, 0)) {		// �o�^
			LOGA(em_WAR), "[%s] DM�x���� �L���[�t��(�p�g���C�g)", my_sThreadName);
			LOGG(em_WAR), "[%s] DM�x���� �L���[�t��(�p�g���C�g)", my_sThreadName);
			delete pDeli->data;
			delete pDeli;
		}
	}
	// �{�C�X
	// ����Ƀ{�C�X�̗v���𑗐M���ADmemManager���ő��M���̗}�������{����
	int id;
	for (id = 0; id < OUTPUT_VOICE_NUM; id++)
	{
		if (Output.voice[id]) break;
	}
	
	// ���炩�̃{�C�X�o�͂���
	if (OUTPUT_VOICE_NUM != id)
	{
		//// �V�[�P���T
		DmemManager::DELIVERY* pDeli = new DmemManager::DELIVERY;
		pDeli->data = new DmemManager::OCCUR_VOICE;
		DmemManager::OCCUR_VOICE *pData = (DmemManager::OCCUR_VOICE*)pDeli->data;
		pDeli->emNo = DmemManager::em_VOICE;
		memcpy(pData, &Output.voice, sizeof(DmemManager::OCCUR_VOICE));

		// �o�^
		if (!mque_pAddDmData->AddToTailFreeCheck(pDeli, 0)) {		// �o�^
			LOGA(em_WAR), "[%s] DM�x���� �L���[�t��(�{�C�X)", my_sThreadName);
			LOGG(em_WAR), "[%s] DM�x���� �L���[�t��(�{�C�X)", my_sThreadName);
			delete pDeli->data;
			delete pDeli;
		}
	}

	//// �ŏI�f�[�^�Z�b�g
	memcpy(&m_NowAlarme, &Output, sizeof(ALARME_OUTPUT));
}

// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// �x��o��


//------------------------------------------
// �r�ڋ� �x��o�͔���
// ALARME_OUTPUT*	p	�r�ڋ߃����v�̏��
//------------------------------------------
void AlarmeManager::Check_Alarme(ALARME_OUTPUT*	p)
{
	int rec, row;
	int sIndex, eIndex;								// ���z�}�b�v�̑ΏۊJ�n�E�I�� ���Έʒu (100���e�\���ʒu 0:������ 200:�㗬��)

	TYP_VirtualMap	map;						// �\���ʒu�i������P�j�ł̉��z�}�b�v (�O��100m�Ǝ����̈ʒu)
	TYP_VirtualMap	map2;						// �\���ʒu�i������Q�j�ł̉��z�}�b�v (�O��100m�Ǝ����̈ʒu)

    int				ranp[NUM_MEN][OUTPUT_MESYU_SEKKIN_COL];				// �r�ڋ߃����v�̏��
	int				alarm[NUM_MEN][OUTPUT_MESYU_SEKKIN_COL];			// �r�ڋ߃A���[���̏��
	int				pato[OUTPUT_PATO_POS_NUM][OUTPUT_PATO_KIND_NUM];	// �p�g���C�g�̏��
	int				voice[OUTPUT_VOICE_NUM];							// �{�C�X�̏��
	int				voicedef[NUM_MEN];									// ���׌��o�{�C�X�̏��


	////======================================================
	//// �O����
	//if( ! mcls_pStatus->IsRun() ) return;		// ������~���͌x��o�͂Ȃ�
	//if( DIV_UNTEN_NOMAL != mcls_pStatus->GetUnten() ) return;

	// ���C�����䂪�������̏ꍇ�́A�҂�
	mcls_pLock->BlockTh();


	// ������
	const ParamManager::P_COMMON_MAST& prm = mcls_pParam->GetCommonMast();
	memset(ranp,	em_RAMP_NON,		sizeof(ranp));
	memset(alarm,	em_RAMP_NON,		sizeof(alarm));
	memset(pato,	0x00,				sizeof(pato));
	memset(voice,	em_VOICE_STAT_OFF,	sizeof(voice));
	memset(voicedef,em_VOICE_ERR_NON,	sizeof(voicedef));

	// ���zMAP�쐬
	 Create_VirtualMap(DIV_DISP_POS_DSP_P1, &map, VIRTUALROW, OUTPUT_MESYU_SEKKIN_ROW, OUTPUT_MESYU_SEKKIN_MAX);
	 Create_VirtualMap(DIV_DISP_POS_DSP_P2, &map2, VIRTUALROW, OUTPUT_MESYU_SEKKIN_ROW, OUTPUT_MESYU_SEKKIN_MAX);

	 // �o�̓`�F�b�N
	 // �ݔ��ُ�{�C�X�o�̓`�F�b�N
	 Check_VoiceErr();

	 // �����\���p �����r ���o �֘A�`�F�b�N
	 Check_DetectSekkin();

	//======================================================
	// �x��o�̓f�[�^ �쐬
	//// 
	// --------------------
	// �����v
	//// �͈͓����������� �r�ڋߑΏۂ����邩�m�F
	 if (0 != prm.nKizuLamp[0] || 0 != prm.nKizuLamp[1])
	 {																				// �o�͖���
		 sIndex = VIRTUALROW - (prm.nKizuLamp[1] / OUTPUT_MESYU_SEKKIN_ROW);		// ������̉�m��܂ŏo�́H
		 eIndex = VIRTUALROW + (prm.nKizuLamp[0] / OUTPUT_MESYU_SEKKIN_ROW);		// ������̉�m��O����o�́H

		 for (int men = 0; men < NUM_MEN; men++)
		 {
			 for (int idx = sIndex; idx <= eIndex; idx++)
			 {
				 // �f�[�^�`�F�b�N�i������P�j
				 rec = map.ary[idx].rec;
				 row = map.ary[idx].row;
				 if (-1 == rec || -1 == row)		continue;		// �Ώۃf�[�^�Ȃ�

				 COIL_INF const* pCoil = mcls_pCoil->GetCoilRec(rec);
				 if (NULL == pCoil)				continue;							// �R�C�������B
				 if (pCoil->bAlarmCancel)		continue;							// �x��L�����Z����
				 if (pCoil->bCoilInit)          continue;							// �����J�n�P�{�ڎ�
				 if (!Check_NoLamp(rec, row, OUTPUT_MESYU_SEKKIN_ROW))  continue;	// �n�ړ_�O��̕s����

				 for (int col = 0; col < OUTPUT_MESYU_SEKKIN_COL; col++)
				 {
					 if (!pCoil->alarme.ranp[men][row][col].bUmu) continue;			// �f�[�^����

					 // �����v�_��
					 if (ranp[men][col] < pCoil->alarme.ranp[men][row][col].nKind)
					 {
						 //LOG(em_INF), "[%s] �r�ڋ߃����v���o [%s �rNo:%d ��:%d �r��:%d ���苗��:%d �x����:%d]", my_sThreadName,
							// pCoil->data.cCoilNoIn,
							// pCoil->alarme.ranp[men][row][col].nKizuNo,
							// pCoil->alarme.ranp[men][row][col].nMen,
							// pCoil->alarme.ranp[men][row][col].nTid,
							// pCoil->alarme.ranp[men][row][col].nLen,
							// pCoil->alarme.ranp[men][row][col].nKind);
						 ranp[men][col] = pCoil->alarme.ranp[men][row][col].nKind;	// �Ώۋ�ԓ��Ƀ����v�_���r��1���ȏ�L��
					 }
				 }
			 }
		 }
	 }

	// �A���[��
	//// �͈͓����������� �r�ڋߑΏۂ����邩�m�F
	 if (0 != prm.nKizuAlarm[0] || 0 != prm.nKizuAlarm[1])
	 {																				// �o�͖���
		 sIndex = VIRTUALROW - (prm.nKizuAlarm[1] / OUTPUT_MESYU_SEKKIN_ROW);		// ������̉�m��܂ŏo�́H
		 eIndex = VIRTUALROW + (prm.nKizuAlarm[0] / OUTPUT_MESYU_SEKKIN_ROW);		// ������̉�m��O����o�́H

		 for (int men = 0; men < NUM_MEN; men++)
		 {
			 for (int idx = sIndex; idx <= eIndex; idx++)
			 {
				 // �f�[�^�`�F�b�N�i������Q�j
				 rec = map2.ary[idx].rec;
				 row = map2.ary[idx].row;
				 if (-1 == rec || -1 == row)	continue;							// �Ώۃf�[�^�Ȃ�

				 COIL_INF const* pCoil = mcls_pCoil->GetCoilRec(rec);
				 if (NULL == pCoil)				continue;							// �R�C�������B
				 if (pCoil->bAlarmCancel)		continue;							// �x��L�����Z����
				 if (pCoil->bCoilInit)          continue;							// �����J�n�P�{�ڎ�
				 if (!Check_NoAlarm(rec, row, OUTPUT_MESYU_SEKKIN_ROW))  continue;	// �n�ړ_�O��̕s����

				 for (int col = 0; col < OUTPUT_MESYU_SEKKIN_COL; col++)
				 {
					 if (!pCoil->alarme.ranp[men][row][col].bUmu) continue;			// �f�[�^����

																					// �����v�_��
					 if (alarm[men][col] < pCoil->alarme.ranp[men][row][col].nKind)
					 {
						 //LOG(em_INF), "[%s] �r�ڋ߃A���[�����o [%s �rNo:%d ��:%d �r��:%d ���苗��:%d �x����:%d]", my_sThreadName,
							// pCoil->data.cCoilNoIn,
							// pCoil->alarme.ranp[men][row][col].nMen,
							// pCoil->alarme.ranp[men][row][col].nTid,
							// pCoil->alarme.ranp[men][row][col].nLen,
							// pCoil->alarme.ranp[men][row][col].nKind);
						alarm[men][col] = pCoil->alarme.ranp[men][row][col].nKind;	// �Ώۋ�ԓ��Ƀ����v�_���r��1���ȏ�L��
					 }
				 }
			 }
		 }
	 }

	// --------------------
	// �p�g���C�g
	int nTotalPatoCnt = 0;								// �p�g���C�g�o�͒�����
	std::list<PATO_INF*>::iterator		itr;			// �C�e���[�^
	for(int men =0; men<NUM_MEN; men++) {
		eIndex = VIRTUALROW + (prm.nKizuLamp[0] / OUTPUT_MESYU_SEKKIN_ROW);		// ������̉�m��O����o�́H
		rec = map.ary[eIndex].rec;
		row = map.ary[eIndex].row;
		if (-1 == rec || -1 == row)		continue;		// �Ώۃf�[�^�Ȃ�

		COIL_INF const* pCoil = mcls_pCoil->GetCoilRec(rec);
		if (NULL == pCoil)				continue;		// �R�C�������B
		if (pCoil->bAlarmCancel)		continue;		// �x��L�����Z����
		if (pCoil->bCoilInit)			continue;		// �����J�n�P�{�ڎ�
		//if (!Check_NoAlarm(rec, row, OUTPUT_MESYU_SEKKIN_ROW))  continue;		// �n�ړ_�O��̕s���т̓��X�g�ɒǉ����Ƀ`�F�b�N����

		int nPatoCnt = 0;
		for( itr = mlst_AlmBuf[men].begin(); itr != mlst_AlmBuf[men].end(); itr++ ) {
			PATO_INF* pInf = *itr;						// �g���₷���悤��
			if(pInf == NULL)			continue;		// ��΂ɋN����Ȃ��͂������O�̂���

			// ���Â��H
			int nAns = memcmp(pCoil->cKizukenNo, pInf->cKizukenNo, SIZE_KIZUKEN_NO);
			if( ( 0 < nAns) || ( 0== nAns && row >= pInf->rec) ) {
				nTotalPatoCnt++;
				nPatoCnt++;
				pInf->bRun = true;
			}
		}
		pato[men][0] = pato[men][1] = nPatoCnt;
	}
	m_nPato = nTotalPatoCnt;

	// �����p�g���C�gON
	if (m_bPatoManualOn) {
		if (0 == m_nPato) {	// �p�g���C�g��OFF�̂Ƃ��̂ݎ��{
			m_nPato = 1;
			pato[0][em_PATO_KIND_BUZZ] = pato[1][em_PATO_KIND_BUZZ] = m_nPato;
		}
	}

	// --------------------
	// ���׌��o�{�C�X
	//// �͈͓����������� �r�ڋߑΏۂ����邩�m�F
	if (0 != prm.nVoicePos) {
		eIndex = VIRTUALROW + (prm.nVoicePos / OUTPUT_MESYU_SEKKIN_ROW);		// ������̉�m��O����o�́H

		for (int men = 0; men<NUM_MEN; men++) {
			// �f�[�^�`�F�b�N
			rec = map.ary[eIndex].rec;
			row = map.ary[eIndex].row;
			if (-1 == rec || -1 == row)		continue;		// �Ώۃf�[�^�Ȃ�

			COIL_INF const* pCoil = mcls_pCoil->GetCoilRec(rec);
			if (NULL == pCoil)				continue;		// �R�C�������B
			if (pCoil->bAlarmCancel)		continue;		// �x��L�����Z����
			if (pCoil->bCoilInit)			continue;		// �����J�n�P�{�ڎ�
			if (!Check_NoLamp(rec, row, OUTPUT_MESYU_SEKKIN_ROW))  continue;		// �n�ړ_�O��̕s����

			if (pCoil->alarme.voice[men][row].bUmu) {
				// �͈͓��̍ł��D��x�������{�C�X���o��
				if (GetVoicePriType((ENUM_VOICE_ERR_TYPE)voicedef[men])
					< GetVoicePriType((ENUM_VOICE_ERR_TYPE)pCoil->alarme.voice[men][row].nKind)) {
					voicedef[men] = pCoil->alarme.voice[men][row].nKind;
					double dLen = mcls_pPlg->GetLen_All(DIV_DISP_POS_DSP_P1);
					LOGA(em_MSG), "[%s] ���׌��o�{�C�X��� [men:%d id:%d len:%.0f]", my_sThreadName, men, voicedef[men], dLen);
				}
			}
		}
	}
	//======================================================
	// �f�[�^�Z�b�g
	// --------------------
	// �����v
	for (int men = 0; men < NUM_MEN; men++) {
		for (int col = 0; col < OUTPUT_MESYU_SEKKIN_COL; col++) {
			int type = ranp[men][col];
			if(em_RAMP_NON == type) continue;
			p->ranp[men][col][type] = em_RAMP_STAT_ON;
		}
	}
	// �A���[��
	for (int men = 0; men < NUM_MEN; men++) {
		for (int col = 0; col < OUTPUT_MESYU_SEKKIN_COL; col++) {
			int type = alarm[men][col];
			if (em_RAMP_NON == type) continue;
			p->alarm[men][col][type] = em_RAMP_STAT_ON;
		}
	}

	// --------------------
	// �p�g���C�g
	memcpy(p->pato, pato, sizeof(p->pato));

	// --------------------
	// �{�C�X
	// �ʏ�^�]���[�h�̂݋��L�������ɃZ�b�g
	if (DIV_UNTEN_NOMAL == mcls_pStatus->GetUnten()) {
		//// �ݔ��ُ�{�C�X
		if (em_VOICE_ERR_NON != m_nVoicErrNo) {
			voice[m_nVoicErrNo] = em_VOICE_STAT_ON;
			//LOGA(em_MSG), "[%s] �ݔ��ُ�{�C�X�o�͐ݒ� [id:%d]", my_sThreadName, m_nVoicErrNo);
		}
		//// ���׌��o�{�C�X
		////else if (em_VOICE_ERR_NON != m_nVoicDetectNo) {
		////	int men = 0 == m_nVoicDetectTorB ? em_VOICE_DEF_TOP : em_VOICE_DEF_BOT;
		////	int id = GetVoiceDefType((ENUM_VOICE_PRI_TYPE)m_nVoicDetectNo);
		////	voice[men] = em_VOICE_STAT_ON;
		////	voice[id] = em_VOICE_STAT_ON;
		////}
		else if (0 != m_nVoicDetectNo && (em_VOICE_ERR_NON != voicedef[0] || em_VOICE_ERR_NON != voicedef[1])) {
			int men = GetVoicePriType((ENUM_VOICE_ERR_TYPE)voicedef[0]) >= GetVoicePriType((ENUM_VOICE_ERR_TYPE)voicedef[1]) ? em_VOICE_DEF_TOP : em_VOICE_DEF_BOT;
			int id = max(voicedef[0], voicedef[1]);
			voice[men]	= em_VOICE_STAT_ON;
			voice[id]	= em_VOICE_STAT_ON;
			LOGA(em_MSG), "[%s] ���׌��o�{�C�X�o�͐ݒ� [men:%d id:%d]", my_sThreadName, men - em_VOICE_DEF_TOP, id);
		}
		else
		{
			//voice[em_VOICE_ERR_NON] = em_VOICE_STAT_ON;
		}

		memcpy(&p->voice, &voice, sizeof(p->voice));
	}
}

// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// �p�g���C�g

//------------------------------------------
// ������
//------------------------------------------
void AlarmeManager::Init_Pato()
{
	AutoLock autolock(&m_csLock);

	// ���C�����䂪�������̏ꍇ�́A�҂�
	mcls_pLock->BlockTh();


	//// �o�b�t�@������
	std::list<PATO_INF*>::iterator		itr;					// �C�e���[�^
	for (int ii = 0; ii<NUM_MEN; ii++) {
		for (itr = mlst_AlmBuf[ii].begin(); itr != mlst_AlmBuf[ii].end(); ) {
			//if( 0==(*itr)) {			// ����̗v�f�����Ȃ炱��IF����L��������	
			if ((*itr) == NULL)			continue;		// ��΂ɋN����Ȃ��͂������O�̂���		

			OpSendFunc::SendOp_PatoReset((*itr)->cKizukenNo, (*itr)->nMen, (*itr)->nKizuSeq);	// �����g�̃L�����Z���̂�
			SAFE_DELETE(*itr);						// �̈�J��
			itr = mlst_AlmBuf[ii].erase(itr);		// �v�f�폜
			continue;
			//}
			//itr++;
		}
	}

	////// �ُ����
	int id[2];
	id[0] = mcls_pStatus->GetStatusRead().PatoNg[0].id;
	id[1] = mcls_pStatus->GetStatusRead().PatoNg[1].id;
	mcls_pStatus->SetStatusQue(id[0], true, false);
	mcls_pStatus->SetStatusQue(id[1], true, false);
#ifdef DEBUG_PATO_LOG
	LOGA(em_MSG), "[%s] ���ײ��r������", my_sThreadName);
	LOGG(em_MSG), "[%s] ���ײ��r������", my_sThreadName );
#endif
}

//------------------------------------------
// �p�g���C�g�r �ǉ� & F���s���у`�F�b�N (��HYRecv)
// char const* cKizukenNo �r���Ǘ�No
// int nMen ��
// int nKizuSeq �r�A��
// long y �r���S�̒���ʒu [mm]
// �߂�l �F �\���փp�g���C�g�r�ʒm��
//------------------------------------------
bool AlarmeManager::Add_Pato(char const* cKizukenNo, int nMen, int nKizuSeq, long y)
{
	AutoLock autolock(&m_csLock);

	// ���C�����䂪�������̏ꍇ�́A�҂�
	mcls_pLock->BlockTh();

	int rec = y / OUTPUT_MESYU_SEKKIN_ROW;		// 500mm�Ԋu�̃��R�[�h (�x��o�͗p�̓������b�V���̔z��ʒu)


	//================================================
	// �s���у`�F�b�N (�����ł́AF�������ł��Ȃ��BT�����R�C�����m�莞�ɍs��)
	const ParamManager::P_COMMON_MAST& prm = mcls_pParam->GetCommonMast();
	// ��[���̕s���тɓ����Ă�
	if (prm.nNoLamp[0] > y) {
		// ��		LOG(em_WAR), "[%s] �p�g���C�g�Ώ��r �s����(F)�B�j��[%s,�\��=%d,�r�A��=%d,%0.3fm]", my_sThreadName, cKizukenNo, nMen, nKizuSeq, y/1000.0);
		return false;
	}

	//================================================
	// �o�^
	int nCnt = (int)mlst_AlmBuf[nMen].size();

	if (nCnt < MAX_DEFLIST_BUF) {
		// ���X�g�ɓo�^
		PATO_INF*	pInf = new PATO_INF;
		memcpy(pInf->cKizukenNo, cKizukenNo, SIZE_KIZUKEN_NO);
		pInf->nMen = nMen;
		pInf->nKizuSeq = nKizuSeq;
		pInf->rec = rec;
		pInf->nY = y;
		pInf->bRun = false;
		mlst_AlmBuf[nMen].push_back(pInf);
#ifdef DEBUG_PATO_LOG
		LOGA(em_MSG), "[%s] ���ײ��r�ǉ� (%s)(men=%d)(KizuSeq=%d)(Y=%d)", my_sThreadName, pInf->cKizukenNo, pInf->nMen, pInf->nKizuSeq, pInf->nY );
		LOGG(em_MSG), "[%s] ���ײ��r�ǉ� (%s)(men=%d)(KizuSeq=%d)(Y=%d)", my_sThreadName, pInf->cKizukenNo, pInf->nMen, pInf->nKizuSeq, pInf->nY );
#endif
		return true;

	}
	else {
		// �o�b�t�@�t�� (�󂫗L�聨�����ω���)
		// ��		LOG(em_WAR), "[%s] �p�g���C�g�Ώ��r�L���[�t���B�j��[%s,�\��=%d,�r�A��=%d,%0.3fm]", my_sThreadName, cKizukenNo, nMen, nKizuSeq, y/1000.0);
		if (1 != mcls_pStatus->GetStatusRead().PatoNg[nMen].stat) {
			// �X�e�[�^�X�X�V
			int id = mcls_pStatus->GetStatusRead().PatoNg[nMen].id;
			mcls_pStatus->SetStatusQue(id, false);
#ifdef DEBUG_PATO_LOG
			LOGA(em_ERR), "[%s] ���ײ��r�ǉ��L���[�t�� (%s)(men=%d)(KizuSeq=%d)(Y=%d)", my_sThreadName, cKizukenNo, nMen, nKizuSeq, y  );
			LOGG(em_ERR), "[%s] ���ײ��r�ǉ��L���[�t�� (%s)(men=%d)(KizuSeq=%d)(Y=%d)", my_sThreadName, cKizukenNo, nMen, nKizuSeq, y  );
#endif
		}
//#ifdef DEBUG_PATO_LOG
//		LOG(em_MSG), "���ײ��r�ǉ��L���[�t�� (%s)(men=%d)(KizuSeq=%d)", cKizukenNo, nMen, nKizuSeq );
//		LOGG(em_MSG), "���ײ��r�ǉ��L���[�t�� (%s)(men=%d)(KizuSeq=%d)", cKizukenNo, nMen, nKizuSeq );
//#endif
		return false;
	}
}



// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ���ʏ���

//------------------------------------------
// �\���ʒu����_�Ƃ��āA�O��100m�̃}�b�v�����
// int pos �e�\���ʒu�C���f�b�N�X (0�I���W��)
// TYP_VirtualMap& map[201] �̃}�b�v
// int nMapMax VirtualMap�̍ő�s���̔��� (���ۂ́AVIRTUALROW*2+1 �ƂȂ�)
// int nMesyu ���b�V���� [mm]
// int nMax	�ő�R�C���s��
//------------------------------------------
void AlarmeManager::Create_VirtualMap(int pos, TYP_VirtualMap* map, int nMapMax, int nMesyu, int nRowMax)
{
	int ii;

	// �܂��͏�����
	for(ii=0; ii<(nMapMax*2+1); ii++) {
		map->ary[ii].rec = -1;
		map->ary[ii].row = -1;
	}

	//======================================================
	// �������`�F�b�N
	const COIL_INF*  pCoilBase = mcls_pCoil->GetCoilAll(pos);
	double	dLen = mcls_pPlg->GetLen_All(pos) ;
	if( NULL == pCoilBase ) return;					// �e�\���ʒu�ɖ����B���́A����ŏI��
	if( dLen >= COIL_LEN_MAX*1000.0 ) return;		// ����z���́A�g���b�L���O���������̂őΏۊO�ɂ��Ă����B

	//// �K���O��1�R�����R�C�����ׂ��Ȃ��� 3���R�[�h�������肦�Ȃ�
	int nRec[3];					// �e�ʒu�̃��R�[�h    0:1�O(������)  1:�\����_�̃R�C��  2:��(�㗬��) 
	const COIL_INF*  pCoil[3];		// �e�ʒu�̃R�C�����
	// �\����_�̃R�C��
	pCoil[1] = pCoilBase;
	nRec[1] = pCoil[1]->rec;				
	// �O��̃R�C��
	nRec[0] = mcls_pCoil->GetRecIndex(nRec[1], 1);
	nRec[2] = mcls_pCoil->GetRecIndex(nRec[1], -1);
	pCoil[0] = mcls_pCoil->GetCoilRec(nRec[0]);
	pCoil[2] = mcls_pCoil->GetCoilRec(nRec[2]);

	int nRow[2];				// �e�R�C���̍ő�s��
	nRow[0] = pCoil[0]->nEndMode != 0 ? pCoil[0]->nLength / nMesyu : nRowMax;
	nRow[1] = pCoil[1]->nEndMode != 0 ? pCoil[1]->nLength / nMesyu : nRowMax;


	//// �e�\���ʒu��ʉ߂��Ă���z��ʒu���Z�o
	int maprow;
	int valRec;
	int valRow;
	int row = (int)(dLen / nMesyu);				// �e�\���ʒu��0�Ƃ��ĉ��}�X���i��ł��邩 (0�I���W��)
	for(ii=nMapMax; ii>=-nMapMax; ii--) {								// �㗬[200]���牺��[0]�Ɍ������ď��ɃZ�b�g���Ă���
		valRec = nRec[1];
		valRow = row+ii;

		// 1�O�̃R�C����
		if( valRow < 0 ) {	
			valRec = nRec[0];
			valRow = nRow[0] + (valRow+1);		// valRow=-1�̂Ƃ��́A�O�R�C���̈�ԍŌ�B�܂�AnRow[0]����-0�ڂ̔z��ʒu�ƂȂ�
			if(valRow < 0) break;				// �O�̑O�܂ōs��������� (���肦�Ȃ�)	

		// ���R�C����
		} else if( valRow > nRow[1] ) {
			valRec = nRec[2];
			valRow = (valRow-1) - nRow[1];		// nRow[1]=500,valRow=501�̎��́A���R�C���̐擪�B�܂�AnRow[2]��0�Ԗڂ̔z��ƂȂ�
			if(valRow < 0) break;			
		
		// ���R�C����
		} else {
			// �������Ȃ�
		}

		// �f�[�^�Z�b�g
		maprow = nMapMax + ii;
		map->ary[maprow].rec = valRec;
		map->ary[maprow].row = valRow;

		if (nRowMax < map->ary[maprow].row) map->ary[maprow].row = nRowMax;
	}
}


//------------------------------------------
// �s���є͈͓����`�F�b�N
// int rec �`�F�b�N����R�C����񃌃R�[�h
// int row �`�F�b�N����s�ʒu (0�I���W��)
// int nMesyu ���b�V���� [mm]
// �߂�l false:�s����(�x��֎~) true:�ʏ�
//------------------------------------------
bool AlarmeManager::Check_NoLamp(int rec, int row, int nMesyu)
{
	const ParamManager::P_COMMON_MAST& prm = mcls_pParam->GetCommonMast();

	// ��[���̕s���тɓ����Ă�
	if ((prm.nNoLamp[0] / nMesyu) > row) return false;

	// ���[
	const COIL_INF*  pCoil = mcls_pCoil->GetCoilRec(rec);
	if (0 == pCoil->nEndMode) {		// �܂��ʔ�
		return true;
	}
	else {
		int nRowEnd = (pCoil->nLength / nMesyu);
		// ���[���̕s���тɓ����Ă�
		if (nRowEnd - (prm.nNoLamp[1] / nMesyu) < row) return false;
	}
	return true;
}
bool AlarmeManager::Check_NoAlarm(int rec, int row, int nMesyu)
{
	const ParamManager::P_COMMON_MAST& prm = mcls_pParam->GetCommonMast();

	// ��[���̕s���тɓ����Ă�
	if ((prm.nNoAlarm[0] / nMesyu) > row) return false;

	// ���[
	const COIL_INF*  pCoil = mcls_pCoil->GetCoilRec(rec);
	if (0 == pCoil->nEndMode) {		// �܂��ʔ�
		return true;
	}
	else {
		int nRowEnd = (pCoil->nLength / nMesyu);
		// ���[���̕s���тɓ����Ă�
		if (nRowEnd - (prm.nNoAlarm[1] / nMesyu) < row) return false;
	}
	return true;
}

//------------------------------------------
// ���׌��o�x����� (�� MI)
// char const* cKizukenNo �\�ʌ����� �ʔ��̃R�C��
//------------------------------------------
void AlarmeManager::AlarmCancel_Pato(char const* cKizukenNo)
{
	AutoLock autolock(&m_csLock);

	// ���C�����䂪�������̏ꍇ�́A�҂�
	mcls_pLock->BlockTh();

	//// �ΏۃR�C������O�͂��ׂăL�����Z��
	// �T�[�`
	std::list<PATO_INF*>::iterator		itr;					// �C�e���[�^
	for (int ii = 0; ii<NUM_MEN; ii++) {
		for (itr = mlst_AlmBuf[ii].begin(); itr != mlst_AlmBuf[ii].end(); ) {
			PATO_INF* pInf = *itr;						// �g���₷���悤��
			if (pInf == NULL)			continue;		// ��΂ɋN����Ȃ��͂������O�̂���		

														// �ΏہH
			if (0 <= memcmp(cKizukenNo, pInf->cKizukenNo, SIZE_KIZUKEN_NO)) {

				// ����OFF�ʒm
				OpSendFunc::SendOp_PatoReset(pInf->cKizukenNo, pInf->nMen, pInf->nKizuSeq);
				LOGA(em_WAR), "[%s] �p�g���C�g�Ώ��r �x������B�j��[%s,�\��=%d,�r�A��=%d, %d]", my_sThreadName, pInf->cKizukenNo, pInf->nMen, pInf->nKizuSeq, pInf->nY );
				LOGG(em_WAR), "[%s] �p�g���C�g�Ώ��r �x������B�j��[%s,�\��=%d,�r�A��=%d, %d]", my_sThreadName, pInf->cKizukenNo, pInf->nMen, pInf->nKizuSeq, pInf->nY );

				// �v�f�폜		
				SAFE_DELETE(pInf);						// �̈�J��
				itr = mlst_AlmBuf[ii].erase(itr);		// �v�f�폜
				continue;
			}
			itr++;
		}

		//// ���Z�b�g�Y��
		// �ُ�K�C�_���X����
		if (0 != mcls_pStatus->GetStatusRead().PatoNg[ii].stat) {
			int id = mcls_pStatus->GetStatusRead().PatoNg[ii].id;
			mcls_pStatus->SetStatusQue(id, true, false);
		}

	}
}

//------------------------------------------
// ���Z�b�gPB���� (��MI)
//------------------------------------------
void AlarmeManager::Reset_Pato()
{
	AutoLock autolock(&m_csLock);

	// ���C�����䂪�������̏ꍇ�́A�҂�
	mcls_pLock->BlockTh();

	//// �f�[�^�L���`�F�b�N
	int nCnt = 0;
	for (int ii = 0; ii<NUM_MEN; ii++) nCnt += (int)mlst_AlmBuf[ii].size();
	if (0 == nCnt) return;


	//// �ŌÂ����
	// �r�ʒu���ł��Ⴂ���
	bool bOk = false;											// �����Z�b�g�����ς݃t���O
	std::list<PATO_INF*>::iterator		itr;					// �C�e���[�^
	std::list<PATO_INF*>::iterator		itrOld;					// �ŌÏ��

	for (int ii = 0; ii<NUM_MEN; ii++) {
		for (itr = mlst_AlmBuf[ii].begin(); itr != mlst_AlmBuf[ii].end(); itr++) {
			if ((*itr) == NULL)continue;						// ��΂ɋN����Ȃ��͂������O�̂���
			if (!(*itr)->bRun) continue;						// �p�g���C�g�o�͒��r�̂� 
			bool bChange = false;

			if (!bOk) {
				bChange = true;
			}
			else {
				// ���Â��H
				int nAns = memcmp((*itrOld)->cKizukenNo, (*itr)->cKizukenNo, SIZE_KIZUKEN_NO);
				if ((0 < nAns) ||
					(0 == nAns && (*itrOld)->nY > (*itr)->nY)) {
					bChange = true;
				}
			}

			// ����ւ�
			if (bChange) {
				bOk = true;
				itrOld = itr;
			}
		}
	}

	//// �ŌÂ��r�ɑ΂��ď���
	if (bOk) {
		// �폜�ʒm // �� �����B�r�摜�͎c�����߁A�L�����Z���𑗂�
		OpSendFunc::SendOp_PatoReset((*itrOld)->cKizukenNo, (*itrOld)->nMen, (*itrOld)->nKizuSeq);
#ifdef DEBUG_PATO_LOG
		LOGA(em_MSG), "[%s] ���ײ��r�폜 (%s)(men=%d)(KizuSeq=%d)(Y=%d)", my_sThreadName, (*itrOld)->cKizukenNo, (*itrOld)->nMen, (*itrOld)->nKizuSeq, (*itrOld)->nY  );
		LOGG(em_MSG), "[%s] ���ײ��r�폜 (%s)(men=%d)(KizuSeq=%d)(Y=%d)", my_sThreadName, (*itrOld)->cKizukenNo, (*itrOld)->nMen, (*itrOld)->nKizuSeq, (*itrOld)->nY  );
#endif

		// �v�f�폜
		int nMen = (*itrOld)->nMen;
		SAFE_DELETE(*itrOld);						// �̈�J��
		mlst_AlmBuf[nMen].erase(itrOld);

		// �ُ�K�C�_���X����
		if (0 != mcls_pStatus->GetStatusRead().PatoNg[nMen].stat) {
			int id = mcls_pStatus->GetStatusRead().PatoNg[nMen].id;
			mcls_pStatus->SetStatusQue(id, true, false);
#ifdef DEBUG_PATO_LOG
			LOGA(em_MSG), "[%s] ���ײ��r�ǉ��L���[�t������", my_sThreadName );
			LOGG(em_MSG), "[%s] ���ײ��r�ǉ��L���[�t������", my_sThreadName );
#endif
		}
	}
}
//
////------------------------------------------
//// �p�g���C�g�o�͂̃`�F�b�N (�� DMemManager)
//// char const* cKizukenNo �\�ʌ����� �ʔ��̃R�C��
//// int rec 1m�P�ʂ̃��R�[�h�ʒu
//// ���A��� �p�g���C�g�o�͑Ώی�
////------------------------------------------
//int AlarmeManager::OutPut_Pato(char const* cKizukenNo, int rec)
//{
//	AutoLock autolock(&m_csLock);
//
//	int nCnt = 0;								// �p�g���C�g�o�͒�����
//	//	int rec  = y / OUTPUT_MESYU_VAL_ROW;		// 1m�Ԋu�̃��R�[�h (�x��o�͗p�̓������b�V���̔z��ʒu)
//
//
//	// �p�g���C�g�o��
//	std::list<PATO_INF*>::iterator		itr;					// �C�e���[�^
//	for (int ii = 0; ii<NUM_MEN; ii++) {
//		for (itr = mlst_AlmBuf[ii].begin(); itr != mlst_AlmBuf[ii].end(); itr++) {
//			PATO_INF* pInf = *itr;						// �g���₷���悤��
//
//// ���Â��H
//			int nAns = memcmp(cKizukenNo, pInf->cKizukenNo, SIZE_KIZUKEN_NO);
//			if ((0 < nAns) ||
//				(0 == nAns && rec >= pInf->rec)) {
//				nCnt++;
//				pInf->bRun = true;
//			}
//		}
//	}
//
//	return nCnt;
//}



//------------------------------------------
// �ݔ��ُ�{�C�X�o�̓`�F�b�N
//------------------------------------------
void AlarmeManager::Check_VoiceErr()
{
	int		no = em_VOICE_ERR_NON;		// �ݔ��ُ�No
	int		ii;

	// �@���Ԏ擾
	TYP_STATUS_READ			typStatus;
	TYP_STATUS_NOWSYSTEM	typNowSys;
	memcpy(&typStatus, (TYP_STATUS_READ*)&mcls_pStatus->GetStatusRead(),	sizeof(TYP_STATUS_READ));
	memcpy(&typNowSys, (TYP_STATUS_NOWSYSTEM*)&mcls_pStatus->GetStatusNowSystem(), sizeof(TYP_STATUS_NOWSYSTEM));


	////// �ݔ��ُ��ԃ`�F�b�N
	// �E���ݔ��ُ킪�������ڂ����������ꍇ�́A����ւ���
	// �E�ُ�����ɂȂ�܂ŏo�����ςȂ�
	
	// �{�C�X�o��2 �R�C����񂪎�M�ł��܂���B
	if(em_VOICE_ERR_NON == no) {
		if( 0 != typStatus.Coil_Ken.stat ||
			0 != typStatus.Coil_Dsp.stat   ) {
			no = em_VOICE_ERR_RECV_COILINF;
		}	
	}
	
	// �{�C�X�o��3 �r�����ё��M�ُ�ł��B
	if(em_VOICE_ERR_NON == no) {
		if( 0 != typStatus.ZiProCreateErr.stat ||
			0 != typStatus.ZiFfCreateErr.stat   ) {
			no = em_VOICE_ERR_SEND_ZISEKI;
		}	
	}

	// �{�C�X�o��4 �Ɩ����u�ُ�ł��B
	if(em_VOICE_ERR_NON == no) {
		for(ii=0; ii<NUM_MEN; ii++) {
			if (0 != typStatus.LedRunOff[ii].stat) {
				no = em_VOICE_ERR_LIGHT;
				break;
			}
			for(int jj=0; jj<NUM_LIGHT; jj++){
				if (0 != typStatus.led.Connect[ii][jj].stat) {
					no = em_VOICE_ERR_LIGHT;
					break;
				}
			}
			for(int jj=0; jj<DIV_LED_COLOR_END; jj++){
				if (0 != typStatus.led.ch.Break[ii][jj].stat ||
					0 != typStatus.led.ch.TempWar[ii][jj].stat ||
					0 != typStatus.led.ch.TempErr[ii][jj].stat ) {
					no = em_VOICE_ERR_LIGHT;
					break;
				}
			}
		}
	}
	
	// �{�C�X�o��5 �J�����ُ�ł��B
	if(em_VOICE_ERR_NON == no) {
		for(ii=0; ii<NUM_CAMERA; ii++) {
			if( 0 != typStatus.CamGrab[ii].stat) {
				no = em_VOICE_ERR_CAM;
				break;
			}
		}
	}

	// �{�C�X�o��6 ���x�ُ�ł��B
	if(em_VOICE_ERR_NON == no) {
		// ����
		for(ii=0; ii<NUM_ALL_HANTEI; ii++) {
			if( 0 != typStatus.EvtLog_Ht[ii].stat ) {
				no = em_VOICE_ERR_TEMP;
				break;
			}
		}
		
		// ���̑�PC
		if (0 != typStatus.To.EvtLog.stat ||
			0 != typStatus.Bk.EvtLog.stat ||
			0 != typStatus.Rpe[0].EvtLog.stat ||
			0 != typStatus.Rpe[1].EvtLog.stat ||
			0 != typStatus.Rpe[2].EvtLog.stat ||
			0 != typStatus.Hy[0].EvtLog.stat ||
			0 != typStatus.Hy[1].EvtLog.stat ||
			0 != typStatus.So.EvtLog.stat) {
			no = em_VOICE_ERR_TEMP;
		}	

		// �摜�����Փ��̉��x�ُ펞
		for (int num=0; num<NUM_TEMPMT_UNIT; num++)
		{
			if (0 != typStatus.tempmt[0].unit.TempSt[num].stat)
			{
				no = em_VOICE_ERR_TEMP;
				break;
			}
		}
	}

	// �{�C�X�o��1 �r���ُ�ł��B (��ԗD��x�Ⴂ)
	if(em_VOICE_ERR_NON == no) {
		for(ii=0; ii<NUM_MEN; ii++) {
			if( DIV_KIKI_JYU == typNowSys.kiki[ii] ) {
				no = em_VOICE_ERR_KEN;		// ���̑��d�̏�
				break;
			}
		}
	}

	////// �ݔ��ُ�{�C�X�o�̓L�����Z�� �m�F
	if(em_VOICE_ERR_NON == no) {
		// �ݔ��ُ�{�C�X�o�̓L�����Z�� �� ���� ���
		if(m_bVoicErrCancel) {
			LOGA(em_MSG), "[%s] �ݔ��ُ�{�C�X�o�̓L�����Z�� ����", my_sThreadName);
			LOGG(em_MSG), "[%s] �ݔ��ُ�{�C�X�o�̓L�����Z�� ����", my_sThreadName);
			m_bVoicErrCancel = false;
		}
		// �o�̓���
		if(em_VOICE_ERR_NON != m_nVoicErrNo){
			LOGA(em_INF), "[%s] �ݔ��ُ�{�C�X�ύX[%s��%s]", my_sThreadName, GetVoiceTypeName(m_nVoicErrNo), GetVoiceTypeName(no));
			LOGG(em_INF), "[%s] �ݔ��ُ�{�C�X�ύX[%s��%s]", my_sThreadName, GetVoiceTypeName(m_nVoicErrNo), GetVoiceTypeName(no));
			m_nVoicErrNo = em_VOICE_ERR_NON;
		}

	} else if(em_VOICE_ERR_NON != no && m_bVoicErrCancel ) {
		// �ݔ��ُ�{�C�X�o�̓L�����Z���ɂ��{�C�X�o�͂�����
		if(em_VOICE_ERR_NON != m_nVoicErrNo) m_nVoicErrNo = em_VOICE_ERR_NON;

	} else if(no != m_nVoicErrNo) {
		LOGA(em_INF), "[%s] �ݔ��ُ�{�C�X�ύX[%s��%s]", my_sThreadName, GetVoiceTypeName(m_nVoicErrNo), GetVoiceTypeName(no));
		LOGG(em_INF), "[%s] �ݔ��ُ�{�C�X�ύX[%s��%s]", my_sThreadName, GetVoiceTypeName(m_nVoicErrNo), GetVoiceTypeName(no));
		m_nVoicErrNo = no;			// �{�C�X�o�͕ύX
	}
}


//------------------------------------------
// �r�ڋߔ���
//------------------------------------------
void AlarmeManager::Check_DetectSekkin()
{
	//// ������~���͌x��o�͂Ȃ�
	if( ! mcls_pStatus->IsRun() ) {
		//memset(m_nVoicFastSet, 0x00, sizeof(m_nVoicFastSet));
		m_nVoicDetectNo = 0;
		m_nVoicDetectTorB = 0;
		m_nPato = 0;
		return;
	}

	//// �\�ʌ�����ʒu��ʔ��R�C�����Ώ�
	const COIL_INF * pCoil = mcls_pCoil->GetCoilDsp(0);
	if( NULL == pCoil ) {
		//memset(m_nVoicFastSet, 0x00, sizeof(m_nVoicFastSet));
		m_nVoicDetectNo = 0;
		m_nVoicDetectTorB = 0;
		m_nPato = 0;
		return;
	}

	//// ///////////////////////////////////////////////////////////////////
	//// �p�g���C�g��Ԕc��
	//int pato = 0;
	//COIL_INF const* pCoilDspO = mcls_pCoil->GetCoilDsp(0);
	//if(NULL != pCoilDspO) {
	//	if( ! pCoilDspO->bAlarmCancel ) {
	//		pato = OutPut_Pato(pCoilDspO->cKizukenNo, (int)(mcls_pPlg->GetLen_Dsp(0)/ OUTPUT_MESYU_VOIC_ROW));
	//	}
	//}
	//m_nPato = pato;
	
	
	// ///////////////////////////////////////////////////////////////////
	// �{�C�X��Ԕc��

	//// �� �ʂ̌��׌��o�x��o�͒��̓Z�b�g���Ȃ�
	if(0 != m_tId_Alarme) {
		//memset(m_nVoicFastSet, 0x00, sizeof(m_nVoicFastSet));
		return;
	}

	////// �������� �����o�͑ΏۗL��H
	//if( em_VOICE_PRI_NON == m_nVoicFastSet[0] && em_VOICE_PRI_NON == m_nVoicFastSet[1] ) {
	//	m_nVoicDetectNo = 0;
	//	m_nVoicDetectTorB = 0;	
	//	return;
	//}

	//// ���׌��o�x��������H
	if( pCoil->bAlarmCancel ) {
		//memset(m_nVoicFastSet, 0x00, sizeof(m_nVoicFastSet));
		m_nVoicDetectNo = 0;
		m_nVoicDetectTorB = 0;
		return;
	}

	//// �n�ړ_����̕s���сH
	const ParamManager::P_COMMON_MAST& prm = mcls_pParam->GetCommonMast();
	if( prm.nNoLamp[0] > (int)(mcls_pPlg->GetLen_Dsp(0)) ) {
		//memset(m_nVoicFastSet, 0x00, sizeof(m_nVoicFastSet));
		m_nVoicDetectNo = 0;
		m_nVoicDetectTorB = 0;
		return;
	}

	////// �����܂ł�����A�����r �����{�C�X�o�͑Ώ�
	//m_nVoicDetectNo   = max(m_nVoicFastSet[0], m_nVoicFastSet[1]);
	//if(m_nVoicFastSet[0] ) {
	//	m_nVoicDetectTorB = 0;
	//} else if(m_nVoicFastSet[1] ) {
	//	m_nVoicDetectTorB = 1;
	//}
	//// �����܂ł�����A�����r �����{�C�X�o�͑Ώ�
	m_nVoicDetectNo = 1;
	m_nVoicDetectTorB = 1;

	//// �����o������ ������
	//memset(m_nVoicFastSet, 0x00, sizeof(m_nVoicFastSet));
}
