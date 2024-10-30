// KS_MASTERDlg.h : ヘッダー ファイル
//

#pragma once
#include "afxwin.h"


// CKS_MASTERDlg ダイアログ
class CKS_MASTERDlg : public CDialog
{
// コンストラクション
public:
	CKS_MASTERDlg(CWnd* pParent = NULL);	// 標準コンストラクタ

// ダイアログ データ
	enum { IDD = IDD_KS_MASTER_DIALOG };

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
private:
	// 処理関数
	void OnCancel() {};											// Escキー対策
	static UINT WINAPI MainThread(LPVOID param);				// メインスレッド
	void InitProcess();											// 各プロセス初期化処理
	void ExitProcess();											// 各プロセス終了処理

	// 各ワーカースレッド
	LogFileManager* mcls_pLog;									// LogFileManagerスレッドインスタンス
	MainInctance mcls_MI;										// メインインスタンス制御クラス
	int m_nEndStatus;											// 終了ステータス (0:いつも通り 1:シャットダウン 2:再起動)
	HANDLE m_evManualEnd;										// 手動の強制終了シグナル



	void SendMail_Stop();									// 検査停止通知 送信

// <<<-----------------------------

	afx_msg void OnBnClickedOk();
	afx_msg void OnClose();
//	afx_msg void OnTimer(UINT_PTR nIDEvent);
	CListBox m_ListBox;
	afx_msg void OnLbnDblclkList1();
	afx_msg void OnBnClickedAlldown();
	afx_msg void OnBnClickedAllreboot();
//	afx_msg void OnBnClickedMydown();
	afx_msg void OnBnClickedMyreboot();
public:
	afx_msg void OnBnClickedAlllogoff();
	afx_msg void OnBnClickedMylogoff();
	afx_msg void OnBnClickedAlltaskend();
};
