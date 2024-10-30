#include "StdAfx.h"
#include "MainInctance.h"


//------------------------------------------
// コンストラクタ
//------------------------------------------
MainInctance::MainInctance(void) :
m_bInitalize(false),
m_bStateMaster(false),
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
	//// PCマスター管理チェック有無
	int nWk = GetPrivateProfileInt(KS_SLAVE, "DEBUG", 0, PCINI_NAME);
	if( 0 != nWk && 1 != nWk && 2 != nWk ) nWk = 0;
	m_nDebug = nWk;

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
	gcls_pCSta = new CheckStatus(false);
	gcls_pCSta->SetLogMgr(mcls_pLog);
	gcls_pCSta->SetMainPostTid(GetTid());

	// データベース管理スレッド
	gcls_pCDb = new CheckDB(false);
	gcls_pCDb->SetLogMgr(mcls_pLog);
	gcls_pCDb->SetMainPostTid(GetTid());

	//// ワーカースレッド 起動
	if(2 == m_nDebug) {
		//PostThreadMessage(GetTid(), E_DEF_DB_FAST_CONNECT, 0, 0);
		gcls_pCDb->SetFastExecDebug();	// 強制OK状態
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
	if(2 != m_nDebug ) {
		gcls_pCDb->Stop();
	}

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
	COMMON_QUE que;
	UINT_PTR tId_Check = 0;					// 定周期 マスター確認タイマーID
	int	nMasterRecvCnt = 0;					// PCマスター管理からの立ち上げ完了通知受信回数

	////// メインインスタンス取り出し
	MainInctance *p = (MainInctance *)param;						// 自クラスのインスタンスを取得
	PeekMessage(&msg, NULL, WM_USER, WM_USER, PM_NOREMOVE);			// メッセージ作成
	p->SetTid();													// スレッドIDセット

	////// 他ワークスレッドの準備が整うまで待機
	pLOG(em_MSG), "[MainInctance] 他ワークスレッドの準備が整うまで待機中・・・ [0x%X]", p->GetTid() );
	WaitForSingleObject(p->m_evThStart, INFINITE);
	
	//// ローカルタイマー始動
	SAFE_TIMER_SET(tId_Check,	  PARAM_TIMEID_CHECK,	  PARAM_TIMEOUT_CHECK);		// 定周期 マスター状態問い合わせタイマー

	////// メッセージキュー読み込み
	pLOG(em_MSG), "[MainInctance] スレッド開始 [0x%X]", p->GetTid());
	while( int ret = GetMessage(&msg, NULL, 0, 0) ){
		_ASSERT( -1 != ret );
		switch (msg.message) {

//-----------------------------------------------------------------------------------------------
		case WM_TIMER:							// タイマー起動
			if( msg.wParam == tId_Check ) {						// 定周期 マスター確認タイマー
				// PCマスター管理へ状態問い合わせ
				que.nEventNo = FACT_KS_MASTER_03;
				que.nLineNo = getlineid();															// ラインID
				strcpy(que.fc.data, getmypcname() );  
				int retc = send_mail( KS_MASTER, MASTER_PC_NAME, &que );		
				if( 0 != retc ) syslog(SYSNO_MAIL_SNED_ERR, "[%d:PCマスター管理へ状態問い合わせ <err=%d>]", FACT_KS_MASTER_03, retc);
				pLOG(em_MSG), "PCマスター管理へ状態問い合わせ中･･･(%s)", getmypcname());
				
				// デバック用
				if( 0 != p->m_nDebug ) {
					memset( &que, 0x00, sizeof(que));
					que.nEventNo = FACT_KS_SLAVE_03;
					que.nLineNo = getlineid();
					send_mail( getmytaskname(), ".", &que );
				}
			}
			break;

//-----------------------------------------------------------------------------------------------
		case E_DEF_DB_FAST_CONNECT:				// DB初期接続完了通知
			//// イニシャル中待機画面へDB接続完了通知送信
			que.nEventNo = FACT_KS_WAKEUP_02; 
			que.nLineNo = getlineid();															// ラインID
			send_mail( KS_WAKEUP, ".", &que );	

			// DB, PCマスター管理 が両方正常の時のみ
			if( ! p->gcls_pCDb->GetFastExec() && p->m_bStateMaster ) {
				p->Init_Process();							// プロセス生成
			}
			break;

//-----------------------------------------------------------------------------------------------
		case E_DEF_ML_MASTEROK:					// PCマスター管理立ち上げ完了通知
			SAFE_TIMER_KILL(tId_Check);					// PCマスター管理OKの為、タイマー終了
			nMasterRecvCnt ++;

			// 念の為にもう一度マスターの確認 (NECのパソコンにしてから、マスターがメールを正常に受信出来ない事があったための処置)
			if( 1 == nMasterRecvCnt ) {
				// 1回目
				SAFE_TIMER_SET(tId_Check,	  PARAM_TIMEID_CHECK,	  PARAM_TIMEOUT_CHECK2);		// 定周期 マスター状態問い合わせタイマー
				pLOG(em_MSG), "立ち上げ完了報告 受付1回目なので、再度問い合わせ実施開始");
				// デバック用
				if( 0 != p->m_nDebug ) {
					memset( &que, 0x00, sizeof(que));
					que.nEventNo = FACT_KS_SLAVE_03;
					que.nLineNo = getlineid();
					send_mail( getmytaskname(), ".", &que );
				}

			} else {
				if( ! p->m_bStateMaster ) {
					//// PCマスター管理立ち上がり完了済み
					p->m_bStateMaster = true;

					//// イニシャル中待機画面へPCマスター管理立ち上げ完了通知送信
					que.nEventNo = FACT_KS_WAKEUP_01; 
					que.nLineNo = getlineid();															// ラインID
					send_mail( KS_WAKEUP, ".", &que );


					// DB, PCマスター管理 が両方正常の時のみ
					if( ! p->gcls_pCDb->GetFastExec() ) {
						p->Init_Process();							// プロセス生成
					}

				} else {
					// 統括へ 立ち上げ完了通知送信
					p->Send_MASTER_OK();
				}
			}
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
	LOG(em_MSG), "Init_Process開始");
	Sleep(1000);					// DBが起動完了後の動作不安定をなくす為にちょっと待ち。

	COMMON_QUE que;
	if(m_bInitalize) return;		// 2タイミングあるから念の為に 整合性確認
	m_bInitalize = true;

