/***********************************************************************
*
*        CUSTOMER    :   �V���{���d(��)�a
*        PROCESS     :   �߰��ڽ����
*        CPU NAME    :   Pentium �U 233MHz
*        MODULE      :   ̧�ٍ\����
*
*        FILE NAME   :   AP_File.H
*
*        COMPUTER    :   DIGITAL PC
*        OS-TYPE     :   Windows NT Workstation  4.0
*        COMPILER    :   Visual C/C++ Ver5.0
*        PROGRAM     :   �����H�Ɓi��)
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
#define		WAIT_COUNT	12													// �r����������(1998/03/31�ǉ�)
#define		WAIT_TIMER	500													// �r���������(1998/03/31�ǉ�)
//
//-----------------------------------------------------------------------------
//	���t���Ǘ�̧�� �\����
//-----------------------------------------------------------------------------
typedef struct {
			char cDate[9];													// �N������
			char cTime[9];													// �������b
}MKDATE;
//
typedef struct {
			short	sOldpnt;												// �Ō��߲��
			short	sNewpnt;												// �ŐV�߲��
			char 	cYobi[14];												// �\��
			MKDATE	sDate[MAXDAY];											// �P�Q�W���f�[�^
}MKDAY;
//
typedef    struct
{
			int	iPrnt_f;													// ������g�p�׸�
			int	iFile_f;													// ̧�َQ�ƒ��׸�
			int	iAbort;														// ��ݾ��׸�
			int	iShow_f;													// ������ʕ\���׸�
}GLBDAT;
//
#define    GLBDAT_SIZE       sizeof( GLBDAT )								// GLBDAT 1ں��޻���
//
#define    COMMON_SIZE_FS	(   (GLBDAT_SIZE)		\
							)												// ���L��ذ����
//
typedef struct {
			char	cYear[4];												// �N
			char	cMonth[2];												// ��
			char	cDay[2];												// ��
			char	cSp1;													// �X�y�[�X
			char	cHour[2];												// ��
			char	cMinute[2];												// ��
			char	cSecond[2];												// �b
			char	cSp2[2];												// �X�y�[�X
			char	cMessage[240];											// ���b�Z�[�W
}RIREKI;

#endif
