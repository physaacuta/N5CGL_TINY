#include "StdAfx.h"
#include "DummyLEDManager.h"

// メモリーリーク検出 ----->>>
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif
// <<<------------------------

//------------------------------------------
// コンストラクタ
// char const* cName 名前 (iniファイルのキーとなる)
// int nId		     ID (0オリジン) (セクションの何番目)
// int nBlock        配光ブロック数
// int nTempUnit     LED温度計測定点数
//------------------------------------------
DummyLedManager::DummyLedManager(char const* cName, int nId, int nBlock, int nTempUnit) :
ThreadManager(cName),
m_nMyID(nId),
m_bIsConnect(false),
m_emMainMode(MODE_NONE),
m_nNowScode(MAX_SCODE)					// 表面状態は、デフォルト表面状態とする
{
	CString  sSession;

	// 自分の設定キーを決定
	m_cMySession.Format("%s_%s", DummyLED, cName );

	//// 相手の設定キーを決定
	m_cYourSession.Format("%s_%s", KT_LEDMGR, cName );
	m_nMyMen		= GetPrivateProfileInt(m_cYourSession, "MEN",		0, TASKINI_NAME);
	m_nChNum		= GetPrivateProfileInt(m_cYourSession, "CH_NUM",	0, TASKINI_NAME);
	_ASSERT(0!=m_nChNum);


	//// 使用するCHの決定
	memset(&m_bUsedCh, 0x00, sizeof(m_bUsedCh));
	int	nChCnt = 0;
	char cWk[10];
	for(int ii=0; ii<MAX_LED_CH; ii++){
		sSession.Format("%s_%s_CH%d", KT_LEDMGR, cName, ii+1);
		GetPrivateProfileSection(sSession, cWk, 10, TASKINI_NAME);

		if( 0 != strlen(cWk)){
			m_bUsedCh[ii] = true;
			nChCnt++;
		}
	}
	_ASSERT(nChCnt >= m_nChNum);

	for(int ii=0; ii<MAX_LED_CH-1; ii++){
		// 未使用CHの場合は次へ
		if( !m_bUsedCh[ii] )	continue;

		sSession.Format("%s_%s_CH%d", KT_LEDMGR, cName, ii+1 );
		m_nBlock[ii]    = GetPrivateProfileInt(sSession, "BLOCK",     nBlock,    TASKINI_NAME);		// 配光ブロック数(未定義の場合、共通設定のブロック数)
		m_nMyIndex[ii]  = GetPrivateProfileInt(sSession, "INDEX",     ii+1,      TASKINI_NAME);		// 対応するIndex(未定義の場合、Ch番号)
		m_nTempUnit[ii] = GetPrivateProfileInt(sSession, "TEMP_UNIT", nTempUnit, TASKINI_NAME);		// LED温度計測点数(未定義の場合、共通設定のLED温度計測点数)
	}

	//// 初期化
	for(int ii=0; ii<PARAM_SOCK_MAX; ii++) {
		mcls_pSock[ii]		= NULL;
		m_eStateMain[ii]	= SOCK_NONE;
	}
	for(int ii=0; ii<MAX_LED_CH; ii++) {
		m_nNowLedBase[ii]	= 0;
		m_nStateBreak[ii]	= 0;
		m_nStateTemp[ii]	= 0;
		m_nStateOnOff[ii]	= 1;
		for (int jj=0; jj<MAX_LED_BLOCK; jj++) {
			m_nNowLedHosei[ii][jj]	= 0;
		}

		for (int jj=0; jj<MAX_TEMP_UNIT; jj++) {
			m_nTemp[ii][jj]		= 20;
			m_nTempSts[ii][jj]	= 0;
		}
		m_nLedOnOff[ii] = 0;
	}
	memset( m_cSockBuf, 0x00, sizeof(m_cSockBuf));

}

//------------------------------------------
// デストラクタ
//------------------------------------------
DummyLedManager::~DummyLedManager(void)
{
}


