// KS_MAILTRDlg.cpp : 実装ファイル
//

#include "stdafx.h"
#include "KS_MAILTR.h"
#include "KS_MAILTRDlg.h"

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


// CKS_MAILTRDlg ダイアログ




CKS_MAILTRDlg::CKS_MAILTRDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CKS_MAILTRDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CKS_MAILTRDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, m_ListBox);
}

BEGIN_MESSAGE_MAP(CKS_MAILTRDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDOK, &CKS_MAILTRDlg::OnBnClickedOk)
	ON_LBN_DBLCLK(IDC_LIST1, &CKS_MAILTRDlg::OnLbnDblclkList1)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_DEBUG_MANUAL, &CKS_MAILTRDlg::OnBnClickedDebugManual)
	ON_BN_CLICKED(IDC_BTN_TRANS_FL, &CKS_MAILTRDlg::OnBnClickedBtnTransFl)
	ON_BN_CLICKED(IDC_BTN_TRANS_FC, &CKS_MAILTRDlg::OnBnClickedBtnTransFc)
	ON_BN_CLICKED(IDC_BTN_TRANS_MIX, &CKS_MAILTRDlg::OnBnClickedBtnTransMix)
	ON_BN_CLICKED(IDC_BTN_CLEAR_R, &CKS_MAILTRDlg::OnBnClickedBtnClearR)
END_MESSAGE_MAP()


// CKS_MAILTRDlg メッセージ ハンドラ

BOOL CKS_MAILTRDlg::OnInitDialog()
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
// 画面初期化セット----------------------------------------->>>
	SetDlgItemTextA(IDC_PC, ".");
	SetDlgItemTextA(IDC_LINE, "0");
	GetDlgItem(IDC_IPADDR_UDP)->EnableWindow(false);
// <<<---------------------------------------------------------
	
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

void CKS_MAILTRDlg::OnPaint()
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
HCURSOR CKS_MAILTRDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


// これ以降 手動追加 -------------------->>>

//￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣
// ×ボタン
//＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿
void CKS_MAILTRDlg::OnClose()
{
	//CDialog::OnClose();		// 既存の処理をヤメ
}

//￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣
// 強制終了ボタン
//＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿
void CKS_MAILTRDlg::OnBnClickedOk()
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
void CKS_MAILTRDlg::OnLbnDblclkList1()
{
	// ListBoxコントロールのプロパティは、ソート禁止にしておく事
	// ListBoxコントロールのダブルクリックイベントを追加
	mcls_pLog->ShowEdit();
}


