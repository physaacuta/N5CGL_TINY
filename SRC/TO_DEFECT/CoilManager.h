#pragma once

// 共通ヘッダー
#include "..\..\Include\LineCommon.h"									// インクルード
#include "..\..\Include\ToTbl.h"										// 統括共有メモリテーブル インクルード
#include "..\..\Include\OutInterface.h"									// 外部インターフェース用インクルード

// ローカルのクラス
#include "MainConst.h"
#include "CoilBaseManager.h"											// コイル情報基本クラス
#include "StatusManager.h"												// ステータス制御クラス
#include "ParamManager.h"												// パラメータ管理
#include "DBManager.h"													// DB登録制御クラス

using namespace KizuLib;

class CoilManager :	public CoilBaseManager
{

public:
	CoilManager(void);
	virtual ~CoilManager(void);



/////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 情報収集 コイル情報
public:
	COIL_INF*	GetCoilKen_DataSet(int men) { return m_pCoilKenV[men];}				// データセット用 [★誰も使ってない]
	COIL_INF*	GetCoilRec_DataSet(int rec){ return &m_CoilInf[rec];}				// 指定されたレコードのコイル情報取得＋取得先でデータ変更可能

	int					GetRecKey(char const* cMeiNo, char const* cCoilNo);			// レコード位置取得（命令No、生産No）


/////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 情報収集 行単位関係
	void SetData_StKensa(int men, int rec, EM_DIV_KENSA st);						// 現コイル情報に検査状態をセット [←PlgMgr]
	void SetData_StKiki (int men, int rec, EM_DIV_KIKI  st);						// 現コイル情報に機器状態をセット [←PlgMgr]
	void SetData_Edge   (int men, int rec, float* dEdge);							// 現コイル情報にエッジ位置をセット [←HtRecv]
	void SetData_Row_V	(int men, int rec, int index, int val);						// 現コイル情報に行単位の情報をセット [←HtRecv] 
	void SetData_Row_R	(int men, int rec, int index, int val);						// 現コイル情報に行単位の情報をセット [←PlgMgr]
	void SetData_Row_V_Ls(int men, int rec);										// 現コイル情報の最終レコード情報更新 [←HtRecv]
	void SetData_Row_R_Ls(int men, int rec);										// 現コイル情報の最終レコード情報更新 [★誰も使ってない] 
	void SetData_CoilPosition(bool mode, int men);									// コイル位置情報を生成 [←CoilMgr, HtRecv]



/////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 情報収集 コイル単位関係
	void SetData_CoilResult_Real(int endMode, double dLen, double dLenSub);			// 入側論理コイル実績を生成 (実際の物の流れのシメ) [←MI]
	void SetData_CoilResult_Virtual();												// 入側論理コイル実績をセット (データの流れのシメ) [←MI, HtRecv]
	void SetData_ShaCut(double dLenIn, int mode=0);									// シャーカット実行(mode=0:通常カット、1:実績確定時)[←MI]
	void SetData_StartWpd(int nMode);												// コイル先頭の切替条件 [←MI]
	void SetData_CoilCnt();															// 検査開始からN本目 [←MI]
	void SetData_CoilResult_BrVal(int* pBr);										// 現エッジ内平均輝度をコイル実績にセット [←MI]
	void SetData_CoilResult_AgcVal(int* pExp);										// 現露光時間をコイル実績にセット [←MI]
	void SetData_CoilLenYotei(double dInLen);										// 予定コイル長セット [←MI]


/////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 情報収集 その他
	void SetAlameInf(DETECT_BASE_DATA const* pD, int men, COIL_INF* pCoil, double const* pAttr);
	void SetData_AlarmCancel(int pos);												// 欠陥検出警報解除	[←MI]

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ほか
public:
	bool	IsCoilNextKen()	const{ return 0!=strlen(m_CoilNext_Ken.cKizukenNo) ? true : false; }	// 次コイル情報受信済み？ [←MI、Dlg、OpSendFunc]
	void	ClearCoilNextKen() { CoilClear(&m_CoilNext_Ken); }										// 次コイル情報バッファをクリア [←MI]
	void	ClearFirstCoilInf();																	// 現コイル反映時、検査開始1本目をバッファからクリア[←MI]
	CString GetDefectDumpPath(int nMen, int nRec);													// 疵情報ファイル出力先ファイル名称を返却[←CoilMgr]
	int		SetDefectCount(int men, const char* pKizukenNo, int row);								// 指定最小区間の疵数を＋１する
	int		GetDefectCount(int men, const char* pKizukenNo, int row);								// 指定最小区間の疵数を取得する

	void SetLogMgrA(LogFileManager* pLog) { mcls_pLogA = pLog; }					// 警報用ログファイルセット

private:
	LogFileManager*		mcls_pLogA;													// 警報用ログクラス
};