// KizuHtRecv.cpp: KizuHtRecv �N���X�̃C���v�������e�[�V����
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "KizuHtRecv.h"

#define LOGKIZUHTRECV(t) if(mcls_pLogD!=NULL)     mcls_pLogD->Write(KizuLib::LogFileManager::t		// �����p���O�o��

//////////////////////////////////////////////////////////////////////
// �\�z/����
//////////////////////////////////////////////////////////////////////
//------------------------------------------
// �R���X�g���N�^
// const char *cSession �Z�b�V����������
// int torb �\�����ʃt���O(0:�\, 1:��)
// int accept_num �N���C�A���g�� (���������)
// int recv_num �t���[���P�ʂł̎�M�� (�_�������) (�ʏ�accept_num�Ɠ����B�����̕������J������1��̔���ŏ�������ꍇ�ɕς���Ă���)
// int recv_max_size ��M�o�b�t�@�̍ő�l (�f�t�H���g32M)
//------------------------------------------
KizuHtRecv::KizuHtRecv(const char *cSession, int torb, int accept_num, int recv_num, int recv_max_size) :
my_nThSock(0),
my_cSession(cSession), 
my_nTorB(torb),
my_nAcceptNum(accept_num),
my_nRecvNum(recv_num),
my_bRun(FALSE),
PARAM_SOCK_RECV_DATASIZE(recv_max_size),	// Ver.1.3.0.0
mcls_pSock(NULL),
mcls_pLog(NULL),
mcls_pLogD(NULL)
{
	//// ������
	for(int ii=0; ii<PARAM_SOCK_MAX; ii++) {
		memset(my_cRecvWkHead, 0x00, sizeof(my_cRecvWkHead));
		memset(my_cRecvHead, 0x00, sizeof(my_cRecvHead));
	}

	// ����������Ń��[�v
//	for(int ii=0; ii<my_nAcceptNum; ii++) {
	for(int ii=0; ii<PARAM_SOCK_MAX; ii++) {
		// �C�x���g
		my_evDisConnect[ii]	 = CreateEvent(NULL, FALSE, FALSE, NULL);	// �ؒf�C�x���g�F��(ID)
		my_evRecvTimeOut[ii] = CreateEvent(NULL, FALSE, FALSE, NULL);	// ��M�^�C���A�E�g�C�x���g�F��(ID)
		my_evRecv[ii]		 = CreateEvent(NULL, FALSE, FALSE, NULL);	// ��M�C�x���g�F��(ID)
	}

	// �_��������Ń��[�v
	for(int ii=0; ii<my_nRecvNum; ii++) {
		// ��M�f�[�^
		my_cRecvData[ii]	= new BYTE[PARAM_SOCK_RECV_DATASIZE];		// ��M�f�[�^(RECVID)
		memset(my_cRecvData[ii], 0x00, PARAM_SOCK_RECV_DATASIZE);
	}


	// �C�x���g
	my_evConnectAll 		= CreateEvent(NULL, FALSE, FALSE, NULL);	// �S�N���C�A���g�ڑ�������
	my_evErrToClose 		= CreateEvent(NULL, FALSE, FALSE, NULL);	// �ُ픭���ɂ�� �N���C�A���g�ؒf��
	my_evDetectWaitStart	= CreateEvent(NULL, FALSE, FALSE, NULL);	// �����J�n�҂��v���V�O�i��
	my_evDetectRun			= CreateEvent(NULL, FALSE, FALSE, NULL);	// �����J�n�v���V�O�i��
	my_evDetectStop			= CreateEvent(NULL, FALSE, FALSE, NULL);	// ������~�v���V�O�i��
	my_evAccept				= CreateEvent(NULL, FALSE, FALSE, NULL);	// �e����PC���Accept��

	//// ���Z�b�g(������)
	Reset();
}

