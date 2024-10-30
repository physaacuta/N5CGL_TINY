// PLGReceive.cpp: PLGReceive �N���X�̃C���v�������e�[�V����
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "PLGReceive.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// �\�z/����
//////////////////////////////////////////////////////////////////////
//------------------------------------------
// �R���X�g���N�^
//------------------------------------------
PLGReceive::PLGReceive() : 
mcls_pLog(NULL)
{
	int poot;
	int id = GetPrivateProfileInt(INI_COMMON, "PCID", 0, PCINI_NAME);
	poot = EPC_UDP_PORT;


	// UDP�\�P�b�g����
	WSADATA	wsadata;
//	WSAStartup(MAKEWORD(2,2), &wsadata);
	WORD wVersionRequired = MAKEWORD(2, 2);
	WSAStartup(wVersionRequired, &wsadata);
	m_nSock = WSASocket(AF_INET, SOCK_DGRAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
	_ASSERT(INVALID_SOCKET != m_nSock);
	int opt = 1;
	setsockopt(m_nSock, SOL_SOCKET, SO_REUSEADDR, (const char *)&opt, sizeof(opt));
	// bind
	struct sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(poot);
	int ret = bind(m_nSock, (LPSOCKADDR)&addr, sizeof(SOCKADDR_IN));
	_ASSERT(0 == ret);

	// �C�x���g����
	m_evReset = CreateEvent(NULL, FALSE, FALSE, NULL);
	ASSERT(INVALID_HANDLE_VALUE != m_evReset);

}

//------------------------------------------
// �f�X�g���N�^
//------------------------------------------
PLGReceive::~PLGReceive()
{
	// �����Ȃ��̊J��
	closesocket(m_nSock);
	CloseHandle(m_evReset);

}



//------------------------------------------
// �X���b�h
//------------------------------------------
UINT WINAPI PLGReceive::MainThread(LPVOID param)
{
	////// ���C���C���X�^���X���o��
	PLGReceive *p = (PLGReceive *)param;
	p->SetTid();								// ���X���b�hID�Z�b�g

	DWORD fc = 0;			// �t���[�����J�E���g
	int ret, errcode;

	//// �I�[�o�[���b�v����
	OVERLAPPED ovlRecv;
	memset(&ovlRecv, 0, sizeof(OVERLAPPED));
	ovlRecv.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);		// manual reset

	//// UDP��M�p
	DEBUG_SPEED_INFO	readInf;					// ��M�o�b�t�@
	WSABUF wsabuf;								// ��M�\����
	wsabuf.buf = (char *) &readInf;
	wsabuf.len = sizeof(readInf);
	struct sockaddr_in fromAddr;				// ��M��
	int fromLen = sizeof(fromAddr);
	DWORD dwTransBytes; 
	DWORD flags;

	////// �C�x���g�쐬
	// ��~�v��, ��M�҂��L�����Z���ʒm, ��M�����ʒm
	HANDLE	hArray[3] = {p->my_evStop, p->m_evReset, ovlRecv.hEvent};

	pLOG(em_MSG), "[PLGReceive] �X���b�h�J�n [0x%X]", p->GetTid() );

ReStart:
	// ���� �ǂݍ��݊J�n
	flags = 0;
	ret = WSARecvFrom(p->m_nSock, &wsabuf, 1, &dwTransBytes, &flags, (struct sockaddr *)&fromAddr, &fromLen, &ovlRecv, NULL);
	errcode = WSAGetLastError();
	if (SOCKET_ERROR == ret && WSA_IO_PENDING != errcode) {		// �ُ�L��
		pLOG(em_ERR), "[PLGReceive] WSARecvFrom ���s [%d]", errcode);
	}

	while (1) {
		// �C�x���g���[�v
		DWORD ret = WaitForMultipleObjects(3, hArray, FALSE, INFINITE);

		// STOP�������̂ŏI������
		if (ret == WAIT_OBJECT_0) {					
			break;
		}

		// ���Z�b�g
		if (ret == WAIT_OBJECT_0+1) {				
			CancelIo((HANDLE)p->m_nSock);			// �\�P�b�g�L�����Z��
			fc = 0;									// �J�E���^�[���Z�b�g
			goto ReStart;			// ���ǂݍ��݊J�n
		}
		
		////// �ǂݍ��݊���
		BOOL bRet = WSAGetOverlappedResult(p->m_nSock, &ovlRecv, &dwTransBytes, TRUE, &flags);
		ResetEvent(ovlRecv.hEvent);
		if (!bRet) {								// �ǂݍ��ݎ��s
			errcode = WSAGetLastError();
			pLOG(em_ERR), "[PLGReceive] WSAGetOverlappedResult ���s [%d]", errcode);
			goto ReStart;			// ���ǂݍ��݊J�n
		}
		//ASSERT(dwTransBytes == sizeof(nc_speed_info));		// ��M�T�C�Y�`�F�b�N

		if(dwTransBytes == sizeof(DEBUG_SPEED_INFO)) {		// �{���f�[�^��M��
	
		} else {										// �U�f�[�^��M��

		}

		// ���xPLG��M�����̂� ���L�������ɂP�t���[�����̃f�[�^���Z�b�g
		p->mcls_pData->SetFrameData(&readInf);



		// ���̓ǂݍ���
		flags = 0;
		ret = WSARecvFrom(p->m_nSock, &wsabuf, 1, &dwTransBytes, &flags, (struct sockaddr *)&fromAddr, &fromLen, &ovlRecv, NULL);
		errcode = WSAGetLastError();
		if (SOCKET_ERROR == ret && WSA_IO_PENDING != errcode) {		// �ُ�L��
			pLOG(em_ERR), "[PLGReceive] WSARecvFrom ���s [%d]", errcode);
		}
	}
	pLOG(em_MSG), "[PLGReceive] �X���b�h�I�� [0x%X]", p->my_Tid);
	return 0;
}
