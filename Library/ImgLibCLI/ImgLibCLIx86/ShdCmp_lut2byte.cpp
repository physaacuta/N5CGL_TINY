#ifdef TEST
	#include <iomanip>
	#include <iostream>
	#include <cstdlib>
	#ifdef _MSC_VER
		namespace std { using ::rand; }
	#endif
#endif
#include "ShdCmp_NSimg.h"

#if 0

void __stdcall lut2byte(
	ubyte_t*	   quo,
	const ubyte_t* num,
	const ubyte_t* den,
	std::size_t	   szx,
	std::size_t	   szy,
	const ubyte_t* lut) throw()
{
	szx *= szy;
	num += szx;
	den += szx;
	quo += szx;
	for (int n = -int(szx); n < 0; ++n)
		quo[n] = lut[num[n] << 8 | den[n]];
}

#elif 0

__declspec(naked) void __stdcall lut2byte(
	ubyte_t*,
	const ubyte_t*,
	const ubyte_t*,
	std::size_t,
	std::size_t,
	const ubyte_t*) throw()
{
	__asm
	  {
		push	ebp;
		push	edi;
		push	esi;
		push	ebx;
		mov		edi, 20[esp];
		mov		esi, 24[esp];
		mov		ebx, 28[esp];
		mov		ebp, 32[esp];
		imul	ebp, 36[esp];
		mov		edx, 40[esp];
		dec		edi;
		jmp		L0;
	L0:	movzx	eax, BYTE PTR [esi];
		inc		esi;
		movzx	ecx, BYTE PTR [ebx];
		shl		eax, 8;
		inc		edi;
		or		eax, ecx;
		inc		ebx;
		mov		al, BYTE PTR [edx + eax];
		dec		ebp;
		mov		[edi], al;
		jnz		L0;
		pop		ebx;
		pop		esi;
		pop		edi;
		pop		ebp;
		ret		24;
	  }
}

#else

__declspec(naked) void __stdcall lut2byte(
	ubyte_t*,
	const ubyte_t*,
	const ubyte_t*,
	std::size_t,
	std::size_t,
	const ubyte_t*) throw()
{
	__asm
	  {
		push		edi;
		push		esi;
		push		ebx;

		mov			edi, 16[esp];
		mov			ecx, 20[esp];
		mov			edx, 24[esp];
		mov			esi, 28[esp];
		imul		esi, 32[esp];
		mov			ebx, 36[esp];
		sub			esi, 8;
		jb			L1;

	L0:	movq		mm1, [ecx];
		add			ecx, 8;
		movq		mm2, [edx];
		add			edx, 8;
		movq		mm3, mm2;
		punpcklbw	mm2, mm1;
		punpckhbw	mm3, mm1;

		movd		eax, mm2;
		psrlq		mm2, 16;
		and			eax, 0xFFFF;
		movzx		eax, BYTE PTR [ebx + eax];
		movd		mm4, eax;

		movd		eax, mm2;
		psrlq		mm2, 16;
		and			eax, 0xFFFF;
		movzx		eax, BYTE PTR [ebx + eax];
		movd		mm1, eax;
		psllq		mm1, 8;
		por			mm4, mm1;

		movd		eax, mm2;
		psrlq		mm2, 16;
		and			eax, 0xFFFF;
		movzx		eax, BYTE PTR [ebx + eax];
		movd		mm1, eax;
		psllq		mm1, 16;
		por			mm4, mm1;

		movd		eax, mm2;
		and			eax, 0xFFFF;
		movzx		eax, BYTE PTR [ebx + eax];
		movd		mm1, eax;
		psllq		mm1, 24;
		por			mm4, mm1;

		movd		eax, mm3;
		psrlq		mm3, 16;
		and			eax, 0xFFFF;
		movzx		eax, BYTE PTR [ebx + eax];
		movd		mm1, eax;
		psllq		mm1, 32;
		por			mm4, mm1;

		movd		eax, mm3;
		psrlq		mm3, 16;
		and			eax, 0xFFFF;
		movzx		eax, BYTE PTR [ebx + eax];
		movd		mm1, eax;
		psllq		mm1, 40;
		por			mm4, mm1;

		movd		eax, mm3;
		psrlq		mm3, 16;
		and			eax, 0xFFFF;
		movzx		eax, BYTE PTR [ebx + eax];
		movd		mm1, eax;
		psllq		mm1, 48;
		por			mm4, mm1;

		movd		eax, mm3;
		and			eax, 0xFFFF;
		movzx		eax, BYTE PTR [ebx + eax];
		movd		mm1, eax;
		psllq		mm1, 56;
		por			mm4, mm1;

		movq		[edi], mm4;
		add			edi, 8;
		sub			esi, 8;
		jae			L0;

	L1:	add			esi, 8;
		jz			L3;
		push		ebp;
		dec			edi;

	L2:	movzx		eax, BYTE PTR [ecx];
		inc			ecx;
		movzx		ebp, BYTE PTR [edx];
		shl			eax, 8;
		inc			edi;
		or			eax, ebp;
		inc			edx;
		mov			al, BYTE PTR [ebx + eax];
		dec			esi;
		mov			[edi], al;
		jnz			L2;

		pop			ebp;

	L3:	pop			ebx;
		pop			esi;
		pop			edi;
		emms;
		ret			24;
	  }
}

__declspec(naked) void __stdcall lut2bytent(
	ubyte_t*,
	const ubyte_t*,
	const ubyte_t*,
	std::size_t,
	std::size_t,
	const ubyte_t*) throw()
{
	__asm
	  {
		push		edi;
		push		esi;
		push		ebx;

		mov			edi, 16[esp];
		mov			ecx, 20[esp];
		mov			edx, 24[esp];
		mov			esi, 28[esp];
		imul		esi, 32[esp];
		mov			ebx, 36[esp];
		sub			esi, 8;
		jb			L1;

	L0:	
		movq		mm1, [ecx];
		add			ecx, 8;
		movq		mm2, [edx];
		add			edx, 8;
		movq		mm3, mm2;
		punpcklbw	mm2, mm1;
		punpckhbw	mm3, mm1;

		movd		eax, mm2;
		psrlq		mm2, 16;
		and			eax, 0xFFFF;
		movzx		eax, BYTE PTR [ebx + eax];
		movd		mm4, eax;

		movd		eax, mm2;
		psrlq		mm2, 16;
		and			eax, 0xFFFF;
		movzx		eax, BYTE PTR [ebx + eax];
		movd		mm1, eax;
		psllq		mm1, 8;
		por			mm4, mm1;

		movd		eax, mm2;
		psrlq		mm2, 16;
		and			eax, 0xFFFF;
		movzx		eax, BYTE PTR [ebx + eax];
		movd		mm1, eax;
		psllq		mm1, 16;
		por			mm4, mm1;

		movd		eax, mm2;
		and			eax, 0xFFFF;
		movzx		eax, BYTE PTR [ebx + eax];
		movd		mm1, eax;
		psllq		mm1, 24;
		por			mm4, mm1;

		movd		eax, mm3;
		psrlq		mm3, 16;
		and			eax, 0xFFFF;
		movzx		eax, BYTE PTR [ebx + eax];
		movd		mm1, eax;
		psllq		mm1, 32;
		por			mm4, mm1;

		movd		eax, mm3;
		psrlq		mm3, 16;
		and			eax, 0xFFFF;
		movzx		eax, BYTE PTR [ebx + eax];
		movd		mm1, eax;
		psllq		mm1, 40;
		por			mm4, mm1;

		movd		eax, mm3;
		psrlq		mm3, 16;
		and			eax, 0xFFFF;
		movzx		eax, BYTE PTR [ebx + eax];
		movd		mm1, eax;
		psllq		mm1, 48;
		por			mm4, mm1;

		movd		eax, mm3;
		and			eax, 0xFFFF;
		movzx		eax, BYTE PTR [ebx + eax];
		movd		mm1, eax;
		psllq		mm1, 56;
		por			mm4, mm1;

		movntq		[edi], mm4;
		add			edi, 8;
		sub			esi, 8;
		jae			L0;

	L1:	add			esi, 8;
		jz			L3;
		push		ebp;
		dec			edi;

	L2:	movzx		eax, BYTE PTR [ecx];
		inc			ecx;
		movzx		ebp, BYTE PTR [edx];
		shl			eax, 8;
		inc			edi;
		or			eax, ebp;
		inc			edx;
		mov			al, BYTE PTR [ebx + eax];
		dec			esi;
		mov			[edi], al;
		jnz			L2;

		pop			ebp;

	L3:	pop			ebx;
		pop			esi;
		pop			edi;
		emms;
		ret			24;
	  }
}
#endif

