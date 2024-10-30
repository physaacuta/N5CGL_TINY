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
ThreadManager("MI")
{
	// シグナル生成
	m_evThStart  = CreateEvent(NULL, FALSE, FALSE, NULL);				// メインスレッド同期用

	// 同期シグナル (タイマー)
	m_evTimeReStart		= CreateWaitableTimer(NULL, FALSE, NULL);		// 強制検査再起動待ちタイマー
	m_evTimeInitStart	= CreateWaitableTimer(NULL, FALSE, NULL);		// カメラリンクスイッチ設定アンサー待ちタイマー
	m_evTimeWaitStart	= CreateWaitableTimer(NULL, FALSE, NULL);		// 検査開始 判定アンサー待ちタイマー

	// 同期シグナル (トラッキング)
	m_evTr_Ken = CreateEvent(NULL, FALSE, FALSE, NULL);					// Tr 検査基点WPD～基準面検査装置位置までのトラッキング
	m_evTr_KenGate = CreateEvent(NULL, FALSE, FALSE, NULL);				// Tr ゲート信号ON～検査基点までのトラッキング
	m_evTr_DspGate = CreateEvent(NULL, FALSE, FALSE, NULL);				// Tr ゲート信号ON～表示基点までのトラッキング

	// WPDゲート信号ON～WPD信号ONの間trueになるフラグ
	m_bIsWpdGateTrKen = false;
	m_bIsWpdGateTrDsp = false;
	
	// 前回送信したコマンド情報を初期化
	memset(m_strCmd, 0x00, sizeof(m_strCmd));

	// 論理フレームNo初期化
	for (int ii = 0; ii<NUM_MEN; ii++) {
		for (int jj = 0; jj<NUM_CAM_POS; jj++) {
			m_nFrameNo_V[ii][jj] = 0;
		}
	}

	// コマンドリストクリア
	LogicParaRunFunc::ClearLinkswCmd();
}
//------------------------------------------
// デストラクタ
//------------------------------------------
MainInstance::~MainInstance(void)
{
	// シグナル開放
	CloseHandle(m_evThStart);
	CloseHandle(m_evTimeReStart);
	CloseHandle(m_evTimeInitStart);
	CloseHandle(m_evTimeWaitStart);
	CloseHandle(LogicParaRunFunc::GetEvTimeCamset());
	CloseHandle(LogicParaRunFunc::GetEvTimeResumeStart());
	for (int ii = 0; ii < NUM_ALL_HTPARA; ii++) {
		CloseHandle(LogicParaRunFunc::GetEvTimeResumeWait(ii));
	}
	CloseHandle(m_evTr_Ken);
	CloseHandle(m_evTr_KenGate);
	CloseHandle(m_evTr_DspGate);
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
	if( ! mque_DeliMail.AddToTailFreeCheck(pMail) ) {
		delete pMail;
		LOG(em_ERR), "[%s] mque_DeliMail キューフル!!", my_sThreadName);
	}
}

//------------------------------------------
// 初期化処理 (一度のみ起動)
//------------------------------------------
BOOL MainInstance::Init()
{
	CString cWk;
	bool    bWk;

	//// メインインスタンスのみ、スレッドだけは作っておく (すぐに非シグナルとする)
	//// 継承元クラス実行 (メイン制御スレッドの為、ちょっと優先度高め)
	ThreadManager::Start(THREAD_PRIORITY_ABOVE_NORMAL );

	// スレッドIDが割りつくまでちょっと休憩
	while( 0==GetTid() ) {
		LOG(em_MSG), "[%s] スレッドID割り当て中・・・", my_sThreadName);
		Sleep(1);			// スレッド起動までちょっと待ち
	}


	////////////////////////////////////
	//// ワーカースレッド 初期化
	//// ※ 特別じゅうよう ※
	////////////////////////////////////

	//// パラメータ管理クラス
	gcls_pParam = new ParamManager();
	gcls_pParam->SetLogMgr(mcls_pLog);
	while (true) {
		if (gcls_pParam->ReadParam_All()) break;

		LOG(em_MSG), "[%s] パラメータ読込失敗(DB接続異常？)。リトライ中･･･", my_sThreadName);
		Sleep(10000);
	}

	// グループ毎の必要判定PC台数を取得（対象は判定PCのみ）
	gcls_pParam->Read_HtGroupNum(DIV_GROUP_MAX - 1);
	int nNumHt[DIV_GROUP_MAX - 1];
	gcls_pParam->GetHtGroupNum(nNumHt);
	LOG(em_MSG), "[%s] 判定PC台数 表面：<Group1:%d><Group2:%d> 裏面：<Group3:%d><Group4:%d>",
		my_sThreadName, nNumHt[0], nNumHt[1], nNumHt[2], nNumHt[3]);

	// コンストラクタ呼び出しの為のインスタンス化（一度だけ）
	LogicParaRunFunc objLogicParaRunFunc(mcls_pLog, &gcls_Status, &gcls_Plg);
	ToLinkswUtil objToLinkswUtil(mcls_pLog, &gcls_Status);
		
	// ロジックパララン用カメラセット情報セット
	// ※前回値を設定
	LogicParaRunFunc::SetHtParaCamNo(&gcls_pParam->GetCommonMast().nLogiParaCam[0][0]);

	//// フレームNoトラッキングクラス
	gcls_TrFrm.Alloc(MAX_TR_FRAME);


	//// ステータスログクラス
	gcls_pStatusFunc = new StatusLogFunc();
	gcls_pStatusFunc->SetLogMgr(mcls_pLog);
	gcls_pStatusFunc->SetQueAddSQL((ThreadQueue<DELIVERY_DB>*)&gcls_DB.gque_AddSQLData);	// 依存関係が循環になってしまうため、型を同じ構造体の奴にキャスト
	gcls_pStatusFunc->Alloc();

	// 前工程情報管理クラス
	gcls_Mae.SetLogMgr(mcls_pLog);

	//// ステータス制御クラス
	gcls_Status.SetLogMgr(mcls_pLog);
	gcls_Status.SetParamMgr(gcls_pParam);
	gcls_Status.SetStatusFunc(gcls_pStatusFunc);
	gcls_Status.SetKadou( DIV_KADOU_INIT );
	gcls_Status.SetDspSyslog(true);
	gcls_Status.SetAddStlog(true);

	gcls_Status.Start();			// 機器IDを取得したい為、最初に動かしておく
	gcls_Status.SetStatusRead();	// 機器IDを取得したい為、即時読み込み


	////////////////////////////////////
	//// ワーカースレッド 初期化
	////////////////////////////////////
	
	//// DB登録制御クラス
	gcls_DB.SetLogMgr(mcls_pLog);
	gcls_DB.SetStatusMgr(&gcls_Status);
	gcls_DB.Alloc();


	//// PIO管理クラス
	// 今回は無し

	//// コイル情報管理クラス
	gcls_pCoil = new CoilManager();
	gcls_pCoil->SetLogMgr(mcls_pLog);
	gcls_pCoil->SetLogMgrA(mcls_pLogA);
	gcls_pCoil->SetStatusMgr(&gcls_Status);
	gcls_pCoil->SetParamMgr(gcls_pParam);
	gcls_pCoil->SetDbMgr(&gcls_DB);
	gcls_pCoil->SetEdgePosCont(&gcls_EdgePosCont);
	gcls_pCoil->SetMaeMgr(&gcls_Mae);
	gcls_pCoil->SetQueAddSQL(&gcls_DB.gque_AddSQLData);
	gcls_pCoil->SetDistPostion(gcls_pParam->GetParamCommon().nDistPosition);
	gcls_pCoil->SetDmemMgr(&gcls_Dmem);

	//// 操業状態管理クラス
	gcls_pSogyo = new SogyoManager("SOGYO_MODE");
	gcls_pSogyo->SetLogMgr(mcls_pLog);
	gcls_pSogyo->SetStatusMgr(&gcls_Status);

	//// エッジ検出整合性管理クラス
	gcls_EdgePosCont.SetLogMgr(mcls_pLog);
	gcls_EdgePosCont.SetParamMgr(gcls_pParam);
	gcls_EdgePosCont.SetFrameSkipAddDB(true);										// スキップ個数をDBに登録
	gcls_EdgePosCont.SetOldMode(false);												// 昔と同じ機能の場合は、True
	gcls_EdgePosCont.SetQueAddSQL(&gcls_DB.gque_AddSQLData);

	//// データメモリ通信管理クラス
	gcls_Dmem.SetLogMgr(mcls_pLog);
	gcls_Dmem.SetLogMgrA(mcls_pLogA);
	gcls_Dmem.SetParamMgr(gcls_pParam);
	//gcls_Dmem.SetCoilMgr(gcls_pCoil);
	gcls_Dmem.SetStatusMgr(&gcls_Status);
	//gcls_Dmem.SetPioMgr(&gcls_Pio);
	gcls_Dmem.SetQueAddSQL(&gcls_DB.gque_AddSQLData);
	//gcls_Dmem.SetAlarmeMgr(&gcls_Arm);

	// シーケンサ通信受信クラス
	gcls_pSeqFrameR = new SeqFrameRecv();
	gcls_pSeqFrameR->SetLogMgr(mcls_pLog);
	gcls_pSeqFrameR->SetLogSeqMgr(mcls_pLogSeq);
	gcls_pSeqFrameR->SetStatusMgr(&gcls_Status);

	//// エッジ検出範囲指定クラス
	gcls_EdgeRestSd.SetLogMgr(mcls_pLog);
	gcls_EdgeRestSd.SetParamMgr(gcls_pParam);
	gcls_EdgeRestSd.SetStatusMgr(&gcls_Status);

	//// PLG管理クラス
	gcls_Plg.SetLogMgr(mcls_pLog);
	gcls_Plg.SetLockMgr(&gcls_Lock);
	gcls_Plg.SetStatusMgr(&gcls_Status);
	gcls_Plg.SetParamMgr(gcls_pParam);
	gcls_Plg.SetCoilMgr(gcls_pCoil);
	gcls_Plg.SetSeqFrameR(gcls_pSeqFrameR);
	gcls_Plg.SetSogyoMgr(gcls_pSogyo);
	gcls_Plg.SetEdgePosCont(&gcls_EdgePosCont);
	gcls_Plg.SetDmemMgr(&gcls_Dmem);
	gcls_Plg.SetEdgeRestMgr(&gcls_EdgeRestSd);
	gcls_Plg.SetDistPostion(gcls_pParam->GetParamCommon().nDistPosition);
	gcls_Plg.SetDistLen((DWORD)gcls_pParam->GetParamCommon().dDistLen);
#ifdef DSP_POS_REV_OPERATION
	gcls_Plg.SetPioMgr(&gcls_Pio);		// 鹿島独自機能のため
#endif
	//gcls_Plg.SetLogMMgr(mcls_pLogMark);
	gcls_Plg.SetTrFrmMgr(&gcls_TrFrm);
	gcls_Plg.Alloc();

	// 警報出力管理クラス
	gcls_Arm.SetLogMgr(mcls_pLog);
	gcls_Arm.SetLogMgrA(mcls_pLogA);
	gcls_Arm.SetLockMgr(&gcls_Lock);
	gcls_Arm.SetStatusMgr(&gcls_Status);
	gcls_Arm.SetParamMgr(gcls_pParam);
	gcls_Arm.SetCoilMgr(gcls_pCoil);
	gcls_Arm.SetPlgMgr(&gcls_Plg);
	gcls_Arm.SetLogGMgr(mcls_pLogG);
	gcls_Arm.SetQue_Pio(&gcls_Dmem.mque_AddData);		// カット指示、減速指示 PO出力用
	
	// 出側物理コイル単位の状態管理用トラッキングクラス (必ずPLGクラスの初期化後)
	gcls_pTlCoilOutSt = new TrackingList(gcls_Plg.Get_pLenKenDist());
	gcls_pCoil->SetTrackingList(gcls_pTlCoilOutSt);

	//// 疵情報ファイル管理クラス
	setlocale(LC_ALL, "Japanese");	// これが無いと日本語パスを含むフォルダの場合、保存できない。(ライブラリの中で実行するべきか迷っているが、変更するとタスク全体に影響するので、とりあえずここで設定)
	CString sNameKizu;

	for (int ii = 0; ii<NUM_MEN; ii++) {
		// 疵CSV出力ｸﾗｽ
		sNameKizu = GetDefectDumpName(ii);
		// (入側単位)
		//gcls_pDefDump[ii] = new DefectDumpManager(sNameKizu, SIZE_COIL_BUF, 100);
		// (出側単位)
		gcls_pDefDump[ii] = new DefectDumpManager(ii, sNameKizu, SIZE_COIL_OUT_BUF, 100);

		gcls_pDefDump[ii]->SetLogMgr(mcls_pLog);

	}

	//// シーケンサ用通信制御クラス(通常伝文)

	//// 判定⇒統括間通信管理クラス (0:表,1:裏)
	for (int ii = 0; ii<NUM_MEN; ii++) {
#ifdef ENABLE_CYCLE
		if(0==ii)	cWk = INI_CY_RECV_TOP;
		else		cWk = INI_CY_RECV_BOT;
		if(ii==gcls_pParam->GetParamCommon().nDistPosition) bWk = true;
		else												bWk = false;

		gcls_pCyRecv[ii] = new CyRecv(cWk, ii, bWk);
		gcls_pCyRecv[ii]->SetLogMgr(mcls_pLog);
		gcls_pCyRecv[ii]->SetLogMgrD(mcls_pLogD);
		gcls_pCyRecv[ii]->SetLockMgr(&gcls_Lock);
		gcls_pCyRecv[ii]->SetStatusMgr(&gcls_Status);
		gcls_pCyRecv[ii]->SetParamMgr(gcls_pParam);
		gcls_pCyRecv[ii]->SetCoilMgr(gcls_pCoil);
		gcls_pCyRecv[ii]->SetPlgMgr(&gcls_Plg);
		gcls_pCyRecv[ii]->SetDbMgr(&gcls_DB);

		gcls_pCyRecv[ii]->SetQueAddSQL(&gcls_DB.gque_AddSQLData);
		gcls_pCyRecv[ii]->SetDumpMgr(gcls_pDefDump[ii]);
		gcls_pCyRecv[ii]->SetTrFrmMgr(&gcls_TrFrm);

#endif

		if(0==ii)	cWk = INI_HT_RECV_TOP;
		else		cWk = INI_HT_RECV_BOT;
		if(ii==gcls_pParam->GetParamCommon().nDistPosition) bWk = true;
		else												bWk = false;

		gcls_pHtRecv[ii] = new HtRecv(cWk, ii, bWk);
		gcls_pHtRecv[ii]->SetLogMgr(mcls_pLog);
		//gcls_pHtRecv[ii]->SetLogMMgr(mcls_pLogMark);
		gcls_pHtRecv[ii]->SetLogMgrD(mcls_pLogD);
		gcls_pHtRecv[ii]->SetLogMgrA(mcls_pLogA);
		gcls_pHtRecv[ii]->SetLockMgr(&gcls_Lock);
		gcls_pHtRecv[ii]->SetStatusMgr(&gcls_Status);
		gcls_pHtRecv[ii]->SetParamMgr(gcls_pParam);
		gcls_pHtRecv[ii]->SetCoilMgr(gcls_pCoil);
		gcls_pHtRecv[ii]->SetPlgMgr(&gcls_Plg);
		gcls_pHtRecv[ii]->SetDbMgr(&gcls_DB);
#ifndef DISABLE_GAP_CALC
		gcls_pHtRecv[ii]->SetGapInfMgr(gcls_pGap); 
#endif
#ifndef DISABLE_TINY_CALC
		gcls_pHtRecv[ii]->SetTinyMgr(gcls_pTiny[ii]); 
#endif
		gcls_pHtRecv[ii]->SetEdgePosCont(&gcls_EdgePosCont); 

		//gcls_pHtRecv[ii]->SetCountMgr(gcls_pCount[ii]);


		//gcls_pHtRecv[ii]->SetMarkMgr(gcls_pMark[ii]);

		gcls_pHtRecv[ii]->SetQueAddSQL(&gcls_DB.gque_AddSQLData);
		gcls_pHtRecv[ii]->SetQueDmem(&gcls_Dmem.mque_AddData);
		gcls_pHtRecv[ii]->SetDumpMgr(gcls_pDefDump[ii]);
		gcls_pHtRecv[ii]->SetOneFrameLen( gcls_pParam->CalcResV(DIV_PLG_LINE)*SIZE_FRAME_IMG_Y );
		gcls_pHtRecv[ii]->SetTrFrmMgr(&gcls_TrFrm);
		gcls_pHtRecv[ii]->SetArmMgr(&gcls_Arm);

#ifdef ENABLE_CYCLE
		gcls_pHtRecv[ii]->SetQueCycleData(&gcls_pCyRecv[ii]->gque_Deli);
#endif
	}

	//// 統括→表示 送信クラス
	for(int ii=0;ii<HY_SOCK_NUM; ii++) {
		CString sWk;
		sWk.Format("%s_%d", INI_OP_SEND, ii+1);
		gcls_pOpS[ii] = new KizuOpSend(sWk);
		gcls_pOpS[ii]->SetLogMgr(mcls_pLog);
	}

	//// 統括→表示 送信情報生成モジュール
	gcls_OpSend.SetLogMgr(mcls_pLog);
	for(int ii=0; ii<HY_SOCK_NUM; ii++)gcls_OpSend.SetQueOpSend(&gcls_pOpS[ii]->gque_Deli, ii);
	gcls_OpSend.SetCsLockCoil(gcls_pCoil->GetCsLock());
	gcls_OpSend.SetStatusMgr(&gcls_Status);
	gcls_OpSend.SetParamMgr(gcls_pParam);
	gcls_OpSend.SetCoilMgr(gcls_pCoil);
	gcls_OpSend.SetPlgMgr(&gcls_Plg);
	for(int ii=0; ii<NUM_MEN; ii++)	gcls_OpSend.SetHtRecv(ii, gcls_pHtRecv[ii]);
	
	//// トラッキング管理クラス (必ずPLGクラスの初期化後)
	gcls_pTrKenDistPos = new Tracking(gcls_pParam->GetCommonMast().Dist.nKenBase, gcls_Plg.Get_pLenKenDist(), &m_evTr_Ken);

	//// iPortViewerコマンド受付クラス
	gcls_IPort.SetLogMgr(mcls_pLog);
	gcls_IPort.SetStatusMgr(&gcls_Status);

	// カメラリンクスイッチ用 シリアル通信速度変更クラス
	gcls_CamLinkSpdMgr.SetLogMgr(mcls_pLog);
	gcls_CamLinkSpdMgr.SetStatusMgr(&gcls_Status);

#ifdef ENABLE_RECORDER_UDP
	gcls_CmdSender.SetLogMgr(mcls_pLog);
#endif

	Sleep(100);

	////////////////////////////////////
	//// ワーカースレッド 起動
	////////////////////////////////////
	gcls_pParam->Start();
	gcls_DB.Start();
	gcls_Mae.Start();
	gcls_pCoil->Start();

	gcls_pSogyo->Start();
	gcls_Plg.Start(THREAD_PRIORITY_TIME_CRITICAL);
	gcls_Arm.Start();

#ifndef DISABLE_TINY_CALC
	for (int ii = 0; ii<NUM_MEN; ii++)	gcls_pTiny[ii]->Start();
#endif
	for (int ii = 0; ii<NUM_MEN; ii++)	gcls_pHtRecv[ii]->Start();
#ifdef ENABLE_CYCLE
	for (int ii = 0; ii<NUM_MEN; ii++)	gcls_pCyRecv[ii]->Start();
#endif

	for (int ii = 0; ii<HY_SOCK_NUM; ii++)gcls_pOpS[ii]->Start();
	gcls_pSeqFrameR->Start();

	gcls_Dmem.Start();

	gcls_EdgeRestSd.Start();

	gcls_pTrKenDistPos->Start();

	gcls_IPort.Start();

	for (int ii = 0; ii<NUM_MEN; ii++) {
		gcls_pDefDump[ii]->Start();
	}

	gcls_CamLinkSpdMgr.Start();

#ifdef ENABLE_RECORDER_UDP
	gcls_CmdSender.Start();
#endif

	// 他スレッドがちゃんと起動するまでちょっと待ち
	Sleep(50);

	return TRUE;
}

//------------------------------------------
// 終了処理 (一度のみ起動)
//------------------------------------------
void MainInstance::Exit()
{

	//// ワーカースレッド 停止
#ifndef DISABLE_TINY_CALC
	for (int ii = 0; ii<NUM_MEN; ii++)		gcls_pTiny[ii]->Stop();
#endif
	for (int ii = 0; ii<NUM_MEN; ii++)		gcls_pHtRecv[ii]->Stop();
#ifdef ENABLE_CYCLE
	for (int ii = 0; ii<NUM_MEN; ii++)		gcls_pCyRecv[ii]->Stop();
#endif
	gcls_pCoil->Stop();
	for (int ii = 0; ii<HY_SOCK_NUM; ii++)gcls_pOpS[ii]->Stop();
	gcls_pSeqFrameR->Stop();
	gcls_pSogyo->Stop();

	gcls_pTrKenDistPos->Stop();
	gcls_pTlCoilOutSt->QueAllFree();
	gcls_pTlCoilOutSt->Stop();
	gcls_pParam->Stop();


	//// 単一クラスの停止
	gcls_Arm.Stop();
	gcls_Plg.Stop();
	gcls_Mae.Stop();
	gcls_DB.Stop();
	gcls_DB.Free();
	gcls_Status.Stop();
	gcls_EdgeRestSd.Stop();
	gcls_Dmem.Stop();
	gcls_IPort.Stop();

	for (int ii = 0; ii<NUM_MEN; ii++) {
		gcls_pDefDump[ii]->Stop();
	}

	gcls_CamLinkSpdMgr.Stop();

#ifdef ENABLE_RECORDER_UDP
	gcls_CmdSender.Stop();
#endif

	//// ワーカースレッド 開放
#ifndef DISABLE_TINY_CALC
	for (int ii = 0; ii<NUM_MEN; ii++) delete gcls_pTiny[ii];
#endif
	for (int ii = 0; ii<NUM_MEN; ii++) delete gcls_pHtRecv[ii];
#ifdef ENABLE_CYCLE
	for (int ii = 0; ii<NUM_MEN; ii++) delete gcls_pCyRecv[ii];
#endif
	for (int ii = 0; ii<MAX_TR_FRAME; ii++) gcls_TrFrm.DelAll(ii);
	gcls_TrFrm.Free();

	delete gcls_pCoil;
	for (int ii = 0; ii<HY_SOCK_NUM; ii++)delete gcls_pOpS[ii];
	delete gcls_pSeqFrameR;
	delete gcls_pSogyo;

	delete gcls_pTrKenDistPos;
	delete gcls_pTlCoilOutSt;
	delete gcls_pParam;
	delete gcls_pStatusFunc;

	//// ログ関係
	for (int ii = 0; ii<NUM_MEN; ii++) {
		delete gcls_pDefDump[ii];
	}
}