//------------------------------------------
// �f�X�g���N�^
//------------------------------------------
KizuHtRecv::~KizuHtRecv()
{
	//// �\�P�b�g���
	if( NULL != mcls_pSock ) {
		delete mcls_pSock;
		mcls_pSock = NULL;
	}
	//// �����J��
	// �����I�Ȑ��Ń��[�v
	for(int ii=0; ii<PARAM_SOCK_MAX; ii++) {
		// �C�x���g
		CloseHandle(my_evDisConnect[ii]);
		CloseHandle(my_evRecvTimeOut[ii]);
		CloseHandle(my_evRecv[ii]);
	}
	// �_���I�Ȑ��Ń��[�v
	for(int ii=0; ii<my_nRecvNum; ii++) {
		// ��M�f�[�^
		delete [] my_cRecvData[ii];
		my_cRecvData[ii] = NULL;
	}

	//// �C�x���g
	CloseHandle(my_evConnectAll);	// �S�N���C�A���g�ڑ�������
	CloseHandle(my_evErrToClose);	// �ُ픭���ɂ�� �N���C�A���g�ؒf��
	CloseHandle(my_evDetectWaitStart);
	CloseHandle(my_evDetectRun);
	CloseHandle(my_evDetectStop);
	CloseHandle(my_evAccept);	// �e����PC���Accept��
}

//------------------------------------------
// ���݂̃X�e�[�^�X����
// int id �������ID
//------------------------------------------
CString KizuHtRecv::GetSockStateName(int id) const
{
	CString wk;
	if		(my_emSockState[id] == STATE_NONE)	{ wk = "������� (�ؒf��)"; }
	else if (my_emSockState[id] == STATE_IDLE)	{ wk = "�������� (�ڑ���)"; }
	else if (my_emSockState[id] == STATE_HEAD)	{ wk = "�w�b�_�� ��M��";}
	else if (my_emSockState[id] == STATE_DATA)	{ wk = "�f�[�^�� ��M��";}
	else										{ wk = "�X�e�[�^�X�ُ�";}
	return wk;
}

// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// �X�e�[�^�X����
//------------------------------------------
// �N���C�A���g���̂��Z�b�g (����Ŕ���PC����̃C�j�V�����`���𔻒f���Ăǂ̔���@�\�ƌq�����������f����)
// int id 0�I���W�� (DS,DQ�c�̏�)
// char const* cVal �N���C�A���g��
//------------------------------------------
void KizuHtRecv::SetCliantName(int id, char const* cVal)
{
	sprintf(my_cClientName[id], cVal);
}


// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// �V�[�P���X����

//------------------------------------------
// ���ׂẴw�b�_�[��M���J�n
//------------------------------------------
BOOL KizuHtRecv::RecvAllHeader()
{
	int ii;
	//// �J�n�\���`�F�b�N
	for(ii=0; ii<my_nAcceptNum; ii++) {
		if( my_bRun && my_PC[ii] < 0 ) {					// PCID�����蓖��
			LOG(em_ERR), "[KizuHtRecv] <%s> �S�N���C���g���q�����Ă��Ȃ����߃w�b�_��M���~ [�ڑ���=%d]", my_cSession, ii);
			//syslog(206, "[�ڑ���=%d]", ii);
			// �S�ẴN���C�A���g��ؒf
			CloseAll();
			return FALSE;

		} else if( STATE_IDLE != my_emSockState[ii] ) {		// �w�b�_�[��M�\��ԂłȂ�
			LOG(em_ERR), "[KizuHtRecv] <%s> �w�b�_�[��M�\��ԂłȂ� [PCID=%d status=%d]", my_cSession, my_PC[ii], my_emSockState[ii]);
			//syslog(207, "[PCID=%d status=%d]", my_PC[ii], my_emSockState[ii]);
			// �S�ẴN���C�A���g��ؒf
			CloseAll();
			return FALSE;
		}
	}

	//// �f�[�^��M�J�n����
	for(ii=0; ii<my_nRecvNum; ii++) {			// �_������P��
		my_bDataReady[ii] = false;						// �f�[�^��M�҂�
	}
	for(ii=0; ii<my_nAcceptNum; ii++) {			// ��������P��
		my_nRecvingDataSize[ii] = 0;
		my_nRecvNowCnt[ii] = 0;
	}

	//// �f�[�^��M�J�n
	for(ii=0; ii<my_nAcceptNum; ii++) {
		// �w�b�_�[��M�J�n
		if( ! RecvHeaderDefect(ii) ) {
			return FALSE;
		}
	}
	return TRUE;
}

