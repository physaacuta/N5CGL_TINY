// *********************************************************************************
//	�S�r���ŋ��ʂ̃C���N���[�h
//	[Ver]
//		Ver.01    2007/01/30  vs2005 �Ή�
//
//	[����]
//		��{�I�ɁA���������s��
// *********************************************************************************

#ifndef	_KIZUCOMMON_H
#define	_KIZUCOMMON_H

#include <windef.h>

// �r���C���N���[�h
#include "KizuListID.h"										// �r���H���R�[�h��`�w�b�_�[
#include "KizuCommonSelect.h"								// �S�r���ł̐ؑ֒萔�̃C���N���[�h



//***************************************************************
// �ݒ�萔 ��`
//***************************************************************

//=======================================
// �Œ蕶��
//=======================================

// �r���ݒ�t�@�C�� (�@�\�P�ʂ̐ݒ�t�@�C��)
#ifndef LOCAL	// �{�ԗp
	#define		TASKINI_NAME		".\\KizuTask.ini"		// �r���ݒ�t�@�C��
#else			// �J���p
	#define		TASKINI_NAME		".\\KizuTask.ini"		// �r���ݒ�t�@�C��
#endif

// �r���ݒ�t�@�C�� (PC�P�ʂ̐ݒ�t�@�C��)
#ifndef LOCAL	// �{�ԗp
	#define		PCINI_NAME			".\\KizuPc.ini"			// PC�ݒ�t�@�C��
#else			// �J���p
	#define		PCINI_NAME			".\\KizuPc.ini"			// PC�ݒ�t�@�C��
#endif


// �r���ݒ�t�@�C�� (PC�̃��[�J���P�ʂ̐ݒ�t�@�C��)
	// �����ini�t�@�C���Ȃ̂ŁA�ȉ��̎��ɒ���
		// �E�X�̃p�\�R���̈ꎞ�I�Ȑݒ����ۑ����Ă����t�@�C��
		// �E���O�I�t�����Ă��㏑������Ȃ�
		// �E��ini�t�@�C���������Ă��A�K���^�X�N���͓��삷��(�f�t�H���g�l�Ȃǂ�)�悤�ɁA����Ă�����
#ifndef LOCAL	// �{�ԗp
	#define		LOCALINI_NAME		".\\KizuLocal.ini"		// Local�ݒ�t�@�C��
#else			// �J���p
	#define		LOCALINI_NAME		".\\KizuLocal.ini"		// Local�ݒ�t�@�C��
#endif



// ini�t�@�C�� ���ʃZ�b�V���� (�F��ȃ^�X�N�Ŏg�p���鍀�ڂ̂�)
#define		INI_COMMON				"COMMON"				// ���ʐݒ� �p�Z�b�V����
#define		INI_DB					"DB"					// DB�ڑ� �p�Z�b�V���� (�I�����C��)
#define		INI_DB_A				"DB_A"					// DB�ڑ� �p�Z�b�V���� (A�n)
#define		INI_DB_B				"DB_B"					// DB�ڑ� �p�Z�b�V���� (B�n)
#define		INI_DB_PARA				"DB_1"					// DB�ڑ� �p�Z�b�V���� (���W�b�N�p�������p)
#define		INI_STATUS				"STATUS"				// �@��ID �p�Z�b�V����

//#define		INI_MEM					"MEM"					// ���L������ �p�Z�b�V����
//#define		INI_EXE					"EXECUTE"				// �v���Z�X �p�Z�b�V����


//=======================================
// �Œ�T�C�Y
//=======================================
#define SIZE_MSL					256//400				// send_mail,recv_mail�T�C�Y
#define SIZE_MSL_DATA				(SIZE_MSL-sizeof(long)*2)	// nEventNo�ȍ~�̃f�[�^��
#define SIZE_NAME_PC				16						// �p�\�R������
#define SIZE_NAME_TASK				16						// �^�X�N����
#define SIZE_IMAGE_PATH				32						// �摜�ۑ��p�̃p�X��
#define	SIZE_KIZUKEN_NO				20						// �r���Ǘ�No (=�Ǘ�No)  ([XXyyyymmdd hhnnss\0]�̌`��)
#define SIZE_SERIAL_NO				16						// ��ӂȊǗ�No ([yyyymmdd hhnnss\0]�̌`��)
#define SIZE_SERIAL_FF_NO			20						// ��ӂȊǗ�No ([yyyymmdd hhnnssfff\0]�̌`��)

// --->>> C++�̂�
#define	M_PI					3.1415926535897932384626	// ��
#define M_STEEL_RATE			7.87						// �S��d
// <<<------

// �������EPC�֘A�̃}�X�^�[
#define MAX_HT1_CAMSET				16						// ����PC1��̍ő�J�����Z�b�g (EPC��8�Z�b�g�܂ł����Ή����Ă��Ȃ�)
#define MAX_HT1_CAMANGLE			4						// ����PC1��̍ő�J�����p�x   (EPC��4�p�x�܂ł����Ή����Ă��Ȃ�)
#define MAX_ATTR_BASE				256						// �ő�����ʐ� (��{������)
//#define MAX_ATTR_ANGLE				128						// �ő�����ʐ� (�p�x�ʓ�����)		�� ����
#define MAX_ATTR_ANGLE				256						// �ő�����ʐ� (�p�x�ʓ�����)		�� �F��

#define MAX_TAJUUTHRESHOLD			30						// ���d臒l��� MAX
#define MAX_REDUCTION				30						// �k������� MAX
#define MAX_FILTER					30						// �t�B���^��� MAX
#define MAX_CROP_SHRINK             4                       // �Ԉ����� MAX

// DB�}�X�^�[����
#define MAX_STATUS					1024					// �ő� �@���ԃ}�X�^�[��
#define MAX_TYPE					256						// �r����MAX
// 2023.11.17 �ŏI�H���������u���@�\�����̂R -------------------->>>>>
//#define MAX_GRADE					8						// �O���[�h���MAX
#define MAX_GRADE					32						// �O���[�h���MAX
// 2023.11.17 �ŏI�H���������u���@�\�����̂R <<<<<--------------------
#define MAX_CAMSET					(NUM_CAM_POS*2)			// �J�����Z�b�g��MAX (����)
#define MAX_CAMANGLE				NUM_CAM_ANGLE			// �J�����p�x��MAX (�_���I�Ȋp�x)
#define MAX_PHYSICAL_CAMANGLE		1						// �J�����p�x��MAX (�����I�Ȋp�x)
#define MAX_SCODE					64						// �����\�ʏ��MAX (�f�t�H���g����1�܂�) (1�`64)
#define MAX_PCODE					64						// �����p�^�[��MAX (�f�t�H���g����1�܂�) (1�`64) ��(A�`Z)
#define MAX_CCODE					64						// �J�E���g�p�^�[��MAX (�f�t�H���g����1�܂�) (1�`64)
#define MAX_CODE_JUDGE				32						// �����\�ʏ�ԁE�����p�^�[���̏���MAX
#define MAX_IMAGE_CNT				4						// �摜���� (=�G���W������o�͂���錋�ʐ�) (�����܂ł�MAX_CAMANGLE��MAX_IMAGE_CNT�͓��`���������A�����ł́A�J�����P��ŕ����摜������΍�)

// �J���������N�X�C�b�`��`
#define	MAX_CAMLINKSW_DAISYCHAIN	8						// �f�C�W�[�`�F�[���ڑ��\�䐔
#define	MAX_CAMLINKSW_INPUT			4						// �X�C�b�`FPGA�{�[�h1��������̍ő���͐�
#define	MAX_CAMLINKSW_OUTPUT		15						// �X�C�b�`FPGA�{�[�h1��������̍ő�o�͐�
#define	MAX_CAMLINKSW_FPGA			2						// �J���������N�X�C�b�`1�䓖����̃X�C�b�`FPGA�{�[�h����
#define	MAX_CAMLINKSW_ALLINPUT		MAX_CAMLINKSW_INPUT*MAX_CAMLINKSW_FPGA	// �J���������N�X�C�b�`1�䓖����̍ő���͐�
#define	MAX_CAMLINKSW_ALLOUTPUT		MAX_CAMLINKSW_OUTPUT*MAX_CAMLINKSW_FPGA	// �J���������N�X�C�b�`1�䓖����̍ő�o�͐�


// NOA��`
#define NOA_ONLINE					0						// �I�����C��
#define NOA_OFFLINE					1						// �I�t���C��
#define NOA_DEBUG					9						// �f�o�b�O�p (�g���₷���p�ɑS������)
#define NOA_KAISEKI					-1						// ��͗p(�قڃf�o�b�O�Ɠ���)

//=======================================
// PC/�^�X�N�ݒ�
//=======================================
#ifndef LOCAL	// �{�ԗp
	#define MASTER_PC_NAME			TOKATU					// �r����ԊǗ������PC [KS_MASTER�̋N��PC���̂��w��]("."�w��̓_��)
	#define SYSLOG_PC_NAME			TOKATU					// syslog�����PC��
#else			// �J���p
	#define MASTER_PC_NAME			TOKATU					// �r����ԊǗ������PC [KS_MASTER�̋N��PC���̂��w��]("."�w��̓_��)
	#define SYSLOG_PC_NAME			"." //TOKATU					// syslog�����PC��
#endif


//=======================================
// �p�����[�^�ύX�敪 �i�r�����ʕ����j
//   �e���C���ŗL��ʂ̏ꍇ�́ALineCommon�ɋL�q���邱��
//=======================================
#define CH_SCODE_NON				0						// �����\�ʏ�ԑΏۊO
#define CH_SCODE_ALL				999						// �����\�ʏ�ԑS���ύX�敪

// ���삳�񃍃W�b�N���A�S�p�����[�^�ꊇ�Ǎ��݂ƂȂ����ׁA��ʒP�ʂł̒ʒm�ɕύX
#define CH_PARAM_ALL				0						// �S�e�[�u�� (�f�o�b�N�p)
#define CH_PARAM_TYPE_GRADE			1						// �r��EGr

#define CH_PARAM_ATTR_NAME			5						// �����ʖ���
#define CH_PARAM_PCINFODESC			6						// ��ʏ��

#define CH_PARAM_COMMON				10						// �Œ�p�����[�^
#define CH_PARAM_CAMERA				11						// �J�����ݒ�
#define CH_PARAM_EDGE				12						// �G�b�W���o
#define CH_PARAM_SHD				13						// �V�F�[�f�B���O�␳
#define CH_PARAM_THRESHOLD			14						// 臒l�ݒ�
#define CH_PARAM_DETECT				15						// �r���o
#define CH_PARAM_CYCLE				16						// �P���p�����r����
#define CH_PARAM_MERGE				17						// �J�������E����(�r��������)
#define CH_PARAM_LIGHT				18						// �Ɩ�
#define CH_PARAM_TINY				19						// ��������
#define CH_PARAM_SYNCADD			20						// �摜���Z (+���[��) ������PC�����郉�C���̂�

#define CH_PARAM_COLOR_DIV			21						// ���F����
//#define CH_PARAM_YOUSET				22						// �n�ړ_���o


#define CH_PARAM_SETTIG_SCODE		110						// �����\�ʏ�� �}�X�^�[�E�����ݒ�E�o�͐ݒ�
#define CH_PARAM_SETTIG_PCODE		111						// �����p�^�[�� �}�X�^�[�E�����ݒ�E�o�͐ݒ�
#define CH_PARAM_SETTIG_COMMON		112						// �����萔�ݒ�
#define CH_PARAM_SETTIG_PCOMMON		113						// �����p�^�[�� �ŗL �o�͐ݒ�
#define CH_PARAM_SETTIG_PALL		114						// �����p�^�[�� ���� �o�͐ݒ�
#define CH_PARAM_SETTIG_LOG			115						// �V�X���O�e�[�u��
#define CH_PARAM_SETTIG_STATUS		116						// �@���ԃe�[�u��




//***************************************************************
// DIV ��`
//   KizuLibMFC �� DivNameManager �Ŗ��̂��擾�\
//***************************************************************

//=======================================
// �g��BOOL
//=======================================
enum EM_DIV_BOOL {
	DIV_BOOL_TRUE = 1,										// ���� ��
	DIV_BOOL_FALSE = 0,										// �ُ� �~
	DIV_BOOL_NON = -1,										// ���� �|
	DIV_BOOL_NULL = -9										// �ΏۊO ��
};

//=======================================
// PLG�敪
//=======================================
enum EM_DIV_PLG {
	DIV_PLG_LINE	= 1,									// [PLG�敪] ���C��PLG
	DIV_PLG_TOP,											// [PLG�敪] �\�Z���p�p���X
	DIV_PLG_BOT,											// [PLG�敪] ���Z���p�p���X
	DIV_PLG_TEST											// [PLG�敪] �e�X�g�p���X
};

//=======================================
// �^�]���
//=======================================	
enum EM_DIV_UNTEN {
	DIV_UNTEN_STOP	= 0,									// [�^�]���] ��~���[�h �� ������ĕK�v�H
	DIV_UNTEN_NOMAL,										// [�^�]���] �ʏ�^�]���[�h
	DIV_UNTEN_SAMP,											// [�^�]���] �e�X�g�^�]���[�h (�T���v���B�e)
	DIV_UNTEN_CAMERA,										// [�^�]���] �Z�����[�h (�J��������)
	DIV_UNTEN_SMYU											// [�^�]���] �V�~�����[�V�������[�h
};

//=======================================
// ���Ə��
//=======================================	
enum EM_DIV_SOGYO {
	DIV_SOGYO_NON = -1,										// [���Ə��] �s��

	DIV_SOGYO_STOP = 0,										// [���Ə��] ���C���x�~��
	DIV_SOGYO_SLOW,											// [���Ə��] ���C���ꎞ��~or�ᑬ����
	DIV_SOGYO_RUN											// [���Ə��] ���C�����ƒ�
};

//=======================================
// �ғ����
//=======================================	
enum EM_DIV_KADOU {
	DIV_KADOU_INIT	= 1,									// [�ғ����] ��������
	DIV_KADOU_STOP,											// [�ғ����] ��~�� (��������)
	DIV_KADOU_START,										// [�ғ����] �ғ��� & �e�X�g��
	DIV_KADOU_RETRY											// [�ғ����] ���g���C��
};

//=======================================
// ������� (�l���傫�������A��苭���ُ�ƂȂ�悤�ɂ��鎖)
//=======================================	
enum EM_DIV_KENSA {
	DIV_KENSA_NON  = -9,									// [�������] �ΏۊO (��~��)

// �g����---->>
	DIV_KENSA_INIT = -1,									// [�������] ��~ (�\�����ѕʊǗ����ɂ́A����ŏ��������Ă����B9 _STOP�Ɠ�����ʕ\��)
// <----

	DIV_KENSA_OK   = 0,										// [�������] ����
	DIV_KENSA_NG   = 1,										// [�������] �k��
	DIV_KENSA_MENTE= 2,										// [�������] ��Ò�   ���v���r�W����

// �g����---->>
	DIV_KENSA_EDGENG= 4,									// [�������] ���ޕs�� (�G�b�W�����o�A�G�b�W�������ُ�)
	DIV_KENSA_DBNON	= 5,									// [�������] DB���і��ۑ�(�L�QGr���ђx��A�ۑ�����������B)
	DIV_KENSA_SKIP	= 6,									// [�������] ���������  (�摜�����G���W�����X�L�b�v)
// <----

	DIV_KENSA_STOP = 9										// [�������] ��~  (�Жʈُ�̏ꍇ�A������~��)
};

//=======================================
// �@���� (�l���傫�������A��苭���ُ�ƂȂ�悤�ɂ��鎖)
//=======================================
enum EM_DIV_KIKI {
	DIV_KIKI_OK  = 0,										// [�@����] ����
	DIV_KIKI_KEI = 1,										// [�@����] �y�̏�
	//DIV_KIKI_TYU = 2,										// [�@����] ���̏� (�v���r�W����)
	DIV_KIKI_JYU = 3										// [�@����] �d�̏�
};

