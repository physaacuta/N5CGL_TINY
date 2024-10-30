#include "StdAfx.h"
#include "MainInctance.h"

#include "HT_DEFECT.h"			// 画面の部品に直接アクセスするため


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
m_nCamPair(0),
m_nMen(-1),
m_emKindHt(DIV_HANTEI_HT),
m_bDetectProc(false),
m_bDetectProc_pd(false),
m_bDetectProc_pr(false),
m_bAdjustProc(false),
m_bRectProc(false),
m_bNgCam(false),
m_nAppParam(0),
ThreadManager("MI")
{
	for (int ii=0; ii<FRAME_HT_SEND_PORT; ii++) {
		gcls_pSockRpi[ii] = NULL;
		gque_pSockRpi[ii] = NULL;
		gcls_pCull[ii] = NULL;
	}
	
	for (int ii=0; ii<NUM_MEN; ii++) {
		m_nNowScode[ii] = MAX_SCODE;
	}
	for(int ii=0; ii<HT_CONNECT_NUM; ii++) {
		gcls_pSock[ii]  = NULL;
	}
	memset(&m_pInfo, 0x00, sizeof(m_pInfo));
	memset(&m_typMergedInfo, 0x00, sizeof(m_typMergedInfo));

	// シグナル生成
	m_evThStart = CreateEvent(NULL, FALSE, FALSE, NULL);				// メインスレッド同期用
	m_evHelcy	= CreateWaitableTimer(NULL, FALSE, NULL);
	m_evGcpCheck = CreateWaitableTimer(NULL, FALSE, NULL);
}

//------------------------------------------
// デストラクタ
//------------------------------------------
MainInctance::~MainInctance(void)
{
	// シグナル開放
	CloseHandle(m_evThStart);
	CloseHandle(m_evHelcy);
	CloseHandle(m_evGcpCheck);
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

	// ステータス制御クラス
	gcls_Status.SetLogMgr(mcls_pLog);
	while( true ) {
		if( gcls_Status.GetParam_CamPairInf() ) break;
		LOG(em_MSG), "[%s] ステータスクラス パラメータ読込失敗(DB接続異常？！)。リトライ中・・・", my_sThreadName);
		Sleep(5000);
	}

	// パラメータクラス
	gcls_PrmMgr.SetLogMgr(mcls_pLog); 
	while( true ) {
		if( gcls_PrmMgr.ReadParam_All() ) break;
		LOG(em_MSG), "[%s] パラメータクラス パラメータ読込失敗(DB接続異常？！)。リトライ中・・・", my_sThreadName);
		Sleep(5000);
	}

	////////////////////////////////////
	//// 全長画像関係
#ifdef FRAME_RAW_SEND
	for (int ii=0; ii<FRAME_HT_SEND_PORT; ii++) {
		// バッファ確保
		gque_pSockRpi[ii] = new ThreadLoopQueue<HT_RPI_FDATA>(MAX_BUF_QUE_RPISOCK);
		int n[1] = { SIZE_FRAME_IMG_X * SIZE_FRAME_IMG_Y * MAX_IMAGE_CNT };
		QueueAlloc(ii, MAX_BUF_QUE_RPISOCK, n);
		// 送受信スレッド起動
		gcls_pSockRpi[ii] = new SockOneToAnySend();
		gcls_pSockRpi[ii]->SetLogMgr(mcls_pLog);
		gcls_pSockRpi[ii]->SetTLQue((ThreadLoopQueue<SOCK_F_HEAD2>*) gque_pSockRpi[ii]);
		////gcls_pSockRpi[ii]->StartLock(THREAD_PRIORITY_TIME_CRITICAL);

		// 間引きクラス
		gcls_pCull[ii] = new CullManager();
		gcls_pCull[ii]->SetLogMgr(mcls_pLog);
		gcls_pCull[ii]->SetLogMgrCull(mcls_pLogCull);
		gcls_pCull[ii]->SetParamMgr(&gcls_PrmMgr);
		gcls_pCull[ii]->SetRpiToSend(gcls_pSockRpi[ii]);
		gcls_pCull[ii]->SetQueSockRpi(gque_pSockRpi[ii]);
		////gcls_pCull[ii]->StartLock();

	}
#endif






	////////////////////////////////////
	//// ワーカースレッド 初期化

	// コマンド送信用クラス
	gcls_pCom = new CommandSender(EPC_PIPE_COMMAND);
	gcls_pCom->SetLogMgr(mcls_pLog);

	// データ受信用クラス
	gcls_pData = new DataRecver(EPC_PIPE_DATA, EPC_SMEM_DATA);
	gcls_pData->SetLogMgr(mcls_pLog);
	
	// 判定⇒統括間通信 クラス
	for(int ii=0; ii<HT_CONNECT_NUM; ii++) {	
		gcls_pSock[ii] = new KizuToSend();
		gcls_pSock[ii]->SetLogMgr(mcls_pLog);
	}

	// Newデータ管理クラス
	gcls_pSend = new DataSender();
	gcls_pSend->SetLogMgr(mcls_pLog);
	gcls_pSend->SetParamMgr(&gcls_PrmMgr);
	gcls_pSend->SetStatusMgr(&gcls_Status);
	gcls_pSend->SetDataRecver(gcls_pData);
	gcls_pSend->SetKizuToSend(gcls_pSock);
	gcls_pSend->SetCullMgr(gcls_pCull);


	// ロジックファイル有無チェッククラス
	gcls_Logic.SetLogMgr(mcls_pLog);


	////////////////////////////////////
	//// ワーカースレッド 起動
	gcls_PrmMgr.StartLock();
	gcls_pCom->StartLock();
	gcls_pData->StartLock();
	for(int ii=0; ii<HT_CONNECT_NUM; ii++) gcls_pSock[ii]->Start();
	//gcls_pSend->StartLock();
	gcls_pSend->StartLock(THREAD_PRIORITY_ABOVE_NORMAL);
	//gcls_pSend->StartLock(THREAD_PRIORITY_TIME_CRITICAL);		// エンジン処理遅れが発生する
	gcls_Logic.StartLock();

	// ロジックファイル状態表示
	gcls_Logic.Check(0);


	////////////////////////////////////
	//// 初期状態

	int pcid = getpcid();
	gcls_Status.SetMainState(StatusManager::STATE_INIT);
	KizuFunction::SetStatus("HT_CANNOT_%d_ID", pcid, 0);
	Send_ToDefect_State();			// 統括通知
	return TRUE;
}

//------------------------------------------
// 終了処理 (一度のみ起動)
//------------------------------------------
void MainInctance::Exit()
{
	//// ワーカースレッド 停止
	gcls_pData->Stop();
	gcls_pSend->Stop();
	gcls_pCom->Stop();
	for(int ii=0; ii<HT_CONNECT_NUM; ii++) gcls_pSock[ii]->Stop();
	gcls_Logic.Stop();
	gcls_PrmMgr.Stop();


	//// ワーカースレッド 開放
	delete gcls_pCom;
	delete gcls_pData;
	for(int ii=0; ii<HT_CONNECT_NUM; ii++) delete gcls_pSock[ii];
	delete gcls_pSend;

	////////////////////////////////////
	// 全長画像関係
#ifdef FRAME_RAW_SEND
	QueueFree();

	for (int ii=0; ii<FRAME_HT_SEND_PORT; ii++) {
		if( gcls_pSockRpi[ii]->IsThreadEvOn() ) gcls_pSockRpi[ii]->Stop();
		if( gcls_pCull[ii]->IsThreadEvOn()) gcls_pCull[ii]->Stop();

		delete gque_pSockRpi[ii];
		delete gcls_pSockRpi[ii];
		delete gcls_pCull[ii];
	}
#endif
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
	//HANDLE hArray[] = { 
	//					mque_DeliMail.g_evSem,
	//					gcls_pSock[0]->GetEvClose(),						// 片面(通常)
	//					gcls_pCom->GetEvOnError(),
	//					gcls_pData->GetEvOnError()
	//					};
	//SetEventNum(sizeof(hArray)/sizeof(HANDLE), hArray);

	HANDLE h[8];
	//// イベント設定
	AddEventNum(1, &mque_DeliMail.g_evSem);
	AddEventNum(1, &gcls_pCom->gque_CPipeErr.g_evSem);
	AddEventNum(1, &gcls_pData->gque_CPipeErr.g_evSem);
	AddEventNum(1, &m_evHelcy);		// 定周期送信タイマー
	AddEventNum(1, &m_evGcpCheck);	// カメラGCP結果チェックタイマー

	// 全長
#ifdef FRAME_RAW_SEND
	for (int ii=0; ii<FRAME_HT_SEND_PORT; ii++) h[ii] = gcls_pSockRpi[ii]->GetEvConnect();	
	AddEventNum(FRAME_HT_SEND_PORT, h);		// 全長接続
#endif


	for (int ii = 0; ii<HT_CONNECT_NUM; ii++) {
		h[0] = gcls_pSock[ii]->GetEvClose();
		AddEventNum(1, h);
	}
	return 0;
}

//------------------------------------------
// スレッド終了前処理 (終了シグナル後にコールバック)
//------------------------------------------
int MainInctance::ThreadLast()
{
	//// もし 探傷中なら 停止
	if(gcls_Status.IsRunning()) {
		LOG(em_ERR), "[%s] 検査中に いきなりメインスレッド終了要求", my_sThreadName);
		if (DIV_HANTEI_PR != m_emKindHt)
		{
			DetectStop(0, 0);
		}
		else
		{
			ResumeStop(0, 0);
		}
	}
	//// 画像処理ソフト終了
	if( DIV_UNTEN_CAMERA == gcls_Status.GetUnten() )	DisposeProcess_iPortViewer();
	else												DisposeProcess_DetectProc();

	////　外形算出プロセス
	DisposeProcess_RectProc();
	return 0;
}

//------------------------------------------
// 実行可能状態かチェック
// int nMen		対象面 （0：表  1：裏  2：両面）
// int nKind	判定機能 （-1：全機能　1：単発機能  2：周期機能  3：パララン機能）
// 戻り値  true:実行可能	false:対象外
//------------------------------------------
bool MainInctance::DoSeqenc(int nMen, int nKind )
{
	if( 0 == m_nCamPair) return false;									// バックアップ機なら無視
	if( DIV_UNTEN_CAMERA == gcls_Status.GetUnten() ) return false;		// カメラ調整時
	if( ! gcls_Status.IsRunning() ) return false;						// 停止時は、無視

	// 面
	if( 2 != nMen && m_nMen != nMen ) return false;

	// 機能
	if( -1 != nKind && m_emKindHt != nKind ) return false;
	return true;
}


//------------------------------------------
// スレッドイベント発生
// int nEventNo イベントNo (0オリジン)
//------------------------------------------
void MainInctance::ThreadEvent(int nEventNo)
{
	DELIVERY_MAIL* pMail;
	int retc;

	////// シグナルの条件
	enum {	EV_MAIL = 0,					// メールスロット 通知 (メールは一番最後にしようかな)
			EV_CLASS_COMMAND_ERR,			// コマンドパイプクラス エラー
			EV_CLASS_DATA_ERR,				// データパイプクラス エラー
			EV_ENG_HELCY_STAT,				// 定周期状態
			EV_CAM_GCP_CHECK,				// カメラGCP結果チェック

#ifdef FRAME_RAW_SEND
			EV_RPI_SOCK_CONECT,				// 対全長 接続 (FRAME_HT_SEND_PORT分)

			EV_TO_SOCK_CLOSE=EV_RPI_SOCK_CONECT+FRAME_HT_SEND_PORT		// 対統括用の回線が切断		(HT_CONNECT_NUM分)
#else
			EV_TO_SOCK_CLOSE				// 対統括用の回線が切断		(HT_CONNECT_NUM分)
#endif
	};

	////// シグナル条件分け
	switch (nEventNo) {

//-----------------------------------------------------------------------------------------------
	case EV_MAIL: 						// メールスロット 通知
	{
		pMail = mque_DeliMail.GetFromHead();
		switch(pMail->emNo) {

//-----------------------------------------------------------
		case E_DEF_ML_PARAM :									// パラメータ変更要求 
			// L[0]  実行フレームNo (0：即時)
			// L[1]  判定PC区分 （0：表  1：裏  2：両面）
			// L[2]  検査表面状態No （1～：No   99：全部）
			// L[3]  判定機能 （-1：全機能  1：単発機能  2：周期機能  3：パララン）
			/***
			if( 0 == m_nCamPair) break;									// バックアップ機なら無視
			if( DIV_UNTEN_CAMERA == gcls_Status.GetUnten() ) break;		// カメラ調整時
			if( ! gcls_Status.IsRunning() ) break;						// 停止時は、無視

			if( DIV_HANTEI_HT == m_emKindHt ) {							// 自分は現在 判定機能
				if( DIV_HANTEI_HT  != pMail->que.fl.data[3] && 
					DIV_HANTEI_NON != pMail->que.fl.data[3] ) break;
				//if( 2 != pMail->que.fl.data[1] &&						// 両面対象、もしくは、該当面でない場合は無視
				//	((m_nCamPair-1)/(NUM_CAMPAIR/NUM_MEN)) != pMail->que.fl.data[1] ) break; 
				if (2 != pMail->que.fl.data[1] &&						// 両面対象、もしくは、該当面でない場合は無視
					m_nMen != pMail->que.fl.data[1]) break;


			} else {													// 自分は現在 周期機能
				if( DIV_HANTEI_CY  != pMail->que.fl.data[3] && 
					DIV_HANTEI_NON != pMail->que.fl.data[3] ) break;
				//if( 2 != pMail->que.fl.data[1] &&						// 両面対象、もしくは、該当面でない場合は無視
				//	((m_nCamPair-1)/(NUM_CAMPAIR_CYCLE/NUM_MEN)) != pMail->que.fl.data[1] ) break;  
				if (2 != pMail->que.fl.data[1] &&						// 両面対象、もしくは、該当面でない場合は無視
					m_nMen != pMail->que.fl.data[1]) break;

			}
			***/

			if( ! DoSeqenc(pMail->que.fl.data[1], pMail->que.fl.data[3]) ) break;
			retc = SendCommand_Param( pMail->que.fl.data[0], pMail->que.fl.data[2]);		
			CommandAnswerProc(retc, APP_PARAM_PARAM);	// アンサーにより処理分け
			break;	


//-----------------------------------------------------------
		case E_DEF_ML_START :									// 検査開始要求
			// L[0]  運転モード（1：通常  2：テスト[ｻﾝﾌﾟﾙ撮影]  3：校正[ｶﾒﾗ調整]）
			// L[1]  表面 検査表面状態No（1～）
			// L[2]  裏面 検査表面状態No（1～）
			// L[3]  表面 初期距離[mm]
			// L[4]  裏面 初期距離[mm]
			// L[5 + 2 ×（判定ID-1） + 0]  PCID順に カメラペアIDをセット（判定用、周期用）（1～：オンライン   0：ﾊﾞｯｸｱｯﾌﾟ or 対象外)
			// L[5 + 2 ×（判定ID-1） + 1]  PCID順に 該当する機能をセット  判定機能 （1：単発機能  2：周期機能  0：ﾊﾞｯｸｱｯﾌﾟ  3：パララン）
			if (true)
			{
				int				pcid = (getpcid() - 1);
				EM_DIV_HANTEI	emKind = (EM_DIV_HANTEI)pMail->que.fl.data[5 + 2 * pcid + 1];

				// 統括へのデータ送信状態セット
				bool bFrameSend = DIV_HANTEI_PR != emKind;
				gcls_pSend->SetFrameSend(bFrameSend);
					
				if (DIV_HANTEI_PR != emKind)
				{
					DetectStart(&pMail->que);
				}
				else
				{
					int	nScode[2];
					nScode[0] = pMail->que.fl.data[1];					// 検査表面状態 表
					nScode[1] = pMail->que.fl.data[2];					// 検査表面状態 裏
					int	pcid = (getpcid() - 1);							// PCID(PCKINDではない) (0オリジン)
					int	campair = getpcid() - ((NUM_HANTEI * NUM_MEN) + NUM_ALL_HTBACK);	// カメラペア 1～

					LOG(em_MSG), "[%s] 検査開始要求。(s=%d:%d campair=%d kind=%d[1:判定 2:周期 3:パララン] データ送信:%s)", my_sThreadName, nScode[0], nScode[1], campair, emKind, (gcls_pSend->IsFrameSend() ? "あり" : "なし"));

					// 統括へ結果を送信
					int			mailRetc;
					COMMON_QUE	sque;
					sque.nEventNo = FACT_TO_DEFECT_06;				// 判定開始アンサー
					sque.nLineNo = getlineid();

					sque.fl.data[0] = campair;						// カメラペア=PcKind
					sque.fl.data[1] = getpcid();					// PcId
					sque.fl.data[2] = 0;							// 結果
					sque.fl.data[3] = (int)emKind;					// 担当判定
					mailRetc = send_mail(TO_DEFECT, TOKATU, &sque);
					if (0 != mailRetc) syslog(SYSNO_MAIL_SNED_ERR, "[検査開始アンサー err_code=%d]", mailRetc);
				}
			}
			break;	

//-----------------------------------------------------------
		case E_DEF_ML_STOP :									// 検査停止要求
			if( 0 == m_nCamPair) break;									// バックアップ機なら無視

			if (DIV_HANTEI_PR != m_emKindHt)
			{
				DetectStop(0, 0);
			}
			else
			{
				int			pcid = (getpcid() - 1);
				int			cond = pMail->que.fl.data[pcid];

				// 現ステータス確認
				if (StatusManager::STATE_NON == gcls_Status.GetMainState() ||
					StatusManager::STATE_INIT == gcls_Status.GetMainState() ||
					StatusManager::STATE_CANNOT == gcls_Status.GetMainState()) {
					LOG(em_ERR), "[%s] [%s] 時に検査停止要求受信", my_sThreadName, gcls_Status.GetMainStateName() );
					return;
				}

				LOG(em_MSG), "[%s] 検査停止要求。(%s) ==============>>>", my_sThreadName, "正常停止要求");
				LOG(em_MSG), "[%s] -->途中停止処理を呼び出し", my_sThreadName);

				ResumeStop(0, 0);

				int			mailRetc;
				COMMON_QUE	sque;
				sque.nEventNo = FACT_TO_DEFECT_07;				// 判定停止アンサー
				sque.nLineNo = getlineid();
				sque.fl.data[0] = m_nCamPair;					// カメラペア=PcKind
				sque.fl.data[1] = getpcid();					// PcId
				sque.fl.data[2] = 0;							// 結果
				sque.fl.data[3] = 0;							// 区分 (0:要求に対するアンサー 1:異常発生時)
				sque.fl.data[4] = m_emKindHt;

				mailRetc = send_mail(TO_DEFECT, TOKATU, &sque);
				if (0 != mailRetc) syslog(SYSNO_MAIL_SNED_ERR, "[検査停止アンサー err_code=%d]", mailRetc);
			}
			break;	

