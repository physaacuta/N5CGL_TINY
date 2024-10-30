#include "StdAfx.h"
#include "CmdSender.h"


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
CmdSender::CmdSender(void) :
ThreadManager("CmdSend")
{

	////====================================
	//// Iniファイルからパラメータ読込み
	CString cWk;

	// 送信用　相手ポートNo
	int nSendPort[NUM_MEN];
	for(int ii=0; ii<NUM_MEN; ii++){
		cWk.Format("HT_SEND_PORT_%d", ii);
		nSendPort[ii] = GetPrivateProfileInt("UDP", cWk, 0, TASKINI_NAME);
		_ASSERT( 0 != nSendPort[ii] );
	}

	// 送信用 IPアドレス
	memset(m_cUniIp, 0x00, sizeof(m_cUniIp));
	for(int ii=0; ii<NUM_SENDER; ii++){
		cWk.Format("HT_SEND_IP_%d", ii+1);
		GetPrivateProfileString("UDP", cWk, "", m_cUniIp[ii], 32, TASKINI_NAME);
		_ASSERT(0 != strlen(m_cUniIp[ii]));
	}


	////====================================
	//// UDP設定

	//---------------------------
	// 画像保存PC 送信ソケット
	WSADATA wsaData;
	WORD wVersionRequired = MAKEWORD(2, 2);
	WSAStartup(wVersionRequired, &wsaData);
	m_SockHtSend = (int)socket(AF_INET, SOCK_DGRAM, 0);
	_ASSERT(INVALID_SOCKET != m_SockHtSend);
	for(int ii=0; ii<NUM_MEN; ii++){
		m_AddrHtSend[ii].sin_family			= AF_INET;
		m_AddrHtSend[ii].sin_addr.s_addr	= INADDR_ANY;
		m_AddrHtSend[ii].sin_port			= htons( nSendPort[ii] );
	}
}

//------------------------------------------
// デストラクタ
//------------------------------------------
CmdSender::~CmdSender(void)
{

	//// ソケット解放
	if(INVALID_SOCKET != m_SockHtSend)		closesocket(m_SockHtSend);

	// 解放
	WSACleanup();
}

//------------------------------------------
// スレッド開始前処理 (スレッド実行直後(シグナルセット前)にコールバック)
//------------------------------------------
int CmdSender::ThreadFirst()
{
	// イベント設定
	HANDLE hArray[] = { gque_SendUdp.g_evSem
						};
	SetEventNum(sizeof(hArray)/sizeof(HANDLE), hArray);

	return 0;
}
//------------------------------------------
// スレッド終了前処理 (スレッド終了直前にコールバック)
//------------------------------------------
int CmdSender::ThreadLast()
{

	return 0;
}
//------------------------------------------
// スレッドイベント発生
// int nEventNo イベントNo (0オリジン)
//------------------------------------------
void CmdSender::ThreadEvent(int nEventNo)
{
	////// シグナルの条件		
	enum {	EV_SEND_HT = 0,							// 調整用レコーダー	UDP送信
			EV_END	};

	////// シグナル条件分け
	switch (nEventNo) {
//-----------------------------------------------------------------------------------------------
	case EV_SEND_HT:								// 調整用レコーダーへUDP送信
		if(true){
			SEND_UDP* pData = gque_SendUdp.GetFromHead();
			HtUdp_Send(pData->nTorB, pData->nPcId, pData->data, pData->nSize, pData->nCommand);
			delete pData;
		}
		break;
	
//-----------------------------------------------------------------------------------------------	
	default :
		// ありえない！！
		break;
	}
}

//=============================================
// UDP送信(ブロードキャスト) 
//---------------------------------------------
// int		TorB		: 0:表、1:裏、-1:両面
// char*	cData		: 送信データ
// int		nSize		: 送信サイズ
// int		nCommand	: 送信コマンド
//=============================================
void CmdSender::HtUdp_Send(int TorB, char* cData, int nSize, int nCommand)
{
	HtUdp_Send(TorB, -1, cData, nSize, nCommand);
}


//==================================================
// UDP送信
//--------------------------------------------------
// int		TorB		: 0:表、1:裏、-1:両面
// int		pcid		: 送信するPC(0オリジン)
//						　-1時、ブロードキャスト送信		
// char*	cData		: 送信データ
// int		nSize		: 送信サイズ
// int		nCommand	: 送信コマンド
//==================================================
void CmdSender::HtUdp_Send(int TorB, int nPcNo, char* cData, int nSize, int nCommand)
{
	_ASSERT(-1 <= nPcNo && NUM_SENDER > nPcNo);

	// 送信先を決定
	int ii_s, ii_e;
	if( -1 == TorB ){	ii_s = 0;	ii_e = 1;	}
	else			{	ii_s = ii_e = TorB;		}


	for(int ii = ii_s; ii<=ii_e; ii++){
		// 実際に送信
		if( -1 == nPcNo )	{ m_AddrHtSend[ii].sin_addr.s_addr = inet_addr(EPC_UDP_IP);	}
		else				{ m_AddrHtSend[ii].sin_addr.s_addr = inet_addr(m_cUniIp[nPcNo]); }
		int ret = sendto(m_SockHtSend, cData, nSize, 0, (SOCKADDR*)&m_AddrHtSend[ii], sizeof(m_AddrHtSend[ii]));

		// 内部ログを出力
		CString	sLogMsg;
		sLogMsg.Format("[%s] コマンド送信", my_sThreadName);
		sLogMsg += (0 == ii?"[表面]":"[裏面]");
		switch(nCommand){
			case NCL_START:		sLogMsg += "[検査開始(NC_START)]";			break;
			case NCL_STOP:		sLogMsg += "[検査停止(NC_STOP)]";			break;
			case NCL_PARAM:		sLogMsg += "[パラメータ変更(NC_PARAM)]";	break;
			case NCL_COILINFO:	sLogMsg += "[上位情報送信(NC_COILINFO)]";	break;
			default:			sLogMsg += "[不明なコマンド]";				break;	
		}

		// パラメータ変更 or 上位情報送信の場合は更に情報を追加!
		if( NCL_PARAM == nCommand ){
			int		scode = ((NCL_CMD_PARAM *)cData)->scode;
			CString	sWk;
			sWk.Format("[scode=%d]", scode);
			sLogMsg += sWk;
		}else if(NCL_COILINFO == nCommand){
			char	cHotCoilNo[PRO_SIZE_COIL_NO + 1];
			memset(cHotCoilNo, 0x00, PRO_SIZE_COIL_NO + 1);
			memcpy(cHotCoilNo, ((TYPE_EPC_COIL *)((NCL_CMD_COILINFO *)cData)->coil_info)->cCoilNo, PRO_SIZE_COIL_NO );
			
			sLogMsg += "[コイルNo=";
			sLogMsg += cHotCoilNo;
			sLogMsg += "]";
		}

		if( SOCKET_ERROR != ret){
			LOG(em_MSG), sLogMsg);
		}else{
			LOG(em_ERR), "%s[err_code=%d]",sLogMsg, GetLastError());
		}
	}

}

