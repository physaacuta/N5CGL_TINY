
// TI_TESTDlg.cpp : 実装ファイル
//

#include "stdafx.h"
#include "TI_TEST.h"
#include "TI_TESTDlg.h"
#include "afxdialogex.h"

// メモリーリーク検出 手動追加 ----->>>
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif
// <<<------------------------


// CTI_TESTDlg ダイアログ



CTI_TESTDlg::CTI_TESTDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_TI_TEST_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CTI_TESTDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, m_ListBox);
	DDX_Control(pDX, IDC_LIST_ST, m_ListSt);
}

BEGIN_MESSAGE_MAP(CTI_TESTDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDOK, &CTI_TESTDlg::OnBnClickedOk)
	ON_LBN_DBLCLK(IDC_LIST1, &CTI_TESTDlg::OnLbnDblclkList1)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_RAD_INPUT_MAN, &CTI_TESTDlg::OnBnClickedRadInputMan)
	ON_BN_CLICKED(IDC_RAD_INPUT_BMP, &CTI_TESTDlg::OnBnClickedRadInputBmp)
	ON_BN_CLICKED(IDC_RAD_INPUT_AVI, &CTI_TESTDlg::OnBnClickedRadInputAvi)
	ON_BN_CLICKED(IDC_BTN_MAN, &CTI_TESTDlg::OnBnClickedBtnMan)
	ON_BN_CLICKED(IDC_CHK_CYCLE, &CTI_TESTDlg::OnBnClickedChkCycle)
	ON_BN_CLICKED(IDC_DEBUG_MANUAL, &CTI_TESTDlg::OnBnClickedDebugManual)
	ON_BN_CLICKED(IDC_BTN_START, &CTI_TESTDlg::OnBnClickedBtnStart)
	ON_BN_CLICKED(IDC_BTN_STOP, &CTI_TESTDlg::OnBnClickedBtnStop)
	ON_BN_CLICKED(IDC_BTN_FRAME, &CTI_TESTDlg::OnBnClickedBtnFrame)
	ON_WM_DROPFILES()
	ON_BN_CLICKED(IDC_BTN_NEXT_COIL, &CTI_TESTDlg::OnBnClickedBtnNextCoil)
	ON_BN_CLICKED(IDC_CHK_RENZOKU, &CTI_TESTDlg::OnBnClickedChkRenzoku)
END_MESSAGE_MAP()


// CTI_TESTDlg メッセージ ハンドラー

BOOL CTI_TESTDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// このダイアログのアイコンを設定します。アプリケーションのメイン ウィンドウがダイアログでない場合、
	//  Framework は、この設定を自動的に行います。
	SetIcon(m_hIcon, TRUE);			// 大きいアイコンの設定
	SetIcon(m_hIcon, FALSE);		// 小さいアイコンの設定

	// TODO: 初期化をここに追加します。
	// 画面初期化セット----------------------------------------->>>
	CWnd::CheckRadioButton(IDC_RAD_INPUT_MAN, IDC_RAD_INPUT_AVI, IDC_RAD_INPUT_MAN);			// モード選択
	CWnd::GetDlgItem(IDC_CHK_CYCLE)->EnableWindow(false);										// 周期実行
	CWnd::CheckRadioButton(IDC_RAD_O, IDC_RAD_U, IDC_RAD_O);									// 検査面
	CWnd::CheckDlgButton(IDC_DEBUG_MANUAL, 1);

// 手動追加 -------------------->>>
// 画面位置 (サイズはいまのまま)
	::SetWindowPos(m_hWnd, NULL, 400, 600, 0, 0, SWP_NOSIZE | SWP_NOZORDER);

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

void CTI_TESTDlg::OnPaint()
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
		CDialogEx::OnPaint();
	}
}

// ユーザーが最小化したウィンドウをドラッグしているときに表示するカーソルを取得するために、
//  システムがこの関数を呼び出します。
HCURSOR CTI_TESTDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



// これ以降 手動追加 -------------------->>>

//￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣
// ×ボタン
//＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿
void CTI_TESTDlg::OnClose()
{
	//CDialog::OnClose();
}

//￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣
// 強制終了ボタン
//＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿
void CTI_TESTDlg::OnBnClickedOk()
{
	if (IDYES == MessageBox("終了しますか？", "重要なお知らせ", MB_YESNO | MB_DEFBUTTON2 | MB_ICONQUESTION)) {
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
void CTI_TESTDlg::OnLbnDblclkList1()
{
	// ListBoxコントロールのプロパティは、ソート禁止にしておく事
	// ListBoxコントロールのダブルクリックイベントを追加
	mcls_pLog->ShowEdit();
}

//------------------------------------------
// 各プロセス初期化処理
//------------------------------------------
void CTI_TESTDlg::InitProcess()
{
	//// タスク初期化処理
	int nCamStart = GetPrivateProfileInt(TI_TEST, "CAM_START", 1, TASKINI_NAME);
	CString sTaskName;
	sTaskName.Format("%s%02d", TI_TEST, nCamStart);
	if (0 != task_init(sTaskName)) {
	//if (0 != task_init(TI_TEST)) {
			_ASSERT(0);
		exit(-9);		// 自プロセス強制終了
		return;
	}

	//// LogFileManagerインスタンス生成 (これ以降 ログ出力可能)
	setlocale(LC_ALL, "Japanese");	// これが無いと日本語パスを含むフォルダの場合、保存できない。(ライブラリの中で実行するべきか迷っているが、変更するとタスク全体に影響するので、とりあえずここで設定)
	mcls_pLog = new LogFileManager(getmytaskname(), &m_ListBox, 50, 5);
	LOG(em_MSG), "[Dlg] タスク起動 [%s]", sTaskName);
	LOG(em_MSG), "[Dlg] 各プロセス開始処理 実行中・・・");

	mcls_pLogCull = new LogFileManager("CULL", NULL);
	LOGCULL(em_MSG), "[Dlg] タスク起動");

	//-----------------------
	// ステータス状態 リストビューインスタンス生成 & 定義
	if (true) {
		mcnt_pListSt = new DspListView(&m_ListSt);
		// 見た目定義
		m_ListSt.SetExtendedStyle(m_ListSt.GetExtendedStyle() | LVS_EX_GRIDLINES);			// 罫線描画
		m_ListSt.SetExtendedStyle(m_ListSt.GetExtendedStyle() | LVS_EX_FULLROWSELECT);		// 1行全体選択

		// 列追加			0		1			
		char name[][32] = { "状態",	"1", "2", "3", "4", "5", "6", "7", "8", "9", "10" };
		int  wid[] = { 140, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40 };
		mcnt_pListSt->SetColumnAll(sizeof(wid) / sizeof(int), 32, wid, (char const*)name);
		// 行追加
		mcnt_pListSt->SetRowAll(6);
		// 固定値セット
		//mcnt_pListSt->SetItem(1, 0, "操業状態検");
		mcnt_pListSt->SetItem(1, 0, "FrameNo");
		mcnt_pListSt->SetItem(2, 0, "QUE  Free");
		mcnt_pListSt->SetItem(3, 0, "QUE  Max");
		mcnt_pListSt->SetItem(4, 0, "送信 Ave");
		mcnt_pListSt->SetItem(5, 0, "送信 Max");
	}

	// 試験用初期値
	if (true) {
		CWnd::SetDlgItemTextA(IDC_ED_INPUT_BMP, "data\\test_Set%d_Ang%d_No%d.bmp");
		CWnd::SetDlgItemTextA(IDC_CYCLE_BMP, "60");
		CWnd::SetDlgItemTextA(IDC_ST_BMP, "0");
		CWnd::SetDlgItemTextA(IDC_ED_BMP, "3");
		CWnd::CheckRadioButton(IDC_RAD_INPUT_MAN, IDC_RAD_INPUT_AVI, IDC_RAD_INPUT_BMP);			// モード選択
		CWnd::GetDlgItem(IDC_CHK_CYCLE)->EnableWindow(true);										// 周期実行

		// 連続稼働ON
		CWnd::CheckDlgButton(IDC_CHK_RENZOKU, 1);
	}

	//// メインインスタンス 開始処理
	mcls_MI.SetLogMgr(mcls_pLog);
	mcls_MI.SetLogMgrCull(mcls_pLogCull);
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
void CTI_TESTDlg::ExitProcess()
{
	//// メインインスタンス 終了処理
	KillTimer(1);			// 画面更新タイマ
	mcls_MI.Stop(20000);
	mcls_MI.Exit();

	//// ログ リストボックス表示処理終了
	mcls_pLog->SetListOut(false);					// これ以降 リストボックスへのログ表示禁止

													//// ログクラス開放
	LOG(em_MSG), "[Dlg] タスク終了");
	if (NULL != mcls_pLog) {
		delete mcls_pLog;
		mcls_pLog = NULL;
	}
	LOGCULL(em_MSG), "[Dlg] タスク終了");
	if (NULL != mcls_pLogCull) {
		delete mcls_pLogCull;
		mcls_pLogCull = NULL;
	}

	//// 画面コントロール開放
	delete mcnt_pListSt;

	// タスク終了処理
	task_exit();

	// 画面の終了要求
	PostMessage(WM_QUIT);
}

