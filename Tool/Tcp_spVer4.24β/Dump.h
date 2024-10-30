#ifndef  __DUMP_H
#define  __DUMP_H

#include	<windows.h>

#define		_DISP_OFF		0
#define		_DISP_ON		1

#define		_ASCII			0
#define		_S_JIS			1
#define		_EBCDIC			2

#define		uiListMaxCt		2000					// ÿΩƒï\é¶MAXåèêî

typedef		unsigned char	BYTE;

void	cDump (BYTE *Data, int DispOut, BYTE *DevName, int Div, BYTE eCode, BYTE *Msg);
void	wkDump(HWND dp_hWnd, int iListID, BYTE *Data, int DispOut, BYTE *DevName, int Div, int Len, BYTE *Msg);

//void	msgLog( BYTE *DevName, const char *__format, ... );
void	msgLog( HWND hWnd, int iListID, BYTE *DevName, const char *__format, ... );

void	sgtim(int* date);

#endif

