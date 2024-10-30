#include "StdAfx.h"
#include "SockManager.h"

// メモリーリーク検出 ----->>>
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif
// <<<------------------------

//------------------------------------------
// コンストラクタ
//------------------------------------------
SockManager::SockManager(void) : 
m_bIsConnect(false),
m_bDumpMode(false),
//mcls_pLog(NULL),
ThreadManager("SockMgr")
{
	// 初期化
	for(int ii=0; ii<PARAM_SOCK_MAX; ii++) {
		mcls_pSock[ii] = NULL;
		m_eStateMain[ii] = SOCK_NONE;
		m_eStateNowRecv[ii] = SOCK_NR_RES;
		m_nNowEventNo[ii] = 0;
	}

	//// シグナル生成
	m_evAnserTimeOut = CreateWaitableTimer(NULL, FALSE, NULL);	// 応答伝文タイムアウトシグナル
	m_evHelcyTimeOut = CreateWaitableTimer(NULL, FALSE, NULL);	// ヘルシー伝文受信タイムアウトシグナル
}

//------------------------------------------
// デストラクタ
//------------------------------------------
SockManager::~SockManager(void)
{
	//// メンバー 解放
	//delete mcls_pData;
	// シグナル開放
	CloseHandle(m_evAnserTimeOut);
	CloseHandle(m_evHelcyTimeOut);
}

//------------------------------------------
// スレッド開始前処理 (スレッド実行直後(シグナルセット前)にコールバック)
//------------------------------------------
int SockManager::ThreadFirst()
{
	//// 事前準備
	SockInit();

	//// イベント設定
	// WaitForMultipleObjectsイベント
	HANDLE	hArray[] = {
		m_evAnserTimeOut,
		m_evHelcyTimeOut,
		mcls_pSock[PORT_RECV]->gque_Anser.g_evSem,
		mcls_pSock[PORT_SEND]->gque_Anser.g_evSem,
		mque_pDeli->g_evSem,
	};
	SetEventNum(sizeof(hArray) / sizeof(HANDLE), hArray);
	return 0;
}

//------------------------------------------
// スレッド終了前処理 (終了シグナル後にコールバック)
//------------------------------------------
int SockManager::ThreadLast()
{
	//// 後始末
	SockExit();

	return 0;
}


//------------------------------------------
// スレッドイベント発生
// int nEventNo イベントNo (0オリジン)
//------------------------------------------
void SockManager::ThreadEvent(int nEventNo)
{
	BaseTcpIp::DELIVERY_ANS*	queAns = NULL;			// TCP/IPクラス 操作結果
	bool						bSendStatus = true;		// 送信処理ステータス(True:正常,False:失敗)

														////// シグナルの条件
	enum {
		EV_SEND_ANSER_TIMEOUT = 0,					// 送信後、応答伝文タイムアウト
		EV_RECV_HELCY_TIMEOUT,						// ヘルシー伝文受信タイムアウト
		EV_SOCK_RECV_ANS,							// 受信ポート 操作結果
		EV_SOCK_SEND_ANS,							// 送信ポート 操作結果
		EV_QUE_DELE,								// 送信依頼キューイベント
		EV_END
	};

	////// シグナル条件分け
	switch (nEventNo) {

	//-----------------------------------------------------------------------------------------------
	case EV_SEND_ANSER_TIMEOUT:									// 送信後、応答伝文タイムアウト
		LOG(em_ERR), "[%s] 応答伝文待ちタイムアウト発生 イベントNo %d", my_sThreadName, m_nNowEventNo[ID_SEND] );
		syslog(322, "[応答伝文タイムアウト:イベントNo %d]", m_nNowEventNo[ID_SEND]);
		UserClose();
		break;

	//-----------------------------------------------------------------------------------------------
	case EV_RECV_HELCY_TIMEOUT:									// ヘルシー伝文受信タイムアウト
		LOG(em_ERR), "[%s] ヘルシー伝文受信タイムアウト", my_sThreadName);
		syslog(322, "[ヘルシー伝文:受信タイムアウト]");
		UserClose();
		break;

		//-----------------------------------------------------------------------------------------------
	case EV_SOCK_RECV_ANS:										// 受信ポート 操作結果
		queAns = mcls_pSock[PORT_RECV]->gque_Anser.GetFromHead();
		// 操作結果の処理
		ExecAnsQue(queAns->nMyID, queAns);
		// 取り出した操作結果を開放
		SAFE_DELETE(queAns->Data);
		SAFE_DELETE(queAns);
		break;

		//-----------------------------------------------------------------------------------------------
	case EV_SOCK_SEND_ANS:										// 送信ポート 操作結果
		queAns = mcls_pSock[PORT_SEND]->gque_Anser.GetFromHead();
		// 操作結果の処理
		ExecAnsQue(queAns->nMyID, queAns);
		// 取り出した操作結果を開放
		SAFE_DELETE(queAns->Data);
		SAFE_DELETE(queAns);
		break;

		//-----------------------------------------------------------------------------------------------
	case EV_QUE_DELE:											// 送信依頼キューイベント

		if (!SendLockStart()) {
			// 切断処理実行
			UserClose();
		}

		//// 送信処理中の失敗の場合は、機器状態を変更
		if (SOCK_EXEC == m_eStateMain[ID_SEND]) {
			// 前回が送信失敗の場合は、機器状態変更
			if (!bSendStatus) {
				//KizuFunction::SetStatus("ZI_SEND_ERR_ID", true);		// 送信失敗時の機器状態は無し。故障ログのみ
				bSendStatus = true;										// 送信結果を保存
			}
		}
		else {
			// 前回が送信成功の場合は、機器状態変更
			if (bSendStatus) {
				//KizuFunction::SetStatus("ZI_SEND_ERR_ID", false);		// 送信失敗時の機器状態は無し。故障ログのみ
				bSendStatus = false;									// 送信結果を保存
			}
		}
		break;

		//-----------------------------------------------------------------------------------------------
	default:
		// ありえない！！
		LOG(em_ERR), "[%s] ThreadEvent EventNo NG=%d", my_sThreadName, nEventNo );
		return;
	}
}

