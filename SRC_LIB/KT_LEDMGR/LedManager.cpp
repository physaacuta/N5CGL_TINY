#include "StdAfx.h"
#include "LedManager.h"

// �������[���[�N���o ----->>>
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif
// <<<------------------------

//------------------------------------------
// �R���X�g���N�^
// char const* cName ���O (ini�t�@�C���̃L�[�ƂȂ�)
// int nId		     ID (0�I���W��) (�Z�N�V�����̉��Ԗ�)
// int nBlock        �z���u���b�N��
// int nTempUnit     LED���x�v����_��
// int nTempThr      ���x臒l [�x]
// int nTempThrDown  ����OFF���x臒l [�x]
// int nTimerCycle   ���x��荞�ݎ��� [�b]
//------------------------------------------
LedManager::LedManager(char const* cName, int nId, int nBlock, int nTempUnit, int nTempThr, int nTempThrDown, int nTimerCycle) :
ThreadManager(cName),
m_nMyID(nId),
m_bIsConnect(false),
m_emMainMode(MODE_NONE),
m_nNowScode(MAX_SCODE),					// �\�ʏ�Ԃ́A�f�t�H���g�\�ʏ�ԂƂ���
m_bSendStatus(true),

//m_nBlock(nBlock),						/// 2014.10.28 ����(���P�xLED�Ή�)
//m_nTempUnit(nTempUnit),				/// 2014.10.28 ����(���P�xLED�Ή�)
m_nTempThr(nTempThr),
m_nTempThrDown(nTempThrDown),
m_nTimerCycle(nTimerCycle)
{
	int      nWk;
	CString  sWk;
	CString  sSession;

	//// �����̐ݒ�L�[������
	m_cMySession.Format("%s_%s", KT_LEDMGR, cName );
	m_nMyMen		= GetPrivateProfileInt(m_cMySession, "MEN",		0, TASKINI_NAME);
	m_nChNum		= GetPrivateProfileInt(m_cMySession, "CH_NUM",	0, TASKINI_NAME);
	/// 2014.10.28 ����(���P�xLED�Ή�)------------------------->>>>
	m_nChStart		= GetPrivateProfileInt(m_cMySession, "CH_START", 1, TASKINI_NAME);				// �擪ch�̕ЖʒP�ʂ�ID(����`�̏ꍇ�A1)
	/// 2014.10.28 ����(���P�xLED�Ή�)<<<<-------------------------
	_ASSERT(0!=m_nChNum);

	/// 2014.10.28 ����(���P�xLED�Ή�)------------------------->>>>
	//// �g�p����CH�̌���
	memset(&m_bUsedCh, 0x00, sizeof(m_bUsedCh));
	int	nChCnt = 0;
	char cWk[10];
	for(int ii=0; ii<MAX_LED_CH; ii++){
		sSession.Format("%s_%s_CH%d", KT_LEDMGR, cName, ii+1);
		GetPrivateProfileSection(sSession, cWk, 10, TASKINI_NAME);

		if( 0 != strlen(cWk)){
			m_bUsedCh[ii] = true;
			nChCnt++;
		}
	}
	_ASSERT(nChCnt >= m_nChNum);

	/// 2018.12.06 ����(�s��C��)------------------------->>>>
	//for(int ii=0; ii<MAX_LED_CH-1; ii++){
	for(int ii=0; ii<MAX_LED_CH; ii++){
	/// 2014.10.28 ����(�s��C��)<<<<-------------------------
		// ���g�pCH�̏ꍇ�͎���
		if( !m_bUsedCh[ii] )	continue;

		sSession.Format("%s_%s_CH%d", KT_LEDMGR, cName, ii+1 );
		m_nBlock[ii]    = GetPrivateProfileInt(sSession, "BLOCK",     nBlock,    TASKINI_NAME);		// �z���u���b�N��(����`�̏ꍇ�A���ʐݒ�̃u���b�N��)
		m_nMyIndex[ii]  = GetPrivateProfileInt(sSession, "INDEX",     ii+1,      TASKINI_NAME);		// �Ή�����Index(����`�̏ꍇ�ACh�ԍ�)
		m_nTempUnit[ii] = GetPrivateProfileInt(sSession, "TEMP_UNIT", nTempUnit, TASKINI_NAME);		// LED���x�v���_��(����`�̏ꍇ�A���ʐݒ��LED���x�v���_��)
		nWk				= GetPrivateProfileInt(sSession, "LR",		  0,		 TASKINI_NAME);		// �Ɠx�␳�l���W�J�t���O(����`�̏ꍇ�A���W�J)
		m_bIsPositiveDeployment[ii] = (0 == nWk ? true : false);
	}
	/// 2014.10.28 ����(���P�xLED�Ή�)<<<<-------------------------

	//// �@��ID���擾
	m_nIdConnect	= GetPrivateProfileInt(m_cMySession, "LED_CONNECT_ID",		0, TASKINI_NAME);		// �Ɩ����u �ʐM���
	for(int ii=0; ii<MAX_LED_CH; ii++) {
		// ���g�pCH�̏ꍇ�͎���
		if( !m_bUsedCh[ii] )	continue;

		sSession.Format("%s_%s_CH%d", KT_LEDMGR, cName, ii+1 );

		m_nIdBreak[ii]		= GetPrivateProfileInt(sSession, "LED_BREAK_ID",	0, TASKINI_NAME);		// �f��	
		m_nIdHotWar[ii]		= GetPrivateProfileInt(sSession, "LED_HOTWAR_ID",	0, TASKINI_NAME);		// ���M���o
		m_nIdHotErr[ii]		= GetPrivateProfileInt(sSession, "LED_HOTERR_ID",	0, TASKINI_NAME);		// ���M�V���b�g�_�E��
		m_nIdOnOff[ii]		= GetPrivateProfileInt(sSession, "LED_ONOFF_ID",	0, TASKINI_NAME);		// LED�_���������
		m_nIdValNow[ii]		= GetPrivateProfileInt(sSession, "LED_VALNOW_ID",	0, TASKINI_NAME);		// ���ݐݒ�P�x�l
		m_nIdTotalTime[ii]	= GetPrivateProfileInt(sSession, "LED_TOTALTIME_ID",0, TASKINI_NAME);		// �ғ��݌v����
		for (int jj=0; jj<MAX_TEMP_UNIT; jj++) {
			sWk.Format("LED_TEMP_ST_%d_ID", jj+1);
			m_nIdTempSt[ii][jj]	= GetPrivateProfileInt(sSession, sWk,			0, TASKINI_NAME);		// ���x���

			sWk.Format("LED_TEMP_%d_ID", jj+1);
			m_nIdTemp[ii][jj]	= GetPrivateProfileInt(sSession, sWk,			0, TASKINI_NAME);		// ���x
		}
	}
	

	//// ������
	for(int ii=0; ii<PARAM_SOCK_MAX; ii++) {
		mcls_pSock[ii]		= NULL;
		m_eStateMain[ii]	= SOCK_NONE;
		m_nNowEventNo[ii]	= 0;
	}
	for(int ii=0; ii<MAX_LED_CH; ii++) {
		m_nNowLedBase[ii]	= 0;
		for (int jj=0; jj<MAX_LED_BLOCK; jj++) {
			m_nNowLedHosei[ii][jj]	= 0;
		}
		m_nStateOnOff[ii]	= -1;
		m_nStateBreak[ii]	= -1;
		m_nStateTemp[ii]	= -1;

		for (int jj=0; jj<MAX_TEMP_UNIT; jj++) {
			m_nTemp[ii][jj]		= 0;
			m_nTempSts[ii][jj]	= -1;
		}
	}
	memset( m_cSockBuf, 0x00, sizeof(m_cSockBuf));

	//// �n���h������
	m_tmSendAnser	= CreateWaitableTimer(NULL, FALSE, NULL);
	m_tmSendOk		= CreateWaitableTimer(NULL, FALSE, NULL);
	m_tmStateCheck	= CreateWaitableTimer(NULL, FALSE, NULL);
	m_tmReCheck		= CreateWaitableTimer(NULL, FALSE, NULL);
	for(int ii=0; ii<MAX_LED_CH; ii++) m_tmAddTime[ii] = CreateWaitableTimer(NULL, FALSE, NULL);
}

//------------------------------------------
// �f�X�g���N�^
//------------------------------------------
LedManager::~LedManager(void)
{
	// �n���h�����
	CloseHandle(m_tmSendAnser);
	CloseHandle(m_tmSendOk);
	CloseHandle(m_tmStateCheck);
	CloseHandle(m_tmReCheck);
	for(int ii=0; ii<MAX_LED_CH; ii++) CloseHandle(m_tmAddTime[ii]);
}


//------------------------------------------
// �X���b�h�J�n�O���� (�X���b�h���s����(�V�O�i���Z�b�g�O)�ɃR�[���o�b�N)
//------------------------------------------
int LedManager::ThreadFirst()
{
	// TCP/IP ����N���X������
	SockInit(); 			

	// �p�����[�^�ǂݍ���
	while(TRUE) {
		if( GetParam() ) break;
		Sleep(3000);
	}

	//// �C�x���g�ݒ�
	HANDLE hArray[] = { mcls_pSock[PORT_SEND]->gque_Anser.g_evSem,
						m_tmSendAnser,
						m_tmSendOk,
						m_tmStateCheck,
						m_tmReCheck
						};
	SetEventNum(sizeof(hArray)/sizeof(HANDLE), hArray);

	//// �ǉ�
	AddEventNum(MAX_LED_CH, m_tmAddTime);
	AddEventNum(1, &mque_SendItem.g_evSem);	
	return 0;
}


//------------------------------------------
// �X���b�h�I���O���� (�I���V�O�i����ɃR�[���o�b�N)
//------------------------------------------
int LedManager::ThreadLast()
{
	// TCP/IP ����N���X�J��
	SockExit();
	return 0;
}
//------------------------------------------
// �X���b�h��WaitFor�҂��̒��O����  
// int nDefWaitNum ���̐�
// int* nNowWaitNum �ύX��̐�
//------------------------------------------
void LedManager::ThreadWaitFor(int nDefWaitNum, int* nNowWaitNum)
{
	// ��M���́A���M�v�����󂯕t���Ȃ����߂̏��u
	if( SOCK_EXEC == m_eStateMain[ID_SEND] )		*nNowWaitNum = nDefWaitNum-1;
	else											*nNowWaitNum = nDefWaitNum;
}

