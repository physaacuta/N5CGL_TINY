#include "StdAfx.h"
#include "ProcessManager.h"

#include <stdio.h>
#include <stdlib.h>
#include "..\..\..\Include\KizuCommon.h"					// 疵検共通インクルード
#include "..\KizuLib.h"										// 疵検ライブラリインクルード

using namespace KizuLib;
/*
//------------------------------------------
// コンストラクタ
//------------------------------------------
ProcessManager::ProcessManager(void)
{
}

//------------------------------------------
// デストラクタ
//------------------------------------------
ProcessManager::~ProcessManager(void)
{
}
*/

//------------------------------------------
// 常駐プロセス全員にメールを送信
// COMMON_QUE* que 送信メール内容
//------------------------------------------
void ProcessManager::AllProcessSend(COMMON_QUE* que)
{
	char cKey[64];
	char cWk[256];
	int nNum = 0;
	while(true) {
		nNum++;
		// タスク名称取得
		sprintf( cKey, "TASK%d", nNum);			// Key生成
		GetPrivateProfileString(KIZULIB_PROCESSMANAGER_SESSION, cKey, "", cWk, sizeof(cWk), TASKINI_NAME);
		
		// タスク終了通知
		if( 0 == strlen(cWk) ) {
			break;
		} else {
			char* taskName;
			char const strDelimit[]   = ",\n";			// デリミタ文字群
			//char * next_token;
			//taskName = strtok_s( cWk, strDelimit, &next_token);
			taskName = strtok( cWk, strDelimit);
			send_mail(taskName, ".", que);
		}
	}
}

//------------------------------------------
// 常駐プロセス起動関数
//------------------------------------------
void ProcessManager::ExecProcess()
{
	char cKey[64];
	char cWk[256];
	int nNum = 0;
	while(true) {
		nNum++;
		// タスク名称取得
		sprintf( cKey, "TASK%d", nNum);			// Key生成
		GetPrivateProfileString(KIZULIB_PROCESSMANAGER_SESSION, cKey, "", cWk, sizeof(cWk), TASKINI_NAME);
		
		// タスク実行
		if( 0 == strlen(cWk) ) break;
		else				   WinExec(cWk);
	}
}

//------------------------------------------
// 常駐プロセス終了関数
//------------------------------------------
void ProcessManager::ExitProcess()
{
	COMMON_QUE que;
	que.nEventNo = FACT_KS_END;
	que.nLineNo = LINE_ID;

	AllProcessSend(&que);
/*
	char cKey[64];
	char cWk[256];
	int nNum = 0;
	while(true) {
		nNum++;
		// タスク名称取得
		sprintf( cKey, "TASK%d", nNum);			// Key生成
		GetPrivateProfileString(KIZULIB_PROCESSMANAGER_SESSION, cKey, "", cWk, sizeof(cWk), TASKINI_NAME);
		
		// タスク終了通知
		if( 0 == strlen(cWk) ) {
			break;
		} else {
			char* taskName;
			char const strDelimit[]   = ",\n";			// デリミタ文字群
			char * next_token;
			taskName = strtok( cWk, strDelimit, &next_token);
			send_mail(taskName, ".", &que);
		}
	}
*/
}

