#include "StdAfx.h"
#include "MainInctance.h"

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
MainInctance::MainInctance(void) :
mcls_pLog(NULL)
{
	// シグナル生成
	m_evThStart  = CreateEvent(NULL, FALSE, FALSE, NULL);				// メインスレッド同期用


	//// 各クラス関連初期化
	memset(g_nClassCnt, 0x00, sizeof(g_nClassCnt));
	for(int ii=0; ii<KizuSyncroManager::PARAM_MAX_THREAD; ii++) {
		gcls_pSync[ii] = NULL;
	}
}


//------------------------------------------
// デストラクタ
//------------------------------------------
MainInctance::~MainInctance(void)
{
	// シグナル開放
	CloseHandle(m_evThStart);
}

//------------------------------------------
// 初期化処理 (一度のみ起動)
//------------------------------------------
BOOL MainInctance::Init()
{
	int ii, jj;

	//// メインインスタンスのみ、スレッドだけは作っておく (すぐに非シグナルとする)
	//// 継承元クラス実行 (メイン制御スレッドの為、ちょっと優先度高め)
	WorkThread::Start(MainThread, THREAD_PRIORITY_ABOVE_NORMAL );

	// スレッドIDが割りつくまでちょっと休憩
	while( 0==GetTid() ) {
		LOG(em_MSG), "[MainInctance] スレッドID割り当て中・・・");
		Sleep(1);			// スレッド起動までちょっと待ち
	}



	//// iniファイル読み込み 
	int		nCnt = 1;
	int		nType;				// セッションのタイプ
	CString	cSession;			// セッション

	char    cWk[1024];
	CString strVal;

	while(true) {
		nType = -1;
		cSession = "";

		strVal.Format("SESSION_%d", nCnt);
		GetPrivateProfileString(KS_EQALDB, strVal, "", cWk, sizeof(cWk), TASKINI_NAME);		
	
		if( 0 < strlen(cWk) ) {
			strVal = cWk;
			
			//// トークン分割
			int		rec = 0;	
			int		curPos = 0;
			CString strToken = strVal.Tokenize(",", curPos);
			while ( strToken != "" ) {
				strToken.Trim(_T(" 	"));		// iniファイル設定文字の前後のスペース、タブを取り除く

				switch( rec ) {
				case 0:
					cSession = strToken;
					break;

				case 1:
					nType = atoi(strToken); 
					break;
				}

				// 次へ
				strToken = strVal.Tokenize(",", curPos);
				rec ++;
			}
			if( 2 != rec || -1 == nType || 0 == cSession.GetLength() ) break;		// 分割したトークンが有効？
			if( KizuSyncroManager::PARAM_MAX_THREAD <= g_nClassCnt[nType] ) break;

			//// 今回取得した情報からクラスを生成
			if		(TYPE_DB_SYNC == nType)		gcls_pSync[ g_nClassCnt[nType] ] = new SyncManager(cSession, mcls_pLog);
			else if	(TYPE_DB_COPY == nType)		gcls_pCopy[ g_nClassCnt[nType] ] = new CopyManager(cSession, mcls_pLog);
			else if	(TYPE_DB_MOVE == nType)		gcls_pMove[ g_nClassCnt[nType] ] = new MoveManager(cSession, mcls_pLog);
			else if	(TYPE_DB_SYNCOPY == nType)	gcls_pSynCopy[ g_nClassCnt[nType] ] = new SyncopyManager(cSession, mcls_pLog);

			g_nClassCnt[nType] ++;

			// 次へ
			nCnt++;

		} else {
			break;
		}
	}

	
	//// リンクサーバー定義
	nCnt = 1;
	while(true) {
		strVal.Format("LINKSRV_%d", nCnt);
		GetPrivateProfileString(strVal, "SELECT_ODBC_KEY", "", cWk, sizeof(cWk), TASKINI_NAME);		
		if( 0 >= strlen(cWk) ) break;

		AddLinkServer(strVal);		// リンクサーバー定義

		// 次へ
		nCnt++;
	}


	//// 使いやすいように各クラスの参照をセットしておく
	for(ii=0; ii<KizuSyncroManager::PARAM_MAX_THREAD; ii++) {
		if(NULL != gcls_pSync[ii] ) gcls_pDb[TYPE_DB_SYNC][ii] = gcls_pSync[ii];
		else						gcls_pDb[TYPE_DB_SYNC][ii] = NULL;

		if(NULL != gcls_pCopy[ii] ) gcls_pDb[TYPE_DB_COPY][ii] = gcls_pCopy[ii];
		else						gcls_pDb[TYPE_DB_COPY][ii] = NULL;

		if(NULL != gcls_pMove[ii] ) gcls_pDb[TYPE_DB_MOVE][ii] = gcls_pMove[ii];
		else						gcls_pDb[TYPE_DB_MOVE][ii] = NULL;

		if(NULL != gcls_pSynCopy[ii] ) gcls_pDb[TYPE_DB_SYNCOPY][ii] = gcls_pSynCopy[ii];
		else						   gcls_pDb[TYPE_DB_SYNCOPY][ii] = NULL;
	}

/*
	//// ワーカースレッド 初期化
	for(ii=0; ii<TYPE_DB_END; ii++) {
		for(jj=0; jj<g_nClassCnt[ii]; jj++) {
//			gcls_pDb[ii][jj]->SetLogMgr(mcls_pLog); 
		}
	}
*/

	//// ワーカースレッド 起動
	for(ii=0; ii<TYPE_DB_END; ii++) {
		for(jj=0; jj<g_nClassCnt[ii]; jj++) {
			gcls_pDb[ii][jj]->Start(); 
		}
	}

	return TRUE;
}