//------------------------------------------
// スレッド開始前処理 (スレッド実行直後(シグナルセット前)にコールバック)
//------------------------------------------
int DummyLedManager::ThreadFirst()
{
	// TCP/IP 操作クラス初期化
	SockInit(); 			

	//// イベント設定
	HANDLE hArray[] = { mcls_pSock[PORT_SEND]->gque_Anser.g_evSem,
						mque_SendItem.g_evSem
						};
	SetEventNum(sizeof(hArray)/sizeof(HANDLE), hArray);

	return 0;
}


//------------------------------------------
// スレッド終了前処理 (終了シグナル後にコールバック)
//------------------------------------------
int DummyLedManager::ThreadLast()
{
	// TCP/IP 操作クラス開放
	SockExit();
	return 0;
}
//------------------------------------------
// スレッドのWaitFor待ちの直前処理  
// int nDefWaitNum 元の数
// int* nNowWaitNum 変更後の数
//------------------------------------------
void DummyLedManager::ThreadWaitFor(int nDefWaitNum, int* nNowWaitNum)
{
	// 受信中は、送信要求を受け付けないための処置
	if( SOCK_EXEC == m_eStateMain[ID_SEND] )		*nNowWaitNum = nDefWaitNum-1;
	else											*nNowWaitNum = nDefWaitNum;
}

//------------------------------------------
// スレッドイベント発生
// int nEventNo イベントNo (0オリジン)
//------------------------------------------
void DummyLedManager::ThreadEvent(int nEventNo)
{
	BaseTcpIp::DELIVERY_ANS*	queAns  = NULL;			// TCP/IPクラス 操作結果

	////// シグナルの条件
	enum {	EV_SOCK_SEND_ANS = 0,			// 送信ポート 操作結果
			EV_SOCK_SEND_REQ
	};


	////// シグナル条件分け
	switch (nEventNo) {

	//-----------------------------------------------------------------------------------------------
	case EV_SOCK_SEND_ANS:											// 送信ポート 操作結果

		queAns = mcls_pSock[PORT_SEND]->gque_Anser.GetFromHead(); 

		// 操作結果の処理
		ExecAnsQue(queAns->nMyID, queAns);

		// 取り出した操作結果を解放
		SAFE_DELETE(queAns->Data);
		SAFE_DELETE(queAns);
		break;

//-----------------------------------------------------------------------------------------------

	case EV_SOCK_SEND_REQ:

		if(true) {
			// 送信データ取り出し
			INF_SEND* pDeli = mque_SendItem.GetFromHead(); 
			memcpy(&m_InfSend, pDeli, sizeof(m_InfSend));
			delete pDeli;

			// 状態確認
			if( MODE_NONE == m_emMainMode) break;

			// 送信
			int retc;
			if( ! SendStart(&retc) ){
				LOG(em_ERR), "[%s] UserClose SendStart Err", my_sThreadName);
				UserClose();
			}
		}
		break;
	}
}