//------------------------------------------
// スレッド開始前処理 (スレッド実行直後(シグナルセット前)にコールバック)
//------------------------------------------
int MainInstance::ThreadFirst()
{
	////// 他ワークスレッドの準備が整うまで待機 (MainInstanceのみ)
	LOG(em_MSG), "[%s] 他ワークスレッドの準備が整うまで待機中・・・ [0x%X]", my_sThreadName, GetTid() );
	WaitForSingleObject(m_evThStart, INFINITE);


	//////////////////////////////////////////////////////////////////////
	// 統括管理の準備完了ができたので、処理開始

	//// ステータス初期化
	for(int ii=0; ii<NUM_MEN; ii++){
		gcls_Status.SetSys(ii, DIV_SYS_IDLE);
#ifdef ENABLE_CYCLE
		gcls_Status.SetCycleSys(ii, DIV_SYS_IDLE);
#endif
	}
	// ----------------------
	// パララン機能システム状態変更
	for (int ii = 0; ii < NUM_ALL_HTPARA; ii++)
	{
		gcls_Status.SetParaSys(ii, DIV_SYS_IDLE);
	}
	gcls_Status.SetKadou( DIV_KADOU_STOP );
	gcls_Status.SetUnten( DIV_UNTEN_NOMAL );
	gcls_Status.SetPlg( DIV_PLG_LINE );

	//// 判定状態問合せ
	Send_HtDefect_Status();

	//// 外部へ時刻設定
	ToOutMailSender::Send_KsMaster_TimeSet();

#ifndef DISABLE_AUTOSTART
	//// 自動探傷開始
	__int64 ts = (__int64)300 * 1000 * -10000;	// 1[ms]→100[ns] (マイナス指定で、指定時間経過でシグナル) (第3引数は2回目以降の起動間隔)
	SetWaitableTimer(m_evTimeReStart, (LARGE_INTEGER*)&ts, 0, NULL, NULL, FALSE);
	LOG(em_WAR), "[%s] 自動探傷開始まで暫くお待ち下さい。(約%d秒)", my_sThreadName, ts / (1000 * -10000));
#endif

	//// イベント設定
	HANDLE hArray[] = { 
						m_evTimeReStart,						// 強制検査再起動タイマー
						m_evTimeInitStart,						// カメラリンクスイッチ設定アンサー待ちタイムアウト
						m_evTimeWaitStart,						// 検査開始タイムアウト
						LogicParaRunFunc::GetEvTimeCamset(),	// カメラ選択設定アンサー待ちタイムアウト
						LogicParaRunFunc::GetEvTimeResumeStart(),// 途中参加設定アンサー待ちタイムアウト
						LogicParaRunFunc::GetEvTimeResumeWait(0),			// 途中開始判定アンサー待ちタイムアウト
						LogicParaRunFunc::GetEvTimeResumeWait(1),			// 途中開始判定アンサー待ちタイムアウト
						//m_evTimeJoinStart[0],					// 全長処理 途中参加タイムアウト(表)
						//m_evTimeJoinStart[1],					// 全長処理 途中参加タイムアウト(裏)

						gcls_Dmem.GetEvPI_WPD_KenBaseGateOn(),	// 検査基点WPD ゲート信号ON
						gcls_Dmem.GetEvPI_WPD_KenBase(),		// 検査基点WPD
						gcls_Dmem.GetEvPI_WPD_KenBaseGateOff(),	// 検査基点WPD ゲート信号OFF
						gcls_pCoil->GetNextCoilKen_Recv(),	// 検査用 次コイル情報受信完了

						gcls_Dmem.GetEvPI_WPD_DspBaseGateOn(),	// 表示基点WPD ゲート信号ON
						gcls_Dmem.GetEvPI_WPD_DspBase(),		// 表示基点WPD
						gcls_Dmem.GetEvPI_WPD_DspBaseGateOff(),	// 表示基点WPD ゲート信号OFF

						gcls_Dmem.GetEvPI_WPD_ShacutF(),		// ファーストカット信号 ON
						gcls_Dmem.GetEvPI_WPD_ShacutL(),		// ラストカット信号 ON
						gcls_Dmem.GetEvPI_WPD_PatoReset(),		// パトライト疵リセット信号 ON


						gcls_pTrKenDistPos->GetEv_TrEnd(),		// 基準面検査装置位置に溶接点/異種点到達

						gcls_Plg.GetEvPLG_KenNotPos(),			// 基準面と異なる方の検査装置位置に溶接点到達
						
						gcls_pOpS[0]->GetEvConnect(),			// 通信状態関連
						gcls_pOpS[1]->GetEvConnect(),			// 通信状態関連
						gcls_pOpS[2]->GetEvConnect(),			// 通信状態関連

						gcls_pHtRecv[0]->GetEvErrToClose(),		// 異常処理関係
						gcls_pHtRecv[1]->GetEvErrToClose(),

						gcls_Status.GetQueMyErrWar().g_evSem,	// エラー通知キュー

						gcls_pParam->GetEvPrm_ReadEnd(),		// パラメータ読込完了

						mque_DeliMail.g_evSem					// メールスロットキュー
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
	enum {
			EV_RESTART = 0,					// 強制検査再起動タイマー
			EV_WAIT_INIT_START,				// カメラリンクスイッチ設定アンサー待ちタイムアウト
			EV_DETECT_WAIT_START,			// 検査開始 判定アンサー待ちタイムアウト
			EV_WAIT_CAMSET,					// カメラリンクスイッチ設定アンサー待ちタイムアウト
			EV_WAIT_RESUME_START,			// 途中参加設定アンサー待ちタイムアウト
			EV_WAIT_RESUME_WAIT1,			// 途中開始判定アンサー待ちタイムアウト
			EV_WAIT_RESUME_WAIT2,			// 途中開始判定アンサー待ちタイムアウト
			//EV_RPI_JOIN_START_TOP,			// 全長処理 途中参加タイムアウト(表)
			//EV_RPI_JOIN_START_BOT,			// 全長処理 途中参加タイムアウト(裏)


			EV_PI_WPD_KEN_BASE_GATE,		// 検査基点WPD ゲート信号ON
			EV_PI_WPD_KEN_BASE,				// 検査基点WPD
			EV_PI_WPD_KEN_BASE_GATE_OFF,	// 検査基点WPD ゲート信号OFF
			EV_PI_NEXT_KEN_COILINF_RECV,	// 検査用次コイル情報受信完了

			EV_PI_WPD_DSP_BASE_GATE,		// 表示基点WPD ゲート信号ON
			EV_PI_WPD_DSP_BASE,				// 表示基点WPD
			EV_PI_WPD_DSP_BASE_GATE_OFF,	// 表示基点WPD ゲート信号OFF

			EV_PI_SHACUT_F,					// ファーストカット信号 ON
			EV_PI_SHACUT_L,					// ラストカット信号 ON
			EV_PI_PATO_RESET,				// パトライト疵リセット信号 ON

			EV_TR_KEN_BASE,					// 検査位置に溶接点/異種点到達

			EV_TR_KEN_NOTPOS,				// 基準面と異なる方の検査装置位置に溶接点到達

			EV_OP_CONNECT_1,				// オペレータ機能より接続有り
			EV_OP_CONNECT_2,				// オペレータ機能より接続有り
			EV_OP_CONNECT_3,				// オペレータ機能より接続有り

			EV_HT_T_ERRCLOSE,				// 判定 表面 異常切断
			EV_HT_B_ERRCLOSE,				// 判定 裏面 異常切断

			EV_OTHER_ONERROR,				// 何かしらの 異常発生
			EV_PARAM_READ_END,				// パラメータ読み込み完了通知

			EV_MAIL,						// メールスロット 通知 (メールは一番最後にしようかな)
	};

	////// シグナル条件分け
	switch (nEventNo) {
//-----------------------------------------------------------------------------------------------
	case EV_RESTART:									// 強制検査再起動タイマー
		// 既に検査中なら無処理
		if( ! gcls_Status.IsRun() ) {
			LOG(em_ERR), "[%s] 強制検査再起動開始", my_sThreadName);
			syslog(232, "");
			// データ生成
			COMMON_QUE que;
			que.fl.data[0] = gcls_Status.GetUnten();
			que.fl.data[1] = gcls_Status.GetPlg();
			que.fl.data[2] = gcls_Status.GetMenMode();
			que.fl.data[3] = gcls_pCoil->GetInitScode(0);
			que.fl.data[4] = gcls_pCoil->GetInitScode(1);
			que.fl.data[5] = gcls_pCoil->GetInitPcode(0);
			que.fl.data[6] = gcls_pCoil->GetInitPcode(1);
			que.fl.data[7] = LogicParaRunFunc::GetHtParaCamNo(0, 0);	// ロジックパララン機能01 入力カメラ01
			que.fl.data[8] = LogicParaRunFunc::GetHtParaCamNo(0, 1);	// ロジックパララン機能01 入力カメラ02
			que.fl.data[9] = LogicParaRunFunc::GetHtParaCamNo(1, 0);	// ロジックパララン機能02 入力カメラ01
			que.fl.data[10] = LogicParaRunFunc::GetHtParaCamNo(1, 1);	// ロジックパララン機能02 入力カメラ02

			// 検査開始通知を送る
			SetDeliveryMail(E_DEF_ML_START, &que);
		}
		break;

//-----------------------------------------------------------------------------------------------
	case EV_WAIT_INIT_START:							// カメラリンクスイッチ設定アンサー待ちタイムアウト
		LOG(em_ERR), "[%s] カメラリンクスイッチ設定アンサー待ちタイムアウト発生！！", my_sThreadName);

		// コマンドリストクリア
		LogicParaRunFunc::ClearLinkswCmd();

		// 検査開始初期化待ち処理(タイムアウト発生)
		WaitInit_TimeOut();
		break;

//-----------------------------------------------------------------------------------------------
	case EV_DETECT_WAIT_START:							// 検査開始 判定アンサー待ちタイムアウト
		LOG(em_ERR), "[%s] 検査開始設定アンサータイムアウト発生！", my_sThreadName);
		WaitStart_TimeOut();
		break;

//-----------------------------------------------------------------------------------------------
	case EV_WAIT_CAMSET:								// カメラ選択設定アンサー待ちタイムアウト
		LOG(em_ERR), "[%s] カメラ選択設定アンサー待ちタイムアウト発生！！", my_sThreadName);
		LogicParaRunFunc::WaitCamset_TimeOut();
		break;

//-----------------------------------------------------------------------------------------------
	case EV_WAIT_RESUME_START:							// 途中参加設定アンサー待ちタイムアウト
		LOG(em_ERR), "[%s] 途中参加設定アンサー待ちタイムアウト発生！！", my_sThreadName);
		LogicParaRunFunc::WaitResumeStart_Timeout();
		break;

//-----------------------------------------------------------------------------------------------
	case EV_WAIT_RESUME_WAIT1:							// 途中開始判定アンサー待ちタイムアウト
	case EV_WAIT_RESUME_WAIT2:							// 途中開始判定アンサー待ちタイムアウト
		if (true) {
			int nKind = nEventNo - EV_WAIT_RESUME_WAIT1;		// 検査開始区分（0：ロジックパララン機能1、1：ロジックパララン機能2）
			LOG(em_ERR), "[%s] 途中開始判定アンサー待ちタイムアウト発生！！ ロジックパララン機能%d", my_sThreadName, nKind + 1);
			LogicParaRunFunc::WaitResumeWait_Timeout(nKind);
		}
		break;

//-----------------------------------------------------------------------------------------------
	case EV_PI_WPD_KEN_BASE_GATE:						// 検査基点WPD ゲート信号ON
		if(true){
			// フラグをON
			m_bIsWpdGateTrKen = true;

			if( ! gcls_Status.IsDoSequence() )	break;			// シーケンス実行可能？

			// 連続ONは無視
			if( gcls_Plg.IsCoilChange_Ken() ){
				LOG(em_ERR), "[%s] 検査基点WPD ゲート信号ON [%0.3fm] <連続でONにより無効>", my_sThreadName, gcls_Plg.GetLen_KenDist()/1000.0 );
				syslog(267, "[検査用：連続入力 <%0.3fm未満で再ON>]", gcls_Plg.GetLen_KenDist()/1000.0);
				break;
			}

#ifndef LOCAL
			// 検査開始一本目、強制再探傷一本目、コイル長上限到達後のコイルは小コイル判定しない
			if( (0 == gcls_Status.GetStatusRead().Coil_Init.stat &&
				 0 == gcls_Status.GetStatusRead().Coil_ReStart.stat &&
				 0 == gcls_Status.GetStatusRead().MaxLen_Ken.stat ) && 
				COIL_LEN_MIN*1000 > gcls_Plg.GetLen_KenDist() + gcls_pParam->GetCommonMast().Dist.nKenBase ) {
				LOG(em_ERR), "[%s] 検査基点WPD ゲート信号ON [%0.3fm] <小コイル未満により無効>", my_sThreadName, gcls_Plg.GetLen_KenDist()/1000.0 );
				syslog(267, "[検査用：小コイル判定 <%0.3fm>]", gcls_Plg.GetLen_KenDist()/1000.0);
				break;
			}
#endif

			LOG(em_MSG), "[%s] 検査基点WPD ゲート信号ON [%0.3fm]", my_sThreadName, gcls_Plg.GetLen_KenDist()/1000.0);
			syslog(290, "[検査用:検査基点WPD ゲート信号ON: %0.3fm]", gcls_Plg.GetLen_KenDist() / 1000.0);
		}
		break;

//-----------------------------------------------------------------------------------------------
	case EV_PI_WPD_KEN_BASE:							// 検査基点WPD
		if(true){
			if( ! gcls_Status.IsDoSequence() )	break;			// シーケンス実行可能？

			// 連続ONは無視
			if( gcls_pTrKenDistPos->IsRunning() || gcls_Plg.IsCoilChange_Ken()){
				LOG(em_ERR), "[%s] 検査基点WPD ON [%0.3fm] <連続でONにより無効> Chg:%d", my_sThreadName, gcls_Plg.GetLen_KenDist()/1000.0, gcls_Plg.IsCoilChange_Ken() );
				syslog(267, "[検査用：WPD 連続入力 <%0.3fm未満で再ON>]", gcls_Plg.GetLen_KenDist()/1000.0);
				break;
			}
			// ゲート信号OFFは無視
#ifndef PIO_DEBUG
#ifdef ENABLE_GATE_KEN
			if( !gcls_Dmem.GetPI_KenGateYou() ) {
				LOG(em_ERR), "[%s] 検査基点WPD ON [%0.3fm] <ゲート信号OFFにより無効>", my_sThreadName, gcls_Plg.GetLen_KenDist()/1000.0 );
				syslog(267, "[検査用：ゲート信号OFF <%0.3fmでON>]", gcls_Plg.GetLen_KenDist()/1000.0);
				break;
			}
#endif
#endif

#ifndef LOCAL
			// 検査開始一本目、強制再探傷一本目、コイル長上限到達後のコイルは小コイル判定しない
			if( (0 == gcls_Status.GetStatusRead().Coil_Init.stat &&
				 0 == gcls_Status.GetStatusRead().Coil_ReStart.stat &&
				 0 == gcls_Status.GetStatusRead().MaxLen_Ken.stat ) && 
				COIL_LEN_MIN*1000 > gcls_Plg.GetLen_KenDist() + gcls_pParam->GetCommonMast().Dist.nKenBase) {
				LOG(em_ERR), "[%s] 検査基点WPD ON [%0.3fm] <小コイル未満により無効>", my_sThreadName, gcls_Plg.GetLen_KenDist()/1000.0 );
				syslog(267, "[検査用：小コイル判定 <%0.3fm>]", gcls_Plg.GetLen_KenDist()/1000.0);
				break;
			}
#endif

			// フラグをOFF
			m_bIsWpdGateTrKen = false;

			// 検査用WPD認識
			double dSpmDiff = 0.0;
			WPD_KenBase(0, &dSpmDiff);

			LOG(em_MSG), "[%s] 検査基点WPD ON 今=%0.3fm→%0.3fm [%0.3fm後に基準面到達] SPM伸率=%.4f DmFrameNo=%d PlgFrameNo=%d LineSpeed=%d", my_sThreadName,
				(double)gcls_Plg.GetLen_KenDist() / 1000.0,
				((double)gcls_Plg.GetLen_KenDist() + (double)gcls_pParam->GetCommonMast().Dist.nKenBase + dSpmDiff) / 1000.0,
				((double)gcls_pParam->GetCommonMast().Dist.nKenBase + dSpmDiff) / 1000.0,
				gcls_pCoil->GetLastSpm(),
				gcls_Dmem.GetDmRecv().ken.nWpdOnFrameNo,
				gcls_Plg.GetFrameNo(),
				gcls_pSogyo->GetSpeed());
			syslog(265, "[検査基点WPD: 今=%0.3fm→%0.3fm [%0.3fm後に基準面到達] SPM伸率=%.4f] DmFrameNo=,%d, PlgFrameNo=,%d, LineSpeed=,%d",
				(double)gcls_Plg.GetLen_KenDist() / 1000.0,
				((double)gcls_Plg.GetLen_KenDist() + (double)gcls_pParam->GetCommonMast().Dist.nKenBase + dSpmDiff) / 1000.0,
				((double)gcls_pParam->GetCommonMast().Dist.nKenBase + dSpmDiff) / 1000.0,
				gcls_pCoil->GetLastSpm(),
				gcls_Dmem.GetDmRecv().ken.nWpdOnFrameNo,
				gcls_Plg.GetFrameNo(),
				gcls_pSogyo->GetSpeed());
		}
		break;

//-----------------------------------------------------------------------------------------------
	case EV_PI_WPD_KEN_BASE_GATE_OFF:					// 検査基点WPD ゲート信号OFF
		if(true){
			// フラグをOFF
			bool bWk = m_bIsWpdGateTrKen;
			m_bIsWpdGateTrKen = false;


#ifdef ENABLE_GATE_KEN
			if( ! gcls_Status.IsDoSequence() )	break;			// シーケンス実行可能？
			syslog(290, "[検査用:検査基点WPD ゲート信号OFF: %0.3fm]", gcls_Plg.GetLen_KenDist()/1000.0);

			// 既に検査基点WPD ONしていれば無視
			//if( gcls_pTrKenDistPos->IsRunning() ){
			if (!bWk) {
				LOG(em_INF), "[%s] 検査基点WPD～ゲート信号OFF [%0.3fm] <検査基点WPD検出済につき検査WPDバックアップ不要>", my_sThreadName, gcls_Plg.GetLen_KenDist()/1000.0 );
				break;
			}

			// 検査用WPD認識
			double dSpmDiff = 0.0;
			WPD_KenBase(2, &dSpmDiff);

			LOG(em_MSG), "[%s] 検査基点WPD(GATE) OFF <検査WPDバックアップ> 今=%0.3fm→%0.3fm [%0.3fm後に基準面到達] SPM伸率=%.4f DmFrameNo=%d PlgFrameNo=%d LineSpeed=%d", my_sThreadName,
				(double)gcls_Plg.GetLen_KenDist() / 1000.0,
				((double)gcls_Plg.GetLen_KenDist() + (double)gcls_pParam->GetCommonMast().Dist.nKenBase + dSpmDiff) / 1000.0,
				((double)gcls_pParam->GetCommonMast().Dist.nKenBase + dSpmDiff) / 1000.0,
				gcls_pCoil->GetLastSpm(),
				gcls_Dmem.GetDmRecv().ken.nFrameNo,
				gcls_Plg.GetFrameNo(),
				gcls_pSogyo->GetSpeed());
			syslog(269, "[検査基点WPD～ゲート信号OFF <検査WPDバックアップ> 今=%0.3fm→%0.3fm [%0.3fm後に基準面到達] SPM伸率=%.4f DmFrameNo=,%d, PlgFrameNo=,%d, LineSpeed=,%d,]",
				(double)gcls_Plg.GetLen_KenDist() / 1000.0,
				((double)gcls_Plg.GetLen_KenDist() + (double)gcls_pParam->GetCommonMast().Dist.nKenBase + dSpmDiff) / 1000.0,
				((double)gcls_pParam->GetCommonMast().Dist.nKenBase + dSpmDiff) / 1000.0,
				gcls_pCoil->GetLastSpm(),
				gcls_Dmem.GetDmRecv().ken.nFrameNo,
				gcls_Plg.GetFrameNo(),
				gcls_pSogyo->GetSpeed());
#else
			LOG(em_MSG), "[%s] 検査基点WPD(GATE) OFF 今:%0.3fm", my_sThreadName, gcls_Plg.GetLen_KenDist() / 1000.0);
			syslog(290, "[検査用:検査基点WPD ゲート信号OFF: %0.3fm]", gcls_Plg.GetLen_KenDist() / 1000.0);
#endif
		}
		break;
		
//-----------------------------------------------------------------------------------------------
	case EV_PI_NEXT_KEN_COILINF_RECV:					// 検査用次コイル情報受信完了
		if(true) {
			LOG(em_MSG), "[%s] 検査用次コイル情報受信完了", my_sThreadName);
			SetParam_NextCoilInfRecv();
			// 既にトラッキング中の場合は表示PCに検査用コイル情報を送信する
			if( gcls_pTrKenDistPos->IsRunning() || gcls_Plg.IsCoilChange_Ken()){
				if(!gcls_pCoil->IsCoilNextKen()) break;
				////// コイル情報 を表示PCに送る 
				//// ※ 従来は、前工程情報を前もって取得するためにコイル情報受信時に送信していたが、
				//		検査管理Noをコイル情報受信毎に更新したいためWPD ONかつコイル情報受信済み時に送信する
				OpSendFunc::SendOp_CoilInf_Ken( gcls_pCoil->GetCoilNextKen() );
			}
		}
		break;

//-----------------------------------------------------------------------------------------------
	case EV_PI_WPD_DSP_BASE_GATE:						// 表示基点WPD ゲート信号ON
		if(true){
			// フラグをON
			m_bIsWpdGateTrDsp = true;

			if( ! gcls_Status.IsDoSequence() )	break;			// シーケンス実行可能？

			// 連続ONは無視
			if( gcls_Plg.IsCoilChange_Dsp() ){
				LOG(em_ERR), "[%s] 表示基点WPD ゲート信号ON [%0.3fm] <連続でONにより無効>", my_sThreadName, gcls_Plg.GetLen_BaseDsp()/1000.0 );
				syslog(292, "[表示用：連続入力 <%0.3fm未満で再ON>]", gcls_Plg.GetLen_BaseDsp()/1000.0);
				break;
			}

// 表示は小コイル判定なし
//#ifndef LOCAL
//			// 検査開始一本目、強制再探傷一本目、コイル長上限到達後のコイルは小コイル判定しない
//			if( (0 == gcls_Status.GetStatusRead().Coil_Init.stat &&
//				 0 == gcls_Status.GetStatusRead().Coil_ReStart.stat &&
//				 0 == gcls_Status.GetStatusRead().MaxLen_Dsp.stat ) && 
//				//COIL_LEN_MIN*1000 > gcls_Plg.GetLen_BaseDsp() ) {
//				LOG(em_ERR), "[%s] 検査基点WPD ゲート信号ON [%0.3fm] <小コイル未満により無効>", my_sThreadName, gcls_Plg.GetLen_BaseDsp()/1000.0 );
//				syslog(268, "[表示用：小コイル判定 <%0.3fm>]", gcls_Plg.GetLen_BaseDsp()/1000.0);
//				break;
//			}
//#endif
			LOG(em_MSG), "[%s] 表示基点WPD ゲート信号ON [%0.3fm]", my_sThreadName, gcls_Plg.GetLen_BaseDsp()/1000.0);
			syslog(292, "[表示用:表示基点WPD ゲート信号ON: %0.3fm]", gcls_Plg.GetLen_BaseDsp() / 1000.0);
		}
		break;

