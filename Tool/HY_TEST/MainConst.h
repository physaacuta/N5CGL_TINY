// *********************************************************************************
//	���^�X�N�̊�{�萔
//	[Ver]
//		Ver.01    2007/08/01  vs2005 �Ή�
//
//	[����]
//		
// *********************************************************************************

#pragma once

#include "..\..\Include\LineCommon.h"									// �r�����ʃw�b�_�[

//=======================================
// �Œ�T�C�Y
//=======================================
#define QSIZE_TO_HY				140										// ����->�\�� ��M�L���[�o�b�t�@
#define	QSIZE_IMG_SAVE			300										// �r�摜�ۑ��L���[�o�b�t�@ (��\�r)
#define	QSIZE_IMG_SAVE_LOW		200										// �r�摜�ۑ��L���[�o�b�t�@ (���X�g���ŉB����r)
#define	QUE_SKIP_COUNT			120										// �X�L�b�v�J�n�L���[�� (��ʂ��r�����������ꍇ�̑΍�)
#define THREAD_IMG_SAVE			2										// �摜�ۑ��X���b�h��

#define SIZE_FFNAME_TYPE		2										// FF����	�r��T�C�Y
#define SIZE_FFNAME_GRADE		2		

//=======================================
// �e�X�g�p�f�[�^�T�C�Y�Ȃ�
//=======================================
#define MAX_COIL_NUM			10										// �e�X�g�p�ɕێ����Ă���R�C�����

//=======================================
// SPM�����L��
//=======================================
enum EM_DIV_SPM
{
	DIV_SPM_OFF = 0,										// [SPM�����L��] ����
	DIV_SPM_ON,												// [SPM�����L��] �L��
	DIV_SPM_END												// [SPM�����L��] �I�[�Ŏg�p�����ߒ���
};

//=======================================
// ���b�Z�[�W�ʒm
//=======================================
//// �S��MainInctance�Ŏ󂯂�
//       �ꍇ�ɂ���ẮAMainInctance���e���[�J�[�X���b�h�ɂ��ꗬ������
enum ENUM_MAININCTANCE {
	E_DEF_ML_PARAM = WM_USER + 100,									// �p�����[�^�ύX�ʒm


	E_DEF_END														// �I��
};