//=======================================
// DS, WS ���̃|�W�V����
//=======================================
enum EM_DIV_POS {
#ifdef NUM_CAM_POS_1
	DIV_POS_C = 0											// C
#endif
#ifdef NUM_CAM_POS_2
	DIV_POS_DS = 0,											// DS
	DIV_POS_WS												// WS
#endif
#ifdef NUM_CAM_POS_3
	DIV_POS_DS = 0,											// DS
	DIV_POS_C,												// C
	DIV_POS_WS												// WS
#endif
#ifdef NUM_CAM_POS_4
	DIV_POS_DS = 0,											// DS
	DIV_POS_D1,												// D1
	DIV_POS_W1,												// W1
	DIV_POS_WS												// WS
#endif
#ifdef NUM_CAM_POS_5
	DIV_POS_DS = 0,											// DS
	DIV_POS_D1,												// D1
	DIV_POS_C,												// C
	DIV_POS_W1,												// W1
	DIV_POS_WS												// WS
#endif
#ifdef NUM_CAM_POS_6
	DIV_POS_DS = 0,											// DS
	DIV_POS_D1,												// D1
	DIV_POS_D2,												// D2
	DIV_POS_W2,												// W2
	DIV_POS_W1,												// W1
	DIV_POS_WS												// WS
#endif
#ifdef NUM_CAM_POS_7
	DIV_POS_DS = 0,											// DS
	DIV_POS_D1,												// D1
	DIV_POS_D2,												// D2
	DIV_POS_C,												// C
	DIV_POS_W2,												// W2
	DIV_POS_W1,												// W1
	DIV_POS_WS												// WS
#endif
#ifdef NUM_CAM_POS_8
	DIV_POS_DS = 0,											// DS
	DIV_POS_D1,												// D1
	DIV_POS_D2,												// D2
	DIV_POS_D3,												// D3
	DIV_POS_W3,												// W3
	DIV_POS_W2,												// W2
	DIV_POS_W1,												// W1
	DIV_POS_WS												// WS
#endif
#ifdef NUM_CAM_POS_9
	DIV_POS_DS = 0,											// DS
	DIV_POS_D1,												// D1
	DIV_POS_D2,												// D2
	DIV_POS_D3,												// D3
	DIV_POS_C,												// C
	DIV_POS_W3,												// W3
	DIV_POS_W2,												// W2
	DIV_POS_W1,												// W1
	DIV_POS_WS												// WS
#endif
#ifdef NUM_CAM_POS_10
	DIV_POS_DS = 0,											// DS
	DIV_POS_D1,												// D1
	DIV_POS_D2,												// D2
	DIV_POS_D3,												// D3
	DIV_POS_D4,												// D4
	DIV_POS_W4,												// W4
	DIV_POS_W3,												// W3
	DIV_POS_W2,												// W2
	DIV_POS_W1,												// W1
	DIV_POS_WS												// WS
#endif
#ifdef NUM_CAM_POS_11
	DIV_POS_DS = 0,											// DS
	DIV_POS_D1,												// D1
	DIV_POS_D2,												// D2
	DIV_POS_D3,												// D3
	DIV_POS_D4,												// D4
	DIV_POS_C,												// C
	DIV_POS_W4,												// W4
	DIV_POS_W3,												// W3
	DIV_POS_W2,												// W2
	DIV_POS_W1,												// W1
	DIV_POS_WS												// WS
#endif
#ifdef NUM_CAM_POS_12
	DIV_POS_DS = 0,											// DS
	DIV_POS_D1,												// D1
	DIV_POS_D2,												// D2
	DIV_POS_D3,												// D3
	DIV_POS_D4,												// D4
	DIV_POS_D5,												// D5
	DIV_POS_W5,												// W5
	DIV_POS_W4,												// W4
	DIV_POS_W3,												// W3
	DIV_POS_W2,												// W2
	DIV_POS_W1,												// W1
	DIV_POS_WS												// WS
#endif
#ifdef NUM_CAM_POS_13
	DIV_POS_DS = 0,											// DS
	DIV_POS_D1,												// D1
	DIV_POS_D2,												// D2
	DIV_POS_D3,												// D3
	DIV_POS_D4,												// D4
	DIV_POS_D5,												// D5
	DIV_POS_C,												// C
	DIV_POS_W5,												// W5
	DIV_POS_W4,												// W4
	DIV_POS_W3,												// W3
	DIV_POS_W2,												// W2
	DIV_POS_W1,												// W1
	DIV_POS_WS												// WS
#endif
#ifdef NUM_CAM_POS_14
	DIV_POS_DS = 0,											// DS
	DIV_POS_D1,												// D1
	DIV_POS_D2,												// D2
	DIV_POS_D3,												// D3
	DIV_POS_D4,												// D4
	DIV_POS_D5,												// D5
	DIV_POS_D6,												// D6
	DIV_POS_W6,												// W6
	DIV_POS_W5,												// W5
	DIV_POS_W4,												// W4
	DIV_POS_W3,												// W3
	DIV_POS_W2,												// W2
	DIV_POS_W1,												// W1
	DIV_POS_WS												// WS
#endif
#ifdef NUM_CAM_POS_15
	DIV_POS_DS = 0,											// DS
	DIV_POS_D1,												// D1
	DIV_POS_D2,												// D2
	DIV_POS_D3,												// D3
	DIV_POS_D4,												// D4
	DIV_POS_D5,												// D5
	DIV_POS_D6,												// D6
	DIV_POS_C,												// C
	DIV_POS_W6,												// W6
	DIV_POS_W5,												// W5
	DIV_POS_W4,												// W4
	DIV_POS_W3,												// W3
	DIV_POS_W2,												// W2
	DIV_POS_W1,												// W1
	DIV_POS_WS												// WS
#endif
#ifdef NUM_CAM_POS_16
	DIV_POS_DS = 0,											// DS
	DIV_POS_D1,												// D1
	DIV_POS_D2,												// D2
	DIV_POS_D3,												// D3
	DIV_POS_D4,												// D4
	DIV_POS_D5,												// D5
	DIV_POS_D6,												// D6
	DIV_POS_D7,												// D7
	DIV_POS_W7,												// W7
	DIV_POS_W6,												// W6
	DIV_POS_W5,												// W5
	DIV_POS_W4,												// W4
	DIV_POS_W3,												// W3
	DIV_POS_W2,												// W2
	DIV_POS_W1,												// W1
	DIV_POS_WS												// WS
#endif
};

//=======================================
// �J�����Z�b�gNo
//=======================================
enum EM_DIV_CAMSET {
	DIV_CAMSET_BACK = 0,									// [�J�����y�ANo] �o�b�N�A�b�v

#ifdef NUM_CAM_POS_1
	DIV_CAMSET_T_C,
	DIV_CAMSET_B_C
#endif
#ifdef NUM_CAM_POS_2
	DIV_CAMSET_T_DS,										// �\
	DIV_CAMSET_T_WS,
	DIV_CAMSET_B_DS,
	DIV_CAMSET_B_WS
#endif
#ifdef NUM_CAM_POS_3
	DIV_CAMSET_T_DS,										// �\
	DIV_CAMSET_T_C,
	DIV_CAMSET_T_WS,
	DIV_CAMSET_B_DS,
	DIV_CAMSET_B_C,
	DIV_CAMSET_B_WS
#endif
#ifdef NUM_CAM_POS_4
	DIV_CAMSET_T_DS,										// �\
	DIV_CAMSET_T_D1,
	DIV_CAMSET_T_W1,
	DIV_CAMSET_T_WS,

	DIV_CAMSET_B_DS,
	DIV_CAMSET_B_D1,
	DIV_CAMSET_B_W1,
	DIV_CAMSET_B_WS
#endif
#ifdef NUM_CAM_POS_5
	DIV_CAMSET_T_DS,										// �\
	DIV_CAMSET_T_D1,
	DIV_CAMSET_T_C,
	DIV_CAMSET_T_W1,
	DIV_CAMSET_T_WS,