//------------------------------------------
// �Ɩ��`�� �w�b�_�[����M����
// int id ID
// �߂�l�F -1:�ُ� 0�`:RECVID=CAMPOS 
//------------------------------------------
int KizuHtRecv::AnsHeader(int id)
{
	_ASSERT(NULL != mcls_pSock);

	//// �z���g�ɋƖ��`�����m�F
	if(0 != memcmp( my_cRecvWkHead[id].base_head.code, T_CODE_DEFECT, SIZE_SOCK_TC) ) {
		LOG(em_ERR), "[KizuHtRecv] <%s> �Ɩ��`���`�� �g�����U�N�V�����R�[�h�ُ�[name=%s code=%4.4s]", my_cSession, my_cClientName[my_PC[id]-1], my_cRecvWkHead[id].base_head.code);
		syslog(SYSNO_SOCK_TC_NG, "[TC�ُ� name=%s code=%4.4s]", my_cClientName[my_PC[id]-1], my_cRecvWkHead[id].base_head.code);
		// �S�ẴN���C�A���g��ؒf
		CloseAll();
		return -1;
	}

	//// �_�����ID�͐��킩
	if( 0 > my_cRecvWkHead[id].base_head.subinf || my_nRecvNum <= my_cRecvWkHead[id].base_head.subinf ) {
		LOG(em_ERR), "[KizuHtRecv] <%s> �Ɩ��`���`�� �_�����ID�ُ�[name=%s id=%d]", my_cSession, my_cClientName[my_PC[id]-1], my_cRecvWkHead[id].base_head.subinf);
		syslog(SYSNO_SOCK_TC_NG, "[�_�����ID�ُ� name=%s id=%d]", my_cClientName[my_PC[id]-1], my_cRecvWkHead[id].base_head.subinf);
		// �S�ẴN���C�A���g��ؒf
		CloseAll();
		return -1;
	}

	//// �f�[�^����M�J�n�̏���
	int recvid = my_cRecvWkHead[id].base_head.subinf;								// �_�����ID
	DWORD sz   = my_cRecvWkHead[id].base_head.len - SIZE_SOCK_HEAD;					// ��M�T�C�Y�m�F
	memcpy( &my_cRecvHead[recvid], &my_cRecvWkHead[id], sizeof(SOCK_F_HEAD));		// ���[�N�w�b�_�[�̏��� �_��������ɕ��ёւ�
	my_nRecvingDataSize[id] = 0;													// �f�[�^����M�T�C�Y������


	//// �Ɩ��`����M
	//��	LOG(em_INF), "[KizuHtRecv] �f�[�^����M [%s, %d]", my_cClientName[my_PC[id]-1], sz);//��
//LOGKIZUHTRECV(em_INF), "[KizuHtRecv] <%s> �f�[�^����M [%s, %d]", my_cSession, my_cClientName[my_PC[id]-1], sz);

	_ASSERT(sz >= 0);
	if( 0 == sz ) {					// �w�b�_�[���̂�
		// �Ɩ��`�� ��M�����`�F�b�N
		if( AnsData(id) ) {
			return  recvid;
		} else {
			return -1;
		}
	}

	//// �f�[�^��������ꍇ�́A�����ăf�[�^������M
	if( RecvDataDefect(id, recvid) ) {
		return recvid;
	} else {
		return -1;
	}
}

//------------------------------------------
// �Ɩ��`�� ��M�����`�F�b�N
// int id ID
//------------------------------------------
BOOL KizuHtRecv::AnsData(int id)
{
	int recvid = my_cRecvWkHead[id].base_head.subinf;		// �_�����ID

	//// �_������P�ʂŃz���g�Ƀf�[�^��S����M�ł����H (�������M�Ή�)
	if(my_cRecvWkHead[id].base_head.len > my_nRecvingDataSize[id]+SIZE_SOCK_HEAD) {

//LOG(em_INF), "[KizuHtRecv] <%s> �`��������M id=%d reccvid=%d size=%d total=%d", my_cSession, id, recvid, my_nRecvingDataSize[id], my_cRecvWkHead[id].base_head.len);
//LOGKIZUHTRECV(em_INF), "[KizuHtRecv] <%s> �`��������M id=%d reccvid=%d size=%d total=%d", my_cSession, id, recvid, my_nRecvingDataSize[id], my_cRecvWkHead[id].base_head.len);

		// �܂��f�[�^���̎�M�r��
		return RecvDataDefect(id, recvid);
	} 


	//// �_������P�ʂŎ�M����
	my_bDataReady[recvid] = true;	
	my_nRecvNowCnt[id] += 1;

//	LOG(em_INF), "[KizuHtRecv] <%s> �_�������M���� id=%d reccvid=%d", my_cSession, id, recvid);
	//// ��������P�ʂŃf�[�^��M�����H
	if( my_nRecvNowCnt[id] < my_nRecvMaxCnt[id] ) {
//LOG(em_INF), "[KizuHtRecv] <%s> �����_�������M�J�n id=%d reccvid=%d", my_cSession, id, recvid);

		// ���_������̃w�b�_�[����M�J�n
		return RecvHeaderDefect(id); 
	}

//��	LOG(em_INF), "[KizuHtRecv] <%s> ���������M���� id=%d reccvid=%d", my_cSession, id, recvid);
LOGKIZUHTRECV(em_INF), "[KizuHtRecv] <%s> ���������M���� id=%d reccvid=%d", my_cSession, id, recvid);

	//// �����܂ŗ�����A���̕�������ł̂P�t���[�����̏��͑S�Ď�M����
	my_emSockState[id] = STATE_IDLE;				// �X�e�[�^�X�ύX
	return TRUE;
}

