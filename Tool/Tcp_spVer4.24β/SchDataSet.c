#include    <conio.h>
#include    <stdio.h>
#include    <windows.h>
#include    <winsock.h>
#include	"AP_Func.h"
#include    "TcpCtrl.h"
#include	"SchDataSet.h"


/*-------------------------------------------------------------------
*
*   定周期送信の読み込み
*
*--------------------------------------------------------------------*/
BOOL  MakeCycleSchDataSet(HWND hWnd, unsigned char* buff, int setAddr)
{
    int     dmy;
    int     sec;
    int     port;
    struct	_stat	buf;
    char    fname[512];

   	sscanf(buff, "%s %d %d %s", &dmy, &port, &sec, fname);

    if(_stat(fname, &buf) != 0) {
  	    MyMessage(hWnd, "");
        MyMessage(hWnd, "[定周期ｽｹｼﾞｭｰﾙ管理ﾌｧｲﾙ異常]", fname);
        MyMessage(hWnd, "[%s] 定義ﾌｧｲﾙ無し", fname);
        return FALSE;
    }
    strcpy(CycDef[setAddr].fname, fname);
    CycDef[setAddr].sec     = sec;
    CycDef[setAddr].port    = port;

    return TRUE;
}

/*-------------------------------------------------------------------
*
*   シナリオ情報の読み込み
*
*--------------------------------------------------------------------*/
BOOL  MakeSinaSchDataSet(HWND hWnd, unsigned char* buff, int setAddr)
{
    int     sec;
    int     port;
    struct	_stat	buf;
    char    fname[512];

   	sscanf(buff, "%d %d %s", &port, &sec, fname);

    if(_stat(fname, &buf) != 0) {
  	    MyMessage(hWnd, "");
        MyMessage(hWnd, "[ｼﾅﾘｵﾌｧｲﾙ異常]", fname);
        MyMessage(hWnd, "[%s] 定義ﾌｧｲﾙ無し", fname);
        return FALSE;
    }

    strcpy(SinaDef[setAddr].fname, fname);
    SinaDef[setAddr].sec     = sec;
    SinaDef[setAddr].port    = port;

    return TRUE;
}


/*-------------------------------------------------------------------
*
*   シナリオ情報の読み込み
*
*--------------------------------------------------------------------*/
BOOL  MakeRecvSinaSchDataSet(HWND hWnd, unsigned char* buff, int setAddr, int point, int rcv_port)
{
    int     sec;
    int     port;
    struct	_stat	buf;
    char    fname[512];

   	sscanf(buff, "%d %d %s", &port, &sec, fname);

    if(_stat(fname, &buf) != 0) {
  	    MyMessage(hWnd, "");
        MyMessage(hWnd, "[受信ｼﾅﾘｵﾌｧｲﾙ異常]", fname);
        MyMessage(hWnd, "[%s] 定義ﾌｧｲﾙ無し", fname);
        return FALSE;
    }
    strcpy(RecvSinaDef[point][setAddr].fname, fname);
    RecvSinaDef[point][setAddr].sec     = sec;
    RecvSinaDef[point][setAddr].port    = port;
    if(0 == port) {
        RecvSinaDef[point][setAddr].port = rcv_port;
    } else {
        RecvSinaDef[point][setAddr].port = port;
    }
    return TRUE;
}



/*-------------------------------------------------------------------
*
*   ＳＨＯＲＴデータ作成、バッファ格納
*
*--------------------------------------------------------------------*/
BOOL ShortDataSetSch(HWND hWnd, unsigned char* buff, int setAddr, int itemAddr)
{
    char    type[10];
    short   sData;
    int     addr;
    int     port;
	char    dmy[32];

	if(0 == itemAddr) {
	   	sscanf(buff, "%d %d %s %d %s %d %d", &port, &addr, type, &sData, SchDef[setAddr].fname, &SchDef[setAddr].fnum, &SchDef[setAddr].randam);
	} else {
	   	sscanf(buff, "%s %d %s %d", dmy, &addr, type, &sData);
	}

	if(0 == itemAddr) {
	    SchDef[setAddr].port     = port;
	}
    SchDef[setAddr].item[itemAddr].addr = addr;
    SchDef[setAddr].item[itemAddr].type = SHORT_TYPE;
    SchDef[setAddr].item[itemAddr].length = sizeof(short);
    memcpy(SchDef[setAddr].item[itemAddr].ptn, &sData, sizeof(short));

    return TRUE;
}

BOOL XShortDataSetSch(HWND hWnd, unsigned char* buff, int setAddr, int itemAddr)
{
    char    type[10];
    short   sData;
    int     addr;
    int     port;
	char    dmy[32];
    char    cWork[2];
    char    cData[2];


	if(0 == itemAddr) {
	   	sscanf(buff, "%d %d %s %d %s %d %d", &port, &addr, type, &sData, SchDef[setAddr].fname, &SchDef[setAddr].fnum, &SchDef[setAddr].randam);
	} else {
	   	sscanf(buff, "%s %d %s %d", dmy, &addr, type, &sData);
	}
   	memcpy(cWork, &sData, sizeof(short));

	if(0 == itemAddr) {
	    SchDef[setAddr].port     = port;
	}

    cData[0] = cWork[1];
    cData[1] = cWork[0];

    SchDef[setAddr].item[itemAddr].addr = addr;
    SchDef[setAddr].item[itemAddr].type = XSHORT_TYPE;
    SchDef[setAddr].item[itemAddr].length = sizeof(short);
    memcpy(SchDef[setAddr].item[itemAddr].ptn, &cData, sizeof(short));

    return TRUE;
}

/*-------------------------------------------------------------------
*
*   ＬＯＮＧデータ作成、バッファ格納
*
*--------------------------------------------------------------------*/
BOOL LongDataSetSch(HWND hWnd, unsigned char* buff, int setAddr, int itemAddr)
{
    char    type[10];
    long    sData;
    int     addr;
    int     port;
	char    dmy[32];

	if(0 == itemAddr) {
	   	sscanf(buff, "%d %d %s %d %s %d %d", &port, &addr, type, &sData, SchDef[setAddr].fname, &SchDef[setAddr].fnum, &SchDef[setAddr].randam);
	} else {
	   	sscanf(buff, "%s %d %s %d", dmy, &addr, type, &sData);
	}
	if(0 == itemAddr) {
		SchDef[setAddr].port     = port;
	}
    SchDef[setAddr].item[itemAddr].addr     = addr;
    SchDef[setAddr].item[itemAddr].type     = LONG_TYPE;
    SchDef[setAddr].item[itemAddr].length   = sizeof(long);
    memcpy(SchDef[setAddr].item[itemAddr].ptn, &sData, sizeof(long));

    return TRUE;
}

BOOL XLongDataSetSch(HWND hWnd, unsigned char* buff, int setAddr, int itemAddr)
{
    char    type[10];
    long    sData;
    int     addr;
    int     port;
	char    dmy[32];
    char    cWork[4];
    char    cData[4];

	if(0 == itemAddr) {
	   	sscanf(buff, "%d %d %s %d %s %d %d", &port, &addr, type, &sData, SchDef[setAddr].fname, &SchDef[setAddr].fnum, &SchDef[setAddr].randam);
	} else {
	   	sscanf(buff, "%s %d %s %d", dmy, &addr, type, &sData);
	}
    memcpy(cWork, &sData, sizeof(long));

    cData[0] = cWork[3];
    cData[1] = cWork[2];
    cData[2] = cWork[1];
    cData[3] = cWork[0];

	if(0 == itemAddr) {
		SchDef[setAddr].port     = port;
	}
    SchDef[setAddr].item[itemAddr].addr     = addr;
    SchDef[setAddr].item[itemAddr].type     = XLONG_TYPE;
    SchDef[setAddr].item[itemAddr].length   = sizeof(long);
    memcpy(SchDef[setAddr].item[itemAddr].ptn, &cData, sizeof(long));

    return TRUE;
}

