// KT_LEDMGR.h : PROJECT_NAME �A�v���P�[�V�����̃��C�� �w�b�_�[ �t�@�C���ł��B
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

// CKT_LEDMGRApp:
// ���̃N���X�̎����ɂ��ẮAKT_LEDMGR.cpp ���Q�Ƃ��Ă��������B
//

class CKT_LEDMGRApp : public CWinApp
{
public:
	CKT_LEDMGRApp();

// �I�[�o�[���C�h
	public:
	virtual BOOL InitInstance();

// ����

	DECLARE_MESSAGE_MAP()
};

extern CKT_LEDMGRApp theApp;