#include    <conio.h>
#include    <stdio.h>
#include    <windows.h>
#include    <winsock.h>
#include	"AP_Func.h"
#include    "TcpCtrl.h"
#include	"MakeDataSet.h"
#include    "resource.h"
/*-------------------------------------------------------------------
*
*   ＳＨＯＲＴデータ作成、バッファ格納
*
*--------------------------------------------------------------------*/
BOOL ShortDataSet(HWND hWnd, unsigned char* buff)
{
    char    type[10];
	char	s_ini[8];
    short   sData;
    int     addr;

    if('0' <= buff[0] &&  buff[0] <= '9') {
    	sscanf(buff, "%d %s %s", &addr, type, s_ini);
        g_setAddr = addr;
    } else {
    	sscanf(buff, "%s %s", type, s_ini);
        addr = g_setAddr;
    }
 	if(0 == memcmp(s_ini, "INIT", 4)) {
    	memcpy(&g_pSendDmy[addr], &g_sInitBuf[0], sizeof(short));
	}else{
        if('0' <= buff[0] && buff[0] <= '9') {
        	sscanf(buff, "%d %s %d", &addr, type, &sData);
            g_setAddr = addr;
        } else {
    	    sscanf(buff, "%s %d", type, &sData);
            addr = g_setAddr;
        }
//  MyMessage( hWnd, "%d %s %d ", addr, type, sData );
    	memcpy(&g_pSendDmy[addr], &sData, sizeof(short));
	}
    g_setAddr = g_setAddr + sizeof(short);

    return TRUE;
}


/*-------------------------------------------------------------------
*
*   ＬＯＮＧデータ作成、バッファ格納
*
*--------------------------------------------------------------------*/
BOOL LongDataSet(HWND hWnd, unsigned char* buff)
{
    char    type[10];
	char	l_ini[8];
    long    sData;
    int     addr;

    if('0' <= buff[0] &&  buff[0] <= '9') {
    	sscanf(buff, "%d %s %s", &addr, type, l_ini);
        g_setAddr = addr;
    } else {
    	sscanf(buff, "%s %s", type, l_ini);
        addr = g_setAddr;
    }

	if(0 == memcmp(l_ini, "INIT", 4)) {
		memcpy(&g_pSendDmy[addr], &g_lInitBuf[0], sizeof(long));
	}else{
        if('0' <= buff[0] && buff[0] <= '9') {
        	sscanf(buff, "%d %s %d", &addr, type, &sData);
            g_setAddr = addr;
        } else {
        	sscanf(buff, "%s %d", type, &sData);
            addr = g_setAddr;
        }
    	memcpy(&g_pSendDmy[addr], &sData, sizeof(long));
	}
    g_setAddr = g_setAddr + sizeof(long);

    return TRUE;
}

/*-------------------------------------------------------------------
*
*   ＦＬＯＡＴデータ作成、バッファ格納
*
*--------------------------------------------------------------------*/
BOOL FloatDataSet(HWND hWnd, unsigned char* buff)
{
    char    type[10];
    float   sData;
    int     addr;
	char	sWork[128];

   	memset(sWork, 0x00, sizeof(sWork));

    if('0' <= buff[0] &&  buff[0] <= '9') {
        sscanf(buff, "%d %s %s", &addr, type, &sWork[0]);
        g_setAddr = addr;
    } else {
        sscanf(buff, "%s %s", type, &sWork[0]);
        addr = g_setAddr;
    }
	
	sData = (float)atof(sWork);

    memcpy(&g_pSendDmy[addr], &sData, sizeof(float));

    g_setAddr = g_setAddr + sizeof(float);

    return TRUE;
}

/*-------------------------------------------------------------------
*
*   ＤＯＵＢＬＥデータ作成、バッファ格納
*
*--------------------------------------------------------------------*/
BOOL DoubleDataSet(HWND hWnd, unsigned char* buff)
{
    char    type[10];
    double  sData;
    int     addr;
	char	sWork[128];

	memset(sWork, 0x00, sizeof(sWork));

    if('0' <= buff[0] &&  buff[0] <= '9') {
        sscanf(buff, "%d %s %s", &addr, type, &sWork[0]);
        g_setAddr = addr;
	} else {
        sscanf(buff, "%s %s", type, &sWork[0]);
        addr = g_setAddr;
    }
	sData = (double)atof(sWork);

    memcpy(&g_pSendDmy[addr], &sData, sizeof(double));

    g_setAddr = g_setAddr + sizeof(double);

    return TRUE;
}


/*-------------------------------------------------------------------
*
*   ＣＨＡＲデータ作成、バッファ格納
*
*--------------------------------------------------------------------*/
BOOL CharDataSet(HWND hWnd, unsigned char* buff)
{
    char    type[10];

    char    cData[1024];
    int     addr;
    int     i;
    int     set;
    int     sw;

    if('0' <= buff[0] &&  buff[0] <= '9') {
        sscanf(buff, "%d %s", &addr, type);
        g_setAddr = addr;
    } else {
        addr = g_setAddr;
    }
    memset(cData, 0x00, sizeof(cData));
    set = 0;
    sw  = 1;
    for(i=0; i<256 || 3==sw; i++) {
        switch(sw) {
        case 1:
            if('\"' == *(buff+i)) {
                sw = 2;
            }
            break;
        case 2:
            if('\"' == *(buff+i)) {
                sw = 3;
            } else {
                cData[set] = *(buff+i);
                set++;
            }
            break;
        }
        if(3 == sw) {
            break;
        }
    }

    if(3 != sw) {
		MyMessage( hWnd, "CHAR 定義異常 [%s] 定義情報エラー！！", buff);
        return FALSE;
    }

    memcpy(&g_pSendDmy[addr], cData, set);

    g_setAddr = g_setAddr + set;

    return TRUE;
}


/*-------------------------------------------------------------------
*
*   ＨＥＸデータ作成、バッファ格納
*
*--------------------------------------------------------------------*/
BOOL HexDataSet(HWND hWnd, int num, unsigned char* buff)
{
    char    type[10];

    char    cHex[4];
    short   sHex;
    int     iHex;
    int     addr;
    int     cnt;
    int     i;
	int     rc;
	char    cdmy[32];
    char    cnt_buf[512];

    memset(cnt_buf, 0x00, sizeof(cnt_buf));
    if(1 == num) {
        if('0' <= buff[0] &&  buff[0] <= '9') {
            rc=sscanf(buff, "%d %s %s %s", &addr, type, cdmy, cnt_buf);
            g_setAddr = addr;
        } else {
            rc=sscanf(buff, "%s %s %s", type, cdmy, cnt_buf);
            addr = g_setAddr;
        }
        if('0' <= cnt_buf[0] && cnt_buf[0] <= '9') {
            cnt = atoi(cnt_buf);
            if(cnt <= 0) {
                cnt = 1;
            }
        } else {
            cnt = 1;
        }
		if(0 == memcmp(cdmy, "0x", 2)){
			sscanf(&cdmy[2], "%2x", &cHex[0]);
		} else {
			sscanf(&cdmy[0], "%2x", &cHex[0]);
		}

        for(i=0; i<cnt; i++) {
            memcpy(&g_pSendDmy[addr+(i*sizeof(char))], &cHex[0], sizeof(char));
        }
        g_setAddr = g_setAddr + (sizeof(char) * cnt);

    } else if(2 == num) {
        if('0' <= buff[0] &&  buff[0] <= '9') {
            rc=sscanf(buff, "%d %s %s %s", &addr, type, cdmy, cnt_buf);
            g_setAddr = addr;
        } else {
            rc=sscanf(buff, "%s %s %s", type, cdmy, cnt_buf);
            addr = g_setAddr;
        }
        if('0' <= cnt_buf[0] && cnt_buf[0] <= '9') {
            cnt = atoi(cnt_buf);
            if(cnt <= 0) {
                cnt = 1;
            }
        } else {
            cnt = 1;
        }
		if(0 == memcmp(cdmy, "0x", 2)){
			sscanf(&cdmy[2], "%4x", &sHex);
		} else {
			sscanf(&cdmy[0], "%4x", &sHex);
		}

        for(i=0; i<cnt; i++) {
            memcpy(&g_pSendDmy[addr+(i*sizeof(short))], &sHex, sizeof(short));
        }
        g_setAddr = g_setAddr + (sizeof(short) * cnt);

    } else if(4 == num) {
        if('0' <= buff[0] &&  buff[0] <= '9') {
            rc=sscanf(buff, "%d %s %s %s", &addr, type, cdmy, cnt_buf);
            g_setAddr = addr;
        } else {
            rc=sscanf(buff, "%s %s %s", type, cdmy, cnt_buf);
            addr = g_setAddr;
        }
        if('0' <= cnt_buf[0] && cnt_buf[0] <= '9') {
            cnt = atoi(cnt_buf);
            if(cnt <= 0) {
                cnt = 1;
            }
        } else {
            cnt = 1;
        }
		if(0 == memcmp(cdmy, "0x", 2)){
			sscanf(&cdmy[2], "%8x", &iHex);
		} else {
			sscanf(&cdmy[0], "%8x", &iHex);
		}

        for(i=0; i<cnt; i++) {
            memcpy(&g_pSendDmy[addr+(i*sizeof(int))], &iHex, sizeof(int));
        }
        g_setAddr = g_setAddr + (sizeof(int) * cnt);
    } else {
		MyMessage( hWnd, "HEX 定義異常 [%s] 定義情報エラー！！", buff);
        return FALSE;
    }

    return TRUE;
}


