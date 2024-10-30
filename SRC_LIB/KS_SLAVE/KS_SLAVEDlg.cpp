// KS_SLAVEDlg.cpp : 実装ファイル
//

#include "stdafx.h"
#include "KS_SLAVE.h"
#include "KS_SLAVEDlg.h"

#include "AllUserLogOff.h"		// リモート強制ログオフ

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CKS_SLAVEDlg ダイアログ




CKS_SLAVEDlg::CKS_SLAVEDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CKS_SLAVEDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CKS_SLAVEDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, m_ListBox);
}

BEGIN_MESSAGE_MAP(CKS_SLAVEDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_LBN_DBLCLK(IDC_LIST1, &CKS_SLAVEDlg::OnLbnDblclkList1)
	ON_BN_CLICKED(IDOK, &CKS_SLAVEDlg::OnBnClickedOk)
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDC_MYDOWN2, &CKS_SLAVEDlg::OnBnClickedMydown2)
	ON_BN_CLICKED(IDC_MYREBOOT, &CKS_SLAVEDlg::OnBnClickedMyreboot)
	ON_BN_CLICKED(IDC_MYLOGOFF, &CKS_SLAVEDlg::OnBnClickedMylogoff)
	ON_BN_CLICKED(IDC_MYTASKEND, &CKS_SLAVEDlg::OnBnClickedMytaskend)
END_MESSAGE_MAP()


// CKS_SLAVEDlg メッセージ ハンドラ

BOOL CKS_SLAVEDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// このダイアログのアイコンを設定します。アプリケーションのメイン ウィンドウがダイアログでない場合、
	//  Framework は、この設定を自動的に行います。
	SetIcon(m_hIcon, TRUE);			// 大きいアイコンの設定
	SetIcon(m_hIcon, FALSE);		// 小さいアイコンの設定

	// TODO: 初期化をここに追加します。
//---------------------
	// 独自
	m_evManualEnd	= CreateEvent(NULL, FALSE, FALSE, NULL);

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
// <<<-------------------------------

	return TRUE;  // フォーカスをコントロールに設定した場合を除き、TRUE を返します。
}

// ダイアログに最小化ボタンを追加する場合、アイコンを描画するための
//  下のコードが必要です。ドキュメント/ビュー モデルを使う MFC アプリケーションの場合、
//  これは、Framework によって自動的に設定されます。

void CKS_SLAVEDlg::OnPaint()
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
HCURSOR CKS_SLAVEDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

// これ以降 手動追加 -------------------->>>

//￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣
// ×ボタン
//＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿
void CKS_SLAVEDlg::OnClose()
{
	// TODO: ここにメッセージ ハンドラ コードを追加するか、既定の処理を呼び出します。
	//CDialog::OnClose();
}
//￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣
// 閉じるボタン
//＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿
void CKS_SLAVEDlg::OnBnClickedOk()
{
	if( IDYES == MessageBox("終了しますか？", "重要なお知らせ", MB_YESNO | MB_DEFBUTTON2 | MB_ICONQUESTION) ) {
		LOG(em_MSG), "閉じるボタン");
		COMMON_QUE que;
		que.nEventNo = FACT_KS_END;
		que.nLineNo = getlineid();
		send_mail(getmytaskname(), ".", &que);	
		// 起動待ちのときの対策
		SetEvent(m_evManualEnd);
	}
}
//￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣
// メモ帳起動
//＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿
void CKS_SLAVEDlg::OnLbnDblclkList1()
{
	// ListBoxコントロールのプロパティは、ソート禁止にしておく事
	// ListBoxコントロールのダブルクリックイベントを追加
	mcls_pLog->ShowEdit();
}

