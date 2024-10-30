#include "StdAfx.h"
#include "AlarmeManager.h"

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
AlarmeManager::AlarmeManager(void) :
ThreadManager("AlarmeMgr"),
m_tId_Alarme(0),
m_bPatoManualOn(false)
{
	//// ハンドル生成
	m_evTimerExec  = CreateWaitableTimer(NULL, FALSE, NULL);

	//// 初期化
	memset(&m_NowAlarme, 0x00, sizeof(m_NowAlarme));
	//memset(&m_nSlowDown, 0x00, sizeof(m_nSlowDown));

	// 初期化
	InitializeCriticalSection(&m_csLock);								// クリティカルセクションオブジェクトを初期化

	//// 判定用データ 初期化
	m_bVoicErrCancel = false;
	m_nVoicErrNo = em_VOICE_ERR_NON;
}

//------------------------------------------
// デストラクタ
//------------------------------------------
AlarmeManager::~AlarmeManager(void)
{
	//// バッファ初期化
	std::list<PATO_INF*>::iterator		itr;					// イテレータ
	for (int ii = 0; ii<NUM_MEN; ii++) {
		for (itr = mlst_AlmBuf[ii].begin(); itr != mlst_AlmBuf[ii].end(); ) {	
			SAFE_DELETE(*itr);						// 領域開放
			itr = mlst_AlmBuf[ii].erase(itr);		// 要素削除
			continue;
		}
	}

	//// ハンドル解放
	CloseHandle(m_evTimerExec);

	//// 開放
	DeleteCriticalSection(&m_csLock);									// クリティカルセクションオブジェクトを開放
}

//------------------------------------------
// スレッド開始前処理 (スレッド実行直後(シグナルセット前)にコールバック)
//------------------------------------------
int AlarmeManager::ThreadFirst()
{
	//// 事前準備
	__int64 ts = TIME_INTERVAL_EXEC * -10000; 
	SetWaitableTimer(m_evTimerExec, (LARGE_INTEGER *)&ts, TIME_INTERVAL_EXEC, NULL, NULL, FALSE);

	//// イベント設定
	HANDLE hArray[] = { m_evTimerExec
						};
	SetEventNum(sizeof(hArray)/sizeof(HANDLE), hArray);
	return 0;
}

//------------------------------------------
// スレッド終了前処理 (終了シグナル後にコールバック)
//------------------------------------------
int AlarmeManager::ThreadLast()
{
	// 定周期停止
	CancelWaitableTimer(m_evTimerExec);
	return 0;
}

//------------------------------------------
// スレッドイベント発生
// int nEventNo イベントNo (0オリジン)
//------------------------------------------
void AlarmeManager::ThreadEvent(int nEventNo)
{
	MSG	msg;								// MSG構造体
	PeekMessage(&msg, NULL, WM_USER, WM_USER, PM_NOREMOVE);			// メッセージ作成

	////// シグナルの条件
	enum {	EV_TIMER_EXEC = 0,				// 処理実行 定周期タイマ
	};

	////// シグナル条件分け
	switch (nEventNo) {

//-----------------------------------------------------------------------------------------------
	case EV_TIMER_EXEC: 					// 処理実行 定周期タイマ
		Output_Alarme();
		//Output_Sizi();
		break;
	
//-----------------------------------------------------------------------------------------------
	default :
		// ありえない！！
		LOG(em_ERR), "[%s] ThreadEvent EventNo NG=%d", my_sThreadName, nEventNo );
		break;
	}

	// ここに来たときは、メッセージが複数個あるかもしれない
	// 従ってループでメッセージを全部吸い出す (したがって PeekMessage でないとダメ)
	int rc;
	while (rc = PeekMessage(&msg, NULL, 0, 0, PM_REMOVE) ) {
		switch( msg.message ) {

//-----------------------------------------------------------------------------------------------
		case WM_TIMER:								// 欠陥検出アラーム
			// 欠陥検出 ボイス出力停止
			SAFE_TIMER_KILL(m_tId_Alarme);
			break;
		}
	}
}



// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 出力

