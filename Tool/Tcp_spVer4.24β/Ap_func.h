/***********************************************************************
*
*        CUSTOMER    :   新日本製鐡(株)殿
*        PROCESS     :   ﾍﾟｰﾊﾟﾚｽｼｽﾃﾑ
*        CPU NAME    :   Pentium Ⅱ 233MHz
*        MODULE      :   共通ｻﾌﾞﾍｯﾀﾞ
*
*        FILE NAME   :   AP_FUNC.H
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
//
#include	<dos.h>
#include	<fcntl.h>
#include	<stdio.h>
#include	<conio.h>
#include	<memory.h>
#include	<stdlib.h>
#include	<string.h>
#include	<time.h>
#include	<io.h>
#include	<sys\types.h>
#include	<sys\timeb.h>
#include	<sys\stat.h>
#include	<share.h>
#include	<math.h>
#include	<windows.h>
#include	<excpt.h>
#include	<richedit.h>
#include	<commctrl.h>
#include	<windows.h>
#include 	<winsock.h>
//
#include 	"AP_File.h"
//
#ifndef	_AP_FUNC_H
#define	_AP_FUNC_H
//
//*********************************************************************
//       関  数  宣  言
//*********************************************************************
//
//************************************
// MODULE : 共通(FILESUB)
//************************************
//
int		AP_Sfopen(char *cFname, int iType);									// ﾌｧｲﾙｵｰﾌﾟﾝ処理
int		AP_Sgrec(int iFd, unsigned int iRecordSize, unsigned int iRecordNo, void *cBuffer);	// ﾌｧｲﾙ読込み処理1
int		AP_Sprec(int iFd, unsigned int iRecordSize, unsigned int iRecordNo, void *cBuffer);	// ﾌｧｲﾙ書込み処理1
int		AP_Asprec(int iFd, unsigned int iRecordsize, void *cBuffer);		// ﾌｧｲﾙ書込み処理2
int		AP_Sfclose(int iFd);												// ﾌｧｲﾙｸﾛｰｽﾞ処理
int 	AP_Diskfree(char *cDrv,unsigned long *ulHddFree);					// ﾃﾞｨｽｸ残容量ﾁｪｯｸ
void 	AP_Syslog(const char *__cBuffer, ... );								// ｼｽﾃﾑﾛｷﾞﾝｸﾞ処理
//
int		AP_MkdayfWt( char *cDir, WORD *wDateTime );							// 日付け管理ﾌｧｲﾙ書込み処理
int 	AP_MkdayfRd( char *cDir, char *cFileName );							// 日付け管理ﾌｧｲﾙ読込み処理
//
void	AP_Kyoyu( );														// 共有ﾒﾓﾘ使用宣言
void	AP_CommonAdr( char* adrs );

int strnatoe(const char* ascii,  char* ebcdic, int bytes);
int strnetoa(const char* ebcdic, char* ascii,  int bytes);
//
//************************************
// MODULE : 共通(TIMSUB)
//************************************
//
void	AP_Sgtim(WORD *wDate_Time);											// 時間取込処理
void	AP_Sptim(WORD *wDate_Time);											// 時間書込処理
																			// 日付加減算処理
int 	AP_Tcsas( WORD *wInDateTime, WORD *wTime, int iFlag, WORD *wOutDateTime );
int		AP_DateTimeCheck( WORD *iDate_Time );								// 日付時間ﾁｪｯｸ処理
//
#endif
