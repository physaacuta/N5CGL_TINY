#include "StdAfx.h"
#include "BaseTcpIp.h"

//////////////////////////////////////////////////////////////////////
// �\�z/����
//////////////////////////////////////////////////////////////////////
//------------------------------------------
// �R���X�g���N�^
// DELIVERY_COMMON* del �󂯓n���\����
//------------------------------------------
BaseTcpIp::BaseTcpIp(DELIVERY_COMMON* del) :
my_emSockState(STATE_NONE),
my_pNowDataR(NULL),
my_pNowDataS(NULL),
my_nCloseKind(-1),
my_bIsErrConnectQueing(false),
my_tId_ReTry(0),
mque_Running(16),
gque_Anser(16),
mcls_pSock(NULL),
mcls_pLog(NULL)
{
	// �����Z�b�g
	memcpy(&my_Del_Common, del, sizeof(my_Del_Common) );

	// �V�O�i������
	my_evUserClose   = CreateEvent(NULL, FALSE, FALSE, NULL);		// ���[�U�[����̋����ؒf�ʒm
	my_evRunningEnd  = CreateEvent(NULL, FALSE, FALSE, NULL);		// ���[�U�[����̑����Ԋ���������ʒm

	// �����V�O�i�� (�\�P�b�g�֘A)
	my_evReTryConnect = CreateWaitableTimer(NULL, FALSE, NULL);		// �Đڑ��v���^�C�}�C�x���g
	my_evAccept		  = CreateEvent(NULL, FALSE, FALSE, NULL);		// Accept �C�x���g
	my_evDisConnect	  = CreateEvent(NULL, FALSE, FALSE, NULL);		// �ؒf �C�x���g
	my_evSendTimeout  = CreateEvent(NULL, FALSE, FALSE, NULL);		// ���M�^�C���A�E�g �C�x���g
	my_evRecvEnd	  = CreateEvent(NULL, FALSE, FALSE, NULL);		// ��M���� �C�x���g
	my_evSendEnd	  = CreateEvent(NULL, FALSE, FALSE, NULL);		// ���M���� �C�x���g

	my_evLockRecvEnd = CreateEvent(NULL, FALSE, FALSE, NULL);		// �u���b�L���O��M�����C�x���g
	my_evLockSendEnd = CreateEvent(NULL, FALSE, FALSE, NULL);		// �u���b�L���O���M�����C�x���g
}

//------------------------------------------
// �f�X�g���N�^
//------------------------------------------
BaseTcpIp::~BaseTcpIp(void)
{
	// �V�O�i���J��
	CloseHandle(my_evUserClose);
	CloseHandle(my_evRunningEnd);

	// �����V�O�i�� (�\�P�b�g�֘A)
	CloseHandle(my_evReTryConnect);
	CloseHandle(my_evAccept);
	CloseHandle(my_evDisConnect);
	CloseHandle(my_evSendTimeout);
	CloseHandle(my_evRecvEnd);
	CloseHandle(my_evSendEnd);

	CloseHandle(my_evLockRecvEnd);
	CloseHandle(my_evLockSendEnd);
}

//------------------------------------------
// ���݂̃X�e�[�^�X����
// SOCK_STATE st �X�e�[�^�X
//------------------------------------------
CString BaseTcpIp::GetSockStateName(SOCK_STATE st) const
{
	CString wk;
	if		(st == STATE_NONE)		{ wk = "������� (�ؒf��)"; }
	else if (st == STATE_IDLE)		{ wk = "�������� (�ڑ���)"; }
	else if (st == STATE_RSING)		{ wk = "����M��";}
	else if (st == STATE_RECVING)	{ wk = "��M��";}
	else if (st == STATE_SENDING)	{ wk = "���M��";}
	else							{ wk = "�X�e�[�^�X�ُ�";}
	return wk;
}

//------------------------------------------
// �A���T�[����
// DELI_ANS_STATE st �A���T�[�X�e�[�^�X
//------------------------------------------
CString BaseTcpIp::GetAnsStateName(DELI_ANS_STATE st) const
{
	CString wk;
	if		(st == ANS_CONNECT)		{ wk = "�ڑ������ʒm"; }
	else if (st == ANS_CLOSE)		{ wk = "�ؒf�����ʒm"; }
	else if (st == ANS_RECVEND)		{ wk = "�񓯊���M�����ʒm";}
	else if (st == ANS_SENDEND)		{ wk = "�񓯊����M�����ʒm";}
	else if (st == ANS_ERR_CONNECT)	{ wk = "�R�l�N�g���s�ʒm";}
	else							{ wk = "�X�e�[�^�X�ُ�";}
	return wk;
}

