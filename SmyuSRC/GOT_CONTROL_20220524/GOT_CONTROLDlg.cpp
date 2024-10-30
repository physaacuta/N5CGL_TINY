// GOT_CONTROLDlg.cpp : 実装ファイル
//

#include "stdafx.h"
#include "GOT_CONTROL.h"
#include "GOT_CONTROLDlg.h"

// メモリーリーク検出 手動追加 ----->>>
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif
// <<<------------------------

// CGOT_CONTROLDlg ダイアログ


CGOT_CONTROLDlg::CGOT_CONTROLDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CGOT_CONTROLDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CGOT_CONTROLDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, m_ListBox);
	DDX_Control(pDX, IDC_COMBO1, m_cmbPioName);
}

BEGIN_MESSAGE_MAP(CGOT_CONTROLDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDOK, &CGOT_CONTROLDlg::OnBnClickedOk)
	ON_WM_CLOSE()
	ON_LBN_DBLCLK(IDC_LIST1, &CGOT_CONTROLDlg::OnLbnDblclkList1)
	ON_WM_TIMER()
//	ON_BN_CLICKED(IDC_PLG_FRAMECYCLE_S, &CGOT_CONTROLDlg::OnBnClickedPlgFramecycleS)
	ON_BN_CLICKED(IDC_PLG_FRAMECYCLE_C, &CGOT_CONTROLDlg::OnBnClickedPlgFramecycleC)
	ON_BN_CLICKED(IDC_PLG_SPEED_C0, &CGOT_CONTROLDlg::OnBnClickedPlgSpeedC0)
	ON_BN_CLICKED(IDC_PLG_SPEED_S0, &CGOT_CONTROLDlg::OnBnClickedPlgSpeedS0)
	ON_BN_CLICKED(IDC_PLG_SPEED_C1, &CGOT_CONTROLDlg::OnBnClickedPlgSpeedC1)
	ON_BN_CLICKED(IDC_PLG_SPEED_S1, &CGOT_CONTROLDlg::OnBnClickedPlgSpeedS1)
	ON_BN_CLICKED(IDC_PLG_SPEED_C2, &CGOT_CONTROLDlg::OnBnClickedPlgSpeedC2)
	ON_BN_CLICKED(IDC_PLG_SPEED_S2, &CGOT_CONTROLDlg::OnBnClickedPlgSpeedS2)
	ON_BN_CLICKED(IDC_PLG_SPEED_C3, &CGOT_CONTROLDlg::OnBnClickedPlgSpeedC3)
	ON_BN_CLICKED(IDC_PLG_SPEED_S3, &CGOT_CONTROLDlg::OnBnClickedPlgSpeedS3)
	ON_BN_CLICKED(IDC_PLG_SPEED_C4, &CGOT_CONTROLDlg::OnBnClickedPlgSpeedC4)
	ON_BN_CLICKED(IDC_PLG_SPEED_S4, &CGOT_CONTROLDlg::OnBnClickedPlgSpeedS4)
	ON_BN_CLICKED(IDC_PLG_SPEED_C5, &CGOT_CONTROLDlg::OnBnClickedPlgSpeedC5)
	ON_BN_CLICKED(IDC_PLG_SPEED_S5, &CGOT_CONTROLDlg::OnBnClickedPlgSpeedS5)
	ON_BN_CLICKED(IDC_PLG_SPEED_C6, &CGOT_CONTROLDlg::OnBnClickedPlgSpeedC6)
	ON_BN_CLICKED(IDC_PLG_SPEED_S6, &CGOT_CONTROLDlg::OnBnClickedPlgSpeedS6)
	ON_BN_CLICKED(IDC_PLG_SPEED_C7, &CGOT_CONTROLDlg::OnBnClickedPlgSpeedC7)
	ON_BN_CLICKED(IDC_PLG_SPEED_S7, &CGOT_CONTROLDlg::OnBnClickedPlgSpeedS7)
	ON_BN_CLICKED(IDC_DEGUB_PI, &CGOT_CONTROLDlg::OnBnClickedDegubPi)
	ON_BN_CLICKED(IDC_DEGUB_PO, &CGOT_CONTROLDlg::OnBnClickedDegubPo)
	ON_BN_CLICKED(IDC_PI_00, &CGOT_CONTROLDlg::OnBnClickedPi00)
	ON_BN_CLICKED(IDC_PI_01, &CGOT_CONTROLDlg::OnBnClickedPi01)
	ON_BN_CLICKED(IDC_PI_02, &CGOT_CONTROLDlg::OnBnClickedPi02)
	ON_BN_CLICKED(IDC_PI_03, &CGOT_CONTROLDlg::OnBnClickedPi03)
	ON_BN_CLICKED(IDC_PI_04, &CGOT_CONTROLDlg::OnBnClickedPi04)
	ON_BN_CLICKED(IDC_PI_05, &CGOT_CONTROLDlg::OnBnClickedPi05)
	ON_BN_CLICKED(IDC_PI_06, &CGOT_CONTROLDlg::OnBnClickedPi06)
	ON_BN_CLICKED(IDC_PI_07, &CGOT_CONTROLDlg::OnBnClickedPi07)
	ON_BN_CLICKED(IDC_PI_10, &CGOT_CONTROLDlg::OnBnClickedPi10)
	ON_BN_CLICKED(IDC_PI_11, &CGOT_CONTROLDlg::OnBnClickedPi11)
	ON_BN_CLICKED(IDC_PI_12, &CGOT_CONTROLDlg::OnBnClickedPi12)
	ON_BN_CLICKED(IDC_PI_13, &CGOT_CONTROLDlg::OnBnClickedPi13)
	ON_BN_CLICKED(IDC_PI_14, &CGOT_CONTROLDlg::OnBnClickedPi14)
	ON_BN_CLICKED(IDC_PI_15, &CGOT_CONTROLDlg::OnBnClickedPi15)
	ON_BN_CLICKED(IDC_PI_16, &CGOT_CONTROLDlg::OnBnClickedPi16)
	ON_BN_CLICKED(IDC_PI_17, &CGOT_CONTROLDlg::OnBnClickedPi17)
	ON_BN_CLICKED(IDC_PI_20, &CGOT_CONTROLDlg::OnBnClickedPi20)
	ON_BN_CLICKED(IDC_PI_21, &CGOT_CONTROLDlg::OnBnClickedPi21)
	ON_BN_CLICKED(IDC_PI_22, &CGOT_CONTROLDlg::OnBnClickedPi22)
	ON_BN_CLICKED(IDC_PI_23, &CGOT_CONTROLDlg::OnBnClickedPi23)
	ON_BN_CLICKED(IDC_PI_24, &CGOT_CONTROLDlg::OnBnClickedPi24)
	ON_BN_CLICKED(IDC_PI_25, &CGOT_CONTROLDlg::OnBnClickedPi25)
	ON_BN_CLICKED(IDC_PI_26, &CGOT_CONTROLDlg::OnBnClickedPi26)
	ON_BN_CLICKED(IDC_PI_27, &CGOT_CONTROLDlg::OnBnClickedPi27)
	ON_BN_CLICKED(IDC_PI_30, &CGOT_CONTROLDlg::OnBnClickedPi30)
	ON_BN_CLICKED(IDC_PI_31, &CGOT_CONTROLDlg::OnBnClickedPi31)
	ON_BN_CLICKED(IDC_PI_32, &CGOT_CONTROLDlg::OnBnClickedPi32)
	ON_BN_CLICKED(IDC_PI_33, &CGOT_CONTROLDlg::OnBnClickedPi33)
	ON_BN_CLICKED(IDC_PI_34, &CGOT_CONTROLDlg::OnBnClickedPi34)
	ON_BN_CLICKED(IDC_PI_35, &CGOT_CONTROLDlg::OnBnClickedPi35)
	ON_BN_CLICKED(IDC_PI_36, &CGOT_CONTROLDlg::OnBnClickedPi36)
	ON_BN_CLICKED(IDC_PI_37, &CGOT_CONTROLDlg::OnBnClickedPi37)
	ON_BN_CLICKED(IDC_PI_40, &CGOT_CONTROLDlg::OnBnClickedPi40)
	ON_BN_CLICKED(IDC_PI_41, &CGOT_CONTROLDlg::OnBnClickedPi41)
	ON_BN_CLICKED(IDC_PI_42, &CGOT_CONTROLDlg::OnBnClickedPi42)
	ON_BN_CLICKED(IDC_PI_43, &CGOT_CONTROLDlg::OnBnClickedPi43)
	ON_BN_CLICKED(IDC_PI_44, &CGOT_CONTROLDlg::OnBnClickedPi44)
	ON_BN_CLICKED(IDC_PI_45, &CGOT_CONTROLDlg::OnBnClickedPi45)
	ON_BN_CLICKED(IDC_PI_46, &CGOT_CONTROLDlg::OnBnClickedPi46)
	ON_BN_CLICKED(IDC_PI_47, &CGOT_CONTROLDlg::OnBnClickedPi47)
	ON_BN_CLICKED(IDC_PI_50, &CGOT_CONTROLDlg::OnBnClickedPi50)
	ON_BN_CLICKED(IDC_PI_51, &CGOT_CONTROLDlg::OnBnClickedPi51)
	ON_BN_CLICKED(IDC_PI_52, &CGOT_CONTROLDlg::OnBnClickedPi52)
	ON_BN_CLICKED(IDC_PI_53, &CGOT_CONTROLDlg::OnBnClickedPi53)
	ON_BN_CLICKED(IDC_PI_54, &CGOT_CONTROLDlg::OnBnClickedPi54)
	ON_BN_CLICKED(IDC_PI_55, &CGOT_CONTROLDlg::OnBnClickedPi55)
	ON_BN_CLICKED(IDC_PI_56, &CGOT_CONTROLDlg::OnBnClickedPi56)
	ON_BN_CLICKED(IDC_PI_57, &CGOT_CONTROLDlg::OnBnClickedPi57)
	ON_BN_CLICKED(IDC_PI_60, &CGOT_CONTROLDlg::OnBnClickedPi60)
	ON_BN_CLICKED(IDC_PI_61, &CGOT_CONTROLDlg::OnBnClickedPi61)
	ON_BN_CLICKED(IDC_PI_62, &CGOT_CONTROLDlg::OnBnClickedPi62)
	ON_BN_CLICKED(IDC_PI_63, &CGOT_CONTROLDlg::OnBnClickedPi63)
	ON_BN_CLICKED(IDC_PI_64, &CGOT_CONTROLDlg::OnBnClickedPi64)
	ON_BN_CLICKED(IDC_PI_65, &CGOT_CONTROLDlg::OnBnClickedPi65)
	ON_BN_CLICKED(IDC_PI_66, &CGOT_CONTROLDlg::OnBnClickedPi66)
	ON_BN_CLICKED(IDC_PI_67, &CGOT_CONTROLDlg::OnBnClickedPi67)
	ON_BN_CLICKED(IDC_PI_70, &CGOT_CONTROLDlg::OnBnClickedPi70)
	ON_BN_CLICKED(IDC_PI_71, &CGOT_CONTROLDlg::OnBnClickedPi71)
	ON_BN_CLICKED(IDC_PI_72, &CGOT_CONTROLDlg::OnBnClickedPi72)
	ON_BN_CLICKED(IDC_PI_73, &CGOT_CONTROLDlg::OnBnClickedPi73)
	ON_BN_CLICKED(IDC_PI_74, &CGOT_CONTROLDlg::OnBnClickedPi74)
	ON_BN_CLICKED(IDC_PI_75, &CGOT_CONTROLDlg::OnBnClickedPi75)
	ON_BN_CLICKED(IDC_PI_76, &CGOT_CONTROLDlg::OnBnClickedPi76)
	ON_BN_CLICKED(IDC_PI_77, &CGOT_CONTROLDlg::OnBnClickedPi77)
	ON_BN_CLICKED(IDC_PO_00, &CGOT_CONTROLDlg::OnBnClickedPo00)
	ON_BN_CLICKED(IDC_PO_01, &CGOT_CONTROLDlg::OnBnClickedPo01)
	ON_BN_CLICKED(IDC_PO_02, &CGOT_CONTROLDlg::OnBnClickedPo02)
	ON_BN_CLICKED(IDC_PO_03, &CGOT_CONTROLDlg::OnBnClickedPo03)
	ON_BN_CLICKED(IDC_PO_04, &CGOT_CONTROLDlg::OnBnClickedPo04)
	ON_BN_CLICKED(IDC_PO_05, &CGOT_CONTROLDlg::OnBnClickedPo05)
	ON_BN_CLICKED(IDC_PO_06, &CGOT_CONTROLDlg::OnBnClickedPo06)
	ON_BN_CLICKED(IDC_PO_07, &CGOT_CONTROLDlg::OnBnClickedPo07)
	ON_BN_CLICKED(IDC_PO_10, &CGOT_CONTROLDlg::OnBnClickedPo10)
	ON_BN_CLICKED(IDC_PO_11, &CGOT_CONTROLDlg::OnBnClickedPo11)
	ON_BN_CLICKED(IDC_PO_12, &CGOT_CONTROLDlg::OnBnClickedPo12)
	ON_BN_CLICKED(IDC_PO_13, &CGOT_CONTROLDlg::OnBnClickedPo13)
	ON_BN_CLICKED(IDC_PO_14, &CGOT_CONTROLDlg::OnBnClickedPo14)
	ON_BN_CLICKED(IDC_PO_15, &CGOT_CONTROLDlg::OnBnClickedPo15)
	ON_BN_CLICKED(IDC_PO_16, &CGOT_CONTROLDlg::OnBnClickedPo16)
	ON_BN_CLICKED(IDC_PO_17, &CGOT_CONTROLDlg::OnBnClickedPo17)
	ON_BN_CLICKED(IDC_PO_20, &CGOT_CONTROLDlg::OnBnClickedPo20)
	ON_BN_CLICKED(IDC_PO_21, &CGOT_CONTROLDlg::OnBnClickedPo21)
	ON_BN_CLICKED(IDC_PO_22, &CGOT_CONTROLDlg::OnBnClickedPo22)
	ON_BN_CLICKED(IDC_PO_23, &CGOT_CONTROLDlg::OnBnClickedPo23)
	ON_BN_CLICKED(IDC_PO_24, &CGOT_CONTROLDlg::OnBnClickedPo24)
	ON_BN_CLICKED(IDC_PO_25, &CGOT_CONTROLDlg::OnBnClickedPo25)
	ON_BN_CLICKED(IDC_PO_26, &CGOT_CONTROLDlg::OnBnClickedPo26)
	ON_BN_CLICKED(IDC_PO_27, &CGOT_CONTROLDlg::OnBnClickedPo27)
	ON_BN_CLICKED(IDC_PO_30, &CGOT_CONTROLDlg::OnBnClickedPo30)
	ON_BN_CLICKED(IDC_PO_31, &CGOT_CONTROLDlg::OnBnClickedPo31)
	ON_BN_CLICKED(IDC_PO_32, &CGOT_CONTROLDlg::OnBnClickedPo32)
	ON_BN_CLICKED(IDC_PO_33, &CGOT_CONTROLDlg::OnBnClickedPo33)
	ON_BN_CLICKED(IDC_PO_34, &CGOT_CONTROLDlg::OnBnClickedPo34)
	ON_BN_CLICKED(IDC_PO_35, &CGOT_CONTROLDlg::OnBnClickedPo35)
	ON_BN_CLICKED(IDC_PO_36, &CGOT_CONTROLDlg::OnBnClickedPo36)
	ON_BN_CLICKED(IDC_PO_37, &CGOT_CONTROLDlg::OnBnClickedPo37)
	ON_BN_CLICKED(IDC_PO_40, &CGOT_CONTROLDlg::OnBnClickedPo40)
	ON_BN_CLICKED(IDC_PO_41, &CGOT_CONTROLDlg::OnBnClickedPo41)
	ON_BN_CLICKED(IDC_PO_42, &CGOT_CONTROLDlg::OnBnClickedPo42)
	ON_BN_CLICKED(IDC_PO_43, &CGOT_CONTROLDlg::OnBnClickedPo43)
	ON_BN_CLICKED(IDC_PO_44, &CGOT_CONTROLDlg::OnBnClickedPo44)
	ON_BN_CLICKED(IDC_PO_45, &CGOT_CONTROLDlg::OnBnClickedPo45)
	ON_BN_CLICKED(IDC_PO_46, &CGOT_CONTROLDlg::OnBnClickedPo46)
	ON_BN_CLICKED(IDC_PO_47, &CGOT_CONTROLDlg::OnBnClickedPo47)
	ON_BN_CLICKED(IDC_PO_50, &CGOT_CONTROLDlg::OnBnClickedPo50)
	ON_BN_CLICKED(IDC_PO_51, &CGOT_CONTROLDlg::OnBnClickedPo51)
	ON_BN_CLICKED(IDC_PO_52, &CGOT_CONTROLDlg::OnBnClickedPo52)
	ON_BN_CLICKED(IDC_PO_53, &CGOT_CONTROLDlg::OnBnClickedPo53)
	ON_BN_CLICKED(IDC_PO_54, &CGOT_CONTROLDlg::OnBnClickedPo54)
	ON_BN_CLICKED(IDC_PO_55, &CGOT_CONTROLDlg::OnBnClickedPo55)
	ON_BN_CLICKED(IDC_PO_56, &CGOT_CONTROLDlg::OnBnClickedPo56)
	ON_BN_CLICKED(IDC_PO_57, &CGOT_CONTROLDlg::OnBnClickedPo57)
	ON_BN_CLICKED(IDC_PO_60, &CGOT_CONTROLDlg::OnBnClickedPo60)
	ON_BN_CLICKED(IDC_PO_61, &CGOT_CONTROLDlg::OnBnClickedPo61)
	ON_BN_CLICKED(IDC_PO_62, &CGOT_CONTROLDlg::OnBnClickedPo62)
	ON_BN_CLICKED(IDC_PO_63, &CGOT_CONTROLDlg::OnBnClickedPo63)
	ON_BN_CLICKED(IDC_PO_64, &CGOT_CONTROLDlg::OnBnClickedPo64)
	ON_BN_CLICKED(IDC_PO_65, &CGOT_CONTROLDlg::OnBnClickedPo65)
	ON_BN_CLICKED(IDC_PO_66, &CGOT_CONTROLDlg::OnBnClickedPo66)
	ON_BN_CLICKED(IDC_PO_67, &CGOT_CONTROLDlg::OnBnClickedPo67)
	ON_BN_CLICKED(IDC_PO_70, &CGOT_CONTROLDlg::OnBnClickedPo70)
	ON_BN_CLICKED(IDC_PO_71, &CGOT_CONTROLDlg::OnBnClickedPo71)
	ON_BN_CLICKED(IDC_PO_72, &CGOT_CONTROLDlg::OnBnClickedPo72)
	ON_BN_CLICKED(IDC_PO_73, &CGOT_CONTROLDlg::OnBnClickedPo73)
	ON_BN_CLICKED(IDC_PO_74, &CGOT_CONTROLDlg::OnBnClickedPo74)
	ON_BN_CLICKED(IDC_PO_75, &CGOT_CONTROLDlg::OnBnClickedPo75)
	ON_BN_CLICKED(IDC_PO_76, &CGOT_CONTROLDlg::OnBnClickedPo76)
	ON_BN_CLICKED(IDC_PO_77, &CGOT_CONTROLDlg::OnBnClickedPo77)
	ON_BN_CLICKED(IDC_GOT_START, &CGOT_CONTROLDlg::OnBnClickedGotStart)
	ON_BN_CLICKED(IDC_GOT_STOP, &CGOT_CONTROLDlg::OnBnClickedGotStop)
