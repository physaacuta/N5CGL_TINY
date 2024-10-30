// KS_MAILTRDlg.h : �w�b�_�[ �t�@�C��
//

#pragma once
#include "afxwin.h"


// CKS_MAILTRDlg �_�C�A���O
class CKS_MAILTRDlg : public CDialog
{
// �R���X�g���N�V����
public:
	CKS_MAILTRDlg(CWnd* pParent = NULL);	// �W���R���X�g���N�^

// �_�C�A���O �f�[�^
	enum { IDD = IDD_KS_MAILTR_DIALOG };

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
	afx_msg void OnBnClickedOk();
	CListBox m_ListBox;
	afx_msg void OnLbnDblclkList1();
	afx_msg void OnTimer(UINT_PTR nIDEvent);

// �蓮�ǉ� -------------------->>>
	// VS2005��VS2008���L�Ƃ����ꍇ�́A�\���v���p�e�B/�����J/�o�̓t�@�C�� ���ȉ��ɕύX
	// $(OutDir)\$(ProjectName).exe  �� $(OutDir)\KS_MAILTR.exe
private:
	// �����֐�
	void	OnCancel() {};										// Esc�L�[�΍�
	static UINT WINAPI	MainThread(LPVOID param);				// ���C���X���b�h
	void	InitProcess();										// �e�v���Z�X����������
	void	ExitProcess();										// �e�v���Z�X�I������

	// �e���[�J�[�X���b�h
	LogFileManager*			mcls_pLog;							// LogFileManager�X���b�h�C���X�^���X
	MainInctance			mcls_MI;							// ���C���C���X�^���X����N���X
// <<<-----------------------------
public:
	afx_msg void OnBnClickedDebugManual();
	afx_msg void OnBnClickedBtnTransFl();
	afx_msg void OnBnClickedBtnTransFc();
	afx_msg void OnBnClickedBtnTransMix();
	afx_msg void OnBnClickedBtnClearR();
};
