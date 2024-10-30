#pragma once

#include "LibSelect.h"										// ���C�u�����ؑփw�b�_�[

//***************************************************************
// PIO�֌W
//***************************************************************
//=======================================
// �萔

//// ���L����������
#ifndef DEFECTLIB_CONTEC_PIO_WMD		// �]����
	#define SMYU_PIO_TBL_NAME  "TBL_GOT_PIO_%d"
#else									// WMD��
	#define SMYU_PIO_TBL_NAME  "TBL_GOT_PIO_%s"
#endif
#define SMYU_PIO_NET_TBL_NAME  "TBL_GOT_PIO_%s"

//// �萔
#define SMYU_PIO_MAX_BOOD		4							// �ő�{�[�h��
#define SMYU_PIO_MAX_PORT		8							// �ő�|�[�g��
#define SMYU_STORY_MAX_FILE		4							// �ő�V�i���I��


//=======================================
// �\����

// PIO���L�������\����
struct SMYU_TBL_PIO {
	BYTE		nDi[SMYU_PIO_MAX_PORT];						// �ǂݍ��݃o�b�t�@
	BYTE		nDo[SMYU_PIO_MAX_PORT];						// �������݃o�b�t�@
};


//***************************************************************
// PLG�֌W
//***************************************************************
//=======================================
// �萔

//// ���L����������
#define SMYU_PLG_TBL_NAME  "TBL_GOT_PLG_%s"

//// �萔
#define SMYU_PLG_MAX_CHANNEL		8						// �ő�`�����l����
#define SMYU_PLG_MAX_SAMPBUFNUM		16						// �T���v�����O�o�b�t�@��
//#define SMYU_PLG_MAX_SAMPBUFNUM		32						// �T���v�����O�o�b�t�@��
#define SMYU_PLG_MAX_SAMPBUFSIZE	4096					// �T���v�����O�o�b�t�@�T�C�Y
#define SMYU_PLG_BUNSYU				16//4						// ���� (�w�肵���t���[������/���̒l) (=1�t���[���ł��̉񐔕������������ăZ�b�g����) �� �c�t���[���T�C�Y�Ŋ���؂��l�ɂ��Ă�


//=======================================
// �\����

// PLG���L�������\����
struct SMYU_TBL_PLG {
	DWORD		nSamplingCount;								// �ŐV�̃T���v�����O�J�E���g�l

	DWORD		nCount[SMYU_PLG_MAX_CHANNEL];				// �ŐV�̃J�E���g�l
	DWORD		nSampBuf[SMYU_PLG_MAX_SAMPBUFNUM*SMYU_PLG_MAX_SAMPBUFSIZE][SMYU_PLG_MAX_CHANNEL];	// �T���v�����O�o�b�t�@

	double		dLen[SMYU_PLG_MAX_CHANNEL];					// �ʔ���
};


//***************************************************************
// �f�n�s�V�~�����[�^�֌W
//***************************************************************

//=======================================
// �^�X�N��

#define GOT_CONTROL					"GOT_CONTROL"			// GOT�V�~�����[�^�Ǘ�
#define GOT_CAMERA					"GOT_CAMERA"			// �[���J�����R���g���[��
#define GOT_LIGHTS					"GOT_LIGHTS"			// �[�����^���n���C�h�������u



//=======================================
// �N���v��
#define FACT_GOT_CONTROL_01			10						// �J�n��~�w�� (fl.data[0]= 0:��~ 1:�J�n)	[�� GOT_CAMERA]

//=======================================
// �g���o�X�V���[�V���X���t���O
#define SEQ_PLG												// �J�E���^�[�{�[�h���X���̑Ή��ł́A�錾�L��
#define SEQ_PIO												// PIO�{�[�h���X���̑Ή��ł́A�錾�L��
