//=====================================================================
// iPortViewer I/Fクラス
//	【全ライン】
//		・iPortViewerとのI/Fを行う
//=====================================================================
#pragma once

//#include "..\..\Library\KizuLib\KizuLib.h"								// 疵検ライブラリ
//#include "..\..\Library\KizuLib\Class\KizuMacro.h"						// マクロ集
//#include "..\..\Library\KizuLib\Class\KizuFunction.h"					// スタティックライブラリ集
//#include "..\..\Library\KizuLibMFC\Class\LogFileManager.h"				// ログ管理クラス
//
//// 疵検クラス インクルード
//#include "..\..\Library\KizuLib\Class\WorkThread.h"						// ワーカースレッドクラス


// 疵検クラス インクルード
#include "..\..\Library\KizuLibMFC\CLASS\ThreadManager.h"				// ワーカースレッドマネージャー
#include "..\..\Library\KizuLib\CLASS\MailSlotManager.h"				// メールスロット管理

// 共通ヘッダー
#include "..\..\Include\LineCommon.h"									// 疵検インクルード

// ローカルのクラス
#include "MainConst.h"
#include "StatusManager.h"												// ステータス管理クラス
#include "ToOutMailSender.h"											// 外部メールスロット送信モジュール

using namespace KizuLib;

class IPortViewerManager : public ThreadManager  //WorkThread //
{

//// 公開関数
public:
	IPortViewerManager(void);
	virtual ~IPortViewerManager(void);

	void	Stop();												// スレッド終了

	// 外部プロパティ
	void	SetStatusMgr(StatusManager* p)	{ mcls_pStatus = p; }		// ステータス管理クラス[←MI]


//// メンバー関数
private :
	// ワーカースレッドとして動作させないため、関係関数を非公開に変更

	StatusManager*	mcls_pStatus;										// ステータス管理クラス

	int				ThreadFirst()				{return 0;}				// スレッド開始前処理 (スレッド実行直後(シグナルセット前)にコールバック)
	int				ThreadLast()				{return 0;}				// スレッド終了前処理 (終了シグナル後にコールバック)
    void			ThreadEvent(int nEventNo)	{;}						// スレッドイベント発生

	int				ThreadMainLoop();

	void			SetLightVal(int camset, int index, BYTE val);		// 式差用LED照明光量設定 ※式差のみ
	void			SetLightOnOff(int cam, int index, int onoff);		// 式差用LED照明onoff設定 ※式差のみ
	void			SetAGC(int camset, int ro);							// 露光時間設定
	void			SetCameraChange(int hantei, int cam);				// カメラ切替設定 ※カメラリンクスイッチのみ

};

