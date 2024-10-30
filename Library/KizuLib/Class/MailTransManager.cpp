#include "stdafx.h"
#include <windows.h>
#include <WinSock2.h>
#include "MailTransManager.h"

#include "MailSlotManager.h"	//for sendmail
#include "..\..\..\include\KizuCommon.h"	//for common_que

using namespace KizuLib;

MailTransManager::MailTransManager()
{
}


MailTransManager::~MailTransManager()
{
}

//------------------------------------------
// 送信
// LPCSTR SendSlotName 送信メールスロット名称
// LPCSTR SendPcName 送信PC名称
// LPCVOID pDate 送信情報
// DWORD len 送信バイト数 (MAX400byte)
// 戻り値 復帰情報
//------------------------------------------
int MailTransManager::SendMailTrans(LPCSTR SendSlotName, LPCSTR SendPcName, LPCVOID pDate, DWORD len)
{
	char SendUdpAddr[256];
	GetPrivateProfileString(KIZULIB_MAILTR, KIZULIB_DEFAULT_UDPADDR,  "", SendUdpAddr, sizeof(SendUdpAddr), TASKINI_NAME);

	return SendMailTrans(SendUdpAddr, SendSlotName, SendPcName, pDate, len);
}

//------------------------------------------
// 送信
// LPCSTR SendUdpAddr 送信先IPアドレス
// LPCSTR SendSlotName 送信メールスロット名称
// LPCSTR SendPcName 送信PC名称
// LPCVOID pDate 送信情報
// DWORD len 送信バイト数 (MAX400byte)
// 戻り値 復帰情報
//------------------------------------------
int MailTransManager::SendMailTrans(LPCSTR SendUdpAddr, LPCSTR SendSlotName, LPCSTR SendPcName, LPCVOID pDate, DWORD len)
{
	if (!SendUdpAddr || SendUdpAddr[0] == 0) {
		//相手がいない！
		return -1;
	}

	COMMON_QUE que;
	que.nEventNo = FACT_KS_MAILTR_01;
	que.nLineNo = LINE_ID;

	que.mailtr_que.nUdpIpAddr = inet_addr(SendUdpAddr);
	strncpy(que.mailtr_que.cTaskName, SendSlotName, sizeof(que.mailtr_que.cTaskName));
	strncpy(que.mailtr_que.cPcName, SendPcName, sizeof(que.mailtr_que.cPcName));

	//切り捨てられるのは仕方ない。
	memcpy(que.mailtr_que.cMsg, pDate, min(sizeof(que.mailtr_que.cMsg), len));

	// メールスロット転送タスクに通知
	return MailSlotManager::SendMail(KS_MAILTR, "*", &que, sizeof(que));
}
