#include <windows.h>
#include "IppResize.h"


//C���̂��֐��錾
extern "C" const int cf_EnableIpp;

extern "C" int WINAPI cf_ippStaticInit();

//���̐錾��IntPtr�Ȃ̂́A�}�[�V�������O�̖ʓ|������邽�߁B
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
// �ÓI�R���X�g���N�^�B
//�@�v���Z�X�ŁA�N���X�����o�[�̎g�p�O�ɂP��Ă΂��
//------------------------------------------
static IppResize::IppResize(void)
{
	cf_ippStaticInit();
}

//------------------------------------------
// �R���X�g���N�^
//------------------------------------------
IppResize::IppResize(void)
{

}

//------------------------------------------
// �k���֐��̌Ăяo��
//  ���摜
//   IntPtr src   �摜�s�N�Z���̐擪�|�C���^
//   int sstride  �P���C��������o�C�g��
//   int swidth �@���s�N�Z����
//   int sheight  ���s�N�Z����
//  �k����摜
//   IntPtr dst   �摜�s�N�Z���̐擪�|�C���^
//   int dstride  �P���C��������o�C�g��
//   int dwidth �@���s�N�Z����
//   int dheight  ���s�N�Z����
//  �k������
//   ResizeMethod interpolation_method �⊮���@
//------------------------------------------

//------------------------------------------
// �k���֐��̌Ăяo��( 8bit�O���C�X�P�[�� )
//------------------------------------------
int IppResize::Resize_8u_C1R( 	
		IntPtr src, int sstride,int swidth, int sheight,
		IntPtr dst, int dstride, int dwidth, int dheight,
		int interpolation_method )
{
	//C�֐���CLR�Ƃ̃C���^�[�t�F�[�X�ϊ�
	SIZE ssize = { swidth, sheight };
	SIZE dsize = { dwidth, dheight };
	
	//�{���������v�Z
	double xFactor = dwidth / double(swidth);	
	double yFactor = dheight / double(sheight);	

	//�A���}�l�[�W��
	return ::cf_Resize_8u_C1R(
			src, sstride, ssize,
			dst, dstride, dsize,
			xFactor, yFactor,
			interpolation_method
		);		
}

//------------------------------------------
// �k���֐��̌Ăяo��( 24bit�J���[ )
//------------------------------------------
int IppResize::Resize_8u_C3R( 	
		IntPtr src, int sstride,int swidth, int sheight,
		IntPtr dst, int dstride, int dwidth, int dheight,
		int interpolation_method )
{
	//C�֐���CLR�Ƃ̃C���^�[�t�F�[�X�ϊ�
	SIZE ssize = { swidth, sheight };
	SIZE dsize = { dwidth, dheight };
	
	//�{���������v�Z
	double xFactor = dwidth / double(swidth);	
	double yFactor = dheight / double(sheight);	

	//�A���}�l�[�W��
	return ::cf_Resize_8u_C3R(
			src, sstride, ssize,
			dst, dstride, dsize,
			xFactor, yFactor,
			interpolation_method
		);		
}


//------------------------------------------
// �k���֐��̌Ăяo��( 32bit�J���[ )
//------------------------------------------
int IppResize::Resize_8u_C4R( 	
		IntPtr src, int sstride,int swidth, int sheight,
		IntPtr dst, int dstride, int dwidth, int dheight,
		int interpolation_method )
{
	//C�֐���CLR�Ƃ̃C���^�[�t�F�[�X�ϊ�
	SIZE ssize = { swidth, sheight };
	SIZE dsize = { dwidth, dheight };
	
	//�{���������v�Z
	double xFactor = dwidth / double(swidth);	
	double yFactor = dheight / double(sheight);	

	//�A���}�l�[�W��
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
