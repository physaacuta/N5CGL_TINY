#include "StdAfx.h"
#include "SockManager.h"

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
SockManager::SockManager(void) :
ThreadManager("SockManager"),
m_bIsConnect(false),
m_bIsFfConnectErr(false),
m_eStateMain(SOCK_NONE)
{
	//// �V�O�i������
	m_evAnserTimeOut   = CreateWaitableTimer(NULL, FALSE, NULL);				// �����`���҂��^�C���A�E�g�V�O�i��
	m_evHelcySendTimer = CreateWaitableTimer(NULL, FALSE, NULL);				// �w���V�[�`�����M�^�C�}�[�V�O�i��
	m_evJukosyoTimer   = CreateWaitableTimer(NULL, FALSE, NULL);				// �d�̏�Ď��^�C�}�[�V�O�i��
}

//------------------------------------------
// �f�X�g���N�^
//------------------------------------------
SockManager::~SockManager(void)
{
	// �V�O�i���J��
	CloseHandle(m_evAnserTimeOut);			// �����`���҂��^�C���A�E�g�V�O�i��
	CloseHandle(m_evHelcySendTimer);		// �w���V�[�`�����M�^�C�}�[�V�O�i��
	CloseHandle(m_evJukosyoTimer);			// �d�̏�Ď��^�C�}�[�V�O�i��
}

//------------------------------------------
// �X���b�h�J�n�O���� (�X���b�h���s����(�V�O�i���Z�b�g�O)�ɃR�[���o�b�N)
//------------------------------------------
int SockManager::ThreadFirst()
{
	//// TCP/IP ����N���X������
	SockInit(); 

	// WaitForMultipleObjects�C�x���g
	HANDLE	hArray[] = {m_evAnserTimeOut,								// �����`���҂��^�C���A�E�g�V�O�i��
						mcls_pSock[PORT_RECV]->gque_Anser.g_evSem,		// ��M�|�[�g[0] �p����N���X
						mcls_pSock[PORT_SEND]->gque_Anser.g_evSem,		// ���M�|�[�g[1] �p����N���X
						m_evHelcySendTimer,								// �w���V�[�`�����M�^�C�}�[�V�O�i��
						m_evJukosyoTimer,								// �d�̏�Ď��^�C�}�[�V�O�i��
						gque_Deli.g_evSem								// ���M�˗��p�L���[ 
						};

	// �C�x���g�Z�b�g
	SetEventNum(sizeof(hArray)/sizeof(HANDLE), hArray);

	return 0;
}

//------------------------------------------
// �X���b�h�I���O���� (�I���V�O�i����ɃR�[���o�b�N)
//------------------------------------------
int SockManager::ThreadLast()
{
	//// TCP/IP ����N���X�J��
	SockExit();

	return 0;
}

//------------------------------------------
// �X���b�h�C�x���g����
// int nEventNo �C�x���gNo (0�I���W��)
//------------------------------------------
void SockManager::ThreadEvent(int nEventNo)
{
	BaseTcpIp::DELIVERY_ANS*	queAns  = NULL;	// TCP/IP�N���X ���쌋��


	enum {	EV_SEND_ANSER_TIMEOUT=0,			// �����`���҂��^�C���A�E�g�V�O�i��
			EV_SOCK_RECV_ANS,					// ��M�|�[�g ���쌋��
			EV_SOCK_SEND_ANS,					// ���M�|�[�g ���쌋��
			EV_SEND_HELCY_TIMER,				// �w���V�[�`�����M�^�C�}�[�V�O�i�� (��� ���M�L���[������)
			EV_JUKOSYO_TIMER,					// �d�̏�Ď��^�C�}�[�V�O�i��
			EV_QUE_DELI,						// ���M�˗��L���[�C�x���g
			};

	////// �V�O�i����������
	switch (nEventNo) {

//-----------------------------------------------------------------------------------------------
		case EV_SEND_ANSER_TIMEOUT:									// ���M��A�����`���^�C���A�E�g
			// �����`���҂��^�C���A�E�g
			LOG(em_ERR), "[%s] �����`���҂��^�C���A�E�g���� (%s)", my_sThreadName, GetSendStatName());
			syslog( PARAM_SYSLOG_BASENO + 11, "[�����`���^�C���A�E�g:%s]", GetSendStatName());
			FFAnserTimeOut();
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
		case EV_SEND_HELCY_TIMER:									// �w���V�[�`�����M�^�C�}�[�V�O�i��
			{
				LOG(em_INF), "[%s] �w���V�[���M�˗�", my_sThreadName);
				// �w���V�[�`�����ŗD��ő��M����
				Delivery* deli = new Delivery;
				memset(deli, 0x00, sizeof(Delivery));
				deli->eStat = QUE_SEND_HELCY;
				gque_Deli.AddToHead(deli);			// �擪�ɃL���[�C���O����̂Œ���
			}
			break;

