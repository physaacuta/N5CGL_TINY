// XX_CPPSMPEXDlg.cpp : 実装ファイル
//

#include "stdafx.h"
#include "XX_CPPSMPEX.h"
#include "XX_CPPSMPEXDlg.h"


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


// テスト用
#define		XX_CPPSMPEX		"XX_CPPSMPEX"

#define		DEBUG_LOG_BCPTIME		// BCP処理時間のログ出力

// CXX_CPPSMPEXDlg ダイアログ


CXX_CPPSMPEXDlg::CXX_CPPSMPEXDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CXX_CPPSMPEXDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CXX_CPPSMPEXDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, m_ListBox);
}

BEGIN_MESSAGE_MAP(CXX_CPPSMPEXDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_LBN_DBLCLK(IDC_LIST1, &CXX_CPPSMPEXDlg::OnLbnDblclkList1)
	ON_BN_CLICKED(IDOK, &CXX_CPPSMPEXDlg::OnBnClickedOk)
	ON_WM_CLOSE()
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_BUTTON1, &CXX_CPPSMPEXDlg::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON2, &CXX_CPPSMPEXDlg::OnBnClickedButton2)
	ON_BN_CLICKED(IDC_BUTTON3, &CXX_CPPSMPEXDlg::OnBnClickedButton3)
END_MESSAGE_MAP()


// CXX_CPPSMPEXDlg メッセージ ハンドラ

BOOL CXX_CPPSMPEXDlg::OnInitDialog()
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

void CXX_CPPSMPEXDlg::OnPaint()
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
HCURSOR CXX_CPPSMPEXDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

// これ以降 手動追加 -------------------->>>

//￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣
// ×ボタン
//＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿
void CXX_CPPSMPEXDlg::OnClose()
{
	//CDialog::OnClose();		// 既存の処理をヤメ
}

//￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣
// 強制終了ボタン
//＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿
void CXX_CPPSMPEXDlg::OnBnClickedOk()
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
void CXX_CPPSMPEXDlg::OnLbnDblclkList1()
{
	// ListBoxコントロールのプロパティは、ソート禁止にしておく事
	// ListBoxコントロールのダブルクリックイベントを追加
	mcls_pLog->ShowEdit();
}


//------------------------------------------
// 各プロセス初期化処理
//------------------------------------------
void CXX_CPPSMPEXDlg::InitProcess()
{
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
	int nRetc = task_init(XX_CPPSMPEX);
	if( 0 != nRetc ) {
		if(-1 == nRetc) { syslog(SYSNO_TASK_INIT_ERR, "[%s] 既にタスク起動済みにより、起動中断。", XX_CPPSMPEX); }
		else			{ syslog(SYSNO_TASK_INIT_ERR, "[%s]", XX_CPPSMPEX); }
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
void CXX_CPPSMPEXDlg::ExitProcess()
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
unsigned WINAPI CXX_CPPSMPEXDlg::MainThread(void* param)
{
	CXX_CPPSMPEXDlg* p = (CXX_CPPSMPEXDlg *)param;
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
void CXX_CPPSMPEXDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: ここにメッセージ ハンドラ コードを追加するか、既定の処理を呼び出します。

	CDialog::OnTimer(nIDEvent);
}







//￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣
// 画面更新タイマ
//＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿

#include "..\..\Library\KizuBCP\BCPBase.h"								// DB超高速書込みクラス
#include "..\..\Library\KizuLib\Class\KizuPerformanceCounter.h"			// 時間計測管理クラス


	struct TYP_BCP_DEFECT_IMG
	{
		char					cKizukenNo[20];			// 疵検管理No
		int						nKizuNo;				// 疵No
		BCPBase::typBindDataEx 	img[4];					// 画像データ G/B/R/R-B  (可変長対応版)
	};

	//// 特徴量 (基本、角度 同一構造体)
	struct TYP_BCP_DEFECT_ATTR
	{
		char					cKizukenNo[SIZE_KIZUKEN_NO];						// 管理No
		int						nKizuNo;											// 疵No
		double					attr[MAX_ATTR_BASE];								// 特徴量
	};

	BCPBase*				mcls_pBcpDefectImg;			// DB超高速書込みクラス
	TYP_BCP_DEFECT_IMG*		m_pBcpDefectImgBuf;			// BCP書き込み用の固定メモリバッファ

	BCPBase*				mcls_pBcpDefectAttr;			// DB超高速書込みクラス
	TYP_BCP_DEFECT_ATTR*	m_pBcpDefectAttrBuf;			// BCP書き込み用の固定メモリバッファ

	static const int MAX_KIZU = 1300;
	static const int MAX_BATCH = 500;
	//static const int MAX_KIZU = 10;
	//static const int MAX_BATCH = 3;
	//static const int MAX_KIZU = 10044;
	//static const int MAX_BATCH = 50;
	static const int IMG_SIZE = 30*1024;

void CXX_CPPSMPEXDlg::OnBnClickedButton1()
{		
	//// 事前準備

	BYTE	bData[IMG_SIZE];
	char	cKizuKen[20] = "test";
	int		nCnt = 0;

	//// 接続
	mcls_pBcpDefectImg = new BCPBase();
	if( ! mcls_pBcpDefectImg->Connect(TASKINI_NAME) ) {
		LOG(em_ERR), "DB コネクト失敗");
		return ;
	}

	//// BCPイニシャル
#ifdef DEBUG_LOG_BCPTIME
	LOG(em_INF), " BCP_Init --->>>");
#endif
	if( !mcls_pBcpDefectImg->BCP_Init(DB_DEFECT_IMG_T) ) {
		LOG(em_ERR), "BCP Init失敗 [%s]", DB_DEFECT_IMG_T);
		mcls_pBcpDefectImg->DisConnect();
		return ;
	}
