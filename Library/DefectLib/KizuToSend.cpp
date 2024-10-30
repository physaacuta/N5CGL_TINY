#include "StdAfx.h"
#include "KizuToSend.h"


//------------------------------------------
// �R���X�g���N�^
//------------------------------------------
KizuToSend::KizuToSend() :
my_cSession(""),
my_bRetryMode(false),
my_emSockState(STATE_NONE),
my_pHead(NULL),
my_nSendingDataSize(0),
my_nSendNum(1),
mcls_pSock(NULL),
mcls_pLog(NULL)
{
	// �V�O�i������
	my_evReTryConnect	= CreateWaitableTimer(NULL, FALSE, NULL);
	my_evConnect		= CreateEvent(NULL, FALSE, FALSE, NULL);
	my_evDisConnect		= CreateEvent(NULL, FALSE, FALSE, NULL);
	my_evSendTimeout	= CreateEvent(NULL, FALSE, FALSE, NULL);
	my_evSendEnd		= CreateEvent(NULL, FALSE, FALSE, NULL);
	my_evAnserClose		= CreateEvent(NULL, FALSE, FALSE, NULL);

//	// PC���̎擾
//	memcpy(my_cPCname, getmypcname(), SIZE_NAME_PC);
	memset(my_cPCname, 0x00, sizeof(my_cPCname));
}

//------------------------------------------
// �f�X�g���N�^
//------------------------------------------
KizuToSend::~KizuToSend(void)
{
	// �\�P�b�g�j��
	SAFE_DELETE(mcls_pSock);

	// �V�O�i���J��
	CloseHandle(my_evReTryConnect);
	CloseHandle(my_evConnect);
	CloseHandle(my_evDisConnect);
	CloseHandle(my_evSendTimeout);
	CloseHandle(my_evSendEnd);
	CloseHandle(my_evAnserClose);
}

//------------------------------------------
// ���݂̃X�e�[�^�X����
//------------------------------------------
CString KizuToSend::GetSockStateName() const
{
	CString wk;
	if		(my_emSockState == STATE_NONE)		{ wk = "������� (�ؒf��)"; }
	else if (my_emSockState == STATE_IDLE)		{ wk = "�������� (�ڑ���)"; }
	else if (my_emSockState == STATE_HEAD)		{ wk = "�w�b�_�� ���M��";}
	else if (my_emSockState == STATE_DATA)		{ wk = "�f�[�^�� ���M��";}
	else										{ wk = "�X�e�[�^�X�ُ�";}
	return wk;
}


//------------------------------------------
// ���o�b�t�@��ԋp����
//------------------------------------------
void KizuToSend::QueBufSendBack()
{
	if(NULL == my_pHead) return;
	gque_SendBack.AddToTail(my_pHead);
	my_pHead = NULL;
}
//------------------------------------------
// ���ׂẴL���[��ԋp����
//------------------------------------------
void KizuToSend::QueAllSendBack()
{
	SOCK_F_HEAD*		deli;

	// �f�o�b�N�p���O
	if(0 !=gque_Deli.GetMaxCount() ){		// 1��������Ă��Ȃ����͏o���Ȃ�
		LOG(em_MSG), "[KizuToSend] �����M�L���[�J��! Que=%d, MAX=%d", gque_Deli.GetCount(), gque_Deli.GetMaxCount());
	}

	// �L���[�ɗ��܂��Ă���C���X�^���X��S���J��
	while(true) {
		deli = gque_Deli.GetFromHead(0);
		if (deli == NULL) break;

		// �ԋp
		gque_SendBack.AddToTail(deli);
		deli = NULL;
	}
	// �ő匏�����Z�b�g
	gque_Deli.ReSetMaxCount(); 
	// ���o���ς݂̃o�b�t�@������Έꏏ�ɊJ������
	QueBufSendBack();
}


