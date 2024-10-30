#include "StdAfx.h"
#include "PingManager.h"

#include <crtdbg.h>


//------------------------------------------
// �R���X�g���N�^
//------------------------------------------
PingManager::PingManager(void)
{
	WSADATA wsaData;
	WORD wVersionRequired = MAKEWORD(2, 2);
	WSAStartup(wVersionRequired, &wsaData);
}

//------------------------------------------
// �f�X�g���N�^
//------------------------------------------
PingManager::~PingManager(void)
{
	// WinSock�����
    WSACleanup();
}

//------------------------------------------
// �z�X�g�� ���� IP�A�h���X �ɕϊ�
// char const* cHostName �z�X�g��
// CString* cIp IP�A�h���X (10.17.5.207 �̌`��)
// �߂�l : false:�֐��ُ�
//------------------------------------------
bool PingManager::HostToIp(char const* cHostName, char* cIp)
{
	//// �z�X�g�� ���� IP�A�h���X �ɕϊ�
	LPHOSTENT lpHost = gethostbyname(cHostName);
	if (lpHost == NULL) return false;

	//// ������₷���悤�� ������ɕϊ�
	// ��x char [4] �ɕϊ�
	in_addr wk;
	wk.S_un.S_addr  = *((u_long FAR *) (lpHost->h_addr));
	// �����ɕϊ�
	sprintf( cIp, "%d.%d.%d.%d", wk.S_un.S_un_b.s_b1, wk.S_un.S_un_b.s_b2, wk.S_un.S_un_b.s_b3, wk.S_un.S_un_b.s_b4 );
	return true;
}

//------------------------------------------
// �ڍ�Ping ���s
// char const* cIP IP�A�h���X (10.17.5.207 �̌`��) (�z�X�g������������Ȃ��ꍇ�́A���O��HostToIp�����s����IP���擾���邱��)
// int nTimeOut �^�C���A�E�g�҂����� [ms]
// PING_ANSDATA* pAns ���ʏ��
// �߂�l: -1:�֐��ُ� 0:�^�C���A�E�g 1:����
//------------------------------------------
int PingManager::Ping(char const* cIP, int nTimeOut, PING_ANSDATA* pAns)
{
	////// Ping�̒��ӓ_�B
	// Ping�́A�|�[�g������UDP�Ƃ������Ȃ����ȃ\�P�b�g�ƂȂ�B
	// ���ׁ̈A��M�҂������Ă���ꍇ�ɁA�R�}���h�v�����v�g���瑼��Ping�𔭍s�����肷���
	// ���̃A���T�[����M����B(�ǂꂩ�킩��Ȃ��ׁA������M�҂����Ă���S�\�P�b�g�ɓ����Ă�����Ǝv����)
	// �����ŁA��M��A�������v���������肩���Ping���`�F�b�N���āA�֌W�����ɂƂ��炾�ƁA�ēx��M�҂��ɂ���(�҂������̑҂����Ԃ͌��Z������)

	int nRet;
	memset(pAns, 0x00, sizeof(PING_ANSDATA));
	pAns->ans = -1;
	
	// �\�P�b�g����
	SOCKET hSock = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
	if( hSock == SOCKET_ERROR ) return -1;

	// ����ȍ~�́Areturn �֎~


	// ����\�P�b�g�A�h���X�̐ݒ�
	SOCKADDR_IN addr_in;
	addr_in.sin_family		= AF_INET;
	addr_in.sin_port		= 0;
	addr_in.sin_addr.s_addr = inet_addr( cIP );

	// ICMP�G�R�[�v���𑗐M
	nRet = SendEchoRequest(hSock, &addr_in);
	if( -1 == nRet ) goto Ending;


	// �Ⴄ���肩��̎�M�΍�
	int	  nTimer = nTimeOut;
	DWORD nFastTime = GetTickCount();
	while( true ) {
		
		// ����������܂Ńu���b�N
		nRet = WaitForEchoReply(hSock, nTimer);
		if( -1 == nRet ) {
			break;		// �֐��ُ�
		} else if( 0 == nRet )	{
			pAns->ans = 0;		// �^�C���A�E�g
			break;
		}

		// ���������o��
		SOCKADDR_IN 	saSrc;
		ECHOREPLY		echoReply;
		nRet = RecvEchoReply(hSock, &saSrc, &echoReply);
		if( -1 == nRet ) break;

		// ���������s����Ping���ǂ����`�F�b�N
		if(echoReply.ipHdr.iaSrc.S_un.S_addr == addr_in.sin_addr.s_addr) {
			// ���������s����Ping�ɑ΂��鉞���B
			pAns->ans	= 1;
			pAns->time	= GetTickCount() - echoReply.echoRequest.dwTime;
			pAns->ttl	= echoReply.ipHdr.TTL;
			sprintf(pAns->ip, "%d.%d.%d.%d", saSrc.sin_addr.S_un.S_un_b.s_b1, saSrc.sin_addr.S_un.S_un_b.s_b2, saSrc.sin_addr.S_un.S_un_b.s_b3, saSrc.sin_addr.S_un.S_un_b.s_b4);
			break;

		} else {
//			_RPT2(_CRT_WARN, "�ႤICMP�w�b�_ (%d, %d)\n", echoReply.ipHdr.iaDst.S_un.S_addr, addr_in.sin_addr.s_addr);

			// ���ɑ҂������ԕ������Z�B
			nTimer = nTimeOut - (GetTickCount() - nFastTime);
			if(1 >= nTimeOut) {
				pAns->ans = 0;		// �^�C���A�E�g
				break;
			}
		}
	}	

Ending:
	// ��n��
	closesocket(hSock);
	return pAns->ans;
}

