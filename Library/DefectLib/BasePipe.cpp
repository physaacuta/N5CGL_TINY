#include "StdAfx.h"
#include "BasePipe.h"

//------------------------------------------
// �R���X�g���N�^
//------------------------------------------
BasePipe::BasePipe(void) :
my_sThreadName("BasePipe"),
my_nConnect(0),
my_bIsServer(false),
my_sPipeName(""),
mcls_pPipe(NULL),
mcls_pLog(NULL)
{
	memset(&my_ovlSend,	0, sizeof(OVERLAPPED));
	memset(&my_ovlRecv,	0, sizeof(OVERLAPPED));
	memset(&my_ovlAccept,0, sizeof(OVERLAPPED));
	my_ovlSend.hEvent	= CreateEvent(NULL, TRUE, FALSE, NULL); // manual, reset
	my_ovlRecv.hEvent	= CreateEvent(NULL, TRUE, FALSE, NULL); // manual, reset
	my_ovlAccept.hEvent	= CreateEvent(NULL, FALSE, FALSE, NULL); // auto, reset

	InitializeCriticalSection(&my_cs);
}

//------------------------------------------
// �f�X�g���N�^
//------------------------------------------
BasePipe::~BasePipe(void)
{
	CloseHandle(my_ovlSend.hEvent);
	CloseHandle(my_ovlRecv.hEvent);
	CloseHandle(my_ovlAccept.hEvent);

	DeleteCriticalSection(&my_cs);
}

// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// �p�C�v����

//------------------------------------------
// �R�l�N�g (�ڑ������܂Ńu���b�N�����)
//------------------------------------------
bool BasePipe::Connect()
{
	if(my_bIsServer) return false;
	if(mcls_pPipe) return true;		// ���ɐڑ��ς�

	//// �ڑ�����
	try {
		mcls_pPipe = new PipeManager(my_sPipeName, my_bIsServer);
		my_nConnect = 1;
		return true;
	} catch(int code) {
		LOG(em_WAR), "[%s] PIPE �C���X�^���X�������s [%d]", my_sThreadName, code);
		mcls_pPipe = NULL;
		return false;
	}
}

//------------------------------------------
// ���X�j���O (�m���u���b�N)
//  �T�[�o�[���̐ڑ��֐�
//------------------------------------------
bool BasePipe::Listen()
{
	if( ! my_bIsServer) return false;

	// �C���X�^���X���܂��Ȃ����New
	if( NULL == mcls_pPipe) {
		try {
			mcls_pPipe = new PipeManager(my_sPipeName, my_bIsServer);
		} catch(int code) {
			LOG(em_WAR), "[%s] PIPE �C���X�^���X�������s [%d]", my_sThreadName, code);
			mcls_pPipe = NULL;
			return false;
		}
	}

	// ���X�j���O
	DWORD ret = mcls_pPipe->Listen(&my_ovlAccept);
	if(0 != ret) {
		LOG(em_WAR), "[%s] PIPE ���X�j���O���s [%d]", my_sThreadName, ret);
		if (ERROR_PIPE_CONNECTED == ret) {
			// �ڑ��ς�(�ʏ킠�肦�Ȃ�)
			return false;
		}
		return false;
	}

	return true;
}

//------------------------------------------
// �A�N�Z�v�g����
//------------------------------------------
void BasePipe::Accepting()
{
	EnterCriticalSection(&my_cs);

	// ��ԕύX
	my_nConnect += 1;

	LeaveCriticalSection(&my_cs);
}

//------------------------------------------
// �N���[�Y
//------------------------------------------
void BasePipe::Close()
{
	EnterCriticalSection(&my_cs);

	// �ڑ���S����������
	if( NULL != mcls_pPipe) {
		mcls_pPipe->Cancel();		// ��������I/O��������
		delete mcls_pPipe;			// �C���X�^���X�J��
		mcls_pPipe = NULL;
	}

	// �n���h������U�V�O�i���𗎂Ƃ�
	ResetEvent(my_ovlSend.hEvent);
	ResetEvent(my_ovlRecv.hEvent);
	ResetEvent(my_ovlAccept.hEvent);

	// ��ԕύX
	my_nConnect = 0;

	LeaveCriticalSection(&my_cs);
}

