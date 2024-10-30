/***********************************************************************
*
*        CUSTOMER    :   新日本製鐡(株)殿
*        PROCESS     :   ﾍﾟｰﾊﾟﾚｽｼｽﾃﾑ
*        CPU NAME    :   Pentium Ⅱ 233MHz
*        MODULE      :   ﾌｧｲﾙ構造体
*
*        FILE NAME   :   AP_File.H
*
*        COMPUTER    :   DIGITAL PC
*        OS-TYPE     :   Windows NT Workstation  4.0
*        COMPILER    :   Visual C/C++ Ver5.0
*        PROGRAM     :   太平工業（株)
*        DATE        :   1998/12/03
*        PROGRAMER   :
*
*        VERSION     :   1.0 ...    /  /
*        REVISION    :   1.1 ...    /  /             by	
*
*
*
*
************************************************************************/
#ifndef	_AP_FILE_H
#define	_AP_FILE_H
//
#define		MAXDAY			180
//
#define		AP_RDONLY		1
#define		AP_WRONLY		2
#define		AP_APENDWR		3
#define		AP_CREATWR		4
#define		AP_RDWR			5
#define		AP_CREATRDWR	6
#define		WAIT_COUNT	12													// 排他ﾁｪｯｸｶｳﾝﾀ(1998/03/31追加)
#define		WAIT_TIMER	500													// 排他ﾁｪｯｸﾀｲﾏ(1998/03/31追加)
//
//-----------------------------------------------------------------------------
//	日付け管理ﾌｧｲﾙ 構造体
//-----------------------------------------------------------------------------
typedef struct {
			char cDate[9];													// 年､月､日
			char cTime[9];													// 時､分､秒
}MKDATE;
//
typedef struct {
			short	sOldpnt;												// 最古ﾎﾟｲﾝﾀ
			short	sNewpnt;												// 最新ﾎﾟｲﾝﾀ
			char 	cYobi[14];												// 予備
			MKDATE	sDate[MAXDAY];											// １２８日データ
}MKDAY;
//
typedef    struct
{
			int	iPrnt_f;													// ﾌﾟﾘﾝﾀ使用ﾌﾗｸﾞ
			int	iFile_f;													// ﾌｧｲﾙ参照中ﾌﾗｸﾞ
			int	iAbort;														// ｷｬﾝｾﾙﾌﾗｸﾞ
			int	iShow_f;													// 検索画面表示ﾌﾗｸﾞ
}GLBDAT;
//
#define    GLBDAT_SIZE       sizeof( GLBDAT )								// GLBDAT 1ﾚｺｰﾄﾞｻｲｽﾞ
//
#define    COMMON_SIZE_FS	(   (GLBDAT_SIZE)		\
							)												// 共有ﾒﾓﾘｰｻｲｽﾞ
//
typedef struct {
			char	cYear[4];												// 年
			char	cMonth[2];												// 月
			char	cDay[2];												// 日
			char	cSp1;													// スペース
			char	cHour[2];												// 時
			char	cMinute[2];												// 分
			char	cSecond[2];												// 秒
			char	cSp2[2];												// スペース
			char	cMessage[240];											// メッセージ
}RIREKI;

#endif
