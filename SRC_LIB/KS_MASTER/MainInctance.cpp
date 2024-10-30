#include "StdAfx.h"
#include "MainInctance.h"

//------------------------------------------
// コンストラクタ
//------------------------------------------
MainInctance::MainInctance(void) :
m_bInitalize(false),
mcls_pLog(NULL)
{
	// シグナル生成
	m_evThStart  = CreateEvent(NULL, FALSE, FALSE, NULL);				// メインスレッド同期用
}

//------------------------------------------
// デストラクタ
//------------------------------------------
MainInctance::~MainInctance(void)
{
	// シグナル開放
	CloseHandle(m_evThStart);
}

//------------------------------------------
// 初期化処理 (一度のみ起動)
//------------------------------------------
BOOL MainInctance::Init()
{
	//// メインインスタンスのみ、スレッドだけは作っておく (すぐに非シグナルとする)
	//// 継承元クラス実行 (メイン制御スレッドの為、ちょっと優先度高め)
	WorkThread::Start(MainThread, THREAD_PRIORITY_ABOVE_NORMAL );

	// スレッドIDが割りつくまでちょっと休憩
	while( 0==GetTid() ) {
		LOG(em_MSG), "[MainInctance] スレッドID割り当て中・・・");
		Sleep(1);			// スレッド起動までちょっと待ち
	}

	//// ワーカースレッド 初期化
	// 機器状態管理クラス
	gcls_pCSta = new CheckStatus(true);
	gcls_pCSta->SetLogMgr(mcls_pLog);
	gcls_pCSta->SetMainPostTid(GetTid());

	// データベース管理スレッド
	gcls_pCDb = new CheckDB(true);
	gcls_pCDb->SetLogMgr(mcls_pLog);
	gcls_pCDb->SetMainPostTid(GetTid());
	if( 0==getpcid() || -1 == getpcid() ) {
		gcls_pCDb->SetFastExecDebug();	// 強制OK状態
		PostThreadMessage(GetTid(), E_DEF_DB_FAST_CONNECT, 0, 0);
	} else {
		gcls_pCDb->Start();
	}

	return TRUE;
}

//------------------------------------------
// 終了処理 (一度のみ起動)
//------------------------------------------
void MainInctance::Exit()
{

	//// ワーカースレッド 停止
	gcls_pCSta->Stop();
	gcls_pCDb->Stop();

	//// ワーカースレッド 開放
	delete gcls_pCSta;
	delete gcls_pCDb;
}

//------------------------------------------
// スレッド
//------------------------------------------
UINT WINAPI MainInctance::MainThread(LPVOID param)
{
	MSG	msg;								// MSG構造体

	////// メインインスタンス取り出し
	MainInctance *p = (MainInctance *)param;						// 自クラスのインスタンスを取得
	PeekMessage(&msg, NULL, WM_USER, WM_USER, PM_NOREMOVE);			// メッセージ作成
	p->SetTid();													// スレッドIDセット


	////// 他ワークスレッドの準備が整うまで待機
	pLOG(em_MSG), "[MainInctance] 他ワークスレッドの準備が整うまで待機中・・・ [0x%X]", p->GetTid() );
	WaitForSingleObject(p->m_evThStart, INFINITE);


	////// メッセージキュー読み込み
	pLOG(em_MSG), "[MainInctance] スレッド開始 [0x%X]", p->GetTid());
	while( int ret = GetMessage(&msg, NULL, 0, 0) ){
		_ASSERT( -1 != ret );
		switch (msg.message) {

//-----------------------------------------------------------------------------------------------
		case E_DEF_DB_FAST_CONNECT:					// DB初期接続完了通知
			p->Init_Process();							// プロセス生成
			break;


//-----------------------------------------------------------------------------------------------
		case E_DEF_DB_NG_TO_OK:						// DB接続状態 NG→OK (復旧したよ)
			// PC状態等が食い違っている場合対策
			p->Send_MASTER_OK("*");
			break;

//-----------------------------------------------------------------------------------------------
		case WM_USER_TH_END: 					// スレッド終了要求
			pLOG(em_MSG), "[MainInctance] スレッド終了 [0x%X]", p->GetTid());
			p->Exit_Process();							// プロセス開放
			return 0;

		default :
			// ありえない！！
			break;
		}
	}

	// ありえない
	pLOG(em_ERR), "[MainInctance] メインループから抜けた！！[0x%X]", p->GetTid());
	return 0;
}

