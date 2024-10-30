// DummyLEDDlg.cpp : 実装ファイル
//

#include "stdafx.h"
#include "DummyLED.h"
#include "DummyLEDDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif
// メモリーリーク検出 手動追加 ----->>>
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif
// <<<------------------------


// CDummyLEDDlg ダイアログ




CDummyLEDDlg::CDummyLEDDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CDummyLEDDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CDummyLEDDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, m_ListBox);
	DDX_Control(pDX, IDC_SESSION, m_cmbSession);
	DDX_Control(pDX, IDC_COMBO3, m_cmbCh);
	DDX_Control(pDX, IDC_DEBUG, m_btnDebug);
	DDX_Control(pDX, IDC_COMBO4, debug_ch);
	DDX_Control(pDX, IDC_COMBO5, debug_uni);
	DDX_Control(pDX, IDC_STATE_TEMP, debug_tmp);
	DDX_Control(pDX, IDC_STATE_ONOFF_CHK, StatOnOff);
}

BEGIN_MESSAGE_MAP(CDummyLEDDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDOK, &CDummyLEDDlg::OnBnClickedOk)
	ON_LBN_DBLCLK(IDC_LIST1, &CDummyLEDDlg::OnLbnDblclkList1)
	ON_WM_TIMER()
//	ON_BN_CLICKED(IDC_DEBUG_BREAK, &CDummyLEDDlg::OnBnClickedDebugBreak)
	ON_BN_CLICKED(IDC_BTN_TEMP, &CDummyLEDDlg::OnBnClickedBtnTemp)
	ON_BN_CLICKED(IDC_BTN_BREAK, &CDummyLEDDlg::OnBnClickedBtnBreak)
	ON_BN_CLICKED(IDC_STATE_ONOFF_CHK, &CDummyLEDDlg::OnBnClickedStateOnoff)
	ON_CBN_SELCHANGE(IDC_COMBO4, &CDummyLEDDlg::OnCbnSelchangeCombo4)
	ON_CBN_SELCHANGE(IDC_COMBO5, &CDummyLEDDlg::OnCbnSelchangeCombo5)
END_MESSAGE_MAP()


// CDummyLEDDlg メッセージ ハンドラ

BOOL CDummyLEDDlg::OnInitDialog()
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

void CDummyLEDDlg::OnPaint()
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
HCURSOR CDummyLEDDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


// これ以降 手動追加 -------------------->>>

//￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣
// ×ボタン
//＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿
void CDummyLEDDlg::OnClose()
{
	//CDialog::OnClose();		// 既存の処理をヤメ
}

//￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣
// 強制終了ボタン
//＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿
void CDummyLEDDlg::OnBnClickedOk()
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
void CDummyLEDDlg::OnLbnDblclkList1()
{
	// ListBoxコントロールのプロパティは、ソート禁止にしておく事
	// ListBoxコントロールのダブルクリックイベントを追加
	mcls_pLog->ShowEdit();
}


