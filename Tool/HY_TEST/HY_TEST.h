
// HY_TEST.h : PROJECT_NAME �A�v���P�[�V�����̃��C�� �w�b�_�[ �t�@�C���ł��B
//

#pragma once

#ifndef __AFXWIN_H__
	#error "PCH �ɑ΂��Ă��̃t�@�C�����C���N���[�h����O�� 'stdafx.h' ���C���N���[�h���Ă�������"
#endif

#include "resource.h"		// ���C�� �V���{��

// �蓮�ǉ� -------------------->>>
// �w�b�_�[
#include "MainInctance.h"					// ���C���C���X�^���X����
// <<<-----------------------------

// CHY_TESTApp:
// ���̃N���X�̎����ɂ��ẮAHY_TEST.cpp ���Q�Ƃ��Ă��������B
//

class CHY_TESTApp : public CWinApp
{
public:
	CHY_TESTApp();

// �I�[�o�[���C�h
public:
	virtual BOOL InitInstance();

// ����

	DECLARE_MESSAGE_MAP()
};

extern CHY_TESTApp theApp;