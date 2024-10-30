#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include "../../../include/LibSelect.h"									// ライブラリ切替ヘッダー

#ifdef DEFECTLIB_KIZUJPEGMANAGER
	#include "ipp.h"

	#ifndef _M_X64
		#pragma comment(lib, "ippimerged.lib")
		#pragma comment(lib, "ippsmerged.lib")
		#pragma comment(lib, "ippjmerged.lib")
		#pragma comment(lib, "ippcvmerged.lib")
		#pragma comment(lib, "ippsemerged.lib")
		#pragma comment(lib, "ippiemerged.lib")
		#pragma comment(lib, "ippjemerged.lib")
		#pragma comment(lib, "ippcvemerged.lib")
		#pragma comment(lib, "ippcorel.lib")

	#else
		#pragma comment(lib, "ippimergedem64t.lib")
		#pragma comment(lib, "ippsmergedem64t.lib")
		#pragma comment(lib, "ippjmergedem64t.lib")
		#pragma comment(lib, "ippcvmergedem64t.lib")
		#pragma comment(lib, "ippsemergedem64t.lib")
		#pragma comment(lib, "ippiemergedem64t.lib")
		#pragma comment(lib, "ippjemergedem64t.lib")
		#pragma comment(lib, "ippcvemergedem64t.lib")
		#pragma comment(lib, "ippcoreem64tl.lib")
	#endif

extern const int cf_EnableIpp = 1;

	//------------------------------------------
	// 初期化
	// 　起動後に１回呼び出しておくと、
	// 　IPPが実行時のCPUに応じて最適な処理を選択する
	//------------------------------------------
	int WINAPI cf_ippStaticInit()
	{
		return ippStaticInit();
	}


	//------------------------------------------
	// 縮小の実行
	//  元画像
	//   const unsigned char * src   画像ピクセルの先頭ポインタ
	//   int sstride           １ラインあたりバイト数
	//   IppiSize ssize 　     幅ピクセル数
	//  縮小先画像
	//   unsigned char * dst   画像ピクセルの先頭ポインタ
	//   int dstride           １ラインあたりバイト数
	//   IppiSize dsize 　     幅ピクセル数
	//  縮小特性
	//	 double xFactor			幅方向拡大率
	//   double yFactor         幅方向拡大率
	//   ResizeMethod interpolation_method 補完方法
	//------------------------------------------

	//8bitグレイスケール画像の縮小
	int WINAPI cf_Resize_8u_C1R(
		const unsigned char *src, int sstride, IppiSize ssize,
			  unsigned char *dst, int dstride, IppiSize dsize,
		double xFactor, double yFactor,
		int interpolation_method )
	{
		IppiRect sRect = { 0, 0, ssize.width, ssize.height };
		
		return ippiResize_8u_C1R( 
			src, ssize, sstride, sRect, 
			dst, dstride, dsize, 
			xFactor, yFactor,
			interpolation_method);
	}

	//24bitカラー画像の縮小
	int WINAPI cf_Resize_8u_C3R(
		const unsigned char *src, int sstride, IppiSize ssize,
			  unsigned char *dst, int dstride, IppiSize dsize,
		double xFactor, double yFactor,
		int interpolation_method )
	{
		IppiRect sRect = { 0, 0, ssize.width, ssize.height };
		
		return ippiResize_8u_C3R( 
			src, ssize, sstride, sRect, 
			dst, dstride, dsize, 
			xFactor, yFactor,
			interpolation_method);
	}

	//32bitカラー画像の縮小
	int WINAPI cf_Resize_8u_C4R(
		const unsigned char *src, int sstride, IppiSize ssize,
			  unsigned char *dst, int dstride, IppiSize dsize,
		double xFactor, double yFactor,
		int interpolation_method )
	{
		IppiRect sRect = { 0, 0, ssize.width, ssize.height };
		
		return ippiResize_8u_C4R( 
			src, ssize, sstride, sRect, 
			dst, dstride, dsize, 
			xFactor, yFactor,
			interpolation_method);
	}
#else 
//IPPない時は、こっちが実行されます。

extern const int cf_EnableIpp = 0;

	typedef struct {
		int width;
		int height;
	} IppiSize;

	//------------------------------------------
	//ダミー初期化関数
	//------------------------------------------
	int WINAPI cf_ippStaticInit()
	{
		return 0xdeadbeef;
	}

	//------------------------------------------
	//ダミー縮小関数
	//------------------------------------------
	int WINAPI cf_Resize_8u_C1R(
		const unsigned char *src, int sstride, IppiSize ssize,
		unsigned char *dst, int dstride, IppiSize dsize,
		double xFactor, double yFactor,
		int interpolation_method )
	{
		return 0xdeadbeef;
	}
	int WINAPI cf_Resize_8u_C3R(
		const unsigned char *src, int sstride, IppiSize ssize,
		unsigned char *dst, int dstride, IppiSize dsize,
		double xFactor, double yFactor,
		int interpolation_method )
	{
		return 0xdeadbeef;
	}
	int WINAPI cf_Resize_8u_C4R(
		const unsigned char *src, int sstride, IppiSize ssize,
		unsigned char *dst, int dstride, IppiSize dsize,
		double xFactor, double yFactor,
		int interpolation_method )
	{
		return 0xdeadbeef;
	}
#endif 