	DIV_CAMSET_B_DS,
	DIV_CAMSET_B_D1,
	DIV_CAMSET_B_C,
	DIV_CAMSET_B_W1,
	DIV_CAMSET_B_WS
#endif
#ifdef NUM_CAM_POS_6
	DIV_CAMSET_T_DS,										// �\
	DIV_CAMSET_T_D1,
	DIV_CAMSET_T_D2,
	DIV_CAMSET_T_W2,
	DIV_CAMSET_T_W1,
	DIV_CAMSET_T_WS,

	DIV_CAMSET_B_DS,
	DIV_CAMSET_B_D1,
	DIV_CAMSET_B_D2,
	DIV_CAMSET_B_W2,
	DIV_CAMSET_B_W1,
	DIV_CAMSET_B_WS
#endif
#ifdef NUM_CAM_POS_7
	DIV_CAMSET_T_DS,										// �\
	DIV_CAMSET_T_D1,
	DIV_CAMSET_T_D2,
	DIV_CAMSET_T_C,
	DIV_CAMSET_T_W2,
	DIV_CAMSET_T_W1,
	DIV_CAMSET_T_WS,

	DIV_CAMSET_B_DS,
	DIV_CAMSET_B_D1,
	DIV_CAMSET_B_D2,
	DIV_CAMSET_B_C,
	DIV_CAMSET_B_W2,
	DIV_CAMSET_B_W1,
	DIV_CAMSET_B_WS
#endif
#ifdef NUM_CAM_POS_8
	DIV_CAMSET_T_DS,										// �\
	DIV_CAMSET_T_D1,
	DIV_CAMSET_T_D2,
	DIV_CAMSET_T_D3,
	DIV_CAMSET_T_W3,
	DIV_CAMSET_T_W2,
	DIV_CAMSET_T_W1,
	DIV_CAMSET_T_WS,

	DIV_CAMSET_B_DS,
	DIV_CAMSET_B_D1,
	DIV_CAMSET_B_D2,
	DIV_CAMSET_B_D3,
	DIV_CAMSET_B_W3,
	DIV_CAMSET_B_W2,
	DIV_CAMSET_B_W1,
	DIV_CAMSET_B_WS
#endif
#ifdef NUM_CAM_POS_9
	DIV_CAMSET_T_DS,										// �\
	DIV_CAMSET_T_D1,
	DIV_CAMSET_T_D2,
	DIV_CAMSET_T_D3,
	DIV_CAMSET_T_C,
	DIV_CAMSET_T_W3,
	DIV_CAMSET_T_W2,
	DIV_CAMSET_T_W1,
	DIV_CAMSET_T_WS,

	DIV_CAMSET_B_DS,
	DIV_CAMSET_B_D1,
	DIV_CAMSET_B_D2,
	DIV_CAMSET_B_D3,
	DIV_CAMSET_B_C,
	DIV_CAMSET_B_W3,
	DIV_CAMSET_B_W2,
	DIV_CAMSET_B_W1,
	DIV_CAMSET_B_WS
#endif
#ifdef NUM_CAM_POS_10
	DIV_CAMSET_T_DS,										// �\
	DIV_CAMSET_T_D1,
	DIV_CAMSET_T_D2,
	DIV_CAMSET_T_D3,
	DIV_CAMSET_T_D4,
	DIV_CAMSET_T_W4,
	DIV_CAMSET_T_W3,
	DIV_CAMSET_T_W2,
	DIV_CAMSET_T_W1,
	DIV_CAMSET_T_WS,

	DIV_CAMSET_B_DS,
	DIV_CAMSET_B_D1,
	DIV_CAMSET_B_D2,
	DIV_CAMSET_B_D3,
	DIV_CAMSET_B_D4,
	DIV_CAMSET_B_W4,
	DIV_CAMSET_B_W3,
	DIV_CAMSET_B_W2,
	DIV_CAMSET_B_W1,
	DIV_CAMSET_B_WS
#endif
#ifdef NUM_CAM_POS_11
	DIV_CAMSET_T_DS,										// �\
	DIV_CAMSET_T_D1,
	DIV_CAMSET_T_D2,
	DIV_CAMSET_T_D3,
	DIV_CAMSET_T_D4,
	DIV_CAMSET_T_C,
	DIV_CAMSET_T_W4,
	DIV_CAMSET_T_W3,
	DIV_CAMSET_T_W2,
	DIV_CAMSET_T_W1,
	DIV_CAMSET_T_WS,

	DIV_CAMSET_B_DS,
	DIV_CAMSET_B_D1,
	DIV_CAMSET_B_D2,
	DIV_CAMSET_B_D3,
	DIV_CAMSET_B_D4,
	DIV_CAMSET_B_C,
	DIV_CAMSET_B_W4,
	DIV_CAMSET_B_W3,
	DIV_CAMSET_B_W2,
	DIV_CAMSET_B_W1,
	DIV_CAMSET_B_WS
#endif
#ifdef NUM_CAM_POS_12
	DIV_CAMSET_T_DS,										// �\
	DIV_CAMSET_T_D1,
	DIV_CAMSET_T_D2,
	DIV_CAMSET_T_D3,
	DIV_CAMSET_T_D4,
	DIV_CAMSET_T_D5,
	DIV_CAMSET_T_W5,
	DIV_CAMSET_T_W4,
	DIV_CAMSET_T_W3,
	DIV_CAMSET_T_W2,
	DIV_CAMSET_T_W1,
	DIV_CAMSET_T_WS,

	DIV_CAMSET_B_DS,
	DIV_CAMSET_B_D1,
	DIV_CAMSET_B_D2,
	DIV_CAMSET_B_D3,
	DIV_CAMSET_B_D4,
	DIV_CAMSET_B_D5,
	DIV_CAMSET_B_W5,
	DIV_CAMSET_B_W4,
	DIV_CAMSET_B_W3,
	DIV_CAMSET_B_W2,
	DIV_CAMSET_B_W1,
	DIV_CAMSET_B_WS
#endif
#ifdef NUM_CAM_POS_13
	DIV_CAMSET_T_DS,										// �\
	DIV_CAMSET_T_D1,
	DIV_CAMSET_T_D2,
	DIV_CAMSET_T_D3,
	DIV_CAMSET_T_D4,
	DIV_CAMSET_T_D5,
	DIV_CAMSET_T_C,
	DIV_CAMSET_T_W5,
	DIV_CAMSET_T_W4,
	DIV_CAMSET_T_W3,
	DIV_CAMSET_T_W2,
	DIV_CAMSET_T_W1,
	DIV_CAMSET_T_WS,

	DIV_CAMSET_B_DS,
	DIV_CAMSET_B_D1,
	DIV_CAMSET_B_D2,
	DIV_CAMSET_B_D3,
	DIV_CAMSET_B_D4,
	DIV_CAMSET_B_D5,
	DIV_CAMSET_B_C,
	DIV_CAMSET_B_W5,
	DIV_CAMSET_B_W4,
	DIV_CAMSET_B_W3,
	DIV_CAMSET_B_W2,
	DIV_CAMSET_B_W1,
	DIV_CAMSET_B_WS
#endif
#ifdef NUM_CAM_POS_14
	DIV_CAMSET_T_DS,										// �\
	DIV_CAMSET_T_D1,
	DIV_CAMSET_T_D2,
	DIV_CAMSET_T_D3,
	DIV_CAMSET_T_D4,
	DIV_CAMSET_T_D5,
	DIV_CAMSET_T_D6,
	DIV_CAMSET_T_W6,
	DIV_CAMSET_T_W5,
	DIV_CAMSET_T_W4,
	DIV_CAMSET_T_W3,
	DIV_CAMSET_T_W2,
	DIV_CAMSET_T_W1,
	DIV_CAMSET_T_WS,

