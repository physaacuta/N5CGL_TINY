#include "StdAfx.h"
#include "CamLinkSpeedManager.h"

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
CamLinkSpeedManager::CamLinkSpeedManager(void) :
ThreadManager("LnkSpdMgr")
{
}

//------------------------------------------
// デストラクタ
//------------------------------------------
CamLinkSpeedManager::~CamLinkSpeedManager(void)
{
}


//------------------------------------------
// スレッド開始前処理 (スレッド実行直後(シグナルセット前)にコールバック)
//------------------------------------------
int CamLinkSpeedManager::ThreadFirst()
{
	//// UDPの受信準備
	OpenUdpPort();

	//// Iniファイルから情報を取得
	GetIniSetting();

	//// イベント設定
	HANDLE hArray[] = { m_hEventRecv
						};
	SetEventNum(sizeof(hArray)/sizeof(HANDLE), hArray);
	return 0;
}

//------------------------------------------
// スレッド終了前処理 (終了シグナル後にコールバック)
//------------------------------------------
int CamLinkSpeedManager::ThreadLast()
{
	CloseUdpPort();
	return 0;
}

//------------------------------------------
// スレッドイベント発生
// int nEventNo イベントNo (0オリジン)
//------------------------------------------
void CamLinkSpeedManager::ThreadEvent(int nEventNo)
{
	////// シグナルの条件
	enum {	EV_RECV_UDP = 0,				// 画像処理エンジンからUDP受信
	};

	////// シグナル条件分け
	switch (nEventNo) {

//-----------------------------------------------------------------------------------------------
	case EV_RECV_UDP: 						// 画像処理エンジンからUDP受信

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

				const TYP_SER_CHANGE_REQ *packet = (const TYP_SER_CHANGE_REQ*)buf;
				if( cam_ser_packet_id != packet->PacketId ){
					LOG(em_ERR), "[%s] 無効なパケットを受信. [size:%d][PacketID:%x]", my_sThreadName, len, packet->PacketId );
					break;
				}

				// ここまで来たら、正常な速度変更通知
				LOG(em_MSG), "[%s] シリアル通信速度 変更要求を受信 [CamID:%d][Speed:%dbps]", my_sThreadName, packet->CamId, packet->nSpeed );


				// カメラIDから、フレームグラバーボードIDを特定
				int nBoardID[MAX_PHYSICAL_CAMANGLE];
				int nCnt = SelectBoardID(packet->CamId, nBoardID);

				if( 0 != nBoardID ){

					for(int ii=0; ii<nCnt; ii++){

						LOG(em_MSG), "[%s] UDPパケット判読完了 [CamID:%d]→[BoardID:%d][Speed:%dbps]", my_sThreadName,
																									   packet->CamId,
																									   nBoardID[ii],
																									   packet->nSpeed);

						ToOutMailSender::Send_ToLinksw_ChangeLinkSpeed( nBoardID[ii], packet->nSpeed );
					}

				}else{

					LOG(em_ERR), "[%s] UDPパケットのカメラIDからグラバーボードNoを特定できなかった[CamID:%d]", my_sThreadName,
																											   packet->CamId);
				}
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
void CamLinkSpeedManager::OpenUdpPort()
{
	WSADATA	wsaData;
	WORD	wVersionRequired = MAKEWORD(2, 2);		// WinSock 2.0 Windows NT
	WSAStartup(wVersionRequired, &wsaData);

	m_udp		 = 0;
	m_hEventRecv = 0;
	
	// 画像処理エンジンと通信用ポートは正常？
	_ASSERT(1024 < cam_ser_udp_port);

	m_hEventRecv = WSACreateEvent();
	m_udp        = socket( AF_INET, SOCK_DGRAM, IPPROTO_IP );
	if( INVALID_SOCKET == m_udp ){

		LOG(em_ERR), "[%s] Socket Creation Err [Port:%d]", my_sThreadName, cam_ser_udp_port);
	}

	BOOL bTrue = 1;
	setsockopt(m_udp, SOL_SOCKET, SO_BROADCAST, (const char *)&bTrue, 4 );
	setsockopt(m_udp, SOL_SOCKET, SO_REUSEADDR, (const char *)&bTrue, 4 );

	sockaddr_in sin = {AF_INET};
	sin.sin_port = htons(cam_ser_udp_port);
	if( bind( m_udp, (sockaddr*)&sin, sizeof(sin) ) !=0){
		LOG(em_ERR), "[%s] Socket Error. Bind [Port:%d]", my_sThreadName, cam_ser_udp_port);
	}
	WSAEventSelect( m_udp, m_hEventRecv, FD_READ );
}

//=====================================
// UDPポート クローズ
//=====================================
void CamLinkSpeedManager::CloseUdpPort()
{
	if( 0 < m_udp ){
		WSAEventSelect( m_udp, m_hEventRecv, 0 );
		closesocket( m_udp );
		WSACloseEvent(m_hEventRecv);
		m_udp = 0;
	}
}

