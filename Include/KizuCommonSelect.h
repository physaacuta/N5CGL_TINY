// *********************************************************************************
//	�S�Ӄւł̐ؑ֒萔�̃C���N���[�h
//	[Ver]
//		Ver.01    2016/06/09  ���ō쐬
//
//	[����]
//		��{�I�ɁA���������� (�؂�ւ��X�C�b�`�p�̂�)
// *********************************************************************************

#ifndef	_KIZUCOMMONSELECT_H
#define	_KIZUCOMMONSELECT_H

// �ӃփC���N���[�h
#include "LibSelect.h"										// ���C�u�����ؑփw�b�_�[
#include "KizuListID.h"										// �r�����ʃw�b�_�[

//***************************************************************
// �ݒ�ϒ萔 ��`
//***************************************************************
//=======================================
// ���W�J�p���C���敪
//=======================================
// LINE�X�C�b�` (�K��1�R�̂ݗL���ɂ��鎖)
// #define LINE_		����{�n

// �Ӄ�
#define LINE_5CGL_TINY



//=======================================
// �@�\�ǉ� �ߓn���p�X�C�b�` (define��`�ŋ@�\�L�����ƂȂ�悤�ɍ�邱��)
//=======================================
#define ENABLE_RPI_SAVE										// �S���ۑ��@�\�L��
#define ENABLE_RPI_TRANS									// �t���[���摜�]��(HT��RPI)�L��


//=======================================
//// �r���ݔ��֌W
//=======================================
//// �J�����ݒu �������ʒu
//#define NUM_CAM_POS_1				1						// �ʓ���̃J�����ʒu�� (C)
//#define NUM_CAM_POS_2				2						// �ʓ���̃J�����ʒu�� (DS,WS)
//#define NUM_CAM_POS_3				3						// �ʓ���̃J�����ʒu�� (DS,C,WS)
//#define NUM_CAM_POS_4				4						// �ʓ���̃J�����ʒu�� (DS,D1,W1,WS)
//#define NUM_CAM_POS_5				5						// �ʓ���̃J�����ʒu�� (DS,D1,C,W1,WS)
//#define NUM_CAM_POS_6				6						// �ʓ���̃J�����ʒu�� (DS,D1,D2,W2,W1,WS)
//#define NUM_CAM_POS_7				7						// �ʓ���̃J�����ʒu�� (DS,D1,D2,C,W2,W1,WS)
//#define NUM_CAM_POS_8				8						// �ʓ���̃J�����ʒu�� (DS,D1,D2,D3,W3,W2,W1,WS)
//#define NUM_CAM_POS_9				9						// �ʓ���̃J�����ʒu�� (DS,D1,D2,D3,C,W3,W2,W1,WS)
#define NUM_CAM_POS_10			10						// �ʓ���̃J�����ʒu�� (DS,D1,D2,D3,D4,W4,W3,W2,W1,WS)
//#define NUM_CAM_POS_11			11						// �ʓ���̃J�����ʒu�� (DS,D1,D2,D3,D4,C,W4,W3,W2,W1,WS)
//#define NUM_CAM_POS_12			12						// �ʓ���̃J�����ʒu�� (DS,D1,D2,D3,D4,D5,W5,W4,W3,W2,W1,WS)
//#define NUM_CAM_POS_13			13						// �ʓ���̃J�����ʒu�� (DS,D1,D2,D3,D4,D5,C,W5,W4,W3,W2,W1,WS)
//#define NUM_CAM_POS_14			14						// �ʓ���̃J�����ʒu�� (DS,D1,D2,D3,D4,D5,D6,W6,W5,W4,W3,W2,W1,WS)
//#define NUM_CAM_POS_15			15						// �ʓ���̃J�����ʒu�� (DS,D1,D2,D3,D4,D5,D6,C,W6,W5,W4,W3,W2,W1,WS)
//#define NUM_CAM_POS_16			16						// �ʓ���̃J�����ʒu�� (DS,D1,D2,D3,D4,D5,D6,D7,W7,W6,W5,W4,W3,W2,W1,WS)




