// *********************************************************************************
//	�e���C���ŋ��ʂ̃C���N���[�h
//	[Ver]
//		Ver.01    2007/01/30  vs2005 �Ή�
//
//	[����]
//		��{�I�ɁA����������
// *********************************************************************************

// �X�̃V�X�e���ŗL�̃C���N���[�h
#pragma once

// �ӃփC���N���[�h
#include "KizuCommon.h"										// �Ӄ֋��ʃw�b�_�[
#include "OutInterface.h"									// �O���C���^�[�t�F�[�X�p�w�b�_�[


//***************************************************************
// �ݒ�ϒ萔 ��`
//***************************************************************
//=======================================
// �ݒ�萔 ��`
//=======================================

//=======================================
// �Œ�T�C�Y
//=======================================
// ���C�����
#ifndef LOCAL
	#define COIL_LEN_MAX				10000				// �����R�C����MAX[m] (�ǂ̍��ڂł����̒��������ƂȂ�)
	#define COIL_LEN_MIN				200					// �����R�C����MIN[m] (�悭�������R�C��)
	#define COIL_WID_MAX				1750				// �R�C����MAX[mm] (�ǂ̍��ڂł����̕������ƂȂ�)
	#define COIL_THICK_MAX				1600				// �ޗ���MAX[��m]
#else
	#define COIL_LEN_MAX				10000				// �����R�C����MAX[m] (�ǂ̍��ڂł����̒��������ƂȂ�)
	#define COIL_LEN_MIN				200					// �����R�C����MIN[m] (�悭�������R�C��)
	#define COIL_WID_MAX				1750				// �R�C����MAX[mm] (�ǂ̍��ڂł����̕������ƂȂ�)
	#define COIL_THICK_MAX				1600				// �ޗ���MAX[��m]
#endif

// ���u
#define NUM_MEN						2										// ���u �ʒP�� (���̒P�ʂŁA������~���\) �� �\/������Ȃ����ɒ���
#define NUM_MAX_HANTEI				7										// �Ж� ����PC�̐� (�I�t���C���@���܂߂��䐔)
#define NUM_MAX_CYCLE				1										// �Ж� ����PC�̐� (�I�t���C���@���܂߂��䐔) �� 0���ƃR���p�C�����ʂ�Ȃ���������̂ŁA0�ł�1�ɂ��Ă���
#define NUM_MAX_PARA				1										// �Ж� ���W�b�N�p������PC�̐�
#ifndef LOCAL															// �I�����C��
	#define NUM_HANTEI				5 //NUM_CAM_POS							// �Ж� ����PC�̐� (�ЖʂɌq���锻��PC�̐�) (�I�����C���@�̂�)
	#define NUM_HANTEI_CYCLE		1										// �Ж� ����PC�̐� (�ЖʂɌq�������PC�̐�) (�I�����C���@�̂�) �� 0���ƃR���p�C�����ʂ�Ȃ���������̂ŁA0�ł�1�ɂ��Ă���
	#define NUM_HANTEI_PARA			1										// �Ж� ���W�b�N�p������PC�̐�
	#define NUM_RPIEXEC				1										// �Ж� �S������PC�̐� (�ЖʂɌq����S������PC�̐�) (�I�����C���@�̂�)
	#define NUM_CAMPAIR				10										// �J�����y�A�� (�J�����y�A�Ƃ́A�����̃J�����Z�b�g���P����PC��œ��삷��ꍇ�̃J�����Z�b�g�̉�̌�)(����́A1PC��2�J�����Z�b�g)
	#define NUM_CAMPAIR_CYCLE		1										// �J�����y�A�� (����PC�P��œ��삷��J�����Z�b�g) �� 0���ƃR���p�C�����ʂ�Ȃ���������̂ŁA0�ł�1�ɂ��Ă���
	#define NUM_CAMPAIR_PARA		1										// �J�����y�A�� (���W�b�N�p������PC�P��œ��삷��J�����Z�b�g)(����́A1PC��2�J�����Z�b�g)
	#define NUM_SENDER				2										// �Ж� �S�����R�[�_�[PC�̐� (�ЖʂɌq����S�����R�[�_�[PC�̐�)
#else																	// ���[�J���e�X�g
	#define NUM_HANTEI				5										// �Ж� ����PC�̐� (�ЖʂɌq���锻��PC�̐�) (�I�����C���@�̂�)
	#define NUM_HANTEI_CYCLE		1										// �Ж� ����PC�̐� (�ЖʂɌq�������PC�̐�) (�I�����C���@�̂�)
	#define NUM_HANTEI_PARA			1										// ���W�b�N�p������PC�̐�
	#define NUM_RPIEXEC				1										// �Ж� �S������PC�̐� (�ЖʂɌq����S������PC�̐�) (�I�����C���@�̂�)
	#define NUM_CAMPAIR				10										// �J�����y�A�� (�J�����y�A�Ƃ́A�����̃J�����Z�b�g���P����PC��œ��삷��ꍇ�̃J�����Z�b�g�̉�̌�)(����́A1PC��2�J�����Z�b�g)
	#define NUM_CAMPAIR_CYCLE		1										// �J�����y�A�� (����PC�P��œ��삷��J�����Z�b�g)(����́A1��4)
	#define NUM_CAMPAIR_PARA		1										// �J�����y�A�� (���W�b�N�p������PC�P��œ��삷��J�����Z�b�g)(����́A1PC��2�J�����Z�b�g)
	#define NUM_SENDER				2										// �Ж� �S�����R�[�_�[PC�̐� (�ЖʂɌq����S�����R�[�_�[PC�̐�)
#endif
#define	NUM_ALL_HTBACK				4										// �S�ʂ̔���o�b�N�A�b�vPC�̑䐔
#define	NUM_ALL_HTPARA				(NUM_HANTEI_PARA*NUM_MEN)				// �S�ʂ̔��胍�W�b�N�p������PC�̑䐔
#define NUM_ALL_HANTEI				(NUM_HANTEI*NUM_MEN) + NUM_ALL_HTBACK + NUM_ALL_HTPARA	// �S�ʂ̔���PC�̑䐔
#define	NUM_ALL_HT					(NUM_HANTEI*NUM_MEN) + NUM_ALL_HTBACK	// �S�ʂ̔���o�b�N�A�b�vPC�̑䐔�i���胍�W�b�N�p������PC�܂܂��j
#define NUM_CAMERA					(MAX_CAMSET*MAX_PHYSICAL_CAMANGLE)		// �S�ʂ̃J�����䐔 (�J�����Z�b�g �~ �J�����p�x)
#define MAX_IMAGE_CNT				4										// �摜���� (=�G���W������o�͂���錋�ʐ�) (�F���܂ł�MAX_CAMANGLE��MAX_IMAGE_CNT�͓��`���������A�����ł́A�J�����P��ŕ����摜������΍�)

#define NUM_LIGHT					2										// �Ж� �Ɩ����u�̐�
#define NUM_LED_COLOR				3										// �Ɩ����u�̐F���iRGB�j
#define NUM_LED_BLOCK				48										// ��ʐݒ�\�Ȕz���u���b�N�� (ini�t�@�C���͎��ۂ̃u���b�N��) (MAX256)
//#define NUM_LED_BLOCK				36										// ��ʐݒ�\�Ȕz���u���b�N�� (ini�t�@�C���͎��ۂ̃u���b�N��) (MAX256)

#define	NUM_CAMLINKSW				4										// �J���������N�X�C�b�`�䐔
#define	NUM_CAMLINK_BOARD			2										// PC1�䓖����̍ő���͐� (BOARD�ɂȂ��Ă��邪�A���ۂ͎�荞�݌����B)
#define	NUM_CAMLINKSW_CH_MAX		15										// �`�����l�����
#define	NUM_CAMLINKSW_CAM_MAX		8										// �J�����Z�b�g���
#define	NUM_CAMLINKSW_CAM_MIN		-8										// �J�����Z�b�g����

// �V�X�e�����
// 20200520 �r�ۑ�������g�� --->>>
#define	VRCL_DEFECT_NO_START		64001									// VRCL�V�K�r�̓��͔͈͂��rNo(�J�n)
#define	VRCL_DEFECT_NO_END			65000									// VRCL�V�K�r�̓��͔͈͂��rNo(�I��)
#define	VRCL_DEFECT_NUM				(VRCL_DEFECT_NO_END - VRCL_DEFECT_NO_START + 1)	// VRCL�V�K�r�̓����r��
//#define MAX_DETECT_SAVE				32000									// 1�R�C��������̕ێ������r�� (�\������)
#define MAX_DETECT_SAVE				524000 - (VRCL_DEFECT_NUM / NUM_MEN)	// 1�R�C��������̕ێ������r�� (�\����)
// 20200520 �r�ۑ�������g�� ---<<<
#define MAX_DETECT_LOWGR_SAVE		25000									// 1�R�C��������̕ێ����閳�QGr�r�� (�\����)��1CAPL�����ł͕s�g�p
//#define MAX_DETECT_IMAGE_SAVE		32000									// 1�R�C��������̕ێ������r�摜 (�\����)
//#define MAX_DEFECT_LOWER_IMG_SAVE	7000									// 1�R�C��������̕ێ����閳�Q�r�摜�i�\���ʁj
#define MAX_MAP_DEFECT_NUM			200000									// 1�R�C��������̕\���r�� (�̂́A�񐔁~�s���~�ʂł悩�������A�[��������������߁A�ő匏�������߂�悤�ɂ���)

#define MAP_REPRES_MIN				5										// �ŏ���\�r�Ԋu [m]
#define MAP_COL_INTERVAL			200 									// �}�b�v��1�񂠂���̕� [mm]
#define MAP_COL_NUM					8										// �}�b�v�ő��
#define MAP_ROW_NUM					(COIL_LEN_MAX/MAP_REPRES_MIN)			// �}�b�v�ő�s�� (2000)
#define MAP_STAGE_NUM				3										// �}�b�v�̐؂�ւ��i�� (0:�ŏ��Ԋu 1:���ۂ̕\���G���A�� 2:���ۂ̕\���G���A��)
#define MAP_NEST_NUM				5										// �}�b�v�̃Z�����̃��X�g����
#define MAP_CYCLE_COLNUM			1										// �����r�}�b�v��
#define LAMP_COL_NUM				8								// �r�ڋ߃����v�ő��
#define ALARM_COL_NUM				4										// �r�ڋ߃A���[���ő��

#define SIZE_FRAME_IMG_X			4096									// ���t���[���摜�T�C�Y [pixel] �� �摜�����t���[���T�C�Y
#define SIZE_FRAME_IMG_Y			4096									// �c�t���[���摜�T�C�Y [pixel]
#define SIZE_GRAB_IMG_X				4080									// ���t���[���摜�T�C�Y [pixel] �� �B���t���[���T�C�Y (Deca�̏ꍇ�A16��10�Ŋ���؂��l)
#define SIZE_GRAB_IMG_Y				4096									// �c�t���[���摜�T�C�Y [pixel]

// ��������
//////#define MAP_CYCLE_COLNUM			1									// �����r�}�b�v��
//////#define STARTINDEX_CYCLE_PCID		(NUM_ALL_HANTEI+1)					// ����PC��PCID�̐擪�ʒu
//////#define GET_CYCLE_PCID(pcid)		(pcid-STARTINDEX_CYCLE_PCID+1)		// ����PC��PCID����A1�n�܂�̘A�Ԃ��Z�o����B
//////#define GET_CYCLE_PCKIND(pckind)	(pckind-PCNO_CYCLE)					// ����PC��PCKIND����A1�n�܂�̘A�Ԃ��Z�o����B


// �J����������DS/WS�Ή�
#define CAMLEFT_IS_DS_T				0											// 0:�J����������DS  1:�J����������WS (�\��)
#define CAMLEFT_IS_DS_B				0											// 0:�J����������DS  1:�J����������WS (����)
#define CAMLEFT_IS_DS(men)			(0==men?CAMLEFT_IS_DS_T:CAMLEFT_IS_DS_B)	// ���[�v���g����悤��

// �S���ۑ�
#define	NUM_JOIN_FRAME				1000									// ���t���[������r���Q��������t���[����
																			// �@�r���J�n�R�}���h�̏������сF��56[sec] �� �^�C���A�E�g�F60[sec]
																			// �@16[fps] �~ 60[sec] = 960[frame] �� �r���Q���t���[���F1000[frame]

//------------------------------------------
// ���̑� (�e�V�X�e���ŗL)
#define SIZE_COIL_KEY				20										// �R�C�����肷�邽�߂̃L�[��� (����́A�R�C��No�A����No)
#define SIZE_COIL_NO				PRO_SIZE_COIL_NO						// �R�C��No		�T�C�Y
#define SIZE_MEI_NO					PRO_SIZE_ORDER_NO						// ����No		�T�C�Y
//#define MAX_CAMERA_GAP_WIDDIV		10										// ����Y���␳ ������������(=�ő�O���[�v��)
#define NUM_SPM_STAT				2										// SPM��Ԑ�

#define NUM_TEMPMT_UNIT				4										// ���x�p�l�����[�^��

#define MAX_SHACUT_COUNT			10										// �ő�J�b�g�Ǘ���
#define MAX_PROGRP_TYPE				10										// �v���R���֑��M�����r��O���[�v�i0�F����  1�`�F�e�Ή�����O���[�v�j
#define MAX_PROGRP_GRADE			6										// �v���R���֑��M����O���[�h�O���[�v�i0�F����  1�`�F�e�Ή�����O���[�v�j

#define NUM_DSP_WPD					1										// �\����_��
#define BASE_DSP_MEN				1										// �\����_�̊�ʁi���ʁj(�\���@�\�ւ̒ʔ������M�Ŏg�p)

#define MAX_SHALINK_COUNT			3										// �ő�o�������Ǘ���

// �O�H���֌W
#define MAX_MAE_LINE				16										// �O�H���o�^��
#define MAX_MAE_TYPE				64										// �O�H���r��ő�o�^��
#define MAX_MAE_GRADE				10										// �O�H���O���[�h�ő�o�^��
#define MAX_MAE_IMAGE				4										// �O�H���摜�ő吔
#define MAE_MAP_COL_NUM				1										// �O�H��MAP�ő�� 

// �d���׈ꗗ���
#define MAX_DEFLIST_BUF				25										// �d���׈ꗗ��ʗp �o�b�t�@����

// 2024.07.26 Kouno 2CGL�������u�Ӄ֐��x����@�\����(������) ---------------->>>>>
// ����������
#define MAX_LONGPD_FILTER			16										// �������̍ő�t�B���^��
// 2024.07.26 Kouno 2CGL�������u�Ӄ֐��x����@�\����(������) <<<<<----------------

// �\���\�P�b�g��
#define HY_SOCK_NUM					3										// �\���\�P�b�g��

// �\���ʒu
enum EM_DIV_DISP {											// ��{
	//------����
	DIV_LEN_POS_KEN_TOP = 0,									// [�ʔ���] ������_ �\�@(�J�����ʒu)
	DIV_LEN_POS_KEN_BOT,										// [�ʔ���] ������_ ���@(�J�����ʒu)
	DIV_LEN_POS_DSP_TOP,										// [�ʔ���] �\����_ �\�@(������ʒu1)
	DIV_LEN_POS_DSP_BOT,										// [�ʔ���] �\����_ ���@(������ʒu2)�i������ʒu1�Ɠ�����ʂ�\���j
	DIV_LEN_POS_BASE_END,										// ���ʍ��ڂ̍Ō�

	// ��
	DIV_DISP_POS_DSP_P1,										// [�\���ʒu] ������1�ʒu
	DIV_DISP_POS_DSP_P2,										// [�\���ʒu] ������2�ʒu
//	DIV_DISP_POS_DSP_P3,										// [�\���ʒu] ������3�ʒu 
//	DIV_DISP_POS_DSP_P4,										// [�\���ʒu] ������4�ʒu 
//	DIV_DISP_POS_DSP_P5,										// [�\���ʒu] ������5�ʒu 
//	DIV_DISP_POS_DSP_SHR_I = DIV_DISP_POS_BASE_END,

	DIV_DISP_POS_END = DIV_LEN_POS_BASE_END						// �Ō�Ɏg�p
};

#define MAX_DISP_POS				DIV_DISP_POS_END			// �\����ʒu �� (0:������_ �\�� 1:������_ ���� 2:�\����_ �\�� 3:�\����_ ����)
#define MAX_ALARME_POS				2							// �x��o�͈ʒu �� (0:�\�ʌ�����, 1:���ʌ�����)

//=======================================
// �Œ�No�Z�o�}�N�� (�J�����ԍ�����̎Z�o)
//  0�`7�̘A�Ԃ��� TorB  Pos  Angle  Set �̒l�����肷��
//	0�`7�̏��Ԃ́A�ȉ��̌Œ�Ƃ���
//	  �\DS,�\DQ,�\WQ,�\WS
//	  ��DS,��DQ,��WQ,��WS
//		TorB = 0:�\ 1:��
//		CamPos = 0:DS 1:WS
//		CamAngle = 0:��
//=======================================
#define NUM_TorB(x)			(x / (NUM_CAM_POS*MAX_PHYSICAL_CAMANGLE))
#define NUM_CamPos(x)		((x % MAX_CAMSET) / MAX_PHYSICAL_CAMANGLE)
#define NUM_CamAngle(x)		(x % MAX_PHYSICAL_CAMANGLE)
#define NUM_CamSet(x)		(x / MAX_PHYSICAL_CAMANGLE)


//=======================================
// �f�[�^�x�[�X
//=======================================
#define		INI_DBZI				"DB_ZI"							// DB�ڑ� �p�Z�b�V���� (����)

#define DB_COIL_INF					"T_COIL_INF"					// [DB] �R�C�����e�[�u��
#define DB_COIL_RESULT				"T_COIL_RESULT"					// [DB] �R�C�����уe�[�u��
#define DB_COIL_CAMERA				"T_COIL_CAMERA"					// [DB] �J�������уe�[�u��
//#define DB_COIL_OUTINF_R			"T_COIL_OUTINF_R"				// [DB] �o�������R�C�����уe�[�u��
#define DB_COIL_POSITION			"T_COIL_POSITION"				// [DB] �R�C���ʒu���e�[�u��
#define DB_COIL_TRAND				"T_COIL_TRAND"					// [DB] �R�C���g�����h���e�[�u��
#define DB_COIL_CUT_POS				"T_COIL_CUT_POS"				// [DB] �J�b�g�ʒu�e�[�u��
#define DB_COIL_CUT_RESULT			"T_COIL_CUT_RESULT"				// [DB] �J�b�g���уe�[�u��
#define DB_COIL_SAISUN_RESULT		"T_COIL_SAISUN_RESULT"			// [DB] �̐����уe�[�u��
#define DB_COIL_ORDER				"T_COIL_ORDER"					// [DB] �R�C���ʔ��ʃe�[�u��

#define DB_DEFECT_INF_T				"T_DEFECT_INF_T"				// [DB] �\�r���e�[�u��
#define DB_DEFECT_IMG_T				"T_DEFECT_IMG_T"				// [DB] �\�r�摜�e�[�u��
#define DB_DEFECT_ATTR0_T			"T_DEFECT_ATTR0_T"				// [DB] �\��{�����ʃe�[�u��
#define DB_DEFECT_ATTR1_T			"T_DEFECT_ATTR1_T"				// [DB] �\�J�����p�x�P�����ʃe�[�u��
#define DB_DEFECT_ATTR2_T			"T_DEFECT_ATTR2_T"				// [DB] �\�J�����p�x�Q�����ʃe�[�u��
#define DB_DEFECT_ATTR3_T			"T_DEFECT_ATTR3_T"				// [DB] �\�J�����p�x�R�����ʃe�[�u��
#define DB_DEFECT_ATTR4_T			"T_DEFECT_ATTR4_T"				// [DB] �\�J�����p�x�S�����ʃe�[�u��

#define DB_DEFECT_INF_B				"T_DEFECT_INF_B"				// [DB] ���r���e�[�u��
#define DB_DEFECT_IMG_B				"T_DEFECT_IMG_B"				// [DB] ���r�摜�e�[�u��
#define DB_DEFECT_ATTR0_B			"T_DEFECT_ATTR0_B"				// [DB] ����{�����ʃe�[�u��
#define DB_DEFECT_ATTR1_B			"T_DEFECT_ATTR1_B"				// [DB] ���J�����p�x�P�����ʃe�[�u��
#define DB_DEFECT_ATTR2_B			"T_DEFECT_ATTR2_B"				// [DB] ���J�����p�x�Q�����ʃe�[�u��
#define DB_DEFECT_ATTR3_B			"T_DEFECT_ATTR3_B"				// [DB] ���J�����p�x�R�����ʃe�[�u��
#define DB_DEFECT_ATTR4_B			"T_DEFECT_ATTR4_B"				// [DB] ���J�����p�x�S�����ʃe�[�u��

//// �G�b�W���o����
#define DB_EDGE_COIL_INF			"T_EDGE_COIL_INF"				// [DB] �G�b�W���o���уe�[�u��
#define DB_EDGE_CHANGE_INF			"T_EDGE_CHANGE_INF"				// [DB] �G�b�W���o���[�h�ω��Ǘ��e�[�u��

//// ���H��
#define DB_ACCESS_NEXT_COIL_INF		"T_ACCESS_NEXT_COIL_INF"		// [DB] ���H���R�C�����e�[�u��
#define DB_ACCESS_NEXT_DEFECT_INF	"T_ACCESS_NEXT_DEFECT_INF"		// [DB] ���H���r���e�[�u��

//// ����M�`���o�b�t�@
#define DB_BUF_PRO_R_COILINF		"BUF_PRO_R_COILINF"				// [DB] �v���R���R�C������M�o�b�t�@�e�[�u��
#define DB_BUF_PRO_R_SAISUN			"BUF_PRO_R_SAISUN"				// [DB] �v���R���̐����ю�M�o�b�t�@�e�[�u��
#define DB_BUF_PRO_R_CUT			"BUF_PRO_R_CUT"					// [DB] �v���R���J�b�g�`����M�o�b�t�@�e�[�u��
#define DB_BUF_PRO_S_ZISEKI			"BUF_PRO_S_ZISEKI"				// [DB] �v���R���������ё��M�o�b�t�@�e�[�u��
#define DB_BUF_FF_S_ZISEKI			"BUF_FF_S_ZISEKI"				// [DB] FF�������ё��M�o�b�t�@�e�[�u��
#define DB_BUF_FF_R_MAEINF			"BUF_FF_R_MAEINF"				// [DB] FF�O�H���r����M�o�b�t�@�e�[�u��

// �O�H���p
#define DB_MAE_MASTER				"M_MAE_MASTER"					// [DB] �O�H���}�X�^�[�e�[�u��
#define DB_MAE_TYPE_NAME			"M_MAE_TYPE_NAME"				// [DB] �O�H���r��}�X�^�[�e�[�u��
#define DB_MAE_GRADE_NAME			"M_MAE_GRADE_NAME"				// [DB] �O�H���O���[�h�}�X�^�[�e�[�u��

#define DB_MAE_COIL_INF				"T_MAE_COIL_INF"				// [DB] �O�H���R�C�����e�[�u��
#define DB_MAE_COIL_LINE			"T_MAE_COIL_LINE"				// [DB] �O�H�����C���ʃR�C�����e�[�u��
#define DB_MAE_FF_INF				"T_MAE_FF_INF"					// [DB] �O�H���e�e�r���e�[�u��
#define DB_MAE_DEFECT_INF			"T_MAE_DEFECT_INF"				// [DB] �O�H���r���e�[�u��
#define DB_MAE_DEFECT_IMG			"T_MAE_DEFECT_IMG"				// [DB] �O�H���r�摜�e�[�u��

// 2024.07.26 Kouno 2CGL�������u�Ӄ֐��x����@�\����(������) ---------------->>>>>
// �������p
#define DB_LONGPD_COMMON				"M_LONGPD_COMMON"				// [DB] ���������׌��o_�}�X�^�[�ݒ�e�[�u��
#define DB_LONGPD_PCODE					"M_LONGPD_PCODE"				// [DB] ���������׌��o_�����p�^�[���R�[�h�ʐݒ�e�[�u��
#define DB_LONGPD_PCODE_FILTER			"M_LONGPD_PCODE_FILTER"			// [DB] ���������׌��o_�����p�^�[���~�t�B���^�[�ݒ�e�[�u��
#define DB_LONGPD_PCODE_FILTER_OUTPUT	"M_LONGPD_PCODE_FILTER_OUTPUT"	// [DB] ����������_�r��ݒ�e�[�u��

#define DB_LONGPD_COIL_RESULT			"T_LONGPD_COIL_RESULT"			// [DB] ���������׌��o_���уe�[�u��
#define DB_LONGPD_EXEC_RESULT_WIDTH		"T_LONGPD_EXEC_RESULT_WIDTH"	// [DB] ���������׌��o_�t�B���^�[���уe�[�u��
#define DB_LONGPD_EXEC_RESULT_DEFECT_T	"T_LONGPD_EXEC_RESULT_DEFECT_T"	// [DB] ����������_�\�r���e�[�u��
#define DB_LONGPD_EXEC_RESULT_DEFECT_B	"T_LONGPD_EXEC_RESULT_DEFECT_B"	// [DB] ����������_���r���e�[�u��
// 2024.07.26 Kouno 2CGL�������u�Ӄ֐��x����@�\����(������) <<<<<----------------

//=======================================
// �p�����[�^�ύX�敪 �i���C���ŗL�����j
//   �r�����ʉ�ʂ̏ꍇ�́AKizuCommon�ɋL�q���邱��
//=======================================
#define CH_PARAM_DEFLIST				200							// �r�ꗗ
#define CH_PARAM_SETTIG_ALARM			201							// �r���x��o�͐ݒ�
#define CH_PARAM_MAE_MAST				209							// �O�H���ݒ�
#define CH_PARAM_TAJUUTHRESHOLD			210							// ���d����\臒l�ݒ�
#define CH_PARAM_REDUCTION				211							// �k�����ݒ�
#define CH_PARAM_FILTER					212							// �t�B���^�ݒ�
////#define CH_PARAM_CYCLETHRESHOLD			213						// �����p����\臒l�ݒ�
#define CH_PARAM_PARA					214                         // ���W�b�N�p�������ݒ�
#define CH_PARAM_CROP_SHRINK			215                         // �Ԉ����ݒ�
// 2024.07.26 Kouno 2CGL�������u�Ӄ֐��x����@�\����(������) ---------------->>>>>
#define CH_PARAM_LONGPD					301							// ���������׌��o
// 2024.07.26 Kouno 2CGL�������u�Ӄ֐��x����@�\����(������) <<<<<----------------

//=======================================
// �^�X�N����
//=======================================
// ���ʃ^�X�N (�኱���C�����̂�BLineCommon���g��Ȃ��ł悢�̂ł���΁AKizuCommon�ɒ�`)
#define KT_LEDMGR					"KT_LEDMGR"						// ��LED�ʐM (�A�C�e�b�N��) (ifdef�̃X�C�b�`�ɂ��g�p)
#define KT_LLBGMG					"KT_LLBGMG"						// �΍��P�xLED�ʐM (�A�C�e�b�N��) 

// �����@�\
#define TO_DEFECT					"TO_DEFECT"						// �����Ǘ�
#define TO_CAMERA					"TO_CAMERA"						// �J�����R���g���[����H�ʐM
#define TO_LINKSW					"TO_LINKSW"						// �΃J���������N�X�C�b�`�ʐM
#define TO_SEQDM					"TO_SEQDM"						// �΃V�[�P���T �f�[�^�������ʐM
#define TO_GETDAT					"TO_GETDAT"						// ���̓f�[�^�ҏW�Ǘ�
#define TO_PUTDAT					"TO_PUTDAT"						// ���ѕҏW����
#define TO_MAEDAT					"TO_MAEDAT"						// �O�H�����ѕҏW����
#define TO_PROCON					"TO_PROCON"						// �΃v���R���ʐM
#define TO_FFSERV					"TO_FFSERV"						// ��FF�T�[�o�[�ʐM
// 2024.07.26 Kouno 2CGL�������u�Ӄ֐��x����@�\����(������) ---------------->>>>>
#define TO_CHKLONG					"TO_CHKLONG"					// ���������׊Ď�
// 2024.07.26 Kouno 2CGL�������u�Ӄ֐��x����@�\����(������) <<<<<----------------

// ����@�\
#define HT_DEFECT					"HT_DEFECT"						// ����Ǘ�

// �\���@�\
#define HY_DEFECT					"HY_DEFECT"						// �\���Ǘ�
#define HY_TANSHO					"HY_TANSHO"						// �T�����
#define HY_DEFLIST					"HY_DEFLIST"					// �r�ꗗ�\�����
//#define HY_DEFMAP					"HY_DEFMAP"						// �o���P�� �r�_�}�b�v���

// ����@�\
#define SO_GAMENN					"SO_GAMENN"						// ������
#define SO_TRAND					"SO_TRAND"						// �g�����h���
#define SO_DEFMAP					"SO_DEFMAP"						// �����P�� �r�_�}�b�v���

// ��͋@�\
#define KA_PARSET					"KA_PARSET"						// �p�����[�^�ݒ���
#define KA_PARCOLOR					"KA_PARCOLOR"					// �摜�����G���W���p�p�����[�^�ݒ���

//***************************************************************
// �N���v��
//***************************************************************
//=======================================
// ���ʃ^�X�N�@�\
//=======================================
// KT_LEDMGR (�A�C�e�b�NTCP/IP)
#define FACT_KT_LEDMGR_01			2401					// �p�����[�^�ύX�v��		(��TO_DEFECT)
#define FACT_KT_LEDMGR_02			2402					// �ݒ�v��(�\�ʏ�Ԏw��)	(��TO_DEFECT)
#define FACT_KT_LEDMGR_03			2403					// �ݒ�v��(���ʎw��)		(��TO_DEFECT)
#define FACT_KT_LEDMGR_04			2404					// �_���E�����w��			(��TO_DEFECT)
#define FACT_KT_LEDMGR_05			2405					// �_���ݐώ��ԃ��Z�b�g		(��SO_GAMENN)

// KT_LLBGMG (�A�C�e�b�NTCP/IP)
#define FACT_KT_LLBGMG_01			2801					// �p�����[�^�ύX�v��		(��TO_DEFECT)
#define FACT_KT_LLBGMG_02			2802					// �ݒ�v��(�\�ʏ�Ԏw��)	(��TO_DEFECT)
#define FACT_KT_LLBGMG_03			2803					// �ݒ�v��(���ʎw��)		(��TO_DEFECT)
#define FACT_KT_LLBGMG_04			2804					// �_���E�����w��			(��TO_DEFECT)
#define FACT_KT_LLBGMG_05			2805					// �_���ݐώ��ԃ��Z�b�g		(��SO_GAMENN)

//=======================================
// �����@�\
//=======================================
// TO_DEFECT
#define FACT_TO_DEFECT_01			2101					// �p�����[�^�ύX�v�� (��KA_PARSET)
#define FACT_TO_DEFECT_02			2102					// �����J�n�v�� (��SO_GAMENN)
#define FACT_TO_DEFECT_03			2103					// ������~�v�� (��SO_GAMENN)
#define FACT_TO_DEFECT_04			2104					// �ғ���Ԏ擾�v�� (��SO_GAMENN)
//#define FACT_TO_DEFECT_05			2105					// �����񖢎g�p
#define FACT_TO_DEFECT_06			2106					// ����J�n�A���T�[ (��HT_DEFECT)
#define FACT_TO_DEFECT_07			2107					// �����~�A���T�[ (��HT_DEFECT)
//#define FACT_TO_DEFECT_08			2108					// ����p�����[�^�ύX�A���T�[ (��HT_DEFECT)
#define FACT_TO_DEFECT_09			2109					// �����Ԗ⍇���� (��HT_DEFECT)
//#define FACT_TO_DEFECT_10			2110					// �t���[���摜�擾���s�ʒm (��HT_DEFECT)
#define FACT_TO_DEFECT_11			2111					// �R�C������M�ʒm (��TO_GETDAT)
#define FACT_TO_DEFECT_12			2112					// �O�H���r��񏑍��݊���(��TO_MAEDAT)
#define FACT_TO_DEFECT_13			2113					// ���я������ݏ󋵖⍇�� (��TO_PUTDAT)
#define FACT_TO_DEFECT_14			2114					// ���I�����ԕ� (��TO_CAMERA)
#define FACT_TO_DEFECT_15			2115					// �C�j�V�����`����M�ʒm (��TO_PROCON)
//#define FACT_TO_DEFECT_16			2116					// �J�b�g�͈͏��ʒm (��TO_PUTDAT)
//#define FACT_TO_DEFECT_17			2117					// �����񖢎g�p
//#define FACT_TO_DEFECT_18			2118					// �����񖢎g�p
//#define FACT_TO_DEFECT_19			2119					// �����񖢎g�p
#define FACT_TO_DEFECT_20			2120					// �{�C�X�x��o�͉����v�� (��HY_TANSHO)
#define FACT_TO_DEFECT_21			2121					// �r���J�n�v�� (��SO_GAMENN)
#define FACT_TO_DEFECT_22			2122					// �r����~�v�� (��SO_GAMENN)
#define FACT_TO_DEFECT_23			2123					// �r���J�n�A���T�[ (��HT_DEFECT)
#define FACT_TO_DEFECT_24			2124					// �r����~�A���T�[ (��HT_DEFECT)
// FACT_TO_DEFECT_30�ȍ~��TO_LINKSW
#define FACT_TO_DEFECT_30			2130					// �J���������N�X�C�b�`�ݒ芮���ʒm (��TO_LINKSW)
#define FACT_TO_DEFECT_31			2131					// �r���Q���ݒ芮���ʒm (��TO_LINKSW)
#define FACT_TO_DEFECT_32			2132					// �J�����I��ݒ芮���ʒm (��TO_LINKSW)

// TO_CAMERA
#define FACT_TO_CAMERA_01			2201					// �p�����[�^�ύX�ʒm (��TO_DEFECT)
#define FACT_TO_CAMERA_02			2202					// �����p�ݒ�R�}���h (��TO_DEFECT)
#define FACT_TO_CAMERA_03			2203					// �p���X�o�͐ݒ� (��TO_DEFECT)
#define FACT_TO_CAMERA_04			2204					// AGC�ݒ� (��TO_DEFECT)
#define FACT_TO_CAMERA_05			2205					// �R�C���ؑ� (��TO_DEFECT)
#define FACT_TO_CAMERA_06			2206					// ���I�����Ԗ₢���킹 (��TO_DEFECT)
#define	FACT_TO_CAMERA_07			2207					// �Z���p�ݒ�R�}���h (��TO_DEFECT)

// TO_LINKSW
#define FACT_TO_LINKSW_01			2301					// �p�����[�^�ύX�ʒm (��TO_DEFECT)
#define FACT_TO_LINKSW_02			2302					// �����p�ݒ�R�}���h (��TO_DEFECT)
#define FACT_TO_LINKSW_03			2303					// �J�����I��ݒ�R�}���h (��TO_DEFECT)
#define FACT_TO_LINKSW_04			2304					// �r���Q���ݒ�R�}���h	(��TO_DEFECT)
#define FACT_TO_LINKSW_05			2305					// �������Z�b�g�ʒm	(��TO_DEFECT)
#define FACT_TO_LINKSW_06			2306					// AOC���x�ύX�w��	(��TO_LINKSW)

