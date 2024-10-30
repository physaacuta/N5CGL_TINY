/*=============================================================================
*
*
*
*
*
*============================================================================*/
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <dos.h>
#include <time.h>
#include <sys\timeb.h>

#include "resource.h"

#include "dump.h"

#define		MAX_LENGTH	10240							// cDump用 最大出力数


/*-----------------------------------------------------------------------------
*	cDump, wkDump ﾍｯﾀﾞｰ部 ﾌｫｰﾏｯﾄ
*----------------------------------------------------------------------------*/
static	char	cDumpFmt[] =
{" << Charcter Data Dump List >> [%04d/%02d/%02d %02d:%02d:%02d.%03d] <%s>\n\n"};

static	char	wkDumpFmt[] =
//{" <<  Work Data Dump List  >>    [%04d/%02d/%02d %02d:%02d:%02d]  <%s>"};
//{" <<  Trace Data  >>           [%04d/%02d/%02d %02d:%02d:%02d.%03d] <%s>"};
  {" <<  Trace Data  >>  [%04d/%02d/%02d %02d:%02d:%02d.%03d] len=%05d <%s>"};

#ifdef XXX
/*-----------------------------------------------------------------------------
*	cDump, wkDump ｱﾄﾞﾚｽ部 ﾌｫｰﾏｯﾄ
*----------------------------------------------------------------------------*/
static  char    FixFmt[3][100] = {
{" ADDR :   0  1  2  3  4  5  6  7 -  8  9  A  B  C  D  E  F  [    ASCII     ]" },
{" ADDR :   0  1  2  3  4  5  6  7 -  8  9  A  B  C  D  E  F  [    S-JIS     ]" },
{" ADDR :   0  1  2  3  4  5  6  7 -  8  9  A  B  C  D  E  F  [    EBCDIC    ]" }};

/*-----------------------------------------------------------------------------
*	cDump, wkDump ﾀﾞﾝﾌﾟ部 ﾌｫｰﾏｯﾄ
*----------------------------------------------------------------------------*/
static  char    DumpFmt[] =
//{" %04X :  %02X %02X %02X %02X %02X %02X %02X %02X - %02X %02X %02X %02X %02X %02X %02X %02X  %c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c" };
{" %04X :  %02X %02X %02X %02X %02X %02X %02X %02X - %02X %02X %02X %02X %02X %02X %02X %02X  %c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c" };


static  char    DumpFmt1[16][128] =
{" %04X :  %02X %02X %02X %02X %02X %02X %02X %02X - %02X %02X %02X %02X %02X %02X %02X %02X  %c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c",
 " %04X :  %02X %02X %02X %02X %02X %02X %02X %02X - %02X %02X %02X %02X %02X %02X %02X     %c%c%c%c%c%c%c%c%c%c%c%c%c%c%c" ,
 " %04X :  %02X %02X %02X %02X %02X %02X %02X %02X - %02X %02X %02X %02X %02X %02X        %c%c%c%c%c%c%c%c%c%c%c%c%c%c",
 " %04X :  %02X %02X %02X %02X %02X %02X %02X %02X - %02X %02X %02X %02X %02X           %c%c%c%c%c%c%c%c%c%c%c%c%c",
 " %04X :  %02X %02X %02X %02X %02X %02X %02X %02X - %02X %02X %02X %02X              %c%c%c%c%c%c%c%c%c%c%c%c",
 " %04X :  %02X %02X %02X %02X %02X %02X %02X %02X - %02X %02X %02X                 %c%c%c%c%c%c%c%c%c%c%c",
 " %04X :  %02X %02X %02X %02X %02X %02X %02X %02X - %02X %02X                    %c%c%c%c%c%c%c%c%c%c",
 " %04X :  %02X %02X %02X %02X %02X %02X %02X %02X - %02X                       %c%c%c%c%c%c%c%c%c",
 " %04X :  %02X %02X %02X %02X %02X %02X %02X %02X -                          %c%c%c%c%c%c%c%c",
 " %04X :  %02X %02X %02X %02X %02X %02X %02X                               %c%c%c%c%c%c%c",
 " %04X :  %02X %02X %02X %02X %02X %02X                                  %c%c%c%c%c%c",
 " %04X :  %02X %02X %02X %02X %02X                                     %c%c%c%c%c",
 " %04X :  %02X %02X %02X %02X                                        %c%c%c%c",
 " %04X :  %02X %02X %02X                                           %c%c%c",
 " %04X :  %02X %02X                                              %c%c",
 " %04X :  %02X                                                 %c" };