//=====================================
// Iniファイルから接続CHを取得して配列にセット
//=====================================
void CamLinkSpeedManager::GetIniSetting()
{
	// 以下の仕様でINIファイルに定義されている接続CH情報を取得
	//-------------------------------------------------------------
	// CAMLINKSW_PCIDx_y : 判定PCと接続されているｶﾒﾗﾘﾝｸｽｲｯﾁ出力 (FPGAボードNo*100 + ch(1～15))
	// 		x:PCID 1～
	// 		y:接続口 1～

	memset(m_nBoardID, 0x00, sizeof(m_nBoardID));

	for(int ii = 1; ii <= NUM_ALL_HANTEI; ii++){

		for(int jj = 1; jj <= NUM_CAMLINK_BOARD; jj++){

			CString	ss;	
			ss.Format("CAMLINKSW_PCID%d_%d", ii,jj);

			int nIniID	= GetPrivateProfileInt(TO_DEFECT, ss, 0, TASKINI_NAME);		// INIファイルから取ってきた生値

			// 定義があったら、出力chを特定
			if( 0 != nIniID ){

				int nFPGA	 = nIniID / 100;		// FPGAボードのID。0：1台目のMaster、1：1台目のSlave、以降連番
				int nBoardID = nIniID % 100;		// 出力chのNo（1～15）

				// 整合性のチェック
				if( 0 > nFPGA ||
					NUM_CAMLINKSW * MAX_CAMLINKSW_FPGA <= nFPGA ||
					0 >= nBoardID ||
					MAX_CAMLINKSW_OUTPUT < nBoardID){

					LOG(em_ERR),"[%s] INI設定異常[%s = %d]→[FPGABoard:%d][ch:%d]", my_sThreadName, ss, nIniID, nFPGA, nBoardID);
					continue;
				}

				// 配列に保持
				m_nBoardID[ii-1][jj-1] = (nFPGA * MAX_CAMLINKSW_OUTPUT) + nBoardID;

#ifdef ENABLE_DEBUG_CAMLINK
				LOG(em_MSG), "[%s] INI設定読込完了[%s = %d]→[FPGABoard:%d][ch:%d]", my_sThreadName, ss, nIniID, nFPGA, nBoardID);
#endif
			}
		}
	}
}

//==========================================================
// カメラIDから、フレームグラバーボードNoを特定
//----------------------------------------------------------
// int		nCamId	: カメラID(1オリジン)
// int*		nBoardID: フレームグラバーボードNo
// 戻値		int		: フレームグラバーボードNoの数(異常時、0)
//==========================================================
int CamLinkSpeedManager::SelectBoardID(const int nCamId, int* nBoardID)
{
	int nCamSetPerPc = mcls_pStatus->GetCamSetPerPc();				// 1PCあたりのカメラセット数

	int	nTorB	= (nCamId - 1) / NUM_CAM_POS;						// 表／裏		0:表 1:裏
	int nPos	= (nCamId - 1) % NUM_CAM_POS;						// 位置			0:DS 1:DQ 2:WQ 3:WS
	int nPcNo	= nPos / nCamSetPerPc;								// 担当するPCno(0オリジン)
	int nCamSet = nPos % nCamSetPerPc;								// 1PC中の、CamSet何個目？(0オリジン)
	int nPcId	= mcls_pStatus->GetHtID(nTorB, nPcNo);				// 担当するPCID(1オリジン)

	int nNum    = 0;	// 対象となるフレームグラバーボードNoの数(MAX_PHYSICAL_CAMANGLEと等しくなるはず)

	// カメラ校正モード中( = iPortViewerからの要求)の場合は、
	// 該当する判定PCの先頭に割り振られたカメラNoを切り替え
	if( EM_DIV_UNTEN::DIV_UNTEN_CAMERA == mcls_pStatus->GetUnten() ){
	
		for(int ii=0; ii<MAX_PHYSICAL_CAMANGLE; ii++){
			nBoardID[ii] = m_nBoardID[nPcId - 1][ii];
			nNum++;
		}

	// それ以外の場合は、該当する判定PCの、該当するカメラのみ切り替え
	}else{
		for(int ii=0; ii<MAX_PHYSICAL_CAMANGLE; ii++){

			if( NUM_CAMLINK_BOARD > nCamSet+ii ){
				nBoardID[ii] = m_nBoardID[nPcId - 1][(nCamSet*MAX_PHYSICAL_CAMANGLE)+ii];
				nNum++;
			}
		}
	}

	return nNum;
}


//==========================================================
// シリアル速度を初期設定に戻す
//----------------------------------------------------------
// int		nBaudrate	: シリアル通信速度[bps]
//==========================================================
void CamLinkSpeedManager::InitCamLinkSpeed(int nBaudrate)
{
	for(int ii=0; ii<NUM_ALL_HANTEI; ii++){
		for(int jj=0; jj<NUM_CAMLINK_BOARD; jj++){

			int nBoardID = m_nBoardID[ii][jj];

			if( 0 != nBoardID ){

				ToOutMailSender::Send_ToLinksw_ChangeLinkSpeed(nBoardID, nBaudrate);
			}
		}
	}
}
