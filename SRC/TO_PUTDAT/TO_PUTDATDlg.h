
// TO_PUTDATDlg.h : ヘッダー ファイル
//

#pragma once
#include "afxwin.h"
#include "afxcmn.h"

#include "..\..\Library\KizuLibMFC\CLASS\DspListView.h"				// リストビュー操作クラス

// CTO_PUTDATDlg ダイアログ
class CTO_PUTDATDlg : public CDialog
{
// コンストラクション
public:
	CTO_PUTDATDlg(CWnd* pParent = NULL);	// 標準コンストラクター

// ダイアログ データ
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_TO_PUTDAT_DIALOG };
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

	// 手動追加 -------------------->>>
	// VS2005とVS2008共有とした場合は、構成プロパティ/リンカ/出力ファイル を以下に変更
	// $(OutDir)\$(ProjectName).exe  → $(OutDir)\XX_CPPSMPEX.exe
private:
	// 処理関数
	void	OnCancel() {};										// Escキー対策
	static unsigned WINAPI	MainThread(LPVOID param);				// メインスレッド
	void	InitProcess();										// 各プロセス初期化処理
	void	ExitProcess();										// 各プロセス終了処理

	// 各ワーカースレッド
	LogFileManager*			mcls_pLog;							// LogFileManagerスレッドインスタンス
	MainInstance			mcls_MI;							// メインインスタンス制御クラス
	// <<<-----------------------------

public:
	afx_msg void OnClose();
	afx_msg void OnBnClickedOk();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnLbnDblclkList1();
	CListBox m_ListBox;
	afx_msg void OnBnClickedDbgchkbox();
	afx_msg void OnBnClickedBtnSaisun();
	afx_msg void OnBnClickedBtnCut();
};
