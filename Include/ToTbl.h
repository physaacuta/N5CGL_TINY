// *********************************************************************************
//	�������L�������p�w�b�_�[
//	[Ver]
//		Ver.01    2015/06/15	���ō쐬
//
//	[����]
//		��{�I�ɁA����������
// *********************************************************************************

// �X�̃V�X�e���ŗL�̃C���N���[�h
#pragma once

// �r���C���N���[�h
#include "LineCommon.h"										// �r�����ʃw�b�_�[

//=======================================
// ���L����������
//=======================================
#define		TBL_TO_COIL_NAME		"TBL_TO_COIL"				// �R�C�����󂯓n���e�[�u��
#define		TBL_TO_SEQMEM_NAME		"TBL_TO_SEQMEM"				// �V�[�P���T���e�[�u���i�f�[�^�������j
//#define	TBL_TO_CAMERA_NAME		"TBL_TO_CAMERA"				// �J�����R���g���[���ݒ���e�[�u���i�f�[�^�������j

//=======================================
// �R�C�����e�[�u��
//=======================================
struct TO_COIL_TBL {
	struct TO_COIL_REC {
		COIL_SETTING		setting;							// ��{�ݒ�
		COIL_BASEDATA		basedata;							// ��{���
		BYTE				val[2048
								-sizeof(COIL_SETTING)
								-sizeof(COIL_BASEDATA)];		// �R�C�����󂯓n���o�b�t�@
	} rec[2];
};

//=======================================
// �V�[�P���T���e�[�u���i�f�[�^�������j
//=======================================
#define DM_DATA_MAX			256									// �V�[�P���T�[�f�[�^��
#define DM_AREA_MAX			2									// �V�[�P���T�[�f�[�^AREA��
#define DM_RECV_REC			0									// ��M�G���A (�p�́A�V�[�P���T������)
#define DM_SEND_REC			1									// ���M�G���A (�p�́A�V�[�P���T������)

struct TO_SEQMEM_TBL {
	// BYTE�f�[�^
	struct FB_DM {
			BYTE	Data[DM_DATA_MAX];							// �f�[�^	
	} fb_dm[DM_AREA_MAX];										// DMAREA	( 0:��M�G���A 1:���M�G���A )
};

#ifdef TBL_TO_CAMERA_NAME
//=======================================
// �J�����R���g���[���ݒ���e�[�u���i�f�[�^�������j
//=======================================
#define IS_TO_CAMERA_TBL										// ���L�������������A�R�����g��

struct TO_CAMERA_TBL {
	struct CAM_INF{
		int		nExpTime;										// ���ݘI������ [clk]
		int		nMode;											// ���ݏ�� (0:����͈�   -1:�������B  1:������B)
	} caminf[NUM_CAMERA*MAX_CAMANGLE];							// �J�����Z�b�g/�p�x ��
};
#endif