//------------------------------------------
// TCP/IPクラス 操作結果の処理
// int id ポートID (よく使うから引数かしておく)
// BaseTcpIp::DELIVERY_ANS* que 操作結果
//------------------------------------------
void DummyLedManager::ExecAnsQue(int id, BaseTcpIp::DELIVERY_ANS* que)
{
	switch(que->state) {

	//-----------------------------------------------------------------------------------------------
	case BaseTcpIp::ANS_CLOSE :										// クローズ通知
		if( KIZUSOCK_CLOSEKIND_SOCK == mcls_pSock[id]->GetCloseKind() ) {
			// 相手から切断
			LOG(em_ERR), "[%s][%s] 相手から切断要求！！", my_sThreadName, mcls_pSock[id]->GetMyName());
		}
		// 切断処理実行
		LOG(em_ERR), "[%s] UserClose 切断処理実行", my_sThreadName);
		UserClose();
		break;

	//-----------------------------------------------------------------------------------------------
	case BaseTcpIp::ANS_CONNECT :									// コネクト完了通知
		if( BaseTcpIp::STATE_NONE != mcls_pSock[PORT_SEND]->GetSockState() ) {

			// 整合性チェック (コネクト通知が来る前に、上記IF文で、両ポート正常になる可能性がある対策)
			if(m_bIsConnect) break;

			for(int ii=0; ii<PARAM_SOCK_MAX; ii++) {
				m_eStateMain[ii] = SOCK_IDLE;
			}
			
			// 接続したので総合モードを待ち状態にする
			m_emMainMode = MODE_WAIT;

			// 送信ポートを 受信待ちにしておく。
			if( ! RecvStart( PORT_SEND, PARAM_COM_BUFSIZE, m_cSockBuf[id][BUF_RECV] )) {
				mcls_pSock[PORT_SEND]->UserClose();
				break;
			}
		}
		break;

	//-----------------------------------------------------------------------------------------------
	case BaseTcpIp::ANS_RECVEND :									// 非同期 受信完了通知
		if( 0 != que->Data->nAns) {
			// 正常受信完了じゃないので切断しよう
			LOG(em_ERR), "[%s] UserClose 正常受信完了じゃない Ans:%d", my_sThreadName, que->Data->nAns);
			UserClose();
		} else {
			// 受信完了時の処理を行う
			int mode = RecvEnd(id, que);

			// 処理わけ
			if(-1 == mode) {										// 異常
				// 異常だから切断
				LOG(em_ERR), "[%s] UserClose 異常だから切断", my_sThreadName);
				UserClose();

			} else if(9 == mode) {									// 送信許可を受信
				Sleep(110);											// すぐに送ると異常になるため、ちょっと待ってから送信
				m_eStateMain[ID_SEND] = SOCK_IDLE;

			} else if(0 == mode) {									// 引き続き受信
				// 引き続き受信の為、何もしない

			} else if(1 == mode) {									// 受信コマンド受信
				// コマンドに対する受信
				memset(&m_InfSend, 0x00, sizeof(m_InfSend));		// 送信コマンドを初期化しておく
			}
		}
		break;

	//-----------------------------------------------------------------------------------------------
	case BaseTcpIp::ANS_SENDEND :									// 送信完了通知
		// 非同期送信は 今回は、行わない (処理簡略化の為)
		break;
	
	//-----------------------------------------------------------------------------------------------
	default:			// ありえない
		LOG(em_ERR), "[%s] モード異常 [%d]", my_sThreadName, que->state);
		break;
	}
}

// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 送受信処理

//------------------------------------------
// キューに溜まっている送信依頼を全て削除
// 戻り値 復帰情報
//------------------------------------------
void DummyLedManager::DeleteAllQue()
{
	INF_SEND* pDeli = NULL;

	while(true) {
		pDeli = mque_SendItem.GetFromHead(0);
		if( pDeli == NULL ) break;
		
		// 解放
		delete pDeli;
	}
}

