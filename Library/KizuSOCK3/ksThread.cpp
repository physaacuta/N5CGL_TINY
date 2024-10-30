// ksThread.cpp: ksThread �N���X�̃C���v�������e�[�V����
//
//////////////////////////////////////////////////////////////////////


#include "stdafx.h"
#include <process.h>
#include <crtdbg.h>

#include <winsock2.h>
#include "ksThread.h"

using namespace KizuSock3;

//////////////////////////////////////////////////////////////////////
// �\�z/����
//////////////////////////////////////////////////////////////////////
//------------------------------------------
// �R���X�g���N�^
// SOCKET sock �ʐM����\�P�b�g�n���h��
// DWORD timeout �^�C���A�E�g�Ԋu [ms]
// DWORD tid PostThreadMessage�̑���� (ThreadID��n���Ηǂ�)
// DWORD msgbase ���b�Z�[�W�x�[�X�ԍ�
// HANDLE evRecvTimeout ��M�^�C���A�E�g�n���h��
// HANDLE evSendTimeout ��M�^�C���A�E�g�n���h��
// HANDLE evDisconnect �ؒf�ʒm�n���h��
//------------------------------------------
ksThread::ksThread(SOCKET sock, DWORD timeout, DWORD tid, DWORD msgbase,
				   HANDLE evRecvTimeout, HANDLE evSendTimeout, HANDLE evDisconnect) :
m_sock((HANDLE)sock),
m_nTimeout(timeout),
m_nTid(tid),
m_nMsgBase(msgbase),
m_evRecvTimeout(evRecvTimeout),
m_evSendTimeout(evSendTimeout),
m_evDisConnect(evDisconnect),
m_hThread(NULL),
m_stat(0),
m_apierr(0),
m_nCloseKind(KIZUSOCK_CLOSEKIND_NON)
{

	// �V�O�i������
	m_evStopEvent	= CreateEvent(NULL, FALSE, FALSE, NULL);
	m_evSetTimeout	= CreateEvent(NULL, FALSE, FALSE, NULL);
	m_evCancelEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	m_evRecvStart	= CreateEvent(NULL, FALSE, FALSE, NULL);
	m_evSendStart	= CreateEvent(NULL, FALSE, FALSE, NULL);
	m_evRecvAck		= CreateEvent(NULL, FALSE, FALSE, NULL);
	m_evSendAck		= CreateEvent(NULL, FALSE, FALSE, NULL);
}
//------------------------------------------
// �f�X�g���N�^
//------------------------------------------
ksThread::~ksThread()
{
	Stop();
	closesocket((SOCKET)m_sock);
	// �V�O�i���J��
	CloseHandle(m_evStopEvent);
	CloseHandle(m_evSetTimeout);
	CloseHandle(m_evCancelEvent);
	CloseHandle(m_evRecvStart);
	CloseHandle(m_evSendStart);
	CloseHandle(m_evRecvAck);
	CloseHandle(m_evSendAck);
}

//------------------------------------------
// �X���b�h�J�n
// int nPriority �D��x
//------------------------------------------
int ksThread::Start(int nPriority)
{
	unsigned int tid;
	m_hThread = (HANDLE)_beginthreadex(NULL, 0, MainThread, (LPVOID)this, 0, &tid);
	if (0 == m_hThread) {
		m_stat = KIZUSOCK_STAT_THREAD_ERR;
		return -1;
	}

	// �X���b�h�D��x�w��
	if(THREAD_PRIORITY_NORMAL != nPriority) {
		SetThreadPriority(m_hThread, nPriority);
	}

	m_stat = KIZUSOCK_STAT_ACTIVE;
	return 0;
}

//------------------------------------------
// �X���b�h��~
//------------------------------------------
void ksThread::Stop()
{
	if (NULL != m_hThread) {
		DWORD exitCode;
		GetExitCodeThread(m_hThread, &exitCode);
		if (STILL_ACTIVE == exitCode) {
			SetEvent(m_evStopEvent);
			DWORD dwRet = WaitForSingleObject(m_hThread, 5000);
			_ASSERT(dwRet == WAIT_OBJECT_0);
			if (dwRet == WAIT_TIMEOUT) {
				TerminateThread(m_hThread, 0);
			}
		}
		CloseHandle(m_hThread);
		m_hThread = NULL;
	}
}


