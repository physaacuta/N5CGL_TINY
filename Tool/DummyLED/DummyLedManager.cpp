#include "StdAfx.h"
#include "DummyLEDManager.h"

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
//------------------------------------------
DummyLedManager::DummyLedManager(char const* cName, int nId, int nBlock, int nTempUnit) :
ThreadManager(cName),
m_nMyID(nId),
m_bIsConnect(false),
m_emMainMode(MODE_NONE),
m_nNowScode(MAX_SCODE)					// �\�ʏ�Ԃ́A�f�t�H���g�\�ʏ�ԂƂ���
{
	CString  sSession;

	// �����̐ݒ�L�[������
	m_cMySession.Format("%s_%s", DummyLED, cName );

	//// ����̐ݒ�L�[������
	m_cYourSession.Format("%s_%s", KT_LEDMGR, cName );
	m_nMyMen		= GetPrivateProfileInt(m_cYourSession, "MEN",		0, TASKINI_NAME);
	m_nChNum		= GetPrivateProfileInt(m_cYourSession, "CH_NUM",	0, TASKINI_NAME);
	_ASSERT(0!=m_nChNum);


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

	for(int ii=0; ii<MAX_LED_CH-1; ii++){
		// ���g�pCH�̏ꍇ�͎���
		if( !m_bUsedCh[ii] )	continue;

		sSession.Format("%s_%s_CH%d", KT_LEDMGR, cName, ii+1 );
		m_nBlock[ii]    = GetPrivateProfileInt(sSession, "BLOCK",     nBlock,    TASKINI_NAME);		// �z���u���b�N��(����`�̏ꍇ�A���ʐݒ�̃u���b�N��)
		m_nMyIndex[ii]  = GetPrivateProfileInt(sSession, "INDEX",     ii+1,      TASKINI_NAME);		// �Ή�����Index(����`�̏ꍇ�ACh�ԍ�)
		m_nTempUnit[ii] = GetPrivateProfileInt(sSession, "TEMP_UNIT", nTempUnit, TASKINI_NAME);		// LED���x�v���_��(����`�̏ꍇ�A���ʐݒ��LED���x�v���_��)
	}

	//// ������
	for(int ii=0; ii<PARAM_SOCK_MAX; ii++) {
		mcls_pSock[ii]		= NULL;
		m_eStateMain[ii]	= SOCK_NONE;
	}
	for(int ii=0; ii<MAX_LED_CH; ii++) {
		m_nNowLedBase[ii]	= 0;
		m_nStateBreak[ii]	= 0;
		m_nStateTemp[ii]	= 0;
		m_nStateOnOff[ii]	= 1;
		for (int jj=0; jj<MAX_LED_BLOCK; jj++) {
			m_nNowLedHosei[ii][jj]	= 0;
		}

		for (int jj=0; jj<MAX_TEMP_UNIT; jj++) {
			m_nTemp[ii][jj]		= 20;
			m_nTempSts[ii][jj]	= 0;
		}
		m_nLedOnOff[ii] = 0;
	}
	memset( m_cSockBuf, 0x00, sizeof(m_cSockBuf));

}

//------------------------------------------
// �f�X�g���N�^
//------------------------------------------
DummyLedManager::~DummyLedManager(void)
{
}


//------------------------------------------
// �X���b�h�J�n�O���� (�X���b�h���s����(�V�O�i���Z�b�g�O)�ɃR�[���o�b�N)
//------------------------------------------
int DummyLedManager::ThreadFirst()
{
	// TCP/IP ����N���X������
	SockInit(); 			

	//// �C�x���g�ݒ�
	HANDLE hArray[] = { mcls_pSock[PORT_SEND]->gque_Anser.g_evSem,
						mque_SendItem.g_evSem
						};
	SetEventNum(sizeof(hArray)/sizeof(HANDLE), hArray);

	return 0;
}


//------------------------------------------
// �X���b�h�I���O���� (�I���V�O�i����ɃR�[���o�b�N)
//------------------------------------------
int DummyLedManager::ThreadLast()
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
void DummyLedManager::ThreadWaitFor(int nDefWaitNum, int* nNowWaitNum)
{
	// ��M���́A���M�v�����󂯕t���Ȃ����߂̏��u
	if( SOCK_EXEC == m_eStateMain[ID_SEND] )		*nNowWaitNum = nDefWaitNum-1;
	else											*nNowWaitNum = nDefWaitNum;
}