//-----------------------------------------------------------------------------------------------
		case EV_JUKOSYO_TIMER:										// �d�̏�Ď��^�C�}�[�V�O�i��
			LOG(em_ERR), "[%s] FF�T�[�o�[�R�l�N�g���s [%d��]�o�߁I�I", my_sThreadName, PARAM_JUKOSYO_TIMER);
			KizuFunction::SetStatus("FFSERV_ID", false);
			KizuFunction::SetStatus("FF_CONNECT_ERR_ID", true);

			break;

//-----------------------------------------------------------------------------------------------
		case EV_QUE_DELI:											// ���M�˗��L���[�C�x���g
			{
				Delivery* deli;
				deli = gque_Deli.GetFromHead();
				if( ! SendLockStart( deli ) ) {
					// �ؒf�������s
					UserClose();
				}
				SAFE_DELETE(deli);
			}

			break;

//-----------------------------------------------------------------------------------------------
		default :
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
	if( SOCK_EXEC == m_eStateMain ) *nNowWaitNum = nDefWaitNum-1;
	else							*nNowWaitNum = nDefWaitNum;
}

//------------------------------------------
// �L���[���폜����
//------------------------------------------
void SockManager::QueDelete()
{
	// �L���[�ɗ��܂��Ă���C���X�^���X��S���J��
	Delivery* deli = NULL;
	while(true) {
		deli = gque_Deli.GetFromHead(0);
		if(deli == NULL) break;
		delete deli;
		deli = NULL;
	}
}

//------------------------------------------
// ���݂̃X�e�[�^�X����
// SOCK_STATE st �X�e�[�^�X
//------------------------------------------
CString SockManager::GetSockStatName(SOCK_STAT st) const
{
	CString wk;
	if		(st == SOCK_NONE)	{ wk = "�ؒf��"; }
	else if(st == SOCK_IDLE)	{ wk = "�҂�"; }
	else if(st == SOCK_EXEC)	{ wk = "���s��";}
	else						{ wk = "";}
	return wk;
}

//------------------------------------------
// ���݂̑��M�X�e�[�^�X����
// SOCK_STATE st �X�e�[�^�X
//------------------------------------------
CString SockManager::GetSendStatName(QUE_SEND_STAT st) const
{
	CString wk;
	if		(st == QUE_SEND_ZISEKI)	{ wk = "�r���ё��M"; }
	else if(st == QUE_SEND_MAE)	{ wk = "�O�H�����їv��"; }
	else if(st == QUE_SEND_HELCY)	{ wk = "�w���V�[";}
	else							{ wk = "";}
	return wk;
}

// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// TPC/IP ����� �C�x���g����

//------------------------------------------
// TCP/IP�N���X ���쌋�ʂ̏���
// int id �|�[�gID (�悭�g��������������Ă���)
// BaseTcpIp::DELIVERY_ANS* que ���쌋��
//------------------------------------------
void SockManager::ExecAnsQue(int id, BaseTcpIp::DELIVERY_ANS* que)
{
	switch(que->state) {

//-----------------------------------------------------------------------------------------------
	case BaseTcpIp::ANS_CLOSE :										// �N���[�Y�ʒm
		if( KIZUSOCK_CLOSEKIND_SOCK == mcls_pSock[id]->GetCloseKind() ) {
			// ���肩��ؒf
			LOG(em_ERR), "[%s] %s ���肩��ؒf�v���I�I", my_sThreadName, mcls_pSock[id]->GetMyName());
		}
		// �ؒf�������s
		UserClose();
		break;

//-----------------------------------------------------------------------------------------------
	case BaseTcpIp::ANS_ERR_CONNECT :								// �R�l�N�g���s�ʒm
		if( ! m_bIsFfConnectErr) {
			LOG(em_ERR), "[%s] FF�T�[�o�[�R�l�N�g���s�I�I", my_sThreadName);
			m_bIsFfConnectErr = true;
			KizuFunction::SetStatus("FFSERV_ID", true, false);
			KizuFunction::SetStatus("FF_CONNECT_ERR_ID", false, false);
			// �d�̏�^�C�}�[�Z�b�g
			SetJukosyoTimer();
		}
		break;

//-----------------------------------------------------------------------------------------------
	case BaseTcpIp::ANS_CONNECT :									// �R�l�N�g�����ʒm
		// �����q��������A�������F���Ƃ���
		if( BaseTcpIp::STATE_NONE != mcls_pSock[PORT_RECV]->GetSockState() &&
			BaseTcpIp::STATE_NONE != mcls_pSock[PORT_SEND]->GetSockState() ) {
		
			// �������`�F�b�N (�R�l�N�g�ʒm������O�ɁA��LIF���ŁA���|�[�g����ɂȂ�\��������΍�)
			if(m_bIsConnect) break;

			// �t���O�ύX
			m_bIsConnect = true;
			m_bIsFfConnectErr = false;
			m_eStateMain = SOCK_IDLE;
			memset(&m_ExecItem, 0x00, sizeof(m_ExecItem));

			KizuFunction::SetStatus("FFSERV_ID", true);
			KizuFunction::SetStatus("FF_CONNECT_ERR_ID", true);
			LOG(em_MSG), "[%s] ������� ===================================", my_sThreadName);
			syslog( PARAM_SYSLOG_BASENO + 8, "" );
			
			// �d�̏�^�C�}�[�L�����Z��
			CancelWaitableTimer(m_evJukosyoTimer);

			// ��M�|�[�g�� ��M�҂��ɂ��Ă����B
			if( ! RecvStart( PORT_RECV, sizeof(FF_ANS), m_cSockBuf[PORT_RECV][BUF_RECV] )) {
				UserClose();
				break;
			}

			// �����w���V�[�`���𑗐M�˗�
			SetHelcySendTimer(0);

			//// �����M���т̑��M�˗�
			AddQue_SendZiseki();
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
				UserClose();
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

