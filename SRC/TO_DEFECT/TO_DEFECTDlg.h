
// TO_DEFECTDlg.h : �w�b�_�[ �t�@�C��
//

#pragma once
#include "afxwin.h"

#include "..\..\Library\KizuLibMFC\CLASS\DspListView.h"				// ���X�g�r���[����N���X
#include "SListBox.h"
#include "afxcmn.h"


// CTO_DEFECTDlg �_�C�A���O
class CTO_DEFECTDlg : public CDialogEx
{
// �R���X�g���N�V����
public:
	CTO_DEFECTDlg(CWnd* pParent = NULL);	// �W���R���X�g���N�^�[

// �_�C�A���O �f�[�^
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_TO_DEFECT_DIALOG };
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
public:
	afx_msg void OnClose();
	afx_msg void OnBnClickedOk();
	CListBox m_ListBox;
//	afx_msg void OnLbnDblclkList1();
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

	// ���̑��֐�
	void	DummyCoilCreate(int nSendPoint);					// �_�~�[�R�C������(nSendPoint:���M�ʒu)

	// �e���[�J�[�X���b�h
	LogFileManager*			mcls_pLog;							// LogFileManager�X���b�h�C���X�^���X
	LogFileManager*			mcls_pLogSeq;						// �V�[�P���TUDP����M���O�p�N���X
	LogFileManager*			mcls_pLogD;							// �������O�p�N���X
	LogFileManager*			mcls_pLogG;							// �K�C�_���X���O�p�N���X
	LogFileManager*			mcls_pLogE;							// �G�b�WUDP���O�p�N���X
	LogFileManager*			mcls_pLogA;							// �x�񃍃O�p�N���X
	MainInstance			mcls_MI;							// ���C���C���X�^���X����N���X

	DspListView*			mcnt_pListSt;						// �X�e�[�^�X���
	DspListView*			mcnt_pListSys;						// �V�X�e�����
	DspListView*			mcnt_pListHt;						// ���茋�ʕ\�����X�g�r���[�N���X(�P��)
	DspListView*			mcnt_pListHtTime;					// ���菈�����ԃ��X�g�r���[�N���X
	DspListView*			mcnt_pListOp;						// �\�����
	DspListView*			mcnt_pListPr;						// ���茋�ʕ\�����X�g�r���[�N���X(�p������)

	DspListView*			mcnt_pListSetuDsp;					// �ݔ��ʒu�g���b�L���O��񃊃X�g�r���[�N���X
	DspListView*			mcnt_pListEdge;						// �G�b�W���
	DspListView*			mcnt_pListKiki;						// �@����
	DspListView*			mcnt_pListRamp;						// �r�ڋ߃����v���
	DspListView*			mcnt_pListPato;						// �p�g���C�g���
	DspListView*			mcnt_pListVoice;					// �{�C�X���
	DspListView*			mcnt_pListDmem;						// �f�[�^������

	DspListView*			mcnt_pListCoil;						// �R�C�����\�����X�g�r���[�N���X
	DspListView*			mcnt_pListPlgKen;					// PLG ������_��񃊃X�g�r���[�N���X
	DspListView*			mcnt_pListPlgDsp;					// PLG �\����_��񃊃X�g�r���[�N���X
	DspListView*			mcnt_pListPlg;						// PLG ��{��񃊃X�g�r���[�N���X

	DspListView*			mcnt_pListDb;						// DB���ʕ\�����X�g�r���[�N���X
	DspListView*			mcnt_pListDbInfo;					// DB��񃊃X�g�r���[�N���X

	DspListView*			mcnt_pListCmd;						// �R�}���h
	SListBox				mlst_Cmd;							// �R�}���h

	DspListView*			mcnt_pListInput;					// �f�o�b�N����
	SListBox				mlst_Input;							// �f�o�b�N����

	// �f�o�b�O�p
	int						m_nCoil;							// �R�C��No�_�~�[�J�E���g(0�`99)
	bool					m_bInit;							// �������t���O�i�N���`����^�C�}�C�x���g�܂Łj

// <<<-----------------------------
public:
	afx_msg void OnLbnDblclkList1();
	afx_msg void OnNMClickListCmd(NMHDR *pNMHDR, LRESULT *pResult);
	CListCtrl m_ListSt;
	CListCtrl m_ListSys;
	CListCtrl m_ListHt;
	CListCtrl m_ListHtTime;
	CListCtrl m_ListCoil;
	CListCtrl m_ListPlgKen;
	CListCtrl m_ListPlgDsp;
	CListCtrl m_ListPlg;
	CListCtrl m_ListDb;
	CListCtrl m_ListDbInfo;
	CListCtrl m_ListOp;
	CListCtrl m_ListEdge;
	CListCtrl m_ListKiki;
	CListCtrl m_ListRamp;
	CListCtrl m_ListPato;
	CListCtrl m_ListVoice;
	CListCtrl m_ListSetuDsp;
	afx_msg void OnBnClickedSmyuPio();
	afx_msg void OnBnClickedSmyuPlg();
	afx_msg void OnBnClickedDebugLedonoff();
	afx_msg void OnNMClickListInput(NMHDR *pNMHDR, LRESULT *pResult);
	CListCtrl m_ListPr;
	int m_nParaKind;
	int m_nCamsetResult;
};
