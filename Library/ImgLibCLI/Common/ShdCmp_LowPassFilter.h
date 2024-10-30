#ifndef SWDETECT_IMM_LPF_H
#define SWDETECT_IMM_LPF_H

#include <Windows.h>

namespace ImgLibCLI
{
    class CLPFMMX
    {
    public:
        CLPFMMX();
        virtual ~CLPFMMX();

        struct Params
        {
            long lpf_width;
            long lpf_height;
        };

        virtual void Dispose();

        virtual BOOL Init(long width, long height, const Params& params);
        void Exec(const BYTE* src, BYTE* dst) const;

        virtual BOOL Set(const Params& params)
            {
                my_params = params;
                return TRUE;
            }
        
    protected:
        long my_width;
        long my_height;
        Params my_params;
    };
};

#endif
