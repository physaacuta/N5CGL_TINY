#include "StdAfx.h"
#include "CheckStatus.h"

//------------------------------------------
// コンストラクタ
// bool isMaster マスターPC管理時true
//------------------------------------------
CheckStatus::CheckStatus(bool isMaster) :
m_bIsMaster(isMaster),
mcls_pLog(NULL)
{
	//// 初期化
	InitializeCriticalSection(&m_csLock);							// クリティカルセクションオブジェクトを初期化
	m_nStateCount = 0;
	memset( m_typStateCheck, 0x00, sizeof(m_typStateCheck) );

	//// シグナル生成
	m_evHBeatCheck = CreateWaitableTimer(NULL, FALSE, NULL);
	m_evHBeatSend = CreateWaitableTimer(NULL, FALSE, NULL);

	//// iniファイルデータ取得
	Get_IniData();
}

//------------------------------------------
// デストラクタ
//------------------------------------------
CheckStatus::~CheckStatus(void)
{
	//// 開放
	DeleteCriticalSection(&m_csLock);								// クリティカルセクションオブジェクトを開放
	//// シグナル開放
	CloseHandle(m_evHBeatCheck);
	CloseHandle(m_evHBeatSend);
}

//------------------------------------------
// スレッド
//------------------------------------------
UINT WINAPI CheckStatus::MainThread(LPVOID param)
{
	////// メインインスタンス取り出し
	CheckStatus *p = (CheckStatus *)param;
	p->SetTid();								// 自スレッドIDセット

	// WaitForMultipleObjectsイベント
	HANDLE	hArray[] = {p->my_evStop,
						p->m_evHBeatCheck,
						p->m_evHBeatSend
						};
	enum {	EV_STOP = WAIT_OBJECT_0,		// スレッド終了
			EV_HB_CHECK,					// ハートビート監視タイマイベント
			EV_HB_SEND,						// ハートビート送信タイマイベント
			EV_END	};
	

	//// タイマ設定
	// ハートビートチェックタイマ
	if( p->m_bIsMaster || 0 != p->m_nMasterCheckID ) {
		LARGE_INTEGER ts;
		ts.QuadPart = (__int64)HEART_BEAT_CHECK_CYCLE * -10000;	// 1[ms]→100[ns] (マイナス指定で、指定時間経過でシグナル) (第3引数は2回目以降の起動間隔)
		SetWaitableTimer(p->m_evHBeatCheck, &ts, HEART_BEAT_CHECK_CYCLE, NULL, NULL, FALSE);		
	}

	//// ハートビート送信タイマ
	if( 0 != p->m_nSendHBID ) {
		LARGE_INTEGER ts;
		ts.QuadPart = (__int64)HEART_BEAT_SEND_CYCLE * -10000;	// 1[ms]→100[ns] (マイナス指定で、指定時間経過でシグナル) (第3引数は2回目以降の起動間隔)
		SetWaitableTimer(p->m_evHBeatSend, &ts, HEART_BEAT_SEND_CYCLE, NULL, NULL, FALSE);		
	}


	// メッセージキュー読み込み
	pLOG(em_MSG), "[CheckStatus] スレッド開始 [0x%X]", p->GetTid());
	while (1) {
		DWORD ret = WaitForMultipleObjects(EV_END, hArray, FALSE, INFINITE);

		switch (ret) {

//-----------------------------------------------------------------------------------------------
		case EV_STOP:												// スレッド終了
			pLOG(em_MSG), "[CheckStatus] スレッド終了 [0x%X]", p->GetTid());
			return 0;

//-----------------------------------------------------------------------------------------------
		case EV_HB_CHECK:											// ハートビート監視タイマイベント
			p->Check_HeatBeat();
			break;

//-----------------------------------------------------------------------------------------------
		case EV_HB_SEND:											// ハートビート送信タイマイベント
			p->Send_HeatBeat();
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
// データ確保
//------------------------------------------
void CheckStatus::Alloc()
{
	//// 機器状態テーブル初期化
	if(m_bIsMaster) Init_StatusInf();

	//// 機器IDを取得
	if(m_bIsMaster)					 Get_Status_ID(0);					// マスターPC管理
	else if( 0 != m_nMasterCheckID ) Get_Status_ID(m_nMasterCheckID);	// スレーブPC管理 且つ 統括ハートビート確認有り時
};

//------------------------------------------
// iniファイルデータ取得
//------------------------------------------
void CheckStatus::Get_IniData()
{
	CString cSession;
	cSession = m_bIsMaster ? KS_MASTER : KS_SLAVE;

	// ハートビート送信有無
	m_nSendHBID = 0;
	m_nSendHBID = GetPrivateProfileInt(cSession, "HB_SEND_ENABLE", 0, PCINI_NAME);

	// 統括ハートビート確認有無 (スレーブPC管理のみ)
	m_nMasterCheckID = 0;
	if( ! m_bIsMaster ) {
		m_nMasterCheckID = GetPrivateProfileInt(cSession, "HB_MASTER_CHECK", 0, PCINI_NAME);
	}
}

//------------------------------------------
// 機器状態テーブル初期化 (マスター時のみ)
//------------------------------------------
bool CheckStatus::Init_StatusInf()
{
	AutoLock autolock(&m_csLock);

	//// ODBC接続クラス インスタンス生成
	OdbcBase cls_DB; 
	if( !cls_DB.Connect(TASKINI_NAME)) {
		// データベース接続エラー (まずありえない)
		LOG(em_ERR), "[CheckStatus] 接続エラー");
		syslog(SYSNO_DB_CONNECT_ERR,"");
		return false;
	}

	//// 登録件数取得
	CString cSql;
	int nCnt;
	cSql.Format("SELECT COUNT(*) FROM %s ", DB_STATUS_MASTER);
	cls_DB.GetSelectKey(cSql, &nCnt); 

	//// 更新
	for( int ii=1; ii<=nCnt; ii++ ) {
/*
UPDATE STATUS_INF SET 機器状態=(select 初期値 from STATUS_MASTER where 機器ID=3)
where 機器ID=3 and 機器ID=(select 機器ID from STATUS_MASTER where 初期値 is not null and 機器ID=3)
*/
		cSql.Format("UPDATE %s SET 機器状態=(SELECT 初期値 FROM %s WHERE 機器ID=%d) WHERE 機器ID=(SELECT 機器ID FROM %s WHERE 初期値 IS NOT NULL AND 機器ID=%d)",
					DB_STATUS_INF, DB_STATUS_MASTER, ii, DB_STATUS_MASTER, ii );

		// 更新
		if( ! cls_DB.ExecuteDirect(cSql) ) {
			// データベース接続エラー (まずありえない)
			LOG(em_ERR), "[CheckStatus] UPDATEエラー [%s]", cSql);
			syslog(SYSNO_DB_EXECUTEDIRECT_ERR, "[%s]", DB_STATUS_INF);
		}
	}
	//// コミット
	cls_DB.TranCommit();
	//// データベース切断
	cls_DB.DisConnect();
	
	LOG(em_MSG), "[CheckStatus] 機器状態テーブル初期化完了");
	return true;
}

