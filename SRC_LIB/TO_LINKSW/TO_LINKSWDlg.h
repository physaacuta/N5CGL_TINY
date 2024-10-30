// TO_LINKSWDlg.h : ヘッダー ファイル
//

#pragma once
#include "afxwin.h"
#include "afxcmn.h"

#include "..\..\Library\KizuLibMFC\CLASS\DspListView.h"				// リストビュー操作クラス

// CTO_LINKSWDlg ダイアログ
class CTO_LINKSWDlg : public CDialog
{
// コンストラクション
public:
	CTO_LINKSWDlg(CWnd* pParent = NULL);	// 標準コンストラクタ

// ダイアログ データ
	enum { IDD = IDD_TO_LINKSW_DIALOG };

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
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnBnClickedOk();
	CListBox m_ListBox;
	afx_msg void OnLbnDblclkList1();
// 手動追加 -------------------->>>
private:
	// 処理関数
	void OnCancel() {};											// Escキー対策
	static UINT WINAPI MainThread(LPVOID param);				// メインスレッド
	void InitProcess();											// 各プロセス初期化処理
	void ExitProcess();											// 各プロセス終了処理

	void CommandSender(COMMON_QUE* que);						// コマンド処理開始要求

	// 各ワーカースレッド
	LogFileManager* mcls_pLog;									// LogFileManagerスレッドインスタンス
	MainInstance mcls_MI;										// メインインスタンス制御クラス
	DspListView* mcnt_pListBoard;								// ボード接続状況リストビュークラス
// <<<-----------------------------
public:

	afx_msg void OnBnClickedDebugParam();
	afx_msg void OnBnClickedDebugStartSet();
	afx_msg void OnBnClickedDebugCameraSet();
	afx_msg void OnBnClickedDebugCcmode();
	afx_msg void OnBnClickedDebugSlaveSet();
	afx_msg void OnBnClickedDebugCounterReset();
	afx_msg void OnBnClickedDebugCounterCheck();
	afx_msg void OnBnClickedDebugSetSerialSpeed();
	CComboBox m_cmbUnitDsp;
	CListCtrl m_ListBoard;
	CComboBox m_cmbUnitExec;
};
