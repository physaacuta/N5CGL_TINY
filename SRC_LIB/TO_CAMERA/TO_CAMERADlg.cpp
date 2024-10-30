// TO_CAMERADlg.cpp : 実装ファイル
//

#include "stdafx.h"
#include "TO_CAMERA.h"
#include "TO_CAMERADlg.h"

// メモリーリーク検出 ----->>>
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif
// <<<------------------------

// CTO_CAMERADlg ダイアログ




CTO_CAMERADlg::CTO_CAMERADlg(CWnd* pParent /*=NULL*/)
	: CDialog(CTO_CAMERADlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CTO_CAMERADlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, m_ListBox);
	DDX_Control(pDX, IDC_UNIT_DSP, m_cmbUnitDsp);
	DDX_Control(pDX, IDC_UNIT_EXEC, m_cmbUnitExec);
}

BEGIN_MESSAGE_MAP(CTO_CAMERADlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_WM_CLOSE()
	ON_WM_TIMER()
	ON_BN_CLICKED(IDOK, &CTO_CAMERADlg::OnBnClickedOk)
	ON_LBN_DBLCLK(IDC_LIST1, &CTO_CAMERADlg::OnLbnDblclkList1)
	ON_BN_CLICKED(IDC_ROUP1, &CTO_CAMERADlg::OnBnClickedRoup1)
	ON_BN_CLICKED(IDC_DEBUG_INIT, &CTO_CAMERADlg::OnBnClickedDebugInit)
	ON_BN_CLICKED(IDC_DEBUG_PARAM, &CTO_CAMERADlg::OnBnClickedDebugParam)
	ON_BN_CLICKED(IDC_DEBUG_PLSON, &CTO_CAMERADlg::OnBnClickedDebugPlson)
	ON_BN_CLICKED(IDC_DEBUG_PLSOFF, &CTO_CAMERADlg::OnBnClickedDebugPlsoff)
	ON_BN_CLICKED(IDC_DEBUG_SCODE_T, &CTO_CAMERADlg::OnBnClickedDebugScodeT)
	ON_BN_CLICKED(IDC_DEBUG_SCODE_B, &CTO_CAMERADlg::OnBnClickedDebugScodeB)
	ON_BN_CLICKED(IDC_DEBUG_AGC, &CTO_CAMERADlg::OnBnClickedDebugAgc)
	ON_BN_CLICKED(IDC_ROUP2, &CTO_CAMERADlg::OnBnClickedRoup2)
	ON_BN_CLICKED(IDC_ROUP3, &CTO_CAMERADlg::OnBnClickedRoup3)
	ON_BN_CLICKED(IDC_ROUP4, &CTO_CAMERADlg::OnBnClickedRoup4)
	ON_BN_CLICKED(IDC_ROUP5, &CTO_CAMERADlg::OnBnClickedRoup5)
	ON_BN_CLICKED(IDC_ROUP6, &CTO_CAMERADlg::OnBnClickedRoup6)
	ON_BN_CLICKED(IDC_ROUP7, &CTO_CAMERADlg::OnBnClickedRoup7)
	ON_BN_CLICKED(IDC_ROUP8, &CTO_CAMERADlg::OnBnClickedRoup8)
	ON_BN_CLICKED(IDC_ROUP9, &CTO_CAMERADlg::OnBnClickedRoup9)
	ON_BN_CLICKED(IDC_ROUP10, &CTO_CAMERADlg::OnBnClickedRoup10)
	ON_BN_CLICKED(IDC_ROUP11, &CTO_CAMERADlg::OnBnClickedRoup11)
	ON_BN_CLICKED(IDC_ROUP12, &CTO_CAMERADlg::OnBnClickedRoup12)
	ON_BN_CLICKED(IDC_ROUP13, &CTO_CAMERADlg::OnBnClickedRoup13)
	ON_BN_CLICKED(IDC_ROUP14, &CTO_CAMERADlg::OnBnClickedRoup14)
	ON_BN_CLICKED(IDC_ROUP15, &CTO_CAMERADlg::OnBnClickedRoup15)
	ON_BN_CLICKED(IDC_ROUP16, &CTO_CAMERADlg::OnBnClickedRoup16)
	ON_BN_CLICKED(IDC_RODOWN1, &CTO_CAMERADlg::OnBnClickedRodown1)
	ON_BN_CLICKED(IDC_RODOWN2, &CTO_CAMERADlg::OnBnClickedRodown2)
	ON_BN_CLICKED(IDC_RODOWN3, &CTO_CAMERADlg::OnBnClickedRodown3)
	ON_BN_CLICKED(IDC_RODOWN4, &CTO_CAMERADlg::OnBnClickedRodown4)
	ON_BN_CLICKED(IDC_RODOWN5, &CTO_CAMERADlg::OnBnClickedRodown5)
	ON_BN_CLICKED(IDC_RODOWN6, &CTO_CAMERADlg::OnBnClickedRodown6)
	ON_BN_CLICKED(IDC_RODOWN7, &CTO_CAMERADlg::OnBnClickedRodown7)
	ON_BN_CLICKED(IDC_RODOWN8, &CTO_CAMERADlg::OnBnClickedRodown8)
	ON_BN_CLICKED(IDC_RODOWN9, &CTO_CAMERADlg::OnBnClickedRodown9)
	ON_BN_CLICKED(IDC_RODOWN10, &CTO_CAMERADlg::OnBnClickedRodown10)
	ON_BN_CLICKED(IDC_RODOWN11, &CTO_CAMERADlg::OnBnClickedRodown11)
	ON_BN_CLICKED(IDC_RODOWN12, &CTO_CAMERADlg::OnBnClickedRodown12)
	ON_BN_CLICKED(IDC_RODOWN13, &CTO_CAMERADlg::OnBnClickedRodown13)
	ON_BN_CLICKED(IDC_RODOWN14, &CTO_CAMERADlg::OnBnClickedRodown14)
	ON_BN_CLICKED(IDC_RODOWN15, &CTO_CAMERADlg::OnBnClickedRodown15)
	ON_BN_CLICKED(IDC_RODOWN16, &CTO_CAMERADlg::OnBnClickedRodown16)
	ON_BN_CLICKED(IDC_ROUP17, &CTO_CAMERADlg::OnBnClickedRoup17)
	ON_BN_CLICKED(IDC_ROUP18, &CTO_CAMERADlg::OnBnClickedRoup18)
	ON_BN_CLICKED(IDC_ROUP19, &CTO_CAMERADlg::OnBnClickedRoup19)
	ON_BN_CLICKED(IDC_ROUP20, &CTO_CAMERADlg::OnBnClickedRoup20)
	ON_BN_CLICKED(IDC_ROUP21, &CTO_CAMERADlg::OnBnClickedRoup21)
	ON_BN_CLICKED(IDC_ROUP22, &CTO_CAMERADlg::OnBnClickedRoup22)
	ON_BN_CLICKED(IDC_ROUP23, &CTO_CAMERADlg::OnBnClickedRoup23)
	ON_BN_CLICKED(IDC_ROUP24, &CTO_CAMERADlg::OnBnClickedRoup24)
	ON_BN_CLICKED(IDC_ROUP25, &CTO_CAMERADlg::OnBnClickedRoup25)
	ON_BN_CLICKED(IDC_ROUP26, &CTO_CAMERADlg::OnBnClickedRoup26)
	ON_BN_CLICKED(IDC_ROUP27, &CTO_CAMERADlg::OnBnClickedRoup27)
	ON_BN_CLICKED(IDC_ROUP28, &CTO_CAMERADlg::OnBnClickedRoup28)
	ON_BN_CLICKED(IDC_ROUP29, &CTO_CAMERADlg::OnBnClickedRoup29)
	ON_BN_CLICKED(IDC_ROUP30, &CTO_CAMERADlg::OnBnClickedRoup30)
	ON_BN_CLICKED(IDC_ROUP31, &CTO_CAMERADlg::OnBnClickedRoup31)
	ON_BN_CLICKED(IDC_ROUP32, &CTO_CAMERADlg::OnBnClickedRoup32)
	ON_BN_CLICKED(IDC_RODOWN17, &CTO_CAMERADlg::OnBnClickedRodown17)
	ON_BN_CLICKED(IDC_RODOWN18, &CTO_CAMERADlg::OnBnClickedRodown18)
	ON_BN_CLICKED(IDC_RODOWN19, &CTO_CAMERADlg::OnBnClickedRodown19)
	ON_BN_CLICKED(IDC_RODOWN20, &CTO_CAMERADlg::OnBnClickedRodown20)
	ON_BN_CLICKED(IDC_RODOWN21, &CTO_CAMERADlg::OnBnClickedRodown21)
	ON_BN_CLICKED(IDC_RODOWN22, &CTO_CAMERADlg::OnBnClickedRodown22)
	ON_BN_CLICKED(IDC_RODOWN23, &CTO_CAMERADlg::OnBnClickedRodown23)
	ON_BN_CLICKED(IDC_RODOWN24, &CTO_CAMERADlg::OnBnClickedRodown24)
	ON_BN_CLICKED(IDC_RODOWN25, &CTO_CAMERADlg::OnBnClickedRodown25)
	ON_BN_CLICKED(IDC_RODOWN26, &CTO_CAMERADlg::OnBnClickedRodown26)
	ON_BN_CLICKED(IDC_RODOWN27, &CTO_CAMERADlg::OnBnClickedRodown27)
	ON_BN_CLICKED(IDC_RODOWN28, &CTO_CAMERADlg::OnBnClickedRodown28)
	ON_BN_CLICKED(IDC_RODOWN29, &CTO_CAMERADlg::OnBnClickedRodown29)
	ON_BN_CLICKED(IDC_RODOWN30, &CTO_CAMERADlg::OnBnClickedRodown30)
	ON_BN_CLICKED(IDC_RODOWN31, &CTO_CAMERADlg::OnBnClickedRodown31)
	ON_BN_CLICKED(IDC_RODOWN32, &CTO_CAMERADlg::OnBnClickedRodown32)
	ON_BN_CLICKED(IDC_CMD_SEND, &CTO_CAMERADlg::OnBnClickedCmdSend)
	ON_BN_CLICKED(IDC_DEBUG_CHECK_INIT, &CTO_CAMERADlg::OnBnClickedDebugCheckInit)
END_MESSAGE_MAP()


// CTO_CAMERADlg メッセージ ハンドラ

BOOL CTO_CAMERADlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// このダイアログのアイコンを設定します。アプリケーションのメイン ウィンドウがダイアログでない場合、
	//  Framework は、この設定を自動的に行います。
	SetIcon(m_hIcon, TRUE);			// 大きいアイコンの設定
	SetIcon(m_hIcon, FALSE);		// 小さいアイコンの設定

	// TODO: 初期化をここに追加します。

	// 画面初期化
	CWnd::CheckRadioButton(IDC_RAD_MODE1, IDC_RAD_MODE4, IDC_RAD_MODE1);
	SetDlgItemInt(IDC_SCODESET_T, MAX_SCODE);
	SetDlgItemInt(IDC_SCODESET_B, MAX_SCODE);
	SetDlgItemInt(IDC_ROSET1, 0);
	SetDlgItemInt(IDC_ROSET2, 0);
	SetDlgItemInt(IDC_ROSET3, 0);
	SetDlgItemInt(IDC_ROSET4, 0);
	SetDlgItemInt(IDC_ROSET5, 0);
	SetDlgItemInt(IDC_ROSET6, 0);
	SetDlgItemInt(IDC_ROSET7, 0);
	SetDlgItemInt(IDC_ROSET8, 0);
	SetDlgItemInt(IDC_ROSET9, 0);
	SetDlgItemInt(IDC_ROSET10, 0);
	SetDlgItemInt(IDC_ROSET11, 0);
	SetDlgItemInt(IDC_ROSET12, 0);
	SetDlgItemInt(IDC_ROSET13, 0);
	SetDlgItemInt(IDC_ROSET14, 0);
	SetDlgItemInt(IDC_ROSET15, 0);
	SetDlgItemInt(IDC_ROSET16, 0);

	// 画面位置 (サイズはいまのまま)
	::SetWindowPos(m_hWnd, NULL, 0, 550, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
	
// ------------------------------->>>
	// 実行環境判別用
	CString		cTitle;
	GetWindowText(cTitle);

#ifdef VER_AVALDATA
	cTitle = "<ｶﾒﾗﾘﾝｸｽｲｯﾁ動作>" + cTitle;
#endif


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

void CTO_CAMERADlg::OnPaint()
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
HCURSOR CTO_CAMERADlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

// これ以降 手動追加 -------------------->>>

//￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣
// ×ボタン
//＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿
void CTO_CAMERADlg::OnClose()
{
	//CDialog::OnClose();
}

//￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣
// 閉じるボタン
//＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿
void CTO_CAMERADlg::OnBnClickedOk()
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
void CTO_CAMERADlg::OnLbnDblclkList1()
{
	// ListBoxコントロールのプロパティは、ソート禁止にしておく事
	// ListBoxコントロールのダブルクリックイベントを追加
	mcls_pLog->ShowEdit();
}

//------------------------------------------
// 各プロセス初期化処理
//------------------------------------------
void CTO_CAMERADlg::InitProcess()
{
	// タスク初期化処理
	setlineid(LINE_ID);
	if( 0 != task_init(TO_CAMERA) ) {
		syslog(SYSNO_TASK_INIT_ERR, "[%s]", TO_CAMERA);
		_ASSERT(0);
		exit(-9);		// 自プロセス強制終了
		return;
	}

	// LogFileManagerインスタンス生成 (これ以降 ログ出力可能)
	setlocale(LC_ALL, "Japanese");	// これが無いと日本語パスを含むフォルダの場合、保存できない。(ライブラリの中で実行するべきか迷っているが、変更するとタスク全体に影響するので、とりあえずここで設定)
	mcls_pLog = new LogFileManager(getmytaskname(), &m_ListBox);
	LOG(em_MSG), "タスク起動");
	LOG(em_MSG), "各プロセス開始処理 実行中・・・");

	// メインインスタンス 開始処理
	mcls_MI.SetLogMgr(mcls_pLog);
	mcls_MI.Init();
	

	//// 画面更新用データセット
	// コンボボックスにセッション名追加
	m_cmbUnitExec.AddString( "全ユニット" );
	for(int ii=0; ii<MAX_THREAD; ii++) {
		////// ？？ なぜかReleaseの場合だけ 文字列に _ が入っていると登録失敗する
		if( NULL == mcls_MI.gcls_pCam[ii] ) break;
		m_cmbUnitDsp.AddString( mcls_MI.gcls_pCam[ii]->GetMySession() );
		m_cmbUnitExec.AddString( mcls_MI.gcls_pCam[ii]->GetMySession() );
	}
	m_cmbUnitDsp.SetCurSel(0);				// 0番目を初期選択とする
	m_cmbUnitExec.SetCurSel(0);	

	//// 画面更新用タイマ生成
	SetTimer(1, 500, NULL);

	// メインスレッドの実行を行う
	LOG(em_MSG), "各プロセス開始処理 完了");
	mcls_MI.Start();	
	return;
}

//￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣
// 各プロセス開放処理
//＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿
void CTO_CAMERADlg::ExitProcess()
{
	//// ログ リストボックス表示処理終了
	mcls_pLog->SetListOut(false);					// これ以降 リストボックスへのログ表示禁止

	// メインインスタンス 終了処理
	KillTimer(1);			// 画面更新タイマ
	mcls_MI.Stop();
	mcls_MI.Exit();

	//// ログクラス開放
	LOG(em_MSG), "タスク終了");
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
unsigned WINAPI CTO_CAMERADlg::MainThread(void* param)
{
	CTO_CAMERADlg* p = (CTO_CAMERADlg *)param;
	COMMON_QUE que;			// 受信構造体

	//// 各プロセス初期化処理 (これ以前はログ出力禁止)
	p->InitProcess();

	//// 起動要因取り込み ループ開始
	while(true) {
		recv_mail(&que);			// 起動要因取り込み

		switch(que.nEventNo) {
//-----------------------------------------------------------------------------------------------
		case FACT_TO_CAMERA_01:								// パラメータ変更要求 (←TO_DEFECT)
			pLOG(em_MSG), "[mailslot] パラメータ変更要求 (FACT_TO_CAMERA_01) [ﾕﾆｯﾄ=%d] [検査表面状態=%d]", que.nLineNo, que.fl.data[0]); 
			
			p->CommandSender(&que);
			break;

//-----------------------------------------------------------------------------------------------
		case FACT_TO_CAMERA_02:								// 検査用設定コマンド (←TO_DEFECT)
			pLOG(em_MSG), "[mailslot] 検査用設定コマンド (FACT_TO_CAMERA_02) [ﾕﾆｯﾄ=%d] [PLG区分=%d]", que.nLineNo, que.fl.data[0]); 

			p->CommandSender(&que);

			// 画面に反映
			if		( DIV_PLG_LINE == que.fl.data[0] ) { p->CWnd::CheckRadioButton(IDC_RAD_MODE1, IDC_RAD_MODE4, IDC_RAD_MODE1); }
			else if ( DIV_PLG_TOP  == que.fl.data[0] ) { p->CWnd::CheckRadioButton(IDC_RAD_MODE1, IDC_RAD_MODE4, IDC_RAD_MODE2); }
			else if ( DIV_PLG_BOT  == que.fl.data[0] ) { p->CWnd::CheckRadioButton(IDC_RAD_MODE1, IDC_RAD_MODE4, IDC_RAD_MODE3); }
			else if ( DIV_PLG_TEST == que.fl.data[0] ) { p->CWnd::CheckRadioButton(IDC_RAD_MODE1, IDC_RAD_MODE4, IDC_RAD_MODE4); }
			break;

//-----------------------------------------------------------------------------------------------
		case FACT_TO_CAMERA_03:								// パルス出力設定 (←TO_DEFECT)
			pLOG(em_MSG), "[mailslot] パルス出力設定 (FACT_TO_CAMERA_03) [ﾕﾆｯﾄ=%d] [%s]", que.nLineNo, (que.fl.data[0] == 0 ? "ON" : "OFF") ); 
			
			p->CommandSender(&que);
			break;

//-----------------------------------------------------------------------------------------------
		case FACT_TO_CAMERA_04:								// AGC設定 (←TO_DEFECT)
			if( -1 == que.fl.data[0] ) {	// 全カメラ指定
				pLOG(em_MSG), "[mailslot] AGC設定 (FACT_TO_CAMERA_04) [ﾕﾆｯﾄ=%d] [ｶﾒﾗ番号=%d ﾓｰﾄﾞ=%d] [%d,%d,%d,%d][%d,%d,%d,%d] [%d,%d,%d,%d][%d,%d,%d,%d]",
					que.nLineNo, que.fl.data[0]+1,  que.fl.data[1],
					que.fl.data[2],  que.fl.data[3],  que.fl.data[4],  que.fl.data[5],  que.fl.data[6],  que.fl.data[7],  que.fl.data[8],  que.fl.data[9],
					que.fl.data[10], que.fl.data[11], que.fl.data[12], que.fl.data[13], que.fl.data[14], que.fl.data[15], que.fl.data[16], que.fl.data[17] ); 
			} else {
//				pLOG(em_MSG), "[mailslot] AGC設定 (FACT_TO_CAMERA_04) [ｶﾒﾗ番号=%d ﾓｰﾄﾞ=%d] [%d]", que.fl.data[0]+1, que.fl.data[1], que.fl.data[2]); 
			}

			p->CommandSender(&que);
			break;

//-----------------------------------------------------------------------------------------------
		case FACT_TO_CAMERA_05:								// コイル切替 (←TO_DEFECT)
			pLOG(em_MSG), "[mailslot] コイル切替 (FACT_TO_CAMERA_05) [ﾕﾆｯﾄ=%d] [検査表面状態=%d 表裏区分=%d]", que.nLineNo, que.fl.data[0], que.fl.data[1]); 
			
			p->CommandSender(&que);
			break;

//-----------------------------------------------------------------------------------------------
		case FACT_TO_CAMERA_06:								// 現露光時間問い合わせ (←TO_DEFECT)
			pLOG(em_MSG), "[mailslot] 現露光時間問い合わせ (FACT_TO_CAMERA_06) [ﾕﾆｯﾄ=%d]", que.nLineNo); 
			
			p->CommandSender(&que);
			break;

//-----------------------------------------------------------------------------------------------
		case FACT_TO_CAMERA_07:								// 校正用設定コマンド (←TO_DEFECT)
			pLOG(em_MSG), "[mailslot] 校正用設定コマンド (FACT_TO_CAMERA_07) [テストパルス周期=%d(μs) 露光時間=%d(μs)]", que.fl.data[0], que.fl.data[1]);

			p->CommandSender(&que);
			break;

//-----------------------------------------------------------------------------------------------
		case FACT_TO_CAMERA_09:								// 任意コマンド	
			pLOG(em_MSG), "[mailslot] 任意コマンド送信 (FACT_TO_CAMERA_09) [ﾕﾆｯﾄ=%d][%s]", que.nLineNo, que.fc.data); 
			
			p->CommandSender(&que);
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

//------------------------------------------
// コマンド処理開始要求
// COMMON_QUE* que
//------------------------------------------
void CTO_CAMERADlg::CommandSender(COMMON_QUE* que)
{
	int ii_s, ii_e;
	mcls_MI.GetChLoop(que->nLineNo, ii_s, ii_e);

	for(int ii=ii_s; ii<ii_e; ii++) {
		COMMON_QUE *pCq = new COMMON_QUE;
		memcpy(pCq, que, sizeof(COMMON_QUE));

		mcls_MI.gcls_pCam[ii]->gque_Deli.AddToTail(pCq); 
	}
}


//￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣
// 画面更新タイマ
//＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿
void CTO_CAMERADlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: ここにメッセージ ハンドラ コードを追加するか、既定の処理を呼び出します。
	CString strVal;	
	int ii, jj;


	//// インスタンス選択
	int nIdx = m_cmbUnitDsp.GetCurSel();
	if( -1 == nIdx) return;					// iniファイルの設定無し
	CameraManager*			pCls  = mcls_MI.gcls_pCam[nIdx];
	CameraManager::PARAM	param = pCls->GetParamInf();			// パラメータ

	int scode[2];	// 現在の表面状態
	scode[0] = pCls->GetNowScode(0);
	scode[1] = pCls->GetNowScode(1);


	// パラメータ
	SetDlgItemInt(IDC_PARAM_LT, param.nExSyncLT);
	SetDlgItemInt(IDC_PARAM_TEST, param.nPlgTestInterval);

	int nIndex = pCls->GetPlgMode() -1;
	SetDlgItemInt(IDC_PARAM_MUL, param.nPlgMul[nIndex]);
	SetDlgItemInt(IDC_PARAM_DIV, param.nPlgDiv[nIndex]);
	SetDlgItemText(IDC_PARAM_PULSE, (0==param.nPlgPulse[nIndex]  ? "差動":"TTL"));
	SetDlgItemText(IDC_PARAM_ISOU,  (0==param.nPlgIsou[nIndex]   ? "二相":"単相"));
	SetDlgItemText(IDC_PARAM_ISOUSA,(0==param.nPlgIsouSa[nIndex] ? "ｱｯﾌﾟｶｳﾝﾄ":"ﾀﾞｳﾝｶｳﾝﾄ"));


	// 各カメラ設定
	int txtCamSet[]   = {IDC_CAMSET1, IDC_CAMSET2, IDC_CAMSET3, IDC_CAMSET4, IDC_CAMSET5, IDC_CAMSET6, IDC_CAMSET7, IDC_CAMSET8,
					     IDC_CAMSET9, IDC_CAMSET10,IDC_CAMSET11,IDC_CAMSET12,IDC_CAMSET13,IDC_CAMSET14,IDC_CAMSET15,IDC_CAMSET16,
						 IDC_CAMSET17, IDC_CAMSET18, IDC_CAMSET19, IDC_CAMSET20, IDC_CAMSET21, IDC_CAMSET22, IDC_CAMSET23, IDC_CAMSET24};
	int txtCamAngle[] = {IDC_CAMANGLE1, IDC_CAMANGLE2, IDC_CAMANGLE3, IDC_CAMANGLE4, IDC_CAMANGLE5, IDC_CAMANGLE6, IDC_CAMANGLE7, IDC_CAMANGLE8,
					     IDC_CAMANGLE9, IDC_CAMANGLE10,IDC_CAMANGLE11,IDC_CAMANGLE12,IDC_CAMANGLE13,IDC_CAMANGLE14,IDC_CAMANGLE15,IDC_CAMANGLE16,
						 IDC_CAMANGLE17, IDC_CAMANGLE18, IDC_CAMANGLE19, IDC_CAMANGLE20, IDC_CAMANGLE21, IDC_CAMANGLE22, IDC_CAMANGLE23, IDC_CAMANGLE24,
						 IDC_CAMANGLE25, IDC_CAMANGLE26, IDC_CAMANGLE27, IDC_CAMANGLE28, IDC_CAMANGLE29, IDC_CAMANGLE30, IDC_CAMANGLE31, IDC_CAMANGLE32};

	int txtRoDisp[] = {IDC_RODISP1, IDC_RODISP2, IDC_RODISP3, IDC_RODISP4, IDC_RODISP5, IDC_RODISP6, IDC_RODISP7, IDC_RODISP8,
					   IDC_RODISP9, IDC_RODISP10,IDC_RODISP11,IDC_RODISP12,IDC_RODISP13,IDC_RODISP14,IDC_RODISP15,IDC_RODISP16,
					   IDC_RODISP17, IDC_RODISP18, IDC_RODISP19, IDC_RODISP20, IDC_RODISP21, IDC_RODISP22, IDC_RODISP23, IDC_RODISP24,
					   IDC_RODISP25, IDC_RODISP26, IDC_RODISP27, IDC_RODISP28, IDC_RODISP29, IDC_RODISP30, IDC_RODISP31, IDC_RODISP32};
	int txtExpIni[] = {IDC_EXPINI1, IDC_EXPINI2, IDC_EXPINI3, IDC_EXPINI4, IDC_EXPINI5, IDC_EXPINI6, IDC_EXPINI7, IDC_EXPINI8,
					   IDC_EXPINI9, IDC_EXPINI10,IDC_EXPINI11,IDC_EXPINI12,IDC_EXPINI13,IDC_EXPINI14,IDC_EXPINI15,IDC_EXPINI16,
					   IDC_EXPINI17, IDC_EXPINI18, IDC_EXPINI19, IDC_EXPINI20, IDC_EXPINI21, IDC_EXPINI22, IDC_EXPINI23, IDC_EXPINI24,
					   IDC_EXPINI25, IDC_EXPINI26, IDC_EXPINI27, IDC_EXPINI28, IDC_EXPINI29, IDC_EXPINI30, IDC_EXPINI31, IDC_EXPINI32};
/*/
	for(ii=0; ii<MAX_CAMSET; ii++ ) {
		for(jj=0; jj<MAX_PHYSICAL_CAMANGLE; jj++) {
			int rec = (MAX_PHYSICAL_CAMANGLE*ii) + jj;
			if(32 <= rec) break;

			SetDlgItemInt(txtCamSet[rec],   ii+1);
			SetDlgItemInt(txtCamAngle[rec], jj+1);

			SetDlgItemInt(txtRoDisp[rec], pCls->GetExpTime(rec) );
			SetDlgItemInt(txtExpIni[rec], param.nInitialExpTime[ scode[NUM_TorB(rec)]-1 ][rec] );
		}
	}
//*/
	for(ii=0; ii<pCls->GetMyCamNum(); ii++ ) {
		int camid = pCls->GetMyCamIdStart() + ii;

		SetDlgItemInt(txtCamSet[ii],   NUM_CamSet(camid)+1);
		SetDlgItemInt(txtCamAngle[ii], NUM_CamAngle(camid)+1);

		SetDlgItemInt(txtRoDisp[ii], pCls->GetExpTime(camid) );
		SetDlgItemInt(txtExpIni[ii], param.nInitialExpTime[ scode[NUM_TorB(camid)]-1 ][camid] );
	}


	// 上下限範囲
	SetDlgItemInt(IDC_EXPUP1, param.nExSyncLT - param.nExSyncMinL );
	SetDlgItemInt(IDC_EXPLOW1, param.nExSyncMinH );

	// ステップサイズ
#ifndef VER_IDULE_CAMERA
	SetDlgItemInt(IDC_AGCSTEP_T, (int)(param.dAGCstep[scode[0]-1]*100) );
	SetDlgItemInt(IDC_AGCSTEP_B, (int)(param.dAGCstep[scode[1]-1]*100) );
#else
	SetDlgItemInt(IDC_AGCSTEP_T, (int)(param.dAGCstep[scode[0]-1][0]*100) );
	SetDlgItemInt(IDC_AGCSTEP_B, (int)(param.dAGCstep[scode[1]-1][0]*100) );
#endif

	// 現状態
	SetDlgItemText(IDC_PLS,  (pCls->IsPulsu() ? "○" : "×"));
	switch( pCls->GetPlgMode() ) {
		case DIV_PLG_LINE:
			SetDlgItemText(IDC_PLGMODE, "ラインPLG");
			break;
		case DIV_PLG_TOP:
			SetDlgItemText(IDC_PLGMODE, "表校正");
			break;
		case DIV_PLG_BOT:
			SetDlgItemText(IDC_PLGMODE, "裏校正");
			break;	
		case DIV_PLG_TEST:
			SetDlgItemText(IDC_PLGMODE, "テストパルス");
			break;
	}

	SetDlgItemInt(IDC_SCODEDISP_T, scode[0] );
	SetDlgItemInt(IDC_SCODEDISP_B, scode[1] );


	CDialog::OnTimer(nIDEvent);
}


// これ以降 デバック用コマンド -------------------->>>


//￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣
// パラメータ更新
//＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿
void CTO_CAMERADlg::OnBnClickedDebugParam()
{
	if ( BST_CHECKED != CWnd::IsDlgButtonChecked(IDC_DEBUG_MANUAL) ) return;
	int nIdx = m_cmbUnitExec.GetCurSel();	if( -1 == nIdx) return;	

	COMMON_QUE que;
	que.nEventNo = FACT_TO_CAMERA_01;				// パラメータ変更要求
	que.nLineNo = nIdx;
	que.fl.data[0] = CH_SCODE_ALL;					// 全検査表面状態変更
	send_mail(TO_CAMERA, ".", &que);
}

//￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣
// 初期化
//＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿
void CTO_CAMERADlg::OnBnClickedDebugInit()
{
	if ( BST_CHECKED != CWnd::IsDlgButtonChecked(IDC_DEBUG_MANUAL) ) return;
	int nIdx = m_cmbUnitExec.GetCurSel();	if( -1 == nIdx) return;	

	COMMON_QUE que;
	que.nEventNo = FACT_TO_CAMERA_02;				// 検査用設定コマンド
	que.nLineNo = nIdx;
	// PLG区分
	if		( BST_CHECKED == CWnd::IsDlgButtonChecked( IDC_RAD_MODE1) ) { que.fl.data[0] = DIV_PLG_LINE; }
	else if	( BST_CHECKED == CWnd::IsDlgButtonChecked( IDC_RAD_MODE2) ) { que.fl.data[0] = DIV_PLG_TOP; }	
	else if	( BST_CHECKED == CWnd::IsDlgButtonChecked( IDC_RAD_MODE3) ) { que.fl.data[0] = DIV_PLG_BOT; }	
	else if	( BST_CHECKED == CWnd::IsDlgButtonChecked( IDC_RAD_MODE4) ) { que.fl.data[0] = DIV_PLG_TEST; }	

	send_mail(TO_CAMERA, ".", &que);
}

//￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣
// 校正初期化
//＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿
void CTO_CAMERADlg::OnBnClickedDebugCheckInit()
{
	if (BST_CHECKED != CWnd::IsDlgButtonChecked(IDC_DEBUG_MANUAL)) return;
	int nIdx = m_cmbUnitExec.GetCurSel();	if (-1 == nIdx) return;

	COMMON_QUE que;
	que.nEventNo = FACT_TO_CAMERA_07;				// 校正初期化コマンド
	que.nLineNo = nIdx;
	// PLG区分
	que.fl.data[0] = GetDlgItemInt(IDC_INETRVAL);
	que.fl.data[1] = GetDlgItemInt(IDC_EXPTIME);

	send_mail(TO_CAMERA, ".", &que);
}

//￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣
// パルス出力ON
//＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿
void CTO_CAMERADlg::OnBnClickedDebugPlson()
{
	if ( BST_CHECKED != CWnd::IsDlgButtonChecked(IDC_DEBUG_MANUAL) ) return;
	int nIdx = m_cmbUnitExec.GetCurSel();	if( -1 == nIdx) return;	
	COMMON_QUE que;
	que.nEventNo = FACT_TO_CAMERA_03;				// パルス出力設定
	que.nLineNo = nIdx;
	que.fl.data[0] = 0;				
	send_mail(TO_CAMERA, ".", &que);
}
//￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣
// パルス出力OFF
//＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿
void CTO_CAMERADlg::OnBnClickedDebugPlsoff()
{
	if ( BST_CHECKED != CWnd::IsDlgButtonChecked(IDC_DEBUG_MANUAL) ) return;
	int nIdx = m_cmbUnitExec.GetCurSel();	if( -1 == nIdx) return;	
	COMMON_QUE que;
	que.nEventNo = FACT_TO_CAMERA_03;				// パルス出力設定
	que.nLineNo = nIdx;
	que.fl.data[0] = 1;				
	send_mail(TO_CAMERA, ".", &que);
}
//￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣
// 表 検査表面状態切り替え
//＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿
void CTO_CAMERADlg::OnBnClickedDebugScodeT()
{
	if ( BST_CHECKED != CWnd::IsDlgButtonChecked(IDC_DEBUG_MANUAL) ) return;
	int nIdx = m_cmbUnitExec.GetCurSel();	if( -1 == nIdx) return;	
	int val;
	val = GetDlgItemInt(IDC_SCODESET_T);	// 異常値入力でも 0 が帰るから 入力異常のチェックはしないよ
	if( 0 >= val || val > MAX_SCODE ) return;

	COMMON_QUE que;
	que.nEventNo = FACT_TO_CAMERA_05;				// コイル切替
	que.nLineNo = nIdx;
	que.fl.data[0] = val;		// 検査表面状態No
	que.fl.data[1] = 0;			// 表裏
	send_mail(TO_CAMERA, ".", &que);
}
//￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣
// 裏 検査表面状態切り替え
//＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿
void CTO_CAMERADlg::OnBnClickedDebugScodeB()
{
	if ( BST_CHECKED != CWnd::IsDlgButtonChecked(IDC_DEBUG_MANUAL) ) return;
	int nIdx = m_cmbUnitExec.GetCurSel();	if( -1 == nIdx) return;	
	int val;
	val = GetDlgItemInt(IDC_SCODESET_B);	// 異常値入力でも 0 が帰るから 入力異常のチェックはしないよ
	if( 0 >= val || val > MAX_SCODE ) return;

	COMMON_QUE que;
	que.nEventNo = FACT_TO_CAMERA_05;				// コイル切替
	que.nLineNo = nIdx;
	que.fl.data[0] = val;		// 検査表面状態No
	que.fl.data[1] = 1;			// 表裏
	send_mail(TO_CAMERA, ".", &que);
}

//￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣
// AGC設定 全指定
//＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿
void CTO_CAMERADlg::OnBnClickedDebugAgc()
{
	if ( BST_CHECKED != CWnd::IsDlgButtonChecked(IDC_DEBUG_MANUAL) ) return;
	int nIdx = m_cmbUnitExec.GetCurSel();	if( -1 == nIdx) return;	
	COMMON_QUE que;
	que.nEventNo = FACT_TO_CAMERA_04;				// AGC設定
	que.nLineNo = nIdx;
	que.fl.data[0] = -1;			// 全カメラ
	que.fl.data[1] = 1;				// 絶対値

	que.fl.data[2] = GetDlgItemInt(IDC_ROSET1);
	que.fl.data[3] = GetDlgItemInt(IDC_ROSET2);
	que.fl.data[4] = GetDlgItemInt(IDC_ROSET3);
	que.fl.data[5] = GetDlgItemInt(IDC_ROSET4);
	que.fl.data[6] = GetDlgItemInt(IDC_ROSET5);
	que.fl.data[7] = GetDlgItemInt(IDC_ROSET6);
	que.fl.data[8] = GetDlgItemInt(IDC_ROSET7);
	que.fl.data[9] = GetDlgItemInt(IDC_ROSET8);
	que.fl.data[10] = GetDlgItemInt(IDC_ROSET9);
	que.fl.data[11] = GetDlgItemInt(IDC_ROSET10);
	que.fl.data[12] = GetDlgItemInt(IDC_ROSET11);
	que.fl.data[13] = GetDlgItemInt(IDC_ROSET12);
	que.fl.data[14] = GetDlgItemInt(IDC_ROSET13);
	que.fl.data[15] = GetDlgItemInt(IDC_ROSET14);
	que.fl.data[16] = GetDlgItemInt(IDC_ROSET15);
	que.fl.data[17] = GetDlgItemInt(IDC_ROSET16);

	send_mail(TO_CAMERA, ".", &que);
}

//￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣
// AGC設定  相対上げ
//＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿
void CTO_CAMERADlg::OnBnClickedRoup1()
{
	if ( BST_CHECKED != CWnd::IsDlgButtonChecked(IDC_DEBUG_MANUAL) ) return;
	int nIdx = m_cmbUnitDsp.GetCurSel();	if( -1 == nIdx) return;					// iniファイルの設定無し
	CameraManager*			pCls  = mcls_MI.gcls_pCam[nIdx];

	COMMON_QUE que;
	que.nEventNo = FACT_TO_CAMERA_04;				// AGC設定
	que.nLineNo = nIdx+1;
	que.fl.data[0] = 0 + pCls->GetMyCamIdStart();			// 指定カメラ番号
	que.fl.data[1] = 0;			// 相対
	que.fl.data[2] = 1;
	send_mail(TO_CAMERA, ".", &que);
}
void CTO_CAMERADlg::OnBnClickedRoup2()
{
	if ( BST_CHECKED != CWnd::IsDlgButtonChecked(IDC_DEBUG_MANUAL) ) return;
	int nIdx = m_cmbUnitDsp.GetCurSel();	if( -1 == nIdx) return;					// iniファイルの設定無し
	CameraManager*			pCls  = mcls_MI.gcls_pCam[nIdx];

	COMMON_QUE que;
	que.nEventNo = FACT_TO_CAMERA_04;				// AGC設定
	que.nLineNo = nIdx+1;
	que.fl.data[0] = 1 + pCls->GetMyCamIdStart();			// 指定カメラ番号
	que.fl.data[1] = 0;			// 相対
	que.fl.data[2] = 1;
	send_mail(TO_CAMERA, ".", &que);
}
void CTO_CAMERADlg::OnBnClickedRoup3()
{
	if ( BST_CHECKED != CWnd::IsDlgButtonChecked(IDC_DEBUG_MANUAL) ) return;
	int nIdx = m_cmbUnitDsp.GetCurSel();	if( -1 == nIdx) return;					// iniファイルの設定無し
	CameraManager*			pCls  = mcls_MI.gcls_pCam[nIdx];

	COMMON_QUE que;
	que.nEventNo = FACT_TO_CAMERA_04;				// AGC設定
	que.nLineNo = nIdx+1;
	que.fl.data[0] = 2 + pCls->GetMyCamIdStart();			// 指定カメラ番号
	que.fl.data[1] = 0;			// 相対
	que.fl.data[2] = 1;
	send_mail(TO_CAMERA, ".", &que);
}
void CTO_CAMERADlg::OnBnClickedRoup4()
{
	if ( BST_CHECKED != CWnd::IsDlgButtonChecked(IDC_DEBUG_MANUAL) ) return;
	int nIdx = m_cmbUnitDsp.GetCurSel();	if( -1 == nIdx) return;					// iniファイルの設定無し
	CameraManager*			pCls  = mcls_MI.gcls_pCam[nIdx];

	COMMON_QUE que;
	que.nEventNo = FACT_TO_CAMERA_04;				// AGC設定
	que.nLineNo = nIdx+1;
	que.fl.data[0] = 3 + pCls->GetMyCamIdStart();			// 指定カメラ番号
	que.fl.data[1] = 0;			// 相対
	que.fl.data[2] = 1;
	send_mail(TO_CAMERA, ".", &que);
}
void CTO_CAMERADlg::OnBnClickedRoup5()
{
	if ( BST_CHECKED != CWnd::IsDlgButtonChecked(IDC_DEBUG_MANUAL) ) return;
	int nIdx = m_cmbUnitDsp.GetCurSel();	if( -1 == nIdx) return;					// iniファイルの設定無し
	CameraManager*			pCls  = mcls_MI.gcls_pCam[nIdx];

	COMMON_QUE que;
	que.nEventNo = FACT_TO_CAMERA_04;				// AGC設定
	que.nLineNo = nIdx+1;
	que.fl.data[0] = 4 + pCls->GetMyCamIdStart();			// 指定カメラ番号
	que.fl.data[1] = 0;			// 相対
	que.fl.data[2] = 1;
	send_mail(TO_CAMERA, ".", &que);
}
void CTO_CAMERADlg::OnBnClickedRoup6()
{
	if ( BST_CHECKED != CWnd::IsDlgButtonChecked(IDC_DEBUG_MANUAL) ) return;
	int nIdx = m_cmbUnitDsp.GetCurSel();	if( -1 == nIdx) return;					// iniファイルの設定無し
	CameraManager*			pCls  = mcls_MI.gcls_pCam[nIdx];

	COMMON_QUE que;
	que.nEventNo = FACT_TO_CAMERA_04;				// AGC設定
	que.nLineNo = nIdx+1;
	que.fl.data[0] = 5 + pCls->GetMyCamIdStart();			// 指定カメラ番号
	que.fl.data[1] = 0;			// 相対
	que.fl.data[2] = 1;
	send_mail(TO_CAMERA, ".", &que);
}
void CTO_CAMERADlg::OnBnClickedRoup7()
{
	if ( BST_CHECKED != CWnd::IsDlgButtonChecked(IDC_DEBUG_MANUAL) ) return;
	int nIdx = m_cmbUnitDsp.GetCurSel();	if( -1 == nIdx) return;					// iniファイルの設定無し
	CameraManager*			pCls  = mcls_MI.gcls_pCam[nIdx];

	COMMON_QUE que;
	que.nEventNo = FACT_TO_CAMERA_04;				// AGC設定
	que.nLineNo = nIdx+1;
	que.fl.data[0] = 6 + pCls->GetMyCamIdStart();			// 指定カメラ番号
	que.fl.data[1] = 0;			// 相対
	que.fl.data[2] = 1;
	send_mail(TO_CAMERA, ".", &que);
}
void CTO_CAMERADlg::OnBnClickedRoup8()
{
	if ( BST_CHECKED != CWnd::IsDlgButtonChecked(IDC_DEBUG_MANUAL) ) return;
	int nIdx = m_cmbUnitDsp.GetCurSel();	if( -1 == nIdx) return;					// iniファイルの設定無し
	CameraManager*			pCls  = mcls_MI.gcls_pCam[nIdx];

	COMMON_QUE que;
	que.nEventNo = FACT_TO_CAMERA_04;				// AGC設定
	que.nLineNo = nIdx+1;
	que.fl.data[0] = 7 + pCls->GetMyCamIdStart();			// 指定カメラ番号
	que.fl.data[1] = 0;			// 相対
	que.fl.data[2] = 1;
	send_mail(TO_CAMERA, ".", &que);
}
void CTO_CAMERADlg::OnBnClickedRoup9()
{
	if ( BST_CHECKED != CWnd::IsDlgButtonChecked(IDC_DEBUG_MANUAL) ) return;
	int nIdx = m_cmbUnitDsp.GetCurSel();	if( -1 == nIdx) return;					// iniファイルの設定無し
	CameraManager*			pCls  = mcls_MI.gcls_pCam[nIdx];

	COMMON_QUE que;
	que.nEventNo = FACT_TO_CAMERA_04;				// AGC設定
	que.nLineNo = nIdx+1;
	que.fl.data[0] = 8 + pCls->GetMyCamIdStart();			// 指定カメラ番号
	que.fl.data[1] = 0;			// 相対
	que.fl.data[2] = 1;
	send_mail(TO_CAMERA, ".", &que);
}
void CTO_CAMERADlg::OnBnClickedRoup10()
{
	if ( BST_CHECKED != CWnd::IsDlgButtonChecked(IDC_DEBUG_MANUAL) ) return;
	int nIdx = m_cmbUnitDsp.GetCurSel();	if( -1 == nIdx) return;					// iniファイルの設定無し
	CameraManager*			pCls  = mcls_MI.gcls_pCam[nIdx];

	COMMON_QUE que;
	que.nEventNo = FACT_TO_CAMERA_04;				// AGC設定
	que.nLineNo = nIdx+1;
	que.fl.data[0] = 9 + pCls->GetMyCamIdStart();			// 指定カメラ番号
	que.fl.data[1] = 0;			// 相対
	que.fl.data[2] = 1;
	send_mail(TO_CAMERA, ".", &que);
}
void CTO_CAMERADlg::OnBnClickedRoup11()
{
	if ( BST_CHECKED != CWnd::IsDlgButtonChecked(IDC_DEBUG_MANUAL) ) return;
	int nIdx = m_cmbUnitDsp.GetCurSel();	if( -1 == nIdx) return;					// iniファイルの設定無し
	CameraManager*			pCls  = mcls_MI.gcls_pCam[nIdx];

	COMMON_QUE que;
	que.nEventNo = FACT_TO_CAMERA_04;				// AGC設定
	que.nLineNo = nIdx+1;
	que.fl.data[0] = 10 + pCls->GetMyCamIdStart();			// 指定カメラ番号
	que.fl.data[1] = 0;			// 相対
	que.fl.data[2] = 1;
	send_mail(TO_CAMERA, ".", &que);
}
void CTO_CAMERADlg::OnBnClickedRoup12()
{
	if ( BST_CHECKED != CWnd::IsDlgButtonChecked(IDC_DEBUG_MANUAL) ) return;
	int nIdx = m_cmbUnitDsp.GetCurSel();	if( -1 == nIdx) return;					// iniファイルの設定無し
	CameraManager*			pCls  = mcls_MI.gcls_pCam[nIdx];

	COMMON_QUE que;
	que.nEventNo = FACT_TO_CAMERA_04;				// AGC設定
	que.nLineNo = nIdx+1;
	que.fl.data[0] = 11 + pCls->GetMyCamIdStart();			// 指定カメラ番号
	que.fl.data[1] = 0;			// 相対
	que.fl.data[2] = 1;
	send_mail(TO_CAMERA, ".", &que);
}
void CTO_CAMERADlg::OnBnClickedRoup13()
{
	if ( BST_CHECKED != CWnd::IsDlgButtonChecked(IDC_DEBUG_MANUAL) ) return;
	int nIdx = m_cmbUnitDsp.GetCurSel();	if( -1 == nIdx) return;					// iniファイルの設定無し
	CameraManager*			pCls  = mcls_MI.gcls_pCam[nIdx];

	COMMON_QUE que;
	que.nEventNo = FACT_TO_CAMERA_04;				// AGC設定
	que.nLineNo = nIdx+1;
	que.fl.data[0] = 12 + pCls->GetMyCamIdStart();			// 指定カメラ番号
	que.fl.data[1] = 0;			// 相対
	que.fl.data[2] = 1;
	send_mail(TO_CAMERA, ".", &que);
}
void CTO_CAMERADlg::OnBnClickedRoup14()
{
	if ( BST_CHECKED != CWnd::IsDlgButtonChecked(IDC_DEBUG_MANUAL) ) return;
	int nIdx = m_cmbUnitDsp.GetCurSel();	if( -1 == nIdx) return;					// iniファイルの設定無し
	CameraManager*			pCls  = mcls_MI.gcls_pCam[nIdx];

	COMMON_QUE que;
	que.nEventNo = FACT_TO_CAMERA_04;				// AGC設定
	que.nLineNo = nIdx+1;
	que.fl.data[0] = 13 + pCls->GetMyCamIdStart();			// 指定カメラ番号
	que.fl.data[1] = 0;			// 相対
	que.fl.data[2] = 1;
	send_mail(TO_CAMERA, ".", &que);
}
void CTO_CAMERADlg::OnBnClickedRoup15()
{
	if ( BST_CHECKED != CWnd::IsDlgButtonChecked(IDC_DEBUG_MANUAL) ) return;
	int nIdx = m_cmbUnitDsp.GetCurSel();	if( -1 == nIdx) return;					// iniファイルの設定無し
	CameraManager*			pCls  = mcls_MI.gcls_pCam[nIdx];

	COMMON_QUE que;
	que.nEventNo = FACT_TO_CAMERA_04;				// AGC設定
	que.nLineNo = nIdx+1;
	que.fl.data[0] = 14 + pCls->GetMyCamIdStart();			// 指定カメラ番号
	que.fl.data[1] = 0;			// 相対
	que.fl.data[2] = 1;
	send_mail(TO_CAMERA, ".", &que);
}
void CTO_CAMERADlg::OnBnClickedRoup16()
{
	if ( BST_CHECKED != CWnd::IsDlgButtonChecked(IDC_DEBUG_MANUAL) ) return;
	int nIdx = m_cmbUnitDsp.GetCurSel();	if( -1 == nIdx) return;					// iniファイルの設定無し
	CameraManager*			pCls  = mcls_MI.gcls_pCam[nIdx];

	COMMON_QUE que;
	que.nEventNo = FACT_TO_CAMERA_04;				// AGC設定
	que.nLineNo = nIdx+1;
	que.fl.data[0] = 15 + pCls->GetMyCamIdStart();			// 指定カメラ番号
	que.fl.data[1] = 0;			// 相対
	que.fl.data[2] = 1;
	send_mail(TO_CAMERA, ".", &que);
}
void CTO_CAMERADlg::OnBnClickedRoup17()
{
	if ( BST_CHECKED != CWnd::IsDlgButtonChecked(IDC_DEBUG_MANUAL) ) return;
	int nIdx = m_cmbUnitDsp.GetCurSel();	if( -1 == nIdx) return;					// iniファイルの設定無し
	CameraManager*			pCls  = mcls_MI.gcls_pCam[nIdx];

	COMMON_QUE que;
	que.nEventNo = FACT_TO_CAMERA_04;				// AGC設定
	que.nLineNo = nIdx+1;
	que.fl.data[0] = 16 + pCls->GetMyCamIdStart();			// 指定カメラ番号
	que.fl.data[1] = 0;			// 相対
	que.fl.data[2] = 1;
	send_mail(TO_CAMERA, ".", &que);
}
void CTO_CAMERADlg::OnBnClickedRoup18()
{
	if ( BST_CHECKED != CWnd::IsDlgButtonChecked(IDC_DEBUG_MANUAL) ) return;
	int nIdx = m_cmbUnitDsp.GetCurSel();	if( -1 == nIdx) return;					// iniファイルの設定無し
	CameraManager*			pCls  = mcls_MI.gcls_pCam[nIdx];

	COMMON_QUE que;
	que.nEventNo = FACT_TO_CAMERA_04;				// AGC設定
	que.nLineNo = nIdx+1;
	que.fl.data[0] = 17 + pCls->GetMyCamIdStart();			// 指定カメラ番号
	que.fl.data[1] = 0;			// 相対
	que.fl.data[2] = 1;
	send_mail(TO_CAMERA, ".", &que);
}
void CTO_CAMERADlg::OnBnClickedRoup19()
{
	if ( BST_CHECKED != CWnd::IsDlgButtonChecked(IDC_DEBUG_MANUAL) ) return;
	int nIdx = m_cmbUnitDsp.GetCurSel();	if( -1 == nIdx) return;					// iniファイルの設定無し
	CameraManager*			pCls  = mcls_MI.gcls_pCam[nIdx];

	COMMON_QUE que;
	que.nEventNo = FACT_TO_CAMERA_04;				// AGC設定
	que.nLineNo = nIdx+1;
	que.fl.data[0] = 18 + pCls->GetMyCamIdStart();			// 指定カメラ番号
	que.fl.data[1] = 0;			// 相対
	que.fl.data[2] = 1;
	send_mail(TO_CAMERA, ".", &que);
}
void CTO_CAMERADlg::OnBnClickedRoup20()
{
	if ( BST_CHECKED != CWnd::IsDlgButtonChecked(IDC_DEBUG_MANUAL) ) return;
	int nIdx = m_cmbUnitDsp.GetCurSel();	if( -1 == nIdx) return;					// iniファイルの設定無し
	CameraManager*			pCls  = mcls_MI.gcls_pCam[nIdx];

	COMMON_QUE que;
	que.nEventNo = FACT_TO_CAMERA_04;				// AGC設定
	que.nLineNo = nIdx+1;
	que.fl.data[0] = 19 + pCls->GetMyCamIdStart();			// 指定カメラ番号
	que.fl.data[1] = 0;			// 相対
	que.fl.data[2] = 1;
	send_mail(TO_CAMERA, ".", &que);
}
void CTO_CAMERADlg::OnBnClickedRoup21()
{
	if ( BST_CHECKED != CWnd::IsDlgButtonChecked(IDC_DEBUG_MANUAL) ) return;
	int nIdx = m_cmbUnitDsp.GetCurSel();	if( -1 == nIdx) return;					// iniファイルの設定無し
	CameraManager*			pCls  = mcls_MI.gcls_pCam[nIdx];

	COMMON_QUE que;
	que.nEventNo = FACT_TO_CAMERA_04;				// AGC設定
	que.nLineNo = nIdx+1;
	que.fl.data[0] = 20 + pCls->GetMyCamIdStart();			// 指定カメラ番号
	que.fl.data[1] = 0;			// 相対
	que.fl.data[2] = 1;
	send_mail(TO_CAMERA, ".", &que);
}
void CTO_CAMERADlg::OnBnClickedRoup22()
{
	if ( BST_CHECKED != CWnd::IsDlgButtonChecked(IDC_DEBUG_MANUAL) ) return;
	int nIdx = m_cmbUnitDsp.GetCurSel();	if( -1 == nIdx) return;					// iniファイルの設定無し
	CameraManager*			pCls  = mcls_MI.gcls_pCam[nIdx];

	COMMON_QUE que;
	que.nEventNo = FACT_TO_CAMERA_04;				// AGC設定
	que.nLineNo = nIdx+1;
	que.fl.data[0] = 21 + pCls->GetMyCamIdStart();			// 指定カメラ番号
	que.fl.data[1] = 0;			// 相対
	que.fl.data[2] = 1;
	send_mail(TO_CAMERA, ".", &que);
}
void CTO_CAMERADlg::OnBnClickedRoup23()
{
	if ( BST_CHECKED != CWnd::IsDlgButtonChecked(IDC_DEBUG_MANUAL) ) return;
	int nIdx = m_cmbUnitDsp.GetCurSel();	if( -1 == nIdx) return;					// iniファイルの設定無し
	CameraManager*			pCls  = mcls_MI.gcls_pCam[nIdx];

	COMMON_QUE que;
	que.nEventNo = FACT_TO_CAMERA_04;				// AGC設定
	que.nLineNo = nIdx+1;
	que.fl.data[0] = 22 + pCls->GetMyCamIdStart();			// 指定カメラ番号
	que.fl.data[1] = 0;			// 相対
	que.fl.data[2] = 1;
	send_mail(TO_CAMERA, ".", &que);
}
void CTO_CAMERADlg::OnBnClickedRoup24()
{
	if ( BST_CHECKED != CWnd::IsDlgButtonChecked(IDC_DEBUG_MANUAL) ) return;
	int nIdx = m_cmbUnitDsp.GetCurSel();	if( -1 == nIdx) return;					// iniファイルの設定無し
	CameraManager*			pCls  = mcls_MI.gcls_pCam[nIdx];

	COMMON_QUE que;
	que.nEventNo = FACT_TO_CAMERA_04;				// AGC設定
	que.nLineNo = nIdx+1;
	que.fl.data[0] = 23 + pCls->GetMyCamIdStart();			// 指定カメラ番号
	que.fl.data[1] = 0;			// 相対
	que.fl.data[2] = 1;
	send_mail(TO_CAMERA, ".", &que);
}
void CTO_CAMERADlg::OnBnClickedRoup25()
{
	if ( BST_CHECKED != CWnd::IsDlgButtonChecked(IDC_DEBUG_MANUAL) ) return;
	int nIdx = m_cmbUnitDsp.GetCurSel();	if( -1 == nIdx) return;					// iniファイルの設定無し
	CameraManager*			pCls  = mcls_MI.gcls_pCam[nIdx];

	COMMON_QUE que;
	que.nEventNo = FACT_TO_CAMERA_04;				// AGC設定
	que.nLineNo = nIdx+1;
	que.fl.data[0] = 24 + pCls->GetMyCamIdStart();			// 指定カメラ番号
	que.fl.data[1] = 0;			// 相対
	que.fl.data[2] = 1;
	send_mail(TO_CAMERA, ".", &que);
}
void CTO_CAMERADlg::OnBnClickedRoup26()
{
	if ( BST_CHECKED != CWnd::IsDlgButtonChecked(IDC_DEBUG_MANUAL) ) return;
	int nIdx = m_cmbUnitDsp.GetCurSel();	if( -1 == nIdx) return;					// iniファイルの設定無し
	CameraManager*			pCls  = mcls_MI.gcls_pCam[nIdx];

	COMMON_QUE que;
	que.nEventNo = FACT_TO_CAMERA_04;				// AGC設定
	que.nLineNo = nIdx+1;
	que.fl.data[0] = 25 + pCls->GetMyCamIdStart();			// 指定カメラ番号
	que.fl.data[1] = 0;			// 相対
	que.fl.data[2] = 1;
	send_mail(TO_CAMERA, ".", &que);
}
void CTO_CAMERADlg::OnBnClickedRoup27()
{
	if ( BST_CHECKED != CWnd::IsDlgButtonChecked(IDC_DEBUG_MANUAL) ) return;
	int nIdx = m_cmbUnitDsp.GetCurSel();	if( -1 == nIdx) return;					// iniファイルの設定無し
	CameraManager*			pCls  = mcls_MI.gcls_pCam[nIdx];

	COMMON_QUE que;
	que.nEventNo = FACT_TO_CAMERA_04;				// AGC設定
	que.nLineNo = nIdx+1;
	que.fl.data[0] = 26 + pCls->GetMyCamIdStart();			// 指定カメラ番号
	que.fl.data[1] = 0;			// 相対
	que.fl.data[2] = 1;
	send_mail(TO_CAMERA, ".", &que);
}
void CTO_CAMERADlg::OnBnClickedRoup28()
{
	if ( BST_CHECKED != CWnd::IsDlgButtonChecked(IDC_DEBUG_MANUAL) ) return;
	int nIdx = m_cmbUnitDsp.GetCurSel();	if( -1 == nIdx) return;					// iniファイルの設定無し
	CameraManager*			pCls  = mcls_MI.gcls_pCam[nIdx];

	COMMON_QUE que;
	que.nEventNo = FACT_TO_CAMERA_04;				// AGC設定
	que.nLineNo = nIdx+1;
	que.fl.data[0] = 27 + pCls->GetMyCamIdStart();			// 指定カメラ番号
	que.fl.data[1] = 0;			// 相対
	que.fl.data[2] = 1;
	send_mail(TO_CAMERA, ".", &que);
}
void CTO_CAMERADlg::OnBnClickedRoup29()
{
	if ( BST_CHECKED != CWnd::IsDlgButtonChecked(IDC_DEBUG_MANUAL) ) return;
	int nIdx = m_cmbUnitDsp.GetCurSel();	if( -1 == nIdx) return;					// iniファイルの設定無し
	CameraManager*			pCls  = mcls_MI.gcls_pCam[nIdx];

	COMMON_QUE que;
	que.nEventNo = FACT_TO_CAMERA_04;				// AGC設定
	que.nLineNo = nIdx+1;
	que.fl.data[0] = 28 + pCls->GetMyCamIdStart();			// 指定カメラ番号
	que.fl.data[1] = 0;			// 相対
	que.fl.data[2] = 1;
	send_mail(TO_CAMERA, ".", &que);
}
void CTO_CAMERADlg::OnBnClickedRoup30()
{
	if ( BST_CHECKED != CWnd::IsDlgButtonChecked(IDC_DEBUG_MANUAL) ) return;
	int nIdx = m_cmbUnitDsp.GetCurSel();	if( -1 == nIdx) return;					// iniファイルの設定無し
	CameraManager*			pCls  = mcls_MI.gcls_pCam[nIdx];

	COMMON_QUE que;
	que.nEventNo = FACT_TO_CAMERA_04;				// AGC設定
	que.nLineNo = nIdx+1;
	que.fl.data[0] = 29 + pCls->GetMyCamIdStart();			// 指定カメラ番号
	que.fl.data[1] = 0;			// 相対
	que.fl.data[2] = 1;
	send_mail(TO_CAMERA, ".", &que);
}
void CTO_CAMERADlg::OnBnClickedRoup31()
{
	if ( BST_CHECKED != CWnd::IsDlgButtonChecked(IDC_DEBUG_MANUAL) ) return;
	int nIdx = m_cmbUnitDsp.GetCurSel();	if( -1 == nIdx) return;					// iniファイルの設定無し
	CameraManager*			pCls  = mcls_MI.gcls_pCam[nIdx];

	COMMON_QUE que;
	que.nEventNo = FACT_TO_CAMERA_04;				// AGC設定
	que.nLineNo = nIdx+1;
	que.fl.data[0] = 30 + pCls->GetMyCamIdStart();			// 指定カメラ番号
	que.fl.data[1] = 0;			// 相対
	que.fl.data[2] = 1;
	send_mail(TO_CAMERA, ".", &que);
}
void CTO_CAMERADlg::OnBnClickedRoup32()
{
	if ( BST_CHECKED != CWnd::IsDlgButtonChecked(IDC_DEBUG_MANUAL) ) return;
	int nIdx = m_cmbUnitDsp.GetCurSel();	if( -1 == nIdx) return;					// iniファイルの設定無し
	CameraManager*			pCls  = mcls_MI.gcls_pCam[nIdx];

	COMMON_QUE que;
	que.nEventNo = FACT_TO_CAMERA_04;				// AGC設定
	que.nLineNo = nIdx+1;
	que.fl.data[0] = 31 + pCls->GetMyCamIdStart();			// 指定カメラ番号
	que.fl.data[1] = 0;			// 相対
	que.fl.data[2] = 1;
	send_mail(TO_CAMERA, ".", &que);
}



//￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣
// AGC設定  相対下げ
//＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿
void CTO_CAMERADlg::OnBnClickedRodown1()
{
	if ( BST_CHECKED != CWnd::IsDlgButtonChecked(IDC_DEBUG_MANUAL) ) return;
	int nIdx = m_cmbUnitDsp.GetCurSel();	if( -1 == nIdx) return;					// iniファイルの設定無し
	CameraManager*			pCls  = mcls_MI.gcls_pCam[nIdx];

	COMMON_QUE que;
	que.nEventNo = FACT_TO_CAMERA_04;				// AGC設定
	que.nLineNo = nIdx+1;
	que.fl.data[0] = 0 + pCls->GetMyCamIdStart();			// 指定カメラ番号
	que.fl.data[1] = 0;			// 相対
	que.fl.data[2] = -1;
	send_mail(TO_CAMERA, ".", &que);
}
void CTO_CAMERADlg::OnBnClickedRodown2()
{
	if ( BST_CHECKED != CWnd::IsDlgButtonChecked(IDC_DEBUG_MANUAL) ) return;
	int nIdx = m_cmbUnitDsp.GetCurSel();	if( -1 == nIdx) return;					// iniファイルの設定無し
	CameraManager*			pCls  = mcls_MI.gcls_pCam[nIdx];

	COMMON_QUE que;
	que.nEventNo = FACT_TO_CAMERA_04;				// AGC設定
	que.nLineNo = nIdx+1;
	que.fl.data[0] = 1 + pCls->GetMyCamIdStart();			// 指定カメラ番号
	que.fl.data[1] = 0;			// 相対
	que.fl.data[2] = -1;
	send_mail(TO_CAMERA, ".", &que);
}
void CTO_CAMERADlg::OnBnClickedRodown3()
{
	if ( BST_CHECKED != CWnd::IsDlgButtonChecked(IDC_DEBUG_MANUAL) ) return;
	int nIdx = m_cmbUnitDsp.GetCurSel();	if( -1 == nIdx) return;					// iniファイルの設定無し
	CameraManager*			pCls  = mcls_MI.gcls_pCam[nIdx];

	COMMON_QUE que;
	que.nEventNo = FACT_TO_CAMERA_04;				// AGC設定
	que.nLineNo = nIdx+1;
	que.fl.data[0] = 2 + pCls->GetMyCamIdStart();			// 指定カメラ番号
	que.fl.data[1] = 0;			// 相対
	que.fl.data[2] = -1;
	send_mail(TO_CAMERA, ".", &que);
}
void CTO_CAMERADlg::OnBnClickedRodown4()
{
	if ( BST_CHECKED != CWnd::IsDlgButtonChecked(IDC_DEBUG_MANUAL) ) return;
	int nIdx = m_cmbUnitDsp.GetCurSel();	if( -1 == nIdx) return;					// iniファイルの設定無し
	CameraManager*			pCls  = mcls_MI.gcls_pCam[nIdx];

	COMMON_QUE que;
	que.nEventNo = FACT_TO_CAMERA_04;				// AGC設定
	que.nLineNo = nIdx+1;
	que.fl.data[0] = 3 + pCls->GetMyCamIdStart();			// 指定カメラ番号
	que.fl.data[1] = 0;			// 相対
	que.fl.data[2] = -1;
	send_mail(TO_CAMERA, ".", &que);
}
void CTO_CAMERADlg::OnBnClickedRodown5()
{
	if ( BST_CHECKED != CWnd::IsDlgButtonChecked(IDC_DEBUG_MANUAL) ) return;
	int nIdx = m_cmbUnitDsp.GetCurSel();	if( -1 == nIdx) return;					// iniファイルの設定無し
	CameraManager*			pCls  = mcls_MI.gcls_pCam[nIdx];

	COMMON_QUE que;
	que.nEventNo = FACT_TO_CAMERA_04;				// AGC設定
	que.nLineNo = nIdx+1;
	que.fl.data[0] = 4 + pCls->GetMyCamIdStart();			// 指定カメラ番号
	que.fl.data[1] = 0;			// 相対
	que.fl.data[2] = -1;
	send_mail(TO_CAMERA, ".", &que);
}
void CTO_CAMERADlg::OnBnClickedRodown6()
{
	if ( BST_CHECKED != CWnd::IsDlgButtonChecked(IDC_DEBUG_MANUAL) ) return;
	int nIdx = m_cmbUnitDsp.GetCurSel();	if( -1 == nIdx) return;					// iniファイルの設定無し
	CameraManager*			pCls  = mcls_MI.gcls_pCam[nIdx];

	COMMON_QUE que;
	que.nEventNo = FACT_TO_CAMERA_04;				// AGC設定
	que.nLineNo = nIdx+1;
	que.fl.data[0] = 5 + pCls->GetMyCamIdStart();			// 指定カメラ番号
	que.fl.data[1] = 0;			// 相対
	que.fl.data[2] = -1;
	send_mail(TO_CAMERA, ".", &que);
}
void CTO_CAMERADlg::OnBnClickedRodown7()
{
	if ( BST_CHECKED != CWnd::IsDlgButtonChecked(IDC_DEBUG_MANUAL) ) return;
	int nIdx = m_cmbUnitDsp.GetCurSel();	if( -1 == nIdx) return;					// iniファイルの設定無し
	CameraManager*			pCls  = mcls_MI.gcls_pCam[nIdx];

	COMMON_QUE que;
	que.nEventNo = FACT_TO_CAMERA_04;				// AGC設定
	que.nLineNo = nIdx+1;
	que.fl.data[0] = 6 + pCls->GetMyCamIdStart();			// 指定カメラ番号
	que.fl.data[1] = 0;			// 相対
	que.fl.data[2] = -1;
	send_mail(TO_CAMERA, ".", &que);
}
void CTO_CAMERADlg::OnBnClickedRodown8()
{
	if ( BST_CHECKED != CWnd::IsDlgButtonChecked(IDC_DEBUG_MANUAL) ) return;
	int nIdx = m_cmbUnitDsp.GetCurSel();	if( -1 == nIdx) return;					// iniファイルの設定無し
	CameraManager*			pCls  = mcls_MI.gcls_pCam[nIdx];

	COMMON_QUE que;
	que.nEventNo = FACT_TO_CAMERA_04;				// AGC設定
	que.nLineNo = nIdx+1;
	que.fl.data[0] = 7 + pCls->GetMyCamIdStart();			// 指定カメラ番号
	que.fl.data[1] = 0;			// 相対
	que.fl.data[2] = -1;
	send_mail(TO_CAMERA, ".", &que);
}
void CTO_CAMERADlg::OnBnClickedRodown9()
{
	if ( BST_CHECKED != CWnd::IsDlgButtonChecked(IDC_DEBUG_MANUAL) ) return;
	int nIdx = m_cmbUnitDsp.GetCurSel();	if( -1 == nIdx) return;					// iniファイルの設定無し
	CameraManager*			pCls  = mcls_MI.gcls_pCam[nIdx];

	COMMON_QUE que;
	que.nEventNo = FACT_TO_CAMERA_04;				// AGC設定
	que.nLineNo = nIdx+1;
	que.fl.data[0] = 8 + pCls->GetMyCamIdStart();			// 指定カメラ番号
	que.fl.data[1] = 0;			// 相対
	que.fl.data[2] = -1;
	send_mail(TO_CAMERA, ".", &que);
}
void CTO_CAMERADlg::OnBnClickedRodown10()
{
	if ( BST_CHECKED != CWnd::IsDlgButtonChecked(IDC_DEBUG_MANUAL) ) return;
	int nIdx = m_cmbUnitDsp.GetCurSel();	if( -1 == nIdx) return;					// iniファイルの設定無し
	CameraManager*			pCls  = mcls_MI.gcls_pCam[nIdx];

	COMMON_QUE que;
	que.nEventNo = FACT_TO_CAMERA_04;				// AGC設定
	que.nLineNo = nIdx+1;
	que.fl.data[0] = 9 + pCls->GetMyCamIdStart();			// 指定カメラ番号
	que.fl.data[1] = 0;			// 相対
	que.fl.data[2] = -1;
	send_mail(TO_CAMERA, ".", &que);
}
void CTO_CAMERADlg::OnBnClickedRodown11()
{
	if ( BST_CHECKED != CWnd::IsDlgButtonChecked(IDC_DEBUG_MANUAL) ) return;
	int nIdx = m_cmbUnitDsp.GetCurSel();	if( -1 == nIdx) return;					// iniファイルの設定無し
	CameraManager*			pCls  = mcls_MI.gcls_pCam[nIdx];

	COMMON_QUE que;
	que.nEventNo = FACT_TO_CAMERA_04;				// AGC設定
	que.nLineNo = nIdx+1;
	que.fl.data[0] =10 + pCls->GetMyCamIdStart();			// 指定カメラ番号
	que.fl.data[1] = 0;			// 相対
	que.fl.data[2] = -1;
	send_mail(TO_CAMERA, ".", &que);
}
void CTO_CAMERADlg::OnBnClickedRodown12()
{
	if ( BST_CHECKED != CWnd::IsDlgButtonChecked(IDC_DEBUG_MANUAL) ) return;
	int nIdx = m_cmbUnitDsp.GetCurSel();	if( -1 == nIdx) return;					// iniファイルの設定無し
	CameraManager*			pCls  = mcls_MI.gcls_pCam[nIdx];

	COMMON_QUE que;
	que.nEventNo = FACT_TO_CAMERA_04;				// AGC設定
	que.nLineNo = nIdx+1;
	que.fl.data[0] =11 + pCls->GetMyCamIdStart();			// 指定カメラ番号
	que.fl.data[1] = 0;			// 相対
	que.fl.data[2] = -1;
	send_mail(TO_CAMERA, ".", &que);
}
void CTO_CAMERADlg::OnBnClickedRodown13()
{
	if ( BST_CHECKED != CWnd::IsDlgButtonChecked(IDC_DEBUG_MANUAL) ) return;
	int nIdx = m_cmbUnitDsp.GetCurSel();	if( -1 == nIdx) return;					// iniファイルの設定無し
	CameraManager*			pCls  = mcls_MI.gcls_pCam[nIdx];

	COMMON_QUE que;
	que.nEventNo = FACT_TO_CAMERA_04;				// AGC設定
	que.nLineNo = nIdx+1;
	que.fl.data[0] =12 + pCls->GetMyCamIdStart();			// 指定カメラ番号
	que.fl.data[1] = 0;			// 相対
	que.fl.data[2] = -1;
	send_mail(TO_CAMERA, ".", &que);
}
void CTO_CAMERADlg::OnBnClickedRodown14()
{
	if ( BST_CHECKED != CWnd::IsDlgButtonChecked(IDC_DEBUG_MANUAL) ) return;
	int nIdx = m_cmbUnitDsp.GetCurSel();	if( -1 == nIdx) return;					// iniファイルの設定無し
	CameraManager*			pCls  = mcls_MI.gcls_pCam[nIdx];

	COMMON_QUE que;
	que.nEventNo = FACT_TO_CAMERA_04;				// AGC設定
	que.nLineNo = nIdx+1;
	que.fl.data[0] =13 + pCls->GetMyCamIdStart();			// 指定カメラ番号
	que.fl.data[1] = 0;			// 相対
	que.fl.data[2] = -1;
	send_mail(TO_CAMERA, ".", &que);
}
void CTO_CAMERADlg::OnBnClickedRodown15()
{
	if ( BST_CHECKED != CWnd::IsDlgButtonChecked(IDC_DEBUG_MANUAL) ) return;
	int nIdx = m_cmbUnitDsp.GetCurSel();	if( -1 == nIdx) return;					// iniファイルの設定無し
	CameraManager*			pCls  = mcls_MI.gcls_pCam[nIdx];

	COMMON_QUE que;
	que.nEventNo = FACT_TO_CAMERA_04;				// AGC設定
	que.nLineNo = nIdx+1;
	que.fl.data[0] =14 + pCls->GetMyCamIdStart();			// 指定カメラ番号
	que.fl.data[1] = 0;			// 相対
	que.fl.data[2] = -1;
	send_mail(TO_CAMERA, ".", &que);
}
void CTO_CAMERADlg::OnBnClickedRodown16()
{
	if ( BST_CHECKED != CWnd::IsDlgButtonChecked(IDC_DEBUG_MANUAL) ) return;
	int nIdx = m_cmbUnitDsp.GetCurSel();	if( -1 == nIdx) return;					// iniファイルの設定無し
	CameraManager*			pCls  = mcls_MI.gcls_pCam[nIdx];

	COMMON_QUE que;
	que.nEventNo = FACT_TO_CAMERA_04;				// AGC設定
	que.nLineNo = nIdx+1;
	que.fl.data[0] =15 + pCls->GetMyCamIdStart();			// 指定カメラ番号
	que.fl.data[1] = 0;			// 相対
	que.fl.data[2] = -1;
	send_mail(TO_CAMERA, ".", &que);
}
void CTO_CAMERADlg::OnBnClickedRodown17()
{
	if ( BST_CHECKED != CWnd::IsDlgButtonChecked(IDC_DEBUG_MANUAL) ) return;
	int nIdx = m_cmbUnitDsp.GetCurSel();	if( -1 == nIdx) return;					// iniファイルの設定無し
	CameraManager*			pCls  = mcls_MI.gcls_pCam[nIdx];

	COMMON_QUE que;
	que.nEventNo = FACT_TO_CAMERA_04;				// AGC設定
	que.nLineNo = nIdx+1;
	que.fl.data[0] =16 + pCls->GetMyCamIdStart();			// 指定カメラ番号
	que.fl.data[1] = 0;			// 相対
	que.fl.data[2] = -1;
	send_mail(TO_CAMERA, ".", &que);
}
void CTO_CAMERADlg::OnBnClickedRodown18()
{
	if ( BST_CHECKED != CWnd::IsDlgButtonChecked(IDC_DEBUG_MANUAL) ) return;
	int nIdx = m_cmbUnitDsp.GetCurSel();	if( -1 == nIdx) return;					// iniファイルの設定無し
	CameraManager*			pCls  = mcls_MI.gcls_pCam[nIdx];

	COMMON_QUE que;
	que.nEventNo = FACT_TO_CAMERA_04;				// AGC設定
	que.nLineNo = nIdx+1;
	que.fl.data[0] =17 + pCls->GetMyCamIdStart();			// 指定カメラ番号
	que.fl.data[1] = 0;			// 相対
	que.fl.data[2] = -1;
	send_mail(TO_CAMERA, ".", &que);
}
void CTO_CAMERADlg::OnBnClickedRodown19()
{
	if ( BST_CHECKED != CWnd::IsDlgButtonChecked(IDC_DEBUG_MANUAL) ) return;
	int nIdx = m_cmbUnitDsp.GetCurSel();	if( -1 == nIdx) return;					// iniファイルの設定無し
	CameraManager*			pCls  = mcls_MI.gcls_pCam[nIdx];

	COMMON_QUE que;
	que.nEventNo = FACT_TO_CAMERA_04;				// AGC設定
	que.nLineNo = nIdx+1;
	que.fl.data[0] =18 + pCls->GetMyCamIdStart();			// 指定カメラ番号
	que.fl.data[1] = 0;			// 相対
	que.fl.data[2] = -1;
	send_mail(TO_CAMERA, ".", &que);
}
void CTO_CAMERADlg::OnBnClickedRodown20()
{
	if ( BST_CHECKED != CWnd::IsDlgButtonChecked(IDC_DEBUG_MANUAL) ) return;
	int nIdx = m_cmbUnitDsp.GetCurSel();	if( -1 == nIdx) return;					// iniファイルの設定無し
	CameraManager*			pCls  = mcls_MI.gcls_pCam[nIdx];

	COMMON_QUE que;
	que.nEventNo = FACT_TO_CAMERA_04;				// AGC設定
	que.nLineNo = nIdx+1;
	que.fl.data[0] =19 + pCls->GetMyCamIdStart();			// 指定カメラ番号
	que.fl.data[1] = 0;			// 相対
	que.fl.data[2] = -1;
	send_mail(TO_CAMERA, ".", &que);
}
void CTO_CAMERADlg::OnBnClickedRodown21()
{
	if ( BST_CHECKED != CWnd::IsDlgButtonChecked(IDC_DEBUG_MANUAL) ) return;
	int nIdx = m_cmbUnitDsp.GetCurSel();	if( -1 == nIdx) return;					// iniファイルの設定無し
	CameraManager*			pCls  = mcls_MI.gcls_pCam[nIdx];

	COMMON_QUE que;
	que.nEventNo = FACT_TO_CAMERA_04;				// AGC設定
	que.nLineNo = nIdx+1;
	que.fl.data[0] =20 + pCls->GetMyCamIdStart();			// 指定カメラ番号
	que.fl.data[1] = 0;			// 相対
	que.fl.data[2] = -1;
	send_mail(TO_CAMERA, ".", &que);
}
void CTO_CAMERADlg::OnBnClickedRodown22()
{
	if ( BST_CHECKED != CWnd::IsDlgButtonChecked(IDC_DEBUG_MANUAL) ) return;
	int nIdx = m_cmbUnitDsp.GetCurSel();	if( -1 == nIdx) return;					// iniファイルの設定無し
	CameraManager*			pCls  = mcls_MI.gcls_pCam[nIdx];

	COMMON_QUE que;
	que.nEventNo = FACT_TO_CAMERA_04;				// AGC設定
	que.nLineNo = nIdx+1;
	que.fl.data[0] =21 + pCls->GetMyCamIdStart();			// 指定カメラ番号
	que.fl.data[1] = 0;			// 相対
	que.fl.data[2] = -1;
	send_mail(TO_CAMERA, ".", &que);
}
void CTO_CAMERADlg::OnBnClickedRodown23()
{
	if ( BST_CHECKED != CWnd::IsDlgButtonChecked(IDC_DEBUG_MANUAL) ) return;
	int nIdx = m_cmbUnitDsp.GetCurSel();	if( -1 == nIdx) return;					// iniファイルの設定無し
	CameraManager*			pCls  = mcls_MI.gcls_pCam[nIdx];

	COMMON_QUE que;
	que.nEventNo = FACT_TO_CAMERA_04;				// AGC設定
	que.nLineNo = nIdx+1;
	que.fl.data[0] =22 + pCls->GetMyCamIdStart();			// 指定カメラ番号
	que.fl.data[1] = 0;			// 相対
	que.fl.data[2] = -1;
	send_mail(TO_CAMERA, ".", &que);
}
void CTO_CAMERADlg::OnBnClickedRodown24()
{
	if ( BST_CHECKED != CWnd::IsDlgButtonChecked(IDC_DEBUG_MANUAL) ) return;
	int nIdx = m_cmbUnitDsp.GetCurSel();	if( -1 == nIdx) return;					// iniファイルの設定無し
	CameraManager*			pCls  = mcls_MI.gcls_pCam[nIdx];

	COMMON_QUE que;
	que.nEventNo = FACT_TO_CAMERA_04;				// AGC設定
	que.nLineNo = nIdx+1;
	que.fl.data[0] =23 + pCls->GetMyCamIdStart();			// 指定カメラ番号
	que.fl.data[1] = 0;			// 相対
	que.fl.data[2] = -1;
	send_mail(TO_CAMERA, ".", &que);
}
void CTO_CAMERADlg::OnBnClickedRodown25()
{
	if ( BST_CHECKED != CWnd::IsDlgButtonChecked(IDC_DEBUG_MANUAL) ) return;
	int nIdx = m_cmbUnitDsp.GetCurSel();	if( -1 == nIdx) return;					// iniファイルの設定無し
	CameraManager*			pCls  = mcls_MI.gcls_pCam[nIdx];

	COMMON_QUE que;
	que.nEventNo = FACT_TO_CAMERA_04;				// AGC設定
	que.nLineNo = nIdx+1;
	que.fl.data[0] =24 + pCls->GetMyCamIdStart();			// 指定カメラ番号
	que.fl.data[1] = 0;			// 相対
	que.fl.data[2] = -1;
	send_mail(TO_CAMERA, ".", &que);
}
void CTO_CAMERADlg::OnBnClickedRodown26()
{
	if ( BST_CHECKED != CWnd::IsDlgButtonChecked(IDC_DEBUG_MANUAL) ) return;
	int nIdx = m_cmbUnitDsp.GetCurSel();	if( -1 == nIdx) return;					// iniファイルの設定無し
	CameraManager*			pCls  = mcls_MI.gcls_pCam[nIdx];

	COMMON_QUE que;
	que.nEventNo = FACT_TO_CAMERA_04;				// AGC設定
	que.nLineNo = nIdx+1;
	que.fl.data[0] =25 + pCls->GetMyCamIdStart();			// 指定カメラ番号
	que.fl.data[1] = 0;			// 相対
	que.fl.data[2] = -1;
	send_mail(TO_CAMERA, ".", &que);
}
void CTO_CAMERADlg::OnBnClickedRodown27()
{
	if ( BST_CHECKED != CWnd::IsDlgButtonChecked(IDC_DEBUG_MANUAL) ) return;
	int nIdx = m_cmbUnitDsp.GetCurSel();	if( -1 == nIdx) return;					// iniファイルの設定無し
	CameraManager*			pCls  = mcls_MI.gcls_pCam[nIdx];

	COMMON_QUE que;
	que.nEventNo = FACT_TO_CAMERA_04;				// AGC設定
	que.nLineNo = nIdx+1;
	que.fl.data[0] =26 + pCls->GetMyCamIdStart();			// 指定カメラ番号
	que.fl.data[1] = 0;			// 相対
	que.fl.data[2] = -1;
	send_mail(TO_CAMERA, ".", &que);
}
void CTO_CAMERADlg::OnBnClickedRodown28()
{
	if ( BST_CHECKED != CWnd::IsDlgButtonChecked(IDC_DEBUG_MANUAL) ) return;
	int nIdx = m_cmbUnitDsp.GetCurSel();	if( -1 == nIdx) return;					// iniファイルの設定無し
	CameraManager*			pCls  = mcls_MI.gcls_pCam[nIdx];

	COMMON_QUE que;
	que.nEventNo = FACT_TO_CAMERA_04;				// AGC設定
	que.nLineNo = nIdx+1;
	que.fl.data[0] =27 + pCls->GetMyCamIdStart();			// 指定カメラ番号
	que.fl.data[1] = 0;			// 相対
	que.fl.data[2] = -1;
	send_mail(TO_CAMERA, ".", &que);
}
void CTO_CAMERADlg::OnBnClickedRodown29()
{
	if ( BST_CHECKED != CWnd::IsDlgButtonChecked(IDC_DEBUG_MANUAL) ) return;
	int nIdx = m_cmbUnitDsp.GetCurSel();	if( -1 == nIdx) return;					// iniファイルの設定無し
	CameraManager*			pCls  = mcls_MI.gcls_pCam[nIdx];

	COMMON_QUE que;
	que.nEventNo = FACT_TO_CAMERA_04;				// AGC設定
	que.nLineNo = nIdx+1;
	que.fl.data[0] =28 + pCls->GetMyCamIdStart();			// 指定カメラ番号
	que.fl.data[1] = 0;			// 相対
	que.fl.data[2] = -1;
	send_mail(TO_CAMERA, ".", &que);
}
void CTO_CAMERADlg::OnBnClickedRodown30()
{
	if ( BST_CHECKED != CWnd::IsDlgButtonChecked(IDC_DEBUG_MANUAL) ) return;
	int nIdx = m_cmbUnitDsp.GetCurSel();	if( -1 == nIdx) return;					// iniファイルの設定無し
	CameraManager*			pCls  = mcls_MI.gcls_pCam[nIdx];

	COMMON_QUE que;
	que.nEventNo = FACT_TO_CAMERA_04;				// AGC設定
	que.nLineNo = nIdx+1;
	que.fl.data[0] =29 + pCls->GetMyCamIdStart();			// 指定カメラ番号
	que.fl.data[1] = 0;			// 相対
	que.fl.data[2] = -1;
	send_mail(TO_CAMERA, ".", &que);
}
void CTO_CAMERADlg::OnBnClickedRodown31()
{
	if ( BST_CHECKED != CWnd::IsDlgButtonChecked(IDC_DEBUG_MANUAL) ) return;
	int nIdx = m_cmbUnitDsp.GetCurSel();	if( -1 == nIdx) return;					// iniファイルの設定無し
	CameraManager*			pCls  = mcls_MI.gcls_pCam[nIdx];

	COMMON_QUE que;
	que.nEventNo = FACT_TO_CAMERA_04;				// AGC設定
	que.nLineNo = nIdx+1;
	que.fl.data[0] =30 + pCls->GetMyCamIdStart();			// 指定カメラ番号
	que.fl.data[1] = 0;			// 相対
	que.fl.data[2] = -1;
	send_mail(TO_CAMERA, ".", &que);
}
void CTO_CAMERADlg::OnBnClickedRodown32()
{
	if ( BST_CHECKED != CWnd::IsDlgButtonChecked(IDC_DEBUG_MANUAL) ) return;
	int nIdx = m_cmbUnitDsp.GetCurSel();	if( -1 == nIdx) return;					// iniファイルの設定無し
	CameraManager*			pCls  = mcls_MI.gcls_pCam[nIdx];

	COMMON_QUE que;
	que.nEventNo = FACT_TO_CAMERA_04;				// AGC設定
	que.nLineNo = nIdx+1;
	que.fl.data[0] =31 + pCls->GetMyCamIdStart();			// 指定カメラ番号
	que.fl.data[1] = 0;			// 相対
	que.fl.data[2] = -1;
	send_mail(TO_CAMERA, ".", &que);
}



//￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣
// 任意コマンド送信
//＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿
void CTO_CAMERADlg::OnBnClickedCmdSend()
{
	if ( BST_CHECKED != CWnd::IsDlgButtonChecked(IDC_DEBUG_MANUAL) ) return;
	int nIdx = m_cmbUnitExec.GetCurSel();	if( -1 == nIdx) return;	

	// 送信
	COMMON_QUE que;
	que.nEventNo = FACT_TO_CAMERA_09;
	que.nLineNo = nIdx;

	GetDlgItemText(IDC_CMD_INPUT, que.fc.data, sizeof(que.fc.data));
	if( NULL == que.fc.data[0]) return;
	send_mail(TO_CAMERA, ".", &que);
}