//------------------------------------------
// 終了処理 (一度のみ起動)
//------------------------------------------
void MainInctance::Exit()
{
	int ii, jj;

	//// ワーカースレッド 停止通知
	for(ii=0; ii<TYPE_DB_END; ii++) {
		for(jj=0; jj<g_nClassCnt[ii]; jj++) {
			gcls_pDb[ii][jj]->SetStop(); 
		}
	}
	//// ワーカースレッド 停止
	for(ii=0; ii<TYPE_DB_END; ii++) {
		for(jj=0; jj<g_nClassCnt[ii]; jj++) {
			gcls_pDb[ii][jj]->Stop(); 
		}
	}

	//// ワーカースレッド開放
	for(ii=0; ii<TYPE_DB_END; ii++) {
		for(jj=0; jj<g_nClassCnt[ii]; jj++) {
			SAFE_DELETE( gcls_pDb[ii][jj] );
		}
	}
}


//------------------------------------------
// 即時実行を振り分け
//------------------------------------------
void MainInctance::SelectNowExec(int nAtoB, int nMailKey)
{
	int ii, jj;

	for(ii=0; ii<TYPE_DB_END; ii++) {
		for(jj=0; jj<g_nClassCnt[ii]; jj++) {
			gcls_pDb[ii][jj]->SetEvNowExec(nAtoB, nMailKey); 
		}
	}
}

//------------------------------------------
// スレッド
//------------------------------------------
UINT WINAPI MainInctance::MainThread(LPVOID param)
{
	MSG	msg;								// MSG構造体

	////// メインインスタンス取り出し
	MainInctance *p = (MainInctance *)param;						// 自クラスのインスタンスを取得
	PeekMessage(&msg, NULL, WM_USER, WM_USER, PM_NOREMOVE);			// メッセージ作成
	p->SetTid();													// スレッドIDセット


	////// 他ワークスレッドの準備が整うまで待機
	pLOG(em_MSG), "[MainInctance] 他ワークスレッドの準備が整うまで待機中・・・ [0x%X]", p->GetTid() );
	WaitForSingleObject(p->m_evThStart, INFINITE);


	////// メッセージキュー読み込み
	pLOG(em_MSG), "[MainInctance] スレッド開始 [0x%X]", p->GetTid());
	while( int ret = GetMessage(&msg, NULL, 0, 0) ){
		_ASSERT( -1 != ret );
		switch (msg.message) {

		case WM_USER_TH_END: 					// スレッド終了要求
			pLOG(em_MSG), "[MainInctance] スレッド終了 [0x%X]", p->GetTid());
			return 0;

		default :
			// ありえない！！
			break;
		}
	}

	// ありえない
	pLOG(em_ERR), "[MainInctance] メインループから抜けた！！[0x%X]", p->GetTid());
	return 0;
}


//------------------------------------------
// リンクサーバーを設定
// char const* cKey 定義設定キー
//------------------------------------------
void MainInctance::AddLinkServer(char const* cKey)
{
	char	cOdbc[64];
	char	cDbOnLink[64];
	char	cDbOnAddr[64];
	char	cDbOnDsn [64];
	char	cDbOnUser[64];
	char	cDbOnPass[64];
	char	cSrvproduct[64];
	char	cProvider[64];
						
	// iniファイル定義取得
	GetPrivateProfileString(cKey, "SELECT_ODBC_KEY", "", cOdbc, sizeof(cOdbc), TASKINI_NAME);		
	GetPrivateProfileString(cKey, "LINK_NAME", "",	cDbOnLink, sizeof(cDbOnLink), TASKINI_NAME);
	GetPrivateProfileString(cKey, "DBADDLESS", "",	cDbOnAddr, sizeof(cDbOnAddr), TASKINI_NAME);		
	GetPrivateProfileString(cKey, "DSNAME", "",		cDbOnDsn, sizeof(cDbOnDsn), TASKINI_NAME);		
	GetPrivateProfileString(cKey, "USERID", "",		cDbOnUser, sizeof(cDbOnUser), TASKINI_NAME);		
	GetPrivateProfileString(cKey, "PASSWORD", "",	cDbOnPass, sizeof(cDbOnPass), TASKINI_NAME);		
	GetPrivateProfileString(cKey, "SRVPRODUCT", "", cSrvproduct, sizeof(cSrvproduct), TASKINI_NAME);		
	GetPrivateProfileString(cKey, "PROVIDER", "",	cProvider, sizeof(cProvider), TASKINI_NAME);



	// 追加したいDBに接続
	OdbcBase	clsDB;
	if( ! clsDB.Connect(TASKINI_NAME, cOdbc)) {
		// データベース接続エラー
		LOG(em_ERR), "[MainInctance] [%s] DB接続エラー", cOdbc);
		syslog(SYSNO_DB_CONNECT_ERR, "[MainInctance] [%s]", cOdbc);
		return;
	}

	// リンクサーバー追加
	if( ! clsDB.ExecuteDirect_AddLinkServer(
		cDbOnLink, cDbOnAddr, cDbOnDsn, cDbOnUser, cDbOnPass, true, cSrvproduct, cProvider) ){
		LOG(em_ERR), "[MainInctance] リンクサーバー追加失敗");
		return;
	}

	LOG(em_MSG), "[MainInctance] リンクサーバー追加完了 [%s] [%s:%s:%s]", cOdbc, cDbOnLink, cDbOnAddr, cDbOnDsn);
}
