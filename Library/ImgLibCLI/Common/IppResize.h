#pragma once

using namespace System;
using namespace System::Runtime::InteropServices;

namespace ImgLibCLI
{
	public ref class IppResize
	{
	public:
		//=======================================
		// 公開メソッド
		//=======================================
		static IppResize(void);		//クラスイニシャライザ

		IppResize(void);				//コンストラクタ



		//リサイズ時の補間方法
		enum class  ResizeMethod : int
		{
			NN = 1,
			LINEAR = 2,
			CUBIC = 4,
			SUPER = 8,
			LANCZOS = 16,
		};

		//ImgLibCliがIPP無でコンパイルされているとき、False
		//なので、このクラスのResizeを使ってはいけない。
		static bool IsEnableIpp() ;


		//8bit グレースケールのリサイズ
		int Resize_8u_C1R( 	
			IntPtr src, int sstride, int swidth, int sheight,
			IntPtr dst, int dstride, int dwidth, int dheight,
			int interpolation_method );
		//24bit カラーのリサイズ
		int Resize_8u_C3R( 	
			IntPtr src, int sstride, int swidth, int sheight,
			IntPtr dst, int dstride, int dwidth, int dheight,
			int interpolation_method );
		//32bit カラーのリサイズ
		int Resize_8u_C4R( 	
			IntPtr src, int sstride, int swidth, int sheight,
			IntPtr dst, int dstride, int dwidth, int dheight,
			int interpolation_method );
	};
};
