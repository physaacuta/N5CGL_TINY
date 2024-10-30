#include "StdAfx.h"
#include "SockManager.h"

// �������[���[�N���o ----->>>
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif
// <<<------------------------

//------------------------------------------
// �R���X�g���N�^
//------------------------------------------
SockManager::SockManager(void) : 
m_bIsConnect(false),
m_bDumpMode(false),
//mcls_pLog(NULL),
ThreadManager("SockMgr")
{
	// ������
	for(int ii=0; ii<PARAM_SOCK_MAX; ii++) {
		mcls_pSock[ii] = NULL;
		m_eStateMain[ii] = SOCK_NONE;
		m_eStateNowRecv[ii] = SOCK_NR_RES;
		m_nNowEventNo[ii] = 0;
	}

	//// �V�O�i������
	m_evAnserTimeOut = CreateWaitableTimer(NULL, FALSE, NULL);	// �����`���^�C���A�E�g�V�O�i��
	m_evHelcyTimeOut = CreateWaitableTimer(NULL, FALSE, NULL);	// �w���V�[�`����M�^�C���A�E�g�V�O�i��
}

//------------------------------------------
// �f�X�g���N�^
//------------------------------------------
SockManager::~SockManager(void)
{
	//// �����o�[ ���
	//delete mcls_pData;
	// �V�O�i���J��
	CloseHandle(m_evAnserTimeOut);
	CloseHandle(m_evHelcyTimeOut);
}

//------------------------------------------
// �X���b�h�J�n�O���� (�X���b�h���s����(�V�O�i���Z�b�g�O)�ɃR�[���o�b�N)
//------------------------------------------
int SockManager::ThreadFirst()
{
	//// ���O����
	SockInit();

	//// �C�x���g�ݒ�
	// WaitForMultipleObjects�C�x���g
	HANDLE	hArray[] = {
		m_evAnserTimeOut,
		m_evHelcyTimeOut,
		mcls_pSock[PORT_RECV]->gque_Anser.g_evSem,
		mcls_pSock[PORT_SEND]->gque_Anser.g_evSem,
		mque_pDeli->g_evSem,
	};
	SetEventNum(sizeof(hArray) / sizeof(HANDLE), hArray);
	return 0;
}

//------------------------------------------
// �X���b�h�I���O���� (�I���V�O�i����ɃR�[���o�b�N)
//------------------------------------------
int SockManager::ThreadLast()
{
	//// ��n��
	SockExit();

	return 0;
}


