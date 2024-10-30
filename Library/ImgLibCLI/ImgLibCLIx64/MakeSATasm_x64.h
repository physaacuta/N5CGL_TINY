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
    int step,						// 8�̔{��
    unsigned long * sat);
extern "C" void __cdecl HorzAccumStep(
		unsigned long * va,
		int w,
		int step,						// 1�`�C��
		unsigned long * sat);
extern "C" void __cdecl MakeSAT(
    const unsigned char* data,        // �Ώۉ摜�f�[�^ 16 byte boundary
    int w,           // ��
    int h,
    unsigned long * out);		// SAT
extern "C" void __cdecl MakeSATline(
    const unsigned char* data,        // �Ώۉ摜�f�[�^ 16 byte boundary
    int w,           // ��
    unsigned long * out,
    unsigned long * upper);
extern "C" void __cdecl LUTline(
    const unsigned char* data,
    int w,
    unsigned char * out,
    unsigned char * lut);
