// TI_RPISAVEDlg.h : �w�b�_�[ �t�@�C��
//

#pragma once
#include "afxwin.h"

#include "..\..\Library\KizuLibMFC\CLASS\DspListView.h"				// ���X�g�r���[����N���X
#include "afxcmn.h"


// CTI_RPISAVEDlg �_�C�A���O
class CTI_RPISAVEDlg : public CDialog
{
// �R���X�g���N�V����
public:
	CTI_RPISAVEDlg(CWnd* pParent = NULL);	// �W���R���X�g���N�^

// �_�C�A���O �f�[�^
	enum { IDD = IDD_TI_RPISAVE_DIALOG };

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
//	afx_msg void OnLbnSelchangeList1();
	afx_msg void OnBnClickedOk();
	afx_msg void OnClose();
	afx_msg void OnTimer(UINT_PTR nIDEvent);


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


	DspListView*			mcnt_pQue;							// ���X�g�r���[ �L���[���
	DspListView*			mcnt_pExe;							// ���X�g�r���[ ��������
	DspListView*			mcnt_pAll;							// ���X�g�r���[ Ins���
	DspListView*			mcnt_pRpi;							// ���X�g�r���[ Rpi���
	DspListView*			mcnt_pSts;							// ���X�g�r���[ Sts���

// <<<-----------------------------

public:
	CListCtrl m_ViewAll;
	CListCtrl m_ViewExec;
	CListCtrl m_ViewQue;
	CListCtrl m_ViewSts;
	afx_msg void OnLbnDblclkList1();
	afx_msg void OnBnClickedDebugStart();
	afx_msg void OnBnClickedDebugStop();
	afx_msg void OnBnClickedDspwindow1();
	CListCtrl m_ViewRpi;
	afx_msg void OnBnClickedDebugResync();
};