//------------------------------------------
// �w���V�[�`�� �ŗD�摗�M�v��
// int nTimer �w���V�[���M�҂����� [ms]
//------------------------------------------
void SockManager::SetHelcySendTimer(int nTimer)
{
	LARGE_INTEGER ts;
	ts.QuadPart = (__int64)nTimer * -10000;	// 1[ms]��100[ns] (�}�C�i�X�w��ŁA�w�莞�Ԍo�߂ŃV�O�i��) (��3������2��ڈȍ~�̋N���Ԋu)
	SetWaitableTimer(m_evHelcySendTimer, &ts, 0, NULL, NULL, FALSE);
}

//------------------------------------------
// �d�̏�^�C�}�[�Z�b�g����
//------------------------------------------
void SockManager::SetJukosyoTimer()
{
	LARGE_INTEGER ts;
	ts.QuadPart = (__int64)PARAM_JUKOSYO_TIMER * (60 * 1000) * -10000;	// 1[��]��100[ns] (�}�C�i�X�w��ŁA�w�莞�Ԍo�߂ŃV�O�i��) (��3������2��ڈȍ~�̋N���Ԋu)
	SetWaitableTimer(m_evJukosyoTimer, &ts, 0, NULL, NULL, FALSE);
}

//------------------------------------------
// �����`���^�C���A�E�g���̌ŗL����
//------------------------------------------
void SockManager::FFAnserTimeOut()
{
	if(QUE_SEND_ZISEKI == m_ExecItem.emStat) {
		// ���щ����`���̑��M�^�C���A�E�g�̏���
		mcls_pData->SetZisekiSendAnser( m_ExecItem.cKeyId, SockDataExec::FF_SENDKIND_ERR );	
	}
}

//------------------------------------------
// �����M���т̑��M�˗�
//------------------------------------------
void SockManager::AddQue_SendZiseki()
{
	Delivery* deli = new Delivery;
	memset(deli, 0x00, sizeof(Delivery));
	deli->eStat = QUE_SEND_ZISEKI;
	gque_Deli.AddToTail(deli);
}

// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// TPC/IP �ʐM����
//------------------------------------------
// ���������� (��x�̂݋N��)
//------------------------------------------ 
void SockManager::SockInit()
{
	BaseTcpIp::DELIVERY_COMMON	deli;

	//// ��M�p�X���b�h ����
	// ��`
	deli.nMyID = PORT_RECV;
	deli.bSorC = false;
	deli.bRorS = true;
	strcpy_s (deli.cMyName,  "[��M�|�[�g]");
	sprintf(deli.cSession, "%s_RECV", TO_FFSERV);
	deli.nBufferSize = PARAM_SOCK_BUFSIZE;
	deli.nRetryTimer = PARAM_SOCK_TIMEOUT;
	// ����
	mcls_pSock[PORT_RECV] = new BaseTcpIp(&deli);
	mcls_pSock[PORT_RECV]->SetLogMgr(mcls_pLog);
	mcls_pSock[PORT_RECV]->SetMainPostTid(GetTid());
	mcls_pSock[PORT_RECV]->SetErrConnectQueing(true);	// �R�l�N�g���s����AP�ɒʒm�L�� 

	//// ���M�p�X���b�h ����
	// ��`
	deli.nMyID = PORT_SEND;
	deli.bSorC = false;
	deli.bRorS = false;
	strcpy_s (deli.cMyName,  "[���M�|�[�g]");
	sprintf(deli.cSession, "%s_SEND", TO_FFSERV);
	deli.nBufferSize = PARAM_SOCK_BUFSIZE;
	deli.nRetryTimer = PARAM_SOCK_TIMEOUT;
	// ����
	mcls_pSock[PORT_SEND] = new BaseTcpIp(&deli);
	mcls_pSock[PORT_SEND]->SetLogMgr(mcls_pLog);
	mcls_pSock[PORT_SEND]->SetMainPostTid(GetTid());
	mcls_pSock[PORT_SEND]->SetErrConnectQueing(true);	// �R�l�N�g���s����AP�ɒʒm�L��

	//// ���[�J�[�X���b�h�J�n
	mcls_pSock[PORT_RECV]->Start();
	mcls_pSock[PORT_SEND]->Start();

}

//------------------------------------------
// �I������ (��x�̂݋N��)
//------------------------------------------
void SockManager::SockExit()
{
	//�I�����ɑ��M�˗��L���[�����܂��Ă�����폜����
	QueDelete();

	// ���[�J�[�X���b�h��~
	mcls_pSock[PORT_RECV]->Stop();
	mcls_pSock[PORT_SEND]->Stop();

	// ���[�J�[�X���b�h�J��
	delete mcls_pSock[PORT_RECV];
	delete mcls_pSock[PORT_SEND];

}