//------------------------------------------
// 各プロセス初期化処理
//------------------------------------------
void CKS_SLAVEDlg::InitProcess()
{
	//// 一番初めにいろいろ初期化
	mcls_pLog = NULL;

	// PCが立ち上げ後安定するまでちょっと待ち
	int nWaitTime = GetPrivateProfileInt(KS_SLAVE, "START_TIMEWAIT", 5, PCINI_NAME);
	nWaitTime *= 1000;	// [s] → [ms]
	//Sleep(nWaitTime);	
	if( WAIT_TIMEOUT != WaitForSingleObject(m_evManualEnd, nWaitTime))	{
		// タイムアウトじゃない。途中中断
		exit(-9);		// 自プロセス強制終了
		return;
	}

	// タスク初期化処理
	setlineid(LINE_ID);
	int nRetc = task_init(KS_SLAVE);
	if( 0 != nRetc ) {
		if(-1 == nRetc) { syslog(SYSNO_TASK_INIT_ERR, "[%s] 既にタスク起動済みにより、起動中断。", KS_SLAVE); }
		else			{ syslog(SYSNO_TASK_INIT_ERR, "[%s]", KS_SLAVE); }
		_ASSERT(0);
		exit(-9);		// 自プロセス強制終了
		return;
	}
	m_nEndStatus = ProcessManager::P_NOTHING;

	// LogFileManagerインスタンス生成 (これ以降 ログ出力可能)
	setlocale(LC_ALL, "Japanese");	// これが無いと日本語パスを含むフォルダの場合、保存できない。(ライブラリの中で実行するべきか迷っているが、変更するとタスク全体に影響するので、とりあえずここで設定)
	mcls_pLog = new LogFileManager(getmytaskname(), &m_ListBox);
	LOG(em_MSG), "タスク起動");
	LOG(em_MSG), "各プロセス開始処理 実行中・・・");

	// メインインスタンス 開始処理
	mcls_MI.SetLogMgr(mcls_pLog);
	mcls_MI.Init();
	
	//// 画面更新用タイマ生成
	//SetTimer(1, 500, NULL);

	// メインスレッドの実行を行う
	LOG(em_MSG), "各プロセス開始処理 完了");
	mcls_MI.Start();

	// PCマスター管理確認有無
	if(mcls_MI.GetDebug()) {
		// 統括確認
		LOG(em_INF), "デバック実行" );
		COMMON_QUE que;
		memset(&que, 0x00, sizeof(que));
		que.nEventNo = FACT_KS_SLAVE_03;
		que.nLineNo = getlineid();															// ラインID
		int retc = send_mail( getmytaskname(), ".", &que );
	}
	return;
}

//￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣
// 各プロセス開放処理
//＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿
void CKS_SLAVEDlg::ExitProcess()
{
	//// ログ リストボックス表示処理終了
	mcls_pLog->SetListOut(false);					// これ以降 リストボックスへのログ表示禁止

	// メインインスタンス 終了処理
	//KillTimer(1);			// 画面更新タイマ
	mcls_MI.Stop();
	mcls_MI.Exit();

	//// ログ リストボックス表示処理終了
	LOG(em_MSG), "タスク終了");
	//if(NULL != mcls_pLog){
	//	delete mcls_pLog;
	//	mcls_pLog = NULL;
	//}
	mcls_pLog->SetListOut(false);					// これ以降 リストボックスへのログ表示禁止


	// タスク終了処理
	task_exit();
	CloseHandle( m_evManualEnd );

	// PC終了区分
	Sleep(2000);			// 常駐タスクが全部終了するまで待ち
	if( ProcessManager::P_NOTHING != m_nEndStatus) {				// シャットダウン // 再起動
		// リモートセッション終了
		AllUserLogOff	clsLogoff;
		clsLogoff.SetLogMgr(mcls_pLog);
		clsLogoff.Exec();


		// PC終了
		int retc = ProcessManager::ExitWin(m_nEndStatus);
		if( -1 == retc) {			// シャットダウン失敗
			syslog(SYSNO_SHUTDOWN_ERR, "");
		} else if(-2 == retc) {		// 再起動失敗
			syslog(SYSNO_RESTAET_ERR, "");
		} else if(-3 == retc) {		// ログオフ失敗
			syslog(SYSNO_LOGOFF_ERR, "");
		}
	}

	//// ログクラス開放
	if(NULL != mcls_pLog){
		delete mcls_pLog;
		mcls_pLog = NULL;
	}


	// 画面の終了要求
	PostMessage(WM_QUIT);
}