//	ON_CBN_EDITCHANGE(IDC_COMBO1, &CGOT_CONTROLDlg::OnCbnEditchangeCombo1)
//	ON_CBN_EDITUPDATE(IDC_COMBO1, &CGOT_CONTROLDlg::OnCbnEditupdateCombo1)
ON_BN_CLICKED(IDC_GOT_CHANGE, &CGOT_CONTROLDlg::OnBnClickedGotChange)
ON_BN_CLICKED(IDC_PLG_FRAME_RESET, &CGOT_CONTROLDlg::OnBnClickedPlgFrameReset)
END_MESSAGE_MAP()


// CGOT_CONTROLDlg メッセージ ハンドラ

BOOL CGOT_CONTROLDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// このダイアログのアイコンを設定します。アプリケーションのメイン ウィンドウがダイアログでない場合、
	//  Framework は、この設定を自動的に行います。
	SetIcon(m_hIcon, TRUE);			// 大きいアイコンの設定
	SetIcon(m_hIcon, FALSE);		// 小さいアイコンの設定

	// TODO: 初期化をここに追加します。
// 手動追加 -------------------->>>
	// 画面位置 (サイズはいまのまま)
	//::SetWindowPos(m_hWnd, NULL, 1, 1, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
	
	// 実行環境判別用
	CString		cTitle;
	GetWindowText(cTitle);
