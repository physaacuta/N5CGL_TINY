// KS_EQALDBDlg.cpp : 実装ファイル
//

#include "stdafx.h"
#include "KS_EQALDB.h"
#include "KS_EQALDBDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CKS_EQALDBDlg ダイアログ




CKS_EQALDBDlg::CKS_EQALDBDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CKS_EQALDBDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CKS_EQALDBDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, m_ListBox);
	DDX_Control(pDX, IDC_CMB_DISP, m_cmbSession);
}

BEGIN_MESSAGE_MAP(CKS_EQALDBDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDOK, &CKS_EQALDBDlg::OnBnClickedOk)
	ON_LBN_DBLCLK(IDC_LIST1, &CKS_EQALDBDlg::OnLbnDblclkList1)
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDC_NOWEXEC_AB, &CKS_EQALDBDlg::OnBnClickedNowexecAb)
	ON_BN_CLICKED(IDC_NOWEXEC_BA, &CKS_EQALDBDlg::OnBnClickedNowexecBa)
	ON_WM_TIMER()
END_MESSAGE_MAP()


// CKS_EQALDBDlg メッセージ ハンドラ

BOOL CKS_EQALDBDlg::OnInitDialog()
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

void CKS_EQALDBDlg::OnPaint()
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
HCURSOR CKS_EQALDBDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

// これ以降 手動追加 -------------------->>>

//￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣
// ×ボタン
//＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿
void CKS_EQALDBDlg::OnClose()
{
//	CDialog::OnClose();
}

//￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣
// 閉じるボタン
//＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿
void CKS_EQALDBDlg::OnBnClickedOk()
{
	if( IDYES == MessageBox("終了しますか？", "重要なお知らせ", MB_YESNO | MB_DEFBUTTON2 | MB_ICONQUESTION) ) {
		LOG(em_MSG), "閉じるボタン");
		COMMON_QUE que;
		que.nEventNo = FACT_KS_END;
		que.nLineNo = LINE_ID;						
		send_mail(getmytaskname(), ".", &que);	
	}
}
//￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣
// メモ帳起動
//＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿
void CKS_EQALDBDlg::OnLbnDblclkList1()
{
	// ListBoxコントロールのプロパティは、ソート禁止にしておく事
	// ListBoxコントロールのダブルクリックイベントを追加
	mcls_pLog->ShowEdit();
}


//------------------------------------------
// 各プロセス初期化処理
//------------------------------------------
void CKS_EQALDBDlg::InitProcess()
{
	int ii, jj;

	// タスク初期化処理
	if( 0 != task_init(KS_EQALDB) ) {
		syslog(SYSNO_TASK_INIT_ERR, "[%s]", KS_EQALDB);
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
	for(ii=0; ii<MainInctance::TYPE_DB_END; ii++) {
		for(jj=0; jj<mcls_MI.g_nClassCnt[ii]; jj++) {
			m_cmbSession.AddString( mcls_MI.gcls_pDb[ii][jj]->GetSession() );
		}
	}
	m_cmbSession.SetCurSel(0);				// 0番目を初期選択とする


	SetTimer(1, 500, NULL);					// 画面更新タイマー


	// メインスレッドの実行を行う
	LOG(em_MSG), "各プロセス開始処理 完了");
	mcls_MI.Start();	
	return;
}

//￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣
// 各プロセス開放処理
//＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿
void CKS_EQALDBDlg::ExitProcess()
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
unsigned WINAPI CKS_EQALDBDlg::MainThread(void* param)
{
	CKS_EQALDBDlg* p = (CKS_EQALDBDlg *)param;
	COMMON_QUE que;			// 受信構造体

	//// 各プロセス初期化処理 (これ以前はログ出力禁止)
	p->InitProcess();

	//// 起動要因取り込み ループ開始
	while(true) {
		recv_mail(&que);			// 起動要因取り込み

		switch(que.nEventNo) {

//-----------------------------------------------------------------------------------------------
		case FACT_KS_EQALDB_01:								// 即時実行通知（←KS_BACKUP）
			pLOG(em_MSG), "[MAIL] 即時実行通知 (FACT_KS_EQALDB_01) [%s] 実行キー=%d", 0 == que.fl.data[0] ? "A→B" : "B→A", que.fl.data[1] );		
			p->mcls_MI.SelectNowExec( que.fl.data[0], que.fl.data[1]);
			break;

//-----------------------------------------------------------------------------------------------
		case FACT_KS_PCNAME_CHANGE:							// PC名称変更通知 (←KS_SLAVE)
				// 無視
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
// 定周期タイマー
//＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿
void CKS_EQALDBDlg::OnTimer(UINT_PTR nIDEvent)
{
	int ii, jj;
	KizuSyncroManager*	cls_pDb = NULL;

	//// 現在選択されているインスタンスを取得する
	CString strSession = "";
	int nCmbIndex = m_cmbSession.GetCurSel();
	if( -1 == nCmbIndex) return;					// iniファイルの設定無し
	
	// インスタンスの特定
	m_cmbSession.GetLBText( nCmbIndex, strSession);
	for(ii=0; ii<MainInctance::TYPE_DB_END; ii++) {
		for(jj=0; jj<mcls_MI.g_nClassCnt[ii]; jj++) {
			if( 0 == strcmp(strSession, mcls_MI.gcls_pDb[ii][jj]->GetSession() ) ) {
				cls_pDb = mcls_MI.gcls_pDb[ii][jj];
			}
		}
	}
	if(NULL == cls_pDb) goto Ending; 
	

	//// 現在の状態
	SetDlgItemText(IDC_DSP_TYPE, cls_pDb->GetType() );
	SetDlgItemInt(IDC_DSP_MAILKEY, cls_pDb->GetMailkey() );
	SetDlgItemInt(IDC_DSP_TIMEFAST, cls_pDb->GetTimeFast() );
	SetDlgItemInt(IDC_DSP_TIMEINTERVAL, cls_pDb->GetTimeInterval() );

	SetDlgItemText(IDC_DSP_ODBC_A, cls_pDb->GetOdbc_A() );
	SetDlgItemText(IDC_DSP_ODBC_B, cls_pDb->GetOdbc_B() );
	SetDlgItemText(IDC_DSP_LINK_A, cls_pDb->GetLink_A() );
	SetDlgItemText(IDC_DSP_LINK_B, cls_pDb->GetLink_B() );


Ending:

	CDialog::OnTimer(nIDEvent);
}


//￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣
// 即時実行 (A系→B系)
//＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿
void CKS_EQALDBDlg::OnBnClickedNowexecAb()
{
	// TODO: ここにコントロール通知ハンドラ コードを追加します。
	if ( BST_CHECKED != CWnd::IsDlgButtonChecked(IDC_DEBUG_MANUAL) ) return;
	LOG(em_WAR), "[DEBUG] 即時実行ボタン (A系→B系)" ); 

	COMMON_QUE que;
	que.nEventNo = FACT_KS_EQALDB_01;
	que.nLineNo = getlineid();
	que.fl.data[0] = 0;
	que.fl.data[1] = GetDlgItemInt(IDC_MAILKEY);
	send_mail(getmytaskname(), ".", &que);
}

//￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣
// 即時実行 (B系→A系)
//＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿
void CKS_EQALDBDlg::OnBnClickedNowexecBa()
{
	// TODO: ここにコントロール通知ハンドラ コードを追加します。
	if ( BST_CHECKED != CWnd::IsDlgButtonChecked(IDC_DEBUG_MANUAL) ) return;
	LOG(em_WAR), "[DEBUG] 即時実行ボタン (B系→A系)" ); 

	COMMON_QUE que;
	que.nEventNo = FACT_KS_EQALDB_01;
	que.nLineNo = getlineid();
	que.fl.data[0] = 1;
	que.fl.data[1] = GetDlgItemInt(IDC_MAILKEY);
	send_mail(getmytaskname(), ".", &que);
}