//------------------------------------------
// 他プロセス作成処理
// char* strInit 生成タスクパス (タスクパス 引数(無しでもOK), ウィンドウフラグ)の形式
//------------------------------------------
void ProcessManager::WinExec(char* strInit)
{
	STARTUPINFO	si;
	PROCESS_INFORMATION pi;
	char * taskPath;							// 起動タスクパス
	char * show_window_flg;						// ウィンドウフラグ 
	int	flg;									// ウィンドウフラグ 
	char const strDelimit[]   = ",\n";			// デリミタ文字群
	char * wkPath;								// ワークパス


	//// 分割
//	taskPath = strtok( strInit, strDelimit, &next_token);
//	show_window_flg = strtok( NULL, strDelimit, &next_token);
	taskPath = strtok( strInit, strDelimit);
	show_window_flg = strtok( NULL, strDelimit);
	flg = atoi(show_window_flg);

	// ホントの実行Exeがあるなら変更する
	wkPath = strtok( NULL, strDelimit);
	if( NULL != wkPath) {
		KizuFunction::Trim(wkPath, 0);	// トリムが入ると正常にタスクを起動できない対策
		taskPath = wkPath;
	}


	//// タスク起動準備
	si.cb = sizeof(si);
	si.dwFlags = STARTF_USESHOWWINDOW;		// ウインドウ作成フラグ
	if( show_window_flg == NULL ){
		si.wShowWindow = SW_SHOWNORMAL;		//通常
								
	}else{

		// ウインドウ表示スタイル
		switch( flg )
		{
			case 0:			//通常
				si.wShowWindow = SW_SHOWNORMAL;  										
				break;

			case 1:			//最小化
				si.wShowWindow = SW_SHOWMINIMIZED;  										
				break;

			case 2:			//最大化
				si.wShowWindow = SW_SHOWMAXIMIZED;  										
				break;

			case 3:			//非表示
				si.wShowWindow = SW_HIDE;  										
				break;

			default:			//通常
				si.wShowWindow = SW_SHOWNORMAL;  										
				break;
		}
	}
	si.lpReserved = NULL;		// 予約済み、NULLでなければならない
	si.lpDesktop = NULL;		// デスクトップ名
	si.cbReserved2 = 0;			// 予約済み、0でなければならない
	si.lpReserved2 = NULL;		// 予約済み、NULLでなければならない
	si.lpTitle = NULL;			// ウインドウスタイル

    //// プロセス起動
	CreateProcess( NULL, taskPath, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi );
	CloseHandle(pi.hProcess);	// プロセスハンドルクローズ
	CloseHandle(pi.hThread);	// プロセス・スレッド・ハンドルクローズ

	return;
}


//------------------------------------------
// Windows終了処理
// int status P_POWEROFF:シャットダウン(1)  P_REBOOT:再起動(2)
//------------------------------------------
int ProcessManager::ExitWin(int status)
{
	// 指定したアクセストークン内の特権を有効にする
	ksAdToken(SE_SHUTDOWN_NAME);
	ksAdToken(SE_SYSTEMTIME_NAME);

	Sleep(3000);

	// 再起動、シャットダウン
	switch(status) {
	case P_POWEROFF:

		if( !ExitWindowsEx((EWX_POWEROFF | EWX_FORCE), 0) )
			// シャットダウン失敗
			return -1;
		break;

	case P_REBOOT:
		if( !ExitWindowsEx((EWX_REBOOT | EWX_FORCE), 0) )
			// 再起動失敗
			return -2;
		break;

	case P_LOGOFF:
		if( !ExitWindowsEx((EWX_LOGOFF | EWX_FORCE), 0) )
			// ログオフ失敗
			return -3;
		break;

	default:
		break;
	}
	return 0;
}

//------------------------------------------
// 指定したアクセストークン内の特権を有効にする
// LPCSTR lpName 名称
// 戻り値 復帰情報
//------------------------------------------
BOOL ProcessManager::ksAdToken(LPCSTR lpName)
{
	// プロセスのアクセストークンを 
	// TOKEN_QUERY | TOKEN_ADJUST_PRIVILEGES を指定して Open する。
	HANDLE hToken;			// ハンドル
	TOKEN_PRIVILEGES tp;	// 特権情報をtpにする

	if(!OpenProcessToken( GetCurrentProcess(), TOKEN_QUERY | TOKEN_ADJUST_PRIVILEGES, &hToken)) {
		return FALSE;
	}

	// SE_SYSTEM_NAME の LUID を取得する。
	LookupPrivilegeValue(NULL, lpName, &tp.Privileges[0].Luid);

	// SE_SYSTEM_NAME 特権を SE_PRIVILEGE_ENABLED 属性で ENABLE とする。
	tp.PrivilegeCount = 1;	//特権ユーザーは１である
	tp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
	AdjustTokenPrivileges(hToken, FALSE, &tp, 0, NULL, 0);
	return TRUE;
}

//------------------------------------------
// コマンドを実行する
// char const* cCmd コマンド
// int flg 起動状態 (0:非表示 1:通常 2:最小化 3:最大化)
//------------------------------------------
void ProcessManager::ProcessExec(char const* cCmd, int flg)
{
	STARTUPINFO si;
	PROCESS_INFORMATION pi;
	GetStartupInfo(&si);
	si.dwFlags = flg;			// SW_HIDE

	DWORD mode = NORMAL_PRIORITY_CLASS;
	if(0 == flg) mode |= CREATE_NO_WINDOW;

	// プロセス起動
	CreateProcess(NULL, (LPSTR)cCmd, NULL, NULL, FALSE, mode, NULL, NULL, &si, &pi);
	CloseHandle(pi.hProcess);	// プロセスハンドルクローズ
	CloseHandle(pi.hThread);	// プロセス・スレッド・ハンドルクローズ
}