//------------------------------------------
// 疵接近 警報出力
//------------------------------------------
void AlarmeManager::Output_Alarme()
{
	ALARME_OUTPUT	Output;
	memset(&Output, 0x00, sizeof(ALARME_OUTPUT));

	//// 今回の状態を取得
	Check_Alarme(&Output);

	// 検査開始1本目はパトライト、疵接近ランプ、欠陥ボイスは出力しない
	if (0 != mcls_pStatus->GetStatusRead().Coil_Init.stat ||
		0 != mcls_pStatus->GetStatusRead().Coil_ReStart.stat) {
		memset(Output.ranp, em_RAMP_STAT_OFF, sizeof(Output.ranp));
		memset(Output.alarm, em_RAMP_STAT_OFF, sizeof(Output.alarm));
		memset(Output.pato, 0x00, sizeof(Output.pato));
		for(int ii=OUTPUT_VOICE_ERR_NUM-1; ii<OUTPUT_VOICE_NUM;ii++) {
			Output.voice[ii] = em_VOICE_STAT_OFF;
		}
	}

	//// 警報出力の状態に変化があれば、表示管理とシーケンサ（DM経由）に通知
	// 疵接近ランプ
	bool bEmargOut = false;
	if (0 != memcmp(&m_NowAlarme.ranp, &Output.ranp, sizeof(m_NowAlarme.ranp)))
	{
		LOGA(em_MSG), "[%s] 疵接近ランプ状態変化検知", my_sThreadName);

		bEmargOut = true;

		// シーケンサ
		DmemManager::DELIVERY* pDeli = new DmemManager::DELIVERY;
		pDeli->data = new DmemManager::OCCUR_RAMP;
		DmemManager::OCCUR_RAMP *pData = (DmemManager::OCCUR_RAMP*)pDeli->data;
		pDeli->emNo = DmemManager::em_RAMP;
		memcpy(pData, &Output.ranp, sizeof(DmemManager::OCCUR_RAMP));

		// 登録
		if (!mque_pAddDmData->AddToTailFreeCheck(pDeli, 0)) {		// 登録
			LOGA(em_WAR), "[%s] DM警報情報 キューフル(疵接近ランプ)", my_sThreadName);
			LOGG(em_WAR), "[%s] DM警報情報 キューフル(疵接近ランプ)", my_sThreadName);
			delete pDeli->data;
			delete pDeli;
		}
	}
	// 疵接近アラーム
	if (0 != memcmp(&m_NowAlarme.alarm, &Output.alarm, sizeof(m_NowAlarme.alarm)))
	{
		LOGA(em_MSG), "[%s] 疵接近アラーム状態変化検知", my_sThreadName);

		bEmargOut = true;

		// シーケンサ
		DmemManager::DELIVERY* pDeli = new DmemManager::DELIVERY;
		pDeli->data = new DmemManager::OCCUR_ALARM;
		DmemManager::OCCUR_ALARM *pData = (DmemManager::OCCUR_ALARM*)pDeli->data;
		pDeli->emNo = DmemManager::em_ALARM;
		memset(pData, em_RAMP_STAT_OFF, sizeof(DmemManager::OCCUR_ALARM));

		// ８列から４列に変換
		// ※裏面のみ
		for (int ii = 0; ii < OUTPUT_ALARM_COL; ii++)
		{
			// 軽疵
			if (em_RAMP_STAT_OFF != Output.alarm[1][(ii * 2)][0] ||
				em_RAMP_STAT_OFF != Output.alarm[1][(ii * 2) + 1][0])
			{
				pData->alarm[ii][0] = em_RAMP_STAT_ON;
			}
			// 重疵
			if (em_RAMP_STAT_OFF != Output.alarm[1][(ii * 2)][1] ||
				em_RAMP_STAT_OFF != Output.alarm[1][(ii * 2) + 1][1])
			{
				pData->alarm[ii][1] = em_RAMP_STAT_ON;
			}
		}
		// ８列の情報も保持する（表示用）
		for (int ii = 0; ii < OUTPUT_MESYU_SEKKIN_COL; ii++)
		{
			for (int jj = 0; jj < OUTPUT_RAMP_NUM; jj++)
			{
				pData->alarmPos[ii][jj] = Output.alarm[1][ii][jj];
			}
		}
		//LOG(em_MSG), "[%s] アラーム状態変更[1:%d/%d 2:%d/%d 3:%d/%d 4:%d/%d]", my_sThreadName,
		//	pData->alarm[0][0], pData->alarm[0][1], pData->alarm[1][0], pData->alarm[1][1],
		//	pData->alarm[2][0], pData->alarm[2][1], pData->alarm[3][0], pData->alarm[3][1]);

		// 登録
		if (!mque_pAddDmData->AddToTailFreeCheck(pDeli, 0)) {		// 登録
			LOGA(em_WAR), "[%s] DM警報情報 キューフル(疵接近アラーム)", my_sThreadName);
			LOGG(em_WAR), "[%s] DM警報情報 キューフル(疵接近アラーム)", my_sThreadName);
			delete pDeli->data;
			delete pDeli;
		}
	}
	// 疵接近ランプ、アラームの状態変化あり
	if (bEmargOut)
	{
		//// 表示管理
		OpSendFunc::SendOp_AlarmData(&Output);
	}
	// パトライト
	if (0 != memcmp(&m_NowAlarme.pato, &Output.pato, sizeof(DmemManager::OCCUR_PATO)))
	{
		LOGA(em_MSG), "[%s] パトライト状態変化検知", my_sThreadName);

		//// シーケンサ
		DmemManager::DELIVERY* pDeli = new DmemManager::DELIVERY;
		pDeli->data = new DmemManager::OCCUR_PATO;
		DmemManager::OCCUR_PATO *pData = (DmemManager::OCCUR_PATO*)pDeli->data;
		pDeli->emNo = DmemManager::em_PATO;
		memcpy(pData, &Output.pato, sizeof(DmemManager::OCCUR_PATO));
		//LOG(em_MSG), "[%s] パトライト個数変更[保有個数=%d/%d/%d/%d]", my_sThreadName, pData->pato[0][0], pData->pato[0][1], pData->pato[1][0], pData->pato[1][1]);

		// 登録
		if (!mque_pAddDmData->AddToTailFreeCheck(pDeli, 0)) {		// 登録
			LOGA(em_WAR), "[%s] DM警報情報 キューフル(パトライト)", my_sThreadName);
			LOGG(em_WAR), "[%s] DM警報情報 キューフル(パトライト)", my_sThreadName);
			delete pDeli->data;
			delete pDeli;
		}
	}
	// ボイス
	// ※常にボイスの要求を送信し、DmemManager側で送信中の抑制を実施する
	int id;
	for (id = 0; id < OUTPUT_VOICE_NUM; id++)
	{
		if (Output.voice[id]) break;
	}
	
	// 何らかのボイス出力あり
	if (OUTPUT_VOICE_NUM != id)
	{
		//// シーケンサ
		DmemManager::DELIVERY* pDeli = new DmemManager::DELIVERY;
		pDeli->data = new DmemManager::OCCUR_VOICE;
		DmemManager::OCCUR_VOICE *pData = (DmemManager::OCCUR_VOICE*)pDeli->data;
		pDeli->emNo = DmemManager::em_VOICE;
		memcpy(pData, &Output.voice, sizeof(DmemManager::OCCUR_VOICE));

		// 登録
		if (!mque_pAddDmData->AddToTailFreeCheck(pDeli, 0)) {		// 登録
			LOGA(em_WAR), "[%s] DM警報情報 キューフル(ボイス)", my_sThreadName);
			LOGG(em_WAR), "[%s] DM警報情報 キューフル(ボイス)", my_sThreadName);
			delete pDeli->data;
			delete pDeli;
		}
	}

	//// 最終データセット
	memcpy(&m_NowAlarme, &Output, sizeof(ALARME_OUTPUT));
}

// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 警報出力


