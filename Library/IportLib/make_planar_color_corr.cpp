#include "StdAfx.h"
#include <Windows.h>

#include "make_planar_color_corr.h"

// メモリーリーク検出 ----->>>
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif
// <<<------------------------

using namespace IportLib;


#define BYTE_SAT(v) (v) > 255 ? 255 : (v) < 0 ? 0 : (v)


//------------------------------------------
// カラー画像をRGB画像に分離
// BYTE* dstImg		RGB分離画像 (先)
// void** pImgsrc	カラー画像	(元)
// long pitch_dst	iPortあたりの幅
// long pitch_src	iPortあたりの幅(画素数考慮) 
// long width		最終画像サイズ
// long height		最終画像サイズ
//------------------------------------------
void IportLib::Copy_PackedColor8_1p(BYTE* dstImg, void** pImgsrc, long pitch_dst, long pitch_src, long width, long height)
{
    const long sz_pack = 3; // RGB
	const BYTE*const psrc0 = (BYTE*)pImgsrc[0];

    BYTE*const pdstR = &dstImg[width*height*IPORT_COLOR_R];
    BYTE*const pdstG = &dstImg[width*height*IPORT_COLOR_G];
    BYTE*const pdstB = &dstImg[width*height*IPORT_COLOR_B];

	long src_yoffset = 0;
	long dst_yoffset = 0;
    int srcoffset;
    for(int y=0; y<height; y++) {
        for(int x=0; x<width; x++) {
			srcoffset = src_yoffset + sz_pack * x;

         //   pdstR[dst_yoffset + x]     = psrc0[srcoffset + 2];	//R
	        //pdstG[dst_yoffset + x]     = psrc0[srcoffset + 1];	//G
         //   pdstB[dst_yoffset + x]     = psrc0[srcoffset    ];	//B
            pdstR[dst_yoffset + x]     = psrc0[srcoffset + 0];	//R
	        pdstG[dst_yoffset + x]     = psrc0[srcoffset + 1];	//G
            pdstB[dst_yoffset + x]     = psrc0[srcoffset + 2];	//B
		}
		src_yoffset += pitch_src;
		dst_yoffset += pitch_dst;
    }
}

//------------------------------------------
// カラー画像をRGB画像に分離
// BYTE* dstImg		RGB分離画像 (先)
// void** pImgsrc	カラー画像	(元)
// long pitch_dst	iPortあたりの幅
// long pitch_src	iPortあたりの幅(画素数考慮) ※ iPort2台分
// long width		最終画像サイズ
// long height		最終画像サイズ
//------------------------------------------
void IportLib::Copy_PackedColor8_2p(BYTE* dstImg, void** pImgsrc, long pitch_dst, long pitch_src, long width, long height)
{
    const long sz_pack = 3; // RGB
    const BYTE*const psrc0 = (BYTE*)pImgsrc[0];
    const BYTE*const psrc1 = (BYTE*)pImgsrc[1];

    BYTE*const pdstR = &dstImg[width*height*IPORT_COLOR_R];
    BYTE*const pdstG = &dstImg[width*height*IPORT_COLOR_G];
    BYTE*const pdstB = &dstImg[width*height*IPORT_COLOR_B];

	long src_yoffset = 0;
	long dst_yoffset = 0;
    int srcoffset;
	for(int y=0; y<height; y++) {
        for(int x=0; x<width; x+=2) {
			srcoffset = src_yoffset + sz_pack * x/2;

			//// 偶数画素						// 左半分
   //         pdstR[dst_yoffset + x]     = psrc0[srcoffset + 2];	//R
			//pdstG[dst_yoffset + x]     = psrc0[srcoffset + 1];	//G
   //         pdstB[dst_yoffset + x]     = psrc0[srcoffset    ];	//B
			//// 奇数画素						// 右半分
   //         pdstR[dst_yoffset + x + 1] = psrc1[srcoffset + 2];	//R
			//pdstG[dst_yoffset + x + 1] = psrc1[srcoffset + 1];	//G
   //         pdstB[dst_yoffset + x + 1] = psrc1[srcoffset    ];	//B
			// 偶数画素						// 左半分
            pdstR[dst_yoffset + x]     = psrc0[srcoffset + 0];	//R
			pdstG[dst_yoffset + x]     = psrc0[srcoffset + 1];	//G
            pdstB[dst_yoffset + x]     = psrc0[srcoffset + 2];	//B
			// 奇数画素						// 右半分
            pdstR[dst_yoffset + x + 1] = psrc1[srcoffset + 0];	//R
			pdstG[dst_yoffset + x + 1] = psrc1[srcoffset + 1];	//G
            pdstB[dst_yoffset + x + 1] = psrc1[srcoffset + 2];	//B

        }
		src_yoffset += pitch_src;
		dst_yoffset += pitch_dst;
    }
}