//------------------------------------------
// �X���b�h
//------------------------------------------
UINT WINAPI KizuToSend::MainThread(LPVOID param)
{
	////// ���C���C���X�^���X���o��
	KizuToSend *p = (KizuToSend *)param;
	p->SetTid();								// ���X���b�hID�Z�b�g

	// WaitForMultipleObjects�C�x���g
	HANDLE	hArray[] = {p->my_evStop,
						p->my_evReTryConnect,
						p->my_evConnect,
						p->my_evDisConnect, 
						p->my_evSendTimeout,
						p->my_evSendEnd,
						p->gque_Deli.g_evSem
						};
	enum {	EV_STOP = WAIT_OBJECT_0,		// �X���b�h�I��
			EV_RETRY,						// �Đڑ��v���^�C�}�C�x���g
			EV_CONNECT,						// �ڑ��C�x���g
			EV_DISCONNECT,					// �ؒf�C�x���g
			EV_SENDTIMEOUT,					// ���M�^�C���A�E�g �C�x���g
			EV_SENDEND,						// ���M�����C�x���g
			EV_QUE,							// ���M�˗�
			EV_END	};

	// ���b�Z�[�W�L���[�ǂݍ���
	pLOG(em_MSG), "[KizuToSend] <%s> �X���b�h�J�n [0x%X]", p->my_cSession, p->GetTid());
	while (1) {
		int wait_num = (p->my_emSockState != STATE_NONE && p->my_emSockState != STATE_IDLE ) ? EV_END-1 : EV_END;
		DWORD ret = WaitForMultipleObjects(wait_num, hArray, FALSE, INFINITE);

		switch (ret) {

//-----------------------------------------------------------------------------------------------
		case EV_STOP:												// �X���b�h�I��
			pLOG(em_MSG), "[KizuToSend] <%s> �X���b�h�I�� [0x%X]", p->my_cSession, p->GetTid());
			// �\�P�b�g�j��
			SAFE_DELETE(p->mcls_pSock);
			return 0;

//-----------------------------------------------------------------------------------------------
		case EV_RETRY:												// �Đڑ��v���^�C�}�C�x���g
			p->Connect(false);											// ���g���C�ڑ�
			break;

//-----------------------------------------------------------------------------------------------
		case EV_CONNECT:											// �ڑ��v���C�x���g
			pLOG(em_MSG), "[KizuToSend] <%s> ����ڑ��v��", p->my_cSession);
			p->Connect(true);											// �����ڑ�
			break;

//-----------------------------------------------------------------------------------------------
		case EV_DISCONNECT:											// �ؒf�C�x���g
			pLOG(em_WAR), "[KizuToSend] ����ؒf�ʒm");
			p->Close();													// �ؒf
			break;

//-----------------------------------------------------------------------------------------------
		case EV_SENDTIMEOUT:										// ���M�^�C���A�E�g �C�x���g
			pLOG(em_WAR), "[KizuToSend] ���M�^�C���A�E�g");
			syslog(SYSNO_SOCK_SEND_TIMEOUT,"", p->my_cSession);
			p->Close();
			break;

//-----------------------------------------------------------------------------------------------
		case EV_SENDEND:											// ���M�����C�x���g
			if(NULL == p->mcls_pSock) {										// �ؒf�ς�
				pLOG(em_WAR), "[KizuToSend] �ؒf�ςݏ�ԂŁA���M�����C�x���g");
				break;	
			}

			switch( p->my_emSockState ) {
			case KizuToSend::STATE_NONE:									// �C�j�V�����`�����M����
				pLOG(em_INF), "[KizuToSend] �C�j�V�����`�����M����");

				//�o�N�C�� 20080514  (���ꂾ�ƁA�����ō�����f�[�^�Ȃ̂ɁA���M�����f�[�^�󂯓n���L���[�ɃL���[�C���O����Ă��܂�) --->>>
				//p->SendEnd(); 
				SAFE_DELETE(p->my_pHead);
				p->my_emSockState = STATE_IDLE;		// �X�e�[�^�X�ύX
				// <<<----
				break;

			case KizuToSend::STATE_HEAD:									// �w�b�_�[�����M����
				// ���̓`���́A�w���V�[�H
				if( 0 == memcmp( p->my_pHead->base_head.code, T_CODE_HELCY, SIZE_SOCK_TC) ) {
					delete p->my_pHead;
					p->my_pHead = NULL;
					p->my_emSockState = STATE_IDLE;		// �X�e�[�^�X�ύX
					break;
				}

				// ���̃t���[���̓w�b�_�[���̂݁H
				if( sizeof(SOCK_F_HEAD) == p->my_pHead->base_head.len ) {
					p->SendEnd();
				} else {
					// �����ăf�[�^�����M
					p->SendData();
				}
				break;

			case KizuToSend::STATE_DATA:									// �f�[�^�����M����
				// �܂��������M���H
				if(p->my_pHead->base_head.len > p->my_nSendingDataSize+SIZE_SOCK_HEAD) {
					
					// �c��̕����𑗐M
					p->SendData();
				} else {
					// ��M����
					p->SendEnd();
				}
				break;
			}
			break;

