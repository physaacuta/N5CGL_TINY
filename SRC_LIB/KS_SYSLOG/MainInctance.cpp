#include "StdAfx.h"
#include "MainInctance.h"

//------------------------------------------
// コンストラクタ
//------------------------------------------
MainInctance::MainInctance(void) :
m_bSyslogDsp(false),
m_nDay(0),
m_nAutoNo(0),
m_nOdbcID(0),
mque_AddDB(512),
mcls_pLog(NULL)
{
	// シグナル生成
	m_evThStart  = CreateEvent(NULL, FALSE, FALSE, NULL);				// メインスレッド同期用
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
	//// メインインスタンスのみ、スレッドだけは作っておく (すぐに非シグナルとする)
	//// 継承元クラス実行 (メイン制御スレッドの為、ちょっと優先度高め)
	WorkThread::Start(MainThread, THREAD_PRIORITY_ABOVE_NORMAL );

	// スレッドIDが割りつくまでちょっと休憩
	while( 0==GetTid() ) {
		LOG(em_MSG), "[MainInctance] スレッドID割り当て中・・・");
		Sleep(1);			// スレッド起動までちょっと待ち
	}

	// AutoNoに必要なデータを初期セットする
	InitAutoNo();

	//// ワーカースレッド 初期化
	// DB書込みクラス
	gcls_pDb = new DBSaveManager(0, &mque_AddDB);
	gcls_pDb->SetLogMgr(mcls_pLog);
	gcls_pDb->SetMainPostTid(GetTid());

	//// ワーカースレッド 起動
	gcls_pDb->Start();

	return TRUE;
}

//------------------------------------------
// 終了処理 (一度のみ起動)
//------------------------------------------
void MainInctance::Exit()
{
	//// ワーカースレッド 停止
	gcls_pDb->Stop();

	//// ワーカースレッド 開放
	SAFE_DELETE(gcls_pDb);
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
			pLOG(em_INF), "[MainInctance] 現在のAutoNo=%d", p->m_nAutoNo);
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
// Syslogをスレッドキューに登録
// COMMON_QUE const* que メールスロットデータ
//------------------------------------------
void MainInctance::AddSysLogSqlQue(COMMON_QUE const* que)
{
	// SQL文作成
	CString cSql;
	CTime t = CTime::GetCurrentTime();

	// 日付によるオートNoリセット
	RefrashAutoNo(t.GetDay());

	// SQL文生成
	cSql.Format( "INSERT INTO %s VALUES('%d/%d/%d %d:%d:%d', '%s', '%s', %d, %d, '%s', '%s', %d)",
					DB_LOG_INF,
					que->syslog_que.date[0],	que->syslog_que.date[1], que->syslog_que.date[2], 
					que->syslog_que.date[3],	que->syslog_que.date[4], que->syslog_que.date[5],
					que->syslog_que.cTaskName,	que->syslog_que.cPcName, que->syslog_que.nPcKind,
					que->syslog_que.nSysNo,		que->syslog_que.cMsg,
					t.Format("%Y/%m/%d %H:%M:%S"), (m_nAutoNo*10 + m_nOdbcID)
	);

	// 登録可能？
	if( mque_AddDB.GetFreeCount() < 10 ) {
		// 満杯
		LOG(em_ERR), "[MainInctance] キューフル！！ [%s]", cSql );
	} else {
		// データセット
		DBSaveManager::DeliverySQL * pDeli = new DBSaveManager::DeliverySQL();
		pDeli->strSql = cSql;
		pDeli->addmode	= false;
		pDeli->cnt = 0;

		// 登録
		mque_AddDB.AddToTail(pDeli); 
	}

	// ログ出力
	if(m_bSyslogDsp) {
		LOG(em_NON), "[%04d/%02d/%02d %02d:%02d:%02d] %04d  %s \t %s \t PCKIND=%02d | %s ",
					que->syslog_que.date[0],	que->syslog_que.date[1],	que->syslog_que.date[2], 
					que->syslog_que.date[3],	que->syslog_que.date[4],	que->syslog_que.date[5],
					que->syslog_que.nSysNo,		que->syslog_que.cTaskName,	que->syslog_que.cPcName,
					que->syslog_que.nPcKind,	que->syslog_que.cMsg );
	}
}

