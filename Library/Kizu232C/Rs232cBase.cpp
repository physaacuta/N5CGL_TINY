#include "StdAfx.h"
#include "Rs232cBase.h"
#include <crtdbg.h>

#include <process.h>
#include <stdio.h>


using namespace Kizu232C;

//------------------------------------------
// �R���X�g���N�^
// const char *cSession ini�t�@�C�� �Z�b�V����
// const char *cIniPath ini�t�@�C�� �p�X
// DWORD tid PostThreadMessage�̑���� (ThreadID��n���Ηǂ�)
// DWORD r_size 232C�̎�M�o�b�t�@�T�C�Y
// DWORD s_size 232C�̑��M�o�b�t�@�T�C�Y
// DWORD msgbase ���b�Z�[�W�x�[�X�ԍ�
// bool bSoftFolwOff �\�t�g�t���[����𖳌���true = ��������ڑ�����Ă��Ȃ�RS-232C�P�[�u�����g�p���ɗL���Ƃ���
//------------------------------------------
Rs232cBase::Rs232cBase(const char *cSession, const char *cIniPath, DWORD tid,
					   DWORD r_size, DWORD s_size, DWORD msgbase, bool bSoftFolwOff) :
m_nTid(tid),
m_nMsgBase(msgbase),
m_hCom(INVALID_HANDLE_VALUE),
m_hThread(NULL),
m_nTimeout(INFINITE),
m_evStopEvent(NULL),
m_evSetTimeout(NULL),
m_evCancelEvent(NULL),
m_evRecvStart(NULL),
m_evSendStart(NULL),
m_evRecvAck(NULL),
m_evSendAck(NULL),
m_evRecvEnd(NULL),
m_evSendEnd(NULL),
m_cRecvCR(NULL),
m_stat(0),
m_apierr(0)
{
	DCB	dcb;					// DCB�\����
								// �ʐM�p�����[�^
	char cComName[12], cBaud[12], cParity[12], cData[12], cStop[12], cTo[12], cDtr[12], cRts[12], cIdsr[12], cOcts[12], cParam[128];
	COMMTIMEOUTS CommTimeOuts;	// COMMTIMEOUTS�\����
	
	// COM�|�[�g�擾
	GetPrivateProfileString(cSession, "com", "",	cComName, sizeof(cComName), cIniPath);
	if (strlen(cComName) == 0) {
		throw -8;				// COM�|�[�g�w�肪�Ȃ�
	}
	// �ʐM�|�[�g���I�[�v������
	m_hCom = CreateFile(cComName, GENERIC_READ|GENERIC_WRITE , NULL,
						NULL, OPEN_EXISTING,
						FILE_ATTRIBUTE_NORMAL | FILE_FLAG_OVERLAPPED,
						NULL);
	if(INVALID_HANDLE_VALUE == m_hCom) throw -1;

	// �o�b�t�@�T�C�Y���w�� 
	if( !SetupComm(m_hCom, r_size, s_size) ) {
		CloseHandle(m_hCom);
		m_hCom = INVALID_HANDLE_VALUE;
		throw -2;
	}

	// �ʐM�p�����[�^��ݒ肷��
	GetCommState(m_hCom, &dcb);		// ���݂̐���ݒ���f�o�C�X����u���b�N�iDCB �\���́j�Ɋi�[
	
	GetPrivateProfileString(cSession, "baud",	"", cBaud,	 sizeof(cBaud),	  cIniPath);		// �{�[���[�g�擾
	GetPrivateProfileString(cSession, "parity", "", cParity, sizeof(cParity), cIniPath);		// �p���e�B�ݒ�擾
	GetPrivateProfileString(cSession, "data",	"", cData,	 sizeof(cData),   cIniPath);		// �f�[�^�r�b�g�擾
	GetPrivateProfileString(cSession, "stop",	"", cStop,   sizeof(cStop),   cIniPath);		// �X�g�b�v�r�b�g�擾
	GetPrivateProfileString(cSession, "to",		"", cTo,	 sizeof(cTo),	  cIniPath);		// �����^�C���A�E�g�����擾
	GetPrivateProfileString(cSession, "dtr",	"", cDtr,	 sizeof(cDtr),	  cIniPath);		// �f�[�^ �^�[�~�i�� ���f�B (DTR) �擾
	GetPrivateProfileString(cSession, "rts",	"", cRts,	 sizeof(cRts),	  cIniPath);		// ���M�v�� (RTS) �擾
	GetPrivateProfileString(cSession, "idsr",	"", cIdsr,	 sizeof(cIdsr),   cIniPath);		// DSR ����擾
	GetPrivateProfileString(cSession, "octs",	"", cOcts,	 sizeof(cOcts),   cIniPath);		// ���M�� (CTS) ����擾

	if( strlen(cBaud) == 0 || strlen(cParity) == 0 ||  strlen(cData) == 0 || strlen(cStop) == 0
		|| strlen(cTo) == 0 || strlen(cDtr) == 0 ||  strlen(cRts) == 0 || strlen(cIdsr) == 0 || strlen(cOcts) == 0) {
		CloseHandle(m_hCom);
		m_hCom = INVALID_HANDLE_VALUE;
		throw -7;	// INI�t�@�C�����ݒ肳��Ă��Ȃ�
	}

	// �ݒ蕶����쐬
	sprintf(cParam, "baud=%s parity=%s data=%s stop=%s to=%s octs=%s dtr=%s rts=%s idsr=%s",
		cBaud, cParity, cData, cStop, cTo, cOcts, cDtr, cRts, cIdsr);
									
	if( !BuildCommDCBAndTimeouts(cParam, &dcb, &CommTimeOuts) ) {	// �f�o�C�X����u���b�N�Ɋi�[
		CloseHandle(m_hCom);
		m_hCom = INVALID_HANDLE_VALUE;
		throw -3;	
	}
	CommTimeOuts.ReadIntervalTimeout = 0;
	CommTimeOuts.ReadTotalTimeoutMultiplier = 0;
	CommTimeOuts.ReadTotalTimeoutConstant = 0;
	CommTimeOuts.WriteTotalTimeoutConstant = 0;
	CommTimeOuts.WriteTotalTimeoutMultiplier = 0;

	// �^�C���A�E�g�p�����[�^��ݒ�
	if( !SetCommTimeouts(m_hCom, &CommTimeOuts) ) {	
		CloseHandle(m_hCom);
		m_hCom = INVALID_HANDLE_VALUE;
		throw -4;	
	}	

	// �\�t�g�t���[�����Ή� (�J�����R���g���[���[ ��������z���p�΍���)
	if(bSoftFolwOff) {
		dcb.fOutxCtsFlow = 0;
		dcb.fOutxDsrFlow  = 0;
		dcb.fInX = 0;
		dcb.fOutX = 0;
	} else {
		// �f�t�H���g�ł́A�\�t�g�t���[����͂��ׂėL����Ԃׁ̈A�������Ȃ�
	}

	// �f�o�C�X����u���b�N�iDCB �\���́j�̎w��ɏ]���ĒʐM�f�o�C�X���\��
	dcb.fAbortOnError = 0;
	if( !SetCommState(m_hCom, &dcb) ) {	
		CloseHandle(m_hCom);
		m_hCom = INVALID_HANDLE_VALUE;
		throw -5;	
	}

	// �o�b�t�@���N���A
	PurgeComm(m_hCom, PURGE_TXABORT | PURGE_RXABORT | PURGE_TXCLEAR | PURGE_RXCLEAR);

	// �X���b�h�C�x���g����
	m_evStopEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	m_evSetTimeout = CreateEvent(NULL, FALSE, FALSE, NULL);
	m_evCancelEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	m_evRecvStart = CreateEvent(NULL, FALSE, FALSE, NULL);
	m_evSendStart = CreateEvent(NULL, FALSE, FALSE, NULL);
	m_evRecvAck = CreateEvent(NULL, FALSE, FALSE, NULL);
	m_evSendAck = CreateEvent(NULL, FALSE, FALSE, NULL);

	// ���C���X���b�h���J�n
	UINT uiTid;					// �X���b�hID
	m_hThread = (HANDLE)_beginthreadex(NULL, 0, MainThread, (LPVOID)this, 0, &uiTid);
	if (NULL == m_hThread) {
		CloseHandle(m_hCom);
		m_hCom = INVALID_HANDLE_VALUE;
		throw -6;
	}

	// �X�e�[�^�X�C�j�V����
	m_stat = KIZU232C_STAT_ACTIVE;
}

