/***********************************************************************
*
*        CUSTOMER    :   �V���{���d(��)�a
*        PROCESS     :   �߰��ڽ����
*        CPU NAME    :   Pentium �U 233MHz
*        MODULE      :   ���ʻ��ͯ��
*
*        FILE NAME   :   AP_FUNC.H
*
*        COMPUTER    :   DIGITAL PC
*        OS-TYPE     :   Windows NT Workstation  4.0
*        COMPILER    :   Visual C/C++ Ver5.0
*        PROGRAM     :   �����H�Ɓi��)
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
//       ��  ��  ��  ��
//*********************************************************************
//
//************************************
// MODULE : ����(FILESUB)
//************************************
//
int		AP_Sfopen(char *cFname, int iType);									// ̧�ٵ���ݏ���
int		AP_Sgrec(int iFd, unsigned int iRecordSize, unsigned int iRecordNo, void *cBuffer);	// ̧�ٓǍ��ݏ���1
int		AP_Sprec(int iFd, unsigned int iRecordSize, unsigned int iRecordNo, void *cBuffer);	// ̧�ُ����ݏ���1
int		AP_Asprec(int iFd, unsigned int iRecordsize, void *cBuffer);		// ̧�ُ����ݏ���2
int		AP_Sfclose(int iFd);												// ̧�ٸ۰�ޏ���
int 	AP_Diskfree(char *cDrv,unsigned long *ulHddFree);					// �ި���c�e������
void 	AP_Syslog(const char *__cBuffer, ... );								// ����۷�ݸޏ���
//
int		AP_MkdayfWt( char *cDir, WORD *wDateTime );							// ���t���Ǘ�̧�ُ����ݏ���
int 	AP_MkdayfRd( char *cDir, char *cFileName );							// ���t���Ǘ�̧�ٓǍ��ݏ���
//
void	AP_Kyoyu( );														// ���L��؎g�p�錾
void	AP_CommonAdr( char* adrs );

int strnatoe(const char* ascii,  char* ebcdic, int bytes);
int strnetoa(const char* ebcdic, char* ascii,  int bytes);
//
//************************************
// MODULE : ����(TIMSUB)
//************************************
//
void	AP_Sgtim(WORD *wDate_Time);											// ���Ԏ捞����
void	AP_Sptim(WORD *wDate_Time);											// ���ԏ�������
																			// ���t�����Z����
int 	AP_Tcsas( WORD *wInDateTime, WORD *wTime, int iFlag, WORD *wOutDateTime );
int		AP_DateTimeCheck( WORD *iDate_Time );								// ���t������������
//
#endif
