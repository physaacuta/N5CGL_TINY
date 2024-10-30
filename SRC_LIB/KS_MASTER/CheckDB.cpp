#include "StdAfx.h"
#include "CheckDB.h"


//------------------------------------------
// コンストラクタ
// bool isMaster マスターPC管理時true
//------------------------------------------
CheckDB::CheckDB(bool isMaster) :
m_bIsMaster(isMaster),
m_bStateDB(false),
m_bFastExec(true),
mcls_pLog(NULL)
{
	//// シグナル生成
	m_evDBCheck = CreateWaitableTimer(NULL, FALSE, NULL);

	//// iniファイルデータ取得
	Get_IniData();
}

//------------------------------------------
// デストラクタ
//------------------------------------------
CheckDB::~CheckDB(void)
{
	//// シグナル開放
	CloseHandle(m_evDBCheck);
}

//------------------------------------------
// スレッド
//------------------------------------------
UINT WINAPI CheckDB::MainThread(LPVOID param)
{
	////// メインインスタンス取り出し
	CheckDB *p = (CheckDB *)param;
	p->SetTid();								// 自スレッドIDセット

	// WaitForMultipleObjectsイベント
	HANDLE	hArray[] = {p->my_evStop,
						p->m_evDBCheck
						};
	enum {	EV_STOP = WAIT_OBJECT_0,		// スレッド終了
			EV_DB_CHECK,					// DB監視タイマイベント
			EV_END	};
	

	//// タイマ設定
	// ハートビートチェックタイマ
	LARGE_INTEGER ts;
	ts.QuadPart = 0;	// 1[ms]→100[ns] (マイナス指定で、指定時間経過でシグナル) (第3引数は2回目以降の起動間隔)
	SetWaitableTimer(p->m_evDBCheck, &ts, HEART_BEAT_DBCHECK_CYCLE, NULL, NULL, FALSE);

	// メッセージキュー読み込み
	pLOG(em_MSG), "[CheckDB] スレッド開始 [0x%X]", p->GetTid());
	while (1) {
		DWORD ret = WaitForMultipleObjects(EV_END, hArray, FALSE, INFINITE);

		switch (ret) {

//-----------------------------------------------------------------------------------------------
		case EV_STOP:												// スレッド終了
			pLOG(em_MSG), "[CheckDB] スレッド終了 [0x%X]", p->GetTid());
			return 0;

//-----------------------------------------------------------------------------------------------
		case EV_DB_CHECK:											// DB監視タイマイベント
			p->Check_DataBase();
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
// iniファイルデータ取得
//------------------------------------------
void CheckDB::Get_IniData()
{
	CString cSession;
	cSession = m_bIsMaster ? KS_MASTER : KS_SLAVE;

	// DB定周期チェック有無
	int nWk;
	nWk = GetPrivateProfileInt(cSession, "DB_ENABLE", 0, PCINI_NAME);
	m_bCheckDB = nWk == 1 ? true : false;

	// 送信タスク取得
	CString cWk;
	m_nSendTaskCnt = 0;
	memset(m_cSendTaskName, 0x00, sizeof(m_cSendTaskName));
	for( int ii=0; ii<MAX_SEND_TASK; ii++ ) {
		cWk.Format("DB_SENDTASK%d", ii+1 );
		GetPrivateProfileString(cSession, cWk, "", m_cSendTaskName[ii], SIZE_NAME_TASK, PCINI_NAME);
		if( 0==strlen(m_cSendTaskName[ii]) ) break;
		m_nSendTaskCnt ++;
	}
}

//------------------------------------------
// DBチェック
//------------------------------------------
void CheckDB::Check_DataBase()
{
	COMMON_QUE que;
	OdbcBase cls_DB;									// ODBC接続クラス インスタンス生成

	if( m_bFastExec ) {												// 初期接続中
		// DB立ち上げ確認強化
		//if( cls_DB.OpenCheck( TASKINI_NAME ) ) {							// 接続
		if( OpenCheckEx() ) {

			//// 初期接続完了
			m_bStateDB = true;
			m_bFastExec = false;
			CancelWaitableTimer(m_evDBCheck);
			// 定周期DBチェック有無
			if( m_bCheckDB ) {
				// 定周期スレッド生成
				LARGE_INTEGER ts;
				ts.QuadPart = (__int64)HEART_BEAT_DBCHECK_CYCLE * -10000;	// 1[ms]→100[ns] (マイナス指定で、指定時間経過でシグナル) (第3引数は2回目以降の起動間隔)
				SetWaitableTimer(m_evDBCheck, &ts, 0, NULL, NULL, FALSE);		
			}
			// 接続完了通知
			PostThreadMessage(GetMainPostTid(), E_DEF_DB_FAST_CONNECT, 0, 0);
			LOG(em_MSG), "DB接続 完了");

		} else {															// 未接続
			//// 初期接続失敗
			LOG(em_WAR), "DB接続待ち･･･");
		}

	} else {														// DB定周期確認中
		
		// 登録タスクへ通知
//LOG(em_MSG), "DB接続開始 --->>");
		bool bState = cls_DB.OpenCheck( TASKINI_NAME );
		for(int ii=0; ii<=4; ii++) {
			if( m_bStateDB && ! bState ) {
				// 正常→異常となる場合のみ 念の為に再チェック
				LOG(em_WAR), "DB接続 異常?? %d回目開始", ii+1); 
				syslog(SYSNO_DB_CHECK_RETRY, "リトライ%d回目開始", ii+1);
				Sleep(100);
				bState = cls_DB.OpenCheck( TASKINI_NAME );
			}
		}
