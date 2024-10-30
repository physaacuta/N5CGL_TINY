#pragma once

// 疵検クラス インクルード
#include "..\..\Library\KizuLibMFC\CLASS\ThreadManager.h"				// ワーカースレッドマネージャー

#include "DefragManager.h"												// インデックス管理


class ControlDefragManager : public ThreadManager
{
public:
	ControlDefragManager(void);
	~ControlDefragManager(void);


	void WorkerNew(int id, char* cSession);								// 管理スレッド生成
	void WorkerStop();													// 管理スレッド停止処理


	// 外部アクセス
	bool IsDefragEnable()	const { return m_nDefragIntervalDay == 0 ? false : true; }


//// メンバー関数
private:
	int ThreadFirst();													// スレッド開始前処理 (スレッド実行直後(シグナルセット前)にコールバック)
	int ThreadLast();													// スレッド終了前処理 (終了シグナル後にコールバック)
    void ThreadEvent(int nEventNo);										// スレッドイベント発生

	void Exec();														// インデックス デフラグ実行可否判定

protected:
	// 各ワーカースレッド
	int						m_nClassCnt;								// インスタンスの生成数
	DefragManager*			mcls_pDefrag[CONTROL_MAXCNT];				// インデックス管理

	//// ハンドル
	HANDLE					m_evTimerExec;								// 定周期実行イベント


	// その他もろもろ
	int						m_nDefragIntervalDay;						// デフラグ実行間隔 [日] 0:未実行


	// 子供が参照
	bool					m_bExecDefrag;								// デフラグ実行可の場合、true

};
