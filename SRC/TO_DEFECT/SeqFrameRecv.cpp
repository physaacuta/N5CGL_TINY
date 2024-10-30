#include "stdafx.h"
#include "SeqFrameRecv.h"

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
SeqFrameRecv::SeqFrameRecv() :
ThreadManager("SeqFrameR"),
m_nSock(INVALID_SOCKET),
m_bSt(false),
m_bRun(false),
my_bHelcyOk(false),
m_bDebugWpd(false)
{
	// イベント
	my_tmHelcy	= CreateWaitableTimer(NULL, FALSE, NULL);


	// 初期化
	memset(my_RecvBuf, 0x00, sizeof(my_RecvBuf));
	memset(&m_typLastFrame, 0x00, sizeof(m_typLastFrame));



	// UDPソケット生成
	WSADATA	wsadata;
	//WSAStartup(MAKEWORD(2,2), &wsadata);
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
    addr.sin_port = htons(SEQ_UDP_PORT);
	int ret = bind(m_nSock, (LPSOCKADDR)&addr, sizeof(SOCKADDR_IN));
	_ASSERT(0 == ret);

	// オーバーラップ生成
	memset(&m_ovlRecv, 0, sizeof(OVERLAPPED));
	m_ovlRecv.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);		// manual reset
}

//------------------------------------------
// デストラクタ
//------------------------------------------
SeqFrameRecv::~SeqFrameRecv()
{
	// ヘルシー伝文タイムー監視終了
	CancelWaitableTimer(my_tmHelcy);

	//// ソケット解放
	closesocket(m_nSock);

	//// イベント
	CloseHandle(my_tmHelcy);	
	CloseHandle(m_ovlRecv.hEvent);
}


// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// スレッド制御

//------------------------------------------
// スレッド開始前処理 (スレッド実行直後(シグナルセット前)にコールバック)
//------------------------------------------
int SeqFrameRecv::ThreadFirst()
{
	//// イベント設定
	AddEventNum(1, &m_ovlRecv.hEvent);
	AddEventNum(1, &my_tmHelcy);


	// 開始
	SetEvWaitableTimer(my_tmHelcy, PARAM_TIMEOUT_HELCY_RECV, PARAM_TIMEOUT_HELCY_RECV);
	if( ! Recving() ) OnErr(false);
	return 0;
}
//------------------------------------------
// スレッド終了前処理 (終了シグナル後にコールバック)
//------------------------------------------
int SeqFrameRecv::ThreadLast()
{
	// ヘルシー伝文タイムー監視終了
	CancelWaitableTimer(my_tmHelcy);
	return 0;
}

//------------------------------------------
// スレッドイベント発生
// int nEventNo イベントNo (0オリジン)
//------------------------------------------
void SeqFrameRecv::ThreadEvent(int nEventNo)
{
	////// シグナルの条件
	enum {	EV_RECVEND	= 0,		// 受信完了通知
			EV_HELCY				// ヘルシータイマー
	};
	
	////// シグナル条件分け
	switch (nEventNo) {

//-----------------------------------------------------------------------------------------------
	case EV_RECVEND: 									// 受信完了通知
		if( ! RecvCheck() ) {
			OnErr(false);
		}
		break;

//-----------------------------------------------------------------------------------------------
	case EV_HELCY: 										// ヘルシー監視
		if( ! my_bHelcyOk ) {
			OnErr(false);
			if(m_bSt) {			// 正常→異常
				LOG(em_ERR), "[%s] ヘルシー監視タイムアウト", my_sThreadName);
				LOGSEQ(em_ERR), "[%s] ヘルシー監視タイムアウト", my_sThreadName);
				mcls_pStatus->SetStatusQue( "UDP_SEQ_ID", false, true);
			}

			m_bSt = false;	
		} else {
			if( ! m_bSt) {		// 異常→正常
				LOG(em_MSG), "[%s] ヘルシー監視正常復帰", my_sThreadName);
				LOGSEQ(em_MSG), "[%s] ヘルシー監視正常復帰", my_sThreadName);
				mcls_pStatus->SetStatusQue( "UDP_SEQ_ID", true, true);
			}
			m_bSt = true;
		}
		my_bHelcyOk = false;
		break;

