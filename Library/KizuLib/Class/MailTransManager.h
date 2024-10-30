// *********************************************************************************
//	MailTrans �N���X
//	[Ver]
//		Ver.01    2018/01/11  �V�K�쐬
//
//	[����]
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
		#define KIZULIB_MAILTR			"KS_MAILTR"				// ini�Z�N�V��������
		#define KIZULIB_DEFAULT_UDPADDR "DEFAULT_UDP_ADDR"		// ini�L�[����(�f�t�H���g���M���IP�A�h���X)

		static int SendMailTrans(LPCSTR SendSlotName, LPCSTR SendPcName, LPCVOID pDate, DWORD len);							// ���[���X���b�g�]��
		static int SendMailTrans(LPCSTR SendUdpAddr, LPCSTR SendSlotName, LPCSTR SendPcName, LPCVOID pDate, DWORD len);		// ���[���X���b�g�]��
	};

};