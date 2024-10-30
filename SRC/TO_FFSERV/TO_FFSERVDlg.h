// TO_FFSERVDlg.h : ヘッダー ファイル
//

#pragma once
#include "afxwin.h"

// CTO_FFSERVDlg ダイアログ
class CTO_FFSERVDlg : public CDialog
{
// コンストラクション
public:
	CTO_FFSERVDlg(CWnd* pParent = NULL);	// 標準コンストラクタ

// ダイアログ データ
	enum { IDD = IDD_TO_FFSERV_DIALOG };

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
	MainInctance			mcls_MI;							// メインインスタンス制御クラス
// <<<-----------------------------
public:
	CListBox m_ListBox;
	afx_msg void OnClose();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedSendZiseki();
	afx_msg void OnBnClickedSendMae();
	afx_msg void OnBnClickedDb();
	afx_msg void OnBnClickedBin();
	afx_msg void OnBnClickedChkLogAlldump();
	afx_msg void OnBnClickedChkDebugFunction();
	afx_msg void OnLbnDblclkListLog();
};
