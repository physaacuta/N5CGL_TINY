// TO_FFSERVDlg.cpp : 実装ファイル
//

#include "stdafx.h"
#include "TO_FFSERV.h"
#include "TO_FFSERVDlg.h"


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


// CTO_FFSERVDlg ダイアログ




CTO_FFSERVDlg::CTO_FFSERVDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CTO_FFSERVDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CTO_FFSERVDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_LOG, m_ListBox);
}

BEGIN_MESSAGE_MAP(CTO_FFSERVDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_LBN_DBLCLK(IDC_LIST_LOG, &CTO_FFSERVDlg::OnLbnDblclkListLog)
	ON_WM_CLOSE()
	ON_WM_TIMER()
	ON_BN_CLICKED(IDOK, &CTO_FFSERVDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDC_SEND_ZISEKI, &CTO_FFSERVDlg::OnBnClickedSendZiseki)
	ON_BN_CLICKED(IDC_SEND_MAE, &CTO_FFSERVDlg::OnBnClickedSendMae)
	ON_BN_CLICKED(IDC_DB, &CTO_FFSERVDlg::OnBnClickedDb)
	ON_BN_CLICKED(IDC_BIN, &CTO_FFSERVDlg::OnBnClickedBin)
	ON_BN_CLICKED(IDC_CHK_LOG_ALLDUMP, &CTO_FFSERVDlg::OnBnClickedChkLogAlldump)
	ON_BN_CLICKED(IDC_CHK_DEBUG_FUNCTION, &CTO_FFSERVDlg::OnBnClickedChkDebugFunction)
END_MESSAGE_MAP()


// CTO_FFSERVDlg メッセージ ハンドラ

BOOL CTO_FFSERVDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// このダイアログのアイコンを設定します。アプリケーションのメイン ウィンドウがダイアログでない場合、
	//  Framework は、この設定を自動的に行います。
	SetIcon(m_hIcon, TRUE);			// 大きいアイコンの設定
	SetIcon(m_hIcon, FALSE);		// 小さいアイコンの設定

	// TODO: 初期化をここに追加します。
	//// 画面初期設定
	CWnd::CheckRadioButton(IDC_DB, IDC_BIN, IDC_DB);			// テストデータの取得先


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

void CTO_FFSERVDlg::OnPaint()
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
HCURSOR CTO_FFSERVDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

// これ以降 手動追加 -------------------->>>

//￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣
// ×ボタン
//＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿
void CTO_FFSERVDlg::OnClose()
{
	//CDialog::OnClose();		// 既存の処理をヤメ
}

//￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣
// 強制終了ボタン
//＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿
void CTO_FFSERVDlg::OnBnClickedOk()
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
void CTO_FFSERVDlg::OnLbnDblclkListLog()
{
	// ListBoxコントロールのプロパティは、ソート禁止にしておく事
	// ListBoxコントロールのダブルクリックイベントを追加
	mcls_pLog->ShowEdit();
}

//------------------------------------------
// 各プロセス初期化処理
//------------------------------------------
void CTO_FFSERVDlg::InitProcess()
{
	//// 一番初めにいろいろ初期化
	mcls_pLog = NULL;

	//// タスク初期化処理
	//if( 0 != task_init(TO_FFSERV, 0) ) {		// PCIDがない場合。
	//if( 0 != task_init(TO_FFSERV) ) {
	//	syslog(SYSNO_TASK_INIT_ERR, "[%s]", TO_FFSERV);
	//	_ASSERT(0);
	//	exit(-9);		// 自プロセス強制終了
	//	return;
	//}
	int nRetc = task_init(TO_FFSERV);
	if( 0 != nRetc ) {
		if(-1 == nRetc) { syslog(SYSNO_TASK_INIT_ERR, "[%s] 既にタスク起動済みにより、起動中断。", TO_FFSERV); }
		else			{ syslog(SYSNO_TASK_INIT_ERR, "[%s]", TO_FFSERV); }
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
void CTO_FFSERVDlg::ExitProcess()
{
	//// メインインスタンス 終了処理
	KillTimer(1);			// 画面更新タイマ
	mcls_MI.Stop();
	mcls_MI.Exit();

	//// ログ リストボックス表示処理終了
	mcls_pLog->SetListOut(false);					// これ以降 リストボックスへのログ表示禁止

	//// ログクラス開放
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
unsigned WINAPI CTO_FFSERVDlg::MainThread(void* param)
{
	CTO_FFSERVDlg* p = (CTO_FFSERVDlg *)param;
	COMMON_QUE que;			// 受信構造体

	//// 各プロセス初期化処理 (これ以前はログ出力禁止)
	p->InitProcess();

	//// 起動要因取り込み ループ開始
	while(true) {
		recv_mail(&que);			// 起動要因取り込み

		switch(que.nEventNo) {

//-----------------------------------------------------------------------------------------------
		case FACT_TO_FFSERV_01:								// 検査実績送信要求 (←TO_PUTDAT)
			pLOG(em_MSG), "[Mail] 検査実績送信要求 (FACT_TO_FFSERV_01)");

			// 回線切断中は送信要求をキューに溜め込まない、接続時に要求を作成しキューに溜め込む
			if( ! p->mcls_MI.gcls_pSock->IsConnect() ) {
				pLOG(em_ERR), "[Mail] ソケット切断中に 検査実績送信要求 有り");	
				break;
			}
			// メインインスタンスへ通知
			p->mcls_MI.SetDeliveryMail( QUE_SEND_ZISEKI, &que);
			break;


//-----------------------------------------------------------------------------------------------
		case FACT_TO_FFSERV_02:								// 前工程疵情報要求 (←TO_MAEDAT)
			pLOG(em_MSG), "[Mail] 前工程疵情報要求 (FACT_TO_FFSERV_02) [コイルNo=%s][命令No=%s]", que.mix.cdata[3], que.mix.cdata[2]);

			if( ! p->mcls_MI.gcls_pSock->IsConnect() ) {
				pLOG(em_ERR), "[Mail] ソケット切断中に 前工程疵情報要求 有り。[コイルNo=%s][命令No=%s]", que.mix.cdata[3], que.mix.cdata[2]);
				break;
			} else if( (PRO_SIZE_NEXTINF_CNT + 1) <= p->mcls_MI.gcls_pSock->gque_Deli.GetCount() ) {	// バッファーにどんどんたまる対策
				pLOG(em_ERR), "[Mail] 大量要求により 前工程疵情報要求 破棄!! [コイルNo=%s][命令No=%s]", que.mix.cdata[3], que.mix.cdata[2]);
				break;
			}
			// メインインスタンスへ通知
			p->mcls_MI.SetDeliveryMail( QUE_SEND_MAE, &que);
			break;

//-----------------------------------------------------------------------------------------------

		case FACT_KS_PCNAME_CHANGE:							// PC名称変更通知 (←KS_SLAVE)
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
void CTO_FFSERVDlg::OnTimer(UINT_PTR nIDEvent)
{
	// ステータス
	SetDlgItemText(IDC_STATE_SOCK, mcls_MI.gcls_pSock->GetSockStatName() );
	SetDlgItemText(IDC_STATE_SEND, mcls_MI.gcls_pSock->GetSendStatName() );

	CDialog::OnTimer(nIDEvent);
}

//￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣
// [デバッグ用Function]チェックボックス
//＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿
void CTO_FFSERVDlg::OnBnClickedChkDebugFunction()
{
	bool bVal = (BST_CHECKED == CWnd::IsDlgButtonChecked(IDC_CHK_DEBUG_FUNCTION) ? true : false );
	GetDlgItem(IDC_SEND_ZISEKI    )->EnableWindow(bVal);
	GetDlgItem(IDC_DB             )->EnableWindow(bVal);
	GetDlgItem(IDC_BIN            )->EnableWindow(bVal);
	GetDlgItem(IDC_SEND_MAE       )->EnableWindow(bVal);
	GetDlgItem(IDC_STATIC_MEI_NO  )->EnableWindow(bVal);
	GetDlgItem(IDC_EDIT_KEY		  )->EnableWindow(bVal);
	GetDlgItem(IDC_CHK_LOG_ALLDUMP)->EnableWindow(bVal);
	GetDlgItem(IDC_LIST_LOG       )->SetFocus();
}

//￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣
// 検査実績送信ボタン
//＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿
void CTO_FFSERVDlg::OnBnClickedSendZiseki()
{
	if( IDYES != MessageBox("[デバック] 実績伝文を送信しますか？", "重要なお知らせ",MB_YESNO | MB_DEFBUTTON2 | MB_ICONQUESTION) ) {
		return;
	}
	COMMON_QUE que;
	memset(&que, 0x00, sizeof(que));
	que.nEventNo = FACT_TO_FFSERV_01;
	que.nLineNo = getlineid();
	// デバック時 実績表ファイル名称
	strcpy( que.mix.cdata[0], "KizuZissekiOmote.bin" );
	// デバック時 実績裏ファイル名称
	strcpy( que.mix.cdata[1], "KizuZissekiUra.bin" );
	// デバック時 状態表ファイル名称
	strcpy(que.mix.cdata[2], "KizuStOmote.bin");
	// デバック時 状態裏ファイル名称
	strcpy(que.mix.cdata[3], "KizuStUra.bin");
	send_mail(getmytaskname(), ".", &que);
}

//------------------------------------------
// DB指定ボタンクリックイベント
//	
//------------------------------------------
void CTO_FFSERVDlg::OnBnClickedDb()
{
	mcls_MI.gcls_pData->SetReadDataMode( true );
}

//------------------------------------------
// ファイル指定ボタンクリックイベント
//	
//------------------------------------------
void CTO_FFSERVDlg::OnBnClickedBin()
{
	mcls_MI.gcls_pData->SetReadDataMode( false );
}

//￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣
// 前工程疵情報要求送信ボタン
//＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿
void CTO_FFSERVDlg::OnBnClickedSendMae()
{
	CString msg;
	char cKey[SIZE_COIL_KEY+1];				// コイル特定するためのキー情報
	char cLine_Code[SIZE_KOUTEI_CODE+1];	// 工程コード
	PRO_TYPE_KEY typKey;


	// 初期化
	memset(&cKey, 0x20, sizeof(cKey));
	memset(&cLine_Code, 0x00, sizeof(cLine_Code));

	if( IDYES != MessageBox("[デバック] 前工程疵情報要求伝文を送信しますか？", "重要なお知らせ",MB_YESNO | MB_DEFBUTTON2 | MB_ICONQUESTION) ) {
		return;
	}
	// 画面よりキー情報取得
	GetDlgItemText(IDC_EDIT_KEY, cKey, SIZE_COIL_KEY + 1);
	memset(&typKey, 0x00, sizeof(typKey));
	memcpy(&typKey, cKey, sizeof(typKey));

	// 固定
	memcpy(&cLine_Code, KOUTEI_CODE_5CGL, sizeof(KOUTEI_CODE_5CGL));

	COMMON_QUE que;
	memset(&que, 0x00, sizeof(que));
	que.nEventNo = FACT_TO_FFSERV_02;
	que.nLineNo = getlineid();

	memcpy( que.mix.cdata[0], cKey, SIZE_COIL_KEY );			// コイル特定するためのキー情報
	memcpy( que.mix.cdata[1], cLine_Code, SIZE_KOUTEI_CODE );	// 工程コード
	memcpy( que.mix.cdata[2], typKey.cMeiNo, SIZE_MEI_NO );		// 命令No
	memcpy( que.mix.cdata[3], typKey.cCoilNo, SIZE_COIL_NO );	// コイルNo

	send_mail(getmytaskname(), ".", &que);
}

//￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣
// [ログ全件出力]チェックボックスイベント 
//＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿
void CTO_FFSERVDlg::OnBnClickedChkLogAlldump()
{
	bool bVal = (BST_CHECKED == CWnd::IsDlgButtonChecked(IDC_CHK_LOG_ALLDUMP) ? true : false );
	mcls_MI.gcls_pSock->SetDumpMode(bVal);
}