//------------------------------------------
// �����ؒf
//------------------------------------------
void SockManager::UserClose()
{
	CancelWaitableTimer(m_evAnserTimeOut);			// �����`���҂��^�C���A�E�g�V�O�i��
	CancelWaitableTimer(m_evHelcySendTimer);		// �w���V�[�`�����M�^�C�}�[�V�O�i��
	CancelWaitableTimer(m_evJukosyoTimer);			// �d�̏�Ď��^�C�}�[�V�O�i��


	if( m_bIsConnect ) {		// ���ڑ����H
		// �ǂ̃|�[�g����ł� �N���[�Y�ʒm��������A�S�|�[�g�����I��������
		mcls_pSock[PORT_RECV]->UserClose();
		mcls_pSock[PORT_SEND]->UserClose();
		// �@���Ԃɒʒm
		KizuFunction::SetStatus("FF_CONNECT_ERR_ID", false, false);
		LOG(em_ERR), "[%s] ����ُ� ===================================", my_sThreadName);
		syslog( PARAM_SYSLOG_BASENO + 9, "" );
	}

	// �X�e�[�^�X�֘A������
	m_bIsConnect = false;
	m_bIsFfConnectErr = false;
	m_eStateMain = SOCK_NONE;
	memset(&m_ExecItem, 0x00, sizeof(m_ExecItem));
}

//------------------------------------------
// �񓯊� ��M�����J�n
// int id �|�[�gID
// int size �T�C�Y
// BYTE* pBuf �o�b�t�@�|�C���^
// int point ��M�f�[�^�J�n�ʒu
// �߂�l false:����ؒf���������{
//------------------------------------------
bool SockManager::RecvStart(int id, int size, BYTE* pBuf, int point)
{
	//// �񓯊� ��M
	// ��M���Z�b�g
	BaseTcpIp::DELIVERY_DATA* pDeleData = new BaseTcpIp::DELIVERY_DATA;
	pDeleData->bAnsQueing = true;				// �Œ�
	pDeleData->bRorS = true;					// ��M
	pDeleData->nStartPoint = point;
	pDeleData->nSize = size;
	pDeleData->Data = pBuf;
	pDeleData->nUserWk[0] = size+point;			// ���v��M�o�C�g���Ƃ��ăZ�b�g���Ă���
	
	// ��M�v��
	if( mcls_pSock[id]->SetRunningData(pDeleData)) {
		return true;	
	} else {
		LOG(em_ERR), "[%s] �񓯊� ��M�v�����s", my_sThreadName);
		SAFE_DELETE(pDeleData);
		return false;
	}
}


// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ����M�V�[�P���X

//------------------------------------------
// �Ɩ��`�� ���M �������M
// �߂�l false:�ؒf���Ă˗v��
//------------------------------------------
bool SockManager::SendLockStart(Delivery* deli)
{
	// ��Ԋm�F
	if(! m_bIsConnect) {
		LOG(em_ERR), "[%s] �\�P�b�g�ؒf���ɖ��������̑��M�˗��L��B�˗��j��[%d]", my_sThreadName, deli->eStat);
		return true;
	}

	//// ���M�f�[�^ �擾
	if( ! mcls_pData->GetSendData(deli, &m_ExecItem ) ) {
		return true;
	}

	//// �������M
	LOG(em_MSG), "[%s] �V�[�P���X�J�n (%s)==================>>>", my_sThreadName, GetSendStatName());
	if( ! SendExec() ) {
		return false;
	}

	return true;
}