//-----------------------------------------------------------------------------------------------
	default :
		// ありえない！！
		LOG(em_ERR), "[%s] ThreadEvent EventNo NG=%d", my_sThreadName, nEventNo );
		LOGSEQ(em_ERR), "[%s] ThreadEvent EventNo NG=%d", my_sThreadName, nEventNo );
		return;
	}
}

//------------------------------------------
// 受信データのチェック
// 戻り値 false:切断  ture:何もしない
//------------------------------------------
bool SeqFrameRecv::RecvCheck()
{
	if( INVALID_SOCKET == m_nSock ) return false;


	//// 読込
	DWORD dwTransBytes; 
	DWORD flags = 0;
	BOOL bRet = WSAGetOverlappedResult(m_nSock, &m_ovlRecv, &dwTransBytes, TRUE, &flags);
	ResetEvent(m_ovlRecv.hEvent);
	if( ! bRet ) {								// 読み込み失敗
		int errcode = WSAGetLastError();
		LOGSEQ(em_ERR), "[%s] WSAGetOverlappedResult 失敗 [%d]", my_sThreadName, errcode);
		//return false;
		goto Ending;
	}

	// 何かしら受信できた
	SEQ_TYPE_HEAD& h = (SEQ_TYPE_HEAD&)my_RecvBuf;


	//======================================================
	// 伝文振り分け
	if( SEQ_TC_HELCY == h.nTC) {			// ヘルシー？
		// 無処理
		my_bHelcyOk = true;		

	} else if( SEQ_TC_FRAME == h.nTC) {			// フレーム情報？
		my_bHelcyOk = true;		
		if( ! AddNewFrame() ) goto Ending;	// データ受信処理

	} else {
		// それ以外ならありえない。
		LOGSEQ(em_ERR), "[%s] TC異常伝文受信 [tc=%d size=%d]", my_sThreadName, h.nTC, h.nSeqLen);
		goto Ending;
	}


	//// 次
Ending:
	if( ! Recving() ) return false;		// 次伝文受信
	return true;
}

//------------------------------------------
// 新規フレーム情報受信
// 戻り値 false:切断  ture:何もしない
//------------------------------------------
bool SeqFrameRecv::AddNewFrame()
{
	// デバック。強制WPD認識
	if( m_bDebugWpd ) {
		m_bDebugWpd = false;
		SEQ_TYPE_FRAME& wk = (SEQ_TYPE_FRAME&)my_RecvBuf;
		wk.data.nWpdFramePos = CTime::GetCurrentTime().GetSecond();
		LOGSEQ(em_WAR), "[%s] 強制WPD認識 F=%d", my_sThreadName, wk.data.nFrameNo);
	}


	// データセット
	SEQ_TYPE_FRAME& h = (SEQ_TYPE_FRAME&)my_RecvBuf;
//▲ h.data.nFrameNo -= 2;

	int nQue = mque_Frame.GetCount();
	memcpy( &m_typLastFrame, &h.data, sizeof(SEQ_TYPE_DATA_FRAME));

	if( -1 != h.data.nWpdFramePos ) {
		//LOG(em_MSG), "[%s] ---- F=%d ----------------------------------", my_sThreadName, h.data.nFrameNo);
		LOGSEQ(em_MSG), "[%s] ---- F=%d ----------------------------------", my_sThreadName, h.data.nFrameNo);
	}
	LOGSEQ(em_MSG), "[%s] F=%lu PLG差=%lu tm差=%lu LVAL積=%lu PLG積=%lu TIME積=%lu WPD=%d QUE=%d", my_sThreadName,
		h.data.nFrameNo, h.data.nPlgDiff, h.data.nTimeDiff, h.data.nExSyncAdd, h.data.nPlgAdd, h.data.nTimeAdd, h.data.nWpdFramePos, nQue);



	if( ! m_bRun) {		// 検査中じゃないので終わり
		//LOGSEQ(em_ERR), "[%s] 開始前にシーケンサUDP受付 Fno:%d", my_sThreadName, h.data.nFrameNo );	
		return true;
	}

	// キューイング
	SEQ_TYPE_DATA_FRAME* pDeli = new SEQ_TYPE_DATA_FRAME;
	memcpy( pDeli, &h.data, sizeof(SEQ_TYPE_DATA_FRAME));
	if( ! mque_Frame.AddToTail(pDeli) ) {
		syslog(SYSNO_QUEFULL_ERR, "[新規フレーム情報登録 F=%d]", h.data.nFrameNo);
		LOG(em_ERR), "[%s] 新規フレーム情報登録キューフル F=%d", my_sThreadName, h.data.nFrameNo);
		LOGSEQ(em_ERR), "[%s] 新規フレーム情報登録キューフル F=%d", my_sThreadName, h.data.nFrameNo);
		delete pDeli;
		return false;
	}
	return true;
}

// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// TPC/IP 通信制御


//------------------------------------------
// 業務伝文の受信を開始
//------------------------------------------
bool SeqFrameRecv::Recving()
{
	if( INVALID_SOCKET == m_nSock ) return false;

	memset(&my_RecvBuf, 0x00, sizeof(my_RecvBuf));

	// 情報生成
	WSABUF wsabuf;								// 受信構造体
	wsabuf.buf = (char*)&my_RecvBuf[0];
	wsabuf.len = PARAM_BUFSIZE;					// 一番大きい伝文でも耐えれるサイズにしておく。
	//struct sockaddr_in fromAddr;				// 受信元
	my_fromLen = sizeof(my_fromAddr);
	DWORD dwTransBytes; 
	DWORD flags = 0;


	// ヘッダー部受信開始
	ResetEvent(m_ovlRecv.hEvent);
	int ret = WSARecvFrom(m_nSock, &wsabuf, 1, &dwTransBytes, &flags, (struct sockaddr *)&my_fromAddr, &my_fromLen, &m_ovlRecv, NULL);
	int errcode = WSAGetLastError();
	if (SOCKET_ERROR == ret && WSA_IO_PENDING != errcode) {		// 異常有り
		LOGSEQ(em_ERR), "[%s]ソケット ヘッダー部受信失敗 err_code=%d", my_sThreadName, errcode);
		syslog(SYSNO_SOCK_RECV_ERR, "[%s,ヘッダー部 err_code=%d]", my_sThreadName, errcode);
		return false;
	}

	//LOGSEQ(em_MSG), "[%s]ソケット ヘッダー部受信", my_sThreadName);

	return true;
}

//------------------------------------------
// 異常通知
// bool bManyu   true:自分で分かって切断
//------------------------------------------
void SeqFrameRecv::OnErr(bool bManyu)
{
	QueAllDelete();						// 未送信バッファを返却

	LOGSEQ(em_ERR), "[%s] 異常通知(%d)", my_sThreadName, bManyu);

	// 外部へ 切断を通知
	if( ! bManyu && m_bSt ) {
		mcls_pStatus->AddQueMyErrWar(true, ON_SEQ_FRAME_SOCK_CLOSE);		// シーケンサとの通信断絶
	}
}

//------------------------------------------
// すべてのキューを返却する
//------------------------------------------
void SeqFrameRecv::QueAllDelete()
{
	SEQ_TYPE_DATA_FRAME* deli;

	// デバック用ログ
	if(0 !=mque_Frame.GetMaxCount() ){		// 1回も動いていない時は出さない
		LOGSEQ(em_MSG), "[%s] 未送信キュー開放! Que=%d, MAX=%d", my_sThreadName, mque_Frame.GetCount(), mque_Frame.GetMaxCount());
	}

	// キューに溜まっているインスタンスを全部開放
	while(true) {
		deli = mque_Frame.GetFromHead(0);
		if (deli == NULL) break;

		// 返却
		delete deli;
		deli = NULL;
	}
	// 最大件数リセット
	mque_Frame.ReSetMaxCount(); 
}
