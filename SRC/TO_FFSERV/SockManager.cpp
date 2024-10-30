#include "StdAfx.h"
#include "SockManager.h"

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
SockManager::SockManager(void) :
ThreadManager("SockManager"),
m_bIsConnect(false),
m_bIsFfConnectErr(false),
m_eStateMain(SOCK_NONE)
{
	//// シグナル生成
	m_evAnserTimeOut   = CreateWaitableTimer(NULL, FALSE, NULL);				// 応答伝文待ちタイムアウトシグナル
	m_evHelcySendTimer = CreateWaitableTimer(NULL, FALSE, NULL);				// ヘルシー伝文送信タイマーシグナル
	m_evJukosyoTimer   = CreateWaitableTimer(NULL, FALSE, NULL);				// 重故障監視タイマーシグナル
}

//------------------------------------------
// デストラクタ
//------------------------------------------
SockManager::~SockManager(void)
{
	// シグナル開放
	CloseHandle(m_evAnserTimeOut);			// 応答伝文待ちタイムアウトシグナル
	CloseHandle(m_evHelcySendTimer);		// ヘルシー伝文送信タイマーシグナル
	CloseHandle(m_evJukosyoTimer);			// 重故障監視タイマーシグナル
}

//------------------------------------------
// スレッド開始前処理 (スレッド実行直後(シグナルセット前)にコールバック)
//------------------------------------------
int SockManager::ThreadFirst()
{
	//// TCP/IP 操作クラス初期化
	SockInit(); 

	// WaitForMultipleObjectsイベント
	HANDLE	hArray[] = {m_evAnserTimeOut,								// 応答伝文待ちタイムアウトシグナル
						mcls_pSock[PORT_RECV]->gque_Anser.g_evSem,		// 受信ポート[0] 用操作クラス
						mcls_pSock[PORT_SEND]->gque_Anser.g_evSem,		// 送信ポート[1] 用操作クラス
						m_evHelcySendTimer,								// ヘルシー伝文送信タイマーシグナル
						m_evJukosyoTimer,								// 重故障監視タイマーシグナル
						gque_Deli.g_evSem								// 送信依頼用キュー 
						};

	// イベントセット
	SetEventNum(sizeof(hArray)/sizeof(HANDLE), hArray);

	return 0;
}

//------------------------------------------
// スレッド終了前処理 (終了シグナル後にコールバック)
//------------------------------------------
int SockManager::ThreadLast()
{
	//// TCP/IP 操作クラス開放
	SockExit();

	return 0;
}

//------------------------------------------
// スレッドイベント発生
// int nEventNo イベントNo (0オリジン)
//------------------------------------------
void SockManager::ThreadEvent(int nEventNo)
{
	BaseTcpIp::DELIVERY_ANS*	queAns  = NULL;	// TCP/IPクラス 操作結果


	enum {	EV_SEND_ANSER_TIMEOUT=0,			// 応答伝文待ちタイムアウトシグナル
			EV_SOCK_RECV_ANS,					// 受信ポート 操作結果
			EV_SOCK_SEND_ANS,					// 送信ポート 操作結果
			EV_SEND_HELCY_TIMER,				// ヘルシー伝文送信タイマーシグナル (絶対 送信キューよりも先)
			EV_JUKOSYO_TIMER,					// 重故障監視タイマーシグナル
			EV_QUE_DELI,						// 送信依頼キューイベント
			};

	////// シグナル条件分け
	switch (nEventNo) {

//-----------------------------------------------------------------------------------------------
		case EV_SEND_ANSER_TIMEOUT:									// 送信後、応答伝文タイムアウト
			// 応答伝文待ちタイムアウト
			LOG(em_ERR), "[%s] 応答伝文待ちタイムアウト発生 (%s)", my_sThreadName, GetSendStatName());
			syslog( PARAM_SYSLOG_BASENO + 11, "[応答伝文タイムアウト:%s]", GetSendStatName());
			FFAnserTimeOut();
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
		case EV_SEND_HELCY_TIMER:									// ヘルシー伝文送信タイマーシグナル
			{
				LOG(em_INF), "[%s] ヘルシー送信依頼", my_sThreadName);
				// ヘルシー伝文を最優先で送信する
				Delivery* deli = new Delivery;
				memset(deli, 0x00, sizeof(Delivery));
				deli->eStat = QUE_SEND_HELCY;
				gque_Deli.AddToHead(deli);			// 先頭にキューイングするので注意
			}
			break;

//-----------------------------------------------------------------------------------------------
		case EV_JUKOSYO_TIMER:										// 重故障監視タイマーシグナル
			LOG(em_ERR), "[%s] FFサーバーコネクト失敗 [%d分]経過！！", my_sThreadName, PARAM_JUKOSYO_TIMER);
			KizuFunction::SetStatus("FFSERV_ID", false);
			KizuFunction::SetStatus("FF_CONNECT_ERR_ID", true);

			break;

