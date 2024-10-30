// TO_CAMERADlg.h : ヘッダー ファイル
//

#pragma once
#include "afxwin.h"


// CTO_CAMERADlg ダイアログ
class CTO_CAMERADlg : public CDialog
{
// コンストラクション
public:
	CTO_CAMERADlg(CWnd* pParent = NULL);	// 標準コンストラクタ

// ダイアログ データ
	enum { IDD = IDD_TO_CAMERA_DIALOG };

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
	MainInctance mcls_MI;										// メインインスタンス制御クラス
// <<<-----------------------------
public:
	afx_msg void OnBnClickedRoup1();
	afx_msg void OnBnClickedDebugInit();
	afx_msg void OnBnClickedDebugParam();
	afx_msg void OnBnClickedDebugPlson();
	afx_msg void OnBnClickedDebugPlsoff();
	afx_msg void OnBnClickedDebugScodeT();
	afx_msg void OnBnClickedDebugScodeB();
	afx_msg void OnBnClickedDebugAgc();
	afx_msg void OnBnClickedRoup2();
	afx_msg void OnBnClickedRoup3();
	afx_msg void OnBnClickedRoup4();
	afx_msg void OnBnClickedRoup5();
	afx_msg void OnBnClickedRoup6();
	afx_msg void OnBnClickedRoup7();
	afx_msg void OnBnClickedRoup8();
	afx_msg void OnBnClickedRoup9();
	afx_msg void OnBnClickedRoup10();
	afx_msg void OnBnClickedRoup11();
	afx_msg void OnBnClickedRoup12();
	afx_msg void OnBnClickedRoup13();
	afx_msg void OnBnClickedRoup14();
	afx_msg void OnBnClickedRoup15();
	afx_msg void OnBnClickedRoup16();
	afx_msg void OnBnClickedRodown1();
	afx_msg void OnBnClickedRodown2();
	afx_msg void OnBnClickedRodown3();
	afx_msg void OnBnClickedRodown4();
	afx_msg void OnBnClickedRodown5();
	afx_msg void OnBnClickedRodown6();
	afx_msg void OnBnClickedRodown7();
	afx_msg void OnBnClickedRodown8();
	afx_msg void OnBnClickedRodown9();
	afx_msg void OnBnClickedRodown10();
	afx_msg void OnBnClickedRodown11();
	afx_msg void OnBnClickedRodown12();
	afx_msg void OnBnClickedRodown13();
	afx_msg void OnBnClickedRodown14();
	afx_msg void OnBnClickedRodown15();
	afx_msg void OnBnClickedRodown16();
	afx_msg void OnBnClickedRoup17();
	afx_msg void OnBnClickedRoup18();
	afx_msg void OnBnClickedRoup19();
	afx_msg void OnBnClickedRoup20();
	afx_msg void OnBnClickedRoup21();
	afx_msg void OnBnClickedRoup22();
	afx_msg void OnBnClickedRoup23();
	afx_msg void OnBnClickedRoup24();
	afx_msg void OnBnClickedRoup25();
	afx_msg void OnBnClickedRoup26();
	afx_msg void OnBnClickedRoup27();
	afx_msg void OnBnClickedRoup28();
	afx_msg void OnBnClickedRoup29();
	afx_msg void OnBnClickedRoup30();
	afx_msg void OnBnClickedRoup31();
	afx_msg void OnBnClickedRoup32();
	afx_msg void OnBnClickedRodown17();
	afx_msg void OnBnClickedRodown18();
	afx_msg void OnBnClickedRodown19();
	afx_msg void OnBnClickedRodown20();
	afx_msg void OnBnClickedRodown21();
	afx_msg void OnBnClickedRodown22();
	afx_msg void OnBnClickedRodown23();
	afx_msg void OnBnClickedRodown24();
	afx_msg void OnBnClickedRodown25();
	afx_msg void OnBnClickedRodown26();
	afx_msg void OnBnClickedRodown27();
	afx_msg void OnBnClickedRodown28();
	afx_msg void OnBnClickedRodown29();
	afx_msg void OnBnClickedRodown30();
	afx_msg void OnBnClickedRodown31();
	afx_msg void OnBnClickedRodown32();
	CComboBox m_cmbUnitDsp;
	CComboBox m_cmbUnitExec;
	afx_msg void OnBnClickedCmdSend();
	afx_msg void OnBnClickedDebugCheckInit();
};
