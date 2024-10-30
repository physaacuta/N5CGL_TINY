// *********************************************************************************
//	�O���C���^�[�t�F�[�X�p�w�b�_�[
//	[Ver]
//		Ver.01    2015/06/18  ���ō쐬
//
//	[����]
//		��{�I�ɁA����������
//
// *********************************************************************************

// �X�̃V�X�e���ŗL�̃C���N���[�h

#ifndef	_OUTINTERFACE_H
#define	_OUTINTERFACE_H

// �ӃփC���N���[�h
#include "KizuCommon.h"								// �Ӄ֋��ʃw�b�_�[

//***************************************************************
// �ݒ�ϒ萔 ��`
//***************************************************************

//***************************************************************
// �v���R���ʐM�@��`
//***************************************************************
//=======================================
// �v���R�� �萔
//=======================================
//#define PRO_SIZE_TC					4					// �g�����U�N�V�����R�[�h MID
#define PRO_MAX_CUT_COUNT			16					// �ő�J�b�g��
#define PRO_SIZE_COIL_NO			12					// �R�C��No (�����A�o������)
#define PRO_SIZE_ORDER_NO			4					// ����No

#define PRO_SIZE_NEXTINF_CNT		4					// ���R�C����񌏐�

// �g�����U�N�V�����R�[�h
#define	PRO_TC_COIL					1000				// �R�C�����`��
#define	PRO_TC_SAISUN				1001				// �̐����ѓ`��
#define	PRO_TC_CUT					1002				// �J�b�g���ѓ`��
#define	PRO_TC_INIT					1003				// �C�j�V�����`��
#define	PRO_TC_ZISEKI				2000				// �r�������ѓ`��
//#define	PRO_TC_HELS					3000				// �w���V�[�`��
#define	PRO_TC_HELS					-1					// �w���V�[�`��

// �����敪
#define	PRO_KUBUN_NOMAL				0					// �Ɩ��`��
#define PRO_KUBUN_ANSER				1					// �A���T�[�`��

// �O�����g�t���O
#define PRO_GRAND_OK				0					// �`���̒��g ����
#define PRO_GRAND_NG				1					// �`���̒��g �ُ�

// �v���R���`���T�u�w�b�_�[ [0]
static const BYTE	PRO_HEAD_H_GYOUMU	= 0x60;			// �Ɩ��`�� ���� �����`��

// �v���R���`���T�u�w�b�_�[ [1]
static const BYTE	PRO_HEAD_L_OK		= 0x00;			// ����

// TC����̒���		�� ���b�Z�[�WNo����̒���
// ���[�h�P�� (�w�b�_�[���܂܂Ȃ�) ��Csemi�Ή�
#define	PRO_TYPE_LENGTH(size)			((size-4)/2)		// �`���̃f�[�^���ɃZ�b�g����l
#define	PRO_TYPE_LENGTH_ORIG(size)		(size*2+4)			// �`���̃f�[�^������{���̃T�C�Y�ɕϊ�
// �o�C�g�P�� (�w�b�_�[���܂�)
//#define	PRO_TYPE_LENGTH(size)			(size)			// �`���̃f�[�^���ɃZ�b�g����l
//#define	PRO_TYPE_LENGTH_ORIG(size)		(size)			// �`���̃f�[�^������{���̃T�C�Y�ɕϊ�

//=======================================
// �v���R�� �\����
//=======================================

#pragma pack(push, 2)

//// ���X�|���X�`�� (2byte)
struct PRO_TYPE_RESP
{
	BYTE			bHead[2];							// �T�u�w�b�_�[
};

//// �w�b�_�[�� (8byte)
struct PRO_TYPE_HEAD
{
	BYTE			bHead[2];							// �T�u�w�b�_�[ (�z���g�͂����ɓ��ꂽ���Ȃ��������� long�������Ă���̂Ńo�E���_�����Y����)

	short			nLen;								// �f�[�^�� (byte�P��)
	long			nTC;								// ���b�Z�[�WNo
};

//// �f�[�^��

//// ���ʏ�� (56byte)
struct PRO_TYPE_COMMON
{
	short			nKubun;								// �`���敪 (0:�Ɩ��`�� 1:�����`��)
	short			nDate[6];							// ���M���� (�N���������b)
	short			nGrandFlg;							// �O�����g�t���O (0:�`���̒��g ���� 1:�`���̒��g �ُ�)
	char			cYobi[40];
};

struct PRO_TYPE_KEY
{
	char			cMeiNo[PRO_SIZE_ORDER_NO];			// ����No
	char			cCoilNo[PRO_SIZE_COIL_NO];			// �L�[��� �R�C��No

};

// �C�j�V�����`��
struct PRO_TYPE_DATA_INIT
{
	PRO_TYPE_COMMON		Common;							// ���ʏ��

	char				cYobi[64];						// �\��
};

// �R�C�����`��
struct PRO_TYPE_COIL
{
	PRO_TYPE_KEY		KeyInf;							// �L�[���
	long				nSendPoint;						// ���M�ʒu
	long				nLenIn;							// �����_���R�C�� ���� [m]
	long				nAtuZai;						// �ޗ��� [��m] (����)
	long				nWidZai;						// �ޗ��� [mm]
	long				nAtuSei;						// ������ [��m] (�o��)
	long				nWidSei;						// ������ [mm]
	long				nWidTrim;						// ���H���g������ [mm] (�m�[�g�������́A0)
	long				nNobiSPM;						// SPM�L�ї� [10-2%]
	long				nNobiTLV;						// TLV�L�ї� [10-2%]
	long				nCutF;							// �����ɂ�����t�����g�[�J�b�g�̎��ђ�[mm]
	long				nCutT;							// �����ɂ�����e�[���[�J�b�g�̎��ђ�[mm]
	long				nSPMRoolKei_Up;					// SPM���[�N���[���a (��) [10-1mm]
	long				nSPMRoolKei_Down;				// SPM���[�N���[���a (��) [10-1mm]
	long				nYobi[31];

