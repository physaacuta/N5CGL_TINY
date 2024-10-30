// TI_RPISAVEDlg.h : ヘッダー ファイル
//

#pragma once
#include "afxwin.h"

#include "..\..\Library\KizuLibMFC\CLASS\DspListView.h"				// リストビュー操作クラス
#include "afxcmn.h"


// CTI_RPISAVEDlg ダイアログ
class CTI_RPISAVEDlg : public CDialog
{
// コンストラクション
public:
	CTI_RPISAVEDlg(CWnd* pParent = NULL);	// 標準コンストラクタ

// ダイアログ データ
	enum { IDD = IDD_TI_RPISAVE_DIALOG };

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
//	afx_msg void OnLbnSelchangeList1();
	afx_msg void OnBnClickedOk();
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


	DspListView*			mcnt_pQue;							// リストビュー キュー状態
	DspListView*			mcnt_pExe;							// リストビュー 処理時間
	DspListView*			mcnt_pAll;							// リストビュー Ins情報
	DspListView*			mcnt_pRpi;							// リストビュー Rpi情報
	DspListView*			mcnt_pSts;							// リストビュー Sts情報

// <<<-----------------------------

public:
	CListCtrl m_ViewAll;
	CListCtrl m_ViewExec;
	CListCtrl m_ViewQue;
	CListCtrl m_ViewSts;
	afx_msg void OnLbnDblclkList1();
	afx_msg void OnBnClickedDebugStart();
	afx_msg void OnBnClickedDebugStop();
	afx_msg void OnBnClickedDspwindow1();
	CListCtrl m_ViewRpi;
	afx_msg void OnBnClickedDebugResync();
};
