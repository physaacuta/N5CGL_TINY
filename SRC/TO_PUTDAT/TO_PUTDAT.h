
// TO_PUTDAT.h : PROJECT_NAME �A�v���P�[�V�����̃��C�� �w�b�_�[ �t�@�C���ł��B
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


// CTO_PUTDAT:
// ���̃N���X�̎����ɂ��ẮATO_PUTDAT.cpp ���Q�Ƃ��Ă��������B
//

class CTO_PUTDAT : public CWinApp
{
public:
	CTO_PUTDAT();

// �I�[�o�[���C�h
public:
	virtual BOOL InitInstance();

// ����

	DECLARE_MESSAGE_MAP()
};

extern CTO_PUTDAT theApp;