#include "StdAfx.h"
#include "StatusManager.h"

// メモリーリーク検出 ----->>>
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif
// <<<------------------------

#include "MainInstance.h"

//------------------------------------------
// コンストラクタ
//------------------------------------------
StatusManager::StatusManager(void) :
m_bIsDoorOpen(false),
m_bIsKeyOpen(false),
m_pCoil(NULL)
{
	int ii;

	//// 判定機能 初期化
	memset( m_nHt_ID, 0x00, sizeof(m_nHt_ID));
	//memset( m_nCy_ID, 0x00, sizeof(m_nCy_ID));
	memset(m_nHt_Para_ID, 0x00, sizeof(m_nHt_Para_ID));
	for(ii=0; ii< NUM_MEN; ii++) SetHtState_AllOff(ii);
	//for(ii=0; ii< NUM_MEN; ii++) SetCyState_AllOff(ii);	
	SetHtParaState_AllOff();

	//// タイマー
	m_evTimerWriteSt    = CreateWaitableTimer(NULL, FALSE, NULL);
	m_evTimerSogyoSt    = CreateWaitableTimer(NULL, FALSE, NULL);
	

	//// LED照明関係
	m_bLedAutoCheckOff = false;
	m_nLedLineStopOffTime = GetPrivateProfileInt(TO_DEFECT, "LED_LINESTOP_OFFTIME", 5, TASKINI_NAME);	// LED照明 ライン休止OFF時間 [分]
	m_nLedDaisyaOffLineTime = GetPrivateProfileInt(TO_DEFECT, "LED_DAISYA_OFFTIME", -1, TASKINI_NAME);	// LED照明 台車オフラインOFF時間 [秒]
	for(int ii=0; ii< NUM_MEN; ii++) {
		m_nLedLineStopCnt[ii]	= 0;						// ライン停止時の経過回数
		m_nLedLineRunOffCnt[ii]	= 0;
		m_nLedDaisyaOffLineCnt[ii] = 0;						// 台車オフライン時の経過回数
	}

	//// 機器状態関連 初期化
	// カメラ温度
	memset(m_nCamTemp, 0x00, sizeof(m_nCamTemp));

	// カメラゲイン
	memset(m_nCamGain, 0x00, sizeof(m_nCamGain));

	// カメラ露光時間
	memset(m_nExpTime, 0x00, sizeof(m_nExpTime));

	// カメラ露光時間
	memset(m_nBrValue, 0x00, sizeof(m_nBrValue));

	// カメラ電圧
	memset(m_nCamVolt, 0x00, sizeof(m_nCamVolt));
	
	// 通板状態関係
	m_nLineSpeed	= 0;
	m_nLength		= 0;
#ifdef DSP_SOGYO_CHECK
	m_nLineSpeedDsp	= 0;
#endif

	// ライン信号関係
	m_emSpmStat= DIV_SPM_INI;				// SPM状態

	// LED照明装置調光値
	memset(m_nLedValue, 0x00, sizeof(m_nLedValue));

	// エッジUDP状態
	for(int ii=0; ii<NUM_MEN; ii++) {
		m_bEdgeUdpHealthySt[ii] = false;
		m_bEdgeUdpTrackingSt[ii] = false;
	}

	//// 共有メモリ
	// カメコン情報テーブル
#ifdef IS_TO_CAMERA_TBL
#ifdef LOCAL
	m_hCam = CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, sizeof(TO_CAMERA_TBL), TBL_TO_CAMERA_NAME);
	mtbl_pCam =(TO_CAMERA_TBL*) MapViewOfFile(m_hCam, FILE_MAP_ALL_ACCESS, 0, 0, 0);
#endif

	mem_connect(&m_hCam, (LPVOID *)&mtbl_pCam, TBL_TO_CAMERA_NAME);
	if(m_hCam == NULL) {
		LOG(em_ERR), "[CameraManager] カメコン情報テーブル アクセス失敗");
		syslog(SYSNO_MEM_OPEN, "カメコン情報テーブル アクセス失敗");
		_ASSERT(FALSE);
	}
	LOG(em_INF), "[CameraManager] カメコン情報テーブル アクセスモード");
#endif

}

//------------------------------------------
// デストラクタ
//------------------------------------------
StatusManager::~StatusManager(void)
{
	CloseHandle(m_evTimerWriteSt);
	CloseHandle(m_evTimerSogyoSt);

	// カメコン情報テーブル
#ifdef IS_TO_CAMERA_TBL
	if( NULL != m_hCam ) mem_close(&m_hCam, (LPVOID *)&mtbl_pCam);							// カメコン情報
#endif
}

// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// スレッド操作

//------------------------------------------
// シグナル待ち行列の追加
//------------------------------------------
void StatusManager::ThreadFirst_AddHandle() 
{
	//// 拡張設定


	// 定周期タイマ起動
	__int64 ts;
	ts = (__int64)PARAM_ST_WRITE_INTERVAL_TIME * -10000;			// 1ミリ秒→100ナノ秒 (マイナス指定で、指定時間経過でシグナル) (第3引数は2回目以降の起動間隔)
	SetWaitableTimer(m_evTimerWriteSt,   (LARGE_INTEGER *)&ts, PARAM_ST_WRITE_INTERVAL_TIME, NULL, NULL, FALSE);

	ts = (__int64)PARAM_ST_SOGYO_INTERVAL_TIME * -10000;			// 1ミリ秒→100ナノ秒 (マイナス指定で、指定時間経過でシグナル) (第3引数は2回目以降の起動間隔)
	SetWaitableTimer(m_evTimerSogyoSt,   (LARGE_INTEGER *)&ts, PARAM_ST_SOGYO_INTERVAL_TIME, NULL, NULL, FALSE);

	// 追加シグナルの定義
	AddEventNum(1, &m_evTimerWriteSt);
	AddEventNum(1, &m_evTimerSogyoSt);
}

//------------------------------------------
// シグナルを追加した奴のイベント (追加したものを0始まりとする)
// int nEventNo イベントNo (0オリジン)
//------------------------------------------
void StatusManager::OnAddSignal(int nEventNo) 
{
	////// シグナルの条件
	enum {	EV_STATUS_WRIT = 0,					// ステータス書き込み
			EV_SOGYO_WRIT						// 操業状態書き込み
	};

	////// シグナル条件分け
	switch (nEventNo) {
	case EV_STATUS_WRIT:						// ステータス書き込み
		WriteStatus();
		break;

	case EV_SOGYO_WRIT:							// 操業状態書き込み
		SogyoStatus_LedOnOff();
		break;

	}
}


// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 機器状態書き込み

