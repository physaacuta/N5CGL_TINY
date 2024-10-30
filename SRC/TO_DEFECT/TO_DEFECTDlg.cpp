
// TO_DEFECTDlg.cpp : 実装ファイル
//

#include "stdafx.h"
#include "TO_DEFECT.h"
#include "TO_DEFECTDlg.h"
#include "afxdialogex.h"
#include "LogicParaRunFunc.h"											// ロジックパララン関連処理モジュール

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


// CTO_DEFECTDlg ダイアログ



CTO_DEFECTDlg::CTO_DEFECTDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_TO_DEFECT_DIALOG, pParent)
	, m_nParaKind(0)
	, m_nCamsetResult(0)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CTO_DEFECTDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, m_ListBox);
	DDX_Control(pDX, IDC_LIST_ST, m_ListSt);
	DDX_Control(pDX, IDC_LIST_SYS, m_ListSys);
	DDX_Control(pDX, IDC_LIST_HT, m_ListHt);
	DDX_Control(pDX, IDC_LIST_COIL, m_ListCoil);
	DDX_Control(pDX, IDC_LIST_PLG_KEN, m_ListPlgKen);
	DDX_Control(pDX, IDC_LIST_PLG_DSP, m_ListPlgDsp);
	DDX_Control(pDX, IDC_LIST_PLG, m_ListPlg);
	DDX_Control(pDX, IDC_LIST_DB, m_ListDb);
	DDX_Control(pDX, IDC_LIST_DBINFO, m_ListDbInfo);
	DDX_Control(pDX, IDC_LIST_OP, m_ListOp);
	DDX_Control(pDX, IDC_LIST_EDGE, m_ListEdge);
	DDX_Control(pDX, IDC_LIST_KIKI, m_ListKiki);
	DDX_Control(pDX, IDC_LIST_SETU_TR, m_ListSetuDsp);
	DDX_Control(pDX, IDC_LIST_HTTIME, m_ListHtTime);
	DDX_Control(pDX, IDC_LIST_RAMP, m_ListRamp);
	DDX_Control(pDX, IDC_LIST_PATO, m_ListPato);
	DDX_Control(pDX, IDC_LIST_VOICE, m_ListVoice);
	DDX_Control(pDX, IDC_LIST_PR, m_ListPr);
	DDX_Radio(pDX, IDC_BTN_PARA1, m_nParaKind);
	DDX_Radio(pDX, IDC_BTN_CAMSET_RESULT1, m_nCamsetResult);
}

BEGIN_MESSAGE_MAP(CTO_DEFECTDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDOK, &CTO_DEFECTDlg::OnBnClickedOk)
	ON_WM_TIMER()
	ON_LBN_DBLCLK(IDC_LIST1, &CTO_DEFECTDlg::OnLbnDblclkList1)
	ON_NOTIFY(NM_CLICK, IDC_LIST_CMD, &CTO_DEFECTDlg::OnNMClickListCmd)
	ON_BN_CLICKED(IDC_SMYU_PIO, &CTO_DEFECTDlg::OnBnClickedSmyuPio)
	ON_BN_CLICKED(IDC_SMYU_PLG, &CTO_DEFECTDlg::OnBnClickedSmyuPlg)
	ON_BN_CLICKED(IDC_DEBUG_LEDONOFF, &CTO_DEFECTDlg::OnBnClickedDebugLedonoff)
	ON_NOTIFY(NM_CLICK, IDC_LIST_INPUT, &CTO_DEFECTDlg::OnNMClickListInput)
END_MESSAGE_MAP()


// CTO_DEFECTDlg メッセージ ハンドラー

BOOL CTO_DEFECTDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// このダイアログのアイコンを設定します。アプリケーションのメイン ウィンドウがダイアログでない場合、
	//  Framework は、この設定を自動的に行います。
	SetIcon(m_hIcon, TRUE);			// 大きいアイコンの設定
	SetIcon(m_hIcon, FALSE);		// 小さいアイコンの設定

	// TODO: 初期化をここに追加します。
	
// 画面初期化セット----------------------------------------->>>
#ifdef LOCAL
	CWnd::CheckDlgButton(IDC_DEBUG_MANUAL, 1);
#else
	CWnd::CheckDlgButton(IDC_DEBUG_MANUAL, 1);
#endif
	//// デバッグ項目　初期設定
	CString ss;

	CWnd::CheckRadioButton(IDC_RAD_DEBUG_MEN1, IDC_RAD_DEBUG_MEN2, IDC_RAD_DEBUG_MEN1);			// 片面検査不許可
	CWnd::CheckRadioButton(IDC_RAD_DEBUG_UNTEN1, IDC_RAD_DEBUG_UNTEN4, IDC_RAD_DEBUG_UNTEN1);	// 通常運転モード
	CWnd::CheckRadioButton(IDC_RAD_DEBUG_PLG1, IDC_RAD_DEBUG_PLG4, IDC_RAD_DEBUG_PLG1);			// ラインPLG

	ss.Format("%d", MAX_SCODE);
	CWnd::SetDlgItemTextA(IDC_DEF_SCODE1, ss);
	CWnd::SetDlgItemTextA(IDC_DEF_SCODE2, ss);

	ss.Format("%d", MAX_PCODE);
	CWnd::SetDlgItemTextA(IDC_DEF_PCODE1, ss);
	CWnd::SetDlgItemTextA(IDC_DEF_PCODE2, ss);

	//-----------------------
	// コマンド ボタン形式のリストビュー
	if (true) {
		// 定義
		mlst_Cmd.SubclassWindow(*GetDlgItem(IDC_LIST_CMD));
		mcnt_pListCmd = new DspListView(&mlst_Cmd);
		// 見た目定義
		//	mlst_Cmd.SetExtendedStyle( mlst_Cmd.GetExtendedStyle() | LVS_EX_GRIDLINES );			// 罫線描画
		//	mlst_Cmd.SetExtendedStyle( mlst_Cmd.GetExtendedStyle() | LVS_EX_FULLROWSELECT );		// 1行全体選択
		// 列追加			0						
		char name[][32] = { "操作(ｸﾘｯｸして)" };
		int  wid[] = { 148 };
		mcnt_pListCmd->SetColumnAll(sizeof(wid) / sizeof(int), 32, wid, (char const*)name);

		// 行高さ (=ボタン高さ)
		int nHeight = 16;
		CImageList image;
		image.Create(1, nHeight, ILC_COLOR, 0, 0);
		mlst_Cmd.SetImageList(&image, LVSIL_STATE);

		// ボタン項目
		for (int ii = 0; ii<E_DEBUG_CMD_END; ii++) {
			mlst_Cmd.InsertItem(ii, DebugCmd[ii]);
		}
		mlst_Cmd.SyncButtons(0);			// 0列目固定		

	}

	//-----------------------
	// コマンド ボタン形式のリストビュー
	if (true) {
		// 定義
		mlst_Input.SubclassWindow(*GetDlgItem(IDC_LIST_INPUT));
		mcnt_pListInput = new DspListView(&mlst_Input);
		// 見た目定義
		//mlst_Input.SetExtendedStyle( mlst_Input.GetExtendedStyle() | LVS_EX_GRIDLINES );			// 罫線描画
		//mlst_Input.SetExtendedStyle( mlst_Input.GetExtendedStyle() | LVS_EX_FULLROWSELECT );		// 1行全体選択
		// 列追加			0						
		char name[][32] = { "操作(ｸﾘｯｸして)" };
		int  wid[] = { 148 };
		mcnt_pListInput->SetColumnAll(sizeof(wid) / sizeof(int), 32, wid, (char const*)name);

		// 行高さ (=ボタン高さ)
		int nHeight = 16;
		CImageList image;
		image.Create(1, nHeight, ILC_COLOR, 0, 0);
		mlst_Input.SetImageList(&image, LVSIL_STATE);

		// ボタン項目
		for (int ii = 0; ii<E_DEBUG_INPUT_END; ii++) {
			mlst_Input.InsertItem(ii, DebugInput[ii]);
		}

		mlst_Input.SyncButtons(0);			// 0列目固定		

		m_bInit = false;
	}

	// <<<---------------------------------------------------------

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

void CTO_DEFECTDlg::OnPaint()
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
HCURSOR CTO_DEFECTDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



//￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣
// ×ボタン
//＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿
void CTO_DEFECTDlg::OnClose()
{
	//CDialogEx::OnClose();
}

//￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣
// 強制終了ボタン
//＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿
void CTO_DEFECTDlg::OnBnClickedOk()
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
void CTO_DEFECTDlg::OnLbnDblclkList1()
{
	// ListBoxコントロールのプロパティは、ソート禁止にしておく事
	// ListBoxコントロールのダブルクリックイベントを追加
	mcls_pLog->ShowEdit();
}

//------------------------------------------
// 各プロセス初期化処理
//------------------------------------------
void CTO_DEFECTDlg::InitProcess()
{
	CString sWk;

	//// 一番初めにいろいろ初期化
	mcls_pLog = NULL;
	mcls_pLogSeq = NULL;
	mcls_pLogE = NULL;
	mcls_pLogG = NULL;
	mcls_pLogD = NULL;
	mcls_pLogA = NULL;

	//// タスク初期化処理
	int nRetc = task_init(TO_DEFECT);
	if (0 != nRetc) {
		if (-1 == nRetc) { syslog(SYSNO_TASK_INIT_ERR, "[%s] 既にタスク起動済みにより、起動中断。", TO_DEFECT); }
		else { syslog(SYSNO_TASK_INIT_ERR, "[%s]", TO_DEFECT); }
		_ASSERT(0);
		exit(-9);		// 自プロセス強制終了
		return;
	}

	//// LogFileManagerインスタンス生成 (これ以降 ログ出力可能)
	setlocale(LC_ALL, "Japanese");	// これが無いと日本語パスを含むフォルダの場合、保存できない。(ライブラリの中で実行するべきか迷っているが、変更するとタスク全体に影響するので、とりあえずここで設定)
	mcls_pLog = new LogFileManager(getmytaskname(), &m_ListBox);
	LOG(em_MSG), "[Dlg] タスク起動");
	LOG(em_MSG), "[Dlg] 各プロセス開始処理 実行中・・・");

	//// 画面に出さないログ関係
	mcls_pLogSeq = new LogFileManager("SeqUDP受信", NULL, 5, 5);
	LOGSEQ(em_MSG), "[Dlg] タスク起動");

	mcls_pLogE = new LogFileManager("EdgeUDP", NULL);
	LOGE(em_MSG), "[Dlg] タスク起動");

	mcls_pLogG = new LogFileManager("GuidanceLog", NULL);
	LOGG(em_MSG), "[Dlg] タスク起動");

	mcls_pLogD = new LogFileManager("HtCheckD", NULL);
	LOGD(em_MSG), "[Dlg] タスク起動");

	mcls_pLogA = new LogFileManager("AlarmLog", NULL);
	LOGA(em_MSG), "[Dlg] タスク起動");


	//-----------------------
	// ステータス状態 リストビューインスタンス生成 & 定義
	if (true) {
		mcnt_pListSt = new DspListView(&m_ListSt);
		// 見た目定義
		m_ListSt.SetExtendedStyle(m_ListSt.GetExtendedStyle() | LVS_EX_GRIDLINES);			// 罫線描画
		m_ListSt.SetExtendedStyle(m_ListSt.GetExtendedStyle() | LVS_EX_FULLROWSELECT);		// 1行全体選択

		// 列追加			0		1			
		char name[][32] = { "項目",	"値" };
		int  wid[] = { 70, 	160 };
		mcnt_pListSt->SetColumnAll(sizeof(wid) / sizeof(int), 32, wid, (char const*)name);
		// 行追加
		mcnt_pListSt->SetRowAll(6);
		// 固定値セット
		mcnt_pListSt->SetItem(0, 0, "稼動状態");
		mcnt_pListSt->SetItem(1, 0, "操業状態検");
#ifdef DSP_SOGYO_CHECK
		mcnt_pListSt->SetItem(2, 0, "操業状態表");
#endif
		mcnt_pListSt->SetItem(3, 0, "運転状態");
		mcnt_pListSt->SetItem(4, 0, "PLG区分");
		mcnt_pListSt->SetItem(5, 0, "検査条件");
	}

	//-----------------------
	// システム状態 リストビューインスタンス生成 & 定義
	if (true) {
		mcnt_pListSys = new DspListView(&m_ListSys);
		// 見た目定義
		m_ListSys.SetExtendedStyle(m_ListSys.GetExtendedStyle() | LVS_EX_GRIDLINES);			// 罫線描画
		m_ListSys.SetExtendedStyle(m_ListSys.GetExtendedStyle() | LVS_EX_FULLROWSELECT);		// 1行全体選択

		// 列追加			0		1			
		char name[][32] = { "項目",	"表面",	"裏面" };
		int  wid[] = { 80, 	75,		75 };
		mcnt_pListSys->SetColumnAll(sizeof(wid) / sizeof(int), 32, wid, (char const*)name);
		// 行追加
		mcnt_pListSys->SetRowAll(4);
		// 固定値セット
		mcnt_pListSys->SetItem(0, 0, "HTｼｽﾃﾑ状態");
		mcnt_pListSys->SetItem(1, 0, "検査状態");
		mcnt_pListSys->SetItem(2, 0, "機器状態");
		mcnt_pListSys->SetItem(3, 0, "処理時間");
	}


	//// 判定関係 リストビューインスタンス生成 & 定義
	if (true) {		// 単発
		mcnt_pListHt = new DspListView(&m_ListHt);
		// 見た目定義
		m_ListHt.SetExtendedStyle(m_ListHt.GetExtendedStyle() | LVS_EX_GRIDLINES);			// 罫線描画
		m_ListHt.SetExtendedStyle(m_ListHt.GetExtendedStyle() | LVS_EX_FULLROWSELECT);		// 1行全体選択

#ifndef HT_IS_COLOR
		// 列追加			0		1		2		3		4			
		char name[][32] = { "機能",	"PCID",	"回線",	"状態",	"FrameNo", "Edge",	"正", "", "乱", "" };
		int  wid[] = { 52,		40,		36,		36,		62,			45,		32,	  18,	32,	18 };
#else
		// 列追加			0		1		2		3		4			
		char name[][32] = {	"HT機能",	"PCID",	"回線",	"状態",	"FrameNo", 	"R", "輝", "露", "ゲ",  "G", "輝", "露", "ゲ", "B", "輝", "露", "ゲ"};
		int  wid[]		= {	52,			40,		36,		36,		62,			 18,   32,   32,   32,   18,   32,   32,   32,  18,   32,   32,  32};

#endif
		mcnt_pListHt->SetColumnAll(sizeof(wid) / sizeof(int), 32, wid, (char const*)name);
		// 行追加
		mcnt_pListHt->SetRowAll(NUM_CAM_POS*NUM_MEN);
		// 固定値セット
		int nRow = 0;
		for (int ii = 1; ii <= MAX_CAMSET; ii++) {
			mcnt_pListHt->SetItem(nRow, 0, DivNameManager::GetDivCamSet(ii));		// 機能
			nRow++;
		}
	}
