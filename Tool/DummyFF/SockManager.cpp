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
m_eStateMain(SOCK_NONE),
m_nSt(0)
{

}

//------------------------------------------
// �f�X�g���N�^
//------------------------------------------
SockManager::~SockManager(void)
{

}

//------------------------------------------
// �X���b�h�J�n�O���� (�X���b�h���s����(�V�O�i���Z�b�g�O)�ɃR�[���o�b�N)
//------------------------------------------
int SockManager::ThreadFirst()
{
	//// TCP/IP ����N���X������
	SockInit(); 

	// WaitForMultipleObjects�C�x���g
	HANDLE	hArray[] = {mcls_pSock[PORT_RECV]->gque_Anser.g_evSem,		// ��M�|�[�g[0] �p����N���X
						mcls_pSock[PORT_SEND]->gque_Anser.g_evSem,		// ���M�|�[�g[1] �p����N���X
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


	enum {	EV_SOCK_RECV_ANS,					// ��M�|�[�g ���쌋��
			EV_SOCK_SEND_ANS,					// ���M�|�[�g ���쌋��
			EV_QUE_DELI							// ���M�˗��L���[�C�x���g
			};

	////// �V�O�i����������
	switch (nEventNo) {
		
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
	//if( SOCK_EXEC == m_eStateMain ) *nNowWaitNum = nDefWaitNum-1;
	//else							*nNowWaitNum = nDefWaitNum;
	*nNowWaitNum = nDefWaitNum;
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
	if (st == QUE_SEND_ZISEKI_T) { wk = "�r���ё��M�@�\"; }
	else if (st == QUE_SEND_ZISEKI_B) { wk = "�r���ё��M�@��"; }
	else if (st == QUE_SEND_STAT_T) { wk = "������ԑ��M�@�\"; }
	else if (st == QUE_SEND_STAT_B) { wk = "������ԑ��M�@��"; }
	else if (st == QUE_SEND_MAE_T) { wk = "�O�H�����їv���@�\"; }
	else if (st == QUE_SEND_MAE_B) { wk = "�O�H�����їv���@��"; }
	else if (st == QUE_SEND_HELCY) { wk = "�w���V�["; }
	else { wk = ""; }
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

			LOG(em_MSG), "[%s] ������� ===================================", my_sThreadName);

			// ��M�|�[�g�� ��M�҂��ɂ��Ă����B
			if( ! RecvStart( PORT_RECV, sizeof(FF_ANS), m_cSockBuf[PORT_RECV][BUF_RECV] )) {
				UserClose();
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
	deli.bSorC = true;
	deli.bRorS = true;
	strcpy_s (deli.cMyName,  "[��M�|�[�g]");
	sprintf(deli.cSession, "%s_RECV", DummyFF);
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
	deli.bSorC = true;
	deli.bRorS = false;
	strcpy_s (deli.cMyName,  "[���M�|�[�g]");
	sprintf(deli.cSession, "%s_SEND", DummyFF);
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
	if( m_bIsConnect ) {		// ���ڑ����H
		// �ǂ̃|�[�g����ł� �N���[�Y�ʒm��������A�S�|�[�g�����I��������
		mcls_pSock[PORT_RECV]->UserClose();
		mcls_pSock[PORT_SEND]->UserClose();
		// �@���Ԃɒʒm
		KizuFunction::SetStatus("FF_CONNECT_ERR_ID", false, false);
		LOG(em_ERR), "[%s] ����ُ� ===================================", my_sThreadName);
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

	LOG(em_MSG), "[%s] �V�[�P���X�I�� (%s) <<<==================", my_sThreadName, GetSendStatName());
	return true;
}

//------------------------------------------
// �Ɩ��`�����M ���ʏ���
//------------------------------------------
bool SockManager::SendExec()
{
	//// �������M
	memcpy(	m_cSockBuf[PORT_SEND][BUF_SEND], m_ExecItem.cSendBuf, m_ExecItem.nSendSize);
	//// ���f�[�^�_���v
	CString dumpName;
	dumpName.Format("%s ���M(��)�_���v (%s).log", TO_FFSERV, GetSendStatName() );
	KizuFunction::DumpData( dumpName, m_ExecItem.nSendSize, m_cSockBuf[PORT_SEND][BUF_SEND], mcls_pData->GetDumpMode());
	int iRetc = mcls_pSock[PORT_SEND]->LockSend( m_cSockBuf[PORT_SEND][BUF_SEND], m_ExecItem.nSendSize, PARAM_SOCK_TIMEOUT );

	if(0 != iRetc) {
		LOG(em_ERR), "[%s] %s ���M���s [����No=%s �R�C��No=%s]", my_sThreadName, GetSendStatName(m_ExecItem.emStat), m_ExecItem.cMeiNo, m_ExecItem.cCoilNo);
		LOG(em_ERR), "[%s] �V�[�P���X�ُ튮�� <<<==================", my_sThreadName);

		return false;

	} else {
		
		//// �X�e�[�^�X�ύX
		m_eStateMain = SOCK_EXEC;
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
	static char	cTc[2];							// �g�����U�N�V�����R�[�h�m�F�p
	memcpy(cTc,  data, sizeof(cTc));
	//// ���f�[�^�_���v
	CString dumpName;
	dumpName.Format("%s ��M(��)�_���v (TC=%.2s).log", DummyFF,cTc);
	KizuFunction::DumpData(dumpName, nSize, data, mcls_pData->GetDumpMode());

	
	LOG(em_MSG), "[%s] �`����M (TC=%.2s)", my_sThreadName, cTc );

	////////////////////////////////////
	// 1�`�� �������m�F (�����Ƃ����Ƃ���)
	////////////////////////////////////
	//// �|�[�g�������`�F�b�N
	if(PORT_SEND == id)  {
		// ���M�|�[�g�ł͎�M���鎖�͂Ȃ��̂ŃG���[�Ƃ���
		LOG(em_ERR), "[%s] ���M�|�[�g�ŉ����`���͎�M���܂���", my_sThreadName);
		return false;
	}

	////// �V�[�P���X�m�F
	//if (0 == memcmp(cTc, FF_TC_CODE_JI_O, sizeof(cTc))) {

	//}
	//else if (0 == memcmp(cTc, FF_TC_CODE_JI_U, sizeof(cTc))) {

	//}
	//else if (0 == memcmp(cTc, FF_TC_CODE_STSJI_O, sizeof(cTc))) {

	//}
	//else if (0 == memcmp(cTc, FF_TC_CODE_STSJI_U, sizeof(cTc))) {

	//}	
	//else if( 0 == memcmp( cTc, FF_TC_CODE_MAEREQ, sizeof(cTc))){

	//}
	//else if( 0 == memcmp( cTc, FF_TC_CODE_HELCY, sizeof(cTc)) ) {

	//} else {
	//	LOG(em_ERR), "[%s] �s�������`������M���܂����B(%s, No=%d, TC=%.2s)", my_sThreadName, GetSendStatName(), m_ExecItem.nRecvNowNo, cTc );
	//	return false;
	//}


	////////////////////////////////////
	// �Ɩ��`������
	////////////////////////////////////
	//// ���e�m�F
	if( -1 == SetRecvData( data, cTc ) ) return false ;
	//if( RECV_RET_ERR_NONE == nAnser ) {
	//	// �����`���҂��^�C���A�E�g�L�����Z��
	//	CancelWaitableTimer(m_evAnserTimeOut);
	//	LOG(em_ERR), "[%s] �V�[�P���X�ُ튮�� <<<==================", my_sThreadName);

	//	return false;
	//}
	//else if( RECV_RET_ERR_IDLE == nAnser ) {
	//	// �����`���҂��^�C���A�E�g�L�����Z��
	//	CancelWaitableTimer(m_evAnserTimeOut);

	//	//// �X�e�[�^�X�ύX
	//	m_eStateMain = SOCK_IDLE;
	//	LOG(em_ERR), "[%s] �V�[�P���X�ُ튮�� <<<==================", my_sThreadName);
	//}
	//else if(RECV_RET_OK == nAnser) {
	//	// �����`���҂��^�C���A�E�g�L�����Z��
	//	CancelWaitableTimer(m_evAnserTimeOut);	
	//	
	//	//// �X�e�[�^�X�ύX
	//	m_eStateMain = SOCK_IDLE;
	//	LOG(em_MSG), "[%s] �V�[�P���X���� <<<==================", my_sThreadName);
	//}
	//else if( RECV_RET_NEXT == nAnser ) {
	//	// ���`�����M�J�n
	//	if( ! SendExec() ) return false;
	//}
	//else if( RECV_RET_NONEXT == nAnser ) {
	//	// �������Ȃ�
	//}


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
// BYTE const* data ��M�f�[�^
// char const* cTc ��M�g�����U�N�V�����R�[�h
// �߂�l
// -9:���Ȃ�ُ�(�ؒf�����K�v) -1:�ُ�(�ؒf�͂��Ȃ�)  0:1�V�[�P���X���ׂĊ���
//  1:�����M���������{���邱��  9:����M���������{���邱��(=�������Ȃ�)
//------------------------------------------
int SockManager::SetRecvData( BYTE const* data, char* cTc )
{
	int nAnser = 0;
  	char cMeiNo[SIZE_MEI_NO+1];		// ����No
	CString	cWk;
	SYSTEMTIME STime;
	GetLocalTime(&STime);
	

	//// �e�`������
//-----------------------------------------------------------------------------------------------
	if (0 == memcmp(cTc, FF_TC_CODE_JI_O, sizeof(cTc)) ||
		0 == memcmp(cTc, FF_TC_CODE_JI_U, sizeof(cTc))) {				// �r���ѓ`��
		FF_JISSEKI *pRecv = (FF_JISSEKI*)data;

		Delivery* deli = new Delivery;
		memset(deli, 0x00, sizeof(Delivery));
		// �`���R�[�h
		if (0 == memcmp(cTc, FF_TC_CODE_JI_O, sizeof(cTc))) {
			deli->eStat = QUE_SEND_ZISEKI_T;
		}
		else {
			deli->eStat = QUE_SEND_ZISEKI_B;
		}
		// �H���R�[�h
		memcpy(deli->cLine_Code, LINE_CODE, sizeof(deli->cLine_Code));

		// ����No
		memcpy(deli->cMeiNo, pRecv->Hd.Meino, sizeof(deli->cMeiNo));

		// �≄�R�C��No
		memcpy(deli->cCoilNo, pRecv->Hd.Coilno, sizeof(deli->cCoilNo));

		gque_Deli.AddToTail(deli);
	}
//-----------------------------------------------------------------------------------------------
	else if(	0 == memcmp(cTc, FF_TC_CODE_STSJI_O, sizeof(cTc) )||
		0 == memcmp(cTc, FF_TC_CODE_STSJI_U, sizeof(cTc) ) ){				// ������ԓ`��
		FF_STS_JISSEKI *pRecv = (FF_STS_JISSEKI*) data;

		Delivery* deli = new Delivery;
		memset(deli, 0x00, sizeof(Delivery));
		// �`���R�[�h
		if (0 == memcmp(cTc, FF_TC_CODE_STSJI_O, sizeof(cTc))) {
			deli->eStat = QUE_SEND_STAT_T;
		} else {
			deli->eStat = QUE_SEND_STAT_B;
		}
		// �H���R�[�h
		memcpy(deli->cLine_Code, LINE_CODE, sizeof(deli->cLine_Code));

		// ����No
		memcpy(deli->cMeiNo, pRecv->Hd.Meino, sizeof(deli->cMeiNo));

		// �≄�R�C��No
		memcpy(deli->cCoilNo, pRecv->Hd.Coilno, sizeof(deli->cCoilNo));

		gque_Deli.AddToTail(deli);
	}
//-----------------------------------------------------------------------------------------------
	else if (0 == memcmp(cTc, FF_TC_CODE_MAEREQ, sizeof(cTc))) {			// �O�H�����їv��
		for(int ii=0; ii<2; ii++) {
			FF_MAE_REQUEST *pRecv = (FF_MAE_REQUEST*) data;

			Delivery* deli = new Delivery;
			memset(deli, 0x00, sizeof(Delivery));
			// �`���R�[�h
			deli->eStat = 0== ii? QUE_SEND_MAE_T : QUE_SEND_MAE_B;

			// ���M�`���t�@�C����
			cWk.Format(FILE_FORMAT, pRecv->MeireiNo, ii+1);
			memcpy(deli->cFileName, cWk, sizeof(deli->cFileName));

			gque_Deli.AddToTail(deli);
		}
	}

//-----------------------------------------------------------------------------------------------
	else if(0 == memcmp(cTc, FF_TC_CODE_HELCY, sizeof(cTc))) {					// �w���V�[
		FF_MAE_REQUEST *pRecv = (FF_MAE_REQUEST*)data;

		Delivery* deli = new Delivery;
		memset(deli, 0x00, sizeof(Delivery));
		// �`���R�[�h
		deli->eStat = QUE_SEND_HELCY;
		gque_Deli.AddToTail(deli);

	}

//-----------------------------------------------------------------------------------------------
	else {
		LOG(em_ERR), "[%s] �s���`�� ��M�I�I", my_sThreadName);
		return -1;
	}
	return 0;
}