	char				cKenKijun_O[4];					// ����� �\
	char				cKenKijun_U[4];					// ����� ��
	char				cKurometo_Kind[2];				// �N�����[�g ���ю��
	char				cKurometo_Umu[2];				// �N�����[�g ���їL��
	char				cHinCode[1];					// �i��
	char				cMekkiKind[1];					// ���b�L���
	char				cSyuZai[2];						// ��ގ�
	char				cDifMaker[1];					// �f�B�t�@�����V�����}�[�J�[
	char				cKenban[1];						// ���敪
	char				cMekkiCycle[1];					// ���b�L�T�C�N��
	char				cInLineSPM[1];					// ���x���[ �C�����C��SPM
	char				cKagakuToyu[1];					// ���w�����h��
	char				cAtukkaSPM[1];					// �I�t���C��SPM������
	char				cMekkiKubun[1];					// ���b�L�敪
	char				cOmoteSyori1[1];				// �\�ʏ���1����
	char				cOmoteSyori2[1];				// �\�ʏ���2����
	char				cToyu[1];						// �h��
	char				cKouteiYobi[1];					// �H���L���\��
	char				cBaund[1];						// �o�E���_�����킹
	char				cCaplSyuZai[10];				// CAPL ��ގ�
	char				cMetuke[8];						// �ڕt�K�i
	char				cNi[2];							// Ni�t����
	char				cSodoTCM[2];					// TCM���[���e�x
	char				cSpmUmu[2];						// SPM�����L�� (�L��:O  ����:X)
	char				cNextLine[2];					// �v�掟�H���R�[�h
	char				cMuke[32];						// ����
	char				cYouto[4];						// �p�r
	char				cCaplSiage[4];					// CAPL�\�ʎd�グ
	char				cCaplSanUmu[2];					// CAPL�_���L��
	char				cHinMode[1];					// �i��敪
	
	char				cGaCrKubun[1];					// GA/CR�敪
	char				cSpmJisekiUmu[2];				// SPM�������їL���i�L��F0 ����:X�j
	char				cKouteiCode[2];					// �H���R�[�h
	char				cYobi1[42];
};

// �����p�R�C�����`��
struct PRO_TYPE_DATA_COILINF
{
	PRO_TYPE_COMMON		Common;							// ���ʏ��

	PRO_TYPE_COIL		CoilInf;						// �R�C�����

	// ���R�C���ȍ~�̏��
	struct NEXT_COIL {
		PRO_TYPE_KEY	KeyInf;							// �L�[���
		
		long			nFrontLen;						// �����t�����g�[���� [mm]
		char			cKouteiCode[2];					// ���H���R�[�h
		char			cYobi[2];

	} next_coil[PRO_SIZE_NEXTINF_CNT];
	char				cYobi[16];						// �\��
};

// �\���p�R�C�����`��
typedef PRO_TYPE_DATA_COILINF PRO_TYPE_DATA_COILDSP;

// �̐����ѓ`��
struct PRO_TYPE_DATA_SAISUN
{
	PRO_TYPE_COMMON		Common;							// ���ʏ��

	PRO_TYPE_KEY		KeyInf;							// �L�[���

	long			nLen;								// ���уR�C����[0.1m]
	long			nMakiLenF;							// �t�����g�������܂꒷��[0.1m]
	long			nSetCnt;							// �Z�b�g�����̐���
	long			nYobi[9];

	struct _SAISUN_INF{
		long		nStart;								// �̐��J�n�ʒu[0.1m]
		long		nEnd;								// �̐��I���ʒu[0.1m]
		long		nYobi[2];
	} saisun[PRO_MAX_CUT_COUNT];
	char			cYobi[128];							// �\��
};

// �J�b�g����
struct PRO_TYPE_DATA_CUT
{
	PRO_TYPE_COMMON		Common;							// ���ʏ��

	PRO_TYPE_KEY		KeyInf;							// �L�[���
	
	long			nMode;								// ���ы敪 (0:�n�ړ_�ʉ� 1:F�J�b�g)
	long			nStart;								// �̐��J�n�ʒu[0.1m]
	long			nEnd;								// �̐��I���ʒu[0.1m]
	long			nKoban;								// ���� (1CAPL�̂�)
	char			cYobi[32];
};

// ��������
struct PRO_TYPE_DATA_ZISEKI
{
	PRO_TYPE_COMMON		Common;							// ���ʏ��

	long			nZiMode;							// ���ы敪 (0:���� 1:�ُ�(�J�b�g���тŎ�M�����Y���R�C������) )
	long			nKikiMode;							// ���W�敪 (0:���� 1:�ُ�)

	PRO_TYPE_KEY	KeyInf;								// �L�[���
	long			nKoban;								// ���� (1CAPL�̂� �J�b�g���т̏��Ԃ����̂܂܃Z�b�g����)
	char			cYobi1[36];

	// �O���[�h 1�`6�����r���������
	struct GRADE_INF {
		short		nMixedRate;							// ������ [10-2%]
		short		nYobi1[2];
		short		nTypeCnt[10];						// �r�� (�r��1�`10)
		short		nYobi2[3];
	} grade_inf[6];

	char			cYobi[192];
};

//// �w���V�[�`��
//struct PRO_TYPE_DATA_HELS
//{
//	char			cYobi[64];
//};

