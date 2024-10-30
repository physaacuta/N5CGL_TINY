
// HY_DEFECTDlg.h : �w�b�_�[ �t�@�C��
//

#pragma once
#include "afxwin.h"
#include "afxcmn.h"

#include "..\..\Library\KizuLibMFC\CLASS\DspListView.h"				// ���X�g�r���[����N���X


// CHY_DEFECTDlg �_�C�A���O
class CHY_DEFECTDlg : public CDialog
{
// �R���X�g���N�V����
public:
	CHY_DEFECTDlg(CWnd* pParent = NULL);	// �W���R���X�g���N�^�[

// �_�C�A���O �f�[�^
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_HY_DEFECT_DIALOG };
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
	static unsigned WINAPI	MainThread(LPVOID param);				// ���C���X���b�h
	void	InitProcess();										// �e�v���Z�X����������
	void	ExitProcess();										// �e�v���Z�X�I������


	CString GetName_DspStatus(DIV_DSP_STATUS em);					// �\�����
	CString GetArrayFormat(int const* p, char* sprit, int nCnt);	// �z��P�ʂ̃t�H�[�}�b�g

																// �e���[�J�[�X���b�h
	LogFileManager*			mcls_pLog;							// LogFileManager�X���b�h�C���X�^���X
	MainInctance			mcls_MI;							// ���C���C���X�^���X����N���X
// <<<-----------------------------

																// ���X�g�r���[�N���X
	DspListView*			mcnt_pListSt;						// �X�e�[�^�X���
	DspListView*			mcnt_pListPtr;						// �|�C���^���
	DspListView*			mcnt_pListRec;						// ���R�[�h���

	enum LIST_ST_ROW {
		EM_ROW_STATUS=0,										// �X�e�[�^�X
		EM_ROW_INTERVAL,										// �Ԋu
		EM_ROW_RECV_QUE,										// ��MQue
		EM_ROW_IMG_QUE,											// �摜�D��Que
		EM_ROW_IMG_LOW_QUE,										// �摜���Que
		LIST_ST_ROWMAX
	};

	enum LIST_PTR_ROW {
		EM_ROW_REFRESH=0,										// �X�V���t���O
		EM_ROW_REC_WRITE_BASE,									// ������ں���
		EM_ROW_REC_WRITE_TOP,										// �������u�ʒu�ł̌���ں���
		EM_ROW_REC_WRITE_BOT,										// �������u�ʒu�ł̌���ں���
		EM_ROW_KEN_TOP,											// �J�����ʒu(������ʒu1)
		EM_ROW_KEN_BOT,											// �J�����ʒu(������ʒu1)
		EM_ROW_DSP_TOP,											// �\����_(������ʒu2)
		EM_ROW_DSP_BOT,											// �\����_(������ʒu2)
		LIST_PTR_ROWMAX
	};

	enum LIST_REC_COL {
		EM_COL_NO = 0,											// ں���
		EM_COL_KANRINO,											// �Ǘ�No.
		EM_COL_DSP_STATUS,										// �\�����
		EM_COL_KIZU_NUM,										// �r��
		EM_COL_KEN_TOP,											// �J�����ʒu(�\��) �ʔ���(���ݕ\���s)
		EM_COL_KEN_BOT,											// �J�����ʒu(����) �ʔ���(���ݕ\���s)
		EM_COL_DSP_TOP,											// �\����_(�\��) �ʔ���(���ݕ\���s)
		EM_COL_DSP_BOT,											// �\����_(����) �ʔ���(���ݕ\���s)
		EM_COL_ROW_WRITE_TOP,										// ���ݏ������ݍs
		EM_COL_ROW_WRITE_BOT,										// ���ݏ������ݍs
		EM_COL_ROW_LAST,										// ���������s
// 2024.07.26 Kouno 2CGL�������u�Ӄ֐��x����@�\����(������) ---------------->>>>>
		EM_COL_LONGPD_FLAG,										// ���������ה���
// 2024.07.26 Kouno 2CGL�������u�Ӄ֐��x����@�\����(������) <<<<<----------------
		LIST_REC_COLMAX
	};

public:
	afx_msg void OnBnClickedOk();
	afx_msg void OnClose();
	CListBox m_ListBox;
	afx_msg void OnLbnDblclkList1();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	CListCtrl m_ListSt;
	CListCtrl m_ListPtr;
	CListCtrl m_ListRec;
	afx_msg void OnBnClickedDebugParam();
	//afx_msg void OnBnClickedButtonOutput();
	//afx_msg void OnBnClickedButtonInput();
	afx_msg void OnBnClickedMemOutput();
	afx_msg void OnBnClickedMemInput();
// 2024.07.26 Kouno 2CGL�������u�Ӄ֐��x����@�\����(������) ---------------->>>>>
	afx_msg void OnBnClickedDebugLongpd();
// 2024.07.26 Kouno 2CGL�������u�Ӄ֐��x����@�\����(������) <<<<<----------------
};