#ifdef _M_X64			// x64 コンパイル
	cTitle += " x64";
#else					// x86 コンパイル
	cTitle += " x86";
#endif
	SetWindowText(cTitle);

	// ×ボタン無効
	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL) pSysMenu->EnableMenuItem(SC_CLOSE, MF_GRAYED);
	
	// メインスレッド開始
	unsigned int tid;	// スレッドID
	_beginthreadex(NULL, 0, MainThread, (LPVOID)this, 0, &tid);
// <<<-------------------------------
	return TRUE;  // フォーカスをコントロールに設定した場合を除き、TRUE を返します。
}

// ダイアログに最小化ボタンを追加する場合、アイコンを描画するための
//  下のコードが必要です。ドキュメント/ビュー モデルを使う MFC アプリケーションの場合、
//  これは、Framework によって自動的に設定されます。

void CGOT_CONTROLDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 描画のデバイス コンテキスト

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// クライアントの四角形領域内の中央
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// アイコンの描画
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// ユーザーが最小化したウィンドウをドラッグしているときに表示するカーソルを取得するために、
//  システムがこの関数を呼び出します。
HCURSOR CGOT_CONTROLDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


// これ以降 手動追加 -------------------->>>

//￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣
// ×ボタン
//＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿
void CGOT_CONTROLDlg::OnClose()
{
	//CDialog::OnClose();		// 既存の処理をヤメ
}

//￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣
// 強制終了ボタン
//＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿
void CGOT_CONTROLDlg::OnBnClickedOk()
{
	if( IDYES == MessageBox("終了しますか？", "重要なお知らせ", MB_YESNO | MB_DEFBUTTON2 | MB_ICONQUESTION) ) {
		LOG(em_MSG), "[Dlg] 強制終了ボタン");
		COMMON_QUE que;
		que.nEventNo = FACT_KS_END;
		que.nLineNo = LINE_ID;			
		send_mail(getmytaskname(), ".", &que);
	}
}

//￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣
// メモ帳起動
//＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿
void CGOT_CONTROLDlg::OnLbnDblclkList1()
{
	// ListBoxコントロールのプロパティは、ソート禁止にしておく事
	// ListBoxコントロールのダブルクリックイベントを追加
	mcls_pLog->ShowEdit();
}

//------------------------------------------
// 各プロセス初期化処理
//------------------------------------------
void CGOT_CONTROLDlg::InitProcess()
{
	//// タスク初期化処理
	if( 0 != task_init(GOT_CONTROL, 0) ) {
		syslog(SYSNO_TASK_INIT_ERR, "[%s]", GOT_CONTROL);
		_ASSERT(0);
		exit(-9);		// 自プロセス強制終了
		return;
	}

	//// LogFileManagerインスタンス生成 (これ以降 ログ出力可能)
	setlocale(LC_ALL, "Japanese");	// これが無いと日本語パスを含むフォルダの場合、保存できない。(ライブラリの中で実行するべきか迷っているが、変更するとタスク全体に影響するので、とりあえずここで設定)
	mcls_pLog = new LogFileManager(getmytaskname(), &m_ListBox);
	LOG(em_MSG), "[Dlg] タスク起動");
	LOG(em_MSG), "[Dlg] 各プロセス開始処理 実行中・・・");

	//// メインインスタンス 開始処理
	mcls_MI.SetLogMgr(mcls_pLog);
	mcls_MI.Init();
	

	//// PIOコンボボックスにセッション名追加
	for(int ii=0; ii<mcls_MI.gcls_pPio->GetMaxBood(); ii++) {
		m_cmbPioName.AddString( mcls_MI.gcls_pPio->GetSmyuName(ii) );
	}
	m_cmbPioName.SetCurSel(0);				// 0番目を初期選択とする


	//// 画面更新用タイマ生成
	SetTimer(1, 500, NULL);

	//// メインスレッドの実行を行う
	LOG(em_MSG), "[Dlg] 各プロセス開始処理 完了");
	mcls_MI.Start();	
	return;
}

//￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣
// 各プロセス開放処理
//＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿
void CGOT_CONTROLDlg::ExitProcess()
{
	//// メインインスタンス 終了処理
	KillTimer(1);			// 画面更新タイマ
	mcls_MI.Stop();
	mcls_MI.Exit();

	//// ログ リストボックス表示処理終了
	mcls_pLog->SetListOut(false);					// これ以降 リストボックスへのログ表示禁止

	//// ログクラス開放
	LOG(em_MSG), "[Dlg] タスク終了");
	if(NULL != mcls_pLog){
		delete mcls_pLog;
		mcls_pLog = NULL;
	}
	
	// タスク終了処理
	task_exit();

	// 画面の終了要求
	PostMessage(WM_QUIT);
}

//------------------------------------------
// メインスレッド DlgThread
// LPVOID param ダイヤログポインタ
//------------------------------------------
unsigned WINAPI CGOT_CONTROLDlg::MainThread(void* param)
{
	CGOT_CONTROLDlg* p = (CGOT_CONTROLDlg *)param;
	COMMON_QUE que;			// 受信構造体

	//// 各プロセス初期化処理 (これ以前はログ出力禁止)
	p->InitProcess();

	//// 起動要因取り込み ループ開始
	while(true) {
		recv_mail(&que);			// 起動要因取り込み

		switch(que.nEventNo) {

//-----------------------------------------------------------------------------------------------
		case FACT_GOT_CONTROL_01:								// 開始停止指示 (fl.data[0]= 0:停止 1:開始)	[← GOT_CAMERA]
			pLOG(em_MSG), "[MAIL] 開始停止指示 (FACT_GOT_CONTROL_01) [%s]", que.fl.data[0]==0?"停止":"開始"); 
			// メインインスタンスへ通知
			if(que.fl.data[0]==0) 	p->mcls_MI.SetDeliveryMail( E_DEF_ML_SMYU_STOP, &que);
			else					p->mcls_MI.SetDeliveryMail( E_DEF_ML_SMYU_START, &que);
			break;


		case FACT_KS_END:									// 終了要求
			// 各プロセス終了処理
			p->ExitProcess();
			return 0;

		default:
			syslog(SYSNO_MAILNO_ERR, "[EventNo=%d]", que.nEventNo);
			break;
		}
	}

	return 0;
}

//￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣
// 画面更新タイマ
//＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿
void CGOT_CONTROLDlg::OnTimer(UINT_PTR nIDEvent)
{
	CString ss;
	int ii;


	//================================================
	//// PLG情報
	PlgControlManager::TYP_SAVEDATA const& plg = mcls_MI.gcls_pPlg->GetSaveData();
	// フレーム出力周期
	SetDlgItemInt(IDC_PLG_FRAMECYCLE_D, plg.nFrameCycle);	
	// ライン速度
	int txtSpeedDsp[SMYU_PLG_MAX_CHANNEL]	= { IDC_PLG_SPEED_D0,IDC_PLG_SPEED_D1,IDC_PLG_SPEED_D2,IDC_PLG_SPEED_D3,	IDC_PLG_SPEED_D4,IDC_PLG_SPEED_D5,IDC_PLG_SPEED_D6,IDC_PLG_SPEED_D7 };
	int txtLenDsp[SMYU_PLG_MAX_CHANNEL]		= { IDC_PLG_LEN_0,IDC_PLG_LEN_1,IDC_PLG_LEN_2,IDC_PLG_LEN_3,	IDC_PLG_LEN_4,IDC_PLG_LEN_5,IDC_PLG_LEN_6,IDC_PLG_LEN_7 };
	for(ii=0; ii<SMYU_PLG_MAX_CHANNEL; ii++) {
		SetDlgItemInt(txtSpeedDsp[ii], plg.nSpeed[ii]);	
		ss.Format("%0.3f", mcls_MI.gcls_pPlg->GetLength(ii)/1000.0);	 SetDlgItemText(txtLenDsp[ii], ss);
	}
	ss.Format("FrameNo=%d", mcls_MI.gcls_pPlg->GetFrameNo());
	SetDlgItemText(IDC_FRAME_NO, ss);

	//================================================
	//// PIO情報
	int txtPi[SMYU_PIO_MAX_PORT][8] = 
		{ {IDC_PI_00, IDC_PI_01, IDC_PI_02, IDC_PI_03, IDC_PI_04, IDC_PI_05, IDC_PI_06, IDC_PI_07},
		  {IDC_PI_10, IDC_PI_11, IDC_PI_12, IDC_PI_13, IDC_PI_14, IDC_PI_15, IDC_PI_16, IDC_PI_17},
		  {IDC_PI_20, IDC_PI_21, IDC_PI_22, IDC_PI_23, IDC_PI_24, IDC_PI_25, IDC_PI_26, IDC_PI_27},
		  {IDC_PI_30, IDC_PI_31, IDC_PI_32, IDC_PI_33, IDC_PI_34, IDC_PI_35, IDC_PI_36, IDC_PI_37},
	
		  {IDC_PI_40, IDC_PI_41, IDC_PI_42, IDC_PI_43, IDC_PI_44, IDC_PI_45, IDC_PI_46, IDC_PI_47},
		  {IDC_PI_50, IDC_PI_51, IDC_PI_52, IDC_PI_53, IDC_PI_54, IDC_PI_55, IDC_PI_56, IDC_PI_57},
		  {IDC_PI_60, IDC_PI_61, IDC_PI_62, IDC_PI_63, IDC_PI_64, IDC_PI_65, IDC_PI_66, IDC_PI_67},
		  {IDC_PI_70, IDC_PI_71, IDC_PI_72, IDC_PI_73, IDC_PI_74, IDC_PI_75, IDC_PI_76, IDC_PI_77} };

	int txtPo[SMYU_PIO_MAX_PORT][8] = 
		{ {IDC_PO_00, IDC_PO_01, IDC_PO_02, IDC_PO_03, IDC_PO_04, IDC_PO_05, IDC_PO_06, IDC_PO_07},
		  {IDC_PO_10, IDC_PO_11, IDC_PO_12, IDC_PO_13, IDC_PO_14, IDC_PO_15, IDC_PO_16, IDC_PO_17},
		  {IDC_PO_20, IDC_PO_21, IDC_PO_22, IDC_PO_23, IDC_PO_24, IDC_PO_25, IDC_PO_26, IDC_PO_27},
		  {IDC_PO_30, IDC_PO_31, IDC_PO_32, IDC_PO_33, IDC_PO_34, IDC_PO_35, IDC_PO_36, IDC_PO_37},

		  {IDC_PO_40, IDC_PO_41, IDC_PO_42, IDC_PO_43, IDC_PO_44, IDC_PO_45, IDC_PO_46, IDC_PO_47},
		  {IDC_PO_50, IDC_PO_51, IDC_PO_52, IDC_PO_53, IDC_PO_54, IDC_PO_55, IDC_PO_56, IDC_PO_57},
		  {IDC_PO_60, IDC_PO_61, IDC_PO_62, IDC_PO_63, IDC_PO_64, IDC_PO_65, IDC_PO_66, IDC_PO_67},
		  {IDC_PO_70, IDC_PO_71, IDC_PO_72, IDC_PO_73, IDC_PO_74, IDC_PO_75, IDC_PO_76, IDC_PO_77} };

	//// 選択中ボードIndex取得
	int nBood = m_cmbPioName.GetCurSel();
	if( -1 != nBood) {
		//// 入力タイトル設定
		for(int ii=0; ii<SMYU_PIO_MAX_PORT; ii++) {
			for(int jj=0; jj<8; jj++) {
				CWnd::CheckDlgButton(txtPi[ii][jj], mcls_MI.gcls_pPio->GetValPi(nBood, ii, jj) );
				SetDlgItemText(txtPi[ii][jj], mcls_MI.gcls_pPio->GetBitName(nBood, ii, jj, 0));
			}
		}

		//// 出力タイトル設定
		for(int ii=0; ii<SMYU_PIO_MAX_PORT; ii++) {
			for(int jj=0; jj<8; jj++) {
				CWnd::CheckDlgButton(txtPo[ii][jj], mcls_MI.gcls_pPio->GetValPo(nBood, ii, jj) );
				SetDlgItemText(txtPo[ii][jj], mcls_MI.gcls_pPio->GetBitName(nBood, ii, jj, 1));
			}
		}

	}


	CDialog::OnTimer(nIDEvent);
}

// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// シミュレータ 制御

void CGOT_CONTROLDlg::OnBnClickedGotStart()
{
	COMMON_QUE que;
	que.nEventNo = FACT_GOT_CONTROL_01;
	que.fl.data[0] = 1;
	send_mail(GOT_CONTROL, ".", &que);
}
void CGOT_CONTROLDlg::OnBnClickedGotStop()
{
	COMMON_QUE que;
	que.nEventNo = FACT_GOT_CONTROL_01;
	que.fl.data[0] = 0;
	send_mail(GOT_CONTROL, ".", &que);
}
void CGOT_CONTROLDlg::OnBnClickedGotChange()
{
	bool bMode =  ( BST_CHECKED == CWnd::IsDlgButtonChecked(IDC_PLG_SPEED_S0)) ? false : true;

	for(int ii=0; ii<SMYU_PLG_MAX_CHANNEL; ii++) {
		mcls_MI.gcls_pPlg->SetTimer_SpeedCycle(ii, bMode);
	}
}



// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// PLG

//￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣
// フレーム出力周期 制御
//＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿
void CGOT_CONTROLDlg::OnBnClickedPlgFramecycleC()
{
	mcls_MI.gcls_pPlg->SetSaveData().nFrameCycle  = GetDlgItemInt( IDC_PLG_FRAMECYCLE_W);


	// ライン速度変更
	double dWk = mcls_MI.gcls_pPlg->GetSetting().dResY[0]*mcls_MI.gcls_pPlg->GetSetting().nFrameSizeY;
	int nWk = (int)(dWk * (1000.0 / mcls_MI.gcls_pPlg->SetSaveData().nFrameCycle) / 1000.0 * 60.0);
	SetDlgItemInt(IDC_PLG_SPEED_D0, nWk ); 
	mcls_MI.gcls_pPlg->SetSaveData().nSpeed[0] = nWk;

	if(1==mcls_MI.gcls_pPlg->GetSetting().nSamplingEnable) {
		if ( BST_CHECKED == CWnd::IsDlgButtonChecked(IDC_PLG_SPEED_S0)) mcls_MI.gcls_pPlg->SetTimer_SpeedCycle(0, true);
	}
}

//￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣
// ライン速度 制御
//＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿
//0
void CGOT_CONTROLDlg::OnBnClickedPlgSpeedC0()
{
	mcls_MI.gcls_pPlg->SetSaveData().nSpeed[0]  = GetDlgItemInt( IDC_PLG_SPEED_W0);

	// 周期変更
	double dWk =  mcls_MI.gcls_pPlg->SetSaveData().nSpeed[0] / 60.0 * 1000.0;
	dWk  = dWk / ( mcls_MI.gcls_pPlg->GetSetting().dResY[0]*mcls_MI.gcls_pPlg->GetSetting().nFrameSizeY);
	int nWk = (int)(1.0 / dWk * 1000.0);
	SetDlgItemInt(IDC_PLG_FRAMECYCLE_D, nWk ); 
	mcls_MI.gcls_pPlg->SetSaveData().nFrameCycle = nWk;

	if ( BST_CHECKED == CWnd::IsDlgButtonChecked(IDC_PLG_SPEED_S0))	mcls_MI.gcls_pPlg->SetTimer_SpeedCycle(0, true);
}
void CGOT_CONTROLDlg::OnBnClickedPlgSpeedS0()
{
//	if(1==mcls_MI.gcls_pPlg->GetSetting().nSamplingEnable) {
//		if ( BST_CHECKED == CWnd::IsDlgButtonChecked(IDC_PLG_SPEED_S0))	mcls_MI.gcls_pPlg->SetTimer_FrameCycle(true);
//		else															mcls_MI.gcls_pPlg->SetTimer_FrameCycle(false);
//	} else {

		if ( BST_CHECKED == CWnd::IsDlgButtonChecked(IDC_PLG_SPEED_S0))	mcls_MI.gcls_pPlg->SetTimer_SpeedCycle(0, true);
		else															mcls_MI.gcls_pPlg->SetTimer_SpeedCycle(0, false);
//	}
}

//1
void CGOT_CONTROLDlg::OnBnClickedPlgSpeedC1()
{
	mcls_MI.gcls_pPlg->SetSaveData().nSpeed[1]  = GetDlgItemInt( IDC_PLG_SPEED_W1);
	if ( BST_CHECKED == CWnd::IsDlgButtonChecked(IDC_PLG_SPEED_S1))	mcls_MI.gcls_pPlg->SetTimer_SpeedCycle(1, true);
}
void CGOT_CONTROLDlg::OnBnClickedPlgSpeedS1()
{
	if ( BST_CHECKED == CWnd::IsDlgButtonChecked(IDC_PLG_SPEED_S1))	mcls_MI.gcls_pPlg->SetTimer_SpeedCycle(1, true);
	else															mcls_MI.gcls_pPlg->SetTimer_SpeedCycle(1, false);
}
//2
void CGOT_CONTROLDlg::OnBnClickedPlgSpeedC2()
{
	mcls_MI.gcls_pPlg->SetSaveData().nSpeed[2]  = GetDlgItemInt( IDC_PLG_SPEED_W2);
	if ( BST_CHECKED == CWnd::IsDlgButtonChecked(IDC_PLG_SPEED_S2))	mcls_MI.gcls_pPlg->SetTimer_SpeedCycle(2, true);
}
void CGOT_CONTROLDlg::OnBnClickedPlgSpeedS2()
{
	if ( BST_CHECKED == CWnd::IsDlgButtonChecked(IDC_PLG_SPEED_S2))	mcls_MI.gcls_pPlg->SetTimer_SpeedCycle(2, true);
	else															mcls_MI.gcls_pPlg->SetTimer_SpeedCycle(2, false);
}
//3
void CGOT_CONTROLDlg::OnBnClickedPlgSpeedC3()
{
	mcls_MI.gcls_pPlg->SetSaveData().nSpeed[3]  = GetDlgItemInt( IDC_PLG_SPEED_W3);
	if ( BST_CHECKED == CWnd::IsDlgButtonChecked(IDC_PLG_SPEED_S3))	mcls_MI.gcls_pPlg->SetTimer_SpeedCycle(3, true);
}
void CGOT_CONTROLDlg::OnBnClickedPlgSpeedS3()
{
	if ( BST_CHECKED == CWnd::IsDlgButtonChecked(IDC_PLG_SPEED_S3))	mcls_MI.gcls_pPlg->SetTimer_SpeedCycle(3, true);
	else															mcls_MI.gcls_pPlg->SetTimer_SpeedCycle(3, false);
}
//4
void CGOT_CONTROLDlg::OnBnClickedPlgSpeedC4()
{
	mcls_MI.gcls_pPlg->SetSaveData().nSpeed[4]  = GetDlgItemInt( IDC_PLG_SPEED_W4);
	if ( BST_CHECKED == CWnd::IsDlgButtonChecked(IDC_PLG_SPEED_S4))	mcls_MI.gcls_pPlg->SetTimer_SpeedCycle(4, true);
}
void CGOT_CONTROLDlg::OnBnClickedPlgSpeedS4()
{
	if ( BST_CHECKED == CWnd::IsDlgButtonChecked(IDC_PLG_SPEED_S4))	mcls_MI.gcls_pPlg->SetTimer_SpeedCycle(4, true);
	else															mcls_MI.gcls_pPlg->SetTimer_SpeedCycle(4, false);
}
//5
void CGOT_CONTROLDlg::OnBnClickedPlgSpeedC5()
{
	mcls_MI.gcls_pPlg->SetSaveData().nSpeed[5]  = GetDlgItemInt( IDC_PLG_SPEED_W5);
	if ( BST_CHECKED == CWnd::IsDlgButtonChecked(IDC_PLG_SPEED_S5))	mcls_MI.gcls_pPlg->SetTimer_SpeedCycle(5, true);
}
void CGOT_CONTROLDlg::OnBnClickedPlgSpeedS5()
{
	if ( BST_CHECKED == CWnd::IsDlgButtonChecked(IDC_PLG_SPEED_S5))	mcls_MI.gcls_pPlg->SetTimer_SpeedCycle(5, true);
	else															mcls_MI.gcls_pPlg->SetTimer_SpeedCycle(5, false);
}
//6
void CGOT_CONTROLDlg::OnBnClickedPlgSpeedC6()
{
	mcls_MI.gcls_pPlg->SetSaveData().nSpeed[6]  = GetDlgItemInt( IDC_PLG_SPEED_W6);
	if ( BST_CHECKED == CWnd::IsDlgButtonChecked(IDC_PLG_SPEED_S6))	mcls_MI.gcls_pPlg->SetTimer_SpeedCycle(6, true);
}
void CGOT_CONTROLDlg::OnBnClickedPlgSpeedS6()
{
	if ( BST_CHECKED == CWnd::IsDlgButtonChecked(IDC_PLG_SPEED_S6))	mcls_MI.gcls_pPlg->SetTimer_SpeedCycle(6, true);
	else															mcls_MI.gcls_pPlg->SetTimer_SpeedCycle(6, false);
}
//7
void CGOT_CONTROLDlg::OnBnClickedPlgSpeedC7()
{
	mcls_MI.gcls_pPlg->SetSaveData().nSpeed[7]  = GetDlgItemInt( IDC_PLG_SPEED_W7);
	if ( BST_CHECKED == CWnd::IsDlgButtonChecked(IDC_PLG_SPEED_S7))	mcls_MI.gcls_pPlg->SetTimer_SpeedCycle(7, true);
}
void CGOT_CONTROLDlg::OnBnClickedPlgSpeedS7()
{
	if ( BST_CHECKED == CWnd::IsDlgButtonChecked(IDC_PLG_SPEED_S7))	mcls_MI.gcls_pPlg->SetTimer_SpeedCycle(7, true);
	else															mcls_MI.gcls_pPlg->SetTimer_SpeedCycle(7, false);
}

// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// PI
//￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣
// デバック切替
//＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿
void CGOT_CONTROLDlg::OnBnClickedDegubPi()
{
	if ( BST_CHECKED == CWnd::IsDlgButtonChecked(IDC_DEGUB_PI) )	mcls_MI.gcls_pPio->SetDebug_Pi(true);
	else															mcls_MI.gcls_pPio->SetDebug_Pi(false);
}
void CGOT_CONTROLDlg::OnBnClickedDegubPo()
{
	if ( BST_CHECKED == CWnd::IsDlgButtonChecked(IDC_DEGUB_PO) )	mcls_MI.gcls_pPio->SetDebug_Po(true);
	else															mcls_MI.gcls_pPio->SetDebug_Po(false);
}
//--------------
void CGOT_CONTROLDlg::ManualPio(int pio, int port, int bit, bool flg)
{
	if ( 0 == pio && BST_CHECKED != CWnd::IsDlgButtonChecked(IDC_DEGUB_PI) ) return;
	if ( 1 == pio && BST_CHECKED != CWnd::IsDlgButtonChecked(IDC_DEGUB_PO) ) return;

	int nBood = m_cmbPioName.GetCurSel();
	if(-1 == nBood) return;

	if(0 == pio) {
		mcls_MI.gcls_pPio->SetValPi(nBood, port, bit, flg);
		LOG(em_INF), "[Degub] <PI> bood=%d, port=%d, bit=%d, val=%d", nBood, port, bit, flg);
	} else {
		mcls_MI.gcls_pPio->SetValPo(nBood, port, bit, flg);
		LOG(em_INF), "[Degub] <PO> bood=%d, port=%d, bit=%d, val=%d", nBood, port, bit, flg);
	}
}

