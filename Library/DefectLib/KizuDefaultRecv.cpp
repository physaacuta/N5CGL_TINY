#include "StdAfx.h"
#include "KizuDefaultRecv.h"

//////////////////////////////////////////////////////////////////////
// 構築/消滅
//////////////////////////////////////////////////////////////////////
//------------------------------------------
// コンストラクタ
// const char *cSession iniファイルセッション
// ThreadQueue<SOCK_BASE_HEAD>* que_pRecv 受信データ構造体受け渡しキュー
//------------------------------------------
KizuDefaultRecv::KizuDefaultRecv(const char *cSession, ThreadQueue<SOCK_BASE_HEAD>* que_pRecv) :
m_cSession(cSession),
mque_pRecv(que_pRecv),
my_emSockState(STATE_NONE),
my_nRecvingDataSize(0),
my_pRecvData(NULL),
mcls_pSock(NULL),
mcls_pLog(NULL)
{
	//// シグナル生成
	my_evReTryConnect	= CreateWaitableTimer(NULL, FALSE, NULL);
	my_evHelcy			= CreateWaitableTimer(NULL, FALSE, NULL);
	my_evDisConnect		= CreateEvent(NULL, FALSE, FALSE, NULL);
	my_evRecvEnd		= CreateEvent(NULL, FALSE, FALSE, NULL);
}

//------------------------------------------
// デストラクタ
//------------------------------------------
KizuDefaultRecv::~KizuDefaultRecv(void)
{
	//// 開放
	SAFE_DELETE_ARRAY(my_pRecvData);
	SAFE_DELETE(mcls_pSock);				// ソケットクラス 開放
	// シグナル開放
	CloseHandle(my_evReTryConnect);
	CloseHandle(my_evHelcy);
	CloseHandle(my_evDisConnect);
	CloseHandle(my_evRecvEnd);
}

//------------------------------------------
// 指定のステータス名称
// SOCK_STATE st ステータス
//------------------------------------------
CString KizuDefaultRecv::GetSockStateName(SOCK_STATE st) const
{
	CString wk;
	if		(st == STATE_NONE)		{ wk = "初期状態 (切断中)"; }
	else if (st == STATE_IDLE)		{ wk = "準備完了 (接続中)"; }
	else if (st == STATE_HEAD)		{ wk = "ヘッダ部 受信中";}
	else if (st == STATE_DATA)		{ wk = "データ部 受信中";}
	else							{ wk = "ステータス異常";}
	return wk;
}

//------------------------------------------
// スレッド
//------------------------------------------
UINT WINAPI KizuDefaultRecv::MainThread(LPVOID param)
{
	////// メインインスタンス取り出し
	KizuDefaultRecv *p = (KizuDefaultRecv *)param;
	p->SetTid();								// 自スレッドIDセット

	// WaitForMultipleObjectsイベント
	HANDLE	hArray[] = {p->my_evStop,
						p->my_evDisConnect, 
						p->my_evReTryConnect,
						p->my_evHelcy,
						p->my_evRecvEnd  
						};
	enum {	EV_STOP = WAIT_OBJECT_0,		// スレッド終了
			EV_DISCONNECT,					// 切断イベント
			EV_RETRY,						// 再接続要求タイマイベント
			EV_HELCY_TIMEOUT,				// ヘルシー伝文タイムアウト
			EV_RECVEND,						// 受信完了イベント
			EV_END	};

	//// 初期接続
	p->Connect(true);

	// メッセージキュー読み込み
	pLOG(em_MSG), "[KizuDefaultRecv] <%s> スレッド開始 [0x%X]", p->m_cSession, p->GetTid());
	while (1) {
		DWORD ret = WaitForMultipleObjects(EV_END, hArray, FALSE, INFINITE);

		switch (ret) {

//-----------------------------------------------------------------------------------------------
		case EV_STOP:												// スレッド終了
			pLOG(em_MSG), "[KizuDefaultRecv] <%s> スレッド終了 [0x%X]", p->m_cSession, p->GetTid());
			return 0;

//-----------------------------------------------------------------------------------------------
		case EV_DISCONNECT:											// 切断イベント
			p->Close();													// 切断
			break;

//-----------------------------------------------------------------------------------------------
		case EV_RETRY:												// 再接続要求タイマイベント
			p->Connect(false);											// 初期接続
			break;

//-----------------------------------------------------------------------------------------------
		case EV_HELCY_TIMEOUT:										// ヘルシー伝文タイムアウト
			// タイムアウトによる切断
			pLOG(em_ERR), "[KizuDefaultRecv] <%s> ソケット ヘルシー伝文タイムアウト", p->m_cSession);
			syslog(SYSNO_SOCK_HELCY_TIMEOUT, "[%s]", p->m_cSession);

			p->Close();	
			break;

