#include "ShdCmp_NSimg.h"
#include "ShdCmp_LPF.h"

#pragma warning(disable: 4799) // "No EMMS instruction."

// #define MACRO_EXPAND_X

const uword_t* __stdcall expand_x_1(
	const uword_t* srcptr,
	const uword_t* srcend,
	ulong_t*	   dstptr,
	ulong_t*	   dstend,
	ulong_t		   bsiz_y)
{
	// src には blks_x = size_x / bsiz_x 個分のデータが含まれる.
	// size_x % bsiz_x != 0 のとき, 右側の半端なピクセルには, すべて最
	// 後のデータを埋めることにした.

	std::size_t scal_y = iscalb(bsiz_y);
	ulong_t s0 = *srcptr++ << (!0 ? scal_y : 0), ss = s0 << 1;

	// 左端.
	for (ulong_t x = 0; x < 1 << (1 - 1); ++x)
	{
		*dstptr++ = ss;
	}

	// 本体 (blks_x - 1) * bsiz_x ピクセル.
	while (srcptr < srcend)
	{
		ulong_t s1 = *srcptr++ << (!0 ? scal_y : 0);
		ulong_t ds = s1 - s0;
		s0 = s1;
		for (ulong_t x = 0; x < 1 << 1; ++x)
		{
			*dstptr++ = ss, ss += ds;
		}
	}

	// 右端.
	while (dstptr < dstend)
	{
		*dstptr++ = ss;
	}

	return srcptr;
}

const uword_t* __stdcall expand_xdiff_1(
	const uword_t* srcptr,
	const uword_t* srcend,
	ulong_t*	   dstptr,
	ulong_t*	   dstend,
	const ulong_t* basptr,
	ulong_t		   bsiz_y)
{
	// src には blks_x = size_x / bsiz_x 個分のデータが含まれる.
	// size_x % bsiz_x != 0 のとき, 右側の半端なピクセルには, すべて最
	// 後のデータを埋めることにした.

	std::size_t scal_y = iscalb(bsiz_y);
	ulong_t s0 = *srcptr++ << (!1 ? scal_y : 0), ss = s0 << 1;

	// 左端.
	for (ulong_t x = 0; x < 1 << (1 - 1); ++x)
	{
		*dstptr++ = ss - (*basptr++ >> scal_y);
	}

	// 本体 (blks_x - 1) * bsiz_x ピクセル.
	while (srcptr < srcend)
	{
		ulong_t s1 = *srcptr++ << (!1 ? scal_y : 0);
		ulong_t ds = s1 - s0;
		s0 = s1;
		for (ulong_t x = 0; x < 1 << 1; ++x)
		{
			*dstptr++ = ss - (*basptr++ >> scal_y), ss += ds;
		}
	}

	// 右端.
	while (dstptr < dstend)
	{
		*dstptr++ = ss - (*basptr++ >> scal_y);
	}

	return srcptr;
}

const uword_t* __stdcall expand_x_2(
	const uword_t* srcptr,
	const uword_t* srcend,
	ulong_t*	   dstptr,
	ulong_t*	   dstend,
	ulong_t		   bsiz_y)
{
	// src には blks_x = size_x / bsiz_x 個分のデータが含まれる.
	// size_x % bsiz_x != 0 のとき, 右側の半端なピクセルには, すべて最
	// 後のデータを埋めることにした.

	std::size_t scal_y = iscalb(bsiz_y);
	ulong_t s0 = *srcptr++ << (!0 ? scal_y : 0), ss = s0 << 2;

	// 左端.
	for (ulong_t x = 0; x < 1 << (2 - 1); ++x)
	{
		*dstptr++ = ss;
	}

	// 本体 (blks_x - 1) * bsiz_x ピクセル.
	while (srcptr < srcend)
	{
		ulong_t s1 = *srcptr++ << (!0 ? scal_y : 0);
		ulong_t ds = s1 - s0;
		s0 = s1;
		for (ulong_t x = 0; x < 1 << 2; ++x)
		{
			*dstptr++ = ss, ss += ds;
		}
	}

	// 右端.
	while (dstptr < dstend)
	{
		*dstptr++ = ss;
	}

	return srcptr;
}

