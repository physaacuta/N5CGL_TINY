#pragma once

using namespace System;
using namespace System::Runtime::InteropServices;

namespace ImgLibCLI
{
	public ref class IppResize
	{
	public:
		//=======================================
		// ���J���\�b�h
		//=======================================
		static IppResize(void);		//�N���X�C�j�V�����C�U

		IppResize(void);				//�R���X�g���N�^



		//���T�C�Y���̕�ԕ��@
		enum class  ResizeMethod : int
		{
			NN = 1,
			LINEAR = 2,
			CUBIC = 4,
			SUPER = 8,
			LANCZOS = 16,
		};

		//ImgLibCli��IPP���ŃR���p�C������Ă���Ƃ��AFalse
		//�Ȃ̂ŁA���̃N���X��Resize���g���Ă͂����Ȃ��B
		static bool IsEnableIpp() ;


		//8bit �O���[�X�P�[���̃��T�C�Y
		int Resize_8u_C1R( 	
			IntPtr src, int sstride, int swidth, int sheight,
			IntPtr dst, int dstride, int dwidth, int dheight,
			int interpolation_method );
		//24bit �J���[�̃��T�C�Y
		int Resize_8u_C3R( 	
			IntPtr src, int sstride, int swidth, int sheight,
			IntPtr dst, int dstride, int dwidth, int dheight,
			int interpolation_method );
		//32bit �J���[�̃��T�C�Y
		int Resize_8u_C4R( 	
			IntPtr src, int sstride, int swidth, int sheight,
			IntPtr dst, int dstride, int dwidth, int dheight,
			int interpolation_method );
	};
};
