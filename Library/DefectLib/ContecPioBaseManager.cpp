// ContecPioBaseManager.cpp: ContecPioBaseManager クラスのインプリメンテーション
//
//////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "ContecPioBaseManager.h"
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
ContecPioBaseManager::ContecPioBaseManager() :
m_nPortNum(0),
m_inpPortNo(NULL),
m_outPortNo(NULL),
m_bufInp(NULL),
m_bufOut(NULL),
m_pDI(NULL),
m_pDO(NULL),
m_bStatusDi(TRUE),
m_bStatusDo(TRUE),
m_nCheckNum(0),
m_bAuto(true),
m_bSmyu(false),
mcls_pLog(NULL)
{
	//// 初期化
	memset(m_CheckArray, 0x00, sizeof(m_CheckArray));

	//// イベント生成
	m_evDiErr		= CreateEvent(NULL, FALSE, FALSE, NULL);
	m_evDoErr		= CreateEvent(NULL, FALSE, FALSE, NULL);
	m_evOutputEvt	= CreateEvent(NULL, FALSE, FALSE, NULL);
	m_evLockReadStart = CreateEvent(NULL, FALSE, FALSE, NULL);				// 強制読込み開始イベント
	m_evLockReadEnd	  = CreateEvent(NULL, FALSE, FALSE, NULL);				// 強制読込み完了イベント


	//// DOパルス出力用
	m_nPulsDoNum = 0;
	memset(m_DoPulsArray, 0x00, sizeof(m_DoPulsArray));

	//// シミュレータ用
	m_sSmyuName = "";
	m_hTblSmyu = NULL;
	mtbl_pSmyu = NULL; 
}

//------------------------------------------
// デストラクタ
//------------------------------------------
ContecPioBaseManager::~ContecPioBaseManager()
{
	//// 領域開放
	Free();

	//// イベント開放
	CloseHandle(m_evDiErr);
	CloseHandle(m_evDoErr);
	CloseHandle(m_evOutputEvt);
	CloseHandle(m_evLockReadStart);
	CloseHandle(m_evLockReadEnd);

	//// DOパルス出力用
	for(int ii=0; ii<mc_nMaxDoPulsCount; ii++) {
		if(NULL != m_DoPulsArray[ii].evTimer) {
			CancelWaitableTimer(m_DoPulsArray[ii].evTimer);
			CloseHandle(m_DoPulsArray[ii].evTimer);
		}
	}

	//// シミュレータ用
	SetSmyuMode(false);
}

/*
//------------------------------------------
// スレッド
//------------------------------------------
UINT WINAPI ContecPioBaseManager::MainThread(LPVOID param)
{
	int ii;
	bool bRefresh = false;

	////// メインインスタンス取り出し
	ContecPioBaseManager *p = (ContecPioBaseManager *)param;
	p->SetTid();								// 自スレッドIDセット

	//// タイマーセット
	HANDLE evTimer = CreateWaitableTimer(NULL, FALSE, NULL);
	__int64 ts = 0;	// 1μ秒→100ナノ秒 (マイナス指定で、指定時間経過でシグナル) (第3引数は2回目以降の起動間隔)
	SetWaitableTimer(evTimer, (LARGE_INTEGER *)&ts, mc_nReadInterval, NULL, NULL, FALSE);


//
//	// WaitForMultipleObjectsイベント
//	HANDLE	hArray[] = {p->my_evStop,
//						p->m_evOutputEvt,
//						evTimer 
//						};
//
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
	pLOG(em_MSG), "[ContecPioBaseManager] スレッド開始 [0x%X]", p->GetTid());
	while (1) {
		DWORD ret = WaitForMultipleObjects(nWaitNum, hArray, FALSE, INFINITE);

		switch (ret) {

//-----------------------------------------------------------------------------------------------
		case EV_STOP:												// スレッド終了
			pLOG(em_MSG), "[ContecPioBaseManager] スレッド終了 [0x%X]", p->GetTid());
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

			//// 信号状態をチェック
			for( ii=0; ii<(int)p->m_nCheckNum; ii++ ) {
				int retc = p->PioNowState(p->m_CheckArray[ii].port, p->m_CheckArray[ii].addr);
				// シグナル通知
				if( 1 == retc && NULL != p->m_CheckArray[ii].evSignOn  ) SetEvent(p->m_CheckArray[ii].evSignOn  );
				if(-1 == retc && NULL != p->m_CheckArray[ii].evSignOff ) SetEvent(p->m_CheckArray[ii].evSignOff );
			}
			
			//// ステータスのアクセスの為、今回値を反映
			memcpy(p->m_pDI, p->m_bufInp, sizeof(BYTE)*p->m_nPortNum);


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
*/
//------------------------------------------
// デジタル値更新
// int port チェックするポート配列位置 
// BYTE addr チェックするビット位置 (0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80 のいずれか)
// BYTE* now 今回値 
// BYTE* old 前回値
// 戻り値 変更状態 (9:変更無し(ON) -9:変更無し(OFF) 1:OFF→ON -1:ON→OFF)
//------------------------------------------
int ContecPioBaseManager::PioNowState(PIO_PORT port, PIO_BIT addr, BYTE* now, BYTE* old )
{
	if( (now[port] & addr) == addr ) {		// 今回 ON
		if( (old[port] & addr) == 0 ) {			// 前回 OFF
			old[port] |= addr;						// 値変更(指定ビットのみON)
			return 1;										// OFF→ON
		} else {
			return 9;										// ONのまま
		}
	} else {								// 今回 OFF
		if( (old[port] & addr) == addr ) {		// 前回 ON
			old[port] &= ~addr;						// 値変更(指定ビットのみOFF)
			return -1;										// ON→OFF
		} else {
			return -9;										// OFFのまま
		}
	}
}
//------------------------------------------
// PIO変化状態チェック
// int port チェックするポート配列位置 
// BYTE addr チェックするビット位置 (0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80 のいずれか)
// BYTE const* now 今回値 
// BYTE const* old 前回値
// 戻り値 変更状態 (9:変更無し(ON) -9:変更無し(OFF) 1:OFF→ON -1:ON→OFF)
//------------------------------------------
int ContecPioBaseManager::PioNowStateCheck(PIO_PORT port, PIO_BIT addr, BYTE const* now, BYTE const* old )
{
	if( (now[port] & addr) == addr ) {		// 今回 ON
		if( (old[port] & addr) == 0 ) {			// 前回 OFF
			return 1;										// OFF→ON
		} else {
			return 9;										// ONのまま
		}
	} else {									// 今回 OFF
		if( (old[port] & addr) == addr ) {		// 前回 ON
			return -1;										// ON→OFF
		} else {
			return -9;										// OFFのまま
		}
	}
}

