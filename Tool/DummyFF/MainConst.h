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
#include "..\..\Include\LineCommon.h"									// �r�����ʃw�b�_�[

enum QUE_SEND_STAT {
	QUE_SEND_ZISEKI_T = WM_USER + 100,					// �r���щ����v�� �\
	QUE_SEND_ZISEKI_B,									// �r���щ����v�� ��
	QUE_SEND_STAT_T,									// ������ԉ����v�� �\
	QUE_SEND_STAT_B,									// ������ԉ����v�� ��
	QUE_SEND_MAE_T,										// �O�H�����ё��M�v�� �\
	QUE_SEND_MAE_B,										// �O�H�����ё��M�v�� ��
	QUE_SEND_HELCY										// �w���V�[�`�����M�v��
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

//// �󂯓n���\����
struct Delivery
{
	QUE_SEND_STAT			eStat;							// �v�����[�h
	PRO_TYPE_KEY 			keyinf;							// �R�C�����肷�邽�߂̃L�[���
	char					cLine_Code[SIZE_KOUTEI_CODE];	// �H���R�[�h
	char					cMeiNo[4];			// ����No
	char					cCoilNo[10];			// �R�C��No
	char					cFileName[32];					// �f�o�b�N�� �O�H�����`���t�@�C������
};


//// ���M�`���󂯓n���\����
struct ExecItem
{
	QUE_SEND_STAT			emStat;						// �`�����
	//char					cKeyId[SIZE_SERIAL_FF_NO];	// DB��ID (�������NULL)
  	char					cMeiNo[SIZE_MEI_NO+1];		// ����No
	char					cCoilNo[SIZE_COIL_NO+1];	// �R�C��No
	//PRO_TYPE_KEY			keyinf;						// �R�C�����肷�邽�߂̃L�[���

	//int						nSendNowNo;					// ���Ԗڂ̓`�� (0�I���W��)
	//int						nRecvNowNo;


	//int						nSendMaxNo;					// ���Z�b�g���Ă��邩
	int						nSendSize;					// �`���̃T�C�Y
	BYTE					cSendBuf[65000];			// ���M�f�[�^

	//int						nRecvMaxNo;					// ����M����ׂ���
	//int						nRecvSize[4];				// �`���̃T�C�Y
	//BYTE					cRecvBuf[4][65000];			// ��M�f�[�^
};

#define DummyFF "DummyFF"
#define LINE_CODE "54"
#define FILE_FORMAT "FF\\ff_%.4s_%d.bin"