//------------------------------------------
// カラー画像をRGB画像+混色分離に分離
// BYTE* dstImg		RGB分離画像 (先)
// void** pImgsrc	カラー画像	(元)
// long pitch_dst	iPortあたりの幅
// long pitch_src	iPortあたりの幅(画素数考慮) ※ iPort1台分
// long width		最終画像サイズ
// long height		最終画像サイズ
// const float*const c	混色行列
//------------------------------------------
void IportLib::Copy_PackedColor_Corr_1p(BYTE* dstImg, void** pImgsrc, long pitch_dst, long pitch_src, long width, long height, const float*const c)
{
    const long sz_pack = 3; // RGB
    const BYTE* psrc0 = (BYTE*)pImgsrc[0]; // even

	BYTE*const pdstR = &dstImg[width*height*IPORT_COLOR_R];
    BYTE*const pdstG = &dstImg[width*height*IPORT_COLOR_G];
    BYTE*const pdstB = &dstImg[width*height*IPORT_COLOR_B];

	long src_yoffset = 0;
	long dst_yoffset = 0;
    int srcoffset;
    for(int y=0; y<height; y++) {
        for(int x=0; x<width; x++) {
			srcoffset = src_yoffset + sz_pack * x;

			// 偶数
            //const BYTE R0 = psrc0[srcoffset + 2];
            //const BYTE G0 = psrc0[srcoffset + 1];
            //const BYTE B0 = psrc0[srcoffset    ];
            const BYTE R0 = psrc0[srcoffset + 0];
            const BYTE G0 = psrc0[srcoffset + 1];
            const BYTE B0 = psrc0[srcoffset + 2];

            const float RR0 = R0 * c[0] + G0 * c[1] + B0 * c[2]  + c[3];
            const float GG0 = R0 * c[4] + G0 * c[5] + B0 * c[6]  + c[7];
            const float BB0 = R0 * c[8] + G0 * c[9] + B0 * c[10] + c[11];

            pdstR[dst_yoffset + x] = BYTE_SAT(RR0);
			pdstG[dst_yoffset + x] = BYTE_SAT(GG0);
            pdstB[dst_yoffset + x] = BYTE_SAT(BB0);
        }
		src_yoffset += pitch_src; 
		dst_yoffset += pitch_dst;
    }
}

//------------------------------------------
// カラー画像をRGB画像+混色分離に分離
// BYTE* dstImg		RGB分離画像 (先)
// void** pImgsrc	カラー画像	(元)
// long pitch_dst	iPortあたりの幅
// long pitch_src	iPortあたりの幅(画素数考慮) ※ iPort2台分
// long width		最終画像サイズ
// long height		最終画像サイズ
// const float*const c	混色行列
//------------------------------------------
void IportLib::Copy_PackedColor_Corr_2p(BYTE* dstImg, void** pImgsrc, long pitch_dst, long pitch_src, long width, long height, const float*const c)
{
    const long sz_pack = 3; // RGB
    const BYTE*const psrc0 = (BYTE*)pImgsrc[0]; // even
    const BYTE*const psrc1 = (BYTE*)pImgsrc[1]; // odd

	BYTE*const pdstR = &dstImg[width*height*IPORT_COLOR_R];
    BYTE*const pdstG = &dstImg[width*height*IPORT_COLOR_G];
    BYTE*const pdstB = &dstImg[width*height*IPORT_COLOR_B];

	long src_yoffset = 0;
	long dst_yoffset = 0;
    int srcoffset;
    for(int y=0; y<height; y++) {
        for(int x=0; x<width; x+=2) {
			srcoffset = src_yoffset + sz_pack * x/2;

			// 偶数
            //const BYTE R0 = psrc0[srcoffset + 2];
            //const BYTE G0 = psrc0[srcoffset + 1];
            //const BYTE B0 = psrc0[srcoffset    ];
            const BYTE R0 = psrc0[srcoffset + 0];
            const BYTE G0 = psrc0[srcoffset + 1];
            const BYTE B0 = psrc0[srcoffset + 2];

            const float RR0 = R0 * c[0] + G0 * c[1] + B0 * c[2]  + c[3];
            const float GG0 = R0 * c[4] + G0 * c[5] + B0 * c[6]  + c[7];
            const float BB0 = R0 * c[8] + G0 * c[9] + B0 * c[10] + c[11];

            pdstR[dst_yoffset + x] = BYTE_SAT(RR0);
			pdstG[dst_yoffset + x] = BYTE_SAT(GG0);
            pdstB[dst_yoffset + x] = BYTE_SAT(BB0);

			// 奇数
            //const BYTE R1 = psrc1[srcoffset + 2];
            //const BYTE G1 = psrc1[srcoffset + 1];
            //const BYTE B1 = psrc1[srcoffset    ];
            const BYTE R1 = psrc1[srcoffset + 0];
            const BYTE G1 = psrc1[srcoffset + 1];
            const BYTE B1 = psrc1[srcoffset + 2];

            const float RR1 = R1 * c[0] + G1 * c[1] + B1 * c[2]  + c[3];
            const float GG1 = R1 * c[4] + G1 * c[5] + B1 * c[6]  + c[7];
            const float BB1 = R1 * c[8] + G1 * c[9] + B1 * c[10] + c[11];

            pdstR[dst_yoffset + x + 1] = BYTE_SAT(RR1);
            pdstG[dst_yoffset + x + 1] = BYTE_SAT(GG1);
            pdstB[dst_yoffset + x + 1] = BYTE_SAT(BB1);
        }
		src_yoffset += pitch_src;
		dst_yoffset += pitch_dst;
    }
}