//	if (true) {		// 周期
//		mcnt_pListCy = new DspListView(&m_ListCy);
//		// 見た目定義
//		m_ListCy.SetExtendedStyle(m_ListHt.GetExtendedStyle() | LVS_EX_GRIDLINES);			// 罫線描画
//		m_ListCy.SetExtendedStyle(m_ListHt.GetExtendedStyle() | LVS_EX_FULLROWSELECT);		// 1行全体選択
//
//#ifndef HT_IS_COLOR
//		// 列追加			0		1		2		3		4			
//		char name[][32] = { "機能",	"PCID",	"回線",	"状態",	"FrameNo", "正", "", "乱", "" };
//		int  wid[] = { 52,		40,		36,		36,		62,			32,	  18,	32,	18 };
//#else
//		// 列追加			0		1		2		3		4			
//		char name[][32] = { "CY機能",	"PCID",	"回線",	"状態",	"FrameNo" };
//		int  wid[] = { 52,			40,		36,		36,		62 };
//
//#endif
//		mcnt_pListCy->SetColumnAll(sizeof(wid) / sizeof(int), 32, wid, (char const*)name);
//		// 行追加
//		mcnt_pListCy->SetRowAll(NUM_CAM_POS*NUM_MEN);
//		// 固定値セット
//		int nRow = 0;
//		for (int ii = 1; ii <= MAX_CAMSET; ii++) {
//			mcnt_pListCy->SetItem(nRow, 0, DivNameManager::GetDivCamSet(ii));		// 機能
//			nRow++;
//		}
//	}
	if (true) {		// パララン
		mcnt_pListPr = new DspListView(&m_ListPr);
		
		// 見た目定義
		m_ListPr.SetExtendedStyle(m_ListHt.GetExtendedStyle() | LVS_EX_GRIDLINES);			// 罫線描画
		m_ListPr.SetExtendedStyle(m_ListHt.GetExtendedStyle() | LVS_EX_FULLROWSELECT);		// 1行全体選択
		
		char name[][32] = { "機能", "PCID", "状態", "c1", "c2", "ｼｽﾃﾑ状態" };
		int  wid[]		= {     45,		38,		36,	  25,	25,		  67 };

		mcnt_pListPr->SetColumnAll(sizeof(wid) / sizeof(int), 32, wid, (char const*)name);
		// 行追加
		mcnt_pListPr->SetRowAll(NUM_ALL_HTPARA);
		// 固定値セット
		int nRow = 0;
		CString sWk;
		for (int ii = 0; ii < NUM_ALL_HTPARA; ii++)
		{
			sWk.Format("ﾊﾟﾗﾗﾝ%d", ii + 1);
			mcnt_pListPr->SetItem(nRow, 0, sWk);			// 機能
			nRow++;
		}
	}
	//// 判定処理時間関係 リストビューインスタンス生成 & 定義
	if (true) {
		mcnt_pListHtTime = new DspListView(&m_ListHtTime);
		m_ListHtTime.SetExtendedStyle(m_ListHtTime.GetExtendedStyle() | LVS_EX_GRIDLINES);			// 罫線描画
		m_ListHtTime.SetExtendedStyle(m_ListHtTime.GetExtendedStyle() | LVS_EX_FULLROWSELECT);		// 1行全体選択

		// 列追加
		char name[][32] = { "",	"表Ave",	"表Max",	"裏Ave",	"裏Max" };
		int  wid[] = { 48,	45,			45,			45,			45 };
		mcnt_pListHtTime->SetColumnAll(sizeof(wid) / sizeof(int), 32, wid, (char const*)name);
		// 行追加
		mcnt_pListHtTime->SetRowAll(7);
		// 固定値セット
		mcnt_pListHtTime->SetItem(0, 0, "DS HT");
		mcnt_pListHtTime->SetItem(1, 0, "WS HT");
		mcnt_pListHtTime->SetItem(2, 0, "伝送1");
		mcnt_pListHtTime->SetItem(3, 0, "伝送2");
		mcnt_pListHtTime->SetItem(4, 0, "HT-TO");
		mcnt_pListHtTime->SetItem(5, 0, "TO");
		mcnt_pListHtTime->SetItem(6, 0, "ﾄｰﾀﾙ");

	}


	//// 設備位置トラッキング リストビューインスタンス生成 & 定義
	if(true){
		mcnt_pListSetuDsp = new DspListView(&m_ListSetuDsp);
		// 見た目定義
		m_ListSetuDsp.SetExtendedStyle( m_ListSetuDsp.GetExtendedStyle() | LVS_EX_GRIDLINES );			// 罫線描画
		m_ListSetuDsp.SetExtendedStyle( m_ListSetuDsp.GetExtendedStyle() | LVS_EX_FULLROWSELECT );		// 1行全体選択
		//m_ListSetuDsp.GetHeaderCtrl()->ModifyStyle(0, HDS_HIDDEN);	// 行ヘッダー無し

		// 列追加			0		1		2	
		char name[][32] = {	"",		"rec",	"コイルNo"};
		int  wid[]		= {	73,		31,		80};
		int	 fmt[]		= {LVCFMT_LEFT, LVCFMT_CENTER, LVCFMT_LEFT};
		mcnt_pListSetuDsp->SetColumnAll(sizeof(wid)/sizeof(int), 32, wid, (char const*)name, fmt);
		// 行追加
		mcnt_pListSetuDsp->SetRowAll(7);		// 検査位置+表示基点+シャー
		// 固定値セット
		int nRow = 0;
		mcnt_pListSetuDsp->SetItem( nRow, 0, "表 検査位置");	nRow ++;
		mcnt_pListSetuDsp->SetItem( nRow, 0, "裏 検査位置");	nRow ++;
		mcnt_pListSetuDsp->SetItem( nRow, 0, "次検査コイル");	nRow ++;
		mcnt_pListSetuDsp->SetItem( nRow, 0, "表 検査台位置");	nRow ++;
		mcnt_pListSetuDsp->SetItem( nRow, 0, "裏 検査台位置");	nRow ++;
		mcnt_pListSetuDsp->SetItem( nRow, 0, "シャー位置");		nRow ++;
		mcnt_pListSetuDsp->SetItem( nRow, 0, "次表示コイル");	nRow++;
	}

	//-----------------------
	// 表示関係
	if (true) {
		mcnt_pListOp = new DspListView(&m_ListOp);
		// 見た目定義
		m_ListOp.SetExtendedStyle(m_ListOp.GetExtendedStyle() | LVS_EX_GRIDLINES);			// 罫線描画
		m_ListOp.SetExtendedStyle(m_ListOp.GetExtendedStyle() | LVS_EX_FULLROWSELECT);		// 1行全体選択

		// 列追加			0		1		2				
		char name[][32] = { "HY情報",	"今",	"最大" };
		int  wid[] = { 70,		40,		40, };
		mcnt_pListOp->SetColumnAll(sizeof(wid) / sizeof(int), 32, wid, (char const*)name);
		// 行追加
		mcnt_pListOp->SetRowAll(6);
		mcnt_pListOp->SetItem(0, 0, "[1]接続[件]");
		mcnt_pListOp->SetItem(1, 0, "[1]送信Que");
		mcnt_pListOp->SetItem(2, 0, "[2]接続[件]");
		mcnt_pListOp->SetItem(3, 0, "[2]送信Que");
		mcnt_pListOp->SetItem(4, 0, "[3]接続[件]");
		mcnt_pListOp->SetItem(5, 0, "[3]送信Que");
	}

	//-----------------------
	// エッジ関係
	if (true) {
		mcnt_pListEdge = new DspListView(&m_ListEdge);
		// 見た目定義
		m_ListEdge.SetExtendedStyle(m_ListEdge.GetExtendedStyle() | LVS_EX_GRIDLINES);			// 罫線描画
		m_ListEdge.SetExtendedStyle(m_ListEdge.GetExtendedStyle() | LVS_EX_FULLROWSELECT);		// 1行全体選択

		// 列追加			0		1		2		3			4		5
		char name[][32] = { "",	"モード",		"DS",	"WS",	"生DS",	"生WS", };
		int  wid[]		= { 20,		60,		50,		50,		50,		50 };
		mcnt_pListEdge->SetColumnAll(sizeof(wid) / sizeof(int), 32, wid, (char const*)name);
		// 行追加
		mcnt_pListEdge->SetRowAll(2);
		mcnt_pListEdge->SetItem(0, 0, "表");
		mcnt_pListEdge->SetItem(1, 0, "裏");
	}

	//-----------------------
	// 機器情報関係
	if (true) {
		mcnt_pListKiki = new DspListView(&m_ListKiki);
		// 見た目定義
		m_ListKiki.SetExtendedStyle(m_ListKiki.GetExtendedStyle() | LVS_EX_GRIDLINES);			// 罫線描画
		m_ListKiki.SetExtendedStyle(m_ListKiki.GetExtendedStyle() | LVS_EX_FULLROWSELECT);		// 1行全体選択
																								//m_ListKiki.GetHeaderCtrl()->ModifyStyle(0, HDS_HIDDEN);	// 行ヘッダー無し

		// 列追加			0		1
		char name[][32] = { "",		 "状態" };
		int  wid[]		= { 130,		140 };
		mcnt_pListKiki->SetColumnAll(sizeof(wid) / sizeof(int), 32, wid, (char const*)name);
		// 行追加
		mcnt_pListKiki->SetRowAll(11);
		mcnt_pListKiki->SetItem(0, 0, "ｼｰｹﾝｻ状態");
		mcnt_pListKiki->SetItem(1, 0, "ｼｰｹﾝｻ稼動状態");
		mcnt_pListKiki->SetItem(2, 0, "台車位置(表)");
		mcnt_pListKiki->SetItem(3, 0, "台車位置(裏)");
		mcnt_pListKiki->SetItem(4, 0, "SPM状態");
		mcnt_pListKiki->SetItem(5, 0, "塗油ｽﾌﾟﾚｰ状態");
		mcnt_pListKiki->SetItem(6, 0, "ﾁﾗｰ運転(表/裏/BK)");
		mcnt_pListKiki->SetItem(7, 0, "ﾁﾗｰ異常(表/裏/BK)");
		mcnt_pListKiki->SetItem(8, 0, "ｴｱﾄﾞﾗｲﾔ(表) 運転/異常");
		mcnt_pListKiki->SetItem(9, 0, "ｴｱﾄﾞﾗｲﾔ(裏) 運転/異常");
		mcnt_pListKiki->SetItem(10, 0, "扉/鍵状態");
	}

	//-----------------------
	// ボイス状態
	if (true) {
		mcnt_pListVoice = new DspListView(&m_ListVoice);
		// 見た目定義
		m_ListVoice.SetExtendedStyle(m_ListVoice.GetExtendedStyle() | LVS_EX_GRIDLINES);			// 罫線描画
		m_ListVoice.SetExtendedStyle(m_ListVoice.GetExtendedStyle() | LVS_EX_FULLROWSELECT);		// 1行全体選択

		// 列追加			0		1
		char name[][32] = { "面",	"条件" };
		int  wid[]		= { 25,70 };
		mcnt_pListVoice->SetColumnAll(sizeof(wid) / sizeof(int), 32, wid, (char const*)name);
		// 行追加
		mcnt_pListVoice->SetRowAll(1);
	}

	//-----------------------
	// 疵接近ランプ状態
	if (true) {
		mcnt_pListRamp = new DspListView(&m_ListRamp);
		// 見た目定義
		m_ListRamp.SetExtendedStyle(m_ListRamp.GetExtendedStyle() | LVS_EX_GRIDLINES);			// 罫線描画
		m_ListRamp.SetExtendedStyle(m_ListRamp.GetExtendedStyle() | LVS_EX_FULLROWSELECT);		// 1行全体選択

		// 列追加			0	1		2		3		4		5		6		7		8
		char name[][32] = { "",	"DS",	"2",	"3",	"4",	"5",	"6",	"7",	"WS" };
		int  wid[]		= { 40,	28 ,	28 ,	28 ,	28 ,	28 ,	28 ,	28 ,	28 };
		mcnt_pListRamp->SetColumnAll(sizeof(wid) / sizeof(int), 32, wid, (char const*)name);
		// 行追加
		mcnt_pListRamp->SetRowAll(3);
		mcnt_pListRamp->SetItem(0, 0, "表Lmp");
		mcnt_pListRamp->SetItem(1, 0, "裏Lmp");
		mcnt_pListRamp->SetItem(2, 0, "裏Alm");
	}

	//-----------------------
	// パトライト状態
	if (true) {
		mcnt_pListPato = new DspListView(&m_ListPato);
		// 見た目定義
		m_ListPato.SetExtendedStyle(m_ListPato.GetExtendedStyle() | LVS_EX_GRIDLINES);			// 罫線描画
		m_ListPato.SetExtendedStyle(m_ListPato.GetExtendedStyle() | LVS_EX_FULLROWSELECT);		// 1行全体選択

		// 列追加			0	　　1		2
		char name[][32] = { "位置",	"ﾊﾟﾄ",	"ﾌﾞｻﾞ" };
		int  wid[]		= { 65, 35,	35 };
		mcnt_pListPato->SetColumnAll(sizeof(wid) / sizeof(int), 32, wid, (char const*)name);
		// 行追加
		mcnt_pListPato->SetRowAll(3);
		mcnt_pListPato->SetItem(0, 0, "表検台(表)");
		mcnt_pListPato->SetItem(1, 0, "　　　　 (裏)");
		mcnt_pListPato->SetItem(2, 0, "総出力数");
	}

	////-----------------------
	//// データメモリ
	//if (true) {
	//	mcnt_pListDmem = new DspListView(&m_ListDmem);
	//	// 見た目定義
	//	m_ListDmem.SetExtendedStyle(m_ListDmem.GetExtendedStyle() | LVS_EX_GRIDLINES);			// 罫線描画
	//	m_ListDmem.SetExtendedStyle(m_ListDmem.GetExtendedStyle() | LVS_EX_FULLROWSELECT);		// 1行全体選択

	//	// 列追加			0		1
	//	char name[][32] = { "名称",	"状態",	"名称",	"状態" };
	//	int  wid[] = { 100,	45,		100,	45 };
	//	mcnt_pListDmem->SetColumnAll(sizeof(wid) / sizeof(int), 32, wid, (char const*)name);
	//	// 行追加
	//	mcnt_pListDmem->SetRowAll(6);
	//	mcnt_pListDmem->SetItem(0, 0, "ﾏｰｷﾝｸﾞPB");
	//	mcnt_pListDmem->SetItem(1, 0, "ﾏｰｸ指令許可");
	//	mcnt_pListDmem->SetItem(2, 0, "ﾏｰｸ監視ﾍﾙｼｰ");
	//	mcnt_pListDmem->SetItem(3, 0, "ﾏｰｸ監視稼動");
	//	mcnt_pListDmem->SetItem(4, 0, "ﾏｰｸ監視検査");
	//	mcnt_pListDmem->SetItem(5, 0, "ﾏｰｸ監視機器");
	//	mcnt_pListDmem->SetItem(0, 2, "ﾏｰｸ警報");
	//	mcnt_pListDmem->SetItem(1, 2, "ﾏｰｸ異常");
	//	mcnt_pListDmem->SetItem(2, 2, "ﾏｰｸ印字可能");
	//	mcnt_pListDmem->SetItem(3, 2, "ﾏｰｸ使用");
	//	mcnt_pListDmem->SetItem(4, 2, "ﾆﾚｺ稼動");
	//	mcnt_pListDmem->SetItem(5, 2, "ﾆﾚｺ異常");

	//}

	//// コイル関係 リストビューインスタンス生成 & 定義
	if (true) {
		mcnt_pListCoil = new DspListView(&m_ListCoil);
		// 見た目定義
		m_ListCoil.SetExtendedStyle(m_ListCoil.GetExtendedStyle() | LVS_EX_GRIDLINES);			// 罫線描画
		m_ListCoil.SetExtendedStyle(m_ListCoil.GetExtendedStyle() | LVS_EX_FULLROWSELECT);		// 1行全体選択

		// 列追加			0	1		2			3			4			5		6			7			8			9			10			11
		char name[][32]	= { "",	"rec",	"管理No",	"命令No",	"コイルNo",	"ｺｲﾙ長",	"幅",	"Scode表",	"Scode裏",	"Pcode表",	"Pcode裏",	"圧下有無"};
		int  wid[]		= { 30,	28,		119,		40,			80,			45,			36,		25,			25,			25,			25,			25};
		mcnt_pListCoil->SetColumnAll(sizeof(wid) / sizeof(int), 32, wid, (char const*)name);
		// 行追加
		mcnt_pListCoil->SetRowAll(2 + SIZE_COIL_BUF - 1);
		// 固定値セット
		int nRow = 0;
		mcnt_pListCoil->SetItem(0, 0, "次");
		mcnt_pListCoil->SetItem(1, 0, "現");
		for (int ii = 0; ii<SIZE_COIL_BUF - 1; ii++) {
			sWk.Format("前%d", ii + 1);
			mcnt_pListCoil->SetItem(2 + ii, 0, sWk);
		}
	}

	//// PLG 検査基点情報 リストビューインスタンス生成 & 定義
	if (true) {
		mcnt_pListPlgKen = new DspListView(&m_ListPlgKen);
		// 見た目定義
		m_ListPlgKen.SetExtendedStyle(m_ListPlgKen.GetExtendedStyle() | LVS_EX_GRIDLINES);		// 罫線描画
		m_ListPlgKen.SetExtendedStyle(m_ListPlgKen.GetExtendedStyle() | LVS_EX_FULLROWSELECT);	// 1行全体選択

		// 列追加
		char name[][32] = { "検査基点",	"",	"" };
		int  wid[] = { 115, 65, 33 };
		mcnt_pListPlgKen->SetColumnAll(sizeof(wid) / sizeof(int), 32, wid, (char const*)name);
		// 行追加
		mcnt_pListPlgKen->SetRowAll(5);
		// 固定値セット
		mcnt_pListPlgKen->SetItem(0, 0, "表 検査位置 距離");
		mcnt_pListPlgKen->SetItem(1, 0, "裏 検査位置 距離");
		mcnt_pListPlgKen->SetItem(2, 0, "比較用PLG");
		mcnt_pListPlgKen->SetItem(3, 0, "コイル残長");
		mcnt_pListPlgKen->SetItem(4, 0, "前コイル実測長");
		for (int ii = 0; ii<5; ii++) {
			mcnt_pListPlgKen->SetItem(ii, 2, "[m]");
		}
	}

	//// PLG 表示基点情報 リストビューインスタンス生成 & 定義
	if (true) {
		mcnt_pListPlgDsp = new DspListView(&m_ListPlgDsp);
		// 見た目定義
		m_ListPlgDsp.SetExtendedStyle(m_ListPlgDsp.GetExtendedStyle() | LVS_EX_GRIDLINES);		// 罫線描画
		m_ListPlgDsp.SetExtendedStyle(m_ListPlgDsp.GetExtendedStyle() | LVS_EX_FULLROWSELECT);	// 1行全体選択

		// 列追加
		char name[][32] = { "表示基点",	"",	"" };
		int  wid[] = { 115, 65, 33 };
		mcnt_pListPlgDsp->SetColumnAll(sizeof(wid) / sizeof(int), 32, wid, (char const*)name);
		// 行追加
		mcnt_pListPlgDsp->SetRowAll(6);
		// 固定値セット
		mcnt_pListPlgDsp->SetItem(0, 0, "表示基点 距離");
		mcnt_pListPlgDsp->SetItem(1, 0, "表検査台 距離");
		mcnt_pListPlgDsp->SetItem(2, 0, "裏検査台 距離");
		mcnt_pListPlgDsp->SetItem(3, 0, "シャー(入側論理)");
		mcnt_pListPlgDsp->SetItem(4, 0, "シャー(出側論理)");
		mcnt_pListPlgDsp->SetItem(5, 0, "前コイル実測長");
		for (int ii = 0; ii<6; ii++) {
			mcnt_pListPlgDsp->SetItem(ii, 2, "[m]");
		}
	}

	//// PLG基本情報 リストビューインスタンス生成 & 定義
	if (true) {
		mcnt_pListPlg = new DspListView(&m_ListPlg);
		// 見た目定義
		m_ListPlg.SetExtendedStyle(m_ListPlg.GetExtendedStyle() | LVS_EX_GRIDLINES);		// 罫線描画
		m_ListPlg.SetExtendedStyle(m_ListPlg.GetExtendedStyle() | LVS_EX_FULLROWSELECT);	// 1行全体選択

		// 列追加
		char name[][32] = { "基本情報",	"",	"" };
		int  wid[] = { 90, 60, 60 };
		mcnt_pListPlg->SetColumnAll(sizeof(wid) / sizeof(int), 32, wid, (char const*)name);
		// 行追加
		mcnt_pListPlg->SetRowAll(11);
		// 固定値セット
		mcnt_pListPlg->SetItem(0, 0, "統括FrameNo");
		mcnt_pListPlg->SetItem(1, 0, "1ﾌﾚｰﾑ周期");
		mcnt_pListPlg->SetItem(2, 0, "1ﾌﾚｰﾑ長さ");
		mcnt_pListPlg->SetItem(3, 0, "ライン速度");
		mcnt_pListPlg->SetItem(4, 0, "縦分解能");
		mcnt_pListPlg->SetItem(5, 0, "(都度)");
		mcnt_pListPlg->SetItem(6, 0, "簡易ﾗｲﾝ速度検");
		mcnt_pListPlg->SetItem(7, 0, "簡易ﾗｲﾝ速度表");
		mcnt_pListPlg->SetItem(8, 0, "SEQ FrameNo");
		mcnt_pListPlg->SetItem(9, 0, "SEQ Exsync");
		mcnt_pListPlg->SetItem(10, 0, "SEQ Plg");

		mcnt_pListPlg->SetItem(0, 2, "");
		mcnt_pListPlg->SetItem(1, 2, "[ms]");
		mcnt_pListPlg->SetItem(2, 2, "[mm]");
		mcnt_pListPlg->SetItem(3, 2, "[mpm]");
		mcnt_pListPlg->SetItem(4, 2, "[mm/px]");
		mcnt_pListPlg->SetItem(5, 2, "[mm/px]");
		mcnt_pListPlg->SetItem(6, 2, "[mpm]");
		mcnt_pListPlg->SetItem(7, 2, "[mpm]");
		mcnt_pListPlg->SetItem(8, 2, "");
		mcnt_pListPlg->SetItem(9, 2, "[pls]");
		mcnt_pListPlg->SetItem(10, 2, "[pls]");
	}

	//-----------------------
	// DB関係 リストビューインスタンス生成 & 定義
	if (true) {
		mcnt_pListDb = new DspListView(&m_ListDb);
		// 見た目定義
		m_ListDb.SetExtendedStyle(m_ListDb.GetExtendedStyle() | LVS_EX_GRIDLINES);			// 罫線描画
		m_ListDb.SetExtendedStyle(m_ListDb.GetExtendedStyle() | LVS_EX_FULLROWSELECT);		// 1行全体選択

		// 列追加			0		1				2		3		4			5			6				7	
		char name[][32] = { "",		"管理No",		"保存",	"全数",	"無害削除",	"有害削除", "無害画像削除", "有害画像削除" };
		int  wid[]		= { 28,		115,			70,		70,			70,		70,			70,				70 };
		mcnt_pListDb->SetColumnAll(sizeof(wid) / sizeof(int), 32, wid, (char const*)name);
		// 行追加
		mcnt_pListDb->SetRowAll(DbBaseManager::SIZE_SQLINF_NUM);
		// 固定値セット
		int nRow = 0;
		mcnt_pListDb->SetItem(0, 0, "現");
		for (int ii = 0; ii<mcnt_pListDb->GetRowCount(); ii++) {
			sWk.Format("前%d", ii + 1);
			mcnt_pListDb->SetItem(1 + ii, 0, sWk);
		}
	}
	if (true) {
		mcnt_pListDbInfo = new DspListView(&m_ListDbInfo);
		// 見た目定義
		m_ListDbInfo.SetExtendedStyle(m_ListDbInfo.GetExtendedStyle() | LVS_EX_GRIDLINES);			// 罫線描画
		m_ListDbInfo.SetExtendedStyle(m_ListDbInfo.GetExtendedStyle() | LVS_EX_FULLROWSELECT);		// 1行全体選択

		// 列追加			0		1		2				
		char name[][32] = { "項目",	"今",	"最大" };
		int  wid[]		= { 115,		40,		40, };
		mcnt_pListDbInfo->SetColumnAll(sizeof(wid) / sizeof(int), 32, wid, (char const*)name);
		// 行追加
		mcnt_pListDbInfo->SetRowAll(10);
		// 固定値セット
		mcnt_pListDbInfo->SetItem(0, 0, "(SQL) SQL生成[件]");
		mcnt_pListDbInfo->SetItem(1, 0, "(SQL) DB登録[件]");
		mcnt_pListDbInfo->SetItem(2, 0, "(SQL) 画像登録[ms]");
		mcnt_pListDbInfo->SetItem(3, 0, "(SQL) SQL登録[ms]");
		mcnt_pListDbInfo->SetItem(4, 0, "(BCP) SQL生成[件]");
		mcnt_pListDbInfo->SetItem(5, 0, "(BCP) 画像登録[ms]");
		mcnt_pListDbInfo->SetItem(6, 0, "(BCP) ATTR SR[ms]");
		mcnt_pListDbInfo->SetItem(7, 0, "(BCP) ATTR Bat[ms]");
		mcnt_pListDbInfo->SetItem(8, 0, "(BCP) IMG SR[ms]");
		mcnt_pListDbInfo->SetItem(9, 0, "(BCP) IMG Bat[ms]");
	}
	
	//// メインインスタンス 開始処理
	mcls_MI.SetLogMgr(mcls_pLog);
	mcls_MI.SetLogMgrSeq(mcls_pLogSeq);
	mcls_MI.SetLogMgrE(mcls_pLogE);
	mcls_MI.SetLogMgrG(mcls_pLogG);
	mcls_MI.SetLogMgrD(mcls_pLogD);
	mcls_MI.SetLogMgrA(mcls_pLogA);
	mcls_MI.Init();

	//// 画面更新用タイマ生成
	SetTimer(1, 500, NULL);

	//// デバック用
	m_nCoil = 0;

	//// メインスレッドの実行を行う
	LOG(em_MSG), "[Dlg] 各プロセス開始処理 完了");
	mcls_MI.Start();
	return;
}

//￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣
// 各プロセス開放処理
//＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿
void CTO_DEFECTDlg::ExitProcess()
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

	LOGSEQ(em_MSG), "[Dlg] タスク終了");
	if (NULL != mcls_pLogSeq) {
		delete mcls_pLogSeq;
		mcls_pLogSeq = NULL;
	}

	LOGE(em_MSG), "[Dlg] タスク終了");
	if (NULL != mcls_pLogE) {
		delete mcls_pLogE;
		mcls_pLogE = NULL;
	}

	LOGG(em_MSG), "[Dlg] タスク終了");
	if (NULL != mcls_pLogG) {
		delete mcls_pLogG;
		mcls_pLogG = NULL;
	}

	LOGD(em_MSG), "[Dlg] タスク終了");
	if (NULL != mcls_pLogD) {
		delete mcls_pLogD;
		mcls_pLogD = NULL;
	}

	LOGA(em_MSG), "[Dlg] タスク終了");
	if (NULL != mcls_pLogA) {
		delete mcls_pLogA;
		mcls_pLogA = NULL;
	}

	//// 画面コントロール開放
	delete mcnt_pListSt;
	delete mcnt_pListSys;
	delete mcnt_pListHt;
	delete mcnt_pListHtTime;
	delete mcnt_pListOp;
	delete mcnt_pListPr;

	delete mcnt_pListDb;
	delete mcnt_pListDbInfo;

	delete mcnt_pListSetuDsp;
	delete mcnt_pListEdge;
	delete mcnt_pListKiki;
	delete mcnt_pListRamp;
	delete mcnt_pListPato;
	delete mcnt_pListVoice;
	//delete mcnt_pListDmem;

	delete mcnt_pListCoil;
	delete mcnt_pListPlgKen;
	delete mcnt_pListPlgDsp;
	delete mcnt_pListPlg;

	delete mcnt_pListCmd;
	delete mcnt_pListInput;

	// タスク終了処理
	task_exit();

	// 画面の終了要求
	PostMessage(WM_QUIT);
}

//------------------------------------------
// メインスレッド DlgThread
// LPVOID param ダイヤログポインタ
//------------------------------------------
unsigned WINAPI CTO_DEFECTDlg::MainThread(void* param)
{
	CTO_DEFECTDlg* p = (CTO_DEFECTDlg *)param;
	COMMON_QUE que;			// 受信構造体
	bool bWk;

							//// 各プロセス初期化処理 (これ以前はログ出力禁止)
	p->InitProcess();

	//// 起動要因取り込み ループ開始
	while (true) {
		recv_mail(&que);			// 起動要因取り込み

		switch (que.nEventNo) {

//-----------------------------------------------------------------------------------------------
		case FACT_TO_DEFECT_01:								// パラメータ変更要求通知 (←KA_PARSET, KA_PARENG)
			pLOG(em_MSG), "[MAIL] パラメータ変更要求 (FACT_TO_DEFECT_01) [テーブル=%d]", que.fl.data[1]); 
			// メインインスタンスへ通知
			p->mcls_MI.SetDeliveryMail( E_DEF_ML_PARAM, &que);
			break;

//-----------------------------------------------------------------------------------------------
		case FACT_TO_DEFECT_02:								// 検査開始要求 (←SO_GAMENN)
			bWk = ( 0 == que.fl.data[2] ? false : true);
			pLOG(em_MSG), "[MAIL] 検査開始要求 (FACT_TO_DEFECT_02) [%s, %s, %s, s=%d:%d, p=%d:%d, パララン１=%d:%d, パララン２=%d:%d]",
				DivNameManager::GetDivUnten(que.fl.data[0]), DivNameManager::GetDivPlg(que.fl.data[1]), p->mcls_MI.gcls_Status.GetMenModeName(bWk), que.fl.data[3], que.fl.data[4], que.fl.data[5], que.fl.data[6], que.fl.data[7], que.fl.data[8], que.fl.data[9], que.fl.data[10] );
			syslog(230, "[%s; %s; %s; s=%d:%d; p=%d:%d]", DivNameManager::GetDivUnten(que.fl.data[0]), DivNameManager::GetDivPlg(que.fl.data[1]), p->mcls_MI.gcls_Status.GetMenModeName(bWk), que.fl.data[3], que.fl.data[4], que.fl.data[5], que.fl.data[6]);

			//// 統括で判断する 判定状態は初期化しておく
			for(int ii=0; ii<NUM_ALL_HANTEI; ii++ ) {
				CString wk;
				wk.Format("HANTEI_ANS_%d_ID", ii+1 );
				KizuFunction::SetStatus(wk, true, false);
			}
			for (int ii = 0; ii < NUM_CAMERA; ii++)
			{
				CString wk;
				wk.Format("CAM_GCP_ERR_%d_ID ", ii + 1);
				KizuFunction::SetStatus(wk, true, false);
			}
			
			// メインインスタンスへ通知
			p->mcls_MI.SetDeliveryMail( E_DEF_ML_START, &que);
			break;

//-----------------------------------------------------------------------------------------------
		case FACT_TO_DEFECT_03:								// 検査停止要求 (←SO_GAMENN)
			pLOG(em_MSG), "[MAIL] 検査停止要求 (FACT_TO_DEFECT_03)"); 
			syslog(231, "");

			// メインインスタンスへ通知
			p->mcls_MI.SetDeliveryMail( E_DEF_ML_STOP, &que);
			break;

//-----------------------------------------------------------------------------------------------
		case FACT_TO_DEFECT_04:								// 稼動状態取得要求 (←SO_GAMENN)
			pLOG(em_MSG), "[MAIL] 稼動状態取得要求 (FACT_TO_DEFECT_04)");

			// オペレータ画面へアンサー返却
			p->mcls_MI.Send_SoGamenn_MailStatus();
			break;

//-----------------------------------------------------------------------------------------------
		case FACT_TO_DEFECT_06:								// 判定開始アンサー (←HT_DEFECT)
			pLOG(em_MSG), "[MAIL] 判定開始アンサー (FACT_TO_DEFECT_06) [カメラペア=%d PCID=%d 結果=%d kind=%d(1:nc,2:pd,3:pr)]", que.fl.data[0], que.fl.data[1], que.fl.data[2], que.fl.data[3] );

			// メインインスタンスへ通知
			p->mcls_MI.SetDeliveryMail( E_DEF_ML_WAIT_START_ANSWER, &que);
			break;

//-----------------------------------------------------------------------------------------------
		case FACT_TO_DEFECT_07:								// 判定停止アンサー (←HT_DEFECT)
			pLOG(em_MSG), "[MAIL] 判定停止アンサー (FACT_TO_DEFECT_07) [カメラペア=%d PCID=%d 結果=%d <%s> kind=%d(1:nc,2:pd,3:pr)]", que.fl.data[0], que.fl.data[1], que.fl.data[2], que.fl.data[3]==0 ? "応答":"異常認識", que.fl.data[4] );
			
			// メインインスタンスへ通知
			p->mcls_MI.SetDeliveryMail( E_DEF_ML_WAIT_STOP_ANSWER, &que );
			break;

//-----------------------------------------------------------------------------------------------
		case FACT_TO_DEFECT_09:								// 判定状態問合せ報告 (←HT_DEFECT)
			pLOG(em_MSG), "[MAIL] 判定状態問合せ報告 (FACT_TO_DEFECT_09) [カメラペア=%d PCID=%d 結果=%d kind=%d(1:nc,2:pd,3:pr)]", que.fl.data[0], que.fl.data[1], que.fl.data[2], que.fl.data[3] );

			// メインインスタンスへ通知
			p->mcls_MI.SetDeliveryMail( E_DEF_ML_STATE_ANSWER, &que);
			break;

