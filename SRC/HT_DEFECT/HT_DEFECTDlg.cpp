// HT_DEFECTDlg.cpp : 実装ファイル
//

#include "stdafx.h"
#include "HT_DEFECT.h"
#include "HT_DEFECTDlg.h"

// メモリーリーク検出 手動追加 ----->>>
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif
// <<<------------------------


// CHT_DEFECTDlg ダイアログ


CHT_DEFECTDlg::CHT_DEFECTDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CHT_DEFECTDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CHT_DEFECTDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, m_ListBox);
	//DDX_Control(pDX, IDC_COMBO_MEN, m_cmbMen);
	DDX_Control(pDX, IDC_LIST_ST, m_ListSt);
	DDX_Control(pDX, IDC_LIST_TO, m_ListTo);
}

BEGIN_MESSAGE_MAP(CHT_DEFECTDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDOK, &CHT_DEFECTDlg::OnBnClickedOk)
	ON_WM_CLOSE()
	ON_WM_TIMER()
	ON_LBN_DBLCLK(IDC_LIST1, &CHT_DEFECTDlg::OnLbnDblclkList1)
	ON_BN_CLICKED(IDC_DEBUG_START, &CHT_DEFECTDlg::OnBnClickedDebugStart)
	ON_BN_CLICKED(IDC_DEBUG_STOP, &CHT_DEFECTDlg::OnBnClickedDebugStop)
	ON_BN_CLICKED(IDC_DEBUG_PARAM, &CHT_DEFECTDlg::OnBnClickedDebugParam)
	ON_BN_CLICKED(IDC_DEBUG_COILINF, &CHT_DEFECTDlg::OnBnClickedDebugCoilinf)
	ON_BN_CLICKED(IDC_DEBUG_CANOK, &CHT_DEFECTDlg::OnBnClickedDebugCanok)
//	ON_BN_CLICKED(IDC_DEBUG_FRAME, &CHT_DEFECTDlg::OnBnClickedDebugFrame)
//	ON_BN_CLICKED(IDC_DEBUG_PROFILE, &CHT_DEFECTDlg::OnBnClickedDebugProfile)
//	ON_BN_CLICKED(IDC_DEBUG_DLYCANCEL, &CHT_DEFECTDlg::OnBnClickedDebugDlycancel)
	ON_BN_CLICKED(IDC_DEBUG_IMGKIND, &CHT_DEFECTDlg::OnBnClickedDebugImgkind)
	ON_BN_CLICKED(IDC_DEBUG_PARAM_PRELOAD, &CHT_DEFECTDlg::OnBnClickedDebugParamPreload)
	ON_BN_CLICKED(IDC_DEBUG_SET_POSITION, &CHT_DEFECTDlg::OnBnClickedDebugSetPosition)
	ON_BN_CLICKED(IDC_DEBUG_RESUME_START, &CHT_DEFECTDlg::OnBnClickedDebugResumeStart)
	ON_BN_CLICKED(IDC_DEBUG_RESUME_STOP, &CHT_DEFECTDlg::OnBnClickedDebugResumeStop)
	ON_BN_CLICKED(IDD_DEBUG_KIND1, &CHT_DEFECTDlg::OnBnClickedDebugKind1)
	ON_BN_CLICKED(IDD_DEBUG_KIND2, &CHT_DEFECTDlg::OnBnClickedDebugKind2)
	ON_BN_CLICKED(IDD_DEBUG_KIND3, &CHT_DEFECTDlg::OnBnClickedDebugKind3)
END_MESSAGE_MAP()


// CHT_DEFECTDlg メッセージ ハンドラ

BOOL CHT_DEFECTDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// このダイアログのアイコンを設定します。アプリケーションのメイン ウィンドウがダイアログでない場合、
	//  Framework は、この設定を自動的に行います。
	SetIcon(m_hIcon, TRUE);			// 大きいアイコンの設定
	SetIcon(m_hIcon, FALSE);		// 小さいアイコンの設定

	// TODO: 初期化をここに追加します。

	//// 画面初期設定
	CWnd::CheckDlgButton(IDC_DEBUG_MANUAL, 1);

	CWnd::CheckRadioButton(IDD_DEBUG_KIND1, IDD_DEBUG_KIND3, IDD_DEBUG_KIND1);						// 判定機能
	CWnd::CheckRadioButton(IDC_DEBUG_START_MODE1, IDC_DEBUG_START_MODE4, IDC_DEBUG_START_MODE1);	// 運転モード

	SetDlgItemInt(IDC_DEBUG_CAMPAIR, 1);
	SetDlgItemInt(IDC_DEBUG_CAMSET1, 1);
	SetDlgItemInt(IDC_DEBUG_CAMSET2, 2);
	SetDlgItemInt(IDC_DEBUG_SCODE, MAX_SCODE);

	CWnd::GetDlgItem(IDC_DEBUG_CAMSET1)->EnableWindow(FALSE);
	CWnd::GetDlgItem(IDC_DEBUG_CAMSET2)->EnableWindow(FALSE);
	
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

void CHT_DEFECTDlg::OnPaint()
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
HCURSOR CHT_DEFECTDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



// これ以降 手動追加 -------------------->>>

//￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣
// ×ボタン
//＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿
void CHT_DEFECTDlg::OnClose()
{
	//CDialog::OnClose();
}

//￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣
// 強制終了ボタン
//＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿
void CHT_DEFECTDlg::OnBnClickedOk()
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
void CHT_DEFECTDlg::OnLbnDblclkList1()
{
	// ListBoxコントロールのプロパティは、ソート禁止にしておく事
	// ListBoxコントロールのダブルクリックイベントを追加
	mcls_pLog->ShowEdit();
}

//------------------------------------------
// 各プロセス初期化処理
//------------------------------------------
void CHT_DEFECTDlg::InitProcess()
{
	//// タスク初期化処理
	//if( 0 != task_init(XX_CPPSMPEX, 0) ) {		// PCIDがない場合。
	if( 0 != task_init(HT_DEFECT) ) {
		syslog(SYSNO_TASK_INIT_ERR, "[%s]", HT_DEFECT);
		_ASSERT(0);
		exit(-9);		// 自プロセス強制終了
		return;
	}

	//// LogFileManagerインスタンス生成 (これ以降 ログ出力可能)
	setlocale(LC_ALL, "Japanese");	// これが無いと日本語パスを含むフォルダの場合、保存できない。(ライブラリの中で実行するべきか迷っているが、変更するとタスク全体に影響するので、とりあえずここで設定)
	mcls_pLog = new LogFileManager(getmytaskname(), &m_ListBox, 50, 5);
	LOG(em_MSG), "[Dlg] タスク起動");
	LOG(em_MSG), "[Dlg] 各プロセス開始処理 実行中・・・");

	mcls_pLogCull = new LogFileManager("CULL", NULL);
	LOGCULL(em_MSG), "[Dlg] タスク起動");

	//// メインインスタンス 開始処理
	mcls_MI.SetLogMgr(mcls_pLog);
	mcls_MI.SetLogMgrCull(mcls_pLogCull);
	mcls_MI.Init();
	

	//// 画面更新用データセット
	//// コンボボックスにセッション名追加
	//CString sWk;
	//for(int ii=0; ii<HT_CONNECT_NUM; ii++) {
	//	sWk.Format("%d", ii+1);
	//	m_cmbMen.AddString( sWk );
	//}
	//m_cmbMen.SetCurSel(0);				// 0番目を初期選択とする


	//-----------------------
	// 状態 リストビューインスタンス生成 & 定義
	if(true) {
		mcnt_pListSt = new DspListView(&m_ListSt);
		// 見た目定義
		m_ListSt.SetExtendedStyle(m_ListSt.GetExtendedStyle() | LVS_EX_GRIDLINES );			// 罫線描画
		m_ListSt.SetExtendedStyle(m_ListSt.GetExtendedStyle() | LVS_EX_FULLROWSELECT );		// 1行全体選択

		// 列追加			0		1			
		char name[][32] = {	"項目",	"値"};
		int  wid[]		= {	60, 	160	};
		mcnt_pListSt->SetColumnAll(sizeof(wid)/sizeof(int), 32, wid, (char const*)name);
		// 行追加
		mcnt_pListSt->SetRowAll(8);
		// 固定値セット
		mcnt_pListSt->SetItem(0, 0, "ステータス");
		mcnt_pListSt->SetItem(1, 0, "運転状態");
		mcnt_pListSt->SetItem(2, 0, "対象面");
		mcnt_pListSt->SetItem(3, 0, "ｶﾒﾗｾｯﾄ");
		mcnt_pListSt->SetItem(4, 0, "名称一覧");
		mcnt_pListSt->SetItem(5, 0, "判定機能");
		mcnt_pListSt->SetItem(6, 0, "表面状態");
		mcnt_pListSt->SetItem(7, 0, "画像形式");
	}

	//-----------------------
	// 対統括状態 リストビューインスタンス生成 & 定義
	if (true) {
		mcnt_pListTo = new DspListView(&m_ListTo);
		// 見た目定義
		m_ListTo.SetExtendedStyle(m_ListTo.GetExtendedStyle() | LVS_EX_GRIDLINES);			// 罫線描画
		m_ListTo.SetExtendedStyle(m_ListTo.GetExtendedStyle() | LVS_EX_FULLROWSELECT);		// 1行全体選択

		// 列追加			0		1			
		char name[][32] = { "項目",	"値" };
		int  wid[] = { 100, 	120 };
		mcnt_pListTo->SetColumnAll(sizeof(wid) / sizeof(int), 32, wid, (char const*)name);
		// 行追加
		mcnt_pListTo->SetRowAll(16);
		// 固定値セット
		mcnt_pListTo->SetItem(0, 0, "統括回線状態");
		mcnt_pListTo->SetItem(1, 0, "Toｷｭｰ 現/最大");
		mcnt_pListTo->SetItem(2, 0, "Eng受信FrameNo");
		mcnt_pListTo->SetItem(3, 0, "統括送信FrameNo");

		mcnt_pListTo->SetItem(4, 0, "受信間隔[ms]");
		mcnt_pListTo->SetItem(5, 0, "●全処理時間");
		mcnt_pListTo->SetItem(6, 0, "1ﾌﾚｰﾑ処理[ms]");
		mcnt_pListTo->SetItem(7, 0, "全長関連[ms]");
		mcnt_pListTo->SetItem(8, 0, "統括関連[ms]");
		mcnt_pListTo->SetItem(9, 0, "ﾌﾚｰﾑJpeg[ms]");

		mcnt_pListTo->SetItem(11, 0, "1全長回線状態");
		mcnt_pListTo->SetItem(12, 0, "1全長空き/Max");
		mcnt_pListTo->SetItem(13, 0, "2全長回線状態");
		mcnt_pListTo->SetItem(14, 0, "2全長空き/Max");
		mcnt_pListTo->SetItem(15, 0, "全長関連[ms]");
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
void CHT_DEFECTDlg::ExitProcess()
{
	//// メインインスタンス 終了処理
	KillTimer(1);			// 画面更新タイマ
	mcls_MI.Stop(20000);
	mcls_MI.Exit();

	//// ログ リストボックス表示処理終了
	mcls_pLog->SetListOut(false);					// これ以降 リストボックスへのログ表示禁止

	//// ログクラス開放
	LOG(em_MSG), "[Dlg] タスク終了");
	if(NULL != mcls_pLog){
		delete mcls_pLog;
		mcls_pLog = NULL;
	}
	LOGCULL(em_MSG), "[Dlg] タスク終了");
	if(NULL != mcls_pLogCull){
		delete mcls_pLogCull;
		mcls_pLogCull = NULL;
	}
	
	//// 画面コントロール開放
	delete mcnt_pListSt;
	delete mcnt_pListTo;


	// タスク終了処理
	task_exit();

	// 画面の終了要求
	PostMessage(WM_QUIT);
}

