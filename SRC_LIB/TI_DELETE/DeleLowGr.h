#pragma once

// 疵検 インクルード
#include "..\..\Library\KizuLibMFC\CLASS\ThreadManager.h"				// ワーカースレッドマネージャー
#include "..\..\Library\KizuLib\CLASS\KizuMacro.h"						// 疵検マクロ
#include "..\..\Library\KizuODBC\OdbcBase.h"							// ODBC操作クラス

// 疵検ライブラリ インクルード
#include "MainConst.h"


using namespace KizuLib;
using namespace KizuODBC;


class DeleLowGr : public ThreadManager
{
private:
	//------------------------------------------
	// 固定定数
	//------------------------------------------
	static const int DELETE_MAXCNT				= 200;					// 1回の削除件数
	static const int SLEEP_TIME					= 2000;					// 1回削除後の待ち時間 [ms]

//// 公開関数
public:
	DeleLowGr(int id, HANDLE evEndExec, SETTING_TBL const* pTyp);
	virtual ~DeleLowGr(void);


	//// 外部アクセス
	void SetStop()				{ m_bStopFlg = true; SetEvent(my_evStop);}	// 終了要求
	void SetExec(char* cNo)		{ memcpy(m_cKey, cNo, sizeof(m_cKey)); SetEvent(m_evExec); m_bIsExec=true;};	// 実行依頼
	bool IsExec() const			{ return m_bIsExec; }
	const char* ExecKey()		{ return m_cKey; }


//// メンバー関数
private :
	int ThreadFirst();													// スレッド開始前処理 (スレッド実行直後(シグナルセット前)にコールバック)
	int ThreadLast();													// スレッド終了前処理 (終了シグナル後にコールバック)
    void ThreadEvent(int nEventNo);										// スレッドイベント発生


	bool StartExec();													// 無害Gr削除開始
	bool DeleteInf(OdbcBase* pClsDB, char const* cKeyTbl, char const* cKeyWhereSql, char const* cDeleTbl, char const* cKizukenNo, int nKizuNo);	// 無害Gr削除実行
	bool SetDeleteOkFlg(OdbcBase* pClsDB, char const* cKizukenNo);		// 無害Gr削除管理テーブルをアップデート


//// メンバー変数
private :

	//// 同期シグナル
	HANDLE					m_evExec;									// 削除実行シグナル
	HANDLE					m_evEndExec;								// 削除完了イベント

	//// 実行中情報
	bool					m_bIsExec;									// 処理実行中
	char					m_cKey[64];									// 実行対象主キー

	//// iniファイル
	CString					m_cIniSqlSelectEnd;							// 無害Gr削除後の一覧テーブル更新SQL (WHERE句には削除した主キーがセットされる)

	//// その他もろもろ
	int						m_nID;										// 自分のID (1オリジン)
	bool					m_bStopFlg;									// 終了要求時 true
	SETTING_TBL const *		m_pTypSet;									// 削除対象テーブル情報

};