//------------------------------------------
// 疵接近 警報出力判定
// ALARME_OUTPUT*	p	疵接近ランプの状態
//------------------------------------------
void AlarmeManager::Check_Alarme(ALARME_OUTPUT*	p)
{
	int rec, row;
	int sIndex, eIndex;								// 仮想マップの対象開始・終了 相対位置 (100が各表示位置 0:下流側 200:上流側)

	TYP_VirtualMap	map;						// 表示位置（検査台１）での仮想マップ (前後100mと自分の位置)
	TYP_VirtualMap	map2;						// 表示位置（検査台２）での仮想マップ (前後100mと自分の位置)

    int				ranp[NUM_MEN][OUTPUT_MESYU_SEKKIN_COL];				// 疵接近ランプの状態
	int				alarm[NUM_MEN][OUTPUT_MESYU_SEKKIN_COL];			// 疵接近アラームの状態
	int				pato[OUTPUT_PATO_POS_NUM][OUTPUT_PATO_KIND_NUM];	// パトライトの状態
	int				voice[OUTPUT_VOICE_NUM];							// ボイスの状態
	int				voicedef[NUM_MEN];									// 欠陥検出ボイスの状態


	////======================================================
	//// 前準備
	//if( ! mcls_pStatus->IsRun() ) return;		// 検査停止中は警報出力なし
	//if( DIV_UNTEN_NOMAL != mcls_pStatus->GetUnten() ) return;

	// メイン制御が処理中の場合は、待ち
	mcls_pLock->BlockTh();


	// 初期化
	const ParamManager::P_COMMON_MAST& prm = mcls_pParam->GetCommonMast();
	memset(ranp,	em_RAMP_NON,		sizeof(ranp));
	memset(alarm,	em_RAMP_NON,		sizeof(alarm));
	memset(pato,	0x00,				sizeof(pato));
	memset(voice,	em_VOICE_STAT_OFF,	sizeof(voice));
	memset(voicedef,em_VOICE_ERR_NON,	sizeof(voicedef));

	// 仮想MAP作成
	 Create_VirtualMap(DIV_DISP_POS_DSP_P1, &map, VIRTUALROW, OUTPUT_MESYU_SEKKIN_ROW, OUTPUT_MESYU_SEKKIN_MAX);
	 Create_VirtualMap(DIV_DISP_POS_DSP_P2, &map2, VIRTUALROW, OUTPUT_MESYU_SEKKIN_ROW, OUTPUT_MESYU_SEKKIN_MAX);

	 // 出力チェック
	 // 設備異常ボイス出力チェック
	 Check_VoiceErr();

	 // 即時表示用 周期疵 検出 関連チェック
	 Check_DetectSekkin();

	//======================================================
	// 警報出力データ 作成
	//// 
	// --------------------
	// ランプ
	//// 範囲内を検索して 疵接近対象があるか確認
	 if (0 != prm.nKizuLamp[0] || 0 != prm.nKizuLamp[1])
	 {																				// 出力無し
		 sIndex = VIRTUALROW - (prm.nKizuLamp[1] / OUTPUT_MESYU_SEKKIN_ROW);		// 検査台の何m後まで出力？
		 eIndex = VIRTUALROW + (prm.nKizuLamp[0] / OUTPUT_MESYU_SEKKIN_ROW);		// 検査台の何m手前から出力？

		 for (int men = 0; men < NUM_MEN; men++)
		 {
			 for (int idx = sIndex; idx <= eIndex; idx++)
			 {
				 // データチェック（検査台１）
				 rec = map.ary[idx].rec;
				 row = map.ary[idx].row;
				 if (-1 == rec || -1 == row)		continue;		// 対象データなし

				 COIL_INF const* pCoil = mcls_pCoil->GetCoilRec(rec);
				 if (NULL == pCoil)				continue;							// コイル未到達
				 if (pCoil->bAlarmCancel)		continue;							// 警報キャンセル時
				 if (pCoil->bCoilInit)          continue;							// 検査開始１本目時
				 if (!Check_NoLamp(rec, row, OUTPUT_MESYU_SEKKIN_ROW))  continue;	// 溶接点前後の不感帯

				 for (int col = 0; col < OUTPUT_MESYU_SEKKIN_COL; col++)
				 {
					 if (!pCoil->alarme.ranp[men][row][col].bUmu) continue;			// データ無し

					 // ランプ点灯
					 if (ranp[men][col] < pCoil->alarme.ranp[men][row][col].nKind)
					 {
						 //LOG(em_INF), "[%s] 疵接近ランプ検出 [%s 疵No:%d 面:%d 疵種:%d 長手距離:%d 警報種類:%d]", my_sThreadName,
							// pCoil->data.cCoilNoIn,
							// pCoil->alarme.ranp[men][row][col].nKizuNo,
							// pCoil->alarme.ranp[men][row][col].nMen,
							// pCoil->alarme.ranp[men][row][col].nTid,
							// pCoil->alarme.ranp[men][row][col].nLen,
							// pCoil->alarme.ranp[men][row][col].nKind);
						 ranp[men][col] = pCoil->alarme.ranp[men][row][col].nKind;	// 対象区間内にランプ点灯疵が1こ以上有り
					 }
				 }
			 }
		 }
	 }

	// アラーム
	//// 範囲内を検索して 疵接近対象があるか確認
	 if (0 != prm.nKizuAlarm[0] || 0 != prm.nKizuAlarm[1])
	 {																				// 出力無し
		 sIndex = VIRTUALROW - (prm.nKizuAlarm[1] / OUTPUT_MESYU_SEKKIN_ROW);		// 検査台の何m後まで出力？
		 eIndex = VIRTUALROW + (prm.nKizuAlarm[0] / OUTPUT_MESYU_SEKKIN_ROW);		// 検査台の何m手前から出力？

		 for (int men = 0; men < NUM_MEN; men++)
		 {
			 for (int idx = sIndex; idx <= eIndex; idx++)
			 {
				 // データチェック（検査台２）
				 rec = map2.ary[idx].rec;
				 row = map2.ary[idx].row;
				 if (-1 == rec || -1 == row)	continue;							// 対象データなし

				 COIL_INF const* pCoil = mcls_pCoil->GetCoilRec(rec);
				 if (NULL == pCoil)				continue;							// コイル未到達
				 if (pCoil->bAlarmCancel)		continue;							// 警報キャンセル時
				 if (pCoil->bCoilInit)          continue;							// 検査開始１本目時
				 if (!Check_NoAlarm(rec, row, OUTPUT_MESYU_SEKKIN_ROW))  continue;	// 溶接点前後の不感帯

				 for (int col = 0; col < OUTPUT_MESYU_SEKKIN_COL; col++)
				 {
					 if (!pCoil->alarme.ranp[men][row][col].bUmu) continue;			// データ無し

																					// ランプ点灯
					 if (alarm[men][col] < pCoil->alarme.ranp[men][row][col].nKind)
					 {
						 //LOG(em_INF), "[%s] 疵接近アラーム検出 [%s 疵No:%d 面:%d 疵種:%d 長手距離:%d 警報種類:%d]", my_sThreadName,
							// pCoil->data.cCoilNoIn,
							// pCoil->alarme.ranp[men][row][col].nMen,
							// pCoil->alarme.ranp[men][row][col].nTid,
							// pCoil->alarme.ranp[men][row][col].nLen,
							// pCoil->alarme.ranp[men][row][col].nKind);
						alarm[men][col] = pCoil->alarme.ranp[men][row][col].nKind;	// 対象区間内にランプ点灯疵が1こ以上有り
					 }
				 }
			 }
		 }
	 }

	// --------------------
	// パトライト
	int nTotalPatoCnt = 0;								// パトライト出力中件数
	std::list<PATO_INF*>::iterator		itr;			// イテレータ
	for(int men =0; men<NUM_MEN; men++) {
		eIndex = VIRTUALROW + (prm.nKizuLamp[0] / OUTPUT_MESYU_SEKKIN_ROW);		// 検査台の何m手前から出力？
		rec = map.ary[eIndex].rec;
		row = map.ary[eIndex].row;
		if (-1 == rec || -1 == row)		continue;		// 対象データなし

		COIL_INF const* pCoil = mcls_pCoil->GetCoilRec(rec);
		if (NULL == pCoil)				continue;		// コイル未到達
		if (pCoil->bAlarmCancel)		continue;		// 警報キャンセル時
		if (pCoil->bCoilInit)			continue;		// 検査開始１本目時
		//if (!Check_NoAlarm(rec, row, OUTPUT_MESYU_SEKKIN_ROW))  continue;		// 溶接点前後の不感帯はリストに追加時にチェックする

		int nPatoCnt = 0;
		for( itr = mlst_AlmBuf[men].begin(); itr != mlst_AlmBuf[men].end(); itr++ ) {
			PATO_INF* pInf = *itr;						// 使いやすいように
			if(pInf == NULL)			continue;		// 絶対に起こらないはずだが念のため

			// より古い？
			int nAns = memcmp(pCoil->cKizukenNo, pInf->cKizukenNo, SIZE_KIZUKEN_NO);
			if( ( 0 < nAns) || ( 0== nAns && row >= pInf->rec) ) {
				nTotalPatoCnt++;
				nPatoCnt++;
				pInf->bRun = true;
			}
		}
		pato[men][0] = pato[men][1] = nPatoCnt;
	}
	m_nPato = nTotalPatoCnt;

	// 強制パトライトON
	if (m_bPatoManualOn) {
		if (0 == m_nPato) {	// パトライトがOFFのときのみ実施
			m_nPato = 1;
			pato[0][em_PATO_KIND_BUZZ] = pato[1][em_PATO_KIND_BUZZ] = m_nPato;
		}
	}

	// --------------------
	// 欠陥検出ボイス
	//// 範囲内を検索して 疵接近対象があるか確認
	if (0 != prm.nVoicePos) {
		eIndex = VIRTUALROW + (prm.nVoicePos / OUTPUT_MESYU_SEKKIN_ROW);		// 検査台の何m手前から出力？

		for (int men = 0; men<NUM_MEN; men++) {
			// データチェック
			rec = map.ary[eIndex].rec;
			row = map.ary[eIndex].row;
			if (-1 == rec || -1 == row)		continue;		// 対象データなし

			COIL_INF const* pCoil = mcls_pCoil->GetCoilRec(rec);
			if (NULL == pCoil)				continue;		// コイル未到達
			if (pCoil->bAlarmCancel)		continue;		// 警報キャンセル時
			if (pCoil->bCoilInit)			continue;		// 検査開始１本目時
			if (!Check_NoLamp(rec, row, OUTPUT_MESYU_SEKKIN_ROW))  continue;		// 溶接点前後の不感帯

			if (pCoil->alarme.voice[men][row].bUmu) {
				// 範囲内の最も優先度が高いボイスを出力
				if (GetVoicePriType((ENUM_VOICE_ERR_TYPE)voicedef[men])
					< GetVoicePriType((ENUM_VOICE_ERR_TYPE)pCoil->alarme.voice[men][row].nKind)) {
					voicedef[men] = pCoil->alarme.voice[men][row].nKind;
					double dLen = mcls_pPlg->GetLen_All(DIV_DISP_POS_DSP_P1);
					LOGA(em_MSG), "[%s] 欠陥検出ボイス候補 [men:%d id:%d len:%.0f]", my_sThreadName, men, voicedef[men], dLen);
				}
			}
		}
	}
	//======================================================
	// データセット
	// --------------------
	// ランプ
	for (int men = 0; men < NUM_MEN; men++) {
		for (int col = 0; col < OUTPUT_MESYU_SEKKIN_COL; col++) {
			int type = ranp[men][col];
			if(em_RAMP_NON == type) continue;
			p->ranp[men][col][type] = em_RAMP_STAT_ON;
		}
	}
	// アラーム
	for (int men = 0; men < NUM_MEN; men++) {
		for (int col = 0; col < OUTPUT_MESYU_SEKKIN_COL; col++) {
			int type = alarm[men][col];
			if (em_RAMP_NON == type) continue;
			p->alarm[men][col][type] = em_RAMP_STAT_ON;
		}
	}

	// --------------------
	// パトライト
	memcpy(p->pato, pato, sizeof(p->pato));

	// --------------------
	// ボイス
	// 通常運転モードのみ共有メモリにセット
	if (DIV_UNTEN_NOMAL == mcls_pStatus->GetUnten()) {
		//// 設備異常ボイス
		if (em_VOICE_ERR_NON != m_nVoicErrNo) {
			voice[m_nVoicErrNo] = em_VOICE_STAT_ON;
			//LOGA(em_MSG), "[%s] 設備異常ボイス出力設定 [id:%d]", my_sThreadName, m_nVoicErrNo);
		}
		//// 欠陥検出ボイス
		////else if (em_VOICE_ERR_NON != m_nVoicDetectNo) {
		////	int men = 0 == m_nVoicDetectTorB ? em_VOICE_DEF_TOP : em_VOICE_DEF_BOT;
		////	int id = GetVoiceDefType((ENUM_VOICE_PRI_TYPE)m_nVoicDetectNo);
		////	voice[men] = em_VOICE_STAT_ON;
		////	voice[id] = em_VOICE_STAT_ON;
		////}
		else if (0 != m_nVoicDetectNo && (em_VOICE_ERR_NON != voicedef[0] || em_VOICE_ERR_NON != voicedef[1])) {
			int men = GetVoicePriType((ENUM_VOICE_ERR_TYPE)voicedef[0]) >= GetVoicePriType((ENUM_VOICE_ERR_TYPE)voicedef[1]) ? em_VOICE_DEF_TOP : em_VOICE_DEF_BOT;
			int id = max(voicedef[0], voicedef[1]);
			voice[men]	= em_VOICE_STAT_ON;
			voice[id]	= em_VOICE_STAT_ON;
			LOGA(em_MSG), "[%s] 欠陥検出ボイス出力設定 [men:%d id:%d]", my_sThreadName, men - em_VOICE_DEF_TOP, id);
		}
		else
		{
			//voice[em_VOICE_ERR_NON] = em_VOICE_STAT_ON;
		}

		memcpy(&p->voice, &voice, sizeof(p->voice));
	}
}

// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// パトライト

//------------------------------------------
// 初期化
//------------------------------------------
void AlarmeManager::Init_Pato()
{
	AutoLock autolock(&m_csLock);

	// メイン制御が処理中の場合は、待ち
	mcls_pLock->BlockTh();


	//// バッファ初期化
	std::list<PATO_INF*>::iterator		itr;					// イテレータ
	for (int ii = 0; ii<NUM_MEN; ii++) {
		for (itr = mlst_AlmBuf[ii].begin(); itr != mlst_AlmBuf[ii].end(); ) {
			//if( 0==(*itr)) {			// 特定の要素だけならこのIF文を有効化する	
			if ((*itr) == NULL)			continue;		// 絶対に起こらないはずだが念のため		

			OpSendFunc::SendOp_PatoReset((*itr)->cKizukenNo, (*itr)->nMen, (*itr)->nKizuSeq);	// 強調枠のキャンセルのみ
			SAFE_DELETE(*itr);						// 領域開放
			itr = mlst_AlmBuf[ii].erase(itr);		// 要素削除
			continue;
			//}
			//itr++;
		}
	}

	////// 異常解除
	int id[2];
	id[0] = mcls_pStatus->GetStatusRead().PatoNg[0].id;
	id[1] = mcls_pStatus->GetStatusRead().PatoNg[1].id;
	mcls_pStatus->SetStatusQue(id[0], true, false);
	mcls_pStatus->SetStatusQue(id[1], true, false);
#ifdef DEBUG_PATO_LOG
	LOGA(em_MSG), "[%s] ﾊﾟﾄﾗｲﾄ疵初期化", my_sThreadName);
	LOGG(em_MSG), "[%s] ﾊﾟﾄﾗｲﾄ疵初期化", my_sThreadName );