//------------------------------------------
// �C�j�V�����`�� ��M����
// int id ID
//------------------------------------------
BOOL KizuHtRecv::AnsInitializeSocket(int id)
{
	// �z���g�ɃC�j�V�����`�����m�F
	if(0 != memcmp( my_cRecvWkHead[id].base_head.code, T_CODE_NAME, SIZE_SOCK_TC) ) {
		LOG(em_ERR), "[KizuHtRecv] <%s> �C�j�V�����`�� �g�����U�N�V�����R�[�h�ُ�[code = %4.4s]", my_cSession, my_cRecvWkHead[id].base_head.code);
		syslog(SYSNO_SOCK_TC_NG, "[section=%s code=%4.4s]", my_cSession, my_cRecvWkHead[id].base_head.code);
		// �ُ�ʒm
		CloseAll();
		return FALSE;
	}
	//LOG(em_WAR), "[%d] <%s> �C�j�V�����`��", my_nTorB, my_cRecvWkHead[id].base_head.pname);
LOGKIZUHTRECV(em_WAR), "[KizuHtRecv] [%d] <%s> �C�j�V�����`����M", my_nTorB, my_cRecvWkHead[id].base_head.pname);

	// PCID ������
	for(int ii=0; ii<my_nAcceptNum; ii++) {
		if(0 != strcmp( my_cRecvWkHead[id].base_head.pname, my_cClientName[ii] ) ) continue;

		// �_�������
		int recvcnt = my_cRecvWkHead[id].base_head.subinf;
		if( 0 >= recvcnt || my_nRecvNum < recvcnt ) {
			LOG(em_ERR), "[KizuHtRecv] <%s> �C�j�V�����`�� �_��������ُ�[name = %s, recvcnt=%d]", my_cSession, my_cRecvWkHead[id].base_head.pname, recvcnt);
			syslog(SYSNO_SOCK_TC_NG, "[�_��������ُ� name=%s recvcnt=%d]", my_cRecvWkHead[id].base_head.pname, recvcnt);
			return FALSE;
		}
		my_nRecvMaxCnt[id] = recvcnt;
		my_nRecvNowCnt[id] = 0;

		// PCID ������
		my_PC[id] = ii+1;
		// �X�e�[�^�X�ύX
		my_emSockState[id] = STATE_IDLE;

		LOG(em_MSG), "[KizuHtRecv] <%s> �C�j�V�����`����M [%s] ID=%d PCID=%d", my_cSession, my_cRecvWkHead[id].base_head.pname, id, my_PC[id]);
LOGKIZUHTRECV(em_MSG), "[KizuHtRecv] <%s> �C�j�V�����`����M [%s] ID=%d PCID=%d", my_cSession, my_cRecvWkHead[id].base_head.pname, id, my_PC[id]);

		syslog(SYSNO_SOCK_INITIALIZ, "[name=%s]", my_cRecvWkHead[id].base_head.pname);
		return TRUE;
	}

	// PCID�����蓖��
	LOG(em_ERR), "[KizuHtRecv] <%s> �C�j�V�����`�� PCID�����蓖��[name = %s]", my_cSession, my_cRecvWkHead[id].base_head.pname);
LOGKIZUHTRECV(em_ERR), "[KizuHtRecv] <%s> �C�j�V�����`�� PCID�����蓖��[name = %s]", my_cSession, my_cRecvWkHead[id].base_head.pname);
	syslog(SYSNO_SOCK_TC_NG, "[PCID�����蓖�� name=%s]", my_cRecvWkHead[id].base_head.pname);
	return FALSE;
}

