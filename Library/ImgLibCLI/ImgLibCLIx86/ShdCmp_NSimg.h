#ifndef NSimg_h
#define NSimg_h 1

#ifdef __cplusplus /* C++ */

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

// エッジ検出関数
long __stdcall get_edge(
	const ubyte_t* begin,				/* バッファ */
	std::size_t	   size_x,				/* 幅 */
	std::size_t	   size_y,				/* 高さ */
	std::size_t	   dec_rate,			/* 間引き率 */
	std::size_t	   threshold,			/* スレッショルド */
	std::size_t	   cont_pels,			/* 連続数 */
	int			   direction) throw();	/* 走査方向 */

// 縮小して拡大することによってローパスフィルタをかける関数
void __stdcall LPF(
	const ubyte_t* source,
	ubyte_t*	   destination,
	std::size_t	   size_x,
	std::size_t	   size_y,
	std::size_t	   block_size_x,
	std::size_t	   block_size_y);

// look_up_table[num*256+den]を引いてquoを求める関数
void __stdcall lut2byte(
	ubyte_t*	   quotient,
	const ubyte_t* numerator,
	const ubyte_t* denominator,
	std::size_t	   size_x,
	std::size_t	   size_y,
	const ubyte_t* look_up_table) throw();

// look_up_table[num*256+den]を引いてquoを求める関数(non-temporalバージョン)
void __stdcall lut2bytent(
	ubyte_t*	   quotient,
	const ubyte_t* numerator,
	const ubyte_t* denominator,
	std::size_t	   size_x,
	std::size_t	   size_y,
	const ubyte_t* look_up_table) throw();
    
void __stdcall patch_binarize(
	ubyte_t		 * destination,
	const ubyte_t* source,
	std::size_t	   size_x,
	std::size_t	   size_y,
	std::size_t	   px,
	std::size_t	   py,
	std::size_t	   lx,
	std::size_t	   ly,
	const ubyte_t* thlo,
	const ubyte_t* thhi,
	const uword_t* thrp);

// パッチ二値化関数（以下_1.._8はレベル数)
void __stdcall patch_binarize_1(
	ubyte_t		 * destination,
	const ubyte_t* source,
	std::size_t	   size_x,
	std::size_t	   size_y,
	std::size_t	   px,
	std::size_t	   py,
	std::size_t	   lx,
	std::size_t	   ly,
	const ubyte_t* thlo,
	const ubyte_t* thhi,
	const uword_t* thrp);

void __stdcall patch_binarize_2(
	ubyte_t		 * destination,
	const ubyte_t* source,
	std::size_t	   size_x,
	std::size_t	   size_y,
	std::size_t	   px,
	std::size_t	   py,
	std::size_t	   lx,
	std::size_t	   ly,
	const ubyte_t* thlo,
	const ubyte_t* thhi,
	const uword_t* thrp);

void __stdcall patch_binarize_3(
	ubyte_t		 * destination,
	const ubyte_t* source,
	std::size_t	   size_x,
	std::size_t	   size_y,
	std::size_t	   px,
	std::size_t	   py,
	std::size_t	   lx,
	std::size_t	   ly,
	const ubyte_t* thlo,
	const ubyte_t* thhi,
	const uword_t* thrp);

void __stdcall patch_binarize_4(
	ubyte_t		 * destination,
	const ubyte_t* source,
	std::size_t	   size_x,
	std::size_t	   size_y,
	std::size_t	   px,
	std::size_t	   py,
	std::size_t	   lx,
	std::size_t	   ly,
	const ubyte_t* thlo,
	const ubyte_t* thhi,
	const uword_t* thrp);

void __stdcall patch_binarize_5(
	ubyte_t		 * destination,
	const ubyte_t* source,
	std::size_t	   size_x,
	std::size_t	   size_y,
	std::size_t	   px,
	std::size_t	   py,
	std::size_t	   lx,
	std::size_t	   ly,
	const ubyte_t* thlo,
	const ubyte_t* thhi,
	const uword_t* thrp);

void __stdcall patch_binarize_6(
	ubyte_t		 * destination,
	const ubyte_t* source,
	std::size_t	   size_x,
	std::size_t	   size_y,
	std::size_t	   px,
	std::size_t	   py,
	std::size_t	   lx,
	std::size_t	   ly,
	const ubyte_t* thlo,
	const ubyte_t* thhi,
	const uword_t* thrp);

void __stdcall patch_binarize_7(
	ubyte_t		 * destination,
	const ubyte_t* source,
	std::size_t	   size_x,
	std::size_t	   size_y,
	std::size_t	   px,
	std::size_t	   py,
	std::size_t	   lx,
	std::size_t	   ly,
	const ubyte_t* thlo,
	const ubyte_t* thhi,
	const uword_t* thrp);

void __stdcall patch_binarize_8(
	ubyte_t		 * destination,
	const ubyte_t* source,
	std::size_t	   size_x,
	std::size_t	   size_y,
	std::size_t	   px,
	std::size_t	   py,
	std::size_t	   lx,
	std::size_t	   ly,
	const ubyte_t* thlo,
	const ubyte_t* thhi,
	const uword_t* thrp);

} /* end of extern "C" */