//------------------------------------------
// 送信コマンド
// EM_LED_SEND_COMMAND em 送信コマンド
// int	nCh 対象チャンネル
// 戻り値 復帰情報
//------------------------------------------
bool DummyLedManager::AddQue_SendCommand(EM_LED_SEND_COMMAND em, int nCh)
{
	int ii_s, ii_e;

	//	//======================================================
	// 事前チェック

	if( !m_bUsedCh[nCh] ) return true;	// 未使用CHの場合、送信しない

	//======================================================
	//// 生成
	INF_SEND* pDeli = new INF_SEND;
	memset(pDeli, 0x00, sizeof(INF_SEND));

	pDeli->emLedCommand = em;
	pDeli->nSize = PARAM_COM_BUFSIZE;
	pDeli->nCh	 = nCh+1;

	//// コマンドセット
	pDeli->cBuf[0] = 'A';
	switch( em ) {
		case SEND_NEXT_SEND_OK:
			pDeli->cBuf[1] = 'C';
			break;
		case SEND_STATE_ON:
			pDeli->cBuf[1] = 'x';
			break;
		case SEND_STATE_OFF:
			pDeli->cBuf[1] = 'y';
			break;
		case SEND_STATE_BREAK:
			pDeli->cBuf[1] = 'A';
			break;
		case SEND_STATE_HOT:
			pDeli->cBuf[1] = 'B';
			break;
		case SEND_STATE_SHATDOWN:
			pDeli->cBuf[1] = 'D';
			break;
		case SEND_STATE_OK:
			pDeli->cBuf[1] = 'N';
			break;
		case SEND_READ_EEPROM:
			pDeli->cBuf[1] = 'E';
			pDeli->cBuf[5] = m_nNowLedBase[nCh];
			memcpy( &pDeli->cBuf[6], m_nNowLedHosei[nCh],m_nBlock[nCh]);
			pDeli->nSize = PARAM_COM_BUFSIZE + m_nBlock[nCh] + 1;
			break;
		//case SEND_READ_RAM:
		//	pDeli->cBuf[1] = 'P';
		//	pDeli->cBuf[5] = 255;
		//	for(int ii=0; ii<m_nBlock[nCh]; ii++) pDeli->cBuf[6+ii] = 1;
		//	break;
		case SEND_LED_TEMP:
			pDeli->cBuf[1] = 't';
			for(int ii=0; ii<m_nTempUnit[nCh]; ii++) {
				pDeli->cBuf[5+ii] = (char) m_nTemp[nCh][ii];
			}
			pDeli->nSize = PARAM_COM_BUFSIZE + m_nTempUnit[nCh];
			break;
		case SEND_GET_BLOCK:
			pDeli->cBuf[1] = 'b';
			pDeli->cBuf[5] = m_nBlock[nCh];
			break;
	}
		pDeli->cBuf[2] = 0x01;
		pDeli->cBuf[3] = 0x01 + nCh;		// チャンネル
		pDeli->cBuf[4] = 0x00;

	// 追加
	if( ! mque_SendItem.AddToTail(pDeli, 20, 20) ) {		// 絶対登録
		// 絶対登録も無理・・・
		LOG(em_ERR), "[%s] 送信依頼キューフル [CH=%d] [%s]", my_sThreadName, nCh+1, GetSendCommandName(em));
		syslog(SYSNO_QUEFULL_ERR, "[%s:CH=%d:%s]", my_sThreadName, nCh+1, GetSendCommandName(em));
	
		// 領域解放
		delete pDeli;
		return false;
	}
	return true;
}


//------------------------------------------
// 送信開始
// int* retStat 送信依頼モード(戻り値)
// 戻り値 false:切断してね要求
//------------------------------------------
bool DummyLedManager::SendStart(int* retStat)
{
	// 状態確認
	if( MODE_NONE == m_emMainMode) {
		//LOG(em_ERR), "[%s] ソケット切断中に送信依頼有り。依頼破棄[CH=%d][%d]", my_sThreadName, m_InfSend.nCh, m_InfSend.emLedCommand);
		return false;
	}
	
	
	// データコピー
	memcpy(&m_cSockBuf[PORT_SEND][BUF_SEND], m_InfSend.cBuf, m_InfSend.nSize);
	*retStat = m_InfSend.emLedCommand;	


	//// 同期送信
	LOG(em_INF), "[%s] コマンド送信 [CH=%d][%s] [%.2s]", my_sThreadName, m_InfSend.nCh, GetSendCommandName(m_InfSend.emLedCommand), m_InfSend.cBuf );
//◆	LOG(em_INF), "[%s] コマンド送信 [CH=%d][%s] [%.2s]", my_sThreadName, m_InfSend.nCh, GetSendCommandName(m_InfSend.emLedCommand), m_InfSend.cBuf );
	//syslog(213,"[%s:%s]", my_sThreadName, GetSendCommandName(m_InfSend.emLedCommand) );
	LOG(em_MSG), "   送信[%d][%s][%02X,%02X,%02X,%02X,%02X,%02X]", m_InfSend.nSize, m_InfSend.cBuf, m_InfSend.cBuf[0], m_InfSend.cBuf[1], m_InfSend.cBuf[2], m_InfSend.cBuf[3], m_InfSend.cBuf[4], m_InfSend.cBuf[5]);
 

	int iRetc = mcls_pSock[PORT_SEND]->LockSend( m_cSockBuf[PORT_SEND][BUF_SEND], m_InfSend.nSize, PARAM_RES_TIMEOUT );
	if(0 != iRetc) {
		LOG(em_ERR), "[%s][CH=%d][%s] TCP送信エラー(err_code = %d)", my_sThreadName, m_InfSend.nCh, GetSendCommandName(m_InfSend.emLedCommand), iRetc);
		syslog(SYSNO_SOCK_SEND_ERR, "[%s CH=%d err_code = %d]", my_sThreadName, m_InfSend.nCh, iRetc);
		return false;
	}

	return true;
}

