#pragma once
//VisualC++ にstdint.h式の型名を与えるためのヘッダ。

#if _MSC_VER >= 1600

//VC2010以降は、標準ヘッダにある
#include <stdint.h>

#else
typedef   signed char  int8_t ;
typedef unsigned char uint8_t ;

typedef   signed short  int16_t ;
typedef unsigned short uint16_t ;

typedef   signed int  int32_t ;
typedef unsigned int uint32_t ;

typedef   signed __int64  int64_t ;
typedef unsigned __int64 uint64_t ;

#endif