//------------------------------------------
// �X���b�h�C�x���g����
// int nEventNo �C�x���gNo (0�I���W��)
//------------------------------------------
void LedManager::ThreadEvent(int nEventNo)
{
	BaseTcpIp::DELIVERY_ANS*	queAns  = NULL;			// TCP/IP�N���X ���쌋��

	////// �V�O�i���̏���
	enum {	EV_SOCK_SEND_ANS = 0,			// ���M�|�[�g ���쌋��
			EV_TIME_SEND_ANSER,				// ���M�`���҂��^�C�}�[ �^�C���A�b�v
			EV_TIME_SEND_OK,				// ���M���҂��^�C�}�[ �^�C���A�b�v
			EV_TIME_SATE_CHECK,				// �X�e�[�^�X������`�F�b�N
			EV_RECHECK,						// �ă`�F�b�N �^�C���A�b�v
			EV_ADDTIME						// �_���݌v���ԉ��Z
	};


	////// �V�O�i����������
	switch (nEventNo) {

	//-----------------------------------------------------------------------------------------------
	case EV_SOCK_SEND_ANS:											// ���M�|�[�g ���쌋��

		queAns = mcls_pSock[PORT_SEND]->gque_Anser.GetFromHead(); 

		// ���쌋�ʂ̏���
		ExecAnsQue(queAns->nMyID, queAns);

		// ���o�������쌋�ʂ����
		SAFE_DELETE(queAns->Data);
		SAFE_DELETE(queAns);
		break;

//-----------------------------------------------------------------------------------------------
	case EV_TIME_SEND_ANSER:				// ���M�`���҂��^�C�}�[ �^�C���A�b�v
		LOG(em_ERR), "[%s] ���M�ɑ΂���A���T�[�`���^�C���A�E�g [�O�񑗐M=%s]", my_sThreadName, GetSendCommandName(m_InfSend.emLedCommand) );
//		syslog(SYSNO_SOCK_RECV_TIMEOUT, "[%s:%s]", my_sThreadName, GetSendCommandName(m_InfSend.emLedCommand));

		// �����ؒf
		LOG(em_ERR), "[%s] UserClose �����ؒf", my_sThreadName);
		if(m_bIsConnect) UserClose(-1);		// �q�����Ă����ԂŃ^�C���A�E�g
		else			 UserClose(0);
		return;

//-----------------------------------------------------------------------------------------------
	case EV_TIME_SEND_OK:				// ���M���҂��^�C�}�[ �^�C���A�b�v
		if( SEND_READ_EEPROM == m_InfSend.emLedCommand ||
			SEND_WRITE_EEPROM == m_InfSend.emLedCommand ) {
			// ����EEPROM�Ǎ� ���ɂ́A���M���҂� ���A���Ă��Ȃ����߁A����F���Ƃ���
		} else {
			LOG(em_WAR), "[%s] ���M���� ����M�ɂ��^�C���A�E�g [�O�񑗐M=%s]", my_sThreadName, GetSendCommandName(m_InfSend.emLedCommand) );

			//// 110ms�ȏ�o�߂����瑗�M�����҂��Ȃ��Ă悢�炵�����߁A�ُ�ɂ��Ȃ�
			//syslog(SYSNO_SOCK_RECV_TIMEOUT, "[%s:%s]", my_sThreadName, GetSendCommandName(m_InfSend.emLedCommand));

			//// �����ؒf
			//LOG(em_ERR), "[%s] UserClose �����ؒf", my_sThreadName);
			//UserClose();
		}

		//// ���̃R�}���h�𑗐M���Ă�OK
		m_eStateMain[ID_SEND] = SOCK_IDLE;
		return;

//-----------------------------------------------------------------------------------------------
	// ���ڂ̂��Ƃ肪����ɍs����܂Œ�����m�F�͓��삵�Ȃ�
	case EV_TIME_SATE_CHECK:				// �X�e�[�^�X������`�F�b�N
		LOG(em_MSG), "[%s] ������m�F�i�f���m�F�ALED���M�m�F�ALED���x�m�F�j", my_sThreadName);
		AddQue_SendCommand(SEND_CHECK_BREAK, 0);	// �f���m�F
		AddQue_SendCommand(SEND_CHECK_HEATING, 0);	// LED���M�m�F
		AddQue_SendCommand(SEND_CHECK_TEMP, 0);		// LED���x�m�F
		return;

//-----------------------------------------------------------------------------------------------
	case EV_RECHECK:						// �ă`�F�b�N �^�C���A�b�v
		KizuFunction::SetStatus(m_nIdConnect, false);	// �@���Ԃɒʒm
		LOG(em_ERR), "[%s] ����ُ� ===================================>>>>", my_sThreadName);
		return;

//-----------------------------------------------------------------------------------------------
	default :
		if(EV_ADDTIME <= nEventNo && EV_ADDTIME+MAX_LED_CH-1 >= nEventNo) {		// �_���݌v���ԉ��Z
			int nCh = nEventNo - EV_ADDTIME + 1;
			AddTime_Exec(nCh);

//-----------------------------------------------------------------------------------------------
		} else {																// ���M�˗��L���[

			if(true) {
				// ���M�f�[�^���o��
				INF_SEND* pDeli = mque_SendItem.GetFromHead(); 
				memcpy(&m_InfSend, pDeli, sizeof(m_InfSend));
				delete pDeli;

				// ��Ԋm�F
				if( MODE_NONE == m_emMainMode) break;

				// ���M
				int retc;
				if( ! SendStart(&retc) ){
					LOG(em_ERR), "[%s] UserClose SendStart Err", my_sThreadName);
					UserClose();
				}
			}
		}
		break;
	}
}

//------------------------------------------
// TCP/IP�N���X ���쌋�ʂ̏���
// int id �|�[�gID (�悭�g��������������Ă���)
// BaseTcpIp::DELIVERY_ANS* que ���쌋��
//------------------------------------------
void LedManager::ExecAnsQue(int id, BaseTcpIp::DELIVERY_ANS* que)
{
	switch(que->state) {

	//-----------------------------------------------------------------------------------------------
	case BaseTcpIp::ANS_CLOSE :										// �N���[�Y�ʒm
		if( KIZUSOCK_CLOSEKIND_SOCK == mcls_pSock[id]->GetCloseKind() ) {
			// ���肩��ؒf
			LOG(em_ERR), "[%s][%s] ���肩��ؒf�v���I�I", my_sThreadName, mcls_pSock[id]->GetMyName());
		}
		// �ؒf�������s
		LOG(em_ERR), "[%s] UserClose �ؒf�������s", my_sThreadName);
		UserClose();
		break;

	//-----------------------------------------------------------------------------------------------
	case BaseTcpIp::ANS_CONNECT :									// �R�l�N�g�����ʒm
		if( BaseTcpIp::STATE_NONE != mcls_pSock[PORT_SEND]->GetSockState() ) {

			// �������`�F�b�N (�R�l�N�g�ʒm������O�ɁA��LIF���ŁA���|�[�g����ɂȂ�\��������΍�)
			if(m_bIsConnect) break;

			for(int ii=0; ii<PARAM_SOCK_MAX; ii++) {
				m_eStateMain[ii] = SOCK_IDLE;
				m_nNowEventNo[ii] = 0;
			}
			
			// �ڑ������̂ő������[�h��҂���Ԃɂ���
			m_emMainMode = MODE_WAIT;
			AddQue_SendCommand(SEND_GET_BLOCK, 0);		// BLOCK���擾  ��1�d���������^�C�v and 24�u���b�N�ȏ�̎��ɂ́A�K���Ɩ����u�ɑ΂��ău���b�N���擾���s�����ƁB��������Ȃ��ƏƖ����u�����̃u���b�N��������ɔF������Ȃ����߁A�z���擾�Ƃ�����Ƃ��������Ȃ�

			AddQue_SendCommand(SEND_CHECK_BREAK, 0);	// �f���m�F
			AddQue_SendCommand(SEND_CHECK_HEATING, 0);	// LED���M�m�F
			AddQue_SendCommand(SEND_CHECK_TEMP, 0);		// LED���x�m�F
			AddQue_SendCommand(SEND_CHECK_ONOFF, 0);	// �_�������m�F
			AddQue_SendCommand(SEND_READ_EEPROM, 0);	// EEPROM�Ǎ�

			// ���M�|�[�g�� ��M�҂��ɂ��Ă����B
			if( ! RecvStart( PORT_SEND, PARAM_COM_BUFSIZE, m_cSockBuf[id][BUF_RECV] )) {
				mcls_pSock[PORT_SEND]->UserClose();
				break;
			}
		}
		break;

	//-----------------------------------------------------------------------------------------------
	case BaseTcpIp::ANS_RECVEND :									// �񓯊� ��M�����ʒm
		if( 0 != que->Data->nAns) {
			// �����M��������Ȃ��̂Őؒf���悤
			LOG(em_ERR), "[%s] UserClose �����M��������Ȃ� Ans:%d", my_sThreadName, que->Data->nAns);
			UserClose();
		} else {
			// ��M�������̏������s��
			int mode = RecvEnd(id, que);

			// �����킯
			if(-1 == mode) {										// �ُ�
				// �ُ킾����ؒf
				LOG(em_ERR), "[%s] UserClose �ُ킾����ؒf", my_sThreadName);
				UserClose();

			} else if(9 == mode) {									// ���M������M
				Sleep(110);											// �����ɑ���ƈُ�ɂȂ邽�߁A������Ƒ҂��Ă��瑗�M
				m_eStateMain[ID_SEND] = SOCK_IDLE;

			} else if(0 == mode) {									// ����������M
				// ����������M�ׁ̈A�������Ȃ�

			} else if(1 == mode) {									// ��M�R�}���h��M
				// �R�}���h�ɑ΂����M
				memset(&m_InfSend, 0x00, sizeof(m_InfSend));		// ���M�R�}���h�����������Ă���
			}
		}
		break;

	//-----------------------------------------------------------------------------------------------
	case BaseTcpIp::ANS_SENDEND :									// ���M�����ʒm
		// �񓯊����M�� ����́A�s��Ȃ� (�����ȗ����̈�)
		break;
	
	//-----------------------------------------------------------------------------------------------
	default:			// ���肦�Ȃ�
		LOG(em_ERR), "[%s] ���[�h�ُ� [%d]", my_sThreadName, que->state);
		break;
	}
}

// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ����M����

//------------------------------------------
// �L���[�ɗ��܂��Ă��鑗�M�˗���S�č폜
// �߂�l ���A���
//------------------------------------------
void LedManager::DeleteAllQue()
{
	INF_SEND* pDeli = NULL;

	while(true) {
		pDeli = mque_SendItem.GetFromHead(0);
		if( pDeli == NULL ) break;
		
		// ���
		delete pDeli;
	}
}