/*-------------------------------------------------------------------
*
*   ＣＨＡＲデータ作成、バッファ格納
*
*--------------------------------------------------------------------*/
BOOL CharDataSetSch(HWND hWnd, unsigned char* buff, int setAddr, int itemAddr)
{
    char    type[10];

    char    cData[1024];
    char    dmy[1024];
    char    dmy1[1024];
    int     addr;
    int     i;
    int     set;
    int     sw;
    int     port;

	if(0 == itemAddr) {
	   	sscanf(buff, "%d %d %s %s", &port, &addr, type, dmy);
	} else {
	   	sscanf(buff, "%s %d %s %s", dmy1, &addr, type, dmy);
	}
    memset(cData, 0x00, sizeof(cData));
    set = 0;
    sw  = 1;
    for(i=0; i<512 || 3==sw; i++) {
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

    if(0 == itemAddr) {
		sscanf(&buff[i+1], "%s %d %d", SchDef[setAddr].fname, &SchDef[setAddr].fnum, &SchDef[setAddr].randam);
		SchDef[setAddr].port     = port;
	}
    SchDef[setAddr].item[itemAddr].addr     = addr;
    SchDef[setAddr].item[itemAddr].type     = CHAR_TYPE;
    SchDef[setAddr].item[itemAddr].length   = set;
    memcpy(SchDef[setAddr].item[itemAddr].ptn, cData, set);

    return TRUE;
}

/*-------------------------------------------------------------------
*
*   EBCDICデータ作成、バッファ格納
*
*--------------------------------------------------------------------*/
BOOL EbcdicDataSetSch(HWND hWnd, unsigned char* buff, int setAddr, int itemAddr)
{
    char    type[10];

    char    cData[1024];
    char    eData[1024];
    char    dmy[32];
    char    dmy1[32];
    int     addr;
    int     i;
    int     set;
    int     sw;
    int     port;

	if(0 == itemAddr) {
	   	sscanf(buff, "%d %d %s %s", &port, &addr, type, dmy);
	} else {
	   	sscanf(buff, "%s %d %s %s", dmy1, &addr, type, dmy);
	}
    memset(cData, 0x00, sizeof(cData));
    set = 0;
    sw  = 1;
    for(i=0; i<512 || 3==sw; i++) {
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

    if(0 == itemAddr) {
		sscanf(&buff[i+1], "%s %d %d", SchDef[setAddr].fname, &SchDef[setAddr].fnum, &SchDef[setAddr].randam);
		SchDef[setAddr].port     = port;
	}

    SchDef[setAddr].item[itemAddr].addr     = addr;
    SchDef[setAddr].item[itemAddr].type     = EBCDIC_TYPE;
    SchDef[setAddr].item[itemAddr].length   = set;
    strnatoe(cData, eData, set);
    memcpy(SchDef[setAddr].item[itemAddr].ptn, eData, set);

    return TRUE;
}



/*-------------------------------------------------------------------
*
*   ＨＥＸデータ作成、バッファ格納
*
*--------------------------------------------------------------------*/
BOOL HexDataSetSch(HWND hWnd, int num, unsigned char* buff, int setAddr, int itemAddr)
{
    char    type[10];

    char    cHex[4];
    short   sHex;
    int     iHex;
    int     addr;
    int     cnt;
    int     port;
    int     i;
	int     rc;
	char    dmy[32];
	char    cdmy[32];

    if(1 == num) {
		if(0 == itemAddr) {
	        rc=sscanf(buff, "%d %d %s %s %d %s %d %d", &port, &addr, type, cdmy, &cnt, SchDef[setAddr].fname, &SchDef[setAddr].fnum, &SchDef[setAddr].randam);
			if(rc != 8) {
				MyMessage( hWnd, "[%s]  定義情報エラー！！", buff);
				MyMessage( hWnd, "アドレス=%d  TYPE=%s  定義情報エラー！！", addr, type);
				return FALSE;
			}
			if(0 == memcmp(cdmy, "0x", 2)){
				sscanf(&cdmy[2], "%2x", cHex);
			} else {
				sscanf(&cdmy[0], "%2x", cHex);
			}
		} else {
	        rc=sscanf(buff, "%s %d %s %s %d", dmy, &addr, type, cdmy, &cnt);
			if(rc != 5) {
				MyMessage( hWnd, "[%s]  定義情報エラー！！", buff);
				MyMessage( hWnd, "アドレス=%d  TYPE=%s  定義情報エラー！！", addr, type);
				return FALSE;
			}
			if(0 == memcmp(cdmy, "0x", 2)){
				sscanf(&cdmy[2], "%2x", cHex);
			} else {
				sscanf(&cdmy[0], "%2x", cHex);
			}
		}
		if(0 == itemAddr) {
	        SchDef[setAddr].port     = port;
		}
        SchDef[setAddr].item[itemAddr].addr     = addr;
        SchDef[setAddr].item[itemAddr].type     = HEX1_TYPE;
        SchDef[setAddr].item[itemAddr].length   = cnt*sizeof(char);
        for(i=0; i<cnt; i++) {
            memcpy(&SchDef[setAddr].item[itemAddr].ptn[(i*sizeof(char))], &cHex[0], sizeof(char));
        }
    } else if(2 == num) {
		if(0 == itemAddr) {
	        rc=sscanf(buff, "%d %d %s %s %d %s %d %d", &port, &addr, type, cdmy, &cnt, SchDef[setAddr].fname, &SchDef[setAddr].fnum, &SchDef[setAddr].randam);
			if(rc != 8) {
				MyMessage( hWnd, "[%s]  定義情報エラー！！", buff);
				MyMessage( hWnd, "アドレス=%d  TYPE=%s  定義情報エラー！！", addr, type);
				return FALSE;
			}
			if(0 == memcmp(cdmy, "0x", 2)){
				sscanf(&cdmy[2], "%4x", &sHex);
			} else {
				sscanf(&cdmy[0], "%4x", &sHex);
			}
		} else {
	        rc=sscanf(buff, "%s %d %s %s %d", dmy, &addr, type, cdmy, &cnt);
			if(rc != 5) {
				MyMessage( hWnd, "[%s]  定義情報エラー！！", buff);
				MyMessage( hWnd, "アドレス=%d  TYPE=%s  定義情報エラー！！", addr, type);
				return FALSE;
			}
			if(0 == memcmp(cdmy, "0x", 2)){
				sscanf(&cdmy[2], "%4x", &sHex);
			} else {
				sscanf(&cdmy[0], "%4x", &sHex);
			}
		}
		if(0 == itemAddr) {
	        SchDef[setAddr].port     = port;
		}
        SchDef[setAddr].item[itemAddr].addr     = addr;
        SchDef[setAddr].item[itemAddr].type     = HEX2_TYPE;
        SchDef[setAddr].item[itemAddr].length   = cnt*sizeof(short);
        for(i=0; i<cnt; i++) {
            memcpy(&SchDef[setAddr].item[itemAddr].ptn[(i*sizeof(short))], &sHex, sizeof(short));
        }
    } else if(4 == num) {
		if(0 == itemAddr) {
//			rc=sscanf(buff, "%d %d %s %8x %d %s %d %d", &port, &addr, type, &iHex, &cnt, SchDef[setAddr].fname, &SchDef[setAddr].fnum, &SchDef[setAddr].randam);
			rc=sscanf(buff, "%d %d %s %s %d %s %d %d", &port, &addr, type, cdmy, &cnt, SchDef[setAddr].fname, &SchDef[setAddr].fnum, &SchDef[setAddr].randam);
			if(rc != 8) {
				MyMessage( hWnd, "[%s]  定義情報エラー！！", buff);
				MyMessage( hWnd, "アドレス=%d  TYPE=%s  定義情報エラー！！", addr, type);
				return FALSE;
			}
			if(0 == memcmp(cdmy, "0x", 2)){
				sscanf(&cdmy[2], "%8x", &iHex);
			} else {
				sscanf(&cdmy[0], "%8x", &iHex);
			}
		} else {
	        rc=sscanf(buff, "%s %d %s %s %d", dmy, &addr, type, cdmy, &cnt);
			if(rc != 5) {
				MyMessage( hWnd, "[%s]  定義情報エラー！！", buff);
				MyMessage( hWnd, "アドレス=%d  TYPE=%s  定義情報エラー！！", addr, type);
				return FALSE;
			}
			if(0 == memcmp(cdmy, "0x", 2)){
				sscanf(&cdmy[2], "%8x", &iHex);
			} else {
				sscanf(&cdmy[0], "%8x", &iHex);
			}
		}

		if(0 == itemAddr) {
	        SchDef[setAddr].port     = port;
		}
        SchDef[setAddr].item[itemAddr].addr     = addr;
        SchDef[setAddr].item[itemAddr].type     = HEX4_TYPE;
        SchDef[setAddr].item[itemAddr].length   = cnt*sizeof(int);
        for(i=0; i<cnt; i++) {
            memcpy(&SchDef[setAddr].item[itemAddr].ptn[(i*sizeof(int))], &iHex, sizeof(int));
        }
    } else {
        return FALSE;
    }

    return TRUE;
}

BOOL XHexDataSetSch(HWND hWnd, int num, unsigned char* buff, int setAddr, int itemAddr)
{
    char    type[10];

//    char    cHex;
    short   sHex;
    int     iHex;
    int     addr;
    int     cnt;
    int     port;
    int     i;
	int     rc;
	char    dmy[32];
	char    cData[32];
	char    cWork[32];
	char    cdmy[32];

    if(1 == num) {
/* --->>
		if(0 == itemAddr) {
	        rc=sscanf(buff, "%d %d %s %2x %d %s %d %d", &port, &addr, type, &cHex, &cnt, SchDef[setAddr].fname, &SchDef[setAddr].fnum, &SchDef[setAddr].randam);
			if(rc != 8) {
				MyMessage( hWnd, "[%s]  定義情報エラー！！", buff);
				MyMessage( hWnd, "アドレス=%d  TYPE=%s  定義情報エラー！！", addr, type);
				return FALSE;
			}
		} else {
	        rc=sscanf(buff, "%s %d %s %2x %d", dmy, &addr, type, &cHex, &cnt);
			if(rc != 5) {
				MyMessage( hWnd, "[%s]  定義情報エラー！！", buff);
				MyMessage( hWnd, "アドレス=%d  TYPE=%s  定義情報エラー！！", addr, type);
				return FALSE;
			}
		}
		if(0 == itemAddr) {
	        SchDef[setAddr].port     = port;
		}
        SchDef[setAddr].item[itemAddr].addr     = addr;
        SchDef[setAddr].item[itemAddr].type     = HEX1_TYPE;
        SchDef[setAddr].item[itemAddr].length   = cnt*sizeof(char);
        for(i=0; i<cnt; i++) {
            memcpy(&SchDef[setAddr].item[itemAddr].ptn[(i*sizeof(char))], &cHex, sizeof(char));
        }
<<---- */
    } else if(2 == num) {
		if(0 == itemAddr) {
	        rc=sscanf(buff, "%d %d %s %s %d %s %d %d", &port, &addr, type, cdmy, &cnt, SchDef[setAddr].fname, &SchDef[setAddr].fnum, &SchDef[setAddr].randam);
			if(rc != 8) {
				MyMessage( hWnd, "[%s]  定義情報エラー！！", buff);
				MyMessage( hWnd, "アドレス=%d  TYPE=%s  定義情報エラー！！", addr, type);
				return FALSE;
			}
		} else {
	        rc=sscanf(buff, "%s %d %s %s %d", dmy, &addr, type, cdmy, &cnt);
			if(rc != 5) {
				MyMessage( hWnd, "[%s]  定義情報エラー！！", buff);
				MyMessage( hWnd, "アドレス=%d  TYPE=%s  定義情報エラー！！", addr, type);
				return FALSE;
			}
		}
		if(0 == memcmp(cdmy, "0x", 2)){
			sscanf(&cdmy[2], "%4x", &sHex);
		} else {
			sscanf(&cdmy[0], "%4x", &sHex);
		}

   		memcpy(cWork, &sHex, sizeof(short));
		cData[0] = cWork[1];
		cData[1] = cWork[0];
   		memcpy(&sHex, cData, sizeof(short));

		if(0 == itemAddr) {
	        SchDef[setAddr].port     = port;
		}
        SchDef[setAddr].item[itemAddr].addr     = addr;
        SchDef[setAddr].item[itemAddr].type     = XHEX2_TYPE;
        SchDef[setAddr].item[itemAddr].length   = cnt*sizeof(short);
        for(i=0; i<cnt; i++) {
            memcpy(&SchDef[setAddr].item[itemAddr].ptn[(i*sizeof(short))], &sHex, sizeof(short));
        }
    } else if(4 == num) {
		if(0 == itemAddr) {
			rc=sscanf(buff, "%d %d %s %s %d %s %d %d", &port, &addr, type, cdmy, &cnt, SchDef[setAddr].fname, &SchDef[setAddr].fnum, &SchDef[setAddr].randam);
			if(rc != 8) {
				MyMessage( hWnd, "[%s]  定義情報エラー！！", buff);
				MyMessage( hWnd, "アドレス=%d  TYPE=%s  定義情報エラー！！", addr, type);
				return FALSE;
			}
		} else {
	        rc=sscanf(buff, "%s %d %s %s %d", dmy, &addr, type, cdmy, &cnt);
			if(rc != 5) {
				MyMessage( hWnd, "[%s]  定義情報エラー！！", buff);
				MyMessage( hWnd, "アドレス=%d  TYPE=%s  定義情報エラー！！", addr, type);
				return FALSE;
			}
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
   		memcpy(&iHex, cData, sizeof(int));

		if(0 == itemAddr) {
	        SchDef[setAddr].port     = port;
		}
        SchDef[setAddr].item[itemAddr].addr     = addr;
        SchDef[setAddr].item[itemAddr].type     = XHEX4_TYPE;
        SchDef[setAddr].item[itemAddr].length   = cnt*sizeof(int);
        for(i=0; i<cnt; i++) {
            memcpy(&SchDef[setAddr].item[itemAddr].ptn[(i*sizeof(int))], &iHex, sizeof(int));
        }
    } else {
        return FALSE;
    }

    return TRUE;
}

/*-------------------------------------------------------------------
*
*   ＨＥＸデータ作成、バッファ格納
*
*--------------------------------------------------------------------*/
BOOL HexDataSetSch2(HWND hWnd, unsigned char* buff)
{
	int		port;
	char    wk1[32];
	char    wk2[32];
    short   sHex1;
    short   sHex2;
	int     rc;
	unsigned char	buff2[3];

    rc=sscanf(buff, "%d %s %s", &port, wk1, wk2);
	if(rc != 3) {
		MyMessage( hWnd, "レスポンス=%s サブヘッダ=%s　定義情報エラー！！", wk1, wk2);
		return FALSE;
	}
	if(0 == memcmp(wk1, "0x", 2)){
		sscanf(&wk1[2], "%4x", &sHex1);
	} else {
		sscanf(&wk1[0], "%4x", &sHex1);
	}
	if(0 == memcmp(wk2, "0x", 2)){
		sscanf(&wk2[2], "%4x", &sHex2);
	} else {
		sscanf(&wk2[0], "%4x", &sHex2);
	}

	// レスポンス定義
	memset(buff2, 0x00, sizeof(buff2));
	memcpy(buff2, &sHex1, sizeof(short));
	G_respons[0] = buff2[1];
	G_respons[1] = buff2[0];
	// サブヘッダ定義
	memset(buff2, 0x00, sizeof(buff2));
	memcpy(buff2, &sHex2, sizeof(short));
	G_hed[0] = buff2[1];
	G_hed[1] = buff2[0];

    return TRUE;
}