#endif

/*-----------------------------------------------------------------------------
*	cDump, wkDump ｱﾄﾞﾚｽ部 ﾌｫｰﾏｯﾄ
*----------------------------------------------------------------------------*/
static  char    FixFmt[3][128] = {
{" ADDR (DEC) :   0  1  2  3  4  5  6  7 -  8  9  A  B  C  D  E  F  [    ASCII     ]" },
{" ADDR (DEC) :   0  1  2  3  4  5  6  7 -  8  9  A  B  C  D  E  F  [    S-JIS     ]" },
{" ADDR (DEC) :   0  1  2  3  4  5  6  7 -  8  9  A  B  C  D  E  F  [    EBCDIC    ]" }};

/*-----------------------------------------------------------------------------
*	cDump, wkDump ﾀﾞﾝﾌﾟ部 ﾌｫｰﾏｯﾄ
*----------------------------------------------------------------------------*/
static  char    DumpFmt[] =
//{" %04X(%04d) :  %02X %02X %02X %02X %02X %02X %02X %02X - %02X %02X %02X %02X %02X %02X %02X %02X  %c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c" };
{" %04X(%04d) :  %02X %02X %02X %02X %02X %02X %02X %02X - %02X %02X %02X %02X %02X %02X %02X %02X  %c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c" };


static  char    DumpFmt1[16][150] =
{" %04X(%04d) :  %02X %02X %02X %02X %02X %02X %02X %02X - %02X %02X %02X %02X %02X %02X %02X %02X  %c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c",
 " %04X(%04d) :  %02X %02X %02X %02X %02X %02X %02X %02X - %02X %02X %02X %02X %02X %02X %02X     %c%c%c%c%c%c%c%c%c%c%c%c%c%c%c" ,
 " %04X(%04d) :  %02X %02X %02X %02X %02X %02X %02X %02X - %02X %02X %02X %02X %02X %02X        %c%c%c%c%c%c%c%c%c%c%c%c%c%c",
 " %04X(%04d) :  %02X %02X %02X %02X %02X %02X %02X %02X - %02X %02X %02X %02X %02X           %c%c%c%c%c%c%c%c%c%c%c%c%c",
 " %04X(%04d) :  %02X %02X %02X %02X %02X %02X %02X %02X - %02X %02X %02X %02X              %c%c%c%c%c%c%c%c%c%c%c%c",
 " %04X(%04d) :  %02X %02X %02X %02X %02X %02X %02X %02X - %02X %02X %02X                 %c%c%c%c%c%c%c%c%c%c%c",
 " %04X(%04d) :  %02X %02X %02X %02X %02X %02X %02X %02X - %02X %02X                    %c%c%c%c%c%c%c%c%c%c",
 " %04X(%04d) :  %02X %02X %02X %02X %02X %02X %02X %02X - %02X                       %c%c%c%c%c%c%c%c%c",
 " %04X(%04d) :  %02X %02X %02X %02X %02X %02X %02X %02X -                          %c%c%c%c%c%c%c%c",
 " %04X(%04d) :  %02X %02X %02X %02X %02X %02X %02X                               %c%c%c%c%c%c%c",
 " %04X(%04d) :  %02X %02X %02X %02X %02X %02X                                  %c%c%c%c%c%c",
 " %04X(%04d) :  %02X %02X %02X %02X %02X                                     %c%c%c%c%c",
 " %04X(%04d) :  %02X %02X %02X %02X                                        %c%c%c%c",
 " %04X(%04d) :  %02X %02X %02X                                           %c%c%c",
 " %04X(%04d) :  %02X %02X                                              %c%c",
 " %04X(%04d) :  %02X                                                 %c" };

/*-----------------------------------------------------------------------------
*	cDump, wkDump 関数宣言
*----------------------------------------------------------------------------*/
static	void	DumpDataPrint(int DispOut, FILE *fp, BYTE *Data, int divC, int dataLen);
static	int		DumpDataWkSet(BYTE *Data, BYTE *DataWk, int dataLen, int ib);
static	void	sJisDataSet(BYTE *Data, BYTE *cData, int dataLen, int ib);
static	void	AsciiDataSet(BYTE *Data, BYTE *cData, int dataLen, int ib);
static	void	EbcdicDataSet(BYTE *Data, BYTE *cData, int dataLen, int ib);
static	void	toEbcdicChar(BYTE data, BYTE *EbcChar);
static	void	DumpPrintf(int DispOut, FILE *fp, const char *__format, ... );
static	FILE*	openDevName(BYTE *DevName);