//------------------------------------------
// プロセス スタート
//------------------------------------------
void MainInctance::Init_Process()
{
	COMMON_QUE que;

	m_bInitalize = true;

	//// コマンド実行
	Exec_Cmd( "START" );

	//// 全PC名称をDBより取得
	if( getpcname() ) {
		LOG(em_MSG), "全PC名称をDBより取得");
	} else {
		LOG(em_ERR), "全PC名称をDBより取得失敗");		// ここでエラーはありえないはず
	}

	//// 機器状態管理クラスの初期化
	gcls_pCSta->Alloc();


	//// イニシャル中待機画面へDB接続完了通知送信
	que.nEventNo = FACT_KS_WAKEUP_02; 
	que.nLineNo = getlineid();															// ラインID
	send_mail( KS_WAKEUP, ".", &que );	


	//// 共有メモリ生成
	mcls_Smem.CreateMem(true); 
	LOG(em_MSG), "共有メモリ生成完了");

	//// 常駐プロセス起動
	ProcessManager::ExecProcess(); 
	LOG(em_MSG), "常駐プロセス起動完了");
	
	//// 機器状態管理クラス起動
	gcls_pCSta->Start();

	//// 全PCへ 時刻設定要求
	Send_MASTER_TIME(); 

	//// 自分自身に 立ち上げ完了報告
	que.nEventNo = FACT_KS_MASTER_04; 
	que.nLineNo = getlineid();															// ラインID
	que.fl.data[0] = GetPrivateProfileInt(KS_MASTER, "APP_ENABLE_ID", 0, PCINI_NAME);
	que.fl.data[1] = GetPrivateProfileInt(KS_MASTER, "HB_SEND_ENABLE", 0, PCINI_NAME);
	int retc = send_mail( getmytaskname(), ".", &que );	
	if( 0 != retc ) syslog(SYSNO_MAIL_SNED_ERR, "[%d:立ち上げ完了報告 <err=%d>]", FACT_KS_MASTER_04, retc);

	//// 他PCへ PCマスター管理立ち上げ完了通知
	Send_MASTER_OK("*");

	//// イニシャル中待機画面 統括準備完了通知 (あんまり意味なし)
	que.nEventNo = FACT_KS_WAKEUP_01;
	que.nLineNo = getlineid();															// ラインID
	send_mail( KS_WAKEUP, ".", &que );

	//// イニシャル中待機画面 終了
	que.nEventNo = FACT_KS_END;
	que.nLineNo = getlineid();															// ラインID
	send_mail( KS_WAKEUP, ".", &que );
}

//------------------------------------------
// プロセス エンド
//------------------------------------------
void MainInctance::Exit_Process()
{
	//// 初期化していない場合は、なにもしない
	if( m_bInitalize) {
		//// 機器状態管理クラス停止
		gcls_pCSta->Stop();

		//// 常駐プロセス終了
		ProcessManager::ExitProcess(); 
		Sleep(1000);						// 全タスクが終了処理を受け付けるまでちょっと待ち

		//// 共有メモリ開放
		mcls_Smem.DeleteMem(); 
	}
	
	//// コマンド実行
	Exec_Cmd( "END" );

	//// 自分自身に 立ち下げ完了報告
	COMMON_QUE que;
	que.nEventNo = FACT_KS_MASTER_05; 
	que.nLineNo = getlineid();															// ラインID
	que.fl.data[0] = GetPrivateProfileInt(KS_MASTER, "APP_ENABLE_ID", 0, PCINI_NAME);
	que.fl.data[1] = GetPrivateProfileInt(KS_MASTER, "HB_SEND_ENABLE", 0, PCINI_NAME);
	int retc = send_mail( getmytaskname(), ".", &que );
	if( 0 != retc ) syslog(SYSNO_MAIL_SNED_ERR, "[%d:立ち下げ完了報告 <err=%d>]", FACT_KS_MASTER_05, retc);
}

