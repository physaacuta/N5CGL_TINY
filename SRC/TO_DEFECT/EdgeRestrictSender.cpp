#include "StdAfx.h"
#include "EdgeRestrictSender.h"


// メモリーリーク検出 ----->>>
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif
// <<<------------------------


#include "MainInstance.h"

// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 基準

//------------------------------------------
// コンストラクタ
//------------------------------------------
EdgeRestrictSender::EdgeRestrictSender(void) :
ThreadManager("EdgeRestrictSender")
{

}

//------------------------------------------
// デストラクタ
//------------------------------------------
EdgeRestrictSender::~EdgeRestrictSender(void)
{

}

//------------------------------------------
// スレッド開始前処理 (スレッド実行直後(シグナルセット前)にコールバック)
//------------------------------------------
int EdgeRestrictSender::ThreadFirst()
{
	// ポートNoがパラメータから取得するため、コンストラクタでは実施できない

	// 送信用　相手ポートNo
	int nSendPort = mcls_pParam->GetParamCommon().nEdgeRestrictPort;
	_ASSERT( 0 != nSendPort );

	////====================================
	//// UDP設定

	//---------------------------
	// 判定PC 送信ソケット
	WSADATA wsaData;
	WORD wVersionRequired = MAKEWORD(2, 2);
	WSAStartup(wVersionRequired, &wsaData);
	m_SockHtSend = (int)socket(AF_INET, SOCK_DGRAM, 0);
	_ASSERT(INVALID_SOCKET != m_SockHtSend);
	m_AddrHtSend.sin_family			= AF_INET;
	m_AddrHtSend.sin_addr.s_addr	= inet_addr(EPC_UDP_IP_EDGE);
	m_AddrHtSend.sin_port			= htons( nSendPort );

	// イベント設定
	HANDLE hArray[] = { gque_SendUdp.g_evSem
						};
	SetEventNum(sizeof(hArray)/sizeof(HANDLE), hArray);

	return 0;
}
//------------------------------------------
// スレッド終了前処理 (スレッド終了直前にコールバック)
//------------------------------------------
int EdgeRestrictSender::ThreadLast()
{

	//// ソケット解放
	if(INVALID_SOCKET != m_SockHtSend)		closesocket(m_SockHtSend);

	// 解放
	WSACleanup();

	return 0;
}
//------------------------------------------
// スレッドイベント発生
// int nEventNo イベントNo (0オリジン)
//------------------------------------------
void EdgeRestrictSender::ThreadEvent(int nEventNo)
{
	////// シグナルの条件		
	enum {	EV_SEND_HT = 0,							// エッジ検出範囲指定 UDP送信
			EV_END	};

	////// シグナル条件分け
	switch (nEventNo) {
//-----------------------------------------------------------------------------------------------
	case EV_SEND_HT:								// エッジ検出範囲指定 UDP送信
		if(true){
			NCL_EDGE_RESTRICT_DATA* pData = gque_SendUdp.GetFromHead();
			int ret = sendto(m_SockHtSend, (const char *)pData, sizeof(NCL_EDGE_RESTRICT_DATA), 0, (SOCKADDR*)&m_AddrHtSend, sizeof(m_AddrHtSend));
#ifdef DEBUG_LOG_EDGE_RESTRICT
			LOG(em_MSG), "[%s] エッジ検出範囲指定 UDP送信 フレームNo:%05d [<%s> L(%.1f～%.1fmm) R(%.1f～%.1fmm)] [<%s> L(%.1f～%.1fmm) R(%.1f～%.1fmm)]", 
				my_sThreadName, pData->frame_num,
				(TRUE == pData->limit[0].use_limit ? "範囲指定":"判定エッジ"), pData->limit[0].left_outer, pData->limit[0].left_inner, pData->limit[0].right_inner, pData->limit[0].right_outer,
				(TRUE == pData->limit[1].use_limit ? "範囲指定":"判定エッジ"), pData->limit[1].left_outer, pData->limit[1].left_inner, pData->limit[1].right_inner, pData->limit[1].right_outer);
#endif
				memcpy(&m_strLastSendData, pData, sizeof(NCL_EDGE_RESTRICT_DATA));

			delete pData;
		}
		break;
	
//-----------------------------------------------------------------------------------------------	
	default :
		// ありえない！！
		break;
	}
}

//*******************************************
// PFCソフトにUDP送信(他クラスから起動)
//*******************************************
//==========================================
// 画像処理エンジンへエッジ位置情報送信
//------------------------------------------
// int		nFrame		: フレームNo
// int*		nUseLimit	: エッジ判定フラグ(0:判定エッジ, 1:エッジ検出範囲指定) [0:表 1:裏]
// float*	dEdge_Left	: 左エッジ位置[0:表 1:裏]
// float*	dEdge_Right	: 右エッジ位置[0:表 1:裏]
//==========================================
void EdgeRestrictSender::SendEdgeRestrict(int nFrame, int* nUseLimit, float* dEdge_Left, float* dEdge_Right)
{
	NCL_EDGE_RESTRICT_DATA*	data = new NCL_EDGE_RESTRICT_DATA;
	memset(data, 0x00, sizeof(NCL_EDGE_RESTRICT_DATA));

	int		nType			= mcls_pParam->GetCommonMast().Edge_Restrict.nType;				// エッジ検出_実施種別
	float	dDetectRange	= mcls_pParam->GetCommonMast().Edge_Restrict.dDetectRange;		// エッジ検出_検出範囲

	data->frame_num = nFrame;

	if(0 != nType) {		// エッジ検出範囲 指定有りの場合
		for(int ii=0; ii<NUM_MEN; ii++) {
				// エッジ検出器の信頼性確認
				float dEdge[2];		// エッジ位置[0:DS側,1:WS側]
				switch (nType) {
				//-------------------------
				case 1:				// 統括エッジ範囲指定
					if (0 == nUseLimit[ii]) {
						data->limit[ii].use_limit = FALSE;										// FALSE:判定エッジ TRUE:エッジ検出範囲指定
						continue;
					} else {
						dEdge[0] = dEdge_Left[ii];
						dEdge[1] = dEdge_Right[ii];
					}
					break;
				//-------------------------
				default:				// その他
					dEdge[0] = dEdge_Left[ii];
					dEdge[1] = dEdge_Right[ii];
					break;
				}

				// データセット
				data->limit[ii].use_limit = TRUE;												// FALSE:判定エッジ TRUE:エッジ検出範囲指定
				data->limit[ii].left_outer  = dEdge[0]  - (float)(dDetectRange * 0.5);	// DS側エッジ範囲(外側)
				data->limit[ii].left_inner  = dEdge[0]  + (float)(dDetectRange * 0.5);	// DS側エッジ範囲(内側)
				data->limit[ii].right_inner = dEdge[1] - (float)(dDetectRange * 0.5);	// WS側エッジ範囲(内側)
				data->limit[ii].right_outer = dEdge[1] + (float)(dDetectRange * 0.5);	// WS側エッジ範囲(外側)
		}
	}
	gque_SendUdp.AddToTail(data);
}

