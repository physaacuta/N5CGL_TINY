// *********************************************************************************
//	���^�X�N�̊�{�萔
//	[Ver]
//		Ver.01    2007/08/01  vs2005 �Ή�
//
//	[����]
//		
// *********************************************************************************

#pragma once

#include "..\..\Include\KizuCommon.h"									// �������ʃw�b�_�[

#pragma once

// Mail�]�����邽�߂�UDP�̃p�P�b�g�\���B�Œ蒷�B
struct UdpMail {
	char		hostname[16];		// UDP����M�����l���A���[���X���b�g�𓊂�����PC
	char		mailname[16];		// ���[���X���b�g��
	int			datalen;			// �f�[�^�����B�i����data�̂����A�ǂꂾ���L�����j�B
	unsigned char	data[512];		// ���[���X���b�g�̃f�[�^�B(UDP�Ƃ��Ă͌Œ�j
};

// ���[���X���b�gUdpProxy�T�[�o�́AUDP�|�[�g�B�Œ�B
static const WORD KS_MAILTR_PORT = 10445;


//=======================================
// ���b�Z�[�W�ʒm
//=======================================
//// �S��MainInctance�Ŏ󂯂�
//       �ꍇ�ɂ���ẮAMainInctance���e���[�J�[�X���b�h�ɂ��ꗬ������
enum ENUM_MAININCTANCE {
	E_DEF_ML_TRANS = WM_USER + 100,									// ���[���X���b�g�]���v��

	E_DEF_END														// �I��
};