//------------------------------------------
// �X���b�h
//------------------------------------------
UINT WINAPI BaseTcpIp::MainThread(LPVOID param)
{
	////// ���C���C���X�^���X���o��
	BaseTcpIp *p = (BaseTcpIp *)param;
	p->SetTid();								// ���X���b�hID�Z�b�g

	//// WaitForMultipleObjects�C�x���g
	HANDLE	hArray[] = {p->my_evStop,
						p->my_evUserClose,
						p->my_evReTryConnect,
						p->my_evAccept,
						p->my_evDisConnect,
						p->my_evSendTimeout,
						p->my_evRecvEnd,
						p->my_evSendEnd,
						p->mque_Running.g_evSem
						};
	enum {	EV_STOP = WAIT_OBJECT_0,		// �X���b�h�I��
			EV_USERCLOSE,					// ���[�U�[����̋����ؒf�ʒm
			EV_RETRY,						// �Đڑ��v���^�C�}�C�x���g
			EV_ACCEPT,						// Accept �C�x���g
			EV_DISCONNECT,					// �ؒf�C�x���g
			EV_SENDTIMEOUT,					// ���M�^�C���A�E�g �C�x���g
			EV_RECVEND,						// ��M�����C�x���g
			EV_SENDEND,						// ���M�����C�x���g
			EV_QUE,							// �L���[�C�x���g
			EV_END	};


	//// �\�P�b�g �C���X�^���X�̐���
	p->SockNew(true);

	//// Listen (�T�[�o�[���̂�) (��������܂Ńu���b�N)
	if( p->my_Del_Common.bSorC ) {
		bool disp = true;
		while( TRUE ) {
			if( p->Listen(disp) ) break;		// �ŏ��̈��̂݃��O�o�� (�ʏ�Listen�Ŏ��s���鎖����)
			Sleep(2000);
			disp = false;
		}
	}
	//// Connect (�N���C�A���g���̂�)
	p->my_tId_ReTry = 0;
	if( ! p->my_Del_Common.bSorC ) {
		if( ! p->Connect(true) ) {
			p->Close(true);				// ���g���C�^�C�}�̈�
		}
	}

	// ���b�Z�[�W�L���[�ǂݍ���
	pLOG(em_INF), "[BaseTcpIp] <%d:%s> �X���b�h�J�n [0x%X]", p->my_Del_Common.nMyID, p->my_Del_Common.cMyName, p->GetTid());
	while (1) {
		//int ret = MsgWaitForMultipleObjects(EV_END, hArray, FALSE, INFINITE, QS_POSTMESSAGE | QS_TIMER);
		DWORD ret = WaitForMultipleObjects(EV_END, hArray, FALSE, INFINITE);
		switch (ret) {

//-----------------------------------------------------------------------------------------------
		case EV_STOP:												// �X���b�h�I��
			// �ؒf (�ʒm�������������ׁA�蓮�ő���)
			if( NULL != p->mcls_pSock ) {
				if( p->my_Del_Common.bSorC ) p->mcls_pSock->CloseAll();		// �Ȃ����Ă���S�ẴN���C�A���g��ؒf����	
				else						 p->SockDelete(false);			// �C���X�^���X�J��
			}
			// �\�P�b�g �C���X�^���X�̊J��
			p->SockDelete(true);

			// �񓯊��p�̃C���X�^���X���c���Ă����� �d���������̂ł��̃N���X�ŏ�����z��������  // �� �z���g�ɂ����H�H
			SAFE_DELETE(p->my_pNowDataS);
			SAFE_DELETE(p->my_pNowDataR);

			pLOG(em_INF), "[BaseTcpIp] <%d:%s> �X���b�h�I�� [0x%X]", p->my_Del_Common.nMyID, p->my_Del_Common.cMyName, p->GetTid());
			return 0;

//-----------------------------------------------------------------------------------------------
		case EV_USERCLOSE:											// ���[�U�[����̋����ؒf�ʒm
			// �\�P�b�g�ؒf
			p->Close(true);
			break;

//-----------------------------------------------------------------------------------------------
		case EV_RETRY:												// �Đڑ��v���^�C�}�C�x���g
			if( ! p->Connect(false) ) {
				p->Close(false);											// ���g���C�^�C�}�̈�
			}
			break;

//-----------------------------------------------------------------------------------------------
		case EV_ACCEPT:												// Accept �C�x���g
			if( ! p->Accept() ) {
				p->Close(true);												// ���g���C�^�C�}�̈�
			}
			break;

//-----------------------------------------------------------------------------------------------
		case EV_DISCONNECT:											// �ؒf�C�x���g
			if(NULL != p->mcls_pSock) p->my_nCloseKind = p->mcls_pSock->GetCloseKindID(0);			// �ؒf��ʎ擾
			p->Close(true);
			break;

//-----------------------------------------------------------------------------------------------
		case EV_SENDTIMEOUT:										// ���M�^�C���A�E�g �C�x���g
			pLOG(em_WAR), "[BaseTcpIp] <%d:%s> �\�P�b�g ���M�^�C���A�E�g", p->my_Del_Common.nMyID, p->my_Del_Common.cMyName);
			if(NULL != p->my_pNowDataS ) {
				p->my_pNowDataS->nAns = 1; // �^�C���A�E�g
				p->CheckSendEnd();
			}
			break;