//------------------------------------------
// �X���b�h�C�x���g����
// int nEventNo �C�x���gNo (0�I���W��)
//------------------------------------------
void SockManager::ThreadEvent(int nEventNo)
{
	BaseTcpIp::DELIVERY_ANS*	queAns = NULL;			// TCP/IP�N���X ���쌋��
	bool						bSendStatus = true;		// ���M�����X�e�[�^�X(True:����,False:���s)

														////// �V�O�i���̏���
	enum {
		EV_SEND_ANSER_TIMEOUT = 0,					// ���M��A�����`���^�C���A�E�g
		EV_RECV_HELCY_TIMEOUT,						// �w���V�[�`����M�^�C���A�E�g
		EV_SOCK_RECV_ANS,							// ��M�|�[�g ���쌋��
		EV_SOCK_SEND_ANS,							// ���M�|�[�g ���쌋��
		EV_QUE_DELE,								// ���M�˗��L���[�C�x���g
		EV_END
	};

	////// �V�O�i����������
	switch (nEventNo) {

	//-----------------------------------------------------------------------------------------------
	case EV_SEND_ANSER_TIMEOUT:									// ���M��A�����`���^�C���A�E�g
		LOG(em_ERR), "[%s] �����`���҂��^�C���A�E�g���� �C�x���gNo %d", my_sThreadName, m_nNowEventNo[ID_SEND] );
		syslog(322, "[�����`���^�C���A�E�g:�C�x���gNo %d]", m_nNowEventNo[ID_SEND]);
		UserClose();
		break;

	//-----------------------------------------------------------------------------------------------
	case EV_RECV_HELCY_TIMEOUT:									// �w���V�[�`����M�^�C���A�E�g
		LOG(em_ERR), "[%s] �w���V�[�`����M�^�C���A�E�g", my_sThreadName);
		syslog(322, "[�w���V�[�`��:��M�^�C���A�E�g]");
		UserClose();
		break;

		//-----------------------------------------------------------------------------------------------
	case EV_SOCK_RECV_ANS:										// ��M�|�[�g ���쌋��
		queAns = mcls_pSock[PORT_RECV]->gque_Anser.GetFromHead();
		// ���쌋�ʂ̏���
		ExecAnsQue(queAns->nMyID, queAns);
		// ���o�������쌋�ʂ��J��
		SAFE_DELETE(queAns->Data);
		SAFE_DELETE(queAns);
		break;

		//-----------------------------------------------------------------------------------------------
	case EV_SOCK_SEND_ANS:										// ���M�|�[�g ���쌋��
		queAns = mcls_pSock[PORT_SEND]->gque_Anser.GetFromHead();
		// ���쌋�ʂ̏���
		ExecAnsQue(queAns->nMyID, queAns);
		// ���o�������쌋�ʂ��J��
		SAFE_DELETE(queAns->Data);
		SAFE_DELETE(queAns);
		break;

		//-----------------------------------------------------------------------------------------------
	case EV_QUE_DELE:											// ���M�˗��L���[�C�x���g

		if (!SendLockStart()) {
			// �ؒf�������s
			UserClose();
		}

		//// ���M�������̎��s�̏ꍇ�́A�@���Ԃ�ύX
		if (SOCK_EXEC == m_eStateMain[ID_SEND]) {
			// �O�񂪑��M���s�̏ꍇ�́A�@���ԕύX
			if (!bSendStatus) {
				//KizuFunction::SetStatus("ZI_SEND_ERR_ID", true);		// ���M���s���̋@���Ԃ͖����B�̏Ⴭ�O�̂�
				bSendStatus = true;										// ���M���ʂ�ۑ�
			}
		}
		else {
			// �O�񂪑��M�����̏ꍇ�́A�@���ԕύX
			if (bSendStatus) {
				//KizuFunction::SetStatus("ZI_SEND_ERR_ID", false);		// ���M���s���̋@���Ԃ͖����B�̏Ⴭ�O�̂�
				bSendStatus = false;									// ���M���ʂ�ۑ�
			}
		}
		break;

		//-----------------------------------------------------------------------------------------------
	default:
		// ���肦�Ȃ��I�I
		LOG(em_ERR), "[%s] ThreadEvent EventNo NG=%d", my_sThreadName, nEventNo );
		return;
	}
}

//------------------------------------------
// �X���b�h��WaitFor�҂��̒��O����  
// int nDefWaitNum ���̐�
// int* nNowWaitNum �ύX��̐�
//------------------------------------------
void SockManager::ThreadWaitFor(int nDefWaitNum, int* nNowWaitNum)
{
	if (SOCK_EXEC == m_eStateMain[ID_RECV] || SOCK_EXEC == m_eStateMain[ID_SEND]) *nNowWaitNum = nDefWaitNum - 1;
	else                                                                          *nNowWaitNum = nDefWaitNum;
}

//------------------------------------------
// ���݂̃X�e�[�^�X����
// SOCK_STATE st �X�e�[�^�X
//------------------------------------------
CString SockManager::GetSockStatName(SOCK_STAT st) const
{
	CString wk;
	if (st == SOCK_NONE) { wk = "�ؒf��"; }
	else if (st == SOCK_IDLE) { wk = "�҂�"; }
	else if (st == SOCK_EXEC) { wk = "���s��"; }
	else { wk = ""; }
	return wk;
}

// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// TCP/IP ����� �C�x���g����

//------------------------------------------
// TCP/IP�N���X ���쌋�ʂ̏���
// int id �|�[�gID (�悭�g��������������Ă���)
// BaseTcpIp::DELIVERY_ANS* que ���쌋��
//------------------------------------------
void SockManager::ExecAnsQue(int id, BaseTcpIp::DELIVERY_ANS* que)
{
//	LOG(em_MSG), "[%s] ----------- ���쌋�ʒʒm <%s> [%s] Retc=%d-----------", 
//		my_sThreadName, mcls_pSock[id]->GetMyName(), mcls_pSock[id]->GetAnsStateName(que->state), (que->Data==NULL ? 0 : que->Data->nAns) );

	switch(que->state) {

//-----------------------------------------------------------------------------------------------
	case BaseTcpIp::ANS_CLOSE :										// �N���[�Y�ʒm
		if (KIZUSOCK_CLOSEKIND_SOCK == mcls_pSock[id]->GetCloseKind() ||
			KIZUSOCK_CLOSEKIND_PROG == mcls_pSock[id]->GetCloseKind()) {
			// ���肩��ؒf
			LOG(em_ERR), "[%s] %s ���肩��ؒf�v���I�I", my_sThreadName, mcls_pSock[id]->GetMyName());
		}
		LOG(em_MSG), "[%s] %s �ؒf�敪====[%d]", my_sThreadName, mcls_pSock[id]->GetMyName(), mcls_pSock[id]->GetCloseKind());
		// �ؒf�������s
		UserClose();
		break;

//-----------------------------------------------------------------------------------------------
	case BaseTcpIp::ANS_CONNECT :									// �R�l�N�g�����ʒm
		// �X�e�[�^�X�֘A������
		for(int ii=0; ii<PARAM_SOCK_MAX; ii++) {
			if (BaseTcpIp::STATE_NONE != mcls_pSock[ii]->GetSockState()) {
				m_eStateMain[ii] = SOCK_IDLE;
				m_eStateNowRecv[ii] = SOCK_NR_RES;
				m_nNowEventNo[ii] = 0;
			}
		}

		// �����q��������A�������F���Ƃ���
		if( BaseTcpIp::STATE_NONE != mcls_pSock[PORT_RECV]->GetSockState() &&
			BaseTcpIp::STATE_NONE != mcls_pSock[PORT_SEND]->GetSockState() ) {
		
			// �������`�F�b�N (�R�l�N�g�ʒm������O�ɁA��LIF���ŁA���|�[�g����ɂȂ�\��������΍�)
			if(m_bIsConnect) break;

			// �t���O�ύX
			m_bIsConnect = true;
			//for(int ii=0; ii<PARAM_SOCK_MAX; ii++) {
			//	m_eStateMain[ii] = SOCK_IDLE;
			//	m_eStateNowRecv[ii] = SOCK_NR_RES;
			//	m_nNowEventNo[ii] = 0;
			//}
			KizuFunction::SetStatus(MYID_KIKI, true, false);
			LOG(em_MSG), "[%s] �ڑ����� ===================================>>>>", my_sThreadName);
			syslog(320, "[�ڑ�����]" );

			// �w���V�[�`���Ď� �J�n
//��		
			SetHelcy();
	
			// ��M�|�[�g�� ��M�҂��ɂ��Ă����B
			if( ! RecvStart( PORT_RECV, sizeof(PRO_TYPE_RESP), m_cSockBuf[PORT_RECV][BUF_RECV] )) {
				mcls_pSock[PORT_RECV]->UserClose();
				break;
			}
			if( ! RecvStart( PORT_SEND, sizeof(PRO_TYPE_RESP), m_cSockBuf[PORT_SEND][BUF_RECV] )) {
				mcls_pSock[PORT_SEND]->UserClose();
				break;
			}
		}

		break;

//-----------------------------------------------------------------------------------------------
	case BaseTcpIp::ANS_RECVEND :									// �񓯊� ��M�����ʒm
		if( 0 != que->Data->nAns) {
			// �����M��������Ȃ��̂Őؒf���悤
			UserClose();
		} else {
			// ��M�������̏������s��
			if( ! RecvEnd(id, que)) {
				mcls_pSock[PORT_RECV]->UserClose();
				break;
			}
		}
		break;