//------------------------------------------
// ���M�R�}���h
// EM_LED_SEND_COMMAND em ���M�R�}���h
// int	nCh �Ώۃ`�����l�� (0:�S�`�����l��  1�`:�w��`�����l���̂�)
// �߂�l ���A���
//------------------------------------------
bool LedManager::AddQue_SendCommand(EM_LED_SEND_COMMAND em, int nCh)
{
	int ii_s, ii_e;
	GetChLoop(nCh, ii_s, ii_e);

	//// �Ώۃ`�����l������ ���M�R�}���h���s
	for(int ii=ii_s; ii<=ii_e; ii++) {
		//======================================================
		// ���O�`�F�b�N

		if( !m_bUsedCh[ii-1] ) continue;	// ���g�pCH�̏ꍇ�A���M���Ȃ�

		if(SEND_EXEC_ON == em) {
			// �_���w���̂Ƃ��ɁA���x�ُ�̏ꍇ�͓_�������Ȃ�
			bool bTempStsNg = false;
			// ===>> �⊪ 2018.04.10 LED���M�V���b�g�_�E�����ɓ��F��Ch��LED�������ɂȂ�Ȃ��s��C��
			//for(int jj=0; jj<m_nTempUnit[ii-1]; jj++) { if(2 == m_nTempSts[ii-1][jj]) bTempStsNg = true; }
			//if( 2 == m_nStateTemp[ii-1] || bTempStsNg) {
			//	LOG(em_ERR), "[%s] ���x�ُ�ɂ��_���֎~ [CH=%d]", my_sThreadName, ii);
			//	continue;
			//}
			bool bHotErr = false;
			for(int kk=0; kk<MAX_LED_CH;kk++) {
				// �ႤIndex�̏ꍇ�͖�������
				if(m_nMyIndex[ii-1] != m_nMyIndex[kk]) continue;
				for(int jj=0; jj<m_nTempUnit[kk]; jj++) { if(2 == m_nTempSts[kk][jj]) bTempStsNg = true; }

				if( 2 == m_nStateTemp[kk] || bTempStsNg) {
				LOG(em_ERR), "[%s] ���x�ُ�ɂ��_���֎~ [CH=%d]", my_sThreadName, ii);
					bHotErr = true;
					break;
				}
			}
			if(bHotErr) continue;
			// <<===
		}

		/// 2014.10.28 ����(���P�xLED�Ή�)------------------------------------------------>>>>
		if( (SEND_CHECK_BREAK == em && 0 == m_nIdBreak[ii-1]) ||
			(SEND_CHECK_ONOFF == em && 0 == m_nIdOnOff[ii-1]) ||
			(SEND_CHECK_HEATING == em && 0 == m_nIdHotWar[ii-1]) ||
			(SEND_CHECK_HEATING == em && 0 == m_nIdHotErr[ii-1]) ) {
			LOG(em_INF), "[%s] �@��ID����`�̈׃X�e�[�^�X�v����~ [CH=%d][%s]", my_sThreadName, ii, GetSendCommandName(em));
			continue;
		}
		if(SEND_CHECK_TEMP == em && 0 == m_nTempUnit[ii-1] ) {
			// ���x���j�b�g�����Ă��Ȃ��ꍇ������̂ŁA���̂Ƃ��ɂ̓��O�͏o�������Ȃ�
			continue;
		}
		/// 2014.10.28 ����(���P�xLED�Ή�)<<<<------------------------------------------------


		//======================================================
		//// ����
		INF_SEND* pDeli = new INF_SEND;
		memset(pDeli, 0x00, sizeof(INF_SEND));

		pDeli->emLedCommand = em;
		pDeli->nSize = PARAM_COM_BUFSIZE;
		pDeli->nCh	 = ii;

		//// �R�}���h�Z�b�g

		// ���M�R�}���h�̂�
		//   �E���M���`�����󂯎��Ȃ���΁A���V�[�P���X�֐��ڂł��Ȃ�
		//   �E���M���`���̑O�Ɏ�M���Ȃ���΂Ȃ�Ȃ��`��������B���V�[�P���X�֐��ڂł��Ȃ�
		//   �@(bRecvUmu���Afalse�ł���΋��`���݂̂ƂȂ�)

		pDeli->cBuf[0] = 'R';
		switch( em ) {
			// �f���L�� (�X�e�[�^�X�v��)
			case SEND_CHECK_BREAK:
				pDeli->cBuf[1] = 'a';
				pDeli->bRecvUmu = true;
				break;
			// �_��������� (�X�e�[�^�X�v��)
			case SEND_CHECK_ONOFF:
				pDeli->cBuf[1] = 'l';
				pDeli->bRecvUmu = true;
				break;
			// LED���M��� (�X�e�[�^�X�v��)
			case SEND_CHECK_HEATING:
				pDeli->cBuf[1] = 'b';
				pDeli->bRecvUmu = true;
				break;
			// LED���x��� (�X�e�[�^�X�v��)
			case SEND_CHECK_TEMP:
				pDeli->cBuf[1] = 't';
				pDeli->bRecvUmu = true;
				break;
			// �_���w��
			case SEND_EXEC_ON:
				pDeli->cBuf[1] = 'X';
				pDeli->bRecvUmu = true;
				break;
			// �����w��
			case SEND_EXEC_OFF:
				pDeli->cBuf[1] = 'Y';
				pDeli->bRecvUmu = true;
				break;
			// �����f�[�^���M (RAM�ɃZ�b�g)
			case SEND_SEND_VALUE:
				pDeli->cBuf[1] = 'S';
				pDeli->bRecvUmu = false;
				break;
			// RAM�̒����f�[�^��EEPROM(�t�@�C��)�ɏ���
			case SEND_WRITE_EEPROM:
				pDeli->cBuf[1] = 'W';
				pDeli->bRecvUmu = false;
				break;
			// EEPROM(�t�@�C��)�̒����f�[�^��Ǎ�
			case SEND_READ_EEPROM:
				pDeli->cBuf[1] = 'M';
				pDeli->bRecvUmu = true;
				break;
			// RAM�̒����f�[�^��Ǎ�
			case SEND_READ_RAM:
				pDeli->cBuf[1] = 'R';
				pDeli->bRecvUmu = true;
				break;
			// �u���b�N���擾
			case SEND_GET_BLOCK:
				pDeli->cBuf[1] = 'B';
				pDeli->bRecvUmu = true;
				break;
		}
		pDeli->cBuf[2] = 0x01;
		pDeli->cBuf[3] = 0x01 + ii-1;		// �`�����l��
		pDeli->cBuf[4] = 0x00;

		// �ǉ�
		if( ! mque_SendItem.AddToTail(pDeli, 20, 20) ) {		// ��Γo�^
			// ��Γo�^�������E�E�E
			LOG(em_ERR), "[%s] ���M�˗��L���[�t�� [CH=%d] [%s]", my_sThreadName, ii, GetSendCommandName(em));
			syslog(SYSNO_QUEFULL_ERR, "[%s:CH=%d:%s]", my_sThreadName, ii, GetSendCommandName(em));
		
			// �̈���
			delete pDeli;
			return false;
		}
	}
	return true;
}





//------------------------------------------
// �}�X�^�[�{�z���f�[�^�Z�b�g
// int nCh �Ώۃ`�����l�� (0:�S�`�����l��  1�`:�w��`�����l���̂�)
// int nLedBase �S�̒���
// BYTE* nLedHosei �z���f�[�^
// �߂�l ���A���
//------------------------------------------
bool LedManager::AddQue_SetLightMastVal(int nCh, BYTE nLedBase, BYTE* nLedHosei)
{
	bool retc = true;
	int ii_s, ii_e;
	GetChLoop(nCh, ii_s, ii_e);
	
	//// ���M
	for(int ii=ii_s; ii<=ii_e; ii++) {

		if( !m_bUsedCh[ii-1] ) continue;	// ���g�pCH�̏ꍇ�A���M���Ȃ�

		// ����
		INF_SEND* pDeli = new INF_SEND;
		memset(pDeli, 0x00, sizeof(INF_SEND));

		pDeli->emLedCommand = SEND_SEND_VALUE;
		pDeli->nSize = PARAM_COM_BUFSIZE + 1 + m_nBlock[ii-1];	// ���M�R�}���h + �S�̒����f�[�^ �{ �␳�f�[�^�u���b�N��
		pDeli->nCh	 = ii;

		pDeli->bRecvUmu = false;
		pDeli->cBuf[0] = 'R';
		pDeli->cBuf[1] = 'S';
		pDeli->cBuf[2] = 0x01;
		pDeli->cBuf[3] = 0x01 + ii-1;		// �`�����l��
		pDeli->cBuf[4] = 0x00;

		pDeli->cBuf[5] = nLedBase;
		memcpy(&pDeli->cBuf[6], nLedHosei, m_nBlock[ii-1]);

		// �ǉ�
		if( ! mque_SendItem.AddToTail(pDeli, 20, 20) ) {		// ��Γo�^
			// ��Γo�^�������E�E�E
			LOG(em_ERR), "[%s] ���M�˗��L���[�t�� [CH=%d] [%s]", my_sThreadName, ii, GetSendCommandName(SEND_SEND_VALUE));
			syslog(SYSNO_QUEFULL_ERR, "[%s:CH=%d:%s]", my_sThreadName, ii, GetSendCommandName(SEND_SEND_VALUE));
		
			// �̈���
			delete pDeli;
			return false;
		}

		// �����f�[�^�𑗐M���́A�K��EPPROM�ɏ�������
		// �������񂾂̂��AEPPROM�̃f�[�^���擾���Ă݂�B�ŉ�ʓ��Ɍ��ʂ𔽉f������
		retc &= AddQue_SendCommand(SEND_WRITE_EEPROM, ii);
		retc &= AddQue_SendCommand(SEND_READ_EEPROM, ii);
	}
	return retc;
}

//------------------------------------------
// �}�X�^�[�{�z���f�[�^�Z�b�g (scode)
// int	nCh �Ώۃ`�����l�� (0:�S�`�����l��  1�`:�w��`�����l���̂�)
// int scode (1�I���W��)
// �߂�l ���A���
//------------------------------------------
bool LedManager::AddQue_SetLightMastVal(int nCh, int scode)
{
	// �SSCODE�Ŗ����ꍇ�͍X�V
	if( CH_SCODE_ALL != scode )	m_nNowScode = scode;	

	bool retc = true;
	int ii_s, ii_e;
	GetChLoop(nCh, ii_s, ii_e);
	
	//// ���M
	for(int ii=ii_s; ii<=ii_e; ii++) {

		if( !m_bUsedCh[ii-1] ) continue;	// ���g�pCH�̏ꍇ�A���M���Ȃ�

		retc &= AddQue_SetLightMastVal( ii, m_Param.nLedBase[m_nNowScode-1][ii-1], m_Param.nLedHosei[m_nNowScode-1][ii-1] );
	}
	return retc;
}

//------------------------------------------
// �}�X�^�[�{�z���f�[�^�Z�b�g  (�z�����͑S������l)
// int	nCh �Ώۃ`�����l�� (0:�S�`�����l��  1�`:�w��`�����l���̂�)
// int nLedBase �S�̒���
// BYTE nLedHosei �z���f�[�^ (�z�����͑S������l)
// �߂�l ���A���
//------------------------------------------
bool LedManager::AddQue_SetLightMastVal(int nCh, BYTE nLedBase, BYTE nLedHosei)
{
	bool	retc = true;
	int ii_s, ii_e;
	GetChLoop(nCh, ii_s, ii_e);

	//// �Ώۃ`�����l������ ���M�R�}���h���s
	for(int ii=ii_s; ii<=ii_e; ii++) {

		if( !m_bUsedCh[ii-1] ) continue;	// ���g�pCH�̏ꍇ�A���M���Ȃ�

		//// �z���f�[�^
		BYTE  nWk[MAX_LED_BLOCK];
		for(int jj=0; jj<m_nBlock[ii-1]; jj++) nWk[jj] = nLedHosei;
		retc &= AddQue_SetLightMastVal( ii, nLedBase, nWk );
	}

	return retc;
}


