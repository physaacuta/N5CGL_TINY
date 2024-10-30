#pragma once
#include <stdio.h>
#include <windows.h>
#include <winsock2.h>

class PingManager
{

//// PING�ɕK�v�ȍ\����
private:
	#pragma pack(1)

//	#define ICMP_ECHOREPLY	0
	static const int ICMP_ECHOREQ = 8;	// Echo�^�C�v (�G�R�[�v������^�C�v)
	static const int REQ_DATASIZE = 32;	// Echo Request Data size


	// IP Header -- RFC 791
	typedef struct tagIPHDR
	{
		u_char			VIHL;			// Version and IHL
		u_char			TOS;			// Type Of Service
		short			TotLen;			// Total Length
		short			ID;				// Identification
		short			FlagOff;		// Flags and Fragment Offset
		u_char			TTL;			// Time To Live
		u_char			Protocol;		// Protocol
		u_short			Checksum;		// Checksum
		struct in_addr	iaSrc;			// Internet Address - Source
		struct in_addr	iaDst;			// Internet Address - Destination
	} IPHDR, *PIPHDR;


	// ICMP Header - RFC 792
	typedef struct tagICMPHDR
	{
		u_char			Type;			// Type
		u_char			Code;			// Code
		u_short			Checksum;		// Checksum
		u_short			ID;				// Identification
		u_short			Seq;			// Sequence
		char			Data;			// Data
	} ICMPHDR, *PICMPHDR;


	// ICMP Echo Request
	typedef struct tagECHOREQUEST
	{
		ICMPHDR			icmpHdr;
		DWORD			dwTime;
		char			cData[REQ_DATASIZE];
	} ECHOREQUEST, *PECHOREQUEST;


	// ICMP Echo Reply
	typedef struct tagECHOREPLY
	{
		IPHDR			ipHdr;
		ECHOREQUEST		echoRequest;
		char			cFiller[256];
	} ECHOREPLY, *PECHOREPLY;

	#pragma pack()


//// PING�ɕK�v�ȍ\����
public:
	struct PING_ANSDATA {
		int				ans;			// ���� (-1:�֐��ُ� 0:�^�C���A�E�g 1:����)
		DWORD			time;			// ��������
		int				ttl;			// TTL
		char			ip[20];			// IP�A�h���X (10.17.5.207\0�`��)
	};

public:
	PingManager(void);
	~PingManager(void);

	// �ÓI���J�֐�
	static bool HostToIp(char const* cHostName, char* cIp);


	// ���J�֐�
	bool	Ping(char const* cIP, int nTimeOut=1000);								// �Ȉ�Ping ���s
	int		Ping(char const* cIP, int nTimeOut, PingManager::PING_ANSDATA* pAns);	// �ڍ�Ping ���s

private:
	//// �����֐�
	int		SendEchoRequest(SOCKET s, LPSOCKADDR_IN lpstToAddr);			// Ping �f�[�^ ���M
	int		WaitForEchoReply(SOCKET s, int interval);						// ����������܂Ńu���b�N
//	DWORD	RecvEchoReply(SOCKET s, LPSOCKADDR_IN lpsaFrom, u_char *pTTL);	// ��M�f�[�^�̎��o���A���
	int		RecvEchoReply(SOCKET s, LPSOCKADDR_IN lpsaFrom, PECHOREPLY pEchoReply);
	u_short in_cksum(u_short *addr, int len);								// �`�F�b�N�T�����Z�o

};