//*******************************************
// PFCソフトにコマンド送信(他クラスから起動)
//*******************************************
//==========================================
// 検査開始コマンド通知
//------------------------------------------
// int	TorB	: 0:表、1:裏、-1:両面
//==========================================
void CmdSender::SendCommand_Start(int TorB)
{
	NCL_CMD		typWk;
	typWk.cmd = NCL_START;
	//typWk.dlyExec = 0;

	SEND_UDP*	data = new SEND_UDP;
	memset(data, 0x00, sizeof(SEND_UDP));

	data->nTorB		= TorB;
	data->nPcId		= -1;
	data->nSize		= sizeof(NCL_CMD);
	data->nCommand	= NCL_START;
	memcpy(data->data, &typWk, data->nSize);


	gque_SendUdp.AddToTail(data);
}

//==========================================
// 検査停止コマンド通知
//------------------------------------------
// int	TorB	: 0:表、1:裏、-1:両面
//==========================================
void CmdSender::SendCommand_Stop(int TorB)
{
	NCL_CMD		typWk;
	typWk.cmd = NCL_STOP;
	//typWk.dlyExec = 0;

	SEND_UDP*	data = new SEND_UDP;
	memset(data, 0x00, sizeof(SEND_UDP));

	data->nTorB		= TorB;
	data->nPcId		= -1;
	data->nSize		= sizeof(NCL_CMD);
	data->nCommand	= NCL_STOP;
	memcpy(data->data, &typWk, data->nSize);


	gque_SendUdp.AddToTail(data);
}

//==============================================
// 検査表面状態変更コマンド通知
//----------------------------------------------
// int		TorB	: 0:表、1:裏、-1:両面
// int		scode	: 検査表面状態
//==============================================
void CmdSender::SendCommand_Param(int TorB, int scode)
{
	NCL_CMD_PARAM	typWk;
	typWk.cmd  = NCL_PARAM;
	//typWk.dlyExec = 0;
	typWk.scode = scode;
	typWk.framenum = 0;

	SEND_UDP*	data = new SEND_UDP;
	memset(data, 0x00, sizeof(SEND_UDP));

	data->nTorB		= TorB;
	data->nPcId		= -1;
	data->nSize		= sizeof(NCL_CMD_PARAM);
	data->nCommand	= NCL_PARAM;
	memcpy(data->data, &typWk, data->nSize);


	gque_SendUdp.AddToTail(data);
}

//===================================================
// コイル情報を通知
//---------------------------------------------------
// int				TorB		: 0:表、1:裏、-1:両面
// TYPE_EPC_COIL	typData		: コイル情報
//===================================================
void CmdSender::SendCommand_CoilInf(int TorB, TYPE_EPC_COIL typData)
{
	BYTE			Info[sizeof(NCL_CMD_COILINFO) + sizeof(TYPE_EPC_COIL)];
	NCL_CMD_COILINFO&	typInfo = (NCL_CMD_COILINFO&)Info;
	typInfo.cmd = NCL_COILINFO;
	//typInfo.dlyExec = 0;
	typInfo.info_size = sizeof(TYPE_EPC_COIL);
	memcpy(typInfo.coil_info, &typData, typInfo.info_size);

	SEND_UDP*	data = new SEND_UDP;
	memset(data, 0x00, sizeof(SEND_UDP));
	data->nTorB		= TorB;
	data->nPcId		= -1;
	data->nSize		= sizeof(Info);
	data->nCommand	= NCL_COILINFO;
	memcpy(data->data, &typInfo, sizeof(NCL_CMD_COILINFO) + sizeof(TYPE_EPC_COIL));
	gque_SendUdp.AddToTail(data);
}
//==========================================
// コマンド名称を取得
//------------------------------------------
// int	nCommand	: コマンド
// 戻値	CString		: コマンド名称
//==========================================
CString	CmdSender::GetCmdName(int nCommand)
{
	CString	sWk;

	switch(nCommand){
		case NCL_START:		sWk.Format("検査開始(NC_START)");			break;
		case NCL_STOP:		sWk.Format("検査停止(NC_STOP)");			break;
		case NCL_PARAM:		sWk.Format("パラメータ変更(NC_PARAM)");		break;
		case NCL_COILINFO:	sWk.Format("上位情報設定(NC_COILINFO)");	break;
		default:			sWk.Format("不明なコマンド(%d)", nCommand);	break;
	}

	return sWk;
}