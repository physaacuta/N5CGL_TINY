#include <windows.h>
#include "IppResize.h"


//C側のを関数宣言
extern "C" const int cf_EnableIpp;

extern "C" int WINAPI cf_ippStaticInit();

//この宣言がIntPtrなのは、マーシャリングの面倒を避けるため。
extern "C" int WINAPI cf_Resize_8u_C1R(
	IntPtr src, int sstride, SIZE ssize,
	IntPtr dst, int dstride, SIZE dsize,
	double xFactor, double yFactor,
	int interpolation_method );

extern "C" int WINAPI cf_Resize_8u_C3R(
	IntPtr src, int sstride, SIZE ssize,
	IntPtr dst, int dstride, SIZE dsize,
	double xFactor, double yFactor,
	int interpolation_method );

extern "C" int WINAPI cf_Resize_8u_C4R(
	IntPtr src, int sstride, SIZE ssize,
	IntPtr dst, int dstride, SIZE dsize,
	double xFactor, double yFactor,
	int interpolation_method );

using namespace ImgLibCLI;

//------------------------------------------
// 静的コンストラクタ。
//　プロセスで、クラスメンバーの使用前に１回呼ばれる
//------------------------------------------
static IppResize::IppResize(void)
{
	cf_ippStaticInit();
}

//------------------------------------------
// コンストラクタ
//------------------------------------------
IppResize::IppResize(void)
{

}

//------------------------------------------
// 縮小関数の呼び出し
//  元画像
//   IntPtr src   画像ピクセルの先頭ポインタ
//   int sstride  １ラインあたりバイト数
//   int swidth 　幅ピクセル数
//   int sheight  高ピクセル数
//  縮小先画像
//   IntPtr dst   画像ピクセルの先頭ポインタ
//   int dstride  １ラインあたりバイト数
//   int dwidth 　幅ピクセル数
//   int dheight  高ピクセル数
//  縮小特性
//   ResizeMethod interpolation_method 補完方法
//------------------------------------------

//------------------------------------------
// 縮小関数の呼び出し( 8bitグレイスケール )
//------------------------------------------
int IppResize::Resize_8u_C1R( 	
		IntPtr src, int sstride,int swidth, int sheight,
		IntPtr dst, int dstride, int dwidth, int dheight,
		int interpolation_method )
{
	//C関数とCLRとのインターフェース変換
	SIZE ssize = { swidth, sheight };
	SIZE dsize = { dwidth, dheight };
	
	//倍率を自動計算
	double xFactor = dwidth / double(swidth);	
	double yFactor = dheight / double(sheight);	

	//アンマネージへ
	return ::cf_Resize_8u_C1R(
			src, sstride, ssize,
			dst, dstride, dsize,
			xFactor, yFactor,
			interpolation_method
		);		
}

//------------------------------------------
// 縮小関数の呼び出し( 24bitカラー )
//------------------------------------------
int IppResize::Resize_8u_C3R( 	
		IntPtr src, int sstride,int swidth, int sheight,
		IntPtr dst, int dstride, int dwidth, int dheight,
		int interpolation_method )
{
	//C関数とCLRとのインターフェース変換
	SIZE ssize = { swidth, sheight };
	SIZE dsize = { dwidth, dheight };
	
	//倍率を自動計算
	double xFactor = dwidth / double(swidth);	
	double yFactor = dheight / double(sheight);	

	//アンマネージへ
	return ::cf_Resize_8u_C3R(
			src, sstride, ssize,
			dst, dstride, dsize,
			xFactor, yFactor,
			interpolation_method
		);		
}


//------------------------------------------
// 縮小関数の呼び出し( 32bitカラー )
//------------------------------------------
int IppResize::Resize_8u_C4R( 	
		IntPtr src, int sstride,int swidth, int sheight,
		IntPtr dst, int dstride, int dwidth, int dheight,
		int interpolation_method )
{
	//C関数とCLRとのインターフェース変換
	SIZE ssize = { swidth, sheight };
	SIZE dsize = { dwidth, dheight };
	
	//倍率を自動計算
	double xFactor = dwidth / double(swidth);	
	double yFactor = dheight / double(sheight);	

	//アンマネージへ
	return ::cf_Resize_8u_C4R(
			src, sstride, ssize,
			dst, dstride, dsize,
			xFactor, yFactor,
			interpolation_method
		);		
}


bool IppResize::IsEnableIpp() {
	return cf_EnableIpp!=0;
}
