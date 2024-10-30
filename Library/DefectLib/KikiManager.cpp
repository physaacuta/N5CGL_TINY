#include "StdAfx.h"
#include "KikiManager.h"


//////////////////////////////////////////////////////////////////////
// 構築/消滅
//////////////////////////////////////////////////////////////////////
//------------------------------------------
// コンストラクタ
// int nCycleTime 機器状態の取り込み周期 [ms]
// LPCSTR cIniPath 接続DBの定義iniファイル
// LPCSTR cSession 接続DBの定義セッション
//------------------------------------------
KikiManager::KikiManager(int nCycleTime, LPCSTR cIniPath, LPCSTR cSession) :
m_nCycleTime(nCycleTime),
m_nMsgReadCnt(0),
m_bOutPutLog(false),
m_bIsKadou(false),
m_nSysArea_BitPattan(0),
mque_Deli(512),		// 状態登録が増えたため128から512に変更
mcls_pDB(NULL),
mcls_pLog(NULL)
{
	// 初期化
	InitializeCriticalSection(&m_csLock);									// クリティカルセクションオブジェクトを初期化
	m_evTimeCycleRead = CreateWaitableTimer(NULL, FALSE, NULL);	
	m_evReadEnd		  = CreateEvent(NULL, FALSE, FALSE, NULL);					// 非同期読込み完了イベント
	m_evLockReadStart = CreateEvent(NULL, FALSE, FALSE, NULL);				// 強制読込み開始イベント
	m_evLockReadEnd	  = CreateEvent(NULL, FALSE, FALSE, NULL);				// 強制読込み完了イベント
	m_evSyncRead	  = CreateEvent(NULL, FALSE, FALSE, NULL);	
	memset(m_typStatInf, 0x00, sizeof(m_typStatInf));
	memset(&m_typStatNow, 0x00, sizeof(m_typStatNow));

	// iniファイルの定義
	memset(m_cIniPath, 0x00, sizeof(m_cIniPath));
	memset(m_cIniSession, 0x00, sizeof(m_cIniSession));
	strcpy(m_cIniPath, cIniPath);
	strcpy(m_cIniSession, cSession);

	//// ODBC接続クラス インスタンス生成
	mcls_pDB = new OdbcBase(); 
	// DB初期接続
	ConnectDB(true);

	// 一回データを取得しておく
	CheckStatus();
	SetEvent(m_evReadEnd);										// 読込み完了シグナルセット
}

//------------------------------------------
// デストラクタ
//------------------------------------------
KikiManager::~KikiManager(void)
{
	// ODBC接続クラス 開放
	mcls_pDB->DisConnect();
	SAFE_DELETE(mcls_pDB);

	//// 開放
	CancelWaitableTimer(m_evTimeCycleRead);
	CloseHandle(m_evTimeCycleRead);
	CloseHandle(m_evReadEnd);
	CloseHandle(m_evLockReadStart);
	CloseHandle(m_evLockReadEnd);
	CloseHandle(m_evSyncRead);
	DeleteCriticalSection(&m_csLock);							// クリティカルセクションオブジェクトを開放
}

