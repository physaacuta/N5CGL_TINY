// KS_EVTLOG.h : PROJECT_NAME �A�v���P�[�V�����̃��C�� �w�b�_�[ �t�@�C���ł��B
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

// CKS_EVTLOGApp:
// ���̃N���X�̎����ɂ��ẮAKS_EVTLOG.cpp ���Q�Ƃ��Ă��������B
//

class CKS_EVTLOGApp : public CWinApp
{
public:
	CKS_EVTLOGApp();

// �I�[�o�[���C�h
	public:
	virtual BOOL InitInstance();

// ����

	DECLARE_MESSAGE_MAP()
};

extern CKS_EVTLOGApp theApp;