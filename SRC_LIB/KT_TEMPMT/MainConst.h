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
#define MAX_TEMP_UNIT		16										// ���x���j�b�g�ő吔


//=======================================
// �ʐM���
//=======================================
// ���x�p�l�����[�^�ʐM���
enum EM_TEMP_MAINMODE
{
	MODE_NONE = 0,														// �������
	MODE_OPEN_CHECK,													// ���������m�F��
	MODE_WAIT,															// �ҋ@

	MODE_READ_STATUS,													// �R���g���[���X�e�[�^�X�Ǐo����
//	MODE_READ_MONITOR,													// ���j�^�l�Ǐo����

	MODE_RETRY															// �I�[�v�����g���C��
};

//=======================================
// �`��
//=======================================
//// �`���J�n�E�I�[����
#define	STX_CODE					0x02								// STX
#define	ETX_CODE					0x03								// ETX

//// ���X�|���X�t���[��
// �I���R�[�h
#define EXT_SIZE					2								// �I���R�[�h�̃T�C�Y
#define	EXT_OK						"00"							// ����I��
#define	EXT_ERR_FINS				"0F"							// FINS�R�}���h�G���[
#define EXT_ERR_PARITY				"10"							// �p���e�B�G���[
#define	EXT_ERR_FRAMING				"11"							// �t���[�~���O�G���[
#define	EXT_ERR_OVERRUN				"12"							// �I�[�o�[�����G���[
#define	EXT_ERR_BCC					"13"							// BCC�G���[
#define	EXT_ERR_FORMAT				"14"							// �t�H�[�}�b�g�G���[
#define	EXT_ERR_SUBADDRESS			"16"							// �T�u�A�h���X�G���[
#define	EXT_ERR_FRAMELEN			"18"							// �t���[�����G���[

//// ���X�|���X�R�[�h
#define	RES_SIZE					4								// ���X�|���X�R�[�h�̃T�C�Y
#define	RES_OK						"0000"							// ����I��
#define	RES_ERR_CMDLEN_OVER			"1001"							// �R�}���h���I�[�o�[
#define	RES_ERR_CMDLEN_SHORT		"1002"							// �R�}���h���s��
#define	RES_ERR_AREAKIND			"1101"							// �G���A��ʃG���[
#define	RES_ERR_RESLEN_OVER			"110B"							// ���X�|���X���I�[�o�[
#define	RES_ERR_PARAM				"1100"							// �p�����[�^�G���[
#define	RES_ERR_SYSTEM				"2203"							// ����G���[

//// FINS-mini�R�}���h�̃T�[�r�X�ꗗ
//// MCR + SCR
#define	FINS_READ_DATA				"0101"							// ���j�^�l/�ݒ�f�[�^�Ǐo��
#define	FINS_WRITE_DATA				"0102"							// �ݒ�f�[�^������
#define	FINS_READ_MULTI_DATA		"0104"							// ���j�^�l/�ݒ�f�[�^�����Ǐo��
#define	FINS_WRITE_MULTI_DATA		"0113"							// �ݒ�f�[�^����������
#define	FINS_ENTRY_READ_DATA		"0110"							// ���j�^�l/�ݒ�f�[�^�����o�^�Ǐo
#define	FINS_READ_ENTRY_WRITE_DATA	"0111"							// ���j�^�l/�ݒ�f�[�^�����Ǐo�o�^(����)
#define	FINS_READ_ENTRY_CHECK_DATA	"0112"							// ���j�^�l/�ݒ�f�[�^�����Ǐo�o�^�m�F(�Ǐo)
#define	FINS_READ_ATTRIBUTE			"0503"							// �{�̑����Ǐo��
#define	FINS_READ_STATUS			"0601"							// �R���g���[���X�e�[�^�X�Ǐo��
#define	FINS_ECHOBACK_TEST			"0801"							// �G�R�[�o�b�N�e�X�g
#define	FINS_COMMAND				"3005"							// ����w��

//// �X�e�[�^�X
#define	MT_STATUS_OK				0x00							// ����
#define	MT_STATUS_ERR_NO_MEASURE	0x01							// ���v�����
#define	MT_STATUS_ERR_DISPLAY_OVER	0x02							// �\���͈̓I�[�o�[
#define	MT_STATUS_ERR_INPUT_A		0x04							// ���ُ͈�A
#define	MT_STATUS_ERR_INPUT_B		0x08							// ���ُ͈�B

