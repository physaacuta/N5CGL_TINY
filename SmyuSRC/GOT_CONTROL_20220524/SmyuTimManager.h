#pragma once

#include "SmyuBaseManager.h"

class SmyuTimManager :	public SmyuBaseManager
{
//// メンバー定数
private :
	static const int	TIME_MAX_NUM		= 32;						// 最大タイマー数

//// 公開関数
public:
	SmyuTimManager(int nMyID);
	virtual ~SmyuTimManager(void);

	virtual void SmyuStart();
	virtual void SmyuStop();

	void TimerExec(int nId, int nFirst, int nCycle);					// タイマー操作


//// メンバー関数
private:
	int ThreadFirst();													// スレッド開始前処理 (スレッド実行直後(シグナルセット前)にコールバック)
	int ThreadLast();													// スレッド終了前処理 (終了シグナル後にコールバック)
    void ThreadEvent(int nEventNo);										// スレッドイベント発生
	

	void SmyuExec(int nIndex);											// シミュレータ 実行


//// メンバー変数
private:

	//// ハンドル
	HANDLE					m_evTimerExec[TIME_MAX_NUM];				// 定周期実行イベント
};
