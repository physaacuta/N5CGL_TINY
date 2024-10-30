
// TO_PUTDATDlg.cpp : 実装ファイル
//

#include "stdafx.h"
#include "TO_PUTDAT.h"
#include "TO_PUTDATDlg.h"
#include "afxdialogex.h"

// メモリーリーク検出 ----->>>
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif
// <<<------------------------

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CTO_PUTDATDlg ダイアログ



CTO_PUTDATDlg::CTO_PUTDATDlg(CWnd* pParent /*=NULL*/)
	: CDialog(IDD_TO_PUTDAT_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CTO_PUTDATDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, m_ListBox);
}

BEGIN_MESSAGE_MAP(CTO_PUTDATDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDOK, &CTO_PUTDATDlg::OnBnClickedOk)
	ON_WM_CLOSE()
	ON_WM_TIMER()
	ON_LBN_DBLCLK(IDC_LIST1, &CTO_PUTDATDlg::OnLbnDblclkList1)
	ON_BN_CLICKED(IDC_DBGCHKBOX, &CTO_PUTDATDlg::OnBnClickedDbgchkbox)
	ON_BN_CLICKED(IDC_BTN_SAISUN, &CTO_PUTDATDlg::OnBnClickedBtnSaisun)
	ON_BN_CLICKED(IDC_BTN_CUT, &CTO_PUTDATDlg::OnBnClickedBtnCut)
END_MESSAGE_MAP()


// CTO_PUTDATDlg メッセージ ハンドラー

BOOL CTO_PUTDATDlg::OnInitDialog()
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

void CTO_PUTDATDlg::OnPaint()
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
HCURSOR CTO_PUTDATDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


// これ以降 手動追加 -------------------->>>


//￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣
// ×ボタン
//＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿
void CTO_PUTDATDlg::OnClose()
{
	//CDialog::OnClose();
}

//￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣
// 強制終了ボタン
//＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿
void CTO_PUTDATDlg::OnBnClickedOk()
{
	if (IDYES == MessageBox("終了しますか？", "重要なお知らせ", MB_YESNO | MB_DEFBUTTON2 | MB_ICONQUESTION))
	{
		LOG(em_MSG), "閉じるボタン");
		COMMON_QUE que;
		que.nEventNo = FACT_KS_END;
		que.nLineNo = getlineid();
		send_mail(getmytaskname(), ".", &que);
	}
}

//￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣
// メモ帳起動
//＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿
void CTO_PUTDATDlg::OnLbnDblclkList1()
{
	// ListBoxコントロールのプロパティは、ソート禁止にしておく事
	// ListBoxコントロールのダブルクリックイベントを追加
	mcls_pLog->ShowEdit();
}

//------------------------------------------
// 各プロセス初期化処理
//------------------------------------------
void CTO_PUTDATDlg::InitProcess()
{
	CString ss;

	//// タスク初期化処理
	if (0 != task_init(TO_PUTDAT))
	{
		syslog(SYSNO_TASK_INIT_ERR, "[%s]", TO_PUTDAT);
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

	//// 画面更新用タイマ生成
	SetTimer(1, 500, NULL);

	//// メインスレッドの実行を行う
	LOG(em_MSG), "[Dlg] 各プロセス開始処理 完了");
	mcls_MI.Start();
	return;
}

//￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣
// 各プロセス解放処理
//＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿
void CTO_PUTDATDlg::ExitProcess()
{
	//// メインインスタンス 終了処理
	KillTimer(1);			// 画面更新タイマ
	mcls_MI.Stop();
	mcls_MI.Exit();

	//// ログ リストボックス表示処理終了
	mcls_pLog->SetListOut(false);					// これ以降 リストボックスへのログ表示禁止

	//// ログクラス解放
	LOG(em_MSG), "[Dlg] タスク終了");
	SAFE_DELETE(mcls_pLog);

	// タスク終了処理
	task_exit();

	// 画面の終了要求
	PostMessage(WM_QUIT);
}