#ifdef _MSC_VER
#define for if (false) ; else for
#endif

#else /* OLD GOOD C */

#include <stddef.h>

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

long __stdcall get_edge(
	const ubyte_t* begin,				/* バッファ */
	size_t		   size_x,				/* 幅 */
	size_t		   size_y,				/* 高さ */
	size_t		   dec_rate,			/* 間引き率 */
	size_t		   threshold,			/* スレッショルド */
	size_t		   cont_pels,			/* 連続数 */
	int			   direction);			/* 走査方向 */

void __stdcall LPF(
	const ubyte_t* source,
	ubyte_t*	   destination,
	size_t		   size_x,
	size_t		   size_y,
	size_t		   block_size_x,
	size_t		   block_size_y);

void __stdcall lut2byte(
	ubyte_t*	   quotient,
	const ubyte_t* numerator,
	const ubyte_t* denominator,
	size_t		   size_x,
	size_t		   size_y,
	const ubyte_t* look_up_table);

void __stdcall patch_binarize(
	ubyte_t		 * destination,
	const ubyte_t* source,
	size_t		   size_x,
	size_t		   size_y,
	size_t		   px,					/* タイルサイズ */
	size_t		   py,
	size_t		   lx,					/* オーバラップ */
	size_t		   ly,
	const ubyte_t* thlo,
	const ubyte_t* thhi,
	const uword_t* thrp);

void __stdcall patch_binarize_1(
	ubyte_t		 * destination,
	const ubyte_t* source,
	size_t		   size_x,
	size_t		   size_y,
	size_t		   px,					/* タイルサイズ */
	size_t		   py,
	size_t		   lx,					/* オーバラップ */
	size_t		   ly,
	const ubyte_t* thlo,
	const ubyte_t* thhi,
	const uword_t* thrp);

void __stdcall patch_binarize_2(
	ubyte_t		 * destination,
	const ubyte_t* source,
	size_t		   size_x,
	size_t		   size_y,
	size_t		   px,					/* タイルサイズ */
	size_t		   py,
	size_t		   lx,					/* オーバラップ */
	size_t		   ly,
	const ubyte_t* thlo,
	const ubyte_t* thhi,
	const uword_t* thrp);

void __stdcall patch_binarize_3(
	ubyte_t		 * destination,
	const ubyte_t* source,
	size_t		   size_x,
	size_t		   size_y,
	size_t		   px,					/* タイルサイズ */
	size_t		   py,
	size_t		   lx,					/* オーバラップ */
	size_t		   ly,
	const ubyte_t* thlo,
	const ubyte_t* thhi,
	const uword_t* thrp);

void __stdcall patch_binarize_4(
	ubyte_t		 * destination,
	const ubyte_t* source,
	size_t		   size_x,
	size_t		   size_y,
	size_t		   px,					/* タイルサイズ */
	size_t		   py,
	size_t		   lx,					/* オーバラップ */
	size_t		   ly,
	const ubyte_t* thlo,
	const ubyte_t* thhi,
	const uword_t* thrp);

void __stdcall patch_binarize_5(
	ubyte_t		 * destination,
	const ubyte_t* source,
	size_t		   size_x,
	size_t		   size_y,
	size_t		   px,					/* タイルサイズ */
	size_t		   py,
	size_t		   lx,					/* オーバラップ */
	size_t		   ly,
	const ubyte_t* thlo,
	const ubyte_t* thhi,
	const uword_t* thrp);

void __stdcall patch_binarize_6(
	ubyte_t		 * destination,
	const ubyte_t* source,
	size_t		   size_x,
	size_t		   size_y,
	size_t		   px,					/* タイルサイズ */
	size_t		   py,
	size_t		   lx,					/* オーバラップ */
	size_t		   ly,
	const ubyte_t* thlo,
	const ubyte_t* thhi,
	const uword_t* thrp);

void __stdcall patch_binarize_7(
	ubyte_t		 * destination,
	const ubyte_t* source,
	size_t		   size_x,
	size_t		   size_y,
	size_t		   px,					/* タイルサイズ */
	size_t		   py,
	size_t		   lx,					/* オーバラップ */
	size_t		   ly,
	const ubyte_t* thlo,
	const ubyte_t* thhi,
	const uword_t* thrp);

void __stdcall patch_binarize_8(
	ubyte_t		 * destination,
	const ubyte_t* source,
	size_t		   size_x,
	size_t		   size_y,
	size_t		   px,					/* タイルサイズ */
	size_t		   py,
	size_t		   lx,					/* オーバラップ */
	size_t		   ly,
	const ubyte_t* thlo,
	const ubyte_t* thhi,
	const uword_t* thrp);
#endif /* C or C++ */

/*
 * local variables:
 * tab-width: 4
 * end:
 * char-encoding: Ｓｈｉｆｔ－ＪＩＳ.
 */
#endif /* !NSimg_h */
