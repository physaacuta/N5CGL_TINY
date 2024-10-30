
// HY_DEFECTDlg.cpp : 実装ファイル
//

#include "stdafx.h"
#include "HY_DEFECT.h"
#include "HY_DEFECTDlg.h"
#include "afxdialogex.h"

// メモリーリーク検出 手動追加 ----->>>
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif
// <<<------------------------


// CHY_DEFECTDlg ダイアログ



CHY_DEFECTDlg::CHY_DEFECTDlg(CWnd* pParent /*=NULL*/)
	: CDialog(IDD_HY_DEFECT_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CHY_DEFECTDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, m_ListBox);
	DDX_Control(pDX, IDC_LIST_ST, m_ListSt);
	DDX_Control(pDX, IDC_LIST_REC, m_ListRec);
	DDX_Control(pDX, IDC_LIST_PTR, m_ListPtr);
}

BEGIN_MESSAGE_MAP(CHY_DEFECTDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDOK, &CHY_DEFECTDlg::OnBnClickedOk)
	ON_WM_CLOSE()
	ON_LBN_DBLCLK(IDC_LIST1, &CHY_DEFECTDlg::OnLbnDblclkList1)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_DEBUG_PARAM, &CHY_DEFECTDlg::OnBnClickedDebugParam)
	ON_BN_CLICKED(IDC_MEM_OUTPUT, &CHY_DEFECTDlg::OnBnClickedMemOutput)
	ON_BN_CLICKED(IDC_MEM_INPUT, &CHY_DEFECTDlg::OnBnClickedMemInput)
// 2024.07.26 Kouno 2CGL検査装置φω精度向上機能導入(長周期) ---------------->>>>>
	ON_BN_CLICKED(IDC_DEBUG_LONGPD, &CHY_DEFECTDlg::OnBnClickedDebugLongpd)
// 2024.07.26 Kouno 2CGL検査装置φω精度向上機能導入(長周期) <<<<<----------------
END_MESSAGE_MAP()


// CHY_DEFECTDlg メッセージ ハンドラー

BOOL CHY_DEFECTDlg::OnInitDialog()
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

void CHY_DEFECTDlg::OnPaint()
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
HCURSOR CHY_DEFECTDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

// これ以降 手動追加 -------------------->>>

//￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣
// ×ボタン
//＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿
void CHY_DEFECTDlg::OnClose()
{
	//CDialog::OnClose();
}
//￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣
// 強制終了ボタン
//＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿
void CHY_DEFECTDlg::OnBnClickedOk()
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
void CHY_DEFECTDlg::OnLbnDblclkList1()
{
	// ListBoxコントロールのプロパティは、ソート禁止にしておく事
	// ListBoxコントロールのダブルクリックイベントを追加
	mcls_pLog->ShowEdit();
}

