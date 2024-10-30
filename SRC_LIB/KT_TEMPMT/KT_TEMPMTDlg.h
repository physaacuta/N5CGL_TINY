// KT_TEMPMTDlg.h : �w�b�_�[ �t�@�C��
//

#pragma once
#include "afxwin.h"

#include "..\..\Library\KizuLibMFC\CLASS\DspListView.h"				// ���X�g�r���[����N���X
#include "afxcmn.h"

// CKT_TEMPMTDlg �_�C�A���O
class CKT_TEMPMTDlg : public CDialog
{
// �R���X�g���N�V����
public:
	CKT_TEMPMTDlg(CWnd* pParent = NULL);	// �W���R���X�g���N�^

// �_�C�A���O �f�[�^
	enum { IDD = IDD_KT_TEMPMT_DIALOG };

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
	afx_msg void OnLbnDblclkList1();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnClose();
	afx_msg void OnBnClickedOk();
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

	DspListView*			mcnt_pListVal;						// ���X�g�r���[�N���X
// <<<-----------------------------

public:
	CComboBox m_cmbSession;
	CListCtrl m_ListVal;
};