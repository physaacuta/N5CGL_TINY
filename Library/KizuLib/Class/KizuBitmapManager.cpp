#include "Stdafx.h"
#include "KizuBitmapManager.h"

#include <fstream>			
#include "KizuMacro.h"												// マクロ集

using namespace KizuLib;

//------------------------------------------
// コンストラクタ
//------------------------------------------
KizuBitmapManager::KizuBitmapManager(void)
{
}

//------------------------------------------
// デストラクタ
//------------------------------------------
KizuBitmapManager::~KizuBitmapManager(void)
{
}


// //////////////////////////////////////////////////////////////////////////////
// カラー画像24bpp用
// //////////////////////////////////////////////////////////////////////////////

//------------------------------------------
// Bitmapヘッダサイズ取得
// 戻り値 Bitmapヘッダーサイズ
//------------------------------------------
inline int KizuBitmapManager::GetBitmapHeaderSize_Color()
{
	return sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER); 
}

//------------------------------------------
// Bitmapヘッダ生成
// int nW BMP画像幅
// int nH BMP画像高さ
// BYTE *BmpHead BitMapヘッダ(あらかじめBitmapヘッダサイズ分の領域を確保しておくこと)
// bool nTB 画像反転 true:反転 
// 戻り値 復帰情報 0:正常 1～:各所エラー
//------------------------------------------
int KizuBitmapManager::CreateBitmapHeader_Color(int nW, int nH, BYTE *BmpHead, bool nTB)
{
	BITMAPFILEHEADER	bmpfhead;
	BITMAPINFOHEADER	bmpinf;
	int					nBitCount = 24;					// ビット深さ

	// 初期化
	memset(&bmpfhead, 0x00, sizeof(bmpfhead));
	memset(&bmpinf, 0x00, sizeof(bmpinf));

	// ファイルヘッダ作成
	memcpy(&bmpfhead.bfType, "BM", 2);					// ファイル識別子("BM"固定)
	bmpfhead.bfSize = (DWORD)							// ファイルサイズ(バイト単位)
		((nW * nH * nBitCount / 8) + GetBitmapHeaderSize_Color());
	bmpfhead.bfOffBits = GetBitmapHeaderSize_Color();	// ビットマップイメージデータ開始位置

	// ビットマップヘッダ作成
	bmpinf.biSize = 40;									// ヘッダサイズ(40固定)
	bmpinf.biWidth = (long)nW;							// イメージの横幅(ピクセル単位)
	bmpinf.biHeight = (long)(nTB ? nH : -nH); //(long)nH*(-1);					// イメージの高さ(ピクセル単位) (Windowsは画像が上下反転の為)
	bmpinf.biPlanes = 1;								// プレーン数(1固定)
	bmpinf.biBitCount = nBitCount;							// １ピクセル当たりのビット数(1,4,8,24)
	bmpinf.biCompression = BI_RGB;						// 圧縮形式(BI_RGB:0->非圧縮, BI_RLE8:1->RLE圧縮)
	bmpinf.biSizeImage = (DWORD)(nW * nH * nBitCount / 8);				// 圧縮後のイメージサイズ(バイト単位)
	bmpinf.biClrUsed = 0;								// 水平解像度(ピクセル/メートル)
	bmpinf.biClrImportant = 0;						// 垂直解像度(ピクセル/メートル)

	// データセット
	memcpy(&BmpHead[0], (unsigned char*)&bmpfhead, sizeof(bmpfhead));
	memcpy(&BmpHead[sizeof(bmpfhead)], (unsigned char*)&bmpinf, sizeof(bmpinf));

	return 0;
}