//------------------------------------------
// ステータス書き込み処理
//------------------------------------------
void StatusManager::WriteStatus() 
{
	if( NULL == mcls_pParam) return;

	//CString wk;
	int ii, jj;
	int nId;

	//================================================
	// カメラ温度
	for(ii=0; ii<MAX_CAMSET; ii++) {
		for(jj=0; jj<MAX_PHYSICAL_CAMANGLE; jj++) {
			// カメラ温度異常チェック
			//wk.Format("CAM_TEMPSTATE_%d_%d_ID", ii+1, jj+1);
			nId	 = this->GetStatusRead().CamTempSt[ii*MAX_PHYSICAL_CAMANGLE+jj].id;
			if( m_nCamTemp[ii][jj] >= mcls_pParam->GetCommonMast().nCamTempThr ) {		// 異常
				if( 0 == m_oldStateRead.CamTempSt[ii*MAX_PHYSICAL_CAMANGLE+jj].stat )	this->SetStatusQue(nId, false);
			} else {																	// 正常復帰
				if( 0 != m_oldStateRead.CamTempSt[ii*MAX_PHYSICAL_CAMANGLE+jj].stat )	this->SetStatusQue(nId, true);
			}

			// カメラ温度セット
			if( m_oldStateRead.CamTemp[ii*MAX_PHYSICAL_CAMANGLE+jj].stat != m_nCamTemp[ii][jj]) {
				//wk.Format("CAM_TEMP_%d_%d_ID", ii+1, jj+1);
				nId	 = this->GetStatusRead().CamTemp[ii*MAX_PHYSICAL_CAMANGLE+jj].id;
				this->SetStatusQue(nId, m_nCamTemp[ii][jj], false);
			}
		}
	}

	//================================================
	// カメラゲイン
	for (ii = 0; ii<MAX_CAMANGLE - 1; ii++) {
		for (jj = 0; jj<MAX_CAMSET; jj++) {
				nId = this->GetStatusRead().CamGain[ii][jj].id;
			int nNowGain = this->GetStatusRead().CamGain[ii][jj].stat;

			if (0 != m_nCamGain[jj][ii] && nNowGain != m_nCamGain[jj][ii]) {
				this->SetStatusQue(nId, m_nCamGain[jj][ii], false);
			}
		}
	}

	//================================================
	// カメラ露光時間
	for (ii = 0; ii<MAX_CAMANGLE - 1; ii++) {
		for (jj = 0; jj<MAX_CAMSET; jj++) {
			nId = this->GetStatusRead().CamExpTime[ii][jj].id;
			int nNowExp = this->GetStatusRead().CamExpTime[ii][jj].stat;
			if( m_nExpTime[jj][ii] != nNowExp ){
				this->SetStatusQue(nId, m_nExpTime[jj][ii], false);
			}
		}
	}

	//================================================
	// カメラ輝度
	for (ii = 0; ii<MAX_CAMANGLE - 1; ii++) {
		for (jj = 0; jj<MAX_CAMSET; jj++) {
			nId = this->GetStatusRead().CamBrValue[ii][jj].id;
			int nNowBr = this->GetStatusRead().CamBrValue[ii][jj].stat;
			if(m_nBrValue[jj][ii] != nNowBr){
				this->SetStatusQue(nId, m_nBrValue[jj][ii], false);
			}
		}
	}

	//================================================
	// カメラ電圧時間
	for(ii=0; ii<NUM_CAMERA; ii++){
		nId = this->GetStatusRead().CamVolt[ii].id;
		int nNowVolt = this->GetStatusRead().CamVolt[ii].stat;
		if(m_nCamVolt[ii] != nNowVolt){
			this->SetStatusQue(nId, m_nCamVolt[ii], false);
		}
	}

	//================================================
	// LED照明 消灯チェック
	// ラインが動いているのに、消灯していたらNGよ
	for( ii=0; ii<NUM_MEN; ii++) {
		bool bSate = false;			// 1灯でも消灯中 且つ   通板中 or 低速操業

		// 状態チェック
		// 台車がオンライン位置にあることが前提 植松仕様 20150206
		for( jj=0; jj<DIV_LED_COLOR_END; jj++) {
#ifdef DISABLE_LED_DAISYA
			if( 0 == m_oldStateRead.led[ii].ch[jj].OnOff.stat &&
				//0 == m_oldStateRead.DaisyaErr[ii].stat &&
				(DIV_SOGYO_RUN == m_emSogyo || DIV_SOGYO_SLOW == m_emSogyo) ) {
#else
			if( 0 == m_oldStateRead.led.ch.OnOff[ii][jj].stat &&
				0 == m_oldStateRead.DaisyaErr[ii].stat &&
				(DIV_SOGYO_RUN == m_emSogyo || DIV_SOGYO_SLOW == m_emSogyo) ) {
#endif

				bSate = true;
			}
		}

		// ステータス変更
		if(bSate) {
			if( 0 == m_oldStateRead.LedRunOff[ii].stat ) {
				m_nLedLineRunOffCnt[ii]++;
				if(2 <= m_nLedLineRunOffCnt[ii]) {
					//wk.Format("LED_RUNOFF_%d_ID", ii+1);
					nId	 = this->GetStatusRead().LedRunOff[ii].id;
					this->SetStatusQue(nId, false);
					LOG(em_ERR), "[%s] %s LED照明装置 ライン稼動中に消灯 異常", my_sThreadName, DivNameManager::GetTorB(ii));
				}
			}	

		} else {
			m_nLedLineRunOffCnt[ii] = 0;
			if( 0 != m_oldStateRead.LedRunOff[ii].stat ) {
				//wk.Format("LED_RUNOFF_%d_ID", ii+1);
				nId	 = this->GetStatusRead().LedRunOff[ii].id;
				this->SetStatusQue(nId, true);
			}
		}

		// 調光値変更
		for(int jj=0; jj<DIV_LED_COLOR_END; jj++) {
			// LEDがOFFの時は無視
			if(1 == this->GetStatusRead().led.ch.OnOff[ii][jj].stat) {
				m_nLedValue[ii][jj] =  this->GetStatusRead().led.ch.ValNow[ii][jj].stat;
			}
			else {
				m_nLedValue[ii][jj] = 0;
			}
		}
	}

	//================================================
	// ライン状態
	// 命令No
	int nWk = 0;
	if (NULL != m_pCoil) {
		// 汎用的に、文字も含む奴の中から 数値部 を取得する
		CString sWk = m_pCoil->data.cMeiNo;			// 終端は必ずNULLなのでそのまま突っ込む
		nWk = atoi((LPCTSTR)sWk);
	}
	if (m_oldStateRead.Line_Mei.stat != nWk) {
		this->SetStatusQue(this->GetStatusRead().Line_Mei.id, nWk, false);
	}
	// コイルNo
	if( NULL != m_pCoil ) {
		// 汎用的に、文字も含む奴の中から 数値部 を取得する
		CString sWk = m_pCoil->data.cCoilNoIn;			// 終端は必ずNULLなのでそのまま突っ込む
		sWk.Replace("-", "");									// コイルNoが9-9999のようにハイフンが入るため、除外

		int nNumstart = sWk.FindOneOf("0123456789");	// 先頭が 文字対策
		CString sNum = sWk.Mid(nNumstart).SpanIncluding("0123456789");
		nWk = atoi((LPCTSTR)sNum);
	}
	if( m_oldStateRead.Line_No.stat != nWk ) {
		//this->SetStatusQue("LINE_CNO_ID", nWk, false);
		this->SetStatusQue(this->GetStatusRead().Line_No.id, nWk, false);
	}
	// 通板距離
	if( m_oldStateRead.Line_Len.stat != m_nLength ) {
		//this->SetStatusQue("LINE_LEN_ID", m_nLength, false);
		this->SetStatusQue(this->GetStatusRead().Line_Len.id, m_nLength, false);
	}
	// 通板残長
	nWk = 0;
	if( NULL != m_pCoil ) {
		//nWk = (m_pCoil->nCoilLenIn/1000) - m_nLength;
		nWk = (m_pCoil->data.nLenIn) - m_nLength;
	}
	if( m_oldStateRead.Line_ZanLen.stat != nWk ) {
		//this->SetStatusQue("LINE_ZANLEN_ID", nWk, false);
		this->SetStatusQue(this->GetStatusRead().Line_ZanLen.id, nWk, false);
	}
	// ライン速度
	if( m_oldStateRead.Line_Speed.stat != m_nLineSpeed ) {
		//this->SetStatusQue("LINE_SPEED_ID", m_nLineSpeed, false);
		this->SetStatusQue(this->GetStatusRead().Line_Speed.id, m_nLineSpeed, false);
	}


	//================================================
	//// カメラ台車位置チェック
	// ラインが動いているのに、オフライン位置ならNG
	for (int ii = 0; ii<NUM_MEN; ii++) {
		// オンライン位置じゃない 且つ
		// 通板中 or 低速操業
		if (0 != m_oldStateRead.DaisyaSt[ii].stat &&
			(DIV_SOGYO_RUN == m_emSogyo || DIV_SOGYO_SLOW == m_emSogyo)) {

			if (0 == m_oldStateRead.DaisyaErr[ii].stat) {
				this->SetStatusQue(this->GetStatusRead().DaisyaErr[ii].id, false, false);
				LOG(em_ERR), "[StatusManager] %s ライン稼動時にカメラ台車がオンライン位置で無い", DivNameManager::GetTorB(ii));
			}
		}
		else {
			if (0 != m_oldStateRead.DaisyaErr[ii].stat) {
				this->SetStatusQue(this->GetStatusRead().DaisyaErr[ii].id, true, false);
				LOG(em_MSG), "[StatusManager] %s カメラ台車がオンライン位置で無いチェック回復", DivNameManager::GetTorB(ii));
			}
		}
	}
}

