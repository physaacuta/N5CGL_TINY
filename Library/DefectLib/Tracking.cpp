// Tracking.cpp: Tracking クラスのインプリメンテーション
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Tracking.h"

//////////////////////////////////////////////////////////////////////
// 構築/消滅
//////////////////////////////////////////////////////////////////////
//------------------------------------------
// コンストラクタ
// DWORD dist 何mm後に通知? [mm] (固定長ならこれだけでよい。可変長なら、トラッキング開始前にChangeDistで距離をセットしなおすこと)
// const double* length チェックする距離ポインタ [mm] (通常は、カウンターボードで管理している距離のアドレスを渡す)
// const LPHANDLE evEnd 指定距離経過後 ONするハンドルポインタ
// const LPHANDLE evCancel 途中中断した場合に ONするハンドルポインタ
//------------------------------------------
Tracking::Tracking(DWORD dist, const double* length, const LPHANDLE evEnd, const LPHANDLE evCancel) :
m_nDist(dist),
m_dLength(length),
m_evTrackingEnd(evEnd),
m_evTrackingCancel(evCancel),
g_pAddr(NULL),
m_bTkRun(false),
m_dChangeLengthMax(0)
{
	// 初期化
	m_dDistNow			= -1;
	m_evTkCancelStop	= CreateEvent(NULL, FALSE, FALSE, NULL);
	m_evTkTimer			= CreateWaitableTimer(NULL,FALSE, NULL);
}

//------------------------------------------
// デストラクタ
//------------------------------------------
Tracking::~Tracking()
{
	Stop();		// 実行中の場合 一度とめる

	CloseHandle(m_evTkCancelStop);
	CancelWaitableTimer(m_evTkTimer);
	CloseHandle(m_evTkTimer);
}

//------------------------------------------
// トラッキングを開始
//------------------------------------------
BOOL Tracking::TrackingStart()
{
	if(m_bTkRun) return FALSE;

	// 初期化
	m_dDistNow = *m_dLength;
	m_bTkRun = true;
	m_dChangeLengthMax = 0;


	// 念の為に登録済みのシグナルを全てキャンセル
	if(NULL != m_evTrackingEnd)		ResetEvent(*m_evTrackingEnd);
	if(NULL != m_evTrackingCancel)	ResetEvent(*m_evTrackingCancel);


	// トラッキングタイマー開始
	__int64 ts = 0;	// 1[ms]→100[ns] (マイナス指定で、指定時間経過でシグナル) (第3引数は2回目以降の起動間隔)
	SetWaitableTimer(m_evTkTimer, (LARGE_INTEGER *)&ts, 20, NULL, NULL, FALSE);

	return TRUE;
}

//------------------------------------------
// トラッキングを強制停止 (トラッキングを停止。シグナル発行されない。=Exe停止時などに発行)
//------------------------------------------
void Tracking::TrackingStop()
{
	m_bTkRun = false;
	m_dChangeLengthMax = 0;

	// トラッキングタイマー停止
	CancelWaitableTimer(m_evTkTimer);

	// 念の為に登録済みのシグナルを全てキャンセル
	if(NULL != m_evTrackingEnd)		ResetEvent(*m_evTrackingEnd);
	if(NULL != m_evTrackingCancel)	ResetEvent(*m_evTrackingCancel);
}

//------------------------------------------
// スレッド
//------------------------------------------
UINT WINAPI Tracking::MainThread(LPVOID param)
{
	////// メインインスタンス取り出し
	Tracking *p = (Tracking *)param;
	p->SetTid();								// 自スレッドIDセット


	// WaitForMultipleObjectsイベント
	HANDLE	hArray[] = {p->my_evStop, 
						p->m_evTkCancelStop, 
						p->m_evTkTimer };
	enum {	EV_STOP = WAIT_OBJECT_0,		// スレッド終了
			EV_CANCEL,						// 強制停止
			EV_TIMER,						// 定周期タイマ
			EV_END};

	// メッセージキュー読み込み
	while (1) {
		DWORD ret = WaitForMultipleObjects(EV_END, hArray, FALSE, INFINITE);
		switch (ret) {

//-----------------------------------------------------------------------------------------------
		case EV_STOP:												// スレッド終了
			p->m_bTkRun = false;
			return 0;

//-----------------------------------------------------------------------------------------------
		case EV_CANCEL:												// 強制終了
			if( ! p->m_bTkRun) break;														// キャンセルかけた時に、直前にEndとなっていた場合対策

			if(NULL != p->m_evTrackingCancel) SetEvent(*p->m_evTrackingCancel);				// 呼び元へのイベント通知
			CancelWaitableTimer(p->m_evTkTimer);
			p->m_bTkRun = false;
			p->m_dChangeLengthMax = 0;
			break;

//-----------------------------------------------------------------------------------------------
		case EV_TIMER:												// タイマー
			if( ! p->m_bTkRun) break;														// 念の為

			// 通知t対象距離に達した場合に通知
			if( *p->m_dLength - (p->m_dDistNow - p->m_dChangeLengthMax) >= (double)p->m_nDist) {
				SetEvent(*p->m_evTrackingEnd);
				CancelWaitableTimer(p->m_evTkTimer);
				p->m_bTkRun = false;
				p->m_dChangeLengthMax = 0;
			}
			break;
		}
	}
}