//-----------------------------------------------------------
		case E_DEF_ML_COILINF :									// 上位設定情報(分割版)
			//if( 0 == m_nCamPair) break;									// バックアップ機なら無視
			//if( ! gcls_Status.IsRunning() ) break;						// 停止時は、無視
			//if( DIV_UNTEN_CAMERA == gcls_Status.GetUnten() ) break;		// カメラ調整時
			if( ! DoSeqenc(2, -1) ) break;

			Send_CoilInfDiv(&pMail->que);	// メイン処理(情報を取得&全部揃ったら送信)
			break;


//-----------------------------------------------------------
		case E_DEF_ML_CANOK :									// 続行不可能解除
			SendState_PcCanOk(true);
			break;

//-----------------------------------------------------------
		case E_DEF_ML_STATUS :									// 状態問い合わせ
			Send_ToDefect_State();
			break;


//-----------------------------------------------------------
		case E_DEF_ML_CLEAR_QUEUE:								// 未実行設定のキャンセル要求
			if( ! DoSeqenc(pMail->que.fl.data[0], pMail->que.fl.data[1]) ) break;
			retc = SendCommand_ClearQueue( );
			CommandAnswerProc(retc, APP_PARAM_CLEAR_QUEUE);
			break;

//-----------------------------------------------------------
		case E_DEF_ML_SET_POSITION:								// 内部距離カウンタ設定
			if( ! DoSeqenc(pMail->que.fl.data[0], pMail->que.fl.data[1]) ) break;
			retc = SendCommand_SetPosition( (NCL_CMD_SET_POSITION*) &pMail->que.fl.data[2]);
			CommandAnswerProc(retc, APP_PARAM_SET_POSITION);
			break;

//-----------------------------------------------------------
		case E_DEF_ML_PRELOAD_PARAM:							// パラメータ事前読込
			if( ! DoSeqenc(pMail->que.fl.data[0], pMail->que.fl.data[1]) ) break;			
			retc = SendCommand_PreLoad_Param( pMail->que.fl.data[2]);
			CommandAnswerProc(retc, APP_PARAM_PRELOAD_PARAM);
			break;

//-----------------------------------------------------------
		case E_DEF_ML_RESUME_START:								// 途中開始要求
			ResumeStart(&pMail->que);
			break;

//-----------------------------------------------------------
		case E_DEF_ML_RESUME_STOP:								// 途中停止要求
			ResumeStop(0, 0);
			break;

//-----------------------------------------------------------
		}
		// 開放
		delete pMail;
		break;
	}

//-----------------------------------------------------------------------------------------------
	case EV_CLASS_COMMAND_ERR:									// コマンドパイプクラス エラー
		if(true) {
			CPIPE_ERROR *	pCPipeErr = gcls_pCom->gque_CPipeErr.GetFromHead();
			CommandSender::EM_PIPE_ERR	emErrCode = (CommandSender::EM_PIPE_ERR)pCPipeErr->nErrCode;
			long						nDetail = pCPipeErr->nDetailCode;
			
			// エラーチェック
			int							ansRetc;
			ansRetc = CheckCommandPipe_Err(emErrCode, nDetail, pCPipeErr->pData);
			if (NULL != pCPipeErr->pData) delete pCPipeErr->pData;
			delete pCPipeErr;

			// 軽故障 又は 重故障 時は 停止
			if (0 != ansRetc && gcls_Status.IsRunning())
			{
				if (DIV_HANTEI_PR != m_emKindHt)
				{
					DetectStop(1, ansRetc);
				}
				else
				{
					ResumeStop(1, ansRetc);
				}
			}
			// 重故障 = 続行不可能
			if( -1 == ansRetc) SendState_PcCanNot();
		}
		break;


//-----------------------------------------------------------------------------------------------
	case EV_CLASS_DATA_ERR:										// データパイプクラス エラー
		if(true) {
			CPIPE_ERROR *	pCPipeErr = gcls_pData->gque_CPipeErr.GetFromHead();
			DataRecver::EM_PIPE_ERR		emErrCode = (DataRecver::EM_PIPE_ERR)pCPipeErr->nErrCode;
			int	nDetail = pCPipeErr->nDetailCode;
			delete pCPipeErr;


			CString cbuf;
			switch (emErrCode) {
			case DataRecver::CPIPE_OPEN_ERR:
				cbuf = "オープンで失敗";
				break;
			case DataRecver::CPIPE_READ_ERR:
				cbuf = "読込みで失敗";
				break;
			case DataRecver::CPIPE_ERR_SMEM:
				cbuf = "共有メモリオープン失敗";
				break;
			case DataRecver::CPIPE_DISCONNECT_ERR:
				cbuf = "パイプが勝手に切断された";
				break;
			}
			LOG(em_ERR), "[%s] <データパイプエラー> %s [詳細=%d]", my_sThreadName, cbuf, nDetail);
			syslog(108, "[%s:詳細=%d]", cbuf, nDetail);

			// 起動時は 停止
			if (gcls_Status.IsRunning())
			{
				if (DIV_HANTEI_PR != m_emKindHt)
				{
					DetectStop(1, -1);
				}
				else
				{
					ResumeStop(1, -1);
				}
			}
			// 重故障 = 続行不可能
			SendState_PcCanNot();
		}
		break;

//-----------------------------------------------------------------------------------------------
	case EV_ENG_HELCY_STAT:										// 画像処理エンジンへの定周期状態
		if (gcls_pData->IsConnect())
		{
			if (!gcls_pData->IsFirstInfoExists())
			{
#ifdef DEBUG_LOG_HT_HEALTHY
				LOG(em_INF), "[%s] 画像処理エンジンに問い合わせ", my_sThreadName);
#endif
				// 共有メモリにカメラ情報が存在しない場合のみ画像処理エンジンに問い合わせを実施する
				HelcyStat();
			}
			else
			{
				ENG_STAT	st;

#ifdef DEBUG_LOG_HT_HEALTHY
				LOG(em_INF), "[%s] 共有メモリから情報取得", my_sThreadName);
#endif
				// 共有メモリにカメラ情報が存在する場合、共有メモリから情報取得する
				gcls_Status.GetEngStat(&st);
				gcls_pData->HelcyStat(&st);
				if (0 <= st.camnum && NCL_ncam_max > st.camnum)
				{
					for (DWORD ii = 0; ii < st.camnum; ii++)
					{
						LOG(em_INF), "[%s] 共有メモリのカメラ情報 [ID:%d][温度:%.3f][露光:%.3f %.3f %.3f][ゲイン:%.3f %.3f %.3f][電圧:%.3f]"
							, my_sThreadName
							, st.camid[ii]
							, st.temp[ii]
							, st.exptime[0][ii]
							, st.exptime[1][ii]
							, st.exptime[2][ii]
							, st.gain[0][ii]
							, st.gain[1][ii]
							, st.gain[2][ii]
							, st.volt[ii]
							);
					}
				}
				else
				{
					LOG(em_INF), "[%s] 共有メモリのカメラ数異常:%d", my_sThreadName, st.camnum);
				}
				gcls_Status.SetEngStat(&st);
			}

#ifdef DEBUG_LOG_HT_HEALTHY
			{
				ENG_STAT	st;
				gcls_Status.GetEngStat(&st);

				for (int ii = 0; ii < st.camnum; ii++)
				{
					LOG(em_FIL), "[%s] [ID:%d][温度:%.3f][露光:%.3f %.3f %.3f][ゲイン:%.3f %.3f %.3f][電圧:%.3f]"
						, my_sThreadName
						, st.camid[ii]
						, st.temp[ii]
						, st.exptime[0][ii]
						, st.exptime[1][ii]
						, st.exptime[2][ii]
						, st.gain[0][ii]
						, st.gain[1][ii]
						, st.gain[2][ii]
						, st.volt[ii]
						);
				}
			}
#endif
		}
		break;

//-----------------------------------------------------------
	case EV_CAM_GCP_CHECK:										// カメラGCP結果チェック
		{
			LOG(em_MSG), "[%s] カメラGCP結果 チェック開始 ------------------->>>>>", my_sThreadName);

			GcpCheck();

			LOG(em_MSG), "[%s] カメラGCP結果 チェック完了 <<<<<-------------------", my_sThreadName);
		}
		break;

//-----------------------------------------------------------
#ifdef FRAME_RAW_SEND
	case EV_RPI_SOCK_CONECT +0:									// 対全長 接続
		LOG(em_MSG), "[%s] 対全長1 接続完了", my_sThreadName);
		break;
	case EV_RPI_SOCK_CONECT +1:									// 対全長 接続
		LOG(em_MSG), "[%s] 対全長2 接続完了", my_sThreadName);
		break;
#endif

//-----------------------------------------------------------------------------------------------
	case EV_TO_SOCK_CLOSE + 0:									// 対統括用の回線が切断
	case EV_TO_SOCK_CLOSE + 1:									// 対統括用の回線が切断
		if( gcls_Status.IsRunning() ) {
			LOG(em_ERR), "[%s] 検査中に対統括通信回線が切断", my_sThreadName);
			syslog(111, "[検査中に強制切断]");
			if (DIV_HANTEI_PR != m_emKindHt)
			{
				DetectStop(1, 1);
			}
			else
			{
				ResumeStop(1, 1);
			}
		} else {
			LOG(em_MSG), "[%s] 検査中以外での回線切断は、正常", my_sThreadName);
		}
		break;

//-----------------------------------------------------------------------------------------------
	default :
		// ありえない！！
		LOG(em_ERR), "[%s] ThreadEvent EventNo NG=%d", my_sThreadName, nEventNo );
		return;
	}
}

// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
#pragma region // 全長

//------------------------------------------
// 全長処理送信用の回転寿司キュー確保
// int n		キュー数
// int const* p	受渡情報
//					[0] 受渡画像最大サイズ
//------------------------------------------
void MainInctance::QueueAlloc(int idx, int n, int const* p)
{	
	HT_RPI_FDATA *pD = NULL;

	//// 空の器を作成
	for (int ii = 0; ii < n; ii++) {
		pD = new HT_RPI_FDATA;
		pD->Alloc(p);
		gque_pSockRpi[idx]->AddFree(pD);			// 器を追加
	}
	
	LOG(em_MSG),"[%s] 全長処理<%d> 送信キュー%d件確保 [%dMB]", my_sThreadName, idx, n, (__int64)p[0] * n / 1024 / 1024);
	return ;
}

//------------------------------------------
// 全長処理送信用の回転寿司キュー解放
//------------------------------------------
void MainInctance::QueueFree()
{	
	HT_RPI_FDATA *pD = NULL;
	int nPoolNum = 0;
	int nFreeNum = 0;

	for (int ii=0; ii<FRAME_HT_SEND_PORT; ii++) {
		// 具有キュー
		while(true) {
			pD = gque_pSockRpi[ii]->GetPool();
			if( NULL == pD) break;
			pD->Free();
			delete pD;
			pD = NULL;
			nPoolNum ++;
		}
		// 空きキュー
		while(true) {
			pD = gque_pSockRpi[ii]->GetFree();
			if( NULL == pD) break;
			pD->Free();
			delete pD;
			pD = NULL;
			nFreeNum ++;
		}
	}
	LOG(em_MSG),"[%s] 全長処理 送信キュー解放 [PoolQue=%d; FreeQue=%d]", my_sThreadName, nPoolNum, nFreeNum );
}

#pragma endregion


// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
#pragma region // プロセス制御