const uword_t* __stdcall expand_xdiff_2(
	const uword_t* srcptr,
	const uword_t* srcend,
	ulong_t*	   dstptr,
	ulong_t*	   dstend,
	const ulong_t* basptr,
	ulong_t		   bsiz_y)
{
	// src には blks_x = size_x / bsiz_x 個分のデータが含まれる.
	// size_x % bsiz_x != 0 のとき, 右側の半端なピクセルには, すべて最
	// 後のデータを埋めることにした.

	std::size_t scal_y = iscalb(bsiz_y);
	ulong_t s0 = *srcptr++ << (!1 ? scal_y : 0), ss = s0 << 2;

	// 左端.
	for (ulong_t x = 0; x < 1 << (2 - 1); ++x)
	{
		*dstptr++ = ss - (*basptr++ >> scal_y);
	}

	// 本体 (blks_x - 1) * bsiz_x ピクセル.
	while (srcptr < srcend)
	{
		ulong_t s1 = *srcptr++ << (!1 ? scal_y : 0);
		ulong_t ds = s1 - s0;
		s0 = s1;
		for (ulong_t x = 0; x < 1 << 2; ++x)
		{
			*dstptr++ = ss - (*basptr++ >> scal_y), ss += ds;
		}
	}

	// 右端.
	while (dstptr < dstend)
	{
		*dstptr++ = ss - (*basptr++ >> scal_y);
	}

	return srcptr;
}

const uword_t* __stdcall expand_x_3(
	const uword_t* srcptr,
	const uword_t* srcend,
	ulong_t*	   dstptr,
	ulong_t*	   dstend,
	ulong_t		   bsiz_y)
{
	// src には blks_x = size_x / bsiz_x 個分のデータが含まれる.
	// size_x % bsiz_x != 0 のとき, 右側の半端なピクセルには, すべて最
	// 後のデータを埋めることにした.

	std::size_t scal_y = iscalb(bsiz_y);
	ulong_t s0 = *srcptr++ << (!0 ? scal_y : 0), ss = s0 << 3;

	// 左端.
	for (ulong_t x = 0; x < 1 << (3 - 1); ++x)
	{
		*dstptr++ = ss;
	}

	// 本体 (blks_x - 1) * bsiz_x ピクセル.
	while (srcptr < srcend)
	{
		ulong_t s1 = *srcptr++ << (!0 ? scal_y : 0);
		ulong_t ds = s1 - s0;
		s0 = s1;
		for (ulong_t x = 0; x < 1 << 3; ++x)
		{
			*dstptr++ = ss, ss += ds;
		}
	}

	// 右端.
	while (dstptr < dstend)
	{
		*dstptr++ = ss;
	}

	return srcptr;
}

