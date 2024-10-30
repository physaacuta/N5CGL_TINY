#include "stdafx.h"
#include "DmemManager.h"


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
DmemManager::DmemManager() :
m_nState(-1),
m_bInitReadOk(false),
m_nHBOldVal(-1),					// 違う値にする為、ありえない数値を初期値としておく
ThreadManager("DmenMgr")
{
//*/----> ◆
#ifdef LOCAL
	m_hDataMem = CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, sizeof(TO_SEQMEM_TBL), TBL_TO_SEQMEM_NAME);
	mtbl_pDM =(TO_SEQMEM_TBL*) MapViewOfFile(m_hDataMem, FILE_MAP_ALL_ACCESS, 0, 0, 0);
#endif
//*///<<---

	//// 共有メモリ
	// シーケンサ受渡情報テーブル
	mem_connect(&m_hDataMem, (LPVOID *)&mtbl_pDM, TBL_TO_SEQMEM_NAME);
	if(m_hDataMem == NULL) {
		syslog(SYSNO_MEM_OPEN, "データメモリ情報テーブル アクセス失敗");
		_ASSERT(FALSE);
	}

	// 使いやすいようにエリアセット
	mtbl_pRecv = (SEQ_DM_RECV_BASE*)&mtbl_pDM->fb_dm[DM_RECV_REC];
	mtbl_pSend = (SEQ_DM_SEND_DEFECT*)&mtbl_pDM->fb_dm[DM_SEND_REC];
	memset(&m_typOldDmRecv, 0x00, sizeof(m_typOldDmRecv));
	memset(&m_typOldDmRecvLow, 0x00, sizeof(m_typOldDmRecvLow));


	// シグナル
	m_evTimeHlsRead		= CreateWaitableTimer(NULL, FALSE, NULL);
	m_evTimeHlsWrit		= CreateWaitableTimer(NULL, FALSE, NULL);
	m_evTimeStatusRead	= CreateWaitableTimer(NULL, FALSE, NULL);
	m_evTimeStatusWrit	= CreateWaitableTimer(NULL, FALSE, NULL);
	m_evTimeAlarmePulse = CreateWaitableTimer(NULL, FALSE, NULL);
	m_evTimeCntResetPulse = CreateWaitableTimer(NULL, FALSE, NULL);
	m_evTimeCntStartPulse = CreateWaitableTimer(NULL, FALSE, NULL);
	m_evTimeTestWpdPulse = CreateWaitableTimer(NULL, FALSE, NULL);
	InitializeCriticalSection(&m_csLock);								// クリティカルセクションオブジェクトを初期化
	
	m_evPI_WPD_KenBaseGateOn = CreateEvent(NULL, FALSE, FALSE, NULL);
	m_evPI_WPD_KenBaseGateOff = CreateEvent(NULL, FALSE, FALSE, NULL);
	m_evPI_WPD_DspBaseGateOn = CreateEvent(NULL, FALSE, FALSE, NULL);
	m_evPI_WPD_DspBaseGateOff = CreateEvent(NULL, FALSE, FALSE, NULL);
	m_evPI_WPD_KenBase = CreateEvent(NULL, FALSE, FALSE, NULL);
	m_evPI_WPD_DspBase = CreateEvent(NULL, FALSE, FALSE, NULL);
	m_evPI_ShrcutF = CreateEvent(NULL, FALSE, FALSE, NULL);
	m_evPI_ShrcutL = CreateEvent(NULL, FALSE, FALSE, NULL);
	m_evPI_PatoReset = CreateEvent(NULL, FALSE, FALSE, NULL);


	// その他
	memset( m_nOutPls, 0x00, sizeof(m_nOutPls));
	m_nVoiceMen = -1;
	m_nVoice = -1;
	memset(m_nRamp, -1, sizeof(m_nRamp));
	memset(m_nPato, 0x00, sizeof(m_nPato));
	memset(m_nAlarm, -1, sizeof(m_nAlarm));
	// ===>> 坂巻 2021.06.03 小コイル長材通販時、コイル長比較が正常にできない不具合修正
	m_nWpdOnLength = 0;				// WPDがON時の検査コイル長
	m_nWpdOnLengthDsp = 0;			// WPDがON時の表示コイル長
	m_nWpdOnLengthSub = 0;			// WPDがON時の比較コイル長
	// <<===

}

//------------------------------------------
// デストラクタ
//------------------------------------------
DmemManager::~DmemManager()
{
	//// 共有メモリ開放
	mtbl_pRecv = NULL;
	mtbl_pSend = NULL;
	mem_close(&m_hDataMem, (LPVOID *)&mtbl_pDM);				// データメモリ情報

	// シグナル
	CancelWaitableTimer(m_evTimeHlsRead);
	CancelWaitableTimer(m_evTimeHlsWrit);
	CancelWaitableTimer(m_evTimeStatusRead);
	CancelWaitableTimer(m_evTimeStatusWrit);
	CancelWaitableTimer(m_evTimeAlarmePulse);
	CancelWaitableTimer(m_evTimeCntResetPulse);
	CancelWaitableTimer(m_evTimeCntStartPulse);
	CancelWaitableTimer(m_evTimeTestWpdPulse);
	CloseHandle(m_evTimeHlsRead);
	CloseHandle(m_evTimeHlsWrit);
	CloseHandle(m_evTimeStatusRead);
	CloseHandle(m_evTimeStatusWrit);
	CloseHandle(m_evTimeAlarmePulse);
	CloseHandle(m_evTimeCntResetPulse);
	CloseHandle(m_evTimeCntStartPulse);
	CloseHandle(m_evTimeTestWpdPulse);
	DeleteCriticalSection(&m_csLock);									// クリティカルセクションオブジェクトを開放

	CloseHandle(m_evPI_WPD_KenBaseGateOn);
	CloseHandle(m_evPI_WPD_KenBaseGateOff);
	CloseHandle(m_evPI_WPD_DspBaseGateOn);
	CloseHandle(m_evPI_WPD_DspBaseGateOff);
	CloseHandle(m_evPI_WPD_KenBase);
	CloseHandle(m_evPI_WPD_DspBase);
	CloseHandle(m_evPI_ShrcutF);
	CloseHandle(m_evPI_ShrcutL);
	CloseHandle(m_evPI_PatoReset);
}

//------------------------------------------
// 状態変化チェック
// int now	今回値
// int old	前回値
// 戻り値	変更状態 (9:変更無し(ON) -9:変更無し(OFF) 1:OFF→ON -1:ON→OFF)
//------------------------------------------
int DmemManager::ChangeState(int now, int old )
{
	if( 1 == now ) {		// 今回 正常
		if( 0 == old )	return 1;	// OFF→ON
		else			return 9;	// ONのまま
	} else {				// 今回 異常
		if( 1 == old )	return -1;	// ON→OFF
		else			return -9;	// OFFのまま
	}
}


// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// スレッド操作

