#include "StdAfx.h"
#include "SockOneToAnyRecv.h"

//------------------------------------------
// �R���X�g���N�^
//------------------------------------------
SockOneToAnyRecv::SockOneToAnyRecv(void) :
ThreadManager("SockRecv"),
my_cSession(""),
mcls_pSock(NULL),
mque_pDeli(NULL)
{
	//// ������
	for(int ii=0; ii<PARAM_SOCK_MAX; ii++) {
		my_emSockState[ii]	 = STATE_NONE;
		my_pHead[ii]		 = NULL; 
		my_nRecvingDataSize[ii] = 0;
		my_emRecvAddId[ii]	 = STATE_SUB_NONE;
		my_bHelcyOk[ii]		 = false;

		// �C�x���g
		my_evDisConnect[ii]	 = CreateEvent(NULL, FALSE, FALSE, NULL);	// �ؒf�C�x���g�F��(ID)
		my_evRecvTimeOut[ii] = CreateEvent(NULL, FALSE, FALSE, NULL);	// ��M�^�C���A�E�g�C�x���g�F��(ID)
		my_evRecv[ii]		 = CreateEvent(NULL, FALSE, FALSE, NULL);	// ��M�C�x���g�F��(ID)

	}

	// �C�x���g
	my_evCloseReq			= CreateEvent(NULL, FALSE, FALSE, NULL);	// ����ؒf�v��
	my_evAccept				= CreateEvent(NULL, FALSE, FALSE, NULL);
	my_tmHelcy				= CreateWaitableTimer(NULL, FALSE, NULL);

	//// ���Z�b�g(������)
	Reset();
}

//------------------------------------------
// �f�X�g���N�^
//------------------------------------------
SockOneToAnyRecv::~SockOneToAnyRecv(void)
{
	//// �\�P�b�g���
	if( NULL != mcls_pSock ) {
		delete mcls_pSock;
		mcls_pSock = NULL;
	}

	//// �C�x���g
	CloseHandle(my_evCloseReq);
	CloseHandle(my_evAccept);	// �e����PC���Accept��
	CloseHandle(my_tmHelcy);
}

// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// TPC/IP �ʐM����

//------------------------------------------
// ���Z�b�g
//------------------------------------------
void SockOneToAnyRecv::Reset()
{
	//// ���݂̓Ǎ��L�����Z��
	if(mcls_pSock) mcls_pSock->Cancel();

	//// ������
	memset( my_RecvWkHead, 0x00, sizeof(my_RecvWkHead));

	//// ���łɃL���[����������Ă�����ԋp
	for(int ii=0; ii<PARAM_SOCK_MAX; ii++) {
		if( NULL != my_pHead[ii] ) {
			mque_pDeli->AddFree(my_pHead[ii]);
			LOG(em_INF), "[%s] <%s> �L���[�����ԋp", my_sThreadName, my_cSession);
			my_pHead[ii] = NULL;
		}
	}

}

//------------------------------------------
// Listen
// bool bLogDsp ���O�o�͗L�� (false:���s���O�o���Ȃ�)
//------------------------------------------
BOOL SockOneToAnyRecv::Listen(bool bLogDsp)
{
	int retc;
	// �\�P�b�g�C���X�^���X����
	try {
		mcls_pSock = new SockBase(my_cSession, TASKINI_NAME, GetTid(), PARAM_SOCK_TIMEOUT, PARAM_SOCK_MAX, PARAM_SOCK_RECV_BUFSIZE);
		//mcls_pSock->SetOpt_NewAcceptNgMode(1);		// �A�N�Z�v�g�ُ펞�̋�����ݒ�B�n�[�t�R�l�N�V�������̕ێ�IP�ƐV�KIP�����ꎞ�ɂ́A�V�K�Z�b�V�����Ƃ��Ď󂯕t����
		mcls_pSock->SetOpt_NewAcceptNgMode(0);
		mcls_pSock->SetKsThPriority( GetThreadPriority(my_hThread) );	// �������g�̃X���b�h�D��x�������p��

	} catch(int code) {
		mcls_pSock = NULL;
		if(bLogDsp) {
			LOG(em_ERR), "[%s] <%s> �\�P�b�g New���s err_code=%d", my_sThreadName, my_cSession, code);
			syslog(SYSNO_SOCK_NEW_ERR, "[%s: err_code=%d]", my_cSession, code);
		}
		return FALSE;
	}

	// Listen
	if( NULL != mcls_pSock ) {
		mcls_pSock->SetEvAccept(my_evAccept);			// Accept�F�����C�x���g�ɂ��� (�T�[�o�[�̂�)

		retc = mcls_pSock->Listen();
		if( 0 != retc ) {
			delete mcls_pSock;
			mcls_pSock = NULL;
			if(bLogDsp) {
				LOG(em_ERR), "[%s] <%s> �\�P�b�g Listen���s err_code=%d", my_sThreadName, my_cSession, retc);
				syslog(SYSNO_SOCK_LISTEN_ERR, "[%s: err_code=%d]", my_cSession, retc);
			}
			return FALSE;
		}
	}

	LOG(em_MSG), "[%s] <%s> �\�P�b�g Listen�J�n", my_sThreadName, my_cSession);
	return TRUE;
}

