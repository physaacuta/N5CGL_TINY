// KS_MASTERDlg.cpp : 実装ファイル
//

#include "stdafx.h"
#include "KS_MASTER.h"
#include "KS_MASTERDlg.h"

#include "AllUserLogOff.h"		// リモート強制ログオフ

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CKS_MASTERDlg ダイアログ




CKS_MASTERDlg::CKS_MASTERDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CKS_MASTERDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CKS_MASTERDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, m_ListBox);
}

BEGIN_MESSAGE_MAP(CKS_MASTERDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDOK, &CKS_MASTERDlg::OnBnClickedOk)
	ON_WM_CLOSE()
//	ON_WM_TIMER()
	ON_LBN_DBLCLK(IDC_LIST1, &CKS_MASTERDlg::OnLbnDblclkList1)
	ON_BN_CLICKED(IDC_ALLDOWN, &CKS_MASTERDlg::OnBnClickedAlldown)
	ON_BN_CLICKED(IDC_ALLREBOOT, &CKS_MASTERDlg::OnBnClickedAllreboot)
//	ON_BN_CLICKED(IDC_MYDOWN, &CKS_MASTERDlg::OnBnClickedMydown)
	ON_BN_CLICKED(IDC_MYREBOOT, &CKS_MASTERDlg::OnBnClickedMyreboot)
	ON_BN_CLICKED(IDC_ALLLOGOFF, &CKS_MASTERDlg::OnBnClickedAlllogoff)
	ON_BN_CLICKED(IDC_MYLOGOFF, &CKS_MASTERDlg::OnBnClickedMylogoff)
	ON_BN_CLICKED(IDC_ALLTASKEND, &CKS_MASTERDlg::OnBnClickedAlltaskend)
END_MESSAGE_MAP()


// CKS_MASTERDlg メッセージ ハンドラ

BOOL CKS_MASTERDlg::OnInitDialog()
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

void CKS_MASTERDlg::OnPaint()
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
HCURSOR CKS_MASTERDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

// これ以降 手動追加 -------------------->>>

//￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣
// ×ボタン
//＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿
void CKS_MASTERDlg::OnClose()
{
	//CDialog::OnClose();
}
//￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣
// 閉じるボタン
//＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿
void CKS_MASTERDlg::OnBnClickedOk()
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
void CKS_MASTERDlg::OnLbnDblclkList1()
{
	// ListBoxコントロールのプロパティは、ソート禁止にしておく事
	// ListBoxコントロールのダブルクリックイベントを追加
	mcls_pLog->ShowEdit();
}