/*-------------------------------------------------------------------
*
*   ＸＨＥＸデータ作成、バッファ格納
*
*--------------------------------------------------------------------*/
BOOL XHexDataSet(HWND hWnd, int num, unsigned char* buff)
{
    char    type[10];

	char	cWork[4];
	char	cData[4];
	char    cdmy[32];
    short   sHex;
    int     iHex;
    int     addr;
    int     cnt;
    int     i;
    int     rc;
    char    cnt_buf[512];

    memset(cnt_buf, 0x00, sizeof(cnt_buf));
    if(2 == num) {
        if('0' <= buff[0] &&  buff[0] <= '9') {
            rc=sscanf(buff, "%d %s %s %s", &addr, type, cdmy, cnt_buf);
            g_setAddr = addr;
        } else {
            rc=sscanf(buff, "%s %s %s", type, cdmy, cnt_buf);
            addr = g_setAddr;
        }
        if('0' <= cnt_buf[0] && cnt_buf[0] <= '9') {
            cnt = atoi(cnt_buf);
            if(cnt <= 0) {
                cnt = 1;
            }
        } else {
            cnt = 1;
        }
		if(0 == memcmp(cdmy, "0x", 2)){
			sscanf(&cdmy[2], "%4x", &sHex);
		} else {
			sscanf(&cdmy[0], "%4x", &sHex);
		}

    	memcpy(cWork, &sHex, sizeof(short));
    	cData[0] = cWork[1];
    	cData[1] = cWork[0];
        for(i=0; i<cnt; i++) {
            memcpy(&g_pSendDmy[addr+(i*sizeof(short))], &cData, sizeof(short));
        }
        g_setAddr = g_setAddr + (sizeof(short) * cnt);
    } else if(4 == num) {
        if('0' <= buff[0] &&  buff[0] <= '9') {
            rc=sscanf(buff, "%d %s %s %s", &addr, type, cdmy, cnt_buf);
            g_setAddr = addr;
        } else {
            rc=sscanf(buff, "%s %s %s", type, cdmy, cnt_buf);
            addr = g_setAddr;
        }
//		if(rc != 4) {
//			MyMessage( hWnd, "アドレス=%d  TYPE=%s  定義情報エラー！！", addr, type);
//			return FALSE;
//		}

        if('0' <= cnt_buf[0] && cnt_buf[0] <= '9') {
            cnt = atoi(cnt_buf);
            if(cnt <= 0) {
                cnt = 1;
            }
        } else {
            cnt = 1;
        }
		if(0 == memcmp(cdmy, "0x", 2)){
			sscanf(&cdmy[2], "%8x", &iHex);
		} else {
			sscanf(&cdmy[0], "%8x", &iHex);
		}

    	memcpy(cWork, &iHex, sizeof(int));
    	cData[0] = cWork[3];
    	cData[1] = cWork[2];
    	cData[2] = cWork[1];
    	cData[3] = cWork[0];
        for(i=0; i<cnt; i++) {
            memcpy(&g_pSendDmy[addr+(i*sizeof(int))], &cData, sizeof(int));
        }
        g_setAddr = g_setAddr + (sizeof(int) * cnt);
    } else {
		MyMessage( hWnd, "XHEX 定義異常 [%s] 定義情報エラー！！", buff);
        return FALSE;
    }

    return TRUE;
}


/*-------------------------------------------------------------------
*
*   ＢＩＮデータ作成、バッファ格納
*
*--------------------------------------------------------------------*/
BOOL BinDataSet(HWND hWnd, unsigned char* buff)
{
    char    type[10];

    char    cData[128];
    int     addr;
    int     i;
    int     set;
    int     sw;
    char    bData;

    if('0' <= buff[0] &&  buff[0] <= '9') {
        sscanf(buff, "%d %s", &addr, type);
        g_setAddr = addr;
    } else {
        addr = g_setAddr;
    }

    memset(cData, 0x00, sizeof(cData));
    set = 0;
    sw  = 1;
    for(i=0; i<256 || 3==sw; i++) {
        switch(sw) {
        case 1:
            if('\"' == *(buff+i)) {
                sw = 2;
            }
            break;
        case 2:
            if('\"' == *(buff+i)) {
                sw = 3;
            } else {
                cData[set] = *(buff+i);
                set++;
            }
            break;
        }
        if(3 == sw) {
            break;
        }
    }

    if(3 != sw) {
		MyMessage( hWnd, "BIN 定義異常 [%s] 定義情報エラー！！", buff);
        return FALSE;
    }

    bData = 0;
    for(i=0; i<8; i++) {
        bData <<= 1;
        if('1' == cData[i]) {
            bData |=  0x01;
        }
    }

    memcpy(&g_pSendDmy[addr], &bData, sizeof(char));
    g_setAddr = g_setAddr + sizeof(char);
    return TRUE;
}

/*-------------------------------------------------------------------
*
*   スペースデータ作成、バッファ格納
*
*--------------------------------------------------------------------*/
BOOL SpaceDataSet(HWND hWnd, unsigned char* buff)
{
    char    type[10];

    int     count;
    int     addr;

    count = 0;
    if('0' <= buff[0] &&  buff[0] <= '9') {
        sscanf(buff, "%d %s %d", &addr, type, &count);
        g_setAddr = addr;
    } else {
        sscanf(buff, "%s %d", type, &count);
        addr = g_setAddr;
    }
    memset(&g_pSendDmy[addr], 0x20, count);
    g_setAddr = g_setAddr + count;

    return TRUE;
}

/*-------------------------------------------------------------------
*
*   EBCDIC スペースデータ作成、バッファ格納
*
*--------------------------------------------------------------------*/
BOOL EbcSpaceDataSet(HWND hWnd, unsigned char* buff)
{
    char    type[10];

    int     count;
    int     addr;

    count = 0;
    if('0' <= buff[0] &&  buff[0] <= '9') {
        sscanf(buff, "%d %s %d", &addr, type, &count);
        g_setAddr = addr;
    } else {
        sscanf(buff, "%s %d", type, &count);
        addr = g_setAddr;
    }
    memset(&g_pSendDmy[addr], 0x40, count);
    g_setAddr = g_setAddr + count;

    return TRUE;
}

