#include    <conio.h>
#include    <stdio.h>
#include    <windows.h>
#include    <winsock.h>
#include	"AP_Func.h"
#include    "TcpCtrl.h"
#include	"SchDataSet.h"
#include	"MakeDataSet.h"
#include	"MakeCntSet.h"

extern unsigned char   g_pRecvBuf[2][COM_BUF_MAX];		// 受信ﾃﾞｰﾀ格納用
extern struct sockaddr_in  UdpDsAddr[3];               // 接続先アドレス (UDP)

//----------------------------------------------------------------------------
//
//----------------------------------------------------------------------------
BOOL  MakeCountDataSet(HWND hWnd, unsigned char* buff, int setAddr, int itemAddr)
{
    int     addr;
    char    type[10];
    char    dmy[32];

//  MyMessage(hWnd, "[%s]", buff);

//F    0000    CHAR     "00"          CHAR_LONG ADDR=0 LEN=5 REC=1 SET=1
//F    0000    CHAR     "00"          CHAR_CHAR ADDR=0 LEN=5 REC=1 SET=1
//F    0000    CHAR     "00"          INT                    REC=1 SET=1

    if(0 == itemAddr) {
        sscanf(buff, "%s %d %s", dmy, &addr, type);
    } else {
        sscanf(buff, "%s %d %s", dmy, &addr, type);
    }

//  MyMessage(hWnd, "[%d %d %s]", port, addr, type);

    if(0 == strcmp(type, "CHAR")) {
        CntCharDataSet(hWnd, buff, setAddr, itemAddr);
    }

#ifdef AAA
    if(0 == strcmp(type, "SHORT")) {
        ShortDataSetSch(hWnd, buff, setAddr, itemAddr);
    } else if(0 == strcmp(type, "LONG")) {
        LongDataSetSch(hWnd, buff, setAddr, itemAddr);
    } else if(0 == strcmp(type, "CHAR")) {
        CntCharDataSet(hWnd, buff, setAddr, itemAddr);
    } else if(0 == strcmp(type, "HEX1")) {
        HexDataSetSch(hWnd, 1, buff, setAddr, itemAddr);
    } else if(0 == strcmp(type, "HEX2")) {
        HexDataSetSch(hWnd, 2, buff, setAddr, itemAddr);
    } else if(0 == strcmp(type, "HEX4")) {
        HexDataSetSch(hWnd, 4, buff, setAddr, itemAddr);
    } else if(0 == strcmp(type, "EBCDIC")) {
        EbcdicDataSetSch(hWnd, buff, setAddr, itemAddr);
    } else if(0 == strcmp(type, "XSHORT")) {
        XShortDataSetSch(hWnd, buff, setAddr, itemAddr);
    } else if(0 == strcmp(type, "XLONG")) {
        XLongDataSetSch(hWnd, buff, setAddr, itemAddr);
    } else if(0 == strcmp(type, "XHEX2")) {
        XHexDataSetSch(hWnd, 2, buff, setAddr, itemAddr);
    } else if(0 == strcmp(type, "XHEX4")) {
        XHexDataSetSch(hWnd, 4, buff, setAddr, itemAddr);
    }
#endif
    return TRUE;
}


int GetItemInt(unsigned char* buff, unsigned char* item, int* num)
{
    unsigned int     len, i;
    int     ok;
    int     addr;

    ok = 0;
    len = strlen(item);

    for(i=0; i<strlen(buff)-len; i++) {
        if(0 == memcmp(buff+i, item, len)) {
            ok = 1;
            break;
        }
    }
    if(0 == ok) {
        return 1;
    }

    addr = i + len;
    sscanf(buff+addr, "%d", num);

    return 0;
}