//------------------------------------------
// メインスレッド DlgThread
// LPVOID param ダイヤログポインタ
//------------------------------------------
unsigned WINAPI CTI_TESTDlg::MainThread(void* param)
{
	CTI_TESTDlg* p = (CTI_TESTDlg *)param;
	COMMON_QUE que;			// 受信構造体

							//// 各プロセス初期化処理 (これ以前はログ出力禁止)
	p->InitProcess();

	//// 起動要因取り込み ループ開始
	while (true) {
		recv_mail(&que);			// 起動要因取り込み

		switch (que.nEventNo) {

			//-----------------------------------------------------------------------------------------------
		case FACT_TI_TEST_01:								// 試験用 検査開始通知 (←TI_TEST)
			pLOG(em_MSG), "[MAIL] 試験用 周期開始通知 (%d) [%d]", que.nEventNo, que.fl.data[0] );

			if (0 == que.fl.data[0]) {
				p->CWnd::CheckDlgButton(IDC_CHK_CYCLE, BST_UNCHECKED);			// 周期ボタンOFF

				// 周期送信停止
				TestManager::DELIVERY_KEY* pQue = new TestManager::DELIVERY_KEY;
				memset(pQue, 0x00, sizeof(TestManager::DELIVERY_KEY));
				pQue->nMode = p->mcls_MI.gcls_pTest->GRAB_MODE_BMP;			// 選択モード
				pQue->nCycle = 0;											// 周期(0:停止)
				p->mcls_MI.gcls_pTest->gque_Deli.AddToTail(pQue);
			}
			else {
				if (BST_UNCHECKED == p->CWnd::IsDlgButtonChecked(IDC_CHK_CYCLE)) {
					p->CWnd::CheckDlgButton(IDC_CHK_CYCLE, BST_CHECKED);	// 周期ボタンON
					p->OnBnClickedChkCycle();								// 周期送信開始
				}
			}
			break;

			//-----------------------------------------------------------------------------------------------
		case FACT_TI_TEST_02:								// 試験用 フレームNoリセット通知 (←TI_TEST)
			pLOG(em_MSG), "[MAIL] 試験用 フレームNoリセット通知 (%d) [%d][%d]", que.nEventNo, que.fl.data[0], que.fl.data[1] );

			p->mcls_MI.gcls_pTest->SetFrameNo(que.fl.data[0]);
			p->CWnd::SetDlgItemInt(IDC_CYCLE_BMP, que.fl.data[1]);
			break;

			//-----------------------------------------------------------------------------------------------
		case FACT_KS_PCNAME_CHANGE:							// PC名称変更通知 (←KS_SLAVE)
			pLOG(em_MSG), "[MAIL] PC名称変更通知 (FACT_KS_PCNAME_CHANGE)" );
			// PC名称取得
			getpcname();
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
void CTI_TESTDlg::OnTimer(UINT_PTR nIDEvent)
{
	CString cwk;
	CString ss;


	//==================================================
	// ステータス状態
#ifdef FRAME_RAW_SEND
	mcnt_pListSt->SetItem(0, 0, (0==mcls_MI.GetSysMode() ? "停止" : "開始"));
	for (int ii = 0; ii < NUM_CAM_POS; ii++) {
		//	for (int ii = 0; ii<FRAME_HT_SEND_PORT; ii++) {
		mcnt_pListSt->SetItem(0, 1 + ii, (mcls_MI.gcls_pSockRpi[ii]->IsConnect() ? "○" : "×"));
		mcnt_pListSt->SetItem(1, 1 + ii, mcls_MI.gcls_pCull[ii]->GetFrameNo());
		mcnt_pListSt->SetItem(2, 1 + ii, mcls_MI.gque_pSockRpi[ii]->GetCountFree());
		mcnt_pListSt->SetItem(3, 1 + ii, mcls_MI.gque_pSockRpi[ii]->GetMaxCountPool());
		mcnt_pListSt->SetItem(4, 1 + ii, mcls_MI.gcls_pCull[ii]->g_nTime_Rpi[0]);
		mcnt_pListSt->SetItem(5, 1 + ii, mcls_MI.gcls_pCull[ii]->g_nTime_Rpi[1]);
	}
#endif

//#ifdef FRAME_RAW_SEND
//	for (int ii = 0; ii<FRAME_HT_SEND_PORT; ii++) {
//		mcnt_pListTo->SetItem(11 + 2 * ii, 1, mcls_MI.gcls_pSockRpi[ii]->GetSockStateName());
//		ss.Format("%d (%d)", mcls_MI.gque_pSockRpi[ii]->GetCountFree(), mcls_MI.gque_pSockRpi[ii]->GetMaxCountPool());	mcnt_pListTo->SetItem(12 + ii * 2, 1, ss);
//	}
//
//	double dAve = (mcls_MI.gcls_pCull[0]->g_nTime_Rpi[0] + mcls_MI.gcls_pCull[1]->g_nTime_Rpi[0]) / 2;
//	double dMax = max(mcls_MI.gcls_pCull[0]->g_nTime_Rpi[1], mcls_MI.gcls_pCull[1]->g_nTime_Rpi[1]);
//	ss.Format("%.0f (%.0f)", dAve, dMax);	mcnt_pListTo->SetItem(15, 1, ss);
//#endif

	int	nFrameNo = mcls_MI.gcls_pTest->GetFrameNo();
	CWnd::SetDlgItemInt(IDC_EDIT_FRAME_NEXT, nFrameNo);

	int nChgFrame = mcls_MI.gcls_pTest->GetChangeFrm();
	int nRenFrame = 0;
	if (0 < nChgFrame) { nRenFrame = ((nFrameNo / nChgFrame) + 1) * nChgFrame; }
	CWnd::SetDlgItemInt(IDC_ED_REN_NEXT, nRenFrame);


	CDialog::OnTimer(nIDEvent);
}