/*-------------------------------------------------------------------
*
*   ＸＳＨＯＲＴデータ作成、バッファ格納（バイト反転をしてセットする）
*
*--------------------------------------------------------------------*/
BOOL XShortDataSet(HWND hWnd, unsigned char* buff)
{
    char    type[10];
    char    cWork[2];
    char    cData[2];
	char	s_ini[8];
    short   sData;
    int     addr;

    if('0' <= buff[0] &&  buff[0] <= '9') {
    	sscanf(buff, "%d %s %s", &addr, type, s_ini);
        g_setAddr = addr;
    } else {
    	sscanf(buff, "%s %s", type, s_ini);
        addr = g_setAddr;
    }
	if(0 == memcmp(s_ini, "INIT", 4)) {
		memcpy(cWork, &g_xsInitBuf[0], sizeof(short));
	}else{
        if('0' <= buff[0] &&  buff[0] <= '9') {
        	sscanf(buff, "%d %s %d", &addr, type, &sData);
            g_setAddr = addr;
        } else {
        	sscanf(buff, "%s %d", type, &sData);
            addr = g_setAddr;
        }
    	memcpy(cWork, &sData, sizeof(short));
	}

    cData[0] = cWork[1];
    cData[1] = cWork[0];

    memcpy(&g_pSendDmy[addr], cData, sizeof(short));
    g_setAddr =  g_setAddr + sizeof(short);

    return TRUE;
}


/*-------------------------------------------------------------------
*
*   ＸＬＯＮＧデータ作成、バッファ格納（バイト反転をしてからセットする）
*
*--------------------------------------------------------------------*/
BOOL XLongDataSet(HWND hWnd, unsigned char* buff)
{
    char    type[10];
    char    cWork[4];
    char    cData[4];
	char	l_ini[8];

    long    sData;
    int     addr;

    if('0' <= buff[0] &&  buff[0] <= '9') {
	    sscanf(buff, "%d %s %s", &addr, type, l_ini);
        g_setAddr = addr;
    } else {
	    sscanf(buff, "%s %s", type, l_ini);
        addr = g_setAddr;
    }
	if(0 == memcmp(l_ini, "INIT", 4)) {
		memcpy(cWork, &g_xlInitBuf[0], sizeof(long));
	}else{
        if('0' <= buff[0] &&  buff[0] <= '9') {
        	sscanf(buff, "%d %s %d", &addr, type, &sData);
            g_setAddr = addr;
        } else {
        	sscanf(buff, "%s %d", type, &sData);
            addr = g_setAddr;
        }
    	memcpy(cWork, &sData, sizeof(long));
	}

    cData[0] = cWork[3];
    cData[1] = cWork[2];
    cData[2] = cWork[1];
    cData[3] = cWork[0];

    memcpy(&g_pSendDmy[addr], &cData, sizeof(long));
    g_setAddr =  g_setAddr + sizeof(long);

    return TRUE;
}


/*-------------------------------------------------------------------
*
*   ＥＢＣＤＩＣデータ作成、バッファ格納（ＥＢＣＤＩＣコード変換後データセット）
*
*--------------------------------------------------------------------*/
BOOL EbcdicDataSet(HWND hWnd, unsigned char* buff)
{
    char    type[10];

    char    cData[1024];
    char    eData[1024];
    int     addr;
    int     i;
    int     set;
    int     sw;

    if('0' <= buff[0] &&  buff[0] <= '9') {
        sscanf(buff, "%d %s", &addr, type);
        g_setAddr = addr;
    } else {
        addr = g_setAddr;
    }

    memset(cData, 0x00, sizeof(cData));
    set = 0;
    sw  = 1;
    for(i=0; i<256 || 3==sw; i++) {
        switch(sw) {
        case 1:
            if('\"' == *(buff+i)) {
                sw = 2;
            }
            break;
        case 2:
            if('\"' == *(buff+i)) {
                sw = 3;
            } else {
                cData[set] = *(buff+i);
                set++;
            }
            break;
        }
        if(3 == sw) {
            break;
        }
    }

    if(3 != sw) {
		MyMessage( hWnd, "EBCDIC 定義異常 [%s] 定義情報エラー！！", buff);
        return FALSE;
    }

    strnatoe(cData, eData, set);

    memcpy(&g_pSendDmy[addr], eData, set);
    g_setAddr =  g_setAddr + set;

    return TRUE;
}

/*-------------------------------------------------------------------
*
*   日付作成、バッファ格納
*
*--------------------------------------------------------------------*/
BOOL DateTimeSet(HWND hWnd, unsigned char* buff, int kind)
{
    char    type[10];
    char    dFlg[10];
    char    tFlg[10];
    int     addr;
    short   wDate_Time[6];
    int     iDate_Time[6];

    if('0' <= buff[0] &&  buff[0] <= '9') {
        sscanf(buff, "%d %s %s %s", &addr, type, dFlg, tFlg);
        g_setAddr = addr;
    } else {
        sscanf(buff, "%s %s %s", type, dFlg, tFlg);
        addr = g_setAddr;
    }
    AP_Sgtim(wDate_Time);											// 時間取込処理

    if(2 == kind) {
        if((0 == strcmp(dFlg, "ON")) && (0 == strcmp(tFlg, "ON"))) {
            memcpy(&g_pSendDmy[addr], wDate_Time, sizeof(short)*6);
            g_setAddr =  g_setAddr + (sizeof(short)*6);
        } else if((0 == strcmp(dFlg, "ON")) && (0 != strcmp(tFlg, "ON"))) {            
            memcpy(&g_pSendDmy[addr], wDate_Time, sizeof(short)*3);
            g_setAddr =  g_setAddr + (sizeof(short)*3);
        } else if((0 != strcmp(dFlg, "ON")) && (0 == strcmp(tFlg, "ON"))) {            
            memcpy(&g_pSendDmy[addr], &wDate_Time[3], sizeof(short)*3);
            g_setAddr =  g_setAddr + (sizeof(short)*3);
        }
    } else if(4 == kind) {
        iDate_Time[0] = wDate_Time[0];
        iDate_Time[1] = wDate_Time[1];
        iDate_Time[2] = wDate_Time[2];
        iDate_Time[3] = wDate_Time[3];
        iDate_Time[4] = wDate_Time[4];
        iDate_Time[5] = wDate_Time[5];
        if((0 == strcmp(dFlg, "ON")) && (0 == strcmp(tFlg, "ON"))) {
            memcpy(&g_pSendDmy[addr], iDate_Time, sizeof(int)*6);
            g_setAddr =  g_setAddr + (sizeof(int)*6);
        } else if((0 == strcmp(dFlg, "ON")) && (0 != strcmp(tFlg, "ON"))) {            
            memcpy(&g_pSendDmy[addr], iDate_Time, sizeof(int)*3);
            g_setAddr =  g_setAddr + (sizeof(int)*3);
        } else if((0 != strcmp(dFlg, "ON")) && (0 == strcmp(tFlg, "ON"))) {            
            memcpy(&g_pSendDmy[addr], &iDate_Time[3], sizeof(int)*3);
            g_setAddr =  g_setAddr + (sizeof(int)*3);
        }
    }
    return TRUE;
}