//------------------------------------------
// 各プロセス初期化処理
//------------------------------------------
void CHY_DEFECTDlg::InitProcess()
{
	CString ss;

	//// タスク初期化処理
	if (0 != task_init(HY_DEFECT)) {
		syslog(SYSNO_TASK_INIT_ERR, "[%s]", HY_DEFECT);
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
	// ステータス状態 リストビューインスタンス生成 & 定義
	if (true) {
		mcnt_pListSt = new DspListView(&m_ListSt);
		// 見た目定義
		m_ListSt.SetExtendedStyle(m_ListSt.GetExtendedStyle() | LVS_EX_GRIDLINES);			// 罫線描画
		m_ListSt.SetExtendedStyle(m_ListSt.GetExtendedStyle() | LVS_EX_FULLROWSELECT);		// 1行全体選択

																							// 列追加			0		1			
		char name[][32] = { "項目",	"状態" };
		int  wid[] = { 120, 	120 };
		mcnt_pListSt->SetColumnAll(sizeof(wid) / sizeof(int), 32, wid, (char const*)name);
		// 行追加
		mcnt_pListSt->SetRowAll(LIST_ST_ROWMAX);
		// 固定値セット
		mcnt_pListSt->SetItem(EM_ROW_STATUS, 0, "ステータス");
		mcnt_pListSt->SetItem(EM_ROW_INTERVAL, 0, "間隔[m]");
		mcnt_pListSt->SetItem(EM_ROW_RECV_QUE, 0, "受信Que(Max)");
		mcnt_pListSt->SetItem(EM_ROW_IMG_QUE, 0, "画像優先Que");
		mcnt_pListSt->SetItem(EM_ROW_IMG_LOW_QUE, 0, "画像一般Que");
	}

	//-----------------------
	// ポインタ情報 リストビューインスタンス生成 & 定義
	if (true) {
		mcnt_pListPtr = new DspListView(&m_ListPtr);
		// 見た目定義
		m_ListPtr.SetExtendedStyle(m_ListPtr.GetExtendedStyle() | LVS_EX_GRIDLINES);			// 罫線描画
		m_ListPtr.SetExtendedStyle(m_ListPtr.GetExtendedStyle() | LVS_EX_FULLROWSELECT);		// 1行全体選択

		char name[][32] = { "項目",	"状態" };
		int  wid[] = { 160, 	80 };
		mcnt_pListPtr->SetColumnAll(sizeof(wid) / sizeof(int), 32, wid, (char const*)name);
		// 行追加
		mcnt_pListPtr->SetRowAll(LIST_PTR_ROWMAX);
		// 固定値セット
		mcnt_pListPtr->SetItem(EM_ROW_REFRESH, 0, "更新中フラグ");
		mcnt_pListPtr->SetItem(EM_ROW_REC_WRITE_BASE, 0, "書込みﾚｺｰﾄﾞ");
		mcnt_pListPtr->SetItem(EM_ROW_REC_WRITE_TOP, 0, "検査装置位置での現在ﾚｺｰﾄﾞ");
		mcnt_pListPtr->SetItem(EM_ROW_REC_WRITE_BOT, 0, "検査装置位置での現在ﾚｺｰﾄﾞ");
		mcnt_pListPtr->SetItem(EM_ROW_KEN_TOP, 0, "カメラ位置");
		mcnt_pListPtr->SetItem(EM_ROW_KEN_BOT, 0, "カメラ位置");
		mcnt_pListPtr->SetItem(EM_ROW_DSP_TOP, 0, "表示基点");
		mcnt_pListPtr->SetItem(EM_ROW_DSP_BOT, 0, "表示基点");
	}

	//-----------------------
	// レコード状態 リストビューインスタンス生成 & 定義
	if (true) {
		mcnt_pListRec = new DspListView(&m_ListRec);
		// 見た目定義
		m_ListRec.SetExtendedStyle(m_ListRec.GetExtendedStyle() | LVS_EX_GRIDLINES);			// 罫線描画
		m_ListRec.SetExtendedStyle(m_ListRec.GetExtendedStyle() | LVS_EX_FULLROWSELECT);		// 1行全体選択

		char name[LIST_REC_COLMAX][64];
		int	wid[LIST_REC_COLMAX];

		memset(name, 0, sizeof(name));
		memset(wid, 0, sizeof(wid));

		strcpy(name[EM_COL_NO], "ﾚｺｰﾄﾞ");	wid[EM_COL_NO] = 30;
		strcpy(name[EM_COL_KANRINO], "管理No");	wid[EM_COL_KANRINO] = 100;
		strcpy(name[EM_COL_DSP_STATUS], "表示状態");	wid[EM_COL_DSP_STATUS] = 60;
		strcpy(name[EM_COL_KIZU_NUM], "疵数");	wid[EM_COL_KIZU_NUM] = 40;
		strcpy(name[EM_COL_KEN_TOP], "カメラ位置(表面) 通板距離(現在表示行)");	wid[EM_COL_KEN_TOP] = 108;
		strcpy(name[EM_COL_KEN_BOT], "カメラ位置(裏面) 通板距離(現在表示行)");	wid[EM_COL_KEN_BOT] = 108;
		strcpy(name[EM_COL_DSP_TOP], "表示基点(表面) 通板距離(現在表示行)");	wid[EM_COL_DSP_TOP] = 108;
		strcpy(name[EM_COL_DSP_BOT], "表示基点(裏面) 通板距離(現在表示行)");	wid[EM_COL_DSP_BOT] = 108;
		strcpy(name[EM_COL_ROW_WRITE_TOP], "現在書込行");	wid[EM_COL_ROW_WRITE_TOP] = 76;
		strcpy(name[EM_COL_ROW_WRITE_BOT], "現在書込行");	wid[EM_COL_ROW_WRITE_BOT] = 76;
		strcpy(name[EM_COL_ROW_LAST], "検査完了行");	wid[EM_COL_ROW_LAST] = 80;
// 2024.07.26 Kouno 2CGL検査装置φω精度向上機能導入(長周期) ---------------->>>>>
		strcpy(name[EM_COL_LONGPD_FLAG], "長周期");	wid[EM_COL_LONGPD_FLAG] = 60;
// 2024.07.26 Kouno 2CGL検査装置φω精度向上機能導入(長周期) <<<<<----------------
		mcnt_pListRec->SetColumnAll(sizeof(wid) / sizeof(int), 64, wid, (char const*)name);

		// 行追加
		mcnt_pListRec->SetRowAll(TBL_HY_REC_MAX);
		for (int ii = 0; ii<TBL_HY_REC_MAX; ii++) mcnt_pListRec->SetItem(ii, EM_COL_NO, ii + 1);
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
void CHY_DEFECTDlg::ExitProcess()
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

	if (mcnt_pListSt) delete(mcnt_pListSt);
	if (mcnt_pListPtr) delete(mcnt_pListPtr);
	if (mcnt_pListRec) delete(mcnt_pListRec);

	// 画面の終了要求
	PostMessage(WM_QUIT);
}

//------------------------------------------
// メインスレッド DlgThread
// LPVOID param ダイヤログポインタ
//------------------------------------------
unsigned WINAPI CHY_DEFECTDlg::MainThread(LPVOID param)
{
	CHY_DEFECTDlg* p = (CHY_DEFECTDlg *)param;
	COMMON_QUE que;			// 受信構造体

							//// 各プロセス初期化処理 (これ以前はログ出力禁止)
	p->InitProcess();

	//// 起動要因取り込み ループ開始
	while (true) {
		recv_mail(&que);			// 起動要因取り込み

		switch (que.nEventNo) {

			//-----------------------------------------------------------------------------------------------
		case FACT_HY_DEFECT_01:		// パラメータ変更通知
			pLOG(em_MSG), "[Mail] パラメータ変更通知 (FACT_HY_DEFECT_01)");
			// メインインスタンスへ通知
			p->mcls_MI.SetDeliveryMail(E_DEF_ML_PARAM, &que);
			break;

			//-----------------------------------------------------------------------------------------------
		case FACT_HY_DEFECT_02:		// イニシャル通知
			pLOG(em_MSG), "[Mail] イニシャル通知 (FACT_HY_DEFECT_02)");
			// メインインスタンスへ通知
			p->mcls_MI.SetDeliveryMail(E_DEF_ML_INIT, &que);
			break;

			//-----------------------------------------------------------------------------------------------
		case FACT_HY_DEFECT_03:		// 前工程疵情報読込通知 (←TO_DEFECT)
			pLOG(em_MSG), "[Mail] 前工程疵情報読込通知 (FACT_HY_DEFECT_03) [%s]", que.fc.data);
			// メインインスタンスへ通知
			p->mcls_MI.SetDeliveryMail(E_DEF_ML_MAE_READ, &que);
			break;

// 2023.11.17 最終工程検査装置高機能化その４ -------------------->>>>>
		case FACT_HY_DEFECT_04:		// 前工程疵情報読込完了通知 (←HY_DEFECT)
			pLOG(em_MSG), "[Mail] 前工程疵情報読込完了通知 (FACT_HY_DEFECT_04) [Rec=%d]", que.fl.data[0]);
			// メインインスタンスへ通知
			p->mcls_MI.SetDeliveryMail(E_DEF_ML_MAE_READ_END, &que);
			break;
// 2023.11.17 最終工程検査装置高機能化その４ <<<<<--------------------

// 2024.07.26 Kouno 2CGL検査装置φω精度向上機能導入(長周期) ---------------->>>>>
		case FACT_HY_DEFECT_05:		// 長周期欠陥発生通知 (←TO_DEFECT)
			pLOG(em_MSG), "[Mail] 長周期欠陥発生通知 (FACT_HY_DEFECT_05)");
			// メインインスタンスへ通知
			p->mcls_MI.SetDeliveryMail(E_DEF_ML_LONGPD_OCCUR, &que);
			break;
