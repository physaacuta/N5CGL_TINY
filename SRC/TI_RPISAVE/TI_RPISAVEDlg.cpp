// TI_RPISAVEDlg.cpp : 実装ファイル
//

#include "stdafx.h"
#include "TI_RPISAVE.h"
#include "TI_RPISAVEDlg.h"

// メモリーリーク検出 手動追加 ----->>>
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif
// <<<------------------------

// CTI_RPISAVEDlg ダイアログ




CTI_RPISAVEDlg::CTI_RPISAVEDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CTI_RPISAVEDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CTI_RPISAVEDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, m_ListBox);
	DDX_Control(pDX, IDC_LIST_All, m_ViewAll);
	DDX_Control(pDX, IDC_LIST_EXEC, m_ViewExec);
	DDX_Control(pDX, IDC_LIST_QUE, m_ViewQue);
	DDX_Control(pDX, IDC_LIST_RPI, m_ViewRpi);
	DDX_Control(pDX, IDC_LIST_STS, m_ViewSts);
}

BEGIN_MESSAGE_MAP(CTI_RPISAVEDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
//	ON_LBN_SELCHANGE(IDC_LIST1, &CTI_RPISAVEDlg::OnLbnSelchangeList1)
	ON_BN_CLICKED(IDOK, &CTI_RPISAVEDlg::OnBnClickedOk)
	ON_WM_CLOSE()
	ON_WM_TIMER()
	ON_LBN_DBLCLK(IDC_LIST1, &CTI_RPISAVEDlg::OnLbnDblclkList1)
	ON_BN_CLICKED(IDC_DEBUG_START, &CTI_RPISAVEDlg::OnBnClickedDebugStart)
	ON_BN_CLICKED(IDC_DEBUG_STOP, &CTI_RPISAVEDlg::OnBnClickedDebugStop)
	ON_BN_CLICKED(IDC_DSPWINDOW1, &CTI_RPISAVEDlg::OnBnClickedDspwindow1)
	ON_BN_CLICKED(IDC_DEBUG_RESYNC, &CTI_RPISAVEDlg::OnBnClickedDebugResync)
END_MESSAGE_MAP()


// CTI_RPISAVEDlg メッセージ ハンドラ

BOOL CTI_RPISAVEDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// このダイアログのアイコンを設定します。アプリケーションのメイン ウィンドウがダイアログでない場合、
	//  Framework は、この設定を自動的に行います。
	SetIcon(m_hIcon, TRUE);			// 大きいアイコンの設定
	SetIcon(m_hIcon, FALSE);		// 小さいアイコンの設定

	// TODO: 初期化をここに追加します。
// 画面初期値セット ============================>>>>
	CWnd::CheckDlgButton(IDC_DEBUG_MANUAL, 1);


	// TODO: 初期化をここに追加します。
// 手動追加 -------------------->>>
	// 画面位置 (サイズはいまのまま)
	::SetWindowPos(m_hWnd, NULL, 335, 430, 0, 0, SWP_NOSIZE | SWP_NOZORDER);

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

void CTI_RPISAVEDlg::OnPaint()
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
HCURSOR CTI_RPISAVEDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

// これ以降 手動追加 -------------------->>>

//￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣
// ×ボタン
//＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿
void CTI_RPISAVEDlg::OnClose()
{
	//CDialog::OnClose();
}

//￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣
// 強制終了ボタン
//＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿
void CTI_RPISAVEDlg::OnBnClickedOk()
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
void CTI_RPISAVEDlg::OnLbnDblclkList1()
{
	// ListBoxコントロールのプロパティは、ソート禁止にしておく事
	// ListBoxコントロールのダブルクリックイベントを追加
	mcls_pLog->ShowEdit();
}

#include <locale.h>
#include "..\..\Library\KizuLibMFC\Class\LogFileManager.h"				// ログ管理クラス



//------------------------------------------
// 各プロセス初期化処理
//------------------------------------------
void CTI_RPISAVEDlg::InitProcess()
{
	CString sWk;

	//// 一番初めにいろいろ初期化
	mcls_pLog = NULL;

	//// タスク初期化処理
	int nRetc = task_init(TI_RPISAVE);
	if( 0 != nRetc ) {
		if(-1 == nRetc) { syslog(SYSNO_TASK_INIT_ERR, "[%s] 既にタスク起動済みにより、起動中断。", TI_RPISAVE); }
		else			{ syslog(SYSNO_TASK_INIT_ERR, "[%s]", TI_RPISAVE); }
		_ASSERT(0);
		exit(-9);		// 自プロセス強制終了
		return;
	}

	//// LogFileManagerインスタンス生成 (これ以降 ログ出力可能)
	setlocale(LC_ALL, "Japanese");	// これが無いと日本語パスを含むフォルダの場合、保存できない。(ライブラリの中で実行するべきか迷っているが、変更するとタスク全体に影響するので、とりあえずここで設定)
	mcls_pLog = new LogFileManager(getmytaskname(), &m_ListBox, 100, 5);
	for( int ii = 0; ii < MAX_INS; ii++){
		CString logtitle;logtitle.Format("sockins%d", ii);

		mcls_MI.gcls_InsLog[ii] = new LogFileManager(logtitle, NULL, 100, 5);
	}
	mcls_MI.gcls_InsLog_ImgBuf_Union = new LogFileManager("ImgUnion", NULL, 100, 5);

	LOG(em_MSG), "[Dlg] タスク起動");
	LOG(em_MSG), "[Dlg] 各プロセス開始処理 実行中・・・");

	//// メインインスタンス 開始処理
	mcls_MI.SetLogMgr(mcls_pLog);
	mcls_MI.Init();
	

	// =========================================================
	// 画面定義
	ParamManager::P_MAIN		const& prm	= mcls_MI.gcls_pParam->GetMain();
	int nMaxCam = 0;			// 最大カメラ数
	int nRpiNum = 0;			// RPI生成数
	for(int ii=0; ii<prm.nIns; ii++) {
		ParamManager::P_MAIN::INS   const& prmI	= mcls_MI.gcls_pParam->GetMain().ins[ii];
		nMaxCam = __max(prmI.nCamsetNum, nMaxCam);
		nRpiNum += prm.nImgNumTotal;
	}

	//-----------------------
	// キュー状態
	mcnt_pQue = new DspListView(&m_ViewQue);
	m_ViewQue.SetExtendedStyle( m_ViewQue.GetExtendedStyle() | LVS_EX_GRIDLINES );			// 罫線描画
	m_ViewQue.SetExtendedStyle( m_ViewQue.GetExtendedStyle() | LVS_EX_FULLROWSELECT );		// 1行全体選択
	
	// 列
	mcnt_pQue->SetColumnAdd(100, "QUE状態");
	mcnt_pQue->SetColumnAdd(50, "最大Que");
	for(int ii=0; ii<prm.nIns; ii++) { sWk.Format("Ins%d", ii); mcnt_pQue->SetColumnAdd(35, sWk); }
	// 行
	mcnt_pQue->SetRowAll(11);	
	// 固定値
	mcnt_pQue->SetItem( 0, 0, "各受信ｷｭｰ空き");
	mcnt_pQue->SetItem( 1, 0, "連結後ｷｭｰ空き");
	mcnt_pQue->SetItem( 2, 0, "幅方向待ち数");

	mcnt_pQue->SetItem( 4, 0, "ｶﾒﾗ同期待ち");
	mcnt_pQue->SetItem( 5, 0, "連結待ち");
	mcnt_pQue->SetItem( 6, 0, "圧縮待ち");
	mcnt_pQue->SetItem( 7, 0, "書込み待ち");
	mcnt_pQue->SetItem( 8, 0, "表示待ち");



	//-----------------------
	// 処理時間
	mcnt_pExe = new DspListView(&m_ViewExec);
	m_ViewExec.SetExtendedStyle( m_ViewExec.GetExtendedStyle() | LVS_EX_GRIDLINES );			// 罫線描画
	m_ViewExec.SetExtendedStyle( m_ViewExec.GetExtendedStyle() | LVS_EX_FULLROWSELECT );		// 1行全体選択
	
	// 列
	mcnt_pExe->SetColumnAdd(70, "処理時間");
	mcnt_pExe->SetColumnAdd(40, "今");
	mcnt_pExe->SetColumnAdd(40, "最大");
	// 行
	mcnt_pExe->SetRowAll(6);	
	// 固定値
	mcnt_pExe->SetItem( 0, 0, "ｶﾒﾗ全取得");
	mcnt_pExe->SetItem( 1, 0, "PLG紐付");
	mcnt_pExe->SetItem( 2, 0, "連結");
	mcnt_pExe->SetItem( 3, 0, "画像Buf");
	mcnt_pExe->SetItem( 4, 0, "画像圧縮");
	mcnt_pExe->SetItem( 5, 0, "圧縮中Th");


	//-----------------------
	//// Ins情報
	mcnt_pAll = new DspListView(&m_ViewAll);
	m_ViewAll.SetExtendedStyle( m_ViewAll.GetExtendedStyle() | LVS_EX_GRIDLINES );			// 罫線描画
	m_ViewAll.SetExtendedStyle( m_ViewAll.GetExtendedStyle() | LVS_EX_FULLROWSELECT );		// 1行全体選択
	
	// 列
	mcnt_pAll->SetColumnAdd(30, "Ins");
	mcnt_pAll->SetColumnAdd(25, "面");
	mcnt_pAll->SetColumnAdd(40, "同期");
	mcnt_pAll->SetColumnAdd(25, "S");
	mcnt_pAll->SetColumnAdd(25, "cam");
	mcnt_pAll->SetColumnAdd(25, "E");

	mcnt_pAll->SetColumnAdd(42, "FNo"); 
	for(int ii=1; ii<nMaxCam; ii++) { sWk.Format("%d", ii+1); mcnt_pAll->SetColumnAdd(42, sWk); }
	// 行
	mcnt_pAll->SetRowAll(prm.nIns);	
	for(int ii=0; ii<mcnt_pAll->GetRowCount(); ii++) {
		sWk.Format("Ins%d", ii); 
		mcnt_pAll->SetItem( ii, 0, sWk);
	}


	//-----------------------
	//// Sts情報
	mcnt_pSts = new DspListView(&m_ViewSts);
	m_ViewSts.SetExtendedStyle(m_ViewSts.GetExtendedStyle() | LVS_EX_GRIDLINES);			// 罫線描画
	m_ViewSts.SetExtendedStyle(m_ViewSts.GetExtendedStyle() | LVS_EX_FULLROWSELECT);		// 1行全体選択

																							// 列
	mcnt_pSts->SetColumnAdd(30, "Ins");
	mcnt_pSts->SetColumnAdd(60, "状態");

	for (int ii = 0; ii<nMaxCam; ii++) { sWk.Format("%d", ii + 1); mcnt_pSts->SetColumnAdd(25, sWk); }
	mcnt_pSts->SetColumnAdd(120, "次コイル 管理No");
	// 行
	mcnt_pSts->SetRowAll(prm.nIns);
	for (int ii = 0; ii<mcnt_pSts->GetRowCount(); ii++) {
		sWk.Format("Ins%d", ii);
		mcnt_pSts->SetItem(ii, 0, sWk);
	}


	//-----------------------
	// Rpi情報
	mcnt_pRpi = new DspListView(&m_ViewRpi);
	m_ViewRpi.SetExtendedStyle( m_ViewRpi.GetExtendedStyle() | LVS_EX_GRIDLINES );			// 罫線描画
	m_ViewRpi.SetExtendedStyle( m_ViewRpi.GetExtendedStyle() | LVS_EX_FULLROWSELECT );		// 1行全体選択
	
	// 列
	mcnt_pRpi->SetColumnAdd(30, "Ins");
	mcnt_pRpi->SetColumnAdd(25, "面");
	mcnt_pRpi->SetColumnAdd(25, "色");
	mcnt_pRpi->SetColumnAdd(120, "管理No");
	mcnt_pRpi->SetColumnAdd(50, "CNo");
	mcnt_pRpi->SetColumnAdd(50, "書込数");
	mcnt_pRpi->SetColumnAdd(60, "書込MB/s");
	mcnt_pRpi->SetColumnAdd(50, "書込ms");
	mcnt_pRpi->SetColumnAdd(50, "最大ms");
	mcnt_pRpi->SetColumnAdd(25, "保");
	mcnt_pRpi->SetColumnAdd(400, "パス");
	// 行
	mcnt_pRpi->SetRowAll(nRpiNum);



	//// 画面更新用タイマ生成
	SetTimer(1, 500, NULL);

	//// メインスレッドの実行を行う
	LOG(em_MSG), "[Dlg] 各プロセス開始処理 完了");
	mcls_MI.Start();	

	mcls_MI.gcls_pDispProc->SetEv_ShowWin();
	return;
}

//￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣
// 各プロセス開放処理
//＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿
void CTI_RPISAVEDlg::ExitProcess()
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
	for( int ii = 0; ii < MAX_INS; ii++){
		delete mcls_MI.gcls_InsLog[ii];
		mcls_MI.gcls_InsLog[ii] = 0;
	}	
	delete mcls_MI.gcls_InsLog_ImgBuf_Union; mcls_MI.gcls_InsLog_ImgBuf_Union =0;
	//// 画面コントロール開放
	delete mcnt_pAll;
	delete mcnt_pRpi;
	delete mcnt_pQue;
	delete mcnt_pExe;
	delete mcnt_pSts;

	// タスク終了処理
	task_exit();

	// 画面の終了要求
	PostMessage(WM_QUIT);
}

//------------------------------------------
// メインスレッド DlgThread
// LPVOID param ダイヤログポインタ
//------------------------------------------
unsigned WINAPI CTI_RPISAVEDlg::MainThread(void* param)
{
	CTI_RPISAVEDlg* p = (CTI_RPISAVEDlg *)param;
	COMMON_QUE que;			// 受信構造体

	//// 各プロセス初期化処理 (これ以前はログ出力禁止)
	p->InitProcess();

	//// 起動要因取り込み ループ開始
	while(true) {
		recv_mail(&que);			// 起動要因取り込み

		switch(que.nEventNo) {

//-----------------------------------------------------------------------------------------------
		case FACT_TI_RPISAVE_01:								// 検査開始要求 (←TO_DEFECT)
			pLOG(em_MSG), "[MAIL] 検査開始要求 (FACT_HT_DEFECT_01) fno=%d", que.fl.data[0]); 

			// メインインスタンスへ通知
			p->mcls_MI.SetDeliveryMail( E_DEF_ML_START, &que);
			break;

//-----------------------------------------------------------------------------------------------
		case FACT_TI_RPISAVE_02:								// 検査停止要求 (←TO_DEFECT)
			pLOG(em_MSG), "[MAIL] 検査停止要求 (FACT_HT_DEFECT_02)"); 
			if( StatusManager::STATE_RUN != p->mcls_MI.gcls_Status.GetMainSt() ) {
				pLOG(em_MSG), "[MAIL] 検査停止要求 ヤメ。");	// 繰り返し防止対策
				break;
			}

			// メインインスタンスへ通知
			p->mcls_MI.SetDeliveryMail( E_DEF_ML_STOP, &que);
			break;