//------------------------------------------
// 文字を描画 (2バイト文字に対応済み)
// int nW BMP画像幅
// int nH BMP画像高さ
// BYTE* BmpData BitMapデータ部
// int nLeft 座標
// char const* szText 文字列
// int nFontSize フォントサイズ
// DWORD color 文字色。
//------------------------------------------
bool KizuBitmapManager::DrawText_Color(int nW, int nH, BYTE* BmpData, int nLeft, int nTop, char const* szText, int nFontSize, DWORD color)
{
	//// フォントの生成
	LOGFONT font	= { nFontSize, 0, 0, 0, 0, 0, 0, 0, SHIFTJIS_CHARSET, OUT_TT_ONLY_PRECIS,
					   CLIP_DEFAULT_PRECIS, PROOF_QUALITY, FIXED_PITCH | FF_MODERN, "ＭＳ ゴシック" };
	HFONT	hFont	= CreateFontIndirect(&font);
	if( NULL == hFont) return false;

	//// デバイスコンテキスト取得
	HDC		hDc		= GetDC(NULL);
	HFONT	oldFont = (HFONT)SelectObject(hDc, hFont);

	//色をバイト単位に分解
	BYTE colorR = GetRValue(color);
	BYTE colorG = GetGValue(color);
	BYTE colorB = GetBValue(color);


	//// 文字数分ループ	※ワイド文字加味
	const char* c = szText;
	while (*c) {
		// 文字コード取得
		// マルチバイト文字の場合、1バイト文字のコードは1バイト目のUINT変換、
		// 2バイト文字のコードは[先導コード]*256 + [文字コード]
		UINT nCode = 0;
		int nWidth;
		if (IsDBCSLeadByte(*c)) {
			nCode = (BYTE)c[0]<<8 | (BYTE)c[1];
			c += 2;
			nWidth = nFontSize;
		} else {
			nCode = c[0];
			c++;
			nWidth = nFontSize / 2;
		}
		// フォントビットマップ取得
		// GGO_BITMAPの場合、「DWORDにアラインメントされたモノトーンBMP」が返る
		// モノトーンBMPとは1ピクセルを1ビットで表現するビットマップの事
		TEXTMETRIC tm;
		GetTextMetrics(hDc, &tm);
		GLYPHMETRICS gm;
		CONST MAT2 mat = {{0,1},{0,0},{0,0},{0,1}};
		DWORD size = GetGlyphOutline(hDc, nCode, GGO_BITMAP, &gm, 0, NULL, &mat);
		BYTE *ptr = new BYTE[size*2];	// 多めに領域を確保する。必要ないかも知れないが、メモリを壊すと怖いため
		memset(ptr, 0x00, size*2);
		GetGlyphOutline(hDc, nCode, GGO_BITMAP, &gm, size, ptr, &mat);
		// フォント情報の書き込み
		// 取得したビットが1ピクセルに相当するので、
		// nOffsetX, nOffsetY : 書き出し位置(左上)
		// nBmpW, nBmpH : フォントビットマップの幅高
		int nOffsetX = gm.gmptGlyphOrigin.x + nLeft;				// 書き込み位置のオフセット横位置
		int nOffsetY = tm.tmAscent - gm.gmptGlyphOrigin.y + nTop;	// 書き込み位置のオフセット横位置
		int nBmpW = gm.gmBlackBoxX;									// 取得したBMPの横ピクセル数
		int nBmpH = gm.gmBlackBoxY;									// BMPの縦ピクセル数
		int iUseBYTEparLine = (1 + (gm.gmBlackBoxX / 32))*4;		// 1行に使用しているBYTE数（4バイトアラインメント）
		
		// 整合性チェック
		if(0 > nOffsetY) nOffsetY = 0;
		if(0 > nOffsetX) nOffsetX = 0;
		if(nH < nOffsetY+nBmpH) nOffsetY = nH - nBmpH;
		if(nW < nOffsetX+nBmpW) nOffsetX = nW - nBmpW;
		
		for (int y=nOffsetY; y < nOffsetY+nBmpH; y++) {
			for (int x=nOffsetX; x < nOffsetX+nBmpW; x++) {
				DWORD num = (x-nOffsetX) / 8;									// 現在のxが1行の何BYTE目かを算出
				BYTE bit = (x-nOffsetX) % 8;									// 現在のxが1バイト内の何ビット目かを算出
				BYTE mask = ((BYTE)1)<<(7-bit);									// 現在のビット数のマスク作成
				BYTE Cur = ptr[ iUseBYTEparLine*(y-nOffsetY)+num ];	// 現在のバイト位置にあるビットマップを取得
				if (Cur & mask){
					BmpData[(y*nW + x)*3 +0] = colorR;								// ドット書き込み（1ピクセル）
					BmpData[(y*nW + x)*3 +1] = colorG;								// ドット書き込み（1ピクセル）
					BmpData[(y*nW + x)*3 +2] = colorB;								// ドット書き込み（1ピクセル）
				}
			}
		}
		delete[] ptr;
		nLeft += nWidth;
	}

	//// デバイスコンテキストとフォントハンドルの開放
	SelectObject(hDc, oldFont);
	DeleteObject(hFont);
	ReleaseDC(NULL, hDc);
	return true;
}



