#include "StdAfx.h"
#include "SmyuEvtManager.h"

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
SmyuEvtManager::SmyuEvtManager(void) :
SmyuBaseManager("SmyuEvtMgr", 0)
{
	//// メールスロット準備数
	m_nMailCnt = 0;
	for(int ii=0; ii<MAX_MAIL_WAIT_NUM; ii++) {
		mcls_pMail[ii]   = NULL;
	}

	//// PO準備
	memset(m_nDoBuf, 0x00, sizeof(m_nDoBuf));


	//// ハンドル生成
	m_evTimerExec  = CreateWaitableTimer(NULL, FALSE, NULL);
	for(int ii=0; ii<MAX_MAIL_WAIT_NUM; ii++) {
		m_evRecvMail[ii] = CreateEvent(NULL, FALSE, FALSE, NULL);
	}
}

//------------------------------------------
// デストラクタ
//------------------------------------------
SmyuEvtManager::~SmyuEvtManager(void)
{
	SmyuStop();

	//// ハンドル解放
	CloseHandle(m_evTimerExec);
	for(int ii=0; ii<MAX_MAIL_WAIT_NUM; ii++) {
		CloseHandle(m_evRecvMail[ii]);
	}
}

//------------------------------------------
// スレッド開始前処理 (スレッド実行直後(シグナルセット前)にコールバック)
//------------------------------------------
int SmyuEvtManager::ThreadFirst()
{
	//// イベント設定
	this->AddEventNum(1, &m_evTimerExec);
	this->AddEventNum(MAX_MAIL_WAIT_NUM, m_evRecvMail);
	return 0;
}

//------------------------------------------
// スレッド終了前処理 (終了シグナル後にコールバック)
//------------------------------------------
int SmyuEvtManager::ThreadLast()
{
	// 定周期停止
	CancelWaitableTimer(m_evTimerExec);
	return 0;
}

//------------------------------------------
// スレッドイベント発生
// int nEventNo イベントNo (0オリジン)
//------------------------------------------
void SmyuEvtManager::ThreadEvent(int nEventNo)
{
	////// シグナルの条件
	enum {	EV_TIMER_EXEC = 0,				// 処理実行 定周期タイマ
			EV_RECV_MAIL					// メールスロット受信
	};

	////// シグナル条件分け
	if(nEventNo == EV_TIMER_EXEC) {
		// イベント発生処理
		SmyuExec_Po();

		// 前回値としてデータセット
		GetPoNowData();

	} else if( nEventNo >= EV_RECV_MAIL && nEventNo < EV_RECV_MAIL+MAX_MAIL_WAIT_NUM) {
		int nIndex = nEventNo - EV_RECV_MAIL;

		// イベント発生処理
		SmyuExec_Mail(nIndex);

		// 非同期受信開始
		MailRecvStart(nIndex);
	}
}



// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 処理

//------------------------------------------
// シミュレータ スタート
//------------------------------------------
void SmyuEvtManager::SmyuStart()
{
	int ii, jj;

	//================================================
	// メールスロット受信の準備を行う
	m_nMailCnt = 0;
	for( ii=0; ii<(int)m_lstStory.size(); ii++ ) {
		if(DIV_KIND_MAIL == m_lstStory[ii]->emRaise ) {
			if(MAX_MAIL_WAIT_NUM <= m_nMailCnt) _ASSERT(FALSE);
			TYP_RAISE_EVT_MAIL* pD = (TYP_RAISE_EVT_MAIL*)m_lstStory[ii]->pRaise;


			// 既に同じものが登録済み？
			for(jj=0; jj<m_nMailCnt; jj++) {
				if( 0 == strcmp( m_RecvName[jj], pD->cTaskName) ) break;
			}
			if(jj != m_nMailCnt) continue;		// 既に登録済み


			// 準備
			mcls_pMail[m_nMailCnt] = new KizuLib::MailSlotManager();
			m_RecvName[m_nMailCnt] = pD->cTaskName;
			if( ! mcls_pMail[m_nMailCnt]->Create(m_RecvName[m_nMailCnt]) ) {
				_ASSERT(FALSE);
			}

			// 非同期受信開始
			MailRecvStart(m_nMailCnt);
			m_nMailCnt++;
		}
	}

	//================================================
	// PO変化
	// 前回値として、現在の値を取得
	GetPoNowData();

	// タイマー
	__int64 ts = TIME_EXEC_INTERVAL * -10000; 
	SetWaitableTimer(m_evTimerExec, (LARGE_INTEGER *)&ts, TIME_EXEC_INTERVAL, NULL, NULL, FALSE);
}