//-----------------------------------------------------------------------------------------------
		case EV_RECVEND:											// 受信完了イベント
			switch( p->my_emSockState) {
				case STATE_HEAD :										// ヘッダ部受信完了

					//// トランザクションコードによる処理わけ
					if( 0 == memcmp( p->my_RecvHead.code, T_CODE_HELCY, SIZE_SOCK_TC ) ) {	
						//pLOG(em_INF), "[KizuDefaultRecv] ヘルシー伝文受信");
						// ヘルシー監視リセット
						p->Recv_Helcy();

						// 次受信開始
						if( ! p->Recv_Head() ) p->Close();

					} else {
						// データ部受信開始
						if( ! p->Recv_Data() ) p->Close();
					}
					break;

				case STATE_DATA :										// データ部受信完了		

					//pLOG(em_MSG), "%d[byte]/%d[byte]受信完了", p->my_nRecvingDataSize, p->my_RecvHead.len);

					//// 論理回線単位でホントにデータを全部受信できた？ (分割送信対応)
					if(p->my_RecvHead.len > p->my_nRecvingDataSize+sizeof(p->my_RecvHead)) {
						// まだデータ部の受信途中
						if( !p->Recv_Data() ) p->Close();
						break;
					} 

					
					// データコピー
					BYTE *pRecvBuf;											// キュー登録用バッファ
					pRecvBuf = new BYTE[p->my_RecvHead.len];					// キューの取り出し先で開放される
					memcpy( pRecvBuf, &p->my_RecvHead, sizeof(p->my_RecvHead));
					memcpy( &pRecvBuf[sizeof(p->my_RecvHead)], p->my_pRecvData, p->my_RecvHead.len - sizeof(p->my_RecvHead) );
					
					// キューに登録
					if( p->mque_pRecv->GetFreeCount() > 0 ) {
						p->mque_pRecv->AddToTail( (SOCK_BASE_HEAD*)pRecvBuf );			

					} else {												// 空き無し
						pLOG(em_WAR), "[KizuDefaultRecv] <%s> キューフル 受信サイズ[%dbyte]", p->m_cSession, p->my_RecvHead.len);
					
						// 今作ったインスタンス破棄
						SAFE_DELETE_ARRAY(pRecvBuf); 
					}
					
					// 受信バッファ開放
					SAFE_DELETE_ARRAY(p->my_pRecvData);
	
					// 次受信開始
					if( ! p->Recv_Head() ) p->Close();
					break;

				default :
					pLOG(em_MSG), "[KizuDefaultRecv] <%s> 受信完了イベント時ソケットモード異常 [%d]", p->m_cSession, p->my_emSockState);
					break;
			}
			break;

//-----------------------------------------------------------------------------------------------
		default :
			// ありえない！！
			break;
		}
	}
	return 0;
}

// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// シーケンス制御

//------------------------------------------
// ヘッダ部受信開始
//------------------------------------------
bool KizuDefaultRecv::Recv_Head()
{
	if(NULL == mcls_pSock) return false;

	memset(&my_RecvHead, 0x00, sizeof(my_RecvHead));
	my_nRecvingDataSize = 0;

	// 受信完了イベント通知タイプの受信開始 (クライアントは0固定)
	int retc = mcls_pSock->Recv( 0, &my_RecvHead, sizeof(my_RecvHead), my_evRecvEnd);
	if( 0 == retc ) {							// 正常完了
		my_emSockState = STATE_HEAD;						// ソケット状態 ヘッダ部受信中
		return true;
	} else {
		LOG(em_ERR), "[KizuDefaultRecv] <%s> ソケット Recv_Head失敗 err_code=%d", m_cSession, retc);
		syslog(SYSNO_SOCK_RECV_ERR, "[%s,ヘッダ部 err_code=%d]", m_cSession, retc);
		return false;
	}
}

//------------------------------------------
// データ部受信開始
//------------------------------------------
bool KizuDefaultRecv::Recv_Data()
{
	if(NULL == mcls_pSock) return false;
/*
	// データ部受信バッファ生成
	int size = my_RecvHead.len - sizeof(my_RecvHead);
	my_pRecvData = new BYTE[size];

	// 受信完了イベント通知タイプの受信開始 (クライアントは0固定)
	int retc = mcls_pSock->Recv( 0, my_pRecvData, size, my_evRecvEnd);
*/

	// データ部受信バッファ生成
	DWORD size = my_RecvHead.len - sizeof(my_RecvHead) - my_nRecvingDataSize;
	if( 0 == my_nRecvingDataSize ) my_pRecvData = new BYTE[size];
	if(size > SIZE_SOCK_DATA_MAX) size = SIZE_SOCK_DATA_MAX;	// 1回の受信サイズオーバー

	// 受信完了イベント通知タイプの受信開始 (クライアントは0固定)
	int retc = mcls_pSock->Recv( 0, &my_pRecvData[my_nRecvingDataSize], size, my_evRecvEnd);

	if( 0 == retc ) {							// 正常完了
		my_emSockState = STATE_DATA;						// ソケット状態 データ部受信中
		my_nRecvingDataSize += size;
		return true;
	} else {
		LOG(em_ERR), "[KizuDefaultRecv] <%s> ソケット Recv_Data失敗 err_code=%d", m_cSession, retc);
		syslog(SYSNO_SOCK_RECV_ERR, "[%s,データ部 err_code=%d]", m_cSession, retc);
		return false;
	}
}