/*-------------------------------------------------------------------
*
*   日付作成、バッファ格納
*
*--------------------------------------------------------------------*/
BOOL CDateTimeSet(HWND hWnd, unsigned char* buff)
{
    char    type[10];
    char    dFlg[10];
    char    tFlg[10];
    int     addr;
    short   wDate_Time[6];
    char    work[30];

    memset(work, 0x00, sizeof(work));

    if('0' <= buff[0] &&  buff[0] <= '9') {
        sscanf(buff, "%d %s %s %s", &addr, type, dFlg, tFlg);
        g_setAddr = addr;
    } else {
        sscanf(buff, "%s %s %s", type, dFlg, tFlg);
        addr = g_setAddr;
    }
    AP_Sgtim(wDate_Time);							// 時間取込処理
    sprintf(work, "%04d%02d%02d%02d%02d%02d", 
                    wDate_Time[0], wDate_Time[1], wDate_Time[2],
                    wDate_Time[3], wDate_Time[4], wDate_Time[5]);

    if((0 == strcmp(dFlg, "ON")) && (0 == strcmp(tFlg, "ON"))) {
        memcpy(&g_pSendDmy[addr], work, 14);
        g_setAddr =  g_setAddr + 14;
    } else if((0 == strcmp(dFlg, "ON")) && (0 != strcmp(tFlg, "ON"))) {            
        memcpy(&g_pSendDmy[addr], work, 8);
        g_setAddr =  g_setAddr + 8;
    } else if((0 != strcmp(dFlg, "ON")) && (0 == strcmp(tFlg, "ON"))) {            
        memcpy(&g_pSendDmy[addr], &work[8], 6);
        g_setAddr =  g_setAddr + 6;
    }
    return TRUE;
}

/*-------------------------------------------------------------------
*
*   日付作成、バッファ格納（EBCDIC）
*
*--------------------------------------------------------------------*/
BOOL EDateTimeSet(HWND hWnd, unsigned char* buff)
{
    char    type[10];
    char    dFlg[10];
    char    tFlg[10];
    int     addr;
    short   wDate_Time[6];
    char    work[30];
    char    ework[30];

    memset(work, 0x00, sizeof(work));

    if('0' <= buff[0] &&  buff[0] <= '9') {
        sscanf(buff, "%d %s %s %s", &addr, type, dFlg, tFlg);
        g_setAddr = addr;
    } else {
        sscanf(buff, "%s %s %s", type, dFlg, tFlg);
        addr = g_setAddr;
    }
    AP_Sgtim(wDate_Time);							// 時間取込処理
    sprintf(work, "%04d%02d%02d%02d%02d%02d", 
                    wDate_Time[0], wDate_Time[1], wDate_Time[2],
                    wDate_Time[3], wDate_Time[4], wDate_Time[5]);

    if((0 == strcmp(dFlg, "ON")) && (0 == strcmp(tFlg, "ON"))) {
        strnatoe(work, ework, strlen(work));
		memcpy(&g_pSendDmy[addr], ework, 14);
        g_setAddr =  g_setAddr + 14;
    } else if((0 == strcmp(dFlg, "ON")) && (0 != strcmp(tFlg, "ON"))) {            
        strnatoe(work, ework, strlen(work));
        memcpy(&g_pSendDmy[addr], ework, 8);
        g_setAddr =  g_setAddr + 8;
    } else if((0 != strcmp(dFlg, "ON")) && (0 == strcmp(tFlg, "ON"))) {            
        strnatoe(work, ework, strlen(work));
        memcpy(&g_pSendDmy[addr], &ework[8], 6);
        g_setAddr =  g_setAddr + 6;
    }
    return TRUE;
}

/*-------------------------------------------------------------------
*
*   日付作成、バッファ格納 バイト反転有り
*
*--------------------------------------------------------------------*/
BOOL XDateTimeSet(HWND hWnd, unsigned char* buff, int kind)
{
    char    type[10];
    char    dFlg[10];
    char    tFlg[10];
    int     addr;
    short   wDate_Time[6];
    int     iDate_Time[6];
    char    wDate[12];

    char    cWork[24];

    memset(cWork, 0x00, sizeof(cWork));

    if('0' <= buff[0] &&  buff[0] <= '9') {
        sscanf(buff, "%d %s %s %s", &addr, type, dFlg, tFlg);
        g_setAddr = addr;
    } else {
        sscanf(buff, "%s %s %s", type, dFlg, tFlg);
        addr = g_setAddr;
    }
    AP_Sgtim(wDate_Time);				// 時間取込処理

    if(2 == kind) {
        memcpy(wDate, wDate_Time, sizeof(wDate_Time));
        cWork[0]  = wDate[1];
        cWork[1]  = wDate[0];
        cWork[2]  = wDate[3];
        cWork[3]  = wDate[2];
        cWork[4]  = wDate[5];
        cWork[5]  = wDate[4];
        cWork[6]  = wDate[7];
        cWork[7]  = wDate[6];
        cWork[8]  = wDate[9];
        cWork[9]  = wDate[8];
        cWork[10] = wDate[11];
        cWork[11] = wDate[10];

        if((0 == strcmp(dFlg, "ON")) && (0 == strcmp(tFlg, "ON"))) {
            memcpy(&g_pSendDmy[addr], cWork, sizeof(short)*6);
            g_setAddr =  g_setAddr + (sizeof(short)*6);
        } else if((0 == strcmp(dFlg, "ON")) && (0 != strcmp(tFlg, "ON"))) {
            memcpy(&g_pSendDmy[addr], cWork, sizeof(short)*3);
            g_setAddr =  g_setAddr + (sizeof(short)*3);
        } else if((0 != strcmp(dFlg, "ON")) && (0 == strcmp(tFlg, "ON"))) {
            memcpy(&g_pSendDmy[addr], &cWork[6], sizeof(short)*3);
            g_setAddr =  g_setAddr + (sizeof(short)*3);
        }
    } else if(4 == kind) {
        iDate_Time[0] = wDate_Time[0];
        iDate_Time[1] = wDate_Time[1];
        iDate_Time[2] = wDate_Time[2];
        iDate_Time[3] = wDate_Time[3];
        iDate_Time[4] = wDate_Time[4];
        iDate_Time[5] = wDate_Time[5];
        memcpy(wDate, iDate_Time, sizeof(iDate_Time));

        cWork[0]  = wDate[3];
        cWork[1]  = wDate[2];
        cWork[2]  = wDate[1];
        cWork[3]  = wDate[0];

        cWork[4]  = wDate[7];
        cWork[5]  = wDate[6];
        cWork[6]  = wDate[5];
        cWork[7]  = wDate[4];

        cWork[8]  = wDate[11];
        cWork[9]  = wDate[10];
        cWork[10] = wDate[9];
        cWork[11] = wDate[8];

        cWork[12] = wDate[15];
        cWork[13] = wDate[14];
        cWork[14] = wDate[13];
        cWork[15] = wDate[12];

        cWork[16] = wDate[19];
        cWork[17] = wDate[18];
        cWork[18] = wDate[17];
        cWork[19] = wDate[16];

        cWork[20] = wDate[23];
        cWork[21] = wDate[22];
        cWork[22] = wDate[21];
        cWork[23] = wDate[20];
        if((0 == strcmp(dFlg, "ON")) && (0 == strcmp(tFlg, "ON"))) {
            memcpy(&g_pSendDmy[addr], cWork, sizeof(int)*6);
            g_setAddr =  g_setAddr + (sizeof(int)*6);
        } else if((0 == strcmp(dFlg, "ON")) && (0 != strcmp(tFlg, "ON"))) {            
            memcpy(&g_pSendDmy[addr], cWork, sizeof(int)*3);
            g_setAddr =  g_setAddr + (sizeof(int)*3);
        } else if((0 != strcmp(dFlg, "ON")) && (0 == strcmp(tFlg, "ON"))) {            
            memcpy(&g_pSendDmy[addr], &cWork[12], sizeof(int)*3);
            g_setAddr =  g_setAddr + (sizeof(int)*3);
        }
    }
    return TRUE;
}