//------------------------------------------
// シミュレータ ストップ
//------------------------------------------
void SmyuEvtManager::SmyuStop()
{
	//================================================
	// メールスロット受信
	for(int ii=0; ii<MAX_MAIL_WAIT_NUM; ii++ ) {
		SAFE_DELETE( mcls_pMail[ii]);
		m_RecvName[ii] = "";
	}
	m_nMailCnt = 0;

	//================================================
	// PO変化
	CancelWaitableTimer(m_evTimerExec);
}

//------------------------------------------
// メールスロット 受信開始
// int nIndex 配列位置 
//------------------------------------------
void SmyuEvtManager::MailRecvStart(int nIndex)
{
	if( NULL == mcls_pMail[nIndex]) return;

	OVERLAPPED ovlRecv;
	memset(&ovlRecv, 0x00, sizeof(OVERLAPPED));

	ovlRecv.hEvent = m_evRecvMail[nIndex];
	mcls_pMail[nIndex]->Recv(&m_RecvBuf[nIndex], sizeof(COMMON_QUE), &ovlRecv);
}

//------------------------------------------
// Po現在値取得
//------------------------------------------
void SmyuEvtManager::GetPoNowData()
{
	for(int ii=0; ii<mcls_pPio->GetMaxBood(); ii++) {
		for(int jj=0; jj<SMYU_PIO_MAX_PORT; jj++) {
			for(int kk=0; kk<8; kk++) {
				m_nDoBuf[ii][jj][kk] = mcls_pPio->GetValPo(ii, jj, kk);
			}
		}
	}
}

//------------------------------------------
// シミュレータ 実行 (メールスロット受信)
// int nIndex 何個目のメールスロット配列 (0オリジン)
//------------------------------------------
void SmyuEvtManager::SmyuExec_Mail(int nIndex)
{
	if( NULL == mcls_pMail[nIndex]) return;

	//// 非同期受信情報取得
	OVERLAPPED ovlRecv;
	memset(&ovlRecv, 0x00, sizeof(OVERLAPPED));
	if( ! mcls_pMail[nIndex]->GetResult( &ovlRecv) ) return;


	//// 対象の発生条件部を特定
	for(int ii=0; ii<(int)m_lstStory.size(); ii++ ) {
		if(DIV_KIND_MAIL != m_lstStory[ii]->emRaise ) continue;

		TYP_RAISE_EVT_MAIL* pD = (TYP_RAISE_EVT_MAIL*)m_lstStory[ii]->pRaise;
 		if( 0 == strcmp( m_RecvName[nIndex], pD->cTaskName) &&  m_RecvBuf[nIndex].nEventNo == pD->nEventNo ) {
			
			// シナリオ 発生
			this->RaiseEvents(m_lstStory[ii]->emEvent, m_lstStory[ii]->nEventSub, (BYTE*)m_lstStory[ii]->pEvent);
		}
	}
}

//------------------------------------------
// シミュレータ 実行 (PO変化)
//------------------------------------------
void SmyuEvtManager::SmyuExec_Po()
{
	//// 対象の発生条件部を特定
	for(int ii=0; ii<(int)m_lstStory.size(); ii++ ) {
		if(DIV_KIND_PO != m_lstStory[ii]->emRaise ) continue;
	

		TYP_RAISE_EVT_PO* pD = (TYP_RAISE_EVT_PO*)m_lstStory[ii]->pRaise;
 		
		// 前回値と変化があった？
		if( m_nDoBuf[pD->nBood][pD->nPort][pD->nBit] != mcls_pPio->GetValPo(pD->nBood, pD->nPort, pD->nBit) ) {
			// 1:立上がり 0:立下り
			if( (!! pD->nMode) == mcls_pPio->GetValPo(pD->nBood, pD->nPort, pD->nBit)) {

				// シナリオ 発生
				this->RaiseEvents(m_lstStory[ii]->emEvent, m_lstStory[ii]->nEventSub, (BYTE*)m_lstStory[ii]->pEvent);
			}
		}
	}
}