// 2024.07.26 Kouno 2CGL検査装置φω精度向上機能導入(長周期) <<<<<----------------

			//-----------------------------------------------------------------------------------------------
		case FACT_KS_PCNAME_CHANGE:								// PC名称変更通知 (←KS_SLAVE)
			break;

		case FACT_KS_END:									// 終了要求
															// 各プロセス終了処理
			p->ExitProcess();
			return 0;

		default:
			pLOG(em_MSG), "[Mail] イベントコード不明[EventNo=%d]", que.nEventNo);
			syslog(SYSNO_MAILNO_ERR, "[EventNo=%d]", que.nEventNo);
			break;
		}
	}

	return 0;
}

//------------------------------------------
// 表示状態
//------------------------------------------
CString CHY_DEFECTDlg::GetName_DspStatus(DIV_DSP_STATUS em)
{
	CString s;
	if (DIV_DSP_NG == em)				s = "表示不可";
	else if (DIV_DSP_INITIALIZE == em)	s = "－";
	else if (DIV_DSP_STANDBY == em)		s = "表示準備";
	else if (DIV_DSP_OK == em)			s = "表示可";
	else								s.Format("ﾃﾞｰﾀ異常=%d", em);
	return s;
}


//------------------------------------------
// 配列単位のフォーマット
//------------------------------------------
CString CHY_DEFECTDlg::GetArrayFormat(int const* p, char* sprit, int nCnt)
{
	CString cWk;
	cWk.Format("%d", p[0]);
	for (int ii = 1; ii<nCnt; ii++) {
		cWk.AppendFormat("%s%d", sprit, p[ii]);
	}
	return cWk;
}


//￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣
// 画面更新タイマ
//＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿
void CHY_DEFECTDlg::OnTimer(UINT_PTR nIDEvent)
{
	CString ss;
	CString sWk;

	//======================================================
	// ステータス情報
	mcnt_pListSt->SetItem(EM_ROW_STATUS, 1, mcls_MI.gcls_pRecv->GetSockStateName());
	mcnt_pListSt->SetItem(EM_ROW_INTERVAL, 1, GetArrayFormat(mcls_MI.gcls_pParam->GetCommonMast().nMapDsp, "/", MAP_STAGE_NUM));
	ss.Format("%d(%d)", mcls_MI.gque_OpRecv.GetCount(), mcls_MI.gque_OpRecv.GetMaxCount());
	mcnt_pListSt->SetItem(EM_ROW_RECV_QUE, 1, ss);
	ss.Format("%d(%d)", mcls_MI.gque_ImgSave.GetCount(), mcls_MI.gque_ImgSave.GetMaxCount());
	mcnt_pListSt->SetItem(EM_ROW_IMG_QUE, 1, ss);
	ss.Format("%d(%d)", mcls_MI.gque_ImgSaveLow.GetCount(), mcls_MI.gque_ImgSaveLow.GetMaxCount());
	mcnt_pListSt->SetItem(EM_ROW_IMG_LOW_QUE, 1, ss);


	//======================================================
	// ポインタ情報
	mcnt_pListPtr->SetItem(EM_ROW_REFRESH, 1, TRUE == mcls_MI.gcls_pData->GetPointTbl()->nRefresh ? "×" : "");
	mcnt_pListPtr->SetItem(EM_ROW_REC_WRITE_BASE, 1, mcls_MI.gcls_pData->GetPointTbl()->nRecWriteBase);
	mcnt_pListPtr->SetItem(EM_ROW_REC_WRITE_TOP, 1, GetArrayFormat((int const*)mcls_MI.gcls_pData->GetPointTbl()->nRecWrite, "/", NUM_MEN));
	mcnt_pListPtr->SetItem(EM_ROW_REC_WRITE_BOT, 1, GetArrayFormat((int const*)mcls_MI.gcls_pData->GetPointTbl()->nRecWrite, "/", NUM_MEN));
	mcnt_pListPtr->SetItem(EM_ROW_KEN_TOP, 1, mcls_MI.gcls_pData->GetPointTbl()->nRecDisp[DIV_LEN_POS_KEN_TOP]);
	mcnt_pListPtr->SetItem(EM_ROW_KEN_BOT, 1, mcls_MI.gcls_pData->GetPointTbl()->nRecDisp[DIV_LEN_POS_KEN_BOT]);
	mcnt_pListPtr->SetItem(EM_ROW_DSP_TOP, 1, mcls_MI.gcls_pData->GetPointTbl()->nRecDisp[DIV_LEN_POS_DSP_TOP]);
	mcnt_pListPtr->SetItem(EM_ROW_DSP_BOT, 1, mcls_MI.gcls_pData->GetPointTbl()->nRecDisp[DIV_LEN_POS_DSP_BOT]);

	//======================================================
	// レコード情報
	for (int ii = 0; ii<TBL_HY_REC_MAX; ii++) {

		POINT_TBL pPtr = mcls_MI.gcls_pData->GetPointTbl()->pnt[ii];
		mcnt_pListRec->SetItem(ii, EM_COL_KANRINO, pPtr.cKizukenNo);
		mcnt_pListRec->SetItem(ii, EM_COL_DSP_STATUS, GetName_DspStatus(pPtr.emDispStatus));

		mcnt_pListRec->SetItem(ii, EM_COL_KIZU_NUM, mcls_MI.gcls_pData->GetDefectInfTbl()->rec[ii].num);
		ss.Format("%dm (%s)", pPtr.typ.nLenDisp[DIV_LEN_POS_KEN_TOP], GetArrayFormat((int const*)pPtr.typ.nRowDisp[DIV_LEN_POS_KEN_TOP], "/", MAP_STAGE_NUM));
		mcnt_pListRec->SetItem(ii, EM_COL_KEN_TOP, ss);
		ss.Format("%dm (%s)", pPtr.typ.nLenDisp[DIV_LEN_POS_KEN_BOT], GetArrayFormat((int const*)pPtr.typ.nRowDisp[DIV_LEN_POS_KEN_BOT], "/", MAP_STAGE_NUM));
		mcnt_pListRec->SetItem(ii, EM_COL_KEN_BOT, ss);
		ss.Format("%dm (%s)", pPtr.typ.nLenDisp[DIV_LEN_POS_DSP_TOP], GetArrayFormat((int const*)pPtr.typ.nRowDisp[DIV_LEN_POS_DSP_TOP], "/", MAP_STAGE_NUM));
		mcnt_pListRec->SetItem(ii, EM_COL_DSP_TOP, ss);
		ss.Format("%dm (%s)", pPtr.typ.nLenDisp[DIV_LEN_POS_DSP_BOT], GetArrayFormat((int const*)pPtr.typ.nRowDisp[DIV_LEN_POS_DSP_BOT], "/", MAP_STAGE_NUM));
		mcnt_pListRec->SetItem(ii, EM_COL_DSP_BOT, ss);

		mcnt_pListRec->SetItem(ii, EM_COL_ROW_WRITE_TOP, GetArrayFormat((int const*)pPtr.typ.nRowWrite[0], "/", MAP_STAGE_NUM));
		mcnt_pListRec->SetItem(ii, EM_COL_ROW_WRITE_BOT, GetArrayFormat((int const*)pPtr.typ.nRowWrite[1], "/", MAP_STAGE_NUM));
		mcnt_pListRec->SetItem(ii, EM_COL_ROW_LAST, GetArrayFormat((int const*)pPtr.typ.nRowLast, "/", MAP_STAGE_NUM));
		// 2024.07.26 Kouno 2CGL検査装置φω精度向上機能導入(長周期) ---------------->>>>>
		mcnt_pListRec->SetItem(ii, EM_COL_LONGPD_FLAG, ( 0 == mcls_MI.gcls_pData->GetCoilLongPdTbl()->rec[ii].nLongPdOccurrenceFlag ? "なし" : "あり"));
		// 2024.07.26 Kouno 2CGL検査装置φω精度向上機能導入(長周期) <<<<<----------------
	}

	CDialog::OnTimer(nIDEvent);
}

//￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣
// パラメータ変更通知ボタン
//＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿
void CHY_DEFECTDlg::OnBnClickedDebugParam()
{
	if (BST_CHECKED != CWnd::IsDlgButtonChecked(IDC_DEBUG_MANUAL)) return;
	LOG(em_WAR), "[DEBUG] パラメータ変更通知ボタン" );

	COMMON_QUE que;
	que.nEventNo = FACT_HY_DEFECT_01;
	que.nLineNo = getlineid();
	que.fl.data[0] = CH_PARAM_ALL;
	send_mail(getmytaskname(), ".", &que);
}
//￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣
// バイナリ出力ボタン
//＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿
void CHY_DEFECTDlg::OnBnClickedMemOutput()
{
	if (BST_CHECKED != CWnd::IsDlgButtonChecked(IDC_DEBUG_MANUAL)) return;
	LOG(em_WAR), "[DEBUG] バイナリ出力ボタン" );

	// 共有メモリバイナリ出力
	mcls_MI.gcls_pData->OnOutputTbl();
}
//￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣
// バイナリ読込ボタン
//＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿
void CHY_DEFECTDlg::OnBnClickedMemInput()
{
	if (BST_CHECKED != CWnd::IsDlgButtonChecked(IDC_DEBUG_MANUAL)) return;
	LOG(em_WAR), "[DEBUG] バイナリ読込ボタン" );

	// 共有メモリバイナリ読込
	mcls_MI.gcls_pData->OnInputTbl();
}

// 2024.07.26 Kouno 2CGL検査装置φω精度向上機能導入(長周期) ---------------->>>>>
//￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣
// 長周期欠陥発生通知通知ボタン
//＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿
void CHY_DEFECTDlg::OnBnClickedDebugLongpd()
{
	if (BST_CHECKED != CWnd::IsDlgButtonChecked(IDC_DEBUG_MANUAL)) return;
	LOG(em_WAR), "[DEBUG] 長周期欠陥発生通知ボタン" );

	COMMON_QUE que;
	memset(&que, 0x00, sizeof(que));

	que.nEventNo = FACT_HY_DEFECT_05;
	que.nLineNo = getlineid();
	
	char cManagementNo[SIZE_KIZUKEN_NO];
	memset(cManagementNo, 0x00, sizeof(cManagementNo));
	GetDlgItemText(IDC_MANAGEMENTNO, cManagementNo, SIZE_KIZUKEN_NO);

	memcpy(que.mix.cdata[0], cManagementNo, SIZE_KIZUKEN_NO );

	send_mail(getmytaskname(), ".", &que);
}
// 2024.07.26 Kouno 2CGL検査装置φω精度向上機能導入(長周期) <<<<<----------------
