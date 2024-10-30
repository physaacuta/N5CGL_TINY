
// HY_TESTDlg.cpp : 実装ファイル
//

#include "stdafx.h"
#include "HY_TEST.h"
#include "HY_TESTDlg.h"
#include "afxdialogex.h"

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

// テスト用
#define		HY_TEST		"HY_TEST"

// CHY_TESTDlg ダイアログ



CHY_TESTDlg::CHY_TESTDlg(CWnd* pParent /*=NULL*/)
	: CDialog(IDD_HY_TEST_DIALOG, pParent)
	, m_rdoKadou(0)
	, m_rdoUnten(0)
	, m_rdoKiki_o(0)
	, m_rdoKensa_o(0)
	, m_rdoKiki_u(0)
	, m_rdoKensa_u(0)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CHY_TESTDlg::DoDataExchange(CDataExchange* pDX)
{
	// 初期値設定
	m_rdoKadou = 2;
	m_rdoUnten = 1;
	m_rdoKiki_o = 0;
	m_rdoKensa_o = 0;
	m_rdoKiki_u = 0;
	m_rdoKensa_u = 0;

	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, m_ListBox);
	DDX_Radio(pDX, IDC_RADIO1, m_rdoKadou);
	DDV_MinMaxInt(pDX, m_rdoKadou, 0, 3);
	DDX_Radio(pDX, IDC_RADIO5, m_rdoUnten);
	DDV_MinMaxInt(pDX, m_rdoUnten, 0, 4);
	DDX_Radio(pDX, IDC_RADIO10, m_rdoKiki_o);
	DDV_MinMaxInt(pDX, m_rdoKiki_o, 0, 3);
	DDX_Radio(pDX, IDC_RADIO14, m_rdoKensa_o);
	DDV_MinMaxInt(pDX, m_rdoKensa_o, 0, 7);
	DDX_Radio(pDX, IDC_RADIO22, m_rdoKiki_u);
	DDV_MinMaxInt(pDX, m_rdoKiki_u, 0, 3);
	DDX_Radio(pDX, IDC_RADIO26, m_rdoKensa_u);
	DDV_MinMaxInt(pDX, m_rdoKensa_u, 0, 7);
	DDX_Control(pDX, IDC_LIST_COIL, m_ListCoil);
	DDX_Control(pDX, IDC_LIST_DSP, m_ListDsp);
	DDX_Control(pDX, IDC_CHECK_DSWS, m_chkDsWs);
}

BEGIN_MESSAGE_MAP(CHY_TESTDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDOK, &CHY_TESTDlg::OnBnClickedOk)
	ON_WM_CLOSE()
	ON_LBN_DBLCLK(IDC_LIST1, &CHY_TESTDlg::OnLbnDblclkList1)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_BTN_STATUS, &CHY_TESTDlg::OnBnClickedBtnStatus)
	ON_BN_CLICKED(IDC_BTN_INIT, &CHY_TESTDlg::OnBnClickedBtnInit)
	ON_BN_CLICKED(IDC_BTN_PARAM, &CHY_TESTDlg::OnBnClickedBtnParam)
	ON_BN_CLICKED(IDC_BTN_ADDCOIL, &CHY_TESTDlg::OnBnClickedBtnAddcoil)
	ON_BN_CLICKED(IDC_BTN_CUT, &CHY_TESTDlg::OnBnClickedBtnCut)
	ON_BN_CLICKED(IDC_BTN_SPEED, &CHY_TESTDlg::OnBnClickedBtnSpeed)
	ON_BN_CLICKED(IDC_CHK_PLG, &CHY_TESTDlg::OnBnClickedChkPlg)
	ON_BN_CLICKED(IDC_BTN_SLOW, &CHY_TESTDlg::OnBnClickedBtnSlow)
	ON_BN_CLICKED(IDC_BTN_COIL_LEN, &CHY_TESTDlg::OnBnClickedBtnCoilLen)
	ON_BN_CLICKED(IDC_BTN_COIL_WID, &CHY_TESTDlg::OnBnClickedBtnCoilWid)
	ON_BN_CLICKED(IDC_CHK_DEF, &CHY_TESTDlg::OnBnClickedChkDef)
	ON_BN_CLICKED(IDC_BTN_DEF_RATE, &CHY_TESTDlg::OnBnClickedBtnDefRate)
	ON_BN_CLICKED(IDC_BTN_SETCODE, &CHY_TESTDlg::OnBnClickedBtnSetcode)
	ON_BN_CLICKED(IDC_BTN_DEF_SEND, &CHY_TESTDlg::OnBnClickedBtnDefSend)
	ON_BN_CLICKED(IDC_CHK_POS, &CHY_TESTDlg::OnBnClickedChkPos)
	ON_BN_CLICKED(IDC_CHK_MAE, &CHY_TESTDlg::OnBnClickedCheckMae)
	ON_BN_CLICKED(IDC_CHK_YOUSETUCUT, &CHY_TESTDlg::OnBnClickedCheckYousetuCut)
	ON_BN_CLICKED(IDC_BTN_DEFLST_SEND, &CHY_TESTDlg::OnBnClickedBtnLstCancelSend)
	ON_BN_CLICKED(IDC_BTN_ALARM_SEND, &CHY_TESTDlg::OnBnClickedBtnAlarmSend)
	ON_BN_CLICKED(IDC_BTN_DEFCNT_SEND, &CHY_TESTDlg::OnBnClickedBtnDefcntSend)
