// TO_MAEDATDlg.cpp : 実装ファイル
//

#include "stdafx.h"
#include "TO_MAEDAT.h"
#include "TO_MAEDATDlg.h"

//#ifdef _DEBUG
//#define new DEBUG_NEW
//#endif
// メモリーリーク検出 手動追加 ----->>>
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif
// <<<------------------------

// CTO_MAEDATDlg ダイアログ




CTO_MAEDATDlg::CTO_MAEDATDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CTO_MAEDATDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CTO_MAEDATDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, m_ListBox);
}

BEGIN_MESSAGE_MAP(CTO_MAEDATDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_LBN_DBLCLK(IDC_LIST1, &CTO_MAEDATDlg::OnLbnDblclkList1)
	ON_BN_CLICKED(IDOK, &CTO_MAEDATDlg::OnBnClickedOk)
	ON_WM_CLOSE()
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_BUTTON1, &CTO_MAEDATDlg::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON2, &CTO_MAEDATDlg::OnBnClickedButton2)
END_MESSAGE_MAP()


// CTO_MAEDATDlg メッセージ ハンドラ

BOOL CTO_MAEDATDlg::OnInitDialog()
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

void CTO_MAEDATDlg::OnPaint()
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
HCURSOR CTO_MAEDATDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


// これ以降 手動追加 -------------------->>>

//￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣
// ×ボタン
//＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿
void CTO_MAEDATDlg::OnClose()
{
	//CDialog::OnClose();		// 既存の処理をヤメ
}

//￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣
// 強制終了ボタン
//＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿
void CTO_MAEDATDlg::OnBnClickedOk()
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
void CTO_MAEDATDlg::OnLbnDblclkList1()
{
	// ListBoxコントロールのプロパティは、ソート禁止にしておく事
	// ListBoxコントロールのダブルクリックイベントを追加
	mcls_pLog->ShowEdit();
}


//------------------------------------------
// 各プロセス初期化処理
//------------------------------------------
void CTO_MAEDATDlg::InitProcess()
{
	//// 一番初めにいろいろ初期化
	mcls_pLog = NULL;

	//// タスク初期化処理
	//if( 0 != task_init(TO_MAEDAT, 0) ) {		// PCIDがない場合。
	//if( 0 != task_init(TO_MAEDAT) ) {
	//	syslog(SYSNO_TASK_INIT_ERR, "[%s]", TO_MAEDAT);
	//	_ASSERT(0);
	//	exit(-9);		// 自プロセス強制終了
	//	return;
	//}
	int nRetc = task_init(TO_MAEDAT);
	if( 0 != nRetc ) {
		if(-1 == nRetc) { syslog(SYSNO_TASK_INIT_ERR, "[%s] 既にタスク起動済みにより、起動中断。", TO_MAEDAT); }
		else			{ syslog(SYSNO_TASK_INIT_ERR, "[%s]", TO_MAEDAT); }
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
// 各プロセス開放処理
//＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿
void CTO_MAEDATDlg::ExitProcess()
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
unsigned WINAPI CTO_MAEDATDlg::MainThread(void* param)
{
	CTO_MAEDATDlg* p = (CTO_MAEDATDlg *)param;
	COMMON_QUE que;			// 受信構造体

	//// 各プロセス初期化処理 (これ以前はログ出力禁止)
	p->InitProcess();

	//// 起動要因取り込み ループ開始
	while (true) {
		recv_mail( &que );			// 起動要因取り込み

		switch( que.nEventNo ) {

//-----------------------------------------------------------------------------------------------
		//case FACT_TO_MAEDAT_01:									// パラメータ変更要求 (←TO_DEFECT)
		//	pLOG(em_MSG), "[MAIL] パラメータ変更要求通知 (FACT_TO_MAEDAT_01) [テーブル=%d]", que.fl.data[0] ); 
		//	
		//	// コイル受信時に毎回パラメータ読み込みを行っているので
		//	// パラメータ変更要求時には処理を行う必要は無い
		//	break;

//-----------------------------------------------------------------------------------------------
		case FACT_TO_MAEDAT_02:			// コイル情報受信 (←TO_DEFECT)
			pLOG(em_MSG), "[Mail]コイル情報受信通知 (FACT_TO_MAEDAT_02) [管理No=%s]", que.mix.cdata[0] );
			p->mcls_MI.SetDeliveryMail( E_PD_ML_COIL, &que );
			break;

//-----------------------------------------------------------------------------------------------
		case FACT_TO_MAEDAT_03:			// FF前工程疵情報受信通知 (←TO_FFSERV)
			pLOG(em_MSG), "[Mail]FF前工程疵情報受信通知 (FACT_TO_MAEDAT_03) [ID=%s]", que.fc.data );
			p->mcls_MI.SetDeliveryMail( E_PD_ML_FF, &que );
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
void CTO_MAEDATDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: ここにメッセージ ハンドラ コードを追加するか、既定の処理を呼び出します。

	CDialog::OnTimer(nIDEvent);
}

// コイル情報受信通知送信
void CTO_MAEDATDlg::OnBnClickedButton1()
{
	// TODO: ここにコントロール通知ハンドラ コードを追加します。
	// 2014.05.09 諸藤 デバッグボタン追加 ------->>>
	if( BST_CHECKED != CWnd::IsDlgButtonChecked(IDC_DEBUG) ) return;
	// 2014.05.09 諸藤 デバッグボタン追加 <<<-------
	if( IDYES != MessageBox("コイル情報受信通知を送信しますか？", "重要なお知らせ",MB_YESNO | MB_DEFBUTTON2 | MB_ICONQUESTION) ) {
		return;
	}
	COMMON_QUE que;
	memset( &que, 0x00, sizeof(que) );
	que.nEventNo = FACT_TO_MAEDAT_02;
	que.mix.idata[0] = 0;				// 受信レコード位置
	GetDlgItemText( IDC_KizuKenNo, que.mix.cdata[0], SIZE_KIZUKEN_NO );		// 管理No
	que.nLineNo = getlineid();
	
	send_mail(getmytaskname(), ".", &que);
}

// FF前工程受信通知送信
void CTO_MAEDATDlg::OnBnClickedButton2()
{
	// TODO: ここにコントロール通知ハンドラ コードを追加します。
	// 2014.05.09 諸藤 デバッグボタン追加 ------->>>
	if( BST_CHECKED != CWnd::IsDlgButtonChecked(IDC_DEBUG) ) return;
	// 2014.05.09 諸藤 デバッグボタン追加 <<<-------

	if( IDYES != MessageBox("FF前工程受信通知を送信しますか？", "重要なお知らせ",MB_YESNO | MB_DEFBUTTON2 | MB_ICONQUESTION) ) {
		return;
	}
	COMMON_QUE que;
	memset( &que, 0x00, sizeof(que) );
	que.nEventNo = FACT_TO_MAEDAT_03;
	GetDlgItemText( IDC_ID, &que.fc.data[0], SIZE_SERIAL_FF_NO );
	que.nLineNo = getlineid();
	
	send_mail(getmytaskname(), ".", &que);

}
