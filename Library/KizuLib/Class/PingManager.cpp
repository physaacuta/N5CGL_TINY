#include "StdAfx.h"
#include "PingManager.h"

#include <crtdbg.h>


//------------------------------------------
// コンストラクタ
//------------------------------------------
PingManager::PingManager(void)
{
	WSADATA wsaData;
	WORD wVersionRequired = MAKEWORD(2, 2);
	WSAStartup(wVersionRequired, &wsaData);
}

//------------------------------------------
// デストラクタ
//------------------------------------------
PingManager::~PingManager(void)
{
	// WinSockを解放
    WSACleanup();
}

//------------------------------------------
// ホスト名 から IPアドレス に変換
// char const* cHostName ホスト名
// CString* cIp IPアドレス (10.17.5.207 の形式)
// 戻り値 : false:関数異常
//------------------------------------------
bool PingManager::HostToIp(char const* cHostName, char* cIp)
{
	//// ホスト名 から IPアドレス に変換
	LPHOSTENT lpHost = gethostbyname(cHostName);
	if (lpHost == NULL) return false;

	//// 分かりやすいように 文字列に変換
	// 一度 char [4] に変換
	in_addr wk;
	wk.S_un.S_addr  = *((u_long FAR *) (lpHost->h_addr));
	// 文字に変換
	sprintf( cIp, "%d.%d.%d.%d", wk.S_un.S_un_b.s_b1, wk.S_un.S_un_b.s_b2, wk.S_un.S_un_b.s_b3, wk.S_un.S_un_b.s_b4 );
	return true;
}

//------------------------------------------
// 詳細Ping 実行
// char const* cIP IPアドレス (10.17.5.207 の形式) (ホスト名しか分からない場合は、事前にHostToIpを実行してIPを取得すること)
// int nTimeOut タイムアウト待ち時間 [ms]
// PING_ANSDATA* pAns 結果情報
// 戻り値: -1:関数異常 0:タイムアウト 1:成功
//------------------------------------------
int PingManager::Ping(char const* cIP, int nTimeOut, PING_ANSDATA* pAns)
{
	////// Pingの注意点。
	// Pingは、ポート無しのUDPというかなり特殊なソケットとなる。
	// その為、受信待ちをしている場合に、コマンドプロンプトから他のPingを発行したりすると
	// そのアンサーも受信する。(どれかわからない為、多分受信待ちしている全ソケットに入っていくると思われる)
	// そこで、受信後、自分が要求した相手からのPingかチェックして、関係無いにとからだと、再度受信待ちにする(待った分の待ち時間は減算させる)

	int nRet;
	memset(pAns, 0x00, sizeof(PING_ANSDATA));
	pAns->ans = -1;
	
	// ソケット生成
	SOCKET hSock = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
	if( hSock == SOCKET_ERROR ) return -1;

	// これ以降は、return 禁止


	// 宛先ソケットアドレスの設定
	SOCKADDR_IN addr_in;
	addr_in.sin_family		= AF_INET;
	addr_in.sin_port		= 0;
	addr_in.sin_addr.s_addr = inet_addr( cIP );

	// ICMPエコー要求を送信
	nRet = SendEchoRequest(hSock, &addr_in);
	if( -1 == nRet ) goto Ending;


	// 違う相手からの受信対策
	int	  nTimer = nTimeOut;
	DWORD nFastTime = GetTickCount();
	while( true ) {
		
		// 応答が来るまでブロック
		nRet = WaitForEchoReply(hSock, nTimer);
		if( -1 == nRet ) {
			break;		// 関数異常
		} else if( 0 == nRet )	{
			pAns->ans = 0;		// タイムアウト
			break;
		}

		// 応答を取り出し
		SOCKADDR_IN 	saSrc;
		ECHOREPLY		echoReply;
		nRet = RecvEchoReply(hSock, &saSrc, &echoReply);
		if( -1 == nRet ) break;

		// 自分が発行したPingかどうかチェック
		if(echoReply.ipHdr.iaSrc.S_un.S_addr == addr_in.sin_addr.s_addr) {
			// 自分が発行したPingに対する応答。
			pAns->ans	= 1;
			pAns->time	= GetTickCount() - echoReply.echoRequest.dwTime;
			pAns->ttl	= echoReply.ipHdr.TTL;
			sprintf(pAns->ip, "%d.%d.%d.%d", saSrc.sin_addr.S_un.S_un_b.s_b1, saSrc.sin_addr.S_un.S_un_b.s_b2, saSrc.sin_addr.S_un.S_un_b.s_b3, saSrc.sin_addr.S_un.S_un_b.s_b4);
			break;

		} else {
//			_RPT2(_CRT_WARN, "違うICMPヘッダ (%d, %d)\n", echoReply.ipHdr.iaDst.S_un.S_addr, addr_in.sin_addr.s_addr);

			// 既に待った時間分を減算。
			nTimer = nTimeOut - (GetTickCount() - nFastTime);
			if(1 >= nTimeOut) {
				pAns->ans = 0;		// タイムアウト
				break;
			}
		}
	}	

Ending:
	// 後始末
	closesocket(hSock);
	return pAns->ans;
}