//------------------------------------------
// 外形算出プロセス 起動
// 戻り値 true:起動成功  false:起動失敗
//------------------------------------------
bool MainInctance::ExecuteProcess_RectProc()
{
	CString				sWk;

	//=======================================
	// 起動対象PCを選定
	sWk.Format("EXEC_PCKIND_%d", m_nMen);
	int nKind = GetPrivateProfileInt(HT_DEFECT, sWk, 0, TASKINI_NAME);
	if( 0 == m_nCamPair || nKind != m_nCamPair ) {
		// 自分と違うので、前回もし起動していたのであれば、終了
		DisposeProcess_RectProc();

		if( 0 != m_nCamPair) {
			Sleep(1000);	// rectが動くPCでの、rect起動をちょっと待つ
		}
		return true;
	}


	//=======================================
	// 起動チェック

	// ホントに起動中？ 途中で異常終了していないか？
	//if( m_bRectProc ) return true;
	if( m_bRectProc ) {
		if( WAIT_TIMEOUT == WaitForSingleObject( m_pInfoRect.hProcess, 300) ) { 
			return true;	// 生きている
		} else {
			LOG(em_ERR), "[%s] 外形算出ソフト が異常終了していた可能性有り。再立上", my_sThreadName);
			m_bRectProc = false;
		}
	}



	//=======================================
	// 起動するプロセス名称を取得
	char cPath[MAX_PATH];
	GetPrivateProfileString(HT_DEFECT, "RECT_EXE", "", cPath, MAX_PATH, TASKINI_NAME);

	char cCmd[MAX_PATH];					// 引数
	memset(cCmd, 0x00, sizeof(cCmd));

	if( 0==m_nMen)  { sprintf(cCmd, "%s", cPath);}		// 裏のみ引数1
	else			{ sprintf(cCmd, "%s %d", cPath, m_nMen); }
	sWk.Format("RECT_%d_ID", m_nMen);
	int nID = GetPrivateProfileInt(HT_DEFECT, sWk, 0, TASKINI_NAME);
	

	//=======================================
	// 起動
	// STARTUPINFOの内容を取得
	STARTUPINFO si;
	GetStartupInfo(&si);	
	si.dwFlags = 0;

	// 起動
	memset(&m_pInfoRect, 0x00, sizeof(m_pInfoRect));
	BOOL bRetc = CreateProcess( NULL, cCmd, NULL, NULL, FALSE, NORMAL_PRIORITY_CLASS, NULL, NULL, &si, &m_pInfoRect );		// NULL:表  1:裏
	if( ! bRetc) {					// 即時失敗
		LOG(em_ERR), "[%s] 外形算出ソフト 即時失敗 [err_code = %d]", my_sThreadName, GetLastError());
		syslog(100, "[外形算出ソフト err_code = %d]", GetLastError() );

		KizuFunction::SetStatus(nID, false, false );
		return false;
	}

	// 起動のチェック
	DWORD	nExitCode;
	GetExitCodeProcess(m_pInfoRect.hProcess, &nExitCode);
	if( nExitCode != STILL_ACTIVE ) {
		// 終了しちゃってる
		switch( nExitCode ) {
		case NCL_PROCESS_NOERR:
			LOG(em_ERR), "[%s] 画像処理ソフト 起動失敗 [NCL_PROCESS_NOERR]", my_sThreadName);
			syslog(101, "[NCL_PROCESS_NOERR]");
			break;
		case NCL_PROCESS_ERR_STARTUP:
			LOG(em_ERR), "[%s] 画像処理ソフト 起動失敗 [NCL_PROCESS_ERR_STARTUP]", my_sThreadName);
			syslog(101, "[NCL_PROCESS_ERR_STARTUP]");
			break;
		case NCL_PROCESS_ERR_UNKNOWN:
			LOG(em_ERR), "[%s] 画像処理ソフト 起動失敗 [NP_CLROCESS_ERR_UNKNOWN]", my_sThreadName);
			syslog(101, "[NCL_PROCESS_ERR_UNKNOWN]");
			break;
		default:
			LOG(em_ERR), "[%s] 画像処理ソフト 起動失敗 [不明:%d]", my_sThreadName, nExitCode);
			syslog(101, "[不明:%d]", nExitCode);
			break;
		}
		KizuFunction::SetStatus(nID, false, false );
		return false;
	}

	KizuFunction::SetStatus(nID, true, false );

	LOG(em_MSG), "[%s] 外形算出ソフト 起動完了 [%s]", my_sThreadName, cPath);
	m_bRectProc = true;
	return true;
}


//------------------------------------------
// 外形算出プロセス 終了
// 戻り値 true:正常終了  false:正常終了じゃない
//------------------------------------------
bool MainInctance::DisposeProcess_RectProc()
{
	if( !m_bRectProc ) return true;


	//// 外形算出タスク 終了
	TerminateProcess(m_pInfoRect.hProcess, -1);

	// たち下がるまでちょっと待ち
	Sleep(PROCESS_PIPE_WAIT_TIME);
	LOG(em_MSG), "[%s] 外形算出ソフト 終了", my_sThreadName);
	m_bRectProc		= false;

	CString sWk;
	sWk.Format("RECT_%d_ID", m_nMen);
	int nID = GetPrivateProfileInt(HT_DEFECT, sWk, 0, TASKINI_NAME);
	KizuFunction::SetStatus(nID, true, false );

	return true;
}



//------------------------------------------
// EPC画像処理ソフト起動 手続き
// 戻り値 true:起動成功  false:起動失敗
//------------------------------------------
bool MainInctance::ExecuteProcess_DetectProc()
{
	//=======================================
	// 起動チェック
	if( m_emKindHt == DIV_HANTEI_HT ) {
		if( m_bDetectProc ) return true;
	} else if( m_emKindHt == DIV_HANTEI_PR ) {
		if( m_bDetectProc_pr ) return true;
	} else {
		if( m_bDetectProc_pd ) return true;
	}


	//=======================================
	// 起動するプロセス名称を取得
	char cPath[MAX_PATH];
	if( m_emKindHt == DIV_HANTEI_HT ) {
		GetPrivateProfileString(HT_DEFECT, "DETECT_EXE", "", cPath, MAX_PATH, TASKINI_NAME);
	} else if (m_emKindHt == DIV_HANTEI_PR) {
		GetPrivateProfileString(HT_DEFECT, "DETECT_PR_EXE", "", cPath, MAX_PATH, TASKINI_NAME);
	} else {
		GetPrivateProfileString(HT_DEFECT, "DETECT_PD_EXE", "", cPath, MAX_PATH, TASKINI_NAME);
	}
	_ASSERT(strlen(cPath));


	//=======================================
	// 探傷プロセスが起動されていたら、殺してから実施
	if( m_emKindHt == DIV_HANTEI_HT ) {
		if( m_bDetectProc )		DisposeProcess_DetectProc();
	} else if (m_emKindHt == DIV_HANTEI_PR) {
		if( m_bDetectProc_pr )	DisposeProcess_DetectProc();
	} else {
		if( m_bDetectProc_pd )	DisposeProcess_DetectProc();
	}
	if( m_bAdjustProc )	DisposeProcess_iPortViewer();


	//=======================================
	// 画像処理エンジンデバック可能チェック
	CWnd*   cwnd		= ((CHT_DEFECTApp*)AfxGetApp())->GetMainWnd();	// 使いやすいようにエイリアス
	bool bDebugNC		= false;										// 画像処理エンジンデバック許可時 true
	if( cwnd->IsDlgButtonChecked(IDC_DEBUG_MANUAL ) ) {
		bDebugNC = !! cwnd->IsDlgButtonChecked(IDC_DEBUG_NC);
	}

	//=======================================
	// 起動
	if( ! bDebugNC ) {
		// STARTUPINFOの内容を取得
		STARTUPINFO si;
		GetStartupInfo(&si);	
		si.dwFlags = 0;

		// 起動
		memset(&m_pInfo, 0x00, sizeof(m_pInfo));
		BOOL bRetc = CreateProcess( cPath, "", NULL, NULL, FALSE, NORMAL_PRIORITY_CLASS, NULL, NULL, &si, &m_pInfo );
		if( ! bRetc) {					// 即時失敗
			LOG(em_ERR), "[%s] 画像処理ソフト 即時失敗 [err_code = %d]", my_sThreadName, GetLastError());
			syslog(100, "[画像処理ソフト err_code = %d]", GetLastError() );
			return false;
		}

		// 立ち上がるまでちょっと待ち
		Sleep( PROCESS_INIT_WAIT_TIME );

		// 起動のチェック
		DWORD	nExitCode;
		GetExitCodeProcess(m_pInfo.hProcess, &nExitCode);
		if( nExitCode != STILL_ACTIVE ) {
			// 終了しちゃってる
			switch( nExitCode ) {
			case NCL_PROCESS_NOERR:
				LOG(em_ERR), "[%s] 画像処理ソフト 起動失敗 [NCL_PROCESS_NOERR]", my_sThreadName);
				syslog(101, "[NCL_PROCESS_NOERR]");
				break;
			case NCL_PROCESS_ERR_STARTUP:
				LOG(em_ERR), "[%s] 画像処理ソフト 起動失敗 [NCL_PROCESS_ERR_STARTUP]", my_sThreadName);
				syslog(101, "[NCL_PROCESS_ERR_STARTUP]");
				break;
			case NCL_PROCESS_ERR_UNKNOWN:
				LOG(em_ERR), "[%s] 画像処理ソフト 起動失敗 [NP_CLROCESS_ERR_UNKNOWN]", my_sThreadName);
				syslog(101, "[NCL_PROCESS_ERR_UNKNOWN]");
				break;
			default:
				LOG(em_ERR), "[%s] 画像処理ソフト 起動失敗 [不明:%d]", my_sThreadName, nExitCode);
				syslog(101, "[不明:%d]", nExitCode);
				break;
			}
			return false;
		}
	} else {
		// デバッカから起動を受付
		LOG(em_MSG), "[%s] 今から3秒間 手動の画像処理ソフト 起動 を待ちます", my_sThreadName, cPath);
		Sleep(3000);
	}


	LOG(em_MSG), "[%s] 画像処理ソフト 起動完了 [%s]", my_sThreadName, cPath);
	if( m_emKindHt == DIV_HANTEI_HT ) 		m_bDetectProc	= true;
	else if( m_emKindHt == DIV_HANTEI_PR )	m_bDetectProc_pr= true;
	else							  		m_bDetectProc_pd= true;



	//=======================================
	//// 画像処理タスク間 通信をすべて確立
	// 準備
	gcls_pCom->SetPipeName(EPC_PIPE_COMMAND);
	gcls_pData->SetPipeName(EPC_PIPE_DATA);
	gcls_pData->SetSmemName(EPC_SMEM_DATA);

	// 接続要求
	gcls_pCom->SetEvPipeOpen();
	gcls_pData->SetEvPipeOpen();


	//=======================================
	//// 接続確認

	// ちょっと待ち
	for(int ii=0; ii<5; ii++) {
		if( gcls_pCom->IsConnect() && gcls_pData->IsConnect() ) break;
		Sleep( 1100 );
	}

	// とりあえず簡易的だが、接続確立したか確認
	if( ! gcls_pCom->IsConnect() ) {
		LOG(em_ERR), "[%s] 画像処理ソフト 起動失敗 [コマンドパイプ:未接続]", my_sThreadName);
		syslog(101, "[コマンドパイプ:未接続]");
		return false;
	}
	if( ! gcls_pData->IsConnect() ) {
		LOG(em_ERR), "[%s] 画像処理ソフト 起動失敗 [データパイプ:未接続]", my_sThreadName);
		syslog(101, "[データパイプ:未接続]");
		return false;
	}
	LOG(em_MSG), "[%s] 画像処理ソフト間 通信確立完了", my_sThreadName);
	return true;
}


//------------------------------------------
// カメラ調整ソフト起動
// 戻り値 true:起動成功  false:起動失敗
//------------------------------------------
bool MainInctance::ExecuteProcess_iPortViewer()
{
	//=======================================
	// 起動チェック
	if( m_bAdjustProc ) return true;

	//=======================================
	// 起動するプロセス名称を取得
	char cPath[MAX_PATH];
	char cCPath[MAX_PATH];
	if (m_emKindHt == DIV_HANTEI_HT) {
		GetPrivateProfileString(HT_DEFECT, "IPORTVIEWER_EXE", "", cPath, MAX_PATH, TASKINI_NAME);
		GetPrivateProfileString(HT_DEFECT, "IPORTVIEWER_PATH", "", cCPath, MAX_PATH, TASKINI_NAME);
	}
	else if (m_emKindHt == DIV_HANTEI_PR) {
		GetPrivateProfileString(HT_DEFECT, "IPORTVIEWER_PR_EXE", "", cPath, MAX_PATH, TASKINI_NAME);
		GetPrivateProfileString(HT_DEFECT, "IPORTVIEWER_PR_PATH", "", cCPath, MAX_PATH, TASKINI_NAME);
	}
	else {
		GetPrivateProfileString(HT_DEFECT, "IPORTVIEWER_PR_EXE", "", cPath, MAX_PATH, TASKINI_NAME);
		GetPrivateProfileString(HT_DEFECT, "IPORTVIEWER_PR_PATH", "", cCPath, MAX_PATH, TASKINI_NAME);
	}
	_ASSERT(strlen(cPath));


	//=======================================
	// 探傷プロセスが起動されていたら、殺してから実施
	if( m_bDetectProc || m_bDetectProc_pd || m_bDetectProc_pr ) DisposeProcess_DetectProc();
	if( m_bAdjustProc )						DisposeProcess_iPortViewer();


	//=======================================
	// 起動

	// STARTUPINFOの内容を取得
	STARTUPINFO si;
	GetStartupInfo(&si);	
	si.dwFlags = 0;

	// 起動
	//BOOL bRetc = CreateProcess( cPath, "", NULL, NULL, FALSE, NORMAL_PRIORITY_CLASS, NULL, NULL, &si, &m_pInfo );
	BOOL bRetc = CreateProcess( cPath, "", NULL, NULL, FALSE, NORMAL_PRIORITY_CLASS, NULL, cCPath, &si, &m_pInfo );
	if( ! bRetc) {					// 即時失敗
		LOG(em_ERR), "[%s] カメラ調整ソフト 即時失敗 [err_code = %d]", my_sThreadName, GetLastError());
		syslog(100, "[カメラ調整ソフト: err_code = %d]", GetLastError() );
		return false;
	}

	// 立ち上がるまでちょっと待ち
	Sleep( PROCESS_INIT_WAIT_TIME );
	m_bAdjustProc	 = true;

	LOG(em_MSG), "[%s] カメラ調整ソフト 起動完了 [%s]", my_sThreadName, cPath);
	return true;
}


//------------------------------------------
// EPC画像処理ソフト終了
// 戻り値 true:正常終了  false:正常終了じゃない
//------------------------------------------
bool MainInctance::DisposeProcess_DetectProc()
{
	if( !m_bDetectProc && !m_bDetectProc_pd && !m_bDetectProc_pr ) return true;

	//// 検査中の場合は、ストップを行う。
	if( gcls_Status.IsRunning() ) {
		if (DIV_HANTEI_PR != m_emKindHt)
		{
			DetectStop(1, 0);
		}
		else
		{
			ResumeStop(1, 0);
		}
	}

	//// 切断してもエラーが出ないように
	gcls_pCom->SetRunExit();
	gcls_pData->SetRunExit();

	//// 画像処理タスク 終了
	SendCommand_Exit();								// 終了コマンド送信

	//// プロセスの終了を最大５秒間待ってみる
	if( WAIT_TIMEOUT == WaitForSingleObject( m_pInfo.hProcess, PROCESS_EXIT_WAIT_TIME) ) { 
		LOG(em_ERR), "[%s] 画像処理ソフト 終了失敗", my_sThreadName);
		// まだ生きているなら強制終了
		TerminateProcess(m_pInfo.hProcess, -1);
	}
	// たち下がるまでちょっと待ち
	Sleep(PROCESS_PIPE_WAIT_TIME);
	LOG(em_MSG), "[%s] 画像処理ソフト 終了", my_sThreadName);
	m_bDetectProc		= false;
	m_bDetectProc_pd	= false;
	m_bDetectProc_pr	= false;

	//// 画像処理タスク間 通信をすべて切断
	gcls_pCom->SetEvPipeClose();
	gcls_pData->SetEvPipeClose();
	LOG(em_MSG), "[%s] 画像処理ソフト間 通信切断完了", my_sThreadName);
	return true;
}

//------------------------------------------
// カメラ調整ソフト 終了
// 戻り値 true:正常終了  false:正常終了じゃない
//------------------------------------------
bool MainInctance::DisposeProcess_iPortViewer()
{
	if( !m_bAdjustProc ) return true;
	
	//// \mailslot\iPortViewrへメールスロット
	// 内容は何でも良いけど、とりあえず、終了コードを送る
	COMMON_QUE que;
	que.nEventNo = FACT_KS_END;
	send_mail(EPC_MAIL_IPORTVIEWER, ".", &que);

	//// プロセスの終了を最大５秒間待ってみる
	if( WAIT_TIMEOUT == WaitForSingleObject( m_pInfo.hProcess, PROCESS_EXIT_WAIT_TIME) ) { 
		LOG(em_ERR), "[%s] カメラ調整ソフト 終了失敗", my_sThreadName);
		// まだ生きているなら強制終了
		TerminateProcess(m_pInfo.hProcess, -1);
	}
	// たち下がるまでちょっと待ち
	Sleep(PROCESS_PIPE_WAIT_TIME);
	LOG(em_MSG), "[%s] カメラ調整ソフト 終了", my_sThreadName);
	m_bAdjustProc		= false;

	return true;
}
#pragma endregion

// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// シーケンス制御


