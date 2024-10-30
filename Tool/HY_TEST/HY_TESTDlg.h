
// HY_TESTDlg.h : �w�b�_�[ �t�@�C��
//

#pragma once
#include "afxwin.h"

#include "..\..\Library\KizuLibMFC\CLASS\DspListView.h"				// ���X�g�r���[����N���X
#include "afxcmn.h"


// CHY_TESTDlg �_�C�A���O
class CHY_TESTDlg : public CDialog
{
// �R���X�g���N�V����
public:
	CHY_TESTDlg(CWnd* pParent = NULL);	// �W���R���X�g���N�^�[

// �_�C�A���O �f�[�^
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_HY_TEST_DIALOG };
#endif

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
	//void	SetDlgItemFloat(int nID, float fData);
	//float	GetDlgItemFloat(int nID);
	CString GetName_SpmUmu(EM_DIV_SPM em);

	// �e���[�J�[�X���b�h
	LogFileManager*			mcls_pLog;							// LogFileManager�X���b�h�C���X�^���X
	MainInctance			mcls_MI;							// ���C���C���X�^���X����N���X
	// <<<-----------------------------
																// ���X�g�r���[�N���X
	DspListView*			mcnt_pListCoil;						// �R�C�����
	DspListView*			mcnt_pListDsp;						// ��������

	enum LIST_COIL_COL {
		EM_COIL_LEN = 0,										// �����ʒu
		EM_COIL_KANRINO,										// �Ǘ�No
		EM_COIL_COILNO,											// �R�C��No
		EM_COIL_SPM_UMU,										// SPM�����L��
		LIST_COIL_COLMAX
	};

	enum LIST_DSP_COL {
		EM_DSP_NO = 0,											// No
		EM_DSP_NAME,											// �����䖼
		EM_DSP_KANRINO,											// �Ǘ�No
		EM_DSP_DISTANCE,										// �ʔʒu
		LIST_DSP_COLMAX
	};

	int	m_rdoKadou;
	int	m_rdoUnten;
	int	m_rdoKiki_o;
	int	m_rdoKensa_o;
	int	m_rdoKiki_u;
	int	m_rdoKensa_u;

public:
	afx_msg void OnBnClickedOk();
	afx_msg void OnClose();
	CListBox m_ListBox;
	afx_msg void OnLbnDblclkList1();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnBnClickedBtnStatus();
	afx_msg void OnBnClickedBtnInit();
	afx_msg void OnBnClickedBtnParam();
	afx_msg void OnBnClickedBtnAddcoil();
	afx_msg void OnBnClickedBtnCut();
	afx_msg void OnBnClickedBtnSpeed();
	afx_msg void OnBnClickedChkPlg();
	afx_msg void OnBnClickedBtnSlow();
	afx_msg void OnBnClickedBtnCoilLen();
	afx_msg void OnBnClickedBtnCoilWid();
	CListCtrl m_ListCoil;
	CListCtrl m_ListDsp;
	afx_msg void OnBnClickedChkDef();
	afx_msg void OnBnClickedBtnDefRate();
	afx_msg void OnBnClickedBtnSetcode();
	afx_msg void OnBnClickedBtnDefSend();
	afx_msg void OnBnClickedChkPos();
	afx_msg void OnBnClickedCheckMae();
	afx_msg void OnBnClickedCheckYousetuCut();
	afx_msg void OnBnClickedBtnLstCancelSend();
	afx_msg void OnBnClickedCheckOutDef();
	afx_msg void OnBnClickedBtnAlarmSend();
	afx_msg void OnBnClickedBtnDefcntSend();
	CButton m_chkDsWs;
};
