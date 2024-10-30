// TO_SEQDMDlg.h : �w�b�_�[ �t�@�C��
//

#pragma once
#include "afxwin.h"
#include "actqj71e71udp1.h"


// CTO_SEQDMDlg �_�C�A���O
class CTO_SEQDMDlg : public CDialog
{
// �R���X�g���N�V����
public:
	CTO_SEQDMDlg(CWnd* pParent = NULL);	// �W���R���X�g���N�^

// �_�C�A���O �f�[�^
	enum { IDD = IDD_TO_SEQDM_DIALOG };

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
	afx_msg void OnClose();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnLbnDblclkList1();

// �蓮�ǉ� -------------------->>>
private:
	//// �萔
	static const int MX_OPEN_RETRY_NID_EVENT	= 10;					// MX�R���|�[�l���g�I�[�v���v�� �^�C�}�[ID
	static const int MX_OPEN_RETRY_TIMER		= 2000;					// �^�C�}�[���� [ms]

private:
	// �����֐�
	void OnCancel() {};											// Esc�L�[�΍�
	static UINT WINAPI MainThread(LPVOID param);				// ���C���X���b�h
	void InitProcess();											// �e�v���Z�X����������
	void ExitProcess();											// �e�v���Z�X�I������


	//// MX�R���|�[�l�g ����
	bool MX_Connect(bool bLogDsp);								// �ڑ�
	void MX_Close();											// �ؒf & ���g���C�ݒ�
	UINT					m_tidMxConnect;						// MX�R���|�[�l�g�����ڑ��^�C�}�[


	// �e���[�J�[�X���b�h
	LogFileManager*			mcls_pLog;							// LogFileManager�X���b�h�C���X�^���X
	MainInctance			mcls_MI;							// ���C���C���X�^���X����N���X
// <<<-----------------------------
public:
	CActqj71e71udp1 m_cCom;
	afx_msg void OnBnClickedOk();
};