END_MESSAGE_MAP()


// CHY_TESTDlg メッセージ ハンドラー

BOOL CHY_TESTDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// このダイアログのアイコンを設定します。アプリケーションのメイン ウィンドウがダイアログでない場合、
	//  Framework は、この設定を自動的に行います。
	SetIcon(m_hIcon, TRUE);			// 大きいアイコンの設定
	SetIcon(m_hIcon, FALSE);		// 小さいアイコンの設定

	// TODO: 初期化をここに追加します。

	CheckDlgButton(IDC_CHK_POS, 1);
	CheckDlgButton(IDC_CHK_DEF, 0);

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

void CHY_TESTDlg::OnPaint()
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
HCURSOR CHY_TESTDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

// これ以降 手動追加 -------------------->>>

//￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣
// ×ボタン
//＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿
void CHY_TESTDlg::OnClose()
{
	//CDialog::OnClose();		// 既存の処理をヤメ
}

//￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣
// 強制終了ボタン
//＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿
void CHY_TESTDlg::OnBnClickedOk()
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
void CHY_TESTDlg::OnLbnDblclkList1()
{
	// ListBoxコントロールのプロパティは、ソート禁止にしておく事
	// ListBoxコントロールのダブルクリックイベントを追加
	mcls_pLog->ShowEdit();
}

