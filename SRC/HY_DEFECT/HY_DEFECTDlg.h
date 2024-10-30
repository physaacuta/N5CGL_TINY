
// HY_DEFECTDlg.h : ヘッダー ファイル
//

#pragma once
#include "afxwin.h"
#include "afxcmn.h"

#include "..\..\Library\KizuLibMFC\CLASS\DspListView.h"				// リストビュー操作クラス


// CHY_DEFECTDlg ダイアログ
class CHY_DEFECTDlg : public CDialog
{
// コンストラクション
public:
	CHY_DEFECTDlg(CWnd* pParent = NULL);	// 標準コンストラクター

// ダイアログ データ
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_HY_DEFECT_DIALOG };
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


	CString GetName_DspStatus(DIV_DSP_STATUS em);					// 表示状態
	CString GetArrayFormat(int const* p, char* sprit, int nCnt);	// 配列単位のフォーマット

																// 各ワーカースレッド
	LogFileManager*			mcls_pLog;							// LogFileManagerスレッドインスタンス
	MainInctance			mcls_MI;							// メインインスタンス制御クラス
// <<<-----------------------------

																// リストビュークラス
	DspListView*			mcnt_pListSt;						// ステータス状態
	DspListView*			mcnt_pListPtr;						// ポインタ状態
	DspListView*			mcnt_pListRec;						// レコード状態

	enum LIST_ST_ROW {
		EM_ROW_STATUS=0,										// ステータス
		EM_ROW_INTERVAL,										// 間隔
		EM_ROW_RECV_QUE,										// 受信Que
		EM_ROW_IMG_QUE,											// 画像優先Que
		EM_ROW_IMG_LOW_QUE,										// 画像一般Que
		LIST_ST_ROWMAX
	};

	enum LIST_PTR_ROW {
		EM_ROW_REFRESH=0,										// 更新中フラグ
		EM_ROW_REC_WRITE_BASE,									// 書込みﾚｺｰﾄﾞ
		EM_ROW_REC_WRITE_TOP,										// 検査装置位置での現在ﾚｺｰﾄﾞ
		EM_ROW_REC_WRITE_BOT,										// 検査装置位置での現在ﾚｺｰﾄﾞ
		EM_ROW_KEN_TOP,											// カメラ位置(検査台位置1)
		EM_ROW_KEN_BOT,											// カメラ位置(検査台位置1)
		EM_ROW_DSP_TOP,											// 表示基点(検査台位置2)
		EM_ROW_DSP_BOT,											// 表示基点(検査台位置2)
		LIST_PTR_ROWMAX
	};

	enum LIST_REC_COL {
		EM_COL_NO = 0,											// ﾚｺｰﾄﾞ
		EM_COL_KANRINO,											// 管理No.
		EM_COL_DSP_STATUS,										// 表示状態
		EM_COL_KIZU_NUM,										// 疵数
		EM_COL_KEN_TOP,											// カメラ位置(表面) 通板距離(現在表示行)
		EM_COL_KEN_BOT,											// カメラ位置(裏面) 通板距離(現在表示行)
		EM_COL_DSP_TOP,											// 表示基点(表面) 通板距離(現在表示行)
		EM_COL_DSP_BOT,											// 表示基点(裏面) 通板距離(現在表示行)
		EM_COL_ROW_WRITE_TOP,										// 現在書き込み行
		EM_COL_ROW_WRITE_BOT,										// 現在書き込み行
		EM_COL_ROW_LAST,										// 検査完了行
// 2024.07.26 Kouno 2CGL検査装置φω精度向上機能導入(長周期) ---------------->>>>>
		EM_COL_LONGPD_FLAG,										// 長周期欠陥発生
// 2024.07.26 Kouno 2CGL検査装置φω精度向上機能導入(長周期) <<<<<----------------
		LIST_REC_COLMAX
	};

public:
	afx_msg void OnBnClickedOk();
	afx_msg void OnClose();
	CListBox m_ListBox;
	afx_msg void OnLbnDblclkList1();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	CListCtrl m_ListSt;
	CListCtrl m_ListPtr;
	CListCtrl m_ListRec;
	afx_msg void OnBnClickedDebugParam();
	//afx_msg void OnBnClickedButtonOutput();
	//afx_msg void OnBnClickedButtonInput();
	afx_msg void OnBnClickedMemOutput();
	afx_msg void OnBnClickedMemInput();
// 2024.07.26 Kouno 2CGL検査装置φω精度向上機能導入(長周期) ---------------->>>>>
	afx_msg void OnBnClickedDebugLongpd();
// 2024.07.26 Kouno 2CGL検査装置φω精度向上機能導入(長周期) <<<<<----------------
};
