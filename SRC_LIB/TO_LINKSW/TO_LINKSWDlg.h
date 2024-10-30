// TO_LINKSWDlg.h : �w�b�_�[ �t�@�C��
//

#pragma once
#include "afxwin.h"
#include "afxcmn.h"

#include "..\..\Library\KizuLibMFC\CLASS\DspListView.h"				// ���X�g�r���[����N���X

// CTO_LINKSWDlg �_�C�A���O
class CTO_LINKSWDlg : public CDialog
{
// �R���X�g���N�V����
public:
	CTO_LINKSWDlg(CWnd* pParent = NULL);	// �W���R���X�g���N�^

// �_�C�A���O �f�[�^
	enum { IDD = IDD_TO_LINKSW_DIALOG };

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
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnBnClickedOk();
	CListBox m_ListBox;
	afx_msg void OnLbnDblclkList1();
// �蓮�ǉ� -------------------->>>
private:
	// �����֐�
	void OnCancel() {};											// Esc�L�[�΍�
	static UINT WINAPI MainThread(LPVOID param);				// ���C���X���b�h
	void InitProcess();											// �e�v���Z�X����������
	void ExitProcess();											// �e�v���Z�X�I������

	void CommandSender(COMMON_QUE* que);						// �R�}���h�����J�n�v��

	// �e���[�J�[�X���b�h
	LogFileManager* mcls_pLog;									// LogFileManager�X���b�h�C���X�^���X
	MainInstance mcls_MI;										// ���C���C���X�^���X����N���X
	DspListView* mcnt_pListBoard;								// �{�[�h�ڑ��󋵃��X�g�r���[�N���X
// <<<-----------------------------
public:

	afx_msg void OnBnClickedDebugParam();
	afx_msg void OnBnClickedDebugStartSet();
	afx_msg void OnBnClickedDebugCameraSet();
	afx_msg void OnBnClickedDebugCcmode();
	afx_msg void OnBnClickedDebugSlaveSet();
	afx_msg void OnBnClickedDebugCounterReset();
	afx_msg void OnBnClickedDebugCounterCheck();
	afx_msg void OnBnClickedDebugSetSerialSpeed();
	CComboBox m_cmbUnitDsp;
	CListCtrl m_ListBoard;
	CComboBox m_cmbUnitExec;
};