const uword_t* __stdcall expand_xdiff_3(
	const uword_t* srcptr,
	const uword_t* srcend,
	ulong_t*	   dstptr,
	ulong_t*	   dstend,
	const ulong_t* basptr,
	ulong_t		   bsiz_y)
{
	// src には blks_x = size_x / bsiz_x 個分のデータが含まれる.
	// size_x % bsiz_x != 0 のとき, 右側の半端なピクセルには, すべて最
	// 後のデータを埋めることにした.

	std::size_t scal_y = iscalb(bsiz_y);
	ulong_t s0 = *srcptr++ << (!1 ? scal_y : 0), ss = s0 << 3;

	// 左端.
	for (ulong_t x = 0; x < 1 << (3 - 1); ++x)
	{
		*dstptr++ = ss - (*basptr++ >> scal_y);
	}

	// 本体 (blks_x - 1) * bsiz_x ピクセル.
	while (srcptr < srcend)
	{
		ulong_t s1 = *srcptr++ << (!1 ? scal_y : 0);
		ulong_t ds = s1 - s0;
		s0 = s1;
		for (ulong_t x = 0; x < 1 << 3; ++x)
		{
			*dstptr++ = ss - (*basptr++ >> scal_y), ss += ds;
		}
	}

	// 右端.
	while (dstptr < dstend)
	{
		*dstptr++ = ss - (*basptr++ >> scal_y);
	}

	return srcptr;
}
/*
// △
const uword_t* __stdcall expand_x_4(
	const uword_t* srcptr,
	const uword_t* srcend,
	ulong_t*	   dstptr,
	ulong_t*	   dstend,
	ulong_t		   bsiz_y)
{
	// src には blks_x = size_x / bsiz_x 個分のデータが含まれる.
	// size_x % bsiz_x != 0 のとき, 右側の半端なピクセルには, すべて最
	// 後のデータを埋めることにした.

	std::size_t scal_y = iscalb(bsiz_y);
	ulong_t s0 = *srcptr++ << (!0 ? scal_y : 0), ss = s0 << 4;

	// 左端.
	for (ulong_t x = 0; x < 1 << (4 - 1); ++x)
	{
		*dstptr++ = ss;
	}

	// 本体 (blks_x - 1) * bsiz_x ピクセル.
	while (srcptr < srcend)
	{
		ulong_t s1 = *srcptr++ << (!0 ? scal_y : 0);
		ulong_t ds = s1 - s0;
		s0 = s1;
		for (ulong_t x = 0; x < 1 << 4; ++x)
		{
			*dstptr++ = ss, ss += ds;
		}
	}

	// 右端.
	while (dstptr < dstend)
	{
		*dstptr++ = ss;
	}

	return srcptr;
}

// △
const uword_t* __stdcall expand_xdiff_4(
	const uword_t* srcptr,
	const uword_t* srcend,
	ulong_t*	   dstptr,
	ulong_t*	   dstend,
	const ulong_t* basptr,
	ulong_t		   bsiz_y)
{
	// src には blks_x = size_x / bsiz_x 個分のデータが含まれる.
	// size_x % bsiz_x != 0 のとき, 右側の半端なピクセルには, すべて最
	// 後のデータを埋めることにした.

	std::size_t scal_y = iscalb(bsiz_y);
	ulong_t s0 = *srcptr++ << (!1 ? scal_y : 0), ss = s0 << 4;

	// 左端.
	for (ulong_t x = 0; x < 1 << (4 - 1); ++x)
	{
		*dstptr++ = ss - (*basptr++ >> scal_y);
	}

	// 本体 (blks_x - 1) * bsiz_x ピクセル.
	while (srcptr < srcend)
	{
		ulong_t s1 = *srcptr++ << (!1 ? scal_y : 0);
		ulong_t ds = s1 - s0;
		s0 = s1;
		for (ulong_t x = 0; x < 1 << 4; ++x)
		{
			*dstptr++ = ss - (*basptr++ >> scal_y), ss += ds;
		}
	}

	// 右端.
	while (dstptr < dstend)
	{
		*dstptr++ = ss - (*basptr++ >> scal_y);
	}

	return srcptr;
}
*/

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
	ulong_t		   blks_x)
{
	ulong_t* tm1ptr = tm0ptr + size_x;
	std::size_t scale_y = iscalb(bsiz_y);

	// bsiz_x, bsiz_y <= 2^5 のとき, *srcptr の最大値は 2^8 * 2^5 *
	// 2^5 = 2^18 未満であるから, 18 ビットに収まっている.
	//
	// x, y 方向への線形補間の際, それぞれ最大 bsiz_x, bsiz_y 倍するた
	// め, 途中の結果は 2^18 * 2^5 * 2^5 = 2^28, つまり 28 ビットまで使
	// 用する可能性がある. 32-bit 符号付きデータとしては, あと 3 ビッ
	// トしか余裕がないので, 注意すること.

	// 上端.
	srcptr = expand_x_0(
		srcptr, srcptr + blks_x,
		tm0ptr, tm0ptr + size_x, bsiz_y);
	for (ulong_t y = 0; y < bsiz_y / 2; ++y)
	{
		for (ulong_t x = 0; x < size_x; ++x)
		{
			*dstptr++ = ubyte_t(tm0ptr[x] >> 2 * (0 + scale_y));
		}
	}

	// 本体.
	while (srcptr < srcend)
	{
		srcptr = expand_xdiff_0(
			srcptr, srcptr + blks_x,
			tm1ptr, tm1ptr + size_x, tm0ptr, bsiz_y);
		dstptr = expand_xy(dstptr, tm0ptr, tm1ptr, size_x, bsiz_y, 2 * (0 + scale_y));
	}

	// 下端.
	while (dstptr < dstend)
	{
		for (ulong_t x = 0; x < size_x; ++x)
		{
			*dstptr++ = ubyte_t(tm0ptr[x] >> 2 * (0 + scale_y));
		}
	}
}

