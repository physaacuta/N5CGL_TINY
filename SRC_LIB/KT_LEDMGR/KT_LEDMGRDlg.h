// KT_LEDMGRDlg.h : ヘッダー ファイル
//

#pragma once
#include "afxwin.h"
#include "afxcmn.h"

#include "..\..\Library\KizuLibMFC\CLASS\DspListView.h"				// リストビュー操作クラス


// CKT_LEDMGRDlg ダイアログ
class CKT_LEDMGRDlg : public CDialog
{
// コンストラクション
public:
	CKT_LEDMGRDlg(CWnd* pParent = NULL);	// 標準コンストラクタ

// ダイアログ データ
	enum { IDD = IDD_KT_LEDMGR_DIALOG };

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
	afx_msg void OnLbnDblclkList1();
	afx_msg void OnBnClickedOk();
	afx_msg void OnClose();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
//	afx_msg void OnEndSession(BOOL bEnding);

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

	DspListView*			mcnt_pListVal;						// 補正配光リストビュークラス
	DspListView*			mcnt_pListTemp;						// 温度計測点リストビュークラス

	int						m_nColCntVal;						// 補正配光リストビュー列数(全装置、全chの中で最大のブロック数)
	int						m_nColCntTemp;						// 温度計測店リストビュークラス列数(全装置、全chの中で最大の温度計測点数)

// <<<-----------------------------
public:
	CComboBox m_cmbSession;
	afx_msg void OnBnClickedDebagState();
	afx_msg void OnBnClickedDebagTemp();
	afx_msg void OnBnClickedDebagOn();
	afx_msg void OnBnClickedDebagOff();
	afx_msg void OnBnClickedDebagReset();
	afx_msg void OnBnClickedDebagLedScode();
	afx_msg void OnBnClickedDebagLedVal();
	CComboBox m_cmbCh;
	CComboBox m_cmbDebagCh;
	CListCtrl m_ListVal;
	afx_msg void OnBnClickedDebagLedHosei();
	afx_msg void OnBnClickedDebagLedHoseiAll();
	CListCtrl m_lstTemp;
};
