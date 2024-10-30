#include <crtdbg.h>

#include "ShdCmp_DeShade.h"

using namespace ImgLibCLI;

CDeShadeMMX::CDeShadeMMX()
    : my_width(0),
      my_height(0),
      my_lpf_image(NULL)
{
    ZeroMemory(&my_params, sizeof(my_params));
}

CDeShadeMMX::~CDeShadeMMX()
{
    Dispose();
}

BOOL CDeShadeMMX::Init(long width, long height, const Params& params)
{
    my_width = width;
    my_height = height;
    my_params = params;

    CLPFMMX::Params lpf_params;
    lpf_params.lpf_width = params.lpf_width;
    lpf_params.lpf_height = params.lpf_height;
    if( !my_lpf.Init(width, height, lpf_params) ) return FALSE;

    CDevideMMX::Params devide_params;
    devide_params.mag = params.target;
    devide_params.offset = 0;
    if( !my_devide.Init(width, height, devide_params, TRUE) ) return FALSE;

    my_lpf_image = (BYTE*)_aligned_malloc(width * height, 32);

    return TRUE;
}

void CDeShadeMMX::Dispose()
{
    my_width = 0;
    my_height = 0;

    my_lpf.Dispose();
    my_devide.Dispose();
    ZeroMemory(&my_params, sizeof(my_params));
    if( NULL != my_lpf_image ) _aligned_free(my_lpf_image);
    my_lpf_image = NULL;
}
//------------------------------------------
// シェーディング補正(全部)
//------------------------------------------
void CDeShadeMMX::Exec(const BYTE* src, BYTE* dst) const
{
    _ASSERT( NULL != my_lpf_image );
    my_lpf.Exec(src, my_lpf_image);
    my_devide.Exec(src, my_lpf_image, dst);
}


//------------------------------------------
// シェーディング補正(エッジ外を黒埋め)
//------------------------------------------
void CDeShadeMMX::ExecEdgeMask(long nEdgeL, long nEdgeR, const BYTE* src, BYTE* dst) const
{
    _ASSERT( NULL != my_lpf_image );
    my_lpf.Exec(src, my_lpf_image);

	// エッジ外マスク
	int nIdx = 0;
	int nEdgeSizeR = my_width-nEdgeR;
	for(int ii=0; ii<my_height; ii++) {

		memset( &my_lpf_image[nIdx], 0x00, nEdgeL );
		memset( &my_lpf_image[nIdx+nEdgeR], 0x00, nEdgeSizeR );
		nIdx += my_width;
	}


    my_devide.Exec(src, my_lpf_image, dst);
}
//------------------------------------------
// シェーディング補正(エッジ外は元画像)
//------------------------------------------
void CDeShadeMMX::ExecEdgeCopy(long nEdgeL, long nEdgeR, const BYTE* src, BYTE* dst) const
{   
	Exec(src, dst);

	// エッジ外コピー
	int nIdx = 0;
	int nEdgeSizeR = my_width-nEdgeR;
	for(int ii=0; ii<my_height; ii++) {

		memcpy( &dst[nIdx], &src[nIdx], nEdgeL );
		memcpy( &dst[nIdx+nEdgeR], &src[nIdx], nEdgeSizeR );
		nIdx += my_width;
	}
}