#ifdef DEBUG_LOG_BCPTIME
	LOG(em_INF), " BCP_Init ---<<<");
#endif

	m_pBcpDefectImgBuf = new TYP_BCP_DEFECT_IMG;
	for(int jj=0; jj<4; jj++) m_pBcpDefectImgBuf->img[jj].Alloc(KIZUBCP_MAX_IMGSIZE);	
#ifdef DEBUG_LOG_BCPTIME
	LOG(em_INF), " BCP_Bind --->>>");
#endif
	mcls_pBcpDefectImg->BCP_Bind_String	(1, m_pBcpDefectImgBuf->cKizukenNo);
	mcls_pBcpDefectImg->BCP_Bind_Value 	(2, (BYTE*)&m_pBcpDefectImgBuf->nKizuNo,	BCPBase::em_Int);
	for(int jj=0; jj<4; jj++) mcls_pBcpDefectImg->BCP_Bind_Binary(3+jj, &m_pBcpDefectImgBuf->img[jj], BCPBase::em_VerBinary);
#ifdef DEBUG_LOG_BCPTIME
	LOG(em_INF), " BCP_Bind ---<<<");
#endif


	KizuPerformanceCounter  ti;
	ti.InitAve(); 
	ti.CounterStart();

	LOG(em_MSG), "開始");



	for(int ii=0; ii<MAX_KIZU; ii++) {
		for(int jj=0; jj<4; jj++) {
			memcpy(m_pBcpDefectImgBuf->img[jj].pData, bData, IMG_SIZE);
			m_pBcpDefectImgBuf->img[jj].SetSize( IMG_SIZE);
#ifdef DEBUG_LOG_BCPTIME
	//LOG(em_INF), " BCP_ColLen --->>>");
#endif
			mcls_pBcpDefectImg->BCP_ColLen( 3+jj, IMG_SIZE );	// varbinary(MAX) に定義変更してから、0xFFFFまでしか登録できない暫定対策
#ifdef DEBUG_LOG_BCPTIME
	//LOG(em_INF), " BCP_ColLen ---<<<");
#endif
		}

		memcpy(m_pBcpDefectImgBuf->cKizukenNo, cKizuKen, sizeof(cKizuKen));		// 疵検管理No
		m_pBcpDefectImgBuf->nKizuNo = ii;													// 疵No
#ifdef DEBUG_LOG_BCPTIME
		//LOG(em_INF), " BCP_SendRow --->>>");
#endif
		bool bWk = mcls_pBcpDefectImg->BCP_SendRow(  );
#ifdef DEBUG_LOG_BCPTIME
		//LOG(em_INF), " BCP_SendRow ---<<<");
#endif

		
		nCnt+= 1;
		if( nCnt >= MAX_BATCH ) {
#ifdef DEBUG_LOG_BCPTIME
			LOG(em_INF), " BCP_Batch --->>>");
#endif
			mcls_pBcpDefectImg->BCP_Batch();
#ifdef DEBUG_LOG_BCPTIME
			LOG(em_INF), " BCP_Batch ---<<<");
#endif
			nCnt = 0;
			LOG(em_INF), "途中・・・ %d行", ii);

		}
	}
	if( nCnt != 0) {
#ifdef DEBUG_LOG_BCPTIME
		LOG(em_INF), " BCP_Batch --->>>");
