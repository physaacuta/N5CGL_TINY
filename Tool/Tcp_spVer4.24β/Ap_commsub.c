/***********************************************************************
*
*        CUSTOMER    :   新日本製鐡(株)殿
*        PROCESS     :   ﾍﾟｰﾊﾟﾚｽｼｽﾃﾑ
*        CPU NAME    :   Pentium Ⅱ 233MHz
*        MODULE      :   共通ｻﾌﾞﾙｰﾁﾝ
*
*        FILE NAME   :   AP_COMMSUB.C
*
*        COMPUTER    :   DIGITAL PC
*        OS-TYPE     :   Windows NT Workstation  4.0
*        COMPILER    :   Visual C/C++ Ver5.0
*        PROGRAM     :   太平工業（株)
*        DATE        :   1998/10/07
*        PROGRAMER   :
*
*        VERSION     :   1.0 ...    /  /
*        REVISION    :   1.1 ...    /  /             by	
*
*
*
*
************************************************************************/
//*********************************************************************
//       System  include  Declarative  Statemant
//*********************************************************************
//
//*************************
//**** Include         ****
//*************************
// ヘッダー定義
#include	"AP_Func.h"
//
#define		SYSLOG	"Syslog.dat"
GLBDAT*		Glbdat;													// 共有ﾒﾓﾘｴﾘｱ
//
//*********************************************************************
//
//	AP_Sfopen()		:	ﾌｧｲﾙｵｰﾌﾟﾝ処理
//
//	para	cFname 	: 	file name   (char *)
//			iType	: 	file type	(int)
//
//	file open subroutine
//	return code = 
//		normal   	: 	file handle
//		abnormal 	: 	-1
//       System  include  Declarative  Statemant
//
//*********************************************************************
int 	AP_Sfopen(char *cFname,int iType)
{
    switch ( iType ) {
	case AP_RDONLY :												// 読込専用
		return _open( cFname,O_RDONLY | O_BINARY,S_IREAD | S_IWRITE );
		break;
	case AP_WRONLY :              									// 書込専用(NOT CREAT)
		return _open( cFname,O_WRONLY | O_BINARY,S_IREAD | S_IWRITE );
		break;
	case AP_APENDWR :              									// 書込専用(APPEND)
		return _open( cFname,O_WRONLY | O_APPEND | O_BINARY,S_IREAD | S_IWRITE );
 		break;
	case AP_CREATWR :			              						// 書込専用(CREAT)
		return _open( cFname,O_WRONLY | O_CREAT | O_BINARY,S_IREAD | S_IWRITE );
		break;
	case AP_RDWR :                									// 読書込専用(NOT CREAT)
		return _open( cFname,O_RDWR | O_BINARY,S_IREAD | S_IWRITE );
		break;
	case AP_CREATRDWR :            									// 読書込専用(CREAT)
		return _open( cFname,O_RDWR |O_CREAT|O_BINARY,S_IREAD|S_IWRITE );
		break;
	case 7 :                    									// 読込専用(TEXT MODE)
		return _open( cFname,O_RDONLY | O_TEXT,S_IREAD|S_IWRITE );
		break;
	default :
		return -1;
		break;
    }
}
//
//*********************************************************************
//
//	AP_Sgrec()			:	ﾚｺｰﾄﾞ指定ﾌｧｲﾙ読込み処理
//
//	para	iFd         :	file handle   (int)
//	        iRecordSize :	record size   (unsigned int)
//	        iRecordNo   :	record no.    (unsigned int)
//	        *cBuffer    :	read buffer   (char *)
//
//
//	file read subroutine with record
//		from offset to record size
//	return code =
//		normal   		: 	read byte number
//		abnormal		: 	-1
//
//*********************************************************************
int 	AP_Sgrec(int iFd,unsigned int iRecordSize,unsigned int iRecordNo,void *cBuffer)
{
	_off_t	offset;															// file offset

	offset = (unsigned long)iRecordNo * (unsigned long)iRecordSize;
	if ( _lseek(iFd, offset, SEEK_SET) < 0 ) return -1;
	return _read( iFd, cBuffer, iRecordSize );
}
//
//*********************************************************************
//
// 	AP_Sprec()			:	ﾚｺｰﾄﾞ指定ﾌｧｲﾙ書込み
//
//	para	iFd         :	file handle   (int)
//	        iRecordSize	:	record size   (unsigned int)
//	        iRecordNo  	:	record no.    (unsigned int)
//	        *cBuffer    :	read buffer   (char *)
//
//
//	file write subroutine with record
//		from offset to record size
//	return code =
//		normal   		: 	write byte number
//		abnormal 		:	-1
//
//*********************************************************************
int 	AP_Sprec(int iFd, unsigned int iRecordSize, unsigned int iRecordNo, void *cBuffer)
{
	_off_t	offset;															// file offset

	offset = (unsigned long)iRecordNo * (unsigned long)iRecordSize;
	if ( _lseek(iFd, offset, SEEK_SET) < 0 ) return -1;
	return _write( iFd, cBuffer, iRecordSize );
}
//
/*********************************************************************
* 	AP_Asprec()
*
*	para	iFd         :file handle   (int)
*	        iRecordSize	:record size   (unsigned int)
*	        *cBuffer    :read buffer   (char *)
*
*	file write subroutine with record
*		from offset to record size
*	return code =
*		normal   : write byte number
*		abnormal : -1
*********************************************************************/
int 	AP_Asprec(int iFd, unsigned int iRecordSize, void *cBuffer)
{
	return _write( iFd, cBuffer, iRecordSize );
}
//
//*********************************************************************
//
//	AP_fclose()			:	ﾌｧｲﾙｸﾛｰｽﾞ
//
//   	para  iFd     	: 	file handle     (int)
//
//	file close subroutine
//	return code = 
//		normal   		: 	0
//		abnormal 		: 	-1
//
//*********************************************************************
int 	AP_Sfclose(int iFd)
{
	int	irtn=0;
	int	ilpct=0;

	while ( TRUE ) {
		irtn = _close(iFd);
		if ( irtn < 0 ) {
			ilpct++;
			if ( ilpct > WAIT_COUNT ) break;
			Sleep(WAIT_TIMER);
		} else {
			break;
		}
	}
	return irtn;
}
//
//*********************************************************************
//
//	AP_Diskfree()			:	ﾃﾞｨｽｸ残容量算出
//
//   	para  *cDrv       	: 	rute directry     (char *)
//			  *ulHddFree 	: 	Disk Free Size    (unsigned long *)
//
//	diskfree size subroutine
//	return code = 
//		normal   			: 	0
//		abnormal 			: 	-1
//
//*********************************************************************
int 	AP_Diskfree( char *cDrv,unsigned long *ulHddFree )
{
	int 	ret;
	DWORD 	spc;
	DWORD 	bps;
	DWORD 	freec;
	DWORD 	clst;

	ret = GetDriveType( cDrv );
	if ( ret <= 1 ) return -1;
   	ret = GetDiskFreeSpace( cDrv,											// Rute directry
       	                    &spc,											// Secter per Cluster
           	                &bps,    										// Byte per Secter
               	            &freec,  										// Free Cluster
                   	        &clst ); 										// Number of Cluster
    if ( ret != 1 ) return -1;
    *ulHddFree = spc * bps * freec; 										// 残容量算出
	return 0;
}
//
//*************************************************************************
//
//	AP_Sgtim()				:	時刻取込
//
//   	para  *wDateTime	: date time get buffer     (WORD)
//
//*************************************************************************
void 	AP_Sgtim(WORD *wDateTime)
{
	SYSTEMTIME Systemtime;

//	GetSystemTime( (LPSYSTEMTIME)&Systemtime );								// ｸﾞﾘﾆｯｼﾞ時刻
	GetLocalTime( (LPSYSTEMTIME)&Systemtime );								// 東京時刻

	*wDateTime = Systemtime.wYear;											// 年
	*(wDateTime+1) = Systemtime.wMonth;										// 月
	*(wDateTime+2) = Systemtime.wDay;										// 日
	*(wDateTime+3) = Systemtime.wHour;										// 時
	*(wDateTime+4) = Systemtime.wMinute;									// 分
	*(wDateTime+5) = Systemtime.wSecond;									// 秒
}
//
//*************************************************************************
//
//	AP_Sptim( )	:	時刻書込
//
//   	para  *iDate_Time	: date time get buffer     (WORD)
//
//*************************************************************************
void 	AP_Sptim(WORD *wDateTime)
{
	SYSTEMTIME Systemtime;

	Systemtime.wYear = *wDateTime;											// 年
	Systemtime.wMonth = *(wDateTime+1);										// 月
	Systemtime.wDay = *(wDateTime+2);										// 日
	Systemtime.wHour = *(wDateTime+3);										// 時
	Systemtime.wMinute = *(wDateTime+4);									// 分
	Systemtime.wSecond = *(wDateTime+5);									// 秒

//	SetSystemTime( (LPSYSTEMTIME)&Systemtime );								// ｸﾞﾘﾆｯｼﾞ時刻
	SetLocalTime( (LPSYSTEMTIME)&Systemtime );								// 東京時刻
}
//
//*************************************************************************
//
//	AP_Tcsas( )	:	日付加減算処理
//
//   	para  *wInDateTime	: date time get buffer		(WORD)
//   	      *wTime		: time get buffer		   	(WORD)
//   	      iFlag			: 0:未来 1:過去				(INT)
//            *wOutDateTime	: date time set buffer		(WORD)
//
//	return code = 
//		normal   			: 	0
//		para error			: 	-1
//
//*************************************************************************
int 	AP_Tcsas( WORD *wInDateTime, WORD *wTime, int iFlag, WORD *wOutDateTime )
{
																			// 時刻編集用Buffer
	int	idate[8];                          									// 日付時刻編集Buffer
	int	iyear;                             									// 年編集Buffer
	int	iday;                              									// 日編集Buffer
	int	ihour;                             									// 時編集Buffer
	int	imin;                              									// 分編集Buffer
	int	isec;                              									// 秒編集Buffer

	int	ickyear;                            								// 年編集Buff(年変換用)
	int	isumday;                            								// 日編集Buff(日換算用)
	int	isubday;                            								// 日編集Buff(日減算用)
	int	ickhour;                            								// 時編集Buff(時差分用)

	int	ilp;
	int	iurudos;															// 閏年判定flag
	int	idaydt[13] = { 0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };

   																			// 区分の上下限check
	if (( 0 > iFlag ) || ( iFlag > 1 )) return -1;

																			// 入力時刻の上下限check
	iurudos = AP_DateTimeCheck( wInDateTime );
	if ( iurudos < 0 ) return -1;
																			// 入力時間の上下限check
	if ( 0 > (int)wTime[0] ) return -1;
	if (( 0 > (int)wTime[1] ) || ( (int)wTime[1] > 59 ) ) return -1;
	if (( 0 > (int)wTime[2] ) || ( (int)wTime[2] > 59 ) ) return -1;

	memset( idate, '\0', sizeof(idate));
	for ( ilp=0;ilp<6;ilp++ ) idate[ilp] = (int)*(wInDateTime+ilp);

																			// 編集年度の月を日数に換算する
	isumday = 0;
	if ( idate[1] > 1 ) {
		for ( ilp= 0;ilp<idate[1];ilp++ ) isumday = isumday + idaydt[ilp];
		if (( idate[1] > 2 ) && ( iurudos == -1 )) {
      		isumday = isumday + 1;
   		}
	}
	isumday = isumday + idate[2];

	if ( iFlag == 0 ) {                         							// 加算
																			// 秒算出
		isec = idate[5] + (int)wTime[2];
		*(wOutDateTime+5) = isec % 60;                 						// 秒
																			// 分算出
		imin = idate[4] + (int)wTime[1] + ( isec / 60 );
		*(wOutDateTime+4) = (int)(imin % 60);                 				// 分
																			// 時算出
		ihour = idate[3] + (int)wTime[0] + ( imin / 60 );
		*(wOutDateTime+3) = ihour % 24;                  					// 時
																			// 日算出
		iday = ( ihour / 24 ) + isumday;

		if ( iurudos == 1 ) {
	    	ickyear = 366;                      							// 閏年
		} else {
    		ickyear = 365;
		}

		iyear = idate[0];
		while ( iday > ickyear ) {
			iday  = iday - ickyear;              							// 1年分削除する
			iyear = iyear + 1;                   							// 年加算
																			// 処理年度が閏年か判定する */
			if ((( iyear % 4 )   == 0 ) && (( iyear % 100 ) != 0 ) || (( iyear % 400 ) == 0 )) {
				ickyear = 366;                   							// 閏年
			} else {
				ickyear = 365;
			}
		}
	} else {
																			// 初期値設定 */
		isec  = idate[5];                   								// 秒
		imin  = idate[4];                      								// 分
		ihour = idate[3];                      								// 時
		iday  = isumday;                        							// 日

																			// 秒算出
		if (( isec < (int)wTime[2] ) || ( isec < 0 )) {
			isec = isec + 60;                    							// 分 -> 秒
			imin = imin - 1;
		}
		*(wOutDateTime+5) = (WORD)isec - wTime[2];     						// 秒差分
																			// 分算出
		if (( imin < (int)wTime[1] ) || ( imin < 0 )) {
			imin  = imin + 60;                   							// 時 -> 分
			ihour = ihour - 1;
		}
		*(wOutDateTime+4) = (WORD)imin - wTime[1];             				// 分差分
																			// 時算出
		isubday = (int)wTime[0] / 24;              							// 減算日数算出
		ickhour = (int)wTime[0] % 24;              							// 減算時間算出
		if (( ihour < ickhour ) || ( ihour < 0 )) {
			ihour = ihour + 24;                  							// 日 -> 時
			iday  = iday -1;
		}
		*(wOutDateTime+3) = ihour - ickhour;              					// 時差分
																			// 日算出
		iyear = idate[0];
		while ((( iday < isubday ) || ( iday == 0 ))) {
			iyear = iyear -1;                   							// 年減算
																			// 処理年度が閏年か判定する
			if ((( iyear % 4 ) == 0 ) && (( iyear % 100 ) != 0 ) || (( iyear % 400 ) == 0 )) {
				iday = iday + 366;                							// 閏年
			} else {
				iday = iday + 365;
			}
		}
		iday = iday - isubday;
	}
																			// 年月日を求める
	for ( ilp=1; ilp<=12; ilp++ ) {
		if ((((iyear % 4)   == 0 ) && ((iyear % 100) != 0 ) || ((iyear % 400 ) == 0 )) && ( ilp == 2)) {
			if ( iday <= (idaydt[ilp] + 1) ) {
				*(wOutDateTime+2) = iday;                  					// 日
				*(wOutDateTime+1) = ilp;                   					// 月
				break;
			}
			iday = iday - ( idaydt[ilp] + 1 );
		} else {
			if ( iday <= idaydt[ilp] ) {
				*(wOutDateTime+2) = iday;                  					// 日
				*(wOutDateTime+1) = ilp;                   					// 月
				break;
			}
			iday = iday - idaydt[ilp];
		}
	}
	if ( iyear < 0 ) return -1;
	*wOutDateTime = iyear;													// 年
	return 0;
}
//
//*********************************************************************
//
//	AP_Date_Time_Check()	:	日付時間入力ﾁｪｯｸ
//
//   	para  *wDateTime	: 	date time set     		(WORD *)
//
//	next date time get subroutine
//	return code = 
//		normal   			: 	0
//		閏年				:	1
//		Year Error			:	-1
//		Month Error			:	-2
//		Day Error			:	-3
//		Hour Error			:	-4
//		Minute Error		:	-5
//		Second Error		:	-6
//
//*********************************************************************
int		AP_DateTimeCheck( WORD *wDateTime )
{
																			// 年ﾁｪｯｸ
	if ( wDateTime[0] < 1970 || wDateTime[0] > 2050 ) return -1;
																			// 月ﾁｪｯｸ
	if( wDateTime[1] < 1 || wDateTime[1] > 12 ) return -2;
																			// 日ﾁｪｯｸ
	if ( (wDateTime[0] % 4) == 0 ) {
		if ( wDateTime[1] == 2 && wDateTime[2] > 29 ) return -3;
	} else {
		if ( wDateTime[1] == 2 && wDateTime[2] > 28 ) return -3;
	}


	if ( (wDateTime[1] == 4 || wDateTime[1] == 6 ||
		wDateTime[1] == 9 || wDateTime[1] == 11) &&
		wDateTime[2] > 30 ) return -3;

	if ( (wDateTime[1] == 1 || wDateTime[1] == 3 ||
		wDateTime[1] == 5 || wDateTime[1] == 7 ||
		wDateTime[1] == 8 || wDateTime[1] == 10 ||
		wDateTime[1] == 12) && wDateTime[2] > 31 ) return -3;
																			// 時ﾁｪｯｸ
	if ( (int)wDateTime[3] < 0 || (int)wDateTime[3] > 23 ) return -4;
																			// 分ﾁｪｯｸ
	if ( (int)wDateTime[4] < 0 || (int)wDateTime[4] > 59 ) return -5;
																			// 秒ﾁｪｯｸ
	if ( (int)wDateTime[5] < 0 || (int)wDateTime[5] > 59 ) return -6;

	if ( ( wDateTime[0] % 4 ) == 0 ) return 1;								// 閏年ﾁｪｯｸ
	return 0;
}
//
//*********************************************************************
//	AP_Syslog()
//
//   	para	*_cBuffer	: Error Msg    		(const char *)
//				...			: 引数省略記号		(...)
//
//*********************************************************************
void 	AP_Syslog( const char *__cBuffer, ... )
{
	int		ilp;
	int 	ifp;
	int		imaxlen;
	WORD 	wdatetime[6];
	long 	lrec;
	char	crec[10];
	char 	cerrdat[128];
	va_list vmarker;

	memset( wdatetime,'\0',sizeof(wdatetime) );
	memset( cerrdat,'\0',sizeof(cerrdat) );

	ifp = AP_Sfopen( SYSLOG, AP_RDWR );
	if ( ifp < 0 ) {
		memset( cerrdat, ' ', sizeof(cerrdat) );
		cerrdat[126] = 0x0d;
		cerrdat[127] = 0x0a;
    	ifp = AP_Sfopen( SYSLOG, AP_CREATRDWR );
	    AP_Sprec( ifp, 128, 0, cerrdat );
	}
	AP_Sfclose( ifp );

	memset( cerrdat, '\0', sizeof(cerrdat) );
	memset( crec, '\0', sizeof(crec) );
	ifp = AP_Sfopen( SYSLOG, AP_CREATRDWR );
	AP_Sgrec( ifp, 128, 0, cerrdat );										// 最新ﾎﾟｲﾝﾀ読込
	memcpy( crec, cerrdat, 5 );
	lrec = atol(crec);
	lrec++;
	if ( lrec == 1000 ) lrec = 1;
	memset( crec, '\0', sizeof(crec) );
	sprintf( crec, "%05d", lrec);
	memcpy( cerrdat, crec, 5 );
	AP_Sprec( ifp, 128, 0, cerrdat );

	memset( cerrdat,'\0',sizeof(cerrdat) );
	AP_Sgtim( wdatetime );													// 現在時刻読込
	sprintf( cerrdat, "%04d/%02d/%02d %02d:%02d:%02d ",
			 wdatetime[0], wdatetime[1], wdatetime[2], wdatetime[3], wdatetime[4], wdatetime[5] );
	va_start( vmarker, __cBuffer );   										// 可変個の引数の初期化
	vsprintf( &cerrdat[20], __cBuffer, vmarker );
	va_end( vmarker );              										// 可変個の引数のリセット
	imaxlen = strlen(cerrdat);
	for ( ilp=imaxlen;ilp<128;ilp++ ) cerrdat[ilp] = 0x20;
	cerrdat[126] = 0x0d;
	cerrdat[127] = 0x0a;
	AP_Sprec( ifp, 128, (unsigned int)lrec, cerrdat );
	AP_Sfclose( ifp );
}
//
//******************************************************************************
//	日付管理ﾌｧｲﾙ書込処理
//
//	PARAMETER
//		char	*cDir		: 格納ﾃﾞｨﾚｸﾄﾘ名
//		WORD	wDateTime	: 日付、時間
//
//  RETURN	= 	 0 正常
//        		-1 日付管理ﾌｧｲﾙ ｵ-ﾌﾟﾝｴﾗｰ
//        		-2 日付管理ﾌｧｲﾙ 書込1ｴﾗｰ
//        		-3 日付管理ﾌｧｲﾙ 読込1ｴﾗｰ
//        		-4 日付管理ﾌｧｲﾙ 書込2ｴﾗｰ
//
//******************************************************************************
int 	AP_MkdayfWt( char *cDir, WORD *wDateTime )
{
	int 	ifp=0;
	int 	irtn=0;
	int 	icmp;
	char 	cdate[9];
	char	cfilename[50];
	char	cmkdayfname[50];
	MKDAY   stmkday;

	memset( cdate,'\0',sizeof(cdate) );
	memset( cmkdayfname,'\0',sizeof(cmkdayfname) );
	memset( &stmkday,'\0',sizeof(stmkday) );
	sprintf( cmkdayfname, "%s\\Mkday.dat", cDir );
	ifp = AP_Sfopen( cmkdayfname, AP_RDWR );								// 日付管理ﾌｧｲﾙ ｵｰﾌﾟﾝ(READ WRITE)
	if ( ifp < 0 ) {
    	ifp = AP_Sfopen( cmkdayfname, AP_CREATRDWR );						// 日付管理ﾌｧｲﾙ ｵｰﾌﾟﾝ(READ WRITE CREAT)
	    if ( ifp < 0 ) return -1;

   	    stmkday.sOldpnt = 0;       											// 最古ﾎﾟｲﾝﾀ
        stmkday.sNewpnt = 0;       											// 最新ﾎﾟｲﾝﾀ
																			// 日付ｾｯﾄ
       	sprintf( stmkday.sDate[stmkday.sNewpnt].cDate, "%04d%02d%02d", wDateTime[0], wDateTime[1], wDateTime[2] );
																			// 時間ｾｯﾄ
       	sprintf( stmkday.sDate[stmkday.sNewpnt].cTime, "%02d%02d%02d", wDateTime[3], wDateTime[4], wDateTime[5] );
																			// 日付管理ﾌｧｲﾙ書込
        irtn = AP_Sprec( ifp, sizeof(stmkday), 0, (char *)&stmkday );
        if ( irtn < 0 ) {
            AP_Sfclose( ifp );
			return -2;
		}
	} else {
    	irtn = AP_Sgrec( ifp, sizeof(stmkday), 0, (char *)&stmkday );		// ﾌｧｲﾙ読込み処理
	    if ( irtn < 0 ) {
            AP_Sfclose( ifp );
			return -3;
		}

       	sprintf( cdate,"%04d%02d%02d", wDateTime[0], wDateTime[1], wDateTime[2] );
																			// 日付更新ﾁｪｯｸ
        icmp = strncmp( cdate, stmkday.sDate[stmkday.sNewpnt].cDate, 9 );
        if ( icmp > 0 ) {
            if ( stmkday.sOldpnt > stmkday.sNewpnt ) {
				memset( cfilename,'\0',sizeof(cfilename) );
				sprintf( cfilename, "%s\\%s.dat", cDir, stmkday.sDate[stmkday.sOldpnt].cDate );
        	    unlink( cfilename );									// YYYYMMDD.DAT ﾌｧｲﾙ削除
               	stmkday.sNewpnt++;      									// 最新ﾎﾟｲﾝﾀ 更新
               	stmkday.sOldpnt++;      									// 最古ﾎﾟｲﾝﾀ 更新
				if ( stmkday.sOldpnt == MAXDAY ) stmkday.sOldpnt = 0;		// 最古ﾎﾟｲﾝﾀ 更新
       	    } else {
   	            stmkday.sNewpnt++;       									// 最新ﾎﾟｲﾝﾀ 更新
   	            if ( stmkday.sNewpnt == MAXDAY ) {
					memset( cfilename,'\0',sizeof(cfilename) );
					sprintf( cfilename, "%s\\%s.dat", cDir, stmkday.sDate[0].cDate );
                	unlink( cfilename );								// YYYYMMDD.DAT ﾌｧｲﾙ削除
          	        stmkday.sNewpnt = 0;    								// 最新ﾎﾟｲﾝﾀ 更新
               	    stmkday.sOldpnt++;      								// 最古ﾎﾟｲﾝﾀ
       	        }
           	}
																			// 日付ｾｯﾄ
            sprintf( stmkday.sDate[stmkday.sNewpnt].cDate, "%04d%02d%02d", wDateTime[0], wDateTime[1], wDateTime[2] );
																			// 時間ｾｯﾄ
            sprintf( stmkday.sDate[stmkday.sNewpnt].cTime, "%02d%02d%02d", wDateTime[3], wDateTime[4], wDateTime[5] );
																			// 日付管理ﾌｧｲﾙ書込
       	    irtn = AP_Sprec( ifp, sizeof(stmkday), 0, (char *)&stmkday );
           	if ( irtn < 0 ) {
                AP_Sfclose( ifp );
   	            return -4;
       	    }
    	}
	}
	AP_Sfclose( ifp );              										// 日付管理ﾌｧｲﾙｸﾛｰｽﾞ
	return 0;
}
//
//******************************************************************************
//	日付管理ﾌｧｲﾙ読込処理
//
//	PARAMETER
//		char	*cDir		: 格納ﾃﾞｨﾚｸﾄﾘ名
//		char	*cFileName	: ﾃﾞｰﾀ格納ﾌｧｲﾙ名称(YYYYDDMM.DAT)
//
//  RETURN	= 	 0 正常
//        		-1 日付管理ﾌｧｲﾙ ｵ-ﾌﾟﾝｴﾗｰ
//        		-2 日付管理ﾌｧｲﾙ 読込ｴﾗｰ
//
//******************************************************************************
int 	AP_MkdayfRd( char *cDir, char *cFileName )
{
	int 	ifp=0;
	int 	irtn=0;
	char	cmkdayfname[50];
	MKDAY   stmkday;

	memset( cmkdayfname,'\0',sizeof(cmkdayfname) );
	memset( &stmkday,'\0',sizeof(stmkday) );
	sprintf( cmkdayfname, "%s\\Mkday.dat", cDir );
	ifp = AP_Sfopen( cmkdayfname, AP_RDWR );								// 日付管理ﾌｧｲﾙ ｵｰﾌﾟﾝ(READ WRITE)
	if ( ifp < 0 ) return -1;

   	irtn = AP_Sgrec( ifp, sizeof(stmkday), 0, (char *)&stmkday );			// ﾌｧｲﾙ読込み処理
    if ( irtn < 0 ) {
		AP_Sfclose( ifp );             										// 日付管理ﾌｧｲﾙｸﾛｰｽﾞ
		return -2;
	}
																			// 日付ｾｯﾄ
    sprintf( cFileName, "%s.dat", stmkday.sDate[stmkday.sNewpnt].cDate );
	AP_Sfclose( ifp );              										// 日付管理ﾌｧｲﾙｸﾛｰｽﾞ
	return 0;
}
//
//******************************************************************************
//	共有ﾒﾓﾘ使用宣言
//
//******************************************************************************
void	AP_Kyoyu( )
{
    HANDLE    hMapFile;
    LPVOID    lpMapped;

    hMapFile = CreateFileMapping( (HANDLE)0xffffffff, NULL, PAGE_READWRITE, 0, COMMON_SIZE_FS, "MyShareMemory01" );
    lpMapped = MapViewOfFile( hMapFile, FILE_MAP_ALL_ACCESS, 0, 0, 0 );

    AP_CommonAdr( (char*)lpMapped );

}
//
void	AP_CommonAdr( char* adrs )
{
	// 共有ﾒﾓﾘ
    Glbdat = (void* )adrs;
    adrs += sizeof( *Glbdat );
    return;
}



