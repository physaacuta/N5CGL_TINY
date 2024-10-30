#include "StdAfx.h"
#include "DMemManager.h"

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
DMemManager::DMemManager(void) :
m_bIsConnect(false),
m_bIsStatus(false),
m_pCom(NULL),
mcls_pLog(NULL)
{
//*/----> ◆
	m_hDataMem = CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, sizeof(TO_SEQMEM_TBL), TBL_TO_SEQMEM_NAME);
	mtbl_pDM =(TO_SEQMEM_TBL*) MapViewOfFile(m_hDataMem, FILE_MAP_ALL_ACCESS, 0, 0, 0);
//*///<<---

	//// 共有メモリ
	// データメモリ情報テーブル
	mem_connect(&m_hDataMem, (LPVOID *)&mtbl_pDM, TBL_TO_SEQMEM_NAME);
	if(m_hDataMem == NULL) {
		syslog(SYSNO_MEM_OPEN, "データメモリ情報テーブル アクセス失敗");
		_ASSERT(FALSE);
	}

	//// iniファイルデータ取得
	GetPrivateProfileString(TO_SEQDM, "READADDR", "", m_cAdderssRead, sizeof(m_cAdderssRead), TASKINI_NAME);
	if ( 0 == strlen( m_cAdderssRead )) {
		_ASSERT(FALSE);
	}
	GetPrivateProfileString(TO_SEQDM, "WRITEADDR", "", m_cAdderssWrite, sizeof(m_cAdderssWrite), TASKINI_NAME);
	if ( 0 == strlen( m_cAdderssWrite )) {
		_ASSERT(FALSE);
	}
}

//------------------------------------------
// デストラクタ
//------------------------------------------
DMemManager::~DMemManager(void)
{
	//// 共有メモリ開放
	mem_close(&m_hDataMem, (LPVOID *)&mtbl_pDM);				// データメモリ情報
}

