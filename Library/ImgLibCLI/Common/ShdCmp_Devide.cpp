#include <malloc.h>
#include <crtdbg.h>

#include "ShdCmp_Devide.h"


#ifdef _M_X64			// x64 ƒRƒ“ƒpƒCƒ‹
	#include "..\ImgLibCLIx64\ShdCmp_NSImg.h"
#else
	#include "..\ImgLibCLIx86\ShdCmp_NSImg.h"
#endif

#ifdef for
#undef for
#endif

using namespace ImgLibCLI;

CDevideMMX::CDevideMMX()
    : my_width(0),
      my_height(0),
      my_lut(NULL),
      my_func(lut2byte)
{
    ZeroMemory(&my_params, sizeof(my_params));
}

CDevideMMX::~CDevideMMX()
{
    Dispose();
}

BOOL CDevideMMX::Init(long width, long height, const Params& params, BOOL nt)
{
    my_width = width;
    my_height = height;
    my_func = nt ? lut2bytent : lut2byte;

    _ASSERT( NULL == my_lut );
    my_lut = (BYTE*)_aligned_malloc(256 * 256, 32);

    my_params = params;

    for(long i=0; i<256; i++)
    {
        for(long j=0; j<256; j++)
        {
            long value = 0==j
                //? 255
                ? 0
                : i * params.mag / j + params.offset;
            if( value < 0 )        value = 0;
            else if( value > 255 ) value = 255;
            my_lut[(i << 8) + j] = (BYTE)value;
        }
    }

    return TRUE;
}

void CDevideMMX::Dispose()
{
    if( NULL != my_lut )
    {
        _aligned_free(my_lut);
        my_lut = NULL;
    }
    my_width = 0;
    my_height = 0;
    ZeroMemory(&my_params, sizeof(my_params));
}

void CDevideMMX::Exec(const BYTE* num, const BYTE* den, BYTE* dst) const
{
    _ASSERT( 0 != my_width );
    _ASSERT( 0 != my_height );
    lut2byte(dst, num, den, my_width, my_height, my_lut);
}
