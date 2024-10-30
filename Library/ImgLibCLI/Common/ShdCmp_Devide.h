#ifndef SWDETECT_IMM_DEVIDE_H
#define SWDETECT_IMM_DEVIDE_H

#include <Windows.h>

namespace ImgLibCLI
{
    // quo(8u) <-- num(8u) * mag(8u) / den(8u) + offset(8u)
    class CDevideMMX
    {
    public:
        CDevideMMX();
        virtual ~CDevideMMX();

        struct Params
        {
            BYTE mag;
            BYTE offset;
        };

        virtual void Dispose();

        // dst <-- mag * num / den + offset
        virtual BOOL Init(long width, long height, const Params& params, BOOL nt);
        void Exec(const BYTE* num, const BYTE* den, BYTE* dst) const;

    protected:
        long my_width;
        long my_height;
        Params my_params;
        void (__stdcall * my_func)(
            BYTE *	   quotient,
            const BYTE * numerator,
            const BYTE * denominator,
            size_t	   size_x,
            size_t	   size_y,
            const BYTE * look_up_table);
        BYTE* my_lut;
    };
};

#endif