//=======================================
// �v���R�� �`��������
//=======================================
// �C�j�V�����`��
struct PRO_TYPE_INIT
{
	PRO_TYPE_HEAD			head;
	PRO_TYPE_DATA_INIT		data;
};
// �R�C�����`��
struct PRO_TYPE_COILINF
{
	PRO_TYPE_HEAD			head;
	PRO_TYPE_DATA_COILINF	data;
};
// �̐����ѓ`��
struct PRO_TYPE_SAISUN
{
	PRO_TYPE_HEAD			head;
	PRO_TYPE_DATA_SAISUN	data;
};
// �J�b�g���ѓ`��
struct PRO_TYPE_CUT
{
	PRO_TYPE_HEAD			head;
	PRO_TYPE_DATA_CUT		data;
};
// �r�������ѓ`��
struct PRO_TYPE_ZISEKI
{
	PRO_TYPE_HEAD			head;
	PRO_TYPE_DATA_ZISEKI	data;
};
// �w���V�[�`��
struct PRO_TYPE_HEALTHY{
	PRO_TYPE_HEAD			head;
};

#pragma pack(pop)

//***************************************************************
// FF�ʐM�@��`
//***************************************************************
//=======================================
// FF�ʐM �萔
//=======================================
#define FFSERV_OFFLINE		// �{�Ԃł́A�R�����g�ɂ��邱��(FF�T�[�o�[�����p)

#ifndef FFSERV_OFFLINE			// ---------------�{�Ԍn---------------

//// FF�`�ǃR�}���h
#define FF_DENKAN_NO			"TE"					// 

//// �g�����U�N�V�����R�[�h

// �������ѓ`���̓`���R�[�h�@����~
#define FF_TC_CODE_JI_O			"85"					// �\
#define FF_TC_CODE_JI_U			"86"					// ��

// �����`���̓`���R�[�h�@����~
#define FF_TC_CODE_ANS_O		"8G"					// �\
#define FF_TC_CODE_ANS_U		"8H"					// ��

// �������ѓ`���̓`���R�[�h�@����~
#define FF_TC_CODE_STSJI_O		"95"					// �\
#define FF_TC_CODE_STSJI_U		"96"					// ��

// �����`���̓`���R�[�h�@����~
#define FF_TC_CODE_STSANS_O		"9G"					// �\
#define FF_TC_CODE_STSANS_U		"9H"					// ��

// �O�H���r���v���`���̓`���R�[�h
#define FF_TC_CODE_MAEREQ		"A4"					// 

// �O�H���r���̓`���R�[�h
#define FF_TC_CODE_MAEJI_O		"C4"					// �\
#define FF_TC_CODE_MAEJI_U		"D4"					// ��

// �w���V�[�̓`���R�[�h
#define FF_TC_CODE_HELCY		"ZZ"					// �w���V�[�`��

#else							// ---------------�����n---------------

//// FF�`�ǃR�}���h
#define FF_DENKAN_NO			"TT"					// 


//// �g�����U�N�V�����R�[�h

// �������ѓ`���̓`���R�[�h
#define FF_TC_CODE_JI_O			"TU"					// �\
#define FF_TC_CODE_JI_U			"TV"					// ��

// �����`���̓`���R�[�h
#define FF_TC_CODE_ANS_O		"TW"					// �\
#define FF_TC_CODE_ANS_U		"TX"					// ��

// ������ԓ`���̓`���R�[�h
#define FF_TC_CODE_STSJI_O		"TM"					// �\
#define FF_TC_CODE_STSJI_U		"TN"					// ��

// ������ԉ����`���̓`���R�[�h
#define FF_TC_CODE_STSANS_O		"TP"					// �\
#define FF_TC_CODE_STSANS_U		"TQ"					// ��

// �O�H���r���v���`���̓`���R�[�h
#define FF_TC_CODE_MAEREQ		"TR"					// 

// �O�H���r���̓`���R�[�h
#define FF_TC_CODE_MAEJI_O		"C4"					// �\
#define FF_TC_CODE_MAEJI_U		"D4"					// ��

// �w���V�[�̓`���R�[�h
#define FF_TC_CODE_HELCY		"ZZ"					// �w���V�[�`��

#endif

//=======================================
// FF �ʐM�n�X�e�[�^�X�@�萔��`
//=======================================

// FF�����`�� �������ʁ@�萔
#define FF_ANS_OK						"00"			// ����
#define FF_ANS_IPCHK_NG					"01"			// �C���v�b�g�`�F�b�NNG
#define FF_ANS_DBERRER					"03"			// DB�o�^�ُ�

//=======================================
// FF�ʐM�n�@�Œ�T�C�Y
//=======================================
#define MAX_FF_DFCT_CNT					1800			// �e�e�������ѓ`���r���
#define MAX_FF_STS_CNT					30000			// �e�e������ԓ`���r���

#define MAX_FF_MAE_DINF_CNT				7				// �e�e�O�H���r���ѓ`�� �r���o���

#define MAX_FF_MAE_DEF_CNT				2000			// �e�e�O�H���r���ѓ`�� (�S��)
//#define MAX_FF_MAE_CC_CNT				10				// �e�e�O�H���r���ѓ`�� �b�b�r���
//#define MAX_FF_MAE_HOT_CNT				50				// �e�e�O�H���r���ѓ`�� �g�n�s�r���
//#define MAX_FF_MAE_PL_CNT				80				// �e�e�O�H���r���ѓ`�� �o�k�r���
//#define MAX_FF_MAE_TCM_ANA_CNT			30				// �e�e�O�H���r���ѓ`�� �s�b�l�����
//#define MAX_FF_MAE_YOBI_CNT				30				// �e�e�O�H���r���ѓ`�� �r���\��
//#define MAX_FF_MAE_MAE_CNT				1800			// �e�e�O�H���r���ѓ`�� �����r���
// 20221021 2TCM���@���ɔ������H������ ���� --->>>
#define	MAX_FF_MAE_N2TCM_KIZU_CNT		900				// �e�e�O�H���r���ѓ`�� �s�b�l���iN2TCM���@�j
// 20221021 2TCM���@���ɔ������H������ ���� ---<<<

