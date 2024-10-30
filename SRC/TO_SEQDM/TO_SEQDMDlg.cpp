// TO_SEQDMDlg.cpp : 実装ファイル
//

#include "stdafx.h"
#include "TO_SEQDM.h"
#include "TO_SEQDMDlg.h"

// メモリーリーク検出 ----->>>
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif
// <<<------------------------

// CTO_SEQDMDlg ダイアログ




CTO_SEQDMDlg::CTO_SEQDMDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CTO_SEQDMDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CTO_SEQDMDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, m_ListBox);
	DDX_Control(pDX, IDC_ACTQJ71E71UDP1, m_cCom);
}

BEGIN_MESSAGE_MAP(CTO_SEQDMDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_WM_CLOSE()
	ON_WM_TIMER()
	ON_LBN_DBLCLK(IDC_LIST1, &CTO_SEQDMDlg::OnLbnDblclkList1)
	ON_BN_CLICKED(IDOK, &CTO_SEQDMDlg::OnBnClickedOk)
END_MESSAGE_MAP()


// CTO_SEQDMDlg メッセージ ハンドラ

BOOL CTO_SEQDMDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// このダイアログのアイコンを設定します。アプリケーションのメイン ウィンドウがダイアログでない場合、
	//  Framework は、この設定を自動的に行います。
	SetIcon(m_hIcon, TRUE);			// 大きいアイコンの設定
	SetIcon(m_hIcon, FALSE);		// 小さいアイコンの設定

	// TODO: 初期化をここに追加します。
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

void CTO_SEQDMDlg::OnPaint()
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
HCURSOR CTO_SEQDMDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}




// これ以降 手動追加 -------------------->>>

//￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣
// ×ボタン
//＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿
void CTO_SEQDMDlg::OnClose()
{
//	CDialog::OnClose();
}


//￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣
// 閉じるボタン
//＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿
void CTO_SEQDMDlg::OnBnClickedOk()
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
void CTO_SEQDMDlg::OnLbnDblclkList1()
{
	// ListBoxコントロールのプロパティは、ソート禁止にしておく事
	// ListBoxコントロールのダブルクリックイベントを追加
	mcls_pLog->ShowEdit();
}