//------------------------------------------
// Statelogをスレッドキューに登録
// COMMON_QUE const* que メールスロットデータ
//------------------------------------------
void MainInctance::AddStateLogSqlQue(COMMON_QUE const* que)
{
	// SQL文作成
	CString cSql;
	CTime t = CTime::GetCurrentTime();

	// 日付によるオートNoリセット
	RefrashAutoNo(t.GetDay());

	// SQL文生成
	cSql.Format( "INSERT INTO %s(発生日付時刻,発生元,PC名,PCKIND,アラームNo,内容,登録日付,AutoNo) SELECT '%d/%d/%d %d:%d:%d', '%s', '%s', %d, %d, '<'+ 機器名 +'> [備考:'+ 異常MSG +'] %s (%d)', '%s', %d FROM %s WHERE 機器ID=%d",
					DB_LOG_INF,
					que->statelog_que.date[0],		que->statelog_que.date[1], que->statelog_que.date[2], 
					que->statelog_que.date[3],		que->statelog_que.date[4], que->statelog_que.date[5],
					que->statelog_que.cTaskName,	que->statelog_que.cPcName, que->statelog_que.nPcKind,
					que->statelog_que.nSysNo,		
					que->statelog_que.cMsg,			que->statelog_que.nStateVal,
					t.Format("%Y/%m/%d %H:%M:%S"),	(m_nAutoNo*10 + m_nOdbcID),
					DB_STATUS_MASTER,				que->statelog_que.nStateId 
	);


	// 登録可能？
	if( mque_AddDB.GetFreeCount() < 10 ) {
		// 満杯
		LOG(em_ERR), "[MainInctance] キューフル！！ [%s]", cSql );
	} else {
		// データセット
		DBSaveManager::DeliverySQL * pDeli = new DBSaveManager::DeliverySQL();
		pDeli->strSql = cSql;
		pDeli->addmode	= false;
		pDeli->cnt = 0;
		// 登録
		mque_AddDB.AddToTail(pDeli); 
	}

	// ログ出力
	if(m_bSyslogDsp) {
		LOG(em_NON), "[%04d/%02d/%02d %02d:%02d:%02d] %04d  %s \t %s \t PCKIND=%02d | %s<機器ID=%d> %s (%d)",
					que->statelog_que.date[0],	que->statelog_que.date[1],		que->statelog_que.date[2], 
					que->statelog_que.date[3],	que->statelog_que.date[4],		que->statelog_que.date[5],
					que->statelog_que.nSysNo,	que->statelog_que.cTaskName,	que->statelog_que.cPcName,
					que->statelog_que.nPcKind,	
					"機器状態変更",				que->statelog_que.nStateId,		que->statelog_que.cMsg,
					que->statelog_que.nStateVal	 );
	}
}
/*
//------------------------------------------
// 日付をチェックして、日が変われば、オートNoをリセットする。
// int nDay 登録日
//------------------------------------------
void MainInctance::ResetAutoNo(int nDay)
{
	CString cSql;					// SQL文

	// 日が変更された？
	if(m_nDay == nDay) return;
	
	// オートNo型をリセット
	cSql.Format("DBCC CHECKIDENT(%s, RESEED, 0)", DB_LOG_INF);

	// 絶対登録
	DBSaveManager::DeliverySQL * pDeli = new DBSaveManager::DeliverySQL();
	pDeli->strSql = cSql;
	pDeli->addmode	= false;
	pDeli->cnt = 0;
	if( ! mque_AddDB.AddToTail(pDeli, 20, 30) ) {
		LOG(em_ERR), "[MI] オートNo型をリセット キューフル！！" );
		SAFE_DELETE(pDeli);
		// 異常の場合でも、メンバー変数の初期化をしておく!!
	}


	// セット
	LOG(em_MSG), "[MI] オートNoリセット 変更日[%d→%d]", m_nDay, nDay);
	m_nDay = nDay;
}
*/
//------------------------------------------
// AutoNoに必要なデータを初期セットする
//------------------------------------------
void MainInctance::InitAutoNo()
{
	//// 現在日をセット
	m_nDay = CTime::GetCurrentTime().GetDay(); 

	//// ODBCの設定位置を取得する
	// 現在のセッションを取得する
	char Session[128];
	GetPrivateProfileString("DB", "SESSION", "", (char *)Session, sizeof(Session), PCINI_NAME);
	// 位置特定
	char SessionWK[128];
	CString		cKey;
	for(int ii=1; ii<10; ii++) {
		cKey.Format("ODBCSET_%d", ii); 
		GetPrivateProfileString("DB", cKey, "", SessionWK, sizeof(SessionWK), PCINI_NAME);
		if(0 == strcmp(Session, SessionWK)) {
			m_nOdbcID = ii;
			break;
		}
	}


	//// 最新日付のAutoNoを取得する
	CString cSql;					// SQL文

	// SQL文
	cSql.Format("SELECT TOP(1) AutoNo FROM %s ORDER BY 登録日付 desc, AutoNo desc", DB_LOG_INF);

	// 実行
	int nWk;
	if( ! KizuODBC::OdbcBase::GetSelectKey_Static(cSql, &nWk, TASKINI_NAME, INI_DB) ) {
		LOG(em_ERR), "[MI] オートNo 最新値 取得失敗！！" );
		m_nAutoNo = 0;
	} else {
		m_nAutoNo = nWk / 10;
		LOG(em_INF), "[MI] オートNo 最新値 DBのAutoNo=%d, 現在日付=%d (連番=%d, ID=%d)", nWk, m_nDay, m_nAutoNo, m_nOdbcID);
	}
}

//------------------------------------------
// 日付をチェックして、日が変われば、オートNoをリセットする。
// int nDay 登録日
//------------------------------------------
void MainInctance::RefrashAutoNo(int nDay)
{
	// 日が変更された？
	if(m_nDay == nDay) {
		// 同じ日なら AutoNoを更新するだけ
		m_nAutoNo ++;
		return;
	}

	// セット
	LOG(em_MSG), "[MI] オートNoリセット 変更日[%d→%d]", m_nDay, nDay);
	m_nDay	  = nDay;
	m_nAutoNo = 1;
}