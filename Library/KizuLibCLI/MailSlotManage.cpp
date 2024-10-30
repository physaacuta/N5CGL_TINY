// これは メイン DLL ファイルです。

#include "stdafx.h"

#include "MailSlotManage.h"
/*
#include "..\KizuLib\KizuLib.h"

using namespace System;
using namespace System::Runtime::InteropServices; 

using namespace KizuLibCLI;

//using namespace KizuLib;

/*

int task_init(String^ cTaskName);										// タスク生成
int task_init(String^ cTaskName, String^ cPcName);						// タスク生成
void task_exit();														// タスク終了
*/
/*
//------------------------------------------
// タスク生成
// String cTaskName 自タスク名称
// 戻り値 復帰情報
//------------------------------------------
int MailSlotManage::task_init(String^ cTaskName)
{
	IntPtr pTaskName = Marshal::StringToHGlobalAnsi(cTaskName);			// ansicに変換後の文字ポインタ
	int retc = KizuLib::task_init((const char*)pTaskName.ToPointer());
	Marshal::FreeHGlobal(pTaskName);
	return retc;
}

*/