//------------------------------------------
// メインスレッド DlgThread
// LPVOID param ダイヤログポインタ
//------------------------------------------
unsigned WINAPI CHT_DEFECTDlg::MainThread(void* param)
{
	CHT_DEFECTDlg* p = (CHT_DEFECTDlg *)param;
	COMMON_QUE que;			// 受信構造体

	//// 各プロセス初期化処理 (これ以前はログ出力禁止)
	p->InitProcess();

	//// 起動要因取り込み ループ開始
	while(true) {
		recv_mail(&que);			// 起動要因取り込み

		switch(que.nEventNo) {

//-----------------------------------------------------------------------------------------------
		case FACT_HT_DEFECT_01:								// パラメータ変更通知 (←TO_DEFECT)
			pLOG(em_MSG), "[MAIL] パラメータ変更通知 (FACT_HT_DEFECT_01) [dly=%d, scode=%d, id=%d(0:表,1:裏,2:両面), kind=%d(1:nc,2:pd,3:pr)]",
				que.fl.data[0], que.fl.data[2], que.fl.data[1], que.fl.data[3]); 
			
			// メインインスタンスへ通知
			p->mcls_MI.SetDeliveryMail( E_DEF_ML_PARAM, &que);
			break;


//-----------------------------------------------------------------------------------------------
		case FACT_HT_DEFECT_02:								// 検査開始要求 (←TO_DEFECT)
			if(true) {
				int campair = que.fl.data[ 5 + (getpcid() -1)*2 + 0 ]; 
				int kind    = que.fl.data[ 5 + (getpcid() -1)*2 + 1 ]; 
				if(0 == campair) {										// 対象外かチェック
					pLOG(em_MSG), "[MAIL] 検査開始要求 対象外！！ (FACT_HT_DEFECT_02) [mode=%s, scode=%d:%d, pair=%d, kind=%d(1:nc,2:pd,3:pr)]",
						p->mcls_MI.gcls_Status.GetUntenName((EM_DIV_UNTEN)que.fl.data[0]), que.fl.data[1], que.fl.data[2], campair, kind); 
					break;
				}

				pLOG(em_MSG), "[MAIL] 検査開始要求 (FACT_HT_DEFECT_02) [mode=%s, scode=%d:%d, pair=%d, kind=%d(1:nc,2:pd,3:pr) len=%d:%d]",
					p->mcls_MI.gcls_Status.GetUntenName((EM_DIV_UNTEN)que.fl.data[0]), que.fl.data[1], que.fl.data[2], campair, kind, que.fl.data[3], que.fl.data[4]); 

				//// メインインスタンスへ通知
				p->mcls_MI.SetDeliveryMail( E_DEF_ML_START, &que);
			}
			break;


//-----------------------------------------------------------------------------------------------
		case FACT_HT_DEFECT_03:								// 検査停止要求 (←TO_DEFECT)
			if(true) {
				int umu = que.fl.data[ getpcid() -1]; 
				pLOG(em_MSG), "[MAIL] 検査停止要求 (FACT_HT_DEFECT_03) [%s]", 0==umu?"対象":"対象外"); 
				if(0 != umu) break;

				// メインインスタンスへ通知
				p->mcls_MI.SetDeliveryMail( E_DEF_ML_STOP, &que);
			}
			break;

//-----------------------------------------------------------------------------------------------
		case FACT_HT_DEFECT_04:								// Jpeg圧縮率設定 (←TO_DEFECT)
			pLOG(em_MSG), "[MAIL] Jpeg圧縮率設定 (FACT_HT_DEFECT_04) [JpegQuality=%d]", que.fl.data[0]); 

			// メインインスタンスへ通知
			p->mcls_MI.gcls_pSend->SetJpegQuality(que.fl.data[0]); 
			break;


