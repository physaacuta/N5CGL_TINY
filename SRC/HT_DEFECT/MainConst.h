// *********************************************************************************
//	���^�X�N�̊�{�萔
//	[Ver]
//		Ver.01    2007/08/01  vs2005 �Ή�
//
//	[����]
//		
// *********************************************************************************

#pragma once

#include <string.h>

#include "..\..\Include\LineCommon.h"									// �r�����ʃw�b�_�[

//#define FRAME_HT_SEND_PORT 2	// �b��
//#define FRAME_RAW_SEND	// �b��

//=======================================
// �f�o�b�N

//#define SOT_TEST								// �P�̃e�X�g�� �R�����g�A�E�g
//#define SOT_IMG_OUTPUT						// �P�̃e�X�g�p���ԉ摜�o�͎� �R�����g�A�E�g 
//#define DEBUG_LOG_HT_HEALTHY					// �I�����C���� �R�����g�A�E�g


//=======================================
// �؂�ւ��X�C�b�`
//   ������͎g����
//=======================================

	
//=======================================
// �Œ�T�C�Y
//=======================================
#define PROCESS_INIT_WAIT_TIME		8000		// �v���Z�X�J�n�҂����� (�摜�����G���W���̃G�b�W�����҂����Ԃ�10�b�̂���)
//#define PROCESS_INIT_WAIT_TIME		10000		// �v���Z�X�J�n�҂�����
#define PROCESS_EXIT_WAIT_TIME		5000		// �v���Z�X�I���҂�����

#define PROCESS_PIPE_WAIT_TIME		2000		// �v���Z�X�ԒʐM�m���҂�����
#define COMMAND_INIT_WAIT			90000		// 1���߂��|���邱�Ƃ�����//20000 //��	12000	// �������R�}���h���s�� �A���T�[�̑҂����� (�����ŃA���T�[�҂������Ă��鎞�Ԃ��Z�����鎖)
#define COMMAND_DISPOSE_WAIT		20000		// �����J���R�}���h���s�� �A���T�[�̑҂����� (�����ŃA���T�[�҂������Ă��鎞�Ԃ��Z�����鎖)
#define COMMAND_PARAM_WAIT			30000		// �p�����[�^�R�}���h���s�� �A���T�[�̑҂����� (SPM�̏����ŃR�C���؂�ւ����ɐU��Ԃ�\���������)


#define STAT_CHECK_INTERVAL			(3*60*1000)		// ������Ď�����
#define ST_SEND_INTERVAL			(1*60*1000)		// �����ւ̃w���V�[���M����


// ����
//#define HT_ONE_TOP_BOT							// ����@�\ �P��ŕ\�ʁA���ʂ̗��� �\�����R�����g�A�E�g  �� ���Ȃ�������̂ŁA���ʎg��Ȃ�
//#ifndef HT_ONE_TOP_BOT
//	#define HT_CONNECT_NUM			1			// 1PC �Жʓ���
//#else
//	#define HT_CONNECT_NUM			2			// 1PC ���ʓ���
//#endif
#define HT_CONNECT_NUM			1				// 1PC �Жʓ��� �̂ݍ�肱��



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

	E_DEF_ML_START,								// �����J�n�v��
	E_DEF_ML_STOP,								// ������~�v��
	E_DEF_ML_COILINF,							// ��ʐݒ���
	E_DEF_ML_CANOK,								// ���s�s�\����
	E_DEF_ML_STATUS,							// ��Ԗ₢���킹
	E_DEF_ML_CLEAR_QUEUE,						// �����s�ݒ�̃L�����Z��
	E_DEF_ML_SET_POSITION,						// ���������J�E���^�ݒ�
	E_DEF_ML_PRELOAD_PARAM,						// �p�����[�^���O�Ǎ�
	E_DEF_ML_RESUME_START,						// �r���J�n�v��
	E_DEF_ML_RESUME_STOP,						// �r����~�v��

	E_DEF_END									// �I��
};


//=======================================
// �X���b�h�Ԏ󂯓n���f�[�^�p �\����
//=======================================

//// �R�}���h�p�C�v�G���[
struct CPIPE_ERROR
{
	int								nErrCode;						// �G���[�R�[�h (���g��CommandSender::EM_PIPE_ERR)
	int								nDetailCode;					// �G���[�ڍ׃R�[�h
	BYTE*							pData;							// ��M�f�[�^���̂��́B���󂯓n�����̂������ꍇ��NULL�BNULL�Ŗ�����Ύ󂯎�����ق���delete�K�{
};