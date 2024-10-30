// DBManager.cpp: DBManager クラスのインプリメンテーション
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "DBSaveManager.h"

// static 実体
CRITICAL_SECTION	DBSaveManager::m_csLock;
DWORD				DBSaveManager::m_nEndLockNo = 0;
DWORD				DBSaveManager::m_nPushLockNo= 0;


//////////////////////////////////////////////////////////////////////
// 構築/消滅
//////////////////////////////////////////////////////////////////////
//------------------------------------------
// コンストラクタ
// int myID 自分のID (0始まり)
// ThreadQueue<DeliverySQL>* que_pDeli 受け渡し用スレッドキュー
//------------------------------------------
DBSaveManager::DBSaveManager(int myID, ThreadQueue<DeliverySQL>* que_pDeli) :
m_nMyID(myID),
mque_pDeli(que_pDeli),
m_bDBConnect(false),
m_bDBDelete(true),
m_bSendDbManager(false),
m_bReTry(false), 
m_bReTryExecOk(false),
m_cSession(""),
m_evSynclock(NULL),
mcls_pLog(NULL)
{
	//// シグナル生成
	m_evReTryConnect = CreateWaitableTimer(NULL, FALSE, NULL);
	m_evReSet		 = CreateEvent(NULL, FALSE, FALSE, NULL);


	//// ODBC接続クラス インスタンス生成
	mcls_pDB = new OdbcBase();


	// 順序保障
	if(0 == m_nMyID) {
		InitializeCriticalSection(&m_csLock);
	}
}

//------------------------------------------
// デストラクタ
//------------------------------------------
DBSaveManager::~DBSaveManager()
{
	m_bDBConnect = false;

	// シグナル開放
	CloseHandle(m_evReTryConnect);
	CloseHandle(m_evReSet);

	// ODBC接続クラス 開放
	mcls_pDB->DisConnect();
	SAFE_DELETE(mcls_pDB);

	// 順序保障
	if(0 == m_nMyID) {
		DeleteCriticalSection(&m_csLock);
	}
}

//------------------------------------------
// スレッド
//------------------------------------------
UINT WINAPI DBSaveManager::MainThread(LPVOID param)
{
	WORD ii;
	bool bCalcTime;
	DBSaveManager::DeliverySQL *pDeli;				// キューデータ

	////// メインインスタンス取り出し
	DBSaveManager *p = (DBSaveManager *)param;
	p->SetTid();								// 自スレッドIDセット

	// WaitForMultipleObjectsイベント
	HANDLE	hArray[] = {p->my_evStop,
						p->m_evReTryConnect,
						p->m_evReSet,
						p->mque_pDeli->g_evSem 
						};
	enum {	EV_STOP = WAIT_OBJECT_0,		// スレッド終了
			EV_RETRY,						// 再接続要求タイマイベント
			EV_RESET,						// DBリセット要求
			EV_QUE,							// キューイベント
			EV_END	};

	//// 速度測定用
	p->mcls_Counter.InitAve();

	//// DB管理に通知
	p->Send_To_DbManage();

	//// DB初期接続
	p->ConnectDB(true);

	// メッセージキュー読み込み
	pLOG(em_MSG), "[DBSaveManager] <%d> スレッド開始 [0x%X]", p->m_nMyID, p->GetTid());
	while (1) {
		DWORD waitnum = ( p->m_bDBConnect==true ? EV_END : 2);
		DWORD ret = WaitForMultipleObjects(waitnum, hArray, FALSE, INFINITE);

		switch (ret) {

//-----------------------------------------------------------------------------------------------
		case EV_STOP:												// スレッド終了
			pLOG(em_MSG), "[DBSaveManager] <%d> スレッド終了 [0x%X]", p->m_nMyID, p->GetTid());
			// 待ちデータは開放しておく
			if(0==p->m_nMyID) {
				int nCnt = p->mque_pDeli->GetCount();		// 残数
				if(0 != nCnt) pLOG(em_WAR), "[DBSaveManager] <%d> SQL文強制開放 %d件", p->m_nMyID, nCnt);

				while(true) {
					pDeli = p->mque_pDeli->GetFromHead(0);
					if( pDeli == NULL ) break;
					// 必須登録？
					if( pDeli->addmode ) p->sqlExecute(pDeli);
					
					// 開放
					for(ii=0; ii< pDeli->cnt; ii++) {
						SAFE_DELETE_ARRAY(pDeli->pImg[ii]);
					}
					SAFE_DELETE(pDeli->SendMailItem);
					delete pDeli;
				}
		
				// DB削除管理に通知
				p->Send_To_DbManage();
			}
			return 0;

//-----------------------------------------------------------------------------------------------
		case EV_RETRY:												// 再接続要求タイマイベント
		case EV_RESET:												// DBリセット要求
			// 一度切断する
			p->CloseDB();
			// 接続
			p->ConnectDB(false);										// DB初期接続
			break;
		
