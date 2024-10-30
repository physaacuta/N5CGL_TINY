#ifndef  __PCTERM_H
#define  __PCTERM_H

#define STNET_MIN       12
#define STNET_MAX       (12+4000+2)
#define FCH_LENP        2
#define PAD_CHAR        0
#define GETLEN(a)       ((unsigned short)(a<<8)|(a>>8))
#define GETDATA(a)      ((unsigned short)(a))
#define SETSUM(a)       ((unsigned short)(a))

#define RC_OK           0
#define RC_SUMERR       1
#define RC_LENERR       2

int     makePcterm(HWND hWnd, unsigned char* g_pSendBuf, unsigned char* g_pSendDmy, int g_nTotalToSend, unsigned char g_pcterm_pa);
int     checksum( HWND hWnd, int type, char *fch );
#endif