// TO_SEQDM
//#define FACT_TO_SEQDM_01			2401					// �����񖢎g�p

// TO_GETDAT
//#define FACT_TO_GETDAT_01			2501					// �p�����[�^�ύX�v�� (��TO_DEFECT)
#define FACT_TO_GETDAT_02			2502					// �R�C������M�ʒm (��TO_PROCON)

// TO_PUTDAT
//#define FACT_TO_PUTDAT_01			2601					// �p�����[�^�ύX�v�� (��TO_DEFECT)
#define FACT_TO_PUTDAT_02			2602					// �̐����ѓ`����M�ʒm (��TO_PROCON)
#define FACT_TO_PUTDAT_03			2603					// �J�b�g���ѓ`����M�ʒm�i��TO_PROCON�j
#define FACT_TO_PUTDAT_04			2604					// ���я������݊����ʒm(��TO_DEFECT)

// TO_MAEDAT
//#define FACT_TO_MAEDAT_01			2701					// �p�����[�^�ύX�v�� (��TO_DEFECT)
#define FACT_TO_MAEDAT_02			2702					// �R�C������M�ʒm (��TO_DEFECT)
#define FACT_TO_MAEDAT_03			2703					// �O�H���r����M�����ʒm (��TO_FFSERV)

// TO_PROCON
#define FACT_TO_PROCON_01			2801					// �v���R���������ё��M�v�� (��TO_PUTDAT)
#define FACT_TO_PROCON_09			2809					// �f�o�b�N�p�`����M�ʒm (��GOT_CONTROL)

// TO_FFSERV
#define FACT_TO_FFSERV_01			2901					// �������ѓ`�����M�v�� (��TO_PUTDAT)
#define FACT_TO_FFSERV_02			2902					// �O�H���r���v���ʒm (��TO_MAEDAT)


//=======================================
// ����@�\
//=======================================
// HT_DEFECT
#define FACT_HT_DEFECT_01			3101					// �p�����[�^�ύX�ʒm (��TO_DEFECT)
#define FACT_HT_DEFECT_02			3102					// �����J�n�v�� (��TO_DEFECT)
#define FACT_HT_DEFECT_03			3103					// ������~�v�� (��TO_DEFECT)
#define FACT_HT_DEFECT_04			3104					// JPEG���k���ݒ� (��TO_DEFECT)
#define FACT_HT_DEFECT_05			3105					// ��ʏ��ݒ�v�� (��TO_DEFECT)
//#define FACT_HT_DEFECT_06			3106					// �x�����s�L�����Z�� (��TO_DEFECT)
#define FACT_HT_DEFECT_07			3107					// ���s�s�\���� (��TO_DEFECT)
#define FACT_HT_DEFECT_08			3108					// ��Ԗ⍇�� (��TO_DEFECT)
//#define FACT_HT_DEFECT_09			3109					// �t���[���摜�擾�v�� (��TO_DEFECT)
//#define FACT_HT_DEFECT_10			3110					// �G�b�W���o�T�[�`�͈͕ύX�˗� (��TO_DEFECT)
//#define FACT_HT_DEFECT_11			3111					// ���S���]���f�[�^���W�v�� (��TO_DEFECT)
//#define FACT_HT_DEFECT_12			3112					// �摜���R�[�f�B���O�v�� (��TO_DEFECT)
//#define FACT_HT_DEFECT_13			3113					// ����AGC�ݒ� (��TO_DEFECT)
//#define FACT_HT_DEFECT_14			3114					// ��ʏ��ݒ�ύX�v��(������) (��TO_DEFECT)
//#define FACT_HT_DEFECT_15			3115					// �J�����Q�C���ύX�v�� (��TO_DEFECT)
//#define FACT_HT_DEFECT_16			3116					// �I�����ԕύX�ʒm (��TO_DEFECT)
#define FACT_HT_DEFECT_19			3119					// �����s�ݒ�̃L�����Z���v�� (��TO_DEFECT)
#define FACT_HT_DEFECT_20			3120					// ���������J�E���^�ݒ�v�� (��TO_DEFECT)
#define FACT_HT_DEFECT_21			3121					// �p�����[�^���O�Ǎ��v�� (��TO_DEFECT)
#define FACT_HT_DEFECT_22			3122					// �r���J�n�v�� (��TO_DEFECT)
#define FACT_HT_DEFECT_23			3123					// �r����~�v�� (��TO_DEFECT)

//=======================================
// �\���@�\
//=======================================
// HY_DEFECT
#define FACT_HY_DEFECT_01			5101					// �p�����[�^�ύX�ʒm (��TO_DEFECT)
#define FACT_HY_DEFECT_02			5102					// �C�j�V�����ʒm (��TO_DEFECT)
#define FACT_HY_DEFECT_03			5103					// �O�H���r���Ǎ��ʒm (��TO_DEFECT)
// 2023.11.17 �ŏI�H���������u���@�\�����̂S -------------------->>>>>
#define FACT_HY_DEFECT_04			5104					// �O�H���r���Ǎ������ʒm (��HY_DEFECT)
// 2023.11.17 �ŏI�H���������u���@�\�����̂S <<<<<--------------------
// 2024.07.26 Kouno 2CGL�������u�Ӄ֐��x����@�\����(������) ---------------->>>>>
#define FACT_HY_DEFECT_05			5105					// ���������ה����ʒm (��TO_LONGCHK)
// 2024.07.26 Kouno 2CGL�������u�Ӄ֐��x����@�\����(������) <<<<<----------------

// HY_TANSHO
#define FACT_HY_TANSHO_01			5201					// �p�����[�^�ύX�ʒm (��HY_DEFECT)
#define FACT_HY_TANSHO_02			5202					// �C�j�V�����ʒm (��HY_DEFECT)

// HY_DEFLIST
#define FACT_HY_DEFLIST_01			5301					// �p�����[�^�ύX�ʒm (��HY_DEFECT)
//#define FACT_HY_DEFLIST_02			5302					// �C�j�V�����ʒm (��HY_DEFECT)

// HY_DEFMAP
//#define FACT_HY_DEFMAP_01			5401					// �p�����[�^�ύX�ʒm (��HY_DEFECT)
//#define FACT_HY_DEFMAP_02			5402					// �C�j�V�����ʒm (��HY_DEFECT)
//#define	FACT_HY_DEFMAP_03			5403					// �\���X�V�v�� (��HY_DEFECT)

//=======================================
// ����@�\
//=======================================
// SO_GAMENN
#define FACT_SO_GAMENN_01			6101					// �ғ���Ԓʒm (��TO_DEFECT)
// SO_TRAND
#define FACT_SO_TRAND_01			6201					// ��ʍX�V (��TO_DEFECT)
// SO_DEFMAP
#define FACT_SO_DEFMAP_01			6301					// �p�����[�^�ύX�ʒm (��TO_DEFECT)
#define FACT_SO_DEFMAP_02			6302					// ��ʍX�V (��SO_TRAND)


//=======================================
// ��͋@�\
//=======================================
// KA_PARSET
///// 7101

// KA_CEDATA
///// 7201

//=======================================
// �S���摜�ۑ��@�\
//=======================================
// TI_RPISAVE
// OutIF_TI_ROISAVE.h�ɒ�`����ׁA�����ɂ͕s�v
//#define FACT_TI_RPISAVE_01			7101					// �����J�n�v�� (��TO_DEFECT)
//#define FACT_TI_RPISAVE_02			7102					// ������~�v�� (��TO_DEFECT)
//#define FACT_TI_RPISAVE_03			7103					// �B���J�n/��~�v�� (��TO_DEFECT)
//#define FACT_TI_RPISAVE_04			7104					// ���R�C����񎖑O�ʒm (��TO_DEFECT)
//#define FACT_TI_RPISAVE_05			7105					// �O���R�C���ؑ֒ʒm (��TO_DEFECT)
//#define FACT_TI_RPISAVE_06			7106					// �₢���킹�v�� (��TO_DEFECT)
//#define FACT_TI_RPISAVE_07			7107					// �ē����v�� (��TO_DEFECT)
//#define FACT_TI_RPISAVE_08			7108					// �r���Q�������ʒm (��TO_DEFECT)

//***************************************************************
// DIV ��`
//   ToHtCommon.h �Ŗ��̂��擾�\
//***************************************************************
//=======================================
// �V�X�e�����
//=======================================	
enum EM_DIV_SYS {
	DIV_SYS_INIT	= 1,									// [�V�X�e�����] ��~���[�h
	DIV_SYS_IDLE,											// [�V�X�e�����] ��������
	DIV_SYS_INITSTART,										// [�V�X�e�����] �����J�n��������
	DIV_SYS_WAITSTART,										// [�V�X�e�����] �����J�n�҂�

	DIV_SYS_RUN,											// [�V�X�e�����] ������
	
	DIV_SYS_WAITSTOP,										// [�V�X�e�����] ������~�҂�
	DIV_SYS_STOP,											// [�V�X�e�����] ������~
	DIV_SYS_RETRY											// [�V�X�e�����] ���������ċN����
};

//=======================================
// �����@�\�֘A
//=======================================
// �����@�\���
enum EM_DIV_CYCLESTAT {
	DIV_CYCLESTAT_NON  = 1,									// [�����@�\���] �ΏۊO (SPM�J������)
	DIV_CYCLESTAT_OK   = 0,									// [�����@�\���] ����
	DIV_CYCLESTAT_STOP = -9									// [�����@�\���] ��~  (�Жʈُ�̏ꍇ�A������~��)
};

//=======================================
// ���W�b�N�p������ �ғ����
//=======================================	
enum EM_DIV_PARA_KADOU {
	DIV_PARA_KADOU_STOP = 0,								// [�ғ����] ��~
	DIV_PARA_KADOU_WAITSTOP,								// [�ғ����] ��~��
	DIV_PARA_KADOU_WAITSTART,								// [�ғ����] �J�n��
	DIV_PARA_KADOU_START,									// [�ғ����] ������
};

//=======================================
// �S������
//=======================================
enum EM_DIV_HANTEI 
{
	DIV_HANTEI_NON = -1,									// [�S������] �֌W����

	DIV_HANTEI_BK = 0,										// [�S������] �o�b�N�A�b�v
	DIV_HANTEI_HT,											// [�S������] �P������
	DIV_HANTEI_CY,											// [�S������] ��������
	DIV_HANTEI_PR											// [�S������] �p����������
};

//=======================================
// �\���Ǘ��̎�M���
//=======================================
enum EM_DIV_HY_RECVST
{
	DIV_HY_RECVST_SKIP = -1,								// [��M���] �X�L�b�v (�����x��ɂ���M�f�[�^�j�����)
	DIV_HY_RECVST_NON = 0,									// [��M���] ����M
	DIV_HY_RECVST_OK										// [��M���] �����M
};

//=======================================
// LED�Ɩ����u �J���[ 
//=======================================
enum EM_DIV_LED_COLOR
{
	DIV_LED_COLOR_R = 0,									// [LED�Ɩ����u] R
	DIV_LED_COLOR_G1,										// [LED�Ɩ����u] G1
	DIV_LED_COLOR_G2,										// [LED�Ɩ����u] G2
	DIV_LED_COLOR_B1,										// [LED�Ɩ����u] B1
	DIV_LED_COLOR_B2,										// [LED�Ɩ����u] B2

	//DIV_LED_COLOR_W = 0,									// [LED�Ɩ����u] W
	DIV_LED_COLOR_END										// �I�[�Ŏg�p���邽�ߒ���
};

//=======================================
// LED�Ɩ����u ���j�b�g 
//=======================================
enum EM_DIV_LED_UNIT
{
	DIV_LED_UNIT_R = 0,										// ���j�b�g1 [RGG]
	DIV_LED_UNIT_G = DIV_LED_UNIT_R,						// 
	DIV_LED_UNIT_B,											// ���j�b�g2 [ BB]
	DIV_LED_UNIT_END										// �I�[�Ŏg�p���邽�ߒ���
};

