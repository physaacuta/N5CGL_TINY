// MagicPacket.cpp: MagicPacket クラスのインプリメンテーション
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "MagicPacket.h"

#include <winsock2.h>
#include <stdio.h>
#include "..\..\..\Include\KizuCommon.h"					// 疵検共通インクルード

using namespace KizuLib;

//////////////////////////////////////////////////////////////////////
// 構築/消滅
//////////////////////////////////////////////////////////////////////

//------------------------------------------
// コンストラクタ
//------------------------------------------
MagicPacket::MagicPacket()
{

}
//------------------------------------------
// デストラクタ
//------------------------------------------
MagicPacket::~MagicPacket()
{

}

//------------------------------------------
// MACアドレス(FF:FF:FF:FF:FF:FF) の形式から BYTE[6]の10進の形式に変換
// LPCSTR code 16進 2文字 (00～FF) (17文字)
// char* ans 10進 (6バイト)
// 復帰情報
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
// 文字１０進変換関数
// LPCSTR code 16進 2文字 (00～FF)
// 戻り値 10進数
//------------------------------------------
BYTE MagicPacket::HexToDec(LPCSTR code)
{
     // 変換文字ワーク  一桁/二桁目 
     BYTE work_H = 0;				// 上位ワード
     BYTE work_L = 0;				// 下位ワード

     // 文字変換 
     work_H = HexToDec(code[0]);
     work_L = HexToDec(code[1]);
     // 判定チェック
     if( -1 == work_H || -1 == work_L ){   
         return -1;
     }
     // １０進変換値を返す 
     return (16 * work_H) + work_L;
}

//------------------------------------------
// 文字１０進変換関数
// char code 16進 1文字 (0～F)
// 戻り値 10進数
//------------------------------------------
BYTE MagicPacket::HexToDec(char code)
{
     BYTE ans;  // 戻り値 
     // 文字判定 
     switch( code ) {
            // 数字変換
            case '0': case '1': case '2': case '3':
            case '4': case '5': case '6': case '7':
            case '8': case '9':
                ans = code - 0x30;
                break;
            // 小文字変換 
            case 'a': case 'b': case 'c': case 'd':
            case 'e': case 'f':
                ans = code - 0x57;
                break;
            // 大文字変換 
            case 'A': case 'B': case 'C': case 'D':
            case 'E': case 'F':
                ans = code - 0x37;
                break;
            // 異文字 
            default:
                ans = -1;
                break;
     }
     // 変換文字を返す
     return  ans;
}

//------------------------------------------
// マジックパケット作成
// char* cBuf マックアドレス10進 (6バイト)
// char* cWOL WOL送信パケット (0xff×6回＋MACアドレス（6バイト）×16回の計102バイト)
//------------------------------------------
void MagicPacket::makeMagicPacket(const char* cBuf, char* cWOL)
{
	int ii;

	// ff 6回
	for(ii=0; ii<KIZULIB_MAGICPACKET_SIZEOF_SYNC; ii++) cWOL[ii] = (char)0xff;
	// MACアドレス 16回
	for(ii=0; ii<KIZULIB_MAGICPACKET_REFRAIN; ii++) {
		//memcpy( &cWOL[6 + ii*6], cBuf, 6);
		memcpy( &cWOL[KIZULIB_MAGICPACKET_SIZEOF_SYNC + ii*KIZULIB_MAGICPACKET_SIZEOF_MAC_ADDRESS], cBuf, KIZULIB_MAGICPACKET_SIZEOF_MAC_ADDRESS);
	}
}

