#ifndef	_MAKE_DATA_SET_H
#define	_MAKE_DATA_SET_H

BOOL    ShortDataSet(HWND hWnd, unsigned char* buff);
BOOL    LongDataSet(HWND hWnd, unsigned char* buff);
BOOL    FloatDataSet(HWND hWnd, unsigned char* buff);
BOOL    DoubleDataSet(HWND hWnd, unsigned char* buff);
BOOL    HexDataSet(HWND hWnd, int num, unsigned char* buff);
BOOL    XHexDataSet(HWND hWnd, int num, unsigned char* buff);
BOOL    CharDataSet(HWND hWnd, unsigned char* buff);
BOOL    BinDataSet(HWND hWnd, unsigned char* buff);
BOOL    SpaceDataSet(HWND hWnd, unsigned char* buff);
BOOL    EbcSpaceDataSet(HWND hWnd, unsigned char* buff);
BOOL    XShortDataSet(HWND hWnd, unsigned char* buff);
BOOL    XLongDataSet(HWND hWnd, unsigned char* buff);
BOOL    EbcdicDataSet(HWND hWnd, unsigned char* buff);
BOOL    DateTimeSet(HWND hWnd, unsigned char* buff, int kind);
BOOL    CDateTimeSet(HWND hWnd, unsigned char* buff);
BOOL    EDateTimeSet(HWND hWnd, unsigned char* buff);
BOOL    XDateTimeSet(HWND hWnd, unsigned char* buff, int kind);
BOOL    CountShortDataSet(HWND hWnd, unsigned char* buff, unsigned char* fileName);
BOOL    CountLongDataSet(HWND hWnd, unsigned char* buff, unsigned char* fileName);
BOOL    CountXShortDataSet(HWND hWnd, unsigned char* buff, unsigned char* fileName);
BOOL    CountXLongDataSet(HWND hWnd, unsigned char* buff, unsigned char* fileName);
BOOL    CountCharDataSet(HWND hWnd, unsigned char* buff, unsigned char* fileName);
BOOL    CountEbcDataSet(HWND hWnd, unsigned char* buff, unsigned char* fileName);
BOOL    FileIntToCharSet(HWND hWnd, unsigned char* buff);
BOOL    FileCharToCharSet(HWND hWnd, unsigned char* buff);
BOOL    FileIntToEbcSet(HWND hWnd, unsigned char* buff);
BOOL    FileCharToEbcSet(HWND hWnd, unsigned char* buff);
BOOL    FileIntToIntSet(HWND hWnd, unsigned char* buff);
BOOL    FileIntToShortSet(HWND hWnd, unsigned char* buff);
BOOL    FileIntToXShortSet(HWND hWnd, unsigned char* buff);
BOOL    FileIntToBinSet(HWND hWnd, unsigned char* buff);

static int get_i_nup(int start, int end, char nup[256]);
static void calc_idata(int start, int end, int i_nup, int* idata);

#endif
