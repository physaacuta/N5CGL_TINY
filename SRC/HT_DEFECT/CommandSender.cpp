#include "StdAfx.h"
#include "CommandSender.h"


// メモリーリーク検出 ----->>>
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif
// <<<------------------------

//------------------------------------------
// コンストラクタ
// LPCSTR cPipeName 接続するPIPEパス
//------------------------------------------
CommandSender::CommandSender(LPCSTR cPipeName) :
m_cPipeName(cPipeName),
m_bConnect(false),
m_nSendSize(0),
mcls_pPipe(NULL),
m_bRunExit(false),
m_bNgCam(false),
ThreadManager("CmdSender")
{
	// 初期化
	memset( m_cRecvWk,  0x00, sizeof(m_cRecvWk) );
	memset( m_cRecvBuf, 0x00, sizeof(m_cRecvBuf) );
	memset( m_cSendBuf, 0x00, sizeof(m_cSendBuf) );

	// シグナル生成
	m_evOpenPipe  = CreateEvent(NULL, FALSE, FALSE, NULL);				// PIPEの接続用イベント
	m_evClosePipe = CreateEvent(NULL, FALSE, FALSE, NULL);				// PIPEの切断用イベント
	m_evSendStart = CreateEvent(NULL, FALSE, FALSE, NULL);				// 受信開始イベント認識用
	m_evSendEnd	  = CreateEvent(NULL, FALSE, FALSE, NULL);				// 送信完了インベト認識用
}

//------------------------------------------
// デストラクタ
//------------------------------------------
CommandSender::~CommandSender(void)
{
	CloseHandle(m_evOpenPipe);
	CloseHandle(m_evClosePipe);
	CloseHandle(m_evSendStart);
	CloseHandle(m_evSendEnd);
}

//------------------------------------------
// スレッド開始前処理 (スレッド実行直後(シグナルセット前)にコールバック)
//------------------------------------------
int CommandSender::ThreadFirst()
{
	//// 事前準備
	memset(&m_ovlRecv, 0x00, sizeof(OVERLAPPED));
	m_ovlRecv.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);		// 手動リセット

	//// イベント設定
	HANDLE hArray[] = { 
						m_evOpenPipe,
						m_evClosePipe,
						m_evSendStart,
						m_ovlRecv.hEvent
						};
	SetEventNum(sizeof(hArray)/sizeof(HANDLE), hArray);
	return 0;
}

//------------------------------------------
// スレッド終了前処理 (終了シグナル後にコールバック)
//------------------------------------------
int CommandSender::ThreadLast()
{
	// パイプクラスのインスタンスがある場合は、開放
	if(NULL != mcls_pPipe) {
		mcls_pPipe->Cancel();										// 未処理のI/Oを取り消す
		SAFE_DELETE(mcls_pPipe);									// インスタンス開放
	}
	m_bConnect = false;

	// ハンドルの解放
	CloseHandle(m_ovlRecv.hEvent);
	return 0;
}

//------------------------------------------
// スレッドイベント発生
// int nEventNo イベントNo (0オリジン)
//------------------------------------------
void CommandSender::ThreadEvent(int nEventNo)
{
	////// シグナルの条件
	enum {	
			EV_OPEN = 0,					// オープン要求
			EV_CLOSE,						// クローズ要求
			EV_SEND_START,					// 送信開始要求
			EV_RECV_END,					// 受信完了通知
			EV_END
	};

	////// シグナル条件分け
	switch (nEventNo) {

//-----------------------------------------------------------------------------------------------
	case EV_OPEN:												// オープン要求
		if(NULL != mcls_pPipe) break;
		
		// パイプ インスタンス生成
		int nCnt;
		for(nCnt=0; nCnt<PIPE_OPEN_RETRY; nCnt++) {
			try {
				mcls_pPipe = new PipeManager(m_cPipeName);
				break;
			} catch(int code) {
				LOG(em_WAR), "[%s] PIPE インスタンス生成失敗 [%d]", my_sThreadName, code);
				mcls_pPipe = NULL;
			}
			Sleep(PIPE_OPEN_INTERVAL);
		}
		if(PIPE_OPEN_RETRY == nCnt) {
			LOG(em_ERR), "[%s] PIPE インスタンス生成失敗", my_sThreadName);
			//m_emErrCode 		= CPIPE_OPEN_ERR;
			//m_nDetailErrCode	= NULL==mcls_pPipe ? 0 : mcls_pPipe->GetError();
			//SetEvent(m_evOnError);							// 外部へ通知
			CPIPE_ERROR * pCPipeErr = new CPIPE_ERROR;
			memset(pCPipeErr, 0x00, sizeof(CPIPE_ERROR));
			pCPipeErr->nErrCode		= CPIPE_OPEN_ERR;
			pCPipeErr->nDetailCode	= NULL==mcls_pPipe ? 0 : mcls_pPipe->GetError();
			if( ! gque_CPipeErr.AddToTailFreeCheck( pCPipeErr ) ) {
				LOG(em_ERR), "[%s] 受渡しキューフル (生成)", my_sThreadName);
				delete pCPipeErr;
			}
			break;
		}
		
		// 初期化
		ResetEvent(m_evSendStart);
		ResetEvent(m_evSendEnd);


		// パイプ読み込み
		if( !mcls_pPipe->Recv(m_cRecvWk, PIPE_RECVBUF_SIZE, &m_ovlRecv) ) {
			LOG(em_ERR), "[%s] PIPE 読み込み失敗", my_sThreadName);
			//m_emErrCode			= CPIPE_READ_ERR;
			//m_nDetailErrCode	= mcls_pPipe->GetError();
			//SetEvent(m_evOnError);
			CPIPE_ERROR * pCPipeErr = new CPIPE_ERROR;
			memset(pCPipeErr, 0x00, sizeof(CPIPE_ERROR));
			pCPipeErr->nErrCode		= CPIPE_READ_ERR;
			pCPipeErr->nDetailCode	= mcls_pPipe->GetError();
			if( ! gque_CPipeErr.AddToTailFreeCheck( pCPipeErr ) ) {
				LOG(em_ERR), "[%s] 受渡しキューフル (読込)", my_sThreadName);
				delete pCPipeErr;
			}
		}
		m_bConnect = true;
		LOG(em_MSG), "[%s] PIPE 接続完了", my_sThreadName);
		break;