//------------------------------------------
// 各プロセス初期化処理
//------------------------------------------
void CKS_MASTERDlg::InitProcess()
{
	//// 一番初めにいろいろ初期化
	mcls_pLog = NULL;

	// PCが立ち上げ後安定するまでちょっと待ち
	int nWaitTime = GetPrivateProfileInt(KS_MASTER, "START_TIMEWAIT", 5, PCINI_NAME);
	nWaitTime *= 1000;	// [s] → [ms]
	//Sleep(nWaitTime);	
	if( WAIT_TIMEOUT != WaitForSingleObject(m_evManualEnd, nWaitTime))	{
		// タイムアウトじゃない。途中中断
		exit(-9);		// 自プロセス強制終了
		return;
	}

	// タスク初期化処理
	setlineid(LINE_ID);
	int nRetc = task_init(KS_MASTER);
	if( 0 != nRetc ) {
		if(-1 == nRetc) { syslog(SYSNO_TASK_INIT_ERR, "[%s] 既にタスク起動済みにより、起動中断。", KS_MASTER); }
		else			{ syslog(SYSNO_TASK_INIT_ERR, "[%s]", KS_MASTER); }
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
	return;
}

//￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣
// 各プロセス開放処理
//＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿
void CKS_MASTERDlg::ExitProcess()
{
	//// ログ リストボックス表示処理終了
	mcls_pLog->SetListOut(false);					// これ以降 リストボックスへのログ表示禁止

	// メインインスタンス 終了処理
	//KillTimer(1);			// 画面更新タイマ
	mcls_MI.Stop();
	mcls_MI.Exit();

	//// ログクラス開放
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
unsigned WINAPI CKS_MASTERDlg::MainThread(void* param)
{
	CKS_MASTERDlg* p = (CKS_MASTERDlg *)param;
	COMMON_QUE que;			// 受信構造体
	COMMON_QUE sque;		// 送信構造体
	char sendPC[SIZE_NAME_PC];		// 送信PC名称
	int retc;
	int nCnt;

	//// 各プロセス初期化処理 (これ以前はログ出力禁止)
	p->InitProcess();

	//// 起動要因取り込み ループ開始
	while(true) {
		recv_mail(&que);			// 起動要因取り込み

		switch(que.nEventNo) {

//-----------------------------------------------------------------------------------------------
		case FACT_KS_MASTER_01 :							// 時刻変更要求 (←SO_GAMENN？)
			// 微妙に統括時間がズレる為の対策
			//   探傷開始時に毎回統括の時刻で設定を行う様にした為、ミリ秒が削られての誤差が無視できなくなった。
//			SetLocalTime(&que.timeset_que.systemtime);				// 時刻設定
//			p->mcls_MI.Send_MASTER_TIME( );							// 他PCへ時刻変更要求
			if(true) {
				SYSTEMTIME nowtime;
				GetLocalTime(&nowtime);								// 自PCの現在時間を取得

				// 年月日時分秒まで 現在時刻と一緒なら、自PCの時刻は再設定しない
				if( nowtime.wYear   != que.timeset_que.systemtime.wYear ||
				    nowtime.wMonth  != que.timeset_que.systemtime.wMonth ||
					nowtime.wDay    != que.timeset_que.systemtime.wDay ||
					nowtime.wHour   != que.timeset_que.systemtime.wHour ||
					nowtime.wMinute != que.timeset_que.systemtime.wMinute ||
					nowtime.wSecond != que.timeset_que.systemtime.wSecond ) {					
					SetLocalTime(&que.timeset_que.systemtime);		// 自PCの時刻設定
				}
				p->mcls_MI.Send_MASTER_TIME( );						// 他PCへ時刻変更要求
			}
			pLOG(em_MSG), "[MAIL] 時刻変更要求 (%4d/%2d/%2d %2d:%2d:%2d)", 
				que.timeset_que.systemtime.wYear, que.timeset_que.systemtime.wMonth, que.timeset_que.systemtime.wDay,
				que.timeset_que.systemtime.wHour, que.timeset_que.systemtime.wMinute, que.timeset_que.systemtime.wSecond );
			break;

//-----------------------------------------------------------------------------------------------
		case FACT_KS_MASTER_02 :							// ハートビート (←KS_SLAVE)
			if( 0 == que.fl.data[0]) break;							// 機器ID==0は無視

			p->mcls_MI.gcls_pCSta->Set_HeatBeatID( que.fl.data[0], true ); 
			break;

//-----------------------------------------------------------------------------------------------
		case FACT_KS_MASTER_03 :							// PCマスター管理状態問い合わせ (←KS_SLAVE)
			pLOG(em_MSG), "[MAIL] PCマスター管理状態問い合わせ (PC=%s)", que.fc.data);
			// DB初期接続確立後のみ、返信
			p->mcls_MI.Send_MASTER_OK(que.fc.data);
			break;

//-----------------------------------------------------------------------------------------------
		case FACT_KS_MASTER_04 :							// 立ち上げ完了報告 (←KS_SLAVE, KS_MASTER)
			if( 0 == que.fl.data[0]) break;							// 機器ID==0は無視

			// 該当の機器IDは存在する？
			if( -1 != p->mcls_MI.gcls_pCSta->GetIndex(que.fl.data[0]) ) {
				// 立ち上げ直後では、失敗する場合がある対策。これで失敗すると該当PCが立ち上がっていても、異常となる為
				nCnt = 0;
				while(true) {	
					if( KizuFunction::SetStatus( que.fl.data[0], true) ) break;
					pLOG(em_WAR), "[Dlg] SetStatus 失敗によりリトライ。 立ち上げ完了報告 [%d]", que.fl.data[0]);
					Sleep(50);
					nCnt++;
					if(5 < nCnt) {
						pLOG(em_ERR), "[Dlg] SetStatus 失敗！！ 立ち上げ完了報告 [%d]", que.fl.data[0]);
						break;
					}
				}
				pLOG(em_MSG), "[MAIL] 立ち上げ完了報告 [PC名:%s]", p->mcls_MI.gcls_pCSta->GetName(que.fl.data[0]) );
			} else {
				pLOG(em_ERR), "[MAIL] 立ち上げ完了報告異常 [%d]", que.fl.data[0] );
			}
			// HBも復旧させておく
			if( 0 < que.fl.data[1] ) {
				p->mcls_MI.gcls_pCSta->Set_HeatBeatID( que.fl.data[1], true ); 
				// 立ち上げ直後では、失敗する場合がある対策。
				nCnt = 0;
				while(true) {	
					if( KizuFunction::SetStatus( que.fl.data[1], true) ) break;
					pLOG(em_WAR), "[Dlg] SetStatus 失敗によりリトライ。 HB報告 [%d]", que.fl.data[0]);
					Sleep(50);
					nCnt++;
					if(5 < nCnt) {
						pLOG(em_ERR), "[Dlg] SetStatus 失敗！！ HB報告 [%d]", que.fl.data[0]);
						break;
					}
				}
			}
			break;

