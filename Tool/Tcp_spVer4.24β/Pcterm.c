/*=============================================================================
*  rev.001 2009.10.22 松岡 奇数バイトの伝文送信不可の不具合対応
*
*
*
*
*============================================================================*/
#include    <stdio.h>
#include    "TcpCtrl.h"
#include    "pcterm.h"

/*-------------------------------------------------------------------
*
*   PCTERM用送信ﾃﾞｰﾀ作成
*
*--------------------------------------------------------------------*/
int makePcterm(HWND hWnd, unsigned char* g_pSendBuf, unsigned char* g_pSendDmy, int g_nTotalToSend, unsigned char g_pcterm_pa)
{
    short       length;
    short       lno;
	char		sbuf[2];
	char		sbuf2[2];
	char		dbuf;
	int			rc;
	int         amari;                  // rev.001


    // FCH部設定
    g_pSendBuf[0] = 0xA0;               // HID
    g_pSendBuf[1] = 0xB1;               // SID
                                        // Data Length
    length = g_nTotalToSend + 12;
	memcpy(sbuf, &length, sizeof(short));
	dbuf = sbuf[0];
	sbuf[0] = sbuf[1];
	sbuf[1] = dbuf;
    memcpy(&g_pSendBuf[2], &sbuf[0], sizeof(short));

    g_pSendBuf[4] = 0x00;               // 処理区分
    g_pSendBuf[5] = 0x00;               // 処理ﾚﾍﾞﾙ
                                        // AP LINK NO
    lno = 1;
	memcpy(sbuf2, &lno, sizeof(short));
	dbuf = sbuf2[0];
	sbuf2[0] = sbuf2[1];
	sbuf2[1] = dbuf;
    memcpy(&g_pSendBuf[6], sbuf2, sizeof(short));

    g_pSendBuf[8] = 0x00;               // HA
    g_pSendBuf[9] = g_pcterm_pa;        // PA
    g_pSendBuf[10] = 0x00;              // Job No.
    g_pSendBuf[11] = 0x02;              // ID区分
    g_pSendBuf[12] = 0x02;              // SUB
    g_pSendBuf[13] = 0x00;              // COMMAND
    g_pSendBuf[14] = 0x00;              // 完了ﾀｲﾏ値
    g_pSendBuf[15] = 0x00;              // CONTROL
    memcpy(&g_pSendBuf[16], g_pSendDmy, g_nTotalToSend);
/* rev.001 start */
	amari = g_nTotalToSend % 2;
	if ( 0 != amari ) {
		g_pSendBuf[16 + g_nTotalToSend] = 0x00;
	}
/* rev.001 end   */
    rc = checksum(hWnd, 0, g_pSendBuf); // CHECK SUM作成

	return (length + 6 + amari);
}

/********************************************************/
/* ST-NET Checksum subroutine                           */
/* for INTEL-type Processor                             */
/*                                                      */
/*      type : 0   checksum Set                         */
/*           : !0  checksum Verify                      */
/*      fch  : FCH pointer                              */
/*                                                      */
/*    on exit                                           */
/*       0 : complete                                   */
/*       1 : checksum error                             */
/*       2 : FCH length error                           */
/********************************************************/
int checksum( HWND hWnd, int type, char *fch )
{
    register unsigned short sum = 0;
    unsigned short          total_len;
    unsigned short          loop_cnt;
    unsigned short          *sum_ptr;
    register unsigned short *datap;

/*********************************/
/* get data length               */
/*********************************/

    datap = (unsigned short *)&fch[FCH_LENP];
    total_len = GETLEN(*datap);

    if ((total_len<STNET_MIN) || (total_len>STNET_MAX))
        return( (int)RC_LENERR );

    total_len += 2;

    if ( total_len & 0x0001 )
     {
        if ( !type )
        {
            fch[total_len+FCH_LENP] = PAD_CHAR;
        }
        total_len++;
    }

    sum_ptr = (unsigned short *)&fch[total_len+FCH_LENP];

/*********************************/
/* compute checksum              */
/*********************************/

    for( loop_cnt=(total_len >> 1) ; loop_cnt ; loop_cnt-- )
    {
        sum += GETDATA( *datap );
       datap++;
    }

/*********************************/
/* set or verify checksum        */
/*********************************/

    if ( !type )
    {
		*sum_ptr = SETSUM( sum );
        return( (int)RC_OK );
    }
    else
    {
    if ( sum == GETDATA( *sum_ptr ) )
        return( (int)RC_OK );
    else
        return( (int)RC_SUMERR );
}
/********** not reached **********/
}