static	void	sgtim(int *date);

static	HWND	dp_hWnd;
static	int		dpListID;

/******************************************************************************
*	Dataの内容を eCode まで指定ﾃﾞﾊﾞｲｽに出力する
*==============================================================================
* Prototype
*	void	cDump (BYTE *Data, int DispOut, BYTE *DevName, int Div,
*													BYTE eCode, BYTE *Msg);
*
* Parameter
*	Data	: Dump Data
*	DispOut	: _DISP_ON  = 画面出力有り
*	          _DISP_OFF = 画面出力無し
*	DevName	: NULL      = ﾃﾞﾊﾞｲｽ出力無し
*			  NULL     != DevNameへ出力
*	Div		: _ASCII    = ASCII CODE
*			  _S_JIS    = S-JIS CODE
*			  _EBCDIC   = EBCDIC CODE
*	eCode	: Dump End Code
*	Msg		: ﾒｯｾｰｼﾞ
*
* Return code
*   None
*
******************************************************************************/
void	cDump( BYTE *Data, int DispOut, BYTE *DevName, int Div, BYTE eCode, BYTE *Msg )
{
//  BORLANDC 用 start ===>>>
//	struct	time	Ntm;
//	struct	date	Ndt;
//  end <<<===
	int		date[7];

	int		i, dataLen;
	int		divC;
	FILE	*fp;

	if(Data == NULL)
		return;

	if(DevName != NULL) {
		fp = openDevName(DevName);
	} else {
		fp = NULL;
	}

//  BORLANDC 用 start ===>>>
//	gettime( &Ntm );
//	getdate( &Ndt );

//	DumpPrintf(DispOut, fp, cDumpFmt,
//				(int)Ndt.da_year, (int)Ndt.da_mon, (int)Ndt.da_day,
//				(int)Ntm.ti_hour, (int)Ntm.ti_min, (int)Ntm.ti_sec, Msg);
//  BORLANDC 用 end  <<<===

    sgtim( date );	                /* OUTPUT TIME                */
	DumpPrintf(DispOut, fp, cDumpFmt,
				date[0], date[1], date[2], date[3], date[4], date[5], date[6], Msg);

	divC = Div;
	if(!(divC == _ASCII || divC == _S_JIS || divC == _EBCDIC))
		divC = _ASCII;

	DumpPrintf(DispOut, fp, "%s", FixFmt[divC]);

	for(i=0; i<MAX_LENGTH; i++) {
		if(*(Data+i) == eCode)	break;
	}
	dataLen = i + 1;
	DumpDataPrint(DispOut, fp, Data, divC, dataLen);

	if(fp != NULL)
		fclose(fp);

	return;
}


/******************************************************************************
*	Dataの内容をLen byteまで指定ﾃﾞﾊﾞｲｽに出力する
*==============================================================================
* Prototype
*	void	wkDump(HWND hWnd, BYTE *Data, int DispOut, BYTE *DevName, int Div,
*													 int Len, BYTE *Msg)
*
* Parameter
*	HWND 	: hWnd
*	Data	: Dump Data
*	DispOut	: _DISP_ON  = 画面出力有り
*	          _DISP_OFF = 画面出力無し
*	DevName	: NULL      = ﾃﾞﾊﾞｲｽ出力無し
*			  NULL     != DevNameへ出力
*	Div		: _ASCII    = ASCII CODE
*			  _S_JIS    = S-JIS CODE
*			  _EBCDIC   = EBCDIC CODE
*	Len		: 出力ﾊﾞｲﾄ数 
*	Msg		: ﾒｯｾｰｼﾞ
*
* Return code
*   None
*
******************************************************************************/
void	wkDump( HWND hWnd, int iListID, BYTE *Data, int DispOut, BYTE *DevName, int Div, int Len, BYTE *Msg )
{

//	int		ListBoxCount;

	int		date[7];

	int		divC;
	FILE	*fp;

	if(Data == NULL)
		return;
	if(Len <= 0)
		return;

	if(DevName != NULL) {
		fp = openDevName(DevName);
	} else {
		fp = NULL;
	}

	dp_hWnd = hWnd;
	dpListID = iListID;

//	DumpPrintf(DispOut, fp, "%s", " ");
	DumpPrintf(DispOut, fp, "%s", " ");

    sgtim( date );	                /* OUTPUT TIME                */
//	DumpPrintf(DispOut, fp, wkDumpFmt,
//				date[0], date[1], date[2], date[3], date[4], date[5], date[6], Msg);
    DumpPrintf(DispOut, fp, wkDumpFmt,
                date[0], date[1], date[2], date[3], date[4], date[5], date[6], Len, Msg);

	divC = Div;
	if(!(divC == _ASCII || divC == _S_JIS || divC == _EBCDIC))
		divC = _ASCII;

	DumpPrintf(DispOut, fp, "%s", FixFmt[divC]);

	DumpDataPrint(DispOut, fp, Data, divC, Len);

//  選択バーを最新の移動する
//	ListBoxCount=SendDlgItemMessage((HWND)dp_hWnd, dpListID, LB_GETCOUNT, 0 , 0);
//	ListBoxCount=SendDlgItemMessage((HWND)dp_hWnd, dpListID, LB_SETCURSEL, ListBoxCount-1, 0);

	if(fp != NULL)
		fclose(fp);

	return;
}


