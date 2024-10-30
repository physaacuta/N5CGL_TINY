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
// ���M
// LPCSTR SendSlotName ���M���[���X���b�g����
// LPCSTR SendPcName ���MPC����
// LPCVOID pDate ���M���
// DWORD len ���M�o�C�g�� (MAX400byte)
// �߂�l ���A���
//------------------------------------------
int MailTransManager::SendMailTrans(LPCSTR SendSlotName, LPCSTR SendPcName, LPCVOID pDate, DWORD len)
{
	char SendUdpAddr[256];
	GetPrivateProfileString(KIZULIB_MAILTR, KIZULIB_DEFAULT_UDPADDR,  "", SendUdpAddr, sizeof(SendUdpAddr), TASKINI_NAME);

	return SendMailTrans(SendUdpAddr, SendSlotName, SendPcName, pDate, len);
}

//------------------------------------------
// ���M
// LPCSTR SendUdpAddr ���M��IP�A�h���X
// LPCSTR SendSlotName ���M���[���X���b�g����
// LPCSTR SendPcName ���MPC����
// LPCVOID pDate ���M���
// DWORD len ���M�o�C�g�� (MAX400byte)
// �߂�l ���A���
//------------------------------------------
int MailTransManager::SendMailTrans(LPCSTR SendUdpAddr, LPCSTR SendSlotName, LPCSTR SendPcName, LPCVOID pDate, DWORD len)
{
	if (!SendUdpAddr || SendUdpAddr[0] == 0) {
		//���肪���Ȃ��I
		return -1;
	}

	COMMON_QUE que;
	que.nEventNo = FACT_KS_MAILTR_01;
	que.nLineNo = LINE_ID;

	que.mailtr_que.nUdpIpAddr = inet_addr(SendUdpAddr);
	strncpy(que.mailtr_que.cTaskName, SendSlotName, sizeof(que.mailtr_que.cTaskName));
	strncpy(que.mailtr_que.cPcName, SendPcName, sizeof(que.mailtr_que.cPcName));

	//�؂�̂Ă���͎̂d���Ȃ��B
	memcpy(que.mailtr_que.cMsg, pDate, min(sizeof(que.mailtr_que.cMsg), len));

	// ���[���X���b�g�]���^�X�N�ɒʒm
	return MailSlotManager::SendMail(KS_MAILTR, "*", &que, sizeof(que));
}