//	//// ファイルコピー
//	ExecFileCopy();


	//// コマンド実行
	Exec_Cmd( "START" );


	//// 全PC名称をDBより取得
	if(2 != m_nDebug ) {
		if( getpcname() ) {
			LOG(em_MSG), "全PC名称をDBより取得");
		} else {
			LOG(em_ERR), "全PC名称をDBより取得失敗");		// ここでエラーはありえないはず
		}
	}

	//// 機器状態管理クラスの初期化
	gcls_pCSta->Alloc();

	//// 共有メモリ生成
	mcls_Smem.CreateMem(true); 
	LOG(em_MSG), "共有メモリ生成完了");

	//// 常駐プロセス起動
	ProcessManager::ExecProcess(); 
	LOG(em_MSG), "常駐プロセス起動完了");
	
	//// 機器状態管理クラス起動
	gcls_pCSta->Start();

	//// PCマスター管理に 立ち上げ完了報告
	Send_MASTER_OK();

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
	que.fl.data[0] = GetPrivateProfileInt(KS_SLAVE, "APP_ENABLE_ID", 0, PCINI_NAME);
	que.fl.data[1] = GetPrivateProfileInt(KS_SLAVE, "HB_SEND_ENABLE", 0, PCINI_NAME);
	int retc = send_mail( KS_MASTER, MASTER_PC_NAME, &que );
	if( 0 != retc ) syslog(SYSNO_MAIL_SNED_ERR, "[%d:立ち下げ完了報告 <err=%d>]", FACT_KS_MASTER_05, retc);
}


//------------------------------------------
// 統括へ 立ち上げ完了通知 アンサー
//------------------------------------------
void MainInctance::Send_MASTER_OK()
{
	COMMON_QUE que;

	//// PCマスター管理に 立ち上げ完了報告
	que.nEventNo = FACT_KS_MASTER_04; 
	que.nLineNo = getlineid();															// ラインID
	que.fl.data[0] = GetPrivateProfileInt(KS_SLAVE, "APP_ENABLE_ID", 0, PCINI_NAME);
	que.fl.data[1] = GetPrivateProfileInt(KS_SLAVE, "HB_SEND_ENABLE", 0, PCINI_NAME);
	int retc = send_mail( KS_MASTER, MASTER_PC_NAME, &que );	
	if( 0 != retc ) syslog(SYSNO_MAIL_SNED_ERR, "[%d:立ち上げ完了報告 <err=%d>]", FACT_KS_MASTER_04, retc);
	LOG(em_MSG), "立ち上げ完了報告");
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
		GetPrivateProfileString(KS_SLAVE,	sKey, "", cWk, sizeof(cWk), PCINI_NAME); 
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
		int nWait = GetPrivateProfileInt(KS_SLAVE, sKey, 0, PCINI_NAME);
		nWait *= 1000;		// [s]→[ms]
		if(0 != nWait) Sleep(nWait);
	
		LOG(em_MSG), "[%s]時のコマンド実行完了", cIniKey);
	}
}




/*
//------------------------------------------
// ファイルコピー実行
//------------------------------------------
void MainInctance::ExecFileCopy()
{
	char		cFrom[MAX_PATH];		// コピー元
	char		cTo[MAX_PATH];			// コピー先
	CString		cKey;

	// コピー実行
	int cnt = 1;
	while(true) {
		cKey.Format("F_%d", cnt); 
		GetPrivateProfileString("FILECOPY", cKey, "", cFrom, MAX_PATH, TASKINI_NAME);
		cKey.Format("T_%d", cnt); 
		GetPrivateProfileString("FILECOPY", cKey, "", cTo,   MAX_PATH, TASKINI_NAME);

		// 有無
		if(0 == strlen(cFrom) || 0 == strlen(cTo)) break;

		// コピー
		if( ! CopyFile(cFrom, cTo, FALSE) ) {
			LOG(em_ERR), "[FILECOPY] [%s→%s]", cFrom, cTo);
			return;
		}
		cnt ++;
	}
}
*/