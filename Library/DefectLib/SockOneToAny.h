#pragma once
// ���Œ�
#include "..\..\Include\KizuCommon.h"									// �r���C���N���[�h


// 20150316 �ǉ� DefectLib/SockOneToAnySend
struct SOCK_F_HEAD2 {
	SOCK_BASE_HEAD	base_head;								// ���ʃw�b�_��

	// ------>> DefectLib/SockOneToAnySend �ɂĎg�p
	// �� SOCK_BASE_HEAD�ł́ASOCK_F_HEAD����A���f�[�^�{addr�̗����ɑΉ��o���Ȃ��΍�

	struct SOCK_BASE_HEAD_ADDR {
		// SOCK_BASE_HEAD/len �́A�S�Ẵf�[�^�T�C�Y�Blen = len_head+len_add[0]+len_add[1]
		long			len_head;				// base_head����̘A���f�[�^
		long			len_addr[2];			// addr�����̃f�[�^��
	} base_head_addr;

	// <<------

	char			reserve[SIZE_SOCK_HEAD-sizeof(SOCK_BASE_HEAD)-sizeof(SOCK_BASE_HEAD_ADDR)];	// �ePC�ԒʐM�ŉσw�b�_�[��
};
