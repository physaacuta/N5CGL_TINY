// KS_SYSLOGDlg.h : ヘッダー ファイル
//

#pragma once
#include "afxwin.h"


// CKS_SYSLOGDlg ダイアログ
class CKS_SYSLOGDlg : public CDialog
{
// コンストラクション
public:
	CKS_SYSLOGDlg(CWnd* pParent = NULL);	// 標準コンストラクタ

// ダイアログ データ
	enum { IDD = IDD_KS_SYSLOG_DIALOG };

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
	afx_msg void OnBnClickedLogdsp();
public:
	afx_msg void OnBnClickedOk();
public:
	afx_msg void OnLbnDblclkList1();



// 手動追加 -------------------->>>
private:
	// 処理関数
	void OnCancel() {};											// Escキー対策
	static UINT WINAPI MainThread(LPVOID param);				// メインスレッド
	void InitProcess();											// 各プロセス初期化処理
	void ExitProcess();											// 各プロセス終了処理

	// 各ワーカースレッド
	LogFileManager* mcls_pLog;									// LogFileManagerスレッドインスタンス
	MainInctance mcls_MI;										// メインインスタンス制御クラス
// <<<-----------------------------
	afx_msg void OnClose();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	CListBox m_ListBox;
public:
	afx_msg void OnBnClickedButton1();
};