#define MAX_FF_MAE_THAN_CNT				10				// �e�e�O�H���r���ѓ`�� �ʔ��я��
#define MAX_FF_MAE_BNKT_CNT				20				// �e�e�O�H���r���ѓ`�� �������

//=======================================
// �\���́i��FF�T�[�o�[�`���j
//=======================================
//// �������ѓ`��
struct FF_TYPE_HEAD {
	char			Header_Code[2];						// �`���R�[�h ��������[83/84] �������[95/96]
	char			Rec_Code[2];						// �A��(00)
	char			Date[14];							// �`���������� �`�� YYYYMMDHHNNSS
	char			Line_Code[2];						// �H���R�[�h [5CGL: 85]
	char			Meino[4];							// ���ߔԍ�
	char			Coilno[10];							// �≄�R�C��No
};

struct FF_TYPE_DATA {
	char			Kizukbn[1];							// �r���o�敪 0:�Ӄ�
	char			Kind[2];							// �r��
	char			Grade[2];							// �r�O���[�h
	char			WidPos[4];							// �r�Е����ʒu
	char			Len[4];								// �r����
	char			DefectPos[6];						// �r�����������ʒu
	char			kbn[1];								// �r�敪
	char			No[4];								// �rNO (�r�ȊO�́A4��0�ݒ�)
	char			OpeDsp[1];							// �I�y�K�C�\���t���O
	char			SysKbn[1];							// �V�X�e���敪
	char			Filler[6];							// �\��
};

struct FF_JISSEKI {										// �{��
	FF_TYPE_HEAD	Hd;									// �`���w�b�_�[
	char			CoilLen[6];							// �R�C������ [10-1m]
	char			Mode[2];							// �����������[�h (0:���ʏk�ށA1:���ʐ���A2:���ʏk�ށA3:�\�ʏk�ށA4:�r����~�A5:�G�b�W�����o��ԗL��)
	char			OverStatus[2];						// �r���I�[�o�[�X�e�[�^�X (2:1800�����ް���A1:1800���ȓ���)
	char			Id[20];								// �Ǘ�NO
	char			DataDefault[4];						// �f�[�^�ݒ萔
	char			cYobi1[32];							// �\��
	FF_TYPE_DATA	Dt[MAX_FF_DFCT_CNT];				// �r���		1800��
	FF_TYPE_DATA	Dtyobi[2000-MAX_FF_DFCT_CNT];		// �\���r��� 200��
	char			cYobi[900];							// �\��
};

//// �����`��
struct FF_ANS_HD {
	char			Header_Code[2];						// �`���R�[�h [M7/M8]
	char			Rec_Code[2];						// �A��(00)
	char			Date[14];							// �`���������� �`�� YYYYMMDHHNNSS
	char			Line_Code[2];						// �H���R�[�h [CTL:A1]
	char			Meino[4];							// ���ߔԍ�
	char			Coilno[10];							// �����R�C���ԍ�
};

struct FF_ANS_DATA {
	char			Status[2];							// �������� (00:OK, 01:INPUT CHACK NG 03:DB�o�^�ُ�)
};

struct FF_ANS {
	FF_ANS_HD		Hd;									// �`���w�b�_�[
	FF_ANS_DATA		Data;								// �f�[�^
	char			cYobi[64964];						// �\��
};

//// �O�H���r���їv���`��
struct FF_MAE_REQUEST {
	char			HeaderCode[2];						// �`���R�[�h
	char			Rec_Code[2];						// �A�� ("00"�Œ�)
	char			DateTime[14];						// �`���������� �`�� YYYYMMDDHHNNSS
	char			Line_Code[2];						// �H���R�[�h  (2PL=12 3PL=13 4PL=14)
	char			MeireiNo[4];						// ���ߔԍ��S��('�O'�{���ߔԍ��R��)���E�l�ݒ�
	char			Filler[64976];						// �\�� ��(0x20)
};

// �O�H���r���ѓ`��
// �`���w�b�_�[
struct FF_MAE_TYPE_HEAD {
	char			Header_Code[2];						// �`���R�[�h (??:�\, ??:��)
	char			Rec_Code[2];						// �A�� ("00"�Œ�)
	char			DateTime[14];						// �`���������� �`�� YYYYMMDDHHNNSS
	char			Line_Code[2];						// �H���R�[�h  (5CGL=85)
	char			Meino[4];							// ���ߔԍ�
	char			O_UKbn[2];							// �\���敪(1:�\, 2:��)
	char			ReiCoilNo[10];						// �≄�R�C���ԍ�
	char			ReiCoilG[5];						// �≄�R�C���d�� [10-3t]
	char			ReiCoilL[6];						// �≄�R�C������ [10-1m]
	char			ReiCoilAtu[5];						// �≄�� [10-3mm]
	char			ItaatuMin[5];						// ���@�����@MIN [10-3mm]
	char			ItaatuMax[5];						// ���@�����@MAX [10-3mm]