//------------------------------------------
// �Ɩ��`�����M ���ʏ���
//------------------------------------------
bool SockManager::SendExec()
{
	//// �������M
	int nRec = m_ExecItem.nSendNowNo;
	memcpy(	m_cSockBuf[PORT_SEND][BUF_SEND], m_ExecItem.cSendBuf[nRec], m_ExecItem.nSendSize[nRec]);
	//// ���f�[�^�_���v
	CString dumpName;
	dumpName.Format("%s ���M(��)�_���v (%s, No=%d).log", TO_FFSERV, 
					GetSendStatName(), m_ExecItem.nSendNowNo );
	KizuFunction::DumpData( dumpName, m_ExecItem.nSendSize[nRec], m_cSockBuf[PORT_SEND][BUF_SEND], mcls_pData->GetDumpMode());
	int iRetc = mcls_pSock[PORT_SEND]->LockSend( m_cSockBuf[PORT_SEND][BUF_SEND], m_ExecItem.nSendSize[nRec], PARAM_SOCK_TIMEOUT );

	if(0 != iRetc) {
		LOG(em_ERR), "[%s] %s ���M���s [�`���敪=%d ����No=%s ID=%s]", my_sThreadName, GetSendStatName(m_ExecItem.emStat), m_ExecItem.nSendNowNo, m_ExecItem.cMeiNo, m_ExecItem.cKeyId);
		LOG(em_ERR), "[%s] �V�[�P���X�ُ튮�� <<<==================", my_sThreadName);


		// �ʏ���
		if(QUE_SEND_ZISEKI == m_ExecItem.emStat) {
			syslog( PARAM_SYSLOG_BASENO + 10, "[���ѓ`��:�`���敪(0:���ѕ\ 1:���ї� 2:��ԕ\ 3:��ԗ�)=%d retc=%d ����No=%s ID=%s]", m_ExecItem.nSendNowNo, iRetc, m_ExecItem.cMeiNo, m_ExecItem.cKeyId);
			// ���M���ʂ𔽉f
			mcls_pData->SetZisekiSendAnser( m_ExecItem.cKeyId, SockDataExec::FF_SENDKIND_ERR );
		} else if(QUE_SEND_MAE == m_ExecItem.emStat) {
			syslog( PARAM_SYSLOG_BASENO + 10, "[�O�H���`��:retc=%d �R�C��No=%s ����No=%s]", iRetc, m_ExecItem.cCoilNo, m_ExecItem.cMeiNo);
		} else if(QUE_SEND_HELCY == m_ExecItem.emStat) {
			syslog( PARAM_SYSLOG_BASENO + 10, "[�w���V�[�`��:retc=%d]", iRetc);
		}
		return false;

	} else {

		// �ʏ���
		if(QUE_SEND_ZISEKI == m_ExecItem.emStat) {
			LOG(em_MSG), "[%s] %s ���M���� [�`���敪=%d ����No=%s ID=%s]", my_sThreadName, GetSendStatName(m_ExecItem.emStat), m_ExecItem.nSendNowNo, m_ExecItem.cMeiNo, m_ExecItem.cKeyId);
			syslog( PARAM_SYSLOG_BASENO + 0, "[�`���敪(0:���ѕ\ 1:���ї� 2:��ԕ\ 3:��ԗ�)=%d ����No=%s ID=%s]", m_ExecItem.nSendNowNo, m_ExecItem.cMeiNo, m_ExecItem.cKeyId);
			// ���M���ʂ𔽉f
			mcls_pData->SetZisekiSendAnser( m_ExecItem.cKeyId, SockDataExec::FF_SENDKIND_ERR );
		} else if(QUE_SEND_MAE == m_ExecItem.emStat) {
			LOG(em_MSG), "[%s] %s ���M���� [�R�C��No=%s ����No=%s]", my_sThreadName, GetSendStatName(m_ExecItem.emStat), m_ExecItem.cCoilNo, m_ExecItem.cMeiNo);
			syslog( PARAM_SYSLOG_BASENO + 3, "[�R�C��No=%s ����No=%s]", m_ExecItem.cCoilNo, m_ExecItem.cMeiNo);
		} else if(QUE_SEND_HELCY == m_ExecItem.emStat) {
			LOG(em_MSG), "[%s] %s ���M����", my_sThreadName, GetSendStatName(m_ExecItem.emStat) );
		}


		//// �X�e�[�^�X�ύX
		m_eStateMain = SOCK_EXEC;
		m_ExecItem.nSendNowNo += 1;

		//// �����`���^�C���A�E�g�ݒ�
		LARGE_INTEGER ts;
		ts.QuadPart = (__int64)PARAM_SOCKANSER_TIMEOUT * -10000;	// 1[ms]��100[ns] (�}�C�i�X�w��ŁA�w�莞�Ԍo�߂ŃV�O�i��) (��3������2��ڈȍ~�̋N���Ԋu)
		SetWaitableTimer(m_evAnserTimeOut, &ts, 0, NULL, NULL, FALSE);

		return true;
	}
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
	int			nSize = que->Data->nSize;
	char		cTc[2];							// �g�����U�N�V�����R�[�h�m�F�p
	memcpy(cTc,  &data[0], sizeof(cTc));

	//// ���f�[�^�_���v
	CString dumpName;
	dumpName.Format("%s ��M(��)�_���v (%s, No=%d, TC=%.2s).log", TO_FFSERV, 
					GetSendStatName(), m_ExecItem.nRecvNowNo, cTc );
	KizuFunction::DumpData( dumpName, nSize, data, mcls_pData->GetDumpMode());

	LOG(em_MSG), "[%s] �`����M (%s, No=%d, TC=%.2s)", my_sThreadName, GetSendStatName(), m_ExecItem.nRecvNowNo, cTc );

	////////////////////////////////////
	// 1�`�� �������m�F (�����Ƃ����Ƃ���)
	////////////////////////////////////
	//// �|�[�g�������`�F�b�N
	if(PORT_SEND == id)  {
		// ���M�|�[�g�ł͎�M���鎖�͂Ȃ��̂ŃG���[�Ƃ���
		LOG(em_ERR), "[%s] ���M�|�[�g�ŉ����`���͎�M���܂���", my_sThreadName);
		syslog( PARAM_SYSLOG_BASENO + 12 , "[���M�|�[�g�ŉ����`������M���܂����B]");
		return false;
	}

	//// �V�[�P���X�m�F
	if( 0 == memcmp( cTc, FF_TC_CODE_ANS_O, sizeof(cTc)) || 0 == memcmp( cTc, FF_TC_CODE_ANS_U, sizeof(cTc)) ||
		0 == memcmp( cTc, FF_TC_CODE_STSANS_O, sizeof(cTc)) || 0 == memcmp( cTc, FF_TC_CODE_STSANS_U, sizeof(cTc))) {
		if( QUE_SEND_ZISEKI != m_ExecItem.emStat ) {
			LOG(em_ERR), "[%s] �V�[�P���X�s��v�ȉ����`������M�B(%s, No=%d, TC=%.2s)", my_sThreadName, GetSendStatName(), m_ExecItem.nRecvNowNo, cTc );
			syslog( PARAM_SYSLOG_BASENO + 13, "[�V�[�P���X�s��v�ȉ����`������M�B(%s, No=%d, TC=%.2s)]", GetSendStatName(), m_ExecItem.nRecvNowNo, cTc );
			return false;
		}
		if( (0 == m_ExecItem.nRecvNowNo && 0 != memcmp( cTc, FF_TC_CODE_ANS_O, sizeof(cTc))) ||
			(1 == m_ExecItem.nRecvNowNo && 0 != memcmp( cTc, FF_TC_CODE_ANS_U, sizeof(cTc))) ||
			(2 == m_ExecItem.nRecvNowNo && 0 != memcmp( cTc, FF_TC_CODE_STSANS_O, sizeof(cTc))) ||
			(3 == m_ExecItem.nRecvNowNo && 0 != memcmp( cTc, FF_TC_CODE_STSANS_U, sizeof(cTc))) ) {
			if (2 == m_ExecItem.nRecvNowNo || 3 == m_ExecItem.nRecvNowNo){
				LOG(em_ERR), "[%s] �\���s��v�ȉ����`������M�B(%s(�������), No=%d, TC=%.2s)", my_sThreadName, GetSendStatName(), m_ExecItem.nRecvNowNo, cTc );
				syslog(PARAM_SYSLOG_BASENO + 7, "[�\���s��v�ȉ����`������M�B(%s(�������), No=%d, TC=%.2s)]", GetSendStatName(), m_ExecItem.nRecvNowNo, cTc);
			} else {
				LOG(em_ERR), "[%s] �\���s��v�ȉ����`������M�B(%s(����), No=%d, TC=%.2s)", my_sThreadName, GetSendStatName(), m_ExecItem.nRecvNowNo, cTc );
				syslog( PARAM_SYSLOG_BASENO + 7 , "[�\���s��v�ȉ����`������M�B(%s(����), No=%d, TC=%.2s)]", GetSendStatName(), m_ExecItem.nRecvNowNo, cTc );
			}
			return false;
		}

	} else if( 0 == memcmp( cTc, FF_TC_CODE_MAEJI_O, sizeof(cTc)) ||
		0 == memcmp( cTc, FF_TC_CODE_MAEJI_U, sizeof(cTc)) ) {
//		if( QUE_SEND_MAE != m_ExecItem.emStat ) {
		if( QUE_SEND_MAE != m_ExecItem.emStat || 1 < m_ExecItem.nRecvNowNo ) { // ں���No�������Ȃ��Ɨ��̖���No�s��v���ɍēx�������󂯂�Ɨ�O
			LOG(em_ERR), "[%s] �V�[�P���X�s��v�ȉ����`������M�B(%s, No=%d, TC=%.2s)", my_sThreadName, GetSendStatName(), m_ExecItem.nRecvNowNo, cTc );
			syslog( PARAM_SYSLOG_BASENO + 13 , "[�V�[�P���X�s��v�ȉ����`������M�B(%s, No=%d, TC=%.2s)]", GetSendStatName(), m_ExecItem.nRecvNowNo, cTc );
			return false;
		}
		if( (0 == m_ExecItem.nRecvNowNo && 0 != memcmp( cTc, FF_TC_CODE_MAEJI_O, sizeof(cTc))) ||
			(1 == m_ExecItem.nRecvNowNo && 0 != memcmp( cTc, FF_TC_CODE_MAEJI_U, sizeof(cTc))) ) {
			LOG(em_ERR), "[%s] �\���s��v�ȉ����`������M�B(%s, No=%d, TC=%.2s)", my_sThreadName, GetSendStatName(), m_ExecItem.nRecvNowNo, cTc );
			syslog( PARAM_SYSLOG_BASENO + 7 , "[�\���s��v�ȉ����`������M�B(%s, No=%d, TC=%.2s)]", GetSendStatName(), m_ExecItem.nRecvNowNo, cTc );
			return false;
		}

	} else if( 0 == memcmp( cTc, FF_TC_CODE_HELCY, sizeof(cTc)) ) {
		if( QUE_SEND_HELCY != m_ExecItem.emStat ) {
			LOG(em_ERR), "[%s] �V�[�P���X�s��v�ȉ����`������M�B(%s, No=%d, TC=%.2s)", my_sThreadName, GetSendStatName(), m_ExecItem.nRecvNowNo, cTc );
			syslog( PARAM_SYSLOG_BASENO + 13, "[�V�[�P���X�s��v�ȉ����`������M�B(%s, No=%d, TC=%.2s)]", GetSendStatName(), m_ExecItem.nRecvNowNo, cTc );
			return false;
		}

	} else {
		LOG(em_ERR), "[%s] �s�������`������M���܂����B(%s, No=%d, TC=%.2s)", my_sThreadName, GetSendStatName(), m_ExecItem.nRecvNowNo, cTc );
		syslog( PARAM_SYSLOG_BASENO + 13 , "[�s�������`������M���܂����B(%s, No=%d, TC=%.2s)]", GetSendStatName(), m_ExecItem.nRecvNowNo, cTc );
		return false;
	}


	////////////////////////////////////
	// �Ɩ��`������
	////////////////////////////////////
	//// ���e�m�F
	int nAnser = SetRecvData( &m_ExecItem, nSize, data, cTc );
	if( RECV_RET_ERR_NONE == nAnser ) {
		// �����`���҂��^�C���A�E�g�L�����Z��
		CancelWaitableTimer(m_evAnserTimeOut);
		LOG(em_ERR), "[%s] �V�[�P���X�ُ튮�� <<<==================", my_sThreadName);

		return false;
	}
	else if( RECV_RET_ERR_IDLE == nAnser ) {
		// �����`���҂��^�C���A�E�g�L�����Z��
		CancelWaitableTimer(m_evAnserTimeOut);

		//// �X�e�[�^�X�ύX
		m_eStateMain = SOCK_IDLE;
		LOG(em_ERR), "[%s] �V�[�P���X�ُ튮�� <<<==================", my_sThreadName);
	}
	else if(RECV_RET_OK == nAnser) {
		// �����`���҂��^�C���A�E�g�L�����Z��
		CancelWaitableTimer(m_evAnserTimeOut);	
		
		//// �X�e�[�^�X�ύX
		m_eStateMain = SOCK_IDLE;
		LOG(em_MSG), "[%s] �V�[�P���X���� <<<==================", my_sThreadName);
	}
	else if( RECV_RET_NEXT == nAnser ) {
		// ���`�����M�J�n
		if( ! SendExec() ) return false;
	}
	else if( RECV_RET_NONEXT == nAnser ) {
		// �������Ȃ�
	}


	//// ����M�J�n
	// ��M�|�[�g�� ��M�҂��ɂ��Ă����B
	if( ! RecvStart( PORT_RECV, sizeof(FF_ANS), m_cSockBuf[PORT_RECV][BUF_RECV] )) {
		return false;
	}
	return true;
}



// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// �ŗL����

//------------------------------------------
// ��M�f�[�^����
// ExecItem* item ����M�f�[�^
// int nSize ��M�T�C�Y
// BYTE const* data ��M�f�[�^
// char const* cTc ��M�g�����U�N�V�����R�[�h
// �߂�l
// -9:���Ȃ�ُ�(�ؒf�����K�v) -1:�ُ�(�ؒf�͂��Ȃ�)  0:1�V�[�P���X���ׂĊ���
//  1:�����M���������{���邱��  9:����M���������{���邱��(=�������Ȃ�)
//------------------------------------------
int SockManager::SetRecvData( ExecItem* item, int nSize, BYTE const* data, char const* cTc )
{
	int nAnser = 0;
  	char cMeiNo[SIZE_MEI_NO+1];		// ����No

	//// �f�[�^�̍X�V
	int rec = item->nRecvNowNo;
	item->nRecvSize[rec] = nSize;
	memcpy(item->cRecvBuf[rec], data, nSize);
	item->nRecvNowNo ++;


	//// �e�`������
//-----------------------------------------------------------------------------------------------
	if(QUE_SEND_ZISEKI == item->emStat) {								// �r���ѓ`��
		FF_ANS& pBuf = (FF_ANS&)item->cRecvBuf[rec];
		
		//// �������m�F
		// �����X�e�[�^�X�`�F�b�N
		SockDataExec::FF_SENDKIND_NO emKind;

		memset( cMeiNo, 0x00, sizeof(cMeiNo));
		memcpy( cMeiNo, pBuf.Hd.Meino, SIZE_MEI_NO );

		if( 0 == memcmp(pBuf.Data.Status, FF_ANS_OK, sizeof(pBuf.Data.Status)) ) {
			// ����
			LOG(em_MSG), "[%s] �����`�������X�e�[�^�X ���� [����No=%s]", my_sThreadName, cMeiNo);
			syslog( PARAM_SYSLOG_BASENO + 1, "[����No=%s]", cMeiNo);
			emKind = SockDataExec::FF_SENDKIND_OK;		// ���푗�M������

		} else if( 0 == memcmp(pBuf.Data.Status, FF_ANS_IPCHK_NG, sizeof(pBuf.Data.Status)) ) {
			// �C���v�b�g�`�F�b�NNG
			LOG(em_MSG), "[%s] �����`�������X�e�[�^�X �C���v�b�g�ُ� [����No=%s]", my_sThreadName, cMeiNo);
			syslog( PARAM_SYSLOG_BASENO + 2, "[ID=%s, �C���v�b�g�`�F�b�N�G���[]", item->cKeyId);
			emKind = SockDataExec::FF_SENDKIND_NOTSEND;	// �đ��M�Ȃ�
		
		} else if( 0 == memcmp(pBuf.Data.Status, FF_ANS_DBERRER, sizeof(pBuf.Data.Status)) ) {
			// DB�o�^�ُ�
			LOG(em_MSG), "[%s] �����`�������X�e�[�^�X DB�ُ� [����No=%s]", my_sThreadName, cMeiNo);
			syslog( PARAM_SYSLOG_BASENO + 2, "[ID=%s, DB�o�^�G���[]", item->cKeyId);
			emKind = SockDataExec::FF_SENDKIND_ERR;		// �đ��M����
		
		} else {
			LOG(em_MSG), "[%s] �����`�������X�e�[�^�X �s�� [����No=%s][%.2s]", my_sThreadName, cMeiNo, pBuf.Data.Status);
			syslog( PARAM_SYSLOG_BASENO + 2, "[ID=%s, �s��=%.2s]", item->cKeyId, pBuf.Data.Status);
			emKind = SockDataExec::FF_SENDKIND_ERR;		// �đ��M����
		}
		
		//// ���`���𑗎�M
		if( SockDataExec::FF_SENDKIND_OK == emKind &&
			item->nRecvNowNo != item->nRecvMaxNo ) {
			return RECV_RET_NEXT;
		}

		//// ����ŏI���
		// DB�Ɍ��ʔ��f
		mcls_pData->SetZisekiSendAnser(item->cKeyId, emKind);
		// ����M�f�[�^������
		memset(item, 0x00, sizeof(ExecItem));
		// �����M�˗�
		AddQue_SendZiseki();

		if(SockDataExec::FF_SENDKIND_OK != emKind)  return RECV_RET_ERR_IDLE;
		else										return RECV_RET_OK;
	}