//-----------------------------------------------------------------------------------------------
	case EV_PI_WPD_DSP_BASE:							// 表示基点WPD
		if(true) {

			if( ! gcls_Status.IsDoSequence() ) break;							// シーケンス実行可能?
			
			// ゲート信号OFFは無視
#ifndef PIO_DEBUG
#ifdef ENABLE_GATE_DSP
			if( !gcls_Dmem.GetPI_DspGateYou() ) {
				LOG(em_ERR), "[%s] 表示基点WPD ON [%0.3fm] <ゲート信号OFFにより無効>", my_sThreadName, gcls_Plg.GetLen_BaseDsp()/1000.0 );
				syslog(268, "[表示用：ゲート信号OFF <%0.3fmでON>]", gcls_Plg.GetLen_BaseDsp()/1000.0);
				break;
			}
#endif
#endif

			// フラグをOFF
			m_bIsWpdGateTrDsp = false;

			LOG(em_MSG), "[%s] 表示基点WPD ON [%0.3fm]", my_sThreadName, gcls_Plg.GetLen_BaseDsp()/1000.0 );
			syslog(266, "[表示基点WPD: %0.3fm]", gcls_Plg.GetLen_BaseDsp()/1000.0 );

			// 表示用WPD認識
			WPD_DspBase();

		}
		break;

		
//-----------------------------------------------------------------------------------------------
	case EV_PI_WPD_DSP_BASE_GATE_OFF:					// 表示基点WPD ゲート信号OFF
		if(true){
			// フラグをOFF
			bool bWk = m_bIsWpdGateTrDsp;
			m_bIsWpdGateTrDsp = false;

#ifdef ENABLE_GATE_DSP
			if (!gcls_Status.IsDoSequence())	break;			// シーケンス実行可能？
			syslog(292, "[表示用:表示基点WPD ゲート信号OFF: %0.3fm]", gcls_Plg.GetLen_BaseDsp()/1000.0);

			// 既に検査基点WPD ONしていれば無視
			//if (gcls_pTrKenDistPos->IsRunning()) {
			if (!bWk) {
				LOG(em_INF), "[%s] 表示基点WPD～ゲート信号OFF [%0.3fm] <表示基点WPD検出済につき表示WPDバックアップ不要>", my_sThreadName, gcls_Plg.GetLen_BaseDsp() / 1000.0 );
				break;
			}
			
			// 表示用WPD認識
			WPD_DspBase();

			int nVal = gcls_pParam->GetCommonMast().Dist.nDspBaseKensaBot;
			LOG(em_MSG), "[%s] 表示基点WPD(GATE) OFF 今:%0.3fm→%0.3fm [%0.3fm後に表示基点位置到達] <表示WPDバックアップ>", my_sThreadName, gcls_Plg.GetLen_BaseDsp() / 1000.0, (gcls_Plg.GetLen_BaseDsp() + nVal) / 1000.0, nVal / 1000.0);
			syslog(270, "[表示基点WPD～ゲート信号OFF: %0.3fm後に表示基点位置に到達] <表示WPDバックアップ>", nVal / 1000.0);
#else
			LOG(em_MSG), "[%s] 表示基点WPD(GATE) OFF 今:%0.3fm", my_sThreadName, gcls_Plg.GetLen_BaseDsp() / 1000.0);
			syslog(292, "[表示用:表示基点WPD ゲート信号OFF: %0.3fm]", gcls_Plg.GetLen_BaseDsp() / 1000.0);
#endif
		}
		break;
				
//-----------------------------------------------------------------------------------------------
	case EV_PI_SHACUT_F:								// ファーストカット信号 ON
		if(true) {
			// 現在検査中であれば無視
			if( ! gcls_Status.IsRun() ) break;
			// 通常運転モードかシミュレーションモード以外なら無視
			if((DIV_UNTEN_NOMAL != gcls_Status.GetUnten() && DIV_UNTEN_SMYU != gcls_Status.GetUnten()) ) break;
			
			// 条件チェック
			if( NULL == gcls_pCoil->GetCoilDsp(POS_DSP_SHR_I) ) {
				LOG(em_WAR), "[MI] シャーカット位置までトラッキング未達時に、シャーカットON");
				break;
			}			

			// シャーカット認識
			if(true) {
				gcls_Lock.LockTh();	//// -------------->>>>>

				double dInLen  = gcls_Plg.GetLen_Dsp(POS_DSP_SHR_I);	// シャーカット位置での入側論理コイル距離
				double dOutLen = gcls_Plg.GetLen_OutCut();				// シャーカット位置での出側物理コイル距離
				COIL_INF const* typCoil = gcls_pCoil->GetCoilDsp(POS_DSP_SHR_I);

				// 出側物理単位の距離クリア
				gcls_Plg.CoilChange_OutCut();
	
				LOG(em_MSG), "[MI] ファーストカット ON <%.12s> %s [入側論理=%0.3fm 出側物理=%0.3fm]",
					typCoil->data.cCoilNoIn, typCoil->cKizukenNo, dInLen/1000.0, dOutLen/1000.0 );
				syslog(293, "[%.12s 入側論理=%0.3fm 出側物理=%0.3fm]", typCoil->data.cCoilNoIn, dInLen/1000.0, dOutLen/1000.0 );

				// コイル情報に登録
				gcls_pCoil->SetData_ShaCut( dInLen );

				// PLG管理クラスシャーカット認識
				gcls_Plg.CoilChange_FirstCut();

				gcls_Lock.UnlockTh();	//// <<<<<--------------
			}
		}
		break;
//-----------------------------------------------------------------------------------------------
	case EV_PI_SHACUT_L:								// ラストカット信号 ON
		if(true) {
			// 現在検査中であれば無視
			if( ! gcls_Status.IsRun() ) break;
			// 通常運転モードかシミュレーションモード以外なら無視
			if((DIV_UNTEN_NOMAL != gcls_Status.GetUnten() && DIV_UNTEN_SMYU != gcls_Status.GetUnten()) ) break;

			double dInLen = gcls_Plg.GetLen_Dsp(POS_DSP_SHR_I);		// シャーカット位置での入側論理コイル距離
			double dOutLen = gcls_Plg.GetLen_OutCut();				// シャーカット位置での出側物理コイル距離
			COIL_INF const* typCoil = gcls_pCoil->GetCoilDsp(POS_DSP_SHR_I);

			LOG(em_MSG), "[MI] ラストカット ON <%.12s> %s [入側論理=%0.3fm 出側物理=%0.3fm]",
				typCoil->data.cCoilNoIn, typCoil->cKizukenNo, dInLen / 1000.0, dOutLen / 1000.0 );
		}
		break;
//-----------------------------------------------------------------------------------------------
	case EV_PI_PATO_RESET:									// PIO パトライト疵リセット信号 ON
		gcls_Arm.Reset_Pato();
		syslog(296, "");
		break;

				
//-----------------------------------------------------------------------------------------------
	case EV_TR_KEN_BASE:								// 基準面検査装置位置に溶接点/異種点到達
		if(true){
			if( ! gcls_Status.IsDoSequence() )	break;			// シーケンス実行可能？

			gcls_Lock.LockTh();		//// ----------------->>>>>>



			long		nMode = (long)gcls_pTrKenDistPos->g_pAddr;		// 0:溶接点 1:異種点 2:溶接点(GATE)
			double	dLen  = gcls_Plg.GetLen_Ken(gcls_Plg.GetDistPos());

			CString sWk = (0==nMode?"溶接点":(1==nMode?"異種点":"溶接点バックアップ"));

			LOG(em_INF),  "===========================================================>>>>");
			LOG(em_INF),  "[%s] 論理コイル境界点 基準面検査装置位置通過！！ <%s> 前回コイル長[%0.3fm]", my_sThreadName, sWk, dLen/1000.0 );

			// コイル切り替え制御 (基準面)
			ChangeCoil_Ken( gcls_Plg.GetDistPos() );

			// コイル切替情報をセット
			if(2 == nMode)	gcls_pCoil->SetData_StartWpd(1);
			gcls_pCoil->SetData_CoilCnt();

//#ifdef DSP_NO_TRACKING		// リアルタイム表示の場合、ここで表示も切替
//			
//			WPD_DspBase();
//
//#endif

			gcls_Lock.UnlockTh();	//// <<<<<<-----------------
		}
		break;

//-----------------------------------------------------------------------------------------------
	case EV_TR_KEN_NOTPOS:								// 基準面と異なる方の検査装置位置に溶接点到達
		if(true){
			if( ! gcls_Status.IsDoSequence() )	break;			// シーケンス実行可能？

			gcls_Lock.LockTh();		//// -------------------------->>>>


			LOG(em_INF), "[%s] --------------", my_sThreadName);
			LOG(em_INF), "[%s] 論理コイル境界点 その他検査装置位置通過！！", my_sThreadName);

			// コイル切替制御
			ChangeCoil_Ken( gcls_Plg.GetDistNotPos() );

#ifdef DSP_NO_TRACKING		// リアルタイム表示の場合、ここで表示も切替
			
			WPD_DspBase();

#endif



			gcls_Lock.UnlockTh();	//// <<<<--------------------------
		}
		break;

//-----------------------------------------------------------------------------------------------
	case EV_OP_CONNECT_1:									// オペレータ機能より接続有り
	case EV_OP_CONNECT_2:
	case EV_OP_CONNECT_3:
		//// どこかから繋がった為、みんなに送信する
		// 現状態を送信
		OpSendFunc::SendOp_Status();
		OpSendFunc::SendOp_AlarmData(gcls_Arm.GetAlarmeData());
		
		// 稼動中の場合は、今のコイル情報を送信する
		if( gcls_Status.IsRun() && DIV_UNTEN_CAMERA != gcls_Status.GetUnten() ){
			const COIL_INF*		pCoilInf = gcls_pCoil->GetCoilBaseKen();
			OpSendFunc::SendOp_CoilInf_Ken( pCoilInf );
			MainInstance::Send_HyDefect_MaeRead(pCoilInf->cKizukenNo);
			if( pCoilInf->bDspOk ) OpSendFunc::SendOp_CoilInf_Dsp( pCoilInf );

			for (int jj = 0; jj<pCoilInf->nCutCount; jj++) {
				TO_SO_DATA_CUT  data;
				memset(&data, 0x00, sizeof(data));
				data.nMode = DIV_CUT_FCUT;
				data.nPos = pCoilInf->nCutLen[jj];
				OpSendFunc::SendOp_CutInf(pCoilInf->cKizukenNo, &data);
			}

			// 行列、行情報
			for(int ii=0; ii<NUM_MEN; ii++) {
				gcls_pHtRecv[ii]->SetEvOpNewConnect(); 
			}
			// 通板距離
			OpSendFunc::SendOp_Length();
		}
		break;
//-----------------------------------------------------------------------------------------------
	case EV_HT_T_ERRCLOSE:								// 判定 表面 異常切断
		LOG(em_ERR), "[%s] 判定 表面 異常切断[%s][%s][%d]", my_sThreadName,
			DivNameManager::GetDivKadou(gcls_Status.GetKadou()), gcls_Status.GetMenModeName(gcls_Status.GetMenMode()), gcls_Status.GetSys(0) );
		if( ! gcls_Status.IsRun() ) break;
		// 片面続行可能？
		if(gcls_Status.GetMenMode() && DIV_SYS_RUN == gcls_Status.GetSys(0) ) {
			DetectRun_Cancel(0);					// 表面検査停止
		} else {
			gcls_Status.SetStatusQue("HANTEI_TOP_ANS_ID", false);
			gcls_Status.RefreshStatus();			// 強制反映
			AllDetectStop(false, true);				// 検査停止してから検査強制再起動へ
		}
		break;

	case EV_HT_B_ERRCLOSE:										// 判定 裏面 異常切断
		LOG(em_ERR), "[%s] 判定 裏面 異常切断[%s][%s][%d]", my_sThreadName,
			DivNameManager::GetDivKadou(gcls_Status.GetKadou()), gcls_Status.GetMenModeName(gcls_Status.GetMenMode()), gcls_Status.GetSys(0) );
		if( ! gcls_Status.IsRun() ) break;
		// 片面続行可能？
		if(gcls_Status.GetMenMode() && DIV_SYS_RUN == gcls_Status.GetSys(1) ) {
			DetectRun_Cancel(1);					// 裏面検査停止
		} else {
			gcls_Status.SetStatusQue("HANTEI_BOT_ANS_ID", false);
			gcls_Status.RefreshStatus();			// 強制反映
			AllDetectStop(false, true);				// 検査停止してから検査強制再起動へ
		}
		break;

//-----------------------------------------------------------------------------------------------
	case EV_OTHER_ONERROR:										// 何かしらの 異常発生
		if(true) {
			StatusBaseManager::TYP_MY_ERRWAR_INF* pInf = gcls_Status.GetQueMyErrWar().GetFromHead();
			bool retc = gcls_Status.OnMyErrWar(pInf);
			delete pInf;

			// 重故障のため、検査停止
			if( retc && gcls_Status.IsRun() ) {
				gcls_Status.RefreshStatus();			// 強制反映
				AllDetectStop(false, true);				// 検査停止してから検査強制再起動へ
			}
		}
		break;

//-----------------------------------------------------------------------------------------------
	case EV_PARAM_READ_END:										// パラメータ読み込み完了通知
		// 今まで良くほしくなる時があったので、作ってみた
		if(true) {
			// DBクラスへのJpeg圧縮率変更のみ
			gcls_DB.SetJpgQuality(gcls_pParam->GetCommonMast().nJpegQuality ); 
			
			// 距離関係の更新
			gcls_Plg.SetDistLen( (long)gcls_pParam->GetParamCommon().dDistLen );

		}
		break;

//-----------------------------------------------------------------------------------------------
	case EV_MAIL: 						// メールスロット 通知
		pMail = mque_DeliMail.GetFromHead();
		switch(pMail->emNo) {

	//-----------------------------------------------------------
		case E_DEF_ML_PARAM:							// パラメータ変更要求
			if (true) {
				// パラメータ変更コマンド
				COMMON_QUE* pQue = new COMMON_QUE;
				memcpy( pQue, &pMail->que, sizeof(COMMON_QUE) );

				// SCODEを基準面検査位置を通過中のSCODEに書換
				if( CH_SCODE_NON == pQue->fl.data[2] || CH_SCODE_ALL == pQue->fl.data[2]) {
					int men = gcls_pCoil->GetDistPos();
					pQue->fl.data[2] = gcls_pCoil->GetCoilKenV( men )->setting.scode[ men ];	// 基準面
				}
				if (CH_SCODE_NON == pQue->fl.data[2]) pQue->fl.data[2] = MAX_SCODE;

				// 面固定での検査表面状態
				for(int ii=0; ii<NUM_MEN; ii++) {
					pQue->fl.data[3+ii] = gcls_pCoil->GetCoilKenV( ii )->setting.scode[ ii ];		// 面固定
					if (CH_SCODE_NON == pQue->fl.data[3+ii]) pQue->fl.data[3+ii] = MAX_SCODE;
				}
								
				gcls_pParam->gque_Deli.AddToTail( pQue ); 
			}
			break;
	//-----------------------------------------------------------
		case E_DEF_ML_START:							// 検査開始要求
			// 判定状態問合せ
			Send_HtDefect_Status();
			Sleep(1000);			// 結果が返るまでちょっと待ち

			// 検査初期化処理へ
			DetectInitStart(&pMail->que);
			break;

	//-----------------------------------------------------------
		case E_DEF_ML_STOP:								// 検査停止要求
			AllDetectStop(true, false);
			break;

	//-----------------------------------------------------------
		case E_DEF_ML_WAIT_START_ANSWER:				// 判定開始アンサー
			WaitStart_Answer(&pMail->que);
			break;

	//-----------------------------------------------------------
		case E_DEF_ML_WAIT_STOP_ANSWER:					// 判定停止アンサー
			WaitStop_Answer(&pMail->que);
			break;

	//-----------------------------------------------------------
		case E_DEF_ML_STATE_ANSWER:						// 判定状態問合せアンサー
			HT_StatAnswer(&pMail->que);
			break;

	////-----------------------------------------------------------
	//	case E_DEF_ML_RECV_COILINF:						// 検査用コイル情報受信
	//		break;

	//-----------------------------------------------------------
		case E_DEF_ML_INITIAL:							// イニシャル伝文受信
			if(true) {
				// 検査中以外は無視
				if (!gcls_Status.IsDoSequence()) {
					LOG(em_ERR), "[MI] 検査中以外でイニシャル伝文受信");
					break;
				}
				
				syslog(264, "");
				AllDetectStop(true, true);								// 検査停止 → 強制検査再起動
			}
			break;

	//-----------------------------------------------------------
		case E_DEF_ML_ALAMECANCEL:						// 警報出力解除通知
			if( 0 == pMail->que.fl.data[0] ) {
				//// 設備異常警報解除 (正常復帰するまで有効)
				// いつでも有効
				// データメモリ管理クラスへ通知
				gcls_Arm.SetVoicErrCancel();
				LOGA(em_MSG), "[%s] 設備異常警報解除 受付完了！", my_sThreadName);
			} else {
				//// 欠陥検出警報解除

				// 検査中のみ
				if( ! gcls_Status.IsRun() ) break;
				// コイル情報管理クラスへ通知
				gcls_pCoil->SetData_AlarmCancel(DIV_LEN_POS_DSP_TOP);
				// パトライト出力クラスへ通知
				if( NULL != gcls_pCoil->GetCoilDsp(0) ) {
					gcls_Arm.AlarmCancel_Pato( gcls_pCoil->GetCoilDsp(0)->cKizukenNo );
				}
			}
			break;
			
	//-----------------------------------------------------------
		case E_DEF_ML_MAE_WRITE:						// 前工程疵情報書込み完了通知		
			if (true) {
				COMMON_QUE* pQue = new COMMON_QUE;
				memcpy(pQue, &pMail->que, sizeof(COMMON_QUE));

				// データセット
				gcls_Mae.RecvMaeWrite(pQue->mix.cdata[0], pQue->mix.cdata[1]);
				delete pQue;
			}
			break;

	//-----------------------------------------------------------
		case E_DEF_ML_DB_ADD_SYNC:						// DB実績保存状況問い合わせ通知
			Add_Synclock(&pMail->que);
			break;
			
	//-----------------------------------------------------------
		case E_DEF_ML_LINKSW_ANSWER:					// カメラリンクスイッチ設定完了通知
			// 検査初期化待ち処理 (カメラリンクスイッチ設定完了通知)
			WaitInit_Answer(&pMail->que);
			break;

	//-----------------------------------------------------------
		case E_DEF_ML_LINKSW_CAMSET_ANSWER:				// カメラ選択設定完了通知
			// カメラ選択設定アンサー待ちタイマー破棄
			CancelWaitableTimer(LogicParaRunFunc::GetEvTimeCamset());
			LogicParaRunFunc::WaitCamset_Answer(&pMail->que);
			break;

	//-----------------------------------------------------------
		case E_DEF_ML_LINKSW_JOIN_ANSWER:				// 途中参加設定完了通知
			// 途中参加設定アンサー待ちタイマー破棄
			CancelWaitableTimer(LogicParaRunFunc::GetEvTimeResumeStart());
			LogicParaRunFunc::WaitLinkswJoin_Answer(&pMail->que, gcls_pCoil);
			break;

	//-----------------------------------------------------------
		case E_DEF_ML_RESUME_START_ANSWER:				// 途中開始アンサー
			LogicParaRunFunc::WaitResumeStart_Answer(&pMail->que);
			break;

	//-----------------------------------------------------------
		case E_DEF_ML_RESUME_STOP_ANSWER:				// 途中停止アンサー
			LogicParaRunFunc::WaitResumeStop_Answer(&pMail->que);
			break;
		
	//-----------------------------------------------------------
		case E_DEF_ML_RESUME_START:						// 途中開始要求
			LogicParaRunFunc::ResumeStart(&pMail->que);
			break;

	//-----------------------------------------------------------
		case E_DEF_ML_RESUME_STOP:						// 途中停止要求
			LogicParaRunFunc::ResumeStop(&pMail->que);
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




//==========================================
// 全停止
//------------------------------------------
// bool bMode	: true:正常停止 false:異常停止
// bool bRetry	: true:強制再起動あり,false:無し
//==========================================
void MainInstance::AllDetectStop(bool bMode, bool bRetry)
{
#ifdef ENABLE_CYCLE
	Cy_DetectStop(2, bMode);
#endif
	DetectStop(bMode);

	if( bRetry ) DetectReStart();
}


//==========================================
// 強制検査再起動 要求 (強制検査再起動中へ)
//==========================================
void MainInstance::DetectReStart()
{

	//// 既に 強制検査再起動中なら 無処理 (タイマーが一旦リセットされる為)
	if( DIV_SYS_RETRY == gcls_Status.GetSys(0) ) return;
	LOG(em_ERR), "[%s] %d秒後に強制検査再起動", my_sThreadName, PARAM_TIMEOUT_RESTART / 1000);

	// 直前まで動作しているタイマーを破棄
	CancelWaitableTimer(m_evTimeReStart);
	CancelWaitableTimer(m_evTimeInitStart);
	CancelWaitableTimer(m_evTimeWaitStart);
	CancelWaitableTimer(LogicParaRunFunc::GetEvTimeCamset());
	CancelWaitableTimer(LogicParaRunFunc::GetEvTimeResumeStart());

	//// タイマー起動
	__int64 ts = (__int64)PARAM_TIMEOUT_RESTART * -10000;	// 1[ms]→100[ns] (マイナス指定で、指定時間経過でシグナル) (第3引数は2回目以降の起動間隔)
	SetWaitableTimer(m_evTimeReStart, (LARGE_INTEGER*)&ts, 0, NULL, NULL, FALSE);

	//// ステータス変更
	gcls_Status.SetKadou( DIV_KADOU_RETRY );
	for(int ii=0; ii<NUM_MEN; ii++) {
		gcls_Status.SetSys(ii, DIV_SYS_RETRY);
#ifdef ENABLE_CYCLE
		gcls_Status.SetCycleSys(ii, DIV_SYS_RETRY);
#endif
	}
	// ----------------------
	// パララン機能システム状態変更
	for (int ii = 0; ii < NUM_ALL_HTPARA; ii++)
	{
		gcls_Status.SetParaSys(ii, DIV_SYS_RETRY);
	}

	//// 判定PCに現在の状態を問い合わせ
	Send_HtDefect_Status();
	Send_HtDefect_PcCanOk();				// とりあえず判定PCに実行不可能解除を送っておく。運がよければ次回復旧しているはず

	//// オペレータ機能へ現状態を送信
	Send_SoGamenn_MailStatus();
	OpSendFunc::SendOp_Status(); 
}



