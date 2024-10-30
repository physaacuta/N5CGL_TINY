#pragma once

#include "SmyuBaseManager.h"

class SmyuPlgManager :	public SmyuBaseManager
{
//// メンバー定数
private :
	static const int	TIME_EXEC_INTERVAL		= 30;					// PLGチェック実行周期

//// 公開関数
public:
	SmyuPlgManager(int nMyID);
	virtual ~SmyuPlgManager(void);


	virtual void SmyuStart();
	virtual void SmyuStop();


//// メンバー関数
private:
	int ThreadFirst();													// スレッド開始前処理 (スレッド実行直後(シグナルセット前)にコールバック)
	int ThreadLast();													// スレッド終了前処理 (終了シグナル後にコールバック)
    void ThreadEvent(int nEventNo);										// スレッドイベント発生
	
	void SmyuExec();													// シミュレータ 実行


//// メンバー変数
private:

	//// ハンドル
	HANDLE					m_evTimerExec;								// 定周期実行イベント
	
	//// その他
	int						m_nStoryIndex[SMYU_PLG_MAX_CHANNEL];		// シナリオ実行位置
};