//-----------------------------------------------------------------------------------------------
		case FACT_TO_DEFECT_11:								// コイル情報受信通知 (←TO_GETDAT)
			pLOG(em_MSG), "[MAIL] コイル情報受信通知 (FACT_TO_DEFECT_11) [受信位置=%d (0:検査基点 1:表示基点)]", que.fl.data[0]);
			
			//// 不明受信位置の伝文受信
			if (0 != que.fl.data[0] && 1 != que.fl.data[0]) {
				pLOG(em_ERR), "[MI] 受信位置不明の伝文受信 [受信位置=%d]", que.fl.data[0]);
				syslog(251, "[受信位置=%d]", que.fl.data[0]);
				break;
			}

			//// 通常検査中以外は無視
			if( ! p->mcls_MI.gcls_Status.IsDoSequence() ) {
				pLOG(em_ERR), "[MI] 通常検査中以外で%sコイル情報受信", 0 == que.fl.data[0] ? "検査用" : "表示用");
				syslog(251, "[%sコイル情報受信]", 0 == que.fl.data[0]? "検査用":"表示用");
				break;
			}
			// 次コイル情報セット要求
			if(0 == que.fl.data[0])p->mcls_MI.gcls_pCoil->SetEvNextCoilKen();		// 検査用
			if(1 == que.fl.data[0])p->mcls_MI.gcls_pCoil->SetEvNextCoilDsp();		// 表示用
			break;

//-----------------------------------------------------------------------------------------------
		case FACT_TO_DEFECT_12:								// 前工程疵情報書込み完了通知 (←TO_MAEDAT)
			pLOG(em_MSG), "[MAIL] 前工程疵書込み完了通知 (FACT_TO_DEFECT_12) [命令No=%.4s, コイルNo=%s]", que.mix.cdata[0], que.mix.cdata[1]);

			// メインインスタンスへ通知
			p->mcls_MI.SetDeliveryMail(E_DEF_ML_MAE_WRITE, &que);
			break;

//-----------------------------------------------------------------------------------------------
		case FACT_TO_DEFECT_13:								// 実績書き込み状況問い合わせ通知 (←TO_PUTDAT)
			pLOG(em_MSG), "[MAIL] 実績書き込み状況問合せ (FACT_TO_DEFECT_13)  [コイルNo=%s][管理No=%s]", que.mix.cdata[0], que.mix.cdata[1]);

			// メインインスタンスへ通知
			p->mcls_MI.SetDeliveryMail(E_DEF_ML_DB_ADD_SYNC, &que);
			break;

//-----------------------------------------------------------------------------------------------
		case FACT_TO_DEFECT_14:								// 現露光時間報告 (←TO_CAMERA)
			break;

//-----------------------------------------------------------------------------------------------
		case FACT_TO_DEFECT_15:								// イニシャル伝文受信通知 (←TO_PROCON)
			pLOG(em_MSG), "[MAIL] イニシャル伝文受信 (FACT_TO_DEFECT_15)");

			// メインインスタンスへ通知
			p->mcls_MI.SetDeliveryMail(E_DEF_ML_INITIAL, &que);
			break;
			
//-----------------------------------------------------------------------------------------------
		case FACT_TO_DEFECT_20:								// ボイス警報出力解除要求
			pLOG(em_MSG), "[MAIL] ボイス警報出力解除要求 (FACT_TO_DEFECT_20) [%s]", (0==que.fl.data[0] ? "設備異常警報解除": "欠陥検出警報解除") );

			// メインインスタンスへ通知
			p->mcls_MI.SetDeliveryMail( E_DEF_ML_ALAMECANCEL, &que);
			break;

//-----------------------------------------------------------------------------------------------
		case FACT_TO_DEFECT_21:								// 途中開始要求 (←SO_GAMENN)
			bWk = (0 == que.fl.data[2] ? false : true);
			pLOG(em_MSG), "[MAIL] 途中開始要求 (FACT_TO_DEFECT_21) [%s, %s, s=%d:%d, p=%d:%d, ロジックパラランPC%d, カメラ入力=%d:%d]",
				DivNameManager::GetDivUnten(que.fl.data[0]), DivNameManager::GetDivPlg(que.fl.data[1]), que.fl.data[3], que.fl.data[4], que.fl.data[5], que.fl.data[6], que.fl.data[7] + 1, que.fl.data[8], que.fl.data[9]);
			syslog(247, "[%s; %s; s=%d:%d; p=%d:%d, ロジックパラランPC%d, カメラ入力=%d:%d]", DivNameManager::GetDivUnten(que.fl.data[0]), DivNameManager::GetDivPlg(que.fl.data[1]), que.fl.data[3], que.fl.data[4], que.fl.data[5], que.fl.data[6], que.fl.data[7] + 1, que.fl.data[8], que.fl.data[9]);

			// メインインスタンスへ通知
			p->mcls_MI.SetDeliveryMail(E_DEF_ML_RESUME_START, &que);
			break;

//-----------------------------------------------------------------------------------------------
		case FACT_TO_DEFECT_22:								// 途中停止要求 (←SO_GAMENN)
			pLOG(em_MSG), "[MAIL] 途中停止要求 (FACT_TO_DEFECT_22) [検査停止区分=ロジックパラランPC%d]", que.fl.data[0] + 1);
			syslog(248, "検査停止区分=ロジックパラランPC%d", que.fl.data[0] + 1);

			// メインインスタンスへ通知
			p->mcls_MI.SetDeliveryMail(E_DEF_ML_RESUME_STOP, &que);
			break;

//-----------------------------------------------------------------------------------------------
		case FACT_TO_DEFECT_23:								// 途中開始アンサー (←HT_DEFECT)
			pLOG(em_MSG), "[MAIL] 途中開始アンサー (FACT_TO_DEFECT_23) [PCID=%d 結果=%d 途中参加フレームNo=%d]", que.fl.data[1], que.fl.data[2], que.fl.data[3]);

			// メインインスタンスへ通知
			p->mcls_MI.SetDeliveryMail(E_DEF_ML_RESUME_START_ANSWER, &que);
			break;

//-----------------------------------------------------------------------------------------------
		case FACT_TO_DEFECT_24:								// 途中停止アンサー (←HT_DEFECT)
			pLOG(em_MSG), "[MAIL] 途中停止アンサー (FACT_TO_DEFECT_24) [PCID=%d 結果=%d 区分=%d]", que.fl.data[1], que.fl.data[2], que.fl.data[3]);

			// メインインスタンスへ通知
			p->mcls_MI.SetDeliveryMail(E_DEF_ML_RESUME_STOP_ANSWER, &que);
			break;

//-----------------------------------------------------------------------------------------------
		case FACT_TO_DEFECT_30:								// カメラリンクスイッチ設定完了通知
			pLOG(em_MSG), "[MAIL] カメラリンクスイッチ設定完了通知 (FACT_TO_DEFECT_30)");

			// メインインスタンスへ通知
			p->mcls_MI.SetDeliveryMail( E_DEF_ML_LINKSW_ANSWER, &que );
			break;

//-----------------------------------------------------------------------------------------------
		case FACT_TO_DEFECT_31:								// 途中参加設定完了通知 (←TO_LINKSW)
			pLOG(em_MSG), "[MAIL] 途中参加設定完了通知 (FACT_TO_DEFECT_31) [表裏区分=%d 途中参加フレームNo=%d]", que.fl.data[0], que.fl.data[1]);

			// メインインスタンスへ通知
			p->mcls_MI.SetDeliveryMail( E_DEF_ML_LINKSW_JOIN_ANSWER, &que);
			break;

//-----------------------------------------------------------------------------------------------
		case FACT_TO_DEFECT_32:								// カメラ選択設定完了通知 (←TO_LINKSW)
			pLOG(em_MSG), "[MAIL] カメラ選択設定完了通知 (FACT_TO_DEFECT_32) [スイッチFPGAボードID=%d, フレームグラバーボードNo=%d, カメラNo=%d, Master／Slave設定=%d, 設定結果=%d]",
				que.fl.data[0], que.fl.data[1], que.fl.data[2], que.fl.data[3], que.fl.data[4]);

			// メインインスタンスへ通知
			p->mcls_MI.SetDeliveryMail(E_DEF_ML_LINKSW_CAMSET_ANSWER, &que);
			break;

//-----------------------------------------------------------------------------------------------
		case FACT_KS_DB_STATEUS:							// DB状態変更通知(←KS_SLAVE)
			if( 1 == que.fl.data[0] ) {
				if( ! p->mcls_MI.gcls_Status.IsRun() ) break;
				pLOG(em_ERR), "[MAIL] DB異常により 検査停止" );
				syslog(233, "");

				// DBが異常 (上で最低減チェックしている為、通常ありえない)
				p->mcls_MI.gcls_Status.AddQueMyErrWar(true, ON_DB_ACCESS_ERR);
			}
			break;

