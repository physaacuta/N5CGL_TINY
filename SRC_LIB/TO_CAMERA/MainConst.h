// *********************************************************************************
//	���^�X�N�̊�{�萔
//	[Ver]
//		Ver.01    2014/10/11  vs2005 �Ή�
//
//	[����]
//		
// *********************************************************************************

#pragma once

// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// �X�C�b�`


/////// AGC�@�\�̗L��
#define AGCOFF														// AGC�@�\�𖳌��ɂ���ꍇ�A���R�����g
																			// �����ɂ���@�\�́A�ȉ��̒ʂ�
																			// �E�R�C���؂�ւ����́A�����I�����ԃZ�b�g
																			// �E�^�X�N���������́A�I�����ԃZ�b�g
																			// �E���[���X���b�g�Œʒm������Ώ����͂���B
																			// ����ʂ���̋����I�����ԕύX�͗L���Ƃ���B

#define ONLINE_INPUT_C			1										// �I�����C�����͑I��
																			// 1 : �I�����C���P (�g���b�L���O)
																			// 2 : �I�����C���Q (�J����)
#define	VER_AVALDATA													// AVALDATA���o�[�W�����̏ꍇ�A���R�����g
																			// ������ݸSW�̏ꍇ�A�I�����Ԃ̶��No��4�R��΂�CC1�`CC4�܂ł��邽��

#ifndef AGCOFF			// AGC�L�����̂�
  #define	VER_P4_CAMERA												// P4�J�����̏ꍇ�A���R�����g
  #define	VER_SCODE_CHG												// �\�ʏ�Ԃ̕ω����f��L���ɂ���ꍇ�A���R�����g
  //#define VER_IDULE_CAMERA												// iDule ADP4M-B-CLSNT�J�����̏ꍇ�A���R�����g	
#endif

#ifdef VER_AVALDATA		// �ȉ� AVALDATA���̂݁A�I���\
  #define	LINKSW_CAM_SELECT_MODE	1									// �e�J�������J���������N�X�C�b�`�̂ǂ̓���CH�Ɋ���U�邩���[�h
																			// 0:�O�l�߂Őڑ�Ver
																			// 1:�����z�Őڑ�Ver

#endif


// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// �ŗL�萔

#define MAX_THREAD			8											// �ő�X���b�h�Ǘ���
	
#ifndef FACT_TO_CAMERA_09
  #define FACT_TO_CAMERA_09			2209
#endif

//#define IS_TO_CAMERA_TBL



#ifndef	VER_AVALDATA													// �J�����R���g���[���̏ꍇ
#define	NUM_CAMLINKSW			1										// �J���������N�X�C�b�`�䐔��1�Œ�
#endif