/*-------------------------------------------------------------------
*
*
*
*--------------------------------------------------------------------*/
BOOL CountShortDataSet(HWND hWnd, unsigned char* buff, unsigned char *fileName)
{
    char    type[50];
    short   sData;
    long    idata;
    int     addr;
    char    cnt_file[256];
	struct	_stat	buf;
    char    nup[256];
    int     nfp, start, end, i_nup;

    memset(cnt_file, 0x00, sizeof(cnt_file));

    if('0' <= buff[0] &&  buff[0] <= '9') {
    	sscanf(buff, "%d %s %d %d %s", &addr, type, &start, &end, nup);
        g_setAddr = addr;
    } else {
    	sscanf(buff, "%s %d %d %s", type,  &start, &end, nup);
        addr = g_setAddr;
    }
//    MyMessage( hWnd, "%d %s %d %d %s", addr, type, start, end, nup );

    i_nup = get_i_nup(start, end, nup);

    sprintf(cnt_file, "%s-%05d", fileName, addr);

    // 追番を追加したﾌｧｲﾙ名の存在ﾁｪｯｸ
	if(_stat(cnt_file, &buf) < 0) {
        // ﾌｧｲﾙが存在しない場合、ﾌｧｲﾙ生成
        idata = start;
    	nfp = AP_Sfopen(cnt_file, AP_CREATRDWR);
	    if (nfp < 0) {
    		MyMessage( hWnd, "CNT_SHORT COUNTﾌｧｲﾙ生成異常");
            return FALSE;
	    }
        AP_Sprec(nfp, sizeof(long), 0, &idata);
	    AP_Sfclose(nfp);
		SendDlgItemMessage( hWnd, IDC_CNT_FILE, LB_ADDSTRING, 0, (LPARAM)(unsigned char *)cnt_file );
    } else {
	  	nfp = AP_Sfopen(cnt_file, AP_RDWR);
		if (nfp < 0) {
    		MyMessage( hWnd, "CNT_SHORT COUNTﾌｧｲﾙ読み込み異常");
            return FALSE;
		}
		AP_Sgrec(nfp, sizeof(long), 0, &idata);

        calc_idata(start, end, i_nup, &idata);

	    AP_Sprec(nfp, sizeof(long), 0, &idata);
	    AP_Sfclose(nfp);
	}

    sData = (short)idata;
   	memcpy(&g_pSendDmy[addr], &sData, sizeof(short));

    g_setAddr = g_setAddr + sizeof(short);

    return TRUE;
}

/*-------------------------------------------------------------------
*
*   
*
*--------------------------------------------------------------------*/
BOOL CountLongDataSet(HWND hWnd, unsigned char* buff, unsigned char *fileName)
{
    char    type[50];
    long    idata;
    int     addr;
    char    cnt_file[256];
	struct	_stat	buf;
    char    nup[256];
    int     nfp, start, end, i_nup;

    memset(cnt_file, 0x00, sizeof(cnt_file));

    if('0' <= buff[0] &&  buff[0] <= '9') {
    	sscanf(buff, "%d %s %d %d %s", &addr, type, &start, &end, nup);
        g_setAddr = addr;
    } else {
    	sscanf(buff, "%s %d %d %s", type,  &start, &end, nup);
        addr = g_setAddr;
    }
//    MyMessage( hWnd, "%d %s %d %d %s", addr, type, start, end, nup );

    i_nup = get_i_nup(start, end, nup);

    sprintf(cnt_file, "%s-%05d", fileName, addr);

    // 追番を追加したﾌｧｲﾙ名の存在ﾁｪｯｸ
	if(_stat(cnt_file, &buf) < 0) {
        // ﾌｧｲﾙが存在しない場合、ﾌｧｲﾙ生成
        idata = start;
    	nfp = AP_Sfopen(cnt_file, AP_CREATRDWR);
	    if (nfp < 0) {
    		MyMessage( hWnd, "CNT_LONG COUNTﾌｧｲﾙ生成異常");
		    return FALSE;
	    }
        AP_Sprec(nfp, sizeof(long), 0, &idata);
	    AP_Sfclose(nfp);
		SendDlgItemMessage( hWnd, IDC_CNT_FILE, LB_ADDSTRING, 0, (LPARAM)(unsigned char *)cnt_file );
    } else {
	  	nfp = AP_Sfopen(cnt_file, AP_RDWR);
		if (nfp < 0) {
    		MyMessage( hWnd, "CNT_LONG COUNTﾌｧｲﾙ読み込み異常");
			return FALSE;
		}
		AP_Sgrec(nfp, sizeof(long), 0, &idata);

        calc_idata(start, end, i_nup, &idata);

	    AP_Sprec(nfp, sizeof(long), 0, &idata);
	    AP_Sfclose(nfp);
	}

   	memcpy(&g_pSendDmy[addr], &idata, sizeof(long));

    g_setAddr = g_setAddr + sizeof(long);

    return TRUE;
}


/*-------------------------------------------------------------------
*
*   
*
*--------------------------------------------------------------------*/
BOOL CountXShortDataSet(HWND hWnd, unsigned char* buff, unsigned char *fileName)
{
    char    type[50];
    char    cWork[2];
    char    cData[2];
    short   sData;
    long    idata;
    int     addr;
    char    cnt_file[256];
	struct	_stat	buf;
    char    nup[256];
    int     nfp, start, end, i_nup;

    memset(cnt_file, 0x00, sizeof(cnt_file));

    if('0' <= buff[0] &&  buff[0] <= '9') {
    	sscanf(buff, "%d %s %d %d %s", &addr, type, &start, &end, nup);
        g_setAddr = addr;
    } else {
    	sscanf(buff, "%s %d %d %s", type,  &start, &end, nup);
        addr = g_setAddr;
    }
//    MyMessage( hWnd, "%d %s %d %d", addr, type, start, end );

    i_nup = get_i_nup(start, end, nup);

    sprintf(cnt_file, "%s-%05d", fileName, addr);

    // 追番を追加したﾌｧｲﾙ名の存在ﾁｪｯｸ
	if(_stat(cnt_file, &buf) < 0) {
        // ﾌｧｲﾙが存在しない場合、ﾌｧｲﾙ生成
        idata = start;
    	nfp = AP_Sfopen(cnt_file, AP_CREATRDWR);
	    if (nfp < 0) {
    		MyMessage( hWnd, "CNT_XSHORT COUNTﾌｧｲﾙ生成異常");
		    return FALSE;
	    }
        AP_Sprec(nfp, sizeof(long), 0, &idata);
	    AP_Sfclose(nfp);
		SendDlgItemMessage( hWnd, IDC_CNT_FILE, LB_ADDSTRING, 0, (LPARAM)(unsigned char *)cnt_file );
    } else {
	  	nfp = AP_Sfopen(cnt_file, AP_RDWR);
		if (nfp < 0) {
    		MyMessage( hWnd, "CNT_XSHORT COUNTﾌｧｲﾙ読み込み異常");
			return FALSE;
		}
		AP_Sgrec(nfp, sizeof(long), 0, &idata);

        calc_idata(start, end, i_nup, &idata);

	    AP_Sprec(nfp, sizeof(long), 0, &idata);
	    AP_Sfclose(nfp);
	}

    sData = (short)idata;
   	memcpy(cWork, &sData, sizeof(short));

    cData[0] = cWork[1];
    cData[1] = cWork[0];

    memcpy(&g_pSendDmy[addr], cData, sizeof(short));

    g_setAddr = g_setAddr + sizeof(short);

    return TRUE;
}