	char			Mukesaki1i[5];						// ������P�@�� [10-1mm]
	char			Kensa[4];							// �����
	char			Maki[1];							// ��������
	char			Zairyou[5];							// �ޗ��� [10-1mm]
	char			Mukesaki1kMin[5];					// ������P�@�Ќ����@MIN [10-1mm]
	char			Mukesaki1kMax[5];					// ������P�@�Ќ����@MAX [10-1mm]
	char			Mukesaki1L[5];						// ������P�@����(���b�h����) [m]
	char			Mukesaki1N[3];						// ������P�@���a [10-1���]
	char			Itaatu[5];							// �� [10-3mm]
	char			Irigawa[3];							// �������a [10-1���]
	char			Mukesaki1KL[5];						// ������P�@���f�� [10-1mm]
	char			Makoutei[2];						// �O�H���R�[�h
	char			RealMakoutei[2];					// ���O�H���R�[�h
	char			TCMFlag[1];							// ����/TCM�����ރt���O
};

struct FF_MAE_TYPE_INF {
	char			Kizukbn[2];							// �r���o�H��
	char			Id[20];								// �Ǘ�NO
	char			Mode[2];							// �����������[�h
	char			DataDefault[4];						// �r�ݒ��
	char			OverStatus[2];						// �r���I�[�o�[�X�e�[�^�X
};

struct FF_MAE_TYPE_VRCLINF {
	char			KizukenNo[20];						// VRCL�Ǘ�NO
};

struct FF_MAE_TYPE_FROMTO {
	char			CoilNo[10];							// �≄�R�C��No
	char			From[6];							// �̎�J�n�ʒu�����iFrom�j	[0.1m]
	char			To[6];								// �̎�I���ʒu�����iTo�j	[0.1m]
	char			Koban[2];							// �̎�q��
};

struct FF_MAE_TYPE_DATA {
	char			Kind[2];							// �r��
	char			Grade[2];							// �r�O���[�h
	char			Kizukbn[2];							// �r���o�H��
	char			kbn[1];								// �r�敪
	char			DefectPos[6];						// �r���������ʒu [10-1m]
	char			WidPos[4];							// �r�Е����ʒu [mm]
	char			Len[4];								// �r���� [10-1m]
	char			No[4];								// �rNO
	char			SysKbn[1];							// �V�X�e���敪
	char			Filler[4];							// �\��
};

struct FF_MAE_TYPE_BUNKATSU {
	char			MakiHoukou[1];						// ���ъ����� (1:��o�㌞, 2:��o����, 3:���o�㌞, 4:���o����)
	char			CoilLen[6];							// �R�C����
	char			Cutkbn[1];							// �J�b�g�ꏊ�敪 (I:�����J�b�g, O:�o���J�b�g)
	char			GenpinNo[10];						// �������i�Ǘ�NO
	char			Filler[2];							// �\��
};

struct FF_MAE_TYPE_THUHAN {
	char					JissekiCode[2];				// ���эH���R�[�h
	char					MeireiNo[4];				// ���ߔԍ�
	char					ThuhanCnt[1];				// �ʔ�
	char					GenpinNo[10];				// �������i�Ǘ�NO
	char					CoilLen[6];					// �����R�C������
	char					Yobi[2];					// �\��
	FF_MAE_TYPE_BUNKATSU	Bu[MAX_FF_MAE_BNKT_CNT];	// �������
};


// �O�H���r���`���{��
struct FF_MAE_JISSEKI {
	FF_MAE_TYPE_HEAD	Hd;								// �w�b�_�[��
	char				Yobi1[27];						// �\��
	FF_MAE_TYPE_INF		Inf[MAX_FF_MAE_DINF_CNT];		// �r���o���
	char				Yobi2[90];						// �\��
	FF_MAE_TYPE_VRCLINF	VRcl;							// VRCL���
	FF_MAE_TYPE_FROMTO	FromTo;							// From-To
	char				Yobi3[116];						// �\��

	FF_MAE_TYPE_DATA	def[MAX_FF_MAE_DEF_CNT];		// �r��� (�w�b�_�[���r���o�H�� �� �X���r���r���o�H�� �� �S������ׂĕR�t����)

	FF_MAE_TYPE_THUHAN	Th[MAX_FF_MAE_THAN_CNT];		// �ʔ��я��

	char				Yobi[150];						// �\��
};

// �r���o�H�� �w�b�_�[�� �z��INDEX
enum EM_DIV_PROC_KIND_INDEX {
	DIV_PROC_KIND_INDEX_CC = 0,							// [�O�H�����z��INDEX] CC
	DIV_PROC_KIND_INDEX_HOT,							// [�O�H�����z��INDEX] HOT
	DIV_PROC_KIND_INDEX_PL,								// [�O�H�����z��INDEX] PL
	DIV_PROC_KIND_INDEX_TCM_ANA,						// [�O�H�����z��INDEX] TCM����

	DIV_PROC_KIND_INDEX_CGL,							// [�O�H�����z��INDEX] ����n�r��
	// 20221021 2TCM���@���ɔ������H������ ���� --->>>
	//DIV_PROC_KIND_INDEX_FLL2,							// [�O�H�����z��INDEX] FLL���~���r��
	DIV_PROC_KIND_INDEX_TCM_KIZU,						// [�O�H�����z��INDEX] TCM����
	// 20221021 2TCM���@���ɔ������H������ ���� ---<<<
	DIV_PROC_KIND_INDEX_FLL3							// [�O�H�����z��INDEX] FLL�o���r��
};


// ������ԓ`��
struct FF_STSTYPE_HEAD
{
	char			Header_Code[2];						// �`���R�[�h �\:95 ��:96
	char			Rec_Code[2];						// �A��(00)
	char			Date[14];							// �`���������� �`�� YYYYMMDHHNNSS
	char			Line_Code[2];						// �r�����C���̍H���R�[�h['81','85','54','82(53)']�ݒ�
	char			Meino[4];							// ���ߔԍ�
	char			Coilno[10];							// �≄�R�C���ԍ�
};

