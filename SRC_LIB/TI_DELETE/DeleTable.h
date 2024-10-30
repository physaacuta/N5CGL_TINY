#pragma once
#include "kizudeletemanager.h"

class DeleTable: public KizuDeleteManager
{
public:
	DeleTable(int id, int* nWaitTime, HANDLE evEndCycle, HANDLE evEndSizeMin, CString cSession, CString cOdbcSession);
	virtual ~DeleTable(void);


private:
    // //////////////////////////////////////////////////
    // 親クラス メソッドの実装
    // //////////////////////////////////////////////////
	void ExecCycle();													// 定周期実行メソッド
	void ExecSizeMin();													// DB容量低下実行メソッド
	void ExecFast();													// 初回実行メソッド


    // //////////////////////////////////////////////////
    // 独自 メソッド
    // //////////////////////////////////////////////////
	void Delete_BackUp(const char* cKizukenNo);							// バックアップ削除


    // //////////////////////////////////////////////////
    // これより下が今回システム毎に変更
    // //////////////////////////////////////////////////
	int						m_nDelTblCoil;								// 削除テーブル件数
	int						m_nDelTblTime;								// 削除テーブル件数	
	int						m_nDelTblCnt;								// 削除テーブル件数	
	DEL_TBL*				m_pDelTblCoil;								// 削除テーブル		(コイル実績に紐付く情報)
	DEL_TBL*				m_pDelTblTime;								// 削除テーブル		(日付に紐付く情報)
	DEL_TBL_CNT*			m_pDelTblCnt;								// 削除テーブル		(保存上限数に紐付く情報)
};
