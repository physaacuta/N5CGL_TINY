// DummyLEDDlg.h : �w�b�_�[ �t�@�C��
//

#pragma once
#include "afxwin.h"


// CDummyLEDDlg �_�C�A���O
class CDummyLEDDlg : public CDialog
{
// �R���X�g���N�V����
public:
	CDummyLEDDlg(CWnd* pParent = NULL);	// �W���R���X�g���N�^

// �_�C�A���O �f�[�^
	enum { IDD = IDD_DUMMYLED_DIALOG };

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

// �蓮�ǉ� -------------------->>>
	// VS2005��VS2008���L�Ƃ����ꍇ�́A�\���v���p�e�B/�����J/�o�̓t�@�C�� ���ȉ��ɕύX
	// $(OutDir)\$(ProjectName).exe  �� $(OutDir)\XX_CPPSMPEX.exe
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
	afx_msg void OnClose();
	afx_msg void OnBnClickedOk();
	afx_msg void OnLbnDblclkList1();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	CListBox m_ListBox;
	CComboBox m_cmbSession;
	CComboBox m_cmbCh;
	CButton m_btnDebug;
	afx_msg void OnBnClickedDebugBreak();
	afx_msg void OnBnClickedBtnTemp();
	CComboBox debug_ch;
	CComboBox debug_uni;
	CEdit debug_tmp;
	afx_msg void OnBnClickedBtnBreak();
	CButton StatOnOff;
	afx_msg void OnBnClickedStateOnoff();
	afx_msg void OnCbnSelchangeCombo4();
	afx_msg void OnCbnSelchangeCombo5();
};