//------------------------------------------
// 操業状態チェック処理_LED照明点灯/消灯制御
//------------------------------------------
void StatusManager::SogyoStatus_LedOnOff() 
{
	bool bSate = false;							// true:LED照明装置 点灯消灯通知有り

	// カメラ調整時は無効 || 自動チェック無し
	if( DIV_UNTEN_CAMERA == m_emUnten || m_bLedAutoCheckOff) return;
	for(int ii=0; ii<NUM_MEN; ii++) {

		bSate = false;
		for (int jj = 0; jj<DIV_LED_COLOR_END; jj++) {
			if (1 == m_oldStateRead.led.ch.OnOff[ii][jj].stat) {
				bSate = true;					// 点灯中あり
			}
		}
		if (0 == m_oldStateRead.DaisyaErr[ii].stat)	m_nLedDaisyaOffLineCnt[ii] = 0;		// 台車がオンライン位置
		else if(bSate)								m_nLedDaisyaOffLineCnt[ii]  ++;		// 台車がオフライン位置

		// 通板中 or 低速操業
		if( DIV_SOGYO_RUN == m_emSogyo || DIV_SOGYO_SLOW == m_emSogyo ) {

			// 消灯 → 点灯
			m_nLedLineStopCnt[ii] = 0;			// LED照明点灯→消灯制御カウンタクリア


			bSate = false;					// 1灯でも消灯中 且つ   通板中 or 低速操業
			for(int jj=0; jj<DIV_LED_COLOR_END; jj++) {
#ifdef DISABLE_LED_DAISYA
				if (0 == m_oldStateRead.led[ii].ch[jj].OnOff.stat) {			// ★植松要望 試験中は台車オフラインのため、状態を見ない
#else
				if (0 == m_oldStateRead.led.ch.OnOff[ii][jj].stat && 0 == m_oldStateRead.DaisyaErr[ii].stat) {
#endif
					bSate = true;				// 消灯中あり
				}
			}

			// ライン稼働中でかつ台車がオンライン位置であれば、LED照明点灯
			if(bSate) {
//▲				LOG(em_MSG), "[StatusManager] %s LED照明装置 点灯指示 送信", DivNameManager::GetTorB(ii));
				ToOutMailSender::Send_KtLedMgr_OnOff(ii, TRUE);
			}

		// 休止中 or 不明
		} else {

			// 点灯 → 消灯
			bSate = false;
			for(int jj=0; jj<DIV_LED_COLOR_END; jj++) {
				if (1 == m_oldStateRead.led.ch.OnOff[ii][jj].stat) {
					bSate = true;				// 点灯中あり
				}
			}

			if(bSate) {
				m_nLedLineStopCnt[ii]++;

				int nLimitCnt = (int)(m_nLedLineStopOffTime*60*1000 / PARAM_ST_SOGYO_INTERVAL_TIME );

				if( m_nLedLineStopCnt[ii] >= nLimitCnt ) {
					LOG(em_MSG), "[StatusManager] %s LED照明装置 消灯指示 送信", DivNameManager::GetTorB(ii));
					ToOutMailSender::Send_KtLedMgr_OnOff(ii, FALSE);
					m_nLedLineStopCnt[ii] = 0;
				}
			}
		}
		// 台車がオフライン位置で一定時間経過後、LED照明を消灯
		if (0 <= m_nLedDaisyaOffLineTime) {
			// 点灯 → 消灯
			bSate = false;
			for(int jj=0; jj<DIV_LED_COLOR_END; jj++) {
				if (1 == m_oldStateRead.led.ch.OnOff[ii][jj].stat) {
					bSate = true;				// 点灯中あり
				}
			}
			if(bSate) {

				int nDaisyaOffLineCnt = (int)(m_nLedDaisyaOffLineTime * 1000 / PARAM_ST_SOGYO_INTERVAL_TIME);

				if (m_nLedDaisyaOffLineCnt[ii] >= nDaisyaOffLineCnt) {
					LOG(em_MSG), "[StatusManager] %s 台車オフライン LED照明装置 消灯指示 送信", DivNameManager::GetTorB(ii));
#ifdef DISABLE_LED_DAISYA
					// ★植松要望 試験中は台車オフラインのため、状態を見ない
#else
					ToOutMailSender::Send_KtLedMgr_OnOff(ii, FALSE);
#endif
					m_nLedDaisyaOffLineCnt[ii] = 0;
				}
			}
		}
	}
}


// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 機器状態把握

//------------------------------------------
// 機器状態取得 & 状態チェック 完了時
// bool bChange 前回と比べて値が変わったとき true
//------------------------------------------
void StatusManager::OnReadKikiStatusEnd(bool bChange)
{
	//// 現状の機器状態を検査中のコイル情報に紐付け
	CheckCoilInfStatus();

	//// 表示機能へ送信
	if(bChange) OpSendFunc::SendOp_Status();

}

// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 検査中異常把握

//------------------------------------------
// 検査中の状態把握 (自タスク内の異常把握)
// 戻り値 true時探傷停止
//------------------------------------------
bool StatusManager::OnMyErrWar(TYP_MY_ERRWAR_INF const* pInf)
{
	CString s;
	int n = pInf->nKind;

	if		(ON_QUE_COIL_INF == n)			 { s = "<CoilMgr> コイル情報登録時キューフル"; }
	else if (ON_QUE_COIL_RESULT == n)		 { s = "<CoilMgr> コイル実績登録時キューフル"; }
	else if (ON_QUE_COIL_RESULT_UPDATE == n) { s = "<CoilMgr> コイル実績アップデート時キューフル"; }
	else if (ON_SAVE_COIL_INF == n)			 { s = "<DbMgr> コイル情報登録時キューフル"; }
	else if (ON_SAVE_COIL_RESULT == n)		 { s = "<DbMgr> コイル実績登録時キューフル"; }
	else if (ON_SAVE_DEFECT_INF == n)		 { s = "<DbMgr> 疵情報登録時キューフル"; }
	else if( ON_QUE_CAMLINKSW	== n)		 { s = "<MI> カメラリンクスイッチ設定登録時キューフル"; }
	
	else if (ON_DB_ACCESS_ERR == n)			 { s = "DB接続失敗"; }
	else if (ON_SEQ_FRAME_DELAY == n)		 { s = "シーケンサフレーム受信遅れ"; }
	else if (ON_PLG_FRAME_DELETE == n)		 { s = "受信フレーム消失"; }
	else if (ON_SEQ_FRAME_SOCK_CLOSE == n)	 { s = "切断認識"; }
	else if (ON_SEQ_FRAME_SOCK_CLOSE == n)	 { s = "切断認識"; }
	else								 	 { s = "ステータス異常";}



	LOG(em_ERR), "[%s] AP異常。[異常区分:%s]", my_sThreadName, s );
	return pInf->bErr;
}