void __stdcall expand_1(
	const uword_t* srcptr,
	const uword_t* srcend,
	ubyte_t*	   dstptr,
	ubyte_t*	   dstend,
	ulong_t*	   tm0ptr,
	ulong_t		   size_x,
	ulong_t		   bsiz_y,
	ulong_t		   blks_x)
{
	ulong_t* tm1ptr = tm0ptr + size_x;
	std::size_t scale_y = iscalb(bsiz_y);

	// bsiz_x, bsiz_y <= 2^5 のとき, *srcptr の最大値は 2^8 * 2^5 *
	// 2^5 = 2^18 未満であるから, 18 ビットに収まっている.
	//
	// x, y 方向への線形補間の際, それぞれ最大 bsiz_x, bsiz_y 倍するた
	// め, 途中の結果は 2^18 * 2^5 * 2^5 = 2^28, つまり 28 ビットまで使
	// 用する可能性がある. 32-bit 符号付きデータとしては, あと 3 ビッ
	// トしか余裕がないので, 注意すること.

	// 上端.
	srcptr = expand_x_1(
		srcptr, srcptr + blks_x,
		tm0ptr, tm0ptr + size_x, bsiz_y);
	for (ulong_t y = 0; y < bsiz_y / 2; ++y)
	{
		for (ulong_t x = 0; x < size_x; ++x)
		{
			*dstptr++ = ubyte_t(tm0ptr[x] >> 2 * (1 + scale_y));
		}
	}

	// 本体.
	while (srcptr < srcend)
	{
		srcptr = expand_xdiff_1(
			srcptr, srcptr + blks_x,
			tm1ptr, tm1ptr + size_x, tm0ptr, bsiz_y);
		dstptr = expand_xy(dstptr, tm0ptr, tm1ptr, size_x, bsiz_y, 2 * (1 + scale_y));
	}

	// 下端.
	while (dstptr < dstend)
	{
		for (ulong_t x = 0; x < size_x; ++x)
		{
			*dstptr++ = ubyte_t(tm0ptr[x] >> 2 * (1 + scale_y));
		}
	}
}

void __stdcall expand_2(
	const uword_t* srcptr,
	const uword_t* srcend,
	ubyte_t*	   dstptr,
	ubyte_t*	   dstend,
	ulong_t*	   tm0ptr,
	ulong_t		   size_x,
	ulong_t		   bsiz_y,
	ulong_t		   blks_x)
{
	ulong_t* tm1ptr = tm0ptr + size_x;
	std::size_t scale_y = iscalb(bsiz_y);

	// bsiz_x, bsiz_y <= 2^5 のとき, *srcptr の最大値は 2^8 * 2^5 *
	// 2^5 = 2^18 未満であるから, 18 ビットに収まっている.
	//
	// x, y 方向への線形補間の際, それぞれ最大 bsiz_x, bsiz_y 倍するた
	// め, 途中の結果は 2^18 * 2^5 * 2^5 = 2^28, つまり 28 ビットまで使
	// 用する可能性がある. 32-bit 符号付きデータとしては, あと 3 ビッ
	// トしか余裕がないので, 注意すること.

	// 上端.
	srcptr = expand_x_2(
		srcptr, srcptr + blks_x,
		tm0ptr, tm0ptr + size_x, bsiz_y);
	for (ulong_t y = 0; y < bsiz_y / 2; ++y)
	{
		for (ulong_t x = 0; x < size_x; ++x)
		{
			*dstptr++ = ubyte_t(tm0ptr[x] >> 2 * (2 + scale_y));
		}
	}

	// 本体.
	while (srcptr < srcend)
	{
		srcptr = expand_xdiff_2(
			srcptr, srcptr + blks_x,
			tm1ptr, tm1ptr + size_x, tm0ptr, bsiz_y);
		dstptr = expand_xy(dstptr, tm0ptr, tm1ptr, size_x, bsiz_y, 2 * (2 + scale_y));
	}

	// 下端.
	while (dstptr < dstend)
	{
		for (ulong_t x = 0; x < size_x; ++x)
		{
			*dstptr++ = ubyte_t(tm0ptr[x] >> 2 * (2 + scale_y));
		}
	}
}