#endif
}

//------------------------------------------
// パトライト疵 追加 & F部不感帯チェック (←HYRecv)
// char const* cKizukenNo 疵検管理No
// int nMen 面
// int nKizuSeq 疵連番
// long y 疵中心の長手位置 [mm]
// 戻り値 ： 表示へパトライト疵通知可否
//------------------------------------------
bool AlarmeManager::Add_Pato(char const* cKizukenNo, int nMen, int nKizuSeq, long y)
{
	AutoLock autolock(&m_csLock);

	// メイン制御が処理中の場合は、待ち
	mcls_pLock->BlockTh();

	int rec = y / OUTPUT_MESYU_SEKKIN_ROW;		// 500mm間隔のレコード (警報出力用の内部メッシュの配列位置)


	//================================================
	// 不感帯チェック (ここでは、F部しかできない。T部かコイル長確定時に行う)
	const ParamManager::P_COMMON_MAST& prm = mcls_pParam->GetCommonMast();
	// 先端部の不感帯に入ってる
	if (prm.nNoLamp[0] > y) {
		// ▲		LOG(em_WAR), "[%s] パトライト対象疵 不感帯(F)。破棄[%s,表裏=%d,疵連番=%d,%0.3fm]", my_sThreadName, cKizukenNo, nMen, nKizuSeq, y/1000.0);
		return false;
	}

	//================================================
	// 登録
	int nCnt = (int)mlst_AlmBuf[nMen].size();

	if (nCnt < MAX_DEFLIST_BUF) {
		// リストに登録
		PATO_INF*	pInf = new PATO_INF;
		memcpy(pInf->cKizukenNo, cKizukenNo, SIZE_KIZUKEN_NO);
		pInf->nMen = nMen;
		pInf->nKizuSeq = nKizuSeq;
		pInf->rec = rec;
		pInf->nY = y;
		pInf->bRun = false;
		mlst_AlmBuf[nMen].push_back(pInf);
#ifdef DEBUG_PATO_LOG
		LOGA(em_MSG), "[%s] ﾊﾟﾄﾗｲﾄ疵追加 (%s)(men=%d)(KizuSeq=%d)(Y=%d)", my_sThreadName, pInf->cKizukenNo, pInf->nMen, pInf->nKizuSeq, pInf->nY );
		LOGG(em_MSG), "[%s] ﾊﾟﾄﾗｲﾄ疵追加 (%s)(men=%d)(KizuSeq=%d)(Y=%d)", my_sThreadName, pInf->cKizukenNo, pInf->nMen, pInf->nKizuSeq, pInf->nY );
#endif
		return true;

	}
	else {
		// バッファフル (空き有り→無し変化時)
		// ▲		LOG(em_WAR), "[%s] パトライト対象疵キューフル。破棄[%s,表裏=%d,疵連番=%d,%0.3fm]", my_sThreadName, cKizukenNo, nMen, nKizuSeq, y/1000.0);
		if (1 != mcls_pStatus->GetStatusRead().PatoNg[nMen].stat) {
			// ステータス更新
			int id = mcls_pStatus->GetStatusRead().PatoNg[nMen].id;
			mcls_pStatus->SetStatusQue(id, false);
#ifdef DEBUG_PATO_LOG
			LOGA(em_ERR), "[%s] ﾊﾟﾄﾗｲﾄ疵追加キューフル (%s)(men=%d)(KizuSeq=%d)(Y=%d)", my_sThreadName, cKizukenNo, nMen, nKizuSeq, y  );
			LOGG(em_ERR), "[%s] ﾊﾟﾄﾗｲﾄ疵追加キューフル (%s)(men=%d)(KizuSeq=%d)(Y=%d)", my_sThreadName, cKizukenNo, nMen, nKizuSeq, y  );
#endif
		}
//#ifdef DEBUG_PATO_LOG
//		LOG(em_MSG), "ﾊﾟﾄﾗｲﾄ疵追加キューフル (%s)(men=%d)(KizuSeq=%d)", cKizukenNo, nMen, nKizuSeq );
//		LOGG(em_MSG), "ﾊﾟﾄﾗｲﾄ疵追加キューフル (%s)(men=%d)(KizuSeq=%d)", cKizukenNo, nMen, nKizuSeq );
//#endif
		return false;
	}
}



// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 共通処理

