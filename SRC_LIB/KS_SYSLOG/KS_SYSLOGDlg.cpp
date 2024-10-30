// KS_SYSLOGDlg.cpp : 実装ファイル
//

#include "stdafx.h"
#include "KS_SYSLOG.h"
#include "KS_SYSLOGDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CKS_SYSLOGDlg ダイアログ




CKS_SYSLOGDlg::CKS_SYSLOGDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CKS_SYSLOGDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CKS_SYSLOGDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, m_ListBox);
	DDX_Control(pDX, IDC_LIST1, m_ListBox);
}

BEGIN_MESSAGE_MAP(CKS_SYSLOGDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_LOGDSP, &CKS_SYSLOGDlg::OnBnClickedLogdsp)
	ON_BN_CLICKED(IDOK, &CKS_SYSLOGDlg::OnBnClickedOk)
	ON_LBN_DBLCLK(IDC_LIST1, &CKS_SYSLOGDlg::OnLbnDblclkList1)
	ON_WM_CLOSE()
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_BUTTON1, &CKS_SYSLOGDlg::OnBnClickedButton1)
END_MESSAGE_MAP()


// CKS_SYSLOGDlg メッセージ ハンドラ

BOOL CKS_SYSLOGDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// このダイアログのアイコンを設定します。アプリケーションのメイン ウィンドウがダイアログでない場合、
	//  Framework は、この設定を自動的に行います。
	SetIcon(m_hIcon, TRUE);			// 大きいアイコンの設定
	SetIcon(m_hIcon, FALSE);		// 小さいアイコンの設定

	// TODO: 初期化をここに追加します。
// ------------------------------->>>
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

void CKS_SYSLOGDlg::OnPaint()
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
HCURSOR CKS_SYSLOGDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

// これ以降 手動追加 -------------------->>>

//￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣
// ×ボタン
//＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿
void CKS_SYSLOGDlg::OnClose()
{
	//CDialog::OnClose();
}

//￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣
// 閉じるボタン
//＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿
void CKS_SYSLOGDlg::OnBnClickedOk()
{
	// TODO: ここにコントロール通知ハンドラ コードを追加します。
	if( IDYES == MessageBox("終了しますか？", "重要なお知らせ", MB_YESNO | MB_DEFBUTTON2 | MB_ICONQUESTION) ) {
		LOG(em_MSG), "強制終了ボタン");
		COMMON_QUE que;
		que.nEventNo = FACT_KS_END;
		que.nLineNo = getlineid();
		send_mail(getmytaskname(), ".", &que);
	}
}

//￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣
// メモ帳起動
//＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿
void CKS_SYSLOGDlg::OnLbnDblclkList1()
{
	// ListBoxコントロールのプロパティは、ソート禁止にしておく事
	// ListBoxコントロールのダブルクリックイベントを追加
	mcls_pLog->ShowEdit();
}

//------------------------------------------
// 各プロセス初期化処理
//------------------------------------------
void CKS_SYSLOGDlg::InitProcess()
{
	// タスク初期化処理
	setlineid(LINE_ID);
	if( 0 != task_init(KS_SYSLOG) ) {
		syslog(SYSNO_TASK_INIT_ERR, "[%s]", KS_SYSLOG);
		_ASSERT(0);
		exit(-9);		// 自プロセス強制終了
		return;
	}

	// LogFileManagerインスタンス生成 (これ以降 ログ出力可能)
	mcls_pLog = new LogFileManager(getmytaskname(), &m_ListBox);
	LOG(em_MSG), "タスク起動");
	LOG(em_MSG), "各プロセス開始処理 実行中・・・");

	// メインインスタンス 開始処理
	mcls_MI.SetLogMgr(mcls_pLog);
	mcls_MI.Init();
	
	//// 画面更新用タイマ生成
	SetTimer(1, 500, NULL);

	// メインスレッドの実行を行う
	LOG(em_MSG), "各プロセス開始処理 完了");
	mcls_MI.Start();	
	return;
}

//￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣
// 各プロセス開放処理
//＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿
void CKS_SYSLOGDlg::ExitProcess()
{
	//// ログ リストボックス表示処理終了
	mcls_pLog->SetListOut(false);					// これ以降 リストボックスへのログ表示禁止

	// メインインスタンス 終了処理
	KillTimer(1);			// 画面更新タイマ
	mcls_MI.Stop();
	mcls_MI.Exit();

	//// ログクラス開放
	LOG(em_MSG), "タスク終了");
	if(NULL != mcls_pLog){
		delete mcls_pLog;
		mcls_pLog = NULL;
	}
	
	// タスク終了処理
	task_exit();

	// 画面の終了要求
	PostMessage(WM_QUIT);
}

// 20170321 ﾒｰﾙｽﾛｯﾄｻｲｽﾞが異なるﾀｽｸ間通信の対応 --->
//------------------------------------------
// メインスレッド DlgThread
// LPVOID param ダイヤログポインタ
//------------------------------------------
unsigned WINAPI CKS_SYSLOGDlg::MainThread(void* param)
{
	CKS_SYSLOGDlg* p = (CKS_SYSLOGDlg *)param;

	//// 各プロセス初期化処理 (これ以前はログ出力禁止)
	p->InitProcess();

	//// 起動要因取り込み ループ開始
	while(true) {
		COMMON_QUE que[2];									// 受信構造体（念の為に毎回初期化、[2]はMAIL_SLOTが長かった場合を考慮）
		int ncode = recv_mail(&que[0]);						// 起動要因取り込み

		if (ncode > 0) {
			// 末尾が削れるだけで正常処理出来るかもしれないのでCOINTINUEしない
			if (ERROR_MORE_DATA == ncode) {
				pLOG(em_WAR), "recv_mail Size UnMatch [ErrCode=%d][EventNo=%d]", ncode, que[0].nEventNo);
			} else {
				pLOG(em_WAR), "recv_mail Err [ErrCode=%d][EventNo=%d]", ncode, que[0].nEventNo);
			}
		}

		switch(que[0].nEventNo) {

		case FACT_KS_SYSLOG_01:								// シスログ登録
			p->mcls_MI.AddSysLogSqlQue(&que[0]);
			break;

		case FACT_KS_SYSLOG_02:								// 機器状態ログ登録
			p->mcls_MI.AddStateLogSqlQue(&que[0]);
			break;


		case FACT_KS_PCNAME_CHANGE:							// PC名称変更通知 (←KS_SLAVE)
			// 無処理
			break;

		case FACT_KS_END:									// 終了要求
			// 各プロセス終了処理
			p->ExitProcess();
			return 0;

		default:
			pLOG(em_ERR), "MAILNO_ERR [EventNo=%d]", que[0].nEventNo);
			syslog(SYSNO_MAILNO_ERR, "[EventNo=%d]", que[0].nEventNo);
			break;
		}
	}
	return 0;
}
// 20170321 ﾒｰﾙｽﾛｯﾄｻｲｽﾞが異なるﾀｽｸ間通信の対応 <---

//￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣
// 画面更新タイマ
//＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿
void CKS_SYSLOGDlg::OnTimer(UINT_PTR nIDEvent)

{
	// TODO: ここにメッセージ ハンドラ コードを追加するか、既定の処理を呼び出します。
	// とりあえず追加したけど、今の所必要なかった・・・
	CDialog::OnTimer(nIDEvent);
}

//￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣
// ログ表示チェックボックス
//＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿
void CKS_SYSLOGDlg::OnBnClickedLogdsp()
{
	if ( BST_CHECKED == CWnd::IsDlgButtonChecked( IDC_LOGDSP) ) {		// ログ表示
		if( IDYES == MessageBox("パフォーマンスが低下します。デバック時のみ使用してください", "重要な確認",	MB_YESNO | MB_DEFBUTTON2 | MB_ICONQUESTION) ) {
			mcls_MI.SetSyslogDsp(true); 
		} else {
			CWnd::CheckDlgButton(IDC_LOGDSP, FALSE);	// 凹みキャンセル
		}
	} else {															// ログ出力無し
		mcls_MI.SetSyslogDsp(false);
	}

}

void CKS_SYSLOGDlg::OnBnClickedButton1()
{
	syslog(0, "test [%s]", KS_SYSLOG);

}