//-----------------------------------------------------------------------------------------------
		case FACT_KS_PCNAME_CHANGE:							// PC名称変更通知 (←KS_SLAVE)
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
void CTO_DEFECTDlg::OnTimer(UINT_PTR nIDEvent)
{
	CString	ss, sWk;
	int		nWk;
	int		nRow;

	//==================================================
	// ステータス状態
	mcnt_pListSt->SetItem(0, 1, DivNameManager::GetDivKadou(mcls_MI.gcls_Status.GetKadou()));
	mcnt_pListSt->SetItem(1, 1, DivNameManager::GetDivSogyo(mcls_MI.gcls_Status.GetSogyo()));
#ifdef DSP_SOGYO_CHECK
	mcnt_pListSt->SetItem(2, 1, DivNameManager::GetDivSogyo(mcls_MI.gcls_Status.GetSogyoDsp()));
#endif
	mcnt_pListSt->SetItem(3, 1, DivNameManager::GetDivUnten(mcls_MI.gcls_Status.GetUnten()));
	mcnt_pListSt->SetItem(4, 1, DivNameManager::GetDivPlg(mcls_MI.gcls_Status.GetPlg()));
	mcnt_pListSt->SetItem(5, 1, mcls_MI.gcls_Status.GetMenModeName(mcls_MI.gcls_Status.GetMenMode()));

	//==================================================
	// システム状態
	for (int ii = 0; ii<NUM_MEN; ii++) {
		mcnt_pListSys->SetItem(0, 1 + ii, GetDivSys(mcls_MI.gcls_Status.GetSys(ii)));
		mcnt_pListSys->SetItem(1, 1 + ii, DivNameManager::GetDivKensa(mcls_MI.gcls_Status.GetKensa(ii)));
		mcnt_pListSys->SetItem(2, 1 + ii, DivNameManager::GetDivKiki(mcls_MI.gcls_Status.GetKiki(ii)));

		// 判定処理時間
		ss.Format("%0.3f", mcls_MI.gcls_pHtRecv[ii]->GetAveTime());	mcnt_pListSys->SetItem(3, 1 + ii, ss);
	}

	//==================================================
	// 判定
	nRow = 0;
//	int nVCnt = NUM_CAM_POS / NUM_HANTEI;		// 物理回線1つにたいして、何論理回線あるか？  (このまま普通のφωにも使える！)
	int nVCnt = NUM_CAMLINK_BOARD;
	for (int ii = 0; ii<NUM_MEN; ii++) {
		for (int jj = 0; jj<NUM_CAM_POS; jj++) {
			if ((NUM_HANTEI * NUM_CAMLINK_BOARD) > jj)
			{
				// 物理回線単位
				mcnt_pListHt->SetItem(nRow, 1, mcls_MI.gcls_Status.GetHtID(ii, (jj / nVCnt)));
				if (0 == (jj % nVCnt))
				{
					ss = (mcls_MI.gcls_pHtRecv[ii]->IsConnect((jj / nVCnt) + 1) ? "○" : "×");				mcnt_pListHt->SetItem(nRow, 2, ss);
					ss = DivNameManager::GetDivBool(mcls_MI.gcls_Status.GetHtState(ii, (jj / nVCnt)));		mcnt_pListHt->SetItem(nRow, 3, ss);
				}
				else
				{
					mcnt_pListHt->SetItem(nRow, 2, "");
					mcnt_pListHt->SetItem(nRow, 3, "");
				}

				// 論理回線単位
				mcnt_pListHt->SetItem(nRow, 4, mcls_MI.gcls_pHtRecv[ii]->GetFrameNo(jj));	// Fno	
				//mcnt_pListHt->SetItem(nRow, 5, (int)mcls_MI.gcls_EdgePosCont.GetEdgePos(ii, jj) );	// エッジ

				for (int kk = 0; kk < 3; kk++)
				{
					//mcnt_pListHt->SetItem(nRow, 5 + (2 * kk) + 0, mcls_MI.gcls_pHtRecv[ii]->GetBr(jj, kk));
					mcnt_pListHt->SetItem(nRow, 5 + (4 * kk) + 0, "");
					mcnt_pListHt->SetItem(nRow, 6 + (4 * kk) + 0, mcls_MI.gcls_pHtRecv[ii]->GetBr(jj, kk));
					mcnt_pListHt->SetItem(nRow, 7 + (4 * kk) + 0, mcls_MI.gcls_Status.GetExpTime((ii + 1)*jj, kk));
					mcnt_pListHt->SetItem(nRow, 8 + (4 * kk) + 0, mcls_MI.gcls_Status.GetCamGain((ii + 1)*jj, kk));
				}
			}
			else
			{
				mcnt_pListHt->SetItem(nRow, 1, "");
				mcnt_pListHt->SetItem(nRow, 2, "");
				mcnt_pListHt->SetItem(nRow, 3, "");
				mcnt_pListHt->SetItem(nRow, 4, "");
				for (int kk = 0; kk<3; kk++) {
					mcnt_pListHt->SetItem(nRow, 5 + (4 * kk) + 0, "");
					mcnt_pListHt->SetItem(nRow, 6 + (4 * kk) + 0, mcls_MI.gcls_pHtRecv[ii]->GetBr(jj, kk));
					mcnt_pListHt->SetItem(nRow, 7 + (4 * kk) + 0, mcls_MI.gcls_Status.GetExpTime((ii + 1)*jj, kk));
					mcnt_pListHt->SetItem(nRow, 8 + (4 * kk) + 0, mcls_MI.gcls_Status.GetCamGain((ii + 1)*jj, kk));
				}
			}

			nRow++;
		}
	}
#ifdef ENABLE_CYCLE
	nRow = 0;
	nVCnt = NUM_CAM_POS / NUM_HANTEI_CYCLE;		// 物理回線1つにたいして、何論理回線あるか？  (このまま普通のφωにも使える！)
	for (int ii = 0; ii<NUM_MEN; ii++) {
		for (int jj = 0; jj<NUM_CAM_POS; jj++) {
			mcnt_pListCy->SetItem(nRow, 1, mcls_MI.gcls_Status.GetCyID(ii, (jj / nVCnt)));
			// 物理回線単位
			if (0 == (jj % nVCnt)) {
				ss = (mcls_MI.gcls_pCyRecv[ii]->IsConnect((jj / nVCnt) + 1) ? "○" : "×");				mcnt_pListCy->SetItem(nRow, 2, ss);
				ss = DivNameManager::GetDivBool(mcls_MI.gcls_Status.GetCyState(ii, (jj / nVCnt)));		mcnt_pListCy->SetItem(nRow, 3, ss);
			}
			else {
				mcnt_pListCy->SetItem(nRow, 2, "");
				mcnt_pListCy->SetItem(nRow, 3, "");
			}

			// 論理回線単位
			mcnt_pListCy->SetItem(nRow, 4, mcls_MI.gcls_pCyRecv[ii]->GetFrameNo(jj));	// Fno	
			nRow++;
		}
	}
#endif
	//==================================================
	// ロジックパララン
	nRow = 0;
	for (int ii = 0; ii < NUM_ALL_HTPARA; ii++)
	{
		mcnt_pListPr->SetItem(nRow, 1, mcls_MI.gcls_Status.GetHtParaID(ii));
		ss = DivNameManager::GetDivBool(mcls_MI.gcls_Status.GetHtParaState(ii));
		mcnt_pListPr->SetItem(nRow, 2, ss);
		for (int jj = 0; jj < NUM_CAMLINK_BOARD; jj++)
		{
			mcnt_pListPr->SetItem(nRow, 3 + jj, LogicParaRunFunc::GetHtParaCamNo(ii, jj));
		}
		mcnt_pListPr->SetItem(nRow, 3 + NUM_CAMLINK_BOARD, GetDivSys(mcls_MI.gcls_Status.GetParaSys(ii)));
		nRow++;
	}

	//==================================================
	// 判定処理時間
	//  1. TO_DEFECT	1フレーム取込完了(PLG)
	//  2. ncldetect	画像取り込み
	//  3. ncldetect	画像処理
	//  4. HT_DEFECT	統括に送信
	//  5. TO_DEFECT	フレーム情報受信(フレーム内の1番目受信)
	//  6. TO_DEFECT	フレーム情報受信(フレーム内の2番目受信)
	//  7. TO_DEFECT	フレーム情報受信(全幅分を受信)
	//  8. TO_DEFECT	受信処理(疵情報保存, マーキング指示)
	//  9. TO_DEFECT	1フレーム処理完了

	for (int ii = 0; ii<NUM_MEN; ii++) {
		// [DS HT]  画像処理エンジン処理完了～統括伝送開始までの平均、最大所要時間[ms]			(No.4)
		ss.Format("%0.2f", mcls_MI.gcls_pHtRecv[ii]->GetNcHtProcAveTime(0));	mcnt_pListHtTime->SetItem(0, (ii * 2) + 1, ss);
		ss.Format("%0.2f", mcls_MI.gcls_pHtRecv[ii]->GetNcHtProcMaxTime(0));	mcnt_pListHtTime->SetItem(0, (ii * 2) + 2, ss);
		// [WS HT]
		ss.Format("%0.2f", mcls_MI.gcls_pHtRecv[ii]->GetNcHtProcAveTime(1));	mcnt_pListHtTime->SetItem(1, (ii * 2) + 1, ss);
		ss.Format("%0.2f", mcls_MI.gcls_pHtRecv[ii]->GetNcHtProcMaxTime(1));	mcnt_pListHtTime->SetItem(1, (ii * 2) + 2, ss);

		// [伝送1]  フレーム内1番目～2番目受信までの平均、最大所要時間[ms]						(No.5-6)
		ss.Format("%0.2f", mcls_MI.gcls_pHtRecv[ii]->GetRecvAveTime());			mcnt_pListHtTime->SetItem(2, (ii * 2) + 1, ss);
		ss.Format("%0.2f", mcls_MI.gcls_pHtRecv[ii]->GetRecvMaxTime());			mcnt_pListHtTime->SetItem(2, (ii * 2) + 2, ss);

		// [伝送2]  フレーム内1番目～全幅分受信までの平均、最大所要時間[ms]						(No.5-7)
		ss.Format("%0.2f", mcls_MI.gcls_pHtRecv[ii]->GetRecvTotalAveTime());	mcnt_pListHtTime->SetItem(3, (ii * 2) + 1, ss);
		ss.Format("%0.2f", mcls_MI.gcls_pHtRecv[ii]->GetRecvTotalMaxTime());	mcnt_pListHtTime->SetItem(3, (ii * 2) + 2, ss);

		// [HT-TO]  画像取込～判定→統括伝送完了までの平均、最大所要時間[ms]					(No.4-7)
		ss.Format("%0.2f", mcls_MI.gcls_pHtRecv[ii]->GetHtProcAveTime());		mcnt_pListHtTime->SetItem(4, (ii * 2) + 1, ss);
		ss.Format("%0.2f", mcls_MI.gcls_pHtRecv[ii]->GetHtProcMaxTime());		mcnt_pListHtTime->SetItem(4, (ii * 2) + 2, ss);

		// [TO]     統括伝送完了→統括処理完了までの平気、最大所要時間[ms]						(No.8-9)
		ss.Format("%0.2f", mcls_MI.gcls_pHtRecv[ii]->GetAveTime());				mcnt_pListHtTime->SetItem(5, (ii * 2) + 1, ss);
		ss.Format("%0.2f", mcls_MI.gcls_pHtRecv[ii]->GetMaxTime());				mcnt_pListHtTime->SetItem(5, (ii * 2) + 2, ss);

		// [ﾄｰﾀﾙ]   画像取込～統括処理完了までの平均、最大所要時間[ms]							(No.4-9)
		ss.Format("%0.2f", mcls_MI.gcls_pHtRecv[ii]->GetToProcAveTime());		mcnt_pListHtTime->SetItem(6, (ii * 2) + 1, ss);
		ss.Format("%0.2f", mcls_MI.gcls_pHtRecv[ii]->GetToProcMaxTime());		mcnt_pListHtTime->SetItem(6, (ii * 2) + 2, ss);

	}

	//==================================================
	// PLG情報
	nWk = mcls_MI.gcls_pCoil->GetKenRecBase();
	if (0>nWk) nWk = 0;		// 検査未開始時は-1の為の処置
	double	dCoilZan = mcls_MI.gcls_pCoil->GetCoilIndex(nWk, 0)->data.nLenIn - (mcls_MI.gcls_Plg.GetLen_KenDist()) / 1000.0;

	// 検査基点
	ss.Format("%0.3f", mcls_MI.gcls_Plg.GetLen_Ken(0) / 1000.0);			mcnt_pListPlgKen->SetItem(0, 1, ss);
	ss.Format("%0.3f", mcls_MI.gcls_Plg.GetLen_Ken(1) / 1000.0);			mcnt_pListPlgKen->SetItem(1, 1, ss);
	ss.Format("%0.3f", mcls_MI.gcls_Plg.GetLen_KenSub() / 1000.0);		mcnt_pListPlgKen->SetItem(2, 1, ss);
	ss.Format("%0.3f", dCoilZan);										mcnt_pListPlgKen->SetItem(3, 1, ss);
	ss.Format("%0.3f", mcls_MI.gcls_Plg.GetCoilOldLen_Ken() / 1000.0);	mcnt_pListPlgKen->SetItem(4, 1, ss);

	// 表示基点(シャー位置)
	ss.Format("%0.3f", mcls_MI.gcls_Plg.GetLen_BaseDsp() / 1000.0);			mcnt_pListPlgDsp->SetItem(0, 1, ss);
	ss.Format("%0.3f", mcls_MI.gcls_Plg.GetLen_Dsp(0) / 1000.0);			mcnt_pListPlgDsp->SetItem(1, 1, ss);
	ss.Format("%0.3f", mcls_MI.gcls_Plg.GetLen_Dsp(1) / 1000.0);			mcnt_pListPlgDsp->SetItem(2, 1, ss);
	ss.Format("%0.3f", mcls_MI.gcls_Plg.GetLen_Dsp(2) / 1000.0);			mcnt_pListPlgDsp->SetItem(3, 1, ss);
	ss.Format("%0.3f", mcls_MI.gcls_Plg.GetLen_OutCut() / 1000.0);			mcnt_pListPlgDsp->SetItem(4, 1, ss);
	ss.Format("%0.3f", mcls_MI.gcls_Plg.GetCoilOldLen_BaseDsp() / 1000.0);	mcnt_pListPlgDsp->SetItem(5, 1, ss);


	// PLG 基本情報
	ss.Format("%d", mcls_MI.gcls_Plg.GetFrameNo());						mcnt_pListPlg->SetItem(0, 1, ss);
	ss.Format("%d", mcls_MI.gcls_Plg.GetFrameIntervalTime());			mcnt_pListPlg->SetItem(1, 1, ss);
	ss.Format("%0.3f", mcls_MI.gcls_Plg.GetDistance());					mcnt_pListPlg->SetItem(2, 1, ss);
	ss.Format("%d", mcls_MI.gcls_Plg.GetSpeed());						mcnt_pListPlg->SetItem(3, 1, ss);
	ss.Format("%0.6f", mcls_MI.gcls_Plg.GetResV());						mcnt_pListPlg->SetItem(4, 1, ss);
#ifdef RESV_KOTEI
	ss.Format("%0.6f", mcls_MI.gcls_Plg.GetResVCycle());				mcnt_pListPlg->SetItem(5, 1, ss);
#endif
	ss.Format("%d", mcls_MI.gcls_pSogyo->GetSpeed());					mcnt_pListPlg->SetItem(6, 1, ss);
#ifdef DSP_SOGYO_CHECK
	ss.Format("%d", mcls_MI.gcls_pSogyo->GetSpeedDsp());				mcnt_pListPlg->SetItem(7, 1, ss);
#endif
	ss.Format("%u", mcls_MI.gcls_Dmem.GetDmRecv().ken.nFrameNo);		mcnt_pListPlg->SetItem(8, 1, ss);
	ss.Format("%u", mcls_MI.gcls_Dmem.GetDmRecv().ken.nLvalAdd);		mcnt_pListPlg->SetItem(9, 1, ss);
	ss.Format("%u", mcls_MI.gcls_Dmem.GetDmRecv().ken.nPlgAdd);			mcnt_pListPlg->SetItem(10, 1, ss);

	//======================================================
	// コイル情報関連操作
	// ------------------>>>
	mcls_MI.gcls_pCoil->SetCsLock();		// クリティカルセクション ロック 	
											//// 検査用コイル情報
											// 次
	nRow = 0;
	mcnt_pListCoil->SetItem(nRow, 1, mcls_MI.gcls_pCoil->GetCoilNextKen()->rec);
	mcnt_pListCoil->SetItem(nRow, 2, mcls_MI.gcls_pCoil->GetCoilNextKen()->cKizukenNo);
	mcnt_pListCoil->SetItem(nRow, 3, mcls_MI.gcls_pCoil->GetCoilNextKen()->data.cMeiNo);
	mcnt_pListCoil->SetItem(nRow, 4, mcls_MI.gcls_pCoil->GetCoilNextKen()->data.cCoilNoIn);
	mcnt_pListCoil->SetItem(nRow, 5, mcls_MI.gcls_pCoil->GetCoilNextKen()->data.nLenIn);
	mcnt_pListCoil->SetItem(nRow, 6, mcls_MI.gcls_pCoil->GetCoilNextKen()->data.nWid);

	for (int jj = 0; jj<NUM_MEN; jj++) {
		mcnt_pListCoil->SetItem(nRow, 7 + jj, mcls_MI.gcls_pCoil->GetCoilNextKen()->setting.scode[jj]);
		mcnt_pListCoil->SetItem(nRow, 9 + jj, mcls_MI.gcls_pCoil->GetCoilNextKen()->setting.pcode[jj]);
	}
	mcnt_pListCoil->SetItem(nRow, 11, mcls_MI.gcls_pCoil->GetCoilNextKen()->nSt_Spm);
	// 現, 前1, 前2, 前3
	nWk = mcls_MI.gcls_pCoil->GetKenRecBase();
	if (0>nWk) nWk = 0;		// 検査未開始時は-1の為の処置
	for (int ii = 0; ii<SIZE_COIL_BUF; ii++) {
		nRow = 1 + ii;
		mcnt_pListCoil->SetItem(nRow, 1, mcls_MI.gcls_pCoil->GetCoilIndex(nWk, ii)->rec);
		mcnt_pListCoil->SetItem(nRow, 2, mcls_MI.gcls_pCoil->GetCoilIndex(nWk, ii)->cKizukenNo);
		mcnt_pListCoil->SetItem(nRow, 3, mcls_MI.gcls_pCoil->GetCoilIndex(nWk, ii)->data.cMeiNo);
		mcnt_pListCoil->SetItem(nRow, 4, mcls_MI.gcls_pCoil->GetCoilIndex(nWk, ii)->data.cCoilNoIn);
		mcnt_pListCoil->SetItem(nRow, 5, mcls_MI.gcls_pCoil->GetCoilIndex(nWk, ii)->data.nLenIn);
		mcnt_pListCoil->SetItem(nRow, 6, mcls_MI.gcls_pCoil->GetCoilIndex(nWk, ii)->data.nWid);
		for (int jj = 0; jj < NUM_MEN; jj++)
		{
			int nScode = (0 != mcls_MI.gcls_pCoil->GetCoilIndex(nWk, ii)->nSt_Spm ? mcls_MI.gcls_pCoil->GetCoilIndex(nWk, ii)->setting.scode[jj] : mcls_MI.gcls_pCoil->GetCoilIndex(nWk, ii)->setting.scode_spmoff[jj]);
			int nPcode = (0 != mcls_MI.gcls_pCoil->GetCoilIndex(nWk, ii)->nSt_Spm ? mcls_MI.gcls_pCoil->GetCoilIndex(nWk, ii)->setting.pcode[jj] : mcls_MI.gcls_pCoil->GetCoilIndex(nWk, ii)->setting.pcode_spmoff[jj]);
			mcnt_pListCoil->SetItem(nRow, 7 + jj, nScode);
			mcnt_pListCoil->SetItem(nRow, 9 + jj, nPcode);
		}
		mcnt_pListCoil->SetItem(nRow, 11, mcls_MI.gcls_pCoil->GetCoilIndex(nWk, ii)->nSt_Spm);
	}

	////// 出側コイル情報
	//nRow = 0;
	//nWk = mcls_MI.gcls_pCoil->GetShaRecBase();

	//if (0>nWk) nWk = 0;		//  検査未開始時は-1の為の処置
	//for (int ii = 0; ii<SIZE_COIL_OUT_BUF; ii++) {
	//	nRow = (ii * 2);
	//	//ss.Format("%d", mcls_MI.gcls_pCoil->GetShaRecIndex(nWk, ii));						mcnt_pListOutCoil->SetItem(nRow, 1, ss);
	//	ss.Format("N%d", mcls_MI.gcls_pCoil->GetCoilOutIndex(nWk, ii)->nCnt);				mcnt_pListOutCoil->SetItem(nRow + 1, 1, ss);
	//	ss.Format("%s", mcls_MI.gcls_pCoil->GetCoilOutIndex(nWk, ii)->cOutKizukenNo);		mcnt_pListOutCoil->SetItem(nRow, 2, ss);
	//	for (int jj = 0; jj<MAX_SHALINK_COUNT / 2; jj++) {
	//		if (NULL != mcls_MI.gcls_pCoil->GetCoilOutIndex(nWk, ii)->incoil[jj].pCoilIn) {
	//			ss.Format("%s", mcls_MI.gcls_pCoil->GetCoilOutIndex(nWk, ii)->incoil[jj].pCoilIn->data.cCoilNoIn);	mcnt_pListOutCoil->SetItem(nRow, 3 + jj, ss);
	//			ss.Format("%0.4f", mcls_MI.gcls_pCoil->GetCoilOutIndex(nWk, ii)->incoil[jj].nLength / 1000.0);		mcnt_pListOutCoil->SetItem(nRow + 1, 3 + jj, ss);
	//		}
	//		else {
	//			mcnt_pListOutCoil->SetItem(nRow, 3 + jj, "");
	//			mcnt_pListOutCoil->SetItem(nRow + 1, 3 + jj, "");
	//		}
	//	}

	//	//ss.Format("N:%d", mcls_MI.gcls_pCoil->GetCoilOutIndex(nWk, ii)->nCnt);				mcnt_pListOutCoil->SetItem(nRow+1, 1, ss);
	//	ss.Format("F:%0.4f / T:%0.4f", mcls_MI.gcls_pCoil->GetCoilOutIndex(nWk, ii)->nFCutLen / 1000.0, mcls_MI.gcls_pCoil->GetCoilOutIndex(nWk, ii)->nTCutLen / 1000.0);
	//	mcnt_pListOutCoil->SetItem(nRow + 1, 2, ss);
	//}

	//======================================================
	// 設備位置トラッキング (検査位置+表示基点)
	mcnt_pListSetuDsp->SetItem(0, 1, mcls_MI.gcls_pCoil->GetCoilKenR(0)->rec);
	mcnt_pListSetuDsp->SetItem(1, 1, mcls_MI.gcls_pCoil->GetCoilKenR(1)->rec);
	mcnt_pListSetuDsp->SetItem(2, 1, mcls_MI.gcls_pCoil->GetCoilNextKen()->rec);
	if(NULL != mcls_MI.gcls_pCoil->GetCoilBaseDsp())
		mcnt_pListSetuDsp->SetItem(3, 1, mcls_MI.gcls_pCoil->GetCoilBaseDsp()->rec);
	mcnt_pListSetuDsp->SetItem(0, 2, mcls_MI.gcls_pCoil->GetCoilKenR(0)->data.cCoilNoIn);
	mcnt_pListSetuDsp->SetItem(1, 2, mcls_MI.gcls_pCoil->GetCoilKenR(1)->data.cCoilNoIn);
	mcnt_pListSetuDsp->SetItem(2, 2, mcls_MI.gcls_pCoil->GetCoilNextKen()->data.cCoilNoIn);
	if (NULL != mcls_MI.gcls_pCoil->GetCoilBaseDsp())
		mcnt_pListSetuDsp->SetItem(3, 2, mcls_MI.gcls_pCoil->GetCoilBaseDsp()->data.cCoilNoIn);

	// 表示基点 以降
	nRow = 3;
	for (int ii = POS_DSP_P1; ii < POS_DSP_END; ii++) {
		COIL_INF const* pC = mcls_MI.gcls_pCoil->GetCoilDsp(ii);

		if (NULL != pC) {
			mcnt_pListSetuDsp->SetItem(nRow, 1, pC->rec);
			mcnt_pListSetuDsp->SetItem(nRow, 2, pC->data.cCoilNoIn);

		}
		else {
			mcnt_pListSetuDsp->SetItem(nRow, 1, -1);
			mcnt_pListSetuDsp->SetItem(nRow, 2, "");
		}
		nRow++;
	}
	mcnt_pListSetuDsp->SetItem(nRow, 2, mcls_MI.gcls_pCoil->GetCoilNextDsp(0)->CoilInf.KeyInf.cCoilNo);
	
	mcls_MI.gcls_pCoil->SetCsUnLock();		// クリティカルセクション アンロック
	
	//======================================================
	// DB情報
	mcnt_pListDbInfo->SetItem(0, 1, mcls_MI.gcls_DB.GetQueData_Count());
	mcnt_pListDbInfo->SetItem(0, 2, mcls_MI.gcls_DB.GetQueData_Max());
	mcnt_pListDbInfo->SetItem(1, 1, mcls_MI.gcls_DB.GetQueDB_Count());
	mcnt_pListDbInfo->SetItem(1, 2, mcls_MI.gcls_DB.GetQueDB_Max());

	ss.Format("%0.1f", mcls_MI.gcls_DB.GetAveTime()); mcnt_pListDbInfo->SetItem(2, 1, ss);
	ss.Format("%0.1f", mcls_MI.gcls_DB.GetMaxTime()); mcnt_pListDbInfo->SetItem(2, 2, ss);
	ss.Format("%0.1f", mcls_MI.gcls_DB.GetAveSqlTime()); mcnt_pListDbInfo->SetItem(3, 1, ss);
	ss.Format("%0.1f", mcls_MI.gcls_DB.GetMaxSqlTime()); mcnt_pListDbInfo->SetItem(3, 2, ss);
	
	// BCP情報
	mcnt_pListDbInfo->SetItem(4, 1, mcls_MI.gcls_DB.GetBcpSaveMgr().GetQueData_Count());
	mcnt_pListDbInfo->SetItem(4, 2, mcls_MI.gcls_DB.GetBcpSaveMgr().GetQueData_Max());
	//mcnt_pListDbInfo->SetItem(5, 1, mcls_MI.gcls_DB.GetBcpSaveMgr().GetQueDB_Count());
	//mcnt_pListDbInfo->SetItem(5, 2, mcls_MI.gcls_DB.GetBcpSaveMgr().GetQueDB_Max());

	ss.Format("%0.1f", mcls_MI.gcls_DB.GetBcpSaveMgr().GetAveTime()); mcnt_pListDbInfo->SetItem(5, 1, ss);
	ss.Format("%0.1f", mcls_MI.gcls_DB.GetBcpSaveMgr().GetMaxTime()); mcnt_pListDbInfo->SetItem(5, 2, ss);

	ss.Format("%0.1f", mcls_MI.gcls_DB.GetBcpSaveMgr().GetAttrSendRowAveTime()); mcnt_pListDbInfo->SetItem(6, 1, ss);
	ss.Format("%0.1f", mcls_MI.gcls_DB.GetBcpSaveMgr().GetAttrSendRowMaxTime()); mcnt_pListDbInfo->SetItem(6, 2, ss);
	ss.Format("%0.1f", mcls_MI.gcls_DB.GetBcpSaveMgr().GetAttrBcpBatchAveTime()); mcnt_pListDbInfo->SetItem(7, 1, ss);
	ss.Format("%0.1f", mcls_MI.gcls_DB.GetBcpSaveMgr().GetAttrBcpBatchMaxTime()); mcnt_pListDbInfo->SetItem(7, 2, ss);
	ss.Format("%0.1f", mcls_MI.gcls_DB.GetBcpSaveMgr().GetImgSendRowAveTime()); mcnt_pListDbInfo->SetItem(8, 1, ss);
	ss.Format("%0.1f", mcls_MI.gcls_DB.GetBcpSaveMgr().GetImgSendRowMaxTime()); mcnt_pListDbInfo->SetItem(8, 2, ss);
	ss.Format("%0.1f", mcls_MI.gcls_DB.GetBcpSaveMgr().GetImgBcpBatchAveTime()); mcnt_pListDbInfo->SetItem(9, 1, ss);
	ss.Format("%0.1f", mcls_MI.gcls_DB.GetBcpSaveMgr().GetImgBcpBatchMaxTime()); mcnt_pListDbInfo->SetItem(9, 2, ss);

	// DB
	for (int ii = 0; ii<DbBaseManager::SIZE_SQLINF_NUM; ii++) {
		mcnt_pListDb->SetItem(ii, 1, mcls_MI.gcls_DB.GetInf_KizukenNo(ii));
		ss.Format("%d/%d", mcls_MI.gcls_DB.GetInf_SaveCnt(ii, 0), mcls_MI.gcls_DB.GetInf_SaveCnt(ii, 1));					mcnt_pListDb->SetItem(ii, 2, ss);
		ss.Format("%d/%d", mcls_MI.gcls_DB.GetInf_TotalCnt(ii, 0), mcls_MI.gcls_DB.GetInf_TotalCnt(ii, 1));					mcnt_pListDb->SetItem(ii, 3, ss);
		ss.Format("%d/%d", mcls_MI.gcls_DB.GetInf_LowGrDeleCnt(ii, 0), mcls_MI.gcls_DB.GetInf_LowGrDeleCnt(ii, 1));			mcnt_pListDb->SetItem(ii, 4, ss);
		ss.Format("%d/%d", mcls_MI.gcls_DB.GetInf_DeleCnt(ii, 0), mcls_MI.gcls_DB.GetInf_DeleCnt(ii, 1));					mcnt_pListDb->SetItem(ii, 5, ss);
		ss.Format("%d/%d", mcls_MI.gcls_DB.GetInf_ImgLowGrDeleCnt(ii, 0), mcls_MI.gcls_DB.GetInf_ImgLowGrDeleCnt(ii, 1));	mcnt_pListDb->SetItem(ii, 6, ss);
		ss.Format("%d/%d", mcls_MI.gcls_DB.GetInf_ImgDeleCnt(ii, 0), mcls_MI.gcls_DB.GetInf_ImgDeleCnt(ii, 1));				mcnt_pListDb->SetItem(ii, 7, ss);
	}



#ifdef DEBUG_DB_TIME
	mcls_pL->Write(KizuLib::LogFileManager::em_NON, "SQL=,%d,DB=,%d,Cnt=,%d,%d",
		mcls_MI.gcls_DB.GetQueData_Count(), mcls_MI.gcls_DB.GetQueDB_Count(),
		mcls_MI.gcls_DB.GetInf_TotalCnt(0, 0), mcls_MI.gcls_DB.GetInf_TotalCnt(0, 1));
#endif


	//======================================================
	// OP情報
	for(int ii=0; ii<HY_SOCK_NUM; ii++) {
		mcnt_pListOp->SetItem(ii * 2 + 0, 1, mcls_MI.gcls_pOpS[ii]->GetConnectCount());
		mcnt_pListOp->SetItem(ii * 2 + 1, 1, mcls_MI.gcls_pOpS[ii]->gque_Deli.GetCount());
		mcnt_pListOp->SetItem(ii * 2 + 1, 2, mcls_MI.gcls_pOpS[ii]->gque_Deli.GetMaxCount());
	}



	//======================================================
	// エッジ検出
	float dWk1, dWk2;

	mcnt_pListEdge->SetItem(0, 1, mcls_MI.gcls_EdgePosCont.GetDivEdgeMode(mcls_MI.gcls_EdgePosCont.GetEdgeMode(0)));
	mcnt_pListEdge->SetItem(1, 1, mcls_MI.gcls_EdgePosCont.GetDivEdgeMode(mcls_MI.gcls_EdgePosCont.GetEdgeMode(1)));

	mcls_MI.gcls_EdgePosCont.SearchEdgePos(0, dWk1, dWk2);
	ss.Format("%d", (int)dWk1);		mcnt_pListEdge->SetItem(0, 2, ss);
	ss.Format("%d", (int)dWk2);		mcnt_pListEdge->SetItem(0, 3, ss);
	mcls_MI.gcls_EdgePosCont.SearchEdgePos(1, dWk1, dWk2);
	ss.Format("%d", (int)dWk1);		mcnt_pListEdge->SetItem(1, 2, ss);
	ss.Format("%d", (int)dWk2);		mcnt_pListEdge->SetItem(1, 3, ss);

	mcls_MI.gcls_EdgePosCont.SearchEdgeRawPos(0, dWk1, dWk2);
	ss.Format("%d", (int)dWk1);		mcnt_pListEdge->SetItem(0, 4, ss);
	ss.Format("%d", (int)dWk2);		mcnt_pListEdge->SetItem(0, 5, ss);
	mcls_MI.gcls_EdgePosCont.SearchEdgeRawPos(1, dWk1, dWk2);
	ss.Format("%d", (int)dWk1);		mcnt_pListEdge->SetItem(1, 4, ss);
	ss.Format("%d", (int)dWk2);		mcnt_pListEdge->SetItem(1, 5, ss);
	
	//======================================================
	//// 機器情報関連
	mcnt_pListKiki->SetItem(0, 1, GetSeqStat((EM_DIV_SEQ_STAT)mcls_MI.gcls_Status.GetStatusRead().SeqState.stat));
	mcnt_pListKiki->SetItem(1, 1, GetSeqKadou((EM_DIV_SEQ_KADOU)mcls_MI.gcls_Status.GetStatusRead().SeqKadou.stat));
	mcnt_pListKiki->SetItem(2, 1, GetDaisyaPos((DIV_DAISYA_POS)mcls_MI.gcls_Status.GetStatusRead().DaisyaSt[0].stat));
	mcnt_pListKiki->SetItem(3, 1, GetDaisyaPos((DIV_DAISYA_POS)mcls_MI.gcls_Status.GetStatusRead().DaisyaSt[1].stat));
	mcnt_pListKiki->SetItem(4, 1, GetSpmStat((EM_DIV_SPM_STAT)mcls_MI.gcls_Status.GetSpmStat()));
	mcnt_pListKiki->SetItem(5, 1, GetSprayStat((DIV_SPRAY_STAT)mcls_MI.gcls_Status.GetStatusRead().ToyuSprSt.stat));

	ss.Format("%s/%s/%s", GetChilerStop((DIV_CHILER_STOP)mcls_MI.gcls_Status.GetStatusRead().ChilerPiStop[0].stat),
		GetChilerStop((DIV_CHILER_STOP)mcls_MI.gcls_Status.GetStatusRead().ChilerPiStop[1].stat),
		GetChilerStop((DIV_CHILER_STOP)mcls_MI.gcls_Status.GetStatusRead().ChilerPiStop[2].stat));
	mcnt_pListKiki->SetItem(6, 1, ss);
	ss.Format("%s/%s/%s", GetChilerErr((DIV_CHILER_ERR)mcls_MI.gcls_Status.GetStatusRead().ChilerPiErr[0].stat),
		GetChilerErr((DIV_CHILER_ERR)mcls_MI.gcls_Status.GetStatusRead().ChilerPiErr[1].stat),
		GetChilerErr((DIV_CHILER_ERR)mcls_MI.gcls_Status.GetStatusRead().ChilerPiErr[2].stat));
	mcnt_pListKiki->SetItem(7, 1, ss);
	for (int ii = 0; ii < NUM_MEN; ii++) {
		ss.Format("%s/%s", GetDryerStop((DIV_DRYER_STOP)mcls_MI.gcls_Status.GetStatusRead().airblow[ii].AirBlowPiStop.stat),
			GetDryerErr((DIV_DRYER_ERR)mcls_MI.gcls_Status.GetStatusRead().airblow[ii].AirBlowPiErr.stat));
		mcnt_pListKiki->SetItem(8 + ii, 1, ss);
	}
	ss.Format("%s/%s", GetDoorStat((DIV_DOOR_STAT)mcls_MI.gcls_Status.GetStatusRead().SecurityDoor.stat),
		GetKeyStat((DIV_KEY_STAT)mcls_MI.gcls_Status.GetStatusRead().SecurityKey.stat));
	mcnt_pListKiki->SetItem(10, 1, ss);

	//======================================================
	//// ボイス状態
	if (true) {
		nWk = mcls_MI.gcls_Dmem.GetVoiceMen();
		if (0 == nWk)		ss = "表";
		else if (1 == nWk)	ss = "裏";
		else				ss = "";
		mcnt_pListVoice->SetItem(0, 0, ss);
		mcnt_pListVoice->SetItem(0, 1, GetVoiceTypeName( mcls_MI.gcls_Dmem.GetVoiceSts()));
	}
	//======================================================
	//// 疵接近ランプ状態
	if (true) {
		for (int ii = 0; ii < 3; ii++) {
			for (int jj = 0; jj < 8; jj++) {
				if (2 > ii)
				{
					nWk = mcls_MI.gcls_Dmem.GetRampSts(ii, jj);
				}
				else
				{
					nWk = mcls_MI.gcls_Dmem.GetAlrmSts(jj);
				}
				if (0 == nWk)		ss = "○";
				else if (1 == nWk)	ss = "重";
				else				ss = "";
				mcnt_pListRamp->SetItem(ii, jj + 1, ss);
			}
		}
	}

	//======================================================
	//// パトライト状態
	if (true) {
		for (int ii = 0; ii < 2; ii++) {
			for (int jj = 0; jj < 2; jj++) {
				mcnt_pListPato->SetItem(ii, jj+1, mcls_MI.gcls_Dmem.GetPatoSts(ii, jj));
			}
		}
		for (int ii = 0; ii < 2; ii++)
		{
			int nOutputCnt = 0;
			for (int jj = 0; jj < 2; jj++)
			{
				nOutputCnt += mcls_MI.gcls_Dmem.GetPatoSts(jj, ii);
			}
			mcnt_pListPato->SetItem(2, ii + 1, nOutputCnt);
		}
	}

	//======================================================
	//// データメモリ
	if (true) {
		//mcnt_pListDmem->SetItem(0, 1, (mcls_MI.gcls_Dmem.GetIjmPb() ? "○" : "×"));				// マーキングPB状態
		//mcnt_pListDmem->SetItem(1, 1, (mcls_MI.gcls_Dmem.GetIjmAllow() ? "○" : "×"));				// マーク指令許可信号状態
		//mcnt_pListDmem->SetItem(2, 1, (mcls_MI.gcls_Dmem.GetIjmHealthy() ? "○" : "×"));			// マーク監視装置ヘルシー状態
		//mcnt_pListDmem->SetItem(3, 1, (mcls_MI.gcls_Dmem.GetIjmKado() ? "○" : "×"));				// マーク監視装置 稼動状態検査中
		//mcnt_pListDmem->SetItem(4, 1, (mcls_MI.gcls_Dmem.GetIjmKensa() ? "○" : "×"));				// マーク監視装置 検査状態異常
		//mcnt_pListDmem->SetItem(5, 1, (mcls_MI.gcls_Dmem.GetIjmKiki() ? "○" : "×"));				// マーク監視装置 機器状態異常
		//mcnt_pListDmem->SetItem(0, 3, (mcls_MI.gcls_Dmem.GetIjmWarning() ? "○" : "×"));			// マーク警報
		//mcnt_pListDmem->SetItem(1, 3, (mcls_MI.gcls_Dmem.GetIjmError() ? "○" : "×"));				// マーク異常
		//mcnt_pListDmem->SetItem(2, 3, (mcls_MI.gcls_Dmem.GetIjmEnable() ? "○" : "×"));			// マーク印字可能
		//mcnt_pListDmem->SetItem(3, 3, (1 == mcls_MI.gcls_Status.GetStatusRead().IjmUse.stat ? "有り" : "無し"));		// マーク使用有無
		//mcnt_pListDmem->SetItem(4, 3, (0 == mcls_MI.gcls_Status.GetStatusRead().IjmNirekoKado.stat ? "○" : "×"));		// ニレコセンサー稼働中
		//mcnt_pListDmem->SetItem(5, 3, (0 == mcls_MI.gcls_Status.GetStatusRead().IjmNirekoErr.stat ? "○" : "×"));		// ニレコセンサー異常中
	}

	if (!m_bInit)
	{
		m_bInit = true;

		if (NULL != mcls_MI.gcls_pParam)
		{
			CString sWk;
			sWk.Format("%d", mcls_MI.gcls_pParam->GetCommonMast().nLogiParaCam[0][0]);
			CWnd::SetDlgItemTextA(IDC_PARA_CAM1_1, sWk);
			sWk.Format("%d", mcls_MI.gcls_pParam->GetCommonMast().nLogiParaCam[0][1]);
			CWnd::SetDlgItemTextA(IDC_PARA_CAM1_2, sWk);
			sWk.Format("%d", mcls_MI.gcls_pParam->GetCommonMast().nLogiParaCam[1][0]);
			CWnd::SetDlgItemTextA(IDC_PARA_CAM2_1, sWk);
			sWk.Format("%d", mcls_MI.gcls_pParam->GetCommonMast().nLogiParaCam[1][1]);
			CWnd::SetDlgItemTextA(IDC_PARA_CAM2_2, sWk);
		}
	}
}


