// *********************************************************************************
//	トラッキング制御クラス
//	[Ver]
//		Ver.01    2007/03/14  vs2005 対応
//		Ver.02	  2009/02/26  ワーカースレッドの毎回停止開始は、ヤメ。全面作り直し。
//
//	[メモ]
//		m_evTrackingEnd, m_evTrackingCancel が シグナルとなると、自動的にトラッキングが停止します。
// *********************************************************************************
#pragma once

// 疵検クラス インクルード
#include "..\KizuLib\Class\WorkThread.h"								// ワーカースレッドクラス

using namespace KizuLib;

class Tracking : public WorkThread
{

private:
	//// ローカル定数
	static const int PARAM_TIME_TRACKING		= 20;					// トラッキング周期読込処理実行 タイマ周期 [ms]

public:
	Tracking(DWORD dist, const double* length, const LPHANDLE evEnd, const LPHANDLE evCancel = NULL);	// コンストラクタ
	virtual ~Tracking();																				// デストラクタ

	void Start() { WorkThread::Start(MainThread); };					// スレッド開始
	void Stop()  { WorkThread::Stop(); };								// スレッド終了


	//// トラッキング操作
	BOOL TrackingStart();												// トラッキングを開始
	void TrackingStop();												// トラッキングを強制停止 (トラッキングを停止。シグナル発行されない。=Exe停止時などに発行)
	void TrackingCancel() { if(m_bTkRun)  SetEvent(m_evTkCancelStop); }	// トラッキングを中断 (中断シグナル発行後、トラッキング停止)


	//// その他もろもろ
	long*			g_pAddr;											// 受け渡し情報

	//// イベントシグナル
	HANDLE			GetEv_TrEnd()    const { return (const HANDLE)*m_evTrackingEnd; }		// トラッキング完了シグナル
	HANDLE			GetEv_TrCancel() const { return (const HANDLE)*m_evTrackingCancel; }	// トラッキング中断シグナル


	//// 外部アクセス
	// Get
	double			GetDist() const {return *m_dLength - (m_dDistNow - m_dChangeLengthMax );}	// 後何mm
	double			GetDistNow() const {return m_dDistNow;}				// 開始時の距離 mm
	double			GetLength() const {return *m_dLength;}				// 今何mm
	bool			IsRunning() const {return m_bTkRun;}				// 実行中？
	// Set
	void			ChangeDist(DWORD dist)	{ m_nDist = dist; }			// 後何mm 後に通知?を強制変更
	bool			TrackingLengthChange(double dOldLen) 				// コイル切替等が発生時
						{ 
							if(0.0 <= GetDist()) return false;
							m_dChangeLengthMax  = dOldLen; 
							return true;
						}


private:
	static UINT WINAPI MainThread(LPVOID param);						// メインスレッド

	
	// トラッキングに必要な者達
	long			m_nDist;											// 何mm後に通知? [mm]
	double			m_dDistNow;											// トラッキング開始時の距離 [mm]
	double			m_dChangeLengthMax;									// コイル切替等でチェック距離が0クリアされた場合のゲタはかせ分

	const double*	m_dLength;											// チェックする値 [mm]
	const HANDLE*	m_evTrackingEnd;									// 指定距離経過後 ONするハンドル
	const HANDLE*	m_evTrackingCancel;									// 途中中断した場合に ONするハンドル


	// 同期シグナル
	HANDLE			m_evTkCancelStop;									// トラッキングを中断 (中断シグナル発行後、トラッキング停止)
	HANDLE			m_evTkTimer;										// トラッキングタイマー

	// その他もろもろ
	bool			m_bTkRun;											// トラッキング実行中時 True
};