//-----------------------------------------------------------------------------------------------
		case FACT_HT_DEFECT_05:								// 上位情報設定要求 (←TO_DEFECT)
			pLOG(em_MSG), "[MAIL] 上位情報設定要求 (FACT_HT_DEFECT_05) [dly=%d, size=%d] (%d/%d) scode(%d)", 
				que.fl.data[0], que.fl.data[1], que.fl.data[2], que.fl.data[3], que.fl.data[4]);

			// メインインスタンスへ通知
			p->mcls_MI.SetDeliveryMail( E_DEF_ML_COILINF, &que);
			break;


//-----------------------------------------------------------------------------------------------
		case FACT_HT_DEFECT_07:								// 続行不可能解除 (←SO_GAMENN)
			pLOG(em_MSG), "[MAIL] 続行不可能解除 (FACT_HT_DEFECT_07)"); 

			// メインインスタンスへ通知
			p->mcls_MI.SetDeliveryMail( E_DEF_ML_CANOK, &que);
			break;


//-----------------------------------------------------------------------------------------------
		case FACT_HT_DEFECT_08:								// 状態問い合わせ (←TO_DEFECT)
			pLOG(em_MSG), "[MAIL] 状態問い合わせ (FACT_HT_DEFECT_08)"); 

			// メインインスタンスへ通知
			p->mcls_MI.SetDeliveryMail( E_DEF_ML_STATUS, &que);
			break;

//-----------------------------------------------------------------------------------------------
		case FACT_HT_DEFECT_19:								// 未実行設定のキャンセル依頼 (←TO_DEFECT)
			pLOG(em_MSG), "[MAIL] 未実行設定のキャンセル依頼 (FACT_HT_DEFECT_19) [面=%d Kind=%d]", que.fl.data[0], que.fl.data[1]);

			// メインインスタンスへ通知
			p->mcls_MI.SetDeliveryMail(E_DEF_ML_CLEAR_QUEUE, &que);
			break;

//-----------------------------------------------------------------------------------------------
		case FACT_HT_DEFECT_20:								// 内部距離カウンタ設定依頼 (←TO_DEFECT)
			pLOG(em_MSG), "[MAIL] 内部距離カウンタ設定依頼 (FACT_HT_DEFECT_20) [面=%d Kind=%d]", que.fl.data[0], que.fl.data[1]);

			// メインインスタンスへ通知
			p->mcls_MI.SetDeliveryMail( E_DEF_ML_SET_POSITION, &que);
			break;

//-----------------------------------------------------------------------------------------------
		case FACT_HT_DEFECT_21:								// パラーメータ事前読込依頼 (←TO_DEFECT)
			pLOG(em_MSG), "[MAIL] パラーメータ事前読込依頼 (FACT_HT_DEFECT_21) [面=%d Kind=%d scode=%d]", que.fl.data[0], que.fl.data[1], que.fl.data[2]);  

			// メインインスタンスへ通知
			p->mcls_MI.SetDeliveryMail( E_DEF_ML_PRELOAD_PARAM, &que);
			break;


//-----------------------------------------------------------------------------------------------
		case FACT_HT_DEFECT_22:								// 途中開始要求 (←TO_DEFECT)
			if (true)
			{
				int	pcid = (getpcid() - 1);
				int	nStartFrameNo = que.fl.data[5];
				int camset1 = que.fl.data[6 + pcid * 3 + 0];
				int camset2 = que.fl.data[6 + pcid * 3 + 1];
				EM_DIV_HANTEI emKind = (EM_DIV_HANTEI)que.fl.data[6 + pcid * 3 + 2];

				if (DIV_HANTEI_PR == emKind)
				{
					// パララン判定のみ処理対象とする
					if (0 != camset1 && 0 != camset2)
					{
						// 0のまま送ると何処に対しての要求なのか分からないため、
						// -1で送信し、判定側で0に戻す
						if (-1 == camset1) que.fl.data[6 + pcid * 3 + 0] = 0;
						if (-1 == camset2) que.fl.data[6 + pcid * 3 + 1] = 0;

						pLOG(em_MSG), "[MAIL] 途中開始要求 (FACT_HT_DEFECT_22) [mode=%s, scode=%d:%d, camset1=%d, camset2=%d, frame=%d, kind=%d(1:nc,2:pd,3:pr) len=%d:%d]",
							p->mcls_MI.gcls_Status.GetUntenName((EM_DIV_UNTEN)que.fl.data[0]), que.fl.data[1], que.fl.data[2], camset1, camset2, nStartFrameNo, emKind, que.fl.data[3], que.fl.data[4]);

						// メインインスタンスへ通知
						p->mcls_MI.SetDeliveryMail(E_DEF_ML_RESUME_START, &que);
					}
					else
					{
						pLOG(em_MSG), "[MAIL] 途中開始要求 対象外！！ (FACT_HT_DEFECT_22) [mode=%s, scode=%d:%d, camset1=%d, camset2=%d, frame=%d, kind=%d(1:nc,2:pd,3:pr)]",
							p->mcls_MI.gcls_Status.GetUntenName((EM_DIV_UNTEN)que.fl.data[0]), que.fl.data[1], que.fl.data[2], camset1, camset2, nStartFrameNo, emKind);
						break;
					}
				}
			}
			break;

