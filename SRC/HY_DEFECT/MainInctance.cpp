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
gque_OpRecv(QSIZE_TO_HY),
gque_ImgSave(QSIZE_IMG_SAVE),
gque_ImgSaveLow(QSIZE_IMG_SAVE_LOW),
ThreadManager("MI")
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
// スレッドキューにメールデータ登録
// ENUM_MAININCTANCE emNo メッセージNo (イベントNoそのままにしようかと思ったが、FACT??_01とかだと分かりにくいからやめた)
// COMMON_QUE* que メールスロット構造体そのもの
//------------------------------------------
void MainInctance::SetDeliveryMail(ENUM_MAININCTANCE emNo, COMMON_QUE* que)
{
	DELIVERY_MAIL* pMail = new DELIVERY_MAIL;
	pMail->emNo	= emNo;
	memcpy( &pMail->que, que, sizeof(COMMON_QUE));

	// スレッドキューに登録
	if( ! mque_DeliMail.AddToTailFreeCheck(pMail) ) {
		delete pMail;
		LOG(em_ERR), "[%s] mque_DeliMail キューフル!!", my_sThreadName);
	}
}

//------------------------------------------
// 初期化処理 (一度のみ起動)
//------------------------------------------
BOOL MainInctance::Init()
{
	//// メインインスタンスのみ、スレッドだけは作っておく (すぐに非シグナルとする)
	//// 継承元クラス実行 (メイン制御スレッドの為、ちょっと優先度高め)
	ThreadManager::Start(THREAD_PRIORITY_ABOVE_NORMAL );

	// スレッドIDが割りつくまでちょっと休憩
	while( 0==GetTid() ) {
		LOG(em_MSG), "[%s] スレッドID割り当て中・・・", my_sThreadName);
		Sleep(1);			// スレッド起動までちょっと待ち
	}


	////////////////////////////////////
	//// パラメータ読込待ち
	////////////////////////////////////
	// パラメータ管理クラス
	gcls_pParam = new ParamManager();
	gcls_pParam->SetLogMgr(mcls_pLog);
	while( true ) {
		if( gcls_pParam->ReadParam_All(true) ) break;

		LOG(em_MSG), "[%s] パラメータ読込失敗(DB接続異常？！)。リトライ中・・・", my_sThreadName);
		Sleep(10000);			
	}


	////////////////////////////////////
	//// ワーカースレッド 初期化
	////////////////////////////////////
	// 受信クラス
	gcls_pRecv = new KizuDefaultRecv(HY_DEFECT, &gque_OpRecv);
	gcls_pRecv->SetLogMgr(mcls_pLog);

	// ファイル削除管理クラス
	gcls_FDele.SetLogMgr(mcls_pLog);

	// ファイル保存管理クラス
	for(int ii=0; ii<THREAD_IMG_SAVE; ii++) {
		gcls_pFSave[ii] = new FileSaveManager();
		gcls_pFSave[ii]->SetLogMgr(mcls_pLog);
		gcls_pFSave[ii]->SetQueImgSave(&gque_ImgSave);
		gcls_pFSave[ii]->SetQueImgSaveLow(&gque_ImgSaveLow);
	}

	// データセットクラス
	gcls_pData = new DataSetManager();
	gcls_pData->SetLogMgr(mcls_pLog);
	gcls_pData->SetParamMgr(gcls_pParam);
	gcls_pData->SetFDeleMgr(&gcls_FDele);
	gcls_pData->SetQueImgSave(&gque_ImgSave);
	gcls_pData->SetQueImgSaveLow(&gque_ImgSaveLow);
	gcls_pData->SetQueOpRecv(&gque_OpRecv);
// 2023.11.17 最終工程検査装置高機能化その４ -------------------->>>>>
	gcls_pData->SetQue_MaeRequest(&gque_DataRequest);
// 2023.11.17 最終工程検査装置高機能化その４ <<<<<--------------------
// 2024.07.26 Kouno 2CGL検査装置φω精度向上機能導入(長周期) ---------------->>>>>
	gcls_pData->SetQue_LongPdRequest(&gque_DataRequestLongPd);
// 2024.07.26 Kouno 2CGL検査装置φω精度向上機能導入(長周期) <<<<<----------------
	gcls_pData->Allock();

	// 前工程管理クラス
	gcls_pMaeMgr = new MaeManager();
	gcls_pMaeMgr->SetLogMgr(mcls_pLog);
	gcls_pMaeMgr->SetParamMgr(gcls_pParam);
	gcls_pMaeMgr->SetDataSetMgr(gcls_pData);
	gcls_pMaeMgr->SetQue_MaeRequest(&gque_MaeRequest);

	////////////////////////////////////
	//// ワーカースレッド 起動
	////////////////////////////////////
	gcls_FDele.Start();
	for(int ii=0; ii<THREAD_IMG_SAVE; ii++) gcls_pFSave[ii]->Start();

	gcls_pParam->Start();
	gcls_pRecv->Start();
	gcls_pData->Start();
	gcls_pMaeMgr->Start();

	return TRUE;
}

//------------------------------------------
// 終了処理 (一度のみ起動)
//------------------------------------------
void MainInctance::Exit()
{
	//// ワーカースレッド 停止
	gcls_pMaeMgr->Stop();
	gcls_pRecv->Stop();
	gcls_pData->Stop();
	gcls_pParam->Stop();
	gcls_FDele.Stop();

	gcls_pFSave[0]->SetEvClearQue();			// 未処理キュー開放 
	for(int ii=0; ii<THREAD_IMG_SAVE; ii++) gcls_pFSave[ii]->Stop();

	//// ワーカースレッド 開放
	delete gcls_pRecv;
	delete gcls_pData;
	delete gcls_pMaeMgr;
	delete gcls_pParam;

	for(int ii=0; ii<THREAD_IMG_SAVE; ii++) delete gcls_pFSave[ii];

}