//////////////////////////////
//// �R�}���h�`��
// �G�R�[�o�b�N�e�X�g
struct CMD_TYPE_ECHO_BACK
{
	char			cSTX[1];										// STX
	char			cNodeNo[2];										// �m�[�hNo (���j�b�gNo)
	char			cSubAddr[2];									// �T�u�A�h���X ("00"�Œ�)
	char			cServiceID[1];									// �T�[�r�XID ("0"�Œ�)

	// FINS-mini
	char			cMRC[2];										// MRC ("08")
	char			cSRC[2];										// SRC ("01")

	char			cETX[1];										// ETX
	char			cBCC[1];										// BCC (�m�[�hNo�`ETX��XOR)
};

// �R���g���[���X�e�[�^�X�Ǐo��
struct CMD_TYPE_CONTROLER_STATUS
{
	char			cSTX[1];										// STX
	char			cNodeNo[2];										// �m�[�hNo (���j�b�gNo)
	char			cSubAddr[2];									// �T�u�A�h���X ("00"�Œ�)
	char			cServiceID[1];									// �T�[�r�XID ("0"�Œ�)

	// FINS-mini
	char			cMRC[2];										// MRC ("06")
	char			cSRC[2];										// SRC ("01")

	char			cETX[1];										// ETX
	char			cBCC[1];										// BCC (�m�[�hNo�`ETX��XOR)
};

// ���j�^�[�l�Ǐo��
struct CMD_TYPE_MONITOR_READ
{
	char			cSTX[1];										// STX
	char			cNodeNo[2];										// �m�[�hNo (���j�b�gNo)
	char			cSubAddr[2];									// �T�u�A�h���X ("00"�Œ�)
	char			cServiceID[1];									// �T�[�r�XID ("0"�Œ�)

	// FINS-mini
	char			cMRC[2];										// MRC ("01")
	char			cSRC[2];										// SRC ("01")
	char			cKind[2];										// �ϐ����
	char			cAddr[4];										// �A�h���X
	char			cBitPos[2];										// �r�b�g�ʒu
	char			cCount[4];										// �v�f��

	char			cETX[1];										// ETX
	char			cBCC[1];										// BCC (�m�[�hNo�`ETX��XOR)
};


//////////////////////////////
//// ���X�|���X�`��
// �G�R�[�o�b�N�e�X�g
struct RES_TYPE_ECHO_BACK
{
	char			cSTX[1];										// STX
	char			cNodeNo[2];										// �m�[�hNo (���j�b�gNo)
	char			cSubAddr[2];									// �T�u�A�h���X ("00"�Œ�)
	char			cEXT[2];										// �I���R�[�h

	// FINS-mini
	char			cMRC[2];										// MRC ("08")
	char			cSRC[2];										// SRC ("01")
	char			cResCode[4];									// ���X�|���X�R�[�h

	char			cETX[1];										// ETX
	char			cBCC[1];										// BCC
};

// �R���g���[���X�e�[�^�X�Ǐo��
struct RES_TYPE_CONTROLER_STATUS
{
	char			cSTX[1];										// STX
	char			cNodeNo[2];										// �m�[�hNo (���j�b�gNo)
	char			cSubAddr[2];									// �T�u�A�h���X ("00"�Œ�)
	char			cEXT[2];										// �I���R�[�h

	// FINS-mini
	char			cMRC[2];										// MRC ("06")
	char			cSRC[2];										// SRC ("01")
	char			cResCode[4];									// ���X�|���X�R�[�h
	char			cData1[2];										// �^�]���
	char			cData2[2];										// �X�e�[�^�X

	char			cETX[1];										// ETX
	char			cBCC[1];										// BCC
};

// ���j�^�[�l�Ǐo��
struct RES_TYPE_MONITOR_READ
{
	char			cSTX[1];										// STX
	char			cNodeNo[2];										// �m�[�hNo (���j�b�gNo)
	char			cSubAddr[2];									// �T�u�A�h���X ("00"�Œ�)
	char			cEXT[2];										// �I���R�[�h

	// FINS-mini
	char			cMRC[2];										// MRC ("01")
	char			cSRC[2];										// SRC ("01")
	char			cResCode[4];									// ���X�|���X�R�[�h
	char			cData[8];										// 

	char			cETX[1];										// ETX
	char			cBCC[1];										// BCC
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

