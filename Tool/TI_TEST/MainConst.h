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

#define FRAME_HT_SEND_PORT 2
#define FRAME_RAW_SEND

//=======================================
// �f�o�b�N

//#define SOT_TEST								// �P�̃e�X�g�� �R�����g�A�E�g
//#define SOT_IMG_OUTPUT						// �P�̃e�X�g�p���ԉ摜�o�͎� �R�����g�A�E�g 

// �e�X�g�p
#define		TI_TEST		"TI_TEST"


//=======================================
// �؂�ւ��X�C�b�`
//   ������͎g����
//=======================================

	
//=======================================
// �Œ�T�C�Y
//=======================================



//=======================================
// ���O�}�N��
#define LOGSOCK(t)  if(mcls_pLogSock!=NULL)       mcls_pLogSock->Write(KizuLib::LogFileManager::t		// �\�P�b�g�p���O�o��
#define LOGCULL(t)  if(mcls_pLogCull!=NULL)       mcls_pLogCull->Write(KizuLib::LogFileManager::t		// �S�������p���O�o��




//=======================================
// ���b�Z�[�W�ʒm
//=======================================
//// �S��MainInctance�Ŏ󂯂�
//       �ꍇ�ɂ���ẮAMainInctance���e���[�J�[�X���b�h�ɂ��ꗬ������
enum ENUM_MAININCTANCE {
	E_DEF_ML_PARAM = WM_USER + 100,				// �p�����[�^�ύX�ʒm

	E_DEF_END									// �I��
};


#define FACT_TI_TEST_01					9801					// �c�[���ԘA�� �N���v��
#define FACT_TI_TEST_02					9802					// �c�[���ԘA�� �N���v��
