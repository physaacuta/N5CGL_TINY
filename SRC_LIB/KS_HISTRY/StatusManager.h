#pragma once

// 疵検クラス インクルード
#include "..\..\Library\KizuLibMFC\CLASS\ThreadManager.h"				// ワーカースレッドマネージャー
#include "..\..\Library\KizuLibMFC\Class\CreateSQL.h"					// SQL文生成補助クラス

// STL
#include <vector>														// リスト用

// ローカルのクラス
#include "MainConst.h"

class StatusManager : public ThreadManager
{
//// メンバー関数
protected:
	static const int MAX_COLUMN		= 64;								// １行で管理する最大列数
//	static const int DB_CHECK_TIME	= 1 * 60 * 1000;					// 機器状態チェック周期 [ms]
	static const int DB_CHECK_TIME	=  10*1000;							// 機器状態チェック周期 [ms]


	// 履歴管理構造体
	struct TYP_LIST {
		//// マスター情報
		int			nKey;												// 区分
		int			nCycle;												// 収集間隔 [分]
		CString		sName[MAX_COLUMN];									// 名称
		int			nId[MAX_COLUMN];									// 収集する機器ID (0:対象外)
		
		//// 付加情報
		int			nNextCnt;											// 次収集待ちカウント (nCycleを入れて、1分毎に減産 0:で実行)

		CString		sNewTime;											// 登録時間 (主キーの一つ)
		
		bool		bSetN[MAX_COLUMN];									// データセット有無 (これがfalseのときは、DBにNULLセット)
		int			nNewN[MAX_COLUMN];									// 最新の値

		bool		bSetS[MAX_COLUMN];									// データセット有無
		CString		sNewS[MAX_COLUMN];									// 最新の付加情報
	};

//// 公開関数
public:
	StatusManager(void);
	virtual ~StatusManager(void);


//// メンバー関数
protected:
	int ThreadFirst();													// スレッド開始前処理 (スレッド実行直後(シグナルセット前)にコールバック)
	int ThreadLast();													// スレッド終了前処理 (終了シグナル後にコールバック)
    void ThreadEvent(int nEventNo);										// スレッドイベント発生
	
	// メイン処理
	bool	Init();														// 初期化
	void	CheckItem();												// 収集タイミングを監視
	bool	GetStatusInf(TYP_LIST* t);									// DBから設定情報を取得

	virtual void SetStringData(TYP_LIST* t) {};							// 付加情報部に登録



//// メンバー変数
protected:
	// ハンドル
	HANDLE								m_evTimerCheck;					// DBチェック タイマー [ms]

	// 定義
	int									m_nDbColumnKiki;				// 機器状態 の列数
	int									m_nDbColumnTemp;				// 付加情報 の列数

	// その他もろもろ
	std::vector<TYP_LIST>				m_vItem;						// 履歴管理情報 (単方向リスト)
};