//------------------------------------------
// ��M�J�n���� (���C���X���b�h�̎�M�J�n��������������܂Ńu���b�N)
// LPVOID pData ��M�̈�
// DWORD len ��M����T�C�Y
// HANDLE evEvent ��M�������ɒʒm����V�O�i��
//------------------------------------------
int ksThread::Recv(LPVOID pData, DWORD len)
{
	return Recv(pData, len, NULL);
}
int ksThread::Recv(LPVOID pData, DWORD len, HANDLE evEvent)
{
	if (STAT_WAIT_RECV(m_stat)) return -1;					// ���̃C���X�^���X�͑O��RECV�����s��
	if (!STAT_ACTIVE(m_stat)) return -2;					// �q�X���b�h���Ƃ܂��Ă���
	// �Z�b�g
	m_pRecvData = (char*)pData;
	m_nRecvSz = len;
	m_evRecvEnd = evEvent;									// ��M�������ɒʒm����V�O�i�� (NULL���̓��b�Z�[�W�ʒm�ƂȂ�)
	// ��M�J�n�˗�
//	_RPT1(_CRT_WARN, "OVL Recv event %p\n", this);	
	SetEvent(m_evRecvStart);								// ���C���X���b�h�Ɏ�M�J�n��ʒm
	DWORD ret = WaitForSingleObject(m_evRecvAck, 3000);		// ���C���X���b�h����M�J�n������������܂Ńu���b�N
	if (WAIT_TIMEOUT == ret) return -3;
	return 0;
}

//------------------------------------------
// ���M�J�n���� (���C���X���b�h�̑��M�J�n��������������܂Ńu���b�N)
// LPVOID pData ���M�̈�
// DWORD len ���M����T�C�Y
// HANDLE evEvent ���M�������ɒʒm����V�O�i��
//------------------------------------------
int ksThread::Send(LPCVOID pData, DWORD len)
{
	return Send(pData, len, NULL);
}
int ksThread::Send(LPCVOID pData, DWORD len, HANDLE evEvent)
{
	if (STAT_WAIT_SEND(m_stat)) return -1;	// ���̃C���X�^���X�͑O��SEND�����s��
	if (!STAT_ACTIVE(m_stat)) return -2; // �q�X���b�h���Ƃ܂��Ă���
	// �Z�b�g
	m_pSendData = (char const*)pData;
	m_nSendSz = len;
	m_evSendEnd = evEvent;									// ���M�������ɒʒm����V�O�i�� (NULL���̓��b�Z�[�W�ʒm�ƂȂ�)
	// ���M�J�n�˗�
	SetEvent(m_evSendStart);								// ���C���X���b�h�ɑ��M�J�n��ʒm
	DWORD ret = WaitForSingleObject(m_evSendAck, 3000);		// ���C���X���b�h�����M�J�n������������܂Ńu���b�N
	if (WAIT_TIMEOUT == ret) return -3;
	return 0;
}

//------------------------------------------
// ����M�҂��������
//------------------------------------------
void ksThread::Cancel()
{
	SetEvent(m_evCancelEvent);
	// �X���b�h�ُ�I�����Ă���ꍇ�� �����ɏI��� (���M���A��M���� �N���[�Y�ʒm�������ꍇ�A���Ŗ������[�v�ƂȂ�ׂ̏��u)
	if(STAT_THREAD_ERR(m_stat)) return;
	// ����M�Ƃ�������܂ő҂�
	while (STAT_WAIT_SEND(m_stat) || STAT_WAIT_RECV(m_stat)) {
		Sleep(50);
	}
}
//------------------------------------------
// �^�C���A�E�g�Đݒ�
// DWORD timeout �^�C���A�E�g�Ԋu [ms]
//------------------------------------------
void ksThread::SetTimeout(DWORD timeout)
{
	m_nTimeout = timeout;
	SetEvent(m_evSetTimeout);			// ���C���X���b�h�ɒʒm���āA��x���s�����āA�V�K���Ԃł܂��҂��ɂ���B
}

