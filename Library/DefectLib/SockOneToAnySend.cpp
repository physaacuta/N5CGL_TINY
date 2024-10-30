#include "StdAfx.h"
#include "SockOneToAnySend.h"

//------------------------------------------
// �R���X�g���N�^
//------------------------------------------
SockOneToAnySend::SockOneToAnySend() :
ThreadManager("SockSend"),
my_cSession(""),
my_bRetryMode(false),

my_emSockState(STATE_NONE),
my_pHead(NULL),
my_nSendingDataSize(0),
my_emSendAddId(STATE_SUB_NONE),
my_bHelcyCancel(false),

mcls_pSock(NULL),
mque_pDeli(NULL)
{
	// �V�O�i������
	my_tmReTryConnect	= CreateWaitableTimer(NULL, FALSE, NULL);
	my_tmHelcy			= CreateWaitableTimer(NULL, FALSE, NULL);

	my_evConnect		= CreateEvent(NULL, FALSE, FALSE, NULL);
	my_evDisConnect		= CreateEvent(NULL, FALSE, FALSE, NULL);
	my_evSendTimeout	= CreateEvent(NULL, FALSE, FALSE, NULL);
	my_evSendEnd		= CreateEvent(NULL, FALSE, FALSE, NULL);
	my_evAnserClose		= CreateEvent(NULL, FALSE, FALSE, NULL);
	my_evAnserConnect	= CreateEvent(NULL, FALSE, FALSE, NULL);

	InitializeCriticalSection(&m_csLock);
}

//------------------------------------------
// �f�X�g���N�^
//------------------------------------------
SockOneToAnySend::~SockOneToAnySend(void)
{
	// �\�P�b�g�j��
	SAFE_DELETE(mcls_pSock);

	// �V�O�i���J��
	CloseHandle(my_tmReTryConnect);
	CloseHandle(my_tmHelcy);

	CloseHandle(my_evConnect);
	CloseHandle(my_evDisConnect);
	CloseHandle(my_evSendTimeout);
	CloseHandle(my_evSendEnd);
	CloseHandle(my_evAnserClose);
	CloseHandle(my_evAnserConnect);

	DeleteCriticalSection(&m_csLock);

}

//------------------------------------------
// �w��̃X�e�[�^�X����
// SOCK_STATE st �X�e�[�^�X
//------------------------------------------
CString SockOneToAnySend::GetSockStateName(SOCK_STATE st) const
{
	CString wk;
	if		(st == STATE_NONE)		{ wk = "������� (�ؒf��)"; }
	else if (st == STATE_IDLE)		{ wk = "�������� (�ڑ���)"; }
	else if (st == STATE_HEAD)		{ wk = "�w�b�_�� ���M��";}
	else if (st == STATE_DATA)		{ wk = "�f�[�^�� ���M��";}
	else							{ wk = "�X�e�[�^�X�ُ�";}
	return wk;
}

//------------------------------------------
// ���o�b�t�@������L���[�ɕԋp����
//------------------------------------------
void SockOneToAnySend::QueBufSendBack()
{
	if(NULL == my_pHead) return;
	mque_pDeli->AddFree(my_pHead);
	my_pHead = NULL;
}

//------------------------------------------
// ���ׂẴL���[��ԋp����
//------------------------------------------
void SockOneToAnySend::QueAllSendBack()
{
	SOCK_F_HEAD2*		deli = NULL;

	// �f�o�b�N�p���O
	if(0 !=mque_pDeli->GetMaxCountPool() ){		// 1��������Ă��Ȃ����͏o���Ȃ�
		LOG(em_MSG), "[%s] �����M�L���[�J��! Que=%d, MAX=%d", my_sThreadName, mque_pDeli->GetCountPool(), mque_pDeli->GetMaxCountPool());
	}

	// �L���[�ɗ��܂��Ă���C���X�^���X��S���J��
	while(true) {
		DWORD ret = WaitForMultipleObjects(1, &mque_pDeli->g_evSemPool, FALSE, 0);
		if( WAIT_TIMEOUT==ret ) break;
		deli = mque_pDeli->GetPool();
		if (deli == NULL) break;


		// �ԋp
		mque_pDeli->AddFree(deli);
		deli = NULL;
	}
	// �ő匏�����Z�b�g
	mque_pDeli->ReSetMaxCountPool(); 
	// ���o���ς݂̃o�b�t�@������Έꏏ�ɊJ������
	QueBufSendBack();
}