//------------------------------------------
// メインスレッド DlgThread
// LPVOID param ダイヤログポインタ
//------------------------------------------
unsigned WINAPI CTO_PUTDATDlg::MainThread(LPVOID param)
{
	CTO_PUTDATDlg* p = (CTO_PUTDATDlg *)param;
	COMMON_QUE que;					// 受信構造体

	//// 各プロセス初期化処理 (これ以前はログ出力禁止)
	p->InitProcess();

	//// 起動要因取り込み ループ開始
	while (true)
	{
		recv_mail(&que);			// 起動要因取り込み

		switch (que.nEventNo)
		{
//*/
//-----------------------------------------------------------------------------------------------
		//case FACT_TO_PUTDAT_01:									// パラメータ変更要求 (←TO_DEFECT)
		//	pLOG(em_MSG), "[MAIL] パラメータ変更要求通知 (FACT_TO_PUTDAT_01) [テーブル=%d]", que.fl.data[0] ); 
		//	
		//	break;
//-----------------------------------------------------------------------------------------------
		case FACT_TO_PUTDAT_02:						// プロコン採寸実績受信通知 (←TO_PROCON)
			pLOG(em_MSG), "[MAIL] プロコン採寸実績受信通知 (FACT_TO_PUTDAT_02) [ID=%s]", que.fc.data);
			// メインインスタンスへ通知
			p->mcls_MI.SetDeliveryMail(E_DEF_ML_PRO_R_SAI, &que);
			break;

//-----------------------------------------------------------------------------------------------
		case FACT_TO_PUTDAT_03:						// プロコンカット実績受信通知 (←TO_PROCON)
			pLOG(em_MSG), "[MAIL] プロコンカット実績受信通知 (FACT_TO_PUTDAT_03) [ID=%s]", que.fc.data);
			// メインインスタンスへ通知
			p->mcls_MI.SetDeliveryMail(E_DEF_ML_PRO_R_CUT, &que);
			break;

//-----------------------------------------------------------------------------------------------
		case FACT_TO_PUTDAT_04:						// 実績書込み完了通知 (←TO_DEFECT)
			pLOG(em_MSG), "[MAIL] 実績書込み完了通知 (FACT_TO_PUTDAT_04) [アンサー結果=%d]", que.fl.data[0]);
			// メインインスタンスへ通知
			p->mcls_MI.SetDeliveryMail(E_DEF_ML_COIL_RESULT_ADD, &que);
			break;

//-----------------------------------------------------------------------------------------------
		case FACT_KS_PCNAME_CHANGE:							// PC名称変更通知
			// 無処理
			break;

//-----------------------------------------------------------------------------------------------
		case FACT_KS_END:									// 終了要求
			// 各プロセス終了処理
			p->ExitProcess();
			return 0;

//-----------------------------------------------------------------------------------------------
		default:
			pLOG(em_ERR), "[MAIL] イベントNo 無し [EventNo=%d]", que.nEventNo);
			syslog(SYSNO_MAILNO_ERR, "[EventNo=%d]", que.nEventNo);
			break;
		}
	}
	return 0;
}

//￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣
// 画面更新タイマ
//＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿
void CTO_PUTDATDlg::OnTimer(UINT_PTR nIDEvent)
{
	CDialog::OnTimer(nIDEvent);
}

//￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣
// デバック機能有効/無効チェックボックス
//＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿
void CTO_PUTDATDlg::OnBnClickedDbgchkbox()
{
	bool bVal = (BST_CHECKED == CWnd::IsDlgButtonChecked(IDC_DBGCHKBOX) ? true : false);
	GetDlgItem(IDC_BTN_SAISUN)->EnableWindow(bVal);
	GetDlgItem(IDC_IDINPUT_SAISUN)->EnableWindow(bVal);
	GetDlgItem(IDC_BTN_CUT)->EnableWindow(bVal);
	GetDlgItem(IDC_IDINPUT_CUT)->EnableWindow(bVal);
	GetDlgItem(IDC_LIST1)->SetFocus();
}

//￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣
// 採寸実績受信通知
//＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿
void CTO_PUTDATDlg::OnBnClickedBtnSaisun()
{
	if( IDYES != MessageBox("[デバック] 採寸実績受信通知を送信しますか？", "重要なお知らせ",MB_YESNO | MB_DEFBUTTON2 | MB_ICONQUESTION) )
	{
		return;
	}

	COMMON_QUE que;
	memset(&que, 0x00, sizeof(que));
	que.nEventNo = FACT_TO_PUTDAT_02;
	que.nLineNo = getlineid();
	GetDlgItemText(IDC_IDINPUT_SAISUN, que.fc.data, SIZE_SERIAL_FF_NO);

	LOG(em_WAR), "[DEBUG] プロコン採寸実績受信通知ボタン [ID=%s]", que.fc.data);

	// 送信
	send_mail(getmytaskname(), ".", &que);
}

//￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣
// カット実績受信通知
//＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿
void CTO_PUTDATDlg::OnBnClickedBtnCut()
{
	if (IDYES != MessageBox("[デバック] カット実績受信通知を送信しますか？", "重要なお知らせ", MB_YESNO | MB_DEFBUTTON2 | MB_ICONQUESTION))
	{
		return;
	}

	COMMON_QUE que;
	memset(&que, 0x00, sizeof(que));
	que.nEventNo = FACT_TO_PUTDAT_03;
	que.nLineNo = getlineid();
	GetDlgItemText(IDC_IDINPUT_CUT, que.fc.data, SIZE_SERIAL_FF_NO);
		
	LOG(em_WAR), "[DEBUG] プロコンカット実績受信通知ボタン [ID=%s]", que.fc.data);

	// 送信
	send_mail(getmytaskname(), ".", &que);
}
