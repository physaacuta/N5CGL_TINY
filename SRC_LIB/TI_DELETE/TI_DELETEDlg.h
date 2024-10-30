// TI_DELETEDlg.h : �w�b�_�[ �t�@�C��
//

#pragma once
#include "afxwin.h"


// CTI_DELETEDlg �_�C�A���O
class CTI_DELETEDlg : public CDialog
{
// �R���X�g���N�V����
public:
	CTI_DELETEDlg(CWnd* pParent = NULL);	// �W���R���X�g���N�^

// �_�C�A���O �f�[�^
	enum { IDD = IDD_TI_DELETE_DIALOG };

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
	afx_msg void OnLbnDblclkList1();
	afx_msg void OnClose();
	afx_msg void OnBnClickedOk();
	afx_msg void OnTimer(UINT_PTR nIDEvent);



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


	CListBox m_ListBox;
public:
	CComboBox m_cmbSession;
};
