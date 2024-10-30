// TO_SEQDMDlg.h : ヘッダー ファイル
//

#pragma once
#include "afxwin.h"
#include "actqj71e71udp1.h"


// CTO_SEQDMDlg ダイアログ
class CTO_SEQDMDlg : public CDialog
{
// コンストラクション
public:
	CTO_SEQDMDlg(CWnd* pParent = NULL);	// 標準コンストラクタ

// ダイアログ データ
	enum { IDD = IDD_TO_SEQDM_DIALOG };

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
	afx_msg void OnClose();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnLbnDblclkList1();

// 手動追加 -------------------->>>
private:
	//// 定数
	static const int MX_OPEN_RETRY_NID_EVENT	= 10;					// MXコンポーネントオープン要求 タイマーID
	static const int MX_OPEN_RETRY_TIMER		= 2000;					// タイマー周期 [ms]

private:
	// 処理関数
	void OnCancel() {};											// Escキー対策
	static UINT WINAPI MainThread(LPVOID param);				// メインスレッド
	void InitProcess();											// 各プロセス初期化処理
	void ExitProcess();											// 各プロセス終了処理


	//// MXコンポーネト 操作
	bool MX_Connect(bool bLogDsp);								// 接続
	void MX_Close();											// 切断 & リトライ設定
	UINT					m_tidMxConnect;						// MXコンポーネト初期接続タイマー


	// 各ワーカースレッド
	LogFileManager*			mcls_pLog;							// LogFileManagerスレッドインスタンス
	MainInctance			mcls_MI;							// メインインスタンス制御クラス
// <<<-----------------------------
public:
	CActqj71e71udp1 m_cCom;
	afx_msg void OnBnClickedOk();
};
