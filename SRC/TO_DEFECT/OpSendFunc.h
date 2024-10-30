// *********************************************************************************
//	ちょっと特殊なモジュール群
//	[メモ]
//		各インスタンスは、staticで .cppに持たす
// *********************************************************************************

#pragma once

// 半固定
#include "..\..\Include\LineCommon.h"									// インクルード
#include "..\..\Include\ToHtCommon.h"									// 統括判定専用インクルード

#include "..\..\Library\KizuLibMFC\Class\LogFileManager.h"				// ログ管理クラス
#include "..\..\Library\KizuLib\Class\KizuFunction.h"					// スタティックライブラリ集

// EPCインクルード
//#include "..\..\Include\EPC\NCsmemData.h"
//#include "..\..\Include\EPC\NCattr.h"
#include "..\..\Include\EPC\NCL2DETECT_if.h"							// I/F定義
#include "..\..\Include\EPC\NCL2attr.h"									// 特徴量定義

// ローカル
#include "MainConst.h"

using namespace KizuLib;

class OpSendFunc
{
public:
	OpSendFunc(void);
	virtual ~OpSendFunc(void);



///////////////////////////////////////////////////////////////////////////////////////////////////////
//// メンバー定数
private:
	static const int			DEFECT_BUF_CNT = MAP_COL_NUM * MAP_NEST_NUM;	// システム最小間隔での最大疵バッファリング数


///////////////////////////////////////////////////////////////////////////////////////////////////////
//// 公開構造体
public: 




///////////////////////////////////////////////////////////////////////////////////////////////////////
//// プロパティ
public:
	void SetLogMgr(LogFileManager* pCls) {mcls_pLog = pCls;}							// ログクラスセット
	void SetLogMarkMgr(LogFileManager* pCls) {mcls_pLogMark = pCls;}					// マーキング関連ログクラスセット
	void SetQueOpSend(ThreadQueue<SOCK_BASE_HEAD>* pQue, int ii) { mque_pOpSend[ii] = pQue; }		// 統括→表示送信通知のキュー
	void SetCsLockCoil(CRITICAL_SECTION * pCS) {m_csLockCoil = pCS;}

	// 各インスタンス (定義のみ。実体は .cpp に記述)
	void SetStatusMgr(void* p);
	void SetParamMgr(void* p);
	void SetCoilMgr(void* p);
	void SetPlgMgr(void* p);
	void SetHtRecv(int men, void* pCls);



///////////////////////////////////////////////////////////////////////////////////////////////////////
//// 共通処理

private:


///////////////////////////////////////////////////////////////////////////////////////////////////////
//// 送信 (すべて静的にする)
public:
	static void		SendOp_Status();																						// #0 ステータス
	static void		SendOp_CoilInf_Ken(COIL_INF const* pCoilInf);															// #1 コイル情報 (検査用)
	static void		SendOp_CoilResult(COIL_INF const* pCoilInf);															// #2 コイル実績 (1コイル検査完了時)
	static void		SendOp_CoilInf_Dsp(COIL_INF const* pCoilInf);															// #3 コイル情報 (表示用)
	static void		SendOp_DetectData(int men, COIL_INF const* pCoilInf, DETECT_BASE_DATA const* pData,
								DEFECT_DATA_IMAGE const* pImg, float dLen);													// #4 疵情報 (疵情報+疵画像)
	static void		SendOp_Length(int men=-1, int y=-1);																	// #5 通板距離
	static void		SendOp_CutInf(char const* cKizukenNo, TO_SO_DATA_CUT const* pInf);										// #6 カット情報
	static void		SendOp_AlarmData(ALARME_OUTPUT const* pInf);															// #7 警報情報
	static void		SendOp_PatoReset(char const* cKizukenNo, int nMen, int nKizuSeq);										// #8 疵一覧表疵情報削除
	static void		SendOp_DetectNum(char const* cKizukenNo, int nMen, long nLen, long nNum);								// #9 疵個数情報 (最小区間内)

	//=========================================
	// 疵情報
	static void		SendOp_ColRowData(int men, COIL_INF const* pCoilInf, long y, TYP_COLROW_BUF pBufDefCol[], TYP_COLROW_BUF pBufDefAry[][MAP_NEST_NUM], int nDefCnt);// 表示機能に最小区間の行列情報を送信
	static void		SendOp_ColRowData(int men, COIL_INF const* pCoilInf, /*long y, int x,*/ TYP_COLROW_BUF* pData);				// 表示機能に最小区間の行列情報を送信

///////////////////////////////////////////////////////////////////////////////////////////////////////
//// メンバー 変数
private:

	//=========================================
	// 同期制御用
	static CRITICAL_SECTION*		m_csLockCoil;							// CoilManagerが本体


	//=========================================
	// 受け渡しキュー
	static ThreadQueue<SOCK_BASE_HEAD>*	mque_pOpSend[HY_SOCK_NUM];			// 統括→表示送信通知のキュー


	//=========================================
	// ログ用
	static LogFileManager*			mcls_pLog;								// LogFileManagerスレッドインスタンス
	static LogFileManager*			mcls_pLogMark;							// マーキング関連LogFileManagerスレッドインスタンス
	static CString					my_sThreadName;							// 自スレッド名
};