//------------------------------------------
// �X���b�h�J�n�O���� (�X���b�h���s����(�V�O�i���Z�b�g�O)�ɃR�[���o�b�N)
//------------------------------------------
int SockOneToAnySend::ThreadFirst()
{
	mcls_Time.InitAve();

	//// �C�x���g�ݒ�
	HANDLE hArray[] = { my_tmReTryConnect,
						my_evConnect,
						my_evDisConnect, 
						my_evSendTimeout,
						my_evSendEnd,
						my_tmHelcy,
						mque_pDeli->g_evSemPool
						};
	SetEventNum(sizeof(hArray)/sizeof(HANDLE), hArray);
	return 0;
}

//------------------------------------------
// �X���b�h�I���O���� (�I���V�O�i����ɃR�[���o�b�N)
//------------------------------------------
int SockOneToAnySend::ThreadLast()
{
	return 0;
}

//------------------------------------------
// �X���b�h��WaitFor�҂��̒��O����  
// int nDefWaitNum ���̐�
// int* nNowWaitNum �ύX��̐�
//------------------------------------------
void SockOneToAnySend::ThreadWaitFor(int nDefWaitNum, int* nNowWaitNum)
{
	// ���s�����m�F
	bool bExec = (my_emSockState != STATE_NONE && my_emSockState != STATE_IDLE ) ? true : false;

	// �҂��s�񐔕ύX
	if( bExec )	*nNowWaitNum = nDefWaitNum-1;
	else		*nNowWaitNum = nDefWaitNum;
}

//------------------------------------------
// �X���b�h�C�x���g����
// int nEventNo �C�x���gNo (0�I���W��)
//------------------------------------------
void SockOneToAnySend::ThreadEvent(int nEventNo)
{	
	////// �V�O�i���̏���
	enum {	EV_RETRY,						// �Đڑ��v���^�C�}�C�x���g
			EV_CONNECT,						// �ڑ��C�x���g
			EV_DISCONNECT,					// �ؒf�C�x���g
			EV_SENDTIMEOUT,					// ���M�^�C���A�E�g �C�x���g
			EV_SENDEND,						// ���M�����C�x���g
			EV_HELCY,						// �w���V�[�^�C�}�[
			EV_QUE							// ���M�˗�
	};

	
// ��	LOG(em_MSG), "[%s] <%s> �C�x���g��M=%d", my_sThreadName, my_cSession, nEventNo);


	////// �V�O�i����������
	switch (nEventNo) {
//-----------------------------------------------------------------------------------------------
	case EV_RETRY:												// �Đڑ��v���^�C�}�C�x���g
		Connect(false);											// ���g���C�ڑ�
		return;

//-----------------------------------------------------------------------------------------------
	case EV_CONNECT:											// �ڑ��v���C�x���g
		LOG(em_MSG), "[%s] <%s> ����ڑ��v��", my_sThreadName, my_cSession);
		Connect(true);											// �����ڑ�
		return;

//-----------------------------------------------------------------------------------------------
	case EV_DISCONNECT:											// �ؒf�C�x���g
		LOG(em_WAR), "[%s] <%s> ����ؒf�ʒm", my_sThreadName, my_cSession);
		Close();													// �ؒf
		return;

//-----------------------------------------------------------------------------------------------
	case EV_SENDTIMEOUT:										// ���M�^�C���A�E�g �C�x���g
		LOG(em_WAR), "[%s] <%s> ���M�^�C���A�E�g", my_sThreadName, my_cSession);
		syslog(SYSNO_SOCK_SEND_TIMEOUT,"", my_cSession);
		Close();
		return;

//-----------------------------------------------------------------------------------------------
	case EV_SENDEND:											// ���M�����C�x���g
		SendData();
		return;