//------------------------------------------
// スレッド開始前処理 (スレッド実行直後(シグナルセット前)にコールバック)
//------------------------------------------
int DmemManager::ThreadFirst()
{
	//// ステータス用メンバ変数を初期化
	m_nPlsRate[0] = (int)(mcls_pParam->GetParamCommon().dPls * PLS_RATE_TANI);
	m_nPlsRate[1] = (int)(mcls_pParam->GetParamCommon().dPlsSub * PLS_RATE_TANI);
	m_nPlsRate[2] = (int)(mcls_pParam->GetParamCommon().dPlsDsp * PLS_RATE_TANI);
	// パルスレート、長手分解能のDB更新(長手分解能との不一致防止対策)
	UpdatePlsRate(m_nPlsRate);

	//// 定周期シグナル設定
	SetEvWaitableTimer(m_evTimeHlsRead,		100, PARAM_TIME_HLS_READ);
	SetEvWaitableTimer(m_evTimeHlsWrit,		100, PARAM_TIME_HLS_WRIT);
	SetEvWaitableTimer(m_evTimeStatusRead,	100, PARAM_TIME_STATUS_READ);
	SetEvWaitableTimer(m_evTimeStatusWrit,	100, PARAM_TIME_STATUS_WRIT);


	//// イベント設定
	AddEventNum(1, &mque_AddData.g_evSem);
	AddEventNum(1, &m_evTimeHlsRead);
	AddEventNum(1, &m_evTimeHlsWrit);
	AddEventNum(1, &m_evTimeStatusRead);
	AddEventNum(1, &m_evTimeStatusWrit);
	AddEventNum(1, &m_evTimeAlarmePulse);
	AddEventNum(1, &m_evTimeCntResetPulse);
	AddEventNum(1, &m_evTimeCntStartPulse);
	AddEventNum(1, &m_evTimeTestWpdPulse);

	//// 初期値セット
	m_nStatusReadLowInterval = GetTickCount();
	return 0;
}

//------------------------------------------
// スレッドイベント発生
// int nEventNo イベントNo (0オリジン)
//------------------------------------------
void DmemManager::ThreadEvent(int nEventNo)
{
	//AutoLock autolock(&m_csLock);			// キュー渡しがメンドクサイところがあるので。


	////// シグナルの条件
	enum {	EV_QUE = 0,							// 外部操作キュー
			EV_HLS_READ,						// ヘルシー読込
			EV_HLS_WRIT,						// ヘルシー書込
			EV_STATUS_READ,						// ステータス信号読み込み
			EV_STATUS_WRIT,						// ステータス信号書き込み
			EV_ALARME_PULSE,					// 疵接近ボイス出力パルス停止
			EV_CNT_RESET_PULSE,					// 高機能カウンタリセットパルス停止
			EV_CNT_START_PULSE,					// 高機能カウンタ開始パルス停止
			EV_TEST_WPD_PULSE					// テスト用 WPD信号ON要求パルス停止
	};

	////// シグナル条件分け
	switch (nEventNo) {

//-----------------------------------------------------------------------------------------------
	case EV_QUE: 									// 外部操作キュー
	{
		DELIVERY* pDeli = mque_AddData.GetFromHead();		// 登録依頼データ
		switch (pDeli->emNo) {
			//-----------------------
		case em_RAMP:										// 疵接近ランプ
			if (true) {
				OCCUR_RAMP typWk;
				memcpy(&typWk, pDeli->data, sizeof(OCCUR_RAMP));

				WritDmemRamp(typWk);
			}
			break;
			//-----------------------
		case em_ALARM:										// 疵接近アラーム
			if (true) {
				OCCUR_ALARM typWk;
				memcpy(&typWk, pDeli->data, sizeof(OCCUR_ALARM));

				WritDmemAlarm(typWk);
			}
			break;
			//-----------------------
		case em_PATO:										// パトライト
			if (true) {
				OCCUR_PATO typWk;
				memcpy(&typWk, pDeli->data, sizeof(OCCUR_PATO));

				WritDmemPato(typWk);
			}
			break;
			//-----------------------
		case em_VOICE:										// ボイス
			if (m_nVoiceMen != -1 && m_nVoice != -1)
			{
				LOGA(em_MSG), "[%s] ボイス出力中につき要求破棄", my_sThreadName);
			}
			else
			{
				OCCUR_VOICE typWk;
				memcpy(&typWk, pDeli->data, sizeof(OCCUR_VOICE));

				WritDmemVoice(typWk);
			}
			break;
		}
		// 開放
		SAFE_DELETE(pDeli->data);
		SAFE_DELETE(pDeli);

		break;
	}
//-----------------------------------------------------------------------------------------------
	case EV_HLS_READ:										// ヘルシーチェック
		ReadDmemHls();
		break;
	
//-----------------------------------------------------------------------------------------------
	case EV_HLS_WRIT:										// ヘルシー書き込み
		WritDmemHls();
		//Out1sPls();										// 1秒パルス出力
		break;
	

//-----------------------------------------------------------------------------------------------
	case EV_STATUS_READ:									// ステータス状態読み込み
		if (true) {
			//================================================
			// 事前準備
			//// 一括で共有メモリに書く為、一度バッファ内で編集
			SEQ_DM_RECV_BASE tbl;
			memcpy(&tbl, mtbl_pRecv, sizeof(tbl));

			// 高速用
			ReadDmemStatus(tbl);

			// 低速用
			if( GetTickCount() > m_nStatusReadLowInterval+ PARAM_TIME_STATUS_READ_LOW) {
				ReadDmemStatusLow(tbl);
				m_nStatusReadLowInterval = GetTickCount();
				// 前回値コピー
				memcpy(&m_typOldDmRecvLow, &tbl, sizeof(SEQ_DM_RECV_BASE));		// 前回値保存

			}
		
			// 前回値コピー
			memcpy(&m_typOldDmRecv, &tbl, sizeof(SEQ_DM_RECV_BASE));		// 前回値保存
		}
		break;
	

//-----------------------------------------------------------------------------------------------
	case EV_STATUS_WRIT:									// ステータス状態書き込み
		WritDmemStatus();
		break;
	
//-----------------------------------------------------------------------------------------------
	case EV_ALARME_PULSE:									// 疵接近ボイス出力パルス停止
		WritDmemVoiceReset();
		break;

//-----------------------------------------------------------------------------------------------
	case EV_CNT_RESET_PULSE:								// 高機能カウンタリセットパルス停止
		mtbl_pSend->nCntReset = 0;

		//// PLGカウンタリセット要求中フラグをOFF
		//m_bPlgReset = false;
		break;

//-----------------------------------------------------------------------------------------------
	case EV_CNT_START_PULSE:								// 高機能カウンタ開始パルス停止
		mtbl_pSend->nCntStart = 0;

		//// PLGカウンタリセット要求中フラグをOFF
		//m_bPlgReset = false;
		break;

//-----------------------------------------------------------------------------------------------
	case EV_TEST_WPD_PULSE:									// テスト用 WPD信号ON要求パルス停止
		mtbl_pSend->nTestWpdReq = 0;
		break;
	

//-----------------------------------------------------------------------------------------------
	default:
		// ありえない！！
		LOG(em_ERR), "[%s] ThreadEvent EventNo NG=%d", my_sThreadName, nEventNo );
		break;
	}
}

// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 処理
//	 ※ この中では、ブロックされないようにすること。
//   ※ 特に、機器状態変更はDBでブロックされる可能性があるのでキュータイプにすること


//------------------------------------------
// ヘルシー読み込み
//------------------------------------------
void DmemManager::ReadDmemHls()
{
	//// 前回と同一値なら異常
	if( m_nHBOldVal == mtbl_pRecv->nHBCnt ) {						// 異常認識
		// 正常→異常
		if(1 != m_nState) {
			LOG(em_ERR), "[%s] ヘルシー異常", my_sThreadName);
			syslog(601, "");
			mcls_pStatus->SetStatusQue("DATAMEM_SEQ_HB_ID", false);
			m_nState = 1;
		}
		
	} else {														// 正常認識
		// 異常→正常
		if(0 != m_nState) {
			LOG(em_MSG), "[%s] ヘルシー正常復帰", my_sThreadName);
			syslog(600, "");
			mcls_pStatus->SetStatusQue("DATAMEM_SEQ_HB_ID", true);
			m_nState = 0;
		}

		//// タスク起動後初めての正常認識
		if (!m_bInitReadOk) {
			m_bInitReadOk = true;
		}
	}

	//// 前回値取得
	m_nHBOldVal = mtbl_pRecv->nHBCnt;
}

//------------------------------------------
// ヘルシー書き込み
//------------------------------------------
void DmemManager::WritDmemHls()
{
	if( HB_MAX_VAL <= mtbl_pSend->typKensaStat.nHBCnt )	mtbl_pSend->typKensaStat.nHBCnt  = 1;
	else											mtbl_pSend->typKensaStat.nHBCnt += 1;

	//// PC時間
 //   SYSTEMTIME time;
 //   GetLocalTime(&time);
	//mtbl_pSend->nPcTime[0] = time.wYear;
	//mtbl_pSend->nPcTime[1] = time.wMonth;
	//mtbl_pSend->nPcTime[2] = time.wDay;
	//mtbl_pSend->nPcTime[3] = time.wHour;
	//mtbl_pSend->nPcTime[4] = time.wMinute;
	//mtbl_pSend->nPcTime[5] = time.wSecond;
}

//------------------------------------------
// ステータス状態書き込み
//------------------------------------------
void DmemManager::WritDmemStatus()
{
	int nWk;

	//================================================
	// 事前準備
	//// 一括で共有メモリに書く為、一度バッファ内で編集
	SEQ_DM_SEND_DEFECT tbl;
	memcpy( &tbl, mtbl_pSend, sizeof(tbl));


	//================================================
	// 疵検ステータス 登録

	//// 稼動状態

	nWk = mcls_pStatus->GetKadou();
		 if(DIV_KADOU_INIT == nWk )		tbl.typKensaStat.nKadou = 1;
	else if(DIV_KADOU_STOP == nWk )		tbl.typKensaStat.nKadou = 2;
	else if(DIV_KADOU_START == nWk )	tbl.typKensaStat.nKadou = 3;
	else if(DIV_KADOU_RETRY == nWk )	tbl.typKensaStat.nKadou = 4;
	else								tbl.typKensaStat.nKadou = 0;

	for(int ii=0; ii<NUM_MEN; ii++) {
		//// 機器状態
		nWk = mcls_pStatus->GetKiki(ii);
			 if(DIV_KIKI_OK == nWk )		tbl.typKensaStat.nKiki[ii] = 0;
		else if(DIV_KIKI_KEI == nWk )		tbl.typKensaStat.nKiki[ii] = 1;
		else if(DIV_KIKI_JYU == nWk )		tbl.typKensaStat.nKiki[ii] = 3;
		else								tbl.typKensaStat.nKiki[ii] = 3;

		//// 検査状態
		nWk = mcls_pStatus->GetKensa(ii);
			 if(DIV_KENSA_NON == nWk )		tbl.typKensaStat.nKensa[ii] = 9;
		else if(DIV_KENSA_OK == nWk )		tbl.typKensaStat.nKensa[ii] = 0;
		else if(DIV_KENSA_NG == nWk )		tbl.typKensaStat.nKensa[ii] = 1;
		//else if(DIV_KENSA_MENTE == nWk )	tbl.typKensaStat.nKensa[ii] = 1;
		else if(DIV_KENSA_EDGENG == nWk )	tbl.typKensaStat.nKensa[ii] = 4;
		else if(DIV_KENSA_DBNON == nWk )	tbl.typKensaStat.nKensa[ii] = 5;
		else if(DIV_KENSA_SKIP == nWk )		tbl.typKensaStat.nKensa[ii] = 6;
		else if(DIV_KENSA_STOP == nWk )		tbl.typKensaStat.nKensa[ii] = 9;
		else								tbl.typKensaStat.nKensa[ii] = 1;
	}

	// PLG状態
	tbl.typKensaStat.nPlg = (int)mcls_pStatus->GetPlg() - 1;
	
	// 通常運転モードのみ
	if(DIV_UNTEN_NOMAL == mcls_pStatus->GetUnten() ||
	   DIV_UNTEN_STOP  == mcls_pStatus->GetUnten() ) {
		memcpy( &mtbl_pSend->typKensaStat, &tbl.typKensaStat, sizeof(tbl.typKensaStat));
	}
}