//------------------------------------------
// �}�X�^�[�����f�[�^�Z�b�g
// int	nCh �Ώۃ`�����l�� (0:�S�`�����l��  1�`:�w��`�����l���̂�)
// int nLedBase �S�̒���
// �߂�l ���A���
//------------------------------------------
bool LedManager::AddQue_SetLightMaster(int nCh, int nLedBase)
{
	bool	retc = true;
	int ii_s, ii_e;
	GetChLoop(nCh, ii_s, ii_e);

	//// �Ώۃ`�����l������ ���M�R�}���h���s
	for(int ii=ii_s; ii<=ii_e; ii++) {

		if( !m_bUsedCh[ii-1] ) continue;	// ���g�pCH�̏ꍇ�A���M���Ȃ�

		// ����
		INF_SEND* pDeli = new INF_SEND;
		memset(pDeli, 0x00, sizeof(INF_SEND));

/*** 		// 3.2.1 �C�� (�Ȃ��� RT �͎󂯕t���Ă���Ȃ��̂ŁARS�ő��M���đ�p����)
		pDeli->emLedCommand = SEND_SEND_MASTER;
		pDeli->nSize = PARAM_COM_BUFSIZE + 1;	// ���M�R�}���h + �S�̒����f�[�^
		pDeli->nCh	 = ii;

		pDeli->bRecvUmu = false;
		pDeli->cBuf[0] = 'R';
		pDeli->cBuf[1] = 'T';
		pDeli->cBuf[2] = 0x01;
		pDeli->cBuf[3] = 0x01 + ii-1;		// �`�����l��
		pDeli->cBuf[4] = 0x00;

		pDeli->cBuf[5] = nLedBase;
***/
		pDeli->emLedCommand = SEND_SEND_MASTER;
		pDeli->nSize = PARAM_COM_BUFSIZE + 1 + m_nBlock[ii-1];	// ���M�R�}���h + �S�̒����f�[�^ �{ �␳�f�[�^�u���b�N��
		pDeli->nCh	 = ii;

		pDeli->bRecvUmu = false;
		pDeli->cBuf[0] = 'R';
		pDeli->cBuf[1] = 'S';
		pDeli->cBuf[2] = 0x01;
		pDeli->cBuf[3] = 0x01 + ii-1;		// �`�����l��
		pDeli->cBuf[4] = 0x00;

		pDeli->cBuf[5] = nLedBase;
		memcpy(&pDeli->cBuf[6], m_nNowLedHosei[ii-1], m_nBlock[ii-1]);


		// �ǉ�
		if( ! mque_SendItem.AddToTail(pDeli, 20, 20) ) {		// ��Γo�^
			// ��Γo�^�������E�E�E
			LOG(em_ERR), "[%s] ���M�˗��L���[�t�� [CH=%d] [%s]", my_sThreadName, ii, GetSendCommandName(SEND_SEND_MASTER));
			syslog(SYSNO_QUEFULL_ERR, "[%s:CH=%d:%s]", my_sThreadName, ii, GetSendCommandName(SEND_SEND_MASTER));
		
			// �̈���
			delete pDeli;
			return false;
		}

		// �����f�[�^�𑗐M���́A�K��EPPROM�ɏ�������
		// �������񂾂̂��AEPPROM�̃f�[�^���擾���Ă݂�B�ŉ�ʓ��Ɍ��ʂ𔽉f������
		retc &= AddQue_SendCommand(SEND_WRITE_EEPROM, ii);
		retc &= AddQue_SendCommand(SEND_READ_EEPROM, ii);
	}
	return retc;
}

//------------------------------------------
// �����f�[�^�Z�b�g (�z�����͈ꊇ�w��)
// int nCh �Ώۃ`�����l�� (0:�S�`�����l��  1�`:�w��`�����l���̂�)
// BYTE* nLedHosei �z���f�[�^
// �߂�l ���A���
//------------------------------------------
bool LedManager::AddQue_SetLightValue(int nCh, BYTE* nLedHosei)
{
	bool	retc = true;
	int ii_s, ii_e;
	GetChLoop(nCh, ii_s, ii_e);

	//// �Ώۃ`�����l������ ���M�R�}���h���s
	for(int ii=ii_s; ii<=ii_e; ii++) {

		if( !m_bUsedCh[ii-1] ) continue;	// ���g�pCH�̏ꍇ�A���M���Ȃ�

		retc &= AddQue_SetLightMastVal(nCh, m_nNowLedBase[ii-1], nLedHosei);
	}

	return retc;
}

//------------------------------------------
// �z���f�[�^�Z�b�g (�z�����͑S������l)
// int nCh �Ώۃ`�����l�� (0:�S�`�����l��  1�`:�w��`�����l���̂�)
// BYTE nLedHosei �z���f�[�^
// �߂�l ���A���
//------------------------------------------
bool LedManager::AddQue_SetLightValue(int nCh, BYTE nLedHosei)
{
	bool	retc = true;
	int ii_s, ii_e;
	GetChLoop(nCh, ii_s, ii_e);

	//// �Ώۃ`�����l������ ���M�R�}���h���s
	for(int ii=ii_s; ii<=ii_e; ii++) {

		if( !m_bUsedCh[ii-1] ) continue;	// ���g�pCH�̏ꍇ�A���M���Ȃ�

		retc &= AddQue_SetLightMastVal(nCh, m_nNowLedBase[ii-1], nLedHosei);
	}

	return retc;
}

//------------------------------------------
// �����f�[�^�Z�b�g (�z�����͌ʎw��)
// int nCh �Ώۃ`�����l�� (0:�S�`�����l��  1�`:�w��`�����l���̂�)
// BYTE nLedHosei �z���f�[�^
// �߂�l ���A���
//------------------------------------------
bool LedManager::AddQue_SetLightValue(int nCh, int nBlock, BYTE nLedHosei)
{
	bool	retc = true;
	int ii_s, ii_e;
	GetChLoop(nCh, ii_s, ii_e);

	BYTE  nWk[MAX_LED_BLOCK];		// �z���f�[�^

	//// �Ώۃ`�����l������ ���M�R�}���h���s
	for(int ii=ii_s; ii<=ii_e; ii++) {

		if( !m_bUsedCh[ii-1] ) continue;	// ���g�pCH�̏ꍇ�A���M���Ȃ�

		for(int jj=0; jj<m_nBlock[ii-1]; jj++) nWk[jj] = m_nNowLedHosei[ii-1][jj];	// �܂��͌��݂̒l��ω��Ȃ����ڂƂ��ăZ�b�g
		nWk[nBlock] = nLedHosei;	// �ύX�L�蕔���̂ݍăZ�b�g


		retc &= AddQue_SetLightMastVal(nCh, m_nNowLedBase[ii-1], nWk);
	}

	return retc;
}

//------------------------------------------
// ���M�J�n
// int* retStat ���M�˗����[�h(�߂�l)
// �߂�l false:�ؒf���Ă˗v��
//------------------------------------------
bool LedManager::SendStart(int* retStat)
{
	// ��Ԋm�F
	if( MODE_NONE == m_emMainMode) {
		//LOG(em_ERR), "[%s] �\�P�b�g�ؒf���ɑ��M�˗��L��B�˗��j��[CH=%d][%d]", my_sThreadName, m_InfSend.nCh, m_InfSend.emLedCommand);
		return false;
	}
	
	
	// �f�[�^�R�s�[
	memcpy(&m_cSockBuf[PORT_SEND][BUF_SEND], m_InfSend.cBuf, m_InfSend.nSize);
	*retStat = m_InfSend.emLedCommand;	


	//// �������M
	LOG(em_INF), "[%s] �R�}���h���M [CH=%d][%s] [%.2s]", my_sThreadName, m_InfSend.nCh, GetSendCommandName(m_InfSend.emLedCommand), m_InfSend.cBuf );
//��	LOG(em_INF), "[%s] �R�}���h���M [CH=%d][%s] [%.2s]", my_sThreadName, m_InfSend.nCh, GetSendCommandName(m_InfSend.emLedCommand), m_InfSend.cBuf );
	//syslog(213,"[%s:%s]", my_sThreadName, GetSendCommandName(m_InfSend.emLedCommand) );
	LOG(em_MSG), "   ���M[%d][%s][%02X,%02X,%02X,%02X,%02X,%02X]", m_InfSend.nSize, m_InfSend.cBuf, m_InfSend.cBuf[0], m_InfSend.cBuf[1], m_InfSend.cBuf[2], m_InfSend.cBuf[3], m_InfSend.cBuf[4], m_InfSend.cBuf[5]);
 

	int iRetc = mcls_pSock[PORT_SEND]->LockSend( m_cSockBuf[PORT_SEND][BUF_SEND], m_InfSend.nSize, PARAM_RES_TIMEOUT );
	if(0 != iRetc) {
		LOG(em_ERR), "[%s][CH=%d][%s] TCP���M�G���[(err_code = %d)", my_sThreadName, m_InfSend.nCh, GetSendCommandName(m_InfSend.emLedCommand), iRetc);
		syslog(SYSNO_SOCK_SEND_ERR, "[%s CH=%d err_code = %d]", my_sThreadName, m_InfSend.nCh, iRetc);
		return false;
	}


	// -------------------------------------
	__int64 ts;		// 1�ʕb��100�i�m�b (�}�C�i�X�w��ŁA�w�莞�Ԍo�߂ŃV�O�i��) (��3������2��ڈȍ~�̋N���Ԋu)

	//// �A���T�[��������̂̂݁A�����`���҂��^�C�}�[ON	
	if (m_InfSend.bRecvUmu) {
		m_emMainMode		  = MODE_RECV_EXEC;				// ���M�����̂Ŏ�M�R�}���h�҂��ɂ���
		ts = PARAM_ANSER_TIMEOUT * -10000;	// 1�ʕb��100�i�m�b (�}�C�i�X�w��ŁA�w�莞�Ԍo�߂ŃV�O�i��) (��3������2��ڈȍ~�̋N���Ԋu)
		SetWaitableTimer(m_tmSendAnser, (LARGE_INTEGER *)&ts, 0, NULL, NULL, FALSE);
	} else {
		m_emMainMode		  = MODE_WAIT;					// �A���T�[�Ȃ��Ȃ̂ŁA�����Ȃ著�M���҂��ɂ���
	}

	//// ���M���҂��^�C�}�[
	// �S�`���ɑ��M���҂��^�C�}�[���Z�b�g
	if (m_InfSend.bRecvUmu)	ts = PARAM_SENDOK_TIMEOUT * -10000;
	else					ts = PARAM_SENDOK_NOWAIT_TIMEOUT * -10000;
	SetWaitableTimer(m_tmSendOk,  (LARGE_INTEGER *)&ts, 0, NULL, NULL, FALSE);
	m_eStateMain[ID_SEND] = SOCK_EXEC;
	return true;
}

