// *********************************************************************************
//	���^�X�N�̊�{�萔
//	[Ver]
//		Ver.01    2007/08/03  vs2005 �Ή�
//
//	[����]
//		
// *********************************************************************************

#pragma once

//#define tDEBUG																	// �f�o�b�N�� �R�����g�A�E�g
// �o�C�g���]�L��i1�A5CGL�A1CAPL�j
// �o�C�g���]�����i2CGL�A5CGL�ɔ����j
//#define BYTE_HANTEN					// �o�C�g���]�L���i�R�����g������Ă��鎞�́A�o�C�g���]�����j


//=======================================
// ���N���X�̒萔
//=======================================
#define	MYID_KIKI				"PROCON_ID"										// �v���R���ʐM�p�̋@��ID


//=======================================
// ���M�˗��L���[
//=======================================
//// ���M�˗�
enum QUE_STAT {
//		QUE_HEALTHY_SNED = 0,		// �w���V�[�`��
//		QUE_TEST_SNED,				// �e�X�g�`��
	QUE_ZISEKI_SEND = 0,			// �r���ѓ`��
	QUE_DEBUG_SEND					// [DEBUG] �r���ѓ`��
};
//// �󂯓n���\����
typedef struct{
	QUE_STAT				eStat;						// �v�����[�h
	BYTE*					pData;						// ���M����o�C�i���f�[�^

	char					cKey[SIZE_SERIAL_FF_NO];	// ���M����L�[
	int						nSize;						// ���M�T�C�Y
	int						nTc;						// �g�����U�N�V�����R�[�h
} Delivery;


//=======================================
// ���b�Z�[�W�ʒm
//=======================================
//// �S��MainInctance�Ŏ󂯂�
//       �ꍇ�ɂ���ẮAMainInctance���e���[�J�[�X���b�h�ɂ��ꗬ������
enum ENUM_MAININCTANCE {
	E_DEF_ML_PARAM = WM_USER + 100,						// �p�����[�^�ύX�ʒm
	E_DEF_DEBUG_RECV,									// �f�o�b�N�p�`����M�ʒm

	E_DEF_END											// �I��
};

