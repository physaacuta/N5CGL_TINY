// *********************************************************************************
//	Wake On Lan �N���X
//	[Ver]
//		Ver.01    2007/01/30  vs2005 �Ή�
//
//	[����]
//
// *********************************************************************************

#pragma once
#pragma warning(disable : 4996)							// ***���Â��`���Ƃ��Đ錾����܂����B���o���Ȃ��΍�

#include <windows.h>

namespace KizuLib
{
	class MagicPacket  
	{
	private:

		#define KIZULIB_MAGICPACKET_SESSION						"MAGICPACKET"	// ini�t�@�C���Z�b�V����
		static const int KIZULIB_MAGICPACKET_MAXSENDPC			= 50;			// �ő呗�MPC��

		// �}�W�b�N�p�P�b�g �����p
		static const int KIZULIB_MAGICPACKET_PORT_NO			= 8000;			// WOL���M�|�[�g
		static const int KIZULIB_MAGICPACKET_REFRAIN			= 16;			// WOL���}�b�N�A�h���X�J��Ԃ���
		static const int KIZULIB_MAGICPACKET_SIZEOF_SYNC		= 6;			// FF�̃T�C�Y
		static const int KIZULIB_MAGICPACKET_SIZEOF_MAC_ADDRESS	= 6;			// �}�W�b�N�p�P�b�g��(10�i�̂�)
			

	public:
		MagicPacket();
		virtual ~MagicPacket();

		static int Send(LPCSTR cIniPath);							// ���M

		static bool Mac_FFtoDec(LPCSTR code, char* ans);			// MAC�A�h���X(FF:FF:FF:FF:FF:FF) �̌`������ BYTE[6]��10�i�̌`���ɕϊ�
		static BYTE HexToDec(LPCSTR code);							// �����P�O�i�ϊ��֐�	
		static BYTE HexToDec(char code);							// �����P�O�i�ϊ��֐�
		static void makeMagicPacket(const char* cBuf, char* cWOL);	// �}�W�b�N�p�P�b�g�쐬

	};
};
