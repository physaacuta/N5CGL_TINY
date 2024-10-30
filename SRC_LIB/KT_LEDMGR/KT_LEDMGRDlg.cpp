// KT_LEDMGRDlg.cpp : 実装ファイル
//

#include "stdafx.h"
#include "KT_LEDMGR.h"
#include "KT_LEDMGRDlg.h"

// メモリーリーク検出 手動追加 ----->>>
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif
// <<<------------------------

// CKT_LEDMGRDlg ダイアログ


CKT_LEDMGRDlg::CKT_LEDMGRDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CKT_LEDMGRDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CKT_LEDMGRDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, m_ListBox);
	DDX_Control(pDX, IDC_CMB_DISP, m_cmbSession);
	DDX_Control(pDX, IDC_CMB_CH, m_cmbCh);
	DDX_Control(pDX, IDC_CMB_DEBAG_CH, m_cmbDebagCh);
	DDX_Control(pDX, IDC_LIST_VAL, m_ListVal);
	DDX_Control(pDX, IDC_LIST_TEMP, m_lstTemp);
}

BEGIN_MESSAGE_MAP(CKT_LEDMGRDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_LBN_DBLCLK(IDC_LIST1, &CKT_LEDMGRDlg::OnLbnDblclkList1)
	ON_BN_CLICKED(IDOK, &CKT_LEDMGRDlg::OnBnClickedOk)
	ON_WM_CLOSE()
	ON_WM_TIMER()
//	ON_WM_ENDSESSION()
	ON_BN_CLICKED(IDC_DEBAG_STATE, &CKT_LEDMGRDlg::OnBnClickedDebagState)
	ON_BN_CLICKED(IDC_DEBAG_TEMP, &CKT_LEDMGRDlg::OnBnClickedDebagTemp)
	ON_BN_CLICKED(IDC_DEBAG_ON, &CKT_LEDMGRDlg::OnBnClickedDebagOn)
	ON_BN_CLICKED(IDC_DEBAG_OFF, &CKT_LEDMGRDlg::OnBnClickedDebagOff)
	ON_BN_CLICKED(IDC_DEBAG_RESET, &CKT_LEDMGRDlg::OnBnClickedDebagReset)
	ON_BN_CLICKED(IDC_DEBAG_LED_SCODE, &CKT_LEDMGRDlg::OnBnClickedDebagLedScode)
	ON_BN_CLICKED(IDC_DEBAG_LED_VAL, &CKT_LEDMGRDlg::OnBnClickedDebagLedVal)
	ON_BN_CLICKED(IDC_DEBAG_LED_HOSEI, &CKT_LEDMGRDlg::OnBnClickedDebagLedHosei)
	ON_BN_CLICKED(IDC_DEBAG_LED_HOSEI_ALL, &CKT_LEDMGRDlg::OnBnClickedDebagLedHoseiAll)
END_MESSAGE_MAP()


// CKT_LEDMGRDlg メッセージ ハンドラ

BOOL CKT_LEDMGRDlg::OnInitDialog()
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

void CKT_LEDMGRDlg::OnPaint()
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
HCURSOR CKT_LEDMGRDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

// これ以降 手動追加 -------------------->>>

//￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣
// ×ボタン
//＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿
void CKT_LEDMGRDlg::OnClose()
{
//	CDialog::OnClose();
}


//￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣
// 強制終了ボタン
//＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿
void CKT_LEDMGRDlg::OnBnClickedOk()
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
// Windowsシャットダウン
//＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿
//void CKT_LEDMGRDlg::OnEndSession(BOOL bEnding)
//{
//	// いきなりWindowsを終了した対策
//	if( bEnding ) ExitProcess();
//
//	CDialog::OnEndSession(bEnding);
//}

//￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣
// メモ帳起動
//＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿
void CKT_LEDMGRDlg::OnLbnDblclkList1()
{
	// ListBoxコントロールのプロパティは、ソート禁止にしておく事
	// ListBoxコントロールのダブルクリックイベントを追加
	mcls_pLog->ShowEdit();
}


//------------------------------------------
// 各プロセス初期化処理
//------------------------------------------
void CKT_LEDMGRDlg::InitProcess()
{
	CString sWk;

	//// タスク初期化処理
	//if( 0 != task_init(XX_CPPSMPEX, 0) ) {		// PCIDがない場合。
	if( 0 != task_init(KT_LEDMGR) ) {
		syslog(SYSNO_TASK_INIT_ERR, "[%s]", KT_LEDMGR);
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
		////// ？？ なぜかReleaseの場合だけ 文字列に _ が入っていると登録失敗する
		if( NULL == mcls_MI.gcls_pLedMgr[ii] ) break;
		m_cmbSession.AddString( mcls_MI.gcls_pLedMgr[ii]->GetMyName() );
	}
	m_cmbSession.SetCurSel(0);				// 0番目を初期選択とする

	for(int ii=0; ii<MAX_LED_CH; ii++) {
		sWk.Format("%d", ii+1);
		m_cmbCh.AddString( sWk );
	}
	m_cmbCh.SetCurSel(0);	
	for(int ii=0; ii<=MAX_LED_CH; ii++) {
		sWk.Format("%d", ii);
		m_cmbDebagCh.AddString( sWk );
	}
	m_cmbDebagCh.SetCurSel(0);	


	//-----------------------
	// 配光関係 リストビューインスタンス生成 & 定義
	if(true) {
		mcnt_pListVal = new DspListView(&m_ListVal);
		// 見た目定義
		m_ListVal.SetExtendedStyle( m_ListVal.GetExtendedStyle() | LVS_EX_GRIDLINES );			// 罫線描画
		m_ListVal.SetExtendedStyle( m_ListVal.GetExtendedStyle() | LVS_EX_FULLROWSELECT );		// 1行全体選択
		m_ListVal.ModifyStyle( 0, LVS_SHOWSELALWAYS);			// 非フォーカス時も選択が残る

	
		// 列追加
		// 基本のブロック数を取得(CHに設定されてなければ、この値になる)
		int nBlock		= GetPrivateProfileInt(KT_LEDMGR, "BLOCK", 0, TASKINI_NAME);

		// 全CHのキーをチェックして、最大の配光ブロック数を取得
		CString sWk;
		char	cKey[64];
		for(int ii=0; ii<MAX_THREAD; ii++) {
			// キーチェック(装置単位)
			sWk.Format("NAME_%d", ii+1);
			GetPrivateProfileString(KT_LEDMGR, sWk, "", cKey, sizeof(cKey), TASKINI_NAME);
			if(0 == strlen(cKey) ) break;

			for(int jj=1; jj<=MAX_LED_CH; jj++){
				sWk.Format("KT_LEDMGR_%s_CH%d", cKey, jj);
				int nWk = GetPrivateProfileInt(sWk, "BLOCK", -1, TASKINI_NAME);
				nBlock = max(nBlock, nWk);
			}
		}

		m_nColCntVal = nBlock;

		mcnt_pListVal->SetColumnAdd(40, ""); 
		for(int ii=1; ii<=nBlock; ii++) {
			sWk.Format("%d", ii); 
			mcnt_pListVal->SetColumnAdd(40, sWk); 
		}

		// 行追加
		mcnt_pListVal->SetRowAll(2);	
		// 固定値セット
		mcnt_pListVal->SetItem( 0, 0, "補正");
		mcnt_pListVal->SetItem( 1, 0, "輝度");

	}

	//-----------------------
	// LED温度 リストビューインスタンス生成 & 定義
	if(true) {
		mcnt_pListTemp = new DspListView(&m_lstTemp);
		// 見た目定義
		m_lstTemp.SetExtendedStyle( m_lstTemp.GetExtendedStyle() | LVS_EX_GRIDLINES );			// 罫線描画
		m_lstTemp.SetExtendedStyle( m_lstTemp.GetExtendedStyle() | LVS_EX_FULLROWSELECT );		// 1行全体選択
		m_lstTemp.ModifyStyle( 0, LVS_SHOWSELALWAYS);			// 非フォーカス時も選択が残る

	
		// 列追加
		// 基本の計測点数を取得(CHに設定されてなければ、この値になる)
		int nTemp		= GetPrivateProfileInt(KT_LEDMGR, "TEMP_UNIT", 0, TASKINI_NAME);

		// 全CHのキーをチェックして、最大の配光ブロック数を取得
		CString sWk;
		char	cKey[64];
		for(int ii=0; ii<MAX_THREAD; ii++) {
			// キーチェック(装置単位)
			sWk.Format("NAME_%d", ii+1);
			GetPrivateProfileString(KT_LEDMGR, sWk, "", cKey, sizeof(cKey), TASKINI_NAME);
			if(0 == strlen(cKey) ) break;

			for(int jj=1; jj<=MAX_LED_CH; jj++){
				sWk.Format("KT_LEDMGR_%s_CH%d", cKey, jj);
				int nWk = GetPrivateProfileInt(sWk, "TEMP_UNIT", -1, TASKINI_NAME);
				nTemp = max(nTemp, nWk);
			}
		}

		m_nColCntTemp = nTemp;

//		mcnt_pListTemp->SetColumnAdd(40, ""); 
		for(int ii=0; ii<nTemp; ii++) {
			sWk.Format("%d", ii+1); 
			mcnt_pListTemp->SetColumnAdd(40, sWk); 
		}

		// 行追加
		mcnt_pListTemp->SetRowAll(1);	
		// 固定値セット
//		mcnt_pListTemp->SetItem( 0, 0, "温度");

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
void CKT_LEDMGRDlg::ExitProcess()
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
	

	// リストビューの終了
	delete(mcnt_pListTemp);
	delete(mcnt_pListVal);


	// タスク終了処理
	task_exit();

	// 画面の終了要求
	PostMessage(WM_QUIT);
}

//------------------------------------------
// メインスレッド DlgThread
// LPVOID param ダイヤログポインタ
//------------------------------------------
unsigned WINAPI CKT_LEDMGRDlg::MainThread(void* param)
{
	CKT_LEDMGRDlg* p = (CKT_LEDMGRDlg *)param;
	COMMON_QUE que;			// 受信構造体

	//// 各プロセス初期化処理 (これ以前はログ出力禁止)
	p->InitProcess();

	//// 起動要因取り込み ループ開始
	while(true) {
		recv_mail(&que);			// 起動要因取り込み

		switch(que.nEventNo) {
//-----------------------------------------------------------------------------------------------
		case FACT_KT_LEDMGR_01:								// パラメータ変更要求 (←TO_DEFECT)
			pLOG(em_MSG), "[MAIL] パラメータ変更要求 (FACT_KT_LEDMGR_01) [scode=%d]", que.fl.data[0]); 
			// メインインスタンスへ通知
			p->mcls_MI.SetDeliveryMail( E_DEF_ML_PARAM, &que);
			break;

//-----------------------------------------------------------------------------------------------
		case FACT_KT_LEDMGR_02:								// 光量変更要求(検査表面状態指定) (←TO_DEFECT)
			/// 2014.10.28 諸藤(高輝度LED対応)------------------------------------------------>>>>
			//pLOG(em_MSG), "[MAIL] 光量変更要求(検査表面状態指定) (FACT_KT_LEDMGR_02) [表裏=%d][CH=%d][scode=%d]", que.fl.data[0], que.fl.data[1], que.fl.data[2]); 
			//if( 0 > que.fl.data[0] || que.fl.data[0] >= NUM_MEN ) break;
			//if( 0 > que.fl.data[1] || que.fl.data[1] > MAX_LED_CH ) break;
			//if( 0 >= que.fl.data[2] || que.fl.data[2] > MAX_SCODE ) break;

			//for(int ii=0; ii<MAX_THREAD; ii++) {
			//	if(NULL == p->mcls_MI.gcls_pLedMgr[ii]) continue;	

			//	if( que.fl.data[0] == p->mcls_MI.gcls_pLedMgr[ii]->GetMen() ) {

			//		p->mcls_MI.gcls_pLedMgr[ii]->AddQue_SetLigthMastVal(que.fl.data[1], que.fl.data[2]);
			//	}
			//}
			pLOG(em_MSG), "[MAIL] 光量変更要求(検査表面状態指定) (FACT_KT_LEDMGR_02) [表裏=%d][INDEX=%d][scode=%d]", que.fl.data[0], que.fl.data[1], que.fl.data[2]); 
			if( 0 > que.fl.data[0] || que.fl.data[0] > NUM_MEN ) break;
			//if( 0 > que.fl.data[0] || que.fl.data[0] >= NUM_MEN ) break;
			if( 0 > que.fl.data[1] || que.fl.data[1] > DEF_INDEX_END ) break;
			if( 0 >= que.fl.data[2] || que.fl.data[2] > MAX_SCODE ) break;

			for(int ii=0; ii<MAX_THREAD; ii++) {
				if(NULL == p->mcls_MI.gcls_pLedMgr[ii]) continue;	
				if(que.fl.data[0] == p->mcls_MI.gcls_pLedMgr[ii]->GetMen() ){
					p->mcls_MI.gcls_pLedMgr[ii]->AddQue_SetLightMastVal_Index(que.fl.data[1], que.fl.data[2]);
				}
			}
			/// 2014.10.28 諸藤(高輝度LED対応)<<<<------------------------------------------------
			break;

//-----------------------------------------------------------------------------------------------
		case FACT_KT_LEDMGR_03:								// 光量変更要求(光量指定) (←TO_DEFECT)
			//pLOG(em_MSG), "[MAIL] 光量変更要求(光量指定) (FACT_KT_LEDMGR_03) [表裏=%d][CH=%d][val=%d,%d]", que.fl.data[0], que.fl.data[1], que.fl.data[2], que.fl.data[3]); 
			//if( 0 > que.fl.data[0] || que.fl.data[0] >= NUM_MEN	) break;
			//if( 0 > que.fl.data[1] || que.fl.data[1] > MAX_LED_CH ) break;		
			//if( 0 > que.fl.data[2] || que.fl.data[2] >= 256 ) break;
			//if( 0 > que.fl.data[3] || que.fl.data[3] >= 256 ) break;

			//for(int ii=0; ii<MAX_THREAD; ii++) {
			//	if(NULL == p->mcls_MI.gcls_pLedMgr[ii]) continue;	

			//	if( que.fl.data[0] == p->mcls_MI.gcls_pLedMgr[ii]->GetMen() ) {
			//		p->mcls_MI.gcls_pLedMgr[ii]->AddQue_SetLigthMastVal(que.fl.data[1], (BYTE)que.fl.data[2], (BYTE)que.fl.data[3]);
			//	}
			//}
			pLOG(em_MSG), "[MAIL] 光量変更要求(光量指定) (FACT_KT_LEDMGR_03) [表裏=%d][INDEX=%d][val=%d,%d]", que.fl.data[0], que.fl.data[1], que.fl.data[2], que.fl.data[3]); 
			//if( 0 > que.fl.data[0] || que.fl.data[0] >= NUM_MEN	) break;
			if( 0 > que.fl.data[0] || que.fl.data[0] > NUM_MEN	) break;
			if( 0 > que.fl.data[1] || que.fl.data[1] > DEF_INDEX_END ) break;		
			if( 0 > que.fl.data[2] || que.fl.data[2] >= 256 ) break;
			if( 0 > que.fl.data[3] || que.fl.data[3] >= 256 ) break;

			for(int ii=0; ii<MAX_THREAD; ii++) {
				if(NULL == p->mcls_MI.gcls_pLedMgr[ii]) continue;
				if( que.fl.data[0] == p->mcls_MI.gcls_pLedMgr[ii]->GetMen() ){
					p->mcls_MI.gcls_pLedMgr[ii]->AddQue_SetLightMastVal_Index(que.fl.data[1], que.fl.data[2], (BYTE)que.fl.data[3]);
				}
			}
			break;

//-----------------------------------------------------------------------------------------------
		case FACT_KT_LEDMGR_04:								// 点灯消灯指示 (←TO_DEFECT)
			//pLOG(em_MSG), "[MAIL] 点灯消灯指示 (FACT_KT_LEDMGR_04) [表裏=%d][CH=%d][%s]", que.fl.data[0], que.fl.data[1], (1==que.fl.data[2]?"点灯":"消灯") ); 
			//if( 0 > que.fl.data[0] || que.fl.data[0] >= NUM_MEN ) break;
			//if( 0 > que.fl.data[1] || que.fl.data[1] > MAX_LED_CH ) break;	

			//for(int ii=0; ii<MAX_THREAD; ii++) {
			//	if(NULL == p->mcls_MI.gcls_pLedMgr[ii]) continue;	

			//	if( que.fl.data[0] == p->mcls_MI.gcls_pLedMgr[ii]->GetMen() ) {
			//		if(1==que.fl.data[2]) p->mcls_MI.gcls_pLedMgr[ii]->AddQue_SendCommand(LedManager::SEND_EXEC_ON, que.fl.data[1]);
			//		else				  p->mcls_MI.gcls_pLedMgr[ii]->AddQue_SendCommand(LedManager::SEND_EXEC_OFF, que.fl.data[1]);		
			//	}
			//}
			pLOG(em_MSG), "[MAIL] 点灯消灯指示 (FACT_KT_LEDMGR_04) [表裏=%d][Index=%d][%s]", que.fl.data[0], que.fl.data[1], (1==que.fl.data[2]?"点灯":"消灯") ); 
			if( 0 > que.fl.data[0] || que.fl.data[0] > NUM_MEN ) break;
			//if( 0 > que.fl.data[0] || que.fl.data[0] >= NUM_MEN ) break;
			if( 0 > que.fl.data[1] || que.fl.data[1] > DEF_INDEX_END ) break;	

			for(int ii=0; ii<MAX_THREAD; ii++) {
				if( NULL == p->mcls_MI.gcls_pLedMgr[ii] ) continue;
				if( que.fl.data[0] == p->mcls_MI.gcls_pLedMgr[ii]->GetMen() ){
					if(1==que.fl.data[2])	p->mcls_MI.gcls_pLedMgr[ii]->AddQue_SendCommand_Index(LedManager::SEND_EXEC_ON, que.fl.data[1]);
					else					p->mcls_MI.gcls_pLedMgr[ii]->AddQue_SendCommand_Index(LedManager::SEND_EXEC_OFF, que.fl.data[1]);
				}
			}
			break;

//-----------------------------------------------------------------------------------------------
		case FACT_KT_LEDMGR_05:								// 点灯累積時間リセット (←SO_GAMENN)
			//pLOG(em_MSG), "[MAIL] 点灯累積時間リセット (FACT_KT_LEDMGR_05) [表裏=%d][CH=%d]", que.fl.data[0], que.fl.data[1] ); 
			//if( 0 > que.fl.data[0] || que.fl.data[0] >= NUM_MEN ) break;
			//if( 0 > que.fl.data[1] || que.fl.data[1] > MAX_LED_CH ) break;	

			//for(int ii=0; ii<MAX_THREAD; ii++) {
			//	if(NULL == p->mcls_MI.gcls_pLedMgr[ii]) continue;	

			//	if( que.fl.data[0] == p->mcls_MI.gcls_pLedMgr[ii]->GetMen() ) {
			//		p->mcls_MI.gcls_pLedMgr[ii]->AddTime_Reset(que.fl.data[1]);
			//	}
			//}
			pLOG(em_MSG), "[MAIL] 点灯累積時間リセット (FACT_KT_LEDMGR_05) [表裏=%d][INDEX=%d]", que.fl.data[0], que.fl.data[1] ); 
			if( 0 > que.fl.data[0] || que.fl.data[0] > NUM_MEN ) break;
			//if( 0 > que.fl.data[0] || que.fl.data[0] >= NUM_MEN ) break;
			if( 0 > que.fl.data[1] || que.fl.data[1] > DEF_INDEX_END ) break;	

			for(int ii=0; ii<MAX_THREAD; ii++) {
				if(NULL == p->mcls_MI.gcls_pLedMgr[ii]) continue;	
				if( que.fl.data[0] == p->mcls_MI.gcls_pLedMgr[ii]->GetMen() ){
					p->mcls_MI.gcls_pLedMgr[ii]->AddTime_Reset_Index(que.fl.data[1]);
				}
			}
			break;

