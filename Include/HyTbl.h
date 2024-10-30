// *********************************************************************************
//	�\��PC���L�������p�w�b�_�[
//	[Ver]
//		Ver.01    2017/06/27  ���ō쐬
//
//	[����]
//		��{�I�ɁA����������
// *********************************************************************************

// �X�̃V�X�e���ŗL�̃C���N���[�h
#pragma once

// �ӃփC���N���[�h
#include "LineCommon.h"	// �Ӄ֋��ʃw�b�_�[

//=======================================
// ���L����������
//=======================================
#define		TBL_HY_STATUS_NAME		"TBL_HY_STATUS"				// �X�e�[�^�X���e�[�u��
#define		TBL_HY_POINT_NAME		"TBL_HY_POINT"				// �|�C���^���e�[�u��
#define		TBL_HY_COILINF_NAME		"TBL_HY_COILINF"			// �R�C�����e�[�u��
#define		TBL_HY_COILRESULT_NAME	"TBL_HY_COILRESULT"			// �R�C�����уe�[�u��
//#define		TBL_HY_MAP_COLROW_NAME	"TBL_HY_MAP_COLROW"			// �}�b�v���e�[�u��
#define		TBL_HY_MAP_ROW_NAME		"TBL_HY_MAP_ROW"			// �}�b�v�ʒu�e�[�u��
#define		TBL_HY_DEFECTINF_NAME	"TBL_HY_DEFECTINF"			// �r���e�[�u��
#define		TBL_HY_ALARM_NAME		"TBL_HY_ALARM"				// �x����e�[�u��

#define		TBL_HY_DEFLIST_NAME		"TBL_HY_DEFLIST"			// �d���׈ꗗ�\���|�C���^���e�[�u��
//#define		TBL_HY_DSPLENGTH_NAME	"TBL_HY_DSPLENGTH_NAME"		// �\���ʒu���e�[�u��
#define		TBL_HY_DEFMAP_NAME		"TBL_HY_DEFMAP"				// �r�}�b�v�\�����e�[�u��

//#define		TBL_HY_MAE_MAP_NAME		"TBL_HY_MAE_MAP"			// �O�H���}�b�v���e�[�u��
#define		TBL_HY_MAE_INF_NAME		"TBL_HY_MAE_INF"			// �O�H���r���e�[�u��

//#define		TBL_HY_CYCLE_MAP_NAME	"TBL_HY_CYCLE_MAP"			// �����}�b�v���e�[�u��
//#define		TBL_HY_CYCLE_INF_NAME	"TBL_HY_CYCLE_INF"			// �����r���e�[�u��

// 2024.07.26 Kouno 2CGL�������u�Ӄ֐��x����@�\����(������) ---------------->>>>>
#define		TBL_HY_LONGPD_NAME		"TBL_HY_LONGPD"				// ���������e�[�u��
// 2024.07.26 Kouno 2CGL�������u�Ӄ֐��x����@�\����(������) <<<<<----------------

//=======================================
// �Œ�T�C�Y
//=======================================
#ifndef LOCAL	// �{�ԗp
	#define		TBL_HY_REC_MAX			6						// �\��PC���L������ ���R�[�h��
	#define		HY_LST_DEFLIST_MAX		25						// �d���׈ꗗ�\���r��� �ő匏��
	// �O�H���֘A
	#define		TBL_HY_MAE_INF_MAX		2000					// �O�H���r���
	//// �����r�֘A
	//#define		TBL_HY_CYCLE_INF_MAX	2000					// �����r���
	#define		DSP_COIL_MAX			5                       // �T����ʕ\���R�C��MAX(���݃R�C���{�ߋ�4�R�C���\���\)
#else			// �J���p
	#define		TBL_HY_REC_MAX			6						// �\��PC���L������ ���R�[�h��
	#define		HY_LST_DEFLIST_MAX		25						// �d���׈ꗗ�\���r��� �ő匏��
	// �O�H���֘A
	#define		TBL_HY_MAE_INF_MAX		2000					// �O�H���r���
	//// �����r�֘A
	//#define		TBL_HY_CYCLE_INF_MAX	2000					// �����r���
	#define		DSP_COIL_MAX			2                       // �T����ʕ\���R�C��MAX(���݃R�C���{�ߋ�4�R�C���\���\)
#endif

#define		TBL_HY_COIL_MAX				TBL_HY_REC_MAX			// �\��PC���L������ �R�C����

//***************************************************************
// DIV ��`
//   �\���Ǘ����L���������L�̂��̂��`����
//***************************************************************
//=======================================
// ��ʕ\�����
//=======================================
enum DIV_DSP_STATUS {
	DIV_DSP_NG = -1,											// ��ʕ\���s�� (�\���ʒm�����ŁA���̃R�C���ɂ����������)
	DIV_DSP_INITIALIZE,											// �f�[�^�Ȃ�   (�����l)
	DIV_DSP_STANDBY,											// ��ʕ\���p�� (�����������A�Ώۍނ��܂��A�\����_��ʉ߂��Ă��Ȃ�)
	DIV_DSP_OK,													// ��ʕ\����
};

//=======================================
// ���L�������\����
//=======================================

//------------------------------
// �X�e�[�^�X���e�[�u��
//
struct TBL_HY_STATUS {
	EM_DIV_UNTEN			emUnten;							// �^�]��� (DIV_UNTEN_STOP���)
	EM_DIV_KADOU			emKadou;							// �ғ���� (DIV_KADOU_INIT���) 
	TYP_STATUS_NOWSYSTEM	status;								// �X�e�[�^�X
};

//------------------------------
// �|�C���^���e�[�u��
//
// �e������̍s���
struct POINT_ROW_INFO {
	long			nLenDisp[MAX_DISP_POS];								// �ʔ��� [m]
																		// [0,1,2]�F�㗬,�����J����,�\����_�ʒu�Œ�
	long			nRowDisp[MAX_DISP_POS][MAP_STAGE_NUM];				// �ʔs (�ŏ��Ԋu, ���ۂ̕\���G���A��, ���ۂ̕\���G���A��) (1�I���W��)	(200m�P�ʂ̏ꍇ 0�`199m�܂ł� 0)
																		// 	�� �ʒu����M�Ŗ���f�[�^�Z�b�g
																		// 	[0,1,2]�F�㗬,�����J����,�\����_�ʒu�Œ�
	long			nRowWrite[NUM_MEN][MAP_STAGE_NUM];					// ���ݏ������ݍs(�����I�����_�ōŏI�s�ɂȂ�)(0:�\,1:��) (0�I���W��) (200m�P�ʂ̏ꍇ 0�`199m�܂ł� 0)
																		// 	�� �s����M�Ŗ���f�[�^�Z�b�g
																		// 	�� �Y���ʒ�~�̏ꍇ�A�ʒu����M���ɍX�V
	long			nRowLast[MAP_STAGE_NUM];							// �����ŏI�s(�����I�����_�ōŏI�s�ɂȂ�) (1�I���W��)
																		// 	�� �R�C�����тōŏI�l���m�肳����
	long			yobi[2];											// �\��
};

// �e���R�[�h�P�ʂ̃|�C���^���
struct POINT_TBL {
	char			cKizukenNo[SIZE_KIZUKEN_NO];						// �Ǘ�No
	DIV_DSP_STATUS	emDispStatus;										// ��ʕ\�����
	POINT_ROW_INFO	typ;												// �ʒu���
};
	
//// �S�̕t�����
//struct POINT_TBL_INF_NOW {
//	long			nLenZan;											// �R�C���c�� [mm] (�L�ї��l���R�C���� - ������ʒu�ł̒ʔ���)
//};
// �{��
struct TBL_HY_POINT {
	// nRecWrite,nRecDispBase�́A���܂������΍���폜�\��

	BOOL					nRefresh;									// �X�V���t���O  ��TRUE�̎��́A[nRecDisp]�Q�Ƌ֎~
	long					nRecWriteBase;								// �����݃��R�[�h(�\������)   [0:����  1�`:�l-1��pnt�̔z��ʒu�ƂȂ�]					�� �����p�R�C������M �ōX�V
	long					nRecWrite[NUM_MEN];							// �e�������u�ʒu�ł� ���݃��R�[�h(�\����) [0:����  1�`:�l-1��pnt�̔z��ʒu�ƂȂ�]		�� �s����M �ōX�V

