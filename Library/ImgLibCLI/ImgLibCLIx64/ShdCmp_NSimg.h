#ifndef NSimg_h
#define NSimg_h 1

#include <cstddef>
#ifdef _MSC_VER
namespace std { using ::size_t; }
#endif

extern "C" {

typedef unsigned char	   ubyte_t;
typedef signed char		   sbyte_t;
typedef unsigned short	   uword_t;
typedef short			   sword_t;
typedef unsigned long	   ulong_t;
typedef long			   slong_t;
#if !defined _MSC_VER && !defined __TURBOC__
typedef unsigned long long uquad_t;
typedef long long		   squad_t;
#else
typedef unsigned __int64   uquad_t;
typedef __int64			   squad_t;
#endif

// k¬‚µ‚ÄŠg‘å‚·‚é‚±‚Æ‚É‚æ‚Á‚Äƒ[ƒpƒXƒtƒBƒ‹ƒ^‚ğ‚©‚¯‚éŠÖ”
void __stdcall LPF(
	const ubyte_t* source,
	ubyte_t*	   destination,
	std::size_t	   size_x,
	std::size_t	   size_y,
	std::size_t	   block_size_x,
	std::size_t	   block_size_y);

// look_up_table[num*256+den]‚ğˆø‚¢‚Äquo‚ğ‹‚ß‚éŠÖ”
void __cdecl lut2byte(
	ubyte_t*	   quotient,
	const ubyte_t* numerator,
	const ubyte_t* denominator,
	std::size_t	   size_x,
	std::size_t	   size_y,
	const ubyte_t* look_up_table);

// look_up_table[num*256+den]‚ğˆø‚¢‚Äquo‚ğ‹‚ß‚éŠÖ”(non-temporalƒo[ƒWƒ‡ƒ“)
void __cdecl lut2bytent(
	ubyte_t*	   quotient,
	const ubyte_t* numerator,
	const ubyte_t* denominator,
	std::size_t	   size_x,
	std::size_t	   size_y,
	const ubyte_t* look_up_table);
    
} /* end of extern "C" */

#ifdef _MSC_VER
#define for if (false) ; else for
#endif

/*
 * local variables:
 * tab-width: 4
 * end:
 * char-encoding: ‚r‚ˆ‚‰‚†‚”|‚i‚h‚r.
 */
#endif /* !NSimg_h */