//------------------------------------------
// 各プロセス初期化処理
//------------------------------------------
void CTO_SEQDMDlg::InitProcess()
{
	// タスク初期化処理
	setlineid(LINE_ID);
	if( 0 != task_init(TO_SEQDM) ) {
		syslog(SYSNO_TASK_INIT_ERR, "[%s]", TO_SEQDM);
		_ASSERT(0);
		exit(-9);		// 自プロセス強制終了
		return;
	}

	// LogFileManagerインスタンス生成 (これ以降 ログ出力可能)
	mcls_pLog = new LogFileManager(getmytaskname(), &m_ListBox);
	LOG(em_MSG), "タスク起動");
	LOG(em_MSG), "各プロセス開始処理 実行中・・・");


	// MXコンポーネントのインスタンスを渡す (MainInctanceのinitの前にクラスに渡しておく)
	KizuFunction::SetStatus("DATAMEM_SEQ_ID", false, false);
	mcls_MI.gcls_DMem.SetMxComponent(&m_cCom);

	// メインインスタンス 開始処理
	mcls_MI.SetLogMgr(mcls_pLog);
	mcls_MI.Init();

	
	//// MXコンポーネント初期接続
	m_tidMxConnect = SetTimer(MX_OPEN_RETRY_NID_EVENT, MX_OPEN_RETRY_TIMER, NULL);	

		
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
void CTO_SEQDMDlg::ExitProcess()
{
	//// ログ リストボックス表示処理終了
	mcls_pLog->SetListOut(false);					// これ以降 リストボックスへのログ表示禁止

	// メインインスタンス 終了処理
	KillTimer(1);			// 画面更新タイマ
	mcls_MI.Stop();
	mcls_MI.Exit();

	//// MXコンポーネント終了
	MX_Close();

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
unsigned WINAPI CTO_SEQDMDlg::MainThread(void* param)
{
	CTO_SEQDMDlg* p = (CTO_SEQDMDlg *)param;
	COMMON_QUE que;			// 受信構造体

	//// 各プロセス初期化処理 (これ以前はログ出力禁止)
	p->InitProcess();

	//// 起動要因取り込み ループ開始
	while(true) {
		recv_mail(&que);			// 起動要因取り込み

		switch(que.nEventNo) {
	
		case FACT_KS_PCNAME_CHANGE:							// PC名称変更通知 (←KS_SLAVE)
			// 何も無し
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
void CTO_SEQDMDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: ここにメッセージ ハンドラ コードを追加するか、既定の処理を呼び出します。

	if( nIDEvent == MX_OPEN_RETRY_NID_EVENT ) {

		// MXコンポーネント初期接続
		MX_Connect(false); 
	
	} else {

		// MXコンポーネントの状態
		SetDlgItemText(IDC_MX_CONNECT,		mcls_MI.gcls_DMem.IsConnect() ? "○" : "×");
		SetDlgItemText(IDC_MX_RWSTATUS,		mcls_MI.gcls_DMem.IsStatus() ? "○" : "×");
	}


	CDialog::OnTimer(nIDEvent);
}


//------------------------------------------
// MXコンポーネント オープン
// bool bLogDsp ログ出力
//------------------------------------------
bool CTO_SEQDMDlg::MX_Connect(bool bLogDsp)
{
	if( mcls_MI.gcls_DMem.IsConnect() ) return true;  
	int  nWk;
	char cWk[32];

	// ----------------------------------------------------------
	// MXコンポーネント接続基本設定
	// ----------------------------------------------------------
	//// パソコン側 (自局)
	// ネットワークNo
	nWk = GetPrivateProfileInt(TO_SEQDM, "PC_NETWORK_NO", 0, TASKINI_NAME);		_ASSERT( 0 != nWk);
	m_cCom.put_ActSourceNetworkNumber(nWk);			
	// 局番
	nWk = GetPrivateProfileInt(TO_SEQDM, "PC_STATION_NO", 0, TASKINI_NAME);		_ASSERT( 0 != nWk);
	m_cCom.put_ActSourceStationNumber(nWk);			

	//// シーケンサ側 (他局)
	// ネットワークNo
	nWk = GetPrivateProfileInt(TO_SEQDM, "SEQ_NETWORK_NO", 0, TASKINI_NAME);	_ASSERT( 0 != nWk);
	m_cCom.put_ActNetworkNumber(nWk);	
	// 局番
	nWk = GetPrivateProfileInt(TO_SEQDM, "SEQ_STATION_NO", 0, TASKINI_NAME);	_ASSERT( 0 != nWk);
	m_cCom.put_ActStationNumber(nWk);					

	//// CPUタイプ
	nWk = GetPrivateProfileInt(TO_SEQDM, "SEQ_CPU_TYPE", 0, TASKINI_NAME);		_ASSERT( 0 != nWk);
	m_cCom.put_ActCpuType(nWk);						// マニュアル sh080275f.pdf  P53を参照 
	
	////// 通信
	// タイムアウト [ms]
	nWk = GetPrivateProfileInt(TO_SEQDM, "CONNECT_TIMEOUT", 0, TASKINI_NAME);	_ASSERT( 0 != nWk);
	m_cCom.put_ActTimeOut(nWk);					
	// ポートNo	
	nWk = GetPrivateProfileInt(TO_SEQDM, "CONNECT_PORT", 0, TASKINI_NAME);		_ASSERT( 0 != nWk);
	m_cCom.put_ActPortNumber(nWk);								 
	// IPアドレス
	GetPrivateProfileString(TO_SEQDM,	"CONNECT_IP", "", cWk, sizeof(cWk), TASKINI_NAME ); _ASSERT( 0 != strlen(cWk));
	m_cCom.put_ActHostAddress(cWk);		


	// ----------------------------------------------------------
	// 接続
	// ----------------------------------------------------------
	bool bStatOld = mcls_MI.gcls_DMem.IsConnect();
	// オープン
	long lRet = m_cCom.Open();
	if( 0 == lRet ) {					// 接続
		KillTimer( m_tidMxConnect );

		//// 接続完了
		mcls_MI.gcls_DMem.SetConnect(true);

		LOG(em_MSG), "[Dlg] MXコンポーネント オープン");
		syslog(345, "");
		KizuFunction::SetStatus("DATAMEM_SEQ_ID", true);
		return true;
	} else {							// 切断
		// 接続失敗 
		mcls_MI.gcls_DMem.SetConnect(false);

		if(bLogDsp || bStatOld) {	// 前回が正常。要は正常から異常に変わったときのみ
			LOG(em_ERR), "[Dlg] MXコンポーネント オープン失敗 [%X]", lRet);
			syslog(340, "[errcode = %X]", lRet);
			KizuFunction::SetStatus("DATAMEM_SEQ_ID", false, false);
		}
		return false;
	}
}

//------------------------------------------
// MXコンポーネント クローズ
//------------------------------------------
void CTO_SEQDMDlg::MX_Close()
{
	m_cCom.Close();
	mcls_MI.gcls_DMem.SetConnect(false);

	LOG(em_MSG), "[Dlg] MXコンポーネント クローズ");
	syslog(346, "");
	KizuFunction::SetStatus("DATAMEM_SEQ_ID", false);
}

