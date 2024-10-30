
// HY_TESTDlg.h : ヘッダー ファイル
//

#pragma once
#include "afxwin.h"

#include "..\..\Library\KizuLibMFC\CLASS\DspListView.h"				// リストビュー操作クラス
#include "afxcmn.h"


// CHY_TESTDlg ダイアログ
class CHY_TESTDlg : public CDialog
{
// コンストラクション
public:
	CHY_TESTDlg(CWnd* pParent = NULL);	// 標準コンストラクター

// ダイアログ データ
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_HY_TEST_DIALOG };
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
	static UINT WINAPI	MainThread(LPVOID param);				// メインスレッド
	void	InitProcess();										// 各プロセス初期化処理
	void	ExitProcess();										// 各プロセス終了処理
	//void	SetDlgItemFloat(int nID, float fData);
	//float	GetDlgItemFloat(int nID);
	CString GetName_SpmUmu(EM_DIV_SPM em);

	// 各ワーカースレッド
	LogFileManager*			mcls_pLog;							// LogFileManagerスレッドインスタンス
	MainInctance			mcls_MI;							// メインインスタンス制御クラス
	// <<<-----------------------------
																// リストビュークラス
	DspListView*			mcnt_pListCoil;						// コイル情報
	DspListView*			mcnt_pListDsp;						// 検査台情報

	enum LIST_COIL_COL {
		EM_COIL_LEN = 0,										// 検査位置
		EM_COIL_KANRINO,										// 管理No
		EM_COIL_COILNO,											// コイルNo
		EM_COIL_SPM_UMU,										// SPM圧下有無
		LIST_COIL_COLMAX
	};

	enum LIST_DSP_COL {
		EM_DSP_NO = 0,											// No
		EM_DSP_NAME,											// 検査台名
		EM_DSP_KANRINO,											// 管理No
		EM_DSP_DISTANCE,										// 通板位置
		LIST_DSP_COLMAX
	};

	int	m_rdoKadou;
	int	m_rdoUnten;
	int	m_rdoKiki_o;
	int	m_rdoKensa_o;
	int	m_rdoKiki_u;
	int	m_rdoKensa_u;

public:
	afx_msg void OnBnClickedOk();
	afx_msg void OnClose();
	CListBox m_ListBox;
	afx_msg void OnLbnDblclkList1();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnBnClickedBtnStatus();
	afx_msg void OnBnClickedBtnInit();
	afx_msg void OnBnClickedBtnParam();
	afx_msg void OnBnClickedBtnAddcoil();
	afx_msg void OnBnClickedBtnCut();
	afx_msg void OnBnClickedBtnSpeed();
	afx_msg void OnBnClickedChkPlg();
	afx_msg void OnBnClickedBtnSlow();
	afx_msg void OnBnClickedBtnCoilLen();
	afx_msg void OnBnClickedBtnCoilWid();
	CListCtrl m_ListCoil;
	CListCtrl m_ListDsp;
	afx_msg void OnBnClickedChkDef();
	afx_msg void OnBnClickedBtnDefRate();
	afx_msg void OnBnClickedBtnSetcode();
	afx_msg void OnBnClickedBtnDefSend();
	afx_msg void OnBnClickedChkPos();
	afx_msg void OnBnClickedCheckMae();
	afx_msg void OnBnClickedCheckYousetuCut();
	afx_msg void OnBnClickedBtnLstCancelSend();
	afx_msg void OnBnClickedCheckOutDef();
	afx_msg void OnBnClickedBtnAlarmSend();
	afx_msg void OnBnClickedBtnDefcntSend();
	CButton m_chkDsWs;
};
