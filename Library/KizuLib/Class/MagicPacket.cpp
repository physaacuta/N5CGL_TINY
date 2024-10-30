// MagicPacket.cpp: MagicPacket �N���X�̃C���v�������e�[�V����
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "MagicPacket.h"

#include <winsock2.h>
#include <stdio.h>
#include "..\..\..\Include\KizuCommon.h"					// �r�����ʃC���N���[�h

using namespace KizuLib;

//////////////////////////////////////////////////////////////////////
// �\�z/����
//////////////////////////////////////////////////////////////////////

//------------------------------------------
// �R���X�g���N�^
//------------------------------------------
MagicPacket::MagicPacket()
{

}
//------------------------------------------
// �f�X�g���N�^
//------------------------------------------
MagicPacket::~MagicPacket()
{

}

//------------------------------------------
// MAC�A�h���X(FF:FF:FF:FF:FF:FF) �̌`������ BYTE[6]��10�i�̌`���ɕϊ�
// LPCSTR code 16�i 2���� (00�`FF) (17����)
// char* ans 10�i (6�o�C�g)
// ���A���
//------------------------------------------
bool MagicPacket::Mac_FFtoDec(LPCSTR code, char* ans)
{
	BYTE nWk;
	for(int ii=0; ii<KIZULIB_MAGICPACKET_SIZEOF_MAC_ADDRESS; ii++) {
		nWk = HexToDec(&code[ii*3]);
		if(-1 == nWk) {
			return false;
		}
		ans[ii] = nWk;
	}
	return true;
}

//------------------------------------------
// �����P�O�i�ϊ��֐�
// LPCSTR code 16�i 2���� (00�`FF)
// �߂�l 10�i��
//------------------------------------------
BYTE MagicPacket::HexToDec(LPCSTR code)
{
     // �ϊ��������[�N  �ꌅ/�񌅖� 
     BYTE work_H = 0;				// ��ʃ��[�h
     BYTE work_L = 0;				// ���ʃ��[�h

     // �����ϊ� 
     work_H = HexToDec(code[0]);
     work_L = HexToDec(code[1]);
     // ����`�F�b�N
     if( -1 == work_H || -1 == work_L ){   
         return -1;
     }
     // �P�O�i�ϊ��l��Ԃ� 
     return (16 * work_H) + work_L;
}

//------------------------------------------
// �����P�O�i�ϊ��֐�
// char code 16�i 1���� (0�`F)
// �߂�l 10�i��
//------------------------------------------
BYTE MagicPacket::HexToDec(char code)
{
     BYTE ans;  // �߂�l 
     // �������� 
     switch( code ) {
            // �����ϊ�
            case '0': case '1': case '2': case '3':
            case '4': case '5': case '6': case '7':
            case '8': case '9':
                ans = code - 0x30;
                break;
            // �������ϊ� 
            case 'a': case 'b': case 'c': case 'd':
            case 'e': case 'f':
                ans = code - 0x57;
                break;
            // �啶���ϊ� 
            case 'A': case 'B': case 'C': case 'D':
            case 'E': case 'F':
                ans = code - 0x37;
                break;
            // �ٕ��� 
            default:
                ans = -1;
                break;
     }
     // �ϊ�������Ԃ�
     return  ans;
}

//------------------------------------------
// �}�W�b�N�p�P�b�g�쐬
// char* cBuf �}�b�N�A�h���X10�i (6�o�C�g)
// char* cWOL WOL���M�p�P�b�g (0xff�~6��{MAC�A�h���X�i6�o�C�g�j�~16��̌v102�o�C�g)
//------------------------------------------
void MagicPacket::makeMagicPacket(const char* cBuf, char* cWOL)
{
	int ii;

	// ff 6��
	for(ii=0; ii<KIZULIB_MAGICPACKET_SIZEOF_SYNC; ii++) cWOL[ii] = (char)0xff;
	// MAC�A�h���X 16��
	for(ii=0; ii<KIZULIB_MAGICPACKET_REFRAIN; ii++) {
		//memcpy( &cWOL[6 + ii*6], cBuf, 6);
		memcpy( &cWOL[KIZULIB_MAGICPACKET_SIZEOF_SYNC + ii*KIZULIB_MAGICPACKET_SIZEOF_MAC_ADDRESS], cBuf, KIZULIB_MAGICPACKET_SIZEOF_MAC_ADDRESS);
	}
}