//------------------------------------------
// 簡易Ping 実行
// char const* cIP IPアドレス (10.17.5.207 の形式) (ホスト名しか分からない場合は、事前にHostToIpを実行してIPを取得すること)
// int nTimeOut タイムアウト待ち時間 [ms]
// 戻り値: false:異常 true:応答有り
//------------------------------------------
bool PingManager::Ping(char const* cIP, int nTimeOut)
{
	PING_ANSDATA ans;
	int nRet = Ping( cIP, nTimeOut, &ans);
	if( 1 == nRet ) return true;
	else			return false;
}

//------------------------------------------
// Ping データ 送信
// SOCKET s ソケット
// LPSOCKADDR_IN lpstToAddr 送信先情報
// 戻り値: -1:関数異常  1～:送信バイト数
//------------------------------------------
int PingManager::SendEchoRequest(SOCKET s, LPSOCKADDR_IN lpstToAddr) 
{
	//// シーケンシャルな番号が必要な為、staticにしておく。メンバー変数でもいいのだが
	static int nId = 1;
	static int nSeq = 1;
	if( 30000<nId )  nId = 1;
	if( 30000<nSeq ) nSeq = 1;

	// エコー要求に情報を設定
	ECHOREQUEST echoReq;
	memset(&echoReq, 0x00, sizeof(ECHOREQUEST));
	
	echoReq.icmpHdr.Type		= ICMP_ECHOREQ;
	echoReq.icmpHdr.Code		= 0;
	echoReq.icmpHdr.Checksum	= 0;
	echoReq.icmpHdr.ID			= nId++;
	echoReq.icmpHdr.Seq			= nSeq++;
	echoReq.dwTime				= GetTickCount();						// 送信時の時間をセット
	for(int ii=0; ii<REQ_DATASIZE; ii++) echoReq.cData[ii] = 0x20 + ii;	// 送信データを設定
	echoReq.icmpHdr.Checksum = in_cksum((u_short*)&echoReq, sizeof(ECHOREQUEST));	// 全データセット後に チェックサムを計算

	// エコー要求を送信
	int nRet;						// -1:エラー  1～:送信バイト数
	nRet = sendto(	s,							// ソケット
					(LPSTR)&echoReq,			// バッファ
					sizeof(ECHOREQUEST),
					0,							// フラグ
					(LPSOCKADDR)lpstToAddr,		// 宛先
					sizeof(SOCKADDR_IN));		// アドレスの長さ

	// 戻り値をチェック
	if (nRet == SOCKET_ERROR) return -1;
	return nRet;
}

//------------------------------------------
// 応答が来るまでブロック
//   ※ select()を使用して、データが読み取り待機中かどうかを判別する
// SOCKET s ソケット
// int interval 待ち時間 [ms]
// 戻り値： -1:関数異常  0:タイムアウト  1～:成功(時間内に応答があった)
//------------------------------------------
int PingManager::WaitForEchoReply(SOCKET s, int interval)
{
	int nSockCount = 1;		// 監視対象ソケット数
	
	// 監視条件セット
	fd_set readfds;
//	readfds.fd_count = nSockCount;
//	readfds.fd_array[0] = s;

	FD_ZERO(&readfds);
	FD_SET(s, &readfds);

	// タイムアウト時間
	struct timeval	timout;
	timout.tv_sec  = interval / 1000;
    timout.tv_usec = (interval % 1000)*1000;

	// 応答が来るまでブロック
	return( select(nSockCount, &readfds, NULL, NULL, &timout) );
}

//------------------------------------------
// 受信データの取り出し、解析
// SOCKET s ソケット
// LPSOCKADDR_IN lpsaFrom 取得情報
// PECHOREPLY pEchoReply  取得情報
// 戻り値： -1:関数異常  0:接続先が正常にシャットダウンした   1～:トータル受信バイト数
//------------------------------------------
int PingManager::RecvEchoReply(SOCKET s, LPSOCKADDR_IN lpsaFrom, PECHOREPLY pEchoReply) 
{
	int nAddrLen = sizeof(SOCKADDR_IN);

	// エコー応答を受信
	int nRet;						// -1:エラー   0:接続先が正常にシャットダウンした   1～:トータル受信バイト数
	nRet = recvfrom(s,						// ソケット
					(LPSTR)pEchoReply,		// バッファ
					sizeof(ECHOREPLY),		// バッファのサイズ
					0,						// フラグ
					(LPSOCKADDR)lpsaFrom,	// 送信元アドレス
					&nAddrLen);				// アドレス長へのポインタ

	// 戻り値をチェック
	if (nRet == SOCKET_ERROR) return -1;
	return nRet;
}

//------------------------------------------
// チェックサムを算出
// u_short *addr チェックサム算出ポインタ
// int len 長さ
// 戻り値 : チェックサム
//------------------------------------------
u_short PingManager::in_cksum(u_short *addr, int len)
{
	//// ループでガリガリと計算する為、レジスタ変数とする
	register u_short *w		= addr;
	register int	 nleft	= len;
	register int	 sum	= 0;
	register u_short answer;
	
	//// チェックサムの計算は、めんどくさいので コピペ
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