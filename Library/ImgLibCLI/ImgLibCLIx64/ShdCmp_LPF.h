#ifndef LPF_h
#define LPF_h 1

#include <memory>

#pragma warning(disable: 4799) // "No EMMS instruction."

// #define MACRO_REDUCE

extern "C" void __cdecl reduce_0(
	const ubyte_t *srcptr,
	uword_t		  *dstptr,
	uword_t		  *dstend,
	ulong_t		   size_x,
	ulong_t		   bsiz_r,
	ulong_t		   blks_x);
extern "C" void __cdecl reduce_1(
	const ubyte_t* srcptr,
	uword_t*	   dstptr,
	uword_t*	   dstend,
	ulong_t		   size_x,
	ulong_t		   bsiz_r,
	ulong_t		   blks_x);
extern "C" void __cdecl reduce_2(
	const ubyte_t* srcptr,
	uword_t*	   dstptr,
	uword_t*	   dstend,
	ulong_t		   size_x,
	ulong_t		   bsiz_r,
	ulong_t		   blks_x);
extern "C" void __cdecl reduce_3(
	const ubyte_t* srcptr,
	uword_t*	   dstptr,
	uword_t*	   dstend,
	ulong_t		   size_x,
	ulong_t		   bsiz_r,
	ulong_t		   blks_x);
extern "C" void __cdecl reduce_4(
	const ubyte_t* srcptr,
	ulong_t*	   dstptr,
	ulong_t*	   dstend,
	ulong_t		   size_x,
	ulong_t		   bsiz_r,
	ulong_t		   blks_x);
extern "C" void __cdecl reduce_5(
	const ubyte_t* srcptr,
	ulong_t*	   dstptr,
	ulong_t*	   dstend,
	ulong_t		   size_x,
	ulong_t		   bsiz_r,
	ulong_t		   blks_x);

// #undef MACRO_REDUCE

extern "C" ubyte_t* __cdecl expand_xy(
	ubyte_t*	   dstptr,
	ulong_t*	   srcptr,
	const ulong_t* difptr,
	ulong_t		   size_x,
	ulong_t		   bsiz_y,
	ulong_t		   scale);

extern "C" ulong_t __cdecl iscalb(ulong_t);

// #define MACRO_EXPAND_X

extern "C" const uword_t* __cdecl expand_x_0(
	const uword_t* srcptr,
	const uword_t* srcend,
	ulong_t*	   dstptr,
	ulong_t*	   dstend,
	ulong_t		   bsiz_y);
extern "C" const uword_t* __cdecl expand_xdiff_0(
	const uword_t* srcptr,
	const uword_t* srcend,
	ulong_t*	   dstptr,
	ulong_t*	   dstend,
	const ulong_t* basptr,
	ulong_t		   bsiz_y);
const uword_t* __stdcall expand_x_1(
	const uword_t* srcptr,
	const uword_t* srcend,
	ulong_t*	   dstptr,
	ulong_t*	   dstend,
	ulong_t		   bsiz_y);
const uword_t* __stdcall expand_xdiff_1(
	const uword_t* srcptr,
	const uword_t* srcend,
	ulong_t*	   dstptr,
	ulong_t*	   dstend,
	const ulong_t* basptr,
	ulong_t		   bsiz_y);
const uword_t* __stdcall expand_x_2(
	const uword_t* srcptr,
	const uword_t* srcend,
	ulong_t*	   dstptr,
	ulong_t*	   dstend,
	ulong_t		   bsiz_y);
const uword_t* __stdcall expand_xdiff_2(
	const uword_t* srcptr,
	const uword_t* srcend,
	ulong_t*	   dstptr,
	ulong_t*	   dstend,
	const ulong_t* basptr,
	ulong_t		   bsiz_y);
const uword_t* __stdcall expand_x_3(
	const uword_t* srcptr,
	const uword_t* srcend,
	ulong_t*	   dstptr,
	ulong_t*	   dstend,
	ulong_t		   bsiz_y);
const uword_t* __stdcall expand_xdiff_3(
	const uword_t* srcptr,
	const uword_t* srcend,
	ulong_t*	   dstptr,
	ulong_t*	   dstend,
	const ulong_t* basptr,
	ulong_t		   bsiz_y);
extern "C" const ulong_t* __cdecl expand_x_4(
	const ulong_t* srcptr,
	const ulong_t* srcend,
	ulong_t*	   dstptr,
	ulong_t*	   dstend,
	ulong_t		   bsiz_y);
extern "C" const ulong_t* __cdecl expand_xdiff_4(
	const ulong_t* srcptr,
	const ulong_t* srcend,
	ulong_t*	   dstptr,
	ulong_t*	   dstend,
	const ulong_t* basptr,
	ulong_t		   bsiz_y);
extern "C" const ulong_t* __cdecl expand_x_5(
	const ulong_t* srcptr,
	const ulong_t* srcend,
	ulong_t*	   dstptr,
	ulong_t*	   dstend,
	ulong_t		   bsiz_y);
extern "C" const ulong_t* __cdecl expand_xdiff_5(
	const ulong_t* srcptr,
	const ulong_t* srcend,
	ulong_t*	   dstptr,
	ulong_t*	   dstend,
	const ulong_t* basptr,
	ulong_t		   bsiz_y);

// #undef MACRO_EXPAND_X

// #define MACRO_EXPAND

void __stdcall expand_0(
	const uword_t* srcptr,
	const uword_t* srcend,
	ubyte_t*	   dstptr,
	ubyte_t*	   dstend,
	ulong_t*	   tm0ptr,
	ulong_t		   size_x,
	ulong_t		   bsiz_y,
	ulong_t		   blks_x);
void __stdcall expand_1(
	const uword_t* srcptr,
	const uword_t* srcend,
	ubyte_t*	   dstptr,
	ubyte_t*	   dstend,
	ulong_t*	   tm0ptr,
	ulong_t		   size_x,
	ulong_t		   bsiz_y,
	ulong_t		   blks_x);
void __stdcall expand_2(
	const uword_t* srcptr,
	const uword_t* srcend,
	ubyte_t*	   dstptr,
	ubyte_t*	   dstend,
	ulong_t*	   tm0ptr,
	ulong_t		   size_x,
	ulong_t		   bsiz_y,
	ulong_t		   blks_x);
void __stdcall expand_3(
	const uword_t* srcptr,
	const uword_t* srcend,
	ubyte_t*	   dstptr,
	ubyte_t*	   dstend,
	ulong_t*	   tm0ptr,
	ulong_t		   size_x,
	ulong_t		   bsiz_y,
	ulong_t		   blks_x);
void __stdcall expand_4(
	const ulong_t* srcptr,
	const ulong_t* srcend,
	ubyte_t*	   dstptr,
	ubyte_t*	   dstend,
	ulong_t*	   tm0ptr,
	ulong_t		   size_x,
	ulong_t		   bsiz_y,
	ulong_t		   blks_x);
void __stdcall expand_5(
	const ulong_t* srcptr,
	const ulong_t* srcend,
	ubyte_t*	   dstptr,
	ubyte_t*	   dstend,
	ulong_t*	   tm0ptr,
	ulong_t		   size_x,
	ulong_t		   bsiz_y,
	ulong_t		   blks_x);

// #undef MACRO_EXPAND

#pragma warning(default: 4799)

extern "C" void __cdecl lpf_emms();

/*
 * local variables:
 * tab-width: 4
 * end:
 * char-encoding: ÇrÇàÇâÇÜÇîÅ|ÇiÇhÇr.
 */
#endif /* !LPF_h */