//------------------------------------------
// 各プロセス初期化処理
//------------------------------------------
void CHY_TESTDlg::InitProcess()
{
	//// 一番初めにいろいろ初期化
	mcls_pLog = NULL;

	int nRetc = task_init(HY_TEST);
	if (0 != nRetc) {
		if (-1 == nRetc) { syslog(SYSNO_TASK_INIT_ERR, "[%s] 既にタスク起動済みにより、起動中断。", HY_TEST); }
		else { syslog(SYSNO_TASK_INIT_ERR, "[%s]", HY_TEST); }
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

	//-----------------------
	// コイル情報 リストビューインスタンス生成 & 定義
	if (true) {
		mcnt_pListCoil = new DspListView(&m_ListCoil);
		// 見た目定義
		m_ListCoil.SetExtendedStyle(m_ListCoil.GetExtendedStyle() | LVS_EX_GRIDLINES);			// 罫線描画
		m_ListCoil.SetExtendedStyle(m_ListCoil.GetExtendedStyle() | LVS_EX_FULLROWSELECT);		// 1行全体選択

																							// 列追加			0		1			
		char name[LIST_COIL_COLMAX][32];
		int  wid[LIST_COIL_COLMAX];
		
		memset(name, 0, sizeof(name));
		memset(wid, 0, sizeof(wid));

		strcpy(name[EM_COIL_LEN], "検査位置");		wid[EM_COIL_LEN] = 100;
		strcpy(name[EM_COIL_KANRINO], "管理No");	wid[EM_COIL_KANRINO] = 140;
		strcpy(name[EM_COIL_COILNO], "コイルNo");	wid[EM_COIL_COILNO] = 100;
		strcpy(name[EM_COIL_SPM_UMU], "SPM");		wid[EM_COIL_SPM_UMU] = 40;

		mcnt_pListCoil->SetColumnAll(LIST_COIL_COLMAX, 32, wid, (char const*)name);
		// 行追加
		mcnt_pListCoil->SetRowAll(MAX_COIL_NUM);
	}

	//-----------------------
	// 検査台情報 リストビューインスタンス生成 & 定義
	if (true) {
		mcnt_pListDsp = new DspListView(&m_ListDsp);
		// 見た目定義
		m_ListDsp.SetExtendedStyle(m_ListDsp.GetExtendedStyle() | LVS_EX_GRIDLINES);			// 罫線描画
		m_ListDsp.SetExtendedStyle(m_ListDsp.GetExtendedStyle() | LVS_EX_FULLROWSELECT);		// 1行全体選択

																								// 列追加			0		1			
		char name[LIST_DSP_COLMAX][32];
		int  wid[LIST_DSP_COLMAX];

		memset(name, 0, sizeof(name));
		memset(wid, 0, sizeof(wid));

		strcpy(name[EM_DSP_NO], "");				wid[EM_DSP_NO] = 20;
		strcpy(name[EM_DSP_NAME], "検査台名");		wid[EM_DSP_NAME] = 100;
		strcpy(name[EM_DSP_KANRINO], "管理No");		wid[EM_DSP_KANRINO] = 140;
		strcpy(name[EM_DSP_DISTANCE], "通板位置");	wid[EM_DSP_DISTANCE] = 100;

		mcnt_pListDsp->SetColumnAll(LIST_DSP_COLMAX, 32, wid, (char const*)name);
		// 行追加
		mcnt_pListDsp->SetRowAll(DIV_LEN_POS_FINAL);
		for (int ii = 0; ii<DIV_LEN_POS_FINAL; ii++) mcnt_pListDsp->SetItem(ii, EM_DSP_NO, ii + 1);
	}


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
void CHY_TESTDlg::ExitProcess()
{
	//// メインインスタンス 終了処理
	KillTimer(1);			// 画面更新タイマ
	mcls_MI.Stop();
	mcls_MI.Exit();

	//// ログ リストボックス表示処理終了
	mcls_pLog->SetListOut(false);					// これ以降 リストボックスへのログ表示禁止

													//// ログクラス開放
	LOG(em_MSG), "[Dlg] タスク終了");
	if (NULL != mcls_pLog) {
		delete mcls_pLog;
		mcls_pLog = NULL;
	}

	// タスク終了処理
	task_exit();

	if (mcnt_pListDsp) delete(mcnt_pListDsp);
	if (mcnt_pListCoil) delete(mcnt_pListCoil);

	// 画面の終了要求
	PostMessage(WM_QUIT);
}

//------------------------------------------
// メインスレッド DlgThread
// LPVOID param ダイヤログポインタ
//------------------------------------------
unsigned WINAPI CHY_TESTDlg::MainThread(LPVOID param)
{
	CHY_TESTDlg* p = (CHY_TESTDlg *)param;
	COMMON_QUE que;			// 受信構造体

							//// 各プロセス初期化処理 (これ以前はログ出力禁止)
	p->InitProcess();

	//// 起動要因取り込み ループ開始
	while (true) {
		recv_mail(&que);			// 起動要因取り込み

		switch (que.nEventNo) {
			//*/
			//-----------------------------------------------------------------------------------------------
		case FACT_KS_PCNAME_CHANGE:								// PC名称変更通知 (←KS_SLAVE)
			pLOG(em_MSG), "[MAIL] パラメータ変更要求 (FACT_TO_DEFECT_01) [テーブル=%d]", que.fl.data[0]);
			// メインインスタンスへ通知
			p->mcls_MI.SetDeliveryMail(E_DEF_ML_PARAM, &que);
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
void CHY_TESTDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: ここにメッセージ ハンドラ コードを追加するか、既定の処理を呼び出します。


	PRO_TYPE_DATA_COILINF const* pCoilInf = mcls_MI.gcls_pTest->GetCoilInf();
	TestManager::TEST_COIL_INF const* pTestCoilInf = mcls_MI.gcls_pTest->GetTestCoilInf();
	TestManager::DSP_INF const* pDspInf = mcls_MI.gcls_pTest->GetDspInf();

	// コイル情報
	for (int ii = 0; ii < MAX_COIL_NUM; ii++) {
		CString wk;
		wk.Format("%5d/%5d m", int(pTestCoilInf[ii].nLen / 1000), int(pTestCoilInf[ii].nCoilLen / 1000));
		mcnt_pListCoil->SetItem(ii, EM_COIL_LEN, wk);								// 検査位置
		mcnt_pListCoil->SetItem(ii, EM_COIL_KANRINO, pTestCoilInf[ii].cKizukenNo);	// 管理No.
		mcnt_pListCoil->SetItem(ii, EM_COIL_COILNO, pTestCoilInf[ii].cCoilNo);		// コイルNo.
		if (strlen(pTestCoilInf[ii].cKizukenNo)) {
			wk = GetName_SpmUmu((EM_DIV_SPM)pTestCoilInf[ii].nSpmUmu);
		} else {
			// 管理No.が無いときは空白
			wk = "";
		}
		mcnt_pListCoil->SetItem(ii, EM_COIL_SPM_UMU, wk);							// SPM圧下有無
	}

	// 検査台情報
	for (int ii = 0; ii < DIV_LEN_POS_FINAL; ii++) {
		CString wk;
		wk.Format("%8d mm", pDspInf[ii].nDistance);
		mcnt_pListDsp->SetItem(ii, EM_DSP_NAME, pDspInf[ii].Name);			// 検査台名
		mcnt_pListDsp->SetItem(ii, EM_DSP_DISTANCE, wk);					// 通販距離
		mcnt_pListDsp->SetItem(ii, EM_DSP_KANRINO, pDspInf[ii].cKizukenNo);	// 管理No.
	}

	// ライン速度表示
	{
		CString wk;

		wk.Format("%03d(mpm)", mcls_MI.gcls_pTest->GetSpeed());
		SetDlgItemText(IDC_SPEED, wk);
	}

	// コイル長範囲表示
	{
		CString wk;

		wk.Format("%02d(m)", mcls_MI.gcls_pTest->GetCoilLenMin() / 1000);
		SetDlgItemText(IDC_COIL_LEN_MIN, wk);

		wk.Format("%02d(m)", mcls_MI.gcls_pTest->GetCoilLenMax() / 1000);
		SetDlgItemText(IDC_COIL_LEN_MAX, wk);

	}

	// コイル幅範囲表示
	{
		CString wk;

		wk.Format("%02d(mm)", mcls_MI.gcls_pTest->GetCoilWidthMin());
		SetDlgItemText(IDC_COIL_WID_MIN, wk);

		wk.Format("%02d(mm)", mcls_MI.gcls_pTest->GetCoilWidthMax());
		SetDlgItemText(IDC_COIL_WID_MAX, wk);
	}

	// 疵発生数表示
	{
		CString wk;

		wk.Format("%02d(個)", mcls_MI.gcls_pTest->GetDefRate());
		SetDlgItemText(IDC_DEF_RATE, wk);

	}

	CDialog::OnTimer(nIDEvent);
}


//// 初期化通知
void CHY_TESTDlg::OnBnClickedBtnInit()
{
	// TODO: ここにコントロール通知ハンドラー コードを追加します。
	if( IDYES == MessageBox("初期化通知を送信しますか？", "重要なお知らせ", MB_YESNO | MB_DEFBUTTON2 | MB_ICONQUESTION) ) {
		LOG(em_MSG), "[Dlg] 初期化通知送信ボタン");
		TestManager::DELIVERY_KEY* pQue = new TestManager::DELIVERY_KEY;
		memset(pQue, 0x00, sizeof(TestManager::DELIVERY_KEY));
		pQue->id = TestManager::EM_INIT_NOTIFY;
		mcls_MI.gcls_pTest->gque_Deli.AddToTail(pQue);
	}

}

//// パラメータ変更通知
void CHY_TESTDlg::OnBnClickedBtnParam()
{
	// TODO: ここにコントロール通知ハンドラー コードを追加します。
	TestManager::DELIVERY_KEY* pQue = new TestManager::DELIVERY_KEY;
	memset(pQue, 0x00, sizeof(TestManager::DELIVERY_KEY));
	pQue->id = TestManager::EM_PARAM_NOTIFY;
	mcls_MI.gcls_pTest->gque_Deli.AddToTail(pQue);
}


//// コイル情報追加
void CHY_TESTDlg::OnBnClickedBtnAddcoil()
{
	// TODO: ここにコントロール通知ハンドラー コードを追加します。
	TestManager::DELIVERY_KEY* pQue = new TestManager::DELIVERY_KEY;
	memset(pQue, 0x00, sizeof(TestManager::DELIVERY_KEY));
	pQue->id = TestManager::EM_COIL_ADD;

	mcls_MI.gcls_pTest->gque_Deli.AddToTail(pQue);

}

//// カット情報送信
void CHY_TESTDlg::OnBnClickedBtnCut()
{
	// TODO: ここにコントロール通知ハンドラー コードを追加します。
	LOG(em_MSG), "[Dlg] カット情報送信ボタン");
	TestManager::DELIVERY_KEY* pQue = new TestManager::DELIVERY_KEY;
	memset(pQue, 0x00, sizeof(TestManager::DELIVERY_KEY));
	pQue->id = TestManager::EM_COIL_CUT;
	mcls_MI.gcls_pTest->gque_Deli.AddToTail(pQue);
}

