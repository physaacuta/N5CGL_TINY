// PLGReceive.cpp: PLGReceive クラスのインプリメンテーション
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
// 構築/消滅
//////////////////////////////////////////////////////////////////////
//------------------------------------------
// コンストラクタ
//------------------------------------------
PLGReceive::PLGReceive() : 
mcls_pLog(NULL)
{
	int poot;
	int id = GetPrivateProfileInt(INI_COMMON, "PCID", 0, PCINI_NAME);
	poot = EPC_UDP_PORT;


	// UDPソケット生成
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

	// イベント生成
	m_evReset = CreateEvent(NULL, FALSE, FALSE, NULL);
	ASSERT(INVALID_HANDLE_VALUE != m_evReset);

}

//------------------------------------------
// デストラクタ
//------------------------------------------
PLGReceive::~PLGReceive()
{
	// いろんなもの開放
	closesocket(m_nSock);
	CloseHandle(m_evReset);

}



//------------------------------------------
// スレッド
//------------------------------------------
UINT WINAPI PLGReceive::MainThread(LPVOID param)
{
	////// メインインスタンス取り出し
	PLGReceive *p = (PLGReceive *)param;
	p->SetTid();								// 自スレッドIDセット

	DWORD fc = 0;			// フレーム数カウント
	int ret, errcode;

	//// オーバーラップ生成
	OVERLAPPED ovlRecv;
	memset(&ovlRecv, 0, sizeof(OVERLAPPED));
	ovlRecv.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);		// manual reset

	//// UDP受信用
	DEBUG_SPEED_INFO	readInf;					// 受信バッファ
	WSABUF wsabuf;								// 受信構造体
	wsabuf.buf = (char *) &readInf;
	wsabuf.len = sizeof(readInf);
	struct sockaddr_in fromAddr;				// 受信元
	int fromLen = sizeof(fromAddr);
	DWORD dwTransBytes; 
	DWORD flags;

	////// イベント作成
	// 停止要求, 受信待ちキャンセル通知, 受信完了通知
	HANDLE	hArray[3] = {p->my_evStop, p->m_evReset, ovlRecv.hEvent};

	pLOG(em_MSG), "[PLGReceive] スレッド開始 [0x%X]", p->GetTid() );

ReStart:
	// 一回目 読み込み開始
	flags = 0;
	ret = WSARecvFrom(p->m_nSock, &wsabuf, 1, &dwTransBytes, &flags, (struct sockaddr *)&fromAddr, &fromLen, &ovlRecv, NULL);
	errcode = WSAGetLastError();
	if (SOCKET_ERROR == ret && WSA_IO_PENDING != errcode) {		// 異常有り
		pLOG(em_ERR), "[PLGReceive] WSARecvFrom 失敗 [%d]", errcode);
	}

	while (1) {
		// イベントループ
		DWORD ret = WaitForMultipleObjects(3, hArray, FALSE, INFINITE);

		// STOPが来たので終了する
		if (ret == WAIT_OBJECT_0) {					
			break;
		}

		// リセット
		if (ret == WAIT_OBJECT_0+1) {				
			CancelIo((HANDLE)p->m_nSock);			// ソケットキャンセル
			fc = 0;									// カウンターリセット
			goto ReStart;			// 次読み込み開始
		}
		
		////// 読み込み完了
		BOOL bRet = WSAGetOverlappedResult(p->m_nSock, &ovlRecv, &dwTransBytes, TRUE, &flags);
		ResetEvent(ovlRecv.hEvent);
		if (!bRet) {								// 読み込み失敗
			errcode = WSAGetLastError();
			pLOG(em_ERR), "[PLGReceive] WSAGetOverlappedResult 失敗 [%d]", errcode);
			goto ReStart;			// 次読み込み開始
		}
		//ASSERT(dwTransBytes == sizeof(nc_speed_info));		// 受信サイズチェック

		if(dwTransBytes == sizeof(DEBUG_SPEED_INFO)) {		// 本物データ受信時
	
		} else {										// 偽データ受信時

		}

		// 速度PLG受信したので 共有メモリに１フレーム分のデータをセット
		p->mcls_pData->SetFrameData(&readInf);



		// 次の読み込み
		flags = 0;
		ret = WSARecvFrom(p->m_nSock, &wsabuf, 1, &dwTransBytes, &flags, (struct sockaddr *)&fromAddr, &fromLen, &ovlRecv, NULL);
		errcode = WSAGetLastError();
		if (SOCKET_ERROR == ret && WSA_IO_PENDING != errcode) {		// 異常有り
			pLOG(em_ERR), "[PLGReceive] WSARecvFrom 失敗 [%d]", errcode);
		}
	}
	pLOG(em_MSG), "[PLGReceive] スレッド終了 [0x%X]", p->my_Tid);
	return 0;
}
