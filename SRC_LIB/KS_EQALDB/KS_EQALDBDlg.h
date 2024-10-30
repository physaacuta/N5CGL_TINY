// KS_EQALDBDlg.h : �w�b�_�[ �t�@�C��
//

#pragma once
#include "afxwin.h"


// CKS_EQALDBDlg �_�C�A���O
class CKS_EQALDBDlg : public CDialog
{
// �R���X�g���N�V����
public:
	CKS_EQALDBDlg(CWnd* pParent = NULL);	// �W���R���X�g���N�^

// �_�C�A���O �f�[�^
	enum { IDD = IDD_KS_EQALDB_DIALOG };

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
	afx_msg void OnBnClickedOk();
	afx_msg void OnLbnDblclkList1();
	afx_msg void OnClose();
	CListBox m_ListBox;

// �蓮�ǉ� -------------------->>>
private:
	// �����֐�
	void OnCancel() {};											// Esc�L�[�΍�
	static UINT WINAPI MainThread(LPVOID param);				// ���C���X���b�h
	void InitProcess();											// �e�v���Z�X����������
	void ExitProcess();											// �e�v���Z�X�I������

	// �e���[�J�[�X���b�h
	LogFileManager*			mcls_pLog;							// LogFileManager�X���b�h�C���X�^���X
	MainInctance			mcls_MI;							// ���C���C���X�^���X����N���X
// <<<-----------------------------

public:
	afx_msg void OnBnClickedMoveDb();
	afx_msg void OnBnClickedSyncroAb();
	afx_msg void OnBnClickedSyncroBa();
	afx_msg void OnBnClickedNowexecAb();
	afx_msg void OnBnClickedNowexecBa();
	CComboBox m_cmbSession;
	afx_msg void OnTimer(UINT_PTR nIDEvent);
};