//// 状態通知
void CHY_TESTDlg::OnBnClickedBtnStatus()
{
	// TODO: ここにコントロール通知ハンドラー コードを追加します。
	UpdateData();

	TestManager::DELIVERY_KEY* pQue = new TestManager::DELIVERY_KEY;
	memset(pQue, 0x00, sizeof(TestManager::DELIVERY_KEY));
	pQue->id = TestManager::EM_STATUS_NOTIFY;

	// 運転状態セット
	switch (m_rdoUnten) {
	case 0:	// [運転状態] 停止モード
		pQue->nUnten = DIV_UNTEN_STOP;
		break;
	case 1:	// [運転状態] 通常運転モード
		pQue->nUnten = DIV_UNTEN_NOMAL;
		break;
	case 2:	// [運転状態] テスト運転モード (サンプル撮影)
		pQue->nUnten = DIV_UNTEN_SAMP;
		break;
	case 3:	// [運転状態] 校正モード (カメラ調整)
		pQue->nUnten = DIV_UNTEN_CAMERA;
		break;
	case 4:	// [運転状態] シミュレーションモード
		pQue->nUnten = DIV_UNTEN_SMYU;
		break;
	default:
		pQue->nUnten = DIV_UNTEN_NOMAL;
		break;
	}

	// 稼動状態セット
	switch (m_rdoKadou) {
	case 0:	// [稼動状態] 初期化中
		pQue->nKadou = DIV_KADOU_INIT;
		break;
	case 1:	// [稼動状態] 停止中 (準備完了)
		pQue->nKadou = DIV_KADOU_STOP;
		break;
	case 2:	// [稼動状態] 稼動中 & テスト中
		pQue->nKadou = DIV_KADOU_START;
		break;
	case 3:	// [稼動状態] リトライ中
		pQue->nKadou = DIV_KADOU_RETRY;
		break;
	default:
		pQue->nKadou = DIV_KADOU_START;
		break;
	}

	// 機器状態セット
	switch (m_rdoKiki_o) {
	case 0:	// 正常
		pQue->nKiki[0] = DIV_KIKI_OK;
		break;
	case 1:	// 軽故障
		pQue->nKiki[0] = DIV_KIKI_KEI;
		break;
	case 2:	// 中故障
		pQue->nKiki[0] = 2;
		break;
	case 3:	// 重故障
		pQue->nKiki[0] = DIV_KIKI_JYU;
		break;
	default:
		pQue->nKiki[0] = DIV_KIKI_OK;
		break;
	}
	switch (m_rdoKiki_u) {
	case 0:	// 正常
		pQue->nKiki[1] = DIV_KIKI_OK;
		break;
	case 1:	// 軽故障
		pQue->nKiki[1] = DIV_KIKI_KEI;
		break;
	case 2:	// 中故障
		pQue->nKiki[1] = 2;
		break;
	case 3:	// 重故障
		pQue->nKiki[1] = DIV_KIKI_JYU;
		break;
	default:
		pQue->nKiki[1] = DIV_KIKI_OK;
		break;
	}

	// 検査状態セット
	switch (m_rdoKensa_o) {
	case 0:	// 正常
		pQue->nKensa[0] = DIV_KENSA_OK;
		break;
	case 1:	// 縮退
		pQue->nKensa[0] = DIV_KENSA_NG;
		break;
	case 2:	// メンテ中
		pQue->nKensa[0] = DIV_KENSA_MENTE;
		break;
	case 3: // 停止
		pQue->nKensa[0] = DIV_KENSA_STOP;
		break;
	case 4: // エッジ不正
		pQue->nKensa[0] = DIV_KENSA_EDGENG;
		break;
	case 5: // DB未保存
		pQue->nKensa[0] = DIV_KENSA_DBNON;
		break;
	case 6: // 未検索間
		pQue->nKensa[0] = DIV_KENSA_SKIP;
		break;
	case 7: // 対象外
		pQue->nKensa[0] = DIV_KENSA_NON;
		break;
	default:
		pQue->nKensa[0] = DIV_KENSA_OK;
		break;
	}
	switch (m_rdoKensa_u) {
	case 0:	// 正常
		pQue->nKensa[1] = DIV_KENSA_OK;
		break;
	case 1:	// 縮退
		pQue->nKensa[1] = DIV_KENSA_NG;
		break;
	case 2:	// メンテ中
		pQue->nKensa[1] = DIV_KENSA_MENTE;
		break;
	case 3: // 停止
		pQue->nKensa[1] = DIV_KENSA_STOP;
		break;
	case 4: // エッジ不正
		pQue->nKensa[1] = DIV_KENSA_EDGENG;
		break;
	case 5: // DB未保存
		pQue->nKensa[1] = DIV_KENSA_DBNON;
		break;
	case 6: // 未検索間
		pQue->nKensa[1] = DIV_KENSA_SKIP;
		break;
	case 7: // 対象外
		pQue->nKensa[1] = DIV_KENSA_NON;
		break;
	default:
		pQue->nKensa[1] = DIV_KENSA_OK;
		break;
	}

	LOG(em_MSG), "[Dlg] 運転状態[%d] 稼動状態[%d] 機器状態[%d/%d] 検査状態[%d/%d]", pQue->nUnten, pQue->nKadou, pQue->nKiki[0], pQue->nKiki[1], pQue->nKensa[0], pQue->nKensa[1] );
	mcls_MI.gcls_pTest->gque_Deli.AddToTail(pQue);

}