//-----------------------------------------------------------------------------------------------
		case EV_QUE_DELI:											// 送信依頼キューイベント
			{
				Delivery* deli;
				deli = gque_Deli.GetFromHead();
				if( ! SendLockStart( deli ) ) {
					// 切断処理実行
					UserClose();
				}
				SAFE_DELETE(deli);
			}

			break;

//-----------------------------------------------------------------------------------------------
		default :
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
	if( SOCK_EXEC == m_eStateMain ) *nNowWaitNum = nDefWaitNum-1;
	else							*nNowWaitNum = nDefWaitNum;
}

//------------------------------------------
// キューを削除する
//------------------------------------------
void SockManager::QueDelete()
{
	// キューに溜まっているインスタンスを全部開放
	Delivery* deli = NULL;
	while(true) {
		deli = gque_Deli.GetFromHead(0);
		if(deli == NULL) break;
		delete deli;
		deli = NULL;
	}
}

//------------------------------------------
// 現在のステータス名称
// SOCK_STATE st ステータス
//------------------------------------------
CString SockManager::GetSockStatName(SOCK_STAT st) const
{
	CString wk;
	if		(st == SOCK_NONE)	{ wk = "切断中"; }
	else if(st == SOCK_IDLE)	{ wk = "待ち"; }
	else if(st == SOCK_EXEC)	{ wk = "実行中";}
	else						{ wk = "";}
	return wk;
}

//------------------------------------------
// 現在の送信ステータス名称
// SOCK_STATE st ステータス
//------------------------------------------
CString SockManager::GetSendStatName(QUE_SEND_STAT st) const
{
	CString wk;
	if		(st == QUE_SEND_ZISEKI)	{ wk = "疵実績送信"; }
	else if(st == QUE_SEND_MAE)	{ wk = "前工程実績要求"; }
	else if(st == QUE_SEND_HELCY)	{ wk = "ヘルシー";}
	else							{ wk = "";}
	return wk;
}

// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// TPC/IP からの イベント処理

//------------------------------------------
// TCP/IPクラス 操作結果の処理
// int id ポートID (よく使うから引数かしておく)
// BaseTcpIp::DELIVERY_ANS* que 操作結果
//------------------------------------------
void SockManager::ExecAnsQue(int id, BaseTcpIp::DELIVERY_ANS* que)
{
	switch(que->state) {

//-----------------------------------------------------------------------------------------------
	case BaseTcpIp::ANS_CLOSE :										// クローズ通知
		if( KIZUSOCK_CLOSEKIND_SOCK == mcls_pSock[id]->GetCloseKind() ) {
			// 相手から切断
			LOG(em_ERR), "[%s] %s 相手から切断要求！！", my_sThreadName, mcls_pSock[id]->GetMyName());
		}
		// 切断処理実行
		UserClose();
		break;

//-----------------------------------------------------------------------------------------------
	case BaseTcpIp::ANS_ERR_CONNECT :								// コネクト失敗通知
		if( ! m_bIsFfConnectErr) {
			LOG(em_ERR), "[%s] FFサーバーコネクト失敗！！", my_sThreadName);
			m_bIsFfConnectErr = true;
			KizuFunction::SetStatus("FFSERV_ID", true, false);
			KizuFunction::SetStatus("FF_CONNECT_ERR_ID", false, false);
			// 重故障タイマーセット
			SetJukosyoTimer();
		}
		break;

//-----------------------------------------------------------------------------------------------
	case BaseTcpIp::ANS_CONNECT :									// コネクト完了通知
		// 両方繋がったら、回線正常認識とする
		if( BaseTcpIp::STATE_NONE != mcls_pSock[PORT_RECV]->GetSockState() &&
			BaseTcpIp::STATE_NONE != mcls_pSock[PORT_SEND]->GetSockState() ) {
		
			// 整合性チェック (コネクト通知が来る前に、上記IF文で、両ポート正常になる可能性がある対策)
			if(m_bIsConnect) break;

			// フラグ変更
			m_bIsConnect = true;
			m_bIsFfConnectErr = false;
			m_eStateMain = SOCK_IDLE;
			memset(&m_ExecItem, 0x00, sizeof(m_ExecItem));

			KizuFunction::SetStatus("FFSERV_ID", true);
			KizuFunction::SetStatus("FF_CONNECT_ERR_ID", true);
			LOG(em_MSG), "[%s] 回線正常 ===================================", my_sThreadName);
			syslog( PARAM_SYSLOG_BASENO + 8, "" );
			
			// 重故障タイマーキャンセル
			CancelWaitableTimer(m_evJukosyoTimer);

			// 受信ポートを 受信待ちにしておく。
			if( ! RecvStart( PORT_RECV, sizeof(FF_ANS), m_cSockBuf[PORT_RECV][BUF_RECV] )) {
				UserClose();
				break;
			}

			// 即時ヘルシー伝文を送信依頼
			SetHelcySendTimer(0);

			//// 未送信実績の送信依頼
			AddQue_SendZiseki();
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
				UserClose();
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

