// *********************************************************************************
//	���^�X�N�̊�{�萔
//	[Ver]
//		Ver.01    2007/08/01  vs2005 �Ή�
//
//	[����]
//		
// *********************************************************************************

#pragma once

#include "..\..\Include\LineCommon.h"									// �r�����ʃw�b�_�[


//=======================================
// ���b�Z�[�W�ʒm
//=======================================
//// �S��MainInctance�Ŏ󂯂�
//       �ꍇ�ɂ���ẮAMainInctance���e���[�J�[�X���b�h�ɂ��ꗬ������
enum ENUM_MAININCTANCE {
	E_DEF_ML_COIL = WM_USER + 100,									// �R�C������M�ʒm
	E_DEF_ML_COIL_REV,												// �\�����]���ăR�C�����Ď�M

	E_DEF_END														// �I��
};

//=======================================
// �������\�b�h
//=======================================


//------------------------------------------
// ����������ւ�
// char*     str1 �����񂻂�1
// char*	 str2 �����񂻂�2
// const int size ������̃T�C�Y(256�܂�)�f�t�H���g�ŁA������̒Z�����̕�����
//------------------------------------------
static void ChangeString(char* str1, char* str2, const int size=-1)
{
	char cWk[256];
	memset(cWk, 0x00, 256);

	int  nSize = size;
	if( -1 == nSize )	nSize = (int)min(strlen(str1), strlen(str2));


	memcpy(cWk,  str1, nSize);	// str1 �� wk   ��
	memcpy(str1, str2, nSize);	// str2 �� str1 ��
	memcpy(str2, cWk,  nSize);	// wk   �� str2 ��

}