//------------------------------------------
// スレッド
//------------------------------------------
UINT WINAPI KikiManager::MainThread(LPVOID param)
{
	KikiManager::DELIVERYSTATUS* pDeli;			// キューデータ

	////// メインインスタンス取り出し
	KikiManager *p = (KikiManager *)param;
	p->SetTid();								// 自スレッドIDセット

	// 定周期タイマー
	if(0 != p->m_nCycleTime) {
		__int64 ts = 100 * -10000;			// 1ミリ秒→100ナノ秒 (マイナス指定で、指定時間経過でシグナル) (第3引数は2回目以降の起動間隔)
		SetWaitableTimer(p->m_evTimeCycleRead,   (LARGE_INTEGER *)&ts, p->m_nCycleTime,   NULL, NULL, FALSE);
	}

	// WaitForMultipleObjectsイベント
	HANDLE	hArray[] = {p->my_evStop,
						p->m_evLockReadStart,
						p->mque_Deli.g_evSem,
						p->m_evTimeCycleRead,
						p->m_evSyncRead
						};
	enum {	EV_STOP = WAIT_OBJECT_0,		// スレッド終了
			EV_READ_START,					// 強制読込み
			EV_QUE,							// キューイベント
			EV_READ_CYCLE,					// 定周期読み込み
			EV_SYNC_READ,					// 非同期読み込み
			EV_END	};


//p->mcls_Counter.InitAve();
	// メッセージキュー読み込み
	pLOG(em_MSG), "[KikiManager] スレッド開始 [0x%X]", p->GetTid());
	while (1) {
		DWORD ret = WaitForMultipleObjects(EV_END, hArray, FALSE, INFINITE);

		switch (ret) {

//-----------------------------------------------------------------------------------------------
		case EV_STOP:												// スレッド終了
			CancelWaitableTimer(p->m_evTimeCycleRead);

			// 待ちデータは全部登録しておく
			if(true) {
				int nCnt = p->mque_Deli.GetCount();		// 残数
				if(0 != nCnt) pLOG(em_WAR), "[KikiManager] ステータス変更強制セット %d件", nCnt);

				while(true) {
					pDeli = p->mque_Deli.GetFromHead(0);
					if( pDeli == NULL ) break;

					p->SetStatusThread(pDeli);			
					delete pDeli;
				}	
			}
			pLOG(em_MSG), "[KikiManager] スレッド終了 [0x%X]", p->GetTid());

			return 0;

//-----------------------------------------------------------------------------------------------
		case EV_READ_START:											// 強制読込み
//pLOG(em_MSG), "[KikiManager] 強制ステータス読込開始");
			p->m_nMsgReadCnt = 1;											// 強制読込みは早く読みたいからMSGは未取得とする
			p->CheckStatus();
			SetEvent(p->m_evLockReadEnd);									// 読込み完了シグナルセット
//pLOG(em_MSG), "[KikiManager] 強制ステータス読込完了");
			break;
	
//-----------------------------------------------------------------------------------------------
		case EV_QUE:												// 非同期ステータス変更通知
			pDeli = p->mque_Deli.GetFromHead();
			if( pDeli == NULL ) break;

			p->SetStatusThread(pDeli);			
			delete pDeli;

			// 読み込み依頼を指定しておく
			SetEvent(p->m_evSyncRead);
			break;

//-----------------------------------------------------------------------------------------------
		case EV_READ_CYCLE:											// 定周期 機器状態チェック
		case EV_SYNC_READ:											// 非同期読み込み
//pLOG(em_MSG), "[KikiManager] ステータス読込開始");
//p->mcls_Counter.CounterStart(); 
			p->CheckStatus();
			SetEvent(p->m_evReadEnd);										// 読込み完了シグナルセット
//p->mcls_Counter.CounterEnd(); 
//pLOG(em_MSG), "[KikiManager] ステータス読込完了");
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
// 強制更新 (読込み完了するまでブロックされる)
//------------------------------------------
void KikiManager::Refresh()
{
	SetEvent(m_evLockReadStart);
	WaitForSingleObject(m_evLockReadEnd, 2000);
}

//------------------------------------------
// DB接続
// bool bLogDsp ログ出力有無 (false:失敗ログ出さない)
//------------------------------------------
bool KikiManager::ConnectDB(bool bLogDsp)
{
	// チェック
	if(mcls_pDB->IsConnect()) return true;			// 既にに接続済み
	// DB接続
	//if( ! mcls_pDB->Connect(TASKINI_NAME)) {
	if( ! mcls_pDB->Connect(m_cIniPath, m_cIniSession)) {
		// データベース接続エラー
		if(bLogDsp) {
			LOG(em_ERR), "[KikiManager] 接続エラー");
			syslog(SYSNO_DB_CONNECT_ERR,"[KikiManager]");
		}
		return false;

	} else {
		LOG(em_MSG), "[KikiManager] 接続完了");
		return true;
	}
}

//------------------------------------------
// DB切断
//------------------------------------------
void KikiManager::CloseDB()
{
	if( ! mcls_pDB->IsConnect() ) return;		// 既に切断済み
	
	// 切断
	mcls_pDB->DisConnect();
}


// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ステータス読込関係

