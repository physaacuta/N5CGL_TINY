#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include "../../../include/LibSelect.h"									// ���C�u�����ؑփw�b�_�[

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
	// ������
	// �@�N����ɂP��Ăяo���Ă����ƁA
	// �@IPP�����s����CPU�ɉ����čœK�ȏ�����I������
	//------------------------------------------
	int WINAPI cf_ippStaticInit()
	{
		return ippStaticInit();
	}


	//------------------------------------------
	// �k���̎��s
	//  ���摜
	//   const unsigned char * src   �摜�s�N�Z���̐擪�|�C���^
	//   int sstride           �P���C��������o�C�g��
	//   IppiSize ssize �@     ���s�N�Z����
	//  �k����摜
	//   unsigned char * dst   �摜�s�N�Z���̐擪�|�C���^
	//   int dstride           �P���C��������o�C�g��
	//   IppiSize dsize �@     ���s�N�Z����
	//  �k������
	//	 double xFactor			�������g�嗦
	//   double yFactor         �������g�嗦
	//   ResizeMethod interpolation_method �⊮���@
	//------------------------------------------

	//8bit�O���C�X�P�[���摜�̏k��
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

	//24bit�J���[�摜�̏k��
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

	//32bit�J���[�摜�̏k��
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
//IPP�Ȃ����́A�����������s����܂��B

extern const int cf_EnableIpp = 0;

	typedef struct {
		int width;
		int height;
	} IppiSize;

	//------------------------------------------
	//�_�~�[�������֐�
	//------------------------------------------
	int WINAPI cf_ippStaticInit()
	{
		return 0xdeadbeef;
	}

	//------------------------------------------
	//�_�~�[�k���֐�
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