//-----------------------------------------------------------------------------------------------
		case EV_RECVEND:											// ��M�����C�x���g
		//	pLOG(em_INF), "[BaseTcpIp] <%d:%s> �\�P�b�g ��M����", p->my_Del_Common.nMyID, p->my_Del_Common.cMyName);
			// �X�e�[�^�X�ύX
			if( STATE_RSING == p->my_emSockState ) p->my_emSockState = STATE_SENDING;
			else								   p->my_emSockState = STATE_IDLE;
			// �ʒm ���� �J��
			p->CheckRecvEnd();
			break;

//-----------------------------------------------------------------------------------------------
		case EV_SENDEND:											// ���M�����C�x���g
		//	pLOG(em_INF), "[BaseTcpIp] <%d:%s> �\�P�b�g ���M����", p->my_Del_Common.nMyID, p->my_Del_Common.cMyName);
			// �X�e�[�^�X�ύX
			if( STATE_RSING == p->my_emSockState ) p->my_emSockState = STATE_RECVING;
			else								   p->my_emSockState = STATE_IDLE;
			// �ʒm ���� �J��
			p->CheckSendEnd();
			break;

//-----------------------------------------------------------------------------------------------
		case EV_QUE:												// ����M����˗�
			p->RunningStart();
			break;
		}
	}
	return 0;
}

// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// �V�[�P���X����

//------------------------------------------
// �񓯊� ����M����˗�
// �߂�l false:�L���[�ɗ��܂�߂��H
//------------------------------------------
bool BaseTcpIp::SetRunningData(DELIVERY_DATA* data)
{
	// �O�̔񓯊�����p�����H
	if( data->bRorS ) {
		if(NULL != my_pNowDataR) {
			return false;}
	} else {
		if(NULL != my_pNowDataS) return false;
	}
	
	// �o�^
	if( 0 != mque_Running.GetFreeCount() ) {		// �o�^
		mque_Running.AddToTail(data);
		return true;
	} else {									// �󂫖���
		return false;
	}
}


