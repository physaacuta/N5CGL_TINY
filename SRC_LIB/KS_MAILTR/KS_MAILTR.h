// KS_MAILTR.h : PROJECT_NAME �A�v���P�[�V�����̃��C�� �w�b�_�[ �t�@�C���ł��B
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

// CKS_MAILTRApp:
// ���̃N���X�̎����ɂ��ẮAKS_MAILTR.cpp ���Q�Ƃ��Ă��������B
//

class CKS_MAILTRApp : public CWinApp
{
public:
	CKS_MAILTRApp();

// �I�[�o�[���C�h
	public:
	virtual BOOL InitInstance();

// ����

	DECLARE_MESSAGE_MAP()
};

extern CKS_MAILTRApp theApp;