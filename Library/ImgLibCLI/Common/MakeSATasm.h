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
		int step,						// 8�̔{��
		unsigned long * sat);
	void __stdcall HorzAccumStep(
		unsigned long * va,
		int w,
		int step,						// 1�`�C��
		unsigned long * sat);
    void __stdcall MakeSAT(
        const unsigned char* data,        // �Ώۉ摜�f�[�^ 16 byte boundary
        int w,           // ��
		int h,
        unsigned long * out);		// SAT
	void __stdcall MakeSATline(
        const unsigned char* data,        // �Ώۉ摜�f�[�^ 16 byte boundary
        int w,           // ��
        unsigned long * out,
		unsigned long * upper);
	void __stdcall LUTline(
		const unsigned char* data,
		int w,
		unsigned char * out, 
		unsigned char * lut);
};

#endif