// //////////////////////////////////////////////////////////////////////////////////////////////////////
// デバッグボタン

//==========================================
// PIO取得先を共有メモリに変更
//==========================================

void CTO_DEFECTDlg::OnBnClickedSmyuPio()
{
	//if (BST_CHECKED != CWnd::IsDlgButtonChecked(IDC_DEBUG_MANUAL))	return;

	////// シミュレータ設定
	//bool bMode = (BST_CHECKED == CWnd::IsDlgButtonChecked(IDC_SMYU_PIO) ? true : false);

	//LOG(em_WAR), "[DEBUG] PIO接続先 (%s)", bMode ? "ｼﾐｭﾚｰﾀ" : "通常");
	////mcls_MI.gcls_Dmem.SetSmyuMode(bMode);
}

//==========================================
// PLG取得先を共有メモリに変更
//==========================================
void CTO_DEFECTDlg::OnBnClickedSmyuPlg()
{
	//if (BST_CHECKED != CWnd::IsDlgButtonChecked(IDC_DEBUG_MANUAL)) return;

	////// シミュレータ設定
	//bool bMode = (BST_CHECKED == CWnd::IsDlgButtonChecked(IDC_SMYU_PLG) ? true : false);

	//LOG(em_WAR), "[DEBUG] PLG接続先 (%s)", bMode ? "ｼﾐｭﾚｰﾀ" : "通常" );
	//if (!mcls_MI.gcls_Plg.SetSmyuMode(bMode)) {
	//	LOG(em_ERR), "[Dlg] PLG シミュレータ用共有メモリアクセス失敗" );
	//}
}

