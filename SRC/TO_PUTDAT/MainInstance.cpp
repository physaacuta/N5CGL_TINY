#include "StdAfx.h"
#include "MainInstance.h"

// メモリーリーク検出 ----->>>
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif
// <<<------------------------

//------------------------------------------
// コンストラクタ
//------------------------------------------
MainInstance::MainInstance(void) :
ThreadManager("MI")
{
	// シグナル生成
	m_evThStart  = CreateEvent(NULL, FALSE, FALSE, NULL);				// メインスレッド同期用
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
// スレッドキューにメールデータ登録
// ENUM_MAININSTANCE emNo メッセージNo (イベントNoそのままにしようかと思ったが、FACT??_01とかだと分かりにくいからやめた)
// COMMON_QUE* que メールスロット構造体そのもの
//------------------------------------------
void MainInstance::SetDeliveryMail(ENUM_MAININSTANCE emNo, COMMON_QUE* que)
{
	DELIVERY_MAIL* pMail = new DELIVERY_MAIL;
	pMail->emNo	= emNo;
	memcpy( &pMail->que, que, sizeof(COMMON_QUE));

	// スレッドキューに登録
	if (!mque_DeliMail.AddToTailFreeCheck(pMail))
	{
		SAFE_DELETE(pMail);
		LOG(em_ERR), "[%s] mque_DeliMail キューフル!!", my_sThreadName);
	}
}

//------------------------------------------
// 初期化処理 (一度のみ起動)
//------------------------------------------
BOOL MainInstance::Init()
{
	//// メインインスタンスのみ、スレッドだけは作っておく (すぐに非シグナルとする)
	//// 継承元クラス実行 (メイン制御スレッドの為、ちょっと優先度高め)
	ThreadManager::Start(THREAD_PRIORITY_ABOVE_NORMAL);

	// スレッドIDが割りつくまでちょっと休憩
	while (0==GetTid())
	{
		LOG(em_MSG), "[%s] スレッドID割り当て中・・・", my_sThreadName);
		Sleep(1);			// スレッド起動までちょっと待ち
	}
	
	//// ワーカースレッド 初期化
	// プロコン検査実績作成管理クラス
	gcls_Pro.SetLogMgr(mcls_pLog);
	// ＦＦ検査実績作成管理クラス
	gcls_FF.SetLogMgr(mcls_pLog);
	gcls_FF.SetMakeProMgr(&gcls_Pro); 

	//// ワーカースレッド 起動
	gcls_Pro.Start();
	gcls_FF.Start();

	return TRUE;
}

//------------------------------------------
// 終了処理 (一度のみ起動)
//------------------------------------------
void MainInstance::Exit()
{
	//// ワーカースレッド 停止
	gcls_Pro.SetEvDefectDbStop();	// 統括待ちをしている場合は、強制的にキャンセル
	gcls_Pro.Stop();
	gcls_FF.Stop();
}

//------------------------------------------
// スレッド開始前処理 (スレッド実行直後(シグナルセット前)にコールバック)
//------------------------------------------
int MainInstance::ThreadFirst()
{
	////// 他ワークスレッドの準備が整うまで待機 (MainInstanceのみ)
	LOG(em_MSG), "[%s] 他ワークスレッドの準備が整うまで待機中・・・ [0x%X]", my_sThreadName, GetTid() );
	WaitForSingleObject(m_evThStart, INFINITE);
	
	//// イベント設定
	HANDLE hArray[] =
	{
		mque_DeliMail.g_evSem
	};
	SetEventNum(sizeof(hArray)/sizeof(HANDLE), hArray);
	return 0;
}

//------------------------------------------
// スレッドイベント発生
// int nEventNo イベントNo (0オリジン)
//------------------------------------------
void MainInstance::ThreadEvent(int nEventNo)
{
	DELIVERY_MAIL* pMail;

	////// シグナルの条件
	enum
	{
		EV_MAIL = 0,					// メールスロット 通知 (メールは一番最後にしようかな)
	};

	////// シグナル条件分け
	switch (nEventNo)
	{
//-----------------------------------------------------------------------------------------------
	case EV_MAIL: 						// メールスロット 通知
		pMail = mque_DeliMail.GetFromHead();
		switch (pMail->emNo)
		{
//-----------------------------------------------------------
		case E_DEF_ML_PRO_R_CUT:								// カット実績受信通知
			if (true)
			{
				MakeProManager::DELIVERY_KEY* pQue = new MakeProManager::DELIVERY_KEY;
				memset(pQue, 0x00, sizeof(MakeProManager::DELIVERY_KEY));
				strcpy(pQue->cKey, pMail->que.fc.data);
				gcls_Pro.gque_Deli.AddToTail(pQue); 
			}
			break;

//-----------------------------------------------------------
		case E_DEF_ML_PRO_R_SAI:								// 採寸実績受信通知
			if (true)
			{
				MakeFfManager::DELIVERY_KEY* pQue = new MakeFfManager::DELIVERY_KEY;
				memset(pQue, 0x00, sizeof(MakeFfManager::DELIVERY_KEY));
				strcpy(pQue->cKey, pMail->que.fc.data);
				gcls_FF.gque_Deli.AddToTail(pQue); 
			}
			break;

//-----------------------------------------------------------
		case E_DEF_ML_COIL_RESULT_ADD:							// 実績書き込み完了通知
			// シグナル通知
			gcls_Pro.SetEvDefectDbAddOk(pMail->que.fl.data[0]);
			break;

//-----------------------------------------------------------
		}
		// 解放
		delete pMail;
		return;

//-----------------------------------------------------------------------------------------------
	default :
		// ありえない！！
		LOG(em_ERR), "[%s] ThreadEvent EventNo NG=%d", my_sThreadName, nEventNo );
		return;
	}
}
