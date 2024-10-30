// TO_LINKSWDlg.cpp : 実装ファイル
//

#include "stdafx.h"
#include "TO_LINKSW.h"
#include "TO_LINKSWDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CTO_LINKSWDlg ダイアログ




CTO_LINKSWDlg::CTO_LINKSWDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CTO_LINKSWDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CTO_LINKSWDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, m_ListBox);
	DDX_Control(pDX, IDC_UNIT_DSP, m_cmbUnitDsp);
	DDX_Control(pDX, IDC_BOARD, m_ListBoard);
	DDX_Control(pDX, IDC_UNIT_EXEC, m_cmbUnitExec);
}

BEGIN_MESSAGE_MAP(CTO_LINKSWDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_WM_CLOSE()
	ON_WM_TIMER()
	ON_BN_CLICKED(IDOK, &CTO_LINKSWDlg::OnBnClickedOk)
	ON_LBN_DBLCLK(IDC_LIST1, &CTO_LINKSWDlg::OnLbnDblclkList1)
	ON_BN_CLICKED(IDC_DEBUG_PARAM, &CTO_LINKSWDlg::OnBnClickedDebugParam)
	ON_BN_CLICKED(IDC_DEBUG_START_SET, &CTO_LINKSWDlg::OnBnClickedDebugStartSet)
	ON_BN_CLICKED(IDC_DEBUG_CAMERA_SET, &CTO_LINKSWDlg::OnBnClickedDebugCameraSet)
	ON_BN_CLICKED(IDC_DEBUG_CCMODE, &CTO_LINKSWDlg::OnBnClickedDebugCcmode)
	ON_BN_CLICKED(IDC_DEBUG_SLAVE_SET, &CTO_LINKSWDlg::OnBnClickedDebugSlaveSet)
	ON_BN_CLICKED(IDC_DEBUG_COUNTER_RESET, &CTO_LINKSWDlg::OnBnClickedDebugCounterReset)
	ON_BN_CLICKED(IDC_DEBUG_COUNTER_CHECK, &CTO_LINKSWDlg::OnBnClickedDebugCounterCheck)
	ON_BN_CLICKED(IDC_DEBUG_SET_SERIAL_SPEED, &CTO_LINKSWDlg::OnBnClickedDebugSetSerialSpeed)
END_MESSAGE_MAP()


// CTO_LINKSWDlg メッセージ ハンドラ

BOOL CTO_LINKSWDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// このダイアログのアイコンを設定します。アプリケーションのメイン ウィンドウがダイアログでない場合、
	//  Framework は、この設定を自動的に行います。
	SetIcon(m_hIcon, TRUE);			// 大きいアイコンの設定
	SetIcon(m_hIcon, FALSE);		// 小さいアイコンの設定

	// TODO: 初期化をここに追加します。

	// 画面位置 (サイズはいまのまま)
	::SetWindowPos(m_hWnd, NULL, 0, 1, 0, 0, SWP_NOSIZE | SWP_NOZORDER);

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

	SendDlgItemMessage(	IDC_DEBUG_SERIAL_SPEED_VALUE, CB_ADDSTRING, 0, (LPARAM)"9600");
	SendDlgItemMessage(	IDC_DEBUG_SERIAL_SPEED_VALUE, CB_ADDSTRING, 0, (LPARAM)"115200");
// <<<-------------------------------
	return TRUE;  // フォーカスをコントロールに設定した場合を除き、TRUE を返します。
}

// ダイアログに最小化ボタンを追加する場合、アイコンを描画するための
//  下のコードが必要です。ドキュメント/ビュー モデルを使う MFC アプリケーションの場合、
//  これは、Framework によって自動的に設定されます。

void CTO_LINKSWDlg::OnPaint()
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
HCURSOR CTO_LINKSWDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

// これ以降 手動追加 -------------------->>>

//￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣
// ×ボタン
//＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿
void CTO_LINKSWDlg::OnClose()
{
	//CDialog::OnClose();
}

//￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣
// 閉じるボタン
//＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿
void CTO_LINKSWDlg::OnBnClickedOk()
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
void CTO_LINKSWDlg::OnLbnDblclkList1()
{
	// ListBoxコントロールのプロパティは、ソート禁止にしておく事
	// ListBoxコントロールのダブルクリックイベントを追加
	mcls_pLog->ShowEdit();
}

//￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣
// 各プロセス初期化処理
//＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿
void CTO_LINKSWDlg::InitProcess()
{
	// タスク初期化処理
	if( 0 != task_init(TO_LINKSW) )
	{
		syslog(SYSNO_TASK_INIT_ERR, "[%s]", TO_LINKSW);
		_ASSERT(0);
		exit(-9);		// 自プロセス強制終了
		return;
	}

	//// LogFileManagerインスンタス生成 (これ以降、ログ出力可能)
	setlocale(LC_ALL, "Japanese");	// これが無いと日本語パスを含むフォルダの場合、保存できない。 (ライブラリの中で実行するべきか迷っているが、変更するとタスク全体に影響するので、とりあえずここで設定)
	mcls_pLog = new LogFileManager(getmytaskname(), &m_ListBox);
	LOG(em_MSG), "[Dlg] タスク起動");
	LOG(em_MSG), "[Dlg] 各プロセス開始処理 実行中・・・");

	//-----------------------
	// ボード接続状態
	if(true) {
		mcnt_pListBoard = new DspListView(&m_ListBoard);
		// 見た目定義
		m_ListBoard.SetExtendedStyle(m_ListBoard.GetExtendedStyle() | LVS_EX_GRIDLINES );			// 罫線描画
		m_ListBoard.SetExtendedStyle(m_ListBoard.GetExtendedStyle() | LVS_EX_FULLROWSELECT );		// 1行全体選択
		m_ListBoard.ModifyStyle( 0, LVS_SHOWSELALWAYS);			// 非フォーカス時も選択が残る

		// 列追加			0		1	
		//					2		3			4			5			6			7			8			9
		//					10		11			12			13			14			15			16
		//					17		18
		char name[][32] = { "ID",	"ｽﾃｰﾀｽ"
						, "ﾎﾞｰﾄﾞ1",  "2",  "3",  "4",  "5",  "6",  "7",  "8"
						, "9", "10", "11", "12", "13", "14", "15"
						, "周囲温","ﾀﾞｲ温"};
		int  wid[]		= { 23, 	50
						, 50,		50,			50,			50,			50,			50,			50,			50
						, 50,		50,			50,			50,			50,			50,			50
						, 50,		50};
		mcnt_pListBoard->SetColumnAll(sizeof(wid) / sizeof(int), 32, wid, (char const*)name);
		// 行追加
		mcnt_pListBoard->SetRowAll(2 * NUM_CAMLINKSW * MAX_CAMLINKSW_FPGA);

	}

	//// メインインスタンス 開始処理
	mcls_MI.SetLogMgr(mcls_pLog);
	mcls_MI.Init();

	//// 画面更新用データセット
	// コンボボックスにセッション名追加
	m_cmbUnitExec.AddString("全ユニット");
	for (int ii = 0; ii<MAX_THREAD; ii++) {
		////// ？？ なぜかReleaseの場合だけ 文字列に _ が入っていると登録失敗する
		if (NULL == mcls_MI.gcls_pLinksw[ii]) break;
		m_cmbUnitDsp.AddString(mcls_MI.gcls_pLinksw[ii]->GetMySession());
		m_cmbUnitExec.AddString(mcls_MI.gcls_pLinksw[ii]->GetMySession());
	}
	m_cmbUnitDsp.SetCurSel(0);				// 0番目を初期選択とする
	m_cmbUnitExec.SetCurSel(0);

	//// 画面更新用タイマ生成
	SetTimer(1, 500, NULL);

	//// メインスレッドの実行を行う
	LOG(em_MSG), "[Dlg] 各プロセス開始処理 完了");
	mcls_MI.Start();
	return;
}

//￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣
// 各プロセス終了処理
//＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿
void CTO_LINKSWDlg::ExitProcess()
{
	//// メインインスンタス 終了処理
	KillTimer(1);			// 画面更新タイマ
	mcls_MI.Stop();
	mcls_MI.Exit();

	//// ログ リストボックス表示処理終了
	mcls_pLog->SetListOut(false);					// これ以降 リストボックスへのログ表示禁止

	//// ログクラス解放
	LOG(em_MSG), "[Dlg] タスク終了");
	if( NULL != mcls_pLog )
	{
		delete mcls_pLog;
		mcls_pLog = NULL;
	}

	delete mcnt_pListBoard;

	//// タスク終了処理
	task_exit();

	// 画面の終了要求
	PostMessage(WM_QUIT);
}

//￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣
// メインスレッド DlgThread
// LPVOID param ダイアログポインタ
//＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿
unsigned WINAPI CTO_LINKSWDlg::MainThread(LPVOID param)
{
	CTO_LINKSWDlg * p = (CTO_LINKSWDlg *)param;
	COMMON_QUE que;			// 受信構造体

	//// 各プロセス初期化処理 (これ以前はログ出力禁止)
	p->InitProcess();

	//// 起動要因取り込み ループ開始
	while( true )
	{
		recv_mail(&que);			// 起動要因取り込み

		switch( que.nEventNo )
		{
//-----------------------------------------------------------------------------------------------
		case FACT_TO_LINKSW_01:								// パラメータ変更要求 (←TO_DEFECT)
			pLOG(em_MSG), "[MAIL] パラメータ変更要求 (FACT_TO_LINKSW_01) [検査表面状態=%d]", que.fl.data[0]);

			p->CommandSender(&que);
			break;

//-----------------------------------------------------------------------------------------------
		case FACT_TO_LINKSW_02:								// 検査用設定コマンド (←TO_DEFECT)
			pLOG(em_MSG), "[MAIL] 検査用設定コマンド (FACT_TO_LINKSW_02)");

			p->CommandSender(&que);
			break;

//-----------------------------------------------------------------------------------------------
		case FACT_TO_LINKSW_03:								// カメラ選択設定コマンド (←TO_DEFECT)
			// 2017.02.27 Master/SlaveをDB指定 ---------->>>>
			//pLOG(em_MSG), "[MAIL] カメラ選択設定コマンド (FACT_TO_LINKSW_03) [ID=%d ボード=%d カメラ選択=%d]", que.fl.data[0], que.fl.data[1], que.fl.data[2]);
			pLOG(em_MSG), "[MAIL] カメラ選択設定コマンド (FACT_TO_LINKSW_03) [ID=%d ボード=%d カメラ選択=%d Master/Slave区分=%d]", que.fl.data[0], que.fl.data[1], que.fl.data[2], que.fl.data[3]);
			// 2017.02.27 Master/SlaveをDB指定 <<<<----------

			p->CommandSender(&que);
			break;

//-----------------------------------------------------------------------------------------------
		case FACT_TO_LINKSW_04:								// 途中参加設定コマンド (←TO_DEFECT)
			pLOG(em_MSG), "[MAIL] 途中参加設定コマンド (FACT_TO_LINKSW_04) [表裏区分=%d 途中参加フレームNo=%u]", que.fl.data[0], (DWORD)que.fl.data[1]);

			p->CommandSender(&que);
			break;

//-----------------------------------------------------------------------------------------------
		case FACT_TO_LINKSW_05:								// 高速リセット通知 (←TO_DEFECT)
			pLOG(em_MSG), "[MAIL] 高速リセット通知 (FACT_TO_LINKSW_05) [カウンタリセットフレームNo=%d]", que.fl.data[0]);

			p->CommandSender(&que);
			break;

//-----------------------------------------------------------------------------------------------
		case FACT_TO_LINKSW_06:								// AOC速度変更指示( udp )

			// 高速通信が無効の場合は、ここで止める
			if(true) {
				int ii_s, ii_e;
				bool bRet = true;
				p->mcls_MI.GetChLoop(que.nLineNo, ii_s, ii_e);

				for (int ii = ii_s; ii < ii_e; ii++) {
					if (!p->mcls_MI.gcls_pLinksw[ii]->IsEnableRapidSerial()) bRet = false;
				}
				if( bRet ){
					pLOG(em_MSG), "[MAIL] AOCシリアル設定 (FACT_TO_LINKSW_06) [BoardID=%d, Speed=%d]", que.fl.data[0], que.fl.data[1]);

					p->CommandSender(&que);
				}else{

					pLOG(em_WAR), "[MAIL] AOCシリアル設定 (FACT_TO_LINKSW_06) 通信速度切替無効の為、依頼を破棄！ [BoardID=%d, Speed=%d]", que.fl.data[0], que.fl.data[1]);
				}
			}
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

//-----------------------------------------------------------------------------------------------
		default:
			// ありえないはず！！
			pLOG(em_MSG), "[MAIL] イベントNo無し [EventNo=%d]", que.nEventNo);
			syslog(SYSNO_MAILNO_ERR, "[EventNo=%d]", que.nEventNo);
			break;
		}
	}

	return 0;
}

//￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣
// コマンド処理開始要求
// COMMON_QUE * que
//＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿
void CTO_LINKSWDlg::CommandSender(COMMON_QUE * que)
{
	//COMMON_QUE * pCq = new COMMON_QUE;
	//memcpy(pCq, que, sizeof(COMMON_QUE));
	//mcls_MI.gcls_pLinksw->gque_Deli.AddToTail(pCq);
	int ii_s, ii_e;
	mcls_MI.GetChLoop(que->nLineNo, ii_s, ii_e);

	for (int ii = ii_s; ii<ii_e; ii++) {
		COMMON_QUE *pCq = new COMMON_QUE;
		memcpy(pCq, que, sizeof(COMMON_QUE));

		mcls_MI.gcls_pLinksw[ii]->gque_Deli.AddToTail(pCq);
	}
}

//￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣
// 画面更新タイマ
//＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿
void CTO_LINKSWDlg::OnTimer(UINT_PTR nIDEvent)
{
	//// インスタンス選択
	int nIdx = m_cmbUnitDsp.GetCurSel();
	if (-1 == nIdx) return;					// iniファイルの設定無し

	CString sWk;
	LinkswManager::PARAM	param = mcls_MI.gcls_pLinksw[nIdx]->GetParamInf();	// パラメータ

	static bool bIsInitEvent = true;		// 初回の更新タイマ？

	// 初回更新時のみ実行
	if( bIsInitEvent ){

		bIsInitEvent = false;

		if( ! mcls_MI.gcls_pLinksw[nIdx]->IsEnableRapidSerial() ){

			// 高速切替が無効の場合は、高速切替用のデバッグメニューを無効にする
			CWnd::GetDlgItem(IDC_DEBUG_SET_SERIAL_SPEED)->EnableWindow(false);		// 「AOC速度変更」 ボタン
			CWnd::GetDlgItem(IDC_DEBUG_CAMNO)->EnableWindow(false);					// 「ボードNO」    テキストボックス
			CWnd::GetDlgItem(IDC_DEBUG_SERIAL_SPEED_VALUE)->EnableWindow(false);	// 「シリアル速度」コンボボックス
			CWnd::GetDlgItem(IDC_STATIC_CAMNO)->EnableWindow(false);				// 「ボードNO」    ラベル
			CWnd::GetDlgItem(IDC_STATIC_SERIAL_SPEED)->EnableWindow(false);			// 「シリアル速度」ラベル
		}
	}

	// 回線状態
	SetDlgItemText(IDC_EDIT_RS232C,		(mcls_MI.gcls_pLinksw[nIdx]->IsConnect() ? "○" : "×"));

	// 固定パラメータ
	SetDlgItemInt(IDC_PARAM_CAMTAPNUM,	param.nLineBuffer);
	SetDlgItemInt(IDC_PARAM_YSIZE,		param.nYSize);

	// カメラリンクスイッチ設定
	//int txtID[MAX_CAMLINKSW_DAISYCHAIN * MAX_CAMLINKSW_FPGA] =
	//{
	//	IDC_ID0,  IDC_ID1,  IDC_ID2,  IDC_ID3,
	//	IDC_ID4,  IDC_ID5,  IDC_ID6,  IDC_ID7,
	//	IDC_ID8,  IDC_ID9,  IDC_ID10, IDC_ID11,
	//	IDC_ID12, IDC_ID13, IDC_ID14, IDC_ID15
	//};
	//int txtStatus[MAX_CAMLINKSW_DAISYCHAIN * MAX_CAMLINKSW_FPGA] =
	//{
	//	IDC_STATUS0,  IDC_STATUS1,  IDC_STATUS2,  IDC_STATUS3,
	//	IDC_STATUS4,  IDC_STATUS5,  IDC_STATUS6,  IDC_STATUS7,
	//	IDC_STATUS8,  IDC_STATUS9,  IDC_STATUS10, IDC_STATUS11,
	//	IDC_STATUS12, IDC_STATUS13, IDC_STATUS14, IDC_STATUS15
	//};
	//int txtBoard[MAX_CAMLINKSW_DAISYCHAIN * MAX_CAMLINKSW_FPGA][MAX_CAMLINKSW_OUTPUT] =
	//{
	//	{
	//		IDC_BOARD0_0,  IDC_BOARD1_0,  IDC_BOARD2_0,  IDC_BOARD3_0,  IDC_BOARD4_0,
	//		IDC_BOARD5_0,  IDC_BOARD6_0,  IDC_BOARD7_0,  IDC_BOARD8_0,  IDC_BOARD9_0,
	//		IDC_BOARD10_0, IDC_BOARD11_0, IDC_BOARD12_0, IDC_BOARD13_0, IDC_BOARD14_0
	//	},
	//	{
	//		IDC_BOARD0_1,  IDC_BOARD1_1,  IDC_BOARD2_1,  IDC_BOARD3_1,  IDC_BOARD4_1,
	//		IDC_BOARD5_1,  IDC_BOARD6_1,  IDC_BOARD7_1,  IDC_BOARD8_1,  IDC_BOARD9_1,
	//		IDC_BOARD10_1, IDC_BOARD11_1, IDC_BOARD12_1, IDC_BOARD13_1, IDC_BOARD14_1
	//	},
	//	{
	//		IDC_BOARD0_2,  IDC_BOARD1_2,  IDC_BOARD2_2,  IDC_BOARD3_2,  IDC_BOARD4_2,
	//		IDC_BOARD5_2,  IDC_BOARD6_2,  IDC_BOARD7_2,  IDC_BOARD8_2,  IDC_BOARD9_2,
	//		IDC_BOARD10_2, IDC_BOARD11_2, IDC_BOARD12_2, IDC_BOARD13_2, IDC_BOARD14_2
	//	},
	//	{
	//		IDC_BOARD0_3,  IDC_BOARD1_3,  IDC_BOARD2_3,  IDC_BOARD3_3,  IDC_BOARD4_3,
	//		IDC_BOARD5_3,  IDC_BOARD6_3,  IDC_BOARD7_3,  IDC_BOARD8_3,  IDC_BOARD9_3,
	//		IDC_BOARD10_3, IDC_BOARD11_3, IDC_BOARD12_3, IDC_BOARD13_3, IDC_BOARD14_3
	//	},
	//	{
	//		IDC_BOARD0_4,  IDC_BOARD1_4,  IDC_BOARD2_4,  IDC_BOARD3_4,  IDC_BOARD4_4,
	//		IDC_BOARD5_4,  IDC_BOARD6_4,  IDC_BOARD7_4,  IDC_BOARD8_4,  IDC_BOARD9_4,
	//		IDC_BOARD10_4, IDC_BOARD11_4, IDC_BOARD12_4, IDC_BOARD13_4, IDC_BOARD14_4
	//	},
	//	{
	//		IDC_BOARD0_5,  IDC_BOARD1_5,  IDC_BOARD2_5,  IDC_BOARD3_5,  IDC_BOARD4_5,
	//		IDC_BOARD5_5,  IDC_BOARD6_5,  IDC_BOARD7_5,  IDC_BOARD8_5,  IDC_BOARD9_5,
	//		IDC_BOARD10_5, IDC_BOARD11_5, IDC_BOARD12_5, IDC_BOARD13_5, IDC_BOARD14_5
	//	},
	//	{
	//		IDC_BOARD0_6,  IDC_BOARD1_6,  IDC_BOARD2_6,  IDC_BOARD3_6,  IDC_BOARD4_6,
	//		IDC_BOARD5_6,  IDC_BOARD6_6,  IDC_BOARD7_6,  IDC_BOARD8_6,  IDC_BOARD9_6,
	//		IDC_BOARD10_6, IDC_BOARD11_6, IDC_BOARD12_6, IDC_BOARD13_6, IDC_BOARD14_6
	//	},
	//	{
	//		IDC_BOARD0_7,  IDC_BOARD1_7,  IDC_BOARD2_7,  IDC_BOARD3_7,  IDC_BOARD4_7,
	//		IDC_BOARD5_7,  IDC_BOARD6_7,  IDC_BOARD7_7,  IDC_BOARD8_7,  IDC_BOARD9_7,
	//		IDC_BOARD10_7, IDC_BOARD11_7, IDC_BOARD12_7, IDC_BOARD13_7, IDC_BOARD14_7
	//	},
	//	{
	//		IDC_BOARD0_8,  IDC_BOARD1_8,  IDC_BOARD2_8,  IDC_BOARD3_8,  IDC_BOARD4_8,
	//		IDC_BOARD5_8,  IDC_BOARD6_8,  IDC_BOARD7_8,  IDC_BOARD8_8,  IDC_BOARD9_8,
	//		IDC_BOARD10_8, IDC_BOARD11_8, IDC_BOARD12_8, IDC_BOARD13_8, IDC_BOARD14_8
	//	},
	//	{
	//		IDC_BOARD0_9,  IDC_BOARD1_9,  IDC_BOARD2_9,  IDC_BOARD3_9,  IDC_BOARD4_9,
	//		IDC_BOARD5_9,  IDC_BOARD6_9,  IDC_BOARD7_9,  IDC_BOARD8_9,  IDC_BOARD9_9,
	//		IDC_BOARD10_9, IDC_BOARD11_9, IDC_BOARD12_9, IDC_BOARD13_9, IDC_BOARD14_9
	//	},
	//	{
	//		IDC_BOARD0_10,  IDC_BOARD1_10,  IDC_BOARD2_10,  IDC_BOARD3_10,  IDC_BOARD4_10,
	//		IDC_BOARD5_10,  IDC_BOARD6_10,  IDC_BOARD7_10,  IDC_BOARD8_10,  IDC_BOARD9_10,
	//		IDC_BOARD10_10, IDC_BOARD11_10, IDC_BOARD12_10, IDC_BOARD13_10, IDC_BOARD14_10
	//	},
	//	{
	//		IDC_BOARD0_11,  IDC_BOARD1_11,  IDC_BOARD2_11,  IDC_BOARD3_11,  IDC_BOARD4_11,
	//		IDC_BOARD5_11,  IDC_BOARD6_11,  IDC_BOARD7_11,  IDC_BOARD8_11,  IDC_BOARD9_11,
	//		IDC_BOARD10_11, IDC_BOARD11_11, IDC_BOARD12_11, IDC_BOARD13_11, IDC_BOARD14_11
	//	},
	//	{
	//		IDC_BOARD0_12,  IDC_BOARD1_12,  IDC_BOARD2_12,  IDC_BOARD3_12,  IDC_BOARD4_12,
	//		IDC_BOARD5_12,  IDC_BOARD6_12,  IDC_BOARD7_12,  IDC_BOARD8_12,  IDC_BOARD9_12,
	//		IDC_BOARD10_12, IDC_BOARD11_12, IDC_BOARD12_12, IDC_BOARD13_12, IDC_BOARD14_12
	//	},
	//	{
	//		IDC_BOARD0_13,  IDC_BOARD1_13,  IDC_BOARD2_13,  IDC_BOARD3_13,  IDC_BOARD4_13,
	//		IDC_BOARD5_13,  IDC_BOARD6_13,  IDC_BOARD7_13,  IDC_BOARD8_13,  IDC_BOARD9_13,
	//		IDC_BOARD10_13, IDC_BOARD11_13, IDC_BOARD12_13, IDC_BOARD13_13, IDC_BOARD14_13
	//	},
	//	{
	//		IDC_BOARD0_14,  IDC_BOARD1_14,  IDC_BOARD2_14,  IDC_BOARD3_14,  IDC_BOARD4_14,
	//		IDC_BOARD5_14,  IDC_BOARD6_14,  IDC_BOARD7_14,  IDC_BOARD8_14,  IDC_BOARD9_14,
	//		IDC_BOARD10_14, IDC_BOARD11_14, IDC_BOARD12_14, IDC_BOARD13_14, IDC_BOARD14_14
	//	},
	//	{
	//		IDC_BOARD0_15,  IDC_BOARD1_15,  IDC_BOARD2_15,  IDC_BOARD3_15,  IDC_BOARD4_15,
	//		IDC_BOARD5_15,  IDC_BOARD6_15,  IDC_BOARD7_15,  IDC_BOARD8_15,  IDC_BOARD9_15,
	//		IDC_BOARD10_15, IDC_BOARD11_15, IDC_BOARD12_15, IDC_BOARD13_15, IDC_BOARD14_15
	//	}
	//};
	//int txtAmbientTemp[MAX_CAMLINKSW_DAISYCHAIN * MAX_CAMLINKSW_FPGA] =
	//					  { IDC_AMBIENT_TEMP_0,  IDC_AMBIENT_TEMP_1,  IDC_AMBIENT_TEMP_2,  IDC_AMBIENT_TEMP_3,  IDC_AMBIENT_TEMP_4,  IDC_AMBIENT_TEMP_5,
	//						IDC_AMBIENT_TEMP_6,  IDC_AMBIENT_TEMP_7,  IDC_AMBIENT_TEMP_8,  IDC_AMBIENT_TEMP_9,  IDC_AMBIENT_TEMP_10, IDC_AMBIENT_TEMP_11,
	//						IDC_AMBIENT_TEMP_12, IDC_AMBIENT_TEMP_13, IDC_AMBIENT_TEMP_14, IDC_AMBIENT_TEMP_15 };
	//int txtDieTemp[MAX_CAMLINKSW_DAISYCHAIN * MAX_CAMLINKSW_FPGA] =
	//					  { IDC_FPGADIE_TEMP_0,  IDC_FPGADIE_TEMP_1,  IDC_FPGADIE_TEMP_2,  IDC_FPGADIE_TEMP_3,  IDC_FPGADIE_TEMP_4,  IDC_FPGADIE_TEMP_5,
	//						IDC_FPGADIE_TEMP_6,  IDC_FPGADIE_TEMP_7,  IDC_FPGADIE_TEMP_8,  IDC_FPGADIE_TEMP_9,  IDC_FPGADIE_TEMP_10, IDC_FPGADIE_TEMP_11,
	//						IDC_FPGADIE_TEMP_12, IDC_FPGADIE_TEMP_13, IDC_FPGADIE_TEMP_14, IDC_FPGADIE_TEMP_15 };
	for( int ii=0; ii<NUM_CAMLINKSW * MAX_CAMLINKSW_FPGA; ii++ )
	{
		//SetDlgItemInt(txtID[ii], ii);
		//SetDlgItemText(txtStatus[ii], param.linksw[ii].bStatus ? "ﾏｽﾀ" : "ｽﾚｰﾌﾞ");
		mcnt_pListBoard->SetItem(ii*2, EM_DSP_COL_NAME::COL_ID, ii);
		mcnt_pListBoard->SetItem(ii*2, EM_DSP_COL_NAME::COL_STATUS, param.linksw[ii].bStatus ? "ﾏｽﾀ" : "ｽﾚｰﾌﾞ");
		for( int jj=0; jj<MAX_CAMLINKSW_OUTPUT; jj++ )
		{
			//SetDlgItemInt(txtBoard[ii][jj], param.linksw[ii].nSelectCam[jj]);
			mcnt_pListBoard->SetItem(ii*2,   jj+ EM_DSP_COL_NAME::COL_BOARD, param.linksw[ii].nSelectCam[jj]);
			if(0 != param.linksw[ii].nSelectCam[jj]) {
				sWk.Format("%d",
					mcls_MI.gcls_pLinksw[nIdx]->GetCounterNow(ii, jj)
				);
			}
			else {
				sWk.Format("");
			}
			mcnt_pListBoard->SetItem(ii*2+1, jj+ EM_DSP_COL_NAME::COL_BOARD, sWk);
		}
		//SetDlgItemInt(txtAmbientTemp[ii], mcls_MI.gcls_pLinksw[nIdx]->GetAmbTemp(ii));
		//SetDlgItemInt(txtDieTemp[ii], mcls_MI.gcls_pLinksw[nIdx]->GetDieTemp(ii));
		mcnt_pListBoard->SetItem(ii*2, EM_DSP_COL_NAME::COL_TEMP_SYU, mcls_MI.gcls_pLinksw[nIdx]->GetAmbTemp(ii));
		mcnt_pListBoard->SetItem(ii*2, EM_DSP_COL_NAME::COL_TEMP_DAI, mcls_MI.gcls_pLinksw[nIdx]->GetDieTemp(ii));
	}
	for( int ii=0; ii<NUM_CAMLINKSW * MAX_CAMLINKSW_FPGA; ii++ )
	{
		for( int jj=0; jj<MAX_CAMLINKSW_ALLINPUT; jj++ )
		{
			if( 0 == param.nCamSelBoardNo[ii][jj] ) continue;

			int nCamSelBoard =  param.nCamSelBoardNo[ii][jj]-1;
			//int nRelativeInputCh = GetDlgItemInt(txtBoard[ii][nCamSelBoard])-1;
			int nRelativeInputCh = atoi(mcnt_pListBoard->GetItem(ii*2, nCamSelBoard+ EM_DSP_COL_NAME::COL_BOARD))-1;
			int nInputCh = nRelativeInputCh + (ii/MAX_CAMLINKSW_FPGA)*MAX_CAMLINKSW_ALLINPUT ;

			//sWk.Format("%u (○)", GetDlgItemInt(txtBoard[ii][ param.nCamSelBoardNo[ii][jj]-1 ]));
			sWk.Format("%u (%d)", 
				nRelativeInputCh + 1,
				param.GetSerialSpeed(nInputCh)/1000
			
				);
			//SetDlgItemText(txtBoard[ii][ nCamSelBoard ], sWk);
			mcnt_pListBoard->SetItem(ii*2,   nCamSelBoard + EM_DSP_COL_NAME::COL_BOARD, sWk);
		}
	}

	CDialog::OnTimer(nIDEvent);
}


