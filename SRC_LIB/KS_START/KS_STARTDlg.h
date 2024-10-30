// KS_STARTDlg.h : ヘッダー ファイル
//

#pragma once
#include "afxwin.h"

// インクルード
#include <process.h>

// 疵検 インクルード
#include "..\..\include\KizuCommon.h"									// 疵検共通ヘッダー
//#include "..\..\Library\KizuLib\KizuLib.h"								// 疵検ライブラリヘッダー


// 疵検クラス インクルード
#include "..\..\Library\KizuLib\CLASS\KizuMacro.h"						// 疵検マクロ
#include "..\..\Library\KizuLibMFC\CLASS\LogFileManager.h"				// ログマネージャー

// ローカルクラス
#include "MainExec.h"


using namespace KizuLib;

// CKS_STARTDlg ダイアログ
class CKS_STARTDlg : public CDialog
{
// コンストラクション
public:
	CKS_STARTDlg(CWnd* pParent = NULL);	// 標準コンストラクタ

// ダイアログ データ
	enum { IDD = IDD_KS_START_DIALOG };

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
public:
	afx_msg void OnBnClickedOk();

// 手動追加 -------------------->>>
private:
	// 処理関数
	void OnCancel() {};											// Escキー対策
	static UINT WINAPI MainThread(LPVOID param);				// メインスレッド

	// 各ワーカースレッド
	LogFileManager* mcls_pLog;									// LogFileManagerスレッドインスタンス

	// メンバー変数
	bool m_bCancel;												// 強制キャンセル時 true

	// 追加
	bool CheckMultiExec(LPCSTR SendSlotName);					// 常駐タスク 複数実行チェック
// <<<-----------------------------
public:
	CListBox m_ListBox;
	afx_msg void OnLbnDblclkList1();
};