//------------------------------------------
// メインスレッド DlgThread
// LPVOID param ダイヤログポインタ
//------------------------------------------
unsigned WINAPI CKS_SLAVEDlg::MainThread(void* param)
{
	CKS_SLAVEDlg* p = (CKS_SLAVEDlg *)param;
	COMMON_QUE que;			// 受信構造体
	COMMON_QUE sque;		// 送信構造体

	//// 各プロセス初期化処理 (これ以前はログ出力禁止)
	p->InitProcess();

	//// 起動要因取り込み ループ開始
	while(true) {
		recv_mail(&que);			// 起動要因取り込み

		switch(que.nEventNo) {

//-----------------------------------------------------------------------------------------------
		case FACT_KS_SLAVE_01 :								// 時刻変更要求 (←KS_MASTER)
			pLOG(em_MSG), "[MAIL] 時刻変更要求 (%04d/%02d/%02d %02d:%02d:%02d)", 
				que.timeset_que.systemtime.wYear, que.timeset_que.systemtime.wMonth, que.timeset_que.systemtime.wDay,
				que.timeset_que.systemtime.wHour, que.timeset_que.systemtime.wMinute, que.timeset_que.systemtime.wSecond );
			SetLocalTime(&que.timeset_que.systemtime);				// 時刻設定
			break;

//-----------------------------------------------------------------------------------------------
		case FACT_KS_SLAVE_02 :								// ハートビート (←KS_MASTER)
			p->mcls_MI.gcls_pCSta->Set_HeatBeatID( que.fl.data[0], true ); 
			break;

//-----------------------------------------------------------------------------------------------
		case FACT_KS_SLAVE_03 :								// PCマスター管理立ち上げ完了通知 (←KS_MASTER)
			pLOG(em_MSG), "[MAIL] PCマスター管理立ち上げ完了通知");

			// メインインスタンスへ通知
			PostThreadMessage(p->mcls_MI.GetTid(), E_DEF_ML_MASTEROK, 0, 0);
			break;

//-----------------------------------------------------------------------------------------------
		case FACT_KS_SLAVE_04 :								// シャットダウン要求 (←KS_MASTER)
			pLOG(em_MSG), "[MAIL] シャットダウン要求");
			syslog(SYSNO_SHUTDOWN_OK, "");

			p->m_nEndStatus = ProcessManager::P_POWEROFF;
			sque.nEventNo = FACT_KS_END;
			que.nLineNo = getlineid();															// ラインID
			send_mail(getmytaskname(), ".", &sque);
			break;

//-----------------------------------------------------------------------------------------------
		case FACT_KS_SLAVE_05 :								// PC再起動要求 (←KS_MASTER)
			pLOG(em_MSG), "[MAIL] PC再起動要求 [対象区分:%d]", que.fl.data[0] );

			// 蓄積PC以外の再起動要求時は、自分が蓄積PCの時は再起動やめ
			if( 1 == que.fl.data[0] && 0 == strcmp(getmypcname(), TIKUSEKI) ) {
				pLOG(em_MSG), "蓄積機能に対し再起動が来たので、再起動要求取りやめ");
				break;
			}

			syslog(SYSNO_RESTAET_OK, "");
			// ここまで来たら、再起動要求を受け付けたら必ず、再起動するよ。
			p->m_nEndStatus = ProcessManager::P_REBOOT;
			sque.nEventNo = FACT_KS_END;
			que.nLineNo = getlineid();															// ラインID
			send_mail(getmytaskname(), ".", &sque);	
			break;

//-----------------------------------------------------------------------------------------------
		case FACT_KS_SLAVE_06 :								// PCログオフ要求 (←KS_MASTER)
			pLOG(em_MSG), "[MAIL] PCログオフ要求 [対象区分:%d]", que.fl.data[0] );

			// 蓄積PC以外の再起動要求時は、自分が蓄積PCの時は再起動やめ
			if( 1 == que.fl.data[0] && 0 == strcmp(getmypcname(), TIKUSEKI) ) {
				pLOG(em_MSG), "蓄積機能に対しログオフが来たので、ログオフ要求取りやめ");
				break;
			}

			syslog(SYSNO_LOGOFF_OK, "");
			// ここまで来たら、再起動要求を受け付けたら必ず、再起動するよ。
			p->m_nEndStatus = ProcessManager::P_LOGOFF;
			sque.nEventNo = FACT_KS_END;
			que.nLineNo = getlineid();															// ラインID
			send_mail(getmytaskname(), ".", &sque);	
			break;