//// �J�����ݒu �p�x
//#define NUM_CAM_ANGLE_1				1						// �J�����p�x�� (1=�P��)
//#define NUM_CAM_ANGLE_2				2						// �J�����p�x�� (2=���E��)
//#define NUM_CAM_ANGLE_3				3						// �J�����p�x�� (3=���E���E��)
#define NUM_CAM_ANGLE_4				4						// �J�����p�x�� (4=���E���E���E��)		������ (G,B,R, R-B)
//#define NUM_CAM_ANGLE_5				5						// �J�����p�x�� (5=���E���E���E���E��)


// �J�����ݒu �������ʒu (�����X�C�b�`)
#ifdef NUM_CAM_POS_1
	#define NUM_CAM_POS					NUM_CAM_POS_1
#endif
#ifdef NUM_CAM_POS_2
	#define NUM_CAM_POS					NUM_CAM_POS_2
#endif
#ifdef NUM_CAM_POS_3
	#define NUM_CAM_POS					NUM_CAM_POS_3
#endif
#ifdef NUM_CAM_POS_4
	#define NUM_CAM_POS					NUM_CAM_POS_4
#endif
#ifdef NUM_CAM_POS_5
	#define NUM_CAM_POS					NUM_CAM_POS_5
#endif
#ifdef NUM_CAM_POS_6
	#define NUM_CAM_POS					NUM_CAM_POS_6
#endif
#ifdef NUM_CAM_POS_7
	#define NUM_CAM_POS					NUM_CAM_POS_7
#endif
#ifdef NUM_CAM_POS_8
	#define NUM_CAM_POS					NUM_CAM_POS_8
#endif
#ifdef NUM_CAM_POS_9
	#define NUM_CAM_POS					NUM_CAM_POS_9
#endif
#ifdef NUM_CAM_POS_10
	#define NUM_CAM_POS					NUM_CAM_POS_10
#endif
#ifdef NUM_CAM_POS_11
	#define NUM_CAM_POS					NUM_CAM_POS_11
#endif
#ifdef NUM_CAM_POS_12
	#define NUM_CAM_POS					NUM_CAM_POS_12
#endif
#ifdef NUM_CAM_POS_13
	#define NUM_CAM_POS					NUM_CAM_POS_13
#endif
#ifdef NUM_CAM_POS_14
	#define NUM_CAM_POS					NUM_CAM_POS_14
#endif
#ifdef NUM_CAM_POS_15
	#define NUM_CAM_POS					NUM_CAM_POS_15
#endif
#ifdef NUM_CAM_POS_16
	#define NUM_CAM_POS					NUM_CAM_POS_16
#endif


// �J�����ݒu �p�x (�����X�C�b�`)
#ifdef NUM_CAM_ANGLE_1
	#define NUM_CAM_ANGLE				NUM_CAM_ANGLE_1
#endif
#ifdef NUM_CAM_ANGLE_2
	#define NUM_CAM_ANGLE				NUM_CAM_ANGLE_2
#endif
#ifdef NUM_CAM_ANGLE_3
	#define NUM_CAM_ANGLE				NUM_CAM_ANGLE_3
#endif
#ifdef NUM_CAM_ANGLE_4
	#define NUM_CAM_ANGLE				NUM_CAM_ANGLE_4
#endif


//***************************************************************
// �ݒ�萔 ��`
//***************************************************************

// ���W�J�pID
// #define LINE_ID_		����{�n
#define LINE_ID_N5CGL_TINY		0

// �����C��ID (�K�{�F���W�J�����Ȃ�0��OK)
#define LINE_ID					LINE_ID_N5CGL_TINY


// �Ǘ�No�̐擪��2���R�[�h
#define KIZUKEN_CODE			KIZUKEN_CODE_N5CGL_TINY


// �H���R�[�h
#define KOUTEI_CODE				KOUTEI_CODE_5CGL


//=======================================
// PC���� (C++�̂�)
//	�� ����ȊO�̍��ڂɂ��ẮALineCommon�ɒ�`����
//=======================================
#define MAX_PCCOUNT					64					// �{�V�X�e���̑SPC�� (�o�b�N�A�b�v�@�܂�)
extern char PCNAME[MAX_PCCOUNT][16];					// PC���̋L���̈� (PCKIND��) (��{�I�ɒ��ڎQ�Ƌ֎~) (�����ł́A�錾�̂݁B���̂́AKizuLib.cpp���ɂ���)