//------------------------------------------
// �f�X�g���N�^
//------------------------------------------
Rs232cBase::~Rs232cBase(void)
{
	// �X���b�h���I��
	if (NULL != m_hThread) {
		DWORD exitCode;
		GetExitCodeThread(m_hThread, &exitCode);
		if (STILL_ACTIVE == exitCode) {
			SetEvent(m_evStopEvent);
			DWORD dwRet = WaitForSingleObject(m_hThread, 5000);
			//_ASSERT(dwRet == WAIT_OBJECT_0);
			if (dwRet == WAIT_TIMEOUT) {
				TerminateThread(m_hThread, 0);
			}
		}
		CloseHandle(m_hThread);
		m_hThread = NULL;
	}
	// �X���b�h�C�x���g���
	CloseHandle(m_evStopEvent);
	CloseHandle(m_evSetTimeout);
	CloseHandle(m_evCancelEvent);
	CloseHandle(m_evRecvStart);
	CloseHandle(m_evSendStart);
	CloseHandle(m_evRecvAck);
	CloseHandle(m_evSendAck);

	// �ʐM�|�[�g���N���[�Y����
	if (m_hCom != INVALID_HANDLE_VALUE) {
		CloseHandle(m_hCom);
		m_hCom = INVALID_HANDLE_VALUE;
	}
}

