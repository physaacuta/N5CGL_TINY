#ifndef	_SCH_DATA_SET_H
#define	_SCH_DATA_SET_H

BOOL ShortDataSetSch(HWND hWnd, unsigned char* buff, int setAddr, int itemAddr);
BOOL LongDataSetSch(HWND hWnd, unsigned char* buff, int setAddr, int itemAddr);
BOOL CharDataSetSch(HWND hWnd, unsigned char* buff, int setAddr, int itemAddr);
BOOL EbcdicDataSetSch(HWND hWnd, unsigned char* buff, int setAddr, int itemAddr);
BOOL HexDataSetSch(HWND hWnd, int ptn, unsigned char* buff, int setAddr, int itemAddr);
BOOL HexDataSetSch2(HWND hWnd, unsigned char* buff);

BOOL XShortDataSetSch(HWND hWnd, unsigned char* buff, int setAddr, int itemAddr);
BOOL XLongDataSetSch(HWND hWnd, unsigned char* buff, int setAddr, int itemAddr);
BOOL XHexDataSetSch(HWND hWnd, int ptn, unsigned char* buff, int setAddr, int itemAddr);

#endif