//// PC�J�n�ʒu (PCKIND�̏���) ��0�I���W��
#define PCNO_HANTEI			0
#define PCNO_CYCLE			PCNO_HANTEI					// �����������̏ꍇ�́AHANTEI�Ɠ����ł悢
//#define PCNO_HTBACK			(5-1)						// ����̃o�b�N�A�b�vPC
#define PCNO_PARA			(15-1)

#define PCNO_TOKATU			(21-1)
#define PCNO_TIKUSEKI		PCNO_TOKATU					// �������~��PC�ׁ̈APCID����
#define	PCNO_BACKUP			(22-1)
#define	PCNO_HYOZI			(26-1)
#define	PCNO_SOUSA			(28-1)
#define	PCNO_RPIEXEC		(31-1)
#define	PCNO_RPISAVE		(34-1)


//// PC���̎擾�}�N��
#ifndef LOCAL	// �{�ԗp
		#define PCID_NAME(pckind)		PCNAME[pckind-1]			// PC���� (pcid 1�I���W��)�@PCNO_*** ���g���ꍇ�́APCID_NAME(***+1) ���邱�ƁB

		#define HANTEI(no)				PCNAME[PCNO_HANTEI+no]		// ����@�\��L����PC���� (0�I���W��)
		#define CYCLE(no)				PCNAME[PCNO_CYCLE+no]		// ��������@�\��L����PC���� (0�I���W��)
//		#define HTBACK(no)				PCNAME[PCNO_HTBACK+no]		// ����̃o�b�N�A�b�v�@�\��L����PC���� (0�I���W��)

		#define TOKATU					PCNAME[PCNO_TOKATU]			// �����@�\��L����PC����
		#define TIKUSEKI				PCNAME[PCNO_TIKUSEKI]		// �~�ϋ@�\��L����PC����
		#define SOUSA					PCNAME[PCNO_SOUSA]			// ����@�\��L����PC����				�������g��Ȃ��B����P�Ƃ̒ʒm�͖���
		#define HYOUZI(no)				PCNAME[PCNO_HYOZI+no]		// �\���@�\��L����PC���� (0�I���W��)
		#define RPIEXEC(no)				PCNAME[PCNO_RPIEXEC+no]		// �S���摜�����@�\��L����PC���� (0�I���W��)
		#define RPISAVE(no)				PCNAME[PCNO_RPISAVE+no]		// �S���摜�ۑ��@�\��L����PC���� (0�I���W��)
#else			// �J���p
		#define PCID_NAME(pckind)		PCNAME[pckind-1]			// PC���� (pcid 1�I���W��)

		#define HANTEI(no)				PCNAME[PCNO_HANTEI+no]		// ����@�\��L����PC���� (0�I���W��)
		#define CYCLE(no)				PCNAME[PCNO_CYCLE+no]		// ��������@�\��L����PC���� (0�I���W��)
//		#define HTBACK(no)				PCNAME[PCNO_HTBACK+no]		// ����A�����A�S���̃o�b�N�A�b�v�@�\��L����PC���� (0�I���W��)
		#define PARA(no)				PCNAME[PCNO_PARA+no]		// �p����������@�\��L����PC���� (0�I���W��)

		#define TOKATU					PCNAME[PCNO_TOKATU]			// �����@�\��L����PC����
		#define TIKUSEKI				PCNAME[PCNO_TIKUSEKI]		// �~�ϋ@�\��L����PC����
		#define SOUSA					PCNAME[PCNO_SOUSA]			// ����@�\��L����PC����				���g��Ȃ��B����P�Ƃ̒ʒm�͖���
		#define HYOUZI(no)				PCNAME[PCNO_HYOZI+no]		// �\���@�\��L����PC���� (0�I���W��)
		#define RPIEXEC(no)				PCNAME[PCNO_RPIEXEC+no]		// �S���摜�����@�\��L����PC���� (0�I���W��)
		#define RPISAVE(no)				PCNAME[PCNO_RPISAVE+no]		// �S���摜�ۑ��@�\��L����PC���� (0�I���W��)
#endif

#endif
  