//==========================================
// 検査初期化処理 (カメラリンクスイッチ設定完了アンサー待ちへ)
//------------------------------------------
// COMMON_QUE *		que		: 起動情報
//==========================================
void MainInstance::DetectInitStart(COMMON_QUE * que)
{
	//========================================================
	// 下準備
	EM_DIV_UNTEN	nUnten = (EM_DIV_UNTEN)que->fl.data[0];					// 運転状態
	EM_DIV_PLG		nPlg   = (EM_DIV_PLG)que->fl.data[1];					// PLG区分
	bool			bMenMode = ( 0 == que->fl.data[2] ? false : true );		// 片面検査許可/不許可(true:許可)
	int				scode[2] = {que->fl.data[3], que->fl.data[4]};			// 検査表面状態
	int				pcode[2] = {que->fl.data[5], que->fl.data[6]};			// 検査パターン
	LogicParaRunFunc::SetHtParaCamNo(0, 0, que->fl.data[7]);					// ロジックパララン機能01 入力カメラ01
	LogicParaRunFunc::SetHtParaCamNo(0, 1, que->fl.data[8]);					// ロジックパララン機能01 入力カメラ02
	LogicParaRunFunc::SetHtParaCamNo(1, 0, que->fl.data[9]);					// ロジックパララン機能02 入力カメラ01
	LogicParaRunFunc::SetHtParaCamNo(1, 1, que->fl.data[10]);					// ロジックパララン機能02 入力カメラ02

	//========================================================
	// 事前チェック
	// 　この時点でのエラーは、リトライしない
	
	//// 必ず停止中のみ
	for(int ii=0; ii<NUM_MEN; ii++){
		if( DIV_SYS_IDLE  != gcls_Status.GetSys(ii) &&
			DIV_SYS_STOP  != gcls_Status.GetSys(ii) &&
#ifdef ENABLE_CYCLE
			DIV_SYS_IDLE  != gcls_Status.GetCycleSys(ii) &&
			DIV_SYS_STOP  != gcls_Status.GetCycleSys(ii) &&
			DIV_SYS_RETRY != gcls_Status.GetCycleSys(ii) &&
#endif
			DIV_SYS_RETRY != gcls_Status.GetSys(ii) ){
			
			LOG(em_ERR), "[%s] <%s> [%s] 時に検査初期化要求受信", my_sThreadName, DivNameManager::GetTorB(ii), GetDivSys(gcls_Status.GetSys(ii)) );
			syslog(236, "[<%s> %s 時に検査初期化要求受信]", DivNameManager::GetTorB(ii), GetDivSys(gcls_Status.GetSys(ii)) );
			return ;
		}
	}

#ifdef CAM_ADJUST_REMOTE
	//// カメラ調整にリモートデスクトップ使用
	if( DIV_UNTEN_CAMERA == nUnten ){
		gcls_Status.SetMenMode(bMenMode);
		gcls_Status.SetPlg(nPlg);
		gcls_Status.SetUnten(nUnten);
		gcls_Status.SetKadou( DIV_KADOU_START );
		for(int ii=0; ii<NUM_MEN; ii++){
			gcls_Status.SetSys(ii, DIV_SYS_RUN);

			// 点灯状態に変更。KT_LEDMGRのみ
#ifdef KT_LEDMGR
			ToOutMailSender::Send_KtLedMgr_OnOff(ii, true);
			ToOutMailSender::Send_KtLedMgr_CoilChange(ii, gcls_pCoil->GetInitScode(ii));
#endif
		}


		//// オペレータ機能へ送信
		Send_SoGamenn_MailStatus();
		OpSendFunc::SendOp_Status();
		return;
	}
#endif

	//======================================================
	// 判定PCの割り振り開始
	bool bOk[NUM_MEN];							// 単発判定結果(true:成功, false:失敗)

	if( ! gcls_Status.SelectHanteiPC_MultiGroup( bOk )){
		for(int ii=0; ii<NUM_MEN; ii++){
			if(!bOk[ii]){
				LOG(em_ERR), "[%s] <%s> 判定PCの割り振り不可能", my_sThreadName, DivNameManager::GetTorB(ii));
				syslog(238, "[<%s> 判定PCの割り振り不可能]", DivNameManager::GetTorB(ii));
			
				gcls_Status.SetSys(ii, DIV_SYS_STOP);
			}
		}
	}
	// 判定異常／正常復帰をセット
	KizuFunction::SetStatus("HANTEI_TOP_ANS_ID", bOk[0]);	// 表
	KizuFunction::SetStatus("HANTEI_BOT_ANS_ID", bOk[1]);	// 裏

#ifdef ENABLE_CYCLE
	//======================================================
	// 周期PCの割り振り開始
	bool bCycleOk[NUM_MEN];						// 周期判定結果(true:成功, false:失敗)

	if( ! gcls_Status.SelectHanteiPC_BackupGroup( DIV_CYCLE, bCycleOk )){
		for(int ii=0; ii<NUM_MEN; ii++){
			if(!bCycleOk[ii]){
				LOG(em_ERR), "[%s] <%s> 周期PCの割り振り不可能", my_sThreadName, DivNameManager::GetTorB(ii));
				syslog(238, "[<%s> 周期PCの割り振り不可能]", DivNameManager::GetTorB(ii));
			
				gcls_Status.SetCycleSys(ii, DIV_SYS_STOP);
			}
		}
	}
	// 周期異常／正常復帰をセット
	KizuFunction::SetStatus("CYCLE_TOP_ANS_ID", bCycleOk[0]);	// 表
	KizuFunction::SetStatus("CYCLE_BOT_ANS_ID", bCycleOk[1]);	// 裏
#endif

	// ----------------------
	// ロジックパラランPCのチェック
	bool bParaOk = gcls_Status.SelectHanteiPC_ResumeGroup();
	if (DIV_UNTEN_CAMERA != nUnten)
	{
		// 「カメラ校正」モード以外

		if (!bParaOk)
		{
			LOG(em_ERR), "[%s] ロジックパラランPC定義異常", my_sThreadName);
			syslog(238, "[ロジックパラランPCの割り振り不可能]");

			for (int ii = 0; ii < NUM_ALL_HTPARA; ii++)
			{
				gcls_Status.SetParaSys(ii, DIV_SYS_STOP);
			}
		}
	}
	else
	{
		// 「カメラ校正」モード時
		bParaOk = false;
	}

	//======================================================
	// 外部通知

	// 全PCへ統括時刻を送信
	ToOutMailSender::Send_KsMaster_TimeSet();

	// PC名変更通知
	ToOutMailSender::Send_KsMaster_PcChange();
	Sleep(200);									// HT_DEFECTにいきわたるまでちょっと待つ


	//======================================================
	// 検査開始チェック

	// 両面検査時は、一個でもエラーがあると NG
	// 片面許可時は、両方エラーで NG
	if( ( ! bMenMode && (!bOk[0] || !bOk[1]) ) ||
		(   bMenMode && (!bOk[0] && !bOk[1]) ) )  {

		gcls_Status.SetKadou( DIV_KADOU_STOP );
		LOG(em_ERR), "[%s] 検査開始失敗！！", my_sThreadName );
		syslog(238, "[検査開始失敗]");

		//// オペレータ機能へを送信
		Send_SoGamenn_MailStatus();
		OpSendFunc::SendOp_Status(); 
		return;
	}

	//======================================================
	// 開始準備

	//// 全パラメータ読込
	if( ! gcls_pParam->ReadParam_All() ){
		LOG(em_ERR), "[%s] 全パラメータ読込失敗", my_sThreadName);
	}


	//// 他タスク　＆他スレッドへ通知
	// カメラ調整時以外
	if( DIV_UNTEN_CAMERA != nUnten ){
		// 判定受信クラスの検査開始待ち通知
		for(int ii=0; ii<NUM_MEN; ii++){
			if(!bOk[ii]) continue;
			gcls_pHtRecv[ii]->SetEvDetectWaitStart();
#ifdef ENABLE_CYCLE
			gcls_pCyRecv[ii]->SetEvDetectWaitStart();
#endif
		}
	}

	//======================================================
	// 自クラスの初期化
	// コイル情報初期化
	for(int ii=0; ii<NUM_MEN; ii++){
		gcls_pCoil->SetInitScode(ii, scode[ii]);
		gcls_pCoil->SetInitPcode(ii, pcode[ii]);
	}
	gcls_pCoil->AllDelete();
	// エッジ検出強化
	gcls_EdgePosCont.Init();
	if( DIV_UNTEN_CAMERA == nUnten || DIV_UNTEN_SAMP == nUnten) gcls_EdgePosCont.SetOldMode(true);	// カメラ調整時は、整合性判定をしない
	else														gcls_EdgePosCont.SetOldMode(false);


	//======================================================
	// 他タスクに検査開始通知を送信

	//// カメラコントローラに検査開始通知
	ToOutMailSender::Send_ToCamera_Palse(false);				// 初期化時は、パルス出力停止のみ
	ToOutMailSender::Send_ToCamera_Start(nPlg);

	//// 検査開始一発目用の設定を各機器に通知
	for(int ii=0; ii<NUM_MEN; ii++){
		ToOutMailSender::Send_ToCamera_CoilChange(ii, gcls_pCoil->GetInitScode(ii));
#ifndef KT_LEDMGR
		ToOutMailSender::Send_ToLights_CoilChange(ii, gcls_pCoil->GetInitScode(ii));
#else
		ToOutMailSender::Send_KtLedMgr_OnOff(ii, TRUE);
		ToOutMailSender::Send_KtLedMgr_CoilChange(ii, gcls_pCoil->GetInitScode(ii));
#endif
	}

	//// オペレータ機能へ表示情報初期化通知
	Send_HyDefect_MemDataInit();

#ifdef ENABLE_RPI_SAVE
	// 全長保存開始
	gcls_IfRpiSave.Send_TiRpiSave_Start();
#endif

	// ----------------------
	// カメラ選択コマンド事前準備
	LogicParaRunFunc::ClearLinkswCmd(LogicParaRunFunc::E_LINKSW_CMD_START);		// コマンドリストクリア

	if (DIV_UNTEN_CAMERA != nUnten)
	{
		// 「カメラ校正」モード以外

		// カメラ選択設定コマンドキューイング（全ロジックパラランPC分）
		LogicParaRunFunc::RegistLinkswCmd();
	}

	//// カメラリンクスイッチ通信へ 検査開始通知 (※ DB登録完了時に送信)
	//// 事前準備
	DBManager::DeliveryDB* deliDB = new DBManager::DeliveryDB;
	deliDB->kubun	= DBManager::em_CAMLINKSW;
	deliDB->data	= (BYTE*)new DBDATA_CAMERA_LINKSWITCH;
	DBDATA_CAMERA_LINKSWITCH* pwk = (DBDATA_CAMERA_LINKSWITCH*)deliDB->data;
	memset(pwk, 0x00, sizeof(DBDATA_CAMERA_LINKSWITCH));

	//// データセット
	for(int ii=0; ii<NUM_MEN; ii++){
		if(!bOk[ii]) continue;
		for(int jj=0; jj<NUM_HANTEI; jj++){
			// 判定PCID
			pwk->nPCID[ii][jj] = gcls_Status.GetHtID(ii,jj);
		}
	}
	for (int ii = 0; ii < NUM_ALL_HTPARA; ii++)
	{
		if (!bParaOk) continue;
		// ロジックパラランPCID
		pwk->nParaPCID[ii] = gcls_Status.GetHtParaID(ii);
	}
	// ロジックパララン用カメラセット
	// ※現在メモリ保持している値（最新）を設定する
	memcpy(pwk->nLogiParaCam, LogicParaRunFunc::GetHtParaCamNo(), LogicParaRunFunc::GetHtParaCamSize());

	if( ! gcls_DB.gque_AddSQLData.AddToHead( deliDB ) )
	{
		LOG(em_ERR), "[%s] DB登録依頼キューフル [DBDATA_CAMERA_LINKSWITCH]", my_sThreadName);
		syslog(SYSNO_QUEFULL_ERR, "[DBDATA_CAMERA_LINKSWITCH]");
		delete deliDB->data;
		delete deliDB;
		// DBが異常 (通常ありない)
		gcls_Status.AddQueMyErrWar( true, ON_QUE_CAMLINKSW );
		return;
	}


	////////////////////////////////////
	// 終了処理
	////////////////////////////////////
	//// ステータス変更
	for( int ii=0; ii<NUM_MEN; ii++ )
	{
		if( bOk[ii] ) gcls_Status.SetSys( ii, DIV_SYS_INITSTART );
		else		  gcls_Status.SetSys( ii, DIV_SYS_STOP );
#ifdef ENABLE_CYCLE
		if( bCycleOk[ii] ) gcls_Status.SetCycleSys( ii, DIV_SYS_INITSTART );
		else			   gcls_Status.SetCycleSys( ii, DIV_SYS_STOP );
#endif
	}
	gcls_Status.SetMenMode( bMenMode );
	gcls_Status.SetPlg( nPlg );
	gcls_Status.SetUnten( nUnten );
	for(int ii=0; ii<NUM_MEN; ii++) gcls_Status.SetHtState_AllOff(ii);


#ifndef CAM_ADJUST_REMOTE
	//======================================================
	//// カメラ調整時、各リンクスイッチの先頭判定以外は正常をセット
	if (DIV_UNTEN_CAMERA == nUnten)
	{
		int nGroupNo = -1;
		for (int ii = 0; ii < NUM_MEN; ii++)
		{
			for (int jj = 0; jj < NUM_HANTEI; jj++)
			{
				int nWk = ToLinkswUtil::GetHanteiGroup(ii, jj, 1);
				if (nWk == -1 || nGroupNo == nWk)
				{
					gcls_Status.SetHtState(ii, jj, DIV_BOOL_TRUE);
				}
				if (nWk != -1 && nGroupNo != nWk) nGroupNo = nWk;
			}
		}
	}
#endif

	// ----------------------
	// 機器状態－カメラリンクスイッチ通信応答無し（正常）
	KizuFunction::SetStatus("CAMLINKSW_ANS_ID", true);
	if (DIV_UNTEN_CAMERA == nUnten) LOG(em_INF), "[%s] カメラ校正モード時はロジックパララン無効", my_sThreadName);
	for (int ii = 0; ii < NUM_ALL_HTPARA; ii++)
	{
		// パララン機能システム状態変更
		if (bParaOk)  gcls_Status.SetParaSys(ii, DIV_SYS_INITSTART);
		else		  gcls_Status.SetParaSys(ii, DIV_SYS_STOP);

		// 機器状態－ロジックパララン機能 
		for (int jj = 0; jj < NUM_CAMLINK_BOARD; jj++)
		{
			// 選択結果（正常）
			KizuFunction::SetStatus(gcls_Status.GetStatusRead().ParaState[ii][jj].id, true, false);
			// 入力カメラ（不明）
			KizuFunction::SetStatus(gcls_Status.GetStatusRead().ParaCam[ii][jj].id, -1, false);
		}
	}
	// カメラ選択設定アンサー待ちタイマー破棄
	CancelWaitableTimer(LogicParaRunFunc::GetEvTimeCamset());
	// 途中参加設定アンサー待ちタイマー破棄
	CancelWaitableTimer(LogicParaRunFunc::GetEvTimeResumeStart());
	// ロジックパララン状態をリセット（無し）
	gcls_Status.SetHtParaState_AllOff();

	// カメラリンクスイッチ設定アンサー待ちタイマ 起動
	__int64 ts = (__int64)PARAM_TIMEOUT_DETECT_INIT_START * -10000;	// 1[ms]→100[ns] (マイナス指定で、指定時間経過でシグナル) (第3引数は2回目以降の起動間隔)
	SetWaitableTimer( m_evTimeInitStart, (LARGE_INTEGER*)&ts, 0, NULL, NULL, FALSE );

	LOG(em_MSG), "[%s] カメラリンクスイッチ設定完了通知待ち....", my_sThreadName);
}


//==========================================
// 検査初期化待ち処理
// (カメラリンクスイッチ設定完了通知)
//==========================================
void MainInstance::WaitInit_Answer(COMMON_QUE* que)
{
	////////////////////////////////////
	// 事前チェック
	//   この時点でのエラー時は、リトライしない
	////////////////////////////////////
	//// ステータス確認
	for (int ii = 0; ii<NUM_MEN; ii++)
	{
		// カメラ調整
		if (DIV_UNTEN_CAMERA == gcls_Status.GetUnten()) {
			LOG(em_MSG), "[%s] <%s> カメラ校正モード時のカメラリンクスイッチ設定完了通知", my_sThreadName, DivNameManager::GetTorB(ii));
			//return;		// iPortViewerからのカメラ切り替え時
		}

		// 片面検査許可時は、他方の面が検査開始初期化中であれば、正常受信とする
		if (DIV_SYS_STOP == gcls_Status.GetSys(ii)) {
			int ou_not = ii == 1 ? 0 : 1;								// 他方の面
			if (!gcls_Status.GetMenMode() ||
				(gcls_Status.GetMenMode() && DIV_SYS_INITSTART != gcls_Status.GetSys(ou_not))) {
				LOG(em_MSG), "[%s] <%s> [%s] 時に検査初期化アンサー通知受信(カメラ切替)", my_sThreadName, DivNameManager::GetTorB(ii), GetDivSys(gcls_Status.GetSys(ii)));
				return;
			}
		}

		if (DIV_SYS_INITSTART != gcls_Status.GetSys(ii) &&
			DIV_SYS_STOP != gcls_Status.GetSys(ii))
		{
			LOG(em_ERR), "[%s] <%s> [%s] 時に検査初期化アンサー通知受信", my_sThreadName, DivNameManager::GetTorB(ii), GetDivSys(gcls_Status.GetSys(ii)));
			return;
		}
	}

	//// もろもろタイマー破棄
	CancelWaitableTimer(m_evTimeInitStart);

	// 設定結果チェック
	if (1 != que->fl.data[0])
	{
		// 設定結果：失敗
		LOG(em_ERR), "[%s] カメラリンクスイッチ設定完了通知 異常検出(結果異常) ⇒ 強制検査再起動 要求", my_sThreadName);

		// 機器状態－カメラリンクスイッチ通信応答無し（異常）
		KizuFunction::SetStatus("CAMLINKSW_ANS_ID", false);

		// コマンドリストクリア
		LogicParaRunFunc::ClearLinkswCmd();

		// 検査停止・開始
		syslog(235, "[カメラリンクスイッチ設定完了通知 結果異常]");
		DetectStop(false);							// 検査停止
		DetectReStart();							// 強制検査再起動
	}
	else
	{
		// 設定結果：成功

		// 設定結果：成功
		LOG(em_MSG), "[%s] カメラリンクスイッチ設定完了！！", my_sThreadName);

		// 検査開始待ち処理
		DetectWaitStart();
	}

	if (DIV_UNTEN_CAMERA != gcls_Status.GetUnten())
	{
		// 「カメラ校正」モード時はロジックパララン無し

		LOG(em_MSG), "[%s] ロジックパララン設定中....", my_sThreadName);

		// 次のコマンド送信（キュー制御）
		LogicParaRunFunc::Send_NextCmdSelect();
	}
}

//==========================================
// 検査開始待ち開始処理 (判定検査開始アンサー待ちへ)
//==========================================
void MainInstance::DetectWaitStart()
{

	//======================================================
	// 事前チェック
	//   この時点でのエラー時は、リトライしない
	// 必ず停止中のみ
	for(int ii=0; ii<NUM_MEN; ii++) {
		// 現ステータス確認
		if( DIV_SYS_IDLE      != gcls_Status.GetSys(ii) && 
			DIV_SYS_INITSTART != gcls_Status.GetSys(ii) &&
			DIV_SYS_STOP      != gcls_Status.GetSys(ii) &&
#ifdef ENABLE_CYCLE
			DIV_SYS_IDLE      != gcls_Status.GetCycleSys(ii) && 
			DIV_SYS_INITSTART != gcls_Status.GetCycleSys(ii) && 
			DIV_SYS_STOP      != gcls_Status.GetCycleSys(ii) &&
			DIV_SYS_RETRY     != gcls_Status.GetCycleSys(ii) &&
#endif
			DIV_SYS_RETRY!= gcls_Status.GetSys(ii)  ) {
			LOG(em_ERR), "[%s] <%s> [%s] 時に検査開始要求受信", my_sThreadName, DivNameManager::GetTorB(ii), GetDivSys(gcls_Status.GetSys(ii)) );
			syslog(236, "[<%s> %s 時に検査開始要求受信]", DivNameManager::GetTorB(ii), GetDivSys(gcls_Status.GetSys(ii)) );
			return ;
		}
	}

	////////////////////////////////////
	// 検査開始待ち処理
	////////////////////////////////////
	// PLGカウンタ値 リセット要求
#ifndef DISABLE_PLG_RESET
	gcls_Dmem.PoPuls_CountReset();
	syslog(242, "探傷開始時に自動リセット");
	LOG(em_INF), "[%s] シーケンサPLGカウントリセット(検査開始時)[Exsync値=%d][Plg値=%d]", my_sThreadName, gcls_Dmem.GetDmRecv().ken.nLvalAdd, gcls_Dmem.GetDmRecv().ken.nPlgAdd);
#endif
	//// 判定PCに検査開始通知を送信
	Send_HtDefect_Start();
	//// 判定PC側でJPEG圧縮させる
	Send_HtDefect_JpegQuality(gcls_pParam->GetCommonMast().nJpegQuality);

	//======================================================
	// 終了処理

	// ステータス変更
	for(int ii=0; ii<NUM_MEN; ii++) {
		if( DIV_SYS_STOP == gcls_Status.GetSys(ii) ) continue;
		gcls_Status.SetSys(ii, DIV_SYS_WAITSTART );
#ifdef ENABLE_CYCLE
		gcls_Status.SetCycleSys(ii, DIV_SYS_WAITSTART );
#endif
	}
	// 検査開始時には正常化
	KizuFunction::SetStatus("PLGLEN_KEN_CHECK_ID", true, false);	// 検査用PLG補正率
	KizuFunction::SetStatus("PLGLEN_DSP_CHECK_ID", true, false);	// 表示用PLG補正率
	KizuFunction::SetStatus("HOSEIRITU_ID", true, false);	// プロコン長補正率
	for (int ii = 0; ii<NUM_MEN; ii++) {
		KizuFunction::SetStatus(gcls_Status.GetStatusRead().FrameGrabStop[ii].id, true);	// 撮像フレーム停止
	}

	// パトライト出力管理初期化
	gcls_Arm.Init_Pato();

	//======================================================
	// 判定PCからの応答待ちタイマー起動
	__int64 ts = (__int64)PARAM_TIMEOUT_WAIT_START * -10000;	// 1[ms]→100[ns] (マイナス指定で、指定時間経過でシグナル) (第3引数は2回目以降の起動間隔)
	SetWaitableTimer(m_evTimeWaitStart, (LARGE_INTEGER*)&ts, 0, NULL, NULL, FALSE);
}

