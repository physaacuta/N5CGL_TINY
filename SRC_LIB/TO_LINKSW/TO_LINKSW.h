// TO_LINKSW.h : PROJECT_NAME �A�v���P�[�V�����̃��C�� �w�b�_�[ �t�@�C���ł��B
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

// CTO_LINKSWApp:
// ���̃N���X�̎����ɂ��ẮATO_LINKSW.cpp ���Q�Ƃ��Ă��������B
//

class CTO_LINKSWApp : public CWinApp
{
public:
	CTO_LINKSWApp();

// �I�[�o�[���C�h
	public:
	virtual BOOL InitInstance();

// ����

	DECLARE_MESSAGE_MAP()
};

extern CTO_LINKSWApp theApp;