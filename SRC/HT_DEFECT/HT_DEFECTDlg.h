// HT_DEFECTDlg.h : �w�b�_�[ �t�@�C��
//

#pragma once
#include "afxwin.h"

// �e���[�J�[�X���b�h
#include "MainConst.h"													// ���N���X�萔�w�b�_�[
#include "..\..\Library\KizuLibMFC\CLASS\DspListView.h"					// ���X�g�r���[����N���X
#include "afxcmn.h"



// CHT_DEFECTDlg �_�C�A���O
class CHT_DEFECTDlg : public CDialog
{
// �R���X�g���N�V����
public:
	CHT_DEFECTDlg(CWnd* pParent = NULL);	// �W���R���X�g���N�^

// �_�C�A���O �f�[�^
	enum { IDD = IDD_HT_DEFECT_DIALOG };

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
	afx_msg void OnBnClickedOk();
	afx_msg void OnClose();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnLbnDblclkList1();


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
	LogFileManager*			mcls_pLogCull;						// LogFileManager�X���b�h�C���X�^���X
	MainInctance			mcls_MI;							// ���C���C���X�^���X����N���X


	// ���X�g�r���[
	DspListView*			mcnt_pListSt;						// ��ԃ��X�g�r���[�N���X
	DspListView*			mcnt_pListTo;						// �Γ�����ԃ��X�g�r���[�N���X

																
// <<<-----------------------------
public:
	afx_msg void OnBnClickedDebugStart();
	afx_msg void OnBnClickedDebugStop();
	afx_msg void OnBnClickedDebugParam();
	afx_msg void OnBnClickedDebugCoilinf();
	afx_msg void OnBnClickedDebugCanok();
//	afx_msg void OnBnClickedDebugFrame();
//	afx_msg void OnBnClickedDebugProfile();
//	afx_msg void OnBnClickedDebugDlycancel();
	afx_msg void OnBnClickedDebugImgkind();
	CComboBox m_cmbMen;
	CListCtrl m_ListSt;
	CListCtrl m_ListTo;
	afx_msg void OnBnClickedDebugParamPreload();
	afx_msg void OnBnClickedDebugSetPosition();
	afx_msg void OnBnClickedDebugResumeStart();
	afx_msg void OnBnClickedDebugResumeStop();
	afx_msg void OnBnClickedDebugKind1();
	afx_msg void OnBnClickedDebugKind2();
	afx_msg void OnBnClickedDebugKind3();
};