//-----------------------------------------------------------------------------------------------
	case EV_HELCY:											// �w���V�[���M�C�x���g
		SendHelcy();
		return;

//-----------------------------------------------------------------------------------------------
	case EV_QUE:												// ���M�˗�
		SendStart();
		return;

//-----------------------------------------------------------------------------------------------
	default :
		// ���肦�Ȃ��I�I
		LOG(em_ERR), "[%s] ThreadEvent EventNo NG=%d", my_sThreadName, nEventNo );
		return;
	}
}

// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// �V�[�P���X����

//------------------------------------------
// �w�b�_�[�����M�J�n
//------------------------------------------
void SockOneToAnySend::SendHeader()
{
	if(NULL == my_pHead) {
		LOG(em_WAR), "[%s] <%s> �w�b�_�� ���M�o�b�t�@����", my_sThreadName, my_cSession);
		return ;
	}

	_ASSERT( my_pHead->base_head_addr.len_head <= SIZE_SOCK_DATA_MAX);
//LOG(em_INF), "[%s] Start size=%d sub=%d", my_sThreadName, my_pHead->base_head.len, my_pHead->base_head_addr.len_head );	// ��Ń`�F�b�N���Ă���͂��Ȃ̂�

	// �\�P�b�g��ԕύX
	my_emSockState = STATE_HEAD;
	my_emSendAddId = STATE_SUB_NONE;
	my_nSendingDataSize = 0;

	// ���M
	int iRet = mcls_pSock->Send(0, my_pHead, my_pHead->base_head_addr.len_head, my_evSendEnd);
	if( iRet ) {					// �\�P�b�g���M�G���[
		LOG(em_ERR), "[%s] <%s> �w�b�_�� ���M���s", my_sThreadName, my_cSession);
		syslog(SYSNO_SOCK_SEND_ERR, "[SockOneToAnySend err_code = %d] (�r���`�� �w�b�_�[��)", iRet);

		// �\�P�b�g�ؒf
		Close();
	}
}

//------------------------------------------
// �f�[�^�����M�J�n
//------------------------------------------
void SockOneToAnySend::SendData()
{
	//----------------
	// ���ǂ�H
	AutoLock lock(&m_csLock);
	if(NULL == my_pHead) {
		LOG(em_WAR), "[%s] <%s> �f�[�^�� ���M�o�b�t�@����", my_sThreadName, my_cSession);
		return ;
	}

	DWORD size = 0;
	long* p = NULL;
	while(1) {
		// 
		if( STATE_SUB_NONE >= my_emSendAddId && 0 != my_pHead->base_head_addr.len_addr[0]  ) {
			my_emSendAddId = STATE_SUB_ADDR0;
			size = my_pHead->base_head_addr.len_addr[0];
			p = my_pHead->base_head.addr[0]; 
			break;
		}
		//
		if( STATE_SUB_ADDR0 >= my_emSendAddId && 0 != my_pHead->base_head_addr.len_addr[1]  ) {
			my_emSendAddId = STATE_SUB_ADDR1;
			size = my_pHead->base_head_addr.len_addr[1];
			p = my_pHead->base_head.addr[1]; 
			break;
		}
		break;
	}

	// ���͂���H
	if( NULL == p ) {
		SendEnd();
		return ;
	}

	//----------------
	// ���M
//LOG(em_INF), "[%s] size=%d sub=%d QueFree:%d", my_sThreadName, size, my_emSendAddId, mque_pDeli->GetCountFree());	// ��Ń`�F�b�N���Ă���͂��Ȃ̂�

	// ���M
	int iRet = mcls_pSock->Send(0, p, size, my_evSendEnd);
	if( iRet ) {					// �\�P�b�g���M�G���[
		LOG(em_ERR), "[%s] <%s> �f�[�^�� ���M���s [size=%d, total=%d]", my_sThreadName, my_cSession, size, my_nSendingDataSize);
		syslog(SYSNO_SOCK_SEND_ERR, "[SockOneToAnySend err_code = %d] (�f�[�^�� size=%d total=%d)", iRet, size, my_nSendingDataSize);

		// �\�P�b�g�ؒf
		Close();
	} else {
		// �\�P�b�g��ԕύX
		my_emSockState = STATE_DATA;
		my_nSendingDataSize += size;
	}
}