// //////////////////////////////////////////////////////////////////////////////
// グレー画像用
// //////////////////////////////////////////////////////////////////////////////
//------------------------------------------
// Bitmapヘッダサイズ取得
// 戻り値 Bitmapヘッダーサイズ
//------------------------------------------
inline int KizuBitmapManager::GetBitmapHeaderSize_Gray()
{
	return sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + sizeof(RGBQUAD) * 256;
}

//------------------------------------------
// Bitmapヘッダ生成
// int nW BMP画像幅
// int nH BMP画像高さ
// BYTE *BmpHead BitMapヘッダ(あらかじめBitmapヘッダサイズ分の領域を確保しておくこと)
// bool nTB 画像反転 true:反転 
// 戻り値 復帰情報 0:正常 1～:各所エラー
//------------------------------------------
int KizuBitmapManager::CreateBitmapHeader_Gray(int nW, int nH, BYTE *BmpHead, bool nTB)
{
	int					ii;
	BITMAPFILEHEADER	bmpfhead;
	BITMAPINFOHEADER	bmpinf;
	RGBQUAD				rgb[256];
	int					nBitCount = 8;					// ビット深さ

	// 初期化
	memset(&bmpfhead, 0x00, sizeof(bmpfhead));
	memset(&bmpinf, 0x00, sizeof(bmpinf));
	memset(&rgb[0], 0x00, sizeof(rgb));

	// ファイルヘッダ作成
	memcpy(&bmpfhead.bfType, "BM", 2);					// ファイル識別子("BM"固定)
	bmpfhead.bfSize = (DWORD)							// ファイルサイズ(バイト単位)
		((nW * nH * nBitCount / 8) + GetBitmapHeaderSize_Gray());
	bmpfhead.bfOffBits = GetBitmapHeaderSize_Gray();	// ビットマップイメージデータ開始位置

	// ビットマップヘッダ作成
	bmpinf.biSize = 40;									// ヘッダサイズ(40固定)
	bmpinf.biWidth = (long)nW;							// イメージの横幅(ピクセル単位)
	bmpinf.biHeight = (long)(nTB ? nH : -nH); //(long)nH*(-1);					// イメージの高さ(ピクセル単位) (Windowsは画像が上下反転の為)
	bmpinf.biPlanes = 1;								// プレーン数(1固定)
	bmpinf.biBitCount = nBitCount;							// １ピクセル当たりのビット数(1,4,8,24)
	bmpinf.biCompression = BI_RGB;						// 圧縮形式(BI_RGB:0->非圧縮, BI_RLE8:1->RLE圧縮)
	bmpinf.biSizeImage = (DWORD)(nW * nH * nBitCount / 8);				// 圧縮後のイメージサイズ(バイト単位)
	//bmpinf.biClrUsed = 256;								// 水平解像度(ピクセル/メートル)
	//bmpinf.biClrImportant = 256;						// 垂直解像度(ピクセル/メートル)
	bmpinf.biClrUsed = 0;								// 水平解像度(ピクセル/メートル)
	bmpinf.biClrImportant = 0;						// 垂直解像度(ピクセル/メートル)

	// カラーパレット作成(パレットの中身)
    for (ii = 0; ii < 256; ii++) {	
		rgb[ii].rgbBlue = (BYTE)ii;
		rgb[ii].rgbRed = (BYTE)ii;
		rgb[ii].rgbGreen = (BYTE)ii;
	}
	// データセット
	memcpy(&BmpHead[0], (unsigned char*)&bmpfhead, sizeof(bmpfhead));
	memcpy(&BmpHead[sizeof(bmpfhead)], (unsigned char*)&bmpinf, sizeof(bmpinf));
	memcpy(&BmpHead[sizeof(bmpfhead) + sizeof(bmpinf)], (unsigned char*)&rgb[0], sizeof(RGBQUAD) * 256);

	return 0;
}