//------------------------------------------
// コマンドを実行する、で終了まで待つ
// char const* cCmd コマンド
// int flg 起動状態 (0:非表示 1:通常 2:最小化 3:最大化)
//------------------------------------------
int ProcessManager::ProcessExecWait(char const* cCmd, int flg)
{
	STARTUPINFO si;
	PROCESS_INFORMATION pi;
	GetStartupInfo(&si);
	si.dwFlags = flg;			// SW_HIDE

	DWORD mode = NORMAL_PRIORITY_CLASS;
	if(0 == flg) mode |= CREATE_NO_WINDOW;

	// プロセス起動
	if( !CreateProcess(NULL, (LPSTR)cCmd, NULL, NULL, FALSE, mode, NULL, NULL, &si, &pi) ) {
		return -99;
	}

	// プロセスの終了を待つ
	WaitForSingleObject( pi.hProcess, INFINITE );

	// 終了コードの取得
	DWORD  exitcode = 0;
	GetExitCodeProcess( pi.hProcess, &exitcode);

	// 後始末
	CloseHandle(pi.hProcess);	// プロセスハンドルクローズ
	CloseHandle(pi.hThread);	// プロセス・スレッド・ハンドルクローズ
	return (int)exitcode;
}

//------------------------------------------
// コマンドを実行する、 結果を抽出、で終了まで待つ
// char const* cCmd コマンド
// int flg 起動状態 (0:非表示 1:通常 2:最小化 3:最大化)
// char* cAns 結果文字列
//------------------------------------------
int ProcessManager::ProcessExecWaitAns(char const* cCmd, char* cAns, int flg)
{
	int er = 0;
	// 取得用
	HANDLE hReadPipe = 0;
	HANDLE hWritePipe = 0;
	SECURITY_ATTRIBUTES saInherit;
	memset(&saInherit, 0x00, sizeof(saInherit));
	saInherit.bInheritHandle = TRUE;

	if( 0 == CreatePipe(&hReadPipe, &hWritePipe, &saInherit, 0) ) return -2;


	STARTUPINFO si;
	PROCESS_INFORMATION pi;
	GetStartupInfo(&si);
	si.dwFlags = flg;			// SW_HIDE
	si.dwFlags = si.dwFlags | STARTF_USESTDHANDLES | STARTF_USESHOWWINDOW;
	si.hStdInput = GetStdHandle(STD_INPUT_HANDLE);
	si.hStdOutput = hWritePipe;
	//si.hStdError= hWritePipe;			// エラーメッセージを出力して調査する場合は有効化

	DWORD mode = NORMAL_PRIORITY_CLASS;
	if(0 == flg) mode |= CREATE_NO_WINDOW;


	// プロセス起動
	BOOL bRet = CreateProcess(NULL, (LPSTR)cCmd, NULL, NULL, TRUE, mode, NULL, NULL, &si, &pi);
	er = GetLastError();
	CloseHandle(hWritePipe);

	if( ! bRet ) {
		CloseHandle(hReadPipe);
		return -er;
	}


	// 結果抽出
	int nLength = 0;
	while (1) {
		DWORD nRead=0;

		if( ::ReadFile(hReadPipe, &cAns[nLength], 2048, &nRead, 0) == 0 ) break;
		nLength += nRead;
	}
	CloseHandle(hReadPipe);


	// プロセスの終了を待つ
	WaitForSingleObject( pi.hProcess, INFINITE );

	// 終了コードの取得
	DWORD  exitcode = 0;
	GetExitCodeProcess( pi.hProcess, &exitcode);

	// 後始末
	CloseHandle(pi.hProcess);	// プロセスハンドルクローズ
	CloseHandle(pi.hThread);	// プロセス・スレッド・ハンドルクローズ
	return (int)exitcode;
}



//------------------------------------------
// ファイル名を指定して実行 と同一動作
// char const* cCmd コマンド
// char const* cPrm パラメータ部分
// int flg 起動状態 (0:非表示 1:通常 2:最小化 3:最大化)
// char const* cExe 実行形式
//------------------------------------------
void ProcessManager::ShellExec(char const* cCmd, char const* cPrm, int flg, char const* cExe)
{
	ShellExecute( NULL, cExe, cCmd, cPrm, NULL, flg);
}