//------------------------------------------
// スレッドのWaitFor待ちの直前処理  
// int nDefWaitNum 元の数
// int* nNowWaitNum 変更後の数
//------------------------------------------
void SockManager::ThreadWaitFor(int nDefWaitNum, int* nNowWaitNum)
{
	if (SOCK_EXEC == m_eStateMain[ID_RECV] || SOCK_EXEC == m_eStateMain[ID_SEND]) *nNowWaitNum = nDefWaitNum - 1;
	else                                                                          *nNowWaitNum = nDefWaitNum;
}

//------------------------------------------
// 現在のステータス名称
// SOCK_STATE st ステータス
//------------------------------------------
CString SockManager::GetSockStatName(SOCK_STAT st) const
{
	CString wk;
	if (st == SOCK_NONE) { wk = "切断中"; }
	else if (st == SOCK_IDLE) { wk = "待ち"; }
	else if (st == SOCK_EXEC) { wk = "実行中"; }
	else { wk = ""; }
	return wk;
}

// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// TCP/IP からの イベント処理

//------------------------------------------
// TCP/IPクラス 操作結果の処理
// int id ポートID (よく使うから引数かしておく)
// BaseTcpIp::DELIVERY_ANS* que 操作結果
//------------------------------------------
void SockManager::ExecAnsQue(int id, BaseTcpIp::DELIVERY_ANS* que)
{
//	LOG(em_MSG), "[%s] ----------- 操作結果通知 <%s> [%s] Retc=%d-----------", 
//		my_sThreadName, mcls_pSock[id]->GetMyName(), mcls_pSock[id]->GetAnsStateName(que->state), (que->Data==NULL ? 0 : que->Data->nAns) );

	switch(que->state) {

//-----------------------------------------------------------------------------------------------
	case BaseTcpIp::ANS_CLOSE :										// クローズ通知
		if (KIZUSOCK_CLOSEKIND_SOCK == mcls_pSock[id]->GetCloseKind() ||
			KIZUSOCK_CLOSEKIND_PROG == mcls_pSock[id]->GetCloseKind()) {
			// 相手から切断
			LOG(em_ERR), "[%s] %s 相手から切断要求！！", my_sThreadName, mcls_pSock[id]->GetMyName());
		}
		LOG(em_MSG), "[%s] %s 切断区分====[%d]", my_sThreadName, mcls_pSock[id]->GetMyName(), mcls_pSock[id]->GetCloseKind());
		// 切断処理実行
		UserClose();
		break;

