#include "StdAfx.h"
#include "MainInctance.h"

#include "TI_RPISAVE.h"			// 画面の部品に直接アクセスするため


#include "DummyFrame.h"

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
ThreadManager("MI")
{
	// シグナル生成
	m_evThStart  = CreateEvent(NULL, FALSE, FALSE, NULL);				// メインスレッド同期用


	// 初期化 キュー
	gque_pUnion				= NULL;
	gque_pFrame				= NULL;
	gque_pComp				= NULL;

	for(int ii=0; ii<MAX_INS; ii++) {
		gque_pSockRecv[ii]	= NULL;
		gque_pCoilC[ii] = NULL;
		for(int jj=0; jj<MAX_COLOR_NUM; jj++ )	gque_pRpiWrite[ii][jj] = NULL;
	}

	// 初期化 クラス
	gcls_pImgBufProc = NULL;
	gcls_pRpiCProc = NULL;
	gcls_pDispProc = NULL;
	for(int ii=0; ii<MAX_INS; ii++) {
		gcls_InsLog[ii] = NULL;
		for(int jj=0; jj<NUM_CAM_POS; jj++) {gcls_pSock[ii][jj] = NULL; }
		gcls_pSyncProc[ii] = NULL;
		gcls_pCoilProc[ii] = NULL;
		for(int jj=0; jj<MAX_COLOR_NUM; jj++ )	gcls_pRpiWProc[ii][jj] = NULL;
	}
	gcls_InsLog_ImgBuf_Union = NULL;
	for(int ii=0; ii<MAX_THREAD_MULTI_P; ii++) gcls_pUnionProc[ii] = NULL;
	for(int ii=0; ii<MAX_THREAD_MULTI_P; ii++) gcls_pCompProc[ii] = NULL;

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
	CString sWk;
	int		nWk;

	//// メインインスタンスのみ、スレッドだけは作っておく (すぐに非シグナルとする)
	//// 継承元クラス実行 (メイン制御スレッドの為、ちょっと優先度高め)
	ThreadManager::Start(THREAD_PRIORITY_ABOVE_NORMAL );

	// スレッドIDが割りつくまでちょっと休憩
	while( 0==GetTid() ) {
		LOG(em_MSG), "[%s] スレッドID割り当て中・・・", my_sThreadName);
		Sleep(1);			// スレッド起動までちょっと待ち
	}


	////////////////////////////////////
	// 初期化
	//// パラメータ読込待ち
	gcls_pParam = new ParamManager();
	gcls_pParam->SetLogMgr(mcls_pLog);
	while( true ) {
		if( gcls_pParam->ReadParam_All() ) break;

		
		LOG(em_MSG), "[%s] パラメータ読込失敗(DB接続異常？！)。リトライ中・・・", my_sThreadName);
		Sleep(10000);
	}


	//// 機器状態初期化
	gcls_Status.SetLogMgr(mcls_pLog);
	gcls_Status.SetParamMgr(gcls_pParam);


	////////////////////////////////////
	////  初期化
	ParamManager::P_MAIN		const& prm  = gcls_pParam->GetMain();
	ParamManager::P_MAIN::INS   const& prmI	= gcls_pParam->GetMain().ins[0];
	int nInsNum = prm.nIns;
	
	if( prmI.nCamsetNum > NUM_CAM_POS ){
		CString err;
		err.Format("[DB]prm.nCamSet(%d) >= [CNST]NUM_CAM_POS(%d)なのはおかしい", prmI.nCamsetNum, NUM_CAM_POS);
		MessageBox(NULL,err, "整合性チェック異常", MB_OK | MB_ICONEXCLAMATION);
	}

	//---------------------
	//// 名称セット
	CString		cTitle;
	CWnd*   cwnd		= ((CTI_RPISAVEApp*)AfxGetApp())->GetMainWnd();	// 使いやすいようにエイリアス
	cwnd->GetWindowTextA(cTitle);
	cTitle += " <";
	if( COMP_TYPE_JPEG==prmI.emCompType)	cTitle += "JPEG; ";	
	if( COMP_TYPE_LAC==prmI.emCompType )	cTitle += "LAC; ";	
	if( COMP_TYPE_RAW==prmI.emCompType )	cTitle += "RAW; ";	
	if( COMP_TYPE_BMP==prmI.emCompType )	cTitle += "BMP; ";	
	if( COMP_TYPE_NUC==prmI.emCompType )	cTitle += "NUC; ";	
	if( COMP_TYPE_LAC5==prmI.emCompType )	cTitle += "LAC5; ";	
	cTitle += ">";
	cwnd->SetWindowTextA(cTitle);

	//---------------------
	//// 領域確保

	gque_pFrame = new ThreadLoopQueueImpl<FrameDataManager>( prm.nLoopQueFrameSize );

	gque_pUnion = new ThreadSyncManagerImpl<FRAME_DATA_ARRAY>("QueUnion", prm.nLoopQueRecvSize, prm.nLoopQueRecvSize);
	gque_pUnion->SetLogMgr(mcls_pLog);

	gque_pComp  = new ThreadSyncManagerImpl<FrameDataManager>("QueComp", prm.nLoopQueFrameSize, prm.nLoopQueFrameSize);
	gque_pComp->SetLogMgr(mcls_pLog);


	//---------------------
	// 受信受渡キュー
	{
		CDummyQueueFree::InitMemory(prm.nFrameSizeX * prm.nFrameSizeY * MAX_IMAGE_CNT);

		for(int ii=0; ii<nInsNum; ii++) {
			gque_pSockRecv[ii] = new ThreadLoopQueueImpl<HT_RPI_FDATA>( prm.nLoopQueRecvSize );
			// 領域確保
			for(int jj=0; jj<prm.nLoopQueRecvSize; jj++) {
				static const int n[1] = { prm.nFrameSizeX * prm.nFrameSizeY * MAX_IMAGE_CNT};		// キューサイズ

				HT_RPI_FDATA* p = new HT_RPI_FDATA;
				p->Alloc( n );
				gque_pSockRecv[ii]->AddFree(p);			// 器を追加
			}
		}
		int nMemSize = ((__int64)nInsNum * prm.nFrameSizeX * prm.nFrameSizeY * MAX_IMAGE_CNT * prm.nLoopQueRecvSize)/1024/1024;
		LOG(em_MSG),"[%s] 受信キューサイズ [%dMB]", my_sThreadName, nMemSize);
	}

	for(int ii=0; ii<nInsNum; ii++) {
		gque_pCoilC[ii]		= new ThreadQueueImpl<FRAME_DATA_ARRAY>();	
		for(int jj=0; jj<MAX_COLOR_NUM; jj++ ) {
			gque_pRpiWrite[ii][jj]	= new ThreadQueueImpl<DELIVERY_RPIWRITE>();
		}
	}

	//---------------------
	// フレーム画像連結後用
	int  nAryWk[8];				// 0～1 は、固定。
								// 2 圧縮後の1フレームのデータサイズ(割り切れる数)
								// 3 書込みモード (0:同期書込みモード 1:セクタ書込みモード)
								// 4 メモリ管理関数ポインタ
								// 5 同上
	nWk = prm.nBmpHeadSize + (prm.nFrameSizeX * prm.nFrameSizeY * NUM_CAM_POS); //物理的な最大とする//(prm.nMaxWidth * prm.nMaxHeight);			// 生連結画像サイズ (生連結画像の先頭には BMPヘッダーを付与)
	nAryWk[0] = GET_SIZE_JUST(nWk, 32);									// 32バイト アライメントの領域確保のため
	nAryWk[1] =	1;														// カメラ角度数(=1固定)
	nWk = prm.nBmpHeadSize + RPIFrameSizeInfo::LINK_SIZE_Cf + (prm.nMaxWidth * prm.nMaxHeight);
	if(1 == prm.nAddFrameInfoType) {
		nWk += sizeof(RpiLib::T_RPI_FRAME_INFO_T1);	
	}
	nAryWk[2] = GET_SIZE_JUST(nWk, 512);
	nAryWk[3] =1;
	// GPU版LAC メモリ管理定義
	gcls_pCudaAlloc = new CudaAlloc(&gcls_Lac);
	memcpy(&nAryWk[4], &gcls_pCudaAlloc, sizeof(void *));
	//nAryWk[4] = 0;
	//nAryWk[5] = 0;

	gque_pFrame->QueueAlloc(gque_pFrame->GetSemCount(), nAryWk); 

	LOG(em_MSG), "[%s] 回転寿司形式のキューを確保 [%dMB]", my_sThreadName, ((__int64)nAryWk[0]*nAryWk[1]*gque_pFrame->GetSemCount())/1024/1024 );




	////////////////////////////////////
	//// ワーカースレッド 初期化


	// 画像表示クラス
	gcls_ImgWin.SetThreadName("ImgWin");
	gcls_ImgWin.SetLogMgr(mcls_pLog);

	// HDD管理クラス
	gcls_Hdd.SetLogMgr(mcls_pLog);
	gcls_Hdd.SetParamMgr(gcls_pParam);
	gcls_Hdd.SetQueRpiMove(&gque_RpiMove);

	////////////////////////////////////
	//// 画像処理用

	for( int ii=0; ii<nInsNum; ii++ ){
		que_pSockRecv_Free.push_back( new CDummyQueueFree(gque_pSockRecv[ii]->gIQue_pFree) );
	}

	// 受信クラス
	for(int ii=0; ii<nInsNum; ii++) {
		for(int jj=0; jj<NUM_CAM_POS; jj++) {
			gcls_pSock[ii][jj] = new SockOneToAnyRecv();
			gcls_pSock[ii][jj]->SetLogMgr(mcls_pLog);
			gcls_pSock[ii][jj]->SetTLQue((ThreadLoopQueue<SOCK_F_HEAD2>*) gque_pSockRecv[ii]);	// 同じものを渡す

			sWk.Format("HT_RPI_%d_%d", ii, jj);
			gcls_pSock[ii][jj]->SetSession(sWk);
		}
	}
	// 幅方向紐付けクラス
	for(int ii=0; ii<nInsNum; ii++) {
		gcls_pSyncProc[ii] = new ImgSyncProc(ii);
		gcls_pSyncProc[ii]->SetLogMgr(mcls_pLog);
		gcls_pSyncProc[ii]->SetDetailLog( gcls_InsLog[ii] );
		gcls_pSyncProc[ii]->SetStatusMgr(&gcls_Status);
		gcls_pSyncProc[ii]->SetParamMgr(gcls_pParam);

		
		gcls_pSyncProc[ii]->SetQueFree(que_pSockRecv_Free[ii]);
		gcls_pSyncProc[ii]->SetQueIn(gque_pSockRecv[ii]->gIQue_pPool);
		gcls_pSyncProc[ii]->SetQueOut(gque_pCoilC[ii]);

	}
	// コイル管理クラス
	for(int ii=0; ii<nInsNum; ii++) {
		gcls_pCoilProc[ii] = new CoilControlProc(ii);
		gcls_pCoilProc[ii]->SetLogMgr(mcls_pLog);
		gcls_pCoilProc[ii]->SetParamMgr(gcls_pParam);
		gcls_pCoilProc[ii]->SetQueIn(gque_pCoilC[ii]);
		gcls_pCoilProc[ii]->SetQueOut(gque_pUnion->gIQue_pExec);
		gcls_pCoilProc[ii]->SetQueRecvFree(que_pSockRecv_Free[ii]);
	}


	// 幅方向カメラ同期クラス (並列処理)
	for(int ii=0; ii<MAX_THREAD_MULTI_P; ii++) gcls_pUnionProc[ii] = NULL;
	for(int ii=0; ii<prm.nThreadUnionNum; ii++) {
		gcls_pUnionProc[ii] = new ImgUnionProc(ii);
		gcls_pUnionProc[ii]->SetLogMgr(mcls_pLog);
		gcls_pUnionProc[ii]->SetDetailLog( gcls_InsLog_ImgBuf_Union );
		gcls_pUnionProc[ii]->SetStatusMgr(&gcls_Status);
		gcls_pUnionProc[ii]->SetParamMgr(gcls_pParam);
		gcls_pUnionProc[ii]->SetQueIn(gque_pUnion->gIQue_pExec);
		gcls_pUnionProc[ii]->SetQueOut(gque_pUnion->gIQue_pEnd);
		gcls_pUnionProc[ii]->SetQueFrame(gque_pFrame);
		gcls_pUnionProc[ii]->SetQueComp(gque_pComp);		// キュー個数参照のためだけ
	}

	// フレーム画像 バッファリング管理クラス
	gcls_pImgBufProc = new ImgBufProc();
	gcls_pImgBufProc->SetLogMgr(mcls_pLog);
	gcls_pImgBufProc->SetDetailLog(gcls_InsLog_ImgBuf_Union);
	gcls_pImgBufProc->SetParamMgr(gcls_pParam);
	gcls_pImgBufProc->SetQueIn(gque_pUnion->gIQue_pEnd);
	gcls_pImgBufProc->SetQueOut(gque_pComp->gIQue_pExec);
	gcls_pImgBufProc->SetQueOutSkip(&gque_Disp);

	gcls_pImgBufProc->SetQueRecvFree( &que_pSockRecv_Free[0]);
	gcls_pImgBufProc->Alloc();


	// 画像圧縮クラス (並列処理)
	KizuFunction::SetStatus(gcls_pParam->GetMain().nStateId, true, false);		// 機器状態正常
	for(int ii=0; ii<MAX_THREAD_MULTI_P; ii++) gcls_pCompProc[ii] = NULL;
	for(int ii=0; ii<prm.nThreadCompNum; ii++) {
		gcls_pCompProc[ii] = new ImgCompProc(ii);
		gcls_pCompProc[ii]->SetLogMgr(mcls_pLog);
		gcls_pCompProc[ii]->SetParamMgr(gcls_pParam);
		gcls_pCompProc[ii]->SetQueIn(gque_pComp->gIQue_pExec);
		gcls_pCompProc[ii]->SetQueOut(gque_pComp->gIQue_pEnd);
	}



	// RPIファイル書込クラス
	for(int ii=0; ii<nInsNum; ii++) {
		for(int jj=0; jj<prm.nImgNumTotal; jj++ ) {
			gcls_pRpiWProc[ii][jj] = new RpiWriteProc(ii, jj);
			gcls_pRpiWProc[ii][jj]->SetLogMgr(mcls_pLog);
			gcls_pRpiWProc[ii][jj]->SetParamMgr(gcls_pParam);
			gcls_pRpiWProc[ii][jj]->SetQueIn(gque_pRpiWrite[ii][jj]);
			gcls_pRpiWProc[ii][jj]->SetQueOut(&gque_Disp);
			gcls_pRpiWProc[ii][jj]->SetQueRpiMove(&gque_RpiMove);
		}
	}

	// RPIファイル制御クラス
	gcls_pRpiCProc = new RpiControlProc();
	gcls_pRpiCProc->SetLogMgr(mcls_pLog);
	gcls_pRpiCProc->SetParamMgr(gcls_pParam);
	gcls_pRpiCProc->SetRpiWProc(gcls_pRpiWProc);
	gcls_pRpiCProc->SetQueIn(gque_pComp->gIQue_pEnd);
	gcls_pRpiCProc->SetQueOut(gque_pRpiWrite);
	gcls_pRpiCProc->Init();


	// 結果表示クラス
	gcls_pDispProc = new DispProc();
	gcls_pDispProc->SetLogMgr(mcls_pLog);
	gcls_pDispProc->SetParamMgr(gcls_pParam);
	gcls_pDispProc->SetImgWin(&gcls_ImgWin);
	gcls_pDispProc->SetQueIn(&gque_Disp);
	gcls_pDispProc->SetQueOut(gque_pFrame->gIQue_pFree);


	////////////////////////////////////
	// ワーカースレッド 起動

	//// キュー
	gque_pUnion->Start();
	gque_pComp->Start();

	//// クラス
	gcls_Status.StartLock();


	//// メインループ
	for(int ii=0; ii<nInsNum; ii++) {
		for(int jj=0; jj<NUM_CAM_POS; jj++) {
			gcls_pSock[ii][jj]->StartLock(THREAD_PRIORITY_TIME_CRITICAL);	

		}
	}
	for(int ii=0; ii<nInsNum; ii++) {
		gcls_pSyncProc[ii]->StartLock();
		gcls_pCoilProc[ii]->StartLock();
	}
	gcls_pImgBufProc->StartLock();
	for(int ii=0; ii<MAX_THREAD_MULTI_P; ii++) {
		if(NULL == gcls_pUnionProc[ii]) continue;
		gcls_pUnionProc[ii]->StartLock(THREAD_PRIORITY_TIME_CRITICAL);
	}
	for(int ii=0; ii<MAX_THREAD_MULTI_P; ii++) {
		if(NULL == gcls_pCompProc[ii]) continue;
		gcls_pCompProc[ii]->StartLock();//THREAD_PRIORITY_LOWEST
	}
	gcls_pRpiCProc->StartLock();
	for(int ii=0; ii<nInsNum; ii++) {
		for(int jj=0; jj<prm.nImgNumTotal; jj++) gcls_pRpiWProc[ii][jj]->Start(THREAD_PRIORITY_LOWEST);
	}
	gcls_pDispProc->StartLock();

	// 他
	gcls_Hdd.Start();

	// 完了
	gcls_Status.SetMainSt(StatusManager::STATE_INIT);

	return TRUE;
}