//==========================================
// デバッグメニュー コマンドボタンクリック
//==========================================
void CTO_DEFECTDlg::OnNMClickListCmd(NMHDR *pNMHDR, LRESULT *pResult)
{
	//LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	//// TODO: ここにコントロール通知ハンドラー コードを追加します。
	*pResult = 0;

	if (BST_CHECKED != CWnd::IsDlgButtonChecked(IDC_DEBUG_MANUAL)) return;


	//// 選択された行を特定
	int idx = SListBox::GetClickItemIndex(pNMHDR);		// 0オリジン。変なところは-1
	if (-1 == idx) return;

	switch (idx) {
		//------------------------------------------------------------------------------
	case E_DEBUG_START:								// 検査開始
		if (true) {
			EM_DIV_UNTEN		nUnten = DIV_UNTEN_NOMAL;
			if (BST_CHECKED == CWnd::IsDlgButtonChecked(IDC_RAD_DEBUG_UNTEN1)) nUnten = DIV_UNTEN_NOMAL;
			if (BST_CHECKED == CWnd::IsDlgButtonChecked(IDC_RAD_DEBUG_UNTEN2)) nUnten = DIV_UNTEN_SAMP;
			if (BST_CHECKED == CWnd::IsDlgButtonChecked(IDC_RAD_DEBUG_UNTEN3)) nUnten = DIV_UNTEN_CAMERA;
			if (BST_CHECKED == CWnd::IsDlgButtonChecked(IDC_RAD_DEBUG_UNTEN4)) nUnten = DIV_UNTEN_SMYU;
			EM_DIV_PLG			nPlg = DIV_PLG_LINE;
			if (BST_CHECKED == CWnd::IsDlgButtonChecked(IDC_RAD_DEBUG_PLG1)) nPlg = DIV_PLG_LINE;
			if (BST_CHECKED == CWnd::IsDlgButtonChecked(IDC_RAD_DEBUG_PLG2)) nPlg = DIV_PLG_TOP;
			if (BST_CHECKED == CWnd::IsDlgButtonChecked(IDC_RAD_DEBUG_PLG3)) nPlg = DIV_PLG_BOT;
			if (BST_CHECKED == CWnd::IsDlgButtonChecked(IDC_RAD_DEBUG_PLG4)) nPlg = DIV_PLG_TEST;
			bool				bMenMode = false;
			if (BST_CHECKED == CWnd::IsDlgButtonChecked(IDC_RAD_DEBUG_MEN1)) bMenMode = false;
			if (BST_CHECKED == CWnd::IsDlgButtonChecked(IDC_RAD_DEBUG_MEN2)) bMenMode = true;

			int scode[NUM_MEN], pcode[NUM_MEN];
			scode[0] = CWnd::GetDlgItemInt(IDC_DEF_SCODE1);
			scode[1] = CWnd::GetDlgItemInt(IDC_DEF_SCODE2);
			pcode[0] = CWnd::GetDlgItemInt(IDC_DEF_PCODE1);
			pcode[1] = CWnd::GetDlgItemInt(IDC_DEF_PCODE2);

			int	nLogiParaCam[NUM_ALL_HTPARA][NUM_CAMLINK_BOARD];
			int nWk = CWnd::GetDlgItemInt(IDC_PARA_CAM1_1);
			if (1 > nWk) CWnd::SetDlgItemInt(IDC_PARA_CAM1_1, 1);
			if (20 < nWk) CWnd::SetDlgItemInt(IDC_PARA_CAM1_1, 20);
			nWk = CWnd::GetDlgItemInt(IDC_PARA_CAM1_2);
			if (1 > nWk) CWnd::SetDlgItemInt(IDC_PARA_CAM1_2, 1);
			if (20 < nWk) CWnd::SetDlgItemInt(IDC_PARA_CAM1_2, 20);
			nWk = CWnd::GetDlgItemInt(IDC_PARA_CAM2_1);
			if (1 > nWk) CWnd::SetDlgItemInt(IDC_PARA_CAM2_1, 1);
			if (20 < nWk) CWnd::SetDlgItemInt(IDC_PARA_CAM2_1, 20);
			nWk = CWnd::GetDlgItemInt(IDC_PARA_CAM2_2);
			if (1 > nWk) CWnd::SetDlgItemInt(IDC_PARA_CAM2_2, 1);
			if (20 < nWk) CWnd::SetDlgItemInt(IDC_PARA_CAM2_2, 20);
			nLogiParaCam[0][0] = CWnd::GetDlgItemInt(IDC_PARA_CAM1_1);
			nLogiParaCam[0][1] = CWnd::GetDlgItemInt(IDC_PARA_CAM1_2);
			nLogiParaCam[1][0] = CWnd::GetDlgItemInt(IDC_PARA_CAM2_1);
			nLogiParaCam[1][1] = CWnd::GetDlgItemInt(IDC_PARA_CAM2_2);

			// 検査開始条件により処理わけ
			LOG(em_WAR), "[DEBUG] 探傷開始ボタン [%s, %s, %s, s=%d:%d, p=%d:%d para1(cam1:%d cam2:%d) para2(cam1:%d cam2:%d)]",
				DivNameManager::GetDivUnten(nUnten), DivNameManager::GetDivPlg(nPlg), mcls_MI.gcls_Status.GetMenModeName(bMenMode),
				scode[0], scode[1], pcode[0], pcode[1],
				nLogiParaCam[0][0], nLogiParaCam[0][1], nLogiParaCam[1][0], nLogiParaCam[1][1]);

				COMMON_QUE que;
				que.nEventNo = FACT_TO_DEFECT_02;
				que.nLineNo = getlineid();
				que.fl.data[0] = (int)nUnten;
				que.fl.data[1] = (int)nPlg;
				que.fl.data[2] = (int)bMenMode;
				que.fl.data[3] = scode[0];
				que.fl.data[4] = scode[1];
				que.fl.data[5] = pcode[0];
				que.fl.data[6] = pcode[1];
				que.fl.data[7] = nLogiParaCam[0][0];
				que.fl.data[8] = nLogiParaCam[0][1];
				que.fl.data[9] = nLogiParaCam[1][0];
				que.fl.data[10] = nLogiParaCam[1][1];
				send_mail(TO_DEFECT, ".", &que);
		}
		break;

		//------------------------------------------------------------------------------
	case E_DEBUG_STOP:								// 検査停止
		if (true) {
			LOG(em_WAR), "[DEBUG] 探傷停止ボタン" );

			COMMON_QUE que;
			que.nEventNo = FACT_TO_DEFECT_03;
			que.nLineNo = getlineid();
			send_mail(TO_DEFECT, ".", &que);
		}
		break;

		//------------------------------------------------------------------------------
	case E_DEBUG_RESUME_START:						// 途中開始
		if (true)
		{
			EM_DIV_UNTEN		nUnten = DIV_UNTEN_NOMAL;
			if (BST_CHECKED == CWnd::IsDlgButtonChecked(IDC_RAD_DEBUG_UNTEN1)) nUnten = DIV_UNTEN_NOMAL;
			if (BST_CHECKED == CWnd::IsDlgButtonChecked(IDC_RAD_DEBUG_UNTEN2)) nUnten = DIV_UNTEN_SAMP;
			if (BST_CHECKED == CWnd::IsDlgButtonChecked(IDC_RAD_DEBUG_UNTEN3)) nUnten = DIV_UNTEN_CAMERA;
			if (BST_CHECKED == CWnd::IsDlgButtonChecked(IDC_RAD_DEBUG_UNTEN4)) nUnten = DIV_UNTEN_SMYU;
			EM_DIV_PLG			nPlg = DIV_PLG_LINE;
			if (BST_CHECKED == CWnd::IsDlgButtonChecked(IDC_RAD_DEBUG_PLG1)) nPlg = DIV_PLG_LINE;
			if (BST_CHECKED == CWnd::IsDlgButtonChecked(IDC_RAD_DEBUG_PLG2)) nPlg = DIV_PLG_TOP;
			if (BST_CHECKED == CWnd::IsDlgButtonChecked(IDC_RAD_DEBUG_PLG3)) nPlg = DIV_PLG_BOT;
			if (BST_CHECKED == CWnd::IsDlgButtonChecked(IDC_RAD_DEBUG_PLG4)) nPlg = DIV_PLG_TEST;
			bool				bMenMode = false;
			if (BST_CHECKED == CWnd::IsDlgButtonChecked(IDC_RAD_DEBUG_MEN1)) bMenMode = false;
			if (BST_CHECKED == CWnd::IsDlgButtonChecked(IDC_RAD_DEBUG_MEN2)) bMenMode = true;

			int scode[NUM_MEN], pcode[NUM_MEN];
			scode[0] = CWnd::GetDlgItemInt(IDC_DEF_SCODE1);
			scode[1] = CWnd::GetDlgItemInt(IDC_DEF_SCODE2);
			pcode[0] = CWnd::GetDlgItemInt(IDC_DEF_PCODE1);
			pcode[1] = CWnd::GetDlgItemInt(IDC_DEF_PCODE2);

			int	nLogiParaCam[NUM_ALL_HTPARA][NUM_CAMLINK_BOARD];
			int nWk = CWnd::GetDlgItemInt(IDC_PARA_CAM1_1);
			if (1 > nWk) CWnd::SetDlgItemInt(IDC_PARA_CAM1_1, 1);
			if (20 < nWk) CWnd::SetDlgItemInt(IDC_PARA_CAM1_1, 20);
			nWk = CWnd::GetDlgItemInt(IDC_PARA_CAM1_2);
			if (1 > nWk) CWnd::SetDlgItemInt(IDC_PARA_CAM1_2, 1);
			if (20 < nWk) CWnd::SetDlgItemInt(IDC_PARA_CAM1_2, 20);
			nWk = CWnd::GetDlgItemInt(IDC_PARA_CAM2_1);
			if (1 > nWk) CWnd::SetDlgItemInt(IDC_PARA_CAM2_1, 1);
			if (20 < nWk) CWnd::SetDlgItemInt(IDC_PARA_CAM2_1, 20);
			nWk = CWnd::GetDlgItemInt(IDC_PARA_CAM2_2);
			if (1 > nWk) CWnd::SetDlgItemInt(IDC_PARA_CAM2_2, 1);
			if (20 < nWk) CWnd::SetDlgItemInt(IDC_PARA_CAM2_2, 20);
			nLogiParaCam[0][0] = CWnd::GetDlgItemInt(IDC_PARA_CAM1_1);
			nLogiParaCam[0][1] = CWnd::GetDlgItemInt(IDC_PARA_CAM1_2);
			nLogiParaCam[1][0] = CWnd::GetDlgItemInt(IDC_PARA_CAM2_1);
			nLogiParaCam[1][1] = CWnd::GetDlgItemInt(IDC_PARA_CAM2_2);

			int nParaKind = 0;
			if (BST_CHECKED == CWnd::IsDlgButtonChecked(IDC_BTN_PARA2)) nParaKind = 1;

			LOG(em_WAR), "[DEBUG] 途中開始ボタン [%s, %s, %s, s=%d:%d, p=%d:%d para%d(cam1:%d cam2:%d)]",
				DivNameManager::GetDivUnten(nUnten), DivNameManager::GetDivPlg(nPlg), mcls_MI.gcls_Status.GetMenModeName(bMenMode),
				scode[0], scode[1], pcode[0], pcode[1],
				nParaKind + 1, nLogiParaCam[nParaKind][0], nLogiParaCam[nParaKind][1]);

				COMMON_QUE que;
				que.nEventNo = FACT_TO_DEFECT_21;
				que.nLineNo = getlineid();
				que.fl.data[0] = (int)nUnten;
				que.fl.data[1] = (int)nPlg;
				que.fl.data[2] = (int)bMenMode;
				que.fl.data[3] = scode[0];
				que.fl.data[4] = scode[1];
				que.fl.data[5] = pcode[0];
				que.fl.data[6] = pcode[1];
				que.fl.data[7] = nParaKind;
				que.fl.data[8] = nLogiParaCam[nParaKind][0];
				que.fl.data[9] = nLogiParaCam[nParaKind][1];
				send_mail(TO_DEFECT, ".", &que);
		}
		break;

		//------------------------------------------------------------------------------
	case E_DEBUG_RESUME_STOP:						// 途中停止
		if (true)
		{
			int nParaKind = 0;
			if (BST_CHECKED == CWnd::IsDlgButtonChecked(IDC_BTN_PARA2)) nParaKind = 1;

			LOG(em_WAR), "[DEBUG] 途中停止ボタン [パラランPC%d]", nParaKind + 1);

			COMMON_QUE que;
			que.nEventNo = FACT_TO_DEFECT_22;
			que.nLineNo = getlineid();
			que.fl.data[0] = nParaKind;
			send_mail(TO_DEFECT, ".", &que);
		}
		break;

		//------------------------------------------------------------------------------
	case E_DEBUG_HT_CANNOT_CLEAR:					// 判定続行不可能解除
		if (true) {
			LOG(em_WAR), "[DEBUG] 判定続行不能解除ボタン" );
			mcls_MI.Send_HtDefect_PcCanOk();
		}
		break;

		//------------------------------------------------------------------------------
	case E_DEBUG_CAMLINKSW_READY:					// カメラリンクスイッチ設定完了
		if (true) {
			LOG(em_WAR), "[DEBUG] カメラリンクスイッチ設定完了通知ボタン" );

			COMMON_QUE que;
			que.nEventNo = FACT_TO_DEFECT_30;
			que.nLineNo = getlineid();
			que.fl.data[0] = 1;
			send_mail(TO_DEFECT, ".", &que);
		}
		break;

		//------------------------------------------------------------------------------
	case E_DEBUG_CAMLINKSW_CAMSET:					// カメラ選択設定完了
		LOG(em_WAR), "[DEBUG] カメラ選択設定完了通知ボタン" );
		{
			int mode;
			if (BST_CHECKED == CWnd::IsDlgButtonChecked(IDC_BTN_CAMSET_RESULT2))
			{
				// 異常応答
				mode = 1;
			}
			else if (BST_CHECKED == CWnd::IsDlgButtonChecked(IDC_BTN_CAMSET_RESULT3))
			{
				// 伝文不一致
				mode = 2;
			}
			else
			{
				// 正常応答
				mode = 0;
			}
			LogicParaRunFunc::DebugSend_CamLinksw_CamsetCmpl(mode);
		}
		break;

		//------------------------------------------------------------------------------
	case E_DEBUG_PARAM:								// パラメータ取得
		if (true) {
			LOG(em_WAR), "[DEBUG] パラメータ取得ボタン" );

			COMMON_QUE que;
			que.nEventNo = FACT_TO_DEFECT_01;
			que.nLineNo = getlineid();
			que.fl.data[0] = 0;
			que.fl.data[1] = CH_PARAM_ALL;
			que.fl.data[2] = CH_SCODE_ALL;
			send_mail(TO_DEFECT, ".", &que);
		}
		break;

		//------------------------------------------------------------------------------
	case E_DEBUG_RESUME_START_CMPL:					// 途中参加設定完了
		if (true)
		{
			int nParaKind = 0;
			int nJoinFrameNo = mcls_MI.gcls_Plg.GetFrameNo() + NUM_JOIN_FRAME;
			if (BST_CHECKED == CWnd::IsDlgButtonChecked(IDC_BTN_PARA2)) nParaKind = 1;

			LOG(em_WAR), "[DEBUG] 途中参加設定完了ボタン [パラランPC%d frame=%d]", nParaKind + 1, nJoinFrameNo);

			COMMON_QUE que;
			que.nEventNo = FACT_TO_DEFECT_31;
			que.nLineNo = getlineid();
			que.fl.data[0] = 2 + nParaKind;
			que.fl.data[1] = nJoinFrameNo;
			send_mail(TO_DEFECT, ".", &que);
		}
		break;

		//------------------------------------------------------------------------------
	default:
		// ありえない
		break;
	}
}