//------------------------------------------
// 検査開始処理
//		※ 通常時は、資源の割り当て、パラメータ設定、スタート を実施
//		※ 校正時は、画像処理タスク終了、iPortViewer起動
// COMMON_QUE* que 起動要因
//------------------------------------------
void MainInctance::DetectStart(COMMON_QUE* que)
{
	int					nRetc = 0;											// 結果 (0:正常 -1:重故障 1:軽故障)
	CString				sWk[HT_CONNECT_NUM];

	//// 下準備
	EM_DIV_UNTEN		unten	= (EM_DIV_UNTEN) que->fl.data[0];			// 運転モード
	int					nScode[2];
						nScode[0] = que->fl.data[1];						// 検査表面状態 表
						nScode[1] = que->fl.data[2];						// 検査表面状態 裏

	int					nLen[2];
						nLen[0] = que->fl.data[3];
						nLen[1] = que->fl.data[4];

	int					pcid = (getpcid() -1);								// PCID(PCKINDではない) (0オリジン)
	int					campair	= que->fl.data[ 5 + 2*pcid + 0];			// カメラペア 1～ (0:バックアップ)   マイナスの場合異常カメラ担当
	bool				bNgCam = false;										// 故障カメラ担当か？ 故障時true
	if (0 > campair) {
		campair = campair*-1;		// 正数に戻す
		bNgCam = true;
		LOG(em_WAR), "[%s] 異常カメラ担当判定(%d)", my_sThreadName, campair );
	}

	EM_DIV_HANTEI		emKind	= (EM_DIV_HANTEI)que->fl.data[ 5 + 2*pcid + 1];	// 担当機能 (1:判定)
	KizuPcInfManager*	cls_Pc  = (KizuPcInfManager*)getcls_pcinfmgr();		// PC情報クラス
	int					nMen	= (0 == ((campair-1)/(NUM_CAMPAIR/NUM_MEN)) ? 0 : 1);											


	//// バックアップ機なら これ以降処理無し
	if( 0 == campair ) {
		m_nCamPair = 0;	
		m_nMen = -1;
		return;
	}

	//// 現ステータス確認
	if( StatusManager::STATE_INIT != gcls_Status.GetMainState() ) {
		LOG(em_ERR), "[%s] [%s] 時に検査開始要求受信", my_sThreadName, gcls_Status.GetMainStateName() );
		return ;
	}
	LOG(em_MSG), "[%s] 検査開始要求。(s=%d:%d men=%d campair=%d kind=%d[1:判定 2:周期 3:パララン] データ送信:%s)", my_sThreadName, nScode[0], nScode[1], nMen, campair, emKind, (gcls_pSend->IsFrameSend() ? "あり" : "なし"));


	//// メンバー変数に登録
	m_nAppParam = 0;	// エラーをリセット
	gcls_Status.SetUnten( unten );
	m_nMen			= nMen;
	m_nCamPair		= campair;
	if( DIV_HANTEI_HT == emKind ) {
		m_emKindHt	= DIV_HANTEI_HT;
	} else {
		m_emKindHt	= DIV_HANTEI_CY; 
	}

	m_nNowScode[0]	= nScode[nMen];

	//// 異常カメラ
	m_bNgCam = bNgCam;
	gcls_pCom->SetNgCam(bNgCam);

	//// 検査開始時に、自分がわからない場合は、もう一度DBからPC名を取得する (なんかDBの調子が悪いよ対策)
	if( m_emKindHt == DIV_HANTEI_HT) {
		if( 0 == memcmp( &"*", HANTEI( campair-1), 1) ) {
			LOG(em_ERR), "[%s] 自分の名前不定", my_sThreadName );
			getpcname();
		}
		LOG(em_MSG), "[%s] 自分のPC名は[%s]", my_sThreadName, HANTEI( campair-1));  
		for(int ii=0; ii<HT_CONNECT_NUM; ii++) gcls_pSock[ii]->SetPcName( HANTEI( campair-1) ); 

	} else {
		if( 0 == memcmp( &"*", CYCLE( campair-1), 1) ) {
			LOG(em_ERR), "[%s] 自分の名前不定", my_sThreadName );
			getpcname();
		}
		LOG(em_MSG), "[%s] 自分のPC名は[%s]", my_sThreadName, CYCLE( campair-1));  
		for(int ii=0; ii<HT_CONNECT_NUM; ii++) gcls_pSock[ii]->SetPcName( CYCLE( campair-1) ); 
	}


	
	//// パラメータ読み直し
	//gcls_PrmMgr.SetEv_PrmRead();
	gcls_PrmMgr.ReadParam_All();	//同期 読込に変更



	////////////////////////////////////
	// カメラ調整
	////////////////////////////////////
	if( DIV_UNTEN_CAMERA == unten ) {				// カメラ調整時

		// iPortViewer 起動
		if( ! ExecuteProcess_iPortViewer() ) {
			// 異常
			nRetc = -1;
			SendState_PcCanNot();
		}
		// カメラ調整時は、これでOK
		goto ending;
	}

	////////////////////////////////////
	// 通常 又は サンプル撮影 又は シミュレーション
	////////////////////////////////////

	//// 外形算出プロセス 起動
	if( ! ExecuteProcess_RectProc() ) {
		// 異常 △		
	}

	//// 画像処理ソフト 起動
	if (!m_bNgCam) {
		if (!ExecuteProcess_DetectProc()) {
			// 異常
			nRetc = -1;
			SendState_PcCanNot();
		}
	}




	//---------------------------------------
	//// 統括と回線接続を行う
	int					nNum[HT_CONNECT_NUM];
	for(int ii=0; ii<HT_CONNECT_NUM; ii++) nNum[ii] = 0;

	// iniキー決定
	// 表 裏？
	if( m_emKindHt == DIV_HANTEI_HT) {
		sWk[0].Format("%s_TO_%d", HT_DEFECT, m_nMen );
	} else {
		sWk[0].Format("%s_PD_TO_%d", HT_DEFECT, m_nMen);
	}
	// 接続数決定
	nNum[0] = gcls_Status.GetCamPairItemNum(m_nCamPair, m_emKindHt);

#ifndef SOT_TEST
	for(int ii=0; ii<HT_CONNECT_NUM; ii++) {
		// 設定
		gcls_pSock[ii]->SetSession( sWk[ii] );
		gcls_pSock[ii]->SetSendNum( nNum[ii] );

		// 非同期接続 (リトライは行わない。通常繋がらないはずはない。1回ダメならもうだめのはず。)
		gcls_pSock[ii]->SetConnect(false);		
	}


	//// 統括との回線が確立出来た？
	int cnt=0;
	bool bIsConnect = true;
	while(true) {
		cnt++;
		
		// 全部つながっているか？
		bIsConnect = true;
		for(int ii=0; ii<HT_CONNECT_NUM; ii++) {
			bIsConnect &= gcls_pSock[ii]->IsConnect();
		}

		if( bIsConnect ) {
			LOG(em_MSG), "[%s] 統括と回線接続完了！", my_sThreadName);		
			break;
		} else {
			if( 10 > cnt ) {
				LOG(em_MSG), "[%s] 統括と回線接続中・・・ %d回目", my_sThreadName, cnt);	
				Sleep(100);
			} else {
				LOG(em_MSG), "[%s] 統括と回線接続失敗", my_sThreadName);
				for(int ii=0; ii<HT_CONNECT_NUM; ii++) gcls_pSock[ii]->SetDisConnect(); 
				nRetc = -1;
				SendState_PcCanNot();
				goto ending;
			}
		}
	}
#endif

	//// 初期化コマンド
	nRetc = SendCommand_Init();
	if( 0 != nRetc ) {
		if(-1 == nRetc) SendState_PcCanNot();

		nRetc = -1;
		goto ending;
	}


	// ------------------------------------------------------------------------------
	//// 初期状態 (これ以降のエラーの場合は、資源開放の処理も必要ね)
	gcls_Status.SetMainState(StatusManager::STATE_WAIT);
	//Sleep(200);												// 画像処理タスクが初期化されるまでちょっと待ち
	// 単独起動時に初期化コマンド送信してからRPCサーバーの紐づけまでの時間が短かったため、待ち時間を10秒に変更
	Sleep(10000);												// 画像処理タスクが初期化されるまでちょっと待ち

	//// その他準備
	gcls_pSend->ClearFrameNo();
	gcls_pSend->SetCamPair(m_nCamPair);
	gcls_pSend->SetKindHt(m_emKindHt);






	//// 内部距離カウンタ
	NCL_CMD_SET_POSITION  pos;
	memset(&pos, 0x00, sizeof(pos));
	pos.cmd = NCL_SET_POSITION;
	pos.framenum = 0;
	pos.update_mask = 0xFFFFFFFF;
	pos.front_pos = (float)nLen[m_nMen];
	if( -1 == SendCommand_SetPosition(&pos)) {										// 重故障
		DetectStop(-1, 0);											// 統括への送信は無し 
		nRetc = -1;
		SendState_PcCanNot();
		goto ending;
	} 


	//// パラメータ事前読込
	if( -1 == SendCommand_PreLoad_Param( m_nNowScode[0] ) ) {
		DetectStop(-1, 0);											// 統括への送信は無し 
		nRetc = -1;
		SendState_PcCanNot();
		goto ending;
	} 
	//// 指定検査表面状態コードがMAX以外の場合、MAX_SCODEの事前読込を実施する
	//// ※検査基点WPD～検査位置間でイニシャル電文を受信した場合の対策
	//// 　MAX_SCODEを事前読込しておかないと画像エンジンがダウンする
	if (MAX_SCODE != m_nNowScode[0])
	{
		//// パラメータ事前読込(検査表面状態MAX値)
		if (-1 == SendCommand_PreLoad_Param(MAX_SCODE)) {
			DetectStop(-1, 0);										// 統括への送信は無し 
			nRetc = -1;
			SendState_PcCanNot();
			goto ending;
		}
	}

	//// パラメータ変更コマンド
	if( -1 == SendCommand_Param(0, m_nNowScode[0])) {				// 重故障
		DetectStop(-1, 0);											// 統括への送信は無し 
		nRetc = -1;
		SendState_PcCanNot();
		goto ending;
	} 


	//// 探傷開始コマンド
	if( -1 == SendCommand_Start() ) {
		DetectStop(-1, 0);										// 統括への送信は無し 
		nRetc = -1;
		SendState_PcCanNot();
		goto ending;
	}


	// ロジックファイル状態表示
	gcls_Logic.Check(0);


	//// ヘルシー問い合わせ開始
	__int64 ts = (10 * 1000) * -10000;			// 1ミリ秒→100ナノ秒 (マイナス指定で、指定時間経過でシグナル) (第3引数は2回目以降の起動間隔)
	SetWaitableTimer(m_evHelcy, (LARGE_INTEGER *)&ts, STAT_CHECK_INTERVAL, NULL, NULL, FALSE);
	gcls_pSend->StartHls();

	//// カメラGCPチェックを開始
	GcpCheck_StartTimer();

	//---------------------------------------
	//// 全長処理と接続

#ifdef FRAME_RAW_SEND
	//if (!m_bNgCam) {
	if( DIV_HANTEI_HT == m_emKindHt	) {
		int nCamset = gcls_Status.GetCamPairItemCamset(m_nCamPair, 0, m_emKindHt) ;

		// ===>> 坂巻 2022.07.20 検査停止開始後、全長画像が生成されない不具合修正		
		// キューの解放（念のため）
		for (int ii = 0; ii < FRAME_HT_SEND_PORT; ii++) {
			int num = gque_pSockRpi[ii]->GetCountPool();
			if (0 < num)
			{
				LOG(em_ERR), "[%s] 検査開始時に全長画像送信キューあり[ii=%d, num=%d]", my_sThreadName, ii, num);
				syslog(130, "[ii=%d, num=%d]", ii, num);
				QueueFree();
				break;
			}
		}
		// <<===
		
		for (int ii=0; ii<FRAME_HT_SEND_PORT; ii++) {
			gcls_pSockRpi[ii]->StartLock(THREAD_PRIORITY_TIME_CRITICAL);
			gcls_pCull[ii]->StartLock();
		}
		Sleep(100);

		for (int ii=0; ii<FRAME_HT_SEND_PORT; ii++) {
			int nId = (nCamset + ii - 1) % NUM_CAM_POS;
			sWk[0].Format("%s_RPI_TO_%d_%d", HT_DEFECT, m_nMen, nId);
			gcls_pSockRpi[ii]->SetSession(sWk[0]);
			gcls_pSockRpi[ii]->SetRetryMode(true);
			gcls_pSockRpi[ii]->SetConnect();
		}
	}
#endif



ending:

	////////////////////////////////////
	// 最後の処理
	////////////////////////////////////


	// 統括へ結果を送信
	int			mailRetc;
	COMMON_QUE	sque;
	sque.nEventNo = FACT_TO_DEFECT_06;				// 判定開始アンサー
	sque.nLineNo = getlineid();

	sque.fl.data[0] = m_nCamPair;					// カメラペア=PcKind
	sque.fl.data[1] = getpcid();					// PcId
	sque.fl.data[2] = nRetc;						// 結果
	sque.fl.data[3] = (int)emKind;					// 担当判定
	mailRetc = send_mail(TO_DEFECT, TOKATU, &sque);
	if( 0 != mailRetc ) syslog(SYSNO_MAIL_SNED_ERR, "[検査開始アンサー err_code=%d]", mailRetc);

	// 正常？異常
	if( 0 == nRetc ) {
		SendState_Grab(0, true);
		SendState_App(0);
		gcls_Status.SetMainState(StatusManager::STATE_RUN);			 // 検査中

	} else {
		SendState_Grab(0, false);
		SendState_App(APP_PARAM_EXEC);	// 実行失敗
	}
}

//------------------------------------------
// 検査停止処理
//		※ 通常時は、ストップ、資源開放 を実施
//		※ 校正時は、iPortViewer終了、画像処理タスク起動
// int mode 要求状態 (0:正常な終了要求  1:何かおかしくて強制的に終了要求 -1:送信しない)
// int ansRetc 結果 (0:正常 1:軽故障 -1:重故障) ※ 処理中に、重故障が発生した場合は、重故障になる
//------------------------------------------
void MainInctance::DetectStop(int mode, int ansRetc)
{
	////////////////////////////////////
	// 下準備
	////////////////////////////////////
	EM_DIV_UNTEN		unten	= gcls_Status.GetUnten();					// 運転モード

	// バックアップ機なら これ以降処理無し
	if( 0 == m_nCamPair ) return;


	// 現ステータス確認
	if( StatusManager::STATE_NON	== gcls_Status.GetMainState() ||
		StatusManager::STATE_INIT	== gcls_Status.GetMainState() ||
		StatusManager::STATE_CANNOT == gcls_Status.GetMainState() ) {
		LOG(em_ERR), "[%s] [%s] 時に検査停止要求受信", my_sThreadName, gcls_Status.GetMainStateName() );
		return;
	}
	LOG(em_MSG), "[%s] 検査停止要求。(%s) ==============>>>", my_sThreadName, (0 == mode ? "正常停止要求" : "異常による強制停止要求"));  


	////////////////////////////////////
	// カメラ調整
	////////////////////////////////////
	if( DIV_UNTEN_CAMERA == unten ) {				// カメラ調整時

		// iPortViewer 終了
		DisposeProcess_iPortViewer();



	////////////////////////////////////
	// 通常 又は サンプル撮影 又は シミュレーション
	////////////////////////////////////
	} else {
		//// ヘルシータイマー停止
		CancelWaitableTimer(m_evHelcy);
		gcls_pSend->StopHls();

		//// カメラGCPチェックを停止
		CancelWaitableTimer(m_evGcpCheck);

		//---------------------------------------
		// 全長処理 切断
#ifdef FRAME_RAW_SEND
		//for (int ii=0; ii<FRAME_HT_SEND_PORT; ii++) {
		//	gcls_pSockRpi[ii]->SetRetryMode(false);		// リトライはしたくない
		//	gcls_pSockRpi[ii]->SetDisConnect();
		//}

		for (int ii=0; ii<FRAME_HT_SEND_PORT; ii++) {

			if( gcls_pSockRpi[ii]->IsThreadEvOn() ) {
				gcls_pSockRpi[ii]->SetRetryMode(false);		// リトライはしたくない
				gcls_pSockRpi[ii]->SetDisConnect();
			}
		}
		Sleep(1000);		// 非同期で全長処理切断しているため、切断される前にスレッド停止されてしまうと異常終了となる。ちょっと長めに待つ
		//Sleep(300);
		for (int ii=0; ii<FRAME_HT_SEND_PORT; ii++) {		
			if( gcls_pSockRpi[ii]->IsThreadEvOn()) gcls_pSockRpi[ii]->Stop();
			if( gcls_pCull[ii]->IsThreadEvOn()) gcls_pCull[ii]->Stop();
		}
		// ===>> 坂巻 2022.07.20 検査停止開始後、全長画像が生成されない不具合修正
		// キューの解放（念のため）
		for (int ii = 0; ii < FRAME_HT_SEND_PORT; ii++) {
			int num = gque_pSockRpi[ii]->GetCountPool();
			if (0 < num)
			{
				LOG(em_ERR), "[%s] 検査停止時に全長画像送信キューあり[ii=%d, num=%d]", my_sThreadName, ii, num);
				syslog(131, "[ii=%d, num=%d]", ii, num);
				QueueFree();
				break;
			}
		}
		//// <<===

