#include <crtdbg.h>
#include "ShdCmp_LowPassFilter.h"

#ifdef _M_X64			// x64 ƒRƒ“ƒpƒCƒ‹
	#include "..\ImgLibCLIx64\ShdCmp_NSImg.h"
#else
	#include "..\ImgLibCLIx86\ShdCmp_NSImg.h"
#endif

#ifdef for
#undef for
#endif

using namespace ImgLibCLI;

CLPFMMX::CLPFMMX()
    : my_width(0),
      my_height(0)
{
    ZeroMemory(&my_params, sizeof(my_params));
}

CLPFMMX::~CLPFMMX()
{
    Dispose();
}

BOOL CLPFMMX::Init(long width, long height, const Params& params)
{
    my_width = width;
    my_height = height;
    my_params = params;
    return TRUE;
}

void CLPFMMX::Dispose()
{
    my_width = 0;
    my_height = 0;
}

void CLPFMMX::Exec(const unsigned char* src, unsigned char* dst) const
{
    _ASSERT( 0 != my_width );
    _ASSERT( 0 != my_height );

    LPF(src, dst, my_width, my_height,
        my_params.lpf_width, my_params.lpf_height);
}