//-----------------------------------------------------------------------------------------------
		case FACT_KS_MASTER_05 :							// 立ち下げ完了報告 (←KS_SLAVE)
			if( 0 == que.fl.data[0]) break;							// 機器ID==0は無視

			// 該当の機器IDは存在する？
			if( -1 != p->mcls_MI.gcls_pCSta->GetIndex(que.fl.data[0]) ) {
				KizuFunction::SetStatus( que.fl.data[0], false);
				pLOG(em_MSG), "[MAIL] 立ち下げ完了報告 [%s]", p->mcls_MI.gcls_pCSta->GetName(que.fl.data[0]) );
			} else {
				pLOG(em_ERR), "[MAIL] 立ち下げ完了報告異常 [%d]", que.fl.data[0] );
			}
			// HBも異常にしておく
			if( 0 < que.fl.data[1] ) {
				p->mcls_MI.gcls_pCSta->Set_HeatBeatID( que.fl.data[1], false ); 
				KizuFunction::SetStatus( que.fl.data[1], false);
			}
			break;

//-----------------------------------------------------------------------------------------------
		case FACT_KS_MASTER_06 :							// システム終了要求 (←SO_GAMENN)
			pLOG(em_MSG), "[MAIL] システム終了要求");
			syslog(SYSNO_SHUTDOWN_OK, "");

			// 検査停止通知
			p->SendMail_Stop();

			// 全PCへシステム終了要求
			sque.nEventNo = FACT_KS_SLAVE_04;
			sque.nLineNo = getlineid();															// ラインID
			retc = send_mail( KS_SLAVE, "*", &sque );
			if( 0 != retc ) syslog(SYSNO_MAIL_SNED_ERR, "[%d:システム終了要求 <err=%d>]", FACT_KS_MASTER_06, retc);
			
			// システム終了時は、自分も必ずPCシャットダウン対象
			Sleep(200);				// 他のPCから通知を受け取れるようにちょっと待ち
			p->m_nEndStatus = ProcessManager::P_POWEROFF;
			sque.nEventNo = FACT_KS_END;
			sque.nLineNo = getlineid();															// ラインID
			send_mail(getmytaskname(), ".", &sque);	
			break;

