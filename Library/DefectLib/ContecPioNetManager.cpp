// ContecPioNetManager.cpp: ContecPioNetManager クラスのインプリメンテーション
//
//////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "ContecPioNetManager.h"
#ifndef  DEFECTLIB_CONTEC_PIO_NON		////// 完全無効化

// メモリーリーク検出 ----->>>
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif
// <<<------------------------

//////////////////////////////////////////////////////////////////////
// 構築/消滅
//////////////////////////////////////////////////////////////////////
//------------------------------------------
// コンストラクタ
//------------------------------------------
ContecPioNetManager::ContecPioNetManager() :
m_hDrv(-1),
m_bIsOpen(false),
ContecPioBaseManager()
{

}

//------------------------------------------
// デストラクタ
//------------------------------------------
ContecPioNetManager::~ContecPioNetManager()
{
	//// 領域開放
	Free();

}

//------------------------------------------
// 領域確保
// WORD port_num ポート数
// WORD kid 機器ID
// char const* dev_name ボード名称
//------------------------------------------
bool ContecPioNetManager::Alloc(WORD port_num, WORD kid, char const* dev_name)
{
	_ASSERT( port_num<=8 );
	m_nPortNum = port_num;
	m_nKID = kid;
	memset(m_dev_name, 0x00, sizeof(m_dev_name));
	strcpy(m_dev_name, dev_name);


	// new
	m_inpPortNo = new short[m_nPortNum];
	m_outPortNo = new short[m_nPortNum];
	m_bufInp	= new BYTE[m_nPortNum];
	m_bufOut	= new BYTE[m_nPortNum];
	m_bufWk		= new BYTE[m_nPortNum];
	m_pDI		= new BYTE[m_nPortNum];
	m_pDO		= new BYTE[m_nPortNum];
	
	// Set port
	for( WORD ii=0; ii<port_num; ii++ ) {
		m_inpPortNo[ii] = ii;
		m_outPortNo[ii] = ii;
	}


	// 信号初期化
	memset(m_pDI,		0x00, sizeof(BYTE)*m_nPortNum);
	memset(m_pDO,		0x00, sizeof(BYTE)*m_nPortNum);
	memset(m_bufInp,	0x00, sizeof(BYTE)*m_nPortNum);
	memset(m_bufOut,	0x00, sizeof(BYTE)*m_nPortNum);


	//// シミュレータ用準備
	m_sSmyuName.Format(SMYU_PIO_NET_TBL_NAME, dev_name );


#ifndef NO_HW
	return Open(true);

#endif
	return true;
}

//------------------------------------------
// 領域開放
//------------------------------------------
void ContecPioNetManager::Free()
{
#ifndef NO_HW
	Close();
#endif
	// 解放
	SAFE_DELETE_ARRAY(m_inpPortNo);
	SAFE_DELETE_ARRAY(m_outPortNo);
	SAFE_DELETE_ARRAY(m_bufInp);
	SAFE_DELETE_ARRAY(m_bufOut);
	SAFE_DELETE_ARRAY(m_bufWk);
	SAFE_DELETE_ARRAY(m_pDI);
	SAFE_DELETE_ARRAY(m_pDO);
}


//------------------------------------------
// 接続
// bool bDsp true時 ログ表示
//------------------------------------------
bool ContecPioNetManager::Open(bool bDsp)
{
	if(m_bIsOpen) return true;	

	// オープン
	long  nRet = DioInit(m_dev_name, &m_hDrv);
	if( 0 != nRet ) {
		m_hDrv = -1;
		m_bIsOpen = false;

		if(bDsp) {
			LOG(em_ERR), "[ContecPioNetManager] オープンエラー [err_code = %d]", nRet);
			syslog(SYSNO_PIO_ERR, "[OPEN err_code = %d]", nRet);
		}
		m_bStatusDi = FALSE;
		m_bStatusDo = FALSE;
		SetEvent(m_evDiErr);
		SetEvent(m_evDoErr);
		KizuFunction::SetStatus(m_nKID, false, bDsp, "[PIOボード オープンエラー ]");
		return false;
	} else {
		m_bIsOpen = true;
		LOG(em_MSG), "[ContecPioNetManager] オープン完了");
		KizuFunction::SetStatus(m_nKID, true, true);
	}


	//// デジタルフィルタ
	//nRet = DioSetDigitalFilter(m_hDrv, 13);
	//if( 0 != nRet ) {
	//	return false;
	//}

	return true;
}