//------------------------------------------
// スレッド開始前処理 (スレッド実行直後(シグナルセット前)にコールバック)
//------------------------------------------
int MainInctance::ThreadFirst()
{
	////// 他ワークスレッドの準備が整うまで待機 (MainInctanceのみ)
	LOG(em_MSG), "[%s] 他ワークスレッドの準備が整うまで待機中・・・ [0x%X]", my_sThreadName, GetTid() );
	WaitForSingleObject(m_evThStart, INFINITE);


	//// イベント設定
	HANDLE hArray[] = { mque_DeliMail.g_evSem
						};
	SetEventNum(sizeof(hArray)/sizeof(HANDLE), hArray);
	return 0;
}

//------------------------------------------
// スレッドイベント発生
// int nEventNo イベントNo (0オリジン)
//------------------------------------------
void MainInctance::ThreadEvent(int nEventNo)
{
	DELIVERY_MAIL* pMail;

	////// シグナルの条件
	enum {	EV_MAIL = 0,					// メールスロット 通知 (メールは一番最後にしようかな)
	};

	////// シグナル条件分け
	switch (nEventNo) {

//-----------------------------------------------------------------------------------------------
	case EV_MAIL: 							// メールスロット 通知
		pMail = mque_DeliMail.GetFromHead();
		switch(pMail->emNo) {

//-----------------------------------------------------------
		case E_DEF_ML_PARAM:				// パラメータ変更通知
			if(true) {
				Send_Hy_Param(&pMail->que);

				// パラメータ変更コマンド
				COMMON_QUE* pQue = new COMMON_QUE;
				memcpy( pQue, &pMail->que, sizeof(COMMON_QUE));
				gcls_pParam->gque_Deli.AddToTailFreeCheck(pQue); 
			}
			break;
//-----------------------------------------------------------
		case E_DEF_ML_INIT:					// イニシャル通知
			Send_Hy_Init();
			gcls_pData->SetEvClearTbl();	// 共有メモリに対しても初期化通知を送る 
			break;

			//-----------------------------------------------------------
		case E_DEF_ML_MAE_READ:				// 前工程疵情報読込通知
			if (true) {
				MaeManager::TYP_MAE_REQUEST* pDeli = new MaeManager::TYP_MAE_REQUEST;
				pDeli->nMode = 0;
				pDeli->pData = NULL;	// 読込依頼時は NULL固定
				memcpy(pDeli->cKanriNo, pMail->que.fc.data, SIZE_KIZUKEN_NO);
				gque_MaeRequest.AddToTailFreeCheck(pDeli);
			}
			break;

// 2023.11.17 最終工程検査装置高機能化その４ -------------------->>>>>
//-----------------------------------------------------------
		case E_DEF_ML_MAE_READ_END:			// 前工程疵情報読込完了通知
			if (true) {
				DataSetManager::TYP_DATASET_REQMAE* pDeli = new DataSetManager::TYP_DATASET_REQMAE;
				pDeli->nRec = pMail->que.fl.data[0];
				gque_DataRequest.AddToTailFreeCheck(pDeli);
			}
			break;
// 2023.11.17 最終工程検査装置高機能化その４ <<<<<--------------------

// 2024.07.26 Kouno 2CGL検査装置φω精度向上機能導入(長周期) ---------------->>>>>
//-----------------------------------------------------------
		case E_DEF_ML_LONGPD_OCCUR:			// 長周期発生通知
			if (true) {
				DataSetManager::TYP_DATASET_REQLONGPD* pDeli = new DataSetManager::TYP_DATASET_REQLONGPD;
				memset(pDeli, 0x00, sizeof(DataSetManager::TYP_DATASET_REQLONGPD));
				memcpy(pDeli->cKanriNo, pMail->que.mix.cdata[0], SIZE_KIZUKEN_NO);

				gque_DataRequestLongPd.AddToTailFreeCheck(pDeli);
			}
			break;
// 2024.07.26 Kouno 2CGL検査装置φω精度向上機能導入(長周期) <<<<<----------------

			//-----------------------------------------------------------
		}
		// 開放
		delete pMail;
		return;

//-----------------------------------------------------------------------------------------------
	default :
		// ありえない！！
		LOG(em_ERR), "[%s] ThreadEvent EventNo NG=%d", my_sThreadName, nEventNo );
		return;
	}
}

	
// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 外部タスク通知


//------------------------------------------
// 他タスクにパラメータ変更通知を送信
// COMMON_QUE* pQue キュー受け渡し
//------------------------------------------
void MainInctance::Send_Hy_Param(COMMON_QUE* pQue)
{
	COMMON_QUE  que;
	memcpy(&que, pQue, sizeof(que));

	// 探傷画面
	que.nEventNo = FACT_HY_TANSHO_01;
	send_mail(HY_TANSHO, ".", &que);

	// その他画面
	// 重欠陥一覧画面
	que.nEventNo = FACT_HY_DEFLIST_01;
	send_mail(HY_DEFLIST, ".", &que);
}


//------------------------------------------
// 他タスクに初期化通知を送信
//------------------------------------------
void MainInctance::Send_Hy_Init()
{
	COMMON_QUE  que;
	memset(&que, 0x00, sizeof(que));
	//que.nLineNo = getlineid();

	// 探傷画面
	que.nEventNo = FACT_HY_TANSHO_02;	
	send_mail(HY_TANSHO,".", &que);

}