//------------------------------------------
// �񓯊� ����M����˗�
//------------------------------------------
void BaseTcpIp::RunningStart()
{
	//// ����v�����o��
	DELIVERY_DATA* wk;
	wk = mque_Running.GetFromHead();
	wk->nAns = -9;								// �X�e�[�^�X������

	//// �\�P�b�g�L���H
	if( ! IsConnect() ) {
		if( wk->bRorS ){
			SetAnserQue(ANS_RECVEND, wk);
		} else{
			SetAnserQue(ANS_SENDEND, wk);
		}
		return;
	}

	//// �O�̔񓯊�����p����
	if( wk->bRorS ) {
		if(NULL != my_pNowDataR) {
			wk->nAns = -7;
			SetAnserQue(ANS_RECVEND, wk);
			return;
		}
	} else {
		if(NULL != my_pNowDataS) {
			wk->nAns = -7;
			SetAnserQue(ANS_SENDEND, wk);
			return;
		}
	}

	//// ������s
	if( wk->bRorS ) {								// ��M
	//	LOG(em_INF), "[BaseTcpIp] <%d:%s> �\�P�b�g ��M�J�n", my_Del_Common.nMyID, my_Del_Common.cMyName);

		wk->nAns = mcls_pSock->Recv(0, &wk->Data[wk->nStartPoint], wk->nSize, my_evRecvEnd );

		// ��ԕύX
		if(0 == wk->nAns) {					// ��M���쐬��
			if( STATE_SENDING == my_emSockState ) my_emSockState = STATE_RSING;
			else								  my_emSockState = STATE_RECVING;
			my_pNowDataR = wk;							// ����f�[�^�Z�b�g
		} else {										// ��M���쎸�s
			SetAnserQue(ANS_RECVEND, wk);					// ��M�����ʒm
		}

	} else {										// ���M
	//	LOG(em_INF), "[BaseTcpIp] <%d:%s> �\�P�b�g ���M�J�n", my_Del_Common.nMyID, my_Del_Common.cMyName);

		wk->nAns = mcls_pSock->Send(0, &wk->Data[wk->nStartPoint], wk->nSize, my_evSendEnd );

		// ��ԕύX
		if(0 == wk->nAns) {					// ���M���쐬��
			if( STATE_RECVING == my_emSockState ) my_emSockState = STATE_RSING;
			else								  my_emSockState = STATE_SENDING;
			my_pNowDataS = wk;							// ����f�[�^�Z�b�g
		} else {										// ���M���쎸�s
			SetAnserQue(ANS_SENDEND, wk);					// ���M�����ʒm
		}
	}
}

//------------------------------------------
// �O���ʒm�p�ɃL���[�C���O
// DELI_ANS_STATE ans �X�e�[�^�X
// DELIVERY_DATA* data �f�[�^ (����M�ȊO��NULL) �f�t�H���gNULL�w��
//------------------------------------------
bool BaseTcpIp::SetAnserQue(DELI_ANS_STATE state, DELIVERY_DATA* data)
{
	// �o�^
	if( 0 != gque_Anser.GetFreeCount() ) {		// �o�^
		// �󂯓n���f�[�^����
		DELIVERY_ANS * ans = new DELIVERY_ANS;
		ans->nMyID = my_Del_Common.nMyID;
		ans->state = state;
		ans->Data = data;
		
		// �o�^
		gque_Anser.AddToTail(ans);
		return true;
	} else {									// �󂫖���
		LOG(em_WAR), "[BaseTcpIp] <%d:%s> �A���T�[�L���[�󂫖���", my_Del_Common.nMyID, my_Del_Common.cMyName);
		return false;
	}
}

//------------------------------------------
// ���� ���M
// BYTE* data ���M�f�[�^
// int size ���M�T�C�Y
// int timeout �^�C���A�E�g����
// �߂�l ���A���
//			0 :���폈������
//			-1:���̃C���X�^���X�͑O��SEND�����s��
//			-2:�q�X���b�h���Ƃ܂��Ă���
//			-9:�\�P�b�g������
//------------------------------------------
int BaseTcpIp::LockSend(BYTE* data, int size, int timeout)
{
	//// �\�P�b�g�L���H
	if( ! IsConnect() ) return -9;

	// ���� ���M
	int retc;
	retc = mcls_pSock->Send(0, data, size, my_evLockSendEnd );
	if(0 != retc) return retc;			// �������s

	// �������� ���� �^�C���A�E�g �܂Ńu���b�N
	DWORD ret = WaitForSingleObject(my_evLockSendEnd, timeout);
	if (WAIT_TIMEOUT == ret) return -1;	// �R�}���h��������
	return 0;
}