/*-------------------------------------------------------------------
*
*   
*
*--------------------------------------------------------------------*/
BOOL CountXLongDataSet(HWND hWnd, unsigned char* buff, unsigned char *fileName)
{
    char    type[50];
    char    cWork[4];
    char    cData[4];
    long    idata;
    int     addr;
    char    cnt_file[256];
	struct	_stat	buf;
    char    nup[256];
    int     nfp, start, end, i_nup;

    memset(cnt_file, 0x00, sizeof(cnt_file));

    if('0' <= buff[0] &&  buff[0] <= '9') {
    	sscanf(buff, "%d %s %d %d %s", &addr, type, &start, &end, nup);
        g_setAddr = addr;
    } else {
    	sscanf(buff, "%s %d %d %s", type,  &start, &end, nup);
        addr = g_setAddr;
    }
//    MyMessage( hWnd, "%d %s %d %d", addr, type, start, end );

    i_nup = get_i_nup(start, end, nup);

    sprintf(cnt_file, "%s-%05d", fileName, addr);

    // 追番を追加したﾌｧｲﾙ名の存在ﾁｪｯｸ
	if(_stat(cnt_file, &buf) < 0) {
        // ﾌｧｲﾙが存在しない場合、ﾌｧｲﾙ生成
        idata = start;
    	nfp = AP_Sfopen(cnt_file, AP_CREATRDWR);
	    if (nfp < 0) {
    		MyMessage( hWnd, "CNT_XLONG COUNTﾌｧｲﾙ生成異常");
		    return FALSE;
	    }
        AP_Sprec(nfp, sizeof(long), 0, &idata);
	    AP_Sfclose(nfp);
		SendDlgItemMessage( hWnd, IDC_CNT_FILE, LB_ADDSTRING, 0, (LPARAM)(unsigned char *)cnt_file );
    } else {
	  	nfp = AP_Sfopen(cnt_file, AP_RDWR);
		if (nfp < 0) {
    		MyMessage( hWnd, "CNT_XLONG COUNTﾌｧｲﾙ読み込み異常");
			return FALSE;
		}
		AP_Sgrec(nfp, sizeof(long), 0, &idata);

        calc_idata(start, end, i_nup, &idata);

	    AP_Sprec(nfp, sizeof(long), 0, &idata);
	    AP_Sfclose(nfp);
	}

   	memcpy(cWork, &idata, sizeof(long));

    cData[0] = cWork[3];
    cData[1] = cWork[2];
    cData[2] = cWork[1];
    cData[3] = cWork[0];

    memcpy(&g_pSendDmy[addr], &cData, sizeof(long));
    g_setAddr =  g_setAddr + sizeof(long);

    return TRUE;
}


/*-------------------------------------------------------------------
*
*   
*
*--------------------------------------------------------------------*/
BOOL CountCharDataSet(HWND hWnd, unsigned char* buff, unsigned char *fileName)
{
    char    type[50];
    char    fmt[50];
    char    cbuff[50];
    long    idata;
    int     addr;
    char    cnt_file[256];
	struct	_stat	buf;
    char    nup[256];
    int     nfp, start, end, i_nup;

    memset(cnt_file, 0x00, sizeof(cnt_file));
    memset(cbuff, 0x00, sizeof(cbuff));

    if('0' <= buff[0] &&  buff[0] <= '9') {
    	sscanf(buff, "%d %s %s %d %d %s", &addr, type, fmt, &start, &end, nup);
        g_setAddr = addr;
    } else {
    	sscanf(buff, "%s %s %d %d %s", type, fmt, &start, &end, nup);
        addr = g_setAddr;
    }
//    MyMessage( hWnd, "%d %s %s %d %d", addr, type, fmt, start, end );

    i_nup = get_i_nup(start, end, nup);

    sprintf(cnt_file, "%s-%05d", fileName, addr);

    // 追番を追加したﾌｧｲﾙ名の存在ﾁｪｯｸ
	if(_stat(cnt_file, &buf) < 0) {
        // ﾌｧｲﾙが存在しない場合、ﾌｧｲﾙ生成
        idata = start;
    	nfp = AP_Sfopen(cnt_file, AP_CREATRDWR);
	    if (nfp < 0) {
    		MyMessage( hWnd, "CNT_CHAR COUNTﾌｧｲﾙ生成異常");
		    return FALSE;
	    }
        AP_Sprec(nfp, sizeof(long), 0, &idata);
	    AP_Sfclose(nfp);
		SendDlgItemMessage( hWnd, IDC_CNT_FILE, LB_ADDSTRING, 0, (LPARAM)(unsigned char *)cnt_file );
    } else {
	  	nfp = AP_Sfopen(cnt_file, AP_RDWR);
		if (nfp < 0) {
    		MyMessage( hWnd, "CNT_CHAR COUNTﾌｧｲﾙ読み込み異常");
			return FALSE;
		}
		AP_Sgrec(nfp, sizeof(long), 0, &idata);

        calc_idata(start, end, i_nup, &idata);

	    AP_Sprec(nfp, sizeof(long), 0, &idata);
	    AP_Sfclose(nfp);
	}

    sprintf(cbuff, fmt, idata);
   	memcpy(&g_pSendDmy[addr], cbuff, strlen(cbuff));

    g_setAddr = g_setAddr + strlen(cbuff);

    return TRUE;
}

/*-------------------------------------------------------------------
*
*   
*
*--------------------------------------------------------------------*/
BOOL CountEbcDataSet(HWND hWnd, unsigned char* buff, unsigned char *fileName)
{
    char    type[50];
    char    fmt[50];
    char    cbuff[50];
    char    eData[50];
    long    idata;
    int     addr;
    char    cnt_file[256];
	struct	_stat	buf;
    char    nup[256];
    int     nfp, start, end, i_nup;

    memset(cnt_file, 0x00, sizeof(cnt_file));
    memset(cbuff, 0x00, sizeof(cbuff));

    if('0' <= buff[0] &&  buff[0] <= '9') {
    	sscanf(buff, "%d %s %s %d %d %s", &addr, type, fmt, &start, &end, nup);
        g_setAddr = addr;
    } else {
    	sscanf(buff, "%s %s %d %d %s", type, fmt, &start, &end, nup);
        addr = g_setAddr;
    }
//    MyMessage( hWnd, "%d %s %s %d %d", addr, type, fmt, start, end );

    i_nup = get_i_nup(start, end, nup);

    sprintf(cnt_file, "%s-%05d", fileName, addr);

    // 追番を追加したﾌｧｲﾙ名の存在ﾁｪｯｸ
	if(_stat(cnt_file, &buf) < 0) {
        // ﾌｧｲﾙが存在しない場合、ﾌｧｲﾙ生成
        idata = start;
    	nfp = AP_Sfopen(cnt_file, AP_CREATRDWR);
	    if (nfp < 0) {
    		MyMessage( hWnd, "CNT_EBC COUNTﾌｧｲﾙ生成異常");
		    return FALSE;
	    }
        AP_Sprec(nfp, sizeof(long), 0, &idata);
	    AP_Sfclose(nfp);
		SendDlgItemMessage( hWnd, IDC_CNT_FILE, LB_ADDSTRING, 0, (LPARAM)(unsigned char *)cnt_file );
    } else {
	  	nfp = AP_Sfopen(cnt_file, AP_RDWR);
		if (nfp < 0) {
    		MyMessage( hWnd, "CNT_EBC COUNTﾌｧｲﾙ読み込み異常");
			return FALSE;
		}
		AP_Sgrec(nfp, sizeof(long), 0, &idata);

        calc_idata(start, end, i_nup, &idata);

	    AP_Sprec(nfp, sizeof(long), 0, &idata);
	    AP_Sfclose(nfp);
	}

    sprintf(cbuff, fmt, idata);
    strnatoe(cbuff, eData, strlen(cbuff));
   	memcpy(&g_pSendDmy[addr], eData, strlen(cbuff));
    g_setAddr = g_setAddr + strlen(cbuff);

    return TRUE;
}

static int get_i_nup(int start, int end, char nup[256])
{
    int i_nup;

    if('0' <= nup[0] && nup[0] <= '9') {
        i_nup = atoi(nup);
    } else {
        i_nup = 1;
    }
    if( start > end ) {
        i_nup = i_nup * (-1);
    }
    return i_nup;
}

static void calc_idata(int start, int end, int i_nup, int* idata)
{
	*idata = *idata + i_nup;

    if( start <= end ) {
        if(start <= *idata && *idata <= end) {
            // nothing to do
        } else {
            *idata = start;
        }
	} else {
        if(end <= *idata && *idata <= start) {
            // nothing to do
		} else {
	        *idata = start;
		}
    }
    return;
}