//------------------------------------------
// �X���b�h�C�x���g����
// int nEventNo �C�x���gNo (0�I���W��)
//------------------------------------------
void DummyLedManager::ThreadEvent(int nEventNo)
{
	BaseTcpIp::DELIVERY_ANS*	queAns  = NULL;			// TCP/IP�N���X ���쌋��

	////// �V�O�i���̏���
	enum {	EV_SOCK_SEND_ANS = 0,			// ���M�|�[�g ���쌋��
			EV_SOCK_SEND_REQ
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

	case EV_SOCK_SEND_REQ:

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
		break;
	}
}

//------------------------------------------
// TCP/IP�N���X ���쌋�ʂ̏���
// int id �|�[�gID (�悭�g��������������Ă���)
// BaseTcpIp::DELIVERY_ANS* que ���쌋��
//------------------------------------------
void DummyLedManager::ExecAnsQue(int id, BaseTcpIp::DELIVERY_ANS* que)
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
			}
			
			// �ڑ������̂ő������[�h��҂���Ԃɂ���
			m_emMainMode = MODE_WAIT;

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
void DummyLedManager::DeleteAllQue()
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
// int	nCh �Ώۃ`�����l��
// �߂�l ���A���
//------------------------------------------
bool DummyLedManager::AddQue_SendCommand(EM_LED_SEND_COMMAND em, int nCh)
{
	int ii_s, ii_e;

	//	//======================================================
	// ���O�`�F�b�N

	if( !m_bUsedCh[nCh] ) return true;	// ���g�pCH�̏ꍇ�A���M���Ȃ�

	//======================================================
	//// ����
	INF_SEND* pDeli = new INF_SEND;
	memset(pDeli, 0x00, sizeof(INF_SEND));

	pDeli->emLedCommand = em;
	pDeli->nSize = PARAM_COM_BUFSIZE;
	pDeli->nCh	 = nCh+1;

	//// �R�}���h�Z�b�g
	pDeli->cBuf[0] = 'A';
	switch( em ) {
		case SEND_NEXT_SEND_OK:
			pDeli->cBuf[1] = 'C';
			break;
		case SEND_STATE_ON:
			pDeli->cBuf[1] = 'x';
			break;
		case SEND_STATE_OFF:
			pDeli->cBuf[1] = 'y';
			break;
		case SEND_STATE_BREAK:
			pDeli->cBuf[1] = 'A';
			break;
		case SEND_STATE_HOT:
			pDeli->cBuf[1] = 'B';
			break;
		case SEND_STATE_SHATDOWN:
			pDeli->cBuf[1] = 'D';
			break;
		case SEND_STATE_OK:
			pDeli->cBuf[1] = 'N';
			break;
		case SEND_READ_EEPROM:
			pDeli->cBuf[1] = 'E';
			pDeli->cBuf[5] = m_nNowLedBase[nCh];
			memcpy( &pDeli->cBuf[6], m_nNowLedHosei[nCh],m_nBlock[nCh]);
			pDeli->nSize = PARAM_COM_BUFSIZE + m_nBlock[nCh] + 1;
			break;
		//case SEND_READ_RAM:
		//	pDeli->cBuf[1] = 'P';
		//	pDeli->cBuf[5] = 255;
		//	for(int ii=0; ii<m_nBlock[nCh]; ii++) pDeli->cBuf[6+ii] = 1;
		//	break;
		case SEND_LED_TEMP:
			pDeli->cBuf[1] = 't';
			for(int ii=0; ii<m_nTempUnit[nCh]; ii++) {
				pDeli->cBuf[5+ii] = (char) m_nTemp[nCh][ii];
			}
			pDeli->nSize = PARAM_COM_BUFSIZE + m_nTempUnit[nCh];
			break;
		case SEND_GET_BLOCK:
			pDeli->cBuf[1] = 'b';
			pDeli->cBuf[5] = m_nBlock[nCh];
			break;
	}
		pDeli->cBuf[2] = 0x01;
		pDeli->cBuf[3] = 0x01 + nCh;		// �`�����l��
		pDeli->cBuf[4] = 0x00;

	// �ǉ�
	if( ! mque_SendItem.AddToTail(pDeli, 20, 20) ) {		// ��Γo�^
		// ��Γo�^�������E�E�E
		LOG(em_ERR), "[%s] ���M�˗��L���[�t�� [CH=%d] [%s]", my_sThreadName, nCh+1, GetSendCommandName(em));
		syslog(SYSNO_QUEFULL_ERR, "[%s:CH=%d:%s]", my_sThreadName, nCh+1, GetSendCommandName(em));
	
		// �̈���
		delete pDeli;
		return false;
	}
	return true;
}


//------------------------------------------
// ���M�J�n
// int* retStat ���M�˗����[�h(�߂�l)
// �߂�l false:�ؒf���Ă˗v��
//------------------------------------------
bool DummyLedManager::SendStart(int* retStat)
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

	return true;
}