	long					nRecDispBase;								// �\����_�ʒu�ł� ���ݕ\�����R�[�h(�\����) [0:����  1�`:�l-1��pnt�̔z��ʒu�ƂȂ�]	�� �\���p�R�C������M �ōX�V
	long					nRecDisp[MAX_DISP_POS];						// �e������ʒu�ł� ���ݕ\�����R�[�h(�\������) [0:����  1�`:�l-1��pnt�̔z��ʒu�ƂȂ�]	�� TO_SO_DATA_KIND_LENGTH��M�ōX�V (�g���b�L���O�\�����Ɏg�p)
																		// 	[0]�F�㗬�J�����ʒu�Œ�
																		// 	[1]�F�����J�����ʒu�Œ�
																		// 	[2]�F�\����_�ʒu�Œ� �\��
																		// 	[3]�F�\����_�ʒu�Œ� ����
																		// 	[4]�F������ʒu �\��
																		// 	[5]�F������ʒu ����

	//POINT_TBL_INF_NOW		inf[MAX_DISP_POS];							// �S�̕t�����

	long					yobi[2];									// �\��

	POINT_TBL				pnt[TBL_HY_REC_MAX];						// �e�ʔ|�C���g�ł� ���݃��R�[�h
};

//------------------------------
// �R�C�����e�[�u��
//
struct TBL_HY_COILINF {
	struct REC {
		COINFO_BASE			coilinf;									// �R�C�����\����
	} rec[TBL_HY_COIL_MAX];
};

//------------------------------
// �R�C�����уe�[�u��
//
struct TBL_HY_COILRESULT {
	struct REC {
		long				nLen;										// ���������� [mm]

		long				nCutCnt;									// �J�b�g���Z�b�g��
		TO_SO_DATA_CUT		cut[MAX_SHACUT_COUNT];						// �J�b�g���

	} rec[TBL_HY_COIL_MAX];
};

//------------------------------
// �}�b�v���e�[�u��
//
//struct TBL_HY_MAPCOLROW {
//	DSP_MAP_COLROW			rec[TBL_HY_REC_MAX];
//};

//------------------------------
// �}�b�v�ʒu�e�[�u��
//
struct TBL_HY_MAPROW {
	DSP_MAP_ROW				rec[TBL_HY_REC_MAX];
};

//------------------------------
// �r���e�[�u��
//
struct TBL_HY_DEFECTINF {
	DSP_MAP_DEFECTINF		rec[TBL_HY_REC_MAX];
};


//------------------------------
// �d���׈ꗗ�\���|�C���^���
//
struct TBL_HY_DEFLIST
{
	struct DSP_LIST_HEADER
	{
		int		nPos[NUM_MEN];											// �ŐV���R�[�h[ 0:�\ 1:�� ] 0:�f�[�^�Ȃ� 1�`100 �f�[�^���̍ŐV���R�[�h 
		char	cYobi1[24];
	} head;
	struct DSP_LIST_DATA
	{
		DSP_TYP_DEFLIST	defect[HY_LST_DEFLIST_MAX];
	} data[NUM_MEN];
};


//------------------------------
// �O�H���}�b�v���e�[�u��
//
// �O�H���������
struct DSP_TYP_MAE_DEFECTINF
{
	short					nImageFlg;									// �摜�`�� (0:Jpeg 1:BMP)
	short					nDspMode;									// ���O�ҏW�t���O (-1:��\��  0:�ʏ�\��  1:�����\��) ���v���r�W����

	WORD					men;										// �\���敪  (0:�\ 1:��)
	WORD					cycle_div;									// �����r�敪(0:����r, 1:�ȈՎ����r,�A���r 2:��������)
	WORD					dsws;										// DSWS�敪  (0:DS 1:WS)
	WORD					yobi;	

	// ����A���ʒu�́AFF�����g�p
	int						speed;										// ���x[mpm]
	int						pitch;										// �s�b�`[mm]
	float					roolkei;									// ���[���a[mm]
	short					box_wid;									// �r�� [mm]
	short					box_len;									// �r���� [mm]

	// �摜�\���ɕK�v
	float					h_res;										// �c����\[mm/pixel]
	float					w_res;										// ������\[mm/pixel]
	short					box_x_min;									// �O��Xmin[pixel]		(�؂�o���摜�̍��ォ��̈ʒu) (�}�C�i�X�l���肦��)
	short					box_x_max;									// �O��Xmax[pixel]
	short					box_y_min;									// �O��Ymin[pixel]
	short					box_y_max;									// �O��Ymax[pixel]
	short					center_x;									// �r���S�ʒuX [pixel]  (�؂�o���摜�̍��ォ��̈ʒu)
	short					center_y;									// �r���S�ʒuY [pixel]
	WORD					h_size;										// �c��f��[pixel]
	WORD					w_size;										// ����f��[pixel]

};