/*
void make_rgbplanar_color_corr_C(BYTE*const* pdst, long pitch_dst,
                                 const BYTE*const* psrc,
                                 long width, long height, const short*const c)
{
    const long sz_pack = 3; // RGB

    const BYTE* psrc0 = (BYTE*)psrc[0]; // even
    const BYTE* psrc1 = (BYTE*)psrc[1]; // odd
    const long pitch_src = sz_pack * width / 2;

    BYTE* pdstR = (BYTE*)pdst[0];
    BYTE* pdstG = (BYTE*)pdst[1];
    BYTE* pdstB = (BYTE*)pdst[2];

    const long mask = 1048575;

    for(int y=0; y<height; y++)
    {
        for(int x=0; x<width; x+=2)
        {
            const long R0 = psrc0[sz_pack * x/2 + 2];
            const long G0 = psrc0[sz_pack * x/2 + 1];
            const long B0 = psrc0[sz_pack * x/2    ];

            const long GG0 = (G0 * c[0] + B0 * c[1] + R0 * c[2]  + c[3] ) >> 12;
            const long BB0 = (G0 * c[4] + B0 * c[5] + R0 * c[6]  + c[7] ) >> 12;
            const long RR0 = (G0 * c[8] + B0 * c[9] + R0 * c[10] + c[11]) >> 12;

            pdstG[x] = BYTE_SAT(GG0);
            pdstB[x] = BYTE_SAT(BB0);
            pdstR[x] = BYTE_SAT(RR0);

            const long R1 = psrc1[sz_pack * x/2 + 2];
            const long G1 = psrc1[sz_pack * x/2 + 1];
            const long B1 = psrc1[sz_pack * x/2    ];

            const long GG1 = (G1 * c[0] + B1 * c[1] + R1 * c[2]  + c[3] ) >> 12;
            const long BB1 = (G1 * c[4] + B1 * c[5] + R1 * c[6]  + c[7] ) >> 12;
            const long RR1 = (G1 * c[8] + B1 * c[9] + R1 * c[10] + c[11]) >> 12;

            pdstG[x + 1] = BYTE_SAT(GG1);
            pdstB[x + 1] = BYTE_SAT(BB1);
            pdstR[x + 1] = BYTE_SAT(RR1);
        }
        psrc0 += pitch_src;
        psrc1 += pitch_src;
        pdstR += pitch_dst;
        pdstG += pitch_dst;
        pdstB += pitch_dst;
    }
}
*/

#include "ColorTwist.h"

//------------------------------------------
// カラー画像をRGB画像+混色分離に分離
// BYTE* dstImg		RGB分離画像 (先)
// void** pImgsrc	カラー画像	(元)
// long pitch_dst	iPortあたりの幅
// long pitch_src	iPortあたりの幅(画素数考慮) ※ iPort1台分
// long width		最終画像サイズ
// long height		最終画像サイズ
// const float*const c	混色行列
//------------------------------------------

void IportLib::Copy_PackedColor_Corr_1p_MMX(BYTE* dstImg, void** pImgsrc, long pitch_dst, long pitch_src, long width, long height, const float*const c)
{
    const long sz_pack = 3; // RGB
    const BYTE* psrc0 = (BYTE*)pImgsrc[0]; // even

	BYTE *arrDst[] = {
		&dstImg[width*height*IPORT_COLOR_R],
		&dstImg[width*height*IPORT_COLOR_G],
		&dstImg[width*height*IPORT_COLOR_B],
	};

	rgbplanar_colortwist_asm( arrDst, width, pitch_dst, psrc0, pitch_src, height,c );
}

void IportLib::Copy_PackedColor_Corr_2p_MMX(BYTE* dstImg, void** pImgsrc, long pitch_dst, long pitch_src, long width, long height, const float*const c)
{
    const long sz_pack = 3; // RGB
    const BYTE* psrc0 = (BYTE*)pImgsrc[0]; // even

	BYTE *arrDst[] = {
		&dstImg[width*height*IPORT_COLOR_R],
		&dstImg[width*height*IPORT_COLOR_G],
		&dstImg[width*height*IPORT_COLOR_B],
	};

	rgbplanar_colortwist_asm( arrDst, width, pitch_dst, (const BYTE*const*)pImgsrc, pitch_src, height,c );
}