struct FF_KENSA_STATUS {
	char			status[1];							// �������(1m�Ԃ̌�����Ԃ�1byte�Ɋi�[(���30000m))
};

struct FF_STSTYPE_DATA {
	char			Id[20];								// �r���Ǘ�No
	char			dataSize[5];						// ������ԃZ�b�g���F������Ԃ��i�[����byte�������E�l�ݒ�(�O����)
	char			yobi1[2];							// �\���P�F������ԏ��̌����H���R�[�h��ݒ�
	char			yobi2[1];							// �\���Q�FFF���̔��f������Ԃ��Z�b�g("0":���� / "1":�r������Y�����F��)
	char			yobi[38];							// �\��
	FF_KENSA_STATUS data[MAX_FF_STS_CNT];				// �������(1m�Ԃ̌�����Ԃ�1byte�Ɋi�[(���30000m))
};

struct FF_STS_JISSEKI {									// �{��
	FF_STSTYPE_HEAD		Hd;								// �`���w�b�_�[
	FF_STSTYPE_DATA		Dt;								// �f�[�^
	char				Filler2[34900];					// �\��
};

enum { FF_STSREC_SIZE = sizeof(FF_STS_JISSEKI) };		// ������ԓ`�� �`����

// �����`��
struct FF_STSANS_HD {
	char			Header_Code[2];						// �`���R�[�h �\:9G ��:9H
	char			Rec_Code[2];						// �A��(00)
	char			Date[14];							// �`���������� �`�� YYYYMMDHHNNSS
	char			Line_Code[2];						// �r�����C���̍H���R�[�h['81','85','54','82(53)']�ݒ�
	char			Meino[4];							// ���ߔԍ�
	char			Coilno[10];							// �����R�C���ԍ�
};

struct FF_STSANS_DATA {
	char			Status[2];							// �������� 00:OK, 01:INPUT CHACK NG 03:DB�o�^�ُ�
};

struct FF_STSANS {
	FF_STSANS_HD	Hd;									// �`���w�b�_�[
	FF_STSANS_DATA	Data;								// �f�[�^
	char			Filler[64964];						// �\��
};
enum { FF_STSANS_REC_SIZE = sizeof(FF_STSANS) };		// ������ԁ@�����`���@�`����


//// �w���V�[�`�����w���V�[�����`��
struct FF_HELCY {
	char			Header_Code[2];						// �`���R�[�h (ZZ)
	char			Rec_Code[2];						// �A�� (FF)
	char			DateTime[14];						// �`���������� �`�� YYYYMMDDHHNNSS
	char			Line_Code[2];						// �H���R�[�h [A1]
	char			Denkan_No[2];						// �`�ǃR�}���hNo [AA]
	char			Yobi[64978];						// �\��
};

//#endif


//***************************************************************
// �V�[�P���T�ʐM�@��`
//**************************************************************
// SEQ �W�� (UDP)
#define SEQ_SIZE_HEAD_BASE			4					// �V�[�P���T�w�b�_�[�T�C�Y	
#define SEQ_SIZE_HEAD				8					// �V�[�P���T�w�b�_�[�T�C�Y	

// SEQ�w�b�_�[�p
//	�W���F���[�h�P�ʂŃ��b�Z�[�W������̒���
#define	SEQ_TYPE_LENGTH(size)		((size-4)/2)		// �`���̃f�[�^���ɃZ�b�g����l			(�{�����`����)
#define	SEQ_TYPE_LENGTH_ORIG(size)	(size*2+4)			// �`���̃f�[�^������{���̃T�C�Y�ɕϊ� (�`�������{��)

//   CSEMI�̂悤�Ɏg���ꍇ�B���[�h�P�ʂŃf�[�^������̒���
//#define	SEQ_SEMI_TYPE_LENGTH(size)		(((size-4)/2)-2)	// �`���̃f�[�^���ɃZ�b�g����l			(�{�����`����)
//#define	SEQ_SEMI_TYPE_LENGTH_ORIG(size)	((size*2+4))		// �`���̃f�[�^������{���̃T�C�Y�ɕϊ�	(�`�������{��)



// TC
#define	SEQ_TC_HELCY				-1					// �w���V�[�`��
#define	SEQ_TC_FRAME				1					// �t���[�����`��

// ���̑�
#define SEQ_UDP_PORT				51000				// UDP��M�|�[�g


#pragma pack(push, 1)

//// ���X�|���X�`�� (2byte)
struct SEQ_TYPE_RESP
{
	BYTE			bHead[2];						// �T�u�w�b�_�[
};

//// �{���̃V�[�P���T�w�b�_�[(4byte)
struct SEQ_TYPE_HEAD_BASE : public SEQ_TYPE_RESP
{
	short			nSeqLen;						// �f�[�^�� (���[�h�P��)
};

//// ����̃V�[�P���T�w�b�_�[(8byte)	���g���₷���悤��CSEMI�w�b�_�[�ƍ��킹��
struct SEQ_TYPE_HEAD : public SEQ_TYPE_HEAD_BASE
{
	int				nTC;							// �g�����U�N�V�����R�[�h
};

#pragma pack(pop)


//=======================================
// �V�[�P���T �\����
#pragma pack(push, 1)
// �t���[�����
struct SEQ_TYPE_DATA_FRAME {
	DWORD					nFrameNo;				// �t���[��No �����J�n���Ă���̘A�� (0�I���W��) ������t���[����0
	DWORD					nPlgDiff;				// PLG���� �O�t���[������̍��� [pls]
	DWORD					nTimeDiff;				// ���ԍ��� [10us]