//------------------------------------------
// 各プロセス初期化処理
//------------------------------------------
void CDummyLEDDlg::InitProcess()
{
	CString sWk;
	//// 一番初めにいろいろ初期化
	mcls_pLog = NULL;

	//// タスク初期化処理
	//if( 0 != task_init(XX_CPPSMPEX, 0) ) {		// PCIDがない場合。
	//if( 0 != task_init(XX_CPPSMPEX) ) {
	//	syslog(SYSNO_TASK_INIT_ERR, "[%s]", XX_CPPSMPEX);
	//	_ASSERT(0);
	//	exit(-9);		// 自プロセス強制終了
	//	return;
	//}
	int nRetc = task_init(DummyLED);
	if( 0 != nRetc ) {
		if(-1 == nRetc) { syslog(SYSNO_TASK_INIT_ERR, "[%s] 既にタスク起動済みにより、起動中断。", DummyLED); }
		else			{ syslog(SYSNO_TASK_INIT_ERR, "[%s]", DummyLED); }
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
	
	//// 画面更新用データセット

	//-----------------------
	// コンボボックスにセッション名追加
	for(int ii=0; ii<MAX_THREAD; ii++) {
		if( NULL == mcls_MI.gcls_pDmLedMgr[ii] ) break;
		m_cmbSession.AddString( mcls_MI.gcls_pDmLedMgr[ii]->GetMyName() );
	}
	m_cmbSession.SetCurSel(0);				// 0番目を初期選択とする

	for(int ii=0; ii<MAX_LED_CH; ii++) {
		sWk.Format("%d", ii+1);
		m_cmbCh.AddString( sWk );
	}
	m_cmbCh.SetCurSel(0);

	for(int ii=0; ii<MAX_LED_CH; ii++) {
		sWk.Format("%d", ii+1);
		debug_ch.AddString( sWk );
	}
	debug_ch.SetCurSel(0);

	for(int ii=0; ii<MAX_TEMP_UNIT; ii++) {
		sWk.Format("%d", ii+1);
		debug_uni.AddString( sWk );
	}	
	debug_uni.SetCurSel(0);

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
void CDummyLEDDlg::ExitProcess()
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
unsigned WINAPI CDummyLEDDlg::MainThread(void* param)
{
	CDummyLEDDlg* p = (CDummyLEDDlg *)param;
	COMMON_QUE que;			// 受信構造体

	//// 各プロセス初期化処理 (これ以前はログ出力禁止)
	p->InitProcess();

	//// 起動要因取り込み ループ開始
	while(true) {
		recv_mail(&que);			// 起動要因取り込み

		switch(que.nEventNo) {
//*/
//-----------------------------------------------------------------------------------------------
		case FACT_KS_PCNAME_CHANGE:								// PC名称変更通知 (←KS_SLAVE)
			pLOG(em_MSG), "[MAIL] パラメータ変更要求 (FACT_TO_DEFECT_01) [テーブル=%d]", que.fl.data[0]); 
			// メインインスタンスへ通知
			p->mcls_MI.SetDeliveryMail( E_DEF_ML_PARAM, &que);
			break;
//*/
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
void CDummyLEDDlg::OnTimer(UINT_PTR nIDEvent)
{
	CString		sWk;
	int			nWk;
	//// 現在選択されているインスタンスを取得する
	int nIndex = m_cmbSession.GetCurSel();
	if( -1 == nIndex) return;					// iniファイルの設定無し
	DummyLedManager* pCls = mcls_MI.gcls_pDmLedMgr[nIndex];
	if( NULL == pCls ) return;

	int nCh = m_cmbCh.GetCurSel();
	if( -1 == nCh) return;
	//int nUni = debug_uni.GetCurSel();
	//if (-1 == nUni) return;

	SetDlgItemInt(IDC_STATE_VALUE,		pCls->GetNowLedBase(nCh));

	SetDlgItemInt(IDC_STATE_VALUE21,	pCls->GetLedTemp(nCh,0));
	SetDlgItemInt(IDC_STATE_VALUE22,	pCls->GetLedTemp(nCh,1));
	SetDlgItemInt(IDC_STATE_VALUE23,	pCls->GetLedTemp(nCh,2));

	int nbreak = pCls->GetStateBreak(nCh);
	if( 0 == nbreak)SetDlgItemText(IDC_STATE_VALUE3,"○");
	else if( 1 == nbreak)SetDlgItemText(IDC_STATE_VALUE3,"×");
	else			SetDlgItemText(IDC_STATE_VALUE3,"？");

	//int nStTmp = pCls->SetLedTempSts(nCh, nUni);
	int nStTmp = pCls->GetStateTemp(nCh);
	if( 0 == nStTmp)SetDlgItemText(IDC_STATE_VALUE4,"○");
	else if( 1 == nStTmp)SetDlgItemText(IDC_STATE_VALUE4,"△");
	else if( 2 == nStTmp)SetDlgItemText(IDC_STATE_VALUE4,"×");
	else			SetDlgItemText(IDC_STATE_VALUE4,"？");


	int nOnOff = pCls->GetStateOnOff(nCh);
	if( 0 == nOnOff)SetDlgItemText(IDC_STATE_ONOFF,"○");
	else			SetDlgItemText(IDC_STATE_ONOFF, "×");
	//if( 0 == nOnOff)CheckDlgButton(IDC_STATE_ONOFF,true);
	//else			CheckDlgButton(IDC_STATE_ONOFF,false);

	CDialog::OnTimer(nIDEvent);
}



void CDummyLEDDlg::OnBnClickedBtnTemp()
{
	if ( BST_CHECKED != CWnd::IsDlgButtonChecked(IDC_DEBUG) ) return;


	//// 現在選択されているインスタンスを取得する
	int nIndex = m_cmbSession.GetCurSel();
	if( -1 == nIndex) return;					// iniファイルの設定無し
	DummyLedManager* pCls = mcls_MI.gcls_pDmLedMgr[nIndex];
	if( NULL == pCls ) return;

	int nCh = debug_ch.GetCurSel();
	if( -1 == nCh) return;
	int nUni = debug_uni.GetCurSel();
	if( -1 == nUni) return;
	int nTemp = GetDlgItemInt(IDC_STATE_TEMP);
	if( -1 == nTemp) return;
	pCls->SetTemp(nCh,nUni,nTemp);

	pCls->SetLedTempSts(nCh, nUni);
	//pCls->SetStateTemp(nCh);

	LOG(em_WAR), "[DEBUG] 温度変更 CH:%d Unit:%d Temp:%d(Sts:%d)", nCh, nUni, nTemp, pCls->GetLedTempSts(nCh, nUni));
}

void CDummyLEDDlg::OnBnClickedBtnBreak()
{
	if ( BST_CHECKED != CWnd::IsDlgButtonChecked(IDC_DEBUG) ) return;
	int nIndex = m_cmbSession.GetCurSel();
	if( -1 == nIndex) return;					// iniファイルの設定無し
	DummyLedManager* pCls = mcls_MI.gcls_pDmLedMgr[nIndex];
	if( NULL == pCls ) return;
	int nCh = debug_ch.GetCurSel();
	if( -1 == nCh) return;
	pCls->SetBreak(nCh);

	LOG(em_WAR), "[DEBUG] 断線状態変更 CH:%d Break:%d", nCh, pCls->GetStateBreak(nCh));

}


//==========================================
// 点灯/消灯切替
//==========================================
void CDummyLEDDlg::OnBnClickedStateOnoff()
{
	if (BST_CHECKED != CWnd::IsDlgButtonChecked(IDC_DEBUG)) return;
	int nIndex = m_cmbSession.GetCurSel();
	if (-1 == nIndex) return;					// iniファイルの設定無し
	DummyLedManager* pCls = mcls_MI.gcls_pDmLedMgr[nIndex];
	if (NULL == pCls) return;
	int nCh = debug_ch.GetCurSel();
	if (-1 == nCh) return;

	// 点灯/消灯切替
	int nOnOff = 0;		// 点灯/消灯要求に従う
	if (BST_CHECKED == CWnd::IsDlgButtonChecked(IDC_STATE_ONOFF_CHK)) {
		nOnOff = 1;		// 消灯固定
	}
	pCls->SetLedOnOff(nCh, nOnOff);		// 点灯/消灯制御切替

	LOG(em_WAR), "[DEBUG] 点灯/消灯制御 変更 CH:%d %s", nCh, (0 == nOnOff ? "要求に従う" : "消灯固定"));
}


void CDummyLEDDlg::OnCbnSelchangeCombo4()
{
	int nIndex = m_cmbSession.GetCurSel();
	if (-1 == nIndex) return;					// iniファイルの設定無し
	DummyLedManager* pCls = mcls_MI.gcls_pDmLedMgr[nIndex];
	if (NULL == pCls) return;
	int nCh = debug_ch.GetCurSel();
	if (-1 == nCh) return;

	int nOnOff = pCls->GetLedOnOff(nCh);	// 点灯/消灯制御取得


	if( 0 == nOnOff) CheckDlgButton(IDC_STATE_ONOFF_CHK, false);
	else             CheckDlgButton(IDC_STATE_ONOFF_CHK, true);

	LOG(em_WAR), "[DEBUG] CH変更 点灯/消灯制御 CH:%d %s", nCh, (0 == nOnOff ? "要求に従う" : "消灯固定"));
}


void CDummyLEDDlg::OnCbnSelchangeCombo5()
{
	int nIndex = m_cmbSession.GetCurSel();
	if (-1 == nIndex) return;					// iniファイルの設定無し
	DummyLedManager* pCls = mcls_MI.gcls_pDmLedMgr[nIndex];
	if (NULL == pCls) return;
	int nCh = debug_ch.GetCurSel();
	if (-1 == nCh) return;

	int nOnOff = pCls->GetLedOnOff(nCh);	// 点灯/消灯制御取得


	if (0 == nOnOff) CheckDlgButton(IDC_STATE_ONOFF_CHK, false);
	else             CheckDlgButton(IDC_STATE_ONOFF_CHK, true);

	LOG(em_WAR), "[DEBUG] CH変更 点灯/消灯制御 CH:%d %s", nCh, (0 == nOnOff ? "要求に従う" : "消灯固定"));
}