//------------------------------------------
// Accept (�V�O�i���ʒm�p)
// �߂�l -1:NG  0�`�F����B�������ID
//------------------------------------------
int SockOneToAnyRecv::Accept()
{
	_ASSERT(NULL != mcls_pSock);

	int id = mcls_pSock->Accept();
	if( id < 0 ) {
		LOG(em_ERR), "[%s] <%s> �N���C�A���g���I�[�o�[", my_sThreadName, my_cSession);
		syslog(SYSNO_SOCK_CLIENT_MAX, "[%s]", my_cSession);
		return -1;
	}
	// ���q�������\�P�b�g�̏�Ԃ�ύX
	mcls_pSock->SetEvDisConnect(id, my_evDisConnect[id]);					// �ؒf�F�����C�x���g�ɂ���
	mcls_pSock->SetEvRecvTimeout(id, my_evRecvTimeOut[id]);					// ��M�^�C���A�E�g�F�����C�x���g�ɂ���

	// �X�e�[�^�X�ύX
	my_emSockState[id] = STATE_IDLE;
	my_bHelcyOk[id]	= true;					// �܂��͂P��ڂƂ��Đڑ����ꂽ�̂ŁA����F���ɂ��Ă���
	return id;
}

//------------------------------------------
// �N���C�A���g�ؒf
// int id �������ID
//------------------------------------------
void SockOneToAnyRecv::Close(int id)
{
	if(STATE_NONE == my_emSockState[id]) {
		LOG(em_MSG), "[%s] <%s> (%d) �ؒf", my_sThreadName, my_cSession, id);
	}
	mcls_pSock->CloseID(id);
	my_emSockState[id] = STATE_NONE;
}

//------------------------------------------
// �S�N���C�A���g�ؒf & �ُ�ʒm
// bool bErr �ُ�ʒm (false:�ُ�ؒf true:����ؒf)
//------------------------------------------
void SockOneToAnyRecv::CloseAll(bool bErr)
{
	if(NULL == mcls_pSock) return;
	// �ؒf
	mcls_pSock->CloseAll();
	// ���Z�b�g
	Reset();
	// �ُ�ʒm
	if(! bErr) ErrCall();
}

//------------------------------------------
// �ُ�ʒm�̂� (����ؒf�͂��Ȃ�)
// ���̊֐����R�[�������̂́A�ُ�̎������̂͂�
//------------------------------------------
void SockOneToAnyRecv::ErrCall()
{
	if(NULL == mcls_pSock) return;

	// ���Z�b�g
	Reset();
}

//------------------------------------------
// �Ɩ��`���̃w�b�_�[��M���J�n
// int id �������ID
//------------------------------------------
BOOL SockOneToAnyRecv::RecvHead(int id)
{
	my_nRecvingDataSize[id] = 0;
	my_emRecvAddId[id] = STATE_SUB_NONE;

	// �w�b�_�[��M�J�n
	int iRet = mcls_pSock->Recv(id, &my_RecvWkHead[id], SIZE_SOCK_HEAD, my_evRecv[id]);
	if( 0 != iRet ) {
		LOG(em_ERR), "[%s] <%s> (%d)�w�b�_�[��M�G���[ [err_code=%d]", my_sThreadName, my_cSession, id, iRet);
		syslog(SYSNO_SOCK_RECV_ERR, "[<%s> (HEAD) id=%d err_code=%d]", my_cSession, id, iRet);
		return FALSE;
	} else {
		// �X�e�[�^�X�ύX
		my_emSockState[id] = STATE_HEAD;
		my_nRecvingDataSize[id] = SIZE_SOCK_HEAD;
		return TRUE;
	}
}