//------------------------------------------
// ���� ��M
// BYTE* data ��M�f�[�^
// int size ��M�T�C�Y
// int timeout �^�C���A�E�g����
// �߂�l ���A���
//			0 :���폈������
//			-1:���̃C���X�^���X�͑O��SEND�����s��
//			-2:�q�X���b�h���Ƃ܂��Ă���
//			-9:�\�P�b�g������
//------------------------------------------
int BaseTcpIp::LockRecv(BYTE* data, int size, int timeout)
{
	//// �\�P�b�g�L���H
	if( ! IsConnect() ) return -9;

	// ���� ��M
	int retc;
	retc = mcls_pSock->Recv(0, data, size, my_evLockRecvEnd );
	if(0 != retc) return retc;			// �������s

	// �������� ���� �^�C���A�E�g �܂Ńu���b�N
	DWORD ret = WaitForSingleObject(my_evLockRecvEnd, timeout);
	if (WAIT_TIMEOUT == ret) return -1;	// �R�}���h��������
	return 0;
}



//------------------------------------------
// �񓯊� ��M����
//------------------------------------------
void BaseTcpIp::CheckRecvEnd()
{
	if(NULL==my_pNowDataR) return;
	// �V�O�i���󂯎��ƒ������������̂ŁA�܂�NULL�ɂ��Ă���
	DELIVERY_DATA* wk = my_pNowDataR;
	my_pNowDataR = NULL;
	// �ʒm or �J�� ?
	if( wk->bAnsQueing ) {
		SetAnserQue(ANS_RECVEND, wk);
	} else {
		DeleteEndData(wk);
	}
	my_pNowDataR = NULL;
}

//------------------------------------------
// �񓯊� ���M����
//------------------------------------------
void BaseTcpIp::CheckSendEnd()
{
	if(NULL==my_pNowDataS) return;
	// �V�O�i���󂯎��ƒ������������̂ŁA�܂�NULL�ɂ��Ă���
	DELIVERY_DATA* wk = my_pNowDataS;
	my_pNowDataS = NULL;
	// �ʒm or �J�� ?
	if( wk->bAnsQueing ) {
		SetAnserQue(ANS_SENDEND, wk);
	} else {
		DeleteEndData(wk);
	}
	my_pNowDataS = NULL;
}

//------------------------------------------
// �񓯊� ����M�f�[�^�̊��������̂������J�� (����M�̌��ʏ���߂�l�Ƃ��ĕԂ�)
//------------------------------------------
int BaseTcpIp::DeleteEndData(DELIVERY_DATA* pData )	
{	
	if(NULL==pData) return false;
	int wk = pData->nAns;
	SAFE_DELETE_ARRAY(pData->Data);
	SAFE_DELETE(pData); 
	return wk;
};

// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// TPC/IP �ʐM����

//------------------------------------------
// �C���X�^���X����
// bool bLogDsp ���O�o�͗L�� (false:���s���O�o���Ȃ�)
//------------------------------------------
BOOL BaseTcpIp::SockNew(bool bLogDsp)
{
	// �\�P�b�g�C���X�^���X����
	try {
		mcls_pSock = new SockBase( my_Del_Common.cSession, TASKINI_NAME, GetTid(), PARAM_SOCK_TIMEOUT, 1, 
									my_Del_Common.nBufferSize, my_Del_Common.nBufferSize);
		mcls_pSock->SetOpt_NewAcceptNgMode(1);		// �A�N�Z�v�g�ُ펞�̋�����ݒ�B�n�[�t�R�l�N�V�������̕ێ�IP�ƐV�KIP�����ꎞ�ɂ́A�V�K�Z�b�V�����Ƃ��Ď󂯕t����

	} catch(int code) {
		mcls_pSock = NULL;
		if(bLogDsp) {
			LOG(em_WAR), "[BaseTcpIp] <%d:%s> �\�P�b�g New���s err_code=%d", my_Del_Common.nMyID, my_Del_Common.cMyName, code);
			syslog(SYSNO_SOCK_NEW_ERR, "[%d:%s err_code=%d]", my_Del_Common.nMyID, my_Del_Common.cMyName, code);
		}
		return FALSE;
	}

	//// �㏈��
	if( my_Del_Common.bSorC ) mcls_pSock->SetEvAccept(my_evAccept);	// Accept�F�����C�x���g�ɂ��� (�T�[�o�[�̂�)
	mcls_pSock->SetEvDisConnect(my_evDisConnect);					// �ؒf�F�����C�x���g�ɂ���
	mcls_pSock->SetEvSendTimeout(my_evSendTimeout);					// ���M�^�C���A�E�g�F�����C�x���g�ɂ���
	return TRUE;
}
//------------------------------------------
// �C���X�^���X�J��
// bool bLogDsp ���O�o�͗L�� (false:���s���O�o���Ȃ�)
//------------------------------------------
BOOL BaseTcpIp::SockDelete(bool bLogDsp)
{
	// �C���X�^���X�J��
	SAFE_DELETE( mcls_pSock);
	return TRUE;
}

