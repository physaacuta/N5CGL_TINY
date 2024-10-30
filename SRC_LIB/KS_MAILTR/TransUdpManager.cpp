#include "StdAfx.h"
#include "TransUdpManager.h"

// メモリーリーク検出 ----->>>
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif
// <<<------------------------

//------------------------------------------
// コンストラクタ
//------------------------------------------
TransUdpManager::TransUdpManager(void) :
ThreadManager("TrUdpMgr")
{
	memset(&m_RecvUdp, 0x00, sizeof(m_RecvUdp));
}

//------------------------------------------
// デストラクタ
//------------------------------------------
TransUdpManager::~TransUdpManager(void)
{
}


//------------------------------------------
// スレッド開始前処理 (スレッド実行直後(シグナルセット前)にコールバック)
//------------------------------------------
int TransUdpManager::ThreadFirst()
{
	//// UDPの受信準備
	OpenUdpPort();

	//// イベント設定
	HANDLE hArray[] = { m_hEventRecv
						};
	SetEventNum(sizeof(hArray)/sizeof(HANDLE), hArray);
	return 0;
}

//------------------------------------------
// スレッド終了前処理 (終了シグナル後にコールバック)
//------------------------------------------
int TransUdpManager::ThreadLast()
{
	CloseUdpPort();
	return 0;
}

//------------------------------------------
// スレッドイベント発生
// int nEventNo イベントNo (0オリジン)
//------------------------------------------
void TransUdpManager::ThreadEvent(int nEventNo)
{
	////// シグナルの条件
	enum {	EV_RECV_UDP = 0,				// メールスロット転送管理からUDP受信
	};

	////// シグナル条件分け
	switch (nEventNo) {

//-----------------------------------------------------------------------------------------------
	case EV_RECV_UDP: 						// メールスロット転送管理からUDP受信

		if(true){

			// イベントをリセット
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
					LOG(em_ERR), "[%s] 無効なパケットを受信 [Pc:%s][Task:%s][len:%d]", my_sThreadName, packet->hostname, packet->mailname, packet->datalen);
					break;
				}

				// 受信情報保持
				memcpy(&m_RecvUdp, packet, sizeof(UdpMail));

				// ここまで来たら、正常なメールスロット転送通知
#ifdef ENABLE_DEBUG_MAILTR
				LOG(em_MSG), "[%s] メールスロット転送要求を受信 [Pc:%s][Task:%s][len:%d]", my_sThreadName, packet->hostname, packet->mailname, packet->datalen);
#endif
				// メールスロット転送要求を受信し、メール送信を実施
				Send_KsMailtr_MailTrans(packet->mailname, packet->hostname, packet->data, packet->datalen);
			}
		}
		return;

	
//-----------------------------------------------------------------------------------------------
	default :
		// ありえない！！
		LOG(em_ERR), "[%s] ThreadEvent EventNo NG=%d", my_sThreadName, nEventNo );
		return;
	}
}


//=====================================
// UDPポート オープン
//=====================================
void TransUdpManager::OpenUdpPort()
{
	WSADATA	wsaData;
	WORD	wVersionRequired = MAKEWORD(2, 2);		// WinSock 2.0 Windows NT
	WSAStartup(wVersionRequired, &wsaData);

	m_udp		 = 0;
	m_hEventRecv = 0;
	
	// 通信用ポートは正常？
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
// UDPポート クローズ
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
// メールスロット転送受信からメール送信
//----------------------------------------------------------
// LPCSTR SendSlotName 送信メールスロット名称
// LPCSTR SendPcName   送信PC名称
// LPCVOID pDate       送信情報
// DWORD  len          送信バイト数
//==========================================================
void TransUdpManager::Send_KsMailtr_MailTrans(LPCSTR SendSlotName, LPCSTR SendPcName, LPCVOID pDate, DWORD len)
{
	// メールスロット転送タスクに通知
	COMMON_QUE que;

	//切り捨てられるのは仕方ない。
	memcpy(&que, pDate, min(sizeof(que), len));
	
	// 送信
	int nRetc = send_mail(SendSlotName, SendPcName, &que);
	if( 0 != nRetc ) {
		syslog(SYSNO_MAIL_SNED_ERR, "[メールスロット転送通知 err_code=%d]", nRetc);
		LOG(em_ERR), "[%s] [メールスロット転送通知 err_code=%d]", my_sThreadName, nRetc);
	}
}