#endif

		//---------------------------------------
		// エンジン

		//// 検査中の場合は、停止
		if( StatusManager::STATE_RUN == gcls_Status.GetMainState() ) {
			// 停止コマンド
			if( 1 == SendCommand_Stop() ) {
				if(-1 != ansRetc) ansRetc = 1;
			}
			gcls_Status.SetMainState(StatusManager::STATE_WAIT);
			Sleep(200);										// 画像処理タスクが初期化されるまでちょっと待ち
		}

		//// 資源開放
		if( -1 == SendCommand_Dispose() ) {
			ansRetc = -1;
			SendState_PcCanNot();
			goto ending;
		}
	}

ending:

	////////////////////////////////////
	// 最後の処理
	////////////////////////////////////
	// 統括へ結果を送信
	if( -1 != mode ) {
		int			mailRetc;
		COMMON_QUE	sque;
		sque.nEventNo = FACT_TO_DEFECT_07;				// 判定停止アンサー
		sque.nLineNo = getlineid();
		sque.fl.data[0] = m_nCamPair;					// カメラペア=PcKind
		sque.fl.data[1] = getpcid();					// PcId
		sque.fl.data[2] = ansRetc;						// 結果
		sque.fl.data[3] = mode;							// 区分 (0:要求に対するアンサー 1:異常発生時)
		sque.fl.data[4] = m_emKindHt;

		mailRetc = send_mail(TO_DEFECT, TOKATU, &sque);
		if( 0 != mailRetc ) syslog(SYSNO_MAIL_SNED_ERR, "[検査停止アンサー err_code=%d]", mailRetc);
	}
	if(0 == ansRetc) { LOG(em_MSG), "[%s] 検査停止処理完了 <<<========================", my_sThreadName); }
	else			 { LOG(em_ERR), "[%s] 検査停止処理完了。結果=%d <<<========================", my_sThreadName, ansRetc); }

	// ステータス 初期状態
	if( -1 != ansRetc )	gcls_Status.SetMainState(StatusManager::STATE_INIT);
}



//------------------------------------------
// 状態取得
//------------------------------------------
void MainInctance::HelcyStat()
{
	NCL_ANSWER_QUERY_STATE	ans;
	ENG_STAT				st;

	memset( &ans, 0x00, sizeof(ans));
	memset( &st, 0x00, sizeof(st));


	//---------------------------
	// エンジン状態
	int		nRetc;					// 結果 (0:正常 -1:重故障 1:軽故障)

	nRetc = SendCommand_Query_State(&ans);
	if( 0 == nRetc && ( StatusManager::STATE_RUN == gcls_Status.GetMainState() ) ) {
		if(NCL_PREINIT == ans.state || NCL_TRANSITION == ans.state || NCL_INITIALIZED == ans.state ) {
			LOG(em_ERR), "[%s] [%s] 時にエンジン停止(%d)", my_sThreadName, gcls_Status.GetMainStateName(), ans.state );
			nRetc = -1;
		}
	}
	if(0 != nRetc) {
		if (DIV_HANTEI_PR != m_emKindHt)
		{
			DetectStop(-1, 0);
		}
		else
		{
			ResumeStop(-1, 0);
		}
		nRetc = -1;
		SendState_PcCanNot();
		return;
	}

	//LOG(em_INF), "[%s] エンジン状態 ENG→判定 %d num:%d id:%d %d temp:%.1f %.1f", my_sThreadName, ans.state, ans.camnum, ans.camid[0], ans.camid[1], ans.temp[0], ans.temp[1]);

	// セット
	st.camnum = ans.camnum;
	for(int ii=0; ii<(int)st.camnum; ii++) {
		st.camid[ii] = ans.camid[ii];

		for(int jj=0; jj<(MAX_CAMANGLE - 1); jj++) {
			st.exptime[jj][ii] = ans.exptime[jj][ii];
			st.gain[jj][ii] = ans.gain[jj][ii];
		}
		st.temp[ii] = ans.temp[ii];
		st.volt[ii] = ans.volt[ii];
	}
	gcls_Status.SetEngStat(&st); 
	//LOG(em_INF), "[%s] エンジン状態 判定→統括 %d num:%d id:%d %d temp:%.1f %.1f", my_sThreadName, st.state, st.camnum, st.camid[0], st.camid[1], st.temp[0], st.temp[1]);

}

//------------------------------------------
// 途中開始処理
//		※ 通常時は、資源の割り当て、パラメータ設定、スタート を実施
//		※ 校正時は、画像処理タスク終了、iPortViewer起動
// COMMON_QUE* que 起動要因
//------------------------------------------
void MainInctance::ResumeStart(COMMON_QUE* que)
{
	int					nRetc = 0;											// 結果 (0:正常 -1:重故障 1:軽故障)
	CString				sWk[HT_CONNECT_NUM];

	// L[0]  運転モード（1：通常  2：テスト[ｻﾝﾌﾟﾙ撮影]  3：校正[ｶﾒﾗ調整]）
	// L[1]  表面 検査表面状態No（1～）
	// L[2]  裏面 検査表面状態No（1～）
	// L[3]  表面 初期距離[mm]
	// L[4]  裏面 初期距離[mm]
	// L[5]  途中参加フレームNo （0：即時 ）
	// L[6 + 2 ×（判定ID-1） + 0]  PCID順に （判定用、周期用：カメラペアID）または（ロジックパララン用：カメラセット１）をセット（1～：オンライン   0：ﾊﾞｯｸｱｯﾌﾟ or 対象外)
	// L[6 + 2 ×（判定ID-1） + 1]  PCID順に （判定用、周期用：未使用）または（ロジックパララン用：カメラセット２）をセット（1～：オンライン   0：ﾊﾞｯｸｱｯﾌﾟ or 対象外)
	// L[6 + 2 ×（判定ID-1） + 2]  PCID順に 該当する機能をセット  判定機能 （1：単発機能  2：周期機能  0：ﾊﾞｯｸｱｯﾌﾟ 3：ロジックパララン）
	EM_DIV_UNTEN		unten = (EM_DIV_UNTEN)que->fl.data[0];				// 運転モード
	int					nScode[2];
	nScode[0] = que->fl.data[1];											// 検査表面状態 表
	nScode[1] = que->fl.data[2];											// 検査表面状態 裏

	int					nLen[2];
	nLen[0] = que->fl.data[3];
	nLen[1] = que->fl.data[4];

	int					nStartFrameNo = que->fl.data[5];
	int					pcid = (getpcid() - 1);								// PCID(PCKINDではない) (0オリジン)
	int					camset1 = que->fl.data[6 + 3 * pcid + 0];			// カメラセット１ (1オリジン)
	int					camset2 = que->fl.data[6 + 3 * pcid + 1];			// カメラセット２ (1オリジン)

	EM_DIV_HANTEI		emKind = (EM_DIV_HANTEI)que->fl.data[6 + 3 * pcid + 2];
	KizuPcInfManager*	cls_Pc = (KizuPcInfManager*)getcls_pcinfmgr();		// PC情報クラス

	// カメラセットNoから面を割り出す
	// ※有効値が設定されている場合のみ算出する
	// ※両方とも無効値の場合、表面とする
	int nMen = 0;
	if (0 != camset1) nMen = (0 == ((camset1 - 1) / (MAX_CAMSET / NUM_MEN)) ? 0 : 1);
	if (0 != camset2) nMen = (0 == ((camset2 - 1) / (MAX_CAMSET / NUM_MEN)) ? 0 : 1);

	if (DIV_HANTEI_PR != emKind)
	{
		LOG(em_WAR), "[%s] 担当判定異常(pcid=%d kind=%d [1:判定 2:周期 3:パララン])", my_sThreadName, getpcid(), emKind);
		nRetc = -1;
		goto ending;
	}
		
	if (0 > camset1)
	{
		LOG(em_WAR), "[%s] カメラセット異常(pcid=%d Camset1=%d)", my_sThreadName, getpcid(), camset1);
		nRetc = -1;
		goto ending;
	}

	if (0 > camset2)
	{
		LOG(em_WAR), "[%s] カメラセット異常(pcid=%d Camset2=%d)", my_sThreadName, getpcid(), camset2);
		nRetc = -1;
		goto ending;
	}

	//// 現ステータス確認
	if (StatusManager::STATE_INIT != gcls_Status.GetMainState())
	{
		LOG(em_ERR), "[%s] [%s] 時に途中開始要求受信(pcid=%d)", my_sThreadName, gcls_Status.GetMainStateName(), getpcid());
		nRetc = -1;
		goto ending;
	}

	// 統括へのデータ送信状態セット（送信なし）
	gcls_pSend->SetFrameSend(false);

	LOG(em_MSG), "[%s] 途中開始要求 (pcid=%d s=%d:%d camset1=%d camset2=%d frame=%d kind=%d[1:判定 2:周期 3:パララン] データ送信:%s)", my_sThreadName, getpcid(), nScode[0], nScode[1], camset1, camset2, nStartFrameNo, emKind, (gcls_pSend->IsFrameSend() ? "あり" : "なし"));

	//// メンバー変数に登録
	m_nAppParam = 0;	// エラーをリセット
	gcls_Status.SetUnten(unten);
	gcls_Status.SetParaCamset(camset1, camset2);
	m_nMen = nMen;
	m_nCamPair = getpcid() - ((NUM_HANTEI * NUM_MEN) + NUM_ALL_HTBACK);
	m_emKindHt = DIV_HANTEI_PR;
	m_nNowScode[0] = nScode[m_nMen];

	//// 検査開始時に、自分がわからない場合は、もう一度DBからPC名を取得する (なんかDBの調子が悪いよ対策)
	if (0 == memcmp(&"*", HANTEI(pcid), 1))
	{
		LOG(em_ERR), "[%s] 自分の名前不定", my_sThreadName);
		getpcname();
	}
	LOG(em_MSG), "[%s] 自分のPC名は[%s]", my_sThreadName, HANTEI(pcid));

	//// パラメータ読み直し
	gcls_PrmMgr.ReadParam_All();	//同期 読込に変更

	////////////////////////////////////
	// カメラ調整
	////////////////////////////////////
	if (DIV_UNTEN_CAMERA == unten)
	{
		// カメラ調整時

		// iPortViewer 起動
		if (!ExecuteProcess_iPortViewer())
		{
			// 異常
			nRetc = -1;
			SendState_PcCanNot();
		}
		// カメラ調整時は、これでOK
		goto ending;
	}

	////////////////////////////////////
	// 通常 又は サンプル撮影 又は シミュレーション
	////////////////////////////////////

	//// 外形算出プロセス 起動
	if (!ExecuteProcess_RectProc())
	{
		// 異常 △		
	}

	//// 画像処理ソフト 起動
	if (!m_bNgCam)
	{
		if (!ExecuteProcess_DetectProc())
		{
			// 異常
			nRetc = -1;
			SendState_PcCanNot();
		}
	}

	//// 初期化コマンド
	nRetc = SendCommand_Init();
	if (0 != nRetc)
	{
		if (-1 == nRetc) SendState_PcCanNot();
		nRetc = -1;
		goto ending;
	}

	// ------------------------------------------------------------------------------
	//// 初期状態 (これ以降のエラーの場合は、資源開放の処理も必要ね)
	gcls_Status.SetMainState(StatusManager::STATE_WAIT);
	// 単独起動時に初期化コマンド送信してからRPCサーバーの紐づけまでの時間が短かったため、待ち時間を10秒に変更
	Sleep(10000);												// 画像処理タスクが初期化されるまでちょっと待ち

	//// その他準備
	gcls_pSend->ClearFrameNo();
	gcls_pSend->SetCamPair(m_nCamPair);
	gcls_pSend->SetKindHt(m_emKindHt);

	//// 内部距離カウンタ
	NCL_CMD_SET_POSITION  pos;
	memset(&pos, 0x00, sizeof(pos));
	pos.cmd = NCL_SET_POSITION;
	pos.framenum = 0;
	pos.update_mask = 0xFFFFFFFF;
	pos.front_pos = (float)nLen[m_nMen];
	if (-1 == SendCommand_SetPosition(&pos))
	{
		// 重故障
		ResumeStop(-1, 0);											// 統括への送信は無し 
		nRetc = -1;
		SendState_PcCanNot();
		goto ending;
	}

	//// パラメータ事前読込
	if (-1 == SendCommand_PreLoad_Param(m_nNowScode[0]))
	{
		// 重故障
		ResumeStop(-1, 0);											// 統括への送信は無し 
		nRetc = -1;
		SendState_PcCanNot();
		goto ending;
	}
	//// 指定検査表面状態コードがMAX以外の場合、MAX_SCODEの事前読込を実施する
	//// ※検査基点WPD～検査位置間でイニシャル電文を受信した場合の対策
	//// 　MAX_SCODEを事前読込しておかないと画像エンジンがダウンする
	if (MAX_SCODE != m_nNowScode[0])
	{
		//// パラメータ事前読込(検査表面状態MAX値)
		if (-1 == SendCommand_PreLoad_Param(MAX_SCODE))
		{
			// 重故障
			ResumeStop(-1, 0);										// 統括への送信は無し 
			nRetc = -1;
			SendState_PcCanNot();
			goto ending;
		}
	}

	//// パラメータ変更コマンド
	if (-1 == SendCommand_Param(0, m_nNowScode[0]))
	{
		// 重故障
		ResumeStop(-1, 0);											// 統括への送信は無し 
		nRetc = -1;
		SendState_PcCanNot();
		goto ending;
	}

	//// 途中参加コマンド
	if (-1 == SendCommand_Resume(nStartFrameNo))
	{
		// 重故障
		ResumeStop(-1, 0);											// 統括への送信は無し 
		nRetc = -1;
		SendState_PcCanNot();
		goto ending;
	}

	// ロジックファイル状態表示
	gcls_Logic.Check(0);

	//// ヘルシー問い合わせ開始
	__int64 ts = (10 * 1000) * -10000;			// 1ミリ秒→100ナノ秒 (マイナス指定で、指定時間経過でシグナル) (第3引数は2回目以降の起動間隔)
	SetWaitableTimer(m_evHelcy, (LARGE_INTEGER *)&ts, STAT_CHECK_INTERVAL, NULL, NULL, FALSE);
	gcls_pSend->StartHls();

	//// カメラGCPチェックを開始
	GcpCheck_StartTimer();

ending:

	////////////////////////////////////
	// 最後の処理
	////////////////////////////////////
	// 統括へ結果を送信
	ResumeResponse(nStartFrameNo, emKind, nRetc);

	// 正常？異常
	if (0 == nRetc) {
		SendState_App(0);
		gcls_Status.SetMainState(StatusManager::STATE_RUN);			 // 検査中

	}
	else {
		SendState_App(APP_PARAM_EXEC);	// 実行失敗
	}
}