//------------------------------------------
// Listen
// bool bLogDsp ���O�o�͗L�� (false:���s���O�o���Ȃ�)
//------------------------------------------
BOOL BaseTcpIp::Listen(bool bLogDsp)
{
	int retc;

	// Listen
	if( NULL != mcls_pSock ) {
		retc = mcls_pSock->Listen();
		if( 0 != retc ) {
			if(bLogDsp) {
				LOG(em_WAR), "[BaseTcpIp] <%d:%s> �\�P�b�g Listen���s err_code=%d", my_Del_Common.nMyID, my_Del_Common.cMyName, retc);
				syslog(SYSNO_SOCK_LISTEN_ERR, "[%d:%s err_code=%d]", my_Del_Common.nMyID, my_Del_Common.cMyName, retc);
			}
			return FALSE;
		}
	}

	LOG(em_INF), "[BaseTcpIp] <%d:%s> �\�P�b�g Listen�J�n", my_Del_Common.nMyID, my_Del_Common.cMyName);
	return TRUE;
}

//------------------------------------------
// �ڑ�(�u���b�L���O)
// bool bLogDsp ���O�o�͗L�� (false:���s���O�o���Ȃ�)
//------------------------------------------
BOOL BaseTcpIp::Connect(bool bLogDsp)
{
	int retc;

	// �C���X�^���X���o���Ė��������琶��
	if( NULL == mcls_pSock ) {
		SockNew(false);
	}

	// �R�l�N�g
	if( NULL != mcls_pSock ) {
		retc = mcls_pSock->Connect();
		if( 0 != retc ) {
			//delete mcls_pSock;
			//mcls_pSock = NULL;
			if(bLogDsp) {
				LOG(em_WAR), "[BaseTcpIp] <%d:%s> �\�P�b�g Connect���s err_code=%d", my_Del_Common.nMyID, my_Del_Common.cMyName, retc);
				syslog(SYSNO_SOCK_CONNECT_ERR, "[%d:%s err_code=%d]", my_Del_Common.nMyID, my_Del_Common.cMyName, retc);
			}

			// �R�l�N�g���s�� �L���[�C���O
			if(my_bIsErrConnectQueing) SetAnserQue(ANS_ERR_CONNECT);

			return FALSE;
		}
	}

	//// �o�b�t�@�m�F
	if(NULL!=my_pNowDataR) {
		SAFE_DELETE(my_pNowDataR);
		LOG(em_WAR), "[BaseTcpIp] <%d:%s> �\�P�b�g Connect�� �񓯊���M�o�b�t�@�f�[�^�L��", my_Del_Common.nMyID, my_Del_Common.cMyName);
	}
	if(NULL!=my_pNowDataS) {
		SAFE_DELETE(my_pNowDataS);
		LOG(em_WAR), "[BaseTcpIp] <%d:%s> �\�P�b�g Connect�� �񓯊����M�o�b�t�@�f�[�^�L��", my_Del_Common.nMyID, my_Del_Common.cMyName);
	}

	//// ��ԕύX
	my_nCloseKind = -1;
	my_emSockState = STATE_IDLE;
	SetAnserQue(ANS_CONNECT);				// �ڑ������ʒm

	LOG(em_INF), "[BaseTcpIp] <%d:%s> �\�P�b�g Connect����", my_Del_Common.nMyID, my_Del_Common.cMyName);
	syslog(SYSNO_SOCK_CONNECT, "[%d:%s]", my_Del_Common.nMyID, my_Del_Common.cMyName);
	return TRUE;
}

