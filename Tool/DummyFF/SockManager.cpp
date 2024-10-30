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
m_eStateMain(SOCK_NONE),
m_nSt(0)
{

}

//------------------------------------------
// デストラクタ
//------------------------------------------
SockManager::~SockManager(void)
{

}

//------------------------------------------
// スレッド開始前処理 (スレッド実行直後(シグナルセット前)にコールバック)
//------------------------------------------
int SockManager::ThreadFirst()
{
	//// TCP/IP 操作クラス初期化
	SockInit(); 

	// WaitForMultipleObjectsイベント
	HANDLE	hArray[] = {mcls_pSock[PORT_RECV]->gque_Anser.g_evSem,		// 受信ポート[0] 用操作クラス
						mcls_pSock[PORT_SEND]->gque_Anser.g_evSem,		// 送信ポート[1] 用操作クラス
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


	enum {	EV_SOCK_RECV_ANS,					// 受信ポート 操作結果
			EV_SOCK_SEND_ANS,					// 送信ポート 操作結果
			EV_QUE_DELI							// 送信依頼キューイベント
			};

	////// シグナル条件分け
	switch (nEventNo) {
		
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
	//if( SOCK_EXEC == m_eStateMain ) *nNowWaitNum = nDefWaitNum-1;
	//else							*nNowWaitNum = nDefWaitNum;
	*nNowWaitNum = nDefWaitNum;
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
	if (st == QUE_SEND_ZISEKI_T) { wk = "疵実績送信　表"; }
	else if (st == QUE_SEND_ZISEKI_B) { wk = "疵実績送信　裏"; }
	else if (st == QUE_SEND_STAT_T) { wk = "検査状態送信　表"; }
	else if (st == QUE_SEND_STAT_B) { wk = "検査状態送信　裏"; }
	else if (st == QUE_SEND_MAE_T) { wk = "前工程実績要求　表"; }
	else if (st == QUE_SEND_MAE_B) { wk = "前工程実績要求　裏"; }
	else if (st == QUE_SEND_HELCY) { wk = "ヘルシー"; }
	else { wk = ""; }
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

			LOG(em_MSG), "[%s] 回線正常 ===================================", my_sThreadName);

			// 受信ポートを 受信待ちにしておく。
			if( ! RecvStart( PORT_RECV, sizeof(FF_ANS), m_cSockBuf[PORT_RECV][BUF_RECV] )) {
				UserClose();
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
	deli.bSorC = true;
	deli.bRorS = true;
	strcpy_s (deli.cMyName,  "[受信ポート]");
	sprintf(deli.cSession, "%s_RECV", DummyFF);
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
	deli.bSorC = true;
	deli.bRorS = false;
	strcpy_s (deli.cMyName,  "[送信ポート]");
	sprintf(deli.cSession, "%s_SEND", DummyFF);
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
	if( m_bIsConnect ) {		// 今接続中？
		// どのポートからでも クローズ通知が来たら、全ポート強制終了させる
		mcls_pSock[PORT_RECV]->UserClose();
		mcls_pSock[PORT_SEND]->UserClose();
		// 機器状態に通知
		KizuFunction::SetStatus("FF_CONNECT_ERR_ID", false, false);
		LOG(em_ERR), "[%s] 回線異常 ===================================", my_sThreadName);
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

	LOG(em_MSG), "[%s] シーケンス終了 (%s) <<<==================", my_sThreadName, GetSendStatName());
	return true;
}

//------------------------------------------
// 業務伝文送信 共通処理
//------------------------------------------
bool SockManager::SendExec()
{
	//// 同期送信
	memcpy(	m_cSockBuf[PORT_SEND][BUF_SEND], m_ExecItem.cSendBuf, m_ExecItem.nSendSize);
	//// 生データダンプ
	CString dumpName;
	dumpName.Format("%s 送信(生)ダンプ (%s).log", TO_FFSERV, GetSendStatName() );
	KizuFunction::DumpData( dumpName, m_ExecItem.nSendSize, m_cSockBuf[PORT_SEND][BUF_SEND], mcls_pData->GetDumpMode());
	int iRetc = mcls_pSock[PORT_SEND]->LockSend( m_cSockBuf[PORT_SEND][BUF_SEND], m_ExecItem.nSendSize, PARAM_SOCK_TIMEOUT );

	if(0 != iRetc) {
		LOG(em_ERR), "[%s] %s 送信失敗 [命令No=%s コイルNo=%s]", my_sThreadName, GetSendStatName(m_ExecItem.emStat), m_ExecItem.cMeiNo, m_ExecItem.cCoilNo);
		LOG(em_ERR), "[%s] シーケンス異常完了 <<<==================", my_sThreadName);

		return false;

	} else {
		
		//// ステータス変更
		m_eStateMain = SOCK_EXEC;
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
	static char	cTc[2];							// トランザクションコード確認用
	memcpy(cTc,  data, sizeof(cTc));
	//// 生データダンプ
	CString dumpName;
	dumpName.Format("%s 受信(生)ダンプ (TC=%.2s).log", DummyFF,cTc);
	KizuFunction::DumpData(dumpName, nSize, data, mcls_pData->GetDumpMode());

	
	LOG(em_MSG), "[%s] 伝文受信 (TC=%.2s)", my_sThreadName, cTc );

	////////////////////////////////////
	// 1伝文 整合性確認 (ざっとしたところ)
	////////////////////////////////////
	//// ポート整合性チェック
	if(PORT_SEND == id)  {
		// 送信ポートでは受信する事はないのでエラーとする
		LOG(em_ERR), "[%s] 送信ポートで応答伝文は受信しません", my_sThreadName);
		return false;
	}

	////// シーケンス確認
	//if (0 == memcmp(cTc, FF_TC_CODE_JI_O, sizeof(cTc))) {

	//}
	//else if (0 == memcmp(cTc, FF_TC_CODE_JI_U, sizeof(cTc))) {

	//}
	//else if (0 == memcmp(cTc, FF_TC_CODE_STSJI_O, sizeof(cTc))) {

	//}
	//else if (0 == memcmp(cTc, FF_TC_CODE_STSJI_U, sizeof(cTc))) {

	//}	
	//else if( 0 == memcmp( cTc, FF_TC_CODE_MAEREQ, sizeof(cTc))){

	//}
	//else if( 0 == memcmp( cTc, FF_TC_CODE_HELCY, sizeof(cTc)) ) {

	//} else {
	//	LOG(em_ERR), "[%s] 不明応答伝文を受信しました。(%s, No=%d, TC=%.2s)", my_sThreadName, GetSendStatName(), m_ExecItem.nRecvNowNo, cTc );
	//	return false;
	//}


	////////////////////////////////////
	// 業務伝文処理
	////////////////////////////////////
	//// 内容確認
	if( -1 == SetRecvData( data, cTc ) ) return false ;
	//if( RECV_RET_ERR_NONE == nAnser ) {
	//	// 応答伝文待ちタイムアウトキャンセル
	//	CancelWaitableTimer(m_evAnserTimeOut);
	//	LOG(em_ERR), "[%s] シーケンス異常完了 <<<==================", my_sThreadName);

	//	return false;
	//}
	//else if( RECV_RET_ERR_IDLE == nAnser ) {
	//	// 応答伝文待ちタイムアウトキャンセル
	//	CancelWaitableTimer(m_evAnserTimeOut);

	//	//// ステータス変更
	//	m_eStateMain = SOCK_IDLE;
	//	LOG(em_ERR), "[%s] シーケンス異常完了 <<<==================", my_sThreadName);
	//}
	//else if(RECV_RET_OK == nAnser) {
	//	// 応答伝文待ちタイムアウトキャンセル
	//	CancelWaitableTimer(m_evAnserTimeOut);	
	//	
	//	//// ステータス変更
	//	m_eStateMain = SOCK_IDLE;
	//	LOG(em_MSG), "[%s] シーケンス完了 <<<==================", my_sThreadName);
	//}
	//else if( RECV_RET_NEXT == nAnser ) {
	//	// 次伝文送信開始
	//	if( ! SendExec() ) return false;
	//}
	//else if( RECV_RET_NONEXT == nAnser ) {
	//	// 何もしない
	//}


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
// BYTE const* data 受信データ
// char const* cTc 受信トランザクションコード
// 戻り値
// -9:かなり異常(切断処理必要) -1:異常(切断はしない)  0:1シーケンスすべて完了
//  1:次送信処理を実施すること  9:次受信処理を実施すること(=何もしない)
//------------------------------------------
int SockManager::SetRecvData( BYTE const* data, char* cTc )
{
	int nAnser = 0;
  	char cMeiNo[SIZE_MEI_NO+1];		// 命令No
	CString	cWk;
	SYSTEMTIME STime;
	GetLocalTime(&STime);
	

	//// 各伝文生成
//-----------------------------------------------------------------------------------------------
	if (0 == memcmp(cTc, FF_TC_CODE_JI_O, sizeof(cTc)) ||
		0 == memcmp(cTc, FF_TC_CODE_JI_U, sizeof(cTc))) {				// 疵実績伝文
		FF_JISSEKI *pRecv = (FF_JISSEKI*)data;

		Delivery* deli = new Delivery;
		memset(deli, 0x00, sizeof(Delivery));
		// 伝文コード
		if (0 == memcmp(cTc, FF_TC_CODE_JI_O, sizeof(cTc))) {
			deli->eStat = QUE_SEND_ZISEKI_T;
		}
		else {
			deli->eStat = QUE_SEND_ZISEKI_B;
		}
		// 工程コード
		memcpy(deli->cLine_Code, LINE_CODE, sizeof(deli->cLine_Code));

		// 命令No
		memcpy(deli->cMeiNo, pRecv->Hd.Meino, sizeof(deli->cMeiNo));

		// 冷延コイルNo
		memcpy(deli->cCoilNo, pRecv->Hd.Coilno, sizeof(deli->cCoilNo));

		gque_Deli.AddToTail(deli);
	}
//-----------------------------------------------------------------------------------------------
	else if(	0 == memcmp(cTc, FF_TC_CODE_STSJI_O, sizeof(cTc) )||
		0 == memcmp(cTc, FF_TC_CODE_STSJI_U, sizeof(cTc) ) ){				// 検査状態伝文
		FF_STS_JISSEKI *pRecv = (FF_STS_JISSEKI*) data;

		Delivery* deli = new Delivery;
		memset(deli, 0x00, sizeof(Delivery));
		// 伝文コード
		if (0 == memcmp(cTc, FF_TC_CODE_STSJI_O, sizeof(cTc))) {
			deli->eStat = QUE_SEND_STAT_T;
		} else {
			deli->eStat = QUE_SEND_STAT_B;
		}
		// 工程コード
		memcpy(deli->cLine_Code, LINE_CODE, sizeof(deli->cLine_Code));

		// 命令No
		memcpy(deli->cMeiNo, pRecv->Hd.Meino, sizeof(deli->cMeiNo));

		// 冷延コイルNo
		memcpy(deli->cCoilNo, pRecv->Hd.Coilno, sizeof(deli->cCoilNo));

		gque_Deli.AddToTail(deli);
	}
//-----------------------------------------------------------------------------------------------
	else if (0 == memcmp(cTc, FF_TC_CODE_MAEREQ, sizeof(cTc))) {			// 前工程実績要求
		for(int ii=0; ii<2; ii++) {
			FF_MAE_REQUEST *pRecv = (FF_MAE_REQUEST*) data;

			Delivery* deli = new Delivery;
			memset(deli, 0x00, sizeof(Delivery));
			// 伝文コード
			deli->eStat = 0== ii? QUE_SEND_MAE_T : QUE_SEND_MAE_B;

			// 送信伝文ファイル名
			cWk.Format(FILE_FORMAT, pRecv->MeireiNo, ii+1);
			memcpy(deli->cFileName, cWk, sizeof(deli->cFileName));

			gque_Deli.AddToTail(deli);
		}
	}

//-----------------------------------------------------------------------------------------------
	else if(0 == memcmp(cTc, FF_TC_CODE_HELCY, sizeof(cTc))) {					// ヘルシー
		FF_MAE_REQUEST *pRecv = (FF_MAE_REQUEST*)data;

		Delivery* deli = new Delivery;
		memset(deli, 0x00, sizeof(Delivery));
		// 伝文コード
		deli->eStat = QUE_SEND_HELCY;
		gque_Deli.AddToTail(deli);

	}

//-----------------------------------------------------------------------------------------------
	else {
		LOG(em_ERR), "[%s] 不明伝文 受信！！", my_sThreadName);
		return -1;
	}
	return 0;
}