//------------------------------------------
// 終了処理 (一度のみ起動)
//------------------------------------------
void MainInctance::Exit()
{

	ParamManager::P_MAIN  const& prm = gcls_pParam->GetMain();
	int nInsNum = prm.nIns;

	////////////////////////////////////
	// ワーカースレッド 停止

	// キュー
	gque_pUnion->Stop();
	gque_pComp->Stop();

	// クラス停止
	gcls_Status.Stop();
	gcls_pParam->Stop();
	gcls_ImgWin.Dispose();		// 念のため
	gcls_Hdd.Stop();

	// メインループ
	gcls_pImgBufProc->Stop();
	gcls_pImgBufProc->Free();
	gcls_pRpiCProc->Stop();
	gcls_pRpiCProc->ClosingRpi();
	gcls_pDispProc->Stop();
	for(int ii=0; ii<nInsNum; ii++) {
		gcls_pCoilProc[ii]->Stop();
		gcls_pSyncProc[ii]->Stop();

		for(int jj=0; jj<NUM_CAM_POS; jj++) {
			gcls_pSock[ii][jj]->Stop();
		}
		for(int jj=0; jj<prm.nImgNumTotal; jj++) {
			gcls_pRpiWProc[ii][jj]->Stop();
		}
	}

	for(int ii=0; ii<MAX_THREAD_MULTI_P; ii++) {
		if(NULL == gcls_pUnionProc[ii]) continue;
		gcls_pUnionProc[ii]->Stop();
	}
	for(int ii=0; ii<MAX_THREAD_MULTI_P; ii++) {
		if(NULL == gcls_pCompProc[ii]) continue;
		gcls_pCompProc[ii]->Stop();
	}


	////////////////////////////////////
	//// ワーカースレッド 解放
	delete gcls_pImgBufProc;
	delete gcls_pRpiCProc;
	delete gcls_pDispProc;
	for(int ii=0; ii<nInsNum; ii++) {
		delete gcls_pCoilProc[ii];
		delete gcls_pSyncProc[ii];
		for(int jj=0; jj<NUM_CAM_POS; jj++) { SAFE_DELETE( gcls_pSock[ii][jj] ); }
		for(int jj=0; jj<prm.nImgNumTotal; jj++) delete gcls_pRpiWProc[ii][jj];
	}
	for(int ii=0; ii<MAX_THREAD_MULTI_P; ii++) {
		if(NULL == gcls_pUnionProc[ii]) continue;
		delete gcls_pUnionProc[ii];
	}
	for(int ii=0; ii<MAX_THREAD_MULTI_P; ii++) {
		if(NULL == gcls_pCompProc[ii]) continue;
		delete gcls_pCompProc[ii];
	}


	////////////////////////////////////
	// 器開放
	// 受信受渡キュー
	{
		//int nCnt = 0;

		int nCntPool = 0;
		int nCntFree = 0;

		HT_RPI_FDATA* p = NULL;
		for(int ii=0; ii<nInsNum; ii++) {
			// 具有キュー
			while(true) {
				p = gque_pSockRecv[ii]->GetPool();
				if( NULL == p) break;
				p->Free();
				delete p;
				p = NULL;
				nCntPool ++;
			}
			// 空きキュー
			while(true) {
				p = gque_pSockRecv[ii]->GetFree();
				if( NULL == p) break;
				p->Free();
				delete p;
				p = NULL;
				nCntFree ++;
			}
			delete gque_pSockRecv[ii];
			LOG(em_MSG), "[%s]<%d> キュー解放 具有り=%d, 具無し=%d", my_sThreadName, ii, nCntPool, nCntFree);
		}
	}
	gque_pFrame->QueueFree();
	LOG(em_MSG), "[%s] 回転寿司形式のキューを解放", my_sThreadName);

	// 領域
	for(int ii=0; ii<nInsNum; ii++) {
		delete gque_pCoilC[ii];	
		for(int jj=0; jj<MAX_COLOR_NUM; jj++ ) {
			delete gque_pRpiWrite[ii][jj];
		}
	}
	delete gque_pFrame;
	delete gque_pUnion;
	delete gque_pComp;


	for( int ii=0; ii< (int)que_pSockRecv_Free.size(); ii++){
		delete  que_pSockRecv_Free[ii];
	}
	que_pSockRecv_Free.clear();
	// クラス
	delete gcls_pParam;

}

