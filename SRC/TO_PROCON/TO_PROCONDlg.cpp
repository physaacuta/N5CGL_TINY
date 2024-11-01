// TO_PROCONDlg.cpp : 実装ファイル
//

#include "stdafx.h"
#include "TO_PROCON.h"
#include "TO_PROCONDlg.h"

//#ifdef _DEBUG
//#define new DEBUG_NEW
//#endif
// メモリーリーク検出 手動追加 ----->>>
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif
// <<<------------------------


// CTO_PROCONDlg ダイアログ




CTO_PROCONDlg::CTO_PROCONDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CTO_PROCONDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CTO_PROCONDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, m_ListBox);
}

BEGIN_MESSAGE_MAP(CTO_PROCONDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDOK, &CTO_PROCONDlg::OnBnClickedOk)
	ON_WM_CLOSE()
	ON_WM_TIMER()
	ON_LBN_DBLCLK(IDC_LIST1, &CTO_PROCONDlg::OnLbnDblclkList1)
//	ON_EN_CHANGE(IDC_STATE_PORT_RECV, &CTO_PROCONDlg::OnEnChangeStatePortRecv)
ON_BN_CLICKED(IDC_DUMP1, &CTO_PROCONDlg::OnBnClickedDump1)
ON_BN_CLICKED(IDC_DUMP2, &CTO_PROCONDlg::OnBnClickedDump2)
ON_BN_CLICKED(IDC_CHK_DEBUG_FUNC, &CTO_PROCONDlg::OnBnClickedChkDebugFunc)
ON_BN_CLICKED(IDC_CHK_LOG_ALLDUMP, &CTO_PROCONDlg::OnBnClickedChkLogAlldump)
ON_BN_CLICKED(IDC_BTN_TCP_CLOSE, &CTO_PROCONDlg::OnBnClickedBtnTcpClose)
ON_BN_CLICKED(IDC_BTN_DBG_SEND, &CTO_PROCONDlg::OnBnClickedBtnDbgSend)
ON_BN_CLICKED(IDC_BTN_DBG_RECV, &CTO_PROCONDlg::OnBnClickedBtnDbgRecv)
END_MESSAGE_MAP()


// CTO_PROCONDlg メッセージ ハンドラ

BOOL CTO_PROCONDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// このダイアログのアイコンを設定します。アプリケーションのメイン ウィンドウがダイアログでない場合、
	//  Framework は、この設定を自動的に行います。
	SetIcon(m_hIcon, TRUE);			// 大きいアイコンの設定
	SetIcon(m_hIcon, FALSE);		// 小さいアイコンの設定

	// TODO: 初期化をここに追加します。
	
	//// 画面初期設定
	CWnd::CheckRadioButton(IDC_DUMP1, IDC_DUMP2, IDC_DUMP1);	// ダンプ出力状態

// ------------------------------->>>
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

void CTO_PROCONDlg::OnPaint()
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
HCURSOR CTO_PROCONDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}






// これ以降 手動追加 -------------------->>>

//￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣
// ×ボタン
//＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿
void CTO_PROCONDlg::OnClose()
{
	//CDialog::OnClose();
}
//￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣
// 閉じるボタン
//＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿
void CTO_PROCONDlg::OnBnClickedOk()
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
void CTO_PROCONDlg::OnLbnDblclkList1()
{
	// ListBoxコントロールのプロパティは、ソート禁止にしておく事
	// ListBoxコントロールのダブルクリックイベントを追加
	mcls_pLog->ShowEdit();
}

//------------------------------------------
// 各プロセス初期化処理
//------------------------------------------
void CTO_PROCONDlg::InitProcess()
{
	// タスク初期化処理
	setlineid(LINE_ID);
	if( 0 != task_init(TO_PROCON) ) {
		syslog(SYSNO_TASK_INIT_ERR, "[%s]", TO_PROCON);
		_ASSERT(0);
		exit(-9);		// 自プロセス強制終了
		return;
	}

	// LogFileManagerインスタンス生成 (これ以降 ログ出力可能)
	setlocale(LC_ALL, "Japanese");	// これが無いと日本語パスを含むフォルダの場合、保存できない。(ライブラリの中で実行するべきか迷っているが、変更するとタスク全体に影響するので、とりあえずここで設定)
	mcls_pLog = new LogFileManager(getmytaskname(), &m_ListBox);
	LOG(em_MSG), "[Dlg] タスク起動");
	LOG(em_MSG), "[Dlg] 各プロセス開始処理 実行中・・・");

	// メインインスタンス 開始処理
	mcls_MI.SetLogMgr(mcls_pLog);
	mcls_MI.Init();
	
	//// 画面更新用タイマ生成
	SetTimer(1, 500, NULL);

	// メインスレッドの実行を行う
	LOG(em_MSG), "[Dlg] 各プロセス開始処理 完了");
	mcls_MI.Start();
	return;
}

//￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣
// 各プロセス開放処理
//＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿
void CTO_PROCONDlg::ExitProcess()
{
	//// ログ リストボックス表示処理終了
	mcls_pLog->SetListOut(false);					// これ以降 リストボックスへのログ表示禁止

	// メインインスタンス 終了処理
	KillTimer(1);			// 画面更新タイマ
	mcls_MI.Stop();
	mcls_MI.Exit();

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
unsigned WINAPI CTO_PROCONDlg::MainThread(void* param)
{
	CTO_PROCONDlg* p = (CTO_PROCONDlg *)param;
	COMMON_QUE que;			// 受信構造体

	//// 各プロセス初期化処理 (これ以前はログ出力禁止)
	p->InitProcess();

	//// 起動要因取り込み ループ開始
	while(true) {
		recv_mail(&que);			// 起動要因取り込み

		switch(que.nEventNo) {

//-----------------------------------------------------------------------------------------------
		case FACT_TO_PROCON_01:								// プロコン検査実績送信要求 (←TO_DEFECT)
			if(true) {
				pLOG(em_MSG), "[Mail] 実績送信伝文送信通知 (FACT_TO_PROCON_01) [%s]", que.fc.data);
				p->mcls_MI.gcls_pSock->AddQue_Ziseki(&que);
			}
			break;

//-----------------------------------------------------------------------------------------------
		case FACT_TO_PROCON_09:						// デバック用伝文受信通知 (←GOT_CONTROL)
			pLOG(em_MSG), "==========================================================================");
			pLOG(em_MSG), "[Mail] デバック用伝文受信通知 (FACT_TO_PROCON_09) [TC=%d][%s]", que.mix.idata[0], que.mix.cdata[0]);

			// メインインスタンスへ通知
			p->mcls_MI.SetDeliveryMail(E_DEF_DEBUG_RECV, &que);

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
			syslog(SYSNO_MAILNO_ERR, "[EventNo=%d]", que.nEventNo);
			break;
		}
	}

	return 0;
}

//￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣
// 画面更新タイマ
//＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿
void CTO_PROCONDlg::OnTimer(UINT_PTR nIDEvent)
{
	CString strVal;	
	// TODO: ここにメッセージ ハンドラ コードを追加するか、既定の処理を呼び出します。

	// ステータス
	SetDlgItemText(IDC_STATE_PORT_RECV, mcls_MI.gcls_pSock->GetStateName(0) );
	SetDlgItemText(IDC_STATE_PORT_SEND, mcls_MI.gcls_pSock->GetStateName(1) );
	SetDlgItemText(IDC_STATE_PORT_RECV2, mcls_MI.gcls_pSock->GetSockStatName(0));
	SetDlgItemText(IDC_STATE_PORT_SEND2, mcls_MI.gcls_pSock->GetSockStatName(1));
	// TCP状態
	SetDlgItemText(IDC_EDIT_TCP_R, (mcls_MI.gcls_pSock->IsConnect(0) ? "○" : "×"));
	SetDlgItemText(IDC_EDIT_TCP_S, (mcls_MI.gcls_pSock->IsConnect(1) ? "○" : "×"));

	CDialog::OnTimer(nIDEvent);
}

//￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣
// ダンプ出力 1件
//＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿
void CTO_PROCONDlg::OnBnClickedDump1()
{
	
	// ダンプ出力モードセット
	mcls_MI.gcls_pSock->SetDumpMode(false);
}

//￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣
// ダンプ出力 全件
//＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿
void CTO_PROCONDlg::OnBnClickedDump2()
{
	// ダンプ出力モードセット
	mcls_MI.gcls_pSock->SetDumpMode(true);
}

//￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣
// [デバッグ用Function]チェックボックス
//＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿
void CTO_PROCONDlg::OnBnClickedChkDebugFunc()
{
	bool bVal = (BST_CHECKED == CWnd::IsDlgButtonChecked(IDC_CHK_DEBUG_FUNC) ? true : false);
	GetDlgItem(IDC_CHK_LOG_ALLDUMP)->EnableWindow(bVal);
	GetDlgItem(IDC_BTN_TCP_CLOSE)->EnableWindow(bVal);
	GetDlgItem(IDC_BTN_DBG_SEND)->EnableWindow(bVal);
	GetDlgItem(IDC_BTN_DBG_RECV)->EnableWindow(bVal);
	GetDlgItem(IDC_EDIT_ID)->EnableWindow(bVal);
	GetDlgItem(IDC_EDIT_DBG_TC)->EnableWindow(bVal);
	GetDlgItem(IDC_EDIT_DBG_BINFILE)->EnableWindow(bVal);
	GetDlgItem(IDC_STATIC_ID)->EnableWindow(bVal);
	GetDlgItem(IDC_STATIC_TC)->EnableWindow(bVal);
	GetDlgItem(IDC_STATIC_BINFILE)->EnableWindow(bVal);
	GetDlgItem(IDC_LIST1)->SetFocus();
}



//￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣
// [ログ全件出力]チェックボックスイベント 
//＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿
void CTO_PROCONDlg::OnBnClickedChkLogAlldump()
{
	bool bVal = (BST_CHECKED == CWnd::IsDlgButtonChecked(IDC_CHK_LOG_ALLDUMP) ? true : false);
	mcls_MI.gcls_pSock->SetDumpMode(bVal);
	mcls_MI.gcls_pSock->SetDumpMode(bVal);
}

//￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣
// [強制切断]ボタンイベント
//＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿
void CTO_PROCONDlg::OnBnClickedBtnTcpClose()
{
	//	if( BST_CHECKED != CWnd::IsDlgButtonChecked(IDC_DEBUG_MANUAL) ) return;
	LOG(em_WAR), "テスト 強制切断");
	mcls_MI.gcls_pSock->UserClose(false);
}

//￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣
// 擬似 送信処理
//＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿
void CTO_PROCONDlg::OnBnClickedBtnDbgSend()
{
	if (IDYES != MessageBox("テスト 実績伝文を送信しますか？", "重要なお知らせ", MB_YESNO | MB_DEFBUTTON2 | MB_ICONQUESTION)) {
		return;
	}
	COMMON_QUE que;
	memset(&que, 0x00, sizeof(que));
	que.nEventNo = FACT_TO_PROCON_01;
	que.nLineNo = getlineid();
	GetDlgItemText(IDC_EDIT_ID, &que.fc.data[0], SIZE_SERIAL_FF_NO);
	send_mail(getmytaskname(), ".", &que);
}

//￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣
// 擬似 受信処理
//＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿
void CTO_PROCONDlg::OnBnClickedBtnDbgRecv()
{
	if (IDYES != MessageBox("受信しますか？", "重要なお知らせ", MB_YESNO | MB_DEFBUTTON2 | MB_ICONQUESTION)) {
		return;
	}

		COMMON_QUE que;
		memset( &que, 0x00, sizeof(que) );
		que.nEventNo = FACT_TO_PROCON_09;
		que.nLineNo = getlineid();
	
		char cWk1[256];
		char cWk2[256];
		memset( cWk1, 0x00, sizeof(cWk1) );
		memset( cWk2, 0x00, sizeof(cWk2) );
		GetDlgItemText( IDC_EDIT_DBG_TC     , cWk1, sizeof(cWk1) );
		GetDlgItemText( IDC_EDIT_DBG_BINFILE, cWk2, sizeof(cWk2) );
	
	//	que.mix.idata[0] = atoi( cWk1 );
		que.mix.idata[0] = strtol( cWk1 , NULL, 0); // 10進/16進(0x??)入力対応
		memcpy( que.mix.cdata[0], cWk2, sizeof( que.mix.cdata[0]) * 5);
		send_mail( getmytaskname(), ".", &que );
}

