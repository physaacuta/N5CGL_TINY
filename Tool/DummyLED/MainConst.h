// *********************************************************************************
//	���^�X�N�̊�{�萔
//	[Ver]
//		Ver.01    2007/08/01  vs2005 �Ή�
//
//	[����]
//		
// *********************************************************************************

#pragma once

#include "..\..\Include\KizuCommon.h"									// �r�����ʃw�b�_�[

//=======================================
// �Ǘ��萔
//=======================================
#define MAX_THREAD			8										// �ő�X���b�h�Ǘ���
#define MAX_LED_BLOCK		256										// �Ɩ����u �ő�z���u���b�N��
#define MAX_LED_CH			4										// �Ɩ����u �ő�`�����l����
#define MAX_TEMP_UNIT		9										// �Ɩ����u ���x���j�b�g�ő吔

#define DEF_ALL_INDEX		0										// �SIndex�w��
#define DEF_INDEX_END		4										// Index�I�[

//=======================================
// ���b�Z�[�W�ʒm
//=======================================
//// �S��MainInctance�Ŏ󂯂�
//       �ꍇ�ɂ���ẮAMainInctance���e���[�J�[�X���b�h�ɂ��ꗬ������
enum ENUM_MAININCTANCE {
	E_DEF_ML_PARAM = WM_USER + 100,									// �p�����[�^�ύX�ʒm


	E_DEF_END														// �I��
};

#define KT_LEDMGR "KT_LEDMGR"

#define DummyLED "DummyLED"