void __stdcall expand_3(
	const uword_t* srcptr,
	const uword_t* srcend,
	ubyte_t*	   dstptr,
	ubyte_t*	   dstend,
	ulong_t*	   tm0ptr,
	ulong_t		   size_x,
	ulong_t		   bsiz_y,
	ulong_t		   blks_x)
{
	ulong_t* tm1ptr = tm0ptr + size_x;
	std::size_t scale_y = iscalb(bsiz_y);

	// bsiz_x, bsiz_y <= 2^5 のとき, *srcptr の最大値は 2^8 * 2^5 *
	// 2^5 = 2^18 未満であるから, 18 ビットに収まっている.
	//
	// x, y 方向への線形補間の際, それぞれ最大 bsiz_x, bsiz_y 倍するた
	// め, 途中の結果は 2^18 * 2^5 * 2^5 = 2^28, つまり 28 ビットまで使
	// 用する可能性がある. 32-bit 符号付きデータとしては, あと 3 ビッ
	// トしか余裕がないので, 注意すること.

	// 上端.
	srcptr = expand_x_3(
		srcptr, srcptr + blks_x,
		tm0ptr, tm0ptr + size_x, bsiz_y);
	for (ulong_t y = 0; y < bsiz_y / 2; ++y)
	{
		for (ulong_t x = 0; x < size_x; ++x)
		{
			*dstptr++ = ubyte_t(tm0ptr[x] >> 2 * (3 + scale_y));
		}
	}

	// 本体.
	while (srcptr < srcend)
	{
		srcptr = expand_xdiff_3(
			srcptr, srcptr + blks_x,
			tm1ptr, tm1ptr + size_x, tm0ptr, bsiz_y);
		dstptr = expand_xy(dstptr, tm0ptr, tm1ptr, size_x, bsiz_y, 2 * (3 + scale_y));
	}

	// 下端.
	while (dstptr < dstend)
	{
		for (ulong_t x = 0; x < size_x; ++x)
		{
			*dstptr++ = ubyte_t(tm0ptr[x] >> 2 * (3 + scale_y));
		}
	}
}

void __stdcall expand_4(
	const ulong_t* srcptr,
	const ulong_t* srcend,
	ubyte_t*	   dstptr,
	ubyte_t*	   dstend,
	ulong_t*	   tm0ptr,
	ulong_t		   size_x,
	ulong_t		   bsiz_y,
	ulong_t		   blks_x)
{
	ulong_t* tm1ptr = tm0ptr + size_x;
	std::size_t scale_y = iscalb(bsiz_y);

	// bsiz_x, bsiz_y <= 2^5 のとき, *srcptr の最大値は 2^8 * 2^5 *
	// 2^5 = 2^18 未満であるから, 18 ビットに収まっている.
	//
	// x, y 方向への線形補間の際, それぞれ最大 bsiz_x, bsiz_y 倍するた
	// め, 途中の結果は 2^18 * 2^5 * 2^5 = 2^28, つまり 28 ビットまで使
	// 用する可能性がある. 32-bit 符号付きデータとしては, あと 3 ビッ
	// トしか余裕がないので, 注意すること.

	// 上端.
	srcptr = expand_x_4(
		srcptr, srcptr + blks_x,
		tm0ptr, tm0ptr + size_x, bsiz_y);
	for (ulong_t y = 0; y < bsiz_y / 2; ++y)
	{
		for (ulong_t x = 0; x < size_x; ++x)
		{
			*dstptr++ = ubyte_t(tm0ptr[x] >> 2 * (4 + scale_y));
		}
	}

	// 本体.
	while (srcptr < srcend)
	{
		srcptr = expand_xdiff_4(
			srcptr, srcptr + blks_x,
			tm1ptr, tm1ptr + size_x, tm0ptr, bsiz_y);
		dstptr = expand_xy(dstptr, tm0ptr, tm1ptr, size_x, bsiz_y, 2 * (4 + scale_y));
	}

	// 下端.
	while (dstptr < dstend)
	{
		for (ulong_t x = 0; x < size_x; ++x)
		{
			*dstptr++ = ubyte_t(tm0ptr[x] >> 2 * (4 + scale_y));
		}
	}
}