#define MAX_LED_CH			3								// 1���j�b�g�ӂ�̃`�����l����
#define MAX_LED_TEMP_UNIT	3								// ���x���j�b�g��

//=======================================
// �摜��� (�G���W������o�͂����摜�Z�b�g��)
//=======================================
enum EM_DIM_IMAGE_KIND
{
	DIM_IMAGE_KIND_R = 0,									//
	DIM_IMAGE_KIND_G,
	DIM_IMAGE_KIND_B,
	DIM_IMAGE_KIND_GB
};

//=======================================
// �J�b�g�敪��
//=======================================
enum EM_DIV_CUT
{
	DIV_CUT_FCUT = 1,										// F�J�b�g
	DIV_CUT_LCUT,											// L�J�b�g
	DIV_CUT_DIV,											// ����
	DIV_CUT_WPD_OTHER										// �َ�_

	//DIV_CUT_CUT = 1			0								// ���i ��[-���[�ʒu
};

#define	ROW_KIND_UPPER		0x01							// ���i��[
#define	ROW_KIND_BOTTOM		0x02							// ���i���[
#define	ROW_KIND_OTHER		0x04							// �َ�_
#define	ROW_KIND_YOSETU		0x08							// �n�ړ_

//=======================================
// SPM�����敪
//=======================================
enum EM_DIV_SPM_STAT
{
	DIV_SPM_INI = -1,										// SPM���������l
	DIV_SPM_FALSE = 0,										// SPM��������
	DIV_SPM_TRUE											// SPM�����L��
};

//=======================================
// ���ъi�[�敪
//=======================================
enum ENUM_END_MODE {
	DIV_END_NON = 0,												// �s��
	DIV_END_NORMAL = 1,												// �ʏ�؂�ւ�
	DIV_END_BACKUP,													// �o�b�N�A�b�v�ؑ�(�����p�R�C�������Ǘ�����ɓ��B��)
	DIV_END_STOP_MANUAL,											// �蓮��~
	DIV_END_STOP_AUTO												// ������~
};

//=======================================
// �v���R���̐��敪
//=======================================
enum EM_DIV_PRO_CUT
{
	DIV_PRO_CUT_NORMAL = 0,									// [�v���R���̐��敪] �ʏ�J�b�g
	DIV_PRO_CUT_FRONT_UNION,								// [�v���R���̐��敪] �O�R�C���ƌ���
	DIV_PRO_CUT_TAIL_UNION,									// [�v���R���̐��敪] ��R�C���ƌ���
	DIV_PRO_CUT_UNION									    // [�v���R���̐��敪] ����(�O�R�C���A���R�C��)����
};

//***************************************************************
// �@���� C++�̂�
//***************************************************************
// ��� (�K���}�X�^�[�ƍ��ڂ̕��т����킹�Ă�����)
struct TYP_STATUS_READ
{	

#ifndef LOCAL												// �I�����C��
	TYP_STATUS_INF		Ht_App[NUM_MEN][NUM_HANTEI];							// ����@�\ App
#else														// ���[�J���e�X�g
	TYP_STATUS_INF		Ht_App[NUM_MEN][5];										// ����@�\ App
#endif
	TYP_STATUS_INF		Ht_Para_App[NUM_ALL_HTPARA];							// ���胍�W�b�N�p�������@�\ App

	TYP_STATUS_INF		Ht_Cannot[NUM_ALL_HANTEI];								// ����PC ���s�s�\
	TYP_STATUS_INF		AppEnable_Ht[NUM_ALL_HANTEI];							// ����PC �N��
	TYP_STATUS_INF		HbSend_Ht[NUM_ALL_HANTEI];								// ����PC �n�[�g�r�[�g
	TYP_STATUS_INF		ApErr_Ht[NUM_ALL_HANTEI];								// ����PC �A�v���P�[�V�����ُ� (�C�x���g���O)
	TYP_STATUS_INF		EvtLog_Ht[NUM_ALL_HANTEI];								// ����PC �C�x���g���O�ňُ팟�o (�C�x���g���O)
	TYP_STATUS_INF		HddErr_Ht[NUM_ALL_HANTEI];								// ����PC HDD�ُ�

	// PC���
	struct TYP_PC_ST1 {
		TYP_STATUS_INF		AppEnable;											// �N��
		TYP_STATUS_INF		HbSend;												// �n�[�g�r�[�g
		TYP_STATUS_INF		ApErr;												// �A�v���P�[�V�����ُ� (�C�x���g���O)
		TYP_STATUS_INF		EvtLog;												// �C�x���g���O�ňُ팟�o (�C�x���g���O)
	};
	struct TYP_PC_ST2 : public TYP_PC_ST1 {
		TYP_STATUS_INF		HddErr;												// HDD�ُ�
	};

	TYP_PC_ST2			To;														// ����PC
	TYP_PC_ST2			Bk;														// �o�b�N�A�b�vPC
	TYP_PC_ST2			Rps;													// �S���ۑ�PC
	TYP_PC_ST2			Rpe[3];													// �S������PC
	TYP_PC_ST2			Hy[2];													// �\��PC
	TYP_PC_ST2			So;														// ����PC

	TYP_STATUS_INF		Ht_NonAnswer[NUM_ALL_HANTEI];							// ����PC �������� (�������f)
	TYP_STATUS_INF		Ht_Ans[NUM_MEN];										// �Жʔ��� �ُ픭��

	TYP_STATUS_INF		CamGrab[NUM_CAMERA];									// �J���� �摜���� (1Port2��)
	TYP_STATUS_INF		CamTempSt[NUM_CAMERA];									// �J���� ���x�ُ�
	TYP_STATUS_INF		CamTemp[NUM_CAMERA];									// �J���� ���x
	TYP_STATUS_INF		CamVolt[NUM_CAMERA];									// �J���� �d��
	TYP_STATUS_INF		CamGain[MAX_CAMANGLE - 1][NUM_CAMERA];					// �J�����Q�C��[RGB][�J�����䐔]
	TYP_STATUS_INF		CamExpTime[MAX_CAMANGLE - 1][NUM_CAMERA];				// �J���� �I������[RGB][�J�����䐔]
	TYP_STATUS_INF		CamBrValue[MAX_CAMANGLE - 1][NUM_CAMERA];				// �J���� �P�x[RGB][�J�����䐔]


	struct _LED {													// LED���u
		TYP_STATUS_INF		Connect[NUM_MEN][NUM_LIGHT];						// �ʐM
		struct _CH {
			TYP_STATUS_INF		Break[NUM_MEN][DIV_LED_COLOR_END];				// �f�����
			TYP_STATUS_INF		TempWar[NUM_MEN][DIV_LED_COLOR_END];			// ���x���M
			TYP_STATUS_INF		TempErr[NUM_MEN][DIV_LED_COLOR_END];			// ���x�ُ�
			TYP_STATUS_INF		OnOff[NUM_MEN][DIV_LED_COLOR_END];				// �_��������� (0:ON  1:OFF)
			TYP_STATUS_INF		ValNow[NUM_MEN][DIV_LED_COLOR_END];				// �ݒ�P�x�l
			TYP_STATUS_INF		TotalTime[NUM_MEN][DIV_LED_COLOR_END];			// �ғ��݌v���� [��]
		} ch;																	// Ch��
		struct _COLOR {
			TYP_STATUS_INF		TempSt[NUM_MEN][NUM_LED_COLOR][MAX_LED_TEMP_UNIT];	// ���x���
			TYP_STATUS_INF		Temp[NUM_MEN][NUM_LED_COLOR][MAX_LED_TEMP_UNIT];	// ���x
		} color;																// 3�F
	} led;

	TYP_STATUS_INF		LedRunOff[NUM_MEN];										// LED�Ɩ�����01(���C���ғ���)

	TYP_STATUS_INF		CamSwitch_NonAnswer;									// �J���������N�X�C�b�`�ʐM �������� (�������f)
	TYP_STATUS_INF		CamControl;												// �J���������N�X�C�b�` �J�����R���g���[��FPGA�ʐM
	TYP_STATUS_INF		CamSwitch;												// �J���������N�X�C�b�` �X�C�b�`FPGA�ʐM
	TYP_STATUS_INF		CamFPGADieTempSt[NUM_CAMLINKSW*MAX_CAMLINKSW_FPGA];		// �J���������N�X�C�b�` �_�C���x�ُ�
	TYP_STATUS_INF		CamFPGADieTemp[NUM_CAMLINKSW*MAX_CAMLINKSW_FPGA];		// �J���������N�X�C�b�` �_�C���x
	TYP_STATUS_INF		CamAmbientTempSt[NUM_CAMLINKSW*MAX_CAMLINKSW_FPGA];		// �J���������N�X�C�b�` ���͉��x�ُ�
	TYP_STATUS_INF		CamAmbientTemp[NUM_CAMLINKSW*MAX_CAMLINKSW_FPGA];		// �J���������N�X�C�b�` ���͉��x
	TYP_STATUS_INF		CamSwStat[NUM_CAMLINKSW*MAX_CAMLINKSW_FPGA*MAX_CAMLINKSW_INPUT][2];		// �J���������N�X�C�b�`_CH���[���͐�][B/MF]

	TYP_STATUS_INF		ChilerPiStop[3];										// �`���[���u �^�]��� [0:�\ 1:�� 2:BK]
	TYP_STATUS_INF		ChilerPiErr[3];											// �`���[���u �ُ��� [0:�\ 1:�� 2:BK]

	struct _CHILER {															// �`���[���u
		TYP_STATUS_INF		Connect[NUM_MEN];									// �ʐM
		TYP_STATUS_INF		Sts[NUM_MEN];										// �^�]���
		TYP_STATUS_INF		Err[NUM_MEN];										// ���u���
		TYP_STATUS_INF		TempSt[NUM_MEN];									// ���x���
		TYP_STATUS_INF		Temp[NUM_MEN];										// ���x
	} chiler;

	struct _AIRBLOW {															// �G�A�h���C�����u
		TYP_STATUS_INF		AirBlowPiStop;										// �G�A�h���C�����u �^�]���
		TYP_STATUS_INF		AirBlowPiErr;										// �G�A�h���C�����u �ُ���
	} airblow[NUM_MEN];

	//TYP_STATUS_INF		CoolerPiErr[NUM_MEN];									// �N�[���[���uPI[0:�\ 1:��]

	TYP_STATUS_INF		Procon;													// �v���R�� �ʐM���

	TYP_STATUS_INF		DataMem_Ap;												// �f�[�^�������ʐM
	TYP_STATUS_INF		DataMem_Hb;												// �f�[�^������HB
	TYP_STATUS_INF		SeqState_War;											// �V�[�P���T���_����
	TYP_STATUS_INF		SeqState_Err;											// �V�[�P���T���_�d�̏�
	TYP_STATUS_INF		SeqState;												// �V�[�P���T���
	TYP_STATUS_INF		SeqKadou;												// �V�[�P���T�ғ����
	TYP_STATUS_INF		UdpSeq;													// �V�[�P���TUDP����ُ�

	TYP_STATUS_INF		FFserv;													// FF�T�[�o�[ �ʐM���
	TYP_STATUS_INF		FFConnectErr;											// FF�T�[�o�[�C�j�V������


	TYP_STATUS_INF		Div_Kadou;												// �ғ����
	TYP_STATUS_INF		Div_Unten;												// �^�]���[�h
	TYP_STATUS_INF		Div_Sogyo;												// ���Ə��
	TYP_STATUS_INF		Div_PlgMode;											// PLG�敪

	TYP_STATUS_INF		Line_Mei;												// ����No
	TYP_STATUS_INF		Line_No;												// �R�C��No
	TYP_STATUS_INF		Line_Len;												// �ʔ���
	TYP_STATUS_INF		Line_ZanLen;											// �ʔc��
	TYP_STATUS_INF		Line_Speed;												// ���C�����x

	TYP_STATUS_INF		Scode[NUM_MEN];											// �����\�ʏ��
	TYP_STATUS_INF		Pcode[NUM_MEN];											// �����p�^�[��

	TYP_STATUS_INF		Coil_Init;												// �����J�n1�{��
	TYP_STATUS_INF		Coil_ReStart;											// ���������ċN��
	TYP_STATUS_INF		Coil_Timing_Err;										// �����J�n�^�C�~���O�s��

	TYP_STATUS_INF		Coil_Ken;												// �����p�R�C�����
	TYP_STATUS_INF		Coil_Dsp;												// �\���p�R�C�����

	TYP_STATUS_INF		MaxLen_Ken;												// �����p�R�C����������B
	TYP_STATUS_INF		MaxLen_Dsp;												// �\���p�R�C����������B

	TYP_STATUS_INF		PlgLengthCheckKen;										// �����pPLG�������ُ�
	TYP_STATUS_INF		PlgLengthCheckDsp;										// �\���pPLG�������ُ�
	TYP_STATUS_INF		ProHoseiRitu;											// �v���R�����␳��

	TYP_STATUS_INF		ZiProCreateErr;											// �v���R���������ѐ������s
	TYP_STATUS_INF		ZiFfCreateErr;											// FF�������ё��M���s

	TYP_STATUS_INF		SaveRpiErr[NUM_MEN];									// �S���摜�ۑ�

	TYP_STATUS_INF		DB_Tran_A;												// DB�g�����U�N�V�������O���� A
	TYP_STATUS_INF		DB_Tran_B;												// DB�g�����U�N�V�������O���� B

	TYP_STATUS_INF		DB_BcpErr;												// �摜�������ݎ��s

	TYP_STATUS_INF		DB_SaveMax[NUM_MEN];									// �������ѕۑ�������B
	TYP_STATUS_INF		DB_SaveSkip[NUM_MEN];									// �������ѕۑ��X�L�b�v
	TYP_STATUS_INF		FrameSkip[NUM_MEN];										// ���������
	TYP_STATUS_INF		EdgeLost[NUM_MEN];										// �G�b�W���o�ʒu�s�m��
	TYP_STATUS_INF		FrameUnmatch[NUM_MEN];									// �t���[��No����

	TYP_STATUS_INF		PatoNg[NUM_MEN];										// �p�g���C�g�o���r�t��

	TYP_STATUS_INF		SecurityDoor;											// �����
	TYP_STATUS_INF		SecurityKey;											// �����

	TYP_STATUS_INF		DaisyaSt[NUM_MEN];										// ��Ԉʒu���
	TYP_STATUS_INF		DaisyaErr[NUM_MEN];										// ��Ԉʒu�ُ�

	struct _TEMPMT {															// ���x�p�l�����[�^�[
		TYP_STATUS_INF		TempMtConnect;										// �ʐM
		struct _UNIT {
			TYP_STATUS_INF	Temp[NUM_TEMPMT_UNIT];								// ���x
			TYP_STATUS_INF	TempSt[NUM_TEMPMT_UNIT];							// ���x���
			TYP_STATUS_INF	Unten[NUM_TEMPMT_UNIT];								// �^�]���
			TYP_STATUS_INF	Status[NUM_TEMPMT_UNIT];							// �X�e�[�^�X
		} unit;
	} tempmt[1];

	TYP_STATUS_INF		ToyuSprSt;												// �h���X�v���[ ���t���

	TYP_STATUS_INF		RpiImgComp_BcpErr[NUM_MEN];								// �S���摜���k���s

	TYP_STATUS_INF		ParaKadou[NUM_ALL_HTPARA];								// ���W�b�N�p�������@�\ �ғ����
	TYP_STATUS_INF		ParaCam[NUM_ALL_HTPARA][NUM_CAMLINK_BOARD];				// ���W�b�N�p�������@�\ ���̓J����No
	TYP_STATUS_INF		ParaState[NUM_ALL_HTPARA][NUM_CAMLINK_BOARD];			// ���W�b�N�p�������@�\ ���̓J�����I�����
	TYP_STATUS_INF		FrameGrabStop[NUM_MEN];									// �B���t���[����~
	TYP_STATUS_INF		DaisyaEx_St[NUM_MEN];									// ��Ԉʒu�I�����C���ȊO		20210312
	TYP_STATUS_INF		CamGcpErr[NUM_CAMERA];									// �J���� GCP�ُ�

// 2024.07.26 Kouno 2CGL�������u�Ӄ֐��x����@�\����(������) ---------------->>>>>
	TYP_PC_ST2			LongPd;													// ������PC
// 2024.07.26 Kouno 2CGL�������u�Ӄ֐��x����@�\����(������) <<<<<----------------
};


//***************************************************************
// �V�X�e�����ʍ\����
//***************************************************************
//=======================================
// ��{�r��� �\���� (100byte+��)
//=======================================
struct DETECT_BASE_DATA
{
	int						nKizuNo;						// �rNo (1�`�̘A��)
	
	WORD					TorB;							// �\���敪 (0:�\ 1:��)
	WORD					nTid;							// �r��EdasysID
	WORD					nGid;							// �O���[�hEdasysID

	WORD					cycle_div;						// �����r�敪(0:����r, 1:�ȈՎ����r,�A���r )
	WORD					dsws;							// DSWS�敪 (0:DS 1:WS)
	WORD					camset;							// �J�����Z�b�gID (1�`)	�\DS, �\DQ, �\WQ, �\WS, ��DS, ��DQ, ��WQ, ��WS

	float					y;								// ������[����̈ʒu[mm]
	float					y_out;							// �o�������R�C����[����̈ʒu[mm]	�� �����ł�y�Ɠ��������B �\���Ǘ��ŁA�o�������ɐU�蒼��
	float					ds;								// DS���G�b�W����̈ʒu[mm]
	float					ws;								// WS���G�b�W����̈ʒu[mm]
	float					dsc;							// �Z���^�[����̋���[mm]
	float					wsc;							// �Z���^�[����̋���[mm]

	int						speed;							// ���x[mpm]
	int						pitch;							// �s�b�`[mm]
	float					roolkei;						// ���[���a[mm]

	float					aria;							// �ʐ� [mm^2]
	int						kizu_wid;						// �r�� [mm]
	int						kizu_len;						// �r���� [mm]

	float					h_res;							// �c����\[mm/pixel]
	float					w_res;							// ������\[mm/pixel]
	DWORD					nFrameNo;						// �t���[��No

	// �t���[���摜�̍��ォ��̈ʒu
	short					frm_x_min;						// �t���[���O��Xmin[pixel]
	short					frm_x_max;						// �t���[���O��Xmax[pixel]
	short					frm_y_min;						// �t���[���O��Ymin[pixel]
	short					frm_y_max;						// �t���[���O��Ymax[pixel]

	// �t���[���摜�̍��ォ��̊Ԉ��������ʒu
	short					cull_frm_x_min;					// �Ԉ����t���[���O��Xmin[pixel]
	short					cull_frm_x_max;					// �Ԉ����t���[���O��Xmax[pixel]
	short					cull_frm_y_min;					// �Ԉ����t���[���O��Ymin[pixel]
	short					cull_frm_y_max;					// �Ԉ����t���[���O��Ymax[pixel]

	// �؂�o���摜�֌W (���_�́A�؂�o���摜�̍��ォ��̈ʒu)
	short					box_x_min;						// �O��Xmin[pixel]		(�؂�o���摜�̍��ォ��̈ʒu) (�}�C�i�X�l���肦��)
	short					box_x_max;						// �O��Xmax[pixel]
	short					box_y_min;						// �O��Ymin[pixel]
	short					box_y_max;						// �O��Ymax[pixel]
	short					center_x;						// �r���S�ʒuX [pixel]  (�؂�o���摜�̍��ォ��̈ʒu)
	short					center_y;						// �r���S�ʒuY [pixel]
	WORD					h_size;							// �c��f��[pixel]
	WORD					w_size;							// ����f��[pixel]

	// �����܂ł� 108byte (�c84byte�ȓ�)------------------------------------------

	// �t�я��
	WORD					nTYuu;							// �r��D��x (1�ŋ�)
	WORD					nGYuu;							// �O���[�h�D��x (1�ŋ�)
	int						nWidAbsPos;						// ��������Έʒu
	short					nWidDiv;						// �������敪 (0:DS �` 7:WS)
	short					nObjTansho;						// �T����ʏo�͑Ώ��r(1:�Ώ�)
	short					nObjDeflist;					// �d���׈ꗗ��ʑΏ��r(1:�Ώ�)
	short					nModeDeflist;					// �d���׈ꗗ�����\���Ώ��r  (0:�����g����(�����r�폜)  1:�����g�\��(�蓮�r�폜))  
	short					time[6];						// �r�������� 0:�N 1:�� 2:�� 3:�� 4:�� 5:�b
	// ���摜�̐؂�o���摜�֌W (���_�́A�؂�o���摜�̍��ォ��̈ʒu) ���Ԉ����O
	short					base_box_x_min;					// ���摜_�O��Xmin[pixel]		(�؂�o���摜�̍��ォ��̈ʒu) (�}�C�i�X�l���肦��)
	short					base_box_x_max;					// ���摜_�O��Xmax[pixel]
	short					base_box_y_min;					// ���摜_�O��Ymin[pixel]
	short					base_box_y_max;					// ���摜_�O��Ymax[pixel]
	short					base_center_x;					// ���摜_�r���S�ʒuX [pixel]  (�؂�o���摜�̍��ォ��̈ʒu)
	short					base_center_y;					// ���摜_�r���S�ʒuY [pixel]
	WORD					base_h_size;					// ���摜_�c��f��[pixel]
	WORD					base_w_size;					// ���摜_����f��[pixel]
	float					base_h_res;						// ���摜_�c����\[mm/pixel]
	float					base_w_res;						// ���摜_������\[mm/pixel]
	int						nShrinkRatio;					// �k���� (1, 2, 4)

// 2023.09.14 �摜�������u H�ES�\�t�g���� -------------------->>>>>
//	char					cYobi[28];

	int						nIFMergeFlg;					// �t���[���ԘA���t���O(0:�P���r 1:�A���r�擪 2:�A���r�擪�ȊO)
	char					cYobi[24];
// 2023.09.14 �摜�������u H�ES�\�t�g���� <<<<<--------------------

};

//// �R�C����{��� (�R�C����񂩂��r���ŕK�v�ȏ��̂ݎ��o��)
struct COIL_BASEDATA
{
	// ��{�� (TO_GETDAT�ŃZ�b�g)
	char				cMeiNo[SIZE_MEI_NO+1];				// ����No
	char				cCoilNoIn[SIZE_COIL_NO+1];			// �����R�C��No+NULL  (�R�C��No) �K�C�_���X�p
	char				cCoilKey[SIZE_COIL_KEY];			// �R�C������ӂɓ��肷�邽�߂̃L�[   (�R�C��No)
	char				cYobi1[2];							// �o�E���_�����킹

	int					nLenIn;								// �����R�C���� [m] (��ʏ��̒���)
	int					nAtu;								// �� [��m]
	int					nWid;								// �� [mm](=�ʏ��ʂ̔������A��ʏ�񂪖����ꍇ���́A�ő�����Z�b�g���Ă���)				

	int					nCutLenF;							// �����t�����g�[����[mm]
	int					nCutLenT;							// �����e�[���[����[mm]
	char				cYobi2[4];							// �\��
	double				dNobiSPM;							// SPM�L�ї�[10^2%]
	int					nLenOut;							// �o���R�C����[m]
	char				cYobi3[20];							// �\��

	// �g���� (TO_DEFECT�ŃZ�b�g)
};

//// �R�C����{�ݒ� (�R�C����񓙂����肱�񂾏��)
struct COIL_SETTING
{
	bool				bWalm;								// �E�H�[�}�ދ敪 (ture:�E�H�[�}��)
	bool				bDefScode[NUM_MEN];					// �����\�ʏ�� �f�t�H���g�l�g�p (true:��������ُ̈�Ńf�t�H���g�l�g�p��)
	bool				bDefPcode[NUM_MEN];					// �����p�^�[�� �f�t�H���g�l�g�p (true:��������ُ̈�Ńf�t�H���g�l�g�p��)
	char				yobi[3];

