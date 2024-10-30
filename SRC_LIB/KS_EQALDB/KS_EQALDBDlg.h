// KS_EQALDBDlg.h : ヘッダー ファイル
//

#pragma once
#include "afxwin.h"


// CKS_EQALDBDlg ダイアログ
class CKS_EQALDBDlg : public CDialog
{
// コンストラクション
public:
	CKS_EQALDBDlg(CWnd* pParent = NULL);	// 標準コンストラクタ

// ダイアログ データ
	enum { IDD = IDD_KS_EQALDB_DIALOG };

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
	static UINT WINAPI MainThread(LPVOID param);				// メインスレッド
	void InitProcess();											// 各プロセス初期化処理
	void ExitProcess();											// 各プロセス終了処理

	// 各ワーカースレッド
	LogFileManager*			mcls_pLog;							// LogFileManagerスレッドインスタンス
	MainInctance			mcls_MI;							// メインインスタンス制御クラス
// <<<-----------------------------

public:
	afx_msg void OnBnClickedMoveDb();
	afx_msg void OnBnClickedSyncroAb();
	afx_msg void OnBnClickedSyncroBa();
	afx_msg void OnBnClickedNowexecAb();
	afx_msg void OnBnClickedNowexecBa();
	CComboBox m_cmbSession;
	afx_msg void OnTimer(UINT_PTR nIDEvent);
};