//-----------------------------------------------------------------------------------------------
	case BaseTcpIp::ANS_CONNECT :									// コネクト完了通知
		// ステータス関連初期化
		for(int ii=0; ii<PARAM_SOCK_MAX; ii++) {
			if (BaseTcpIp::STATE_NONE != mcls_pSock[ii]->GetSockState()) {
				m_eStateMain[ii] = SOCK_IDLE;
				m_eStateNowRecv[ii] = SOCK_NR_RES;
				m_nNowEventNo[ii] = 0;
			}
		}

		// 両方繋がったら、回線正常認識とする
		if( BaseTcpIp::STATE_NONE != mcls_pSock[PORT_RECV]->GetSockState() &&
			BaseTcpIp::STATE_NONE != mcls_pSock[PORT_SEND]->GetSockState() ) {
		
			// 整合性チェック (コネクト通知が来る前に、上記IF文で、両ポート正常になる可能性がある対策)
			if(m_bIsConnect) break;

			// フラグ変更
			m_bIsConnect = true;
			//for(int ii=0; ii<PARAM_SOCK_MAX; ii++) {
			//	m_eStateMain[ii] = SOCK_IDLE;
			//	m_eStateNowRecv[ii] = SOCK_NR_RES;
			//	m_nNowEventNo[ii] = 0;
			//}
			KizuFunction::SetStatus(MYID_KIKI, true, false);
			LOG(em_MSG), "[%s] 接続完了 ===================================>>>>", my_sThreadName);
			syslog(320, "[接続完了]" );

			// ヘルシー伝文監視 開始
//▲		
			SetHelcy();
	
			// 受信ポートを 受信待ちにしておく。
			if( ! RecvStart( PORT_RECV, sizeof(PRO_TYPE_RESP), m_cSockBuf[PORT_RECV][BUF_RECV] )) {
				mcls_pSock[PORT_RECV]->UserClose();
				break;
			}
			if( ! RecvStart( PORT_SEND, sizeof(PRO_TYPE_RESP), m_cSockBuf[PORT_SEND][BUF_RECV] )) {
				mcls_pSock[PORT_SEND]->UserClose();
				break;
			}
		}

		break;

//-----------------------------------------------------------------------------------------------
	case BaseTcpIp::ANS_RECVEND :									// 非同期 受信完了通知
		if( 0 != que->Data->nAns) {
			// 正常受信完了じゃないので切断しよう
			UserClose();
		} else {
			// 受信完了時の処理を行う
			if( ! RecvEnd(id, que)) {
				mcls_pSock[PORT_RECV]->UserClose();
				break;
			}
		}
		break;

//-----------------------------------------------------------------------------------------------
	case BaseTcpIp::ANS_SENDEND :									// 送信完了通知
		// 非同期送信は 今回は、行わない (処理簡略化の為)
		break;
	
	default:			// ありえない
		LOG(em_ERR), "[%s] モード異常 [%d]", my_sThreadName, que->state);
		break;
	}
}

// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// TCP/IP 通信制御
//------------------------------------------
// 初期化処理 (一度のみ起動)
//------------------------------------------
void SockManager::SockInit()
{
	BaseTcpIp::DELIVERY_COMMON	deli;

	//// 受信用スレッド 生成
	// 定義
	deli.nMyID = PORT_RECV;
	deli.bSorC = true;
	deli.bRorS = true;
	strcpy_s (deli.cMyName,  "[受信ポート]");
	sprintf(deli.cSession, "%s_RECV", TO_PROCON);
	deli.nBufferSize = PARAM_SOCK_BUFSIZE;
	deli.nRetryTimer = PARAM_SOCK_RETRY_TIMER;
	// 生成
	mcls_pSock[PORT_RECV] = new BaseTcpIp(&deli);
	mcls_pSock[PORT_RECV]->SetLogMgr(mcls_pLog);
	mcls_pSock[PORT_RECV]->SetMainPostTid(GetTid());

	//// 送信用スレッド 生成
	// 定義
	deli.nMyID = PORT_SEND;
	deli.bSorC = true;//true;
	deli.bRorS = false;
	strcpy_s (deli.cMyName,  "[送信ポート]");
	sprintf(deli.cSession, "%s_SEND", TO_PROCON);
	deli.nBufferSize = PARAM_SOCK_BUFSIZE;
	deli.nRetryTimer = PARAM_SOCK_RETRY_TIMER;
	// 生成
	mcls_pSock[PORT_SEND] = new BaseTcpIp(&deli);
	mcls_pSock[PORT_SEND]->SetLogMgr(mcls_pLog);
	mcls_pSock[PORT_SEND]->SetMainPostTid(GetTid());

	//// ワーカースレッド開始
	mcls_pSock[PORT_RECV]->Start();
	mcls_pSock[PORT_SEND]->Start();
}