//------------------------------------------
// 現在の機器状態をコイル情報にセット
//------------------------------------------
void StatusManager::CheckCoilInfStatus()
{
	int ii;
	if(NULL == m_pCoil) return;

	//================================================
	// 一区間でもフラグのチェック
	for(ii=0; ii<NUM_MEN; ii++) {
		// 判定状態
		if( 0 != m_oldStateRead.Ht_Ans[ii].stat )		m_pCoil->nSt_Hantei[ii] = 1;
		// LED状態
		for(int jj=0; jj<NUM_LIGHT; jj++) {
			if( 0 != m_oldStateRead.led.Connect[ii][jj].stat ) m_pCoil->nSt_Light[ii] = 1;
		}
		for(int jj=0; jj<DIV_LED_COLOR_END; jj++) {
			if( 0 != m_oldStateRead.led.ch.Break[ii][jj].stat ) m_pCoil->nSt_Light[ii] = 1;
		}
		for(int jj=0; jj<NUM_LED_COLOR; jj++) {
			for (int kk = 0; kk < MAX_LED_TEMP_UNIT; kk++) {
				if (0 != m_oldStateRead.led.color.TempSt[ii][jj][kk].stat) m_pCoil->nSt_Light[ii] = 1;
			}
		}

		// スキップ
		if( 0 != m_oldStateRead.FrameSkip[ii].stat )	m_pCoil->nSt_FrameSkip[ii] = 1;
		if( 0 != m_oldStateRead.DB_SaveMax[ii].stat || 0 != m_oldStateRead.DB_SaveSkip[ii].stat )
														m_pCoil->nSt_DbSkip[ii] = 1;

		// カメラ台車状態
		if( 0 != m_oldStateRead.DaisyaErr[ii].stat )		m_pCoil->nSt_Daisya[ii] = 1;
	}

	// カメラコントローラ
	if( 0 != m_oldStateRead.CamControl.stat )			m_pCoil->nSt_CamControl = 1;
	// カメラリンクスイッチ
	if( 0 != m_oldStateRead.CamSwitch.stat )			m_pCoil->nSt_CamSwitch = 1;
	// プロコン
	if( 0 != m_oldStateRead.Procon.stat  )				m_pCoil->nSt_Procon = 1;
	// FF
	if( 0 != m_oldStateRead.FFserv.stat  )				m_pCoil->nSt_FF = 1;
	// シーケンサ
	if (0 != m_oldStateRead.DataMem_Ap.stat || 0 != m_oldStateRead.DataMem_Hb.stat)
														m_pCoil->nSt_SqeDm = 1;
	if (0 != m_oldStateRead.UdpSeq.stat) 				m_pCoil->nSt_SqeUdp = 1;
	// 塗油スプレー装置
	if( 0 != m_oldStateRead.ToyuSprSt.stat )			m_pCoil->nSt_ToyuSplr = 1;
}