//------------------------------------------
// ��M����
// int id �|�[�gID
// BaseTcpIp::DELIVERY_ANS* que ���쌋��
// �߂�l 0:����������M  -1:�ُ�(�ؒf���Ă�) 1:�R�}���h�ɑ΂���A���T�[����M  9:���M������M
//------------------------------------------
int LedManager::RecvEnd(int id, BaseTcpIp::DELIVERY_ANS* que)
{
	// �R�}���h����
	EM_LED_RECV_COMMAND  emRecv = GetRecvCommand(m_cSockBuf[PORT_SEND][BUF_RECV][1]);
	EM_LED_SEND_COMMAND  emSend = GetSendCommand(m_cSockBuf[PORT_SEND][BUF_SEND][1]);


	// ///////////////////////////////////////////////////////////////////
	// �O����
	//

	// ��M�R�}���h�̒����Ɋ֌W����ׁA��ɓ���
	m_InfRecv.nCh	= m_cSockBuf[PORT_SEND][BUF_RECV][3];	// �`�����l��

	// �f�[�^���������M�R�}���h����M
	// ��M�R�}���h�̎�ʂŁA�f�[�^���̒��������߂ĕ�����̂ŁA�S�̒������Ē�`
	if( RECV_LED_TEMP == emRecv) {
		if (PARAM_COM_BUFSIZE == que->Data->nSize) {
			// �c�����M
			m_InfRecv.nSize = PARAM_COM_BUFSIZE + m_nTempUnit[m_InfRecv.nCh-1];		// ��M�R�}���h + LED���x�i�[�_��
			if( ! RecvStart( id, m_nTempUnit[m_InfRecv.nCh-1], m_cSockBuf[id][BUF_RECV], PARAM_COM_BUFSIZE)) return -1;
			return 0;
		}
	} else 	if( RECV_READ_EEPROM == emRecv || RECV_READ_RAM == emRecv) {
		if (PARAM_COM_BUFSIZE == que->Data->nSize) {
			// �c�����M
			m_InfRecv.nSize = PARAM_COM_BUFSIZE + 1 + m_nBlock[m_InfRecv.nCh-1]; // ��M�R�}���h + �S�̒����f�[�^ �{ �␳�f�[�^�u���b�N��
			if( ! RecvStart( id, m_InfRecv.nSize, m_cSockBuf[id][BUF_RECV], PARAM_COM_BUFSIZE)) return -1;
			return 0;
		}

	} else {
		m_InfRecv.nSize = que->Data->nSize;
	}

	memcpy(&m_InfRecv.cBuf, &m_cSockBuf[PORT_SEND][BUF_RECV], m_InfRecv.nSize);


	// ///////////////////////////////////////////////////////////////////
	// �P�R�}���h��M������
	//
	LOG(em_INF), "[%s][CH=%d] �R�}���h��M [%s] [size=%d,%.2s] [�O�񑗐M=%s]",
		my_sThreadName, m_InfRecv.nCh, GetRecvCommandName(emRecv), m_InfRecv.nSize, m_InfRecv.cBuf, GetSendCommandName(emSend) );
//��	LOG(em_INF), "[%s][CH=%d] �R�}���h��M [%s] [size=%d,%.2s] [�O�񑗐M=%s]",
//��		my_sThreadName, m_InfRecv.nCh, GetRecvCommandName(emRecv), m_InfRecv.nSize, m_InfRecv.cBuf, GetSendCommandName(emSend) );
	LOG(em_MSG), "   ��M[%d][%.5s][%02X,%02X,%02X,%02X,%02X,%02X]", m_InfRecv.nSize, m_InfRecv.cBuf, m_InfRecv.cBuf[0], m_InfRecv.cBuf[1], m_InfRecv.cBuf[2], m_InfRecv.cBuf[3], m_InfRecv.cBuf[4], m_InfRecv.cBuf[5]);

	// ����ڑ��F�� (������M������OK)
	if( ! m_bIsConnect) {
		CancelWaitableTimer(m_tmReCheck);

		LOG(em_MSG), "[%s] �Ɩ����u ��������", my_sThreadName);
		syslog(SYSNO_SOCK_CONNECT, "[%s]", my_sThreadName);

		m_bIsConnect = true;
		KizuFunction::SetStatus(m_nIdConnect, true);

		// ����ڑ������������J�n
		__int64 ts = (__int64)(m_nTimerCycle*1000)* -10000;	// 1�ʕb��100�i�m�b (�}�C�i�X�w��ŁA�w�莞�Ԍo�߂ŃV�O�i��) (��3������2��ڈȍ~�̋N���Ԋu)
		SetWaitableTimer(m_tmStateCheck, (LARGE_INTEGER *)&ts, m_nTimerCycle*1000, NULL, NULL, FALSE);
	}

	// �R�}���h�ɂ��U�蕪��
	if( RECV_NON == emRecv) {								// �s���Ȃ�ُ�Ƃ��Ă���
		LOG(em_ERR), "[%s] �s���R�}���h [CH=%d,size=%d,%s]", my_sThreadName, m_InfRecv.nCh, m_InfRecv.nSize, m_InfRecv.cBuf);
		syslog(SYSNO_SOCK_RECV_ERR, "[%s:CH=%d:size=%d:%s]", my_sThreadName, m_InfRecv.nCh, m_InfRecv.nSize, m_InfRecv.cBuf);
		// ���޲�߽ 20131101
		//return -1;
		// ������ς�L���� 20160524  ���z���g�Ɉُ�̏ꍇ�A�����؂肽��
		return -1;

	} else if(RECV_NOTHING == emRecv) {						// ���S�ɖ�������R�}���h�B
		return 0;

	} else if(RECV_NEXT_SEND_OK == emRecv) {				// ���M���Ȃ�^�C�}�[�L�����Z�����ďI���
		// ���M���^�C�}�[�L�����Z��
		CancelWaitableTimer(m_tmSendOk);
		
		// ��M�R�}���h��t���ɂ����Ȃ著�M������M�B (= �V�[�P���X�������Ă���)
		if(MODE_WAIT != m_emMainMode) {
			LOG(em_ERR), "[%s][CH=%d] ��M�R�}���h��t���ɂ����Ȃ著�M������M", my_sThreadName, m_InfRecv.nCh);
			syslog(SYSNO_SOCK_RECV_ERR, "[%s:CH=%d]", my_sThreadName, m_InfRecv.nCh);
			//return -1;
		} else {
			LOG(em_MSG), "[%s:CH=%d] ���M����M", my_sThreadName, m_InfRecv.nCh);
