#ifndef SWDETECT_MMX_MAKESAT_H
#define SWDETECT_MMX_MAKESAT_H

namespace SwdetectMMX
{
	void __stdcall VertAdd(
		const unsigned char* data,
		int w,
		unsigned long * acc);
	void __stdcall VertAddLimit(
		const unsigned char* data,
		int w,
		unsigned long * acc,
		unsigned char lower,
		unsigned char upper);
	void __stdcall HorzAccum(
		unsigned long * va,
		int w,
		unsigned long * sat);
	void __stdcall HorzAccumStep8(
		unsigned long * va,
		int w,
		int step,						// 8の倍数
		unsigned long * sat);
	void __stdcall HorzAccumStep(
		unsigned long * va,
		int w,
		int step,						// 1～任意
		unsigned long * sat);
    void __stdcall MakeSAT(
        const unsigned char* data,        // 対象画像データ 16 byte boundary
        int w,           // 幅
		int h,
        unsigned long * out);		// SAT
	void __stdcall MakeSATline(
        const unsigned char* data,        // 対象画像データ 16 byte boundary
        int w,           // 幅
        unsigned long * out,
		unsigned long * upper);
	void __stdcall LUTline(
		const unsigned char* data,
		int w,
		unsigned char * out, 
		unsigned char * lut);
};

#endif