//------------------------------------------
// Bitmapファイルへ出力
// int nW BMP画像幅
// int nH BMP画像高さ
// const BYTE* BmpData BitMapデータ部
// const BYTE* BmpHead BitMapヘッダ部
// const char* Path 出力ファイルパス
//------------------------------------------
void KizuBitmapManager::DataToBMPfile_Gray(int nW, int nH, const BYTE* BmpData, const BYTE* BmpHead, const char* Path)
{
	std::ofstream os;

//	os.open(Path, std::ios::out | std::ios::binary, filebuf::sh_read | filebuf::sh_write);
	os.open(Path, std::ios::out | std::ios::binary);
	os.write((char*)BmpHead, GetBitmapHeaderSize_Gray() );
	os.write((char*)BmpData, nW * nH);
	os.close();
}

//------------------------------------------
// Bitmapファイルへ出力
// int nW BMP画像幅
// int nH BMP画像高さ
// const BYTE* BmpData BitMapデータ部
// bool nTB 画像反転 true:反転 
// const char* Path 出力ファイルパス
//------------------------------------------
void KizuBitmapManager::DataToBMPfile_Gray(int nW, int nH, const BYTE* BmpData, const char* Path, bool nTB)
{
	BYTE *BmpHead = new BYTE[GetBitmapHeaderSize_Gray()];
	CreateBitmapHeader_Gray(nW, nH, BmpHead, nTB);
	DataToBMPfile_Gray(nW, nH, BmpData, BmpHead, Path);
	delete BmpHead;
}

//------------------------------------------
// Bitmapファイル読み込み
// const char* Path 読込ファイルパス
// int* nW BMP画像幅
// int* nH BMP画像高さ
// const BYTE* BmpData Bitmapデータ部
// 戻り値 0:異常 1～:Bitmapデータサイズ
//------------------------------------------
int KizuBitmapManager::BMPfileToData_Gray(const char* Path, int* nW, int* nH, BYTE *BmpData)
{
	std::ifstream os;
	BYTE *BmpHead = new BYTE[GetBitmapHeaderSize_Gray()];

	//os.open(Path, ios::in | ios::binary, filebuf::sh_read|filebuf::sh_write);
	os.open(Path, std::ios::in | std::ios::binary);
	os.read((char*)BmpHead, GetBitmapHeaderSize_Gray());		// Bitmapヘッダ部のみ読込
	
	// 画像サイズ決定
	*nW = abs( ((BITMAPINFOHEADER*)&BmpHead[sizeof(BITMAPFILEHEADER)])->biWidth );
	*nH = abs( ((BITMAPINFOHEADER*)&BmpHead[sizeof(BITMAPFILEHEADER)])->biHeight );
	int size = (*nW) * (*nH);

	os.read((char*)BmpData, size);
	os.close();

	delete BmpHead;
	return size;
}
//------------------------------------------
// Bitmapファイルのヘッダー部のみ読込み
//------------------------------------------
int KizuBitmapManager::BMPfileToData_Gray(const char* Path, int* nW, int* nH)
{
	std::ifstream os;
	BYTE *BmpHead = new BYTE[GetBitmapHeaderSize_Gray()];

	//os.open(Path, ios::in | ios::binary, filebuf::sh_read|filebuf::sh_write);
	os.open(Path, std::ios::in | std::ios::binary);
	os.read((char*)BmpHead, GetBitmapHeaderSize_Gray());		// Bitmapヘッダ部のみ読込
	
	// 画像サイズ決定
	*nW = abs( ((BITMAPINFOHEADER*)&BmpHead[sizeof(BITMAPFILEHEADER)])->biWidth );
	*nH = abs( ((BITMAPINFOHEADER*)&BmpHead[sizeof(BITMAPFILEHEADER)])->biHeight );
	int size = (*nW) * (*nH);
	delete BmpHead;
	return size;
}