//// 速度設定
void CHY_TESTDlg::OnBnClickedBtnSpeed()
{
	// TODO: ここにコントロール通知ハンドラー コードを追加します。
	TestManager::DELIVERY_KEY* pQue = new TestManager::DELIVERY_KEY;
	memset(pQue, 0x00, sizeof(TestManager::DELIVERY_KEY));
	pQue->id = TestManager::EM_SPEED_SETTING;
	pQue->speed = GetDlgItemInt(IDC_EDIT_SPEED);
	mcls_MI.gcls_pTest->gque_Deli.AddToTail(pQue);
}

//// PLG停止＆起動
void CHY_TESTDlg::OnBnClickedChkPlg()
{
	// TODO: ここにコントロール通知ハンドラー コードを追加します。
	TestManager::DELIVERY_KEY* pQue = new TestManager::DELIVERY_KEY;
	memset(pQue, 0x00, sizeof(TestManager::DELIVERY_KEY));

	if (!IsDlgButtonChecked(IDC_CHK_PLG)) {
		// 停止
		SetDlgItemText(IDC_CHK_PLG, "PLG停止中");
		pQue->id = TestManager::EM_PLG_STOP;
	}
	else {
		// 起動
		SetDlgItemText(IDC_CHK_PLG, "PLG起動中");
		pQue->id = TestManager::EM_PLG_START;
	}
	mcls_MI.gcls_pTest->gque_Deli.AddToTail(pQue);
}