//==========================================
// デバッグ実行ボタン押下
//==========================================
void CTI_TESTDlg::OnBnClickedDebugManual()
{
	//if (BST_CHECKED != CWnd::IsDlgButtonChecked(IDC_DEBUG_MANUAL)) return;		// デバッグ実行
	if (BST_CHECKED == CWnd::IsDlgButtonChecked(IDC_CHK_CYCLE)) {
		CWnd::CheckDlgButton(IDC_CHK_CYCLE, BST_UNCHECKED);
	}
}

//==========================================
// 手動選択ボタン押下
//==========================================
void CTI_TESTDlg::OnBnClickedRadInputMan()
{
	// TODO: ここにコントロール通知ハンドラー コードを追加します。
	CWnd::GetDlgItem(IDC_BTN_MAN)->EnableWindow(true);				// 手動画像取込
	CWnd::GetDlgItem(IDC_CHK_CYCLE)->EnableWindow(false);			// 周期実行
}


//==========================================
// BMP選択ボタン押下
//==========================================
void CTI_TESTDlg::OnBnClickedRadInputBmp()
{
	//if (BST_CHECKED != CWnd::IsDlgButtonChecked(IDC_DEBUG_MANUAL)) return;		// デバッグ実行
	if (BST_CHECKED == CWnd::IsDlgButtonChecked(IDC_CHK_CYCLE)) {
		CWnd::CheckDlgButton(IDC_CHK_CYCLE, BST_UNCHECKED);
	}
	CWnd::GetDlgItem(IDC_BTN_MAN)->EnableWindow(false);				// 手動画像取込
	CWnd::GetDlgItem(IDC_CHK_CYCLE)->EnableWindow(true);			// 周期実行
}