//------------------------------------------
// 表示位置を基点として、前後100mのマップを作る
// int pos 各表示位置インデックス (0オリジン)
// TYP_VirtualMap& map[201] のマップ
// int nMapMax VirtualMapの最大行数の半分 (実際は、VIRTUALROW*2+1 となる)
// int nMesyu メッシュ幅 [mm]
// int nMax	最大コイル行数
//------------------------------------------
void AlarmeManager::Create_VirtualMap(int pos, TYP_VirtualMap* map, int nMapMax, int nMesyu, int nRowMax)
{
	int ii;

	// まずは初期化
	for(ii=0; ii<(nMapMax*2+1); ii++) {
		map->ary[ii].rec = -1;
		map->ary[ii].row = -1;
	}

	//======================================================
	// 整合性チェック
	const COIL_INF*  pCoilBase = mcls_pCoil->GetCoilAll(pos);
	double	dLen = mcls_pPlg->GetLen_All(pos) ;
	if( NULL == pCoilBase ) return;					// 各表示位置に未到達時は、これで終了
	if( dLen >= COIL_LEN_MAX*1000.0 ) return;		// 上限越えは、トラッキングが怪しいので対象外にしておく。

	//// 必ず前後1コしかコイルを跨がない為 3レコードしかありえない
	int nRec[3];					// 各位置のレコード    0:1個前(下流側)  1:表示基点のコイル  2:次(上流側) 
	const COIL_INF*  pCoil[3];		// 各位置のコイル情報
	// 表示基点のコイル
	pCoil[1] = pCoilBase;
	nRec[1] = pCoil[1]->rec;				
	// 前後のコイル
	nRec[0] = mcls_pCoil->GetRecIndex(nRec[1], 1);
	nRec[2] = mcls_pCoil->GetRecIndex(nRec[1], -1);
	pCoil[0] = mcls_pCoil->GetCoilRec(nRec[0]);
	pCoil[2] = mcls_pCoil->GetCoilRec(nRec[2]);

	int nRow[2];				// 各コイルの最大行数
	nRow[0] = pCoil[0]->nEndMode != 0 ? pCoil[0]->nLength / nMesyu : nRowMax;
	nRow[1] = pCoil[1]->nEndMode != 0 ? pCoil[1]->nLength / nMesyu : nRowMax;


	//// 各表示位置を通過している配列位置を算出
	int maprow;
	int valRec;
	int valRow;
	int row = (int)(dLen / nMesyu);				// 各表示位置を0として何マス分進んでいるか (0オリジン)
	for(ii=nMapMax; ii>=-nMapMax; ii--) {								// 上流[200]から下流[0]に向かって順にセットしていく
		valRec = nRec[1];
		valRow = row+ii;

		// 1個前のコイル内
		if( valRow < 0 ) {	
			valRec = nRec[0];
			valRow = nRow[0] + (valRow+1);		// valRow=-1のときは、前コイルの一番最後。つまり、nRow[0]から-0目の配列位置となる
			if(valRow < 0) break;				// 前の前まで行っちゃった (ありえない)	

		// 次コイル内
		} else if( valRow > nRow[1] ) {
			valRec = nRec[2];
			valRow = (valRow-1) - nRow[1];		// nRow[1]=500,valRow=501の時は、次コイルの先頭。つまり、nRow[2]の0番目の配列となる
			if(valRow < 0) break;			
		
		// 現コイル内
		} else {
			// 何もしない
		}

		// データセット
		maprow = nMapMax + ii;
		map->ary[maprow].rec = valRec;
		map->ary[maprow].row = valRow;

		if (nRowMax < map->ary[maprow].row) map->ary[maprow].row = nRowMax;
	}
}


//------------------------------------------
// 不感帯範囲内かチェック
// int rec チェックするコイル情報レコード
// int row チェックする行位置 (0オリジン)
// int nMesyu メッシュ幅 [mm]
// 戻り値 false:不感帯(警報禁止) true:通常
//------------------------------------------
bool AlarmeManager::Check_NoLamp(int rec, int row, int nMesyu)
{
	const ParamManager::P_COMMON_MAST& prm = mcls_pParam->GetCommonMast();

	// 先端部の不感帯に入ってる
	if ((prm.nNoLamp[0] / nMesyu) > row) return false;

	// 尾端
	const COIL_INF*  pCoil = mcls_pCoil->GetCoilRec(rec);
	if (0 == pCoil->nEndMode) {		// まだ通板中
		return true;
	}
	else {
		int nRowEnd = (pCoil->nLength / nMesyu);
		// 尾端部の不感帯に入ってる
		if (nRowEnd - (prm.nNoLamp[1] / nMesyu) < row) return false;
	}
	return true;
}
bool AlarmeManager::Check_NoAlarm(int rec, int row, int nMesyu)
{
	const ParamManager::P_COMMON_MAST& prm = mcls_pParam->GetCommonMast();

	// 先端部の不感帯に入ってる
	if ((prm.nNoAlarm[0] / nMesyu) > row) return false;

	// 尾端
	const COIL_INF*  pCoil = mcls_pCoil->GetCoilRec(rec);
	if (0 == pCoil->nEndMode) {		// まだ通板中
		return true;
	}
	else {
		int nRowEnd = (pCoil->nLength / nMesyu);
		// 尾端部の不感帯に入ってる
		if (nRowEnd - (prm.nNoAlarm[1] / nMesyu) < row) return false;
	}
	return true;
}

//------------------------------------------
// 欠陥検出警報解除 (← MI)
// char const* cKizukenNo 表面検査台 通板中のコイル
//------------------------------------------
void AlarmeManager::AlarmCancel_Pato(char const* cKizukenNo)
{
	AutoLock autolock(&m_csLock);

	// メイン制御が処理中の場合は、待ち
	mcls_pLock->BlockTh();

	//// 対象コイルから前はすべてキャンセル
	// サーチ
	std::list<PATO_INF*>::iterator		itr;					// イテレータ
	for (int ii = 0; ii<NUM_MEN; ii++) {
		for (itr = mlst_AlmBuf[ii].begin(); itr != mlst_AlmBuf[ii].end(); ) {
			PATO_INF* pInf = *itr;						// 使いやすいように
			if (pInf == NULL)			continue;		// 絶対に起こらないはずだが念のため		

														// 対象？
			if (0 <= memcmp(cKizukenNo, pInf->cKizukenNo, SIZE_KIZUKEN_NO)) {

				// 強調OFF通知
				OpSendFunc::SendOp_PatoReset(pInf->cKizukenNo, pInf->nMen, pInf->nKizuSeq);
				LOGA(em_WAR), "[%s] パトライト対象疵 警報解除。破棄[%s,表裏=%d,疵連番=%d, %d]", my_sThreadName, pInf->cKizukenNo, pInf->nMen, pInf->nKizuSeq, pInf->nY );
				LOGG(em_WAR), "[%s] パトライト対象疵 警報解除。破棄[%s,表裏=%d,疵連番=%d, %d]", my_sThreadName, pInf->cKizukenNo, pInf->nMen, pInf->nKizuSeq, pInf->nY );

				// 要素削除		
				SAFE_DELETE(pInf);						// 領域開放
				itr = mlst_AlmBuf[ii].erase(itr);		// 要素削除
				continue;
			}
			itr++;
		}

		//// リセット忘れ
		// 異常ガイダンス解除
		if (0 != mcls_pStatus->GetStatusRead().PatoNg[ii].stat) {
			int id = mcls_pStatus->GetStatusRead().PatoNg[ii].id;
			mcls_pStatus->SetStatusQue(id, true, false);
		}

	}
}