//------------------------------------------
// 受信完了
// int id ポートID
// BaseTcpIp::DELIVERY_ANS* que 操作結果
// 戻り値 0:引き続き受信  -1:異常(切断してね) 1:コマンドに対するアンサーを受信  9:送信許可を受信
//------------------------------------------
int DummyLedManager::RecvEnd(int id, BaseTcpIp::DELIVERY_ANS* que)
{
	// コマンド特定
	EM_LED_RECV_COMMAND  emRecv = GetRecvCommand(m_cSockBuf[PORT_SEND][BUF_RECV][1]);


	// ///////////////////////////////////////////////////////////////////
	// 前処理
	//

	// 受信コマンドの長さに関係する為、先に特定
	m_InfRecv.nCh	= m_cSockBuf[PORT_SEND][BUF_RECV][3];	// チャンネル
	int nCh = m_InfRecv.nCh-1;

	// データ部がある受信コマンドを受信
	// 受信コマンドの種別で、データ部の長さが初めて分かるので、全体長さを再定義
	if( RECV_SEND_VALUE == emRecv) {
		if (PARAM_COM_BUFSIZE == que->Data->nSize) {
			// 残りを受信
			m_InfRecv.nSize = PARAM_COM_BUFSIZE + 1 + m_nBlock[nCh];		// 受信コマンド + LED温度格納点数
			if( ! RecvStart( id, 1 + m_nBlock[nCh], m_cSockBuf[id][BUF_RECV], PARAM_COM_BUFSIZE)) return -1;
			return 0;
		}
	//} else 	if( RECV_READ_EEPROM == emRecv || RECV_READ_RAM == emRecv) {
	//	if (PARAM_COM_BUFSIZE == que->Data->nSize) {
	//		// 残りを受信
	//		m_InfRecv.nSize = PARAM_COM_BUFSIZE + 1 + m_nBlock[nCh]; // 受信コマンド + 全体調光データ ＋ 補正データブロック数
	//		if( ! RecvStart( id, m_InfRecv.nSize, m_cSockBuf[id][BUF_RECV], PARAM_COM_BUFSIZE)) return -1;
	//		return 0;
	//	}

	} else {
		m_InfRecv.nSize = que->Data->nSize;
	}

	memcpy(&m_InfRecv.cBuf, &m_cSockBuf[PORT_SEND][BUF_RECV], m_InfRecv.nSize);


	// 回線接続認識 (何か受信したらOK)
	if( ! m_bIsConnect) {
		LOG(em_MSG), "[%s] 照明装置 準備完了", my_sThreadName);
		syslog(SYSNO_SOCK_CONNECT, "[%s]", my_sThreadName);

		m_bIsConnect = true;
	}

	// ///////////////////////////////////////////////////////////////////
	// １コマンド受信完了時
	//
	switch(emRecv) {
			// 断線有無 (ステータス要求)
			case RECV_CHECK_BREAK:
				AddQue_SendCommand(CheckBreak(nCh),nCh);
				break;
			// 点灯消灯状態 (ステータス要求)
			case RECV_CHECK_ONOFF:
				AddQue_SendCommand(CheckOnOff(nCh),nCh);
				break;
			// LED加熱状態 (ステータス要求)
			case RECV_CHECK_HEATING:
				AddQue_SendCommand(CheckHeating(nCh),nCh);
				break;
			// LED温度状態 (ステータス要求)
			case RECV_CHECK_TEMP:
				AddQue_SendCommand(SEND_LED_TEMP,nCh);
				break;
			// 点灯指示
			case RECV_EXEC_ON:
				if (1 == m_nLedOnOff[nCh]) {		// 消灯制御ON時
					m_nStateOnOff[nCh]=1;
					AddQue_SendCommand(SEND_STATE_OFF,nCh);
				} else {
					m_nStateOnOff[nCh]=0;
					AddQue_SendCommand(SEND_STATE_ON,nCh);
				}
				break;
			// 消灯指示
			case RECV_EXEC_OFF:
				m_nStateOnOff[nCh]=1;
				AddQue_SendCommand(SEND_STATE_OFF,nCh);
				break;
			// 調光データ送信 (RAMにセット)
			case RECV_SEND_VALUE:
				GetMasterData(nCh);
				break;
			// RAMの調光データをEEPROM(ファイル)に書込
			case RECV_WRITE_EEPROM:
				break;
			// EEPROM(ファイル)の調光データを読込
			case RECV_READ_EEPROM:
				AddQue_SendCommand(SEND_READ_EEPROM,nCh);
				break;
			// RAMの調光データを読込
			case RECV_READ_RAM:
				break;
			// ブロック数取得
			case RECV_GET_BLOCK:
				AddQue_SendCommand(SEND_GET_BLOCK,nCh);
				break;
	}
	//if( RECV_READ_EEPROM != emRecv)
		AddQue_SendCommand(SEND_NEXT_SEND_OK,nCh);
	// また最初から受信開始
	if( ! RecvStart( id, PARAM_COM_BUFSIZE, m_cSockBuf[id][BUF_RECV] )) {
		return -1;
	}

	// 後始末
	m_emMainMode = MODE_WAIT;
	return 0;
}


// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// TPC/IP 通信制御

//------------------------------------------
// 初期化処理 (一度のみ起動)
//------------------------------------------
void DummyLedManager::SockInit()
{
	BaseTcpIp::DELIVERY_COMMON	deli;

	//// 送信用スレッド 生成
	// 定義
	deli.nMyID = PORT_SEND;
	deli.bSorC = true;
	deli.bRorS = false;
	sprintf(deli.cMyName,  "%s", my_sThreadName);
	sprintf(deli.cSession, "%s", m_cMySession);
	deli.nBufferSize = PARAM_SOCK_BUFSIZE;
	deli.nRetryTimer = PARAM_SOCK_TIMEOUT;

	// 生成
	mcls_pSock[PORT_SEND] = new BaseTcpIp(&deli);
	mcls_pSock[PORT_SEND]->SetLogMgr(mcls_pLog);
	mcls_pSock[PORT_SEND]->SetMainPostTid(GetTid());

	// ワーカースレッド開始
	mcls_pSock[PORT_SEND]->Start();
}

//------------------------------------------
// 終了処理 (一度のみ起動)
//------------------------------------------
void DummyLedManager::SockExit()
{
	// ワーカースレッド停止
	mcls_pSock[PORT_SEND]->Stop();
	// ワーカースレッド解放
	SAFE_DELETE( mcls_pSock[PORT_SEND] );
}


//------------------------------------------
// 強制切断
// int nClose 区分 (-1:切断するけどまだ異常にしない  0:切断&異常  1:初期化のみ  )
//------------------------------------------
void DummyLedManager::UserClose(int nClose)
{
	// どのポートからでも クローズ通知が来たら、全ポート強制終了させる
	mcls_pSock[PORT_SEND]->UserClose();
	LOG(em_ERR), "[%s] 回線異常 ===================================>>>>", my_sThreadName);

	for(int ii=0; ii<PARAM_SOCK_MAX; ii++) {
		m_eStateMain[ii]  = SOCK_NONE;
		//m_nNowEventNo[ii] = 0;
	}
	// ステータス関連初期化
	m_bIsConnect = false;

	// 切断したので総合モードを待ち状態にする
	m_emMainMode = MODE_NONE;

	// 送信依頼のキューがあれば削除しておく
	DeleteAllQue();
}

