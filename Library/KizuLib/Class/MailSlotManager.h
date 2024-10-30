// *********************************************************************************
//	MailSlot �N���X
//	[Ver]
//		Ver.01    2007/01/30  vs2005 �Ή�
//
//	[����]
//
// *********************************************************************************

#pragma once
#pragma warning(disable : 4996)							// ***���Â��`���Ƃ��Đ錾����܂����B���o���Ȃ��΍�

#include "ICommunity.h"										// �ʐM�֘A�̑���M���b�\�h�̃C���^�[�t�F�[�X



namespace KizuLib
{
	class MailSlotManager : implements ICommunity,
							implements ICommunityAsync
	{
	public:
		MailSlotManager();
		virtual ~MailSlotManager();

		// syslog���X��New���ĂȂ����Ɏg�p����悤
		static int SendMail(LPCSTR SendSlotName, LPCSTR SendPcName, LPCVOID sendbuff, DWORD len);	// ���M
		static int Connect(LPCSTR SendSlotName, LPCSTR SendPcName, HANDLE* hSend);					// ���M�X���b�g�ڑ�
		static void Close(HANDLE* hSend);															// �X���b�g�ؒf


		bool Create(LPCSTR SlotName, DWORD lReadTimeout = MAILSLOT_WAIT_FOREVER);					// �X���b�g�\�z


		// ICommunity�̎���
		virtual int Recv(LPVOID pDate, DWORD len);													// ��M
		virtual int Send(LPCVOID pDate, DWORD len);													// ���M

		// ICommunityAsync�̎���
		virtual bool Recv(LPVOID pDate, DWORD len, OVERLAPPED *ovl);								// ��M
		virtual bool Send(LPCVOID pDate, DWORD len, OVERLAPPED *ovl);								// ���M
		virtual bool GetResult(OVERLAPPED *ovl);													// �񓯊����ʖ₢���킹
		virtual int GetError() { return m_nLastError; }												// �ŏI�G���[�擾 (���ۃN���X���܂ވ�const�����Ⴞ��)

	private:
		HANDLE					m_hSlot;									// �X���b�g�{��
		int						m_nLastError;								// �ŏI�G���[
		static CRITICAL_SECTION	m_csLock;									// �N���e�B�J���Z�N�V���� (SendMail�̃Z�[�t�X���b�g��)
	};
};