//==========================================
// 検査可能な判定PCを判別する [←MainInstance]
// 表・裏が同一のGroupIDバージョン
// NUM_MEN=2の前提で作ってます
//------------------------------------------
// bool*	bOk		 : 面単位の判定結果
// 戻り値	bool	 : true:検査可能 false:判定PC足りない
//==========================================
bool StatusManager::SelectHanteiPC_SingleGroup(bool* bOk)
{
	// 取得
	int					nGetAllPcID[NUM_ALL_HANTEI];		// PCIDの昇順に、振替可能なPCID(1オリジン)
	TYP_STATUS_READ		typStatus;							// 現在の機器状態
	int					nHtCount	= 0;					// 判定PCの台数(PC_MASTER)

	// 算出
	int					nSetAllPcID[NUM_ALL_HANTEI];		// PCKINDに対するPCIDの振り直し用 (正常判定PCは前から、異常判定PCは後ろからセット)
	bool				bSetAllPcSt[NUM_ALL_HANTEI];		// nSetAllPcIDに対する正常／異常のログ出力用 (true:正常 false:異常)
	int					nOkCount    = 0;					// 正常だった数
	int					nNgCount    = 0;					// 異常だった数
	int					nOkMenCnt[NUM_MEN];					// 片面ごとの正常台数

	// 条件
	int					nNumHt		= 0;					// 片面 判定PCの台数

	//// 初期化
	memset(m_nHt_ID, 0x00, sizeof(m_nHt_ID));
	memset(nGetAllPcID, 0x00, sizeof(nGetAllPcID));
	memset(nSetAllPcID, 0x00, sizeof(nSetAllPcID));
	memset(bSetAllPcSt, 0x00, sizeof(bSetAllPcSt));
	memset(nOkMenCnt, 0x00, sizeof(nOkMenCnt));
	for(int ii=0; ii<NUM_MEN; ii++)	bOk[ii] = false;

	// 片面判定PCの台数
	nNumHt = NUM_HANTEI;

	//// 今の機器状態から検査開始可能な判定PCをさがす

	int	nBkInfCount	= 0;					// バックアップPCの台数 PC_INF
	int	nBkMstCount	= 0;					// バックアップPCの台数 PC_MASTER
	// /////////////////////////////////////////////////////////////////////////
	// 今の機器状態を取得
	mcls_pKiki->Refresh();
	mcls_pKiki->GetStatusInf((TYP_STATUS_INF*)&typStatus, sizeof(typStatus));

	// 振替可能なPCIDを取得(判定PC)
	KizuPcInfManager::GetGroupPcID(TASKINI_NAME, DIV_HANTEI01, NUM_ALL_HANTEI, &nHtCount, nGetAllPcID, true);	// 取得先はPC_MASTER

	if (NUM_HANTEI * NUM_MEN > nHtCount) {
		LOG(em_ERR), "[%s] 振替可能な判定PC数が異常=%d [DBがおかしい？]", my_sThreadName, nHtCount);
		return false;
	}

	// /////////////////////////////////////////////////////////////////////////
	// 昇順に正常な判定PCを求める(正規の判定PCが対象)
	// ここでは、表・裏共通の配列にセット
	int nNgBk	= 0;
	int	nNgBkPcID[NUM_ALL_HANTEI];						// 異常な判定PCを一時格納用
	memset(nNgBkPcID, 0x00, sizeof(nNgBkPcID));
	for(int ii=0; ii<nHtCount; ii++){
		int id = nGetAllPcID[ii] -1;

		// 判定PCとカメラの割り付けは同じとする
		// 異常な判定PCのところに、バックアップPCを割り付ける

		// 機器状態チェック
		if( !bOk[0] &&									// まだ検査に必要な台数が揃っていない　表
			nNumHt > ii &&								// 表面判定PC
			0 == typStatus.Ht_Cannot[id].stat &&		// 続行不能
			0 == typStatus.Ht_NonAnswer[id].stat &&		// 応答無し
			0 == typStatus.AppEnable_Ht[id].stat &&		// 起動
			0 == typStatus.HbSend_Ht[id].stat )		{	// ハートビート

			// 全部正常ならとりあえずその判定PCは正常とみなす。
			nSetAllPcID[ii] = nGetAllPcID[ii];
			bSetAllPcSt[ii] = true;						// 正常
			nOkMenCnt[0]++;
			if( nNumHt <= nOkMenCnt[0] ) bOk[0] = true;	// 表面の検査に必要な台数が揃った

		} else if( !bOk[1] &&							// まだ検査に必要な台数が揃っていない　裏
			nNumHt*2 > ii &&							// 裏面判定PC
			0 == typStatus.Ht_Cannot[id].stat &&		// 続行不能
			0 == typStatus.Ht_NonAnswer[id].stat &&		// 応答無し
			0 == typStatus.AppEnable_Ht[id].stat &&		// 起動
			0 == typStatus.HbSend_Ht[id].stat )		{	// ハートビート

			// 全部正常ならとりあえずその判定PCは正常とみなす。
				nSetAllPcID[ii] = nGetAllPcID[ii];					// PCIDセット
			bSetAllPcSt[ii] = true;						// 正常
			nOkMenCnt[1]++;
			if(nNumHt <= nOkMenCnt[1] )	bOk[1] = true;	// 裏面の検査に必要な台数が揃った
			

		} else if( 
			nNumHt*2 <= ii &&							// バックアップ判定PC
			0 == typStatus.Ht_Cannot[id].stat &&		// 続行不能
			0 == typStatus.Ht_NonAnswer[id].stat &&		// 応答無し
			0 == typStatus.AppEnable_Ht[id].stat &&		// 起動
			0 == typStatus.HbSend_Ht[id].stat )		{	// ハートビート

			//表面裏面のどちらに追加するか？
			int jj;
			for(jj=0; jj<(nNumHt*NUM_MEN); jj++) {
					if (0 == nSetAllPcID[jj]) {
						nSetAllPcID[jj] = nGetAllPcID[ii];			// PCIDセット
					bSetAllPcSt[jj] = true;					// 正常

					int	nMen;
					if(nNumHt > jj ) nMen = 0;			// 表面
					else			 nMen = 1;			// 裏面

					nOkMenCnt[nMen]++;
					if(nNumHt <= nOkMenCnt[nMen] )	bOk[nMen] = true;	// 片面の検査に必要な台数が揃った
						break;
					}
				}
			if(nNumHt*NUM_MEN == jj) {
				nSetAllPcID[ii] = nGetAllPcID[ii];		// PCIDセット
				bSetAllPcSt[ii] = false;				// 正常だが、バックアップPCなのでfalse
			}


			//// 全部正常ならとりあえずその判定PCは正常とみなす。
			//nSetAllPcID[ii] = nGetAllPcID[ii];			// PCIDセット
			//bSetAllPcSt[ii] = true;						// 正常


			//nOkMenCnt[2]++;

			//if(nNumHt <= nOkMenCnt[2])
			//if(nNumHt <= nOkMenCnt[2] )	bOk[1] = true;	// 裏面の検査に必要な台数が揃った
		
		}else{
			if( ! bOk[0] || !bOk[1] ){
				LOG(em_ERR), "[%s] 判定PC%02d 異常 (続行不能=%d, 起動=%d, HB=%d, 応答無し=%d)", my_sThreadName,
					nGetAllPcID[ii], typStatus.Ht_Cannot[id].stat, typStatus.AppEnable_Ht[id].stat, typStatus.HbSend_Ht[id].stat, typStatus.Ht_NonAnswer[id].stat);
			}


			// 末尾からセットしていく
			//nSetAllPcID[(nHtCount + nBkCount) -nNgCount -1] = nGetAllPcID[ii];
			//nNgCount++;
			// 判定PCとカメラの割付を保持するため、単純に末尾からセットできないため、一度ワークにセット
			nNgBkPcID[nNgBk] = nGetAllPcID[ii];
			nNgBk++;
		}
	}
	nOkCount = nOkMenCnt[0] + nOkMenCnt[1];				// 正常だったPC台数

	for(int ii=0; ii<nNgBk; ii++) {
		for(int jj=0; jj<nHtCount; jj++) {
			if(0 == nSetAllPcID[jj]) {
				// 異常判定PCとバックアップ未実施のPCは、未割り付け部にセット
				nSetAllPcID[jj] = nNgBkPcID[ii];
			nNgCount++;
				break;
			}
		}
	}


	// /////////////////////////////////////////////////////////////////////////
	// 名前を列挙
	// 表・裏別をそれぞれ用意する(ログ出力用)
	CString sOkPcIdMen[NUM_MEN];			// OK PC列挙(面単位)
	CString sNgPcIdMen;						// NG PC列挙
	CString sWk;

	//// 表・裏共通
	for(int ii=0; ii<nHtCount; ii++){
		if(0==nSetAllPcID[ii]) continue;
		sWk.Format("%d", nSetAllPcID[ii]);

		// 表・裏どちらにセットする？
		int men;
		if( nNumHt > ii )	men = 0;		// 表
		else					men = 1;		// 裏
		
		if(bSetAllPcSt[ii]) {				// 正常判定PC
			if( 0 != (strlen(sOkPcIdMen[men])) )	sOkPcIdMen[men] += ", ";
		sOkPcIdMen[men] += sWk;
		} else {							// 異常判定PC
			if( 0 != (strlen(sNgPcIdMen)) )	sNgPcIdMen += ", ";
			sNgPcIdMen += sWk;
	}
	}

	//// 検査に必要な台数があった？
	if(  !bOk[0] && !bOk[1] ){
		for(int ii=0; ii<NUM_MEN; ii++){
			LOG(em_ERR), "[%s] <%s:%s> 検査に必要な台数が足りない 正常判定台数=%d台 [表:%s][裏:%s]", my_sThreadName,
				GetHanteiKind(DIV_HANTEI01), DivNameManager::GetTorB(ii), nOkCount, sOkPcIdMen[0], sOkPcIdMen[1]);
			syslog(239, "[正常判定PC<%s>(%d台)(表:%s)(裏:%s)]", GetHanteiKind(DIV_HANTEI01), nOkCount, sOkPcIdMen[0],sOkPcIdMen[1]);
		}
		return false; 
	}

	//// DBのPCIDを更新
	//KizuPcInfManager::SetGroupPcID(TASKINI_NAME, 1, NUM_ALL_HANTEI, nSetAllPcID);		// GroupNoは1固定
	KizuPcInfManager::SetGroupPcID(TASKINI_NAME, DIV_HANTEI01, nHtCount, nSetAllPcID);	// GroupNoは1固定

	//// 自分自身は即時反映
	getpcname();
		
	//// ここからは面単位で実施
	for(int ii=0; ii<NUM_MEN; ii++){
		
		//// 設定 ※必ず設定。これによって、タイムアウト時の応答無しを検知できる。
		memcpy(&m_nHt_ID[ii], &nSetAllPcID[nNumHt*ii], sizeof(m_nHt_ID[ii]));

		// 片面検査に足りる台数ある？
		if( nNumHt*(ii+1) <= nOkCount ){
	
			////////// 設定
			//////if(DIV_HANTEI == nHtCy) memcpy(&m_nHt_ID[ii], &nSetAllPcID[nNumHt*ii], sizeof(m_nHt_ID[ii]));
			//////else					memcpy(&m_nCy_ID[ii], &nSetAllPcID[nNumHt*ii], sizeof(m_nCy_ID[ii]));
			LOG(em_MSG), "[%s] <%s:%s> 検査可能判定PC算出完了 PCID[%s] バックアップ[%s]", my_sThreadName,
				GetHanteiKind(DIV_HANTEI01), DivNameManager::GetTorB(ii), sOkPcIdMen[ii], sNgPcIdMen);
			syslog(240, "[<%s:%s> 正常判定PC(%s) バックアップ(%s)]", GetHanteiKind(DIV_HANTEI01), DivNameManager::GetTorB(ii), sOkPcIdMen[ii], sNgPcIdMen);

		// 検査に必要な台数が足りない(片面検査許可＆判定PCが足りない場合)
		}else{

			//// 設定はせず、ログ出力のみ
			LOG(em_ERR), "[%s] <%s:%s> 検査に必要な台数が足りない 正常判定台数=%d台 [%s]", my_sThreadName, 
				GetHanteiKind(DIV_HANTEI01), DivNameManager::GetTorB(ii), nOkCount - nNumHt, sOkPcIdMen[ii]);
			syslog(239, "[<%s:%s> 正常判定PC(%d台:%s)]", GetHanteiKind(DIV_HANTEI01), DivNameManager::GetTorB(ii), nOkCount-nNumHt, sOkPcIdMen[ii]);
		}
	}


	return true;


}