//------------------------------------------
// ���b�Z�[�W�E�C�x���g �ʒm
// UINT msg �����敪 (WM_KS_�Ȃ񂿂��)
// WPARAM wParam (���b�Z�[�W�ʒm���̂�)
// LPARAM lParam (���b�Z�[�W�ʒm���̂�)
//------------------------------------------
void ksThread::sendMsg(UINT msg, WPARAM wParam, LPARAM lParam)
{
	if( msg == WM_KS_RECV && m_evRecvEnd != NULL ) {						// ��M���� (�V�O�i���ʒm)
		SetEvent(m_evRecvEnd);


	} else if( msg == WM_KS_SEND && m_evSendEnd != NULL ) {					// ���M���� (�V�O�i���ʒm)
		SetEvent(m_evSendEnd);

	} else if( msg == WM_KS_RECV_TIMEOUT && m_evRecvTimeout != NULL ) {		// ��M�^�C���A�E�g (�V�O�i���ʒm)
		SetEvent(m_evRecvTimeout);

	} else if( msg == WM_KS_SEND_TIMEOUT && m_evSendTimeout != NULL ) {		// ���M�^�C���A�E�g (�V�O�i���ʒm)
		SetEvent(m_evSendTimeout);

	} else if( msg == WM_KS_DISCONNECT && m_evDisConnect != NULL ) {		// �ؒf (�V�O�i���ʒm)
		SetEvent(m_evDisConnect);

	} else if (0 != m_nTid) {												// ���b�Z�[�W�ʒm
		// WM_KS_RECV,			���C���X�^���X�l,		0�Œ�
		// WM_KS_RECV_TIMEOUT,	���C���X�^���X�l,		0�Œ�
		// WM_KS_SEND_TIMEOUT,	���C���X�^���X�l,		0�Œ�
		// WM_KS_DISCONNECT,	���C���X�^���X�l,		(0:���肩��ؒf ����ȊO:�����ŋ����ؒf)
		PostThreadMessage(m_nTid, m_nMsgBase+msg, wParam, lParam);			
	}
}

//
// �X���b�h�{��
//

//------------------------------------------
// ���C���X���b�h
//------------------------------------------
unsigned int __stdcall ksThread::MainThread(LPVOID arg)
{
	// ���C���X���b�h��������
	ksThread * p = (ksThread *)arg;
	SET_ACTIVE(p->m_stat);

	// ��������p�C�x���g
	OVERLAPPED ovlSend, ovlRecv;
	memset(&ovlSend, 0, sizeof(OVERLAPPED));
	memset(&ovlRecv, 0, sizeof(OVERLAPPED));
	ovlSend.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	ovlRecv.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	HANDLE evCloseEvent = CreateEvent(NULL, TRUE, FALSE, NULL);		// �\�P�b�g�̃N���[�Y�V�O�i��
	WSAEventSelect((SOCKET)p->m_sock, evCloseEvent, FD_CLOSE);		// CLOSE��ʒm�\�ɂ���
	p->m_nCloseKind = KIZUSOCK_CLOSEKIND_NON;

	DWORD recvSize, sendSize, recvTotal, sendTotal, restRecv, restSend;
	DWORD recvStartTick, sendStartTick;
	
	// �C�x���g�쐬
	HANDLE evArray[8] = {p->m_evStopEvent, p->m_evSetTimeout, p->m_evCancelEvent,
						 p->m_evRecvStart, p->m_evSendStart,
						 ovlRecv.hEvent, ovlSend.hEvent, evCloseEvent};
	enum { EV_STOP = WAIT_OBJECT_0,			// �I���ʒm
		   EV_SETTIMEOUT,					// �^�C���A�E�g���ԍĐݒ�
		   EV_CANCEL,						// ����M�L�����Z��
		   EV_RECV_START,					// ��M�J�n�˗�
		   EV_SEND_START,					// ���M�J�n�˗�
		   EV_OVL_RECV,						// �񓯊���M����
		   EV_OVL_SEND,						// �񓯊����M����
		   EV_CLOSE,						// �ؒf�ʒm
		   EV_END};

	while (true) {
		DWORD dwWait = WaitForMultipleObjects(EV_END, evArray, FALSE, p->m_nTimeout);
		DWORD evNum = dwWait - WAIT_OBJECT_0;

#ifdef _DEBUG
		char msg[256];
		if (WAIT_TIMEOUT == dwWait) wsprintf(msg, "event timeout\n");
		if (EV_STOP == evNum) wsprintf(msg, "event stop\n");
		if (EV_SETTIMEOUT == evNum) wsprintf(msg, "event settimeout\n");
		if (EV_CANCEL == evNum) wsprintf(msg, "event cancel\n");
		if (EV_RECV_START == evNum) wsprintf(msg, "event recv\n");
		if (EV_SEND_START == evNum) wsprintf(msg, "event send\n");
		if (EV_OVL_RECV == evNum) wsprintf(msg, "event ovl recv\n");
		if (EV_OVL_SEND == evNum) wsprintf(msg, "event ovl send\n");
		if (EV_CLOSE == evNum) wsprintf(msg, "event close\n");
		//		_RPT0(_CRT_WARN, msg);
#endif

		// ��~�͍ŗD�掖��
		if (evNum == EV_STOP) {	
			CancelIo(p->m_sock);
			RST_WAIT_RECV(p->m_stat);
			RST_WAIT_SEND(p->m_stat);
			shutdown((SOCKET)p->m_sock, SD_BOTH);
			p->m_nCloseKind = KIZUSOCK_CLOSEKIND_THEAD;
			CloseHandle(ovlSend.hEvent);
			CloseHandle(ovlRecv.hEvent);
			CloseHandle(evCloseEvent);
			RST_ACTIVE(p->m_stat);				
			return THR_NORMAL_END;					// �����~
		}

		// ��M�^�C���A�E�g�`�F�b�N
		if (STAT_WAIT_RECV(p->m_stat) && WAIT_TIMEOUT == dwWait) {	// �ǂݑ҂��̂Ƃ�
			if (GetTickCount() < 0x80000000 && recvStartTick > 0x80000000) { // ���b�v�A���E���h�ł͂�����x�҂�
				recvStartTick = GetTickCount();
			} else if ((GetTickCount() - recvStartTick) >= p->m_nTimeout) {
				SET_RECV_TIMEOUT(p->m_stat);
				p->sendMsg(WM_KS_RECV_TIMEOUT, (WPARAM)p, 0);	// ���C���X�^���X��Ԃ�
			}
		}
		// ���M�^�C���A�E�g�`�F�b�N
		if (STAT_WAIT_SEND(p->m_stat) && WAIT_TIMEOUT == dwWait) {	// �����҂��̂Ƃ�
			if (GetTickCount() < 0x80000000 && sendStartTick > 0x80000000) { // ���b�v�A���E���h�ł͂�����x�҂�
				sendStartTick = GetTickCount();
			} else if ((GetTickCount() - sendStartTick) >= p->m_nTimeout) {
				SET_SEND_TIMEOUT(p->m_stat);
				p->sendMsg(WM_KS_SEND_TIMEOUT, (WPARAM)p, 0);	// ���C���X�^���X��Ԃ�
			}
		}

		// �^�C���A�E�g ���́A �^�C���A�E�g���ԍĐݒ� �̏ꍇ�́A�����ŏI���
		if (WAIT_TIMEOUT == dwWait || evNum == EV_SETTIMEOUT) continue;


		switch (evNum) {
		case EV_CANCEL:							// CancelIo
			CancelIo(p->m_sock);
			RST_WAIT_RECV(p->m_stat);
			RST_WAIT_SEND(p->m_stat);
//_RPT1(_CRT_WARN, "Cancel %p\n", p);
			break;
			
		case EV_RECV_START:						// �񓯊��ǂݎ����J�n����
			SET_WAIT_RECV(p->m_stat);
			if (!ReadFile(p->m_sock, p->m_pRecvData, p->m_nRecvSz, NULL, &ovlRecv)) {
				p->m_apierr = GetLastError();			// �����s�̏ꍇ
				if (p->m_apierr != ERROR_IO_PENDING) goto read_abort;
			}
			recvStartTick = GetTickCount();				// �����J�n���ԃZ�b�g (�^�C���A�E�g�`�F�b�N�Ɏg�p)
			recvTotal = 0;
			SetEvent(p->m_evRecvAck);					// ��M�J�n��������
//_RPT2(_CRT_WARN, "RecvStart %p size=%d\n", p, p->m_nRecvSz);
			break;
			
		case EV_SEND_START:						// �񓯊����M���J�n����
			SET_WAIT_SEND(p->m_stat);
			if (!WriteFile(p->m_sock, p->m_pSendData, p->m_nSendSz, NULL, &ovlSend)) {
				p->m_apierr = GetLastError();			// �����s�̏ꍇ
				if (p->m_apierr != ERROR_IO_PENDING) goto send_abort;
			}
			sendStartTick = GetTickCount();				// �����J�n���ԃZ�b�g (�^�C���A�E�g�`�F�b�N�Ɏg�p)
			sendTotal = 0;
			SetEvent(p->m_evSendAck);					// ���M�J�n��������
//_RPT2(_CRT_WARN, "SendStart %p size=%d\n", p, p->m_nSendSz);
			break;
			
		case EV_OVL_RECV:						// �񓯊��ǂݎ��̊���
			if (GetOverlappedResult(p->m_sock, &ovlRecv, &recvSize, TRUE)) {	// �Ȃ���TRUE�łȂ��Ƃ����Ȃ�
				if (0 == recvSize) { // �\�P�b�g�������ꍇ
//_RPT0(_CRT_WARN, "socket closed gracefully\n");
					p->m_apierr = 0;
					goto read_abort;
				}
				recvStartTick = GetTickCount();			// ���擾�ł����̂ōēx�V�������Ԃ��Z�b�g (�^�C���A�E�g�`�F�b�N�Ɏg�p)
				// ����Ƀ��[�h
				recvTotal += recvSize;
				if (recvTotal == p->m_nRecvSz) {
//_RPT1(_CRT_WARN, "RecvEnd %d\n", recvTotal);
					// �f�[�^�ǂݍ��݊���
					ResetEvent(ovlRecv.hEvent);
					RST_RECV_TIMEOUT(p->m_stat);
					RST_WAIT_RECV(p->m_stat);	// �X�e�[�^�X�~�낷

					// �ʒm
					p->sendMsg(WM_KS_RECV, (WPARAM)p, 0);

					break;
				}
				// ���̃��[�h���s (���Ŏ�荞�߂Ȃ��ꍇ�B�w��T�C�Y��荞�ނ܂ŌJ��Ԃ�)
				restRecv = p->m_nRecvSz - recvTotal;
//_RPT2(_CRT_WARN, "Recving��� now=%d(zan=%d)\n", recvTotal, restRecv);
				if (!ReadFile(p->m_sock, p->m_pRecvData+recvTotal, restRecv, NULL, &ovlRecv)) {
					p->m_apierr = GetLastError();	// �����s�̏ꍇ
					if (p->m_apierr != ERROR_IO_PENDING) goto read_abort;
				}
				break;

			} else {									// ���[�h�G���[����
				p->m_apierr = GetLastError();
read_abort :
				if (ERROR_OPERATION_ABORTED == p->m_apierr) {
					ResetEvent(ovlRecv.hEvent);
					_RPT0(_CRT_WARN, "read abort\n");
					p->m_nCloseKind = KIZUSOCK_CLOSEKIND_CANCEL;
					break;			// CancelIo�𔭍s�����Ƃ�
				}
				_RPT0(_CRT_WARN, "socket read error\n");
#ifdef _DEBUG
				LPVOID lpMsgBuf;
				FormatMessage( FORMAT_MESSAGE_ALLOCATE_BUFFER |
					FORMAT_MESSAGE_FROM_SYSTEM |
					FORMAT_MESSAGE_IGNORE_INSERTS,
					NULL,
					p->m_apierr,
					MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // ����̌���
					(LPTSTR) &lpMsgBuf,	0, NULL);
				_RPT0(_CRT_WARN, (const char *)lpMsgBuf);
				LocalFree(lpMsgBuf);
#endif
				CancelIo(p->m_sock);
				RST_WAIT_RECV(p->m_stat);
				RST_WAIT_SEND(p->m_stat);
				shutdown((SOCKET)p->m_sock, SD_BOTH);
				p->m_nCloseKind = KIZUSOCK_CLOSEKIND_SOCK;	// ��M�҂��͑��肩��̐ؒf
				CloseHandle(ovlSend.hEvent);
				CloseHandle(ovlRecv.hEvent);
				CloseHandle(evCloseEvent);
				SET_THREAD_ERR(p->m_stat);
				RST_ACTIVE(p->m_stat);
				p->sendMsg(WM_KS_DISCONNECT, (WPARAM)p, p->m_apierr);			// �ُ�ؒf�ʒm
				return THR_RECV_ERR;											// ���C���X���b�h ��M�ُ�ɂ��I��
			}
			
		case EV_OVL_SEND:						// �񓯊��������݂̊���
//				_RPT0(_CRT_WARN, "event ovl send\n");
			if (GetOverlappedResult(p->m_sock, &ovlSend, &sendSize, TRUE)) {
				// ����ɑ��M
				sendStartTick = GetTickCount();			// ���擾�ł����̂ōēx�V�������Ԃ��Z�b�g (�^�C���A�E�g�`�F�b�N�Ɏg�p)
				sendTotal += sendSize;
				if (sendTotal == p->m_nSendSz) {
//_RPT1(_CRT_WARN, "SendEnd %d\n", sendTotal);
					// �f�[�^�������݊���
					ResetEvent(ovlSend.hEvent);
					RST_SEND_TIMEOUT(p->m_stat);
					RST_WAIT_SEND(p->m_stat); // �X�e�[�^�X�~�낷
					// �ʒm
					p->sendMsg(WM_KS_SEND, (WPARAM)p, 0);
					break;
				}
				// ���̑��M���s (���ő���Ȃ��ꍇ�B�w��T�C�Y�����܂ŌJ��Ԃ�)
				restSend = p->m_nSendSz	- sendTotal;
//_RPT2(_CRT_WARN, "Sending��� now=%d(zan=%d)\n", sendTotal, restSend);
				if (!WriteFile(p->m_sock, p->m_pSendData+sendTotal, restSend, NULL, &ovlSend)) {
					p->m_apierr = GetLastError();	// �����s�̏ꍇ
					if (p->m_apierr != ERROR_IO_PENDING) goto send_abort;
				}
				break;
			} else {  // ���M�G���[����
				p->m_apierr = GetLastError();
send_abort:
				if (ERROR_OPERATION_ABORTED == p->m_apierr) {
					ResetEvent(ovlSend.hEvent);
					_RPT0(_CRT_WARN, "send abort\n");
					p->m_nCloseKind = KIZUSOCK_CLOSEKIND_CANCEL;
					break; // CancelIo�𔭍s�����Ƃ�
				}
				_RPT0(_CRT_WARN, "socket send error\n");
#ifdef _DEBUG
				LPVOID lpMsgBuf;
				FormatMessage( FORMAT_MESSAGE_ALLOCATE_BUFFER |
					FORMAT_MESSAGE_FROM_SYSTEM |
					FORMAT_MESSAGE_IGNORE_INSERTS,
					NULL,
					p->m_apierr,
					MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // ����̌���
					(LPTSTR) &lpMsgBuf,	0, NULL);
				_RPT0(_CRT_WARN, (const char *)lpMsgBuf);
				LocalFree(lpMsgBuf);
#endif
				CancelIo(p->m_sock);
				RST_WAIT_RECV(p->m_stat);
				RST_WAIT_SEND(p->m_stat);
				shutdown((SOCKET)p->m_sock, SD_BOTH);
				p->m_nCloseKind = KIZUSOCK_CLOSEKIND_PROG;
				CloseHandle(ovlSend.hEvent);
				CloseHandle(ovlRecv.hEvent);
				CloseHandle(evCloseEvent);
				SET_THREAD_ERR(p->m_stat);
				RST_ACTIVE(p->m_stat);
				p->sendMsg(WM_KS_DISCONNECT, (WPARAM)p, p->m_apierr);			// �ُ�ؒf�ʒm
				return THR_SEND_ERR;											// ���C���X���b�h ���M�ُ�ɂ��I��
			}

		case EV_CLOSE :							// ���������ꂽ�ꍇ
//			_RPT0(_CRT_WARN, "socket close\n");
			CancelIo(p->m_sock);
			RST_WAIT_RECV(p->m_stat);
			RST_WAIT_SEND(p->m_stat);
			shutdown((SOCKET)p->m_sock, SD_BOTH);
			p->m_nCloseKind = KIZUSOCK_CLOSEKIND_SOCK;
			CloseHandle(ovlSend.hEvent);
			CloseHandle(ovlRecv.hEvent);
			CloseHandle(evCloseEvent);
			SET_THREAD_ERR(p->m_stat);
			RST_ACTIVE(p->m_stat);
			p->sendMsg(WM_KS_DISCONNECT, (WPARAM)p, 0);							// ����ؒf�ʒm

			return THR_CLOSE_END;												// ���C���X���b�h ���肩��̐ؒf�ɂ��I��

		default:	// ���肦�Ȃ�
			_ASSERT(0);
		}
	}
}
