#pragma once

// STL
#include <vector>														// リスト用


// 疵検クラス インクルード
#include "..\..\Library\KizuLibMFC\CLASS\ThreadManager.h"				// ワーカースレッドマネージャー
#include "..\..\Library\KizuODBC\OdbcBase.h"							// DB操作クラス

#include "..\..\Library\KizuLib\CLASS\KizuPerformanceCounter.h"			// 時間計測

// 疵検ライブラリ インクルード
#include "MainConst.h"


using namespace KizuLib;
using namespace KizuODBC;


class DefragManager : public ThreadManager
{
private :
	//// 受け渡し構造体
	struct DEF_TBL{
		CString		cDefTable;											// 管理対象の実績テーブル名称
	};


//// 公開関数
public:
	DefragManager(int id, char* cSession);
	virtual ~DefragManager(void);



	//// 外部アクセス
	void SetRunCnacel(bool bMode)	{ m_bRunCancel = bMode; }			// デフラグ強制キャンセル時 true
	void SetExecDefragPnt(bool* p)	{ m_pExecDefrag = p;}				// 参照ポインタ


//// メンバー関数
private:
	int ThreadFirst();													// スレッド開始前処理 (スレッド実行直後(シグナルセット前)にコールバック)
	int ThreadLast();													// スレッド終了前処理 (終了シグナル後にコールバック)
    void ThreadEvent(int nEventNo);										// スレッドイベント発生


	void Exec();														// インデックス デフラグ実行
	bool GetIndexTables(char const* pName, std::vector<CString>* v);	// インデックス 一覧取得
	int  ExecDefrag(char const* pTblName, char const* pIdxName);		// インデックス デフラグ実行


	bool IsExec() { 
		return (*m_pExecDefrag && ! m_bRunCancel) ? true : false; 
	}


//// メンバー変数
private:

	//// ハンドル
	HANDLE					m_evTimerExec;								// 定周期実行イベント

	//// 外部書き換えポインタ
	bool*					m_pExecDefrag;								// デフラグ実行可の場合、true


	//// 管理テーブル
	int						m_nTblNum;									// 管理テーブル件数
	DEF_TBL*				m_pTblDef;									// 管理テーブル情報
	int						m_nTblExecNum;								// 実行途中の番号 (0オリジン)
	int						m_nIdxExecNum;								// 実行途中の番号 (0オリジン)
	int						m_nDefragIntervalDay;						// デフラグ実行間隔 [日] 0:未実行


	//// その他もろもろ
	CString					m_cSession;									// 自分の設定が定義されているiniファイルのセッション
	CString					m_cOdbcSession;								// 接続するODBCが定義されてるセッション
	bool					m_bRunCancel;								// デフラグ強制キャンセル時 true
	CTime					m_LastDefragTime;							// 最終実行日時
};