//-----------------------------------------------------------------------------------------------
	case BaseTcpIp::ANS_SENDEND :									// ���M�����ʒm
		// �񓯊����M�� ����́A�s��Ȃ� (�����ȗ����̈�)
		break;
	
	default:			// ���肦�Ȃ�
		LOG(em_ERR), "[%s] ���[�h�ُ� [%d]", my_sThreadName, que->state);
		break;
	}
}

// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// TCP/IP �ʐM����
//------------------------------------------
// ���������� (��x�̂݋N��)
//------------------------------------------
void SockManager::SockInit()
{
	BaseTcpIp::DELIVERY_COMMON	deli;

	//// ��M�p�X���b�h ����
	// ��`
	deli.nMyID = PORT_RECV;
	deli.bSorC = true;
	deli.bRorS = true;
	strcpy_s (deli.cMyName,  "[��M�|�[�g]");
	sprintf(deli.cSession, "%s_RECV", TO_PROCON);
	deli.nBufferSize = PARAM_SOCK_BUFSIZE;
	deli.nRetryTimer = PARAM_SOCK_RETRY_TIMER;
	// ����
	mcls_pSock[PORT_RECV] = new BaseTcpIp(&deli);
	mcls_pSock[PORT_RECV]->SetLogMgr(mcls_pLog);
	mcls_pSock[PORT_RECV]->SetMainPostTid(GetTid());

	//// ���M�p�X���b�h ����
	// ��`
	deli.nMyID = PORT_SEND;
	deli.bSorC = true;//true;
	deli.bRorS = false;
	strcpy_s (deli.cMyName,  "[���M�|�[�g]");
	sprintf(deli.cSession, "%s_SEND", TO_PROCON);
	deli.nBufferSize = PARAM_SOCK_BUFSIZE;
	deli.nRetryTimer = PARAM_SOCK_RETRY_TIMER;
	// ����
	mcls_pSock[PORT_SEND] = new BaseTcpIp(&deli);
	mcls_pSock[PORT_SEND]->SetLogMgr(mcls_pLog);
	mcls_pSock[PORT_SEND]->SetMainPostTid(GetTid());

	//// ���[�J�[�X���b�h�J�n
	mcls_pSock[PORT_RECV]->Start();
	mcls_pSock[PORT_SEND]->Start();
}

//------------------------------------------
// �I������ (��x�̂݋N��)
//------------------------------------------
void SockManager::SockExit()
{
	// ���[�J�[�X���b�h��~
	mcls_pSock[PORT_RECV]->Stop();
	mcls_pSock[PORT_SEND]->Stop();

	// ���[�J�[�X���b�h�J��
	delete mcls_pSock[PORT_RECV];
	delete mcls_pSock[PORT_SEND];
}

//------------------------------------------
// �����ؒf
// bMode true:����ؒf�v��  false:�����ؒf   
//------------------------------------------
void SockManager::UserClose(bool bMode)
{
	if( m_bIsConnect ) {		// ���ڑ����H
		m_bIsConnect = false;		// �X�e�[�^�X�ύX�O�ɃN���[�Y���������삵�Ă��܂�����

		// �ǂ̃|�[�g����ł� �N���[�Y�ʒm��������A�S�|�[�g�����I��������
		mcls_pSock[PORT_RECV]->UserClose();
		mcls_pSock[PORT_SEND]->UserClose();
		// �@���Ԃɒʒm
		KizuFunction::SetStatus(MYID_KIKI, false, false);
		if (!bMode) {
			LOG(em_ERR), "[%s] �����ؒf ===================================>>>>", my_sThreadName);
		}
		else {
			LOG(em_MSG), "[%s] �ؒf     ===================================>>>>", my_sThreadName);
		}
		syslog(321, "[%s]", (!bMode?"�����ؒf":"�ؒf"));
	}
	// �X�e�[�^�X�֘A������
	m_bIsConnect = false;
	for(int ii=0; ii<PARAM_SOCK_MAX; ii++) {
		m_eStateMain[ii] = SOCK_NONE;
		m_eStateNowRecv[ii] = SOCK_NR_RES;
		m_nNowEventNo[ii] = 0;
	}
	CancelWaitableTimer(m_evAnserTimeOut);			// �����`���^�C���A�E�g�V�O�i�� �L�����Z��
	CancelWaitableTimer(m_evHelcyTimeOut);			// �w���V�[�`����M�^�C���A�E�g�V�O�i��
}

