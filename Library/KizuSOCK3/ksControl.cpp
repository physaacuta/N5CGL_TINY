// ksControl.cpp: ksControl �N���X�̃C���v�������e�[�V����
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include <process.h>
#include <crtdbg.h>
#include <winsock2.h>
#include "ksControl.h"

using namespace KizuSock3;

//////////////////////////////////////////////////////////////////////
// �\�z/����
//////////////////////////////////////////////////////////////////////
//------------------------------------------
// �R���X�g���N�^
// DWORD tid PostThreadMessage�̑���� (ThreadID��n���Ηǂ�)
// DWORD msgbase ���b�Z�[�W�x�[�X�ԍ�
//------------------------------------------
ksControl::ksControl(DWORD tid, DWORD msgbase) : 
m_nTid(tid),
m_nMsgBase(msgbase),
m_nInstCount(0),
m_sock(INVALID_SOCKET),
m_hThread(NULL),
m_nTimeout(INFINITE),
m_evRecvTimeout(NULL),
m_evSendTimeout(NULL),
m_evDisConnect(NULL),
m_evAccept(NULL),
m_nKsThPriority(THREAD_PRIORITY_NORMAL)
{
	// ������
	for (int ii = 0; ii < KS_MAXCLIENTS; ii++) mcls_pInst[ii] = NULL;
	// �C�x���g����
	m_evStopEvent		  = CreateEvent(NULL, FALSE, FALSE, NULL);
	m_evAccepting		  = CreateEvent(NULL, TRUE, FALSE, NULL);	// WSAEventSelect�Ŏg���̂�manual reset�Ƃ��Ă���
	m_evAcceptEndCollBack = CreateEvent(NULL, FALSE, FALSE, NULL);	// �g�����ŃV�O�i���Ƃ���B (Accept�V�O�i���ʒm�������g�p���Ȃ�)

	// �N���e�B�J���Z�N�V����
	InitializeCriticalSection(&m_cs);
}
//------------------------------------------
// �f�X�g���N�^
//------------------------------------------
ksControl::~ksControl()
{
	// �q�X���b�h������
	for (int ii = 0; ii < KS_MAXCLIENTS; ii++) {
		if (NULL != mcls_pInst[ii]) {
			delete mcls_pInst[ii];
			mcls_pInst[ii] = NULL;
		}
	} 
	// Listen�p���C���X���b�h������
	if (NULL != m_hThread) {
		DWORD exitCode;
		GetExitCodeThread(m_hThread, &exitCode);
		if (STILL_ACTIVE == exitCode) {
			SetEvent(m_evStopEvent);
			DWORD dwRet = WaitForSingleObject(m_hThread, 5000);
			_ASSERT(dwRet == WAIT_OBJECT_0);
		}
		CloseHandle(m_hThread);
	}
	// �J��
	if( INVALID_SOCKET != m_sock ) closesocket(m_sock);	// �T�[�o�\�P�b�g�̏���
	CloseHandle(m_evStopEvent);
	CloseHandle(m_evAccepting);
	CloseHandle(m_evAcceptEndCollBack);
	DeleteCriticalSection(&m_cs);
}

//------------------------------------------
// �\�P�b�g���J��
// DWORD rcvBufSz ��M�o�b�t�@�T�C�Y
// DWORD sndBufSz ���M�o�b�t�@�T�C�Y
//------------------------------------------
int ksControl::SockOpen(DWORD rcvBufSz, DWORD sndBufSz)
{
	WSADATA wsaData;
	WORD wVersionRequired = MAKEWORD(2, 2);		// WinSock 2.0 Windows NT

	if( WSAStartup(wVersionRequired, &wsaData) == 0 ){
		// WinSock�ް�ޮ�����
		if( wVersionRequired != wsaData.wVersion ){
			WSACleanup();
			return -1;
		}
	} else {
		return -1;
	}

	// �\�P�b�g�̐���
	m_sock = WSASocket( AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED );
	if( m_sock == INVALID_SOCKET ){
		return -2;
	}
	// �o�b�t�@�T�C�Y�Z�b�g
	int opt = rcvBufSz;
	setsockopt( m_sock, SOL_SOCKET, SO_RCVBUF, (const char *)&opt, sizeof(opt) );
	opt = sndBufSz;
	setsockopt( m_sock, SOL_SOCKET, SO_SNDBUF, (const char *)&opt, sizeof(opt) );
// �o�b�t�@�ɒ��܂����p�P�b�g�𑦁A���M����Ƃ��Ɉȉ��̃R�����g���g�p
//	opt = 1;
//	setsockopt( m_sock, IPPROTO_TCP, TCP_NODELAY, (const char *)&opt, sizeof(opt) );
	return 0;
}

