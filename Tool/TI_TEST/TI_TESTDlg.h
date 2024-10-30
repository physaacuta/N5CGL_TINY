
// TI_TESTDlg.h : ヘッダー ファイル
//

#pragma once
#include "afxwin.h"
#include "afxcmn.h"

#include "..\..\Library\KizuLibMFC\CLASS\DspListView.h"				// リストビュー操作クラス


// CTI_TESTDlg ダイアログ
class CTI_TESTDlg : public CDialogEx
{
// コンストラクション
public:
	CTI_TESTDlg(CWnd* pParent = NULL);	// 標準コンストラクター

// ダイアログ データ
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_TI_TEST_DIALOG };
#endif

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
	afx_msg void OnClose();
	afx_msg void OnBnClickedOk();
	CListBox m_ListBox;
	afx_msg void OnLbnDblclkList1();
	afx_msg void OnTimer(UINT_PTR nIDEvent);



// 手動追加 -------------------->>>
// VS2005とVS2008共有とした場合は、構成プロパティ/リンカ/出力ファイル を以下に変更
// $(OutDir)\$(ProjectName).exe  → $(OutDir)\XX_CPPSMPEX.exe
private:
	// 処理関数
	void	OnCancel() {};										// Escキー対策
	static UINT WINAPI	MainThread(LPVOID param);				// メインスレッド
	void	InitProcess();										// 各プロセス初期化処理
	void	ExitProcess();										// 各プロセス終了処理


																// 各ワーカースレッド
	LogFileManager*			mcls_pLog;							// LogFileManagerスレッドインスタンス
	LogFileManager*			mcls_pLogCull;						// LogFileManagerスレッドインスタンス
	MainInctance			mcls_MI;							// メインインスタンス制御クラス

// <<<-----------------------------

	DspListView*			mcnt_pListSt;						// ステータス状態


public:
	afx_msg void OnBnClickedRadInputMan();
	afx_msg void OnBnClickedRadInputBmp();
	afx_msg void OnBnClickedRadInputAvi();
	afx_msg void OnBnClickedBtnMan();
	afx_msg void OnBnClickedChkCycle();
	afx_msg void OnBnClickedDebugManual();
	afx_msg void OnBnClickedBtnStart();
	afx_msg void OnBnClickedBtnStop();
	CListCtrl m_ListSt;
	afx_msg void OnEnChangeEdit1();
	afx_msg void OnBnClickedBtnFrame();
	afx_msg void OnDropFiles(HDROP hDropInfo);
	afx_msg void OnBnClickedBtnNextCoil();
	afx_msg void OnBnClickedChkRenzoku();
};