//------------------------------------------
// リセットPB押下 (←MI)
//------------------------------------------
void AlarmeManager::Reset_Pato()
{
	AutoLock autolock(&m_csLock);

	// メイン制御が処理中の場合は、待ち
	mcls_pLock->BlockTh();

	//// データ有無チェック
	int nCnt = 0;
	for (int ii = 0; ii<NUM_MEN; ii++) nCnt += (int)mlst_AlmBuf[ii].size();
	if (0 == nCnt) return;


	//// 最古を特定
	// 疵位置が最も若いやつ
	bool bOk = false;											// 初期セット完了済みフラグ
	std::list<PATO_INF*>::iterator		itr;					// イテレータ
	std::list<PATO_INF*>::iterator		itrOld;					// 最古情報

	for (int ii = 0; ii<NUM_MEN; ii++) {
		for (itr = mlst_AlmBuf[ii].begin(); itr != mlst_AlmBuf[ii].end(); itr++) {
			if ((*itr) == NULL)continue;						// 絶対に起こらないはずだが念のため
			if (!(*itr)->bRun) continue;						// パトライト出力中疵のみ 
			bool bChange = false;

			if (!bOk) {
				bChange = true;
			}
			else {
				// より古い？
				int nAns = memcmp((*itrOld)->cKizukenNo, (*itr)->cKizukenNo, SIZE_KIZUKEN_NO);
				if ((0 < nAns) ||
					(0 == nAns && (*itrOld)->nY > (*itr)->nY)) {
					bChange = true;
				}
			}

			// 入れ替え
			if (bChange) {
				bOk = true;
				itrOld = itr;
			}
		}
	}

	//// 最古の疵に対して処理
	if (bOk) {
		// 削除通知 // ← ヤメ。疵画像は残すため、キャンセルを送る
		OpSendFunc::SendOp_PatoReset((*itrOld)->cKizukenNo, (*itrOld)->nMen, (*itrOld)->nKizuSeq);
#ifdef DEBUG_PATO_LOG
		LOGA(em_MSG), "[%s] ﾊﾟﾄﾗｲﾄ疵削除 (%s)(men=%d)(KizuSeq=%d)(Y=%d)", my_sThreadName, (*itrOld)->cKizukenNo, (*itrOld)->nMen, (*itrOld)->nKizuSeq, (*itrOld)->nY  );
		LOGG(em_MSG), "[%s] ﾊﾟﾄﾗｲﾄ疵削除 (%s)(men=%d)(KizuSeq=%d)(Y=%d)", my_sThreadName, (*itrOld)->cKizukenNo, (*itrOld)->nMen, (*itrOld)->nKizuSeq, (*itrOld)->nY  );
#endif

		// 要素削除
		int nMen = (*itrOld)->nMen;
		SAFE_DELETE(*itrOld);						// 領域開放
		mlst_AlmBuf[nMen].erase(itrOld);

		// 異常ガイダンス解除
		if (0 != mcls_pStatus->GetStatusRead().PatoNg[nMen].stat) {
			int id = mcls_pStatus->GetStatusRead().PatoNg[nMen].id;
			mcls_pStatus->SetStatusQue(id, true, false);
#ifdef DEBUG_PATO_LOG
			LOGA(em_MSG), "[%s] ﾊﾟﾄﾗｲﾄ疵追加キューフル解除", my_sThreadName );
			LOGG(em_MSG), "[%s] ﾊﾟﾄﾗｲﾄ疵追加キューフル解除", my_sThreadName );
#endif
		}
	}
}
//
////------------------------------------------
//// パトライト出力のチェック (← DMemManager)
//// char const* cKizukenNo 表面検査台 通板中のコイル
//// int rec 1m単位のレコード位置
//// 復帰情報 パトライト出力対象個数
////------------------------------------------
//int AlarmeManager::OutPut_Pato(char const* cKizukenNo, int rec)
//{
//	AutoLock autolock(&m_csLock);
//
//	int nCnt = 0;								// パトライト出力中件数
//	//	int rec  = y / OUTPUT_MESYU_VAL_ROW;		// 1m間隔のレコード (警報出力用の内部メッシュの配列位置)
//
//
//	// パトライト出力
//	std::list<PATO_INF*>::iterator		itr;					// イテレータ
//	for (int ii = 0; ii<NUM_MEN; ii++) {
//		for (itr = mlst_AlmBuf[ii].begin(); itr != mlst_AlmBuf[ii].end(); itr++) {
//			PATO_INF* pInf = *itr;						// 使いやすいように
//
//// より古い？
//			int nAns = memcmp(cKizukenNo, pInf->cKizukenNo, SIZE_KIZUKEN_NO);
//			if ((0 < nAns) ||
//				(0 == nAns && rec >= pInf->rec)) {
//				nCnt++;
//				pInf->bRun = true;
//			}
//		}
//	}
//
//	return nCnt;
//}