//------------------------------------------
// ��M����
// int id �|�[�gID
// BaseTcpIp::DELIVERY_ANS* que ���쌋��
// �߂�l 0:����������M  -1:�ُ�(�ؒf���Ă�) 1:�R�}���h�ɑ΂���A���T�[����M  9:���M������M
//------------------------------------------
int DummyLedManager::RecvEnd(int id, BaseTcpIp::DELIVERY_ANS* que)
{
	// �R�}���h����
	EM_LED_RECV_COMMAND  emRecv = GetRecvCommand(m_cSockBuf[PORT_SEND][BUF_RECV][1]);


	// ///////////////////////////////////////////////////////////////////
	// �O����
	//

	// ��M�R�}���h�̒����Ɋ֌W����ׁA��ɓ���
	m_InfRecv.nCh	= m_cSockBuf[PORT_SEND][BUF_RECV][3];	// �`�����l��
	int nCh = m_InfRecv.nCh-1;

	// �f�[�^���������M�R�}���h����M
	// ��M�R�}���h�̎�ʂŁA�f�[�^���̒��������߂ĕ�����̂ŁA�S�̒������Ē�`
	if( RECV_SEND_VALUE == emRecv) {
		if (PARAM_COM_BUFSIZE == que->Data->nSize) {
			// �c�����M
			m_InfRecv.nSize = PARAM_COM_BUFSIZE + 1 + m_nBlock[nCh];		// ��M�R�}���h + LED���x�i�[�_��
			if( ! RecvStart( id, 1 + m_nBlock[nCh], m_cSockBuf[id][BUF_RECV], PARAM_COM_BUFSIZE)) return -1;
			return 0;
		}
	//} else 	if( RECV_READ_EEPROM == emRecv || RECV_READ_RAM == emRecv) {
	//	if (PARAM_COM_BUFSIZE == que->Data->nSize) {
	//		// �c�����M
	//		m_InfRecv.nSize = PARAM_COM_BUFSIZE + 1 + m_nBlock[nCh]; // ��M�R�}���h + �S�̒����f�[�^ �{ �␳�f�[�^�u���b�N��
	//		if( ! RecvStart( id, m_InfRecv.nSize, m_cSockBuf[id][BUF_RECV], PARAM_COM_BUFSIZE)) return -1;
	//		return 0;
	//	}

	} else {
		m_InfRecv.nSize = que->Data->nSize;
	}

	memcpy(&m_InfRecv.cBuf, &m_cSockBuf[PORT_SEND][BUF_RECV], m_InfRecv.nSize);


	// ����ڑ��F�� (������M������OK)
	if( ! m_bIsConnect) {
		LOG(em_MSG), "[%s] �Ɩ����u ��������", my_sThreadName);
		syslog(SYSNO_SOCK_CONNECT, "[%s]", my_sThreadName);

		m_bIsConnect = true;
	}

	// ///////////////////////////////////////////////////////////////////
	// �P�R�}���h��M������
	//
	switch(emRecv) {
			// �f���L�� (�X�e�[�^�X�v��)
			case RECV_CHECK_BREAK:
				AddQue_SendCommand(CheckBreak(nCh),nCh);
				break;
			// �_��������� (�X�e�[�^�X�v��)
			case RECV_CHECK_ONOFF:
				AddQue_SendCommand(CheckOnOff(nCh),nCh);
				break;
			// LED���M��� (�X�e�[�^�X�v��)
			case RECV_CHECK_HEATING:
				AddQue_SendCommand(CheckHeating(nCh),nCh);
				break;
			// LED���x��� (�X�e�[�^�X�v��)
			case RECV_CHECK_TEMP:
				AddQue_SendCommand(SEND_LED_TEMP,nCh);
				break;
			// �_���w��
			case RECV_EXEC_ON:
				if (1 == m_nLedOnOff[nCh]) {		// ��������ON��
					m_nStateOnOff[nCh]=1;
					AddQue_SendCommand(SEND_STATE_OFF,nCh);
				} else {
					m_nStateOnOff[nCh]=0;
					AddQue_SendCommand(SEND_STATE_ON,nCh);
				}
				break;
			// �����w��
			case RECV_EXEC_OFF:
				m_nStateOnOff[nCh]=1;
				AddQue_SendCommand(SEND_STATE_OFF,nCh);
				break;
			// �����f�[�^���M (RAM�ɃZ�b�g)
			case RECV_SEND_VALUE:
				GetMasterData(nCh);
				break;
			// RAM�̒����f�[�^��EEPROM(�t�@�C��)�ɏ���
			case RECV_WRITE_EEPROM:
				break;
			// EEPROM(�t�@�C��)�̒����f�[�^��Ǎ�
			case RECV_READ_EEPROM:
				AddQue_SendCommand(SEND_READ_EEPROM,nCh);
				break;
			// RAM�̒����f�[�^��Ǎ�
			case RECV_READ_RAM:
				break;
			// �u���b�N���擾
			case RECV_GET_BLOCK:
				AddQue_SendCommand(SEND_GET_BLOCK,nCh);
				break;
	}
	//if( RECV_READ_EEPROM != emRecv)
		AddQue_SendCommand(SEND_NEXT_SEND_OK,nCh);
	// �܂��ŏ������M�J�n
	if( ! RecvStart( id, PARAM_COM_BUFSIZE, m_cSockBuf[id][BUF_RECV] )) {
		return -1;
	}

	// ��n��
	m_emMainMode = MODE_WAIT;
	return 0;
}


// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// TPC/IP �ʐM����

//------------------------------------------
// ���������� (��x�̂݋N��)
//------------------------------------------
void DummyLedManager::SockInit()
{
	BaseTcpIp::DELIVERY_COMMON	deli;

	//// ���M�p�X���b�h ����
	// ��`
	deli.nMyID = PORT_SEND;
	deli.bSorC = true;
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
void DummyLedManager::SockExit()
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
void DummyLedManager::UserClose(int nClose)
{
	// �ǂ̃|�[�g����ł� �N���[�Y�ʒm��������A�S�|�[�g�����I��������
	mcls_pSock[PORT_SEND]->UserClose();
	LOG(em_ERR), "[%s] ����ُ� ===================================>>>>", my_sThreadName);

	for(int ii=0; ii<PARAM_SOCK_MAX; ii++) {
		m_eStateMain[ii]  = SOCK_NONE;
		//m_nNowEventNo[ii] = 0;
	}
	// �X�e�[�^�X�֘A������
	m_bIsConnect = false;

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
bool DummyLedManager::RecvStart(int id, int size, BYTE* pBuf, int point)
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
// ���ʏ���

//------------------------------------------
// ���M�R�}���h����
// EM_LED_SEND_COMMAND em �R�}���h
// �߂�l ����
//------------------------------------------
CString DummyLedManager::GetRecvCommandName(EM_LED_RECV_COMMAND em)
{
	CString wk;
	switch( em ) {
		case RECV_CHECK_BREAK:
			wk = "�f���L��";
			break;
		case RECV_CHECK_HEATING:
			wk = "LED���M�X�e�[�^�X�v��";
			break;
		case RECV_CHECK_ONOFF:
			wk = "�_���������";
			break;
		case RECV_CHECK_TEMP:
			wk = "LED���x���";
			break;
		case RECV_EXEC_ON:
			wk = "�_���w��";
			break;
		case RECV_EXEC_OFF:
			wk = "�����w��";
			break;
		case RECV_SEND_MASTER:
			wk = "�S�̒������M";
			break;
		case RECV_SEND_VALUE:
			wk = "�����f�[�^���M";
			break;
		case RECV_WRITE_EEPROM:
			wk = "����EEPROM����";
			break;
		case RECV_READ_EEPROM:
			wk = "����EEPROM�Ǎ�";
			break;
		case RECV_READ_RAM:
			wk = "����RAM�Ǎ�";
			break;
		case RECV_GET_BLOCK:
			wk = "�u���b�N���擾";
			break;
		case RECV_NOTHING:
			wk = "";
			break;
		default :
			wk = "���[�h�ُ�";
			break;
	}
	return wk;
}
CString DummyLedManager::GetSendCommandName(EM_LED_SEND_COMMAND em)
{
	CString wk;
	switch( em ) {
		case SEND_NEXT_SEND_OK:
			wk = "���M����";
			break;
		case SEND_STATE_ON:
			wk = "LED�_��";
			break;
		case SEND_STATE_OFF:
			wk = "LED����";
			break;
		case SEND_STATE_BREAK:
			wk = "�f��";
			break;
		case SEND_STATE_HOT:
			wk = "���x�㏸";
			break;
		case SEND_STATE_SHATDOWN:
			wk = "���M�����޳�";
			break;
		case SEND_STATE_OK:
			wk = "����";
			break;
		case SEND_READ_EEPROM:
			wk = "����EEPROM�Ǎ�";
			break;
		case SEND_READ_RAM:
			wk = "����RAM�Ǎ�";
			break;
		case SEND_LED_TEMP:
			wk = "LED���x�Ǎ�";
			break;
		case SEND_GET_BLOCK:
			wk = "�u���b�N���Ǎ�";
			break;
		case SEND_NOTHING:
			wk = "�����R�}���h";
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
DummyLedManager::EM_LED_SEND_COMMAND DummyLedManager::GetSendCommand(char code)
{
	switch( code ) {
		case 'C': return SEND_NEXT_SEND_OK;
		case 'x': return SEND_STATE_ON;
		case 'y': return SEND_STATE_OFF;
		case 'A': return SEND_STATE_BREAK;
		case 'B': return SEND_STATE_HOT;
		case 'D': return SEND_STATE_SHATDOWN;
		case 'N': return SEND_STATE_OK;
		case 'E': return SEND_READ_EEPROM;
		case 'P': return SEND_READ_RAM;
		case 't': return SEND_LED_TEMP;
		case 'b': return SEND_GET_BLOCK;
		case 'T': return SEND_TIME_OUT;
		case 0x80: return SEND_NOTHING;
		default : return SEND_NON;
	}
}

DummyLedManager::EM_LED_RECV_COMMAND DummyLedManager::GetRecvCommand(char code)
{
	switch( code ) {
		case 'a': return RECV_CHECK_BREAK;
		case 'b': return RECV_CHECK_HEATING;
		case 'l': return RECV_CHECK_ONOFF;
		case 't': return RECV_CHECK_TEMP;
		case 'X': return RECV_EXEC_ON;
		case 'Y': return RECV_EXEC_OFF;
		case 'T': return RECV_SEND_MASTER;
		case 'S': return RECV_SEND_VALUE;
		case 'W': return RECV_WRITE_EEPROM;
		case 'M': return RECV_READ_EEPROM;
		case 'R': return RECV_READ_RAM;
		case 'B': return RECV_GET_BLOCK;
		case 0x00: return RECV_NOTHING;
		default : return RECV_NON;
	}
}

//------------------------------------------
// ���C�����[�h����
// EM_LED_MAINMODE em ���[�h
// �߂�l ����
//------------------------------------------
CString DummyLedManager::GetMainModeName(EM_LED_MAINMODE em)
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
// ��Ԋm�F
//------------------------------------------
// �f����ԃ`�F�b�N
// nCh	�`�����l��
// �߂�l ����
//------------------------------------------
DummyLedManager::EM_LED_SEND_COMMAND DummyLedManager::CheckBreak(int nCh)
{
	if(0 == m_nStateBreak[nCh])
		return SEND_STATE_OK;
	else
		return SEND_STATE_BREAK;
}

//------------------------------------------
// �_��������ԃ`�F�b�N
// nCh	�`�����l��
// �߂�l ����
//------------------------------------------
DummyLedManager::EM_LED_SEND_COMMAND DummyLedManager::CheckOnOff(int nCh)
{
	if(0 == m_nStateOnOff[nCh])
		return SEND_STATE_ON;
	else
		return SEND_STATE_OFF;
}

//------------------------------------------
// ���x��ԃ`�F�b�N�i���j�b�g�ʁj
// nCh	�`�����l��
// nUnit ���j�b�g
// �߂�l ����
//------------------------------------------
DummyLedManager::EM_LED_SEND_COMMAND DummyLedManager::CheckHeating(int nCh, int nUnit)
{
	if(0 == m_nTempSts[nCh][nUnit - 1])
		return SEND_STATE_OK;
	else if(0 == m_nTempSts[nCh][nUnit - 1])
		return SEND_STATE_HOT;
	else
		return SEND_STATE_SHATDOWN;
}

//------------------------------------------
// ���x��ԃ`�F�b�N�i�S���j�b�g�j
// nCh	�`�����l��
// �߂�l ����
//------------------------------------------
DummyLedManager::EM_LED_SEND_COMMAND DummyLedManager::CheckHeating(int nCh)
{
	EM_LED_SEND_COMMAND bRet = SEND_STATE_OK;
	for(int ii = 0; ii<m_nBlock[nCh]; ii++){
		if(1 == m_nTempSts[nCh][ii])
			bRet = SEND_STATE_HOT;
		else if(2 == m_nTempSts[nCh][ii])
			bRet = SEND_STATE_SHATDOWN;
	}
	return bRet;
}

//------------------------------------------
// �����f�[�^�擾
// nCh	�`�����l��
// �߂�l ����
//------------------------------------------
void DummyLedManager::GetMasterData(int nCh)
{	
	m_nNowLedBase[nCh] = m_InfRecv.cBuf[5];
	memcpy(m_nNowLedHosei[nCh], &m_InfRecv.cBuf[6], m_nBlock[nCh]);
}

