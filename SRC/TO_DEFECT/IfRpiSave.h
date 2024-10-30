// *********************************************************************************
//	�������S���摜�����Ƃ�I/F�N���X
//	[Ver]
//		Ver.01    2011/12/11  vs2005 �Ή�
//
//	[����]
//		
// *********************************************************************************

#pragma once

#include <winsock2.h>													// UDP���M�p

// �r�� �C���N���[�h
#include "..\..\include\LineCommon.h"									// �r�����ʃw�b�_�[
#include "..\..\Library\KizuLib\KizuLib.h"								// �r�����C�u�����w�b�_�[
#include "..\..\Library\KizuLib\Class\AutoLock.h"						// �N���e�B�J���Z�N�V��������N���X

// ���[�J��
#include "MainConst.h"													// ���N���X�萔�w�b�_�[

// �S���摜�ۑ��p
#include "..\..\include\OutIF_HT_RAW.h"									// ���V�X�e�����J�p�w�b�_�[

using namespace KizuLib;




//#define SEND_RPI_MGR		// TO_RPIMGR���M�� �R�����g�A�E�g (�����o�b�N�A�b�v�֑ؑΉ���)



class IfRpiSave
{
	//// ���J�֐�
public:
	IfRpiSave(void);
	virtual ~IfRpiSave(void);

	// ���[���X���b�g�֌W
	static void Send_TiRpiSave_Start();									// �S���摜���� �� �����J�n�v��
	static void Send_TiRpiSave_Stop();									// �S���摜���� �� ������~�v��
	static void Send_TiRpiSave_CoilSend(char const* cKizukenNo, COIL_SETTING const* setting, COIL_BASEDATA const * base, int framenum, int front_pos, int nMen);		// �S���摜���� �� ���R�C����� [��CoilManager]


};