//-----------------------------------------------------------------------------------------------
		case EV_QUE:												// SQL実行
			// データ取得
			pDeli = p->mque_pDeli->GetFromHead();							// セマフォー減算無しバージョンね
			if(NULL == pDeli) break;

			// 順序保障チェック
			if( pDeli->bIsLock ) {
				while(true) {
					DWORD no = p->ReadEndLockNo();
					if( no+1 == pDeli->nLockNo ) break;
					pLOG(em_INF), "[DBSaveManager] <%d> 実行順番待ち！(現=%d 要求=%d)", p->m_nMyID, no, pDeli->nLockNo);
					Sleep(60);
				}
			}


			// 速度測定用
			bCalcTime = pDeli->bCalcTime;
			if(bCalcTime) p->mcls_Counter.CounterStart();
			// 登録
			p->sqlExecute(pDeli);


			// 順所保障完了
			if( pDeli->bIsLock ) {
				pLOG(em_INF), "[DBSaveManager] <%d> 順序保障SQL実行完了 (%d→%d)", p->m_nMyID, m_nEndLockNo, pDeli->nLockNo);
				m_nEndLockNo = pDeli->nLockNo;
			}


			// 開放
			for(ii=0; ii<pDeli->cnt; ii++) {
				SAFE_DELETE_ARRAY(pDeli->pImg[ii]);
			}
			SAFE_DELETE(pDeli->SendMailItem);
			delete pDeli;
			// 速度測定用
			if(bCalcTime) p->mcls_Counter.CounterEnd();

			// DB削除管理に通知
			p->Send_To_DbManage();
			break;

//-----------------------------------------------------------------------------------------------
		default :
			// ありえない！！
			break;
		}
	}
	return 0;
}

//------------------------------------------
// DB削除管理に通知
//------------------------------------------
void DBSaveManager::Send_To_DbManage()
{
	if( ! m_bSendDbManager ) return ;

	// 現在のQue数を取得
	int cnt = mque_pDeli->GetCount();
	// モード判定
	bool flg;
	if(cnt <= 100) flg = false;			// 削除優先
	else		   flg = true;			// 書き込み優先
	// モード変更無しの場合は 終わり
	if(flg == m_bDBDelete) return;
	m_bDBDelete = flg;

	// 送信
	COMMON_QUE que;
	que.nEventNo = FACT_TI_DELETE_01;
	que.nLineNo = LINE_ID;				
	que.fl.data[0] = (flg ? 1 : 0 );	// 0:削除優先 1:書込み優先
	send_mail(TI_DELETE, ".", &que);
}

//------------------------------------------
// SQL実行
// DeliverySQL * pDeli 受け渡し構造体ポインタ
//------------------------------------------
void DBSaveManager::sqlExecute(DeliverySQL * pDeli)
{
	bool bRetc;

	//// SQL文チェック
	if( 0 == pDeli->strSql.GetLength() ) {		// SQL文無し
		if(NULL != m_evSynclock) {
			// 別スレッドで前のSQLがまだ実行中の可能性がある為、ちょっとだけ待ち
			Sleep(500);
			// 登録状況問い合わせ通知だった
			SetEvent(m_evSynclock);
		}

	} else {									// SQL文有り

		////// DB登録
/*
		if (0 != pDeli->cnt) {						// 画像データ
			// 複数バイナリデータ一括書込み
			if( ! mcls_pDB->SqlBinalyWrite((LPCSTR)pDeli->strSql, pDeli->cnt, pDeli->size, (LPVOID*)pDeli->pImg ) ) {
				LOG(em_ERR), "[DBSaveManager] <%d> SQLバイナリ登録エラー", m_nMyID);
				LOG(em_NON), "%s", pDeli->strSql);
				syslog(SYSNO_DB_BINALYWRITE_ERR, "[DBSaveManager]");
				goto Ending;
			}

		} else {									// 通常のSQL文のみ		
			if( !mcls_pDB->ExecuteDirect((LPCSTR)pDeli->strSql)) {
				LOG(em_ERR), "[DBSaveManager] <%d> SQL登録エラー", m_nMyID);
				LOG(em_NON), "%s", pDeli->strSql);
				syslog(SYSNO_DB_EXECUTEDIRECT_ERR, "[DBSaveManager]");
				goto Ending;
			}
		}
*/
		if (0 != pDeli->cnt) {						// 画像データ

			// 複数バイナリデータ一括書込み
			bRetc = mcls_pDB->SqlBinalyWrite((LPCSTR)pDeli->strSql, pDeli->cnt, pDeli->size, (LPVOID*)pDeli->pImg );
			// リトライ処理
			if(!bRetc && m_bReTry && m_bReTryExecOk) {
				LOG(em_INF), "[DBSaveManager] <%d> SQLバイナリ登録エラーにより、即時リトライ開始！", m_nMyID);
				m_bReTryExecOk = false;
				CloseDB();
				ConnectDB(false);										// DB初期接続
				bRetc = mcls_pDB->SqlBinalyWrite((LPCSTR)pDeli->strSql, pDeli->cnt, pDeli->size, (LPVOID*)pDeli->pImg );
			}

			if( ! bRetc ) {
				LOG(em_ERR), "[DBSaveManager] <%d> SQLバイナリ登録エラー", m_nMyID);
				LOG(em_NON), "%s", pDeli->strSql);
				syslog(SYSNO_DB_BINALYWRITE_ERR, "[DBSaveManager]");
				goto Ending;
			}

		} else {									// 通常のSQL文のみ	
			bRetc = mcls_pDB->ExecuteDirect((LPCSTR)pDeli->strSql);
			// リトライ処理
			if(!bRetc && m_bReTry && m_bReTryExecOk) {
				LOG(em_INF), "[DBSaveManager] <%d> SQLバイナリ登録エラーにより、即時リトライ開始！", m_nMyID);
				m_bReTryExecOk = false;
				CloseDB();
				ConnectDB(false);										// DB初期接続
				bRetc = mcls_pDB->ExecuteDirect((LPCSTR)pDeli->strSql);
			}

			if( ! bRetc ) {
				LOG(em_ERR), "[DBSaveManager] <%d> SQL登録エラー", m_nMyID);
				LOG(em_NON), "%s", pDeli->strSql);
				syslog(SYSNO_DB_EXECUTEDIRECT_ERR, "[DBSaveManager]");
				goto Ending;
			}
		}

		////// コミット
		mcls_pDB->TranCommit();
		m_bReTryExecOk = true;			// 1回でもSQLが成功したら、リトライしてもOK
	}

	////// 登録完了時にメールスロット送信用
	if(NULL != pDeli->SendMailItem) {
		LOG(em_INF), "[DBSaveManager] <%d> SQL登録後のMail送信 [%s:%s:%d]", m_nMyID, pDeli->SendMailItem->cTask, pDeli->SendMailItem->cPc, pDeli->SendMailItem->Que.nEventNo);
		send_mail(pDeli->SendMailItem->cTask, pDeli->SendMailItem->cPc, &pDeli->SendMailItem->Que);
	}
	
	return;
Ending: 
	//// データベース切断
	CloseDB();
	//mcls_pDB->DisConnect();
}

//------------------------------------------
// DB接続
// bool bLogDsp ログ出力有無 (false:失敗ログ出さない)
//------------------------------------------
bool DBSaveManager::ConnectDB(bool bLogDsp)
{
	bool retc;
	
	// リトライタイマーが残っていたらキャンセルしておく
	CancelWaitableTimer(m_evReTryConnect);

	// DB接続
	if( 0 == m_cSession.GetLength() ) {
		retc = mcls_pDB->Connect(TASKINI_NAME);
	} else {
		retc = mcls_pDB->Connect(TASKINI_NAME, m_cSession);
	}

	// DB接続
	if( ! retc ) {
		// データベース接続エラー
		if(bLogDsp) {
			LOG(em_ERR), "[DBSaveManager] <%d> 接続エラー", m_nMyID);
			syslog(SYSNO_DB_CONNECT_ERR,"[DBSaveManager]");
		}
		m_bDBConnect = false;

	} else {
		LOG(em_MSG), "[DBSaveManager] <%d> 接続完了", m_nMyID);
		m_bDBConnect = true;
	}

	//// 接続異常だったら リトライ処理開始
	if( ! m_bDBConnect ) {
		LARGE_INTEGER ts;
		ts.QuadPart =(__int64) PARAM_DB_RETRY_TIME * -10000;	// 1[ms]→100[ns] (マイナス指定で、指定時間経過でシグナル) (第3引数は2回目以降の起動間隔)
		SetWaitableTimer(m_evReTryConnect, &ts, 0, NULL, NULL, FALSE);
	}
	return m_bDBConnect;
}

//------------------------------------------
// DB切断
//------------------------------------------
void DBSaveManager::CloseDB()
{
	if( ! m_bDBConnect ) return;		// 既に切断済み
	
	// 切断
	mcls_pDB->DisConnect();
	m_bDBConnect = false;
	LOG(em_WAR), "[DBSaveManager] <%d> 切断", m_nMyID);

	// リトライ
	LARGE_INTEGER ts;
	ts.QuadPart = (__int64)PARAM_DB_RETRY_TIME * -10000;	// 1[ms]→100[ns] (マイナス指定で、指定時間経過でシグナル) (第3引数は2回目以降の起動間隔)
	SetWaitableTimer(m_evReTryConnect, &ts, 0, NULL, NULL, FALSE);
}
