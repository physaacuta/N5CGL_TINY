#include "StdAfx.h"
#include "DataRecver.h"

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
// LPCSTR cSmemName 共有メモリ名称
//------------------------------------------
DataRecver::DataRecver(LPCSTR cPipeName, LPCSTR cSmemName ) :
m_cPipeName(cPipeName),
m_cSmemName(cSmemName),
m_hMap(NULL),
mtbl_pMap(NULL),
m_bConnect(false),
m_bRunExit(false),
mcls_pPipe(NULL),
m_bFirstInfoExists(false),
ThreadManager("DataRecver")
{
	//// 初期化
	InitializeCriticalSection(&m_csLock);						// クリティカルセクションオブジェクトを初期化

	memset(&m_ovlSend, 0x00, sizeof(OVERLAPPED));
	memset(&m_ovlRecv, 0x00, sizeof(OVERLAPPED));

	// シグナル生成
	m_evOpenPipe  = CreateEvent(NULL, FALSE, FALSE, NULL);				// PIPEの接続用イベント
	m_evClosePipe = CreateEvent(NULL, FALSE, FALSE, NULL);				// PIPEの切断用イベント
	
	m_ovlSend.hEvent = CreateEvent(NULL, FALSE, TRUE, NULL);			// 非同期送信用 (初回は、シグナル状態にしておく)
	m_ovlRecv.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);			// 手動リセット
}

//------------------------------------------
// デストラクタ
//------------------------------------------
DataRecver::~DataRecver(void)
{
	//// 開放
	DeleteCriticalSection(&m_csLock);							// クリティカルセクションオブジェクトを開放

	CloseHandle(m_evOpenPipe);
	CloseHandle(m_evClosePipe);

	CloseHandle(m_ovlSend.hEvent);
	CloseHandle(m_ovlRecv.hEvent);
}

//------------------------------------------
// スレッド開始前処理 (スレッド実行直後(シグナルセット前)にコールバック)
//------------------------------------------
int DataRecver::ThreadFirst()
{
	//// イベント設定
	HANDLE hArray[] = {	
						m_evOpenPipe,
						m_evClosePipe,
						m_ovlRecv.hEvent
						};
	SetEventNum(sizeof(hArray)/sizeof(HANDLE), hArray);
	return 0;
}

//------------------------------------------
// スレッド終了前処理 (終了シグナル後にコールバック)
//------------------------------------------
int DataRecver::ThreadLast()
{
	// パイプクラスのインスタンスがある場合は、開放
	if(NULL != mcls_pPipe) {
		mcls_pPipe->Cancel();										// 未処理のI/Oを取り消す
		SAFE_DELETE(mcls_pPipe);									// インスタンス開放
		mem_close(&m_hMap, (LPVOID *)&mtbl_pMap);					// 共有メモリ
	}
	m_bConnect = false;
	return 0;
}

//------------------------------------------
// スレッドイベント発生
// int nEventNo イベントNo (0オリジン)
//------------------------------------------
void DataRecver::ThreadEvent(int nEventNo)
{
	////// シグナルの条件
	enum {	
			EV_OPEN = 0,					// オープン要求
			EV_CLOSE,						// クローズ要求
			EV_RECV_END,					// 受信完了通知
			EV_END
	};

	////// シグナル条件分け
	switch (nEventNo) {

//-----------------------------------------------------------------------------------------------
	case EV_OPEN:												// オープン要求
		if(NULL != mcls_pPipe) break;

		// 共有メモリ オープン
		mem_connect(&m_hMap, (LPVOID *)&mtbl_pMap, m_cSmemName);
		if(m_hMap == NULL) {
			LOG(em_ERR), "[%s] フレーム情報受け渡しテーブル アクセス失敗", my_sThreadName);			
			syslog(SYSNO_MEM_OPEN, "フレーム情報受け渡しテーブル アクセス失敗");
		
			//m_emErrCode 		= CPIPE_ERR_SMEM;
			//m_nDetailErrCode	= 0;
			//SetEvent(m_evOnError);							// 外部へ通知
			CPIPE_ERROR * pCPipeErr = new CPIPE_ERROR;
			memset(pCPipeErr, 0x00, sizeof(CPIPE_ERROR));
			pCPipeErr->nErrCode		= CPIPE_ERR_SMEM;
			pCPipeErr->nDetailCode	= 0;
			if( ! gque_CPipeErr.AddToTailFreeCheck( pCPipeErr ) ) {
				LOG(em_ERR), "[%s] 受渡しキューフル (ﾃｰﾌﾞﾙｱｸｾｽ失敗)", my_sThreadName);
				delete pCPipeErr;
			}
			break;
		}
	
		{
			FIRST_SHMEM_INFOv1* pSmemH = (FIRST_SHMEM_INFOv1*)mtbl_pMap;	// 共有メモリの先頭アドレス = カメラ情報の位置
			m_bFirstInfoExists = (::IsEqualGUID(ID_FIRST_SHMEM_INFO_IDv1, pSmemH->first_smem_req_id) ? true : false);
		}

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
			//m_nDetailErrCode	= NULL==mcls_pPipe ? 0 : mcls_pPipe->GetError();		// 0;
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


		// パイプ読み込み
		if( !mcls_pPipe->Recv(m_cRecvBuf, PIPE_RECVBUF_SIZE, &m_ovlRecv) ) {
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
		// キューリセット
		m_bConnect = true;
		gque_Deli.ReSetMaxCount(); 
		LOG(em_MSG), "[%s] PIPE 接続完了", my_sThreadName);
		break;