//------------------------------------------
// ステータス状態読み込み
//------------------------------------------
void DmemManager::ReadDmemStatus(SEQ_DM_RECV_BASE& tbl)
{
//#ifdef LOCAL
//	return;
//#endif
	
	//================================================
	// パルス関係処理   

	// 検査WPDゲート信号
	if (1 == ChangeState(tbl.nGateKen, m_typOldDmRecv.nGateKen)) {
		LOG(em_MSG), "[%s] 検査WPDゲート信号 ON", my_sThreadName);
		SetEvent(m_evPI_WPD_KenBaseGateOn);
	}
	else if(-1 == ChangeState(tbl.nGateKen, m_typOldDmRecv.nGateKen)){
		LOG(em_MSG), "[%s] 検査WPDゲート信号 OFF", my_sThreadName);
		SetEvent(m_evPI_WPD_KenBaseGateOff);
	}

	// 表示WPDゲート信号
	if (1 == ChangeState(tbl.nGateDsp, m_typOldDmRecv.nGateDsp)) {
		LOG(em_MSG), "[%s] 表示WPDゲート信号 ON", my_sThreadName);
		SetEvent(m_evPI_WPD_DspBaseGateOn);
	}
	else if(-1 == ChangeState(tbl.nGateDsp, m_typOldDmRecv.nGateDsp)){
		LOG(em_MSG), "[%s] 表示WPDゲート信号 OFF", my_sThreadName);
		SetEvent(m_evPI_WPD_DspBaseGateOff);
	}	

	// 検査WPD  ※立ち上がりのみ
	if( 1 == ChangeState(tbl.pls.nWpd[0], m_typOldDmRecv.pls.nWpd[0]) ) {
		LOG(em_MSG), "[%s] 検査WPD ON [フレームNo:%d]", my_sThreadName, tbl.ken.nWpdOnFrameNo);
		// ===>> 坂巻 2021.06.03 小コイル長材通販時、コイル長比較が正常にできない不具合修正
		m_nWpdOnLength = tbl.nWpdOnLength;
		m_nWpdOnLengthSub = tbl.nWpdOnLengthSub;
		// <<===
		SetEvent(m_evPI_WPD_KenBase);
	}

	// 表示WPD  ※立ち上がりのみ
	if( 1 == ChangeState(tbl.pls.nWpd[2], m_typOldDmRecv.pls.nWpd[2]) ) {
		LOG(em_MSG), "[%s] 表示WPD ON", my_sThreadName);
		// ===>> 坂巻 2021.06.03 小コイル長材通販時、コイル長比較が正常にできない不具合修正
		m_nWpdOnLengthDsp = tbl.nWpdOnLengthDsp;
		// <<===
		SetEvent(m_evPI_WPD_DspBase);
	}

	// 出側ファーストカット信号
	if (1 == ChangeState(tbl.pls.nSharCutF, m_typOldDmRecv.pls.nSharCutF)) {
		LOG(em_MSG), "[%s] 出側ファーストカット信号 ON", my_sThreadName);
		SetEvent(m_evPI_ShrcutF);
	}

	// 出側ラストカット信号
	if (1 == ChangeState(tbl.pls.nSharCutL, m_typOldDmRecv.pls.nSharCutL)) {
		LOG(em_MSG), "[%s] 出側ラストカット信号 ON", my_sThreadName);
		SetEvent(m_evPI_ShrcutL);
	}

	// パトライトリセットPB
	if (1 == ChangeState(tbl.nPatoResetPb, m_typOldDmRecv.nPatoResetPb)) {
		LOG(em_MSG), "[%s] パトライトリセットPB ON", my_sThreadName);
		LOGA(em_MSG), "[%s] パトライトリセットPB ON", my_sThreadName);
		SetEvent(m_evPI_PatoReset);
	}
}

