// *********************************************************************************
//	���^�X�N�̊�{�萔
//	[Ver]
//		Ver.01    2012/08/30
//
//	[����]
//		
// *********************************************************************************

#pragma once

// ���ʃC���N���[�h
#include "..\..\Include\LineCommon.h"								// �r�����ʃw�b�_�[
//=======================================
// ���ʒ萔
//=======================================
static const int PARAM_SYSLOG_BASENO	= 380;						// �V�X���O�̊���No
						// 0 : ���ё��M
						// 1 : ���щ����`����M
						// 2 : ���щ����`���ُ�
						// 3 : �O�H���r���v���`�����M
						// 4 : �O�H���r���ѓ`����M
						// 5 : �O�H���r��񂪗v����������No�ƈقȂ�
						// 6 : �w���V�[�`����M
						// 7 : �����`���̕\���������ُ�
						// 8 : FF�T�[�o�[�ʐM����
						// 9 : FF�T�[�o�[�ʐM�ُ�
						//10 : FF�T�[�o�[�`�����M���s
						//11 : FF�T�[�o�[�����`���҂��^�C���A�E�g
						//12 : FF�T�[�o�[�`����M�ُ�
						//13 : ���M�V�[�P���X���ɉ����`���ȊO����M

//=======================================
// ���b�Z�[�W�ʒm
//=======================================
//// �S��MainInctance�Ŏ󂯂�
//       �ꍇ�ɂ���ẮAMainInctance���e���[�J�[�X���b�h�ɂ��ꗬ������
enum QUE_SEND_STAT {
	QUE_SEND_ZISEKI = WM_USER + 100,					// �r���ё��M�v��
	QUE_SEND_MAE,										// �O�H�����їv��
	QUE_SEND_HELCY										// �w���V�[�`�����M�v��
};

//// �󂯓n���\����
struct Delivery
{
	QUE_SEND_STAT			eStat;							// �v�����[�h
	PRO_TYPE_KEY 			keyinf;							// �R�C�����肷�邽�߂̃L�[���
	char					cLine_Code[SIZE_KOUTEI_CODE];	// �H���R�[�h
	char					cMeiNo[SIZE_MEI_NO];			// ����No
	char					cCoilNo[SIZE_COIL_NO];			// �R�C��No
	char					cOmtFileName[32];				// �f�o�b�N�� ���ѕ\�t�@�C������
	char					cUraFileName[32];				// �f�o�b�N�� ���ї��t�@�C������
	char					cStOmtFileName[32];				// �f�o�b�N�� ��ԕ\�t�@�C������
	char					cStUraFileName[32];				// �f�o�b�N�� ��ԗ��t�@�C������
};


//// ���M�`���󂯓n���\����
struct ExecItem
{
	QUE_SEND_STAT			emStat;						// �`�����
	char					cKeyId[SIZE_SERIAL_FF_NO];	// DB��ID (�������NULL)
  	char					cMeiNo[SIZE_MEI_NO+1];		// ����No
	char					cCoilNo[SIZE_COIL_NO+1];	// �R�C��No
	PRO_TYPE_KEY			keyinf;						// �R�C�����肷�邽�߂̃L�[���

	int						nSendNowNo;					// ���Ԗڂ̓`�� (0�I���W��)
	int						nRecvNowNo;


	int						nSendMaxNo;					// ���Z�b�g���Ă��邩
	int						nSendSize[4];				// �`���̃T�C�Y
	BYTE					cSendBuf[4][65000];			// ���M�f�[�^

	int						nRecvMaxNo;					// ����M����ׂ���
	int						nRecvSize[4];				// �`���̃T�C�Y
	BYTE					cRecvBuf[4][65000];			// ��M�f�[�^
};
