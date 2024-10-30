#pragma once

// 疵検クラス インクルード
#include "..\..\Library\KizuLibMFC\CLASS\ThreadManager.h"				// ワーカースレッドマネージャー
#include "..\..\Library\KizuODBC\OdbcBase.h"							// ODBC操作クラス

#include "..\..\Include\KizuSmyu.h"										// GOTインクルード

// STL
#include <vector>														// リスト用

// ローカル
#include "MainConst.h"													// 自クラス定数ヘッダー


class CsvReader :	public ThreadManager
{
//// 公開関数
public:
	CsvReader(void);
	virtual ~CsvReader(void);


	//// CSV操作
	bool CreatEvents(CString sPath, std::vector<TYP_SINA_INF*>* pEvent);	// シナリオ解析


//// メンバー関数
private:
	int ThreadFirst() {return 0;};										// スレッド開始前処理 (スレッド実行直後(シグナルセット前)にコールバック)
	int ThreadLast() {return 0;};										// スレッド終了前処理 (終了シグナル後にコールバック)
    void ThreadEvent(int nEventNo) {return;};							// スレッドイベント発生
	

	bool ReadCsv(CString sPath, std::vector<CString>* pRow);		// CSVファイル読み込み
	bool CsvToEvent(std::vector<CString>* pRow, std::vector<TYP_SINA_INF*>* pEvent);
	bool GetColumn(CString& sVal, int& curPos, CString& sToken);	// １行のデータから , 単位の文字列を取得


	//================================================
	// 条件発生部
	//// 距離駆動シナリオ
	void RaiseData_Plg_Pos(CString& sVal, int& curPos, TYP_SINA_INF* pInf);
	//// イベント駆動シナリオ
	void RaiseData_Evt_Mail(CString& sVal, int& curPos, TYP_SINA_INF* pInf);
	void RaiseData_Evt_Po(CString& sVal, int& curPos, TYP_SINA_INF* pInf);
	//// 時間駆動シナリオ
	void RaiseData_Time_Cycle(CString& sVal, int& curPos, TYP_SINA_INF* pInf);

	//================================================
	// イベント部
	void EventData_Plg(CString& sVal, int& curPos, TYP_SINA_INF* pInf);
	void EventData_Pi(CString& sVal, int& curPos, TYP_SINA_INF* pInf);
	void EventData_Mem(CString& sVal, int& curPos, TYP_SINA_INF* pInf);
	void EventData_Mail(CString& sVal, int& curPos, TYP_SINA_INF* pInf);
};
