// *********************************************************************************
//	Wake On Lan クラス
//	[Ver]
//		Ver.01    2007/01/30  vs2005 対応
//
//	[メモ]
//
// *********************************************************************************

#pragma once
#pragma warning(disable : 4996)							// ***が古い形式として宣言されました。を出さない対策

#include <windows.h>

namespace KizuLib
{
	class MagicPacket  
	{
	private:

		#define KIZULIB_MAGICPACKET_SESSION						"MAGICPACKET"	// iniファイルセッション
		static const int KIZULIB_MAGICPACKET_MAXSENDPC			= 50;			// 最大送信PC数

		// マジックパケット 生成用
		static const int KIZULIB_MAGICPACKET_PORT_NO			= 8000;			// WOL送信ポート
		static const int KIZULIB_MAGICPACKET_REFRAIN			= 16;			// WOL内マックアドレス繰り返し数
		static const int KIZULIB_MAGICPACKET_SIZEOF_SYNC		= 6;			// FFのサイズ
		static const int KIZULIB_MAGICPACKET_SIZEOF_MAC_ADDRESS	= 6;			// マジックパケット数(10進のね)
			

	public:
		MagicPacket();
		virtual ~MagicPacket();

		static int Send(LPCSTR cIniPath);							// 送信

		static bool Mac_FFtoDec(LPCSTR code, char* ans);			// MACアドレス(FF:FF:FF:FF:FF:FF) の形式から BYTE[6]の10進の形式に変換
		static BYTE HexToDec(LPCSTR code);							// 文字１０進変換関数	
		static BYTE HexToDec(char code);							// 文字１０進変換関数
		static void makeMagicPacket(const char* cBuf, char* cWOL);	// マジックパケット作成

	};
};