//------------------------------------------
// ヘルシー伝文 最優先送信要求
// int nTimer ヘルシー送信待ち時間 [ms]
//------------------------------------------
void SockManager::SetHelcySendTimer(int nTimer)
{
	LARGE_INTEGER ts;
	ts.QuadPart = (__int64)nTimer * -10000;	// 1[ms]→100[ns] (マイナス指定で、指定時間経過でシグナル) (第3引数は2回目以降の起動間隔)
	SetWaitableTimer(m_evHelcySendTimer, &ts, 0, NULL, NULL, FALSE);
}

//------------------------------------------
// 重故障タイマーセット処理
//------------------------------------------
void SockManager::SetJukosyoTimer()
{
	LARGE_INTEGER ts;
	ts.QuadPart = (__int64)PARAM_JUKOSYO_TIMER * (60 * 1000) * -10000;	// 1[分]→100[ns] (マイナス指定で、指定時間経過でシグナル) (第3引数は2回目以降の起動間隔)
	SetWaitableTimer(m_evJukosyoTimer, &ts, 0, NULL, NULL, FALSE);
}

//------------------------------------------
// 応答伝文タイムアウト時の固有処理
//------------------------------------------
void SockManager::FFAnserTimeOut()
{
	if(QUE_SEND_ZISEKI == m_ExecItem.emStat) {
		// 実績応答伝文の送信タイムアウトの処理
		mcls_pData->SetZisekiSendAnser( m_ExecItem.cKeyId, SockDataExec::FF_SENDKIND_ERR );	
	}
}

//------------------------------------------
// 未送信実績の送信依頼
//------------------------------------------
void SockManager::AddQue_SendZiseki()
{
	Delivery* deli = new Delivery;
	memset(deli, 0x00, sizeof(Delivery));
	deli->eStat = QUE_SEND_ZISEKI;
	gque_Deli.AddToTail(deli);
}

// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// TPC/IP 通信制御
//------------------------------------------
// 初期化処理 (一度のみ起動)
//------------------------------------------ 
void SockManager::SockInit()
{
	BaseTcpIp::DELIVERY_COMMON	deli;

	//// 受信用スレッド 生成
	// 定義
	deli.nMyID = PORT_RECV;
	deli.bSorC = false;
	deli.bRorS = true;
	strcpy_s (deli.cMyName,  "[受信ポート]");
	sprintf(deli.cSession, "%s_RECV", TO_FFSERV);
	deli.nBufferSize = PARAM_SOCK_BUFSIZE;
	deli.nRetryTimer = PARAM_SOCK_TIMEOUT;
	// 生成
	mcls_pSock[PORT_RECV] = new BaseTcpIp(&deli);
	mcls_pSock[PORT_RECV]->SetLogMgr(mcls_pLog);
	mcls_pSock[PORT_RECV]->SetMainPostTid(GetTid());
	mcls_pSock[PORT_RECV]->SetErrConnectQueing(true);	// コネクト失敗時にAPに通知有り 

	//// 送信用スレッド 生成
	// 定義
	deli.nMyID = PORT_SEND;
	deli.bSorC = false;
	deli.bRorS = false;
	strcpy_s (deli.cMyName,  "[送信ポート]");
	sprintf(deli.cSession, "%s_SEND", TO_FFSERV);
	deli.nBufferSize = PARAM_SOCK_BUFSIZE;
	deli.nRetryTimer = PARAM_SOCK_TIMEOUT;
	// 生成
	mcls_pSock[PORT_SEND] = new BaseTcpIp(&deli);
	mcls_pSock[PORT_SEND]->SetLogMgr(mcls_pLog);
	mcls_pSock[PORT_SEND]->SetMainPostTid(GetTid());
	mcls_pSock[PORT_SEND]->SetErrConnectQueing(true);	// コネクト失敗時にAPに通知有り

	//// ワーカースレッド開始
	mcls_pSock[PORT_RECV]->Start();
	mcls_pSock[PORT_SEND]->Start();

}

//------------------------------------------
// 終了処理 (一度のみ起動)
//------------------------------------------
void SockManager::SockExit()
{
	//終了時に送信依頼キューが溜まっていたら削除する
	QueDelete();

	// ワーカースレッド停止
	mcls_pSock[PORT_RECV]->Stop();
	mcls_pSock[PORT_SEND]->Stop();

	// ワーカースレッド開放
	delete mcls_pSock[PORT_RECV];
	delete mcls_pSock[PORT_SEND];

}

