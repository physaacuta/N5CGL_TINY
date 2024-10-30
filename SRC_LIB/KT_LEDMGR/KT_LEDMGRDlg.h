// KT_LEDMGRDlg.h : �w�b�_�[ �t�@�C��
//

#pragma once
#include "afxwin.h"
#include "afxcmn.h"

#include "..\..\Library\KizuLibMFC\CLASS\DspListView.h"				// ���X�g�r���[����N���X


// CKT_LEDMGRDlg �_�C�A���O
class CKT_LEDMGRDlg : public CDialog
{
// �R���X�g���N�V����
public:
	CKT_LEDMGRDlg(CWnd* pParent = NULL);	// �W���R���X�g���N�^

// �_�C�A���O �f�[�^
	enum { IDD = IDD_KT_LEDMGR_DIALOG };

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
	afx_msg void OnBnClickedOk();
	afx_msg void OnClose();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
//	afx_msg void OnEndSession(BOOL bEnding);

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

	DspListView*			mcnt_pListVal;						// �␳�z�����X�g�r���[�N���X
	DspListView*			mcnt_pListTemp;						// ���x�v���_���X�g�r���[�N���X

	int						m_nColCntVal;						// �␳�z�����X�g�r���[��(�S���u�A�Sch�̒��ōő�̃u���b�N��)
	int						m_nColCntTemp;						// ���x�v���X���X�g�r���[�N���X��(�S���u�A�Sch�̒��ōő�̉��x�v���_��)

// <<<-----------------------------
public:
	CComboBox m_cmbSession;
	afx_msg void OnBnClickedDebagState();
	afx_msg void OnBnClickedDebagTemp();
	afx_msg void OnBnClickedDebagOn();
	afx_msg void OnBnClickedDebagOff();
	afx_msg void OnBnClickedDebagReset();
	afx_msg void OnBnClickedDebagLedScode();
	afx_msg void OnBnClickedDebagLedVal();
	CComboBox m_cmbCh;
	CComboBox m_cmbDebagCh;
	CListCtrl m_ListVal;
	afx_msg void OnBnClickedDebagLedHosei();
	afx_msg void OnBnClickedDebagLedHoseiAll();
	CListCtrl m_lstTemp;
};