//------------------------------------------
// 機器IDを取得
// int mode モード (マスター時:0固定   スレーブ時：統括確認する該当機器ID。チェック無しの場合は、そもそもコールされない)
//------------------------------------------
bool CheckStatus::Get_Status_ID(int mode)
{
	AutoLock autolock(&m_csLock);

	//// ODBC接続クラス インスタンス生成
	OdbcBase cls_DB;
	if( !cls_DB.Connect(TASKINI_NAME)) {
		// データベース接続エラー (まずありえない)
		LOG(em_ERR), "[CheckStatus] 接続エラー");
		syslog(SYSNO_DB_CONNECT_ERR,"");
		return false;
	}

	//// ハートビートを行う機器を取得
	// SQL文生成
	CString cSql;
	cSql.Format("SELECT m.機器ID, m.機器名, i.機器状態, m.回線確認有無 FROM %s m, %s i WHERE m.機器ID=i.機器ID",
				DB_STATUS_MASTER, DB_STATUS_INF );
	if( 0 != mode ) {
		CString cWk;
		cWk.Format( " AND m.機器ID=%d", mode );
		cSql += cWk;
	} else {
	//	cSql += " AND m.回線確認有無=1";	// 回線確認有りのものしか必要無し	// やめ。機器名称の為だけだけど全部取得する
	}

	// 実行
	if( ! cls_DB.ExecuteDirect(cSql) ) {
		// データベース接続エラー (まずありえない)
		LOG(em_ERR), "[CheckStatus] SELECTエラー [%s]", cSql);
		syslog(SYSNO_DB_EXECUTEDIRECT_ERR, "[%s]", DB_STATUS_MASTER);
		return false;
	}
	
	// 取得
	int nCnt = 0;
	while(true) {
		int sqlrc = cls_DB.Fetch();
		if (sqlrc == KIZUODBC_FETCH_OK) {					// データ有り
			cls_DB.GetData(1, &m_typStateCheck[nCnt].id);
			cls_DB.GetData(2, &m_typStateCheck[nCnt].name, sizeof(m_typStateCheck[nCnt].name));
			cls_DB.GetData(3, &m_typStateCheck[nCnt].status);
			cls_DB.GetData(4, &m_typStateCheck[nCnt].nHb);
			m_typStateCheck[nCnt].statusHb = false;

			nCnt++;
		} else if( sqlrc == KIZUODBC_FETCH_NODATE ) {		// データ無し
			break;	
		} else {								// エラー
			syslog(SYSNO_DB_FETCH_ERR, "[%s]", DB_STATUS_MASTER);
			return false;
		}
	}
	m_nStateCount = nCnt;
	LOG(em_MSG), "[CheckStatus] 機器数=%d", m_nStateCount );

	//// 統括HBチェックの場合は、プログラム内でヘルシー確認するよフラグを立てる
	if(0 != mode && 1 <= nCnt ) {
		m_typStateCheck[0].nHb = 1;
	}

	//// データベース切断
	cls_DB.DisConnect();
	return true;
}