//------------------------------------------
// 途中開始アンサー送信処理
// int			 nStartFrameNo	途中参加フレーム
// EM_DIV_HANTEI emKind			担当判定
// int			 nAns			結果 (0:正常 1:軽故障 -1:重故障)
//------------------------------------------
void MainInctance::ResumeResponse(int nStartFrameNo, EM_DIV_HANTEI emKind, int nAns)
{
	int			mailRetc;
	COMMON_QUE	sque;

	sque.nEventNo	= FACT_TO_DEFECT_23;			// 途中開始アンサー
	sque.nLineNo	= getlineid();
	sque.fl.data[0] = getpcid() - ((NUM_HANTEI * NUM_MEN) + NUM_ALL_HTBACK);
	sque.fl.data[1] = getpcid();					// PcId
	sque.fl.data[2] = nAns;							// 結果
	sque.fl.data[3] = nStartFrameNo;				// 途中参加フレームNo （0オリジン）
	sque.fl.data[4] = emKind;						// 担当判定

	mailRetc = send_mail(TO_DEFECT, TOKATU, &sque);
	if (0 != mailRetc) syslog(SYSNO_MAIL_SNED_ERR, "[途中開始アンサー err_code=%d]", mailRetc);
}

// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 処理制御


//------------------------------------------
// コマンドパイプクラスよりエラー
// CommandSender::EM_PIPE_ERR emErrCode エラーコード
// int nDetail							詳細エラーコード
// BYTE const* pData					受信情報 (※必要なもののみ)
// 戻り値 復帰情報 (0:正常 1:軽故障 -1:重故障)
//------------------------------------------
int MainInctance::CheckCommandPipe_Err(CommandSender::EM_PIPE_ERR emErrCode, int nDetail, BYTE const* pData)
{
	CString cbuf;
	int	 ansRetc = 0;

	//// 処理分け
	if( CommandSender::CPIPE_ERR_COMMAND == emErrCode ) {			// 非同期エラー

		//=======================================
		// 非同期イベント
		if( NCL_SUCCESS == nDetail ) { return 0; }
		else if( 0 < nDetail) {
			LOG(em_INF), "[%s] <コマンドパイプイベント> [%d]", my_sThreadName, nDetail);
			return 0;
		}
		

		//=======================================
		// ログ定義

		//------------------------
		// 非同期エラー
		if	   (NCL_ERR_RECV_EDGE_INFO == nDetail)				{ ansRetc =-1; cbuf = "NCL_ERR_RECV_EDGE_INFO:エッジ位置通信エラー"; }
		else if(NCL_ERR_SEND_EDGE_INFO == nDetail)				{ ansRetc =-1; cbuf = "NCL_ERR_SEND_EDGE_INFO:エッジ位置通信エラー"; }
		else if(NCL_ERR_RECV_EDGE_INFO_TIMEOUT == nDetail)		{ ansRetc =-1; cbuf = "NCL_ERR_RECV_EDGE_INFO_TIMEOUT:エッジ位置通信タイムアウト"; }
		else if(NCL_ERR_SEND_EDGE_INFO_TIMEOUT == nDetail)		{ ansRetc =-1; cbuf = "NCL_ERR_SEND_EDGE_INFO_TIMEOUT:エッジ位置通信タイムアウト"; }
		else if(NCL_ERR_ASYNC_PARAM_FAILED == nDetail)			{ ansRetc = 1; cbuf = "NCL_ERR_ASYNC_PARAM_FAILED:フレーム番号指定のPARAMコマンドの失敗"; }
		else if(NCL_ERR_ASYNC_SET_POSITION_FAILED == nDetail)	{ ansRetc = 1; cbuf = "NCL_ERR_ASYNC_SET_POSITION_FAILED:フレーム番号指定のSET_POSITIONコマンドの失敗"; }


		//------------------------
		// 非同期イベント
		else if(NCL_ERR_FRAME_SKIPPED	== nDetail)				{ ansRetc = 0; cbuf = "NCL_ERR_FRAME_SKIPPED:フレームスキップ発生"; }
		else if(NCL_ERR_ASYNC_PARAM_SUCCESS	== nDetail)			{ ansRetc = 0; cbuf = "NCL_ERR_ASYNC_PARAM_SUCCESS:フレーム番号指定のPARAMコマンドの成功"; }
		else if(NCL_ERR_ASYNC_SET_POSITION_SUCCESS	== nDetail)	{ ansRetc = 0; cbuf = "NCL_ERR_ASYNC_SET_POSITION_SUCCESS:フレーム番号指定のSET_POSITIONコマンドの成功"; }

		//------------------------
		// 非同期 不明
		else												{ ansRetc = 1; cbuf.Format("不明なエラー:%d", nDetail);	}


		//=======================================
		// 種類毎の処理

		//------------------------
		// フレームスキップ発生
		if( NCL_ERR_FRAME_SKIPPED == nDetail) {
			LOG(em_MSG), "[%s] <コマンドパイプ> [%s]", my_sThreadName, cbuf);

			syslog(116, "[非同期通知:詳細=%s]", cbuf);

		}
		// △。他はどうするか？


		//------------------------
		// コマンドパイプ 非同期イベント
		else if( -2000 >= nDetail ) {	

		}

		//------------------------
		// コマンドパイプエラー
		else if( 0 > nDetail ) {						// エラー
			LOG(em_WAR), "[%s] <コマンドパイプエラー> 非同期エラー [%s]", my_sThreadName, cbuf);
			syslog(104, "[非同期エラー:詳細=%s]", cbuf);
			// なんかエラーを検出 = 画像処理タスク自体はおかしくない = 軽故障
			if(0 == ansRetc) ansRetc = 1;
		}

	//// その他エラー
	} else {														// その他のエラー
		switch (emErrCode) {
		case CommandSender::CPIPE_OPEN_ERR:
			cbuf = "オープンで失敗";
			break;
		case CommandSender::CPIPE_READ_ERR:
			cbuf = "読込みで失敗";
			break;
		case CommandSender::CPIPE_DISCONNECT_ERR:
			cbuf = "パイプが勝手に切断された";
			break;
		}
		LOG(em_ERR), "[%s] <コマンドパイプエラー> %s [詳細=%d]", my_sThreadName, cbuf, nDetail);
		syslog(104, "[%s:詳細=%d]", cbuf, nDetail);

		// パイプがおかしい = 画像処理タスク自体がおかしい = 重故障
		ansRetc = -1;

	}

	//// 最終チェック
	if (-1 == ansRetc) {
		StatusManager::MAIN_STATE st = gcls_Status.GetMainState();
		
		if( CommandSender::CPIPE_DISCONNECT_ERR == emErrCode) {
			// パイプの切断は重故障のまま

		} else if(StatusManager::STATE_WAIT != st && StatusManager::STATE_RUN != st) {
			LOG(em_ERR), "[%s] 稼動中では無いので、軽故障に変更", my_sThreadName, gcls_Status.GetMainStateName());
			ansRetc = 1;
		}
	}
	return ansRetc;
}

//------------------------------------------
// APP異常項目パラメータ 切り替え
// DWORD addr ビット指定位置 (ビット操作時のみ使用)
// bool onoff 変更値 (true:正常 false:異常)
//------------------------------------------
void MainInctance::SendState_App(DWORD state)
{
	if(0 == m_nCamPair) return;		// バックアップ機なら無視
	m_nAppParam = state;

	// 機器状態変更
	if(		 DIV_HANTEI_HT == m_emKindHt ) KizuFunction::SetStatus("HT_APP_%d_ID", m_nCamPair, (int)state);
	else if( DIV_HANTEI_PR == m_emKindHt ) KizuFunction::SetStatus("PR_APP_%d_ID", m_nCamPair, (int)state);
	else if( DIV_HANTEI_CY == m_emKindHt ) KizuFunction::SetStatus("CY_APP_%d_ID", m_nCamPair, (int)state); 

	
	if(0==state) { LOG(em_MSG), "[%s] APP異常項目 正常", my_sThreadName); }
	else		 { LOG(em_ERR), "[%s] APP異常項目 異常 [%d]", my_sThreadName, m_nAppParam); }
}
void MainInctance::SendState_App(DWORD addr, bool onoff)
{
	if(0 == m_nCamPair) return;		// バックアップ機なら無視
	bool	bSetting = false;		// 値変更有無
	DWORD	nChange = m_nAppParam;
	
	// ビット操作
	if( ! onoff )	nChange |= addr;		// ビット立てる = 異常
	else			nChange &= ~addr;		// ビット落とす = 正常

	if(nChange == m_nAppParam) return;		// 値の変更無し

	// 値の変更
	SendState_App(nChange);
}


//------------------------------------------
// 続行不可能解除
// bool reboot 画像処理ソフト 再起動 (true:再起動)
//------------------------------------------
void MainInctance::SendState_PcCanOk(bool reboot)
{
	if(StatusManager::STATE_CANNOT != gcls_Status.GetMainState() ) {
		KizuFunction::SetStatus("HT_CANNOT_%d_ID", getpcid(), 0, false);		// 食い違い防止のため	
		return;
	}

	//// 画像処理タスクを再起動
	if(reboot) {
		// 画像処理ソフト 停止
		DisposeProcess_DetectProc();
			
		// 外形算出プロセス
		ExecuteProcess_RectProc();


		// 画像処理ソフト 起動
		if( ! ExecuteProcess_DetectProc() ) {
			// 異常
			SendState_PcCanNot();
			LOG(em_ERR), "[%s] 続行不可能解除失敗！！", my_sThreadName );
			return;
		}
	}
	//// 復活
	KizuFunction::SetStatus("HT_CANNOT_%d_ID", getpcid(), 0);

	//// 初期状態
	gcls_Status.SetMainState(StatusManager::STATE_INIT);
	LOG(em_MSG), "[%s] 続行不可能 解除", my_sThreadName);
	syslog(114, "");

	//// 統括に状態を通知
	Send_ToDefect_State();
}

//------------------------------------------
// 当判定PC が 続行不可能
//------------------------------------------
void MainInctance::SendState_PcCanNot()
{
#ifdef SOT_TEST
	return;
#endif

	// 続行不可能
	gcls_Status.SetMainState(StatusManager::STATE_CANNOT);

	// 機器状態変更
	KizuFunction::SetStatus("HT_CANNOT_%d_ID", getpcid(), 1);
	
	LOG(em_ERR), "[%s] 続行不可能！！", my_sThreadName);
	syslog(115, "");

	//---------------------------------------
	// 全長処理 切断
#ifdef FRAME_RAW_SEND
	for (int ii=0; ii<FRAME_HT_SEND_PORT; ii++) {
		gcls_pSockRpi[ii]->SetRetryMode(false);		// リトライはしたくない
		gcls_pSockRpi[ii]->SetDisConnect();
	}
#endif
}

//------------------------------------------
// 画像入力異常
// int nCamId  対象  (0:全部 1～:該当カメラ) たぶんそのカメラが担当するカメラセット,カメラアングルの順に1～の連番のはず
// bool state  状態  (true:正常 false:異常)
//------------------------------------------
void MainInctance::SendState_Grab(int nCamId, bool state)
{
	// ロジックパララン時は画像入力異常を立てない
	if (DIV_HANTEI_PR == m_emKindHt) return;

	CString wk;
	// 画像入力
	if(0 == nCamId) {
		int nCamSetNum = gcls_Status.GetCamPairItemNum(m_nCamPair, m_emKindHt);
		for(int ii=0; ii<nCamSetNum; ii++) {
			int nCh = 0;

			// %d_%d  カメラペア_今野さんのカメラ番号(カメラセット、カメラ角度順)
			wk.Format("HT_GRAB_%d_%d_ID", gcls_Status.GetCamPairItemCamset(m_nCamPair, ii, m_emKindHt), nCh+1 );
			// 周期機能対応 ------>>>
			if(DIV_HANTEI_CY == m_emKindHt) 	wk.Format("CY_GRAB_%d_%d_ID", gcls_Status.GetCamPairItemCamset(m_nCamPair, ii, m_emKindHt), nCh+1 );
			// <<<-------------------
			
			KizuFunction::SetStatus(wk, state, !state);
			nCh++;

		}
	} else {
		wk.Format("HT_GRAB_%d_%d_ID", m_nCamPair, nCamId );
		// 周期機能対応 ------>>>
		if(DIV_HANTEI_CY == m_emKindHt) 	wk.Format("CY_GRAB_%d_%d_ID", m_nCamPair, nCamId );
		// <<<-------------------

		KizuFunction::SetStatus(wk, state, !state);
	}
}


//------------------------------------------
// 状態を通知
//------------------------------------------
void MainInctance::Send_ToDefect_State()
{
	COMMON_QUE	sque;
	sque.nEventNo = FACT_TO_DEFECT_09;				// 判定停止アンサー
	sque.nLineNo = getlineid();
	sque.fl.data[0] = m_nCamPair;					// カメラペア = PcKind
	sque.fl.data[1] = getpcid();					// PcId
	sque.fl.data[2] = (StatusManager::STATE_CANNOT == gcls_Status.GetMainState() ? -1 : 0);	// 状態 (0:正常 1:軽故障 -1:重故障)
	sque.fl.data[3] = m_emKindHt;

	int mailRetc = send_mail(TO_DEFECT, TOKATU, &sque);
	if( 0 != mailRetc ) syslog(SYSNO_MAIL_SNED_ERR, "[状態問い合わせアンサー err_code=%d]", mailRetc);
}

//------------------------------------------
// フレーム画像取得失敗を通知
// DWORD camset 
// DWORD FrameNo 
//------------------------------------------
//void MainInctance::Send_ToDefect_FrameReqFail(DWORD camset, DWORD FrameNo)
//{
//	COMMON_QUE	sque;
//	sque.nEventNo = FACT_TO_DEFECT_10;				// フレーム画像取得失敗通知
//	sque.nLineNo = getlineid();
//	sque.fl.data[0] = m_nCamPair;					// カメラペア = PcKind
//	sque.fl.data[1] = getpcid();					// PcId
//	sque.fl.data[2] = camset;						// 要求カメラセット
//	sque.fl.data[3] = FrameNo;						// 要求フレームNo
//	int mailRetc = send_mail(TO_DEFECT, TOKATU, &sque);
//	if( 0 != mailRetc ) syslog(SYSNO_MAIL_SNED_ERR, "[フレーム画像取得失敗通知 err_code=%d]", mailRetc);
//}

//------------------------------------------
// 上位情報(分割版)取得&送信メイン処理
// const COMMON_QUE* pQue : キュー情報
// L[0]　遅延実行キー（０：即時　１～：キー）
// L[1]　今回受信する上位情報のサイズ[Byte]
// L[2]　何個目 (1オリジン)
// L[3]　最大分割数
// L[4]　BYTE[220]　上位情報
//------------------------------------------
void MainInctance::Send_CoilInfDiv(const COMMON_QUE* pQue)
{
	////============================
	// 情報取得
	//int nDly = pQue->fl.data[0];				// 遅延実行キー(0:即時実行 1～:キー)
	int nSize = pQue->fl.data[1];				// 今回送信する上位情報のサイズ(SIZE_SEND_DIVを超える事はないはず)
	int nNum = pQue->fl.data[2];				// データの位置(1オリジン)
	int nMaxCnt = pQue->fl.data[3];				// 分割個数
	int scode[NUM_MEN];
	for( int ii = 0; ii < NUM_MEN; ii++ ) { scode[ii] = pQue->fl.data[4 + ii]; }
	void *pData = (void*)&pQue->fl.data[4+NUM_MEN];
	int nOffSet = DIVCOIL_ONE_SIZE_SEND*(nNum-1);

	// 歯抜け発生？
	if (1 != nNum && 0 == m_typMergedInfo.nDivCnt) {
		LOG(em_WAR), "[%s] 上位情報受信 抜け発生! %d個目", my_sThreadName, nNum);
		return;
	}

	////============================================
	// メンバ変数にセット

	//// 分割1個目の情報から取得するデータ
	if (1 == nNum) {
		memset(&m_typMergedInfo, 0x00, sizeof(m_typMergedInfo));	// 初期化

		m_typMergedInfo.nDivCnt = nMaxCnt;	// 分割個数				
	}

	////全ての分割情報から取得するデータ
	m_typMergedInfo.nSize += nSize;								// 合計サイズ
	memcpy(&m_typMergedInfo.bPcInfo[nOffSet], pData, nSize);	// 上位情報
	m_typMergedInfo.bGetFlg[nNum - 1] = true;					// データ位置のフラグをTrueに



	////============================================
	// 分割データ受信の途中ならばここで終了
	bool bSendOk = true;
	for (int ii = 0; ii < m_typMergedInfo.nDivCnt; ii++) {
		if (!m_typMergedInfo.bGetFlg[ii]) bSendOk = false;
	}
	if (!bSendOk) return;

	LOG(em_MSG), "[%s] 上位情報受信 [%d分割]", my_sThreadName, nMaxCnt);


	////==============================================
	// 全ての分割データを受信したらEPCエンジンに送信
	// 上位設定情報コマンド
	int retc = SendCommand_CoilInf(m_typMergedInfo.nSize, m_typMergedInfo.bPcInfo);
	// アンサーにより処理分け
	CommandAnswerProc(retc, APP_PARAM_COILINF);

	// メンバ変数をクリア
	memset(&m_typMergedInfo, 0x00, sizeof(m_typMergedInfo));

	// ロジックファイル状態表示
	gcls_Logic.Check( scode [ m_nMen ]);
}


// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// コマンド制御

//------------------------------------------
// 画像処理タスク 初期化コマンド (資源割り当て)
// 復帰情報 (0:正常 1:軽故障 -1:重故障)
//------------------------------------------
int MainInctance::SendCommand_Init()
{
	NCL_CMD_INIT		cmd;
	memset(&cmd, 0x00, sizeof(cmd));

	//// 初期化コマンド 生成
	cmd.cmd	= NCL_INIT;
	cmd.camnum = gcls_Status.GetCamPairItemNum(m_nCamPair, m_emKindHt);
	for(DWORD ii=0; ii<cmd.camnum; ii++) cmd.camid[ii] = gcls_Status.GetCamPairItemCamset(m_nCamPair, ii, m_emKindHt); 
	cmd.initial_scode = m_nNowScode[0];


	// ODBC接続文字列生成
	char cOdbc[64];
	char cUser[64];
	char cPass[64];
	GetPrivateProfileString(INI_DB, "ODBCNAME", "", cOdbc, sizeof(cOdbc), TASKINI_NAME);
	GetPrivateProfileString(INI_DB, "USERID",   "", cUser, sizeof(cUser), TASKINI_NAME);
	GetPrivateProfileString(INI_DB, "PASSWORD", "", cPass, sizeof(cPass), TASKINI_NAME);
	sprintf(cmd.dbconnstr, EPC_ODBC_FORMAT, cOdbc, cUser, cPass);

	// ロジックパラランの場合
	if (DIV_HANTEI_PR == m_emKindHt)
	{
		// カメラセットは負数とする
		for (DWORD ii = 0; ii < cmd.camnum; ii++)
		{
			if (0 != cmd.camid[ii]) cmd.camid[ii] *= -1;
		}

		// ロジックパララン用データベースに変更
		GetPrivateProfileString(INI_DB_PARA, "ODBCNAME", "", cOdbc, sizeof(cOdbc), TASKINI_NAME);
		GetPrivateProfileString(INI_DB_PARA, "USERID", "", cUser, sizeof(cUser), TASKINI_NAME);
		GetPrivateProfileString(INI_DB_PARA, "PASSWORD", "", cPass, sizeof(cPass), TASKINI_NAME);
		sprintf(cmd.dbconnstr, EPC_ODBC_FORMAT, cOdbc, cUser, cPass);
	}

	//// コマンド送信
	LOG(em_MSG), "[%s] コマンド送信 [%d 初期化コマンド:scode=%d camsetnum=%d camset=%d,%d,%d,%d %d,%d,%d,%d dbconnstr=\"%s\"]", my_sThreadName, 
		cmd.cmd, cmd.initial_scode, cmd.camnum, cmd.camid[0], cmd.camid[1], cmd.camid[2], cmd.camid[3], cmd.camid[4], cmd.camid[5], cmd.camid[6], cmd.camid[7], cmd.dbconnstr );


	NCL_ANSWER retc;
	int nRetc = gcls_pCom->SendCommand( (BYTE*)&cmd, sizeof(cmd), &retc, sizeof(retc), COMMAND_INIT_WAIT);
	if( 0 > nRetc ) {
		LOG(em_ERR), "[%s] コマンド送信失敗 [初期化コマンド:%d]", my_sThreadName, nRetc );
		syslog( 107, "[初期化コマンド:%d]", nRetc );
		return -1;			// 初期化コマンド時は全部重故障

	} else if (0 != nRetc) {
		LOG(em_ERR), "[%s] コマンドアンサー異常 [初期化コマンド:%d:%d]", my_sThreadName, retc.retcode, retc.cmd );
		syslog( 106, "[初期化コマンド:%d:%d]", retc.retcode, retc.cmd );
		return -1;

	} else {
		return 0;
	}
}


//------------------------------------------
// 画像処理タスク 資源開放コマンド
// 復帰情報 (0:正常 1:軽故障 -1:重故障)
//------------------------------------------
int MainInctance::SendCommand_Dispose()
{
	NCL_CMD	cmd;
	memset(&cmd, 0x00, sizeof(cmd));

	// 資源開放コマンド 生成
	cmd.cmd	= NCL_DISPOSE;
	LOG(em_MSG), "[%s] コマンド送信 [%d 資源開放コマンド]", my_sThreadName, cmd.cmd );

	// コマンド送信
	NCL_ANSWER retc;
	int nRetc = gcls_pCom->SendCommand( (BYTE*)&cmd, sizeof(cmd), &retc, sizeof(retc), COMMAND_DISPOSE_WAIT);
	if( 0 > nRetc ) {
		LOG(em_ERR), "[%s] コマンド送信失敗 [資源開放コマンド:%d]", my_sThreadName, nRetc );
		syslog( 107, "[資源開放コマンド:%d]", nRetc );
		return -1;			// 資源開放ココマンド時は全部重故障

	} else if (0 != nRetc) {
		LOG(em_ERR), "[%s] コマンドアンサー異常 [資源開放コマンド:%d:%d]", my_sThreadName, retc.retcode, retc.cmd );
		syslog( 106, "[資源開放コマンド:%d:%d]", retc.retcode, retc.cmd );
		return -1;			// 資源開放ココマンド時は全部重故障

	} else {
		return 0;
	}
}


//------------------------------------------
// 画像処理タスク 終了コマンド
// 復帰情報 (0:正常 1:軽故障 -1:重故障)
//------------------------------------------
int MainInctance::SendCommand_Exit()
{
	NCL_CMD	cmd;
	memset(&cmd, 0x00, sizeof(cmd));

	// 終了コマンド 生成
	cmd.cmd	= NCL_EXIT;
	LOG(em_MSG), "[%s] コマンド送信 [%d 終了コマンド]", my_sThreadName, cmd.cmd );

	// コマンド送信
	NCL_ANSWER retc;
	int nRetc = gcls_pCom->SendCommand( (BYTE*)&cmd, sizeof(cmd), &retc );
	if( 0 != nRetc ) {
		LOG(em_ERR), "[%s] コマンド送信失敗 [終了コマンド:%d]", my_sThreadName, nRetc );
		syslog( 107, "[終了コマンド:%d]", nRetc );
	}
	return 0;
}

//------------------------------------------
// 画像処理タスク 検査開始コマンド
// 復帰情報 (0:正常 1:軽故障 -1:重故障)
//------------------------------------------
int MainInctance::SendCommand_Start()
{
	NCL_CMD	cmd;
	memset(&cmd, 0x00, sizeof(cmd));

	// 開始コマンド 生成
	cmd.cmd	= NCL_START;
	LOG(em_MSG), "[%s] コマンド送信 [%d 検査開始コマンド]", my_sThreadName, cmd.cmd );

	// コマンド送信
	NCL_ANSWER retc;
	int nRetc = gcls_pCom->SendCommand( (BYTE*)&cmd, sizeof(cmd), &retc, sizeof(retc), COMMAND_INIT_WAIT);
	if( 0 > nRetc ) {
		LOG(em_ERR), "[%s] コマンド送信失敗 [検査開始コマンド:%d]", my_sThreadName, nRetc );
		syslog( 107, "[検査開始コマンド:%d]", nRetc );
		return -1;			// 検査開始コマンド時は全部重故障

	} else if (0 != nRetc) {
		LOG(em_ERR), "[%s] コマンドアンサー異常 [検査開始コマンド:%d:%d]", my_sThreadName, retc.retcode, retc.cmd );
		syslog( 106, "[検査開始コマンド:%d:%d]", retc.retcode, retc.cmd );
		return -1;			// 検査開始コマンド時は全部重故障

	} else {
		return 0;
	}
}
//------------------------------------------
// 画像処理タスク 検査停止コマンド
// 復帰情報 (0:正常 1:軽故障 -1:重故障)
//------------------------------------------
int MainInctance::SendCommand_Stop()
{
	NCL_CMD	cmd;
	memset(&cmd, 0x00, sizeof(cmd));

	// 停止コマンド 生成
	cmd.cmd	= NCL_STOP;
	LOG(em_MSG), "[%s] コマンド送信 [%d 検査停止コマンド]", my_sThreadName, cmd.cmd );

	// コマンド送信
	NCL_ANSWER retc;
	int nRetc = gcls_pCom->SendCommand( (BYTE*)&cmd, sizeof(cmd), &retc );
	if( 0 > nRetc ) {
		LOG(em_ERR), "[%s] コマンド送信失敗 [検査停止コマンド:%d]", my_sThreadName, nRetc );
		syslog( 107, "[検査停止コマンド:%d]", nRetc );
		return 1;

	} else if (0 != nRetc) {
		LOG(em_ERR), "[%s] コマンドアンサー異常 [検査停止コマンド:%d:%d]", my_sThreadName, retc.retcode, retc.cmd );
		syslog( 106, "[検査停止コマンド:%d:%d]", retc.retcode, retc.cmd );
		return 1;

	} else {
		return 0;
	}
}

//------------------------------------------
// 画像処理タスク パラーメータ変更コマンド
// DWORD framenum 実行フレーム番号 (0で即時実行)
// DWORD scode 検査表面状態No
// 復帰情報 (0:正常 1:軽故障 -1:重故障)
//------------------------------------------
int MainInctance::SendCommand_Param(DWORD framenum, DWORD scode)
{
	NCL_CMD_PARAM	cmd;
	memset(&cmd, 0x00, sizeof(cmd));

	// パラメータコマンド 生成
	cmd.cmd	= NCL_PARAM;
	cmd.scode	= scode;
	cmd.framenum = framenum;
	
	LOG(em_MSG), "[%s] コマンド送信 [%d パラメータ変更コマンド:framenum=%d scode=%d]", my_sThreadName, cmd.cmd, cmd.framenum, cmd.scode );

	// コマンド送信
	NCL_ANSWER retc;
	int nRetc = gcls_pCom->SendCommand( (BYTE*)&cmd, sizeof(cmd), &retc, sizeof(retc), COMMAND_PARAM_WAIT);
	if( 0 > nRetc ) {
		LOG(em_ERR), "[%s] コマンド送信失敗 [パラメータ変更コマンド:%d]", my_sThreadName, nRetc );
		syslog( 107, "[パラメータ変更コマンド:%d]", nRetc );
		return -1;

	} else if (0 != nRetc) {
		LOG(em_ERR), "[%s] コマンドアンサー異常 [パラメータ変更コマンド:%d:%d]", my_sThreadName, retc.retcode, retc.cmd );
		syslog( 106, "[パラメータ変更コマンド:%d:%d]", retc.retcode, retc.cmd );
		return 1;

	} else {
		return 0;
	}
}

//------------------------------------------
// 画像処理タスク 上位情報設定コマンド
// DWORD size データ部のサイズ
// BYTE const* pData	送信情報
// 復帰情報 (0:正常 1:軽故障 -1:重故障)
//------------------------------------------
int MainInctance::SendCommand_CoilInf(DWORD size, BYTE const* pData)
{
	//BYTE	wk[256];
	BYTE	wk[sizeof(NCL_CMD_COILINFO) + DIVCOIL_MAX_SEND_SIZE];
	NCL_CMD_COILINFO& cmd = (NCL_CMD_COILINFO&)wk;	

	// パラメータコマンド 生成
	cmd.cmd	= NCL_COILINFO;
	cmd.info_size = size;
	memcpy( cmd.coil_info, pData, size);

	LOG(em_MSG), "[%s] コマンド送信 [%d 上位情報設定コマンド:size=%d]", my_sThreadName, cmd.cmd, cmd.info_size);

	// コマンド送信
	NCL_ANSWER retc;
	int nRetc = gcls_pCom->SendCommand( wk, (size+sizeof(NCL_CMD_COILINFO)-sizeof(BYTE)), &retc );
	if( 0 > nRetc ) {
		LOG(em_ERR), "[%s] コマンド送信失敗 [上位情報設定コマンド:%d]", my_sThreadName, nRetc );
		syslog( 107, "[上位情報設定コマンド:%d]", nRetc );
		return -1;

	} else if (0 != nRetc) {
		LOG(em_ERR), "[%s] コマンドアンサー異常 [上位情報設定コマンド:%d:%d]", my_sThreadName, retc.retcode, retc.cmd );
		syslog( 106, "[上位情報設定コマンド:%d:%d]", retc.retcode, retc.cmd );
		return 1;

	} else {
		return 0;
	}
}


//------------------------------------------
// DBからメモリへのパラメータ読み込み設定コマンド
// DWORD scode 検査表面状態No (0:全scode読込)
// 復帰情報 (0:正常 1:軽故障 -1:重故障)
//------------------------------------------
int MainInctance::SendCommand_PreLoad_Param(DWORD scode)
{
	NCL_CMD_PRELOAD_PARAM	cmd;
	memset(&cmd, 0x00, sizeof(cmd));

	// パラメータコマンド 生成
	cmd.cmd		= NCL_PRELOAD_PARAM;
	cmd.scode	= scode;

	LOG(em_MSG), "[%s] コマンド送信 [%d パラーメータ事前読込コマンド:scode=%d]", my_sThreadName, cmd.cmd, cmd.scode );


	// コマンド送信
	NCL_ANSWER retc;
	int nRetc = gcls_pCom->SendCommand( (BYTE*)&cmd, sizeof(cmd), &retc );
	if( 0 > nRetc ) {
		LOG(em_ERR), "[%s] コマンド送信失敗 [パラーメータ事前読込コマンド:%d]", my_sThreadName, nRetc );
		syslog( 107, "[パラーメータ事前読込コマンド:%d]", nRetc );
		return -1;

	} else if (0 != nRetc) {
		LOG(em_ERR), "[%s] コマンドアンサー異常 [パラーメータ事前読込コマンド:%d:%d]", my_sThreadName, retc.retcode, retc.cmd );
		syslog( 106, "[パラーメータ事前読込コマンド:%d:%d]", retc.retcode, retc.cmd );
		return 1;

	} else {
		return 0;
	}
};