void CGOT_CONTROLDlg::OnBnClickedPi00()
{ ManualPio(0, 0, 0, BST_CHECKED == CWnd::IsDlgButtonChecked(IDC_PI_00) ? true: false); }
void CGOT_CONTROLDlg::OnBnClickedPi01()
{ ManualPio(0, 0, 1, BST_CHECKED == CWnd::IsDlgButtonChecked(IDC_PI_01) ? true: false); }
void CGOT_CONTROLDlg::OnBnClickedPi02()
{ ManualPio(0, 0, 2, BST_CHECKED == CWnd::IsDlgButtonChecked(IDC_PI_02) ? true: false); }
void CGOT_CONTROLDlg::OnBnClickedPi03()
{ ManualPio(0, 0, 3, BST_CHECKED == CWnd::IsDlgButtonChecked(IDC_PI_03) ? true: false); }
void CGOT_CONTROLDlg::OnBnClickedPi04()
{ ManualPio(0, 0, 4, BST_CHECKED == CWnd::IsDlgButtonChecked(IDC_PI_04) ? true: false); }
void CGOT_CONTROLDlg::OnBnClickedPi05()
{ ManualPio(0, 0, 5, BST_CHECKED == CWnd::IsDlgButtonChecked(IDC_PI_05) ? true: false); }
void CGOT_CONTROLDlg::OnBnClickedPi06()
{ ManualPio(0, 0, 6, BST_CHECKED == CWnd::IsDlgButtonChecked(IDC_PI_06) ? true: false); }
void CGOT_CONTROLDlg::OnBnClickedPi07()
{ ManualPio(0, 0, 7, BST_CHECKED == CWnd::IsDlgButtonChecked(IDC_PI_07) ? true: false); }

void CGOT_CONTROLDlg::OnBnClickedPi10()
{ ManualPio(0, 1, 0, BST_CHECKED == CWnd::IsDlgButtonChecked(IDC_PI_10) ? true: false); }
void CGOT_CONTROLDlg::OnBnClickedPi11()
{ ManualPio(0, 1, 1, BST_CHECKED == CWnd::IsDlgButtonChecked(IDC_PI_11) ? true: false); }
void CGOT_CONTROLDlg::OnBnClickedPi12()
{ ManualPio(0, 1, 2, BST_CHECKED == CWnd::IsDlgButtonChecked(IDC_PI_12) ? true: false); }
void CGOT_CONTROLDlg::OnBnClickedPi13()
{ ManualPio(0, 1, 3, BST_CHECKED == CWnd::IsDlgButtonChecked(IDC_PI_13) ? true: false); }
void CGOT_CONTROLDlg::OnBnClickedPi14()
{ ManualPio(0, 1, 4, BST_CHECKED == CWnd::IsDlgButtonChecked(IDC_PI_14) ? true: false); }
void CGOT_CONTROLDlg::OnBnClickedPi15()
{ ManualPio(0, 1, 5, BST_CHECKED == CWnd::IsDlgButtonChecked(IDC_PI_15) ? true: false); }
void CGOT_CONTROLDlg::OnBnClickedPi16()
{ ManualPio(0, 1, 6, BST_CHECKED == CWnd::IsDlgButtonChecked(IDC_PI_16) ? true: false); }
void CGOT_CONTROLDlg::OnBnClickedPi17()
{ ManualPio(0, 1, 7, BST_CHECKED == CWnd::IsDlgButtonChecked(IDC_PI_17) ? true: false); }

void CGOT_CONTROLDlg::OnBnClickedPi20()
{ ManualPio(0, 2, 0, BST_CHECKED == CWnd::IsDlgButtonChecked(IDC_PI_20) ? true: false); }
void CGOT_CONTROLDlg::OnBnClickedPi21()
{ ManualPio(0, 2, 1, BST_CHECKED == CWnd::IsDlgButtonChecked(IDC_PI_21) ? true: false); }
void CGOT_CONTROLDlg::OnBnClickedPi22()
{ ManualPio(0, 2, 2, BST_CHECKED == CWnd::IsDlgButtonChecked(IDC_PI_22) ? true: false); }
void CGOT_CONTROLDlg::OnBnClickedPi23()
{ ManualPio(0, 2, 3, BST_CHECKED == CWnd::IsDlgButtonChecked(IDC_PI_23) ? true: false); }
void CGOT_CONTROLDlg::OnBnClickedPi24()
{ ManualPio(0, 2, 4, BST_CHECKED == CWnd::IsDlgButtonChecked(IDC_PI_24) ? true: false); }
void CGOT_CONTROLDlg::OnBnClickedPi25()
{ ManualPio(0, 2, 5, BST_CHECKED == CWnd::IsDlgButtonChecked(IDC_PI_25) ? true: false); }
void CGOT_CONTROLDlg::OnBnClickedPi26()
{ ManualPio(0, 2, 6, BST_CHECKED == CWnd::IsDlgButtonChecked(IDC_PI_26) ? true: false); }
void CGOT_CONTROLDlg::OnBnClickedPi27()
{ ManualPio(0, 2, 7, BST_CHECKED == CWnd::IsDlgButtonChecked(IDC_PI_27) ? true: false); }

void CGOT_CONTROLDlg::OnBnClickedPi30()
{ ManualPio(0, 3, 0, BST_CHECKED == CWnd::IsDlgButtonChecked(IDC_PI_30) ? true: false); }
void CGOT_CONTROLDlg::OnBnClickedPi31()
{ ManualPio(0, 3, 1, BST_CHECKED == CWnd::IsDlgButtonChecked(IDC_PI_31) ? true: false); }
void CGOT_CONTROLDlg::OnBnClickedPi32()
{ ManualPio(0, 3, 2, BST_CHECKED == CWnd::IsDlgButtonChecked(IDC_PI_32) ? true: false); }
void CGOT_CONTROLDlg::OnBnClickedPi33()
{ ManualPio(0, 3, 3, BST_CHECKED == CWnd::IsDlgButtonChecked(IDC_PI_33) ? true: false); }
void CGOT_CONTROLDlg::OnBnClickedPi34()
{ ManualPio(0, 3, 4, BST_CHECKED == CWnd::IsDlgButtonChecked(IDC_PI_34) ? true: false); }
void CGOT_CONTROLDlg::OnBnClickedPi35()
{ ManualPio(0, 3, 5, BST_CHECKED == CWnd::IsDlgButtonChecked(IDC_PI_35) ? true: false); }
void CGOT_CONTROLDlg::OnBnClickedPi36()
{ ManualPio(0, 3, 6, BST_CHECKED == CWnd::IsDlgButtonChecked(IDC_PI_36) ? true: false); }
void CGOT_CONTROLDlg::OnBnClickedPi37()
{ ManualPio(0, 3, 7, BST_CHECKED == CWnd::IsDlgButtonChecked(IDC_PI_37) ? true: false); }

void CGOT_CONTROLDlg::OnBnClickedPi40()
{ ManualPio(0, 4, 0, BST_CHECKED == CWnd::IsDlgButtonChecked(IDC_PI_40) ? true: false); }
void CGOT_CONTROLDlg::OnBnClickedPi41()
{ ManualPio(0, 4, 1, BST_CHECKED == CWnd::IsDlgButtonChecked(IDC_PI_41) ? true: false); }
void CGOT_CONTROLDlg::OnBnClickedPi42()
{ ManualPio(0, 4, 2, BST_CHECKED == CWnd::IsDlgButtonChecked(IDC_PI_42) ? true: false); }
void CGOT_CONTROLDlg::OnBnClickedPi43()
{ ManualPio(0, 4, 3, BST_CHECKED == CWnd::IsDlgButtonChecked(IDC_PI_43) ? true: false); }
void CGOT_CONTROLDlg::OnBnClickedPi44()
{ ManualPio(0, 4, 4, BST_CHECKED == CWnd::IsDlgButtonChecked(IDC_PI_44) ? true: false); }
void CGOT_CONTROLDlg::OnBnClickedPi45()
{ ManualPio(0, 4, 5, BST_CHECKED == CWnd::IsDlgButtonChecked(IDC_PI_45) ? true: false); }
void CGOT_CONTROLDlg::OnBnClickedPi46()
{ ManualPio(0, 4, 6, BST_CHECKED == CWnd::IsDlgButtonChecked(IDC_PI_46) ? true: false); }
void CGOT_CONTROLDlg::OnBnClickedPi47()
{ ManualPio(0, 4, 7, BST_CHECKED == CWnd::IsDlgButtonChecked(IDC_PI_47) ? true: false); }