//------------------------------------------
// 機器状態取得 (セーフスレッドでは無い為注意すること)
//------------------------------------------
bool KikiManager::CheckStatus()
{
	TYP_STATUS_INF			wkInf[MAX_STATUS];				// 取得機器状態ワーク
	TYP_STATUS_NOWSYSTEM	wkNow;							// 現異常状態
	TYP_STATUS_NOWSYSTEM	wkOkNow;						// OKの場合メッセージセット用ワーク Ver.04

	CString cSql;											// SQL文
	int		id;
	bool	isRun = m_bIsKadou;								// 途中で書き換わってもいいようにローカルに持つ
	int		area_sys;										// システム範囲

	//memset( wkInf, 0x00, sizeof(wkInf));
	memcpy( &wkInf, &m_typStatInf, sizeof(wkInf));	// 前回値でコピー
	memset( &wkNow, 0x00, sizeof(wkNow));
	memset( &wkOkNow, 0x00, sizeof(wkOkNow));

	// 接続確認
	if( ! ConnectDB(false) ) {
		LOG(em_ERR), "[KikiManager] DB接続失敗！！");
		return false;					// DB接続できない為、辞め
	}

	//// 初期値セット
	if(isRun) {
		wkNow.kensa[0] = DIV_KENSA_OK;
		wkNow.kensa[1] = DIV_KENSA_OK;
	} else {
		wkNow.kensa[0] = DIV_KENSA_NON;
		wkNow.kensa[1] = DIV_KENSA_NON;
	}

	//// 機器状態取得
	// 機器状態、機器マスター だけでなく、PCに紐付く状態も取得。だって、オフラインのPCだったら異常としたくないから
	// Where句を入れるの禁止。機器状態は、正常異常関係なく全件取得すること
	cSql = "SELECT m.機器ID, m.PCID, m.異常範囲, m.検査異常度, m.機器異常度, m.優先度, ISNULL(p.PC区分,0), i.機器状態 ";
	// システム範囲に対応(0,NULL は全部対象) Ver.02
	if( 0 != m_nSysArea_BitPattan ) cSql += ", (CASE WHEN m.システム範囲 IS NULL OR m.システム範囲=0 THEN 0xffffffff ELSE m.システム範囲 END) ";		
	else							cSql += ", 0 ";
	// 負荷低減対策
	if( 0 == m_nMsgReadCnt)			cSql += ", m.異常MSG ";

	cSql += "FROM (";
		cSql += DB_STATUS_MASTER;
		cSql += " m ";
			cSql += "INNER JOIN ";
			cSql += DB_STATUS_INF;
			cSql += " i ON m.機器ID = i.機器ID ) ";		// ACCESSでもSQL文が通るようする為、カッコで囲むよ
			cSql += " LEFT OUTER JOIN ";
			cSql += DB_PC_INF;
			cSql += " p ON m.PCID = p.PCID ";
	//cSql += "ORDER BY ISNULL(m.優先度, 1000) asc, m.機器異常度 desc, m.検査異常度 desc ";		// Ver.04 検査異常度を追加
	cSql += "ORDER BY ISNULL(m.優先度, 500) asc, m.機器異常度 desc, m.検査異常度 desc ";		// Ver.04 検査異常度を追加