//-----------------------------------------------------------------------------------------------
		case FACT_KS_DB_STATEUS:								// DB状態変更通知 (←KS_SLAVE)
			break;

		case FACT_KS_PCNAME_CHANGE:								// PC名称変更通知 (←KS_SLAVE)
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
void CKT_LEDMGRDlg::OnTimer(UINT_PTR nIDEvent)
{
	CString		sWk;
	int			nWk;

	//// 現在選択されているインスタンスを取得する
	int nIndex = m_cmbSession.GetCurSel();
	if( -1 == nIndex) return;					// iniファイルの設定無し
	LedManager* pCls = mcls_MI.gcls_pLedMgr[nIndex];
	if( NULL == pCls ) return;

	int nCh = m_cmbCh.GetCurSel();
	if( -1 == nCh) return;



	//// ステータスセット
	SetDlgItemText(IDC_STAT_CONNECT,	pCls->GetSendStatName(0));
	SetDlgItemText(IDC_STAT_MODE,		pCls->GetMainModeName());

	SetDlgItemInt(IDC_NOWSCODE,			pCls->GetNowScode() );
	SetDlgItemInt(IDC_NOWLEDBASE,		pCls->GetNowLedBase(nCh) );

	/// 2014.10.28 諸藤(高輝度LED対応)------------------------->>>>
	int nBlock = pCls->GetBlock(nCh);
	for(int ii=0; ii<m_nColCntVal; ii++){
		if(ii < nBlock){
			mcnt_pListVal->SetItem(0, 1+ii, pCls->GetNowLedHosei(nCh, ii) ); 
			mcnt_pListVal->SetItem(1, 1+ii, pCls->GetNowLedValue(nCh, ii) ); 
		}else{
			mcnt_pListVal->SetItem(0, 1+ii, "-" ); 
			mcnt_pListVal->SetItem(1, 1+ii, "-" ); 
		}
	}

	int nTempUnit = pCls->GetTempUnit(nCh);
	for(int ii=0; ii<m_nColCntTemp; ii++){
		if(ii < nTempUnit){
			mcnt_pListTemp->SetItem(0, ii, pCls->GetLedTemp(nCh, ii) ); 
		}else{
			mcnt_pListTemp->SetItem(0, ii, "-" ); 
		}
	}
	/// 2014.10.28 諸藤(高輝度LED対応)<<<<-------------------------


	////SetDlgItemInt(IDC_NOWLEDHOSEI,		pCls->GetNowLedHosei(nCh) );
	////SetDlgItemInt(IDC_NOWLEDVALUE,		pCls->GetNowLedValue(nCh) );

	nWk = pCls->GetStateOnOff(nCh);
	if(0==nWk)		SetDlgItemText(IDC_STATEONOFF, "○");
	else if(1==nWk) SetDlgItemText(IDC_STATEONOFF, "×");
	else			SetDlgItemText(IDC_STATEONOFF, "不明");
	nWk = pCls->GetStateTemp(nCh);
	if(0==nWk)		SetDlgItemText(IDC_STATETEMP, "○");
	else if(1==nWk) SetDlgItemText(IDC_STATETEMP, "加熱");
	else if(2==nWk) SetDlgItemText(IDC_STATETEMP, "ｼｬｯﾄﾀﾞｳﾝ");
	else			SetDlgItemText(IDC_STATETEMP, "不明");
	nWk = pCls->GetStateBreak(nCh);
	if(0==nWk)		SetDlgItemText(IDC_STATEBREAK, "○");
	else if(1==nWk) SetDlgItemText(IDC_STATEBREAK, "×");
	else			SetDlgItemText(IDC_STATEBREAK, "不明");

	SetDlgItemInt(IDC_TEMP1, pCls->GetLedTemp(nCh, 0));
	SetDlgItemInt(IDC_TEMP2, pCls->GetLedTemp(nCh, 1));
	SetDlgItemInt(IDC_TEMP3, pCls->GetLedTemp(nCh, 2));

	CDialog::OnTimer(nIDEvent);
}


//￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣
// 全状態･輝度取得
//＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿
void CKT_LEDMGRDlg::OnBnClickedDebagState()
{
	if ( BST_CHECKED != CWnd::IsDlgButtonChecked(IDC_DEBUG_MANUAL) ) return;
	//// 現在選択されているインスタンスを取得する
	int nIndex = m_cmbSession.GetCurSel();
	if( -1 == nIndex) return;					// iniファイルの設定無し
	LedManager* pCls = mcls_MI.gcls_pLedMgr[nIndex];
	if( NULL == pCls ) return;

	int nCh = m_cmbDebagCh.GetCurSel();
	if( -1 == nCh) return;

	LOG(em_WAR), "[Debag] 全状態取得 <%s>[CH=%d]", pCls->GetMyName(), nCh); 

	pCls->AddQue_SendCommand(LedManager::SEND_CHECK_BREAK, nCh);
	pCls->AddQue_SendCommand(LedManager::SEND_CHECK_HEATING, nCh);
	pCls->AddQue_SendCommand(LedManager::SEND_CHECK_TEMP, nCh);
	pCls->AddQue_SendCommand(LedManager::SEND_CHECK_ONOFF, nCh);
	pCls->AddQue_SendCommand(LedManager::SEND_READ_EEPROM, nCh);
}

//￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣
// LED温度
//＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿
void CKT_LEDMGRDlg::OnBnClickedDebagTemp()
{
	if ( BST_CHECKED != CWnd::IsDlgButtonChecked(IDC_DEBUG_MANUAL) ) return;
	//// 現在選択されているインスタンスを取得する
	int nIndex = m_cmbSession.GetCurSel();
	if( -1 == nIndex) return;					// iniファイルの設定無し
	LedManager* pCls = mcls_MI.gcls_pLedMgr[nIndex];
	if( NULL == pCls ) return;

	int nCh = m_cmbDebagCh.GetCurSel();
	if( -1 == nCh) return;

	LOG(em_WAR), "[Debag] LED温度要求指示 <%s>[CH=%d]", pCls->GetMyName(), nCh); 

	pCls->AddQue_SendCommand(LedManager::SEND_CHECK_TEMP, nCh);
}
//￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣
// 点灯
//＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿
void CKT_LEDMGRDlg::OnBnClickedDebagOn()
{
	if ( BST_CHECKED != CWnd::IsDlgButtonChecked(IDC_DEBUG_MANUAL) ) return;
	//// 現在選択されているインスタンスを取得する
	int nIndex = m_cmbSession.GetCurSel();
	if( -1 == nIndex) return;					// iniファイルの設定無し
	LedManager* pCls = mcls_MI.gcls_pLedMgr[nIndex];
	if( NULL == pCls ) return;

	int nCh = m_cmbDebagCh.GetCurSel();
	if( -1 == nCh) return;

	LOG(em_WAR), "[Debag] 点灯指示 <%s>[CH=%d]", pCls->GetMyName(), nCh); 

	pCls->AddQue_SendCommand(LedManager::SEND_EXEC_ON, nCh);
}
//￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣
// 消灯
//＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿
void CKT_LEDMGRDlg::OnBnClickedDebagOff()
{
	if ( BST_CHECKED != CWnd::IsDlgButtonChecked(IDC_DEBUG_MANUAL) ) return;
	//// 現在選択されているインスタンスを取得する
	int nIndex = m_cmbSession.GetCurSel();
	if( -1 == nIndex) return;					// iniファイルの設定無し
	LedManager* pCls = mcls_MI.gcls_pLedMgr[nIndex];
	if( NULL == pCls ) return;

	int nCh = m_cmbDebagCh.GetCurSel();
	if( -1 == nCh) return;

	LOG(em_WAR), "[Debag] 消灯指示 <%s>[CH=%d]", pCls->GetMyName(), nCh); 

	pCls->AddQue_SendCommand(LedManager::SEND_EXEC_OFF, nCh);
}
//￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣
// 累計時間リセット
//＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿
void CKT_LEDMGRDlg::OnBnClickedDebagReset()
{
	if ( BST_CHECKED != CWnd::IsDlgButtonChecked(IDC_DEBUG_MANUAL) ) return;
	//// 現在選択されているインスタンスを取得する
	int nIndex = m_cmbSession.GetCurSel();
	if( -1 == nIndex) return;					// iniファイルの設定無し
	LedManager* pCls = mcls_MI.gcls_pLedMgr[nIndex];
	if( NULL == pCls ) return;

	int nCh = m_cmbDebagCh.GetCurSel();
	if( -1 == nCh) return;

	LOG(em_WAR), "[Debag] LED表示累積時間リセット <表裏:%d>[CH=%d]", pCls->GetMen(), nCh);

	COMMON_QUE que;
	que.nEventNo = FACT_KT_LEDMGR_05;
	que.nLineNo = getlineid();
	que.fl.data[0] = pCls->GetMen();
	que.fl.data[1] = nCh; 
	send_mail(KT_LEDMGR, ".", &que);
}
//￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣
// scode 指定
//＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿
void CKT_LEDMGRDlg::OnBnClickedDebagLedScode()
{
	if ( BST_CHECKED != CWnd::IsDlgButtonChecked(IDC_DEBUG_MANUAL) ) return;
	//// 現在選択されているインスタンスを取得する
	int nIndex = m_cmbSession.GetCurSel();
	if( -1 == nIndex) return;					// iniファイルの設定無し
	LedManager* pCls = mcls_MI.gcls_pLedMgr[nIndex];
	if( NULL == pCls ) return;

	int nCh = m_cmbDebagCh.GetCurSel();
	if( -1 == nCh) return;

	int val;
	val = GetDlgItemInt(IDC_SET_SCODE);	// 異常値入力でも 0 が帰るから 入力異常のチェックはしないよ
	if( 0 >= val || val > MAX_SCODE ) return;

	LOG(em_WAR), "[Debag] 光量セット <%s>[CH=%d][%d]", pCls->GetMyName(), nCh, val); 
	pCls->AddQue_SetLightMastVal(nCh, val);
}
//￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣
// 全体調光 指定
//＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿
void CKT_LEDMGRDlg::OnBnClickedDebagLedVal()
{
	if ( BST_CHECKED != CWnd::IsDlgButtonChecked(IDC_DEBUG_MANUAL) ) return;
	//// 現在選択されているインスタンスを取得する
	int nIndex = m_cmbSession.GetCurSel();
	if( -1 == nIndex) return;					// iniファイルの設定無し
	LedManager* pCls = mcls_MI.gcls_pLedMgr[nIndex];
	if( NULL == pCls ) return;

	int nCh = m_cmbDebagCh.GetCurSel();
	if( -1 == nCh) return;

	int val;
	val = GetDlgItemInt(IDC_SET_BASE);		// 異常値入力でも 0 が帰るから 入力異常のチェックはしないよ
	if( 0 > val || val >= 256 ) return;

	LOG(em_WAR), "[Debag] 全体配光セット <%s>[CH=%d][%d]", pCls->GetMyName(), nCh, val); 
	pCls->AddQue_SetLightMaster(nCh, val);
}

//￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣
// ブロック配光 指定
//＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿
void CKT_LEDMGRDlg::OnBnClickedDebagLedHosei()
{
	if ( BST_CHECKED != CWnd::IsDlgButtonChecked(IDC_DEBUG_MANUAL) ) return;
	//// 現在選択されているインスタンスを取得する
	int nIndex = m_cmbSession.GetCurSel();
	if( -1 == nIndex) return;					// iniファイルの設定無し
	LedManager* pCls = mcls_MI.gcls_pLedMgr[nIndex];
	if( NULL == pCls ) return;

	int nCh = m_cmbDebagCh.GetCurSel();
	if( -1 == nCh) return;

	int val;
	int block;
	val = GetDlgItemInt(IDC_SET_HOSEI_VAL);	// 異常値入力でも 0 が帰るから 入力異常のチェックはしないよ
	block = GetDlgItemInt(IDC_SET_HOSEI_BLOCK);	// 異常値入力でも 0 が帰るから 入力異常のチェックはしないよ
	if( 0 > val || val >= 256 ) return;
	/// 2014.10.28 諸藤(高輝度LED対応)------------------------->>>>
	//if( 0 >= block || block > mcls_MI.gcls_pLedMgr[0]->GetBlock() ) return;
	if( 0 < nCh)	if( 0 >= block || block > pCls->GetBlock(nCh-1) ) return;
	/// 2014.10.28 諸藤(高輝度LED対応)<<<<-------------------------

	LOG(em_WAR), "[Debag] 光量セット <%s>[CH=%d, Block=%d][%d]", pCls->GetMyName(), nCh, block, val); 
	pCls->AddQue_SetLightValue(nCh, block-1, val);
}
//￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣
// ブロック配光 全部
//＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿
void CKT_LEDMGRDlg::OnBnClickedDebagLedHoseiAll()
{
	if ( BST_CHECKED != CWnd::IsDlgButtonChecked(IDC_DEBUG_MANUAL) ) return;
	//// 現在選択されているインスタンスを取得する
	int nIndex = m_cmbSession.GetCurSel();
	if( -1 == nIndex) return;					// iniファイルの設定無し
	LedManager* pCls = mcls_MI.gcls_pLedMgr[nIndex];
	if( NULL == pCls ) return;

	int nCh = m_cmbDebagCh.GetCurSel();
	if( -1 == nCh) return;

	int val;
	val = GetDlgItemInt(IDC_SET_HOSEI_VAL);	// 異常値入力でも 0 が帰るから 入力異常のチェックはしないよ
	if( 0 > val || val >= 256 ) return;

	LOG(em_WAR), "[Debag] 光量セット <%s>[CH=%d][%d]", pCls->GetMyName(), nCh, val); 
	pCls->AddQue_SetLightValue(nCh, val);
}
