#pragma once

// 疵検クラス インクルード
#include "..\..\Library\KizuLibMFC\CLASS\ThreadManager.h"				// ワーカースレッドマネージャー



class SampManager :	public ThreadManager
{

//// 公開関数
public:
	SampManager(void);
	virtual ~SampManager(void);


//// メンバー関数
private:
	int ThreadFirst();													// スレッド開始前処理 (スレッド実行直後(シグナルセット前)にコールバック)
	int ThreadLast();													// スレッド終了前処理 (終了シグナル後にコールバック)
    void ThreadEvent(int nEventNo);										// スレッドイベント発生
	
//// メンバー変数
private:

	//// ハンドル
	HANDLE					m_evTimerExec;								// 定周期実行イベント


};
