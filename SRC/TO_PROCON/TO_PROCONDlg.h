// TO_PROCONDlg.h : �w�b�_�[ �t�@�C��
//

#pragma once
#include "afxwin.h"


// CTO_PROCONDlg �_�C�A���O
class CTO_PROCONDlg : public CDialog
{
// �R���X�g���N�V����
public:
	CTO_PROCONDlg(CWnd* pParent = NULL);	// �W���R���X�g���N�^

// �_�C�A���O �f�[�^
	enum { IDD = IDD_TO_PROCON_DIALOG };

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
	afx_msg void OnClose();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnLbnDblclkList1();
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
//	afx_msg void OnEnChangeStatePortRecv();
	afx_msg void OnBnClickedDump1();
	afx_msg void OnBnClickedDump2();
	afx_msg void OnBnClickedChkDebugFunc();
	afx_msg void OnBnClickedChkLogAlldump();
	afx_msg void OnBnClickedBtnTcpClose();
	afx_msg void OnBnClickedBtnDbgSend();
	afx_msg void OnBnClickedBtnDbgRecv();
};