	DWORD					nExSyncAdd;				// ExSync�ώZ�l [pls]
	DWORD					nPlgAdd;				// PLG�ώZ�l [pls]
	DWORD					nTimeAdd;				// ���ԐώZ�l [10us]

	int						nWpdFramePos;			// WPD�I�����̃t���[����ExSync�ʒu (-1:����  0�`:�����pWPD�L��t���[��)
	BYTE					yobi[28];
};

//=======================================
// �`��������

// �t���[�����
struct SEQ_TYPE_FRAME
{
	SEQ_TYPE_HEAD			head;
	SEQ_TYPE_DATA_FRAME		data;
};
#pragma pack(pop)

// �V�[�P���T�f�[�^������

// 1�b�p���X�o�͖���   ��EM_EVENT_PLS_OUT �ŃA�N�Z�X
static const char* pls_out_name[] = {
	{ "�������w��" },	//0
	{ "�J�n�w��" },	//1
	{ "" },			//2
	{ "" },			//3
	{ "" },			//4
	{ "" },			//5
	{ "�^��WPD�����w��" }	//6
};


#pragma pack(push, 2)
//=========================================
// �p���X�o�� �^�C�~���O
enum EM_EVENT_PLS_OUT {
	EVENT_PLS_OUT_INIT = 0,										// �������w��
	EVENT_PLS_OUT_START = 1,										// �J�n�w��

	EVENT_PLS_OUT_TEST_WPD = 6,									// WPD�M���f�o�b�N�p
	EVENT_PLS_OUT_END = 7
};


//=========================================
// ��ԕ\���p  ���f�[�^�������o�͂ɂ��g�p
struct TYP_INF_KEN
{
	char			cKeyNo[20];					// KeyNo
	int				nPlsR;										// �p���X���[�g [10-6]
	int				nLen;										// �ʔ��� [10-1m]
	int				nSpeed;										// ���x [10-1mpm]
	WORD			nYobi[4];
};
struct TYP_INF_DSP
{
	char			cKeyNo[20];					// KeyNo
	int				nPlsR;										// �p���X���[�g [10-6]
	int				nLen;										// �ʔ��� [10-1m]
	int				nSpeed;										// ���x [10-1mpm]
	WORD			nYobi[4];
};




//***************************************************************
// ����V�[�P���T�ʐM(MxComponent�ʐM)�@��`
//***************************************************************
//=======================================
// ����V�[�P���T�ʐM(MxComponent�ʐM) �萔
//=======================================
static const int HB_MAX_VAL = 9999;								// ���C�u�J�E���^�ő�l

																// ���΃v���R��I/F�� ��M�`����� (�� �f�[�^�������z�u��)
//=========================================
// �f�[�^�������\����
// ��M�\����	��MAX256byte
struct SEQ_DM_RECV_DEFECT {
	// ��{
	WORD				nHBCnt;										// �V�[�P���T �w���V�[	5s�ω������ŃV�[�P���T�ُ�
	WORD				nSeqSt;										// �V�[�P���T ��� (0:���� 1:���� 2:�ُ�)
	WORD				nSeqKadou;									// �V�[�P���T �ғ���ԁi0:���Z�b�g�ς� 1:�������ς� 2:�\�� 3:�B���J�n�j
	WORD				nYobi1[5];

	WORD				nPls;										// �����pPLG �p���X���[�g
	WORD				nPlsSub;									// ������r�pPLG �p���X���[�g
	WORD				nPlsDsp;									// �\���pPLG �p���X���[�g
	WORD				nYobi2[5];

	DWORD				nLength;									// �����pPLG ��������
	DWORD				nLengthSub;									// ������r�pPLG ��������
	DWORD				nLengthDsp;									// �\���pPLG ��������
	DWORD				nWpdOnLength;								// WPD�M��ON���̌����pPLG ��������
	DWORD				nWpdOnLengthSub;							// WPD�M��ON���̌�����r�pPLG ��������
	DWORD				nWpdOnLengthDsp;							// WPD�M��ON���̕\���pPLG ��������
	WORD				nYobi3[4];
};
struct SEQ_DM_RECV_BASE : public SEQ_DM_RECV_DEFECT
{

	// �����p
	struct TYP_KEN {
		DWORD			nFrameNo;									// �t���[��No
		DWORD			nPlgDiff;									// PLG����
		DWORD			nTimeDiff;									// ���ԍ��� [10us]
		DWORD			nLvalAdd;									// ExSync(=LVAL)�ώZ�l
		DWORD			nPlgAdd;									// PLG�ώZ�l
		DWORD			nTimeAdd;									// ���ԐώZ�l [10us]

		// �O��l�ێ�
		DWORD			nWpdOnFrameNo;								// WPD�I�����̃t���[��No
		DWORD			nWpdOnLvalAdd;								// WPD�I������ExSync�ώZ�l
		DWORD			nWpdOnYCnt;									// WPD�I�����̃t���[����ExSync�ʒu �i0�`�c�t���[���T�C�Y-1�j
	} ken;
	DWORD				nYobi4[3];

	// �\���p
	struct TYP_DSP {
		DWORD			nPlgAdd;									// PLG�ώZ�l
																	// �O��l�ێ�
		DWORD			nWpdOnPlgAdd;								// WPD�I������PLG�ώZ�l
	} dsp;
	DWORD				nYobi5[2];