//------------------------------------------
// �ڑ����Ďq�X���b�h���쐬����i���C���X���b�h�͂Ȃ��j
// ksThread **newInst �����蓖�Ẵ|�C���^�Ɏq�X���b�h�̃|�C���^�l���Z�b�g���ĕԂ�
//------------------------------------------
int ksControl::Connect(ksThread **newInst)
{
	int ii;
	SOCKADDR_IN 	addr_in;

	// ���ނ̱��ڽ���
	addr_in.sin_family = AF_INET;
	addr_in.sin_port   = htons( m_nPort );
	addr_in.sin_addr.s_addr = inet_addr( m_Ip );

	// �ڑ�
	// �N���C�A���g�̏ꍇ�́AWSAEventSelect�͎g��Ȃ��̂Ńu���b�L���O���[�h�̂܂�
	if (connect( m_sock, (LPSOCKADDR)&addr_in, sizeof(SOCKADDR_IN))) {
		int a = GetLastError();
		return -4;
	}

	// �V�����q�X���b�h�̃C���X�^���X�����
	for (ii = 0; ii < KS_MAXCLIENTS; ii++) {		// �󂫂̎q�X���b�h��T��
		if (NULL == mcls_pInst[ii]) break;
	}
	if (KS_MAXCLIENTS == ii) return -1;	// ����ɒB����
	mcls_pInst[ii] = new ksThread(m_sock, m_nTimeout, m_nTid, m_nMsgBase, 
								  m_evRecvTimeout, m_evSendTimeout, m_evDisConnect);

	// �q�X���b�h�N��
	if (NULL == mcls_pInst[ii]) return -2;
	if (mcls_pInst[ii]->Start(m_nKsThPriority) ) {
		delete mcls_pInst[ii];
		mcls_pInst[ii] = NULL;
		return -3;
	}
	// ���������q�X���b�h�l���Z�b�g
	*newInst = mcls_pInst[ii];
	m_nInstCount++;
	return 0;			
}

//------------------------------------------
// �񓯊��ł����ɖ߂�B���ۂɂȂ���ƃ��b�Z�[�W��������
// int backlog �ۗ����̐ڑ��̃L���[�̍ő咷
//------------------------------------------
int ksControl::Listen(int backlog)
{
	SOCKADDR_IN 	addr_in;
	
	// �T�[�o�Ȃ̂Ń|�[�g�ԍ����ė��p�ł���悤�ɂ���
	int opt = 1;
	setsockopt( m_sock, SOL_SOCKET, SO_REUSEADDR, (const char *)&opt, sizeof(opt) );

	// ���ނ̱��ڽ���
	addr_in.sin_family = AF_INET;
	addr_in.sin_port   = htons( m_nPort );
	addr_in.sin_addr.s_addr = htonl(INADDR_ANY);
	
	// �\�P�b�g�ɖ��O������
	if (bind( m_sock, (LPSOCKADDR)&addr_in, sizeof(SOCKADDR_IN))) return -1;

	// �C�x���g���o��悤�ɂ���B����ȍ~�m���u���b�L���O�ɂȂ邱�Ƃɒ���
	if (SOCKET_ERROR == WSAEventSelect(m_sock, m_evAccepting, FD_ACCEPT)) return -3;

	// listen
	if (listen( m_sock, backlog )) return -2;

	// �T�[�o�X���b�h�X�^�[�g
	unsigned int tid;
	m_hThread = (HANDLE)_beginthreadex(NULL, 0, MainThread, (LPVOID)this, 0, &tid);
	if (NULL == m_hThread) return -1;
	return 0;
}

//------------------------------------------
// �ؒf���ĉ��
// ksThread *pChild �ؒf�����q�X���b�h�̃C���X�^���X
//------------------------------------------
int ksControl::CloseChild(ksThread *pChild)	
{
	int ii;
	// �w�肳�ꂽ�q�X���b�h��T��
	for (ii = 0; ii < KS_MAXCLIENTS; ii++) {
		if (pChild == mcls_pInst[ii]) break;
	}
	if (ii == KS_MAXCLIENTS) return -1;

	// �q�X���b�h���폜
	delete mcls_pInst[ii];	// �\�P�b�g�̏���
	mcls_pInst[ii] = NULL;

	EnterCriticalSection(&m_cs);
	m_nInstCount--;
	LeaveCriticalSection(&m_cs);
	return 0;
}