static	void	DumpDataPrint(int DispOut, FILE *fp, BYTE *Data, int divC, int dataLen)
{
	int		ib;
	int		rc;
	char	cData[32];
	BYTE	DataWk[16];
	int		len;

	len = dataLen;
	for(ib=0;  ; ib += 16 ) {
		memset(DataWk, 0x00, sizeof(DataWk));
		rc=DumpDataWkSet(Data, DataWk, dataLen, ib);
		if(rc != 0)
			break;

		memset(cData, 0x00, sizeof(cData));
		if(divC == _ASCII) {
			AsciiDataSet(Data, cData, dataLen, ib);
		} else if(divC == _S_JIS) {
			sJisDataSet(Data, cData, dataLen, ib);
		} else if(divC == _EBCDIC) {
			EbcdicDataSet(Data, cData, dataLen, ib);
		}
		if(cData[16] == 0x00)	cData[16] = 0x20;

		len = len - 16;
		if( len <= 0 ) {
			len = len * (-1);
			switch(len) {
			case 0:
				DumpPrintf(DispOut, fp, DumpFmt1[len], ib, ib,
					DataWk[0],  DataWk[1],  DataWk[2],  DataWk[3],
					DataWk[4],  DataWk[5],  DataWk[6],  DataWk[7],
					DataWk[8],  DataWk[9],  DataWk[10], DataWk[11],
					DataWk[12], DataWk[13], DataWk[14], DataWk[15],
					cData[ 0], cData[ 1], cData[ 2], cData[ 3], cData[ 4],
					cData[ 5], cData[ 6], cData[ 7], cData[ 8], cData[ 9],
					cData[10], cData[11], cData[12], cData[13], cData[14],
					cData[15] );
				break;
			case 15:
				DumpPrintf(DispOut, fp, DumpFmt1[len], ib, ib,
					DataWk[0], cData[0] );
				break;
			case 14:
				DumpPrintf(DispOut, fp, DumpFmt1[len], ib, ib,
					DataWk[0], DataWk[1], cData[ 0], cData[ 1] );
				break;
			case 13:
				DumpPrintf(DispOut, fp, DumpFmt1[len], ib, ib,
					DataWk[0], DataWk[1], DataWk[2],
					cData[ 0], cData[ 1],  cData[ 2] );
				break;
			case 12:
				DumpPrintf(DispOut, fp, DumpFmt1[len], ib, ib,
					DataWk[0], DataWk[1], DataWk[2], DataWk[3],
					cData[ 0], cData[ 1], cData[ 2], cData[ 3] );
				break;
			case 11:
				DumpPrintf(DispOut, fp, DumpFmt1[len], ib, ib,
					DataWk[0], DataWk[1], DataWk[2], DataWk[3],	DataWk[4],
					cData[ 0], cData[ 1], cData[ 2], cData[ 3], cData[ 4] );
				break;
			case 10:
				DumpPrintf(DispOut, fp, DumpFmt1[len], ib, ib,
					DataWk[0], DataWk[1], DataWk[2], DataWk[3], DataWk[4],
					DataWk[5],
					cData[ 0], cData[ 1], cData[ 2], cData[ 3], cData[ 4],
					cData[ 5] );
				break;
			case 9:
				DumpPrintf(DispOut, fp, DumpFmt1[len], ib, ib,
					DataWk[0], DataWk[1], DataWk[2], DataWk[3], DataWk[4],
					DataWk[5], DataWk[6],
					cData[ 0], cData[ 1], cData[ 2], cData[ 3], cData[ 4],
					cData[ 5], cData[ 6] );
				break;
			case 8:
				DumpPrintf(DispOut, fp, DumpFmt1[len], ib, ib,
					DataWk[0], DataWk[1], DataWk[2], DataWk[3], DataWk[4],
					DataWk[5], DataWk[6], DataWk[7],
					cData[ 0], cData[ 1], cData[ 2], cData[ 3], cData[ 4],
					cData[ 5], cData[ 6], cData[ 7] );
				break;
			case 7:
				DumpPrintf(DispOut, fp, DumpFmt1[len], ib, ib,
					DataWk[0], DataWk[1], DataWk[2], DataWk[3], DataWk[4],
					DataWk[5], DataWk[6], DataWk[7], DataWk[8],
					cData[ 0], cData[ 1], cData[ 2], cData[ 3], cData[ 4],
					cData[ 5], cData[ 6], cData[ 7], cData[ 8] );
				break;
			case 6:
				DumpPrintf(DispOut, fp, DumpFmt1[len], ib, ib,
					DataWk[0], DataWk[1], DataWk[2], DataWk[3], DataWk[4],
					DataWk[5], DataWk[6], DataWk[7], DataWk[8], DataWk[9],
					cData[ 0], cData[ 1], cData[ 2], cData[ 3], cData[ 4],
					cData[ 5], cData[ 6], cData[ 7], cData[ 8], cData[ 9] );
				break;
			case 5:
				DumpPrintf(DispOut, fp, DumpFmt1[len], ib, ib,
					DataWk[0], DataWk[1], DataWk[2], DataWk[3], DataWk[4],
					DataWk[5], DataWk[6], DataWk[7], DataWk[8], DataWk[9],
					DataWk[10],
					cData[ 0], cData[ 1], cData[ 2], cData[ 3], cData[ 4],
					cData[ 5], cData[ 6], cData[ 7], cData[ 8], cData[ 9],
					cData[10] );
				break;
			case 4:
				DumpPrintf(DispOut, fp, DumpFmt1[len], ib, ib,
					DataWk[0],  DataWk[1],  DataWk[2],  DataWk[3],  DataWk[4],
					DataWk[5],  DataWk[6],  DataWk[7], 	DataWk[8],  DataWk[9],
					DataWk[10], DataWk[11],
					cData[ 0],  cData[ 1],  cData[ 2],  cData[ 3],  cData[ 4],
					cData[ 5],  cData[ 6],  cData[ 7],  cData[ 8],  cData[ 9],
					cData[10],  cData[11] );
				break;
			case 3:
				DumpPrintf(DispOut, fp, DumpFmt1[len], ib, ib,
					DataWk[0],  DataWk[1],  DataWk[2],  DataWk[3],  DataWk[4],
					DataWk[5],  DataWk[6],  DataWk[7],	DataWk[8],  DataWk[9],
					DataWk[10], DataWk[11],	DataWk[12],
					cData[ 0],  cData[ 1],  cData[ 2],  cData[ 3],  cData[ 4],
					cData[ 5],  cData[ 6],  cData[ 7],  cData[ 8],  cData[ 9],
					cData[10],  cData[11],  cData[12] );
				break;
			case 2:
				DumpPrintf(DispOut, fp, DumpFmt1[len], ib, ib,
					DataWk[0],  DataWk[1],  DataWk[2],  DataWk[3],  DataWk[4], 
					DataWk[5],  DataWk[6],  DataWk[7],	DataWk[8],  DataWk[9],
					DataWk[10], DataWk[11],	DataWk[12], DataWk[13],
					cData[ 0],  cData[ 1],  cData[ 2],  cData[ 3],  cData[ 4],
					cData[ 5],  cData[ 6],  cData[ 7],  cData[ 8],  cData[ 9],
					cData[10],  cData[11],  cData[12],  cData[13] );
				break;
			case 1:
				DumpPrintf(DispOut, fp, DumpFmt1[len], ib, ib,
					DataWk[0],  DataWk[1],  DataWk[2],  DataWk[3],  DataWk[4],
					DataWk[5],  DataWk[6],  DataWk[7],	DataWk[8],  DataWk[9],
					DataWk[10], DataWk[11],	DataWk[12], DataWk[13], DataWk[14],
					cData[ 0],  cData[ 1],  cData[ 2],  cData[ 3],  cData[ 4],
					cData[ 5],  cData[ 6],  cData[ 7],  cData[ 8],  cData[ 9],
					cData[10],  cData[11],  cData[12],  cData[13],  cData[14] );
				break;
			}
		} else {
			DumpPrintf(DispOut, fp, DumpFmt, ib, ib,
				DataWk[0],  DataWk[1],  DataWk[2],  DataWk[3],
				DataWk[4],  DataWk[5],  DataWk[6],  DataWk[7],
				DataWk[8],  DataWk[9],  DataWk[10], DataWk[11],
				DataWk[12], DataWk[13], DataWk[14], DataWk[15],
				cData[ 0], cData[ 1], cData[ 2], cData[ 3], cData[ 4],
				cData[ 5], cData[ 6], cData[ 7], cData[ 8], cData[ 9],
				cData[10], cData[11], cData[12], cData[13], cData[14],
				cData[15] );
//				cData[15], cData[16] );
		}
	}
	DumpPrintf(DispOut, fp, "         data length : %d(0x%04X)[byte]",
				dataLen, dataLen);
}