/*-------------------------------------------------------------------
*
*   
*
*--------------------------------------------------------------------*/
BOOL FileIntToCharSet(HWND hWnd, unsigned char* buff)
{
    char    type[50];
    char    fmt[50];
    char    cbuff[50];
    long    idata;
    int     addr;
    struct  _stat   buf;
    int     recno, nfp;
    CountFile fdata;

    memset(&fdata, 0x00, sizeof(CountFile));
    memset(cbuff, 0x00, sizeof(cbuff));

    if('0' <= buff[0] &&  buff[0] <= '9') {
        sscanf(buff, "%d %s %d %s", &addr, type, &recno, fmt);
        g_setAddr = addr;
    } else {
        sscanf(buff, "%s %d %s", type, &recno, fmt);
        addr = g_setAddr;
    }
//    MyMessage( hWnd, "%d %s %s %d", addr, type, fmt, recno );

    if(_stat(tcpCtrl.counterFile, &buf) < 0) {
//    MyMessage( hWnd, "------[%s]", tcpCtrl.counterFile );
        // ﾌｧｲﾙが存在しない場合、ﾌｧｲﾙ生成
        idata = 0;
    } else {
        nfp = AP_Sfopen(tcpCtrl.counterFile, AP_RDONLY);
        if (nfp < 0) {
            MyMessage( hWnd, "tcpsp counterﾌｧｲﾙ読み込み異常");
            return FALSE;
        }
        AP_Sgrec(nfp, sizeof(CountFile), recno-1, &fdata);
        AP_Sfclose(nfp);
//    MyMessage( hWnd, "------[%d]", fdata.idata );
        idata = fdata.idata;
    }

    sprintf(cbuff, fmt, idata);
    memcpy(&g_pSendDmy[addr], cbuff, strlen(cbuff));

//    MyMessage( hWnd, "cbuff[%s]", cbuff );

    g_setAddr = g_setAddr + strlen(cbuff);

    return TRUE;
}

/*-------------------------------------------------------------------
*
*   
*
*--------------------------------------------------------------------*/
BOOL FileCharToCharSet(HWND hWnd, unsigned char* buff)
{
    char    type[50];
    char    cbuff[50];
    char    wk[128];
    int     addr;
    int     byte;
    struct  _stat   buf;
    int     recno, nfp;
    StrFile fdata;

    memset(&fdata, 0x00, sizeof(CountFile));
    memset(cbuff, 0x00, sizeof(cbuff));
    memset(wk, 0x00, sizeof(wk));

    if('0' <= buff[0] &&  buff[0] <= '9') {
        sscanf(buff, "%d %s %d %d", &addr, type, &recno, &byte);
        g_setAddr = addr;
    } else {
        sscanf(buff, "%s %d %d", type, &recno, &byte);
        addr = g_setAddr;
    }
    // 0010     FILE_CHAR_CHAR     12 %s  3

//    MyMessage( hWnd, "-----addr=%d type=%s recno=%d byte=%d", addr, type, recno, byte );

    if(_stat(tcpCtrl.counterFile, &buf) < 0) {
//    MyMessage( hWnd, "------[%s]", tcpCtrl.counterFile );
        // ﾌｧｲﾙが存在しない場合、ﾌｧｲﾙ生成
    } else {
        nfp = AP_Sfopen(tcpCtrl.counterFile, AP_RDONLY);
        if (nfp < 0) {
            MyMessage( hWnd, "tcpsp counterﾌｧｲﾙ読み込み異常");
            return FALSE;
        }
        AP_Sgrec(nfp, sizeof(CountFile), recno-1, &fdata);
        AP_Sfclose(nfp);
//    MyMessage( hWnd, "------[%d]", fdata.cdata );
        memcpy(wk, fdata.cdata, byte);
    }

    memcpy(&g_pSendDmy[addr], wk, byte);

//    MyMessage( hWnd, "cbuff[%s]", cbuff );

    g_setAddr = g_setAddr + byte;

    return TRUE;
}


/*-------------------------------------------------------------------
*
*   
*
*--------------------------------------------------------------------*/
BOOL FileIntToEbcSet(HWND hWnd, unsigned char* buff)
{
    char    type[50];
    char    fmt[50];
    char    cbuff[50];
    char    ebuff[50];
    long    idata;
    int     addr;
    struct  _stat   buf;
    int     recno, nfp;
    CountFile fdata;

    memset(&fdata, 0x00, sizeof(CountFile));
    memset(cbuff, 0x00, sizeof(cbuff));
    memset(ebuff, 0x00, sizeof(ebuff));

    if('0' <= buff[0] &&  buff[0] <= '9') {
        sscanf(buff, "%d %s %d %s", &addr, type, &recno, fmt);
        g_setAddr = addr;
    } else {
        sscanf(buff, "%s %d %s", type, &recno, fmt);
        addr = g_setAddr;
    }
//    MyMessage( hWnd, "%d %s %s %d", addr, type, fmt, recno );

    if(_stat(tcpCtrl.counterFile, &buf) < 0) {
//    MyMessage( hWnd, "------[%s]", tcpCtrl.counterFile );
        // ﾌｧｲﾙが存在しない場合、ﾌｧｲﾙ生成
        idata = 0;
    } else {
        nfp = AP_Sfopen(tcpCtrl.counterFile, AP_RDONLY);
        if (nfp < 0) {
            MyMessage( hWnd, "tcpsp counterﾌｧｲﾙ読み込み異常");
            return FALSE;
        }
        AP_Sgrec(nfp, sizeof(CountFile), recno-1, &fdata);
        AP_Sfclose(nfp);
//    MyMessage( hWnd, "------[%d]", fdata.idata );
        idata = fdata.idata;
    }

    sprintf(cbuff, fmt, idata);

    strnatoe(cbuff, ebuff, strlen(cbuff));

    memcpy(&g_pSendDmy[addr], ebuff, strlen(cbuff));

//    MyMessage( hWnd, "cbuff[%s]", cbuff );

    g_setAddr = g_setAddr + strlen(cbuff);

    return TRUE;
}

/*-------------------------------------------------------------------
*
*   
*
*--------------------------------------------------------------------*/
BOOL FileCharToEbcSet(HWND hWnd, unsigned char* buff)
{
    char    type[50];
    char    cbuff[50];
    char    wk[128];
    char    ebuff[128];
    int     addr;
    int     byte;
    struct  _stat   buf;
    int     recno, nfp;
    StrFile fdata;

    memset(&fdata, 0x00, sizeof(CountFile));
    memset(cbuff, 0x00, sizeof(cbuff));
    memset(wk, 0x00, sizeof(wk));
    memset(ebuff, 0x00, sizeof(ebuff));

    if('0' <= buff[0] &&  buff[0] <= '9') {
        sscanf(buff, "%d %s %d %d", &addr, type, &recno, &byte);
        g_setAddr = addr;
    } else {
        sscanf(buff, "%s %d %d", type, &recno, &byte);
        addr = g_setAddr;
    }
    // 0010     FILE_CHAR_CHAR     12 %s  3

//    MyMessage( hWnd, "-----addr=%d type=%s recno=%d byte=%d", addr, type, recno, byte );

    if(_stat(tcpCtrl.counterFile, &buf) < 0) {
//    MyMessage( hWnd, "------[%s]", tcpCtrl.counterFile );
        // ﾌｧｲﾙが存在しない場合、ﾌｧｲﾙ生成
    } else {
        nfp = AP_Sfopen(tcpCtrl.counterFile, AP_RDONLY);
        if (nfp < 0) {
            MyMessage( hWnd, "tcpsp counterﾌｧｲﾙ読み込み異常");
            return FALSE;
        }
        AP_Sgrec(nfp, sizeof(CountFile), recno-1, &fdata);
        AP_Sfclose(nfp);
//    MyMessage( hWnd, "------[%d]", fdata.cdata );
        memcpy(wk, fdata.cdata, byte);
    }

    strnatoe(wk, ebuff, byte);

    memcpy(&g_pSendDmy[addr], ebuff, byte);

//    MyMessage( hWnd, "cbuff[%s]", cbuff );

    g_setAddr = g_setAddr + byte;

    return TRUE;
}


