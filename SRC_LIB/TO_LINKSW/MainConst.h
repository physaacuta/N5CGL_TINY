// *********************************************************************************
//	���^�X�N�̊�{�萔
//	[Ver]
//		Ver.01    2007/08/01  vs2005 �Ή�
//
//	[����]
//		
// *********************************************************************************

#pragma once

#include "..\..\Include\LineCommon.h"								// �r�����ʃw�b�_�[

//=======================================
// ��
//=======================================

//#define	AOI_HEIGHT													// AOIheight�L�����A�A���R�����g

#define ENABLE_MAIL_FORCIBLY_MASTER									// �L�����A���[���X���b�g�ɂ��J�����I��ch�������I��Master�ɂ���

#define	COUNTER_MODE	0											// �x���J�E���g�M���I��
																		// 0 : LVAL�J�E���g
																		// 1 : FVAL�J�E���g
static const int LINKW_SERIAL_SPEED = 9600;						// �J���������N�X�C�b�`�E�o�b�Ԃ̑��x
//static const int LINKW_SERIAL_SPEED = 115200;						// �J���������N�X�C�b�`�E�o�b�Ԃ̑��x

//=======================================
// �Œ�
//=======================================

#define MAX_THREAD			8											// �ő�X���b�h�Ǘ���

// �\���p���ږ�
enum EM_DSP_COL_NAME {
	COL_ID = 0,											// ID
	COL_STATUS,											// �ð��
	COL_BOARD,											// �ް��
	COL_TEMP_SYU = COL_BOARD + MAX_CAMLINKSW_OUTPUT,	// ���͉��x
	COL_TEMP_DAI,										// �_�C���x
};

// �n�[�t�ŃJ���������N�X�C�b�`��CC-LINK�\�P�b�g���ݗL��
static const bool HARF_CC_SOCK_EXIST[MAX_CAMLINKSW_OUTPUT] = {
	 false		// Ch01	// Ch16
	,false		// Ch02	// Ch17
	,false		// Ch03	// Ch18
	,false		// Ch04	// Ch19
	,false		// Ch05	// Ch20
	,false		// Ch06	// Ch21
	,false		// Ch07	// Ch22
	,false		// Ch08	// Ch23
	,false		// Ch09	// Ch24
	,true		// Ch10	// Ch25
	,true		// Ch11	// Ch26
	,true		// Ch12	// Ch27
	,true		// Ch13	// Ch28
	,true		// Ch14	// Ch29
	,true		// Ch15	// Ch30
};