//==========================================
// 検査開始待ち 処理 (アンサー受信)
//------------------------------------------
// COMMON_QUE* que	: 起動情報
//==========================================
void MainInstance::WaitStart_Answer(COMMON_QUE* que)
{

	//======================================================
	// 下準備
	int campair = que->fl.data[0];							// カメラペア (1オリジン)
	int pcid	= que->fl.data[1];							// PCID (1オリジン)
	int ans		= que->fl.data[2];							// 結果 (0:正常 1:軽故障 -1:重故障)
	EM_DIV_HANTEI emKind = (EM_DIV_HANTEI)que->fl.data[3];	// 担当判定


	int ou;													// 表裏
	int no;													// No
	if( DIV_HANTEI_HT == emKind ) {
		ou		= ((campair-1) / (NUM_CAMPAIR/NUM_MEN) );
		no		= ((campair-1) % (NUM_CAMPAIR/NUM_MEN) );
	}
#ifdef ENABLE_CYCLE
	if( DIV_HANTEI_CY == emKind ) {
		ou		= ((campair-1) / (NUM_CAMPAIR_CYCLE/NUM_MEN) );
		no		= ((campair-1) % (NUM_CAMPAIR_CYCLE/NUM_MEN) );		
	}
#endif
	// [担当判定] パララン判定
	if (DIV_HANTEI_PR == emKind)
	{
		// 異常応答
		// ※常に正常で返す筈
		if (0 != ans)
		{
			LOG(em_ERR), "[%s] 検査開始異常アンサー受信(ロジックパララン) カメラペア=%d ans=%d", my_sThreadName, campair, ans);

			// [機器設定]－判定PCxx応答無し
			CString wk;
			wk.Format("HANTEI_ANS_%d_ID", pcid);
			KizuFunction::SetStatus(wk, false, true, "検査開始アンサー異常");
		}
		else
		{
			//// パララン判定正常
			if (0 != gcls_Status.GetStatusRead().Ht_NonAnswer[pcid - 1].stat)
			{
				// [機器設定]－判定PCxx応答無し
				KizuFunction::SetStatus("HANTEI_ANS_%d_ID", pcid, 0);
			}
		}

		// ロジックパラランPCの応答処理はここまで
		return;
	}

	//======================================================
	// 事前チェック
	//// ステータス
	if( DIV_HANTEI_HT == emKind &&
		gcls_Status.GetSys(ou) != DIV_SYS_WAITSTART ) {
		LOG(em_ERR), "[%s] <%s> [%s]時に検査開始アンサー通知受信", my_sThreadName, DivNameManager::GetTorB(ou), GetDivSys(gcls_Status.GetSys(ou)));
		return;
	}
#ifdef ENABLE_CYCLE
	if( DIV_HANTEI_CY == emKind &&
		gcls_Status.GetCycleSys(ou) != DIV_SYS_WAITSTART ) {
		LOG(em_ERR), "[%s] <%s> [%s]時に周期判定検査開始アンサー通知受信", my_sThreadName, DivNameManager::GetTorB(ou), GetDivSys(gcls_Status.GetCycleSys(ou)));
		return;
	}
#endif

	//// アンサー結果
	if( 0 != ans ) {
		LOG(em_ERR), "[%s] 検査開始異常アンサー受信 カメラペア=%d ans=%d", my_sThreadName, campair, ans);
		syslog(236, "[異常アンサー受信 カメラペア=%d ans=%d]", campair, ans);
		CString wk;
		wk.Format("HANTEI_ANS_%d_ID", pcid );
		KizuFunction::SetStatus(wk, false, false, "検査開始アンサー異常");
		gcls_Status.SetHtState(ou, no, DIV_BOOL_FALSE); 
//		return;
	}

	//// 判定状態 セット
	if( 0 == ans ) {
		if( DIV_HANTEI_HT == emKind ) gcls_Status.SetHtState(ou, no, DIV_BOOL_TRUE); 
		if( DIV_HANTEI_HT == emKind && 1 == NUM_CAMPAIR) gcls_Status.SetHtState(1, no, DIV_BOOL_TRUE);	// 1PC 両面対応
		if( 0 != gcls_Status.GetStatusRead().Ht_NonAnswer[pcid-1].stat ) {
			KizuFunction::SetStatus("HANTEI_ANS_%d_ID", pcid, 0);
		}
	}

	//// 全判定から何かしらの応答が帰って来た？
	for(int ii=0; ii<NUM_MEN; ii++) {
		if( DIV_SYS_STOP == gcls_Status.GetSys(ii) ) continue;					// 既にNGとわかっている面であれば無視
		
		EM_DIV_BOOL wk;
		for(int jj=0; jj<NUM_MEN; jj++){
			for(int kk=0; kk<NUM_HANTEI; kk++){
				wk =  gcls_Status.GetHtState(jj,kk);
			}
		}

		if( gcls_Status.IsHtState_OneOnOff(ii, DIV_BOOL_NON ) ) return;			// １つでも初期値有り


#ifdef ENABLE_CYCLE
		// 校正時は、周期は動かさない
		if( DIV_UNTEN_CAMERA != gcls_Status.GetUnten() ) {
			if( ! gcls_Status.IsCyState_AllOnOff(ii, DIV_BOOL_TRUE) ) return;
		}
#endif

		// 校正時は、ソケットを繋げないから
		if( DIV_UNTEN_CAMERA != gcls_Status.GetUnten() ) {
			if( ! gcls_pHtRecv[ii]->IsConnectAll() ) return;
#ifdef ENABLE_CYCLE
			if( ! gcls_pCyRecv[ii]->IsConnectAll() ) return;
#endif
		}
	}
	
	//// 片面検査即時チェック対応
	bool bTimeUp = false;
	for(int ii=0; ii<NUM_MEN; ii++) {
		if( gcls_Status.IsHtState_OneOnOff(ii, DIV_BOOL_FALSE) ) bTimeUp = true;
	}
	if(bTimeUp) {
		WaitStart_TimeOut();	// タイムアップしたこととする。
		return;
	}

	
	//// ここまできたら検査中へ遷移
	DetectRun();
}

//==========================================
// 検査停止待ち 処理 (アンサー受信)
//------------------------------------------
// COMMON_QUE* que	: 起動要因	
//==========================================
void MainInstance::WaitStop_Answer(COMMON_QUE* que)
{
	//======================================================
	// 下準備
	int campair = que->fl.data[0];							// カメラペア (1オリジン)
	int pcid	= que->fl.data[1];							// PCID (1オリジン)
	int ans		= que->fl.data[2];							// 結果 (0:正常 1:軽故障 -1:重故障)
	int mode	= que->fl.data[3];							// 区分 (0:応答 1:いきなり)
	EM_DIV_HANTEI emKind = (EM_DIV_HANTEI)que->fl.data[4];	// 担当判定


	int ou = 0;												// 表裏
	int no = 0;												// 0～3
	if( DIV_HANTEI_HT == emKind ) {
		ou		= ((campair-1) / (NUM_CAMPAIR/NUM_MEN) );
		no		= ((campair-1) % (NUM_CAMPAIR/NUM_MEN) );
	}
#ifdef ENABLE_CYCLE	
	if( DIV_HANTEI_CY == emKind ) {
		ou		= ((campair-1) / (NUM_CAMPAIR_CYCLE/NUM_MEN) );
		no		= ((campair-1) % (NUM_CAMPAIR_CYCLE/NUM_MEN) );		
	}
#endif
	// [担当判定] パララン判定
	if (DIV_HANTEI_PR == emKind)
	{
		// 異常応答
		// ※常に正常で返す筈
		if (0 != ans || 0 != mode)
		{
			LOG(em_ERR), "[%s] 検査停止アンサー 異常検出(ロジックパララン) pcid=%d", my_sThreadName, pcid);

			// [機器設定]－判定PCxx応答無し
			CString wk;
			wk.Format("HANTEI_ANS_%d_ID", pcid);
			KizuFunction::SetStatus(wk, false, true, "検査停止アンサー異常");
		}
		else
		{
			//// パララン判定正常
			if (0 != gcls_Status.GetStatusRead().Ht_NonAnswer[pcid - 1].stat)
			{
				// [機器設定]－判定PCxx応答無し
				KizuFunction::SetStatus("HANTEI_ANS_%d_ID", pcid, 0);
			}
		}

		// ロジックパラランPCの状態変更
		for (int ii = 0; ii < NUM_ALL_HTPARA; ii++)
		{
			// 応答したロジックパラランPCのみ状態変更
			if (gcls_Status.GetHtParaID(ii) == pcid)
			{
				// ----------------------
				// ロジックパララン状態 － クリア
				// （ロジックパララン状態欄表示用）
				gcls_Status.SetHtParaState(ii, DIV_BOOL_FALSE);
				break;
			}
		}

		// ロジックパラランPCの応答処理はここまで
		return;
	}
	int ou_not  = ou==1 ? 0 :1;								// 他方の面

	//======================================================
	// 事前チェック
	if( 0 == mode ) {
		//// 判定状態 セット	
		if( DIV_HANTEI_HT == emKind ) gcls_Status.SetHtState(ou, no, DIV_BOOL_FALSE); 
		if( DIV_HANTEI_HT == emKind && 1 == NUM_CAMPAIR) gcls_Status.SetHtState(1, no, DIV_BOOL_FALSE);	// 1PC 両面対応
		//if( DIV_HANTEI_CY == emKind ) gcls_Status.SetCyState(ou, no, DIV_BOOL_FALSE); 
		//if( DIV_HANTEI_CY == emKind && 1 == NUM_CAMPAIR) gcls_Status.SetCyState(1, no, DIV_BOOL_FALSE);	// 1PC 両面対応

		//// 応答
		if( DIV_HANTEI_HT == emKind ) {
			if( gcls_Status.GetSys(ou) == DIV_SYS_WAITSTOP ||
				gcls_Status.GetSys(ou) == DIV_SYS_STOP	) {
				return;
			} else {
				LOG(em_MSG), "[%s] 単発判定検査停止アンサー 受信状態異常 %s campair=%d ans=%d", my_sThreadName, GetDivSys(gcls_Status.GetSys(ou)), campair, ans);
			}
		}
#ifdef ENABLE_CYCLE		
		if( DIV_HANTEI_CY == emKind ) {
			if(	gcls_Status.GetCycleSys(ou) == DIV_SYS_WAITSTOP ||
				gcls_Status.GetCycleSys(ou) == DIV_SYS_STOP	) {
				return;
			} else {
				LOG(em_MSG), "[%s] 周期判定検査停止アンサー 受信状態異常 %s campair=%d ans=%d", my_sThreadName, GetDivSys(gcls_Status.GetCycleSys(ou)), campair, ans);
			}
		}
#endif
	}

	//// 判定正常
	if( 0 != gcls_Status.GetStatusRead().Ht_NonAnswer[pcid-1].stat ) {
		KizuFunction::SetStatus("HANTEI_ANS_%d_ID", pcid, 0);
	}


	//======================================================
	////// いきなり停止通知が来た！！ 又は、 重故障時
	if( DIV_KADOU_STOP == gcls_Status.GetKadou() ) return;
	if( DIV_HANTEI_HT == emKind ) {
		if( (1 == mode && gcls_Status.IsRun() ) || -1 == ans ) {
			// ・片面検査不許可時は、検査停止
			// ・片面検査許可時でも、一方が既にNGの時は、検査停止
			if( ! gcls_Status.GetMenMode() ||
				( gcls_Status.GetMenMode() && DIV_SYS_RUN != gcls_Status.GetSys(ou_not)) ) {
				LOG(em_ERR), "[%s] 検査停止アンサー 異常検出 pcid=%d", my_sThreadName, pcid);

				// 検査停止してから検査強制再起動へ
				AllDetectStop(false, true);
				return;

			// ・片面検査許可時で、両方とも検査中
			} else if( gcls_Status.GetMenMode() && DIV_SYS_RUN == gcls_Status.GetSys(0) && DIV_SYS_RUN == gcls_Status.GetSys(1) ) {
				LOG(em_WAR), "[%s] 検査停止アンサー 異常検出。片面のみで検査続行 pcid=%d", my_sThreadName, pcid);
				// 今回の面のみ検査中断
				DetectRun_Cancel(ou);
			}	
		}
	}
}

//==========================================
// 検査開始待ち 処理 (タイムアウト発生)
//==========================================
void MainInstance::WaitStart_TimeOut()
{

	//======================================================
	//// 事前チェック
	// ステータス
#ifndef ENABLE_CYCLE
	if( ( gcls_Status.GetSys(0) != DIV_SYS_WAITSTART && gcls_Status.GetSys(0) != DIV_SYS_STOP ) &&
		( gcls_Status.GetSys(1) != DIV_SYS_WAITSTART && gcls_Status.GetSys(1) != DIV_SYS_STOP ) ) {
		LOG(em_ERR), "[%s] 検査開始アンサータイムアウト ステータス異常 (表=%s 裏=%s)", my_sThreadName, GetDivSys(gcls_Status.GetSys(0)), GetDivSys(gcls_Status.GetSys(1)));
		return;
	}
#else
	if( ( gcls_Status.GetSys(0) != DIV_SYS_WAITSTART && gcls_Status.GetSys(0) != DIV_SYS_STOP ) &&
		( gcls_Status.GetSys(1) != DIV_SYS_WAITSTART && gcls_Status.GetSys(1) != DIV_SYS_STOP ) &&
		( gcls_Status.GetCycleSys(0) != DIV_SYS_WAITSTART && gcls_Status.GetCycleSys(0) != DIV_SYS_STOP ) &&
		( gcls_Status.GetCycleSys(1) != DIV_SYS_WAITSTART && gcls_Status.GetCycleSys(1) != DIV_SYS_STOP ) ) {
		LOG(em_ERR), "[%s] 検査開始アンサータイムアウト ステータス異常 (表=%s 裏=%s 周期表=%s 周期裏=%s)", my_sThreadName, GetDivSys(gcls_Status.GetSys(0)), GetDivSys(gcls_Status.GetSys(1)), GetDivSys(gcls_Status.GetCycleSys(0)), GetDivSys(gcls_Status.GetCycleSys(1)) );
		return;
	}
#endif

	//======================================================
	//// どの判定機能がダメ？
	for(int ii=0; ii<NUM_MEN; ii++) {
		for(int jj=0; jj<NUM_HANTEI; jj++) {
			if( ! gcls_Status.IsHtState(ii, jj) ) {
				if(0 == gcls_Status.GetHtID(ii, jj) ) continue;
				KizuFunction::SetStatus("HANTEI_ANS_%d_ID", gcls_Status.GetHtID(ii, jj), 1, false);
			}
		}
#ifdef ENABLE_CYCLE
		for(int jj=0; jj<NUM_HANTEI_CYCLE; jj++) {
			if( ! gcls_Status.IsCyState(ii, jj) ) {
				if(0 == gcls_Status.GetCyID(ii, jj) ) continue;
				KizuFunction::SetStatus("HANTEI_ANS_%d_ID", gcls_Status.GetCyID(ii, jj), 1, false);
			}
		}
#endif
	}
	for (int ii = 0; ii < NUM_ALL_HTPARA; ii++)
	{
		if (!gcls_Status.IsHtParaState(ii))
		{
			if (0 == gcls_Status.GetHtParaID(ii)) continue;
			KizuFunction::SetStatus("HANTEI_ANS_%d_ID", gcls_Status.GetHtParaID(ii), 1, false);
		}
	}

	//======================================================
	//// どの面がダメ？
	for(int ii=0; ii<NUM_MEN; ii++) {
		// そもそも 開始準備の時からダメな奴は無処理
		if( gcls_Status.GetSys(ii) == DIV_SYS_STOP ) continue;

		// ダメ？
		if( ! gcls_Status.IsHtState_AllOnOff(ii, DIV_BOOL_TRUE) ||												// アンサー
			( DIV_UNTEN_CAMERA != gcls_Status.GetUnten() && ! gcls_pHtRecv[ii]->IsConnectAll()) ) {		// 校正時は、ソケットを繋げないから

			// 指定面のみの検査中断
			DetectRun_Cancel(ii);
		}
	}


	// 両面検査時は、一個でもエラーがあると NG
	// 片面許可時は、両方エラーで NG
	if( ( ! gcls_Status.GetMenMode() && (gcls_Status.GetSys(0) == DIV_SYS_STOP || gcls_Status.GetSys(1) == DIV_SYS_STOP) ) ||
		(   gcls_Status.GetMenMode() && (gcls_Status.GetSys(0) == DIV_SYS_STOP && gcls_Status.GetSys(1) == DIV_SYS_STOP) ) )  {
	
		for(int ii=0; ii<NUM_MEN; ii++) {
			if( gcls_Status.GetSys(ii) == DIV_SYS_STOP ) continue;
			DetectRun_Cancel(ii);
		}

#ifdef ENABLE_CYCLE
		for(int ii=0; ii<NUM_MEN; ii++) {
			if( gcls_Status.GetCycleSys(ii) == DIV_SYS_STOP ) continue;
			Cy_DetectRun_Cancel(ii);
		}
#endif


		//// オペレータ機能へ現状態を送信
		Send_SoGamenn_MailStatus();
		OpSendFunc::SendOp_Status(); 

		LOG(em_ERR), "[%s] 検査開始アンサータイムアウト 強制検査再起動 要求", my_sThreadName);
		DetectReStart();
		return;
	}

	//// ここまできたら検査中へ遷移
	DetectRun();
}

//------------------------------------------
// 指定面のみの検査中断
// int ou 表:0 裏:1
//------------------------------------------
void MainInstance::DetectRun_Cancel(int ou)
{
	LOG(em_ERR), "[%s] %sの検査中断", my_sThreadName, DivNameManager::GetTorB(ou) );
	if (0 == ou) KizuFunction::SetStatus("HANTEI_TOP_ANS_ID", false, false);
	else	  KizuFunction::SetStatus("HANTEI_BOT_ANS_ID", false, false);
	gcls_Status.SetSys(ou, DIV_SYS_STOP);

	// 検査面状態変更
	int ou_not = ou == 1 ? 0 : 1;				// 他方の面
	gcls_Status.SetMenState(ou_not);		// NGの面と異なる方のみで検査実施

											// 停止処理
	Send_HtDefect_Stop(ou);
	Sleep(500);									// 判定がメールを受け取って、回線を切るまで待ち
	gcls_pHtRecv[ou]->SetEvDetectStop();
}

//==========================================
// 検査中開始処理 (検査中へ)
//==========================================
void MainInstance::DetectRun()
{
	LOG(em_MSG), "[%s] 検査中 処理開始！！", my_sThreadName);

	//======================================================
	//// もろもろのタイマー破棄
	CancelWaitableTimer(m_evTimeWaitStart);

	//======================================================
	//// もろもろ初期化
	gcls_DB.ReSetMaxCount();
	for (int ii = 0; ii < HY_SOCK_NUM; ii++) gcls_pOpS[ii]->gque_Deli.ReSetMaxCount();
	gcls_Plg.CoilInit();						// 距離を初期化しておかないと、ステータス変更から機器状態が変更されて状態が変わる為
	gcls_pCoil->Init( (DIV_KADOU_RETRY==gcls_Status.GetKadou() ? true : false) );	// コイル情報初期化
	// 前回送信したコマンド情報を初期化
	memset(m_strCmd, 0x00, sizeof(m_strCmd));

	// --->>> 20240122 Add Kato       
        // FrameNoトラッキングを停止 (DetectStop関数内で全要素解放しているが、微妙なタイミングで全要素解放後にPlgManager内で要素追加される場合があるための対策)
        for (int ii = 0; ii < MAX_TR_FRAME; ii++)
        {
             gcls_TrFrm.DelAll(ii);
        }
	// <<<--- 20240122 Add Kato       

	// 検査開始時に正常復帰する機器状態をセット
	//////gcls_Status.SetStatusQue(gcls_Status.GetStatusRead().PlgDiff.id,	   true);	// シーケンサ測長ずれ

	//======================================================
	//// ステータス変更 検査中へ
	gcls_Status.SetKadou( DIV_KADOU_START );
	for(int ii=0; ii<NUM_MEN; ii++) {
		if( gcls_Status.GetSys(ii) == DIV_SYS_STOP ) continue;
		gcls_Status.SetSys(ii, DIV_SYS_RUN );
	}
#ifdef ENABLE_CYCLE
	for(int ii=0; ii<NUM_MEN; ii++) {
		if( gcls_Status.GetCycleSys(ii) == DIV_SYS_STOP ) continue;
		gcls_Status.SetCycleSys(ii, DIV_SYS_RUN );
	}
#endif

	//// 両面NGはありえない
	if( DIV_SYS_RUN == gcls_Status.GetSys(0) && DIV_SYS_RUN == gcls_Status.GetSys(1) ) {	// 両面検査実施
		gcls_Status.SetMenState(-1);
	} else if( DIV_SYS_RUN == gcls_Status.GetSys(0) ) {						// 表面のみ検査実施
		gcls_Status.SetMenState(0);
	} else if( DIV_SYS_RUN == gcls_Status.GetSys(1) ) {						// 裏面のみ検査実施
		gcls_Status.SetMenState(1);
	}


	//======================================================
	// カメラ調整以外の処理
	if(DIV_UNTEN_CAMERA != gcls_Status.GetUnten()) {
		// 初回コイルから通板表示する
		OpSendFunc::SendOp_CoilInf_Ken( gcls_pCoil->GetCoilBaseKen() );		// デフォルトの検査用コイル情報
		OpSendFunc::SendOp_CoilInf_Dsp( gcls_pCoil->GetCoilBaseDsp() );		// いきなり表示用のコイル情報を送信
		Send_HyDefect_MaeRead( gcls_pCoil->GetCoilBaseKen()->cKizukenNo );	// 表示PCの共有メモリ初期化の為に通知

#ifndef SENDOP_LENGTH_PLG
		//// 検査開始一本目のコイル情報を表示するために表示機能へ送信
		// 通板位置情報
		OpSendFunc::SendOp_Length();
#endif

		// 判定受信クラスの検査開始通知
		for(int ii=0; ii<NUM_MEN; ii++) {
			if( gcls_Status.GetSys(ii) == DIV_SYS_STOP ) continue;
			gcls_pHtRecv[ii]->SetEvDetectRun();
		}
#ifdef ENABLE_CYCLE
		for(int ii=0; ii<NUM_MEN; ii++) {
			if( gcls_Status.GetCycleSys(ii) == DIV_SYS_STOP ) continue;
			gcls_pCyRecv[ii]->SetEvDetectRun();
		}
#endif

#ifdef ENABLE_RECORDER_UDP
		gcls_CmdSender.SendCommand_Start();					// 検査開始コマンド送信
#endif

		// ステータス情報を再読み込み
		gcls_Status.RefreshStatus();


		// iPort使用時、ここでiPort初期化を待つ
#ifndef LOCAL		// テスト時は待つ必要なし
	//	LOG(em_INF), "[%s] カメラ初期化待ち・・・", my_sThreadName);
	//	Sleep(5000);		// 判定等もろもろが準備完了するまでちょっと待ち (今野さんからのインプット)
#endif

		//// サンプリング開始
		gcls_pSeqFrameR->SetRun(true);
		gcls_Plg.SetEvSampStart();

	}

	//======================================================
	//// カメラコントローラへパルスONを通知 (このタイミングで出力するのは、通常運転のみ)
	// サンプル撮影も追加
	if( DIV_UNTEN_NOMAL == gcls_Status.GetUnten() || DIV_UNTEN_SMYU  == gcls_Status.GetUnten() || DIV_UNTEN_SAMP  == gcls_Status.GetUnten() ) {
		// PLGカウンタ値 リセット要求
#ifndef DISABLE_PLG_RESET
		Sleep(1000);		// シーケンサが初期化出来るまで待ち
		gcls_Dmem.PoPuls_CountStart();
		syslog(241, "探傷開始時に自動リセット");
		LOG(em_INF), "[%s] シーケンサPLGカウント開始要求(撮像開始時)[Exsync値=%d][Plg値=%d]", my_sThreadName, gcls_Dmem.GetDmRecv().ken.nLvalAdd, gcls_Dmem.GetDmRecv().ken.nPlgAdd);
		Sleep(500);			// シーケンサに開始要求をしてからカウントスタートするまで待ち