//-----------------------------------------------------------------------------------------------
	else if( QUE_SEND_MAE == item->emStat) {							// �O�H�����їv��
		FF_MAE_JISSEKI& pBuf = (FF_MAE_JISSEKI&)item->cRecvBuf[rec];

		memset( cMeiNo, 0x00, sizeof(cMeiNo));
		memcpy( cMeiNo, pBuf.Hd.Meino, SIZE_MEI_NO );
		
		//// �������m�F
		// ����No�`�F�b�N
		if( 0 != strcmp(cMeiNo, item->cMeiNo) ) {
			LOG(em_MSG), "[%s] �����`�� ����No�s��v [%s/%s]", my_sThreadName, cMeiNo, item->cMeiNo);
			syslog( PARAM_SYSLOG_BASENO + 5, "[����No�s��v:%s/%s]", cMeiNo, item->cMeiNo);
			return RECV_RET_ERR_IDLE;

		}

		if( 1 == rec ){ // �����`��(��)�̎� 
			FF_MAE_JISSEKI& pBufO = (FF_MAE_JISSEKI&)item->cRecvBuf[0];
			if( 0 != strncmp(pBufO.Hd.Meino, item->cMeiNo, SIZE_MEI_NO) ){ // �\�̖���No�s��v
				LOG(em_MSG), "[%s] �����`��(�\) ����No�s��v�̂��ߕۑ����܂��� [%.*s/%s]", my_sThreadName, SIZE_MEI_NO, pBufO.Hd.Meino, item->cMeiNo);
//				syslog( PARAM_SYSLOG_BASENO + 5, "[����No�s��v:%.*s/%s]", SIZE_MEI_NO, pBufO.Hd.Meino, item->cMeiNo); // �\��M���ɏo���Ă���͂��Ȃ̂ŕs�v
				return RECV_RET_ERR_IDLE;
			}
		}

		//// ���`������M
		if(item->nRecvNowNo != item->nRecvMaxNo ) {
			return RECV_RET_NONEXT;
		}

		//// ����ŏI���
		syslog( PARAM_SYSLOG_BASENO + 4 , "[����No=%s]", item->cMeiNo);
		// DB�Ɍ��ʔ��f
		mcls_pData->InsertMaeBuf(item);
		// ����M�f�[�^������
		memset(item, 0x00, sizeof(ExecItem));
		return RECV_RET_OK;
	}

//-----------------------------------------------------------------------------------------------
	else if( QUE_SEND_HELCY == item->emStat) {							// �w���V�[
		//// ����ŏI���
		LOG(em_MSG), "[%s] �w���V�[�����`�� ��M����", my_sThreadName);
		syslog( PARAM_SYSLOG_BASENO + 6, "");
		
		//// ����w���V�[�`�����M�^�C�}�[
		SetHelcySendTimer(PARAM_HELCYSEND_TIMER);
		return RECV_RET_OK;
	}

//-----------------------------------------------------------------------------------------------
	else {
		LOG(em_ERR), "[%s] �s���`�� ��M�I�I", my_sThreadName);
		return RECV_RET_ERR_NONE;
	}
}