//------------------------------------------
// WOL送信
// LPCSTR cIniPath iniファイルフルパス
// 戻り値 復帰情報
//------------------------------------------
// 使用ini
// [MAGICPACKET] セクション
// B_ADDR ブロードキャストアドレス
// SEND_CNT 送信PC台数
// PC%d PCマックアドレス ("ff:ff:ff:ff:ff:ff"の形式)
//------------------------------------------
int MagicPacket::Send(LPCSTR cIniPath)
{
	//// UDP SOCKET準備
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
	int nSendPc_Cnt;										// PC台数
	bool bSendFlg[KIZULIB_MAGICPACKET_MAXSENDPC];			// 送信有効フラグ
	char cPc_Mac[KIZULIB_MAGICPACKET_MAXSENDPC][KIZULIB_MAGICPACKET_SIZEOF_MAC_ADDRESS];		// PC Macアドレス (10進)
	char cBroadcast_Addr[64];								// ブロードキャストアドレス (10進) 

	SOCKET sd;												// Socket記述子
	struct sockaddr_in sv_addr;								// 相手側情報


	//// INIファイルよりＰＣ台数を取得する。
	nSendPc_Cnt = GetPrivateProfileInt(KIZULIB_MAGICPACKET_SESSION, "SEND_CNT", -1, cIniPath);
	if( 0 >= nSendPc_Cnt ) {
		return -3;
	}

	//// INIファイルからの情報取得を行う。
	// MACアドレス取得
	memset(cPc_Mac, 0x00, sizeof(cPc_Mac));
	for( ii=0; ii < nSendPc_Cnt; ii++ ) {
		sprintf( cKey, "PC%d", ii+1);			// Key生成
		GetPrivateProfileString(KIZULIB_MAGICPACKET_SESSION, cKey, "", cWk, sizeof(cWk), cIniPath);
		// 16進→10進
		if( 0 != strlen(cWk) ) {
			bSendFlg[ii] = Mac_FFtoDec(cWk, cPc_Mac[ii] );
		} else {
			bSendFlg[ii] = false;
		}
	}

	//// INIファイルよりブロードキャストアドレス取得
	GetPrivateProfileString(KIZULIB_MAGICPACKET_SESSION, "B_ADDR", "", cWk, sizeof(cWk), cIniPath);
	if( 0 == strlen(cWk) ) return -3;
	memcpy(cBroadcast_Addr, cWk, 18);



    //// ソケット生成（Open the UDP socket）
	//sd = socket(AF_INET, SOCK_DGRAM, 0 );
	sd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP );
	if(INVALID_SOCKET == sd) {
		WSACleanup(); 
		return  -4;
	}
    //// ブロードキャストを可能にさせる 
#ifdef WIN32
	BOOL t = TRUE;  /* socket option value (always TRUE in this program) */
#else
    int  t = 1;     /* socket option value (always 1 in this program)    */
#endif
	if( setsockopt(sd, SOL_SOCKET, SO_BROADCAST, (char *)&t, sizeof(t) ) == SOCKET_ERROR ) {
		WSACleanup();
		return -5;
    }
	//// 相手側のaddressを設定する 
    memset( &sv_addr, 0x00, sizeof(sv_addr));
    sv_addr.sin_family = AF_INET;
    sv_addr.sin_addr.s_addr = inet_addr(cBroadcast_Addr);
    sv_addr.sin_port = htons((u_short)KIZULIB_MAGICPACKET_PORT_NO);



	//// 送信
	for( ii=0; ii < nSendPc_Cnt; ii++ ) {
		if( ! bSendFlg[ii]) continue;
		// WOL生成
		memset(cWk, 0x00, sizeof(cWk));
		makeMagicPacket(cPc_Mac[ii], cWk);
		// ホントの送信
		Sleep(100);
		// 編集データ送信する 
		if(sendto( sd, (const char *)cWk, 
			KIZULIB_MAGICPACKET_REFRAIN*KIZULIB_MAGICPACKET_SIZEOF_MAC_ADDRESS + KIZULIB_MAGICPACKET_SIZEOF_SYNC, 
			0, (sockaddr*)&sv_addr, sizeof(sv_addr) )==SOCKET_ERROR){
			return -6;
		}
	}

	// クローズ
	if (sd >= 0){
#ifdef WIN32
		closesocket(sd);
#else
		close(sd);
#endif
	}
	return 0;
}