//------------------------------------------
// 線を描画 (直線、斜線に対応済み)
// int nW BMP画像幅
// int nH BMP画像高さ
// BYTE* BmpData BitMapデータ部
// int nX1 座標
// BYTE color 線色 0:黒 255:白
//------------------------------------------
void KizuBitmapManager::DrawLine_Gray(int nW, int nH, BYTE* BmpData, int nX1, int nY1, int nX2, int nY2, BYTE color)
{
	//// 入力値チェック
	if(0 > nX1) nX1 = 0;
	if(0 > nX2) nX2 = 0;
	if(0 > nY1) nY1 = 0;
	if(0 > nY2) nY2 = 0;

	if(nW <= nX1) nX1 = nW-1;
	if(nW <= nX2) nX2 = nW-1;
	if(nH <= nY1) nY1 = nH-1;
	if(nH <= nY2) nY2 = nH-1;

	//// 必ず小さいやつが1 となるように
	int x1, x2, y1, y2;
	if( nX1 < nX2 ) { x1 = nX1; x2 = nX2; }
	else			{ x1 = nX2; x2 = nX1; }
	if( nY1 < nY2 ) { y1 = nY1; y2 = nY2; }
	else			{ y1 = nY2; y2 = nY1; }


	//// 描画
	if( nX1 != nX2 && nY1 != nY2) {
		// 斜線
		float a = (float)(nY2-nY1) / (float)(nX2-nX1);	// 傾き
		
		for(int x=x1; x<=x2; x++) {
			int y;
			if ( a < 0 ) {
				y = (int)(a*(x-x2))+y1;
			} else {
				y = (int)(a*(x-x1))+y1;
			}
			//int y = (int)(a*(x-x1))+y1;

			BmpData[y*nW + x] = color;
		}

	} else if(nX1 == nX2) {
		// 縦線
		for(int y=y1; y<=y2; y++) {
			BmpData[y*nW + nX1] = color;
		}

	} else {
		// 横線
		for(int x=x1; x<=x2; x++) {
			BmpData[nY1*nW + x] = color;
		}
	}
}
//------------------------------------------
// 文字を描画 (2バイト文字に対応済み)
// int nW BMP画像幅
// int nH BMP画像高さ
// BYTE* BmpData BitMapデータ部
// int nLeft 座標
// char const* szText 文字列
// int nFontSize フォントサイズ
// BYTE color 線色 0:黒 255:白
//------------------------------------------
bool KizuBitmapManager::DrawText_Gray(int nW, int nH, BYTE* BmpData, int nLeft, int nTop, char const* szText, int nFontSize, BYTE color)
{
	//// フォントの生成
	LOGFONT font	= { nFontSize, 0, 0, 0, 0, 0, 0, 0, SHIFTJIS_CHARSET, OUT_TT_ONLY_PRECIS,
					   CLIP_DEFAULT_PRECIS, PROOF_QUALITY, FIXED_PITCH | FF_MODERN, "ＭＳ ゴシック" };
	HFONT	hFont	= CreateFontIndirect(&font);
	if( NULL == hFont) return false;

	//// デバイスコンテキスト取得
	HDC		hDc		= GetDC(NULL);
	HFONT	oldFont = (HFONT)SelectObject(hDc, hFont);


	//// 文字数分ループ	※ワイド文字加味
	char* c = (char*)szText;
	while (*c) {
		// 文字コード取得
		// マルチバイト文字の場合、1バイト文字のコードは1バイト目のUINT変換、
		// 2バイト文字のコードは[先導コード]*256 + [文字コード]
		UINT nCode = 0;
		int nWidth;
		if (IsDBCSLeadByte(*c)) {
			nCode = (BYTE)c[0]<<8 | (BYTE)c[1];
			c += 2;
			nWidth = nFontSize;
		} else {
			nCode = c[0];
			c++;
			nWidth = nFontSize / 2;
		}
		// フォントビットマップ取得
		// GGO_BITMAPの場合、「DWORDにアラインメントされたモノトーンBMP」が返る
		// モノトーンBMPとは1ピクセルを1ビットで表現するビットマップの事
		TEXTMETRIC tm;
		GetTextMetrics(hDc, &tm);
		GLYPHMETRICS gm;
		CONST MAT2 mat = {{0,1},{0,0},{0,0},{0,1}};
		DWORD size = GetGlyphOutline(hDc, nCode, GGO_BITMAP, &gm, 0, NULL, &mat);
		BYTE *ptr = new BYTE[size*2];	// 多めに領域を確保する。必要ないかも知れないが、メモリを壊すと怖いため
		memset(ptr, 0x00, size*2);
		GetGlyphOutline(hDc, nCode, GGO_BITMAP, &gm, size, ptr, &mat);
		// フォント情報の書き込み
		// 取得したビットが1ピクセルに相当するので、
		// nOffsetX, nOffsetY : 書き出し位置(左上)
		// nBmpW, nBmpH : フォントビットマップの幅高
		int nOffsetX = gm.gmptGlyphOrigin.x + nLeft;				// 書き込み位置のオフセット横位置
		int nOffsetY = tm.tmAscent - gm.gmptGlyphOrigin.y + nTop;	// 書き込み位置のオフセット横位置
		int nBmpW = gm.gmBlackBoxX;									// 取得したBMPの横ピクセル数
		int nBmpH = gm.gmBlackBoxY;									// BMPの縦ピクセル数
		int iUseBYTEparLine = (1 + (gm.gmBlackBoxX / 32))*4;		// 1行に使用しているBYTE数（4バイトアラインメント）
		
		// 整合性チェック
		if(0 > nOffsetY) nOffsetY = 0;
		if(0 > nOffsetX) nOffsetX = 0;
		if(nH < nOffsetY+nBmpH) nOffsetY = nH - nBmpH;
		if(nW < nOffsetX+nBmpW) nOffsetX = nW - nBmpW;
		
		for (int y=nOffsetY; y < nOffsetY+nBmpH; y++) {
			for (int x=nOffsetX; x < nOffsetX+nBmpW; x++) {
				DWORD num = (x-nOffsetX) / 8;									// 現在のxが1行の何BYTE目かを算出
				BYTE bit = (x-nOffsetX) % 8;									// 現在のxが1バイト内の何ビット目かを算出
				BYTE mask = ((BYTE)1)<<(7-bit);									// 現在のビット数のマスク作成
				BYTE Cur = *((BYTE*)ptr + iUseBYTEparLine*(y-nOffsetY)+num);	// 現在のバイト位置にあるビットマップを取得
				Cur &= mask;													// 作成したマスクと現在のバイト位置とで論理積演算
				if (Cur) BmpData[y*nW + x] = color;								// ドットか書き込み（1ピクセル）
			}
		}
		delete[] ptr;
		nLeft += nWidth;
	}

	//// デバイスコンテキストとフォントハンドルの開放
	SelectObject(hDc, oldFont);
	DeleteObject(hFont);
	ReleaseDC(NULL, hDc);
	return true;
}