//-----------------------------------------------------------------------------------------------
	case EV_CLOSE:												// クローズ要求
		// パイプクラスのインスタンスがある場合は、開放
		if(NULL != mcls_pPipe) {
			mcls_pPipe->Cancel();										// 未処理のI/Oを取り消す
			SAFE_DELETE(mcls_pPipe);									// インスタンス開放
			m_bConnect = false;
			LOG(em_MSG), "[%s] PIPE 切断完了", my_sThreadName);
		}
		break;

		
//-----------------------------------------------------------------------------------------------
	case EV_SEND_START:											// 送信開始要求
		if(NULL != mcls_pPipe) {
			// ここでは、特にエラー処理はしないよ。ここで失敗したら、アンサータイムアウトが絶対発生するから
//LOG(em_WAR), "[送信] size=%d, cmd=%d, %02X, %02X, %02X, %02X", m_nSendSize, ((int*)m_cSendBuf)[0], m_cSendBuf[4], m_cSendBuf[5], m_cSendBuf[6], m_cSendBuf[7]);
			mcls_pPipe->Send(m_cSendBuf, m_nSendSize);					// 同期送信とする。
		}
		break;


//-----------------------------------------------------------------------------------------------
	case EV_RECV_END:											// 受信完了通知
		// 非同期結果問い合わせ (受信待ち状態で、I/Oキャンセル時もコールされる)
		bool bRetc;
		if( mcls_pPipe != NULL )	bRetc = mcls_pPipe->GetResult(&m_ovlRecv);
		else 						bRetc = false;	
		ResetEvent(m_ovlRecv.hEvent);									// シグナルマニュアルリセット

		// データ取得後の処理
		if( bRetc ) {
			// 受信データの最初の4バイトを取得? リターンコードが入っている
			int retcode = *((int *)m_cRecvWk);
//LOG(em_WAR), "[受信] %d, %02X, %02X, %02X, %02X", retcode, m_cRecvBuf[0], m_cRecvBuf[1], m_cRecvBuf[2], m_cRecvBuf[3]);

			if ( 0 <= retcode ) {										// 負の値は、非同期エラー
				memcpy(m_cRecvBuf, m_cRecvWk, sizeof(m_cRecvBuf));			// 下の次受信で、上書きされるのを防止 
				SetEvent(m_evSendEnd);										// コマンド受信完了により、外部にイベント通知

			} else {
				//m_emErrCode			= CPIPE_ERR_COMMAND;
				//m_nDetailErrCode	= retcode;
				//SetEvent(m_evOnError);

				//-------------------------------------------
				// 外部へ通知 (非同期関連)
				CPIPE_ERROR * pCPipeErr = new CPIPE_ERROR;
				memset(pCPipeErr, 0x00, sizeof(CPIPE_ERROR));
				pCPipeErr->nErrCode		= CPIPE_ERR_COMMAND;
				pCPipeErr->nDetailCode	= retcode;

				//-------------------------------------------
				// 特殊処理部 ※外部に情報全部受け渡し
				//if(ERR_EVENT_TOP_DETECTED == retcode) {
				//	pCPipeErr->pData = new BYTE[PIPE_RECVBUF_SIZE];
				//	memcpy(pCPipeErr->pData, m_cRecvWk, sizeof(m_cRecvBuf));
				//} else if(ERR_EVENT_BOT_DETECTED == retcode) {
				//	pCPipeErr->pData = new BYTE[PIPE_RECVBUF_SIZE];
				//	memcpy(pCPipeErr->pData, m_cRecvWk, sizeof(m_cRecvBuf));
				//} 


				// 依頼
				if( ! gque_CPipeErr.AddToTailFreeCheck( pCPipeErr ) ) {
					LOG(em_ERR), "[%s] 受渡しキューフル (受信結果 重大異常)", my_sThreadName);
					delete pCPipeErr;
				}
			}

			// パイプ読み込み
			if( !mcls_pPipe->Recv(m_cRecvWk, PIPE_RECVBUF_SIZE, &m_ovlRecv) ) {
				//m_emErrCode			= CPIPE_READ_ERR;
				//m_nDetailErrCode	= mcls_pPipe->GetError();
				//SetEvent(m_evOnError);
				CPIPE_ERROR * pCPipeErr = new CPIPE_ERROR;
				memset(pCPipeErr, 0x00, sizeof(CPIPE_ERROR));
				pCPipeErr->nErrCode		= CPIPE_READ_ERR;
				pCPipeErr->nDetailCode	= mcls_pPipe->GetError();
				if( ! gque_CPipeErr.AddToTailFreeCheck( pCPipeErr ) ) {
					LOG(em_ERR), "[%s] 受渡しキューフル (読込)", my_sThreadName);
					delete pCPipeErr;
				}
			}


		} else {
			// 終了処理中は例外
			if( m_bRunExit ) {
				m_bRunExit = false;
				break; 
			}
			// コマンドパイプパイプ切断
			LOG(em_ERR), "[%s] PIPE が勝手に切断された", my_sThreadName);
			//m_emErrCode				= CPIPE_DISCONNECT_ERR;
			//m_nDetailErrCode		= mcls_pPipe->GetError();	// 0;
			//SetEvent(m_evOnError);
			CPIPE_ERROR * pCPipeErr = new CPIPE_ERROR;
			memset(pCPipeErr, 0x00, sizeof(CPIPE_ERROR));
			pCPipeErr->nErrCode		= CPIPE_DISCONNECT_ERR;
			pCPipeErr->nDetailCode	= mcls_pPipe->GetError();
			if( ! gque_CPipeErr.AddToTailFreeCheck( pCPipeErr ) ) {
				LOG(em_ERR), "[%s] 受渡しキューフル (勝手に切断)", my_sThreadName);
				delete pCPipeErr;
			}
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
// コマンド受付
// BYTE *pSendData 送信データセット
// DWORD nSendSize 送信サイズセット
// void *pRecvRetc  受信データの先頭*byteの値 (通常4byteのintの値がセットされる)
// int nRecvSize   受信データからの取り込みサイズ (通常4byte)
// DWORD nWaitTime 最大待ち時間
// 戻り値 復帰情報 (0:正常アンサー 1:異常アンサー -1:アンサー無し -9:パイプ無し)
//------------------------------------------
int CommandSender::SendCommand(BYTE *pSendData, DWORD nSendSize, void *pRecvRetc, int nRecvSize, DWORD nWaitTime)
{
	int nSendCode = *((int*)pSendData);
	if( m_bNgCam ) return 0;	// 異常カメラなら全部正常
	if( ! m_bConnect ) {
		LOG(em_ERR), "[%s] コマンドパイプ 切断中 [送信コマンド=%d]", my_sThreadName, nSendCode);
		return -9;
	}

	// 送信情報セット
	memcpy(m_cSendBuf, pSendData, nSendSize);
	m_nSendSize = nSendSize;

	// 送信要求
	ResetEvent(m_evSendEnd);	// 普通ありえないはずだけど、前のシグナルが残りっぱなしのとき
	SetEvent(m_evSendStart);
	
	// 送信完了して アンサーが帰って来るまで待ち
	DWORD ret = WaitForSingleObject(m_evSendEnd, nWaitTime);
	if (WAIT_TIMEOUT == ret) {
		LOG(em_ERR), "[%s] コマンドアンサー 応答無し [送信コマンド=%d]", my_sThreadName, nSendCode);