	int					scode[NUM_MEN];						// �����\�ʏ�� (�\/��)(1�I���W��)
	int					pcode[NUM_MEN];						// �����p�^�[�� (�\/��)(1�I���W��)
	int					scode_spmoff[NUM_MEN];				// SPM�J�����̌����\�ʏ�� (�\/��)(1�I���W��)
	int					pcode_spmoff[NUM_MEN];				// SPM�J�����̌����p�^�[�� (�\/��)(1�I���W��)
	char				cYobi[24];							// �\��
};


//=======================================
// ��ʕ\���p �\����
//=======================================
/////////////////////
//// �R�C����񋤒ʕ� 128 + 1024 byte
struct COINFO_BASE
{
	char				cKizukenNo[SIZE_KIZUKEN_NO];		// �Ǘ�No
	int					scode[NUM_MEN];						// �����\�ʏ�� (1�I���W��)
	int					pcode[NUM_MEN];						// �����p�^�[�� (1�I���W��)
	int					nMapColNum;							// �}�b�v��

	int					nCoilKensa;							// ���̃R�C���ň��Ԃł��k�ގ�:1 ����:0
	char				cYobi1[84];

	// ���̊Ԃ� 1024byte --->
	COIL_BASEDATA			base;							// �V�X�e�����ʂ̃R�C�����
	PRO_TYPE_DATA_COILINF	data;							// �R�C�����
	char reserve[1024-sizeof(PRO_TYPE_DATA_COILINF)-sizeof(COIL_BASEDATA)];		// �\��
	// <--------
};



/////////////////////
//// �}�b�v�\���p (�s�~��e�[�u��)
// �f�[�^
struct DSP_TYP_COLROW								// 16 byte
{
	long					pnt;							// �r���e�[�u���ւ̃|�C���^[1�`] (0:�r�Ȃ�)
	short					nTid;							// �r��EasysID					 (0:�r�Ȃ�)
	short					nGid;							// �O���[�hEdasysID				(0:�r�Ȃ�)
	short					yobi[4];
};
// �z�u
struct DSP_MAP_COLROW
{
	struct STAGE {
		struct MEN {
			struct ROW {
				struct COL {
					short				nSt;				// �X�e�[�^�X (0:�}�b�v��, 1:�G�b�W�O)
					short				nTotalCnt;			// �Z���ɃZ�b�g�Ώۂ��r�g�[�^���� (�����A�\�������̃g�[�^������)
					short				nSetCnt;			// ���ۂɃ��X�g�ɃZ�b�g������		
					short				nYobi[1];		
					DSP_TYP_COLROW		typ[MAP_NEST_NUM];	// �D��x�̍������ɃZ�b�g

				} col[MAP_COL_NUM];							// ��
			} row[MAP_ROW_NUM];								// �s (�ő�R�C����/�ŏ��}�b�v�Ԋu)
		} men[NUM_MEN];										// �\ ��
	} stage[MAP_STAGE_NUM];									// �}�b�v�ؑ֗p (0:�ŏ��Ԋu 1:���ۂ̕\���G���A�� 2:���ۂ̕\���G���A��)
};

/////////////////////
//// �}�b�v�\���p (�s�e�[�u��)
// �f�[�^
struct DSP_TYP_ROW											// 64 byte
{
	int						nEnable;						// ����M��� (�̂́A�s�����-1�Ŕ��f���Ă����z) (0:����M 1:��M)
	EM_DIV_KADOU			emKadou;						// �ғ���� (DIV_KADOU_INIT���)

	EM_DIV_KENSA			emDspKensa;						// �\���p������� (DIV_KENSA_NON���)
	EM_DIV_KIKI				emDspKiki;						// �\���p�@���� (DIV_KIKI_OK���)

	int						nKizuCnt;						// �s�P�ʂ̕\���Ώ��r��

	int						nLen_i;							// �������� (�p�͗n�ړ_����̋���) [m]
	int						nLen_o;							// �o������ (�p�̓J�b�g����̋���) [m]
	int						nCut;							// �V���[�J�b�g�敪�i0:�V���[�J�b�g�����A1:�V���[�J�b�g���ꂽ�j
	int						nKind;							// �t����� (0:�t����񖳂� 0x01:�V���[�J�b�g)

	int						pnt[2];							// �r���ւ̃|�C���^[0:�ŏ�/1:�ő�]
	int						pntMae[2];						// �O�H���r���ւ̃|�C���^[0:�ŏ�/1:�ő�]
	
	int						yobi[3];						// �\��
};
// �z�u
struct DSP_MAP_ROW
{
	struct STAGE {
		struct MEN {
			struct ROW {	

				DSP_TYP_ROW			typ;					// �ڍ׏��

			} row[MAP_ROW_NUM];								// �s (�ő�R�C����/�ŏ��}�b�v�Ԋu)
		} men[NUM_MEN];										// �\ ��
	} stage[MAP_STAGE_NUM];									// �}�b�v�ؑ֗p (0:�ŏ��Ԋu 1:���ۂ̕\���G���A�� 2:���ۂ̕\���G���A��)
};

/////////////////////
//// �r���p
// �f�[�^
struct DSP_TYP_DEFECTINF									// 440 byte
{
	DETECT_BASE_DATA				data;					// �r���

	// VB��2�����z��̃}�[�V�����̕��@���������� ����
	//  �����̏ꍇ�AG,B,R,R-B�̏��ŃZ�b�g
	char							image_path_s [SIZE_IMAGE_PATH];		// �摜�̃p�X[��] (�摜��������ALL NULL) (��΃p�X�ł�OK�B�ɗ͑��΃p�X�ɂ��Ă�)
	char							image_path_r [SIZE_IMAGE_PATH];		// �摜�̃p�X[��] (�摜��������ALL NULL) (��΃p�X�ł�OK�B�ɗ͑��΃p�X�ɂ��Ă�)
	char							image_path_r2[SIZE_IMAGE_PATH];		// �摜�̃p�X[��2](�摜��������ALL NULL) (��΃p�X�ł�OK�B�ɗ͑��΃p�X�ɂ��Ă�)
	char							image_path_r3[SIZE_IMAGE_PATH];		// �摜�̃p�X[��3](�摜��������ALL NULL) (��΃p�X�ł�OK�B�ɗ͑��΃p�X�ɂ��Ă�)

	char reserve[440-sizeof(DETECT_BASE_DATA)-SIZE_IMAGE_PATH*4];		// �\��
};

// �z�u
struct DSP_MAP_DEFECTINF
{
	char							KizukenNo[SIZE_KIZUKEN_NO];			// �Ǘ�No
	char							cMeiNo[SIZE_MEI_NO+1];				// ����No
	char							cyobi[3];
	DWORD							num;								// ���݂̊i�[����
	DWORD							yobi;	
	struct PNT {					
		DSP_TYP_DEFECTINF			typ;
	} pnt[MAX_MAP_DEFECT_NUM];
};

/////////////////////
//// �d���׈ꗗ�\���|�C���^���p
// �f�[�^
struct DSP_TYP_DEFLIST													// 484 byte	( 44 byte + 440 byte )
{
	char					KizuKenNo[SIZE_KIZUKEN_NO];					// �Ǘ�No
	char					CoilNo[SIZE_COIL_NO+1];						// �R�C��No
	char					MeiNo[SIZE_MEI_NO+1];						// ����No
	char					yobi1[2];									// �\��
	DSP_TYP_DEFECTINF		typ;										// �r���
};


/////////////////////
//// �O�H���r�}�b�v�p
// �f�[�^
struct DSP_TYP_TCM_ROW
{
	long					pnt;										// TCM�����e�[�u���ւ̃|�C���^
	char					ff_t[2];									// FF�r��
	char					ff_g[2];									// FF�O���[�h
	char					proc[2];									// �O�H���R�[�h
	char					yobi[2];									// �\��
	short					t_id;										// �r��EdasysID
	short					g_id;										// �O���[�hEdasysID
};

// �z�u
struct DSP_MAP_ROW_TCM 
{
	struct INTVAL {
		struct TORB {
			struct ROW {

				DSP_TYP_TCM_ROW		typ;		

			} row[COIL_LEN_MAX/MAP_REPRES_MIN];							// �s (�ő�R�C����/�ŏ��}�b�v�Ԋu)
		} torb[2];														// �\ ��
	} stage[MAP_STAGE_NUM];
};



//=======================================
// �{�V�X�e���ŗL�̍\����
//=======================================

// �摜�捞�J�n�`�������������܂ł̏��v����
#define MAX_REQUIRED_TIME		10
enum EM_REQUIRED_TIME
{
	EM_TIME_HT_IMGGETSTART_TO_RECVEND = 0,								// �摜�捞�`���聨�����`�������܂ł̏��v����
	EM_TIME_HT_IMGGETSTART_TO_EXECEND,									// �摜�捞�`�������������܂ł̏��v����
	EM_TIME_HT_IMGEXECEND_TO_RECVSTART,									// �摜�����G���W�����������`���聨�����`���J�n�܂ł̏��v����
	EM_TIME_TO_RECVSTART_TO_RECVEND_CAM,								// DS(WS)���r����M�J�n�`DS(WS)���r����M�J�n�܂ł̏��v����
	EM_TIME_TO_RECVSTART_TO_RECVEND_ALL,								// DS(WS)���r����M�J�n�`WS(DS)���r����M�����܂ł̏��v����
	EM_TIME_HT_IMGGETSTART_HT_IMGEXECEND								// �摜�捞�`�摜�����G���W�����������܂ł̏��v����

};

	
//=======================================
// PC�ԒʐM
//=======================================
//-----------------------------------------------------------------------------
//// [����PC]�@�ˁ@[�\��PC]

//// �f�[�^�����
enum EM_TO_SO_DATA_KIND {
											//�Ǘ�No,�\��,�t�����
	TO_SO_DATA_KIND_STATUS = 0,				// 0 :�~:�~:��: �X�e�[�^�X���
	TO_SO_DATA_KIND_COIL_KEN,				// 1 :��:�~:��: �R�C����� (�����p)		�� PointTbl nRecWriteBase ���X�V
	TO_SO_DATA_KIND_COIL_RESULT,			// 2 :��:�~:�~: �R�C������ (1�R�C������������)
	TO_SO_DATA_KIND_COIL_DSP,				// 3 :��:�~:�~: �R�C����� (�\���p)		�� PointTbl nRecDispBase ���X�V + nDispOk ��True
	TO_SO_DATA_KIND_DETECT,					// 4 :��:��:��: �r��� (�r���+�r�摜)
	TO_SO_DATA_KIND_LENGTH,					// 5 :�~:�~:��: �ʔʒu���(�������u�ʒu�A�\����_�ʒu�A�e�\���R�C���ʔʒu���)	
	TO_SO_DATA_KIND_CUT,					// 6 :��:�~:��: �J�b�g���i�J�b�gPIO�M����M�^�C�~���O�j
	TO_SO_DATA_KIND_ALARM,					// 7 :�~:�~:��: �x����
	TO_SO_DATA_KIND_DEFLIST_CANCEL,			// 8 :��:��:�~: �d�����r���폜�v��
	TO_SO_DATA_KIND_DETECT_NUM				// 9 :��:��:�~: �r����� (�ŏ���ԓ�)
};

////// �w�b�_�[��[256byte]
struct TO_SO_HEAD {
	SOCK_BASE_HEAD			base_head;						// ���ʕ�(48byte)
	char					KizukenNo[SIZE_KIZUKEN_NO];		// �Ǘ�No (�����̎����Ă��郌�R�[�h�����ׂČ������鎖�B����L�[�������ꍇ�́A�V�K���R�[�h�Ƃ���) (�֌W�����ꍇ�́ANULL�ł��Z�b�g���Ă���������)
	long					men;							// �\:0 ��:1 (�\���֌W�����ꍇ�́A-1�ł��Z�b�g���Ă���������)
	EM_TO_SO_DATA_KIND		kind;							// �f�[�^���̋敪 ( EM_TO_SO_DATA_KIND ���g�� )