int GetItemChar(unsigned char* buff, unsigned char* item, char* str)
{
    unsigned int     len, i, set, sw;
    int     ok;
    int     addr;
    char    cData[512];

    ok = 0;
    len = strlen(item);

    for(i=0; i<strlen(buff)-len; i++) {
        if(0 == memcmp(buff+i, item, len)) {
            ok = 1;
            break;
        }
    }
    if(0 == ok) {
        return 1;
    }

    addr = i + len;

    memset(cData, 0x00, sizeof(cData));
    set = 0;
    sw  = 1;
    for(i=0; i<512 || 3==sw; i++) {
        switch(sw) {
        case 1:
            if('\"' == *(buff+addr+i)) {
                sw = 2;
            }
            break;
        case 2:
            if('\"' == *(buff+addr+i)) {
                sw = 3;
            } else {
                cData[set] = *(buff+addr+i);
                set++;
            }
            break;
        }
        if(3 == sw) {
            break;
        }
    }
    memcpy(str, cData, strlen(cData));

    return 0;
}


char* GetTypeText(int no)
{
    static char ctype[][7] = {"SHORT",
		                      "LONG",
							  "CHAR",
							  "HEX1",
							  "HEX2",
							  "HEX4",
							  "EBCDIC",
							  "----",
							  "----",
							  "----",
							  "XSHORT",
							  "XLONG",
							  "----",
							  "XHEX1",
							  "XHEX2",
							  "XHEX4"
    };
    return(ctype[no-1]);
}

