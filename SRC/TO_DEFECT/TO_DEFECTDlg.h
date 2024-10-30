
// TO_DEFECTDlg.h : ヘッダー ファイル
//

#pragma once
#include "afxwin.h"

#include "..\..\Library\KizuLibMFC\CLASS\DspListView.h"				// リストビュー操作クラス
#include "SListBox.h"
#include "afxcmn.h"


// CTO_DEFECTDlg ダイアログ
class CTO_DEFECTDlg : public CDialogEx
{
// コンストラクション
public:
	CTO_DEFECTDlg(CWnd* pParent = NULL);	// 標準コンストラクター

// ダイアログ データ
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_TO_DEFECT_DIALOG };
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
public:
	afx_msg void OnClose();
	afx_msg void OnBnClickedOk();
	CListBox m_ListBox;
//	afx_msg void OnLbnDblclkList1();
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

	// その他関数
	void	DummyCoilCreate(int nSendPoint);					// ダミーコイル生成(nSendPoint:送信位置)

	// 各ワーカースレッド
	LogFileManager*			mcls_pLog;							// LogFileManagerスレッドインスタンス
	LogFileManager*			mcls_pLogSeq;						// シーケンサUDP送受信ログ用クラス
	LogFileManager*			mcls_pLogD;							// 調査ログ用クラス
	LogFileManager*			mcls_pLogG;							// ガイダンスログ用クラス
	LogFileManager*			mcls_pLogE;							// エッジUDPログ用クラス
	LogFileManager*			mcls_pLogA;							// 警報ログ用クラス
	MainInstance			mcls_MI;							// メインインスタンス制御クラス

	DspListView*			mcnt_pListSt;						// ステータス状態
	DspListView*			mcnt_pListSys;						// システム状態
	DspListView*			mcnt_pListHt;						// 判定結果表示リストビュークラス(単発)
	DspListView*			mcnt_pListHtTime;					// 判定処理時間リストビュークラス
	DspListView*			mcnt_pListOp;						// 表示状態
	DspListView*			mcnt_pListPr;						// 判定結果表示リストビュークラス(パララン)

	DspListView*			mcnt_pListSetuDsp;					// 設備位置トラッキング情報リストビュークラス
	DspListView*			mcnt_pListEdge;						// エッジ状態
	DspListView*			mcnt_pListKiki;						// 機器状態
	DspListView*			mcnt_pListRamp;						// 疵接近ランプ状態
	DspListView*			mcnt_pListPato;						// パトライト状態
	DspListView*			mcnt_pListVoice;					// ボイス状態
	DspListView*			mcnt_pListDmem;						// データメモリ

	DspListView*			mcnt_pListCoil;						// コイル情報表示リストビュークラス
	DspListView*			mcnt_pListPlgKen;					// PLG 検査基点情報リストビュークラス
	DspListView*			mcnt_pListPlgDsp;					// PLG 表示基点情報リストビュークラス
	DspListView*			mcnt_pListPlg;						// PLG 基本情報リストビュークラス

	DspListView*			mcnt_pListDb;						// DB結果表示リストビュークラス
	DspListView*			mcnt_pListDbInfo;					// DB情報リストビュークラス

	DspListView*			mcnt_pListCmd;						// コマンド
	SListBox				mlst_Cmd;							// コマンド

	DspListView*			mcnt_pListInput;					// デバック操作
	SListBox				mlst_Input;							// デバック操作

	// デバッグ用
	int						m_nCoil;							// コイルNoダミーカウント(0～99)
	bool					m_bInit;							// 初期化フラグ（起動～初回タイマイベントまで）

// <<<-----------------------------
public:
	afx_msg void OnLbnDblclkList1();
	afx_msg void OnNMClickListCmd(NMHDR *pNMHDR, LRESULT *pResult);
	CListCtrl m_ListSt;
	CListCtrl m_ListSys;
	CListCtrl m_ListHt;
	CListCtrl m_ListHtTime;
	CListCtrl m_ListCoil;
	CListCtrl m_ListPlgKen;
	CListCtrl m_ListPlgDsp;
	CListCtrl m_ListPlg;
	CListCtrl m_ListDb;
	CListCtrl m_ListDbInfo;
	CListCtrl m_ListOp;
	CListCtrl m_ListEdge;
	CListCtrl m_ListKiki;
	CListCtrl m_ListRamp;
	CListCtrl m_ListPato;
	CListCtrl m_ListVoice;
	CListCtrl m_ListSetuDsp;
	afx_msg void OnBnClickedSmyuPio();
	afx_msg void OnBnClickedSmyuPlg();
	afx_msg void OnBnClickedDebugLedonoff();
	afx_msg void OnNMClickListInput(NMHDR *pNMHDR, LRESULT *pResult);
	CListCtrl m_ListPr;
	int m_nParaKind;
	int m_nCamsetResult;
};