//-----------------------------------------------------------------------------------------------
		case FACT_HT_DEFECT_23:								// 途中停止要求 (←TO_DEFECT)
			if (true)
			{
				int umu = que.fl.data[getpcid() - 1];
				pLOG(em_MSG), "[MAIL] 途中停止要求 (FACT_HT_DEFECT_23) [%s]", 0 == umu ? "対象" : "対象外");
				if (0 != umu) break;

				// メインインスタンスへ通知
				p->mcls_MI.SetDeliveryMail(E_DEF_ML_RESUME_STOP, &que);
			}
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
void CHT_DEFECTDlg::OnTimer(UINT_PTR nIDEvent)
{
	CString cwk;
	CString ss;

	////// 現在選択されているインスタンスを取得する
	//int idx = m_cmbMen.GetCurSel();			// 0オリジン
	//if( -1 == idx) return;					// 設定無し
	int idx = 0;

	//// ステータス情報
	mcnt_pListSt->SetItem(0, 1, mcls_MI.gcls_Status.GetMainStateName());
	mcnt_pListSt->SetItem(1, 1, mcls_MI.gcls_Status.GetUntenName());
	mcnt_pListSt->SetItem(2, 1, DivNameManager::GetTorB(mcls_MI.GetMen()));	// 2   対象面

	EM_DIV_HANTEI	emKindHt = mcls_MI.GetKindHt();
	ss = DIV_HANTEI_HT == emKindHt ? "判定機能" : "周期機能";
	ss = (DIV_HANTEI_PR == emKindHt ? "パララン機能" : ss);
	mcnt_pListSt->SetItem(5, 1, ss);

	mcnt_pListSt->SetItem(6, 1, mcls_MI.GetNowScode(0));
	
	ss = mcls_MI.gcls_pSend->GetJpegQuality() == 0 ? "BMP" : "JPEG圧縮";
	mcnt_pListSt->SetItem(7, 1, ss);

	// カメラセット
	ss = "";
	cwk = "";
	int	nCamPair = mcls_MI.GetCamPair();
	if (0 < nCamPair)
	{
		// パララン判定の場合、カメラペアではなくカメラセットで指示が来るため
		// nCamPairは自PCが有効であるか否かの判断にのみ使用
		// カメラセットは先頭固定で格納してあるので、この時点で０をセットし文字列取得する
		if (DIV_HANTEI_PR == emKindHt) nCamPair = 0;

		int	nCamNum = mcls_MI.gcls_Status.GetCamPairItemNum(nCamPair, mcls_MI.GetKindHt());
		for (int ii = 0; ii < nCamNum; ii++)
		{
			int nCamSet = mcls_MI.gcls_Status.GetCamPairItemCamset(nCamPair, ii, emKindHt);
			ss.AppendFormat("%d/ ", nCamSet);
			if (DIV_HANTEI_PR != emKindHt)
			{
				// 単発判定の場合、カメラセット０はバックアップを意味する
				cwk.AppendFormat("%s/ ", DivNameManager::GetDivCamSet(nCamSet));
			}
			else
			{
				// パララン判定の場合、カメラセット０は無効カメラの選択を意味する
				cwk.AppendFormat("%s/ ", (0 != nCamSet ? DivNameManager::GetDivCamSet(nCamSet) : "対象外"));
			}
		}
	}
	mcnt_pListSt->SetItem(3, 1, ss);
	mcnt_pListSt->SetItem(4, 1, cwk);


	//// 処理状態
	mcnt_pListTo->SetItem(0, 1, mcls_MI.gcls_pSock[idx]->GetSockStateName());
	ss.Format("%d (%d)", mcls_MI.gcls_pSock[idx]->gque_Deli.GetCount(), mcls_MI.gcls_pSock[idx]->gque_Deli.GetMaxCount());	mcnt_pListTo->SetItem(1, 1, ss);
	mcnt_pListTo->SetItem(2, 1, mcls_MI.gcls_pSend->GetDspRecvFrameNo());
	mcnt_pListTo->SetItem(3, 1, mcls_MI.gcls_pSend->GetDspFrameNo());

	mcnt_pListTo->SetItem(4, 1, mcls_MI.gcls_pSend->GetTimeIntval());
	ss.Format("%d (%d)", mcls_MI.gcls_pSend->GetTimeAllNow(), mcls_MI.gcls_pSend->GetTimeAllMax());	mcnt_pListTo->SetItem(5, 1, ss);
	ss.Format("%d (%d)", mcls_MI.gcls_pSend->g_nTime_All[0], mcls_MI.gcls_pSend->g_nTime_All[1]);	mcnt_pListTo->SetItem(6, 1, ss);
	ss.Format("%d (%d)", mcls_MI.gcls_pSend->g_nTime_Rpi[0], mcls_MI.gcls_pSend->g_nTime_Rpi[1]);	mcnt_pListTo->SetItem(7, 1, ss);
	ss.Format("%d (%d)", mcls_MI.gcls_pSend->g_nTime_To[0], mcls_MI.gcls_pSend->g_nTime_To[1]);	mcnt_pListTo->SetItem(8, 1, ss);
	ss.Format("%d (%d)", mcls_MI.gcls_pSend->g_nTime_ToFrame[0], mcls_MI.gcls_pSend->g_nTime_ToFrame[1]);	mcnt_pListTo->SetItem(9, 1, ss);

#ifdef FRAME_RAW_SEND
	for (int ii=0; ii<FRAME_HT_SEND_PORT; ii++) {
		mcnt_pListTo->SetItem(11+2*ii, 1, mcls_MI.gcls_pSockRpi[ii]->GetSockStateName());
		ss.Format("%d (%d)", mcls_MI.gque_pSockRpi[ii]->GetCountFree(), mcls_MI.gque_pSockRpi[ii]->GetMaxCountPool());	mcnt_pListTo->SetItem(12+ii*2, 1, ss);
	}

	double dAve = (mcls_MI.gcls_pCull[0]->g_nTime_Rpi[0] + mcls_MI.gcls_pCull[1]->g_nTime_Rpi[0]) / 2;
	double dMax = max(mcls_MI.gcls_pCull[0]->g_nTime_Rpi[1], mcls_MI.gcls_pCull[1]->g_nTime_Rpi[1]);
	ss.Format("%.0f (%.0f)", dAve, dMax);	mcnt_pListTo->SetItem(15, 1, ss);
#endif



	CDialog::OnTimer(nIDEvent);
}