//------------------------------------------
// 低速用ステータス状態読み込み
//------------------------------------------
void DmemManager::ReadDmemStatusLow(SEQ_DM_RECV_BASE& tbl)
{
//#ifdef LOCAL
//	return;
//#endif
//	
	//int nVal;
	int nWk[2];

	//================================================
	// 処理

	// 機器状態関係
	// シーケンサ状態
	nWk[0] = tbl.nSeqSt;
	if( 0 == nWk[0] && 0 != mcls_pStatus->GetStatusRead().SeqState.stat ) {
		mcls_pStatus->SetStatusQue( mcls_pStatus->GetStatusRead().SeqState.id, 0, false);
		mcls_pStatus->SetStatusQue( mcls_pStatus->GetStatusRead().SeqState_War.id, 0, false);
		mcls_pStatus->SetStatusQue( mcls_pStatus->GetStatusRead().SeqState_Err.id, 0, false);
		LOG(em_MSG), "[%s] シーケンサ状態復旧", my_sThreadName);
		syslog(602, "正常復帰");
	}
	else if( 1 == nWk[0] && 1 != mcls_pStatus->GetStatusRead().SeqState.stat ) {
		mcls_pStatus->SetStatusQue( mcls_pStatus->GetStatusRead().SeqState.id, 1, false);
		mcls_pStatus->SetStatusQue(mcls_pStatus->GetStatusRead().SeqState_War.id, 1, false);
		mcls_pStatus->SetStatusQue(mcls_pStatus->GetStatusRead().SeqState_Err.id, 0, false);
		LOG(em_WAR), "[%s] シーケンサ 軽故障状態", my_sThreadName);
		syslog(603, "[軽故障]");
	}
	else if( 2 == nWk[0] && 2 != mcls_pStatus->GetStatusRead().SeqState.stat ) {
		mcls_pStatus->SetStatusQue( mcls_pStatus->GetStatusRead().SeqState.id, 2, false);
		mcls_pStatus->SetStatusQue(mcls_pStatus->GetStatusRead().SeqState_War.id, 0, false);
		mcls_pStatus->SetStatusQue(mcls_pStatus->GetStatusRead().SeqState_Err.id, 1, false);
		LOG(em_ERR), "[%s] シーケンサ 重故障状態", my_sThreadName);
		syslog(603, "[重故障]");
	}

	// シーケンサ稼動状態
	nWk[0] = tbl.nSeqKadou;
	if (0 == nWk[0] && 0 != mcls_pStatus->GetStatusRead().SeqKadou.stat) {
		mcls_pStatus->SetStatusQue(mcls_pStatus->GetStatusRead().SeqKadou.id, 0, false);
		LOG(em_MSG), "[%s] シーケンサ稼動状態: 正常", my_sThreadName);
		syslog(604, "[正常]");
	}
	else if (1 == nWk[0] && 1 != mcls_pStatus->GetStatusRead().SeqKadou.stat) {
		mcls_pStatus->SetStatusQue(mcls_pStatus->GetStatusRead().SeqKadou.id, 1, false);
		LOG(em_MSG), "[%s] シーケンサ稼動状態: 初期化済み", my_sThreadName);
		syslog(604, "[初期化済み]");
	}
	else if (2 == nWk[0] && 2 != mcls_pStatus->GetStatusRead().SeqKadou.stat) {
		mcls_pStatus->SetStatusQue(mcls_pStatus->GetStatusRead().SeqKadou.id, 2, false);
		LOG(em_MSG), "[%s] シーケンサ稼動状態: 予備", my_sThreadName);
		syslog(604, "[予備]");
	}
	else if (3 == nWk[0] && 3 != mcls_pStatus->GetStatusRead().SeqKadou.stat) {
		mcls_pStatus->SetStatusQue(mcls_pStatus->GetStatusRead().SeqKadou.id, 3, false);
		LOG(em_MSG), "[%s] シーケンサ稼動状態: 撮像中", my_sThreadName);
		syslog(604, "[撮像中]");
	}

	// 台車位置情報
	for (int ii = 0; ii < NUM_MEN; ii++) {
		nWk[0] = tbl.daisya[ii].nOnline;
		nWk[1] = tbl.daisya[ii].nOffline;
		if (1 == nWk[0] && 0 == nWk[1] && 0 != mcls_pStatus->GetStatusRead().DaisyaSt[ii].stat) {
			mcls_pStatus->SetStatusQue(mcls_pStatus->GetStatusRead().DaisyaSt[ii].id, 0, false);
			LOG(em_MSG), "[%s] %sカメラ台車オンライン位置", my_sThreadName, DivNameManager::GetTorB(ii));
			syslog(295, "[%s:オンライン位置]", DivNameManager::GetTorB(ii));
			
		}
		else if (0 == nWk[0] && 1 == nWk[1]  && 1 != mcls_pStatus->GetStatusRead().DaisyaSt[ii].stat) {
			mcls_pStatus->SetStatusQue(mcls_pStatus->GetStatusRead().DaisyaSt[ii].id, 1, false);
			LOG(em_ERR), "[%s] %sカメラ台車オフライン位置", my_sThreadName, DivNameManager::GetTorB(ii));
			syslog(295, "[%s:オフライン位置]", DivNameManager::GetTorB(ii));
		}
		else if ( ( (0 == nWk[0] && 0 == nWk[1]) || (1 == nWk[0] && 1 == nWk[1]) )&&
			-1 != mcls_pStatus->GetStatusRead().DaisyaSt[ii].stat) {
			mcls_pStatus->SetStatusQue(mcls_pStatus->GetStatusRead().DaisyaSt[ii].id, -1, false);
			LOG(em_ERR), "[%s] %sカメラ台車不明位置", my_sThreadName, DivNameManager::GetTorB(ii));
			syslog(295, "[%s:不明位置]", DivNameManager::GetTorB(ii));
		}
	}

	// 20210312 --->>>
	// 台車位置オンライン以外
	for (int ii = 0; ii < NUM_MEN; ii++) {
		nWk[0] = tbl.daisya[ii].nOnline;
		nWk[1] = tbl.daisya[ii].nOffline;
		if (1 == nWk[0] && 0 == nWk[1] && 0 != mcls_pStatus->GetStatusRead().DaisyaEx_St[ii].stat) {
			mcls_pStatus->SetStatusQue(mcls_pStatus->GetStatusRead().DaisyaEx_St[ii].id, 0, false);
			LOG(em_MSG), "[%s] %sφω台車位置オンライン以外（0:疵保存中） Frame:%d", my_sThreadName, DivNameManager::GetTorB(ii), tbl.ken.nFrameNo);
			syslog(298, "[%s:疵保存中 Frame:%d]", DivNameManager::GetTorB(ii), tbl.ken.nFrameNo);
		}
		else if (0 == nWk[0] && 1 != mcls_pStatus->GetStatusRead().DaisyaEx_St[ii].stat) {
			mcls_pStatus->SetStatusQue(mcls_pStatus->GetStatusRead().DaisyaEx_St[ii].id, 1, false);
			LOG(em_MSG), "[%s] %sφω台車位置オンライン以外（1:疵保存スキップ開始 Frame:%d）", my_sThreadName, DivNameManager::GetTorB(ii), tbl.ken.nFrameNo);
			syslog(298, "[%s:疵保存スキップ開始 Frame:%d]", DivNameManager::GetTorB(ii), tbl.ken.nFrameNo);
		}
		else if ((1 == nWk[0] && 1 == nWk[1]) && 1 != mcls_pStatus->GetStatusRead().DaisyaEx_St[ii].stat) {
			mcls_pStatus->SetStatusQue(mcls_pStatus->GetStatusRead().DaisyaEx_St[ii].id, 1, false);
			LOG(em_MSG), "[%s] %sφω台車位置オンライン以外（1:疵保存スキップ開始 Frame:%d）", my_sThreadName, DivNameManager::GetTorB(ii), tbl.ken.nFrameNo);
			syslog(298, "[%s:疵保存スキップ開始 Frame:%d]", DivNameManager::GetTorB(ii), tbl.ken.nFrameNo);
		}
	}
	// <<<---

	// チラー運転信号
	CString sChiler[3] = { "表面","裏面","BK" };
	for(int ii=0; ii<3; ii++) {
		nWk[0] = tbl.nChilerSt[ii];
		if (1 == nWk[0] && 0 != mcls_pStatus->GetStatusRead().ChilerPiStop[ii].stat) {
			mcls_pStatus->SetStatusQue(mcls_pStatus->GetStatusRead().ChilerPiStop[ii].id, 0, false);
			LOG(em_MSG), "[%s] %s PIチラー: 運転中", my_sThreadName, sChiler[ii]);
			syslog(610, "[%s]", sChiler[ii]);
		}
		else if (0 == nWk[0] && 1 != mcls_pStatus->GetStatusRead().ChilerPiStop[ii].stat) {
			mcls_pStatus->SetStatusQue(mcls_pStatus->GetStatusRead().ChilerPiStop[ii].id, 1, false);
			LOG(em_ERR), "[%s] %s PIチラー: 停止中", my_sThreadName, sChiler[ii]);
			syslog(611, "[%s]", sChiler[ii]);
		}
	}

	// チラー異常信号
	for(int ii=0; ii<3; ii++) {
		nWk[0] = tbl.nChilerErr[ii];
		if (1 == nWk[0] && 0 != mcls_pStatus->GetStatusRead().ChilerPiErr[ii].stat) {
			mcls_pStatus->SetStatusQue(mcls_pStatus->GetStatusRead().ChilerPiErr[ii].id, 0, true);
			LOG(em_MSG), "[%s] %s PIチラー: 正常", my_sThreadName, sChiler[ii]);
		}
		else if (0 == nWk[0] && 1 != mcls_pStatus->GetStatusRead().ChilerPiErr[ii].stat) {
			mcls_pStatus->SetStatusQue(mcls_pStatus->GetStatusRead().ChilerPiErr[ii].id, 1, true);
			LOG(em_ERR), "[%s] %s PIチラー: 異常", my_sThreadName, sChiler[ii]);
		}
	}

	// エアドライヤー停止信号
	for (int ii = 0; ii < NUM_MEN; ii++) {
		nWk[0] = tbl.AirBlow[ii].nAirBlowSt;
		if (1 == nWk[0] && 0 != mcls_pStatus->GetStatusRead().airblow[ii].AirBlowPiStop.stat) {
			mcls_pStatus->SetStatusQue(mcls_pStatus->GetStatusRead().airblow[ii].AirBlowPiStop.id, 0, true);
			LOG(em_MSG), "[%s] %s エアドライヤー: 運転中", my_sThreadName, DivNameManager::GetTorB(ii));
			syslog(613, "%s", DivNameManager::GetTorB(ii));
		}
		else if (0 == nWk[0] && 1 != mcls_pStatus->GetStatusRead().airblow[ii].AirBlowPiStop.stat) {
			mcls_pStatus->SetStatusQue(mcls_pStatus->GetStatusRead().airblow[ii].AirBlowPiStop.id, 1, true);
			LOG(em_WAR), "[%s] %s エアドライヤー: 停止中", my_sThreadName, DivNameManager::GetTorB(ii));
			syslog(612, "%s", DivNameManager::GetTorB(ii));
		}

		// エアドライヤー異常信号
		nWk[0] = tbl.AirBlow[ii].nAirBlowErr;
		if (1 == nWk[0] && 0 != mcls_pStatus->GetStatusRead().airblow[ii].AirBlowPiErr.stat) {
			mcls_pStatus->SetStatusQue(mcls_pStatus->GetStatusRead().airblow[ii].AirBlowPiErr.id, 0, true);
			LOG(em_MSG), "[%s] %s エアドライヤー: 正常", my_sThreadName, DivNameManager::GetTorB(ii));
		}
		else if (0 == nWk[0] && 1 != mcls_pStatus->GetStatusRead().airblow[ii].AirBlowPiErr.stat) {
			mcls_pStatus->SetStatusQue(mcls_pStatus->GetStatusRead().airblow[ii].AirBlowPiErr.id, 1, true);
			LOG(em_ERR), "[%s] %s エアドライヤー: 異常", my_sThreadName, DivNameManager::GetTorB(ii));
		}
	}

	// 台車制御盤INV電源OFF
	nWk[0] = tbl.nDaisyaInvSt;
	if (1 == nWk[0] && 0 == m_typOldDmRecvLow.nDaisyaInvSt) {
		LOG(em_ERR), "[%s] 台車制御盤 INV電源: 電源OFF", my_sThreadName);
	}
	else if (0 == nWk[0] && 1 == m_typOldDmRecvLow.nDaisyaInvSt) {
		LOG(em_MSG), "[%s] 台車制御盤 INV電源: 電源ON", my_sThreadName);
	}


	// 電気室 扉 開閉状態
	nWk[0] = tbl.Sec.nDoor;
	// 開状態（PI:0、STATUS:1）←値が逆なので注意！
	if (0 == nWk[0] && 1 != mcls_pStatus->GetStatusRead().SecurityDoor.stat) {
		mcls_pStatus->SetStatusQue(mcls_pStatus->GetStatusRead().SecurityDoor.id, 1, false);
		LOG(em_MSG), "[%s] 電気室　扉　開状態", my_sThreadName);
		syslog(616, "[扉; 閉状態→開状態]");
	}
	// 閉状態（PI:1、STATUS:0）
	else if (1 == nWk[0] && 0 != mcls_pStatus->GetStatusRead().SecurityDoor.stat) {
		mcls_pStatus->SetStatusQue(mcls_pStatus->GetStatusRead().SecurityDoor.id, 0, false);
		LOG(em_MSG), "[%s] 電気室　扉　閉状態", my_sThreadName);
		syslog(616, "[扉; 開状態→閉状態]");
	}

	// 電気室 鍵 開閉状態
	nWk[0] = tbl.Sec.nKey;
	// 開状態（PI:0、STAT1US:1）←値が逆なので注意！
	if (0 == nWk[0] && 1 != mcls_pStatus->GetStatusRead().SecurityKey.stat) {
		mcls_pStatus->SetStatusQue(mcls_pStatus->GetStatusRead().SecurityKey.id, 1, false);
		LOG(em_MSG), "[%s] 電気室　鍵　開状態", my_sThreadName);
		syslog(617, "[鍵; 閉状態→開状態]");
	}
	// 閉状態（PI:1、STATUS:0）
	else if (1 == nWk[0] && 0 != mcls_pStatus->GetStatusRead().SecurityKey.stat) {
		mcls_pStatus->SetStatusQue(mcls_pStatus->GetStatusRead().SecurityKey.id, 0, false);
		LOG(em_MSG), "[%s] 電気室　鍵　閉状態", my_sThreadName);
		syslog(617, "[鍵; 開状態→閉状態]");
	}

	// パルスレート変更確認
	int nNowPls[3];	// 0.0001mm/pls
					// [0:検査用PLG, 1:検査比較用PLG, 2:表示用PLG]

	nNowPls[0] = tbl.nPls;
	nNowPls[1] = tbl.nPlsSub;
	nNowPls[2] = tbl.nPlsDsp;

	if( (m_nPlsRate[0] != nNowPls[0] && 0 != nNowPls[0]) ||
		(m_nPlsRate[1] != nNowPls[1] && 0 != nNowPls[1]) ||
		(m_nPlsRate[2] != nNowPls[2] && 0 != nNowPls[2]) ){

		if( (0 >= nNowPls[0] || 0 >= nNowPls[1] || 0 >= nNowPls[2] ) && m_bInitReadOk ){
			LOG(em_WAR), "[%s] シーケンサより入力されたパルスレートが異常[検査用:%.4fmm/pls][検査比較用:%.4fmm/pls][表示用:%.4fmm/pls][※生値:%d,%d,%d]", my_sThreadName, nNowPls[0]/ PLS_RATE_TANI, nNowPls[1]/ PLS_RATE_TANI, nNowPls[2]/ PLS_RATE_TANI, nNowPls[0], nNowPls[1], nNowPls[2]);
			syslog(605, "検査用:%.4fmm/pls, 検査比較用:%.4fmm/pls, 表示用:%.4fmm/pls", nNowPls[0] / PLS_RATE_TANI, nNowPls[1] / PLS_RATE_TANI, nNowPls[2] / PLS_RATE_TANI);

		}else{
			// ログ出力
			for(int ii=0;ii<3; ii++) {
				if(m_nPlsRate[ii] != nNowPls[ii]) {
					CString sWk[3] = { "検査用", "検査比較用", "表示用" };
					LOG(em_MSG),"[%s] シーケンサより%sパルスレート変更[%d → %d]", sWk[ii], my_sThreadName, m_nPlsRate[ii], nNowPls[ii]);
					syslog(606, "[%s：%.4fmm/pls→%.4fmm/pls]", sWk[ii], m_nPlsRate[ii] / PLS_RATE_TANI, nNowPls[ii] / PLS_RATE_TANI);
				}
			}

			// パルスレート、長手分解能のDB更新
			UpdatePlsRate(nNowPls);
		}
		memcpy(m_nPlsRate, nNowPls, sizeof(m_nPlsRate) );
	}

	//// SPM開放信号
	bool bLog = false;		// true時、状態変更のログを出力
	for (int ii = 0; ii < 2; ii++) {
		nWk[0] = tbl.nSpm;
		nWk[1] = mcls_pStatus->GetSpmStat();
		if (0 == nWk[0] && 1 != nWk[1]) {
			// 圧下状態（PI:0、STAT1US:1）←値が逆なので注意！
			mcls_pStatus->SetSpmStat(DIV_SPM_TRUE);
			LOG(em_MSG), "[%s] SPM開放信号 圧下中", my_sThreadName);
			syslog(294, "[SPM状態 開放→圧下]");
		}
		else if(1 == nWk[0] && 0 != nWk[1]) {
			// 開放状態（PI:1、STATUS:0）
			mcls_pStatus->SetSpmStat(DIV_SPM_FALSE);
			LOG(em_MSG), "[%s] SPM開放信号 開放中", my_sThreadName);
			syslog(294, "[SPM状態 圧下→開放]");
		}
	}

	//// 塗油スプレー吹き付け状態
	nWk[0] = tbl.nToyuSply;
	// 未吹付状態（PI:0、STAT1US:0）
	if (0 == nWk[0] && 0 != mcls_pStatus->GetStatusRead().ToyuSprSt.stat) {
		mcls_pStatus->SetStatusQue(mcls_pStatus->GetStatusRead().ToyuSprSt.id, 0, false);
		LOG(em_MSG), "[%s] 塗油スプレー装置 吹付状態 未吹付状態", my_sThreadName);
		syslog(618, "[吹付状態; 吹付中状態→未吹付状態]");
	}
	// 吹付中状態（PI:1、STATUS:1）
	else if (1 == nWk[0] && 1 != mcls_pStatus->GetStatusRead().ToyuSprSt.stat) {
		mcls_pStatus->SetStatusQue(mcls_pStatus->GetStatusRead().ToyuSprSt.id, 1, false);
		LOG(em_MSG), "[%s] 塗油スプレー装置 吹付状態 吹付中状態", my_sThreadName);
		syslog(618, "[吹付状態; 未吹付状態→吹付中状態]");
	}
}