//------------------------------------------
// �T���J�n����
//------------------------------------------
void KizuHtRecv::DefectStart()
{
	_ASSERT(NULL != mcls_pSock);

	mcls_pSock->Cancel();	// �Ǎ���������΃L�����Z��
	// ������
	for(int ii=0; ii<my_nAcceptNum; ii++) {
		my_emSockState[ii] = STATE_IDLE;
	}
	for(int ii=0; ii<my_nRecvNum; ii++) {
		my_nFrameNo[ii] = -1;
		my_bDataReady[ii] = false;
	}

	my_bRun = TRUE;
}
//------------------------------------------
// �T����~����
//------------------------------------------
void KizuHtRecv::DefectStop()
{
	my_bRun = FALSE;
}

//------------------------------------------
// �t���[��No�̘A�ԃ`�F�b�N
// DWORD framenum �����M�����t���[��No (0�I���W��)
// int id �������ID
// int recvid �_�����ID
//------------------------------------------
BOOL KizuHtRecv::CheckFrameNo(DWORD framenum, int id, int recvid)
{
	// �T�����ȊO�͐���
	if( !my_bRun ) return TRUE;

	// �t���[��No�`�F�b�N
	if( my_nFrameNo[recvid] + 1 == framenum ) {
		my_nFrameNo[recvid] = framenum;
		return TRUE;
	}
	
	// �t���[��No�H���Ⴂ�G���[
	LOG(em_ERR), "[KizuHtRecv] <%s> �t���[��No�H���Ⴂ���� [%s, recvid=%d pre=%d, now=%d]", my_cSession, my_cClientName[my_PC[id]-1], recvid, my_nFrameNo[recvid], framenum);
LOGKIZUHTRECV(em_ERR), "[KizuHtRecv] <%s> �t���[��No�H���Ⴂ���� [%s, recvid=%d pre=%d, now=%d]", my_cSession, my_cClientName[my_PC[id]-1], recvid, my_nFrameNo[recvid], framenum);
	//syslog(250, "[%s: recvid=%d pre=%d now=%d]", my_cClientName[my_PC[id]-1], recvid, my_nFrameNo[recvid], framenum);

	// �S�ẴN���C�A���g��ؒf
	CloseAll();
	return FALSE;
}

//------------------------------------------
// �S����PC�����M����
// �߂�l TRUE:�S����PC�Ǎ�����
//------------------------------------------
BOOL KizuHtRecv::CheckAllReadEnd()
{
	// �T����~���̓_��
	if( !my_bRun ) return FALSE;

	// �Ɩ��`����M�ς݁H
	for(int ii=0; ii<my_nRecvNum; ii++) {
		if( !my_bDataReady[ii] ) return FALSE;
	}
	return TRUE;
}

// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// TPC/IP �ʐM����
//------------------------------------------
// ���Z�b�g
//------------------------------------------
void KizuHtRecv::Reset()
{
	//// ���݂̓Ǎ��L�����Z��
//	my_bRun = FALSE;
	if(mcls_pSock) mcls_pSock->Cancel();

	//// ������
	memset( my_cRecvWkHead, 0x00, sizeof(my_cRecvWkHead));
	memset( my_cRecvHead, 0x00, sizeof(my_cRecvHead));

	// �_��������Ń��[�v
	for(int ii=0; ii<my_nRecvNum; ii++) {
		my_nFrameNo[ii] = -1;
		my_bDataReady[ii] = false;
	}
	// ����������Ń��[�v
	for(int ii=0; ii<my_nAcceptNum; ii++) {
		my_emSockState[ii] = STATE_NONE;
		memset( my_cClientName[ii], 0x00, SIZE_NAME_PC);
		my_PC[ii] = -1;
		my_nRecvingDataSize[ii]	= 0;
		my_nRecvMaxCnt[ii]		= 0;
		my_nRecvNowCnt[ii]		= 0;
		ResetEvent(my_evRecv[ii]);
	}
}