//------------------------------------------
// ステータスセット
// int id 機器ID
// bool status ステータス (true:正常)
//------------------------------------------
void CheckStatus::Set_StatusID(int id, bool status)
{
	// 該当機器IDがどの位置？
	for(int ii=0; ii<m_nStateCount; ii++) {
		if( m_typStateCheck[ii].id == id ) {
			// あった
			Set_StatusIndex(ii, status);
			return ;
		}
	}
	LOG(em_ERR), "[CheckStatus] 機器ID無し [機器ID=%d]", id);
}
void CheckStatus::Set_StatusIndex(int index, bool status)
{
	AutoLock autolock(&m_csLock);
	m_typStateCheck[index].status = status ? 0 : 1;
}

//------------------------------------------
// ハートビートが来たよフラグセット
// int id 機器ID
// bool status ステータス (true:正常)
//------------------------------------------
void CheckStatus::Set_HeatBeatID(int id, bool status)
{
	if( !m_bIsMaster && 0 == m_nMasterCheckID ) return;

	// 該当機器IDがどの位置？
	for(int ii=0; ii<m_nStateCount; ii++) {
		if( m_typStateCheck[ii].id == id ) {
			// あった
			if(1 != m_typStateCheck[ii].nHb) { LOG(em_ERR), "[CheckStatus] ハートビート確認無しのはずなのにハートビートが来た [機器ID=%d]", id); }

			Set_HeatBeatIndex(ii, status);
			return ;
		}
	}
	LOG(em_ERR), "[CheckStatus] ハートビートの機器ID無し [機器ID=%d]", id);
}

void CheckStatus::Set_HeatBeatIndex(int index, bool status)
{
	AutoLock autolock(&m_csLock);
	m_typStateCheck[index].statusHb = status;
}

//------------------------------------------
// ハートビートチェック
//------------------------------------------
void CheckStatus::Check_HeatBeat()
{
	// ハートビートの機器全部をチェック
	for(int ii=0; ii<m_nStateCount; ii++) {
		if(1 != m_typStateCheck[ii].nHb) continue;			// ハートビート確認無し

		if( m_typStateCheck[ii].statusHb ) {				// ハートビート来た
			// 現時点で異常？		
			if( 0 != m_typStateCheck[ii].status ) {
				// 正常復帰
				LOG(em_INF), "[CheckStatus] ハートビート正常復帰：%s", m_typStateCheck[ii].name );

				// 状態変更
				Set_StatusIndex(ii, true);
				KizuFunction::SetStatus(m_typStateCheck[ii].id, true, true, "[ハートビート]");
			}

		} else {									// ハートビート来てない
			// 現時点で正常？
			if( 0 == m_typStateCheck[ii].status ) {
				// 正常復帰
				LOG(em_WAR), "[CheckStatus] ハートビート異常検出：%s", m_typStateCheck[ii].name );
				
				// 状態変更
				Set_StatusIndex(ii, false);
				KizuFunction::SetStatus(m_typStateCheck[ii].id, false, true, "[ハートビート]");		
			}
		}

		// 共通
		Set_HeatBeatIndex(ii, false );
	}
}

//------------------------------------------
// ハートビート送信
//------------------------------------------
void CheckStatus::Send_HeatBeat()
{
	COMMON_QUE que;
	if(m_bIsMaster) {	// マスター → スレーブ
		que.nEventNo = FACT_KS_SLAVE_02;
		que.nLineNo = getlineid();															// ラインID
		que.fl.data[0] = m_nSendHBID;  
		int retc = send_mail( KS_SLAVE, "*", &que );		// 受け取り先がどれか知らない為、UDPで送信
		if( 0 != retc ) syslog(SYSNO_MAIL_SNED_ERR, "[%d:ハートビート通知 <err=%d>]", FACT_KS_SLAVE_02, retc);

	} else {				// スレーブ → マスター
		que.nEventNo = FACT_KS_MASTER_02;
		que.nLineNo = getlineid();															// ラインID
		que.fl.data[0] = m_nSendHBID;  
		int retc = send_mail( KS_MASTER, MASTER_PC_NAME, &que );
		if( 0 != retc ) syslog(SYSNO_MAIL_SNED_ERR, "[%d:ハートビート通知 <err=%d>]", FACT_KS_MASTER_02, retc);
	}
}

//------------------------------------------
// 機器IDから配列位置を取得する
// int id 機器ID
// 戻り値 配列位置 (0オリジン) -1:該当無し
//------------------------------------------
int CheckStatus::GetIndex(int id)
{
	for(int ii=0; ii<m_nStateCount; ii++) {
		if( m_typStateCheck[ii].id == id ) return ii;
	}
	return -1;
}
//------------------------------------------
// 該当の機器IDの機器名称を取得
// int id 機器ID
// 戻り値 機器名称
//------------------------------------------
char const * CheckStatus::GetName(int id)
{
	// 該当機器IDがどの位置？
	for(int ii=0; ii<m_nStateCount; ii++) {
		if( m_typStateCheck[ii].id == id ) {
			return m_typStateCheck[ii].name ;
		}
	}
	return NULL;
}