//------------------------------------------
// �Ȉ�Ping ���s
// char const* cIP IP�A�h���X (10.17.5.207 �̌`��) (�z�X�g������������Ȃ��ꍇ�́A���O��HostToIp�����s����IP���擾���邱��)
// int nTimeOut �^�C���A�E�g�҂����� [ms]
// �߂�l: false:�ُ� true:�����L��
//------------------------------------------
bool PingManager::Ping(char const* cIP, int nTimeOut)
{
	PING_ANSDATA ans;
	int nRet = Ping( cIP, nTimeOut, &ans);
	if( 1 == nRet ) return true;
	else			return false;
}

//------------------------------------------
// Ping �f�[�^ ���M
// SOCKET s �\�P�b�g
// LPSOCKADDR_IN lpstToAddr ���M����
// �߂�l: -1:�֐��ُ�  1�`:���M�o�C�g��
//------------------------------------------
int PingManager::SendEchoRequest(SOCKET s, LPSOCKADDR_IN lpstToAddr) 
{
	//// �V�[�P���V�����Ȕԍ����K�v�ȈׁAstatic�ɂ��Ă����B�����o�[�ϐ��ł������̂���
	static int nId = 1;
	static int nSeq = 1;
	if( 30000<nId )  nId = 1;
	if( 30000<nSeq ) nSeq = 1;

	// �G�R�[�v���ɏ���ݒ�
	ECHOREQUEST echoReq;
	memset(&echoReq, 0x00, sizeof(ECHOREQUEST));
	
	echoReq.icmpHdr.Type		= ICMP_ECHOREQ;
	echoReq.icmpHdr.Code		= 0;
	echoReq.icmpHdr.Checksum	= 0;
	echoReq.icmpHdr.ID			= nId++;
	echoReq.icmpHdr.Seq			= nSeq++;
	echoReq.dwTime				= GetTickCount();						// ���M���̎��Ԃ��Z�b�g
	for(int ii=0; ii<REQ_DATASIZE; ii++) echoReq.cData[ii] = 0x20 + ii;	// ���M�f�[�^��ݒ�
	echoReq.icmpHdr.Checksum = in_cksum((u_short*)&echoReq, sizeof(ECHOREQUEST));	// �S�f�[�^�Z�b�g��� �`�F�b�N�T�����v�Z

	// �G�R�[�v���𑗐M
	int nRet;						// -1:�G���[  1�`:���M�o�C�g��
	nRet = sendto(	s,							// �\�P�b�g
					(LPSTR)&echoReq,			// �o�b�t�@
					sizeof(ECHOREQUEST),
					0,							// �t���O
					(LPSOCKADDR)lpstToAddr,		// ����
					sizeof(SOCKADDR_IN));		// �A�h���X�̒���

	// �߂�l���`�F�b�N
	if (nRet == SOCKET_ERROR) return -1;
	return nRet;
}

