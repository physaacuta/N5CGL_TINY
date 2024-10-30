#pragma once

// 疵検クラス インクルード
#include "..\..\Library\KizuLibMFC\CLASS\ThreadManager.h"				// ワーカースレッドマネージャー
#include "..\..\Library\KizuODBC\OdbcBase.h"							// ODBC操作クラス

#include "..\..\Include\KizuSmyu.h"										// GOTインクルード

// STL
#include <vector>														// リスト用


// ローカル
#include "MainConst.h"		
#include "PlgControlManager.h"
#include "PioControlManager.h"

class SmyuBaseManager :	public ThreadManager
{

//// 公開関数
public:
	SmyuBaseManager(const char* pThreadName, int nMyID);
	virtual ~SmyuBaseManager(void);

	void SetPlgMgr(PlgControlManager* pCls) { mcls_pPlg = pCls;};		// ステータス制御クラスセット
	void SetPioMgr(PioControlManager* pCls) { mcls_pPio = pCls;};		// ステータス制御クラスセット

	bool SetCsvFilePath(LPCSTR sPath);									// シナリオ読み込み


	virtual void SmyuStart() {};
	virtual void SmyuStop() {};



protected:
	void RaiseEvents(EM_DIV_KIND emEvent, int nEventSub, BYTE* pEvent);	// イベント発生



//// メンバー関数
private:

	//================================================
	// シナリオ生成部
	bool ReadCsv(LPCSTR sPath, std::vector<CString>* pRow);			// CSVファイル読み込み
	bool CsvToEvent(std::vector<CString>* pRow, std::vector<TYP_SINA_INF*>* pEvent);
	bool GetColumn(CString& sVal, int& curPos, CString& sToken);	// １行のデータから , 単位の文字列を取得

	//-----------------------------
	// 条件発生部
	/// 距離駆動シナリオ
	void RaiseData_Plg_Pos(CString& sVal, int& curPos, TYP_SINA_INF* pInf);
	/// イベント駆動シナリオ
	void RaiseData_Evt_Mail(CString& sVal, int& curPos, TYP_SINA_INF* pInf);
	void RaiseData_Evt_Po(CString& sVal, int& curPos, TYP_SINA_INF* pInf);
	/// 時間駆動シナリオ
	void RaiseData_Time_Cycle(CString& sVal, int& curPos, TYP_SINA_INF* pInf);

	//-----------------------------
	// イベント部
	void EventData_Plg(CString& sVal, int& curPos, TYP_SINA_INF* pInf);
	void EventData_Pi(CString& sVal, int& curPos, TYP_SINA_INF* pInf);
	void EventData_Mem(CString& sVal, int& curPos, TYP_SINA_INF* pInf);
	void EventData_Mail(CString& sVal, int& curPos, TYP_SINA_INF* pInf);
	void EventData_Time(CString& sVal, int& curPos, TYP_SINA_INF* pInf);
	void EventData_Zz(CString& sVal, int& curPos, TYP_SINA_INF* pInf);

//// メンバー変数
protected:

	//=========================================
	// 各インスタンス
	PlgControlManager*			mcls_pPlg;								// PLG距離管理クラス
	PioControlManager*			mcls_pPio;								// PIO管理クラス


	//=========================================
	// シナリオ
	std::vector<TYP_SINA_INF*>	m_lstStory;							// シナリオ


	//=========================================
	// その他
	int							m_nMyID;								// 自分のID (0オリジン)
};
