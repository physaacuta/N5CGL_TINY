#pragma once
// 半固定
#include "..\..\Include\LineCommon.h"									// 疵検インクルード

// 疵検クラス インクルード
#include "..\..\Library\KizuLibMFC\CLASS\ThreadManager.h"				// ワーカースレッドマネージャー

#include "..\..\Library\AtlRegExpLib\AtlRegManager.h"					// 正規表現 クラス
#include "..\..\Library\KizuLibMFC\Class\DivNameManager.h"				// DIV定数 名称取得クラス

// ローカル
#include "..\..\Include\ToHtCommon.h"									// 統括判定用インクルード
#include "..\..\Include\ToTbl.h"										// 統括共有メモリテーブル

#include "MainConst.h"
#include "ParamManager.h"

using namespace KizuLib;

// ファンクションの使い方だが、ThreadManagerを継承しておく。ワーカースレッドは起動しない
class CoilManager :	public ThreadManager
{
//// 公開構造体
public: 
	struct TYP_NEWCOIL {
		COIL_SETTING 	setting;
		COIL_BASEDATA	basedata;
	};



//// 公開関数
public:
	CoilManager(LogFileManager* pLog);
	virtual ~CoilManager(void);
	
	// 検査装置共通情報特定
	void Check_CoilInf(int nRec, bool bRevFlg=false, bool bHandSetFlg = false);			// コイル情報を特定 [←Dlg]

	// 画面用
	TYP_NEWCOIL const*	GetNewCoil()				{ return &m_typNewCoil; }	

	// ダイアログから検査条件を手入力 対応
	void SetHandSetScode(int men, int val)		{ m_nHandSetScode[men] = val; }			// 手入力されたSCODE [←Dlg]
	void SetHandSetPcode(int men, int val)		{ m_nHandSetPcode[men] = val; }			// 手入力されたPCODE [←Dlg]

	int  GetHandSetScode(int men)				{ return m_nHandSetScode[men]; }		// 手入力されたSCODE [←Dlg]
	int  GetHandSetPcode(int men)				{ return m_nHandSetPcode[men]; }		// 手入力されたPCODE [←Dlg]

	CString			GetSpmModeName(bool bSpm)	  const { return bSpm ? "SPM圧下時" : "SPM開放時"; };	// SPM状態の名称取得 

//// メンバー関数
private:
	// ワーカースレッドとして動作させないため、関係関数を非公開に変更
	int				ThreadFirst()				{return 0;}				// スレッド開始前処理 (スレッド実行直後(シグナルセット前)にコールバック)
	int				ThreadLast()				{return 0;}				// スレッド終了前処理 (終了シグナル後にコールバック)
    void			ThreadEvent(int nEventNo)	{;}						// スレッドイベント発生

	void	SelectScode(PRO_TYPE_DATA_COILINF const* c, int ou, bool bSpm, int* scode);	// 検査表面状態を決定
	void	SelectPcode(PRO_TYPE_DATA_COILINF const* c, int ou, bool bSpm, int* pcode);	// 検査パターンを決定

//// メンバー 変数
private:

	//// 各インスタンス 
	ParamManager			mcls_Param;									// パラメータ管理

	// 共有メモリ情報
	HANDLE					m_hCoil;									// コイル情報 ハンドル
	TO_COIL_TBL*			mtbl_pCoil;									// コイル情報 データ

	// その他もろもろ
	TYP_NEWCOIL				m_typNewCoil;								// 最新のコイル情報

	int						m_nHandSetScode[NUM_MEN];					// ダイアログに手入力された SCODE
	int						m_nHandSetPcode[NUM_MEN];					// ダイアログに手入力された PCODE

};