//==========================================
// 疵接近ランプ状態書き込み
//------------------------------------------
// OCCUR_RAMP typRamp: ランプ種類
//==========================================
void DmemManager::WritDmemRamp(OCCUR_RAMP typRamp)
{
	//memcpy(mtbl_pSend->nSekkin, typRamp.ranp, sizeof(mtbl_pSend->nSekkin));
	for (int ii = 0; ii<NUM_MEN; ii++) {
		for (int jj = 0; jj < OUTPUT_MESYU_SEKKIN_COL; jj++) {
			for (int kk = 0; kk < OUTPUT_RAMP_NUM; kk++) {
				mtbl_pSend->nSekkin[ii][jj][kk] = typRamp.ranp[ii][jj][kk];
			}
		}
	}

	for(int ii=0; ii<NUM_MEN; ii++) {
		for (int jj=0; jj < OUTPUT_MESYU_SEKKIN_COL; jj++) {
			for (int kk=OUTPUT_RAMP_NUM-1; kk>=0 ; kk--) {
				if(em_RAMP_STAT_ON == typRamp.ranp[ii][jj][kk]) {
					LOGA(em_MSG), "[%s] 疵接近ランプ 発生[men=%s, 位置=%d, %s]", my_sThreadName, DivNameManager::GetTorB(ii), jj, 0==kk?"軽":"重");
					m_nRamp[ii][jj] = kk;
					break;
				}
				m_nRamp[ii][jj] = -1;
			}
		}
	}
}