//-----------------------------------------------------------------------------------------------
		case EV_QUE:												// ���M�˗�
			p->SendStart();
			break;

//-----------------------------------------------------------------------------------------------
		default :
			// ���肦�Ȃ��I�I
			break;
		}
	}
	return 0;
}


// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// �V�[�P���X����


//------------------------------------------
// �w�b�_�[�����M�J�n
//------------------------------------------
void KizuToSend::SendHeader()
{
	// �\�P�b�g��ԕύX
	my_emSockState = STATE_HEAD;
	
	// ���M
	int iRet = mcls_pSock->Send(0, my_pHead, sizeof(SOCK_F_HEAD), my_evSendEnd);
	if( iRet ) {					// �\�P�b�g���M�G���[
		LOG(em_ERR), "[KizuToSend] �r���`�� �w�b�_�� ���M���s");
		syslog(SYSNO_SOCK_SEND_ERR, "[KizuToSend err_code = %d] (�r���`�� �w�b�_�[��)", iRet);

		// �\�P�b�g�ؒf
		Close();
	}
}

//------------------------------------------
// �f�[�^�����M�J�n
//------------------------------------------
void KizuToSend::SendData()
{
	// �f�[�^�����z���g�ɂ��邩�m�F
	if(my_pHead->base_head.addr[0] == NULL) {
		//LOG(em_ERR), "[KizuToSend] �r���`�� �f�[�^�� �A�h���X����");
		SendEnd();
		return;
	}

	// ���M�T�C�Y�w��
	DWORD size = my_pHead->base_head.len - SIZE_SOCK_HEAD - my_nSendingDataSize;		// ���M�T�C�Y
	if(size > SIZE_SOCK_DATA_MAX) size = SIZE_SOCK_DATA_MAX;							// 1��̑��M�T�C�Y�I�[�o�[

//LOG(em_ERR), "[KizuToSend] �f�[�^�����M [%dbyte��, %X]", my_nSendingDataSize, ((BYTE*)my_pHead->base_head.addr[0])[my_nSendingDataSize]);

	// ���M
	int iRet = mcls_pSock->Send(0, &((BYTE*)my_pHead->base_head.addr[0])[my_nSendingDataSize], size, my_evSendEnd);
	if( iRet ) {					// �\�P�b�g���M�G���[
		LOG(em_ERR), "[KizuToSend] �r���`�� �f�[�^�� ���M���s [size=%d, total=%d]", size, my_nSendingDataSize);
		syslog(SYSNO_SOCK_SEND_ERR, "[KizuToSend err_code = %d] (�r���`�� �f�[�^�� size=%d total=%d)", iRet, size, my_nSendingDataSize);

		// �\�P�b�g�ؒf
		Close();
	} else {
		// �\�P�b�g��ԕύX
		my_emSockState = STATE_DATA;
		my_nSendingDataSize += size;
	}
}

//------------------------------------------
// �r���`�����M �J�n����
//------------------------------------------
void KizuToSend::SendStart()
{
	// �o�b�t�@�ɃL���[�����łɂ���H
	if(my_pHead != NULL) {
		LOG(em_ERR), "[KizuToSend] �o�b�t�@�ɃL���[�����łɗL��");
	}

	// �L���[���o��
	my_pHead = gque_Deli.GetFromHead();
	my_nSendingDataSize = 0;

	// �\�P�b�g���ڑ����́A�C���X�^���X�J��
	if( STATE_NONE == my_emSockState ) {
		// �փ��V�[�H
		if( 0 == memcmp( my_pHead->base_head.code, T_CODE_HELCY, SIZE_SOCK_TC) ) {
			delete my_pHead;
			my_pHead = NULL;
			return;
		}
		
		QueBufSendBack();
		return;
	}

	// �w�b�_�[���M�J�n
	SendHeader();
}

//------------------------------------------
// �r���`�����M ��������
//------------------------------------------
void KizuToSend::SendEnd()
{
	if( my_pHead == NULL ) return;
	// ���o�b�t�@�̃C���X�^���X��ԋp
	QueBufSendBack();						
	my_emSockState = STATE_IDLE;		// �X�e�[�^�X�ύX
}


// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// TPC/IP �ʐM����

//------------------------------------------
// �ڑ�
// bool bLogDsp ���O�o�͗L�� (false:���s���O�o���Ȃ�)
//------------------------------------------
void KizuToSend::Connect(bool bLogDsp)
{
	int retc;

	//// �����M�o�b�t�@��ԋp (�O�̈�)
	QueAllSendBack();

	// �\�P�b�g�C���X�^���X����
	try {
		mcls_pSock = new SockBase( my_cSession, TASKINI_NAME, GetTid(), PARAM_SOCK_TIMEOUT, 1, 0, PARAM_SOCK_SEND_BUFSIZE);
	} catch(int code) {
		mcls_pSock = NULL;
		if(bLogDsp) {
			LOG(em_ERR), "[KizuToSend] <%s> �\�P�b�g New���s err_code=%d", my_cSession, code);
			syslog(SYSNO_SOCK_NEW_ERR, "[%s: err_code=%d]", my_cSession, code);
		}
	}

	// �R�l�N�g
	if( NULL != mcls_pSock ) {
		retc = mcls_pSock->Connect();
		if( 0 != retc ) {
			delete mcls_pSock;
			mcls_pSock = NULL;
			if(bLogDsp) {
				LOG(em_ERR), "[KizuToSend] <%s> �\�P�b�g Connect���s err_code=%d", my_cSession, retc);
				syslog(SYSNO_SOCK_CONNECT_ERR, "[%s: err_code=%d]", my_cSession, retc);
			}
		}
	}

	// �㏈��
	if( NULL != mcls_pSock ) {								// ����
		mcls_pSock->SetEvSendTimeout(my_evSendTimeout);			// ���M�^�C���A�E�g���C�x���g�ɂ���
		mcls_pSock->SetEvDisConnect(my_evDisConnect);			// �ؒf�F�����C�x���g�ɂ���

		// �C�j�V�����`������
		my_pHead = new SOCK_F_HEAD;
		memset(my_pHead, 0x00, sizeof(SOCK_F_HEAD));
		memcpy(my_pHead->base_head.code, T_CODE_NAME, sizeof(my_pHead->base_head.code));	// �g�����U�N�V�����R�[�h
		memcpy(my_pHead->base_head.pname, my_cPCname, sizeof(my_cPCname));					// PC����
		my_pHead->base_head.subinf = my_nSendNum;											// �_�������
		my_pHead->base_head.len = sizeof(SOCK_F_HEAD);										// �`����
		my_pHead->base_head.addr[0] = NULL;													// �f�[�^���̃A�h���X
		my_pHead->base_head.addr[1] = NULL;													// ���g�p


		// ���M
		int iRet = mcls_pSock->Send(0, my_pHead, my_pHead->base_head.len, my_evSendEnd );
		if(iRet) {				// ���M�ُ�
			LOG(em_ERR), "[KizuToSend] �C�j�V�����`�����M���s, err_code=%d", iRet);
			syslog(SYSNO_SOCK_SEND_ERR, "[err_code = %d] (�C�j�V�����`��)", iRet);
			
			// �\�P�b�g�C���X�^���X�j��
			SAFE_DELETE(mcls_pSock);

			// �ؒf
			Close();												// �ؒf & ���g���C�ݒ�
		} else {				// ���M����
			// �A�v�����x�� �\�P�b�g�ڑ�����
			LOG(em_MSG), "[KizuToSend] ����������� �R�l�N�V�����m��");
		}

	} else {												// �\�P�b�g�������s
		Close();												// �ؒf & ���g���C�ݒ�
	}
}

//------------------------------------------
// �ؒf & ���g���C�ݒ�
//------------------------------------------
void KizuToSend::Close()
{
	// �\�P�b�g�ؒf
	if(NULL != mcls_pSock) {
		SAFE_DELETE(mcls_pSock); 
		LOG(em_WAR), "[KizuToSend] <%s> �\�P�b�g �ؒf", my_cSession);
		syslog(SYSNO_SOCK_CLOSE, "[%s]", my_cSession);
	}

	// ������
	my_emSockState = STATE_NONE;					// �\�P�b�g��� ������
	QueAllSendBack();								// �����M�o�b�t�@��ԋp
	
	// �O���� �ؒf��ʒm
	SetEvent(my_evAnserClose); 

	// ���g���C (�L�����̂�)
	if(my_bRetryMode) {
		LARGE_INTEGER ts;
		ts.QuadPart = (__int64)PARAM_SOCK_RETRY_TIME * -10000;	// 1[ms]��100[ns] (�}�C�i�X�w��ŁA�w�莞�Ԍo�߂ŃV�O�i��) (��3������2��ڈȍ~�̋N���Ԋu)
		SetWaitableTimer(my_evReTryConnect, &ts, 0, NULL, NULL, FALSE);
	}
}