//// PLG１周期
void CHY_TESTDlg::OnBnClickedBtnSlow()
{
	// TODO: ここにコントロール通知ハンドラー コードを追加します。
	TestManager::DELIVERY_KEY* pQue = new TestManager::DELIVERY_KEY;
	memset(pQue, 0x00, sizeof(TestManager::DELIVERY_KEY));
	pQue->id = TestManager::EM_PLG_SLOW;
	mcls_MI.gcls_pTest->gque_Deli.AddToTail(pQue);
}

//// コイル長　最大・最小セット
void CHY_TESTDlg::OnBnClickedBtnCoilLen()
{
	// TODO: ここにコントロール通知ハンドラー コードを追加します。

	int nMin = GetDlgItemInt(IDC_EDIT_COIL_LEN_MIN);
	int nMax = GetDlgItemInt(IDC_EDIT_COIL_LEN_MAX);

	if ((nMin != 0) && (nMax != 0)) {
		mcls_MI.gcls_pTest->SetCoilLenMin(nMin * 1000);
		mcls_MI.gcls_pTest->SetCoilLenMax(nMax * 1000);
	}

}

//// コイル幅　最大・最小セット
void CHY_TESTDlg::OnBnClickedBtnCoilWid()
{
	// TODO: ここにコントロール通知ハンドラー コードを追加します。

	int nMin = GetDlgItemInt(IDC_EDIT_COIL_WID_MIN);
	int nMax = GetDlgItemInt(IDC_EDIT_COIL_WID_MAX);

	if ((nMin != 0) && (nMax != 0)) {
		mcls_MI.gcls_pTest->SetCoilWidthMin(nMin);
		mcls_MI.gcls_pTest->SetCoilWidthMax(nMax);
	}

}

// 前工程情報の作成 あり・なし チェックボックス
void CHY_TESTDlg::OnBnClickedCheckMae()
{
	// TODO: ここにコントロール通知ハンドラ コードを追加します。
	if (!IsDlgButtonChecked(IDC_CHK_MAE)) {
		// 作成無し
		SetDlgItemText(IDC_CHK_MAE, "前工程情報作成無し");
		mcls_MI.gcls_pTest->SetMaeInfoStatus(false);
	}
	else {
		// 作成あり
		SetDlgItemText(IDC_CHK_MAE, "前工程情報作成あり");
		mcls_MI.gcls_pTest->SetMaeInfoStatus(true);
	}
}

// 溶接点シャーカット実施有無 チェックボックス
void CHY_TESTDlg::OnBnClickedCheckYousetuCut()
{
	// TODO: ここにコントロール通知ハンドラ コードを追加します。
	if (!IsDlgButtonChecked(IDC_CHK_YOUSETUCUT)) {
		// 作成無し
		SetDlgItemText(IDC_CHK_YOUSETUCUT, "溶接点カットを実施しない");
		mcls_MI.gcls_pTest->SetYousetuCut(false);
	}
	else {
		// 作成あり
		SetDlgItemText(IDC_CHK_YOUSETUCUT, "溶接点カットを実施する");
		mcls_MI.gcls_pTest->SetYousetuCut(true);
	}
}

//// 疵発生　有り・無し
void CHY_TESTDlg::OnBnClickedChkDef()
{
	// TODO: ここにコントロール通知ハンドラー コードを追加します。
	if (!IsDlgButtonChecked(IDC_CHK_DEF)) {
		// 疵作成無し
		SetDlgItemText(IDC_CHK_DEF, "疵発生無し");
		mcls_MI.gcls_pTest->SetDefectCreate(false);
	}
	else {
		// 疵作成あり
		SetDlgItemText(IDC_CHK_DEF, "疵発生中");
		mcls_MI.gcls_pTest->SetDefectCreate(true);
	}

}

//// 疵発生数変更
void CHY_TESTDlg::OnBnClickedBtnDefRate()
{
	// TODO: ここにコントロール通知ハンドラー コードを追加します。
	int wk = GetDlgItemInt(IDC_EDIT_DEF_RATE);
	mcls_MI.gcls_pTest->SetDefRate(wk);
}


//// 表面状態コード・検査パターンコード、カウントパターンコード設定
void CHY_TESTDlg::OnBnClickedBtnSetcode()
{
	// TODO: ここにコントロール通知ハンドラー コードを追加します。
	int scode = GetDlgItemInt(IDC_EDIT_SCODE);
	int pcode = GetDlgItemInt(IDC_EDIT_PCODE);
//	int ccode = GetDlgItemInt(IDC_EDIT_CCODE);

	mcls_MI.gcls_pTest->SetSCode(scode);
	mcls_MI.gcls_pTest->SetPCode(pcode);
//	mcls_MI.gcls_pTest->SetCCode(ccode);
}