//==========================================
// 疵接近アラーム状態書き込み
//------------------------------------------
// OCCUR_ALARM typAlarm: アラーム種類
//==========================================
void DmemManager::WritDmemAlarm(OCCUR_ALARM typAlarm)
{
	for (int ii = 0; ii < OUTPUT_ALARM_COL; ii++) {
		for (int jj = 0; jj < OUTPUT_RAMP_NUM; jj++) {
			mtbl_pSend->nSekalm[ii][jj] = typAlarm.alarm[ii][jj];
		}
	}

	for (int ii = 0; ii < OUTPUT_MESYU_SEKKIN_COL; ii++) {
		for (int jj = OUTPUT_RAMP_NUM - 1; jj >= 0; jj--) {
			if (em_RAMP_STAT_ON == typAlarm.alarmPos[ii][jj]) {
				LOGA(em_MSG), "[%s] 疵接近アラーム 発生[位置=%d, %s]", my_sThreadName, ii, 0 == jj ? "軽" : "重");
				m_nAlarm[ii] = jj;
				break;
			}
			m_nAlarm[ii] = -1;
		}
	}
}

//==========================================
// パトライト状態書き込み
//------------------------------------------
// OCCUR_PATO typPato: パトライト種類
//==========================================
void DmemManager::WritDmemPato(OCCUR_PATO typPato)
{
	memcpy(mtbl_pSend->nPato, typPato.pato, sizeof(mtbl_pSend->nPato));
	int nPatoCnt = 0;
	for (int ii = 0; ii<OUTPUT_PATO_POS_NUM; ii++) {
		nPatoCnt += typPato.pato[ii][0];
	}
	for (int ii = 0; ii<OUTPUT_PATO_POS_NUM; ii++) {
		for (int jj = 0; jj < OUTPUT_PATO_KIND_NUM; jj++) {
			mtbl_pSend->nPato[ii][jj] = nPatoCnt;
		}
	}
	memcpy( m_nPato, typPato.pato, sizeof(m_nPato));
	//LOG(em_INF), "[%s] パトライト受信[保有個数=%d/%d/%d/%d]", my_sThreadName, typPato.pato[0][0], typPato.pato[0][1], typPato.pato[1][0], typPato.pato[1][1]);
	LOGA(em_MSG), "[%s] パトライト個数変更[保有個数=%d/%d/%d/%d 総数=%d]", my_sThreadName, m_nPato[0][0], m_nPato[0][1], m_nPato[1][0], m_nPato[1][1], nPatoCnt);
}

//==========================================
// 欠陥検出ボイス状態書き込み
//------------------------------------------
// OCCUR_VOICE_ERR	typVoice: ボイス種類
//==========================================
void DmemManager::WritDmemVoice(OCCUR_VOICE typVoice)
{
	int nVoiceMen = -1;
	int nVoice = -1;

	// 設備異常？ or 欠陥検出？
	bool bWk = false;
	for (int ii = 0; ii <OUTPUT_VOICE_NUM; ii++) {
		if (em_VOICE_DEF_TOP == ii) continue;
		if (em_VOICE_DEF_BOT == ii) continue;
		if (em_VOICE_STAT_ON == typVoice.voice[ii]) {
			if (OUTPUT_VOICE_ERR_NUM > ii) {
				nVoiceMen = -1;
				nVoice = ii;
				bWk = true;
			}
			else {
				nVoiceMen = (em_VOICE_STAT_ON == typVoice.voice[em_VOICE_DEF_TOP])? 0 : 1;
				nVoice = ii;
			}
			break;
		}
	}
	// 該当なし 
	if(-1 == nVoice) {
		WritDmemVoiceReset();
		return;
	}

	// メンバー変数にセット
	m_nVoiceMen = nVoiceMen;
	m_nVoice = nVoice;
	memset(&mtbl_pSend->typKensaVoice, 0x00, sizeof(mtbl_pSend->typKensaVoice));
	// 設備異常
	if(bWk) {
		mtbl_pSend->typKensaVoice.nVoice_Err[m_nVoice] = 1;
		CancelWaitableTimer(m_evTimeAlarmePulse);
		LOGA(em_MSG), "[%s] 設備異常ボイス 出力[id=%d]", my_sThreadName, m_nVoice);
	}
	// 欠陥検出
	else {
		mtbl_pSend->typKensaVoice.nVoice_Def[nVoiceMen] = 1;
		mtbl_pSend->typKensaVoice.nVoice_Def[m_nVoice - OUTPUT_VOICE_ERR_NUM] = 1;
		// パルス周期をタイマーセット
		__int64 ts = PARAM_TIME_PULSE_DEF * -10000;	// 1ミリ秒→100ナノ秒 (マイナス指定で、指定時間経過でシグナル) (第3引数は2回目以降の起動間隔)
		SetWaitableTimer(m_evTimeAlarmePulse, (LARGE_INTEGER *)&ts, NULL, NULL, NULL, FALSE);
		LOGA(em_MSG), "[%s] 欠陥検出ボイス 出力[men=%d, id=%d]", my_sThreadName, nVoiceMen, m_nVoice);
	}
}