static int etoa(unsigned char ebcdic, unsigned char* ascii);
static int atoe(unsigned char ascii,  unsigned char* ebcdic);

/*********************************************************************
 *    EBCDIC => ASCII コード変換テーブル
 *********************************************************************/
static const unsigned char ASCII_CODE[] = {
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
/* f8 */     0x38, 0x39, 0xfa, 0xfb, 0xfc, 0xfd, 0xfe, 0xff
};

/*********************************************************************
 *    ASCII => EBCDIC コード変換テーブル
 *********************************************************************/
static const unsigned char EBCDIC_CODE[] = {
/* 00 */     0x00, 0x01, 0x02, 0x03, 0x37, 0x2d, 0x2e, 0x2f,
/* 08 */     0x16, 0x05, 0x25, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
/* 10 */     0x10, 0x11, 0x12, 0x13, 0x3c, 0x3d, 0x32, 0x26,
/* 18 */     0x18, 0x19, 0x3f, 0x27, 0x1c, 0x1d, 0x1e, 0x1f,
/* 20 */     0x40, 0x5a, 0x7f, 0x7b, 0xe0, 0x6c, 0x50, 0x7d,
/* 28 */     0x4d, 0x5d, 0x5c, 0x4e, 0x6b, 0x60, 0x4b, 0x61,
/* 30 */     0xf0, 0xf1, 0xf2, 0xf3, 0xf4, 0xf5, 0xf6, 0xf7,
/* 38 */     0xf8, 0xf9, 0x7a, 0x5e, 0x4c, 0x7e, 0x6e, 0x6f,
/* 40 */     0x7c, 0xc1, 0xc2, 0xc3, 0xc4, 0xc5, 0xc6, 0xc7,
/* 48 */     0xc8, 0xc9, 0xd1, 0xd2, 0xd3, 0xd4, 0xd5, 0xd6,
/* 50 */     0xd7, 0xd8, 0xd9, 0xe2, 0xe3, 0xe4, 0xe5, 0xe6,
/* 58 */     0xe7, 0xe8, 0xe9, 0x70, 0x5b, 0x80, 0xb0, 0x6d,
/* 60 */     0x79, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68,
/* 68 */     0x69, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77,
/* 70 */     0x78, 0x8b, 0x9b, 0xab, 0xb3, 0xb4, 0xb5, 0xb6,
/* 78 */     0xb7, 0xb8, 0xb9, 0xc0, 0x6a, 0xd0, 0xa0, 0x07,
/* 80 */     0x20, 0x21, 0x22, 0x23, 0x24, 0x15, 0x06, 0x17,
/* 88 */     0x28, 0x29, 0x2a, 0x2b, 0x2c, 0x09, 0x0a, 0x1b,
/* 90 */     0x30, 0x31, 0x1a, 0x33, 0x34, 0x35, 0x36, 0x08,
/* 98 */     0x38, 0x39, 0x3a, 0x3b, 0x04, 0x14, 0x3e, 0xe1,
/* a0 */     0x4a, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47,
/* a8 */     0x48, 0x49, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56,
/* b0 */     0x58, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87,
/* b8 */     0x88, 0x89, 0x8a, 0x8c, 0x8d, 0x8e, 0x8f, 0x90,
/* c0 */     0x91, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97, 0x98,
/* c8 */     0x99, 0x9a, 0x9d, 0x9e, 0x9f, 0xa2, 0xa3, 0xa4,
/* d0 */     0xa5, 0xa6, 0xa7, 0xa8, 0xa9, 0xaa, 0xac, 0xad,
/* d8 */     0xae, 0xaf, 0xba, 0xbb, 0xbc, 0xbd, 0xbe, 0xbf,
/* e0 */     0x4f, 0x57, 0x59, 0x5f, 0x9c, 0xa1, 0xb1, 0xb2,
/* e8 */     0xca, 0xcb, 0xcc, 0xcd, 0xce, 0xcf, 0xda, 0xdb,
/* f0 */     0xdc, 0xdd, 0xde, 0xdf, 0xea, 0xeb, 0xec, 0xed,
/* f8 */     0xee, 0xef, 0xfa, 0xfb, 0xfc, 0xfd, 0xfe, 0xff
};

