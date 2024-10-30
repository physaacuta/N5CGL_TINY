#pragma once
#include <stdio.h>
#include <windows.h>
#include <winsock2.h>

class PingManager
{

//// PINGに必要な構造体
private:
	#pragma pack(1)

//	#define ICMP_ECHOREPLY	0
	static const int ICMP_ECHOREQ = 8;	// Echoタイプ (エコー要求するタイプ)
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


//// PINGに必要な構造体
public:
	struct PING_ANSDATA {
		int				ans;			// 結果 (-1:関数異常 0:タイムアウト 1:成功)
		DWORD			time;			// 応答時間
		int				ttl;			// TTL
		char			ip[20];			// IPアドレス (10.17.5.207\0形式)
	};

public:
	PingManager(void);
	~PingManager(void);

	// 静的公開関数
	static bool HostToIp(char const* cHostName, char* cIp);


	// 公開関数
	bool	Ping(char const* cIP, int nTimeOut=1000);								// 簡易Ping 実行
	int		Ping(char const* cIP, int nTimeOut, PingManager::PING_ANSDATA* pAns);	// 詳細Ping 実行

private:
	//// 内部関数
	int		SendEchoRequest(SOCKET s, LPSOCKADDR_IN lpstToAddr);			// Ping データ 送信
	int		WaitForEchoReply(SOCKET s, int interval);						// 応答が来るまでブロック
//	DWORD	RecvEchoReply(SOCKET s, LPSOCKADDR_IN lpsaFrom, u_char *pTTL);	// 受信データの取り出し、解析
	int		RecvEchoReply(SOCKET s, LPSOCKADDR_IN lpsaFrom, PECHOREPLY pEchoReply);
	u_short in_cksum(u_short *addr, int len);								// チェックサムを算出

};