#endif
		mcls_pBcpDefectImg->BCP_Batch();
#ifdef DEBUG_LOG_BCPTIME
		LOG(em_INF), " BCP_Batch ---<<<");
#endif
	}

	ti.CounterEnd(); 
	LOG(em_MSG), "完了 %.3fms", ti.GetCycleTime());
	double dSize = ((IMG_SIZE*4)*MAX_KIZU)/1024/1024;		// DB書込みサイズ(MB)
	LOG(em_MSG), "書込み速度 %.3fMB/sec = %.3fMB / %.3fms", dSize/(ti.GetCycleTime()/1000), dSize, ti.GetCycleTime());


	//// 切断
#ifdef DEBUG_LOG_BCPTIME
	//LOG(em_INF), " BCP_Down --->>>");
#endif
	mcls_pBcpDefectImg->BCP_Down() ;
#ifdef DEBUG_LOG_BCPTIME
	//LOG(em_INF), " BCP_Down ---<<<");
#endif
	delete m_pBcpDefectImgBuf;
	delete mcls_pBcpDefectImg;

#ifdef DEBUG_LOG_BCPTIME
	LOG(em_INF), " DELETE --->>>");
#endif
#if 1
	CString sql;
	sql.Format("DELETE FROM %s WHERE 管理No='%s'", DB_DEFECT_IMG_T, cKizuKen);
	//sql.Format("TRUNCATE TABLE %s", DB_DEFECT_IMG_T);
	//sql.Format("SELECT * FROM %s WHERE 管理No='%s'", DB_DEFECT_IMG_T, cKizuKen);
	OdbcBase	cls;
	cls.Connect(TASKINI_NAME);
	cls.ExecuteDirect(sql);
	cls.DisConnect();
#endif
#ifdef DEBUG_LOG_BCPTIME
	LOG(em_INF), " DELETE ---<<<");
#endif


}


void CXX_CPPSMPEXDlg::OnBnClickedButton2()
{
	//// 事前準備

	BYTE	bData[IMG_SIZE];
	char	cKizuKen[20] = "test";
	int		nCnt = 0;

	CString sql;
	sql.Format("DELETE FROM %s WHERE 管理No='%s'", DB_DEFECT_IMG_T, cKizuKen);
	OdbcBase	cls;
	cls.Connect(TASKINI_NAME);
	cls.ExecuteDirect(sql);
	cls.DisConnect();

	// 書込み計測開始
	cls.Connect(TASKINI_NAME);

	KizuPerformanceCounter  ti;
	ti.InitAve();
	ti.CounterStart();

	LOG(em_MSG), "開始");


	for (int ii = 0; ii<MAX_KIZU; ii++) {
		DWORD wksize[4];
		LPVOID wkpVal[4];
		// データセット

		for (int jj = 0; jj<4; jj++) {
			wksize[jj] = IMG_SIZE;
			wkpVal[jj] = (BYTE*)&bData[0];
		}


		CString		cSql;

		cSql.Format("INSERT INTO %s VALUES(", DB_DEFECT_IMG_T);

		CreateSQL::sqlColumnInsert(&cSql, cKizuKen, SIZE_KIZUKEN_NO);	// 管理No
		CreateSQL::sqlColumnInsert(&cSql, ii);							// 疵No
		cSql += "?, ?, ?, ?";										// 生伝文
		cSql += ")";

		// 登録
		if (!cls.SqlBinalyWrite(cSql, 4, wksize, wkpVal, false)) {
			LOG(em_NON), "%s", cSql);
			LOG(em_ERR), "検査実績登録失敗 [%s]", cKizuKen);
		}

		nCnt += 1;
		if (nCnt >= MAX_BATCH) {
			nCnt = 0;
			LOG(em_INF), "途中・・・ %d行", ii);

		}
	}

	ti.CounterEnd();
	LOG(em_MSG), "完了 %.3fms", ti.GetCycleTime());
	double dSize = ((IMG_SIZE * 4)*MAX_KIZU) / 1024 / 1024;		// DB書込みサイズ(MB)
	LOG(em_MSG), "書込み速度 %.3fMB/sec = %.3fMB / %.3fms", dSize / (ti.GetCycleTime() / 1000), dSize, ti.GetCycleTime());


	//// 切断
	cls.DisConnect();

}