//------------------------------------------
// 終了処理 (一度のみ起動)
//------------------------------------------
void SockManager::SockExit()
{
	// ワーカースレッド停止
	mcls_pSock[PORT_RECV]->Stop();
	mcls_pSock[PORT_SEND]->Stop();

	// ワーカースレッド開放
	delete mcls_pSock[PORT_RECV];
	delete mcls_pSock[PORT_SEND];
}

//------------------------------------------
// 強制切断
// bMode true:正常切断要求  false:強制切断   
//------------------------------------------
void SockManager::UserClose(bool bMode)
{
	if( m_bIsConnect ) {		// 今接続中？
		m_bIsConnect = false;		// ステータス変更前にクローズ処理が動作してしまうため

		// どのポートからでも クローズ通知が来たら、全ポート強制終了させる
		mcls_pSock[PORT_RECV]->UserClose();
		mcls_pSock[PORT_SEND]->UserClose();
		// 機器状態に通知
		KizuFunction::SetStatus(MYID_KIKI, false, false);
		if (!bMode) {
			LOG(em_ERR), "[%s] 強制切断 ===================================>>>>", my_sThreadName);
		}
		else {
			LOG(em_MSG), "[%s] 切断     ===================================>>>>", my_sThreadName);
		}
		syslog(321, "[%s]", (!bMode?"強制切断":"切断"));
	}
	// ステータス関連初期化
	m_bIsConnect = false;
	for(int ii=0; ii<PARAM_SOCK_MAX; ii++) {
		m_eStateMain[ii] = SOCK_NONE;
		m_eStateNowRecv[ii] = SOCK_NR_RES;
		m_nNowEventNo[ii] = 0;
	}
	CancelWaitableTimer(m_evAnserTimeOut);			// 応答伝文タイムアウトシグナル キャンセル
	CancelWaitableTimer(m_evHelcyTimeOut);			// ヘルシー伝文受信タイムアウトシグナル
}

//------------------------------------------
// 非同期 受信処理開始
// int id ポートID
// int nSize サイズ
// BYTE* pBuf バッファポインタ
// int point 受信データ開始位置
// 戻り値 false:回線切断処理を実施
//------------------------------------------
bool SockManager::RecvStart(int id, int nSize, BYTE* pBuf, int point)
{
	//// 非同期 受信
	// 受信情報セット
	BaseTcpIp::DELIVERY_DATA* pDeleData = new BaseTcpIp::DELIVERY_DATA;
	pDeleData->bAnsQueing = true;	// 固定
	pDeleData->bRorS = true;		// 受信
	pDeleData->nStartPoint = point;
	pDeleData->nSize = nSize;
	pDeleData->Data = pBuf;
	pDeleData->nUserWk[0] = nSize + point;			// 合計受信バイト数としてセットしておく
	
	// 受信要求
	if( mcls_pSock[id]->SetRunningData(pDeleData)) {
		return true;	
	} else {
		LOG(em_ERR), "[%s] 非同期 受信要求失敗", my_sThreadName);
		SAFE_DELETE(pDeleData);
		return false;
	}
}

