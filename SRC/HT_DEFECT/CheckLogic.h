#pragma once

// 疵検クラス インクルード
#include "..\..\Library\KizuLibMFC\CLASS\ThreadManager.h"				// ワーカースレッドマネージャー
#include "..\..\Library\KizuODBC\OdbcBase.h"							// ODBC操作クラス

class CheckLogic : public ThreadManager
{

private:
	struct TYP_DELEV
	{
		int				scode;											// scode 0:全部
	};


//// 公開関数
public:
	CheckLogic();
	virtual ~CheckLogic();

	void	Check(int scode);											// Logic有無チェック


//// 公開変数
public : 
	// スレッドキュー
	ThreadQueue<TYP_DELEV>	gque_Check;									// チェック依頼用キュー





//// メンバー関数
private:		
	// スレッド動作定義
	int ThreadFirst();													// スレッド開始前処理 (スレッド実行直後(シグナルセット前)にコールバック)
	int ThreadLast() { return 0; };										// スレッド終了前処理 (終了シグナル後にコールバック)
    void ThreadEvent(int nEventNo);										// スレッドイベント発生
	
	EM_DIV_BOOL ExecLogic(int scode);									// ロジックファイル有無チェック


private:

	// その他
	EM_DIV_BOOL			m_bLogicUmu;									// ロジックファイル有無結果
};

