// TI_DELETEDlg.cpp : 実装ファイル
//

#include "stdafx.h"
#include "TI_DELETE.h"
#include "TI_DELETEDlg.h"


// メモリーリーク検出 ----->>>
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif
// <<<------------------------


// CTI_DELETEDlg ダイアログ




CTI_DELETEDlg::CTI_DELETEDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CTI_DELETEDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CTI_DELETEDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, m_ListBox);
	DDX_Control(pDX, IDC_CMB_DISP, m_cmbSession);
}

BEGIN_MESSAGE_MAP(CTI_DELETEDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_LBN_DBLCLK(IDC_LIST1, &CTI_DELETEDlg::OnLbnDblclkList1)
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDOK, &CTI_DELETEDlg::OnBnClickedOk)
	ON_WM_TIMER()
END_MESSAGE_MAP()


// CTI_DELETEDlg メッセージ ハンドラ

BOOL CTI_DELETEDlg::OnInitDialog()
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

void CTI_DELETEDlg::OnPaint()
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
HCURSOR CTI_DELETEDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}




// これ以降 手動追加 -------------------->>>

//￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣
// ×ボタン
//＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿
void CTI_DELETEDlg::OnClose()
{
	//CDialog::OnClose();
}
//￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣
// 閉じるボタン
//＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿
void CTI_DELETEDlg::OnBnClickedOk()
{
	if( IDYES == MessageBox("終了しますか？", "重要なお知らせ", MB_YESNO | MB_DEFBUTTON2 | MB_ICONQUESTION) ) {
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
void CTI_DELETEDlg::OnLbnDblclkList1()
{
	// ListBoxコントロールのプロパティは、ソート禁止にしておく事
	// ListBoxコントロールのダブルクリックイベントを追加
	mcls_pLog->ShowEdit();
}

//------------------------------------------
// 各プロセス初期化処理
//------------------------------------------
void CTI_DELETEDlg::InitProcess()
{
	// タスク初期化処理
	setlineid(LINE_ID);
	if( 0 != task_init(TI_DELETE) ) {
		syslog(SYSNO_TASK_INIT_ERR, "[%s]", TI_DELETE);
		_ASSERT(0);
		exit(-9);		// 自プロセス強制終了
		return;
	}

	// LogFileManagerインスタンス生成 (これ以降 ログ出力可能)
	setlocale(LC_ALL, "Japanese");	// これが無いと日本語パスを含むフォルダの場合、保存できない。(ライブラリの中で実行するべきか迷っているが、変更するとタスク全体に影響するので、とりあえずここで設定)
	mcls_pLog = new LogFileManager(getmytaskname(), &m_ListBox);
	LOG(em_MSG), "タスク起動");
	LOG(em_MSG), "各プロセス開始処理 実行中・・・");

	// メインインスタンス 開始処理
	mcls_MI.SetLogMgr(mcls_pLog);
	mcls_MI.Init();
	
	//// 画面更新用データセット
	// コンボボックスにセッション名追加
	for(int ii=0; ii<mcls_MI.g_nClassCnt; ii++) {
		m_cmbSession.AddString( mcls_MI.gcls_pContMgr[ii]->GetSession() );
	}
	m_cmbSession.SetCurSel(0);				// 0番目を初期選択とする
	SetTimer(1, 500, NULL);

	// メインスレッドの実行を行う
	LOG(em_MSG), "各プロセス開始処理 完了");
	mcls_MI.Start();	
	return;
}

//￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣
// 各プロセス開放処理
//＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿
void CTI_DELETEDlg::ExitProcess()
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

	// コンボボックス初期化
	m_cmbSession.ResetContent();

	// 画面の終了要求
	PostMessage(WM_QUIT);
}

//------------------------------------------
// メインスレッド DlgThread
// LPVOID param ダイヤログポインタ
//------------------------------------------
unsigned WINAPI CTI_DELETEDlg::MainThread(void* param)
{
	CTI_DELETEDlg* p = (CTI_DELETEDlg *)param;
	COMMON_QUE que;			// 受信構造体

	//// 各プロセス初期化処理 (これ以前はログ出力禁止)
	p->InitProcess();

	//// 起動要因取り込み ループ開始
	while(true) {
		recv_mail(&que);			// 起動要因取り込み

		switch(que.nEventNo) {

//-----------------------------------------------------------------------------------------------
		case FACT_TI_DELETE_01:								// 削除･書込優先モード通知 (←TO_DEFECT)
			pLOG(em_MSG), "%s優先モード", (0 == que.fl.data[0] ? "削除" : "書込"));
			p->mcls_MI.SetWaitMode( (0==que.fl.data[0] ? true : false) );
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
void CTI_DELETEDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: ここにメッセージ ハンドラ コードを追加するか、既定の処理を呼び出します。
	int ii;
	CString strVal;	
	int nWk;

	//// 現在選択されているインスタンスを取得する
	CString strSession = "";
	int nCmbIndex = m_cmbSession.GetCurSel();
	if( -1 == nCmbIndex) return;					// iniファイルの設定無し

	// インスタンスの特定
	ControlManager* pCnt = mcls_MI.gcls_pContMgr[nCmbIndex];


	//// もろもろ
	SetDlgItemText( IDC_DSP_ODBC, pCnt->GetOdbcSession() );
	SetDlgItemText(IDC_STATE_DELETE,  (pCnt->GetWaitMode() ? "○" : "×"));
	

	//// HDD情報
	strVal.Format("%.2f", pCnt->GetDiskSpace().dTotalSize /1024 );	SetDlgItemText( IDC_HDD_TOTAL, strVal );
	strVal.Format("%.2f", pCnt->GetDiskSpace().dFreeSize  /1024 );	SetDlgItemText( IDC_HDD_FREE,  strVal );
	strVal.Format("%.2f", pCnt->GetDiskSpace().dFreeRitu  * 100 );	SetDlgItemText( IDC_HDD_RITU,  strVal );

	//// スレッド実行状態
	int txtTH[9] = {IDC_TH_1, IDC_TH_2, IDC_TH_3, IDC_TH_4, IDC_TH_5, IDC_TH_6, IDC_TH_7, IDC_TH_8, IDC_TH_9};
	for(ii=0; ii<9; ii++) {
		nWk = pCnt->IsExec(ii);

		if( 0 == nWk )    strVal = "×";
		else if(1 == nWk) strVal = "○";
		else			  strVal = "－";
		SetDlgItemText( txtTH[ii], strVal);
	}
	// 定周期スレッド
	nWk = pCnt->IsExec();
	if( 0 == nWk )    strVal = "×";
	else if(1 == nWk) strVal = "○";
	else			  strVal = "－";
	SetDlgItemText( IDC_TH_0, strVal);



	CDialog::OnTimer(nIDEvent);
}