//------------------------------------------
// 内部距離カウンタ設定コマンド
// NCL_CMD_SET_POSITION* pVal 指定情報
// 復帰情報 (0:正常 1:軽故障 -1:重故障)
//------------------------------------------
int MainInctance::SendCommand_SetPosition(NCL_CMD_SET_POSITION* pVal)
{
	NCL_CMD_SET_POSITION	cmd;
	memcpy(&cmd, pVal, sizeof(cmd));

	// パラメータコマンド 生成
	cmd.cmd		= NCL_SET_POSITION;		// 念のために再セット
	LOG(em_MSG), "[%s] コマンド送信 [%d 内部距離カウンタ設定 コマンド:BIT=%X, f=%d (%.0f,%.0f,%.0f)(%.0f,%.0f)(%.0f,%.0f)]", my_sThreadName,
		cmd.cmd, cmd.update_mask, cmd.framenum, cmd.front_pos, cmd.tail_pos, cmd.shear_cut_pos, 
		cmd.univ_cnt_up_pos[0], cmd.univ_cnt_up_pos[1], cmd.univ_cnt_dn_pos[0], cmd.univ_cnt_dn_pos[1]);


	// コマンド送信
	NCL_ANSWER retc;
	int nRetc = gcls_pCom->SendCommand( (BYTE*)&cmd, sizeof(cmd), &retc );
	if( 0 > nRetc ) {
		LOG(em_ERR), "[%s] コマンド送信失敗 [内部距離カウンタ設定コマンド:%d]", my_sThreadName, nRetc );
		syslog( 107, "[内部距離カウンタ設定コマンド:%d]", nRetc );
		return -1;

	} else if (0 != nRetc) {
		LOG(em_ERR), "[%s] コマンドアンサー異常 [内部距離カウンタ設定コマンド:%d:%d]", my_sThreadName, retc.retcode, retc.cmd );
		syslog( 106, "[内部距離カウンタ設定コマンド:%d:%d]", retc.retcode, retc.cmd );
		return 1;

	} else {
		return 0;
	}
};

//------------------------------------------
// 未実行設定のキャンセルコマンド
// 復帰情報 (0:正常 1:軽故障 -1:重故障)
//------------------------------------------
int MainInctance::SendCommand_ClearQueue()
{
	NCL_CMD	cmd;
	memset(&cmd, 0x00, sizeof(cmd));

	// 未実行のPARAM,SET_POSITIONを消去コマンド 生成
	cmd.cmd	= NCL_CLEAR_QUEUE;
	LOG(em_MSG), "[%s] コマンド送信 [%d 未実行設定のキャンセルコマンド]", my_sThreadName, cmd.cmd );

	// コマンド送信
	NCL_ANSWER retc;
	int nRetc = gcls_pCom->SendCommand( (BYTE*)&cmd, sizeof(cmd), &retc );
	if( 0 > nRetc ) {
		LOG(em_ERR), "[%s] コマンド送信失敗 [未実行設定のキャンセルコマンド:%d]", my_sThreadName, nRetc );
		syslog( 107, "[未実行設定のキャンセルコマンド:%d]", nRetc );
		return 1;

	} else if (0 != nRetc) {
		LOG(em_ERR), "[%s] コマンドアンサー異常 [未実行設定のキャンセルコマンド:%d:%d]", my_sThreadName, retc.retcode, retc.cmd );
		syslog( 106, "[未実行設定のキャンセルコマンド:%d:%d]", retc.retcode, retc.cmd );
		return 1;

	} else {
		return 0;
	}
};

//------------------------------------------
// 状態問い合わせ
// NCL_ANSWER_QUERY_STATE* pVal	取得状態
//------------------------------------------
int MainInctance::SendCommand_Query_State(NCL_ANSWER_QUERY_STATE* pVal)
{
	NCL_CMD	cmd;
	memset(&cmd, 0x00, sizeof(cmd));

	// パラメータコマンド 生成
	cmd.cmd	= NCL_QUERY_STATUS;
	LOG(em_MSG), "[%s] コマンド送信 [%d 状態問い合わせ]", my_sThreadName, cmd.cmd );

	// コマンド送信
	NCL_ANSWER_QUERY_STATE retc;
	int nRetc = gcls_pCom->SendCommand( (BYTE*)&cmd, sizeof(cmd), &retc, sizeof(retc),10*1000 );
	if( 0 > nRetc ) {
		LOG(em_ERR), "[%s] コマンド送信失敗 [状態問い合わせ:%d]", my_sThreadName, nRetc );
		syslog( 107, "[状態問い合わせ:%d]", nRetc );
		return -1;

	} else if (0 != nRetc) {
		LOG(em_ERR), "[%s] コマンドアンサー異常 [状態問い合わせ:%d:%d]", my_sThreadName, retc.retcode, retc.cmd );
		syslog( 106, "[状態問い合わせ:%d:%d]", retc.retcode, retc.cmd );
		return 1;

	} else {
		memcpy(pVal, &retc, sizeof(NCL_ANSWER_QUERY_STATE) );
		return 0;
	}
}


//------------------------------------------
// 画像処理タスク アンサー時処理
// int retc 復帰情報
// DWORD addr APP 異常項目 パラメータ
//------------------------------------------
void MainInctance::CommandAnswerProc(int retc, DWORD addr)
{
	if( 0 == retc) {
		SendState_App(addr, true);

	// 軽故障
	} else if (1 == retc) {
		SendState_App(addr, false);

	// 重故障
	} else {
		if (DIV_HANTEI_PR != m_emKindHt)
		{
			DetectStop(1, -1);			// 統括へ異常送信
		}
		else
		{
			ResumeStop(1, -1);			// 統括へ異常送信
		}
		SendState_PcCanNot();
	}
}

//------------------------------------------
// 画像処理タスク 途中参加コマンド
// 復帰情報 (0:正常 1:軽故障 -1:重故障)
//------------------------------------------
int MainInctance::SendCommand_Resume(int nStartFrameNo)
{
	NCL_CMD_RESUME_START	cmd;
	memset(&cmd, 0x00, sizeof(cmd));

	// 開始コマンド 生成
	cmd.cmd = NCL_RESUME_START;
	cmd.start_frame_num = nStartFrameNo;
	LOG(em_MSG), "[%s] コマンド送信 [%d 途中参加コマンド:StartFrameNo=%d]", my_sThreadName, cmd.cmd, cmd.start_frame_num);

	// コマンド送信
	NCL_ANSWER retc;
	int nRetc = gcls_pCom->SendCommand((BYTE*)&cmd, sizeof(cmd), &retc, sizeof(retc), COMMAND_INIT_WAIT);
	if (0 > nRetc) {
		LOG(em_ERR), "[%s] コマンド送信失敗 [途中参加コマンド:%d]", my_sThreadName, nRetc );
		syslog(107, "[検査途中参加コマンド:%d]", nRetc);
		return -1;			// 検査途中参加コマンド時は全部重故障

	}
	else if (0 != nRetc) {
		LOG(em_ERR), "[%s] コマンドアンサー異常 [途中参加コマンド:%d:%d]", my_sThreadName, retc.retcode, retc.cmd );
		syslog(106, "[途中参加コマンド:%d:%d]", retc.retcode, retc.cmd);
		return -1;			// 検査途中参加コマンド時は全部重故障

	}
	else {
		return 0;
	}
}

//------------------------------------------
// 途中停止処理
//		※ 通常時は、ストップ、資源開放 を実施
//		※ 校正時は、iPortViewer終了、画像処理タスク起動
// int mode 要求状態 (0:正常な終了要求  1:何かおかしくて強制的に終了要求 -1:送信しない)
// int ansRetc 結果 (0:正常 1:軽故障 -1:重故障) ※ 処理中に、重故障が発生した場合は、重故障になる
//------------------------------------------
void MainInctance::ResumeStop(int mode, int ansRetc)
{
	////////////////////////////////////
	// 下準備
	////////////////////////////////////
	EM_DIV_UNTEN		unten = gcls_Status.GetUnten();					// 運転モード

	// 現ステータス確認
	if (StatusManager::STATE_NON == gcls_Status.GetMainState() ||
		StatusManager::STATE_INIT == gcls_Status.GetMainState() ||
		StatusManager::STATE_CANNOT == gcls_Status.GetMainState())
	{
		LOG(em_ERR), "[%s] [%s] 時に途中停止要求受信", my_sThreadName, gcls_Status.GetMainStateName() );
		return;
	}
	LOG(em_MSG), "[%s] 途中停止要求。(%s) ==============>>>", my_sThreadName, (0 == mode ? "正常停止要求" : "異常による強制停止要求"));

	////////////////////////////////////
	// カメラ調整
	////////////////////////////////////
	if (DIV_UNTEN_CAMERA == unten)
	{
		// カメラ調整時

		// iPortViewer 終了
		DisposeProcess_iPortViewer();

		////////////////////////////////////
		// 通常 又は サンプル撮影 又は シミュレーション
		////////////////////////////////////
	}
	else
	{
		//// ヘルシータイマー停止
		CancelWaitableTimer(m_evHelcy);
		gcls_pSend->StopHls();

		//// カメラGCPチェックを停止
		CancelWaitableTimer(m_evGcpCheck);

		//---------------------------------------
		// エンジン

		//// 検査中の場合は、停止
		if (StatusManager::STATE_RUN == gcls_Status.GetMainState())
		{
			// 停止コマンド
			if (1 == SendCommand_Stop())
			{
				if (-1 != ansRetc) ansRetc = 1;
			}
			gcls_Status.SetMainState(StatusManager::STATE_WAIT);
			Sleep(200);										// 画像処理タスクが初期化されるまでちょっと待ち
		}

		//// 資源開放
		if (-1 == SendCommand_Dispose())
		{
			ansRetc = -1;
			SendState_PcCanNot();
			goto ending;
		}
	}

ending:

	////////////////////////////////////
	// 最後の処理
	////////////////////////////////////
	// 統括へ結果を送信
	if (-1 != mode)
	{
		int			mailRetc;
		COMMON_QUE	sque;
		sque.nEventNo = FACT_TO_DEFECT_24;				// 途中停止アンサー
		sque.nLineNo = getlineid();
		sque.fl.data[0] = m_nCamPair;					// カメラペア=PcKind
		sque.fl.data[1] = getpcid();					// PcId
		sque.fl.data[2] = ansRetc;						// 結果
		sque.fl.data[3] = mode;							// 区分 (0:要求に対するアンサー 1:異常発生時)
		sque.fl.data[4] = m_emKindHt;

		mailRetc = send_mail(TO_DEFECT, TOKATU, &sque);
		if (0 != mailRetc) syslog(SYSNO_MAIL_SNED_ERR, "[途中停止アンサー err_code=%d]", mailRetc);
	}
	if (0 == ansRetc) { LOG(em_MSG), "[%s] 途中停止処理完了 <<<========================", my_sThreadName); }
	else { LOG(em_ERR), "[%s] 途中停止処理完了。結果=%d <<<========================", my_sThreadName, ansRetc); }

	// ステータス 初期状態
	if (-1 != ansRetc)	gcls_Status.SetMainState(StatusManager::STATE_INIT);
}


//------------------------------------------
// カメラGCP結果の定周期チェックを開始
//------------------------------------------
void MainInctance::GcpCheck_StartTimer()
{
	long nInterval = GetPrivateProfileInt(HT_DEFECT, "GCP_CHECK_TIMER", DEFAULT_GCP_CHECK_INTERVAL, TASKINI_NAME);	// チェック間隔[sec]

	// 初回だけ、すぐに実施する
	__int64 ts = (10 * 1000) * -10000;			// 1ミリ秒→100ナノ秒 (マイナス指定で、指定時間経過でシグナル) (第3引数は2回目以降の起動間隔)
	SetWaitableTimer(m_evGcpCheck, (LARGE_INTEGER *)&ts, (long long)nInterval * 1000LL, NULL, NULL, FALSE);
}


//------------------------------------------
// カメラGCP結果の定周期チェック
//------------------------------------------
void MainInctance::GcpCheck()
{
	// 共有メモリに値はセットされている？
	if (!gcls_pData->IsFirstInfoExists())
	{
		LOG(em_ERR), "[%s] 共有メモリにステータス情報なし", my_sThreadName);
		return;
	}

	FIRST_SHMEM_INFOv1* pSmemH = (FIRST_SHMEM_INFOv1*)gcls_pData->GetMapPointer();	// 共有メモリの先頭アドレス = カメラ情報の位置

	// 担当するカメラセット分だけチェックする
	for (int cam = 0; cam < pSmemH->camnum; cam++)
	{
		int camid = pSmemH->camid[cam];
		char* gcp_result = pSmemH->serial_result[cam].Gcp;
		bool  bIsOk = true;

		char cIniKey[16];	// Iniファイルから取得するためのキー
		char cGcpKey[32];	// Iniファイルから取得した値(GCP項目)
		char cGcpVal[32];	// Iniファイルから取得した値(OKとする文字列)
		for (int key = 1; key <= GCP_CHECK_KEY_MAX; key++)
		{
			//----------------------------------------
			// Iniファイルからキーを取得
			memset(cIniKey, 0x00, sizeof(cIniKey));
			sprintf(cIniKey, "GCP_KEY_%d", key);
			
			memset(cGcpKey, 0x00, sizeof(cGcpKey));
			GetPrivateProfileString(HT_DEFECT, cIniKey, "", cGcpKey, sizeof(cGcpKey), TASKINI_NAME);

			memset(cIniKey, 0x00, sizeof(cIniKey));
			sprintf(cIniKey, "GCP_VAL_%d", key);

			memset(cGcpVal, 0x00, sizeof(cGcpVal));
			GetPrivateProfileString(HT_DEFECT, cIniKey, "", cGcpVal, sizeof(cGcpVal), TASKINI_NAME);

			// いずれか未定義なら、ここで終了
			if (0 == strlen(cGcpKey) || 0 == strlen(cGcpVal)) break;

			// チェック対象のGCP項目確定
			LOG(em_MSG), "[%s] <cam%d> GCPチェック項目%d個目 [%s: %s]", my_sThreadName, camid, key, cGcpKey, cGcpVal);


			// gcp結果から対象の行を抜き出す
			char cTargetLine[64];
			memset(cTargetLine, 0x00, sizeof(cTargetLine));

			if (!FindLine(gcp_result, cGcpKey, cTargetLine))
			{
				LOG(em_ERR), "[%s] <cam%d> GCP結果に[%s]が見つからない", my_sThreadName, camid, cGcpKey);
				continue;
			}

			LOG(em_MSG), "[%s] <cam%d> GCP結果に[%s]を発見! [%s]", my_sThreadName, camid, cGcpKey, cTargetLine);

			char* p = strstr(cTargetLine, cGcpVal);

			if (NULL != p)
			{
				LOG(em_MSG), "[%s] <cam%d> [%s]のチェック結果：OK", my_sThreadName, camid, cGcpKey);
			}
			else
			{
				LOG(em_ERR), "[%s] <cam%d> [%s]のチェック結果：NG", my_sThreadName, camid, cGcpKey);
				syslog(140, "[cam%d][%s]", camid, cTargetLine);

				bIsOk = false;
			}
		}

		// 全てのキーをチェックし終えたら、機器状態を判定&シスログ出力
		if (bIsOk)
		{
			LOG(em_MSG), "[%s] <cam%d> GCPチェックOK", my_sThreadName, camid);

			char cStatusKey[32];
			memset(cStatusKey, 0x00, sizeof(cStatusKey));
			sprintf(cStatusKey, "CAM_GCP_ERR_%d_ID", camid);
			KizuFunction::SetStatus(cStatusKey, true, false);
		}
		else
		{
			LOG(em_ERR), "[%s] <cam%d> GCPチェックNG", my_sThreadName, camid);

			char cStatusKey[32];
			memset(cStatusKey, 0x00, sizeof(cStatusKey));
			sprintf(cStatusKey, "CAM_GCP_ERR_%d_ID", camid);
			KizuFunction::SetStatus(cStatusKey, false, false);
		}
	}
}

//------------------------------------------
// GCP結果から指定の行を抽出
// const char*	src				: GCP結果			
// const char*	target			: 抽出するキー
// char*		retStr			: targetが含まれる行の文字列
// const char*	delimiter		: 改行文字
//------------------------------------------
bool MainInctance::FindLine(const char* src, const char* target, char* retStr, const char* delimiter)
{
	const char* start = strstr(src, target);

	// 探したい文字列がヒットしない
	if (NULL == start)	return false;

	const char* end = strstr(start, delimiter);

	// start以降に改行文字がない
	if (NULL == end) return false;

	int length = end - start;

	memset(retStr, 0x00, length + 1);
	memcpy(retStr, start, length);

	return true;
}