//------------------------------------------
// チェック情報 設定
// DWORD port		チェックするポート配列位置 
// BYTE addr		チェックするビット位置 (0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80 のいずれか)
// HANDLE evSignOn	ON通知用シグナル
// HANDLE evSignOff OFF通知用シグナル
//------------------------------------------
void ContecPioBaseManager::SetCheckArray(PIO_PORT port, PIO_BIT addr, HANDLE evSignOn, HANDLE evSignOff) 
{
	m_CheckArray[m_nCheckNum].addr		 = addr;
	m_CheckArray[m_nCheckNum].port		 = port;
	m_CheckArray[m_nCheckNum].evSignOn	 = evSignOn;
	m_CheckArray[m_nCheckNum].evSignOff  = evSignOff;
	m_nCheckNum++;
}

//------------------------------------------
// DO 指定位置ビット ON/OFF
// int port			チェックするポート配列位置 
// BYTE addr		チェックするビット位置 (0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80 のいずれか)
// bool onoff		On / Off 設定
// bool evt			書込みを行う？ (true:実際にセット false:内部バッファの値にのみセット)
//------------------------------------------
void ContecPioBaseManager::SetDO(PIO_PORT port, PIO_BIT addr, bool onoff, bool evt)
{
	if( onoff )	m_pDO[port] |= addr;
	else		m_pDO[port] &= ~addr;
	if(evt) SetEvent(m_evOutputEvt);
}

//------------------------------------------
// DI 指定位置ビット ON/OFF
// int port			チェックするポート配列位置 
// BYTE addr		チェックするビット位置 (0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80 のいずれか)
// bool onoff		On / Off 設定
//------------------------------------------
void ContecPioBaseManager::SetDI(PIO_PORT port, PIO_BIT addr, bool onoff)
{
	if( onoff )	m_bufInp[port] |= addr;
	else		m_bufInp[port] &= ~addr;
}

//------------------------------------------
// 指定位置ビット を No に変換
// BYTE addr		チェックするビット位置 (0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80 のいずれか)
// 戻り値			0～7
//------------------------------------------
int ContecPioBaseManager::GetBitToIndex(BYTE addr)
{
	if		(BIT_0 == addr)	{ return 0; }
	else if (BIT_1 == addr)	{ return 1; }
	else if (BIT_2 == addr)	{ return 2; }
	else if (BIT_3 == addr)	{ return 3; }
	else if (BIT_4 == addr)	{ return 4; }
	else if (BIT_5 == addr)	{ return 5; }
	else if (BIT_6 == addr)	{ return 6; }
	else if (BIT_7 == addr)	{ return 7; }
	else					{ return 0; }	// 注意		
}
//------------------------------------------
// 指定位置No を ビット に変換
// int index			0～7
// 戻り値		チェックするビット位置 (0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80 のいずれか)			
//------------------------------------------
BYTE ContecPioBaseManager::GetIndexToBit(int index)
{
	if		(0 == index)	{ return BIT_0; }
	else if (1 == index)	{ return BIT_1; }
	else if (2 == index)	{ return BIT_2; }
	else if (3 == index)	{ return BIT_3; }
	else if (4 == index)	{ return BIT_4; }
	else if (5 == index)	{ return BIT_5; }
	else if (6 == index)	{ return BIT_6; }
	else if (7 == index)	{ return BIT_7; }
	else					{ return BIT_0; }	// 注意	
}


