#ifndef	_MAKE_CNT_SET_H
#define	_MAKE_CNT_SET_H

BOOL  MakeCountDataSet(HWND hWnd, unsigned char* buff, int setAddr, int itemAddr);
BOOL CntCharDataSet(HWND hWnd, unsigned char* buff, int setAddr, int itemAddr);
int CntDefSub(int tblno, int addr, int chkAddr);
int CntFilePut(HWND hWnd, int tblno, int addr);
void makeSemaName(HWND hWnd, char* semaname);
int GetItemInt(unsigned char* buff, unsigned char* item, int* num);
int GetItemChar(unsigned char* buff, unsigned char* item, char* str);
char* GetTypeText(int no);

#endif