//------------------------------------------
// スレッド
//------------------------------------------
UINT WINAPI DMemManager::MainThread(LPVOID param)
{
	////// メインインスタンス取り出し
	DMemManager *p = (DMemManager *)param;				// 自クラスのインスタンスを取得
	p->SetTid();										// 自スレッドIDセット


	////////////////////////////////////
	// 事前準備
	////////////////////////////////////
	__int64 ts = 0;	// 1μ秒→100ナノ秒 (マイナス指定で、指定時間経過でシグナル) (第3引数は2回目以降の起動間隔)
	// 実行処理用
	HANDLE evTimerExec = CreateWaitableTimer(NULL, FALSE, NULL);
	SetWaitableTimer(evTimerExec, (LARGE_INTEGER *)&ts, TIME_INTERVAL_EXEC, NULL, NULL, FALSE);


	////////////////////////////////////
	// 通常のメインスレッド処理
	////////////////////////////////////

	// WaitForMultipleObjectsイベント
	HANDLE	hArray[] = {p->my_evStop,
						evTimerExec,
						};
	enum {	EV_STOP = WAIT_OBJECT_0,		// スレッド終了
			EV_TIMER_EXEC,					// 処理実行 定周期タイマ
			EV_END	};


	// メッセージキュー読み込み
	pLOG(em_MSG), "[DMemManager] スレッド開始 [0x%X]", p->GetTid());
	while (1) {
		DWORD ret = WaitForMultipleObjects(EV_END, hArray, FALSE, INFINITE);

		switch (ret) {

//-----------------------------------------------------------------------------------------------
		case EV_STOP:												// スレッド終了
			pLOG(em_MSG), "[DMemManager] スレッド終了 [0x%X]", p->GetTid());
			CancelWaitableTimer(evTimerExec);
			CloseHandle(evTimerExec);
			return 0;


//-----------------------------------------------------------------------------------------------
		case EV_TIMER_EXEC:											// 処理実行 定周期タイマ
			// オープンしている場合のみ
			if( ! p->m_bIsConnect ) break; 

			// 読み書き実行
			p->Exec_ReadWrite();
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
// データの読込と書込み処理
//------------------------------------------
void DMemManager::Exec_ReadWrite()
{
	//// 読み込み処理
	// シーケンサデータ取得
	if( MxRead() ) {
		// シーケンサーデータが正常に読み込めたら共有メモリに書き込む
		memcpy( &mtbl_pDM->fb_dm[DM_RECV_REC], m_cDMBuf[DM_RECV_REC], DM_DATA_MAX);
	}


	//// 書き込み処理
	// 共有メモリデータ取得
	memcpy( m_cDMBuf[DM_SEND_REC], &mtbl_pDM->fb_dm[DM_SEND_REC], DM_DATA_MAX);
	// シーケンサデータ書込み
	MxWrite();

	return;
}


//------------------------------------------
// データメモリ データ取得
//------------------------------------------
bool DMemManager::MxRead()
{
	if(NULL == m_pCom) return false;

	// データ読み込み (ワード単位なので注意)
	long lRet = m_pCom->ReadDeviceBlock2((LPCTSTR)m_cAdderssRead, DM_DATA_MAX/sizeof(short), (short*)&m_cDMBuf[DM_RECV_REC]);         // メモリ上にシーケンサデータを読み込んでいる
	if(m_bIsStatus && 0 != lRet) {
		// 正常→異常となる場合のみ 念の為に再チェック
		LOG(em_WAR), "[DMemManager] データメモリ読込エラー?? [lRet = %X]", lRet);
		lRet = m_pCom->ReadDeviceBlock2((LPCTSTR)m_cAdderssRead, DM_DATA_MAX/sizeof(short), (short*)&m_cDMBuf[DM_RECV_REC]);         // メモリ上にシーケンサデータを読み込んでいる
	}

	if( 0 != lRet ) {
		if( m_bIsStatus ) {                                                    // 読込エラーの場合に設定
			m_bIsStatus = false;
			syslog(341, "[errcode = %X]", lRet);
			LOG(em_ERR), "[DMemManager] データメモリ読込エラー [lRet = %X]", lRet);
			KizuFunction::SetStatus("DATAMEM_SEQ_ID", false);
		}
		return false;

	} else {
		if( !m_bIsStatus ) {                                                   // 前回読込エラーの場合で読み込めた場合は状態を復帰
			m_bIsStatus = true;
			syslog(343, "");
			LOG(em_MSG), "[DMemManager] データメモリ読込復旧");
			KizuFunction::SetStatus("DATAMEM_SEQ_ID", true);
		}
		return true;
	}
}

//------------------------------------------
// データメモリ データ書込
//------------------------------------------
bool DMemManager::MxWrite()
{
	if(NULL == m_pCom) return false;

	// データ読み込み (ワード単位なので注意)
	long lRet = m_pCom->WriteDeviceBlock2((LPCTSTR)m_cAdderssWrite, DM_DATA_MAX/sizeof(short), (short*)&m_cDMBuf[DM_SEND_REC]);    // MXコンポーネントに書込みを行っている
	if(m_bIsStatus && 0 != lRet) {
		// 正常→異常となる場合のみ 念の為に再チェック
		LOG(em_WAR), "[DMemManager] データメモリ書込エラー?? [lRet = %X]", lRet);
		lRet = m_pCom->WriteDeviceBlock2((LPCTSTR)m_cAdderssWrite, DM_DATA_MAX/sizeof(short), (short*)&m_cDMBuf[DM_SEND_REC]);        // メモリ上にシーケンサデータを読み込んでいる
	}	
	
	if( 0 != lRet ) {
		if( m_bIsStatus ) {
			m_bIsStatus = false;
			syslog(342, "[errcode = %X]", lRet);
			LOG(em_ERR), "[DMemManager] データメモリ書込エラー [lRet = %X]", lRet);
			KizuFunction::SetStatus("DATAMEM_SEQ_ID", false);
		}
		return false;

	} else {
		if( !m_bIsStatus ) {
			m_bIsStatus = true;
			syslog(344, "");
			LOG(em_MSG), "[DMemManager] データメモリ書込復旧");
			KizuFunction::SetStatus("DATAMEM_SEQ_ID", true);
		}
		return true;
	}	
}