/*-------------------------------------------------------------------
*
*   ＣＨＡＲデータから作成、バッファ格納
*
*--------------------------------------------------------------------*/
BOOL CntCharDataSet(HWND hWnd, unsigned char* buff, int setAddr, int itemAddr)
{
    char    type[10];

    char    cData[1024];
    char    dmy[1024];
    char    dmy1[1024];
    char    stype[32];
    char    ctext[1024];
    int     addr;
    int     i;
    int     set;
    int     sw;
    int     saddr, slen, srec, sset, sdata;

    srec=0;
    sset=0;
    saddr=0;
    slen=0;
    sdata=0;
    memset(ctext, 0x00, sizeof(ctext));

    if(0 == itemAddr) {
//F    0000    CHAR     "00"     CHAR_LONG ADDR=0 LEN=5 REC=1 SET=1
//F    0000    CHAR     "00"     CHAR_CHAR ADDR=0 LEN=5 REC=1
//F    0000    CHAR     "00"     LONG_SET  DATA=12345   REC=1 SET=0
//F    0000    CHAR     "00"     CHAR_SET  DATA="    "  REC=1
//F    0000    CHAR     "00"     LONG_LONG   ADDR=0     REC=1 SET=1
//F    0000    CHAR     "00"     CNT_ON    REC=1
//     CHAR_CHAR 指定時は SET=? は未使用です。
        sscanf(buff, "%s %d %s %s %s", dmy1, &addr, type, dmy, stype);
        GetItemInt(buff, "ADDR=", &saddr);
        GetItemInt(buff, "LEN=",  &slen);
        GetItemInt(buff, "REC=",  &srec);
        GetItemInt(buff, "SET=",  &sset);
        if ( 0 == memcmp(stype, "LONG_SET", 8) ) {
            GetItemInt(buff, "DATA=", &sdata);
        }
        if ( 0 == memcmp(stype, "CHAR_SET", 8) ) {
            GetItemChar(buff, "DATA=", ctext);  
        }

    } else {
       if(CntDef[setAddr].type == 0) {
            return FALSE;
       }            
       sscanf(buff, "%s %d %s %s", dmy1, &addr, type, dmy);
    }
    if(0 == itemAddr) {
        if(1 > srec || 100 <srec) {
            MyMessage(hWnd, " ");
            MyMessage(hWnd, "ｶｳﾝﾄ定義 ｾｯﾄﾃﾞｰﾀ異常 ﾚｺｰﾄﾞNo.異常=%d", srec);
            return FALSE;
        }
        if ( 0 != memcmp(stype, "CNT_ON", 6) ) {

            if(0 > sset || MAX_TARGET < sset) {
                MyMessage(hWnd, " ");
                MyMessage(hWnd, "ｶｳﾝﾄ定義 ｾｯﾄﾃﾞｰﾀ異常 ｾｯﾄNo.異常=%d", sset);
                return FALSE;
            }
        }
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
        CntDef[setAddr].type = CHAR_TYPE;
        CntDef[setAddr].addr = saddr;
        CntDef[setAddr].len  = slen;
        CntDef[setAddr].rec  = srec;
        CntDef[setAddr].set  = sset;
    }
    CntDef[setAddr].item[itemAddr].addr     = addr;
    CntDef[setAddr].item[itemAddr].type     = CHAR_TYPE;
    CntDef[setAddr].item[itemAddr].length   = set;
    memcpy(CntDef[setAddr].item[itemAddr].ptn, cData, set);

    if(itemAddr == 0 ) {
        if ( 0 == memcmp(stype, "CHAR_CHAR", 9) ) {
            CntDef[setAddr].kind = CNT_CHAR_CHAR;
        } else if ( 0 == memcmp(stype, "LONG_SET", 8) ) {
            CntDef[setAddr].kind = CNT_LONG_SET;
           CntDef[setAddr].addr = sdata;
        } else if ( 0 == memcmp(stype, "CNT_ON", 6) ) {
            CntDef[setAddr].kind = CNT_CNT_ON;
        } else if ( 0 == memcmp(stype, "LONG_LONG", 9) ) {
            CntDef[setAddr].kind = CNT_LONG_LONG;
            CntDef[setAddr].len  = sizeof(int);
        } else if ( 0 == memcmp(stype, "CHAR_LONG", 9) ) {
            CntDef[setAddr].kind = CNT_CHAR_LONG;
        } else if ( 0 == memcmp(stype, "CHAR_SET", 8) ) {
            CntDef[setAddr].kind = CNT_CHAR_SET;
            strcpy( CntDef[setAddr].ctext, ctext );
            CntDef[setAddr].ctext[strlen(ctext)] = 0x00;
            CntDef[setAddr].len = strlen(ctext);
        }

        if ( CNT_LONG_SET == CntDef[setAddr].kind ) {
            MyMessage(hWnd, " ");
            MyMessage(hWnd, "ｶｳﾝﾄ定義 目標値ｾｯﾄ ﾀｲﾌﾟ=%s  SETﾃﾞｰﾀ=%d  ﾚｺｰﾄﾞNo=%d  ｾｯﾄNo=%d (LONG_SET)",
                GetTypeText(CntDef[setAddr].type),
                CntDef[setAddr].addr, 
                CntDef[setAddr].rec, CntDef[setAddr].set);
        } else if( CNT_CNT_ON == CntDef[setAddr].kind ) {
            MyMessage(hWnd, " ");
            MyMessage(hWnd, "ｶｳﾝﾄ定義 CNT開始   ﾀｲﾌﾟ=%s  ﾚｺｰﾄﾞNo=%d  (CNT_ON)",
                GetTypeText(CntDef[setAddr].type),
                CntDef[setAddr].rec);
        } else if( CNT_CHAR_CHAR == CntDef[setAddr].kind ) {
            MyMessage(hWnd, " ");
            MyMessage(hWnd, "ｶｳﾝﾄ定義 目標値ｾｯﾄ ﾀｲﾌﾟ=%s  ｱﾄﾞﾚｽ=%d  ﾃﾞｰﾀ長=%d  ﾚｺｰﾄﾞNo=%d  ｾｯﾄNo=%d (CHAR_CHAR)",
                GetTypeText(CntDef[setAddr].type),
                CntDef[setAddr].addr, CntDef[setAddr].len,
                CntDef[setAddr].rec, CntDef[setAddr].set);
        } else if( CNT_LONG_LONG == CntDef[setAddr].kind ) {
            MyMessage(hWnd, " ");
            MyMessage(hWnd, "ｶｳﾝﾄ定義 目標値ｾｯﾄ ﾀｲﾌﾟ=%s  ｱﾄﾞﾚｽ=%d  ﾃﾞｰﾀ長=%d  ﾚｺｰﾄﾞNo=%d  ｾｯﾄNo=%d (LONG_LONG)",
                GetTypeText(CntDef[setAddr].type),
                CntDef[setAddr].addr, CntDef[setAddr].len,
                CntDef[setAddr].rec, CntDef[setAddr].set);
        } else if( CNT_CHAR_LONG == CntDef[setAddr].kind ) {
            MyMessage(hWnd, " ");
            MyMessage(hWnd, "ｶｳﾝﾄ定義 目標値ｾｯﾄ ﾀｲﾌﾟ=%s  ｱﾄﾞﾚｽ=%d  ﾃﾞｰﾀ長=%d  ﾚｺｰﾄﾞNo=%d  ｾｯﾄNo=%d (CHAR_LONG)",
                GetTypeText(CntDef[setAddr].type),
                CntDef[setAddr].addr, CntDef[setAddr].len,
                CntDef[setAddr].rec, CntDef[setAddr].set);
        } else if( CNT_CHAR_SET == CntDef[setAddr].kind ) {
            MyMessage(hWnd, " ");
            MyMessage(hWnd, "ｶｳﾝﾄ定義 CHARｾｯﾄ ﾀｲﾌﾟ=%s  SETﾃﾞｰﾀ=%s  ﾃﾞｰﾀ長=%d  ﾚｺｰﾄﾞNo=%d  (CHAR_SET)",
                GetTypeText(CntDef[setAddr].type),
                CntDef[setAddr].ctext, CntDef[setAddr].len, 
                CntDef[setAddr].rec);
        }
    }
    MyMessage(hWnd, "         ｱﾄﾞﾚｽ=%d  ﾀｲﾌﾟ=%s  検索ﾊﾟﾀｰﾝ=[%s]  ﾃﾞｰﾀ長=%d",
        CntDef[setAddr].item[itemAddr].addr,
        GetTypeText(CntDef[setAddr].item[itemAddr].type),
        CntDef[setAddr].item[itemAddr].ptn,
        CntDef[setAddr].item[itemAddr].length);

    return TRUE;
}