//▲	LOG(em_ERR), "[KikiManager] 開始");

	bool bRetc = mcls_pDB->ExecuteDirect(cSql);
	if( ! bRetc ) {
		LOG(em_WAR), "[KikiManager] ステータス取得失敗！！即時リトライ開始");
		CloseDB();
		if( ! ConnectDB(false) ) {
			LOG(em_ERR), "[KikiManager] DB接続失敗！！");
			return false;					// DB接続できない為、辞め
		}
		bRetc = mcls_pDB->ExecuteDirect(cSql);
	}
	if( ! bRetc ) {
		LOG(em_ERR), "[KikiManager] ExecuteDirectエラー");
		syslog(SYSNO_DB_EXECUTEDIRECT_ERR,"[KikiManager]");
		CloseDB();
		return false;
	}


	while (TRUE) {
		int sqlrc = mcls_pDB->Fetch();
		if (sqlrc == KIZUODBC_FETCH_OK) {						// データ有り 
			//============================
			// 主キー取り出し
			mcls_pDB->GetData(1, &id);
			if( 0>=id || MAX_STATUS < id) {
				LOG(em_ERR), "[KikiManager] データ不正[%d]", id);
				syslog(SYSNO_DB_DATA_ERR,"[KikiManager data=%d]", id);
				CloseDB();
				return false;
			}

			id = id -1;
			// データセット
			wkInf[id].id = id+1;
			mcls_pDB->GetData(2, &wkInf[id].pcid);
			mcls_pDB->GetData(3, &wkInf[id].area);
			mcls_pDB->GetData(4, (int*)&wkInf[id].kensa); 
			mcls_pDB->GetData(5, (int*)&wkInf[id].kiki); 
			mcls_pDB->GetData(6, &wkInf[id].yusen); 
			mcls_pDB->GetData(7, &wkInf[id].pcmode);
			mcls_pDB->GetData(8, &wkInf[id].stat); 
				
			// システム範囲に対応 Ver.02 --->>>
			if( 0 != m_nSysArea_BitPattan ) {
				mcls_pDB->GetData(9, &area_sys); 
				
				// 対象外の範囲？
				// ビットパターンに一致するビットが1つでもONだったら対象
				if( (area_sys & m_nSysArea_BitPattan ) == 0 ) continue;
			}
			// <<<-----------------------------

			// 負荷低減対策
			if( 0 == m_nMsgReadCnt)	mcls_pDB->GetData(10, &wkInf[id].msg, sizeof(wkInf[id].msg));
			

			//============================
			// 異常状態取得
				// 異常取得はオンラインのみ
				// 異常範囲が-1は、オンラインへの影響無し(制御部では正常認識。RAS画面等画面にのみ影響) 20081216
			if(0 == wkInf[id].pcmode && 0 != wkInf[id].stat && -1 != wkInf[id].area ) {
				// 表
				if(0 == wkInf[id].area || 1 == wkInf[id].area) {
					// 検査状態
					if( wkNow.kensa[0] < wkInf[id].kensa ) wkNow.kensa[0] = wkInf[id].kensa;
					if( ! isRun) wkNow.kensa[0] = DIV_KENSA_NON;			// 探傷停止中は、検査状態 停止固定
					// 機器状態
					if( wkNow.kiki[0]  < wkInf[id].kiki )  wkNow.kiki[0]  = wkInf[id].kiki;
				}
				// 裏
				if(0 == wkInf[id].area || 2 == wkInf[id].area) {
					// 検査状態
					if( wkNow.kensa[1] < wkInf[id].kensa ) wkNow.kensa[1] = wkInf[id].kensa;
					if( ! isRun) wkNow.kensa[1] = DIV_KENSA_NON;			// 探傷停止中、検査状態 停止固定
					// 機器状態
					if( wkNow.kiki[1]  < wkInf[id].kiki )  wkNow.kiki[1]  = wkInf[id].kiki;
				}	
				
				// 機器異常のやつセット
				if( wkNow.nCnt < 5 && 
					( ( isRun && ( 0 != wkInf[id].kensa || 0 != wkInf[id].kiki)) || (!isRun && 0 != wkInf[id].kiki) )	) {

					memcpy( &wkNow.ngInf[wkNow.nCnt], &wkInf[id], sizeof(wkInf[id]));
					wkNow.nCnt ++;
				}
			}


			//============================
			// 正常メッセージ取り出し Ver.04 ------>>>
			// 優先度が900以下の場合は、メッセージ出力
			if( wkOkNow.nCnt < 5 && 0 != wkInf[id].stat &&
				(STATUS_OK_MSG_YUUSENID_MIN <= wkInf[id].yusen && STATUS_OK_MSG_YUUSENID_MAX >= wkInf[id].yusen) ) {

				// 正常メッセージセット
				memcpy( &wkOkNow.ngInf[wkOkNow.nCnt], &wkInf[id], sizeof(wkInf[id]));
				wkOkNow.nCnt ++;
			}
			// <<<-------------


		} else if( sqlrc == KIZUODBC_FETCH_NODATE ) {		// データ無し
			break;
		} else {								// エラー
			LOG(em_ERR), "[KikiManager] Fetchエラー");
			syslog(SYSNO_DB_FETCH_ERR, "[KikiManager]");
			CloseDB();
			return false;
		}
	}
	