//------------------------------------------
// �񓯊� ��M�����J�n
// int id �|�[�gID
// int nSize �T�C�Y
// BYTE* pBuf �o�b�t�@�|�C���^
// int point ��M�f�[�^�J�n�ʒu
// �߂�l false:����ؒf���������{
//------------------------------------------
bool SockManager::RecvStart(int id, int nSize, BYTE* pBuf, int point)
{
	//// �񓯊� ��M
	// ��M���Z�b�g
	BaseTcpIp::DELIVERY_DATA* pDeleData = new BaseTcpIp::DELIVERY_DATA;
	pDeleData->bAnsQueing = true;	// �Œ�
	pDeleData->bRorS = true;		// ��M
	pDeleData->nStartPoint = point;
	pDeleData->nSize = nSize;
	pDeleData->Data = pBuf;
	pDeleData->nUserWk[0] = nSize + point;			// ���v��M�o�C�g���Ƃ��ăZ�b�g���Ă���
	
	// ��M�v��
	if( mcls_pSock[id]->SetRunningData(pDeleData)) {
		return true;	
	} else {
		LOG(em_ERR), "[%s] �񓯊� ��M�v�����s", my_sThreadName);
		SAFE_DELETE(pDeleData);
		return false;
	}
}

//------------------------------------------
// ���X�|���X�`���̑��M (����)
// int id �|�[�g
// BYTE bLowByte ����ُ�敪 (�T�u�w�b�_�[�̉���)
//------------------------------------------
bool SockManager::SendLock_Respons(int id, BYTE bLowByte)
{
	PRO_TYPE_RESP		bResp;				// ���X�|���X�`��

	// �`������
	bResp.bHead[0] = BaseTcpIp::gcTCPSEMI_HEAD_H_RESPONS;		// ���:���X�|���X
	bResp.bHead[1] = bLowByte;

	// �������M
	int iRetc = mcls_pSock[id]->LockSend( (BYTE*)&bResp, sizeof(bResp), PARAM_SOCK_TIMEOUT );
	LOG(em_MSG), "[%s] %s ���X�`�� ���M retc=%d", my_sThreadName, mcls_pSock[id]->GetMyName(), iRetc );
	return ( 0 == iRetc ? true : false);
}
//------------------------------------------
// �w���V�[�`�� ��M�^�C���A�E�g�ݒ�
//------------------------------------------
void SockManager::SetHelcy()
{
	LARGE_INTEGER ts;
	ts.QuadPart = (__int64)PARAM_HELCY_TIMEOUT * -10000;	// 1[ms]��100[ns] (�}�C�i�X�w��ŁA�w�莞�Ԍo�߂ŃV�O�i��) (��3������2��ڈȍ~�̋N���Ԋu)
	SetWaitableTimer(m_evHelcyTimeOut, &ts, 0, NULL, NULL, FALSE);
}


// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ����M�V�[�P���X

