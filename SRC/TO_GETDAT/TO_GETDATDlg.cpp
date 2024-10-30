// TO_GETDATDlg.cpp : 実装ファイル
//

#include "stdafx.h"
#include "TO_GETDAT.h"
#include "TO_GETDATDlg.h"

// メモリーリーク検出 手動追加 ----->>>
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif
// <<<------------------------


// CTO_GETDATDlg ダイアログ




CTO_GETDATDlg::CTO_GETDATDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CTO_GETDATDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CTO_GETDATDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, m_ListBox);
}

BEGIN_MESSAGE_MAP(CTO_GETDATDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDOK, &CTO_GETDATDlg::OnBnClickedOk)
	ON_LBN_DBLCLK(IDC_LIST1, &CTO_GETDATDlg::OnLbnDblclkList1)
	ON_WM_CLOSE()
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_RESEND, &CTO_GETDATDlg::OnBnClickedResend)
END_MESSAGE_MAP()


// CTO_GETDATDlg メッセージ ハンドラ

BOOL CTO_GETDATDlg::OnInitDialog()
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

void CTO_GETDATDlg::OnPaint()
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
HCURSOR CTO_GETDATDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

// これ以降 手動追加 -------------------->>>

//￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣
// ×ボタン
//＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿
void CTO_GETDATDlg::OnClose()
{
	//CDialog::OnClose();		// 既存の処理をヤメ
}

//￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣
// 強制終了ボタン
//＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿
void CTO_GETDATDlg::OnBnClickedOk()
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
void CTO_GETDATDlg::OnLbnDblclkList1()
{
	// ListBoxコントロールのプロパティは、ソート禁止にしておく事
	// ListBoxコントロールのダブルクリックイベントを追加
	mcls_pLog->ShowEdit();
}

//------------------------------------------
// 各プロセス初期化処理
//------------------------------------------
void CTO_GETDATDlg::InitProcess()
{
	//// タスク初期化処理
	if( 0 != task_init(TO_GETDAT) ) {
		syslog(SYSNO_TASK_INIT_ERR, "[%s]", TO_GETDAT);
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
void CTO_GETDATDlg::ExitProcess()
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
unsigned WINAPI CTO_GETDATDlg::MainThread(void* param)
{
	CTO_GETDATDlg* p = (CTO_GETDATDlg *)param;
	COMMON_QUE que;			// 受信構造体

	//// 各プロセス初期化処理 (これ以前はログ出力禁止)
	p->InitProcess();

	//// 起動要因取り込み ループ開始
	while(true) {
		recv_mail(&que);			// 起動要因取り込み

		switch(que.nEventNo) {
//-----------------------------------------------------------------------------------------------
		//case FACT_TO_GETDAT_01:									// パラメータ変更要求 (←TO_DEFECT)
		//	pLOG(em_MSG), "[MAIL] パラメータ変更要求通知 (FACT_TO_GETDAT_01) [テーブル=%d]", que.fl.data[0] ); 
		//	
		//	// コイル受信時に毎回パラメータ読み込みを行っているので
		//	// パラメータ変更要求時には処理を行う必要は無い
		//	break;

//-----------------------------------------------------------------------------------------------
		case FACT_TO_GETDAT_02:									// コイル情報受信通知 (←TO_SEQDM)
			pLOG(em_MSG), "[MAIL] コイル情報受信通知 (FACT_TO_GETDAT_02) [受信レコード=%d]", que.fl.data[0] ); 
			if (0 != que.fl.data[0] && 1 != que.fl.data[0]) {
				pLOG(em_ERR), "[MAIL] コイル情報 レコード位置異常 [%d]", que.fl.data[0]);
				break;		// 対象外
			}

			// メインインスタンスへ通知
			p->mcls_MI.SetDeliveryMail( E_DEF_ML_COIL, &que);
			break;

//-----------------------------------------------------------------------------------------------
		case FACT_KS_PCNAME_CHANGE:								// PC名称変更通知 (←KS_SLAVE)
			// 無処理
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
void CTO_GETDATDlg::OnTimer(UINT_PTR nIDEvent)
{
	// 最新コイル
	SetDlgItemText(IDC_COILNO,	mcls_MI.gcls_pCoil->GetNewCoil()->basedata.cCoilNoIn);

	SetDlgItemInt(IDC_PCODE1,	mcls_MI.gcls_pCoil->GetNewCoil()->setting.pcode[0] );
	SetDlgItemInt(IDC_PCODE2,	mcls_MI.gcls_pCoil->GetNewCoil()->setting.pcode[1] );

	SetDlgItemInt(IDC_SCODE1,	mcls_MI.gcls_pCoil->GetNewCoil()->setting.scode[0] );
	SetDlgItemInt(IDC_SCODE2,	mcls_MI.gcls_pCoil->GetNewCoil()->setting.scode[1] );

	SetDlgItemInt(IDC_PCODE1_SPMOFF,	mcls_MI.gcls_pCoil->GetNewCoil()->setting.pcode_spmoff[0] );
	SetDlgItemInt(IDC_PCODE2_SPMOFF,	mcls_MI.gcls_pCoil->GetNewCoil()->setting.pcode_spmoff[1] );

	SetDlgItemInt(IDC_SCODE1_SPMOFF,	mcls_MI.gcls_pCoil->GetNewCoil()->setting.scode_spmoff[0] );
	SetDlgItemInt(IDC_SCODE2_SPMOFF,	mcls_MI.gcls_pCoil->GetNewCoil()->setting.scode_spmoff[1] );

	CDialog::OnTimer(nIDEvent);
}

//￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣
// コイル情報再送信ボタン
//＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿
void CTO_GETDATDlg::OnBnClickedResend()
{
	if( IDYES != MessageBox("[デバッグ] コイル情報を再送しますか？", "重要なお知らせ",MB_YESNO | MB_DEFBUTTON2 | MB_ICONQUESTION) ) {
		return;
	}

	//// 入力データ編集管理に通知
	COMMON_QUE		que;
	memset(&que, 0x00, sizeof(que));
	que.nEventNo = FACT_TO_GETDAT_02;	// コイル情報受信通知 (←TO_SEQDM)
	que.nLineNo = getlineid();

	int nRec;
	char cRec[20];
	memset(cRec,0x00,sizeof(cRec));
	GetDlgItemText( IDC_READREC , cRec , sizeof(cRec) );
	nRec = atoi(cRec);

	que.fl.data[0] = nRec;

	send_mail(TO_GETDAT, ".", &que);	
}
