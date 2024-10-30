// *********************************************************************************
//	PC・タスク制御クラス
//	[Ver]
//		Ver.01    2007/01/30  vs2005 対応
//
//	[メモ]
//
// *********************************************************************************

#pragma once

#include <windows.h>
#include <shellapi.h>
#include "..\..\..\include\KizuCommon.h"									// 疵検共通ヘッダー
#include "KizuFunction.h"													// サブルーチン集

namespace KizuLib
{
	class ProcessManager
	{
	public :
		enum { P_POWEROFF=1, P_REBOOT, P_LOGOFF, P_NOTHING };

	private:
		#define KIZULIB_PROCESSMANAGER_SESSION				"EXECUTE"			// iniファイルセッション
		 //const char * KIZULIB_PROCESSMANAGER_SESSION = "EXECUTE";
	
	public:
	//	ProcessManager(void);
	//	virtual ~ProcessManager(void);

		static void AllProcessSend(COMMON_QUE* que);							// 常駐プロセス全員にメールを送信
		static void ExecProcess();												// 常駐プロセス起動関数
		static void ExitProcess();												// 常駐プロセス終了関数
		static void WinExec(char* strInit);										// 他プロセス作成処理

		static int ExitWin(int status);											// Windows終了処理
		static BOOL ksAdToken(LPCSTR lpName);									// 指定したアクセストークン内の特権を有効にする
		static void ProcessExec(char const* cCmd, int flg=SW_HIDE);				// コマンドを実行する
		static int  ProcessExecWait(char const* cCmd, int flg=SW_HIDE);			// コマンドを実行する、で終了まで待つ
		static int  ProcessExecWaitAns(char const* cCmd, char* cAns, int flg=SW_HIDE);		// コマンドを実行する、 結果を抽出、で終了まで待つ
		static void ShellExec(char const* cCmd, char const* cPrm, int flg=SW_HIDE, char const* cExe="open"); // ファイル名を指定して実行 と同一動作

	};
};