// *********************************************************************************
//	カメラリンクスイッチ設定ユーティリティ
//	[メモ]
//		各インスタンスは、staticで .cppに持たす
// *********************************************************************************

#pragma once

// 半固定
#include "..\..\Library\KizuLibMFC\Class\LogFileManager.h"				// ログ管理クラス

// ローカル
#include "MainConst.h"
#include "StatusManager.h"												// ステータス制御基底クラス

using namespace KizuLib;

class ToLinkswUtil
{
public:
	ToLinkswUtil(LogFileManager* pCls1, StatusManager* pCls2);
	~ToLinkswUtil();

public:
	//=========================================
	// 外部受け渡し情報
	struct LINKSW_OUTPUT_INFO
	{
		int nCamPort;												// カメラポート (1オリジン) 0:出力停止
		int nFPGABoardNo;											// FPGAボードNo (0～7)
		int nFPGACh;												// FPGAボードCh (1～15)
		int nLinkswNo;												// リンクスイッチNo (0～3)
		int nCamset;												// カメラセット (1オリジン) 0:出力停止
	};
	struct TO_LINKSW_OUTPUT
	{
		LINKSW_OUTPUT_INFO	output[NUM_ALL_HT][NUM_CAMLINK_BOARD];	// [PC台数][PC1台当たりの接続ch数]
	};
	
///////////////////////////////////////////////////////////////////////////////////////////////////////
//// メンバー定数
private:

	//=========================================
	// ログ用
	static LogFileManager*		mcls_pLog;											// LogFileManagerスレッドインスタンス
	static CString				my_sThreadName;										// 自スレッド名

	static StatusManager*		mcls_pStatus;										// ステータス制御基底クラス

public:

///////////////////////////////////////////////////////////////////////////////////////////////////////
//// プロパティ
public:

///////////////////////////////////////////////////////////////////////////////////////////////////////
//// 内部関数
private:

///////////////////////////////////////////////////////////////////////////////////////////////////////
//// 公開関数
public:
	static void GetFPGAInfo(int pcid, int camno, int *pFPGABoardNo, int *pFPGACh);
	static void GetOutputInfo(StatusManager *pStatus, TO_LINKSW_OUTPUT *pOutputInfo);
	static void GetDeterminedHanteiPC(StatusManager *pStatus, TO_LINKSW_OUTPUT *pInfo);
	static int GetTargetCamNo(const TO_LINKSW_OUTPUT *pInfo, long nFPGABoardNo, long nCamno);
	static int GetHanteiGroup(int nMen, int nHantei, int nCam);

///////////////////////////////////////////////////////////////////////////////////////////////////////
//// メンバー 変数
private:
};

