#include "StdAfx.h"
#include "MainInstance.h"

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
MainInstance::MainInstance(void) : 
m_nUnitNum(0),
ThreadManager("MI")
{
	// シグナル生成
	m_evThStart	= CreateEvent(NULL, FALSE, FALSE, NULL);				// メインスレッド同期用
	
	// 初期化
	for(int ii=0; ii<MAX_THREAD; ii++) gcls_pLinksw[ii] = NULL;
}

//------------------------------------------
// デストラクタ
//------------------------------------------
MainInstance::~MainInstance(void)
{
	// シグナル解放
	CloseHandle(m_evThStart);
}

//------------------------------------------
// 初期化処理 (一度のみ起動)
//------------------------------------------
BOOL MainInstance::Init()
{
	//// メインインスンタスのみ、スレッドだけは作っておく (すぐに非シグナルとする)
	//// 継承元クラス実行 (メイン制御スレッドの為、ちょっと優先度高め)
	ThreadManager::Start( THREAD_PRIORITY_ABOVE_NORMAL );

	// スレッドIDが割りつくまでちょっと休憩
	while( 0==GetTid() )
	{
		LOG(em_MSG), "[%s] スレッドID割り当て中・・・", my_sThreadName);
		Sleep(1);			// スレッド起動までちょっと待ち
	}

	////////////////////////////////////
	//// ワーカースレッド 初期化
	////////////////////////////////////
	//// カメラリンクスイッチ通信クラス	
	CString		sSession;
	CString		sWk;
	char		cKey[64];
	sSession.Format("%s_MAIN", TO_LINKSW);
	for(int ii=0; ii<MAX_THREAD; ii++) {
		sWk.Format("NAME_%d", ii+1);
		GetPrivateProfileString(sSession, sWk, "", cKey, sizeof(cKey), TASKINI_NAME);
		if( 0 == ii && 0 == strlen(cKey) ) { strcpy(cKey, TO_LINKSW); }		// 絶対に１つは存在させる。
		if( 0 == strlen(cKey) ) break;										// キーチェック

		// インスタンス生成
		gcls_pLinksw[ii] = new LinkswManager();
		gcls_pLinksw[ii]->SetLogMgr(mcls_pLog);
		gcls_pLinksw[ii]->SetClass(ii, cKey);
		gcls_pLinksw[ii]->SetMainPostTid(GetTid());

		m_nUnitNum ++;
	}


	////////////////////////////////////
	//// ワーカースレッド 起動
	////////////////////////////////////
	for (int ii = 0; ii<MAX_THREAD; ii++) {
		if (NULL == gcls_pLinksw[ii]) continue;
		gcls_pLinksw[ii]->Start();
	}


	return TRUE;
}

//------------------------------------------
// 終了処理 (一度のみ起動)
//------------------------------------------
void MainInstance::Exit()
{
	// ワーカースレッド停止
	for (int ii = 0; ii<MAX_THREAD; ii++) {
		if (NULL == gcls_pLinksw[ii]) continue;
		gcls_pLinksw[ii]->Stop();
	}

	// ワーカースレッド解放
	for (int ii = 0; ii<MAX_THREAD; ii++) {
		if (NULL == gcls_pLinksw[ii]) continue;
		SAFE_DELETE(gcls_pLinksw[ii]);
	}
}


// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// スレッド動作定義


//------------------------------------------
// スレッド開始前処理 (スレッド実行直後(シグナルセット前)にコールバック)
//------------------------------------------
int MainInstance::ThreadFirst()
{

	//// 他ワークスレッドの準備が整うまで待機 (MainInstanceのみ)
	LOG(em_MSG), "[%s] 他ワークスレッドの準備が整うまで待機中・・・ [0x%X]", my_sThreadName, GetTid() );
	WaitForSingleObject(m_evThStart, INFINITE);

	return 0;
}

//------------------------------------------
// スレッド終了前処理 (終了シグナル後にコールバック)
//------------------------------------------
int MainInstance::ThreadLast()
{
	return 0;
}

//------------------------------------------
// スレッドイベント発生
// int nEventNo イベントNo (0オリジン)
//------------------------------------------
void MainInstance::ThreadEvent(int nEventNo)
{
	if( nEventNo == 0 ){

	}

}