//------------------------------------------
// レスポンス伝文の送信 (同期)
// int id ポート
// BYTE bLowByte 正常異常区分 (サブヘッダーの下位)
//------------------------------------------
bool SockManager::SendLock_Respons(int id, BYTE bLowByte)
{
	PRO_TYPE_RESP		bResp;				// レスポンス伝文

	// 伝文生成
	bResp.bHead[0] = BaseTcpIp::gcTCPSEMI_HEAD_H_RESPONS;		// 種別:レスポンス
	bResp.bHead[1] = bLowByte;

	// 同期送信
	int iRetc = mcls_pSock[id]->LockSend( (BYTE*)&bResp, sizeof(bResp), PARAM_SOCK_TIMEOUT );
	LOG(em_MSG), "[%s] %s レス伝文 送信 retc=%d", my_sThreadName, mcls_pSock[id]->GetMyName(), iRetc );
	return ( 0 == iRetc ? true : false);
}
//------------------------------------------
// ヘルシー伝文 受信タイムアウト設定
//------------------------------------------
void SockManager::SetHelcy()
{
	LARGE_INTEGER ts;
	ts.QuadPart = (__int64)PARAM_HELCY_TIMEOUT * -10000;	// 1[ms]→100[ns] (マイナス指定で、指定時間経過でシグナル) (第3引数は2回目以降の起動間隔)
	SetWaitableTimer(m_evHelcyTimeOut, &ts, 0, NULL, NULL, FALSE);
}


// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 送受信シーケンス

//------------------------------------------
// 業務伝文 送信 (同期)
// 戻り値 false:切断してね要求
//------------------------------------------
bool SockManager::SendLockStart()
{
	int nTc;					// 送信するトランザクションコード
	int nSize;					// 送信する伝文長

	//// キュー取り出し
	Delivery* deli = mque_pDeli->GetFromHead();
	// 状態確認
	if(! m_bIsConnect) {
		LOG(em_ERR), "[%s] ソケット切断中に送信依頼有り。依頼破棄[%d]", my_sThreadName, deli->eStat);
		SAFE_DELETE(deli->pData);
		SAFE_DELETE(deli);
		return true;
	}

	// //// 送信データ 取得(既にバイト反転済み)
	PRO_TYPE_HEAD*	val = (PRO_TYPE_HEAD*)deli->pData;
	nSize = deli->nSize;
	nTc = deli->nTc;
	memcpy(m_cSockBuf[PORT_SEND][BUF_SEND], deli->pData, nSize);

	//// 同期送信
	LOG(em_MSG), "[%s] 送信シーケンス開始 ==================>>>", my_sThreadName);
	int iRetc = mcls_pSock[PORT_SEND]->LockSend( m_cSockBuf[PORT_SEND][BUF_SEND], nSize, PARAM_SOCK_TIMEOUT );
	if(0 == iRetc) {
		LOG(em_MSG), "[%s] %s 業務伝文 送信 ID=%s TC=%d retc=%d", my_sThreadName, mcls_pSock[PORT_SEND]->GetMyName(), deli->cKey, nTc, iRetc );

		//// 検査実績伝文のみ
		if(PRO_TC_ZISEKI == nTc) {
			syslog(330, "[コイルNo=%.12s, ID=%s]", ((PRO_TYPE_ZISEKI*)m_cSockBuf[PORT_SEND][BUF_SEND])->data.KeyInf.cCoilNo, deli->cKey);
			// 送信結果反映
			SetSendDataAnser(deli, true );
		}

	} else {
		LOG(em_ERR), "[%s] %s 業務伝文 送信失敗 ID=%s TC=%d retc=%d", my_sThreadName, mcls_pSock[PORT_SEND]->GetMyName(), deli->cKey, nTc, iRetc );

		//// 検査実績伝文のみ
		if(PRO_TC_ZISEKI == nTc) {
			syslog(323, "[コイルNo=%.12s, ID=%s]", ((PRO_TYPE_ZISEKI*)m_cSockBuf[PORT_SEND][BUF_SEND])->data.KeyInf.cCoilNo, deli->cKey);
			// 送信結果反映
			SetSendDataAnser(deli, false );
		}
		SAFE_DELETE(deli->pData);
		SAFE_DELETE(deli);
		return false;
	}

	//// 送信完了後の設定
	m_eStateMain[ID_SEND] = SOCK_EXEC; 	
	m_nNowEventNo[ID_SEND] = nTc;
	// 応答伝文タイムアウト設定
	LARGE_INTEGER ts;
	ts.QuadPart = (__int64)PARAM_SOCKANSER_TIMEOUT * -10000;	// 1[ms]→100[ns] (マイナス指定で、指定時間経過でシグナル) (第3引数は2回目以降の起動間隔)
	SetWaitableTimer(m_evAnserTimeOut, &ts, 0, NULL, NULL, FALSE);

	//// 受け渡し領域開放
	SAFE_DELETE(deli->pData);
	SAFE_DELETE(deli);
	return true;
}