/*-------------------------------------------------------------------
*
*   
*
*--------------------------------------------------------------------*/
BOOL FileIntToIntSet(HWND hWnd, unsigned char* buff)
{
    char    type[50];
    char    cbuff[50];
    long    idata;
    int     addr;
    struct  _stat   buf;
    int     recno, nfp;
    CountFile fdata;

    memset(&fdata, 0x00, sizeof(CountFile));
    memset(cbuff, 0x00, sizeof(cbuff));

    if('0' <= buff[0] &&  buff[0] <= '9') {
        sscanf(buff, "%d %s %d", &addr, type, &recno);
        g_setAddr = addr;
    } else {
        sscanf(buff, "%s %d", type, &recno);
        addr = g_setAddr;
    }
//    MyMessage( hWnd, "%d %s %d", addr, type, recno );

    if(_stat(tcpCtrl.counterFile, &buf) < 0) {
//    MyMessage( hWnd, "------[%s]", tcpCtrl.counterFile );
        // ﾌｧｲﾙが存在しない場合、ﾌｧｲﾙ生成
        idata = 0;
    } else {
        nfp = AP_Sfopen(tcpCtrl.counterFile, AP_RDONLY);
        if (nfp < 0) {
            MyMessage( hWnd, "tcpsp counterﾌｧｲﾙ読み込み異常");
            return FALSE;
        }
        AP_Sgrec(nfp, sizeof(CountFile), recno-1, &fdata);
        AP_Sfclose(nfp);
//    MyMessage( hWnd, "------[%d]", fdata.idata );
        idata = fdata.idata;
    }

    memcpy( &g_pSendDmy[addr], &idata, sizeof(int));

//    MyMessage( hWnd, "cbuff[%s]", cbuff );

    g_setAddr = g_setAddr + sizeof(int);

    return TRUE;
}

/*-------------------------------------------------------------------
*
*   
*
*--------------------------------------------------------------------*/
BOOL FileIntToShortSet(HWND hWnd, unsigned char* buff)
{
    char    type[50];
    char    cbuff[50];
    short   sdata;
    int     addr;
    struct  _stat   buf;
    int     recno, nfp;
    CountFile fdata;

    memset(&fdata, 0x00, sizeof(CountFile));
    memset(cbuff, 0x00, sizeof(cbuff));

    if('0' <= buff[0] &&  buff[0] <= '9') {
        sscanf(buff, "%d %s %d", &addr, type, &recno);
        g_setAddr = addr;
    } else {
        sscanf(buff, "%s %d", type, &recno);
        addr = g_setAddr;
    }
//    MyMessage( hWnd, "%d %s %d", addr, type, recno );

    if(_stat(tcpCtrl.counterFile, &buf) < 0) {
//    MyMessage( hWnd, "------[%s]", tcpCtrl.counterFile );
        // ﾌｧｲﾙが存在しない場合、ﾌｧｲﾙ生成
        sdata = 0;
    } else {
        nfp = AP_Sfopen(tcpCtrl.counterFile, AP_RDONLY);
        if (nfp < 0) {
            MyMessage( hWnd, "tcpsp counterﾌｧｲﾙ読み込み異常");
            return FALSE;
        }
        AP_Sgrec(nfp, sizeof(CountFile), recno-1, &fdata);
        AP_Sfclose(nfp);
//    MyMessage( hWnd, "------[%d]", fdata.idata );
        sdata = fdata.idata;
    }

    memcpy( &g_pSendDmy[addr], &sdata, sizeof(short));

//    MyMessage( hWnd, "cbuff[%s]", cbuff );

    g_setAddr = g_setAddr + sizeof(short);

    return TRUE;
}


/*-------------------------------------------------------------------
*
*   
*
*--------------------------------------------------------------------*/
BOOL FileIntToXShortSet(HWND hWnd, unsigned char* buff)
{
    char    type[50];
    char    cbuff[50];
    char    cData[16];
    char    cWork[16];
    short   sdata;
    int     addr;
    struct  _stat   buf;
    int     recno, nfp;
    CountFile fdata;

    memset(&fdata, 0x00, sizeof(CountFile));
    memset(cbuff, 0x00, sizeof(cbuff));
    memset(cData, 0x00, sizeof(cData));
    memset(cWork, 0x00, sizeof(cWork));

    if('0' <= buff[0] &&  buff[0] <= '9') {
        sscanf(buff, "%d %s %d", &addr, type, &recno);
        g_setAddr = addr;
    } else {
        sscanf(buff, "%s %d", type, &recno);
        addr = g_setAddr;
    }
//    MyMessage( hWnd, "%d %s %d", addr, type, recno );

    if(_stat(tcpCtrl.counterFile, &buf) < 0) {
//    MyMessage( hWnd, "------[%s]", tcpCtrl.counterFile );
        // ﾌｧｲﾙが存在しない場合、ﾌｧｲﾙ生成
        sdata = 0;
    } else {
        nfp = AP_Sfopen(tcpCtrl.counterFile, AP_RDONLY);
        if (nfp < 0) {
            MyMessage( hWnd, "tcpsp counterﾌｧｲﾙ読み込み異常");
            return FALSE;
        }
        AP_Sgrec(nfp, sizeof(CountFile), recno-1, &fdata);
        AP_Sfclose(nfp);
//    MyMessage( hWnd, "------[%d]", fdata.idata );
        sdata = (short)fdata.idata;
    }
    memcpy(cWork, &sdata, sizeof(short));

    cData[0] = cWork[1];
    cData[1] = cWork[0];

    memcpy(&g_pSendDmy[addr], cData, sizeof(short));

    g_setAddr = g_setAddr + sizeof(short);

    return TRUE;
}


/*-------------------------------------------------------------------
*
*   
*
*--------------------------------------------------------------------*/
BOOL FileIntToBinSet(HWND hWnd, unsigned char* buff)
{
    char    type[50];
    char    cbuff[50];
    char    cdata;
    int     addr;
    struct  _stat   buf;
    int     recno, nfp;
    CountFile fdata;

    memset(&fdata, 0x00, sizeof(CountFile));
    memset(cbuff, 0x00, sizeof(cbuff));

    if('0' <= buff[0] &&  buff[0] <= '9') {
        sscanf(buff, "%d %s %d", &addr, type, &recno);
        g_setAddr = addr;
    } else {
        sscanf(buff, "%s %d", type, &recno);
        addr = g_setAddr;
    }
//    MyMessage( hWnd, "%d %s %d", addr, type, recno );

    if(_stat(tcpCtrl.counterFile, &buf) < 0) {
//    MyMessage( hWnd, "------[%s]", tcpCtrl.counterFile );
        // ﾌｧｲﾙが存在しない場合、ﾌｧｲﾙ生成
        cdata = 0;
    } else {
        nfp = AP_Sfopen(tcpCtrl.counterFile, AP_RDONLY);
        if (nfp < 0) {
            MyMessage( hWnd, "tcpsp counterﾌｧｲﾙ読み込み異常");
            return FALSE;
        }
        AP_Sgrec(nfp, sizeof(CountFile), recno-1, &fdata);
        AP_Sfclose(nfp);
//    MyMessage( hWnd, "------[%d]", fdata.idata );
        cdata = (char)fdata.idata;
    }

    memcpy(&g_pSendDmy[addr], &cdata, sizeof(char));

    g_setAddr = g_setAddr + sizeof(char);

    return TRUE;
}