//------------------------------------------
// スレッド開始前処理 (スレッド実行直後(シグナルセット前)にコールバック)
//------------------------------------------
int MainInctance::ThreadFirst()
{
	////// 他ワークスレッドの準備が整うまで待機 (MainInctanceのみ)
	LOG(em_MSG), "[%s] 他ワークスレッドの準備が整うまで待機中・・・ [0x%X]", my_sThreadName, GetTid() );
	WaitForSingleObject(m_evThStart, INFINITE);

//*/
	//// イベント設定
	HANDLE hArray[] = { mque_DeliMail.g_evSem
						};
	SetEventNum(sizeof(hArray)/sizeof(HANDLE), hArray);
//*/
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
	enum {	EV_MAIL = 0,				// メールスロット 通知 (メールは一番最後にしようかな)
	};

	////// シグナル条件分け
	switch (nEventNo) {

//-----------------------------------------------------------------------------------------------
	case EV_MAIL: 						// メールスロット 通知
		pMail = mque_DeliMail.GetFromHead();
		switch(pMail->emNo) {

//-----------------------------------------------------------
		case E_DEF_ML_START :									// 検査開始要求 
			DetectStart( &pMail->que );
			break;

//-----------------------------------------------------------
		case E_DEF_ML_STOP :									// 検査停止要求
			DetectStop(0, 0);
			break;


//-----------------------------------------------------------
		case E_DEF_ML_NEXTCOIL :								// 次コイル情報

			if(true) {
				ParamManager::P_MAIN  const& prm = gcls_pParam->GetMain();
				int nMen = pMail->que.mix.idata[0];		// -1:全部

				//// コイル情報無しで動いてるのであれば、PC立ち上げて途中参加なので、自動的に開始する
				if( ! gcls_Status.IsRun() && -1 != prm.nExecCoilStart) {
					LOG(em_WAR), "[%s] 未起動状態でコイル情報受信！強制開始", my_sThreadName);	

					// 検査開始
					COMMON_QUE que;
					memset(&que, 0x00, sizeof(que));
					que.nEventNo   = FACT_TI_RPISAVE_01;
					que.nLineNo	   = getlineid();
					send_mail(getmytaskname(), ".", &que);
				} 
				
				//// ズレ有り時、再同期
				//if( gcls_Status.IsRun() && 1 == prm.nExecReSync ) {
				if( gcls_Status.IsRun() ) {
					int nIns = gcls_pParam->GetMain().nIns;

					for(int ii=0; ii<nIns; ii++) {
						ParamManager::P_MAIN::INS   const& prmI	= gcls_pParam->GetMain().ins[ii];
						if( -1 != nMen && prmI.nMen != nMen ) continue;	// 対象外

						if( ! gcls_Status.GetFrameSyncSt(ii) ) {
							int nFno = gcls_pSyncProc[ii]->GetRecvFrameNo(0);
							if(0 != nFno) nFno += MAX_RESYNC_FNO;
							LOG(em_WAR), "[%s] フレームズレ有り！再同期開始 Ins=%d Fno=%d", my_sThreadName, ii, nFno);

							gcls_pSyncProc[ii]->Reset();
							gcls_Status.ResetFrameSyncSt(ii, nFno);
						}
					}
				}
			}

			//// 通常のコイル情報受信
			if(true) {
				int nMen = pMail->que.mix.idata[0];		// -1:全部

				int ii_s, ii_e;
				GetInsLoop(-1, ii_s, ii_e);
				for(int ii=ii_s; ii<=ii_e; ii++) {
					ParamManager::P_MAIN::INS   const& prmI	= gcls_pParam->GetMain().ins[ii];
					if( -1 != nMen && prmI.nMen != nMen ) continue;	// 対象外

					gcls_pCoilProc[ii]->SetNextCoil(&pMail->que);
				}
			}

			break;


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
// シーケンス制御