static	int		DumpDataWkSet(BYTE *Data, BYTE *DataWk, int dataLen, int ib)
{
	if(ib >= dataLen)
		return -1;

	if(ib+16 <= dataLen)
		memcpy(DataWk, (BYTE *)Data+ib, 16);
	else
		memcpy(DataWk, (BYTE *)Data+ib, dataLen-ib);

	return 0;
}


/*-----------------------------------------------------------------------------
*	文字表示編集処理
*----------------------------------------------------------------------------*/
static	void	AsciiDataSet(BYTE *Data, BYTE *cData, int dataLen, int ib)
{
	int		cnt;

	for(cnt=0; cnt<16; cnt++) {
		if(!((ib+cnt) < dataLen)) {
			*(cData+cnt) = '.';
			continue;
		}

		if((0x20 <= *(Data+ib+cnt) && *(Data+ib+cnt) <= 0x7E) ||
		   (0xA1 <= *(Data+ib+cnt) && *(Data+ib+cnt) <= 0xDF)) {
			*(cData+cnt) = *(Data+ib+cnt);
		} else {
			*(cData+cnt) = '.';
		}
	}
}


static	void	sJisDataSet(BYTE *Data, BYTE *cData, int dataLen, int ib)
{
	static	int  sjis;
	int		cnt;

	if(ib == 0)	sjis = 0;

	for(cnt=0; cnt<16; cnt++) {
		if(!((ib+cnt) < dataLen)) {
			*(cData+cnt) = '.';
			continue;
		}

		*(cData+cnt) = *(Data+ib+cnt);

		if(sjis == 1) {
			*(cData) = '.';
			sjis = 0;
		}

		if((0x81 <= *(cData+cnt) && *(cData+cnt) <= 0x9F) ||
		   (0xE0 <= *(cData+cnt) && *(cData+cnt) <= 0xFC)) {
			if((0x40 <= *(Data+ib+cnt+1) && *(Data+ib+cnt+1) <= 0x7E) ||
			   (0x80 <= *(Data+ib+cnt+1) && *(Data+ib+cnt+1) <= 0xFC)) {
				*(cData+cnt+1) = *(Data+ib+cnt+1);
				if(cnt == 14)	break;
				if(cnt == 15)
					sjis = 1;
				else
					cnt++;
			} else {
				*(cData+cnt) = '.';
			}
		} else {
			if(((0x20 <= *(cData+cnt)) && (*(cData+cnt) <= 0x7E)) ||
			   ((0xA0 <= *(cData+cnt)) && (*(cData+cnt) <= 0xDF))) {
			} else {
				*(cData+cnt) = '.';
			}
		}
	}
}


