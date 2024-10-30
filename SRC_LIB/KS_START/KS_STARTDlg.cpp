// KS_STARTDlg.cpp : 実装ファイル
//

#include "stdafx.h"
#include "KS_START.h"
#include "KS_STARTDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CKS_STARTDlg ダイアログ




CKS_STARTDlg::CKS_STARTDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CKS_STARTDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CKS_STARTDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, m_ListBox);
}

BEGIN_MESSAGE_MAP(CKS_STARTDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDOK, &CKS_STARTDlg::OnBnClickedOk)
	ON_LBN_DBLCLK(IDC_LIST1, &CKS_STARTDlg::OnLbnDblclkList1)
END_MESSAGE_MAP()


// CKS_STARTDlg メッセージ ハンドラ

BOOL CKS_STARTDlg::OnInitDialog()
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
	// 画面位置
	//::SetWindowPos(m_hWnd, NULL, 1, 1, 10, 10, SWP_NOZORDER);
	::SetWindowPos(m_hWnd, NULL, 1, 1, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
	// メインスレッド開始
	unsigned int tid;	// スレッドID
	_beginthreadex(NULL, 0, MainThread, (LPVOID)this, 0, &tid);
// <<<-------------------------------
	return TRUE;  // フォーカスをコントロールに設定した場合を除き、TRUE を返します。
}

// ダイアログに最小化ボタンを追加する場合、アイコンを描画するための
//  下のコードが必要です。ドキュメント/ビュー モデルを使う MFC アプリケーションの場合、
//  これは、Framework によって自動的に設定されます。

void CKS_STARTDlg::OnPaint()
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
HCURSOR CKS_STARTDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


// これ以降 手動追加 -------------------->>>

//￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣
// ×ボタン
//＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿
void CKS_STARTDlg::OnClose()
{
	//CDialog::OnClose();
}
//￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣
// 閉じるボタン
//＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿
void CKS_STARTDlg::OnBnClickedOk()
{
	if( IDYES == MessageBox("終了しますか？", "重要なお知らせ", MB_YESNO | MB_DEFBUTTON2 | MB_ICONQUESTION) ) {
		// 強制終了通知
		m_bCancel = true;
		LOG(em_MSG), "強制終了を行います。暫くお待ち下さい。");

		CDialog::OnClose();
	}
}

//------------------------------------------
// メインスレッド DlgThread
// LPVOID param ダイヤログポインタ
//------------------------------------------
unsigned WINAPI CKS_STARTDlg::MainThread(void* param)
{
	CKS_STARTDlg* p = (CKS_STARTDlg *)param;

	// LogFileManagerインスタンス生成 (これ以降 ログ出力可能)
	setlocale(LC_ALL, "Japanese");	// これが無いと日本語パスを含むフォルダの場合、保存できない。(ライブラリの中で実行するべきか迷っているが、変更するとタスク全体に影響するので、とりあえずここで設定)
	p->mcls_pLog = new LogFileManager(KS_START, &p->m_ListBox);
	pLOG(em_MSG), "タスク起動");


	//// リモートデスクトップ接続による、２重起動対策
	bool bExec = true;
	bExec &= p->CheckMultiExec(KS_MASTER);
	bExec &= p->CheckMultiExec(KS_SLAVE);


	//// メイン処理
	if(bExec) {
		p->m_bCancel = false;
		MainExec*	cls_pExec = new MainExec(p->mcls_pLog, &p->m_bCancel);
		cls_pExec->DoExec();
		delete cls_pExec;
	}

	//// ログクラス開放
	pLOG(em_MSG), "タスク終了");
	Sleep(100);
	if(NULL != p->mcls_pLog){
		delete p->mcls_pLog;
		p->mcls_pLog = NULL;
	}

	// 画面の終了要求
	p->PostMessage(WM_QUIT);

	return 0;
}
//￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣
// メモ帳起動
//＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿
void CKS_STARTDlg::OnLbnDblclkList1()
{
	// ListBoxコントロールのプロパティは、ソート禁止にしておく事
	// ListBoxコントロールのダブルクリックイベントを追加
	mcls_pLog->ShowEdit();
}

//------------------------------------------
// 常駐タスク 複数実行チェック
// LPCSTR SendSlotName 接続チェックタスク名称
// 戻り値: false:既に実行中
//------------------------------------------
bool CKS_STARTDlg::CheckMultiExec(LPCSTR SendSlotName)
{
	HANDLE hSend = INVALID_HANDLE_VALUE;
	char fullname[MAX_PATH];

	// 接続確認
	sprintf(fullname, "\\\\.\\mailslot\\%s", SendSlotName);
	hSend = CreateFile(fullname, 
						GENERIC_WRITE, 
						FILE_SHARE_WRITE | FILE_SHARE_READ,
						NULL, 
						OPEN_EXISTING, 
						FILE_ATTRIBUTE_NORMAL,	
						NULL);
	if(hSend == INVALID_HANDLE_VALUE) return true;		// 常駐タスク無し

	// 接続できちゃったから、切断
	CloseHandle(hSend);
	return false;
}