//------------------------------------------
// 非同期 受信処理開始
// int id ポートID
// int size サイズ
// BYTE* pBuf バッファポインタ
// int point 受信データ開始位置
// 戻り値 false:回線切断処理を実施
//------------------------------------------
bool DummyLedManager::RecvStart(int id, int size, BYTE* pBuf, int point)
{
	//// 非同期 受信
	// 受信情報セット
	BaseTcpIp::DELIVERY_DATA* pDeleData = new BaseTcpIp::DELIVERY_DATA;
	pDeleData->bAnsQueing = true;	// 固定
	pDeleData->bRorS = true;		// 受信
	pDeleData->nStartPoint = point;
	pDeleData->nSize = size;
	pDeleData->Data = pBuf;
	pDeleData->nUserWk[0] = size+point;			// 合計受信バイト数としてセットしておく
	
	// 受信要求
	if( mcls_pSock[id]->SetRunningData(pDeleData)) {
		return true;	
	} else {
		LOG(em_ERR), "[%s] 非同期 受信要求失敗", my_sThreadName );
		SAFE_DELETE(pDeleData);
		return false;
	}
}


// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 共通処理

//------------------------------------------
// 送信コマンド名称
// EM_LED_SEND_COMMAND em コマンド
// 戻り値 名称
//------------------------------------------
CString DummyLedManager::GetRecvCommandName(EM_LED_RECV_COMMAND em)
{
	CString wk;
	switch( em ) {
		case RECV_CHECK_BREAK:
			wk = "断線有無";
			break;
		case RECV_CHECK_HEATING:
			wk = "LED加熱ステータス要求";
			break;
		case RECV_CHECK_ONOFF:
			wk = "点灯消灯状態";
			break;
		case RECV_CHECK_TEMP:
			wk = "LED温度状態";
			break;
		case RECV_EXEC_ON:
			wk = "点灯指示";
			break;
		case RECV_EXEC_OFF:
			wk = "消灯指示";
			break;
		case RECV_SEND_MASTER:
			wk = "全体調光送信";
			break;
		case RECV_SEND_VALUE:
			wk = "調光データ送信";
			break;
		case RECV_WRITE_EEPROM:
			wk = "調光EEPROM書込";
			break;
		case RECV_READ_EEPROM:
			wk = "調光EEPROM読込";
			break;
		case RECV_READ_RAM:
			wk = "調光RAM読込";
			break;
		case RECV_GET_BLOCK:
			wk = "ブロック数取得";
			break;
		case RECV_NOTHING:
			wk = "";
			break;
		default :
			wk = "モード異常";
			break;
	}
	return wk;
}
CString DummyLedManager::GetSendCommandName(EM_LED_SEND_COMMAND em)
{
	CString wk;
	switch( em ) {
		case SEND_NEXT_SEND_OK:
			wk = "送信許可";
			break;
		case SEND_STATE_ON:
			wk = "LED点灯";
			break;
		case SEND_STATE_OFF:
			wk = "LED消灯";
			break;
		case SEND_STATE_BREAK:
			wk = "断線";
			break;
		case SEND_STATE_HOT:
			wk = "温度上昇";
			break;
		case SEND_STATE_SHATDOWN:
			wk = "加熱ｼｬｯﾄﾀﾞｳﾝ";
			break;
		case SEND_STATE_OK:
			wk = "正常";
			break;
		case SEND_READ_EEPROM:
			wk = "調光EEPROM読込";
			break;
		case SEND_READ_RAM:
			wk = "調光RAM読込";
			break;
		case SEND_LED_TEMP:
			wk = "LED温度読込";
			break;
		case SEND_GET_BLOCK:
			wk = "ブロック数読込";
			break;
		case SEND_NOTHING:
			wk = "無視コマンド";
			break;
		default :
			wk = "モード異常";
			break;
	}
	return wk;
}

//------------------------------------------
// 受信コードの特定
// char code 先頭1文字
// 戻り値 受信コマンド
//------------------------------------------
DummyLedManager::EM_LED_SEND_COMMAND DummyLedManager::GetSendCommand(char code)
{
	switch( code ) {
		case 'C': return SEND_NEXT_SEND_OK;
		case 'x': return SEND_STATE_ON;
		case 'y': return SEND_STATE_OFF;
		case 'A': return SEND_STATE_BREAK;
		case 'B': return SEND_STATE_HOT;
		case 'D': return SEND_STATE_SHATDOWN;
		case 'N': return SEND_STATE_OK;
		case 'E': return SEND_READ_EEPROM;
		case 'P': return SEND_READ_RAM;
		case 't': return SEND_LED_TEMP;
		case 'b': return SEND_GET_BLOCK;
		case 'T': return SEND_TIME_OUT;
		case 0x80: return SEND_NOTHING;
		default : return SEND_NON;
	}
}