//-----------------------------------------------------------------------------------------------
		case FACT_TI_RPISAVE_04:								// 次コイル情報 (←TO_DEFECT)
			pLOG(em_MSG), "[MAIL] 次コイル情報 (FACT_TO_DEFECT_04) [men=%d][%s][%s][fno=%d]", que.mix.idata[0], que.mix.cdata[0], que.mix.cdata[1], que.fl.data[5] ); 

			// メインインスタンスへ通知
			p->mcls_MI.SetDeliveryMail( E_DEF_ML_NEXTCOIL, &que);
			break;

//-----------------------------------------------------------------------------------------------
		case FACT_KS_PCNAME_CHANGE:								// PC名称変更通知 (←KS_SLAVE)
			pLOG(em_MSG), "[MAIL] パラメータ変更要求 (FACT_TO_DEFECT_01) [テーブル=%d]", que.fl.data[0]); 
			// メインインスタンスへ通知
			p->mcls_MI.SetDeliveryMail( E_DEF_ML_PARAM, &que);
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
void CTI_RPISAVEDlg::OnTimer(UINT_PTR nIDEvent)
{
	ParamManager::P_MAIN		const& prm	= mcls_MI.gcls_pParam->GetMain();
	int nInsNum = mcls_MI.gcls_pParam->GetMain().nIns;

	CString ss, sWk, sWk2;
	int ii, jj;
	int nWk;
	int nWkMax;



	// =========================================================
	//// キュー状態

	// 各画像受信ｷｭｰ空き
	for(ii=0; ii<nInsNum; ii++) {
		mcnt_pQue->SetItem(0, 2+ii, mcls_MI.gque_pSockRecv[ii]->GetCountFree() );
	}

	// 連結後ｷｭｰ空き
	mcnt_pQue->SetItem(1, 2, mcls_MI.gque_pFrame->GetCountFree() );			

	// 幅方向待ち数
	nWkMax = 0;
	for(ii=0; ii<nInsNum; ii++) {
		mcnt_pQue->SetItem(2, 2+ii, mcls_MI.gcls_pSyncProc[ii]->GetMaxBufFrameNum() );
		nWkMax = __max(nWkMax, mcls_MI.gcls_pSyncProc[ii]->GetMaxBufFrameNum());
	}
	mcnt_pQue->SetItem(2, 1, nWkMax);

	// PLG紐付け待ち
	nWkMax = 0;
	for(ii=0; ii<nInsNum; ii++) {
		mcnt_pQue->SetItem(4, 2+ii, mcls_MI.gque_pSockRecv[ii]->GetCountPool() );
		nWkMax = __max(nWkMax, mcls_MI.gque_pSockRecv[ii]->GetMaxCountPool());
	}
	mcnt_pQue->SetItem(4, 1, nWkMax);

	// 連結待ち
	mcnt_pQue->SetItem(5, 1, mcls_MI.gque_pUnion->GetSyncQueMaxCount()); 
	mcnt_pQue->SetItem(5, 2, mcls_MI.gque_pUnion->GetSyncQueCount() );		// 連結待ち


	// 圧縮待ち
	mcnt_pQue->SetItem(6, 1, mcls_MI.gque_pComp->GetSyncQueMaxCount()); 
	mcnt_pQue->SetItem(6, 2, mcls_MI.gque_pComp->GetSyncQueCount() );		// 圧縮待ち

	// 書込み待ち
	nWkMax = 0;
	for(ii=0; ii<nInsNum; ii++) {
		nWk = 0;
		for(int jj=0; jj<MAX_COLOR_NUM; jj++) {
			if( NULL == mcls_MI.gque_pRpiWrite[ii][jj] ) continue;
			nWk = __max(nWk, mcls_MI.gque_pRpiWrite[ii][jj]->GetCount());
		} 
		mcnt_pQue->SetItem(7, 2+ii, nWk );		
		nWkMax = __max(nWkMax, nWk);
	}
	mcnt_pQue->SetItem(7, 1, nWkMax);

	// 表示待ち
	mcnt_pQue->SetItem(8, 1, mcls_MI.gque_Disp.GetMaxCount()); 
	mcnt_pQue->SetItem(8, 2, mcls_MI.gque_Disp.GetCount() );		// 表示待ち



	// =========================================================
	//// 処理時間
	for(int ii=0; ii<2; ii++) {
		mcnt_pExe->SetItem(0, 1+ii, (int)mcls_MI.gcls_pDispProc->g_dTime_Iport[ii] );
		mcnt_pExe->SetItem(1, 1+ii, (int)mcls_MI.gcls_pDispProc->g_dTime_PlgSync[ii] );
		mcnt_pExe->SetItem(2, 1+ii, (int)mcls_MI.gcls_pDispProc->g_dTime_ImgUnion[ii] );
		mcnt_pExe->SetItem(3, 1+ii, (int)mcls_MI.gcls_pDispProc->g_dTime_ImgBuf[ii] );
		mcnt_pExe->SetItem(4, 1+ii, (int)mcls_MI.gcls_pDispProc->g_dTime_ImgComp[ii] );
	}


	// 圧縮実行中
	nWk=0;
	for(int ii=0; ii<MAX_THREAD_MULTI_P; ii++) {
		if( NULL == mcls_MI.gcls_pCompProc[ii] ) continue;
		if( mcls_MI.gcls_pCompProc[ii]->IsComp() ) nWk++;
		mcnt_pExe->SetItem(5, 1, nWk );		
	}


	// =========================================================
	// Ins状態
	for(ii=0; ii<prm.nIns; ii++) {
		if(NULL == mcls_MI.gcls_pSyncProc[ii]) return;
		ParamManager::P_MAIN::INS   const& prmI	= mcls_MI.gcls_pParam->GetMain().ins[ii];

		// パラメータ
		mcnt_pAll->SetItem(ii, 1, prmI.nMen);
		ss = (mcls_MI.gcls_Status.GetFrameSyncSt(ii) ? "○" : "×");	mcnt_pAll->SetItem(ii, 2, ss );
		mcnt_pAll->SetItem(ii, 3, prmI.nStartCamset);
		mcnt_pAll->SetItem(ii, 4, "～");
		mcnt_pAll->SetItem(ii, 5, prmI.nStartCamset + prmI.nCamsetNum -1);

		// iPortNo
		for(jj=0; jj<prm.ins[ii].nCamsetNum; jj++) {
			sWk.Format("%05d", mcls_MI.gcls_pSyncProc[ii]->GetNowExecFrameNo(jj) );
			mcnt_pAll->SetItem(ii, jj+6, sWk);
		}
	}

	// =========================================================
	// Sts状態
	mcnt_pSts->SetItem(0, 1, mcls_MI.gcls_Status.GetMainStName());		// 検査状態
	for (ii = 0; ii<prm.nIns; ii++) {
		// Camset LAN接続
		for (jj = 0; jj<prm.ins[ii].nCamsetNum; jj++) {
			sWk.Format("%s", (mcls_MI.gcls_pSock[ii][jj]->IsConnect(0) ? "○" : "×"));
			mcnt_pSts->SetItem(ii, jj + 2, sWk);
		}
		mcnt_pSts->SetItem(0, 2 + prm.ins[ii].nCamsetNum, mcls_MI.gcls_pCoilProc[ii]->GetNextCoil());		// 次コイル管理No
	}

	// =========================================================
	// RPI状態
	nWk = 0;
	for(int ii=0; ii<prm.nIns; ii++) {
		for(int jj=0; jj<prm.nImgNumTotal; jj++) {
			if( NULL == mcls_MI.gcls_pRpiWProc[ii][jj] ) continue;
			ParamManager::P_MAIN::INS   const& prmI	= mcls_MI.gcls_pParam->GetMain().ins[ii];

			sWk.Format("Ins%d", ii ); mcnt_pRpi->SetItem(nWk, 0, sWk);
			mcnt_pRpi->SetItem(nWk, 1, prmI.nMen);
			mcnt_pRpi->SetItem(nWk, 2, jj);
			mcnt_pRpi->SetItem(nWk, 3, mcls_MI.gcls_pRpiWProc[ii][jj]->GetRpiHeader()->theader.cKizukenNo );
			mcnt_pRpi->SetItem(nWk, 4, mcls_MI.gcls_pRpiWProc[ii][jj]->GetRpiHeader()->theader.nFrameNumStart );
			mcnt_pRpi->SetItem(nWk, 5, mcls_MI.gcls_pRpiWProc[ii][jj]->GetRpiHeader()->nframe );

			sWk.Format("%.01f", mcls_MI.gcls_pRpiWProc[ii][jj]->GetWriteSpeedAve() );	mcnt_pRpi->SetItem(nWk, 6, sWk );
			mcnt_pRpi->SetItem(nWk, 7, (int)mcls_MI.gcls_pDispProc->g_dTime_RpiWrite[ii][0] );
			mcnt_pRpi->SetItem(nWk, 8, (int)mcls_MI.gcls_pDispProc->g_dTime_RpiWrite[ii][1] );
			sWk.Format("%s", mcls_MI.gcls_pRpiWProc[ii][jj]->IsOpen() ? "○":"×"); mcnt_pRpi->SetItem(nWk, 9, sWk );
			mcnt_pRpi->SetItem(nWk, 10, mcls_MI.gcls_pRpiWProc[ii][jj]->GetRpiFullPath() );
			nWk++;
		}
	}





	CDialog::OnTimer(nIDEvent);
}