//==========================================
// 検査可能な判定PCを判別する [←MainInstance]
// グループID単位での振替バージョン
// ※ロジックパラランPCは除く
//------------------------------------------
// bool*	bOk		 : 面単位の判定結果
// 戻り値	bool	 : true:検査可能 false:判定PC足りない
//==========================================
bool StatusManager::SelectHanteiPC_MultiGroup(bool* bOk)
{
	CString sWk;

	// 判定PCグループ数
	int		nHT_GROUP_MAX = DIV_GROUP_MAX - 1;

	// Group毎の必要な判定PCの台数（DBから取得）
	//  [0] CameraLinkSW1(表)
	//  [1] CameraLinkSW2(表)
	//  [2] CameraLinkSW3(裏)
	//  [3] CameraLinkSW4(裏)　　
	int		nNumHt[DIV_GROUP_MAX - 1];

	// 取得
	TYP_STATUS_READ	typStatus;										// 現在の機器状態
	int				nHtCount[DIV_GROUP_MAX - 1];					// Group毎の判定PCの台数(PC_MASTERからの取得)

	// 算出
	int				nSetAllPcID[DIV_GROUP_MAX - 1][NUM_ALL_HANTEI];	// Group毎のPCKINDに対するPCIDの振り直し用 (正常判定PCは前から、異常判定PCは後ろからセット)
	bool			bSetAllPcSt[DIV_GROUP_MAX - 1][NUM_ALL_HANTEI];	// Group毎のnSetAllPcIDに対する正常／異常のログ出力用 (true:正常 false:異常)
	CString			sOkPcId[DIV_GROUP_MAX - 1];						// Group毎のOK PC列挙
	CString			sNgPcId[DIV_GROUP_MAX - 1];						// Group毎のNG PC列挙

	// 初期化
	memset(m_nHt_ID,		0x00, sizeof(m_nHt_ID));
	memset(m_nHt_Para_ID,	0x00, sizeof(m_nHt_Para_ID));
	memset(nHtCount,		0x00, sizeof(nHtCount));
	memset(nSetAllPcID,		0x00, sizeof(nSetAllPcID));
	memset(bSetAllPcSt,		0x00, sizeof(bSetAllPcSt));
	memset(nNumHt,			0x00, sizeof(nNumHt));
	for (int ii = 0; ii<NUM_MEN; ii++)	bOk[ii] = false;

	// グループ毎の必要判定PC台数を取得
	mcls_pParam->GetHtGroupNum(nNumHt);

	// 各面の判定PC台数を算出
	int nTopPCNum = 0;
	int nBotPCNum = 0;
	for (int ii = 0; ii < nHT_GROUP_MAX; ii++)
	{
		if (ii < (nHT_GROUP_MAX / 2))	nTopPCNum += nNumHt[ii];
		else							nBotPCNum += nNumHt[ii];
	}

	// 各面の判定PC台数が規定台数であるかチェック
	if ((NUM_HANTEI != nTopPCNum) || (NUM_HANTEI != nBotPCNum))
	{
		// 片面の必要台数が不一致（デバッグ時）
		// ⇒必要台数を調整
		for (int ii = 0; ii<NUM_MEN; ii++)
		{
			if (NUM_HANTEI < (nNumHt[(ii * 2)] + nNumHt[(ii * 2) + 1]))
			{
				// 表面の必要台数を調整
				int nUnit1 = 0;
				int nUnit2 = 0;
				for (int jj = 0; jj < NUM_HANTEI; jj++)
				{
					if (nNumHt[(ii * 2)] > jj)	nUnit1++;
					else						nUnit2++;
				}
				nNumHt[(ii * 2)]	= nUnit1;
				nNumHt[(ii * 2) + 1]= nUnit2;
			}
		}
	}
	sWk.Format("[%s] 判定PC必要台数", my_sThreadName);
	for (int ii = 0; ii < nHT_GROUP_MAX; ii++)
	{
		CString s;
		s.Format(" %s:<Group%d:%d>", (ii < (nHT_GROUP_MAX / 2) ? "表面" : "裏面"), ii + 1, nNumHt[ii]);
		sWk += s;
	}
	LOG(em_MSG), sWk);

	// 各リンクスイッチ(グループ毎)の先頭カメラNoをセット
	int nTopCam = 0;
	for (int ii = 0; ii < nHT_GROUP_MAX; ii++)
	{
		m_nHt_TopCamno[ii] = nTopCam;
		nTopCam += nNumHt[ii] * NUM_CAMLINK_BOARD; // １判定PCにつき
	}

	// 今の機器状態を取得
	mcls_pKiki->Refresh();
	mcls_pKiki->GetStatusInf((TYP_STATUS_INF*)&typStatus, sizeof(typStatus));

	bool	bCompleted[DIV_GROUP_MAX - 1];			// 対象グループが必要とする振替PC数に達した
	for (int ii = 0; ii<nHT_GROUP_MAX; ii++)	bCompleted[ii] = false;

	// ユニット単位(LinkSwitch)で振替を実施する
	for (int grp = 0; grp < nHT_GROUP_MAX; grp++)
	{
		int	nGrpPcID[NUM_ALL_HANTEI];							// PCIDの昇順に、振替可能なPCID(1オリジン)

		memset(nGrpPcID, 0x00, sizeof(nGrpPcID));

		// 対象グループの振替可能なPCIDを取得
		// ※取得先はPC_MASTER
		KizuPcInfManager::GetGroupPcID(TASKINI_NAME, DIV_HANTEI01 + grp, NUM_ALL_HANTEI, &nHtCount[grp], nGrpPcID, true);

		LOG(em_MSG), "[%s] <%s> 台数：%d", my_sThreadName, GetHanteiKind(DIV_HANTEI01 + grp), nHtCount[grp]);

		// LinkSwitch(ハーフ版)毎の必要PC接続台数に満たない場合はエラー
		if (nNumHt[grp] > nHtCount[grp])
		{
			LOG(em_ERR), "[%s] <%s> 振替可能な判定PC数が異常=%d [DBがおかしい？]",
				my_sThreadName, GetHanteiKind(DIV_HANTEI01 + grp), nHtCount[grp]);
			return false;
		}

		// /////////////////////////////////////////////////////////////////////////
		// 昇順に正常な判定PCを求める(正規の判定PCが対象)
		int		nNgBk = 0;									// 異常判定PC格納数
		int		nNgBkPcID[NUM_ALL_HANTEI];					// 異常判定PC一時格納バッファ
		int		nGrpOkCount = 0;							// 対象グループで振替たPC数

		memset(nNgBkPcID, 0x00, sizeof(nNgBkPcID));

		// 必要台数無し
		if (0 >= nNumHt[grp]) bCompleted[grp] = true;

		// 取得した判定PC分、機器状態を見て振替を実施する
		for (int ii = 0; ii<nHtCount[grp]; ii++)
		{
			int id = nGrpPcID[ii] - 1;

			// 正常な判定PCをPCIDの若い順に振替える
			// 判定PCとカメラの割り付けは同じとする
			// 異常な判定PCのところに、バックアップPCを割り付ける

			// 機器状態チェック
			if (!bCompleted[grp] &&							// まだ検査に必要な台数が揃っていない
				0 == typStatus.Ht_Cannot[id].stat &&		// 続行不能
				0 == typStatus.Ht_NonAnswer[id].stat &&		// 応答無し
				0 == typStatus.AppEnable_Ht[id].stat &&		// 起動
				0 == typStatus.HbSend_Ht[id].stat)			// ハートビート
			{
				if (ii < nNumHt[grp])
				{
					// 正規判定PCの割当

					// 全部正常ならとりあえずその判定PCは正常とみなす。
					nSetAllPcID[grp][ii] = nGrpPcID[ii];		// PCIDセット
					bSetAllPcSt[grp][ii] = true;				// 正常
					nGrpOkCount++;
					// 対象LinkSwitchへの接続台数が揃った
					// ※LinkSwitch毎に必要な判定PCが決まっていることを前提とする
					// 　入力カメラ接続によっては１つのLinkSwitchに判定PCを集約することも可能だが考慮しない
					// 　⇒システム構成図の構成に準ずる
					if (nNumHt[grp] <= nGrpOkCount) bCompleted[grp] = true;
				}
				else
				{
					// バックアップ機の割当

					// 必要台数の範囲内で未割当の場所はあるか？
					bool bAssign = false;
					for (int jj = 0; jj < nNumHt[grp]; jj++)
					{
						if (0 == nSetAllPcID[grp][jj])
						{
							// 割り当たっていない場所が見つかった
							nSetAllPcID[grp][jj] = nGrpPcID[ii];// PCIDセット
							bSetAllPcSt[grp][jj] = true;		// 正常
							nGrpOkCount++;
							// 対象LinkSwitchへの接続台数が揃った
							// ※LinkSwitch毎に必要な判定PCが決まっていることを前提とする
							// 　入力カメラ接続によっては１つのLinkSwitchに判定PCを集約することも可能だが考慮しない
							// 　⇒システム構成図の構成に準ずる
							if (nNumHt[grp] <= nGrpOkCount) bCompleted[grp] = true;
							bAssign = true;
							break;
						}
					}
					if (!bAssign)
					{
						// バックアップ機割付されなかった場合
						// ※必要台数に達してなく、且つバックアップ機のIndexなので
						// 　必要台数の範囲内に異常PCが存在している筈
						// 　⇒つまり基本的にはこのケースには入らない（念のため）
						nSetAllPcID[grp][ii] = nGrpPcID[ii];	// PCIDセット
						bSetAllPcSt[grp][ii] = false;			// 正常だが、バックアップPCなのでfalse
					}
				}
			}
			else if (
				0 == typStatus.Ht_Cannot[id].stat &&		// 続行不能
				0 == typStatus.Ht_NonAnswer[id].stat &&		// 応答無し
				0 == typStatus.AppEnable_Ht[id].stat &&		// 起動
				0 == typStatus.HbSend_Ht[id].stat)			// ハートビート
			{
				// 既に必要台数が割りついているので、バックアップ機としてセット
				nSetAllPcID[grp][ii] = nGrpPcID[ii];		// PCIDセット
				bSetAllPcSt[grp][ii] = false;				// 正常だが、バックアップPCなのでfalse
			}
			else
			{
				// 異常PC検出
				if (!bCompleted[grp])
				{
					LOG(em_ERR), "[%s] 判定PC%02d 異常 (続行不能=%d, 起動=%d, HB=%d, 応答無し=%d)", my_sThreadName,
						nGrpPcID[ii], typStatus.Ht_Cannot[id].stat, typStatus.AppEnable_Ht[id].stat, typStatus.HbSend_Ht[id].stat, typStatus.Ht_NonAnswer[id].stat);
				}

				// 末尾からセットしていく
				// 判定PCとカメラの割付を保持するため、単純に末尾からセットできないため、一度ワークにセット
				nNgBkPcID[nNgBk] = nGrpPcID[ii];
				nNgBk++;
			}
		}

		// PCIDのマージ
		// ＃異常判定PCとバックアップ未実施のPCを未割り付け部にセット
		for (int ii = 0; ii<nNgBk; ii++)
		{
			for (int jj = 0; jj<nHtCount[grp]; jj++)
			{
				if (0 == nSetAllPcID[grp][jj])
				{
					nSetAllPcID[grp][jj] = nNgBkPcID[ii];
					bSetAllPcSt[grp][jj] = false;			// 異常
					break;
				}
			}
		}

		// /////////////////////////////////////////////////////////////////////////
		// 名前を列挙(ログ出力用)
		//
		for (int ii = 0; ii<nHtCount[grp]; ii++)
		{
			if (0 == nSetAllPcID[grp][ii]) continue;

			sWk.Format("%d", nSetAllPcID[grp][ii]);

			if (bSetAllPcSt[grp][ii])
			{	// 正常判定PC
				if (0 != sOkPcId[grp].GetLength())	sOkPcId[grp] += ", ";
				sOkPcId[grp] += sWk;
			}
			else
			{	// 異常判定PC
				if (0 != sNgPcId[grp].GetLength())	sNgPcId[grp] += ", ";
				sNgPcId[grp] += sWk;
			}
		}

		// 検査に必要な台数があった？
		if (!bCompleted[grp])
		{
			// 割振失敗（処理中断）
			LOG(em_ERR), "[%s] <%s> 検査に必要な台数が足りない 正常判定台数=%d台 [%s]",
				my_sThreadName, GetHanteiKind(DIV_HANTEI01 + grp), nGrpOkCount, sOkPcId[grp]);
			syslog(239, "[正常判定PC<%s>(%d台) %s)]", GetHanteiKind(DIV_HANTEI01 + grp), nGrpOkCount, sOkPcId[grp]);
		}
	}

	// 呼び元に返す結果フラグをセット
	for (int ii = 0; ii<NUM_MEN; ii++)	bOk[ii] = true;
	for (int grp = 0; grp < nHT_GROUP_MAX; grp++)
	{
		// 台数が揃っていないグループが存在する場合、その面はNG
		if (!bCompleted[grp]) bOk[(grp < (nHT_GROUP_MAX / 2) ? 0 : 1)] = false;
	}

	// 両面ＮＧの場合、処理中断
	if (!bOk[0] && !bOk[1])
	{
		LOG(em_ERR), "[%s] 両面とも必要な判定台数が足りない。", my_sThreadName);
		return false;
	}
	else
	{
		if (bOk[0] && bOk[1])
		{
			LOG(em_MSG), "[%s] 両面とも判定台数ＯＫ！", my_sThreadName);
		}
		else
		{
			LOG(em_MSG), "[%s] %sのみ判定台数ＯＫ！", my_sThreadName, (bOk[0] ? "表面" : "裏面"));
		}
	}

	// DB_PC_INFのPCIDを更新
	for (int grp = 0; grp < nHT_GROUP_MAX; grp++)
	{
		KizuPcInfManager::SetGroupPcID(TASKINI_NAME, DIV_HANTEI01 + grp, nHtCount[grp], nSetAllPcID[grp]);
	}

	// 自分自身は即時反映
	getpcname();

	// 検査対象の判定PCを設定
	// ※必ず設定。これによって、タイムアウト時の応答無しを検知できる。
	int nSetIndex[NUM_MEN] = {0, 0};
	for (int grp = 0; grp < nHT_GROUP_MAX; grp++)
	{
		int nMen = ((grp < (nHT_GROUP_MAX / 2)) ? 0 : 1);

		// ＮＧの面はPCIDをセットしない
		if (!bOk[nMen]) continue;

		// PCIDセット
		memcpy(&m_nHt_ID[nMen][nSetIndex[nMen]], &nSetAllPcID[grp], sizeof(int) * nNumHt[grp]);
		nSetIndex[nMen] += nNumHt[grp];

		// 成功ログ出力
		LOG(em_MSG), "[%s] <%s> 検査可能判定PC算出完了 有効PC[%s] バックアップPC[%s]",
			my_sThreadName, GetHanteiKind(DIV_HANTEI01 + grp), sOkPcId[grp], sNgPcId[grp]);
		syslog(240, "[<%s> 正常判定PC(%s) バックアップ(%s)]", GetHanteiKind(DIV_HANTEI01 + grp), sOkPcId[grp], sNgPcId[grp]);
	}

	return true;
}

