// KS_SLAVEDlg.h : �w�b�_�[ �t�@�C��
//

#pragma once
#include "afxwin.h"


// CKS_SLAVEDlg �_�C�A���O
class CKS_SLAVEDlg : public CDialog
{
// �R���X�g���N�V����
public:
	CKS_SLAVEDlg(CWnd* pParent = NULL);	// �W���R���X�g���N�^

// �_�C�A���O �f�[�^
	enum { IDD = IDD_KS_SLAVE_DIALOG };

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
	CListBox m_ListBox;
public:
	afx_msg void OnLbnDblclkList1();
public:
	afx_msg void OnBnClickedOk();
public:
	afx_msg void OnClose();


// �蓮�ǉ� -------------------->>>
private:
	// �����֐�
	void OnCancel() {};											// Esc�L�[�΍�
	static UINT WINAPI MainThread(LPVOID param);				// ���C���X���b�h
	void InitProcess();											// �e�v���Z�X����������
	void ExitProcess();											// �e�v���Z�X�I������


	// �e���[�J�[�X���b�h
	LogFileManager* mcls_pLog;									// LogFileManager�X���b�h�C���X�^���X
	MainInctance mcls_MI;										// ���C���C���X�^���X����N���X
	int m_nEndStatus;											// �I���X�e�[�^�X (0:�����ʂ� 1:�V���b�g�_�E�� 2:�ċN��)
	HANDLE m_evManualEnd;										// �蓮�̋����I���V�O�i��
// <<<-----------------------------


	afx_msg void OnBnClickedMydown2();
	afx_msg void OnBnClickedMyreboot();
public:
	afx_msg void OnBnClickedMylogoff();
	afx_msg void OnBnClickedMytaskend();
};
