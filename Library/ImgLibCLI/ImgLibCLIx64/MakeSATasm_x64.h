extern "C" void __cdecl VertAdd(
		const unsigned char* data,
		int w,
		unsigned long * acc);
extern "C" void __cdecl VertAddLimit(
		const unsigned char* data,
		int w,
		unsigned long * acc,
		unsigned char lower,
		unsigned char upper);
extern "C" void __cdecl HorzAccum(
    unsigned long * va,
    int w,
    unsigned long * sat);
extern "C" void __cdecl HorzAccumStep8(
    unsigned long * va,
    int w,
    int step,						// 8の倍数
    unsigned long * sat);
extern "C" void __cdecl HorzAccumStep(
		unsigned long * va,
		int w,
		int step,						// 1～任意
		unsigned long * sat);
extern "C" void __cdecl MakeSAT(
    const unsigned char* data,        // 対象画像データ 16 byte boundary
    int w,           // 幅
    int h,
    unsigned long * out);		// SAT
extern "C" void __cdecl MakeSATline(
    const unsigned char* data,        // 対象画像データ 16 byte boundary
    int w,           // 幅
    unsigned long * out,
    unsigned long * upper);
extern "C" void __cdecl LUTline(
    const unsigned char* data,
    int w,
    unsigned char * out,
    unsigned char * lut);