//------------------------------------------
// �w�b�_�[����M�J�n (�񓯊�)
//------------------------------------------
bool BasePipe::StartRecvHeader()
{
	bool ret;
	if( 0 == my_nConnect ) return false;

	EnterCriticalSection(&my_cs);

	memset(&my_cRecvHeadBuf, 0x00, sizeof(my_cRecvHeadBuf));
	my_bRecvHead = true;
	ret = mcls_pPipe->Recv(&my_cRecvHeadBuf, sizeof(my_cRecvHeadBuf), &my_ovlRecv);
	if(!ret) {
		if( ERROR_MORE_DATA == mcls_pPipe->GetError() ) {	// �I�[�v�����Recv�O�Ƀf�[�^�����ɑ��M����Ă���ꍇ�A���������f�[�^���ɂ��f�[�^������Ƃ������[�j���O���o��΍�
			ret = true;
			goto Ending;
		}
		LOG(em_ERR), "[%s] PIPE �w�b�_�[���ǂݍ��ݎ��s", my_sThreadName);
	}
Ending:
	LeaveCriticalSection(&my_cs);
	return ret;
}

//------------------------------------------
// �w�b�_�[����M������
//------------------------------------------
DWORD BasePipe::EndRecvHeader()
{
	DWORD	ret;
	if( 0 == my_nConnect ) return ERROR_BROKEN_PIPE;	// �ؒf
	
	if( mcls_pPipe == NULL ) {
		ret = ERROR_BROKEN_PIPE;	// �ؒf
	} else {
		if( mcls_pPipe->GetResult(&my_ovlRecv) ) {
			ret = ERROR_SUCCESS;
		} else {
			ret = mcls_pPipe->GetError();
		}
	}

	ResetEvent(my_ovlRecv.hEvent);					// �V�O�i���}�j���A�����Z�b�g
	return ret;
}


//------------------------------------------
// �f�[�^����M (����)
// BYTE* buf ��M�o�b�t�@�̃|�C���^
// DWORD len �c��̒���
//------------------------------------------
DWORD BasePipe::GetRecvBody(BYTE* buf, DWORD len)
{
	if( 0 == my_nConnect ) return ERROR_BROKEN_PIPE;	// �ؒf

	// �����Ŏc��̃f�[�^���擾
	return mcls_pPipe->Recv(buf, len);
}

//------------------------------------------
// �f�[�^���M (����)
// BYTE* buf ���M�o�b�t�@�̃|�C���^
// DWORD len ���M�f�[�^
// int nMaxWait �ő�҂����� [ms]
//------------------------------------------
bool BasePipe::SendLock(BYTE* buf, DWORD len, int nMaxWait)
{
	bool ret;
	if( 0 == my_nConnect ) return false;

	EnterCriticalSection(&my_cs);

	ret = mcls_pPipe->Send(buf, len, &my_ovlSend);
	if(!ret) {
		LOG(em_ERR), "[%s] PIPE �������M���s", my_sThreadName);
	} else {
		// ���M�����܂ő҂�
		DWORD dwTransByte;
		DWORD dwWaitRet = WaitForSingleObject(my_ovlSend.hEvent, nMaxWait);
		ret = !! GetOverlappedResult(mcls_pPipe->GetPipeHandle(), &my_ovlSend, &dwTransByte, FALSE);
		if (WAIT_OBJECT_0 == dwWaitRet) ResetEvent(my_ovlSend.hEvent);			// ����ɑ��M�ł�������V�O�i�����Z�b�g
	}

	LeaveCriticalSection(&my_cs);

	return ret;
}

//------------------------------------------
// �f�[�^���M (�񓯊�)
// BYTE* buf ���M�o�b�t�@�̃|�C���^
// DWORD len ���M�f�[�^
// int nMaxWait �ő�҂����� [ms]
//------------------------------------------
bool BasePipe::SendUnLock(BYTE* buf, DWORD len)
{
	return mcls_pPipe->Send(buf, len, &my_ovlSend);;
}