//------------------------------------------
// Accept (�V�O�i���ʒm�p)
//------------------------------------------
BOOL BaseTcpIp::Accept()
{
	// Accept
	if( NULL != mcls_pSock ) {
		if( mcls_pSock->Accept() < 0 ) {
			LOG(em_WAR), "[BaseTcpIp] <%d:%s> �\�P�b�g Accept���s �N���C�A���g���I�[�o�[", my_Del_Common.nMyID, my_Del_Common.cMyName);
			syslog(SYSNO_SOCK_CLIENT_MAX, "[%d:%s]", my_Del_Common.nMyID, my_Del_Common.cMyName);
			return FALSE;
		}
	}

	//// �o�b�t�@�m�F
	if(NULL!=my_pNowDataR) {
		SAFE_DELETE(my_pNowDataR);
		LOG(em_WAR), "[BaseTcpIp] <%d:%s> �\�P�b�g Accept�� �񓯊���M�o�b�t�@�f�[�^�L��", my_Del_Common.nMyID, my_Del_Common.cMyName);
	}
	if(NULL!=my_pNowDataS) {
		SAFE_DELETE(my_pNowDataS);
		LOG(em_WAR), "[BaseTcpIp] <%d:%s> �\�P�b�g Accept�� �񓯊����M�o�b�t�@�f�[�^�L��", my_Del_Common.nMyID, my_Del_Common.cMyName);
	}

	//// ��ԕύX
	my_nCloseKind = -1;
	my_emSockState = STATE_IDLE;
	SetAnserQue(ANS_CONNECT);				// �ڑ������ʒm

	LOG(em_INF), "[BaseTcpIp] <%d:%s> �\�P�b�g Accept����", my_Del_Common.nMyID, my_Del_Common.cMyName);
	syslog(SYSNO_SOCK_CONNECT, "[%d:%s]", my_Del_Common.nMyID, my_Del_Common.cMyName);
	return TRUE;
}

//------------------------------------------
// �ؒf
// bool bLogDsp ���O�o�͗L�� (false:���s���O�o���Ȃ�)
//------------------------------------------
BOOL BaseTcpIp::Close(bool bLogDsp)
{
	int connectNum = 0;							// �Ȃ����Ă�����

	//// �ؒf
	if( NULL != mcls_pSock ) {
		// ����M���̏ꍇ������̂ŁA�L�����Z�����Ă���
		mcls_pSock->Cancel();
		if(NULL != mcls_pSock) my_nCloseKind = mcls_pSock->GetCloseKindID(0);
		// �T�[�o�[�Ȃ�ؒf���邾���B�N���C�A���g�Ȃ�C���X�^���X���폜
		connectNum = mcls_pSock->GetActive();	
		if( my_Del_Common.bSorC ) mcls_pSock->CloseAll();					// �Ȃ����Ă���S�ẴN���C�A���g��ؒf���� 
		else					  SockDelete(false);						// �C���X�^���X�J��
	}

	//// ���g���C�^�C�}�[�Z�b�g (�N���C�A���g���̂�)
	if( ! my_Del_Common.bSorC ) {
		LARGE_INTEGER ts;
		ts.QuadPart = (__int64)my_Del_Common.nRetryTimer * -10000;	// 1[ms]��100[ns] (�}�C�i�X�w��ŁA�w�莞�Ԍo�߂ŃV�O�i��) (��3������2��ڈȍ~�̋N���Ԋu)
		SetWaitableTimer(my_evReTryConnect, &ts, 0, NULL, NULL, FALSE);
	}

	//// �񓯊����쒆��������A���̃f�[�^���Ԃ��Ă���
	if( STATE_RECVING == my_emSockState ) {
		if(NULL != my_pNowDataR) {
			my_pNowDataR->nAns = -8;	// �񓯊����쒆�ɐؒf
			CheckRecvEnd();
		}
	} else if( STATE_SENDING == my_emSockState ) {
		if(NULL != my_pNowDataS) {
			my_pNowDataS->nAns = -8;	// �񓯊����쒆�ɐؒf
			CheckSendEnd();
		}
	}

	//// ��ԕύX
	my_emSockState = STATE_NONE;
	if( 0 != connectNum) {
		SetAnserQue(ANS_CLOSE);				// �ؒf�����ʒm

		LOG(em_INF), "[BaseTcpIp] <%d:%s> �\�P�b�g �ؒf", my_Del_Common.nMyID, my_Del_Common.cMyName);
		syslog(SYSNO_SOCK_CLOSE, "[%d:%s]", my_Del_Common.nMyID, my_Del_Common.cMyName);
	}
	return TRUE;
}