//------------------------------------------
// �Ɩ��`�� ���M (����)
// �߂�l false:�ؒf���Ă˗v��
//------------------------------------------
bool SockManager::SendLockStart()
{
	int nTc;					// ���M����g�����U�N�V�����R�[�h
	int nSize;					// ���M����`����

	//// �L���[���o��
	Delivery* deli = mque_pDeli->GetFromHead();
	// ��Ԋm�F
	if(! m_bIsConnect) {
		LOG(em_ERR), "[%s] �\�P�b�g�ؒf���ɑ��M�˗��L��B�˗��j��[%d]", my_sThreadName, deli->eStat);
		SAFE_DELETE(deli->pData);
		SAFE_DELETE(deli);
		return true;
	}

	// //// ���M�f�[�^ �擾(���Ƀo�C�g���]�ς�)
	PRO_TYPE_HEAD*	val = (PRO_TYPE_HEAD*)deli->pData;
	nSize = deli->nSize;
	nTc = deli->nTc;
	memcpy(m_cSockBuf[PORT_SEND][BUF_SEND], deli->pData, nSize);

	//// �������M
	LOG(em_MSG), "[%s] ���M�V�[�P���X�J�n ==================>>>", my_sThreadName);
	int iRetc = mcls_pSock[PORT_SEND]->LockSend( m_cSockBuf[PORT_SEND][BUF_SEND], nSize, PARAM_SOCK_TIMEOUT );
	if(0 == iRetc) {
		LOG(em_MSG), "[%s] %s �Ɩ��`�� ���M ID=%s TC=%d retc=%d", my_sThreadName, mcls_pSock[PORT_SEND]->GetMyName(), deli->cKey, nTc, iRetc );

		//// �������ѓ`���̂�
		if(PRO_TC_ZISEKI == nTc) {
			syslog(330, "[�R�C��No=%.12s, ID=%s]", ((PRO_TYPE_ZISEKI*)m_cSockBuf[PORT_SEND][BUF_SEND])->data.KeyInf.cCoilNo, deli->cKey);
			// ���M���ʔ��f
			SetSendDataAnser(deli, true );
		}

	} else {
		LOG(em_ERR), "[%s] %s �Ɩ��`�� ���M���s ID=%s TC=%d retc=%d", my_sThreadName, mcls_pSock[PORT_SEND]->GetMyName(), deli->cKey, nTc, iRetc );

		//// �������ѓ`���̂�
		if(PRO_TC_ZISEKI == nTc) {
			syslog(323, "[�R�C��No=%.12s, ID=%s]", ((PRO_TYPE_ZISEKI*)m_cSockBuf[PORT_SEND][BUF_SEND])->data.KeyInf.cCoilNo, deli->cKey);
			// ���M���ʔ��f
			SetSendDataAnser(deli, false );
		}
		SAFE_DELETE(deli->pData);
		SAFE_DELETE(deli);
		return false;
	}

	//// ���M������̐ݒ�
	m_eStateMain[ID_SEND] = SOCK_EXEC; 	
	m_nNowEventNo[ID_SEND] = nTc;
	// �����`���^�C���A�E�g�ݒ�
	LARGE_INTEGER ts;
	ts.QuadPart = (__int64)PARAM_SOCKANSER_TIMEOUT * -10000;	// 1[ms]��100[ns] (�}�C�i�X�w��ŁA�w�莞�Ԍo�߂ŃV�O�i��) (��3������2��ڈȍ~�̋N���Ԋu)
	SetWaitableTimer(m_evAnserTimeOut, &ts, 0, NULL, NULL, FALSE);

	//// �󂯓n���̈�J��
	SAFE_DELETE(deli->pData);
	SAFE_DELETE(deli);
	return true;
}