static	void	EbcdicDataSet(BYTE *Data, BYTE *cData, int dataLen, int ib)
{
	int		cnt;
	BYTE	EbcChar;

	for(cnt=0; cnt<16; cnt++) {
		if(!((ib+cnt) < dataLen)) {
			*(cData+cnt) = '.';
			continue;
		}

		toEbcdicChar(*(Data+ib+cnt), &EbcChar);

		if((0x20 <= EbcChar && EbcChar <= 0x7E) ||
		   (0xA1 <= EbcChar && EbcChar <= 0xDF)) {
			*(cData+cnt) = EbcChar;
		} else {
			*(cData+cnt) = '.';
		}
	}
}


static	void	toEbcdicChar(BYTE data, BYTE *EbcChar)
{
	BYTE	jcode[] = {

/* 00 */     0x00, 0x01, 0x02, 0x03, 0x9c, 0x09, 0x86, 0x7f,
/* 08 */     0x97, 0x8d, 0x8e, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
/* 10 */     0x10, 0x11, 0x12, 0x13, 0x9d, 0x85, 0x08, 0x87,
/* 18 */     0x18, 0x19, 0x92, 0x8f, 0x1c, 0x1d, 0x1e, 0x1f,
/* 20 */     0x80, 0x81, 0x82, 0x83, 0x84, 0x0a, 0x17, 0x1b,
/* 28 */     0x88, 0x89, 0x8a, 0x8b, 0x8c, 0x05, 0x06, 0x07,
/* 30 */     0x90, 0x91, 0x16, 0x93, 0x94, 0x95, 0x96, 0x04,
/* 38 */     0x98, 0x99, 0x9a, 0x9b, 0x14, 0x15, 0x9e, 0x1a,
/* 40 */     0x20, 0xa1, 0xa2, 0xa3, 0xa4, 0xa5, 0xa6, 0xa7,
/* 48 */     0xa8, 0xa9, 0xa0, 0x2e, 0x3c, 0x28, 0x2b, 0xe0,
/* 50 */     0x26, 0xaa, 0xab, 0xac, 0xad, 0xae, 0xaf, 0xe1,
/* 58 */     0xb0, 0xe2, 0x21, 0x5c, 0x2a, 0x29, 0x3b, 0xe3,
/* 60 */     0x2d, 0x2f, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66,
/* 68 */     0x67, 0x68, 0x7c, 0x2c, 0x25, 0x5f, 0x3e, 0x3f,
/* 70 */     0x5b, 0x69, 0x6a, 0x6b, 0x6c, 0x6d, 0x6e, 0x6f,
/* 78 */     0x70, 0x60, 0x3a, 0x23, 0x40, 0x27, 0x3d, 0x22,
/* 80 */     0x5d, 0xb1, 0xb2, 0xb3, 0xb4, 0xb5, 0xb6, 0xb7,
/* 88 */     0xb8, 0xb9, 0xba, 0x71, 0xbb, 0xbc, 0xbd, 0xbe,
/* 90 */     0xbf, 0xc0, 0xc1, 0xc2, 0xc3, 0xc4, 0xc5, 0xc6,
/* 98 */     0xc7, 0xc8, 0xc9, 0x72, 0xe4, 0xca, 0xcb, 0xcc,
/* a0 */     0x7e, 0xe5, 0xcd, 0xce, 0xcf, 0xd0, 0xd1, 0xd2,
/* a8 */     0xd3, 0xd4, 0xd5, 0x73, 0xd6, 0xd7, 0xd8, 0xd9,
/* b0 */     0x5e, 0xe6, 0xe7, 0x74, 0x75, 0x76, 0x77, 0x78,
/* b8 */     0x79, 0x7a, 0xda, 0xdb, 0xdc, 0xdd, 0xde, 0xdf,
/* c0 */     0x7b, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47,
/* c8 */     0x48, 0x49, 0xe8, 0xe9, 0xea, 0xeb, 0xec, 0xed,
/* d0 */     0x7d, 0x4a, 0x4b, 0x4c, 0x4d, 0x4e, 0x4f, 0x50,
/* d8 */     0x51, 0x52, 0xee, 0xef, 0xf0, 0xf1, 0xf2, 0xf3,
/* e0 */     0x24, 0x9f, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58,
/* e8 */     0x59, 0x5a, 0xf4, 0xf5, 0xf6, 0xf7, 0xf8, 0xf9,
/* f0 */     0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37,
/* f8 */     0x38, 0x39, 0xfa, 0xfb, 0xfc, 0xfd, 0xfe, 0xff };

	*EbcChar = jcode[data];

	return;
}