//			LOG(em_MSG), "[%s:CH=%d] ���M����M", my_sThreadName, m_InfRecv.nCh);
		}

		// �܂��ŏ������M�J�n
		if( ! RecvStart( id, PARAM_COM_BUFSIZE, m_cSockBuf[id][BUF_RECV] )) {
			return -1;
		}

		return 9;
	}

	// ///////////////////////////////////////////////////////////////////
	// �����܂ŗ�����A��M�R�}���h
	//
	
	//// �A���T�[�҂��^�C�}�[�L�����Z��
	CancelWaitableTimer(m_tmSendAnser);
	CancelWaitableTimer(m_tmAddTime);

	//// �����킯
	CString  wk;
	int nCh = m_InfRecv.nCh-1;

	if(MODE_RECV_EXEC != m_emMainMode) { LOG(em_INF), "�����Ȃ��M�R�}���h�������Ă����I");}

	// �R�}���h�ɂ�菈������
	switch( emRecv ) {
		// -------------------------------------------------------------
		case RECV_STATE_ON:												// [Ax] �_��
			if(0 == m_nStateOnOff[nCh]) break;										// �O�񂩂�_���Ȃ疳��
			m_nStateOnOff[nCh] = 0;
			LOG(em_MSG), "[%s][CH=%d] �_�����o", my_sThreadName, nCh+1);

			KizuFunction::SetStatus(m_nIdOnOff[nCh], 1, false);
			syslog(SYSLOG_BASE+0, "[%s:CH=%d]", my_sThreadName, nCh+1);

			// �_���ݐώ��ԏW�v�J�n
			AddTime_Start(nCh+1);
			break;

		// -------------------------------------------------------------
		case RECV_STATE_OFF:											// [Ay] ����
			if(1 == m_nStateOnOff[nCh]) break;										// �O�񂩂�����Ȃ疳��
			m_nStateOnOff[nCh] = 1;
			LOG(em_WAR), "[%s][CH=%d] �������o", my_sThreadName, nCh+1);

			KizuFunction::SetStatus(m_nIdOnOff[nCh], 0, false);
			syslog(SYSLOG_BASE+1, "[%s:CH=%d]", my_sThreadName, nCh+1);

			// �_���ݐώ��ԏW�v��~
			AddTime_Stop(nCh+1);
			break;

		// -------------------------------------------------------------
		case RECV_STATE_BREAK:											// [AA] �f��
			if(1 == m_nStateBreak[nCh]) break;								// �O�񂩂�ُ�Ȃ疳��
			m_nStateBreak[nCh] = 1;
			LOG(em_WAR), "[%s][CH=%d] �f�����o", my_sThreadName, nCh+1);

			KizuFunction::SetStatus(m_nIdBreak[nCh], false);
			break;

		// -------------------------------------------------------------
		case RECV_STATE_HOT:											// [AB] ���x�㏸
			if(1 == m_nStateTemp[nCh]) break;								// �O�񂩂�ُ�Ȃ疳��
			m_nStateTemp[nCh] = 1;
			LOG(em_WAR), "[%s][CH=%d] LED���M���o", my_sThreadName, nCh+1);

			// ���M�ƃV���b�g�_�E���͔r���̊֌W
			KizuFunction::SetStatus(m_nIdHotWar[nCh], false);
			KizuFunction::SetStatus(m_nIdHotErr[nCh], true, false);
			break;

		// -------------------------------------------------------------
		case RECV_STATE_SHATDOWN:										// [AD] ���M�����޳�
			if(2 == m_nStateTemp[nCh]) break;								// �O�񂩂�ُ�Ȃ疳��
			m_nStateTemp[nCh] = 2;
			LOG(em_WAR), "[%s][CH=%d] LED���M�V���b�g�_�E�����o", my_sThreadName, nCh+1);

			// ���M�ƃV���b�g�_�E���͔r���̊֌W
			KizuFunction::SetStatus(m_nIdHotWar[nCh], true, false);
			KizuFunction::SetStatus(m_nIdHotErr[nCh], false);

			// 20160524 ���M�V���b�g�_�E�����ɂ́A�Ɩ��d�����͏����B�Ӄփ\�t�g�I�ɂ͓_����ԂƂȂ�B
			// �����ŁA�����w���������āA��Ԃ���v������
			LOG(em_WAR), "[%s] ���������w��", my_sThreadName);
			// ===>> �⊪ 2018.04.10 LED���M�V���b�g�_�E�����ɓ��F��Ch��LED�������ɂȂ�Ȃ��s��C��
			//AddQue_SendCommand(SEND_EXEC_OFF, 0);
			AddQue_SendCommand_Index(SEND_EXEC_OFF, m_nMyIndex[nCh-1]);
			// <<===
			break;

		// -------------------------------------------------------------
		case RECV_STATE_OK:												// [AN] ����
			//// ���̑��M�R�}���h�ɑ΂���A���T�[���H
			if( SEND_CHECK_BREAK == emSend ) {
				LOG(em_MSG), "[%s][CH=%d] �f�������A���T�[��M", my_sThreadName, nCh+1);
				if(0 == m_nStateBreak[nCh]) break;										// �O�񂩂琳��Ȃ疳��
				
				m_nStateBreak[nCh] = 0;
				LOG(em_MSG), "[%s][CH=%d] �f���������o", my_sThreadName, nCh+1);
				KizuFunction::SetStatus(m_nIdBreak[nCh], true);

			} else if( SEND_CHECK_HEATING == emSend ) {
				LOG(em_MSG), "[%s][CH=%d] �ߔM�����A���T�[��M", my_sThreadName, nCh+1);
				if(0 == m_nStateTemp[nCh]) break;										// �O�񂩂琳��Ȃ疳��

				m_nStateTemp[nCh] = 0;
				LOG(em_MSG), "[%s][CH=%d] �ߔM�������o", my_sThreadName, nCh+1);
				KizuFunction::SetStatus(m_nIdHotWar[nCh], true);
				KizuFunction::SetStatus(m_nIdHotErr[nCh], true);

			} else {
				// ���M�R�}���h�ɑΉ����鐳��A���T�[�ł͖���
				LOG(em_WAR), "[%s][CH=%d] ���M�R�}���h�ɑΉ����鐳��A���T�[�ł͖����B[%s]",  my_sThreadName, nCh+1, GetSendCommandName(m_InfSend.emLedCommand));
				return -1;
			}
			break;

		// -------------------------------------------------------------
		case RECV_READ_EEPROM:											// [AE] EEPROM �����f�[�^ �Ǎ�
			if(true) {
				m_nNowLedBase[nCh]  = m_InfRecv.cBuf[5];
				for(int ii=0; ii<m_nBlock[nCh]; ii++) m_nNowLedHosei[nCh][ii] = m_InfRecv.cBuf[6+ii];

				int nVal = (int)((double)(GetNowLedValue(nCh, 0) / 255.0)*100.0);
				LOG(em_MSG), "[%s][CH=%d] �����f�[�^�擾 [�S�̒���=%d;�z��=%d;����=%d:%d%%]", my_sThreadName, nCh+1, m_nNowLedBase[nCh], m_nNowLedHosei[nCh][0], GetNowLedValue(nCh,0), nVal);
				syslog(SYSLOG_BASE+2, "[%s:CH=%d:����=%d:�z��=%d:����=%d:%d%%]", my_sThreadName, nCh+1, m_nNowLedBase[nCh], m_nNowLedHosei[nCh][0], GetNowLedValue(nCh,0), nVal);

				KizuFunction::SetStatus(m_nIdValNow[nCh], nVal, false);	
			}
			break;

		// -------------------------------------------------------------
		case RECV_READ_RAM:												// [AP] RAM �����f�[�^ �Ǎ�
			// �r���ł́A���g�p�̂͂�
			m_nNowLedBase[nCh]  = m_InfRecv.cBuf[5];
			for(int ii=0; ii<m_nBlock[nCh]; ii++) m_nNowLedHosei[nCh][ii] = m_InfRecv.cBuf[6+ii];
			break;

		// -------------------------------------------------------------
		case RECV_LED_TEMP:												// [At] LED���x�f�[�^ �Ǎ�
			if( SEND_CHECK_TEMP == emSend ) {
				// LED���x�f�[�^�Z�b�g
				for(int ii=0; ii<m_nTempUnit[nCh]; ii++) {
					m_nTemp[nCh][ii] = (int)m_InfRecv.cBuf[5+ii];
				}
				LOG(em_MSG), "[%s] ���x[%d][%d][%d]", my_sThreadName, m_nTemp[nCh][0], m_nTemp[nCh][1], m_nTemp[nCh][2]);

				// LED���x�Z�b�g
				SetTemp(nCh+1);
			} else {
				// ���M�R�}���h�ɑΉ����鐳��A���T�[�ł͖���
				LOG(em_WAR), "[%s][CH=%d] ���M�R�}���h�ɑΉ����鐳��A���T�[�ł͖����B[%s]",  my_sThreadName, nCh+1, GetSendCommandName(m_InfSend.emLedCommand));
				return -1;
			}
			break;

		// -------------------------------------------------------------
		case RECV_GET_BLOCK:											// [Ab] �u���b�N�� �Ǎ�
			if(true) {
				// ��M���ă��O���o������
				BYTE nCh	= m_InfRecv.cBuf[3];
				BYTE nBlock = m_InfRecv.cBuf[4];
				LOG(em_MSG), "[%s][CH=%d] �u���b�N���擾 = %d",  my_sThreadName, nCh, nBlock);
			}
			break;
		// -------------------------------------------------------------
		case RECV_CMD_ERROR:											// [Ae] �R�}���h�G���[
			if(true) {
				// ��M���ă��O���o������
				BYTE nCh	= m_InfRecv.cBuf[3];
				LOG(em_ERR), "[%s][CH=%d] �R�}���h�G���[",  my_sThreadName, nCh);
			}
			break;
	}

	// �܂��ŏ������M�J�n
	if( ! RecvStart( id, PARAM_COM_BUFSIZE, m_cSockBuf[id][BUF_RECV] )) {
		return -1;
	}

	// ��n��
	m_emMainMode = MODE_WAIT;

	//2015014
	//
	if(RECV_READ_EEPROM == emRecv)	return 9;
	else							return 0;
}


// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// TPC/IP �ʐM����

//------------------------------------------
// ���������� (��x�̂݋N��)
//------------------------------------------
void LedManager::SockInit()
{
	BaseTcpIp::DELIVERY_COMMON	deli;

	//// ���M�p�X���b�h ����
	// ��`
	deli.nMyID = PORT_SEND;
	deli.bSorC = false;
	deli.bRorS = false;
	sprintf(deli.cMyName,  "%s", my_sThreadName);
	sprintf(deli.cSession, "%s", m_cMySession);
	deli.nBufferSize = PARAM_SOCK_BUFSIZE;
	deli.nRetryTimer = PARAM_SOCK_TIMEOUT;

	// ����
	mcls_pSock[PORT_SEND] = new BaseTcpIp(&deli);
	mcls_pSock[PORT_SEND]->SetLogMgr(mcls_pLog);
	mcls_pSock[PORT_SEND]->SetMainPostTid(GetTid());

	// ���[�J�[�X���b�h�J�n
	mcls_pSock[PORT_SEND]->Start();
}

//------------------------------------------
// �I������ (��x�̂݋N��)
//------------------------------------------
void LedManager::SockExit()
{
	// ���[�J�[�X���b�h��~
	mcls_pSock[PORT_SEND]->Stop();
	// ���[�J�[�X���b�h���
	SAFE_DELETE( mcls_pSock[PORT_SEND] );
}


//------------------------------------------
// �����ؒf
// int nClose �敪 (-1:�ؒf���邯�ǂ܂��ُ�ɂ��Ȃ�  0:�ؒf&�ُ�  1:�������̂�  )
//------------------------------------------
void LedManager::UserClose(int nClose)
{
	if( MODE_NONE != m_emMainMode && 0 == nClose ) {		// ���ڑ����H
		// �ǂ̃|�[�g����ł� �N���[�Y�ʒm��������A�S�|�[�g�����I��������
		mcls_pSock[PORT_SEND]->UserClose();
		KizuFunction::SetStatus(m_nIdConnect, false);	// �@���Ԃɒʒm
		LOG(em_ERR), "[%s] ����ُ� ===================================>>>>", my_sThreadName);
	}

	if( MODE_NONE != m_emMainMode && -1 == nClose ) {		// ���ڑ����H
		// �ǂ̃|�[�g����ł� �N���[�Y�ʒm��������A�S�|�[�g�����I��������
		mcls_pSock[PORT_SEND]->UserClose();
		LOG(em_WAR), "[%s] ����ُ� �ۗ������", my_sThreadName);

		// �z���g�Ɉُ킩�ă`�F�b�N���J�n
		__int64 ts = PARAM_RECHECK_TIMEOUT * -10000;
		SetWaitableTimer(m_tmReCheck,  (LARGE_INTEGER *)&ts, 0, NULL, NULL, FALSE);
	}

	if( 1 != nClose ) 
	{
		for(int ii=0; ii<PARAM_SOCK_MAX; ii++) {
			m_eStateMain[ii]  = SOCK_NONE;
			m_nNowEventNo[ii] = 0;
		}
		// �X�e�[�^�X�֘A������
		m_bIsConnect = false;
		CancelWaitableTimer(m_tmAddTime);
	} else {
		m_eStateMain[ID_SEND]  = SOCK_IDLE;
		m_nNowEventNo[ID_SEND] = 0;
	}

	// ���M���^�C�}�[�L�����Z��
	CancelWaitableTimer(m_tmSendOk);
	CancelWaitableTimer(m_tmStateCheck);

	// �ؒf�����̂ő������[�h��҂���Ԃɂ���
	m_emMainMode = MODE_NONE;

	// ���M�˗��̃L���[������΍폜���Ă���
	DeleteAllQue();
}

//------------------------------------------
// �񓯊� ��M�����J�n
// int id �|�[�gID
// int size �T�C�Y
// BYTE* pBuf �o�b�t�@�|�C���^
// int point ��M�f�[�^�J�n�ʒu
// �߂�l false:����ؒf���������{
//------------------------------------------
bool LedManager::RecvStart(int id, int size, BYTE* pBuf, int point)
{
	//// �񓯊� ��M
	// ��M���Z�b�g
	BaseTcpIp::DELIVERY_DATA* pDeleData = new BaseTcpIp::DELIVERY_DATA;
	pDeleData->bAnsQueing = true;	// �Œ�
	pDeleData->bRorS = true;		// ��M
	pDeleData->nStartPoint = point;
	pDeleData->nSize = size;
	pDeleData->Data = pBuf;
	pDeleData->nUserWk[0] = size+point;			// ���v��M�o�C�g���Ƃ��ăZ�b�g���Ă���
	
	// ��M�v��
	if( mcls_pSock[id]->SetRunningData(pDeleData)) {
		return true;	
	} else {
		LOG(em_ERR), "[%s] �񓯊� ��M�v�����s", my_sThreadName );
		SAFE_DELETE(pDeleData);
		return false;
	}
}

// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// DB�֘A
//------------------------------------------
// ���I�p�����[�^�擾
//------------------------------------------
bool LedManager::GetParam()
{
	CString sql;					// SQL��
	CString sWk;
	int scode;
	PARAM buf;
	memset(&buf, 0x00, sizeof(buf));

	//// ODBC�ڑ��N���X �C���X�^���X����
	OdbcBase clsDB;
	if( ! clsDB.Connect(TASKINI_NAME)) {
		// �f�[�^�x�[�X�ڑ��G���[
		LOG(em_ERR), "[%s] DB�ڑ��G���[", my_sThreadName);
		syslog(SYSNO_DB_CONNECT_ERR, "");
		return false;
	}
/// 2014.10.28 ����(���P�xLED�Ή�)------------------------->>>>
#if 0
	//// ���ʃp�����[�^
	// SQL���쐬
	sql =  "SELECT";
	if(0 == m_nMyMen) {
		sql += " scode, �Ɩ��Ɠx_�\, �Ɩ��Ɠx�␳�l_�\";

		for(int ii=2; ii<=m_nChNum; ii++) {
			sWk.Format(", �Ɩ��Ɠx%d_�\", ii);			sql += sWk;
			sWk.Format(", �Ɩ��Ɠx�␳�l%d_�\", ii);	sql += sWk;
		}

	} else {
		sql += " scode, �Ɩ��Ɠx_��, �Ɩ��Ɠx�␳�l_��";

		for(int ii=2; ii<=m_nChNum; ii++) {
			sWk.Format(", �Ɩ��Ɠx%d_��", ii);			sql += sWk;
			sWk.Format(", �Ɩ��Ɠx�␳�l%d_��", ii);	sql += sWk;
		}
	}
#else
	//// ���ʃp�����[�^
	// SQL���쐬
	sql =  "SELECT";
	if(0 == m_nMyMen) {
		if( 1 == m_nChStart )	{ sWk.Format(" scode, �Ɩ��Ɠx_�\, �Ɩ��Ɠx�␳�l_�\"); }
		else					{ sWk.Format(" scode, �Ɩ��Ɠx%d_�\, �Ɩ��Ɠx�␳�l%d_�\", m_nChStart, m_nChStart); }
		sql += sWk;

		for(int ii=m_nChStart+1; ii<=m_nChNum+m_nChStart-1; ii++) {
			sWk.Format(", �Ɩ��Ɠx%d_�\", ii);			sql += sWk;
			sWk.Format(", �Ɩ��Ɠx�␳�l%d_�\", ii);	sql += sWk;
		}

	} else {
		if( 1 == m_nChStart )	{ sWk.Format(" scode, �Ɩ��Ɠx_��, �Ɩ��Ɠx�␳�l_��", m_nChStart, m_nChStart); }
		else					{ sWk.Format(" scode, �Ɩ��Ɠx%d_��, �Ɩ��Ɠx�␳�l%d_��", m_nChStart, m_nChStart); }
		sql += sWk;

		for(int ii=m_nChStart+1; ii<=m_nChNum+m_nChStart-1; ii++) {
			sWk.Format(", �Ɩ��Ɠx%d_��", ii);			sql += sWk;
			sWk.Format(", �Ɩ��Ɠx�␳�l%d_��", ii);	sql += sWk;
		}
	}
#endif
/// 2014.10.28 ����(���P�xLED�Ή�)<<<<-------------------------	

	sql += " FROM ";
	sql += DB_PARAM_SCODE;

	//// ���s
	if( !clsDB.ExecuteDirect(sql)) {
		syslog(SYSNO_DB_EXECUTEDIRECT_ERR, "[%s]", DB_PARAM_SCODE);
		return false;
	}

	//// �擾
	while (TRUE) {
		int sqlrc = clsDB.Fetch();
		if (sqlrc == KIZUODBC_FETCH_OK) {					// �f�[�^�L��
			// ��L�[
			clsDB.GetData(1, &scode);
			if( scode > MAX_SCODE || scode <= 0 ) {
				// ��L�[�s��
				LOG(em_ERR), "[%s] [%s ��L�[�s�� scode=%d ]", my_sThreadName, DB_PARAM_SCODE, scode);
				syslog(SYSNO_DB_DATA_ERR, "[%s scode=%d]", DB_PARAM_SCODE, scode);					
				continue;
			}

			// �f�[�^��
			int nCh = 0;
			for(int ii=0; ii<m_nChNum; ii++) {
				
				while( !m_bUsedCh[nCh] ) nCh++;

				clsDB.GetData(2 + (2*ii), &buf.nLedBase[scode-1][nCh] );
				int nCnt = clsDB.GetDataImage(3 + (2*ii), &buf.nLedHosei[scode-1][nCh], MAX_LED_BLOCK );	// VarBinary

				// ����Ȃ����́A�擪�̃u���b�N�Ɠ����f�[�^���Z�b�g
				for(int jj=nCnt; jj<m_nBlock[nCh]; jj++) {
					buf.nLedHosei[scode-1][nCh][jj] = buf.nLedHosei[scode-1][nCh][0];
				}

				// 2014.12.01 ����(���P�xLED�Ή�)-------->>>>>
				//���E�t�W�J�̏ꍇ�A�����ŏƓx�␳�l�𔽓]
				if( !GetDeployment(nCh) ) {
					TurnHosei(buf.nLedHosei[scode-1][nCh], m_nBlock[nCh]);
				}
				// 2014.12.01 ����(���P�xLED�Ή�)<<<<<--------
				nCh++;
			}


		} else if( sqlrc == KIZUODBC_FETCH_NODATE ) {		// �f�[�^����
			break;	
		} else {								// �G���[
			syslog(SYSNO_DB_FETCH_ERR, "[%s]", DB_PARAM_SCODE);
			return false;
		}
	}
	
	memcpy(&m_Param, &buf, sizeof(buf));
	LOG(em_MSG), "[%s] ���I�p�����[�^�擾����", my_sThreadName);
	return true;
}

// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ���ʏ���

//------------------------------------------
// ���M�R�}���h����
// EM_LED_SEND_COMMAND em �R�}���h
// �߂�l ����
//------------------------------------------
CString LedManager::GetSendCommandName(EM_LED_SEND_COMMAND em)
{
	CString wk;
	switch( em ) {
		case SEND_CHECK_BREAK:
			wk = "�f���L��";
			break;
		case SEND_CHECK_HEATING:
			wk = "LED���M�X�e�[�^�X�v��";
			break;
		case SEND_CHECK_ONOFF:
			wk = "�_���������";
			break;
		case SEND_CHECK_TEMP:
			wk = "LED���x���";
			break;
		case SEND_EXEC_ON:
			wk = "�_���w��";
			break;
		case SEND_EXEC_OFF:
			wk = "�����w��";
			break;
		case SEND_SEND_MASTER:
			wk = "�S�̒������M";
			break;
		case SEND_SEND_VALUE:
			wk = "�����f�[�^���M";
			break;
		case SEND_WRITE_EEPROM:
			wk = "����EEPROM����";
			break;
		case SEND_READ_EEPROM:
			wk = "����EEPROM�Ǎ�";
			break;
		case SEND_READ_RAM:
			wk = "����RAM�Ǎ�";
			break;
		case SEND_GET_BLOCK:
			wk = "�u���b�N���擾";
			break;
		case SEND_NOTHING:
			wk = "";
			break;
		default :
			wk = "���[�h�ُ�";
			break;
	}
	return wk;
}
CString LedManager::GetRecvCommandName(EM_LED_RECV_COMMAND em)
{
	CString wk;
	switch( em ) {
		case RECV_NEXT_SEND_OK:
			wk = "���M����";
			break;
		case RECV_STATE_ON:
			wk = "LED�_��";
			break;
		case RECV_STATE_OFF:
			wk = "LED����";
			break;
		case RECV_STATE_BREAK:
			wk = "�f��";
			break;
		case RECV_STATE_HOT:
			wk = "���x�㏸";
			break;
		case RECV_STATE_SHATDOWN:
			wk = "���M�����޳�";
			break;
		case RECV_STATE_OK:
			wk = "����";
			break;
		case RECV_READ_EEPROM:
			wk = "����EEPROM�Ǎ�";
			break;
		case RECV_READ_RAM:
			wk = "����RAM�Ǎ�";
			break;
		case RECV_LED_TEMP:
			wk = "LED���x�Ǎ�";
			break;
		case RECV_GET_BLOCK:
			wk = "�u���b�N���Ǎ�";
			break;
		case RECV_NOTHING:
			wk = "�����R�}���h";
			break;
		case RECV_CMD_ERROR:
			wk = "�R�}���h�G���[";
			break;
		default :
			wk = "���[�h�ُ�";
			break;
	}
	return wk;
}

//------------------------------------------
// ��M�R�[�h�̓���
// char code �擪1����
// �߂�l ��M�R�}���h
//------------------------------------------
LedManager::EM_LED_RECV_COMMAND LedManager::GetRecvCommand(char code)
{
	switch( code ) {
		case 'C': return RECV_NEXT_SEND_OK;
		case 'x': return RECV_STATE_ON;
		case 'y': return RECV_STATE_OFF;
		case 'A': return RECV_STATE_BREAK;
		case 'B': return RECV_STATE_HOT;
		case 'D': return RECV_STATE_SHATDOWN;
		case 'N': return RECV_STATE_OK;
		case 'E': return RECV_READ_EEPROM;
		case 'P': return RECV_READ_RAM;
		case 't': return RECV_LED_TEMP;
		case 'b': return RECV_GET_BLOCK;
		case 'T': return RECV_TIME_OUT;
		case 'e': return RECV_CMD_ERROR;
		case 0x80: return RECV_NOTHING;
		default : return RECV_NON;
	}
}

LedManager::EM_LED_SEND_COMMAND LedManager::GetSendCommand(char code)
{
	switch( code ) {
		case 'a': return SEND_CHECK_BREAK;
		case 'b': return SEND_CHECK_HEATING;
		case 'l': return SEND_CHECK_ONOFF;
		case 't': return SEND_CHECK_TEMP;
		case 'X': return SEND_EXEC_ON;
		case 'Y': return SEND_EXEC_OFF;
		case 'T': return SEND_SEND_MASTER;
		case 'S': return SEND_SEND_VALUE;
		case 'W': return SEND_WRITE_EEPROM;
		case 'M': return SEND_READ_EEPROM;
		case 'R': return SEND_READ_RAM;
		case 'B': return SEND_GET_BLOCK;
		case 0x00: return SEND_NOTHING;
		default : return SEND_NON;
	}
}

//------------------------------------------
// ���C�����[�h����
// EM_LED_MAINMODE em ���[�h
// �߂�l ����
//------------------------------------------
CString LedManager::GetMainModeName(EM_LED_MAINMODE em)
{
	CString wk;
	switch( em ) {
		case MODE_NONE:
			wk = "�ؒf��";
			break;
		case MODE_WAIT:
			wk = "�ҋ@";
			break;
		case MODE_RECV_EXEC:
			wk = "��M�R�}���h�҂�";
			break;
		default :
			wk = "���[�h�ُ�";
			break;
	}
	return wk;
}

// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// �_���ݐώ��Ԋ֌W

//------------------------------------------
// �ݐϊJ�n
// int	nCh �Ώۃ`�����l�� (1�`:�w��`�����l���̂�)
//------------------------------------------
void LedManager::AddTime_Start(int nCh)
{
	if(0 == m_nIdTotalTime[nCh-1]) return;

	__int64 ts = (__int64)PARAM_ADDTIME * 60 * 1000 -10000;	// 1�ʕb��100�i�m�b (�}�C�i�X�w��ŁA�w�莞�Ԍo�߂ŃV�O�i��) (��3������2��ڈȍ~�̋N���Ԋu)
	SetWaitableTimer(m_tmAddTime[nCh-1],  (LARGE_INTEGER *)&ts, (PARAM_ADDTIME * 60 * 1000), NULL, NULL, FALSE);
}
//------------------------------------------
// �ݐϒ�~
// int	nCh �Ώۃ`�����l�� (1�`:�w��`�����l���̂�)
//------------------------------------------
void LedManager::AddTime_Stop(int nCh)
{
	CancelWaitableTimer(m_tmAddTime[nCh-1]);
}

//------------------------------------------
// �ݐω��Z
// int	nCh �Ώۃ`�����l�� (1�`:�w��`�����l���̂�)
//------------------------------------------
void LedManager::AddTime_Exec(int nCh)
{
	CString sql;					// SQL��

	//// ODBC�ڑ��N���X �C���X�^���X����
	OdbcBase clsDB;
	if( ! clsDB.Connect(TASKINI_NAME)) {
		// �f�[�^�x�[�X�ڑ��G���[
		LOG(em_ERR), "[%s] DB�ڑ��G���[", my_sThreadName);
		syslog(SYSNO_DB_CONNECT_ERR, "");
		return;
	}

	sql.Format("UPDATE %s SET �@����=�@����+%d WHERE �@��ID=%d", DB_STATUS_INF, PARAM_ADDTIME, m_nIdTotalTime[nCh-1]);
	if( !clsDB.ExecuteDirect(sql)) {
		LOG(em_ERR), "[%s] LED�Ɩ��ғ��݌v���� ���Z���s [CH=%d]", my_sThreadName, nCh);
		syslog(SYSNO_DB_EXECUTEDIRECT_ERR, "[%s:CH=%d]", DB_STATUS_INF, nCh);
		return;
	}
}
//------------------------------------------
// �ݐσ��Z�b�g
// int	nCh �Ώۃ`�����l�� (0:�S�`�����l��  1�`:�w��`�����l���̂�)
//------------------------------------------
void LedManager::AddTime_Reset(int nCh)
{
	int ii_s, ii_e;
	GetChLoop(nCh, ii_s, ii_e);

	for(int ii=ii_s; ii<=ii_e; ii++) {

		if( !m_bUsedCh[ii-1] ) continue;	// ���g�pCH�̏ꍇ�A���Z�b�g���Ȃ�
		KizuFunction::SetStatus(m_nIdTotalTime[ii-1], 0, false);
	}
}

//------------------------------------------
// LED���x�Z�b�g�i���ݏ�ԁj
// int	nCh �Ώۃ`�����l�� (1�`:�w��`�����l���̂�)
//------------------------------------------
void LedManager::SetTemp(int nCh)
{
	for(int ii=0; ii<m_nTempUnit[nCh-1]; ii++) {
		if ( m_nTempThrDown <= m_nTemp[nCh-1][ii] ) {		// ��������+���x�ُ�

			AddQue_SendCommand_Index(SEND_EXEC_OFF, m_nMyIndex[nCh-1]);	// ��������

			if (2 != m_nTempSts[nCh-1][ii]) {
				LOG(em_ERR), "[%s][CH=%d] LED�Ɩ����x�ُ�+��������(%d) %d��", my_sThreadName, nCh, ii+1, m_nTemp[nCh-1][ii]);
				if(0!=m_nIdTempSt[nCh-1][ii]) KizuFunction::SetStatus(m_nIdTempSt[nCh-1][ii], false);
			}
			m_nTempSts[nCh-1][ii] = 2;

		} else if ( m_nTempThr <= m_nTemp[nCh-1][ii] ) {	// ���x�ُ�
			if (1 != m_nTempSts[nCh-1][ii]) {
				LOG(em_ERR), "[%s][CH=%d] LED�Ɩ����x�ُ�(%d) %d��", my_sThreadName, nCh, ii+1, m_nTemp[nCh-1][ii]);
				if(0!=m_nIdTempSt[nCh-1][ii]) KizuFunction::SetStatus(m_nIdTempSt[nCh-1][ii], false);
			}
			m_nTempSts[nCh-1][ii] = 1;
		} else {
			if ( 0 != m_nTempSts[nCh-1][ii]) {
				LOG(em_MSG), "[%s][CH=%d] LED�Ɩ����x����(%d) %d��", my_sThreadName, nCh, ii+1, m_nTemp[nCh-1][ii]);
				if(0!=m_nIdTempSt[nCh-1][ii]) KizuFunction::SetStatus(m_nIdTempSt[nCh-1][ii], true);
			}
			m_nTempSts[nCh-1][ii] = 0;
		}

		if(0!=m_nIdTemp[nCh-1][ii]) KizuFunction::SetStatus(m_nIdTemp[nCh-1][ii], m_nTemp[nCh-1][ii], false);
	}
}

