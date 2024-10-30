#ifndef SWDETECT_IMM_DESHADE_H
#define SWDETECT_IMM_DESHADE_H

#include <Windows.h>
#include "ShdCmp_LowPassFilter.h"
#include "ShdCmp_Devide.h"

namespace ImgLibCLI
{
    class CDeShadeMMX
    {
    public:
        CDeShadeMMX();
        virtual ~CDeShadeMMX();

        struct Params
        {
            long lpf_width;
            long lpf_height;
            BYTE target;
        };

        virtual void Dispose();

        virtual BOOL Init(long width, long height, const Params& params);
        void Exec(const BYTE* src, BYTE* dst) const;									// �V�F�[�f�B���O�␳(�S��)
		void ExecEdgeMask(long nEdgeL, long nEdgeR, const BYTE* src, BYTE* dst) const;	// �V�F�[�f�B���O�␳(�G�b�W�O������)
		void ExecEdgeCopy(long nEdgeL, long nEdgeR, const BYTE* src, BYTE* dst) const;	// �V�F�[�f�B���O�␳(�G�b�W�O�͌��摜)

    protected:
        long my_width;
        long my_height;
        CLPFMMX my_lpf;
        CDevideMMX my_devide;

        BYTE* my_lpf_image;

        Params my_params;
    };
};

#endif