//------------------------------------------
// Listen
// bool bLogDsp ���O�o�͗L�� (false:���s���O�o���Ȃ�)
//------------------------------------------
BOOL KizuHtRecv::Listen(bool bLogDsp)
{
	int retc;
	// �\�P�b�g�C���X�^���X����
	try {
		mcls_pSock = new SockBase(my_cSession, TASKINI_NAME, my_nThSock, PARAM_SOCK_TIMEOUT,
										 my_nAcceptNum, PARAM_SOCK_RECV_BUFSIZE);
	} catch(int code) {
		mcls_pSock = NULL;
		if(bLogDsp) {
			LOG(em_ERR), "[KizuHtRecv] <%s> �\�P�b�g New���s err_code=%d", my_cSession, code);
			syslog(SYSNO_SOCK_NEW_ERR, "[%s: err_code=%d]", my_cSession, code);
		}
		return FALSE;
	}

	// Listen
	if( NULL != mcls_pSock ) {
		mcls_pSock->SetEvAccept(my_evAccept);							// Accept�F�����C�x���g�ɂ��� (�T�[�o�[�̂�)

		retc = mcls_pSock->Listen();
		if( 0 != retc ) {
			delete mcls_pSock;
			mcls_pSock = NULL;
			if(bLogDsp) {
				LOG(em_ERR), "[KizuHtRecv] <%s> �\�P�b�g Listen���s err_code=%d", my_cSession, retc);
				syslog(SYSNO_SOCK_LISTEN_ERR, "[%s: err_code=%d]", my_cSession, retc);
			}
			return FALSE;
		}
	}

	LOG(em_MSG), "[KizuHtRecv] <%s> �\�P�b�g Listen�J�n", my_cSession);
	return TRUE;
}

//------------------------------------------
// �S���萳��?
//------------------------------------------
bool KizuHtRecv::AllConnect()
{
	int ii;
	// �S�ڑ��H
	for(ii=0; ii<my_nAcceptNum; ii++) {
		if(my_emSockState[ii] == STATE_NONE) break;
	}
	if(ii==my_nAcceptNum) return true;
	else				  return false;
}
/*
//------------------------------------------
// Accept
// MSG* msg WM_KS_ACCEPT���̃��b�Z�[�W
//------------------------------------------
int KizuHtRecv::Accept(MSG* msg)
{
	_ASSERT(NULL != mcls_pSock);

	// ID�擾
	int id = mcls_pSock->Accept(msg);
	if( id < 0 ) {
		LOG(em_ERR), "[KizuHtRecv] <%s> �N���C�A���g���I�[�o�[", my_cSession);
		syslog(SYSNO_SOCK_CLIENT_MAX, "[%s]", my_cSession);
		return FALSE;
	}

	// �X�e�[�^�X�ύX
	my_emSockState[id] = STATE_IDLE;

	// �S�ڑ��H
	int ii;
	for(ii=0; ii<my_nAcceptNum; ii++) {
		if(my_emSockState[ii] == STATE_NONE) break;
	}
	if(ii==my_nAcceptNum) SetEvent(my_evConnect);
	return id;
}
*/

//------------------------------------------
// Accept (�V�O�i���ʒm�p)
//------------------------------------------
int KizuHtRecv::Accept()
{
	_ASSERT(NULL != mcls_pSock);

	// ID�擾
	int id = mcls_pSock->Accept();
	if( id < 0 ) {
		LOG(em_ERR), "[KizuHtRecv] <%s> �N���C�A���g���I�[�o�[", my_cSession);
		syslog(SYSNO_SOCK_CLIENT_MAX, "[%s]", my_cSession);
		return FALSE;
	}

	// ���q�������\�P�b�g�̏�Ԃ�ύX
	mcls_pSock->SetEvDisConnect(id, my_evDisConnect[id]);					// �ؒf�F�����C�x���g�ɂ���
	mcls_pSock->SetEvRecvTimeout(id, my_evRecvTimeOut[id]);					// ��M�^�C���A�E�g�F�����C�x���g�ɂ���

	// �X�e�[�^�X�ύX
	my_emSockState[id] = STATE_IDLE;

	// �S�ڑ��H
/*
	int ii;
	for(ii=0; ii<my_nAcceptNum; ii++) {
		if(my_emSockState[ii] == STATE_NONE) break;
	}
	if(ii==my_nAcceptNum) SetEvent(my_evConnectAll);
*/
	if( AllConnect() ) SetEvent(my_evConnectAll);
	return id;
}

//------------------------------------------
// �S�N���C�A���g�ؒf & �ُ�ʒm
// bool bErr �ُ�ʒm (false:�ُ�ؒf true:����ؒf)
//------------------------------------------
void KizuHtRecv::CloseAll(bool bErr)
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
void KizuHtRecv::ErrCall()
{
	if(NULL == mcls_pSock) return;

	// ���Z�b�g
	Reset();
	SetEvent(my_evErrToClose);
}