void __stdcall expand_5(
	const ulong_t* srcptr,
	const ulong_t* srcend,
	ubyte_t*	   dstptr,
	ubyte_t*	   dstend,
	ulong_t*	   tm0ptr,
	ulong_t		   size_x,
	ulong_t		   bsiz_y,
	ulong_t		   blks_x)
{
	ulong_t* tm1ptr = tm0ptr + size_x;
	std::size_t scale_y = iscalb(bsiz_y);

	// bsiz_x, bsiz_y <= 2^5 のとき, *srcptr の最大値は 2^8 * 2^5 *
	// 2^5 = 2^18 未満であるから, 18 ビットに収まっている.
	//
	// x, y 方向への線形補間の際, それぞれ最大 bsiz_x, bsiz_y 倍するた
	// め, 途中の結果は 2^18 * 2^5 * 2^5 = 2^28, つまり 28 ビットまで使
	// 用する可能性がある. 32-bit 符号付きデータとしては, あと 3 ビッ
	// トしか余裕がないので, 注意すること.

	// 上端.
	srcptr = expand_x_5(
		srcptr, srcptr + blks_x,
		tm0ptr, tm0ptr + size_x, bsiz_y);
	for (ulong_t y = 0; y < bsiz_y / 2; ++y)
	{
		for (ulong_t x = 0; x < size_x; ++x)
		{
			*dstptr++ = ubyte_t(tm0ptr[x] >> 2 * (5 + scale_y));
		}
	}

	// 本体.
	while (srcptr < srcend)
	{
		srcptr = expand_xdiff_5(
			srcptr, srcptr + blks_x,
			tm1ptr, tm1ptr + size_x, tm0ptr, bsiz_y);
		dstptr = expand_xy(dstptr, tm0ptr, tm1ptr, size_x, bsiz_y, 2 * (5 + scale_y));
	}

	// 下端.
	while (dstptr < dstend)
	{
		for (ulong_t x = 0; x < size_x; ++x)
		{
			*dstptr++ = ubyte_t(tm0ptr[x] >> 2 * (5 + scale_y));
		}
	}
}

// #undef MACRO_EXPAND

#pragma warning(default: 4799)

void __stdcall LPF(
	const ubyte_t* srcptr,
	ubyte_t*	   dstptr,
	std::size_t	   size_x,
	std::size_t	   size_y,
	std::size_t	   bsiz_x,
	std::size_t	   bsiz_y)
{
	ulong_t scal_x = iscalb(bsiz_x);
	ulong_t scal_y = iscalb(bsiz_y);
	ulong_t blks_x = size_x >> scal_x;		// x方向 ブロック数
	ulong_t blks_y = size_y >> scal_y;		// y方向 ブロック数
	ulong_t blks_t = blks_x * blks_y;		// トータル ブロック数
	if (scal_x <= 3)
	  {
		std::auto_ptr<uword_t> redbuf(new uword_t[blks_t]);
		std::auto_ptr<ulong_t> tmpbuf(new ulong_t[2 * size_x]);
		switch (scal_x ) {
		case 0:
			reduce_0(srcptr, redbuf.get(), redbuf.get() + blks_t, size_x, bsiz_y * size_x, blks_x);
			expand_0(redbuf.get(), redbuf.get() + blks_t, dstptr, dstptr + size_x * size_y, tmpbuf.get(), size_x, bsiz_y, blks_x);
			break;
		case 1:
			reduce_1(srcptr, redbuf.get(), redbuf.get() + blks_t, size_x, bsiz_y * size_x, blks_x);
			expand_1(redbuf.get(), redbuf.get() + blks_t, dstptr, dstptr + size_x * size_y, tmpbuf.get(), size_x, bsiz_y, blks_x);
			break;
		case 2:
			reduce_2(srcptr, redbuf.get(), redbuf.get() + blks_t, size_x, bsiz_y * size_x, blks_x);
			expand_2(redbuf.get(), redbuf.get() + blks_t, dstptr, dstptr + size_x * size_y, tmpbuf.get(), size_x, bsiz_y, blks_x);
			break;
		case 3:
			reduce_3(srcptr, redbuf.get(), redbuf.get() + blks_t, size_x, bsiz_y * size_x, blks_x);
			expand_3(redbuf.get(), redbuf.get() + blks_t, dstptr, dstptr + size_x * size_y, tmpbuf.get(), size_x, bsiz_y, blks_x);
			break;
		}
		lpf_emms();
	  }
	else
	  {
		ulong_t* tmpbuf = new ulong_t[blks_t + 2 * size_x];
		switch (scal_x) {
		case 4:
			reduce_4(srcptr, tmpbuf, tmpbuf + blks_t, size_x, bsiz_y * size_x, blks_x);
			expand_4(tmpbuf, tmpbuf + blks_t, dstptr, dstptr + size_x * size_y, tmpbuf + blks_t, size_x, bsiz_y, blks_x);
			break;
		case 5:
			reduce_5(srcptr, tmpbuf, tmpbuf + blks_t, size_x, bsiz_y * size_x, blks_x);
			expand_5(tmpbuf, tmpbuf + blks_t, dstptr, dstptr + size_x * size_y, tmpbuf + blks_t, size_x, bsiz_y, blks_x);
			break;
		}
		lpf_emms();
		delete [] tmpbuf;
	  }
}
