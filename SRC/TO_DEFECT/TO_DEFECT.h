
// TO_DEFECT.h : PROJECT_NAME �A�v���P�[�V�����̃��C�� �w�b�_�[ �t�@�C���ł��B
//

#pragma once

#ifndef __AFXWIN_H__
	#error "PCH �ɑ΂��Ă��̃t�@�C�����C���N���[�h����O�� 'stdafx.h' ���C���N���[�h���Ă�������"
#endif

#include "resource.h"		// ���C�� �V���{��

// �蓮�ǉ� -------------------->>>
// �w�b�_�[
#include "MainInstance.h"					// ���C���C���X�^���X����
// <<<-----------------------------

// CTO_DEFECTApp:
// ���̃N���X�̎����ɂ��ẮATO_DEFECT.cpp ���Q�Ƃ��Ă��������B
//

class CTO_DEFECTApp : public CWinApp
{
public:
	CTO_DEFECTApp();

// �I�[�o�[���C�h
public:
	virtual BOOL InitInstance();

// ����

	DECLARE_MESSAGE_MAP()
};

extern CTO_DEFECTApp theApp;