//------------------------------------------
// 強制切断
//------------------------------------------
void SockManager::UserClose()
{
	CancelWaitableTimer(m_evAnserTimeOut);			// 応答伝文待ちタイムアウトシグナル
	CancelWaitableTimer(m_evHelcySendTimer);		// ヘルシー伝文送信タイマーシグナル
	CancelWaitableTimer(m_evJukosyoTimer);			// 重故障監視タイマーシグナル


	if( m_bIsConnect ) {		// 今接続中？
		// どのポートからでも クローズ通知が来たら、全ポート強制終了させる
		mcls_pSock[PORT_RECV]->UserClose();
		mcls_pSock[PORT_SEND]->UserClose();
		// 機器状態に通知
		KizuFunction::SetStatus("FF_CONNECT_ERR_ID", false, false);
		LOG(em_ERR), "[%s] 回線異常 ===================================", my_sThreadName);
		syslog( PARAM_SYSLOG_BASENO + 9, "" );
	}

	// ステータス関連初期化
	m_bIsConnect = false;
	m_bIsFfConnectErr = false;
	m_eStateMain = SOCK_NONE;
	memset(&m_ExecItem, 0x00, sizeof(m_ExecItem));
}

//------------------------------------------
// 非同期 受信処理開始
// int id ポートID
// int size サイズ
// BYTE* pBuf バッファポインタ
// int point 受信データ開始位置
// 戻り値 false:回線切断処理を実施
//------------------------------------------
bool SockManager::RecvStart(int id, int size, BYTE* pBuf, int point)
{
	//// 非同期 受信
	// 受信情報セット
	BaseTcpIp::DELIVERY_DATA* pDeleData = new BaseTcpIp::DELIVERY_DATA;
	pDeleData->bAnsQueing = true;				// 固定
	pDeleData->bRorS = true;					// 受信
	pDeleData->nStartPoint = point;
	pDeleData->nSize = size;
	pDeleData->Data = pBuf;
	pDeleData->nUserWk[0] = size+point;			// 合計受信バイト数としてセットしておく
	
	// 受信要求
	if( mcls_pSock[id]->SetRunningData(pDeleData)) {
		return true;	
	} else {
		LOG(em_ERR), "[%s] 非同期 受信要求失敗", my_sThreadName);
		SAFE_DELETE(pDeleData);
		return false;
	}
}


// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 送受信シーケンス

//------------------------------------------
// 業務伝文 送信 同期送信
// 戻り値 false:切断してね要求
//------------------------------------------
bool SockManager::SendLockStart(Delivery* deli)
{
	// 状態確認
	if(! m_bIsConnect) {
		LOG(em_ERR), "[%s] ソケット切断中に未処理分の送信依頼有り。依頼破棄[%d]", my_sThreadName, deli->eStat);
		return true;
	}

	//// 送信データ 取得
	if( ! mcls_pData->GetSendData(deli, &m_ExecItem ) ) {
		return true;
	}

	//// 同期送信
	LOG(em_MSG), "[%s] シーケンス開始 (%s)==================>>>", my_sThreadName, GetSendStatName());
	if( ! SendExec() ) {
		return false;
	}

	return true;
}