//------------------------------------------
// 受信 完了処理
// int id ポートID
// BaseTcpIp::DELIVERY_ANS* que イベント本体
// 戻り値 false:回線切断処理を実施
//------------------------------------------
bool SockManager::RecvEnd(int id, BaseTcpIp::DELIVERY_ANS* que) 
{
	const BYTE* data = que->Data->Data;			// データ部分のショートカット


	////////////////////////////////////
	// 1伝文 未取得
	////////////////////////////////////
	//---------------------------------------------------------
	if( SOCK_NR_RES == m_eStateNowRecv[id] ) {							// レス部
		if( BaseTcpIp::gcTCPSEMI_HEAD_H_RESPONS == data[0] ) {					// レスポンス伝文
			//// レスポンス伝文なら以降処理無しとする
			LOG(em_MSG), "[%s] %s レス伝文 受信", my_sThreadName, mcls_pSock[id]->GetMyName() );

//--->>

			if( id == ID_SEND ) {					// 送信ポート
				if (SOCK_EXEC == m_eStateMain[ID_SEND]) {
					// 応答伝文受信完了 (送信シーケンス完了)
					m_eStateMain[ID_SEND] = SOCK_IDLE;
					m_nNowEventNo[ID_SEND] = 0;
					CancelWaitableTimer(m_evAnserTimeOut);			// 応答伝文タイムアウトシグナル キャンセル
					LOG(em_MSG), "[%s] 送信シーケンス完了 <<<==================", my_sThreadName);
				}
			}
//<<<----

			// また最初から受信開始
			m_eStateNowRecv[id] = SOCK_NR_RES;
			if( ! RecvStart( id, sizeof(PRO_TYPE_RESP), m_cSockBuf[id][BUF_RECV] )) {
				return false;
			}
			return true;

		} else if ( BaseTcpIp::gcTCPSEMI_HEAD_H_GYOUMU == data[0] ) {			// 業務伝文 又は 応答伝文

			//// ヘッダー部を受信する
			m_eStateNowRecv[id] = SOCK_NR_HEAD;
			if( ! RecvStart( id, sizeof(PRO_TYPE_HEAD)-sizeof(PRO_TYPE_RESP), m_cSockBuf[id][BUF_RECV], que->Data->nUserWk[0] )) {
				return false;
			}
			return true;
	
		} else {
			LOG(em_ERR), "[%s] %s サブヘッダ異常 [%02x, %02x]", my_sThreadName, mcls_pSock[id]->GetMyName(), data[0], data[1]);
			syslog(324, "[受信伝文:サブヘッダ異常] %s [%02x, %02x]]", mcls_pSock[id]->GetMyName(), data[0], data[1]);
			return false;
		}

	//---------------------------------------------------------
	} else if( SOCK_NR_HEAD == m_eStateNowRecv[id] ) {							// ヘッダー部
		short nSize = ((PRO_TYPE_HEAD*)data)->nLen;
		// 伝文長取得 (バイト判定しないとダメだから、とりあえずその部分だけバイト反転)
#ifdef BYTE_HANTEN
		// バイト反転有り（1、5CGL、1CAPL）
		nSize = KizuFunction::ByteHanten2( (BYTE*) &nSize);
#endif
		nSize = PRO_TYPE_LENGTH_ORIG(nSize);									// 本来の全伝文長を取得
		//// データ部だけを受信 (伝文長は、ヘッダー部を含めているので、それも考慮)
		nSize -= sizeof(PRO_TYPE_HEAD);
		if(0 == nSize) goto EndFunc;		// データ部が無い = 応答伝文 = 受信完了したとなる
		if(0 > nSize) {						// 伝文長が異常
			LOG(em_ERR), "[%s] %s データ長異常 [%d]", my_sThreadName, mcls_pSock[id]->GetMyName(), nSize);
			syslog(324, "[受信伝文:データ長異常] %s [%d]]", mcls_pSock[id]->GetMyName(), nSize);
			return false;
		}
		if(PORT_SEND == id) {				// 送信ポートはレスポンス伝文以外は受信無し
			LOG(em_ERR), "[%s] %s サブヘッダ異常 [%02x, %02x]", my_sThreadName, mcls_pSock[id]->GetMyName(), data[0], data[1]);
			syslog(324, "[受信伝文:サブヘッダ異常] %s [%02x, %02x]]", mcls_pSock[id]->GetMyName(), data[0], data[1]);
			return false;
		}

		//// データ部を受信する
		// 受信シーケンス管理は不要のためコメント
		//////LOG(em_MSG), "[%s] 受信シーケンス開始 ==================>>> [Size:%04d]", my_sThreadName, nSize);
		//////m_eStateMain[ID_RECV]    = SOCK_EXEC;
		m_eStateNowRecv[ID_RECV] = SOCK_NR_DATA;
		if( ! RecvStart( id, nSize, m_cSockBuf[id][BUF_RECV], que->Data->nUserWk[0] )) {
			return false;
		}
		return true;
	}

	////////////////////////////////////
	// 1伝文 受信完了
	////////////////////////////////////