//------------------------------------------
// WOL���M
// LPCSTR cIniPath ini�t�@�C���t���p�X
// �߂�l ���A���
//------------------------------------------
// �g�pini
// [MAGICPACKET] �Z�N�V����
// B_ADDR �u���[�h�L���X�g�A�h���X
// SEND_CNT ���MPC�䐔
// PC%d PC�}�b�N�A�h���X ("ff:ff:ff:ff:ff:ff"�̌`��)
//------------------------------------------
int MagicPacket::Send(LPCSTR cIniPath)
{
	//// UDP SOCKET����
#ifdef WIN32
	WSADATA wsaData;
	int      err;
	WORD wVersionRequested = MAKEWORD(1, 1); 
	err = WSAStartup(wVersionRequested, &wsaData); 
	if (err != 0) {
		return -1;		// A useable winsock.dll not found.\n 
	}

	if ( LOBYTE( wsaData.wVersion ) != 1 || HIBYTE( wsaData.wVersion ) != 1 ) { 
		// Tell the user that we couldn't find a useable
		// winsock.dll.
		WSACleanup(); 
		return -2;		// A useable winsock.dll not found.\n 
	} 
#endif

	int ii;
	char cWk[256];
	char cKey[64];
	int nSendPc_Cnt;										// PC�䐔
	bool bSendFlg[KIZULIB_MAGICPACKET_MAXSENDPC];			// ���M�L���t���O
	char cPc_Mac[KIZULIB_MAGICPACKET_MAXSENDPC][KIZULIB_MAGICPACKET_SIZEOF_MAC_ADDRESS];		// PC Mac�A�h���X (10�i)
	char cBroadcast_Addr[64];								// �u���[�h�L���X�g�A�h���X (10�i) 

	SOCKET sd;												// Socket�L�q�q
	struct sockaddr_in sv_addr;								// ���葤���


	//// INI�t�@�C�����o�b�䐔���擾����B
	nSendPc_Cnt = GetPrivateProfileInt(KIZULIB_MAGICPACKET_SESSION, "SEND_CNT", -1, cIniPath);
	if( 0 >= nSendPc_Cnt ) {
		return -3;
	}

	//// INI�t�@�C������̏��擾���s���B
	// MAC�A�h���X�擾
	memset(cPc_Mac, 0x00, sizeof(cPc_Mac));
	for( ii=0; ii < nSendPc_Cnt; ii++ ) {
		sprintf( cKey, "PC%d", ii+1);			// Key����
		GetPrivateProfileString(KIZULIB_MAGICPACKET_SESSION, cKey, "", cWk, sizeof(cWk), cIniPath);
		// 16�i��10�i
		if( 0 != strlen(cWk) ) {
			bSendFlg[ii] = Mac_FFtoDec(cWk, cPc_Mac[ii] );
		} else {
			bSendFlg[ii] = false;
		}
	}

	//// INI�t�@�C�����u���[�h�L���X�g�A�h���X�擾
	GetPrivateProfileString(KIZULIB_MAGICPACKET_SESSION, "B_ADDR", "", cWk, sizeof(cWk), cIniPath);
	if( 0 == strlen(cWk) ) return -3;
	memcpy(cBroadcast_Addr, cWk, 18);



    //// �\�P�b�g�����iOpen the UDP socket�j
	//sd = socket(AF_INET, SOCK_DGRAM, 0 );
	sd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP );
	if(INVALID_SOCKET == sd) {
		WSACleanup(); 
		return  -4;
	}
    //// �u���[�h�L���X�g���\�ɂ����� 
#ifdef WIN32
	BOOL t = TRUE;  /* socket option value (always TRUE in this program) */
#else
    int  t = 1;     /* socket option value (always 1 in this program)    */
#endif
	if( setsockopt(sd, SOL_SOCKET, SO_BROADCAST, (char *)&t, sizeof(t) ) == SOCKET_ERROR ) {
		WSACleanup();
		return -5;
    }
	//// ���葤��address��ݒ肷�� 
    memset( &sv_addr, 0x00, sizeof(sv_addr));
    sv_addr.sin_family = AF_INET;
    sv_addr.sin_addr.s_addr = inet_addr(cBroadcast_Addr);
    sv_addr.sin_port = htons((u_short)KIZULIB_MAGICPACKET_PORT_NO);



	//// ���M
	for( ii=0; ii < nSendPc_Cnt; ii++ ) {
		if( ! bSendFlg[ii]) continue;
		// WOL����
		memset(cWk, 0x00, sizeof(cWk));
		makeMagicPacket(cPc_Mac[ii], cWk);
		// �z���g�̑��M
		Sleep(100);
		// �ҏW�f�[�^���M���� 
		if(sendto( sd, (const char *)cWk, 
			KIZULIB_MAGICPACKET_REFRAIN*KIZULIB_MAGICPACKET_SIZEOF_MAC_ADDRESS + KIZULIB_MAGICPACKET_SIZEOF_SYNC, 
			0, (sockaddr*)&sv_addr, sizeof(sv_addr) )==SOCKET_ERROR){
			return -6;
		}
	}

	// �N���[�Y
	if (sd >= 0){
#ifdef WIN32
		closesocket(sd);
#else
		close(sd);
#endif
	}
	return 0;
}