//------------------------------------------
// 業務伝文送信 共通処理
//------------------------------------------
bool SockManager::SendExec()
{
	//// 同期送信
	int nRec = m_ExecItem.nSendNowNo;
	memcpy(	m_cSockBuf[PORT_SEND][BUF_SEND], m_ExecItem.cSendBuf[nRec], m_ExecItem.nSendSize[nRec]);
	//// 生データダンプ
	CString dumpName;
	dumpName.Format("%s 送信(生)ダンプ (%s, No=%d).log", TO_FFSERV, 
					GetSendStatName(), m_ExecItem.nSendNowNo );
	KizuFunction::DumpData( dumpName, m_ExecItem.nSendSize[nRec], m_cSockBuf[PORT_SEND][BUF_SEND], mcls_pData->GetDumpMode());
	int iRetc = mcls_pSock[PORT_SEND]->LockSend( m_cSockBuf[PORT_SEND][BUF_SEND], m_ExecItem.nSendSize[nRec], PARAM_SOCK_TIMEOUT );

	if(0 != iRetc) {
		LOG(em_ERR), "[%s] %s 送信失敗 [伝文区分=%d 命令No=%s ID=%s]", my_sThreadName, GetSendStatName(m_ExecItem.emStat), m_ExecItem.nSendNowNo, m_ExecItem.cMeiNo, m_ExecItem.cKeyId);
		LOG(em_ERR), "[%s] シーケンス異常完了 <<<==================", my_sThreadName);


		// 個別処理
		if(QUE_SEND_ZISEKI == m_ExecItem.emStat) {
			syslog( PARAM_SYSLOG_BASENO + 10, "[実績伝文:伝文区分(0:実績表 1:実績裏 2:状態表 3:状態裏)=%d retc=%d 命令No=%s ID=%s]", m_ExecItem.nSendNowNo, iRetc, m_ExecItem.cMeiNo, m_ExecItem.cKeyId);
			// 送信結果を反映
			mcls_pData->SetZisekiSendAnser( m_ExecItem.cKeyId, SockDataExec::FF_SENDKIND_ERR );
		} else if(QUE_SEND_MAE == m_ExecItem.emStat) {
			syslog( PARAM_SYSLOG_BASENO + 10, "[前工程伝文:retc=%d コイルNo=%s 命令No=%s]", iRetc, m_ExecItem.cCoilNo, m_ExecItem.cMeiNo);
		} else if(QUE_SEND_HELCY == m_ExecItem.emStat) {
			syslog( PARAM_SYSLOG_BASENO + 10, "[ヘルシー伝文:retc=%d]", iRetc);
		}
		return false;

	} else {

		// 個別処理
		if(QUE_SEND_ZISEKI == m_ExecItem.emStat) {
			LOG(em_MSG), "[%s] %s 送信完了 [伝文区分=%d 命令No=%s ID=%s]", my_sThreadName, GetSendStatName(m_ExecItem.emStat), m_ExecItem.nSendNowNo, m_ExecItem.cMeiNo, m_ExecItem.cKeyId);
			syslog( PARAM_SYSLOG_BASENO + 0, "[伝文区分(0:実績表 1:実績裏 2:状態表 3:状態裏)=%d 命令No=%s ID=%s]", m_ExecItem.nSendNowNo, m_ExecItem.cMeiNo, m_ExecItem.cKeyId);
			// 送信結果を反映
			mcls_pData->SetZisekiSendAnser( m_ExecItem.cKeyId, SockDataExec::FF_SENDKIND_ERR );
		} else if(QUE_SEND_MAE == m_ExecItem.emStat) {
			LOG(em_MSG), "[%s] %s 送信完了 [コイルNo=%s 命令No=%s]", my_sThreadName, GetSendStatName(m_ExecItem.emStat), m_ExecItem.cCoilNo, m_ExecItem.cMeiNo);
			syslog( PARAM_SYSLOG_BASENO + 3, "[コイルNo=%s 命令No=%s]", m_ExecItem.cCoilNo, m_ExecItem.cMeiNo);
		} else if(QUE_SEND_HELCY == m_ExecItem.emStat) {
			LOG(em_MSG), "[%s] %s 送信完了", my_sThreadName, GetSendStatName(m_ExecItem.emStat) );
		}


		//// ステータス変更
		m_eStateMain = SOCK_EXEC;
		m_ExecItem.nSendNowNo += 1;

		//// 応答伝文タイムアウト設定
		LARGE_INTEGER ts;
		ts.QuadPart = (__int64)PARAM_SOCKANSER_TIMEOUT * -10000;	// 1[ms]→100[ns] (マイナス指定で、指定時間経過でシグナル) (第3引数は2回目以降の起動間隔)
		SetWaitableTimer(m_evAnserTimeOut, &ts, 0, NULL, NULL, FALSE);

		return true;
	}
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
	int			nSize = que->Data->nSize;
	char		cTc[2];							// トランザクションコード確認用
	memcpy(cTc,  &data[0], sizeof(cTc));

	//// 生データダンプ
	CString dumpName;
	dumpName.Format("%s 受信(生)ダンプ (%s, No=%d, TC=%.2s).log", TO_FFSERV, 
					GetSendStatName(), m_ExecItem.nRecvNowNo, cTc );
	KizuFunction::DumpData( dumpName, nSize, data, mcls_pData->GetDumpMode());

	LOG(em_MSG), "[%s] 伝文受信 (%s, No=%d, TC=%.2s)", my_sThreadName, GetSendStatName(), m_ExecItem.nRecvNowNo, cTc );

	////////////////////////////////////
	// 1伝文 整合性確認 (ざっとしたところ)
	////////////////////////////////////
	//// ポート整合性チェック
	if(PORT_SEND == id)  {
		// 送信ポートでは受信する事はないのでエラーとする
		LOG(em_ERR), "[%s] 送信ポートで応答伝文は受信しません", my_sThreadName);
		syslog( PARAM_SYSLOG_BASENO + 12 , "[送信ポートで応答伝文を受信しました。]");
		return false;
	}

	//// シーケンス確認
	if( 0 == memcmp( cTc, FF_TC_CODE_ANS_O, sizeof(cTc)) || 0 == memcmp( cTc, FF_TC_CODE_ANS_U, sizeof(cTc)) ||
		0 == memcmp( cTc, FF_TC_CODE_STSANS_O, sizeof(cTc)) || 0 == memcmp( cTc, FF_TC_CODE_STSANS_U, sizeof(cTc))) {
		if( QUE_SEND_ZISEKI != m_ExecItem.emStat ) {
			LOG(em_ERR), "[%s] シーケンス不一致な応答伝文を受信。(%s, No=%d, TC=%.2s)", my_sThreadName, GetSendStatName(), m_ExecItem.nRecvNowNo, cTc );
			syslog( PARAM_SYSLOG_BASENO + 13, "[シーケンス不一致な応答伝文を受信。(%s, No=%d, TC=%.2s)]", GetSendStatName(), m_ExecItem.nRecvNowNo, cTc );
			return false;
		}
		if( (0 == m_ExecItem.nRecvNowNo && 0 != memcmp( cTc, FF_TC_CODE_ANS_O, sizeof(cTc))) ||
			(1 == m_ExecItem.nRecvNowNo && 0 != memcmp( cTc, FF_TC_CODE_ANS_U, sizeof(cTc))) ||
			(2 == m_ExecItem.nRecvNowNo && 0 != memcmp( cTc, FF_TC_CODE_STSANS_O, sizeof(cTc))) ||
			(3 == m_ExecItem.nRecvNowNo && 0 != memcmp( cTc, FF_TC_CODE_STSANS_U, sizeof(cTc))) ) {
			if (2 == m_ExecItem.nRecvNowNo || 3 == m_ExecItem.nRecvNowNo){
				LOG(em_ERR), "[%s] 表裏不一致な応答伝文を受信。(%s(検査状態), No=%d, TC=%.2s)", my_sThreadName, GetSendStatName(), m_ExecItem.nRecvNowNo, cTc );
				syslog(PARAM_SYSLOG_BASENO + 7, "[表裏不一致な応答伝文を受信。(%s(検査状態), No=%d, TC=%.2s)]", GetSendStatName(), m_ExecItem.nRecvNowNo, cTc);
			} else {
				LOG(em_ERR), "[%s] 表裏不一致な応答伝文を受信。(%s(実績), No=%d, TC=%.2s)", my_sThreadName, GetSendStatName(), m_ExecItem.nRecvNowNo, cTc );
				syslog( PARAM_SYSLOG_BASENO + 7 , "[表裏不一致な応答伝文を受信。(%s(実績), No=%d, TC=%.2s)]", GetSendStatName(), m_ExecItem.nRecvNowNo, cTc );
			}
			return false;
		}

	} else if( 0 == memcmp( cTc, FF_TC_CODE_MAEJI_O, sizeof(cTc)) ||
		0 == memcmp( cTc, FF_TC_CODE_MAEJI_U, sizeof(cTc)) ) {
//		if( QUE_SEND_MAE != m_ExecItem.emStat ) {
		if( QUE_SEND_MAE != m_ExecItem.emStat || 1 < m_ExecItem.nRecvNowNo ) { // ﾚｺｰﾄﾞNoﾁｪｯｸしないと裏の命令No不一致時に再度応答を受けると例外
			LOG(em_ERR), "[%s] シーケンス不一致な応答伝文を受信。(%s, No=%d, TC=%.2s)", my_sThreadName, GetSendStatName(), m_ExecItem.nRecvNowNo, cTc );
			syslog( PARAM_SYSLOG_BASENO + 13 , "[シーケンス不一致な応答伝文を受信。(%s, No=%d, TC=%.2s)]", GetSendStatName(), m_ExecItem.nRecvNowNo, cTc );
			return false;
		}
		if( (0 == m_ExecItem.nRecvNowNo && 0 != memcmp( cTc, FF_TC_CODE_MAEJI_O, sizeof(cTc))) ||
			(1 == m_ExecItem.nRecvNowNo && 0 != memcmp( cTc, FF_TC_CODE_MAEJI_U, sizeof(cTc))) ) {
			LOG(em_ERR), "[%s] 表裏不一致な応答伝文を受信。(%s, No=%d, TC=%.2s)", my_sThreadName, GetSendStatName(), m_ExecItem.nRecvNowNo, cTc );
			syslog( PARAM_SYSLOG_BASENO + 7 , "[表裏不一致な応答伝文を受信。(%s, No=%d, TC=%.2s)]", GetSendStatName(), m_ExecItem.nRecvNowNo, cTc );
			return false;
		}

	} else if( 0 == memcmp( cTc, FF_TC_CODE_HELCY, sizeof(cTc)) ) {
		if( QUE_SEND_HELCY != m_ExecItem.emStat ) {
			LOG(em_ERR), "[%s] シーケンス不一致な応答伝文を受信。(%s, No=%d, TC=%.2s)", my_sThreadName, GetSendStatName(), m_ExecItem.nRecvNowNo, cTc );
			syslog( PARAM_SYSLOG_BASENO + 13, "[シーケンス不一致な応答伝文を受信。(%s, No=%d, TC=%.2s)]", GetSendStatName(), m_ExecItem.nRecvNowNo, cTc );
			return false;
		}

	} else {
		LOG(em_ERR), "[%s] 不明応答伝文を受信しました。(%s, No=%d, TC=%.2s)", my_sThreadName, GetSendStatName(), m_ExecItem.nRecvNowNo, cTc );
		syslog( PARAM_SYSLOG_BASENO + 13 , "[不明応答伝文を受信しました。(%s, No=%d, TC=%.2s)]", GetSendStatName(), m_ExecItem.nRecvNowNo, cTc );
		return false;
	}


	////////////////////////////////////
	// 業務伝文処理
	////////////////////////////////////
	//// 内容確認
	int nAnser = SetRecvData( &m_ExecItem, nSize, data, cTc );
	if( RECV_RET_ERR_NONE == nAnser ) {
		// 応答伝文待ちタイムアウトキャンセル
		CancelWaitableTimer(m_evAnserTimeOut);
		LOG(em_ERR), "[%s] シーケンス異常完了 <<<==================", my_sThreadName);

		return false;
	}
	else if( RECV_RET_ERR_IDLE == nAnser ) {
		// 応答伝文待ちタイムアウトキャンセル
		CancelWaitableTimer(m_evAnserTimeOut);

		//// ステータス変更
		m_eStateMain = SOCK_IDLE;
		LOG(em_ERR), "[%s] シーケンス異常完了 <<<==================", my_sThreadName);
	}
	else if(RECV_RET_OK == nAnser) {
		// 応答伝文待ちタイムアウトキャンセル
		CancelWaitableTimer(m_evAnserTimeOut);	
		
		//// ステータス変更
		m_eStateMain = SOCK_IDLE;
		LOG(em_MSG), "[%s] シーケンス完了 <<<==================", my_sThreadName);
	}
	else if( RECV_RET_NEXT == nAnser ) {
		// 次伝文送信開始
		if( ! SendExec() ) return false;
	}
	else if( RECV_RET_NONEXT == nAnser ) {
		// 何もしない
	}


	//// 次受信開始
	// 受信ポートを 受信待ちにしておく。
	if( ! RecvStart( PORT_RECV, sizeof(FF_ANS), m_cSockBuf[PORT_RECV][BUF_RECV] )) {
		return false;
	}
	return true;
}



// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 固有処理

//------------------------------------------
// 受信データ処理
// ExecItem* item 送受信データ
// int nSize 受信サイズ
// BYTE const* data 受信データ
// char const* cTc 受信トランザクションコード
// 戻り値
// -9:かなり異常(切断処理必要) -1:異常(切断はしない)  0:1シーケンスすべて完了
//  1:次送信処理を実施すること  9:次受信処理を実施すること(=何もしない)
//------------------------------------------
int SockManager::SetRecvData( ExecItem* item, int nSize, BYTE const* data, char const* cTc )
{
	int nAnser = 0;
  	char cMeiNo[SIZE_MEI_NO+1];		// 命令No

	//// データの更新
	int rec = item->nRecvNowNo;
	item->nRecvSize[rec] = nSize;
	memcpy(item->cRecvBuf[rec], data, nSize);
	item->nRecvNowNo ++;


	//// 各伝文生成
//-----------------------------------------------------------------------------------------------
	if(QUE_SEND_ZISEKI == item->emStat) {								// 疵実績伝文
		FF_ANS& pBuf = (FF_ANS&)item->cRecvBuf[rec];
		
		//// 整合性確認
		// 処理ステータスチェック
		SockDataExec::FF_SENDKIND_NO emKind;

		memset( cMeiNo, 0x00, sizeof(cMeiNo));
		memcpy( cMeiNo, pBuf.Hd.Meino, SIZE_MEI_NO );

		if( 0 == memcmp(pBuf.Data.Status, FF_ANS_OK, sizeof(pBuf.Data.Status)) ) {
			// 正常
			LOG(em_MSG), "[%s] 応答伝文処理ステータス 正常 [命令No=%s]", my_sThreadName, cMeiNo);
			syslog( PARAM_SYSLOG_BASENO + 1, "[命令No=%s]", cMeiNo);
			emKind = SockDataExec::FF_SENDKIND_OK;		// 正常送信完了時

		} else if( 0 == memcmp(pBuf.Data.Status, FF_ANS_IPCHK_NG, sizeof(pBuf.Data.Status)) ) {
			// インプットチェックNG
			LOG(em_MSG), "[%s] 応答伝文処理ステータス インプット異常 [命令No=%s]", my_sThreadName, cMeiNo);
			syslog( PARAM_SYSLOG_BASENO + 2, "[ID=%s, インプットチェックエラー]", item->cKeyId);
			emKind = SockDataExec::FF_SENDKIND_NOTSEND;	// 再送信なし
		
		} else if( 0 == memcmp(pBuf.Data.Status, FF_ANS_DBERRER, sizeof(pBuf.Data.Status)) ) {
			// DB登録異常
			LOG(em_MSG), "[%s] 応答伝文処理ステータス DB異常 [命令No=%s]", my_sThreadName, cMeiNo);
			syslog( PARAM_SYSLOG_BASENO + 2, "[ID=%s, DB登録エラー]", item->cKeyId);
			emKind = SockDataExec::FF_SENDKIND_ERR;		// 再送信あり
		
		} else {
			LOG(em_MSG), "[%s] 応答伝文処理ステータス 不明 [命令No=%s][%.2s]", my_sThreadName, cMeiNo, pBuf.Data.Status);
			syslog( PARAM_SYSLOG_BASENO + 2, "[ID=%s, 不明=%.2s]", item->cKeyId, pBuf.Data.Status);
			emKind = SockDataExec::FF_SENDKIND_ERR;		// 再送信あり
		}
		
		//// 次伝文を送受信
		if( SockDataExec::FF_SENDKIND_OK == emKind &&
			item->nRecvNowNo != item->nRecvMaxNo ) {
			return RECV_RET_NEXT;
		}

		//// 今回で終わり
		// DBに結果反映
		mcls_pData->SetZisekiSendAnser(item->cKeyId, emKind);
		// 送受信データ初期化
		memset(item, 0x00, sizeof(ExecItem));
		// 次送信依頼
		AddQue_SendZiseki();

		if(SockDataExec::FF_SENDKIND_OK != emKind)  return RECV_RET_ERR_IDLE;
		else										return RECV_RET_OK;
	}