//▲	LOG(em_ERR), "[KikiManager] 完了");

	// Ver.04 ------>>>
	// 正常の場合、優先度が900以下の場合は、メッセージ出力
	if( 0 == wkNow.nCnt && 0 != wkOkNow.nCnt ) {
		wkNow.nCnt = wkOkNow.nCnt;
		memcpy(wkNow.ngInf, wkOkNow.ngInf, sizeof(wkOkNow.ngInf));  
	}
	// <<<-------------

	// Ver.05 ------>>>
	if( m_bOutPutLog ) {
//▲	LOG(em_ERR), "[KikiManager] 故障ログ参照開始");

		GetLogInfo(&wkNow);

//▲	LOG(em_ERR), "[KikiManager] 故障ログ参照完了");
	}
	// <<<-------------
	
//LOG(em_ERR), "[KikiManager] 完了");
	// メインにセット
	AutoLock	lock(&m_csLock);
	memcpy( m_typStatInf, wkInf, sizeof(m_typStatInf));
	memcpy( &m_typStatNow, &wkNow, sizeof(m_typStatNow));

	// 次回用
	m_nMsgReadCnt ++;
	if(20 < m_nMsgReadCnt) m_nMsgReadCnt = 0;		// 大体3～5秒周期で取得
	return true;
}


//------------------------------------------
// オペガイするメッセージを取得
// TYP_STATUS_NOWSYSTEM* inf セットエリア
//------------------------------------------
void KikiManager::GetLogInfo(TYP_STATUS_NOWSYSTEM* inf)
{
//▲	LOG(em_ERR), "[KikiManager] LOG読込み");

	// 接続確認
	if( ! ConnectDB(false) ) {
		LOG(em_ERR), "[KikiManager] DB接続失敗！！");
		return;					// DB接続できない為、辞め
	}
/*** 見直し 全期間のLOGINFなので膨大な時間が掛かる。
	CString cSql;											// SQL文
	cSql = "SELECT TOP 1 '【'+SUBSTRING(CONVERT(varchar,I.登録日付,111),6,5)+' '+LEFT(CONVERT(varchar,I.登録日付,108),5)+'】 '+ M.内容+' '+I.内容 ";
	cSql += "FROM ";
	cSql += DB_LOG_INF;
	cSql += " as I LEFT OUTER JOIN ";
	cSql += DB_LOG_MASTER;
	cSql += " as M ON I.アラームNo = M.アラームNo ";
	cSql += "WHERE M.オペガイ有無=1 AND DATEDIFF(SECOND, I.登録日付, GETDATE()) <= M.オペガイ時間 ORDER BY I.登録日付 DESC";
***/

	CString cTime;
	CTime	tWk = CTime::GetCurrentTime();
	cTime.Format( "CONVERT(datetime,'%d/%d/%d')-%d", tWk.GetYear(), tWk.GetMonth(), tWk.GetDay(), 3);	// 直近3日に限定

	CString cSql;											// SQL文
	cSql = "SELECT TOP 1 '【'+SUBSTRING(CONVERT(varchar,I.登録日付,111),6,5)+' '+LEFT(CONVERT(varchar,I.登録日付,108),5)+'】 '+ M.内容+' '+I.内容 ";
	cSql += "FROM ";
	cSql += DB_LOG_INF;
	cSql += " as I INNER JOIN ";

	cSql += DB_LOG_MASTER;
	cSql += " as M ON I.アラームNo = M.アラームNo ";
	cSql += " WHERE M.オペガイ有無=1 ";
	cSql += " AND I.登録日付 >= " + cTime;			// 直近3日間。パラメータ指定は2日間分
	cSql += " AND DATEDIFF(SECOND, I.登録日付, GETDATE()) <= M.オペガイ時間 ";
	cSql += " ORDER BY I.登録日付 DESC";



	//// データ取得
	char cWk[256];
	memset(cWk, 0x00, sizeof(cWk));
	if( ! mcls_pDB->GetSelectKey(cSql, sizeof(cWk), cWk) ) return;

	//// データセット
	for(int ii=5-1; ii>0; ii--) {
		memcpy(&inf->ngInf[ii], &inf->ngInf[ii-1], sizeof(TYP_STATUS_INF));
	}

	//// 先頭のメッセージのみ変更
	memcpy(&inf->ngInf[0].msg, cWk, sizeof(cWk));
	if( 5 > inf->nCnt ) inf->nCnt++;

//▲	LOG(em_ERR), "[KikiManager] %s 完了", inf->ngInf[0].msg);
}

// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ステータス変更関係

