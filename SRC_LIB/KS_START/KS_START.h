// KS_START.h : PROJECT_NAME �A�v���P�[�V�����̃��C�� �w�b�_�[ �t�@�C���ł��B
//

#pragma once

#ifndef __AFXWIN_H__
	#error "PCH �ɑ΂��Ă��̃t�@�C�����C���N���[�h����O�� 'stdafx.h' ���C���N���[�h���Ă�������"
#endif

#include "resource.h"		// ���C�� �V���{��


// CKS_STARTApp:
// ���̃N���X�̎����ɂ��ẮAKS_START.cpp ���Q�Ƃ��Ă��������B
//

class CKS_STARTApp : public CWinApp
{
public:
	CKS_STARTApp();

// �I�[�o�[���C�h
	public:
	virtual BOOL InitInstance();

// ����

	DECLARE_MESSAGE_MAP()
};

extern CKS_STARTApp theApp;