//------------------------------------------
// �Ɩ��`���̃f�[�^����M���J�n
// int id �������ID
//------------------------------------------
BOOL SockOneToAnyRecv::RecvData(int id)
{
	// ��M�T�C�Y����
	DWORD size = 0;
	//long* p = NULL;
	BYTE* p = NULL;
	while(1) {
		// �w�b�_�[���̑����Ńf�[�^���L��
		if( STATE_SUB_NONE >= my_emRecvAddId[id] && SIZE_SOCK_HEAD < my_pHead[id]->base_head_addr.len_head ) {
			my_emRecvAddId[id] = STATE_SUB_HEAD_EX;
			size = my_pHead[id]->base_head_addr.len_head - SIZE_SOCK_HEAD;
			//p = &((long*)my_pHead[id])[ SIZE_SOCK_HEAD ];
			p = &((BYTE*)my_pHead[id])[ SIZE_SOCK_HEAD ];
			break;
		}
		// 
		if( STATE_SUB_HEAD_EX >= my_emRecvAddId[id] && 0 != my_pHead[id]->base_head_addr.len_addr[0]  ) {
			my_emRecvAddId[id] = STATE_SUB_ADDR0;
			size = my_pHead[id]->base_head_addr.len_addr[0];
			p = (BYTE*)my_pHead[id]->base_head.addr[0]; 
			break;
		}
		//
		if( STATE_SUB_ADDR0 >= my_emRecvAddId[id] && 0 != my_pHead[id]->base_head_addr.len_addr[1]  ) {
			my_emRecvAddId[id] = STATE_SUB_ADDR1;
			size = my_pHead[id]->base_head_addr.len_addr[1];
			p = (BYTE*)my_pHead[id]->base_head.addr[1]; 
			break;
		}
		break;
	}


	//�� LOG(em_MSG), "[%s] <%s> (%d) �f�[�^����M�J�n [size=%d, sub=%d]", my_sThreadName, my_cSession, id, size, my_emRecvAddId[id]);


	// �f�[�^����M�J�n
	int iRet = mcls_pSock->Recv(id, p, size, my_evRecv[id]);
	if( 0 != iRet ) {
		LOG(em_ERR), "[%s] <%s> (%d) �f�[�^����M�G���[ [size=%d, err_code=%d]", my_sThreadName, my_cSession, id, size, iRet);
		syslog(SYSNO_SOCK_RECV_ERR, "[(DATA) <%s> id=%d size=%d err_code=%d]", my_cSession, id, size, iRet);
		return FALSE;
	} else {
		// �X�e�[�^�X�ύX
		my_emSockState[id] = STATE_DATA;
		my_nRecvingDataSize[id]	+= size;		// ��M�T�C�Y

		return TRUE;
	}
}

//------------------------------------------
// ��M�f�[�^�̃`�F�b�N
// int id �������ID
// �߂�l -1:�ؒf  0:�������Ȃ�
//------------------------------------------
BOOL SockOneToAnyRecv::RecvCheck(int id)
{
	if( my_emSockState[id] != STATE_HEAD && STATE_DATA != my_emSockState[id]) {
		LOG(em_MSG), "[%s] <%s> (%d) ��M�����C�x���g���\�P�b�g���[�h�ُ� [%d]", my_sThreadName, my_cSession, id, my_emSockState[id]);
		return FALSE;
	}
	my_bHelcyOk[id] = true;		// ���������M�ł����B


	//----------------
	// �w���V�[�H
	if( my_emSockState[id] == STATE_HEAD ) {

		if( 0 == memcmp( my_RecvWkHead[id].base_head.code, T_CODE_HELCY, SIZE_SOCK_TC ) ) {	
			// �w���V�[�`����M
			Recv_Helcy();

			// ���`����M
			if( ! RecvHead(id) ) return FALSE;
			return TRUE;
		}

		// �w�b�_�[���݂̂̓`��
		if( STATE_HEAD == my_emSockState[id] && SIZE_SOCK_HEAD == my_RecvWkHead[id].base_head.len ) {
			if( ! GetFreeQue(id) ) return FALSE;
			// �L���[�o�^
			AddPoolQue(id);


			// ���`����M
			if( ! RecvHead(id) ) return FALSE;
			return TRUE;
		}
	}


	//----------------
	// 1�`���I���
		// �f�[�^���捞����
	if( STATE_DATA == my_emSockState[id] && my_nRecvingDataSize[id]  == my_RecvWkHead[id].base_head.len ) {

//��		LOG(em_MSG), "[%s] <%s> (%d) �f�[�^����M���� [size=%d, sub=%d]", my_sThreadName, my_cSession, id, my_RecvWkHead[id].base_head.len, my_emRecvAddId[id]);
		mcls_Time.PutTimestamp(1);
		//LOG(em_INF), "[%s] <%s> (%d) ��M���� %.3f", my_sThreadName, my_cSession, id, mcls_Time.GetTimestamp(1, 0));

		// �L���[�o�^
		AddPoolQue(id);

		// ���`����M
		if( ! RecvHead(id) ) return FALSE;
		return TRUE;
	}

	//----------------
	// �f�[�^���Ǎ���
	if( STATE_DATA == my_emSockState[id] ) {
		mcls_Time.PutTimestamp(0);
		if( ! RecvData(id)) return FALSE;
		return TRUE;
	}


	//----------------
	// �����܂ŗ�����f�[�^���Ǎ��J�n
	if( ! GetFreeQue(id) ) return FALSE;
	if( ! RecvData(id)) return FALSE;
	return TRUE;
}


