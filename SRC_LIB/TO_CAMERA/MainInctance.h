// *********************************************************************************
//	メイン管理スレッド
//	[Ver]
//		Ver.01    2007/01/30  vs2005 対応
//
//	[メモ]
//		MAX_CAMPAIR と NUM_CAMERA の数を変えるだけで、
//			最小 片面のみ1台の計1個 ～ 最大 両面で4台の計16個まで対応可能
// *********************************************************************************

#pragma once


// インクルード
#include <process.h>

// 疵検 インクルード
#include "..\..\include\KizuCommon.h"									// 疵検共通ヘッダー
#include "..\..\include\LineCommon.h"									// ライン共通ヘッダー
#include "..\..\Library\KizuLib\KizuLib.h"								// 疵検ライブラリヘッダー

// 疵検クラス インクルード
#include "..\..\Library\KizuLib\CLASS\KizuMacro.h"						// 疵検マクロ
#include "..\..\Library\KizuLib\CLASS\WorkThread.h"						// ワーカースレッド
#include "..\..\Library\KizuLibMFC\CLASS\LogFileManager.h"				// ログマネージャー

// 疵検ライブラリ インクルード

// 各ワーカースレッド
#include "CameraManager.h"

using namespace KizuLib;


class MainInctance : public WorkThread
{
public:
	MainInctance(void);
	virtual ~MainInctance(void);

	void Start() { SetEvent(m_evThStart); };							// スレッド開始
	void Stop()  { WorkThread::StopMSG(WM_USER_TH_END); };				// スレッド終了
	BOOL Init();														// 初期化処理
	void Exit();														// 終了処理
	void SetLogMgr(LogFileManager* pLog) { mcls_pLog = pLog; }			// ログファイルセット

	// 各ワーカースレッド
	CameraManager*			gcls_pCam[MAX_THREAD];						// カメラコントローラ通信クラス



private:
	static UINT WINAPI MainThread(LPVOID param);						// メインスレッド

	//// 各インスタンス
	LogFileManager*			mcls_pLog;									// LogFileManagerスレッドインスタンス
	
	// 同期シグナル
	HANDLE					m_evThStart;								// MainInctanceスレッドの実行要求

	// その他もろもろ
	int						m_nUnitNum;									// 管理対象機器数







// ======================================================================
// 小物
public:
	// ループ算出
	void GetChLoop(int nUnit, int& ii_s, int& ii_e)						// ユニット指定時の対象算出
	{
		if(0==nUnit) { ii_s=0; ii_e=m_nUnitNum; }
		else	     { ii_s=nUnit-1; ii_e=nUnit;}
	}

	//// カメラIDからスレッドインデックスと
	//// nCamId 0オリジン
	//// nThreadIdx, nIdx 0オリジン
	//bool GetCamIdToThreadID(int nCamId, int& nThreadIdx, int& nIdx)
	//{
	//	for(int ii=0; ii<MAX_THREAD; ii++) {
	//		if( NULL == gcls_pCam[ii] ) continue;
	//		int nStart	= gcls_pCam[ii]->GetMyCamStartIdx() ;
	//		int nMax	= gcls_pCam[ii]->GetMyCamNum() + nStart;  
	//		if( nCamId >= nStart &&  nMax > nCamId ) {
	//			nThreadIdx	= ii;
	//			nIdx		= nCamId - nStart;
	//			return true;
	//		}
	//	}
	//	return false;		// ここまで来たらNG
	//}




};
