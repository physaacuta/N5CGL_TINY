#include <stdafx.h>
#include "..\Include\LibSelect.h"										// ライブラリ切替ヘッダー


#ifdef HAS_IPP7 
# ifdef _M_X64
#  pragma comment(lib, "../../Include/Other/Ipp/dll/ijl20x64.lib")
# else 
#  pragma comment(lib, "../../Include/Other/Ipp/dll/ijl20x86.lib")
# endif
#endif


#ifdef HAS_IPP6
# ifdef _M_X64
#  pragma comment(lib, "../../Include/Other/Ipp/lib61/x64/ijl20l.lib")
# else 
#  pragma comment(lib, "../../Include/Other/Ipp/lib61/x86/ijl20l.lib")
# endif
#endif

#ifdef HAS_IPP5
#  pragma comment(lib, "../../Include/Other/Ipp/lib50/ijl20l.lib")
#endif

#if defined( HAS_IPP6) && defined(_M_X64)
#pragma comment(lib, "ippimergedem64t.lib")
#pragma comment(lib, "ippsmergedem64t.lib")
#pragma comment(lib, "ippjmergedem64t.lib")
#pragma comment(lib, "ippcvmergedem64t.lib")
#pragma comment(lib, "ippsemergedem64t.lib")
#pragma comment(lib, "ippiemergedem64t.lib")
#pragma comment(lib, "ippjemergedem64t.lib")
#pragma comment(lib, "ippcvemergedem64t.lib")
#pragma comment(lib, "ippcoreem64tl.lib")
#elif defined( HAS_IPP6 ) || defined( HAS_IPP5  )
#pragma comment(lib, "ippimerged.lib")
#pragma comment(lib, "ippsmerged.lib")
#pragma comment(lib, "ippjmerged.lib")
#pragma comment(lib, "ippcvmerged.lib")
#pragma comment(lib, "ippsemerged.lib")
#pragma comment(lib, "ippiemerged.lib")
#pragma comment(lib, "ippjemerged.lib")
#pragma comment(lib, "ippcvemerged.lib")
#pragma comment(lib, "ippcorel.lib")
#endif