//------------------------------------------
// DB_SYSTEM_STATEに機器状態をセットする
// int id 機器ID
// int state 機器状態 (0:正常  1～:異常)
// bool bLogDsp シスログ出力の有無
// char const * cMsg 追加メッセージ
//------------------------------------------
void KikiManager::SetStatusQue(char const * cKey, int state, bool bLogDsp, char const * cMsg)
{
	int id = GetPrivateProfileInt(INI_STATUS, cKey, 0, TASKINI_NAME);
	SetStatusQue(id, state, bLogDsp, cMsg );
}
void KikiManager::SetStatusQue(char const * cKey, bool state, bool bLogDsp, char const * cMsg)
{
	int id = GetPrivateProfileInt(INI_STATUS, cKey, 0, TASKINI_NAME);
	SetStatusQue(id, (state ? 0 : 1), bLogDsp, cMsg );
}
void KikiManager::SetStatusQue(int id, bool state, bool bLogDsp, char const * cMsg)
{
	SetStatusQue(id, (state ? 0 : 1), bLogDsp, cMsg );
}
void KikiManager::SetStatusQue(int id, int state, bool bLogDsp, char const * cMsg)
{
	_ASSERT(0!=id);

	// データセット
	DELIVERYSTATUS* pDeli = new DELIVERYSTATUS;
	pDeli->id = id;
	pDeli->state = state;
	pDeli->bLogDsp = bLogDsp;
	pDeli->strMsg = cMsg;

	// 登録
	if( ! mque_Deli.AddToTailFreeCheck(pDeli) ) {
		// 満タン
		delete pDeli;
		LOG(em_ERR), "[KikiManager] ステータス登録依頼キューフル(id:%d)(st:%d)[%s]", id, state, cMsg);
		syslog(SYSNO_QUEFULL_WAR, "[KikiManager](id:%d)(st:%d)", id, state);
	}
}

//------------------------------------------
// ステータス 非同期書き込み
// DELIVERYSTATUS * pDeli 受け渡し構造体ポインタ
//------------------------------------------
bool KikiManager::SetStatusThread(DELIVERYSTATUS* pDeli)
{
	CString cSql;											// SQL文

	// 接続確認
	if( ! ConnectDB(false) ) {
		LOG(em_ERR), "[KikiManager] DB接続失敗！！");
		return false;					// DB接続できない為、辞め
	}

	//// SQL実行
	cSql.Format( "UPDATE %s SET 機器状態=%d WHERE 機器ID=%d", DB_STATUS_INF, pDeli->state, pDeli->id );
/*
	if( ! mcls_pDB->ExecuteDirect(cSql)) {
		LOG(em_ERR), "[KikiManager] ステータス変更 ExecuteDirectエラー");
		syslog(SYSNO_DB_EXECUTEDIRECT_ERR,"[KikiManager ステータス変更]");
		CloseDB();
		return false;
	}
*/
	bool bRetc = mcls_pDB->ExecuteDirect(cSql);
	if( ! bRetc ) {
		LOG(em_WAR), "[KikiManager] 非同期ステータス変更失敗！！即時リトライ開始");
		CloseDB();
		if( ! ConnectDB(false) ) {
			LOG(em_ERR), "[KikiManager] DB接続失敗！！");
			return false;					// DB接続できない為、辞め
		}
		bRetc = mcls_pDB->ExecuteDirect(cSql);
	}
	if( ! bRetc ) {
		LOG(em_ERR), "[KikiManager] 非同期ステータス変更 ExecuteDirectエラー");
		syslog(SYSNO_DB_EXECUTEDIRECT_ERR,"[KikiManager ステータス変更]");
		CloseDB();
		return false;
	}


	//// コミット
	mcls_pDB->TranCommit();
/*
	//// 現在のメモリにも反映しておく。	要検討！！
	// こうしておくことで、即時反映を保障しておく。
	if(0 < pDeli->id && MAX_STATUS > pDeli->id) {
		m_typStatInf[pDeli->id-1].stat = 
	}
*/

	//// シスログ出力
	if(pDeli->bLogDsp) {
		if(0==pDeli->state) statelog(SYSNO_STATUS_OK, pDeli->id, pDeli->state, pDeli->strMsg );
		else				statelog(SYSNO_STATUS_NG, pDeli->id, pDeli->state, pDeli->strMsg );
	}

	return true;
}