//------------------------------------------
// ��M ��������
// int id �|�[�gID
// BaseTcpIp::DELIVERY_ANS* que �C�x���g�{��
// �߂�l false:����ؒf���������{
//------------------------------------------
bool SockManager::RecvEnd(int id, BaseTcpIp::DELIVERY_ANS* que) 
{
	const BYTE* data = que->Data->Data;			// �f�[�^�����̃V���[�g�J�b�g


	////////////////////////////////////
	// 1�`�� ���擾
	////////////////////////////////////
	//---------------------------------------------------------
	if( SOCK_NR_RES == m_eStateNowRecv[id] ) {							// ���X��
		if( BaseTcpIp::gcTCPSEMI_HEAD_H_RESPONS == data[0] ) {					// ���X�|���X�`��
			//// ���X�|���X�`���Ȃ�ȍ~���������Ƃ���
			LOG(em_MSG), "[%s] %s ���X�`�� ��M", my_sThreadName, mcls_pSock[id]->GetMyName() );

//--->>

			if( id == ID_SEND ) {					// ���M�|�[�g
				if (SOCK_EXEC == m_eStateMain[ID_SEND]) {
					// �����`����M���� (���M�V�[�P���X����)
					m_eStateMain[ID_SEND] = SOCK_IDLE;
					m_nNowEventNo[ID_SEND] = 0;
					CancelWaitableTimer(m_evAnserTimeOut);			// �����`���^�C���A�E�g�V�O�i�� �L�����Z��
					LOG(em_MSG), "[%s] ���M�V�[�P���X���� <<<==================", my_sThreadName);
				}
			}
//<<<----

			// �܂��ŏ������M�J�n
			m_eStateNowRecv[id] = SOCK_NR_RES;
			if( ! RecvStart( id, sizeof(PRO_TYPE_RESP), m_cSockBuf[id][BUF_RECV] )) {
				return false;
			}
			return true;

		} else if ( BaseTcpIp::gcTCPSEMI_HEAD_H_GYOUMU == data[0] ) {			// �Ɩ��`�� ���� �����`��

			//// �w�b�_�[������M����
			m_eStateNowRecv[id] = SOCK_NR_HEAD;
			if( ! RecvStart( id, sizeof(PRO_TYPE_HEAD)-sizeof(PRO_TYPE_RESP), m_cSockBuf[id][BUF_RECV], que->Data->nUserWk[0] )) {
				return false;
			}
			return true;
	
		} else {
			LOG(em_ERR), "[%s] %s �T�u�w�b�_�ُ� [%02x, %02x]", my_sThreadName, mcls_pSock[id]->GetMyName(), data[0], data[1]);
			syslog(324, "[��M�`��:�T�u�w�b�_�ُ�] %s [%02x, %02x]]", mcls_pSock[id]->GetMyName(), data[0], data[1]);
			return false;
		}

	//---------------------------------------------------------
	} else if( SOCK_NR_HEAD == m_eStateNowRecv[id] ) {							// �w�b�_�[��
		short nSize = ((PRO_TYPE_HEAD*)data)->nLen;
		// �`�����擾 (�o�C�g���肵�Ȃ��ƃ_��������A�Ƃ肠�������̕��������o�C�g���])
#ifdef BYTE_HANTEN
		// �o�C�g���]�L��i1�A5CGL�A1CAPL�j
		nSize = KizuFunction::ByteHanten2( (BYTE*) &nSize);
#endif
		nSize = PRO_TYPE_LENGTH_ORIG(nSize);									// �{���̑S�`�������擾
		//// �f�[�^����������M (�`�����́A�w�b�_�[�����܂߂Ă���̂ŁA������l��)
		nSize -= sizeof(PRO_TYPE_HEAD);
		if(0 == nSize) goto EndFunc;		// �f�[�^�������� = �����`�� = ��M���������ƂȂ�
		if(0 > nSize) {						// �`�������ُ�
			LOG(em_ERR), "[%s] %s �f�[�^���ُ� [%d]", my_sThreadName, mcls_pSock[id]->GetMyName(), nSize);
			syslog(324, "[��M�`��:�f�[�^���ُ�] %s [%d]]", mcls_pSock[id]->GetMyName(), nSize);
			return false;
		}
		if(PORT_SEND == id) {				// ���M�|�[�g�̓��X�|���X�`���ȊO�͎�M����
			LOG(em_ERR), "[%s] %s �T�u�w�b�_�ُ� [%02x, %02x]", my_sThreadName, mcls_pSock[id]->GetMyName(), data[0], data[1]);
			syslog(324, "[��M�`��:�T�u�w�b�_�ُ�] %s [%02x, %02x]]", mcls_pSock[id]->GetMyName(), data[0], data[1]);
			return false;
		}

		//// �f�[�^������M����
		// ��M�V�[�P���X�Ǘ��͕s�v�̂��߃R�����g
		//////LOG(em_MSG), "[%s] ��M�V�[�P���X�J�n ==================>>> [Size:%04d]", my_sThreadName, nSize);
		//////m_eStateMain[ID_RECV]    = SOCK_EXEC;
		m_eStateNowRecv[ID_RECV] = SOCK_NR_DATA;
		if( ! RecvStart( id, nSize, m_cSockBuf[id][BUF_RECV], que->Data->nUserWk[0] )) {
			return false;
		}
		return true;
	}

	////////////////////////////////////
	// 1�`�� ��M����
	////////////////////////////////////