void CXX_CPPSMPEXDlg::OnBnClickedButton3()
{
	//// 事前準備

	char	cKizuKen[20] = "test";
	int		nCnt = 0;

	CString sql;
	sql.Format("DELETE FROM %s WHERE 管理No='%s'", DB_DEFECT_ATTR0_T, cKizuKen);
	OdbcBase	cls;
	cls.Connect(TASKINI_NAME);
	cls.ExecuteDirect(sql);
	cls.DisConnect();



	//// 接続
	mcls_pBcpDefectAttr = new BCPBase();
	if (!mcls_pBcpDefectAttr->Connect(TASKINI_NAME)) {
		LOG(em_ERR), "DB コネクト失敗");
		return;
	}

	//// BCPイニシャル
#ifdef DEBUG_LOG_BCPTIME
	LOG(em_INF), " BCP_Init --->>>");
#endif
	if (!mcls_pBcpDefectAttr->BCP_Init(DB_DEFECT_ATTR0_T)) {
		LOG(em_ERR), "BCP Init失敗 [%s]", DB_DEFECT_ATTR0_T);
		mcls_pBcpDefectAttr->DisConnect();
		return;
	}
#ifdef DEBUG_LOG_BCPTIME
	LOG(em_INF), " BCP_Init ---<<<");
#endif

	m_pBcpDefectAttrBuf = new TYP_BCP_DEFECT_ATTR;
#ifdef DEBUG_LOG_BCPTIME
	LOG(em_INF), " BCP_Bind --->>>");
#endif
	mcls_pBcpDefectAttr->BCP_Bind_String(1, m_pBcpDefectAttrBuf->cKizukenNo);
	mcls_pBcpDefectAttr->BCP_Bind_Value(2, (BYTE*)&m_pBcpDefectAttrBuf->nKizuNo, BCPBase::em_Int);
	for (int jj = 0; jj<MAX_ATTR_BASE; jj++) mcls_pBcpDefectAttr->BCP_Bind_Value(3 + jj, (BYTE*)&m_pBcpDefectAttrBuf->attr[jj], BCPBase::em_Double);
#ifdef DEBUG_LOG_BCPTIME
	LOG(em_INF), " BCP_Bind ---<<<");
#endif


	KizuPerformanceCounter  ti;
	ti.InitAve();
	ti.CounterStart();

	LOG(em_MSG), "開始");



	for (int ii = 0; ii<MAX_KIZU; ii++) {

		memcpy(m_pBcpDefectAttrBuf->cKizukenNo, cKizuKen, sizeof(cKizuKen));		// 疵検管理No
		m_pBcpDefectAttrBuf->nKizuNo = ii;													// 疵No
		for (int jj = 0; jj < MAX_ATTR_BASE; jj++) m_pBcpDefectAttrBuf->attr[jj] = jj;
#ifdef DEBUG_LOG_BCPTIME
		//LOG(em_INF), " BCP_SendRow --->>>");
#endif
		bool bWk = mcls_pBcpDefectAttr->BCP_SendRow();
#ifdef DEBUG_LOG_BCPTIME
		//LOG(em_INF), " BCP_SendRow ---<<<");
#endif


		nCnt += 1;
		if (nCnt >= MAX_BATCH) {
#ifdef DEBUG_LOG_BCPTIME
			LOG(em_INF), " BCP_Batch --->>>");
#endif
			mcls_pBcpDefectAttr->BCP_Batch();
#ifdef DEBUG_LOG_BCPTIME
			LOG(em_INF), " BCP_Batch ---<<<");
#endif
			nCnt = 0;
			LOG(em_INF), "途中・・・ %d行", ii);

		}
	}
	if (nCnt != 0) {
#ifdef DEBUG_LOG_BCPTIME
		LOG(em_INF), " BCP_Batch --->>>");
#endif
		mcls_pBcpDefectAttr->BCP_Batch();
#ifdef DEBUG_LOG_BCPTIME
		LOG(em_INF), " BCP_Batch ---<<<");
#endif
	}

	ti.CounterEnd();
	LOG(em_MSG), "完了 %.3fms", ti.GetCycleTime());
	double dSize = ((MAX_ATTR_BASE * 8)*MAX_KIZU) / 1024 / 1024;		// DB書込みサイズ(MB)
	LOG(em_MSG), "書込み速度 %.3fMB/sec = %.3fMB / %.3fms", dSize / (ti.GetCycleTime() / 1000), dSize, ti.GetCycleTime());


	//// 切断
#ifdef DEBUG_LOG_BCPTIME
	//LOG(em_INF), " BCP_Down --->>>");
#endif
	mcls_pBcpDefectAttr->BCP_Down();
#ifdef DEBUG_LOG_BCPTIME
	//LOG(em_INF), " BCP_Down ---<<<");
#endif
	delete m_pBcpDefectAttrBuf;
	delete mcls_pBcpDefectAttr;

}