//-----------------------------------------------------------------------------------------------
		case FACT_KS_MASTER_07 :							// PC再起動要求 (←SO_GAMENN)
			// 統括機能、蓄積機能 の場合は、"*"を指定しておく。
			// vbからの文字列は、NULLで無くてスペースなので、スペースをNULLに変換
			//   ↑ .net版からは、大丈夫なようにしたので、OKのはず。とりあえず入れておく。(何でかは、マネージドコード等々を理解している人には分かるはず)
			KizuFunction::SpaceToNull(que.fc.data, sizeof(que.fc.data));
			pLOG(em_MSG), "[MAIL] PC再起動要求 [対象PC:%s]", que.fc.data);
			syslog(SYSNO_RESTAET_OK, "[%s]", que.fc.data);

			//// PC再起動要求
			memset(sendPC, 0x00, sizeof(sendPC));
			sque.nEventNo = FACT_KS_SLAVE_05;
			sque.nLineNo = getlineid();															// ラインID
			// 対象PC決定
			if( 0 == strcmp(que.fc.data, "*" ) || 0 == strcmp(que.fc.data, TIKUSEKI) ) {
				// 蓄積PCが再起動対象の場合は、全システムのPCを再起動
				sque.fl.data[0] = 0;														// メールを受け取った全PCが再起動
				sendPC[0] = '*';
				p->SendMail_Stop();	// 検査停止通知

			} else if(0 == strcmp(que.fc.data, MASTER_PC_NAME)) {
				// 統括PCが再起動対象の場合は、蓄積PCを除くシステムのPCを再起動
				sque.fl.data[0] = 1;														// メールを受け取ったPCの蓄積以外が再起動
				sendPC[0] = '*';
				p->SendMail_Stop();	// 検査停止通知

			} else {
				// 指定PCのみ再起動
				sque.fl.data[0] = 0;														// メールを受け取った全PCが再起動
				memcpy(sendPC, que.fc.data, sizeof(sendPC));
			}
			retc = send_mail( KS_SLAVE, sendPC, &sque );
			if( 0 != retc ) syslog(SYSNO_MAIL_SNED_ERR, "[%d:PC再起動要求 <err=%d>]", FACT_KS_MASTER_07, retc);

/*			
			// 自分のPCが対象？ (通常ありえないはず。だけど今後のために処理追加)
			if( 0 == strcmp(sendPC, "*" ) ||						// 自分も対象
				0 == strcmp(sendPC, ".")  ||						// 自分が対象
				0 == strcmp(sendPC, PCID_NAME(getpcid())) )	{		// 自分のPC名と一致
*/
			// 自分のPCが対象？ (通常ありえないはず。だけど今後のために処理追加)
			if( 0 == strcmp(sendPC, "*" ) ||						// 自分も対象
				0 == strcmp(sendPC, ".")  ) {						// 自分が対象

				Sleep(500);				// 他のPCから通知を受け取れるようにちょっと待ち
				p->m_nEndStatus = ProcessManager::P_REBOOT;
				sque.nEventNo = FACT_KS_END;
				sque.nLineNo = getlineid();															// ラインID
				send_mail(getmytaskname(), ".", &sque);	
			}
			break;