// これ以降 デバック用コマンド -------------------->>>
//￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣
// 検査開始
//＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿
void CHT_DEFECTDlg::OnBnClickedDebugStart()
{
	if ( BST_CHECKED != CWnd::IsDlgButtonChecked(IDC_DEBUG_MANUAL) ) return;

	int pcid = getpcid() -1;

	EM_DIV_UNTEN emMode = DIV_UNTEN_NOMAL;
	if ( BST_CHECKED == CWnd::IsDlgButtonChecked( IDC_DEBUG_START_MODE1 ) ) emMode = DIV_UNTEN_NOMAL;
	if ( BST_CHECKED == CWnd::IsDlgButtonChecked( IDC_DEBUG_START_MODE2 ) ) emMode = DIV_UNTEN_SAMP;
	if ( BST_CHECKED == CWnd::IsDlgButtonChecked( IDC_DEBUG_START_MODE3 ) ) emMode = DIV_UNTEN_CAMERA;
	if ( BST_CHECKED == CWnd::IsDlgButtonChecked( IDC_DEBUG_START_MODE4 ) ) emMode = DIV_UNTEN_SMYU;

	EM_DIV_HANTEI emKind= DIV_HANTEI_BK;
	if ( BST_CHECKED == CWnd::IsDlgButtonChecked( IDD_DEBUG_KIND1 ) ) emKind = DIV_HANTEI_HT;
	if ( BST_CHECKED == CWnd::IsDlgButtonChecked( IDD_DEBUG_KIND2 ) ) emKind = DIV_HANTEI_CY;
	if ( BST_CHECKED == CWnd::IsDlgButtonChecked( IDD_DEBUG_KIND3 ) ) emKind = DIV_HANTEI_PR;

	COMMON_QUE	que;
	que.nEventNo		= FACT_HT_DEFECT_02;
	que.nLineNo			= getlineid();
	que.fl.data[0]		= (int)emMode;
	que.fl.data[1]		= GetDlgItemInt(IDC_DEBUG_SCODE);
	que.fl.data[2]		= GetDlgItemInt(IDC_DEBUG_SCODE);
	que.fl.data[3]		= 0;
	que.fl.data[4]		= 0;

	que.fl.data[5+ 2*pcid + 0] = GetDlgItemInt(IDC_DEBUG_CAMPAIR);	// 自分のところだけセット
	que.fl.data[5+ 2*pcid + 1] = (int)emKind;

	send_mail(HT_DEFECT, ".", &que);
}
//￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣
// 検査停止
//＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿
void CHT_DEFECTDlg::OnBnClickedDebugStop()
{
	if ( BST_CHECKED != CWnd::IsDlgButtonChecked(IDC_DEBUG_MANUAL) ) return;
	COMMON_QUE que;
	memset(&que, 0x00, sizeof(que));
	que.nEventNo = FACT_HT_DEFECT_03;
	que.nLineNo = getlineid();

	send_mail(HT_DEFECT, ".", &que);
}
//￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣
// パラメータ変更
//＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿
void CHT_DEFECTDlg::OnBnClickedDebugParam()
{
	if ( BST_CHECKED != CWnd::IsDlgButtonChecked(IDC_DEBUG_MANUAL) ) return;
	COMMON_QUE que;
	que.nEventNo = FACT_HT_DEFECT_01;
	que.nLineNo = getlineid();
	que.fl.data[0] = 0;						// 遅延実行無し
	que.fl.data[1] = 2;						// 両面
	que.fl.data[2] = CH_SCODE_ALL;			// 全部

	EM_DIV_HANTEI emKind= DIV_HANTEI_BK;
	if ( BST_CHECKED == CWnd::IsDlgButtonChecked( IDD_DEBUG_KIND1 ) ) emKind = DIV_HANTEI_HT;
	if ( BST_CHECKED == CWnd::IsDlgButtonChecked( IDD_DEBUG_KIND2 ) ) emKind = DIV_HANTEI_CY;
	if ( BST_CHECKED == CWnd::IsDlgButtonChecked( IDD_DEBUG_KIND3 ) ) emKind = DIV_HANTEI_PR;
	que.fl.data[3]	= (int)emKind;

	send_mail(HT_DEFECT, ".", &que);	
}

//￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣
// 上位情報設定要求
//＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿
void CHT_DEFECTDlg::OnBnClickedDebugCoilinf()
{
	if ( BST_CHECKED != CWnd::IsDlgButtonChecked(IDC_DEBUG_MANUAL) ) return;
	COMMON_QUE que;
	que.nEventNo = FACT_HT_DEFECT_05;
	que.nLineNo = getlineid();

	TYPE_EPC_COIL wk;
	memset(&wk, 0x00, sizeof(wk));
	// 擬似データセット
	memcpy(wk.cCoilNo, "1234567890", PRO_SIZE_COIL_NO);
	memcpy(wk.cMeiNo, "123 ", PRO_SIZE_ORDER_NO);

	wk.nAtuSei = 1100;
	wk.nWidZai = 1000;
	wk.nLenIn = 3000*1000;

	// セット完了
	int nCnt = GET_JUST_INDEX( sizeof(TYPE_EPC_COIL) , DIVCOIL_ONE_SIZE_SEND) + 1;
	for (int ii = 0; ii < nCnt; ii++) {
		int nSize = sizeof(TYPE_EPC_COIL) - (DIVCOIL_ONE_SIZE_SEND*ii);
		if (nSize>DIVCOIL_ONE_SIZE_SEND) nSize = DIVCOIL_ONE_SIZE_SEND;
		
		que.fl.data[0] = 0;				// 0固定。
		que.fl.data[1] = nSize;
		que.fl.data[2] = ii+1;
		que.fl.data[3] = nCnt;
		for(int jj=0; jj<NUM_MEN; jj++) que.fl.data[4+jj] = MAX_SCODE;
		memcpy(&que.fl.data[4+NUM_MEN+1], &((BYTE*)&wk)[ii*DIVCOIL_ONE_SIZE_SEND], nSize);
		send_mail(HT_DEFECT, ".", &que);
	}
}

//￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣
// 遅延実行キャンセル
//＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿
//void CHT_DEFECTDlg::OnBnClickedDebugDlycancel()
//{
//	if ( BST_CHECKED != CWnd::IsDlgButtonChecked(IDC_DEBUG_MANUAL) ) return;
//	COMMON_QUE que;
//	que.nEventNo = FACT_HT_DEFECT_06;
//	que.nLineNo = getlineid();
//	send_mail(HT_DEFECT, ".", &que);
//}
//￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣
// 続行不可能解除
//＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿
void CHT_DEFECTDlg::OnBnClickedDebugCanok()
{
	if ( BST_CHECKED != CWnd::IsDlgButtonChecked(IDC_DEBUG_MANUAL) ) return;
	COMMON_QUE que;
	que.nEventNo = FACT_HT_DEFECT_07;
	que.nLineNo = getlineid();
	send_mail(HT_DEFECT, ".", &que);
}
//￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣
// フレーム画像取得要求
//＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿
//void CHT_DEFECTDlg::OnBnClickedDebugFrame()
//{
//	if ( BST_CHECKED != CWnd::IsDlgButtonChecked(IDC_DEBUG_MANUAL) ) return;
//	COMMON_QUE que;
//	que.nEventNo	= FACT_HT_DEFECT_09;
//	que.nLineNo		= getlineid();
//	que.fl.data[0]	= 0;
//	que.fl.data[1]	= GetDlgItemInt(IDC_DEBUG_CAMPAIR);
//	que.fl.data[2]	= 2;
//	send_mail(HT_DEFECT, ".", &que);
//}

//￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣
// 健全性評価データ収集要求
//＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿
//void CHT_DEFECTDlg::OnBnClickedDebugProfile()
//{
//	if ( BST_CHECKED != CWnd::IsDlgButtonChecked(IDC_DEBUG_MANUAL) ) return;
//	COMMON_QUE que;
//	que.nEventNo	= FACT_HT_DEFECT_11;
//	que.nLineNo		= getlineid();
//	int pcid = getpcid() -1;
//	que.fl.data[pcid]	= 0;
//	send_mail(HT_DEFECT, ".", &que);
//}


//￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣
// 画像圧縮
//＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿
void CHT_DEFECTDlg::OnBnClickedDebugImgkind()
{
	if ( BST_CHECKED != CWnd::IsDlgButtonChecked(IDC_DEBUG_MANUAL) ) return;
	COMMON_QUE que;
	que.nEventNo	= FACT_HT_DEFECT_04;
	que.nLineNo		= getlineid();
	que.fl.data[0]	= GetDlgItemInt(IDC_DEBUG_JPEGQUALITY);
	send_mail(HT_DEFECT, ".", &que);
}


//￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣
// 内部距離カウンタ設定
//＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿
void CHT_DEFECTDlg::OnBnClickedDebugSetPosition()
{
	if ( BST_CHECKED != CWnd::IsDlgButtonChecked(IDC_DEBUG_MANUAL) ) return;
	COMMON_QUE que;
	que.nEventNo	= FACT_HT_DEFECT_20;
	que.nLineNo		= getlineid();
	que.fl.data[0]	= 2;
	que.fl.data[1]	= -1;

	NCL_CMD_SET_POSITION& wk = (NCL_CMD_SET_POSITION&)que.fl.data[2];
	memset(&wk, 0x00, sizeof(NCL_CMD_SET_POSITION));

	wk.cmd = NCL_SET_POSITION;
	wk.framenum = 0;
	wk.update_mask = 0xFFFFFFFF;
	wk.front_pos = 1000;

	send_mail(HT_DEFECT, ".", &que);
}

//￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣
// パラメータ事前読込
//＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿
void CHT_DEFECTDlg::OnBnClickedDebugParamPreload()
{
	if ( BST_CHECKED != CWnd::IsDlgButtonChecked(IDC_DEBUG_MANUAL) ) return;
	COMMON_QUE que;
	que.nEventNo	= FACT_HT_DEFECT_21;
	que.nLineNo		= getlineid();
	que.fl.data[0]	= 2;
	que.fl.data[1]	= -1;
	que.fl.data[2]	= 0;
	send_mail(HT_DEFECT, ".", &que);
}

//￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣
// 判定区分の変更（単発）
//＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿
void CHT_DEFECTDlg::OnBnClickedDebugKind1()
{
	CWnd::GetDlgItem(IDC_DEBUG_CAMPAIR)->EnableWindow(TRUE);
	CWnd::GetDlgItem(IDC_DEBUG_CAMSET1)->EnableWindow(FALSE);
	CWnd::GetDlgItem(IDC_DEBUG_CAMSET2)->EnableWindow(FALSE);
}

//￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣
// 判定区分の変更（周期）
//＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿
void CHT_DEFECTDlg::OnBnClickedDebugKind2()
{
	CWnd::GetDlgItem(IDC_DEBUG_CAMPAIR)->EnableWindow(TRUE);
	CWnd::GetDlgItem(IDC_DEBUG_CAMSET1)->EnableWindow(FALSE);
	CWnd::GetDlgItem(IDC_DEBUG_CAMSET2)->EnableWindow(FALSE);
}

//￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣
// 判定区分の変更（パララン）
//＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿
void CHT_DEFECTDlg::OnBnClickedDebugKind3()
{
	CWnd::GetDlgItem(IDC_DEBUG_CAMPAIR)->EnableWindow(FALSE);
	CWnd::GetDlgItem(IDC_DEBUG_CAMSET1)->EnableWindow(TRUE);
	CWnd::GetDlgItem(IDC_DEBUG_CAMSET2)->EnableWindow(TRUE);
}

//￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣
// 途中開始
//＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿
void CHT_DEFECTDlg::OnBnClickedDebugResumeStart()
{
	if (BST_CHECKED != CWnd::IsDlgButtonChecked(IDC_DEBUG_MANUAL)) return;

	int pcid = getpcid() - 1;

	EM_DIV_UNTEN emMode = DIV_UNTEN_NOMAL;
	if (BST_CHECKED == CWnd::IsDlgButtonChecked(IDC_DEBUG_START_MODE1)) emMode = DIV_UNTEN_NOMAL;
	if (BST_CHECKED == CWnd::IsDlgButtonChecked(IDC_DEBUG_START_MODE2)) emMode = DIV_UNTEN_SAMP;
	if (BST_CHECKED == CWnd::IsDlgButtonChecked(IDC_DEBUG_START_MODE3)) emMode = DIV_UNTEN_CAMERA;
	if (BST_CHECKED == CWnd::IsDlgButtonChecked(IDC_DEBUG_START_MODE4)) emMode = DIV_UNTEN_SMYU;

	EM_DIV_HANTEI emKind = DIV_HANTEI_BK;
	if (BST_CHECKED == CWnd::IsDlgButtonChecked(IDD_DEBUG_KIND1)) emKind = DIV_HANTEI_HT;
	if (BST_CHECKED == CWnd::IsDlgButtonChecked(IDD_DEBUG_KIND2)) emKind = DIV_HANTEI_CY;
	if (BST_CHECKED == CWnd::IsDlgButtonChecked(IDD_DEBUG_KIND3)) emKind = DIV_HANTEI_PR;

	COMMON_QUE	que;
	que.nEventNo = FACT_HT_DEFECT_22;
	que.nLineNo = getlineid();
	que.fl.data[0] = (int)emMode;
	que.fl.data[1] = GetDlgItemInt(IDC_DEBUG_SCODE);
	que.fl.data[2] = GetDlgItemInt(IDC_DEBUG_SCODE);
	que.fl.data[3] = 0;
	que.fl.data[4] = 0;
	que.fl.data[5] = NUM_JOIN_FRAME;
	
	if (DIV_HANTEI_PR != emKind)
	{
		// 単発・周期
		que.fl.data[6 + 3 * pcid + 0] = GetDlgItemInt(IDC_DEBUG_CAMPAIR);
		que.fl.data[6 + 3 * pcid + 1] = 0;
	}
	else
	{
		// ロジックパララン
		que.fl.data[6 + 3 * pcid + 0] = GetDlgItemInt(IDC_DEBUG_CAMSET1);
		que.fl.data[6 + 3 * pcid + 1] = GetDlgItemInt(IDC_DEBUG_CAMSET2);
	}
	que.fl.data[6 + 3 * pcid + 2] = (int)emKind;

	send_mail(HT_DEFECT, ".", &que);
}

//￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣
// 途中停止
//＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿
void CHT_DEFECTDlg::OnBnClickedDebugResumeStop()
{
	if (BST_CHECKED != CWnd::IsDlgButtonChecked(IDC_DEBUG_MANUAL)) return;
	COMMON_QUE que;
	memset(&que, 0x00, sizeof(que));
	que.nEventNo = FACT_HT_DEFECT_23;
	que.nLineNo = getlineid();

	send_mail(HT_DEFECT, ".", &que);
}