//------------------------------------------
// 他PCへ PCマスター管理立ち上げ完了通知
// char const * cPc 送信PC名称
//------------------------------------------
void MainInctance::Send_MASTER_OK(char const * cPc)
{
	if( ! gcls_pCDb->GetFastExec() ) {
		COMMON_QUE que;

		// 通知
		LOG(em_MSG), "PCマスター管理立ち上げ完了通知 [送信先：%s]", cPc );
		que.nEventNo = FACT_KS_SLAVE_03;
		que.nLineNo = getlineid();															// ラインID
		int retc = send_mail( KS_SLAVE, cPc, &que );	
		if( 0 != retc ) syslog(SYSNO_MAIL_SNED_ERR, "[%d:PCマスター管理立ち上げ完了通知[%s] <err=%d>]", FACT_KS_SLAVE_03, cPc, retc);
	}
}
//------------------------------------------
// 全PCへ 時刻変更通知
//------------------------------------------
void MainInctance::Send_MASTER_TIME()
{
	LOG(em_MSG), "全PCへ 時刻変更通知");
	COMMON_QUE que;
	que.nEventNo = FACT_KS_SLAVE_01;
	que.nLineNo = getlineid();															// ラインID
	GetLocalTime(&que.timeset_que.systemtime);	// 自PCの時刻を取得	
	int retc = send_mail( KS_SLAVE, "*", &que );	
	if( 0 != retc ) syslog(SYSNO_MAIL_SNED_ERR, "[%d:時刻変更要求 <err=%d>]", FACT_KS_SLAVE_01, retc);
}


//------------------------------------------
// コマンド実行
// char const* cIniKey	AAA_%s	の%sに相当する部分
//------------------------------------------
void MainInctance::Exec_Cmd(char const* cIniKey)
{
	CString		sKey;
	char		cWk[1024];


	//// コマンド実行
	int			nNum = 0;		// 実行回数
	while(true) {
		sKey.Format("CMD_EXEC_%s_%d", cIniKey, nNum+1); 	
		GetPrivateProfileString(KS_MASTER,	sKey, "", cWk, sizeof(cWk), PCINI_NAME); 
		if( 0 == strlen(cWk)) break;


		// プロセス起動
		int nRetc = system( cWk );
		if( -1 == nRetc ) {
			LOG(em_ERR), "[%s]時のコマンド実行失敗(%d)(%s)", cIniKey, nRetc, cWk);
		} else {
			LOG(em_MSG), "[%s]時のコマンド実行成功(%d)(%s)", cIniKey, nRetc, cWk);
		}

		// これだと、コマンド直が難しいのでヤメ
		//STARTUPINFO si;
		//PROCESS_INFORMATION pi;
		//GetStartupInfo(&si);
		//si.dwFlags = 0;

		//CreateProcess(NULL, (LPSTR)cWk, NULL, NULL, FALSE, NORMAL_PRIORITY_CLASS, NULL, NULL, &si, &pi);
		//CreateProcess(NULL, (LPSTR)cWk, NULL, NULL, FALSE, NORMAL_PRIORITY_CLASS, NULL, NULL, &si, &pi);
		//CloseHandle(pi.hProcess);	// プロセスハンドルクローズ
		//CloseHandle(pi.hThread);	// プロセス・スレッド・ハンドルクローズ

		nNum ++;
	}

	//// １つでも実行できたときのみ
	if( 0 != nNum ) {
		sKey.Format("CMD_ENDWAIT_%s", cIniKey); 	
		int nWait = GetPrivateProfileInt(KS_MASTER, sKey, 0, PCINI_NAME);
		nWait *= 1000;		// [s]→[ms]
		if(0 != nWait) Sleep(nWait);
	
		LOG(em_MSG), "[%s]時のコマンド実行完了", cIniKey);
	}
}