//------------------------------------------
// 設備異常ボイス出力チェック
//------------------------------------------
void AlarmeManager::Check_VoiceErr()
{
	int		no = em_VOICE_ERR_NON;		// 設備異常No
	int		ii;

	// 機器状態取得
	TYP_STATUS_READ			typStatus;
	TYP_STATUS_NOWSYSTEM	typNowSys;
	memcpy(&typStatus, (TYP_STATUS_READ*)&mcls_pStatus->GetStatusRead(),	sizeof(TYP_STATUS_READ));
	memcpy(&typNowSys, (TYP_STATUS_NOWSYSTEM*)&mcls_pStatus->GetStatusNowSystem(), sizeof(TYP_STATUS_NOWSYSTEM));


	////// 設備異常状態チェック
	// ・より設備異常が高い項目が発生した場合は、入れ替える
	// ・異常解除になるまで出しっぱなし
	
	// ボイス出力2 コイル情報が受信できません。
	if(em_VOICE_ERR_NON == no) {
		if( 0 != typStatus.Coil_Ken.stat ||
			0 != typStatus.Coil_Dsp.stat   ) {
			no = em_VOICE_ERR_RECV_COILINF;
		}	
	}
	
	// ボイス出力3 疵検実績送信異常です。
	if(em_VOICE_ERR_NON == no) {
		if( 0 != typStatus.ZiProCreateErr.stat ||
			0 != typStatus.ZiFfCreateErr.stat   ) {
			no = em_VOICE_ERR_SEND_ZISEKI;
		}	
	}

	// ボイス出力4 照明装置異常です。
	if(em_VOICE_ERR_NON == no) {
		for(ii=0; ii<NUM_MEN; ii++) {
			if (0 != typStatus.LedRunOff[ii].stat) {
				no = em_VOICE_ERR_LIGHT;
				break;
			}
			for(int jj=0; jj<NUM_LIGHT; jj++){
				if (0 != typStatus.led.Connect[ii][jj].stat) {
					no = em_VOICE_ERR_LIGHT;
					break;
				}
			}
			for(int jj=0; jj<DIV_LED_COLOR_END; jj++){
				if (0 != typStatus.led.ch.Break[ii][jj].stat ||
					0 != typStatus.led.ch.TempWar[ii][jj].stat ||
					0 != typStatus.led.ch.TempErr[ii][jj].stat ) {
					no = em_VOICE_ERR_LIGHT;
					break;
				}
			}
		}
	}
	
	// ボイス出力5 カメラ異常です。
	if(em_VOICE_ERR_NON == no) {
		for(ii=0; ii<NUM_CAMERA; ii++) {
			if( 0 != typStatus.CamGrab[ii].stat) {
				no = em_VOICE_ERR_CAM;
				break;
			}
		}
	}

	// ボイス出力6 温度異常です。
	if(em_VOICE_ERR_NON == no) {
		// 判定
		for(ii=0; ii<NUM_ALL_HANTEI; ii++) {
			if( 0 != typStatus.EvtLog_Ht[ii].stat ) {
				no = em_VOICE_ERR_TEMP;
				break;
			}
		}
		
		// その他PC
		if (0 != typStatus.To.EvtLog.stat ||
			0 != typStatus.Bk.EvtLog.stat ||
			0 != typStatus.Rpe[0].EvtLog.stat ||
			0 != typStatus.Rpe[1].EvtLog.stat ||
			0 != typStatus.Rpe[2].EvtLog.stat ||
			0 != typStatus.Hy[0].EvtLog.stat ||
			0 != typStatus.Hy[1].EvtLog.stat ||
			0 != typStatus.So.EvtLog.stat) {
			no = em_VOICE_ERR_TEMP;
		}	

		// 画像処理盤等の温度異常時
		for (int num=0; num<NUM_TEMPMT_UNIT; num++)
		{
			if (0 != typStatus.tempmt[0].unit.TempSt[num].stat)
			{
				no = em_VOICE_ERR_TEMP;
				break;
			}
		}
	}

	// ボイス出力1 疵検異常です。 (一番優先度低い)
	if(em_VOICE_ERR_NON == no) {
		for(ii=0; ii<NUM_MEN; ii++) {
			if( DIV_KIKI_JYU == typNowSys.kiki[ii] ) {
				no = em_VOICE_ERR_KEN;		// その他重故障
				break;
			}
		}
	}

	////// 設備異常ボイス出力キャンセル 確認
	if(em_VOICE_ERR_NON == no) {
		// 設備異常ボイス出力キャンセル を 無効 状態
		if(m_bVoicErrCancel) {
			LOGA(em_MSG), "[%s] 設備異常ボイス出力キャンセル 無効", my_sThreadName);
			LOGG(em_MSG), "[%s] 設備異常ボイス出力キャンセル 無効", my_sThreadName);
			m_bVoicErrCancel = false;
		}
		// 出力ヤメ
		if(em_VOICE_ERR_NON != m_nVoicErrNo){
			LOGA(em_INF), "[%s] 設備異常ボイス変更[%s→%s]", my_sThreadName, GetVoiceTypeName(m_nVoicErrNo), GetVoiceTypeName(no));
			LOGG(em_INF), "[%s] 設備異常ボイス変更[%s→%s]", my_sThreadName, GetVoiceTypeName(m_nVoicErrNo), GetVoiceTypeName(no));
			m_nVoicErrNo = em_VOICE_ERR_NON;
		}

	} else if(em_VOICE_ERR_NON != no && m_bVoicErrCancel ) {
		// 設備異常ボイス出力キャンセルによりボイス出力をヤメ
		if(em_VOICE_ERR_NON != m_nVoicErrNo) m_nVoicErrNo = em_VOICE_ERR_NON;

	} else if(no != m_nVoicErrNo) {
		LOGA(em_INF), "[%s] 設備異常ボイス変更[%s→%s]", my_sThreadName, GetVoiceTypeName(m_nVoicErrNo), GetVoiceTypeName(no));
		LOGG(em_INF), "[%s] 設備異常ボイス変更[%s→%s]", my_sThreadName, GetVoiceTypeName(m_nVoicErrNo), GetVoiceTypeName(no));
		m_nVoicErrNo = no;			// ボイス出力変更
	}
}


//------------------------------------------
// 疵接近判定
//------------------------------------------
void AlarmeManager::Check_DetectSekkin()
{
	//// 検査停止中は警報出力なし
	if( ! mcls_pStatus->IsRun() ) {
		//memset(m_nVoicFastSet, 0x00, sizeof(m_nVoicFastSet));
		m_nVoicDetectNo = 0;
		m_nVoicDetectTorB = 0;
		m_nPato = 0;
		return;
	}

	//// 表面検査台位置を通板中コイルが対象
	const COIL_INF * pCoil = mcls_pCoil->GetCoilDsp(0);
	if( NULL == pCoil ) {
		//memset(m_nVoicFastSet, 0x00, sizeof(m_nVoicFastSet));
		m_nVoicDetectNo = 0;
		m_nVoicDetectTorB = 0;
		m_nPato = 0;
		return;
	}

	//// ///////////////////////////////////////////////////////////////////
	//// パトライト状態把握
	//int pato = 0;
	//COIL_INF const* pCoilDspO = mcls_pCoil->GetCoilDsp(0);
	//if(NULL != pCoilDspO) {
	//	if( ! pCoilDspO->bAlarmCancel ) {
	//		pato = OutPut_Pato(pCoilDspO->cKizukenNo, (int)(mcls_pPlg->GetLen_Dsp(0)/ OUTPUT_MESYU_VOIC_ROW));
	//	}
	//}
	//m_nPato = pato;
	
	
	// ///////////////////////////////////////////////////////////////////
	// ボイス状態把握

	//// 今 別の欠陥検出警報出力中はセットしない
	if(0 != m_tId_Alarme) {
		//memset(m_nVoicFastSet, 0x00, sizeof(m_nVoicFastSet));
		return;
	}

	////// そもそも 即時出力対象有り？
	//if( em_VOICE_PRI_NON == m_nVoicFastSet[0] && em_VOICE_PRI_NON == m_nVoicFastSet[1] ) {
	//	m_nVoicDetectNo = 0;
	//	m_nVoicDetectTorB = 0;	
	//	return;
	//}

	//// 欠陥検出警報解除中？
	if( pCoil->bAlarmCancel ) {
		//memset(m_nVoicFastSet, 0x00, sizeof(m_nVoicFastSet));
		m_nVoicDetectNo = 0;
		m_nVoicDetectTorB = 0;
		return;
	}

	//// 溶接点直後の不感帯？
	const ParamManager::P_COMMON_MAST& prm = mcls_pParam->GetCommonMast();
	if( prm.nNoLamp[0] > (int)(mcls_pPlg->GetLen_Dsp(0)) ) {
		//memset(m_nVoicFastSet, 0x00, sizeof(m_nVoicFastSet));
		m_nVoicDetectNo = 0;
		m_nVoicDetectTorB = 0;
		return;
	}

	////// ここまできたら、周期疵 即時ボイス出力対象
	//m_nVoicDetectNo   = max(m_nVoicFastSet[0], m_nVoicFastSet[1]);
	//if(m_nVoicFastSet[0] ) {
	//	m_nVoicDetectTorB = 0;
	//} else if(m_nVoicFastSet[1] ) {
	//	m_nVoicDetectTorB = 1;
	//}
	//// ここまできたら、周期疵 即時ボイス出力対象
	m_nVoicDetectNo = 1;
	m_nVoicDetectTorB = 1;

	//// 一回取り出したら 初期化
	//memset(m_nVoicFastSet, 0x00, sizeof(m_nVoicFastSet));
}