//LOG(em_MSG), "DB接続完了 <<---");


		que.nEventNo = FACT_KS_DB_STATEUS;
		que.nLineNo = getlineid();											// ラインID
		que.fl.data[0] = (bState ? 0 : 1 );								// 0:正常時
		for(int ii=0; ii<m_nSendTaskCnt; ii++) {
			send_mail( m_cSendTaskName[ii], "*", &que );
		}
		if( m_bStateDB != bState ) {
			m_bStateDB = bState;
			if(bState)	{ 
				// メインインスタンスに復旧
				LOG(em_MSG), "DB接続 正常復帰");
				PostThreadMessage(GetMainPostTid(), E_DEF_DB_NG_TO_OK, 0, 0);

			} else		{ 
				LOG(em_ERR), "DB接続 異常"); 
				syslog(SYSNO_DB_CONNECT_ERR, "");
			}
		}
		// 定周期スレッド生成
		LARGE_INTEGER ts;
		ts.QuadPart = (__int64)HEART_BEAT_DBCHECK_CYCLE * -10000;	// 1[ms]→100[ns] (マイナス指定で、指定時間経過でシグナル) (第3引数は2回目以降の起動間隔)
		SetWaitableTimer(m_evDBCheck, &ts, 0, NULL, NULL, FALSE);	
	}
}

//------------------------------------------
// DB接続確認
//------------------------------------------
bool CheckDB::OpenCheckEx()
{
	// どうも蓄積PC起動直後に オープンチェックしても インサートとかでたまに失敗となる為、Selectを発行してOKならホントに完了とする
	OdbcBase	cls_DB;
	CString		sql;
	int			nVal;

	// そもそも繋がっている？
	if( ! cls_DB.Connect( TASKINI_NAME ) ) return false;

	// Select確認
	LOG(em_MSG), "DB接続 確認完了。続けてSelect確認");
	sql.Format("SELECT ISNULL(MIN(機器ID), 0) FROM %s", DB_STATUS_MASTER); 
	if( ! cls_DB.GetSelectKey( sql, &nVal ) ) {
		LOG(em_ERR), "Selectで異常");
		cls_DB.DisConnect();
		return false;
	}
	if( 0 == nVal ) return true;		// データが無いのでヤメ

	// UPDATE確認
	LOG(em_MSG), "DB Select 確認完了。続けてUpdate確認");
	sql.Format("UPDATE %s SET 機器ID=%d WHERE 機器ID=%d", DB_STATUS_MASTER, nVal, nVal );
	if( ! cls_DB.ExecuteDirect(sql) ) {
		LOG(em_ERR), "UpDateで異常");
		cls_DB.DisConnect();
		return false;
	}

	// ここまで来たらまず間違いないはず・・・
	cls_DB.DisConnect();
	return true;
}