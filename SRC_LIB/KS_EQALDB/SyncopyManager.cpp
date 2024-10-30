#include "StdAfx.h"
#include "SyncopyManager.h"

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
SyncopyManager::SyncopyManager(CString cSession, LogFileManager* pLog):
KizuSyncroManager(cSession, pLog)
{
	LOG(em_INF), "[SyncopyManager] [%s] 初期化開始 -------->>>>", my_cSession);

	//// 実績移動対象テーブル取り込み
	char    cWk[1024];
	CString strVal;
	memset( m_Tbl, 0x00, sizeof(m_Tbl));
	for(int ii=0; ii<PARAM_MAX_TABLE; ii++ ) {
		strVal.Format("T_%d", ii+1);
		GetPrivateProfileString(my_cSession, strVal, "", cWk, sizeof(cWk), TASKINI_NAME);		
	
		if( 0 < strlen(cWk) ) {
			strVal = cWk;
			
			// トークン分割
			int		rec = 0;
			int		curPos = 0;
			CString strToken = strVal.Tokenize(",", curPos);
			while ( strToken != "" ) {
				strToken.Trim(_T(" 	"));		// iniファイル設定文字の前後のスペース、タブを取り除く

				
				if( 0 == rec ) {
					strcpy(m_Tbl[ii].cTbl, strToken); 
				} else if( 1 == rec ) {
					m_Tbl[ii].nExecCnt = atoi(strToken); 

				} else {
					if( PARAM_MAX_KEYCNT <= m_Tbl[ii].nKeyCnt ) break;
					// 主キー指定
					if( 0 != strToken.GetLength() ) {
						strcpy(m_Tbl[ii].cKey[m_Tbl[ii].nKeyCnt], strToken); 
						m_Tbl[ii].nKeyCnt += 1;
					}
				}

				// 次へ
				strToken = strVal.Tokenize(",", curPos);
				rec ++;
			}

			if( 0 >= m_Tbl[ii].nKeyCnt || 0 >= m_Tbl[ii].nExecCnt ) {
				_ASSERT(false);			
				break;		// 分割したトークンが有効？
			}
			m_Tbl[ii].bEnable = true;
			LOG(em_INF), "[SyncopyManager] T_%d ･･･ %s, [%d=%s,%s,%s]", ii+1, m_Tbl[ii].cTbl, m_Tbl[ii].nKeyCnt, m_Tbl[ii].cKey[0], m_Tbl[ii].cKey[1], m_Tbl[ii].cKey[2] );
		}
	}
	LOG(em_INF), "[SyncopyManager] [%s] 初期化完了 <<<<--------", my_cSession);
}

//------------------------------------------
// デストラクタ
//------------------------------------------
SyncopyManager::~SyncopyManager(void)
{
}

//------------------------------------------
// スレッド
//------------------------------------------
UINT WINAPI SyncopyManager::MainThread(LPVOID param)
{
	////// メインインスタンス取り出し
	SyncopyManager *p = (SyncopyManager *)param;		// 自クラスのインスタンスを取得
	p->SetTid();										// 自スレッドIDセット


	////////////////////////////////////
	// 事前準備
	////////////////////////////////////
	//// 定周期タイマーセット
	HANDLE		evTimeCycle;										// ヘルシー		
	evTimeCycle	= CreateWaitableTimer(NULL, FALSE, NULL);	
	if(0 != p->my_nTimeFast) {
		__int64 ts = (__int64)p->my_nTimeFast * (__int64)-10000;	// 1μ秒→100ナノ秒 (マイナス指定で、指定時間経過でシグナル) (第3引数は2回目以降の起動間隔)
		SetWaitableTimer(evTimeCycle,   (LARGE_INTEGER *)&ts, p->my_nTimeInterval, NULL, NULL, FALSE);
	}

	////////////////////////////////////
	// 通常のメインスレッド処理
	////////////////////////////////////

	// WaitForMultipleObjectsイベント
	HANDLE	hArray[] = {p->my_evStop,
						evTimeCycle,
						p->my_evNowExecAtoB,
						p->my_evNowExecBtoA
						};
	enum {	EV_STOP = WAIT_OBJECT_0,		// スレッド終了
			EV_TIMECYCLE,					// 定周期タイマー
			EV_EXEC_AtoB,					// A→B マスター処理開始
			EV_EXEC_BtoA,					// B→A マスター処理開始
			EV_END	};

	// メッセージキュー読み込み
	pLOG(em_MSG), "[SyncopyManager] [%s] スレッド開始 [0x%X]", p->my_cSession, p->GetTid());
	while (1) {
		DWORD ret = WaitForMultipleObjects(EV_END, hArray, FALSE, INFINITE);

		switch (ret) {

//-----------------------------------------------------------------------------------------------
		case EV_STOP:												// スレッド終了
			pLOG(em_MSG), "[SyncopyManager] [%s] スレッド終了 [0x%X]", p->my_cSession, p->GetTid());
			CancelWaitableTimer(evTimeCycle);
			CloseHandle(evTimeCycle);
			return 0;

//-----------------------------------------------------------------------------------------------
		case EV_TIMECYCLE:											// 定周期タイマー
			pLOG(em_MSG), "[SyncopyManager] [%s] 実績同期開始 -------------->>>", p->my_cSession);
		
			p->Exec(true);
			p->Exec(false);

			pLOG(em_MSG), "[SyncopyManager] [%s] 実績同期完了 <<<--------------", p->my_cSession);
			break;

		case EV_EXEC_AtoB:											// A→B 処理開始
			pLOG(em_MSG), "[SyncopyManager] [%s] A→B 実績同期開始 -------------->>>", p->my_cSession);
		
			p->Exec(true);

			pLOG(em_MSG), "[SyncopyManager] [%s] A→B 実績同期完了 <<<--------------", p->my_cSession);
			break;


//-----------------------------------------------------------------------------------------------
		case EV_EXEC_BtoA:											// B→A 処理開始
			pLOG(em_WAR), "[SyncopyManager] [%s] B→A 実績同期開始 -------------->>>", p->my_cSession);
			
			p->Exec(false);

			pLOG(em_WAR), "[SyncopyManager] [%s] B→A 実績同期開始 <<<--------------", p->my_cSession);
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
// B系の実績をA系に移動させる
// bool bAtoB    true:A→B  false:B→A
//------------------------------------------
bool SyncopyManager::Exec(bool bAtoB)
{
	//// メインとサブが生きている？
	if( ! CheckDB_Connect() ) {
		LOG(em_WAR), "[SyncopyManager] [%s] 2DB接続失敗", my_cSession);
		return false;
	}


	//// DB接続
	OdbcBase clsDbA;
	OdbcBase clsDbB;
	this->SetOdbcBaseA(&clsDbA); 
	this->SetOdbcBaseB(&clsDbB); 

	//// 実績コピー実行
	for(int ii=0; ii<PARAM_MAX_TABLE; ii++) {
		if( m_Tbl[ii].bEnable ) {
			// 実績移動
			Syncopy_DB(bAtoB, m_Tbl[ii].cTbl, m_Tbl[ii].nExecCnt, m_Tbl[ii].nKeyCnt, (char const*)m_Tbl[ii].cKey );	
			if(my_bStopFlg) goto Ending;	 // 終了？

			// 待ち
			Sleep(1000);
		}

		if(my_bStopFlg) goto Ending;	 // 終了？
	}

Ending:
	//// DB接続開放
	this->SetOdbcBaseA(NULL); 
	this->SetOdbcBaseB(NULL); 
	return true;
}