//==========================================
// AVI選択ボタン押下
//==========================================
void CTI_TESTDlg::OnBnClickedRadInputAvi()
{
	//if (BST_CHECKED != CWnd::IsDlgButtonChecked(IDC_DEBUG_MANUAL)) return;		// デバッグ実行
	if (BST_CHECKED == CWnd::IsDlgButtonChecked(IDC_CHK_CYCLE)) {
		CWnd::CheckDlgButton(IDC_CHK_CYCLE, BST_UNCHECKED);
	}
	CWnd::GetDlgItem(IDC_BTN_MAN)->EnableWindow(false);				// 手動画像取込
	CWnd::GetDlgItem(IDC_CHK_CYCLE)->EnableWindow(true);			// 周期実行
}


//==========================================
// 手動画像取込ボタン押下
//==========================================
void CTI_TESTDlg::OnBnClickedBtnMan()
{
	if (BST_CHECKED != CWnd::IsDlgButtonChecked(IDC_DEBUG_MANUAL)) return;		// デバッグ実行
	if (BST_CHECKED != CWnd::IsDlgButtonChecked(IDC_RAD_INPUT_MAN)) return;		// 手動設定

	CString	sPath;
	CWnd::GetDlgItemTextA(IDC_ED_INPUT_MAN, sPath);

	TestManager::DELIVERY_KEY* pQue = new TestManager::DELIVERY_KEY;
	memset(pQue, 0x00, sizeof(TestManager::DELIVERY_KEY));
	pQue->nMode = mcls_MI.gcls_pTest->GRAB_MODE_MAN;			// 選択モード
	memcpy(pQue->cPath, sPath, sizeof(pQue->cPath));			// ファイルパス
	mcls_MI.gcls_pTest->gque_Deli.AddToTail(pQue);
}


//==========================================
// 周期実行ボタン押下
//==========================================
void CTI_TESTDlg::OnBnClickedChkCycle()
{
	if (BST_CHECKED != CWnd::IsDlgButtonChecked(IDC_DEBUG_MANUAL)) return;		// デバッグ実行

	int		nMode;
	CString	sPath;
	CString sCycle;
	CString sStart;
	CString sEnd;
	int		nCycle	= 0;

	if (BST_CHECKED == CWnd::IsDlgButtonChecked(IDC_RAD_INPUT_BMP)) {
		// BMP
		CWnd::GetDlgItemTextA(IDC_ED_INPUT_BMP, sPath);
		CWnd::GetDlgItemTextA(IDC_CYCLE_BMP, sCycle);
		CWnd::GetDlgItemTextA(IDC_ST_BMP, sStart);
		CWnd::GetDlgItemTextA(IDC_ED_BMP, sEnd);
		nMode = mcls_MI.gcls_pTest->GRAB_MODE_BMP;
	}
	else if (BST_CHECKED == CWnd::IsDlgButtonChecked(IDC_RAD_INPUT_AVI)) {
		// AVI
		CWnd::GetDlgItemTextA(IDC_ED_INPUT_AVI, sPath);
		CWnd::GetDlgItemTextA(IDC_CYCLE_AVI, sCycle);
		sStart = "";
		sEnd = "";
		nMode = mcls_MI.gcls_pTest->GRAB_MODE_AVI;
	}
	else {
		return;
	}

	TestManager::DELIVERY_KEY* pQue = new TestManager::DELIVERY_KEY;
	memset(pQue, 0x00, sizeof(TestManager::DELIVERY_KEY));
	pQue->nMode = nMode;										// 選択モード
	memcpy(pQue->cPath, sPath, sizeof(pQue->cPath));			// ファイルパス
	if (BST_CHECKED == CWnd::IsDlgButtonChecked(IDC_CHK_CYCLE)) {
		pQue->nCycle = atoi(sCycle);							// 周期
	}
	else {
		pQue->nCycle = 0;										// 周期(0:停止)
	}
	pQue->nStart = atoi(sStart);								// 開始No
	pQue->nEnd = atoi(sEnd);									// 終了No

	nCycle = pQue->nCycle;										// 周期保持
	mcls_MI.gcls_pTest->gque_Deli.AddToTail(pQue);

	mcls_MI.Send_TiTest_Rendo(nCycle);							// ツール間連動
}



//==========================================
// 開始ボタン押下
//==========================================
void CTI_TESTDlg::OnBnClickedBtnStart()
{
	if (BST_CHECKED != CWnd::IsDlgButtonChecked(IDC_DEBUG_MANUAL)) return;		// デバッグ実行

	if (BST_CHECKED == CWnd::IsDlgButtonChecked(IDC_RAD_O)) mcls_MI.SetMen(0);
	if (BST_CHECKED == CWnd::IsDlgButtonChecked(IDC_RAD_U)) mcls_MI.SetMen(1);

	// 開始イベント
	mcls_MI.SetEvKenStart();

}