//------------------------------------------
// �V�K�L���[�擾
// int id �������ID
//------------------------------------------
bool SockOneToAnyRecv::GetFreeQue(int id)
{
	// �󂫃L���[���o��
	my_pHead[id] = mque_pDeli->GetFree(0);
	if( NULL == my_pHead[id] ) {
		LOG(em_ERR), "[%s] <%s> (%d) �󂫃L���[�����I�`������", my_sThreadName, my_cSession, id);
		return FALSE;
	}

	// �|�C���^�����͌���ێ�(�d�v)
	long*			addr[2] ={ my_pHead[id]->base_head.addr[0], my_pHead[id]->base_head.addr[1] };

	// �w�b�_�[���̓R�s�[
	memcpy( my_pHead[id], &my_RecvWkHead[id], SIZE_SOCK_HEAD);

	// �|�C���^�̖߂�
	my_pHead[id]->base_head.addr[0] = addr[0];
	my_pHead[id]->base_head.addr[1] = addr[1];
	return TRUE;
}

//------------------------------------------
// ��M�L���[�̓o�^
// int id �������ID
//------------------------------------------
void SockOneToAnyRecv::AddPoolQue(int id)
{
	// �L���[�o�^
	mque_pDeli->AddPool(my_pHead[id]);
//��	mque_pDeli->AddFree(my_pHead[id]);

	my_pHead[id] = NULL;
	return;
}

//------------------------------------------
// �w���V�[�`����M���� (�w�b�_�[���̂�)
//------------------------------------------
void SockOneToAnyRecv::Recv_Helcy()
{
	// ������ς�������s�v
	//// ��x�L�����Z��
	//CancelWaitableTimer(my_evHelcy);

	//// �w���V�[�`���^�C���[�Ď��J�n
	//__int64 ts = (__int64)PARAM_TIMEOUT_HELCY_RECV * -10000;	// 1�ʕb��100�i�m�b (�}�C�i�X�w��ŁA�w�莞�Ԍo�߂ŃV�O�i��) (��3������2��ڈȍ~�̋N���Ԋu)
	//SetWaitableTimer(my_evHelcy, (LARGE_INTEGER *)&ts, 0, NULL, NULL, FALSE);
}


// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// �X���b�h����

//------------------------------------------
// �X���b�h�J�n�O���� (�X���b�h���s����(�V�O�i���Z�b�g�O)�ɃR�[���o�b�N)
//------------------------------------------
int SockOneToAnyRecv::ThreadFirst()
{
	mcls_Time.InitAve();

	// �w���V�[�N��
	__int64 ts = (__int64)PARAM_TIMEOUT_HELCY_RECV * -10000;	// 1�ʕb��100�i�m�b (�}�C�i�X�w��ŁA�w�莞�Ԍo�߂ŃV�O�i��) (��3������2��ڈȍ~�̋N���Ԋu)
	SetWaitableTimer(my_tmHelcy, (LARGE_INTEGER *)&ts, PARAM_TIMEOUT_HELCY_RECV, NULL, NULL, FALSE);


	//// �C�x���g�ݒ�
	AddEventNum(PARAM_SOCK_MAX, my_evRecv);
	AddEventNum(PARAM_SOCK_MAX, my_evDisConnect);
	AddEventNum(PARAM_SOCK_MAX, my_evRecvTimeOut);
	AddEventNum(1, &my_evCloseReq);
	AddEventNum(1, &my_evAccept);
	AddEventNum(1, &my_tmHelcy);

	while( TRUE ) {
		if( Listen(true) ) break;
		Sleep(2000);
	}

	return 0;
}