//------------------------------------------
// 各プロセス初期化処理
//------------------------------------------
void CKS_MAILTRDlg::InitProcess()
{
	//// 一番初めにいろいろ初期化
	mcls_pLog = NULL;

	int nRetc = task_init(KS_MAILTR);
	if( 0 != nRetc ) {
		if(-1 == nRetc) { syslog(SYSNO_TASK_INIT_ERR, "[%s] 既にタスク起動済みにより、起動中断。", KS_MAILTR); }
		else			{ syslog(SYSNO_TASK_INIT_ERR, "[%s]", KS_MAILTR); }
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
void CKS_MAILTRDlg::ExitProcess()
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
unsigned WINAPI CKS_MAILTRDlg::MainThread(void* param)
{
	CKS_MAILTRDlg* p = (CKS_MAILTRDlg *)param;
	COMMON_QUE que;			// 受信構造体

	//// 各プロセス初期化処理 (これ以前はログ出力禁止)
	p->InitProcess();

	//// 起動要因取り込み ループ開始
	while(true) {
		recv_mail(&que);			// 起動要因取り込み

		switch(que.nEventNo) {
//*/
//-----------------------------------------------------------------------------------------------
		case FACT_KS_MAILTR_01:								// メールスロット転送要求 (←各タスク)
			pLOG(em_MSG), "[MAIL] メールスロット転送要求 (FACT_KS_MAILTR_01) [Task=%s][Pc=%s]", que.mailtr_que.cTaskName, que.mailtr_que.cPcName); 
			// メインインスタンスへ通知
			p->mcls_MI.SetDeliveryMail( E_DEF_ML_TRANS, &que);
			break;
//*/
//-----------------------------------------------------------------------------------------------
		case FACT_KS_DB_STATEUS:							// DB状態変更通知(←KS_SLAVE)
			break;

		case FACT_KS_PCNAME_CHANGE:								// PC名称変更通知 (←KS_SLAVE)
			pLOG(em_MSG), "[MAIL] PC名称変更通知 (FACT_KS_PCNAME_CHANGE)" );
			// PC名称取得
			getpcname(); 
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
void CKS_MAILTRDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: ここにメッセージ ハンドラ コードを追加するか、既定の処理を呼び出します。

	// UDP転送要求 受信情報取得
	UdpMail* tRecvUdp = mcls_MI.gcls_TrUdpMgr.GetRecvUdp();
	COMMON_QUE	que;
	CString		ss;

	memcpy(&que, tRecvUdp->data, min(sizeof(que), tRecvUdp->datalen));

	SetDlgItemTextA(IDC_PC_R, tRecvUdp->hostname);
	SetDlgItemTextA(IDC_TASK_R, tRecvUdp->mailname);
	ss.Format("%d", que.nEventNo);
	SetDlgItemTextA(IDC_EVENT_R, ss);
	ss.Format("%d", que.nLineNo);
	SetDlgItemTextA(IDC_LINE_R, ss);

	CDialog::OnTimer(nIDEvent);
}

//￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣
// DEBUGボタン押下
//＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿
void CKS_MAILTRDlg::OnBnClickedDebugManual()
{
	bool bVal = ( BST_CHECKED == CWnd::IsDlgButtonChecked(IDC_DEBUG_MANUAL) ? true : false );
	GetDlgItem(IDC_BTN_TRANS_FL)->EnableWindow(bVal);
	GetDlgItem(IDC_BTN_TRANS_FC)->EnableWindow(bVal);
	GetDlgItem(IDC_BTN_TRANS_MIX)->EnableWindow(bVal);
	GetDlgItem(IDC_IPADDR_UDP)->EnableWindow(bVal);
	GetDlgItem(IDC_PC)->EnableWindow(bVal);
	GetDlgItem(IDC_TASK)->EnableWindow(bVal);
	GetDlgItem(IDC_EVENT)->EnableWindow(bVal);
	GetDlgItem(IDC_LINE)->EnableWindow(bVal);
	GetDlgItem(IDC_FL_1)->EnableWindow(bVal);
	GetDlgItem(IDC_FC_1)->EnableWindow(bVal);
	GetDlgItem(IDC_BTN_CLEAR_R)->EnableWindow(bVal);
}

//￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣
// 数値転送ボタン押下
//＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿
void CKS_MAILTRDlg::OnBnClickedBtnTransFl()
{
	COMMON_QUE	que;
	CString		sUdpAddr;
	CString		sTaskName;
	CString		sPcName;

	GetDlgItemTextA(IDC_IPADDR_UDP, sUdpAddr);
	GetDlgItemTextA(IDC_TASK, sTaskName);
	GetDlgItemTextA(IDC_PC, sPcName);
	memset(&que, 0x00, sizeof(que));
	que.nEventNo = GetDlgItemInt(IDC_EVENT);
	que.nLineNo = GetDlgItemInt(IDC_LINE);
	que.fl.data[0] = GetDlgItemInt(IDC_FL_1);

	// メール転送
	send_mail_trans(sUdpAddr, sTaskName, sPcName, &que);
}

//￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣
// 文字転送ボタン押下
//＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿
void CKS_MAILTRDlg::OnBnClickedBtnTransFc()
{
	COMMON_QUE	que;
	CString		sUdpAddr;
	CString		sTaskName;
	CString		sPcName;

	GetDlgItemTextA(IDC_IPADDR_UDP, sUdpAddr);
	GetDlgItemTextA(IDC_TASK, sTaskName);
	GetDlgItemTextA(IDC_PC, sPcName);
	memset(&que, 0x00, sizeof(que));
	que.nEventNo = GetDlgItemInt(IDC_EVENT);
	que.nLineNo = GetDlgItemInt(IDC_LINE);
	GetDlgItemTextA(IDC_FC_1, que.fc.data, sizeof(que.fc.data));

	// メール転送
	send_mail_trans(sUdpAddr, sTaskName, sPcName, &que);
}

//￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣
// 混合転送ボタン押下
//＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿
void CKS_MAILTRDlg::OnBnClickedBtnTransMix()
{
	COMMON_QUE	que;
	CString		sUdpAddr;
	CString		sTaskName;
	CString		sPcName;

	GetDlgItemTextA(IDC_IPADDR_UDP, sUdpAddr);
	GetDlgItemTextA(IDC_TASK, sTaskName);
	GetDlgItemTextA(IDC_PC, sPcName);
	memset(&que, 0x00, sizeof(que));
	que.nEventNo = GetDlgItemInt(IDC_EVENT);
	que.nLineNo = GetDlgItemInt(IDC_LINE);
	que.mix.idata[0] = GetDlgItemInt(IDC_FL_1);
	GetDlgItemTextA(IDC_FC_1, que.mix.cdata[0], sizeof(que.mix.cdata[0]));

	// メール転送
	send_mail_trans(sUdpAddr, sTaskName, sPcName, &que);
}

//￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣
// 表示クリアボタン押下
//＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿
void CKS_MAILTRDlg::OnBnClickedBtnClearR()
{
	// 受信情報クリア
	mcls_MI.gcls_TrUdpMgr.ClearRecvUdp();
}
