// TO_GETDATDlg.h : ヘッダー ファイル
//

#pragma once
#include "afxwin.h"


// CTO_GETDATDlg ダイアログ
class CTO_GETDATDlg : public CDialog
{
// コンストラクション
public:
	CTO_GETDATDlg(CWnd* pParent = NULL);	// 標準コンストラクタ

// ダイアログ データ
	enum { IDD = IDD_TO_GETDAT_DIALOG };

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
	CListBox m_ListBox;
	afx_msg void OnLbnDblclkList1();
	afx_msg void OnClose();
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
	MainInctance			mcls_MI;							// メインインスタンス制御クラス
// <<<-----------------------------
public:
	afx_msg void OnBnClickedResend();
	afx_msg void OnBnClickedResend2();
	afx_msg void OnBnClickedResend3();
};