//-----------------------------------------------------------------------------------------------
	case EV_CLOSE:												// クローズ要求
		// パイプクラスのインスタンスがある場合は、開放
		if(NULL != mcls_pPipe) {
			// パイプ
			mcls_pPipe->Cancel();									// 未処理のI/Oを取り消す
			SAFE_DELETE(mcls_pPipe);									// インスタンス開放
			// 共有メモリ
			mem_close(&m_hMap, (LPVOID *)&mtbl_pMap);

			m_bConnect = false;
			LOG(em_MSG), "[%s] PIPE 切断完了", my_sThreadName);
		}
		break;



//-----------------------------------------------------------------------------------------------
	case EV_RECV_END:											// 受信完了通知
		// 非同期結果問い合わせ (受信待ち状態で、I/Oキャンセル時もコールされる)
		bool bRetc;
		if( mcls_pPipe != NULL )	bRetc = mcls_pPipe->GetResult(&m_ovlRecv);
		else						bRetc = false;	
		ResetEvent(m_ovlRecv.hEvent);										// シグナルマニュアルリセット

		// データ取得後の処理
		if( bRetc ) {

			// キューに追加
			BYTE * pNewData = new BYTE[PIPE_RECVBUF_SIZE];
			memset(pNewData, 0x00, PIPE_RECVBUF_SIZE);
			memcpy(pNewData, m_cRecvBuf, PIPE_RECVBUF_SIZE);

			// スレッドキューに登録
			if( gque_Deli.GetFreeCount() > 0 ) {
				gque_Deli.AddToTail(pNewData);		

			} else {												// 空き無し
				LOG(em_WAR), "[%s] キューフル!!", my_sThreadName);
			}
			

			// パイプ読み込み
			if( !mcls_pPipe->Recv(m_cRecvBuf, PIPE_RECVBUF_SIZE, &m_ovlRecv) ) {
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
			//m_emErrCode				= CPIPE_DISCONNECT_ERR;
			//m_nDetailErrCode		= 0; //cls_pPipe->GetError();
			//SetEvent(m_evOnError);
			CPIPE_ERROR * pCPipeErr = new CPIPE_ERROR;
			memset(pCPipeErr, 0x00, sizeof(CPIPE_ERROR));
			pCPipeErr->nErrCode		= CPIPE_DISCONNECT_ERR;
			pCPipeErr->nDetailCode	= 0;
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
// オフセット領域の返却関数
// DWORD ofs 返却するオフセット位置
//------------------------------------------
void DataRecver::SendBack(DWORD ofs)
{
	static DWORD nWk;			// 非同期送信用に静的にエリア確保


	// 複数スレッドからの返却に備えて排他
	AutoLock  lock(&m_csLock);

//	LOG(em_INF), "SendBack(%d)", ofs);
	// 前回の送信が完了していない場合、ちょっと待ち
	DWORD nWaitRet = WaitForSingleObject(m_ovlSend.hEvent, 1000);
	if ( WAIT_TIMEOUT == nWaitRet) {
		LOG(em_WAR), "[%s] オフセット非同期送信 前回値未完了", my_sThreadName);
	}

	// パイプに非同期書き込み
	nWk= ofs;
	if( mcls_pPipe->Send(&nWk, sizeof(DWORD), &m_ovlSend) ) {
		if (ERROR_IO_PENDING == GetLastError()) {
			mcls_pPipe->GetResult(&m_ovlSend);
		}
	}
}

//------------------------------------------
// 共有メモリ内のカメラ情報有無を返す
//------------------------------------------
bool DataRecver::IsFirstInfoExists()
{
	FIRST_SHMEM_INFOv1* pSmemH = (FIRST_SHMEM_INFOv1*)mtbl_pMap;	// 共有メモリの先頭アドレス = カメラ情報の位置
	bool bFirstInfoExists = (::IsEqualGUID(ID_FIRST_SHMEM_INFO_IDv1, pSmemH->first_smem_req_id) ? true : false);

	if (bFirstInfoExists != m_bFirstInfoExists)
	{
		m_bFirstInfoExists = bFirstInfoExists;
		LOG(em_MSG), "[%s] カメラ情報取得：共有メモリ内にカメラ情報%s", my_sThreadName, (m_bFirstInfoExists ? "在り" : "無し"));
	}

	return m_bFirstInfoExists;
}

//------------------------------------------
// 共有メモリ内のカメラ情報を返す
//------------------------------------------
void DataRecver::HelcyStat(ENG_STAT *st)
{
	FIRST_SHMEM_INFOv1* pSmemH = (FIRST_SHMEM_INFOv1*)mtbl_pMap;	// 共有メモリの先頭アドレス = カメラ情報の位置
	st->camnum = pSmemH->camnum;

	if (0 > st->camnum || NCL_ncam_max <= st->camnum) return;

	for (int ii = 0; ii < (int)st->camnum; ii++)
	{
		st->camid[ii] = pSmemH->camid[ii];

		for (int jj = 0; jj < (MAX_CAMANGLE - 1); jj++)
		{
			st->exptime[jj][ii] = (float)pSmemH->exptime[ii][jj];
			st->gain[jj][ii] = pSmemH->gain[ii][jj];
		}
		st->temp[ii] = pSmemH->temp[ii];
		st->volt[ii] = pSmemH->volt[ii];
	}
}