DummyLedManager::EM_LED_RECV_COMMAND DummyLedManager::GetRecvCommand(char code)
{
	switch( code ) {
		case 'a': return RECV_CHECK_BREAK;
		case 'b': return RECV_CHECK_HEATING;
		case 'l': return RECV_CHECK_ONOFF;
		case 't': return RECV_CHECK_TEMP;
		case 'X': return RECV_EXEC_ON;
		case 'Y': return RECV_EXEC_OFF;
		case 'T': return RECV_SEND_MASTER;
		case 'S': return RECV_SEND_VALUE;
		case 'W': return RECV_WRITE_EEPROM;
		case 'M': return RECV_READ_EEPROM;
		case 'R': return RECV_READ_RAM;
		case 'B': return RECV_GET_BLOCK;
		case 0x00: return RECV_NOTHING;
		default : return RECV_NON;
	}
}

//------------------------------------------
// メインモード名称
// EM_LED_MAINMODE em モード
// 戻り値 名称
//------------------------------------------
CString DummyLedManager::GetMainModeName(EM_LED_MAINMODE em)
{
	CString wk;
	switch( em ) {
		case MODE_NONE:
			wk = "切断中";
			break;
		case MODE_WAIT:
			wk = "待機";
			break;
		case MODE_RECV_EXEC:
			wk = "受信コマンド待ち";
			break;
		default :
			wk = "モード異常";
			break;
	}
	return wk;
}


// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 状態確認
//------------------------------------------
// 断線状態チェック
// nCh	チャンネル
// 戻り値 名称
//------------------------------------------
DummyLedManager::EM_LED_SEND_COMMAND DummyLedManager::CheckBreak(int nCh)
{
	if(0 == m_nStateBreak[nCh])
		return SEND_STATE_OK;
	else
		return SEND_STATE_BREAK;
}

//------------------------------------------
// 点灯消灯状態チェック
// nCh	チャンネル
// 戻り値 名称
//------------------------------------------
DummyLedManager::EM_LED_SEND_COMMAND DummyLedManager::CheckOnOff(int nCh)
{
	if(0 == m_nStateOnOff[nCh])
		return SEND_STATE_ON;
	else
		return SEND_STATE_OFF;
}

//------------------------------------------
// 温度状態チェック（ユニット個別）
// nCh	チャンネル
// nUnit ユニット
// 戻り値 名称
//------------------------------------------
DummyLedManager::EM_LED_SEND_COMMAND DummyLedManager::CheckHeating(int nCh, int nUnit)
{
	if(0 == m_nTempSts[nCh][nUnit - 1])
		return SEND_STATE_OK;
	else if(0 == m_nTempSts[nCh][nUnit - 1])
		return SEND_STATE_HOT;
	else
		return SEND_STATE_SHATDOWN;
}

//------------------------------------------
// 温度状態チェック（全ユニット）
// nCh	チャンネル
// 戻り値 名称
//------------------------------------------
DummyLedManager::EM_LED_SEND_COMMAND DummyLedManager::CheckHeating(int nCh)
{
	EM_LED_SEND_COMMAND bRet = SEND_STATE_OK;
	for(int ii = 0; ii<m_nBlock[nCh]; ii++){
		if(1 == m_nTempSts[nCh][ii])
			bRet = SEND_STATE_HOT;
		else if(2 == m_nTempSts[nCh][ii])
			bRet = SEND_STATE_SHATDOWN;
	}
	return bRet;
}

//------------------------------------------
// 調光データ取得
// nCh	チャンネル
// 戻り値 名称
//------------------------------------------
void DummyLedManager::GetMasterData(int nCh)
{	
	m_nNowLedBase[nCh] = m_InfRecv.cBuf[5];
	memcpy(m_nNowLedHosei[nCh], &m_InfRecv.cBuf[6], m_nBlock[nCh]);
}

