// ����� ���C�� DLL �t�@�C���ł��B

#include "stdafx.h"

#include "MailSlotManage.h"
/*
#include "..\KizuLib\KizuLib.h"

using namespace System;
using namespace System::Runtime::InteropServices; 

using namespace KizuLibCLI;

//using namespace KizuLib;

/*

int task_init(String^ cTaskName);										// �^�X�N����
int task_init(String^ cTaskName, String^ cPcName);						// �^�X�N����
void task_exit();														// �^�X�N�I��
*/
/*
//------------------------------------------
// �^�X�N����
// String cTaskName ���^�X�N����
// �߂�l ���A���
//------------------------------------------
int MailSlotManage::task_init(String^ cTaskName)
{
	IntPtr pTaskName = Marshal::StringToHGlobalAnsi(cTaskName);			// ansic�ɕϊ���̕����|�C���^
	int retc = KizuLib::task_init((const char*)pTaskName.ToPointer());
	Marshal::FreeHGlobal(pTaskName);
	return retc;
}

*/