//------------------------------------------
// �C�j�V�����`����M�p�̃w�b�_�[��M (�悤�� PCID �����蓖�ď�Ԏ�)
// int id �������ID (Accept�̖߂�l��n��)
//------------------------------------------
BOOL KizuHtRecv::RecvHeaderInitialize(int id)
{
	// �w�b�_�[��M�J�n
	int iRet = mcls_pSock->Recv(id, &my_cRecvWkHead[id], SIZE_SOCK_HEAD, my_evRecv[id]);
	if( 0 != iRet ) {
		if( -1 == my_PC[id] ) {
			LOG(em_ERR), "[KizuHtRecv] <%s> �w�b�_�[��M�G���[ [err_code=%d]", my_cSession, iRet);
			syslog(SYSNO_SOCK_RECV_ERR, "[(HEAD) section=%s err_code=%d]", my_cSession, iRet);
		} else {
			LOG(em_ERR), "[KizuHtRecv] <%s> �w�b�_�[��M�G���[ [err_code=%d]", my_cClientName[my_PC[id]-1],iRet);
			syslog(SYSNO_SOCK_RECV_ERR, "[(HEAD) name=%s err_code=%d]", my_cClientName[my_PC[id]-1], iRet);
		}
		// �ُ�ʒm
		CloseAll();
		return FALSE;
	} else {
		// �X�e�[�^�X�ύX
		my_emSockState[id] = STATE_HEAD;
	}
	return TRUE;
}

//------------------------------------------
// �Ɩ��`���̃w�b�_�[��M���J�n
// int id �������ID
//------------------------------------------
BOOL KizuHtRecv::RecvHeaderDefect(int id)
{
	// �w�b�_�[��M�J�n
	int iRet = mcls_pSock->Recv(id, &my_cRecvWkHead[id], SIZE_SOCK_HEAD, my_evRecv[id]);
	if( 0 != iRet ) {
		LOG(em_ERR), "[KizuHtRecv] <%s> �w�b�_�[��M�G���[ [err_code=%d]", my_cClientName[my_PC[id]-1],iRet);
		syslog(SYSNO_SOCK_RECV_ERR, "[(HEAD) name=%s err_code=%d]", my_cClientName[my_PC[id]-1], iRet);
		// �ُ�ʒm
		CloseAll();
		return FALSE;
	} else {
		// �X�e�[�^�X�ύX
		my_emSockState[id] = STATE_HEAD;
		return TRUE;
	}
}

//------------------------------------------
// �Ɩ��`���̃f�[�^����M���J�n
// int id �������ID
// int recvid �_�����ID
//------------------------------------------
BOOL KizuHtRecv::RecvDataDefect(int id, int recvid)
{
	// ��M�T�C�Y����
	DWORD size = my_cRecvWkHead[id].base_head.len - SIZE_SOCK_HEAD - my_nRecvingDataSize[id];		// ��M�T�C�Y
	if(size > SIZE_SOCK_DATA_MAX) size = SIZE_SOCK_DATA_MAX;										// 1��̎�M�T�C�Y�I�[�o�[

	// �f�[�^����M�J�n
	int iRet = mcls_pSock->Recv(id, &my_cRecvData[recvid][my_nRecvingDataSize[id]], size, my_evRecv[id]);
	if( 0 != iRet ) {
		LOG(em_ERR), "[KizuHtRecv] <%s> �f�[�^����M�G���[ [%s, recvid=%d, size=%d, now=%d, total=%d, err_code=%d]", my_cSession, my_cClientName[my_PC[id]-1], recvid, size, my_nRecvingDataSize[id], my_cRecvWkHead[id].base_head.len, iRet);
		syslog(SYSNO_SOCK_RECV_ERR, "[(DATA) name=%s recvid=%d size=%d now=%d total=%d err_code=%d]", my_cClientName[my_PC[id]-1], recvid, size, my_nRecvingDataSize[id], my_cRecvWkHead[id].base_head.len, iRet);
		// �S�ẴN���C�A���g��ؒf
		CloseAll();
		return FALSE;
	} else {
		// �X�e�[�^�X�ύX
		my_emSockState[id] = STATE_DATA;
		my_nRecvingDataSize[id] += size;
		return TRUE;
	}
}