// //////////////////////////////////////////////////////////////////////////////
// 2値化画像用
// //////////////////////////////////////////////////////////////////////////////
//------------------------------------------
// Bitmapヘッダサイズ取得
// 戻り値 Bitmapヘッダーサイズ
//------------------------------------------
inline int KizuBitmapManager::GetBitmapHeaderSize_Bin()
{
	return sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + sizeof(RGBQUAD) * 2;
}

//------------------------------------------
// BitMapヘッダ生成
// int bW BMP画像幅
// int bH BMP画像高さ
// BYTE* BmpHead BitMapヘッダ(あらかじめBitMapヘッダサイズ分の領域を確保しておくこと)
// bool nTB 画像反転 true:反転 
// 戻り値 0:正常 1～:各所エラー
//------------------------------------------
int KizuBitmapManager::CreateBitmapHeader_Bin(int nW, int nH, BYTE *BmpHead, bool nTB)
{
	BITMAPFILEHEADER	bmpfhead;
	BITMAPINFOHEADER	bmpinf;
	RGBQUAD				rgb[2];
	int					nBitCount = 1;					// ビット深さ

	// 初期化
	memset(&bmpfhead, 0x00, sizeof(bmpfhead));
	memset(&bmpinf, 0x00, sizeof(bmpinf));
	memset(&rgb[0], 0x00, sizeof(rgb));

	// ファイルヘッダ作成
	memcpy(&bmpfhead.bfType, "BM", 2);					// ファイル識別子("BM"固定)
	bmpfhead.bfSize = (DWORD)							// ファイルサイズ(バイト単位)
		((nW * nH * nBitCount / 8) + GetBitmapHeaderSize_Bin());
	bmpfhead.bfOffBits = GetBitmapHeaderSize_Bin();		// ビットマップイメージデータ開始位置

	// ビットマップヘッダ作成
	bmpinf.biSize = 40;									// ヘッダサイズ(40固定)
	bmpinf.biWidth = (long)nW;							// イメージの横幅(ピクセル単位)
	bmpinf.biHeight = (long)(nTB ? nH : -nH); //(long)nH*(-1);					// イメージの高さ(ピクセル単位) (Windowsは画像が上下反転の為)
	bmpinf.biPlanes = 1;								// プレーン数(1固定)
	bmpinf.biBitCount = nBitCount;						// １ピクセル当たりのビット数(1,4,8,24)
	bmpinf.biCompression = BI_RGB;						// 圧縮形式(BI_RGB:0->非圧縮, BI_RLE8:1->RLE圧縮)
	bmpinf.biSizeImage = (DWORD)(nW * nH * nBitCount / 8);		// 圧縮後のイメージサイズ(バイト単位)
	bmpinf.biClrUsed = 0;								// 水平解像度(ピクセル/メートル)
	bmpinf.biClrImportant = 0;						// 垂直解像度(ピクセル/メートル)

	// カラーパレット作成(パレットの中身)
	rgb[0].rgbBlue = 0;
	rgb[0].rgbRed = 0;
	rgb[0].rgbGreen = 0;
	rgb[1].rgbBlue = 0xff;
	rgb[1].rgbRed = 0xff;
	rgb[1].rgbGreen = 0xff;

	// データセット
	memcpy(&BmpHead[0], (unsigned char*)&bmpfhead, sizeof(bmpfhead));
	memcpy(&BmpHead[sizeof(bmpfhead)], (unsigned char*)&bmpinf, sizeof(bmpinf));
	memcpy(&BmpHead[sizeof(bmpfhead) + sizeof(bmpinf)], (unsigned char*)&rgb[0], sizeof(RGBQUAD) * 2);

	return 0;
}