//------------------------------------------
// ����������܂Ńu���b�N
//   �� select()���g�p���āA�f�[�^���ǂݎ��ҋ@�����ǂ����𔻕ʂ���
// SOCKET s �\�P�b�g
// int interval �҂����� [ms]
// �߂�l�F -1:�֐��ُ�  0:�^�C���A�E�g  1�`:����(���ԓ��ɉ�����������)
//------------------------------------------
int PingManager::WaitForEchoReply(SOCKET s, int interval)
{
	int nSockCount = 1;		// �Ď��Ώۃ\�P�b�g��
	
	// �Ď������Z�b�g
	fd_set readfds;
//	readfds.fd_count = nSockCount;
//	readfds.fd_array[0] = s;

	FD_ZERO(&readfds);
	FD_SET(s, &readfds);

	// �^�C���A�E�g����
	struct timeval	timout;
	timout.tv_sec  = interval / 1000;
    timout.tv_usec = (interval % 1000)*1000;

	// ����������܂Ńu���b�N
	return( select(nSockCount, &readfds, NULL, NULL, &timout) );
}

//------------------------------------------
// ��M�f�[�^�̎��o���A���
// SOCKET s �\�P�b�g
// LPSOCKADDR_IN lpsaFrom �擾���
// PECHOREPLY pEchoReply  �擾���
// �߂�l�F -1:�֐��ُ�  0:�ڑ��悪����ɃV���b�g�_�E������   1�`:�g�[�^����M�o�C�g��
//------------------------------------------
int PingManager::RecvEchoReply(SOCKET s, LPSOCKADDR_IN lpsaFrom, PECHOREPLY pEchoReply) 
{
	int nAddrLen = sizeof(SOCKADDR_IN);

	// �G�R�[��������M
	int nRet;						// -1:�G���[   0:�ڑ��悪����ɃV���b�g�_�E������   1�`:�g�[�^����M�o�C�g��
	nRet = recvfrom(s,						// �\�P�b�g
					(LPSTR)pEchoReply,		// �o�b�t�@
					sizeof(ECHOREPLY),		// �o�b�t�@�̃T�C�Y
					0,						// �t���O
					(LPSOCKADDR)lpsaFrom,	// ���M���A�h���X
					&nAddrLen);				// �A�h���X���ւ̃|�C���^

	// �߂�l���`�F�b�N
	if (nRet == SOCKET_ERROR) return -1;
	return nRet;
}

//------------------------------------------
// �`�F�b�N�T�����Z�o
// u_short *addr �`�F�b�N�T���Z�o�|�C���^
// int len ����
// �߂�l : �`�F�b�N�T��
//------------------------------------------
u_short PingManager::in_cksum(u_short *addr, int len)
{
	//// ���[�v�ŃK���K���ƌv�Z����ׁA���W�X�^�ϐ��Ƃ���
	register u_short *w		= addr;
	register int	 nleft	= len;
	register int	 sum	= 0;
	register u_short answer;
	
	//// �`�F�b�N�T���̌v�Z�́A�߂�ǂ������̂� �R�s�y
	//  Our algorithm is simple, using a 32 bit accumulator (sum),
	//  we add sequential 16 bit words to it, and at the end, fold
	//  back all the carry bits from the top 16 bits into the lower
	//  16 bits.
	while( nleft > 1 )  {
		sum += *w++;
		nleft -= 2;
	}

	// mop up an odd byte, if necessary 
	if( nleft == 1 ) {
		u_short	u = 0;

		*(u_char *)(&u) = *(u_char *)w ;
		sum += u;
	}
	
	// add back carry outs from top 16 bits to low 16 bits
	sum  = (sum >> 16) + (sum & 0xffff);	// add hi 16 to low 16
	sum += (sum >> 16);						// add carry
	answer = ~sum;							// truncate to 16 bits
	return answer;
}