	// �f�[�^�������قǏo�Ȃ��ꍇ�ɂ����ɃZ�b�g���悤���ȁ[ (32byte)
	union 
	{
		// ��{  (0,1,4,5,6,7)
		struct FL{
			long data[8];
		} fl;
		
		// 2:�R�C������
		struct DATA_COILRESULT {
			long len;												// ������ [mm]
			long yobi[7];
		} data_coilresult;

		// 3:�R�C�����i�\���p�j
		struct DATA_COILINF_DSP {
			long nSpmUmu;											// SPM�����L��(0:���������A1:�����L��)
			long yobi[7];
		} data_coilinf_dsp;

		// 8:�d���ו\�r���폜�v��
		struct DEFLIST_CANCEL{
			long nKizuSeq;											// �L�����Z���Ώۂ��r�A�� (�\���� 1�`)
			long yobi[7];
		} deflist_cancel;
		
		// 9:�r����� (�ŏ���ԓ�)
		struct DATA_DEF_NUM {
			long len;												// ������ [mm]
			long num;												// �r�����(�ŏ���ԓ�)
			long yobi[6];
		} data_defnum;
	};
	BYTE					reserve[SIZE_SOCK_HEAD-sizeof(SOCK_BASE_HEAD)-SIZE_KIZUKEN_NO-sizeof(long)*10];
};

//// �f�[�^��
// 0:�X�e�[�^�X���
struct TO_SO_DATA_STATUS {
	int						nUnten;									// �^�]��� (DIV_UNTEN_STOP���)
	int						nKadou;									// �ғ���� (DIV_KADOU_INIT���)

	TYP_STATUS_NOWSYSTEM	status;									// �X�e�[�^�X
};	

// 1:�R�C����� (�����p)
struct TO_SO_DATA_COIL {
	COIL_BASEDATA			coilbase;								// �V�X�e�����ʃR�C�����

	PRO_TYPE_DATA_COILINF	coilinf;								// �R�C�����\����
	long					scode[NUM_MEN];							// SPM�������̌����\�ʏ�� (�\/��)(1�I���W��)
	long					pcode[NUM_MEN];							// SPM�������̌����p�^�[�� (�\/��)(1�I���W��)
	long					scode_spmoff[NUM_MEN];					// SPM�J�����̌����\�ʏ�� (�\/��)(1�I���W��)
	long					pcode_spmoff[NUM_MEN];					// SPM�J�����̌����p�^�[�� (�\/��)(1�I���W��)
	long					nMapColNum;								// �}�b�v�� (�Z�o�s�\���́A�ő�l���Z�b�g)
};

// 4,9,10:�r��� (�s��P��)
struct TO_SO_DATA_DETECT {
	long					image_len[MAX_IMAGE_CNT];				// �摜�f�[�^�� [��/��] (�ЂƂ��̉摜�T�C�Y) 

	DETECT_BASE_DATA		datas;									// �r���

// ���ǂ������g���₷�����ȁH
	BYTE					image[1];								// �摜�f�[�^�̐擪�|�C���^ (���ۂ�JPEG�摜��MAX_IMAGE_CNT�� �A�����Ă���)
};	


// 5:�ʔʒu��� (500ms�ő��M)
struct TO_SO_DATA_LENGTH {
	struct TO_SO_DATA_LENGTH_INF {
		char				KizukenNo[SIZE_KIZUKEN_NO];				// �Y���ʒu��ʔ��̊Ǘ�No (NULL��������Alen�͕s��l�ׁ̈A�g�p���Ȃ���)
		long				len;									// �Y���ʒu��ʔ��̃R�C���擪����̈ʒu [m]
	};

	TO_SO_DATA_LENGTH_INF	dsp[MAX_DISP_POS];						// �ʔʒu���
};

// ---------- �������炩�Ȃ�V�X�e���ŗL-----------
// 6:�J�b�g���
struct TO_SO_DATA_CUT {
	int						nMode;									// ��� (EM_DIV_CUT)
	int						nPos;									// �R�C����[����̈ʒu [mm]	

};

// 7:�x����
//		���̏ꍇ�́A�Ǘ�No�͖�����
struct TO_SO_DATA_ALARM {
	int						a_mode[MAX_ALARME_POS][NUM_MEN][LAMP_COL_NUM];	// �r�ڋ߃����v [�\������ʒu/��������ʒu][�\/��][��] (2:�d�̏� 1:�y�̏� 0:��~��)
};

// ������
enum EM_TO_SO_CTRL_KIND {
	TO_SO_DATA_KIND_START = 0,				// 0 : �����J�n
	TO_SO_DATA_KIND_LINE_CHANGE,			// 1 : �ŏ���Ԃ��؂�ւ����
	TO_SO_DATA_KIND_STOP					// 2 : ������~
};

// �r��񑗐M�Ǘ�������
struct TO_SO_CTRL_SEND {
	long					men;			// �\:0 ��:1
	EM_TO_SO_CTRL_KIND		ctrl_kind;		// ���䕔�̋敪 ( EM_TO_SO_CTRL_KIND ���g�� )
	char					KizukenNo[SIZE_KIZUKEN_NO];	// �Y���ʒu��ʔ��̊Ǘ�No (NULL��������Alen�͕s��l�ׁ̈A�g�p���Ȃ���)
	long					nBeforeRow;		// �O��̍s
	long					nCurrentRow;	// ���݂̍s
};


//-----------------------------------------------------------------------------
//// [����PC]�@�ˁ@[����PC]

//----------------
//// �G���W���̏�� (����˓���)
struct ENG_STAT {
//	int					nNum;						// �Z�b�g��	// ��{1�����Ȃ��͂�
//	int					nCamId[1];		// (1�I���W��)
//	float				fCamTemp[1];
//	float				fCamGain[3][1];	// (RGB)
//	float				fCamExpT[3][1];	// (RGB)

	// NCL_ANSWER_QUERY_STATE ���甲���B�S�����ꂽ�� 256byte�Ɏ��܂�Ȃ���������B�B�B

	int					state;										// NCL_ENGINE_STATE
	DWORD				camnum;										// �J�����䐔	// 1PC 2�J����
	DWORD				camid[2];									// �J�����ԍ�	// (1�I���W��)
	float				exptime[3][2];								// �I������
	float				gain[3][2];									// �Q�C��
	float				temp[2];									// �J�������x
	float				volt[2];									// �J�����d��
};

//=======================================
// �ʐM

// �����
#pragma pack (4)
// �����
struct HT_TO_HEAD_HLS_BASE {	// 256byte�Œ�
	SOCK_BASE_HEAD			base_head;								// ���ʕ�(32byte)

	// �f�[�^
	BOOL					nEngStatSet;							// �ŐV���X�V�� TRUE
	ENG_STAT				EngStat;								// �G���W�����
};
struct HT_TO_HEAD_HLS : public HT_TO_HEAD_HLS_BASE {
	BYTE	reserve[SIZE_SOCK_HEAD - sizeof(HT_TO_HEAD_HLS_BASE)];	// �\��
};

//// ����́Aaddr�͎g��Ȃ��B1�̘A���f�[�^�Ƃ���B���A��M����Ƃ��́A���܂œ����l�Ɏ�M�\
struct HT_TO_HEAD_BASE {
	SOCK_BASE_HEAD			base_head;								// ���ʕ�(32byte)
	
	DWORD					nFrameNo;								// �t���[��No (0�I���W��)
	int						nFrameSkip;								// 0:����  0�ȊO:�t���[���X�L�b�v�B(�G���W���̃G���[code���Z�b�g)
	int						nEdgeNg;								// 0:����  0�ȊO:�G�b�W���o���s
	int						nFrameSt;								// �G���W������̃t���[����Ԑ��l (NCL_FRAME_STATUS)
	int						nJpegCmp;								// JPEG���k���[�h (-1:JPEG���k���� 0:�r���� 1�`:���k�����T�C�Y�B=Img�ȍ~�̃I�t�Z�b�g�l���Q�Ƃ���ꍇ�́A���̒l�����Z���邱��)

	DWORD					camset;									// �J�����Z�b�g (1�I���W��)
	long					offset;									// ���L�������̃I�t�Z�b�g�l (�����ł͎g�p���Ȃ�) (����ł̂ݎg�p)
	long					size;									// �f�[�^���̃T�C�Y

	long					frame_pos;								// ���Z�J�E���^�l (=front_pos)
	float					edge_pos[2];							// ���̔��肪���o�����G�b�W�ʒu�i�J�������W�n)mm�P��  ������͍��E�G�b�W������͂�
	float					edge_pos_max[2];						// ���̔��肪���o�����ő�G�b�W�ʒu�i�J�������W�n)mm�P��  ������͍��E�G�b�W������͂�
	float					edge_pos_min[2];						// ���̔��肪���o�����ŏ��G�b�W�ʒu�i�J�������W�n)mm�P��  ������͍��E�G�b�W������͂�
	BYTE					br[3];									// RGB

	LARGE_INTEGER			ht_freq[2];								// ����PC�̃p�t�H�[�}���X�J�E���^�̎��g��
	LARGE_INTEGER			ht_timestamp_start[2];					// ����Ǘ� �摜�����G���W������t���[����M���̃^�C���X�^���v
	LARGE_INTEGER			ht_timestamp_end[2];					// ����Ǘ� HT��TO���M���̃^�C���X�^���v
};
struct HT_TO_HEAD : public HT_TO_HEAD_BASE {
	BYTE	reserve[SIZE_SOCK_HEAD - sizeof(HT_TO_HEAD_BASE)];		// �\��
};

// Pipe�f�[�^��
struct HT_TO_DATA {


	// ����ȍ~�́A�t���[���X�L�b�v���ɂ́A���Z�b�g----->>> 
	long	defect_num;		// ���̃t���[�����r���i�����ʔz��Ɠ����ʔz��̐��j
	long	image_num;		// ���̃t���[���̒��̉摜�̐��i���摜�z��̐��j


	// ���̓`��������̃I�t�Z�b�g��
	//  ���G���W�� �w�b�_�[�̃I�t�Z�b�g�ʂ͎g������_��
	//
	//	���f�[�^��������
	//	�E�t���[���w�b�_	NCL_SMEM_FRAME_HEADER
	//	�E���� NCL_DEFECT_RESULT * defect_num
	//	�E������ double[768] * defect_num					RGB,�F����4CH�����̈�͎�邪�AR�ɂ͒l������Ȃ��B
	//	�E�؂�o���摜	DEFECT_HEAD_IMAGE + �؂�o���摜 * image_num
	//  �EJpeg���k�����t���[���摜�B DEFECT_HEAD_IMAGE + jpeg�t���[���摜 * image_num
	DWORD		header_offset;
	DWORD		result_offset;
	DWORD		attr_offset;
	DWORD		image_offset;

	DWORD		frame_jpeg_offset;
};
#pragma pack ()

//***************************************************************
// ���[���X���b�g�\����
//***************************************************************
struct EVENT_QUE {
	//long event;						// �C�x���g�R�[�h
	long nEventNo;											// �C�x���g�R�[�h
	long nLineNo;											// ���C��No (�ʏ�́ALINE_ID�Œ�B1PC�ŕ����̃V�X�e�����������Ă���ꍇ�� 0��1RCL 1��2RCL�p�Ƃ��Ɏg�p����B)

	union 
	{

/***
		// �R�C�����ʒm (TO_PROCOM��TO_DEFECT) 
		struct COILINF {
			COIL_INFO_DATA		data;
			char yobi[SIZE_MSL-sizeof(long) - sizeof(COIL_INFO_DATA)];
		} ev_coilinfo;


		struct TEST{
			long data[SIZE_MSL / sizeof(long) - 1];
		} test;
***/
	};
};