EndFunc:
	// 必要なデータ取り出し
	PRO_TYPE_HEAD* head = (PRO_TYPE_HEAD*)data;			// ヘッダー部
	short nSize  = head->nLen;
	int   nTc    = head->nTC;

#ifdef BYTE_HANTEN
	// バイト反転有り（1、5CGL、1CAPL）
	nSize  = KizuFunction::ByteHanten2( (BYTE*) &nSize);
	nTc    = KizuFunction::ByteHanten4( (BYTE*) &nTc );
#endif

	nSize = PRO_TYPE_LENGTH_ORIG(nSize);									// 本来の全伝文長を取得

	//// 整合性チェック
	// サブヘッダー
	if (BaseTcpIp::gcTCPSEMI_HEAD_H_GYOUMU != head->bHead[0] ||
		BaseTcpIp::gcTCPSEMI_HEAD_L_OK != head->bHead[1]) {
		LOG(em_ERR), "[%s] ヘッダー部(サブヘッダー異常)=%02X%02X", my_sThreadName, head->bHead[0], head->bHead[1]);
		syslog(324, "[受信伝文:ヘッダー部(サブヘッダー異常)=%02X%02X]", nTc, head->bHead[0], head->bHead[1]);
		return false;
	}


	LOG(em_MSG), "[%s] %s 受信完了 [業務伝文] EvtNo=%d, Len=%dbyte", my_sThreadName, mcls_pSock[id]->GetMyName(), nTc, nSize );

	// とりあえず受信したので、レスポンス伝文を返す。(同期送信) (CGLのみ)
	SendLock_Respons(id, BaseTcpIp::gcTCPSEMI_HEAD_L_OK );
	// 受信シーケンス管理は不要のためコメント
	//////if (id == ID_RECV) {					// 受信ポート
	//////	if (SOCK_EXEC == m_eStateMain[ID_RECV]) {
	//////		// 応答伝文受信完了 (受信シーケンス完了)
	//////		m_eStateMain[ID_RECV] = SOCK_IDLE;
	//////		m_nNowEventNo[ID_RECV] = 0;
	//////		LOG(em_MSG), "[%s] 受信シーケンス完了 <<<==================", my_sThreadName);

	//////	}
	//////}

#ifdef BYTE_HANTEN
	// バイト反転有り時は、反転前のダンプを出力
	// 生データダンプ
	CString cDumpName;
	cDumpName.Format("%s 受信(生)ダンプ %s(%s, EvtNo=%d).log", TO_PROCON,
					mcls_pSock[id]->GetMyName(), "業務伝文", nTc );

	KizuFunction::DumpData(cDumpName, nSize, data, GetDumpMode());
#endif



	//// 整合性チェック 
	// ポート (このチェック以降は、受信ポートだけの話となる)
	if(PORT_SEND == id)  {
		// 送信ポートでレスポンス伝文以外を受信する事は無し
		LOG(em_ERR), "[%s] 送信ポートでレスポンス伝文以外を受信する事はありえない", my_sThreadName);
		syslog(324, "[受信伝文:送信ポートで受信]");
		return false;
	}

	////////////////////////////////////
	// 業務伝文
	////////////////////////////////////

	//// 業務伝文 種類により処理分け
	if (!SetRecvData(nTc, nSize, m_cSockBuf[id][BUF_RECV])) {
		return false;
	}


	//// また最初から受信開始
	m_eStateNowRecv[id] = SOCK_NR_RES;
	if( ! RecvStart( id, sizeof(PRO_TYPE_RESP), m_cSockBuf[id][BUF_RECV] )) {
		return false;
	}
	return true;
}