//==========================================
// 欠陥検出ボイス状態書き込みリセット
//==========================================
void DmemManager::WritDmemVoiceReset()
{
	// パルスON出力チェック
	int ii;
	for (ii = 0; ii <OUTPUT_VOICE_NUM; ii++)
	{
		if (em_VOICE_DEF_TOP == ii) continue;
		if (em_VOICE_DEF_BOT == ii) continue;
		if (em_VOICE_STAT_ON == mtbl_pSend->typKensaVoice.nVoice_Def[ii]) break;
	}

	if (OUTPUT_VOICE_NUM != ii)
	{
		// OFF出力開始
		memset(&mtbl_pSend->typKensaVoice, 0x00, sizeof(mtbl_pSend->typKensaVoice));

		__int64 ts = PARAM_TIME_PULSE * -10000;	// 1ミリ秒→100ナノ秒 (マイナス指定で、指定時間経過でシグナル) (第3引数は2回目以降の起動間隔)
		SetWaitableTimer(m_evTimeAlarmePulse, (LARGE_INTEGER *)&ts, NULL, NULL, NULL, FALSE);
	}
	else
	{
		// パルス出力完了
		m_nVoiceMen = -1;
		m_nVoice = -1;
	}
}
//------------------------------------------
// 1秒パルス出力指示
// EM_EVENT_PLS_OUT emPls  指示内容
// int nVal 書き込み値 通常1
//------------------------------------------
void DmemManager::SetData_Pls(EM_EVENT_PLS_OUT emPls, int nVal)
{
	AutoLock autolock(&m_csLock);

	// PLS出力依頼
	m_nOutPls[emPls] = nVal;
	LOG(em_MSG), "[%s] %sパルス出力依頼", my_sThreadName, pls_out_name[emPls]);
}



//==========================================
// カウンタリセット要求信号 [←MI]
//==========================================
void DmemManager::PoPuls_CountReset()
{
	AutoLock autolock(&m_csLock);
	LOG(em_MSG), "[%s] シーケンサカウンタリセット要求", my_sThreadName);
	mtbl_pSend->nCntReset = 1;

	//// PLGカウンタリセット要求中フラグをON
	//m_bPlgReset = true;
	
	// パルス周期をタイマーセット
	__int64 ts = PARAM_TIME_PULSE * -10000;	// 1ミリ秒→100ナノ秒 (マイナス指定で、指定時間経過でシグナル) (第3引数は2回目以降の起動間隔)
	SetWaitableTimer(m_evTimeCntResetPulse, (LARGE_INTEGER *)&ts, NULL, NULL, NULL, FALSE);
}

//==========================================
// カウンタ開始要求信号 [←MI]
//==========================================
void DmemManager::PoPuls_CountStart()
{
	AutoLock autolock(&m_csLock);
	LOG(em_MSG), "[%s] シーケンサカウンタ開始要求信号", my_sThreadName);
	mtbl_pSend->nCntStart = 1;

	//// PLGカウンタリセット要求中フラグをON
	//m_bPlgReset = false;

	// パルス周期をタイマーセット
	__int64 ts = PARAM_TIME_PULSE * -10000;	// 1ミリ秒→100ナノ秒 (マイナス指定で、指定時間経過でシグナル) (第3引数は2回目以降の起動間隔)
	SetWaitableTimer(m_evTimeCntStartPulse, (LARGE_INTEGER *)&ts, NULL, NULL, NULL, FALSE);
}

//==========================================
// テスト用 WPD信号 ON要求信号 [←MI]
//------------------------------------------
// int	nPos: 位置(1:検査基点WPD、2:表示基点WPD)
//==========================================
void DmemManager::PoPuls_TesetWpdOn(int nPos)
{
	// 1:検査基点WPD、2:表示基点WPD、3:検査基点WPD(DMのみ)、4:表示基点WPD(DMのみ)以外はreturn
	if(0 >= nPos && 5 <= nPos) return;

	AutoLock autolock(&m_csLock);
	mtbl_pSend->nTestWpdReq = nPos;

	// パルス周期をタイマーセット
	__int64 ts = PARAM_TIME_PULSE * -10000;	// 1ミリ秒→100ナノ秒 (マイナス指定で、指定時間経過でシグナル) (第3引数は2回目以降の起動間隔)
	SetWaitableTimer(m_evTimeTestWpdPulse, (LARGE_INTEGER *)&ts, NULL, NULL, NULL, FALSE);
}

//==========================================
// パルスレート、長手分解能のDB更新
//------------------------------------------
// int			nPls	: パルスレート
//==========================================
void DmemManager::UpdatePlsRate(int* nPls)
{
	//// 事前準備
	DBManager::DeliveryDB *deliDB = new DBManager::DeliveryDB;
	deliDB->kubun = DBManager::em_PULSE_RATE_UPDATE;
	deliDB->data = (BYTE*)new DBDATA_PULSE_RATE_UPDATE;
	DBDATA_PULSE_RATE_UPDATE* pWk = (DBDATA_PULSE_RATE_UPDATE*)deliDB->data;

	//// データセット
	for (int ii = 0; ii < 3; ii++) {
		pWk->dPls[ii] = ((double)nPls[ii]) / PLS_RATE_TANI;
		pWk->dVres[ii] = pWk->dPls[ii] * mcls_pParam->GetParamCommon().typCamCon[DIV_PLG_LINE - 1].nDiv / mcls_pParam->GetParamCommon().typCamCon[DIV_PLG_LINE - 1].nMul;
	}
	//// 登録依頼
	if (!mque_pAddSQLData->AddToTailFreeCheck(deliDB, 20)) {		// 詰まらないように空きチェック
																	// 登録失敗
		LOG(em_ERR), "[%s] DB登録依頼キューフル [DBDATA_PULSE_RATE_UPDATE]", my_sThreadName);
		syslog(SYSNO_QUEFULL_ERR, "[DBDATA_PULSE_RATE_UPDATE]");
		delete deliDB->data;
		delete deliDB;
		// DBが異常(通常ありえない)
		mcls_pStatus->AddQueMyErrWar(true, ON_DB_ACCESS_ERR);
	}
}