//==========================================
// 検査可能なロジックパラランPCを判別する [←MainInstance]
//------------------------------------------
// 戻り値	bool	 : true:検査可能　false:ロジックパラランPC数不足
//==========================================
bool StatusManager::SelectHanteiPC_ResumeGroup()
{
	// 取得
	TYP_STATUS_READ	typStatus;						// 現在の機器状態
	int				nHtCount;						// ロジックパラランGroupの判定PCの台数(PC_MASTERからの取得)

	// 算出
	int				nSetAllPcID[NUM_ALL_HANTEI];	// ロジックパラランGroupのPCKINDに対するPCIDの振り直し用 (正常判定PCは前から、異常判定PCは後ろからセット)
	bool			bSetAllPcSt[NUM_ALL_HANTEI];	// ロジックパラランGroupのnSetAllPcIDに対する正常／異常のログ出力用 (true:正常 false:異常)
	CString			sOkPcId;						// ロジックパラランGroupのOK PC列挙
	CString			sNgPcId;						// ロジックパラランGroupのNG PC列挙

	// 初期化
	memset(m_nHt_Para_ID, 0x00, sizeof(m_nHt_Para_ID));
	memset(nSetAllPcID, 0x00, sizeof(nSetAllPcID));
	memset(bSetAllPcSt, 0x00, sizeof(bSetAllPcSt));
	nHtCount = 0;

	// 今の機器状態を取得
	mcls_pKiki->Refresh();
	mcls_pKiki->GetStatusInf((TYP_STATUS_INF*)&typStatus, sizeof(typStatus));

	// 対象グループのPCIDリスト(1オリジン)
	int	nGrpPcID[NUM_ALL_HANTEI];
	memset(nGrpPcID, 0x00, sizeof(nGrpPcID));

	// 対象グループの振替可能なPCIDを取得
	// ※取得先はPC_MASTER
	KizuPcInfManager::GetGroupPcID(TASKINI_NAME, DIV_LOGIPARA, NUM_ALL_HANTEI, &nHtCount, nGrpPcID, true);

	// NUM_ALL_HTPARAと一致しない場合はエラーとする
	// ※ロジックパラランPCの登録は必須
	if (NUM_ALL_HTPARA > nHtCount)
	{
		LOG(em_ERR), "[%s] <%s> ロジックパラランPC定義数が異常=%d [DBがおかしい？]",
			my_sThreadName, GetHanteiKind(DIV_LOGIPARA), nHtCount);
		return false;
	}

	LOG(em_MSG), "[%s] <%s> 台数：%d", my_sThreadName, GetHanteiKind(DIV_LOGIPARA), nHtCount);
		
	// 取得したロジックパラランPC分、機器状態を見て格納する
	// ※PCの死活状態のチェックのみ実施する
	for (int ii = 0; ii < NUM_ALL_HTPARA; ii++)
	{
		if (0 == nGrpPcID[ii]) continue;

		int id = nGrpPcID[ii] - 1;

		// 正常な判定PCをPCIDの若い順に振替える
		// 判定PCとカメラの割り付けは同じとする
		// 異常な判定PCのところに、バックアップPCを割り付ける

		// 機器状態チェック
		if (0 == typStatus.Ht_Cannot[id].stat &&			// 続行不能
			0 == typStatus.Ht_NonAnswer[id].stat &&			// 応答無し
			0 == typStatus.AppEnable_Ht[id].stat &&			// 起動
			0 == typStatus.HbSend_Ht[id].stat)				// ハートビート
		{
			// 全部正常ならとりあえずその判定PCは正常とみなす。
			nSetAllPcID[ii] = nGrpPcID[ii];
			bSetAllPcSt[ii] = true;							// 正常
		}
		else
		{
			// 異常なロジックパラランPC
			LOG(em_ERR), "[%s] ロジックパラランPC%02d 異常 (続行不能=%d, 起動=%d, HB=%d, 応答無し=%d)", my_sThreadName,
				nGrpPcID[ii], typStatus.Ht_Cannot[id].stat, typStatus.AppEnable_Ht[id].stat, typStatus.HbSend_Ht[id].stat, typStatus.Ht_NonAnswer[id].stat);

				// 異常なロジックパラランPC
				nSetAllPcID[ii] = nGrpPcID[ii];				// PCIDセット
				bSetAllPcSt[ii] = false;					// 異常PCなのでfalse
		}
	}

	// /////////////////////////////////////////////////////////////////////////
	// 名前を列挙(ログ出力用)
	CString sWk;
	for (int ii = 0; ii < NUM_ALL_HTPARA; ii++)
	{
		if (0 == nSetAllPcID[ii]) continue;

		sWk.Format("%d", nSetAllPcID[ii]);

		if (bSetAllPcSt[ii])
		{	// 正常ロジックパラランPC
			if (0 != sOkPcId.GetLength())	sOkPcId += ", ";
			sOkPcId += sWk;
		}
		else
		{	// 異常ロジックパラランPC
			if (0 != sNgPcId.GetLength())	sNgPcId += ", ";
			sNgPcId += sWk;
		}
	}

	// /////////////////////////////////////////////////////////////////////////
	// ロジックパラランPCIDを格納
	// ※正常・異常関係なく先頭の2PC
	for (int ii = 0; ii < NUM_ALL_HTPARA; ii++)
	{
		m_nHt_Para_ID[ii] = nSetAllPcID[ii];
	}

	// 成功ログ出力
	LOG(em_MSG), "[%s] <%s> 検査可能判定PC算出完了 有効PC[%s] バックアップPC[%s]",
		my_sThreadName, GetHanteiKind(DIV_LOGIPARA), sOkPcId, sNgPcId);
	syslog(240, "[<%s> 正常判定PC(%s) バックアップ(%s)]", GetHanteiKind(DIV_LOGIPARA), sOkPcId, sNgPcId);

	return true;
}

//==========================================
// 指定スイッチFPGAボードIDのグループの先頭カメラNoを返す
//------------------------------------------
// 戻り値	int	 : カメラNo (1 - 20)
//==========================================
int StatusManager::GetGroupTopCamNo(int nFPGABoardNo)
{
	// スイッチFPGAボードIDからグループIDを算出（0-7 ⇒ 0-3）
	int nTargetGroupID = nFPGABoardNo / 2;

	nTargetGroupID = (3 < nTargetGroupID ? 3 : nTargetGroupID);

	return m_nHt_TopCamno[nTargetGroupID];
}