//// 疵情報送信
void CHY_TESTDlg::OnBnClickedBtnDefSend()
{
	// TODO: ここにコントロール通知ハンドラー コードを追加します。
	char cTmpKizuKnNo[SIZE_KIZUKEN_NO];

	memset(cTmpKizuKnNo, 0x00, sizeof(cTmpKizuKnNo));
	GetDlgItemText(IDC_EDIT_DEF_SYSTEM, &cTmpKizuKnNo[0], SIZE_KIZUKEN_NO);		// 疵検管理No

	TestManager::DELIVERY_KEY* pQue = new TestManager::DELIVERY_KEY;
	memset(pQue, 0x00, sizeof(TestManager::DELIVERY_KEY));
	pQue->id = TestManager::EM_DEFECT;
	if (strlen(cTmpKizuKnNo) > 0) {							// 管理No入力有
		memcpy(pQue->cKizukenNo, &cTmpKizuKnNo[0], SIZE_KIZUKEN_NO);
	}
	pQue->nY = GetDlgItemInt(IDC_EDIT_DEF_Y);
	pQue->dDS = GetDlgItemInt(IDC_EDIT_DEF_DSWS);	// 幅方向分割位置を指定(0～7)
	pQue->nMen = GetDlgItemInt(IDC_EDIT_DEF_MEN);
	pQue->Tid = GetDlgItemInt(IDC_EDIT_DEF_TID);
	pQue->Gid = GetDlgItemInt(IDC_EDIT_DEF_GID);
	pQue->bNotChkDsWs = m_chkDsWs.GetCheck();
	mcls_MI.gcls_pTest->gque_Deli.AddToTail(pQue);
}

//// 重欠陥疵情報削除要求
void CHY_TESTDlg::OnBnClickedBtnLstCancelSend()
{
	// TODO: ここにコントロール通知ハンドラー コードを追加します。
	TestManager::DELIVERY_KEY* pQue = new TestManager::DELIVERY_KEY;
	memset(pQue, 0x00, sizeof(TestManager::DELIVERY_KEY));
	char	cTmpKizuKnNo[SIZE_KIZUKEN_NO];
	memset(cTmpKizuKnNo, 0x00, sizeof(cTmpKizuKnNo));
	GetDlgItemText(IDC_EDIT_DEFLST_SYSTEM, &cTmpKizuKnNo[0], SIZE_KIZUKEN_NO);		// 疵検管理No

	pQue->id = TestManager::EM_DEFLST_CANCEL;
	pQue->nCancelNo = GetDlgItemInt(IDC_EDIT_DEFLST_KIZUSEQ);
	memcpy(pQue->cKizukenNo, &cTmpKizuKnNo[0], SIZE_KIZUKEN_NO);
	pQue->nMen = GetDlgItemInt(IDC_EDIT_DEFLST_MEN);
	mcls_MI.gcls_pTest->gque_Deli.AddToTail(pQue);
}

//// 通板位置情報送信
void CHY_TESTDlg::OnBnClickedChkPos()
{
	// TODO: ここにコントロール通知ハンドラー コードを追加します。
	if (!IsDlgButtonChecked(IDC_CHK_POS)) {
		// 送信なし
		mcls_MI.gcls_pTest->SetSendPos(false);
	}
	else {
		// 送信あり
		mcls_MI.gcls_pTest->SetSendPos(true);
	}
}