//==========================================
// デバッグメニュー 入力ボタンクリック
//==========================================
void CTO_DEFECTDlg::OnNMClickListInput(NMHDR *pNMHDR, LRESULT *pResult)
{
	*pResult = 0;

	if (BST_CHECKED != CWnd::IsDlgButtonChecked(IDC_DEBUG_MANUAL)) return;

	//// 選択された行を特定
	int idx = SListBox::GetClickItemIndex(pNMHDR);		// 0オリジン。変なところは-1
	if (-1 == idx) return;

	switch (idx) {
		//------------------------------------------------------------------------------
	case E_DEBUG_RECV_COILINF_KEN:						// 擬似検査コイル受信
		if (true) {
			LOG(em_WAR), "[DEBUG] 擬似検査コイル受信ボタン" );
			DummyCoilCreate(0);			// 擬似検査コイル生成
			COMMON_QUE que;
			que.nEventNo = FACT_TO_DEFECT_11;
			que.nLineNo = getlineid();
			que.mix.idata[0] = 0;
			send_mail(TO_DEFECT, ".", &que);
		}
		break;
		//------------------------------------------------------------------------------
	case E_DEBUG_RECV_COILINF_DSP:						// 擬似表示コイル受信
		if (true) {
			LOG(em_WAR), "[DEBUG] 擬似表示コイル受信ボタン" );
			DummyCoilCreate(1);			// 擬似表示コイル生成
			COMMON_QUE que;
			que.nEventNo = FACT_TO_DEFECT_11;
			que.nLineNo = getlineid();
			que.mix.idata[0] = 1;
			send_mail(TO_DEFECT, ".", &que);
		}
		break;
		//------------------------------------------------------------------------------
	case E_DEBUG_REGET_COILINF_KEN:						// 検査用情報 再受信
		if (true) {
			LOG(em_WAR), "[DEBUG] 検査用情報 再受信ボタン" );
			COMMON_QUE que;
			que.nEventNo = FACT_TO_DEFECT_11;
			que.nLineNo = getlineid();
			que.mix.idata[0] = 0;
			send_mail(TO_DEFECT, ".", &que);
		}
		break;

		//------------------------------------------------------------------------------
	case E_DEBUG_REGET_COILINF_DSP:						// 表示用情報 再受信
		if(true){
			LOG(em_WAR), "[DEBUG] 表示用情報 再受信ボタン" );
			COMMON_QUE que;
			que.nEventNo = FACT_TO_DEFECT_11;
			que.nLineNo  = getlineid();
			que.mix.idata[0] = 1;
			send_mail(TO_DEFECT, ".", &que);
		}
		break;

		//------------------------------------------------------------------------------
	case E_DEBUG_PI_WPD_KEN:							// PI 検査基点WPD検出
		LOG(em_WAR), "[DEBUG] 検査基点WPD検出ボタン");
		mcls_MI.gcls_Dmem.PoPuls_TesetWpdOn(1);
		break;

		//------------------------------------------------------------------------------
	case E_DEBUG_PI_WPD_DSP:							// PI 表示基点WPD検出
		LOG(em_WAR), "[DEBUG] 表示基点WPD検出ボタン");
		mcls_MI.gcls_Dmem.PoPuls_TesetWpdOn(2);
		break;

		//------------------------------------------------------------------------------
	case E_DEBUG_PI_WPD_KEN_DM:							// PI 検査基点WPD検出 DMのみ
		LOG(em_WAR), "[DEBUG] 検査基点WPD検出(DMのみ)ボタン");
		mcls_MI.gcls_Dmem.PoPuls_TesetWpdOn(3);
		break;

		//------------------------------------------------------------------------------
	case E_DEBUG_PI_WPD_DSP_DM:							// PI 表示基点WPD検出 DMのみ
		LOG(em_WAR), "[DEBUG] 表示基点WPD検出(DMのみ)ボタン");
		mcls_MI.gcls_Dmem.PoPuls_TesetWpdOn(4);
		break;

		//------------------------------------------------------------------------------
	case E_DEBUG_INITIAL:							// イニシャル伝文受信通知
		if (true) {
			LOG(em_WAR), "[DEBUG] プロコン イニシャル伝文受信ボタン");
			COMMON_QUE que;
			que.nEventNo = FACT_TO_DEFECT_12;
			que.nLineNo = getlineid();
			send_mail(TO_DEFECT, ".", &que);
		}
		break;

		//------------------------------------------------------------------------------
	case E_DEBUG_PI_SHAR_CUT:						// PI シャーカット生信号
		LOG(em_WAR), "[DEBUG] シャーカット生信号ボタン");
		SetEvent(mcls_MI.gcls_Dmem.GetEvPI_WPD_ShacutF());
		break;

		//------------------------------------------------------------------------------
	case E_DEBUG_PATO_RESET:						// PI パトライトリセット
		LOG(em_WAR), "[DEBUG] パトライトリセット信号ボタン");
		SetEvent(mcls_MI.gcls_Dmem.GetEvPI_WPD_PatoReset());
		break;

		//------------------------------------------------------------------------------
	case E_DEBUG_SEQ_CNT_RESET:						// PO シーケンサカウンタリセット 
		LOG(em_WAR), "[DEBUG] シーケンサカウンタリセット信号ボタン");
		mcls_MI.gcls_Dmem.PoPuls_CountReset();
		break;
		
		//------------------------------------------------------------------------------
	case E_DEBUG_VOICE_RESET_SETU:					// ボイス警報出力解除要求（設備異常警報解除）
		if (true) {
			LOG(em_WAR), "[DEBUG] ボイス警報出力解除要求（設備異常警報解除）" );

			COMMON_QUE que;
			que.nEventNo = FACT_TO_DEFECT_20;
			que.nLineNo = getlineid();
			que.fl.data[0] = 0;
			send_mail(TO_DEFECT, ".", &que);
		}
		break;

		//------------------------------------------------------------------------------
	case E_DEBUG_VOICE_RESET_DEF:					// ボイス警報出力解除要求（欠陥検出警報解除）
		if (true) {
			LOG(em_WAR), "[DEBUG] ボイス警報出力解除要求（欠陥検出警報解除）" );

			COMMON_QUE que;
			que.nEventNo = FACT_TO_DEFECT_20;
			que.nLineNo = getlineid();
			que.fl.data[0] = 1;
			send_mail(TO_DEFECT, ".", &que);
		}
		break;

		//------------------------------------------------------------------------------
	default:
		// ありえない
		break;
	}
}

//==========================================
// LED自動消灯のON/OFF切替
//==========================================
void CTO_DEFECTDlg::OnBnClickedDebugLedonoff()
{

	if (BST_CHECKED != CWnd::IsDlgButtonChecked(IDC_DEBUG_MANUAL)) return;

	bool bVal = (BST_CHECKED == CWnd::IsDlgButtonChecked(IDC_DEBUG_LEDONOFF) ? true : false);
	mcls_MI.gcls_Status.SetLedAutoCheckOff(bVal);
	LOG(em_WAR), "[DEBUG] LED自動消灯 <%s>", (bVal ? "無効" : "有効"));

}


//==========================================
// ダミーコイル情報生成
// nSendPoint: 送信位置(0:検査基点 1:表示基点)
//==========================================
void CTO_DEFECTDlg::DummyCoilCreate(int nSendPoint)
{
	SYSTEMTIME time;
	GetLocalTime(&time);

	TO_COIL_TBL::TO_COIL_REC* p = mcls_MI.gcls_pCoil->GetCoilTbl(nSendPoint);
	memset(p, 0x00, sizeof(TO_COIL_TBL::TO_COIL_REC));

	if(0 == nSendPoint)m_nCoil++;
	if (99 < m_nCoil) m_nCoil = 0;

	char cWk[SIZE_COIL_NO+1];
	//int size = sprintf(cWk, "%02d%02d%02d%02d%02d",
	//	time.wMonth, time.wDay, time.wHour, time.wMinute, time.wSecond);
	int size = sprintf(cWk, "test%02d      ", m_nCoil);

	// 基本情報
	memset(&p->basedata.cCoilNoIn, 0x00, SIZE_COIL_NO+1);
	sprintf(p->basedata.cCoilNoIn, "%s", cWk);
	memset(&p->basedata.cMeiNo, 0x00, SIZE_MEI_NO+1);
	memcpy(&p->basedata.cMeiNo, "0123", SIZE_MEI_NO);
	memcpy(&p->basedata.cCoilKey[0], &p->basedata.cMeiNo, SIZE_MEI_NO);
	memcpy(&p->basedata.cCoilKey[SIZE_MEI_NO], &p->basedata.cCoilNoIn, SIZE_COIL_NO);

	p->basedata.nAtu = 600;
	//p->basedata.nWid = 567;
	p->basedata.nWid = COIL_WID_MAX;
	p->basedata.nLenIn = 1234;

	// 条件
	p->setting.bWalm = true;
	for (int ii = 0; ii < NUM_MEN; ii++)
	{
		p->setting.pcode[ii]		= MAX_PCODE;
		p->setting.scode[ii]		= MAX_SCODE;
		p->setting.pcode_spmoff[ii]	= MAX_PCODE;
		p->setting.scode_spmoff[ii]	= MAX_SCODE;
	}

	// ライン固有
	PRO_TYPE_COILINF* c = (PRO_TYPE_COILINF*)p->val;
	memset(&c->data.CoilInf, 0x20, sizeof(c->data.CoilInf));
	memcpy(c->data.CoilInf.KeyInf.cMeiNo, p->basedata.cMeiNo, sizeof(c->data.CoilInf.KeyInf.cMeiNo));
	memcpy(c->data.CoilInf.KeyInf.cCoilNo, p->basedata.cCoilNoIn, sizeof(c->data.CoilInf.KeyInf.cCoilNo));
	if (0 == nSendPoint)
		c->data.CoilInf.nSendPoint = 0;
	else if (1 == nSendPoint)
		c->data.CoilInf.nSendPoint = 1;
	else
		c->data.CoilInf.nSendPoint = 0;

	int nCoilLen = 2500;
	c->data.CoilInf.nWidZai = p->basedata.nWid;									// 材料巾(入側) [mm]
	c->data.CoilInf.nWidTrim = (int)((double)p->basedata.nWid * 0.9);			// トリム巾 [mm] (トリム無し時は、0)
	c->data.CoilInf.nAtuSei = p->basedata.nAtu;									// 注文厚 [μm]
	c->data.CoilInf.nLenIn = nCoilLen;											// コイル長 [m]
	c->data.CoilInf.nAtuZai = p->basedata.nAtu;									// 材料板厚
	c->data.CoilInf.nWidSei = p->basedata.nWid;									// 材料板幅
	c->data.CoilInf.nNobiSPM = 80;												// SPM伸び率
	c->data.CoilInf.nNobiTLV = 50;												// TLV伸び率
	c->data.CoilInf.nCutF = (int)((double)nCoilLen * 0.01);						// 入側におけるフロント端板カットの実測長
	c->data.CoilInf.nCutT = (int)((double)nCoilLen * 0.02);						// 入側におけるテール端板カットの実測長
	c->data.CoilInf.nSPMRoolKei_Up = 100;										// SPMワークロール径（上）
	c->data.CoilInf.nSPMRoolKei_Down = 200;										// SPMワークロール径（下）
	memcpy(c->data.CoilInf.cKenKijun_O, "    ", 4);								// 検査基準 表
	memcpy(c->data.CoilInf.cKenKijun_U, "    ", 4);								// 検査基準 裏
	memcpy(c->data.CoilInf.cKurometo_Kind, "  ", 2);							// クロメート 実績種類
	memcpy(c->data.CoilInf.cKurometo_Umu, "  ", 2);								// クロメート 実績有無
	memcpy(c->data.CoilInf.cHinCode, " ", 1);									// 品種
	memcpy(c->data.CoilInf.cMekkiKind, " ", 1);									// メッキ種類
	memcpy(c->data.CoilInf.cSyuZai, "  ", 2);									// 主材質
	memcpy(c->data.CoilInf.cDifMaker, " ", 1);									// ディファレンシャルマーカー
	memcpy(c->data.CoilInf.cKenban, " ", 1);									// 原板区分
	memcpy(c->data.CoilInf.cMekkiCycle, " ", 1);								// メッキサイクル
	memcpy(c->data.CoilInf.cInLineSPM, " ", 1);									// レベラー インラインSPM
	memcpy(c->data.CoilInf.cKagakuToyu, " ", 1);								// 化学処理塗油
	memcpy(c->data.CoilInf.cAtukkaSPM, " ", 1);									// オフラインSPM圧下率
	memcpy(c->data.CoilInf.cMekkiKubun, " ", 1);								// メッキ区分
	memcpy(c->data.CoilInf.cOmoteSyori1, " ", 1);								// 表面処理1桁目
	memcpy(c->data.CoilInf.cOmoteSyori2, " ", 1);								// 表面処理2桁目
	memcpy(c->data.CoilInf.cToyu, " ", 1);										// 塗油
	memcpy(c->data.CoilInf.cKouteiYobi, " ", 1);								// 工程記号予備
	memcpy(c->data.CoilInf.cCaplSyuZai, "          ", 10);						// CAPL 主材質
	memcpy(c->data.CoilInf.cMetuke, "        ", 8);								// 目付規格
	memcpy(c->data.CoilInf.cNi, "  ", 2);										// Ni付着量
	memcpy(c->data.CoilInf.cSodoTCM, "  ", 2);									// TCMロール粗度
	memcpy(c->data.CoilInf.cSpmUmu, "  ", 2);									// SPM圧下有無
	memcpy(c->data.CoilInf.cNextLine, "  ", 2);									// 計画次工程コード
	memcpy(c->data.CoilInf.cMuke, "ABCD                            ", 32);		// 向先
	memcpy(c->data.CoilInf.cYouto, "    ", 4);									// 用途
	memcpy(c->data.CoilInf.cCaplSiage, "    ", 4);								// CAPL表面仕上げ
	memcpy(c->data.CoilInf.cCaplSanUmu, "  ", 2);								// CAPL酸洗有無
	memcpy(c->data.CoilInf.cHinMode, " ", 1);									// 品種区分
	memcpy(c->data.CoilInf.cGaCrKubun, " ", 1);									// GA/CR区分
	memcpy(c->data.CoilInf.cSpmJisekiUmu, "  ", 2);								// SPM圧下実績有無
	memcpy(c->data.CoilInf.cKouteiCode, "  ", 2);								// 工程コード

	memcpy(&p->val, c, sizeof(PRO_TYPE_DATA_COILINF));
	return;
}