#endif
		ToOutMailSender::Send_ToCamera_Palse(true);
		LOG(em_INF), "[%s] カメラPLGカウントスタート[Exsync値=%d][Plg値=%d]", my_sThreadName, gcls_Dmem.GetDmRecv().ken.nLvalAdd, gcls_Dmem.GetDmRecv().ken.nPlgAdd);
	}

	//======================================================
	//// オペレータ機能へ稼動状態を送信
	Send_SoGamenn_MailStatus();
	OpSendFunc::SendOp_Status(); 
}


//==========================================
// 検査停止 処理 (判定検査停止アンサー待ちへ)
//------------------------------------------
// bool		bMode			: true:正常停止、false:異常停止	
//==========================================
void MainInstance::DetectStop(bool bMode)
{
	//======================================
	// もろもろのタイマー破棄
	CancelWaitableTimer(m_evTimeReStart);
	CancelWaitableTimer(m_evTimeInitStart);
	CancelWaitableTimer(m_evTimeWaitStart);
	CancelWaitableTimer(LogicParaRunFunc::GetEvTimeCamset());
	CancelWaitableTimer(LogicParaRunFunc::GetEvTimeResumeStart());

	gcls_pTrKenDistPos->TrackingStop();												// トラッキング中ならそれも停止
	gcls_pTlCoilOutSt->TrackingStop();												// トラッキング中ならそれも停止

	// 出側物理コイル単位の状態管理用トラッキング キャンセル
	gcls_pTlCoilOutSt->QueAllFree();												// トラッキング中ならそれも停止

	for(int ii=0; ii<MAX_TR_FRAME; ii++) gcls_TrFrm.DelAll(ii);						// FrameNoトラッキングを停止

	// コマンドリストクリア
	LogicParaRunFunc::ClearLinkswCmd();

	//======================================
	// 実績締めチェック
	bool bResult = false;
	if( DIV_KADOU_STOP   != gcls_Status.GetKadou() &&
		DIV_KADOU_RETRY  != gcls_Status.GetKadou() &&
		DIV_UNTEN_CAMERA != gcls_Status.GetUnten() )		bResult = true;


	//======================================
	// ステータス変更
	gcls_Status.SetKadou( DIV_KADOU_STOP );
	for(int ii=0; ii<NUM_MEN; ii++)	gcls_Status.SetSys(ii, DIV_SYS_WAITSTOP );
	if (DIV_UNTEN_CAMERA != gcls_Status.GetUnten())
	{
		// 「カメラ校正」モード以外
		for (int ii = 0; ii<NUM_ALL_HTPARA; ii++) gcls_Status.SetParaSys(ii, DIV_SYS_WAITSTOP);
	}


#ifdef CAM_ADJUST_REMOTE
	//======================================================
	// カメラ調整時
	//	※ リモートデスクトップ時のみ
	if( DIV_UNTEN_CAMERA == gcls_Status.GetUnten() ) {
		gcls_Status.SetUnten( DIV_UNTEN_NOMAL );	// 通常に戻しておく
		gcls_Status.SetKadou( DIV_KADOU_STOP );
		for(int ii=0; ii<NUM_MEN; ii++) {
			gcls_Status.SetSys(ii, DIV_SYS_STOP );
		}

		//// オペレータ機能へを送信
		Send_SoGamenn_MailStatus();
		OpSendFunc::SendOp_Status(); 
		return ;
	}
#endif



	//======================================
	// 停止準備 (第1段)
	//	・サンプリング停止
	//	・判定に検査停止を通知
	//	・カメラコントローラーへパルス出力停止を通知
	//  ・LED照明 消灯
	//	・判定との回線切断

	//// サンプリング停止
	if(DIV_UNTEN_CAMERA != gcls_Status.GetUnten()){
		gcls_pSeqFrameR->SetRun(false);
		gcls_Plg.SetEvSampStop();
	}

#ifdef ENABLE_RPI_SAVE
	// 全長画像保存停止
	gcls_IfRpiSave.Send_TiRpiSave_Stop();
#endif
#ifdef ENABLE_RECORDER_UDP
	// 調整用レコーダーへ検査停止コマンド送信
	gcls_CmdSender.SendCommand_Stop();
#endif

	//// 判定に検査停止を通知
	Send_HtDefect_Stop();
	Sleep(2000);			// 判定等諸々が停止処理を完了するまで待ち
	
	// PLGカウンタ値 リセット要求
#ifndef DISABLE_PLG_RESET
	gcls_Dmem.PoPuls_CountReset();
	syslog(242, "探傷停止時に自動リセット");
	LOG(em_INF), "[%s] シーケンサPLGカウントリセット(検査終了時)[Exsync値=%d][Plg値=%d]", my_sThreadName, gcls_Dmem.GetDmRecv().ken.nLvalAdd, gcls_Dmem.GetDmRecv().ken.nPlgAdd);
#endif

	//// カメラコントローラにパルス出力停止を通知
	ToOutMailSender::Send_ToCamera_Palse(false);


	//// LED照明をとめる
	for(int ii=0; ii<NUM_MEN; ii++){
		ToOutMailSender::Send_KtLedMgr_OnOff(ii, FALSE);
	}

	//// 判定との回線切断
	for(int ii=0; ii<NUM_MEN; ii++){
		gcls_pHtRecv[ii]->SetEvDetectStop();
	}
	Sleep(20);				// 停止処理完了を待つ

	
	//======================================
	// 実績の締め
	if(bResult) {
		int     endmode = bMode ? DIV_END_STOP_MANUAL : DIV_END_STOP_AUTO;
		
		double dKenLen = gcls_Plg.GetLen_KenDist() + gcls_Plg.GetDistance();
		double dKenLenSub = gcls_Dmem.GetDmRecv().nLengthSub + gcls_Plg.GetDistance();

		gcls_pCoil->SetData_CoilResult_Real( endmode, dKenLen, dKenLenSub);
		gcls_pCoil->SetData_CoilResult_Virtual();

		double	dInLen	= gcls_Plg.GetLen_Dsp(POS_DSP_SHR_I);		// シャーカット位置での入側論理コイル距離
		double	dOutLen	= gcls_Plg.GetLen_OutCut();					// シャーカット位置での出側物理コイル距離
		
		// 出側単位
		//gcls_pCoil->SetData_CoilOutInf_Real(dInLen, dOutLen);
		//gcls_pCoil->SetData_CoilOutInf_Virtual(dInLen, dOutLen, endmode);

		// 最後のシャーカット登録
		gcls_pCoil->SetData_ShaCut(dKenLen, 1);

		gcls_pCoil->AllDelete();
	}


	//======================================
	// 終了処理

	//// ステータス変更
	for(int ii=0; ii<NUM_MEN; ii++) {
		gcls_Status.SetSys(ii, DIV_SYS_STOP );
	}
	if (DIV_UNTEN_CAMERA != gcls_Status.GetUnten())
	{
		// 「カメラ校正」モード以外
		for (int ii = 0; ii < NUM_ALL_HTPARA; ii++)
		{
			gcls_Status.SetParaSys(ii, DIV_SYS_STOP);
		}
	}
	LOG(em_MSG), "[%s] 検査停止！！", my_sThreadName);

	//// オペレータ機能へ現状態を送信
	Send_SoGamenn_MailStatus();
	OpSendFunc::SendOp_Status(); 
}

//==========================================
// 検査初期化待ち処理		(タイムアウト発生)
//==========================================
void MainInstance::WaitInit_TimeOut()
{
	////////////////////////////////////
	// 事前チェック
	////////////////////////////////////
	//// ステータス確認
	for( int ii=0; ii<NUM_MEN; ii++ )
	{
		if( DIV_SYS_INITSTART != gcls_Status.GetSys(ii) &&
			DIV_SYS_STOP      != gcls_Status.GetSys(ii) )
		{
			LOG(em_ERR), "[%s] 検査初期化アンサータイムアウト ステータス異常 (%d=%s)", my_sThreadName, ii, GetDivSys( gcls_Status.GetSys(ii) ));
			return;
		}
	}

	//// 機器状態 変更
	KizuFunction::SetStatus( "CAMLINKSW_ANS_ID", false );

	//// 検査停止・開始
	LOG(em_ERR), "[%s] 検査初期化アンサータイムアウト 強制検査再起動 要求", my_sThreadName);
	syslog(235, "[検査初期化アンサータイムアウト]");
	DetectStop( false );							// 検査停止
	DetectReStart();								// 強制検査再起動


}


//==========================================
// 判定問い合わせアンサー
//------------------------------------------
// COMMON_QUE* que 起動要因
//==========================================
void MainInstance::HT_StatAnswer(COMMON_QUE* que)
{
	// 下準備
	int campair = que->fl.data[0];				// カメラペア (1オリジン)
	int pcid	= que->fl.data[1];				// PCID (1オリジン)
	int ans		= que->fl.data[2];				// 結果 (0:正常 1:軽故障 -1:重故障)

	CString wk;
	wk.Format("HANTEI_ANS_%d_ID", pcid );

	//// 判定状態セット
	if(0 == ans) {
		if( 0 != gcls_Status.GetStatusRead().Ht_NonAnswer[pcid-1].stat ) {
			gcls_Status.SetStatusQue(wk, true);
		}
	} else {
		if( 0 == gcls_Status.GetStatusRead().Ht_NonAnswer[pcid-1].stat ) {
			gcls_Status.SetStatusQue(wk, false);
		}
	}
}

//==========================================
// 各検査装置位置でのコイル切り替え
//------------------------------------------
// int	ou	: 0:表 1:裏	
// ENUM_END_MODE enEndMode
//==========================================
void MainInstance::ChangeCoil_Ken(int ou)
{
	// 基準面？
	bool  isDistPos;					// 基準面のコイル切り替え時 true
	if (ou == gcls_pParam->GetParamCommon().nDistPosition)	isDistPos = true;
	else													isDistPos = false;

	//// PLG切り替え制御
	//gcls_Plg.CoilChange_Ken(ou, emEndMode);
	gcls_Plg.CoilChange_Ken(ou);

	//// 基準面の場合のみ 実コイルの切り替え
	if (isDistPos) {
		double dKenLen = gcls_Plg.GetCoilOldLen_Ken();
		double dKenLenSub = gcls_Dmem.GetDmRecv().nWpdOnLengthSub;
		gcls_pCoil->SetData_CoilResult_Real(DIV_END_NORMAL, dKenLen, dKenLenSub);		// 前コイルの実績の締め
		gcls_pCoil->ChangeCoilKenBase();										// 基準面のコイル切り替え
		//if (DIV_END_BACKUP == emEndMode)	gcls_pCoil->ChangeCoilKenBase(false, false);	// 基準面のコイル切り替え コイル長上限到達
		//else								gcls_pCoil->ChangeCoilKenBase(true, false);		//						　通常切替

	}
	else {
		gcls_pCoil->ChangeCoilKenBase_NotPos();												// 他方面のコイル切り替え
	}

	//// 検査基準位置～シャー位置までトラッキング中の距離を変更する。
	//gcls_pCoil->TrkDistChange(ou, (int)gcls_Plg.GetCoilOldLen_Ken());


	int nSt_Spm	  = gcls_pCoil->GetCoilKenR(ou)->nSt_Spm;
	int nScodePre = gcls_pCoil->GetCoilIndex(gcls_pCoil->GetKenRecBase(), 1)->setting.scode[ou];
	int nScodeNow = (1 == nSt_Spm ? gcls_pCoil->GetCoilKenR(ou)->setting.scode[ou] : gcls_pCoil->GetCoilKenR(ou)->setting.scode_spmoff[ou]);

	//// 切り替えたコイルの最適な条件を他機器へ通知	
	if (nScodePre != nScodeNow)
	{
		// 表面状態が変化した時のみ通知
		LOG(em_INF), "[%s] <%s> <%s> コイル切替(FACT_TO_CAMERA_05) 表面状態:%d", my_sThreadName, DivNameManager::GetTorB(ou), GetSpmStat((EM_DIV_SPM_STAT)gcls_Status.GetSpmStat()), nScodeNow);
		ToOutMailSender::Send_ToCamera_CoilChange(ou, nScodeNow);
	}
	else
	{
		LOG(em_INF), "[%s] <%s> 表面状態:%d で変化無しのため、カメラコントローラに通知無し", my_sThreadName, DivNameManager::GetTorB(ou), nScodeNow);
	}
#ifndef KT_LEDMGR
	ToOutMailSender::Send_ToLights_CoilChange(ou, gcls_pCoil->GetCoilBaseKen()->setting.scode[ou]);
#else
	ToOutMailSender::Send_KtLedMgr_CoilChange(ou, nScodeNow);
	LOG(em_INF), "[%s] <%s> <%s> 設定要求(表面状態指定)(FACT_KT_LEDMGR_02) 表面状態:%d", my_sThreadName, DivNameManager::GetTorB(ou), GetSpmStat((EM_DIV_SPM_STAT)gcls_Status.GetSpmStat()), nScodeNow);
#endif

	//// 切り替えたコイルの最適な条件を他スレッドへ通知
	// エッジ整合性判定の未チェック設定
	gcls_EdgePosCont.CoilChange(ou);

	// 複眼ズレスレッド
#ifndef DISABLE_GAP_CALC
	gcls_pGap->AddCoilChange(ou, gcls_pCoil->GetCoilBaseKen()->setting.scode[ou]);
#endif
	
	// 調整用レコーダーにコイル情報送信
#ifdef ENABLE_RECORDER_UDP
	//////int nTiming = GetPrivateProfileInt(TO_DEFECT, "REC_COIL_TIMING", 0, TASKINI_NAME);
	//////// 送信タイミングは、KizuTask.iniの設定に従う
	//////if ((0 == nTiming &&  isDistPos) ||
	//////	(1 == nTiming && !isDistPos)) {
	//////	TYPE_EPC_COIL data	= gcls_pCoil->GetEpcCoilInfo();
	//////	gcls_pCoil->SendUdpAllFrame(&data);							// コイル情報 を EPC製全長保存PCに送信
	//////	LOG(em_INF), "[%s] <%s> レコーダーPCにコイル情報送信 [%.10s]", my_sThreadName, DivNameManager::GetTorB(ou), gcls_pCoil->GetCoilBaseKen()->data_u.CoilInf.KeyInf.cCoilNo);
	//////}
	//if (DIV_END_NORMAL == emEndMode) {
	//	// 調整用レコーダーにコイル情報送信
	//	gcls_CmdSender.SendCommand_CoilInf(ou, gcls_pCoil->GetEpcCoilInfo());
	//	gcls_CmdSender.SendCommand_Param(ou, gcls_pCoil->GetEpcScode(ou));
	//	LOG(em_INF), "[%s] <%s> 調整用レコーダーへUDP送信[%.12s][scode=%d]",
	//		my_sThreadName, (0 == ou ? "表面" : "裏面"), gcls_pCoil->GetEpcCoilInfo().cCoilNo, gcls_pCoil->GetEpcScode(ou));
	//}
	// 調整用レコーダーにコイル情報送信
	gcls_CmdSender.SendCommand_CoilInf(ou, gcls_pCoil->GetEpcCoilInfo());
	int nEpcScode = (gcls_Status.IsSpmStat() ? gcls_pCoil->GetEpcScode(ou) : gcls_pCoil->GetEpcScode_spmoff(ou));
	gcls_CmdSender.SendCommand_Param(ou, nEpcScode);
	LOG(em_INF), "[%s] <%s> 調整用レコーダーへUDP送信[%.12s][scode=%d][NowF=%d]",
		my_sThreadName, DivNameManager::GetTorB(ou), gcls_pCoil->GetEpcCoilInfo().cCoilNo, nEpcScode, gcls_Plg.GetFrameNo());
#endif

	// 前回送信したコマンド情報をクリア
	memset(m_strCmd, 0x00, sizeof(m_strCmd));
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////
//// WPD制御
//==========================================
// 検査用WPD認識
//------------------------------------------
// int nIsWpd		: 0:WPD, 1:異種点, 2:ゲート信号	
// double *pSpmDiff : SPM状態による伸び量[mm]
//==========================================
void MainInstance::WPD_KenBase(int nIsWpd, double *pSpmDiff)
{
	gcls_Lock.LockTh();	//// -------------->>>>>

	//// 整合性チェック
	// ===>> 坂巻 2021.06.03 小コイル長材通販時、コイル長比較が正常にできない不具合修正
	//const COIL_INF*  pCoil = gcls_pCoil->GetCoilBaseKen();
	int nRecKenBase = gcls_pCoil->GetKenRecBase();
	COIL_INF*  pCoil = gcls_pCoil->GetCoilRec_DataSet(nRecKenBase);
	// <<===
	// フレームずれが発生するため、通販距離、フレームNoは同じタイミングで取得する
	double dKenLen		= gcls_Plg.GetLen_KenDist();	// 基準面検査装置位置での通板距離 [mm]
	double dKenLenNot	= gcls_Plg.GetLen_KenDistNot();	// 他方面検査装置位置での通板距離 [mm]
	DWORD nNowFrame		= gcls_Plg.GetFrameNo();		// 現在のフレームNo
	if(!pCoil->bCoilInit) {
		// ===>> 坂巻 2021.06.03 小コイル長材通販時、コイル長比較が正常にできない不具合修正
		//SEQ_DM_RECV_BASE &tbl = gcls_Dmem.GetDmRecv();
		//double dWpdOnKenLen = tbl.nWpdOnLength;
		//double dSubLen = tbl.nWpdOnLengthSub;
		//double dSa = dWpdOnKenLen - dSubLen;	// 整合性チェック

		//if( 0.0 < gcls_pParam->GetCommonMast().dPlgDiffLimit[0] &&
		//	(tbl.nWpdOnLength * gcls_pParam->GetCommonMast().dPlgDiffLimit[0] / 100) <= abs((int)dSa) ) {
		//	LOG(em_ERR), "[PlgManager] PLG 整合性チェック 異常 [検査用PLG=%0.3fm:比較用PLG=%0.3fm:差=%0.3fm]", tbl.nWpdOnLength /1000.0, tbl.nWpdOnLengthSub /1000.0, dSa/1000.0);
		//	syslog(SYSNO_PLG_ERR, "[検査用PLG=%0.3fm:比較用PLG=%0.3fm:差=%0.3fm]", tbl.nWpdOnLength /1000.0, tbl.nWpdOnLengthSub /1000.0, dSa/1000.0);
		//	if( 0== gcls_Status.GetStatusRead().PlgLengthCheckKen.stat) {				// 正常→異常
		//		gcls_Status.SetStatusQue("PLGLEN_KEN_CHECK_ID", false);
		//	}
		//} else {
		//	LOG(em_MSG), "[PlgManager] PLG 整合性チェック 正常 [検査用PLG=%0.3fm:比較用PLG=%0.3fm:差=%0.3fm]", tbl.nWpdOnLength /1000.0, tbl.nWpdOnLengthSub /1000.0, dSa/1000.0);

		//	if( 0!= gcls_Status.GetStatusRead().PlgLengthCheckKen.stat) {				// 異常→正常
		//		LOG(em_MSG), "[PlgManager] PLG 整合性チェック 正常復帰");
		//		gcls_Status.SetStatusQue("PLGLEN_KEN_CHECK_ID", true);
		//	}
		//}

		double dWpdOnKenLen = gcls_Dmem.GetWpdOnLength();
		double dSubLen = gcls_Dmem.GetWpdOnLengthSub();
		double dSa = dWpdOnKenLen - dSubLen;	// 整合性チェック
		pCoil->dWpdOnLength = dWpdOnKenLen;	// WPD ON時のコイル長を保持

		if (0.0 < gcls_pParam->GetCommonMast().dPlgDiffLimit[0] &&
			(dWpdOnKenLen * gcls_pParam->GetCommonMast().dPlgDiffLimit[0] / 100) <= abs((int)dSa)) {
			LOG(em_ERR), "[PlgManager] PLG 整合性チェック 異常 [検査用PLG=%0.3fm:比較用PLG=%0.3fm:差=%0.3fm]", dWpdOnKenLen / 1000.0, dSubLen / 1000.0, dSa / 1000.0);
			syslog(SYSNO_PLG_ERR, "[検査用PLG=%0.3fm:比較用PLG=%0.3fm:差=%0.3fm]", dWpdOnKenLen / 1000.0, dSubLen / 1000.0, dSa / 1000.0);
			if (0 == gcls_Status.GetStatusRead().PlgLengthCheckKen.stat) {				// 正常→異常
				gcls_Status.SetStatusQue("PLGLEN_KEN_CHECK_ID", false);
			}
		}
		else {
			LOG(em_MSG), "[PlgManager] PLG 整合性チェック 正常 [検査用PLG=%0.3fm:比較用PLG=%0.3fm:差=%0.3fm]", dWpdOnKenLen / 1000.0, dSubLen / 1000.0, dSa / 1000.0);

			if (0 != gcls_Status.GetStatusRead().PlgLengthCheckKen.stat) {				// 異常→正常
				LOG(em_MSG), "[PlgManager] PLG 整合性チェック 正常復帰");
				gcls_Status.SetStatusQue("PLGLEN_KEN_CHECK_ID", true);
			}
		}
		// <<===

		// 検査LAN HUBの異常で統括-判定間の回線切断が発生時
		// [判定:検査停止、統括:検査中]のアンマッチ状態となるが
		// 物理コイルは切り替わるが、論理コイルが切り替わらない状態で
		// 異常検出されないため、コイル切替えのタイミングでチェックを行う
		for (int ii = 0; ii<NUM_MEN; ii++) {
			bool bFrameNo = true;
			if (DIV_SYS_RUN != gcls_Status.GetSys(ii)) {
				// 検査中以外はチェック対象外
				continue;
			}

			for (int jj = 0; jj<NUM_CAM_POS; jj++) {
				DWORD nFrameNo_V = gcls_pHtRecv[ii]->GetFrameNo(jj);
				if (m_nFrameNo_V[ii][jj] == nFrameNo_V) {
					LOG(em_ERR), "[%s] 撮像フレーム停止検出 <%s>[Cam:%d FrameNo:%d]", my_sThreadName, DivNameManager::GetTorB(ii), (jj + 1) + (ii*NUM_CAM_POS), nFrameNo_V);
					syslog(245, "撮像フレーム停止検出 <%s>[Cam:%d FrameNo:%d]", DivNameManager::GetTorB(ii), (jj + 1) + (ii*NUM_CAM_POS), nFrameNo_V);

					bFrameNo = false;
				}
				m_nFrameNo_V[ii][jj] = nFrameNo_V;			// 前回フレームNo保持
			}
			if (!bFrameNo) {
				if (0 == gcls_Status.GetStatusRead().FrameGrabStop[ii].stat) {				// 正常→異常
					LOG(em_ERR), "[%s] 撮像フレーム停止検出 機器状態更新 <%s>", my_sThreadName, DivNameManager::GetTorB(ii));
					gcls_Status.SetStatusQue(gcls_Status.GetStatusRead().FrameGrabStop[ii].id, false);
				}
			} else {
				if (0 != gcls_Status.GetStatusRead().FrameGrabStop[ii].stat) {				// 異常→正常
					LOG(em_MSG), "[%s] 撮像フレーム復旧検出 機器状態更新 <%s>", my_sThreadName, DivNameManager::GetTorB(ii));
					gcls_Status.SetStatusQue(gcls_Status.GetStatusRead().FrameGrabStop[ii].id, true);
				}
			}
		}
	}


	// 検査基点で入側コイル切り替え
	gcls_Plg.CoilChange_KenBase();

	// トラッキング開始
	int nVal = gcls_pParam->GetCommonMast().Dist.nKenBase;
	// SPMが圧下されていたら距離を補正
	if (gcls_Status.IsSpmStat())
	{
		// 検査基点からSPMまでの距離をSPM伸び率による差分を求め加算する
		// ※前コイルの尾端長の補正
		*pSpmDiff = (double)gcls_pParam->GetCommonMast().Dist.nKenBaseSpm * gcls_pCoil->GetLastSpm();
		nVal = (int)ceil((double)nVal + *pSpmDiff);

		LOG(em_MSG), "[%s] SPM伸び率補正(距離) SPM伸び率：%.4f%% 補正前：%.2fm ⇒ 補正後：%.2fm (差分：%.2fmm)", my_sThreadName,
			gcls_pCoil->GetLastSpm(),
			(double)gcls_pParam->GetCommonMast().Dist.nKenBase / 1000.0,
			(double)nVal / 1000.0,
			*pSpmDiff);
	}

	gcls_pTrKenDistPos->ChangeDist(nVal);		// パラメータが変更されてるかも知れないので、毎回セット
	gcls_pTrKenDistPos->g_pAddr = (long*)nIsWpd;
	gcls_pTrKenDistPos->TrackingStart();

	double dLenYotei = dKenLen + nVal;			// コイル予定長セット
	gcls_pCoil->SetData_CoilLenYotei(dLenYotei);

	LOG(em_MSG), "[%s] コイル予定長=%0.3fm(%0.3fm + %0.3fm)", my_sThreadName, dLenYotei / 1000.0, dKenLen / 1000.0, nVal / 1000.0);
	LOG(em_MSG), "[%s] WPD ON時のフレームNo=%lu", my_sThreadName, nNowFrame);

	//// コイル切替時の内部距離カウンタ設定
	SetPosition_CoilChange(nNowFrame, dKenLen, dKenLenNot, *pSpmDiff);
	
	// 既にコイル情報を受信している場合は表示PCに検査用コイル情報を送信する
	if( gcls_pCoil->IsCoilNextKen() ){
		////// コイル情報 を表示PCに送る 
		//// ※ 従来は、前工程情報を前もって取得するためにコイル情報受信時に送信していたが、
		//		検査管理Noをコイル情報受信毎に更新したいためWPD ONかつコイル情報受信済み時に送信する
		OpSendFunc::SendOp_CoilInf_Ken( gcls_pCoil->GetCoilNextKen() );
	}
	gcls_Lock.UnlockTh();	//// <<<<<--------------
}