/// 2014.12.01 ����(���P�xLED�Ή�) ------------------------------------------------>>>>
//==========================================
// �Ɠx�␳�l�̃o�C�i����𔽓]
//------------------------------------------
// BYTE *	nHosei		: �Ɠx�␳�l�̃o�C�i����(�擪�A�h���X)	
// int		nCnt		: �Ɠx�␳�l�̃u���b�N��	
//==========================================
void LedManager::TurnHosei(BYTE * nHosei, int nCnt)
{
	BYTE Wk[MAX_LED_BLOCK];
	
	for(int ii=0; ii<nCnt; ii++){
		Wk[ii] = nHosei[nCnt-ii-1];
	}

	memcpy(nHosei, &Wk, nCnt);
}

//====================================================
// ����M����(INDEX�w���)
//----------------------------------------------------
// Index�ɊY������CH���w�肵�ď]���̑���M�������R�[��
//====================================================

//==========================================
// 1�o�C�g�����̑��M�R�}���h��p
//------------------------------------------
// EM_LED_SEND_COMMAND	em		�F���M�R�}���h	
// int					nIndex	�FIndex
//==========================================
void LedManager::AddQue_SendCommand_Index(EM_LED_SEND_COMMAND em, int nIndex)
{
	for(int ii=0; ii<MAX_LED_CH; ii++){
		if( IsTargetIndex(ii+1, nIndex) ){
			AddQue_SendCommand(em, ii+1);
		}
	}
}

//==========================================
// �}�X�^�[�{�z���f�[�^�Z�b�g
//------------------------------------------
// int		nIndex		: Index		
// BYTE		nLedBase	: �S�̒���
// BYTE*	nLedHosei	: �z���f�[�^
//==========================================
void LedManager::AddQue_SetLightMastVal_Index(int nIndex, BYTE nLedBase, BYTE* nLedHosei)
{
	for(int ii=0; ii<MAX_LED_CH; ii++){
		if( IsTargetIndex(ii+1, nIndex) ){
			AddQue_SetLightMastVal(ii+1, nLedBase, nLedHosei);
		}
	}
}

//==========================================
// �}�X�^�[�{�z���f�[�^�Z�b�g (scode)
//------------------------------------------
// int		nIndex		: Index	
// int		scode		: �����\�ʏ��
//==========================================
void LedManager::AddQue_SetLightMastVal_Index(int nIndex, int scode)
{
	for(int ii=0; ii<MAX_LED_CH; ii++){
		if( IsTargetIndex(ii+1, nIndex) ){
			AddQue_SetLightMastVal(ii+1, scode);
		}
	}
}

//=================================================
// �}�X�^�[�{�z���f�[�^�Z�b�g (�z�����͑S������l)
//-------------------------------------------------
// int		nIndex		: Index
// BYTE		nLedBase	: �S�̒���
// BYTE		nLedHosei	: �z���f�[�^
//=================================================
void LedManager::AddQue_SetLightMastVal_Index(int nIndex, BYTE nLedBase, BYTE nLedHosei)
{
	for(int ii=0; ii<MAX_LED_CH; ii++){
		if( IsTargetIndex(ii+1, nIndex) ){
			AddQue_SetLightMastVal(ii+1, nLedBase, nLedHosei);
		}
	}
}

//==========================================
// �S�̒����f�[�^�Z�b�g
//------------------------------------------
// int	nIndex		: Index
// int	nLedBase	: �S�̒���
//==========================================
void LedManager::AddQue_SetLightMaster_Index(int nIndex, int nLedBase)
{
	for(int ii=0; ii<MAX_LED_CH; ii++){
		if( IsTargetIndex(ii+1, nIndex) ){
			AddQue_SetLightMaster(ii+1, nLedBase);
		}
	}
}

//==========================================
// �����f�[�^�Z�b�g (�z�����͈ꊇ�w��)
//------------------------------------------
// int		nIndex		: Index
// BYTE*	nLedHosei	: �z���f�[�^
//==========================================
void LedManager::AddQue_SetLightValue_Index(int nIndex, BYTE* nLedHosei)
{
	for(int ii=0; ii<MAX_LED_CH; ii++){
		if( IsTargetIndex(ii+1, nIndex) ){
			AddQue_SetLightValue(ii+1, nLedHosei);
		}
	}
}

//==========================================
// �z���f�[�^�Z�b�g (�z�����͑S������l)
//------------------------------------------
// int		nIndex		: Index
// BYTE		nLedHosei	: �z���f�[�^	
//==========================================
void LedManager::AddQue_SetLightValue_Index(int nIndex, BYTE nLedHosei)
{
	for(int ii=0; ii<MAX_LED_CH; ii++){
		if( IsTargetIndex(ii+1, nIndex) ){
			AddQue_SetLightValue(ii+1, nLedHosei);
		}
	}
}

//==========================================
// �����f�[�^�Z�b�g (�z�����͌ʎw��)
//------------------------------------------
// int	nIndex		: Index		
// int	nBlock		: �u���b�N
// BYTE	nLedHosei	: �z���f�[�^
//==========================================
void LedManager::AddQue_SetLightValue_Index(int nIndex, int nBlock, BYTE nLedHosei)
{
	for(int ii=0; ii<MAX_LED_CH; ii++){
		if( IsTargetIndex(ii+1, nIndex) ){
			AddQue_SetLightValue(ii+1, nBlock, nLedHosei);
		}
	}
}

//==========================================
// �ݐϓ_�����ԃ��Z�b�g 
//------------------------------------------
// int	nIndex		: Index	
//==========================================
void LedManager::AddTime_Reset_Index(int nIndex)
{
	for(int ii=0; ii<MAX_LED_CH; ii++){
		if( IsTargetIndex(ii+1, nIndex) ){
			AddTime_Reset(ii+1);
		}
	}
}
/// 2014.12.01 ����(���P�xLED�Ή�) <<<<------------------------------------------------