EndFunc:
	// �K�v�ȃf�[�^���o��
	PRO_TYPE_HEAD* head = (PRO_TYPE_HEAD*)data;			// �w�b�_�[��
	short nSize  = head->nLen;
	int   nTc    = head->nTC;

#ifdef BYTE_HANTEN
	// �o�C�g���]�L��i1�A5CGL�A1CAPL�j
	nSize  = KizuFunction::ByteHanten2( (BYTE*) &nSize);
	nTc    = KizuFunction::ByteHanten4( (BYTE*) &nTc );
#endif

	nSize = PRO_TYPE_LENGTH_ORIG(nSize);									// �{���̑S�`�������擾

	//// �������`�F�b�N
	// �T�u�w�b�_�[
	if (BaseTcpIp::gcTCPSEMI_HEAD_H_GYOUMU != head->bHead[0] ||
		BaseTcpIp::gcTCPSEMI_HEAD_L_OK != head->bHead[1]) {
		LOG(em_ERR), "[%s] �w�b�_�[��(�T�u�w�b�_�[�ُ�)=%02X%02X", my_sThreadName, head->bHead[0], head->bHead[1]);
		syslog(324, "[��M�`��:�w�b�_�[��(�T�u�w�b�_�[�ُ�)=%02X%02X]", nTc, head->bHead[0], head->bHead[1]);
		return false;
	}


	LOG(em_MSG), "[%s] %s ��M���� [�Ɩ��`��] EvtNo=%d, Len=%dbyte", my_sThreadName, mcls_pSock[id]->GetMyName(), nTc, nSize );

	// �Ƃ肠������M�����̂ŁA���X�|���X�`����Ԃ��B(�������M) (CGL�̂�)
	SendLock_Respons(id, BaseTcpIp::gcTCPSEMI_HEAD_L_OK );
	// ��M�V�[�P���X�Ǘ��͕s�v�̂��߃R�����g
	//////if (id == ID_RECV) {					// ��M�|�[�g
	//////	if (SOCK_EXEC == m_eStateMain[ID_RECV]) {
	//////		// �����`����M���� (��M�V�[�P���X����)
	//////		m_eStateMain[ID_RECV] = SOCK_IDLE;
	//////		m_nNowEventNo[ID_RECV] = 0;
	//////		LOG(em_MSG), "[%s] ��M�V�[�P���X���� <<<==================", my_sThreadName);

	//////	}
	//////}

#ifdef BYTE_HANTEN
	// �o�C�g���]�L�莞�́A���]�O�̃_���v���o��
	// ���f�[�^�_���v
	CString cDumpName;
	cDumpName.Format("%s ��M(��)�_���v %s(%s, EvtNo=%d).log", TO_PROCON,
					mcls_pSock[id]->GetMyName(), "�Ɩ��`��", nTc );

	KizuFunction::DumpData(cDumpName, nSize, data, GetDumpMode());
#endif



	//// �������`�F�b�N 
	// �|�[�g (���̃`�F�b�N�ȍ~�́A��M�|�[�g�����̘b�ƂȂ�)
	if(PORT_SEND == id)  {
		// ���M�|�[�g�Ń��X�|���X�`���ȊO����M���鎖�͖���
		LOG(em_ERR), "[%s] ���M�|�[�g�Ń��X�|���X�`���ȊO����M���鎖�͂��肦�Ȃ�", my_sThreadName);
		syslog(324, "[��M�`��:���M�|�[�g�Ŏ�M]");
		return false;
	}

	////////////////////////////////////
	// �Ɩ��`��
	////////////////////////////////////

	//// �Ɩ��`�� ��ނɂ�菈������
	if (!SetRecvData(nTc, nSize, m_cSockBuf[id][BUF_RECV])) {
		return false;
	}


	//// �܂��ŏ������M�J�n
	m_eStateNowRecv[id] = SOCK_NR_RES;
	if( ! RecvStart( id, sizeof(PRO_TYPE_RESP), m_cSockBuf[id][BUF_RECV] )) {
		return false;
	}
	return true;
}