//==========================================
// 終了ボタン押下
//==========================================
void CTI_TESTDlg::OnBnClickedBtnStop()
{
	if (BST_CHECKED != CWnd::IsDlgButtonChecked(IDC_DEBUG_MANUAL)) return;		// デバッグ実行

	if (BST_CHECKED == CWnd::IsDlgButtonChecked(IDC_CHK_CYCLE)) {				// 周期送信停止
		CWnd::CheckDlgButton(IDC_CHK_CYCLE, BST_UNCHECKED);
		OnBnClickedChkCycle();
	}
	// 終了イベント
	mcls_MI.SetEvKenStop();


}




//==========================================
// フレームNoボタン押下
//==========================================
void CTI_TESTDlg::OnBnClickedBtnFrame()
{
	CString	sWk;
	int	nFrameNo;
	CWnd::GetDlgItemTextA(IDC_EDIT_FRAME, sWk);
	nFrameNo = atoi(sWk);
	mcls_MI.gcls_pTest->SetFrameNo(nFrameNo);

	CWnd::GetDlgItemTextA(IDC_CYCLE_BMP, sWk);
	int nCycleTime = atoi(sWk);
	mcls_MI.Send_TiTest_FrameNo(nFrameNo, nCycleTime);		// ツール間連動
}


void CTI_TESTDlg::OnDropFiles(HDROP hDropInfo)
{
	// TODO: ここにメッセージ ハンドラー コードを追加するか、既定の処理を呼び出します。

	CDialogEx::OnDropFiles(hDropInfo);
}


//==========================================
// 次コイル情報事前通知ボタン押下
//==========================================
void CTI_TESTDlg::OnBnClickedBtnNextCoil()
{
	if (BST_CHECKED != CWnd::IsDlgButtonChecked(IDC_DEBUG_MANUAL)) return;		// デバッグ実行


	COMMON_QUE que;
	que.nEventNo = FACT_TI_RPISAVE_04;		// 次コイル情報事前通知 
	que.nLineNo = LINE_ID;
	que.mix.idata[0] = -1;					// 対象インスタンス
	if (BST_CHECKED == CWnd::IsDlgButtonChecked(IDC_RAD_O)) que.mix.idata[0] = 0;
	if (BST_CHECKED == CWnd::IsDlgButtonChecked(IDC_RAD_U)) que.mix.idata[0] = 1;
	que.mix.idata[1] = 1;					// 検査パターン(表)
	que.mix.idata[2] = 1;					// 検査パターン(裏)
	que.mix.idata[3] = 1;					// 検査表面状態(表)
	que.mix.idata[4] = 1;					// 検査表面状態(裏)
	que.mix.idata[5] = CWnd::GetDlgItemInt(IDC_ED_FRAME);		// 実行フレームNo
	que.mix.idata[6] = CWnd::GetDlgItemInt(IDC_ED_TOP_POS);		// 先頭長手位置[mm]

	CString	sWk;
	CWnd::GetDlgItemTextA(IDC_ED_KANRI, sWk);
	memcpy(que.mix.cdata[0], sWk, sizeof(que.mix.cdata[0]));
	CWnd::GetDlgItemTextA(IDC_ED_COILNO, sWk);
	memcpy(que.mix.cdata[1], sWk, sizeof(que.mix.cdata[1]));

	send_mail(TI_RPISAVE, "*", &que);
}


//==========================================
// 連続稼動ボタン押下
//==========================================
void CTI_TESTDlg::OnBnClickedChkRenzoku()
{
	if (BST_CHECKED == CWnd::IsDlgButtonChecked(IDC_CHK_RENZOKU)) {
		// 連続稼動 ON
		CString	sFrame;
		int	nFrameNo;
		CWnd::GetDlgItemTextA(IDC_ED_REN_FRM, sFrame);
		nFrameNo = atoi(sFrame);
		mcls_MI.gcls_pTest->SetChangeFrm(nFrameNo);
	}
	else {
		// 連続稼動 OFF
		mcls_MI.gcls_pTest->SetChangeFrm(0);
	}
}
