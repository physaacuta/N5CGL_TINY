// KS_STARTDlg.h : �w�b�_�[ �t�@�C��
//

#pragma once
#include "afxwin.h"

// �C���N���[�h
#include <process.h>

// �r�� �C���N���[�h
#include "..\..\include\KizuCommon.h"									// �r�����ʃw�b�_�[
//#include "..\..\Library\KizuLib\KizuLib.h"								// �r�����C�u�����w�b�_�[


// �r���N���X �C���N���[�h
#include "..\..\Library\KizuLib\CLASS\KizuMacro.h"						// �r���}�N��
#include "..\..\Library\KizuLibMFC\CLASS\LogFileManager.h"				// ���O�}�l�[�W���[

// ���[�J���N���X
#include "MainExec.h"


using namespace KizuLib;

// CKS_STARTDlg �_�C�A���O
class CKS_STARTDlg : public CDialog
{
// �R���X�g���N�V����
public:
	CKS_STARTDlg(CWnd* pParent = NULL);	// �W���R���X�g���N�^

// �_�C�A���O �f�[�^
	enum { IDD = IDD_KS_START_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV �T�|�[�g


// ����
protected:
	HICON m_hIcon;

	// �������ꂽ�A���b�Z�[�W���蓖�Ċ֐�
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()

public:
	afx_msg void OnClose();
public:
	afx_msg void OnBnClickedOk();

// �蓮�ǉ� -------------------->>>
private:
	// �����֐�
	void OnCancel() {};											// Esc�L�[�΍�
	static UINT WINAPI MainThread(LPVOID param);				// ���C���X���b�h

	// �e���[�J�[�X���b�h
	LogFileManager* mcls_pLog;									// LogFileManager�X���b�h�C���X�^���X

	// �����o�[�ϐ�
	bool m_bCancel;												// �����L�����Z���� true

	// �ǉ�
	bool CheckMultiExec(LPCSTR SendSlotName);					// �풓�^�X�N �������s�`�F�b�N
// <<<-----------------------------
public:
	CListBox m_ListBox;
	afx_msg void OnLbnDblclkList1();
};
