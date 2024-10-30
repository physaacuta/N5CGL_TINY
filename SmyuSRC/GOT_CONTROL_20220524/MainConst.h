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


// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ���ۗp

//=======================================
// �C�x���g���
//=======================================
static const char	STR_KIND_PLG[]	= "PLG";
static const char	STR_KIND_PI[]	= "PI";
static const char	STR_KIND_PO[]	= "PO";
static const char	STR_KIND_MEM[]	= "MEM";
static const char	STR_KIND_MAIL[] = "MAIL";
static const char	STR_KIND_TIME[] = "TIME";
static const char	STR_KIND_ZZ[]	= "ZZ";

//=======================================
// �񋓑�
//=======================================
//// ���
enum EM_DIV_KIND {
	DIV_KIND_NON	= 0,
	DIV_KIND_PLG	= 1,
	DIV_KIND_PI,
	DIV_KIND_PO,
	DIV_KIND_MEM,
	DIV_KIND_MAIL,
	DIV_KIND_TIME,

	DIV_KIND_ZZ
};
//// ���[���X���b�g�f�[�^���\����
enum EM_DIV_MAIL {	
	DIV_MAIL_FL = 0,											// fl
	DIV_MAIL_FC,												// fc
	DIV_MAIL_MIX												// mix
};

//=======================================
// �\����
//=======================================
//// �V�i���I�f�[�^
struct TYP_SINA_INF {
	EM_DIV_KIND		emRaise;									// ���������� ���
	EM_DIV_KIND		emEvent;									// �C�x���g�� ���
	int				nEventSub;									// �C�x���g�� �T�u���

	BYTE*			pRaise;										// ���������� �\���̃|�C���^
	BYTE*			pEvent;										// �C�x���g�� �\���̃|�C���^

public:
	// �f�t�H���g�R���X�g���N�^
	TYP_SINA_INF() {	
		emRaise = DIV_KIND_NON;
		emEvent	= DIV_KIND_NON;
		nEventSub= 0;
		pRaise	= NULL;
		pEvent	= NULL;
	}
};

//// ��ʓ���
static EM_DIV_KIND GetTextToKind(CString str)
{
	//// �g����
	// �X�y�[�X, tab
	str.Trim("	");	

	//// ������r
	if(		0 == strcmp(str, "PLG") )	{return DIV_KIND_PLG;}
	else if(0 == strcmp(str, "PI") )	{return DIV_KIND_PI;}
	else if(0 == strcmp(str, "PO") )	{return DIV_KIND_PO;}
	else if(0 == strcmp(str, "MEM") )	{return DIV_KIND_MEM;}
	else if(0 == strcmp(str, "MAIL") )	{return DIV_KIND_MAIL;}
	else if(0 == strcmp(str, "TIME") )	{return DIV_KIND_TIME;}
	else if(0 == strcmp(str, "ZZ") )	{return DIV_KIND_ZZ;}
	else								{return DIV_KIND_NON;}
};

//=======================================
// �V�i���I�i�����������j�\����
//=======================================
//// �����쓮
struct TYP_RAISE_PLG_POS {
	int		nCh;												// �����Q�ƃ`�����l�� (0�I���W��) 0�`7
	int		nLen;												// ���۔������� [mm]
};
//// �C�x���g�쓮
// ���[��
struct TYP_RAISE_EVT_MAIL {
	char	cTaskName[SIZE_NAME_TASK];							// �^�X�N����
	int		nEventNo;											// �ΏۃC�x���gNo
};
// PO
struct TYP_RAISE_EVT_PO {
	int		nBood;												// �{�[�hNo (0�`7)
	int		nPort;												// �|�[�gNo (0�`7)
	int		nBit;												// �r�b�gNo (0�`7)
	int		nMode;												// �F���^�C�~���O (true:�����オ��Afalse:����������)
};
//// ���ԋ쓮
struct TYP_RAISE_TIM_CYCLE {
	int		nID;												// �z��ʒu (0�I���W��)
	int		nFirst;												// 1��ڊJ�n���� [ms]
	int		nCycle;												// 2��ڈȍ~�̎��� [ms] (0�Ȃ�P��̂�)
};