static	FILE*	openDevName(BYTE *DevName)
{
	if(DevName != NULL)
		return(fopen( DevName, "at"));

	return (FILE*)NULL;
}


/*============================================================================
* Prototype
*
*   void _Cdecl DumpPrintf(int DispOut, FILE *fp, BYTE , const char *__format, ...);
*
* Parameter
*
*   None
*
* Return code
*
*   None
*
============================================================================*/
void	DumpPrintf(int DispOut, FILE *fp, const char *__format, ... )
{
//	int		ListBoxCount;
//	va_list	argptr;
//	char	str[100];
//	va_start( argptr , __format );
//	if(DispOut == _DISP_ON)
//		vprintf( __format, argptr );

//	vsprintf(str, __format, argptr);
//	ListBoxCount=SendDlgItemMessage((HWND)dp_hWnd, dpListID, LB_ADDSTRING, 0, (LPARAM)(str));

	int		ikensu=0;
	char	cbuff[1024];
	va_list vmarker;

	memset( cbuff, '\0', sizeof(cbuff));
	va_start( vmarker, __format );   										// 可変個の引数の初期化
	vsprintf( cbuff, __format, vmarker );
	// ﾘｽﾄ表示
	SendDlgItemMessage( (HWND)dp_hWnd, dpListID, LB_ADDSTRING, 0, (LPARAM)(char *)cbuff );
	ikensu = SendDlgItemMessage( (HWND)dp_hWnd, dpListID, LB_GETCOUNT, 0, 0 );			// ﾘｽﾄ表示件数取得
	SendDlgItemMessage( (HWND)dp_hWnd, dpListID, LB_SETCURSEL, ikensu-1, 0 );			// 最新のﾃﾞｰﾀにｶｰｿﾙｾｯﾄ
	// ﾘｽﾄ表示件数ﾁｪｯｸ
	if( ikensu > (int)uiListMaxCt ){
		SendDlgItemMessage( (HWND)dp_hWnd, dpListID, LB_DELETESTRING, (WPARAM)0, 0 );	// 最古のﾃﾞｰﾀを消去
	}

	if(fp != NULL) {
		vfprintf( fp, __format, vmarker );
		fprintf( fp, "%s", "\n" );
	}

	va_end( vmarker );              										// 可変個の引数のリセット

	return;
}