//------------------------------------------
// �ʐM�G���[�̏��m�F
// �߂�l ���̓o�b�t�@�Ɋi�[����Ă���f�[�^�̃o�C�g�� (�悤�� 0�Ȃ琳��)
//------------------------------------------
DWORD Rs232cBase::GetQueueLen() const
{
	COMSTAT lpCstat;	// COMSTAT�\����
	DWORD dwErrflag;	// �G���[�R�[�h���󂯎��ϐ�

	// �ʐM�G���[�̏����擾
	ClearCommError(m_hCom, &dwErrflag, &lpCstat);
	return(lpCstat.cbInQue);
}

//------------------------------------------
// ��M�J�n���� (���C���X���b�h�̎�M�J�n��������������܂Ńu���b�N)
// LPVOID pData ��M�̈�
// DWORD len ��M����T�C�Y
// HANDLE evEvent ��M�������ɒʒm����V�O�i��
//------------------------------------------
int Rs232cBase::Recv(LPVOID pData, DWORD len)
{
	return Recv(pData, len, NULL);
}
int Rs232cBase::Recv(LPVOID pData, DWORD len, HANDLE evEvent)
{
	if (m_hCom == INVALID_HANDLE_VALUE) return -1;	// �ʐM�|�[�g���N���[�Y����Ă���
	if (STAT_WAIT_RECV(m_stat)) return -2;	// �O�̃��[�h�����s��
	if (!STAT_ACTIVE(m_stat)) return -3;	// �q�X���b�h���Ƃ܂��Ă���

	// �Z�b�g
	m_pRecvData = (char*)pData;
	m_nRecvSz = len;
	m_evRecvEnd  = evEvent;									// ��M�������ɒʒm����V�O�i�� (NULL���̓��b�Z�[�W�ʒm�ƂȂ�)
	// ��M�J�n�˗�
	SetEvent(m_evRecvStart);								// ���C���X���b�h�Ɏ�M�J�n��ʒm
	DWORD ret = WaitForSingleObject(m_evRecvAck, 1000);		// ���C���X���b�h����M�J�n������������܂Ńu���b�N
	if (WAIT_TIMEOUT == ret) return -4;
	return 0;
}

//------------------------------------------
// ���M�J�n���� (���C���X���b�h�̑��M�J�n��������������܂Ńu���b�N)
// LPVOID pData ���M�̈�
// DWORD len ���M����T�C�Y
// HANDLE evEvent ���M�������ɒʒm����V�O�i��
//------------------------------------------
int Rs232cBase::Send(LPCVOID pData, DWORD len)
{
	return Send(pData, len, NULL);
}
int Rs232cBase::Send(LPCVOID pData, DWORD len, HANDLE evEvent)
{
	if (m_hCom == INVALID_HANDLE_VALUE) return -1;	// �ʐM�|�[�g���N���[�Y����Ă���
	if (STAT_WAIT_SEND(m_stat)) return -2;	// �O�̃��C�g�����s��
	if (!STAT_ACTIVE(m_stat)) return -3;	// �q�X���b�h���Ƃ܂��Ă���

	// �Z�b�g
	m_pSendData = (char const*)pData;
	m_nSendSz = len;
	m_evSendEnd = evEvent;									// ���M�������ɒʒm����V�O�i�� (NULL���̓��b�Z�[�W�ʒm�ƂȂ�)
	// ���M�J�n�˗�
	SetEvent(m_evSendStart);								// ���C���X���b�h�ɑ��M�J�n��ʒm
	DWORD ret = WaitForSingleObject(m_evSendAck, 3000);		// ���C���X���b�h�����M�J�n������������܂Ńu���b�N
	if (WAIT_TIMEOUT == ret) return -4;
	return 0;
}

