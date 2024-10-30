// HT_DEFECTDlg.h : ヘッダー ファイル
//

#pragma once
#include "afxwin.h"

// 各ワーカースレッド
#include "MainConst.h"													// 自クラス定数ヘッダー
#include "..\..\Library\KizuLibMFC\CLASS\DspListView.h"					// リストビュー操作クラス
#include "afxcmn.h"



// CHT_DEFECTDlg ダイアログ
class CHT_DEFECTDlg : public CDialog
{
// コンストラクション
public:
	CHT_DEFECTDlg(CWnd* pParent = NULL);	// 標準コンストラクタ

// ダイアログ データ
	enum { IDD = IDD_HT_DEFECT_DIALOG };

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
	afx_msg void OnBnClickedOk();
	afx_msg void OnClose();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnLbnDblclkList1();


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


	// リストビュー
	DspListView*			mcnt_pListSt;						// 状態リストビュークラス
	DspListView*			mcnt_pListTo;						// 対統括状態リストビュークラス

																
// <<<-----------------------------
public:
	afx_msg void OnBnClickedDebugStart();
	afx_msg void OnBnClickedDebugStop();
	afx_msg void OnBnClickedDebugParam();
	afx_msg void OnBnClickedDebugCoilinf();
	afx_msg void OnBnClickedDebugCanok();
//	afx_msg void OnBnClickedDebugFrame();
//	afx_msg void OnBnClickedDebugProfile();
//	afx_msg void OnBnClickedDebugDlycancel();
	afx_msg void OnBnClickedDebugImgkind();
	CComboBox m_cmbMen;
	CListCtrl m_ListSt;
	CListCtrl m_ListTo;
	afx_msg void OnBnClickedDebugParamPreload();
	afx_msg void OnBnClickedDebugSetPosition();
	afx_msg void OnBnClickedDebugResumeStart();
	afx_msg void OnBnClickedDebugResumeStop();
	afx_msg void OnBnClickedDebugKind1();
	afx_msg void OnBnClickedDebugKind2();
	afx_msg void OnBnClickedDebugKind3();
};