/*********************************************************************
 * strnatoe()
 *    ASCII => EBCDIC コード文字列変換関数
 *    ASCII CODE (ASC) を EBCDIC CODE (EBC) に文字数分変換する。
 * 関数の戻り値
 *    正常 :  0
 *    異常 : -1
 *********************************************************************/
int strnatoe(const char* ascii, char* ebcdic, int bytes)
{
    int i;

    for (i = 0; i < bytes; i++) {
        if (0 > atoe(*(ascii + i), (unsigned char*)(ebcdic + i))) {
            return -1;
        }
    }
    return 0;
}

/*********************************************************************
 * strnetoa()
 *    EBCDIC => ASCII コード文字列変換関数
 *    EBCDIC CODE (EBC) を ASCII CODE (ASC) に文字数分変換する。
 * 関数の戻り値
 *    正常 :  0
 *    異常 : -1
 *********************************************************************/
int strnetoa(const char* ebcdic, char* ascii, int bytes)
{
    int i;

    for (i = 0; i < bytes; i++) {
        if (0 > etoa(*(ebcdic + i), (unsigned char*)(ascii + i))) {
            return -1;
        }
    }
    return 0;
}

/*********************************************************************
 * etoa()
 *    EBCDIC => ASCII コード変換関数
 * 関数の戻り値
 *	正常 :  0
 *	異常 : -1
 *********************************************************************/
static
int etoa(unsigned char ebcdic, unsigned char* ascii)
{
    *ascii = ASCII_CODE[ebcdic];
    return 0;
}

/*********************************************************************
 * atoe()
 *    ASCII => EBCDIC コード変換関数
 * 関数の戻り値
 *    正常 :  0
 *    異常 : -1
 *********************************************************************/
static
int atoe(unsigned char ascii, unsigned char* ebcdic)
{
    *ebcdic = EBCDIC_CODE[ascii];
    return 0;
}

/* -------------------- end of file -------------------- */
