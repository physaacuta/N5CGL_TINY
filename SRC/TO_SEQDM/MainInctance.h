// *********************************************************************************
//	メイン管理スレッド
//	[Ver]
//		Ver.01    2007/01/30  vs2005 対応
//
//	[メモ]
//		
// *********************************************************************************


// *********************************************************************************
//	当タスクの基本定数
//	[Ver]
//		Ver.01    2007/08/03  vs2005 対応
//
//	[メモ]
//	○ MXコンポーネントがインストールされている端末で修正を行ってください。
//     インストールされていない場合は、CDからインストールを行ってください。
//
//	○ シーケンサーのユーティリティをインストールを行ってください。
//     ※ 通信設定を行い、接続確認を行ってください。
//     ※ 論理局番をコンポーネントの設定使用します。
//     ※ マニュアルはMELSEC\Manual内のsh080274i.pdfを参照してください
//
//	○ シーケンサーのCPUタイプ、通信手順(TCP,UDP,COM等)によって画面に張付けるコントロールと
//     設定するCPUNOが変わるので、通信に使用するシーケンサのCPUタイプを確認して下さい。
//     ※ N5CGL設定値 … コンポーネント：ACTQJ71E71UDP
//     ※ CDに付属されているマニュアルを参照すること
//        MELSEC\Manual内のsh080275f.pdfを参照
//        ３章ACTコントロールの詳細 P.51  … 使用するコントロール
//        ３章ACTコントロールの詳細 P.52  … CPUNO
//
//	○ MXコンポーネントの設定	
//	   ・シーケンサーとの設定は、コントロールに持っているので変更を行う場合は注意すること	
//		（CPUタイプ、通信タイムアウト値、IPアドレス、ポートNO)
//     ※ N5CGL設定値 … コンポーネント名					： ACTQJ71E71UDP
//                       論理局番(ActLogicalStationNumber	： 1
//                       CPUNO（ActCpuType)					： 35
//                       タイムアウト(ActTimeOut)	[m秒]	：			(プログラム内で再設定)
//                       IPアドレス(ActHostAddress)　		：			(プログラム内で再設定)
//						 ポートNO(ActPortNumber)			： 5001		(プログラム内で再設定)
//                       他の項目はデフォルト設定
//
//	  ・新規で作成した場合、コントロールがツールボックスに表示されない為	
//	    手動でコントロールを読込必要がある。	
//	    このとき多数のコントロールが表示されるので、必要なコントロールだけを読み込む必要があります。	
//
//	○ コントロールを画面に張付ける場合は、コンポーネントのOPENをOnInitDialogでする必要があります。
//
//  ○ コンポーネントのOPEN処理をOnInitDialogで行っている為、
//     再度OPENすることが出来ないので、タスクを再起動する必要があります。
//
//  ○ MXコンポーネントの関数からのエラーコードはHEXで確認してください。
//     ６章 エラーコード P.232
//
//  ★ VS2015 から　リンカー/詳細設定/データ実行防止(DEP) が デフォルトON に変更されている
//     それに伴い、MXコンポーネントのDLL内で落ちるため、 NOに変更が必要
//
// *********************************************************************************

#pragma once


// インクルード
#include <process.h>

// 疵検 インクルード
#include "..\..\include\KizuCommon.h"									// 疵検共通ヘッダー
#include "..\..\Library\KizuLib\KizuLib.h"								// 疵検ライブラリヘッダー

// 疵検クラス インクルード
#include "..\..\Library\KizuLib\CLASS\KizuMacro.h"						// 疵検マクロ
#include "..\..\Library\KizuLib\CLASS\WorkThread.h"						// ワーカースレッド
#include "..\..\Library\KizuLibMFC\CLASS\LogFileManager.h"				// ログマネージャー

// 疵検ライブラリ インクルード
//#include "..\..\Library\KizuODBC\OdbcBase.h"							// ODBC操作クラス


// 各ワーカースレッド
#include "DMemManager.h"


using namespace KizuLib;

class MainInctance : public WorkThread
{
public:
	MainInctance(void);
	virtual ~MainInctance(void);

	void Start() { SetEvent(m_evThStart); };							// スレッド開始
	void Stop()  { WorkThread::StopMSG(WM_USER_TH_END); };				// スレッド終了
	BOOL Init();														// 初期化処理
	void Exit();														// 終了処理
	void SetLogMgr(LogFileManager* pLog) { mcls_pLog = pLog; }			// ログファイルセット

	// 各ワーカースレッド
	DMemManager 			gcls_DMem;									// データメモリ管理クラス


private:
	static UINT WINAPI MainThread(LPVOID param);						// メインスレッド
	void testsub();

	//// 各インスタンス
	LogFileManager*			mcls_pLog;									// LogFileManagerスレッドインスタンス
	
	// 同期シグナル
	HANDLE					m_evThStart;								// MainInctanceスレッドの実行要求

	// その他もろもろ


};
