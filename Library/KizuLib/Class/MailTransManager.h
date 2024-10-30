// *********************************************************************************
//	MailTrans クラス
//	[Ver]
//		Ver.01    2018/01/11  新規作成
//
//	[メモ]
//
// *********************************************************************************

#pragma once

namespace KizuLib
{
	class MailTransManager
	{
	private:
		MailTransManager();
		~MailTransManager();

	public:
		#define KIZULIB_MAILTR			"KS_MAILTR"				// iniセクション名称
		#define KIZULIB_DEFAULT_UDPADDR "DEFAULT_UDP_ADDR"		// iniキー名称(デフォルト送信先のIPアドレス)

		static int SendMailTrans(LPCSTR SendSlotName, LPCSTR SendPcName, LPCVOID pDate, DWORD len);							// メールスロット転送
		static int SendMailTrans(LPCSTR SendUdpAddr, LPCSTR SendSlotName, LPCSTR SendPcName, LPCVOID pDate, DWORD len);		// メールスロット転送
	};

};