//-----------------------------------------------------------------------------------------------
		case FACT_KS_SLAVE_07 :								// PC常駐タスク終了要求 (←KS_MASTER)
			pLOG(em_MSG), "[MAIL] PC常駐タスク終了要求 [対象区分:%d]", que.fl.data[0] );

			// 蓄積PC以外の再起動要求時は、自分が蓄積PCの時は再起動やめ
			if( 1 == que.fl.data[0] && 0 == strcmp(getmypcname(), TIKUSEKI) ) {
				pLOG(em_MSG), "蓄積機能に対し通知が来たので、常駐タスク終了要求取りやめ");
				break;
			}

			syslog(SYSNO_SHUTDOWN_OK, "(常駐タスクのみ終了)");
			// ここまで来たら、再起動要求を受け付けたら必ず、再起動するよ。
			p->m_nEndStatus = ProcessManager::P_NOTHING;
			sque.nEventNo = FACT_KS_END;
			que.nLineNo = getlineid();															// ラインID
			send_mail(getmytaskname(), ".", &sque);	
			break;

//-----------------------------------------------------------------------------------------------
		case FACT_KS_PCNAME_CHANGE:															// PC名称変更通知起動要因
			//// 全PC名称をDBより取得
			if( getpcname() ) {
				pLOG(em_MSG), "[MAIL] 全PC名称をDBより取得");
			} else {
				pLOG(em_ERR), "[MAIL] 全PC名称をDBより取得失敗");
			}
			//// 常駐プロセス全員に通知
			sque.nEventNo = FACT_KS_PCNAME_CHANGE;
			sque.nLineNo = getlineid();
			ProcessManager::AllProcessSend(&sque);
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
// デバックコマンド
//＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿
void CKS_SLAVEDlg::OnBnClickedMydown2()
{
	if( IDYES == MessageBox("自PCをシャットダウンしますか？", "重要なお知らせ", MB_YESNO | MB_DEFBUTTON2 | MB_ICONQUESTION) ) {
		LOG(em_MSG), "自PCシャットダウンボタン");
		COMMON_QUE que;
		que.nEventNo = FACT_KS_SLAVE_04;
		que.nLineNo = getlineid();															// ラインID
		send_mail(getmytaskname(), ".", &que);	
	}
}
void CKS_SLAVEDlg::OnBnClickedMyreboot()
{
	if( IDYES == MessageBox("自PCを再起動しますか？", "重要なお知らせ", MB_YESNO | MB_DEFBUTTON2 | MB_ICONQUESTION) ) {
		LOG(em_MSG), "自PC再起動ボタン");
		COMMON_QUE que;
		que.nEventNo = FACT_KS_SLAVE_05;
		que.nLineNo = getlineid();															// ラインID
		que.fl.data[0] = 0;
		send_mail(getmytaskname(), ".", &que);	
	}
}
void CKS_SLAVEDlg::OnBnClickedMylogoff()
{
	if( IDYES == MessageBox("自PCをログオフしますか？", "重要なお知らせ", MB_YESNO | MB_DEFBUTTON2 | MB_ICONQUESTION) ) {
		LOG(em_MSG), "自PCログオフボタン");
		COMMON_QUE que;
		que.nEventNo = FACT_KS_SLAVE_06;
		que.nLineNo = getlineid();															// ラインID
		que.fl.data[0] = 0;
		send_mail(getmytaskname(), ".", &que);	
	}
}

void CKS_SLAVEDlg::OnBnClickedMytaskend()
{
	if( IDYES == MessageBox("自PCの常駐タスクを終了しますか？", "重要なお知らせ", MB_YESNO | MB_DEFBUTTON2 | MB_ICONQUESTION) ) {
		LOG(em_MSG), "自PC常駐タスク終了ボタン");
		COMMON_QUE que;
		que.nEventNo = FACT_KS_SLAVE_07;
		que.nLineNo = getlineid();															// ラインID
		que.fl.data[0] = 0;
		send_mail(getmytaskname(), ".", &que);	
	}
}
