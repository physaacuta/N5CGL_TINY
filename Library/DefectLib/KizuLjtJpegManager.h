// *********************************************************************************
//	libJpeg-turbo画像生成クラス
//	[Ver]
//		Ver.01    2018/09/25  libjpeg-turbo ( dll ) 対応
//
//	[メモ]
//		LibJpeg-turbo使用時は、LoadLibraryでDLLを見つけるので、コンパイル時は特にライブラリとかの参照は不要
//
////// 使用にあたり
// ・実行環境にDLLを入れる必要あり
//
/*
turbojpeg_x64.dll
turbojpeg_x86.dll
*/
// *********************************************************************************



#pragma once

#include <windows.h>

#include "..\..\Include\LibSelect.h"											// ライブラリ切替ヘッダー

namespace KizuLib
{
	class KizuLjtJpegManager
	{
	private:
		void *tjEncode;

	public:
		KizuLjtJpegManager(void);
		~KizuLjtJpegManager(void);

		int BmpToJpeg(int nW, int nH, const unsigned char* bP, int jquality, int* jSize, unsigned char* jP, bool nTB = false);		// BMP→JPEG 変換（グレー）
		int BmpToJpegColor(int nW, int nH, const unsigned char* bP, int jquality, int* jSize, unsigned char* jP, bool nTB = false);	// BMP→JPEG 変換（カラー）
		int BmpToJPEGfile(int nW, int nH, const BYTE* bP, int jquality, const char* Path, bool nTB = false);						// BMP→JPEG 変換後ファイル書き込み（グレー）


		int JpegToBmp(const unsigned char* jP, int jSize, int nW, int nH, unsigned char* bP, bool nTB = false);						// JPEG→BMP 変換（グレー）
		int JpegToBmpColor(const unsigned char* jP, int jSize, int nW, int nH, unsigned char* bP, bool nTB = false);				// JPEG→BMP 変換（カラー）
		int JpegToHeader(const unsigned char* jP, int jSize, int *nW, int *nH, bool *bColored);
	};
};