void CGOT_CONTROLDlg::OnBnClickedPi50()
{ ManualPio(0, 5, 0, BST_CHECKED == CWnd::IsDlgButtonChecked(IDC_PI_50) ? true: false); }
void CGOT_CONTROLDlg::OnBnClickedPi51()
{ ManualPio(0, 5, 1, BST_CHECKED == CWnd::IsDlgButtonChecked(IDC_PI_51) ? true: false); }
void CGOT_CONTROLDlg::OnBnClickedPi52()
{ ManualPio(0, 5, 2, BST_CHECKED == CWnd::IsDlgButtonChecked(IDC_PI_52) ? true: false); }
void CGOT_CONTROLDlg::OnBnClickedPi53()
{ ManualPio(0, 5, 3, BST_CHECKED == CWnd::IsDlgButtonChecked(IDC_PI_53) ? true: false); }
void CGOT_CONTROLDlg::OnBnClickedPi54()
{ ManualPio(0, 5, 4, BST_CHECKED == CWnd::IsDlgButtonChecked(IDC_PI_54) ? true: false); }
void CGOT_CONTROLDlg::OnBnClickedPi55()
{ ManualPio(0, 5, 5, BST_CHECKED == CWnd::IsDlgButtonChecked(IDC_PI_55) ? true: false); }
void CGOT_CONTROLDlg::OnBnClickedPi56()
{ ManualPio(0, 5, 6, BST_CHECKED == CWnd::IsDlgButtonChecked(IDC_PI_56) ? true: false); }
void CGOT_CONTROLDlg::OnBnClickedPi57()
{ ManualPio(0, 5, 7, BST_CHECKED == CWnd::IsDlgButtonChecked(IDC_PI_57) ? true: false); }

void CGOT_CONTROLDlg::OnBnClickedPi60()
{ ManualPio(0, 6, 0, BST_CHECKED == CWnd::IsDlgButtonChecked(IDC_PI_60) ? true: false); }
void CGOT_CONTROLDlg::OnBnClickedPi61()
{ ManualPio(0, 6, 1, BST_CHECKED == CWnd::IsDlgButtonChecked(IDC_PI_61) ? true: false); }
void CGOT_CONTROLDlg::OnBnClickedPi62()
{ ManualPio(0, 6, 2, BST_CHECKED == CWnd::IsDlgButtonChecked(IDC_PI_62) ? true: false); }
void CGOT_CONTROLDlg::OnBnClickedPi63()
{ ManualPio(0, 6, 3, BST_CHECKED == CWnd::IsDlgButtonChecked(IDC_PI_63) ? true: false); }
void CGOT_CONTROLDlg::OnBnClickedPi64()
{ ManualPio(0, 6, 4, BST_CHECKED == CWnd::IsDlgButtonChecked(IDC_PI_64) ? true: false); }
void CGOT_CONTROLDlg::OnBnClickedPi65()
{ ManualPio(0, 6, 5, BST_CHECKED == CWnd::IsDlgButtonChecked(IDC_PI_65) ? true: false); }
void CGOT_CONTROLDlg::OnBnClickedPi66()
{ ManualPio(0, 6, 6, BST_CHECKED == CWnd::IsDlgButtonChecked(IDC_PI_66) ? true: false); }
void CGOT_CONTROLDlg::OnBnClickedPi67()
{ ManualPio(0, 6, 7, BST_CHECKED == CWnd::IsDlgButtonChecked(IDC_PI_67) ? true: false); }

void CGOT_CONTROLDlg::OnBnClickedPi70()
{ ManualPio(0, 7, 0, BST_CHECKED == CWnd::IsDlgButtonChecked(IDC_PI_70) ? true: false); }
void CGOT_CONTROLDlg::OnBnClickedPi71()
{ ManualPio(0, 7, 1, BST_CHECKED == CWnd::IsDlgButtonChecked(IDC_PI_71) ? true: false); }
void CGOT_CONTROLDlg::OnBnClickedPi72()
{ ManualPio(0, 7, 2, BST_CHECKED == CWnd::IsDlgButtonChecked(IDC_PI_72) ? true: false); }
void CGOT_CONTROLDlg::OnBnClickedPi73()
{ ManualPio(0, 7, 3, BST_CHECKED == CWnd::IsDlgButtonChecked(IDC_PI_73) ? true: false); }
void CGOT_CONTROLDlg::OnBnClickedPi74()
{ ManualPio(0, 7, 4, BST_CHECKED == CWnd::IsDlgButtonChecked(IDC_PI_74) ? true: false); }
void CGOT_CONTROLDlg::OnBnClickedPi75()
{ ManualPio(0, 7, 5, BST_CHECKED == CWnd::IsDlgButtonChecked(IDC_PI_75) ? true: false); }
void CGOT_CONTROLDlg::OnBnClickedPi76()
{ ManualPio(0, 7, 6, BST_CHECKED == CWnd::IsDlgButtonChecked(IDC_PI_76) ? true: false); }
void CGOT_CONTROLDlg::OnBnClickedPi77()
{ ManualPio(0, 7, 7, BST_CHECKED == CWnd::IsDlgButtonChecked(IDC_PI_77) ? true: false); }




void CGOT_CONTROLDlg::OnBnClickedPo00()
{ ManualPio(1, 0, 0, BST_CHECKED == CWnd::IsDlgButtonChecked(IDC_PO_00) ? true: false); }
void CGOT_CONTROLDlg::OnBnClickedPo01()
{ ManualPio(1, 0, 1, BST_CHECKED == CWnd::IsDlgButtonChecked(IDC_PO_01) ? true: false); }
void CGOT_CONTROLDlg::OnBnClickedPo02()
{ ManualPio(1, 0, 2, BST_CHECKED == CWnd::IsDlgButtonChecked(IDC_PO_02) ? true: false); }
void CGOT_CONTROLDlg::OnBnClickedPo03()
{ ManualPio(1, 0, 3, BST_CHECKED == CWnd::IsDlgButtonChecked(IDC_PO_03) ? true: false); }
void CGOT_CONTROLDlg::OnBnClickedPo04()
{ ManualPio(1, 0, 4, BST_CHECKED == CWnd::IsDlgButtonChecked(IDC_PO_04) ? true: false); }
void CGOT_CONTROLDlg::OnBnClickedPo05()
{ ManualPio(1, 0, 5, BST_CHECKED == CWnd::IsDlgButtonChecked(IDC_PO_05) ? true: false); }
void CGOT_CONTROLDlg::OnBnClickedPo06()
{ ManualPio(1, 0, 6, BST_CHECKED == CWnd::IsDlgButtonChecked(IDC_PO_06) ? true: false); }
void CGOT_CONTROLDlg::OnBnClickedPo07()
{ ManualPio(1, 0, 7, BST_CHECKED == CWnd::IsDlgButtonChecked(IDC_PO_07) ? true: false); }

void CGOT_CONTROLDlg::OnBnClickedPo10()
{ ManualPio(1, 1, 0, BST_CHECKED == CWnd::IsDlgButtonChecked(IDC_PO_10) ? true: false); }
void CGOT_CONTROLDlg::OnBnClickedPo11()
{ ManualPio(1, 1, 1, BST_CHECKED == CWnd::IsDlgButtonChecked(IDC_PO_11) ? true: false); }
void CGOT_CONTROLDlg::OnBnClickedPo12()
{ ManualPio(1, 1, 2, BST_CHECKED == CWnd::IsDlgButtonChecked(IDC_PO_12) ? true: false); }
void CGOT_CONTROLDlg::OnBnClickedPo13()
{ ManualPio(1, 1, 3, BST_CHECKED == CWnd::IsDlgButtonChecked(IDC_PO_13) ? true: false); }
void CGOT_CONTROLDlg::OnBnClickedPo14()
{ ManualPio(1, 1, 4, BST_CHECKED == CWnd::IsDlgButtonChecked(IDC_PO_14) ? true: false); }
void CGOT_CONTROLDlg::OnBnClickedPo15()
{ ManualPio(1, 1, 5, BST_CHECKED == CWnd::IsDlgButtonChecked(IDC_PO_15) ? true: false); }
void CGOT_CONTROLDlg::OnBnClickedPo16()
{ ManualPio(1, 1, 6, BST_CHECKED == CWnd::IsDlgButtonChecked(IDC_PO_16) ? true: false); }
void CGOT_CONTROLDlg::OnBnClickedPo17()
{ ManualPio(1, 1, 7, BST_CHECKED == CWnd::IsDlgButtonChecked(IDC_PO_17) ? true: false); }

void CGOT_CONTROLDlg::OnBnClickedPo20()
{ ManualPio(1, 2, 0, BST_CHECKED == CWnd::IsDlgButtonChecked(IDC_PO_20) ? true: false); }
void CGOT_CONTROLDlg::OnBnClickedPo21()
{ ManualPio(1, 2, 1, BST_CHECKED == CWnd::IsDlgButtonChecked(IDC_PO_21) ? true: false); }
void CGOT_CONTROLDlg::OnBnClickedPo22()
{ ManualPio(1, 2, 2, BST_CHECKED == CWnd::IsDlgButtonChecked(IDC_PO_22) ? true: false); }
void CGOT_CONTROLDlg::OnBnClickedPo23()
{ ManualPio(1, 2, 3, BST_CHECKED == CWnd::IsDlgButtonChecked(IDC_PO_23) ? true: false); }
void CGOT_CONTROLDlg::OnBnClickedPo24()
{ ManualPio(1, 2, 4, BST_CHECKED == CWnd::IsDlgButtonChecked(IDC_PO_24) ? true: false); }
void CGOT_CONTROLDlg::OnBnClickedPo25()
{ ManualPio(1, 2, 5, BST_CHECKED == CWnd::IsDlgButtonChecked(IDC_PO_25) ? true: false); }
void CGOT_CONTROLDlg::OnBnClickedPo26()
{ ManualPio(1, 2, 6, BST_CHECKED == CWnd::IsDlgButtonChecked(IDC_PO_26) ? true: false); }
void CGOT_CONTROLDlg::OnBnClickedPo27()
{ ManualPio(1, 2, 7, BST_CHECKED == CWnd::IsDlgButtonChecked(IDC_PO_27) ? true: false); }

