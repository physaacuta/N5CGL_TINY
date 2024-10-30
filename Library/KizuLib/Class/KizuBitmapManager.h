// *********************************************************************************
//	Bitmap画像生成クラス
//	[Ver]
//		Ver.01    2007/01/30  vs2005 対応
//
//	[メモ]
//		・通常は、BMPファイルの先頭の方にデータをセットすると
//			画像としては、下のほうに表示される
//		・疵検やFCで作ったやつは、ヘッダで上下反対としている。データは、配列の上が、画像左上となる
// *********************************************************************************

#pragma once

#include <windows.h>

namespace KizuLib
{
	class KizuBitmapManager
	{
	public:
		KizuBitmapManager(void);												// コンストラクタ
		virtual ~KizuBitmapManager(void);										// デストラクタ
		
		//カラー画像用
		int GetBitmapHeaderSize_Color();														// Bitmapヘッダサイズ取得
		int CreateBitmapHeader_Color(int nW, int nH, BYTE* BmpHead, bool nTB = false);		// Bitmapヘッダ生成
		bool DrawText_Color(int nW, int nH, BYTE* BmpData, int nLeft, int nTop, char const* szText, int nFontSize, DWORD color);// 文字を描画


		// グレー画像用
		int GetBitmapHeaderSize_Gray();														// Bitmapヘッダサイズ取得
		int CreateBitmapHeader_Gray(int nW, int nH, BYTE* BmpHead, bool nTB = false);		// Bitmapヘッダ生成
		void DataToBMPfile_Gray(int nW, int nH, const BYTE* BmpData, const BYTE* BmpHead, const char* Path);// Bitmapファイルへ出力
		void DataToBMPfile_Gray(int nW, int nH, const BYTE* BmpData, const char* Path, bool nTB = false);	// Bitmapファイルへ出力 (Bitmapヘッダを自動で生成)
		int BMPfileToData_Gray(const char* Path, int* nW, int* nH, BYTE *BmpData);			// Bitmapファイル読み込み
		int BMPfileToData_Gray(const char* Path, int* nW, int* nH);							// Bitmapファイルのヘッダー部のみ読込み

		void DrawLine_Gray(int nW, int nH, BYTE *BmpData, int nX1, int nY1, int nX2, int nY2, BYTE color=255);	// 線を描画
		bool DrawText_Gray(int nW, int nH, BYTE* BmpData, int nLeft, int nTop, char const* szText, int nFontSize=16, BYTE color=255);	// 文字を描画


		// 2値画像用
		int GetBitmapHeaderSize_Bin();														// Bitmapヘッダサイズ取得
		int CreateBitmapHeader_Bin(int nW, int nH, BYTE* BmpHead, bool nTB = false);		// Bitmapヘッダ生成
		void DataToBMPfile_Bin(int nW, int nH, const BYTE* BmpData, const BYTE* BmpHead, const char* Path);	// Bitmapファイルへ出力
		void DataToBMPfile_Bin(int nW, int nH, const BYTE* BmpData, const char* Path, bool nTB = false);	// Bitmapファイルへ出力 (Bitmapヘッダを自動で生成)
//		int BMPfileToData(const char* Path, int* nW, int* nH, BYTE *BmpData);				// Bitmapファイル読み込み

/*
	int GetBitmapHeaderSize2();
	int CreateBitmapHeader2(int bW, int bH, BYTE *BmpHead);
	void DataToBMPfile2(int bW, int bH, const BYTE *BmpData, const char *Path);
	void DataToBMPfile2(int bW, int bH, const BYTE *BmpData, BYTE *BmpHead, const char *Path);
*/

	};
};