//------------------------------------------
// 検査開始処理
// COMMON_QUE* que 起動要因
// 戻り値 復帰情報
//------------------------------------------
int MainInctance::DetectStart(COMMON_QUE* que)
{
	int		nRetcAnser	= 0;							// 結果 (0:正常 -1>:重故障 1<:軽故障)
	DWORD	nFno = que->fl.data[0];						// 再同期用Fno
	int		nIns = gcls_pParam->GetMain().nIns;

	////////////////////////////////////
	// 状態チェック
	if( StatusManager::STATE_INIT != gcls_Status.GetMainSt() ) {
		LOG(em_ERR), "[%s] [%s] 時に検査開始要求受信", my_sThreadName, gcls_Status.GetMainStName() );
		return -1;
	}


	////////////////////////////////////
	// 前準備
	//// staticパラメータも含め 全パラメータを再取得
	if( ! gcls_pParam->ReadParam_All(true) ) {
		LOG(em_ERR), "[%s] static全パラメータ読込失敗(DB接続異常？！)。", my_sThreadName);
		nRetcAnser = -10;
//		goto ending;
	}
	gcls_Status.ResetFrameSyncAllSt();

	////////////////////////////////////
	// ネットワーク接続待ち
	//		繋がらないからといって別に何もしない。最大リトライ時間分だけ待っておく

	// 一度すべて切断しておく
	LOG(em_MSG), "[%s] 判定回線を一度すべて切断実施", my_sThreadName);
	for(int ii=0; ii<nIns; ii++) {
		for(int jj=0; jj<NUM_CAM_POS; jj++) {
			//gcls_pSock[ii][jj]->CloseAll(true);			// スレッドで回線切断が実行中に重なると多重開放で異常終了してしまう
			gcls_pSock[ii][jj]->SetEvCloseReq(); 
		}
	}
	Sleep(1000);

	// 接続待ち
	for(int ii=0; ii<nIns; ii++) {
		int nReTry = 5;

		while( nReTry ) {
			int nConnectNum = 0;

			for(int jj=0; jj<NUM_CAM_POS; jj++) {
				if( SockOneToAnyRecv::STATE_NONE != gcls_pSock[ii][jj]->GetSockState(0) ) {
					nConnectNum++;
				}
			}

			// 待ち
			if(NUM_CAM_POS == nConnectNum) break;
			Sleep(1000);
			nReTry--;
		}
	}


	////////////////////////////////////
	// 共通部分の開始処理
	// ResetやInitなど
	for(int ii=0; ii<nIns; ii++) {
		gcls_Status.ResetFrameSyncSt(ii, nFno);
		gcls_pSyncProc[ii]->Reset();
		gcls_pSyncProc[ii]->SetIsExec(true);
		gcls_pCoilProc[ii]->Reset();
		gcls_pCoilProc[ii]->SetIsExec(true);

		for(int jj=0; jj<MAX_COLOR_NUM; jj++ ) {
			if(NULL == gcls_pRpiWProc[ii][jj]) continue;		
			gcls_pRpiWProc[ii][jj]->Reset();
			gcls_pRpiWProc[ii][jj]->SetIsExec(true);
		}
	}
	for(int ii=0; ii<MAX_THREAD_MULTI_P; ii++) {
		if(NULL == gcls_pUnionProc[ii]) continue;
		gcls_pUnionProc[ii]->Reset();
		gcls_pUnionProc[ii]->SetIsExec(true);
	}
	gcls_pImgBufProc->Reset();
	gcls_pImgBufProc->SetIsExec(true);
	for(int ii=0; ii<MAX_THREAD_MULTI_P; ii++) {
		if(NULL == gcls_pCompProc[ii]) continue;
		gcls_pCompProc[ii]->Reset();
		gcls_pCompProc[ii]->SetIsExec(true);
	}
	gcls_pDispProc->Reset();
	gcls_pDispProc->SetIsExec(true);

	gcls_pRpiCProc->Init();


	//////////////////////////////////////
	//// End処理
	
	// ステータス変更
	gcls_Status.SetMainSt(StatusManager::EM_MAIN_STATE::STATE_RUN);
	LOG(em_MSG), "[%s] 検査開始完了", my_sThreadName);

