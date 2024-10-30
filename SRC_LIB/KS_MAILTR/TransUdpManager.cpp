#include "StdAfx.h"
#include "TransUdpManager.h"

// �������[���[�N���o ----->>>
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif
// <<<------------------------

//------------------------------------------
// �R���X�g���N�^
//------------------------------------------
TransUdpManager::TransUdpManager(void) :
ThreadManager("TrUdpMgr")
{
	memset(&m_RecvUdp, 0x00, sizeof(m_RecvUdp));
}

//------------------------------------------
// �f�X�g���N�^
//------------------------------------------
TransUdpManager::~TransUdpManager(void)
{
}


//------------------------------------------
// �X���b�h�J�n�O���� (�X���b�h���s����(�V�O�i���Z�b�g�O)�ɃR�[���o�b�N)
//------------------------------------------
int TransUdpManager::ThreadFirst()
{
	//// UDP�̎�M����
	OpenUdpPort();

	//// �C�x���g�ݒ�
	HANDLE hArray[] = { m_hEventRecv
						};
	SetEventNum(sizeof(hArray)/sizeof(HANDLE), hArray);
	return 0;
}

//------------------------------------------
// �X���b�h�I���O���� (�I���V�O�i����ɃR�[���o�b�N)
//------------------------------------------
int TransUdpManager::ThreadLast()
{
	CloseUdpPort();
	return 0;
}

//------------------------------------------
// �X���b�h�C�x���g����
// int nEventNo �C�x���gNo (0�I���W��)
//------------------------------------------
void TransUdpManager::ThreadEvent(int nEventNo)
{
	////// �V�O�i���̏���
	enum {	EV_RECV_UDP = 0,				// ���[���X���b�g�]���Ǘ�����UDP��M
	};

	////// �V�O�i����������
	switch (nEventNo) {

//-----------------------------------------------------------------------------------------------
	case EV_RECV_UDP: 						// ���[���X���b�g�]���Ǘ�����UDP��M

		if(true){

			// �C�x���g�����Z�b�g
			WSANETWORKEVENTS netevent;
			WSAEnumNetworkEvents( m_udp, m_hEventRecv, &netevent );

			//read all item in que.
			while( 0 != (netevent.lNetworkEvents & FD_READ) )
			{
				char				buf[2048];
				sockaddr_storage	peer;
				int					peerlen = sizeof(peer);
				int					len = recvfrom( m_udp, buf, 2048, 0, (sockaddr*)&peer, &peerlen );
				
				if( 0 >= len ){
					break;
				}

				const UdpMail *packet = (const UdpMail*)buf;
				if(0 == strlen(packet->hostname) || 0 == strlen(packet->mailname) || 0 >= packet->datalen) {
					LOG(em_ERR), "[%s] �����ȃp�P�b�g����M [Pc:%s][Task:%s][len:%d]", my_sThreadName, packet->hostname, packet->mailname, packet->datalen);
					break;
				}

				// ��M���ێ�
				memcpy(&m_RecvUdp, packet, sizeof(UdpMail));

				// �����܂ŗ�����A����ȃ��[���X���b�g�]���ʒm
#ifdef ENABLE_DEBUG_MAILTR
				LOG(em_MSG), "[%s] ���[���X���b�g�]���v������M [Pc:%s][Task:%s][len:%d]", my_sThreadName, packet->hostname, packet->mailname, packet->datalen);
#endif
				// ���[���X���b�g�]���v������M���A���[�����M�����{
				Send_KsMailtr_MailTrans(packet->mailname, packet->hostname, packet->data, packet->datalen);
			}
		}
		return;

	
//-----------------------------------------------------------------------------------------------
	default :
		// ���肦�Ȃ��I�I
		LOG(em_ERR), "[%s] ThreadEvent EventNo NG=%d", my_sThreadName, nEventNo );
		return;
	}
}


//=====================================
// UDP�|�[�g �I�[�v��
//=====================================
void TransUdpManager::OpenUdpPort()
{
	WSADATA	wsaData;
	WORD	wVersionRequired = MAKEWORD(2, 2);		// WinSock 2.0 Windows NT
	WSAStartup(wVersionRequired, &wsaData);

	m_udp		 = 0;
	m_hEventRecv = 0;
	
	// �ʐM�p�|�[�g�͐���H
	_ASSERT(1024 < KS_MAILTR_PORT);

	m_hEventRecv = WSACreateEvent();
	m_udp        = socket( AF_INET, SOCK_DGRAM, IPPROTO_IP );
	if( INVALID_SOCKET == m_udp ){

		LOG(em_ERR), "[%s] Socket Creation Err [Port:%d]", my_sThreadName, KS_MAILTR_PORT);
	}

	BOOL bTrue = 1;
	setsockopt(m_udp, SOL_SOCKET, SO_BROADCAST, (const char *)&bTrue, 4 );
	setsockopt(m_udp, SOL_SOCKET, SO_REUSEADDR, (const char *)&bTrue, 4 );

	sockaddr_in sin = {AF_INET};
	sin.sin_port = htons(KS_MAILTR_PORT);
	if( bind( m_udp, (sockaddr*)&sin, sizeof(sin) ) !=0){
		LOG(em_ERR), "[%s] Socket Error. Bind [Port:%d]", my_sThreadName, KS_MAILTR_PORT);
	}
	WSAEventSelect( m_udp, m_hEventRecv, FD_READ );
}

//=====================================
// UDP�|�[�g �N���[�Y
//=====================================
void TransUdpManager::CloseUdpPort()
{
	if( 0 < m_udp ){
		WSAEventSelect( m_udp, m_hEventRecv, 0 );
		closesocket( m_udp );
		WSACloseEvent(m_hEventRecv);
		m_udp = 0;
	}
}

//==========================================================
// ���[���X���b�g�]����M���烁�[�����M
//----------------------------------------------------------
// LPCSTR SendSlotName ���M���[���X���b�g����
// LPCSTR SendPcName   ���MPC����
// LPCVOID pDate       ���M���
// DWORD  len          ���M�o�C�g��
//==========================================================
void TransUdpManager::Send_KsMailtr_MailTrans(LPCSTR SendSlotName, LPCSTR SendPcName, LPCVOID pDate, DWORD len)
{
	// ���[���X���b�g�]���^�X�N�ɒʒm
	COMMON_QUE que;

	//�؂�̂Ă���͎̂d���Ȃ��B
	memcpy(&que, pDate, min(sizeof(que), len));
	
	// ���M
	int nRetc = send_mail(SendSlotName, SendPcName, &que);
	if( 0 != nRetc ) {
		syslog(SYSNO_MAIL_SNED_ERR, "[���[���X���b�g�]���ʒm err_code=%d]", nRetc);
		LOG(em_ERR), "[%s] [���[���X���b�g�]���ʒm err_code=%d]", my_sThreadName, nRetc);
	}
}