// これ以降 デバック用コマンド -------------------->>>


//￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣
// パラメータ変更
//＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿
void CTO_LINKSWDlg::OnBnClickedDebugParam()
{
	//// インスタンス選択
	int nIdx = m_cmbUnitExec.GetCurSel();
	if (-1 == nIdx) return;					// iniファイルの設定無し
	if( BST_CHECKED != CWnd::IsDlgButtonChecked(IDC_DEBUG_MANUAL) ) return;

	LOG(em_WAR), "[DEBUG] パラメータ変更ボタン");

	COMMON_QUE que;
	que.nEventNo = FACT_TO_LINKSW_01;				// パラメータ変更要求
	//que.nLineNo = getlineid();
	que.nLineNo = nIdx;
	que.fl.data[0] = CH_SCODE_ALL;
	send_mail(TO_LINKSW, ".", &que);
}

//￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣
// 検査開始設定
//＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿
void CTO_LINKSWDlg::OnBnClickedDebugStartSet()
{
	//// インスタンス選択
	int nIdx = m_cmbUnitExec.GetCurSel();
	if (-1 == nIdx) return;					// iniファイルの設定無し
	if( BST_CHECKED != CWnd::IsDlgButtonChecked(IDC_DEBUG_MANUAL) ) return;

	LOG(em_WAR), "[DEBUG] 検査開始設定ボタン");

	COMMON_QUE que;
	que.nEventNo = FACT_TO_LINKSW_02;				// 検査開始設定コマンド
	//que.nLineNo = getlineid();
	que.nLineNo = nIdx;
	send_mail(TO_LINKSW, ".", &que);
}

//￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣
// カメラ選択設定
//＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿
void CTO_LINKSWDlg::OnBnClickedDebugCameraSet()
{
	//// インスタンス選択
	int nIdx = m_cmbUnitExec.GetCurSel();
	if (-1 == nIdx) return;					// iniファイルの設定無し
	if( BST_CHECKED != CWnd::IsDlgButtonChecked(IDC_DEBUG_MANUAL) ) return;

	LOG(em_WAR), "[DEBUG] カメラ選択設定ボタン");

	COMMON_QUE que;
	que.nEventNo = FACT_TO_LINKSW_03;				// カメラ選択設定コマンド
	//que.nLineNo = getlineid();
	que.nLineNo = nIdx;
	que.fl.data[0] = GetDlgItemInt(IDC_DEBUG_ID);
	que.fl.data[1] = GetDlgItemInt(IDC_DEBUG_BOARD);
	que.fl.data[2] = GetDlgItemInt(IDC_DEBUG_CAMERA);
	// 2017.02.27 Master/SlaveをDB指定 ---------->>>>
	que.fl.data[3] = GetDlgItemInt(IDC_DEBUG_DIV_MS);
	// 2017.02.27 Master/SlaveをDB指定 <<<<----------
	send_mail(TO_LINKSW, ".", &que);
}

//￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣
// フレームグラバからCC送信
//＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿
void CTO_LINKSWDlg::OnBnClickedDebugCcmode()
{
	int nIdx = m_cmbUnitExec.GetCurSel();	if (-1 == nIdx) return;

	int ii_s, ii_e;
	mcls_MI.GetChLoop(nIdx, ii_s, ii_e);

	for (int ii = ii_s; ii < ii_e; ii++) {
		if (BST_CHECKED != CWnd::IsDlgButtonChecked(IDC_DEBUG_MANUAL))
		{
			if (BST_CHECKED != CWnd::IsDlgButtonChecked(IDC_DEBUG_CCMODE))
			{
				CWnd::CheckDlgButton(IDC_DEBUG_CCMODE, 1);
			}
			else
			{
				CWnd::CheckDlgButton(IDC_DEBUG_CCMODE, 0);
			}
			return;
		}

		bool bCCMode = BST_CHECKED == CWnd::IsDlgButtonChecked(IDC_DEBUG_CCMODE) ? true : false;

		LOG(em_WAR), "[DEBUG] フレームグラバからCC送信 [%s]", bCCMode ? "ON" : "OFF");

		mcls_MI.gcls_pLinksw[ii]->SetCCMode(bCCMode);
	}
}

//￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣
// 途中参加設定
//＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿
void CTO_LINKSWDlg::OnBnClickedDebugSlaveSet()
{
	//// インスタンス選択
	int nIdx = m_cmbUnitExec.GetCurSel();
	if (-1 == nIdx) return;					// iniファイルの設定無し
	if( BST_CHECKED != CWnd::IsDlgButtonChecked(IDC_DEBUG_MANUAL) ) return;

	LOG(em_WAR), "[DEBUG] 途中参加設定ボタン");

	COMMON_QUE que;
	que.nEventNo = FACT_TO_LINKSW_04;				// 途中参加設定コマンド
	//que.nLineNo = getlineid();
	que.nLineNo = nIdx;
	que.fl.data[0] = GetDlgItemInt(IDC_DEBUG_TORB);
	que.fl.data[1] = GetDlgItemInt(IDC_DEBUG_FNO);
	send_mail(TO_LINKSW, ".", &que);
}

//￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣
// 高速リセット通知
//＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿
void CTO_LINKSWDlg::OnBnClickedDebugCounterReset()
{
	//// インスタンス選択
	int nIdx = m_cmbUnitExec.GetCurSel();
	if (-1 == nIdx) return;					// iniファイルの設定無し
	if( BST_CHECKED != CWnd::IsDlgButtonChecked(IDC_DEBUG_MANUAL) ) return;

	LOG(em_WAR), "[DEBUG] 高速リセット通知ボタン");

	COMMON_QUE que;
	que.nEventNo = FACT_TO_LINKSW_05;				// 高速リセット通知
	//que.nLineNo = getlineid();
	que.nLineNo = nIdx;
	que.fl.data[0] = GetDlgItemInt(IDC_DEBUG_FNO);
	send_mail(TO_LINKSW, ".", &que);
}

//￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣
// カウンタチェック
//＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿
void CTO_LINKSWDlg::OnBnClickedDebugCounterCheck()
{
	int nIdx = m_cmbUnitExec.GetCurSel();	if (-1 == nIdx) return;

	int ii_s, ii_e;
	mcls_MI.GetChLoop(nIdx, ii_s, ii_e);

	for (int ii = ii_s; ii < ii_e; ii++) {

		if (BST_CHECKED != CWnd::IsDlgButtonChecked(IDC_DEBUG_MANUAL)) return;

		LOG(em_WAR), "[DEBUG] カウンタチェックボタン");

		// カウンタチェックイベント通知
		mcls_MI.gcls_pLinksw[ii]->SetCounterCheck();
	}
}
//￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣
// AOCシリアル速度設定
//＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿
void CTO_LINKSWDlg::OnBnClickedDebugSetSerialSpeed()
{
	//// インスタンス選択
	int nIdx = m_cmbUnitExec.GetCurSel();
	if (-1 == nIdx) return;					// iniファイルの設定無し
	if( BST_CHECKED != CWnd::IsDlgButtonChecked(IDC_DEBUG_MANUAL) ) return;

	LOG(em_WAR), "[DEBUG] AOCシリアルボタン");

	COMMON_QUE que;
	que.nEventNo = FACT_TO_LINKSW_06;				// 途中参加設定コマンド
	//que.nLineNo = getlineid();
	que.nLineNo = nIdx;
	que.fl.data[0] = GetDlgItemInt(IDC_DEBUG_CAMNO);
	que.fl.data[1] = GetDlgItemInt(IDC_DEBUG_SERIAL_SPEED_VALUE);
	send_mail(TO_LINKSW, ".", &que);
}
