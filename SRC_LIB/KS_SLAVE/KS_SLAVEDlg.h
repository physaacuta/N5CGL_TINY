// KS_SLAVEDlg.h : ヘッダー ファイル
//

#pragma once
#include "afxwin.h"


// CKS_SLAVEDlg ダイアログ
class CKS_SLAVEDlg : public CDialog
{
// コンストラクション
public:
	CKS_SLAVEDlg(CWnd* pParent = NULL);	// 標準コンストラクタ

// ダイアログ データ
	enum { IDD = IDD_KS_SLAVE_DIALOG };

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
	CListBox m_ListBox;
public:
	afx_msg void OnLbnDblclkList1();
public:
	afx_msg void OnBnClickedOk();
public:
	afx_msg void OnClose();


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
// <<<-----------------------------


	afx_msg void OnBnClickedMydown2();
	afx_msg void OnBnClickedMyreboot();
public:
	afx_msg void OnBnClickedMylogoff();
	afx_msg void OnBnClickedMytaskend();
};