ending:

	//// 異常により、解放手続き
	//if( 0 != nRetcAnser ) {
	//	// スタートしたものをストップしておく
	//	for(ii=0; ii<MAX_THREAD_MULTI_P; ii++) {
	//		if(NULL == gcls_pCompProc[ii]) continue;
	//		gcls_pCompProc[ii]->SetIsExec(false);
	//	}
	//	gcls_pDispProc->SetIsExec(false);
	//}

	return nRetcAnser;
}

//------------------------------------------
// 検査停止処理
// int mode 要求状態 (0:正常な終了要求  1:何かおかしくて強制的に終了要求 -1:送信しない)
// int ansRetc 結果 (0:正常 1:軽故障 -1:重故障) ※ 処理中に、重故障が発生した場合は、重故障になる
// 戻り値 復帰情報
//------------------------------------------
int MainInctance::DetectStop(int mode, int ansRetc)
{
	int		nRetcAnser	= ansRetc;						// 結果 (0:正常 -1>:重故障 1<:軽故障)
	int		nInsNum		= gcls_pParam->GetMain().nIns;


	////////////////////////////////////
	// 前準備

	//// 現ステータス確認
	if( StatusManager::STATE_NON	== gcls_Status.GetMainSt() ||
		StatusManager::STATE_INIT	== gcls_Status.GetMainSt() ) {
		LOG(em_ERR), "[%s] [%s] 時に検査停止要求受信", my_sThreadName, gcls_Status.GetMainStName() );
		return -1;
	}
	LOG(em_MSG), "[%s] 検査停止要求。(%s) ==============>>>", my_sThreadName, 0 == mode ? "正常停止要求" : "異常による強制停止要求");  



	////////////////////////////////////
	// 共通部分の停止処理

	for(int ii=0; ii<MAX_THREAD_MULTI_P; ii++) {
		if(NULL == gcls_pUnionProc[ii]) continue;
		gcls_pUnionProc[ii]->SetIsExec(false);
	}
	gcls_pImgBufProc->SetIsExec(false);
	for(int ii=0; ii<MAX_THREAD_MULTI_P; ii++) {
		if(NULL == gcls_pCompProc[ii]) continue;
		gcls_pCompProc[ii]->SetIsExec(false);
	}

	gcls_pRpiCProc->ClosingRpi();

	for(int ii=0; ii<nInsNum; ii++) {
		gcls_pSyncProc[ii]->SetIsExec(false);
		gcls_pSyncProc[ii]->Reset();
		
		gcls_pCoilProc[ii]->SetIsExec(false);
		gcls_pCoilProc[ii]->Reset();
		for(int jj=0; jj<MAX_COLOR_NUM; jj++ ) {
			if(NULL == gcls_pRpiWProc[ii][jj]) continue;
			gcls_pRpiWProc[ii][jj]->SetIsExec(false);
		}
	}
	gcls_pDispProc->SetIsExec(false);


	////////////////////////////////////
	// End処理
	////////////////////////////////////

	//// ステータス変更
	gcls_Status.SetMainSt(StatusManager::STATE_INIT);

	return nRetcAnser;

	return 0;
}