// *********************************************************************************
//	���^�X�N�̊�{�萔
//	[Ver]
//		Ver.01    2009/07/13  vs2005 �Ή�
//
//	[����]
//		
// *********************************************************************************

#pragma once

// �r�� �C���N���[�h
#include "..\..\Include\LineCommon.h"											// �r�����ʃw�b�_�[

//=======================================
// �������ѓ`��(�e�e�p)
//=======================================
#define Z_FF_RENBAN			"00"	// �A��
#define Z_FF_KMODE_NG		0		// �r���������[�h(�r����ғ�)
#define Z_FF_KMODE_OK		1		// �r���������[�h(�r���ғ�)
#define Z_FF_KMODE_OK_O		2		// �r���������[�h(�\�̂݉ғ�)
#define Z_FF_KMODE_OK_U		3		// �r���������[�h(���̂݉ғ�)
#define Z_FF_KMODE_STOP		4		// �r���������[�h(��~)
#define Z_FF_KMODE_EDGENG	5		// �r���������[�h(�G�b�W�����o��ԗL��)


#define Z_FF_DFCT_KENSA		"0"		// �r���o�敪(�r��)
#define Z_FF_DFCT_CUT		"2"		// �r���o�敪(�J�b�g)
#define Z_FF_DFCT_MAKI		"4"		// �r���o�敪(�n�ڊ������܂�)

#define Z_FF_KOVER_OK		1		// �r���I�[�o�[�X�e�[�^�X(1800���ȓ�)
#define Z_FF_KOVER_NG		2		// �r���I�[�o�[�X�e�[�^�X(1800���I�[�o�[)


// �_�~�[�r�Œ�l
#define Z_FF_KIZUNO_DUMMY	0		// �rNo

//=======================================
// �ؑփX�C�b�`
//=======================================
#define	PRO_SEND_ZI_HOSEI														// �v���R�����M���тɕ␳���K�p���A�A���R�����g

// �����������̑I���敪 (�� �ؑւ́A�e���[�J�[�X���b�h�̃R���X�g���N�^�Őݒ�)
enum ENUM_DIV_LEN_SPM
{
	DIV_LEN_SPM_OFF = 0,														// SPM���������̌�����������I��
	DIV_LEN_SPM_ON,																// SPM�����L��̌�����������I��
	DIV_LEN_SPM_RESULT															// �R�C�����т�SPM�����L���ɏ]���đI��
};

//=======================================
// �����M���т̋������M�p
//=======================================
#define UNSENT_MAX			1000	// �������M�Ώۃ��R�[�hMAX


//=======================================
// ���b�Z�[�W�ʒm
//=======================================
//// �S��MainInstance�Ŏ󂯂�
//       �ꍇ�ɂ���ẮAMainInstance���e���[�J�[�X���b�h�ɂ��ꗬ������
enum ENUM_MAININSTANCE
{
	E_DEF_ML_PRO_R_CUT = WM_USER + 100,											// �J�b�g���ю�M�ʒm
	E_DEF_ML_PRO_R_SAI,															// �̐����ю�M�ʒm
	E_DEF_ML_COIL_RESULT_ADD,													// ���я������݊����ʒm
	
	E_DEF_END																	// �I��
};