//-----------------------------------------------------------------------------------------------
		case FACT_KS_MASTER_08 :							// PCログオフ要求 (←SO_GAMENN)
			// 統括機能、蓄積機能 の場合は、"*"を指定しておく。
			// vbからの文字列は、NULLで無くてスペースなので、スペースをNULLに変換
			//   ↑ .net版からは、大丈夫なようにしたので、OKのはず。とりあえず入れておく。(何でかは、マネージドコード等々を理解している人には分かるはず)
			KizuFunction::SpaceToNull(que.fc.data, sizeof(que.fc.data));
			pLOG(em_MSG), "[MAIL] PCログオフ要求 [対象PC:%s]", que.fc.data);
			syslog(SYSNO_LOGOFF_OK, "[%s]", que.fc.data);

			//// PC再起動要求
			memset(sendPC, 0x00, sizeof(sendPC));
			sque.nEventNo = FACT_KS_SLAVE_06;
			sque.nLineNo = getlineid();															// ラインID
			// 対象PC決定
			if( 0 == strcmp(que.fc.data, "*" ) || 0 == strcmp(que.fc.data, TIKUSEKI) ) {
				// 蓄積PCが再起動対象の場合は、全システムのPCを再起動
				sque.fl.data[0] = 0;														// メールを受け取った全PCが再起動
				sendPC[0] = '*';
				p->SendMail_Stop();	// 検査停止通知
			} else if(0 == strcmp(que.fc.data, MASTER_PC_NAME)) {
				// 統括PCが再起動対象の場合は、蓄積PCを除くシステムのPCを再起動
				sque.fl.data[0] = 1;														// メールを受け取ったPCの蓄積以外が再起動
				sendPC[0] = '*';
				p->SendMail_Stop();	// 検査停止通知
			} else {
				// 指定PCのみ再起動
				sque.fl.data[0] = 0;														// メールを受け取った全PCが再起動
				memcpy(sendPC, que.fc.data, sizeof(sendPC));
			}
			retc = send_mail( KS_SLAVE, sendPC, &sque );
			if( 0 != retc ) syslog(SYSNO_MAIL_SNED_ERR, "[%d:PCログオフ要求 <err=%d>]", FACT_KS_MASTER_08, retc);

/*
			// 自分のPCが対象？ (通常ありえないはず。だけど今後のために処理追加)
			if( 0 == strcmp(sendPC, "*" ) ||						// 自分も対象
				0 == strcmp(sendPC, ".")  ||						// 自分が対象
				0 == strcmp(sendPC, PCID_NAME(getpcid())) )	{		// 自分のPC名と一致
*/
			// 自分のPCが対象？ (通常ありえないはず。だけど今後のために処理追加)
			if( 0 == strcmp(sendPC, "*" ) ||						// 自分も対象
				0 == strcmp(sendPC, ".")  ) {						// 自分が対象

				Sleep(500);				// 他のPCから通知を受け取れるようにちょっと待ち
				p->m_nEndStatus = ProcessManager::P_LOGOFF;
				sque.nEventNo = FACT_KS_END;
				sque.nLineNo = getlineid();															// ラインID
				send_mail(getmytaskname(), ".", &sque);	
			}
			break;

//-----------------------------------------------------------------------------------------------
		case FACT_KS_MASTER_09 :							// PC常駐タスク終了要求 (←SO_GAMENN)
			// 統括機能、蓄積機能 の場合は、"*"を指定しておく。
			// vbからの文字列は、NULLで無くてスペースなので、スペースをNULLに変換
			//   ↑ .net版からは、大丈夫なようにしたので、OKのはず。とりあえず入れておく。(何でかは、マネージドコード等々を理解している人には分かるはず)
			KizuFunction::SpaceToNull(que.fc.data, sizeof(que.fc.data));
			pLOG(em_MSG), "[MAIL] PC常駐タスク終了要求 [対象PC:%s]", que.fc.data);
			syslog(SYSNO_SHUTDOWN_OK, "[%s] (常駐タスクのみ終了)", que.fc.data);

			//// PC再起動要求
			memset(sendPC, 0x00, sizeof(sendPC));
			sque.nEventNo = FACT_KS_SLAVE_07;
			sque.nLineNo = getlineid();															// ラインID
			// 対象PC決定
			if( 0 == strcmp(que.fc.data, "*" ) || 0 == strcmp(que.fc.data, TIKUSEKI) ) {
				// 蓄積PCが再起動対象の場合は、全システムのPCを再起動
				sque.fl.data[0] = 0;														// メールを受け取った全PCが再起動
				sendPC[0] = '*';

				p->SendMail_Stop();	// 検査停止通知
			} else if(0 == strcmp(que.fc.data, MASTER_PC_NAME)) {
				// 統括PCが再起動対象の場合は、蓄積PCを除くシステムのPCを再起動
				sque.fl.data[0] = 1;														// メールを受け取ったPCの蓄積以外が再起動
				sendPC[0] = '*';

				p->SendMail_Stop();	// 検査停止通知
			} else {
				// 指定PCのみ再起動
				sque.fl.data[0] = 0;														// メールを受け取った全PCが再起動
				memcpy(sendPC, que.fc.data, sizeof(sendPC));
			}
			retc = send_mail( KS_SLAVE, sendPC, &sque );
			if( 0 != retc ) syslog(SYSNO_MAIL_SNED_ERR, "[%d:PC常駐タスク終了要求 <err=%d>]", FACT_KS_MASTER_09, retc);