void	sgtim(date)
int		date[7];
{
	struct	timeb	tp;
	struct	tm		*ltm;

	/********************************************/
	/*	現在日時格納							*/
	/********************************************/
	ftime(&tp);
	ltm = localtime(&tp.time);
	date[0] = ltm->tm_year + 1900;
	date[1] = ltm->tm_mon + 1;
	date[2] = ltm->tm_mday;

	date[3] = ltm->tm_hour;
	date[4] = ltm->tm_min;
	date[5] = ltm->tm_sec;
    date[6] = tp.millitm;
}


/******************************************************************************
*	指定ﾃﾞﾊﾞｲｽに出力する
*==============================================================================
* Prototype
*	void	msgLog( BYTE *DevName, const char *__format, ... )
*
* Parameter
*	DevName	: NULL      = ﾃﾞﾊﾞｲｽ出力無し
*			  NULL     != DevNameへ出力
*
* Return code
*   None
*
******************************************************************************/
void	msgLog( HWND hWnd, int iListID, BYTE *DevName, const char *__format, ... )
{
//	FILE	*msgFp;
	int		date[7];
	va_list	argptr;
	char	str[256];

	sgtim(date);

	va_start( argptr , __format );

	// Display 出力 ---------------------------------------
//	printf("\n%04d/%02d/%02d %02d:%02d:%02d ",
//			 date[0], date[1], date[2], date[3], date[4], date[5] );
//	vprintf( __format, argptr );

	vsprintf( str, __format, argptr );
	// ファイル出力 ---------------------------------------
//	if(DevName != NULL) {
//		msgFp = openDevName(DevName);
//	} else {
//		msgFp = NULL;
//	}

//	if(msgFp != NULL) {
//		fprintf(msgFp, "\n%04d/%02d/%02d %02d:%02d:%02d ",
//				 date[0], date[1], date[2], date[3], date[4], date[5] );
//		vfprintf( msgFp, __format, argptr );
//		fclose(msgFp);
//	}

	SendDlgItemMessage((HWND)hWnd, iListID, LB_ADDSTRING, 0, (LPARAM)(str));

	va_end( argptr );

	return;
}