//// 警報情報送信
void CHY_TESTDlg::OnBnClickedBtnAlarmSend()
{
	// TODO: ここにコントロール通知ハンドラー コードを追加します。
	TestManager::DELIVERY_KEY* pQue = new TestManager::DELIVERY_KEY;
	memset(pQue, 0x00, sizeof(TestManager::DELIVERY_KEY));
	pQue->id = TestManager::EM_ALARM;
	pQue->nLanp[0][0][0] = GetDlgItemInt(IDC_EDIT_ALARM0_0_1);
	pQue->nLanp[0][0][1] = GetDlgItemInt(IDC_EDIT_ALARM0_0_2);
	pQue->nLanp[0][0][2] = GetDlgItemInt(IDC_EDIT_ALARM0_0_3);
	pQue->nLanp[0][0][3] = GetDlgItemInt(IDC_EDIT_ALARM0_0_4);
	pQue->nLanp[0][0][4] = GetDlgItemInt(IDC_EDIT_ALARM0_0_5);
	pQue->nLanp[0][0][5] = GetDlgItemInt(IDC_EDIT_ALARM0_0_6);
	pQue->nLanp[0][0][6] = GetDlgItemInt(IDC_EDIT_ALARM0_0_7);
	pQue->nLanp[0][0][7] = GetDlgItemInt(IDC_EDIT_ALARM0_0_8);
	pQue->nLanp[0][1][0] = GetDlgItemInt(IDC_EDIT_ALARM0_1_1);
	pQue->nLanp[0][1][1] = GetDlgItemInt(IDC_EDIT_ALARM0_1_2);
	pQue->nLanp[0][1][2] = GetDlgItemInt(IDC_EDIT_ALARM0_1_3);
	pQue->nLanp[0][1][3] = GetDlgItemInt(IDC_EDIT_ALARM0_1_4);
	pQue->nLanp[0][1][4] = GetDlgItemInt(IDC_EDIT_ALARM0_1_5);
	pQue->nLanp[0][1][5] = GetDlgItemInt(IDC_EDIT_ALARM0_1_6);
	pQue->nLanp[0][1][6] = GetDlgItemInt(IDC_EDIT_ALARM0_1_7);
	pQue->nLanp[0][1][7] = GetDlgItemInt(IDC_EDIT_ALARM0_1_8);
	pQue->nLanp[1][0][0] = GetDlgItemInt(IDC_EDIT_ALARM1_0_1);
	pQue->nLanp[1][0][1] = GetDlgItemInt(IDC_EDIT_ALARM1_0_2);
	pQue->nLanp[1][0][2] = GetDlgItemInt(IDC_EDIT_ALARM1_0_3);
	pQue->nLanp[1][0][3] = GetDlgItemInt(IDC_EDIT_ALARM1_0_4);
	pQue->nLanp[1][0][4] = GetDlgItemInt(IDC_EDIT_ALARM1_0_5);
	pQue->nLanp[1][0][5] = GetDlgItemInt(IDC_EDIT_ALARM1_0_6);
	pQue->nLanp[1][0][6] = GetDlgItemInt(IDC_EDIT_ALARM1_0_7);
	pQue->nLanp[1][0][7] = GetDlgItemInt(IDC_EDIT_ALARM1_0_8);
	pQue->nLanp[1][1][0] = GetDlgItemInt(IDC_EDIT_ALARM1_1_1);
	pQue->nLanp[1][1][1] = GetDlgItemInt(IDC_EDIT_ALARM1_1_2);
	pQue->nLanp[1][1][2] = GetDlgItemInt(IDC_EDIT_ALARM1_1_3);
	pQue->nLanp[1][1][3] = GetDlgItemInt(IDC_EDIT_ALARM1_1_4);
	pQue->nLanp[1][1][4] = GetDlgItemInt(IDC_EDIT_ALARM1_1_5);
	pQue->nLanp[1][1][5] = GetDlgItemInt(IDC_EDIT_ALARM1_1_6);
	pQue->nLanp[1][1][6] = GetDlgItemInt(IDC_EDIT_ALARM1_1_7);
	pQue->nLanp[1][1][7] = GetDlgItemInt(IDC_EDIT_ALARM1_1_8);
	mcls_MI.gcls_pTest->gque_Deli.AddToTail(pQue);
}

//------------------------------------------
// SPM圧下状態
//------------------------------------------
CString CHY_TESTDlg::GetName_SpmUmu(EM_DIV_SPM em)
{
	CString s;
	if (DIV_SPM_OFF == em)		s = "無";
	else if (DIV_SPM_ON == em)	s = "有";
	else						s.Format("%d?", em);
	return s;
}

//------------------------------------------
// 疵個数送信
//------------------------------------------
void CHY_TESTDlg::OnBnClickedBtnDefcntSend()
{
	// TODO: ここにコントロール通知ハンドラー コードを追加します。
	char cTmpKizuKnNo[SIZE_KIZUKEN_NO];
	int nMen = GetDlgItemInt(IDC_EDIT_DEFCNT_MEN);
	int nY = GetDlgItemInt(IDC_EDIT_DEFCNT_Y);
	int nCnt = GetDlgItemInt(IDC_EDIT_DEFCNT_CNT);
	
	memset(cTmpKizuKnNo, 0x00, sizeof(cTmpKizuKnNo));
	GetDlgItemText(IDC_EDIT_DEFCNT_SYSTEM, &cTmpKizuKnNo[0], SIZE_KIZUKEN_NO);		// 疵検管理No

	if (strlen(cTmpKizuKnNo) < 1 || nMen < 0 || nY < 0) {							// 未入力項目有
		return;
	}

	TestManager::DELIVERY_KEY* pQue = new TestManager::DELIVERY_KEY;
	memset(pQue, 0x00, sizeof(TestManager::DELIVERY_KEY));

	pQue->id = TestManager::EM_DEFECT_NUM;
	pQue->nMen = nMen;
	memcpy(pQue->cKizukenNo, &cTmpKizuKnNo[0], SIZE_KIZUKEN_NO);
	pQue->nY = nY;
	pQue->nCnt = nCnt;
	mcls_MI.gcls_pTest->gque_Deli.AddToTail(pQue);
}