//------------------------------------------
// �`�����M �J�n����
//------------------------------------------
void SockOneToAnySend::SendStart()
{
	mcls_Time.PutTimestamp(0);

	AutoLock lock(&m_csLock);
	// �o�b�t�@�ɃL���[�����łɂ���H
	if(my_pHead != NULL) {
		LOG(em_ERR), "[%s] <%s> �o�b�t�@�ɃL���[�����łɗL��", my_sThreadName, my_cSession);
		QueBufSendBack();
	}

	// �L���[���o��
	my_pHead = mque_pDeli->GetPool();

	// �\�P�b�g���ڑ����́A�C���X�^���X�J��
	if( STATE_NONE == my_emSockState ) {
		// ��		LOG(em_WAR), "[%s] <%s> �ؒf���̑��M�˗�", my_sThreadName, my_cSession);

		QueBufSendBack();
		return;
	}

	// �w�b�_�[���M�J�n
	SendHeader();
}

//------------------------------------------
// �r���`�����M ��������
//------------------------------------------
void SockOneToAnySend::SendEnd()
{
	AutoLock lock(&m_csLock);

	if( my_pHead == NULL ) return;

	// �w���V�[�ȊO�H
	if( memcmp(my_pHead->base_head.code, &T_CODE_HELCY, sizeof(my_pHead->base_head.code))) {
		my_bHelcyCancel = true;
	}

	// ���o�b�t�@�̃C���X�^���X��ԋp
	QueBufSendBack();						
	my_emSockState = STATE_IDLE;		// �X�e�[�^�X�ύX
	mcls_Time.PutTimestamp(1);

//LOG(em_INF), "[%s] ���M���� %.3f QueFree:%d", my_sThreadName, mcls_Time.GetTimestamp(1,0), mque_pDeli->GetCountFree());	// ��Ń`�F�b�N���Ă���͂��Ȃ̂�

}


//------------------------------------------
// �w���V�[�`�����M ����
//------------------------------------------
void SockOneToAnySend::SendHelcy()
{
	//// �O��w���V�[����̊ԂŁA���f�[�^�𑗂��Ă���̂ŁA�w���V�[�͕K�v�Ȃ�
	if(my_bHelcyCancel) {
		my_bHelcyCancel = false;
		return;
	}

	//// ���M
	SOCK_F_HEAD2* p = mque_pDeli->GetFree(0);	// �Z�}�t�H�[�҂��o�[�W����(�����Ɍ��ʂ�Ԃ�����)
	if( NULL == p ) {
		LOG(em_ERR), "[%s] <%s> �L���[�Ȃ�", my_sThreadName, my_cSession );
		return;
	}

	strcpy(p->base_head.pname, getmypcname());
	memcpy(p->base_head.code, &T_CODE_HELCY, sizeof(p->base_head.code));
	p->base_head.len		= sizeof(SOCK_F_HEAD);

	p->base_head_addr.len_head	= p->base_head.len;
	p->base_head_addr.len_addr[0]= 0;
	p->base_head_addr.len_addr[1]= 0;
	
	if( ! mque_pDeli->AddPool(p) ) {		// �o�^
		LOG(em_ERR), "[%s] <%s> �w���V�[�`�����M�˗��L���[�t��", my_sThreadName, my_cSession);
		syslog(SYSNO_QUEFULL_WAR, "[PC�� �w���V�[�`��]");
	}
}

// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// TPC/IP �ʐM����


//------------------------------------------
// �ڑ�
// bool bLogDsp ���O�o�͗L�� (false:���s���O�o���Ȃ�)
//------------------------------------------
void SockOneToAnySend::Connect(bool bLogDsp)
{
	int retc;

	//// �����M�o�b�t�@��ԋp (�O�̈�)
	QueAllSendBack();

	// �\�P�b�g�C���X�^���X����
	try {
		mcls_pSock = new SockBase( my_cSession, TASKINI_NAME, GetTid(), PARAM_SOCK_TIMEOUT, PARAM_SOCK_MAX, 0, PARAM_SOCK_SEND_BUFSIZE);
		mcls_pSock->SetKsThPriority( GetThreadPriority(my_hThread) );	// �������g�̃X���b�h�D��x�������p��
		//mcls_pSock = new SockBase( my_cSession, TASKINI_NAME, 0, PARAM_SOCK_TIMEOUT, PARAM_SOCK_MAX, 0, PARAM_SOCK_SEND_BUFSIZE);
	} catch(int code) {
		mcls_pSock = NULL;
		if(bLogDsp) {
			LOG(em_ERR), "[%s] <%s> �\�P�b�g New���s err_code=%d", my_sThreadName, my_cSession, code);
			syslog(SYSNO_SOCK_NEW_ERR, "[%s: err_code=%d]", my_cSession, code);
		}
		return ;
	}

	// �R�l�N�g
	if( NULL != mcls_pSock ) {
		retc = mcls_pSock->Connect();
		if( 0 != retc ) {
			delete mcls_pSock;
			mcls_pSock = NULL;
			if(bLogDsp) {
				LOG(em_ERR), "[%s] <%s> �\�P�b�g Connect���s err_code=%d", my_sThreadName, my_cSession, retc);
				syslog(SYSNO_SOCK_CONNECT_ERR, "[%s: err_code=%d]", my_cSession, retc);
			}
		}
	}

	// �㏈��
	if( NULL != mcls_pSock ) {								// ����
		mcls_pSock->SetEvSendTimeout(my_evSendTimeout);			// ���M�^�C���A�E�g���C�x���g�ɂ���
		mcls_pSock->SetEvDisConnect(my_evDisConnect);			// �ؒf�F�����C�x���g�ɂ���

		LOG(em_MSG), "[%s] <%s> �R�l�N�V�����m��", my_sThreadName, my_cSession);
		my_emSockState = STATE_IDLE;							// �X�e�[�^�X�ύX
		SetEvent(my_evAnserConnect);

		// �w���V�[�N��
		__int64 ts = PARAM_TIME_HELCY_SEND * -10000;	// 1�ʕb��100�i�m�b (�}�C�i�X�w��ŁA�w�莞�Ԍo�߂ŃV�O�i��) (��3������2��ڈȍ~�̋N���Ԋu)
		SetWaitableTimer(my_tmHelcy, (LARGE_INTEGER *)&ts, PARAM_TIME_HELCY_SEND, NULL, NULL, FALSE);
		my_bHelcyCancel = false;

	} else {												// �\�P�b�g�������s
		Close();												// �ؒf & ���g���C�ݒ�
	}
}

//------------------------------------------
// �ؒf & ���g���C�ݒ�
//------------------------------------------
void SockOneToAnySend::Close()
{
	// �\�P�b�g�ؒf
	if(NULL != mcls_pSock) {
		SAFE_DELETE(mcls_pSock); 
		LOG(em_WAR), "[%s] <%s> �\�P�b�g �ؒf", my_sThreadName, my_cSession);
		syslog(SYSNO_SOCK_CLOSE, "[%s]", my_cSession);
	}

	// ������
	AutoLock lock(&m_csLock);
	QueAllSendBack();								// �����M�o�b�t�@��ԋp
	my_emSockState = STATE_NONE;					// �\�P�b�g��� ������
	SetEvent(my_evAnserClose);						// �O���� �ؒf��ʒm

	// �w���V�[�`���^�C���[�Ď��I��
	CancelWaitableTimer(my_tmHelcy);

	// ���g���C (�L�����̂�)
	if(my_bRetryMode) {
		LARGE_INTEGER ts;
		ts.QuadPart = (__int64)PARAM_SOCK_RETRY_TIME * -10000;	// 1[ms]��100[ns] (�}�C�i�X�w��ŁA�w�莞�Ԍo�߂ŃV�O�i��) (��3������2��ڈȍ~�̋N���Ԋu)
		SetWaitableTimer(my_tmReTryConnect, &ts, 0, NULL, NULL, FALSE);
	}
}