#ifdef TEST

static unsigned random(unsigned n)
{
	return unsigned(n * std::rand() / (RAND_MAX + 1.0));
}

uquad_t lut2byte_timing(
	ubyte_t*	   quo,
	const ubyte_t* num,
	const ubyte_t* den,
	std::size_t	   szx,
	std::size_t	   szy,
	const ubyte_t* lut)
{
	uquad_t clock;
	__asm
	  {
		rdtsc;
		mov		DWORD PTR clock[0], eax;
		mov		DWORD PTR clock[4], edx;
	  }
	lut2byte(quo, num, den, szx, szy, lut);
	__asm
	  {
		rdtsc;
		sub		eax, DWORD PTR clock[0];
		sbb		edx, DWORD PTR clock[4];
		mov		DWORD PTR clock[0], eax;
		mov		DWORD PTR clock[4], edx;
	  }
	return clock;
}

int main()
{
	std::size_t sx = 1024, sy = 512;
	ubyte_t* lut = new ubyte_t[65536];
	for (std::size_t n = 0; n < 256; ++n)
		for (std::size_t d = 0; d < 256; ++d)
		  {
			uword_t q = d != 0 ? (n * 16 + d / 2) / d : 255;
			lut[n << 8 | d] = q <= 255 ? q : 255;
		  }
	ubyte_t* num = new ubyte_t[sx * sy];
	ubyte_t* den = new ubyte_t[sx * sy];
	std::cout << "*RANDOM TEST*" << std::endl;
	for (std::size_t iter = 0; iter < 10; ++iter)
	  {
		for (std::size_t n = 0; n < sx * sy; ++n)
		  {
			num[n] = ubyte_t(random(256));
			den[n] = ubyte_t(random(256));
		  }
		squad_t clock = lut2byte_timing(num, num, den, sx, sy, lut);
		std::cout
			<< "Iter #" << iter << ": "
			<< std::fixed << std::setprecision(2) << std::setw(6) << double(clock) / (1E6 / 3)
			<< "msec." << std::endl;
	  }
	std::cout << "*SEQUENCIAL TEST*" << std::endl;
	for (std::size_t iter = 0; iter < 10; ++iter)
	  {
		for (std::size_t n = 0; n < sx * sy; ++n)
		  {
			num[n] = ubyte_t(n);
			den[n] = ubyte_t(n);
		  }
		squad_t clock = lut2byte_timing(num, num, den, sx, sy, lut);
		std::cout
			<< "Iter #" << iter << ": "
			<< std::fixed << std::setprecision(2) << std::setw(6) << double(clock) / (1E6 / 3)
			<< "msec." << std::endl;
	  }
	delete[] den;
	delete[] num;
	delete[] lut;
	return 0;
}

#endif

// local variables:
// tab-width: 4
// end:
// char-encoding: ‚r‚ˆ‚‰‚†‚”|‚i‚h‚r.