//------------------------------------------
// 強制更新 (読込み完了するまでブロックされる)
//------------------------------------------
void ContecPioBaseManager::GetDIRefresh()
{
	SetEvent(m_evLockReadStart);
	WaitForSingleObject(m_evLockReadEnd, 2000);
}


//////////////////////////////////////////////////////////////////////
// DOパルス

//------------------------------------------
// DOパルス設定(Start前に実行すること)
// DWORD port		チェックするポート配列位置 
// BYTE addr		チェックするビット位置 (0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80 のいずれか)
// bool bOnOff		セット時の上体 (true:OFF→ON→OFF  false:ON→OFF→ON)
//------------------------------------------
void ContecPioBaseManager::SetPulsDoSetting(PIO_PORT port, PIO_BIT addr, bool bOnOff)
{
	_ASSERT( mc_nMaxDoPulsCount > m_nPulsDoNum );

	m_DoPulsArray[m_nPulsDoNum].port  = port;
	m_DoPulsArray[m_nPulsDoNum].addr  = addr;
	m_DoPulsArray[m_nPulsDoNum].bOnOff = bOnOff;
	m_DoPulsArray[m_nPulsDoNum].bExec  = false;
	m_DoPulsArray[m_nPulsDoNum].evTimer = CreateWaitableTimer(NULL, FALSE, NULL);

	m_nPulsDoNum++;
}

//------------------------------------------
// DO 指定位置ビットを 指定時間パルス出力
// DWORD port		チェックするポート配列位置 
// BYTE addr		チェックするビット位置 (0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80 のいずれか)
// int nTime		セット時間 [ms]
//------------------------------------------
bool ContecPioBaseManager::SetPulsDO(PIO_PORT port, PIO_BIT addr, int nTime)
{
	__int64 ts = nTime * -10000;			// 1ミリ秒→100ナノ秒 (マイナス指定で、指定時間経過でシグナル) (第3引数は2回目以降の起動間隔)

	for(int ii=0; ii<m_nPulsDoNum; ii++) {
		if( m_DoPulsArray[ii].port == port && m_DoPulsArray[ii].addr == addr) {

			// ON
			m_DoPulsArray[ii].bExec = true; 
			SetDO(m_DoPulsArray[ii].port, m_DoPulsArray[ii].addr, m_DoPulsArray[ii].bOnOff, true);

			// OFFのタイマーセット
			SetWaitableTimer(m_DoPulsArray[ii].evTimer, (LARGE_INTEGER *)&ts, 0, NULL, NULL, FALSE);
			return true;
		}
	}
	return false;
}
//------------------------------------------
// DOパルス出力実行中？
// DWORD port		チェックするポート配列位置 
// BYTE addr		チェックするビット位置 (0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80 のいずれか)
//------------------------------------------
bool ContecPioBaseManager::IsPulsDO(PIO_PORT port, PIO_BIT addr)
{
	for(int ii=0; ii<m_nPulsDoNum; ii++) {
		if( m_DoPulsArray[ii].port == port && m_DoPulsArray[ii].addr == addr) {
			if( m_DoPulsArray[ii].bExec ) return true;
			else						  return false;
		}
	}
	return false;
}

//////////////////////////////////////////////////////////////////////
// シミュレーション
//------------------------------------------
// シミュレータモードの設定
// bool bMode True:シミュレーション時 false:通常
//------------------------------------------
bool ContecPioBaseManager::SetSmyuMode(bool bMode) 
{
	bool bRetc=true;
	//// シミュレーション時 
	if(bMode) {
		mem_connect(&m_hTblSmyu, (LPVOID *)&mtbl_pSmyu, m_sSmyuName);
		if(m_hTblSmyu == NULL) {
			LOG(em_ERR), "[ContecPioBaseManager] シミュレータ用共有メモリアクセス失敗 [%s]", m_sSmyuName);
			m_hTblSmyu = NULL;
			mtbl_pSmyu = NULL; 
			bMode = false;
			bRetc = false;
			goto ending;
		}

	} else {
		if(NULL != mtbl_pSmyu) {
			mem_close(&m_hTblSmyu, (LPVOID *)&mtbl_pSmyu);
		}
		m_hTblSmyu = NULL;
		mtbl_pSmyu = NULL; 
	}

	//// フラグの変更
ending:
	m_bSmyu = bMode;
	return bRetc;
}

//------------------------------------------
// デジタルデータの入力
//------------------------------------------
void ContecPioBaseManager::SmyuInput()
{
	//// 通常のPIO読み込み
	memcpy( m_bufInp, mtbl_pSmyu->nDi, sizeof(BYTE)*m_nPortNum);
}

//------------------------------------------
// デジタルデータの出力
//------------------------------------------
void ContecPioBaseManager::SmyuOutput()
{
	memcpy( mtbl_pSmyu->nDo, m_bufOut, sizeof(BYTE)*m_nPortNum);
}

#endif