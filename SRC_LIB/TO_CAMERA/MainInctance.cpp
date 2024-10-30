#include "StdAfx.h"
#include "MainInctance.h"

// メモリーリーク検出 ----->>>
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif
// <<<------------------------

//------------------------------------------
// コンストラクタ
//------------------------------------------
MainInctance::MainInctance(void) :
m_nUnitNum(0),
mcls_pLog(NULL)
{
	// シグナル生成
	m_evThStart  = CreateEvent(NULL, FALSE, FALSE, NULL);				// メインスレッド同期用

	// 初期化
	for(int ii=0; ii<MAX_THREAD; ii++) gcls_pCam[ii] = NULL;
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
/*
	// カメラコントローラ通信クラス
	gcls_pCam = new CameraManager();
	gcls_pCam->SetLogMgr(mcls_pLog);
	gcls_pCam->SetMainPostTid(GetTid());
*/

	CString		sSession;
	CString		sWk;
	char		cKey[64];
	sSession.Format("%s_MAIN", TO_CAMERA);
	for(int ii=0; ii<MAX_THREAD; ii++) {
		sWk.Format("NAME_%d", ii+1);
		GetPrivateProfileString(sSession, sWk, "", cKey, sizeof(cKey), TASKINI_NAME);
		if( 0 == ii && 0 == strlen(cKey) ) { strcpy(cKey, TO_CAMERA); }		// 絶対に１つは存在させる。
		if( 0 == strlen(cKey) ) break;										// キーチェック

		// インスタンス生成
		gcls_pCam[ii] = new CameraManager();
		gcls_pCam[ii]->SetClass(ii, cKey);
		gcls_pCam[ii]->SetLogMgr(mcls_pLog);
		gcls_pCam[ii]->SetMainPostTid(GetTid());

		m_nUnitNum ++;
	}


	//// ワーカースレッド 起動
	for(int ii=0; ii<MAX_THREAD; ii++) {
		if( NULL == gcls_pCam[ii] ) continue;
		gcls_pCam[ii]->Start();
	}

	return TRUE;
}

//------------------------------------------
// 終了処理 (一度のみ起動)
//------------------------------------------
void MainInctance::Exit()
{
	//// ワーカースレッド 停止
	for(int ii=0; ii<MAX_THREAD; ii++) {
		if( NULL == gcls_pCam[ii] ) continue;
		gcls_pCam[ii]->Stop();
	}

	//// ワーカースレッド 開放
	for(int ii=0; ii<MAX_THREAD; ii++) {
		if( NULL == gcls_pCam[ii] ) continue;
		SAFE_DELETE( gcls_pCam[ii] );
	}
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


		case WM_USER_TH_END: 					// スレッド終了要求
			pLOG(em_MSG), "[MainInctance] スレッド終了 [0x%X]", p->GetTid());
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

