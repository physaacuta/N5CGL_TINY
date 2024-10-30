// GOT_CONTROLDlg.h : ヘッダー ファイル
//

#pragma once
#include "afxwin.h"


// CGOT_CONTROLDlg ダイアログ
class CGOT_CONTROLDlg : public CDialog
{
// コンストラクション
public:
	CGOT_CONTROLDlg(CWnd* pParent = NULL);	// 標準コンストラクタ

// ダイアログ データ
	enum { IDD = IDD_GOT_CONTROL_DIALOG };

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
	afx_msg void OnLbnDblclkList1();
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

	void	ManualPio(int pio, int port, int bit, bool flg);

	// 各ワーカースレッド
	LogFileManager*			mcls_pLog;							// LogFileManagerスレッドインスタンス
	MainInctance			mcls_MI;							// メインインスタンス制御クラス
// <<<-----------------------------

public:
//	afx_msg void OnBnClickedPlgFramecycleS();
	afx_msg void OnBnClickedPlgFramecycleC();
	afx_msg void OnBnClickedPlgSpeedC0();
	afx_msg void OnBnClickedPlgSpeedS0();
	afx_msg void OnBnClickedPlgSpeedC1();
	afx_msg void OnBnClickedPlgSpeedS1();
	afx_msg void OnBnClickedPlgSpeedC2();
	afx_msg void OnBnClickedPlgSpeedS2();
	afx_msg void OnBnClickedPlgSpeedC3();
	afx_msg void OnBnClickedPlgSpeedS3();
	afx_msg void OnBnClickedPlgSpeedC4();
	afx_msg void OnBnClickedPlgSpeedS4();
	afx_msg void OnBnClickedPlgSpeedC5();
	afx_msg void OnBnClickedPlgSpeedS5();
	afx_msg void OnBnClickedPlgSpeedC6();
	afx_msg void OnBnClickedPlgSpeedS6();
	afx_msg void OnBnClickedPlgSpeedC7();
	afx_msg void OnBnClickedPlgSpeedS7();
	CComboBox m_cmbPioName;
	afx_msg void OnBnClickedDegubPi();
	afx_msg void OnBnClickedDegubPo();
	afx_msg void OnBnClickedPi00();
	afx_msg void OnBnClickedPi01();
	afx_msg void OnBnClickedPi02();
	afx_msg void OnBnClickedPi03();
	afx_msg void OnBnClickedPi04();
	afx_msg void OnBnClickedPi05();
	afx_msg void OnBnClickedPi06();
	afx_msg void OnBnClickedPi07();
	afx_msg void OnBnClickedPi10();
	afx_msg void OnBnClickedPi11();
	afx_msg void OnBnClickedPi12();
	afx_msg void OnBnClickedPi13();
	afx_msg void OnBnClickedPi14();
	afx_msg void OnBnClickedPi15();
	afx_msg void OnBnClickedPi16();
	afx_msg void OnBnClickedPi17();
	afx_msg void OnBnClickedPi20();
	afx_msg void OnBnClickedPi21();
	afx_msg void OnBnClickedPi22();
	afx_msg void OnBnClickedPi23();
	afx_msg void OnBnClickedPi24();
	afx_msg void OnBnClickedPi25();
	afx_msg void OnBnClickedPi26();
	afx_msg void OnBnClickedPi27();
	afx_msg void OnBnClickedPi30();
	afx_msg void OnBnClickedPi31();
	afx_msg void OnBnClickedPi32();
	afx_msg void OnBnClickedPi33();
	afx_msg void OnBnClickedPi34();
	afx_msg void OnBnClickedPi35();
	afx_msg void OnBnClickedPi36();
	afx_msg void OnBnClickedPi37();
	afx_msg void OnBnClickedPi40();
	afx_msg void OnBnClickedPi41();
	afx_msg void OnBnClickedPi42();
	afx_msg void OnBnClickedPi43();
	afx_msg void OnBnClickedPi44();
	afx_msg void OnBnClickedPi45();
	afx_msg void OnBnClickedPi46();
	afx_msg void OnBnClickedPi47();
	afx_msg void OnBnClickedPi50();
	afx_msg void OnBnClickedPi51();
	afx_msg void OnBnClickedPi52();
	afx_msg void OnBnClickedPi53();
	afx_msg void OnBnClickedPi54();
	afx_msg void OnBnClickedPi55();
	afx_msg void OnBnClickedPi56();
	afx_msg void OnBnClickedPi57();
	afx_msg void OnBnClickedPi60();
	afx_msg void OnBnClickedPi61();
	afx_msg void OnBnClickedPi62();
	afx_msg void OnBnClickedPi63();
	afx_msg void OnBnClickedPi64();
	afx_msg void OnBnClickedPi65();
	afx_msg void OnBnClickedPi66();
	afx_msg void OnBnClickedPi67();
	afx_msg void OnBnClickedPi70();
	afx_msg void OnBnClickedPi71();
	afx_msg void OnBnClickedPi72();
	afx_msg void OnBnClickedPi73();
	afx_msg void OnBnClickedPi74();
	afx_msg void OnBnClickedPi75();
	afx_msg void OnBnClickedPi76();
	afx_msg void OnBnClickedPi77();
	afx_msg void OnBnClickedPo00();
	afx_msg void OnBnClickedPo01();
	afx_msg void OnBnClickedPo02();
	afx_msg void OnBnClickedPo03();
	afx_msg void OnBnClickedPo04();
	afx_msg void OnBnClickedPo05();
	afx_msg void OnBnClickedPo06();
	afx_msg void OnBnClickedPo07();
	afx_msg void OnBnClickedPo10();
	afx_msg void OnBnClickedPo11();
	afx_msg void OnBnClickedPo12();
	afx_msg void OnBnClickedPo13();
	afx_msg void OnBnClickedPo14();
	afx_msg void OnBnClickedPo15();
	afx_msg void OnBnClickedPo16();
	afx_msg void OnBnClickedPo17();
	afx_msg void OnBnClickedPo20();
	afx_msg void OnBnClickedPo21();
	afx_msg void OnBnClickedPo22();
	afx_msg void OnBnClickedPo23();
	afx_msg void OnBnClickedPo24();
	afx_msg void OnBnClickedPo25();
	afx_msg void OnBnClickedPo26();
	afx_msg void OnBnClickedPo27();
	afx_msg void OnBnClickedPo30();
	afx_msg void OnBnClickedPo31();
	afx_msg void OnBnClickedPo32();
	afx_msg void OnBnClickedPo33();
	afx_msg void OnBnClickedPo34();
	afx_msg void OnBnClickedPo35();
	afx_msg void OnBnClickedPo36();
	afx_msg void OnBnClickedPo37();
	afx_msg void OnBnClickedPo40();
	afx_msg void OnBnClickedPo41();
	afx_msg void OnBnClickedPo42();
	afx_msg void OnBnClickedPo43();
	afx_msg void OnBnClickedPo44();
	afx_msg void OnBnClickedPo45();
	afx_msg void OnBnClickedPo46();
	afx_msg void OnBnClickedPo47();
	afx_msg void OnBnClickedPo50();
	afx_msg void OnBnClickedPo51();
	afx_msg void OnBnClickedPo52();
	afx_msg void OnBnClickedPo53();
	afx_msg void OnBnClickedPo54();
	afx_msg void OnBnClickedPo55();
	afx_msg void OnBnClickedPo56();
	afx_msg void OnBnClickedPo57();
	afx_msg void OnBnClickedPo60();
	afx_msg void OnBnClickedPo61();
	afx_msg void OnBnClickedPo62();
	afx_msg void OnBnClickedPo63();
	afx_msg void OnBnClickedPo64();
	afx_msg void OnBnClickedPo65();
	afx_msg void OnBnClickedPo66();
	afx_msg void OnBnClickedPo67();
	afx_msg void OnBnClickedPo70();
	afx_msg void OnBnClickedPo71();
	afx_msg void OnBnClickedPo72();
	afx_msg void OnBnClickedPo73();
	afx_msg void OnBnClickedPo74();
	afx_msg void OnBnClickedPo75();
	afx_msg void OnBnClickedPo76();
	afx_msg void OnBnClickedPo77();
	afx_msg void OnBnClickedGotStart();
	afx_msg void OnBnClickedGotStop();
	afx_msg void OnBnClickedGotChange();
	afx_msg void OnBnClickedPlgFrameReset();
};