//------------------------------------------
// Bitmapファイルへ出力
// int nW BMP画像幅
// int nH BMP画像高さ
// const BYTE* BmpData BitMapデータ部
// const BYTE* BmpHead BitMapヘッダ部
// const char* Path 出力ファイルパス
//------------------------------------------
void KizuBitmapManager::DataToBMPfile_Bin(int nW, int nH, const BYTE* BmpData, const BYTE* BmpHead, const char* Path)
{
	// エリア確保
	int size = nW * nH * 1 / 8;
	BYTE *pData = new BYTE[size];
	
	// 1bitの画像は、ビット並びを入れ替える
	int rec=0;
	for(int ii=0; ii<size; ii++) {
		pData[ii] = ((BmpData[ii] & 0x1) << 7) |
					 ((BmpData[ii] & 0x2) << 5) |
					 ((BmpData[ii] & 0x4) << 3) |
					 ((BmpData[ii] & 0x8) << 1) |
					 ((BmpData[ii] & 0x10) >> 1) |
					 ((BmpData[ii] & 0x20) >> 3) |
					 ((BmpData[ii] & 0x40) >> 5) |
					 ((BmpData[ii] & 0x80) >> 7) ;
	}

	// 出力
	std::ofstream os;
	os.open(Path, std::ios::out | std::ios::binary);
	os.write((char*)BmpHead, GetBitmapHeaderSize_Bin() );
	//os.write((char*)BmpData, nW * nH);
	os.write((char*)pData, size);
	os.close();

	// エリア開放
	SAFE_DELETE_ARRAY(pData);
//	delete [] pData;
}

//------------------------------------------
// Bitmapファイルへ出力
// int nW BMP画像幅
// int nH BMP画像高さ
// const BYTE* BmpData BitMapデータ部
// const char* Path 出力ファイルパス
// bool nTB 画像反転 true:反転 
//------------------------------------------
void KizuBitmapManager::DataToBMPfile_Bin(int nW, int nH, const BYTE* BmpData, const char* Path, bool nTB)
{
	BYTE *BmpHead = new BYTE[GetBitmapHeaderSize_Bin()];
	CreateBitmapHeader_Bin(nW, nH, BmpHead, nTB);
	DataToBMPfile_Bin(nW, nH, BmpData, BmpHead, Path);
	delete BmpHead;
}