//-----------------------------------------------------------------------------------------------
	else if( QUE_SEND_MAE == item->emStat) {							// 前工程実績要求
		FF_MAE_JISSEKI& pBuf = (FF_MAE_JISSEKI&)item->cRecvBuf[rec];

		memset( cMeiNo, 0x00, sizeof(cMeiNo));
		memcpy( cMeiNo, pBuf.Hd.Meino, SIZE_MEI_NO );
		
		//// 整合性確認
		// 命令Noチェック
		if( 0 != strcmp(cMeiNo, item->cMeiNo) ) {
			LOG(em_MSG), "[%s] 応答伝文 命令No不一致 [%s/%s]", my_sThreadName, cMeiNo, item->cMeiNo);
			syslog( PARAM_SYSLOG_BASENO + 5, "[命令No不一致:%s/%s]", cMeiNo, item->cMeiNo);
			return RECV_RET_ERR_IDLE;

		}

		if( 1 == rec ){ // 応答伝文(裏)の時 
			FF_MAE_JISSEKI& pBufO = (FF_MAE_JISSEKI&)item->cRecvBuf[0];
			if( 0 != strncmp(pBufO.Hd.Meino, item->cMeiNo, SIZE_MEI_NO) ){ // 表の命令No不一致
				LOG(em_MSG), "[%s] 応答伝文(表) 命令No不一致のため保存しません [%.*s/%s]", my_sThreadName, SIZE_MEI_NO, pBufO.Hd.Meino, item->cMeiNo);
//				syslog( PARAM_SYSLOG_BASENO + 5, "[命令No不一致:%.*s/%s]", SIZE_MEI_NO, pBufO.Hd.Meino, item->cMeiNo); // 表受信時に出しているはずなので不要
				return RECV_RET_ERR_IDLE;
			}
		}

		//// 次伝文を受信
		if(item->nRecvNowNo != item->nRecvMaxNo ) {
			return RECV_RET_NONEXT;
		}

		//// 今回で終わり
		syslog( PARAM_SYSLOG_BASENO + 4 , "[命令No=%s]", item->cMeiNo);
		// DBに結果反映
		mcls_pData->InsertMaeBuf(item);
		// 送受信データ初期化
		memset(item, 0x00, sizeof(ExecItem));
		return RECV_RET_OK;
	}

//-----------------------------------------------------------------------------------------------
	else if( QUE_SEND_HELCY == item->emStat) {							// ヘルシー
		//// 今回で終わり
		LOG(em_MSG), "[%s] ヘルシー応答伝文 受信完了", my_sThreadName);
		syslog( PARAM_SYSLOG_BASENO + 6, "");
		
		//// 次回ヘルシー伝文送信タイマー
		SetHelcySendTimer(PARAM_HELCYSEND_TIMER);
		return RECV_RET_OK;
	}

//-----------------------------------------------------------------------------------------------
	else {
		LOG(em_ERR), "[%s] 不明伝文 受信！！", my_sThreadName);
		return RECV_RET_ERR_NONE;
	}
}