//==========================================
// 表示用WPD認識
//==========================================
void MainInstance::WPD_DspBase()
{
	gcls_Lock.LockTh();		//// <<<<--------------------

	
	//// 整合性チェック
	const COIL_INF*  pCoil = gcls_pCoil->GetCoilBaseDsp();
	if (!pCoil->bCoilInit) {
		// ===>> 坂巻 2021.06.03 小コイル長材通販時、コイル長比較が正常にできない不具合修正
		//SEQ_DM_RECV_BASE &tbl = gcls_Dmem.GetDmRecv();
		//double dKenLen = tbl.nWpdOnLength;
		//double dDspLen = tbl.nWpdOnLengthDsp;
		//double dSa = dKenLen - dDspLen;	// 整合性チェック

		//if( 0.0 < gcls_pParam->GetCommonMast().dPlgDiffLimit[1] &&
		//	(tbl.nWpdOnLength * gcls_pParam->GetCommonMast().dPlgDiffLimit[1] / 100) <= abs((int)dSa) ) {
		//	LOG(em_ERR), "[PlgManager] 表示PLG 整合性チェック 異常 [検査用PLG=%0.3fm:表示用PLG=%0.3fm:差=%0.3fm]", tbl.nWpdOnLength /1000.0, tbl.nWpdOnLengthDsp /1000.0, dSa/1000.0);
		//	syslog(SYSNO_PLG_ERR, "[検査用PLG=%0.3fm:表示用PLG=%0.3fm:差=%0.3fm]", tbl.nWpdOnLength /1000.0, tbl.nWpdOnLengthDsp /1000.0, dSa/1000.0);
		//	if( 0==gcls_Status.GetStatusRead().PlgLengthCheckDsp.stat) {				// 正常→異常
		//		gcls_Status.SetStatusQue("PLGLEN_DSP_CHECK_ID", false);
		//	}
		//} else {
		//	LOG(em_MSG), "[PlgManager] 表示PLG 整合性チェック [検査用PLG=%0.3fm:表示用PLG=%0.3fm:差=%0.3fm]", tbl.nWpdOnLength /1000.0, tbl.nWpdOnLengthDsp /1000.0, dSa/1000.0);

		//	if( 0!= gcls_Status.GetStatusRead().PlgLengthCheckDsp.stat) {				// 異常→正常
		//		LOG(em_MSG), "[PlgManager] 表示PLG 整合性チェック 正常復帰");
		//		gcls_Status.SetStatusQue("PLGLEN_DSP_CHECK_ID", true);
		//	}
		//}
		double dKenLen = pCoil->dWpdOnLength;
		double dDspLen = gcls_Dmem.GetWpdOnLengthDsp();
		double dSa = dKenLen - dDspLen;	// 整合性チェック

		if (0.0 < gcls_pParam->GetCommonMast().dPlgDiffLimit[1] &&
			(dKenLen * gcls_pParam->GetCommonMast().dPlgDiffLimit[1] / 100) <= abs((int)dSa)) {
			LOG(em_ERR), "[PlgManager] 表示PLG 整合性チェック 異常 [検査用PLG=%0.3fm:表示用PLG=%0.3fm:差=%0.3fm]", dKenLen / 1000.0, dDspLen / 1000.0, dSa / 1000.0);
			syslog(SYSNO_PLG_ERR, "[検査用PLG=%0.3fm:表示用PLG=%0.3fm:差=%0.3fm]", dKenLen / 1000.0, dDspLen / 1000.0, dSa / 1000.0);
			if (0 == gcls_Status.GetStatusRead().PlgLengthCheckDsp.stat) {				// 正常→異常
				gcls_Status.SetStatusQue("PLGLEN_DSP_CHECK_ID", false);
			}
		}
		else {
			LOG(em_MSG), "[PlgManager] 表示PLG 整合性チェック [検査用PLG=%0.3fm:表示用PLG=%0.3fm:差=%0.3fm]", dKenLen / 1000.0, dDspLen / 1000.0, dSa / 1000.0);

			if (0 != gcls_Status.GetStatusRead().PlgLengthCheckDsp.stat) {				// 異常→正常
				LOG(em_MSG), "[PlgManager] 表示PLG 整合性チェック 正常復帰");
				gcls_Status.SetStatusQue("PLGLEN_DSP_CHECK_ID", true);
			}
		}
		// <<===
	}


	//// 同期実行する為に、各クラスでクリティカルセクションを定義しておく
	//// コイル切替が成功した場合のみ、PLGも初期化する (失敗の場合は、今の表示用コイルがずっと続いているようにする)

	double dNowDspLen = 0.0;
#ifdef DSP_NO_TRACKING
	dNowDspLen = gcls_Plg.GetCoilOldLen_Ken();
#else
	dNowDspLen = gcls_Plg.GetLen_BaseDsp();
#endif
	// 表示用のコイル情報を切替
	if (gcls_pCoil->ChangeCoilDspBase(dNowDspLen)) {
		// 表示用のPLG情報を切替
		gcls_Plg.CoilChange_BaseDsp();
	}

	gcls_Lock.UnlockTh();	//// -------------------->>>>
}


//==========================================
// コイル切替時の内部距離カウンタ設定要求
//==========================================
void MainInstance::SetPosition_CoilChange(DWORD nNowFrame, double dKenDist, double dKenDistNot, double dSpmDiff)
{
	double dFrameLen	= gcls_Plg.GetResV() * SIZE_FRAME_IMG_Y;		// 1フレーム長さ[mm]
	double dDistLen		= gcls_pParam->GetParamCommon().dDistLen;		// 表裏間距離[mm]
	int nKenBase		= gcls_pParam->GetCommonMast().Dist.nKenBase;	// 検査基点～上流カメラ[mm]

	bool  isDistPos;					// 基準面 true
	NCL_CMD_SET_POSITION	cmd;		// 内部距離カウンタ構造体

	memset(&cmd, 0x00, sizeof(cmd));
	cmd.cmd = NCL_SET_POSITION;
	cmd.update_mask = FRONT | TAIL;
	for(int ii=0; ii<NUM_MEN; ii++) {
		// 基準面？
		if(ii == gcls_pParam->GetParamCommon().nDistPosition)	isDistPos = true;
		else													isDistPos = false;

		//double dLen	= gcls_pParam->GetCommonMast().Dist.nKenBase;			// 検査基点～上流カメラ[mm]
		//if(!isDistPos) dLen += gcls_pParam->GetParamCommon().dDistLen;		// 検査基点～下流カメラ[mm]

		//double front_pos	= 0.0;
		//int    framenum		= 0;
		//// 他方面のコイル先頭フレームは、0mとならない。表裏間距離がフレーム単位で割り切れない分の誤差をここで吸収。(表裏間距離を厳密に考慮する)
		//// 実行フレーム番号も他方面は前回コイル長を超過した時に切替のため、+1フレームとなる
		////if(!isDistPos) front_pos = (int)((gcls_pParam->GetCommonMast().Dist.nKenBase / dDistance) + 1) * dDistance - gcls_pParam->GetParamCommon().dDistLen;
		//if(!isDistPos) front_pos = ((int)(dDistLen/dFrameLen)+1) * dFrameLen - dDistLen;
		//if(!isDistPos) framenum = 1;

		//cmd.framenum = (int)ceil(dLen / dFrameLen) + nNowFrame + framenum;	// 実行フレーム番号
		//cmd.front_pos = (float)front_pos;									// 加算カウンタ[mm]
		//cmd.tail_pos = (float)gcls_pCoil->GetCoilNextKen()->data.nLenIn * 1000;	// 減算カウンタ(予定コイル長)[mm]

		int nOrgFrameNo = 0;
		if(isDistPos) {
			nOrgFrameNo = (int)ceil(nKenBase / dFrameLen) + nNowFrame;
			cmd.framenum = (int)ceil(((double)nKenBase + dSpmDiff) / dFrameLen) + nNowFrame;	// 実行フレーム番号
			cmd.front_pos = (float)0.0;											// 加算カウンタ[mm]
			cmd.tail_pos = (float)gcls_pCoil->GetCoilNextKen()->data.nLenIn;	// 減算カウンタ(予定コイル長)[mm]
		} else {
			double dCoilLen_Ken	= (int)ceil((dKenDist + nKenBase + dSpmDiff) / dFrameLen) * dFrameLen;	// 基準面のコイル切替距離(SPM圧下あり)
			double dOrgCoilLen_Ken = (int)ceil((dKenDist + nKenBase) / dFrameLen) * dFrameLen;			// 基準面のコイル切替距離(SPM圧下なし)
			double dLen	= dCoilLen_Ken - dKenDistNot;							// 基準面切替距離 - 他方面通板距離 [mm]
			
			nOrgFrameNo = (int)ceil((dOrgCoilLen_Ken - dKenDistNot) / dFrameLen) + nNowFrame;
			//nOrgFrameNo = (int)ceil(dLen / dFrameLen) + nNowFrame;

			int nChgFrame = (int)ceil(dLen / dFrameLen);			// 他方面の現在位置からコイル切替までのフレーム数
			//int nChgFrame = (int)ceil((dLen + dSpmDiff) / dFrameLen);			// 他方面の現在位置からコイル切替までのフレーム数
			// 他方面のコイル先頭フレームは、0mとならない。表裏間距離がフレーム単位で割り切れない分の誤差をここで吸収。(表裏間距離を厳密に考慮する)
			double front_pos	= (dKenDistNot + (nChgFrame * dFrameLen)) - dCoilLen_Ken;	// 他方面のコイル切替時の先頭位置
			cmd.framenum = nChgFrame + nNowFrame;								// 実行フレーム番号
			cmd.front_pos = (float)front_pos;									// 加算カウンタ[mm]
			cmd.tail_pos = (float)gcls_pCoil->GetCoilNextKen()->data.nLenIn - (float)front_pos;	// 減算カウンタ(予定コイル長)[mm]
		}

		LOG(em_INF), "[%s] SPM伸び率補正(実行フレーム) [%s] (SPM diff：%.2fmm 1frame：%.2fmm) 補正前=%d ⇒ 補正後=%d", my_sThreadName, DivNameManager::GetTorB(ii), dSpmDiff, dFrameLen, nOrgFrameNo, cmd.framenum);

		// パラメータ切替要求
		LOG(em_INF), "[%s] パラメータ切替要求 [%s] FrameNo:%d(%0.3fm) (実行=%d Scode:%d)", my_sThreadName, DivNameManager::GetTorB(ii), nNowFrame, gcls_Plg.GetLen_Ken(ii)/1000.0, cmd.framenum, gcls_pCoil->GetCoilNextKen()->setting.scode[ii]);
		int scode	= MAX_SCODE;
		if(0 != gcls_pCoil->GetCoilNextKen()->setting.scode[ii]) scode = gcls_pCoil->GetCoilNextKen()->setting.scode[ii];
		Send_HtDefect_Param(cmd.framenum, ii, scode, DIV_HANTEI_NON);

		LOG(em_INF), "[%s] 内部距離カウンタ設定 [%s] FrameNo:%d(%0.3fm) (実行=%d F:%.0fmm T:%.0fmm)", my_sThreadName, DivNameManager::GetTorB(ii), nNowFrame, gcls_Plg.GetLen_Ken(ii)/1000.0, cmd.framenum, cmd.front_pos, cmd.tail_pos);
		Send_HtDefect_SetPosition(ii, DIV_HANTEI_NON, &cmd);				// 内部距離カウンタ設定要求

#ifdef ENABLE_RPI_SAVE
		// 全長切り替え
		LOG(em_MSG), "[%s] FACT_TI_RPISAVE_04 送信 [men=%d][%s][%s][fno=%d][fpos=%d]"
			, my_sThreadName, ii, gcls_pCoil->GetCoilNextKen()->cKizukenNo, gcls_pCoil->GetCoilNextKen()->data.cCoilNoIn, cmd.framenum, cmd.front_pos);
		mcls_pIfRpiSave->Send_TiRpiSave_CoilSend(gcls_pCoil->GetCoilNextKen()->cKizukenNo, &gcls_pCoil->GetCoilNextKen()->setting, &gcls_pCoil->GetCoilNextKen()->data, cmd.framenum, (int)cmd.front_pos, ii);
#endif
		// 今回送信のコマンド情報を保持
		memcpy(&m_strCmd[ii], &cmd, sizeof(NCL_CMD_SET_POSITION));

		// 実行フレームNoを保持
		// PLGのコイル切替フレームNo認識に使用する
		gcls_Plg.SetCoilChange_Frame(ii, cmd.framenum);
	}
}

//==========================================
// コイル強制切替時の内部距離カウンタ設定要求(強制切替)
//==========================================
void MainInstance::SetPosition_ForceCoilChange(DWORD nNowFrame, double dKenDist, double dKenDistNot)
{
	double dFrameLen	= gcls_Plg.GetResV() * SIZE_FRAME_IMG_Y;		// 1フレーム長さ[mm]
	double dDistLen		= gcls_pParam->GetParamCommon().dDistLen;		// 表裏間距離[mm]
	int nKenBase		= gcls_pParam->GetCommonMast().Dist.nKenBase;	// 検査基点～上流カメラ[mm]

	bool  isDistPos;					// 基準面 true
	NCL_CMD_SET_POSITION	cmd;		// 内部距離カウンタ構造体

	memset(&cmd, 0x00, sizeof(cmd));
	cmd.cmd = NCL_SET_POSITION;
	cmd.update_mask = FRONT | TAIL;
	for(int ii=0; ii<NUM_MEN; ii++) {
		// 基準面？
		if(ii == gcls_pParam->GetParamCommon().nDistPosition)	isDistPos = true;
		else													isDistPos = false;

		//double dLen			= 0.0;
		//if(!isDistPos) dLen += gcls_pParam->GetParamCommon().dDistLen;		// 検査基点～下流カメラ[mm]

		//double front_pos	= 0.0;
		//int    framenum		= 0;
		//// 他方面のコイル先頭フレームは、0mとならない。表裏間距離がフレーム単位で割り切れない分の誤差をここで吸収。(表裏間距離を厳密に考慮する)
		//// 実行フレーム番号も他方面は前回コイル長を超過した時に切替のため、+1フレームとなる
		//if(!isDistPos) front_pos = ((int)(dDistLen/dFrameLen)+1) * dFrameLen - dDistLen;
		//if(!isDistPos) framenum = 1;

		//cmd.framenum = (int)ceil(dLen / dFrameLen) + nNowFrame + framenum;	// 実行フレーム番号(次フレームで切替し、次々フレームで次コイル開始)
		//cmd.front_pos = (float)front_pos;									// 加算カウンタ[mm]
		//cmd.tail_pos = (float)gcls_pCoil->GetCoilNextKen()->data.nLenIn * 1000;	// 減算カウンタ(予定コイル長)[mm]
		

		if(isDistPos) {
			// 2024.02.22 mizoguchi --->>>
			//cmd.framenum = 0;													// 実行フレーム番号
			cmd.framenum = nNowFrame+1;													// 実行フレーム番号
			// 2024.02.22 mizoguchi <<<---
			cmd.front_pos = (float)0.0;											// 加算カウンタ[mm]
			cmd.tail_pos = (float)gcls_pCoil->GetCoilNextKen()->data.nLenIn;	// 減算カウンタ(予定コイル長)[mm]
		} else {
			double dCoilLen_Ken	= dKenDist + (1.0 * dFrameLen);					// 基準面のコイル切替距離
			double dLen	= dCoilLen_Ken - dKenDistNot;							// 基準面切替距離 - 他方面通板距離 [mm]
			int nChgFrame = (int)ceil(dLen / dFrameLen);						// 他方面の現在位置からコイル切替までのフレーム数
			// 他方面のコイル先頭フレームは、0mとならない。表裏間距離がフレーム単位で割り切れない分の誤差をここで吸収。(表裏間距離を厳密に考慮する)
			double front_pos	= (dKenDistNot + (nChgFrame * dFrameLen)) - dCoilLen_Ken;	// 他方面のコイル切替時の先頭位置
			cmd.framenum = nChgFrame + nNowFrame;								// 実行フレーム番号
			cmd.front_pos = (float)front_pos;									// 加算カウンタ[mm]
			cmd.tail_pos = (float)gcls_pCoil->GetCoilNextKen()->data.nLenIn - (float)front_pos;	// 減算カウンタ(予定コイル長)[mm]
		}

		// ===>> 坂巻　2018.10.21 コイル情報受信～コイル切り替えの間にイニシャル伝文を受信したとき、エンジン強制切り替えがされない不具合
		// 未実行のPARAM,SET_POSITIONを消去
		LOG(em_INF), "[%s] 前回送信の未実行設定のキャンセル要求", my_sThreadName);
		Send_HtDefect_ClearParam(ii, DIV_HANTEI_NON);
		// <<===

		// パラメータ切替要求
		LOG(em_INF), "[%s] パラメータ切替要求(強制) [%s] FrameNo:%d (実行=%d Scode:%d)", my_sThreadName, DivNameManager::GetTorB(ii), nNowFrame, cmd.framenum, gcls_pCoil->GetCoilNextKen()->setting.scode[ii]);
		int scode	= MAX_SCODE;
		if(0 != gcls_pCoil->GetCoilNextKen()->setting.scode[ii]) scode = gcls_pCoil->GetCoilNextKen()->setting.scode[ii];
		Send_HtDefect_Param(cmd.framenum, ii, scode, DIV_HANTEI_NON);

		LOG(em_INF), "[%s] 内部距離カウンタ設定(強制) [%s] FrameNo:%d (実行=%d F:%.0fmm T:%.0fmm)", my_sThreadName, DivNameManager::GetTorB(ii), nNowFrame, cmd.framenum, cmd.front_pos, cmd.tail_pos);
		Send_HtDefect_SetPosition(ii, DIV_HANTEI_NON, &cmd);				// 内部距離カウンタ設定要求

#ifdef ENABLE_RPI_SAVE
		// 全長切り替え
		LOG(em_MSG), "[%s] FACT_TI_RPISAVE_04(強制) 送信 [men=%d][%s][%s][fno=%d][fpos=%d]"
			, my_sThreadName, ii, gcls_pCoil->GetCoilNextKen()->cKizukenNo, gcls_pCoil->GetCoilNextKen()->data.cCoilNoIn, cmd.framenum, cmd.front_pos);
		mcls_pIfRpiSave->Send_TiRpiSave_CoilSend(gcls_pCoil->GetCoilNextKen()->cKizukenNo, &gcls_pCoil->GetCoilNextKen()->setting, &gcls_pCoil->GetCoilNextKen()->data, cmd.framenum, (int)cmd.front_pos, ii);
#endif

		// 実行フレームNoを保持
		// PLGのコイル切替フレームNo認識に使用する
		gcls_Plg.SetCoilChange_Frame(ii, cmd.framenum);

	}
}

