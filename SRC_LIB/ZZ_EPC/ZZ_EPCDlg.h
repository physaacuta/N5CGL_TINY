// ZZ_EPCDlg.h : ヘッダー ファイル
//

#pragma once
#include "afxwin.h"

//#include "..\..\Include\EPC\NCspeed.h"
#include "..\..\Include\EPC\NCL2DETECT_if.h"							// I/F定義

// CZZ_EPCDlg ダイアログ
class CZZ_EPCDlg : public CDialog
{
// コンストラクション
public:
	CZZ_EPCDlg(CWnd* pParent = NULL);	// 標準コンストラクタ

// ダイアログ データ
	enum { IDD = IDD_ZZ_EPC_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV サポート


// 実装
protected:
	HICON m_hIcon;

	// 生成された、メッセージ割り当て関数
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	afx_msg void OnLbnDblclkList1();
	afx_msg void OnClose();
	CListBox m_ListBox;

// 手動追加 -------------------->>>
private:
	// 処理関数
	void OnCancel() {};											// Escキー対策
	void InitProcess();											// 各プロセス初期化処理
	void ExitProcess();											// 各プロセス終了処理

	// 各ワーカースレッド
	LogFileManager*			mcls_pLog;							// LogFileManagerスレッドインスタンス
	MainInctance			mcls_MI;							// メインインスタンス制御クラス
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

	// 疵種1～5個目 出力条件変更
	afx_msg void OnBnClickedCngKizu1()  { ChangeKizuInfo(0); };
	afx_msg void OnBnClickedCngKizu2()  { ChangeKizuInfo(1); };
	afx_msg void OnBnClickedCngKizu3()  { ChangeKizuInfo(2); };
	afx_msg void OnBnClickedCngKizu4()  { ChangeKizuInfo(3); };
	afx_msg void OnBnClickedCngKizu5()  { ChangeKizuInfo(4); };

	// 疵種1～5個目 出力停止/再開
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