	WORD				nGateKen;									// �����p�Q�[�g�M��
	WORD				nGateDsp;									// �\���p�Q�[�g�M��
	// �p���X  (0:OFF  1:ON)
	struct TYP_PLS {
		WORD			nWpd[3];									// WPD�M��[0:#4�����p, 1:#5, 2:#6�\���p]
		WORD			nSharCutF;									// �o���t�@�[�X�g�J�b�g�M��
		WORD			nSharCutL;									// �o�����X�g�J�b�g�M��
	} pls;
	WORD				nSpm;										// SPM�J���M��
	WORD				nYobi6[8];

	WORD				nChilerErr[3];								// �`���[�ُ�M��[0:�\��,1:����,2:BK]
	WORD				nChilerSt[3];								// �`���[�^�]�M��[0:�\��,1:����,2:BK]
	struct TYP_DAISYA {
		WORD			nOnline;									// �Ӄ֑�Ԉʒu �I�����C��
		WORD			nOffline;									// �Ӄ֑�Ԉʒu �I�t���C��
	} daisya[2];	// [0:�\,1:��]

	struct TYP_SECURITY {
		WORD			nDoor;										// �d�C�� �� �J���
		WORD			nKey;										// �d�C�� �� �J���
	} Sec;

	WORD				nPatoResetPb;								// �p�g���C�g���Z�b�gPB

	struct TYP_AIRBLOW {
		WORD			nAirBlowSt;									// �G�A�[�h���C���^�]�M��
		WORD			nAirBlowErr;								// �G�A�[�h���C���ُ�M��
	} AirBlow[2];	// [0:�\,1:��]

	WORD				nDaisyaInvSt;								// ��Ԑ����INV�d��OFF


	WORD				nYobi7[6];

	WORD				nToyuSply;									// �h���X�v���[���u ���t���

	WORD				nYobi8[23];
};

// ���M�\����	��MAX256byte
struct SEQ_DM_SEND_DEFECT {
	struct  TYP_STAT {
		WORD			nHBCnt;										// �w���V�[�i1�`9999�j
		WORD			nKadou;										// �ғ���ԁi0:�s��, 1:�����グ��, 2:��~��, 3:�ғ���, 4:���g���C���j
		WORD			nKiki[2];									// �@����[�\/���i�����͗\���j]�i0:����, 1:�y�̏�, 2:�d�̏�j
		WORD			nKensa[2];									// �������[�\/���i�����͗\���j]�i0:����, 1:�k��, 4:�G�b�W�s��, 5:���ۑ�, 6:������, 9:��~�j
		WORD			nPlg;										// PLG�I���敪�i0:�I�����C��, 1:�\�Z��, 2:���Z��, 3:�e�X�g�p���X�j
	};

	TYP_STAT			typKensaStat;								// �ӃփX�e�[�^�X
	WORD				nYobi1[1];

	struct TYP_VOICE {
		WORD				nVoice_Err[6];							// �ݔ��ُ�{�C�X�o�� �����i0:��~, 1:�o�́j
																	//  0: �ݔ��ُ� 1: �u�r���ُ�ł��v
																	//  1: �ݔ��ُ� 2: �u�R�C����񂪎�M�ł��܂���v
																	//  2: �ݔ��ُ� 3: �u�r�������ё��M�ُ�ł��v
																	//  3: �ݔ��ُ� 4: �u�Ɩ����u�ُ�ł��v
																	//  4: �ݔ��ُ� 5: �u�J�����ُ�ł��v
																	//  5: �ݔ��ُ� 6: �u���x�ُ�ł��v
		WORD				nVoice_Def[10];							// ���׌��o�{�C�X�o�� �����i0:��~, 1:�o�́j
																	//  6: ���׌��o 1: �u�\�v
																	//  7: ���׌��o 2: �u���v
																	//  8: ���׌��o 3: �u�w�Q�ڋ߁v
																	//  9: ���׌��o 4: �u�X���o�[�ڋ߁v
																	// 10: ���׌��o 5: �u�A���r�ڋ߁v
																	// 11: ���׌��o 6: �u�d���r�ڋ߁v
																	// 12: ���׌��o 7: �u�����r���Ӂv
																	// 13: ���׌��o 8: �u�����r���� �P���v
																	// 14: ���׌��o 9: �u�r�ڋ߁v
																	// 15: ���׌��o10: �u�\�����r�ڋ߁v
	};

	TYP_VOICE			typKensaVoice;								// �Ӄփ{�C�X

	//WORD				nVoice[18];									// �{�C�X�o�� �����i0:��~, 1:�o�́j
	WORD				nYobi2[8];

	WORD				nSekkin[2][8][2];							// �r�ڋ߃����v [0:�\, 1:��][0:DS��, ..., 6:WS��][0:�y�r, 1:�d�r]�i0:��~, 1:�o�́j
	WORD				nSekalm[4][2];								// �r�ڋ߃A���[�� (���ʂ̂�)[0:DS��, ..., 6:WS��][0:�y�r, 1:�d�r]�i0:��~, 1:�o�́j
	WORD				nPato[2][2];								// �p�g���C�g[0:������, 1:�^�]��][0:�����v, 1:�u�U�[]�i0:��~, 1�`:�o�́i���`�F�b�N�����j�j

	WORD				nYobi3[12];

	WORD				nCntReset;									// �V�[�P���T ���@�\�J�E���^���Z�b�g�v���i0:�����l, 1:���Z�b�g�v���j
	WORD				nCntStart;									// �V�[�P���T ���@�\�J�E���^�J�n�v���i0:�����l, 1:�J�n�v���j
	WORD				nTestWpdReq;								// �e�X�g�p WPD�M�� ON�v���i���Q�[�g�M����ON�j

	WORD				nYobi4[25];
};
#pragma pack(pop)


#endif