//------------------------------------------
// �X���b�h�I���O���� (�I���V�O�i����ɃR�[���o�b�N)
//------------------------------------------
int SockOneToAnyRecv::ThreadLast()
{
	// �w���V�[�`���^�C���[�Ď��I��
	CancelWaitableTimer(my_tmHelcy);
	return 0;
}

//------------------------------------------
// �X���b�h�C�x���g����
// int nEventNo �C�x���gNo (0�I���W��)
//------------------------------------------
void SockOneToAnyRecv::ThreadEvent(int nEventNo)
{
	int id = 0;										// �������ID (0�I���W��)

	////// �V�O�i���̏���
	enum {	EV_RECV				= 0,								// ��M�����ʒm			(PARAM_SOCK_MAX��)
			EV_DISCONNECT		= EV_RECV+PARAM_SOCK_MAX,			// �ؒf�ʒm				(PARAM_SOCK_MAX��)
			EV_RECVTIMEOUT		= EV_DISCONNECT+PARAM_SOCK_MAX,		// ��M�^�C���A�E�g		(PARAM_SOCK_MAX��)
			EV_CLOSEREQ			= EV_RECVTIMEOUT+PARAM_SOCK_MAX,	// ����ؒf�v��
			EV_ACCEPT,												// �A�N�Z�v�g�ʒm
			EV_HELCY,												// �w���V�[�^�C�}�[
			EV_LAST
	};
	

	////// �V�O�i����������
	//======================================================
	// ��M�����ʒm (PARAM_SOCK_MAX��)
	if(nEventNo >= EV_RECV && nEventNo < EV_RECV+PARAM_SOCK_MAX) {
		id = nEventNo - EV_RECV;
	
		if( ! RecvCheck(id) ) {	
			Close(id);
		}

	//======================================================
	// �ؒf�ʒm (PARAM_SOCK_MAX��)
	} else if(nEventNo >= EV_DISCONNECT && nEventNo < EV_DISCONNECT+PARAM_SOCK_MAX) {
		id = nEventNo - EV_DISCONNECT;
		Close(id);
		LOG(em_MSG), "[%s] <%s> (%d) ����ؒf", my_sThreadName, my_cSession, id );

	//======================================================
	// ��M�^�C���A�E�g (PARAM_SOCK_MAX��)
	} else if(nEventNo >= EV_RECVTIMEOUT && nEventNo < EV_RECVTIMEOUT+PARAM_SOCK_MAX) {

	//======================================================
	// ����ؒf�v�� (PARAM_SOCK_MAX��)
	} else if(nEventNo == EV_CLOSEREQ) {
		CloseAll(true);
		LOG(em_MSG), "[%s] <%s> (%d) �S����ؒf", my_sThreadName, my_cSession, id );

	//======================================================
	// �A�N�Z�v�g�ʒm
	} else if(nEventNo == EV_ACCEPT)	{
		int id = Accept();
		if( 0 > id ) {
			LOG(em_ERR), "[%s] <%s> (%d) �N���C�A���g���I�[�o�[", my_sThreadName, my_cSession, id);
			return;
		}
		LOG(em_MSG), "[%s] <%s> (%d) �A�N�Z�v�g �V�O�i��", my_sThreadName, my_cSession, id);

		
		// �w�b�_�[�Ǎ��J�n
		if( ! RecvHead(id) ) {
			LOG(em_ERR), "[%s] <%s> (%d) ACCEPT��̏���Ǎ����s", my_sThreadName, my_cSession, id);
			Close(id);
		}
	
	//======================================================
	// �w���V�[�ʒm
	} else if(nEventNo == EV_HELCY)	{
		for(int ii=0; ii<PARAM_SOCK_MAX; ii++) {
			if( ! my_bHelcyOk[ii] && IsConnect(ii) ) {
				LOG(em_ERR), "[%s] <%s> (%d) �w���V�[�`���^�C���A�E�g", my_sThreadName, my_cSession, ii);
				Close(ii);
			}
			my_bHelcyOk[ii] = false;
		}
	}

}