	DIV_CAMSET_B_DS,
	DIV_CAMSET_B_D1,
	DIV_CAMSET_B_D2,
	DIV_CAMSET_B_D3,
	DIV_CAMSET_B_D4,
	DIV_CAMSET_B_D5,
	DIV_CAMSET_B_D6,
	DIV_CAMSET_B_W6,
	DIV_CAMSET_B_W5,
	DIV_CAMSET_B_W4,
	DIV_CAMSET_B_W3,
	DIV_CAMSET_B_W2,
	DIV_CAMSET_B_W1,
	DIV_CAMSET_B_WS
#endif
#ifdef NUM_CAM_POS_15
	DIV_CAMSET_T_DS,										// �\
	DIV_CAMSET_T_D1,
	DIV_CAMSET_T_D2,
	DIV_CAMSET_T_D3,
	DIV_CAMSET_T_D4,
	DIV_CAMSET_T_D5,
	DIV_CAMSET_T_D6,
	DIV_CAMSET_T_C,
	DIV_CAMSET_T_W6,
	DIV_CAMSET_T_W5,
	DIV_CAMSET_T_W4,
	DIV_CAMSET_T_W3,
	DIV_CAMSET_T_W2,
	DIV_CAMSET_T_W1,
	DIV_CAMSET_T_WS,

	DIV_CAMSET_B_DS,
	DIV_CAMSET_B_D1,
	DIV_CAMSET_B_D2,
	DIV_CAMSET_B_D3,
	DIV_CAMSET_B_D4,
	DIV_CAMSET_B_D5,
	DIV_CAMSET_B_D6,
	DIV_CAMSET_B_C,
	DIV_CAMSET_B_W6,
	DIV_CAMSET_B_W5,
	DIV_CAMSET_B_W4,
	DIV_CAMSET_B_W3,
	DIV_CAMSET_B_W2,
	DIV_CAMSET_B_W1,
	DIV_CAMSET_B_WS
#endif
#ifdef NUM_CAM_POS_16
	DIV_CAMSET_T_DS,										// �\
	DIV_CAMSET_T_D1,
	DIV_CAMSET_T_D2,
	DIV_CAMSET_T_D3,
	DIV_CAMSET_T_D4,
	DIV_CAMSET_T_D5,
	DIV_CAMSET_T_D6,
	DIV_CAMSET_T_D7,
	DIV_CAMSET_T_W7,
	DIV_CAMSET_T_W6,
	DIV_CAMSET_T_W5,
	DIV_CAMSET_T_W4,
	DIV_CAMSET_T_W3,
	DIV_CAMSET_T_W2,
	DIV_CAMSET_T_W1,
	DIV_CAMSET_T_WS,

	DIV_CAMSET_B_DS,
	DIV_CAMSET_B_D1,
	DIV_CAMSET_B_D2,
	DIV_CAMSET_B_D3,
	DIV_CAMSET_B_D4,
	DIV_CAMSET_B_D5,
	DIV_CAMSET_B_D6,
	DIV_CAMSET_B_D7,
	DIV_CAMSET_B_W7,
	DIV_CAMSET_B_W6,
	DIV_CAMSET_B_W5,
	DIV_CAMSET_B_W4,
	DIV_CAMSET_B_W3,
	DIV_CAMSET_B_W2,
	DIV_CAMSET_B_W1,
	DIV_CAMSET_B_WS
#endif
};

//=======================================
// ���׃O���[�h
//=======================================
enum EM_DETECT_GR {
	DETECT_GR_NON = 0,										// ���Q
	DETECT_GR_KEI,											// �y����
	DETECT_GR_TYU,											// ������
	DETECT_GR_JU											// �d����
};
enum EM_DETECT_GR2 {
	DETECT_GR2_NON = 0,										// ���Q
	DETECT_GR2_KEI,											// �y����
	DETECT_GR2_JU											// �d����
};


//***************************************************************
// ��{�I�� ����ȍ~ �S�r�����ʎd�l
//	�� ����ȊO�̍��ڂɂ��ẮALineCommon�ɒ�`����
//***************************************************************
//=======================================
// �v���Z�X���̈ꗗ
//=======================================
// �x���@�\
#define KS_MASTER					"KS_MASTER"				// PC�}�X�^�[�Ǘ�
#define KS_SLAVE					"KS_SLAVE"				// PC�X���[�u�Ǘ�
#define KS_START					"KS_START"				// �X�^�[�g�A�b�v�Ǘ�
#define KS_WAKEUP					"KS_WAKEUP"				// �C�j�V�������ҋ@���
#define KS_EVTLOG					"KS_EVTLOG"				// �C�x���g�r���[�A�[���Ǘ�
#define KS_SYSLOG					"KS_SYSLOG"				// SYSLOG�o�^�^�X�N
#define KS_UPSMGR					"KS_UPSMGR"				// UPS�ُ�I���^�X�N
#define KS_BACKUP					"KS_BACKUP"				// �o�b�N�A�b�v�ؑ֎x���^�X�N
#define KS_EQALDB					"KS_EQALDB"				// DB�����Ǘ��^�X�N

#define KS_HDDCHK					"KS_HDDCHK"				// HDD��ԊĎ��^�X�N
#define KS_HISTRY					"KS_HISTRY"				// �@���ԗ����Ǘ��^�X�N
#define KS_APCHECK					"KS_APCHECK"			// AP��ԊĎ��^�X�N
#define KS_MAILTR					"KS_MAILTR"				// ���[���X���b�g�]���^�X�N

// ���ʃ^�X�N
	// �� �����ɖ���`��KT�^�X�N�́ALineCommon.h�ɋN���v������`���Ă���̂�����̂Œ���
#define KT_CHILER					"KT_CHILER"				// �΃`���[�ʐM
//#define KT_LEDMGR					"KT_LEDMGR"				// ��LED�Ɩ��ʐM
#define KT_TEMPMT					"KT_TEMPMT"				// �Ή��x�p�l�����[�^ (�v���r�W����)
#define KT_RPICHK					"KT_RPICHK"				// RPI�t�@�C���Ǘ�


// �~�ϋ@�\	
#define TI_DELETE					"TI_DELETE"				// DB�폜�Ǘ��^�X�N	


//=======================================
// �f�[�^�x�[�X�e�[�u������
//=======================================
#define DB_ADD_M											// M_,T_�t���̏ꍇ�t�^
// �����}�X�^�[�֘A
#define DB_STATUS_MASTER			"M_STATUS_MASTER"		// [DB] �@��}�X�^�[�e�[�u��
#define DB_STATUS_INF				"M_STATUS_INF"			// [DB] �@���ԃe�[�u��
#define DB_LOG_MASTER				"M_LOG_MASTER"			// [DB] ���O�}�X�^�[�e�[�u��
#define DB_LOG_INF					"T_LOG_INF"				// [DB] ���O���e�[�u��
#define DB_PC_MASTER				"M_PC_MASTER"			// [DB] PC�}�X�^�[
#define DB_PC_INF					"M_PC_INF"				// [DB] PC���
#define DB_ST_HISTORY_MASTER		"M_ST_HISTORY_MASTER"	// [DB] �@�헚���}�X�^�[�e�[�u��
#define DB_ST_HISTORY_INF			"T_ST_HISTORY_INF"		// [DB] �@�헚���e�[�u��
#define DB_STATUS_LOG				"T_STATUS_LOG"			// [DB] �@���ԃ��O�e�[�u��
#define DB_BASE_ACTION_MASTER		"M_BASE_ACTION_MASTER"	// [DB] ��{����ꗗ�ݒ�

#define DB_COMMON_MASTER			"M_COMMON_MASTER"		// [DB] �r���萔�}�X�^�[
#define DB_TYPE_NAME				"TYPE_NAME"				// [DB] �r��}�X�^�[
#define DB_GRADE_NAME				"GRADE_NAME"			// [DB] �O���[�h�}�X�^�[
#define DB_SCODE_MASTER				"M_SCODE_MASTER"		// [DB] �����\�ʏ�ԃ}�X�^�[
#define DB_SCODE_JUDGE				"M_SCODE_JUDGE"			// [DB] �����\�ʏ�ԏ����ݒ�
#define DB_SCODE_OUTPUT				"M_SCODE_OUTPUT"		// [DB] �����\�ʏ�ԕʏo�͐ݒ�
#define DB_PCODE_MASTER				"M_PCODE_MASTER"		// [DB] �����p�^�[���}�X�^�[
#define DB_PCODE_JUDGE				"M_PCODE_JUDGE"			// [DB] �����p�^�[�������ݒ�
#define DB_PCODE_OUTPUT				"M_PCODE_OUTPUT"		// [DB] �����p�^�[���ʏo�͐ݒ�
#define DB_PALL_OUTPUT				"M_PALL_OUTPUT"			// [DB] �����p�^�[���ŗL�o�͐ݒ�
#define DB_PCOMMON_OUTPUT			"M_PCOMMON_OUTPUT"		// [DB] �����p�^�[�����ʏo�͐ݒ�

#define	DB_CAMERA_LINKSWITCH		"M_CAMERA_LINKSWITCH"	// [DB] �J���������N�X�C�b�`�ݒ�
#define DB_PAR_GRID_SETTING			"M_PAR_GRID_SETTING"	// [DB] ����������ʃf�[�^�O���b�h�\���ݒ�
#define DB_PAR_LOV_ITEM				"M_PAR_LOV_ITEM"		// [DB] ����������ʑI�����ڃA�C�e���}�X�^

// �p�����[�^
#define DB_ATTR_NAME				"ATTR_NAME"				// [DB] �����ʖ��̃}�X�^�[
#define DB_PARAM_COMMON				"PARAM_COMMON"			// [DB] ���ʃp�����[�^
#define DB_PARAM_SCODE				"PARAM_SCODE"			// [DB] �\�ʏ�ԕʃp�����[�^
#define DB_PARAM_CAMSET				"PARAM_CAMSET"			// [DB] �J�����Z�b�g�ʃp�����[�^
#define DB_PARAM_CAMSET_SCODE		"PARAM_CAMSET_SCODE"	// [DB] �J�����Z�b�g�ʁ~�\�ʏ�ԕʃp�����[�^
#define DB_PARAM_CAMSET_ANGLE		"PARAM_CAMSET_ANGLE"	// [DB] �J�����Z�b�g�ʁ~�J�����p�x�ʃp�����[�^
#define DB_PARAM_CAMANGLE_SCODE		"PARAM_CAMANGLE_SCODE"	// [DB] �J�����p�x�ʁ~�\�ʏ�ԕʃp�����[�^
#define DB_PARAM_CAMSET_ANGLE_SCODE	"PARAM_CAMSET_ANGLE_SCODE"	// [DB] �J�����Z�b�g�ʁ~�J�����p�x�ʁ~�\�ʏ�ԕʃp�����[�^
#define DB_ROLL_INFO				"ROLL_INFO"             // [DB] ���[�����
#define DB_PARAM_PCINFO_DESC		"PCINFO_DESC"			// [DB] ��ʏ����e
#define DB_PARAM_REDUCTION			"PARAM_REDUCTION"		// [DB] �k����
#define DB_PARAM_FILTER				"PARAM_FILTER"			// [DB] �t�B���^ 
#define DB_PARAM_THRESHOLD			"PARAM_THRESHOLD"		// [DB] ���d�p臒l
#define DB_PARAM_CYCLE_THRESHOLD	"PD_PARAM_THRESHOLD"	// [DB] �����p臒l
#define DB_PARAM_CROP_SHRINK        "PARAM_CROP_SHRINK"     // [DB] �Ԉ���

// <20221117> <����> <�ŏI�H�����@�\������1> --->>>
#define DB_PARAM_LABELIMAGE			"PARAM_LABELIMAGE"		// [DB]
#define DB_PARAM_SIMPLEBIN			"PARAM_SIMPLEBIN"		// [DB]
#define DB_PARAM_FILTERBIN			"PARAM_FILTERBIN"		// [DB]
// <20221117> <����> <�ŏI�H�����@�\������1> <<<---

//=======================================
// �N���v��
//=======================================

//// ���� (9000�ԑ���g�p)
//// �x���@�\

// KS_MASTER
#define FACT_KS_MASTER_01			9101					// �����ύX�v�� (��TO_DEFECT, SO_GAMENN?)
#define FACT_KS_MASTER_02			9102					// �n�[�g�r�[�g (��KS_SLAVE)
#define FACT_KS_MASTER_03			9103					// PC�}�X�^�[�Ǘ���Ԗ₢���킹 (��KS_SLAVE)
#define FACT_KS_MASTER_04			9104					// �����グ������ (��KS_SLAVE, KS_MASTER)
#define FACT_KS_MASTER_05			9105					// �������������� (��KS_SLAVE, KS_MASTER)
#define FACT_KS_MASTER_06			9106					// �V�X�e���I���v�� (��SO_GAMENN, KS_UPSMGR)
#define FACT_KS_MASTER_07			9107					// PC�ċN���v�� (��SO_GAMENN)
#define FACT_KS_MASTER_08			9108					// PC���O�I�t�v�� (��SO_GAMENN)
#define FACT_KS_MASTER_09			9109					// PC�풓�^�X�N�I���v�� (��KS_MASTER)

// KS_SLAVE
#define FACT_KS_SLAVE_01			9121					// �����ύX�v�� (��KS_MASTER)
#define FACT_KS_SLAVE_02			9122					// �����n�[�g�r�[�g (��KS_MASTER)
#define FACT_KS_SLAVE_03			9123					// PC�}�X�^�[�Ǘ��N�������ʒm (��KS_MASTER)
#define FACT_KS_SLAVE_04			9124					// �V���b�g�_�E���v�� (��KS_MASTER)
#define FACT_KS_SLAVE_05			9125					// PC�ċN���v�� (��KS_MASTER)
#define FACT_KS_SLAVE_06			9126					// PC���O�I�t�v�� (��KS_MASTER)
#define FACT_KS_SLAVE_07			9127					// PC�풓�^�X�N�I���v�� (��KS_MASTER)

// KS_START
////////9131

// KS_SYSLOG
#define	FACT_KS_SYSLOG_01			9141					// �V�X���O�o��
#define	FACT_KS_SYSLOG_02			9142					// �@���ԃ��O

// KS_EVTLOG
////9151					// �C�x���g���O�擾 (��KS_EVTLOG)

// KS_WAKEUP
#define FACT_KS_WAKEUP_01			9161					// ���������グ������ (��KS_MASTER, KS_SLAVE)
#define FACT_KS_WAKEUP_02			9162					// �~�ϗ����グ������ (��KS_MASTER, KS_SLAVE)

// KS_UPSMGR
///////9171

// KS_BACKUP
///////9181

// KS_EQALDB
///////9191
#define FACT_KS_EQALDB_01			9191					// �������s�ʒm�i��KS_BACKUP�j
//#define FACT_KS_EQALDB_02			9192					// ���шړ��ʒm�i��KS_BACKUP�j

// KS_MAILTR
///////9231
#define FACT_KS_MAILTR_01			9231					// ���[���X���b�g�]���v��(mailtr_que)

//// �~�ϋ@�\
// TI_DELETE
#define FACT_TI_DELETE_01			9411					// �폜������D�惂�[�h�ʒm (��TO_DEFECT)


// 9800�`9899 �̓c�[�����Ŏg�p�̈ח\��

// �z���g�ɋ��ʂŎg�p
#define FACT_KS_TASK_STATEUS		9901					// �^�X�N��Ԏ擾�v�� (�`9920�܂ŗ\��)
#define FACT_KS_TASK_STATEUS_ANS	9902					// �^�X�N��Ԏ擾�A���T�[
#define FACT_KS_DB_STATEUS			9997					// DB��ԕύX�ʒm (fl.data[0]= 0:���� 1:�ُ�)	
#define FACT_KS_PCNAME_CHANGE		9998					// PC���̕ύX�ʒm�N���v��
#define FACT_KS_END					9999					// �^�X�N�I���N���v��


//=======================================
// �X���b�h�� �N���v�� C++ �̂�
//=======================================
#define WM_USER_TH_END				WM_USER+999				// �X���b�h�I���v��

//=======================================
// ���[���X���b�g�ʐM�p�\����
//=======================================
struct COMMON_QUE {
	long nEventNo;											// �C�x���g�R�[�h
	long nLineNo;											// ���C��No (�ʏ�́ALINE_ID�Œ�B1PC�ŕ����̃V�X�e�����������Ă���ꍇ�� 0��1RCL 1��2RCL�p�Ƃ��Ɏg�p����B)

	union {
		// �W��[�����̂�]
		struct FL{
			long data[(SIZE_MSL_DATA / sizeof(long))];
		} fl;

		// �W��[�����̂�]
		struct FC{
			char data[SIZE_MSL_DATA];
		} fc;

		// �W��[����&���� ����] (�ŏ���80byte�����l)
		struct MIX{
			long idata[20];
			char cdata[5][32];
			char ydata[SIZE_MSL_DATA-sizeof(long)*20-32*5];
		} mix;



		// �V�X�e�����O���b�Z�[�W
		struct {
			long	nSysNo;									// ���b�Z�[�WNo.
			WORD	date[6];								// ��PC����
			long	nPcKind;								// PCKIND
			char	cTaskName[SIZE_NAME_TASK];				// �^�X�N��
			char	cPcName[SIZE_NAME_PC];					// �o�b��
															// ���b�Z�[�W���e
			char	cMsg[SIZE_MSL_DATA-sizeof(long)*2-sizeof(WORD)*6-SIZE_NAME_TASK-SIZE_NAME_PC];	
		} syslog_que;
		// �@���ԕύX���b�Z�[�W
		struct {
			long	nSysNo;									// ���b�Z�[�WNo.
			WORD	date[6];								// ��PC����
			long	nPcKind;								// PCKIND
			char	cTaskName[SIZE_NAME_TASK];				// �^�X�N��
			char	cPcName[SIZE_NAME_PC];					// �o�b��
			long	nStateId;								// �@����ID
			long	nStateVal;								// �@���Ԓl
															// ���b�Z�[�W���e
			char	cMsg[SIZE_MSL_DATA-sizeof(long)*4-sizeof(WORD)*6-SIZE_NAME_TASK-SIZE_NAME_PC];	
		} statelog_que;

		// �����ݒ�v��
		struct {
			SYSTEMTIME systemtime;
			char reserve[SIZE_MSL_DATA-sizeof(SYSTEMTIME)];
		} timeset_que;

		// ���[���X���b�g�]���v��
		struct {
			unsigned long nUdpIpAddr;								// ���̃��[�����󂯂��^�X�N���AUDP��ł���
																	// inet_addr�������ʁB�\������Ƃ��́Ainet_ntoa()�Ƃ��g���B
			char cTaskName[SIZE_NAME_TASK];							// UDP���󂯂��^�X�N���A�]������^�X�N��
			char cPcName[SIZE_NAME_PC];								// UDP���󂯂��^�X�N���A�]������z�X�g��
			char cMsg[SIZE_MSL_DATA - sizeof(unsigned long) - SIZE_NAME_TASK - SIZE_NAME_PC];
		} mailtr_que;
	};

public:
	COMMON_QUE() {											// �f�t�H���g�R���X�g���N�^
		nEventNo = 0;
		nLineNo	 = 0;
		memset(fc.data, 0x00, sizeof(fc.data));
	}
};

// ���[���p�A�C�e���Z�b�g�ꎮ (����ƕ֗��Ȃ̂Œǉ�)
struct MAIL_ITEM {
	char		cTask[SIZE_NAME_TASK];						// �^�X�N����
	char		cPc[SIZE_NAME_PC];							// PC����
	COMMON_QUE	Que;										// ���f�[�^
};

//=======================================
// �@���� ��{�\����
//=======================================
// �X�̋@����
struct TYP_STATUS_INF 
{
	int					id;									// �@��ID
	int					pcid;								// PCID (0:�Y�������B�@�\�Ɋ���t��   1�`�FPC�Ɋ���t��)
	int					area;								// �ُ�͈� (0:�S�� 1:�\�� 2:����)
	EM_DIV_KENSA		kensa;								// �����ُ�x (0:���Q 1:�k��)
	EM_DIV_KIKI			kiki;								// �@��ُ�x (0:���Q 1:�y�̏� 2:���̏� 3:�d�̏�)
	int					yusen;								// �D��x (1:�ŋ� NULL=99999�F�Ŏ�)
	char				msg[256];							// �ُ펞�̃��b�Z�[�W
	int					pcmode;								// PC�敪 (0:�I�����C��  1:�I�t���C��)

	int					stat;								// �����

};
// �V�X�e���̋@����
struct TYP_STATUS_NOWSYSTEM 
{
	// (�I�t���C��PC�͑ΏۊO)
	EM_DIV_KENSA		kensa[2];							// �\���� �����ُ�x (0:���Q 1:�k�� 9:��~) DIV_KENSA
	EM_DIV_KIKI			kiki[2];							// �\���� �@��ُ�x (0:���Q 1:�y�̏� 2:���̏� 3:�d�̏�) DIV_KIKI

	int					nCnt;								// �ُ�Z�b�g����
	TYP_STATUS_INF		ngInf[5];							// �ُ�̋@������5��
};

//=======================================
// �r���\�P�b�g�ʐM�p�\���� C++�̂�
//=======================================
// �g�����U�N�V�����R�[�h
#define T_CODE_NAME				"NM"						// [����PC]-[����PC]�ԒʐM �C�j�V�����`��
#define T_CODE_DEFECT			"KZ"						// [����PC]-[����PC]�ԒʐM �r���`��
															// [����PC]-[����PC]�ԒʐM �r���`��
#define T_CODE_HELCY			"HL"						// [����PC]-[����PC]�ԒʐM �w���V�[�`��

// �`���T�C�Y
#define SIZE_SOCK_TC			2							// �g�����U�N�V�����R�[�h�T�C�Y
#define SIZE_SOCK_HEAD			256							// �\�P�b�g���ʕ��w�b�_�[���T�C�Y
#define SIZE_SOCK_DATA_MAX		2*1024*1024					// �f�[�^���̍ő呗�M�T�C�Y (2M) (���̃T�C�Y�ȏ�̃f�[�^���𑗐M���悤�Ƃ���ƃ��C�u�������Ŏ����I�ɕ��������)

// �w�b�_�[�@���ʕ� [48byte]
struct SOCK_BASE_HEAD {
	char			code[SIZE_SOCK_TC];						// �g�����U�N�V�����R�[�h
	short			subinf;									// �T�u��� (�ePC�ԂňӖ��������ω�)
	char			pname[SIZE_NAME_PC];					// ���M��PC���
	long			len;									// �`����(�w�b�_�[���{�f�[�^��)
	long*			addr[2];								// �f�[�^���̃A�h���X (NULL�łȂ��ꍇ�́A�g���I�������ɕK�� delete���s��)
															// (������A�g��Ȃ����ł��K��NULL�ɂ��Ă�����)
															// �����̒l�́A�`������Ȃ��̂Œ��ӁB
	// x86���� 16byte�Bx64���� 4byte �̋󂫂��ł���
	char			yobi[48-SIZE_SOCK_TC-sizeof(short)-SIZE_NAME_PC-sizeof(long)-sizeof(long*)*2 ];
};

// �_�~�[�̃w�b�_�[�� (���C�u�������Ŏg�p����ׂ̃w�b�_�\����)
struct SOCK_F_HEAD {
	SOCK_BASE_HEAD	base_head;								// ���ʃw�b�_��
	char			reserve[SIZE_SOCK_HEAD-sizeof(SOCK_BASE_HEAD)];	// �ePC�ԒʐM�ŉσw�b�_�[��
};


//=======================================
// �V�X���ONo
//=======================================
//// ����								// No		// �T�u���b�Z�[�W�`��		// ���b�Z�[�W

// �S�� 0
#define SYSNO_NON						0			//							// [���[���X���b�g] No�����蓖��

// ���[���֘A 1�`4
#define SYSNO_MAIL_SNED_ERR				1			// [�ʒm�� err_code = %d]	// [���[���X���b�g] ���M���s		
#define SYSNO_MAIL_RECV_ERR				2			// [�ʒm�� err_code = %d]	// [���[���X���b�g] ��M���s	
#define SYSNO_MAILNO_ERR				3			// [event = %d]				// [���[���X���b�g] �C�x���g�R�[�h�s��
// �蓮�R�����g�֘A 5�`7
#define SYSNO_COMMENT_ALL				5			//							// �����(�S��)
#define SYSNO_COMMENT_SOFT				6			//							// �����(�\�t�g)
#define SYSNO_COMMENT_HARD				7			//							// �����(�n�[�h)


// �\�P�b�g 10�`24
#define SYSNO_SOCK_SEND_TIMEOUT			10			// [�ʐM��]					// [SOCKET] ���M�^�C���A�E�g
#define SYSNO_SOCK_NEW_ERR				11			// [err_code = %d]			// [SOCKET] �������s
#define SYSNO_SOCK_SEND_ERR				12			// [err_code = %d]			// [SOCKET] ���M�G���[
#define SYSNO_SOCK_CONNECT				13			// [�ʐM��]					// [SOCKET] �ڑ�����
#define SYSNO_SOCK_CLOSE				14			// [�ʐM��]					// [SOCKET] �ؒf
#define SYSNO_SOCK_RECV_ERR				15			// [section=%s err_code=%d]	// [SOCKET] ��M�G���[
#define SYSNO_SOCK_LISTEN_ERR			16			// [section=%s err_code=%d]	// [SOCKET] Listen�G���[
#define SYSNO_SOCK_CLIENT_MAX			17			//							// [SOCKET] �N���C�A���g���I�[�o�[
#define SYSNO_SOCK_INITIALIZ			18			// [name=%s]				// [SOCKET] �C�j�V�����`����M
#define SYSNO_SOCK_TC_NG				19			// [section=%s code=%4.4s]	// [SOCKET] �g�����U�N�V�����R�[�h�ُ�
#define SYSNO_SOCK_RECV_TIMEOUT			20			//							// [SOCKET] ��M�^�C���A�E�g
#define SYSNO_SOCK_CONNECT_ERR			21			// [err_code = %d]			// [SOCKET] �ڑ��ُ�
#define SYSNO_SOCK_HELCY_TIMEOUT		22			// [�ʐM��]					// [SOCKET] �w���V�[�Ď��^�C���A�E�g

// ���L������ 25�`29
#define SYSNO_MEM_OPEN					25			// [���L��������]           // [���L������] �I�[�v�����s
#define SYSNO_MEM_READ				    26			// [���L��������]           // [���L������] �ǂݍ��ݎ��s
#define SYSNO_MEM_WRITE					27			// [���L��������]           // [���L������] �������ݎ��s

// DB 30�`39
#define SYSNO_DB_EXECUTEDIRECT_ERR		30			// [�e�[�u����]				// [DB] SqlExecuteDirect�G���[	
#define SYSNO_DB_FETCH_ERR				31			// [�e�[�u����]				// [DB] SqlFetch�G���[	
#define SYSNO_DB_CONNECT_ERR			32			// [�e�[�u����]				// [DB] SqlConnect�G���[	
#define SYSNO_DB_DATA_ERR				33			// [�e�[�u����, ��=%d]		// [DB] �f�[�^�s��	
#define SYSNO_DB_BINALYWRITE_ERR		34			// [�e�[�u����]				// [DB] �o�C�i�������݃G���[	
#define SYSNO_DB_ODBC_SET_OK			35			// [ODBC��, �A�h���X]		// [DB] ODBC�ݒ菑������
#define SYSNO_DB_ODBC_SET_ERR			36			// [ODBC��, �A�h���X]		// [DB] ODBC�ݒ菑�����s

// RS232C 40�`45
#define SYSNO_232C_NEW_ERR				40			// [err_code=%d]			// [232C] �������s
#define SYSNO_232C_SEND_ERR				41			// [err_code=%d]			// [232C] ���M�G���[
#define SYSNO_232C_RECV_ERR				42			// [err_code=%d]			// [232C] ��M�G���[
#define SYSNO_232C_OPEN					43			// [�ʐM��]					// [232C] �ڑ�����
#define SYSNO_232C_CLOSE				44			// [�ʐM��]					// [232C] �ؒf

// PIPE 46�`49
#define SYSNO_PIPE_OPEN					45			//							// [PIPE] �ڑ�����
#define SYSNO_PIPE_CLOSE				46			//							// [PIPE] �ؒf����
#define SYSNO_PIPE_CONNECT_ERR			47			// [err_code=%d]			// [PIPE] �ڑ����s
#define SYSNO_PIPE_EXEC					48			//							// [PIPE] ���쐳�튮��
#define SYSNO_PIPE_EXEC_ERR				49			// [err_code=%d]			// [PIPE] ����ُ�

// 50�`69 ���̑��O���[�v
#define SYSNO_CNT_ERR					50			// [err_code=%d sub_code=%d]	// [�J�E���^�[�{�[�h] �ُ픭��
#define SYSNO_PIO_ERR					51			// [err_code=%d sub_code=%d]	// [PIO�{�[�h] �ُ픭��
#define SYSNO_AIO_ERR					52			// [err_code=%d sub_code=%d]	// [�A�i���O�{�[�h] �ُ픭��
#define SYSNO_PLG_ERR					53			// [�����p=%0.3f ��r�p=%0.3f ��=%d]	// [�J�E���^�[�{�[�h] PLG�������ُ�

#define SYSNO_STATUS_OK					55			// [�@�햼]					// [�@���ԊĎ�] �@���Q����
#define SYSNO_STATUS_NG					56			// [�@�햼]					// [�@���ԊĎ�] �@���Q���o
#define SYSNO_EVTLOG_MSG_NG				57			// [���b�Z�[�W��]			// [�C�x���g���O�Ď�] �ُ팟�o
#define SYSNO_EVTLOG_AP_NG				58			// [�v���Z�X��]				// [�C�x���g���O�Ď�] �A�v���P�[�V�����ُ팟�o
#define SYSNO_STATUS_WAR				59			// [�@�햼]					// [�@���ԊĎ�] �@��x�����o (�y�̏�)			���v���r�W����

// 70�`99 ��ʂ������
#define SYSNO_TASK_INIT_ERR				70			// [�^�X�N��]				// [LIB] �^�X�N�����N���������s
#define SYSNO_INI_READ_ERR				71			// [�Z�b�V����,�L�[]		// [INI] �擾�G���[
#define SYSNO_PCNAME_GET_ERR			72			//							// [LIB] PC���̎擾���s
#define SYSNO_QUEFULL_ERR				73			// [�敪����]				// [LIB] �����o�b�t�@�L���[�t���I�I
#define SYSNO_QUEFULL_WAR				74			// [�敪����]				// [LIB] �����o�b�t�@�L���[�t��

#define SYSNO_UPS_NG					80			//							// �d���ُ�
#define SYSNO_TIMESET_NG				81			// [err_code = %d]			// �����ݒ莸�s	
#define SYSNO_RESTAET_ERR				82			// 							// �ċN�����s	
#define SYSNO_SHUTDOWN_ERR				83			//							// �V���b�g�_�E�����s
#define SYSNO_TIMESET_OK				84			//							// �����ݒ芮��
#define SYSNO_DELETE_ERR				85			// 							// DB�폜���s
#define SYSNO_COMPRESS_ERR				86			//							// DB���k���s
#define SYSNO_RESTAET_OK				87			// [PC��]					// PC�ċN���v����t						
#define SYSNO_SHUTDOWN_OK				88			//							// �V�X�e���I���v����t
#define SYSNO_SYSTEM_START				89			// [�J�n����]				// �����J�n�v��
#define SYSNO_SYSTEM_STOP				90			//							// ������~�v��
#define SYSNO_LOGOFF_ERR				91			// 							// ���O�I�t���s	
#define SYSNO_LOGOFF_OK					92			// [PC��]					// PC���O�I�t�v����t						
#define SYSNO_DB_CHECK_RETRY			93			// [���g���C��]			// �f�[�^�x�[�X�ڑ��`�F�b�N�Őڑ����s�B
#endif
  