//------------------------------------------
// �q�X���b�h���L�邩
// UINT id �q�X���b�h�̃|�C���^�l
// �߂�l TRUE:�L�� FALSE:����
//------------------------------------------
int ksControl::IsValidChild(UINT id) const
{
	if (0 == id) return FALSE;
	for (int ii = 0; ii < KS_MAXCLIENTS; ii++) {
		if (id == (UINT)mcls_pInst[ii]) return TRUE;
	}
	return FALSE;
}
//------------------------------------------
// �������Ă���X���b�h�̐���Ԃ�
//------------------------------------------
int ksControl::GetActive() const
{
	int num = 0;
	for (int ii = 0; ii < KS_MAXCLIENTS; ii++) {
		if (NULL != mcls_pInst[ii]) {
			if (STAT_ACTIVE(mcls_pInst[ii]->GetStat()))		num++;
		}
	}
	return num;
}

//------------------------------------------
// IP�A�|�[�g�ݒ�
// const char *ip IP�A�h���X (xxx.xxx.xxx.xxx�̌`��)
// short port �|�[�g
//------------------------------------------
void ksControl::SetAddr(const char *ip, short port)
{
	_ASSERT(NULL != m_Ip && NULL != ip && 20>strlen(ip));
	strcpy(m_Ip, ip);
	m_Ip[sizeof(m_Ip)-1] = '\0';
	m_nPort = port;
}

//------------------------------------------
// �^�C���A�E�g�Đݒ�
// DWORD timeout �^�C���A�E�g����
//------------------------------------------
void ksControl::SetTimeout(DWORD timeout)
{
	m_nTimeout = timeout;
	for (int ii = 0; ii < KS_MAXCLIENTS; ii++) {
		if (NULL != mcls_pInst[ii]) {
			if (STAT_ACTIVE(mcls_pInst[ii]->GetStat()) )	mcls_pInst[ii]->SetTimeout(timeout);
		}
	}
}
//------------------------------------------
// ��M�^�C���A�E�g �C�x���g�ݒ�
// HANDLE evRecvTimeout ��M�^�C���A�E�g�C�x���g�p�n���h��
//------------------------------------------
void ksControl::SetEvRecvTimeout(HANDLE evRecvTimeout)
{
	m_evRecvTimeout = evRecvTimeout;
	for (int ii = 0; ii < KS_MAXCLIENTS; ii++) {
		if (NULL != mcls_pInst[ii]) { 
			if (STAT_ACTIVE(mcls_pInst[ii]->GetStat()) )	mcls_pInst[ii]->SetEvRecvTimeout(evRecvTimeout);
		}
	}
}
//------------------------------------------
// ���M�^�C���A�E�g �C�x���g�ݒ�
// HANDLE evSendTimeout ���M�^�C���A�E�g�C�x���g�p�n���h��
//------------------------------------------
void ksControl::SetEvSendTimeout(HANDLE evSendTimeout)
{
	m_evSendTimeout = evSendTimeout;
	for (int ii = 0; ii < KS_MAXCLIENTS; ii++) {
		if (NULL != mcls_pInst[ii]) { 
			if (STAT_ACTIVE(mcls_pInst[ii]->GetStat()) )	mcls_pInst[ii]->SetEvSendTimeout(evSendTimeout);
		}
	}
}
//------------------------------------------
// �ؒf�ʒm �C�x���g�ݒ�
// HANDLE evDisConnect �ؒf�ʒm�C�x���g�p�n���h��
//------------------------------------------
void ksControl::SetEvDisConnect(HANDLE evDisConnect)
{
	m_evDisConnect = evDisConnect;
	for (int ii = 0; ii < KS_MAXCLIENTS; ii++) {
		if (NULL != mcls_pInst[ii]) { 
			if (STAT_ACTIVE(mcls_pInst[ii]->GetStat()) )	mcls_pInst[ii]->SetEvDisConnect(evDisConnect);
		}
	}
}
//------------------------------------------
// �A�N�Z�v�g�ʒm �C�x���g�ݒ�
// HANDLE evAccept �A�N�Z�v�g�ʒm�C�x���g�p�n���h��
//------------------------------------------
void ksControl::SetEvAccept(HANDLE evAccept)
{
	m_evAccept = evAccept;
}

//------------------------------------------
// ���b�Z�[�W�E�C�x���g �ʒm
// UINT msg �����敪 (WM_KS_�Ȃ񂿂��)
// WPARAM wParam (���b�Z�[�W�ʒm���̂�)
// LPARAM lParam (���b�Z�[�W�ʒm���̂�)
//------------------------------------------
void ksControl::sendMsg(UINT msg, WPARAM wParam, LPARAM lParam)
{
	if( msg == WM_KS_ACCEPT && m_evAccept != NULL ) {						// �A�N�Z�v�g�v�� (�V�O�i���ʒm)
		// Accept ���b�Z�[�W�𐶐�
		memset(&m_LastAcceptMsg, 0x00, sizeof(m_LastAcceptMsg));
		m_LastAcceptMsg.message = m_nMsgBase+msg;
		m_LastAcceptMsg.wParam = wParam;
		m_LastAcceptMsg.lParam = lParam;

		// �O���ɒʒm
		SetEvent(m_evAccept);

		// �ʒm���Accept���b�Z�[�W���擾����܂Ńu���b�N�I�I
		DWORD ret = WaitForSingleObject(m_evAcceptEndCollBack, 3000);	
		if (WAIT_TIMEOUT == ret) {
			// �R�[�f�B���O�����������B
			_ASSERT(0);
		}
		

	} else if (0 != m_nTid) {
		// WM_KS_ACCEPT,		�q�X���b�h�C���X�^���X�l,		�ڑ�����IP�A�h���X
		PostThreadMessage(m_nTid, m_nMsgBase+msg, wParam, lParam);
	}
}