//------------------------------------------
// クローズ
//------------------------------------------
bool ContecPioNetManager::Close()
{
	// Close process
	if( -1 != m_hDrv && m_bIsOpen) {
		// 終了する前に 出力している信号は 全部OFFする
		memset(m_pDO, 0x00, sizeof(BYTE)*m_nPortNum);		
		Output();
		// Close
		long  nRet = DioExit(m_hDrv);
		if( 0 != nRet ) {
			LOG(em_ERR), "[ContecPioNetManager] クローズエラー [err_code = %d]", nRet);
			syslog(SYSNO_PIO_ERR, "[CLOSE err_code = %d]", nRet);
			return false;
		} else {
			m_hDrv = -1;
		}
	}
	m_bIsOpen = false;
	return true;
}

//------------------------------------------
// スレッド
//------------------------------------------
UINT WINAPI ContecPioNetManager::MainThread(LPVOID param)
{
	int ii;
	bool bRefresh = false;

	////// メインインスタンス取り出し
	ContecPioNetManager *p = (ContecPioNetManager *)param;
	p->SetTid();								// 自スレッドIDセット

	//// タイマーセット
	HANDLE evTimer = CreateWaitableTimer(NULL, FALSE, NULL);
	__int64 ts = 0;	// 1μ秒→100ナノ秒 (マイナス指定で、指定時間経過でシグナル) (第3引数は2回目以降の起動間隔)
	SetWaitableTimer(evTimer, (LARGE_INTEGER *)&ts, mc_nReadInterval, NULL, NULL, FALSE);


	enum {	EV_STOP = WAIT_OBJECT_0,		// スレッド終了
			EV_OUTPUT,						// DO設定イベント
			EV_READ_START,					// 強制読込み
			EV_TIMER,						// 定周期読込イベント

			EV_DOPULS	};					// DOパルス出力タイマー


	HANDLE	hArray[EV_DOPULS+mc_nMaxDoPulsCount];
	hArray[0] = p->my_evStop;
	hArray[1] = p->m_evOutputEvt;
	hArray[2] = p->m_evLockReadStart;
	hArray[3] = evTimer;


	//// DOパルス出力用タイマーセット
	int nWaitNum = EV_DOPULS + p->m_nPulsDoNum;
	for(ii=0; ii<p->m_nPulsDoNum; ii++) hArray[EV_DOPULS+ii] = p->m_DoPulsArray[ii].evTimer;



	// メッセージキュー読み込み
	pLOG(em_MSG), "[ContecPioNetManager] スレッド開始 [0x%X]", p->GetTid());
	while (1) {
		DWORD ret = WaitForMultipleObjects(nWaitNum, hArray, FALSE, INFINITE);

		switch (ret) {

//-----------------------------------------------------------------------------------------------
		case EV_STOP:												// スレッド終了
			pLOG(em_MSG), "[ContecPioNetManager] スレッド終了 [0x%X]", p->GetTid());
			// タイマー破棄
			CancelWaitableTimer(evTimer);
			CloseHandle(evTimer);
			return 0;

//-----------------------------------------------------------------------------------------------
		case EV_OUTPUT:												// DO設定イベント
			p->Output();										
			break;


//-----------------------------------------------------------------------------------------------
		case EV_READ_START:											// 強制読込み
			bRefresh = true;
		case EV_TIMER:												// 定周期読込イベント

			if( p->m_bAuto ) {
				// バッファ初期化
				memset(p->m_bufInp, 0x00, sizeof(BYTE)*p->m_nPortNum);

				// 信号入力
				p->Input();

			}

			//// ステータスのアクセスの為、今回値を反映
			memcpy(p->m_bufWk, p->m_pDI,	sizeof(BYTE)*p->m_nPortNum);	// 前回値
			memcpy(p->m_pDI,   p->m_bufInp, sizeof(BYTE)*p->m_nPortNum);

			//// 信号状態をチェック
			for( ii=0; ii<(int)p->m_nCheckNum; ii++ ) {
				int retc = p->PioNowState(p->m_CheckArray[ii].port, p->m_CheckArray[ii].addr, p->m_bufInp, p->m_bufWk);
				// シグナル通知
				if( 1 == retc && NULL != p->m_CheckArray[ii].evSignOn  ) SetEvent(p->m_CheckArray[ii].evSignOn  );
				if(-1 == retc && NULL != p->m_CheckArray[ii].evSignOff ) SetEvent(p->m_CheckArray[ii].evSignOff );
			}
			
			////// ステータスのアクセスの為、今回値を反映	順番変更
			//memcpy(p->m_pDI, p->m_bufInp, sizeof(BYTE)*p->m_nPortNum);


			//// 強制読み込み完了シグナル
			if(bRefresh) SetEvent(p->m_evLockReadEnd);									// 読込み完了シグナルセット
			bRefresh = false;
			break;


//-----------------------------------------------------------------------------------------------
		case EV_DOPULS + 0:											// DOパルス出力タイマー
		case EV_DOPULS + 1:
		case EV_DOPULS + 2:
		case EV_DOPULS + 3:
		case EV_DOPULS + 4:
		case EV_DOPULS + 5:
		case EV_DOPULS + 6:
		case EV_DOPULS + 7:
		case EV_DOPULS + 8:
		case EV_DOPULS + 9:
		case EV_DOPULS +10:
		case EV_DOPULS +11:
		case EV_DOPULS +12:
		case EV_DOPULS +13:
		case EV_DOPULS +14:
		case EV_DOPULS +15:
			if(true) {
				// ON→OFF
				int index = ret - EV_DOPULS;
				p->m_DoPulsArray[index].bExec = false; 
				p->SetDO(p->m_DoPulsArray[index].port, p->m_DoPulsArray[index].addr, ! p->m_DoPulsArray[index].bOnOff, false ); 
				
				// 即時反映
				p->Output();
			}
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
// デジタルデータの入力
//------------------------------------------
BOOL ContecPioNetManager::Input()
{
	//// シミュレータ
	if(m_bSmyu) {	
		SmyuInput();
		return TRUE;
	}

	//// オンライン
	long nRet = 0;
#ifndef NO_HW
	if( ! Open(false) ) return FALSE;

	_ASSERT(-1 != m_hDrv);

	// デジタルデータの入力
	nRet = DioInpMultiByte(m_hDrv, m_inpPortNo, m_nPortNum, m_bufInp);
#endif

	if( 0 != nRet ){
		if( m_bStatusDi ) {
			m_bStatusDi = FALSE;
			Close();			// 一度切断

			SetEvent(m_evDiErr);
			LOG(em_ERR), "[ContecPioNetManager] DIエラー [err_code = %d]", nRet);
			syslog(SYSNO_PIO_ERR, "[DI err_code = %d]", nRet);
			KizuFunction::SetStatus(m_nKID, false, true, "[DI 入力エラー ]");
		}
		return FALSE;

	} else {
		if( !m_bStatusDi ) m_bStatusDi = TRUE;
	}
	return TRUE;
}
//------------------------------------------
// デジタルデータの出力
//------------------------------------------
BOOL ContecPioNetManager::Output()
{
	//// 出力バッファにセット
	memcpy(m_bufOut, m_pDO, m_nPortNum);

	//// シミュレータ
	if(m_bSmyu) {	
		SmyuOutput();
		return TRUE;
	}


	//// オンライン
	long  nRet = 0;
#ifndef NO_HW
	if( ! Open(false) ) return FALSE;

	_ASSERT(-1 != m_hDrv);

	// デジタルデータの出力
	nRet = DioOutMultiByte(m_hDrv, m_outPortNo, m_nPortNum, m_bufOut);
#endif



	if( 0 != nRet ){
		if( m_bStatusDo ) {
			m_bStatusDo = FALSE;
			Close();			// 一度切断

			SetEvent(m_evDoErr);
			LOG(em_ERR), "[ContecPioNetManager] DOエラー [err_code = %d]", nRet);
			syslog(SYSNO_PIO_ERR, "[DO err_code = %d]", nRet);
			KizuFunction::SetStatus(m_nKID, false, true, "[DO 出力エラー ]");
		}
		return FALSE;
	} else {
		if( !m_bStatusDo ) m_bStatusDo = true;
	}
	return TRUE;
}
#endif