//------------------------------------------
// ����M�҂��������
//------------------------------------------
void Rs232cBase::Cancel()
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
void Rs232cBase::SetTimeout(DWORD timeout)
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
void Rs232cBase::sendMsg(UINT msg, WPARAM wParam, LPARAM lParam)
{
	if( msg == WM_KS232C_RECV && m_evRecvEnd != NULL ) {					// ��M���� (�V�O�i���ʒm)
		SetEvent(m_evRecvEnd);

	} else if( msg == WM_KS232C_SEND && m_evSendEnd != NULL ) {				// ���M���� (�V�O�i���ʒm)
		SetEvent(m_evSendEnd);

	} else if (0 != m_nTid) {												// ���b�Z�[�W�ʒm
		// WM_KS232C_RECV,		���C���X�^���X�l,		0�Œ�
		// WM_KS232C_SEND,		���C���X�^���X�l,		0�Œ�

		// WM_KS232C_ERR,		���C���X�^���X�l,		�ُ�R�[�h (m_apierr)
		PostThreadMessage(m_nTid, m_nMsgBase+msg, wParam, lParam);			
	}
}



//
// �X���b�h�{��
//
//------------------------------------------
// ���C���X���b�h
//------------------------------------------
unsigned int __stdcall Rs232cBase::MainThread(LPVOID arg)
{
	// ���C���X���b�h��������
	Rs232cBase * p = (Rs232cBase *)arg;
	SET_ACTIVE(p->m_stat);

	// ��������p�C�x���g
	OVERLAPPED ovlSend, ovlRecv;
	memset(&ovlSend, 0, sizeof(OVERLAPPED));
	memset(&ovlRecv, 0, sizeof(OVERLAPPED));
	ovlSend.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	ovlRecv.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);

	DWORD recvSize, sendSize, recvTotal, sendTotal, restRecv, restSend;
	DWORD recvStartTick, sendStartTick;
	
	// �C�x���g�쐬
	HANDLE evArray[7] = {p->m_evStopEvent, p->m_evSetTimeout, p->m_evCancelEvent,
						 p->m_evRecvStart, p->m_evSendStart,
						 ovlRecv.hEvent, ovlSend.hEvent};
	enum { EV_STOP = WAIT_OBJECT_0,			// �I���ʒm
		   EV_SETTIMEOUT,					// �^�C���A�E�g���ԍĐݒ�
		   EV_CANCEL,						// ����M�L�����Z��
		   EV_RECV_START,					// ��M�J�n�˗�
		   EV_SEND_START,					// ���M�J�n�˗�
		   EV_OVL_RECV,						// �񓯊���M����
		   EV_OVL_SEND,						// �񓯊����M����
		   EV_END};

	while (true) {
		DWORD dwWait = WaitForMultipleObjects(EV_END, evArray, FALSE, p->m_nTimeout);
		DWORD evNum = dwWait - WAIT_OBJECT_0;

		// ��~�͍ŗD�掖��
		if (evNum == EV_STOP) {	
			CancelIo(p->m_hCom);
			CloseHandle(ovlSend.hEvent);
			CloseHandle(ovlRecv.hEvent);
			RST_ACTIVE(p->m_stat);				
			return 0;							// �����~
		}

		// ��M�^�C���A�E�g�`�F�b�N
		if (STAT_WAIT_RECV(p->m_stat) && WAIT_TIMEOUT == dwWait) {	// �ǂݑ҂��̂Ƃ�
			if (GetTickCount() < 0x80000000 && recvStartTick > 0x80000000) { // ���b�v�A���E���h�ł͂�����x�҂�
				recvStartTick = GetTickCount();
			} else if ((GetTickCount() - recvStartTick) >= p->m_nTimeout) {
				SET_RECV_TIMEOUT(p->m_stat);
				p->sendMsg(WM_KS232C_RECV_TIMEOUT, (WPARAM)p, 0);	// ���C���X�^���X��Ԃ�
			}
		}
		// ���M�^�C���A�E�g�`�F�b�N
		if (STAT_WAIT_SEND(p->m_stat) && WAIT_TIMEOUT == dwWait) {	// �����҂��̂Ƃ�
			if (GetTickCount() < 0x80000000 && sendStartTick > 0x80000000) { // ���b�v�A���E���h�ł͂�����x�҂�
				sendStartTick = GetTickCount();
			} else if ((GetTickCount() - sendStartTick) >= p->m_nTimeout) {
				SET_SEND_TIMEOUT(p->m_stat);
				p->sendMsg(WM_KS232C_SEND_TIMEOUT, (WPARAM)p, 0);	// ���C���X�^���X��Ԃ�
			}
		}

		// �^�C���A�E�g ���́A �^�C���A�E�g���ԍĐݒ� �̏ꍇ�́A�����ŏI���
		if (WAIT_TIMEOUT == dwWait || evNum == EV_SETTIMEOUT) continue;


		switch (evNum) {
		case EV_CANCEL:							// CancelIo
			RST_WAIT_RECV(p->m_stat);
			RST_WAIT_SEND(p->m_stat);
			break;
			
		case EV_RECV_START:						// �񓯊��ǂݎ����J�n����
			SET_WAIT_RECV(p->m_stat);
			if (!ReadFile(p->m_hCom, p->m_pRecvData, p->m_nRecvSz, NULL, &ovlRecv)) {
				p->m_apierr = GetLastError();			// �����s�̏ꍇ
				if (p->m_apierr != ERROR_IO_PENDING) goto read_abort;
			}
			recvStartTick = GetTickCount();				// �����J�n���ԃZ�b�g (�^�C���A�E�g�`�F�b�N�Ɏg�p)
			recvTotal = 0;
			SetEvent(p->m_evRecvAck);					// ��M�J�n��������
//_RPT1(_CRT_WARN, "RecvStart %p   ------>>>\n", p);
			break;
			
		case EV_SEND_START:						// �񓯊����M���J�n����
			SET_WAIT_SEND(p->m_stat);
			if (!WriteFile(p->m_hCom, p->m_pSendData, p->m_nSendSz, NULL, &ovlSend)) {
				p->m_apierr = GetLastError();			// �����s�̏ꍇ
				if (p->m_apierr != ERROR_IO_PENDING) goto send_abort;
			}
			sendStartTick = GetTickCount();				// �����J�n���ԃZ�b�g (�^�C���A�E�g�`�F�b�N�Ɏg�p)
			sendTotal = 0;
			SetEvent(p->m_evSendAck);					// ���M�J�n��������
//_RPT1(_CRT_WARN, "SendStart %p\n", p);
			break;
			
		case EV_OVL_RECV:						// �񓯊��ǂݎ��̊���
			if (GetOverlappedResult(p->m_hCom, &ovlRecv, &recvSize, TRUE)) {	// �Ȃ���TRUE�łȂ��Ƃ����Ȃ�
				if (0 == recvSize) { // �|�[�g�������ꍇ
					p->m_apierr = 0;
					goto read_abort;
				}
				recvStartTick = GetTickCount();			// ���擾�ł����̂ōēx�V�������Ԃ��Z�b�g (�^�C���A�E�g�`�F�b�N�Ɏg�p)
				
				// ��M�`���ɂ�菈������
				if(NULL == p->m_cRecvCR) {				//// �Œ蒷�ǂݍ���
					// ����Ƀ��[�h
					recvTotal += recvSize;
					if (recvTotal == p->m_nRecvSz) {
						// �f�[�^�ǂݍ��݊���
						ResetEvent(ovlRecv.hEvent);
						RST_RECV_TIMEOUT(p->m_stat);
						RST_WAIT_RECV(p->m_stat);	// �X�e�[�^�X�~�낷
						// �ʒm
						p->sendMsg(WM_KS232C_RECV, (WPARAM)p, 0);
						break;
					}
					// ���̃��[�h���s (���Ŏ�荞�߂Ȃ��ꍇ�B�w��T�C�Y��荞�ނ܂ŌJ��Ԃ�)
					restRecv = p->m_nRecvSz - recvTotal;
					if (!ReadFile(p->m_hCom, p->m_pRecvData+recvTotal, restRecv, NULL, &ovlRecv)) {
						p->m_apierr = GetLastError();	// �����s�̏ꍇ
						if (p->m_apierr != ERROR_IO_PENDING) goto read_abort;
					}

				} else {								//// �I��������`�F�b�N
//_RPT2(_CRT_WARN, "Recving [%c] %p\n", p->m_pRecvData[recvTotal], p);					
					// �I�[���`�F�b�N
					if( p->m_cRecvCR == p->m_pRecvData[recvTotal] ) {
						// �f�[�^�ǂݍ��݊���
						ResetEvent(ovlRecv.hEvent);
						RST_RECV_TIMEOUT(p->m_stat);
						RST_WAIT_RECV(p->m_stat);	// �X�e�[�^�X�~�낷
						// �ʒm
						p->sendMsg(WM_KS232C_RECV, (WPARAM)p, 0);
//_RPT1(_CRT_WARN, "RecvEnd %p   <<<-----------\n", p);
						break;
					}
					// ���̃��[�h���s (���Ŏ�荞�߂Ȃ��ꍇ�B�w��T�C�Y��荞�ނ܂ŌJ��Ԃ�)
					recvTotal += recvSize;		// �ϒ��̏ꍇ�́AreceSize��1�Œ肩��
					if (!ReadFile(p->m_hCom, &p->m_pRecvData[recvTotal], recvSize, NULL, &ovlRecv)) {
						p->m_apierr = GetLastError();	// �����s�̏ꍇ
						if (p->m_apierr != ERROR_IO_PENDING) goto read_abort;
					}
				}

				break;
			} else {									// ���[�h�G���[����
				p->m_apierr = GetLastError();
read_abort :

				CloseHandle(ovlSend.hEvent);
				CloseHandle(ovlRecv.hEvent);
				SET_THREAD_ERR(p->m_stat);
				RST_ACTIVE(p->m_stat);
				p->sendMsg(WM_KS232C_ERR, (WPARAM)p, p->m_apierr);			// �ُ�ؒf�ʒm
				return 0;											// ���C���X���b�h ��M�ُ�ɂ��I��
			}
			
		case EV_OVL_SEND:						// �񓯊��������݂̊���
			if (GetOverlappedResult(p->m_hCom, &ovlSend, &sendSize, TRUE)) {
				// ����ɑ��M
				sendStartTick = GetTickCount();			// ���擾�ł����̂ōēx�V�������Ԃ��Z�b�g (�^�C���A�E�g�`�F�b�N�Ɏg�p)
				sendTotal += sendSize;
				if (sendTotal == p->m_nSendSz) {
					// �f�[�^�������݊���
					ResetEvent(ovlSend.hEvent);
					RST_SEND_TIMEOUT(p->m_stat);
					RST_WAIT_SEND(p->m_stat); // �X�e�[�^�X�~�낷
					// �ʒm
					p->sendMsg(WM_KS232C_SEND, (WPARAM)p, 0);
//_RPT1(_CRT_WARN, "SendEnd %p\n", p);
					break;
				}
				// ���̑��M���s (���ő���Ȃ��ꍇ�B�w��T�C�Y�����܂ŌJ��Ԃ�)
				restSend = p->m_nSendSz	- sendTotal;
				if (!WriteFile(p->m_hCom, p->m_pSendData+sendTotal, restSend, NULL, &ovlSend)) {
					p->m_apierr = GetLastError();	// �����s�̏ꍇ
					if (p->m_apierr != ERROR_IO_PENDING) goto send_abort;
				}
				break;
			} else {  // ���M�G���[����
				p->m_apierr = GetLastError();
send_abort:
				CloseHandle(ovlSend.hEvent);
				CloseHandle(ovlRecv.hEvent);
				SET_THREAD_ERR(p->m_stat);
				RST_ACTIVE(p->m_stat);
				p->sendMsg(WM_KS232C_ERR, (WPARAM)p, p->m_apierr);			// �ُ�ؒf�ʒm
				return 0;											// ���C���X���b�h ���M�ُ�ɂ��I��
			}

		default:	// ���肦�Ȃ�
			continue;
		}
	}
}
