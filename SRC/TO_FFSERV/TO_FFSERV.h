// TO_FFSERV.h : PROJECT_NAME �A�v���P�[�V�����̃��C�� �w�b�_�[ �t�@�C���ł��B
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

// CTO_FFSERVApp:
// ���̃N���X�̎����ɂ��ẮATO_FFSERV.cpp ���Q�Ƃ��Ă��������B
//

class CTO_FFSERVApp : public CWinApp
{
public:
	CTO_FFSERVApp();

// �I�[�o�[���C�h
	public:
	virtual BOOL InitInstance();

// ����

	DECLARE_MESSAGE_MAP()
};

extern CTO_FFSERVApp theApp;