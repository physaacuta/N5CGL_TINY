#include "StdAfx.h"
#include "StatusManager.h"

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
StatusManager::StatusManager(void) :
ThreadManager("StatusMgr"),
// ステータス初期化
m_emMainSt(STATE_NON)
{
	for(int ii=0; ii<MAX_INS; ii++) {
		//m_emSysSt[ii]		= EM_SYS_STATE::SYS_STATE_NON;
		m_bFrameSync[ii]	= false;
		m_nFrameSyncNo[ii]	= 0;
	}
	// 初期化
	InitializeCriticalSection(&m_csLock);						// クリティカルセクションオブジェクトを初期化
	m_evReFrameSync			= CreateEvent(NULL, FALSE, FALSE, NULL);
	m_evReFrameSyncAuto		= CreateEvent(NULL, FALSE, FALSE, NULL);
}

//------------------------------------------
// デストラクタ
//------------------------------------------
StatusManager::~StatusManager(void)
{
	//// 開放
	DeleteCriticalSection(&m_csLock);							// クリティカルセクションオブジェクトを開放
	CloseHandle(m_evReFrameSync);
	CloseHandle(m_evReFrameSyncAuto);
}

// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// スレッド

//------------------------------------------
// スレッド開始前処理 (スレッド実行直後(シグナルセット前)にコールバック)
//------------------------------------------
int StatusManager::ThreadFirst()
{
	//// イベント設定
	HANDLE hArray[] = { m_evReFrameSync,
						m_evReFrameSyncAuto
						};
	SetEventNum(sizeof(hArray)/sizeof(HANDLE), hArray);
	return 0;
}

//------------------------------------------
// スレッドイベント発生
// int nEventNo イベントNo (0オリジン)
//------------------------------------------
void StatusManager::ThreadEvent(int nEventNo)
{
	////// シグナルの条件		
	enum {	EV_RE_FRAME_SYNC = 0,								// 再同期
			EV_RE_FRAME_SYNC_AUTO,								// 再同期 (ズレ時のみ)
			EV_END	};

	////// シグナル条件分け
	switch (nEventNo) {

//-----------------------------------------------------------------------------------------------
	case EV_RE_FRAME_SYNC:									// 再同期
		//▲ReFrameSync();
		break;

	case EV_RE_FRAME_SYNC_AUTO:								// 再同期 (ズレ時のみ)
		// 実行
		//if( ! GetFrameSyncAllSt( mcls_pParam->GetMain().nIns ) ) ReFrameSync();
		break;

//-----------------------------------------------------------------------------------------------
	default :
		// ありえない！！
		break;
	}
}



// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ステータス制御

//------------------------------------------
// 指定のステータス名称取得
// EM_MAIN_STATE st ステータス
//------------------------------------------
CString StatusManager::GetMainStName(EM_MAIN_STATE st) const
{
	CString wk;
	if		(st == STATE_NON)		{ wk = "初期化中"; }
	else if (st == STATE_INIT)		{ wk = "初期状態"; }
	else if (st == STATE_RUN)		{ wk = "検査中";}
//	else if (st == STATE_CANNOT)	{ wk = "続行不可能";}
	else							{ wk.Format("ステータス異常 <%d>", st); }
	return wk;
}

//------------------------------------------
// ステータス変更
// EM_MAIN_STATE st ステータス
//------------------------------------------
bool StatusManager::SetMainSt(EM_MAIN_STATE st)
{
	////// メインインスタンス取り出し
	EM_MAIN_STATE	old;
	old = GetMainSt();
	if( old == st ) {
		LOG(em_ERR), "ステータス変更NG [%s] → [%s]", GetMainStName(old), GetMainStName(st)); 
		return false;
	}

	//// クリティカルセクション突入
	AutoLock lock(&m_csLock);

	//// 変更ステータスにより処理分け
	m_emMainSt = st;
	//syslog(102, "[%s] ⇒ [%s]", GetMainStName(old), GetMainStName(st)); 

	//// 画面状態変更
	LOG(em_MSG), "============= <<ST変更>> [%s] ⇒ [%s] =============", GetMainStName(old), GetMainStName(st)); 
	return true;
}



// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// フレームズレ

//------------------------------------------
// フレームズレ状態取得
// int nInsNum インスタンス数
// 戻り値 全部正常時 true
//------------------------------------------
bool StatusManager::GetFrameSyncAllSt(int nInsNum)
{
	for(int ii=0; ii<nInsNum; ii++) {
		if( ! m_bFrameSync[ii]) return false;	// ズレ有り
	}
	return true;	// ズレ無し
}

//------------------------------------------
// フレームズレ状態変更
// int nIns Ins番号 (0オリジン)
// bool bMode true:同期  false:ズレ
//------------------------------------------
void StatusManager::SetFrameSyncSt(int nIns, bool bMode)
{
	if( m_bFrameSync[nIns] == bMode) return;

	m_bFrameSync[nIns] = bMode;
	if(bMode) {
		LOG(em_MSG), "============= <<フレーム同期完了>> [Ins=%d] =============", nIns); 
	} else {
		LOG(em_ERR), "============= <<フレームズレ発生>> [Ins=%d] =============", nIns); 
	}
}
//------------------------------------------
// フレームズレ状態リセット
//------------------------------------------
void StatusManager::ResetFrameSyncAllSt()
{
	for(int ii=0; ii<MAX_INS; ii++) {
		m_bFrameSync[ii] = false;
		m_nFrameSyncNo[ii] = 0;
	}
}
void StatusManager::ResetFrameSyncSt(int nInsNum, int nFno)
{
	m_bFrameSync[nInsNum] = false;
	m_nFrameSyncNo[nInsNum] = nFno;	// 再同期フレームNo
}

//------------------------------------------
// ソフト強制再起動
//------------------------------------------
void StatusManager::ResetAp()
{
	LOG(em_ERR), "============= ソフト強制再起動 ============="); 
	// ソフト強制リセット処理

	COMMON_QUE que;
	que.nLineNo    = getlineid();
	que.nEventNo = FACT_TI_RPISAVE_02;
	send_mail(getmytaskname(), ".", &que);

	que.nEventNo = FACT_TI_RPISAVE_01;
	send_mail(getmytaskname(), ".", &que);
}