#ifdef ENG_TEST
		return 0;
#endif

		return -1;									// コマンド応答せず
	}

	// 受信データの先頭*バイトをコピー
	memcpy(pRecvRetc, m_cRecvBuf, nRecvSize);

	// 共通構造体に変換？
	NCL_ANSWER* pWk = (NCL_ANSWER*)pRecvRetc;

	if( NCL_SUCCESS == pWk->retcode ) {
//		LOG(em_MSG), "[%s] コマンドアンサー 応答有り [送信コマンド=%d]", my_sThreadName, *((int*)pSendData));
		return 0;
	} else {

		LOG(em_ERR), "[%s] コマンドアンサー異常 [送信コマンド=%d アンサー=%d]", my_sThreadName, nSendCode, pWk->retcode);
		return 1;
	}
}

//------------------------------------------
// 残りのデータ読込 (m_cRecvBufサイズ以上のデータにはそもそも未対応)
// int nOffset				前回までの取り込みサイズ
// void* pRecvRetc	受信データ先頭ポインタ
// int nRecvSize			残り読込サイズ
//------------------------------------------
void CommandSender::ReadPerData(int nOffset, void* pRecvRetc, int nRecvSize )
{
	// 受信データの先頭*バイトをコピー
	memcpy(&((BYTE*)pRecvRetc)[nOffset], &m_cRecvBuf[nOffset], nRecvSize);
}