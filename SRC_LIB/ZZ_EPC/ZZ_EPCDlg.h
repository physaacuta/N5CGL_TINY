// ZZ_EPCDlg.h : �w�b�_�[ �t�@�C��
//

#pragma once
#include "afxwin.h"

//#include "..\..\Include\EPC\NCspeed.h"
#include "..\..\Include\EPC\NCL2DETECT_if.h"							// I/F��`

// CZZ_EPCDlg �_�C�A���O
class CZZ_EPCDlg : public CDialog
{
// �R���X�g���N�V����
public:
	CZZ_EPCDlg(CWnd* pParent = NULL);	// �W���R���X�g���N�^

// �_�C�A���O �f�[�^
	enum { IDD = IDD_ZZ_EPC_DIALOG };

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
	afx_msg void OnBnClickedOk();
	afx_msg void OnLbnDblclkList1();
	afx_msg void OnClose();
	CListBox m_ListBox;

// �蓮�ǉ� -------------------->>>
private:
	// �����֐�
	void OnCancel() {};											// Esc�L�[�΍�
	void InitProcess();											// �e�v���Z�X����������
	void ExitProcess();											// �e�v���Z�X�I������

	// �e���[�J�[�X���b�h
	LogFileManager*			mcls_pLog;							// LogFileManager�X���b�h�C���X�^���X
	MainInctance			mcls_MI;							// ���C���C���X�^���X����N���X
// <<<-----------------------------


public:
	afx_msg void OnBnClickedUdprecv();
	afx_msg void OnDestroy();
//	afx_msg void OnBnClickedPutkizu();
//	afx_msg void OnBnClickedCngkizu();
//	afx_msg void OnBnClickedCngfrm();
//	afx_msg void OnBnClickedChgtype();
//	afx_msg void OnBnClickedChggrade();
	afx_msg void OnBnClickedChangeBrightness();
	afx_msg void OnBnClickedChangeCamgain();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	void ChangeKizuInfo(int n);
	void StopKizuOutPut(int n);
	afx_msg void OnBnClickedChangeEdge();
	afx_msg void OnBnClickedPutedge();

	// �r��1�`5�� �o�͏����ύX
	afx_msg void OnBnClickedCngKizu1()  { ChangeKizuInfo(0); };
	afx_msg void OnBnClickedCngKizu2()  { ChangeKizuInfo(1); };
	afx_msg void OnBnClickedCngKizu3()  { ChangeKizuInfo(2); };
	afx_msg void OnBnClickedCngKizu4()  { ChangeKizuInfo(3); };
	afx_msg void OnBnClickedCngKizu5()  { ChangeKizuInfo(4); };

	// �r��1�`5�� �o�͒�~/�ĊJ
	afx_msg void OnBnClickedStopKizu1() { StopKizuOutPut(0); };
	afx_msg void OnBnClickedStopKizu2() { StopKizuOutPut(1); };
	afx_msg void OnBnClickedStopKizu3() { StopKizuOutPut(2); };
	afx_msg void OnBnClickedStopKizu4() { StopKizuOutPut(3); };
	afx_msg void OnBnClickedStopKizu5() { StopKizuOutPut(4); };
	afx_msg void OnBnClickedUpdate();
	afx_msg void OnBnClickedSkip();
	afx_msg void OnBnClickedCheck2();
	CComboBox m_cmbCamId;
	afx_msg void OnBnClickedChangeCamGain();
	afx_msg void OnBnClickedChangeCamExp();
	afx_msg void OnBnClickedChangeCamVolt();
	afx_msg void OnBnClickedChangeCamTemp();
	afx_msg void OnCbnSelchangeCamId();
	afx_msg void OnBnClickedUseSmem();
	afx_msg void OnCbnSelchangeShrinkratio();
	CComboBox m_cmbShrinkRatio;
};