struct DSP_TYP_MAE_KIZUINF							// 256 byte
{
	DSP_TYP_MAE_DEFECTINF	data;										// �O�H�� �r���

	// VB��2�����z��̃}�[�V�����̕��@���������� ����
	char					image_path_1[SIZE_IMAGE_PATH];				// �摜�̃p�X (�摜��������ALL NULL)
	char					image_path_2[SIZE_IMAGE_PATH];				// �摜�̃p�X (�摜��������ALL NULL)
	char					image_path_3[SIZE_IMAGE_PATH];				// �摜�̃p�X (�摜��������ALL NULL)
	char					image_path_4[SIZE_IMAGE_PATH];				// �摜�̃p�X (�摜��������ALL NULL)

	char reserve[256-sizeof(DSP_TYP_MAE_DEFECTINF)-SIZE_IMAGE_PATH*4];		// �\��
};

// FF���
struct DSP_TYP_MAE_FFINF
{
	char					cMaeKizukenNo[SIZE_KIZUKEN_NO];				// FF�Ǘ�No (�O�H���Ӄւł̊Ǘ�No)
	int						nMaeKizuNo;									// FF�rNo   (�O�H���Ӄւł��rNo)
	char					cT[2];										// FF�r��
	char					cG[2];										// FF�O���[�h
	char					cProc[2];									// FF�r���o�H��
	char					yobi1[2];
	int						nDiv;										// FF�r�敪
	int						nPos;										// FF�r���������ʒu [mm]	������͎g��Ȃ�
	int						nWidth;										// FF�r�Е����ʒu [mm]		(WS����̈ʒu)
	int						nLen;										// FF�r���� [mm]

	// �g��
	int						nY;											// �ŏI����ʒu [mm]
// 2023.11.17 �ŏI�H���������u���@�\�����̂S -------------------->>>>>
	int						nY_Out;										// �ŏI�o������ʒu [mm]
// 2023.11.17 �ŏI�H���������u���@�\�����̂S <<<<<--------------------	
	short					nTid;										// MAE_TYPE_NAME  �� ID  (0:�s��   1�`�F�O�H���̑Ώ��r) ���s���̏ꍇ�́AFF���ŕ\���B�}�X�^�[����`��
	short					nGid;										// MAE_GRADE_NAME �� ID  (0:�s��)
																		// <20221030><�a��><2TCM���@���ɔ������H���̉���> --->>>
	int						nKouteiID;									// �H�����
																		// <20221030><�a��><2TCM���@���ɔ������H���̉���> <<<---
};

// �{��
struct TBL_HY_MAE_INF
{
	struct REC {
		struct MEN {
			DWORD						num;							// ���݂̊i�[����

			struct MAE_INF {
				int						nMaeDsp;						// �O�H���r�̕\����� (0�F�r����   1:�O�H���r���A�r�摜�\��  2:FF���̂ݕ\��) �� MAE_MASTER����擾�����l

				DSP_TYP_MAE_FFINF		ff;								// FF��� (��ɗL��)
				DSP_TYP_MAE_KIZUINF		inf;							// �O�H��������� (�����ꍇ������)
				int						nKizuNo;						// ���H��DB�rNo
			} maeinf[TBL_HY_MAE_INF_MAX];
		} men[NUM_MEN];													// �\ ��
	} rec[TBL_HY_REC_MAX];
};

//------------------------------
// �x����e�[�u��
//
struct TBL_HY_ALARM {
	TO_SO_DATA_ALARM data;
};

// 2024.07.26 Kouno 2CGL�������u�Ӄ֐��x����@�\����(������) ---------------->>>>>
//------------------------------
// ���������e�[�u��
//
struct TBL_HY_LONGPD {
	// �S�̃X�e�[�^�X
	int nGuidanceFlag;													// �K�C�_���X�\���t���O(0:�\���Ȃ� 1:�\������) �����R�C���A�O�R�C���Ŕ���

	// �R�C���P��
	struct REC {
		int			nLongPdOccurrenceFlag;								// �����������L�� (0�F���������ׂȂ�, 1�F���������ׂ���j
	} rec[TBL_HY_COIL_MAX];
};
// 2024.07.26 Kouno 2CGL�������u�Ӄ֐��x����@�\����(������) <<<<<----------------
