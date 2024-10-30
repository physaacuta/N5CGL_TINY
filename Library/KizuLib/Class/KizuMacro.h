// *********************************************************************************
//	�}�N���W
//	[Ver]
//		Ver.01    2007/01/30  vs2005 �Ή�
//
//	[����]
//
// *********************************************************************************

//#ifndef KIZULIB_KIZUMACRO_H
//#define KIZULIB_KIZUMACRO_H

#pragma once

//#pragma warning(once : 4005)								// �}�N���̍Ē�` (SAFE_DELETE �Ƃ����Afc�ł���`����Ă��邽�߂̑΍�) disable �ŏo�Ȃ�����͍̂��邩��A��񂾂��o�͂��Ă���

namespace KizuLib
{
	//// �L�[���[�h
	// interface�L�[���[�h�̒�`
	// (VisualC��BASETYPS.H�ɓ��l�̒�`����)
	#ifndef interface
		#define interface struct
	#endif
	// implements�L�[���[�h�̒�`
	#ifndef implements
		#define implements public
	#endif


	//// �}�N��
	// FC�ɂ���`����Ă���΍�	
	#ifndef SAFE_DELETE
		#define SAFE_DELETE(x)			if (NULL != x) { delete x; x = NULL; }											// New�J��
	#endif
	#ifndef SAFE_DELETE_ARRAY
		#define SAFE_DELETE_ARRAY(x)	if (NULL != x) { delete [] x; x = NULL; }										// New�z��J��
	#endif
	#ifndef SAFE_DELETE_HANDLE
		#define SAFE_DELETE_HANDLE(x)	if (INVALID_HANDLE_VALUE != x) { CloseHandle(x); x = INVALID_HANDLE_VALUE; }	// �n���h���J��
	#endif

	#define SAFE_TIMER_SET(t,id,interval)	if(0==t) { t=SetTimer(NULL, id, interval, NULL); }						// �^�C�}�[�N��
	#define SAFE_TIMER_KILL(t)				if(0!=t) { KillTimer(NULL, t); t = 0; }									// �^�C�}�[�I��


	//=======================================
	// ���O��`
	// �g�p����ꍇ�́A
	//   1:mcls_pLog �������o�[�ϐ��ɒǉ����Ă��鎖
	//   2:�R���X�g���N�^��NULL�������ƃZ�b�g���鎖
	// �g�p��:  LOG(em_MSG), "test_%d_%d", wk, wk );
	//=======================================
	#define LOG(t)  if(mcls_pLog!=NULL)       mcls_pLog->Write(KizuLib::LogFileManager::t							// ���O�o��
	#define pLOG(t) if(p->mcls_pLog!=NULL) p->mcls_pLog->Write(KizuLib::LogFileManager::t							// ���O�o��(�|�C���^��)


	//=======================================
	// ���[�j���O�΍�
	//  ���̃��[�j���O���A�����Ƃ����������������
	//		warning C4805: '|' : ���Z���� 'bool' �^�� 'const int' �^�̍��p�͈��S�ł͂���܂���
	//		warning C4800: 'const int' : �u�[���l�� 'true' �܂��� 'false' �ɋ����I�ɐݒ肵�܂� (�x���̏���)
	//		bool b = i;		// C4800
	//		if (i == b)		// C4805
	//	�g�p���@:  bool b = __BOOL i;	
	//
	//=======================================
	#define __BOOL	!!		// int�^��2�d�ے��bool�^�ɂ���


	//=======================================
	// �������
	//=======================================
	#define GET_JUST_SIZE(x, l)		((((x-1)/l)+1)*l)				// x�T�C�Y��l�̔{���ɑ�����(�K���Ax�̂܂܂��A������Ƒ傫���Ȃ����)
	#define GET_JUST_INDEX(x, l)	((x-1)/l)						// ����؂菈�� ( 5, 5 �̏ꍇ�́A0�ƂȂ�) (���̌��ʂ��g���Ĕz����`����Ƃ���+1���邱��)
	#define BYTE_CAST(x) ((x) > 255 ? 255 : (x) < 0 ? 0 : (x))		// BYTE�^�փL���X�g

};

//#endif