//==========================================
// 次コイル情報受信時のパラメータ設定要求
//==========================================
void MainInstance::SetParam_NextCoilInfRecv()
{
	// コイル情報が先に来ていたらスルーする
	if (0 == m_strCmd[0].cmd && 0 == m_strCmd[1].cmd) {
		LOG(em_INF), "[%s] 次コイル情報受信時のパラメータ切替要求 スルー", my_sThreadName);
		return;
	}

	DWORD nNowFrame		= gcls_Plg.GetFrameNo();						// 現在のフレームNo
	//double dDistance	= gcls_Plg.GetDistance();						// 1フレーム長さ[mm]
	double dFrameLen	= gcls_Plg.GetResV() * SIZE_FRAME_IMG_Y;		// 1フレーム長さ[mm]
	double dDistLen		= gcls_pParam->GetParamCommon().dDistLen;		// 表裏間距離[mm]

	bool  isDistPos;					// 基準面 true
	NCL_CMD_SET_POSITION	cmd;		// 内部距離カウンタ構造体

	for(int ii=0; ii<NUM_MEN; ii++) {
		memcpy(&cmd, &m_strCmd[ii], sizeof(NCL_CMD_SET_POSITION));
		// 基準面？
		if(ii == gcls_pParam->GetParamCommon().nDistPosition)	isDistPos = true;
		else													isDistPos = false;

		double dLen	= gcls_pParam->GetCommonMast().Dist.nKenBase;			// 検査基点～上流カメラ[mm]
		if(!isDistPos) dLen += gcls_pParam->GetParamCommon().dDistLen;		// 検査基点～下流カメラ[mm]

		//double front_pos	= 0.0;
		//int    framenum		= 0;
		// 他方面のコイル先頭フレームは、0mとならない。表裏間距離がフレーム単位で割り切れない分の誤差をここで吸収。(表裏間距離を厳密に考慮する)
		// 実行フレーム番号も他方面は前回コイル長を超過した時に切替のため、+1フレームとなる
		//if(!isDistPos) front_pos = (int)((gcls_pParam->GetCommonMast().Dist.nKenBase / dDistance) + 1) * dDistance - gcls_pParam->GetParamCommon().dDistLen;
		//if(!isDistPos) front_pos = ((int)(dDistLen/dFrameLen)+1) * dFrameLen - dDistLen;
		//if(!isDistPos) framenum = 1;

		//cmd.framenum = (int)ceil(dLen / dFrameLen) + nNowFrame + framenum;	// 実行フレーム番号
		//cmd.front_pos = (float)front_pos;									// 加算カウンタ[mm]
		//cmd.tail_pos = (float)gcls_pCoil->GetCoilNextKen()->data.nLenIn * 1000;	// 減算カウンタ(予定コイル長)[mm]

		// 未実行のPARAM,SET_POSITIONを消去
		LOG(em_INF), "[%s] 前回送信の未実行設定のキャンセル要求" , my_sThreadName);
		Send_HtDefect_ClearParam(ii, DIV_HANTEI_NON);


		// パラメータ切替要求
		LOG(em_INF), "[%s] 次コイル情報受信時のパラメータ切替要求 [%s] FrameNo:%d(%0.3fm) (実行=%d Scode:%d)", my_sThreadName, DivNameManager::GetTorB(ii), nNowFrame, dLen/1000.0, cmd.framenum, gcls_pCoil->GetCoilNextKen()->setting.scode[ii]);
		int scode	= MAX_SCODE;
		if(0 != gcls_pCoil->GetCoilNextKen()->setting.scode[ii]) scode = gcls_pCoil->GetCoilNextKen()->setting.scode[ii];
		Send_HtDefect_Param(cmd.framenum, ii, scode, DIV_HANTEI_NON);

		LOG(em_INF), "[%s] 内部距離カウンタ設定 [%s] FrameNo:%d(%0.3fm) (実行=%d F:%.0fmm T:%.0fmm)", my_sThreadName, DivNameManager::GetTorB(ii), nNowFrame, dLen/1000.0, cmd.framenum, cmd.front_pos, cmd.tail_pos);
		Send_HtDefect_SetPosition(ii, DIV_HANTEI_NON, &cmd);				// 内部距離カウンタ設定要求

#ifdef ENABLE_RPI_SAVE
		// 全長切り替え
		LOG(em_MSG), "[%s] FACT_TI_RPISAVE_04 送信 [men=%d][%s][%s][fno=%d][fpos=%d]"
			, my_sThreadName, ii, gcls_pCoil->GetCoilNextKen()->cKizukenNo, gcls_pCoil->GetCoilNextKen()->data.cCoilNoIn, cmd.framenum, cmd.front_pos);
		mcls_pIfRpiSave->Send_TiRpiSave_CoilSend(gcls_pCoil->GetCoilNextKen()->cKizukenNo, &gcls_pCoil->GetCoilNextKen()->setting, &gcls_pCoil->GetCoilNextKen()->data, cmd.framenum, (int)cmd.front_pos, ii);
#endif

	}
	// 前回WPD信号受信時の内部距離カウンタ構造体 初期化
	memset(m_strCmd, 0x00, sizeof(m_strCmd));
}

//------------------------------------------
// DB登録状況問い合わせ
// COMMON_QUE* que 起動要因
//------------------------------------------
void MainInstance::Add_Synclock(COMMON_QUE* que)
{
	int okng = 0;
	const COIL_INF *pCoil = gcls_pCoil->GetCoilKizukenNo(que->mix.cdata[1]);

	//// そもそも大量にある場合は、無条件に送り返す。(今あるデータだけで処理してね)
	if( 200 < gcls_DB.gque_AddSQLData.GetCount() ) {
		LOG(em_ERR), "[%s] 登録状況問い合わせ。すでに大量データ有りの為、問い合わせ取りやめ", my_sThreadName);
		okng = 1;
	}

	//================================================
	//// 事前準備
	DBManager::DeliveryDB *deliDB = new DBManager::DeliveryDB;
	deliDB->kubun = DBManager::em_DB_ADD_SYNC;
	deliDB->data = (BYTE*)new DBDATA_ADD_SYNC;
	DBDATA_ADD_SYNC* pWk   = (DBDATA_ADD_SYNC*)deliDB->data;		// エイリアス
	
	//================================================
	//// データセット
	memcpy( pWk->sendItem.cTask, TO_PUTDAT, SIZE_NAME_TASK);
	memcpy( pWk->sendItem.cPc, ".", SIZE_NAME_PC);
	pWk->sendItem.Que.nEventNo = FACT_TO_PUTDAT_04;
	pWk->sendItem.Que.nLineNo  = getlineid();
	// 結果 (0:正常 1:異常)
	pWk->sendItem.Que.fl.data[0] = okng;

	//================================================
	//// 登録依頼
	if( ! gcls_DB.gque_AddSQLData.AddToTailFreeCheck(deliDB) ) {	// 登録
		LOG(em_ERR), "[%s] DB登録依頼キューフル [DBDATA_ADD_SYNC]", my_sThreadName);
		syslog(SYSNO_QUEFULL_WAR, "[DBDATA_ADD_SYNC]");
		delete deliDB->data;
		delete deliDB;
	}
}

// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 外部タスク通知
//// HT_DEFDECT
//==========================================
// 判定PC へ パラメータ変更を通知
//------------------------------------------
// int				frame	: フレームNo
// int				id		: 判定PC区分 (0:表 1:裏 2:両面)
// int				scode	: 検査表面状態No
// EM_DIV_HANTEI	hantei	: 対象の判定機能 (DIV_HANTEI_NONだったら全機能)
//==========================================
void MainInstance::Send_HtDefect_Param(int frame, int id, int scode, EM_DIV_HANTEI hantei)
{
	COMMON_QUE que;
	que.nEventNo = FACT_HT_DEFECT_01;
	que.nLineNo = getlineid();
	que.fl.data[0] = frame;
	//que.fl.data[0] = 0;			// 0:即時
	que.fl.data[1] = id;
	que.fl.data[2] = scode;
	que.fl.data[3] = hantei;

	// 送信
	int iRet = send_mail(HT_DEFECT, "*", &que);
	if (0 != iRet) syslog(SYSNO_MAIL_SNED_ERR, "[パラメータ変更要求 err_code=%d]", iRet);
}

//==========================================
// 判定PC へ 検査開始を通知
//==========================================
void MainInstance::Send_HtDefect_Start()
{
	EM_DIV_UNTEN  emUnten = gcls_Status.GetUnten();

	COMMON_QUE que;
	memset(&que, 0x00, sizeof(que));
	que.nEventNo = FACT_HT_DEFECT_02;
	que.nLineNo = getlineid();
	que.fl.data[0] = emUnten;
	que.fl.data[1] = gcls_pCoil->GetInitScode(0);
	que.fl.data[2] = gcls_pCoil->GetInitScode(1);

	int nDistLen = gcls_Plg.GetDistLen();
	int nDistPos = gcls_Plg.GetDistPos();
	int nDistNotPos = gcls_Plg.GetDistNotPos();
	que.fl.data[3 + nDistPos] = nDistLen;			// 基準面が先に進んでいる
	que.fl.data[3 + nDistNotPos] = 0;


	//// とりあえず バックアップの0をセット
	for (int ii = 0; ii<NUM_ALL_HANTEI; ii++) {
		que.fl.data[5 + 2 * ii + 0] = 0;
		que.fl.data[5 + 2 * ii + 1] = DIV_HANTEI_BK;
	}

	//// PCID順に カメラペアをセット
	LogicParaRunFunc::ClearSelectCamset();
	if (DIV_UNTEN_CAMERA != emUnten) {

		for (int ii = 0; ii<NUM_MEN; ii++) {
			// 単発
			for (int jj = 0; jj<NUM_HANTEI; jj++) {
				int pcid = gcls_Status.GetHtID(ii, jj);

				if (pcid > 0 && pcid <= NUM_ALL_HANTEI) {

					que.fl.data[5 + 2 * (pcid - 1) + 0] = ii*NUM_HANTEI + jj + 1;		// カメラペアＩＤ
					que.fl.data[5 + 2 * (pcid - 1) + 1] = DIV_HANTEI_HT;				// 単発機能
					LogicParaRunFunc::SetSelectCamset(pcid - 1, ii * NUM_HANTEI + jj + 1);
				}
			}
#ifdef ENABLE_CYCLE
			// 周期
			for (int jj = 0; jj<NUM_HANTEI_CYCLE; jj++) {
				int pcid = gcls_Status.GetCyID(ii, jj);

				if (pcid > 0 && pcid <= NUM_ALL_HANTEI) {
					que.fl.data[5 + 2 * (pcid - 1) + 0] = ii*NUM_HANTEI_CYCLE + jj + 1;	// カメラペアＩＤ
					que.fl.data[5 + 2 * (pcid - 1) + 1] = DIV_HANTEI_CY;				// 周期機能
				}
			}
#endif

		}
	}
	else
	{
		int nGroupNo = -1;
		for (int ii = 0; ii < NUM_MEN; ii++)
		{
			// 単発
			for (int jj = 0; jj < NUM_HANTEI; jj++)
			{
				int pcid = gcls_Status.GetHtID(ii, jj);
				if (pcid > 0 && pcid <= NUM_ALL_HANTEI)
				{
					int nWk = ToLinkswUtil::GetHanteiGroup(ii, jj, 1);
					// 各リンクスイッチの先頭判定PCのみセット
					if (nWk != -1 && nGroupNo != nWk)
					{
						que.fl.data[5 + 2 * (pcid - 1) + 0] = ii * NUM_HANTEI + jj + 1;
						nGroupNo = nWk;
					}
					que.fl.data[5 + 2 * (pcid - 1) + 1] = DIV_HANTEI_HT;
					LogicParaRunFunc::SetSelectCamset(pcid - 1, ii * NUM_HANTEI + jj + 1);
				}
			}
		}
	}
	// モードに関わらず初期化
	for (int ii = 0; ii < NUM_ALL_HTPARA; ii++)
	{
		int pcid = gcls_Status.GetHtParaID(ii);

		if (pcid > 0 && pcid <= NUM_ALL_HANTEI)
		{
			que.fl.data[5 + 2 * (pcid - 1) + 0] = 0;								// カメラペアＩＤ（クリア）
			que.fl.data[5 + 2 * (pcid - 1) + 1] = DIV_HANTEI_PR;					// パララン
		}
	}


	//// 送信
	int iRet = send_mail(HT_DEFECT, "*", &que);
	if (0 != iRet) syslog(SYSNO_MAIL_SNED_ERR, "[検査開始要求 err_code=%d]", iRet);
}

//==========================================
// 全判定PC へ 検査停止を通知
//==========================================
void MainInstance::Send_HtDefect_Stop()
{
	COMMON_QUE que;
	que.nEventNo = FACT_HT_DEFECT_03;
	que.nLineNo = getlineid();

	// 全部対象
	// 判定が止まるときは、周期、パラランもとめる
	for (int ii = 0; ii<NUM_ALL_HANTEI; ii++) {
		que.fl.data[ii] = 0;
	}

	send_mail(HT_DEFECT, "*", &que);
}

//==========================================
// 該当面の判定PC、周期PC へ 検査停止を通知
//------------------------------------------
// int	men	: 表裏
//==========================================
void MainInstance::Send_HtDefect_Stop(int men)
{
	COMMON_QUE que;
	que.nEventNo = FACT_HT_DEFECT_03;
	que.nLineNo = getlineid();

	//// まず単発疵機能,周期機能部分は対象外としておく
	for (int ii = 0; ii<NUM_ALL_HANTEI; ii++) {
		que.fl.data[ii] = -1;
	}
	//// 判定機能
	for (int jj = 0; jj<NUM_HANTEI; jj++) {
		int pcid = gcls_Status.GetHtID(men, jj);

		if (pcid > 0 && pcid <= NUM_ALL_HANTEI) {
			que.fl.data[pcid - 1] = 0;		// 停止要求されている面
		}
	}
	//// 送信
	send_mail(HT_DEFECT, "*", &que);
}

//==========================================
// 判定PC へ JPEG画像圧縮率設定送信
//------------------------------------------
// int	nJpegQuality	: 0:BMP画像形式, 1～100:JPEG圧縮画像形式
//==========================================
void MainInstance::Send_HtDefect_JpegQuality(int nJpegQuality)
{
	COMMON_QUE que;
	que.nEventNo = FACT_HT_DEFECT_04;
	que.nLineNo = getlineid();
	que.fl.data[0] = nJpegQuality;
	send_mail(HT_DEFECT, "*", &que);
}


//==========================================
// 判定PC へ 上位情報送信
//------------------------------------------
// TYPE_EPC_COIL* pData コイル情報
//==========================================
void MainInstance::Send_HtDefect_Pcinfo(TYPE_EPC_COIL* pData)
{
	COMMON_QUE que;
	que.nEventNo = FACT_HT_DEFECT_05;
	que.nLineNo = getlineid();

	int nCnt = GET_JUST_INDEX(sizeof(TYPE_EPC_COIL), DIVCOIL_ONE_SIZE_SEND) + 1;
	for (int ii = 0; ii < nCnt; ii++) {
		int nSize = sizeof(TYPE_EPC_COIL) - (DIVCOIL_ONE_SIZE_SEND*ii);
		if (nSize>DIVCOIL_ONE_SIZE_SEND) nSize = DIVCOIL_ONE_SIZE_SEND;

		//que.fl.data[0] = 1;				// 遅延実行 有り
		que.fl.data[0] = 0;				// 0固定
		que.fl.data[1] = nSize;
		que.fl.data[2] = ii + 1;
		que.fl.data[3] = nCnt;
		for (int jj = 0; jj<NUM_MEN; jj++) que.fl.data[4 + jj] = MAX_SCODE;
		memcpy(&que.fl.data[4 + NUM_MEN], &((BYTE*)pData)[ii*DIVCOIL_ONE_SIZE_SEND], nSize);
		send_mail(HT_DEFECT, "*", &que);
	}
}

//==========================================
// 判定PC へ 続行不可能解除	
//==========================================
void MainInstance::Send_HtDefect_PcCanOk()
{
	COMMON_QUE que;
	que.nEventNo = FACT_HT_DEFECT_07;
	que.nLineNo = getlineid();
	send_mail(HT_DEFECT, "*", &que);
}

//==========================================
// 判定PC へ 状態問合せ
//==========================================
void MainInstance::Send_HtDefect_Status()
{
	COMMON_QUE que;
	que.nEventNo = FACT_HT_DEFECT_08;
	que.nLineNo = getlineid();

	// 送信
	int iRet = send_mail(HT_DEFECT, "*", &que);
	if (0 != iRet) syslog(SYSNO_MAIL_SNED_ERR, "[状態問い合わせ err_code=%d]", iRet);
}

//==========================================
// 判定PC へ 未実行設定のキャンセル要求
//------------------------------------------
// int				id			: 判定PC区分 (0:表 1:裏 2:両面)
// EM_DIV_HANTEI	hantei		: 対象の判定機能 (DIV_HANTEI_NONだったら全機能)
//==========================================
void MainInstance::Send_HtDefect_ClearParam(int id, EM_DIV_HANTEI hantei)
{
	COMMON_QUE que;
	memset(&que, 0x00, sizeof(que));
	que.nEventNo = FACT_HT_DEFECT_19;
	que.nLineNo = getlineid();
	que.fl.data[0] = id;
	que.fl.data[1] = hantei;
	send_mail(HT_DEFECT, "*", &que);
}

//==========================================
// 判定PC へ 内部距離カウンタ設定要求
//------------------------------------------
// int				id			: 判定PC区分 (0:表 1:裏 2:両面)
// EM_DIV_HANTEI	hantei		: 対象の判定機能 (DIV_HANTEI_NONだったら全機能)
// NCL_CMD_SET_POSITION* cmd	: 内部距離カウンタ設定情報
//==========================================
void MainInstance::Send_HtDefect_SetPosition(int id, EM_DIV_HANTEI hantei, NCL_CMD_SET_POSITION* cmd)
{
	COMMON_QUE que;
	memset(&que, 0x00, sizeof(que));
	que.nEventNo = FACT_HT_DEFECT_20;
	que.nLineNo = getlineid();
	que.fl.data[0] = id;
	que.fl.data[1] = hantei;
	memcpy(&que.fl.data[2], cmd, sizeof(NCL_CMD_SET_POSITION));
	send_mail(HT_DEFECT, "*", &que);
}

//==========================================
// 判定PC へ パラメータ事前読込
//------------------------------------------
// int				id			: 判定PC区分 (0:表 1:裏 2:両面)
// EM_DIV_HANTEI	hantei		: 対象の判定機能 (DIV_HANTEI_NONだったら全機能)
// int				scode		: 検査表面状態No
//==========================================
void MainInstance::Send_HtDefect_PreLoadParam(int id, EM_DIV_HANTEI hantei, int scode)
{
	COMMON_QUE que;
	memset(&que, 0x00, sizeof(que));
	que.nEventNo = FACT_HT_DEFECT_21;
	que.nLineNo = getlineid();
	que.fl.data[0] = id;
	que.fl.data[1] = hantei;
	que.fl.data[2] = scode;
	send_mail(HT_DEFECT, "*", &que);
}


//// HY_DEFECT
//==========================================
// 表示管理に パラメータ変更を通知
//------------------------------------------
// int			id		: テーブルID
//==========================================
void MainInstance::Send_HyDefect_Param(int id)
{
	COMMON_QUE que;
	que.nEventNo = FACT_HY_DEFECT_01;
	que.nLineNo = getlineid();
	que.fl.data[0] = id;
	// 送信
	send_mail(HY_DEFECT, "*", &que);
}

//------------------------------------------
// 表示管理に表示情報初期化通知
//------------------------------------------
void MainInstance::Send_HyDefect_MemDataInit()
{
	COMMON_QUE que;
	que.nEventNo = FACT_HY_DEFECT_02;
	que.nLineNo = getlineid();

	send_mail(HY_DEFECT, "*", &que);
}

//------------------------------------------
// 疵点マップ画面にパラメータ変更を通知
// int id テーブルID
//------------------------------------------
void MainInstance::Send_SoDefMap_Param(int id)
{
	COMMON_QUE que;
	memset(&que, 0x00, sizeof(que));
	que.nLineNo = getlineid();
	que.fl.data[0] = id;

	//// 疵点マップ画面
	que.nEventNo = FACT_SO_DEFMAP_01;
	send_mail(SO_DEFMAP, "*", &que);
}

//------------------------------------------
// 表示管理に 前工程疵情報読込みを通知 [←MaeManager, CoilBaseManager]
//------------------------------------------
// char const* cKizukenNo	管理No
//------------------------------------------
void MainInstance::Send_HyDefect_MaeRead(char const* cKizukenNo)
{
	COMMON_QUE que;
	que.nEventNo = FACT_HY_DEFECT_03;
	que.nLineNo = getlineid();
	memcpy(que.fc.data, cKizukenNo, SIZE_KIZUKEN_NO);

	// プロビジョン
	send_mail(HY_DEFECT, "*", &que);
}

////// TO_GETDAT
////==========================================
//// 入力データ編集管理に パラメータ変更を通知
////------------------------------------------
//// int			id		: テーブルID
////==========================================
//void MainInstance::Send_ToGetdat_Param(int id)
//{
//	COMMON_QUE que;
//	que.nEventNo = FACT_TO_GETDAT_01;
//	que.nLineNo = getlineid();
//	que.fl.data[0] = id;
//	// 送信
//	send_mail(TO_GETDAT, ".", &que);
//}

////// TO_PUTDAT
////==========================================
//// 実績編集管理に パラメータ変更を通知
////------------------------------------------
//// int			id		: テーブルID
////==========================================
//void MainInstance::Send_ToPutdat_Param(int id)
//{
//	COMMON_QUE que;
//	que.nEventNo = FACT_TO_PUTDAT_01;
//	que.nLineNo = getlineid();
//	que.fl.data[0] = id;
//	// 送信
//	send_mail(TO_PUTDAT, ".", &que);
//}

////------------------------------------------
//// 実績編集タスクへ 登録完了通知
//// int okng 0:正常 1:異常
////------------------------------------------
//void MainInstance::Send_ToPutDat_Synclock(int okng)
//{
//	COMMON_QUE que;
//	que.nEventNo = FACT_TO_PUTDAT_03;
//	que.nLineNo = getlineid();
//	que.fl.data[0] = okng;
//
//	// 送信
//	int iRet = send_mail(TO_PUTDAT, ".", &que);
//}
//
////// TO_MAEDAT
////==========================================
//// 前工程実績編集管理に パラメータ変更を通知
////------------------------------------------
//// int			id		: テーブルID
////==========================================
//void MainInstance::Send_ToMaedat_Param(int id)
//{
//	COMMON_QUE que;
//	que.nEventNo = FACT_TO_MAEDAT_01;
//	que.nLineNo = getlineid();
//	que.fl.data[0] = id;
//	// 送信
//	send_mail(TO_MAEDAT, ".", &que);
//}

//------------------------------------------
// 前工程実績編集に 検査用コイル情報受信完了 を通知 [←CoilManager]
// char* KizukenNo 疵検管理No
// int rec 受信レコード (0オリジン)
//------------------------------------------
void MainInstance::Send_ToMaedat_CoilRecv(char* KizukenNo, int rec)
{
	COMMON_QUE que;
	que.nEventNo = FACT_TO_MAEDAT_02;
	que.nLineNo = getlineid();
	que.mix.idata[0] = rec;
	memcpy(que.mix.cdata[0], KizukenNo, SIZE_KIZUKEN_NO);

	// 送信
	int iRet = send_mail(TO_MAEDAT, ".", &que);
}

//// SO_GAMENN
//==========================================
// オペレータ画面に現在の状態通知
//==========================================
void MainInstance::Send_SoGamenn_MailStatus()
{
	COMMON_QUE que;
	que.nEventNo = FACT_SO_GAMENN_01;
	que.nLineNo = getlineid();

	que.fl.data[0] = gcls_Status.GetUnten();
	que.fl.data[1] = gcls_Status.GetPlg();
	que.fl.data[2] = gcls_Status.GetMenMode() ? 1 : 0;
	que.fl.data[3] = gcls_Status.GetKadou();
	que.fl.data[4] = gcls_Status.Get_ParaKadou(gcls_Status.GetParaSys(0));
	que.fl.data[5] = gcls_Status.Get_ParaKadou(gcls_Status.GetParaSys(1));

	// 送信
	send_mail(SO_GAMENN, "*", &que);
}

//// KS_MASTER
//==========================================
// PCマスター管理に再起動要求通知
//==========================================
void MainInstance::Send_KsMaster_RebootPc(char* cPcName)
{
	COMMON_QUE que;
	que.nEventNo = FACT_KS_MASTER_07;
	que.nLineNo = getlineid();

	memcpy(que.fc.data, cPcName, SIZE_NAME_PC);

	// 送信
	send_mail(KS_MASTER, ".", &que);
}