//------------------------------------------
// ヘルシー伝文受信完了 (ヘッダー部のみ)
//------------------------------------------
void KizuDefaultRecv::Recv_Helcy()
{
	// 一度キャンセル
	CancelWaitableTimer(my_evHelcy);

	// ヘルシー伝文タイムー監視開始
	__int64 ts = (__int64)PARAM_TIMEOUT_HELCY_RECV * -10000;	// 1μ秒→100ナノ秒 (マイナス指定で、指定時間経過でシグナル) (第3引数は2回目以降の起動間隔)
	SetWaitableTimer(my_evHelcy, (LARGE_INTEGER *)&ts, 0, NULL, NULL, FALSE);
}

// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// TPC/IP 通信制御

//------------------------------------------
// 接続
// bool bLogDsp ログ出力有無 (false:失敗ログ出さない)
//------------------------------------------
void KizuDefaultRecv::Connect(bool bLogDsp)
{
	int retc;

	// ソケットインスタンス生成
	try {
		mcls_pSock = new SockBase( m_cSession, TASKINI_NAME, GetTid(), 0, 1, PARAM_SOCK_RECV_BUFSIZE, 0);
	} catch(int code) {
		mcls_pSock = NULL;
		if(bLogDsp) {
			LOG(em_ERR), "[KizuDefaultRecv] <%s> ソケット New失敗 err_code=%d", m_cSession, code);
			syslog(SYSNO_SOCK_NEW_ERR, "[%s: err_code=%d]", m_cSession, code);
		}
	}

	// コネクト
	if( NULL != mcls_pSock ) {
		retc = mcls_pSock->Connect();
		if( 0 != retc ) {
			delete mcls_pSock;
			mcls_pSock = NULL;
			if(bLogDsp) {
				LOG(em_ERR), "[KizuDefaultRecv] <%s> ソケット Connect失敗 err_code=%d", m_cSession, retc);
				syslog(SYSNO_SOCK_CONNECT_ERR, "[%s: err_code=%d]", m_cSession, retc);
			}
		}
	}

	// 後処理
	if( NULL != mcls_pSock ) {								// 成功
		mcls_pSock->SetEvDisConnect(my_evDisConnect);			// 切断認識をイベントにする
		my_emSockState = STATE_IDLE;							// ソケット状態 待機
		
		// ヘッダー部受信開始
		if( Recv_Head() ) {

			// ヘルシー伝文監視開始
			Recv_Helcy();
		
			// ソケット接続完了
			LOG(em_MSG), "[KizuDefaultRecv] <%s> ソケット Connect完了", m_cSession);
			syslog(SYSNO_SOCK_CONNECT, "[%s]", m_cSession);

		} else {
			Close();												// 切断 & リトライ設定
		}

	} else {												// ソケット生成失敗
		Close();												// 切断 & リトライ設定
	}
}

//------------------------------------------
// 切断 & リトライ設定
//------------------------------------------
void KizuDefaultRecv::Close()
{
	// ソケット切断
	if(NULL != mcls_pSock) {
		SAFE_DELETE(mcls_pSock); 
		LOG(em_ERR), "[KizuDefaultRecv] <%s> ソケット 切断", m_cSession);
		syslog(SYSNO_SOCK_CLOSE, "[%s]", m_cSession);
	}

	// 初期化
	my_emSockState = STATE_NONE;				// ソケット状態 初期化
	SAFE_DELETE_ARRAY( my_pRecvData );		 	// 受信バッファ初期化

	// ヘルシー伝文タイムー監視終了
	CancelWaitableTimer(my_evHelcy);


	// リトライ
//	LARGE_INTEGER ts;
//	ts.QuadPart = (__int64)PARAM_SOCK_RETRY_TIME * -10000;	// 1[ms]→100[ns] (マイナス指定で、指定時間経過でシグナル) (第3引数は2回目以降の起動間隔)
//	SetWaitableTimer(my_evReTryConnect, &ts, 0, NULL, NULL, FALSE);
	__int64 ts = PARAM_SOCK_RETRY_TIME * -10000;	// 1μ秒→100ナノ秒 (マイナス指定で、指定時間経過でシグナル) (第3引数は2回目以降の起動間隔)
	SetWaitableTimer(my_evReTryConnect, (LARGE_INTEGER *)&ts, 0, NULL, NULL, FALSE);
}