void CGOT_CONTROLDlg::OnBnClickedPo30()
{ ManualPio(1, 3, 0, BST_CHECKED == CWnd::IsDlgButtonChecked(IDC_PO_30) ? true: false); }
void CGOT_CONTROLDlg::OnBnClickedPo31()
{ ManualPio(1, 3, 1, BST_CHECKED == CWnd::IsDlgButtonChecked(IDC_PO_31) ? true: false); }
void CGOT_CONTROLDlg::OnBnClickedPo32()
{ ManualPio(1, 3, 2, BST_CHECKED == CWnd::IsDlgButtonChecked(IDC_PO_32) ? true: false); }
void CGOT_CONTROLDlg::OnBnClickedPo33()
{ ManualPio(1, 3, 3, BST_CHECKED == CWnd::IsDlgButtonChecked(IDC_PO_33) ? true: false); }
void CGOT_CONTROLDlg::OnBnClickedPo34()
{ ManualPio(1, 3, 4, BST_CHECKED == CWnd::IsDlgButtonChecked(IDC_PO_34) ? true: false); }
void CGOT_CONTROLDlg::OnBnClickedPo35()
{ ManualPio(1, 3, 5, BST_CHECKED == CWnd::IsDlgButtonChecked(IDC_PO_35) ? true: false); }
void CGOT_CONTROLDlg::OnBnClickedPo36()
{ ManualPio(1, 3, 6, BST_CHECKED == CWnd::IsDlgButtonChecked(IDC_PO_36) ? true: false); }
void CGOT_CONTROLDlg::OnBnClickedPo37()
{ ManualPio(1, 3, 7, BST_CHECKED == CWnd::IsDlgButtonChecked(IDC_PO_37) ? true: false); }

void CGOT_CONTROLDlg::OnBnClickedPo40()
{ ManualPio(1, 4, 0, BST_CHECKED == CWnd::IsDlgButtonChecked(IDC_PO_40) ? true: false); }
void CGOT_CONTROLDlg::OnBnClickedPo41()
{ ManualPio(1, 4, 1, BST_CHECKED == CWnd::IsDlgButtonChecked(IDC_PO_41) ? true: false); }
void CGOT_CONTROLDlg::OnBnClickedPo42()
{ ManualPio(1, 4, 2, BST_CHECKED == CWnd::IsDlgButtonChecked(IDC_PO_42) ? true: false); }
void CGOT_CONTROLDlg::OnBnClickedPo43()
{ ManualPio(1, 4, 3, BST_CHECKED == CWnd::IsDlgButtonChecked(IDC_PO_43) ? true: false); }
void CGOT_CONTROLDlg::OnBnClickedPo44()
{ ManualPio(1, 4, 4, BST_CHECKED == CWnd::IsDlgButtonChecked(IDC_PO_44) ? true: false); }
void CGOT_CONTROLDlg::OnBnClickedPo45()
{ ManualPio(1, 4, 5, BST_CHECKED == CWnd::IsDlgButtonChecked(IDC_PO_45) ? true: false); }
void CGOT_CONTROLDlg::OnBnClickedPo46()
{ ManualPio(1, 4, 6, BST_CHECKED == CWnd::IsDlgButtonChecked(IDC_PO_46) ? true: false); }
void CGOT_CONTROLDlg::OnBnClickedPo47()
{ ManualPio(1, 4, 7, BST_CHECKED == CWnd::IsDlgButtonChecked(IDC_PO_47) ? true: false); }

void CGOT_CONTROLDlg::OnBnClickedPo50()
{ ManualPio(1, 5, 0, BST_CHECKED == CWnd::IsDlgButtonChecked(IDC_PO_50) ? true: false); }
void CGOT_CONTROLDlg::OnBnClickedPo51()
{ ManualPio(1, 5, 1, BST_CHECKED == CWnd::IsDlgButtonChecked(IDC_PO_51) ? true: false); }
void CGOT_CONTROLDlg::OnBnClickedPo52()
{ ManualPio(1, 5, 2, BST_CHECKED == CWnd::IsDlgButtonChecked(IDC_PO_52) ? true: false); }
void CGOT_CONTROLDlg::OnBnClickedPo53()
{ ManualPio(1, 5, 3, BST_CHECKED == CWnd::IsDlgButtonChecked(IDC_PO_53) ? true: false); }
void CGOT_CONTROLDlg::OnBnClickedPo54()
{ ManualPio(1, 5, 4, BST_CHECKED == CWnd::IsDlgButtonChecked(IDC_PO_54) ? true: false); }
void CGOT_CONTROLDlg::OnBnClickedPo55()
{ ManualPio(1, 5, 5, BST_CHECKED == CWnd::IsDlgButtonChecked(IDC_PO_55) ? true: false); }
void CGOT_CONTROLDlg::OnBnClickedPo56()
{ ManualPio(1, 5, 6, BST_CHECKED == CWnd::IsDlgButtonChecked(IDC_PO_56) ? true: false); }
void CGOT_CONTROLDlg::OnBnClickedPo57()
{ ManualPio(1, 5, 7, BST_CHECKED == CWnd::IsDlgButtonChecked(IDC_PO_57) ? true: false); }

void CGOT_CONTROLDlg::OnBnClickedPo60()
{ ManualPio(1, 6, 0, BST_CHECKED == CWnd::IsDlgButtonChecked(IDC_PO_60) ? true: false); }
void CGOT_CONTROLDlg::OnBnClickedPo61()
{ ManualPio(1, 6, 1, BST_CHECKED == CWnd::IsDlgButtonChecked(IDC_PO_61) ? true: false); }
void CGOT_CONTROLDlg::OnBnClickedPo62()
{ ManualPio(1, 6, 2, BST_CHECKED == CWnd::IsDlgButtonChecked(IDC_PO_62) ? true: false); }
void CGOT_CONTROLDlg::OnBnClickedPo63()
{ ManualPio(1, 6, 3, BST_CHECKED == CWnd::IsDlgButtonChecked(IDC_PO_63) ? true: false); }
void CGOT_CONTROLDlg::OnBnClickedPo64()
{ ManualPio(1, 6, 4, BST_CHECKED == CWnd::IsDlgButtonChecked(IDC_PO_64) ? true: false); }
void CGOT_CONTROLDlg::OnBnClickedPo65()
{ ManualPio(1, 6, 5, BST_CHECKED == CWnd::IsDlgButtonChecked(IDC_PO_65) ? true: false); }
void CGOT_CONTROLDlg::OnBnClickedPo66()
{ ManualPio(1, 6, 6, BST_CHECKED == CWnd::IsDlgButtonChecked(IDC_PO_66) ? true: false); }
void CGOT_CONTROLDlg::OnBnClickedPo67()
{ ManualPio(1, 6, 7, BST_CHECKED == CWnd::IsDlgButtonChecked(IDC_PO_67) ? true: false); }

void CGOT_CONTROLDlg::OnBnClickedPo70()
{ ManualPio(1, 7, 0, BST_CHECKED == CWnd::IsDlgButtonChecked(IDC_PO_70) ? true: false); }
void CGOT_CONTROLDlg::OnBnClickedPo71()
{ ManualPio(1, 7, 1, BST_CHECKED == CWnd::IsDlgButtonChecked(IDC_PO_71) ? true: false); }
void CGOT_CONTROLDlg::OnBnClickedPo72()
{ ManualPio(1, 7, 2, BST_CHECKED == CWnd::IsDlgButtonChecked(IDC_PO_72) ? true: false); }
void CGOT_CONTROLDlg::OnBnClickedPo73()
{ ManualPio(1, 7, 3, BST_CHECKED == CWnd::IsDlgButtonChecked(IDC_PO_73) ? true: false); }
void CGOT_CONTROLDlg::OnBnClickedPo74()
{ ManualPio(1, 7, 4, BST_CHECKED == CWnd::IsDlgButtonChecked(IDC_PO_74) ? true: false); }
void CGOT_CONTROLDlg::OnBnClickedPo75()
{ ManualPio(1, 7, 5, BST_CHECKED == CWnd::IsDlgButtonChecked(IDC_PO_75) ? true: false); }
void CGOT_CONTROLDlg::OnBnClickedPo76()
{ ManualPio(1, 7, 6, BST_CHECKED == CWnd::IsDlgButtonChecked(IDC_PO_76) ? true: false); }
void CGOT_CONTROLDlg::OnBnClickedPo77()
{ ManualPio(1, 7, 7, BST_CHECKED == CWnd::IsDlgButtonChecked(IDC_PO_77) ? true: false); }

void CGOT_CONTROLDlg::OnBnClickedPlgFrameReset()
{
	mcls_MI.gcls_pPlg->Clear_SeqMem();
}