//=======================================
// �V�i���I�i�C�x���g���j�\����
//=======================================
//// ���x�ύX
struct TYP_EVENT_PLG_SPEED {
	int		nCh;												// �����Q�ƃ`�����l�� (0�I���W��) 0�`7:���C�����x  -1:�t���[������
	int		nMpm;												// �ύX���C�����x [mpm]
};
//// �t���[���o�͎����ύX
struct TYP_EVENT_PLG_FRAMECYCLE {
	int		nCycle;												// �ύX���� [ms]
};
//// BIT�w�菑�����݁i�X�e�[�^�X�o�́j
struct TYP_EVENT_PI_BIT {
	int		nBood;												// �{�[�hNo (0�`7)
	int		nPort;												// �|�[�gNo (0�`7)
	int		nBit;												// �r�b�gNo (0�`7)
	int		nMode;												// �Z�b�g (1:ON�A0:OFF)
};
//// �|�[�g�w�菑�����݁i�X�e�[�^�X�o�́j
struct TYP_EVENT_PI_PORT{
	int		nBood;												// �{�[�hNo (0�`7)
	int		nPort;												// �|�[�gNo (0�`7)
	BYTE	nBitSet;											// �Z�b�g����Bit�p�^�[��(1:ON�A0:OFF)
};
//// BIT�w�菑�����݁i�p���X�o�́j
struct TYP_EVENT_PI_BITPULS {
	int		nBood;												// �{�[�hNo (0�`7)
	int		nPort;												// �|�[�gNo (0�`7)
	int		nBit;												// �r�b�gNo (0�`7)
	int		nMode;												// �Z�b�g (1:ON�A0:OFF)
	int		nTime;												// ON���� [ms]
};
//// ���L��������������
struct TYP_EVENT_MEM_WRITE {
	char	cTblName[MAX_PATH];									// �������ދ��L����������
	int		nIndex;												// �������݊J�n�ʒu (0�I���W��)
	char	cWriteDataPath[MAX_PATH];							// �������ރo�C�i���f�[�^�̃p�X
};
//// ���[���X���b�g���M
struct TYP_EVENT_MAIL_SEND {
	char	cPcName[SIZE_NAME_PC];								// ���M�ΏۃR���s���[�^��
	char	cTaskName[SIZE_NAME_TASK];							// ���M�Ώۃ^�X�N��
	
	EM_DIV_MAIL	emMainKind;										// �\���̃^�C�v
	COMMON_QUE	que;											// ���M�\����
};
//// ���ԋ쓮
struct TYP_EVENT_TIM_MODE {
	int		nIndex;												// ���ԋ쓮�N���X�̔z��ʒu (0�I���W��)
	int		nID;												// �z��ʒu (0�I���W��)
	int		nMode;												// 0:��~�w��  1:�J�n�w��
};
//// �����ʔ������Z�b�g
struct TYP_EVENT_ZZ_RESET {
	int		nCh;												// �����Q�ƃ`�����l�� (0�I���W��) 0�`7
	int		nMode;												// �V�i���I�I���敪 (0:�������Ȃ�  1:�ŏ��ɖ߂�)
};

//=======================================
// ���b�Z�[�W�ʒm
//=======================================
//// �S��MainInctance�Ŏ󂯂�
//       �ꍇ�ɂ���ẮAMainInctance���e���[�J�[�X���b�h�ɂ��ꗬ������
enum ENUM_MAININCTANCE {
	E_DEF_ML_SMYU_START = WM_USER + 100,						// �V�~�����[�^�J�n�ʒm
	E_DEF_ML_SMYU_STOP,											// �V�~�����[�^��~�ʒm

	E_DEF_END													// �I��
};