int CntDefSub(int tblno, int addr, int chkAddr1)
{
    int     offset;
    int     chkAddr;

    // 伝文ﾀｲﾌﾟからｵﾌｾｯﾄ取り出し
    offset = getOffsetByComType();

    if(2 <= CntDef[addr].num) {
        chkAddr = CntDef[addr].item[1].addr + offset;
        if(0 != memcmp(CntDef[addr].item[1].ptn, &g_pRecvBuf[tblno][chkAddr], CntDef[addr].item[1].length)) {
            return -1;
        }
    }
    if(3 <= CntDef[addr].num) {
        chkAddr = CntDef[addr].item[2].addr + offset;
        if(0 != memcmp(CntDef[addr].item[2].ptn, &g_pRecvBuf[tblno][chkAddr], CntDef[addr].item[2].length)) {
            return -1;
        }
    }
    if(4 <= CntDef[addr].num) {
        chkAddr = CntDef[addr].item[3].addr + offset;
        if(0 != memcmp(CntDef[addr].item[3].ptn, &g_pRecvBuf[tblno][chkAddr], CntDef[addr].item[3].length)) {
            return -1;
        }
    }
    if(5 <= CntDef[addr].num) {
        chkAddr = CntDef[addr].item[4].addr + offset;
        if(0 != memcmp(CntDef[addr].item[4].ptn, &g_pRecvBuf[tblno][chkAddr], CntDef[addr].item[4].length)) {
            return -1;
        }
    }

    return 0;
}