/*
			// 自分のPCが対象？ (通常ありえないはず。だけど今後のために処理追加)
			if( 0 == strcmp(sendPC, "*" ) ||						// 自分も対象
				0 == strcmp(sendPC, ".")  ||						// 自分が対象
				0 == strcmp(sendPC, PCID_NAME(getpcid())) )	{		// 自分のPC名と一致
*/
			// 自分のPCが対象？ (通常ありえないはず。だけど今後のために処理追加)
			if( 0 == strcmp(sendPC, "*" ) ||						// 自分も対象
				0 == strcmp(sendPC, ".")  ) {						// 自分が対象

				Sleep(500);				// 他のPCから通知を受け取れるようにちょっと待ち
				p->m_nEndStatus = ProcessManager::P_NOTHING;
				sque.nEventNo = FACT_KS_END;
				sque.nLineNo = getlineid();															// ラインID
				send_mail(getmytaskname(), ".", &sque);	
			}
			break;
//-----------------------------------------------------------------------------------------------
		case FACT_KS_PCNAME_CHANGE:															// PC名称変更通知起動要因
			//// 全PC名称をDBより取得
			if( getpcname() ) {
				pLOG(em_MSG), "[MAIL] 全PC名称をDBより取得");
			} else {
				pLOG(em_ERR), "[MAIL] 全PC名称をDBより取得失敗");
			}
			//// PCスレーブ管理に通知
			sque.nEventNo = FACT_KS_PCNAME_CHANGE;
			sque.nLineNo = getlineid();
			send_mail( KS_SLAVE, "*", &sque );
			//// 常駐プロセス全員に通知
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


//------------------------------------------
// 検査停止通知 送信
//------------------------------------------
void CKS_MASTERDlg::SendMail_Stop()
{
	char cPc[SIZE_NAME_PC];
	char cTask[SIZE_NAME_TASK];
	int	 nMailNo;
	int  nWaitTime;

	//// そもそも実施する？
	GetPrivateProfileString(KS_MASTER, "STOP_SEND_MAIL_PC", "", cPc, sizeof(cPc), PCINI_NAME);			
	GetPrivateProfileString(KS_MASTER, "STOP_SEND_MAIL_TASK", "", cTask, sizeof(cTask), PCINI_NAME);
	nMailNo = GetPrivateProfileInt(KS_MASTER, "STOP_SEND_MAIL_NO", 0, PCINI_NAME);
	nWaitTime = GetPrivateProfileInt(KS_MASTER, "STOP_AFFTER_WAIT_TIME", 0, PCINI_NAME);

	KizuFunction::Trim(cPc);
	KizuFunction::Trim(cTask);

	if( 1 > strlen(cPc) ) return;

	// メール送信
	COMMON_QUE sque;		// 送信構造体
	sque.nEventNo = nMailNo;
	sque.nLineNo = getlineid();															// ラインID
	send_mail(cTask, cPc, &sque);	

	Sleep(nWaitTime*1000);
}


//￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣
// 画面更新タイマ
//＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿
//void CKS_MASTERDlg::OnTimer(UINT_PTR nIDEvent)
//{
//	// TODO: ここにメッセージ ハンドラ コードを追加するか、既定の処理を呼び出します。
//
//	CDialog::OnTimer(nIDEvent);
//}

//￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣
// デバックコマンド
//＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿
void CKS_MASTERDlg::OnBnClickedAlldown()
{
	if( IDYES == MessageBox("全PCをシャットダウンしますか？", "重要なお知らせ", MB_YESNO | MB_DEFBUTTON2 | MB_ICONQUESTION) ) {
		LOG(em_MSG), "全PCシャットダウンボタン");
		COMMON_QUE que;
		que.nEventNo = FACT_KS_MASTER_06;
		que.nLineNo = getlineid();															// ラインID
		send_mail(getmytaskname(), ".", &que);	
	}
}
void CKS_MASTERDlg::OnBnClickedAllreboot()
{
	if( IDYES == MessageBox("全PCを再起動しますか？", "重要なお知らせ", MB_YESNO | MB_DEFBUTTON2 | MB_ICONQUESTION) ) {
		LOG(em_MSG), "全PC再起動ボタン");
		COMMON_QUE que;
		que.nEventNo = FACT_KS_MASTER_07;
		que.nLineNo = getlineid();															// ラインID
		strcpy(que.fc.data, "*");
		send_mail(getmytaskname(), ".", &que);	
	}
}
void CKS_MASTERDlg::OnBnClickedAlllogoff()
{
	if( IDYES == MessageBox("全PCをログオフしますか？", "重要なお知らせ", MB_YESNO | MB_DEFBUTTON2 | MB_ICONQUESTION) ) {
		LOG(em_MSG), "全PCログオフボタン");
		COMMON_QUE que;
		que.nEventNo = FACT_KS_MASTER_08;
		que.nLineNo = getlineid();															// ラインID
		strcpy(que.fc.data, "*");
		send_mail(getmytaskname(), ".", &que);	
	}
}

void CKS_MASTERDlg::OnBnClickedAlltaskend()
{
	if( IDYES == MessageBox("全PCの常駐タスクを終了しますか？", "重要なお知らせ", MB_YESNO | MB_DEFBUTTON2 | MB_ICONQUESTION) ) {
		LOG(em_MSG), "全PC常駐タスク終了ボタン");
		COMMON_QUE que;
		que.nEventNo = FACT_KS_MASTER_09;
		que.nLineNo = getlineid();															// ラインID
		strcpy(que.fc.data, "*");
		send_mail(getmytaskname(), ".", &que);	
	}
}

void CKS_MASTERDlg::OnBnClickedMyreboot()
{
	if( IDYES == MessageBox("自PCを再起動しますか？", "重要なお知らせ", MB_YESNO | MB_DEFBUTTON2 | MB_ICONQUESTION) ) {
		LOG(em_MSG), "自PC再起動ボタン");
		COMMON_QUE que;
		que.nEventNo = FACT_KS_MASTER_07;
		que.nLineNo = getlineid();															// ラインID
		strcpy(que.fc.data, MASTER_PC_NAME);
		send_mail(getmytaskname(), ".", &que);	
	}
}

void CKS_MASTERDlg::OnBnClickedMylogoff()
{
	if( IDYES == MessageBox("自PCをログオフしますか？", "重要なお知らせ", MB_YESNO | MB_DEFBUTTON2 | MB_ICONQUESTION) ) {
		LOG(em_MSG), "自PCログオフボタン");
		COMMON_QUE que;
		que.nEventNo = FACT_KS_MASTER_08;
		que.nLineNo = getlineid();															// ラインID
		strcpy(que.fc.data, MASTER_PC_NAME);
		send_mail(getmytaskname(), ".", &que);	
	}
}