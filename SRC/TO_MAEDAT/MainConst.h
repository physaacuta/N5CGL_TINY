// *********************************************************************************
//	���^�X�N�̊�{�萔
//	[Ver]
//		Ver.01    2012/09/06
//
//	[����]
//		
// *********************************************************************************

#ifndef	_MAINCONST_H
#define _MAINCONST_H

// �r�� �C���N���[�h
#include "..\..\include\LineCommon.h"									// �r�����ʃw�b�_�[

//=======================================
// ���ʒ萔
//=======================================
static const int PARAM_SYSLOG_BASENO = 370;						// �V�X���O�̊���No
																// 0 : FF�T�[�o�[�ɊY������No�R�C������
																// 1 : �O�H���r���A�r�摜�o�^���s
																// 2 : �O�H���~��DB�ɐڑ�����
																// 3 : �O�H���~��DB Ping�����ُ�
																// 4 : �O�H���~��DB ODBC�ڑ��ُ�
																// 5 : �O�H���~��DB �R�C�����тȂ�


// 2TCM DB
#define DB_2TCM_DEFECT_INF_IN		"DEFECT_INF_IN"			// [DB] �����r���e�[�u��
#define DB_2TCM_DEFECT_IMG_IN		"DEFECT_IMG_IN"			// [DB] �����r�摜�e�[�u��

// 3TCM DB
#define DB_3TCM_DEFECT_INF_IN		"DEFECT_INF_IN"			// [DB] �����r���e�[�u��
#define DB_3TCM_DEFECT_IMG_IN		"DEFECT_IMG_IN"			// [DB] �����r�摜�e�[�u��

// 20200830 �≄�@�������u�����@�@�n�[�h�E�\�t�g�i2TCM�����X�V�j �a�� ----->>>
// N2TCM_ANA DB
#define DB_N2TCM_ANA_DEFECT_INF		"T_DEFECT_INF"			// [DB] �r���e�[�u��
#define DB_N2TCM_ANA_DEFECT_IMG		"T_DEFECT_IMG"			// [DB] �r�摜�e�[�u��
// 20200830 �≄�@�������u�����@�@�n�[�h�E�\�t�g�i2TCM�����X�V�j �a�� <<<-----

// 20221021 2TCM���@���ɔ������H������ ���� --->>>
// N2TCM_ANA DB
#define DB_N2TCM_KIZU_DEFECT_INF		"T_DEFECT_INF_T"			// [DB] �r���e�[�u��
#define DB_N2TCM_KIZU_DEFECT_IMG		"T_DEFECT_IMG_T"			// [DB] �r�摜�e�[�u��
// 20221021 2TCM���@���ɔ������H������ ���� ---<<<


enum ENUM_MAEDATOPERATION {
	// ���[���X���b�g��M��
	E_PD_ML_COIL = WM_USER + 100,		// �R�C������M�ʒm
	E_PD_ML_FF,							// FF�O�H���r����M�ʒm
	E_PD_END							// �I��
};

// FF�O�H�����я��\����
struct FF_MAE_INF
{
	struct TORB{
		FF_MAE_JISSEKI	typFFData;						
	} torb[NUM_MEN];						// �\ ��				
};

// �O�H���r���\����
struct MAE_DEFECT_IMAGE_INF
{
	char cKizuKenNo[SIZE_KIZUKEN_NO];		// �Ǘ�No
	char cPreKizuKenNo[SIZE_KIZUKEN_NO];	// �O�H���Ǘ�No
	int  nKouteiSyubetu;					// �H����ʁu�r���o�H�����v�����̉��Ԗڂ�
	int  nKizuNo;							// �rNo
	int  nMaeKizuNo;						// �O�H���rNo
	int  nOuDiv;							// �\���敪
	int  nCnt;								// �摜�f�[�^��
	DETECT_BASE_DATA typDefectData;			// �r���
	int  nGazouNo;							// �摜No
	DWORD nSize[MAX_MAE_IMAGE];				// �摜�T�C�Y
	BYTE* pImg[MAX_MAE_IMAGE];				// �摜�f�[�^
};

#endif