int CntFilePut(HWND hWnd, int tblno, int addr)
{
    HANDLE  g_hSemaphore;
    int     result;
    struct  _stat   buf;
    int     recno, nfp, idata;
    char    wk[128];
    CountFile fdata;
    char    semaname[128];
    int     offset;
    int     send_data[16];
    int     nsent, nerror;
    char    cpszmsg[128];

    // 伝文ﾀｲﾌﾟからｵﾌｾｯﾄ取り出し
    offset = getOffsetByComType();

//    MyMessage(hWnd, "CntDef[%d].rec=%d", addr, CntDef[addr].rec);
 
    recno = CntDef[addr].rec - 1;
    if(0 > recno) {
        return FALSE;
    }

//    MyMessage(hWnd, "CHAR_TYPE=%d  tyep=%d set=%d len=%d", CHAR_TYPE, CntDef[addr].type, CntDef[addr].set, CntDef[addr].len);
    if(CHAR_TYPE == CntDef[addr].type) {
        if( CNT_CHAR_CHAR == CntDef[addr].kind ) {
            /* 文字列データをセット CHAR_CHAR */
            memset(wk, 0x00, sizeof(wk));
            memcpy(wk, &g_pRecvBuf[tblno][CntDef[addr].addr+offset], CntDef[addr].len);
//     MyMessage(hWnd, "addr==%d  len=%d", CntDef[addr].addr, CntDef[addr].len);
        } else if( CNT_CHAR_SET == CntDef[addr].kind ) {
            /* 文字列データをセット CHAR_SET */
            memset(wk, 0x00, sizeof(wk));
            memcpy(wk, CntDef[addr].ctext, CntDef[addr].len);
//     MyMessage(hWnd, "addr==%d  len=%d", CntDef[addr].addr, CntDef[addr].len);
        } else if ( CNT_CNT_ON == CntDef[addr].kind ) {
            memset(send_data, 0x00, sizeof(send_data));
            send_data[0] = 1;                           // 処理区分 1:CountUpﾁｪｯｸON 
            send_data[1] = CntDef[addr].rec;            // ﾚｺｰﾄﾞNo
            // UDP
            nsent = sendto( g_sSock[PORT3-1], (char*)send_data, sizeof(int)*16, 0,
                (struct sockaddr *)&UdpDsAddr[PORT3-1],   // 送信先ソケットのアドレスへのポインタ
                 sizeof(struct sockaddr_in));             // 送信先ソケットのアドレスのサイズ
            if ( nsent == SOCKET_ERROR ) {
                nerror = WSAGetLastError( );
                if ( nerror != WSAEWOULDBLOCK ) {
                    memset( cpszmsg, '\0', sizeof(cpszmsg) );
                    wsprintf( cpszmsg, "PORT %d  送信エラー　Error No. %d",PORT3, nerror );
                    MyMessage( hWnd, cpszmsg );
                }
                return 0;
            }
            MyMessage(hWnd, "ｶｳﾝﾀﾌｧｲﾙ　ｶｳﾝﾄｱｯﾌﾟ指令送信  ﾚｺｰﾄﾞNo=%d", CntDef[addr].rec);
            return 0;
        } else if ( CNT_LONG_SET == CntDef[addr].kind ) {
            /* 整数データをセット INT */
            idata = CntDef[addr].addr;
  //   MyMessage(hWnd, "addr==%d [%s] ", CntDef[addr].addr, __FILE__);

        } else if ( CNT_LONG_LONG == CntDef[addr].kind ) {
            memcpy(&idata, &g_pRecvBuf[tblno][CntDef[addr].addr+offset], CntDef[addr].len);
        } else {
            /* 文字列->整数データをセット  */
            memset(wk, 0x00, sizeof(wk));
            memcpy(wk, &g_pRecvBuf[tblno][CntDef[addr].addr+offset], CntDef[addr].len);
            idata = atoi(wk);
        }
    } else {
        return 0;
    }

    memset(semaname, 0x00, sizeof(semaname));
    makeSemaName(hWnd, semaname);
 
    /* セマフォオブジェクトの作成 */
    g_hSemaphore = CreateSemaphore(NULL, 1, 1, semaname);

    /* セマフォオブジェクトの取得 */
    result = WaitForSingleObject(g_hSemaphore, 5000);
    if(result == WAIT_OBJECT_0) {
        if(_stat(tcpCtrl.counterFile, &buf) < 0) {
            // ﾌｧｲﾙが存在しない場合
            return 0;
        }
        nfp = AP_Sfopen(tcpCtrl.counterFile, AP_RDWR);
        if (nfp < 0) {
            MyMessage( hWnd, "tcpsp counterﾌｧｲﾙ読み込み異常");
            return FALSE;
        }
        AP_Sgrec(nfp, sizeof(CountFile), recno, &fdata);

        if( CNT_CHAR_CHAR == CntDef[addr].kind || CNT_CHAR_SET == CntDef[addr].kind ) {
            fdata.kubun = 1;
            memcpy(&fdata.idata, wk, CntDef[addr].len+1);
            fdata.count = 9999999;
        } else {
            if( CntDef[addr].set == 0 ) {
                fdata.idata = idata;
            } else {
                fdata.target[CntDef[addr].set-1] = idata;
            }
        }

        AP_Sprec(nfp, sizeof(CountFile), recno, &fdata);
        AP_Sfclose(nfp);

        /* セマフォオブジェクトの解放 */
        ReleaseSemaphore(g_hSemaphore, 1, NULL);
    }
    /* セマフォオブジェクトの破棄 */
    CloseHandle(g_hSemaphore);

    MyMessage(hWnd, " ");
    if( CNT_CHAR_LONG == CntDef[addr].kind  ) {
        MyMessage(hWnd, "ｶｳﾝﾀﾌｧｲﾙ更新  ﾚｺｰﾄﾞNo=%d  ｾｯﾄNo=%d  ﾃﾞｰﾀ=[%d]  [%s]",
                 CntDef[addr].rec, CntDef[addr].set, idata, semaname);
    } else if( CNT_LONG_SET == CntDef[addr].kind ) {
        MyMessage(hWnd, "ｶｳﾝﾀﾌｧｲﾙ更新  ﾚｺｰﾄﾞNo=%d  ｾｯﾄNo=%d  整数ﾃﾞｰﾀ=[%d]  [%s]",
                 CntDef[addr].rec, CntDef[addr].set, idata, semaname);
    } else if( CNT_CHAR_CHAR == CntDef[addr].kind ) {
        MyMessage(hWnd, "ｶｳﾝﾀﾌｧｲﾙ更新  ﾚｺｰﾄﾞNo=%d  ｾｯﾄﾃﾞｰﾀ=%s  [%s]",
                 CntDef[addr].rec, wk, semaname);
    } else if( CNT_LONG_LONG == CntDef[addr].kind ) {
        MyMessage(hWnd, "ｶｳﾝﾀﾌｧｲﾙ更新  ﾚｺｰﾄﾞNo=%d  ｾｯﾄNo=%d  整数ﾃﾞｰﾀ=[%d]  [%s]",
                 CntDef[addr].rec, CntDef[addr].set, idata, semaname);
    } else if( CNT_CHAR_SET == CntDef[addr].kind ) {
        MyMessage(hWnd, "ｶｳﾝﾀﾌｧｲﾙ更新  ﾚｺｰﾄﾞNo=%d  ﾃﾞｰﾀ=[%s]  [%s]",
                 CntDef[addr].rec, CntDef[addr].ctext, semaname);
    }
    return 0;
}

void makeSemaName(HWND hWnd, char* semaname)
{
	int		i;
	int		len;
	int     move_size;
	char    name[128];

	memset(name, 0x00, sizeof(name));
	move_size = 0;

    if(NULL == semaname) {
		return;
	}

	len = strlen(tcpCtrl.counterFile);
    for(i=len; i>=0; i--) {
		if('\\' == tcpCtrl.counterFile[i] || '/' == tcpCtrl.counterFile[i]) {
			break;
		} else {
			move_size++;
		}
	}
    memcpy(semaname, &tcpCtrl.counterFile[i+1], move_size);
	return;
}