// これ以降 デバック用コマンド -------------------->>>





//￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣
// 検査開始
//＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿
void CTI_RPISAVEDlg::OnBnClickedDebugStart()
{
	if ( BST_CHECKED != CWnd::IsDlgButtonChecked(IDC_DEBUG_MANUAL) ) return;

	// 現時点のフレームNoから再同期するフレームNoを決定する。
	DWORD nFno = mcls_MI.gcls_pSyncProc[0]->GetRecvFrameNo(0);
	if(0 != nFno) {
		nFno += MAX_RESYNC_FNO;
	}

	COMMON_QUE que;
	memset(&que, 0x00, sizeof(que));
	que.nEventNo   = FACT_TI_RPISAVE_01;
	que.nLineNo	   = getlineid();

//	que.fl.data[0] = nFno; 


	LOG(em_WAR), "[DEBUG] 検査開始ボタン Fno=%d", nFno ); 
	send_mail(getmytaskname(), ".", &que);
}

//￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣
// 検査停止
//＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿
void CTI_RPISAVEDlg::OnBnClickedDebugStop()
{
	if ( BST_CHECKED != CWnd::IsDlgButtonChecked(IDC_DEBUG_MANUAL) ) return;
	LOG(em_WAR), "[DEBUG] 検査停止ボタン" ); 

	COMMON_QUE que;
	memset(&que, 0x00, sizeof(que));
	que.nEventNo   = FACT_TI_RPISAVE_02;
	que.nLineNo    = getlineid();

	send_mail(getmytaskname(), ".", &que);
}

//￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣
// 画像表示デバック画面 表示/非表示
//＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿
void CTI_RPISAVEDlg::OnBnClickedDspwindow1()
{
	if(BST_CHECKED == CWnd::IsDlgButtonChecked(IDC_DSPWINDOW1)){
		mcls_MI.gcls_pDispProc->WindowIsOnOff(true);
	} else {
		mcls_MI.gcls_pDispProc->WindowIsOnOff(false);
	}
}
//￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣
// 検査開始
//＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿
void CTI_RPISAVEDlg::OnBnClickedDebugResync()
{
	if ( BST_CHECKED != CWnd::IsDlgButtonChecked(IDC_DEBUG_MANUAL) ) return;

	// 現時点のフレームNoから再同期するフレームNoを決定する。
	DWORD nFno = mcls_MI.gcls_pSyncProc[0]->GetRecvFrameNo(0);
	if(0 != nFno) {
		nFno += MAX_RESYNC_FNO;
	}

	COMMON_QUE que;
	memset(&que, 0x00, sizeof(que));
	que.nEventNo   = FACT_TI_RPISAVE_05;
	que.nLineNo	   = getlineid();

	que.fl.data[0] = -1; 
	que.fl.data[1] = nFno; 

	LOG(em_WAR), "[DEBUG] 再同期ボタン Fno=%d", nFno ); 
	send_mail(getmytaskname(), ".", &que);
}