//------------------------------------------
// �A�N�Z�v�g�V�O�i���ʒm����MSG�����擾 (�A�N�Z�v�g�V�O�i���ʒm���̂ݎg�p)
// MSG* msg �A�N�Z�v�g�V�O�i���ʒm���ꂽ���̃A�N�Z�v�g���
//
// ���ӁF�A�N�Z�v�g�V�O�i���ʒm��A�{���\�b�h���R�[�������܂ŁA�{���C���X���b�h�̓u���b�N�����B
//------------------------------------------
void ksControl::GetAcceptEventMsg(MSG* msg)
{
	// �f�[�^��n��
	memcpy(msg, &m_LastAcceptMsg, sizeof(m_LastAcceptMsg));

	// ���C���X���b�h���s�J�n�ʒm
	SetEvent( m_evAcceptEndCollBack); 
}


//
// �X���b�h�{��
//

//------------------------------------------
// Listen�p���C���X���b�h
//------------------------------------------
//static UINT __stdcall ksControlThread(LPVOID arg)
unsigned int __stdcall ksControl::MainThread(LPVOID arg)
{
	int ii;

	// ���C���X���b�h��������
	ksControl * p = (ksControl *)arg;

	// �C�x���g�쐬
	HANDLE evArray[2] = {p->m_evStopEvent, p->m_evAccepting};
	enum { EV_STOP = WAIT_OBJECT_0,			// �I���ʒm
		   EV_ACCEPT,						// �A�N�Z�v�g�v��
		   EV_END};

	while (true) {
		DWORD dwWait = WaitForMultipleObjects(EV_END, evArray, FALSE, INFINITE);

		// �I���v�����ŗD��
		if (EV_STOP == dwWait) {	// �I��
			return 0;
		}

		// ACCEPT�ɂ��V�K�C���X�^���X�ǉ�
		if (EV_ACCEPT == dwWait) {	
			ResetEvent(p->m_evAccepting);			// �V�O�i�����蓮�ŗ��Ƃ�

			// accept������
			int			iaddrlen;
			SOCKADDR_IN	addr;			// �ڑ����̏��
			DWORD		ulongIP;		// �ڑ�����IP
			SOCKET		newSock;		// ACCEPT�����\�P�b�g�n���h��
			
			iaddrlen = sizeof(addr);
			newSock = accept( p->m_sock, (LPSOCKADDR)&addr, &iaddrlen );

			// �ڑ��v�����i�N���C�A���g�j�̂h�o�A�h���X���o��
			memcpy((char *)&ulongIP, &addr.sin_addr.S_un.S_un_b.s_b1, 4);
			
			// �q�X���b�h�̍쐬�ƋN��
			for (ii = 0; ii < KS_MAXCLIENTS; ii++) {		// �󂫎q�X���b�h��T��
				if (NULL == p->mcls_pInst[ii]) break;
			}
			if (KS_MAXCLIENTS == ii) {
				// ��t�Ȃ̂Őؒf
				shutdown(newSock, SD_SEND);
				closesocket(newSock);
				continue;
			}
			// �q�X���b�h�̐���
			p->mcls_pInst[ii] = new ksThread(newSock, p->m_nTimeout, p->m_nTid, p->m_nMsgBase, 
										     p->m_evRecvTimeout, p->m_evSendTimeout, p->m_evDisConnect);
			
			// �q�X���b�h�N��
			_ASSERT(p->mcls_pInst[ii]);
			p->mcls_pInst[ii]->Start(p->m_nKsThPriority);
			// �J�E���^
			EnterCriticalSection(&p->m_cs);
			p->m_nInstCount++;
			LeaveCriticalSection(&p->m_cs);
//char tmsg[256];
//wsprintf(tmsg, "��[%d] new %d \n",ii,(WPARAM)p->mcls_pInst[ii]);
//_RPT0(_CRT_WARN, tmsg);
			// �ʒm
			p->sendMsg(WM_KS_ACCEPT, (WPARAM)p->mcls_pInst[ii], ulongIP);
			continue;
		}
		_ASSERT(0);
	}
}
